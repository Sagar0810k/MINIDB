#include "../include/executor.h"
#include "../include/storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ── helpers ────────────────────────────────────────────────────── */

static int find_col(Table *t, const char *name) {
    for (int i = 0; i < t->col_count; i++)
        if (strcasecmp(t->columns[i].name, name) == 0) return i;
    return -1;
}

/* Compare a single condition */
static int eval_single(Table *t, Row *row, SingleCond *c) {
    int ci = find_col(t, c->col_name);
    if (ci < 0) return 0;
    if (c->is_text) {
        int cmp = strcmp(row->values[ci].str_val, c->val.str_val);
        switch (c->op) {
            case OP_EQ:  return cmp == 0;
            case OP_NEQ: return cmp != 0;
            case OP_LT:  return cmp <  0;
            case OP_GT:  return cmp >  0;
            case OP_LTE: return cmp <= 0;
            case OP_GTE: return cmp >= 0;
        }
    } else {
        int a = row->values[ci].int_val, b = c->val.int_val;
        switch (c->op) {
            case OP_EQ:  return a == b;
            case OP_NEQ: return a != b;
            case OP_LT:  return a <  b;
            case OP_GT:  return a >  b;
            case OP_LTE: return a <= b;
            case OP_GTE: return a >= b;
        }
    }
    return 0;
}

/* Evaluate full WHERE (AND/OR chain) */
static int row_matches(Table *t, Row *row, Condition *w) {
    if (!w->active) return 1;
    int result = eval_single(t, row, &w->conds[0]);
    for (int i = 1; i < w->count; i++) {
        int next = eval_single(t, row, &w->conds[i]);
        if (w->logic[i-1] == 0) result = result && next;   /* AND */
        else                     result = result || next;   /* OR  */
    }
    return result;
}

/* Compare two values for ORDER BY */
static int cmp_values(Value *a, Value *b, DataType type, int desc) {
    int r;
    if (type == TYPE_INT) r = (a->int_val > b->int_val) - (a->int_val < b->int_val);
    else                  r = strcmp(a->str_val, b->str_val);
    return desc ? -r : r;
}

/* ── dispatch ───────────────────────────────────────────────────── */
void execute_ast(ASTNode *node) {
    if (!node) { print_error("Invalid AST node"); return; }
    switch (node->type) {
        case AST_CREATE_TABLE: execute_create_table((CreateTableData*)node->data); break;
        case AST_INSERT:       execute_insert((InsertData*)node->data);            break;
        case AST_SELECT:       execute_select((SelectData*)node->data);            break;
        case AST_DELETE:       execute_delete((DeleteData*)node->data);            break;
        case AST_UPDATE:       execute_update((UpdateData*)node->data);            break;
        case AST_JOIN:         execute_join((JoinData*)node->data);                break;
        case AST_ALTER_ADD:    execute_alter_add((AlterAddData*)node->data);       break;
        case AST_ALTER_DROP:   execute_alter_drop((AlterDropData*)node->data);     break;
        default: print_error("Unknown AST node type");
    }
}

/* ── CREATE TABLE ───────────────────────────────────────────────── */
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

/* ── INSERT (with PRIMARY KEY / UNIQUE checks) ──────────────────── */
void execute_insert(InsertData *data) {
    if (!data) { print_error("Invalid INSERT data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }
    if (data->value_count != table->col_count) {
        print_error("Value count does not match column count"); return;
    }
    /* Check PRIMARY KEY and UNIQUE constraints */
    for (int ci = 0; ci < table->col_count; ci++) {
        if (!(table->columns[ci].flags & (COL_FLAG_PRIMARY_KEY | COL_FLAG_UNIQUE))) continue;
        for (int r = 0; r < table->row_count; r++) {
            int dup;
            if (table->columns[ci].type == TYPE_INT)
                dup = (table->rows[r].values[ci].int_val == data->values[ci].int_val);
            else
                dup = (strcmp(table->rows[r].values[ci].str_val, data->values[ci].str_val) == 0);
            if (dup) {
                char msg[128];
                snprintf(msg,sizeof(msg),"Duplicate value in %s column '%s'",
                    (table->columns[ci].flags & COL_FLAG_PRIMARY_KEY) ? "PRIMARY KEY" : "UNIQUE",
                    table->columns[ci].name);
                print_error(msg); return;
            }
        }
    }
    if (table_insert_row(table, data->values) < 0) { print_error("Failed to insert row"); return; }
    storage_save_table(table);
    print_success("Row inserted");
}

/* ── SELECT (columns, WHERE, ORDER BY, LIMIT, aggregates) ───────── */
void execute_select(SelectData *data) {
    if (!data) { print_error("Invalid SELECT data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }

    /* Build column index list */
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

    /* Collect matching row indices */
    int matched[MAX_ROWS], nm = 0;
    for (int r = 0; r < table->row_count; r++)
        if (row_matches(table, &table->rows[r], &data->where))
            matched[nm++] = r;

    /* ORDER BY: simple insertion sort on matched[] */
    if (data->has_order) {
        int oci = find_col(table, data->order_col);
        if (oci < 0) { char m[128]; snprintf(m,sizeof(m),"ORDER BY column '%s' not found",data->order_col); print_error(m); return; }
        for (int i = 1; i < nm; i++) {
            int key = matched[i], j = i-1;
            while (j >= 0 && cmp_values(&table->rows[matched[j]].values[oci],
                                        &table->rows[key].values[oci],
                                        table->columns[oci].type, data->order_desc) > 0) {
                matched[j+1] = matched[j]; j--;
            }
            matched[j+1] = key;
        }
    }

    /* LIMIT */
    if (data->limit >= 0 && nm > data->limit) nm = data->limit;

    /* Aggregates: COUNT / SUM / AVG */
    if (data->agg_type > 0) {
        int aci = -1;
        if (data->agg_type != 1) { /* not COUNT(*) */
            aci = find_col(table, data->agg_col);
            if (aci < 0) { char m[128]; snprintf(m,sizeof(m),"Column '%s' not found",data->agg_col); print_error(m); return; }
        }
        if (data->agg_type == 1) {          /* COUNT */
            printf("\nCOUNT\n-----\n%d\n\n1 row returned\n\n", nm);
        } else if (data->agg_type == 2) {   /* SUM */
            long long sum = 0;
            for (int i = 0; i < nm; i++) sum += table->rows[matched[i]].values[aci].int_val;
            printf("\nSUM(%s)\n", data->agg_col);
            for (int i=0; i<(int)strlen(data->agg_col)+5; i++) printf("-"); printf("\n");
            printf("%lld\n\n1 row returned\n\n", sum);
        } else if (data->agg_type == 3) {   /* AVG */
            double sum = 0;
            for (int i = 0; i < nm; i++) sum += table->rows[matched[i]].values[aci].int_val;
            printf("\nAVG(%s)\n", data->agg_col);
            for (int i=0; i<(int)strlen(data->agg_col)+5; i++) printf("-"); printf("\n");
            printf("%.2f\n\n1 row returned\n\n", nm > 0 ? sum/nm : 0.0);
        }
        return;
    }

    /* Normal print */
    printf("\n");
    for (int i = 0; i < ncols; i++) printf("%-15s", table->columns[idx[i]].name);
    printf("\n");
    for (int i = 0; i < ncols*15; i++) printf("-");
    printf("\n");
    for (int i = 0; i < nm; i++) {
        int r = matched[i];
        for (int j = 0; j < ncols; j++) {
            int ci = idx[j];
            if (table->columns[ci].type == TYPE_INT) printf("%-15d", table->rows[r].values[ci].int_val);
            else                                     printf("%-15s", table->rows[r].values[ci].str_val);
        }
        printf("\n");
    }
    printf("\n%d row(s) returned\n\n", nm);
}

/* ── DELETE ─────────────────────────────────────────────────────── */
void execute_delete(DeleteData *data) {
    if (!data) { print_error("Invalid DELETE data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }
    int deleted = 0, new_count = 0;
    for (int r = 0; r < table->row_count; r++) {
        if (row_matches(table, &table->rows[r], &data->where)) {
            free(table->rows[r].values); deleted++;
        } else {
            table->rows[new_count++] = table->rows[r];
        }
    }
    table->row_count = new_count;
    storage_save_table(table);
    char msg[64]; snprintf(msg,sizeof(msg),"%d row(s) deleted",deleted);
    print_success(msg);
}

/* ── UPDATE ─────────────────────────────────────────────────────── */
void execute_update(UpdateData *data) {
    if (!data) { print_error("Invalid UPDATE data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }
    int ci = find_col(table, data->set_col);
    if (ci < 0) {
        char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' not found",data->set_col);
        print_error(msg); return;
    }
    /* Check PK/UNIQUE constraint for updated value */
    if (table->columns[ci].flags & (COL_FLAG_PRIMARY_KEY | COL_FLAG_UNIQUE)) {
        for (int r = 0; r < table->row_count; r++) {
            if (row_matches(table, &table->rows[r], &data->where)) continue; /* will be updated */
            int dup;
            if (data->set_is_text)
                dup = (strcmp(table->rows[r].values[ci].str_val, data->set_val.str_val) == 0);
            else
                dup = (table->rows[r].values[ci].int_val == data->set_val.int_val);
            if (dup) { print_error("Update would violate UNIQUE/PRIMARY KEY constraint"); return; }
        }
    }
    int updated = 0;
    for (int r = 0; r < table->row_count; r++) {
        if (!row_matches(table, &table->rows[r], &data->where)) continue;
        if (data->set_is_text)
            strncpy(table->rows[r].values[ci].str_val, data->set_val.str_val, MAX_STRING_LEN-1);
        else
            table->rows[r].values[ci].int_val = data->set_val.int_val;
        updated++;
    }
    storage_save_table(table);
    char msg[64]; snprintf(msg,sizeof(msg),"%d row(s) updated",updated);
    print_success(msg);
}

/* ── JOIN ───────────────────────────────────────────────────────── */
void execute_join(JoinData *data) {
    if (!data) { print_error("Invalid JOIN data"); return; }
    Table *t1 = db_get_table(global_db, data->table1);
    Table *t2 = db_get_table(global_db, data->table2);
    if (!t1) { char m[128]; snprintf(m,sizeof(m),"Table '%s' not found",data->table1); print_error(m); return; }
    if (!t2) { char m[128]; snprintf(m,sizeof(m),"Table '%s' not found",data->table2); print_error(m); return; }
    int ci1 = find_col(t1, data->col1), ci2 = find_col(t2, data->col2);
    if (ci1 < 0) { char m[128]; snprintf(m,sizeof(m),"Column '%s' not found in '%s'",data->col1,data->table1); print_error(m); return; }
    if (ci2 < 0) { char m[128]; snprintf(m,sizeof(m),"Column '%s' not found in '%s'",data->col2,data->table2); print_error(m); return; }

    printf("\n");
    for (int i = 0; i < t1->col_count; i++) {
        char h[MAX_NAME_LEN*2+2]; snprintf(h,sizeof(h),"%s.%s",t1->name,t1->columns[i].name);
        printf("%-18s",h);
    }
    for (int i = 0; i < t2->col_count; i++) {
        char h[MAX_NAME_LEN*2+2]; snprintf(h,sizeof(h),"%s.%s",t2->name,t2->columns[i].name);
        printf("%-18s",h);
    }
    printf("\n");
    for (int i = 0; i < (t1->col_count+t2->col_count)*18; i++) printf("-");
    printf("\n");

    int count = 0;
    for (int r1 = 0; r1 < t1->row_count; r1++) {
        for (int r2 = 0; r2 < t2->row_count; r2++) {
            int match;
            if (t1->columns[ci1].type == TYPE_INT && t2->columns[ci2].type == TYPE_INT)
                match = (t1->rows[r1].values[ci1].int_val == t2->rows[r2].values[ci2].int_val);
            else
                match = (strcmp(t1->rows[r1].values[ci1].str_val, t2->rows[r2].values[ci2].str_val) == 0);
            if (!match) continue;
            for (int i = 0; i < t1->col_count; i++) {
                if (t1->columns[i].type == TYPE_INT) printf("%-18d", t1->rows[r1].values[i].int_val);
                else                                 printf("%-18s", t1->rows[r1].values[i].str_val);
            }
            for (int i = 0; i < t2->col_count; i++) {
                if (t2->columns[i].type == TYPE_INT) printf("%-18d", t2->rows[r2].values[i].int_val);
                else                                 printf("%-18s", t2->rows[r2].values[i].str_val);
            }
            printf("\n"); count++;
        }
    }
    printf("\n%d row(s) returned\n\n", count);
}

/* ── ALTER TABLE ADD COLUMN ─────────────────────────────────────── */
void execute_alter_add(AlterAddData *data) {
    if (!data) { print_error("Invalid ALTER data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }
    if (table->col_count >= MAX_COLUMNS) { print_error("Too many columns"); return; }
    if (find_col(table, data->new_col.name) >= 0) {
        char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' already exists",data->new_col.name);
        print_error(msg); return;
    }
    table->columns[table->col_count] = data->new_col;
    /* Add default value (0 / empty) to all existing rows */
    for (int r = 0; r < table->row_count; r++) {
        Value *newvals = realloc(table->rows[r].values, sizeof(Value)*(table->col_count+1));
        if (!newvals) { print_error("Memory error"); return; }
        table->rows[r].values = newvals;
        if (data->new_col.type == TYPE_INT) newvals[table->col_count].int_val = 0;
        else memset(newvals[table->col_count].str_val, 0, MAX_STRING_LEN);
        table->rows[r].col_count++;
    }
    table->col_count++;
    storage_save_table(table);
    char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' added to '%s'",data->new_col.name,data->table_name);
    print_success(msg);
}

/* ── ALTER TABLE DROP COLUMN ────────────────────────────────────── */
void execute_alter_drop(AlterDropData *data) {
    if (!data) { print_error("Invalid ALTER data"); return; }
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128]; snprintf(msg,sizeof(msg),"Table '%s' does not exist",data->table_name);
        print_error(msg); return;
    }
    int ci = find_col(table, data->col_name);
    if (ci < 0) {
        char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' not found",data->col_name);
        print_error(msg); return;
    }
    if (table->col_count <= 1) { print_error("Cannot drop the only column"); return; }
    /* Remove column from schema */
    for (int i = ci; i < table->col_count-1; i++) table->columns[i] = table->columns[i+1];
    /* Remove value from each row */
    for (int r = 0; r < table->row_count; r++) {
        for (int i = ci; i < table->rows[r].col_count-1; i++)
            table->rows[r].values[i] = table->rows[r].values[i+1];
        table->rows[r].col_count--;
    }
    table->col_count--;
    storage_save_table(table);
    char msg[128]; snprintf(msg,sizeof(msg),"Column '%s' dropped from '%s'",data->col_name,data->table_name);
    print_success(msg);
}
