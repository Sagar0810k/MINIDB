%{
#include <stdio.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/executor.h"
#include "parser.tab.h"

extern int yylineno;
%}

%option noyywrap
%option yylineno
%option case-insensitive

%%

[ \t\n]+                { /* Skip whitespace */ }
"--".*                  { /* Skip single-line comments */ }

CREATE                  { return CREATE; }
TABLE                   { return TABLE; }
INSERT                  { return INSERT; }
INTO                    { return INTO; }
SELECT                  { return SELECT; }
FROM                    { return FROM; }
VALUES                  { return VALUES; }
INT                     { return INT_TYPE; }
TEXT                    { return TEXT_TYPE; }
EXIT                    { return EXIT; }
QUIT                    { return EXIT; }
SHOW                    { return SHOW; }
TABLES                  { return TABLES; }

[a-zA-Z_][a-zA-Z0-9_]*  { 
                            yylval.str = strdup(yytext); 
                            return IDENTIFIER; 
                        }

[0-9]+                  { 
                            yylval.num = atoi(yytext); 
                            return NUMBER; 
                        }

'([^']|'')*'            { 
                            /* String literal with single quotes */
                            yylval.str = strndup(yytext + 1, strlen(yytext) - 2);
                            return STRING; 
                        }

"*"                     { return ASTERISK; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
","                     { return COMMA; }
";"                     { return SEMICOLON; }

.                       { 
                            printf("Unexpected character: %s\n", yytext); 
                            return yytext[0]; 
                        }

%%
