#include "../include/storage.h"
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #define PATH_SEPARATOR "\\"
    #define mkdir(path,mode) _mkdir(path)
#else
    #define PATH_SEPARATOR "/"
#endif

char* storage_get_filepath(const char *table_name) {
    static char filepath[512];
    snprintf(filepath,sizeof(filepath),"data%s%s.tbl",PATH_SEPARATOR,table_name);
    return filepath;
}

int storage_table_exists(const char *table_name) {
    FILE *f = fopen(storage_get_filepath(table_name),"r");
    if (f) { fclose(f); return 1; }
    return 0;
}

int storage_save_table(Table *table) {
    if (!table) { print_error("Cannot save NULL table"); return -1; }
    mkdir("data",0755);
    FILE *f = fopen(storage_get_filepath(table->name),"w");
    if (!f) { print_error("Cannot open file for writing"); return -1; }

    /* Schema */
    fprintf(f,"--- SCHEMA ---\n");
    fprintf(f,"TABLE: %s\n",table->name);
    fprintf(f,"COLUMNS: %d\n",table->col_count);
    for (int i = 0; i < table->col_count; i++) {
        fprintf(f,"%s %s %d\n",
            table->columns[i].name,
            data_type_to_string(table->columns[i].type),
            table->columns[i].flags);
    }

    /* Data — use pipe | as delimiter to handle commas in text */
    fprintf(f,"--- DATA ---\n");
    for (int i = 0; i < table->row_count; i++) {
        for (int j = 0; j < table->col_count; j++) {
            if (table->columns[j].type == TYPE_INT)
                fprintf(f,"%d",table->rows[i].values[j].int_val);
            else
                fprintf(f,"%s",table->rows[i].values[j].str_val);
            if (j < table->col_count-1) fprintf(f,"|");
        }
        fprintf(f,"\n");
    }
    fclose(f);
    return 0;
}

Table* storage_load_table(const char *table_name) {
    FILE *f = fopen(storage_get_filepath(table_name),"r");
    if (!f) return NULL;

    char line[1024], name[MAX_NAME_LEN];
    int col_count = 0;
    Column columns[MAX_COLUMNS];
    int reading_schema = 0, reading_data = 0;

    while (fgets(line,sizeof(line),f)) {
        /* strip newline */
        line[strcspn(line,"\r\n")] = '\0';

        if (strcmp(line,"--- SCHEMA ---")==0) { reading_schema=1; continue; }
        if (strcmp(line,"--- DATA ---")==0)   { reading_schema=0; reading_data=1; break; }

        if (reading_schema) {
            if (strncmp(line,"TABLE:",6)==0) {
                sscanf(line,"TABLE: %63s",name);
            } else if (strncmp(line,"COLUMNS:",8)==0) {
                /* just skip — we count as we read */
            } else {
                char col_name[MAX_NAME_LEN], col_type[20];
                int flags = 0;
                int n = sscanf(line,"%63s %19s %d",col_name,col_type,&flags);
                if (n >= 2) {
                    strncpy(columns[col_count].name,col_name,MAX_NAME_LEN-1);
                    columns[col_count].type = (strcmp(col_type,"INT")==0) ? TYPE_INT : TYPE_TEXT;
                    columns[col_count].flags = (n==3) ? flags : 0;
                    col_count++;
                }
            }
        }
    }

    Table *table = table_create(name,columns,col_count);
    if (!table) { fclose(f); return NULL; }

    if (reading_data) {
        while (fgets(line,sizeof(line),f)) {
            line[strcspn(line,"\r\n")] = '\0';
            if (strlen(line)==0) continue;
            Value values[MAX_COLUMNS];
            char *tok = strtok(line,"|");
            int ci = 0;
            while (tok && ci < col_count) {
                if (table->columns[ci].type == TYPE_INT)
                    values[ci].int_val = atoi(tok);
                else {
                    strncpy(values[ci].str_val,tok,MAX_STRING_LEN-1);
                    values[ci].str_val[MAX_STRING_LEN-1]='\0';
                }
                tok = strtok(NULL,"|"); ci++;
            }
            if (ci > 0) table_insert_row(table,values);
        }
    }
    fclose(f);
    return table;
}
