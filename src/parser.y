%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "semantic.h"
#include "executor.h"

extern int yylex();
void yyerror(const char *s);

ASTNode *root;
%}

/* ===============================
   Value Types
   =============================== */

%union {
    char *str;
    ASTNode *node;
}

/* ===============================
   Tokens
   =============================== */

%token CREATE TABLE INSERT INTO VALUES SELECT FROM DELETE
%token INT_T STRING_T
%token BEGIN_T COMMIT ROLLBACK EXIT_T
%token <str> IDENTIFIER NUMBER STRING

%type <node> statement create_stmt insert_stmt select_stmt delete_stmt

%%

/* ===============================
   Grammar Rules
   =============================== */

input:
    | input statement ';'
    ;

statement:
      create_stmt   { root = $1; }
    | insert_stmt   { root = $1; }
    | select_stmt   { root = $1; }
    | delete_stmt   { root = $1; }
    | EXIT_T        {
                        root = ast_create_node(AST_EXIT);
                        execute(root);
                        exit(0);
                    }
    ;

/* ===============================
   CREATE TABLE
   =============================== */

create_stmt:
    CREATE TABLE IDENTIFIER '(' column_defs ')'
    {
        $$ = ast_create_node(AST_CREATE_TABLE);
        strcpy($$->schema.table_name, $3);
    }
    ;

column_defs:
      column_def
    | column_defs ',' column_def
    ;

column_def:
    IDENTIFIER INT_T
    {
        int idx = root->schema.column_count++;
        strcpy(root->schema.columns[idx].name, $1);
        root->schema.columns[idx].type = TYPE_INT;
    }
  | IDENTIFIER STRING_T
    {
        int idx = root->schema.column_count++;
        strcpy(root->schema.columns[idx].name, $1);
        root->schema.columns[idx].type = TYPE_STRING;
    }
    ;

/* ===============================
   INSERT
   =============================== */

insert_stmt:
    INSERT INTO IDENTIFIER VALUES '(' value_list ')'
    {
        $$ = ast_create_node(AST_INSERT);
        strcpy($$->table_name, $3);
    }
    ;

value_list:
      value
    | value_list ',' value
    ;

value:
      NUMBER
    {
        int idx = root->record.values[0][0] != '\0'
                    ? strlen(root->record.values[0])
                    : 0;
        strcpy(root->record.values[idx], $1);
    }
    | STRING
    {
        int idx = root->record.values[0][0] != '\0'
                    ? strlen(root->record.values[0])
                    : 0;
        strcpy(root->record.values[idx], $1);
    }
    ;

/* ===============================
   SELECT
   =============================== */

select_stmt:
    SELECT '*' FROM IDENTIFIER
    {
        $$ = ast_create_node(AST_SELECT);
        strcpy($$->table_name, $4);
    }
    ;

/* ===============================
   DELETE
   =============================== */

delete_stmt:
    DELETE FROM IDENTIFIER
    {
        $$ = ast_create_node(AST_DELETE);
        strcpy($$->table_name, $3);
    }
    ;

%%

/* ===============================
   Error Handler
   =============================== */

void yyerror(const char *s) {
    printf("Syntax error: %s\n", s);
}
