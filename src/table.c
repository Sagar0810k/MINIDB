#include "../include/table.h"
#include <string.h>

/* Create a new table */
Table* table_create(const char *name, Column *columns, int col_count) {
    Table *table = (Table*)malloc(sizeof(Table));
    if (!table) {
        print_error("Memory allocation failed for table");
        return NULL;
    }
    
    strncpy(table->name, name, MAX_NAME_LEN - 1);
    table->name[MAX_NAME_LEN - 1] = '\0';
    
    table->col_count = col_count;
    for (int i = 0; i < col_count; i++) {
        table->columns[i] = columns[i];
    }
    
    table->row_count = 0;
    
    return table;
}

/* Free table memory */
void table_free(Table *table) {
    if (table) {
        for (int i = 0; i < table->row_count; i++) {
            if (table->rows[i].values) {
                free(table->rows[i].values);
            }
        }
        free(table);
    }
}

/* Insert a row into table */
int table_insert_row(Table *table, Value *values) {
    if (table->row_count >= MAX_ROWS) {
        print_error("Table is full");
        return -1;
    }
    
    table->rows[table->row_count].values = (Value*)malloc(sizeof(Value) * table->col_count);
    if (!table->rows[table->row_count].values) {
        print_error("Memory allocation failed for row");
        return -1;
    }
    
    for (int i = 0; i < table->col_count; i++) {
        table->rows[table->row_count].values[i] = values[i];
    }
    
    table->rows[table->row_count].col_count = table->col_count;
    table->row_count++;
    
    return 0;
}

/* Print table contents */
void table_print(Table *table) {
    if (!table) {
        print_error("Table does not exist");
        return;
    }
    
    /* Print column headers */
    printf("\n");
    for (int i = 0; i < table->col_count; i++) {
        printf("%-15s", table->columns[i].name);
    }
    printf("\n");
    
    /* Print separator */
    for (int i = 0; i < table->col_count * 15; i++) {
        printf("-");
    }
    printf("\n");
    
    /* Print rows */
    for (int i = 0; i < table->row_count; i++) {
        for (int j = 0; j < table->col_count; j++) {
            if (table->columns[j].type == TYPE_INT) {
                printf("%-15d", table->rows[i].values[j].int_val);
            } else {
                printf("%-15s", table->rows[i].values[j].str_val);
            }
        }
        printf("\n");
    }
    
    printf("\n%d row(s) returned\n\n", table->row_count);
}

/* Print table schema */
void table_print_schema(Table *table) {
    printf("\nTable: %s\n", table->name);
    printf("Columns:\n");
    for (int i = 0; i < table->col_count; i++) {
        printf("  %s (%s)\n", table->columns[i].name, 
               data_type_to_string(table->columns[i].type));
    }
    printf("\n");
}