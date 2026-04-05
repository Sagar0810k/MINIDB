%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/utils.h"
#include "../include/executor.h"
#include "parser.tab.h"

extern int yylineno;

#ifdef _WIN32
char* strndup(const char* s, size_t n) {
    size_t len = strlen(s); if (n < len) len = n;
    char* r = malloc(len+1); if (r){memcpy(r,s,len);r[len]='\0';} return r;
}
#endif
%}

%option noyywrap
%option yylineno
%option case-insensitive

%%

[ \t\n]+        { }
"--".*          { }

CREATE          { return CREATE; }
TABLE           { return TABLE; }
INSERT          { return INSERT; }
INTO            { return INTO; }
SELECT          { return SELECT; }
FROM            { return FROM; }
VALUES          { return VALUES; }
INT             { return INT_TYPE; }
TEXT            { return TEXT_TYPE; }
EXIT            { return EXIT; }
QUIT            { return EXIT; }
SHOW            { return SHOW; }
TABLES          { return TABLES; }
WHERE           { return WHERE; }
AND             { return AND; }
OR              { return OR; }
DELETE          { return DELETE; }
UPDATE          { return UPDATE; }
SET             { return SET; }
JOIN            { return JOIN; }
ON              { return ON; }
INNER           { return INNER; }
ORDER           { return ORDER; }
BY              { return BY; }
ASC             { return ASC; }
DESC            { return DESC; }
LIMIT           { return LIMIT; }
COUNT           { return COUNT; }
SUM             { return SUM; }
AVG             { return AVG; }
PRIMARY         { return PRIMARY; }
KEY             { return KEY; }
UNIQUE          { return UNIQUE; }
ALTER           { return ALTER; }
ADD             { return ADD; }
DROP            { return DROP; }
COLUMN          { return COLUMN; }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return IDENTIFIER; }
[0-9]+                   { yylval.num = atoi(yytext);   return NUMBER; }
'([^']|'')*'             { yylval.str = strndup(yytext+1,strlen(yytext)-2); return STRING; }

"*"   { return ASTERISK; }
"("   { return LPAREN; }
")"   { return RPAREN; }
","   { return COMMA; }
";"   { return SEMICOLON; }
"."   { return DOT; }
"<="  { return LTE; }
">="  { return GTE; }
"!="  { return NEQ; }
"<"   { return LT; }
">"   { return GT; }
"="   { return EQ; }

.  { printf("Unexpected character: %s\n", yytext); return yytext[0]; }

%%
