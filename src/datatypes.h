#ifndef DATATYPES_H
#define DATATYPES_H

/* ===============================
   Global Limits
   =============================== */

#define MAX_TABLE_NAME 32
#define MAX_COLUMN_NAME 32
#define MAX_COLUMNS 16
#define MAX_VALUE_LENGTH 64

/* ===============================
   Supported Data Types
   =============================== */

typedef enum {
    TYPE_INT,
    TYPE_STRING
} DataType;

/* ===============================
   Column Definition
   =============================== */

typedef struct {
    char name[MAX_COLUMN_NAME];  // column name
    DataType type;               // INT or STRING
    int is_primary_key;          // 1 = primary key, 0 = not
} Column;

/* ===============================
   Table Schema Definition
   =============================== */

typedef struct {
    char table_name[MAX_TABLE_NAME];
    int column_count;
    Column columns[MAX_COLUMNS];
} TableSchema;

/* ===============================
   Record (Row) Representation
   =============================== */
/*
   Each record stores values as strings.
   Conversion to INT happens during semantic checks.
*/

typedef struct {
    char values[MAX_COLUMNS][MAX_VALUE_LENGTH];
} Record;

#endif /* DATATYPES_H */
