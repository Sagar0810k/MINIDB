#ifndef STORAGE_H
#define STORAGE_H

#include "datatypes.h"

/* ===============================
   Storage Operations
   =============================== */

int storage_insert(const char *table_name, const Record *record, int column_count);

int storage_select_all(const char *table_name, int column_count);

int storage_delete_all(const char *table_name);

#endif /* STORAGE_H */
