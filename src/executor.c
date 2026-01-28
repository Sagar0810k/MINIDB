#include <stdio.h>
#include <string.h>

#include "executor.h"
#include "catalog.h"
#include "storage.h"
#include "utils.h"

/* ===============================
   Execute query
   =============================== */

int execute(ASTNode *node) {
    TableSchema schema;

    switch (node->type) {

    case AST_CREATE_TABLE:
        if (catalog_save_schema(&node->schema)) {
            printf("Table '%s' created successfully.\n",
                   node->schema.table_name);
        }
        return 1;

    case AST_INSERT:
        if (!catalog_load_schema(node->table_name, &schema))
            return 0;

        if (storage_insert(node->table_name,
                           &node->record,
                           schema.column_count)) {
            printf("1 row inserted.\n");
        }
        return 1;

    case AST_SELECT:
        printf("---- RESULT ----\n");
        catalog_load_schema(node->table_name, &schema);
        storage_select_all(node->table_name, schema.column_count);
        return 1;

    case AST_DELETE:
        if (storage_delete_all(node->table_name)) {
            printf("All records deleted from '%s'.\n",
                   node->table_name);
        }
        return 1;

    case AST_EXIT:
        printf("Bye 👋\n");
        return 0;

    default:
        db_error("Execution not supported");
        return 1;
    }
}
