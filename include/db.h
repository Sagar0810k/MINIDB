#ifndef DB_H
#define DB_H

#include "table.h"

#define MAX_TABLES 100

/* Database Structure */
typedef struct {
    Table *tables[MAX_TABLES];
    int table_count;
} Database;

/* Database Operations */
Database* db_init(void);
void db_free(Database *db);
int db_add_table(Database *db, Table *table);
Table* db_get_table(Database *db, const char *name);
void db_list_tables(Database *db);

/* Global Database Instance */
extern Database *global_db;

#endif /* DB_H */