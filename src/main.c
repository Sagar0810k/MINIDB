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
    printf("==============================================\n");
    printf("                    MiniDB                    \n");
    printf("   Type HELP; for commands  |  EXIT; to quit \n");
    printf("==============================================\n\n");
}

void print_help() {
    printf("=================================================================\n");
    printf(" DDL\n");
    printf("  CREATE TABLE t (id INT PRIMARY KEY,\n");
    printf("    name TEXT NOT NULL,\n");
    printf("    score FLOAT DEFAULT 0,\n");
    printf("    active BOOL);\n");
    printf("  ALTER TABLE t ADD COLUMN col TEXT;\n");
    printf("  ALTER TABLE t DROP COLUMN col;\n");
    printf("  DROP TABLE t;\n");
    printf("  RENAME TABLE t TO new_name;\n");
    printf("  SHOW TABLES;\n\n");

    printf(" DML\n");
    printf("  INSERT INTO t VALUES (1,'Ali',9.5,1);\n");
    printf("  INSERT INTO t (id,name) VALUES (2,'Bob');\n");
    printf("  UPDATE t SET col=val, col2=val2 WHERE ...;\n");
    printf("  DELETE FROM t WHERE ...;\n\n");

    printf(" SELECT\n");
    printf("  SELECT * FROM t;\n");
    printf("  SELECT col1,col2 FROM t WHERE age BETWEEN 18 AND 30;\n");
    printf("  SELECT * FROM t WHERE name LIKE 'A%%';\n");
    printf("  SELECT * FROM t WHERE id IN (1,2,3);\n");
    printf("  SELECT * FROM t WHERE col IS NULL;\n");
    printf("  SELECT * FROM t WHERE a>1 AND b<10 OR c=5;\n");
    printf("  SELECT * FROM t ORDER BY col DESC LIMIT 10;\n\n");

    printf(" AGGREGATES\n");
    printf("  SELECT COUNT(*) FROM t WHERE active=1;\n");
    printf("  SELECT SUM(score)/AVG(score)/MIN(score)/MAX(score) FROM t;\n");
    printf("  SELECT COUNT(*) FROM t GROUP BY dept HAVING COUNT(*) > 2;\n\n");

    printf(" JOINS\n");
    printf("  SELECT * FROM t1 INNER JOIN t2 ON t1.id = t2.tid;\n");
    printf("  SELECT * FROM t1 LEFT OUTER JOIN t2 ON t1.id = t2.tid;\n");
    printf("  SELECT * FROM t1 RIGHT OUTER JOIN t2 ON t1.id = t2.tid;\n");
    printf("  SELECT * FROM t1 FULL OUTER JOIN t2 ON t1.id = t2.tid;\n\n");

    printf(" STRING FUNCTIONS\n");
    printf("  SELECT UPPER(name) FROM t;\n");
    printf("  SELECT LOWER(name) FROM t;\n");
    printf("  SELECT LENGTH(name) FROM t;\n");
    printf("=================================================================\n\n");
}

int main(int argc, char **argv) {
    global_db = db_init();
    if (!global_db) { fprintf(stderr,"Failed to initialize database\n"); return 1; }

    print_banner();

    char input[4096], buffer[4096]="";
    while (!should_exit) {
        printf("MiniDB> "); fflush(stdout);
        if (fgets(input,sizeof(input),stdin)==NULL) break;

        char tmp[16]; sscanf(input,"%15s",tmp);
        if (strcasecmp(tmp,"help;")==0||strcasecmp(tmp,"help")==0){ print_help(); continue; }

        strcat(buffer,input);
        if (strchr(buffer,';')!=NULL) {
            FILE *temp=tmpfile();
            if (!temp){ buffer[0]='\0'; continue; }
            fprintf(temp,"%s",buffer);
            rewind(temp);
            yyin=temp; yylineno=1;
            yyparse();
            fclose(temp);
            buffer[0]='\0';
        }
    }
    db_free(global_db);
    return 0;
}
