
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CREATE = 258,
     TABLE = 259,
     INSERT = 260,
     INTO = 261,
     SELECT = 262,
     FROM = 263,
     VALUES = 264,
     INT_TYPE = 265,
     TEXT_TYPE = 266,
     EXIT = 267,
     SHOW = 268,
     TABLES = 269,
     WHERE = 270,
     AND = 271,
     OR = 272,
     DELETE = 273,
     UPDATE = 274,
     SET = 275,
     JOIN = 276,
     ON = 277,
     INNER = 278,
     ORDER = 279,
     BY = 280,
     ASC = 281,
     DESC = 282,
     LIMIT = 283,
     COUNT = 284,
     SUM = 285,
     AVG = 286,
     PRIMARY = 287,
     KEY = 288,
     UNIQUE = 289,
     ALTER = 290,
     ADD = 291,
     DROP = 292,
     COLUMN = 293,
     LPAREN = 294,
     RPAREN = 295,
     COMMA = 296,
     SEMICOLON = 297,
     ASTERISK = 298,
     DOT = 299,
     EQ = 300,
     NEQ = 301,
     LT = 302,
     GT = 303,
     LTE = 304,
     GTE = 305,
     NUMBER = 306,
     IDENTIFIER = 307,
     STRING = 308
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 25 "parser.y"

    int num;
    char *str;
    Column *col;
    Column *col_list;
    Value *val;
    Value *val_list;
    Condition cond;
    SingleCond scond;



/* Line 1676 of yacc.c  */
#line 118 "..\\build\\parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


