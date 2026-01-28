#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.h"
#include "utils.h"

#define SCHEMA_DIR "data/"
#define SCHEMA_EXT ".meta"

/* ===============================
   Build schema file path
   =============================== */

static void schema_path(const char *table_name, char *path) {
    sprintf(path, "%s%s%s", SCHEMA_DIR, table_name, SCHEMA_EXT);
}

/* ===============================
   Check if table exists
   =============================== */

int catalog_table_exists(const char *table_name) {
    char path[128];
    schema_path(table_name, path);
    return file_exists(path);
}

/* ===============================
   Save schema to disk
   =============================== */

int catalog_save_schema(const TableSchema *schema) {
    char path[128];
    schema_path(schema->table_name, path);

    if (file_exists(path)) {
        db_error("Table already exists");
        return 0;
    }

    FILE *fp = fopen(path, "w");
    if (!fp) {
        db_error("Cannot create schema file");
        return 0;
    }

    fprintf(fp, "%d\n", schema->column_count);

    for (int i = 0; i < schema->column_count; i++) {
        fprintf(fp, "%s %d %d\n",
                schema->columns[i].name,
                schema->columns[i].type,
                schema->columns[i].is_primary_key);
    }

    fclose(fp);
    return 1;
}

/* ===============================
   Load schema from disk
   =============================== */

int catalog_load_schema(const char *table_name, TableSchema *schema) {
    char path[128];
    schema_path(table_name, path);

    if (!file_exists(path)) {
        db_error("Table does not exist");
        return 0;
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        db_error("Cannot open schema file");
        return 0;
    }

    strcpy(schema->table_name, table_name);
    fscanf(fp, "%d\n", &schema->column_count);

    for (int i = 0; i < schema->column_count; i++) {
        fscanf(fp, "%s %d %d\n",
               schema->columns[i].name,
               (int *)&schema->columns[i].type,
               &schema->columns[i].is_primary_key);
    }

    fclose(fp);
    return 1;
}
