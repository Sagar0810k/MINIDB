#include "../include/db.h"
#include "../include/storage.h"

Database *global_db = NULL;

/* Initialize database */
Database* db_init(void) {
    Database *db = (Database*)malloc(sizeof(Database));
    if (!db) {
        print_error("Memory allocation failed for database");
        return NULL;
    }
    
    db->table_count = 0;
    for (int i = 0; i < MAX_TABLES; i++) {
        db->tables[i] = NULL;
    }
    
    return db;
}

/* Free database memory */
void db_free(Database *db) {
    if (db) {
        for (int i = 0; i < db->table_count; i++) {
            table_free(db->tables[i]);
        }
        free(db);
    }
}

/* Add table to database */
int db_add_table(Database *db, Table *table) {
    if (!db || !table) {
        print_error("Invalid database or table");
        return -1;
    }
    
    if (db->table_count >= MAX_TABLES) {
        print_error("Database is full");
        return -1;
    }
    
    /* Check if table already exists */
    for (int i = 0; i < db->table_count; i++) {
        if (strcmp(db->tables[i]->name, table->name) == 0) {
            print_error("Table already exists");
            return -1;
        }
    }
    
    db->tables[db->table_count] = table;
    db->table_count++;
    
    /* Save to file */
    storage_save_table(table);
    
    return 0;
}

/* Get table by name */
Table* db_get_table(Database *db, const char *name) {
    if (!db || !name) {
        return NULL;
    }
    
    /* First check in memory */
    for (int i = 0; i < db->table_count; i++) {
        if (strcmp(db->tables[i]->name, name) == 0) {
            return db->tables[i];
        }
    }
    
    /* Try to load from file */
    Table *table = storage_load_table(name);
    if (table && db->table_count < MAX_TABLES) {
        db->tables[db->table_count] = table;
        db->table_count++;
        return table;
    }
    
    return NULL;
}

/* List all tables */
void db_list_tables(Database *db) {
    if (!db) {
        print_error("Database not initialized");
        return;
    }
    
    printf("\nTables in database:\n");
    printf("--------------------\n");
    for (int i = 0; i < db->table_count; i++) {
        printf("  %s\n", db->tables[i]->name);
    }
    printf("\n%d table(s) found\n\n", db->table_count);
}