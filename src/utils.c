#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/* ===============================
   Check if file exists
   =============================== */

int file_exists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp) {
        fclose(fp);
        return 1;
    }
    return 0;
}

/* ===============================
   Remove trailing newline
   =============================== */

void trim_newline(char *str) {
    if (!str) return;
    str[strcspn(str, "\n")] = '\0';
}

/* ===============================
   Trim leading and trailing spaces
   =============================== */

void trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
}

/* ===============================
   Print DB error
   =============================== */

void db_error(const char *message) {
    fprintf(stderr, "MiniDB Error: %s\n", message);
}
