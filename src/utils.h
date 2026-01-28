#ifndef UTILS_H
#define UTILS_H

/* ===============================
   File Utilities
   =============================== */

int file_exists(const char *filename);

/* ===============================
   String Utilities
   =============================== */

void trim_newline(char *str);
void trim_whitespace(char *str);

/* ===============================
   Error Utility
   =============================== */

void db_error(const char *message);

#endif /* UTILS_H */
