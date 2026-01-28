#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ===============================
   Create a new AST node
   =============================== */

ASTNode *ast_create_node(ASTNodeType type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    node->next = NULL;

    return node;
}

/* ===============================
   Free AST node
   =============================== */

void ast_free(ASTNode *node) {
    if (node) {
        free(node);
    }
}
