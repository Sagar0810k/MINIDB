#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN    64
#define MAX_STRING_LEN  256
#define MAX_COLUMNS     32
#define MAX_ROWS        1000
#define MAX_CONDITIONS  8

/* Data Types */
typedef enum { TYPE_INT, TYPE_TEXT } DataType;

/* Column flags */
#define COL_FLAG_PRIMARY_KEY  1
#define COL_FLAG_UNIQUE       2
#define COL_FLAG_NOT_NULL     4

/* Column Definition */
typedef struct {
    char name[MAX_NAME_LEN];
    DataType type;
    int flags;          /* bitmask of COL_FLAG_* */
} Column;

/* Value */
typedef union {
    int  int_val;
    char str_val[MAX_STRING_LEN];
} Value;

/* Row */
typedef struct {
    Value *values;
    int    col_count;
} Row;

/* Comparison Operators */
typedef enum { OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE } CompOp;

/* Single condition: col op val */
typedef struct {
    char  col_name[MAX_NAME_LEN];
    CompOp op;
    Value  val;
    int    is_text;
} SingleCond;

/* WHERE: up to MAX_CONDITIONS joined by AND/OR */
typedef struct {
    int        active;
    SingleCond conds[MAX_CONDITIONS];
    int        count;
    int        logic[MAX_CONDITIONS]; /* 0=AND 1=OR between conds[i] and conds[i+1] */
} Condition;

/* Utility Functions */
char* trim_whitespace(char *str);
void  print_error(const char *msg);
void  print_success(const char *msg);
char* data_type_to_string(DataType type);

#endif /* UTILS_H */
