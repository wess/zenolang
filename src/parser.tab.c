/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FN = 258,
     LET = 259,
     CONST = 260,
     STRUCT = 261,
     WITH = 262,
     WHERE = 263,
     IF = 264,
     ELSE = 265,
     MATCH = 266,
     RETURN = 267,
     IMPORT = 268,
     TYPE = 269,
     EQ = 270,
     NEQ = 271,
     GE = 272,
     LE = 273,
     AND = 274,
     OR = 275,
     CONCAT = 276,
     ARROW = 277,
     PIPE = 278,
     SPREAD = 279,
     IDENTIFIER = 280,
     TYPE_NAME = 281,
     INT_LITERAL = 282,
     FLOAT_LITERAL = 283,
     STRING_LITERAL = 284,
     BOOL_LITERAL = 285
   };
#endif
/* Tokens.  */
#define FN 258
#define LET 259
#define CONST 260
#define STRUCT 261
#define WITH 262
#define WHERE 263
#define IF 264
#define ELSE 265
#define MATCH 266
#define RETURN 267
#define IMPORT 268
#define TYPE 269
#define EQ 270
#define NEQ 271
#define GE 272
#define LE 273
#define AND 274
#define OR 275
#define CONCAT 276
#define ARROW 277
#define PIPE 278
#define SPREAD 279
#define IDENTIFIER 280
#define TYPE_NAME 281
#define INT_LITERAL 282
#define FLOAT_LITERAL 283
#define STRING_LITERAL 284
#define BOOL_LITERAL 285




/* Copy the first part of user declarations.  */
#line 1 "src/parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// External declarations
extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

// Function declarations
void yyerror(const char* s);
AST_Node* root = NULL;  // Root of the AST


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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 18 "src/parser.y"
{
    char* str;
    AST_Node* node;
    AST_Node_List* node_list;
    TypeInfo* type_info;
    StructField* field;
    StructField_List* field_list;
    ExpressionList* expr_list;
    GuardClause* guard;
    MatchCase* match_case;
    MatchCase_List* match_case_list;
}
/* Line 193 of yacc.c.  */
#line 186 "src/parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 199 "src/parser.tab.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   607

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNRULES -- Number of states.  */
#define YYNSTATES  208

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   285

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    47,     2,     2,     2,    46,     2,     2,
      37,    38,    44,    42,    36,    43,    48,    45,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    33,    31,
      39,    32,    40,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,     2,    41,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    35,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    14,    16,    18,
      20,    24,    30,    36,    44,    50,    58,    64,    72,    74,
      78,    80,    84,    88,    98,   109,   110,   113,   115,   116,
     120,   126,   128,   130,   132,   137,   142,   146,   147,   150,
     152,   155,   157,   159,   161,   163,   169,   177,   183,   185,
     188,   192,   198,   204,   208,   212,   215,   217,   219,   221,
     223,   227,   231,   235,   239,   243,   247,   251,   255,   259,
     263,   267,   271,   275,   279,   283,   287,   290,   293,   297,
     301,   304,   308,   310,   314,   318,   321,   326,   327,   329,
     333,   335,   337,   339,   341,   345,   346,   348
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      52,     0,    -1,    53,    -1,    54,    -1,    53,    54,    -1,
      62,    -1,    57,    -1,    58,    -1,    56,    -1,    55,    -1,
      13,    29,    31,    -1,    14,    25,    32,    66,    31,    -1,
       4,    25,    32,    76,    31,    -1,     4,    25,    33,    66,
      32,    76,    31,    -1,     5,    25,    32,    76,    31,    -1,
       5,    25,    33,    66,    32,    76,    31,    -1,     6,    25,
      34,    60,    35,    -1,     6,    25,     7,    59,    34,    60,
      35,    -1,    25,    -1,    59,    36,    25,    -1,    61,    -1,
      60,    36,    61,    -1,    25,    33,    66,    -1,     3,    64,
      37,    65,    38,    33,    66,    63,    68,    -1,     3,    64,
      37,    65,    38,    33,    66,    22,    76,    31,    -1,    -1,
       8,    76,    -1,    25,    -1,    -1,    25,    33,    66,    -1,
      65,    36,    25,    33,    66,    -1,    26,    -1,    25,    -1,
      67,    -1,    26,    39,    66,    40,    -1,    25,    39,    66,
      40,    -1,    34,    69,    35,    -1,    -1,    69,    70,    -1,
      57,    -1,    76,    31,    -1,    71,    -1,    72,    -1,    75,
      -1,    68,    -1,     9,    37,    76,    38,    70,    -1,     9,
      37,    76,    38,    70,    10,    70,    -1,    11,    76,    34,
      73,    35,    -1,    74,    -1,    73,    74,    -1,    76,    22,
      70,    -1,    25,    33,    66,    22,    70,    -1,    76,     9,
      76,    22,    70,    -1,    41,    22,    70,    -1,    12,    76,
      31,    -1,    12,    31,    -1,    82,    -1,    25,    -1,    80,
      -1,    77,    -1,    76,    23,    76,    -1,    37,    76,    38,
      -1,    76,    42,    76,    -1,    76,    43,    76,    -1,    76,
      44,    76,    -1,    76,    45,    76,    -1,    76,    46,    76,
      -1,    76,    21,    76,    -1,    76,    15,    76,    -1,    76,
      16,    76,    -1,    76,    39,    76,    -1,    76,    40,    76,
      -1,    76,    18,    76,    -1,    76,    17,    76,    -1,    76,
      19,    76,    -1,    76,    20,    76,    -1,    47,    76,    -1,
      43,    76,    -1,    25,    32,    76,    -1,    76,    48,    25,
      -1,    24,    76,    -1,    34,    78,    35,    -1,    79,    -1,
      78,    36,    79,    -1,    25,    33,    76,    -1,    24,    76,
      -1,    25,    37,    81,    38,    -1,    -1,    76,    -1,    81,
      36,    76,    -1,    27,    -1,    28,    -1,    29,    -1,    30,
      -1,    49,    83,    50,    -1,    -1,    76,    -1,    83,    36,
      76,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    55,    55,    59,    60,    64,    65,    66,    67,    68,
      72,    76,    80,    81,    82,    83,    87,    88,    92,    93,
      97,    98,   102,   106,   109,   117,   118,   122,   126,   127,
     128,   135,   136,   137,   141,   142,   146,   150,   151,   155,
     156,   157,   158,   159,   160,   164,   165,   169,   173,   174,
     178,   179,   183,   184,   188,   189,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   221,   225,   226,   230,   231,   235,   239,   240,   241,
     245,   246,   247,   248,   249,   253,   254,   255
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "FN", "LET", "CONST", "STRUCT", "WITH",
  "WHERE", "IF", "ELSE", "MATCH", "RETURN", "IMPORT", "TYPE", "EQ", "NEQ",
  "GE", "LE", "AND", "OR", "CONCAT", "ARROW", "PIPE", "SPREAD",
  "IDENTIFIER", "TYPE_NAME", "INT_LITERAL", "FLOAT_LITERAL",
  "STRING_LITERAL", "BOOL_LITERAL", "';'", "'='", "':'", "'{'", "'}'",
  "','", "'('", "')'", "'<'", "'>'", "'_'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'!'", "'.'", "'['", "']'", "$accept", "program", "declarations",
  "declaration", "import_declaration", "type_declaration",
  "variable_declaration", "struct_declaration", "struct_composition_list",
  "struct_fields", "struct_field", "function_declaration", "guard_clause",
  "function_name", "parameter_list", "type", "type_with_generics",
  "compound_statement", "statements", "statement", "if_statement",
  "match_statement", "match_cases", "match_case", "return_statement",
  "expression", "struct_instantiation", "struct_init_list",
  "struct_init_field", "function_call", "argument_list", "literal",
  "array_elements", 0
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
     285,    59,    61,    58,   123,   125,    44,    40,    41,    60,
      62,    95,    43,    45,    42,    47,    37,    33,    46,    91,
      93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    51,    52,    53,    53,    54,    54,    54,    54,    54,
      55,    56,    57,    57,    57,    57,    58,    58,    59,    59,
      60,    60,    61,    62,    62,    63,    63,    64,    65,    65,
      65,    66,    66,    66,    67,    67,    68,    69,    69,    70,
      70,    70,    70,    70,    70,    71,    71,    72,    73,    73,
      74,    74,    74,    74,    75,    75,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    77,    78,    78,    79,    79,    80,    81,    81,    81,
      82,    82,    82,    82,    82,    83,    83,    83
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       3,     5,     5,     7,     5,     7,     5,     7,     1,     3,
       1,     3,     3,     9,    10,     0,     2,     1,     0,     3,
       5,     1,     1,     1,     4,     4,     3,     0,     2,     1,
       2,     1,     1,     1,     1,     5,     7,     5,     1,     2,
       3,     5,     5,     3,     3,     2,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     3,
       2,     3,     1,     3,     3,     2,     4,     0,     1,     3,
       1,     1,     1,     1,     3,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     2,     3,
       9,     8,     6,     7,     5,    27,     0,     0,     0,     0,
       0,     0,     1,     4,    28,     0,     0,     0,     0,     0,
       0,    10,     0,     0,     0,     0,    57,    90,    91,    92,
      93,     0,     0,     0,     0,    95,     0,    59,    58,    56,
      32,    31,     0,    33,     0,     0,    18,     0,     0,     0,
      20,     0,     0,     0,     0,    80,     0,    87,     0,     0,
       0,    82,     0,    77,    76,    96,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,    16,     0,    11,    29,     0,     0,    78,    88,
       0,    85,     0,    81,     0,    61,     0,    94,    68,    69,
      73,    72,    74,    75,    67,    60,    70,    71,    62,    63,
      64,    65,    66,    79,     0,     0,     0,     0,     0,    19,
      22,    21,     0,    25,     0,    86,    84,    83,    97,    35,
      34,    13,    15,    17,    30,     0,     0,     0,    89,    26,
       0,    37,    23,    24,     0,     0,     0,     0,    37,    36,
      39,    44,    38,    41,    42,    43,     0,     0,     0,    55,
       0,    40,     0,     0,    54,     0,    57,     0,     0,    48,
       0,    45,     0,     0,    47,    49,     0,     0,     0,     0,
      53,     0,    50,    46,     0,     0,    51,    52
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    11,   170,    13,    57,    59,
      60,    14,   157,    16,    34,    52,    53,   171,   164,   172,
     173,   174,   188,   189,   175,   176,    47,    70,    71,    48,
     110,    49,    76
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -83
static const yytype_int16 yypact[] =
{
       0,   -13,    14,    26,    35,     7,    43,    72,     0,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,    42,    56,    60,    -6,
      54,    62,   -83,   -83,    79,   558,   -18,   558,   -18,    82,
      92,   -83,   -18,    64,    11,   558,    49,   -83,   -83,   -83,
     -83,    85,   558,   558,   558,   558,   200,   -83,   -83,   -83,
      63,    94,    89,   -83,   234,   100,   -83,    -1,   101,    77,
     -83,   104,   -18,    99,   103,   502,   558,   558,   558,   105,
      91,   -83,   268,   502,   502,   502,   -27,   558,   558,   558,
     558,   558,   558,   558,   558,   -83,   558,   558,   558,   558,
     558,   558,   558,   112,   -18,   -18,   558,   -83,   558,    92,
     114,   -18,   -83,    92,   -83,   -83,   107,   -18,   502,   502,
      12,   502,   558,   -83,    85,   -83,   558,   -83,   502,   502,
     502,   502,   502,   502,   502,   502,   502,   502,   502,   502,
     502,   502,   502,   -83,   106,   108,   278,   312,    93,   -83,
     -83,   -83,   -18,     8,   558,   -83,   502,   -83,   502,   -83,
     -83,   -83,   -83,   -83,   -83,   558,   558,   109,   502,   502,
     346,   -83,   -83,   -83,    71,   110,   558,   506,    85,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   380,   558,   414,   -83,
     424,   -83,   458,   532,   -83,   155,    37,   122,    -3,   -83,
     190,   135,   -18,   155,   -83,   -83,   558,   155,   155,   127,
     -83,   468,   -83,   -83,   155,   155,   -83,   -83
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -83,   -83,   -83,   142,   -83,   -83,    29,   -83,   -83,    52,
      50,   -83,   -83,   -83,   -83,   -17,   -83,    -2,   -83,   -82,
     -83,   -83,   -83,   -34,   -83,   -25,   -83,   -83,    47,   -83,
     -83,   -83,   -83
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      46,    29,    54,     1,     2,     3,     4,    50,    51,   116,
      65,    55,    15,     5,     6,    61,   155,    72,    73,    74,
      75,    35,   186,   117,    37,    38,    39,    40,    30,    12,
     156,    41,   194,    99,    42,   100,    20,    12,   187,    17,
      43,   108,   109,   111,    44,   105,    45,    63,   144,    64,
     145,    18,   118,   119,   120,   121,   122,   123,   124,   125,
      19,   126,   127,   128,   129,   130,   131,   132,    21,    66,
     192,   136,    22,   137,    67,     2,     3,   134,   135,    24,
     165,    66,   166,   167,   140,    31,    67,   146,    25,    26,
     143,   148,    27,    28,    32,    35,    36,    62,    37,    38,
      39,    40,    94,   191,    33,   168,   169,    56,    42,    68,
      69,   200,   102,   103,    43,   202,   203,    58,    44,   158,
      45,    96,   206,   207,   106,   154,   113,   114,   153,   103,
     159,   160,    98,    95,   101,   104,   107,   133,   112,   139,
     142,   178,   180,   161,   193,   198,   149,   177,   150,   204,
      23,   138,   182,   141,   195,   162,     0,     0,   190,     2,
       3,   147,     0,   190,   165,     0,   166,   167,     0,     0,
       0,   201,     0,     0,     0,   199,     0,     0,     0,    35,
      36,     0,    37,    38,    39,    40,     0,     0,     0,   168,
       0,     0,    42,     0,     0,     0,     0,     0,    43,   196,
       0,     0,    44,     0,    45,    77,    78,    79,    80,    81,
      82,    83,   197,    84,     0,    77,    78,    79,    80,    81,
      82,    83,     0,    84,     0,     0,     0,     0,     0,    86,
      87,    85,    88,    89,    90,    91,    92,     0,    93,    86,
      87,     0,    88,    89,    90,    91,    92,     0,    93,    77,
      78,    79,    80,    81,    82,    83,     0,    84,     0,     0,
       0,     0,     0,     0,     0,    97,     0,     0,     0,     0,
       0,     0,     0,    86,    87,     0,    88,    89,    90,    91,
      92,     0,    93,    77,    78,    79,    80,    81,    82,    83,
       0,    84,     0,    77,    78,    79,    80,    81,    82,    83,
       0,    84,     0,     0,     0,     0,   115,    86,    87,   151,
      88,    89,    90,    91,    92,     0,    93,    86,    87,     0,
      88,    89,    90,    91,    92,     0,    93,    77,    78,    79,
      80,    81,    82,    83,     0,    84,     0,     0,     0,     0,
       0,     0,     0,   152,     0,     0,     0,     0,     0,     0,
       0,    86,    87,     0,    88,    89,    90,    91,    92,     0,
      93,    77,    78,    79,    80,    81,    82,    83,     0,    84,
       0,     0,     0,     0,     0,     0,     0,   163,     0,     0,
       0,     0,     0,     0,     0,    86,    87,     0,    88,    89,
      90,    91,    92,     0,    93,    77,    78,    79,    80,    81,
      82,    83,     0,    84,     0,     0,     0,     0,     0,     0,
       0,   181,     0,     0,     0,     0,     0,     0,     0,    86,
      87,     0,    88,    89,    90,    91,    92,     0,    93,    77,
      78,    79,    80,    81,    82,    83,     0,    84,     0,    77,
      78,    79,    80,    81,    82,    83,     0,    84,   183,     0,
       0,     0,     0,    86,    87,   184,    88,    89,    90,    91,
      92,     0,    93,    86,    87,     0,    88,    89,    90,    91,
      92,     0,    93,    77,    78,    79,    80,    81,    82,    83,
       0,    84,     0,    77,    78,    79,    80,    81,    82,    83,
     205,    84,     0,     0,     0,     0,   185,    86,    87,     0,
      88,    89,    90,    91,    92,     0,    93,    86,    87,     0,
      88,    89,    90,    91,    92,     0,    93,    77,    78,    79,
      80,    81,    82,    83,     0,    84,     0,     0,     0,     0,
      35,    36,     0,    37,    38,    39,    40,   179,     0,     0,
      41,    86,    87,    42,    88,    89,    90,    91,    92,    43,
      93,     0,     0,    44,     0,    45,    35,   186,     0,    37,
      38,    39,    40,     0,     0,     0,    41,     0,     0,    42,
       0,     0,     0,   187,     0,    43,     0,     0,     0,    44,
       0,    45,    35,    36,     0,    37,    38,    39,    40,     0,
       0,     0,    41,     0,     0,    42,     0,     0,     0,     0,
       0,    43,     0,     0,     0,    44,     0,    45
};

static const yytype_int16 yycheck[] =
{
      25,     7,    27,     3,     4,     5,     6,    25,    26,    36,
      35,    28,    25,    13,    14,    32,     8,    42,    43,    44,
      45,    24,    25,    50,    27,    28,    29,    30,    34,     0,
      22,    34,    35,    34,    37,    36,    29,     8,    41,    25,
      43,    66,    67,    68,    47,    62,    49,    36,    36,    38,
      38,    25,    77,    78,    79,    80,    81,    82,    83,    84,
      25,    86,    87,    88,    89,    90,    91,    92,    25,    32,
      33,    96,     0,    98,    37,     4,     5,    94,    95,    37,
       9,    32,    11,    12,   101,    31,    37,   112,    32,    33,
     107,   116,    32,    33,    32,    24,    25,    33,    27,    28,
      29,    30,    39,   185,    25,    34,    35,    25,    37,    24,
      25,   193,    35,    36,    43,   197,   198,    25,    47,   144,
      49,    32,   204,   205,    25,   142,    35,    36,    35,    36,
     155,   156,    32,    39,    33,    31,    33,    25,    33,    25,
      33,   166,   167,    34,    22,    10,    40,    37,    40,    22,
       8,    99,   177,   103,   188,   157,    -1,    -1,   183,     4,
       5,   114,    -1,   188,     9,    -1,    11,    12,    -1,    -1,
      -1,   196,    -1,    -1,    -1,   192,    -1,    -1,    -1,    24,
      25,    -1,    27,    28,    29,    30,    -1,    -1,    -1,    34,
      -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    43,     9,
      -1,    -1,    47,    -1,    49,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    15,    16,    17,    18,    19,
      20,    21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    39,
      40,    31,    42,    43,    44,    45,    46,    -1,    48,    39,
      40,    -1,    42,    43,    44,    45,    46,    -1,    48,    15,
      16,    17,    18,    19,    20,    21,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    40,    -1,    42,    43,    44,    45,
      46,    -1,    48,    15,    16,    17,    18,    19,    20,    21,
      -1,    23,    -1,    15,    16,    17,    18,    19,    20,    21,
      -1,    23,    -1,    -1,    -1,    -1,    38,    39,    40,    31,
      42,    43,    44,    45,    46,    -1,    48,    39,    40,    -1,
      42,    43,    44,    45,    46,    -1,    48,    15,    16,    17,
      18,    19,    20,    21,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    40,    -1,    42,    43,    44,    45,    46,    -1,
      48,    15,    16,    17,    18,    19,    20,    21,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    39,    40,    -1,    42,    43,
      44,    45,    46,    -1,    48,    15,    16,    17,    18,    19,
      20,    21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      40,    -1,    42,    43,    44,    45,    46,    -1,    48,    15,
      16,    17,    18,    19,    20,    21,    -1,    23,    -1,    15,
      16,    17,    18,    19,    20,    21,    -1,    23,    34,    -1,
      -1,    -1,    -1,    39,    40,    31,    42,    43,    44,    45,
      46,    -1,    48,    39,    40,    -1,    42,    43,    44,    45,
      46,    -1,    48,    15,    16,    17,    18,    19,    20,    21,
      -1,    23,    -1,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    -1,    -1,    -1,    -1,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    -1,    48,    39,    40,    -1,
      42,    43,    44,    45,    46,    -1,    48,    15,    16,    17,
      18,    19,    20,    21,    -1,    23,    -1,    -1,    -1,    -1,
      24,    25,    -1,    27,    28,    29,    30,    31,    -1,    -1,
      34,    39,    40,    37,    42,    43,    44,    45,    46,    43,
      48,    -1,    -1,    47,    -1,    49,    24,    25,    -1,    27,
      28,    29,    30,    -1,    -1,    -1,    34,    -1,    -1,    37,
      -1,    -1,    -1,    41,    -1,    43,    -1,    -1,    -1,    47,
      -1,    49,    24,    25,    -1,    27,    28,    29,    30,    -1,
      -1,    -1,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    -1,    -1,    47,    -1,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,    13,    14,    52,    53,    54,
      55,    56,    57,    58,    62,    25,    64,    25,    25,    25,
      29,    25,     0,    54,    37,    32,    33,    32,    33,     7,
      34,    31,    32,    25,    65,    24,    25,    27,    28,    29,
      30,    34,    37,    43,    47,    49,    76,    77,    80,    82,
      25,    26,    66,    67,    76,    66,    25,    59,    25,    60,
      61,    66,    33,    36,    38,    76,    32,    37,    24,    25,
      78,    79,    76,    76,    76,    76,    83,    15,    16,    17,
      18,    19,    20,    21,    23,    31,    39,    40,    42,    43,
      44,    45,    46,    48,    39,    39,    32,    31,    32,    34,
      36,    33,    35,    36,    31,    66,    25,    33,    76,    76,
      81,    76,    33,    35,    36,    38,    36,    50,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    25,    66,    66,    76,    76,    60,    25,
      66,    61,    33,    66,    36,    38,    76,    79,    76,    40,
      40,    31,    31,    35,    66,     8,    22,    63,    76,    76,
      76,    34,    68,    31,    69,     9,    11,    12,    34,    35,
      57,    68,    70,    71,    72,    75,    76,    37,    76,    31,
      76,    31,    76,    34,    31,    38,    25,    41,    73,    74,
      76,    70,    33,    22,    35,    74,     9,    22,    10,    66,
      70,    76,    70,    70,    22,    22,    70,    70
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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

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
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
        case 2:
#line 55 "src/parser.y"
    { root = create_program_node((yyvsp[(1) - (1)].node_list)); ;}
    break;

  case 3:
#line 59 "src/parser.y"
    { (yyval.node_list) = create_node_list((yyvsp[(1) - (1)].node)); ;}
    break;

  case 4:
#line 60 "src/parser.y"
    { append_node((yyvsp[(1) - (2)].node_list), (yyvsp[(2) - (2)].node)); (yyval.node_list) = (yyvsp[(1) - (2)].node_list); ;}
    break;

  case 5:
#line 64 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 6:
#line 65 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 7:
#line 66 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 8:
#line 67 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 9:
#line 68 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 10:
#line 72 "src/parser.y"
    { (yyval.node) = create_import_node((yyvsp[(2) - (3)].str)); ;}
    break;

  case 11:
#line 76 "src/parser.y"
    { (yyval.node) = create_type_declaration_node((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].type_info)); ;}
    break;

  case 12:
#line 80 "src/parser.y"
    { (yyval.node) = create_variable_node(VAR_LET, (yyvsp[(2) - (5)].str), NULL, (yyvsp[(4) - (5)].node)); ;}
    break;

  case 13:
#line 81 "src/parser.y"
    { (yyval.node) = create_variable_node(VAR_LET, (yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].type_info), (yyvsp[(6) - (7)].node)); ;}
    break;

  case 14:
#line 82 "src/parser.y"
    { (yyval.node) = create_variable_node(VAR_CONST, (yyvsp[(2) - (5)].str), NULL, (yyvsp[(4) - (5)].node)); ;}
    break;

  case 15:
#line 83 "src/parser.y"
    { (yyval.node) = create_variable_node(VAR_CONST, (yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].type_info), (yyvsp[(6) - (7)].node)); ;}
    break;

  case 16:
#line 87 "src/parser.y"
    { (yyval.node) = create_struct_node((yyvsp[(2) - (5)].str), NULL, (yyvsp[(4) - (5)].field_list)); ;}
    break;

  case 17:
#line 88 "src/parser.y"
    { (yyval.node) = create_struct_node((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].node_list), (yyvsp[(6) - (7)].field_list)); ;}
    break;

  case 18:
#line 92 "src/parser.y"
    { (yyval.node_list) = create_node_list(create_identifier_node((yyvsp[(1) - (1)].str))); ;}
    break;

  case 19:
#line 93 "src/parser.y"
    { append_node((yyvsp[(1) - (3)].node_list), create_identifier_node((yyvsp[(3) - (3)].str))); (yyval.node_list) = (yyvsp[(1) - (3)].node_list); ;}
    break;

  case 20:
#line 97 "src/parser.y"
    { (yyval.field_list) = create_field_list((yyvsp[(1) - (1)].field)); ;}
    break;

  case 21:
#line 98 "src/parser.y"
    { append_field((yyvsp[(1) - (3)].field_list), (yyvsp[(3) - (3)].field)); (yyval.field_list) = (yyvsp[(1) - (3)].field_list); ;}
    break;

  case 22:
#line 102 "src/parser.y"
    { (yyval.field) = create_struct_field((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].type_info)); ;}
    break;

  case 23:
#line 106 "src/parser.y"
    {
        (yyval.node) = create_function_node((yyvsp[(2) - (9)].str), (yyvsp[(4) - (9)].node_list), (yyvsp[(7) - (9)].type_info), (yyvsp[(8) - (9)].guard), (yyvsp[(9) - (9)].node));
    ;}
    break;

  case 24:
#line 109 "src/parser.y"
    {
        AST_Node* return_stmt = create_return_node((yyvsp[(9) - (10)].node));
        AST_Node_List* body = create_node_list(return_stmt);
        (yyval.node) = create_function_node((yyvsp[(2) - (10)].str), (yyvsp[(4) - (10)].node_list), (yyvsp[(7) - (10)].type_info), NULL, create_compound_statement_node(body));
    ;}
    break;

  case 25:
#line 117 "src/parser.y"
    { (yyval.guard) = NULL; ;}
    break;

  case 26:
#line 118 "src/parser.y"
    { (yyval.guard) = create_guard_clause((yyvsp[(2) - (2)].node)); ;}
    break;

  case 27:
#line 122 "src/parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 28:
#line 126 "src/parser.y"
    { (yyval.node_list) = create_node_list(NULL); ;}
    break;

  case 29:
#line 127 "src/parser.y"
    { (yyval.node_list) = create_node_list(create_parameter_node((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].type_info))); ;}
    break;

  case 30:
#line 128 "src/parser.y"
    { 
        append_node((yyvsp[(1) - (5)].node_list), create_parameter_node((yyvsp[(3) - (5)].str), (yyvsp[(5) - (5)].type_info))); 
        (yyval.node_list) = (yyvsp[(1) - (5)].node_list); 
    ;}
    break;

  case 31:
#line 135 "src/parser.y"
    { (yyval.type_info) = create_type_info((yyvsp[(1) - (1)].str), NULL); ;}
    break;

  case 32:
#line 136 "src/parser.y"
    { (yyval.type_info) = create_type_info((yyvsp[(1) - (1)].str), NULL); ;}
    break;

  case 33:
#line 137 "src/parser.y"
    { (yyval.type_info) = (yyvsp[(1) - (1)].type_info); ;}
    break;

  case 34:
#line 141 "src/parser.y"
    { (yyval.type_info) = create_type_info((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].type_info)); ;}
    break;

  case 35:
#line 142 "src/parser.y"
    { (yyval.type_info) = create_type_info((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].type_info)); ;}
    break;

  case 36:
#line 146 "src/parser.y"
    { (yyval.node) = create_compound_statement_node((yyvsp[(2) - (3)].node_list)); ;}
    break;

  case 37:
#line 150 "src/parser.y"
    { (yyval.node_list) = create_node_list(NULL); ;}
    break;

  case 38:
#line 151 "src/parser.y"
    { append_node((yyvsp[(1) - (2)].node_list), (yyvsp[(2) - (2)].node)); (yyval.node_list) = (yyvsp[(1) - (2)].node_list); ;}
    break;

  case 39:
#line 155 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 40:
#line 156 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 41:
#line 157 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 42:
#line 158 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 43:
#line 159 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 44:
#line 160 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 45:
#line 164 "src/parser.y"
    { (yyval.node) = create_if_node((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node), NULL); ;}
    break;

  case 46:
#line 165 "src/parser.y"
    { (yyval.node) = create_if_node((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].node)); ;}
    break;

  case 47:
#line 169 "src/parser.y"
    { (yyval.node) = create_match_node((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].match_case_list)); ;}
    break;

  case 48:
#line 173 "src/parser.y"
    { (yyval.match_case_list) = create_match_case_list((yyvsp[(1) - (1)].match_case)); ;}
    break;

  case 49:
#line 174 "src/parser.y"
    { append_match_case((yyvsp[(1) - (2)].match_case_list), (yyvsp[(2) - (2)].match_case)); (yyval.match_case_list) = (yyvsp[(1) - (2)].match_case_list); ;}
    break;

  case 50:
#line 178 "src/parser.y"
    { (yyval.match_case) = create_match_case((yyvsp[(1) - (3)].node), NULL, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 51:
#line 179 "src/parser.y"
    { 
        AST_Node* binding = create_binding_pattern((yyvsp[(1) - (5)].str), (yyvsp[(3) - (5)].type_info));
        (yyval.match_case) = create_match_case(binding, NULL, (yyvsp[(5) - (5)].node)); 
    ;}
    break;

  case 52:
#line 183 "src/parser.y"
    { (yyval.match_case) = create_match_case((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 53:
#line 184 "src/parser.y"
    { (yyval.match_case) = create_match_case(create_wildcard_node(), NULL, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 54:
#line 188 "src/parser.y"
    { (yyval.node) = create_return_node((yyvsp[(2) - (3)].node)); ;}
    break;

  case 55:
#line 189 "src/parser.y"
    { (yyval.node) = create_return_node(NULL); ;}
    break;

  case 56:
#line 193 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 57:
#line 194 "src/parser.y"
    { (yyval.node) = create_identifier_node((yyvsp[(1) - (1)].str)); ;}
    break;

  case 58:
#line 195 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 59:
#line 196 "src/parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 60:
#line 197 "src/parser.y"
    { (yyval.node) = create_pipe_node((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 61:
#line 198 "src/parser.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 62:
#line 199 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_ADD, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 63:
#line 200 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_SUB, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 64:
#line 201 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_MUL, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 65:
#line 202 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_DIV, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 66:
#line 203 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_MOD, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 67:
#line 204 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_CONCAT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 68:
#line 205 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_EQ, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 69:
#line 206 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_NEQ, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 70:
#line 207 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_LT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 71:
#line 208 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_GT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 72:
#line 209 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_LE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 73:
#line 210 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_GE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 74:
#line 211 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 75:
#line 212 "src/parser.y"
    { (yyval.node) = create_binary_op_node(OP_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 76:
#line 213 "src/parser.y"
    { (yyval.node) = create_unary_op_node(OP_NOT, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 77:
#line 214 "src/parser.y"
    { (yyval.node) = create_unary_op_node(OP_NEG, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 78:
#line 215 "src/parser.y"
    { (yyval.node) = create_assignment_node((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 79:
#line 216 "src/parser.y"
    { (yyval.node) = create_member_access_node((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].str)); ;}
    break;

  case 80:
#line 217 "src/parser.y"
    { (yyval.node) = create_spread_node((yyvsp[(2) - (2)].node)); ;}
    break;

  case 81:
#line 221 "src/parser.y"
    { (yyval.node) = create_struct_init_node((yyvsp[(2) - (3)].node_list)); ;}
    break;

  case 82:
#line 225 "src/parser.y"
    { (yyval.node_list) = create_node_list((yyvsp[(1) - (1)].node)); ;}
    break;

  case 83:
#line 226 "src/parser.y"
    { append_node((yyvsp[(1) - (3)].node_list), (yyvsp[(3) - (3)].node)); (yyval.node_list) = (yyvsp[(1) - (3)].node_list); ;}
    break;

  case 84:
#line 230 "src/parser.y"
    { (yyval.node) = create_struct_init_field_node((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 85:
#line 231 "src/parser.y"
    { (yyval.node) = create_spread_node((yyvsp[(2) - (2)].node)); ;}
    break;

  case 86:
#line 235 "src/parser.y"
    { (yyval.node) = create_function_call_node((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].expr_list)); ;}
    break;

  case 87:
#line 239 "src/parser.y"
    { (yyval.expr_list) = create_expression_list(NULL); ;}
    break;

  case 88:
#line 240 "src/parser.y"
    { (yyval.expr_list) = create_expression_list((yyvsp[(1) - (1)].node)); ;}
    break;

  case 89:
#line 241 "src/parser.y"
    { append_expression((yyvsp[(1) - (3)].expr_list), (yyvsp[(3) - (3)].node)); (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list); ;}
    break;

  case 90:
#line 245 "src/parser.y"
    { (yyval.node) = create_int_literal_node((yyvsp[(1) - (1)].str)); ;}
    break;

  case 91:
#line 246 "src/parser.y"
    { (yyval.node) = create_float_literal_node((yyvsp[(1) - (1)].str)); ;}
    break;

  case 92:
#line 247 "src/parser.y"
    { (yyval.node) = create_string_literal_node((yyvsp[(1) - (1)].str)); ;}
    break;

  case 93:
#line 248 "src/parser.y"
    { (yyval.node) = create_bool_literal_node((yyvsp[(1) - (1)].str)); ;}
    break;

  case 94:
#line 249 "src/parser.y"
    { (yyval.node) = create_array_literal_node((yyvsp[(2) - (3)].expr_list)); ;}
    break;

  case 95:
#line 253 "src/parser.y"
    { (yyval.expr_list) = create_expression_list(NULL); ;}
    break;

  case 96:
#line 254 "src/parser.y"
    { (yyval.expr_list) = create_expression_list((yyvsp[(1) - (1)].node)); ;}
    break;

  case 97:
#line 255 "src/parser.y"
    { append_expression((yyvsp[(1) - (3)].expr_list), (yyvsp[(3) - (3)].node)); (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2149 "src/parser.tab.c"
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
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
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


#line 258 "src/parser.y"


void yyerror(const char* s) {
    fprintf(stderr, "Parser error at line %d: %s near '%s'\n", yylineno, s, yytext);
    exit(1);
}
