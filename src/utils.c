#include "../include/utils.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char* trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

void print_error(const char *msg) {
    printf("\033[1;31m[ERROR]\033[0m %s\n", msg);
}

void print_success(const char *msg) {
    printf("\033[1;32m[OK]\033[0m %s\n", msg);
}

char* data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT:   return "INT";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_TEXT:  return "TEXT";
        case TYPE_DATE:  return "DATE";
        case TYPE_BOOL:  return "BOOL";
        default:         return "UNKNOWN";
    }
}

DataType string_to_data_type(const char *s) {
    if (strcasecmp(s,"INT")==0)   return TYPE_INT;
    if (strcasecmp(s,"FLOAT")==0) return TYPE_FLOAT;
    if (strcasecmp(s,"TEXT")==0)  return TYPE_TEXT;
    if (strcasecmp(s,"DATE")==0)  return TYPE_DATE;
    if (strcasecmp(s,"BOOL")==0)  return TYPE_BOOL;
    return TYPE_TEXT;
}

void str_upper(char *dst, const char *src) {
    while (*src) { *dst++ = toupper((unsigned char)*src++); }
    *dst = '\0';
}

void str_lower(char *dst, const char *src) {
    while (*src) { *dst++ = tolower((unsigned char)*src++); }
    *dst = '\0';
}

int str_length(const char *s) { return (int)strlen(s); }

void str_concat(char *dst, const char *a, const char *b, size_t max) {
    strncpy(dst, a, max-1); dst[max-1]='\0';
    strncat(dst, b, max-1-strlen(dst));
}

/* Simple LIKE: % matches any sequence, _ matches one char */
int like_match(const char *str, const char *pat) {
    if (*pat == '\0') return *str == '\0';
    if (*pat == '%') {
        while (*pat == '%') pat++;
        if (*pat == '\0') return 1;
        while (*str) {
            if (like_match(str, pat)) return 1;
            str++;
        }
        return 0;
    }
    if (*pat == '_' || tolower((unsigned char)*str) == tolower((unsigned char)*pat))
        return like_match(str+1, pat+1);
    return 0;
}
