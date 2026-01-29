#ifndef TABLE_H
#define TABLE_H

#include "utils.h"

/* Table Schema */
typedef struct {
    char name[MAX_NAME_LEN];
    Column columns[MAX_COLUMNS];
    int col_count;
    Row rows[MAX_ROWS];
    int row_count;
} Table;

/* Table Operations */
Table* table_create(const char *name, Column *columns, int col_count);
void table_free(Table *table);
int table_insert_row(Table *table, Value *values);
void table_print(Table *table);
void table_print_schema(Table *table);

#endif /* TABLE_H */