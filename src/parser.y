%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/db.h"
#include "../include/executor.h"

extern int yylex();
extern FILE *yyin;
extern int yylineno;
void yyerror(const char *s);
int should_exit = 0;

/* helpers to build Condition */
static void cond_init(Condition *c) { memset(c,0,sizeof(*c)); c->active=0; }
static void cond_add(Condition *c, SingleCond sc, int logic) {
    if (c->count >= MAX_CONDITIONS) return;
    if (c->count > 0) c->logic[c->count-1] = logic;
    c->conds[c->count++] = sc;
    c->active = 1;
}
%}

%union {
    int num;
    char *str;
    Column *col;
    Column *col_list;
    Value *val;
    Value *val_list;
    Condition cond;
    SingleCond scond;
}

%token CREATE TABLE INSERT INTO SELECT FROM VALUES
%token INT_TYPE TEXT_TYPE EXIT SHOW TABLES
%token WHERE AND OR DELETE UPDATE SET JOIN ON INNER
%token ORDER BY ASC DESC LIMIT
%token COUNT SUM AVG
%token PRIMARY KEY UNIQUE
%token ALTER ADD DROP COLUMN
%token LPAREN RPAREN COMMA SEMICOLON ASTERISK DOT
%token EQ NEQ LT GT LTE GTE

%token <num> NUMBER
%token <str> IDENTIFIER STRING

%type <col_list> column_defs
%type <col>      column_def col_constraint
%type <val_list> value_list col_name_list
%type <val>      value
%type <cond>     opt_where where_clause
%type <scond>    single_cond
%type <str>      col_ref agg_expr

%start statement_list
%%

statement_list:
    | statement_list statement
    ;

statement:
    create_table_stmt
    | insert_stmt
    | select_stmt
    | delete_stmt
    | update_stmt
    | alter_stmt
    | show_tables_stmt
    | exit_stmt
    | error SEMICOLON { yyerrok; }
    ;

exit_stmt: EXIT SEMICOLON { should_exit=1; printf("Goodbye!\n"); } ;
show_tables_stmt: SHOW TABLES SEMICOLON { db_list_tables(global_db); } ;

/* ── CREATE TABLE ───────────────────────────────────────────────── */
create_table_stmt:
    CREATE TABLE IDENTIFIER LPAREN column_defs RPAREN SEMICOLON {
        CreateTableData *d = calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        d->columns = $5; d->col_count=0;
        Column *t=$5; while(t&&d->col_count<MAX_COLUMNS&&strlen(t->name)>0){d->col_count++;t++;}
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_CREATE_TABLE; n->data=d;
        execute_ast(n);
        free($3); free(d->columns); free(d); free(n);
    }
    ;

column_defs:
    column_def {
        $$=calloc(MAX_COLUMNS,sizeof(Column)); $$[0]=*$1; free($1);
    }
    | column_defs COMMA column_def {
        int n=0; while(n<MAX_COLUMNS&&strlen($1[n].name)>0)n++;
        $1[n]=*$3; free($3); $$=$1;
    }
    ;

column_def:
    IDENTIFIER INT_TYPE col_constraint {
        Column *c=calloc(1,sizeof(*c));
        strncpy(c->name,$1,MAX_NAME_LEN-1); c->type=TYPE_INT;
        c->flags = $3 ? $3->flags : 0;
        if($3)free($3); free($1); $$=c;
    }
    | IDENTIFIER TEXT_TYPE col_constraint {
        Column *c=calloc(1,sizeof(*c));
        strncpy(c->name,$1,MAX_NAME_LEN-1); c->type=TYPE_TEXT;
        c->flags = $3 ? $3->flags : 0;
        if($3)free($3); free($1); $$=c;
    }
    ;

col_constraint:
    /* empty */        { $$=NULL; }
    | PRIMARY KEY      { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_PRIMARY_KEY|COL_FLAG_UNIQUE; $$=c; }
    | UNIQUE           { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_UNIQUE; $$=c; }
    ;

/* ── INSERT ─────────────────────────────────────────────────────── */
insert_stmt:
    INSERT INTO IDENTIFIER VALUES LPAREN value_list RPAREN SEMICOLON {
        InsertData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        d->values=$6; d->value_count=0;
        Value *t=$6;
        while(d->value_count<MAX_COLUMNS){
            d->value_count++; t++;
            if(t->int_val==0&&strlen(t->str_val)==0)break;
        }
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_INSERT; n->data=d;
        execute_ast(n);
        free($3); free(d->values); free(d); free(n);
    }
    ;

value_list:
    value { $$=calloc(MAX_COLUMNS,sizeof(Value)); $$[0]=*$1; free($1); }
    | value_list COMMA value {
        int n=0;
        while(n<MAX_COLUMNS&&!($1[n].int_val==0&&strlen($1[n].str_val)==0))n++;
        $1[n]=*$3; free($3); $$=$1;
    }
    ;

value:
    NUMBER { Value *v=calloc(1,sizeof(*v)); v->int_val=$1; $$=v; }
    | STRING { Value *v=calloc(1,sizeof(*v)); strncpy(v->str_val,$1,MAX_STRING_LEN-1); $$=v; free($1); }
    ;

/* ── SELECT ─────────────────────────────────────────────────────── */
select_stmt:
    /* SELECT * FROM t [WHERE] [ORDER BY] [LIMIT] */
    SELECT ASTERISK FROM IDENTIFIER opt_where opt_order opt_limit SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=1; d->where=$5;
        /* opt_order and opt_limit stored in globals then copied */
        extern char g_order_col[]; extern int g_order_desc; extern int g_has_order; extern int g_limit;
        strncpy(d->order_col,g_order_col,MAX_NAME_LEN-1);
        d->order_desc=g_order_desc; d->has_order=g_has_order; d->limit=g_limit;
        d->agg_type=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free(d); free(n);
    }
    /* SELECT col1,col2 FROM t [WHERE] [ORDER BY] [LIMIT] */
    | SELECT col_name_list FROM IDENTIFIER opt_where opt_order opt_limit SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=0; d->where=$5;
        extern char g_order_col[]; extern int g_order_desc; extern int g_has_order; extern int g_limit;
        strncpy(d->order_col,g_order_col,MAX_NAME_LEN-1);
        d->order_desc=g_order_desc; d->has_order=g_has_order; d->limit=g_limit;
        d->agg_type=0;
        int cnt=0;
        while(cnt<MAX_COLUMNS&&strlen($2[cnt].str_val)>0){
            strncpy(d->col_names[cnt],$2[cnt].str_val,MAX_NAME_LEN-1); cnt++;
        }
        d->col_count=cnt; free($2);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free(d); free(n);
    }
    /* SELECT COUNT/SUM/AVG(...) FROM t [WHERE] */
    | SELECT agg_expr FROM IDENTIFIER opt_where SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=1; d->where=$5; d->has_order=0; d->limit=-1;
        extern int g_agg_type; extern char g_agg_col[];
        d->agg_type=g_agg_type;
        strncpy(d->agg_col,g_agg_col,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free($2); free(d); free(n);
    }
    /* INNER JOIN */
    | SELECT ASTERISK FROM IDENTIFIER INNER JOIN IDENTIFIER ON col_ref EQ col_ref SEMICOLON {
        JoinData *d=calloc(1,sizeof(*d));
        strncpy(d->table1,$4,MAX_NAME_LEN-1); strncpy(d->table2,$7,MAX_NAME_LEN-1);
        char *dot1=strchr($9,'.'), *dot2=strchr($11,'.');
        strncpy(d->col1,dot1?dot1+1:$9, MAX_NAME_LEN-1);
        strncpy(d->col2,dot2?dot2+1:$11,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_JOIN; n->data=d;
        execute_ast(n); free($4); free($7); free($9); free($11); free(d); free(n);
    }
    ;

/* aggregate function expression — sets globals, returns dummy string */
agg_expr:
    COUNT LPAREN ASTERISK RPAREN {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=1; strcpy(g_agg_col,"*"); $$=strdup("COUNT(*)");
    }
    | COUNT LPAREN IDENTIFIER RPAREN {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=1; strncpy(g_agg_col,$3,MAX_NAME_LEN-1); $$=strdup($3); free($3);
    }
    | SUM LPAREN IDENTIFIER RPAREN {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=2; strncpy(g_agg_col,$3,MAX_NAME_LEN-1); $$=strdup($3); free($3);
    }
    | AVG LPAREN IDENTIFIER RPAREN {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=3; strncpy(g_agg_col,$3,MAX_NAME_LEN-1); $$=strdup($3); free($3);
    }
    ;

col_name_list:
    IDENTIFIER { $$=calloc(MAX_COLUMNS,sizeof(Value)); strncpy($$[0].str_val,$1,MAX_NAME_LEN-1); free($1); }
    | col_name_list COMMA IDENTIFIER {
        int n=0; while(n<MAX_COLUMNS&&strlen($1[n].str_val)>0)n++;
        strncpy($1[n].str_val,$3,MAX_NAME_LEN-1); free($3); $$=$1;
    }
    ;

col_ref:
    IDENTIFIER DOT IDENTIFIER {
        char *buf=malloc(MAX_NAME_LEN*2+2); snprintf(buf,MAX_NAME_LEN*2+1,"%s.%s",$1,$3);
        free($1); free($3); $$=buf;
    }
    | IDENTIFIER { $$=$1; }
    ;

/* ORDER BY stored in globals (simple approach) */
opt_order:
    /* empty */ { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                  g_order_col[0]='\0'; g_order_desc=0; g_has_order=0; }
    | ORDER BY IDENTIFIER     { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                                 strncpy(g_order_col,$3,MAX_NAME_LEN-1); g_order_desc=0; g_has_order=1; free($3); }
    | ORDER BY IDENTIFIER ASC { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                                 strncpy(g_order_col,$3,MAX_NAME_LEN-1); g_order_desc=0; g_has_order=1; free($3); }
    | ORDER BY IDENTIFIER DESC{ extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                                 strncpy(g_order_col,$3,MAX_NAME_LEN-1); g_order_desc=1; g_has_order=1; free($3); }
    ;

opt_limit:
    /* empty */ { extern int g_limit; g_limit=-1; }
    | LIMIT NUMBER { extern int g_limit; g_limit=$2; }
    ;

/* ── DELETE ─────────────────────────────────────────────────────── */
delete_stmt:
    DELETE FROM IDENTIFIER opt_where SEMICOLON {
        DeleteData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1); d->where=$4;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_DELETE; n->data=d;
        execute_ast(n); free($3); free(d); free(n);
    }
    ;

/* ── UPDATE ─────────────────────────────────────────────────────── */
update_stmt:
    UPDATE IDENTIFIER SET IDENTIFIER EQ value opt_where SEMICOLON {
        UpdateData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$2,MAX_NAME_LEN-1);
        strncpy(d->set_col,$4,MAX_NAME_LEN-1);
        d->set_val=*$6; d->set_is_text=(strlen($6->str_val)>0);
        d->where=$7;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_UPDATE; n->data=d;
        execute_ast(n); free($2); free($4); free($6); free(d); free(n);
    }
    ;

/* ── ALTER TABLE ────────────────────────────────────────────────── */
alter_stmt:
    ALTER TABLE IDENTIFIER ADD COLUMN IDENTIFIER INT_TYPE SEMICOLON {
        AlterAddData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        strncpy(d->new_col.name,$6,MAX_NAME_LEN-1); d->new_col.type=TYPE_INT; d->new_col.flags=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_ADD; n->data=d;
        execute_ast(n); free($3); free($6); free(d); free(n);
    }
    | ALTER TABLE IDENTIFIER ADD COLUMN IDENTIFIER TEXT_TYPE SEMICOLON {
        AlterAddData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        strncpy(d->new_col.name,$6,MAX_NAME_LEN-1); d->new_col.type=TYPE_TEXT; d->new_col.flags=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_ADD; n->data=d;
        execute_ast(n); free($3); free($6); free(d); free(n);
    }
    | ALTER TABLE IDENTIFIER DROP COLUMN IDENTIFIER SEMICOLON {
        AlterDropData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        strncpy(d->col_name,$6,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_DROP; n->data=d;
        execute_ast(n); free($3); free($6); free(d); free(n);
    }
    ;

/* ── WHERE (AND / OR chains) ────────────────────────────────────── */
opt_where:
    /* empty */ { cond_init(&$$); }
    | where_clause { $$=$1; }
    ;

where_clause:
    WHERE single_cond {
        cond_init(&$$); cond_add(&$$,$2,0);
    }
    | where_clause AND single_cond {
        $$=$1; cond_add(&$$,$3,0);
    }
    | where_clause OR single_cond {
        $$=$1; cond_add(&$$,$3,1);
    }
    ;

single_cond:
    IDENTIFIER EQ  NUMBER { memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_EQ;  $$.val.int_val=$3; $$.is_text=0; free($1); }
    | IDENTIFIER NEQ NUMBER{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_NEQ; $$.val.int_val=$3; $$.is_text=0; free($1); }
    | IDENTIFIER LT  NUMBER{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_LT;  $$.val.int_val=$3; $$.is_text=0; free($1); }
    | IDENTIFIER GT  NUMBER{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_GT;  $$.val.int_val=$3; $$.is_text=0; free($1); }
    | IDENTIFIER LTE NUMBER{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_LTE; $$.val.int_val=$3; $$.is_text=0; free($1); }
    | IDENTIFIER GTE NUMBER{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_GTE; $$.val.int_val=$3; $$.is_text=0; free($1); }
    | IDENTIFIER EQ  STRING{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_EQ;  strncpy($$.val.str_val,$3,MAX_STRING_LEN-1); $$.is_text=1; free($1);free($3); }
    | IDENTIFIER NEQ STRING{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_NEQ; strncpy($$.val.str_val,$3,MAX_STRING_LEN-1); $$.is_text=1; free($1);free($3); }
    | IDENTIFIER LT  STRING{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_LT;  strncpy($$.val.str_val,$3,MAX_STRING_LEN-1); $$.is_text=1; free($1);free($3); }
    | IDENTIFIER GT  STRING{ memset(&$$,0,sizeof($$)); strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_GT;  strncpy($$.val.str_val,$3,MAX_STRING_LEN-1); $$.is_text=1; free($1);free($3); }
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr,"Parse error at line %d: %s\n",yylineno,s);
}
