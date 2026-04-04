#include "../include/executor.h"
#include "../include/storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ═══════════════════════════════════════════════════════════════════
   HELPERS
   ═══════════════════════════════════════════════════════════════════ */

static int find_col(Table *t, const char *name) {
    for (int i = 0; i < t->col_count; i++)
        if (strcasecmp(t->columns[i].name, name) == 0) return i;
    return -1;
}

/* Compare a value in a row against a SingleCond */
static int eval_single(Table *t, Row *row, SingleCond *c) {
    int ci = find_col(t, c->col_name);
    if (ci < 0) return 0;

    Value *rv = &row->values[ci];

    /* NULL checks */
    if (c->op == OP_IS_NULL)     return rv->is_null;
    if (c->op == OP_IS_NOT_NULL) return !rv->is_null;
    if (rv->is_null) return 0;

    /* LIKE */
    if (c->op == OP_LIKE)
        return like_match(rv->str_val, c->val.str_val);

    /* IN */
    if (c->op == OP_IN) {
        for (int i = 0; i < c->in_count; i++) {
            if (c->is_text) {
                if (strcmp(rv->str_val, c->in_vals[i].str_val)==0) return 1;
            } else {
                if (rv->int_val == c->in_vals[i].int_val) return 1;
            }
        }
        return 0;
    }

    /* BETWEEN */
    if (c->op == OP_BETWEEN) {
        if (c->is_text)
            return strcmp(rv->str_val, c->val.str_val)>=0 &&
                   strcmp(rv->str_val, c->val2.str_val)<=0;
        return rv->int_val >= c->val.int_val && rv->int_val <= c->val2.int_val;
    }

    /* Regular compare */
    if (c->is_text) {
        int cmp = strcmp(rv->str_val, c->val.str_val);
        switch (c->op) {
            case OP_EQ:  return cmp==0;
            case OP_NEQ: return cmp!=0;
            case OP_LT:  return cmp<0;
            case OP_GT:  return cmp>0;
            case OP_LTE: return cmp<=0;
            case OP_GTE: return cmp>=0;
            default: return 0;
        }
    } else {
        /* support float comparisons */
        double a = rv->is_float ? rv->float_val : rv->int_val;
        double b = c->val.is_float ? c->val.float_val : c->val.int_val;
        switch (c->op) {
            case OP_EQ:  return a==b;
            case OP_NEQ: return a!=b;
            case OP_LT:  return a<b;
            case OP_GT:  return a>b;
            case OP_LTE: return a<=b;
            case OP_GTE: return a>=b;
            default: return 0;
        }
    }
}

/* Evaluate full WHERE */
static int row_matches(Table *t, Row *row, Condition *w) {
    if (!w->active) return 1;
    int result = eval_single(t, row, &w->conds[0]);
    for (int i = 1; i < w->count; i++) {
        int next = eval_single(t, row, &w->conds[i]);
        result = (w->logic[i-1]==0) ? (result && next) : (result || next);
    }
    return result;
}

/* Value compare for ORDER BY */
static int cmp_val(Value *a, Value *b, DataType type, int desc) {
    int r;
    if (type==TYPE_INT || type==TYPE_BOOL) {
        double av = a->is_float ? a->float_val : a->int_val;
        double bv = b->is_float ? b->float_val : b->int_val;
        r = (av>bv)-(av<bv);
    } else {
        r = strcmp(a->str_val, b->str_val);
    }
    return desc ? -r : r;
}

/* Print a single value */
static void print_val(Value *v, DataType type, int width) {
    if (v->is_null) { printf("%-*s", width, "NULL"); return; }
    if (type==TYPE_INT || type==TYPE_BOOL) printf("%-*d", width, v->int_val);
    else if (type==TYPE_FLOAT) printf("%-*.2f", width, v->float_val);
    else printf("%-*s", width, v->str_val);
}

/* Apply string function to value, store in buf */
static void apply_sfunc(int sfunc, Value *v, char *buf, size_t bufsz) {
    switch(sfunc) {
        case 1: str_upper(buf, v->str_val); break;
        case 2: str_lower(buf, v->str_val); break;
        case 3: snprintf(buf, bufsz, "%d", str_length(v->str_val)); break;
        default: strncpy(buf, v->str_val, bufsz-1);
    }
}

/* ═══════════════════════════════════════════════════════════════════
   DISPATCH
   ═══════════════════════════════════════════════════════════════════ */
void execute_ast(ASTNode *node) {
    if (!node) { print_error("Invalid AST node"); return; }
    switch (node->type) {
        case AST_CREATE_TABLE:  execute_create_table((CreateTableData*)node->data);  break;
        case AST_INSERT:        execute_insert((InsertData*)node->data);              break;
        case AST_SELECT:        execute_select((SelectData*)node->data);              break;
        case AST_DELETE:        execute_delete((DeleteData*)node->data);              break;
        case AST_UPDATE:        execute_update((UpdateData*)node->data);              break;
        case AST_JOIN:          execute_join((JoinData*)node->data);                  break;
        case AST_ALTER_ADD:     execute_alter_add((AlterAddData*)node->data);         break;
        case AST_ALTER_DROP:    execute_alter_drop((AlterDropData*)node->data);       break;
        case AST_DROP_TABLE:    execute_drop_table((DropTableData*)node->data);       break;
        case AST_RENAME_TABLE:  execute_rename_table((RenameTableData*)node->data);   break;
        default: print_error("Unknown AST node type");
    }
}

/* ═══════════════════════════════════════════════════════════════════
   CREATE TABLE
   ═══════════════════════════════════════════════════════════════════ */
void execute_create_table(CreateTableData *data) {
    if (!data) { print_error("Invalid CREATE TABLE data"); return; }
    if (storage_table_exists(data->table_name)) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' already exists",data->table_name);
        print_error(msg); return;
    }
    Table *table = table_create(data->table_name, data->columns, data->col_count);
    if (!table) { print_error("Failed to create table"); return; }
    if (db_add_table(global_db, table) < 0) { table_free(table); return; }
    char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' created",data->table_name);
    print_success(msg);
}

/* ═══════════════════════════════════════════════════════════════════
   INSERT (positional + named columns)
   ═══════════════════════════════════════════════════════════════════ */
void execute_insert(InsertData *data) {
    if (!data) { print_error("Invalid INSERT data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }

    /* Build full row: start with defaults / NULL */
    Value row_vals[MAX_COLUMNS];
    memset(row_vals, 0, sizeof(row_vals));
    for (int i = 0; i < table->col_count; i++) {
        row_vals[i].is_null = 1;
        if (table->columns[i].flags & COL_FLAG_HAS_DEFAULT) {
            row_vals[i].is_null = 0;
            if (table->columns[i].type == TYPE_INT || table->columns[i].type == TYPE_BOOL)
                row_vals[i].int_val = atoi(table->columns[i].default_val);
            else if (table->columns[i].type == TYPE_FLOAT)
                row_vals[i].float_val = atof(table->columns[i].default_val);
            else
                strncpy(row_vals[i].str_val, table->columns[i].default_val, MAX_STRING_LEN-1);
        }
    }

    if (data->named) {
        /* Map named cols to positions */
        for (int i = 0; i < data->named_count; i++) {
            int ci = find_col(table, data->col_names[i]);
            if (ci < 0) {
                char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' not found",data->col_names[i]);
                print_error(msg); return;
            }
            row_vals[ci] = data->values[i];
            row_vals[ci].is_null = 0;
        }
    } else {
        if (data->value_count != table->col_count) {
            print_error("Value count does not match column count"); return;
        }
        for (int i = 0; i < table->col_count; i++) {
            row_vals[i] = data->values[i];
            row_vals[i].is_null = 0;
        }
    }

    /* Constraint checks */
    for (int ci = 0; ci < table->col_count; ci++) {
        if ((table->columns[ci].flags & COL_FLAG_NOT_NULL) && row_vals[ci].is_null) {
            char msg[128]; snprintf(msg,sizeof(msg),"NOT NULL constraint failed on '%s'",table->columns[ci].name);
            print_error(msg); return;
        }
        if (table->columns[ci].flags & (COL_FLAG_PRIMARY_KEY|COL_FLAG_UNIQUE)) {
            for (int r = 0; r < table->row_count; r++) {
                int dup;
                if (table->columns[ci].type==TYPE_INT || table->columns[ci].type==TYPE_BOOL)
                    dup = (table->rows[r].values[ci].int_val == row_vals[ci].int_val);
                else
                    dup = (strcmp(table->rows[r].values[ci].str_val, row_vals[ci].str_val)==0);
                if (dup && !row_vals[ci].is_null) {
                    char msg[128];
                    snprintf(msg,sizeof(msg),"Duplicate in %s column '%s'",
                        (table->columns[ci].flags&COL_FLAG_PRIMARY_KEY)?"PRIMARY KEY":"UNIQUE",
                        table->columns[ci].name);
                    print_error(msg); return;
                }
            }
        }
        /* CHECK constraint stored as "col op val" in default_val with flag */
    }

    if (table_insert_row(table, row_vals) < 0) { print_error("Failed to insert row"); return; }
    storage_save_table(table);
    print_success("Row inserted");
}

/* ═══════════════════════════════════════════════════════════════════
   SELECT  (columns, WHERE, ORDER BY, LIMIT, GROUP BY, HAVING, agg, sfunc)
   ═══════════════════════════════════════════════════════════════════ */
void execute_select(SelectData *data) {
    if (!data) { print_error("Invalid SELECT data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }

    /* Column indices */
    int idx[MAX_COLUMNS], ncols;
    if (data->select_all) {
        ncols = table->col_count;
        for (int i = 0; i < ncols; i++) idx[i] = i;
    } else {
        ncols = data->col_count;
        for (int i = 0; i < ncols; i++) {
            idx[i] = find_col(table, data->col_names[i]);
            if (idx[i] < 0) {
                char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' not found",data->col_names[i]);
                print_error(msg); return;
            }
        }
    }

    /* Collect matching rows */
    int matched[MAX_ROWS], nm = 0;
    for (int r = 0; r < table->row_count; r++)
        if (row_matches(table, &table->rows[r], &data->where))
            matched[nm++] = r;

    /* ── GROUP BY ──────────────────────────────────────────────── */
    if (data->has_group) {
        int gci = find_col(table, data->group_col);
        if (gci < 0) { print_error("GROUP BY column not found"); return; }

        /* For each unique group value, compute aggregate */
        char seen_str[MAX_ROWS][MAX_STRING_LEN];
        int  seen_int[MAX_ROWS];
        int  seen_count = 0;

        printf("\n%-15s %-15s\n", data->group_col,
            data->agg_type==AGG_COUNT?"COUNT":
            data->agg_type==AGG_SUM?"SUM":
            data->agg_type==AGG_AVG?"AVG":
            data->agg_type==AGG_MIN?"MIN":"MAX");
        for (int i=0;i<30;i++) printf("-"); printf("\n");

        for (int i = 0; i < nm; i++) {
            int r = matched[i];
            Value *gv = &table->rows[r].values[gci];
            /* check if seen */
            int found = -1;
            for (int s = 0; s < seen_count; s++) {
                if (table->columns[gci].type==TYPE_INT) {
                    if (seen_int[s]==gv->int_val){found=s;break;}
                } else {
                    if (strcmp(seen_str[s],gv->str_val)==0){found=s;break;}
                }
            }
            if (found >= 0) continue; /* already printed */

            /* register */
            if (table->columns[gci].type==TYPE_INT) seen_int[seen_count]=gv->int_val;
            else strncpy(seen_str[seen_count],gv->str_val,MAX_STRING_LEN-1);
            seen_count++;

            /* compute aggregate for this group */
            int aci = -1;
            if (data->agg_type!=AGG_COUNT) {
                aci = find_col(table, data->agg_col);
                if (aci<0){print_error("Aggregate column not found");return;}
            }
            double agg_result=0; int agg_n=0;
            int first=1;
            for (int j=0;j<nm;j++){
                int rj=matched[j];
                Value *gjv=&table->rows[rj].values[gci];
                int same=(table->columns[gci].type==TYPE_INT)?
                    (gjv->int_val==gv->int_val):
                    (strcmp(gjv->str_val,gv->str_val)==0);
                if (!same) continue;
                double v=(aci>=0)?
                    (table->rows[rj].values[aci].is_float?
                     table->rows[rj].values[aci].float_val:
                     table->rows[rj].values[aci].int_val) : 1.0;
                if(data->agg_type==AGG_COUNT) agg_result+=1;
                else if(data->agg_type==AGG_SUM) agg_result+=v;
                else if(data->agg_type==AGG_AVG){agg_result+=v;agg_n++;}
                else if(data->agg_type==AGG_MIN){if(first||v<agg_result){agg_result=v;first=0;}}
                else if(data->agg_type==AGG_MAX){if(first||v>agg_result){agg_result=v;first=0;}}
                if(data->agg_type==AGG_COUNT)agg_n++;
            }
            if(data->agg_type==AGG_AVG) agg_result = agg_n?agg_result/agg_n:0;

            /* HAVING filter */
            if (data->has_having) {
                double hv = data->having_val;
                int ok=0;
                switch(data->having_op){
                    case OP_EQ:  ok=fabs(agg_result-hv)<1e-9; break;
                    case OP_NEQ: ok=fabs(agg_result-hv)>=1e-9;break;
                    case OP_LT:  ok=agg_result<hv; break;
                    case OP_GT:  ok=agg_result>hv; break;
                    case OP_LTE: ok=agg_result<=hv;break;
                    case OP_GTE: ok=agg_result>=hv;break;
                    default: ok=1;
                }
                if (!ok) continue;
            }

            /* print group key */
            if (table->columns[gci].type==TYPE_INT) printf("%-15d ", gv->int_val);
            else printf("%-15s ", gv->str_val);
            /* print aggregate */
            if(data->agg_type==AGG_COUNT||(data->agg_type!=AGG_AVG&&fmod(agg_result,1.0)==0))
                printf("%-15.0f\n",agg_result);
            else printf("%-15.2f\n",agg_result);
        }
        printf("\n");
        return;
    }

    /* ── Scalar aggregates (no GROUP BY) ───────────────────────── */
    if (data->agg_type > 0) {
        int aci = -1;
        if (data->agg_type != AGG_COUNT) {
            aci = find_col(table, data->agg_col);
            if (aci<0){char m[128];snprintf(m,sizeof(m),"Column '%s' not found",data->agg_col);print_error(m);return;}
        }
        double result=0; int n=0; int first=1;
        for (int i=0;i<nm;i++){
            int r=matched[i];
            double v=(aci>=0)?(table->rows[r].values[aci].is_float?
                table->rows[r].values[aci].float_val:table->rows[r].values[aci].int_val):1.0;
            if(data->agg_type==AGG_COUNT) result++;
            else if(data->agg_type==AGG_SUM) result+=v;
            else if(data->agg_type==AGG_AVG){result+=v;n++;}
            else if(data->agg_type==AGG_MIN){if(first||v<result){result=v;first=0;}}
            else if(data->agg_type==AGG_MAX){if(first||v>result){result=v;first=0;}}
        }
        if(data->agg_type==AGG_AVG) result=n?result/n:0;
        const char *names[]={"","COUNT","SUM","AVG","MIN","MAX"};
        const char *label=names[data->agg_type];
        char header[64];
        if(data->agg_type==AGG_COUNT) snprintf(header,sizeof(header),"COUNT(*)");
        else snprintf(header,sizeof(header),"%s(%s)",label,data->agg_col);
        printf("\n%-15s\n",header);
        for(int i=0;i<15;i++)printf("-");printf("\n");
        if(data->agg_type==AGG_COUNT||(data->agg_type!=AGG_AVG&&fmod(result,1.0)==0))
            printf("%-15.0f\n",result);
        else printf("%-15.2f\n",result);
        printf("\n1 row returned\n\n");
        return;
    }

    /* ── String function ────────────────────────────────────────── */
    if (data->sfunc > 0) {
        int sci = find_col(table, data->sfunc_col);
        if (sci<0){char m[128];snprintf(m,sizeof(m),"Column '%s' not found",data->sfunc_col);print_error(m);return;}
        const char *fnames[]={"","UPPER","LOWER","LENGTH"};
        char hdr[64]; snprintf(hdr,sizeof(hdr),"%s(%s)",fnames[data->sfunc],data->sfunc_col);
        printf("\n%-20s\n",hdr);
        for(int i=0;i<20;i++)printf("-");printf("\n");
        for(int i=0;i<nm;i++){
            char buf[MAX_STRING_LEN];
            apply_sfunc(data->sfunc,&table->rows[matched[i]].values[sci],buf,sizeof(buf));
            printf("%-20s\n",buf);
        }
        printf("\n%d row(s) returned\n\n",nm);
        return;
    }

    /* ── ORDER BY ───────────────────────────────────────────────── */
    if (data->has_order) {
        int oci = find_col(table, data->order_col);
        if (oci<0){char m[128];snprintf(m,sizeof(m),"ORDER BY column '%s' not found",data->order_col);print_error(m);return;}
        for (int i=1;i<nm;i++){
            int key=matched[i],j=i-1;
            while(j>=0 && cmp_val(&table->rows[matched[j]].values[oci],
                                   &table->rows[key].values[oci],
                                   table->columns[oci].type,data->order_desc)>0){
                matched[j+1]=matched[j];j--;
            }
            matched[j+1]=key;
        }
    }

    /* ── LIMIT ──────────────────────────────────────────────────── */
    if (data->limit>=0 && nm>data->limit) nm=data->limit;

    /* ── Print ──────────────────────────────────────────────────── */
    printf("\n");
    for (int i=0;i<ncols;i++) printf("%-15s",table->columns[idx[i]].name);
    printf("\n");
    for (int i=0;i<ncols*15;i++) printf("-");
    printf("\n");
    for (int i=0;i<nm;i++){
        int r=matched[i];
        for(int j=0;j<ncols;j++) print_val(&table->rows[r].values[idx[j]],table->columns[idx[j]].type,15);
        printf("\n");
    }
    printf("\n%d row(s) returned\n\n",nm);
}

/* ═══════════════════════════════════════════════════════════════════
   DELETE
   ═══════════════════════════════════════════════════════════════════ */
void execute_delete(DeleteData *data) {
    if (!data){print_error("Invalid DELETE data");return;}
    Table *table=db_get_table(global_db,data->table_name);
    if(!table){char m[128];snprintf(m,sizeof(m),"Table '%s' does not exist",data->table_name);print_error(m);return;}
    int deleted=0,nc=0;
    for(int r=0;r<table->row_count;r++){
        if(row_matches(table,&table->rows[r],&data->where)){free(table->rows[r].values);deleted++;}
        else table->rows[nc++]=table->rows[r];
    }
    table->row_count=nc;
    storage_save_table(table);
    char msg[64];snprintf(msg,sizeof(msg),"%d row(s) deleted",deleted);print_success(msg);
}

/* ═══════════════════════════════════════════════════════════════════
   UPDATE  (multi-SET)
   ═══════════════════════════════════════════════════════════════════ */
void execute_update(UpdateData *data) {
    if(!data){print_error("Invalid UPDATE data");return;}
    Table *table=db_get_table(global_db,data->table_name);
    if(!table){char m[128];snprintf(m,sizeof(m),"Table '%s' does not exist",data->table_name);print_error(m);return;}
    /* Validate all SET columns */
    int cis[MAX_SET_COLS];
    for(int s=0;s<data->set_count;s++){
        cis[s]=find_col(table,data->sets[s].col);
        if(cis[s]<0){char m[128];snprintf(m,sizeof(m),"Column '%s' not found",data->sets[s].col);print_error(m);return;}
    }
    int updated=0;
    for(int r=0;r<table->row_count;r++){
        if(!row_matches(table,&table->rows[r],&data->where)) continue;
        for(int s=0;s<data->set_count;s++){
            int ci=cis[s];
            if(data->sets[s].is_text)
                strncpy(table->rows[r].values[ci].str_val,data->sets[s].val.str_val,MAX_STRING_LEN-1);
            else if(data->sets[s].val.is_float)
                table->rows[r].values[ci].float_val=data->sets[s].val.float_val;
            else
                table->rows[r].values[ci].int_val=data->sets[s].val.int_val;
            table->rows[r].values[ci].is_null=0;
        }
        updated++;
    }
    storage_save_table(table);
    char msg[64];snprintf(msg,sizeof(msg),"%d row(s) updated",updated);print_success(msg);
}

/* ═══════════════════════════════════════════════════════════════════
   JOIN  (INNER / LEFT / RIGHT / FULL)
   ═══════════════════════════════════════════════════════════════════ */
void execute_join(JoinData *data) {
    if(!data){print_error("Invalid JOIN data");return;}
    Table *t1=db_get_table(global_db,data->table1);
    Table *t2=db_get_table(global_db,data->table2);
    if(!t1){char m[128];snprintf(m,sizeof(m),"Table '%s' not found",data->table1);print_error(m);return;}
    if(!t2){char m[128];snprintf(m,sizeof(m),"Table '%s' not found",data->table2);print_error(m);return;}
    int ci1=find_col(t1,data->col1),ci2=find_col(t2,data->col2);
    if(ci1<0){char m[128];snprintf(m,sizeof(m),"Column '%s' not in '%s'",data->col1,data->table1);print_error(m);return;}
    if(ci2<0){char m[128];snprintf(m,sizeof(m),"Column '%s' not in '%s'",data->col2,data->table2);print_error(m);return;}

    /* Header */
    printf("\n");
    for(int i=0;i<t1->col_count;i++){char h[MAX_NAME_LEN*2+2];snprintf(h,sizeof(h),"%s.%s",t1->name,t1->columns[i].name);printf("%-18s",h);}
    for(int i=0;i<t2->col_count;i++){char h[MAX_NAME_LEN*2+2];snprintf(h,sizeof(h),"%s.%s",t2->name,t2->columns[i].name);printf("%-18s",h);}
    printf("\n");
    for(int i=0;i<(t1->col_count+t2->col_count)*18;i++) printf("-"); printf("\n");

    int matched1[MAX_ROWS]={0}, matched2[MAX_ROWS]={0};
    int count=0;

    for(int r1=0;r1<t1->row_count;r1++){
        for(int r2=0;r2<t2->row_count;r2++){
            int match;
            if(t1->columns[ci1].type==TYPE_INT&&t2->columns[ci2].type==TYPE_INT)
                match=(t1->rows[r1].values[ci1].int_val==t2->rows[r2].values[ci2].int_val);
            else
                match=(strcmp(t1->rows[r1].values[ci1].str_val,t2->rows[r2].values[ci2].str_val)==0);
            if(!match) continue;
            matched1[r1]=1; matched2[r2]=1;
            for(int i=0;i<t1->col_count;i++) print_val(&t1->rows[r1].values[i],t1->columns[i].type,18);
            for(int i=0;i<t2->col_count;i++) print_val(&t2->rows[r2].values[i],t2->columns[i].type,18);
            printf("\n"); count++;
        }
    }

    /* LEFT JOIN: unmatched t1 rows */
    if(data->join_type==1||data->join_type==3){
        for(int r1=0;r1<t1->row_count;r1++){
            if(matched1[r1]) continue;
            for(int i=0;i<t1->col_count;i++) print_val(&t1->rows[r1].values[i],t1->columns[i].type,18);
            for(int i=0;i<t2->col_count;i++) printf("%-18s","NULL");
            printf("\n"); count++;
        }
    }
    /* RIGHT JOIN: unmatched t2 rows */
    if(data->join_type==2||data->join_type==3){
        for(int r2=0;r2<t2->row_count;r2++){
            if(matched2[r2]) continue;
            for(int i=0;i<t1->col_count;i++) printf("%-18s","NULL");
            for(int i=0;i<t2->col_count;i++) print_val(&t2->rows[r2].values[i],t2->columns[i].type,18);
            printf("\n"); count++;
        }
    }
    printf("\n%d row(s) returned\n\n",count);
}

/* ═══════════════════════════════════════════════════════════════════
   ALTER TABLE
   ═══════════════════════════════════════════════════════════════════ */
void execute_alter_add(AlterAddData *data) {
    if(!data){print_error("Invalid ALTER data");return;}
    Table *table=db_get_table(global_db,data->table_name);
    if(!table){char m[128];snprintf(m,sizeof(m),"Table '%s' does not exist",data->table_name);print_error(m);return;}
    if(table->col_count>=MAX_COLUMNS){print_error("Too many columns");return;}
    if(find_col(table,data->new_col.name)>=0){
        char m[128];snprintf(m,sizeof(m),"Column '%s' already exists",data->new_col.name);print_error(m);return;
    }
    table->columns[table->col_count]=data->new_col;
    for(int r=0;r<table->row_count;r++){
        Value *nv=realloc(table->rows[r].values,sizeof(Value)*(table->col_count+1));
        if(!nv){print_error("Memory error");return;}
        table->rows[r].values=nv;
        memset(&nv[table->col_count],0,sizeof(Value));
        nv[table->col_count].is_null=1;
        table->rows[r].col_count++;
    }
    table->col_count++;
    storage_save_table(table);
    char m[128];snprintf(m,sizeof(m),"Column '%s' added to '%s'",data->new_col.name,data->table_name);print_success(m);
}

void execute_alter_drop(AlterDropData *data) {
    if(!data){print_error("Invalid ALTER data");return;}
    Table *table=db_get_table(global_db,data->table_name);
    if(!table){char m[128];snprintf(m,sizeof(m),"Table '%s' does not exist",data->table_name);print_error(m);return;}
    int ci=find_col(table,data->col_name);
    if(ci<0){char m[128];snprintf(m,sizeof(m),"Column '%s' not found",data->col_name);print_error(m);return;}
    if(table->col_count<=1){print_error("Cannot drop the only column");return;}
    for(int i=ci;i<table->col_count-1;i++) table->columns[i]=table->columns[i+1];
    for(int r=0;r<table->row_count;r++){
        for(int i=ci;i<table->rows[r].col_count-1;i++) table->rows[r].values[i]=table->rows[r].values[i+1];
        table->rows[r].col_count--;
    }
    table->col_count--;
    storage_save_table(table);
    char m[128];snprintf(m,sizeof(m),"Column '%s' dropped",data->col_name);print_success(m);
}

/* ═══════════════════════════════════════════════════════════════════
   DROP TABLE
   ═══════════════════════════════════════════════════════════════════ */
void execute_drop_table(DropTableData *data) {
    if(!data){print_error("Invalid DROP TABLE data");return;}
    /* Remove from db */
    int found=-1;
    for(int i=0;i<global_db->table_count;i++){
        if(strcasecmp(global_db->tables[i]->name,data->table_name)==0){found=i;break;}
    }
    if(found<0){
        char m[128];snprintf(m,sizeof(m),"Table '%s' does not exist",data->table_name);print_error(m);return;
    }
    table_free(global_db->tables[found]);
    for(int i=found;i<global_db->table_count-1;i++) global_db->tables[i]=global_db->tables[i+1];
    global_db->table_count--;
    /* Delete file */
    char path[512];snprintf(path,sizeof(path),"data/%s.tbl",data->table_name);
    remove(path);
    char m[128];snprintf(m,sizeof(m),"Table '%s' dropped",data->table_name);print_success(m);
}

/* ═══════════════════════════════════════════════════════════════════
   RENAME TABLE
   ═══════════════════════════════════════════════════════════════════ */
void execute_rename_table(RenameTableData *data) {
    if(!data){print_error("Invalid RENAME data");return;}
    Table *table=db_get_table(global_db,data->old_name);
    if(!table){char m[128];snprintf(m,sizeof(m),"Table '%s' does not exist",data->old_name);print_error(m);return;}
    char old_path[512],new_path[512];
    snprintf(old_path,sizeof(old_path),"data/%s.tbl",data->old_name);
    snprintf(new_path,sizeof(new_path),"data/%s.tbl",data->new_name);
    strncpy(table->name,data->new_name,MAX_NAME_LEN-1);
    rename(old_path,new_path);
    storage_save_table(table);
    char m[128];snprintf(m,sizeof(m),"Table renamed to '%s'",data->new_name);print_success(m);
}
