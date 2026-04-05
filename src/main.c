#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/db.h"
#include "../include/utils.h"

extern int yyparse();
extern FILE *yyin;
extern int should_exit;
extern int yylineno;

/* Globals for ORDER BY / LIMIT / aggregates (set in parser actions) */
char g_order_col[64] = "";
int  g_order_desc    = 0;
int  g_has_order     = 0;
int  g_limit         = -1;
int  g_agg_type      = 0;
char g_agg_col[64]   = "";

void print_banner() {
    printf("\n");
    printf("============================================\n");
    printf("         MiniDB v2.0                        \n");
    printf("   Custom SQL Database Management           \n");
    printf("   Type 'HELP;' for commands, 'EXIT;' quit  \n");
    printf("============================================\n\n");
}

void print_help() {
    printf("Commands:\n");
    printf("  CREATE TABLE t (id INT PRIMARY KEY, name TEXT UNIQUE);\n");
    printf("  INSERT INTO t VALUES (1, 'Alice');\n");
    printf("  SELECT * FROM t;\n");
    printf("  SELECT col1, col2 FROM t;\n");
    printf("  SELECT * FROM t WHERE age > 18 AND name = 'Ali';\n");
    printf("  SELECT * FROM t WHERE age < 30 OR name = 'Bob';\n");
    printf("  SELECT * FROM t ORDER BY age DESC LIMIT 5;\n");
    printf("  SELECT COUNT(*) FROM t;\n");
    printf("  SELECT SUM(age) FROM t WHERE active = 1;\n");
    printf("  SELECT AVG(score) FROM t;\n");
    printf("  SELECT * FROM t1 INNER JOIN t2 ON t1.id = t2.tid;\n");
    printf("  UPDATE t SET age = 25 WHERE name = 'Ali';\n");
    printf("  DELETE FROM t WHERE id = 3;\n");
    printf("  ALTER TABLE t ADD COLUMN email TEXT;\n");
    printf("  ALTER TABLE t DROP COLUMN email;\n");
    printf("  SHOW TABLES;\n");
    printf("  EXIT;\n\n");
}

int main(int argc, char **argv) {
    global_db = db_init();
    if (!global_db) { fprintf(stderr,"Failed to initialize database\n"); return 1; }

    print_banner();
    print_help();

    char input[4096], buffer[4096] = "";
    while (!should_exit) {
        printf("MiniDB> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        /* built-in HELP command */
        char tmp[16]; sscanf(input, "%15s", tmp);
        if (strcasecmp(tmp,"help;")==0||strcasecmp(tmp,"help")==0){ print_help(); continue; }

        strcat(buffer, input);
        if (strchr(buffer,';') != NULL) {
            FILE *temp = tmpfile();
            if (!temp) { fprintf(stderr,"tmpfile failed\n"); buffer[0]='\0'; continue; }
            fprintf(temp,"%s",buffer);
            rewind(temp);
            yyin = temp; yylineno = 1;
            yyparse();
            fclose(temp);
            buffer[0] = '\0';
        }
    }
    db_free(global_db);
    return 0;
}
