#include "../include/storage.h"
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>  /* For _mkdir on Windows */
    #define PATH_SEPARATOR "\\"
    #define mkdir(path, mode) _mkdir(path)
#else
    #define PATH_SEPARATOR "/"
#endif

/* Get file path for table */
char* storage_get_filepath(const char *table_name) {
    static char filepath[512];
    snprintf(filepath, sizeof(filepath), "data%s%s.tbl", PATH_SEPARATOR, table_name);
    return filepath;
}

/* Check if table file exists */
int storage_table_exists(const char *table_name) {
    char *filepath = storage_get_filepath(table_name);
    FILE *file = fopen(filepath, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

/* Save table to file */
int storage_save_table(Table *table) {
    if (!table) {
        print_error("Cannot save NULL table");
        return -1;
    }
    
    /* Ensure data directory exists */
    mkdir("data", 0755);
    
    char *filepath = storage_get_filepath(table->name);
    FILE *file = fopen(filepath, "w");
    if (!file) {
        print_error("Cannot open file for writing");
        return -1;
    }
    
    /* Write schema */
    fprintf(file, "--- SCHEMA ---\n");
    fprintf(file, "TABLE: %s\n", table->name);
    fprintf(file, "COLUMNS: %d\n", table->col_count);
    for (int i = 0; i < table->col_count; i++) {
        fprintf(file, "%s %s\n", table->columns[i].name, 
                data_type_to_string(table->columns[i].type));
    }
    
    /* Write rows */
    fprintf(file, "--- DATA ---\n");
    for (int i = 0; i < table->row_count; i++) {
        for (int j = 0; j < table->col_count; j++) {
            if (table->columns[j].type == TYPE_INT) {
                fprintf(file, "%d", table->rows[i].values[j].int_val);
            } else {
                fprintf(file, "%s", table->rows[i].values[j].str_val);
            }
            if (j < table->col_count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    return 0;
}

/* Load table from file */
Table* storage_load_table(const char *table_name) {
    char *filepath = storage_get_filepath(table_name);
    FILE *file = fopen(filepath, "r");
    if (!file) {
        return NULL;
    }
    
    char line[1024];
    char name[MAX_NAME_LEN];
    int col_count = 0;
    Column columns[MAX_COLUMNS];
    
    /* Read schema */
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "TABLE:", 6) == 0) {
            sscanf(line, "TABLE: %s", name);
        } else if (strncmp(line, "COLUMNS:", 8) == 0) {
            sscanf(line, "COLUMNS: %d", &col_count);
        } else if (strncmp(line, "---", 3) == 0) {
            if (strstr(line, "DATA")) {
                break; /* Start of data section */
            }
        } else {
            /* Column definition */
            char col_name[MAX_NAME_LEN];
            char col_type[20];
            if (sscanf(line, "%s %s", col_name, col_type) == 2) {
                strncpy(columns[col_count - (col_count)].name, col_name, MAX_NAME_LEN - 1);
                if (strcmp(col_type, "INT") == 0) {
                    columns[col_count - (col_count)].type = TYPE_INT;
                } else {
                    columns[col_count - (col_count)].type = TYPE_TEXT;
                }
                col_count++;
            }
        }
    }
    
    /* Create table */
    Table *table = table_create(name, columns, col_count);
    if (!table) {
        fclose(file);
        return NULL;
    }
    
    /* Read data rows */
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) <= 1) continue;
        
        Value values[MAX_COLUMNS];
        char *token = strtok(line, ",");
        int col_idx = 0;
        
        while (token && col_idx < col_count) {
            token = trim_whitespace(token);
            if (table->columns[col_idx].type == TYPE_INT) {
                values[col_idx].int_val = atoi(token);
            } else {
                strncpy(values[col_idx].str_val, token, MAX_STRING_LEN - 1);
                values[col_idx].str_val[MAX_STRING_LEN - 1] = '\0';
            }
            token = strtok(NULL, ",");
            col_idx++;
        }
        
        if (col_idx > 0) {
            table_insert_row(table, values);
        }
    }
    
    fclose(file);
    return table;
}