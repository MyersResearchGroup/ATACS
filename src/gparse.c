/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         gparse
#define yylex           glex
#define yyerror         gerror
#define yydebug         gdebug
#define yynerrs         gnerrs

#define yylval          glval
#define yychar          gchar

/* Copy the first part of user declarations.  */
#line 1 "gparse.y" /* yacc.c:339  */

#define YYSTYPE Node_Ptr
#include "struct.h"
#include "loadg.h"
#include <string>
  
  
  int yyerror (char*);
  int yylex ();
  
  string numToString(int num);
  ineqADT parsehsf(string hsf);
  
  int line_num = 1;
  bool completed;
  
#include "glex.c"

#line 93 "gparse.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "gparse.h".  */
#ifndef YY_G_GPARSE_H_INCLUDED
# define YY_G_GPARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDIV = 258,
    MINF = 259,
    MAXF = 260,
    FLOOR = 261,
    CEIL = 262,
    COMMENTS = 263,
    ID = 264,
    INPUTS = 265,
    OUTPUTS = 266,
    INTERNAL = 267,
    GRAPH = 268,
    END = 269,
    INTEGER = 270,
    MARKING = 271,
    NAME = 272,
    INIT_STATE = 273,
    DUMMYS = 274,
    ATACS = 275,
    DISABLE = 276,
    NOT = 277,
    VERIFY = 278,
    SEARCH = 279,
    REAL = 280,
    INF = 281,
    KEEPS = 282,
    NONINPS = 283,
    ABSTRACTS = 284,
    CONTINUOUS = 285,
    RATES = 286,
    PROPERTY = 287,
    INVARIANTS = 288,
    ENABLINGS = 289,
    ASSIGNS = 290,
    RATE_ASSIGNS = 291,
    DELAY_ASSIGNS = 292,
    PRIORITY_ASSIGNS = 293,
    BOOL_ASSIGNS = 294,
    INT_ASSIGNS = 295,
    BOOL_FALSE = 296,
    BOOL_TRUE = 297,
    ASSIGN = 298,
    VARIABLES = 299,
    INITRATES = 300,
    INITVALS = 301,
    INITINTS = 302,
    Pr = 303,
    Ss = 304,
    AU = 305,
    EU = 306,
    EG = 307,
    EF = 308,
    AG = 309,
    AF = 310,
    PG = 311,
    PF = 312,
    PU = 313,
    PX = 314,
    INTEGERS = 315,
    UNIFORM = 316,
    NORMAL = 317,
    EXPONENTIAL = 318,
    GAMMA = 319,
    LOGNORMAL = 320,
    CHISQ = 321,
    LAPLACE = 322,
    CAUCHY = 323,
    RAYLEIGH = 324,
    POISSON = 325,
    BINOMIAL = 326,
    BERNOULLI = 327,
    AND = 328,
    OR = 329,
    XOR = 330,
    IMPLIC = 331,
    TRANS_RATES = 332,
    FAILTRANS = 333,
    INT = 334,
    BIT = 335,
    RATE = 336,
    BOOL = 337,
    NONDISABLING = 338
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE glval;

int gparse (void);

#endif /* !YY_G_GPARSE_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 228 "gparse.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  54
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   987

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  108
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  100
/* YYNRULES -- Number of rules.  */
#define YYNRULES  320
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  725

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   338

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      84,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   104,    96,     2,
      94,    95,   102,    87,    90,    88,     2,   103,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   100,   106,
      99,    92,    98,    93,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    89,   107,    91,   105,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    85,    97,    86,   101,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    25,    25,    26,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    65,    64,
      74,    75,    81,    82,    88,    89,    95,    96,   102,   108,
     109,   113,   117,   123,   124,   130,   131,   137,   138,   145,
     146,   153,   154,   161,   162,   167,   173,   181,   187,   195,
     202,   207,   213,   220,   225,   232,   233,   247,   258,   272,
     285,   298,   311,   319,   336,   346,   356,   366,   369,   379,
     384,   394,   400,   410,   418,   433,   449,   452,   462,   478,
     494,   511,   529,   530,   531,   532,   535,   536,   539,   550,
     562,   565,   570,   575,   578,   584,   587,   588,   591,   595,
     599,   603,   607,   611,   618,   624,   631,   638,   645,   652,
     660,   665,   672,   681,   684,   685,   688,   694,   700,   706,
     713,   719,   726,   733,   738,   745,   754,   757,   758,   761,
     768,   774,   781,   788,   795,   802,   810,   815,   822,   831,
     834,   835,   838,   843,   851,   854,   855,   858,   921,   926,
     933,   940,   987,  1036,  1037,  1039,  1040,  1044,  1054,  1064,
    1067,  1077,  1080,  1087,  1099,  1110,  1113,  1123,  1133,  1136,
    1146,  1156,  1166,  1176,  1186,  1189,  1197,  1205,  1213,  1214,
    1221,  1226,  1231,  1242,  1257,  1258,  1259,  1263,  1264,  1265,
    1266,  1267,  1270,  1271,  1272,  1273,  1274,  1275,  1276,  1277,
    1278,  1279,  1280,  1283,  1284,  1285,  1286,  1287,  1288,  1289,
    1290,  1291,  1292,  1293,  1297,  1302,  1306,  1316,  1320,  1325,
    1331,  1337,  1344,  1346,  1347,  1354,  1361,  1369,  1377,  1385,
    1394,  1395,  1403,  1411,  1418,  1425,  1432,  1440,  1441,  1443,
    1444,  1445,  1447,  1448,  1450,  1451,  1454,  1458,  1465,  1470,
    1476,  1481,  1489,  1492,  1493,  1496,  1559,  1562,  1563,  1566,
    1584,  1587,  1588,  1591,  1607,  1610,  1611,  1614,  1618,  1622,
    1627,  1631,  1635,  1639,  1645,  1648,  1649,  1652,  1658,  1661,
    1662,  1665,  1672,  1675,  1676,  1679,  1695,  1700,  1706,  1723,
    1724,  1727,  1736,  1744,  1752,  1761,  1762,  1765,  1773,  1782,
    1789
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDIV", "MINF", "MAXF", "FLOOR", "CEIL",
  "COMMENTS", "ID", "INPUTS", "OUTPUTS", "INTERNAL", "GRAPH", "END",
  "INTEGER", "MARKING", "NAME", "INIT_STATE", "DUMMYS", "ATACS", "DISABLE",
  "NOT", "VERIFY", "SEARCH", "REAL", "INF", "KEEPS", "NONINPS",
  "ABSTRACTS", "CONTINUOUS", "RATES", "PROPERTY", "INVARIANTS",
  "ENABLINGS", "ASSIGNS", "RATE_ASSIGNS", "DELAY_ASSIGNS",
  "PRIORITY_ASSIGNS", "BOOL_ASSIGNS", "INT_ASSIGNS", "BOOL_FALSE",
  "BOOL_TRUE", "ASSIGN", "VARIABLES", "INITRATES", "INITVALS", "INITINTS",
  "Pr", "Ss", "AU", "EU", "EG", "EF", "AG", "AF", "PG", "PF", "PU", "PX",
  "INTEGERS", "UNIFORM", "NORMAL", "EXPONENTIAL", "GAMMA", "LOGNORMAL",
  "CHISQ", "LAPLACE", "CAUCHY", "RAYLEIGH", "POISSON", "BINOMIAL",
  "BERNOULLI", "AND", "OR", "XOR", "IMPLIC", "TRANS_RATES", "FAILTRANS",
  "INT", "BIT", "RATE", "BOOL", "NONDISABLING", "'\\n'", "'{'", "'}'",
  "'+'", "'-'", "'['", "','", "']'", "'='", "'?'", "'('", "')'", "'&'",
  "'|'", "'>'", "'<'", "':'", "'~'", "'*'", "'/'", "'%'", "'^'", "';'",
  "'\\\\'", "$accept", "input", "line", "@1", "inputs", "outputs",
  "internals", "variables", "variable", "dummys", "failtrans",
  "nondisabling", "keeps", "abstracts", "noninps", "continuous",
  "property", "probproperty", "probprop", "props", "andprop", "orprop",
  "impliesprop", "prop", "midprop", "propinner", "fronttype", "midtype",
  "bound", "init_state", "place", "marking", "marking_set", "mark",
  "init_rates", "init_rates_set", "init_rate", "init_vals",
  "init_vals_set", "init_val", "rates", "rate_set", "trate", "invariants",
  "invariant_set", "invariant", "edges", "edge", "cons", "expr", "hsf",
  "andexpr", "relation", "arithexpr", "multexpr", "term", "relop", "unop",
  "binop", "ineqs", "hsf_ineqs", "rate_ineqs", "ineq", "rate_ineq",
  "exprd", "plow", "pup", "lower", "upper", "rate", "weight", "node",
  "signal_trans", "enables", "enable_set", "enable", "assigns",
  "assign_set", "assign", "rate_assigns", "rate_assign_set", "rate_assign",
  "delay_assigns", "delay_assign_set", "delay_assign", "priority_assigns",
  "priority_assign_set", "priority_assign", "transition_rates",
  "transition_rate_set", "transition_rate", "bool_assigns",
  "bool_assign_set", "bool_assign", "bool_ineqs", "bool_ineq", "vevents",
  "vevent", "sevents", "sevent", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,    10,   123,   125,    43,    45,    91,
      44,    93,    61,    63,    40,    41,    38,   124,    62,    60,
      58,   126,    42,    47,    37,    94,    59,    92
};
# endif

#define YYPACT_NINF -286

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-286)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      55,   -58,  -286,  -286,  -286,   -35,    -4,     0,   109,  -286,
     904,  -286,    41,  -286,  -286,    42,   129,     8,  -286,    37,
      40,    15,    16,    19,  -286,  -286,    11,    64,    78,    27,
       0,    56,  -286,   113,  -286,  -286,  -286,  -286,  -286,   146,
     334,   162,   165,   171,   174,   210,   224,   226,  -286,   238,
     241,   243,  -286,  -286,  -286,  -286,  -286,  -286,  -286,   156,
     179,   356,  -286,  -286,  -286,  -286,  -286,  -286,  -286,   129,
    -286,    10,  -286,  -286,  -286,   137,  -286,   293,     7,   307,
      28,    26,   257,    29,    30,    31,    33,  -286,   309,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,   377,   389,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,   317,   324,   350,  -286,   148,   836,   676,   676,
     340,  -286,  -286,   -65,  -286,   363,   -50,   371,  -286,   280,
       2,  -286,   365,   380,  -286,   386,  -286,   388,  -286,   399,
    -286,   407,  -286,   412,  -286,   430,  -286,   444,    35,  -286,
     450,  -286,   452,  -286,   459,    47,    53,   401,  -286,  -286,
      79,   360,  -286,  -286,  -286,  -286,  -286,   403,   453,  -286,
    -286,  -286,   -58,   536,  -286,    -7,  -286,    68,  -286,  -286,
    -286,  -286,  -286,  -286,   457,  -286,   214,  -286,   454,   456,
     458,   524,   460,   526,   676,   836,   543,   396,   468,    91,
    -286,   172,  -286,  -286,   462,   218,   218,   465,   466,   467,
     516,   470,   676,   676,   676,   836,   836,  -286,   836,  -286,
     836,   836,   836,   836,   836,   836,   230,  -286,   247,  -286,
     248,  -286,   249,  -286,   258,  -286,   259,  -286,   264,  -286,
    -286,  -286,   265,  -286,   266,  -286,   267,  -286,  -286,  -286,
    -286,  -286,   553,   471,   129,     6,   129,  -286,  -286,   -58,
    -286,   482,    26,   -58,   558,  -286,   106,  -286,    44,  -286,
     559,  -286,   484,  -286,  -286,   485,   486,   487,   181,   439,
     478,  -286,  -286,   363,  -286,   363,  -286,   218,  -286,  -286,
     218,   218,   476,   184,   676,   148,   371,   371,  -286,     2,
       2,   402,  -286,  -286,  -286,  -286,   361,   445,  -286,   559,
    -286,  -286,   129,  -286,  -286,   129,  -286,  -286,   129,  -286,
    -286,   129,  -286,  -286,   129,  -286,  -286,   129,  -286,  -286,
     566,  -286,  -286,   566,  -286,  -286,   129,  -286,   490,   676,
     287,   287,   494,   495,   496,   591,    48,   509,   511,  -286,
    -286,   -58,  -286,   528,    68,   523,   178,   527,   436,   436,
     596,   596,  -286,   836,  -286,   148,   148,  -286,  -286,  -286,
     218,   676,   188,   521,   187,  -286,   836,   531,   532,   533,
     534,   535,   537,   538,  -286,   539,   540,   541,  -286,    -6,
      71,    72,   514,   514,  -286,  -286,  -286,   530,   544,   268,
    -286,  -286,  -286,  -286,   273,   545,   546,    69,   363,   363,
     547,   581,   554,   556,   557,   560,   376,  -286,   -50,  -286,
     -50,  -286,   189,  -286,  -286,  -286,   378,   561,   562,   563,
     564,    67,   565,   567,    58,    61,   583,  -286,  -286,  -286,
     299,  -286,   304,   149,   584,   584,  -286,   354,   597,   598,
     595,   601,   151,   153,   549,   633,   555,   580,   596,   363,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,   676,   676,   559,
     559,   602,   599,   600,   756,   676,   646,   605,   674,    73,
     606,   680,   120,   676,  -286,  -286,  -286,  -286,   323,   330,
     681,   684,   615,   615,   616,   617,   180,   217,  -286,  -286,
     618,   619,   691,   620,   621,   698,  -286,   624,   596,   596,
     625,   596,   -50,   -13,    38,   672,    74,  -286,   673,   196,
    -286,  -286,   704,   676,   631,   199,   201,   679,   200,  -286,
    -286,   626,   635,   358,  -286,   628,   637,   359,   213,  -286,
    -286,  -286,  -286,   331,  -286,   366,   703,   703,   632,   634,
     638,   640,   708,   642,   643,   720,  -286,  -286,   645,  -286,
    -286,   661,   738,   659,   671,  -286,  -286,   669,   676,   670,
     676,   675,   559,   676,   677,   559,   682,   225,   397,   678,
     685,   676,   686,   646,  -286,   405,   327,   689,  -286,   406,
     690,   695,   688,  -286,  -286,   683,   696,  -286,   754,   754,
    -286,  -286,   692,   693,   697,   694,   701,   702,  -286,  -286,
     705,  -286,  -286,  -286,   -50,  -286,   -50,  -286,  -286,   -50,
    -286,  -286,   410,   706,   709,   710,  -286,  -286,   -50,  -286,
    -286,   711,   712,   163,   763,   219,   714,   715,   220,   229,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,   713,  -286,
    -286,   716,   717,   699,   700,  -286,   718,   734,   735,   736,
     719,   792,   721,   722,   755,   793,   749,   750,   758,   761,
     794,   762,   764,   839,  -286,  -286,  -286,   765,   766,  -286,
    -286,  -286,  -286,   767,   768,   769,   770,   771,   775,  -286,
    -286,   772,   773,   781,   776,   777,   782,  -286,  -286,  -286,
     778,  -286,  -286,  -286,   783,  -286,  -286,   784,  -286,  -286,
     785,  -286,  -286,  -286,  -286
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   114,    40,    42,    44,     0,     0,     0,     0,    49,
       0,     4,     0,     3,   267,     0,   168,     0,   266,   268,
     270,     0,     0,     0,     6,     5,     0,     0,     0,     0,
       0,     0,    49,     0,    38,    57,    61,    59,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,     0,    53,    55,     1,     2,     7,   169,     8,   170,
       0,     0,    41,    10,    43,    11,    45,    12,   114,     0,
     122,     0,   117,    36,     9,    50,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   161,     0,   233,
     231,   232,   221,   222,   198,   204,   212,   205,   206,   200,
     201,     0,     0,   104,   105,   102,   103,   226,   227,   213,
     228,   229,   214,   215,   216,   217,   218,   230,   219,   224,
     223,   225,     0,     0,     0,   220,     0,     0,     0,     0,
       0,    73,    74,    87,    96,   110,    75,   179,   181,   185,
     188,   194,     0,     0,   166,     0,   274,     0,   278,     0,
     282,     0,   286,     0,   296,     0,   304,     0,     0,   135,
       0,   148,     0,   300,     0,     0,     0,   173,   269,   271,
       0,     0,   115,   116,    51,    52,    24,     0,     0,   113,
      23,    15,   311,     0,   313,     0,   310,     0,    58,    18,
      62,    20,    60,    19,    64,    21,     0,    27,   207,   210,
     208,     0,   207,     0,     0,     0,     0,     0,     0,     0,
     199,     0,   182,    22,     0,     0,     0,    84,    85,    86,
       0,     0,     0,     0,     0,     0,     0,   207,     0,   189,
       0,     0,     0,     0,     0,     0,     0,    28,     0,    29,
       0,    30,     0,    31,     0,    32,     0,    33,     0,    35,
      47,    13,     0,    25,     0,    26,     0,    34,    54,    16,
      56,    17,     0,   175,     0,     0,     0,   112,   111,   312,
     314,     0,     0,   317,     0,   319,     0,   316,     0,   159,
       0,   160,     0,   211,   209,     0,     0,     0,     0,     0,
       0,   106,   107,   110,    97,   110,   195,     0,    89,    91,
       0,     0,     0,     0,     0,     0,   178,   177,   180,   186,
     187,   184,   190,   191,   192,   193,     0,     0,   164,     0,
     165,   272,     0,   273,   276,     0,   277,   280,     0,   281,
     284,     0,   285,   294,     0,   295,   302,     0,   303,   133,
       0,   134,   146,     0,   147,   298,     0,   299,     0,     0,
     243,   243,     0,     0,     0,     0,     0,     0,     0,    37,
     309,   318,   320,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   197,     0,   196,     0,     0,    93,    88,    90,
       0,     0,     0,     0,     0,   202,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,     0,   174,     0,
       0,     0,   250,   250,   121,   119,   123,     0,     0,     0,
     120,   118,    39,   315,     0,     0,     0,     0,   110,   110,
       0,     0,     0,     0,     0,     0,     0,    98,    99,   100,
     101,    92,     0,   108,    94,    95,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   242,   176,   244,
       0,   245,     0,     0,   257,   257,   124,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   110,
      77,    76,    79,    78,   183,   109,   203,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   246,   248,   247,   249,     0,     0,
       0,     0,   259,   259,     0,     0,     0,     0,    69,    72,
       0,     0,     0,     0,     0,     0,   162,     0,     0,     0,
       0,     0,   237,     0,     0,     0,     0,   235,     0,     0,
     239,   292,     0,     0,   204,     0,     0,     0,     0,   307,
     136,     0,     0,     0,   149,     0,     0,     0,     0,   254,
     253,   256,   255,     0,   258,     0,   262,   262,     0,     0,
       0,     0,     0,     0,     0,     0,    68,    71,     0,    67,
      70,     0,     0,     0,     0,    82,    83,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   137,     0,     0,     0,   150,     0,
       0,     0,     0,   252,   251,     0,     0,   263,   264,   264,
     129,   132,     0,     0,     0,     0,     0,     0,    66,    65,
       0,    80,    81,   167,   236,   275,   240,   279,   234,   241,
     283,   238,     0,     0,     0,     0,   293,   297,   308,   305,
     306,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,   260,   261,   265,   171,   172,   128,   131,     0,   127,
     130,     0,     0,     0,     0,   289,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   126,   125,   163,     0,     0,   290,
     291,   142,   145,     0,     0,     0,     0,     0,     0,   155,
     158,     0,     0,     0,     0,     0,     0,   287,   288,   141,
       0,   144,   140,   143,     0,   154,   157,     0,   153,   156,
       0,   139,   138,   152,   151
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -286,  -286,   844,  -286,  -286,  -286,  -286,  -286,   517,   847,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,   -39,   515,  -286,
    -286,  -286,  -286,  -114,  -286,  -286,  -286,   687,  -285,  -286,
     -26,   850,  -286,   814,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -123,   290,  -116,  -201,   314,  -109,   -93,  -286,  -286,  -286,
     408,  -286,   305,   303,  -286,   542,   488,   434,   387,   335,
     285,   -10,   -67,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,   302,  -286,   641,  -286,   548
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    12,    13,    82,    21,    22,    23,   158,   395,    29,
     165,   166,    83,    85,    84,    86,   130,   421,   422,   132,
     217,   218,   219,   133,   134,   208,   135,   293,   221,    79,
      14,    27,    71,    72,   160,   252,   341,   162,   254,   344,
      88,   196,   281,   145,   236,   320,    15,    16,   263,   350,
     136,   137,   138,   139,   140,   141,   201,   142,   143,   526,
     523,   529,   527,   530,   351,   402,   454,   502,   556,   608,
     654,    17,    18,   147,   238,   323,   149,   240,   326,   151,
     242,   329,   153,   244,   332,   155,   246,   335,   164,   256,
     347,   157,   248,   338,   538,   539,   185,   186,   276,   277
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
      70,   131,   171,   209,   289,   211,    57,    59,   375,   203,
     376,   229,   207,   212,   184,   447,   177,     1,   210,    68,
      68,   354,   178,   214,    62,    64,   222,   311,    66,    19,
      20,   215,   216,   316,   317,   182,    75,    75,   188,   190,
     192,    54,   194,   170,   250,    70,   228,   223,   183,    24,
       1,     2,     3,     4,     5,     6,   258,     7,     8,   365,
       9,    10,   260,   408,     1,     2,     3,     4,     5,     6,
     222,     7,     8,   487,     9,    10,   490,   273,   577,   271,
      25,   288,   481,   272,   464,    26,   449,   451,   542,   448,
     274,   223,    58,   578,   355,   356,   172,   303,   179,    63,
      65,   298,   299,    67,   230,   231,   232,   233,   308,    69,
      69,    76,   181,   189,   191,   193,   270,   195,    28,   251,
     275,   312,   313,   314,   315,    11,    56,   304,   482,   579,
     483,   259,   366,   466,   467,   546,   409,   261,     1,    11,
      60,   291,   292,    61,   578,    78,   488,   489,    73,   491,
     492,    89,    90,    91,    92,    93,   484,    94,   465,   450,
     452,   543,    74,    95,   498,   581,   510,   222,   513,   264,
      96,   265,   426,    97,    98,   499,   167,   511,   670,   514,
     582,   382,   384,   377,   521,   436,   378,   379,   223,    99,
     100,   383,   363,   415,   168,   560,   364,   353,    81,   358,
     103,   104,   105,   106,   416,   184,   561,   362,   547,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   174,   175,   399,   122,   123,   124,
     125,    87,   563,   126,   673,   678,   127,   500,   352,   512,
     357,   515,   128,   564,   681,   674,   679,   144,   222,   129,
     146,   671,   428,   430,   367,   682,   148,   222,   432,   150,
     222,   427,   429,   222,   222,   222,   431,   296,   562,   223,
     103,   104,   105,   106,   381,   222,   372,   222,   223,   433,
     475,   223,   435,   458,   223,   223,   223,   584,   460,   222,
     589,   592,   590,   387,   459,   152,   223,   275,   223,   461,
     279,   222,   585,   126,   602,   565,   593,   675,   680,   154,
     223,   156,   388,   280,   494,   389,   318,   683,   390,   496,
     633,   391,   223,   159,   392,   495,   161,   393,   163,   319,
     497,   424,   425,   321,   324,   327,   397,    89,    90,    91,
      92,    93,   187,    94,   330,   333,   322,   325,   328,    95,
     336,   339,   342,   345,   522,   522,    96,   331,   334,    97,
      98,   535,   536,   337,   340,   343,   346,   225,   226,   504,
     548,   169,   227,   596,   600,    99,   100,   176,   199,   200,
     505,   605,   101,   102,   597,   601,   103,   104,   105,   106,
     400,   180,   606,   197,   401,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     587,   204,   634,   122,   123,   124,   125,   643,   205,   126,
     641,   646,   127,   635,   213,   663,   549,   644,   128,   520,
     550,   642,   647,   551,   603,   129,   664,   552,   604,    89,
      90,    91,    92,    93,   206,    94,   291,   292,   225,   226,
     266,    95,   220,   525,   528,   624,   385,   626,    96,   234,
     629,    97,    98,   225,   226,   225,   226,   224,   638,   198,
     237,   474,   239,   476,   235,   199,   200,    99,   100,   573,
     574,   202,   576,   241,   101,   102,   262,   199,   200,   225,
     226,   243,   418,   419,   267,   420,   245,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   306,   307,   247,   122,   123,   124,   125,    89,
      90,    91,    92,    93,   127,    94,   225,   226,   249,   373,
     128,    95,   225,   226,   253,   386,   255,   129,    96,   309,
     310,    97,    98,   257,   268,   269,   278,   282,   283,   285,
     284,   287,   290,   286,   294,   302,   525,    99,   100,   528,
     297,   300,   348,   301,   305,   349,   359,   361,    68,   368,
     369,   370,   371,   374,   380,   394,   398,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   404,   405,   406,   122,   123,   124,   125,    89,
      90,    91,    92,    93,   127,    94,   407,   410,   227,   411,
     128,    95,   412,   414,   199,   200,   434,   129,    96,   417,
     453,    97,    98,   437,   438,   439,   440,   441,   456,   442,
     443,   444,   445,   446,   457,   462,   463,    99,   100,   469,
     470,   468,   471,   472,   101,   102,   473,   516,   517,   518,
     477,   478,   479,   480,   485,   537,   486,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   493,   501,   519,   122,   123,   124,   125,    89,
      90,    91,    92,    93,   127,    94,   508,   506,   507,   541,
     128,    95,   509,   532,   533,   545,   553,   129,    96,   554,
     531,    97,    98,   540,   544,   555,   568,   558,   559,   566,
     567,   569,   570,   571,   572,   580,   583,    99,   100,   586,
     575,   588,   591,   614,   594,   595,   598,   599,   607,   612,
     610,   613,   611,   615,   616,   617,   618,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   619,   620,   621,   122,   123,   124,   125,    89,
      90,    91,    92,    93,   127,    94,   622,   623,   625,   653,
     128,   534,   632,   627,   651,   630,   636,   129,    96,   645,
     648,    97,    98,   637,   639,   649,   650,   652,   658,   672,
     656,   657,   659,   661,   687,   688,   662,    99,   100,   660,
     666,   667,   668,   669,   665,   676,   677,   694,   698,   703,
     693,   684,   695,   696,   685,   686,   689,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   690,   691,   692,   122,   123,   124,   125,    89,
      90,    91,    92,    93,   127,    94,   697,   699,   700,   701,
     128,    95,   702,   704,   706,   705,    55,   129,    96,   710,
     396,    97,    98,   707,   708,   709,   714,   711,   712,   713,
     715,   716,   717,   720,   718,   719,   721,    99,   100,    80,
      77,   722,   723,   724,   423,   173,   524,   628,   631,   503,
     557,   455,   609,   403,   655,   640,   295,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   413,   360,     0,   122,     0,   124,   125,     0,
      30,     0,    31,    32,   127,     0,     0,    33,    34,     0,
     128,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,     0,     0,     0,     0,    48,    49,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    51,    52,     0,     0,     0,     0,    53
};

static const yytype_int16 yycheck[] =
{
      26,    40,    69,   126,   205,   128,    16,    17,   293,   102,
     295,     9,   126,   129,    81,    21,     9,     9,   127,     9,
       9,    15,    15,    88,     9,     9,    76,   228,     9,    87,
      88,    96,    97,   234,   235,     9,     9,     9,     9,     9,
       9,     0,     9,    69,     9,    71,   139,    97,    22,    84,
       9,    10,    11,    12,    13,    14,     9,    16,    17,    15,
      19,    20,     9,    15,     9,    10,    11,    12,    13,    14,
      76,    16,    17,    15,    19,    20,    15,     9,    91,    86,
      84,   204,    15,    90,    15,    85,    15,    15,    15,    95,
      22,    97,    84,   106,    88,    89,    86,   220,    91,    84,
      84,   215,   216,    84,   102,   103,   104,   105,   224,    99,
      99,    84,    84,    84,    84,    84,   183,    84,     9,    84,
     187,   230,   231,   232,   233,    84,    84,   220,    61,    91,
      63,    84,    88,   418,   419,    15,    88,    84,     9,    84,
     103,    50,    51,   103,   106,    89,    88,    89,    84,    88,
      89,     3,     4,     5,     6,     7,    89,     9,    89,    88,
      88,    88,    84,    15,    15,    91,    15,    76,    15,    90,
      22,    92,   373,    25,    26,    26,    20,    26,    15,    26,
     106,   304,   305,   297,   469,   386,   300,   301,    97,    41,
      42,   305,    86,    15,    15,    15,    90,   264,    85,   266,
      52,    53,    54,    55,    26,   272,    26,   274,    88,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    87,    88,   349,    79,    80,    81,
      82,    85,    15,    85,    15,    15,    88,    88,   264,    88,
     266,    88,    94,    26,    15,    26,    26,    85,    76,   101,
      85,    88,   375,   376,   280,    26,    85,    76,   381,    85,
      76,   375,   376,    76,    76,    76,   380,    95,    88,    97,
      52,    53,    54,    55,    90,    76,    95,    76,    97,    91,
      91,    97,    95,    15,    97,    97,    97,    91,    15,    76,
      91,    91,    91,   319,    26,    85,    97,   364,    97,    26,
      86,    76,   106,    85,    91,    88,   106,    88,    88,    85,
      97,    85,   322,    99,    15,   325,    86,    88,   328,    15,
      95,   331,    97,    85,   334,    26,    85,   337,    85,    99,
      26,   370,   371,    86,    86,    86,   346,     3,     4,     5,
       6,     7,    85,     9,    86,    86,    99,    99,    99,    15,
      86,    86,    86,    86,   477,   478,    22,    99,    99,    25,
      26,   484,   485,    99,    99,    99,    99,    87,    88,    15,
     493,    15,    92,    15,    15,    41,    42,    84,    98,    99,
      26,    15,    48,    49,    26,    26,    52,    53,    54,    55,
     103,    84,    26,    84,   107,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
     533,    94,    15,    79,    80,    81,    82,    90,    94,    85,
      15,    15,    88,    26,    84,    15,   103,   100,    94,   468,
     107,    26,    26,   103,   103,   101,    26,   107,   107,     3,
       4,     5,     6,     7,    94,     9,    50,    51,    87,    88,
      90,    15,    89,   479,   480,   578,    95,   580,    22,    94,
     583,    25,    26,    87,    88,    87,    88,    96,   591,    92,
      84,    95,    84,    95,    94,    98,    99,    41,    42,   518,
     519,    92,   521,    84,    48,    49,    85,    98,    99,    87,
      88,    84,    56,    57,    91,    59,    84,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,   222,   223,    84,    79,    80,    81,    82,     3,
       4,     5,     6,     7,    88,     9,    87,    88,    84,    90,
      94,    15,    87,    88,    84,    90,    84,   101,    22,   225,
     226,    25,    26,    84,    91,     9,    89,    93,    92,    25,
      92,    25,     9,    93,    86,    88,   582,    41,    42,   585,
      98,    96,     9,    97,    94,    94,    84,     9,     9,    85,
      85,    85,    85,    95,    98,     9,    86,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    98,    98,    98,    79,    80,    81,    82,     3,
       4,     5,     6,     7,    88,     9,    15,    98,    92,    98,
      94,    15,    84,    90,    98,    99,    95,   101,    22,    92,
     106,    25,    26,    92,    92,    92,    92,    92,    98,    92,
      92,    92,    92,    92,    90,    90,    90,    41,    42,    58,
      86,    94,    86,    86,    48,    49,    86,    98,    15,    94,
      89,    89,    89,    89,    89,     9,    89,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    89,    89,    94,    79,    80,    81,    82,     3,
       4,     5,     6,     7,    88,     9,    91,    90,    90,    15,
      94,    15,    91,    94,    94,    15,    15,   101,    22,    15,
      98,    25,    26,    98,    98,    90,    15,    91,    91,    91,
      91,    91,    91,    15,    90,    43,    43,    41,    42,    15,
      95,    90,    43,    15,    98,    90,    98,    90,    25,    91,
      98,    91,    98,    91,    91,    15,    91,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    91,    15,    95,    79,    80,    81,    82,     3,
       4,     5,     6,     7,    88,     9,    95,    98,    98,    15,
      94,    15,    90,    98,    91,    98,    98,   101,    22,    90,
      90,    25,    26,    98,    98,    90,    98,    91,    91,    26,
      98,    98,    98,    91,    95,    95,    91,    41,    42,    98,
      91,    91,    91,    91,    98,    91,    91,    15,    15,    15,
      91,    98,    91,    91,    98,    98,    98,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    98,    98,    98,    79,    80,    81,    82,     3,
       4,     5,     6,     7,    88,     9,    91,    98,    98,    91,
      94,    15,    91,    91,    15,    91,    12,   101,    22,    91,
     343,    25,    26,    98,    98,    98,    91,    98,    98,    98,
      98,    98,    91,    91,    98,    98,    98,    41,    42,    32,
      30,    98,    98,    98,   369,    71,   478,   582,   585,   455,
     503,   403,   557,   351,   609,   593,   209,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,   364,   272,    -1,    79,    -1,    81,    82,    -1,
      16,    -1,    18,    19,    88,    -1,    -1,    23,    24,    -1,
      94,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    44,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    83
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     9,    10,    11,    12,    13,    14,    16,    17,    19,
      20,    84,   109,   110,   138,   154,   155,   179,   180,    87,
      88,   112,   113,   114,    84,    84,    85,   139,     9,   117,
      16,    18,    19,    23,    24,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    44,    45,
      46,    77,    78,    83,     0,   110,    84,   179,    84,   179,
     103,   103,     9,    84,     9,    84,     9,    84,     9,    99,
     138,   140,   141,    84,    84,     9,    84,   139,    89,   137,
     117,    85,   111,   120,   122,   121,   123,    85,   148,     3,
       4,     5,     6,     7,     9,    15,    22,    25,    26,    41,
      42,    48,    49,    52,    53,    54,    55,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    79,    80,    81,    82,    85,    88,    94,   101,
     124,   125,   127,   131,   132,   134,   158,   159,   160,   161,
     162,   163,   165,   166,    85,   151,    85,   181,    85,   184,
      85,   187,    85,   190,    85,   193,    85,   199,   115,    85,
     142,    85,   145,    85,   196,   118,   119,    20,    15,    15,
     138,   180,    86,   141,    87,    88,    84,     9,    15,    91,
      84,    84,     9,    22,   180,   204,   205,    85,     9,    84,
       9,    84,     9,    84,     9,    84,   149,    84,    92,    98,
      99,   164,    92,   164,    94,    94,    94,   131,   133,   158,
     163,   158,   160,    84,    88,    96,    97,   128,   129,   130,
      89,   136,    76,    97,    96,    87,    88,    92,   164,     9,
     102,   103,   104,   105,    94,    94,   152,    84,   182,    84,
     185,    84,   188,    84,   191,    84,   194,    84,   200,    84,
       9,    84,   143,    84,   146,    84,   197,    84,     9,    84,
       9,    84,    85,   156,    90,    92,    90,    91,    91,     9,
     180,    86,    90,     9,    22,   180,   206,   207,    89,    86,
      99,   150,    93,    92,    92,    25,    93,    25,   158,   161,
       9,    50,    51,   135,    86,   135,    95,    98,   131,   131,
      96,    97,    88,   158,   164,    94,   159,   159,   160,   162,
     162,   161,   163,   163,   163,   163,   161,   161,    86,    99,
     153,    86,    99,   183,    86,    99,   186,    86,    99,   189,
      86,    99,   192,    86,    99,   195,    86,    99,   201,    86,
      99,   144,    86,    99,   147,    86,    99,   198,     9,    94,
     157,   172,   138,   180,    15,    88,    89,   138,   180,    84,
     205,     9,   180,    86,    90,    15,    88,   138,    85,    85,
      85,    85,    95,    90,    95,   136,   136,   131,   131,   131,
      98,    90,   158,   131,   158,    95,    90,   138,   179,   179,
     179,   179,   179,   179,     9,   116,   116,   179,    86,   158,
     103,   107,   173,   173,    98,    98,    98,    15,    15,    88,
      98,    98,    84,   207,    90,    15,    26,    92,    56,    57,
      59,   125,   126,   126,   125,   125,   161,   131,   158,   131,
     158,   131,   158,    91,    95,    95,   161,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    21,    95,    15,
      88,    15,    88,   106,   174,   174,    98,    90,    15,    26,
      15,    26,    90,    90,    15,    89,   136,   136,    94,    58,
      86,    86,    86,    86,    95,    91,    95,    89,    89,    89,
      89,    15,    61,    63,    89,    89,    89,    15,    88,    89,
      15,    88,    89,    89,    15,    26,    15,    26,    15,    26,
      88,    89,   175,   175,    15,    26,    90,    90,    91,    91,
      15,    26,    88,    15,    26,    88,    98,    15,    94,    94,
     125,   136,   158,   168,   168,   138,   167,   170,   138,   169,
     171,    98,    94,    94,    15,   158,   158,     9,   202,   203,
      98,    15,    15,    88,    98,    15,    15,    88,   158,   103,
     107,   103,   107,    15,    15,    90,   176,   176,    91,    91,
      15,    26,    88,    15,    26,    88,    91,    91,    15,    91,
      91,    15,    90,   125,   125,    95,   125,    91,   106,    91,
      43,    91,   106,    43,    91,   106,    15,   158,    90,    91,
      91,    43,    91,   106,    98,    90,    15,    26,    98,    90,
      15,    26,    91,   103,   107,    15,    26,    25,   177,   177,
      98,    98,    91,    91,    15,    91,    91,    15,    91,    91,
      15,    95,    95,    98,   158,    98,   158,    98,   170,   158,
      98,   171,    90,    95,    15,    26,    98,    98,   158,    98,
     203,    15,    26,    90,   100,    90,    15,    26,    90,    90,
      98,    91,    91,    15,   178,   178,    98,    98,    91,    98,
      98,    91,    91,    15,    26,    98,    91,    91,    91,    91,
      15,    88,    26,    15,    26,    88,    91,    91,    15,    26,
      88,    15,    26,    88,    98,    98,    98,    95,    95,    98,
      98,    98,    98,    91,    15,    91,    91,    91,    15,    98,
      98,    91,    91,    15,    91,    91,    15,    98,    98,    98,
      91,    98,    98,    98,    91,    98,    98,    91,    98,    98,
      91,    98,    98,    98,    98
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   108,   109,   109,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   110,
     112,   112,   113,   113,   114,   114,   115,   115,   116,   117,
     117,   117,   117,   118,   118,   119,   119,   120,   120,   121,
     121,   122,   122,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   124,   124,   125,   125,   125,   125,   125,
     126,   126,   126,   126,   127,   127,   127,   127,   128,   128,
     129,   129,   130,   130,   131,   131,   131,   132,   133,   133,
     133,   133,   134,   134,   134,   134,   135,   135,   136,   136,
     136,   137,   137,   137,   138,   139,   140,   140,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   142,   143,   143,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   145,   146,   146,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   148,
     149,   149,   150,   150,   151,   152,   152,   153,   154,   155,
     155,   155,   155,   156,   156,   157,   157,   158,   158,   158,
     159,   159,   160,   160,   160,   160,   161,   161,   161,   162,
     162,   162,   162,   162,   162,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   164,   164,   164,
     164,   164,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   167,   167,   168,   168,   169,   169,
     170,   171,   172,   173,   173,   173,   173,   173,   173,   173,
     174,   174,   174,   174,   174,   174,   174,   175,   175,   176,
     176,   176,   177,   177,   178,   178,   179,   179,   180,   180,
     180,   180,   181,   182,   182,   183,   184,   185,   185,   186,
     187,   188,   188,   189,   190,   191,   191,   192,   192,   192,
     192,   192,   192,   192,   193,   194,   194,   195,   196,   197,
     197,   198,   199,   200,   200,   201,   202,   202,   203,   204,
     204,   205,   205,   205,   205,   206,   206,   207,   207,   207,
     207
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     2,     2,     2,     2,     3,
       3,     3,     3,     4,     3,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     3,     6,     0,     7,
       0,     2,     0,     2,     0,     2,     0,     2,     1,     0,
       2,     3,     3,     0,     2,     0,     2,     0,     2,     0,
       2,     0,     2,     0,     2,     9,     9,     8,     8,     7,
       8,     8,     7,     1,     1,     1,     6,     6,     6,     6,
       5,     5,     4,     4,     2,     2,     2,     1,     3,     2,
       3,     2,     4,     3,     5,     5,     1,     3,     4,     4,
       4,     4,     1,     1,     1,     1,     1,     1,     4,     5,
       0,     3,     3,     2,     1,     3,     2,     1,     5,     5,
       5,     5,     1,     5,     6,    11,    11,    10,    10,     9,
      10,    10,     9,     3,     2,     0,     5,     6,    11,    11,
      10,    10,     9,    10,    10,     9,     3,     2,     0,     5,
       6,    11,    11,    10,    10,     9,    10,    10,     9,     3,
       2,     0,     5,     9,     3,     2,     0,     7,     1,     2,
       2,    11,    11,     0,     3,     0,     3,     3,     3,     1,
       3,     1,     2,     6,     3,     1,     3,     3,     1,     2,
       3,     3,     3,     3,     1,     3,     4,     4,     1,     2,
       1,     1,     4,     6,     1,     1,     1,     1,     1,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     3,     3,     0,     2,     2,     3,     3,     3,     3,
       0,     4,     4,     3,     3,     3,     3,     0,     2,     0,
       3,     3,     0,     1,     0,     1,     1,     1,     2,     4,
       2,     4,     3,     2,     0,     7,     3,     2,     0,     7,
       3,     2,     0,     7,     3,     2,     0,    10,    10,     8,
       9,     9,     5,     7,     3,     2,     0,     7,     3,     2,
       0,     7,     3,     2,     0,     7,     3,     1,     3,     3,
       1,     1,     2,     1,     2,     3,     1,     1,     2,     1,
       2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 9:
#line 35 "gparse.y" /* yacc.c:1646  */
    { free((yyvsp[-1])); }
#line 1869 "gparse.c" /* yacc.c:1646  */
    break;

  case 38:
#line 65 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (DUMMY_NODE, "dummy");
		    Node_Ptr n = Enter_Place ("dummy_p");
		    Enter_Pred (n, (yyval));
		    Enter_Succ (n,(yyval),0,INFIN,0,(-1)*INFIN,INFIN,NULL,
				NULL,NULL,false,0.0,1);
		  }
#line 1881 "gparse.c" /* yacc.c:1646  */
    break;

  case 41:
#line 76 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (INPUT_NODE, (char*)(yyvsp[0]));
		  }
#line 1889 "gparse.c" /* yacc.c:1646  */
    break;

  case 43:
#line 83 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (OUTPUT_NODE, (char*)(yyvsp[0]));
		  }
#line 1897 "gparse.c" /* yacc.c:1646  */
    break;

  case 45:
#line 90 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (INTERNAL_NODE, (char*)(yyvsp[0]));
		  }
#line 1905 "gparse.c" /* yacc.c:1646  */
    break;

  case 47:
#line 97 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Variable((char*)(yyvsp[0]));
		  }
#line 1913 "gparse.c" /* yacc.c:1646  */
    break;

  case 48:
#line 103 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Variable((char*)(yyvsp[0]));
		  }
#line 1921 "gparse.c" /* yacc.c:1646  */
    break;

  case 50:
#line 110 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (DUMMY_NODE, (char*)(yyvsp[0]));
		  }
#line 1929 "gparse.c" /* yacc.c:1646  */
    break;

  case 51:
#line 114 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (DUMMY_NODE, strcat((char*)(yyvsp[-1]),"+"));
		  }
#line 1937 "gparse.c" /* yacc.c:1646  */
    break;

  case 52:
#line 118 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_IO (DUMMY_NODE, strcat((char*)(yyvsp[-1]),"-"));
		  }
#line 1945 "gparse.c" /* yacc.c:1646  */
    break;

  case 54:
#line 125 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_FailTrans ((char*)(yyvsp[0]));
		  }
#line 1953 "gparse.c" /* yacc.c:1646  */
    break;

  case 56:
#line 132 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_NonDisabling ((char*)(yyvsp[0]));
		  }
#line 1961 "gparse.c" /* yacc.c:1646  */
    break;

  case 58:
#line 139 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Keep ((char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 1970 "gparse.c" /* yacc.c:1646  */
    break;

  case 60:
#line 147 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Abstract ((char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 1979 "gparse.c" /* yacc.c:1646  */
    break;

  case 62:
#line 155 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_NonInp ((char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 1988 "gparse.c" /* yacc.c:1646  */
    break;

  case 64:
#line 163 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[0]),(-1)*INFIN,INFIN);
		    free((yyvsp[0]));
		  }
#line 1997 "gparse.c" /* yacc.c:1646  */
    break;

  case 65:
#line 168 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-7]),(-1)*INFIN,(-1)*atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-7]));
		    free((yyvsp[-1]));
		  }
#line 2007 "gparse.c" /* yacc.c:1646  */
    break;

  case 66:
#line 174 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-7]),(-1)*atoi((char*)(yyvsp[-4])),
				(-1)*atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-7]));
		    free((yyvsp[-4]));
		    free((yyvsp[-1]));
		  }
#line 2019 "gparse.c" /* yacc.c:1646  */
    break;

  case 67:
#line 182 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-6]),(-1)*INFIN,atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-6]));
		    free((yyvsp[-1]));
		  }
#line 2029 "gparse.c" /* yacc.c:1646  */
    break;

  case 68:
#line 188 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-6]),(-1)*atoi((char*)(yyvsp[-3])),
				atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-6]));
		    free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2041 "gparse.c" /* yacc.c:1646  */
    break;

  case 69:
#line 196 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-5]),atoi((char*)(yyvsp[-3])),atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-5]));
		    free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2052 "gparse.c" /* yacc.c:1646  */
    break;

  case 70:
#line 203 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-6]),(-1)*INFIN,INFIN);
		    free((yyvsp[-6]));
		  }
#line 2061 "gparse.c" /* yacc.c:1646  */
    break;

  case 71:
#line 208 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-6]),(-1)*atoi((char*)(yyvsp[-3])),INFIN);
		    free((yyvsp[-6]));
		    free((yyvsp[-3]));
		  }
#line 2071 "gparse.c" /* yacc.c:1646  */
    break;

  case 72:
#line 214 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Cont ((char*)(yyvsp[-5]),atoi((char*)(yyvsp[-3])),INFIN);
		    free((yyvsp[-5]));
		    free((yyvsp[-3]));
		  }
#line 2081 "gparse.c" /* yacc.c:1646  */
    break;

  case 73:
#line 221 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Prop((char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 2090 "gparse.c" /* yacc.c:1646  */
    break;

  case 74:
#line 226 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Prop((char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 2099 "gparse.c" /* yacc.c:1646  */
    break;

  case 76:
#line 234 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-4]))+strlen((char*)(yyvsp[-3]))+
					strlen((char*)(yyvsp[-1]))+5);
		  strcpy((char*)(yyval),"Pr");
		  strcat((char*)(yyval), (char*)(yyvsp[-4]));
		  strcat((char*)(yyval), (char*)(yyvsp[-3]));
		  strcat((char*)(yyval), "{");
		  strcat((char*)(yyval), (char*)(yyvsp[-1]));
		  strcat((char*)(yyval), "}");
		  free((yyvsp[-4]));
		  free((yyvsp[-3]));
		  free((yyvsp[-1])); 
		}
#line 2117 "gparse.c" /* yacc.c:1646  */
    break;

  case 77:
#line 248 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+7);
		  strcpy((char*)(yyval),"Pr");
		  strcat((char*)(yyval),"=");
		  strcat((char*)(yyval),"?");
		  strcat((char*)(yyval), "{");
		  strcat((char*)(yyval), (char*)(yyvsp[-1]));
		  strcat((char*)(yyval), "}");
		  free((yyvsp[-1])); 
		}
#line 2132 "gparse.c" /* yacc.c:1646  */
    break;

  case 78:
#line 259 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-4]))+strlen((char*)(yyvsp[-3]))+
					strlen((char*)(yyvsp[-1]))+5);
		  strcpy((char*)(yyval),"SS");
		  strcat((char*)(yyval), (char*)(yyvsp[-4]));
		  strcat((char*)(yyval), (char*)(yyvsp[-3]));
		  strcat((char*)(yyval), "{");
		  strcat((char*)(yyval), (char*)(yyvsp[-1]));
		  strcat((char*)(yyval), "}");
		  free((yyvsp[-4]));
		  free((yyvsp[-3]));
		  free((yyvsp[-1])); 
		}
#line 2150 "gparse.c" /* yacc.c:1646  */
    break;

  case 79:
#line 273 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+7);
		  strcpy((char*)(yyval),"SS");
		  strcat((char*)(yyval),"=");
		  strcat((char*)(yyval),"?");
		  strcat((char*)(yyval), "{");
		  strcat((char*)(yyval), (char*)(yyvsp[-1]));
		  strcat((char*)(yyval), "}");
		  free((yyvsp[-1])); 
		}
#line 2165 "gparse.c" /* yacc.c:1646  */
    break;

  case 80:
#line 286 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-3])) boundlen = strlen((char*)(yyvsp[-3]));
		    (yyval) = (Node*)GetBlock(boundlen+
					 strlen((char*)(yyvsp[-1]))+5);
		    strcat((char*)(yyval),"PG(");
		    if ((yyvsp[-3])) strcat((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    if ((yyvsp[-3])) free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2182 "gparse.c" /* yacc.c:1646  */
    break;

  case 81:
#line 299 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-3])) boundlen = strlen((char*)(yyvsp[-3]));
		    (yyval) = (Node*)GetBlock(boundlen+
					 strlen((char*)(yyvsp[-1]))+5);
		    strcat((char*)(yyval),"PF(");
		    if ((yyvsp[-3])) strcat((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    if ((yyvsp[-3])) free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2199 "gparse.c" /* yacc.c:1646  */
    break;

  case 82:
#line 312 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock(strlen((char*)(yyvsp[-1]))+5);
		    strcat((char*)(yyval),"PX(");
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    free((yyvsp[-1]));
		  }
#line 2211 "gparse.c" /* yacc.c:1646  */
    break;

  case 83:
#line 320 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-1])) boundlen = strlen((char*)(yyvsp[-1]));
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  boundlen+4+
					  strlen((char*)(yyvsp[0])));
		    strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),"PU");
		    if ((yyvsp[-1])) strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-3]));
		    if ((yyvsp[-1])) free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2230 "gparse.c" /* yacc.c:1646  */
    break;

  case 84:
#line 337 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),"&");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2244 "gparse.c" /* yacc.c:1646  */
    break;

  case 85:
#line 347 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),"|");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2258 "gparse.c" /* yacc.c:1646  */
    break;

  case 86:
#line 357 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+
					  strlen((char*)(yyvsp[0]))+3);
		    strcpy((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),"->");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2272 "gparse.c" /* yacc.c:1646  */
    break;

  case 88:
#line 370 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),"&");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-2]));
		    free((yyvsp[0]));
		  }
#line 2286 "gparse.c" /* yacc.c:1646  */
    break;

  case 89:
#line 380 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (yyvsp[0]);
		  }
#line 2294 "gparse.c" /* yacc.c:1646  */
    break;

  case 90:
#line 385 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),"|");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-2]));
		    free((yyvsp[0]));
		  }
#line 2308 "gparse.c" /* yacc.c:1646  */
    break;

  case 91:
#line 395 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (yyvsp[0]);
		  }
#line 2316 "gparse.c" /* yacc.c:1646  */
    break;

  case 92:
#line 401 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  strlen((char*)(yyvsp[0]))+3);
		    strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),"->");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-3]));
		    free((yyvsp[0]));
                  }
#line 2330 "gparse.c" /* yacc.c:1646  */
    break;

  case 93:
#line 411 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[0]);
		  }
#line 2338 "gparse.c" /* yacc.c:1646  */
    break;

  case 94:
#line 419 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-3])) boundlen = strlen((char*)(yyvsp[-3]));
		    (yyval) = (Node*)GetBlock(strlen((char*)(yyvsp[-4]))+
					 boundlen+
					 strlen((char*)(yyvsp[-1]))+3);
		    strcpy((char*)(yyval),(char*)(yyvsp[-4]));
		    strcat((char*)(yyval),"(");
		    if ((yyvsp[-3])) strcat((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    if ((yyvsp[-3])) free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2357 "gparse.c" /* yacc.c:1646  */
    break;

  case 95:
#line 434 "gparse.y" /* yacc.c:1646  */
    {
		    
		    int boundlen = 0;
		    if ((yyvsp[-3])) boundlen = strlen((char*)(yyvsp[-3]));
		    (yyval) = (Node*)GetBlock(strlen((char*)(yyvsp[-4]))+
					 boundlen+
					 strlen((char*)(yyvsp[-1]))+3);
		    strcpy((char*)(yyval),(char*)(yyvsp[-4]));
		    strcat((char*)(yyval),"(");
		    if ((yyvsp[-3])) strcat((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    if((yyvsp[-3])) free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2377 "gparse.c" /* yacc.c:1646  */
    break;

  case 97:
#line 453 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+3);
		    strcpy((char*)(yyval),"{");
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),"}");
		    free((yyvsp[-1]));
		  }
#line 2389 "gparse.c" /* yacc.c:1646  */
    break;

  case 98:
#line 463 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-1])) boundlen = strlen((char*)(yyvsp[-1]));
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  strlen((char*)(yyvsp[-2]))+
					  boundlen+1+
					  strlen((char*)(yyvsp[0])));
		    strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-2]));
		    if ((yyvsp[-1])) strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-3]));
		    if ((yyvsp[-1])) free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2409 "gparse.c" /* yacc.c:1646  */
    break;

  case 99:
#line 479 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-1])) boundlen = strlen((char*)(yyvsp[-1]));
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  strlen((char*)(yyvsp[-2]))+
					  boundlen+1+
					  strlen((char*)(yyvsp[0])));
		    strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-2]));
		    if ((yyvsp[-1])) strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-3]));
		    if ((yyvsp[-1])) free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2429 "gparse.c" /* yacc.c:1646  */
    break;

  case 100:
#line 495 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-1])) boundlen = strlen((char*)(yyvsp[-1]));
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  strlen((char*)(yyvsp[-2]))+
					  boundlen+1+
					  strlen((char*)(yyvsp[0])));

		    strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-2]));
		    if ((yyvsp[-1])) strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-3]));
		    if ((yyvsp[-1])) free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2450 "gparse.c" /* yacc.c:1646  */
    break;

  case 101:
#line 512 "gparse.y" /* yacc.c:1646  */
    {
		    int boundlen = 0;
		    if ((yyvsp[-1])) boundlen = strlen((char*)(yyvsp[-1]));
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  strlen((char*)(yyvsp[-2]))+
					  boundlen+1+
					  strlen((char*)(yyvsp[0])));
		    strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		    strcat((char*)(yyval),(char*)(yyvsp[-2]));
		    if ((yyvsp[-1])) strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-3]));
		    if ((yyvsp[-1])) free((yyvsp[-1]));
		    free((yyvsp[0]));
		  }
#line 2470 "gparse.c" /* yacc.c:1646  */
    break;

  case 102:
#line 529 "gparse.y" /* yacc.c:1646  */
    {(yyval)=(Node*)CopyString("AG");}
#line 2476 "gparse.c" /* yacc.c:1646  */
    break;

  case 103:
#line 530 "gparse.y" /* yacc.c:1646  */
    {(yyval)=(Node*)CopyString("AF");}
#line 2482 "gparse.c" /* yacc.c:1646  */
    break;

  case 104:
#line 531 "gparse.y" /* yacc.c:1646  */
    {(yyval)=(Node*)CopyString("EG");}
#line 2488 "gparse.c" /* yacc.c:1646  */
    break;

  case 105:
#line 532 "gparse.y" /* yacc.c:1646  */
    {(yyval)=(Node*)CopyString("EF");}
#line 2494 "gparse.c" /* yacc.c:1646  */
    break;

  case 106:
#line 535 "gparse.y" /* yacc.c:1646  */
    {(yyval)=(Node*)CopyString("AU");}
#line 2500 "gparse.c" /* yacc.c:1646  */
    break;

  case 107:
#line 536 "gparse.y" /* yacc.c:1646  */
    {(yyval)=(Node*)CopyString("EU");}
#line 2506 "gparse.c" /* yacc.c:1646  */
    break;

  case 108:
#line 540 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[-1]))+3);
		    strcpy((char*)(yyval),"[");
		    strcat((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),"]");
		    free((yyvsp[-2]));
		    free((yyvsp[-1]));
		  }
#line 2521 "gparse.c" /* yacc.c:1646  */
    break;

  case 109:
#line 551 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					  strlen((char*)(yyvsp[-1]))+4);
		    strcpy((char*)(yyval),"[");
		    strcat((char*)(yyval),(char*)(yyvsp[-3]));
		    strcpy((char*)(yyval),",");
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),"]");
		    free((yyvsp[-3]));
		    free((yyvsp[-1]));
		  }
#line 2537 "gparse.c" /* yacc.c:1646  */
    break;

  case 110:
#line 562 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL;}
#line 2543 "gparse.c" /* yacc.c:1646  */
    break;

  case 111:
#line 566 "gparse.y" /* yacc.c:1646  */
    {
		    Set_Initial_State((char*)(yyvsp[-1]));
		    free((yyvsp[-1]));
		  }
#line 2552 "gparse.c" /* yacc.c:1646  */
    break;

  case 112:
#line 571 "gparse.y" /* yacc.c:1646  */
    {
		    Set_Initial_State((char*)(yyvsp[-1]));
		    free((yyvsp[-1]));
		  }
#line 2561 "gparse.c" /* yacc.c:1646  */
    break;

  case 114:
#line 579 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Place ((char*)(yyvsp[0]));
		  }
#line 2569 "gparse.c" /* yacc.c:1646  */
    break;

  case 118:
#line 592 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, EDGE_MARK, (yyvsp[-3]), (yyvsp[-1]), 1, 1);
		  }
#line 2577 "gparse.c" /* yacc.c:1646  */
    break;

  case 119:
#line 596 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, EDGE_MARK, (yyvsp[-3]), (yyvsp[-1]), 1, 1);
		  }
#line 2585 "gparse.c" /* yacc.c:1646  */
    break;

  case 120:
#line 600 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, EDGE_MARK, (yyvsp[-3]), (yyvsp[-1]), 1, 1);
		  }
#line 2593 "gparse.c" /* yacc.c:1646  */
    break;

  case 121:
#line 604 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, EDGE_MARK, (yyvsp[-3]), (yyvsp[-1]), 1, 1);
		  }
#line 2601 "gparse.c" /* yacc.c:1646  */
    break;

  case 122:
#line 608 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL, (yyvsp[0]), 1, 1);
		  }
#line 2609 "gparse.c" /* yacc.c:1646  */
    break;

  case 123:
#line 612 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				(yyvsp[-3]),atoi((char*)(yyvsp[-1])),
				atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-1]));
		  }
#line 2620 "gparse.c" /* yacc.c:1646  */
    break;

  case 124:
#line 619 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,(yyvsp[-4]),
				(-1)*atoi((char*)(yyvsp[-2])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2630 "gparse.c" /* yacc.c:1646  */
    break;

  case 125:
#line 625 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				(yyvsp[-9]),(-1)*INFIN,
				(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2641 "gparse.c" /* yacc.c:1646  */
    break;

  case 126:
#line 632 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,(yyvsp[-9]),
				(-1)*atoi((char*)(yyvsp[-5])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-5]));
		    free((yyvsp[-2]));
		  }
#line 2652 "gparse.c" /* yacc.c:1646  */
    break;

  case 127:
#line 639 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				(yyvsp[-8]),(-1)*INFIN,
				(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2663 "gparse.c" /* yacc.c:1646  */
    break;

  case 128:
#line 646 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,(yyvsp[-8]),
				(-1)*atoi((char*)(yyvsp[-4])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2674 "gparse.c" /* yacc.c:1646  */
    break;

  case 129:
#line 653 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				(yyvsp[-7]),atoi((char*)(yyvsp[-4])),
				atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2686 "gparse.c" /* yacc.c:1646  */
    break;

  case 130:
#line 661 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,(yyvsp[-8]),
				(-1)*INFIN,INFIN);
		  }
#line 2695 "gparse.c" /* yacc.c:1646  */
    break;

  case 131:
#line 666 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,(yyvsp[-8]),
				(-1)*atoi((char*)(yyvsp[-4])),INFIN);
		    free((yyvsp[-4]));
		  }
#line 2705 "gparse.c" /* yacc.c:1646  */
    break;

  case 132:
#line 673 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				(yyvsp[-7]),atoi((char*)(yyvsp[-4])),
				INFIN);
		    free((yyvsp[-4]));
		  }
#line 2716 "gparse.c" /* yacc.c:1646  */
    break;

  case 136:
#line 689 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				(yyvsp[-3]),atoi((char*)(yyvsp[-1])),atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-1]));
		  }
#line 2726 "gparse.c" /* yacc.c:1646  */
    break;

  case 137:
#line 695 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,(yyvsp[-4]),
				(-1)*atoi((char*)(yyvsp[-1])),(-1)*atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-2]));
		  }
#line 2736 "gparse.c" /* yacc.c:1646  */
    break;

  case 138:
#line 701 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				(yyvsp[-9]),(-1)*INFIN,(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2746 "gparse.c" /* yacc.c:1646  */
    break;

  case 139:
#line 707 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,(yyvsp[-9]),
				(-1)*atoi((char*)(yyvsp[-5])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-5]));
		    free((yyvsp[-2]));
		  }
#line 2757 "gparse.c" /* yacc.c:1646  */
    break;

  case 140:
#line 714 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				(yyvsp[-8]),(-1)*INFIN,(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2767 "gparse.c" /* yacc.c:1646  */
    break;

  case 141:
#line 720 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,(yyvsp[-8]),
				(-1)*atoi((char*)(yyvsp[-4])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2778 "gparse.c" /* yacc.c:1646  */
    break;

  case 142:
#line 727 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				(yyvsp[-7]),atoi((char*)(yyvsp[-4])),atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2789 "gparse.c" /* yacc.c:1646  */
    break;

  case 143:
#line 734 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,(yyvsp[-8]),
				(-1)*INFIN,INFIN);
		  }
#line 2798 "gparse.c" /* yacc.c:1646  */
    break;

  case 144:
#line 739 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,(yyvsp[-8]),
				(-1)*atoi((char*)(yyvsp[-4])),INFIN);
		    free((yyvsp[-4]));
		  }
#line 2808 "gparse.c" /* yacc.c:1646  */
    break;

  case 145:
#line 746 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				(yyvsp[-7]),atoi((char*)(yyvsp[-4])),INFIN);
		    free((yyvsp[-4]));
		  }
#line 2818 "gparse.c" /* yacc.c:1646  */
    break;

  case 149:
#line 762 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				(yyvsp[-3]),atoi((char*)(yyvsp[-1])),
				atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-1]));
		  }
#line 2829 "gparse.c" /* yacc.c:1646  */
    break;

  case 150:
#line 769 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,(yyvsp[-4]),
				(-1)*atoi((char*)(yyvsp[-1])),(-1)*atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-2]));
		  }
#line 2839 "gparse.c" /* yacc.c:1646  */
    break;

  case 151:
#line 775 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				(yyvsp[-9]),(-1)*INFIN,
				(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2850 "gparse.c" /* yacc.c:1646  */
    break;

  case 152:
#line 782 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,(yyvsp[-9]),
				(-1)*atoi((char*)(yyvsp[-5])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-5]));
		    free((yyvsp[-2]));
		  }
#line 2861 "gparse.c" /* yacc.c:1646  */
    break;

  case 153:
#line 789 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				(yyvsp[-8]),(-1)*INFIN,
				(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-2]));
		  }
#line 2872 "gparse.c" /* yacc.c:1646  */
    break;

  case 154:
#line 796 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking,VAR_MARK, NULL,(yyvsp[-8]),
				(-1)*atoi((char*)(yyvsp[-4])),(-1)*atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2883 "gparse.c" /* yacc.c:1646  */
    break;

  case 155:
#line 803 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				(yyvsp[-7]),atoi((char*)(yyvsp[-4])),
				atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2895 "gparse.c" /* yacc.c:1646  */
    break;

  case 156:
#line 811 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,(yyvsp[-8]),
				(-1)*INFIN,INFIN);
		  }
#line 2904 "gparse.c" /* yacc.c:1646  */
    break;

  case 157:
#line 816 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,(yyvsp[-8]),
				(-1)*atoi((char*)(yyvsp[-4])),INFIN);
		    free((yyvsp[-4]));
		  }
#line 2914 "gparse.c" /* yacc.c:1646  */
    break;

  case 158:
#line 823 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				(yyvsp[-7]),atoi((char*)(yyvsp[-4])),
				INFIN);
		    free((yyvsp[-4]));
		  }
#line 2925 "gparse.c" /* yacc.c:1646  */
    break;

  case 162:
#line 839 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Trate ((yyvsp[-3]),atoi((char*)(yyvsp[-1])),atoi((char*)(yyvsp[-1])));
		    free((yyvsp[-1]));
		  }
#line 2934 "gparse.c" /* yacc.c:1646  */
    break;

  case 163:
#line 844 "gparse.y" /* yacc.c:1646  */
    {
		    Enter_Trate ((yyvsp[-7]),atoi((char*)(yyvsp[-4])),atoi((char*)(yyvsp[-2])));
		    free((yyvsp[-4]));
		    free((yyvsp[-2]));
		  }
#line 2944 "gparse.c" /* yacc.c:1646  */
    break;

  case 167:
#line 859 "gparse.y" /* yacc.c:1646  */
    { 
		   string hsfs = (char*)(yyvsp[-2]);
		  ineqADT ineqs = NULL;
		  ineqADT temp;
		  string hsf;
		  while (hsfs.find("&") != string::npos ||
			 hsfs.find("|") != string::npos) {
		    if (hsfs.find("&") < hsfs.find("|")) {
		      hsf = hsfs.substr(0,hsfs.find("&"));
		      hsfs = hsfs.substr(hsfs.find("&")+1,string::npos);
		    } else {
		      hsf = hsfs.substr(0,hsfs.find("|"));
		      hsfs = hsfs.substr(hsfs.find("|")+1,string::npos);
		    }

		    temp = parsehsf(hsf);
		    if (temp) {
		      if (!ineqs)
			ineqs = temp;
		      else {
			for (ineqADT step = ineqs; step;
			     step = step->next) {
			  if (!step->next) {
			    step->next = temp;
			    break;
			  }
			}
		      }
		    }
		  }
		  hsf = hsfs;
		  temp = parsehsf(hsf);
		  if (temp) {
		    if (!ineqs)
		      ineqs = temp;
		    else {
		      for (ineqADT step = ineqs; step;
			   step = step->next) {
			if (!step->next) {
			  step->next = temp;
			  break;
			}
		      }
		    }
		  }
		  if (!(yyvsp[-5])->inequalities) {
		    (yyvsp[-5])->inequalities = ineqs;
		  }
		  else {
		    for (ineqADT step = (yyvsp[-5])->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = temp;
			break;
		      }
		    }
		  }
		  Enter_Hsl((yyvsp[-5]),(char*)(yyvsp[-2]));
		  free((yyvsp[-2]));
	        }
#line 3009 "gparse.c" /* yacc.c:1646  */
    break;

  case 168:
#line 922 "gparse.y" /* yacc.c:1646  */
    {
		  }
#line 3016 "gparse.c" /* yacc.c:1646  */
    break;

  case 169:
#line 927 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[-1]);
		    Enter_Pred ((yyvsp[-1]), (yyvsp[0]));
		    Enter_Succ ((yyvsp[-1]), (yyvsp[0]), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL,
				NULL,NULL,false, 1.0, 1);
		  }
#line 3027 "gparse.c" /* yacc.c:1646  */
    break;

  case 170:
#line 934 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[-1]);
		    Enter_Pred ((yyvsp[-1]), (yyvsp[0]));
		    Enter_Succ ((yyvsp[-1]), (yyvsp[0]), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL,
				NULL,NULL, false, 1.0, 1);
		  }
#line 3038 "gparse.c" /* yacc.c:1646  */
    break;

  case 171:
#line 941 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[-10]);
		    int predtype=0;
		    int plower=-INFIN;
		    if ((yyvsp[-5])) {
		      if (strchr((char*)(yyvsp[-5]),'\\'))
			predtype=2;
		      *(((char*)(yyvsp[-5]))+strlen((char*)(yyvsp[-5]))-1)='\0';
		      if (strcmp((char*)(yyvsp[-5]),"-inf")!=0)
			plower=atoi((char*)(yyvsp[-5]));
		      free((yyvsp[-5]));
		    }
		    int pupper=INFIN;
		    if ((yyvsp[-4])) { 
		      if (strchr((char*)(yyvsp[-4]),'/'))
			predtype=predtype+1;
		      *(((char*)(yyvsp[-4]))+strlen((char*)(yyvsp[-4]))-1)='\0';
		      if (strcmp((char*)(yyvsp[-4]),"inf")!=0)
			pupper=atoi((char*)(yyvsp[-4]));
		      free((yyvsp[-4]));
		    }
		    int lower=0;
		    if ((yyvsp[-3])) {
		      lower=atoi((char*)(yyvsp[-3]));
		      free((yyvsp[-3]));
		    }
		    int upper=INFIN;
		    if ((yyvsp[-2])) {
		      if (strcmp((char*)(yyvsp[-2]),"inf")!=0)
			upper=atoi((char*)(yyvsp[-2]));
		      free((yyvsp[-2]));
		    }
		    double rate=1.0;
		    if ((yyvsp[-1])) {
		      rate=atof((char*)(yyvsp[-1]));
		      free((yyvsp[-1]));
		    }
		    int weight=1;
		    if ((yyvsp[0])) {
		      weight=atoi((char*)(yyvsp[0]));
		      free((yyvsp[0]));
		    }
		    Enter_Pred ((yyvsp[-10]), (yyvsp[-9]));
		    Enter_Succ ((yyvsp[-10]), (yyvsp[-9]),lower,upper,predtype,plower,pupper,
				(char*)(yyvsp[-6]), NULL,(char*)(yyvsp[-7]),false,rate,weight);
		  }
#line 3089 "gparse.c" /* yacc.c:1646  */
    break;

  case 172:
#line 988 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[-10]);
		    int predtype=0;
		    int plower=-INFIN;
		    if ((yyvsp[-5])) {
		      if (strchr((char*)(yyvsp[-5]),'\\'))
			predtype=2;
		      *(((char*)(yyvsp[-5]))+strlen((char*)(yyvsp[-5]))-1)='\0';
		      if (strcmp((char*)(yyvsp[-5]),"-inf")!=0)
			plower=atoi((char*)(yyvsp[-5]));
		      free((yyvsp[-5]));
		    }
		    int pupper=INFIN;
		    if ((yyvsp[-4])) {
		      if (strchr((char*)(yyvsp[-4]),'/'))
			predtype=predtype+1;
		      *(((char*)(yyvsp[-4]))+strlen((char*)(yyvsp[-4]))-1)='\0';
		      if (strcmp((char*)(yyvsp[-4]),"inf")!=0)
			pupper=atoi((char*)(yyvsp[-4]));
		      free((yyvsp[-4]));
		    }
		    int lower=0;
		    if ((yyvsp[-3])) {
		      lower=atoi((char*)(yyvsp[-3]));
		      free((yyvsp[-3]));
		    }
		    int upper=INFIN;
		    if ((yyvsp[-2])) {
		      if (strcmp((char*)(yyvsp[-2]),"inf")!=0)
			upper=atoi((char*)(yyvsp[-2]));
		      free((yyvsp[-2]));
		    }
		    double rate=1.0;
		    if ((yyvsp[-1])) {
		      rate=atof((char*)(yyvsp[-1]));
		      free((yyvsp[-1]));
		    }
		    int weight=1;
		    if ((yyvsp[0])) {
		      weight=atoi((char*)(yyvsp[0]));
		      free((yyvsp[0]));
		    }
		    Enter_Pred ((yyvsp[-10]), (yyvsp[-9]));
		    Enter_Succ ((yyvsp[-10]), (yyvsp[-9]),lower,upper,predtype,plower,pupper,
				(char*)(yyvsp[-6]),NULL,(char*)(yyvsp[-7]),true,rate,weight);
		  }
#line 3140 "gparse.c" /* yacc.c:1646  */
    break;

  case 173:
#line 1036 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3146 "gparse.c" /* yacc.c:1646  */
    break;

  case 174:
#line 1037 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3152 "gparse.c" /* yacc.c:1646  */
    break;

  case 175:
#line 1039 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3158 "gparse.c" /* yacc.c:1646  */
    break;

  case 176:
#line 1040 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3164 "gparse.c" /* yacc.c:1646  */
    break;

  case 177:
#line 1045 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),"|");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-2]));
		    free((yyvsp[0]));
		  }
#line 3178 "gparse.c" /* yacc.c:1646  */
    break;

  case 178:
#line 1055 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[0]))+3);
		    strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),"->");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-2]));
		    free((yyvsp[0]));
		  }
#line 3192 "gparse.c" /* yacc.c:1646  */
    break;

  case 180:
#line 1068 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					 strlen((char*)(yyvsp[0]))+2);
		   strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		   strcat((char*)(yyval),"&");
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-2]));
		   free((yyvsp[0]));
                 }
#line 3206 "gparse.c" /* yacc.c:1646  */
    break;

  case 182:
#line 1081 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),"~");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 3217 "gparse.c" /* yacc.c:1646  */
    break;

  case 183:
#line 1088 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					 strlen((char*)(yyvsp[-1]))+7);
		   strcpy((char*)(yyval),"BIT(");
		   strcat((char*)(yyval),(char*)(yyvsp[-3]));
		   strcat((char*)(yyval),",");
		   strcat((char*)(yyval),(char*)(yyvsp[-1]));
		   strcat((char*)(yyval),")");
		   free((yyvsp[-3]));
		   free((yyvsp[-1]));
                 }
#line 3233 "gparse.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1100 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					 strlen((char*)(yyvsp[-1]))+
					 strlen((char*)(yyvsp[0]))+1);
		   strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		   strcat((char*)(yyval),(char*)(yyvsp[-1]));
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-2]));
		   free((yyvsp[0]));
                 }
#line 3248 "gparse.c" /* yacc.c:1646  */
    break;

  case 186:
#line 1114 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),"+");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-2]));
		    free((yyvsp[0]));
		  }
#line 3262 "gparse.c" /* yacc.c:1646  */
    break;

  case 187:
#line 1124 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					  strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		    strcat((char*)(yyval),"-");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[-2]));
		    free((yyvsp[0]));
		  }
#line 3276 "gparse.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1137 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+
					 strlen((char*)(yyvsp[0]))+2);
		   strcpy((char*)(yyval),(char*)(yyvsp[-1]));
		   strcat((char*)(yyval),"*");
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-1]));
		   free((yyvsp[0]));
                 }
#line 3290 "gparse.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1147 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					 strlen((char*)(yyvsp[0]))+2);
		   strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		   strcat((char*)(yyval),"*");
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-2]));
		   free((yyvsp[0]));
                 }
#line 3304 "gparse.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1157 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					 strlen((char*)(yyvsp[0]))+2);
		   strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		   strcat((char*)(yyval),"/");
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-2]));
		   free((yyvsp[0]));
                 }
#line 3318 "gparse.c" /* yacc.c:1646  */
    break;

  case 192:
#line 1167 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					 strlen((char*)(yyvsp[0]))+2);
		   strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		   strcat((char*)(yyval),"%");
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-2]));
		   free((yyvsp[0]));
                 }
#line 3332 "gparse.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1177 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+
					 strlen((char*)(yyvsp[0]))+2);
		   strcpy((char*)(yyval),(char*)(yyvsp[-2]));
		   strcat((char*)(yyval),"^");
		   strcat((char*)(yyval),(char*)(yyvsp[0]));
		   free((yyvsp[-2]));
		   free((yyvsp[0]));
                 }
#line 3346 "gparse.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1190 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+3);
		    strcpy((char*)(yyval),"(");
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    free((yyvsp[-1]));
		  }
#line 3358 "gparse.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1198 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+7);
		    strcpy((char*)(yyval),"rate(");
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    free((yyvsp[-1]));
		  }
#line 3370 "gparse.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1206 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+6);
		    strcpy((char*)(yyval),"INT(");
		    strcat((char*)(yyval),(char*)(yyvsp[-1]));
		    strcat((char*)(yyval),")");
		    free((yyvsp[-1]));
		  }
#line 3382 "gparse.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1215 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[0]))+2);
		    strcpy((char*)(yyval),"-");
		    strcat((char*)(yyval),(char*)(yyvsp[0]));
		    free((yyvsp[0]));
		  }
#line 3393 "gparse.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1222 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock(6);
		    strcpy((char*)(yyval),"false");
                  }
#line 3402 "gparse.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1227 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (Node*)GetBlock(5);
		    strcpy((char*)(yyval),"true");
                  }
#line 3411 "gparse.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1232 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-3]))+
					 strlen((char*)(yyvsp[-1]))+3);
		   strcpy((char*)(yyval),(char*)(yyvsp[-3]));
		   strcat((char*)(yyval),"(");
		   strcat((char*)(yyval),(char*)(yyvsp[-1]));
		   strcat((char*)(yyval),")");
		   free((yyvsp[-3]));
		   free((yyvsp[-1]));
                 }
#line 3426 "gparse.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1243 "gparse.y" /* yacc.c:1646  */
    {
		   (yyval) = (Node*)GetBlock (strlen((char*)(yyvsp[-5]))+
					 strlen((char*)(yyvsp[-3]))+
					 strlen((char*)(yyvsp[-1]))+4);
		   strcpy((char*)(yyval),(char*)(yyvsp[-5]));
		   strcat((char*)(yyval),"(");
		   strcat((char*)(yyval),(char*)(yyvsp[-3]));
		   strcat((char*)(yyval),",");
		   strcat((char*)(yyval),(char*)(yyvsp[-1]));
		   strcat((char*)(yyval),")");
		   free((yyvsp[-5]));
		   free((yyvsp[-3]));
		   free((yyvsp[-1]));
                 }
#line 3445 "gparse.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1263 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (Node*)CopyString("=");  }
#line 3451 "gparse.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1264 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (Node*)CopyString("<");  }
#line 3457 "gparse.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1265 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (Node*)CopyString("<="); }
#line 3463 "gparse.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1266 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (Node*)CopyString(">");  }
#line 3469 "gparse.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1267 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (Node*)CopyString(">="); }
#line 3475 "gparse.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1298 "gparse.y" /* yacc.c:1646  */
    { 
		  (yyval) = (yyvsp[0]);
		  ((ineqADT)(yyvsp[0]))->next = (ineqADT)(yyvsp[-2]);
		}
#line 3484 "gparse.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1303 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3490 "gparse.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1307 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)GetBlock(strlen((char*)(yyvsp[-2]))+
				       strlen((char*)(yyvsp[0]))+2);
		  strcpy((char*)(yyval), (char*)(yyvsp[-2]));
		  strcat((char*)(yyval), "&");
		  strcat((char*)(yyval), (char*)(yyvsp[0]));
		  //free($1);
		  //free($3);
		}
#line 3504 "gparse.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1321 "gparse.y" /* yacc.c:1646  */
    { 
		  (yyval) = (yyvsp[0]);
		  ((ineqADT)(yyvsp[0]))->next = (ineqADT)(yyvsp[-2]);
		}
#line 3513 "gparse.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1326 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (yyvsp[0]);
		}
#line 3521 "gparse.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1332 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)Enter_Inequality((yyvsp[-2]),5,-INFIN,INFIN,(char*)(yyvsp[0]));
                }
#line 3529 "gparse.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1338 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (Node*)Enter_Inequality((yyvsp[-2]),6,-INFIN,INFIN,(char*)(yyvsp[0]));
                }
#line 3537 "gparse.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1344 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3543 "gparse.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1346 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3549 "gparse.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1348 "gparse.y" /* yacc.c:1646  */
    {
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[0]))+2);
		  strcpy((char*)(yyval),(char*)(yyvsp[0]));
                  strcat((char*)(yyval),"/");
                  free((yyvsp[0]));
   	        }
#line 3560 "gparse.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1355 "gparse.y" /* yacc.c:1646  */
    {
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[0]))+2);
		  strcpy((char*)(yyval),(char*)(yyvsp[0]));
                  strcat((char*)(yyval),"\\");
                  free((yyvsp[0]));
   	        }
#line 3571 "gparse.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1362 "gparse.y" /* yacc.c:1646  */
    { 
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[0]))+3);
                  strcpy((char*)(yyval),"-");
		  strcat((char*)(yyval),(char*)(yyvsp[0]));
                  strcat((char*)(yyval),"/");
                  free((yyvsp[0]));
                }
#line 3583 "gparse.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1370 "gparse.y" /* yacc.c:1646  */
    { 
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[0]))+3);
                  strcpy((char*)(yyval),"-");
		  strcat((char*)(yyval),(char*)(yyvsp[0]));
                  strcat((char*)(yyval),"\\");
                  free((yyvsp[0]));
                }
#line 3595 "gparse.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1378 "gparse.y" /* yacc.c:1646  */
    { 
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[0]))+3);
                  strcpy((char*)(yyval),"-");
		  strcat((char*)(yyval),(char*)(yyvsp[0]));
                  strcat((char*)(yyval),"/");
                  free((yyvsp[0]));
                }
#line 3607 "gparse.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1386 "gparse.y" /* yacc.c:1646  */
    { 
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[0]))+3);
                  strcpy((char*)(yyval),"-");
		  strcat((char*)(yyval),(char*)(yyvsp[0]));
                  strcat((char*)(yyval),"\\");
                  free((yyvsp[0]));
                }
#line 3619 "gparse.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1394 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3625 "gparse.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1396 "gparse.y" /* yacc.c:1646  */
    { 
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+3);
                  strcpy((char*)(yyval),"-");
		  strcat((char*)(yyval),(char*)(yyvsp[-1]));
                  strcat((char*)(yyval),"\\");
                  free((yyvsp[-1]));
                }
#line 3637 "gparse.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1404 "gparse.y" /* yacc.c:1646  */
    { 
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[-2]))+3);
                  strcpy((char*)(yyval),"-");
		  strcat((char*)(yyval),(char*)(yyvsp[-1]));
                  strcat((char*)(yyval),"/");
                  free((yyvsp[-1]));
                }
#line 3649 "gparse.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1412 "gparse.y" /* yacc.c:1646  */
    {
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+2);
		  strcpy((char*)(yyval),(char*)(yyvsp[-1]));
                  strcat((char*)(yyval),"\\");
                  free((yyvsp[-1]));
   	        }
#line 3660 "gparse.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1419 "gparse.y" /* yacc.c:1646  */
    {
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+2);
		  strcpy((char*)(yyval),(char*)(yyvsp[-1]));
                  strcat((char*)(yyval),"/");
                  free((yyvsp[-1]));
   	        }
#line 3671 "gparse.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1426 "gparse.y" /* yacc.c:1646  */
    {
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+2);
		  strcpy((char*)(yyval),(char*)(yyvsp[-1]));
                  strcat((char*)(yyval),"\\");
                  free((yyvsp[-1]));
   	        }
#line 3682 "gparse.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1433 "gparse.y" /* yacc.c:1646  */
    {
                  (yyval) =  (Node*)GetBlock (strlen((char*)(yyvsp[-1]))+2);
		  strcpy((char*)(yyval),(char*)(yyvsp[-1]));
                  strcat((char*)(yyval),"/");
                  free((yyvsp[-1]));
   	        }
#line 3693 "gparse.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1440 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3699 "gparse.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1441 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3705 "gparse.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1443 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3711 "gparse.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1444 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3717 "gparse.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1445 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3723 "gparse.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1447 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3729 "gparse.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1448 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3735 "gparse.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1450 "gparse.y" /* yacc.c:1646  */
    { (yyval) = NULL; }
#line 3741 "gparse.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1451 "gparse.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3747 "gparse.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1455 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[0]);
		  }
#line 3755 "gparse.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1459 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = (yyvsp[0]);
		  }
#line 3763 "gparse.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1466 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Trans ((char*)(yyvsp[-1]), "", '+');
		    free((yyvsp[-1]));
		  }
#line 3772 "gparse.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1471 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Trans ((char*)(yyvsp[-3]), (char*)(yyvsp[0]), '+');
		    free((yyvsp[-3]));
		    free((yyvsp[0]));
		  }
#line 3782 "gparse.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1477 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Trans ((char*)(yyvsp[-1]), "", '-');
		    free((yyvsp[-1]));
		  }
#line 3791 "gparse.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1482 "gparse.y" /* yacc.c:1646  */
    {
		    (yyval) = Enter_Trans ((char*)(yyvsp[-3]), (char*)(yyvsp[0]), '-');
		    free((yyvsp[-3]));
		    free((yyvsp[0]));
		  }
#line 3801 "gparse.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1497 "gparse.y" /* yacc.c:1646  */
    {
		  string hsfs = (char*)(yyvsp[-2]);
		  ineqADT ineqs = NULL;
		  ineqADT temp;
		  string hsf;
		  while (hsfs.find("&") != string::npos ||
			 hsfs.find("|") != string::npos) {
		    if (hsfs.find("&") < hsfs.find("|")) {
		      hsf = hsfs.substr(0,hsfs.find("&"));
		      hsfs = hsfs.substr(hsfs.find("&")+1,string::npos);
		    } else {
		      hsf = hsfs.substr(0,hsfs.find("|"));
		      hsfs = hsfs.substr(hsfs.find("|")+1,string::npos);
		    }

		    temp = parsehsf(hsf);
		    if (temp) {
		      if (!ineqs)
			ineqs = temp;
		      else {
			for (ineqADT step = ineqs; step;
			     step = step->next) {
			  if (!step->next) {
			    step->next = temp;
			    break;
			  }
			}
		      }
		    }
		  }
		  hsf = hsfs;
		  temp = parsehsf(hsf);
		  if (temp) {
		    if (!ineqs)
		      ineqs = temp;
		    else {
		      for (ineqADT step = ineqs; step;
			   step = step->next) {
			if (!step->next) {
			  step->next = temp;
			  break;
			}
		      }
		    }
		  }
		  if (!(yyvsp[-5])->inequalities) {
		    (yyvsp[-5])->inequalities = ineqs;
		  }
		  else {
		    for (ineqADT step = (yyvsp[-5])->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = temp;
			break;
		      }
		    }
		  }
		  Enter_Hsl((yyvsp[-5]),(char*)(yyvsp[-2]));
		  free((yyvsp[-2]));
		}
#line 3866 "gparse.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1567 "gparse.y" /* yacc.c:1646  */
    {
		  if (!(yyvsp[-5])->inequalities) {
		    (yyvsp[-5])->inequalities = (ineqADT)(yyvsp[-2]);
		  }
		  else {
		    for (ineqADT step = (yyvsp[-5])->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = (ineqADT)(yyvsp[-2]);
			break;
		      }
		    }
		  }
		}
#line 3885 "gparse.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1592 "gparse.y" /* yacc.c:1646  */
    {
		  if (!(yyvsp[-5])->inequalities) {
		    (yyvsp[-5])->inequalities = (ineqADT)(yyvsp[-2]);
		  }
		  else {
		    for (ineqADT step = (yyvsp[-5])->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = (ineqADT)(yyvsp[-2]);
			break;
		      }
		    }
		  }
                }
#line 3904 "gparse.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1615 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_Delay((yyvsp[-8]),atoi((char*)(yyvsp[-4])),atoi((char*)(yyvsp[-2])));
		 }
#line 3912 "gparse.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1619 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_Delay((yyvsp[-8]),atoi((char*)(yyvsp[-4])),INFIN);
		 }
#line 3920 "gparse.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1623 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_TransRate((yyvsp[-6]),(char*)(yyvsp[-2]));
		   free((yyvsp[-3]));
		 }
#line 3929 "gparse.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1628 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_Delay((yyvsp[-7]),atoi((char*)(yyvsp[-4])),atoi((char*)(yyvsp[-2])));
		 }
#line 3937 "gparse.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1632 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_Delay((yyvsp[-7]),atoi((char*)(yyvsp[-4])),INFIN);
		 }
#line 3945 "gparse.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1636 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_Delay((yyvsp[-3]),atoi((char*)(yyvsp[-1])),atoi((char*)(yyvsp[-1])));
		 }
#line 3953 "gparse.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1640 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_DelayExpr((yyvsp[-5]),(char*)(yyvsp[-2]));
		 }
#line 3961 "gparse.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1653 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_PriorityExpr((yyvsp[-5]),(char*)(yyvsp[-2]));
		 }
#line 3969 "gparse.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1666 "gparse.y" /* yacc.c:1646  */
    {
		   Enter_TransRate((yyvsp[-5]),(char*)(yyvsp[-2]));
		   free((yyvsp[-2]));
		 }
#line 3978 "gparse.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1680 "gparse.y" /* yacc.c:1646  */
    {
		  if (!(yyvsp[-5])->inequalities) {
		    (yyvsp[-5])->inequalities = (ineqADT)(yyvsp[-2]);
		  }
		  else {
		    for (ineqADT step = (yyvsp[-5])->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = (ineqADT)(yyvsp[-2]);
			break;
		      }
		    }
		  }
                }
#line 3997 "gparse.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1696 "gparse.y" /* yacc.c:1646  */
    { 
		  (yyval) = (yyvsp[0]);
		  ((ineqADT)(yyvsp[0]))->next = (ineqADT)(yyvsp[-2]);
		}
#line 4006 "gparse.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1701 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = (yyvsp[0]);
		}
#line 4014 "gparse.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1707 "gparse.y" /* yacc.c:1646  */
    {
		  Name_Ptr nptr;
		  nptr = Name_Find_Str((char*)(yyvsp[-2]));
		  
		  if ( nptr == NULL ) {
		    printf ("Boolean %s doesn't exist in list.\n",
			    (char*)(yyvsp[-2]));
		    gerror("ERROR");
		  }
		  else {
		    (yyval) = (Node*)Enter_Inequality(Name_Node(nptr),7,-INFIN,INFIN,(char*)(yyvsp[0]));
		  }
                }
#line 4032 "gparse.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1728 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = Enter_Trans ((char*)(yyvsp[0]), "", '$');
		  Node_Ptr n = Enter_Place (strcat((char*)(yyvsp[0]),"_p"));
		  Enter_Pred (n, (yyval));
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL, "C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
#line 4045 "gparse.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1737 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = Enter_Trans ((char*)(yyvsp[0]), "", '$');
		  Node_Ptr n = Enter_Place (strcat((char*)(yyvsp[0]),"_p"));
		  Enter_Pred (n, (yyval));
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL, "C", false, 1.0, 1);
		}
#line 4057 "gparse.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1745 "gparse.y" /* yacc.c:1646  */
    {
		  Node_Ptr n = Enter_Place (strcat((char*)(yyvsp[0]),"_p"));
		  Enter_Pred (n, (yyval));
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
#line 4069 "gparse.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1753 "gparse.y" /* yacc.c:1646  */
    {
		  Node_Ptr n = Enter_Place (strcat((char*)(yyvsp[0]),"_p"));
		  Enter_Pred (n, (yyval));
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		}
#line 4080 "gparse.c" /* yacc.c:1646  */
    break;

  case 317:
#line 1766 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = Enter_Trans ((char*)(yyvsp[0]), "", '$');
		  Node_Ptr n = Enter_Trans ("dummy", "", '$');
		  Enter_Pred ((yyval), n);
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		}
#line 4092 "gparse.c" /* yacc.c:1646  */
    break;

  case 318:
#line 1774 "gparse.y" /* yacc.c:1646  */
    {
		  (yyval) = Enter_Trans ((char*)(yyvsp[0]), "", '$');
		  Node_Ptr n = Enter_Place (strcat((char*)(yyvsp[0]),"_p"));
		  Enter_Pred (n, (yyval));
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
#line 4105 "gparse.c" /* yacc.c:1646  */
    break;

  case 319:
#line 1783 "gparse.y" /* yacc.c:1646  */
    {
		  Node_Ptr n = Enter_Trans ("dummy", "", '$');
		  Enter_Pred ((yyval), n);
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		}
#line 4116 "gparse.c" /* yacc.c:1646  */
    break;

  case 320:
#line 1790 "gparse.y" /* yacc.c:1646  */
    {
		  Node_Ptr n = Enter_Place (strcat((char*)(yyvsp[0]),"_p"));
		  Enter_Pred (n, (yyval));
		  Enter_Succ (n, (yyval), 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
#line 4128 "gparse.c" /* yacc.c:1646  */
    break;


#line 4132 "gparse.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 1798 "gparse.y" /* yacc.c:1906  */

		
//Simple int to string converstion helper function
string numToString(int num) {
  char cnumber[50];
  sprintf(cnumber, "%i", (int)num);
  string toReturn = cnumber;
  return toReturn;
}

// remove extraneous elements from lHS of inequality.  
// only really matters when result is single continuous variable.
string lhs_strip(string hsf){
  //cout <<"LHS Strip of " <<hsf;
  while (hsf.find("~") != string::npos) {
    hsf.replace(hsf.find("~"),1,"");
  }
  while (hsf.find("(") != string::npos) {
    hsf.replace(hsf.find("("),1,"");
  }
  while (hsf.find(")") != string::npos) {
    hsf.replace(hsf.find(")"),1,"");
  }
  while (hsf.find(" ") != string::npos) {
    hsf.replace(hsf.find(" "),1,"");
  }
  //  cout <<"="<<hsf<<endl;
  return hsf;
}

// remove spaces and trailing elements from inequality RHS
string rhs_strip(string hsf){
  unsigned int i;
  int count = 0,unmatch = 0;
  //cout <<"RHS Strip of " <<hsf;
  // strip spaces, I think flex already does this...
  while (hsf.find(" ") != string::npos) {
    hsf.replace(hsf.find(" "),1,"");
  }
  //search for parens to remove trailing elements. 
  //remove everything after the first unmatched ')'
  for(i = 0;i<hsf.length();i++){
    if (hsf.c_str()[i] == '(')
      count++;
    if (hsf.c_str()[i] == ')')
      count--;
    if (count == -1){
      unmatch = 1;
      break;
    }
  }				
  if (unmatch){
    //cout << "unmatched parenthesis, i = \n"<<i<< ;
    hsf = hsf.substr(0,i);
  }
  //cout <<"="<<hsf<<endl;
  return hsf;
}

ineqADT parsehsf(string hsf) {

  string place,num;
  int type;
  
 
  //cout << hsf<<endl;
  if (hsf.find(">=") != string::npos) {
    place = lhs_strip(hsf.substr(0,hsf.find(">=")));
    num = rhs_strip(hsf.substr(hsf.find(">=")+2,string::npos));
    type = 1;
    //enter inequality type 1
  }
  else if (hsf.find("<=") != string::npos) {
    place =  lhs_strip(hsf.substr(0,hsf.find("<=")));
    num =  rhs_strip(hsf.substr(hsf.find("<=")+2,string::npos));
    type = 3;
    //enter inequality type 3
  }
  else if (hsf.find(">") != string::npos) {
    place =  lhs_strip(hsf.substr(0,hsf.find(">")));
    num =  rhs_strip(hsf.substr(hsf.find(">")+1,string::npos));
    type = 0;
    //enter inequality type 0
  }
  else if (hsf.find("<") != string::npos) {
    place =  lhs_strip(hsf.substr(0,hsf.find("<")));
    num =  rhs_strip(hsf.substr(hsf.find("<")+1,string::npos));
    type = 2;
    //enter inequality type 2
  }
  else if (hsf.find("=") != string::npos) {
    place = lhs_strip( hsf.substr(0,hsf.find("=")));
    num =  rhs_strip(hsf.substr(hsf.find("=")+1,string::npos));
    type = 4;
    //enter inequality type 4
  }
  else {
    return NULL;
  }
  char *placestr = CopyString(place.c_str());
  int str_num = atoi(num.c_str());
  char *num_str = new char[255];
  sprintf(num_str,"%d",str_num);
  //  if (!strcmp(num.c_str(),num_str))
  Node *LHS = Enter_Place(placestr,0);
  if (LHS != NULL)
    return Enter_Inequality(LHS,type,
			     atoi(num.c_str()),
			     atoi(num.c_str()),
			    CopyString(num.c_str()));
  else
    return NULL;
}

int yyerror (char *s)
{
    printf ("%s at Line %d\n",s,line_num);
    fprintf (lg,"%s at Line %d\n",s,line_num);
    YY_FLUSH_BUFFER;
    completed=false;
    return 1;
}



