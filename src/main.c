#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"
#include "executor.h"
#include "utils.h"

/* Bison parser function */
extern int yyparse();

/* From parser */
extern ASTNode *root;

int main() {
    char input[1024];

    printf("=================================\n");
    printf("      MiniDBMS SQL Engine\n");
    printf("=================================\n");
    printf("Type EXIT; to quit\n\n");

    while (1) {
        printf("MiniDB> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }

        trim_newline(input);

        /* Ignore empty input */
        if (strlen(input) == 0)
            continue;

        /* Feed input to lexer */
        YY_BUFFER_STATE buffer = yy_scan_string(input);
        yyparse();
        yy_delete_buffer(buffer);

        if (root) {
            if (semantic_check(root)) {
                if (!execute(root)) {
                    break;
                }
            }
            ast_free(root);
            root = NULL;
        }
    }

    return 0;
}
