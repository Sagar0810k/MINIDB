#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NAME_LEN    64
#define MAX_STRING_LEN  256
#define MAX_COLUMNS     32
#define MAX_ROWS        1000
#define MAX_CONDITIONS  16
#define MAX_SET_COLS    16
#define MAX_IN_VALUES   64

/* ── Data Types ─────────────────────────────────────────────────── */
typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_TEXT, TYPE_DATE, TYPE_BOOL } DataType;

/* ── Column flags ───────────────────────────────────────────────── */
#define COL_FLAG_PRIMARY_KEY  1
#define COL_FLAG_UNIQUE       2
#define COL_FLAG_NOT_NULL     4
#define COL_FLAG_HAS_DEFAULT  8

/* ── Column Definition ──────────────────────────────────────────── */
typedef struct {
    char     name[MAX_NAME_LEN];
    DataType type;
    int      flags;
    char     default_val[MAX_STRING_LEN]; /* stored as string, parsed on use */
} Column;

/* ── Value ──────────────────────────────────────────────────────── */
typedef struct {
    int    int_val;
    double float_val;
    char   str_val[MAX_STRING_LEN];
    int    is_null;   /* 1 = NULL */
    int    is_float;  /* 1 = use float_val */
} Value;

/* ── Row ────────────────────────────────────────────────────────── */
typedef struct {
    Value *values;
    int    col_count;
} Row;

/* ── Comparison Operators ───────────────────────────────────────── */
typedef enum {
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE,
    OP_LIKE, OP_IN, OP_BETWEEN, OP_IS_NULL, OP_IS_NOT_NULL
} CompOp;

/* ── Single Condition ───────────────────────────────────────────── */
typedef struct {
    char  col_name[MAX_NAME_LEN];
    CompOp op;
    Value  val;
    int    is_text;
    /* BETWEEN */
    Value  val2;
    /* IN list */
    Value  in_vals[MAX_IN_VALUES];
    int    in_count;
} SingleCond;

/* ── WHERE Condition (AND/OR chain) ─────────────────────────────── */
typedef struct {
    int        active;
    SingleCond conds[MAX_CONDITIONS];
    int        count;
    int        logic[MAX_CONDITIONS]; /* 0=AND 1=OR */
} Condition;

/* ── Utility Functions ──────────────────────────────────────────── */
char* trim_whitespace(char *str);
void  print_error(const char *msg);
void  print_success(const char *msg);
char* data_type_to_string(DataType type);
DataType string_to_data_type(const char *s);

/* String functions */
void str_upper(char *dst, const char *src);
void str_lower(char *dst, const char *src);
int  str_length(const char *s);
void str_concat(char *dst, const char *a, const char *b, size_t max);
int  like_match(const char *str, const char *pat); /* % wildcard */

#endif /* UTILS_H */
