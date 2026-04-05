#include "../include/utils.h"
#include <ctype.h>

/* Trim leading and trailing whitespace */
char* trim_whitespace(char *str) {
    char *end;
    
    /* Trim leading space */
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0)
        return str;
    
    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    /* Write new null terminator */
    end[1] = '\0';
    
    return str;
}

/* Print error message */
void print_error(const char *msg) {
    printf("ERROR: %s\n", msg);
}

/* Print success message */
void print_success(const char *msg) {
    printf("SUCCESS: %s\n", msg);
}

/* Convert data type to string */
char* data_type_to_string(DataType type) {
    switch(type) {
        case TYPE_INT:
            return "INT";
        case TYPE_TEXT:
            return "TEXT";
        default:
            return "UNKNOWN";
    }
}