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
    FILE *f=fopen(storage_get_filepath(table_name),"r");
    if(f){fclose(f);return 1;} return 0;
}

int storage_save_table(Table *table) {
    if(!table){print_error("Cannot save NULL table");return -1;}
    mkdir("data",0755);
    FILE *f=fopen(storage_get_filepath(table->name),"w");
    if(!f){print_error("Cannot open file for writing");return -1;}

    fprintf(f,"--- SCHEMA ---\n");
    fprintf(f,"TABLE: %s\n",table->name);
    fprintf(f,"COLUMNS: %d\n",table->col_count);
    for(int i=0;i<table->col_count;i++){
        fprintf(f,"%s|%s|%d|%s\n",
            table->columns[i].name,
            data_type_to_string(table->columns[i].type),
            table->columns[i].flags,
            table->columns[i].default_val);
    }
    fprintf(f,"--- DATA ---\n");
    for(int i=0;i<table->row_count;i++){
        for(int j=0;j<table->col_count;j++){
            Value *v=&table->rows[i].values[j];
            if(v->is_null) fprintf(f,"\\N");
            else if(table->columns[j].type==TYPE_FLOAT) fprintf(f,"%.10g",v->float_val);
            else if(table->columns[j].type==TYPE_INT||table->columns[j].type==TYPE_BOOL)
                fprintf(f,"%d",v->int_val);
            else fprintf(f,"%s",v->str_val);
            if(j<table->col_count-1) fprintf(f,"\x01"); /* ASCII SOH as delimiter */
        }
        fprintf(f,"\n");
    }
    fclose(f); return 0;
}

Table* storage_load_table(const char *table_name) {
    FILE *f=fopen(storage_get_filepath(table_name),"r");
    if(!f) return NULL;

    char line[4096], name[MAX_NAME_LEN];
    int col_count=0;
    Column columns[MAX_COLUMNS];
    int reading_schema=0,reading_data=0;

    while(fgets(line,sizeof(line),f)){
        line[strcspn(line,"\r\n")]='\0';
        if(strcmp(line,"--- SCHEMA ---")==0){reading_schema=1;continue;}
        if(strcmp(line,"--- DATA ---")==0){reading_schema=0;reading_data=1;break;}
        if(reading_schema){
            if(strncmp(line,"TABLE:",6)==0){sscanf(line,"TABLE: %63s",name);}
            else if(strncmp(line,"COLUMNS:",8)==0){/* skip */}
            else {
                /* format: name|type|flags|default */
                char cname[MAX_NAME_LEN],ctype[20],cdef[MAX_STRING_LEN];
                int cflags=0;
                char *tok=strtok(line,"|");
                if(!tok) continue;
                strncpy(cname,tok,MAX_NAME_LEN-1);
                tok=strtok(NULL,"|"); if(!tok) continue;
                strncpy(ctype,tok,19);
                tok=strtok(NULL,"|"); if(tok) cflags=atoi(tok);
                tok=strtok(NULL,"|"); if(tok) strncpy(cdef,tok,MAX_STRING_LEN-1); else cdef[0]='\0';
                strncpy(columns[col_count].name,cname,MAX_NAME_LEN-1);
                columns[col_count].type=string_to_data_type(ctype);
                columns[col_count].flags=cflags;
                strncpy(columns[col_count].default_val,cdef,MAX_STRING_LEN-1);
                col_count++;
            }
        }
    }

    Table *table=table_create(name,columns,col_count);
    if(!table){fclose(f);return NULL;}

    if(reading_data){
        while(fgets(line,sizeof(line),f)){
            line[strcspn(line,"\r\n")]='\0';
            if(strlen(line)==0) continue;
            Value values[MAX_COLUMNS];
            memset(values,0,sizeof(values));
            char *tok=strtok(line,"\x01");
            int ci=0;
            while(tok&&ci<col_count){
                if(strcmp(tok,"\\N")==0){
                    values[ci].is_null=1;
                } else {
                    values[ci].is_null=0;
                    if(columns[ci].type==TYPE_FLOAT){
                        values[ci].float_val=atof(tok); values[ci].is_float=1;
                    } else if(columns[ci].type==TYPE_INT||columns[ci].type==TYPE_BOOL){
                        values[ci].int_val=atoi(tok);
                    } else {
                        strncpy(values[ci].str_val,tok,MAX_STRING_LEN-1);
                    }
                }
                tok=strtok(NULL,"\x01"); ci++;
            }
            if(ci>0) table_insert_row(table,values);
        }
    }
    fclose(f); return table;
}
