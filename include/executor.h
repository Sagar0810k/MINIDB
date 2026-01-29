#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "db.h"
#include "table.h"

/* AST Node Types */
typedef enum {
    AST_CREATE_TABLE,
    AST_INSERT,
    AST_SELECT
} ASTNodeType;

/* AST Node Structure */
typedef struct ASTNode {
    ASTNodeType type;
    void *data;
} ASTNode;

/* Create Table Data */
typedef struct {
    char table_name[MAX_NAME_LEN];
    Column *columns;
    int col_count;
} CreateTableData;

/* Insert Data */
typedef struct {
    char table_name[MAX_NAME_LEN];
    Value *values;
    int value_count;
} InsertData;

/* Select Data */
typedef struct {
    char table_name[MAX_NAME_LEN];
    int select_all;
} SelectData;

/* Executor Functions */
void execute_ast(ASTNode *node);
void execute_create_table(CreateTableData *data);
void execute_insert(InsertData *data);
void execute_select(SelectData *data);

#endif /* EXECUTOR_H */