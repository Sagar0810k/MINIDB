#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "db.h"
#include "table.h"
#include "utils.h"

/* ── AST Node Types ─────────────────────────────────────────────── */
typedef enum {
    AST_CREATE_TABLE,
    AST_INSERT,
    AST_SELECT,
    AST_DELETE,
    AST_UPDATE,
    AST_JOIN,
    AST_ALTER_ADD,
    AST_ALTER_DROP,
    AST_DROP_TABLE,
    AST_RENAME_TABLE
} ASTNodeType;

typedef struct ASTNode { ASTNodeType type; void *data; } ASTNode;

/* ── SetPair: one col=val for multi-SET UPDATE ──────────────────── */
typedef struct {
    char  col[MAX_NAME_LEN];
    Value val;
    int   is_text;
} SetPair;

/* ── Structs ────────────────────────────────────────────────────── */

typedef struct {
    char    table_name[MAX_NAME_LEN];
    Column *columns;
    int     col_count;
} CreateTableData;

typedef struct {
    char   table_name[MAX_NAME_LEN];
    /* named-column insert */
    char   col_names[MAX_COLUMNS][MAX_NAME_LEN];
    int    named;         /* 1 = INSERT INTO t (cols) VALUES (...) */
    int    named_count;
    Value *values;
    int    value_count;
} InsertData;

/* Aggregate types */
#define AGG_NONE  0
#define AGG_COUNT 1
#define AGG_SUM   2
#define AGG_AVG   3
#define AGG_MIN   4
#define AGG_MAX   5

typedef struct {
    char table_name[MAX_NAME_LEN];
    int  select_all;
    char col_names[MAX_COLUMNS][MAX_NAME_LEN];
    int  col_count;
    Condition where;
    /* ORDER BY */
    char order_col[MAX_NAME_LEN];
    int  order_desc;
    int  has_order;
    /* LIMIT */
    int  limit;
    /* Aggregate */
    int  agg_type;
    char agg_col[MAX_NAME_LEN];
    /* GROUP BY */
    char group_col[MAX_NAME_LEN];
    int  has_group;
    /* HAVING (simple: agg_type op number) */
    int  has_having;
    CompOp having_op;
    double having_val;
    /* string function on one column */
    int  sfunc;   /* 0=none 1=UPPER 2=LOWER 3=LENGTH */
    char sfunc_col[MAX_NAME_LEN];
} SelectData;

typedef struct {
    char table_name[MAX_NAME_LEN];
    Condition where;
} DeleteData;

typedef struct {
    char    table_name[MAX_NAME_LEN];
    SetPair sets[MAX_SET_COLS];
    int     set_count;
    Condition where;
} UpdateData;

typedef struct {
    char table1[MAX_NAME_LEN];
    char table2[MAX_NAME_LEN];
    char col1[MAX_NAME_LEN];
    char col2[MAX_NAME_LEN];
    int  join_type; /* 0=INNER 1=LEFT 2=RIGHT 3=FULL */
    Condition where;
} JoinData;

typedef struct {
    char   table_name[MAX_NAME_LEN];
    Column new_col;
} AlterAddData;

typedef struct {
    char table_name[MAX_NAME_LEN];
    char col_name[MAX_NAME_LEN];
} AlterDropData;

typedef struct { char table_name[MAX_NAME_LEN]; } DropTableData;

typedef struct {
    char old_name[MAX_NAME_LEN];
    char new_name[MAX_NAME_LEN];
} RenameTableData;

/* ── Executor Functions ─────────────────────────────────────────── */
void execute_ast(ASTNode *node);
void execute_create_table(CreateTableData *data);
void execute_insert(InsertData *data);
void execute_select(SelectData *data);
void execute_delete(DeleteData *data);
void execute_update(UpdateData *data);
void execute_join(JoinData *data);
void execute_alter_add(AlterAddData *data);
void execute_alter_drop(AlterDropData *data);
void execute_drop_table(DropTableData *data);
void execute_rename_table(RenameTableData *data);

#endif /* EXECUTOR_H */
