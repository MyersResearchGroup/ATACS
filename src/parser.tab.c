/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

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
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 6 "parser.y"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <stack>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>
#include "classes.h"
#include "events.h"
#include "hse.hpp"
#include "makedecl.h"


#define EXPANDED_RATE_NETS 2
    
    char token_buffer_1[512], token_buffer_2[512], token_buffer_3[512] ;

    char distrib1[128];
    char distrib2[128];

    int yylex (void*);

    int yyerror(char *s);

    int yywarning(char *s);

    void constructor();

    void destructor();

    //#define YYDEBUG 1

    extern FILE *lg;  // Log file from atacs.c

    // The following externs are defined in compile.c /////////////////////////
    extern SymTab*     table;
    extern tels_table* tel_tb;
    extern vhd_tel_tb  *open_entity_lst;
    extern csp_tel_tb  *open_module_list;
    extern string      cur_dir;
    extern string      cur_file;
    extern int	       mingatedelay, maxgatedelay;
    extern char*       file_scope1;
    extern int	       linenumber;
    extern char*       outpath;
    extern char*       outputname;
    extern string      cur_entity;
    extern bool	       toTEL;
    extern bool	       compiled;
    extern long        PSC;        // Program state counter
    extern bool        pre_compile;
    extern map<string,string> *name_mapping;

    // End of externs from compile.c //////////////////////////////////////////

    /*************************************************************************/
    //extern char     g_lline[];      /* from lexer */
    //extern char     yytext[];
    //extern int      column;
    /*************************************************************************/

    //This function is called when a file is needed to compile in the
    //current compiling file. It pushes the old buffer into the stack
    //and allocates new buffer for the new file and then set the file
    //pointer to the new file.  From parser.l.  extern bool
    //switch_buffer(const char *dir, const char *file_name);

    //This function cleans the buffer of the lexer. From parser.l.
    extern void clean_buffer();

    map < string, map < string, int > > signals;
    map < string, int                 > processChannels;
    string reqSuffix=SEND_SUFFIX ; // Setting up defaults for send
    string ackSuffix=RECEIVE_SUFFIX ; // Setting up defaults for send
    actionADT join=NULL; // Branches of parallel sends/receives will join here.
    int  direction=SENT;

    stack < pair < string, tels_table* > > parsing_stack;

    //Indicates whether the process statement contains a sensitivity list
    int __sensitivity_list__= 0;
    int isENTITY= 0;
    int isTOP= 0;


    // Create a dummy action.
    actionADT dummyE();

    // Create a new dummy action with name $name
    actionADT make_dummy(char *name);

    // Create a timed event-rule structure for an guard.
    TERstructADT Guard(const string & expression);

    // Create a timed event-rule structure for a CV assignment
    TERstructADT Assign(const string & expression);
    
    // Create a timed event-rule structure for an guarded action.
    TERstructADT Guard(const string & expression, actionADT target,
		       const string& data);

    TERstructADT FourPhase(const string & channel, const string & data="");

    TERstructADT signalAssignment(TYPES* target, TYPES* waveform);

    TYPES * Probe(const string & channel);

    void declare(list<pair<string,int> >* ports);

    //Converts an double to a string
    string numToString(double num);
    string intToString(int num);



    
/* Line 371 of yacc.c  */
#line 192 "parser.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
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
   by #include "parser.tab.h".  */
#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ABSOLUTE = 258,
     ACCESS = 259,
     AFTER = 260,
     ALIAS = 261,
     ALL = 262,
     AND = 263,
     ARCHITECTURE = 264,
     ARRAY = 265,
     ASSERT = 266,
     ATTRIBUTE = 267,
     VBEGIN = 268,
     BLOCK = 269,
     BODY = 270,
     BUFFER = 271,
     BUS = 272,
     CASE = 273,
     COMPONENT = 274,
     CONFIGURATION = 275,
     CONSTANT = 276,
     DISCONNECT = 277,
     DOWNTO = 278,
     ELSE = 279,
     ELSIF = 280,
     END = 281,
     ENTITY = 282,
     EXIT = 283,
     VFILE = 284,
     FOR = 285,
     FUNCTION = 286,
     PROCEDURE = 287,
     NATURAL = 288,
     GENERATE = 289,
     GENERIC = 290,
     GUARDED = 291,
     IF = 292,
     IMPURE = 293,
     VIN = 294,
     INITIALIZE = 295,
     INOUT = 296,
     IS = 297,
     LABEL = 298,
     LIBRARY = 299,
     LINKAGE = 300,
     LITERAL = 301,
     LOOP = 302,
     MAP = 303,
     MOD = 304,
     NAND = 305,
     NEW = 306,
     NEXT = 307,
     NOR = 308,
     NOT = 309,
     UNAFFECTED = 310,
     UNITS = 311,
     GROUP = 312,
     OF = 313,
     ON = 314,
     OPEN = 315,
     OR = 316,
     OTHERS = 317,
     VOUT = 318,
     VREJECT = 319,
     INERTIAL = 320,
     XNOR = 321,
     PACKAGE = 322,
     PORT = 323,
     POSTPONED = 324,
     PROCESS = 325,
     PURE = 326,
     RANGE = 327,
     RECORD = 328,
     REGISTER = 329,
     REM = 330,
     REPORT = 331,
     RETURN = 332,
     SELECT = 333,
     SEVERITY = 334,
     SHARED = 335,
     SIGNAL = 336,
     SUBTYPE = 337,
     THEN = 338,
     TO = 339,
     TRANSPORT = 340,
     TYPE = 341,
     UNTIL = 342,
     USE = 343,
     VARIABLE = 344,
     VNULL = 345,
     WAIT = 346,
     WHEN = 347,
     WHILE = 348,
     WITH = 349,
     XOR = 350,
     ARROW = 351,
     EXPON = 352,
     ASSIGN = 353,
     BOX = 354,
     SLL = 355,
     SRL = 356,
     SLA = 357,
     SRA = 358,
     ROL = 359,
     ROR = 360,
     AT = 361,
     INF = 362,
     EOC = 363,
     ASSIGNMENT = 364,
     PROBE = 365,
     RECEIVE = 366,
     SEND = 367,
     VASSIGNMENT = 368,
     INITCHANNEL = 369,
     ACROSS = 370,
     BREAK = 371,
     EQ = 372,
     LIMIT = 373,
     NATURE = 374,
     NOISE = 375,
     PROCEDURAL = 376,
     QUANTITY = 377,
     REFERENCE = 378,
     SPECTRUM = 379,
     SUBNATURE = 380,
     TERMINAL = 381,
     THROUGH = 382,
     TOLER = 383,
     VDOT = 384,
     ABOVE = 385,
     MODULE = 386,
     ENDMODULE = 387,
     BOOL = 388,
     PARA = 389,
     ATACS_DELAY = 390,
     ENDPROC = 391,
     SKIP = 392,
     INIT = 393,
     DEL = 394,
     BOOLEAN = 395,
     SLASH = 396,
     CID = 397,
     CPORT = 398,
     CINT = 399,
     CARROW = 400,
     PROC = 401,
     RECURSIVE_ERROR = 402,
     GATE = 403,
     ENDGATE = 404,
     GUARD = 405,
     GUARDAND = 406,
     GUARDOR = 407,
     GUARDTO = 408,
     LARROW = 409,
     INCLUDE = 410,
     ENDTB = 411,
     TESTBENCH = 412,
     AWAIT = 413,
     AWAITALL = 414,
     AWAITANY = 415,
     ATACS_DEFINE = 416,
     CONSTRAINT = 417,
     ENDCONSTRAINT = 418,
     ASSUMP = 419,
     ENDASSUMP = 420,
     SUM = 421,
     DIFF = 422,
     STRLIT = 423,
     BITLIT = 424,
     INT = 425,
     VBOOL = 426,
     BASEDLIT = 427,
     REAL = 428,
     VID = 429,
     ACTION = 430,
     CHARLIT = 431,
     ID = 432,
     NEQ = 433,
     LE = 434,
     GE = 435,
     MED_PRECEDENCE = 436,
     MAX_PRECEDENCE = 437
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 133 "parser.y"

  int  				     intval;
  double			     floatval;
  char				     *C_str;
  char				     *id;
  long int			     basedlit;
  char				     *charlit;
  char				     *strlit;
  char				     *bitlit;
  char            		     *stringval;
  bool            		     boolval;
  string                             *str;
  TYPES				     *types;
  actionADT       		     actionptr;
  TERstructADT    		     TERSptr;
  delayADT        		     delayval;
  TYPELIST        		     *typelist;
  pair<string, string>               *csp_pair;
  pair<TYPES,telADT>                 *ty_tel2;
  map<string, string>                *csp_map;
  list<actionADT>                    *action_list;
  list<string>                       *str_list;
  list<TYPES>                        *ty_lst;
  list<pair<string,string> >         *str_str_l;
  list<pair<string,int> >            *str_bl_lst;
  list<pair<TYPES,telADT> >          *ty_tel_lst;
  list<pair<TYPES,TYPES> >           *ty_ty_lst;
  pair<string,list<string> >         *stmt_pair;
  list<pair<string,list<string> > >  *simul_lst;


/* Line 387 of yacc.c  */
#line 449 "parser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


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

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 476 "parser.tab.c"

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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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

# define YYCOPY_NEEDED 1

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
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3515

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  205
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  391
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1017
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1946

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   437

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   186,   199,
     195,   196,   191,   184,   194,   185,   200,   192,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   189,   188,
     179,   180,   178,     2,   204,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   197,     2,   198,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   202,   187,   203,   201,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   181,   182,   183,   190,   193
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    16,    19,
      20,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    44,    47,    51,    52,    57,    64,    73,    74,    76,
      78,    81,    82,    84,    86,    89,    95,   101,   102,   105,
     107,   109,   111,   113,   115,   117,   119,   121,   123,   125,
     127,   129,   131,   133,   135,   137,   139,   141,   143,   145,
     147,   149,   157,   162,   165,   166,   168,   177,   181,   185,
     187,   189,   190,   193,   195,   197,   199,   200,   213,   214,
     217,   219,   221,   222,   225,   226,   229,   232,   238,   245,
     246,   248,   250,   253,   255,   257,   259,   260,   263,   271,
     273,   278,   280,   282,   284,   285,   288,   289,   292,   294,
     296,   302,   310,   317,   326,   327,   332,   338,   345,   346,
     348,   349,   352,   354,   356,   358,   360,   362,   364,   366,
     368,   370,   372,   374,   376,   378,   380,   382,   384,   386,
     388,   390,   394,   401,   409,   410,   413,   414,   417,   419,
     421,   423,   425,   427,   429,   431,   433,   435,   437,   439,
     442,   445,   451,   457,   466,   467,   469,   471,   474,   482,
     491,   492,   494,   496,   497,   500,   502,   504,   505,   508,
     510,   512,   514,   516,   518,   520,   522,   524,   526,   528,
     530,   532,   534,   536,   538,   540,   546,   550,   552,   554,
     556,   558,   560,   562,   564,   566,   568,   574,   576,   578,
     580,   586,   594,   596,   600,   609,   610,   612,   614,   615,
     618,   621,   628,   637,   638,   640,   646,   653,   657,   662,
     667,   673,   680,   685,   688,   689,   694,   696,   700,   705,
     708,   714,   715,   723,   724,   727,   731,   736,   738,   746,
     748,   750,   752,   753,   756,   758,   759,   760,   762,   767,
     771,   773,   777,   778,   781,   782,   784,   785,   786,   794,
     796,   799,   802,   806,   807,   809,   813,   818,   824,   825,
     834,   838,   840,   842,   844,   847,   852,   854,   856,   858,
     860,   862,   864,   866,   868,   870,   872,   874,   876,   878,
     880,   882,   884,   886,   888,   890,   892,   894,   896,   898,
     900,   905,   909,   911,   913,   915,   917,   921,   922,   925,
     929,   931,   934,   937,   939,   945,   951,   953,   955,   958,
     964,   968,   970,   972,   974,   976,   980,   986,   990,   996,
    1000,  1001,  1006,  1008,  1012,  1014,  1016,  1017,  1021,  1025,
    1027,  1031,  1033,  1035,  1037,  1040,  1042,  1046,  1048,  1050,
    1057,  1065,  1066,  1069,  1072,  1077,  1079,  1081,  1088,  1093,
    1095,  1098,  1102,  1106,  1108,  1112,  1117,  1123,  1125,  1128,
    1133,  1136,  1140,  1145,  1149,  1155,  1158,  1162,  1166,  1171,
    1174,  1178,  1180,  1183,  1185,  1188,  1192,  1195,  1199,  1201,
    1203,  1207,  1209,  1216,  1223,  1228,  1234,  1239,  1245,  1251,
    1253,  1256,  1258,  1265,  1273,  1275,  1277,  1279,  1281,  1283,
    1285,  1287,  1289,  1297,  1306,  1310,  1315,  1319,  1322,  1323,
    1325,  1326,  1328,  1332,  1334,  1335,  1340,  1343,  1348,  1353,
    1360,  1361,  1364,  1365,  1368,  1371,  1373,  1375,  1377,  1379,
    1381,  1383,  1385,  1387,  1389,  1391,  1393,  1395,  1397,  1399,
    1401,  1403,  1405,  1407,  1409,  1411,  1413,  1415,  1417,  1419,
    1420,  1430,  1433,  1437,  1439,  1442,  1445,  1449,  1450,  1452,
    1454,  1459,  1461,  1465,  1466,  1469,  1471,  1473,  1475,  1477,
    1479,  1481,  1483,  1485,  1487,  1489,  1491,  1493,  1495,  1497,
    1498,  1501,  1503,  1506,  1509,  1513,  1518,  1523,  1525,  1528,
    1531,  1534,  1536,  1539,  1542,  1546,  1548,  1551,  1553,  1556,
    1559,  1563,  1566,  1570,  1576,  1578,  1580,  1581,  1583,  1585,
    1588,  1590,  1592,  1596,  1599,  1604,  1605,  1611,  1620,  1624,
    1630,  1639,  1649,  1652,  1653,  1655,  1658,  1659,  1661,  1663,
    1666,  1669,  1672,  1675,  1679,  1686,  1692,  1694,  1696,  1698,
    1700,  1702,  1704,  1705,  1708,  1715,  1721,  1729,  1736,  1748,
    1758,  1759,  1765,  1766,  1769,  1779,  1788,  1793,  1799,  1800,
    1809,  1812,  1816,  1818,  1821,  1822,  1825,  1827,  1829,  1831,
    1833,  1835,  1837,  1839,  1841,  1843,  1845,  1847,  1849,  1851,
    1853,  1855,  1857,  1859,  1861,  1863,  1865,  1867,  1869,  1871,
    1873,  1875,  1877,  1879,  1881,  1883,  1885,  1887,  1889,  1891,
    1893,  1895,  1897,  1899,  1905,  1911,  1917,  1923,  1929,  1935,
    1941,  1947,  1949,  1953,  1955,  1959,  1961,  1963,  1967,  1969,
    1973,  1977,  1983,  1987,  1992,  1997,  2002,  2008,  2014,  2021,
    2027,  2030,  2033,  2036,  2041,  2042,  2045,  2048,  2053,  2055,
    2057,  2063,  2069,  2071,  2075,  2079,  2085,  2087,  2091,  2099,
    2106,  2112,  2114,  2118,  2126,  2132,  2137,  2144,  2150,  2152,
    2156,  2158,  2160,  2164,  2170,  2175,  2178,  2189,  2201,  2202,
    2208,  2209,  2212,  2213,  2215,  2218,  2219,  2222,  2231,  2241,
    2246,  2252,  2260,  2269,  2270,  2273,  2278,  2282,  2287,  2293,
    2294,  2302,  2306,  2311,  2317,  2318,  2326,  2330,  2335,  2338,
    2342,  2347,  2354,  2360,  2365,  2366,  2369,  2371,  2375,  2382,
    2386,  2388,  2392,  2394,  2396,  2398,  2400,  2402,  2404,  2406,
    2411,  2418,  2420,  2422,  2423,  2429,  2436,  2443,  2451,  2457,
    2464,  2465,  2467,  2468,  2471,  2477,  2484,  2489,  2491,  2495,
    2497,  2499,  2501,  2503,  2505,  2509,  2511,  2515,  2517,  2519,
    2523,  2525,  2527,  2531,  2533,  2537,  2539,  2541,  2543,  2546,
    2549,  2552,  2556,  2560,  2564,  2566,  2570,  2573,  2577,  2580,
    2584,  2587,  2589,  2594,  2598,  2600,  2602,  2604,  2606,  2608,
    2610,  2612,  2616,  2618,  2620,  2622,  2624,  2626,  2628,  2630,
    2633,  2635,  2637,  2639,  2643,  2645,  2647,  2649,  2653,  2655,
    2657,  2659,  2661,  2663,  2665,  2669,  2672,  2675,  2677,  2679,
    2681,  2683,  2685,  2689,  2693,  2698,  2701,  2702,  2704,  2710,
    2714,  2716,  2723,  2725,  2727,  2729,  2733,  2735,  2737,  2739,
    2741,  2743,  2745,  2747,  2749,  2753,  2757,  2762,  2764,  2769,
    2771,  2776,  2778,  2782,  2783,  2787,  2790,  2793,  2795,  2797,
    2799,  2801,  2803,  2805,  2807,  2809,  2812,  2815,  2818,  2821,
    2824,  2827,  2833,  2839,  2841,  2845,  2847,  2851,  2855,  2857,
    2859,  2861,  2863,  2865,  2868,  2870,  2871,  2879,  2883,  2887,
    2889,  2893,  2895,  2897,  2898,  2900,  2902,  2905,  2906,  2908,
    2910,  2912,  2918,  2924,  2931,  2935,  2943,  2951,  2961,  2963,
    2967,  2977,  3001,  3018,  3035,  3041,  3054,  3056,  3059,  3062,
    3064,  3066,  3073,  3077,  3079,  3083,  3089,  3096,  3102,  3109,
    3115,  3121,  3127,  3129,  3131,  3138,  3145,  3149,  3151,  3156,
    3160,  3169,  3173,  3178,  3182,  3184,  3188,  3190,  3192,  3194,
    3201,  3203,  3205,  3209,  3213,  3217,  3221,  3225,  3227,  3229,
    3233,  3237,  3242,  3246,  3248,  3252,  3258,  3265,  3274,  3279,
    3286,  3294,  3304,  3306,  3308,  3310,  3317,  3324,  3328,  3332,
    3336,  3345,  3354,  3358,  3362,  3366,  3375,  3384,  3386,  3388,
    3395,  3399,  3401,  3405,  3407,  3411,  3413,  3415,  3418,  3422,
    3427,  3429,  3433,  3435,  3437,  3441,  3444,  3448
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     206,     0,    -1,    -1,   207,   208,    -1,   557,    -1,   556,
      -1,   209,    -1,   208,   209,    -1,   210,   212,    -1,    -1,
     210,   211,    -1,   215,    -1,   335,    -1,   213,    -1,   214,
      -1,   217,    -1,   238,    -1,   250,    -1,   233,    -1,   257,
      -1,   216,   188,    -1,    44,   174,    -1,   216,   194,   174,
      -1,    -1,    27,   174,   218,   219,    -1,    42,   221,   224,
      26,   220,   188,    -1,    42,   221,   224,    13,   231,    26,
     220,   188,    -1,    -1,    27,    -1,   174,    -1,    27,   174,
      -1,    -1,   222,    -1,   223,    -1,   222,   223,    -1,    35,
     195,   497,   196,   188,    -1,    68,   195,   497,   196,   188,
      -1,    -1,   224,   225,    -1,   261,    -1,   265,    -1,   271,
      -1,   368,    -1,   279,    -1,   281,    -1,   293,    -1,   298,
      -1,   313,    -1,   338,    -1,   314,    -1,   336,    -1,   330,
      -1,   335,    -1,   284,    -1,   226,    -1,   229,    -1,   334,
      -1,   277,    -1,   379,    -1,   287,    -1,   286,    -1,    57,
     174,    42,   195,   227,   196,   188,    -1,   227,   194,   320,
     228,    -1,   320,   228,    -1,    -1,    99,    -1,    57,   174,
     189,   174,   195,   230,   196,   188,    -1,   230,   194,   174,
      -1,   230,   194,   176,    -1,   174,    -1,   176,    -1,    -1,
     231,   232,    -1,   406,    -1,   403,    -1,   393,    -1,    -1,
       9,   174,    58,   174,    42,   234,   236,    13,   237,    26,
     235,   188,    -1,    -1,     9,   174,    -1,     9,    -1,   174,
      -1,    -1,   236,   392,    -1,    -1,   237,   382,    -1,   237,
     424,    -1,    20,   174,    58,   174,   239,    -1,    42,   242,
     243,    26,   240,   188,    -1,    -1,    20,    -1,   174,    -1,
      20,   174,    -1,   335,    -1,   314,    -1,   229,    -1,    -1,
     242,   241,    -1,    30,   244,   246,   247,    26,    30,   188,
      -1,   174,    -1,   174,   195,   245,   196,    -1,   373,    -1,
     170,    -1,   173,    -1,    -1,   246,   335,    -1,    -1,   247,
     248,    -1,   243,    -1,   249,    -1,    30,   322,    26,    30,
     188,    -1,    30,   322,   325,   188,    26,    30,   188,    -1,
      30,   322,   243,    26,    30,   188,    -1,    30,   322,   325,
     188,   243,    26,    30,   188,    -1,    -1,    67,   174,   251,
     252,    -1,    42,   254,    26,   253,   188,    -1,    42,   254,
      26,   253,   174,   188,    -1,    -1,    67,    -1,    -1,   254,
     255,    -1,   261,    -1,   271,    -1,   368,    -1,   279,    -1,
     281,    -1,   293,    -1,   298,    -1,   307,    -1,   313,    -1,
     314,    -1,   336,    -1,   330,    -1,   335,    -1,   284,    -1,
     226,    -1,   229,    -1,   277,    -1,   379,    -1,   286,    -1,
      67,    15,   174,    -1,   256,    42,   259,    26,   258,   188,
      -1,   256,    42,   259,    26,   258,   174,   188,    -1,    -1,
      67,    15,    -1,    -1,   259,   260,    -1,   261,    -1,   265,
      -1,   271,    -1,   368,    -1,   279,    -1,   293,    -1,   298,
      -1,   335,    -1,   284,    -1,   226,    -1,   229,    -1,   262,
     188,    -1,    32,   268,    -1,    32,   268,   195,   497,   196,
      -1,   263,    31,   268,    77,   538,    -1,   263,    31,   268,
     195,   497,   196,    77,   538,    -1,    -1,    71,    -1,    38,
      -1,   262,    42,    -1,   264,   269,    13,   267,    26,   266,
     188,    -1,   264,   269,    13,   267,    26,   266,   268,   188,
      -1,    -1,    32,    -1,    31,    -1,    -1,   267,   439,    -1,
     174,    -1,   168,    -1,    -1,   269,   270,    -1,   261,    -1,
     265,    -1,   271,    -1,   368,    -1,   279,    -1,   284,    -1,
     293,    -1,   298,    -1,   313,    -1,   338,    -1,   314,    -1,
     335,    -1,   226,    -1,   229,    -1,   272,    -1,   273,    -1,
      86,   174,    42,   274,   188,    -1,    86,   174,   188,    -1,
     275,    -1,   276,    -1,   365,    -1,   366,    -1,   344,    -1,
     347,    -1,   351,    -1,   355,    -1,   362,    -1,   119,   174,
      42,   278,   188,    -1,   375,    -1,   376,    -1,   377,    -1,
      21,   280,   189,   371,   188,    -1,    21,   280,   189,   371,
      98,   518,   188,    -1,   174,    -1,   280,   194,   174,    -1,
      81,   280,   189,   371,   282,   506,   188,   283,    -1,    -1,
      74,    -1,    17,    -1,    -1,   106,    39,    -1,   106,    63,
      -1,   285,    89,   280,   189,   371,   188,    -1,   285,    89,
     280,   189,   371,    98,   518,   188,    -1,    -1,    80,    -1,
     126,   280,   189,   380,   188,    -1,   122,   280,   189,   371,
     506,   188,    -1,   122,   291,   188,    -1,   122,   288,   291,
     188,    -1,   122,   290,   291,   188,    -1,   122,   288,   290,
     291,   188,    -1,   122,   280,   189,   371,   292,   188,    -1,
     280,   289,   506,   115,    -1,   128,   518,    -1,    -1,   280,
     289,   506,   127,    -1,   537,    -1,   537,    84,   537,    -1,
     124,   526,   194,   526,    -1,   120,   526,    -1,    29,   280,
     189,   371,   188,    -1,    -1,    29,   280,   189,   371,   294,
     296,   188,    -1,    -1,    60,   518,    -1,   295,    42,   297,
      -1,   295,    42,   510,   297,    -1,   537,    -1,     6,   299,
     300,    42,   537,   301,   188,    -1,   174,    -1,   176,    -1,
     168,    -1,    -1,   189,   369,    -1,   303,    -1,    -1,    -1,
     303,    -1,   197,   304,   305,   198,    -1,   197,   305,   198,
      -1,   537,    -1,   304,   194,   537,    -1,    -1,    77,   537,
      -1,    -1,   208,    -1,    -1,    -1,    19,   174,   308,   306,
     309,   310,   312,    -1,   311,    -1,   311,   222,    -1,   311,
     223,    -1,   311,   222,   223,    -1,    -1,    42,    -1,    26,
      19,   188,    -1,    26,    19,   174,   188,    -1,    12,   174,
     189,   174,   188,    -1,    -1,    12,   174,   315,    58,   316,
      42,   518,   188,    -1,   317,   189,   320,    -1,   318,    -1,
      62,    -1,     7,    -1,   319,   302,    -1,   318,   194,   319,
     302,    -1,   174,    -1,   168,    -1,   176,    -1,    27,    -1,
       9,    -1,    20,    -1,    32,    -1,    31,    -1,    67,    -1,
      86,    -1,    82,    -1,    21,    -1,    81,    -1,    89,    -1,
      19,    -1,    43,    -1,    46,    -1,    56,    -1,    57,    -1,
      29,    -1,   119,    -1,   125,    -1,   122,    -1,   126,    -1,
      30,   322,   325,   188,    -1,   323,   189,   537,    -1,   324,
      -1,    62,    -1,     7,    -1,   174,    -1,   324,   194,   174,
      -1,    -1,    88,   326,    -1,    88,   326,   329,    -1,   329,
      -1,    27,   537,    -1,    20,   537,    -1,    60,    -1,    68,
      48,   195,   514,   196,    -1,    35,    48,   195,   514,   196,
      -1,   328,    -1,   327,    -1,   328,   327,    -1,    22,   331,
       5,   518,   188,    -1,   332,   189,   174,    -1,   333,    -1,
      62,    -1,     7,    -1,   174,    -1,   332,   194,   174,    -1,
     118,   331,    94,   518,   188,    -1,    88,   539,   188,    -1,
      40,   337,    84,   518,   188,    -1,   332,   189,   538,    -1,
      -1,   174,   339,   340,   188,    -1,   341,    -1,   340,   194,
     341,    -1,   174,    -1,   342,    -1,    -1,   174,   343,   360,
      -1,   195,   345,   196,    -1,   346,    -1,   345,   194,   346,
      -1,   174,    -1,   176,    -1,   348,    -1,    72,   349,    -1,
     543,    -1,   526,   350,   526,    -1,    84,    -1,    23,    -1,
     348,    56,   353,   352,    26,    56,    -1,   348,    56,   353,
     352,    26,    56,   174,    -1,    -1,   352,   354,    -1,   174,
     188,    -1,   174,   180,   549,   188,    -1,   356,    -1,   357,
      -1,    10,   195,   358,   196,    58,   371,    -1,    10,   360,
      58,   371,    -1,   359,    -1,   358,   359,    -1,   538,    72,
      99,    -1,   195,   361,   196,    -1,   373,    -1,   361,   194,
     373,    -1,    73,   363,    26,    73,    -1,    73,   363,    26,
      73,   174,    -1,   364,    -1,   363,   364,    -1,   280,   189,
     371,   188,    -1,     4,   371,    -1,    29,    58,   537,    -1,
      29,   367,    58,   537,    -1,   195,   518,   196,    -1,    82,
     174,    42,   369,   188,    -1,   538,   289,    -1,   538,   289,
     370,    -1,   538,   511,   289,    -1,   538,   511,   289,   370,
      -1,   372,   289,    -1,   115,   518,   127,    -1,   538,    -1,
     538,   511,    -1,   372,    -1,   538,   348,    -1,   538,   538,
     348,    -1,   538,   538,    -1,   538,   538,   511,    -1,   349,
      -1,   371,    -1,   526,   350,   526,    -1,   372,    -1,   537,
     115,   537,   127,   174,   123,    -1,    10,   195,   358,   196,
      58,   380,    -1,    10,   360,    58,   380,    -1,    73,   378,
      26,    73,   174,    -1,   280,   189,   380,   188,    -1,   378,
     280,   189,   380,   188,    -1,   125,   174,    42,   380,   188,
      -1,   538,    -1,   538,   511,    -1,   381,    -1,   538,   128,
     518,   115,   518,   127,    -1,   538,   511,   128,   518,   115,
     518,   127,    -1,   383,    -1,   393,    -1,   403,    -1,   406,
      -1,   407,    -1,   404,    -1,   416,    -1,   423,    -1,   386,
     387,   388,   390,    13,   391,   384,    -1,   386,   385,   387,
     388,   390,    13,   391,   384,    -1,    26,    14,   188,    -1,
      26,    14,   174,   188,    -1,   195,   518,   196,    -1,   480,
      14,    -1,    -1,    42,    -1,    -1,   222,    -1,   222,   328,
     188,    -1,   223,    -1,    -1,   223,   389,   327,   188,    -1,
     222,   223,    -1,   222,   223,   327,   188,    -1,   222,   328,
     188,   223,    -1,   222,   328,   188,   223,   327,   188,    -1,
      -1,   390,   392,    -1,    -1,   391,   382,    -1,   391,   424,
      -1,   261,    -1,   265,    -1,   271,    -1,   368,    -1,   279,
      -1,   281,    -1,   293,    -1,   298,    -1,   307,    -1,   313,
      -1,   338,    -1,   314,    -1,   336,    -1,   321,    -1,   330,
      -1,   335,    -1,   284,    -1,   226,    -1,   229,    -1,   334,
      -1,   277,    -1,   379,    -1,   287,    -1,   286,    -1,    -1,
     396,   398,   394,   400,    13,   402,    26,   397,   395,    -1,
      70,   188,    -1,    70,   174,   188,    -1,    70,    -1,   480,
      70,    -1,    69,    70,    -1,   480,    69,    70,    -1,    -1,
      69,    -1,   387,    -1,   195,   399,   196,   387,    -1,   537,
      -1,   399,   194,   537,    -1,    -1,   400,   401,    -1,   261,
      -1,   265,    -1,   271,    -1,   368,    -1,   279,    -1,   284,
      -1,   293,    -1,   298,    -1,   313,    -1,   338,    -1,   314,
      -1,   335,    -1,   226,    -1,   229,    -1,    -1,   402,   439,
      -1,   475,    -1,   480,   475,    -1,    69,   475,    -1,   480,
      69,   475,    -1,   480,    19,   537,   188,    -1,   480,   405,
     329,   188,    -1,   537,    -1,    19,   537,    -1,    27,   537,
      -1,    20,   537,    -1,   458,    -1,   480,   458,    -1,    69,
     458,    -1,   480,    69,   458,    -1,   408,    -1,   480,   408,
      -1,   414,    -1,   480,   414,    -1,    69,   408,    -1,   480,
      69,   408,    -1,    69,   414,    -1,   480,    69,   414,    -1,
     409,   182,   410,   412,   188,    -1,   537,    -1,   551,    -1,
      -1,   411,    -1,    36,    -1,    36,   411,    -1,    85,    -1,
      65,    -1,    64,   518,    65,    -1,   413,   472,    -1,   413,
     472,    92,   518,    -1,    -1,   413,   472,    92,   518,    24,
      -1,    94,   518,    78,   409,   182,   410,   415,   188,    -1,
     472,    92,   554,    -1,   415,   194,   472,    92,   554,    -1,
     480,   417,   419,   420,    26,    34,   418,   188,    -1,   480,
     417,   419,    13,   420,    26,    34,   418,   188,    -1,   421,
      34,    -1,    -1,   174,    -1,   419,   392,    -1,    -1,   382,
      -1,   424,    -1,   420,   382,    -1,   420,   424,    -1,    30,
     422,    -1,    37,   518,    -1,   174,    39,   373,    -1,   479,
     116,   495,   456,   494,   188,    -1,   479,   116,   495,   494,
     188,    -1,   425,    -1,   427,    -1,   428,    -1,   431,    -1,
     433,    -1,   491,    -1,    -1,   426,   425,    -1,   480,   526,
     117,   526,   289,   188,    -1,   526,   117,   526,   289,   188,
      -1,   480,   538,   199,   129,   117,   526,   188,    -1,   538,
     199,   129,   117,   526,   188,    -1,   480,    37,   518,    88,
     426,   429,   430,    26,    88,   174,   188,    -1,    37,   518,
      88,   426,   429,   430,    26,    88,   188,    -1,    -1,   429,
      25,   518,    88,   426,    -1,    -1,    24,   426,    -1,   479,
      18,   518,    88,   432,    26,    18,   174,   188,    -1,   479,
      18,   518,    88,   432,    26,    18,   188,    -1,    92,   554,
      96,   426,    -1,   432,    92,   554,    96,   426,    -1,    -1,
     436,   387,   434,   437,    13,   402,    26,   435,    -1,   121,
     188,    -1,   121,   174,   188,    -1,   121,    -1,   480,   121,
      -1,    -1,   437,   438,    -1,   261,    -1,   265,    -1,   271,
      -1,   368,    -1,   279,    -1,   284,    -1,   298,    -1,   313,
      -1,   314,    -1,   335,    -1,   226,    -1,   229,    -1,   454,
      -1,   458,    -1,   471,    -1,   474,    -1,   475,    -1,   476,
      -1,   482,    -1,   484,    -1,   486,    -1,   488,    -1,   490,
      -1,   491,    -1,   492,    -1,   461,    -1,   464,    -1,   468,
      -1,   440,    -1,   493,    -1,   467,    -1,   441,    -1,   442,
      -1,   443,    -1,   444,    -1,   445,    -1,   446,    -1,   150,
     195,   452,   196,   188,    -1,   150,   195,   518,   196,   188,
      -1,   150,   195,   453,   196,   188,    -1,   152,   195,   451,
     196,   188,    -1,   151,   195,   450,   196,   188,    -1,   158,
     195,   449,   196,   188,    -1,   160,   195,   448,   196,   188,
      -1,   159,   195,   447,   196,   188,    -1,   449,    -1,   449,
     194,   447,    -1,   449,    -1,   449,   194,   448,    -1,   409,
      -1,   452,    -1,   452,   194,   450,    -1,   452,    -1,   452,
     194,   451,    -1,   518,   194,   518,    -1,   518,   194,   170,
     194,   173,    -1,   479,    91,   188,    -1,   479,    91,   456,
     188,    -1,   479,    91,   455,   188,    -1,   479,    91,   457,
     188,    -1,   479,    91,   456,   455,   188,    -1,   479,    91,
     456,   457,   188,    -1,   479,    91,   456,   455,   457,   188,
      -1,   479,    91,   455,   457,   188,    -1,    87,   518,    -1,
      59,   399,    -1,    30,   518,    -1,    11,   518,   459,   188,
      -1,    -1,    76,   518,    -1,    79,   174,    -1,    76,   518,
      79,   174,    -1,   112,    -1,   111,    -1,   460,   195,   462,
     196,   188,    -1,   460,   195,   409,   196,   188,    -1,   463,
      -1,   462,   194,   463,    -1,   409,   194,   518,    -1,   109,
     195,   465,   196,   188,    -1,   466,    -1,   465,   194,   466,
      -1,   409,   194,   518,   194,   170,   194,   170,    -1,   538,
     199,   129,    98,   526,   188,    -1,   113,   195,   469,   196,
     188,    -1,   470,    -1,   469,   194,   470,    -1,   409,   194,
     518,   194,   170,   194,   170,    -1,   409,   182,   411,   472,
     188,    -1,   409,   182,   472,   188,    -1,   480,   409,   182,
     411,   472,   188,    -1,   480,   409,   182,   472,   188,    -1,
     473,    -1,   472,   194,   473,    -1,    55,    -1,   518,    -1,
     518,     5,   518,    -1,   480,   409,    98,   518,   188,    -1,
     409,    98,   518,   188,    -1,   537,   188,    -1,   479,    37,
     518,    83,   481,   477,   478,    26,    37,   188,    -1,   479,
      37,   518,    83,   481,   477,   478,    26,    37,   174,   188,
      -1,    -1,   477,    25,   518,    83,   481,    -1,    -1,    24,
     481,    -1,    -1,   480,    -1,   174,   189,    -1,    -1,   481,
     439,    -1,   479,    18,   518,    42,   483,    26,    18,   188,
      -1,   479,    18,   518,    42,   483,    26,    18,   174,   188,
      -1,    92,   554,    96,   481,    -1,   483,    92,   554,    96,
     481,    -1,   479,   485,    47,   481,    26,    47,   188,    -1,
     479,   485,    47,   481,    26,    47,   174,   188,    -1,    -1,
      93,   518,    -1,    30,   174,    39,   373,    -1,   479,    52,
     188,    -1,   479,    52,   174,   188,    -1,   479,    52,    92,
     518,   188,    -1,    -1,   479,    52,   174,   487,    92,   518,
     188,    -1,   479,    28,   188,    -1,   479,    28,   174,   188,
      -1,   479,    28,    92,   518,   188,    -1,    -1,   479,    28,
     174,   489,    92,   518,   188,    -1,   479,    77,   188,    -1,
     479,    77,   518,   188,    -1,    90,   188,    -1,   480,    90,
     188,    -1,   479,    76,   518,   188,    -1,   479,    76,   518,
      79,   174,   188,    -1,   479,   116,   495,   494,   188,    -1,
     479,   116,   494,   188,    -1,    -1,    92,   518,    -1,   496,
      -1,   495,   194,   496,    -1,    30,   537,    88,   537,    96,
     518,    -1,   537,    96,   518,    -1,   498,    -1,   497,   188,
     498,    -1,   502,    -1,   503,    -1,   504,    -1,   508,    -1,
     507,    -1,   499,    -1,   500,    -1,   126,   280,   189,   380,
      -1,   122,   280,   189,   501,   371,   506,    -1,    39,    -1,
      63,    -1,    -1,    21,   280,   189,   371,   506,    -1,    21,
     280,   189,    39,   371,   506,    -1,    81,   280,   189,   371,
     505,   506,    -1,    81,   280,   189,   510,   371,   505,   506,
      -1,    89,   280,   189,   371,   506,    -1,    89,   280,   189,
     510,   371,   506,    -1,    -1,    17,    -1,    -1,    98,   518,
      -1,   280,   189,   371,   505,   506,    -1,   280,   189,   510,
     371,   505,   506,    -1,    29,   280,   189,   509,    -1,   371,
      -1,   509,   194,   371,    -1,    39,    -1,    63,    -1,    41,
      -1,    16,    -1,    45,    -1,   195,   512,   196,    -1,   513,
      -1,   512,   194,   513,    -1,   518,    -1,   374,    -1,   516,
      96,   517,    -1,    60,    -1,   515,    -1,   514,   194,   515,
      -1,   517,    -1,   516,    96,   517,    -1,   537,    -1,   518,
      -1,    60,    -1,   522,   519,    -1,   522,   520,    -1,   522,
     521,    -1,   522,    50,   522,    -1,   522,    53,   522,    -1,
     522,    66,   522,    -1,   522,    -1,   519,     8,   522,    -1,
       8,   522,    -1,   520,    61,   522,    -1,    61,   522,    -1,
     521,    95,   522,    -1,    95,   522,    -1,   524,    -1,   110,
     195,   409,   196,    -1,   524,   523,   524,    -1,   180,    -1,
     181,    -1,   179,    -1,   182,    -1,   178,    -1,   183,    -1,
     526,    -1,   526,   525,   526,    -1,   100,    -1,   101,    -1,
     103,    -1,   102,    -1,   104,    -1,   105,    -1,   528,    -1,
     527,   528,    -1,   184,    -1,   185,    -1,   530,    -1,   528,
     529,   530,    -1,   184,    -1,   185,    -1,   186,    -1,   530,
     531,   532,    -1,   532,    -1,   191,    -1,   192,    -1,    49,
      -1,    75,    -1,   533,    -1,   533,    97,   533,    -1,     3,
     533,    -1,    54,   533,    -1,   537,    -1,   548,    -1,   551,
      -1,   536,    -1,   534,    -1,   195,   518,   196,    -1,    51,
     538,   535,    -1,    51,   538,   538,   535,    -1,    51,   536,
      -1,    -1,   511,    -1,   538,   199,   195,   518,   196,    -1,
     538,   199,   551,    -1,   538,    -1,   538,   199,   130,   195,
     526,   196,    -1,   540,    -1,   174,    -1,   539,    -1,   538,
     200,   542,    -1,   541,    -1,   543,    -1,   168,    -1,   547,
      -1,   174,    -1,   176,    -1,   168,    -1,     7,    -1,   538,
     199,   544,    -1,   540,   199,   544,    -1,   537,   303,   199,
     544,    -1,   174,    -1,   174,   195,   545,   196,    -1,    72,
      -1,    72,   195,   546,   196,    -1,   518,    -1,   545,   194,
     518,    -1,    -1,   546,   194,   518,    -1,   538,   511,    -1,
     541,   511,    -1,   170,    -1,   173,    -1,   172,    -1,   176,
      -1,   169,    -1,    90,    -1,   550,    -1,   174,    -1,   170,
     174,    -1,   173,   174,    -1,   172,   174,    -1,   170,   174,
      -1,   173,   174,    -1,   172,   174,    -1,   195,   553,   194,
     552,   196,    -1,   195,   554,    96,   518,   196,    -1,   553,
      -1,   552,   194,   553,    -1,   518,    -1,   554,    96,   518,
      -1,   554,   187,   555,    -1,   555,    -1,   526,    -1,   374,
      -1,    62,    -1,   147,    -1,   557,   558,    -1,   558,    -1,
      -1,   131,   174,   188,   559,   563,   570,   132,    -1,   155,
     560,   188,    -1,   561,   192,   562,    -1,   562,    -1,   561,
     192,   174,    -1,   174,    -1,   201,    -1,    -1,   174,    -1,
     191,    -1,   563,   564,    -1,    -1,   565,    -1,   566,    -1,
     567,    -1,   135,   174,   180,   569,   188,    -1,   161,   174,
     180,   173,   188,    -1,   161,   174,   180,   185,   173,   188,
      -1,   133,   568,   188,    -1,   133,   568,   180,   202,   138,
     203,   188,    -1,   133,   568,   180,   202,   569,   203,   188,
      -1,   133,   568,   180,   202,   138,   194,   569,   203,   188,
      -1,   174,    -1,   568,   194,   174,    -1,   179,   144,   194,
     144,   188,   144,   194,   144,   178,    -1,   179,   144,   194,
     144,   194,   174,   195,   173,   194,   173,   196,   188,   144,
     194,   144,   194,   174,   195,   173,   194,   173,   196,   178,
      -1,   179,   144,   194,   144,   194,   174,   195,   173,   194,
     173,   196,   188,   144,   194,   144,   178,    -1,   179,   144,
     194,   144,   188,   144,   194,   144,   194,   174,   195,   173,
     194,   173,   196,   178,    -1,   179,   144,   194,   144,   178,
      -1,   179,   144,   194,   144,   194,   174,   195,   173,   194,
     173,   196,   178,    -1,   174,    -1,   570,   574,    -1,   570,
     571,    -1,   574,    -1,   571,    -1,   174,   174,   195,   572,
     196,   188,    -1,   572,   194,   573,    -1,   573,    -1,   174,
     154,   174,    -1,   146,   174,   188,   584,   136,    -1,   146,
     174,   188,   584,   189,   136,    -1,   157,   174,   188,   584,
     575,    -1,   157,   174,   188,   584,   189,   575,    -1,   148,
     174,   188,   576,   149,    -1,   162,   174,   188,   579,   163,
      -1,   164,   174,   188,   584,   165,    -1,   156,    -1,    26,
      -1,   591,    96,   175,   591,    96,   175,    -1,   577,    96,
     175,   577,    96,   175,    -1,   577,   187,   578,    -1,   578,
      -1,   591,   195,   588,   196,    -1,   591,    96,   175,    -1,
     591,    96,   179,   144,   194,   144,   178,   175,    -1,   175,
      96,   580,    -1,   175,   204,    96,   580,    -1,   580,   187,
     581,    -1,   581,    -1,   581,   186,   582,    -1,   582,    -1,
     175,    -1,   174,    -1,   179,   144,   194,   144,   178,   175,
      -1,   583,    -1,   137,    -1,   584,   187,   584,    -1,   584,
     188,   584,    -1,   584,   189,   584,    -1,   584,   134,   584,
      -1,   195,   584,   196,    -1,   582,    -1,   585,    -1,   197,
     586,   198,    -1,   197,   589,   198,    -1,   191,   197,   584,
     198,    -1,   586,   187,   587,    -1,   587,    -1,   589,    96,
     584,    -1,   589,    96,   584,   188,   191,    -1,   589,   195,
     588,   196,    96,   584,    -1,   589,   195,   588,   196,    96,
     584,   188,   191,    -1,   589,    96,   189,   584,    -1,   589,
      96,   189,   584,   188,   191,    -1,   589,   195,   588,   196,
      96,   189,   584,    -1,   589,   195,   588,   196,    96,   189,
     584,   188,   191,    -1,   173,    -1,   144,    -1,   174,    -1,
     166,   195,   588,   194,   588,   196,    -1,   167,   195,   588,
     194,   588,   196,    -1,   588,   191,   173,    -1,   588,   191,
     144,    -1,   588,   191,   174,    -1,   588,   191,   166,   195,
     588,   194,   588,   196,    -1,   588,   191,   167,   195,   588,
     194,   588,   196,    -1,   588,   192,   173,    -1,   588,   192,
     144,    -1,   588,   192,   174,    -1,   588,   192,   166,   195,
     588,   194,   588,   196,    -1,   588,   192,   167,   195,   588,
     194,   588,   196,    -1,   590,    -1,   591,    -1,   179,   144,
     194,   144,   178,   591,    -1,   590,   187,   594,    -1,   594,
      -1,   591,   187,   592,    -1,   592,    -1,   592,   186,   593,
      -1,   593,    -1,   174,    -1,   201,   174,    -1,   195,   591,
     196,    -1,   201,   195,   591,   196,    -1,   138,    -1,   594,
     186,   595,    -1,   595,    -1,   175,    -1,   175,   192,   144,
      -1,   175,   204,    -1,   195,   590,   196,    -1,   583,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   322,   322,   322,   329,   330,   333,   334,   337,   339,
     341,   344,   345,   359,   360,   363,   364,   365,   368,   369,
     372,   375,   377,   387,   386,   412,   414,   419,   420,   421,
     422,   424,   426,   427,   428,   432,   435,   438,   440,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   461,   462,   463,   464,
     465,   469,   471,   472,   474,   475,   478,   480,   481,   482,
     483,   485,   487,   490,   491,   492,   496,   495,   525,   526,
     527,   528,   530,   532,   534,   536,   538,   541,   546,   549,
     551,   552,   553,   556,   557,   558,   561,   563,   566,   570,
     571,   574,   575,   576,   578,   580,   582,   584,   587,   588,
     591,   592,   594,   595,   600,   599,   609,   610,   613,   615,
     617,   619,   622,   623,   624,   625,   626,   627,   628,   629,
     630,   631,   632,   633,   634,   635,   636,   637,   639,   640,
     641,   644,   655,   658,   666,   668,   670,   672,   675,   676,
     677,   678,   679,   680,   681,   682,   683,   684,   685,   693,
     706,   711,   718,   731,   747,   749,   750,   753,   767,   772,
     778,   780,   781,   784,   785,   789,   790,   792,   794,   797,
     798,   799,   800,   801,   802,   803,   804,   805,   806,   807,
     808,   809,   810,   813,   814,   817,   842,   850,   851,   852,
     853,   856,   860,   861,   869,   870,   875,   879,   880,   881,
     886,   888,   898,   900,   904,   990,   992,   992,   995,   996,
     997,  1000,  1010,  1022,  1024,  1029,  1035,  1065,  1074,  1083,
    1092,  1101,  1113,  1116,  1117,  1121,  1125,  1126,  1129,  1130,
    1136,  1139,  1138,  1142,  1144,  1147,  1148,  1151,  1154,  1159,
    1160,  1161,  1164,  1165,  1168,  1169,  1171,  1173,  1176,  1177,
    1180,  1184,  1190,  1192,  1194,  1196,  1200,  1216,  1199,  1238,
    1239,  1240,  1242,  1245,  1247,  1250,  1252,  1256,  1269,  1268,
    1277,  1280,  1281,  1282,  1285,  1286,  1289,  1293,  1295,  1299,
    1299,  1299,  1299,  1299,  1300,  1300,  1300,  1300,  1300,  1300,
    1301,  1301,  1301,  1301,  1301,  1301,  1302,  1302,  1302,  1302,
    1305,  1308,  1312,  1313,  1314,  1317,  1321,  1326,  1328,  1329,
    1330,  1333,  1335,  1340,  1343,  1344,  1347,  1348,  1349,  1353,
    1356,  1362,  1363,  1364,  1367,  1369,  1374,  1378,  1381,  1384,
    1388,  1387,  1392,  1393,  1396,  1398,  1402,  1401,  1409,  1418,
    1423,  1431,  1433,  1438,  1441,  1444,  1445,  1455,  1456,  1459,
    1464,  1472,  1473,  1485,  1492,  1499,  1506,  1514,  1523,  1534,
    1535,  1538,  1571,  1575,  1579,  1586,  1590,  1596,  1597,  1600,
    1604,  1607,  1608,  1611,  1614,  1632,  1634,  1636,  1638,  1640,
    1645,  1651,  1661,  1674,  1677,  1693,  1697,  1706,  1712,  1719,
    1722,  1728,  1736,  1740,  1741,  1745,  1750,  1751,  1756,  1760,
    1762,  1764,  1768,  1769,  1779,  1780,  1781,  1782,  1783,  1784,
    1785,  1786,  1789,  1792,  1798,  1799,  1802,  1805,  1807,  1809,
    1811,  1813,  1814,  1815,  1816,  1816,  1818,  1819,  1820,  1821,
    1824,  1826,  1829,  1830,  1832,  1836,  1837,  1838,  1839,  1840,
    1841,  1842,  1843,  1844,  1845,  1846,  1847,  1848,  1849,  1850,
    1851,  1852,  1853,  1854,  1856,  1857,  1858,  1859,  1860,  1864,
    1863,  1909,  1910,  1913,  1922,  1928,  1935,  1939,  1940,  1943,
    1944,  1950,  1956,  1963,  1965,  1968,  1969,  1970,  1971,  1972,
    1973,  1974,  1975,  1976,  1977,  1978,  1979,  1980,  1981,  1984,
    1985,  1995,  1997,  1998,  1999,  2002,  2008,  2109,  2110,  2111,
    2112,  2115,  2127,  2142,  2143,  2146,  2152,  2154,  2155,  2156,
    2157,  2159,  2160,  2163,  2240,  2241,  2243,  2245,  2246,  2247,
    2250,  2251,  2252,  2255,  2264,  2279,  2280,  2293,  2305,  2324,
    2350,  2353,  2359,  2362,  2364,  2374,  2375,  2380,  2382,  2384,
    2385,  2388,  2389,  2397,  2402,  2404,  2493,  2794,  2798,  2802,
    2808,  2815,  2823,  2824,  2908,  2918,  2927,  2960,  2996,  3063,
    3133,  3134,  3176,  3177,  3184,  3201,  3221,  3239,  3260,  3259,
    3306,  3307,  3311,  3318,  3324,  3326,  3331,  3332,  3333,  3334,
    3335,  3336,  3337,  3338,  3339,  3340,  3341,  3342,  3349,  3361,
    3366,  3371,  3376,  3381,  3386,  3391,  3396,  3401,  3406,  3411,
    3416,  3421,  3426,  3431,  3436,  3442,  3447,  3455,  3455,  3455,
    3456,  3456,  3456,  3459,  3463,  3467,  3472,  3473,  3474,  3475,
    3476,  3478,  3479,  3485,  3486,  3492,  3500,  3501,  3508,  3509,
    3516,  3536,  3573,  3574,  3575,  3579,  3580,  3582,  3584,  3586,
    3590,  3603,  3606,  3626,  3656,  3658,  3659,  3661,  3666,  3674,
    3685,  3686,  3693,  3694,  3700,  3706,  3709,  3710,  3716,  3747,
    3788,  3793,  3794,  3799,  3882,  3890,  3901,  3912,  3924,  3925,
    3934,  3941,  3950,  3967,  3979,  3997,  4001,  4047,  4097,  4098,
    4108,  4109,  4113,  4114,  4117,  4120,  4121,  4130,  4153,  4178,
    4196,  4217,  4243,  4272,  4277,  4286,  4289,  4290,  4291,  4292,
    4292,  4296,  4297,  4298,  4299,  4299,  4303,  4304,  4307,  4308,
    4311,  4312,  4316,  4318,  4321,  4322,  4327,  4328,  4343,  4345,
    4355,  4356,  4370,  4371,  4372,  4373,  4374,  4376,  4377,  4381,
    4385,  4408,  4409,  4410,  4416,  4418,  4422,  4465,  4544,  4547,
    4553,  4555,  4558,  4559,  4564,  4579,  4671,  4675,  4677,  4681,
    4681,  4682,  4682,  4683,  4686,  4692,  4693,  4704,  4710,  4711,
    4716,  4722,  4727,  4731,  4733,  4740,  4743,  4744,  4750,  4759,
    4768,  4779,  4789,  4799,  4811,  4814,  4823,  4829,  4838,  4841,
    4851,  4855,  4856,  4859,  4889,  4889,  4889,  4890,  4890,  4890,
    4893,  4894,  4905,  4905,  4905,  4905,  4905,  4905,  4908,  4909,
    4916,  4917,  4920,  4921,  4929,  4929,  4929,  4932,  4938,  4941,
    4941,  4942,  4942,  4945,  4946,  4947,  4948,  4956,  4964,  4965,
    4966,  4968,  4970,  4974,  4975,  4976,  4978,  4980,  4983,  4984,
    4987,  5007,  5015,  5018,  5020,  5024,  5028,  5029,  5032,  5045,
    5048,  5049,  5050,  5051,  5059,  5067,  5073,  5081,  5082,  5084,
    5085,  5089,  5090,  5094,  5095,  5099,  5138,  5147,  5155,  5162,
    5169,  5176,  5182,  5183,  5186,  5187,  5193,  5199,  5209,  5215,
    5228,  5243,  5255,  5271,  5272,  5287,  5288,  5304,  5317,  5328,
    5329,  5330,  5333,  5348,  5349,  5353,  5352,  5377,  5385,  5399,
    5410,  5415,  5417,  5428,  5432,  5434,  5438,  5450,  5454,  5455,
    5456,  5461,  5469,  5474,  5482,  5493,  5504,  5516,  5531,  5536,
    5573,  5575,  5581,  5586,  5591,  5593,  5596,  5603,  5604,  5615,
    5616,  5620,  5655,  5657,  5661,  5665,  5680,  5695,  5710,  5725,
    5740,  5756,  5774,  5774,  5777,  5797,  5810,  5812,  5814,  5822,
    5832,  5842,  5849,  5858,  5860,  5863,  5865,  5868,  5873,  5878,
    5881,  5884,  5887,  5889,  5891,  5893,  5895,  5896,  5897,  5900,
    5901,  5902,  5905,  5907,  5910,  5912,  5917,  5922,  5928,  5930,
    5935,  5940,  5959,  5960,  5961,  5964,  5965,  5966,  5967,  5968,
    5971,  5973,  5975,  5976,  5977,  5980,  5982,  5986,  5987,  5992,
    6002,  6004,  6007,  6013,  6016,  6022,  6025,  6026,  6031,  6036,
    6042,  6051,  6053,  6056,  6062,  6068,  6078,  6080
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ABSOLUTE", "ACCESS", "AFTER", "ALIAS",
  "ALL", "AND", "ARCHITECTURE", "ARRAY", "ASSERT", "ATTRIBUTE", "VBEGIN",
  "BLOCK", "BODY", "BUFFER", "BUS", "CASE", "COMPONENT", "CONFIGURATION",
  "CONSTANT", "DISCONNECT", "DOWNTO", "ELSE", "ELSIF", "END", "ENTITY",
  "EXIT", "VFILE", "FOR", "FUNCTION", "PROCEDURE", "NATURAL", "GENERATE",
  "GENERIC", "GUARDED", "IF", "IMPURE", "VIN", "INITIALIZE", "INOUT", "IS",
  "LABEL", "LIBRARY", "LINKAGE", "LITERAL", "LOOP", "MAP", "MOD", "NAND",
  "NEW", "NEXT", "NOR", "NOT", "UNAFFECTED", "UNITS", "GROUP", "OF", "ON",
  "OPEN", "OR", "OTHERS", "VOUT", "VREJECT", "INERTIAL", "XNOR", "PACKAGE",
  "PORT", "POSTPONED", "PROCESS", "PURE", "RANGE", "RECORD", "REGISTER",
  "REM", "REPORT", "RETURN", "SELECT", "SEVERITY", "SHARED", "SIGNAL",
  "SUBTYPE", "THEN", "TO", "TRANSPORT", "TYPE", "UNTIL", "USE", "VARIABLE",
  "VNULL", "WAIT", "WHEN", "WHILE", "WITH", "XOR", "ARROW", "EXPON",
  "ASSIGN", "BOX", "SLL", "SRL", "SLA", "SRA", "ROL", "ROR", "AT", "INF",
  "EOC", "ASSIGNMENT", "PROBE", "RECEIVE", "SEND", "VASSIGNMENT",
  "INITCHANNEL", "ACROSS", "BREAK", "EQ", "LIMIT", "NATURE", "NOISE",
  "PROCEDURAL", "QUANTITY", "REFERENCE", "SPECTRUM", "SUBNATURE",
  "TERMINAL", "THROUGH", "TOLER", "VDOT", "ABOVE", "MODULE", "ENDMODULE",
  "BOOL", "PARA", "ATACS_DELAY", "ENDPROC", "SKIP", "INIT", "DEL",
  "BOOLEAN", "SLASH", "CID", "CPORT", "CINT", "CARROW", "PROC",
  "RECURSIVE_ERROR", "GATE", "ENDGATE", "GUARD", "GUARDAND", "GUARDOR",
  "GUARDTO", "LARROW", "INCLUDE", "ENDTB", "TESTBENCH", "AWAIT",
  "AWAITALL", "AWAITANY", "ATACS_DEFINE", "CONSTRAINT", "ENDCONSTRAINT",
  "ASSUMP", "ENDASSUMP", "SUM", "DIFF", "STRLIT", "BITLIT", "INT", "VBOOL",
  "BASEDLIT", "REAL", "VID", "ACTION", "CHARLIT", "ID", "'>'", "'<'",
  "'='", "NEQ", "LE", "GE", "'+'", "'-'", "'&'", "'|'", "';'", "':'",
  "MED_PRECEDENCE", "'*'", "'/'", "MAX_PRECEDENCE", "','", "'('", "')'",
  "'['", "']'", "'\\''", "'.'", "'~'", "'{'", "'}'", "'@'", "$accept",
  "start_point", "$@1", "design_file", "design_unit", "context_clause",
  "context_item", "library_unit", "primary_unit", "secondary_unit",
  "library_clause", "library_list", "entity_declaration", "$@2",
  "entity_descr", "endofentity", "entity_header", "generic_clause",
  "port_clause", "entity_declarative_part", "entity_declarative_item",
  "group_template_declaration", "gtp_body", "box_symbol",
  "group_declaration", "gd_body", "entity_statement_part",
  "entity_statement", "architecture_body", "$@3", "architail",
  "architecture_declarative_part", "architecture_statement_part",
  "configuration_declaration", "config_tail", "configtail",
  "configuration_declarative_item", "configuration_declarative_part",
  "block_configuration", "block_specification", "www", "us", "ci",
  "configuration_item", "component_configuration", "package_declaration",
  "$@4", "package_tail", "packagetail", "package_declarative_part",
  "package_declarative_item", "package_body_head", "package_body",
  "pac_body_tail", "package_body_declarative_part",
  "package_body_declarative_item", "subprogram_declaration",
  "subprogram_specification", "func_head", "sub_body_head",
  "subprogram_body", "subprog_tail", "subprogram_statement_part",
  "designator", "subprogram_declarative_part",
  "subprogram_declarative_item", "type_declaration",
  "full_type_declaration", "incomplete_type_declaration",
  "type_definition", "scalar_type_definition", "composite_type_definition",
  "nature_declaration", "nature_definition", "constant_declaration",
  "identifier_list", "signal_declaration", "signal_kind", "signal_mode",
  "variable_declaration", "var_decl_head", "terminal_declaration",
  "quantity_declaration", "across_aspect", "tolerances", "through_aspect",
  "terminal_aspect", "source_aspect", "file_declaration", "$@5",
  "file_access_mode", "external_file_association", "file_logical_name",
  "alias_declaration", "alias_name", "al_decl_head", "al_decl_tail",
  "signature_symbol", "signature", "signature_body", "signature_tail",
  "recursive_units", "component_declaration", "$@6", "$@7",
  "component_declaration_tail", "comp_decl_head", "comp_decl_tail",
  "attribute_declaration", "attribute_specification", "$@8",
  "entity_specification", "entity_name_list", "entity_name_sequence",
  "entity_designator", "entity_class", "configuration_specification",
  "component_specification", "instantiation_list", "component_list",
  "binding_indication", "entity_aspect", "port_map_aspect",
  "generic_map_aspect", "map_aspects", "disconnection_specification",
  "guarded_signal_specification", "signal_list", "signal_id_list",
  "step_limit_specification", "use_clause", "initialization_specification",
  "signal_specification", "label_declaration", "$@9",
  "statement_name_list", "statement_name", "label_array", "$@10",
  "enumeration_type_definition", "ee", "enumeration_literal",
  "integer_floating_type_definition", "range_constraint", "range",
  "direction", "physical_type_definition", "sud", "base_unit_declaration",
  "secondary_unit_declaration", "array_type_definition",
  "unconstrainted_array_definition", "constrained_array_definition", "isd",
  "index_subtype_definition", "index_constraint", "disdis",
  "record_type_definition", "elde", "element_declaration",
  "access_type_definition", "file_type_definition", "size_constraint",
  "subtype_declaration", "sub_indications", "more_toler",
  "subtype_indication", "subtype_indication1", "discrete_range",
  "discrete_range1", "scalar_nature_definition", "array_nature_definition",
  "record_nature_definition", "record_nature_alternative",
  "subnature_declaration", "subnature_indication", "subnature_indication1",
  "concurrent_statement", "block_statement", "block_end", "guarded_exp",
  "block_label", "is_symbol", "block_header", "$@11",
  "block_declarative_part", "block_statement_part",
  "block_declarative_item", "process_statement", "$@12", "process_end",
  "process_head", "postpone", "process_tail", "sensitivity_list",
  "process_declarative_part", "process_declarative_item",
  "process_statement_part", "concurrent_procedure_call",
  "component_instantiation_statement", "cmpnt_stmt_body",
  "concurrent_assertion_statement",
  "concurrent_signal_assignment_statement",
  "conditional_signal_assignment", "target", "options", "delay_mechanism",
  "conditional_waveforms", "waveform_head", "selected_signal_assignment",
  "selected_waveforms", "generate_statement", "generation_head",
  "generate_tail", "generate_declarative_part",
  "concurrent_statement_sequence", "generation_scheme",
  "parameter_specification", "concurrent_break_statement",
  "simultaneous_statement", "simul_statement",
  "sequence_of_simul_statements", "simple_simultaneous_statement",
  "simultaneous_if_statement", "simul_elsifthen", "simul_else_part",
  "simultaneous_case_statement", "simul_case_statement_alternative",
  "simultaneous_procedural_statement", "$@13", "procedural_end",
  "procedural_head", "sequence_of_declarative_items",
  "procedural_declarative_item", "sequential_statement",
  "guardish_statement", "guard_statement", "guard_or_statement",
  "guard_and_statement", "await_statement", "await_any_statement",
  "await_all_statement", "target_factors", "target_terms", "probedChannel",
  "and_stmts", "or_stmts", "andor_stmt", "andor_ams", "wait_statement",
  "condition_clause", "sensitivity_clause", "timeout_clause",
  "assertion_statement", "tralass", "communication",
  "communicationStatement", "parallelCommunication", "fourPhase",
  "assign_statements", "assign_statement", "assign_stmt",
  "rate_assignment", "vassign_statements", "vassign_statement",
  "vassign_stmt", "signal_assignment_statement", "waveform",
  "waveform_element", "variable_assignment_statement",
  "procedure_call_statement", "if_statement", "elsifthen", "else_part",
  "label_symbol", "label", "sequence_of_statements", "case_statement",
  "case_statement_alternative", "loop_statement", "iteration_scheme",
  "next_statement", "$@14", "exit_statement", "$@15", "return_statement",
  "null_statement", "report_statement", "break_statement", "when_symbol",
  "break_elements", "break_element", "interface_list",
  "interface_declaration", "interface_terminal_declaration",
  "interface_quantity_declaration", "in_out_symbol",
  "interface_constant_declaration", "interface_signal_declaration",
  "interface_variable_declaration", "bus_symbol", "assign_exp",
  "interface_unknown_declaration", "interface_file_declaration",
  "subtype_indication_list", "mode", "gen_association_list",
  "gen_association_list_1", "gen_association_element", "association_list",
  "association_element", "formal_part", "actual_part", "expression",
  "andexpr", "orexpr", "xorexpr", "relation", "relational_operator",
  "shift_expression", "shift_operators", "simple_expression", "sign",
  "terms", "adding_operator", "term", "multiplying_operator", "factor",
  "primary", "allocator", "allocator1", "qualified_expression", "name",
  "mark", "selected_name", "name2", "name3", "suffix", "attribute_name",
  "idparan", "expression_list", "expressions", "insl_name", "literal",
  "physical_literal", "physical_literal_no_default", "aggregate", "elarep",
  "element_association", "choices", "choice", "recursive_errors",
  "csp_compiler", "basic_module", "$@16", "filename", "path_name",
  "file_name", "decls", "decl", "delaydecl", "definedecl", "booldecl",
  "id_list", "delay", "constructs", "component_instances", "port_map_list",
  "formal_2_actual", "process", "tail", "prs", "leakyexprs", "leakyexpr",
  "constraint", "or_constraint", "and_constraint", "action", "skip",
  "body", "guardstruct", "guardcmdset", "guardcmd", "sing_dist", "expr",
  "actionexpr", "levelexpr", "levelconjunct", "leveliteral", "conjunct",
  "csp_literal", YY_NULL
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,    62,    60,
      61,   433,   434,   435,    43,    45,    38,   124,    59,    58,
     436,    42,    47,   437,    44,    40,    41,    91,    93,    39,
      46,   126,   123,   125,    64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   205,   207,   206,   206,   206,   208,   208,   209,   210,
     210,   211,   211,   212,   212,   213,   213,   213,   214,   214,
     215,   216,   216,   218,   217,   219,   219,   220,   220,   220,
     220,   221,   221,   221,   221,   222,   223,   224,   224,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   226,   227,   227,   228,   228,   229,   230,   230,   230,
     230,   231,   231,   232,   232,   232,   234,   233,   235,   235,
     235,   235,   236,   236,   237,   237,   237,   238,   239,   240,
     240,   240,   240,   241,   241,   241,   242,   242,   243,   244,
     244,   245,   245,   245,   246,   246,   247,   247,   248,   248,
     249,   249,   249,   249,   251,   250,   252,   252,   253,   253,
     254,   254,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   256,   257,   257,   258,   258,   259,   259,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   261,
     262,   262,   262,   262,   263,   263,   263,   264,   265,   265,
     266,   266,   266,   267,   267,   268,   268,   269,   269,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,   271,   271,   272,   273,   274,   274,   274,
     274,   275,   275,   275,   276,   276,   277,   278,   278,   278,
     279,   279,   280,   280,   281,   282,   282,   282,   283,   283,
     283,   284,   284,   285,   285,   286,   287,   287,   287,   287,
     287,   287,   288,   289,   289,   290,   291,   291,   292,   292,
     293,   294,   293,   295,   295,   296,   296,   297,   298,   299,
     299,   299,   300,   300,   301,   301,   302,   302,   303,   303,
     304,   304,   305,   305,   306,   306,   308,   309,   307,   310,
     310,   310,   310,   311,   311,   312,   312,   313,   315,   314,
     316,   317,   317,   317,   318,   318,   319,   319,   319,   320,
     320,   320,   320,   320,   320,   320,   320,   320,   320,   320,
     320,   320,   320,   320,   320,   320,   320,   320,   320,   320,
     321,   322,   323,   323,   323,   324,   324,   325,   325,   325,
     325,   326,   326,   326,   327,   328,   329,   329,   329,   330,
     331,   332,   332,   332,   333,   333,   334,   335,   336,   337,
     339,   338,   340,   340,   341,   341,   343,   342,   344,   345,
     345,   346,   346,   347,   348,   349,   349,   350,   350,   351,
     351,   352,   352,   353,   354,   355,   355,   356,   357,   358,
     358,   359,   360,   361,   361,   362,   362,   363,   363,   364,
     365,   366,   366,   367,   368,   369,   369,   369,   369,   369,
     370,   371,   371,   371,   372,   372,   372,   372,   373,   373,
     374,   374,   375,   376,   376,   377,   378,   378,   379,   380,
     380,   380,   381,   381,   382,   382,   382,   382,   382,   382,
     382,   382,   383,   383,   384,   384,   385,   386,   387,   387,
     388,   388,   388,   388,   389,   388,   388,   388,   388,   388,
     390,   390,   391,   391,   391,   392,   392,   392,   392,   392,
     392,   392,   392,   392,   392,   392,   392,   392,   392,   392,
     392,   392,   392,   392,   392,   392,   392,   392,   392,   394,
     393,   395,   395,   396,   396,   396,   396,   397,   397,   398,
     398,   399,   399,   400,   400,   401,   401,   401,   401,   401,
     401,   401,   401,   401,   401,   401,   401,   401,   401,   402,
     402,   403,   403,   403,   403,   404,   404,   405,   405,   405,
     405,   406,   406,   406,   406,   407,   407,   407,   407,   407,
     407,   407,   407,   408,   409,   409,   410,   410,   410,   410,
     411,   411,   411,   412,   412,   413,   413,   414,   415,   415,
     416,   416,   417,   418,   418,   419,   419,   420,   420,   420,
     420,   421,   421,   422,   423,   423,   424,   425,   425,   425,
     425,   425,   426,   426,   427,   427,   427,   427,   428,   428,
     429,   429,   430,   430,   431,   431,   432,   432,   434,   433,
     435,   435,   436,   436,   437,   437,   438,   438,   438,   438,
     438,   438,   438,   438,   438,   438,   438,   438,   439,   439,
     439,   439,   439,   439,   439,   439,   439,   439,   439,   439,
     439,   439,   439,   439,   439,   439,   439,   440,   440,   440,
     440,   440,   440,   441,   441,   441,   442,   443,   444,   445,
     446,   447,   447,   448,   448,   449,   450,   450,   451,   451,
     452,   453,   454,   454,   454,   454,   454,   454,   454,   454,
     455,   456,   457,   458,   459,   459,   459,   459,   460,   460,
     461,   461,   462,   462,   463,   464,   465,   465,   466,   467,
     468,   469,   469,   470,   471,   471,   471,   471,   472,   472,
     472,   473,   473,   474,   474,   475,   476,   476,   477,   477,
     478,   478,   479,   479,   480,   481,   481,   482,   482,   483,
     483,   484,   484,   485,   485,   485,   486,   486,   486,   487,
     486,   488,   488,   488,   489,   488,   490,   490,   491,   491,
     492,   492,   493,   493,   494,   494,   495,   495,   496,   496,
     497,   497,   498,   498,   498,   498,   498,   498,   498,   499,
     500,   501,   501,   501,   502,   502,   503,   503,   504,   504,
     505,   505,   506,   506,   507,   507,   508,   509,   509,   510,
     510,   510,   510,   510,   511,   512,   512,   513,   513,   513,
     513,   514,   514,   515,   515,   516,   517,   517,   518,   518,
     518,   518,   518,   518,   518,   519,   519,   520,   520,   521,
     521,   522,   522,   522,   523,   523,   523,   523,   523,   523,
     524,   524,   525,   525,   525,   525,   525,   525,   526,   526,
     527,   527,   528,   528,   529,   529,   529,   530,   530,   531,
     531,   531,   531,   532,   532,   532,   532,   533,   533,   533,
     533,   533,   533,   534,   534,   534,   535,   535,   536,   536,
     537,   537,   537,   538,   538,   539,   540,   540,   541,   541,
     542,   542,   542,   542,   543,   543,   543,   544,   544,   544,
     544,   545,   545,   546,   546,   547,   547,   548,   548,   548,
     548,   548,   548,   548,   549,   549,   549,   549,   550,   550,
     550,   551,   551,   552,   552,   553,   553,   554,   554,   555,
     555,   555,   556,   557,   557,   559,   558,   558,   560,   560,
     561,   561,   561,   561,   562,   562,   563,   563,   564,   564,
     564,   565,   566,   566,   567,   567,   567,   567,   568,   568,
     569,   569,   569,   569,   569,   569,   569,   570,   570,   570,
     570,   571,   572,   572,   573,   574,   574,   574,   574,   574,
     574,   574,   575,   575,   576,   576,   577,   577,   578,   579,
     579,   579,   579,   580,   580,   581,   581,   582,   582,   582,
     582,   583,   584,   584,   584,   584,   584,   584,   584,   585,
     585,   585,   586,   586,   587,   587,   587,   587,   587,   587,
     587,   587,   588,   588,   588,   588,   588,   588,   588,   588,
     588,   588,   588,   588,   588,   588,   588,   589,   589,   589,
     590,   590,   591,   591,   592,   592,   593,   593,   593,   593,
     593,   594,   594,   595,   595,   595,   595,   595
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     2,     2,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     0,     4,     6,     8,     0,     1,     1,
       2,     0,     1,     1,     2,     5,     5,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     7,     4,     2,     0,     1,     8,     3,     3,     1,
       1,     0,     2,     1,     1,     1,     0,    12,     0,     2,
       1,     1,     0,     2,     0,     2,     2,     5,     6,     0,
       1,     1,     2,     1,     1,     1,     0,     2,     7,     1,
       4,     1,     1,     1,     0,     2,     0,     2,     1,     1,
       5,     7,     6,     8,     0,     4,     5,     6,     0,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     6,     7,     0,     2,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     5,     5,     8,     0,     1,     1,     2,     7,     8,
       0,     1,     1,     0,     2,     1,     1,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     5,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     1,     1,     1,
       5,     7,     1,     3,     8,     0,     1,     1,     0,     2,
       2,     6,     8,     0,     1,     5,     6,     3,     4,     4,
       5,     6,     4,     2,     0,     4,     1,     3,     4,     2,
       5,     0,     7,     0,     2,     3,     4,     1,     7,     1,
       1,     1,     0,     2,     1,     0,     0,     1,     4,     3,
       1,     3,     0,     2,     0,     1,     0,     0,     7,     1,
       2,     2,     3,     0,     1,     3,     4,     5,     0,     8,
       3,     1,     1,     1,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     3,     1,     1,     1,     1,     3,     0,     2,     3,
       1,     2,     2,     1,     5,     5,     1,     1,     2,     5,
       3,     1,     1,     1,     1,     3,     5,     3,     5,     3,
       0,     4,     1,     3,     1,     1,     0,     3,     3,     1,
       3,     1,     1,     1,     2,     1,     3,     1,     1,     6,
       7,     0,     2,     2,     4,     1,     1,     6,     4,     1,
       2,     3,     3,     1,     3,     4,     5,     1,     2,     4,
       2,     3,     4,     3,     5,     2,     3,     3,     4,     2,
       3,     1,     2,     1,     2,     3,     2,     3,     1,     1,
       3,     1,     6,     6,     4,     5,     4,     5,     5,     1,
       2,     1,     6,     7,     1,     1,     1,     1,     1,     1,
       1,     1,     7,     8,     3,     4,     3,     2,     0,     1,
       0,     1,     3,     1,     0,     4,     2,     4,     4,     6,
       0,     2,     0,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       9,     2,     3,     1,     2,     2,     3,     0,     1,     1,
       4,     1,     3,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       2,     1,     2,     2,     3,     4,     4,     1,     2,     2,
       2,     1,     2,     2,     3,     1,     2,     1,     2,     2,
       3,     2,     3,     5,     1,     1,     0,     1,     1,     2,
       1,     1,     3,     2,     4,     0,     5,     8,     3,     5,
       8,     9,     2,     0,     1,     2,     0,     1,     1,     2,
       2,     2,     2,     3,     6,     5,     1,     1,     1,     1,
       1,     1,     0,     2,     6,     5,     7,     6,    11,     9,
       0,     5,     0,     2,     9,     8,     4,     5,     0,     8,
       2,     3,     1,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     5,     5,     5,     5,     5,     5,     5,
       5,     1,     3,     1,     3,     1,     1,     3,     1,     3,
       3,     5,     3,     4,     4,     4,     5,     5,     6,     5,
       2,     2,     2,     4,     0,     2,     2,     4,     1,     1,
       5,     5,     1,     3,     3,     5,     1,     3,     7,     6,
       5,     1,     3,     7,     5,     4,     6,     5,     1,     3,
       1,     1,     3,     5,     4,     2,    10,    11,     0,     5,
       0,     2,     0,     1,     2,     0,     2,     8,     9,     4,
       5,     7,     8,     0,     2,     4,     3,     4,     5,     0,
       7,     3,     4,     5,     0,     7,     3,     4,     2,     3,
       4,     6,     5,     4,     0,     2,     1,     3,     6,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     4,
       6,     1,     1,     0,     5,     6,     6,     7,     5,     6,
       0,     1,     0,     2,     5,     6,     4,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     3,
       1,     1,     3,     1,     3,     1,     1,     1,     2,     2,
       2,     3,     3,     3,     1,     3,     2,     3,     2,     3,
       2,     1,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     3,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     3,     2,     2,     1,     1,     1,
       1,     1,     3,     3,     4,     2,     0,     1,     5,     3,
       1,     6,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     4,     1,     4,     1,
       4,     1,     3,     0,     3,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     5,     5,     1,     3,     1,     3,     3,     1,     1,
       1,     1,     1,     2,     1,     0,     7,     3,     3,     1,
       3,     1,     1,     0,     1,     1,     2,     0,     1,     1,
       1,     5,     5,     6,     3,     7,     7,     9,     1,     3,
       9,    23,    16,    16,     5,    12,     1,     2,     2,     1,
       1,     6,     3,     1,     3,     5,     6,     5,     6,     5,
       5,     5,     1,     1,     6,     6,     3,     1,     4,     3,
       8,     3,     4,     3,     1,     3,     1,     1,     1,     6,
       1,     1,     3,     3,     3,     3,     3,     1,     1,     3,
       3,     4,     3,     1,     3,     5,     6,     8,     4,     6,
       7,     9,     1,     1,     1,     6,     6,     3,     3,     3,
       8,     8,     3,     3,     3,     8,     8,     1,     1,     6,
       3,     1,     3,     1,     3,     1,     1,     2,     3,     4,
       1,     3,     1,     1,     3,     2,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,   892,   903,     0,     9,     5,     4,   894,     0,
     901,   905,   902,     0,     0,   899,     1,     9,     6,     0,
     893,   895,   897,     0,     7,     0,     0,     0,     0,     0,
       0,    10,     8,    13,    14,    11,     0,    15,    18,    16,
      17,     0,    19,    12,   907,   900,   898,     0,     0,    23,
      21,     0,   114,   843,     0,   844,    20,     0,   146,     0,
       0,     0,     0,   141,     0,     0,   337,    22,   164,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   906,   908,
     909,   910,     0,   930,   929,     0,     0,    31,    24,   120,
     115,   853,   852,   850,   851,   845,     0,     0,   144,     0,
       0,   166,     0,   165,   224,     0,     0,   157,   158,   147,
     148,     0,     0,   177,   149,   150,   193,   194,   152,   156,
       0,   153,   154,   155,   151,   918,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   896,   928,   927,    76,    96,
      87,     0,     0,    37,    32,    33,   164,   251,   249,   250,
     252,   212,     0,     0,     0,     0,   176,   175,   160,     0,
       0,     0,   167,   159,     0,   164,     0,     0,   914,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    82,     0,
       0,     0,   164,    34,     0,     0,     0,   118,     0,     0,
       0,     0,     0,   136,   137,   121,   122,     0,   123,   138,
     125,   126,   135,   140,   127,   128,   129,   130,   131,   133,
     134,   132,   124,   139,     0,     0,     0,     0,   145,     0,
     142,     0,     0,     0,     0,     0,     0,   196,     0,   173,
     340,   191,   192,   179,   180,   178,   181,   183,   184,   185,
     186,   187,   189,   190,   188,   182,     0,     0,   919,   926,
       0,     0,   961,   958,   957,     0,     0,     0,     0,   967,
     960,     0,   968,  1010,  1006,     0,     0,     0,     0,   947,
       0,  1003,  1005,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   933,   164,     0,     0,     0,    95,    97,     0,
      94,    93,     0,     0,     0,     0,     0,     0,     0,     0,
     730,   737,   738,   732,   733,   734,   736,   735,     0,    71,
      27,     0,     0,    38,    54,    55,    39,    40,    41,    57,
      43,    44,    53,    60,    59,    45,    46,    47,    49,    51,
      56,    52,    50,    48,    42,    58,   278,   266,   333,   332,
     334,     0,     0,   331,   119,     0,     0,     0,     0,     0,
       0,     0,   253,   234,   234,   844,     0,     0,   393,   391,
     213,   143,   241,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,   198,   201,   202,   353,   203,
     204,   365,   366,   205,   199,   200,     0,     0,   692,     0,
       0,     0,     0,     0,   911,     0,     0,     0,  1013,     0,
       0,  1017,     0,   973,     0,   997,   998,  1001,  1012,     0,
     935,     0,     0,     0,     0,  1007,     0,   939,     0,     0,
       0,     0,     0,     0,   943,   942,     0,   937,   912,     0,
       0,     0,   940,     0,   941,     0,     0,     0,     0,    84,
       0,   462,   463,   445,   446,   447,   465,   449,   450,   461,
     468,   467,   451,   452,   453,   454,   456,   458,   459,   464,
     460,   457,   455,   448,   466,    83,   278,    99,   104,     0,
      89,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28,    29,     0,     0,   848,   212,   234,     0,
       0,     0,   236,   840,   842,   846,   847,   849,     0,     0,
       9,     0,     0,     0,     0,   116,     0,     0,     0,     0,
       0,     0,     0,   389,     0,   385,   394,   234,   396,   255,
       0,   210,   392,   240,   243,   161,   290,   300,   291,   297,
     289,   305,   293,   292,   301,   302,   303,   304,   294,   298,
     296,   295,   299,   306,   308,   307,   309,     0,    64,     0,
     384,   380,     0,     0,     0,     0,     0,     0,     0,     0,
     872,   871,   867,   869,   868,   870,   810,   811,     0,   354,
       0,     0,   808,   812,   818,   823,   831,   830,   827,   840,
     355,   828,   873,   829,     0,     0,   377,   351,   352,     0,
     349,   195,     0,   162,     0,     0,   170,     0,     0,   659,
     658,     0,     0,     0,     0,     0,     0,     0,   843,     0,
       0,   174,   614,   617,   618,   619,   620,   621,   622,   598,
     599,     0,   611,   612,   616,   613,   600,   601,   602,   603,
     703,   693,   604,   605,   606,   607,   608,   609,   610,   615,
     524,   840,   525,   344,     0,   342,   345,     0,     0,     0,
       0,     0,     0,     0,   966,     0,  1015,     0,     0,     0,
     969,     0,     0,   970,     0,     0,   965,   962,   963,   936,
     964,  1008,     0,     0,   946,     0,     0,  1002,   983,     0,
       0,   982,   984,     0,  1004,   938,   913,   951,   954,   956,
       0,   949,     0,   934,   932,   931,   692,   314,   313,   315,
     317,     0,   312,     0,   106,    90,    91,     0,     0,     0,
       0,     0,   743,     0,   762,   759,   761,   763,   760,   750,
       0,   731,    35,    36,    27,     0,   473,    72,    75,   428,
      74,    73,   511,   501,     0,     0,    30,    25,     0,     0,
     752,   234,     0,     0,     0,   227,     0,   262,     0,     0,
     865,     0,   866,     0,     0,     9,   267,     0,     0,   784,
     791,   800,   330,   335,   117,   339,     0,   215,     0,     0,
       0,   207,   208,   209,     0,     0,   411,   409,     0,   233,
     770,   401,   768,     0,   765,     0,   767,   800,   827,   840,
       0,   386,   387,   395,   397,     0,   254,     0,     0,     0,
       0,     0,     0,    65,    63,    69,    70,     0,   398,     0,
     369,     0,   399,   373,   840,     0,   381,     0,     0,   825,
     835,   836,   826,   878,   880,   879,   891,   890,   885,   800,
       0,     0,   888,   358,   357,     0,   809,   814,   815,   816,
       0,   821,   822,   819,   820,     0,     0,     0,     0,     0,
     378,     0,   348,     0,   361,     0,   654,   172,   171,     0,
     718,     0,     0,     0,     0,     0,     0,     0,     0,   694,
     885,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   724,     0,     0,     0,   524,   685,     0,
       0,   341,     0,     0,   221,     0,   915,   916,     0,     0,
     971,  1014,     0,  1016,   972,     0,     0,   974,     0,     0,
    1000,  1011,  1009,     0,     0,     0,     0,     0,     0,   948,
       0,     0,   952,     0,    78,     0,     0,   872,     0,   582,
      85,   414,   428,   415,   416,   419,   417,   418,   515,     0,
     517,   420,   421,    86,   556,   557,   558,   559,   560,   428,
       0,   693,   561,     0,   827,   840,   829,     0,     0,     0,
       0,   327,   326,   320,     0,     0,   867,   868,     0,   101,
     840,     0,   105,    92,    88,     0,   752,   757,   756,   750,
       0,   752,     0,   741,   742,     0,   739,   751,   752,   750,
       0,   475,   513,   503,   429,     0,   479,   469,     0,   474,
     512,   502,     0,   752,     0,     0,   752,     0,   228,   229,
     237,     0,   262,     0,   260,     0,   859,     0,   857,   854,
     855,   277,   283,   282,   287,   286,   288,     0,     0,   281,
     256,   273,     0,   329,     0,     0,     0,     0,     0,     0,
     778,   779,   780,   798,   796,   794,   795,   797,   799,     0,
     802,   803,   805,   804,   806,   807,     0,   338,   217,   216,
     752,     0,     0,     0,     0,   206,     0,   408,     0,   410,
     225,     0,   764,     0,     0,     0,   388,   248,   211,   244,
       0,   242,    64,    61,     0,     0,     0,   370,     0,     0,
     372,     0,   865,   368,   383,   382,     0,   837,   833,   836,
     832,     0,     0,     0,   356,   813,   817,   824,     0,   839,
       0,   375,   350,   363,     0,     0,     0,     0,     0,   168,
       0,     0,     0,   666,     0,     0,   671,     0,     0,     0,
       0,   636,     0,     0,   638,   635,     0,     0,   631,     0,
     633,     0,   680,     0,   531,   530,     0,     0,   678,   681,
       0,     0,   662,     0,     0,   714,   711,     0,     0,     0,
     709,   706,     0,   716,     0,     0,     0,     0,   642,     0,
       0,     0,   704,     0,     0,     0,   724,   726,     0,   695,
     719,     0,     0,     0,     0,   347,   343,     0,     0,   924,
       0,     0,     0,     0,   978,     0,     0,     0,     0,     0,
       0,   988,     0,     0,   987,   989,   993,     0,     0,   992,
     994,   953,   955,     0,    80,    81,     0,     0,   519,   521,
       0,     0,   428,   430,   526,   578,     0,     0,   427,     0,
       0,     0,     0,     0,     0,   872,   583,     0,   516,   518,
     546,     0,     0,   827,   840,     0,     0,     0,     0,     0,
       0,   323,   318,   310,   328,   311,   316,   100,     0,     0,
     108,   107,   109,   752,   744,     0,   752,   750,   748,   752,
     752,   754,   752,    26,     0,   481,   483,   476,   514,   504,
     336,     0,     0,     0,     0,   753,   232,   235,     0,   230,
     263,     0,     0,   259,   856,   863,     0,     0,     0,     0,
       0,   284,   257,   274,     0,   269,     0,   786,   781,   782,
     788,   783,   790,     0,     0,     0,   793,   801,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   766,   777,   769,
     776,   400,   390,   245,     0,   247,    62,    67,    68,    66,
       0,     0,   374,   371,   834,     0,   883,     0,   886,   889,
     887,   885,   379,   376,     0,     0,   362,   163,   655,   656,
     653,   169,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   684,     0,     0,   675,     0,     0,     0,     0,
       0,     0,     0,     0,   712,     0,     0,   695,     0,   707,
       0,     0,   720,   717,   652,   651,   650,   644,     0,   643,
       0,     0,   645,     0,   725,   723,     0,     0,     0,   692,
       0,     0,     0,     0,   222,   917,     0,     0,   959,     0,
       0,   975,     0,   945,   944,     0,     0,     0,     0,     0,
       0,     0,    79,    77,   562,     0,     0,   430,   431,   433,
     440,   528,   535,   527,   584,     0,   724,   508,   510,   509,
       0,   551,   552,   520,   522,     0,   692,   542,     0,     0,
     234,     0,     0,     0,   322,   321,   319,     0,    99,   317,
     745,   758,   746,   752,   749,   740,   755,     0,   428,   164,
     239,     0,   231,   226,   261,   258,     0,     0,   861,     0,
       0,   280,   256,     0,   268,   270,   271,   792,   785,   787,
     789,   218,     0,   404,     0,     0,     0,     0,     0,     0,
     246,   367,     0,   881,     0,   882,   838,   359,     0,     0,
       0,   667,   665,     0,   672,   670,   623,   625,   867,   640,
     624,   627,   637,   626,   639,   628,   630,   632,   629,   634,
     532,   674,   679,   682,   664,   661,     0,   663,   660,     0,
       0,   713,     0,   705,   692,   708,     0,     0,   649,   646,
       0,   647,     0,   727,   722,   729,     0,   696,   683,     0,
     677,     0,     0,     0,   999,   979,     0,   976,     0,     0,
       0,     0,     0,     0,     0,   570,     0,   426,   440,   436,
       0,     0,   164,   529,     0,     0,   164,     0,   724,     0,
     505,     0,   562,   506,   692,   843,   547,   545,   692,   548,
     234,     0,     0,     0,     0,   771,     0,   773,     0,    98,
       0,     0,     0,   747,   482,   480,   499,   497,   498,   485,
     486,   487,   489,   490,   491,   492,   493,   495,   496,   494,
     488,   484,     0,     0,   860,   841,     0,   858,   279,   285,
       0,   272,     0,   214,     0,   406,   405,     0,     0,     0,
       0,   884,   886,   360,     0,     0,     0,   874,     0,   657,
       0,     0,     0,     0,     0,     0,     0,   690,     0,   721,
     648,     0,     0,   676,   669,     0,     0,   980,     0,   985,
     986,     0,     0,     0,     0,   950,   563,   572,     0,   693,
     526,   164,     0,   432,     0,   442,   441,   523,   533,   499,
     596,   597,   586,   587,   588,   590,   591,   592,   593,   594,
     595,   589,   585,     0,     0,     0,   555,   553,   570,   692,
       0,   549,   550,     0,     0,   565,     0,     0,   325,     0,
     324,     0,     0,     0,   692,   238,   864,   862,     0,   275,
     219,   220,   403,   407,   402,   412,     0,   875,   877,   876,
     364,     0,     0,   641,   695,     0,     0,   715,   695,     0,
       0,   710,     0,     0,   701,   920,     0,     0,     0,   977,
       0,     0,     0,     0,   562,     0,     0,     0,     0,   442,
     437,   438,   435,   692,     0,   692,     0,     0,     0,   554,
     572,     0,   543,   564,     0,   567,   772,   774,   110,     0,
       0,     0,   477,   500,   276,   413,     0,     0,   692,     0,
     697,   695,   692,     0,     0,   728,   702,     0,     0,   981,
     990,   991,   995,   996,   573,     0,     0,     0,     0,     0,
     692,     0,     0,   443,   422,   444,   534,     0,   562,     0,
       0,     0,   543,   544,     0,   566,   112,     0,     0,   478,
       0,   668,   673,   698,   692,   695,     0,     0,     0,   562,
       0,   537,     0,     0,   423,   439,     0,   536,     0,   579,
     576,     0,   575,   562,     0,     0,   540,   111,     0,     0,
     470,   692,     0,   686,     0,   925,     0,   571,   569,     0,
     538,     0,   424,     0,   580,   574,   577,     0,   541,   113,
       0,   471,   687,     0,     0,     0,   425,   581,     0,   472,
       0,     0,   539,   568,     0,     0,   923,   922,     0,     0,
       0,     0,     0,     0,     0,   921
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,    17,    18,    19,    31,    32,    33,    34,
      35,    36,    37,    62,    88,   484,   143,  1448,  1449,   182,
     313,   441,   547,   804,   442,   807,   481,   727,    38,   178,
    1216,   283,   696,    39,   140,   707,   288,   179,   289,   468,
     968,   704,   971,  1261,  1262,    40,    64,    90,   345,   146,
     195,    41,    42,   154,    68,   109,   443,   111,   112,   113,
     444,   859,   388,   158,   165,   235,   445,   116,   117,   373,
     374,   375,   446,   770,   447,   298,   448,  1060,  1663,   449,
     120,   450,   451,   489,   513,   490,   491,  1283,   452,   524,
     799,   800,  1333,   453,   150,   215,   795,  1301,   748,  1012,
    1013,   756,   454,   500,  1031,  1304,  1305,  1504,   455,   456,
     499,  1027,  1028,  1029,  1030,   548,   457,   700,   701,   702,
     960,  1252,   961,   962,   963,   458,   341,   342,   343,   459,
     460,   461,   347,   462,   389,   644,   645,   646,   890,   376,
     589,   590,   377,   516,   808,  1074,   379,  1114,   854,  1356,
     380,   381,   382,   809,   810,   553,   811,   383,   585,   586,
     384,   385,   556,   463,   352,   791,   812,   358,   813,   827,
     771,   772,   773,  1064,   464,   775,   776,  1616,   931,  1854,
    1222,   932,   996,  1450,  1601,  1602,  1803,  1716,   933,  1276,
    1900,   729,  1870,   997,  1274,  1489,  1651,  1754,   934,   935,
    1237,   936,   937,   938,   939,  1452,  1453,  1604,  1605,   940,
    1848,   941,  1240,  1864,  1466,  1618,  1241,  1461,   942,  1619,
     944,  1595,   945,   946,  1707,  1796,   947,  1734,   948,  1454,
    1889,   949,  1606,  1732,  1577,   612,   613,   614,   615,   616,
     617,   618,  1137,  1139,  1138,  1130,  1133,  1131,  1128,   619,
    1169,  1170,  1171,   620,  1118,   621,   622,  1151,  1152,   623,
    1122,  1123,   624,   625,  1125,  1126,   626,  1147,  1148,   627,
     628,   629,  1687,  1780,   630,   631,  1419,   632,  1560,   633,
     884,   634,  1400,   635,  1395,   636,   952,   638,   639,  1175,
    1176,  1177,   299,   300,   301,   302,   985,   303,   304,   305,
     988,  1005,   306,   307,   978,   720,   750,   783,   784,  1624,
    1625,  1626,  1627,  1149,  1040,  1041,  1042,   759,  1049,   760,
    1056,   761,   571,   572,   840,   573,   845,   574,   575,   576,
    1098,   577,   578,   579,   355,   494,   495,    95,   496,  1019,
    1499,  1496,   497,   581,  1678,   582,   583,  1345,   830,   831,
     832,     6,     7,     8,    44,    13,    14,    15,    59,    78,
      79,    80,    81,   126,   251,    82,    83,   281,   282,    84,
     427,   267,   268,   269,   277,   687,   688,   259,   260,   668,
     262,   402,   403,   683,   404,   405,   406,   271,   272,   407,
     408
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -1531
static const yytype_int16 yypact[] =
{
     747,     1, -1531,   620,   250, -1531, -1531,   675, -1531,   190,
     341, -1531, -1531,   364,   326, -1531, -1531,   625, -1531,   980,
   -1531, -1531, -1531,   401, -1531,   524,   607,   671,   686,   145,
     743, -1531, -1531, -1531, -1531, -1531,   858, -1531, -1531, -1531,
   -1531,   942, -1531, -1531, -1531,   341, -1531,   866,   960, -1531,
   -1531,   868, -1531, -1531,   849,   881, -1531,   934, -1531,  1700,
     946,   979,  1138, -1531,  1145,   105, -1531, -1531,  1069,  1024,
    1075,  1100,  1132,  1147,  1167,  1178,  1179,  1182, -1531, -1531,
   -1531, -1531,  1568, -1531, -1531,  1190,  1256,   159, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  1007,  1185,  1261,  1185,
     719, -1531,  1204, -1531, -1531,  1208,  1214, -1531, -1531, -1531,
   -1531,   160,  1360, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
    1331, -1531, -1531, -1531, -1531, -1531,   962,  1243,  1252,  1271,
    1277,  1307,  1305,  1311,  1308, -1531, -1531, -1531, -1531, -1531,
   -1531,  1319,  1327, -1531,  1434, -1531,  3315, -1531, -1531, -1531,
    1349, -1531,   921,  1542,   638,   974, -1531, -1531,  1366,   109,
    1522,   169, -1531, -1531,   719,  1595,  1185,  1370, -1531,  1406,
    1038,  1108,   597,  1108,   656,   808,  1108,  1415, -1531,   834,
     432,   432,  3213, -1531,  1422,  1444,   134,  1555,   134,  1185,
    1456,  1462,  1185, -1531, -1531, -1531, -1531,  1454, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531,   743,  1604,   743,  1473, -1531,  1465,
   -1531,   743,   432,  1464,  1497,   743,   164, -1531,    88, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  1191,   765, -1531, -1531,
    1538,  1484, -1531, -1531, -1531,  1543,  1479,  1108,   918, -1531,
   -1531,   899, -1531, -1531, -1531,   597,   783,  1537,   347, -1531,
     435,  1504, -1531,   307,  1517,  1525,   117,  1570,   499,   982,
    1575,    13, -1531,  3069,  1571,  1581,  1589, -1531, -1531,  1708,
   -1531, -1531,  1185,  1185,  1185,  1185,  1185,  1185,  1213,   710,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,   949, -1531,
     161,   134,  1118, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  1550, -1531, -1531, -1531,
   -1531,  1759,  1222, -1531, -1531,   792,  1235,  1683,  1244,  1728,
    1729,  1258, -1531,  1653,   293, -1531,  1127,    68, -1531,   386,
   -1531, -1531,  1596,  1045,  3389,  1590,  1598,   743,  1593,   132,
     853,  1185,   795,  1599, -1531, -1531, -1531, -1531,  1733, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,   743,   432,  2819,  1616,
     743,   622,  1588,  1600, -1531,  1601,  1108,   771,   745,  1648,
     928, -1531,   217, -1531,   317,  1606,  1609,  1613, -1531,  1108,
   -1531,  1108,  1108,  1049,   812, -1531,   597, -1531,  1626,   597,
    1628,   597,  1196,   597, -1531, -1531,   197, -1531, -1531,  1620,
    1015,  1716, -1531,  1035, -1531,  1108,  1630,  1415,  1629, -1531,
     139, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1623, -1531,  1634,
      57,  1264,  1267,  1302,  1361,  1364,  1379,   254,   432,  1631,
    1636,   439,  1651, -1531,  1638,  1735, -1531,   388,   572,  1118,
    1127,  1639,    78,  1155,  1632,  1635, -1531, -1531,  1660,  1779,
     702,  2582,  1664,  1665,  1655, -1531,   743,  2582,   743,   126,
     743,   743,  2582, -1531,   463,  1726, -1531,  1653,   330,  1650,
    2582, -1531, -1531, -1531,  1785, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531,   800,  1750,   915,
   -1531, -1531,   853,  1792,  1127,  2582,  1793,   759,   743,   759,
   -1531, -1531,  1678,  1680,  1681, -1531, -1531, -1531,  2349, -1531,
     709,  1203,  1224,   399, -1531,  1762, -1531, -1531,  1650,  1219,
     566, -1531, -1531, -1531,  1393,   133, -1531, -1531, -1531,  1171,
   -1531, -1531,  1682,   849,  1077,  2582,  1293,  1677,  1673, -1531,
   -1531,  1674,  1675,  1684,  1687,  1688,  1689,  1690,  1698,  2349,
     270, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531,  1694, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
     871,   494, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
     823,  1274, -1531,  1695,  1070, -1531, -1531,   384,  1038,  1685,
    1703,  1732,  1734,   674, -1531,  1749, -1531,  1702,   945,   918,
   -1531,  1081,  1196, -1531,   659,   659,  1124,  1124, -1531, -1531,
   -1531, -1531,  1026,   597, -1531,   531,   597,  1504, -1531,  1699,
    1705, -1531, -1531,  1254, -1531, -1531, -1531,  1710,  1686, -1531,
    1015, -1531,  1754, -1531, -1531, -1531,  1746, -1531, -1531, -1531,
     787,  1712,  1709,  2648,  1814,  1730, -1531,  1717,   173,   743,
     254,   254,   789,   743, -1531, -1531, -1531, -1531, -1531,  1891,
     743, -1531, -1531, -1531,   161,   240, -1531, -1531, -1531,   110,
   -1531, -1531, -1531, -1531,   207,   823, -1531, -1531,  2582,   743,
    1812,   533,  1127,  1723,  1724, -1531,  1127,   470,  1722,   420,
   -1531,   260, -1531,  1738,    86,   809, -1531,  1737,  1739,   831,
    1328,  1592, -1531, -1531, -1531,   849,  1740,   540,  1741,  1185,
    1747, -1531, -1531, -1531,   509,  1751, -1531,   569,  1752, -1531,
   -1531, -1531, -1531,  1334, -1531,  1827, -1531,   766,   289,   345,
    2582, -1531,  1726, -1531, -1531,  1755,  1722,  1756,  2582,  1887,
    1758,  3389,  1760, -1531, -1531, -1531, -1531,  1382, -1531,    32,
   -1531,  1432, -1531, -1531,   460,   743,  1650,  1742,  1127, -1531,
   -1531,  1123, -1531, -1531, -1531, -1531, -1531, -1531,  1761,   699,
    1743,   501, -1531, -1531, -1531,   853,  1224, -1531, -1531, -1531,
    1203, -1531, -1531, -1531, -1531,  1203,   759,   510,   743,  1861,
   -1531,   795, -1531,  1763, -1531,  1865,   667, -1531, -1531,   906,
   -1531,   877,   877,  2582,  2582,  2582,   877,   877,   877, -1531,
   -1531,  2582,  2245,   877,  2582,   537,  1778,  2582,   560,  2582,
    2382,   162,  2582,   381,  1909,  1770,   301,  1650, -1531,   438,
    1764, -1531,  1616,  2582, -1531,  1757, -1531, -1531,   940,  1784,
   -1531, -1531,  1819, -1531, -1531,   348,  1108,   731,  1287,   659,
    1613, -1531, -1531,   512,   546,  1196,  1196,  1230,  1239, -1531,
    1015,  1015,  1710,  1774,    81,  2582,    93,  1677,  2582, -1531,
   -1531, -1531,   119, -1531, -1531, -1531, -1531, -1531, -1531,  1789,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1932,
     346,  1637, -1531,  1858,   882,  1281,  1795,  1930,  1931,   813,
    1794, -1531,  1912, -1531,  1127,  1807,   224,   527,  1788, -1531,
     231,  1241, -1531, -1531, -1531,   743,  1812, -1531,  1791,  1891,
     743,  1812,   743, -1531, -1531,   743, -1531, -1531,  1812,  1891,
    1799, -1531, -1531, -1531, -1531,  1127, -1531, -1531,   242, -1531,
   -1531, -1531,  1801,   890,  2582,   826,  1812,  1803, -1531, -1531,
    1650,  1127,   303,  1796,  1650,   260,  1797,  1798,  1800, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1954,  1808,  1804,
    1650,  1957,   877, -1531,  2582,  2582,  2582,  2582,  2582,  2582,
    1992,  1940,  1908, -1531, -1531, -1531, -1531, -1531, -1531,   853,
   -1531, -1531, -1531, -1531, -1531, -1531,   853, -1531, -1531, -1531,
    1812,   853,  1946,  1409,   156, -1531,  1127, -1531,  2582,  1877,
   -1531,   463, -1531,  2521,   853,  1879, -1531, -1531, -1531, -1531,
     612, -1531,  1750, -1531,  1541,  1820,  1952, -1531,   166,   853,
   -1531,  2169,   124, -1531, -1531,  1650,  1816, -1531, -1531,  1034,
   -1531,  2349,  2582,  2624, -1531,   399, -1531, -1531,  2349, -1531,
    1825,  1840, -1531, -1531,   171,   743,  2582,  1842,  1831, -1531,
    1832,  1828,  1527, -1531,  1836,  1532, -1531,  1830,  1837,  1552,
    1838,  1841,  1843,  1845,  1848, -1531,  1847,  1849,  1850,  1851,
    1852,  1833, -1531,  2582, -1531, -1531,  2450,  1126, -1531,  2027,
    1556,  1557, -1531,  1994,  2582,  1860, -1531,  2010,  1968,  2582,
    1867, -1531,   150, -1531,  1868,  2582,  1127,  2582, -1531,   108,
     205,  1869, -1531,  1127,  2582,  1870,   354, -1531,   310, -1531,
   -1531,  2582,  2245,  1961,   853, -1531, -1531,  1872,  1873, -1531,
    1918,  1889,  1892,  1898,   802,  1326,  1981,  1903,  1904,  1324,
    1469, -1531,  1888,  1890, -1531, -1531, -1531,  1893,  1894, -1531,
   -1531,  1686, -1531,  1938,  1910, -1531,  1899,  1998, -1531, -1531,
    2012,  2582,  1932,   159,   996, -1531,  2582,   359, -1531,  1127,
    1127,  1127,  1919,  2582,   172,  1770, -1531,   530, -1531, -1531,
   -1531,  2058,  1978,   236,  1289,   853,    85,  1901,  1902,  1127,
    1127, -1531,   530, -1531, -1531,  1650, -1531, -1531,  2068,   142,
   -1531, -1531, -1531,  1812, -1531,   743,  1812,  1891, -1531,  1812,
    1812, -1531,  1812, -1531,  1560,  1650, -1531, -1531, -1531, -1531,
   -1531,   853,   853,  1911,  1913, -1531, -1531, -1531,  1973, -1531,
    1650,  1127,  1905, -1531, -1531, -1531,   853,  2582,  2582,  3389,
    1063, -1531, -1531, -1531,  2079,   159,  1914, -1531, -1531, -1531,
   -1531, -1531, -1531,  2582,  2582,  2582, -1531, -1531,  1920,   582,
     743,   743,  2033,  1411,   496,  1996,  2582, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531,  1127,  1650, -1531, -1531, -1531, -1531,
     743,  2008, -1531, -1531, -1531,  1565, -1531,   549,  1921,   709,
   -1531,  1922, -1531, -1531,  2053,  1935, -1531,   849,  2044, -1531,
   -1531, -1531,  2582,   877,  1941,  2582,   877,  1945,  1949,  1950,
    2678,  1951,  1953,  2582,  2582,  1958,  2582,  1959,  1960,   877,
    1964,   877, -1531,  2063,  1141, -1531,  2582,  2582,  2582,  1966,
     877,  1967,  2048,  1971, -1531,  2064,   853, -1531,  1972, -1531,
    2070,  1991, -1531, -1531, -1531,  1974, -1531, -1531,  1983, -1531,
     143,  1986, -1531,   203, -1531, -1531,   359,  1987,  2582,  2845,
    1988,  2450,  1144,   853, -1531, -1531,  1975,  1982, -1531,   597,
    1388,  1479,  1105, -1531, -1531,  1196,  1196,  1196,  1196,  1196,
    1196,  2000, -1531, -1531, -1531,   877,  1970,   159,   661,  2111,
   -1531,  1040, -1531, -1531, -1531,  2092,   149,  1039,  1650,  1650,
    2142, -1531,  2097, -1531, -1531,  1999,  2113, -1531,   853,   547,
    1653,  2069,  2521,  2521,  1650,  1650, -1531,  2001,  1058,   850,
   -1531, -1531, -1531,  1812, -1531, -1531, -1531,  1127,  1932,  3291,
   -1531,  2002, -1531, -1531,  1650, -1531,  1566,  2009, -1531,  1580,
    2016, -1531,  1650,  2171, -1531,  1434, -1531, -1531, -1531, -1531,
   -1531,  2082,  2139, -1531,  2020,  2035,   743,  2036,  2582,  2096,
   -1531, -1531,  2349, -1531,  2582, -1531, -1531,  2038,  1439,  2039,
    2021, -1531, -1531,  2022, -1531, -1531, -1531, -1531,    26, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  2023, -1531, -1531,  2624,
     676, -1531,  2582, -1531,  2489, -1531,  2582,  2026, -1531, -1531,
    2030, -1531,  1127, -1531, -1531, -1531,  2172, -1531, -1531,  1149,
   -1531,  2034,  2077,  2051,  1609,  1479,  1108,   817,  1335,  1345,
    1510,  1519,  1528,  1549,  2050,  2420,  2045, -1531, -1531,  1912,
    2041,  1912,  3081, -1531,  2042,  2450,  3362,  2134,  2145,  2052,
   -1531,   853, -1531, -1531,  1896,   404, -1531, -1531,  1855, -1531,
    1653,  2124,  2054,   853,  1583, -1531,  2147, -1531,  1586, -1531,
    2215,  2220,  2059, -1531,  1650, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531,   853,  2582, -1531, -1531,  2582, -1531, -1531, -1531,
     842, -1531,   820, -1531,   743, -1531, -1531,  2062,  2129,  2126,
    2582, -1531, -1531, -1531,  2080,  2084,  2086, -1531,  2073, -1531,
    2093,  2095,  2083,   568,  2248,  2624,  2081,  1320,  2085, -1531,
   -1531,   430,   879, -1531, -1531,    25,  2076,   851,  1395, -1531,
   -1531,  1196,  1196,  1196,  1196, -1531, -1531,  1375,  2249,  2492,
     996,  3192,  2087,  1434,  2089, -1531, -1531, -1531,   383, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531,  2624,   682,  2090, -1531, -1531,  2420,  2225,
    2238, -1531, -1531,  2100,   853, -1531,  2102,  2521, -1531,  2521,
   -1531,  2103,  2244,  1255,  2877, -1531, -1531, -1531,  2105, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  2153, -1531, -1531, -1531,
   -1531,  2107,  2108, -1531, -1531,   888,   575, -1531, -1531,  2582,
    2258, -1531,  2582,  2115, -1531, -1531,  2130,  2132,  1440,  1479,
    1429,  1447,  1458,  1477, -1531,  2582,  2280,  2582,  2450, -1531,
   -1531,  1912, -1531,  2314,  2582,  2911,   586,  2289,  2624, -1531,
    1375,  2278,  2140, -1531,  2125, -1531, -1531, -1531, -1531,  2133,
    2288,  2294,  2253, -1531, -1531, -1531,  2154,  2156,  1962,  2141,
   -1531, -1531,  2965,  2250,  2290, -1531, -1531,  2136,  2138,  1479,
   -1531, -1531, -1531, -1531,  2420,  2240,  2256,  2097,  1154,   409,
    2314,  2144,  2322, -1531, -1531, -1531,  2323,  2227, -1531,   908,
     589,  2324,  2140, -1531,  2161, -1531, -1531,  2168,  2327, -1531,
    2291, -1531, -1531, -1531,  2746, -1531,   943,  2185,   798, -1531,
    2174, -1531,  2450,  2624, -1531, -1531,   947, -1531,   956, -1531,
    2420,  2175, -1531, -1531,  2271,  2178, -1531, -1531,  2181,  1019,
   -1531,  2716,  2182, -1531,  2166, -1531,  2228,  2420, -1531,   424,
    2184,  2186, -1531,  2187, -1531, -1531,  2420,  2199, -1531, -1531,
    2188, -1531, -1531,  2204,  2192,  2624, -1531, -1531,  2190, -1531,
    2183,  2236,  2184, -1531,  2203,   229, -1531, -1531,  2208,  2194,
    2217,  2197,  2219,  2200,  2224, -1531
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1531, -1531, -1531,  1906,    12, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531,  1701, -1531,   -79,   -84, -1531,
   -1531,   -62, -1531,  1323,   -63, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,  -940, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,   -59,  2261, -1531, -1531,
     -47, -1531, -1531,  -128, -1531, -1531,   -57, -1531, -1531, -1531,
   -1531, -1531,    43, -1531,   -55,   -49,    52, -1531, -1531,   -54,
   -1531,    76,  2230, -1531,  -332,  1933,  -345, -1531,   -58, -1531,
   -1531, -1531,  1092,   -52, -1531, -1531, -1531,   922,  -489, -1531,
    1416, -1531,  2281, -1531, -1531, -1531, -1531, -1531,  -131,  -142,
   -1531, -1531, -1531, -1531,  1131,  -743, -1531,  1173, -1531, -1531,
     955, -1531,  -927,   989,  -507,   255,  2131,  2255, -1531,  2259,
     -12,   557, -1531,  -163, -1531, -1531,  1553, -1531, -1531, -1531,
   -1531,  1597, -1531,  -141,  -338,  1874, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531,  1385,  -755,  -604, -1531, -1531, -1531,  1862,
   -1531, -1531, -1531,   -50,  2226,  1657,  1027,  -213,  -686,  -471,
   -1531, -1531, -1531, -1531,   578,  -473, -1531,  -676, -1531,   600,
   -1531, -1531,  -868,  1009, -1531,   856,   662,  -258,  1977, -1531,
   -1531, -1531, -1531, -1531,  1296, -1531, -1531,   746,  1989, -1531,
   -1531,  1995, -1531,  -850,  -362,   757,  -830, -1531, -1531,  -812,
   -1531, -1531, -1531,   606, -1531,   855, -1531, -1531, -1531,  -655,
   -1521, -1530, -1531, -1531,   735,   665, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531,  -364, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531,  1101,  1098,  -822,  1112,  1113,  -796, -1531, -1531,
    1321,  1025, -1048,  -355, -1531, -1531, -1531, -1531,  1103, -1531,
   -1531,  1133, -1531, -1531, -1531,  1128, -1531, -1085,  1111, -1531,
    -352, -1531, -1531, -1531,  -639,  -422, -1329, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  -361, -1531, -1531, -1105,
    1275,  1087,   -72,  2028, -1531, -1531, -1531, -1531, -1531, -1531,
    -884,  -834, -1531, -1531, -1531,  -638,  -279, -1531,  1436,  1043,
     764,  -469, -1034,  1090, -1531, -1531, -1531,  -858, -1531,  1463,
   -1531,  -284, -1531,  1955, -1531,  1691, -1531,  1692,  -459, -1531,
    1421,  1969,  -257,   -30,  2498, -1531, -1531, -1531,  -342,  -673,
   -1531, -1531, -1531, -1531, -1531, -1531,   299, -1531, -1035, -1068,
    1427, -1531, -1531,  2525, -1531, -1531, -1531,  2512, -1531, -1531,
   -1531, -1531, -1531, -1531,  -177, -1531,  2454, -1531,  2101,  2457,
    2116, -1531,  1875,  2128, -1531,  1859,  1625,  -351,  -195,  -124,
   -1531, -1531,  1900,  -525,  1915,  -347,  -110,  2143,  2146,  1897,
    1926
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -905
static const yytype_int16 yytable[] =
{
      54,   353,   244,   145,   208,   108,   107,    43,   144,   110,
     121,   115,   353,   118,   119,   207,   122,   969,   124,   333,
     930,   114,   515,   242,   611,   465,   610,   637,   580,    24,
     796,  1260,   569,  1347,   241,  1254,   228,   290,   778,  1329,
     328,   943,  1146,   782,  1136,   785,  1140,   261,   152,   273,
     155,   327,   279,   658,  1087,   492,   123,   950,  1082,   734,
     183,  1384,   270,   401,  1223,   278,  1346,  1127,  1564,  1134,
     392,  1417,   980,   982,  1706,   517,  1218,   705,  1020,   689,
     522,  1225,  1738,   194,   193,   378,   570,   196,   204,   198,
    1214,   200,   202,  1022,   205,  1266,   212,  1422,   819,   519,
     822,  1238,   232,   231,   595,  1272,   233,   239,   236,   308,
     237,   238,    91,   240,  1219,   245,   287,   246,   234,   315,
     314,  1408,  1411,   316,   325,   318,   732,   320,   322,   733,
     326,   640,   334,   397,   210,   317,   768,   908,  1165,  1239,
     348,   338,  1264,   351,   743,   744,   697,  1268,  1023,   697,
     363,   223,   994,   243,  1271,   414,   740,  1016,  -392,   849,
      51,   994,   746,   991,  1062,   386,   520,   291,   367,  1284,
     331,  -392,  1288,  1165,   368,     9,  1307,  1308,  1309,  1310,
    1311,  1312,  1322,   595,   354,   792,   359,   928,   482,   199,
     554,   359,  1165,   369,   141,   354,   339,  1354,   201,   769,
     823,   698,   162,  1785,   698,   401,    53,   437,  1166,   438,
     580,   226,   975,   430,  1471,  1017,   752,  1706,   595,  1786,
    1682,  1166,   203,   424,   735,   319,  1318,   142,  1086,  1401,
     787,   706,   492,   492,   321,  1165,   370,   371,  1341,   794,
     986,  1174,  1277,   471,   472,   473,   474,   475,   476,  1167,
      16,   595,   774,   595,  1024,  1215,   521,   788,   323,  1018,
    1025,   486,  1026,   488,  1844,  -391,   928,    53,   570,   886,
     714,  -507,   653,    92,   951,   747,   998,   999,  -391,    93,
    1108,    94,   493,   387,   829,   666,  1185,   667,   609,   670,
     414,  1572,  1167,   715,   486,   716,  1407,   816,   224,   717,
      53,   781,   670,   370,  -507,   995,   672,   151,   340,   675,
     991,   670,  1277,   699,  1221,   594,  1478,   718,  -392,    52,
    -392,   431,   584,  1706,   518,   829,   493,   555,  1890,   518,
     151,  1569,  1016,   424,   252,   483,  1579,   359,  1402,   689,
     486,   732,  1294,  1416,   733,  1355,    53,    53,   163,  1907,
    1168,  1609,  1421,   425,  1447,   781,   593,   227,   641,   372,
     359,   580,  1570,  1916,  1226,   370,    65,   609,   871,  1706,
     992,   253,   254,   993,   887,   486,   255,   793,    21,  1000,
    1011,    53,  1001,  1483,  1463,  -775,  1706,  1107,   256,  1173,
    1199,  1200,   257,  1409,   258,  1706,   781,  1007,   823,  1181,
     747,   209,   370,  1342,   659,    53,  1418,  1937,   486,  1006,
     486,  1173,   953,   661,    53,   660,    53,   370,  -524,   570,
    -102,   512,  1464,  1938,   888,  -391,   514,  -391,    53,  1480,
     847,    65,  1482,   747,  1018,  1484,  1485,   329,  1486,   954,
     741,   409,  1334,   418,   661,  1828,  1174,   359,   841,  1832,
     595,   493,   872,   292,  1347,  1508,  1509,  1510,   370,   493,
     493,   293,  1227,   425,   401,   724,   557,    53,   735,   401,
     401,   895,  -843,  1174,   842,  1804,   765,   735,   359,   493,
     777,   777,   893,  1182,   789,   492,   747,  1671,   514,  1010,
    1014,  1683,  1016,    65,   411,   412,   426,  1291,  1069,  1121,
    1124,  1883,  1874,  1735,  1135,  1135,  1135,   747,   725,   726,
    1016,  1150,   662,   294,   558,   663,  1925,   559,    23,    53,
    1718,   295,   814,   780,   493,   514,  1782,   486,   821,  -904,
      65,   420,  1091,    53,   419,  1092,   584,   907,   789,  1631,
     514,  1302,  1097,   662,   847,    65,  1901,  1011,  1416,   486,
    1017,  1104,    22,   560,   296,    53,  1501,  1058,   297,  1140,
      53,  1095,   658,   675,  1087,   957,   914,  1183,  1017,   689,
    1212,   992,   894,   757,   993,    45,  -843,  1386,  -340,   789,
    1134,   514,  1016,  -843,   885,  -843,    65,  -843,  -843,  -847,
     843,   844,    11,   869,  1018,   433,  1000,  1102,   958,  1001,
     782,   493,   785,  1386,   887,   887,   151,   486,  1197,   887,
     887,   887,  1018,   608,  1059,  -847,   887,  1776,  1386,  1016,
    1635,  1603,   421,  1517,  1066,    -3,  1178,   747,   714,  1154,
     422,   486,   561,   562,    53,   563,   564,    53,   486,   565,
    1017,  -847,  1198,  1278,    53,  1524,  1279,   566,   567,  1633,
    -847,   715,  1159,   716,  -391,   514,  -391,   717,   568,   847,
      65,   512,   486,  -847,  1774,  1806,   955,  1242,    53,   640,
    1306,  1831,  1712,   970,  1714,   718,  1621,  1017,   359,   359,
     359,   359,  1858,   777,  1018,  1893,   421,   642,  1103,   609,
     359,  1092,   972,   747,  1243,   493,   957,  1068,    47,   419,
     512,   825,  1684,   211,   493,  1108,   747,  1255,  1807,   359,
    1563,  1155,   493,  1849,   401,  1817,   493,   493,   421,   580,
    1063,  1018,   833,  -103,   213,  1156,   422,   217,  -264,   142,
    1465,  1120,   833,   421,  1160,   263,  1103,  -264,  1275,   332,
    1860,   735,  1108,  1116,  -264,  1476,  1117,   580,  1161,   580,
    -847,  -847,  -847,   569,  1290,  1103,    53,  -847,  -847,   518,
     335,   739,  1103,  -847,   514,  -847,   217,    24,  1685,    65,
    -264,   264,  1317,  1103,  1808,   887,  1103,   570,  1512,  1088,
     486,    48,  1194,   834,   518,   359,    53,   787,   493,   833,
    1331,  1099,   265,   834,    10,  -889,   252,  1909,   266,  1050,
    1051,  1052,  1053,  1054,  1055,   570,     1,   570,   409,  1324,
     558,    11,   219,  1821,   788,  1910,   648,   829,   359,  1349,
    1097,    12,   957,  1335,   829,   649,   220,   950,   983,   274,
       3,   493,   493,  1249,   398,  -265,   493,   493,   493,  1034,
    1250,   275,   580,   493,  -265,    49,   284,  1513,  1514,   560,
     834,  -265,   984,   493,   909,   958,   557,  1932,   781,  1760,
      50,   411,   412,   435,   285,   409,  1050,  1051,  1052,  1053,
    1054,  1055,   900,  1251,  1851,   959,  1630,  -265,     1,  1278,
     285,  1035,  1279,  1761,  1036,   957,  -889,   156,   781,   874,
     781,   286,  1037,   157,     2,   781,   493,  1038,   478,   875,
     570,   876,     3,   391,   558,   409,   479,   559,   877,  1275,
    1588,  1589,  1590,  1591,  1592,  1593,  1413,    53,   958,  1195,
     435,  1244,    30,   878,    60,  1737,  1039,   486,   561,   562,
     642,   563,   564,    53,   493,   565,   409,   655,   959,   249,
     518,  1286,  1741,   560,   250,   359,   263,   879,   880,   656,
     359,   409,   359,  1287,   568,   359,  1708,   415,   411,   412,
     435,  1470,   881,  1742,   882,   493,   504,   654,   493,   587,
    1178,   588,  1457,  1458,  1459,   950,  1905,   640,   416,   950,
     505,   493,   264,   276,    58,   409,  1906,   883,  1004,    25,
    1430,   435,  1474,  1475,   801,   956,   802,  1490,  1491,   421,
      26,  1121,   493,   265,  1124,  1698,   435,    27,   671,   266,
    1281,   888,  1497,  1302,  1282,  1323,  1758,  1135,    61,  1135,
     747,   486,   561,   562,    28,   563,   564,    53,  1556,   565,
    1759,   814,  1451,   409,  1494,   410,   493,   566,   567,  1788,
     435,   789,    63,  1667,   951,   486,    56,    29,   568,    65,
     493,    53,    57,  1783,   580,   252,   263,   610,   637,   970,
    1143,  1144,  1829,  1741,  -524,   252,   263,  1784,    30,    66,
     888,   789,   609,   789,   156,    96,  1830,  1335,   789,   747,
     157,  1145,  1891,  1596,  1742,  1357,   411,   412,   413,   805,
      97,   806,   264,   398,  1119,    98,  1892,   399,    99,  1708,
     950,   100,   264,   398,  1143,  1144,   887,   101,    67,   887,
     216,   732,   570,   400,   733,   217,   409,  1902,  1189,   266,
      85,  1911,   887,   400,   887,  1145,   102,  1853,  1190,   266,
    1913,  1903,   664,   887,  1191,  1912,   493,   478,  1622,  1581,
     103,   903,   167,   493,  1914,   480,  1109,   434,  1855,   104,
     168,   105,   252,    86,   970,   106,   169,    30,  -223,  1178,
     642,   642,   640,   221,   950,   642,   642,   642,   217,   411,
     412,   435,   642,  1709,  1853,   147,  1790,  1791,  1792,  1793,
      87,   148,   953,   149,  1620,   669,   252,    89,   887,   253,
     254,  1762,   951,  1920,   255,  1855,   951,   493,   125,   493,
     493,   493,   610,   637,   493,  1708,   557,  1921,  1617,   954,
     691,   950,   249,   421,   692,   788,   788,   250,   252,   493,
     493,  1506,   912,   253,   254,   642,  1505,  1610,   255,   514,
    1634,  1024,   138,   478,    65,   359,   747,  1025,   829,  1026,
     256,   525,   252,   357,   257,   252,   258,  -315,   362,   127,
     956,  1708,  -315,   703,   558,   253,   254,   559,   891,   732,
     255,   493,   733,   732,   892,   478,   733,  1258,  1708,   580,
     906,  1259,   256,   855,   128,  1349,   257,  1708,   258,   253,
     254,  1820,   253,   254,   255,   285,   486,   255,  1743,  1088,
     777,   777,   487,   560,  1586,   486,   256,    53,   139,   256,
     257,    53,   258,   257,   493,   258,   129,   640,  1587,   781,
     359,   953,   412,   435,  1385,  1691,  1709,   951,   514,  1584,
    1386,   130,  1096,    65,   857,   858,  1649,   570,   153,  1551,
     953,   642,  1580,   493,   953,  1386,   493,  1693,  1386,  1746,
     678,   131,  1881,  1386,  1778,  1779,   781,  1647,  1882,   493,
     514,   493,   132,   133,   749,    65,   134,   954,  1646,   151,
     493,   954,   679,   680,  1599,   851,   970,   852,  1755,   681,
     682,   486,   561,   562,  1201,   563,   564,    53,   159,   565,
     390,   951,   160,  1206,   732,   217,   493,   733,   161,   641,
    1823,   164,   610,   637,   551,  1109,  1202,  1203,   568,  1794,
    1795,  1349,   477,  1204,  1205,  1207,  1208,   217,   837,   838,
     839,   502,  1209,  1210,   514,   493,   503,   647,   847,    65,
     166,  1661,  1709,   170,   506,  1242,  1638,  1637,   951,   503,
    1639,  1644,  1641,   508,  1642,  1643,   955,  1645,   217,  1650,
     171,  1823,  1640,   610,   637,   917,   918,   511,   732,  1349,
     919,   733,   217,   708,   953,   953,   709,   493,   217,   172,
    1814,   217,  1697,   252,  1729,   173,   610,   637,  1709,   514,
     610,   637,   781,   889,    65,  1728,   514,  1648,   917,   918,
    1246,    65,   954,  1196,   514,  1709,   777,   174,  1469,    65,
     788,   710,   789,   175,  1709,   732,   217,   640,   733,   176,
     253,   254,   142,   177,   719,   255,  1043,  1044,  1045,  1046,
    1047,  1048,   610,   637,   180,   917,   918,  1431,  1435,   953,
     781,   257,   181,   258,  1349,   252,   917,   918,  1071,   789,
    1072,  1699,   252,   642,   641,   767,   917,   918,   214,   610,
     637,  1700,   493,  1721,  1720,  1109,   954,  1722,   640,  1724,
     711,  1725,  1726,   712,  1727,   217,  1731,   218,   217,  1723,
     953,   222,   253,   254,   225,   955,   953,   255,   713,   253,
     254,   640,   247,   217,   255,   640,  1084,   252,  1085,  1585,
     248,   970,   848,   257,   955,   258,  1789,   217,   955,   280,
     257,   758,   258,   954,  1730,   781,   336,   766,  1321,  1349,
    1516,    96,   779,   217,   786,   217,   953,   184,   229,  1674,
     797,  1675,  1676,  1677,   253,   254,    97,   640,   337,   255,
     917,   918,   344,   953,    99,  1840,  1089,   100,  1090,  1801,
     349,  1839,   953,   101,   777,   257,   350,   258,   917,   918,
     557,  1349,   163,  1841,   640,   817,   356,   360,   595,   917,
     918,  1228,   102,   361,  1842,   789,  1229,  1230,   828,   364,
     917,   918,   642,  1436,  1231,   642,   103,  1232,   917,   918,
     781,   365,   394,  1843,  1233,   104,   396,   105,   642,  1244,
     642,   106,   393,    30,  -223,   856,   417,   395,   558,   642,
     423,   559,  1050,  1051,  1052,  1053,  1054,  1055,   429,   870,
     135,   917,   918,   789,  1701,   428,  1234,   999,   955,   955,
     917,   918,   781,  1702,    71,  1337,    72,  1338,   642,   917,
     918,  1363,  1703,  1364,   641,    73,  1366,  1235,  1367,   436,
      75,   928,    76,   432,   470,   976,   977,   979,   981,   498,
     917,   918,    77,  1704,   642,   466,  1370,   989,  1371,   557,
    1388,  1390,  1389,  1391,  1487,   467,  1488,   595,  1236,  1522,
    1653,  1523,  1654,   469,   501,   956,  1003,   507,  1109,   230,
     509,   510,   924,   955,  1656,   641,  1657,  1747,   789,  1748,
    1747,   512,  1750,   925,   523,   549,   550,   591,   552,   592,
     643,   650,   657,   664,   651,   652,   421,   558,   641,   665,
     559,   673,   641,   676,   693,   486,   561,   562,   686,   563,
     564,    53,   690,   565,   955,   926,   726,   695,   703,   722,
     955,   566,   567,   224,   723,   736,   737,   745,  1002,   738,
     514,   751,   568,    69,   753,    70,   927,   754,   762,   763,
     928,   790,  1093,   764,   641,   798,    71,   747,    72,   803,
     815,   818,   823,   789,   824,   825,   853,    73,   557,   846,
     955,    74,    75,   642,    76,   860,   595,   929,   861,   862,
     863,   641,   921,   896,    77,  1110,   898,   955,   899,   864,
    1075,  1740,   865,   866,   867,   868,   955,   869,  1079,   873,
    -346,   897,   925,   901,   915,   789,   902,   920,   923,   557,
     916,   964,    30,   965,   973,   974,   558,   595,   987,   559,
    1004,  1008,  1009,   956,   486,   561,   562,   956,   563,   564,
     608,  1015,   565,  1073,   926,   726,  1021,  1033,  1057,  1080,
     566,   567,  1032,   925,  1111,  1065,  1061,  1101,  1094,  1067,
    1070,   568,  1115,  1077,  1078,   927,  1081,   558,  1083,   928,
     559,  1113,  1157,  1129,  1132,  1132,  1179,  1100,  1180,  1184,
    1188,  1141,  1192,  1193,  1153,   926,   726,  1158,  1213,  1162,
    1164,  1224,  1172,   595,   994,  1245,   929,  -525,  1247,  1248,
     958,  1256,  1253,  1187,  1257,  1265,   927,  1273,  -699,  1280,
     928,  1289,  1295,  1296,  1293,  1297,  1298,  1299,  1300,  1303,
    1313,  1314,  1263,  1315,  1320,  1326,  1332,  1267,  1339,  1269,
    1340,  1108,  1270,  1352,  1353,  1217,  1359,   929,  1220,  1360,
    1361,  1382,  1362,   486,   561,   562,  1368,   563,   564,   608,
    1365,   565,  1387,  1369,  1372,  1373,  1392,  1374,   956,   566,
     567,  1375,  1376,  1377,  1379,  1378,  1381,  1380,  1394,  1396,
     568,  1397,   597,   642,  -699,  1399,  1403,  1412,  1415,  1423,
    1424,  1425,  1426,  1427,   486,   561,   562,  1428,   563,   564,
     608,   598,   565,   599,   600,   601,  1429,  1432,  1433,  1434,
     566,   567,  1441,  1437,  1442,  1438,  1444,  1443,  1439,  1440,
    1445,   568,  1467,  1460,  1285,  1468,  1472,  1473,  1477,  1492,
    1287,  1493,   956,  1495,   642,  1503,  1515,  1343,  1511,  1527,
    1507,  1518,   602,   603,   604,  1528,   557,  1525,  1526,    96,
     605,   606,   607,  1529,   595,   184,  1614,   642,  1550,  1532,
     486,   642,   185,  1535,    97,   186,   608,  1536,  1537,  1540,
    1559,  1541,    99,   440,  -164,   100,  1543,  1545,  1546,   956,
     925,   101,  1548,   188,  1555,  1558,  1562,   609,  1325,  1561,
    1565,   786,  1566,  1330,   558,  1567,  1597,   559,  1487,  1582,
     102,  1568,   557,   642,  1571,  1574,  1578,  1583,  1594,  -434,
    1607,  1611,   926,   726,   103,  1612,  1623,  1613,  1662,  1629,
    1660,   870,  1348,   104,   189,   105,  1652,  1664,  1351,   106,
     642,    30,  -223,   927,  1658,  1655,  1358,   928,  1665,  1666,
    1668,  1670,  1673,  1679,  1689,  1680,  1681,  1388,  1690,  1692,
     558,  1695,  1694,   559,  1696,  1705,  1733,  1710,   557,  1713,
    1717,   311,   190,  1383,   929,   312,   595,  1174,   191,   192,
    1736,  1744,  1745,  1749,  1393,  1751,  1752,  1753,   557,  1398,
    1763,  1811,  1764,  1765,  1767,  1404,  1773,  1406,  1768,   560,
    1769,  1770,   925,  1771,  1414,  1772,  1775,  1226,  1343,  1777,
    1787,  1420,  1812,  1781,  1819,  1800,   558,  1802,  1809,   559,
    1825,   486,   561,   562,  1834,   563,   564,  1615,  1813,   565,
    1815,  1818,  1481,  1824,   926,   726,   558,   566,   567,   559,
    1142,  1826,  1827,  1836,  1837,  1838,  1846,  1859,   568,  1143,
    1144,  1446,  1862,  1865,  1863,   927,  1455,   557,  1867,   928,
    1868,  1866,  1869,  1462,  1871,   595,  1872,  1876,  1879,  1873,
    1145,  1877,  1885,  1875,  1878,   560,  1886,   486,   561,   562,
    1852,   563,   564,    53,  1880,   565,   929,  1887,  1888,  1896,
    1894,   925,   557,   566,   567,   757,  1897,  1898,  1904,  1917,
    1923,  1899,  1908,  1915,   568,   558,  1918,  1521,   559,  1919,
    1922,  1103,  1924,  1928,  1926,  1927,  1929,  1930,  1933,  1934,
    1935,  1936,  1939,   926,   726,   557,  1931,  1498,  1500,  1940,
    1941,  1942,  1943,   486,   561,   562,  1944,   563,   564,   608,
     558,   565,  1945,   559,   927,  1336,   755,   197,   928,   566,
     567,   826,   324,   486,   561,   562,  1519,   563,   564,    53,
     568,   565,   742,   557,  1659,   990,  1520,   206,  1292,   566,
     567,  1502,  1479,   558,  1632,   929,   559,  1600,  -692,   560,
     568,   330,   485,   346,   835,  1186,  1319,   850,  1112,  1076,
    1884,   366,  1530,   557,  1711,  1533,  1598,   925,   728,   757,
    1539,  1850,  1405,  1132,  1539,  1805,  1132,  1798,  1895,  1739,
     730,   558,   560,  1810,   559,  1861,   731,  1553,  1554,  1549,
    1547,  1608,   486,   561,   562,  1542,   563,   564,   608,  1544,
     565,  1410,   757,  1557,  1534,   557,  1531,  1552,   566,   567,
     595,   558,  1456,  1573,   559,  1142,   721,  1327,  1575,   568,
     927,  1816,  1316,  -688,  -688,  -688,  1628,   486,   561,   562,
    1344,   563,   564,    53,   557,   565,   836,   820,    55,  1797,
    1350,  1105,    20,   566,   567,    46,   136,  1106,   694,   137,
     560,   929,   685,   558,   568,  1211,   559,   674,   913,   922,
     486,   561,   562,     0,   563,   564,    53,     0,   565,   904,
     757,   910,  1330,  1330,   677,     0,   566,   567,     0,   684,
    1163,     0,   558,     0,   905,   559,     0,   568,     0,   597,
       0,  1328,  1235,     0,     0,   557,     0,     0,   486,   561,
     562,   911,   563,   564,   608,     0,   565,     0,   598,     0,
     599,   600,   601,     0,   566,   567,     0,     0,  1669,     0,
       0,   560,   870,  1236,  1672,   568,     0,     0,   486,   561,
     562,     0,   563,   564,    53,     0,   565,   557,     0,     0,
       0,   757,     0,   558,   566,   567,   559,     0,     0,   602,
     603,   604,     0,     0,     0,   568,     0,   605,   606,   607,
       0,   557,  1686,     0,     0,     0,  1688,   486,     0,     0,
     486,   561,   562,   608,   563,   564,    53,     0,   565,     0,
       0,     0,   560,     0,     0,   558,   566,   567,   559,     0,
       0,   557,     0,     0,   609,     0,   826,   568,     0,   486,
     561,   562,   757,   563,   564,    53,     0,   565,     0,   558,
       0,     0,   559,     0,     0,   566,   567,     0,     0,     0,
       0,     0,     0,     0,   560,     0,   568,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   595,     0,   558,
       0,     0,   559,     0,     0,     0,     0,     0,   560,     0,
    -689,  -689,  -689,  1756,     0,     0,  1757,     0,     0,     0,
     486,   561,   562,     0,   563,   564,    53,   595,   565,     0,
    1766,     0,     0,     0,     0,     0,   566,   567,   560,     0,
       0,     0,  -700,     0,     0,     0,     0,   568,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   757,     0,
       0,     0,   486,   561,   562,     0,   563,   564,    53,     0,
     565,     0,     0,     0,     0,     0,   597,     0,   566,   567,
       0,     0,     0,     0,     0,     0,   486,   561,   966,   568,
     563,   967,    53,     0,   565,   598,     0,   599,   600,   601,
     595,     0,   566,   567,     0,     0,   597,  1330,  -700,  1330,
       0,     0,     0,   568,     0,   596,   486,   561,  1538,     0,
     563,   564,    53,     0,   565,   598,   595,   599,   600,   601,
       0,     0,   566,   567,     0,     0,   602,   603,   604,  1833,
       0,  1576,  1835,   568,   605,   606,   607,     0,     0,     0,
       0,     0,     0,     0,   486,  1845,     0,  1847,   595,     0,
     608,     0,     0,     0,  1856,     0,   602,   603,   604,     0,
       0,     0,     0,  1822,   605,   606,   607,     0,     0,   597,
       0,   609,     0,     0,   486,     0,     0,     0,     0,     0,
     608,     0,   595,     0,     0,     0,     0,     0,   598,     0,
     599,   600,   601,     0,     0,   597,     0,  1857,     0,     0,
       0,   609,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   598,     0,   599,   600,   601,     0,
       0,     0,     0,     0,     0,     0,     0,   597,     0,   602,
     603,   604,     0,     0,     0,     0,   595,   605,   606,   607,
       0,     0,     0,     0,     0,     0,   598,   486,   599,   600,
     601,  -691,     0,   608,     0,   602,   603,   604,     0,     0,
       0,   597,     0,   605,   606,   607,     0,     0,     0,     0,
       0,     0,     0,   486,   609,     0,     0,     0,     0,   608,
     598,     0,   599,   600,   601,     0,     0,   602,   603,   604,
       0,     0,     0,     0,     0,   605,   606,   607,     0,     0,
     609,     0,     0,     0,     0,   486,     0,     0,     0,     0,
       0,   608,     0,     0,     0,   597,     0,     0,     0,     0,
       0,   602,   603,   604,     0,     0,     0,     0,     0,   605,
     606,   607,   609,     0,   598,    96,   599,   600,   601,   486,
       0,   184,   439,     0,     0,   608,     0,    96,   185,     0,
      97,   186,     0,   184,  1715,     0,     0,     0,    99,   440,
     185,   100,    97,   186,     0,     0,   609,   101,     0,   188,
      99,   440,     0,   100,     0,   602,   603,   604,     0,   101,
       0,   188,     0,   605,   606,   607,   102,     0,     0,     0,
       0,     0,     0,   486,     0,     0,     0,     0,   102,   608,
     103,     0,     0,     0,     0,     0,     0,     0,     0,   104,
     189,   105,   103,     0,     0,   106,     0,    30,  -223,     0,
     609,   104,   189,   105,     0,     0,     0,   106,     0,    30,
    -223,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   311,   190,     0,
       0,   312,     0,     0,   191,   192,     0,     0,    96,   311,
     190,     0,     0,   312,   184,  1799,   191,   192,     0,     0,
       0,   185,     0,    97,   186,     0,     0,     0,     0,    96,
       0,    99,   440,     0,   100,   184,   309,     0,     0,     0,
     101,     0,   188,     0,    97,   186,     0,     0,     0,   310,
       0,     0,    99,   230,     0,   100,     0,     0,     0,   102,
       0,   101,     0,   188,     0,   230,     0,     0,     0,     0,
       0,     0,     0,   103,     0,     0,     0,     0,     0,     0,
     102,     0,   104,   189,   105,     0,     0,     0,   106,     0,
      30,  -223,     0,     0,   103,     0,     0,     0,     0,     0,
       0,     0,     0,   104,   189,   105,     0,    96,     0,   106,
       0,    30,  -223,   184,  1636,     0,     0,     0,     0,     0,
     311,   190,    97,     0,   312,     0,     0,   191,   192,     0,
      99,    96,     0,   100,     0,     0,     0,   184,     0,   101,
       0,   311,   190,     0,   185,   312,    97,   186,   191,   192,
       0,   187,     0,     0,    99,     0,     0,   100,   102,     0,
       0,     0,     0,   101,     0,   188,     0,     0,     0,     0,
       0,     0,   103,     0,     0,     0,   230,     0,    96,     0,
       0,   104,   102,   105,   184,  1719,     0,   106,     0,    30,
    -223,     0,     0,    97,     0,     0,   103,   230,     0,     0,
       0,     0,     0,     0,   100,   104,   189,   105,   526,     0,
     101,   106,     0,    30,  -223,     0,     0,     0,   527,   528,
     529,     0,     0,     0,     0,     0,   530,     0,   531,   102,
     532,   533,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   534,   103,   190,   535,     0,     0,     0,     0,
     191,   192,   104,     0,   105,   536,   537,     0,   106,     0,
      30,  -223,     0,     0,     0,     0,   538,     0,     0,     0,
       0,     0,     0,     0,     0,   230,     0,     0,     0,     0,
     539,   540,     0,     0,     0,   541,     0,     0,   542,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   543,     0,
       0,   544,     0,     0,   545,   546
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1531)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      30,   214,   165,    87,   146,    68,    68,    19,    87,    68,
      68,    68,   225,    68,    68,   146,    68,   703,    68,   182,
     696,    68,   354,   165,   388,   283,   388,   388,   370,    17,
     519,   971,   370,  1101,   165,   962,   164,   179,   511,  1073,
     182,   696,   872,   514,   866,   514,   868,   171,    97,   173,
      99,   182,   176,   400,   809,   312,    68,   696,   801,   481,
     144,  1146,   172,   258,   932,   175,  1101,   863,  1397,   865,
     247,  1176,   710,   711,  1595,   354,   926,    20,   751,   430,
     359,   949,  1612,   146,   146,   226,   370,   146,   146,   146,
       9,   146,   146,     7,   146,   979,   146,  1182,   557,   356,
     559,   951,   165,   165,    11,   989,   165,   165,   165,   181,
     165,   165,     7,   165,   926,   165,   179,   166,   165,   182,
     182,  1169,  1170,   182,   182,   182,   481,   182,   182,   481,
     182,   388,   182,   257,   146,   182,    10,   662,    30,   951,
     189,     7,   976,   192,   489,   490,     7,   981,    62,     7,
     222,    42,    42,   165,   988,   265,   488,    72,    34,    26,
      15,    42,    84,    70,   768,    77,    98,   179,     4,  1003,
     182,    47,  1006,    30,    10,   174,  1034,  1035,  1036,  1037,
    1038,  1039,    26,    11,   214,   517,   216,    94,    27,   146,
      58,   221,    30,    29,    35,   225,    62,    26,   146,    73,
     174,    62,    42,   178,    62,   400,   174,   194,    59,   196,
     552,    42,    39,    96,   129,   130,   495,  1738,    11,   194,
     194,    59,   146,    26,   481,   182,  1060,    68,   196,    79,
     514,   174,   489,   490,   182,    30,    72,    73,    72,   518,
     713,    92,    70,   292,   293,   294,   295,   296,   297,    87,
       0,    11,   509,    11,   168,   174,   188,   514,   182,   174,
     174,   168,   176,   312,  1794,    34,    94,   174,   552,   631,
      16,    35,   396,   168,   696,   197,    69,    70,    47,   174,
     195,   176,   312,   195,   568,   409,   890,   411,   195,   413,
     400,    88,    87,    39,   168,    41,   188,   554,   189,    45,
     174,   514,   426,    72,    68,   195,   416,   174,   174,   419,
      70,   435,    70,   174,   195,   387,   174,    63,   194,   174,
     196,   204,   371,  1844,   354,   609,   356,   195,  1858,   359,
     174,   188,    72,    26,   137,   174,  1421,   367,   188,   690,
     168,   696,  1015,   194,   696,   174,   174,   174,   188,  1879,
     188,  1456,  1182,   156,  1222,   568,   386,   188,   388,   195,
     390,   703,  1410,  1893,    18,    72,   200,   195,    98,  1890,
     725,   174,   175,   725,   631,   168,   179,   518,   188,   734,
      77,   174,   734,  1267,  1234,    96,  1907,   846,   191,    30,
     915,   916,   195,   188,   197,  1916,   609,   742,   174,    98,
     197,   146,    72,  1089,   187,   174,    96,   178,   168,   741,
     168,    30,   696,    96,   174,   198,   174,    72,   182,   703,
     196,   128,  1234,   194,   188,   194,   195,   196,   174,  1263,
     199,   200,  1266,   197,   174,  1269,  1270,   182,  1272,   696,
     489,   134,  1080,    96,    96,  1774,    92,   477,    49,  1778,
      11,   481,   182,    21,  1522,  1313,  1314,  1315,    72,   489,
     490,    29,   116,   156,   659,    26,     3,   174,   725,   664,
     665,   648,    84,    92,    75,    92,   506,   734,   508,   509,
     510,   511,    98,   182,   514,   742,   197,  1522,   195,   746,
     747,  1559,    72,   200,   187,   188,   189,   194,   777,   861,
     862,    92,  1831,  1608,   866,   867,   868,   197,    69,    70,
      72,   873,   195,    81,    51,   198,    92,    54,   192,   174,
    1605,    89,   552,    60,   554,   195,    96,   168,   558,   188,
     200,    96,    72,   174,   187,   814,   585,   661,   568,  1479,
     195,  1030,   821,   195,   199,   200,  1875,    77,   194,   168,
     130,   835,   188,    90,   122,   174,  1299,    17,   126,  1381,
     174,   818,   909,   673,  1319,    35,   676,   129,   130,   920,
     921,   926,   188,   110,   926,   174,   188,   194,   174,   609,
    1376,   195,    72,   195,    90,   197,   200,   199,   200,    23,
     191,   192,   191,   189,   174,    96,   951,    96,    68,   951,
    1071,   631,  1071,   194,   861,   862,   174,   168,    96,   866,
     867,   868,   174,   174,    74,    49,   873,  1685,   194,    72,
    1488,  1451,   187,   127,   115,     0,   883,   197,    16,    92,
     195,   168,   169,   170,   174,   172,   173,   174,   168,   176,
     130,    75,    96,   998,   174,    96,   998,   184,   185,  1483,
      84,    39,    92,    41,   194,   195,   196,    45,   195,   199,
     200,   128,   168,    97,    96,  1733,   696,   951,   174,   926,
    1032,    96,  1599,   703,  1601,    63,   129,   130,   708,   709,
     710,   711,    96,   713,   174,    96,   187,   388,   187,   195,
     720,   970,   704,   197,   951,   725,    35,   128,   174,   187,
     128,   174,    26,   146,   734,   195,   197,   964,    26,   739,
    1396,   174,   742,  1798,   909,  1749,   746,   747,   187,  1061,
     769,   174,    23,   196,   146,   188,   195,   194,    26,    68,
    1237,   859,    23,   187,   174,   138,   187,    35,   995,   182,
    1808,   998,   195,    76,    42,  1252,    79,  1089,   188,  1091,
     184,   185,   186,  1091,  1011,   187,   174,   191,   192,   789,
     182,   189,   187,   197,   195,   199,   194,   755,    92,   200,
      68,   174,  1056,   187,    92,  1032,   187,  1061,   196,   809,
     168,   174,   906,    84,   814,   815,   174,  1071,   818,    23,
    1074,   821,   195,    84,   174,    96,   137,  1882,   201,   100,
     101,   102,   103,   104,   105,  1089,   131,  1091,   134,  1066,
      51,   191,   174,  1753,  1071,  1883,   194,  1101,   848,  1103,
    1099,   201,    35,  1080,  1108,   203,   188,  1466,    39,   173,
     155,   861,   862,    20,   175,    26,   866,   867,   868,     8,
      27,   185,  1184,   873,    35,   174,    12,  1320,  1321,    90,
      84,    42,    63,   883,   195,    68,     3,  1925,  1071,    39,
     174,   187,   188,   189,    30,   134,   100,   101,   102,   103,
     104,   105,   198,    60,  1801,    88,    26,    68,   131,  1234,
      30,    50,  1234,    63,    53,    35,   187,   168,  1101,    18,
    1103,    57,    61,   174,   147,  1108,   926,    66,   188,    28,
    1184,    30,   155,   138,    51,   134,   196,    54,    37,  1166,
    1435,  1436,  1437,  1438,  1439,  1440,  1173,   174,    68,   188,
     189,   951,    88,    52,    58,  1611,    95,   168,   169,   170,
     631,   172,   173,   174,   964,   176,   134,   192,    88,   174,
     970,   115,  1618,    90,   179,   975,   138,    76,    77,   204,
     980,   134,   982,   127,   195,   985,  1595,   174,   187,   188,
     189,  1245,    91,  1618,    93,   995,   174,   196,   998,   174,
    1227,   176,  1229,  1230,  1231,  1614,   178,  1234,   195,  1618,
     188,  1011,   174,   175,    42,   134,   188,   116,    98,     9,
     188,   189,  1249,  1250,   194,   696,   196,  1281,  1282,   187,
      20,  1363,  1032,   195,  1366,   188,   189,    27,   196,   201,
     120,   188,  1296,  1502,   124,  1064,   174,  1379,    58,  1381,
     197,   168,   169,   170,    44,   172,   173,   174,  1390,   176,
     188,  1061,    36,   134,  1291,   136,  1066,   184,   185,   188,
     189,  1071,   174,  1516,  1466,   168,   188,    67,   195,   200,
    1080,   174,   194,   174,  1396,   137,   138,  1419,  1419,  1089,
      64,    65,   174,  1739,   182,   137,   138,   188,    88,   188,
     188,  1101,   195,  1103,   168,     6,   188,  1334,  1108,   197,
     174,    85,   174,  1445,  1739,  1115,   187,   188,   189,   174,
      21,   176,   174,   175,   188,    26,   188,   179,    29,  1738,
    1739,    32,   174,   175,    64,    65,  1363,    38,   174,  1366,
     189,  1466,  1396,   195,  1466,   194,   134,   174,   178,   201,
     174,   174,  1379,   195,  1381,    85,    57,  1803,   188,   201,
     174,   188,   187,  1390,   194,   188,  1166,   188,  1470,  1423,
      71,   196,   180,  1173,   188,   196,   847,   165,  1803,    80,
     188,    82,   137,   174,  1184,    86,   194,    88,    89,  1416,
     861,   862,  1419,   189,  1803,   866,   867,   868,   194,   187,
     188,   189,   873,  1595,  1850,   168,  1701,  1702,  1703,  1704,
      42,   174,  1466,   176,  1468,   136,   137,    42,  1445,   174,
     175,  1664,  1614,   174,   179,  1850,  1618,  1227,   174,  1229,
    1230,  1231,  1564,  1564,  1234,  1844,     3,   188,  1466,  1466,
     175,  1850,   174,   187,   179,  1472,  1473,   179,   137,  1249,
    1250,  1305,   196,   174,   175,   926,  1305,   188,   179,   195,
    1487,   168,    42,   188,   200,  1265,   197,   174,  1522,   176,
     191,   196,   137,   216,   195,   137,   197,   189,   221,   174,
     951,  1890,   194,   195,    51,   174,   175,    54,   188,  1614,
     179,  1291,  1614,  1618,   194,   188,  1618,    26,  1907,  1611,
     189,    30,   191,   196,   174,  1559,   195,  1916,   197,   174,
     175,    26,   174,   175,   179,    30,   168,   179,  1620,  1319,
    1320,  1321,   174,    90,   189,   168,   191,   174,    42,   191,
     195,   174,   197,   195,  1334,   197,   174,  1564,  1432,  1522,
    1340,  1595,   188,   189,   188,  1572,  1738,  1739,   195,  1429,
     194,   174,   199,   200,    31,    32,  1489,  1611,    67,   188,
    1614,  1032,   188,  1363,  1618,   194,  1366,   188,   194,  1623,
     144,   174,   188,   194,    24,    25,  1559,  1489,   194,  1379,
     195,  1381,   174,   174,   199,   200,   174,  1614,  1489,   174,
    1390,  1618,   166,   167,  1448,   194,  1396,   196,  1652,   173,
     174,   168,   169,   170,   144,   172,   173,   174,   174,   176,
     189,  1803,   174,   144,  1739,   194,  1416,  1739,   174,  1419,
    1754,    31,  1754,  1754,   367,  1096,   166,   167,   195,    24,
      25,  1685,   189,   173,   174,   166,   167,   194,   184,   185,
     186,   189,   173,   174,   195,  1445,   194,   390,   199,   200,
      89,  1505,  1844,   180,   189,  1709,  1489,  1489,  1850,   194,
    1489,  1489,  1489,   189,  1489,  1489,  1466,  1489,   194,  1489,
     188,  1805,  1489,  1805,  1805,   191,   192,   189,  1803,  1733,
     196,  1803,   194,   189,  1738,  1739,   189,  1487,   194,   188,
    1744,   194,  1586,   137,  1606,   188,  1828,  1828,  1890,   195,
    1832,  1832,  1685,   199,   200,  1606,   195,  1489,   191,   192,
     199,   200,  1739,   196,   195,  1907,  1516,   180,   199,   200,
    1747,   189,  1522,   188,  1916,  1850,   194,  1754,  1850,   188,
     174,   175,    68,   195,   477,   179,   178,   179,   180,   181,
     182,   183,  1874,  1874,   195,   191,   192,   191,   194,  1803,
    1733,   195,   195,   197,  1808,   137,   191,   192,   194,  1559,
     196,   196,   137,  1234,  1564,   508,   191,   192,   189,  1901,
    1901,   196,  1572,  1606,  1606,  1246,  1803,  1606,  1805,  1606,
     189,  1606,  1606,   189,  1606,   194,  1606,    15,   194,  1606,
    1844,   195,   174,   175,    42,  1595,  1850,   179,   189,   174,
     175,  1828,   202,   194,   179,  1832,   194,   137,   196,   191,
     174,  1611,   189,   195,  1614,   197,   191,   194,  1618,   174,
     195,   501,   197,  1850,  1606,  1808,   174,   507,   189,  1883,
     189,     6,   512,   194,   514,   194,  1890,    12,    13,   170,
     520,   172,   173,   174,   174,   175,    21,  1874,   174,   179,
     191,   192,    67,  1907,    29,   196,   194,    32,   196,  1713,
     174,   191,  1916,    38,  1664,   195,   174,   197,   191,   192,
       3,  1925,   188,   196,  1901,   555,    42,   174,    11,   191,
     192,    14,    57,   188,   196,  1685,    19,    20,   568,   195,
     191,   192,  1363,   194,    27,  1366,    71,    30,   191,   192,
    1883,   174,   188,   196,    37,    80,   197,    82,  1379,  1709,
    1381,    86,   144,    88,    89,   595,   149,   144,    51,  1390,
     186,    54,   100,   101,   102,   103,   104,   105,   173,   609,
     132,   191,   192,  1733,   194,   188,    69,    70,  1738,  1739,
     191,   192,  1925,   194,   146,   174,   148,   176,  1419,   191,
     192,   194,   194,   196,  1754,   157,   194,    90,   196,   154,
     162,    94,   164,   163,    26,   708,   709,   710,   711,   189,
     191,   192,   174,   194,  1445,   174,   194,   720,   196,     3,
     194,   194,   196,   196,   194,   174,   196,    11,   121,   194,
     194,   196,   196,   174,     5,  1466,   739,    84,  1469,   174,
      42,    42,    26,  1803,   194,  1805,   196,   194,  1808,   196,
     194,   128,   196,    37,   188,   195,   188,   188,   195,    56,
     174,   203,   144,   187,   194,   194,   187,    51,  1828,   186,
      54,   175,  1832,   175,   174,   168,   169,   170,   188,   172,
     173,   174,    96,   176,  1844,    69,    70,   188,   195,   188,
    1850,   184,   185,   189,   188,   174,   188,   188,   738,    94,
     195,   199,   195,   133,   174,   135,    90,    58,   174,   174,
      94,   115,   815,   188,  1874,    60,   146,   197,   148,    99,
      58,    58,   174,  1883,   174,   174,   174,   157,     3,    97,
    1890,   161,   162,  1564,   164,   188,    11,   121,   195,   195,
     195,  1901,   186,   188,   174,   848,   144,  1907,   144,   195,
     790,    26,   195,   195,   195,   195,  1916,   189,   798,   195,
     195,   188,    37,   144,   195,  1925,   194,   187,   144,     3,
     195,   189,    88,   194,   174,   188,    51,    11,    17,    54,
      98,   188,   188,  1614,   168,   169,   170,  1618,   172,   173,
     174,   199,   176,    96,    69,    70,   188,   188,   188,    42,
     184,   185,   195,    37,    73,   188,   195,   194,   196,   188,
     188,   195,    77,   188,   188,    90,   188,    51,   188,    94,
      54,   188,   174,   863,   864,   865,    47,   196,   188,   195,
     203,   871,   178,   144,   874,    69,    70,   877,   194,   879,
     880,   182,   882,    11,    42,   117,   121,   182,    48,    48,
      68,   174,   188,   893,   196,   194,    90,   188,    26,   188,
      94,   188,   195,   195,   198,   195,    42,   189,   194,    42,
       8,    61,   975,    95,    58,   128,   127,   980,   188,   982,
      58,   195,   985,   188,   174,   925,   174,   121,   928,   188,
     188,   188,   194,   168,   169,   170,   196,   172,   173,   174,
     194,   176,     5,   196,   196,   194,    42,   194,  1739,   184,
     185,   196,   194,   196,   194,   196,   194,   196,   188,    39,
     195,    83,    90,  1754,    92,   188,   188,   188,   188,    98,
     188,   188,   144,   174,   168,   169,   170,   175,   172,   173,
     174,   109,   176,   111,   112,   113,   178,    96,   175,   175,
     184,   185,   144,   195,   174,   195,    88,   188,   195,   195,
      78,   195,    34,   174,  1004,   117,   195,   195,    30,   188,
     127,   188,  1803,   198,  1805,    26,    73,    99,   188,    56,
     196,   115,   150,   151,   152,   180,     3,   196,   196,     6,
     158,   159,   160,    79,    11,    12,    13,  1828,    65,   188,
     168,  1832,    19,   188,    21,    22,   174,   188,   188,   188,
      92,   188,    29,    30,    31,    32,   188,   188,   188,  1850,
      37,    38,   188,    40,   188,   188,    92,   195,  1068,   188,
     188,  1071,    92,  1073,    51,   174,   196,    54,   194,   194,
      57,   188,     3,  1874,   188,   188,   188,   195,   178,    68,
      88,    39,    69,    70,    71,    88,   117,   188,   106,   188,
      19,  1101,  1102,    80,    81,    82,   194,    58,  1108,    86,
    1901,    88,    89,    90,   188,   196,  1116,    94,   188,   174,
     174,   115,   174,   174,   188,   194,   194,   194,   188,    47,
      51,   144,   188,    54,   173,   175,    92,   182,     3,   188,
     188,   118,   119,  1143,   121,   122,    11,    92,   125,   126,
     188,   117,   188,    96,  1154,    30,    26,   188,     3,  1159,
     188,    26,   123,   127,   174,  1165,   173,  1167,   174,    90,
     174,   188,    37,   170,  1174,   170,    18,    18,    99,   188,
     194,  1181,    34,   188,    30,   188,    51,   188,   188,    54,
     127,   168,   169,   170,    26,   172,   173,   174,   188,   176,
     188,   188,  1265,   188,    69,    70,    51,   184,   185,    54,
      55,   194,   194,   188,   174,   173,    26,    18,   195,    64,
      65,  1221,    34,   188,   174,    90,  1226,     3,    30,    94,
      26,   188,    69,  1233,   170,    11,   170,    37,    88,   188,
      85,   195,   188,    83,   196,    90,    14,   168,   169,   170,
      26,   172,   173,   174,    88,   176,   121,    24,   121,   188,
      26,    37,     3,   184,   185,   110,   188,    30,   173,    88,
     194,    70,   188,   188,   195,    51,   188,  1340,    54,   188,
     188,   187,   144,   174,   188,   188,   188,   173,   188,   196,
     144,   178,   174,    69,    70,     3,   194,  1297,  1298,   195,
     173,   194,   173,   168,   169,   170,   196,   172,   173,   174,
      51,   176,   178,    54,    90,  1082,   500,   146,    94,   184,
     185,    62,   182,   168,   169,   170,  1326,   172,   173,   174,
     195,   176,   489,     3,  1502,   724,  1334,   146,  1012,   184,
     185,  1300,  1259,    51,  1479,   121,    54,  1448,    18,    90,
     195,   182,   311,   188,   570,   892,  1061,   585,   851,   792,
    1850,   225,  1362,     3,  1598,  1365,  1447,    37,   481,   110,
    1370,  1799,  1166,  1373,  1374,  1719,  1376,  1710,  1862,  1614,
     481,    51,    90,  1738,    54,  1810,   481,  1387,  1388,  1381,
    1379,  1456,   168,   169,   170,  1373,   172,   173,   174,  1376,
     176,  1170,   110,  1390,  1366,     3,  1363,  1386,   184,   185,
      11,    51,  1227,  1416,    54,    55,   478,  1071,  1418,   195,
      90,  1747,  1049,    24,    25,    26,  1473,   168,   169,   170,
    1099,   172,   173,   174,     3,   176,   571,   558,    30,    37,
    1103,   840,     7,   184,   185,    23,    82,   845,   437,    82,
      90,   121,   426,    51,   195,   920,    54,   419,   673,   690,
     168,   169,   170,    -1,   172,   173,   174,    -1,   176,   659,
     110,   664,  1472,  1473,   421,    -1,   184,   185,    -1,   423,
     188,    -1,    51,    -1,   659,    54,    -1,   195,    -1,    90,
      -1,    60,    90,    -1,    -1,     3,    -1,    -1,   168,   169,
     170,   665,   172,   173,   174,    -1,   176,    -1,   109,    -1,
     111,   112,   113,    -1,   184,   185,    -1,    -1,  1518,    -1,
      -1,    90,  1522,   121,  1524,   195,    -1,    -1,   168,   169,
     170,    -1,   172,   173,   174,    -1,   176,     3,    -1,    -1,
      -1,   110,    -1,    51,   184,   185,    54,    -1,    -1,   150,
     151,   152,    -1,    -1,    -1,   195,    -1,   158,   159,   160,
      -1,     3,  1562,    -1,    -1,    -1,  1566,   168,    -1,    -1,
     168,   169,   170,   174,   172,   173,   174,    -1,   176,    -1,
      -1,    -1,    90,    -1,    -1,    51,   184,   185,    54,    -1,
      -1,     3,    -1,    -1,   195,    -1,    62,   195,    -1,   168,
     169,   170,   110,   172,   173,   174,    -1,   176,    -1,    51,
      -1,    -1,    54,    -1,    -1,   184,   185,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    90,    -1,   195,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,    -1,    51,
      -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    90,    -1,
      24,    25,    26,  1653,    -1,    -1,  1656,    -1,    -1,    -1,
     168,   169,   170,    -1,   172,   173,   174,    11,   176,    -1,
    1670,    -1,    -1,    -1,    -1,    -1,   184,   185,    90,    -1,
      -1,    -1,    26,    -1,    -1,    -1,    -1,   195,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,
      -1,    -1,   168,   169,   170,    -1,   172,   173,   174,    -1,
     176,    -1,    -1,    -1,    -1,    -1,    90,    -1,   184,   185,
      -1,    -1,    -1,    -1,    -1,    -1,   168,   169,   170,   195,
     172,   173,   174,    -1,   176,   109,    -1,   111,   112,   113,
      11,    -1,   184,   185,    -1,    -1,    90,  1747,    92,  1749,
      -1,    -1,    -1,   195,    -1,    26,   168,   169,   170,    -1,
     172,   173,   174,    -1,   176,   109,    11,   111,   112,   113,
      -1,    -1,   184,   185,    -1,    -1,   150,   151,   152,  1779,
      -1,    26,  1782,   195,   158,   159,   160,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   168,  1795,    -1,  1797,    11,    -1,
     174,    -1,    -1,    -1,  1804,    -1,   150,   151,   152,    -1,
      -1,    -1,    -1,    26,   158,   159,   160,    -1,    -1,    90,
      -1,   195,    -1,    -1,   168,    -1,    -1,    -1,    -1,    -1,
     174,    -1,    11,    -1,    -1,    -1,    -1,    -1,   109,    -1,
     111,   112,   113,    -1,    -1,    90,    -1,    26,    -1,    -1,
      -1,   195,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,    -1,   111,   112,   113,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    -1,   150,
     151,   152,    -1,    -1,    -1,    -1,    11,   158,   159,   160,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   168,   111,   112,
     113,    26,    -1,   174,    -1,   150,   151,   152,    -1,    -1,
      -1,    90,    -1,   158,   159,   160,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   168,   195,    -1,    -1,    -1,    -1,   174,
     109,    -1,   111,   112,   113,    -1,    -1,   150,   151,   152,
      -1,    -1,    -1,    -1,    -1,   158,   159,   160,    -1,    -1,
     195,    -1,    -1,    -1,    -1,   168,    -1,    -1,    -1,    -1,
      -1,   174,    -1,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      -1,   150,   151,   152,    -1,    -1,    -1,    -1,    -1,   158,
     159,   160,   195,    -1,   109,     6,   111,   112,   113,   168,
      -1,    12,    13,    -1,    -1,   174,    -1,     6,    19,    -1,
      21,    22,    -1,    12,    13,    -1,    -1,    -1,    29,    30,
      19,    32,    21,    22,    -1,    -1,   195,    38,    -1,    40,
      29,    30,    -1,    32,    -1,   150,   151,   152,    -1,    38,
      -1,    40,    -1,   158,   159,   160,    57,    -1,    -1,    -1,
      -1,    -1,    -1,   168,    -1,    -1,    -1,    -1,    57,   174,
      71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,
      81,    82,    71,    -1,    -1,    86,    -1,    88,    89,    -1,
     195,    80,    81,    82,    -1,    -1,    -1,    86,    -1,    88,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,    -1,
      -1,   122,    -1,    -1,   125,   126,    -1,    -1,     6,   118,
     119,    -1,    -1,   122,    12,    13,   125,   126,    -1,    -1,
      -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,     6,
      -1,    29,    30,    -1,    32,    12,    13,    -1,    -1,    -1,
      38,    -1,    40,    -1,    21,    22,    -1,    -1,    -1,    26,
      -1,    -1,    29,   174,    -1,    32,    -1,    -1,    -1,    57,
      -1,    38,    -1,    40,    -1,   174,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    80,    81,    82,    -1,    -1,    -1,    86,    -1,
      88,    89,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    82,    -1,     6,    -1,    86,
      -1,    88,    89,    12,    13,    -1,    -1,    -1,    -1,    -1,
     118,   119,    21,    -1,   122,    -1,    -1,   125,   126,    -1,
      29,     6,    -1,    32,    -1,    -1,    -1,    12,    -1,    38,
      -1,   118,   119,    -1,    19,   122,    21,    22,   125,   126,
      -1,    26,    -1,    -1,    29,    -1,    -1,    32,    57,    -1,
      -1,    -1,    -1,    38,    -1,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,   174,    -1,     6,    -1,
      -1,    80,    57,    82,    12,    13,    -1,    86,    -1,    88,
      89,    -1,    -1,    21,    -1,    -1,    71,   174,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    80,    81,    82,     9,    -1,
      38,    86,    -1,    88,    89,    -1,    -1,    -1,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    27,    -1,    29,    57,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    71,   119,    46,    -1,    -1,    -1,    -1,
     125,   126,    80,    -1,    82,    56,    57,    -1,    86,    -1,
      88,    89,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   174,    -1,    -1,    -1,    -1,
      81,    82,    -1,    -1,    -1,    86,    -1,    -1,    89,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,    -1,
      -1,   122,    -1,    -1,   125,   126
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   131,   147,   155,   206,   207,   556,   557,   558,   174,
     174,   191,   201,   560,   561,   562,     0,   208,   209,   210,
     558,   188,   188,   192,   209,     9,    20,    27,    44,    67,
      88,   211,   212,   213,   214,   215,   216,   217,   233,   238,
     250,   256,   257,   335,   559,   174,   562,   174,   174,   174,
     174,    15,   174,   174,   538,   539,   188,   194,    42,   563,
      58,    58,   218,   174,   251,   200,   188,   174,   259,   133,
     135,   146,   148,   157,   161,   162,   164,   174,   564,   565,
     566,   567,   570,   571,   574,   174,   174,    42,   219,    42,
     252,     7,   168,   174,   176,   542,     6,    21,    26,    29,
      32,    38,    57,    71,    80,    82,    86,   226,   229,   260,
     261,   262,   263,   264,   265,   271,   272,   273,   279,   284,
     285,   293,   298,   335,   368,   174,   568,   174,   174,   174,
     174,   174,   174,   174,   174,   132,   571,   574,    42,    42,
     239,    35,    68,   221,   222,   223,   254,   168,   174,   176,
     299,   174,   280,    67,   258,   280,   168,   174,   268,   174,
     174,   174,    42,   188,    31,   269,    89,   180,   188,   194,
     180,   188,   188,   188,   180,   188,   188,   195,   234,   242,
     195,   195,   224,   223,    12,    19,    22,    26,    40,    81,
     119,   125,   126,   226,   229,   255,   261,   262,   271,   277,
     279,   281,   284,   286,   293,   298,   307,   313,   314,   330,
     335,   336,   368,   379,   189,   300,   189,   194,    15,   174,
     188,   189,   195,    42,   189,    42,    42,   188,   268,    13,
     174,   226,   229,   261,   265,   270,   271,   279,   284,   293,
     298,   313,   314,   335,   338,   368,   280,   202,   174,   174,
     179,   569,   137,   174,   175,   179,   191,   195,   197,   582,
     583,   584,   585,   138,   174,   195,   201,   576,   577,   578,
     591,   592,   593,   584,   173,   185,   175,   579,   591,   584,
     174,   572,   573,   236,    12,    30,    57,   229,   241,   243,
     314,   335,    21,    29,    81,    89,   122,   126,   280,   497,
     498,   499,   500,   502,   503,   504,   507,   508,   497,    13,
      26,   118,   122,   225,   226,   229,   261,   265,   271,   277,
     279,   281,   284,   286,   287,   293,   298,   313,   314,   330,
     334,   335,   336,   338,   368,   379,   174,   174,     7,    62,
     174,   331,   332,   333,    67,   253,   332,   337,   280,   174,
     174,   280,   369,   372,   538,   539,    42,   371,   372,   538,
     174,   188,   371,   497,   195,   174,   369,     4,    10,    29,
      72,    73,   195,   274,   275,   276,   344,   347,   348,   351,
     355,   356,   357,   362,   365,   366,    77,   195,   267,   339,
     189,   138,   569,   144,   188,   144,   197,   584,   175,   179,
     195,   583,   586,   587,   589,   590,   591,   594,   595,   134,
     136,   187,   188,   189,   591,   174,   195,   149,    96,   187,
      96,   187,   195,   186,    26,   156,   189,   575,   188,   173,
      96,   204,   163,    96,   165,   189,   154,   194,   196,    13,
      30,   226,   229,   261,   265,   271,   277,   279,   281,   284,
     286,   287,   293,   298,   307,   313,   314,   321,   330,   334,
     335,   336,   338,   368,   379,   392,   174,   174,   244,   174,
      26,   280,   280,   280,   280,   280,   280,   189,   188,   196,
     196,   231,    27,   174,   220,   331,   168,   174,   280,   288,
     290,   291,   537,   538,   540,   541,   543,   547,   189,   315,
     308,     5,   189,   194,   174,   188,   189,    84,   189,    42,
      42,   189,   128,   289,   195,   289,   348,   511,   538,   537,
      98,   188,   511,   188,   294,   196,     9,    19,    20,    21,
      27,    29,    31,    32,    43,    46,    56,    57,    67,    81,
      82,    86,    89,   119,   122,   125,   126,   227,   320,   195,
     188,   371,   195,   360,    58,   195,   367,     3,    51,    54,
      90,   169,   170,   172,   173,   176,   184,   185,   195,   349,
     526,   527,   528,   530,   532,   533,   534,   536,   537,   538,
     543,   548,   550,   551,   280,   363,   364,   174,   176,   345,
     346,   188,    56,   538,   497,    11,    26,    90,   109,   111,
     112,   113,   150,   151,   152,   158,   159,   160,   174,   195,
     409,   439,   440,   441,   442,   443,   444,   445,   446,   454,
     458,   460,   461,   464,   467,   468,   471,   474,   475,   476,
     479,   480,   482,   484,   486,   488,   490,   491,   492,   493,
     537,   538,   551,   174,   340,   341,   342,   371,   194,   203,
     203,   194,   194,   584,   196,   192,   204,   144,   590,   187,
     198,    96,   195,   198,   187,   186,   584,   584,   584,   136,
     584,   196,   591,   175,   578,   591,   175,   592,   144,   166,
     167,   173,   174,   588,   593,   575,   188,   580,   581,   582,
      96,   175,   179,   174,   573,   188,   237,     7,    62,   174,
     322,   323,   324,   195,   246,    20,   174,   240,   189,   189,
     189,   189,   189,   189,    16,    39,    41,    45,    63,   371,
     510,   498,   188,   188,    26,    69,    70,   232,   393,   396,
     403,   406,   458,   475,   480,   537,   174,   188,    94,   189,
     289,   280,   290,   291,   291,   188,    84,   197,   303,   199,
     511,   199,   511,   174,    58,   208,   306,   110,   518,   522,
     524,   526,   174,   174,   188,   538,   518,   371,    10,    73,
     278,   375,   376,   377,   537,   380,   381,   538,   380,   518,
      60,   372,   374,   512,   513,   516,   518,   526,   537,   538,
     115,   370,   289,   348,   511,   301,   303,   518,    60,   295,
     296,   194,   196,    99,   228,   174,   176,   230,   349,   358,
     359,   361,   371,   373,   538,    58,   537,   518,    58,   533,
     536,   538,   533,   174,   174,   174,    62,   374,   518,   526,
     553,   554,   555,    23,    84,   350,   528,   184,   185,   186,
     529,    49,    75,   191,   192,   531,    97,   199,   189,    26,
     364,   194,   196,   174,   353,   196,   518,    31,    32,   266,
     188,   195,   195,   195,   195,   195,   195,   195,   195,   189,
     518,    98,   182,   195,    18,    28,    30,    37,    52,    76,
      77,    91,    93,   116,   485,    90,   409,   537,   188,   199,
     343,   188,   194,    98,   188,   569,   188,   188,   144,   144,
     198,   144,   194,   196,   587,   589,   189,   584,   588,   195,
     594,   595,   196,   577,   591,   195,   195,   191,   192,   196,
     187,   186,   580,   144,    26,    37,    69,    90,    94,   121,
     382,   383,   386,   393,   403,   404,   406,   407,   408,   409,
     414,   416,   423,   424,   425,   427,   428,   431,   433,   436,
     479,   480,   491,   526,   537,   538,   551,    35,    68,    88,
     325,   327,   328,   329,   189,   194,   170,   173,   245,   373,
     538,   247,   335,   174,   188,    39,   371,   371,   509,   371,
     510,   371,   510,    39,    63,   501,   380,    17,   505,   371,
     220,    70,   458,   475,    42,   195,   387,   398,    69,    70,
     458,   475,   518,   371,    98,   506,   289,   291,   188,   188,
     537,    77,   304,   305,   537,   199,    72,   130,   174,   544,
     544,   188,     7,    62,   168,   174,   176,   316,   317,   318,
     319,   309,   195,   188,     8,    50,    53,    61,    66,    95,
     519,   520,   521,   178,   179,   180,   181,   182,   183,   523,
     100,   101,   102,   103,   104,   105,   525,   188,    17,    74,
     282,   195,   360,   280,   378,   188,   115,   188,   128,   511,
     188,   194,   196,    96,   350,   518,   370,   188,   188,   518,
      42,   188,   320,   188,   194,   196,   196,   359,   538,   194,
     196,    72,   511,   371,   196,   537,   199,   511,   535,   538,
     196,   194,    96,   187,   526,   530,   532,   533,   195,   551,
     371,    73,   346,   188,   352,    77,    76,    79,   459,   188,
     268,   409,   465,   466,   409,   469,   470,   452,   453,   518,
     450,   452,   518,   451,   452,   409,   449,   447,   449,   448,
     449,   518,    55,    64,    65,    85,   411,   472,   473,   518,
     409,   462,   463,   518,    92,   174,   188,   174,   518,    92,
     174,   188,   518,   188,   518,    30,    59,    87,   188,   455,
     456,   457,   518,    30,    92,   494,   495,   496,   537,    47,
     188,    98,   182,   129,   195,   360,   341,   518,   203,   178,
     188,   194,   178,   144,   584,   188,   196,    96,    96,   588,
     588,   144,   166,   167,   173,   174,   144,   166,   167,   173,
     174,   581,   582,   194,     9,   174,   235,   518,   408,   414,
     518,   195,   385,   387,   182,   387,    18,   116,    14,    19,
      20,    27,    30,    37,    69,    90,   121,   405,   408,   414,
     417,   421,   526,   537,   538,   117,   199,    48,    48,    20,
      27,    60,   326,   188,   327,   537,   174,   196,    26,    30,
     243,   248,   249,   371,   506,   194,   505,   371,   506,   371,
     371,   506,   505,   188,   399,   537,   394,    70,   458,   475,
     188,   120,   124,   292,   506,   518,   115,   127,   506,   188,
     537,   194,   305,   198,   544,   195,   195,   195,    42,   189,
     194,   302,   303,    42,   310,   311,   409,   522,   522,   522,
     522,   522,   522,     8,    61,    95,   524,   526,   506,   358,
      58,   189,    26,   280,   537,   518,   128,   513,    60,   517,
     518,   526,   127,   297,   510,   537,   228,   174,   176,   188,
      58,    72,   373,    99,   535,   552,   553,   554,   518,   526,
     555,   518,   188,   174,    26,   174,   354,   538,   518,   174,
     188,   188,   194,   194,   196,   194,   194,   196,   196,   196,
     194,   196,   196,   194,   194,   196,   194,   196,   196,   194,
     196,   194,   188,   518,   472,   188,   194,     5,   194,   196,
     194,   196,    42,   518,   188,   489,    39,    83,   518,   188,
     487,    79,   188,   188,   518,   399,   518,   188,   457,   188,
     455,   457,   188,   537,   518,   188,   194,   494,    96,   481,
     518,   411,   472,    98,   188,   188,   144,   174,   175,   178,
     188,   191,    96,   175,   175,   194,   194,   195,   195,   195,
     195,   144,   174,   188,    88,    78,   518,   387,   222,   223,
     388,    36,   410,   411,   434,   518,   495,   537,   537,   537,
     174,   422,   518,   408,   414,   329,   419,    34,   117,   199,
     526,   129,   195,   195,   537,   537,   329,    30,   174,   322,
     506,   371,   506,   505,   506,   506,   506,   194,   196,   400,
     526,   526,   188,   188,   537,   198,   546,   526,   518,   545,
     518,   320,   319,    26,   312,   222,   223,   196,   522,   522,
     522,   188,   196,   380,   380,    73,   189,   127,   115,   518,
     297,   371,   194,   196,    96,   196,   196,    56,   180,    79,
     518,   466,   188,   518,   470,   188,   188,   188,   170,   518,
     188,   188,   450,   188,   451,   188,   188,   447,   188,   448,
      65,   188,   473,   518,   518,   188,   409,   463,   188,    92,
     483,   188,    92,   373,   481,   188,    92,   174,   188,   188,
     457,   188,    88,   496,   188,   518,    26,   439,   188,   472,
     188,   526,   194,   195,   591,   191,   189,   584,   588,   588,
     588,   588,   588,   588,   178,   426,   409,   196,   388,   223,
     328,   389,   390,   411,   412,   413,   437,    88,   456,   494,
     188,    39,    88,   188,    13,   174,   382,   392,   420,   424,
     526,   129,   289,   117,   514,   515,   516,   517,   514,   188,
      26,   243,   325,   506,   537,   387,    13,   226,   229,   261,
     265,   271,   279,   284,   293,   298,   313,   314,   335,   338,
     368,   401,   194,   194,   196,   196,   194,   196,   188,   302,
      19,   223,   106,   283,    58,   188,   174,   380,   174,   518,
     115,   553,   518,   174,   170,   172,   173,   174,   549,   174,
     194,   194,   194,   554,    26,    92,   518,   477,   518,   188,
     188,   537,    47,   188,   188,   144,   173,   584,   188,   196,
     196,   194,   194,   194,   194,   175,   425,   429,   479,   480,
     182,   390,   327,   188,   327,    13,   392,   188,   472,    13,
     226,   229,   261,   265,   271,   279,   284,   298,   313,   314,
     335,   368,   438,    92,   432,   494,   188,   373,   426,   420,
      26,   382,   424,   289,   117,   188,   526,   194,   196,    96,
     196,    30,    26,   188,   402,   526,   518,   518,   174,   188,
      39,    63,   380,   188,   123,   127,   518,   174,   174,   174,
     188,   170,   170,   173,    96,    18,   554,   188,    24,    25,
     478,   188,    96,   174,   188,   178,   194,   194,   188,   191,
     588,   588,   588,   588,    24,    25,   430,    37,   410,    13,
     188,   223,   188,   391,    92,   402,   554,    26,    92,   188,
     429,    26,    34,   188,   526,   188,   515,   517,   188,    30,
      26,   243,    26,   439,   188,   127,   194,   194,   481,   174,
     188,    96,   481,   518,    26,   518,   188,   174,   173,   191,
     196,   196,   196,   196,   426,   518,    26,   518,   415,   472,
     391,   327,    26,   382,   384,   424,   518,    26,    96,    18,
     554,   430,    34,   174,   418,   188,   188,    30,    26,    69,
     397,   170,   170,   188,   481,    83,    37,   195,   196,    88,
      88,   188,   194,    92,   384,   188,    14,    24,   121,   435,
     426,   174,   188,    96,    26,   418,   188,   188,    30,    70,
     395,   481,   174,   188,   173,   178,   188,   426,   188,   472,
     554,   174,   188,   174,   188,   188,   426,    88,   188,   188,
     174,   188,   188,   194,   144,    92,   188,   188,   174,   188,
     173,   194,   554,   188,   196,   144,   178,   178,   194,   174,
     195,   173,   194,   173,   196,   178
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
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
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
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

  YYUSE (yytype);
}




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
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

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
/* Line 1787 of yacc.c  */
#line 322 "parser.y"
    {
                   constructor();
                 }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 326 "parser.y"
    {
		   destructor();
		 }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 329 "parser.y"
    { destructor(); }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 330 "parser.y"
    { return 1; }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 333 "parser.y"
    {}
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 334 "parser.y"
    { }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 337 "parser.y"
    { }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 346 "parser.y"
    {
		   //string f_name = string(".");
		 //for(list<string>::iterator b=$1->begin(); b!=$1->end(); b++)
		   //   f_name += ('/' + *b);
		   //f_name += ".vhd";		
		   //if(switch_buffer(f_name) == false){
		   //destructor();
		   //yy_abort();
		   //return 1;
		   //}
		 }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 359 "parser.y"
    {}
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 360 "parser.y"
    {}
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 368 "parser.y"
    {}
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 369 "parser.y"
    { }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 376 "parser.y"
    { }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 378 "parser.y"
    {}
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 387 "parser.y"
    {  		
		 new_action_table((yyvsp[(2) - (2)].actionptr)->label);
		 new_event_table((yyvsp[(2) - (2)].actionptr)->label);
		 new_rule_table((yyvsp[(2) - (2)].actionptr)->label);
		 new_conflict_table((yyvsp[(2) - (2)].actionptr)->label);
		 delete tel_tb;
		 tel_tb = new tels_table;
		 tel_tb->set_id((yyvsp[(2) - (2)].actionptr)->label);
		 tel_tb->set_type((yyvsp[(2) - (2)].actionptr)->label);
		 table->new_design_unit((yyvsp[(2) - (2)].actionptr)->label);
		 cur_entity = (yyvsp[(2) - (2)].actionptr)->label;
	       }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 400 "parser.y"
    {
		 if(open_entity_lst->insert((yyvsp[(2) - (4)].actionptr)->label,*tel_tb)==false)
		   err_msg("error: multiple declaration of entity '",
			   (yyvsp[(2) - (4)].actionptr)->label, "'");
		 		
		 if((yyvsp[(4) - (4)].C_str) && !strcmp_nocase((yyvsp[(2) - (4)].actionptr)->label,(yyvsp[(4) - (4)].C_str)))
		   err_msg("warning: '", (yyvsp[(4) - (4)].C_str), "' undeclared as an entity name");
		 // delete tel_tb;
		 //tel_tb=0;
	       }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 413 "parser.y"
    { (yyval.C_str) = (yyvsp[(5) - (6)].C_str); }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 416 "parser.y"
    { (yyval.C_str) = (yyvsp[(7) - (8)].C_str); }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 419 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 420 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 421 "parser.y"
    { (yyval.C_str) = (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 422 "parser.y"
    { (yyval.C_str) = (yyvsp[(2) - (2)].actionptr)->label; }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 427 "parser.y"
    {declare((yyvsp[(1) - (1)].str_bl_lst));}
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 428 "parser.y"
    {declare((yyvsp[(2) - (2)].str_bl_lst));}
    break;

  case 36:
/* Line 1787 of yacc.c  */
#line 436 "parser.y"
    { (yyval.str_bl_lst)= (yyvsp[(3) - (5)].str_bl_lst); }
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 443 "parser.y"
    {}
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 444 "parser.y"
    {}
    break;

  case 52:
/* Line 1787 of yacc.c  */
#line 456 "parser.y"
    {}
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 482 "parser.y"
    {}
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 483 "parser.y"
    {}
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 492 "parser.y"
    {}
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 496 "parser.y"
    {
		   signals[cur_entity].clear();
		   delete tel_tb;
		   const tels_table& TT=open_entity_lst->find(Tolower((yyvsp[(4) - (5)].actionptr)->label));
		   if(TT.empty())
		     err_msg("entity '",(yyvsp[(4) - (5)].actionptr)->label,"' undeclared");
		   tel_tb = new tels_table(TT);
		   tel_tb->set_type(Tolower((yyvsp[(4) - (5)].actionptr)->label));

		   if(table->design_unit(Tolower((yyvsp[(4) - (5)].actionptr)->label)) == false){
		     err_msg("entity '",(yyvsp[(4) - (5)].actionptr)->label,"' undeclared");
		   }
		   table->new_tb();
                 }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 512 "parser.y"
    {
		   if((yyvsp[(11) - (12)].C_str) && strcmp_nocase((yyvsp[(11) - (12)].C_str), (yyvsp[(2) - (12)].actionptr)->label) == false)
		     err_msg("warning: '", (yyvsp[(2) - (12)].actionptr)->label,
			     "' undeclared as an architecture name");

		   if(open_entity_lst->insert(Tolower((yyvsp[(4) - (12)].actionptr)->label),
					      Tolower((yyvsp[(2) - (12)].actionptr)->label),tel_tb)==2)
		     err_msg("error: multiple declaration of architecture '",
			     (yyvsp[(2) - (12)].actionptr)->label, "'");
		   table->delete_tb();	
		 }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 525 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 526 "parser.y"
    {   (yyval.C_str) = (yyvsp[(2) - (2)].actionptr)->label; }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 527 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 528 "parser.y"
    { (yyval.C_str) = (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 552 "parser.y"
    {}
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 553 "parser.y"
    {}
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 556 "parser.y"
    {}
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 570 "parser.y"
    { }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 571 "parser.y"
    {}
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 574 "parser.y"
    {}
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 575 "parser.y"
    {}
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 576 "parser.y"
    {}
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 580 "parser.y"
    {}
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 600 "parser.y"
    { table->new_design_unit((yyvsp[(2) - (2)].actionptr)->label); }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 602 "parser.y"
    {
		   if((yyvsp[(4) - (4)].C_str) && strcmp_nocase((yyvsp[(2) - (4)].actionptr)->label, (yyvsp[(4) - (4)].C_str))==false)
		     err_msg("warning: '", (yyvsp[(2) - (4)].actionptr)->label,
			     "' undeclared as a package label");
		 }
    break;

  case 116:
/* Line 1787 of yacc.c  */
#line 609 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 611 "parser.y"
    { (yyval.C_str) = (yyvsp[(5) - (6)].actionptr)->label; }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 622 "parser.y"
    {}
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 634 "parser.y"
    {}
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 645 "parser.y"
    {
		   /*const char *t = table->find($3->label);
		   if(t == 0 || strcmp_nocase(t, "package"))
		     err_msg("warning: '", $3->label,
			     "' undeclared as a package name");

			     $$ = $3->label;*/
		 }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 657 "parser.y"
    {}
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 660 "parser.y"
    {
		   /*if(strcmp_nocase($1, $6->label))
		     err_msg("warning: '", $6->label,
		     "' is not declared as a package body name");*/
		 }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 675 "parser.y"
    {}
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 682 "parser.y"
    {}
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 694 "parser.y"
    {
		   /*if(strcmp_nocase($1->obj_type(), "fct") == 0 &&
		      $1->data_type() == 0)
		     {
		       err_msg("undefined the return type of function '",
			       $1->get_string(), "'");
		       $1->set_data("err");
		     }
		     table->insert($1->get_string(), $1);*/
		 }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 707 "parser.y"
    {
		   /*$$ = new TYPES("prd", 0);
		     $$->set_string($2);*/
		 }
    break;

  case 161:
/* Line 1787 of yacc.c  */
#line 712 "parser.y"
    {/*
		   $$ = new TYPES("prd", 0);
		   //$$->set_string($2);
		   //$$->set_list($4);
		  */
		 }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 719 "parser.y"
    {
		   /*TYPES *mt = table->lookup($5);
		   if(mt == 0)
		     $$ = new TYPES("fct", 0);
		   else	
		     {
		       $$ = new TYPES1(mt->get_ters(), mt);
		       $$->set_obj("fct");
		     }
		   $$->set_string($3);
		   delete mt;*/
		 }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 733 "parser.y"
    {
		   /*TYPES *mt = table->lookup($8->front());
		   if(mt == 0)
		     $$ = new TYPES("fct", 0);
		   else
		     {
		       $$ = new TYPES1(mt->get_ters(), mt);
		       $$->set_obj("fct");
		     }
		   $$->set_string($3);
		   //$$->set_list($5);
		   delete mt;*/
		 }
    break;

  case 167:
/* Line 1787 of yacc.c  */
#line 754 "parser.y"
    {
		   /*$$ = $1;
		   table->new_scope($$->get_string(), "function");
		   TYPELIST *l = $1->get_list();
		   l->iteration_init();
		   while(l->end() == false)
		     {	
		       table->insert(l->value().first, l->value().second);
		       l->next();	
		       }*/
		 }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 769 "parser.y"
    {
		   //table->delete_scope();
		 }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 774 "parser.y"
    {
		   //table->delete_scope();
		 }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 784 "parser.y"
    {}
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 786 "parser.y"
    {}
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 789 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 790 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].strlit); }
    break;

  case 179:
/* Line 1787 of yacc.c  */
#line 797 "parser.y"
    {}
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 808 "parser.y"
    {}
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 818 "parser.y"
    {
		   /*if(strcmp_nocase($4->obj_type(), "uk") == 0)
		     $4->set_obj("typ");

		   $4->set_data($2->label);
		   table->insert($2->label, $4);
		   if($4->get_grp() == TYPES::Physical)
		     {
		       TYPELIST *l = $4->get_list();
		       TYPES *ty = new TYPES("uk", $2->label);
		       l->iteration_init();
		       l->next();
		       l->next();		
		       while(l->end() == false)
			 {
			   const TYPES *tmp = l->value().second;
			   table->insert(tmp->get_string(), ty);
			   l->next();
			 }	
		       delete ty;
		       }*/
		 }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 843 "parser.y"
    {
		   /*TYPES *t = new TYPES("typ", $2->label);
		   table->insert($2->label, t);
		   delete t;*/
		 }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 850 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 851 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 852 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 853 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 857 "parser.y"
    {
		   //$$ = $1;
		 }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 860 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 862 "parser.y"
    {
		   /*$$ = new TYPES;
		   $$->set_grp(TYPES::Physical);
		   $$->set_list($1);*/
		 }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 869 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 870 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 887 "parser.y"
    {}
    break;

  case 211:
/* Line 1787 of yacc.c  */
#line 890 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(2) - (7)].str_list)->begin(); b!=(yyvsp[(2) - (7)].str_list)->end();b++){
		     string LL = Tolower(*b);
		     makeconstantdecl(action(LL), (yyvsp[(6) - (7)].types)->get_int());
		   }
		 }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 899 "parser.y"
    { (yyval.str_list) = new list<string>;  (yyval.str_list)->push_back((yyvsp[(1) - (1)].actionptr)->label); }
    break;

  case 213:
/* Line 1787 of yacc.c  */
#line 901 "parser.y"
    { (yyval.str_list) = (yyvsp[(1) - (3)].str_list); (yyval.str_list)->push_back((yyvsp[(3) - (3)].actionptr)->label); }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 906 "parser.y"
    {
		   list<string> *sufficies = new list<string>;
		   int type(IN);
		   string assigned((yyvsp[(6) - (8)].types)->get_string());
		   delayADT myBounds;
		   int rise_min(0), rise_max(0), fall_min(-1), fall_max(-1);
		   if("init_channel"==assigned){
		     sufficies->push_back(SEND_SUFFIX);
		     sufficies->push_back(RECEIVE_SUFFIX);
		     TYPELIST arguments((yyvsp[(6) - (8)].types)->get_list());
		     TYPELIST::iterator i(arguments.begin());
		     while(i!=arguments.end()){
		       if("timing"==i->second.get_string()){
			 TYPELIST bounds(i->second.get_list());
			 TYPELIST::iterator j(bounds.begin());
			 int pos(1);
			 while(j != bounds.end()){
			   string formal(j->first);
			   int actual(j->second.get_int());
			   if("rise_min"==formal || "lower"==formal ||
			      formal.empty() && 1==pos){
			     rise_min = actual;
			   }
			   else if("rise_max"==formal || "upper"==formal ||
				   formal.empty() && 2==pos){
			     rise_max = actual;
			   }
			   else if("fall_min"==formal || "lower"==formal ||
				   formal.empty() && 3==pos){
			     fall_min = actual;
			   }
			   else if("fall_max"==formal || "upper"==formal ||
				   formal.empty() && 4==pos){
			     fall_max = actual;
			   }
			   j++;
			   pos++;
			 }
		       }
		       i++;
		     }
		   }
		   else if("active"==assigned || "passive"==assigned){
		     err_msg(assigned,
			     " doesn't make sense in signal declaration");
		   }
		   else{
		     sufficies->push_back("" );
		     if((yyvsp[(8) - (8)].C_str) == string("out")){
		       type=OUT|ISIGNAL;
		     }
		   }
		   if(fall_min<0){
		     fall_min = rise_min;
		   }
		   if(fall_max<0){
		     fall_max = rise_max;
		   }
		   assigndelays(&myBounds,
				rise_min,rise_max,NULL,
				fall_min,fall_max,NULL);
		   int init = 0;
		   if((yyvsp[(6) - (8)].types)->get_string() == "'1'"){
		     init = 1;
		   }
		   for(list<string>::iterator b=(yyvsp[(2) - (8)].str_list)->begin(); b!=(yyvsp[(2) - (8)].str_list)->end();b++){
		     list<string>::iterator suffix=sufficies->begin();
		     signals[cur_entity][Tolower(*b)] = type;
		     while ( suffix != sufficies->end() ) {
		       string LL = Tolower(*b) + *suffix;
		       makebooldecl(type, action(LL), init, &myBounds);
		       if(!table->insert(LL, make_pair((string)(yyvsp[(8) - (8)].C_str),(TYPES*)0))){
			 err_msg("multiple declaration of signal '",*b,"'");
		       }
		       suffix++;
		     }
		     //              if(type==OUT)
		     tel_tb->add_signals(*b, type);
		       //else
		       //tel_tb->add_signals(*b, IN);
		   }
		   delete sufficies;
		 }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 995 "parser.y"
    { (yyval.C_str)= "out"; }
    break;

  case 219:
/* Line 1787 of yacc.c  */
#line 996 "parser.y"
    { (yyval.C_str)= "in"; }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 997 "parser.y"
    { (yyval.C_str)= "out"; }
    break;

  case 221:
/* Line 1787 of yacc.c  */
#line 1002 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(3) - (6)].str_list)->begin(); b!=(yyvsp[(3) - (6)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"var",(TYPES*)0))){
		       err_msg("multiple declaration of variable '",*b,"'");
		     }
		   }
		   delete (yyvsp[(3) - (6)].str_list);
		 }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 1012 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(3) - (8)].str_list)->begin(); b!=(yyvsp[(3) - (8)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"var",(TYPES*)0))){
		       err_msg("multiple declaration of variable '",*b,"'");
		     }
		   }
		   delete (yyvsp[(3) - (8)].str_list);
		   delete (yyvsp[(7) - (8)].types);
		 }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 1037 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(2) - (6)].str_list)->begin(); b!=(yyvsp[(2) - (6)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   TERstructADT x;
		   actionADT a = action(*((yyvsp[(2) - (6)].str_list)->begin()));
		   a->type = CONT + VAR + DUMMY;
		   /* TODO: Support span */
		   a->linitval = atoi((yyvsp[(5) - (6)].types)->theExpression.c_str());
		   a->uinitval = atoi((yyvsp[(5) - (6)].types)->theExpression.c_str());
		   x = TERS(a,FALSE,0,0,FALSE);
		   tel_tb->insert(*((yyvsp[(2) - (6)].str_list)->begin()), x);

		   if (EXPANDED_RATE_NETS==0) {
		     TERstructADT xdot;
		     string dollar2 = (*((yyvsp[(2) - (6)].str_list)->begin()));
		     actionADT adot = action((dollar2 + "dot").c_str());
		     adot->type = CONT + VAR + DUMMY;
		     adot->linitval = 0;//atoi($5->theExpression.c_str());
		     adot->uinitval = 0;//atoi($5->theExpression.c_str());
		     xdot = TERS(adot,FALSE,0,0,FALSE);
		     tel_tb->insert((dollar2 + "dot").c_str(), xdot);
		   }
		   
		   delete (yyvsp[(2) - (6)].str_list);
		 }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 1066 "parser.y"
    {
		 /*for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;*/
		 }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 1075 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(2) - (4)].str_list)->begin(); b!=(yyvsp[(2) - (4)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[(2) - (4)].str_list);
		 }
    break;

  case 229:
/* Line 1787 of yacc.c  */
#line 1084 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(2) - (4)].str_list)->begin(); b!=(yyvsp[(2) - (4)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[(2) - (4)].str_list);
		 }
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 1093 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(2) - (5)].str_list)->begin(); b!=(yyvsp[(2) - (5)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[(2) - (5)].str_list);
		 }
    break;

  case 231:
/* Line 1787 of yacc.c  */
#line 1103 "parser.y"
    {
		   for(list<string>::iterator b=(yyvsp[(2) - (6)].str_list)->begin(); b!=(yyvsp[(2) - (6)].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[(2) - (6)].str_list);
		 }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 1113 "parser.y"
    {(yyval.str_list) = (yyvsp[(1) - (4)].str_list);}
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 1121 "parser.y"
    {(yyval.str_list) = (yyvsp[(1) - (4)].str_list);}
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 1137 "parser.y"
    {}
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 1139 "parser.y"
    {}
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 1151 "parser.y"
    {}
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 1155 "parser.y"
    {}
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 1159 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 250:
/* Line 1787 of yacc.c  */
#line 1160 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].charlit); }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 1161 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].strlit); }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 1164 "parser.y"
    {}
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 1165 "parser.y"
    {}
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 1181 "parser.y"
    {}
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 1185 "parser.y"
    {}
    break;

  case 266:
/* Line 1787 of yacc.c  */
#line 1200 "parser.y"
    {
		   parsing_stack.push(make_pair(cur_entity, tel_tb));
		   cur_entity = (yyvsp[(2) - (2)].actionptr)->label;
		   tel_tb = 0;
		   table->push();
		   if(open_entity_lst->find((yyvsp[(2) - (2)].actionptr)->label).empty()){
		     //cout << $2->label<< "   " << (*name_mapping)[$2->label] << endl;
		     if(switch_buffer((*name_mapping)[(yyvsp[(2) - (2)].actionptr)->label] + ".vhd")){
		       new_action_table(cur_entity.c_str());
		       new_event_table(cur_entity.c_str());
		       new_rule_table(cur_entity.c_str());
		       new_conflict_table(cur_entity.c_str());
		     }
		   }
		 }
    break;

  case 267:
/* Line 1787 of yacc.c  */
#line 1216 "parser.y"
    {
		   cur_entity = parsing_stack.top().first;
		   tel_tb = parsing_stack.top().second;
		   parsing_stack.pop();
		   table->pop();
		   new_action_table(cur_entity.c_str());
		   new_event_table(cur_entity.c_str());
		   new_rule_table(cur_entity.c_str());
		   new_conflict_table(cur_entity.c_str());
		 }
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 1227 "parser.y"
    {
		   if(open_entity_lst->find(Tolower((yyvsp[(2) - (7)].actionptr)->label)).empty())
		     err_msg("error: component ",(yyvsp[(2) - (7)].actionptr)->label," not found");
		
		   if((yyvsp[(7) - (7)].C_str) && strcmp_nocase((yyvsp[(2) - (7)].actionptr)->label, (yyvsp[(7) - (7)].C_str))==false)
		     err_msg("warning: '", (yyvsp[(7) - (7)].C_str),
			     "' not declared as a component name");
		   if ((yyvsp[(6) - (7)].str_bl_lst)) delete (yyvsp[(6) - (7)].str_bl_lst);
		 }
    break;

  case 269:
/* Line 1787 of yacc.c  */
#line 1238 "parser.y"
    { (yyval.str_bl_lst) = 0; }
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 1239 "parser.y"
    { (yyval.str_bl_lst) = 0; }
    break;

  case 271:
/* Line 1787 of yacc.c  */
#line 1241 "parser.y"
    { (yyval.str_bl_lst) = (yyvsp[(2) - (2)].str_bl_lst); }
    break;

  case 272:
/* Line 1787 of yacc.c  */
#line 1243 "parser.y"
    { (yyval.str_bl_lst) = (yyvsp[(3) - (3)].str_bl_lst); }
    break;

  case 275:
/* Line 1787 of yacc.c  */
#line 1251 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 276:
/* Line 1787 of yacc.c  */
#line 1253 "parser.y"
    { (yyval.C_str) = (yyvsp[(3) - (4)].actionptr)->label; }
    break;

  case 277:
/* Line 1787 of yacc.c  */
#line 1257 "parser.y"
    {
		  yyerror("Attributes not supported.");
		  /*
		  TYPES *t= search( table, $4->label );
		  TYPES *temp= new TYPES( "attribute", t->datatype() );
		  table->add_symbol( $2->label,temp );
		  delete t,temp;
		  */
		}
    break;

  case 278:
/* Line 1787 of yacc.c  */
#line 1269 "parser.y"
    {
		  yyerror("Attributes not supported.");
		  //Type *t= search( table, $2->label );
		    //delete t;
		  }
    break;

  case 286:
/* Line 1787 of yacc.c  */
#line 1290 "parser.y"
    {   //TYPES *t= search( table, $1->label );
		     //delete t;
                 }
    break;

  case 287:
/* Line 1787 of yacc.c  */
#line 1294 "parser.y"
    {}
    break;

  case 288:
/* Line 1787 of yacc.c  */
#line 1296 "parser.y"
    {}
    break;

  case 311:
/* Line 1787 of yacc.c  */
#line 1309 "parser.y"
    { }
    break;

  case 315:
/* Line 1787 of yacc.c  */
#line 1318 "parser.y"
    {   //TYPES *t= search( table, $1->label );
		     //delete t;
		   }
    break;

  case 316:
/* Line 1787 of yacc.c  */
#line 1322 "parser.y"
    {   //TYPES *t= search( table, $3->label );
		     //delete t;
		   }
    break;

  case 320:
/* Line 1787 of yacc.c  */
#line 1330 "parser.y"
    {}
    break;

  case 321:
/* Line 1787 of yacc.c  */
#line 1334 "parser.y"
    {}
    break;

  case 322:
/* Line 1787 of yacc.c  */
#line 1336 "parser.y"
    {  /* FILE *temp= NULL;//fopen(strcat($2, ".sim"),"r");
		     if( temp )
		         printf("cannot find the configuration");*/
		   }
    break;

  case 324:
/* Line 1787 of yacc.c  */
#line 1343 "parser.y"
    { (yyval.str_str_l)= (yyvsp[(4) - (5)].str_str_l); }
    break;

  case 325:
/* Line 1787 of yacc.c  */
#line 1344 "parser.y"
    { (yyval.str_str_l)= (yyvsp[(4) - (5)].str_str_l); }
    break;

  case 328:
/* Line 1787 of yacc.c  */
#line 1349 "parser.y"
    {(yyval.str_str_l) = (yyvsp[(2) - (2)].str_str_l);}
    break;

  case 330:
/* Line 1787 of yacc.c  */
#line 1357 "parser.y"
    {   //TYPES *t= search( table, $3->label );
		     //delete t;
		   }
    break;

  case 334:
/* Line 1787 of yacc.c  */
#line 1368 "parser.y"
    {}
    break;

  case 335:
/* Line 1787 of yacc.c  */
#line 1370 "parser.y"
    {}
    break;

  case 337:
/* Line 1787 of yacc.c  */
#line 1378 "parser.y"
    { delete (yyvsp[(2) - (3)].str_list); }
    break;

  case 340:
/* Line 1787 of yacc.c  */
#line 1388 "parser.y"
    {}
    break;

  case 344:
/* Line 1787 of yacc.c  */
#line 1397 "parser.y"
    {}
    break;

  case 346:
/* Line 1787 of yacc.c  */
#line 1402 "parser.y"
    {}
    break;

  case 348:
/* Line 1787 of yacc.c  */
#line 1410 "parser.y"
    {
		   /*$$ = new TYPES;
		   $$->set_grp(TYPES::Enumeration);
		   $$->set_list($2);
		   delete $2;*/
		 }
    break;

  case 349:
/* Line 1787 of yacc.c  */
#line 1419 "parser.y"
    { /*
		   TYPES *ty = new TYPES;
		   $$ = new TYPELIST($1, ty);*/
		 }
    break;

  case 350:
/* Line 1787 of yacc.c  */
#line 1424 "parser.y"
    {/*
		   $$ = $1;
		   TYPES *ty = new TYPES;
		   $$->insert($3, ty);*/
		 }
    break;

  case 351:
/* Line 1787 of yacc.c  */
#line 1432 "parser.y"
    {  (yyval.C_str) = (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 352:
/* Line 1787 of yacc.c  */
#line 1434 "parser.y"
    {  (yyval.C_str) = (yyvsp[(1) - (1)].charlit); }
    break;

  case 353:
/* Line 1787 of yacc.c  */
#line 1438 "parser.y"
    {}
    break;

  case 354:
/* Line 1787 of yacc.c  */
#line 1441 "parser.y"
    { (yyval.typelist) = (yyvsp[(2) - (2)].typelist); }
    break;

  case 355:
/* Line 1787 of yacc.c  */
#line 1444 "parser.y"
    { yyerror("Attributes not supported."); }
    break;

  case 356:
/* Line 1787 of yacc.c  */
#line 1446 "parser.y"
    {
		   /*if(strcmp_nocase($1->data_type(), $3->data_type()))
		     err_msg("different types used on sides of ", $2);
		   */
		   (yyval.typelist) = new TYPELIST(make_pair(string(), (yyvsp[(1) - (3)].types)));
		   (yyval.typelist)->insert((yyvsp[(2) - (3)].C_str), (yyvsp[(3) - (3)].types));
		 }
    break;

  case 357:
/* Line 1787 of yacc.c  */
#line 1455 "parser.y"
    { (yyval.C_str) = "'to'"; }
    break;

  case 358:
/* Line 1787 of yacc.c  */
#line 1456 "parser.y"
    { (yyval.C_str) = "'downto'"; }
    break;

  case 359:
/* Line 1787 of yacc.c  */
#line 1460 "parser.y"
    {
		   //$$ = new TYPELIST(0, $3);
		   //$$->combine($4);
		 }
    break;

  case 360:
/* Line 1787 of yacc.c  */
#line 1466 "parser.y"
    {
		   //$$ = new TYPELIST(0, $3);
		   //$$->combine($4);
		 }
    break;

  case 361:
/* Line 1787 of yacc.c  */
#line 1472 "parser.y"
    { (yyval.typelist) = 0; }
    break;

  case 362:
/* Line 1787 of yacc.c  */
#line 1474 "parser.y"
    {
		   /*if($1)
		     {
		       $$ = $1;
		       $$->insert(0, $2);
		     }
		   else
		   $$ =  new TYPELIST(0, $2); */
		 }
    break;

  case 363:
/* Line 1787 of yacc.c  */
#line 1486 "parser.y"
    {
		  //$$ = new TYPES;
		  //$$->set_string($1->label);
		}
    break;

  case 364:
/* Line 1787 of yacc.c  */
#line 1493 "parser.y"
    {		
		  //$$ = new TYPES("uk", "uk");
		  //$$->set_string($1->label);
		}
    break;

  case 365:
/* Line 1787 of yacc.c  */
#line 1500 "parser.y"
    {
		  //$$ = $1;
		  //$$->set_base($$->data_type());
		  //$$->set_data("vec");
		  //$$->set_grp(TYPES::Array);
		}
    break;

  case 366:
/* Line 1787 of yacc.c  */
#line 1507 "parser.y"
    {
		  //$$ = $1;
		  //$$->set_base($$->data_type());
		  //$$->set_data("vec");
		}
    break;

  case 367:
/* Line 1787 of yacc.c  */
#line 1515 "parser.y"
    {
		   /*  $$ = $6;
		   $$->set_base($$->data_type());
		   $$->set_list($3);
		   delete $3;*/
		 }
    break;

  case 368:
/* Line 1787 of yacc.c  */
#line 1524 "parser.y"
    {
		   /*$$ = $4;
		   $2->combine($$->get_list());
		   $$->set_list($2);
		   $$->set_base($$->data_type());
		   $$->set_grp(TYPES::Array);
		   delete $2;*/
		 }
    break;

  case 370:
/* Line 1787 of yacc.c  */
#line 1535 "parser.y"
    {(yyval.typelist) = (yyvsp[(1) - (2)].typelist); (yyval.typelist)->combine((yyvsp[(2) - (2)].typelist)); }
    break;

  case 371:
/* Line 1787 of yacc.c  */
#line 1539 "parser.y"
    {
		   /*TYPES *ty = table->lookup($1->front());
		   if(ty == 0)
		     {
		       table->print();
		       err_msg("'", $1->front(), "' undeclared.");
		       ty = new TYPES("err", "err");
		     }
		   else if(strcmp_nocase(ty->obj_type(), "typ"))
		     {
		       err_msg("'", $1->front(), "' is not a type name.");
		       ty->set_obj("err");
		       ty->set_data("err");
		     }
		   else if(ty->get_grp() != TYPES::Integer &&
			   ty->get_grp() != TYPES::Enumeration)
		     {
		       err_msg("integers or enumerations required for ",
			       "subscripts of arrays");
		       ty->set_obj("err");
		       ty->set_data("err");
		     }
		   TYPES lower; lower.set_int(0);
		   TYPES upper; upper.set_int(0);
		   TYPELIST tl(0, &lower);
		   tl.insert(0, &upper);
		   ty->set_list(&tl);
		   $$ = new TYPELIST(0, ty);
		   delete ty;*/
		 }
    break;

  case 372:
/* Line 1787 of yacc.c  */
#line 1572 "parser.y"
    { (yyval.typelist) = (yyvsp[(2) - (3)].typelist); }
    break;

  case 373:
/* Line 1787 of yacc.c  */
#line 1576 "parser.y"
    {
		   (yyval.typelist) = new TYPELIST(make_pair(string(), (yyvsp[(1) - (1)].types)));
		 }
    break;

  case 374:
/* Line 1787 of yacc.c  */
#line 1580 "parser.y"
    {
		   (yyval.typelist) = (yyvsp[(1) - (3)].typelist);
		   (yyval.typelist)->insert(string(), (yyvsp[(3) - (3)].types));
		 }
    break;

  case 375:
/* Line 1787 of yacc.c  */
#line 1587 "parser.y"
    {
		   //$$= new TYPES("uk", "RECORD","RECORD");
		 }
    break;

  case 376:
/* Line 1787 of yacc.c  */
#line 1591 "parser.y"
    {
		   //$$= new TYPES("uk", "RECORD","RECORD");
		 }
    break;

  case 379:
/* Line 1787 of yacc.c  */
#line 1601 "parser.y"
    {}
    break;

  case 380:
/* Line 1787 of yacc.c  */
#line 1604 "parser.y"
    {}
    break;

  case 381:
/* Line 1787 of yacc.c  */
#line 1607 "parser.y"
    {}
    break;

  case 382:
/* Line 1787 of yacc.c  */
#line 1608 "parser.y"
    {}
    break;

  case 384:
/* Line 1787 of yacc.c  */
#line 1615 "parser.y"
    {
 		   /*if(strcmp_nocase($4->obj_type(), "uk") == 0)
		     $4->set_obj("typ");
		
		   //$4->set_base($4->data_type());
		   //$4->set_data($2->label);
		   //cout<< $2->label<< endl;
		   if(table->insert($2->label, $4) == false)
		     {
		       err_msg("re-declaration of '", $2->label, "'");
		     }
		     delete $4;*/
		 }
    break;

  case 385:
/* Line 1787 of yacc.c  */
#line 1633 "parser.y"
    {}
    break;

  case 386:
/* Line 1787 of yacc.c  */
#line 1635 "parser.y"
    {}
    break;

  case 387:
/* Line 1787 of yacc.c  */
#line 1637 "parser.y"
    {}
    break;

  case 388:
/* Line 1787 of yacc.c  */
#line 1639 "parser.y"
    {}
    break;

  case 389:
/* Line 1787 of yacc.c  */
#line 1641 "parser.y"
    {}
    break;

  case 391:
/* Line 1787 of yacc.c  */
#line 1652 "parser.y"
    {
		   /*$$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		     $$->set_string($1->front().c_str());*/
		 }
    break;

  case 392:
/* Line 1787 of yacc.c  */
#line 1662 "parser.y"
    { /*
		   $$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		   else
		     {
		       $$->set_list($2);
		       }*/
		 }
    break;

  case 393:
/* Line 1787 of yacc.c  */
#line 1674 "parser.y"
    {}
    break;

  case 394:
/* Line 1787 of yacc.c  */
#line 1678 "parser.y"
    {/*
		   $$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		   else if(strcmp_nocase($$->obj_type(), "typ"))
		     {
		       err_msg("'", $1->front(), "' is not a type name");
		       $$->set_obj("err");
		       $$->set_data("err");
		     }
		     $$->set_list($2);*/
		 }
    break;

  case 395:
/* Line 1787 of yacc.c  */
#line 1694 "parser.y"
    {
		   //$$= search( table, $1->label );
		 }
    break;

  case 396:
/* Line 1787 of yacc.c  */
#line 1698 "parser.y"
    { /*
		   $$ = table->lookup($2->front());
		   if($$ == 0)
		     {
		       err_msg("'", $2->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		       }*/
		 }
    break;

  case 397:
/* Line 1787 of yacc.c  */
#line 1707 "parser.y"
    {
		   //$$= search( table, $1->label );
		 }
    break;

  case 398:
/* Line 1787 of yacc.c  */
#line 1713 "parser.y"
    {
		   (yyval.types) = new TYPES;
		   (yyval.types)->set_list((yyvsp[(1) - (1)].typelist));
		   (yyvsp[(1) - (1)].typelist)->iteration_init();
		   (yyval.types)->set_data((yyvsp[(1) - (1)].typelist)->value().second.data_type());
		 }
    break;

  case 400:
/* Line 1787 of yacc.c  */
#line 1723 "parser.y"
    {
		   (yyval.typelist) = new TYPELIST(make_pair(string(), (yyvsp[(1) - (3)].types)));
		   (yyval.typelist)->front().second.theExpression = (yyvsp[(1) - (3)].types)->theExpression;
		   (yyval.typelist)->insert((yyvsp[(2) - (3)].C_str), (yyvsp[(3) - (3)].types));
     		 }
    break;

  case 401:
/* Line 1787 of yacc.c  */
#line 1729 "parser.y"
    {
		   (yyval.typelist)= new TYPELIST(make_pair(string(), (yyvsp[(1) - (1)].types)));
		 }
    break;

  case 403:
/* Line 1787 of yacc.c  */
#line 1740 "parser.y"
    {}
    break;

  case 404:
/* Line 1787 of yacc.c  */
#line 1741 "parser.y"
    {}
    break;

  case 405:
/* Line 1787 of yacc.c  */
#line 1747 "parser.y"
    {}
    break;

  case 409:
/* Line 1787 of yacc.c  */
#line 1761 "parser.y"
    {}
    break;

  case 410:
/* Line 1787 of yacc.c  */
#line 1763 "parser.y"
    {}
    break;

  case 411:
/* Line 1787 of yacc.c  */
#line 1764 "parser.y"
    {}
    break;

  case 412:
/* Line 1787 of yacc.c  */
#line 1768 "parser.y"
    {}
    break;

  case 413:
/* Line 1787 of yacc.c  */
#line 1770 "parser.y"
    {}
    break;

  case 414:
/* Line 1787 of yacc.c  */
#line 1779 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 415:
/* Line 1787 of yacc.c  */
#line 1780 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 416:
/* Line 1787 of yacc.c  */
#line 1781 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 417:
/* Line 1787 of yacc.c  */
#line 1782 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 418:
/* Line 1787 of yacc.c  */
#line 1783 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 419:
/* Line 1787 of yacc.c  */
#line 1784 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 420:
/* Line 1787 of yacc.c  */
#line 1785 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 421:
/* Line 1787 of yacc.c  */
#line 1786 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 422:
/* Line 1787 of yacc.c  */
#line 1791 "parser.y"
    { tel_tb->insert((yyvsp[(1) - (7)].C_str), (yyvsp[(6) - (7)].TERSptr)); }
    break;

  case 423:
/* Line 1787 of yacc.c  */
#line 1795 "parser.y"
    { tel_tb->insert((yyvsp[(1) - (8)].C_str), (yyvsp[(7) - (8)].TERSptr)); }
    break;

  case 426:
/* Line 1787 of yacc.c  */
#line 1802 "parser.y"
    {}
    break;

  case 433:
/* Line 1787 of yacc.c  */
#line 1815 "parser.y"
    {}
    break;

  case 434:
/* Line 1787 of yacc.c  */
#line 1816 "parser.y"
    { }
    break;

  case 435:
/* Line 1787 of yacc.c  */
#line 1817 "parser.y"
    { }
    break;

  case 442:
/* Line 1787 of yacc.c  */
#line 1829 "parser.y"
    { (yyval.TERSptr)= TERSempty(); }
    break;

  case 443:
/* Line 1787 of yacc.c  */
#line 1831 "parser.y"
    { (yyval.TERSptr)= TERScompose((yyvsp[(1) - (2)].TERSptr), (yyvsp[(2) - (2)].TERSptr), "||"); }
    break;

  case 444:
/* Line 1787 of yacc.c  */
#line 1833 "parser.y"
    { (yyval.TERSptr)= TERScompose((yyvsp[(1) - (2)].TERSptr), (yyvsp[(2) - (2)].TERSptr), "||");}
    break;

  case 445:
/* Line 1787 of yacc.c  */
#line 1836 "parser.y"
    {}
    break;

  case 460:
/* Line 1787 of yacc.c  */
#line 1851 "parser.y"
    {}
    break;

  case 469:
/* Line 1787 of yacc.c  */
#line 1864 "parser.y"
    {
		  table->new_tb();
		  processChannels.clear();
		}
    break;

  case 470:
/* Line 1787 of yacc.c  */
#line 1870 "parser.y"
    {
		  map<string,int>::iterator m(processChannels.begin());
		  while(m != processChannels.end()){
		    string name(m->first);
		    int type(m->second);
		    if(type&ACTIVE  && type&SENT||
		       type&PASSIVE && type&RECEIVED){
		      type |= PUSH;
		    }
		    if(type&ACTIVE  && type&RECEIVED ||
		       type&PASSIVE && type&SENT    ){
		      type |= PULL;
		    }
		    signals[cur_entity][name] |= type;
		    action(name+SEND_SUFFIX+"+")->type |= type;
		    action(name+SEND_SUFFIX+"-")->type |= type;
		    action(name+RECEIVE_SUFFIX+"+")->type |= type;
		    action(name+RECEIVE_SUFFIX+"-")->type |= type;
		    m++;
		  }
		  table->delete_tb();
		  (yyval.TERSptr) = 0;
		  if((yyvsp[(6) - (9)].TERSptr)){
		    (yyval.TERSptr) = TERSmakeloop((yyvsp[(6) - (9)].TERSptr));
		    (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		    char s[255];
		    strcpy(s, rmsuffix((yyvsp[(1) - (9)].C_str)));
		    if (1) {
		      printf("Compiled process %s\n",s);
		      fprintf(lg,"Compiled process %s\n",s);
		    } 
		    else
		      emitters(outpath, s, (yyval.TERSptr));
		  }
		  if(tel_tb->insert(((yyvsp[(1) - (9)].C_str)), (yyval.TERSptr)) == false)
		    err_msg("duplicate process label '", (yyvsp[(1) - (9)].C_str), "'");
		}
    break;

  case 473:
/* Line 1787 of yacc.c  */
#line 1914 "parser.y"
    {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   (yyval.C_str) = copy_string(n);
		   //TYPES *t = new TYPES("prc", "uk");
		   //table->new_scope($$, "process", t);
		 }
    break;

  case 474:
/* Line 1787 of yacc.c  */
#line 1923 "parser.y"
    {
		   (yyval.C_str) = copy_string((yyvsp[(1) - (2)].C_str));
		   //TYPES *t = new TYPES("prc", "uk");
		   //table->new_scope($$, "process", t);
		 }
    break;

  case 475:
/* Line 1787 of yacc.c  */
#line 1929 "parser.y"
    {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   (yyval.C_str) = copy_string(n);
		 }
    break;

  case 476:
/* Line 1787 of yacc.c  */
#line 1936 "parser.y"
    { (yyval.C_str) = copy_string((yyvsp[(1) - (3)].C_str)); }
    break;

  case 477:
/* Line 1787 of yacc.c  */
#line 1939 "parser.y"
    { }
    break;

  case 480:
/* Line 1787 of yacc.c  */
#line 1945 "parser.y"
    {
		   __sensitivity_list__= 1;
		 }
    break;

  case 481:
/* Line 1787 of yacc.c  */
#line 1951 "parser.y"
    {
		   //TYPES *t= search( table, $1->getstring());
                   //table->add_symbol( $1->getstring(), t );
                   //delete t;
                 }
    break;

  case 482:
/* Line 1787 of yacc.c  */
#line 1957 "parser.y"
    {
		   //TYPES *t= search( table, $3->getstring());
		   //table->add_symbol($3->getstring(), t );
                   //delete t;
		 }
    break;

  case 485:
/* Line 1787 of yacc.c  */
#line 1968 "parser.y"
    {}
    break;

  case 496:
/* Line 1787 of yacc.c  */
#line 1979 "parser.y"
    {}
    break;

  case 499:
/* Line 1787 of yacc.c  */
#line 1984 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 500:
/* Line 1787 of yacc.c  */
#line 1986 "parser.y"
    {
		   if((yyvsp[(2) - (2)].ty_tel2)->second)
		     (yyval.TERSptr) = TERScompose((yyvsp[(1) - (2)].TERSptr), TERSrename((yyvsp[(1) - (2)].TERSptr),(yyvsp[(2) - (2)].ty_tel2)->second),";");
		   else
		     (yyval.TERSptr) = (yyvsp[(1) - (2)].TERSptr);
		   delete (yyvsp[(2) - (2)].ty_tel2);
		 }
    break;

  case 502:
/* Line 1787 of yacc.c  */
#line 1997 "parser.y"
    {}
    break;

  case 504:
/* Line 1787 of yacc.c  */
#line 1999 "parser.y"
    {}
    break;

  case 505:
/* Line 1787 of yacc.c  */
#line 2003 "parser.y"
    {
		//SymTab *temp= search1(table,$3->name2->label );
		//delete temp;
		
	      }
    break;

  case 506:
/* Line 1787 of yacc.c  */
#line 2009 "parser.y"
    {
		string comp_id = (yyvsp[(2) - (4)].types)->str();
		if(open_entity_lst->find(comp_id).empty()){
		  err_msg("error: component '",comp_id.c_str(), "' not found");
		  delete (yyvsp[(3) - (4)].str_str_l);
		  return 1;
		}
		else {
		  const tels_table& TT = open_entity_lst->find(comp_id,
							       string());
		  if(TT.empty()){
		    err_msg("error: no architecture defined for '",
			    comp_id, "'");
		    return 1;
		  }		
	
		  tels_table* rpc = new tels_table(TT);

		  const list<pair<string,int> >& tmp = rpc->signals1();	
		  if(tmp.size() != (yyvsp[(3) - (4)].str_str_l)->size())
		    err_msg("incorrect port map of '",(yyvsp[(2) - (4)].types)->str(),"'");
		  else {
		    const vector<pair<string,int> > sig_v(tmp.begin(),
							  tmp.end());
		    const map<string,int>& sig_m = rpc->signals();
		    map<string,int>& ports = tel_tb->port_decl();
		    list<pair<string,string> >::size_type pos = 0;
		    map<string,string> port_maps;
		    list<pair<string,string> >::iterator BB;
		    for(BB = (yyvsp[(3) - (4)].str_str_l)->begin(); BB != (yyvsp[(3) - (4)].str_str_l)->end(); BB++, pos++){
		      string formal(BB->first);
		      string actual(BB->second);
		      if(formal.empty() && pos < sig_v.size()){
			formal = sig_v[pos].first;
		      }
		      if(sig_m.find(formal) == sig_m.end()){
			err_msg("undeclared formal signal '", formal,"'");
		      }
		      else{
			int type(sig_m.find(formal)->second);
			if(signals[cur_entity].find(actual) ==
			   signals[cur_entity].end()){
			  if(ports.find(actual) ==   ports.end()){
			    err_msg("undeclared actual signal ", actual);
			  }
			  else{
			    ports[actual] |= type;
			  }
			}
			else{
			  const int use(ACTIVE|PASSIVE|SENT|RECEIVED);
			  int overlap(signals[cur_entity][actual] &type & use);
			  if(overlap & ACTIVE){
			    err_msg("Channel ",actual,
				    " is active on both sides!");
			  }
			  if(overlap & PASSIVE){
			    err_msg("Channel ",actual,
				    " is passive on both sides!");
			  }
			  if(overlap & SENT){
			    err_msg("Channel ",actual,
				    " has sends on both sides!");
			  }
			  if(overlap & RECEIVED){
			    err_msg("Channel ",actual,
				    " has receives on both sides!");
			  }
			  if(type&ACTIVE  && type&SENT||
			     type&PASSIVE && type&RECEIVED){
			    type |= PUSH;
			  }
			  if(type&ACTIVE  && type&RECEIVED ||
			     type&PASSIVE && type&SENT    ){
			    type |= PULL;
			  }
			  signals[cur_entity][actual] |= type;
			  action(actual+SEND_SUFFIX+"+")->type |= type;
			  action(actual+SEND_SUFFIX+"-")->type |= type;
			  action(actual+RECEIVE_SUFFIX+"+")->type |= type;
			  action(actual+RECEIVE_SUFFIX+"-")->type |= type;
			}
		      }
		      port_maps[formal]=actual;
		    }
		    my_list *netlist = new my_list(port_maps);
		    string s = (yyvsp[(2) - (4)].types)->str();
		    rpc->set_id((yyvsp[(1) - (4)].C_str));
		    rpc->set_type(comp_id);
		    rpc->instantiate(netlist, (yyvsp[(1) - (4)].C_str), s);
		    rpc->insert(port_maps);
		    if(tel_tb->insert(((yyvsp[(1) - (4)].C_str)), rpc) == false)
		      err_msg("duplicate component label '", (yyvsp[(1) - (4)].C_str), "'");
		    delete netlist;
		  }		
		  delete (yyvsp[(3) - (4)].str_str_l);
		}
	      }
    break;

  case 508:
/* Line 1787 of yacc.c  */
#line 2110 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (2)].types); }
    break;

  case 509:
/* Line 1787 of yacc.c  */
#line 2111 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (2)].types); }
    break;

  case 510:
/* Line 1787 of yacc.c  */
#line 2112 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (2)].types); }
    break;

  case 511:
/* Line 1787 of yacc.c  */
#line 2116 "parser.y"
    { 
		   char n[100];
		   sprintf(n, "ss%ld", PSC);
		   PSC++;
		   printf("Compiled assertion statement %s\n",n);
		   fprintf(lg,"Compiled assertion statement %s\n",n);
		   TERstructADT x;
		   x = TERSmakeloop((yyvsp[(1) - (1)].TERSptr));
		   x = TERSrepeat(x,";");
		   tel_tb->insert(n, x);
		 }
    break;

  case 512:
/* Line 1787 of yacc.c  */
#line 2127 "parser.y"
    { 
		 char n[100];
		 if ((yyvsp[(1) - (2)].C_str))
		   strcpy(n,(yyvsp[(1) - (2)].C_str));
		 else {
		   sprintf(n, "ss%ld", PSC);
		   PSC++;
		 }
		 printf("Compiled assertion statement %s\n",n);
		 fprintf(lg,"Compiled assertion statement %s\n",n);
		 TERstructADT x;
		 x = TERSmakeloop((yyvsp[(2) - (2)].TERSptr));
		 x = TERSrepeat(x,";");
		 tel_tb->insert(n, x);
	       }
    break;

  case 513:
/* Line 1787 of yacc.c  */
#line 2142 "parser.y"
    { }
    break;

  case 514:
/* Line 1787 of yacc.c  */
#line 2143 "parser.y"
    { }
    break;

  case 515:
/* Line 1787 of yacc.c  */
#line 2147 "parser.y"
    {
		   char id[200];
		   sprintf(id, "CS%ld", (unsigned long)(yyvsp[(1) - (1)].TERSptr));
		   tel_tb->insert((id), (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 516:
/* Line 1787 of yacc.c  */
#line 2153 "parser.y"
    { tel_tb->insert(((yyvsp[(1) - (2)].C_str)), (yyvsp[(2) - (2)].TERSptr)); }
    break;

  case 517:
/* Line 1787 of yacc.c  */
#line 2154 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 518:
/* Line 1787 of yacc.c  */
#line 2155 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 519:
/* Line 1787 of yacc.c  */
#line 2156 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 520:
/* Line 1787 of yacc.c  */
#line 2158 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 521:
/* Line 1787 of yacc.c  */
#line 2159 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 522:
/* Line 1787 of yacc.c  */
#line 2160 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 523:
/* Line 1787 of yacc.c  */
#line 2164 "parser.y"
    {
		   string ENC, EN_r, EN_f;
		   int LL = INFIN, UU = 0;
		   list<pair<TYPES,TYPES> >::iterator b;
		   for(b = (yyvsp[(4) - (5)].ty_ty_lst)->begin(); b != (yyvsp[(4) - (5)].ty_ty_lst)->end(); b++){
		     if(b->first.grp_id() != TYPES::Error &&
			b->second.grp_id() != TYPES::Error){
		       // Calculate the delay.
		       const TYPELIST *t = b->first.get_list();
		       if(b->first.str() != "unaffected" &&
			  t != 0 && t->empty() == false){
			 if(LL>t->front().second.get_int())
			   LL = t->front().second.get_int();
			 if(UU<t->back().second.get_int())
			   UU = t->back().second.get_int();
			 if (!(t->front().second.get_string().empty())) {
			   actionADT a= action(t->front().second.get_string());
			   LL = a->intval;
			 }
			 if (!(t->back().second.get_string().empty())) {
			   actionADT a = action(t->back().second.get_string());
			   UU = a->intval;
			 }
		       }
		       if(!ENC.size()){
			 if(b->first.str() != "unaffected"){
			   EN_r = logic(bool_relation(b->first.str(), "'1'"),
					b->second.str());
			   EN_f = logic(bool_relation(b->first.str(), "'0'"),
					b->second.str());
			 }
			 ENC = my_not(b->second.str());
		       }
		       else{
			 if(b->first.str() != "unaffected"){
			   EN_r = logic(EN_r,
					logic(logic(bool_relation(b->first.str(),
							     "'1'"),
						    b->second.str()),
					      ENC),
					"|");
			   EN_f = logic(EN_f,
					logic(logic(bool_relation(b->first.str(),
							     "'0'"),
						    b->second.str()),
					      ENC),
					"|");
			 }
			 ENC = logic(ENC, my_not(b->second.str()));
		       }
		     }
		   }		

		   string a_r = (yyvsp[(1) - (5)].types)->get_string() + '+';
		   string a_f = (yyvsp[(1) - (5)].types)->get_string() + '-';
		   actionADT a = action((yyvsp[(1) - (5)].types)->str() + '+');
		   telADT RR, FF, RC, FC;
		   string CC = '[' + EN_r + ']' + 'd';
		   RC = TERS(dummyE(), FALSE, 0,0, TRUE, CC.c_str());
		   RR = TERS(action((yyvsp[(1) - (5)].types)->str()+'+'),FALSE,LL,UU,TRUE);
		   CC = '[' + EN_f + ']' + 'd';
		   FC = TERS(dummyE(), FALSE, 0,0, TRUE, CC.c_str());
		   FF = TERS(action((yyvsp[(1) - (5)].types)->str()+'-'),FALSE,LL,UU,TRUE);
		   if (a->initial) {
		     (yyval.TERSptr) = TERScompose(TERScompose(FC, FF, ";"),
				      TERScompose(RC, RR, ";"), ";");
		   } else {
		     (yyval.TERSptr) = TERScompose(TERScompose(RC, RR, ";"),
				      TERScompose(FC, FF, ";"), ";");
		   }
		   (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		   (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		   //emitters(0, "tmp", $$);
		 }
    break;

  case 533:
/* Line 1787 of yacc.c  */
#line 2256 "parser.y"
    {
		   if((yyvsp[(1) - (2)].ty_ty_lst)) (yyval.ty_ty_lst) = (yyvsp[(1) - (2)].ty_ty_lst);
		   else   (yyval.ty_ty_lst) = (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		   TYPES ty("uk","bool");
		   ty.set_grp(TYPES::Enumeration);
		   ty.set_str("true");
		   (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[(2) - (2)].types),ty));
		 }
    break;

  case 534:
/* Line 1787 of yacc.c  */
#line 2265 "parser.y"
    {
		   if((yyvsp[(1) - (4)].ty_ty_lst))
		     (yyval.ty_ty_lst) = (yyvsp[(1) - (4)].ty_ty_lst);
		   else
		     (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		   if((yyvsp[(4) - (4)].types)->grp_id() != TYPES::Error &&
		      (yyvsp[(4) - (4)].types)->data_type() != string("bool")) {
		     err_msg("expect a boolean expression after 'when'");
		     (yyvsp[(4) - (4)].types)->set_grp(TYPES::Error);
		   }
		   (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[(2) - (4)].types), *(yyvsp[(4) - (4)].types)));
		 }
    break;

  case 535:
/* Line 1787 of yacc.c  */
#line 2279 "parser.y"
    { (yyval.ty_ty_lst) = 0; }
    break;

  case 536:
/* Line 1787 of yacc.c  */
#line 2281 "parser.y"
    {
		   if((yyvsp[(1) - (5)].ty_ty_lst))  (yyval.ty_ty_lst) = (yyvsp[(1) - (5)].ty_ty_lst);
		   else    (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		   if((yyvsp[(4) - (5)].types)->grp_id() != TYPES::Error &&
		      (yyvsp[(4) - (5)].types)->data_type() != string("bool")){
		     err_msg("expect a boolean expression after 'when'");
		     (yyvsp[(4) - (5)].types)->set_grp(TYPES::Error);
		   }
		   (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[(2) - (5)].types), *(yyvsp[(4) - (5)].types)));
		 }
    break;

  case 537:
/* Line 1787 of yacc.c  */
#line 2295 "parser.y"
    {
		   delete (yyvsp[(7) - (8)].ty_ty_lst);
		   /*telADT ret_t = TERSempty();
		   if($7){		
		     for(list<pair<TYPES*,TYPES*> >::iterator b = $7->begin();
		     b != $7->end(); b++){*/
		
		 }
    break;

  case 538:
/* Line 1787 of yacc.c  */
#line 2306 "parser.y"
    {
		   (yyval.ty_ty_lst) = 0;
		   if((yyvsp[(3) - (3)].ty_lst)){
		     if((yyvsp[(3) - (3)].ty_lst)->back().data_type() != "bool")
		       err_msg("expect a boolean expression after 'when'");
		     else{
		       (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		       TYPES ty("uk","bool");
		       ty.set_grp(TYPES::Enumeration);
		       list<string> str_l;
		       for(list<TYPES>::iterator b = (yyvsp[(3) - (3)].ty_lst)->begin();
			   b != (yyvsp[(3) - (3)].ty_lst)->end(); b++)
			 str_l.push_back(b->str());
		       ty.set_str(logic(str_l, "|"));
		       (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[(1) - (3)].types),ty));
		     }
		   }
		 }
    break;

  case 539:
/* Line 1787 of yacc.c  */
#line 2325 "parser.y"
    {
		   (yyval.ty_ty_lst) = 0;
		   if((yyvsp[(1) - (5)].ty_ty_lst) && (yyvsp[(5) - (5)].ty_lst)){
		     if((yyvsp[(5) - (5)].ty_lst)->back().data_type() != "bool"){
		       err_msg("expect a boolean expression after 'when'");
		       delete (yyvsp[(1) - (5)].ty_ty_lst); delete (yyvsp[(5) - (5)].ty_lst); (yyvsp[(1) - (5)].ty_ty_lst) = 0;
		     }
		     else{
		       TYPES ty("uk","bool");
		       ty.set_grp(TYPES::Enumeration);
		       list<string> str_l;
		       for(list<TYPES>::iterator b = (yyvsp[(5) - (5)].ty_lst)->begin();
			   b != (yyvsp[(5) - (5)].ty_lst)->end(); b++)
			 str_l.push_back(b->str());
		       ty.set_str(logic(str_l, "|"));
		       (yyvsp[(1) - (5)].ty_ty_lst)->push_back(make_pair(*(yyvsp[(3) - (5)].types),ty));
		     }
		   }
		   (yyval.ty_ty_lst) = (yyvsp[(1) - (5)].ty_ty_lst);
                 }
    break;

  case 540:
/* Line 1787 of yacc.c  */
#line 2352 "parser.y"
    {}
    break;

  case 541:
/* Line 1787 of yacc.c  */
#line 2356 "parser.y"
    {}
    break;

  case 542:
/* Line 1787 of yacc.c  */
#line 2360 "parser.y"
    {}
    break;

  case 544:
/* Line 1787 of yacc.c  */
#line 2365 "parser.y"
    {
		   /*TYPES *t= search( table, $1->label );
		     delete t;
		     SymTab *temp= table->header;
		     delete table;
		     table= temp;*/
		 }
    break;

  case 547:
/* Line 1787 of yacc.c  */
#line 2381 "parser.y"
    {}
    break;

  case 548:
/* Line 1787 of yacc.c  */
#line 2383 "parser.y"
    {}
    break;

  case 552:
/* Line 1787 of yacc.c  */
#line 2390 "parser.y"
    { /*
		   if( !($2.type->isBool() || $2.type->isError()) )
		     err_msg_3( "the 'if' should be followed a boolean",
				"expression", "");*/
		 }
    break;

  case 553:
/* Line 1787 of yacc.c  */
#line 2398 "parser.y"
    {}
    break;

  case 555:
/* Line 1787 of yacc.c  */
#line 2405 "parser.y"
    {
                   TERstructADT breakElem = 0;
                   if ((yyvsp[(3) - (5)].str)) {
		     breakElem = Guard(*(yyvsp[(3) - (5)].str));
		   }
		   //An assign statement
		   if ((yyvsp[(4) - (5)].TERSptr)) {
		     char n[100];
		     if ((yyvsp[(1) - (5)].C_str))
		       strcpy(n,(yyvsp[(1) - (5)].C_str));
		     else {
		       sprintf(n, "ss%ld", PSC);
		       PSC++;
		     }
		     string condition = (yyvsp[(4) - (5)].TERSptr)->O->event->exp;
		     string remainder;

		     //Removes the ]
		     condition = condition.substr(0,condition.size()-1);

		     
		     //change ='0' and ='1' to bool or ~bool
		     while(condition.find("='") != string::npos) {
		       if (condition.find("='1'") != string::npos) {
			 remainder = condition.substr(condition.find("='1'")+
						      4, string::npos);
			 condition = condition.substr(0,
						      condition.find("='1'"));
		       }
		       else if (condition.find("='0'") != string::npos) {
			 remainder = condition.substr(condition.find("='0'")+
						      4, string::npos);
			 condition = condition.substr(1,string::npos);
			 condition = "[~(" +
			   condition.substr(0,condition.find("='0'")) + ")";
		       }
		       condition = condition + remainder;
		     }
		     string action = breakElem->O->event->exp;
		     action = action.substr(1, string::npos);
		     (yyvsp[(4) - (5)].TERSptr)->O->event->exp = CopyString((condition+
					  " & "+ action).c_str());
		     
		     
		     TERstructADT x;
		     //x = TERScompose($4, breakElem, ";");
		     x = TERSmakeloop((yyvsp[(4) - (5)].TERSptr));
		     x = TERSrepeat(x,";");
		     char s[255];
		     strcpy(s, rmsuffix(n));
		     printf("Compiled break statement %s\n",s);
		     fprintf(lg,"Compiled break statement %s\n",s);
		     tel_tb->insert(n, x);
		   }
		   //initial conditions
		   else {
		     for (eventsetADT step = breakElem->O; step; step = step->link) {
		       TERstructADT x;

		       //pull apart the exp to get the variable and the number
		       string var = step->event->exp;
		       var = var.substr(1,var.find(":=")-1);
		       string num = step->event->exp;
		       num = num.substr(num.find(":=")+2,string::npos);
		       num = num.substr(0,num.find(']'));

		       actionADT a = action(var);
		       a->type = CONT + VAR + DUMMY;
		       if (num.find(';') == string::npos) {
			 a->linitval = atoi(num.c_str());
			 a->uinitval = atoi(num.c_str());
		       } else {
			 string lval = num.substr(1,num.find(';'));
			 string uval = num.substr(num.find(';')+1,
						  string::npos-1);
			 a->linitval = atoi(lval.c_str());
			 a->uinitval = atoi(uval.c_str());
		       }
		       x = TERS(a,FALSE,0,0,FALSE);
		       tel_tb->insert(var, x);
		     }
		   }
		 }
    break;

  case 556:
/* Line 1787 of yacc.c  */
#line 2494 "parser.y"
    {

	   /*
	     for (simul_lst::iterator elem = $1->begin();
	     elem != $1->end() && elem != NULL; elem++) {
	     cout << elem->first << endl;
	     for (list<string>::iterator selem=elem->second.begin();
	     selem != elem->second.end(); selem++) {
	     cout << "\t" << *selem << endl;
	     }
	     }
	   */
		   
	   if ((yyvsp[(1) - (1)].simul_lst)) {
	     if ((yyvsp[(1) - (1)].simul_lst)->size() == 1) {
	       //Simple Simultaneous Statment
	       //Like a rate assignment
			
	       char n[1000];
	       sprintf(n, "ss%ld", PSC);
	       PSC++;
	       char s[255];
	       strcpy(s, rmsuffix(n)); 

	       printf("Compiled simple simultaneous statement %s\n",s);
	       fprintf(lg,
		       "Compiled simple simultaneous statement %s\n",s);

	       (yyval.TERSptr) = Guard(*((yyvsp[(1) - (1)].simul_lst)->begin()->second.begin()));
	       (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
	       (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
	       tel_tb->insert(n, (yyval.TERSptr));
	     }
	     else {
		       
	       actionsetADT allActions;
	       eventsetADT allEvents;
	       rulesetADT allRules;
	       conflictsetADT allConflicts;
		       
	       rulesetADT initRules;
	       unsigned int j = 1;
	       int expanded = EXPANDED_RATE_NETS;

	       if (expanded==0 || expanded==2) {
		 actionADT x;
		 x = dummyE();
		 x->maxoccur = 1;
		 allActions = create_action_set(x);
		 allEvents = create_event_set(event(x,1,2,1,NULL,""));
	       }

		       
	       for (simul_lst::iterator elem = (yyvsp[(1) - (1)].simul_lst)->begin();
		    elem != (yyvsp[(1) - (1)].simul_lst)->end(); elem++) {
		 string level = "[" + elem->first;
		 //level = level.substr(0,level.find(" & true"));

		 for(list<string>::iterator selem=elem->second.begin();
		     selem != elem->second.end(); selem++) {
		   level += " & " +  *selem;
		 }
		 while (level.find(" & true") != string::npos) {
		   level.replace(level.find(" & true"),7,"");
		 }
		 while (level.find("true ") != string::npos) {
		   level.replace(level.find("true "),7,"");
		 }

		 if (expanded==2) {
		   for (simul_lst::iterator elem2 = (yyvsp[(1) - (1)].simul_lst)->begin();
			elem2 != (yyvsp[(1) - (1)].simul_lst)->end(); elem2++) {
		     if (elem2 != elem) {
		       for(list<string>::iterator selem2=elem2->second.begin();
			   selem2 != elem2->second.end(); selem2++) {
			 unsigned int first,last,ins,curEnd;
			 curEnd=(*selem2).length()-1;
			 while (curEnd > 0) {
			   for (last=curEnd;(*selem2)[last]!=':' 
				  && last>=0;last--);
			   for (first=last;(*selem2)[first]!=' ' 
				  && first>=0;first--);
			   if (first >= 0) {
			     for (ins=level.length()-1;level[ins]==')'
				    && ins>=0;ins--);
			     if (ins >= 0) {
			       level.insert(ins+1," & " + 
					    (*selem2).substr(first+1,
							     last-first-1) +
					    ":=FALSE");
			     }
			   }
			   for (curEnd=first;(*selem2)[curEnd]!='|' 
				  && curEnd>0;curEnd--);
			 }
		       }
		     }
		   }
		 }

		 level += "]";

		 if (expanded==1) {
		   //Creates one event for each of the other events
		   for (unsigned int i = 1; i <= (yyvsp[(1) - (1)].simul_lst)->size(); i++) {
		     if (i != j) {
		       eventsetADT curEvent;
		       actionsetADT curAction;
		       actionADT a;
			       
		       if (j == 1 && i == 2) {
			 a = dummyE();
			 a->maxoccur = 1;
			 allActions = create_action_set(a);
			 allEvents =
			   create_event_set(event(a,1,i,j,NULL,
						  level.c_str()));
		       }
		       else {
			 a = dummyE();
			 a->maxoccur = 1;
			 curAction = create_action_set(a);
			 curEvent =
			   create_event_set(event(a,1,i,j,
						  NULL,level.c_str()));
			 allActions = union_actions(allActions,
						    curAction);
			 allEvents = union_events(allEvents,curEvent);
		       }
		     }
		   }
		 }
		 else {
		   eventsetADT curEvent;
		   actionsetADT curAction;
		   actionADT a;
		   a = dummyE();
		   a->maxoccur = 1;
		   curAction = create_action_set(a);
		   curEvent =
		     create_event_set(event(a,1,1,2,
					    NULL,level.c_str()));
		   allActions = union_actions(allActions,
					      curAction);
		   allEvents = union_events(allEvents,curEvent);
		 }
		 if (expanded==1) j++;
	       }

	       bool firstR = true;
	       bool firstRp = true;
	       bool firstC = true;
	       for (eventsetADT toEvent = allEvents;
		    toEvent != NULL; toEvent = toEvent->link) {
		 for (eventsetADT fromEvent = allEvents;
		      fromEvent != NULL; fromEvent = fromEvent->link) {
		   if(fromEvent->event->upper==toEvent->event->lower) {
		     if (toEvent->event->lower == 1) {
		       if (firstRp) {
			 initRules =
			   create_rule_set(rule(fromEvent->event,
						toEvent->event,
						0,0,TRIGGER,true,NULL,
						toEvent->event->exp));
			 firstRp = false;
		       }
		       else {
			 initRules = add_rule(initRules,
					      fromEvent->event,
					      toEvent->event,
					      0,0,TRIGGER,NULL,
					      toEvent->event->exp);
		       }
		     }
		     else {
		       if (firstR) {
			 allRules =
			   create_rule_set(rule(fromEvent->event,
						toEvent->event,
						0,0,TRIGGER,true,NULL,
						toEvent->event->exp));
			 firstR = false;
		       }
		       else {
			 allRules = add_rule(allRules,fromEvent->event,
					     toEvent->event,
					     0,0,TRIGGER,NULL,
					     toEvent->event->exp);
		       }
		     }
		   }
		   if(((fromEvent->event->lower!=toEvent->event->lower &&
			fromEvent->event->upper==toEvent->event->upper) ||
		       (fromEvent->event->upper!=toEvent->event->upper &&
			fromEvent->event->lower==toEvent->event->lower)) ||
		      ((expanded==0 || expanded==2) &&
		       fromEvent->event->upper==toEvent->event->upper &&
		       fromEvent->event->lower==toEvent->event->lower &&
		       fromEvent->event->upper != 1 &&
		       strcmp(fromEvent->event->exp,toEvent->event->exp)))
		     {
		       if (firstC) {
			 allConflicts =
			   create_conflict_set(conflict(fromEvent->
							event,
							toEvent->
							event));
			 firstC = false;
		       }
		       else {
			 allConflicts = add_conflict(allConflicts,
						     fromEvent->event,
						     toEvent->event);
		       }
		     }
		 }
	       }

	       for (simul_lst::iterator elem = (yyvsp[(1) - (1)].simul_lst)->begin();
		    elem != (yyvsp[(1) - (1)].simul_lst)->end(); elem++) {
		 if (expanded==2) {
		   for(list<string>::iterator selem=elem->second.begin();
		       selem != elem->second.end(); selem++) {
		     eventsetADT curEvent;
		     actionsetADT curAction;
		     actionADT a;
		     unsigned int first,last,curEnd;
		     curEnd=(*selem).length()-1;
		     while (curEnd > 0) {
		       for (last=curEnd;(*selem)[last]!=':' 
			      && last>=0;last--);
		       for (first=last;(*selem)[first]!=' ' 
			      && first>=0;first--);
		       if ((first >= 0)&&(last >= 0)) {
			 a = action((*selem).substr(first+1,last-first-1)+'+');
			 a->type = OUT;
			 a->maxoccur = 1;
			 curAction = create_action_set(a);
			 curEvent =
			   create_event_set(event(a,1,0,0,NULL,NULL));
			 allActions = union_actions(allActions,
						    curAction);
			 allEvents = union_events(allEvents,curEvent);
			 a = action((*selem).substr(first+1,last-first-1)+'-');
			 a->type = OUT;
			 a->maxoccur = 1;
			 curAction = create_action_set(a);
			 curEvent =
			   create_event_set(event(a,1,0,0,NULL,NULL));
			 allActions = union_actions(allActions,
						    curAction);
			 allEvents = union_events(allEvents,curEvent);
		       }
		       for (curEnd=first;(*selem)[curEnd]!='|' 
			      && curEnd>0;curEnd--);
		     } 
		   }
		 }
	       }

	       (yyval.TERSptr) = new terstruct_tag;
	       (yyval.TERSptr)->A = allActions;
	       (yyval.TERSptr)->O = allEvents;
	       (yyval.TERSptr)->R = allRules;
	       (yyval.TERSptr)->Rp = initRules;
	       if (firstC == true) (yyval.TERSptr)->C = NULL;
	       else (yyval.TERSptr)->C = allConflicts;

		       
	       (yyval.TERSptr)->I = NULL;
	       (yyval.TERSptr)->first = NULL;
	       (yyval.TERSptr)->last = NULL;
	       (yyval.TERSptr)->loop = NULL;
	       (yyval.TERSptr)->CT = NULL;
	       (yyval.TERSptr)->CP = NULL;
	       (yyval.TERSptr)->DPCT = NULL;
	       (yyval.TERSptr)->CPCT = NULL;
	       (yyval.TERSptr)->DTCP = NULL;
	       (yyval.TERSptr)->CTCP = NULL;
	       (yyval.TERSptr)->CPDT = NULL;
	       (yyval.TERSptr)->Cp = NULL;
	       (yyval.TERSptr)->exp = NULL;

	       char n[1000];
	       sprintf(n, "ss%ld", PSC);
	       PSC++;
	       char s[255];
	       strcpy(s,rmsuffix(n));
	       fflush(stdout);
	       tel_tb->insert(n, (yyval.TERSptr));
	       printf("Compiled simultaneous statement %s\n",s);
	       fprintf(lg,"Compiled simultaneous statement %s\n",s);
		       
	     }

	   }
	 }
    break;

  case 557:
/* Line 1787 of yacc.c  */
#line 2795 "parser.y"
    {
		   (yyval.simul_lst) = (yyvsp[(1) - (1)].simul_lst);
		 }
    break;

  case 558:
/* Line 1787 of yacc.c  */
#line 2799 "parser.y"
    {
		   (yyval.simul_lst) = (yyvsp[(1) - (1)].simul_lst);
		 }
    break;

  case 559:
/* Line 1787 of yacc.c  */
#line 2803 "parser.y"
    {
		   (yyval.simul_lst) = (yyvsp[(1) - (1)].simul_lst);
		   //TYPES ty; ty.set_str("case");
		   //$$ = new pair<TYPES,telADT>(ty, $1);
		 }
    break;

  case 560:
/* Line 1787 of yacc.c  */
#line 2809 "parser.y"
    {
		   (yyval.simul_lst) = 0;
		   printf("Procedures are not supported\n");
		   //TYPES ty; ty.set_str("proc");
		   //$$ = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 561:
/* Line 1787 of yacc.c  */
#line 2816 "parser.y"
    {
		   (yyval.simul_lst) = 0;
		   //TYPES ty; ty.set_str("nil");
		   //$$ = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 562:
/* Line 1787 of yacc.c  */
#line 2823 "parser.y"
    { (yyval.simul_lst) = 0; }
    break;

  case 563:
/* Line 1787 of yacc.c  */
#line 2825 "parser.y"
    {
		   (yyval.simul_lst) = new simul_lst;
		   bool simple=true;
		   if ((yyvsp[(1) - (2)].simul_lst)) {
		     for (simul_lst::iterator elem = (yyvsp[(1) - (2)].simul_lst)->begin();
			  elem != (yyvsp[(1) - (2)].simul_lst)->end(); elem++) {
		       if (elem->first != "true") {
			 simple=false;
			 break;
		       }
		     }
		   }
		   if (simple) {
		     if ((yyvsp[(2) - (2)].simul_lst)) {
		       for (simul_lst::iterator elem = (yyvsp[(2) - (2)].simul_lst)->begin();
			    elem != (yyvsp[(2) - (2)].simul_lst)->end(); elem++) {
			 if (elem->first != "true") {
			   simple=false;
			   break;
			 }
		       }
		     }
		   }
		   if (simple) {
		     list<string> expressions;
		     string expression="";
		     bool first=true;
		     if ((yyvsp[(1) - (2)].simul_lst)) {
		       for (simul_lst::iterator elem = (yyvsp[(1) - (2)].simul_lst)->begin();
			    elem != (yyvsp[(1) - (2)].simul_lst)->end(); elem++) {
			 for(list<string>::iterator selem=elem->second.begin();
			     selem != elem->second.end(); selem++) {
			   if (first) {
			     first=false;
			     expression="((";
			   } else 
			     expression+="|(";
			   expression+=(*selem);
			   expression+=")";
			 }
		       }
		     }
		     if ((yyvsp[(2) - (2)].simul_lst)) {
		       for (simul_lst::iterator elem = (yyvsp[(2) - (2)].simul_lst)->begin();
			    elem != (yyvsp[(2) - (2)].simul_lst)->end(); elem++) {
			 for(list<string>::iterator selem=elem->second.begin();
			     selem != elem->second.end(); selem++) {
			   if (first) {
			     first=false;
			     expression="((";
			   } else 
			     expression+="|(";
			   expression+=(*selem);
			   expression+=")";
			 }
		       }
		     }
		     expression+=")";
		     expressions.push_back(expression);
		     (yyval.simul_lst)->push_back(make_pair("true",expressions));
		   } else {
		     if ((yyvsp[(1) - (2)].simul_lst))
		       (yyval.simul_lst) = (yyvsp[(1) - (2)].simul_lst);
		     for (simul_lst::iterator elem = (yyvsp[(2) - (2)].simul_lst)->begin();
			  elem != (yyvsp[(2) - (2)].simul_lst)->end(); elem++) {
		       list<string> expressions;
		       for(list<string>::iterator selem=elem->second.begin();
			   selem != elem->second.end(); selem++) {
			 expressions.push_back(elem->first+" & "+(*selem));
		       }
		       (yyval.simul_lst)->push_back(make_pair("true",expressions));
		     }
		   }
		   /*
		   
		   for (simul_lst::iterator elem = $2->begin();
			elem != $2->end(); elem++) {
		     $$->push_back(*elem);
		     }*/
		 }
    break;

  case 564:
/* Line 1787 of yacc.c  */
#line 2910 "parser.y"
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   expressions.push_back((yyvsp[(2) - (6)].types)->theExpression + ":=" +
					 (yyvsp[(4) - (6)].types)->theExpression);
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));

		 }
    break;

  case 565:
/* Line 1787 of yacc.c  */
#line 2920 "parser.y"
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   expressions.push_back((yyvsp[(1) - (5)].types)->theExpression + ":=" +
					 (yyvsp[(3) - (5)].types)->theExpression);
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));
		 }
    break;

  case 566:
/* Line 1787 of yacc.c  */
#line 2928 "parser.y"
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   if (EXPANDED_RATE_NETS==0) {
		     expressions.push_back("~(" + (*(yyvsp[(2) - (7)].str_list)->begin()) + "dot>=" +
					   (yyvsp[(6) - (7)].types)->theExpression + " & " +
					   (*(yyvsp[(2) - (7)].str_list)->begin()) + "dot<=" +
					   (yyvsp[(6) - (7)].types)->theExpression + ") & " +
					   *((yyvsp[(2) - (7)].str_list)->begin()) + "'dot:=" +
					   (yyvsp[(6) - (7)].types)->theExpression);
		   } else if (EXPANDED_RATE_NETS==1) {
		     expressions.push_back(*((yyvsp[(2) - (7)].str_list)->begin()) + "'dot:=" +
					   (yyvsp[(6) - (7)].types)->theExpression);
		   } else {
		     string temp_str;
		     temp_str =(*(yyvsp[(2) - (7)].str_list)->begin()) + "dot_";
		     for (int i=0;(yyvsp[(6) - (7)].types)->theExpression[i]!='\0';i++) {
		       if ((yyvsp[(6) - (7)].types)->theExpression[i]=='-')
			 temp_str += "m";
		       else if ((yyvsp[(6) - (7)].types)->theExpression[i]==';')
			 temp_str += "_";
		       else if (((yyvsp[(6) - (7)].types)->theExpression[i]!='{') &&
				((yyvsp[(6) - (7)].types)->theExpression[i]!='}'))
			 temp_str += (yyvsp[(6) - (7)].types)->theExpression[i];
		     } 
		     expressions.push_back("~(" + temp_str + ") & " +
					   *((yyvsp[(2) - (7)].str_list)->begin()) + "'dot:=" +
					   (yyvsp[(6) - (7)].types)->theExpression + " & " +
					   temp_str + ":=TRUE"); 
		   }
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));
		 }
    break;

  case 567:
/* Line 1787 of yacc.c  */
#line 2961 "parser.y"
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   if (EXPANDED_RATE_NETS==0) {
		     expressions.push_back("~(" + (*(yyvsp[(1) - (6)].str_list)->begin()) + "dot>=" +
					   (yyvsp[(5) - (6)].types)->theExpression + " & " +
					   (*(yyvsp[(1) - (6)].str_list)->begin()) + "dot<=" +
					   (yyvsp[(5) - (6)].types)->theExpression + ") & " +
					   *((yyvsp[(1) - (6)].str_list)->begin()) + "'dot:=" +
					   (yyvsp[(5) - (6)].types)->theExpression);
		   } else  if (EXPANDED_RATE_NETS==1) {
		     expressions.push_back(*((yyvsp[(1) - (6)].str_list)->begin()) + "'dot:=" +
					   (yyvsp[(5) - (6)].types)->theExpression);
		   } else {
		     string temp_str;
		     temp_str =(*(yyvsp[(1) - (6)].str_list)->begin()) + "dot_";
		     for (int i=0;(yyvsp[(5) - (6)].types)->theExpression[i]!='\0';i++) {
		       if ((yyvsp[(5) - (6)].types)->theExpression[i]=='-')
			 temp_str += "m";
		       else if ((yyvsp[(5) - (6)].types)->theExpression[i]==';')
			 temp_str += "_";
		       else if (((yyvsp[(5) - (6)].types)->theExpression[i]!='{') &&
				((yyvsp[(5) - (6)].types)->theExpression[i]!='}'))
			 temp_str += (yyvsp[(5) - (6)].types)->theExpression[i];
		     } 
		     expressions.push_back("~(" + temp_str + ") & " +
					   *((yyvsp[(1) - (6)].str_list)->begin()) + "'dot:=" +
					   (yyvsp[(5) - (6)].types)->theExpression + " & " +
					   temp_str + ":=TRUE");
		   }
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));
		 }
    break;

  case 568:
/* Line 1787 of yacc.c  */
#line 2999 "parser.y"
    {
		   string exp = "";
		     if((yyvsp[(3) - (11)].types)->data_type() != string("bool"))
		       exp = (yyvsp[(3) - (11)].types)->theExpression;
		     else
		       exp = (yyvsp[(3) - (11)].types)->str();
		     /*
		   if ($3->theExpression.find("=") != string::npos) {
		     if ($3->theExpression.find("\'0\'") != string::npos) {
		       exp = "~";
		     }
		     exp +=
		       $3->theExpression.substr(0,$3->theExpression.find("="));
		   }
		     */

		   (yyval.simul_lst) = new simul_lst;
		   for (simul_lst::iterator elem = (yyvsp[(5) - (11)].simul_lst)->begin();
			elem != (yyvsp[(5) - (11)].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		   string notOthers = "";
		   if ((yyvsp[(6) - (11)].simul_lst)) {
		     string current = "";
		     string last = "";
		     bool first = true;
		     for (simul_lst::iterator elem = (yyvsp[(6) - (11)].simul_lst)->begin();
			  elem != (yyvsp[(6) - (11)].simul_lst)->end(); elem++) {
		       if (elem->first != current) {
			 last = notOthers;
			 if (first) {
			   notOthers = "(";
			   first=false;
			 } else {
			   notOthers += " & ";
			 }
			 notOthers += "~(" + elem->first + ")";
			 current=elem->first;
		       }
		       if (last=="") {
			 (yyval.simul_lst)->push_back(make_pair("~(" + exp +
						 ") & " + 
						 elem->first,
						 elem->second));
		       } else {
			 (yyval.simul_lst)->push_back(make_pair("~(" + exp +
						 ") & " + 
						 last + ") & " +
						 elem->first,
						 elem->second));
		       }
		     }
		     notOthers += ") & ";
  		   }
		   if ((yyvsp[(7) - (11)].simul_lst)) {
		     for (simul_lst::iterator elem = (yyvsp[(7) - (11)].simul_lst)->begin();
			  elem != (yyvsp[(7) - (11)].simul_lst)->end(); elem++) {
		       (yyval.simul_lst)->push_back(make_pair("~(" + exp +
					       ") & " + notOthers+elem->first,
					       elem->second));
		     }
		   }
		 }
    break;

  case 569:
/* Line 1787 of yacc.c  */
#line 3066 "parser.y"
    {
		   string exp = "";
		     if((yyvsp[(2) - (9)].types)->data_type() != string("bool"))
		       exp = (yyvsp[(2) - (9)].types)->theExpression;
		     else
		       exp = (yyvsp[(2) - (9)].types)->str();
		     /*
		   if ($2->theExpression.find("=") != string::npos) {
		     if ($2->theExpression.find("\'0\'") != string::npos) {
		       exp = "~";
		     }
		     exp +=
		       $2->theExpression.substr(0,$2->theExpression.find("="));
		   }
		     */

		   (yyval.simul_lst) = new simul_lst;
		   for (simul_lst::iterator elem = (yyvsp[(4) - (9)].simul_lst)->begin();
			elem != (yyvsp[(4) - (9)].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		   string notOthers = "";
		   if ((yyvsp[(5) - (9)].simul_lst)) {
		     string current = "";
		     string last = "";
		     bool first = true;
		     for (simul_lst::iterator elem = (yyvsp[(5) - (9)].simul_lst)->begin();
			  elem != (yyvsp[(5) - (9)].simul_lst)->end(); elem++) {
		       if (elem->first != current) {
			 last = notOthers;
			 if (first) {
			   notOthers = "(";
			   first=false;
			 } else {
			   notOthers += " & ";
			 }
			 notOthers += "~(" + elem->first + ")";
			 current=elem->first;
		       }
		       if (last=="") {
			 (yyval.simul_lst)->push_back(make_pair("~(" + exp +
						 ") & " + 
						 elem->first,
						 elem->second));
		       } else {
			 (yyval.simul_lst)->push_back(make_pair("~(" + exp +
						 ") & " + 
						 last + ") & " +
						 elem->first,
						 elem->second));
		       }
		     }
		     notOthers += ") & ";
  		   }
		   if ((yyvsp[(6) - (9)].simul_lst)) {
		     for (simul_lst::iterator elem = (yyvsp[(6) - (9)].simul_lst)->begin();
			  elem != (yyvsp[(6) - (9)].simul_lst)->end(); elem++) {
		       (yyval.simul_lst)->push_back(make_pair("~(" + exp +
					       ") & " + notOthers+elem->first,
					       elem->second));
		     }
		   }
		 }
    break;

  case 570:
/* Line 1787 of yacc.c  */
#line 3133 "parser.y"
    { (yyval.simul_lst) = 0; }
    break;

  case 571:
/* Line 1787 of yacc.c  */
#line 3136 "parser.y"
    {
		   string exp = "";
		     if((yyvsp[(3) - (5)].types)->data_type() != string("bool"))
		       exp = (yyvsp[(3) - (5)].types)->theExpression;
		     else
		       exp = (yyvsp[(3) - (5)].types)->str();
		     /*
		   if ($3->theExpression.find("=") != string::npos) {
		     if ($3->theExpression.find("\'0\'") != string::npos) {
		       exp = "~";
		     }
		     exp +=
		       $3->theExpression.substr(0,$3->theExpression.find("="));
		       }*/
		   (yyval.simul_lst) = new simul_lst;
/* 		   string notOthers = ""; */
		   if ((yyvsp[(1) - (5)].simul_lst)) {
/* 		     notOthers = "("; */
/* 		     bool first = true; */
/* 		     for (simul_lst::iterator elem = $1->begin(); */
/* 			  elem != $1->end(); elem++) { */
/* 		       if (!first) notOthers += " & "; */
/* 		       notOthers += "~(" + elem->first + ")"; */
/* 		       first = false; */
/* 		     } */
/* 		     notOthers += ") & "; */
		     (yyval.simul_lst) = (yyvsp[(1) - (5)].simul_lst);
		   }
		  
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = (yyvsp[(5) - (5)].simul_lst)->begin();
			elem != (yyvsp[(5) - (5)].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(/*notOthers +*/
					     exp /*+ " & " +
						   elem->first*/,
					     elem->second));
		   }
		 }
    break;

  case 572:
/* Line 1787 of yacc.c  */
#line 3176 "parser.y"
    { (yyval.simul_lst) = 0; }
    break;

  case 573:
/* Line 1787 of yacc.c  */
#line 3178 "parser.y"
    {
		   (yyval.simul_lst) = (yyvsp[(2) - (2)].simul_lst);
		 }
    break;

  case 574:
/* Line 1787 of yacc.c  */
#line 3187 "parser.y"
    {
		   (yyval.simul_lst) = (yyvsp[(5) - (9)].simul_lst);
		   
		   for (simul_lst::iterator elem = (yyval.simul_lst)->begin();
			elem != (yyval.simul_lst)->end(); elem++) {
		     if (elem->first[0] == '~') {
		       elem->first = "~(" + (yyvsp[(3) - (9)].types)->theExpression + ")" +
			 elem->first.substr(1,string::npos);
		     }
		     else {
		       elem->first = (yyvsp[(3) - (9)].types)->theExpression + elem->first;
		     }
		   }
		 }
    break;

  case 575:
/* Line 1787 of yacc.c  */
#line 3204 "parser.y"
    {
		   
		   (yyval.simul_lst) = (yyvsp[(5) - (8)].simul_lst);
		   for (simul_lst::iterator elem = (yyval.simul_lst)->begin();
			elem != (yyval.simul_lst)->end(); elem++) {
		     if (elem->first[0] == '~') {
		       elem->first = "~(" + (yyvsp[(3) - (8)].types)->theExpression + ")" +
			 elem->first.substr(1,string::npos);
		     }
		     else {
		       elem->first = (yyvsp[(3) - (8)].types)->theExpression + elem->first;
		     }
		   }
		 }
    break;

  case 576:
/* Line 1787 of yacc.c  */
#line 3222 "parser.y"
    {
		   (yyval.simul_lst) = new simul_lst;

		   /* These probably aren't going to be as robust
		      as they should be, but work for now */
		   string exp = "";
		   
		   if ((yyvsp[(2) - (4)].ty_lst)->begin()->str() == "'0'") 
		     exp = "~";
		   		   
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = (yyvsp[(4) - (4)].simul_lst)->begin();
			elem != (yyvsp[(4) - (4)].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		 }
    break;

  case 577:
/* Line 1787 of yacc.c  */
#line 3241 "parser.y"
    {
		   (yyval.simul_lst) = (yyvsp[(1) - (5)].simul_lst);

		   string exp = "";
		   
		   if ((yyvsp[(3) - (5)].ty_lst)->begin()->str() == "'0'") 
		     exp = "~";
		   
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = (yyvsp[(5) - (5)].simul_lst)->begin();
			elem != (yyvsp[(5) - (5)].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		 }
    break;

  case 578:
/* Line 1787 of yacc.c  */
#line 3260 "parser.y"
    {
		  table->new_tb();
		  processChannels.clear();
		}
    break;

  case 579:
/* Line 1787 of yacc.c  */
#line 3266 "parser.y"
    {
		  map<string,int>::iterator m(processChannels.begin());
		  while(m != processChannels.end()){
		    string name(m->first);
		    int type(m->second);
		    if(type&ACTIVE  && type&SENT||
		       type&PASSIVE && type&RECEIVED){
		      type |= PUSH;
		    }
		    if(type&ACTIVE  && type&RECEIVED ||
		       type&PASSIVE && type&SENT    ){
		      type |= PULL;
		    }
		    signals[cur_entity][name] |= type;
		    action(name+SEND_SUFFIX+"+")->type |= type;
		    action(name+SEND_SUFFIX+"-")->type |= type;
		    action(name+RECEIVE_SUFFIX+"+")->type |= type;
		    action(name+RECEIVE_SUFFIX+"-")->type |= type;
		    m++;
		  }
		  table->delete_tb();
		  (yyval.TERSptr) = 0;
		  if((yyvsp[(6) - (8)].TERSptr)){
		    (yyval.TERSptr) = TERSmakeloop((yyvsp[(6) - (8)].TERSptr));
		    (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		    char s[255];
		    strcpy(s, rmsuffix((yyvsp[(1) - (8)].C_str)));
		    if (1) {
		      printf("Compiled process %s\n",s);
		      fprintf(lg,"Compiled process %s\n",s);
		    } 
		    else
		      emitters(outpath, s, (yyval.TERSptr));
		  }
		  if(tel_tb->insert(((yyvsp[(1) - (8)].C_str)), (yyval.TERSptr)) == false)
		    err_msg("duplicate process label '", (yyvsp[(1) - (8)].C_str), "'");
		}
    break;

  case 582:
/* Line 1787 of yacc.c  */
#line 3312 "parser.y"
    {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   (yyval.C_str) = copy_string(n);
		 }
    break;

  case 583:
/* Line 1787 of yacc.c  */
#line 3319 "parser.y"
    {
		   (yyval.C_str) = copy_string((yyvsp[(1) - (2)].C_str));
		 }
    break;

  case 586:
/* Line 1787 of yacc.c  */
#line 3331 "parser.y"
    {}
    break;

  case 587:
/* Line 1787 of yacc.c  */
#line 3332 "parser.y"
    {}
    break;

  case 588:
/* Line 1787 of yacc.c  */
#line 3333 "parser.y"
    {}
    break;

  case 589:
/* Line 1787 of yacc.c  */
#line 3334 "parser.y"
    {}
    break;

  case 590:
/* Line 1787 of yacc.c  */
#line 3335 "parser.y"
    {}
    break;

  case 591:
/* Line 1787 of yacc.c  */
#line 3336 "parser.y"
    {}
    break;

  case 592:
/* Line 1787 of yacc.c  */
#line 3337 "parser.y"
    {}
    break;

  case 593:
/* Line 1787 of yacc.c  */
#line 3338 "parser.y"
    {}
    break;

  case 594:
/* Line 1787 of yacc.c  */
#line 3339 "parser.y"
    {}
    break;

  case 595:
/* Line 1787 of yacc.c  */
#line 3340 "parser.y"
    {}
    break;

  case 596:
/* Line 1787 of yacc.c  */
#line 3341 "parser.y"
    {}
    break;

  case 597:
/* Line 1787 of yacc.c  */
#line 3342 "parser.y"
    {}
    break;

  case 598:
/* Line 1787 of yacc.c  */
#line 3350 "parser.y"
    {
		   /*if(__sensitivity_list__)
		     {
		       err_msg("wait statement must not appear",
			       "in a process statement which has",
			       "a sensitivity list");
		       __sensitivity_list__= 0;
		       }*/
		   TYPES ty; ty.set_str("wait");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 599:
/* Line 1787 of yacc.c  */
#line 3362 "parser.y"
    {
		   TYPES ty; ty.set_str("assert");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 600:
/* Line 1787 of yacc.c  */
#line 3367 "parser.y"
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 601:
/* Line 1787 of yacc.c  */
#line 3372 "parser.y"
    {
		   TYPES ty; ty.set_str("var");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 602:
/* Line 1787 of yacc.c  */
#line 3377 "parser.y"
    {
		   TYPES ty; ty.set_str("proc");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 603:
/* Line 1787 of yacc.c  */
#line 3382 "parser.y"
    {
		   TYPES ty; ty.set_str("if");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 604:
/* Line 1787 of yacc.c  */
#line 3387 "parser.y"
    {
		   TYPES ty; ty.set_str("case");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 605:
/* Line 1787 of yacc.c  */
#line 3392 "parser.y"
    {
		   TYPES ty; ty.set_str("loop");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 606:
/* Line 1787 of yacc.c  */
#line 3397 "parser.y"
    {
		   TYPES ty; ty.set_str("next");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 607:
/* Line 1787 of yacc.c  */
#line 3402 "parser.y"
    {
		   TYPES ty; ty.set_str("exit");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 608:
/* Line 1787 of yacc.c  */
#line 3407 "parser.y"
    {
		   TYPES ty; ty.set_str("rtn");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 609:
/* Line 1787 of yacc.c  */
#line 3412 "parser.y"
    {
		   TYPES ty; ty.set_str("nil");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 610:
/* Line 1787 of yacc.c  */
#line 3417 "parser.y"
    {
		   TYPES ty; ty.set_str("rep");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
    break;

  case 611:
/* Line 1787 of yacc.c  */
#line 3422 "parser.y"
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 612:
/* Line 1787 of yacc.c  */
#line 3427 "parser.y"
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 613:
/* Line 1787 of yacc.c  */
#line 3432 "parser.y"
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 614:
/* Line 1787 of yacc.c  */
#line 3437 "parser.y"
    {
		   TYPES ty; ty.set_str("wait");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 615:
/* Line 1787 of yacc.c  */
#line 3443 "parser.y"
    {
		   TYPES ty; ty.set_str("break");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		 }
    break;

  case 616:
/* Line 1787 of yacc.c  */
#line 3448 "parser.y"
    {
		    TYPES ty; ty.set_str("rate");
		    (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[(1) - (1)].TERSptr));
		   }
    break;

  case 623:
/* Line 1787 of yacc.c  */
#line 3460 "parser.y"
    {
		       (yyval.TERSptr)=Guard((yyvsp[(3) - (5)].types)->str());
		    }
    break;

  case 624:
/* Line 1787 of yacc.c  */
#line 3464 "parser.y"
    {
		      (yyval.TERSptr) = Guard((yyvsp[(3) - (5)].types)->theExpression);
		    }
    break;

  case 625:
/* Line 1787 of yacc.c  */
#line 3468 "parser.y"
    {
		      (yyval.TERSptr) = Guard((yyvsp[(3) - (5)].types)->str());
		    }
    break;

  case 626:
/* Line 1787 of yacc.c  */
#line 3472 "parser.y"
    {(yyval.TERSptr)=Guard((yyvsp[(3) - (5)].types)->str());}
    break;

  case 627:
/* Line 1787 of yacc.c  */
#line 3473 "parser.y"
    {(yyval.TERSptr)=Guard((yyvsp[(3) - (5)].types)->str());}
    break;

  case 628:
/* Line 1787 of yacc.c  */
#line 3474 "parser.y"
    {(yyval.TERSptr)=Guard((yyvsp[(3) - (5)].types)->str());}
    break;

  case 629:
/* Line 1787 of yacc.c  */
#line 3475 "parser.y"
    {(yyval.TERSptr)=Guard((yyvsp[(3) - (5)].types)->str());}
    break;

  case 630:
/* Line 1787 of yacc.c  */
#line 3476 "parser.y"
    {(yyval.TERSptr)=Guard((yyvsp[(3) - (5)].types)->str());}
    break;

  case 632:
/* Line 1787 of yacc.c  */
#line 3480 "parser.y"
    {
		   string s = (yyvsp[(1) - (3)].types)->str() + '&' + (yyvsp[(3) - (3)].types)->str();
		   (yyval.types)->set_str(s);
		 }
    break;

  case 634:
/* Line 1787 of yacc.c  */
#line 3487 "parser.y"
    {
		   string s = (yyvsp[(1) - (3)].types)->str() + '|' + (yyvsp[(3) - (3)].types)->str();
		   (yyval.types)->set_str(s);
		 }
    break;

  case 635:
/* Line 1787 of yacc.c  */
#line 3493 "parser.y"
    {
		   (yyval.types) = new TYPES;
		   (yyval.types)->set_data("bool");
		   (yyval.types)->set_str(bool_relation(Probe((yyvsp[(1) - (1)].types)->str())->str(), "'1'", "="));
		 }
    break;

  case 637:
/* Line 1787 of yacc.c  */
#line 3502 "parser.y"
    {
		   string s = (yyvsp[(1) - (3)].types)->str() + '&' + (yyvsp[(3) - (3)].types)->str();
		   (yyval.types)->set_str(s);
		 }
    break;

  case 639:
/* Line 1787 of yacc.c  */
#line 3510 "parser.y"
    {
		   string s = (yyvsp[(1) - (3)].types)->str() + '|' + (yyvsp[(3) - (3)].types)->str();
		   (yyval.types)->set_str(s);
		 }
    break;

  case 640:
/* Line 1787 of yacc.c  */
#line 3517 "parser.y"
    {
		   pair<string,TYPES> TT1 = table->lookup((yyvsp[(1) - (3)].types)->str());
		   pair<string,TYPES> TT3 = table->lookup((yyvsp[(3) - (3)].types)->str());
		   (yyval.types) = new TYPES;
		   (yyval.types)->set_data("bool");
		   if(table->lookup((yyvsp[(1) - (3)].types)->get_string()).first == "qty" ||
		      table->lookup((yyvsp[(3) - (3)].types)->get_string()).first == "qty")
		     cout << "WARNING: Use the guard(expr,relop,expr) syntax "
			  << "for quantities.";
		   else if(table->lookup((yyvsp[(1) - (3)].types)->get_string()).first == "var" ||
			   table->lookup((yyvsp[(3) - (3)].types)->get_string()).first == "var")
		     (yyval.types)->set_str("maybe");
      		   else {
		     (yyval.types)->set_str(bool_relation((yyvsp[(1) - (3)].types)->str(), (yyvsp[(3) - (3)].types)->str(), "="));
		   }
                 }
    break;

  case 641:
/* Line 1787 of yacc.c  */
#line 3537 "parser.y"
    {
	       
	       pair<string,TYPES> TT1 = table->lookup((yyvsp[(1) - (5)].types)->str());
	       (yyval.types) = new TYPES;
	       (yyval.types)->set_data("expr");
	       if (table->lookup((yyvsp[(1) - (5)].types)->get_string()).first == "qty") {
		 
		 string relop;
		 switch ((yyvsp[(3) - (5)].intval)) {
		 case 0:
		   relop = ">";
		   break;
		 case 1:
		   relop = ">=";
		   break;
		 case 2:
		   relop = "<";
		   break;
		 case 3:
		   relop = "<=";
		   break;
		 case 4:
		   relop = "=";
		   break;
		 default:
		   relop = "=";
		   break;
		 }
		 (yyval.types)->set_str((yyvsp[(1) - (5)].types)->str() + relop + numToString((int)(yyvsp[(5) - (5)].floatval)));
	       }
	       else
		   (yyval.types)->set_str("maybe");
	     }
    break;

  case 642:
/* Line 1787 of yacc.c  */
#line 3573 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 643:
/* Line 1787 of yacc.c  */
#line 3574 "parser.y"
    { (yyval.TERSptr) = 0; }
    break;

  case 644:
/* Line 1787 of yacc.c  */
#line 3576 "parser.y"
    {
		   (yyval.TERSptr) = TERS(dummyE(), FALSE, 0, 0, TRUE, (yyvsp[(3) - (4)].types)->str().c_str());
		 }
    break;

  case 645:
/* Line 1787 of yacc.c  */
#line 3579 "parser.y"
    { (yyval.TERSptr) = (yyvsp[(3) - (4)].TERSptr); }
    break;

  case 646:
/* Line 1787 of yacc.c  */
#line 3581 "parser.y"
    {(yyval.TERSptr) = 0; }
    break;

  case 647:
/* Line 1787 of yacc.c  */
#line 3583 "parser.y"
    {(yyval.TERSptr) = 0; }
    break;

  case 648:
/* Line 1787 of yacc.c  */
#line 3585 "parser.y"
    {(yyval.TERSptr) = 0; }
    break;

  case 649:
/* Line 1787 of yacc.c  */
#line 3587 "parser.y"
    {(yyval.TERSptr) = 0; }
    break;

  case 650:
/* Line 1787 of yacc.c  */
#line 3591 "parser.y"
    {
		   (yyval.types) = (yyvsp[(2) - (2)].types);
		   string s;
		   if((yyvsp[(2) - (2)].types)->data_type() != string("bool"))
		     s = '[' + (yyval.types)->theExpression + ']';
		     else
		       s = '[' + (yyval.types)->str() + ']';
		   
		   (yyval.types)->set_str(s);
		 }
    break;

  case 652:
/* Line 1787 of yacc.c  */
#line 3607 "parser.y"
    {
		   int l = 0,  u = INFIN;
		   const TYPELIST *tl = (yyvsp[(2) - (2)].types)->get_list();
		   if(tl != 0){
		     TYPELIST::const_iterator I = tl->begin();
		     if(I != tl->end()) {
		       l = tl->front().second.get_int();
		       I++;
		       if(I!=tl->end())
			 u = tl->back().second.get_int();
		     }
		     (yyval.TERSptr) = TERS(dummyE(), FALSE, l, u, TRUE);
		   }
		   else{
		     (yyval.TERSptr) = NULL;
		   }
		 }
    break;

  case 653:
/* Line 1787 of yacc.c  */
#line 3627 "parser.y"
    {
		 string expr;

		 if((yyvsp[(2) - (4)].types)->data_type() != string("bool"))
		   expr = "[~(" + (yyvsp[(2) - (4)].types)->theExpression + ")]";
		 else
		   expr = "[~(" + (yyvsp[(2) - (4)].types)->str() + ")]";

		 string LL = "fail+";
		 actionADT a = action(LL);
		 makebooldecl(OUT|ISIGNAL, a, false, NULL);
		 (yyval.TERSptr) = TERS(a, FALSE, 0, 0, FALSE, expr.c_str());
	       }
    break;

  case 656:
/* Line 1787 of yacc.c  */
#line 3660 "parser.y"
    {}
    break;

  case 657:
/* Line 1787 of yacc.c  */
#line 3662 "parser.y"
    {}
    break;

  case 658:
/* Line 1787 of yacc.c  */
#line 3667 "parser.y"
    {
		   reqSuffix = SEND_SUFFIX;
		   ackSuffix = RECEIVE_SUFFIX;
		   join = dummyE();  //Branches of parallel send join here.
		   join->lower = join->upper = 0;
		   direction = SENT;
		 }
    break;

  case 659:
/* Line 1787 of yacc.c  */
#line 3675 "parser.y"
    {
		   reqSuffix = RECEIVE_SUFFIX;
		   ackSuffix = SEND_SUFFIX;
		   join = dummyE();  //Branches of parallel receive join here.
		   join->lower = join->upper = 0;
		   direction = RECEIVED;
		 }
    break;

  case 660:
/* Line 1787 of yacc.c  */
#line 3685 "parser.y"
    {(yyval.TERSptr) = (yyvsp[(3) - (5)].TERSptr);}
    break;

  case 661:
/* Line 1787 of yacc.c  */
#line 3687 "parser.y"
    {
		   (yyval.TERSptr) = FourPhase((yyvsp[(3) - (5)].types)->get_string());
		 }
    break;

  case 663:
/* Line 1787 of yacc.c  */
#line 3695 "parser.y"
    {
		   (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), (yyvsp[(3) - (3)].TERSptr), "||");
    		 }
    break;

  case 664:
/* Line 1787 of yacc.c  */
#line 3701 "parser.y"
    {
		   (yyval.TERSptr) = FourPhase((yyvsp[(1) - (3)].types)->get_string(),(yyvsp[(3) - (3)].types)->get_string());
		 }
    break;

  case 665:
/* Line 1787 of yacc.c  */
#line 3706 "parser.y"
    { (yyval.TERSptr) = (yyvsp[(3) - (5)].TERSptr); }
    break;

  case 667:
/* Line 1787 of yacc.c  */
#line 3711 "parser.y"
    {
		     (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), (yyvsp[(3) - (3)].TERSptr), "||");
    		   }
    break;

  case 668:
/* Line 1787 of yacc.c  */
#line 3717 "parser.y"
    {
		     string s;
		     string sb;
		     TERstructADT x,y,z;
		     if((yyvsp[(3) - (7)].types)->get_string() == "'1'") {
		       actionADT a = action((yyvsp[(1) - (7)].types)->get_string() + '+');
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[(5) - (7)].intval), (yyvsp[(7) - (7)].intval), FALSE);
		     } else if((yyvsp[(3) - (7)].types)->get_string() == "'0'") {
		       actionADT a = action((yyvsp[(1) - (7)].types)->get_string() + '-');
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[(5) - (7)].intval), (yyvsp[(7) - (7)].intval), FALSE);
		     } else {
		       actionADT a = dummyE();
		       a->list_assigns = addAssign(a->list_assigns, (yyvsp[(1) - (7)].types)->get_string(), (yyvsp[(3) - (7)].types)->get_string());
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[(5) - (7)].intval), (yyvsp[(7) - (7)].intval), TRUE);
		     } /* else {
		       s = '[' + $3->get_string() + ']';
		       sb = "[~(" + $3->get_string() + ")]";
		       actionADT a = action($1->get_string() + '+');
		       actionADT b = action($1->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(b, FALSE, $5, $7, FALSE, sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		       }*/
		   }
    break;

  case 669:
/* Line 1787 of yacc.c  */
#line 3748 "parser.y"
    { 
		    if (table->lookup(*((yyvsp[(1) - (6)].str_list)->begin())).first == "qty") {
		      (yyval.TERSptr) = Guard(*((yyvsp[(1) - (6)].str_list)->begin()) + "'dot:=" + 
				 (yyvsp[(5) - (6)].types)->theExpression);
		    }
		    else
		      (yyval.TERSptr) = 0;
		  }
    break;

  case 670:
/* Line 1787 of yacc.c  */
#line 3789 "parser.y"
    {
		     (yyval.TERSptr) = (yyvsp[(3) - (5)].TERSptr);
		   }
    break;

  case 672:
/* Line 1787 of yacc.c  */
#line 3795 "parser.y"
    {
		     (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), (yyvsp[(3) - (3)].TERSptr), "||");
    		   }
    break;

  case 673:
/* Line 1787 of yacc.c  */
#line 3800 "parser.y"
    {
		     string s;
		     string sb;
		     TERstructADT x,y,z;
		     if((yyvsp[(3) - (7)].types)->get_string() == "'1'") {
		       s = "[~" + (yyvsp[(1) - (7)].types)->get_string() + ']';
		       sb = '[' + (yyvsp[(1) - (7)].types)->get_string() + ']';
		       actionADT a = action((yyvsp[(1) - (7)].types)->get_string() + '+');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, (yyvsp[(5) - (7)].intval), (yyvsp[(7) - (7)].intval), FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,(yyvsp[(5) - (7)].intval),(yyvsp[(7) - (7)].intval),FALSE,sb.c_str());
		       (yyval.TERSptr) = TERScompose(y,z,"|");
		       (yyval.TERSptr) = TERScompose(x,(yyval.TERSptr),";");
		     } else if((yyvsp[(3) - (7)].types)->get_string() == "'0'") {
		       s = '[' + (yyvsp[(1) - (7)].types)->get_string() + ']';
		       sb = "[~" + (yyvsp[(1) - (7)].types)->get_string() + ']';
		       actionADT a = action((yyvsp[(1) - (7)].types)->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, (yyvsp[(5) - (7)].intval), (yyvsp[(7) - (7)].intval), FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,(yyvsp[(5) - (7)].intval),(yyvsp[(7) - (7)].intval),FALSE,sb.c_str());
		       (yyval.TERSptr) = TERScompose(y,z,"|");
		       (yyval.TERSptr) = TERScompose(x,(yyval.TERSptr),";");
		     } else {
		       actionADT a = dummyE();
		       a->list_assigns = addAssign(a->list_assigns, (yyvsp[(1) - (7)].types)->get_string(), (yyvsp[(3) - (7)].types)->get_string());
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[(5) - (7)].intval), (yyvsp[(7) - (7)].intval), TRUE);
		     } /* else {
		       s = '[' + $3->get_string() + ']';
		       sb = "[~(" + $3->get_string() + ")]";
		       actionADT a = action($1->get_string() + '+');
		       actionADT b = action($1->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, $5, $7, FALSE, s.c_str());
		       z = TERS(b, FALSE, $5, $7, FALSE, sb.c_str());
		       $$ = TERScompose(y,z,"|");
		       $$ = TERScompose(x,$$,";");
		       }*/
		   }
    break;

  case 674:
/* Line 1787 of yacc.c  */
#line 3883 "parser.y"
    {   /*
		       if(check_type($1, $4))
		     err_msg("left and right sides of",
	                          " '<=' have different types", ";");*/
		   (yyvsp[(4) - (5)].types)->set_str((yyvsp[(4) - (5)].types)->theExpression);
		   (yyval.TERSptr) = signalAssignment((yyvsp[(1) - (5)].types),(yyvsp[(4) - (5)].types));
	         }
    break;

  case 675:
/* Line 1787 of yacc.c  */
#line 3891 "parser.y"
    {
		   (yyvsp[(3) - (4)].types)->set_str((yyvsp[(3) - (4)].types)->theExpression);
		   (yyval.TERSptr) = signalAssignment((yyvsp[(1) - (4)].types),(yyvsp[(3) - (4)].types));
		 }
    break;

  case 676:
/* Line 1787 of yacc.c  */
#line 3902 "parser.y"
    {
		    (yyvsp[(5) - (6)].types)->set_str((yyvsp[(5) - (6)].types)->theExpression);
		    (yyval.TERSptr) = signalAssignment((yyvsp[(2) - (6)].types),(yyvsp[(5) - (6)].types));
		    /*
		    if(check_type($2, $5))
		      err_msg("left and right sides of '<=' have",
			      " different types", ";");
		    $$= TERSempty();*/
	          }
    break;

  case 677:
/* Line 1787 of yacc.c  */
#line 3913 "parser.y"
    {
		    (yyvsp[(4) - (5)].types)->set_str((yyvsp[(4) - (5)].types)->theExpression);
		    (yyval.TERSptr) = signalAssignment((yyvsp[(2) - (5)].types),(yyvsp[(4) - (5)].types));
		    /*
		    if(check_type($2, $4))
		      err_msg("left and right sides of '<=' have",
	                        " different types", ";");
		    $$= TERSempty();  */
                  }
    break;

  case 678:
/* Line 1787 of yacc.c  */
#line 3924 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 679:
/* Line 1787 of yacc.c  */
#line 3926 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (3)].types);
		   /*
		   if(!check_type($1, $3))
		     $$= $1;
		   else if($1->isUnknown())
		     $$= $3; */
		 }
    break;

  case 680:
/* Line 1787 of yacc.c  */
#line 3935 "parser.y"
    {
                   (yyval.types) = new TYPES();
		   (yyval.types)->set_str("unaffected");
		 }
    break;

  case 681:
/* Line 1787 of yacc.c  */
#line 3942 "parser.y"
    {   	
                   (yyval.types) = (yyvsp[(1) - (1)].types);
		   TYPES lower; lower.set_int(0);
		   TYPES upper; upper.set_int(0);
		   str_ty_lst ll(1, make_pair(string(), lower));
		   ll.push_back(make_pair(string(), upper));
		   (yyval.types)->set_list(TYPELIST(ll));
                 }
    break;

  case 682:
/* Line 1787 of yacc.c  */
#line 3951 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (3)].types);
		   if((yyvsp[(3) - (3)].types)->get_list() == 0){
		     TYPES upper; upper.set_int(INFIN);
		     str_ty_lst LL(1, make_pair(string(), *(yyvsp[(3) - (3)].types)));
		     LL.push_back(make_pair(string(), upper));
		     (yyval.types)->set_list(TYPELIST(LL));
		   }
		   else
		     (yyval.types)->set_list((yyvsp[(3) - (3)].types)->get_list());
		   delete (yyvsp[(3) - (3)].types);
		 }
    break;

  case 683:
/* Line 1787 of yacc.c  */
#line 3968 "parser.y"
    {
		   /*
		   if(check_type($2, $4))
		     err_msg("left and right sides of ':='",
			     " have different types", ";");*/
		   if (table->lookup((yyvsp[(2) - (5)].types)->get_string()).first == "qty") {
		     (yyval.TERSptr) = Guard((yyvsp[(2) - (5)].types)->get_string() + ":=" + (yyvsp[(4) - (5)].types)->theExpression);
		   }
		   else
		     (yyval.TERSptr) = 0;
		 }
    break;

  case 684:
/* Line 1787 of yacc.c  */
#line 3980 "parser.y"
    { /*
		   if(check_type($1, $3))
		     err_msg("left and right sides of ':='",
			     " have different types", ";");*/
		   if (table->lookup((yyvsp[(1) - (4)].types)->get_string()).first == "qty") {
		     (yyval.TERSptr) = Guard((yyvsp[(1) - (4)].types)->get_string() + ":=" + (yyvsp[(3) - (4)].types)->theExpression);
		   }
		   else
		     (yyval.TERSptr) = 0;
		 }
    break;

  case 685:
/* Line 1787 of yacc.c  */
#line 3997 "parser.y"
    {}
    break;

  case 686:
/* Line 1787 of yacc.c  */
#line 4003 "parser.y"
    {
		   if((yyvsp[(5) - (10)].ty_tel_lst)->size() == 1 && (yyvsp[(5) - (10)].ty_tel_lst)->front().first.str() == "wait")
		     (yyval.TERSptr) = (yyvsp[(5) - (10)].ty_tel_lst)->front().second;
		   else {
		     telADT T5 = telcompose(*(yyvsp[(5) - (10)].ty_tel_lst));
		     string EN;

		     //condition vs. expression
		     if((yyvsp[(3) - (10)].types)->data_type() != string("bool"))
		       EN = (yyvsp[(3) - (10)].types)->theExpression;
		     else
		       EN = (yyvsp[(3) - (10)].types)->str();
		     
		     string ENC = my_not(EN);
		     string CC = '['+ EN +']';
		     actionADT AA = dummyE();
		     (yyval.TERSptr) = TERS(AA, FALSE, 0, 0, TRUE, CC.c_str());
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), T5, ";");
		     if((yyvsp[(6) - (10)].ty_tel_lst)){
		       ty_tel_lst::iterator BB;
		       for(BB = (yyvsp[(6) - (10)].ty_tel_lst)->begin(); BB != (yyvsp[(6) - (10)].ty_tel_lst)->end(); BB++){

			 if (BB->first.data_type() == string("bool"))
			   CC = '['+ logic(ENC, BB->first.str()) +']';
			 else
			   CC = '[' + logic(ENC, BB->first.theExpression)
			     + ']';
			     
			 telADT TT = TERS(dummyE(),FALSE,0,0,TRUE, CC.c_str());
			 telADT TTT = TERScompose(TT, BB->second, ";");
			 (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), TTT), "|");
			 if (BB->first.data_type() == string("bool"))
			   ENC = logic(ENC, my_not(BB->first.str()));
			 else
			   ENC = logic(ENC, my_not(BB->first.theExpression));
		       }
		     }
		     CC = '['+ ENC +']';
		     telADT LL = TERS(dummyE(), FALSE,0,0,TRUE, CC.c_str());
		     telADT LLL = TERScompose(LL, (yyvsp[(7) - (10)].TERSptr), ";");
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), LLL), "|");
		   }
		   delete (yyvsp[(5) - (10)].ty_tel_lst);
		 }
    break;

  case 687:
/* Line 1787 of yacc.c  */
#line 4049 "parser.y"
    {
		   if((yyvsp[(5) - (11)].ty_tel_lst)->size() == 1 && (yyvsp[(5) - (11)].ty_tel_lst)->front().first.str() == "wait")
		     (yyval.TERSptr) = (yyvsp[(5) - (11)].ty_tel_lst)->front().second;
		   else {
		     telADT T5 = telcompose(*(yyvsp[(5) - (11)].ty_tel_lst));
		     string EN;

		     //condition vs. expression
		     if((yyvsp[(3) - (11)].types)->data_type() != string("bool"))
		       EN = (yyvsp[(3) - (11)].types)->theExpression;
		     else
		       EN = (yyvsp[(3) - (11)].types)->str();
		     
		     string ENC = my_not(EN);
		     string CC = '['+ EN +']';
		     actionADT AA = dummyE();
		     (yyval.TERSptr) = TERS(AA, FALSE, 0, 0, TRUE, CC.c_str());
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), T5, ";");
		     if((yyvsp[(6) - (11)].ty_tel_lst)){
		       ty_tel_lst::iterator BB;
		       for(BB = (yyvsp[(6) - (11)].ty_tel_lst)->begin(); BB != (yyvsp[(6) - (11)].ty_tel_lst)->end(); BB++){

			 if (BB->first.data_type() == string("bool"))
			   CC = '['+ logic(ENC, BB->first.str()) +']';
			 else
			   CC = '[' + logic(ENC, BB->first.theExpression)
			     + ']';
			     
			 telADT TT = TERS(dummyE(),FALSE,0,0,TRUE, CC.c_str());
			 telADT TTT = TERScompose(TT, BB->second, ";");
			 (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), TTT), "|");
			 if (BB->first.data_type() == string("bool"))
			   ENC = logic(ENC, my_not(BB->first.str()));
			 else
			   ENC = logic(ENC, my_not(BB->first.theExpression));
		       }
		     }
		     CC = '['+ ENC +']';
		     telADT LL = TERS(dummyE(), FALSE,0,0,TRUE, CC.c_str());
		     telADT LLL = TERScompose(LL, (yyvsp[(7) - (11)].TERSptr), ";");
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), LLL), "|");
		   }
		   delete (yyvsp[(5) - (11)].ty_tel_lst);
		   if((yyvsp[(1) - (11)].C_str) && (yyvsp[(10) - (11)].actionptr)->label && strcmp_nocase((yyvsp[(1) - (11)].C_str), (yyvsp[(10) - (11)].actionptr)->label)==false)
		     err_msg((yyvsp[(10) - (11)].actionptr)->label, " is not the if statement label.");
		 }
    break;

  case 688:
/* Line 1787 of yacc.c  */
#line 4097 "parser.y"
    { (yyval.ty_tel_lst) = 0; }
    break;

  case 689:
/* Line 1787 of yacc.c  */
#line 4099 "parser.y"
    {
		   if((yyvsp[(1) - (5)].ty_tel_lst)) (yyval.ty_tel_lst) = (yyvsp[(1) - (5)].ty_tel_lst);
		   else (yyval.ty_tel_lst) = new ty_tel_lst;
		   (yyval.ty_tel_lst)->push_back(make_pair(*(yyvsp[(3) - (5)].types), telcompose(*(yyvsp[(5) - (5)].ty_tel_lst))));
		   (yyval.ty_tel_lst)->back().first.theExpression = (yyvsp[(3) - (5)].types)->theExpression;
		   delete (yyvsp[(5) - (5)].ty_tel_lst); delete (yyvsp[(3) - (5)].types);
		 }
    break;

  case 690:
/* Line 1787 of yacc.c  */
#line 4108 "parser.y"
    { (yyval.TERSptr) = 0;}
    break;

  case 691:
/* Line 1787 of yacc.c  */
#line 4110 "parser.y"
    { (yyval.TERSptr) = telcompose(*(yyvsp[(2) - (2)].ty_tel_lst)); delete (yyvsp[(2) - (2)].ty_tel_lst);}
    break;

  case 692:
/* Line 1787 of yacc.c  */
#line 4113 "parser.y"
    { (yyval.C_str) = 0; }
    break;

  case 694:
/* Line 1787 of yacc.c  */
#line 4117 "parser.y"
    {  (yyval.C_str) = (yyvsp[(1) - (2)].actionptr)->label;	 }
    break;

  case 695:
/* Line 1787 of yacc.c  */
#line 4120 "parser.y"
    { (yyval.ty_tel_lst) = 0; }
    break;

  case 696:
/* Line 1787 of yacc.c  */
#line 4122 "parser.y"
    {
		   if((yyvsp[(1) - (2)].ty_tel_lst)) (yyval.ty_tel_lst) = (yyvsp[(1) - (2)].ty_tel_lst);
		   else   (yyval.ty_tel_lst) = new list<pair<TYPES,telADT> >;
		   (yyval.ty_tel_lst)->push_back(*(yyvsp[(2) - (2)].ty_tel2));
		   delete (yyvsp[(2) - (2)].ty_tel2);
                 }
    break;

  case 697:
/* Line 1787 of yacc.c  */
#line 4132 "parser.y"
    {
		   (yyval.TERSptr) = 0;
		   ty_tel_lst::iterator b;
		   string SS = table->lookup((yyvsp[(3) - (8)].types)->get_string()).first;
		   for(b = (yyvsp[(5) - (8)].ty_tel_lst)->begin(); b != (yyvsp[(5) - (8)].ty_tel_lst)->end(); b++){
		     telADT tt = 0;
		     if(SS == "var")
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE);
		     else {
		       string ss='['+bool_relation((yyvsp[(3) - (8)].types)->str(),b->first.str())+']';
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE, ss.c_str());
		     }
		     telADT T = TERScompose(tt,TERSrename(tt,b->second),";");
		     if((yyval.TERSptr))
		       (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), T), "|");
		     else
		       (yyval.TERSptr) = T;
		   }
//		   $$ = TERScompose(TERS(dummyE(),FALSE, 0, 0, TRUE), $$, "|");
		   delete (yyvsp[(5) - (8)].ty_tel_lst);
		 }
    break;

  case 698:
/* Line 1787 of yacc.c  */
#line 4155 "parser.y"
    {
		   (yyval.TERSptr) = 0;
		   ty_tel_lst::iterator b;
		   string SS = table->lookup((yyvsp[(3) - (9)].types)->get_string()).first;
		   for(b = (yyvsp[(5) - (9)].ty_tel_lst)->begin(); b != (yyvsp[(5) - (9)].ty_tel_lst)->end(); b++){
		     telADT tt = 0;
		     if(SS == "var")
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE);
		     else {
		       string ss='['+bool_relation((yyvsp[(3) - (9)].types)->str(),b->first.str())+']';
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE, ss.c_str());
		     }
		     telADT T = TERScompose(tt,TERSrename(tt,b->second),";");
		     if((yyval.TERSptr))
		       (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), T), "|");
		     else
		       (yyval.TERSptr) = T;
		   }
//		   $$ = TERScompose(TERS(dummyE(),FALSE, 0, 0, TRUE), $$, "|");
		   delete (yyvsp[(5) - (9)].ty_tel_lst);
		 }
    break;

  case 699:
/* Line 1787 of yacc.c  */
#line 4179 "parser.y"
    {
		   (yyval.ty_tel_lst) = new list<pair<TYPES,telADT> >;
		   TYPES ty("uk", "bool");
		   if((yyvsp[(2) - (4)].ty_lst)){
		     list<string> str_l;
		     for(list<TYPES>::iterator b = (yyvsp[(2) - (4)].ty_lst)->begin();
			 b != (yyvsp[(2) - (4)].ty_lst)->end(); b++)
		       str_l.push_back(b->str());
		     ty.set_str(logic(str_l, "|"));
		   }
		   else{
		     ty.set_grp(TYPES::Error);
		     ty.set_str("false");
		   }
		   (yyval.ty_tel_lst)->push_back(make_pair(ty, telcompose(*(yyvsp[(4) - (4)].ty_tel_lst))));
		   delete (yyvsp[(2) - (4)].ty_lst); delete (yyvsp[(4) - (4)].ty_tel_lst);
		 }
    break;

  case 700:
/* Line 1787 of yacc.c  */
#line 4198 "parser.y"
    {
		   (yyval.ty_tel_lst) = (yyvsp[(1) - (5)].ty_tel_lst);
		   TYPES ty("uk", "bool");
		   if((yyvsp[(3) - (5)].ty_lst)){
		     list<string> str_l;
		     for(list<TYPES>::iterator b = (yyvsp[(3) - (5)].ty_lst)->begin();
			 b != (yyvsp[(3) - (5)].ty_lst)->end(); b++)
		       str_l.push_back(b->str());
		     ty.set_str(logic(str_l, "|"));
		   }
		   else{
		     ty.set_grp(TYPES::Error);
		     ty.set_str("false");
		   }
		   (yyval.ty_tel_lst)->push_back(make_pair(ty, telcompose(*(yyvsp[(5) - (5)].ty_tel_lst))));
		   delete (yyvsp[(3) - (5)].ty_lst); delete (yyvsp[(5) - (5)].ty_tel_lst);
		 }
    break;

  case 701:
/* Line 1787 of yacc.c  */
#line 4219 "parser.y"
    {
		   telADT T4 = telcompose(*(yyvsp[(4) - (7)].ty_tel_lst)); delete (yyvsp[(4) - (7)].ty_tel_lst);
		   if((yyvsp[(2) - (7)].types) == 0)
		     (yyval.TERSptr) = TERSempty();
		   else{
		     actionADT a = dummyE();
		     string ss =  "[" + (yyvsp[(2) - (7)].types)->str() + "]";
		     TERstructADT t3 = TERS(a, FALSE, 0, 0, TRUE, ss.c_str());
		     TERstructADT loop_body =
		       TERScompose(t3, TERSrename(t3, T4), ";");

		     loop_body = TERSmakeloop(loop_body);
		
		     // create the tel structure for loop exit.
		     string s = '['+ my_not((yyvsp[(2) - (7)].types)->str()) + ']';
		     TERstructADT loop_exit = TERS(dummyE(), FALSE, 0, 0,
						   TRUE, s.c_str());
		     // make conflict between loop body and loop exit
		     (yyval.TERSptr) = TERScompose(loop_body,TERSrename(loop_body,
							   loop_exit), "|");
		       		
		     (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		   }
                 }
    break;

  case 702:
/* Line 1787 of yacc.c  */
#line 4245 "parser.y"
    {
		   telADT T4 = telcompose(*(yyvsp[(4) - (8)].ty_tel_lst)); delete (yyvsp[(4) - (8)].ty_tel_lst);
		   if((yyvsp[(2) - (8)].types) == 0)
		     (yyval.TERSptr) = TERSempty();
		   else{
		     string ss =  "[" + (yyvsp[(2) - (8)].types)->str() + "]";
		     telADT t3=TERS(dummyE(),FALSE,0,0,TRUE, ss.c_str());
		     telADT loop_body = TERScompose(t3,TERSrename(t3, T4),";");
		     loop_body = TERSmakeloop(loop_body);
		
		     // create the tel structure for loop exit.
		     string s = '[' + my_not((yyvsp[(2) - (8)].types)->str()) + ']';
		     telADT loop_exit= TERS(dummyE(),FALSE,0,0,TRUE,s.c_str());
		     // make conflict between loop body and loop exit
		     (yyval.TERSptr) = TERScompose(loop_body,TERSrename(loop_body,
							   loop_exit), "|");
		       		
		     (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");

		     if((yyvsp[(1) - (8)].C_str) && strcmp_nocase((yyvsp[(1) - (8)].C_str), (yyvsp[(7) - (8)].actionptr)->label)==false)
		       err_msg((yyvsp[(7) - (8)].actionptr)->label, " is not the loop statement label.");
		   }
                 }
    break;

  case 703:
/* Line 1787 of yacc.c  */
#line 4272 "parser.y"
    {
		   (yyval.types) = new TYPES();
		   (yyval.types)->set_str("true");
		   (yyval.types)->set_data("bool");
		 }
    break;

  case 704:
/* Line 1787 of yacc.c  */
#line 4278 "parser.y"
    { 	
		   (yyval.types) = (yyvsp[(2) - (2)].types);
		   if(strcmp_nocase((yyvsp[(2) - (2)].types)->data_type(), "bool")==false){
		     warn_msg("non-boolean expression used as condition");
		     (yyval.types)->set_grp(TYPES::Error);
		     (yyval.types)->set_str("maybe");
   		   }
		 }
    break;

  case 705:
/* Line 1787 of yacc.c  */
#line 4286 "parser.y"
    { (yyval.types) = 0; }
    break;

  case 706:
/* Line 1787 of yacc.c  */
#line 4289 "parser.y"
    {}
    break;

  case 707:
/* Line 1787 of yacc.c  */
#line 4290 "parser.y"
    { }
    break;

  case 708:
/* Line 1787 of yacc.c  */
#line 4291 "parser.y"
    {  }
    break;

  case 709:
/* Line 1787 of yacc.c  */
#line 4292 "parser.y"
    {}
    break;

  case 710:
/* Line 1787 of yacc.c  */
#line 4293 "parser.y"
    {}
    break;

  case 711:
/* Line 1787 of yacc.c  */
#line 4296 "parser.y"
    {}
    break;

  case 712:
/* Line 1787 of yacc.c  */
#line 4297 "parser.y"
    {}
    break;

  case 713:
/* Line 1787 of yacc.c  */
#line 4298 "parser.y"
    {}
    break;

  case 714:
/* Line 1787 of yacc.c  */
#line 4299 "parser.y"
    {}
    break;

  case 715:
/* Line 1787 of yacc.c  */
#line 4300 "parser.y"
    {}
    break;

  case 716:
/* Line 1787 of yacc.c  */
#line 4303 "parser.y"
    {}
    break;

  case 717:
/* Line 1787 of yacc.c  */
#line 4304 "parser.y"
    {}
    break;

  case 718:
/* Line 1787 of yacc.c  */
#line 4307 "parser.y"
    {}
    break;

  case 719:
/* Line 1787 of yacc.c  */
#line 4308 "parser.y"
    {}
    break;

  case 720:
/* Line 1787 of yacc.c  */
#line 4311 "parser.y"
    {}
    break;

  case 721:
/* Line 1787 of yacc.c  */
#line 4312 "parser.y"
    {}
    break;

  case 722:
/* Line 1787 of yacc.c  */
#line 4317 "parser.y"
    { (yyval.TERSptr)=Guard(*(yyvsp[(3) - (5)].str)); }
    break;

  case 723:
/* Line 1787 of yacc.c  */
#line 4318 "parser.y"
    { (yyval.TERSptr)=0; }
    break;

  case 724:
/* Line 1787 of yacc.c  */
#line 4321 "parser.y"
    {(yyval.TERSptr) = 0;}
    break;

  case 725:
/* Line 1787 of yacc.c  */
#line 4323 "parser.y"
    {(yyval.TERSptr) = Guard((yyvsp[(2) - (2)].types)->theExpression);}
    break;

  case 726:
/* Line 1787 of yacc.c  */
#line 4327 "parser.y"
    {(yyval.str) = (yyvsp[(1) - (1)].str);}
    break;

  case 727:
/* Line 1787 of yacc.c  */
#line 4329 "parser.y"
    {
		   if ((yyvsp[(1) - (3)].str) && (yyvsp[(3) - (3)].str)) {
		     (yyval.str) = new string((*(yyvsp[(1) - (3)].str)) + " & " + (*(yyvsp[(3) - (3)].str)));
		     delete (yyvsp[(1) - (3)].str);
		     delete (yyvsp[(3) - (3)].str);
		   } else if ((yyvsp[(1) - (3)].str)) {
		     (yyval.str) = (yyvsp[(1) - (3)].str);
		   } else {
		     (yyval.str) = (yyvsp[(3) - (3)].str);
		   }
		 }
    break;

  case 728:
/* Line 1787 of yacc.c  */
#line 4344 "parser.y"
    {(yyval.str) = 0;}
    break;

  case 729:
/* Line 1787 of yacc.c  */
#line 4346 "parser.y"
    {
		   (yyval.str) = new string((yyvsp[(1) - (3)].types)->theExpression + ":=" + (yyvsp[(3) - (3)].types)->theExpression); 
		 }
    break;

  case 731:
/* Line 1787 of yacc.c  */
#line 4357 "parser.y"
    {
		   if((yyvsp[(1) - (3)].str_bl_lst) && (yyvsp[(3) - (3)].str_bl_lst)){
		     (yyval.str_bl_lst) = (yyvsp[(3) - (3)].str_bl_lst);
		     (yyval.str_bl_lst)->splice((yyval.str_bl_lst)->begin(), *(yyvsp[(1) - (3)].str_bl_lst));
		     delete (yyvsp[(1) - (3)].str_bl_lst);
		   }
		   else if((yyvsp[(3) - (3)].str_bl_lst))
		     (yyval.str_bl_lst) = (yyvsp[(3) - (3)].str_bl_lst);
		   else
		     (yyval.str_bl_lst) = (yyvsp[(1) - (3)].str_bl_lst);
		 }
    break;

  case 732:
/* Line 1787 of yacc.c  */
#line 4370 "parser.y"
    { (yyval.str_bl_lst) = 0; }
    break;

  case 733:
/* Line 1787 of yacc.c  */
#line 4371 "parser.y"
    { (yyval.str_bl_lst) = (yyvsp[(1) - (1)].str_bl_lst);  }
    break;

  case 734:
/* Line 1787 of yacc.c  */
#line 4372 "parser.y"
    { (yyval.str_bl_lst) = 0;  }
    break;

  case 735:
/* Line 1787 of yacc.c  */
#line 4373 "parser.y"
    { (yyval.str_bl_lst) = 0; }
    break;

  case 736:
/* Line 1787 of yacc.c  */
#line 4374 "parser.y"
    { (yyval.str_bl_lst)= (yyvsp[(1) - (1)].str_bl_lst);  }
    break;

  case 737:
/* Line 1787 of yacc.c  */
#line 4376 "parser.y"
    { (yyval.str_bl_lst) = 0;}
    break;

  case 738:
/* Line 1787 of yacc.c  */
#line 4377 "parser.y"
    { (yyval.str_bl_lst) = 0;}
    break;

  case 739:
/* Line 1787 of yacc.c  */
#line 4381 "parser.y"
    {}
    break;

  case 740:
/* Line 1787 of yacc.c  */
#line 4387 "parser.y"
    {
		   (yyval.str_bl_lst) = new list<pair<string, int> >;
		   for(list<string>::iterator b=(yyvsp[(2) - (6)].str_list)->begin();b!=(yyvsp[(2) - (6)].str_list)->end(); b++){
		     int init = 0;
		     if((yyvsp[(6) - (6)].types)->get_string() == "'1'")
		       init = 1;
		     string LL = (*b);
		     actionADT a = action(LL);
		     if(strcmp_nocase((yyvsp[(4) - (6)].C_str), "in")){
		       makebooldecl(IN, a, init, NULL);
		       (yyval.str_bl_lst)->push_back(make_pair(LL, IN));
		     }
		     else{
		       makebooldecl(OUT, a, init, NULL);
		       (yyval.str_bl_lst)->push_back(make_pair(LL, OUT));
		     }
		   }
		 }
    break;

  case 741:
/* Line 1787 of yacc.c  */
#line 4408 "parser.y"
    { (yyval.C_str) = "in";}
    break;

  case 742:
/* Line 1787 of yacc.c  */
#line 4409 "parser.y"
    { (yyval.C_str) = "out";}
    break;

  case 743:
/* Line 1787 of yacc.c  */
#line 4410 "parser.y"
    {}
    break;

  case 744:
/* Line 1787 of yacc.c  */
#line 4417 "parser.y"
    {}
    break;

  case 745:
/* Line 1787 of yacc.c  */
#line 4419 "parser.y"
    {}
    break;

  case 746:
/* Line 1787 of yacc.c  */
#line 4424 "parser.y"
    {
		  //$4->set_obj("in");
		  (yyval.str_bl_lst) = new list<pair<string, int> >;
		  for(list<string>::iterator b=(yyvsp[(2) - (6)].str_list)->begin();b!=(yyvsp[(2) - (6)].str_list)->end(); b++){
		    int init = 0;
		    if((yyvsp[(6) - (6)].types)->get_string() == "'1'")
		      init = 1;
		    string LL((*b));
		    actionADT a = action(LL);
		    makebooldecl(IN, a, init, NULL);
		    (yyval.str_bl_lst)->push_back(make_pair(LL, IN));
		  }
		
		  /* for a signal of array
		     else{
		       TYPELIST *index =
			     $4->get_list()->value().second->get_list();
			   index->iteration_init();
			   int len = index->value().second->get_int();
			   index->next();
			   len = len - index->value().second->get_int();
			   int char_len = strlen($$->value().first);
			   char *name = 0;
			   for(int i = 0; i < len; i++)
			     {
			       name = new char[char_len+10];
			       sprintf(name, "%s__%d__+",
				       $$->value().first, i);
			     }
#ifdef DDEBUG
			   assert(name);
#endif
			   actionADT a = action(name, strlen(name));
			   delete [] name;
			
			   makebooldecl(IN, a, init, NULL);
			 }
		
		       $$->next();
		       }*/
		 }
    break;

  case 747:
/* Line 1787 of yacc.c  */
#line 4467 "parser.y"
    {
		  /*if(strcmp_nocase($4, "in") == 0)
		    $5->set_obj("in");
		  else if(strcmp_nocase($4, "out") == 0)
		    $5->set_obj("out");
		  else
		  $5->set_obj("in");*/
		  (yyval.str_bl_lst) = new list<pair<string, int> >;
		  for(list<string>::iterator b=(yyvsp[(2) - (7)].str_list)->begin();b!=(yyvsp[(2) - (7)].str_list)->end(); b++){
		    int init = 0;
		    if((yyvsp[(7) - (7)].types)->get_string() == "'1'")
		      init = 1;
		    string LL = (*b);
		    actionADT a = action(LL);
		    if(strcmp_nocase((yyvsp[(4) - (7)].C_str), "in")){
		      makebooldecl(IN, a, init, NULL);
		      (yyval.str_bl_lst)->push_back(make_pair(LL, IN));
		    }
		    else{
		      makebooldecl(OUT, a, init, NULL);
		      (yyval.str_bl_lst)->push_back(make_pair(LL, OUT));
		    }
		  }
		
		   /*
		   $$->iteration_init();
		   while($$->end() == false)
		     {
		       TYPES *ty = new TYPES($5);
		       $$->value().second = ty;
		
		       int init = 0;
		       if($7)
			 init = !strcmp_nocase($7->get_string(), "'1'");
		
		       if($5->grp_id() != TYPES::Array)
			 {
			   char *name = copy_string($$->value().first);
			   actionADT a = action(name, strlen(name));
			   delete [] name;
			   if(strcmp_nocase($4, "out") == 0)
			     makebooldecl(OUT, a, init, NULL);
			   else
			     makebooldecl(IN, a, init, NULL);
			 }
		       else
			 {
			   TYPELIST *index =
			     $5->get_list();//->value().second->get_list();
			   index->iteration_init();
			   int len = index->value().second->get_int();
			   index->next();
			   len = len - index->value().second->get_int();
			   int char_len = strlen($$->value().first);
			   char *name = 0;
			   for(int i = 0; i < len; i++)
			     {
			       name = new char[char_len+10];
			       sprintf(name, "%s__%d__+",
				       $$->value().first, i);
			     }
#ifdef DDEBUG
			   assert(name);
#endif
			   actionADT a = action(name, strlen(name));
			   delete [] name;
			   if(strcmp_nocase($4, "out") == 0)
			     makebooldecl(OUT, a, init, NULL);
			   else
			     makebooldecl(IN, a, init, NULL);
			 }
			 		
		       $$->next();
		       }*/
		 }
    break;

  case 748:
/* Line 1787 of yacc.c  */
#line 4546 "parser.y"
    {}
    break;

  case 749:
/* Line 1787 of yacc.c  */
#line 4549 "parser.y"
    {}
    break;

  case 752:
/* Line 1787 of yacc.c  */
#line 4558 "parser.y"
    { (yyval.types) = new TYPES; (yyval.types)->set_str("'0'"); }
    break;

  case 753:
/* Line 1787 of yacc.c  */
#line 4560 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (2)].types); }
    break;

  case 754:
/* Line 1787 of yacc.c  */
#line 4565 "parser.y"
    {
		 //$3->set_obj("in");
		 (yyval.str_bl_lst) = new list<pair<string,int> >;
		 for(list<string>::iterator b=(yyvsp[(1) - (5)].str_list)->begin(); b!=(yyvsp[(1) - (5)].str_list)->end(); b++){
		   int init = 0;
		   if((yyvsp[(5) - (5)].types)->str() == "'1'")
		     init = 1;
		   const string LL = (*b);
		   actionADT a = action(LL);
		   makebooldecl(IN, a, init, NULL);
		   (yyval.str_bl_lst)->push_back(make_pair(LL, IN));
		 }
		 delete (yyvsp[(1) - (5)].str_list);
	       }
    break;

  case 755:
/* Line 1787 of yacc.c  */
#line 4581 "parser.y"
    {
		 (yyval.str_bl_lst) = new list<pair<string,int> >;
		 list<string> *sufficies = new list<string>;
		 int type(IN);
		 string assigned((yyvsp[(6) - (6)].types)->get_string());
		 delayADT myBounds;
		 int rise_min(0), rise_max(0), fall_min(-1), fall_max(-1);
		 if("init_channel" == assigned ||
		    "active"  == assigned ||
		    "passive" == assigned ){
		   sufficies->push_back(SEND_SUFFIX);
		   sufficies->push_back(RECEIVE_SUFFIX);
		   TYPELIST arguments((yyvsp[(6) - (6)].types)->get_list());
		   TYPELIST::iterator i(arguments.begin());
		   while(i!=arguments.end()){
		     if("timing"==i->second.get_string()){
		       if("sender"==i->first){
			 type |= SENT;
		       }
		       if("receiver"==i->first){
			 type |= RECEIVED;
		       }
		       TYPELIST bounds(i->second.get_list());
		       TYPELIST::iterator j(bounds.begin());
		       int pos(1);
		       while(j != bounds.end()){
			 string formal(j->first);
			 int actual(j->second.get_int());
			 if("rise_min"==formal || "lower"==formal ||
			    formal.empty() && 1==pos){
			   rise_min = actual;
			 }
			 else if("rise_max"==formal || "upper"==formal ||
				 formal.empty() && 2==pos){
			   rise_max = actual;
			 }
			 else if("fall_min"==formal || "lower"==formal ||
				 formal.empty() && 3==pos){
			   fall_min = actual;
			 }
			 else if("fall_max"==formal || "upper"==formal ||
				 formal.empty() && 4==pos){
			   fall_max = actual;
			 }
			 j++;
			 pos++;
		       }
		     }
		     i++;
		   }
		   if("active"==assigned){
		     type |= ACTIVE;
		   }
		   else if("passive"==assigned){
		     type |= PASSIVE;
		   }
		 }
		 else{
		   sufficies->push_back("" );
		   if(strcmp_nocase((yyvsp[(3) - (6)].C_str),"out")){
		     type = OUT;
		   }
		 }
		 if(fall_min<0){
		   fall_min = rise_min;
		 }
		 if(fall_max<0){
		   fall_max = rise_max;
		 }
		 assigndelays(&myBounds,
			      rise_min,rise_max,NULL,
			      fall_min,fall_max,NULL);
		 int init = 0;
		 if((yyvsp[(6) - (6)].types)->str() == "'1'"){
		   init = 1;
		 }
		 for(list<string>::iterator b=(yyvsp[(1) - (6)].str_list)->begin(); b!=(yyvsp[(1) - (6)].str_list)->end(); b++){
		   (yyval.str_bl_lst)->push_back(make_pair((*b), type));
		   list<string>::iterator suffix=sufficies->begin();
		   while ( suffix != sufficies->end() ) {
		     makebooldecl(type,action((*b)+*suffix),init,
				  &myBounds);
		     suffix++;
		   }
		 }
		 delete sufficies;
		 delete (yyvsp[(1) - (6)].str_list);
	       }
    break;

  case 756:
/* Line 1787 of yacc.c  */
#line 4672 "parser.y"
    {}
    break;

  case 757:
/* Line 1787 of yacc.c  */
#line 4676 "parser.y"
    {   (yyval.types)= (yyvsp[(1) - (1)].types);   }
    break;

  case 758:
/* Line 1787 of yacc.c  */
#line 4678 "parser.y"
    {   (yyval.types)= (yyvsp[(3) - (3)].types);   }
    break;

  case 759:
/* Line 1787 of yacc.c  */
#line 4681 "parser.y"
    { (yyval.C_str)= "in"; }
    break;

  case 760:
/* Line 1787 of yacc.c  */
#line 4681 "parser.y"
    { (yyval.C_str)= "out"; }
    break;

  case 761:
/* Line 1787 of yacc.c  */
#line 4682 "parser.y"
    { (yyval.C_str)= "out"; }
    break;

  case 762:
/* Line 1787 of yacc.c  */
#line 4682 "parser.y"
    {(yyval.C_str)= "out"; }
    break;

  case 763:
/* Line 1787 of yacc.c  */
#line 4683 "parser.y"
    { (yyval.C_str)= "linkage"; }
    break;

  case 764:
/* Line 1787 of yacc.c  */
#line 4687 "parser.y"
    {
		   (yyval.typelist) = (yyvsp[(2) - (3)].typelist);
      		 }
    break;

  case 766:
/* Line 1787 of yacc.c  */
#line 4694 "parser.y"
    {
		   
		   (yyval.typelist) = (yyvsp[(1) - (3)].typelist);
		   (yyval.typelist)->front().second.theExpression += ";" +
		     (yyvsp[(3) - (3)].typelist)->front().second.theExpression;
		   (yyval.typelist)->combine(*(yyvsp[(3) - (3)].typelist));
		   delete (yyvsp[(3) - (3)].typelist);
		 }
    break;

  case 767:
/* Line 1787 of yacc.c  */
#line 4705 "parser.y"
    {
		   (yyval.typelist) = new TYPELIST(make_pair(string(), *(yyvsp[(1) - (1)].types)));
		   (yyval.typelist)->front().second.theExpression = (yyvsp[(1) - (1)].types)->theExpression;
		   delete (yyvsp[(1) - (1)].types);
		 }
    break;

  case 768:
/* Line 1787 of yacc.c  */
#line 4710 "parser.y"
    {  (yyval.typelist) = (yyvsp[(1) - (1)].typelist) ;  }
    break;

  case 769:
/* Line 1787 of yacc.c  */
#line 4712 "parser.y"
    {
 		   (yyval.typelist) = new TYPELIST(make_pair(*(yyvsp[(1) - (3)].str)     , *(yyvsp[(3) - (3)].types)));
		   delete (yyvsp[(1) - (3)].str);  delete (yyvsp[(3) - (3)].types);
		 }
    break;

  case 770:
/* Line 1787 of yacc.c  */
#line 4717 "parser.y"
    {
		   //$$= new TYPES("uk","uk", "uk");
		 }
    break;

  case 771:
/* Line 1787 of yacc.c  */
#line 4723 "parser.y"
    {
		   (yyval.str_str_l) = new list<pair<string,string> >;
                   (yyval.str_str_l)->push_back(*(yyvsp[(1) - (1)].csp_pair));   delete (yyvsp[(1) - (1)].csp_pair);
		 }
    break;

  case 772:
/* Line 1787 of yacc.c  */
#line 4728 "parser.y"
    { (yyval.str_str_l) = (yyvsp[(1) - (3)].str_str_l);   (yyval.str_str_l)->push_back(*(yyvsp[(3) - (3)].csp_pair)); delete (yyvsp[(3) - (3)].csp_pair); }
    break;

  case 773:
/* Line 1787 of yacc.c  */
#line 4732 "parser.y"
    { (yyval.csp_pair) = new pair<string,string>(string(), (yyvsp[(1) - (1)].types)->str()); delete (yyvsp[(1) - (1)].types); }
    break;

  case 774:
/* Line 1787 of yacc.c  */
#line 4734 "parser.y"
    {
		   (yyval.csp_pair) = new pair<string,string>(*(yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].types)->str());
		   delete (yyvsp[(1) - (3)].str);  delete (yyvsp[(3) - (3)].types);
		 }
    break;

  case 775:
/* Line 1787 of yacc.c  */
#line 4740 "parser.y"
    { (yyval.str) = new string((yyvsp[(1) - (1)].types)->str());  delete (yyvsp[(1) - (1)].types); }
    break;

  case 777:
/* Line 1787 of yacc.c  */
#line 4744 "parser.y"
    { (yyval.types) = new TYPES; }
    break;

  case 778:
/* Line 1787 of yacc.c  */
#line 4751 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (2)].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[(1) - (2)].types)->str(), (yyvsp[(2) - (2)].types)->str(), "&"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '&' +
		     (yyvsp[(2) - (2)].types)->theExpression;
		   delete (yyvsp[(2) - (2)].types);
		 }
    break;

  case 779:
/* Line 1787 of yacc.c  */
#line 4760 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (2)].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[(1) - (2)].types)->str(), (yyvsp[(2) - (2)].types)->str(), "|"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '|' +
		     (yyvsp[(2) - (2)].types)->theExpression;
		   delete (yyvsp[(2) - (2)].types);
		 }
    break;

  case 780:
/* Line 1787 of yacc.c  */
#line 4769 "parser.y"
    {
		   string one_bar=("~("+(yyvsp[(1) - (2)].types)->str())+')';
		   string two_bar=("~("+(yyvsp[(2) - (2)].types)->str())+')';
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic(logic((yyvsp[(1) - (2)].types)->str(), two_bar, "&"),
		     logic(one_bar, (yyvsp[(2) - (2)].types)->str(), "&"), "|"));
		   delete (yyvsp[(2) - (2)].types);
		   //$$= new TYPES1(check_logic($1, $2, "'xor'" ));
                   //$$->setTers(TERSempty());
                 }
    break;

  case 781:
/* Line 1787 of yacc.c  */
#line 4780 "parser.y"
    {
		   (yyvsp[(1) - (3)].types)->set_str(("~("+(yyvsp[(1) - (3)].types)->str())+')');
		   (yyvsp[(3) - (3)].types)->set_str(("~("+(yyvsp[(3) - (3)].types)->str())+')');
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[(1) - (3)].types)->str(), (yyvsp[(3) - (3)].types)->str(), "|"));
		   delete (yyvsp[(3) - (3)].types);
		   //$$= new TYPES1(check_logic($1, $3, "'nand'"));
                   //$$->setTers( TERSempty());
                 }
    break;

  case 782:
/* Line 1787 of yacc.c  */
#line 4790 "parser.y"
    {
		   (yyvsp[(1) - (3)].types)->set_str(("~("+(yyvsp[(1) - (3)].types)->str())+')');
		   (yyvsp[(3) - (3)].types)->set_str(("~("+(yyvsp[(3) - (3)].types)->str())+')');
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[(1) - (3)].types)->str(), (yyvsp[(3) - (3)].types)->str(), "&"));
		   delete (yyvsp[(3) - (3)].types);
		   //$$= new TYPES1(check_logic($1, $3, "'nor'"));
                   //$$->setTers( TERSempty());
                 }
    break;

  case 783:
/* Line 1787 of yacc.c  */
#line 4800 "parser.y"
    {
		   string one_bar=("~("+(yyvsp[(1) - (3)].types)->str())+')';
		   string three_bar=("~("+(yyvsp[(3) - (3)].types)->str())+')';
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic(logic((yyvsp[(1) - (3)].types)->str(), (yyvsp[(3) - (3)].types)->str(), "&"),
		     logic(one_bar, three_bar, "&"), "|"));
		   delete (yyvsp[(3) - (3)].types);
		   /* NOTE: should $1 be deleted also??? */
		   //$$= new TYPES1(check_logic($1, $3, "'xnor'"));
                   //$$->setTers(TERSempty());
                 }
    break;

  case 784:
/* Line 1787 of yacc.c  */
#line 4811 "parser.y"
    { (yyval.types) = (yyvsp[(1) - (1)].types); }
    break;

  case 785:
/* Line 1787 of yacc.c  */
#line 4815 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (3)].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[(1) - (3)].types)->str(), (yyvsp[(3) - (3)].types)->str(), "&"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '&' +
		     (yyvsp[(3) - (3)].types)->theExpression;
		   delete (yyvsp[(3) - (3)].types);
                 }
    break;

  case 786:
/* Line 1787 of yacc.c  */
#line 4824 "parser.y"
    {
		   (yyval.types) = (yyvsp[(2) - (2)].types);
		 }
    break;

  case 787:
/* Line 1787 of yacc.c  */
#line 4830 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (3)].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[(1) - (3)].types)->str(), (yyvsp[(3) - (3)].types)->str(), "|"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '|' +
		     (yyvsp[(3) - (3)].types)->theExpression;
		   delete (yyvsp[(3) - (3)].types);
                 }
    break;

  case 788:
/* Line 1787 of yacc.c  */
#line 4838 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (2)].types);  }
    break;

  case 789:
/* Line 1787 of yacc.c  */
#line 4842 "parser.y"
    {
		   string one_bar=("~("+(yyvsp[(1) - (3)].types)->str())+')';
		   string three_bar=("~("+(yyvsp[(3) - (3)].types)->str())+')';
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic(logic((yyvsp[(1) - (3)].types)->str(), three_bar, "&"),
		     logic(one_bar, (yyvsp[(3) - (3)].types)->str(), "&"), "|"));
		   delete (yyvsp[(3) - (3)].types);
		   //$$= new TYPES1(check_logic($1, $3, "'xor'"));
		 }
    break;

  case 790:
/* Line 1787 of yacc.c  */
#line 4851 "parser.y"
    {  (yyval.types) = (yyvsp[(2) - (2)].types);   }
    break;

  case 791:
/* Line 1787 of yacc.c  */
#line 4855 "parser.y"
    { (yyval.types) = (yyvsp[(1) - (1)].types); }
    break;

  case 792:
/* Line 1787 of yacc.c  */
#line 4856 "parser.y"
    {
		 (yyval.types) = Probe((yyvsp[(3) - (4)].types)->str());
	       }
    break;

  case 793:
/* Line 1787 of yacc.c  */
#line 4860 "parser.y"
    {
		   pair<string,TYPES> TT1 = table->lookup((yyvsp[(1) - (3)].types)->str());
		   pair<string,TYPES> TT3 = table->lookup((yyvsp[(3) - (3)].types)->str());
		   /*if(TT1.first == "err" || TT3.first == "err"){
		     if(TT1.first == "err")
		       err_msg("undeclared identifier '",$1->get_string(),"'");
		     if(TT3.first == "err")
		       err_msg("undeclared identifier '",$3->get_string(),"'");
		   }
		   else*/{
		     (yyval.types) = new TYPES;
		     if (table->lookup((yyvsp[(1) - (3)].types)->get_string()).first != "qty" &&
			 table->lookup((yyvsp[(3) - (3)].types)->get_string()).first != "qty")
		       (yyval.types)->set_data("bool");
		     else
		       (yyval.types)->set_data("expr");
		     
		     if(table->lookup((yyvsp[(1) - (3)].types)->get_string()).first == "var" ||
			table->lookup((yyvsp[(3) - (3)].types)->get_string()).first == "var")
		       (yyval.types)->set_str("maybe");
       		     else
		       //$$->set_str($1->str() + $2 + $3->str());
		       (yyval.types)->set_str(bool_relation((yyvsp[(1) - (3)].types)->theExpression, (yyvsp[(3) - (3)].types)->theExpression, (yyvsp[(2) - (3)].C_str)));
		     (yyval.types)->theExpression += (yyvsp[(1) - (3)].types)->theExpression + (yyvsp[(2) - (3)].C_str) + (yyvsp[(3) - (3)].types)->theExpression;
		     //cout << "@relation " << $$->theExpression << endl;
		   }
		 }
    break;

  case 794:
/* Line 1787 of yacc.c  */
#line 4889 "parser.y"
    { (yyval.C_str) = "="; }
    break;

  case 795:
/* Line 1787 of yacc.c  */
#line 4889 "parser.y"
    { (yyval.C_str) = "/="; }
    break;

  case 796:
/* Line 1787 of yacc.c  */
#line 4889 "parser.y"
    { (yyval.C_str) = "<"; }
    break;

  case 797:
/* Line 1787 of yacc.c  */
#line 4890 "parser.y"
    { (yyval.C_str) = "<="; }
    break;

  case 798:
/* Line 1787 of yacc.c  */
#line 4890 "parser.y"
    { (yyval.C_str) = ">"; }
    break;

  case 799:
/* Line 1787 of yacc.c  */
#line 4890 "parser.y"
    { (yyval.C_str) = ">=";}
    break;

  case 800:
/* Line 1787 of yacc.c  */
#line 4893 "parser.y"
    {(yyval.types) = (yyvsp[(1) - (1)].types);}
    break;

  case 801:
/* Line 1787 of yacc.c  */
#line 4895 "parser.y"
    {  /*
		   if( !$3->isInt() ) {
                     TYPES *ty= new TYPES("ERROR", "ERROR", "ERROR");
		     $$= new TYPES1(ty);
		     err_msg("expecting an integer"); }
		   else
		     $$= $1;*/
		 }
    break;

  case 809:
/* Line 1787 of yacc.c  */
#line 4910 "parser.y"
    {
		  (yyval.types) = (yyvsp[(2) - (2)].types);
		  (yyval.types)->theExpression = (yyvsp[(1) - (2)].C_str) + (yyval.types)->theExpression;
		}
    break;

  case 810:
/* Line 1787 of yacc.c  */
#line 4916 "parser.y"
    {(yyval.C_str) = "+";}
    break;

  case 811:
/* Line 1787 of yacc.c  */
#line 4917 "parser.y"
    {(yyval.C_str) = "-";}
    break;

  case 812:
/* Line 1787 of yacc.c  */
#line 4920 "parser.y"
    {(yyval.types) = (yyvsp[(1) - (1)].types);}
    break;

  case 813:
/* Line 1787 of yacc.c  */
#line 4922 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (3)].types);
		   (yyval.types)->theExpression = (yyval.types)->theExpression + (yyvsp[(2) - (3)].C_str)
		     + (yyvsp[(3) - (3)].types)->theExpression;
		 }
    break;

  case 814:
/* Line 1787 of yacc.c  */
#line 4929 "parser.y"
    {(yyval.C_str) = "+";}
    break;

  case 815:
/* Line 1787 of yacc.c  */
#line 4929 "parser.y"
    {(yyval.C_str) = "-";}
    break;

  case 816:
/* Line 1787 of yacc.c  */
#line 4929 "parser.y"
    {(yyval.C_str) = "&";}
    break;

  case 817:
/* Line 1787 of yacc.c  */
#line 4933 "parser.y"
    {
		   (yyval.types) = (yyvsp[(1) - (3)].types);
		   (yyval.types)->theExpression = (yyval.types)->theExpression + (yyvsp[(2) - (3)].C_str) +
		     (yyvsp[(3) - (3)].types)->theExpression;
		 }
    break;

  case 818:
/* Line 1787 of yacc.c  */
#line 4938 "parser.y"
    { (yyval.types) = (yyvsp[(1) - (1)].types);  }
    break;

  case 819:
/* Line 1787 of yacc.c  */
#line 4941 "parser.y"
    {(yyval.C_str) = "*";}
    break;

  case 820:
/* Line 1787 of yacc.c  */
#line 4941 "parser.y"
    {(yyval.C_str) = "/";}
    break;

  case 821:
/* Line 1787 of yacc.c  */
#line 4942 "parser.y"
    {(yyval.C_str) = "%";}
    break;

  case 822:
/* Line 1787 of yacc.c  */
#line 4942 "parser.y"
    {(yyval.C_str) = "rem";}
    break;

  case 825:
/* Line 1787 of yacc.c  */
#line 4947 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (2)].types); }
    break;

  case 826:
/* Line 1787 of yacc.c  */
#line 4949 "parser.y"
    {
		   (yyvsp[(2) - (2)].types)->set_str(("~("+(yyvsp[(2) - (2)].types)->theExpression)+')');
		   (yyval.types) = (yyvsp[(2) - (2)].types);
		   (yyval.types)->theExpression = "~(" + (yyvsp[(2) - (2)].types)->theExpression + ")";
		 }
    break;

  case 827:
/* Line 1787 of yacc.c  */
#line 4957 "parser.y"
    {
		     (yyval.types)=(yyvsp[(1) - (1)].types);
		     if((yyval.types)->get_string()=="true" || (yyval.types)->get_string()=="false"){
		       (yyval.types)->set_data("bool");
		     }
		   }
    break;

  case 830:
/* Line 1787 of yacc.c  */
#line 4967 "parser.y"
    { yyerror("Qualified expressions not supported."); }
    break;

  case 831:
/* Line 1787 of yacc.c  */
#line 4969 "parser.y"
    { yyerror("Allocators not supported."); }
    break;

  case 832:
/* Line 1787 of yacc.c  */
#line 4971 "parser.y"
    { (yyval.types) = (yyvsp[(2) - (3)].types); }
    break;

  case 833:
/* Line 1787 of yacc.c  */
#line 4974 "parser.y"
    {}
    break;

  case 834:
/* Line 1787 of yacc.c  */
#line 4975 "parser.y"
    {}
    break;

  case 835:
/* Line 1787 of yacc.c  */
#line 4976 "parser.y"
    { (yyval.types)= (yyvsp[(2) - (2)].types); }
    break;

  case 837:
/* Line 1787 of yacc.c  */
#line 4980 "parser.y"
    {}
    break;

  case 838:
/* Line 1787 of yacc.c  */
#line 4983 "parser.y"
    { (yyval.types)= (yyvsp[(4) - (5)].types); }
    break;

  case 839:
/* Line 1787 of yacc.c  */
#line 4984 "parser.y"
    { (yyval.types)= (yyvsp[(3) - (3)].types); }
    break;

  case 840:
/* Line 1787 of yacc.c  */
#line 4988 "parser.y"
    {
		   (yyval.types) = new TYPES();
		   (yyval.types)->set_str((yyvsp[(1) - (1)].str_list)->front());
		   (yyval.types)->theExpression += (yyvsp[(1) - (1)].str_list)->front();
		 
                 }
    break;

  case 841:
/* Line 1787 of yacc.c  */
#line 5008 "parser.y"
    {
		   (yyval.types) = new TYPES();
		   (yyval.types)->set_str((yyvsp[(1) - (6)].str_list)->front());
		   (yyval.types)->theExpression += (yyvsp[(1) - (6)].str_list)->front() + ">="
		     + (yyvsp[(5) - (6)].types)->theExpression;
		 }
    break;

  case 842:
/* Line 1787 of yacc.c  */
#line 5015 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 843:
/* Line 1787 of yacc.c  */
#line 5019 "parser.y"
    { (yyval.str_list) = new list<string>;  (yyval.str_list)->push_back((yyvsp[(1) - (1)].actionptr)->label); }
    break;

  case 845:
/* Line 1787 of yacc.c  */
#line 5025 "parser.y"
    { (yyval.str_list) = (yyvsp[(1) - (3)].str_list);  (yyval.str_list)->push_back((yyvsp[(3) - (3)].C_str)); }
    break;

  case 846:
/* Line 1787 of yacc.c  */
#line 5028 "parser.y"
    {  (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 847:
/* Line 1787 of yacc.c  */
#line 5029 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 848:
/* Line 1787 of yacc.c  */
#line 5033 "parser.y"
    {
		   (yyval.types)= new TYPES;
		   //$$->set_str(strtok($1, "\""));
      		   //$$->theExpression += $1;
		   
		   char *chptr;
		   chptr = strpbrk((yyvsp[(1) - (1)].strlit), "\"");
		   chptr = strtok(chptr+1, "\"");
		   (yyval.types)->set_str(chptr);
		   (yyval.types)->theExpression += chptr;
		   //cout << "@STRLIT " << $$->theExpression << endl;
      		 }
    break;

  case 849:
/* Line 1787 of yacc.c  */
#line 5045 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 850:
/* Line 1787 of yacc.c  */
#line 5048 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 851:
/* Line 1787 of yacc.c  */
#line 5049 "parser.y"
    { (yyval.C_str) = (yyvsp[(1) - (1)].charlit);  }
    break;

  case 852:
/* Line 1787 of yacc.c  */
#line 5050 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].strlit); }
    break;

  case 853:
/* Line 1787 of yacc.c  */
#line 5051 "parser.y"
    { (yyval.C_str) = "all"; }
    break;

  case 854:
/* Line 1787 of yacc.c  */
#line 5060 "parser.y"
    {
		  (yyval.types) = new TYPES();
		  (yyval.types)->set_str((yyvsp[(1) - (3)].str_list)->front());
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search(table,$1->label);
                  //$$= t; $$->setstring($1->label);
		}
    break;

  case 855:
/* Line 1787 of yacc.c  */
#line 5068 "parser.y"
    {
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search( table, $1->name );
		  //$$= t;
		}
    break;

  case 856:
/* Line 1787 of yacc.c  */
#line 5074 "parser.y"
    {
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search( table, $1->name2->label );
		  //$$= t;
		}
    break;

  case 857:
/* Line 1787 of yacc.c  */
#line 5081 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (1)].actionptr)->label; }
    break;

  case 858:
/* Line 1787 of yacc.c  */
#line 5083 "parser.y"
    { (yyval.C_str)= (yyvsp[(1) - (4)].actionptr)->label; }
    break;

  case 859:
/* Line 1787 of yacc.c  */
#line 5084 "parser.y"
    { (yyval.C_str)= "range"; }
    break;

  case 860:
/* Line 1787 of yacc.c  */
#line 5085 "parser.y"
    {}
    break;

  case 861:
/* Line 1787 of yacc.c  */
#line 5089 "parser.y"
    {}
    break;

  case 862:
/* Line 1787 of yacc.c  */
#line 5090 "parser.y"
    {}
    break;

  case 863:
/* Line 1787 of yacc.c  */
#line 5094 "parser.y"
    {}
    break;

  case 865:
/* Line 1787 of yacc.c  */
#line 5100 "parser.y"
    {
		  /*($$ = table->lookup($1->front());
		  if($$ == 0)
		    {
		      err_msg("'", $1->front(), "' undeclared");	
		      $$ = new TYPES("err", "err");
		      }*/
		  (yyval.types) = new TYPES;
		  (yyval.types)->set_list((yyvsp[(2) - (2)].typelist));
		  (yyval.types)->set_str((yyvsp[(1) - (2)].str_list)->front());
		  if ((yyvsp[(1) - (2)].str_list)->front()=="delay") {
		    (yyval.types)->theExpression =
		      "{" + (yyvsp[(2) - (2)].typelist)->front().second.theExpression + "}";
		  } else if ((yyvsp[(1) - (2)].str_list)->front()=="selection") {
		    // ZHEN(Done): extract the first integer of $2 and subtract 1 from it
		    // "uniform(0," + $2->front().second.theExpression + ")";
		    //$$->theExpression ="uniform(0," + intToString(atoi((($2->front().second.theExpression).substr(0,($2->front().second.theExpression).find(";")).c_str()))-1) + ")";
		    // "floor(uniform(0," + $2->front().second.theExpression + "))";
		    (yyval.types)->theExpression ="floor(uniform(0," + intToString(atoi((((yyvsp[(2) - (2)].typelist)->front().second.theExpression).substr(0,((yyvsp[(2) - (2)].typelist)->front().second.theExpression).find(";")).c_str()))) + "))";
		    (yyval.types)->set_str((yyval.types)->theExpression);
		  } else if (((yyvsp[(1) - (2)].str_list)->front()=="init_channel") ||
		             ((yyvsp[(1) - (2)].str_list)->front()=="active") ||
                             ((yyvsp[(1) - (2)].str_list)->front()=="passive") ||
                             ((yyvsp[(1) - (2)].str_list)->front()=="timing")) {
		  } else {
		    (yyval.types)->theExpression =
		      "BIT(" + (yyvsp[(1) - (2)].str_list)->front() + "," + (yyvsp[(2) - (2)].typelist)->front().second.theExpression + ")";
		    (yyval.types)->set_str((yyval.types)->theExpression);
		    //cout << "$$->theExpression " << $$->theExpression << endl;
		  }
		  /*
		  char s[200] = {'\0'};
		  $2->iteration_init();
		  if($2->value().second->grp_id() == TYPES::Integer)
		    sprintf(s, "%s__%ld__", $1->front().c_str(),
			    $2->value().second->get_int());
			    $$->set_str(s);*/
		}
    break;

  case 866:
/* Line 1787 of yacc.c  */
#line 5139 "parser.y"
    {
		  (yyval.types) = new TYPES((yyvsp[(1) - (2)].types));
		  //$$->formal_list= NULL;
                  //$$= $1;
                  (yyval.types)->set_list((yyvsp[(2) - (2)].typelist));
		}
    break;

  case 867:
/* Line 1787 of yacc.c  */
#line 5148 "parser.y"
    {
		  (yyval.types) = new TYPES("uk", "integer");
		  (yyval.types)->set_grp(TYPES::Integer);
		  (yyval.types)->set_int((yyvsp[(1) - (1)].intval));
		  (yyval.types)->theExpression += intToString((yyvsp[(1) - (1)].intval));
	          (yyval.types)->set_str((yyval.types)->theExpression);
                }
    break;

  case 868:
/* Line 1787 of yacc.c  */
#line 5156 "parser.y"
    {
                  (yyval.types)= new TYPES("uk", "real");
		  (yyval.types)->set_grp(TYPES::Real);
                  (yyval.types)->set_flt((yyvsp[(1) - (1)].floatval));
		  (yyval.types)->theExpression += numToString((yyvsp[(1) - (1)].floatval));
                }
    break;

  case 869:
/* Line 1787 of yacc.c  */
#line 5163 "parser.y"
    {
		  (yyval.types) = new TYPES("uk", "integer");
		  (yyval.types)->set_grp(TYPES::Integer);
		  (yyval.types)->set_int((yyvsp[(1) - (1)].basedlit));
		  (yyval.types)->theExpression += intToString((yyvsp[(1) - (1)].basedlit));
                }
    break;

  case 870:
/* Line 1787 of yacc.c  */
#line 5170 "parser.y"
    {		
		  (yyval.types) = new TYPES("chr", "uk");
		  (yyval.types)->set_string((yyvsp[(1) - (1)].charlit));
		  (yyval.types)->theExpression += (yyvsp[(1) - (1)].charlit);
		  // cout << "@CHARLIT " << $$->theExpression << endl;
		}
    break;

  case 871:
/* Line 1787 of yacc.c  */
#line 5177 "parser.y"
    {
                  (yyval.types)= new TYPES("uk", "BIT_VECTOR");
                  (yyval.types)->set_string((yyvsp[(1) - (1)].strlit));
		  (yyval.types)->theExpression += (yyvsp[(1) - (1)].strlit);
                }
    break;

  case 872:
/* Line 1787 of yacc.c  */
#line 5182 "parser.y"
    {}
    break;

  case 873:
/* Line 1787 of yacc.c  */
#line 5183 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 874:
/* Line 1787 of yacc.c  */
#line 5186 "parser.y"
    { (yyval.typelist) = new TYPELIST(make_pair((yyvsp[(1) - (1)].actionptr)->label, TYPES())); }
    break;

  case 875:
/* Line 1787 of yacc.c  */
#line 5188 "parser.y"
    {
		  TYPES t("cst", "int");
		  t.set_int((yyvsp[(1) - (2)].intval));
		  (yyval.typelist) = new TYPELIST(make_pair((yyvsp[(2) - (2)].actionptr)->label, t));
		}
    break;

  case 876:
/* Line 1787 of yacc.c  */
#line 5194 "parser.y"
    {
		  TYPES t("cst", "flt");
		  t.set_flt((yyvsp[(1) - (2)].floatval));
		  (yyval.typelist) = new TYPELIST(make_pair((yyvsp[(2) - (2)].actionptr)->label, t));
		}
    break;

  case 877:
/* Line 1787 of yacc.c  */
#line 5200 "parser.y"
    {
		  TYPES t("cst", "int");
		  t.set_int((yyvsp[(1) - (2)].basedlit));
		  (yyval.typelist) = new TYPELIST(make_pair((yyvsp[(2) - (2)].actionptr)->label, t));
		  //TYPES *temp= search(table,$2->label);
		  //$$= new TYPES("CONST", "uk", "uk");
		}
    break;

  case 878:
/* Line 1787 of yacc.c  */
#line 5210 "parser.y"
    {
		  //$$ = table->lookup($2->label);
		  (yyval.types) = new TYPES("cst", "int");
		  (yyval.types)->set_int((yyvsp[(1) - (2)].intval));
		}
    break;

  case 879:
/* Line 1787 of yacc.c  */
#line 5216 "parser.y"
    {
		  (yyval.types) = new TYPES("cst", "flt");
		  (yyval.types)->set_flt((yyvsp[(1) - (2)].floatval));
		  /*
		  TYPES *temp = table->lookup($2->label);
		  if(temp == NULL)
		    {
		      err_msg("'", $2->label, "' was not declared before;");
		      temp = new TYPES("err", "err", "err");
		    }
		  $$= new TYPES("CONST", temp->datatype(), temp->subtype());*/
		 }
    break;

  case 880:
/* Line 1787 of yacc.c  */
#line 5229 "parser.y"
    {
		  (yyval.types) = new TYPES("cst", "int");
		  (yyval.types)->set_int((yyvsp[(1) - (2)].basedlit));
		  /*
		  TYPES *temp = table->lookup($2->label);
		  if(temp == NULL)
		    {
		      err_msg("'", $2->label, "' was not declared before;");
		      temp = new TYPES("err", "err", "err");
		    }
		  $$= new TYPES("CONST", temp->datatype(), temp->subtype() );*/
		}
    break;

  case 881:
/* Line 1787 of yacc.c  */
#line 5244 "parser.y"
    {
		  /*if(strcmp_nocase($2->datatype(), $4->datatype())
		     && !($2->isError() || $4->isError()))
		    {
		      $$= new TYPES("err","err", "err");
		      err_msg("wrong types between '(' and ')'");
		    }
		  else
		  */
		    (yyval.types)= (yyvsp[(4) - (5)].types);
		}
    break;

  case 882:
/* Line 1787 of yacc.c  */
#line 5256 "parser.y"
    {
		  //if($2->isUnknown())
		    (yyval.types)= (yyvsp[(4) - (5)].types);
		    /*
		  else if(($2->datatype() == $4->datatype()) &&
			  !($2->isError() || $4->isError()))
		    $$= $2;
		  else
		    err_msg("different types on left and right sides",
			    "of '=>'", ";");
		  $$= new TYPES("err", "err", "err");
		    */
		}
    break;

  case 886:
/* Line 1787 of yacc.c  */
#line 5289 "parser.y"
    {
		  //if($1->isUnknown())
		    (yyval.types)= (yyvsp[(3) - (3)].types);
		    /*
		  else if(($1->datatype()==$3->datatype()) &&
			  !($1->isError() || $3->isError()))
		    $$= $1;
		  else
		    err_msg("different types on left and right sides",
			    " of '=>' ", ";");
		  $$= new TYPES("err", "err", "err");
	      */
		}
    break;

  case 887:
/* Line 1787 of yacc.c  */
#line 5305 "parser.y"
    {
		  (yyval.ty_lst) = (yyvsp[(1) - (3)].ty_lst);
		  if((yyval.ty_lst) && (yyvsp[(3) - (3)].types)->grp_id() != TYPES::Error){
		    if((yyval.ty_lst)->back().grp_id() != (yyvsp[(3) - (3)].types)->grp_id()){
		      err_msg("type mismatch");
		      delete (yyvsp[(1) - (3)].ty_lst);
		      (yyval.ty_lst) = 0;
		    }
		    else
		      (yyval.ty_lst)->push_back(*(yyvsp[(3) - (3)].types));
		  }
		}
    break;

  case 888:
/* Line 1787 of yacc.c  */
#line 5318 "parser.y"
    {
		  (yyval.ty_lst) = 0;
		  if((yyvsp[(1) - (1)].types)->grp_id() != TYPES::Error){
		    (yyval.ty_lst) = new list<TYPES>;
		    (yyval.ty_lst)->push_back(*(yyvsp[(1) - (1)].types));
		  }
		  delete (yyvsp[(1) - (1)].types);
		}
    break;

  case 889:
/* Line 1787 of yacc.c  */
#line 5328 "parser.y"
    { (yyval.types)= (yyvsp[(1) - (1)].types); }
    break;

  case 890:
/* Line 1787 of yacc.c  */
#line 5329 "parser.y"
    { (yyval.types) = new TYPES(); }
    break;

  case 891:
/* Line 1787 of yacc.c  */
#line 5330 "parser.y"
    { (yyval.types)= new TYPES(); (yyval.types)->set_str("OTHERS"); }
    break;

  case 893:
/* Line 1787 of yacc.c  */
#line 5348 "parser.y"
    {}
    break;

  case 895:
/* Line 1787 of yacc.c  */
#line 5353 "parser.y"
    {
		    new_action_table((yyvsp[(2) - (3)].actionptr)->label);
		    new_event_table((yyvsp[(2) - (3)].actionptr)->label);
		    new_rule_table((yyvsp[(2) - (3)].actionptr)->label);
		    new_conflict_table((yyvsp[(2) - (3)].actionptr)->label);
		    tel_tb = new tels_table;
		  }
    break;

  case 896:
/* Line 1787 of yacc.c  */
#line 5361 "parser.y"
    {		
		    map<string,int> sig_list;
		    if((yyvsp[(5) - (7)].str_bl_lst))
		      for(list<pair<string,int> >::iterator b = (yyvsp[(5) - (7)].str_bl_lst)->begin();
			  b != (yyvsp[(5) - (7)].str_bl_lst)->end(); b++)
			sig_list.insert(*b);
		    if((yyvsp[(6) - (7)].str_bl_lst))
		      for(list<pair<string,int> >::iterator b = (yyvsp[(6) - (7)].str_bl_lst)->begin();
			  b != (yyvsp[(6) - (7)].str_bl_lst)->end(); b++)
			if(sig_list.find(b->first) == sig_list.end())
			  sig_list.insert(make_pair(b->first, OUT));
		    tel_tb->insert(sig_list);
		    tel_tb->set_id((yyvsp[(2) - (7)].actionptr)->label);
		     tel_tb->set_type((yyvsp[(2) - (7)].actionptr)->label);
		    (*open_module_list)[(yyvsp[(2) - (7)].actionptr)->label] = make_pair(tel_tb, false);
		  }
    break;

  case 897:
/* Line 1787 of yacc.c  */
#line 5378 "parser.y"
    {
		    cur_file = *(yyvsp[(2) - (3)].str);
		    switch_buffer(*(yyvsp[(2) - (3)].str));
		    delete (yyvsp[(2) - (3)].str);
		  }
    break;

  case 898:
/* Line 1787 of yacc.c  */
#line 5386 "parser.y"
    {
		    string f_name;
		    if(*(yyvsp[(1) - (3)].str) != "error"){
		      if (toTEL)
			f_name = *(yyvsp[(1) - (3)].str) + '/' + (yyvsp[(3) - (3)].id) + ".hse";
		      else
			f_name = *(yyvsp[(1) - (3)].str) + '/' + (yyvsp[(3) - (3)].id) + ".csp";
		      (yyval.str) = new string(f_name);
		      delete (yyvsp[(1) - (3)].str);
		    }
		    else
		      (yyval.str) = (yyvsp[(1) - (3)].str);
		  }
    break;

  case 899:
/* Line 1787 of yacc.c  */
#line 5400 "parser.y"
    {
		    string f_name;
		    if (toTEL)
		      f_name = cur_dir + '/' + string((yyvsp[(1) - (1)].id)) + ".hse";
		    else
		      f_name = cur_dir + '/' + string((yyvsp[(1) - (1)].id)) + ".csp";
		    (yyval.str) = new string(f_name);
		  }
    break;

  case 900:
/* Line 1787 of yacc.c  */
#line 5411 "parser.y"
    {
		    string ss = *(yyvsp[(1) - (3)].str) + '/' + (yyvsp[(3) - (3)].actionptr)->label;
		    (yyval.str) = new string(ss);
		  }
    break;

  case 901:
/* Line 1787 of yacc.c  */
#line 5416 "parser.y"
    { (yyval.str) = new string((yyvsp[(1) - (1)].actionptr)->label); }
    break;

  case 902:
/* Line 1787 of yacc.c  */
#line 5418 "parser.y"
    {
		    char *s = getenv("HOME");
		    if(s)
		      (yyval.str) = new string(s);
		    else
		      {
			err_msg("home directory not found");
			(yyval.str) = new string("error");
		      }
		  }
    break;

  case 903:
/* Line 1787 of yacc.c  */
#line 5428 "parser.y"
    { (yyval.str) = &cur_dir; }
    break;

  case 904:
/* Line 1787 of yacc.c  */
#line 5433 "parser.y"
    { (yyval.id) = copy_string(((*name_mapping)[(yyvsp[(1) - (1)].actionptr)->label]).c_str()); }
    break;

  case 905:
/* Line 1787 of yacc.c  */
#line 5435 "parser.y"
    { (yyval.id) = "*"; }
    break;

  case 906:
/* Line 1787 of yacc.c  */
#line 5439 "parser.y"
    {
		    if((yyvsp[(1) - (2)].str_bl_lst) && (yyvsp[(2) - (2)].str_bl_lst))
		      {
			(yyvsp[(1) - (2)].str_bl_lst)->splice((yyvsp[(1) - (2)].str_bl_lst)->begin(), *(yyvsp[(2) - (2)].str_bl_lst));
			(yyval.str_bl_lst) = (yyvsp[(1) - (2)].str_bl_lst);
		      }
		    else if((yyvsp[(1) - (2)].str_bl_lst))
		      (yyval.str_bl_lst) = (yyvsp[(1) - (2)].str_bl_lst);
		    else
		      (yyval.str_bl_lst) = (yyvsp[(2) - (2)].str_bl_lst);
		  }
    break;

  case 907:
/* Line 1787 of yacc.c  */
#line 5450 "parser.y"
    { (yyval.str_bl_lst) = 0; }
    break;

  case 911:
/* Line 1787 of yacc.c  */
#line 5462 "parser.y"
    {
		    makedelaydecl((yyvsp[(2) - (5)].actionptr),(yyvsp[(4) - (5)].delayval));
		    (yyval.str_bl_lst) = 0;
		  }
    break;

  case 912:
/* Line 1787 of yacc.c  */
#line 5470 "parser.y"
    {
		    makedefinedecl((yyvsp[(2) - (5)].actionptr),(yyvsp[(4) - (5)].floatval));
		    (yyval.str_bl_lst) = 0;
		  }
    break;

  case 913:
/* Line 1787 of yacc.c  */
#line 5475 "parser.y"
    {
		    makedefinedecl((yyvsp[(2) - (6)].actionptr),(-1.0)*(yyvsp[(5) - (6)].floatval));
		    (yyval.str_bl_lst) = 0;
		  }
    break;

  case 914:
/* Line 1787 of yacc.c  */
#line 5483 "parser.y"
    {
		    initial_stateADT init_st(true, false, false);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[(2) - (3)].action_list)->begin();
			b != (yyvsp[(2) - (3)].action_list)->end(); b++){
		      (*b)->initial_state = init_st;
		      makebooldecl((yyvsp[(1) - (3)].intval), *b, FALSE, NULL);
		      (yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[(1) - (3)].intval)));
		    }
		  }
    break;

  case 915:
/* Line 1787 of yacc.c  */
#line 5494 "parser.y"
    {
		    initial_stateADT init_st(true, true, false);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[(2) - (7)].action_list)->begin();
			b != (yyvsp[(2) - (7)].action_list)->end(); b++) {
		      (*b)->initial_state = init_st;
		      makebooldecl((yyvsp[(1) - (7)].intval), *b, (yyvsp[(5) - (7)].boolval), NULL);
		      (yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[(1) - (7)].intval)));
		    }
		  }
    break;

  case 916:
/* Line 1787 of yacc.c  */
#line 5505 "parser.y"
    {
		    initial_stateADT init_st(true, false, true);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[(2) - (7)].action_list)->begin();
			b != (yyvsp[(2) - (7)].action_list)->end(); b++)
		      {
			(*b)->initial_state = init_st;
			makebooldecl((yyvsp[(1) - (7)].intval), *b, FALSE, &((yyvsp[(5) - (7)].delayval)));
			(yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[(1) - (7)].intval)));
		      }
		  }
    break;

  case 917:
/* Line 1787 of yacc.c  */
#line 5517 "parser.y"
    {
		    initial_stateADT init_st(true, true, true);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[(2) - (9)].action_list)->begin();
			b != (yyvsp[(2) - (9)].action_list)->end(); b++)
		      {
			(*b)->initial_state = init_st;
			makebooldecl((yyvsp[(1) - (9)].intval), *b, (yyvsp[(5) - (9)].boolval), &((yyvsp[(7) - (9)].delayval)));
			(yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[(1) - (9)].intval)));
		      }
		  }
    break;

  case 918:
/* Line 1787 of yacc.c  */
#line 5532 "parser.y"
    {
		   (yyval.action_list) = new list<actionADT>;
                   (yyval.action_list)->push_back((yyvsp[(1) - (1)].actionptr));
		 }
    break;

  case 919:
/* Line 1787 of yacc.c  */
#line 5537 "parser.y"
    {
		   (yyval.action_list) = (yyvsp[(1) - (3)].action_list);
                   (yyval.action_list)->push_back((yyvsp[(3) - (3)].actionptr));
		 }
    break;

  case 920:
/* Line 1787 of yacc.c  */
#line 5574 "parser.y"
    { assigndelays(&((yyval.delayval)),(yyvsp[(2) - (9)].intval),(yyvsp[(4) - (9)].intval),NULL,(yyvsp[(6) - (9)].intval),(yyvsp[(8) - (9)].intval),NULL); }
    break;

  case 921:
/* Line 1787 of yacc.c  */
#line 5577 "parser.y"
    { sprintf(distrib1,"%s(%g,%g)",(yyvsp[(6) - (23)].actionptr)->label,(yyvsp[(8) - (23)].floatval),(yyvsp[(10) - (23)].floatval));
			  sprintf(distrib2,"%s(%g,%g)",(yyvsp[(17) - (23)].actionptr)->label,(yyvsp[(19) - (23)].floatval),(yyvsp[(21) - (23)].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[(2) - (23)].intval),(yyvsp[(4) - (23)].intval),distrib1,(yyvsp[(13) - (23)].intval),(yyvsp[(15) - (23)].intval),distrib2);
                        }
    break;

  case 922:
/* Line 1787 of yacc.c  */
#line 5583 "parser.y"
    { sprintf(distrib1,"%s(%g,%g)",(yyvsp[(6) - (16)].actionptr)->label,(yyvsp[(8) - (16)].floatval),(yyvsp[(10) - (16)].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[(2) - (16)].intval),(yyvsp[(4) - (16)].intval),distrib1,(yyvsp[(13) - (16)].intval),(yyvsp[(15) - (16)].intval),NULL);
                        }
    break;

  case 923:
/* Line 1787 of yacc.c  */
#line 5588 "parser.y"
    { sprintf(distrib2,"%s(%g,%g)",(yyvsp[(10) - (16)].actionptr)->label,(yyvsp[(12) - (16)].floatval),(yyvsp[(14) - (16)].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[(2) - (16)].intval),(yyvsp[(4) - (16)].intval),NULL,(yyvsp[(6) - (16)].intval),(yyvsp[(8) - (16)].intval),distrib2);
                        }
    break;

  case 924:
/* Line 1787 of yacc.c  */
#line 5592 "parser.y"
    { assigndelays(&((yyval.delayval)),(yyvsp[(2) - (5)].intval),(yyvsp[(4) - (5)].intval),NULL,(yyvsp[(2) - (5)].intval),(yyvsp[(4) - (5)].intval),NULL); }
    break;

  case 925:
/* Line 1787 of yacc.c  */
#line 5594 "parser.y"
    { sprintf(distrib1,"%s(%g,%g)",(yyvsp[(6) - (12)].actionptr)->label,(yyvsp[(8) - (12)].floatval),(yyvsp[(10) - (12)].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[(2) - (12)].intval),(yyvsp[(4) - (12)].intval),distrib1,(yyvsp[(2) - (12)].intval),(yyvsp[(4) - (12)].intval),distrib1); }
    break;

  case 926:
/* Line 1787 of yacc.c  */
#line 5597 "parser.y"
    { checkdelay((yyvsp[(1) - (1)].actionptr)->label,(yyvsp[(1) - (1)].actionptr)->type);
			  assigndelays(&((yyval.delayval)),(yyvsp[(1) - (1)].actionptr)->delay.lr,(yyvsp[(1) - (1)].actionptr)->delay.ur,
				       (yyvsp[(1) - (1)].actionptr)->delay.distr,(yyvsp[(1) - (1)].actionptr)->delay.lf,
                                       (yyvsp[(1) - (1)].actionptr)->delay.uf,(yyvsp[(1) - (1)].actionptr)->delay.distf); }
    break;

  case 928:
/* Line 1787 of yacc.c  */
#line 5605 "parser.y"
    {
		if((yyvsp[(1) - (2)].str_bl_lst) && (yyvsp[(2) - (2)].str_bl_lst)){
		  (yyval.str_bl_lst) = (yyvsp[(1) - (2)].str_bl_lst);
		  (yyval.str_bl_lst)->splice((yyval.str_bl_lst)->begin(), *(yyvsp[(2) - (2)].str_bl_lst));
		}
		else if((yyvsp[(1) - (2)].str_bl_lst))
		  (yyval.str_bl_lst) = (yyvsp[(1) - (2)].str_bl_lst);
		else
		  (yyval.str_bl_lst) = (yyvsp[(2) - (2)].str_bl_lst);
	      }
    break;

  case 929:
/* Line 1787 of yacc.c  */
#line 5615 "parser.y"
    { (yyval.str_bl_lst) = 0; }
    break;

  case 931:
/* Line 1787 of yacc.c  */
#line 5621 "parser.y"
    {     		
		pair<tels_table*,bool> rp = (*open_module_list)[(yyvsp[(1) - (6)].actionptr)->label];
		if(rp.first != 0){
		  (yyval.str_bl_lst) = new list<pair<string,int> >;
		  tels_table* rpc = new tels_table(rp.first);
		  rpc->set_id((yyvsp[(2) - (6)].actionptr)->label);
		  rpc->set_type((yyvsp[(1) - (6)].actionptr)->label);
		  rpc->insert(*(yyvsp[(4) - (6)].csp_map));

		  my_list sig_mapping(rpc->portmap());
		  rpc->instantiate(&sig_mapping, (yyvsp[(2) - (6)].actionptr)->label, (yyvsp[(1) - (6)].actionptr)->label, true);
		  if(tel_tb->insert((yyvsp[(2) - (6)].actionptr)->label, rpc) == false)
		    err_msg("duplicate component label '", (yyvsp[(2) - (6)].actionptr)->label, "'");

		  const map<string,int> tmp = rpc->signals();
		  for(map<string,string>::iterator b = (yyvsp[(4) - (6)].csp_map)->begin();
		      b != (yyvsp[(4) - (6)].csp_map)->end(); b++){
		    if(tmp.find(b->first) != tmp.end())
		      (yyval.str_bl_lst)->push_back(make_pair(b->second,
					      tmp.find(b->first)->second));
		    else
		      err_msg("signal '", b->first, "' not found");
		  }
		}
		else {
		  err_msg("module '", (yyvsp[(1) - (6)].actionptr)->label, "' not found");
		  delete (yyvsp[(4) - (6)].csp_map);
		  return 1;
		}
		delete (yyvsp[(4) - (6)].csp_map);
	      }
    break;

  case 932:
/* Line 1787 of yacc.c  */
#line 5656 "parser.y"
    { (yyval.csp_map) = (yyvsp[(1) - (3)].csp_map);  (yyval.csp_map)->insert(*(yyvsp[(3) - (3)].csp_pair));  delete (yyvsp[(3) - (3)].csp_pair); }
    break;

  case 933:
/* Line 1787 of yacc.c  */
#line 5658 "parser.y"
    { (yyval.csp_map) = new map<string, string>;  (yyval.csp_map)->insert(*(yyvsp[(1) - (1)].csp_pair));  delete (yyvsp[(1) - (1)].csp_pair); }
    break;

  case 934:
/* Line 1787 of yacc.c  */
#line 5662 "parser.y"
    { (yyval.csp_pair) = new pair<string, string>((yyvsp[(1) - (3)].actionptr)->label, (yyvsp[(3) - (3)].actionptr)->label); }
    break;

  case 935:
/* Line 1787 of yacc.c  */
#line 5666 "parser.y"
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (5)].TERSptr),";");
		    if(tel_tb->insert((yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr))==false){
		      err_msg("duplicate process label '", (yyvsp[(2) - (5)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
		    if (1) {
		      printf("Compiled process %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		      fprintf(lg,"Compiled process %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		    } else
		      emitters(0, (yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
    break;

  case 936:
/* Line 1787 of yacc.c  */
#line 5681 "parser.y"
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (6)].TERSptr),":");
		    if(tel_tb->insert((yyvsp[(2) - (6)].actionptr)->label, (yyval.TERSptr))==false){
		      err_msg("duplicate process label '", (yyvsp[(2) - (6)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
		    if (1) {
		      printf("Compiled process %s\n",(yyvsp[(2) - (6)].actionptr)->label);
		      fprintf(lg,"Compiled process %s\n",(yyvsp[(2) - (6)].actionptr)->label);
		    } else
		      emitters(0, (yyvsp[(2) - (6)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
    break;

  case 937:
/* Line 1787 of yacc.c  */
#line 5696 "parser.y"
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (5)].TERSptr),";");
		    if(tel_tb->insert((yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr), true)==false) {
		      err_msg("duplicate testbench label '", (yyvsp[(2) - (5)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    if (1) {
		      printf("Compiled testbench %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		      fprintf(lg,"Compiled testbench %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
    break;

  case 938:
/* Line 1787 of yacc.c  */
#line 5711 "parser.y"
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (6)].TERSptr),":");
		    if(tel_tb->insert((yyvsp[(2) - (6)].actionptr)->label, (yyval.TERSptr), true)==false) {
		      err_msg("duplicate testbench label '", (yyvsp[(2) - (6)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    if (1) {
		      printf("Compiled testbench %s\n",(yyvsp[(2) - (6)].actionptr)->label);
		      fprintf(lg,"Compiled testbench %s\n",(yyvsp[(2) - (6)].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[(2) - (6)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
    break;

  case 939:
/* Line 1787 of yacc.c  */
#line 5726 "parser.y"
    {
                    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (5)].TERSptr),";");
		    if(tel_tb->insert((yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr))==false) {
		      err_msg("duplicate gate label '", (yyvsp[(2) - (5)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    if (1) {
		      printf("Compiled gate %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		      fprintf(lg,"Compiled gate %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
                  }
    break;

  case 940:
/* Line 1787 of yacc.c  */
#line 5741 "parser.y"
    {
                    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (5)].TERSptr),";");
		    if(tel_tb->insert((yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr))==false) {
		      err_msg("duplicate constant label '", (yyvsp[(2) - (5)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    TERSmarkord((yyval.TERSptr));
                    if (1) {
		      printf("Compiled constraint %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		      fprintf(lg,"Compiled constraint %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
                  }
    break;

  case 941:
/* Line 1787 of yacc.c  */
#line 5757 "parser.y"
    {
                    (yyval.TERSptr) = TERSrepeat((yyvsp[(4) - (5)].TERSptr),";");
		    if(tel_tb->insert((yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr))==false) {
		      err_msg("duplicate assumption label '", (yyvsp[(2) - (5)].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    TERSmarkassump((yyval.TERSptr));
                    if (1) {
		      printf("Compiled assumption %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		      fprintf(lg,"Compiled assumption %s\n",(yyvsp[(2) - (5)].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[(2) - (5)].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
                  }
    break;

  case 944:
/* Line 1787 of yacc.c  */
#line 5778 "parser.y"
    {
		    (yyvsp[(3) - (6)].actionptr)->vacuous=FALSE;
		    (yyvsp[(6) - (6)].actionptr)->vacuous=FALSE;
		    string ss1 = '[' + *(yyvsp[(1) - (6)].str) + ']' + 'd';
                    (yyval.TERSptr) = TERS((yyvsp[(3) - (6)].actionptr), FALSE,(yyvsp[(3) - (6)].actionptr)->lower,(yyvsp[(3) - (6)].actionptr)->upper,FALSE,
			      ss1.c_str(), (yyvsp[(3) - (6)].actionptr)->dist);
		    string ss2 = '[' + *(yyvsp[(4) - (6)].str) + ']' + 'd';
		    if ((!((yyvsp[(3) - (6)].actionptr)->initial) && !(strchr((yyvsp[(3) - (6)].actionptr)->label,'+'))) ||
			((yyvsp[(3) - (6)].actionptr)->initial && strchr((yyvsp[(3) - (6)].actionptr)->label,'+'))) {
		      (yyval.TERSptr) = TERScompose(TERS((yyvsp[(6) - (6)].actionptr), FALSE,(yyvsp[(6) - (6)].actionptr)->lower,(yyvsp[(6) - (6)].actionptr)->upper,
					    FALSE,ss2.c_str(),(yyvsp[(6) - (6)].actionptr)->dist),(yyval.TERSptr),";");
		    } else {
		      (yyval.TERSptr) = TERScompose((yyval.TERSptr),TERS((yyvsp[(6) - (6)].actionptr), FALSE,(yyvsp[(6) - (6)].actionptr)->lower,(yyvsp[(6) - (6)].actionptr)->upper,
					       FALSE,ss2.c_str(),(yyvsp[(6) - (6)].actionptr)->dist),";");
		    }
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
                    delete (yyvsp[(1) - (6)].str);
                    delete (yyvsp[(4) - (6)].str);
		  }
    break;

  case 945:
/* Line 1787 of yacc.c  */
#line 5798 "parser.y"
    {
		    (yyvsp[(3) - (6)].actionptr)->vacuous=FALSE;
		    (yyvsp[(6) - (6)].actionptr)->vacuous=FALSE;
                    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (6)].TERSptr),TERS((yyvsp[(3) - (6)].actionptr), FALSE,(yyvsp[(3) - (6)].actionptr)->lower,(yyvsp[(3) - (6)].actionptr)->upper,
                                             FALSE,"[true]",(yyvsp[(3) - (6)].actionptr)->dist),";");
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),TERSrename((yyval.TERSptr),(yyvsp[(4) - (6)].TERSptr)),";");
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),TERS((yyvsp[(6) - (6)].actionptr), FALSE,(yyvsp[(6) - (6)].actionptr)->lower,(yyvsp[(6) - (6)].actionptr)->upper,
                                             FALSE,"[true]",(yyvsp[(6) - (6)].actionptr)->dist),";");
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
    break;

  case 946:
/* Line 1787 of yacc.c  */
#line 5811 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)),"|"); }
    break;

  case 948:
/* Line 1787 of yacc.c  */
#line 5815 "parser.y"
    {
		    (yyval.TERSptr) = Guard(*(yyvsp[(1) - (4)].str));
                    delete (yyvsp[(1) - (4)].str);
		    TERSmodifydist((yyval.TERSptr),(yyvsp[(3) - (4)].floatval));
		  }
    break;

  case 949:
/* Line 1787 of yacc.c  */
#line 5823 "parser.y"
    {
		    (yyvsp[(3) - (3)].actionptr)->vacuous=FALSE;
		    string ss1 = '[' + *(yyvsp[(1) - (3)].str) + ']';// + 'd';
                    (yyval.TERSptr) = TERS((yyvsp[(3) - (3)].actionptr),FALSE,0,INFIN,FALSE,ss1.c_str(),(yyvsp[(3) - (3)].actionptr)->dist);
                    //$$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                    //                         FALSE, Cstr, $6->dist),";");
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
                    delete (yyvsp[(1) - (3)].str);
		  }
    break;

  case 950:
/* Line 1787 of yacc.c  */
#line 5833 "parser.y"
    {
		    (yyvsp[(8) - (8)].actionptr)->vacuous=FALSE;
		    string ss1 = '[' + *(yyvsp[(1) - (8)].str) + ']';// + 'd';
                    (yyval.TERSptr) = TERS((yyvsp[(8) - (8)].actionptr),FALSE,(yyvsp[(4) - (8)].intval),(yyvsp[(6) - (8)].intval),FALSE,ss1.c_str(),(yyvsp[(8) - (8)].actionptr)->dist);
                    //$$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                    //                         FALSE, Cstr, $6->dist),";");
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
                    delete (yyvsp[(1) - (8)].str);
		  }
    break;

  case 951:
/* Line 1787 of yacc.c  */
#line 5843 "parser.y"
    {
		    (yyvsp[(1) - (3)].actionptr)->vacuous=FALSE;
                    (yyval.TERSptr) = TERS((yyvsp[(1) - (3)].actionptr),FALSE,0,INFIN,FALSE,"[true]",(yyvsp[(1) - (3)].actionptr)->dist);
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),(yyvsp[(3) - (3)].TERSptr),";");
                    //$$ = TERSmakeloop($$);
		  }
    break;

  case 952:
/* Line 1787 of yacc.c  */
#line 5850 "parser.y"
    {
		    (yyvsp[(1) - (4)].actionptr)->vacuous=TRUE;
                    (yyval.TERSptr) = TERS((yyvsp[(1) - (4)].actionptr),FALSE,0,INFIN,FALSE,"[true]",(yyvsp[(1) - (4)].actionptr)->dist);
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),(yyvsp[(4) - (4)].TERSptr),";");
                    //$$ = TERSmakeloop($$);
		  }
    break;

  case 953:
/* Line 1787 of yacc.c  */
#line 5859 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr),TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)),"#"); }
    break;

  case 955:
/* Line 1787 of yacc.c  */
#line 5864 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr),"||"); }
    break;

  case 957:
/* Line 1787 of yacc.c  */
#line 5869 "parser.y"
    { (yyvsp[(1) - (1)].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[(1) - (1)].actionptr), FALSE, (yyvsp[(1) - (1)].actionptr)->lower, (yyvsp[(1) - (1)].actionptr)->upper, FALSE,
			      "[true]",(yyvsp[(1) - (1)].actionptr)->dist);
		  }
    break;

  case 958:
/* Line 1787 of yacc.c  */
#line 5874 "parser.y"
    {
                    (yyval.TERSptr) = TERS(make_dummy((yyvsp[(1) - (1)].actionptr)->label),FALSE,(yyvsp[(1) - (1)].actionptr)->lower,(yyvsp[(1) - (1)].actionptr)->upper, 
			      FALSE,"[true]",(yyvsp[(1) - (1)].actionptr)->dist);
                  }
    break;

  case 959:
/* Line 1787 of yacc.c  */
#line 5879 "parser.y"
    { (yyvsp[(6) - (6)].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[(6) - (6)].actionptr), FALSE, (yyvsp[(2) - (6)].intval), (yyvsp[(4) - (6)].intval), FALSE); }
    break;

  case 961:
/* Line 1787 of yacc.c  */
#line 5884 "parser.y"
    { (yyval.TERSptr) = TERSempty(); }
    break;

  case 962:
/* Line 1787 of yacc.c  */
#line 5888 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)), "|"); }
    break;

  case 963:
/* Line 1787 of yacc.c  */
#line 5890 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)), ";"); }
    break;

  case 964:
/* Line 1787 of yacc.c  */
#line 5892 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)), ":"); }
    break;

  case 965:
/* Line 1787 of yacc.c  */
#line 5894 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), (yyvsp[(3) - (3)].TERSptr), "||"); }
    break;

  case 966:
/* Line 1787 of yacc.c  */
#line 5895 "parser.y"
    { (yyval.TERSptr) = (yyvsp[(2) - (3)].TERSptr); }
    break;

  case 969:
/* Line 1787 of yacc.c  */
#line 5900 "parser.y"
    {  (yyval.TERSptr)=(yyvsp[(2) - (3)].TERSptr);  (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");  }
    break;

  case 970:
/* Line 1787 of yacc.c  */
#line 5901 "parser.y"
    { (yyval.TERSptr) = (yyvsp[(2) - (3)].TERSptr); }
    break;

  case 971:
/* Line 1787 of yacc.c  */
#line 5902 "parser.y"
    { (yyval.TERSptr)=TERSmakeloop((yyvsp[(3) - (4)].TERSptr)); }
    break;

  case 972:
/* Line 1787 of yacc.c  */
#line 5906 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)),"|"); }
    break;

  case 974:
/* Line 1787 of yacc.c  */
#line 5911 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr), TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)),";"); }
    break;

  case 975:
/* Line 1787 of yacc.c  */
#line 5913 "parser.y"
    {
		    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (5)].TERSptr), TERSrename((yyvsp[(1) - (5)].TERSptr),(yyvsp[(3) - (5)].TERSptr)),";");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
    break;

  case 976:
/* Line 1787 of yacc.c  */
#line 5918 "parser.y"
    {
		    TERSmodifydist((yyvsp[(1) - (6)].TERSptr),(yyvsp[(3) - (6)].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (6)].TERSptr), TERSrename((yyvsp[(1) - (6)].TERSptr),(yyvsp[(6) - (6)].TERSptr)),";");
		  }
    break;

  case 977:
/* Line 1787 of yacc.c  */
#line 5923 "parser.y"
    {
		    TERSmodifydist((yyvsp[(1) - (8)].TERSptr),(yyvsp[(3) - (8)].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (8)].TERSptr), TERSrename((yyvsp[(1) - (8)].TERSptr),(yyvsp[(6) - (8)].TERSptr)),";");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
    break;

  case 978:
/* Line 1787 of yacc.c  */
#line 5929 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (4)].TERSptr), TERSrename((yyvsp[(1) - (4)].TERSptr),(yyvsp[(4) - (4)].TERSptr)),":"); }
    break;

  case 979:
/* Line 1787 of yacc.c  */
#line 5931 "parser.y"
    {
		    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (6)].TERSptr), TERSrename((yyvsp[(1) - (6)].TERSptr),(yyvsp[(4) - (6)].TERSptr)),":");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
    break;

  case 980:
/* Line 1787 of yacc.c  */
#line 5936 "parser.y"
    {
		    TERSmodifydist((yyvsp[(1) - (7)].TERSptr),(yyvsp[(3) - (7)].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (7)].TERSptr), TERSrename((yyvsp[(1) - (7)].TERSptr),(yyvsp[(7) - (7)].TERSptr)),":");
		  }
    break;

  case 981:
/* Line 1787 of yacc.c  */
#line 5941 "parser.y"
    {
		    TERSmodifydist((yyvsp[(1) - (9)].TERSptr),(yyvsp[(3) - (9)].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[(1) - (9)].TERSptr), TERSrename((yyvsp[(1) - (9)].TERSptr),(yyvsp[(7) - (9)].TERSptr)),":");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
    break;

  case 982:
/* Line 1787 of yacc.c  */
#line 5959 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (1)].floatval); }
    break;

  case 983:
/* Line 1787 of yacc.c  */
#line 5960 "parser.y"
    { (yyval.floatval) = (double)(yyvsp[(1) - (1)].intval); }
    break;

  case 984:
/* Line 1787 of yacc.c  */
#line 5962 "parser.y"
    { checkdefine((yyvsp[(1) - (1)].actionptr)->label,(yyvsp[(1) - (1)].actionptr)->type);
		    (yyval.floatval) = (yyvsp[(1) - (1)].actionptr)->real_def; }
    break;

  case 985:
/* Line 1787 of yacc.c  */
#line 5964 "parser.y"
    { (yyval.floatval) = (yyvsp[(3) - (6)].floatval)+(yyvsp[(5) - (6)].floatval); }
    break;

  case 986:
/* Line 1787 of yacc.c  */
#line 5965 "parser.y"
    { (yyval.floatval) = (yyvsp[(3) - (6)].floatval)-(yyvsp[(5) - (6)].floatval); }
    break;

  case 987:
/* Line 1787 of yacc.c  */
#line 5966 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (3)].floatval) * (yyvsp[(3) - (3)].floatval); }
    break;

  case 988:
/* Line 1787 of yacc.c  */
#line 5967 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (3)].floatval) * (yyvsp[(3) - (3)].intval); }
    break;

  case 989:
/* Line 1787 of yacc.c  */
#line 5969 "parser.y"
    { checkdefine((yyvsp[(3) - (3)].actionptr)->label,(yyvsp[(3) - (3)].actionptr)->type);
		    (yyval.floatval) = (yyvsp[(1) - (3)].floatval) * (yyvsp[(3) - (3)].actionptr)->real_def; }
    break;

  case 990:
/* Line 1787 of yacc.c  */
#line 5972 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (8)].floatval)*((yyvsp[(5) - (8)].floatval)+(yyvsp[(7) - (8)].floatval)); }
    break;

  case 991:
/* Line 1787 of yacc.c  */
#line 5974 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (8)].floatval)*((yyvsp[(5) - (8)].floatval)-(yyvsp[(7) - (8)].floatval)); }
    break;

  case 992:
/* Line 1787 of yacc.c  */
#line 5975 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (3)].floatval) / (yyvsp[(3) - (3)].floatval); }
    break;

  case 993:
/* Line 1787 of yacc.c  */
#line 5976 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (3)].floatval) / (yyvsp[(3) - (3)].intval); }
    break;

  case 994:
/* Line 1787 of yacc.c  */
#line 5978 "parser.y"
    { checkdefine((yyvsp[(3) - (3)].actionptr)->label,(yyvsp[(3) - (3)].actionptr)->type);
		    (yyval.floatval) = (yyvsp[(1) - (3)].floatval) / (yyvsp[(3) - (3)].actionptr)->real_def; }
    break;

  case 995:
/* Line 1787 of yacc.c  */
#line 5981 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (8)].floatval)/((yyvsp[(5) - (8)].floatval)+(yyvsp[(7) - (8)].floatval)); }
    break;

  case 996:
/* Line 1787 of yacc.c  */
#line 5983 "parser.y"
    { (yyval.floatval) = (yyvsp[(1) - (8)].floatval)/((yyvsp[(5) - (8)].floatval)-(yyvsp[(7) - (8)].floatval)); }
    break;

  case 998:
/* Line 1787 of yacc.c  */
#line 5988 "parser.y"
    {
		    (yyval.TERSptr) = Guard(*(yyvsp[(1) - (1)].str));
                    delete (yyvsp[(1) - (1)].str);
		  }
    break;

  case 999:
/* Line 1787 of yacc.c  */
#line 5993 "parser.y"
    {
                    //char *t = new char[strlen($6) + 3];
                    //sprintf(t, "[%s]", $6);
		    string ss = '[' + *(yyvsp[(6) - (6)].str) + ']';
                    (yyval.TERSptr) = TERS(dummyE(), FALSE, (yyvsp[(2) - (6)].intval), (yyvsp[(4) - (6)].intval), FALSE, ss.c_str());
                    delete (yyvsp[(6) - (6)].str);
                  }
    break;

  case 1000:
/* Line 1787 of yacc.c  */
#line 6003 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr),TERSrename((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr)),"#"); }
    break;

  case 1002:
/* Line 1787 of yacc.c  */
#line 6008 "parser.y"
    {
		    string ss = *(yyvsp[(1) - (3)].str) + '|' + *(yyvsp[(3) - (3)].str);
		    (yyval.str) = (yyval.str) = new string(ss);
                    delete (yyvsp[(3) - (3)].str);
		  }
    break;

  case 1004:
/* Line 1787 of yacc.c  */
#line 6017 "parser.y"
    {
		    string ss = *(yyvsp[(1) - (3)].str) + '&' + *(yyvsp[(3) - (3)].str);
		    (yyval.str) = new string(ss);
                    delete (yyvsp[(3) - (3)].str);
		  }
    break;

  case 1006:
/* Line 1787 of yacc.c  */
#line 6025 "parser.y"
    { (yyval.str) = new string((yyvsp[(1) - (1)].actionptr)->label); }
    break;

  case 1007:
/* Line 1787 of yacc.c  */
#line 6027 "parser.y"
    {
		    string ss = '~' + string((yyvsp[(2) - (2)].actionptr)->label);
		    (yyval.str) = new string(ss);
		  }
    break;

  case 1008:
/* Line 1787 of yacc.c  */
#line 6032 "parser.y"
    {
		    string ss = '(' + *(yyvsp[(2) - (3)].str) + ')';
		    (yyval.str) = new string( ss );
                  }
    break;

  case 1009:
/* Line 1787 of yacc.c  */
#line 6037 "parser.y"
    {
		    string ss = '(' + *(yyvsp[(3) - (4)].str) + ')';
		    ss = '~' + ss;
		    (yyval.str) = new string( ss );
                  }
    break;

  case 1010:
/* Line 1787 of yacc.c  */
#line 6043 "parser.y"
    {
		    if((yyvsp[(1) - (1)].boolval) == TRUE)
		      (yyval.str) = new string("true");
		    else
		      (yyval.str) = new string("false");
		  }
    break;

  case 1011:
/* Line 1787 of yacc.c  */
#line 6052 "parser.y"
    { (yyval.TERSptr) = TERScompose((yyvsp[(1) - (3)].TERSptr),(yyvsp[(3) - (3)].TERSptr),"||"); }
    break;

  case 1013:
/* Line 1787 of yacc.c  */
#line 6057 "parser.y"
    {
		    (yyvsp[(1) - (1)].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[(1) - (1)].actionptr),TRUE,(yyvsp[(1) - (1)].actionptr)->lower,(yyvsp[(1) - (1)].actionptr)->upper,FALSE,"[true]",
			      (yyvsp[(1) - (1)].actionptr)->dist);
		  }
    break;

  case 1014:
/* Line 1787 of yacc.c  */
#line 6063 "parser.y"
    {
		    (yyvsp[(1) - (3)].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[(1) - (3)].actionptr),TRUE,(yyvsp[(1) - (3)].actionptr)->lower,(yyvsp[(1) - (3)].actionptr)->upper,FALSE,"[true]",
			      (yyvsp[(1) - (3)].actionptr)->dist,(yyvsp[(3) - (3)].intval));
		  }
    break;

  case 1015:
/* Line 1787 of yacc.c  */
#line 6069 "parser.y"
    { (yyvsp[(1) - (2)].actionptr)->vacuous=TRUE;
		    (yyval.TERSptr) = TERS((yyvsp[(1) - (2)].actionptr),TRUE,(yyvsp[(1) - (2)].actionptr)->lower,(yyvsp[(1) - (2)].actionptr)->upper,FALSE,"[true]",
			      (yyvsp[(1) - (2)].actionptr)->dist); }
    break;

  case 1016:
/* Line 1787 of yacc.c  */
#line 6079 "parser.y"
    { (yyval.TERSptr) = (yyvsp[(2) - (3)].TERSptr); }
    break;


/* Line 1787 of yacc.c  */
#line 11198 "parser.tab.c"
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


/* Line 2050 of yacc.c  */
#line 6084 "parser.y"





int yyerror(char* S)
{
  printf("%s\n",S);
  fprintf(lg,"%s\n",S);
  printf("%s: %d: syntax error.\n", file_scope1, linenumber);
  fprintf(lg,"%s: %d: syntax error.\n", file_scope1, linenumber);

  clean_buffer();
  compiled=FALSE;
  return 1;
}

int yywarning(char* S)
{
  printf("%s\n",S);
  fprintf(lg,"%s\n",S);
  //printf("%s: %d: syntax error.\n", file_scope1, linenumber);
  //fprintf(lg,"%s: %d: syntax error.\n", file_scope1, linenumber);

  //clean_buffer();
  //compiled=FALSE;
  return 0;
}


void constructor()
{
  //table = new SymTab;

  PSC = 0;
}


void destructor()
{
  //delete table;
  clean_buffer();
}


actionADT make_dummy(char *name)
{
  char s[1000]; //= string("$") + (int)PSC + '+';
  sprintf(s, "$%s",name);
  actionADT a = action(s, strlen(s));
  a->type = DUMMY;
  return a;
}

actionADT dummyE()
{
  char s[1000]; //= string("$") + (int)PSC + '+';
  sprintf(s, "$%ld+", PSC);
  actionADT a = action(s, strlen(s));
  a->type = DUMMY;
  PSC++;
  return a;
}

//Converts an double into a string
//An int string for now
string numToString(double num) {
  char cnumber[50];
  sprintf(cnumber, "%d", (int)num);
  string toReturn = cnumber;
  return toReturn;
}

//Converts an double into a string
//An int string for now
string intToString(int num) {
  char cnumber[50];
  sprintf(cnumber, "%i", (int)num);
  string toReturn = cnumber;
  return toReturn;
}


// Create a timed event-rule structure for an guard.
TERstructADT Guard(const string & expression){
  string bracketed('[' + expression + ']');
  return TERS(dummyE(),FALSE,0,0,FALSE,bracketed.c_str());
}

TERstructADT Assign(const string & expression) {
  string angled('<' + expression + '>');
  return TERS(dummyE(),FALSE,0,0,FALSE,angled.c_str());
}

// Create a timed event-rule structure for an guarded action.
TERstructADT Guard(const string & expression,
		   actionADT target,
		   const string&data=""){
  string bracketed('[' + expression + ']');
  return TERS(target,FALSE,target->lower,target->upper,FALSE,
	      bracketed.c_str(),NULL,-1,data);
}

TERstructADT FourPhase(const string & channel, const string & data){
  string req(channel+reqSuffix);//channel_send for send,channel_rcv for receive
  string ack(channel+ackSuffix);//channel_rcv for send,channel_send for receive
  TERstructADT result(TERS(++req,FALSE,(++req)->lower,(++req)->upper,FALSE,
			   "[true]",NULL,-1,data));
  result =  TERScompose(result, Guard(    ack,--req,data),";");
  result =  TERScompose(result, Guard('~'+ack, join     ),";");
  int antiType((SENT|RECEIVED)^direction);
  map<string,int>& ports = tel_tb->port_decl();
  if(signals[cur_entity].find(channel) == signals[cur_entity].end()){
    if(ports.find(channel) == ports.end()){
      err_msg("Undeclared channel ", channel);
    }
    else{
      if(ports[channel] & antiType){
	err_msg("send and receive on the same side of channel ", channel);
      }
      ports[channel] |= direction;
    }
  }
  else{
    if(signals[cur_entity][channel] & direction){
      if(direction & SENT){
	err_msg("Attempt to send on both sides of channel ", channel);
      }
      else{
	err_msg("Attempt to receive on both sides of channel ",	channel);
      }
    }
    if(processChannels.find(channel)==processChannels.end()){
      processChannels[channel]=direction;
    }
    else{
      if(processChannels[channel] & antiType){
	err_msg("send and receive on the same side of channel ", channel);
      }
      processChannels[channel] |= direction;
    }
  }
  return result;
}

TYPES * Probe(const string & channel){
  map<string,int>& ports(tel_tb->port_decl());
  if(ports.find(channel)==ports.end()){
    if(signals[cur_entity].find(channel)==
       signals[cur_entity].end()){
      err_msg("Probing undeclared channel ", channel);
    }
    if(signals[cur_entity][channel]&PASSIVE){
      err_msg("Channel ", channel, " is probed on both sides!");
    }
    processChannels[channel] |= PASSIVE;
    signals[cur_entity][channel] |= PASSIVE;
  }
  else{
    if(ports[channel] & ACTIVE){
      err_msg("Attempt to probe active channel ", channel);
    }
    ports[channel] |= PASSIVE;
  }
  TYPES * result = new TYPES;
  result->set_obj("exp");
  result->set_str(logic(channel+SEND_SUFFIX, channel+RECEIVE_SUFFIX, "|"));
  result->set_data("bool");
  return result;
}

actionADT operator++(const string & name){
  actionADT rise(action(name+"+"));
  rise->type &= ~IN;
  rise->type |= OUT;
  return rise;
}

actionADT operator--(const string & name){
  actionADT fall(action(name+"-"));
  fall->type &= ~IN;
  fall->type |= OUT;
  return fall;
}

void declare(list<pair<string,int> >* ports){
  if(ports){
    map<string,int> tmp(ports->begin(), ports->end());	
    tel_tb->insert(tmp);
    tel_tb->insert(*ports);
    for(map<string,int>::iterator b = tmp.begin();  b != tmp.end(); b++){
      if(b->second & IN){
	table->insert(b->first, make_pair((string)"in",(TYPES*)0));
      }
      else{
	table->insert(b->first, make_pair((string)"out",(TYPES*)0));
      }
    }
    delete ports;
  }
}

TERstructADT signalAssignment(TYPES* target, TYPES* waveform){
  string temp;
  if(waveform->get_string() == "'1'")
    temp =  target->get_string() + '+';
  else if(waveform->get_string() == "'0'")
    temp =  target->get_string() + '-';
  else
    temp = '$';// + target->get_string(); //"error";

  int l = 0,  u = INFIN;
  const TYPELIST *tl = waveform->get_list();
  if(tl != 0){
    TYPELIST::const_iterator I = tl->begin();
    if(I != tl->end()) {
      l = tl->front().second.get_int();
      I++;
      if(I!=tl->end())
	u = tl->back().second.get_int();
    }
  }
  actionADT a;
  if (temp[0] == '$') {
    a = dummyE();
    //a->type = DUMMY;
    // cout << "target = " << target->get_string() << " waveform = " << waveform->get_string() << endl;
    // ZHEN(Done): add assignment to action "a" of (target->get_string(),waveform->get_string())
    // a->list_assigns= NULL;
    if (target->get_string().c_str()!=NULL && waveform->get_string().c_str()!=NULL){
      a->list_assigns = addAssign(a->list_assigns, target->get_string(), waveform->get_string());
    }
  } else {
    a = action(temp);
  }
  TERstructADT result(TERS(a, FALSE, l, u, TRUE));
  delete_event_set(result->last);
  result->last = 0;
  return result;
}
