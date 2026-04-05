#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "db.h"
#include "table.h"
#include "utils.h"

/* AST Node Types */
typedef enum {
    AST_CREATE_TABLE,
    AST_INSERT,
    AST_SELECT,
    AST_DELETE,
    AST_UPDATE,
    AST_JOIN,
    AST_ALTER_ADD,
    AST_ALTER_DROP
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    void *data;
} ASTNode;

/* CREATE TABLE */
typedef struct {
    char    table_name[MAX_NAME_LEN];
    Column *columns;
    int     col_count;
} CreateTableData;

/* INSERT */
typedef struct {
    char   table_name[MAX_NAME_LEN];
    Value *values;
    int    value_count;
} InsertData;

/* SELECT */
typedef struct {
    char table_name[MAX_NAME_LEN];
    int  select_all;
    char col_names[MAX_COLUMNS][MAX_NAME_LEN];
    int  col_count;
    Condition where;
    /* ORDER BY */
    char order_col[MAX_NAME_LEN];
    int  order_desc;   /* 0=ASC 1=DESC */
    int  has_order;
    /* LIMIT */
    int  limit;        /* -1 = no limit */
    /* aggregates */
    int  agg_type;     /* 0=none 1=COUNT 2=SUM 3=AVG */
    char agg_col[MAX_NAME_LEN]; /* column for SUM/AVG, or * for COUNT */
} SelectData;

/* DELETE */
typedef struct {
    char table_name[MAX_NAME_LEN];
    Condition where;
} DeleteData;

/* UPDATE: SET one column */
typedef struct {
    char  table_name[MAX_NAME_LEN];
    char  set_col[MAX_NAME_LEN];
    Value set_val;
    int   set_is_text;
    Condition where;
} UpdateData;

/* JOIN */
typedef struct {
    char table1[MAX_NAME_LEN];
    char table2[MAX_NAME_LEN];
    char col1[MAX_NAME_LEN];
    char col2[MAX_NAME_LEN];
    Condition where;
} JoinData;

/* ALTER TABLE ADD COLUMN */
typedef struct {
    char   table_name[MAX_NAME_LEN];
    Column new_col;
} AlterAddData;

/* ALTER TABLE DROP COLUMN */
typedef struct {
    char table_name[MAX_NAME_LEN];
    char col_name[MAX_NAME_LEN];
} AlterDropData;

/* Executor Functions */
void execute_ast(ASTNode *node);
void execute_create_table(CreateTableData *data);
void execute_insert(InsertData *data);
void execute_select(SelectData *data);
void execute_delete(DeleteData *data);
void execute_update(UpdateData *data);
void execute_join(JoinData *data);
void execute_alter_add(AlterAddData *data);
void execute_alter_drop(AlterDropData *data);

#endif /* EXECUTOR_H */
