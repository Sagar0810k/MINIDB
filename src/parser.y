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

/* globals set by opt_order / opt_limit / opt_group / agg_expr */
char g_order_col[MAX_NAME_LEN]  = "";
int  g_order_desc               = 0;
int  g_has_order                = 0;
int  g_limit                    = -1;
char g_group_col[MAX_NAME_LEN]  = "";
int  g_has_group                = 0;
int  g_has_having               = 0;
CompOp g_having_op              = OP_EQ;
double g_having_val             = 0;
int  g_agg_type                 = 0;
char g_agg_col[MAX_NAME_LEN]    = "";
int  g_sfunc                    = 0;
char g_sfunc_col[MAX_NAME_LEN]  = "";

static void cond_init(Condition *c){ memset(c,0,sizeof(*c)); c->active=0; }
static void cond_add(Condition *c, SingleCond sc, int logic){
    if(c->count>=MAX_CONDITIONS) return;
    if(c->count>0) c->logic[c->count-1]=logic;
    c->conds[c->count++]=sc;
    c->active=1;
}
static void copy_select_globals(SelectData *d){
    strncpy(d->order_col,g_order_col,MAX_NAME_LEN-1);
    d->order_desc=g_order_desc; d->has_order=g_has_order; d->limit=g_limit;
    strncpy(d->group_col,g_group_col,MAX_NAME_LEN-1);
    d->has_group=g_has_group;
    d->has_having=g_has_having; d->having_op=g_having_op; d->having_val=g_having_val;
    d->agg_type=g_agg_type;
    strncpy(d->agg_col,g_agg_col,MAX_NAME_LEN-1);
    d->sfunc=g_sfunc;
    strncpy(d->sfunc_col,g_sfunc_col,MAX_NAME_LEN-1);
}
%}

%union {
    int    num;
    double fnum;
    char  *str;
    Column *col;
    Column *col_list;
    Value  *val;
    Value  *val_list;
    Condition cond;
    SingleCond scond;
    SetPair spair;
    SetPair *spair_list;
    CompOp  op;
}

%token CREATE TABLE INSERT INTO SELECT FROM VALUES
%token INT_TYPE FLOAT_TYPE TEXT_TYPE DATE_TYPE BOOL_TYPE
%token EXIT SHOW TABLES
%token WHERE AND OR NOT
%token DELETE UPDATE SET JOIN ON INNER LEFT RIGHT FULL OUTER
%token ORDER BY ASC DESC LIMIT
%token GROUP HAVING
%token COUNT SUM AVG MIN MAX
%token PRIMARY KEY UNIQUE NULLVAL IS BETWEEN LIKE IN
%token ALTER ADD DROP COLUMN RENAME TO DEFAULT
%token UPPER LOWER LENGTH_FN CONCAT_FN
%token LPAREN RPAREN COMMA SEMICOLON ASTERISK DOT
%token EQ NEQ LT GT LTE GTE

%token <num>  NUMBER
%token <fnum> FLOAT_NUM
%token <str>  IDENTIFIER STRING

%type <col_list> column_defs
%type <col>      column_def col_constraint
%type <val_list> value_list col_name_list in_list
%type <val>      value
%type <cond>     opt_where where_clause
%type <scond>    single_cond
%type <spair>    set_pair
%type <spair_list> set_list
%type <str>      col_ref agg_expr sfunc_expr data_type_tok
%type <op>       cmp_op

%start statement_list
%%

statement_list:
    | statement_list statement ;

statement:
    create_table_stmt | insert_stmt | select_stmt | delete_stmt
    | update_stmt | alter_stmt | drop_stmt | rename_stmt
    | show_tables_stmt | exit_stmt
    | error SEMICOLON { yyerrok; }
    ;

exit_stmt: EXIT SEMICOLON { should_exit=1; printf("Goodbye!\n"); } ;
show_tables_stmt: SHOW TABLES SEMICOLON { db_list_tables(global_db); } ;

/* ── Data type tokens ────────────────────────────────────────────── */
data_type_tok:
    INT_TYPE   { $$=strdup("INT");   }
    | FLOAT_TYPE { $$=strdup("FLOAT"); }
    | TEXT_TYPE  { $$=strdup("TEXT");  }
    | DATE_TYPE  { $$=strdup("DATE");  }
    | BOOL_TYPE  { $$=strdup("BOOL");  }
    ;

/* ── CREATE TABLE ────────────────────────────────────────────────── */
create_table_stmt:
    CREATE TABLE IDENTIFIER LPAREN column_defs RPAREN SEMICOLON {
        CreateTableData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        d->columns=$5; d->col_count=0;
        Column *t=$5;
        while(t&&d->col_count<MAX_COLUMNS&&strlen(t->name)>0){d->col_count++;t++;}
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
    IDENTIFIER data_type_tok col_constraint {
        Column *c=calloc(1,sizeof(*c));
        strncpy(c->name,$1,MAX_NAME_LEN-1);
        c->type=string_to_data_type($2);
        c->flags=$3?$3->flags:0;
        if($3){strncpy(c->default_val,$3->default_val,MAX_STRING_LEN-1);free($3);}
        free($1); free($2); $$=c;
    }
    ;

col_constraint:
    /* empty */                        { $$=NULL; }
    | PRIMARY KEY                      { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_PRIMARY_KEY|COL_FLAG_UNIQUE|COL_FLAG_NOT_NULL; $$=c; }
    | UNIQUE                           { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_UNIQUE; $$=c; }
    | NOT NULLVAL                      { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_NOT_NULL; $$=c; }
    | DEFAULT STRING                   { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_HAS_DEFAULT; strncpy(c->default_val,$2,MAX_STRING_LEN-1); free($2); $$=c; }
    | DEFAULT NUMBER                   { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_HAS_DEFAULT; snprintf(c->default_val,MAX_STRING_LEN,"%d",$2); $$=c; }
    | NOT NULLVAL DEFAULT STRING       { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_NOT_NULL|COL_FLAG_HAS_DEFAULT; strncpy(c->default_val,$4,MAX_STRING_LEN-1); free($4); $$=c; }
    | NOT NULLVAL DEFAULT NUMBER       { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_NOT_NULL|COL_FLAG_HAS_DEFAULT; snprintf(c->default_val,MAX_STRING_LEN,"%d",$4); $$=c; }
    ;

/* ── INSERT ──────────────────────────────────────────────────────── */
insert_stmt:
    /* positional */
    INSERT INTO IDENTIFIER VALUES LPAREN value_list RPAREN SEMICOLON {
        InsertData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        d->values=$6; d->named=0; d->value_count=0;
        Value *t=$6;
        while(d->value_count<MAX_COLUMNS){d->value_count++;t++;if(t->int_val==0&&strlen(t->str_val)==0&&!t->is_float)break;}
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_INSERT; n->data=d;
        execute_ast(n); free($3); free(d->values); free(d); free(n);
    }
    /* named columns */
    | INSERT INTO IDENTIFIER LPAREN col_name_list RPAREN VALUES LPAREN value_list RPAREN SEMICOLON {
        InsertData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        d->named=1;
        int cnt=0;
        while(cnt<MAX_COLUMNS&&strlen($5[cnt].str_val)>0){
            strncpy(d->col_names[cnt],$5[cnt].str_val,MAX_NAME_LEN-1); cnt++;
        }
        d->named_count=cnt; free($5);
        d->values=$9; d->value_count=cnt;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_INSERT; n->data=d;
        execute_ast(n); free($3); free(d->values); free(d); free(n);
    }
    ;

value_list:
    value { $$=calloc(MAX_COLUMNS,sizeof(Value)); $$[0]=*$1; free($1); }
    | value_list COMMA value {
        int n=0;
        while(n<MAX_COLUMNS&&!($1[n].int_val==0&&strlen($1[n].str_val)==0&&!$1[n].is_float))n++;
        $1[n]=*$3; free($3); $$=$1;
    }
    ;

value:
    NUMBER     { Value *v=calloc(1,sizeof(*v)); v->int_val=$1; $$=v; }
    | FLOAT_NUM{ Value *v=calloc(1,sizeof(*v)); v->float_val=$1; v->is_float=1; $$=v; }
    | STRING   { Value *v=calloc(1,sizeof(*v)); strncpy(v->str_val,$1,MAX_STRING_LEN-1); $$=v; free($1); }
    | NULLVAL  { Value *v=calloc(1,sizeof(*v)); v->is_null=1; $$=v; }
    ;

/* ── SELECT ──────────────────────────────────────────────────────── */
select_stmt:
    SELECT ASTERISK FROM IDENTIFIER opt_where opt_group opt_order opt_limit SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=1; d->where=$5; copy_select_globals(d);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free(d); free(n);
    }
    | SELECT col_name_list FROM IDENTIFIER opt_where opt_group opt_order opt_limit SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=0; d->where=$5; copy_select_globals(d);
        int cnt=0;
        while(cnt<MAX_COLUMNS&&strlen($2[cnt].str_val)>0){
            strncpy(d->col_names[cnt],$2[cnt].str_val,MAX_NAME_LEN-1); cnt++;
        }
        d->col_count=cnt; free($2);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free(d); free(n);
    }
    /* aggregate: SELECT COUNT(*)/SUM/AVG/MIN/MAX FROM t [WHERE] [GROUP BY] [HAVING] */
    | SELECT agg_expr FROM IDENTIFIER opt_where opt_group opt_having SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=1; d->where=$5; d->has_order=0; d->limit=-1;
        copy_select_globals(d);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free($2); free(d); free(n);
    }
    /* string function: SELECT UPPER(col)/LOWER(col)/LENGTH(col) FROM t [WHERE] */
    | SELECT sfunc_expr FROM IDENTIFIER opt_where SEMICOLON {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$4,MAX_NAME_LEN-1);
        d->select_all=1; d->where=$5; d->has_order=0; d->limit=-1;
        d->agg_type=0; d->sfunc=g_sfunc;
        strncpy(d->sfunc_col,g_sfunc_col,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free($4); free($2); free(d); free(n);
    }
    /* JOINs */
    | SELECT ASTERISK FROM IDENTIFIER INNER JOIN IDENTIFIER ON col_ref EQ col_ref SEMICOLON {
        JoinData *d=calloc(1,sizeof(*d));
        strncpy(d->table1,$4,MAX_NAME_LEN-1); strncpy(d->table2,$7,MAX_NAME_LEN-1);
        char *dot1=strchr($9,'.'),*dot2=strchr($11,'.');
        strncpy(d->col1,dot1?dot1+1:$9,MAX_NAME_LEN-1);
        strncpy(d->col2,dot2?dot2+1:$11,MAX_NAME_LEN-1);
        d->join_type=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_JOIN; n->data=d;
        execute_ast(n); free($4);free($7);free($9);free($11);free(d);free(n);
    }
    | SELECT ASTERISK FROM IDENTIFIER LEFT OUTER JOIN IDENTIFIER ON col_ref EQ col_ref SEMICOLON {
        JoinData *d=calloc(1,sizeof(*d));
        strncpy(d->table1,$4,MAX_NAME_LEN-1); strncpy(d->table2,$8,MAX_NAME_LEN-1);
        char *dot1=strchr($10,'.'),*dot2=strchr($12,'.');
        strncpy(d->col1,dot1?dot1+1:$10,MAX_NAME_LEN-1);
        strncpy(d->col2,dot2?dot2+1:$12,MAX_NAME_LEN-1);
        d->join_type=1;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_JOIN; n->data=d;
        execute_ast(n); free($4);free($8);free($10);free($12);free(d);free(n);
    }
    | SELECT ASTERISK FROM IDENTIFIER RIGHT OUTER JOIN IDENTIFIER ON col_ref EQ col_ref SEMICOLON {
        JoinData *d=calloc(1,sizeof(*d));
        strncpy(d->table1,$4,MAX_NAME_LEN-1); strncpy(d->table2,$8,MAX_NAME_LEN-1);
        char *dot1=strchr($10,'.'),*dot2=strchr($12,'.');
        strncpy(d->col1,dot1?dot1+1:$10,MAX_NAME_LEN-1);
        strncpy(d->col2,dot2?dot2+1:$12,MAX_NAME_LEN-1);
        d->join_type=2;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_JOIN; n->data=d;
        execute_ast(n); free($4);free($8);free($10);free($12);free(d);free(n);
    }
    | SELECT ASTERISK FROM IDENTIFIER FULL OUTER JOIN IDENTIFIER ON col_ref EQ col_ref SEMICOLON {
        JoinData *d=calloc(1,sizeof(*d));
        strncpy(d->table1,$4,MAX_NAME_LEN-1); strncpy(d->table2,$8,MAX_NAME_LEN-1);
        char *dot1=strchr($10,'.'),*dot2=strchr($12,'.');
        strncpy(d->col1,dot1?dot1+1:$10,MAX_NAME_LEN-1);
        strncpy(d->col2,dot2?dot2+1:$12,MAX_NAME_LEN-1);
        d->join_type=3;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_JOIN; n->data=d;
        execute_ast(n); free($4);free($8);free($10);free($12);free(d);free(n);
    }
    ;

agg_expr:
    COUNT LPAREN ASTERISK RPAREN  { g_agg_type=AGG_COUNT; strcpy(g_agg_col,"*"); g_sfunc=0; $$=strdup("COUNT(*)"); }
    | COUNT LPAREN IDENTIFIER RPAREN { g_agg_type=AGG_COUNT; strncpy(g_agg_col,$3,MAX_NAME_LEN-1); g_sfunc=0; $$=strdup($3); free($3); }
    | SUM LPAREN IDENTIFIER RPAREN   { g_agg_type=AGG_SUM;   strncpy(g_agg_col,$3,MAX_NAME_LEN-1); g_sfunc=0; $$=strdup($3); free($3); }
    | AVG LPAREN IDENTIFIER RPAREN   { g_agg_type=AGG_AVG;   strncpy(g_agg_col,$3,MAX_NAME_LEN-1); g_sfunc=0; $$=strdup($3); free($3); }
    | MIN LPAREN IDENTIFIER RPAREN   { g_agg_type=AGG_MIN;   strncpy(g_agg_col,$3,MAX_NAME_LEN-1); g_sfunc=0; $$=strdup($3); free($3); }
    | MAX LPAREN IDENTIFIER RPAREN   { g_agg_type=AGG_MAX;   strncpy(g_agg_col,$3,MAX_NAME_LEN-1); g_sfunc=0; $$=strdup($3); free($3); }
    ;

sfunc_expr:
    UPPER     LPAREN IDENTIFIER RPAREN { g_sfunc=1; strncpy(g_sfunc_col,$3,MAX_NAME_LEN-1); g_agg_type=0; $$=strdup($3); free($3); }
    | LOWER   LPAREN IDENTIFIER RPAREN { g_sfunc=2; strncpy(g_sfunc_col,$3,MAX_NAME_LEN-1); g_agg_type=0; $$=strdup($3); free($3); }
    | LENGTH_FN LPAREN IDENTIFIER RPAREN { g_sfunc=3; strncpy(g_sfunc_col,$3,MAX_NAME_LEN-1); g_agg_type=0; $$=strdup($3); free($3); }
    ;

col_name_list:
    IDENTIFIER { $$=calloc(MAX_COLUMNS,sizeof(Value)); strncpy($$[0].str_val,$1,MAX_NAME_LEN-1); free($1); }
    | col_name_list COMMA IDENTIFIER {
        int n=0; while(n<MAX_COLUMNS&&strlen($1[n].str_val)>0)n++;
        strncpy($1[n].str_val,$3,MAX_NAME_LEN-1); free($3); $$=$1;
    }
    ;

col_ref:
    IDENTIFIER DOT IDENTIFIER { char *b=malloc(MAX_NAME_LEN*2+2); snprintf(b,MAX_NAME_LEN*2+1,"%s.%s",$1,$3); free($1);free($3); $$=b; }
    | IDENTIFIER { $$=$1; }
    ;

opt_order:
    { g_order_col[0]='\0'; g_order_desc=0; g_has_order=0; }
    | ORDER BY IDENTIFIER     { strncpy(g_order_col,$3,MAX_NAME_LEN-1); g_order_desc=0; g_has_order=1; free($3); }
    | ORDER BY IDENTIFIER ASC { strncpy(g_order_col,$3,MAX_NAME_LEN-1); g_order_desc=0; g_has_order=1; free($3); }
    | ORDER BY IDENTIFIER DESC{ strncpy(g_order_col,$3,MAX_NAME_LEN-1); g_order_desc=1; g_has_order=1; free($3); }
    ;

opt_limit:
    { g_limit=-1; }
    | LIMIT NUMBER { g_limit=$2; }
    ;

opt_group:
    { g_group_col[0]='\0'; g_has_group=0; }
    | GROUP BY IDENTIFIER { strncpy(g_group_col,$3,MAX_NAME_LEN-1); g_has_group=1; free($3); }
    ;

opt_having:
    { g_has_having=0; }
    | HAVING agg_expr cmp_op NUMBER  { g_has_having=1; g_having_op=$3; g_having_val=$4; free($2); }
    | HAVING agg_expr cmp_op FLOAT_NUM { g_has_having=1; g_having_op=$3; g_having_val=$4; free($2); }
    ;

/* ── DELETE ──────────────────────────────────────────────────────── */
delete_stmt:
    DELETE FROM IDENTIFIER opt_where SEMICOLON {
        DeleteData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1); d->where=$4;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_DELETE; n->data=d;
        execute_ast(n); free($3); free(d); free(n);
    }
    ;

/* ── UPDATE (multi-SET) ──────────────────────────────────────────── */
update_stmt:
    UPDATE IDENTIFIER SET set_list opt_where SEMICOLON {
        UpdateData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$2,MAX_NAME_LEN-1);
        d->where=$5;
        int n=0;
        SetPair *sp=$4;
        while(n<MAX_SET_COLS&&strlen(sp[n].col)>0){d->sets[n]=sp[n];n++;}
        d->set_count=n; free($4);
        ASTNode *node=calloc(1,sizeof(*node)); node->type=AST_UPDATE; node->data=d;
        execute_ast(node); free($2); free(d); free(node);
    }
    ;

set_list:
    set_pair {
        $$=calloc(MAX_SET_COLS,sizeof(SetPair)); $$[0]=$1;
    }
    | set_list COMMA set_pair {
        int n=0; while(n<MAX_SET_COLS&&strlen($1[n].col)>0)n++;
        $1[n]=$3; $$=$1;
    }
    ;

set_pair:
    IDENTIFIER EQ value {
        memset(&$$,0,sizeof($$));
        strncpy($$.col,$1,MAX_NAME_LEN-1);
        $$.val=*$3;
        $$.is_text=(strlen($3->str_val)>0);
        free($1); free($3);
    }
    ;

/* ── ALTER TABLE ─────────────────────────────────────────────────── */
alter_stmt:
    ALTER TABLE IDENTIFIER ADD COLUMN IDENTIFIER data_type_tok SEMICOLON {
        AlterAddData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        strncpy(d->new_col.name,$6,MAX_NAME_LEN-1);
        d->new_col.type=string_to_data_type($7); d->new_col.flags=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_ADD; n->data=d;
        execute_ast(n); free($3);free($6);free($7);free(d);free(n);
    }
    | ALTER TABLE IDENTIFIER DROP COLUMN IDENTIFIER SEMICOLON {
        AlterDropData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        strncpy(d->col_name,$6,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_DROP; n->data=d;
        execute_ast(n); free($3);free($6);free(d);free(n);
    }
    ;

/* ── DROP TABLE ──────────────────────────────────────────────────── */
drop_stmt:
    DROP TABLE IDENTIFIER SEMICOLON {
        DropTableData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,$3,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_DROP_TABLE; n->data=d;
        execute_ast(n); free($3);free(d);free(n);
    }
    ;

/* ── RENAME TABLE ────────────────────────────────────────────────── */
rename_stmt:
    RENAME TABLE IDENTIFIER TO IDENTIFIER SEMICOLON {
        RenameTableData *d=calloc(1,sizeof(*d));
        strncpy(d->old_name,$3,MAX_NAME_LEN-1);
        strncpy(d->new_name,$5,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_RENAME_TABLE; n->data=d;
        execute_ast(n); free($3);free($5);free(d);free(n);
    }
    ;

/* ── WHERE ───────────────────────────────────────────────────────── */
opt_where:
    { cond_init(&$$); }
    | where_clause { $$=$1; }
    ;

where_clause:
    WHERE single_cond           { cond_init(&$$); cond_add(&$$,$2,0); }
    | where_clause AND single_cond { $$=$1; cond_add(&$$,$3,0); }
    | where_clause OR  single_cond { $$=$1; cond_add(&$$,$3,1); }
    ;

cmp_op:
    EQ  { $$=OP_EQ;  }
    | NEQ { $$=OP_NEQ; }
    | LT  { $$=OP_LT;  }
    | GT  { $$=OP_GT;  }
    | LTE { $$=OP_LTE; }
    | GTE { $$=OP_GTE; }
    ;

in_list:
    value { $$=calloc(MAX_IN_VALUES,sizeof(Value)); $$[0]=*$1; free($1); }
    | in_list COMMA value {
        int n=0;
        while(n<MAX_IN_VALUES&&!($1[n].int_val==0&&strlen($1[n].str_val)==0&&!$1[n].is_float))n++;
        $1[n]=*$3; free($3); $$=$1;
    }
    ;

single_cond:
    IDENTIFIER cmp_op NUMBER {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=$2; $$.val.int_val=$3; $$.is_text=0; free($1);
    }
    | IDENTIFIER cmp_op FLOAT_NUM {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=$2; $$.val.float_val=$3; $$.val.is_float=1; $$.is_text=0; free($1);
    }
    | IDENTIFIER cmp_op STRING {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=$2;
        strncpy($$.val.str_val,$3,MAX_STRING_LEN-1); $$.is_text=1; free($1);free($3);
    }
    | IDENTIFIER IS NULLVAL {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_IS_NULL; free($1);
    }
    | IDENTIFIER IS NOT NULLVAL {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_IS_NOT_NULL; free($1);
    }
    | IDENTIFIER LIKE STRING {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_LIKE;
        strncpy($$.val.str_val,$3,MAX_STRING_LEN-1); $$.is_text=1; free($1);free($3);
    }
    | IDENTIFIER BETWEEN value AND value {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_BETWEEN;
        $$.val=*$3; $$.val2=*$5;
        $$.is_text=(strlen($3->str_val)>0);
        free($1);free($3);free($5);
    }
    | IDENTIFIER IN LPAREN in_list RPAREN {
        memset(&$$,0,sizeof($$));
        strncpy($$.col_name,$1,MAX_NAME_LEN-1); $$.op=OP_IN;
        int n=0;
        while(n<MAX_IN_VALUES&&!($4[n].int_val==0&&strlen($4[n].str_val)==0&&!$4[n].is_float)){
            $$.in_vals[n]=$4[n]; n++;
        }
        $$.in_count=n;
        $$.is_text=(n>0&&strlen($4[0].str_val)>0);
        free($1);free($4);
    }
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr,"Parse error at line %d: %s\n",yylineno,s);
}
