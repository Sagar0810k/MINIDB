#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 64
#define MAX_STRING_LEN 256
#define MAX_COLUMNS 32
#define MAX_ROWS 1000

/* Data Types */
typedef enum {
    TYPE_INT,
    TYPE_TEXT
} DataType;

/* Column Definition */
typedef struct {
    char name[MAX_NAME_LEN];
    DataType type;
} Column;

/* Value Union */
typedef union {
    int int_val;
    char str_val[MAX_STRING_LEN];
} Value;

/* Row Structure */
typedef struct {
    Value *values;
    int col_count;
} Row;

/* Utility Functions */
char* trim_whitespace(char *str);
void print_error(const char *msg);
void print_success(const char *msg);
char* data_type_to_string(DataType type);

#endif /* UTILS_H */