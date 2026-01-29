%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/db.h"
#include "../include/executor.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int yylineno;

void yyerror(const char *s);
int should_exit = 0;
%}

%union {
    int num;
    char *str;
    Column *col;
    Column *col_list;
    Value *val;
    Value *val_list;
}

%token CREATE TABLE INSERT INTO SELECT FROM VALUES
%token INT_TYPE TEXT_TYPE EXIT SHOW TABLES
%token LPAREN RPAREN COMMA SEMICOLON ASTERISK

%token <num> NUMBER
%token <str> IDENTIFIER STRING

%type <col_list> column_defs column_def
%type <val_list> value_list value

%start statement_list

%%

statement_list:
    /* empty */
    | statement_list statement
    ;

statement:
    create_table_stmt
    | insert_stmt
    | select_stmt
    | show_tables_stmt
    | exit_stmt
    | error SEMICOLON { yyerrok; }
    ;

exit_stmt:
    EXIT SEMICOLON {
        should_exit = 1;
        printf("Goodbye!\n");
    }
    ;

show_tables_stmt:
    SHOW TABLES SEMICOLON {
        db_list_tables(global_db);
    }
    ;

create_table_stmt:
    CREATE TABLE IDENTIFIER LPAREN column_defs RPAREN SEMICOLON {
        CreateTableData *data = (CreateTableData*)malloc(sizeof(CreateTableData));
        strncpy(data->table_name, $3, MAX_NAME_LEN - 1);
        data->table_name[MAX_NAME_LEN - 1] = '\0';
        data->columns = $5;
        data->col_count = 0;
        
        /* Count columns */
        Column *temp = $5;
        while (temp && data->col_count < MAX_COLUMNS) {
            data->col_count++;
            temp++;
            if (strlen(temp->name) == 0) break;
        }
        
        ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_CREATE_TABLE;
        node->data = data;
        execute_ast(node);
        
        free($3);
        free(data->columns);
        free(data);
        free(node);
    }
    ;

column_defs:
    column_def {
        $$ = (Column*)calloc(MAX_COLUMNS, sizeof(Column));
        $$[0] = *$1;
        free($1);
    }
    | column_defs COMMA column_def {
        int count = 0;
        while (count < MAX_COLUMNS && strlen($1[count].name) > 0) {
            count++;
        }
        $1[count] = *$3;
        $$ = $1;
        free($3);
    }
    ;

column_def:
    IDENTIFIER INT_TYPE {
        Column *col = (Column*)malloc(sizeof(Column));
        strncpy(col->name, $1, MAX_NAME_LEN - 1);
        col->name[MAX_NAME_LEN - 1] = '\0';
        col->type = TYPE_INT;
        $$ = col;
        free($1);
    }
    | IDENTIFIER TEXT_TYPE {
        Column *col = (Column*)malloc(sizeof(Column));
        strncpy(col->name, $1, MAX_NAME_LEN - 1);
        col->name[MAX_NAME_LEN - 1] = '\0';
        col->type = TYPE_TEXT;
        $$ = col;
        free($1);
    }
    ;

insert_stmt:
    INSERT INTO IDENTIFIER VALUES LPAREN value_list RPAREN SEMICOLON {
        InsertData *data = (InsertData*)malloc(sizeof(InsertData));
        strncpy(data->table_name, $3, MAX_NAME_LEN - 1);
        data->table_name[MAX_NAME_LEN - 1] = '\0';
        data->values = $6;
        data->value_count = 0;
        
        /* Count values */
        Value *temp = $6;
        while (data->value_count < MAX_COLUMNS) {
            data->value_count++;
            temp++;
            /* Simple heuristic: if both int is 0 and str is empty, stop */
            if (temp->int_val == 0 && strlen(temp->str_val) == 0) break;
        }
        
        ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_INSERT;
        node->data = data;
        execute_ast(node);
        
        free($3);
        free(data->values);
        free(data);
        free(node);
    }
    ;

value_list:
    value {
        $$ = (Value*)calloc(MAX_COLUMNS, sizeof(Value));
        $$[0] = *$1;
        free($1);
    }
    | value_list COMMA value {
        int count = 0;
        while (count < MAX_COLUMNS) {
            /* Check if this slot is empty */
            if ($1[count].int_val == 0 && strlen($1[count].str_val) == 0) {
                break;
            }
            count++;
        }
        $1[count] = *$3;
        $$ = $1;
        free($3);
    }
    ;

value:
    NUMBER {
        Value *val = (Value*)calloc(1, sizeof(Value));
        val->int_val = $1;
        $$ = val;
    }
    | STRING {
        Value *val = (Value*)calloc(1, sizeof(Value));
        strncpy(val->str_val, $1, MAX_STRING_LEN - 1);
        val->str_val[MAX_STRING_LEN - 1] = '\0';
        $$ = val;
        free($1);
    }
    ;

select_stmt:
    SELECT ASTERISK FROM IDENTIFIER SEMICOLON {
        SelectData *data = (SelectData*)malloc(sizeof(SelectData));
        strncpy(data->table_name, $4, MAX_NAME_LEN - 1);
        data->table_name[MAX_NAME_LEN - 1] = '\0';
        data->select_all = 1;
        
        ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_SELECT;
        node->data = data;
        execute_ast(node);
        
        free($4);
        free(data);
        free(node);
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d: %s\n", yylineno, s);
}
