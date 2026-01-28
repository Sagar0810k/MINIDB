#ifndef CATALOG_H
#define CATALOG_H

#include "datatypes.h"

/* ===============================
   Catalog Operations
   =============================== */

int catalog_table_exists(const char *table_name);

int catalog_save_schema(const TableSchema *schema);

int catalog_load_schema(const char *table_name, TableSchema *schema);

#endif /* CATALOG_H */
