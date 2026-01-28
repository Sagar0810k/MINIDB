#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage.h"
#include "utils.h"

#define DATA_DIR "data/"
#define DATA_EXT ".data"

/* ===============================
   Build data file path
   =============================== */

static void data_path(const char *table_name, char *path) {
    sprintf(path, "%s%s%s", DATA_DIR, table_name, DATA_EXT);
}

/* ===============================
   Insert record
   =============================== */

int storage_insert(const char *table_name, const Record *record, int column_count) {
    char path[128];
    data_path(table_name, path);

    FILE *fp = fopen(path, "a");
    if (!fp) {
        db_error("Cannot open data file");
        return 0;
    }

    for (int i = 0; i < column_count; i++) {
        fprintf(fp, "%s", record->values[i]);
        if (i < column_count - 1)
            fprintf(fp, "|");
    }
    fprintf(fp, "\n");

    fclose(fp);
    return 1;
}

/* ===============================
   Select all records
   =============================== */

int storage_select_all(const char *table_name, int column_count) {
    char path[128];
    char line[512];

    data_path(table_name, path);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        db_error("No data found");
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }

    fclose(fp);
    return 1;
}

/* ===============================
   Delete all records
   =============================== */

int storage_delete_all(const char *table_name) {
    char path[128];
    data_path(table_name, path);

    if (remove(path) != 0) {
        db_error("Failed to delete data");
        return 0;
    }

    return 1;
}
