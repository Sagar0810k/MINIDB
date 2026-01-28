#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "semantic.h"
#include "catalog.h"
#include "utils.h"

/* ===============================
   Helper: check integer
   =============================== */

static int is_integer(const char *str) {
    if (!str || *str == '\0') return 0;

    if (*str == '-' || *str == '+') str++;

    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

/* ===============================
   Semantic Analyzer Entry
   =============================== */

int semantic_check(ASTNode *node) {
    TableSchema schema;

    switch (node->type) {

    case AST_CREATE_TABLE:
        if (catalog_table_exists(node->schema.table_name)) {
            db_error("Table already exists");
            return 0;
        }
        return 1;

    case AST_INSERT:
        if (!catalog_load_schema(node->table_name, &schema)) {
            return 0;
        }

        if (schema.column_count <= 0) {
            db_error("Invalid table schema");
            return 0;
        }

        for (int i = 0; i < schema.column_count; i++) {
            if (schema.columns[i].type == TYPE_INT) {
                if (!is_integer(node->record.values[i])) {
                    db_error("Type mismatch: expected INT");
                    return 0;
                }
            }
        }
        return 1;

    case AST_SELECT:
    case AST_DELETE:
        if (!catalog_table_exists(node->table_name)) {
            db_error("Table does not exist");
            return 0;
        }
        return 1;

    case AST_BEGIN:
    case AST_COMMIT:
    case AST_ROLLBACK:
    case AST_EXIT:
        return 1;

    default:
        db_error("Unknown statement");
        return 0;
    }
}
