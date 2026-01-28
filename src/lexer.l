%{
#include "parser.tab.h"
#include <string.h>
%}

/* ===============================
   Flex options
   =============================== */
%option noyywrap
%option case-insensitive

/* ===============================
   Regex Definitions
   =============================== */
DIGIT      [0-9]
ID         [a-zA-Z_][a-zA-Z0-9_]*
STRING     \'[^\']*\'

%%

/* ===============================
   SQL Keywords
   =============================== */

"create"        { return CREATE; }
"table"         { return TABLE; }
"insert"        { return INSERT; }
"into"          { return INTO; }
"values"        { return VALUES; }
"select"        { return SELECT; }
"from"          { return FROM; }
"delete"        { return DELETE; }
"begin"         { return BEGIN_T; }
"commit"        { return COMMIT; }
"rollback"      { return ROLLBACK; }
"exit"          { return EXIT_T; }

/* ===============================
   Data Types
   =============================== */

"int"           { return INT_T; }
"string"        { return STRING_T; }

/* ===============================
   Identifiers
   =============================== */

{ID} {
    yylval.str = strdup(yytext);
    return IDENTIFIER;
}

/* ===============================
   Numbers
   =============================== */

{DIGIT}+ {
    yylval.str = strdup(yytext);
    return NUMBER;
}

/* ===============================
   Strings
   =============================== */

{STRING} {
    yylval.str = strdup(yytext + 1);   /* remove ' */
    yylval.str[strlen(yylval.str) - 1] = '\0';
    return STRING;
}

/* ===============================
   Symbols
   =============================== */

","     { return ','; }
";"     { return ';'; }
"("     { return '('; }
")"     { return ')'; }

/* ===============================
   Ignore spaces & newlines
   =============================== */

[ \t\r\n]+   { /* skip */ }

/* ===============================
   Unknown character
   =============================== */

. {
    printf("Unknown character: %s\n", yytext);
}

%%
