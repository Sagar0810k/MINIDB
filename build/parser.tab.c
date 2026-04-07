
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/db.h"
#include "../include/executor.h"

extern int yylex();
extern FILE *yyin;
extern int yylineno;
void yyerror(const char *s);
int should_exit = 0;

/* helpers to build Condition */
static void cond_init(Condition *c) { memset(c,0,sizeof(*c)); c->active=0; }
static void cond_add(Condition *c, SingleCond sc, int logic) {
    if (c->count >= MAX_CONDITIONS) return;
    if (c->count > 0) c->logic[c->count-1] = logic;
    c->conds[c->count++] = sc;
    c->active = 1;
}


/* Line 189 of yacc.c  */
#line 98 "..\\build\\parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
#line 25 "parser.y"

    int num;
    char *str;
    Column *col;
    Column *col_list;
    Value *val;
    Value *val_list;
    Condition cond;
    SingleCond scond;



/* Line 214 of yacc.c  */
#line 200 "..\\build\\parser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 212 "..\\build\\parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   139

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNRULES -- Number of states.  */
#define YYNSTATES  156

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   308

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    26,    29,    33,    41,    43,    47,    51,
      55,    56,    59,    61,    70,    72,    76,    78,    80,    89,
      98,   105,   118,   123,   128,   133,   138,   140,   144,   148,
     150,   151,   155,   160,   165,   166,   169,   175,   184,   193,
     202,   210,   211,   213,   216,   220,   224,   228,   232,   236,
     240,   244,   248,   252,   256,   260
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    -1,    55,    56,    -1,    59,    -1,    63,
      -1,    66,    -1,    72,    -1,    73,    -1,    74,    -1,    58,
      -1,    57,    -1,     1,    42,    -1,    12,    42,    -1,    13,
      14,    42,    -1,     3,     4,    52,    39,    60,    40,    42,
      -1,    61,    -1,    60,    41,    61,    -1,    52,    10,    62,
      -1,    52,    11,    62,    -1,    -1,    32,    33,    -1,    34,
      -1,     5,     6,    52,     9,    39,    64,    40,    42,    -1,
      65,    -1,    64,    41,    65,    -1,    51,    -1,    53,    -1,
       7,    43,     8,    52,    75,    70,    71,    42,    -1,     7,
      68,     8,    52,    75,    70,    71,    42,    -1,     7,    67,
       8,    52,    75,    42,    -1,     7,    43,     8,    52,    23,
      21,    52,    22,    69,    45,    69,    42,    -1,    29,    39,
      43,    40,    -1,    29,    39,    52,    40,    -1,    30,    39,
      52,    40,    -1,    31,    39,    52,    40,    -1,    52,    -1,
      68,    41,    52,    -1,    52,    44,    52,    -1,    52,    -1,
      -1,    24,    25,    52,    -1,    24,    25,    52,    26,    -1,
      24,    25,    52,    27,    -1,    -1,    28,    51,    -1,    18,
       8,    52,    75,    42,    -1,    19,    52,    20,    52,    45,
      65,    75,    42,    -1,    35,     4,    52,    36,    38,    52,
      10,    42,    -1,    35,     4,    52,    36,    38,    52,    11,
      42,    -1,    35,     4,    52,    37,    38,    52,    42,    -1,
      -1,    76,    -1,    15,    77,    -1,    76,    16,    77,    -1,
      76,    17,    77,    -1,    52,    45,    51,    -1,    52,    46,
      51,    -1,    52,    47,    51,    -1,    52,    48,    51,    -1,
      52,    49,    51,    -1,    52,    50,    51,    -1,    52,    45,
      53,    -1,    52,    46,    53,    -1,    52,    47,    53,    -1,
      52,    48,    53,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    60,    60,    61,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    76,    77,    81,    93,    96,   103,   109,
     118,   119,   120,   125,   141,   142,   150,   151,   157,   170,
     187,   198,   211,   215,   219,   223,   230,   231,   238,   242,
     247,   249,   251,   253,   258,   259,   264,   274,   287,   294,
     301,   312,   313,   317,   320,   323,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CREATE", "TABLE", "INSERT", "INTO",
  "SELECT", "FROM", "VALUES", "INT_TYPE", "TEXT_TYPE", "EXIT", "SHOW",
  "TABLES", "WHERE", "AND", "OR", "DELETE", "UPDATE", "SET", "JOIN", "ON",
  "INNER", "ORDER", "BY", "ASC", "DESC", "LIMIT", "COUNT", "SUM", "AVG",
  "PRIMARY", "KEY", "UNIQUE", "ALTER", "ADD", "DROP", "COLUMN", "LPAREN",
  "RPAREN", "COMMA", "SEMICOLON", "ASTERISK", "DOT", "EQ", "NEQ", "LT",
  "GT", "LTE", "GTE", "NUMBER", "IDENTIFIER", "STRING", "$accept",
  "statement_list", "statement", "exit_stmt", "show_tables_stmt",
  "create_table_stmt", "column_defs", "column_def", "col_constraint",
  "insert_stmt", "value_list", "value", "select_stmt", "agg_expr",
  "col_name_list", "col_ref", "opt_order", "opt_limit", "delete_stmt",
  "update_stmt", "alter_stmt", "opt_where", "where_clause", "single_cond", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    57,    58,    59,    60,    60,    61,    61,
      62,    62,    62,    63,    64,    64,    65,    65,    66,    66,
      66,    66,    67,    67,    67,    67,    68,    68,    69,    69,
      70,    70,    70,    70,    71,    71,    72,    73,    74,    74,
      74,    75,    75,    76,    76,    76,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     3,     7,     1,     3,     3,     3,
       0,     2,     1,     8,     1,     3,     1,     1,     8,     8,
       6,    12,     4,     4,     4,     4,     1,     3,     3,     1,
       0,     3,     4,     4,     0,     2,     5,     8,     8,     8,
       7,     0,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,    11,    10,     4,     5,     6,     7,     8,
       9,    12,     0,     0,     0,     0,     0,     0,    36,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,    51,     0,     0,     0,
       0,     0,     0,     0,     0,    51,    51,    51,    37,     0,
       0,    52,     0,     0,     0,     0,     0,    16,     0,    32,
      33,    34,    35,     0,    40,     0,    40,     0,    53,    46,
       0,     0,     0,     0,     0,    20,    20,     0,     0,    26,
      27,     0,    24,     0,     0,    44,    30,    44,     0,     0,
       0,     0,     0,     0,    54,    55,    51,     0,     0,     0,
      22,    18,    19,    15,    17,     0,     0,     0,     0,     0,
       0,     0,    56,    62,    57,    63,    58,    64,    59,    65,
      60,    61,     0,     0,     0,    50,    21,    23,    25,     0,
      41,    45,    28,    29,    47,    48,    49,    39,     0,    42,
      43,     0,     0,    38,     0,    31
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,    14,    15,    66,    67,   111,    16,
      91,    92,    17,    29,    30,   148,    95,   120,    18,    19,
      20,    60,    61,    78
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -78
static const yytype_int8 yypact[] =
{
     -78,     3,   -78,   -28,    19,    11,   -18,   -22,    22,    54,
      15,    64,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,    17,    20,    31,    32,    34,    66,   -78,    67,
      -1,   -78,    35,    24,    58,    27,    41,    72,   -34,    30,
      33,    38,    39,    40,    42,   -78,    68,    43,    13,    45,
      47,    48,    49,    53,    56,     4,    68,    68,   -78,    46,
      57,    36,    55,    63,    65,    44,    16,   -78,   -27,   -78,
     -78,   -78,   -78,    81,    60,    62,    60,   -17,   -78,   -78,
      46,    46,   -27,    59,    61,     9,     9,    70,    45,   -78,
     -78,    18,   -78,    69,    80,    78,   -78,    78,   -16,    -9,
      -6,    -5,    71,    73,   -78,   -78,    68,    50,    74,    75,
     -78,   -78,   -78,   -78,   -78,    76,   -27,    85,    77,    79,
      83,    84,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,    86,    89,    90,   -78,   -78,   -78,   -78,    82,
      37,   -78,   -78,   -78,   -78,   -78,   -78,    91,    88,   -78,
     -78,    87,    82,   -78,    94,   -78
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -78,   -78,   -78,   -78,   -78,   -78,   -78,    21,     1,   -78,
     -78,   -77,   -78,   -78,   -78,   -42,    51,    23,   -78,   -78,
     -78,   -55,   -78,   -15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      74,    75,    76,     2,     3,   106,     4,    43,     5,    51,
       6,    24,    25,    26,    21,     7,     8,    23,    52,    59,
      31,     9,    10,    22,    89,    27,    90,    73,    98,    99,
     100,   101,   102,   103,    28,   122,    32,   123,    11,   138,
      44,   109,   124,   110,   125,   126,   128,   127,   129,    63,
      64,   132,    80,    81,    85,    86,    87,    88,   115,   116,
     133,   134,    33,   149,   150,   104,   105,    34,    35,    36,
      38,    39,    37,    40,    41,    42,    46,    45,    47,    48,
      49,    50,    53,    59,    94,    54,    68,   112,    69,    70,
      55,    56,    57,    71,    58,    62,    72,    65,    77,    79,
      82,    83,    93,    84,    96,   118,   119,   139,   136,   114,
     154,   107,   113,   108,     0,     0,   135,     0,   137,     0,
     121,   117,   130,     0,   131,   142,   143,    97,   144,   140,
     141,   145,   146,   152,   147,   151,   155,     0,     0,   153
};

static const yytype_int16 yycheck[] =
{
      55,    56,    57,     0,     1,    82,     3,     8,     5,    43,
       7,    29,    30,    31,    42,    12,    13,     6,    52,    15,
      42,    18,    19,     4,    51,    43,    53,    23,    45,    46,
      47,    48,    49,    50,    52,    51,    14,    53,    35,   116,
      41,    32,    51,    34,    53,    51,    51,    53,    53,    36,
      37,   106,    16,    17,    10,    11,    40,    41,    40,    41,
      10,    11,     8,    26,    27,    80,    81,    52,     4,    52,
      39,    39,    52,    39,     8,     8,    52,    42,    20,    52,
      39,     9,    52,    15,    24,    52,    39,    86,    40,    40,
      52,    52,    52,    40,    52,    52,    40,    52,    52,    42,
      45,    38,    21,    38,    42,    25,    28,    22,    33,    88,
     152,    52,    42,    52,    -1,    -1,    42,    -1,    42,    -1,
      97,    52,    51,    -1,    51,    42,    42,    76,    42,    52,
      51,    42,    42,    45,    52,    44,    42,    -1,    -1,    52
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    55,     0,     1,     3,     5,     7,    12,    13,    18,
      19,    35,    56,    57,    58,    59,    63,    66,    72,    73,
      74,    42,     4,     6,    29,    30,    31,    43,    52,    67,
      68,    42,    14,     8,    52,     4,    52,    52,    39,    39,
      39,     8,     8,     8,    41,    42,    52,    20,    52,    39,
       9,    43,    52,    52,    52,    52,    52,    52,    52,    15,
      75,    76,    52,    36,    37,    52,    60,    61,    39,    40,
      40,    40,    40,    23,    75,    75,    75,    52,    77,    42,
      16,    17,    45,    38,    38,    10,    11,    40,    41,    51,
      53,    64,    65,    21,    24,    70,    42,    70,    45,    46,
      47,    48,    49,    50,    77,    77,    65,    52,    52,    32,
      34,    62,    62,    42,    61,    40,    41,    52,    25,    28,
      71,    71,    51,    53,    51,    53,    51,    53,    51,    53,
      51,    51,    75,    10,    11,    42,    33,    42,    65,    22,
      52,    51,    42,    42,    42,    42,    42,    52,    69,    26,
      27,    44,    45,    52,    69,    42
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 12:

/* Line 1455 of yacc.c  */
#line 73 "parser.y"
    { yyerrok; ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 76 "parser.y"
    { should_exit=1; printf("Goodbye!\n"); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 77 "parser.y"
    { db_list_tables(global_db); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 81 "parser.y"
    {
        CreateTableData *d = calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(3) - (7)].str),MAX_NAME_LEN-1);
        d->columns = (yyvsp[(5) - (7)].col_list); d->col_count=0;
        Column *t=(yyvsp[(5) - (7)].col_list); while(t&&d->col_count<MAX_COLUMNS&&strlen(t->name)>0){d->col_count++;t++;}
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_CREATE_TABLE; n->data=d;
        execute_ast(n);
        free((yyvsp[(3) - (7)].str)); free(d->columns); free(d); free(n);
    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 93 "parser.y"
    {
        (yyval.col_list)=calloc(MAX_COLUMNS,sizeof(Column)); (yyval.col_list)[0]=*(yyvsp[(1) - (1)].col); free((yyvsp[(1) - (1)].col));
    ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 96 "parser.y"
    {
        int n=0; while(n<MAX_COLUMNS&&strlen((yyvsp[(1) - (3)].col_list)[n].name)>0)n++;
        (yyvsp[(1) - (3)].col_list)[n]=*(yyvsp[(3) - (3)].col); free((yyvsp[(3) - (3)].col)); (yyval.col_list)=(yyvsp[(1) - (3)].col_list);
    ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 103 "parser.y"
    {
        Column *c=calloc(1,sizeof(*c));
        strncpy(c->name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); c->type=TYPE_INT;
        c->flags = (yyvsp[(3) - (3)].col) ? (yyvsp[(3) - (3)].col)->flags : 0;
        if((yyvsp[(3) - (3)].col))free((yyvsp[(3) - (3)].col)); free((yyvsp[(1) - (3)].str)); (yyval.col)=c;
    ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 109 "parser.y"
    {
        Column *c=calloc(1,sizeof(*c));
        strncpy(c->name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); c->type=TYPE_TEXT;
        c->flags = (yyvsp[(3) - (3)].col) ? (yyvsp[(3) - (3)].col)->flags : 0;
        if((yyvsp[(3) - (3)].col))free((yyvsp[(3) - (3)].col)); free((yyvsp[(1) - (3)].str)); (yyval.col)=c;
    ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 118 "parser.y"
    { (yyval.col)=NULL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 119 "parser.y"
    { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_PRIMARY_KEY|COL_FLAG_UNIQUE; (yyval.col)=c; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 120 "parser.y"
    { Column *c=calloc(1,sizeof(*c)); c->flags=COL_FLAG_UNIQUE; (yyval.col)=c; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 125 "parser.y"
    {
        InsertData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(3) - (8)].str),MAX_NAME_LEN-1);
        d->values=(yyvsp[(6) - (8)].val_list); d->value_count=0;
        Value *t=(yyvsp[(6) - (8)].val_list);
        while(d->value_count<MAX_COLUMNS){
            d->value_count++; t++;
            if(t->int_val==0&&strlen(t->str_val)==0)break;
        }
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_INSERT; n->data=d;
        execute_ast(n);
        free((yyvsp[(3) - (8)].str)); free(d->values); free(d); free(n);
    ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 141 "parser.y"
    { (yyval.val_list)=calloc(MAX_COLUMNS,sizeof(Value)); (yyval.val_list)[0]=*(yyvsp[(1) - (1)].val); free((yyvsp[(1) - (1)].val)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 142 "parser.y"
    {
        int n=0;
        while(n<MAX_COLUMNS&&!((yyvsp[(1) - (3)].val_list)[n].int_val==0&&strlen((yyvsp[(1) - (3)].val_list)[n].str_val)==0))n++;
        (yyvsp[(1) - (3)].val_list)[n]=*(yyvsp[(3) - (3)].val); free((yyvsp[(3) - (3)].val)); (yyval.val_list)=(yyvsp[(1) - (3)].val_list);
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 150 "parser.y"
    { Value *v=calloc(1,sizeof(*v)); v->int_val=(yyvsp[(1) - (1)].num); (yyval.val)=v; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 151 "parser.y"
    { Value *v=calloc(1,sizeof(*v)); strncpy(v->str_val,(yyvsp[(1) - (1)].str),MAX_STRING_LEN-1); (yyval.val)=v; free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 157 "parser.y"
    {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(4) - (8)].str),MAX_NAME_LEN-1);
        d->select_all=1; d->where=(yyvsp[(5) - (8)].cond);
        /* opt_order and opt_limit stored in globals then copied */
        extern char g_order_col[]; extern int g_order_desc; extern int g_has_order; extern int g_limit;
        strncpy(d->order_col,g_order_col,MAX_NAME_LEN-1);
        d->order_desc=g_order_desc; d->has_order=g_has_order; d->limit=g_limit;
        d->agg_type=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free((yyvsp[(4) - (8)].str)); free(d); free(n);
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 170 "parser.y"
    {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(4) - (8)].str),MAX_NAME_LEN-1);
        d->select_all=0; d->where=(yyvsp[(5) - (8)].cond);
        extern char g_order_col[]; extern int g_order_desc; extern int g_has_order; extern int g_limit;
        strncpy(d->order_col,g_order_col,MAX_NAME_LEN-1);
        d->order_desc=g_order_desc; d->has_order=g_has_order; d->limit=g_limit;
        d->agg_type=0;
        int cnt=0;
        while(cnt<MAX_COLUMNS&&strlen((yyvsp[(2) - (8)].val_list)[cnt].str_val)>0){
            strncpy(d->col_names[cnt],(yyvsp[(2) - (8)].val_list)[cnt].str_val,MAX_NAME_LEN-1); cnt++;
        }
        d->col_count=cnt; free((yyvsp[(2) - (8)].val_list));
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free((yyvsp[(4) - (8)].str)); free(d); free(n);
    ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 187 "parser.y"
    {
        SelectData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(4) - (6)].str),MAX_NAME_LEN-1);
        d->select_all=1; d->where=(yyvsp[(5) - (6)].cond); d->has_order=0; d->limit=-1;
        extern int g_agg_type; extern char g_agg_col[];
        d->agg_type=g_agg_type;
        strncpy(d->agg_col,g_agg_col,MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_SELECT; n->data=d;
        execute_ast(n); free((yyvsp[(4) - (6)].str)); free((yyvsp[(2) - (6)].str)); free(d); free(n);
    ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 198 "parser.y"
    {
        JoinData *d=calloc(1,sizeof(*d));
        strncpy(d->table1,(yyvsp[(4) - (12)].str),MAX_NAME_LEN-1); strncpy(d->table2,(yyvsp[(7) - (12)].str),MAX_NAME_LEN-1);
        char *dot1=strchr((yyvsp[(9) - (12)].str),'.'), *dot2=strchr((yyvsp[(11) - (12)].str),'.');
        strncpy(d->col1,dot1?dot1+1:(yyvsp[(9) - (12)].str), MAX_NAME_LEN-1);
        strncpy(d->col2,dot2?dot2+1:(yyvsp[(11) - (12)].str),MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_JOIN; n->data=d;
        execute_ast(n); free((yyvsp[(4) - (12)].str)); free((yyvsp[(7) - (12)].str)); free((yyvsp[(9) - (12)].str)); free((yyvsp[(11) - (12)].str)); free(d); free(n);
    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 211 "parser.y"
    {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=1; strcpy(g_agg_col,"*"); (yyval.str)=strdup("COUNT(*)");
    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 215 "parser.y"
    {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=1; strncpy(g_agg_col,(yyvsp[(3) - (4)].str),MAX_NAME_LEN-1); (yyval.str)=strdup((yyvsp[(3) - (4)].str)); free((yyvsp[(3) - (4)].str));
    ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 219 "parser.y"
    {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=2; strncpy(g_agg_col,(yyvsp[(3) - (4)].str),MAX_NAME_LEN-1); (yyval.str)=strdup((yyvsp[(3) - (4)].str)); free((yyvsp[(3) - (4)].str));
    ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 223 "parser.y"
    {
        extern int g_agg_type; extern char g_agg_col[];
        g_agg_type=3; strncpy(g_agg_col,(yyvsp[(3) - (4)].str),MAX_NAME_LEN-1); (yyval.str)=strdup((yyvsp[(3) - (4)].str)); free((yyvsp[(3) - (4)].str));
    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 230 "parser.y"
    { (yyval.val_list)=calloc(MAX_COLUMNS,sizeof(Value)); strncpy((yyval.val_list)[0].str_val,(yyvsp[(1) - (1)].str),MAX_NAME_LEN-1); free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 231 "parser.y"
    {
        int n=0; while(n<MAX_COLUMNS&&strlen((yyvsp[(1) - (3)].val_list)[n].str_val)>0)n++;
        strncpy((yyvsp[(1) - (3)].val_list)[n].str_val,(yyvsp[(3) - (3)].str),MAX_NAME_LEN-1); free((yyvsp[(3) - (3)].str)); (yyval.val_list)=(yyvsp[(1) - (3)].val_list);
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 238 "parser.y"
    {
        char *buf=malloc(MAX_NAME_LEN*2+2); snprintf(buf,MAX_NAME_LEN*2+1,"%s.%s",(yyvsp[(1) - (3)].str),(yyvsp[(3) - (3)].str));
        free((yyvsp[(1) - (3)].str)); free((yyvsp[(3) - (3)].str)); (yyval.str)=buf;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 242 "parser.y"
    { (yyval.str)=(yyvsp[(1) - (1)].str); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 247 "parser.y"
    { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                  g_order_col[0]='\0'; g_order_desc=0; g_has_order=0; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 249 "parser.y"
    { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                                 strncpy(g_order_col,(yyvsp[(3) - (3)].str),MAX_NAME_LEN-1); g_order_desc=0; g_has_order=1; free((yyvsp[(3) - (3)].str)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 251 "parser.y"
    { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                                 strncpy(g_order_col,(yyvsp[(3) - (4)].str),MAX_NAME_LEN-1); g_order_desc=0; g_has_order=1; free((yyvsp[(3) - (4)].str)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 253 "parser.y"
    { extern char g_order_col[]; extern int g_order_desc; extern int g_has_order;
                                 strncpy(g_order_col,(yyvsp[(3) - (4)].str),MAX_NAME_LEN-1); g_order_desc=1; g_has_order=1; free((yyvsp[(3) - (4)].str)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 258 "parser.y"
    { extern int g_limit; g_limit=-1; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 259 "parser.y"
    { extern int g_limit; g_limit=(yyvsp[(2) - (2)].num); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 264 "parser.y"
    {
        DeleteData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(3) - (5)].str),MAX_NAME_LEN-1); d->where=(yyvsp[(4) - (5)].cond);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_DELETE; n->data=d;
        execute_ast(n); free((yyvsp[(3) - (5)].str)); free(d); free(n);
    ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 274 "parser.y"
    {
        UpdateData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(2) - (8)].str),MAX_NAME_LEN-1);
        strncpy(d->set_col,(yyvsp[(4) - (8)].str),MAX_NAME_LEN-1);
        d->set_val=*(yyvsp[(6) - (8)].val); d->set_is_text=(strlen((yyvsp[(6) - (8)].val)->str_val)>0);
        d->where=(yyvsp[(7) - (8)].cond);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_UPDATE; n->data=d;
        execute_ast(n); free((yyvsp[(2) - (8)].str)); free((yyvsp[(4) - (8)].str)); free((yyvsp[(6) - (8)].val)); free(d); free(n);
    ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 287 "parser.y"
    {
        AlterAddData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(3) - (8)].str),MAX_NAME_LEN-1);
        strncpy(d->new_col.name,(yyvsp[(6) - (8)].str),MAX_NAME_LEN-1); d->new_col.type=TYPE_INT; d->new_col.flags=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_ADD; n->data=d;
        execute_ast(n); free((yyvsp[(3) - (8)].str)); free((yyvsp[(6) - (8)].str)); free(d); free(n);
    ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 294 "parser.y"
    {
        AlterAddData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(3) - (8)].str),MAX_NAME_LEN-1);
        strncpy(d->new_col.name,(yyvsp[(6) - (8)].str),MAX_NAME_LEN-1); d->new_col.type=TYPE_TEXT; d->new_col.flags=0;
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_ADD; n->data=d;
        execute_ast(n); free((yyvsp[(3) - (8)].str)); free((yyvsp[(6) - (8)].str)); free(d); free(n);
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 301 "parser.y"
    {
        AlterDropData *d=calloc(1,sizeof(*d));
        strncpy(d->table_name,(yyvsp[(3) - (7)].str),MAX_NAME_LEN-1);
        strncpy(d->col_name,(yyvsp[(6) - (7)].str),MAX_NAME_LEN-1);
        ASTNode *n=calloc(1,sizeof(*n)); n->type=AST_ALTER_DROP; n->data=d;
        execute_ast(n); free((yyvsp[(3) - (7)].str)); free((yyvsp[(6) - (7)].str)); free(d); free(n);
    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 312 "parser.y"
    { cond_init(&(yyval.cond)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 313 "parser.y"
    { (yyval.cond)=(yyvsp[(1) - (1)].cond); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 317 "parser.y"
    {
        cond_init(&(yyval.cond)); cond_add(&(yyval.cond),(yyvsp[(2) - (2)].scond),0);
    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 320 "parser.y"
    {
        (yyval.cond)=(yyvsp[(1) - (3)].cond); cond_add(&(yyval.cond),(yyvsp[(3) - (3)].scond),0);
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 323 "parser.y"
    {
        (yyval.cond)=(yyvsp[(1) - (3)].cond); cond_add(&(yyval.cond),(yyvsp[(3) - (3)].scond),1);
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 329 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_EQ;  (yyval.scond).val.int_val=(yyvsp[(3) - (3)].num); (yyval.scond).is_text=0; free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 330 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_NEQ; (yyval.scond).val.int_val=(yyvsp[(3) - (3)].num); (yyval.scond).is_text=0; free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 331 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_LT;  (yyval.scond).val.int_val=(yyvsp[(3) - (3)].num); (yyval.scond).is_text=0; free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 332 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_GT;  (yyval.scond).val.int_val=(yyvsp[(3) - (3)].num); (yyval.scond).is_text=0; free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 333 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_LTE; (yyval.scond).val.int_val=(yyvsp[(3) - (3)].num); (yyval.scond).is_text=0; free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 334 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_GTE; (yyval.scond).val.int_val=(yyvsp[(3) - (3)].num); (yyval.scond).is_text=0; free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 335 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_EQ;  strncpy((yyval.scond).val.str_val,(yyvsp[(3) - (3)].str),MAX_STRING_LEN-1); (yyval.scond).is_text=1; free((yyvsp[(1) - (3)].str));free((yyvsp[(3) - (3)].str)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 336 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_NEQ; strncpy((yyval.scond).val.str_val,(yyvsp[(3) - (3)].str),MAX_STRING_LEN-1); (yyval.scond).is_text=1; free((yyvsp[(1) - (3)].str));free((yyvsp[(3) - (3)].str)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 337 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_LT;  strncpy((yyval.scond).val.str_val,(yyvsp[(3) - (3)].str),MAX_STRING_LEN-1); (yyval.scond).is_text=1; free((yyvsp[(1) - (3)].str));free((yyvsp[(3) - (3)].str)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 338 "parser.y"
    { memset(&(yyval.scond),0,sizeof((yyval.scond))); strncpy((yyval.scond).col_name,(yyvsp[(1) - (3)].str),MAX_NAME_LEN-1); (yyval.scond).op=OP_GT;  strncpy((yyval.scond).val.str_val,(yyvsp[(3) - (3)].str),MAX_STRING_LEN-1); (yyval.scond).is_text=1; free((yyvsp[(1) - (3)].str));free((yyvsp[(3) - (3)].str)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2053 "..\\build\\parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 341 "parser.y"

void yyerror(const char *s) {
    fprintf(stderr,"Parse error at line %d: %s\n",yylineno,s);
}

