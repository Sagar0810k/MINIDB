#include "../include/executor.h"
#include "../include/storage.h"

/* Execute AST node */
void execute_ast(ASTNode *node) {
    if (!node) {
        print_error("Invalid AST node");
        return;
    }
    
    switch (node->type) {
        case AST_CREATE_TABLE:
            execute_create_table((CreateTableData*)node->data);
            break;
        case AST_INSERT:
            execute_insert((InsertData*)node->data);
            break;
        case AST_SELECT:
            execute_select((SelectData*)node->data);
            break;
        default:
            print_error("Unknown AST node type");
    }
}

/* Execute CREATE TABLE */
void execute_create_table(CreateTableData *data) {
    if (!data) {
        print_error("Invalid CREATE TABLE data");
        return;
    }
    
    /* Check if table already exists */
    if (storage_table_exists(data->table_name)) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Table '%s' already exists", data->table_name);
        print_error(msg);
        return;
    }
    
    /* Create table */
    Table *table = table_create(data->table_name, data->columns, data->col_count);
    if (!table) {
        print_error("Failed to create table");
        return;
    }
    
    /* Add to database */
    if (db_add_table(global_db, table) < 0) {
        table_free(table);
        return;
    }
    
    char msg[128];
    snprintf(msg, sizeof(msg), "Table '%s' created", data->table_name);
    print_success(msg);
}

/* Execute INSERT */
void execute_insert(InsertData *data) {
    if (!data) {
        print_error("Invalid INSERT data");
        return;
    }
    
    /* Get table */
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Table '%s' does not exist", data->table_name);
        print_error(msg);
        return;
    }
    
    /* Check value count */
    if (data->value_count != table->col_count) {
        print_error("Value count does not match column count");
        return;
    }
    
    /* Insert row */
    if (table_insert_row(table, data->values) < 0) {
        print_error("Failed to insert row");
        return;
    }
    
    /* Save to file */
    storage_save_table(table);
    
    print_success("Row inserted");
}

/* Execute SELECT */
void execute_select(SelectData *data) {
    if (!data) {
        print_error("Invalid SELECT data");
        return;
    }
    
    /* Get table */
    Table *table = db_get_table(global_db, data->table_name);
    if (!table) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Table '%s' does not exist", data->table_name);
        print_error(msg);
        return;
    }
    
    /* Print table */
    table_print(table);
}