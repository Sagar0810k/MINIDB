#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/db.h"
#include "../include/utils.h"

extern int yyparse();
extern FILE *yyin;
extern int should_exit;
extern int yylineno;

void print_banner() {
    printf("\n");
    printf("========================================\n");
    printf("         Mini DBMS v1.0                \n");
    printf("   Custom SQL Database Management      \n");
    printf("                                       \n");
    printf("   Type 'EXIT;' to quit                \n");
    printf("   Type 'SHOW TABLES;' to list tables \n");
    printf("========================================\n");
    printf("\n");
}

void print_help() {
    printf("\nSupported Commands:\n");
    printf("  CREATE TABLE table_name (col1 INT, col2 TEXT, ...);\n");
    printf("  INSERT INTO table_name VALUES (val1, 'val2', ...);\n");
    printf("  SELECT * FROM table_name;\n");
    printf("  SHOW TABLES;\n");
    printf("  EXIT;\n\n");
}

int main(int argc, char **argv) {
    /* Initialize global database */
    global_db = db_init();
    if (!global_db) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }
    
    print_banner();
    print_help();
    
    /* Interactive mode */
    char input[4096];
    char buffer[4096] = "";
    
    while (!should_exit) {
        printf("MiniDB> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        /* Accumulate input until semicolon */
        strcat(buffer, input);
        
        /* Check if we have a complete statement (ends with semicolon) */
        if (strchr(buffer, ';') != NULL) {
            /* Create temporary file for parsing */
            FILE *temp = tmpfile();
            if (!temp) {
                fprintf(stderr, "Failed to create temporary file\n");
                buffer[0] = '\0';
                continue;
            }
            
            fprintf(temp, "%s", buffer);
            rewind(temp);
            
            yyin = temp;
            yylineno = 1;
            
            /* Parse and execute */
            yyparse();
            
            fclose(temp);
            
            /* Clear buffer */
            buffer[0] = '\0';
        }
    }
    
    /* Cleanup */
    db_free(global_db);
    
    return 0;
}