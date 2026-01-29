#ifndef STORAGE_H
#define STORAGE_H

#include "table.h"

/* Storage Operations */
int storage_save_table(Table *table);
Table* storage_load_table(const char *table_name);
int storage_table_exists(const char *table_name);
char* storage_get_filepath(const char *table_name);

#endif /* STORAGE_H */