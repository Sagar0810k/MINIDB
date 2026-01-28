#ifndef AST_H
#define AST_H

#include "datatypes.h"

/* ===============================
   AST Node Types (SQL Commands)
   =============================== */

typedef enum {
    AST_CREATE_TABLE,
    AST_INSERT,
    AST_SELECT,
    AST_DELETE,
    AST_BEGIN,
    AST_COMMIT,
    AST_ROLLBACK,
    AST_EXIT
} ASTNodeType;

/* ===============================
   AST Node Structure
   =============================== */

typedef struct ASTNode {
    ASTNodeType type;               // type of SQL command
    char table_name[MAX_TABLE_NAME];

    /* Used by CREATE TABLE */
    TableSchema schema;

    /* Used by INSERT */
    Record record;

    /* For future extension (WHERE, JOIN, etc.) */
    struct ASTNode *next;
} ASTNode;

/* ===============================
   AST Utility Functions
   =============================== */

ASTNode *ast_create_node(ASTNodeType type);
void ast_free(ASTNode *node);

#endif /* AST_H */
