/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 6 "parser.y" /* yacc.c:339  */

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



    

#line 191 "parser.tab.c" /* yacc.c:339  */

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
   by #include "parser.tab.h".  */
#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    GE = 433,
    LE = 434,
    NEQ = 435,
    MED_PRECEDENCE = 436,
    MAX_PRECEDENCE = 437
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 133 "parser.y" /* yacc.c:355  */

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

#line 445 "parser.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 461 "parser.tab.c" /* yacc.c:358  */

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
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3576

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  205
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  391
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1017
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1946

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   437

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   186,   199,
     195,   196,   191,   184,   194,   185,   200,   192,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   190,   189,
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
     175,   176,   177,   181,   182,   183,   188,   193
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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
  "'='", "GE", "LE", "NEQ", "'+'", "'-'", "'&'", "'|'", "MED_PRECEDENCE",
  "';'", "':'", "'*'", "'/'", "MAX_PRECEDENCE", "','", "'('", "')'", "'['",
  "']'", "'\\''", "'.'", "'~'", "'{'", "'}'", "'@'", "$accept",
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
  "csp_literal", YY_NULLPTR
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
      61,   433,   434,   435,    43,    45,    38,   124,   436,    59,
      58,    42,    47,   437,    44,    40,    41,    91,    93,    39,
      46,   126,   123,   125,    64
};
# endif

#define YYPACT_NINF -1531

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1531)))

#define YYTABLE_NINF -905

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      80,    39, -1531,   750,   275, -1531, -1531,    83, -1531,   283,
     394, -1531, -1531,   519,   221, -1531, -1531,   623, -1531,   980,
   -1531, -1531, -1531,   590, -1531,   270,   597,   655,   681,   144,
     698, -1531, -1531, -1531, -1531, -1531,   851, -1531, -1531, -1531,
   -1531,   877, -1531, -1531, -1531,   394, -1531,   909,   926, -1531,
   -1531,   844, -1531, -1531,   839,   919, -1531,   944, -1531,  1700,
     954,   957,  1110, -1531,  1128,   105, -1531, -1531,  1069,  1013,
    1016,  1075,  1078,  1084,  1093,  1155,  1161,  1170, -1531, -1531,
   -1531, -1531,  1568, -1531, -1531,  1222,  1270,   157, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  1007,  1182,  1321,  1182,
     982, -1531,  1236, -1531, -1531,  1273,  1279, -1531, -1531, -1531,
   -1531,   110,  1360, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
    1407, -1531, -1531, -1531, -1531, -1531,  1018,  1304,  1313,  1327,
    1333,  1358,  1361,  1366,  1320, -1531, -1531, -1531, -1531, -1531,
   -1531,  1378,  1387, -1531,  1490, -1531,  3424, -1531, -1531, -1531,
    1371, -1531,   763,  1585,    14,  1020, -1531, -1531,  1433,   115,
    1594,   177, -1531, -1531,   982,  1595,  1182,  1437, -1531,  1476,
    1055,  1108,   765,  1108,   289,   808,  1108,  1479, -1531,   834,
     432,   432,  3302, -1531,  1480,  1485,   134,  1576,   134,  1182,
    1498,  1499,  1182, -1531, -1531, -1531, -1531,  1487, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531,   698,  1640,   698,  1513, -1531,  1497,
   -1531,   698,   432,  1495,  1520,   698,   164, -1531,    88, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  1023,   587, -1531, -1531,
    1561,  1521, -1531, -1531, -1531,  1579,  1536,  1108,   918, -1531,
   -1531,   672, -1531, -1531, -1531,   765,   711,  1580,   416, -1531,
     447,  1548, -1531,   307,  1551,  1582,   153,  1600,   472,   899,
    1610,  1171, -1531,  3156,  1593,  1596,  1597, -1531, -1531,  1748,
   -1531, -1531,  1182,  1182,  1182,  1182,  1182,  1182,  1104,   433,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,   639, -1531,
     194,   134,   995, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  1586, -1531, -1531, -1531,
   -1531,  1772,  1138, -1531, -1531,   652,  1147,  1695,  1188,  1738,
    1739,  1226, -1531,  1654,   293, -1531,  1079,   487, -1531,   352,
   -1531, -1531,  1598,   709,  3440,  1589,  1599,   698,  1590,   103,
     853,  1182,  1264,  1601, -1531, -1531, -1531, -1531,  1730, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,   698,   432,  2889,  1615,
     698,   941,  1588,  1602, -1531,  1605,  1108,   880,   745,  1648,
     928, -1531,   917, -1531,   332,  1606,  1607,  1609, -1531,  1108,
   -1531,  1108,  1108,  1049,   984, -1531,   765, -1531,  1626,   765,
    1628,   765,  1196,   765, -1531, -1531,   197, -1531, -1531,  1619,
     942,  1708, -1531,  1277, -1531,  1108,  1638,  1479,  1629, -1531,
     139, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1622, -1531,  1633,
      57,  1271,  1309,  1324,  1363,  1374,  1382,   549,   432,  1630,
    1635,   439,  1651, -1531,  1641,  1732, -1531,   535,   382,   995,
    1079,  1642,    98,  1086,  1639,  1632, -1531, -1531,  1655,  1776,
     943,  2600,  1663,  1665,  1652, -1531,   698,  2600,   698,   126,
     698,   698,  2600, -1531,   463,  1728, -1531,  1654,   198,  1650,
    2600, -1531, -1531, -1531,  1785, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1334,  1750,  1404,
   -1531, -1531,   853,  1792,  1079,  2600,  1793,   759,   698,   759,
   -1531, -1531,  1678,  1680,  1681, -1531, -1531, -1531,  2349, -1531,
     166,  1203,  1512,   340, -1531,  1759, -1531, -1531,  1650,  1143,
     566, -1531, -1531, -1531,  1428,   140, -1531, -1531, -1531,  1393,
   -1531, -1531,  1685,   839,   885,  2600,  1242,  1676,  1673, -1531,
   -1531,  1674,  1675,  1677,  1683,  1684,  1687,  1688,  1686,  2349,
      77, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531,  1689, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
     871,   494, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
     791,  1153, -1531,  1690,  1043, -1531, -1531,   493,  1055,  1698,
    1701,  1729,  1745,   862, -1531,  1747, -1531,  1699,  1035,   918,
   -1531,  1081,  1196, -1531,   392,   392,  1210,  1210, -1531, -1531,
   -1531, -1531,  1040,   765, -1531,   821,   765,  1548, -1531,  1702,
    1703, -1531, -1531,  1122, -1531, -1531, -1531,  1707,  1710, -1531,
     942, -1531,  1756, -1531, -1531, -1531,  1746, -1531, -1531, -1531,
     805,  1711,  1709,  2708,  1814,  1731, -1531,  1715,   173,   698,
     549,   549,   705,   698, -1531, -1531, -1531, -1531, -1531,  1891,
     698, -1531, -1531, -1531,   194,   240, -1531, -1531, -1531,    96,
   -1531, -1531, -1531, -1531,   207,   791, -1531, -1531,  2600,   698,
    1812,   210,  1079,  1722,  1723, -1531,  1079,   418,  1724,   403,
   -1531,   122, -1531,  1737,    86,   964, -1531,  1726,  1740,   831,
    1142,  1406, -1531, -1531, -1531,   839,  1743,   720,  1733,  1182,
    1749, -1531, -1531, -1531,   509,  1751, -1531,   598,  1753, -1531,
   -1531, -1531, -1531,  1402, -1531,  1831, -1531,   766,   304,   345,
    2600, -1531,  1728, -1531, -1531,  1754,  1724,  1755,  2600,  1892,
    1757,  3440,  1762, -1531, -1531, -1531, -1531,  1409, -1531,    33,
   -1531,  1517, -1531, -1531,   503,   698,  1650,  1741,  1079, -1531,
   -1531,   994, -1531, -1531, -1531, -1531, -1531, -1531,  1752,   699,
    1742,   499, -1531, -1531, -1531,   853,  1512, -1531, -1531, -1531,
    1203, -1531, -1531, -1531, -1531,  1203,   759,   510,   698,  1862,
   -1531,  1264, -1531,  1763, -1531,  1879,  1015, -1531, -1531,   770,
   -1531,   565,   565,  2600,  2600,  2600,   565,   565,   565, -1531,
   -1531,  2600,  2245,   565,  2600,   522,  1765,  2600,   553,  2600,
    2382,   141,  2600,   381,  1910,  1769,   301,  1650, -1531,   599,
    1764, -1531,  1615,  2600, -1531,  1760, -1531, -1531,   816,  1782,
   -1531, -1531,  1818, -1531, -1531,   190,  1108,   622,  1163,   392,
    1609, -1531, -1531,   501,   531,  1196,  1196,  1230,  1239, -1531,
     942,   942,  1707,  1774,    81,  2600,    93,  1676,  2600, -1531,
   -1531, -1531,   109, -1531, -1531, -1531, -1531, -1531, -1531,  1789,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1932,
     262,  1637, -1531,  1858,   948,  1219,  1795,  1930,  1931,   860,
    1791, -1531,  1913, -1531,  1079,  1808,   434,   680,  1788, -1531,
     231,  1403, -1531, -1531, -1531,   698,  1812, -1531,  1797,  1891,
     698,  1812,   698, -1531, -1531,   698, -1531, -1531,  1812,  1891,
    1796, -1531, -1531, -1531, -1531,  1079, -1531, -1531,   242, -1531,
   -1531, -1531,  1798,   650,  2600,  1026,  1812,  1800, -1531, -1531,
    1650,  1079,   113,  1794,  1650,   122,  1799,  1801,  1802, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531,  1951,  1805,  1804,
    1650,  1957,   565, -1531,  2600,  2600,  2600,  2600,  2600,  2600,
    1992,  1940,  1908, -1531, -1531, -1531, -1531, -1531, -1531,   853,
   -1531, -1531, -1531, -1531, -1531, -1531,   853, -1531, -1531, -1531,
    1812,   853,  1946,  1436,   171, -1531,  1079, -1531,  2600,  1877,
   -1531,   463, -1531,  2510,   853,  1881, -1531, -1531, -1531, -1531,
     612, -1531,  1750, -1531,  1541,  1817,  1952, -1531,    92,   853,
   -1531,  2169,   224, -1531, -1531,  1650,  1816, -1531, -1531,  1070,
   -1531,  2349,  2600,  2556, -1531,   340, -1531, -1531,  2349, -1531,
    1824,  1840, -1531, -1531,   176,   698,  2600,  1842,  1830, -1531,
    1832,  1826,  1525, -1531,  1828,  1526, -1531,  1834,  1836,  1532,
    1837,  1841,  1843,  1838,  1847, -1531,  1846,  1848,  1849,  1850,
    1851,  1859, -1531,  2600, -1531, -1531,  2492,  1103, -1531,  2021,
    1545,  1549, -1531,  1994,  2600,  1860, -1531,  2008,  1968,  2600,
    1866, -1531,   143, -1531,  1867,  2600,  1079,  2600, -1531,   130,
     128,  1868, -1531,  1079,  2600,  1869,   288, -1531,   310, -1531,
   -1531,  2600,  2245,  1961,   853, -1531, -1531,  1871,  1872, -1531,
    1918,  1889,  1901,  1899,   644,  1326,  1971,  1903,  1904,  1217,
    1455, -1531,  1887,  1888, -1531, -1531, -1531,  1890,  1893, -1531,
   -1531,  1710, -1531,  1942,  1915, -1531,  1895,  1999, -1531, -1531,
    2012,  2600,  1932,   157,   780, -1531,  2600,   318, -1531,  1079,
    1079,  1079,  1919,  2600,   172,  1769, -1531,   625, -1531, -1531,
   -1531,  2058,  1978,   138,  1274,   853,   386,  1902,  1905,  1079,
    1079, -1531,   625, -1531, -1531,  1650, -1531, -1531,  2066,   142,
   -1531, -1531, -1531,  1812, -1531,   698,  1812,  1891, -1531,  1812,
    1812, -1531,  1812, -1531,  1552,  1650, -1531, -1531, -1531, -1531,
   -1531,   853,   853,  1909,  1912, -1531, -1531, -1531,  1972, -1531,
    1650,  1079,  1907, -1531, -1531, -1531,   853,  2600,  2600,  3440,
    1065, -1531, -1531, -1531,  2077,   157,  1911, -1531, -1531, -1531,
   -1531, -1531, -1531,  2600,  2600,  2600, -1531, -1531,  1917,   762,
     698,   698,  2035,  1448,   237,  1995,  2600, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531,  1079,  1650, -1531, -1531, -1531, -1531,
     698,  2010, -1531, -1531, -1531,  1556, -1531,   556,  1921,   166,
   -1531,  1922, -1531, -1531,  2055,  1935, -1531,   839,  2044, -1531,
   -1531, -1531,  2600,   565,  1939,  2600,   565,  1944,  1948,  1949,
    2637,  1950,  1958,  2600,  2600,  1959,  2600,  1963,  1965,   565,
    1966,   565, -1531,  2064,  1112, -1531,  2600,  2600,  2600,  1967,
     565,  1970,  2048,  1973, -1531,  2049,   853, -1531,  1976, -1531,
    2054,  1986, -1531, -1531, -1531,  1974, -1531, -1531,  1977, -1531,
     132,  1980, -1531,   205, -1531, -1531,   318,  1982,  2600,  2900,
    1985,  2492,  1156,   853, -1531, -1531,  1981,  1983, -1531,   765,
    1388,  1536,  1105, -1531, -1531,  1196,  1196,  1196,  1196,  1196,
    1196,  1998, -1531, -1531, -1531,   565,  1984,   157,   701,  2109,
   -1531,   997, -1531, -1531, -1531,  2091,   149,   945,  1650,  1650,
    2142, -1531,  2097, -1531, -1531,  1997,  2113, -1531,   853,   526,
    1654,  2070,  2510,  2510,  1650,  1650, -1531,  2000,  1286,   795,
   -1531, -1531, -1531,  1812, -1531, -1531, -1531,  1079,  1932,  3343,
   -1531,  1996, -1531, -1531,  1650, -1531,  1557,  2001, -1531,  1560,
    2007, -1531,  1650,  2185, -1531,  1490, -1531, -1531, -1531, -1531,
   -1531,  2082,  2147, -1531,  2019,  2036,   698,  2037,  2600,  2094,
   -1531, -1531,  2349, -1531,  2600, -1531, -1531,  2038,  1439,  2039,
    2020, -1531, -1531,  2022, -1531, -1531, -1531, -1531,   444, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  2023, -1531, -1531,  2556,
     426, -1531,  2600, -1531,  2691, -1531,  2600,  2026, -1531, -1531,
    2029, -1531,  1079, -1531, -1531, -1531,  2172, -1531, -1531,  1191,
   -1531,  2032,  2078,  2051,  1607,  1536,  1108,   662,  1254,  1287,
    1469,  1477,  1501,  1510,  2050,  2420,  2045, -1531, -1531,  1913,
    2040,  1913,  3178, -1531,  2041,  2492,  3487,  2134,  2145,  2052,
   -1531,   853, -1531, -1531,  1896,   572, -1531, -1531,  1855, -1531,
    1654,  2123,  2053,   853,  1565, -1531,  2149, -1531,  1566, -1531,
    2213,  2220,  2061, -1531,  1650, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531,   853,  2600, -1531, -1531,  2600, -1531, -1531, -1531,
     686, -1531,   855, -1531,   698, -1531, -1531,  2063,  2124,  2126,
    2600, -1531, -1531, -1531,  2080,  2084,  2086, -1531,  2067, -1531,
    2093,  2095,  2088,   558,  2248,  2556,  2081,  1399,  2083, -1531,
   -1531,   337,   728, -1531, -1531,   852,  2073,   688,  1395, -1531,
   -1531,  1196,  1196,  1196,  1196, -1531, -1531,  1434,  2251,  2580,
     780,  3280,  2085,  1490,  2089, -1531, -1531, -1531,   331, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531,  2556,   552,  2099, -1531, -1531,  2420,  2225,
    2239, -1531, -1531,  2101,   853, -1531,  2102,  2510, -1531,  2510,
   -1531,  2104,  2247,  1461,  2920, -1531, -1531, -1531,  2112, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  2148, -1531, -1531, -1531,
   -1531,  2090,  2108, -1531, -1531,   797,   568, -1531, -1531,  2600,
    2254, -1531,  2600,  2114, -1531, -1531,  2130,  2132,  1440,  1536,
    1297,  1335,  1345,  1429, -1531,  2600,  2280,  2600,  2492, -1531,
   -1531,  1913, -1531,  2314,  2600,  2992,   589,  2289,  2556, -1531,
    1434,  2278,  2139, -1531,  2125, -1531, -1531, -1531, -1531,  2129,
    2290,  2295,  2253, -1531, -1531, -1531,  2154,  2156,  1962,  2138,
   -1531, -1531,  2997,  2246,  2291, -1531, -1531,  2136,  2137,  1536,
   -1531, -1531, -1531, -1531,  2420,  2244,  2256,  2097,  1213,   354,
    2314,  2158,  2320, -1531, -1531, -1531,  2312,  2227, -1531,   802,
     605,  2323,  2139, -1531,  2161, -1531, -1531,  2167,  2327, -1531,
    2288, -1531, -1531, -1531,  2812, -1531,   846,  2186,   864, -1531,
    2171, -1531,  2492,  2556, -1531, -1531,   898, -1531,   931, -1531,
    2420,  2173, -1531, -1531,  2273,  2174, -1531, -1531,  2177,   936,
   -1531,  2801,  2180, -1531,  2176, -1531,  2228,  2420, -1531,   356,
    2184,  2187, -1531,  2188, -1531, -1531,  2420,  2199, -1531, -1531,
    2189, -1531, -1531,  2201,  2181,  2556, -1531, -1531,  2190, -1531,
    2193,  2236,  2184, -1531,  2203,   902, -1531, -1531,  2208,  2191,
    2217,  2197,  2219,  2200,  2224, -1531
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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
     778,   779,   780,   798,   796,   794,   799,   797,   795,     0,
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

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1531, -1531, -1531,  1906,    12, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531,  1704, -1531,   -79,   -84, -1531,
   -1531,   -62, -1531,  1323,   -63, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,  -940, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,   -59,  2261, -1531, -1531,
     -47, -1531, -1531,  -128, -1531, -1531,   -57, -1531, -1531, -1531,
   -1531, -1531,    52, -1531,   -55,   -49,    74, -1531, -1531,   -54,
   -1531,   255,  2230, -1531,  -332,  1933,  -345, -1531,   -58, -1531,
   -1531, -1531,  1091,   -52, -1531, -1531, -1531,   922,  -489, -1531,
    1414, -1531,  2281, -1531, -1531, -1531, -1531, -1531,  -131,  -142,
   -1531, -1531, -1531, -1531,  1131,  -743, -1531,  1173, -1531, -1531,
     955, -1531,  -927,   989,  -105,   261,  2131,  2255, -1531,  2259,
     -12,   411, -1531,  -163, -1531, -1531,  1553, -1531, -1531, -1531,
   -1531,  1603, -1531,  -141,  -338,  1874, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531,  1385,  -755,  -559, -1531, -1531, -1531,  1863,
   -1531, -1531, -1531,   -50,  2222,  1657,  1027,  -213,  -686,  -471,
   -1531, -1531, -1531, -1531,   479,  -473, -1531,  -676, -1531,   600,
   -1531, -1531,  -868,  1004, -1531,   858,   654,  -258,  1987, -1531,
   -1531, -1531, -1531, -1531,  1292, -1531, -1531,   742,  1988, -1531,
   -1531,  1989, -1531,  -850,  -362,   752,  -830, -1531, -1531,  -812,
   -1531, -1531, -1531,   603, -1531,   859, -1531, -1531, -1531,  -655,
   -1521, -1530, -1531, -1531,   729,   665, -1531, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531,  -364, -1531, -1531, -1531, -1531, -1531,
   -1531, -1531,  1097,  1098,  -822,  1107,  1109,  -796, -1531, -1531,
    1311,  1033, -1048,  -355, -1531, -1531, -1531, -1531,  1101, -1531,
   -1531,  1130, -1531, -1531, -1531,  1134, -1531, -1085,  1111, -1531,
    -352, -1531, -1531, -1531,  -639,  -422, -1329, -1531, -1531, -1531,
   -1531, -1531, -1531, -1531, -1531, -1531,  -361, -1531, -1531, -1105,
    1267,  1080,   -72,  2024, -1531, -1531, -1531, -1531, -1531, -1531,
    -884,  -834, -1531, -1531, -1531,  -638,  -279, -1531,  1430,  1030,
     757,  -469, -1034,  1090, -1531, -1531, -1531,  -858, -1531,  1456,
   -1531,  -284, -1531,  1936, -1531,  1666, -1531,  1667,  -459, -1531,
    1412,  1956,  -257,   -30,  2485, -1531, -1531, -1531,  -342,  -673,
   -1531, -1531, -1531, -1531, -1531, -1531,   299, -1531, -1035, -1068,
    1413, -1531, -1531,  2513, -1531, -1531, -1531,  2501, -1531, -1531,
   -1531, -1531, -1531, -1531,  -177, -1531,  2444, -1531,  2092,  2445,
    2105, -1531,  1857,  2116, -1531,  1852,  1608,  -351,  -195,  -124,
   -1531, -1531,  1873,  -525,  1878,  -347,  -110,  2115,  2117,  1875,
    1880
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

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
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
     326,   640,   334,   397,   210,   317,   768,   908,   994,  1239,
     348,   338,  1264,   351,   743,   744,   697,  1268,  1023,   697,
     363,   994,   162,   243,  1271,   414,   740,   223,  1165,    51,
    1165,   554,  1165,   991,  1341,   386,   849,   291,   367,  1284,
     331,  1165,  1288,  -507,   368,   871,  1307,  1308,  1309,  1310,
    1311,  1312,   746,   595,   354,   792,   359,   928,   219,   833,
    1011,   359,   141,   369,  1016,   354,   339,  1322,   199,   769,
    1166,   698,  1354,   220,   698,   401,  -507,    53,  1166,  1062,
     580,     1,   975,     9,     1,  1167,   752,  1706,   595,   226,
     201,   482,  1401,   424,   735,   142,  1318,     2,  1167,  1086,
     787,   706,   492,   492,   319,     3,   370,   371,     3,   794,
     986,  1174,  1277,   471,   472,   473,   474,   475,   476,   430,
     834,   595,   774,   595,  1024,  1215,   321,   788,  -392,   872,
    1025,   486,  1026,   488,  1844,  -391,   928,    53,   570,   886,
     370,  -392,   653,    92,   951,    16,   998,   999,  -391,    93,
    1226,    94,   493,   387,   829,   666,   661,   667,   609,   670,
     414,   995,    65,  1572,   486,   747,  1018,   816,   555,   163,
      53,   781,   670,   370,  1221,   224,   672,  1291,   340,   675,
     991,   670,  1277,   699,   151,   594,  1478,  1409,    52,  1407,
    -524,  1569,   584,  1706,   518,   829,   493,   888,  1890,   518,
    1168,  1185,  1402,   424,   252,   747,  1579,   359,   512,   689,
     486,   732,  1294,  1416,   733,   151,    53,    53,  1173,  1907,
    1355,  1609,  1421,   425,  1447,   781,   593,   431,   641,   372,
     359,   580,  1570,  1916,  1517,   370,   227,   609,   483,  1706,
     992,   253,   254,   993,   887,   486,   255,   793,  1227,  1000,
    1174,    53,  1001,  1483,  1463,   662,  1706,  1107,   256,   841,
    1199,  1200,   257,   514,   258,  1706,   781,  1007,    65,  1181,
    -775,   203,   747,  1342,   217,    53,  1418,   209,   486,  1006,
     486,  1173,   953,    23,    53,   842,    53,   370,  -392,   570,
    -392,   512,  1464,  1804,   370,  -391,   514,  -391,   661,  1480,
     847,    65,  1482,  1782,   747,  1484,  1485,   323,  1486,   954,
     741,   409,  1334,   329,    47,  1828,  1883,   359,  1925,  1832,
     595,   493,  1684,   292,  1347,  1508,  1509,  1510,  1016,   493,
     493,   293,   274,   425,   401,   724,   557,    53,   735,   401,
     401,   895,    21,  1174,   275,  1016,   765,   735,   359,   493,
     777,   777,  1416,  1182,   789,   492,   486,  1671,   514,  1010,
    1014,  1683,    53,    65,   411,  1011,   412,   426,  1069,  1121,
    1124,   747,  1874,  1735,  1135,  1135,  1135,   747,   725,   726,
     512,  1150,   418,   294,   558,  1471,  1017,   559,  1685,    53,
    1718,   295,   814,   780,   493,  1386,    53,   662,   821,   252,
     663,   843,   844,  1017,   747,  1092,   584,   907,   789,  1631,
     514,  1302,  1097,   420,   847,    65,  1901,   514,  1386,   486,
    1386,  1104,    65,   560,   296,    53,  1501,   211,   297,  1140,
    1018,  1095,   658,   675,  1087,   714,   914,   398,   433,   689,
    1212,   992,   739,   757,   993,  1091,   217,  1018,  1807,   789,
    1134,  1108,  1016,  -904,   885,   520,   486,   909,   715,  -847,
     716,   893,    53,   332,   717,  1102,  1000,  1197,  1016,  1001,
     782,   493,   785,   419,   887,   887,   151,   486,   823,   887,
     887,   887,   718,   608,  1154,  -847,   887,  1776,   823,  -843,
    1635,  1603,   478,    -3,  1066,   213,  1178,  1198,   714,   479,
    -102,   486,   561,   562,   421,   563,   564,    53,  1682,   565,
    1017,  -847,   422,  1278,  1808,  1159,  1279,   566,   567,  1633,
    -847,   715,  1524,   716,  1774,  1621,  1017,   717,   568,   421,
     957,   335,   486,  -847,  1831,  1806,   955,  1242,    53,   640,
    1306,  1016,  1712,   970,  1714,   718,   521,    53,   359,   359,
     359,   359,   894,   777,  1018,  1858,  1103,   642,   419,   609,
     359,  1092,   972,   958,  1243,   493,  1155,  -391,   514,  -391,
    1018,  1893,   847,    65,   493,  1108,   747,  1255,    22,   359,
    1563,  1156,   493,  1849,   401,  1817,   493,   493,   421,   580,
    1063,  1108,   833,    53,  -843,   391,  1068,  1160,  1183,  1017,
    -843,  1120,  -843,   486,  -843,  -843,   957,  1058,  1275,    53,
    1860,   735,  1161,  1103,   983,  1103,  -340,   580,  1004,   580,
    -847,  -847,  -847,   569,  1290,  1103,   409,  -847,  -847,   518,
     609,   249,   869,  -847,    45,  -847,   250,    24,   984,   142,
    1281,    48,  1317,  1018,  1282,   887,  1103,   570,   409,  1088,
     486,    11,  1194,   834,   518,   359,    53,   787,   493,   833,
    1331,  1099,  1103,   514,  1059,  -889,   409,  1909,    65,  1050,
    1051,  1052,  1053,  1054,  1055,   570,   409,   570,   410,  1324,
     558,  1195,   435,  1821,   788,  1910,  1451,   829,   359,  1349,
    1097,  1630,   409,  1335,   829,   285,   504,   950,   478,    49,
     957,   493,   493,  1430,   435,   480,   493,   493,   493,  1034,
     957,   505,   580,   493,  1143,  1144,   284,  1513,  1514,   560,
     834,  1698,   435,   493,   825,    50,   557,  1932,   781,   411,
    1758,   412,   413,   958,   285,  1145,  1050,  1051,  1052,  1053,
    1054,  1055,    53,   958,  1851,  1759,  -103,  1788,   435,  1278,
    1249,  1035,  1279,   959,  1036,   415,  -889,  1250,   781,   874,
     781,   286,  1037,   959,  1760,   781,   493,  1038,   478,   875,
     570,   876,  1783,   263,   558,   525,   416,   559,   877,  1275,
    1588,  1589,  1590,  1591,  1592,  1593,  1413,  1784,  1761,    58,
    1251,  1244,    30,   878,    10,  1737,  1039,   486,   561,   562,
     642,   563,   564,    53,   493,   565,    53,   655,   156,   264,
     518,    11,  1741,   560,   157,   359,   263,   879,   880,   656,
     359,    12,   359,   216,   568,   359,  1708,   217,  1512,  1119,
     265,  1470,   881,  1742,   882,   493,   266,    60,   493,  -264,
    1178,  1829,  1457,  1458,  1459,   950,  1891,   640,  -264,   950,
     888,   493,   264,   276,    61,  -264,  1830,   883,   747,    25,
    -265,  1892,  1474,  1475,  1189,   956,   409,  1490,  1491,  -265,
      26,  1121,   493,   265,  1124,  1190,  -265,    27,   421,   266,
    1191,  -264,  1497,  1302,   409,  1323,   422,  1135,    63,  1135,
    1902,   486,   561,   562,    28,   563,   564,    53,  1556,   565,
    1785,   814,  -265,   409,  1494,  1903,   493,   566,   567,    65,
      56,   789,  1905,  1667,   951,    57,  1786,    29,   568,   411,
     493,   412,   435,  1906,   580,   252,   263,   610,   637,   970,
     900,  1143,  1144,  1741,   434,   252,   263,   411,    30,   412,
     435,   789,  1911,   789,   478,    96,   654,  1335,   789,   252,
    1937,   855,  1145,  1596,  1742,  1357,   411,  1912,   412,   435,
      97,  1116,   264,   398,  1117,    98,  1938,   399,    99,  1708,
     950,   100,   264,   398,   659,  1913,   887,   101,    66,   887,
    1920,   732,   570,   400,   733,   660,   253,   254,    67,   266,
    1914,   255,   887,   400,   887,  1921,   102,  1853,    85,   266,
    -524,    86,  1465,   887,  1610,   648,   493,   888,  1622,  1581,
     103,  1286,   747,   493,   649,   747,  1109,  1476,  1855,   104,
     156,   105,    87,  1287,   970,   106,   157,    30,  -223,  1178,
     642,   642,   640,   486,   950,   642,   642,   642,    53,   487,
      89,   421,   642,  1709,  1853,   147,  1790,  1791,  1792,  1793,
     671,   148,   953,   149,  1620,   669,   252,   125,   887,   514,
     127,  1762,   951,  1096,    65,  1855,   951,   493,   167,   493,
     493,   493,   610,   637,   493,  1708,   557,   168,  1617,   954,
     221,   950,   169,   390,   217,   788,   788,   217,   252,   493,
     493,  1506,   664,   253,   254,   642,  1505,   421,   255,   249,
    1634,   903,   891,  1024,   250,   359,   912,   892,   829,  1025,
     256,  1026,   252,   357,   257,   252,   258,   486,   362,   128,
     956,  1708,   129,    53,   558,   253,   254,   559,   130,   732,
     255,   493,   733,   732,   138,   514,   733,   131,  1708,   580,
      65,   906,   256,   857,   858,  1349,   257,  1708,   258,   253,
     254,   514,   253,   254,   255,   749,    65,   255,  1743,  1088,
     777,   777,  1385,   560,   477,  1586,   256,  1386,   217,   256,
     257,  1551,   258,   257,   493,   258,  1386,   640,  1587,   781,
     359,   953,   139,   917,   918,  1691,  1709,   951,   919,  1584,
    1043,  1044,  1045,  1046,  1047,  1048,  1649,   570,   502,   132,
     953,   642,   503,   493,   953,   133,   493,   506,   514,  1746,
     678,   503,   847,    65,   134,  1580,   781,  1647,   514,   493,
    1386,   493,   889,    65,   917,   918,   151,   954,  1646,  1196,
     493,   954,   679,   680,  1599,   437,   970,   438,  1755,   681,
     682,   486,   561,   562,  1201,   563,   564,    53,   508,   565,
    1693,   951,   217,  1206,   732,  1386,   493,   733,   153,   641,
    1823,   164,   610,   637,   551,  1109,  1202,  1203,   568,   412,
     435,  1349,  1881,  1204,  1205,  1207,  1208,  1882,   917,   918,
     159,  1435,  1209,  1210,   514,   493,   511,   647,  1246,    65,
     217,  1661,  1709,  1778,  1779,  1242,  1638,  1637,   951,  1258,
    1639,  1644,  1641,  1259,  1642,  1643,   955,  1645,   587,  1650,
     588,  1823,  1640,   610,   637,   917,   918,   160,   732,  1349,
    1699,   733,   691,   161,   953,   953,   692,   493,  1794,  1795,
    1814,   708,  1697,   252,  1729,   217,   610,   637,  1709,   514,
     610,   637,   781,  1469,    65,  1728,  -315,  1648,   917,   918,
    -315,   703,   954,  1700,   170,  1709,   777,  1820,   917,   918,
     788,   285,   789,  1840,  1709,   732,   166,   640,   733,   709,
     253,   254,   171,   217,   719,   255,  1050,  1051,  1052,  1053,
    1054,  1055,   610,   637,   710,   177,   172,  1431,   217,   953,
     781,   257,   173,   258,  1349,   252,   917,   918,   801,   789,
     802,  1841,   252,   642,   641,   767,   917,   918,   174,   610,
     637,  1842,   493,  1721,  1720,  1109,   954,  1722,   640,  1724,
     175,  1725,  1726,   711,  1727,   176,  1731,   217,   142,  1723,
     953,   214,   253,   254,   712,   955,   953,   255,   217,   253,
     254,   640,   713,   180,   255,   640,   217,   252,   805,  1585,
     806,   970,   181,   257,   955,   258,  1789,   851,   955,   852,
     257,   758,   258,   954,  1730,   781,  1071,   766,  1072,  1349,
     218,    96,   779,  1084,   786,  1085,   953,   184,   229,  1674,
     797,  1675,  1676,  1677,   253,   254,    97,   640,   848,   255,
     917,   918,   217,   953,    99,  1843,  1321,   100,   222,  1801,
     217,  1839,   953,   101,   777,   257,   225,   258,  1516,   247,
     557,  1349,   217,   344,   640,   817,   917,   918,   595,  1436,
     248,  1228,   102,   280,   336,   789,  1229,  1230,   828,   337,
     917,   918,   642,  1701,  1231,   642,   103,  1232,   917,   918,
     781,  1702,   349,   350,  1233,   104,   163,   105,   642,  1244,
     642,   106,   356,    30,  -223,   856,   361,   360,   558,   642,
     364,   559,   917,   918,   365,  1703,   837,   838,   839,   870,
     135,   917,   918,   789,  1704,   393,  1234,   999,   955,   955,
     394,  1089,   781,  1090,    71,  1337,    72,  1338,   642,  1363,
    1366,  1364,  1367,   395,   641,    73,  1370,  1235,  1371,   417,
      75,   928,    76,   396,   423,   976,   977,   979,   981,  1388,
     428,  1389,    77,  1390,   642,  1391,  1487,   989,  1488,   557,
    1522,  1653,  1523,  1654,  1656,   429,  1657,   595,  1236,  1747,
    1747,  1748,  1750,   432,   436,   956,  1003,   466,  1109,   230,
     467,   469,   924,   955,   470,   641,   498,   501,   789,   507,
     509,   510,   512,   925,   549,   552,   592,   523,   550,   643,
     591,   650,   657,   664,   421,   665,   651,   558,   641,   652,
     559,   673,   641,   676,   690,   486,   561,   562,   686,   563,
     564,    53,   693,   565,   955,   926,   726,   703,   695,   722,
     955,   566,   567,   224,   723,   736,   738,   514,  1002,   753,
     737,   745,   568,    69,   754,    70,   927,   762,   751,   763,
     928,   764,  1093,   790,   641,   798,    71,   747,    72,   803,
     815,   818,   823,   789,   824,   825,   846,    73,   557,   853,
     955,    74,    75,   642,    76,   860,   595,   929,   861,   862,
     863,   641,   864,   898,    77,  1110,   869,   955,   865,   866,
    1075,  1740,   867,   868,   873,  -346,   955,   896,  1079,   899,
     897,   901,   925,   902,   920,   789,   921,   915,   916,   557,
     923,   964,    30,   965,   974,   973,   558,   595,   987,   559,
    1004,  1008,  1009,   956,   486,   561,   562,   956,   563,   564,
     608,  1032,   565,  1015,   926,   726,  1021,  1073,  1061,  1033,
     566,   567,  1057,   925,  1080,  1111,  1101,  1094,  1065,  1157,
    1067,   568,  1070,  1077,  1078,   927,  1081,   558,  1100,   928,
     559,  1083,  1113,  1129,  1132,  1132,  1115,  1179,  1180,  1184,
    1192,  1141,  1193,  1188,  1153,   926,   726,  1158,  1213,  1162,
    1164,  1224,  1172,   595,   994,  1245,   929,  -525,  1247,  1248,
    1253,   958,  1256,  1187,  1257,  1273,   927,  1280,  -699,  1289,
     928,  1265,  1293,  1298,  1295,  1299,  1296,  1297,  1300,  1303,
    1313,  1314,  1263,  1315,  1320,  1326,  1339,  1267,  1332,  1269,
    1340,  1108,  1270,  1352,  1353,  1217,  1359,   929,  1220,  1360,
    1362,  1361,  1365,   486,   561,   562,  1387,   563,   564,   608,
    1368,   565,  1369,  1372,  1375,  1373,  1392,  1374,   956,   566,
     567,  1376,  1377,  1379,  1378,  1381,  1380,  1396,  1382,  1394,
     568,  1397,   597,   642,  -699,  1399,  1403,  1412,  1415,  1423,
    1424,  1425,  1426,  1427,   486,   561,   562,  1432,   563,   564,
     608,   598,   565,   599,   600,   601,  1428,  1429,  1433,  1434,
     566,   567,  1437,  1438,  1443,  1439,  1441,  1444,  1440,  1442,
    1445,   568,  1467,  1460,  1285,  1468,  1477,  1472,  1492,  1287,
    1473,  1493,   956,  1503,   642,  1495,  1511,  1507,  1515,  1343,
    1518,  1527,   602,   603,   604,  1528,   557,  1525,  1526,    96,
     605,   606,   607,  1529,   595,   184,  1614,   642,  1532,  1550,
     486,   642,   185,  1535,    97,   186,   608,  1536,  1537,  1540,
    1559,  1562,    99,   440,  -164,   100,  1566,  1541,  1543,   956,
     925,   101,  1545,   188,  1546,  1548,  1555,   609,  1325,  1558,
    1567,   786,  1561,  1330,   558,  1565,  1568,   559,  1487,  1571,
     102,  1574,   557,   642,  1578,  1582,  1594,  -434,  1583,  1607,
    1597,  1611,   926,   726,   103,  1612,  1613,  1623,  1662,  1629,
    1652,   870,  1348,   104,   189,   105,  1658,  1655,  1351,   106,
     642,    30,  -223,   927,  1660,  1664,  1358,   928,  1665,  1670,
    1666,  1668,  1673,  1679,  1680,  1689,  1681,  1388,  1690,  1692,
     558,  1694,  1695,   559,  1696,  1705,  1733,  1710,   557,  1713,
    1717,   311,   190,  1383,   929,   312,   595,  1174,   191,   192,
    1744,  1736,  1745,  1751,  1393,  1749,  1752,  1764,   557,  1398,
    1753,  1811,  1763,  1765,  1767,  1404,  1770,  1406,  1768,   560,
    1769,  1773,   925,  1771,  1414,  1772,  1775,  1787,  1343,  1226,
    1777,  1420,  1781,  1812,  1800,  1825,   558,  1819,  1802,   559,
    1834,   486,   561,   562,  1826,   563,   564,  1615,  1809,   565,
    1813,  1815,  1481,  1818,   926,   726,   558,   566,   567,   559,
    1142,  1824,  1827,  1836,  1837,  1838,  1846,  1859,   568,  1143,
    1144,  1446,  1862,  1863,  1865,   927,  1455,   557,  1866,   928,
    1867,  1868,  1869,  1462,  1871,   595,  1872,  1873,  1876,  1875,
    1145,  1877,  1879,  1878,  1886,   560,  1887,   486,   561,   562,
    1852,   563,   564,    53,  1880,   565,   929,  1885,  1888,  1894,
    1896,   925,   557,   566,   567,   757,  1897,  1898,  1899,  1904,
    1908,  1917,  1915,  1918,   568,   558,  1919,  1521,   559,  1922,
    1923,  1103,  1924,  1928,  1930,  1931,  1926,  1927,  1929,  1933,
    1935,  1936,  1939,   926,   726,   557,  1940,  1498,  1500,  1934,
    1941,  1942,  1943,   486,   561,   562,  1944,   563,   564,   608,
     558,   565,  1945,   559,   927,  1336,   755,   197,   928,   566,
     567,   826,   324,   486,   561,   562,  1519,   563,   564,    53,
     568,   565,   742,   557,  1659,  1520,  1292,   206,   990,   566,
     567,  1502,  1479,   558,  1632,   929,   559,  1600,  -692,   560,
     568,   330,   485,   346,   835,  1186,  1319,   366,   850,  1076,
    1884,  1598,  1530,  1850,  1112,  1533,  1711,   925,  1405,   757,
    1539,  1805,  1798,  1132,  1539,  1895,  1132,  1810,   728,   730,
     731,   558,   560,  1739,   559,  1861,  1547,  1553,  1554,  1549,
    1542,  1410,   486,   561,   562,  1544,   563,   564,   608,  1608,
     565,  1557,   757,  1531,  1456,   557,  1573,  1552,   566,   567,
    1534,  1327,   721,  1628,  1816,  1316,  1105,   836,  1575,   568,
     927,  1344,  1106,   557,   820,    55,  1350,   486,   561,   562,
      20,   563,   564,    53,    46,   565,   136,   137,  1211,   694,
     913,   685,   904,   566,   567,   674,   677,   905,     0,   910,
     684,   929,   922,   558,   568,   911,   559,  1142,     0,     0,
     486,   561,   562,     0,   563,   564,    53,     0,   565,   557,
       0,   558,  1330,  1330,   559,     0,   566,   567,     0,     0,
    1328,  1163,     0,     0,     0,     0,     0,   568,     0,     0,
       0,     0,   560,   557,     0,     0,     0,     0,   486,   561,
     562,     0,   563,   564,   608,     0,   565,     0,     0,     0,
     560,     0,   757,   557,   566,   567,     0,   558,  1669,     0,
     559,     0,   870,     0,  1672,   568,     0,  1797,   826,     0,
     757,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   558,     0,     0,   559,     0,     0,     0,     0,     0,
     557,     0,     0,     0,     0,     0,   560,     0,     0,     0,
       0,   558,  1686,     0,   559,     0,  1688,     0,     0,     0,
     486,   561,   562,     0,   563,   564,    53,     0,   565,     0,
    1235,     0,     0,     0,     0,     0,   566,   567,   486,   561,
     562,     0,   563,   564,    53,     0,   565,   568,   558,     0,
     560,   559,     0,     0,   566,   567,     0,     0,     0,     0,
       0,  1236,   595,     0,     0,   568,     0,     0,     0,     0,
     757,   557,     0,     0,     0,  -688,  -688,  -688,     0,     0,
       0,     0,     0,     0,   486,   561,   562,   560,   563,   564,
      53,     0,   565,     0,     0,     0,     0,     0,     0,     0,
     566,   567,     0,  1756,     0,     0,  1757,   757,   486,   561,
     562,   568,   563,   564,    53,     0,   565,     0,     0,   558,
    1766,     0,   559,     0,   566,   567,     0,     0,   486,   561,
     562,     0,   563,   564,    53,   568,   565,     0,     0,     0,
       0,   597,     0,     0,   566,   567,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   568,     0,     0,   560,     0,
     598,     0,   599,   600,   601,   486,   561,  1538,     0,   563,
     564,    53,   595,   565,     0,     0,     0,     0,     0,     0,
       0,   566,   567,   595,     0,  -689,  -689,  -689,     0,     0,
       0,     0,   568,     0,     0,     0,     0,  1330,  -700,  1330,
       0,   602,   603,   604,     0,     0,     0,     0,     0,   605,
     606,   607,     0,     0,     0,     0,     0,     0,     0,   486,
       0,     0,     0,     0,     0,   608,     0,     0,     0,  1833,
       0,     0,  1835,     0,     0,     0,   486,   561,   966,     0,
     563,   967,    53,     0,   565,  1845,   609,  1847,     0,     0,
       0,   597,   566,   567,  1856,     0,     0,     0,     0,     0,
     595,     0,   597,   568,  -700,     0,     0,     0,     0,     0,
     598,   595,   599,   600,   601,   596,     0,     0,     0,     0,
       0,   598,     0,   599,   600,   601,  1576,     0,     0,     0,
       0,   595,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1822,     0,     0,     0,
       0,   602,   603,   604,     0,     0,     0,     0,     0,   605,
     606,   607,   602,   603,   604,     0,     0,     0,     0,   486,
     605,   606,   607,     0,     0,   608,     0,     0,     0,   597,
     486,     0,     0,     0,     0,     0,   608,     0,     0,     0,
     597,     0,     0,     0,     0,     0,   609,     0,   598,     0,
     599,   600,   601,   595,     0,     0,     0,   609,   595,   598,
     597,   599,   600,   601,     0,     0,     0,     0,  1857,     0,
       0,     0,     0,  -691,     0,     0,     0,     0,     0,   598,
       0,   599,   600,   601,     0,     0,     0,     0,     0,   602,
     603,   604,     0,     0,     0,     0,     0,   605,   606,   607,
     602,   603,   604,     0,     0,     0,     0,   486,   605,   606,
     607,     0,     0,   608,     0,     0,     0,     0,   486,     0,
     602,   603,   604,     0,   608,     0,     0,     0,   605,   606,
     607,     0,   597,     0,   609,     0,     0,   597,   486,     0,
       0,     0,     0,     0,   608,   609,     0,     0,     0,     0,
       0,   598,     0,   599,   600,   601,   598,     0,   599,   600,
     601,     0,     0,     0,     0,   609,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   602,   603,   604,     0,     0,   602,   603,   604,
     605,   606,   607,     0,     0,   605,   606,   607,     0,     0,
     486,     0,    96,     0,     0,   486,   608,     0,   184,   439,
       0,   608,     0,     0,     0,   185,     0,    97,   186,     0,
       0,     0,     0,     0,    96,    99,   440,   609,   100,     0,
     184,  1715,   609,     0,   101,     0,   188,   185,     0,    97,
     186,     0,     0,     0,     0,     0,     0,    99,   440,     0,
     100,     0,     0,   102,     0,     0,   101,     0,   188,     0,
       0,     0,     0,     0,     0,     0,     0,   103,     0,     0,
       0,     0,     0,     0,     0,   102,   104,   189,   105,     0,
       0,     0,   106,     0,    30,  -223,     0,     0,     0,   103,
       0,     0,     0,     0,     0,     0,     0,     0,   104,   189,
     105,     0,     0,     0,   106,     0,    30,  -223,     0,     0,
       0,     0,     0,     0,   311,   190,     0,     0,   312,     0,
       0,   191,   192,     0,     0,     0,    96,     0,     0,     0,
       0,     0,   184,  1799,     0,     0,   311,   190,     0,   185,
     312,    97,   186,   191,   192,     0,     0,     0,    96,    99,
     440,     0,   100,     0,   184,   309,     0,     0,   101,     0,
     188,     0,     0,    97,   186,     0,     0,     0,   310,     0,
     230,    99,     0,     0,   100,     0,     0,   102,     0,     0,
     101,     0,   188,     0,     0,     0,     0,     0,     0,    96,
       0,   103,   230,     0,     0,   184,  1636,     0,     0,   102,
     104,   189,   105,     0,    97,     0,   106,     0,    30,  -223,
       0,     0,    99,   103,     0,   100,     0,     0,     0,     0,
       0,   101,   104,   189,   105,     0,     0,     0,   106,     0,
      30,  -223,     0,     0,     0,     0,     0,     0,   311,   190,
     102,     0,   312,     0,     0,   191,   192,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,     0,     0,     0,
     311,   190,     0,   104,   312,   105,     0,   191,   192,   106,
      96,    30,  -223,     0,     0,     0,   184,     0,     0,     0,
       0,     0,     0,   185,     0,    97,   186,     0,     0,   526,
     187,     0,     0,    99,   230,     0,   100,     0,     0,   527,
     528,   529,   101,     0,   188,     0,     0,   530,     0,   531,
       0,   532,   533,     0,     0,     0,   230,     0,     0,     0,
       0,   102,     0,   534,     0,     0,   535,     0,     0,     0,
       0,     0,     0,    96,     0,   103,   536,   537,     0,   184,
    1719,     0,     0,     0,   104,   189,   105,   538,    97,     0,
     106,     0,    30,  -223,     0,     0,     0,   230,     0,   100,
       0,   539,   540,     0,     0,   101,   541,     0,     0,   542,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   190,   102,     0,     0,     0,     0,   191,
     192,     0,     0,     0,     0,     0,     0,     0,   103,   543,
       0,     0,   544,     0,     0,   545,   546,   104,     0,   105,
       0,     0,     0,   106,     0,    30,  -223
};

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
     182,   388,   182,   257,   146,   182,    10,   662,    42,   951,
     189,     7,   976,   192,   489,   490,     7,   981,    62,     7,
     222,    42,    42,   165,   988,   265,   488,    42,    30,    15,
      30,    58,    30,    70,    72,    77,    26,   179,     4,  1003,
     182,    30,  1006,    35,    10,    98,  1034,  1035,  1036,  1037,
    1038,  1039,    84,    11,   214,   517,   216,    94,   174,    23,
      77,   221,    35,    29,    72,   225,    62,    26,   146,    73,
      59,    62,    26,   189,    62,   400,    68,   174,    59,   768,
     552,   131,    39,   174,   131,    87,   495,  1738,    11,    42,
     146,    27,    79,    26,   481,    68,  1060,   147,    87,   196,
     514,   174,   489,   490,   182,   155,    72,    73,   155,   518,
     713,    92,    70,   292,   293,   294,   295,   296,   297,    96,
      84,    11,   509,    11,   168,   174,   182,   514,    34,   182,
     174,   168,   176,   312,  1794,    34,    94,   174,   552,   631,
      72,    47,   396,   168,   696,     0,    69,    70,    47,   174,
      18,   176,   312,   195,   568,   409,    96,   411,   195,   413,
     400,   195,   200,    88,   168,   197,   174,   554,   195,   189,
     174,   514,   426,    72,   195,   190,   416,   194,   174,   419,
      70,   435,    70,   174,   174,   387,   174,   189,   174,   189,
     182,   189,   371,  1844,   354,   609,   356,   189,  1858,   359,
     189,   890,   189,    26,   137,   197,  1421,   367,   128,   690,
     168,   696,  1015,   194,   696,   174,   174,   174,    30,  1879,
     174,  1456,  1182,   156,  1222,   568,   386,   204,   388,   195,
     390,   703,  1410,  1893,   127,    72,   189,   195,   174,  1890,
     725,   174,   175,   725,   631,   168,   179,   518,   116,   734,
      92,   174,   734,  1267,  1234,   195,  1907,   846,   191,    49,
     915,   916,   195,   195,   197,  1916,   609,   742,   200,    98,
      96,   146,   197,  1089,   194,   174,    96,   146,   168,   741,
     168,    30,   696,   192,   174,    75,   174,    72,   194,   703,
     196,   128,  1234,    92,    72,   194,   195,   196,    96,  1263,
     199,   200,  1266,    96,   197,  1269,  1270,   182,  1272,   696,
     489,   134,  1080,   182,   174,  1774,    92,   477,    92,  1778,
      11,   481,    26,    21,  1522,  1313,  1314,  1315,    72,   489,
     490,    29,   173,   156,   659,    26,     3,   174,   725,   664,
     665,   648,   189,    92,   185,    72,   506,   734,   508,   509,
     510,   511,   194,   182,   514,   742,   168,  1522,   195,   746,
     747,  1559,   174,   200,   187,    77,   189,   190,   777,   861,
     862,   197,  1831,  1608,   866,   867,   868,   197,    69,    70,
     128,   873,    96,    81,    51,   129,   130,    54,    92,   174,
    1605,    89,   552,    60,   554,   194,   174,   195,   558,   137,
     198,   191,   192,   130,   197,   814,   585,   661,   568,  1479,
     195,  1030,   821,    96,   199,   200,  1875,   195,   194,   168,
     194,   835,   200,    90,   122,   174,  1299,   146,   126,  1381,
     174,   818,   909,   673,  1319,    16,   676,   175,    96,   920,
     921,   926,   190,   110,   926,    72,   194,   174,    26,   609,
    1376,   195,    72,   189,    90,    98,   168,   195,    39,    23,
      41,    98,   174,   182,    45,    96,   951,    96,    72,   951,
    1071,   631,  1071,   187,   861,   862,   174,   168,   174,   866,
     867,   868,    63,   174,    92,    49,   873,  1685,   174,    84,
    1488,  1451,   189,     0,   115,   146,   883,    96,    16,   196,
     196,   168,   169,   170,   187,   172,   173,   174,   194,   176,
     130,    75,   195,   998,    92,    92,   998,   184,   185,  1483,
      84,    39,    96,    41,    96,   129,   130,    45,   195,   187,
      35,   182,   168,    97,    96,  1733,   696,   951,   174,   926,
    1032,    72,  1599,   703,  1601,    63,   189,   174,   708,   709,
     710,   711,   189,   713,   174,    96,   187,   388,   187,   195,
     720,   970,   704,    68,   951,   725,   174,   194,   195,   196,
     174,    96,   199,   200,   734,   195,   197,   964,   189,   739,
    1396,   189,   742,  1798,   909,  1749,   746,   747,   187,  1061,
     769,   195,    23,   174,   189,   138,   128,   174,   129,   130,
     195,   859,   197,   168,   199,   200,    35,    17,   995,   174,
    1808,   998,   189,   187,    39,   187,   174,  1089,    98,  1091,
     184,   185,   186,  1091,  1011,   187,   134,   191,   192,   789,
     195,   174,   190,   197,   174,   199,   179,   755,    63,    68,
     120,   174,  1056,   174,   124,  1032,   187,  1061,   134,   809,
     168,   191,   906,    84,   814,   815,   174,  1071,   818,    23,
    1074,   821,   187,   195,    74,    96,   134,  1882,   200,   100,
     101,   102,   103,   104,   105,  1089,   134,  1091,   136,  1066,
      51,   189,   190,  1753,  1071,  1883,    36,  1101,   848,  1103,
    1099,    26,   134,  1080,  1108,    30,   174,  1466,   189,   174,
      35,   861,   862,   189,   190,   196,   866,   867,   868,     8,
      35,   189,  1184,   873,    64,    65,    12,  1320,  1321,    90,
      84,   189,   190,   883,   174,   174,     3,  1925,  1071,   187,
     174,   189,   190,    68,    30,    85,   100,   101,   102,   103,
     104,   105,   174,    68,  1801,   189,   196,   189,   190,  1234,
      20,    50,  1234,    88,    53,   174,   187,    27,  1101,    18,
    1103,    57,    61,    88,    39,  1108,   926,    66,   189,    28,
    1184,    30,   174,   138,    51,   196,   195,    54,    37,  1166,
    1435,  1436,  1437,  1438,  1439,  1440,  1173,   189,    63,    42,
      60,   951,    88,    52,   174,  1611,    95,   168,   169,   170,
     631,   172,   173,   174,   964,   176,   174,   192,   168,   174,
     970,   191,  1618,    90,   174,   975,   138,    76,    77,   204,
     980,   201,   982,   190,   195,   985,  1595,   194,   196,   189,
     195,  1245,    91,  1618,    93,   995,   201,    58,   998,    26,
    1227,   174,  1229,  1230,  1231,  1614,   174,  1234,    35,  1618,
     189,  1011,   174,   175,    58,    42,   189,   116,   197,     9,
      26,   189,  1249,  1250,   178,   696,   134,  1281,  1282,    35,
      20,  1363,  1032,   195,  1366,   189,    42,    27,   187,   201,
     194,    68,  1296,  1502,   134,  1064,   195,  1379,   174,  1381,
     174,   168,   169,   170,    44,   172,   173,   174,  1390,   176,
     178,  1061,    68,   134,  1291,   189,  1066,   184,   185,   200,
     189,  1071,   178,  1516,  1466,   194,   194,    67,   195,   187,
    1080,   189,   190,   189,  1396,   137,   138,  1419,  1419,  1089,
     198,    64,    65,  1739,   165,   137,   138,   187,    88,   189,
     190,  1101,   174,  1103,   189,     6,   196,  1334,  1108,   137,
     178,   196,    85,  1445,  1739,  1115,   187,   189,   189,   190,
      21,    76,   174,   175,    79,    26,   194,   179,    29,  1738,
    1739,    32,   174,   175,   187,   174,  1363,    38,   189,  1366,
     174,  1466,  1396,   195,  1466,   198,   174,   175,   174,   201,
     189,   179,  1379,   195,  1381,   189,    57,  1803,   174,   201,
     182,   174,  1237,  1390,   189,   194,  1166,   189,  1470,  1423,
      71,   115,   197,  1173,   203,   197,   847,  1252,  1803,    80,
     168,    82,    42,   127,  1184,    86,   174,    88,    89,  1416,
     861,   862,  1419,   168,  1803,   866,   867,   868,   174,   174,
      42,   187,   873,  1595,  1850,   168,  1701,  1702,  1703,  1704,
     196,   174,  1466,   176,  1468,   136,   137,   174,  1445,   195,
     174,  1664,  1614,   199,   200,  1850,  1618,  1227,   180,  1229,
    1230,  1231,  1564,  1564,  1234,  1844,     3,   189,  1466,  1466,
     190,  1850,   194,   190,   194,  1472,  1473,   194,   137,  1249,
    1250,  1305,   187,   174,   175,   926,  1305,   187,   179,   174,
    1487,   196,   189,   168,   179,  1265,   196,   194,  1522,   174,
     191,   176,   137,   216,   195,   137,   197,   168,   221,   174,
     951,  1890,   174,   174,    51,   174,   175,    54,   174,  1614,
     179,  1291,  1614,  1618,    42,   195,  1618,   174,  1907,  1611,
     200,   190,   191,    31,    32,  1559,   195,  1916,   197,   174,
     175,   195,   174,   175,   179,   199,   200,   179,  1620,  1319,
    1320,  1321,   189,    90,   190,   190,   191,   194,   194,   191,
     195,   189,   197,   195,  1334,   197,   194,  1564,  1432,  1522,
    1340,  1595,    42,   191,   192,  1572,  1738,  1739,   196,  1429,
     178,   179,   180,   181,   182,   183,  1489,  1611,   190,   174,
    1614,  1032,   194,  1363,  1618,   174,  1366,   190,   195,  1623,
     144,   194,   199,   200,   174,   189,  1559,  1489,   195,  1379,
     194,  1381,   199,   200,   191,   192,   174,  1614,  1489,   196,
    1390,  1618,   166,   167,  1448,   194,  1396,   196,  1652,   173,
     174,   168,   169,   170,   144,   172,   173,   174,   190,   176,
     189,  1803,   194,   144,  1739,   194,  1416,  1739,    67,  1419,
    1754,    31,  1754,  1754,   367,  1096,   166,   167,   195,   189,
     190,  1685,   189,   173,   174,   166,   167,   194,   191,   192,
     174,   194,   173,   174,   195,  1445,   190,   390,   199,   200,
     194,  1505,  1844,    24,    25,  1709,  1489,  1489,  1850,    26,
    1489,  1489,  1489,    30,  1489,  1489,  1466,  1489,   174,  1489,
     176,  1805,  1489,  1805,  1805,   191,   192,   174,  1803,  1733,
     196,  1803,   175,   174,  1738,  1739,   179,  1487,    24,    25,
    1744,   190,  1586,   137,  1606,   194,  1828,  1828,  1890,   195,
    1832,  1832,  1685,   199,   200,  1606,   190,  1489,   191,   192,
     194,   195,  1739,   196,   180,  1907,  1516,    26,   191,   192,
    1747,    30,  1522,   196,  1916,  1850,    89,  1754,  1850,   190,
     174,   175,   189,   194,   477,   179,   100,   101,   102,   103,
     104,   105,  1874,  1874,   190,   195,   189,   191,   194,  1803,
    1733,   195,   189,   197,  1808,   137,   191,   192,   194,  1559,
     196,   196,   137,  1234,  1564,   508,   191,   192,   180,  1901,
    1901,   196,  1572,  1606,  1606,  1246,  1803,  1606,  1805,  1606,
     189,  1606,  1606,   190,  1606,   189,  1606,   194,    68,  1606,
    1844,   190,   174,   175,   190,  1595,  1850,   179,   194,   174,
     175,  1828,   190,   195,   179,  1832,   194,   137,   174,   191,
     176,  1611,   195,   195,  1614,   197,   191,   194,  1618,   196,
     195,   501,   197,  1850,  1606,  1808,   194,   507,   196,  1883,
      15,     6,   512,   194,   514,   196,  1890,    12,    13,   170,
     520,   172,   173,   174,   174,   175,    21,  1874,   190,   179,
     191,   192,   194,  1907,    29,   196,   190,    32,   195,  1713,
     194,   191,  1916,    38,  1664,   195,    42,   197,   190,   202,
       3,  1925,   194,    67,  1901,   555,   191,   192,    11,   194,
     174,    14,    57,   174,   174,  1685,    19,    20,   568,   174,
     191,   192,  1363,   194,    27,  1366,    71,    30,   191,   192,
    1883,   194,   174,   174,    37,    80,   189,    82,  1379,  1709,
    1381,    86,    42,    88,    89,   595,   189,   174,    51,  1390,
     195,    54,   191,   192,   174,   194,   184,   185,   186,   609,
     132,   191,   192,  1733,   194,   144,    69,    70,  1738,  1739,
     189,   194,  1925,   196,   146,   174,   148,   176,  1419,   194,
     194,   196,   196,   144,  1754,   157,   194,    90,   196,   149,
     162,    94,   164,   197,   186,   708,   709,   710,   711,   194,
     189,   196,   174,   194,  1445,   196,   194,   720,   196,     3,
     194,   194,   196,   196,   194,   173,   196,    11,   121,   194,
     194,   196,   196,   163,   154,  1466,   739,   174,  1469,   174,
     174,   174,    26,  1803,    26,  1805,   190,     5,  1808,    84,
      42,    42,   128,    37,   195,   195,    56,   189,   189,   174,
     189,   203,   144,   187,   187,   186,   194,    51,  1828,   194,
      54,   175,  1832,   175,    96,   168,   169,   170,   189,   172,
     173,   174,   174,   176,  1844,    69,    70,   195,   189,   189,
    1850,   184,   185,   190,   189,   174,    94,   195,   738,   174,
     189,   189,   195,   133,    58,   135,    90,   174,   199,   174,
      94,   189,   815,   115,  1874,    60,   146,   197,   148,    99,
      58,    58,   174,  1883,   174,   174,    97,   157,     3,   174,
    1890,   161,   162,  1564,   164,   189,    11,   121,   195,   195,
     195,  1901,   195,   144,   174,   848,   190,  1907,   195,   195,
     790,    26,   195,   195,   195,   195,  1916,   189,   798,   144,
     189,   144,    37,   194,   187,  1925,   186,   195,   195,     3,
     144,   190,    88,   194,   189,   174,    51,    11,    17,    54,
      98,   189,   189,  1614,   168,   169,   170,  1618,   172,   173,
     174,   195,   176,   199,    69,    70,   189,    96,   195,   189,
     184,   185,   189,    37,    42,    73,   194,   196,   189,   174,
     189,   195,   189,   189,   189,    90,   189,    51,   196,    94,
      54,   189,   189,   863,   864,   865,    77,    47,   189,   195,
     178,   871,   144,   203,   874,    69,    70,   877,   194,   879,
     880,   182,   882,    11,    42,   117,   121,   182,    48,    48,
     189,    68,   174,   893,   196,   189,    90,   189,    26,   189,
      94,   194,   198,    42,   195,   190,   195,   195,   194,    42,
       8,    61,   975,    95,    58,   128,   189,   980,   127,   982,
      58,   195,   985,   189,   174,   925,   174,   121,   928,   189,
     194,   189,   194,   168,   169,   170,     5,   172,   173,   174,
     196,   176,   196,   196,   196,   194,    42,   194,  1739,   184,
     185,   194,   196,   194,   196,   194,   196,    39,   189,   189,
     195,    83,    90,  1754,    92,   189,   189,   189,   189,    98,
     189,   189,   144,   174,   168,   169,   170,    96,   172,   173,
     174,   109,   176,   111,   112,   113,   175,   178,   175,   175,
     184,   185,   195,   195,   189,   195,   144,    88,   195,   174,
      78,   195,    34,   174,  1004,   117,    30,   195,   189,   127,
     195,   189,  1803,    26,  1805,   198,   189,   196,    73,    99,
     115,    56,   150,   151,   152,   180,     3,   196,   196,     6,
     158,   159,   160,    79,    11,    12,    13,  1828,   189,    65,
     168,  1832,    19,   189,    21,    22,   174,   189,   189,   189,
      92,    92,    29,    30,    31,    32,    92,   189,   189,  1850,
      37,    38,   189,    40,   189,   189,   189,   195,  1068,   189,
     174,  1071,   189,  1073,    51,   189,   189,    54,   194,   189,
      57,   189,     3,  1874,   189,   194,   178,    68,   195,    88,
     196,    39,    69,    70,    71,    88,   189,   117,   106,   189,
     194,  1101,  1102,    80,    81,    82,   189,   196,  1108,    86,
    1901,    88,    89,    90,    19,    58,  1116,    94,   189,   115,
     174,   174,   174,   174,   194,   189,   194,   194,   189,    47,
      51,   189,   144,    54,   173,   175,    92,   182,     3,   189,
     189,   118,   119,  1143,   121,   122,    11,    92,   125,   126,
     117,   189,   189,    30,  1154,    96,    26,   123,     3,  1159,
     189,    26,   189,   127,   174,  1165,   189,  1167,   174,    90,
     174,   173,    37,   170,  1174,   170,    18,   194,    99,    18,
     189,  1181,   189,    34,   189,   127,    51,    30,   189,    54,
      26,   168,   169,   170,   194,   172,   173,   174,   189,   176,
     189,   189,  1265,   189,    69,    70,    51,   184,   185,    54,
      55,   189,   194,   189,   174,   173,    26,    18,   195,    64,
      65,  1221,    34,   174,   189,    90,  1226,     3,   189,    94,
      30,    26,    69,  1233,   170,    11,   170,   189,    37,    83,
      85,   195,    88,   196,    14,    90,    24,   168,   169,   170,
      26,   172,   173,   174,    88,   176,   121,   189,   121,    26,
     189,    37,     3,   184,   185,   110,   189,    30,    70,   173,
     189,    88,   189,   189,   195,    51,   189,  1340,    54,   189,
     194,   187,   144,   174,   173,   194,   189,   189,   189,   189,
     144,   178,   174,    69,    70,     3,   195,  1297,  1298,   196,
     173,   194,   173,   168,   169,   170,   196,   172,   173,   174,
      51,   176,   178,    54,    90,  1082,   500,   146,    94,   184,
     185,    62,   182,   168,   169,   170,  1326,   172,   173,   174,
     195,   176,   489,     3,  1502,  1334,  1012,   146,   724,   184,
     185,  1300,  1259,    51,  1479,   121,    54,  1448,    18,    90,
     195,   182,   311,   188,   570,   892,  1061,   225,   585,   792,
    1850,  1447,  1362,  1799,   851,  1365,  1598,    37,  1166,   110,
    1370,  1719,  1710,  1373,  1374,  1862,  1376,  1738,   481,   481,
     481,    51,    90,  1614,    54,  1810,  1379,  1387,  1388,  1381,
    1373,  1170,   168,   169,   170,  1376,   172,   173,   174,  1456,
     176,  1390,   110,  1363,  1227,     3,  1416,  1386,   184,   185,
    1366,  1071,   478,  1473,  1747,  1049,   840,   571,  1418,   195,
      90,  1099,   845,     3,   558,    30,  1103,   168,   169,   170,
       7,   172,   173,   174,    23,   176,    82,    82,   920,   437,
     673,   426,   659,   184,   185,   419,   421,   659,    -1,   664,
     423,   121,   690,    51,   195,   665,    54,    55,    -1,    -1,
     168,   169,   170,    -1,   172,   173,   174,    -1,   176,     3,
      -1,    51,  1472,  1473,    54,    -1,   184,   185,    -1,    -1,
      60,   189,    -1,    -1,    -1,    -1,    -1,   195,    -1,    -1,
      -1,    -1,    90,     3,    -1,    -1,    -1,    -1,   168,   169,
     170,    -1,   172,   173,   174,    -1,   176,    -1,    -1,    -1,
      90,    -1,   110,     3,   184,   185,    -1,    51,  1518,    -1,
      54,    -1,  1522,    -1,  1524,   195,    -1,    37,    62,    -1,
     110,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,
       3,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,
      -1,    51,  1562,    -1,    54,    -1,  1566,    -1,    -1,    -1,
     168,   169,   170,    -1,   172,   173,   174,    -1,   176,    -1,
      90,    -1,    -1,    -1,    -1,    -1,   184,   185,   168,   169,
     170,    -1,   172,   173,   174,    -1,   176,   195,    51,    -1,
      90,    54,    -1,    -1,   184,   185,    -1,    -1,    -1,    -1,
      -1,   121,    11,    -1,    -1,   195,    -1,    -1,    -1,    -1,
     110,     3,    -1,    -1,    -1,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,   168,   169,   170,    90,   172,   173,
     174,    -1,   176,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     184,   185,    -1,  1653,    -1,    -1,  1656,   110,   168,   169,
     170,   195,   172,   173,   174,    -1,   176,    -1,    -1,    51,
    1670,    -1,    54,    -1,   184,   185,    -1,    -1,   168,   169,
     170,    -1,   172,   173,   174,   195,   176,    -1,    -1,    -1,
      -1,    90,    -1,    -1,   184,   185,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   195,    -1,    -1,    90,    -1,
     109,    -1,   111,   112,   113,   168,   169,   170,    -1,   172,
     173,   174,    11,   176,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   184,   185,    11,    -1,    24,    25,    26,    -1,    -1,
      -1,    -1,   195,    -1,    -1,    -1,    -1,  1747,    26,  1749,
      -1,   150,   151,   152,    -1,    -1,    -1,    -1,    -1,   158,
     159,   160,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   168,
      -1,    -1,    -1,    -1,    -1,   174,    -1,    -1,    -1,  1779,
      -1,    -1,  1782,    -1,    -1,    -1,   168,   169,   170,    -1,
     172,   173,   174,    -1,   176,  1795,   195,  1797,    -1,    -1,
      -1,    90,   184,   185,  1804,    -1,    -1,    -1,    -1,    -1,
      11,    -1,    90,   195,    92,    -1,    -1,    -1,    -1,    -1,
     109,    11,   111,   112,   113,    26,    -1,    -1,    -1,    -1,
      -1,   109,    -1,   111,   112,   113,    26,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,
      -1,   150,   151,   152,    -1,    -1,    -1,    -1,    -1,   158,
     159,   160,   150,   151,   152,    -1,    -1,    -1,    -1,   168,
     158,   159,   160,    -1,    -1,   174,    -1,    -1,    -1,    90,
     168,    -1,    -1,    -1,    -1,    -1,   174,    -1,    -1,    -1,
      90,    -1,    -1,    -1,    -1,    -1,   195,    -1,   109,    -1,
     111,   112,   113,    11,    -1,    -1,    -1,   195,    11,   109,
      90,   111,   112,   113,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,   109,
      -1,   111,   112,   113,    -1,    -1,    -1,    -1,    -1,   150,
     151,   152,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     150,   151,   152,    -1,    -1,    -1,    -1,   168,   158,   159,
     160,    -1,    -1,   174,    -1,    -1,    -1,    -1,   168,    -1,
     150,   151,   152,    -1,   174,    -1,    -1,    -1,   158,   159,
     160,    -1,    90,    -1,   195,    -1,    -1,    90,   168,    -1,
      -1,    -1,    -1,    -1,   174,   195,    -1,    -1,    -1,    -1,
      -1,   109,    -1,   111,   112,   113,   109,    -1,   111,   112,
     113,    -1,    -1,    -1,    -1,   195,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   150,   151,   152,    -1,    -1,   150,   151,   152,
     158,   159,   160,    -1,    -1,   158,   159,   160,    -1,    -1,
     168,    -1,     6,    -1,    -1,   168,   174,    -1,    12,    13,
      -1,   174,    -1,    -1,    -1,    19,    -1,    21,    22,    -1,
      -1,    -1,    -1,    -1,     6,    29,    30,   195,    32,    -1,
      12,    13,   195,    -1,    38,    -1,    40,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    -1,
      32,    -1,    -1,    57,    -1,    -1,    38,    -1,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    57,    80,    81,    82,    -1,
      -1,    -1,    86,    -1,    88,    89,    -1,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,
      82,    -1,    -1,    -1,    86,    -1,    88,    89,    -1,    -1,
      -1,    -1,    -1,    -1,   118,   119,    -1,    -1,   122,    -1,
      -1,   125,   126,    -1,    -1,    -1,     6,    -1,    -1,    -1,
      -1,    -1,    12,    13,    -1,    -1,   118,   119,    -1,    19,
     122,    21,    22,   125,   126,    -1,    -1,    -1,     6,    29,
      30,    -1,    32,    -1,    12,    13,    -1,    -1,    38,    -1,
      40,    -1,    -1,    21,    22,    -1,    -1,    -1,    26,    -1,
     174,    29,    -1,    -1,    32,    -1,    -1,    57,    -1,    -1,
      38,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,     6,
      -1,    71,   174,    -1,    -1,    12,    13,    -1,    -1,    57,
      80,    81,    82,    -1,    21,    -1,    86,    -1,    88,    89,
      -1,    -1,    29,    71,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    38,    80,    81,    82,    -1,    -1,    -1,    86,    -1,
      88,    89,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,
      57,    -1,   122,    -1,    -1,   125,   126,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
     118,   119,    -1,    80,   122,    82,    -1,   125,   126,    86,
       6,    88,    89,    -1,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    19,    -1,    21,    22,    -1,    -1,     9,
      26,    -1,    -1,    29,   174,    -1,    32,    -1,    -1,    19,
      20,    21,    38,    -1,    40,    -1,    -1,    27,    -1,    29,
      -1,    31,    32,    -1,    -1,    -1,   174,    -1,    -1,    -1,
      -1,    57,    -1,    43,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    -1,     6,    -1,    71,    56,    57,    -1,    12,
      13,    -1,    -1,    -1,    80,    81,    82,    67,    21,    -1,
      86,    -1,    88,    89,    -1,    -1,    -1,   174,    -1,    32,
      -1,    81,    82,    -1,    -1,    38,    86,    -1,    -1,    89,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   119,    57,    -1,    -1,    -1,    -1,   125,
     126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,   119,
      -1,    -1,   122,    -1,    -1,   125,   126,    80,    -1,    82,
      -1,    -1,    -1,    86,    -1,    88,    89
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   131,   147,   155,   206,   207,   556,   557,   558,   174,
     174,   191,   201,   560,   561,   562,     0,   208,   209,   210,
     558,   189,   189,   192,   209,     9,    20,    27,    44,    67,
      88,   211,   212,   213,   214,   215,   216,   217,   233,   238,
     250,   256,   257,   335,   559,   174,   562,   174,   174,   174,
     174,    15,   174,   174,   538,   539,   189,   194,    42,   563,
      58,    58,   218,   174,   251,   200,   189,   174,   259,   133,
     135,   146,   148,   157,   161,   162,   164,   174,   564,   565,
     566,   567,   570,   571,   574,   174,   174,    42,   219,    42,
     252,     7,   168,   174,   176,   542,     6,    21,    26,    29,
      32,    38,    57,    71,    80,    82,    86,   226,   229,   260,
     261,   262,   263,   264,   265,   271,   272,   273,   279,   284,
     285,   293,   298,   335,   368,   174,   568,   174,   174,   174,
     174,   174,   174,   174,   174,   132,   571,   574,    42,    42,
     239,    35,    68,   221,   222,   223,   254,   168,   174,   176,
     299,   174,   280,    67,   258,   280,   168,   174,   268,   174,
     174,   174,    42,   189,    31,   269,    89,   180,   189,   194,
     180,   189,   189,   189,   180,   189,   189,   195,   234,   242,
     195,   195,   224,   223,    12,    19,    22,    26,    40,    81,
     119,   125,   126,   226,   229,   255,   261,   262,   271,   277,
     279,   281,   284,   286,   293,   298,   307,   313,   314,   330,
     335,   336,   368,   379,   190,   300,   190,   194,    15,   174,
     189,   190,   195,    42,   190,    42,    42,   189,   268,    13,
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
     174,   189,   371,   497,   195,   174,   369,     4,    10,    29,
      72,    73,   195,   274,   275,   276,   344,   347,   348,   351,
     355,   356,   357,   362,   365,   366,    77,   195,   267,   339,
     190,   138,   569,   144,   189,   144,   197,   584,   175,   179,
     195,   583,   586,   587,   589,   590,   591,   594,   595,   134,
     136,   187,   189,   190,   591,   174,   195,   149,    96,   187,
      96,   187,   195,   186,    26,   156,   190,   575,   189,   173,
      96,   204,   163,    96,   165,   190,   154,   194,   196,    13,
      30,   226,   229,   261,   265,   271,   277,   279,   281,   284,
     286,   287,   293,   298,   307,   313,   314,   321,   330,   334,
     335,   336,   338,   368,   379,   392,   174,   174,   244,   174,
      26,   280,   280,   280,   280,   280,   280,   190,   189,   196,
     196,   231,    27,   174,   220,   331,   168,   174,   280,   288,
     290,   291,   537,   538,   540,   541,   543,   547,   190,   315,
     308,     5,   190,   194,   174,   189,   190,    84,   190,    42,
      42,   190,   128,   289,   195,   289,   348,   511,   538,   537,
      98,   189,   511,   189,   294,   196,     9,    19,    20,    21,
      27,    29,    31,    32,    43,    46,    56,    57,    67,    81,
      82,    86,    89,   119,   122,   125,   126,   227,   320,   195,
     189,   371,   195,   360,    58,   195,   367,     3,    51,    54,
      90,   169,   170,   172,   173,   176,   184,   185,   195,   349,
     526,   527,   528,   530,   532,   533,   534,   536,   537,   538,
     543,   548,   550,   551,   280,   363,   364,   174,   176,   345,
     346,   189,    56,   538,   497,    11,    26,    90,   109,   111,
     112,   113,   150,   151,   152,   158,   159,   160,   174,   195,
     409,   439,   440,   441,   442,   443,   444,   445,   446,   454,
     458,   460,   461,   464,   467,   468,   471,   474,   475,   476,
     479,   480,   482,   484,   486,   488,   490,   491,   492,   493,
     537,   538,   551,   174,   340,   341,   342,   371,   194,   203,
     203,   194,   194,   584,   196,   192,   204,   144,   590,   187,
     198,    96,   195,   198,   187,   186,   584,   584,   584,   136,
     584,   196,   591,   175,   578,   591,   175,   592,   144,   166,
     167,   173,   174,   588,   593,   575,   189,   580,   581,   582,
      96,   175,   179,   174,   573,   189,   237,     7,    62,   174,
     322,   323,   324,   195,   246,    20,   174,   240,   190,   190,
     190,   190,   190,   190,    16,    39,    41,    45,    63,   371,
     510,   498,   189,   189,    26,    69,    70,   232,   393,   396,
     403,   406,   458,   475,   480,   537,   174,   189,    94,   190,
     289,   280,   290,   291,   291,   189,    84,   197,   303,   199,
     511,   199,   511,   174,    58,   208,   306,   110,   518,   522,
     524,   526,   174,   174,   189,   538,   518,   371,    10,    73,
     278,   375,   376,   377,   537,   380,   381,   538,   380,   518,
      60,   372,   374,   512,   513,   516,   518,   526,   537,   538,
     115,   370,   289,   348,   511,   301,   303,   518,    60,   295,
     296,   194,   196,    99,   228,   174,   176,   230,   349,   358,
     359,   361,   371,   373,   538,    58,   537,   518,    58,   533,
     536,   538,   533,   174,   174,   174,    62,   374,   518,   526,
     553,   554,   555,    23,    84,   350,   528,   184,   185,   186,
     529,    49,    75,   191,   192,   531,    97,   199,   190,    26,
     364,   194,   196,   174,   353,   196,   518,    31,    32,   266,
     189,   195,   195,   195,   195,   195,   195,   195,   195,   190,
     518,    98,   182,   195,    18,    28,    30,    37,    52,    76,
      77,    91,    93,   116,   485,    90,   409,   537,   189,   199,
     343,   189,   194,    98,   189,   569,   189,   189,   144,   144,
     198,   144,   194,   196,   587,   589,   190,   584,   588,   195,
     594,   595,   196,   577,   591,   195,   195,   191,   192,   196,
     187,   186,   580,   144,    26,    37,    69,    90,    94,   121,
     382,   383,   386,   393,   403,   404,   406,   407,   408,   409,
     414,   416,   423,   424,   425,   427,   428,   431,   433,   436,
     479,   480,   491,   526,   537,   538,   551,    35,    68,    88,
     325,   327,   328,   329,   190,   194,   170,   173,   245,   373,
     538,   247,   335,   174,   189,    39,   371,   371,   509,   371,
     510,   371,   510,    39,    63,   501,   380,    17,   505,   371,
     220,    70,   458,   475,    42,   195,   387,   398,    69,    70,
     458,   475,   518,   371,    98,   506,   289,   291,   189,   189,
     537,    77,   304,   305,   537,   199,    72,   130,   174,   544,
     544,   189,     7,    62,   168,   174,   176,   316,   317,   318,
     319,   309,   195,   189,     8,    50,    53,    61,    66,    95,
     519,   520,   521,   178,   179,   180,   181,   182,   183,   523,
     100,   101,   102,   103,   104,   105,   525,   189,    17,    74,
     282,   195,   360,   280,   378,   189,   115,   189,   128,   511,
     189,   194,   196,    96,   350,   518,   370,   189,   189,   518,
      42,   189,   320,   189,   194,   196,   196,   359,   538,   194,
     196,    72,   511,   371,   196,   537,   199,   511,   535,   538,
     196,   194,    96,   187,   526,   530,   532,   533,   195,   551,
     371,    73,   346,   189,   352,    77,    76,    79,   459,   189,
     268,   409,   465,   466,   409,   469,   470,   452,   453,   518,
     450,   452,   518,   451,   452,   409,   449,   447,   449,   448,
     449,   518,    55,    64,    65,    85,   411,   472,   473,   518,
     409,   462,   463,   518,    92,   174,   189,   174,   518,    92,
     174,   189,   518,   189,   518,    30,    59,    87,   189,   455,
     456,   457,   518,    30,    92,   494,   495,   496,   537,    47,
     189,    98,   182,   129,   195,   360,   341,   518,   203,   178,
     189,   194,   178,   144,   584,   189,   196,    96,    96,   588,
     588,   144,   166,   167,   173,   174,   144,   166,   167,   173,
     174,   581,   582,   194,     9,   174,   235,   518,   408,   414,
     518,   195,   385,   387,   182,   387,    18,   116,    14,    19,
      20,    27,    30,    37,    69,    90,   121,   405,   408,   414,
     417,   421,   526,   537,   538,   117,   199,    48,    48,    20,
      27,    60,   326,   189,   327,   537,   174,   196,    26,    30,
     243,   248,   249,   371,   506,   194,   505,   371,   506,   371,
     371,   506,   505,   189,   399,   537,   394,    70,   458,   475,
     189,   120,   124,   292,   506,   518,   115,   127,   506,   189,
     537,   194,   305,   198,   544,   195,   195,   195,    42,   190,
     194,   302,   303,    42,   310,   311,   409,   522,   522,   522,
     522,   522,   522,     8,    61,    95,   524,   526,   506,   358,
      58,   190,    26,   280,   537,   518,   128,   513,    60,   517,
     518,   526,   127,   297,   510,   537,   228,   174,   176,   189,
      58,    72,   373,    99,   535,   552,   553,   554,   518,   526,
     555,   518,   189,   174,    26,   174,   354,   538,   518,   174,
     189,   189,   194,   194,   196,   194,   194,   196,   196,   196,
     194,   196,   196,   194,   194,   196,   194,   196,   196,   194,
     196,   194,   189,   518,   472,   189,   194,     5,   194,   196,
     194,   196,    42,   518,   189,   489,    39,    83,   518,   189,
     487,    79,   189,   189,   518,   399,   518,   189,   457,   189,
     455,   457,   189,   537,   518,   189,   194,   494,    96,   481,
     518,   411,   472,    98,   189,   189,   144,   174,   175,   178,
     189,   191,    96,   175,   175,   194,   194,   195,   195,   195,
     195,   144,   174,   189,    88,    78,   518,   387,   222,   223,
     388,    36,   410,   411,   434,   518,   495,   537,   537,   537,
     174,   422,   518,   408,   414,   329,   419,    34,   117,   199,
     526,   129,   195,   195,   537,   537,   329,    30,   174,   322,
     506,   371,   506,   505,   506,   506,   506,   194,   196,   400,
     526,   526,   189,   189,   537,   198,   546,   526,   518,   545,
     518,   320,   319,    26,   312,   222,   223,   196,   522,   522,
     522,   189,   196,   380,   380,    73,   190,   127,   115,   518,
     297,   371,   194,   196,    96,   196,   196,    56,   180,    79,
     518,   466,   189,   518,   470,   189,   189,   189,   170,   518,
     189,   189,   450,   189,   451,   189,   189,   447,   189,   448,
      65,   189,   473,   518,   518,   189,   409,   463,   189,    92,
     483,   189,    92,   373,   481,   189,    92,   174,   189,   189,
     457,   189,    88,   496,   189,   518,    26,   439,   189,   472,
     189,   526,   194,   195,   591,   191,   190,   584,   588,   588,
     588,   588,   588,   588,   178,   426,   409,   196,   388,   223,
     328,   389,   390,   411,   412,   413,   437,    88,   456,   494,
     189,    39,    88,   189,    13,   174,   382,   392,   420,   424,
     526,   129,   289,   117,   514,   515,   516,   517,   514,   189,
      26,   243,   325,   506,   537,   387,    13,   226,   229,   261,
     265,   271,   279,   284,   293,   298,   313,   314,   335,   338,
     368,   401,   194,   194,   196,   196,   194,   196,   189,   302,
      19,   223,   106,   283,    58,   189,   174,   380,   174,   518,
     115,   553,   518,   174,   170,   172,   173,   174,   549,   174,
     194,   194,   194,   554,    26,    92,   518,   477,   518,   189,
     189,   537,    47,   189,   189,   144,   173,   584,   189,   196,
     196,   194,   194,   194,   194,   175,   425,   429,   479,   480,
     182,   390,   327,   189,   327,    13,   392,   189,   472,    13,
     226,   229,   261,   265,   271,   279,   284,   298,   313,   314,
     335,   368,   438,    92,   432,   494,   189,   373,   426,   420,
      26,   382,   424,   289,   117,   189,   526,   194,   196,    96,
     196,    30,    26,   189,   402,   526,   518,   518,   174,   189,
      39,    63,   380,   189,   123,   127,   518,   174,   174,   174,
     189,   170,   170,   173,    96,    18,   554,   189,    24,    25,
     478,   189,    96,   174,   189,   178,   194,   194,   189,   191,
     588,   588,   588,   588,    24,    25,   430,    37,   410,    13,
     189,   223,   189,   391,    92,   402,   554,    26,    92,   189,
     429,    26,    34,   189,   526,   189,   515,   517,   189,    30,
      26,   243,    26,   439,   189,   127,   194,   194,   481,   174,
     189,    96,   481,   518,    26,   518,   189,   174,   173,   191,
     196,   196,   196,   196,   426,   518,    26,   518,   415,   472,
     391,   327,    26,   382,   384,   424,   518,    26,    96,    18,
     554,   430,    34,   174,   418,   189,   189,    30,    26,    69,
     397,   170,   170,   189,   481,    83,    37,   195,   196,    88,
      88,   189,   194,    92,   384,   189,    14,    24,   121,   435,
     426,   174,   189,    96,    26,   418,   189,   189,    30,    70,
     395,   481,   174,   189,   173,   178,   189,   426,   189,   472,
     554,   174,   189,   174,   189,   189,   426,    88,   189,   189,
     174,   189,   189,   194,   144,    92,   189,   189,   174,   189,
     173,   194,   554,   189,   196,   144,   178,   178,   194,   174,
     195,   173,   194,   173,   196,   178
};

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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

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
      yychar = yylex (&yylval);
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
        case 2:
#line 322 "parser.y" /* yacc.c:1646  */
    {
                   constructor();
                 }
#line 3393 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 326 "parser.y" /* yacc.c:1646  */
    {
		   destructor();
		 }
#line 3401 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 329 "parser.y" /* yacc.c:1646  */
    { destructor(); }
#line 3407 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 330 "parser.y" /* yacc.c:1646  */
    { return 1; }
#line 3413 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 333 "parser.y" /* yacc.c:1646  */
    {}
#line 3419 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 334 "parser.y" /* yacc.c:1646  */
    { }
#line 3425 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 337 "parser.y" /* yacc.c:1646  */
    { }
#line 3431 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 346 "parser.y" /* yacc.c:1646  */
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
#line 3447 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 359 "parser.y" /* yacc.c:1646  */
    {}
#line 3453 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 360 "parser.y" /* yacc.c:1646  */
    {}
#line 3459 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 368 "parser.y" /* yacc.c:1646  */
    {}
#line 3465 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 369 "parser.y" /* yacc.c:1646  */
    { }
#line 3471 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 376 "parser.y" /* yacc.c:1646  */
    { }
#line 3477 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 378 "parser.y" /* yacc.c:1646  */
    {}
#line 3483 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 387 "parser.y" /* yacc.c:1646  */
    {  		
		 new_action_table((yyvsp[0].actionptr)->label);
		 new_event_table((yyvsp[0].actionptr)->label);
		 new_rule_table((yyvsp[0].actionptr)->label);
		 new_conflict_table((yyvsp[0].actionptr)->label);
		 delete tel_tb;
		 tel_tb = new tels_table;
		 tel_tb->set_id((yyvsp[0].actionptr)->label);
		 tel_tb->set_type((yyvsp[0].actionptr)->label);
		 table->new_design_unit((yyvsp[0].actionptr)->label);
		 cur_entity = (yyvsp[0].actionptr)->label;
	       }
#line 3500 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 400 "parser.y" /* yacc.c:1646  */
    {
		 if(open_entity_lst->insert((yyvsp[-2].actionptr)->label,*tel_tb)==false)
		   err_msg("error: multiple declaration of entity '",
			   (yyvsp[-2].actionptr)->label, "'");
		 		
		 if((yyvsp[0].C_str) && !strcmp_nocase((yyvsp[-2].actionptr)->label,(yyvsp[0].C_str)))
		   err_msg("warning: '", (yyvsp[0].C_str), "' undeclared as an entity name");
		 // delete tel_tb;
		 //tel_tb=0;
	       }
#line 3515 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 413 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[-1].C_str); }
#line 3521 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 416 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[-1].C_str); }
#line 3527 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 419 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 3533 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 420 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 3539 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 421 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[0].actionptr)->label; }
#line 3545 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 422 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[0].actionptr)->label; }
#line 3551 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 427 "parser.y" /* yacc.c:1646  */
    {declare((yyvsp[0].str_bl_lst));}
#line 3557 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 428 "parser.y" /* yacc.c:1646  */
    {declare((yyvsp[0].str_bl_lst));}
#line 3563 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 436 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst)= (yyvsp[-2].str_bl_lst); }
#line 3569 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 443 "parser.y" /* yacc.c:1646  */
    {}
#line 3575 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 444 "parser.y" /* yacc.c:1646  */
    {}
#line 3581 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 456 "parser.y" /* yacc.c:1646  */
    {}
#line 3587 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 482 "parser.y" /* yacc.c:1646  */
    {}
#line 3593 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 483 "parser.y" /* yacc.c:1646  */
    {}
#line 3599 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 492 "parser.y" /* yacc.c:1646  */
    {}
#line 3605 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 496 "parser.y" /* yacc.c:1646  */
    {
		   signals[cur_entity].clear();
		   delete tel_tb;
		   const tels_table& TT=open_entity_lst->find(Tolower((yyvsp[-1].actionptr)->label));
		   if(TT.empty())
		     err_msg("entity '",(yyvsp[-1].actionptr)->label,"' undeclared");
		   tel_tb = new tels_table(TT);
		   tel_tb->set_type(Tolower((yyvsp[-1].actionptr)->label));

		   if(table->design_unit(Tolower((yyvsp[-1].actionptr)->label)) == false){
		     err_msg("entity '",(yyvsp[-1].actionptr)->label,"' undeclared");
		   }
		   table->new_tb();
                 }
#line 3624 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 512 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-1].C_str) && strcmp_nocase((yyvsp[-1].C_str), (yyvsp[-10].actionptr)->label) == false)
		     err_msg("warning: '", (yyvsp[-10].actionptr)->label,
			     "' undeclared as an architecture name");

		   if(open_entity_lst->insert(Tolower((yyvsp[-8].actionptr)->label),
					      Tolower((yyvsp[-10].actionptr)->label),tel_tb)==2)
		     err_msg("error: multiple declaration of architecture '",
			     (yyvsp[-10].actionptr)->label, "'");
		   table->delete_tb();	
		 }
#line 3640 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 525 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 3646 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 526 "parser.y" /* yacc.c:1646  */
    {   (yyval.C_str) = (yyvsp[0].actionptr)->label; }
#line 3652 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 527 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 3658 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 528 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[0].actionptr)->label; }
#line 3664 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 552 "parser.y" /* yacc.c:1646  */
    {}
#line 3670 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 553 "parser.y" /* yacc.c:1646  */
    {}
#line 3676 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 556 "parser.y" /* yacc.c:1646  */
    {}
#line 3682 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 570 "parser.y" /* yacc.c:1646  */
    { }
#line 3688 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 571 "parser.y" /* yacc.c:1646  */
    {}
#line 3694 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 574 "parser.y" /* yacc.c:1646  */
    {}
#line 3700 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 575 "parser.y" /* yacc.c:1646  */
    {}
#line 3706 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 576 "parser.y" /* yacc.c:1646  */
    {}
#line 3712 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 580 "parser.y" /* yacc.c:1646  */
    {}
#line 3718 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 600 "parser.y" /* yacc.c:1646  */
    { table->new_design_unit((yyvsp[0].actionptr)->label); }
#line 3724 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 602 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[0].C_str) && strcmp_nocase((yyvsp[-2].actionptr)->label, (yyvsp[0].C_str))==false)
		     err_msg("warning: '", (yyvsp[-2].actionptr)->label,
			     "' undeclared as a package label");
		 }
#line 3734 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 609 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 3740 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 611 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[-1].actionptr)->label; }
#line 3746 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 622 "parser.y" /* yacc.c:1646  */
    {}
#line 3752 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 634 "parser.y" /* yacc.c:1646  */
    {}
#line 3758 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 645 "parser.y" /* yacc.c:1646  */
    {
		   /*const char *t = table->find($3->label);
		   if(t == 0 || strcmp_nocase(t, "package"))
		     err_msg("warning: '", $3->label,
			     "' undeclared as a package name");

			     $$ = $3->label;*/
		 }
#line 3771 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 657 "parser.y" /* yacc.c:1646  */
    {}
#line 3777 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 660 "parser.y" /* yacc.c:1646  */
    {
		   /*if(strcmp_nocase($1, $6->label))
		     err_msg("warning: '", $6->label,
		     "' is not declared as a package body name");*/
		 }
#line 3787 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 675 "parser.y" /* yacc.c:1646  */
    {}
#line 3793 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 682 "parser.y" /* yacc.c:1646  */
    {}
#line 3799 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 694 "parser.y" /* yacc.c:1646  */
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
#line 3814 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 707 "parser.y" /* yacc.c:1646  */
    {
		   /*$$ = new TYPES("prd", 0);
		     $$->set_string($2);*/
		 }
#line 3823 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 712 "parser.y" /* yacc.c:1646  */
    {/*
		   $$ = new TYPES("prd", 0);
		   //$$->set_string($2);
		   //$$->set_list($4);
		  */
		 }
#line 3834 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 719 "parser.y" /* yacc.c:1646  */
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
#line 3851 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 733 "parser.y" /* yacc.c:1646  */
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
#line 3869 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 754 "parser.y" /* yacc.c:1646  */
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
#line 3885 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 769 "parser.y" /* yacc.c:1646  */
    {
		   //table->delete_scope();
		 }
#line 3893 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 774 "parser.y" /* yacc.c:1646  */
    {
		   //table->delete_scope();
		 }
#line 3901 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 784 "parser.y" /* yacc.c:1646  */
    {}
#line 3907 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 786 "parser.y" /* yacc.c:1646  */
    {}
#line 3913 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 789 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].actionptr)->label; }
#line 3919 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 790 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].strlit); }
#line 3925 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 797 "parser.y" /* yacc.c:1646  */
    {}
#line 3931 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 808 "parser.y" /* yacc.c:1646  */
    {}
#line 3937 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 818 "parser.y" /* yacc.c:1646  */
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
#line 3964 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 843 "parser.y" /* yacc.c:1646  */
    {
		   /*TYPES *t = new TYPES("typ", $2->label);
		   table->insert($2->label, t);
		   delete t;*/
		 }
#line 3974 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 850 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 3980 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 851 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 3986 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 852 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 3992 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 853 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 3998 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 857 "parser.y" /* yacc.c:1646  */
    {
		   //$$ = $1;
		 }
#line 4006 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 860 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 4012 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 862 "parser.y" /* yacc.c:1646  */
    {
		   /*$$ = new TYPES;
		   $$->set_grp(TYPES::Physical);
		   $$->set_list($1);*/
		 }
#line 4022 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 869 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 4028 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 870 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 4034 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 887 "parser.y" /* yacc.c:1646  */
    {}
#line 4040 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 890 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-5].str_list)->begin(); b!=(yyvsp[-5].str_list)->end();b++){
		     string LL = Tolower(*b);
		     makeconstantdecl(action(LL), (yyvsp[-1].types)->get_int());
		   }
		 }
#line 4051 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 899 "parser.y" /* yacc.c:1646  */
    { (yyval.str_list) = new list<string>;  (yyval.str_list)->push_back((yyvsp[0].actionptr)->label); }
#line 4057 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 901 "parser.y" /* yacc.c:1646  */
    { (yyval.str_list) = (yyvsp[-2].str_list); (yyval.str_list)->push_back((yyvsp[0].actionptr)->label); }
#line 4063 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 906 "parser.y" /* yacc.c:1646  */
    {
		   list<string> *sufficies = new list<string>;
		   int type(IN);
		   string assigned((yyvsp[-2].types)->get_string());
		   delayADT myBounds;
		   int rise_min(0), rise_max(0), fall_min(-1), fall_max(-1);
		   if("init_channel"==assigned){
		     sufficies->push_back(SEND_SUFFIX);
		     sufficies->push_back(RECEIVE_SUFFIX);
		     TYPELIST arguments((yyvsp[-2].types)->get_list());
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
		     if((yyvsp[0].C_str) == string("out")){
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
		   if((yyvsp[-2].types)->get_string() == "'1'"){
		     init = 1;
		   }
		   for(list<string>::iterator b=(yyvsp[-6].str_list)->begin(); b!=(yyvsp[-6].str_list)->end();b++){
		     list<string>::iterator suffix=sufficies->begin();
		     signals[cur_entity][Tolower(*b)] = type;
		     while ( suffix != sufficies->end() ) {
		       string LL = Tolower(*b) + *suffix;
		       makebooldecl(type, action(LL), init, &myBounds);
		       if(!table->insert(LL, make_pair((string)(yyvsp[0].C_str),(TYPES*)0))){
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
#line 4151 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 995 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "out"; }
#line 4157 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 996 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "in"; }
#line 4163 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 997 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "out"; }
#line 4169 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1002 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-3].str_list)->begin(); b!=(yyvsp[-3].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"var",(TYPES*)0))){
		       err_msg("multiple declaration of variable '",*b,"'");
		     }
		   }
		   delete (yyvsp[-3].str_list);
		 }
#line 4182 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1012 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-5].str_list)->begin(); b!=(yyvsp[-5].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"var",(TYPES*)0))){
		       err_msg("multiple declaration of variable '",*b,"'");
		     }
		   }
		   delete (yyvsp[-5].str_list);
		   delete (yyvsp[-1].types);
		 }
#line 4196 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1037 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-4].str_list)->begin(); b!=(yyvsp[-4].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   TERstructADT x;
		   actionADT a = action(*((yyvsp[-4].str_list)->begin()));
		   a->type = CONT + VAR + DUMMY;
		   /* TODO: Support span */
		   a->linitval = atoi((yyvsp[-1].types)->theExpression.c_str());
		   a->uinitval = atoi((yyvsp[-1].types)->theExpression.c_str());
		   x = TERS(a,FALSE,0,0,FALSE);
		   tel_tb->insert(*((yyvsp[-4].str_list)->begin()), x);

		   if (EXPANDED_RATE_NETS==0) {
		     TERstructADT xdot;
		     string dollar2 = (*((yyvsp[-4].str_list)->begin()));
		     actionADT adot = action((dollar2 + "dot").c_str());
		     adot->type = CONT + VAR + DUMMY;
		     adot->linitval = 0;//atoi($5->theExpression.c_str());
		     adot->uinitval = 0;//atoi($5->theExpression.c_str());
		     xdot = TERS(adot,FALSE,0,0,FALSE);
		     tel_tb->insert((dollar2 + "dot").c_str(), xdot);
		   }
		   
		   delete (yyvsp[-4].str_list);
		 }
#line 4229 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1066 "parser.y" /* yacc.c:1646  */
    {
		 /*for(list<string>::iterator b=$2->begin(); b!=$2->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete $2;*/
		 }
#line 4242 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1075 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-2].str_list)->begin(); b!=(yyvsp[-2].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[-2].str_list);
		 }
#line 4255 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1084 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-2].str_list)->begin(); b!=(yyvsp[-2].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[-2].str_list);
		 }
#line 4268 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1093 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-3].str_list)->begin(); b!=(yyvsp[-3].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[-3].str_list);
		 }
#line 4281 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1103 "parser.y" /* yacc.c:1646  */
    {
		   for(list<string>::iterator b=(yyvsp[-4].str_list)->begin(); b!=(yyvsp[-4].str_list)->end();b++){
		     if(!table->insert(*b,make_pair((string)"qty",(TYPES*)0))){
		       err_msg("multiple declaration of quantity '",*b,"'");
		     }
		   }
		   delete (yyvsp[-4].str_list);
		 }
#line 4294 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1113 "parser.y" /* yacc.c:1646  */
    {(yyval.str_list) = (yyvsp[-3].str_list);}
#line 4300 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1121 "parser.y" /* yacc.c:1646  */
    {(yyval.str_list) = (yyvsp[-3].str_list);}
#line 4306 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1137 "parser.y" /* yacc.c:1646  */
    {}
#line 4312 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1139 "parser.y" /* yacc.c:1646  */
    {}
#line 4318 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1151 "parser.y" /* yacc.c:1646  */
    {}
#line 4324 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1155 "parser.y" /* yacc.c:1646  */
    {}
#line 4330 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1159 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].actionptr)->label; }
#line 4336 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1160 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].charlit); }
#line 4342 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1161 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].strlit); }
#line 4348 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1164 "parser.y" /* yacc.c:1646  */
    {}
#line 4354 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1165 "parser.y" /* yacc.c:1646  */
    {}
#line 4360 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1181 "parser.y" /* yacc.c:1646  */
    {}
#line 4366 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1185 "parser.y" /* yacc.c:1646  */
    {}
#line 4372 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1200 "parser.y" /* yacc.c:1646  */
    {
		   parsing_stack.push(make_pair(cur_entity, tel_tb));
		   cur_entity = (yyvsp[0].actionptr)->label;
		   tel_tb = 0;
		   table->push();
		   if(open_entity_lst->find((yyvsp[0].actionptr)->label).empty()){
		     //cout << $2->label<< "   " << (*name_mapping)[$2->label] << endl;
		     if(switch_buffer((*name_mapping)[(yyvsp[0].actionptr)->label] + ".vhd")){
		       new_action_table(cur_entity.c_str());
		       new_event_table(cur_entity.c_str());
		       new_rule_table(cur_entity.c_str());
		       new_conflict_table(cur_entity.c_str());
		     }
		   }
		 }
#line 4392 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1216 "parser.y" /* yacc.c:1646  */
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
#line 4407 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1227 "parser.y" /* yacc.c:1646  */
    {
		   if(open_entity_lst->find(Tolower((yyvsp[-5].actionptr)->label)).empty())
		     err_msg("error: component ",(yyvsp[-5].actionptr)->label," not found");
		
		   if((yyvsp[0].C_str) && strcmp_nocase((yyvsp[-5].actionptr)->label, (yyvsp[0].C_str))==false)
		     err_msg("warning: '", (yyvsp[0].C_str),
			     "' not declared as a component name");
		   if ((yyvsp[-1].str_bl_lst)) delete (yyvsp[-1].str_bl_lst);
		 }
#line 4421 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1238 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0; }
#line 4427 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1239 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0; }
#line 4433 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1241 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst); }
#line 4439 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1243 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst); }
#line 4445 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1251 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 4451 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1253 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[-1].actionptr)->label; }
#line 4457 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1257 "parser.y" /* yacc.c:1646  */
    {
		  yyerror("Attributes not supported.");
		  /*
		  TYPES *t= search( table, $4->label );
		  TYPES *temp= new TYPES( "attribute", t->datatype() );
		  table->add_symbol( $2->label,temp );
		  delete t,temp;
		  */
		}
#line 4471 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1269 "parser.y" /* yacc.c:1646  */
    {
		  yyerror("Attributes not supported.");
		  //Type *t= search( table, $2->label );
		    //delete t;
		  }
#line 4481 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1290 "parser.y" /* yacc.c:1646  */
    {   //TYPES *t= search( table, $1->label );
		     //delete t;
                 }
#line 4489 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1294 "parser.y" /* yacc.c:1646  */
    {}
#line 4495 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1296 "parser.y" /* yacc.c:1646  */
    {}
#line 4501 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1309 "parser.y" /* yacc.c:1646  */
    { }
#line 4507 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1318 "parser.y" /* yacc.c:1646  */
    {   //TYPES *t= search( table, $1->label );
		     //delete t;
		   }
#line 4515 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 1322 "parser.y" /* yacc.c:1646  */
    {   //TYPES *t= search( table, $3->label );
		     //delete t;
		   }
#line 4523 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 1330 "parser.y" /* yacc.c:1646  */
    {}
#line 4529 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 1334 "parser.y" /* yacc.c:1646  */
    {}
#line 4535 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 1336 "parser.y" /* yacc.c:1646  */
    {  /* FILE *temp= NULL;//fopen(strcat($2, ".sim"),"r");
		     if( temp )
		         printf("cannot find the configuration");*/
		   }
#line 4544 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 1343 "parser.y" /* yacc.c:1646  */
    { (yyval.str_str_l)= (yyvsp[-1].str_str_l); }
#line 4550 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 325:
#line 1344 "parser.y" /* yacc.c:1646  */
    { (yyval.str_str_l)= (yyvsp[-1].str_str_l); }
#line 4556 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 1349 "parser.y" /* yacc.c:1646  */
    {(yyval.str_str_l) = (yyvsp[0].str_str_l);}
#line 4562 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 1357 "parser.y" /* yacc.c:1646  */
    {   //TYPES *t= search( table, $3->label );
		     //delete t;
		   }
#line 4570 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 334:
#line 1368 "parser.y" /* yacc.c:1646  */
    {}
#line 4576 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 335:
#line 1370 "parser.y" /* yacc.c:1646  */
    {}
#line 4582 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 337:
#line 1378 "parser.y" /* yacc.c:1646  */
    { delete (yyvsp[-1].str_list); }
#line 4588 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 340:
#line 1388 "parser.y" /* yacc.c:1646  */
    {}
#line 4594 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 344:
#line 1397 "parser.y" /* yacc.c:1646  */
    {}
#line 4600 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 346:
#line 1402 "parser.y" /* yacc.c:1646  */
    {}
#line 4606 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 348:
#line 1410 "parser.y" /* yacc.c:1646  */
    {
		   /*$$ = new TYPES;
		   $$->set_grp(TYPES::Enumeration);
		   $$->set_list($2);
		   delete $2;*/
		 }
#line 4617 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 349:
#line 1419 "parser.y" /* yacc.c:1646  */
    { /*
		   TYPES *ty = new TYPES;
		   $$ = new TYPELIST($1, ty);*/
		 }
#line 4626 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 350:
#line 1424 "parser.y" /* yacc.c:1646  */
    {/*
		   $$ = $1;
		   TYPES *ty = new TYPES;
		   $$->insert($3, ty);*/
		 }
#line 4636 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 351:
#line 1432 "parser.y" /* yacc.c:1646  */
    {  (yyval.C_str) = (yyvsp[0].actionptr)->label; }
#line 4642 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 352:
#line 1434 "parser.y" /* yacc.c:1646  */
    {  (yyval.C_str) = (yyvsp[0].charlit); }
#line 4648 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 353:
#line 1438 "parser.y" /* yacc.c:1646  */
    {}
#line 4654 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 354:
#line 1441 "parser.y" /* yacc.c:1646  */
    { (yyval.typelist) = (yyvsp[0].typelist); }
#line 4660 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 355:
#line 1444 "parser.y" /* yacc.c:1646  */
    { yyerror("Attributes not supported."); }
#line 4666 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 356:
#line 1446 "parser.y" /* yacc.c:1646  */
    {
		   /*if(strcmp_nocase($1->data_type(), $3->data_type()))
		     err_msg("different types used on sides of ", $2);
		   */
		   (yyval.typelist) = new TYPELIST(make_pair(string(), (yyvsp[-2].types)));
		   (yyval.typelist)->insert((yyvsp[-1].C_str), (yyvsp[0].types));
		 }
#line 4678 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 357:
#line 1455 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "'to'"; }
#line 4684 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 358:
#line 1456 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "'downto'"; }
#line 4690 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 359:
#line 1460 "parser.y" /* yacc.c:1646  */
    {
		   //$$ = new TYPELIST(0, $3);
		   //$$->combine($4);
		 }
#line 4699 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 360:
#line 1466 "parser.y" /* yacc.c:1646  */
    {
		   //$$ = new TYPELIST(0, $3);
		   //$$->combine($4);
		 }
#line 4708 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 361:
#line 1472 "parser.y" /* yacc.c:1646  */
    { (yyval.typelist) = 0; }
#line 4714 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 362:
#line 1474 "parser.y" /* yacc.c:1646  */
    {
		   /*if($1)
		     {
		       $$ = $1;
		       $$->insert(0, $2);
		     }
		   else
		   $$ =  new TYPELIST(0, $2); */
		 }
#line 4728 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 363:
#line 1486 "parser.y" /* yacc.c:1646  */
    {
		  //$$ = new TYPES;
		  //$$->set_string($1->label);
		}
#line 4737 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 364:
#line 1493 "parser.y" /* yacc.c:1646  */
    {		
		  //$$ = new TYPES("uk", "uk");
		  //$$->set_string($1->label);
		}
#line 4746 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 365:
#line 1500 "parser.y" /* yacc.c:1646  */
    {
		  //$$ = $1;
		  //$$->set_base($$->data_type());
		  //$$->set_data("vec");
		  //$$->set_grp(TYPES::Array);
		}
#line 4757 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 366:
#line 1507 "parser.y" /* yacc.c:1646  */
    {
		  //$$ = $1;
		  //$$->set_base($$->data_type());
		  //$$->set_data("vec");
		}
#line 4767 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 367:
#line 1515 "parser.y" /* yacc.c:1646  */
    {
		   /*  $$ = $6;
		   $$->set_base($$->data_type());
		   $$->set_list($3);
		   delete $3;*/
		 }
#line 4778 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 368:
#line 1524 "parser.y" /* yacc.c:1646  */
    {
		   /*$$ = $4;
		   $2->combine($$->get_list());
		   $$->set_list($2);
		   $$->set_base($$->data_type());
		   $$->set_grp(TYPES::Array);
		   delete $2;*/
		 }
#line 4791 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 370:
#line 1535 "parser.y" /* yacc.c:1646  */
    {(yyval.typelist) = (yyvsp[-1].typelist); (yyval.typelist)->combine((yyvsp[0].typelist)); }
#line 4797 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 371:
#line 1539 "parser.y" /* yacc.c:1646  */
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
#line 4832 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 372:
#line 1572 "parser.y" /* yacc.c:1646  */
    { (yyval.typelist) = (yyvsp[-1].typelist); }
#line 4838 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 373:
#line 1576 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.typelist) = new TYPELIST(make_pair(string(), (yyvsp[0].types)));
		 }
#line 4846 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 374:
#line 1580 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.typelist) = (yyvsp[-2].typelist);
		   (yyval.typelist)->insert(string(), (yyvsp[0].types));
		 }
#line 4855 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 375:
#line 1587 "parser.y" /* yacc.c:1646  */
    {
		   //$$= new TYPES("uk", "RECORD","RECORD");
		 }
#line 4863 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 376:
#line 1591 "parser.y" /* yacc.c:1646  */
    {
		   //$$= new TYPES("uk", "RECORD","RECORD");
		 }
#line 4871 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 379:
#line 1601 "parser.y" /* yacc.c:1646  */
    {}
#line 4877 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 380:
#line 1604 "parser.y" /* yacc.c:1646  */
    {}
#line 4883 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 381:
#line 1607 "parser.y" /* yacc.c:1646  */
    {}
#line 4889 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 382:
#line 1608 "parser.y" /* yacc.c:1646  */
    {}
#line 4895 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 384:
#line 1615 "parser.y" /* yacc.c:1646  */
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
#line 4913 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 385:
#line 1633 "parser.y" /* yacc.c:1646  */
    {}
#line 4919 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 386:
#line 1635 "parser.y" /* yacc.c:1646  */
    {}
#line 4925 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 387:
#line 1637 "parser.y" /* yacc.c:1646  */
    {}
#line 4931 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 388:
#line 1639 "parser.y" /* yacc.c:1646  */
    {}
#line 4937 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 389:
#line 1641 "parser.y" /* yacc.c:1646  */
    {}
#line 4943 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 391:
#line 1652 "parser.y" /* yacc.c:1646  */
    {
		   /*$$ = table->lookup($1->front());
		   if($$ == 0)
		     {
		       err_msg("'", $1->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		     }
		     $$->set_string($1->front().c_str());*/
		 }
#line 4957 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 392:
#line 1662 "parser.y" /* yacc.c:1646  */
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
#line 4974 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 393:
#line 1674 "parser.y" /* yacc.c:1646  */
    {}
#line 4980 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 394:
#line 1678 "parser.y" /* yacc.c:1646  */
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
#line 5000 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 395:
#line 1694 "parser.y" /* yacc.c:1646  */
    {
		   //$$= search( table, $1->label );
		 }
#line 5008 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 396:
#line 1698 "parser.y" /* yacc.c:1646  */
    { /*
		   $$ = table->lookup($2->front());
		   if($$ == 0)
		     {
		       err_msg("'", $2->front(), "' undeclared");
		       $$ = new TYPES("err", "err");
		       }*/
		 }
#line 5021 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 397:
#line 1707 "parser.y" /* yacc.c:1646  */
    {
		   //$$= search( table, $1->label );
		 }
#line 5029 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 398:
#line 1713 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = new TYPES;
		   (yyval.types)->set_list((yyvsp[0].typelist));
		   (yyvsp[0].typelist)->iteration_init();
		   (yyval.types)->set_data((yyvsp[0].typelist)->value().second.data_type());
		 }
#line 5040 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 400:
#line 1723 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.typelist) = new TYPELIST(make_pair(string(), (yyvsp[-2].types)));
		   (yyval.typelist)->front().second.theExpression = (yyvsp[-2].types)->theExpression;
		   (yyval.typelist)->insert((yyvsp[-1].C_str), (yyvsp[0].types));
     		 }
#line 5050 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 401:
#line 1729 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.typelist)= new TYPELIST(make_pair(string(), (yyvsp[0].types)));
		 }
#line 5058 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 403:
#line 1740 "parser.y" /* yacc.c:1646  */
    {}
#line 5064 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 404:
#line 1741 "parser.y" /* yacc.c:1646  */
    {}
#line 5070 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 405:
#line 1747 "parser.y" /* yacc.c:1646  */
    {}
#line 5076 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 409:
#line 1761 "parser.y" /* yacc.c:1646  */
    {}
#line 5082 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 410:
#line 1763 "parser.y" /* yacc.c:1646  */
    {}
#line 5088 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 411:
#line 1764 "parser.y" /* yacc.c:1646  */
    {}
#line 5094 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 412:
#line 1768 "parser.y" /* yacc.c:1646  */
    {}
#line 5100 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 413:
#line 1770 "parser.y" /* yacc.c:1646  */
    {}
#line 5106 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 414:
#line 1779 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5112 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 415:
#line 1780 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5118 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 416:
#line 1781 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5124 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 417:
#line 1782 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5130 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 418:
#line 1783 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5136 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 419:
#line 1784 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5142 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 420:
#line 1785 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5148 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 421:
#line 1786 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5154 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 422:
#line 1791 "parser.y" /* yacc.c:1646  */
    { tel_tb->insert((yyvsp[-6].C_str), (yyvsp[-1].TERSptr)); }
#line 5160 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 423:
#line 1795 "parser.y" /* yacc.c:1646  */
    { tel_tb->insert((yyvsp[-7].C_str), (yyvsp[-1].TERSptr)); }
#line 5166 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 426:
#line 1802 "parser.y" /* yacc.c:1646  */
    {}
#line 5172 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 433:
#line 1815 "parser.y" /* yacc.c:1646  */
    {}
#line 5178 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 434:
#line 1816 "parser.y" /* yacc.c:1646  */
    { }
#line 5184 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 435:
#line 1817 "parser.y" /* yacc.c:1646  */
    { }
#line 5190 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 442:
#line 1829 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr)= TERSempty(); }
#line 5196 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 443:
#line 1831 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr)= TERScompose((yyvsp[-1].TERSptr), (yyvsp[0].TERSptr), "||"); }
#line 5202 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 444:
#line 1833 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr)= TERScompose((yyvsp[-1].TERSptr), (yyvsp[0].TERSptr), "||");}
#line 5208 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 445:
#line 1836 "parser.y" /* yacc.c:1646  */
    {}
#line 5214 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 460:
#line 1851 "parser.y" /* yacc.c:1646  */
    {}
#line 5220 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 469:
#line 1864 "parser.y" /* yacc.c:1646  */
    {
		  table->new_tb();
		  processChannels.clear();
		}
#line 5229 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 470:
#line 1870 "parser.y" /* yacc.c:1646  */
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
		  if((yyvsp[-3].TERSptr)){
		    (yyval.TERSptr) = TERSmakeloop((yyvsp[-3].TERSptr));
		    (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		    char s[255];
		    strcpy(s, rmsuffix((yyvsp[-8].C_str)));
		    if (1) {
		      printf("Compiled process %s\n",s);
		      fprintf(lg,"Compiled process %s\n",s);
		    } 
		    else
		      emitters(outpath, s, (yyval.TERSptr));
		  }
		  if(tel_tb->insert(((yyvsp[-8].C_str)), (yyval.TERSptr)) == false)
		    err_msg("duplicate process label '", (yyvsp[-8].C_str), "'");
		}
#line 5271 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 473:
#line 1914 "parser.y" /* yacc.c:1646  */
    {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   (yyval.C_str) = copy_string(n);
		   //TYPES *t = new TYPES("prc", "uk");
		   //table->new_scope($$, "process", t);
		 }
#line 5284 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 474:
#line 1923 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.C_str) = copy_string((yyvsp[-1].C_str));
		   //TYPES *t = new TYPES("prc", "uk");
		   //table->new_scope($$, "process", t);
		 }
#line 5294 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 475:
#line 1929 "parser.y" /* yacc.c:1646  */
    {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   (yyval.C_str) = copy_string(n);
		 }
#line 5305 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 476:
#line 1936 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = copy_string((yyvsp[-2].C_str)); }
#line 5311 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 477:
#line 1939 "parser.y" /* yacc.c:1646  */
    { }
#line 5317 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 480:
#line 1945 "parser.y" /* yacc.c:1646  */
    {
		   __sensitivity_list__= 1;
		 }
#line 5325 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 481:
#line 1951 "parser.y" /* yacc.c:1646  */
    {
		   //TYPES *t= search( table, $1->getstring());
                   //table->add_symbol( $1->getstring(), t );
                   //delete t;
                 }
#line 5335 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 482:
#line 1957 "parser.y" /* yacc.c:1646  */
    {
		   //TYPES *t= search( table, $3->getstring());
		   //table->add_symbol($3->getstring(), t );
                   //delete t;
		 }
#line 5345 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 485:
#line 1968 "parser.y" /* yacc.c:1646  */
    {}
#line 5351 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 496:
#line 1979 "parser.y" /* yacc.c:1646  */
    {}
#line 5357 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 499:
#line 1984 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5363 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 500:
#line 1986 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[0].ty_tel2)->second)
		     (yyval.TERSptr) = TERScompose((yyvsp[-1].TERSptr), TERSrename((yyvsp[-1].TERSptr),(yyvsp[0].ty_tel2)->second),";");
		   else
		     (yyval.TERSptr) = (yyvsp[-1].TERSptr);
		   delete (yyvsp[0].ty_tel2);
		 }
#line 5375 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 502:
#line 1997 "parser.y" /* yacc.c:1646  */
    {}
#line 5381 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 504:
#line 1999 "parser.y" /* yacc.c:1646  */
    {}
#line 5387 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 505:
#line 2003 "parser.y" /* yacc.c:1646  */
    {
		//SymTab *temp= search1(table,$3->name2->label );
		//delete temp;
		
	      }
#line 5397 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 506:
#line 2009 "parser.y" /* yacc.c:1646  */
    {
		string comp_id = (yyvsp[-2].types)->str();
		if(open_entity_lst->find(comp_id).empty()){
		  err_msg("error: component '",comp_id.c_str(), "' not found");
		  delete (yyvsp[-1].str_str_l);
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
		  if(tmp.size() != (yyvsp[-1].str_str_l)->size())
		    err_msg("incorrect port map of '",(yyvsp[-2].types)->str(),"'");
		  else {
		    const vector<pair<string,int> > sig_v(tmp.begin(),
							  tmp.end());
		    const map<string,int>& sig_m = rpc->signals();
		    map<string,int>& ports = tel_tb->port_decl();
		    list<pair<string,string> >::size_type pos = 0;
		    map<string,string> port_maps;
		    list<pair<string,string> >::iterator BB;
		    for(BB = (yyvsp[-1].str_str_l)->begin(); BB != (yyvsp[-1].str_str_l)->end(); BB++, pos++){
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
		    string s = (yyvsp[-2].types)->str();
		    rpc->set_id((yyvsp[-3].C_str));
		    rpc->set_type(comp_id);
		    rpc->instantiate(netlist, (yyvsp[-3].C_str), s);
		    rpc->insert(port_maps);
		    if(tel_tb->insert(((yyvsp[-3].C_str)), rpc) == false)
		      err_msg("duplicate component label '", (yyvsp[-3].C_str), "'");
		    delete netlist;
		  }		
		  delete (yyvsp[-1].str_str_l);
		}
	      }
#line 5500 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 508:
#line 2110 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 5506 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 509:
#line 2111 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 5512 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 510:
#line 2112 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 5518 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 511:
#line 2116 "parser.y" /* yacc.c:1646  */
    { 
		   char n[100];
		   sprintf(n, "ss%ld", PSC);
		   PSC++;
		   printf("Compiled assertion statement %s\n",n);
		   fprintf(lg,"Compiled assertion statement %s\n",n);
		   TERstructADT x;
		   x = TERSmakeloop((yyvsp[0].TERSptr));
		   x = TERSrepeat(x,";");
		   tel_tb->insert(n, x);
		 }
#line 5534 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 512:
#line 2127 "parser.y" /* yacc.c:1646  */
    { 
		 char n[100];
		 if ((yyvsp[-1].C_str))
		   strcpy(n,(yyvsp[-1].C_str));
		 else {
		   sprintf(n, "ss%ld", PSC);
		   PSC++;
		 }
		 printf("Compiled assertion statement %s\n",n);
		 fprintf(lg,"Compiled assertion statement %s\n",n);
		 TERstructADT x;
		 x = TERSmakeloop((yyvsp[0].TERSptr));
		 x = TERSrepeat(x,";");
		 tel_tb->insert(n, x);
	       }
#line 5554 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 513:
#line 2142 "parser.y" /* yacc.c:1646  */
    { }
#line 5560 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 514:
#line 2143 "parser.y" /* yacc.c:1646  */
    { }
#line 5566 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 515:
#line 2147 "parser.y" /* yacc.c:1646  */
    {
		   char id[200];
		   sprintf(id, "CS%ld", (unsigned long)(yyvsp[0].TERSptr));
		   tel_tb->insert((id), (yyvsp[0].TERSptr));
		 }
#line 5576 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 516:
#line 2153 "parser.y" /* yacc.c:1646  */
    { tel_tb->insert(((yyvsp[-1].C_str)), (yyvsp[0].TERSptr)); }
#line 5582 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 517:
#line 2154 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5588 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 518:
#line 2155 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5594 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 519:
#line 2156 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5600 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 520:
#line 2158 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5606 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 521:
#line 2159 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5612 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 522:
#line 2160 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 5618 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 523:
#line 2164 "parser.y" /* yacc.c:1646  */
    {
		   string ENC, EN_r, EN_f;
		   int LL = INFIN, UU = 0;
		   list<pair<TYPES,TYPES> >::iterator b;
		   for(b = (yyvsp[-1].ty_ty_lst)->begin(); b != (yyvsp[-1].ty_ty_lst)->end(); b++){
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

		   string a_r = (yyvsp[-4].types)->get_string() + '+';
		   string a_f = (yyvsp[-4].types)->get_string() + '-';
		   actionADT a = action((yyvsp[-4].types)->str() + '+');
		   telADT RR, FF, RC, FC;
		   string CC = '[' + EN_r + ']' + 'd';
		   RC = TERS(dummyE(), FALSE, 0,0, TRUE, CC.c_str());
		   RR = TERS(action((yyvsp[-4].types)->str()+'+'),FALSE,LL,UU,TRUE);
		   CC = '[' + EN_f + ']' + 'd';
		   FC = TERS(dummyE(), FALSE, 0,0, TRUE, CC.c_str());
		   FF = TERS(action((yyvsp[-4].types)->str()+'-'),FALSE,LL,UU,TRUE);
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
#line 5697 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 533:
#line 2256 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-1].ty_ty_lst)) (yyval.ty_ty_lst) = (yyvsp[-1].ty_ty_lst);
		   else   (yyval.ty_ty_lst) = (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		   TYPES ty("uk","bool");
		   ty.set_grp(TYPES::Enumeration);
		   ty.set_str("true");
		   (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[0].types),ty));
		 }
#line 5710 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 534:
#line 2265 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-3].ty_ty_lst))
		     (yyval.ty_ty_lst) = (yyvsp[-3].ty_ty_lst);
		   else
		     (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		   if((yyvsp[0].types)->grp_id() != TYPES::Error &&
		      (yyvsp[0].types)->data_type() != string("bool")) {
		     err_msg("expect a boolean expression after 'when'");
		     (yyvsp[0].types)->set_grp(TYPES::Error);
		   }
		   (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[-2].types), *(yyvsp[0].types)));
		 }
#line 5727 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 535:
#line 2279 "parser.y" /* yacc.c:1646  */
    { (yyval.ty_ty_lst) = 0; }
#line 5733 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 536:
#line 2281 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-4].ty_ty_lst))  (yyval.ty_ty_lst) = (yyvsp[-4].ty_ty_lst);
		   else    (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		   if((yyvsp[-1].types)->grp_id() != TYPES::Error &&
		      (yyvsp[-1].types)->data_type() != string("bool")){
		     err_msg("expect a boolean expression after 'when'");
		     (yyvsp[-1].types)->set_grp(TYPES::Error);
		   }
		   (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[-3].types), *(yyvsp[-1].types)));
		 }
#line 5748 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 537:
#line 2295 "parser.y" /* yacc.c:1646  */
    {
		   delete (yyvsp[-1].ty_ty_lst);
		   /*telADT ret_t = TERSempty();
		   if($7){		
		     for(list<pair<TYPES*,TYPES*> >::iterator b = $7->begin();
		     b != $7->end(); b++){*/
		
		 }
#line 5761 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 538:
#line 2306 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.ty_ty_lst) = 0;
		   if((yyvsp[0].ty_lst)){
		     if((yyvsp[0].ty_lst)->back().data_type() != "bool")
		       err_msg("expect a boolean expression after 'when'");
		     else{
		       (yyval.ty_ty_lst) = new list<pair<TYPES,TYPES> >;
		       TYPES ty("uk","bool");
		       ty.set_grp(TYPES::Enumeration);
		       list<string> str_l;
		       for(list<TYPES>::iterator b = (yyvsp[0].ty_lst)->begin();
			   b != (yyvsp[0].ty_lst)->end(); b++)
			 str_l.push_back(b->str());
		       ty.set_str(logic(str_l, "|"));
		       (yyval.ty_ty_lst)->push_back(make_pair(*(yyvsp[-2].types),ty));
		     }
		   }
		 }
#line 5784 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 539:
#line 2325 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.ty_ty_lst) = 0;
		   if((yyvsp[-4].ty_ty_lst) && (yyvsp[0].ty_lst)){
		     if((yyvsp[0].ty_lst)->back().data_type() != "bool"){
		       err_msg("expect a boolean expression after 'when'");
		       delete (yyvsp[-4].ty_ty_lst); delete (yyvsp[0].ty_lst); (yyvsp[-4].ty_ty_lst) = 0;
		     }
		     else{
		       TYPES ty("uk","bool");
		       ty.set_grp(TYPES::Enumeration);
		       list<string> str_l;
		       for(list<TYPES>::iterator b = (yyvsp[0].ty_lst)->begin();
			   b != (yyvsp[0].ty_lst)->end(); b++)
			 str_l.push_back(b->str());
		       ty.set_str(logic(str_l, "|"));
		       (yyvsp[-4].ty_ty_lst)->push_back(make_pair(*(yyvsp[-2].types),ty));
		     }
		   }
		   (yyval.ty_ty_lst) = (yyvsp[-4].ty_ty_lst);
                 }
#line 5809 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 540:
#line 2352 "parser.y" /* yacc.c:1646  */
    {}
#line 5815 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 541:
#line 2356 "parser.y" /* yacc.c:1646  */
    {}
#line 5821 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 542:
#line 2360 "parser.y" /* yacc.c:1646  */
    {}
#line 5827 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 544:
#line 2365 "parser.y" /* yacc.c:1646  */
    {
		   /*TYPES *t= search( table, $1->label );
		     delete t;
		     SymTab *temp= table->header;
		     delete table;
		     table= temp;*/
		 }
#line 5839 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 547:
#line 2381 "parser.y" /* yacc.c:1646  */
    {}
#line 5845 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 548:
#line 2383 "parser.y" /* yacc.c:1646  */
    {}
#line 5851 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 552:
#line 2390 "parser.y" /* yacc.c:1646  */
    { /*
		   if( !($2.type->isBool() || $2.type->isError()) )
		     err_msg_3( "the 'if' should be followed a boolean",
				"expression", "");*/
		 }
#line 5861 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 553:
#line 2398 "parser.y" /* yacc.c:1646  */
    {}
#line 5867 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 555:
#line 2405 "parser.y" /* yacc.c:1646  */
    {
                   TERstructADT breakElem = 0;
                   if ((yyvsp[-2].str)) {
		     breakElem = Guard(*(yyvsp[-2].str));
		   }
		   //An assign statement
		   if ((yyvsp[-1].TERSptr)) {
		     char n[100];
		     if ((yyvsp[-4].C_str))
		       strcpy(n,(yyvsp[-4].C_str));
		     else {
		       sprintf(n, "ss%ld", PSC);
		       PSC++;
		     }
		     string condition = (yyvsp[-1].TERSptr)->O->event->exp;
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
		     (yyvsp[-1].TERSptr)->O->event->exp = CopyString((condition+
					  " & "+ action).c_str());
		     
		     
		     TERstructADT x;
		     //x = TERScompose($4, breakElem, ";");
		     x = TERSmakeloop((yyvsp[-1].TERSptr));
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
#line 5955 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 556:
#line 2494 "parser.y" /* yacc.c:1646  */
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
		   
	   if ((yyvsp[0].simul_lst)) {
	     if ((yyvsp[0].simul_lst)->size() == 1) {
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

	       (yyval.TERSptr) = Guard(*((yyvsp[0].simul_lst)->begin()->second.begin()));
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

		       
	       for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
		    elem != (yyvsp[0].simul_lst)->end(); elem++) {
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
		   for (simul_lst::iterator elem2 = (yyvsp[0].simul_lst)->begin();
			elem2 != (yyvsp[0].simul_lst)->end(); elem2++) {
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
		   for (unsigned int i = 1; i <= (yyvsp[0].simul_lst)->size(); i++) {
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

	       for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
		    elem != (yyvsp[0].simul_lst)->end(); elem++) {
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
#line 6257 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 557:
#line 2795 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = (yyvsp[0].simul_lst);
		 }
#line 6265 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 558:
#line 2799 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = (yyvsp[0].simul_lst);
		 }
#line 6273 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 559:
#line 2803 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = (yyvsp[0].simul_lst);
		   //TYPES ty; ty.set_str("case");
		   //$$ = new pair<TYPES,telADT>(ty, $1);
		 }
#line 6283 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 560:
#line 2809 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = 0;
		   printf("Procedures are not supported\n");
		   //TYPES ty; ty.set_str("proc");
		   //$$ = new pair<TYPES,telADT>(ty, 0);
		 }
#line 6294 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 561:
#line 2816 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = 0;
		   //TYPES ty; ty.set_str("nil");
		   //$$ = new pair<TYPES,telADT>(ty, 0);
		 }
#line 6304 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 562:
#line 2823 "parser.y" /* yacc.c:1646  */
    { (yyval.simul_lst) = 0; }
#line 6310 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 563:
#line 2825 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = new simul_lst;
		   bool simple=true;
		   if ((yyvsp[-1].simul_lst)) {
		     for (simul_lst::iterator elem = (yyvsp[-1].simul_lst)->begin();
			  elem != (yyvsp[-1].simul_lst)->end(); elem++) {
		       if (elem->first != "true") {
			 simple=false;
			 break;
		       }
		     }
		   }
		   if (simple) {
		     if ((yyvsp[0].simul_lst)) {
		       for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
			    elem != (yyvsp[0].simul_lst)->end(); elem++) {
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
		     if ((yyvsp[-1].simul_lst)) {
		       for (simul_lst::iterator elem = (yyvsp[-1].simul_lst)->begin();
			    elem != (yyvsp[-1].simul_lst)->end(); elem++) {
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
		     if ((yyvsp[0].simul_lst)) {
		       for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
			    elem != (yyvsp[0].simul_lst)->end(); elem++) {
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
		     if ((yyvsp[-1].simul_lst))
		       (yyval.simul_lst) = (yyvsp[-1].simul_lst);
		     for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
			  elem != (yyvsp[0].simul_lst)->end(); elem++) {
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
#line 6395 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 564:
#line 2910 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   expressions.push_back((yyvsp[-4].types)->theExpression + ":=" +
					 (yyvsp[-2].types)->theExpression);
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));

		 }
#line 6408 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 565:
#line 2920 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   expressions.push_back((yyvsp[-4].types)->theExpression + ":=" +
					 (yyvsp[-2].types)->theExpression);
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));
		 }
#line 6420 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 566:
#line 2928 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   if (EXPANDED_RATE_NETS==0) {
		     expressions.push_back("~(" + (*(yyvsp[-5].str_list)->begin()) + "dot>=" +
					   (yyvsp[-1].types)->theExpression + " & " +
					   (*(yyvsp[-5].str_list)->begin()) + "dot<=" +
					   (yyvsp[-1].types)->theExpression + ") & " +
					   *((yyvsp[-5].str_list)->begin()) + "'dot:=" +
					   (yyvsp[-1].types)->theExpression);
		   } else if (EXPANDED_RATE_NETS==1) {
		     expressions.push_back(*((yyvsp[-5].str_list)->begin()) + "'dot:=" +
					   (yyvsp[-1].types)->theExpression);
		   } else {
		     string temp_str;
		     temp_str =(*(yyvsp[-5].str_list)->begin()) + "dot_";
		     for (int i=0;(yyvsp[-1].types)->theExpression[i]!='\0';i++) {
		       if ((yyvsp[-1].types)->theExpression[i]=='-')
			 temp_str += "m";
		       else if ((yyvsp[-1].types)->theExpression[i]==';')
			 temp_str += "_";
		       else if (((yyvsp[-1].types)->theExpression[i]!='{') &&
				((yyvsp[-1].types)->theExpression[i]!='}'))
			 temp_str += (yyvsp[-1].types)->theExpression[i];
		     } 
		     expressions.push_back("~(" + temp_str + ") & " +
					   *((yyvsp[-5].str_list)->begin()) + "'dot:=" +
					   (yyvsp[-1].types)->theExpression + " & " +
					   temp_str + ":=TRUE"); 
		   }
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));
		 }
#line 6457 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 567:
#line 2961 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = new simul_lst;
		   list<string> expressions;
		   if (EXPANDED_RATE_NETS==0) {
		     expressions.push_back("~(" + (*(yyvsp[-5].str_list)->begin()) + "dot>=" +
					   (yyvsp[-1].types)->theExpression + " & " +
					   (*(yyvsp[-5].str_list)->begin()) + "dot<=" +
					   (yyvsp[-1].types)->theExpression + ") & " +
					   *((yyvsp[-5].str_list)->begin()) + "'dot:=" +
					   (yyvsp[-1].types)->theExpression);
		   } else  if (EXPANDED_RATE_NETS==1) {
		     expressions.push_back(*((yyvsp[-5].str_list)->begin()) + "'dot:=" +
					   (yyvsp[-1].types)->theExpression);
		   } else {
		     string temp_str;
		     temp_str =(*(yyvsp[-5].str_list)->begin()) + "dot_";
		     for (int i=0;(yyvsp[-1].types)->theExpression[i]!='\0';i++) {
		       if ((yyvsp[-1].types)->theExpression[i]=='-')
			 temp_str += "m";
		       else if ((yyvsp[-1].types)->theExpression[i]==';')
			 temp_str += "_";
		       else if (((yyvsp[-1].types)->theExpression[i]!='{') &&
				((yyvsp[-1].types)->theExpression[i]!='}'))
			 temp_str += (yyvsp[-1].types)->theExpression[i];
		     } 
		     expressions.push_back("~(" + temp_str + ") & " +
					   *((yyvsp[-5].str_list)->begin()) + "'dot:=" +
					   (yyvsp[-1].types)->theExpression + " & " +
					   temp_str + ":=TRUE");
		   }
		   (yyval.simul_lst)->push_back(make_pair("true",expressions));
		 }
#line 6494 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 568:
#line 2999 "parser.y" /* yacc.c:1646  */
    {
		   string exp = "";
		     if((yyvsp[-8].types)->data_type() != string("bool"))
		       exp = (yyvsp[-8].types)->theExpression;
		     else
		       exp = (yyvsp[-8].types)->str();
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
		   for (simul_lst::iterator elem = (yyvsp[-6].simul_lst)->begin();
			elem != (yyvsp[-6].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		   string notOthers = "";
		   if ((yyvsp[-5].simul_lst)) {
		     string current = "";
		     string last = "";
		     bool first = true;
		     for (simul_lst::iterator elem = (yyvsp[-5].simul_lst)->begin();
			  elem != (yyvsp[-5].simul_lst)->end(); elem++) {
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
		   if ((yyvsp[-4].simul_lst)) {
		     for (simul_lst::iterator elem = (yyvsp[-4].simul_lst)->begin();
			  elem != (yyvsp[-4].simul_lst)->end(); elem++) {
		       (yyval.simul_lst)->push_back(make_pair("~(" + exp +
					       ") & " + notOthers+elem->first,
					       elem->second));
		     }
		   }
		 }
#line 6563 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 569:
#line 3066 "parser.y" /* yacc.c:1646  */
    {
		   string exp = "";
		     if((yyvsp[-7].types)->data_type() != string("bool"))
		       exp = (yyvsp[-7].types)->theExpression;
		     else
		       exp = (yyvsp[-7].types)->str();
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
		   for (simul_lst::iterator elem = (yyvsp[-5].simul_lst)->begin();
			elem != (yyvsp[-5].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		   string notOthers = "";
		   if ((yyvsp[-4].simul_lst)) {
		     string current = "";
		     string last = "";
		     bool first = true;
		     for (simul_lst::iterator elem = (yyvsp[-4].simul_lst)->begin();
			  elem != (yyvsp[-4].simul_lst)->end(); elem++) {
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
		   if ((yyvsp[-3].simul_lst)) {
		     for (simul_lst::iterator elem = (yyvsp[-3].simul_lst)->begin();
			  elem != (yyvsp[-3].simul_lst)->end(); elem++) {
		       (yyval.simul_lst)->push_back(make_pair("~(" + exp +
					       ") & " + notOthers+elem->first,
					       elem->second));
		     }
		   }
		 }
#line 6632 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 570:
#line 3133 "parser.y" /* yacc.c:1646  */
    { (yyval.simul_lst) = 0; }
#line 6638 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 571:
#line 3136 "parser.y" /* yacc.c:1646  */
    {
		   string exp = "";
		     if((yyvsp[-2].types)->data_type() != string("bool"))
		       exp = (yyvsp[-2].types)->theExpression;
		     else
		       exp = (yyvsp[-2].types)->str();
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
		   if ((yyvsp[-4].simul_lst)) {
/* 		     notOthers = "("; */
/* 		     bool first = true; */
/* 		     for (simul_lst::iterator elem = $1->begin(); */
/* 			  elem != $1->end(); elem++) { */
/* 		       if (!first) notOthers += " & "; */
/* 		       notOthers += "~(" + elem->first + ")"; */
/* 		       first = false; */
/* 		     } */
/* 		     notOthers += ") & "; */
		     (yyval.simul_lst) = (yyvsp[-4].simul_lst);
		   }
		  
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
			elem != (yyvsp[0].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(/*notOthers +*/
					     exp /*+ " & " +
						   elem->first*/,
					     elem->second));
		   }
		 }
#line 6681 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 572:
#line 3176 "parser.y" /* yacc.c:1646  */
    { (yyval.simul_lst) = 0; }
#line 6687 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 573:
#line 3178 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = (yyvsp[0].simul_lst);
		 }
#line 6695 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 574:
#line 3187 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = (yyvsp[-4].simul_lst);
		   
		   for (simul_lst::iterator elem = (yyval.simul_lst)->begin();
			elem != (yyval.simul_lst)->end(); elem++) {
		     if (elem->first[0] == '~') {
		       elem->first = "~(" + (yyvsp[-6].types)->theExpression + ")" +
			 elem->first.substr(1,string::npos);
		     }
		     else {
		       elem->first = (yyvsp[-6].types)->theExpression + elem->first;
		     }
		   }
		 }
#line 6714 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 575:
#line 3204 "parser.y" /* yacc.c:1646  */
    {
		   
		   (yyval.simul_lst) = (yyvsp[-3].simul_lst);
		   for (simul_lst::iterator elem = (yyval.simul_lst)->begin();
			elem != (yyval.simul_lst)->end(); elem++) {
		     if (elem->first[0] == '~') {
		       elem->first = "~(" + (yyvsp[-5].types)->theExpression + ")" +
			 elem->first.substr(1,string::npos);
		     }
		     else {
		       elem->first = (yyvsp[-5].types)->theExpression + elem->first;
		     }
		   }
		 }
#line 6733 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 576:
#line 3222 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = new simul_lst;

		   /* These probably aren't going to be as robust
		      as they should be, but work for now */
		   string exp = "";
		   
		   if ((yyvsp[-2].ty_lst)->begin()->str() == "'0'") 
		     exp = "~";
		   		   
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
			elem != (yyvsp[0].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		 }
#line 6755 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 577:
#line 3241 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.simul_lst) = (yyvsp[-4].simul_lst);

		   string exp = "";
		   
		   if ((yyvsp[-2].ty_lst)->begin()->str() == "'0'") 
		     exp = "~";
		   
		   /* add the expression to each in the sequence */
		   for (simul_lst::iterator elem = (yyvsp[0].simul_lst)->begin();
			elem != (yyvsp[0].simul_lst)->end(); elem++) {
		     (yyval.simul_lst)->push_back(make_pair(exp + " & " +
					     elem->first,elem->second));
		   }
		 }
#line 6775 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 578:
#line 3260 "parser.y" /* yacc.c:1646  */
    {
		  table->new_tb();
		  processChannels.clear();
		}
#line 6784 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 579:
#line 3266 "parser.y" /* yacc.c:1646  */
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
		  if((yyvsp[-2].TERSptr)){
		    (yyval.TERSptr) = TERSmakeloop((yyvsp[-2].TERSptr));
		    (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		    char s[255];
		    strcpy(s, rmsuffix((yyvsp[-7].C_str)));
		    if (1) {
		      printf("Compiled process %s\n",s);
		      fprintf(lg,"Compiled process %s\n",s);
		    } 
		    else
		      emitters(outpath, s, (yyval.TERSptr));
		  }
		  if(tel_tb->insert(((yyvsp[-7].C_str)), (yyval.TERSptr)) == false)
		    err_msg("duplicate process label '", (yyvsp[-7].C_str), "'");
		}
#line 6826 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 582:
#line 3312 "parser.y" /* yacc.c:1646  */
    {
		   char n[1000];
		   sprintf(n, "p%ld", PSC);
		   PSC++;
		   (yyval.C_str) = copy_string(n);
		 }
#line 6837 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 583:
#line 3319 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.C_str) = copy_string((yyvsp[-1].C_str));
		 }
#line 6845 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 586:
#line 3331 "parser.y" /* yacc.c:1646  */
    {}
#line 6851 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 587:
#line 3332 "parser.y" /* yacc.c:1646  */
    {}
#line 6857 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 588:
#line 3333 "parser.y" /* yacc.c:1646  */
    {}
#line 6863 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 589:
#line 3334 "parser.y" /* yacc.c:1646  */
    {}
#line 6869 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 590:
#line 3335 "parser.y" /* yacc.c:1646  */
    {}
#line 6875 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 591:
#line 3336 "parser.y" /* yacc.c:1646  */
    {}
#line 6881 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 592:
#line 3337 "parser.y" /* yacc.c:1646  */
    {}
#line 6887 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 593:
#line 3338 "parser.y" /* yacc.c:1646  */
    {}
#line 6893 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 594:
#line 3339 "parser.y" /* yacc.c:1646  */
    {}
#line 6899 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 595:
#line 3340 "parser.y" /* yacc.c:1646  */
    {}
#line 6905 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 596:
#line 3341 "parser.y" /* yacc.c:1646  */
    {}
#line 6911 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 597:
#line 3342 "parser.y" /* yacc.c:1646  */
    {}
#line 6917 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 598:
#line 3350 "parser.y" /* yacc.c:1646  */
    {
		   /*if(__sensitivity_list__)
		     {
		       err_msg("wait statement must not appear",
			       "in a process statement which has",
			       "a sensitivity list");
		       __sensitivity_list__= 0;
		       }*/
		   TYPES ty; ty.set_str("wait");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6933 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 599:
#line 3362 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("assert");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6942 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 600:
#line 3367 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6951 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 601:
#line 3372 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("var");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6960 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 602:
#line 3377 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("proc");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
#line 6969 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 603:
#line 3382 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("if");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6978 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 604:
#line 3387 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("case");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6987 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 605:
#line 3392 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("loop");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 6996 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 606:
#line 3397 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("next");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
#line 7005 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 607:
#line 3402 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("exit");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
#line 7014 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 608:
#line 3407 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("rtn");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
#line 7023 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 609:
#line 3412 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("nil");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
#line 7032 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 610:
#line 3417 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("rep");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, 0);
		 }
#line 7041 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 611:
#line 3422 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 7050 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 612:
#line 3427 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 7059 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 613:
#line 3432 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("sig");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 7068 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 614:
#line 3437 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("wait");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 7077 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 615:
#line 3443 "parser.y" /* yacc.c:1646  */
    {
		   TYPES ty; ty.set_str("break");
		   (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		 }
#line 7086 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 616:
#line 3448 "parser.y" /* yacc.c:1646  */
    {
		    TYPES ty; ty.set_str("rate");
		    (yyval.ty_tel2) = new pair<TYPES,telADT>(ty, (yyvsp[0].TERSptr));
		   }
#line 7095 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 623:
#line 3460 "parser.y" /* yacc.c:1646  */
    {
		       (yyval.TERSptr)=Guard((yyvsp[-2].types)->str());
		    }
#line 7103 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 624:
#line 3464 "parser.y" /* yacc.c:1646  */
    {
		      (yyval.TERSptr) = Guard((yyvsp[-2].types)->theExpression);
		    }
#line 7111 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 625:
#line 3468 "parser.y" /* yacc.c:1646  */
    {
		      (yyval.TERSptr) = Guard((yyvsp[-2].types)->str());
		    }
#line 7119 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 626:
#line 3472 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr)=Guard((yyvsp[-2].types)->str());}
#line 7125 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 627:
#line 3473 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr)=Guard((yyvsp[-2].types)->str());}
#line 7131 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 628:
#line 3474 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr)=Guard((yyvsp[-2].types)->str());}
#line 7137 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 629:
#line 3475 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr)=Guard((yyvsp[-2].types)->str());}
#line 7143 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 630:
#line 3476 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr)=Guard((yyvsp[-2].types)->str());}
#line 7149 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 632:
#line 3480 "parser.y" /* yacc.c:1646  */
    {
		   string s = (yyvsp[-2].types)->str() + '&' + (yyvsp[0].types)->str();
		   (yyval.types)->set_str(s);
		 }
#line 7158 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 634:
#line 3487 "parser.y" /* yacc.c:1646  */
    {
		   string s = (yyvsp[-2].types)->str() + '|' + (yyvsp[0].types)->str();
		   (yyval.types)->set_str(s);
		 }
#line 7167 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 635:
#line 3493 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = new TYPES;
		   (yyval.types)->set_data("bool");
		   (yyval.types)->set_str(bool_relation(Probe((yyvsp[0].types)->str())->str(), "'1'", "="));
		 }
#line 7177 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 637:
#line 3502 "parser.y" /* yacc.c:1646  */
    {
		   string s = (yyvsp[-2].types)->str() + '&' + (yyvsp[0].types)->str();
		   (yyval.types)->set_str(s);
		 }
#line 7186 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 639:
#line 3510 "parser.y" /* yacc.c:1646  */
    {
		   string s = (yyvsp[-2].types)->str() + '|' + (yyvsp[0].types)->str();
		   (yyval.types)->set_str(s);
		 }
#line 7195 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 640:
#line 3517 "parser.y" /* yacc.c:1646  */
    {
		   pair<string,TYPES> TT1 = table->lookup((yyvsp[-2].types)->str());
		   pair<string,TYPES> TT3 = table->lookup((yyvsp[0].types)->str());
		   (yyval.types) = new TYPES;
		   (yyval.types)->set_data("bool");
		   if(table->lookup((yyvsp[-2].types)->get_string()).first == "qty" ||
		      table->lookup((yyvsp[0].types)->get_string()).first == "qty")
		     cout << "WARNING: Use the guard(expr,relop,expr) syntax "
			  << "for quantities.";
		   else if(table->lookup((yyvsp[-2].types)->get_string()).first == "var" ||
			   table->lookup((yyvsp[0].types)->get_string()).first == "var")
		     (yyval.types)->set_str("maybe");
      		   else {
		     (yyval.types)->set_str(bool_relation((yyvsp[-2].types)->str(), (yyvsp[0].types)->str(), "="));
		   }
                 }
#line 7216 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 641:
#line 3537 "parser.y" /* yacc.c:1646  */
    {
	       
	       pair<string,TYPES> TT1 = table->lookup((yyvsp[-4].types)->str());
	       (yyval.types) = new TYPES;
	       (yyval.types)->set_data("expr");
	       if (table->lookup((yyvsp[-4].types)->get_string()).first == "qty") {
		 
		 string relop;
		 switch ((yyvsp[-2].intval)) {
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
		 (yyval.types)->set_str((yyvsp[-4].types)->str() + relop + numToString((int)(yyvsp[0].floatval)));
	       }
	       else
		   (yyval.types)->set_str("maybe");
	     }
#line 7254 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 642:
#line 3573 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 7260 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 643:
#line 3574 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0; }
#line 7266 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 644:
#line 3576 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.TERSptr) = TERS(dummyE(), FALSE, 0, 0, TRUE, (yyvsp[-1].types)->str().c_str());
		 }
#line 7274 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 645:
#line 3579 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = (yyvsp[-1].TERSptr); }
#line 7280 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 646:
#line 3581 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = 0; }
#line 7286 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 647:
#line 3583 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = 0; }
#line 7292 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 648:
#line 3585 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = 0; }
#line 7298 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 649:
#line 3587 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = 0; }
#line 7304 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 650:
#line 3591 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[0].types);
		   string s;
		   if((yyvsp[0].types)->data_type() != string("bool"))
		     s = '[' + (yyval.types)->theExpression + ']';
		     else
		       s = '[' + (yyval.types)->str() + ']';
		   
		   (yyval.types)->set_str(s);
		 }
#line 7319 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 652:
#line 3607 "parser.y" /* yacc.c:1646  */
    {
		   int l = 0,  u = INFIN;
		   const TYPELIST *tl = (yyvsp[0].types)->get_list();
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
#line 7341 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 653:
#line 3627 "parser.y" /* yacc.c:1646  */
    {
		 string expr;

		 if((yyvsp[-2].types)->data_type() != string("bool"))
		   expr = "[~(" + (yyvsp[-2].types)->theExpression + ")]";
		 else
		   expr = "[~(" + (yyvsp[-2].types)->str() + ")]";

		 string LL = "fail+";
		 actionADT a = action(LL);
		 makebooldecl(OUT|ISIGNAL, a, false, NULL);
		 (yyval.TERSptr) = TERS(a, FALSE, 0, 0, FALSE, expr.c_str());
	       }
#line 7359 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 656:
#line 3660 "parser.y" /* yacc.c:1646  */
    {}
#line 7365 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 657:
#line 3662 "parser.y" /* yacc.c:1646  */
    {}
#line 7371 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 658:
#line 3667 "parser.y" /* yacc.c:1646  */
    {
		   reqSuffix = SEND_SUFFIX;
		   ackSuffix = RECEIVE_SUFFIX;
		   join = dummyE();  //Branches of parallel send join here.
		   join->lower = join->upper = 0;
		   direction = SENT;
		 }
#line 7383 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 659:
#line 3675 "parser.y" /* yacc.c:1646  */
    {
		   reqSuffix = RECEIVE_SUFFIX;
		   ackSuffix = SEND_SUFFIX;
		   join = dummyE();  //Branches of parallel receive join here.
		   join->lower = join->upper = 0;
		   direction = RECEIVED;
		 }
#line 7395 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 660:
#line 3685 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = (yyvsp[-2].TERSptr);}
#line 7401 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 661:
#line 3687 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.TERSptr) = FourPhase((yyvsp[-2].types)->get_string());
		 }
#line 7409 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 663:
#line 3695 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), (yyvsp[0].TERSptr), "||");
    		 }
#line 7417 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 664:
#line 3701 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.TERSptr) = FourPhase((yyvsp[-2].types)->get_string(),(yyvsp[0].types)->get_string());
		 }
#line 7425 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 665:
#line 3706 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = (yyvsp[-2].TERSptr); }
#line 7431 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 667:
#line 3711 "parser.y" /* yacc.c:1646  */
    {
		     (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), (yyvsp[0].TERSptr), "||");
    		   }
#line 7439 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 668:
#line 3717 "parser.y" /* yacc.c:1646  */
    {
		     string s;
		     string sb;
		     TERstructADT x,y,z;
		     if((yyvsp[-4].types)->get_string() == "'1'") {
		       actionADT a = action((yyvsp[-6].types)->get_string() + '+');
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[-2].intval), (yyvsp[0].intval), FALSE);
		     } else if((yyvsp[-4].types)->get_string() == "'0'") {
		       actionADT a = action((yyvsp[-6].types)->get_string() + '-');
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[-2].intval), (yyvsp[0].intval), FALSE);
		     } else {
		       actionADT a = dummyE();
		       a->list_assigns = addAssign(a->list_assigns, (yyvsp[-6].types)->get_string(), (yyvsp[-4].types)->get_string());
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[-2].intval), (yyvsp[0].intval), TRUE);
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
#line 7470 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 669:
#line 3748 "parser.y" /* yacc.c:1646  */
    { 
		    if (table->lookup(*((yyvsp[-5].str_list)->begin())).first == "qty") {
		      (yyval.TERSptr) = Guard(*((yyvsp[-5].str_list)->begin()) + "'dot:=" + 
				 (yyvsp[-1].types)->theExpression);
		    }
		    else
		      (yyval.TERSptr) = 0;
		  }
#line 7483 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 670:
#line 3789 "parser.y" /* yacc.c:1646  */
    {
		     (yyval.TERSptr) = (yyvsp[-2].TERSptr);
		   }
#line 7491 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 672:
#line 3795 "parser.y" /* yacc.c:1646  */
    {
		     (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), (yyvsp[0].TERSptr), "||");
    		   }
#line 7499 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 673:
#line 3800 "parser.y" /* yacc.c:1646  */
    {
		     string s;
		     string sb;
		     TERstructADT x,y,z;
		     if((yyvsp[-4].types)->get_string() == "'1'") {
		       s = "[~" + (yyvsp[-6].types)->get_string() + ']';
		       sb = '[' + (yyvsp[-6].types)->get_string() + ']';
		       actionADT a = action((yyvsp[-6].types)->get_string() + '+');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, (yyvsp[-2].intval), (yyvsp[0].intval), FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,(yyvsp[-2].intval),(yyvsp[0].intval),FALSE,sb.c_str());
		       (yyval.TERSptr) = TERScompose(y,z,"|");
		       (yyval.TERSptr) = TERScompose(x,(yyval.TERSptr),";");
		     } else if((yyvsp[-4].types)->get_string() == "'0'") {
		       s = '[' + (yyvsp[-6].types)->get_string() + ']';
		       sb = "[~" + (yyvsp[-6].types)->get_string() + ']';
		       actionADT a = action((yyvsp[-6].types)->get_string() + '-');
		       x = TERS(dummyE(), FALSE, 0, 0, FALSE);
		       y = TERS(a, FALSE, (yyvsp[-2].intval), (yyvsp[0].intval), FALSE, s.c_str());
		       z = TERS(dummyE(),FALSE,(yyvsp[-2].intval),(yyvsp[0].intval),FALSE,sb.c_str());
		       (yyval.TERSptr) = TERScompose(y,z,"|");
		       (yyval.TERSptr) = TERScompose(x,(yyval.TERSptr),";");
		     } else {
		       actionADT a = dummyE();
		       a->list_assigns = addAssign(a->list_assigns, (yyvsp[-6].types)->get_string(), (yyvsp[-4].types)->get_string());
		       (yyval.TERSptr) = TERS(a, FALSE, (yyvsp[-2].intval), (yyvsp[0].intval), TRUE);
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
#line 7542 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 674:
#line 3883 "parser.y" /* yacc.c:1646  */
    {   /*
		       if(check_type($1, $4))
		     err_msg("left and right sides of",
	                          " '<=' have different types", ";");*/
		   (yyvsp[-1].types)->set_str((yyvsp[-1].types)->theExpression);
		   (yyval.TERSptr) = signalAssignment((yyvsp[-4].types),(yyvsp[-1].types));
	         }
#line 7554 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 675:
#line 3891 "parser.y" /* yacc.c:1646  */
    {
		   (yyvsp[-1].types)->set_str((yyvsp[-1].types)->theExpression);
		   (yyval.TERSptr) = signalAssignment((yyvsp[-3].types),(yyvsp[-1].types));
		 }
#line 7563 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 676:
#line 3902 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-1].types)->set_str((yyvsp[-1].types)->theExpression);
		    (yyval.TERSptr) = signalAssignment((yyvsp[-4].types),(yyvsp[-1].types));
		    /*
		    if(check_type($2, $5))
		      err_msg("left and right sides of '<=' have",
			      " different types", ";");
		    $$= TERSempty();*/
	          }
#line 7577 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 677:
#line 3913 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-1].types)->set_str((yyvsp[-1].types)->theExpression);
		    (yyval.TERSptr) = signalAssignment((yyvsp[-3].types),(yyvsp[-1].types));
		    /*
		    if(check_type($2, $4))
		      err_msg("left and right sides of '<=' have",
	                        " different types", ";");
		    $$= TERSempty();  */
                  }
#line 7591 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 678:
#line 3924 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 7597 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 679:
#line 3926 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-2].types);
		   /*
		   if(!check_type($1, $3))
		     $$= $1;
		   else if($1->isUnknown())
		     $$= $3; */
		 }
#line 7610 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 680:
#line 3935 "parser.y" /* yacc.c:1646  */
    {
                   (yyval.types) = new TYPES();
		   (yyval.types)->set_str("unaffected");
		 }
#line 7619 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 681:
#line 3942 "parser.y" /* yacc.c:1646  */
    {   	
                   (yyval.types) = (yyvsp[0].types);
		   TYPES lower; lower.set_int(0);
		   TYPES upper; upper.set_int(0);
		   str_ty_lst ll(1, make_pair(string(), lower));
		   ll.push_back(make_pair(string(), upper));
		   (yyval.types)->set_list(TYPELIST(ll));
                 }
#line 7632 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 682:
#line 3951 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-2].types);
		   if((yyvsp[0].types)->get_list() == 0){
		     TYPES upper; upper.set_int(INFIN);
		     str_ty_lst LL(1, make_pair(string(), *(yyvsp[0].types)));
		     LL.push_back(make_pair(string(), upper));
		     (yyval.types)->set_list(TYPELIST(LL));
		   }
		   else
		     (yyval.types)->set_list((yyvsp[0].types)->get_list());
		   delete (yyvsp[0].types);
		 }
#line 7649 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 683:
#line 3968 "parser.y" /* yacc.c:1646  */
    {
		   /*
		   if(check_type($2, $4))
		     err_msg("left and right sides of ':='",
			     " have different types", ";");*/
		   if (table->lookup((yyvsp[-3].types)->get_string()).first == "qty") {
		     (yyval.TERSptr) = Guard((yyvsp[-3].types)->get_string() + ":=" + (yyvsp[-1].types)->theExpression);
		   }
		   else
		     (yyval.TERSptr) = 0;
		 }
#line 7665 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 684:
#line 3980 "parser.y" /* yacc.c:1646  */
    { /*
		   if(check_type($1, $3))
		     err_msg("left and right sides of ':='",
			     " have different types", ";");*/
		   if (table->lookup((yyvsp[-3].types)->get_string()).first == "qty") {
		     (yyval.TERSptr) = Guard((yyvsp[-3].types)->get_string() + ":=" + (yyvsp[-1].types)->theExpression);
		   }
		   else
		     (yyval.TERSptr) = 0;
		 }
#line 7680 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 685:
#line 3997 "parser.y" /* yacc.c:1646  */
    {}
#line 7686 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 686:
#line 4003 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-5].ty_tel_lst)->size() == 1 && (yyvsp[-5].ty_tel_lst)->front().first.str() == "wait")
		     (yyval.TERSptr) = (yyvsp[-5].ty_tel_lst)->front().second;
		   else {
		     telADT T5 = telcompose(*(yyvsp[-5].ty_tel_lst));
		     string EN;

		     //condition vs. expression
		     if((yyvsp[-7].types)->data_type() != string("bool"))
		       EN = (yyvsp[-7].types)->theExpression;
		     else
		       EN = (yyvsp[-7].types)->str();
		     
		     string ENC = my_not(EN);
		     string CC = '['+ EN +']';
		     actionADT AA = dummyE();
		     (yyval.TERSptr) = TERS(AA, FALSE, 0, 0, TRUE, CC.c_str());
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), T5, ";");
		     if((yyvsp[-4].ty_tel_lst)){
		       ty_tel_lst::iterator BB;
		       for(BB = (yyvsp[-4].ty_tel_lst)->begin(); BB != (yyvsp[-4].ty_tel_lst)->end(); BB++){

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
		     telADT LLL = TERScompose(LL, (yyvsp[-3].TERSptr), ";");
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), LLL), "|");
		   }
		   delete (yyvsp[-5].ty_tel_lst);
		 }
#line 7735 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 687:
#line 4049 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-6].ty_tel_lst)->size() == 1 && (yyvsp[-6].ty_tel_lst)->front().first.str() == "wait")
		     (yyval.TERSptr) = (yyvsp[-6].ty_tel_lst)->front().second;
		   else {
		     telADT T5 = telcompose(*(yyvsp[-6].ty_tel_lst));
		     string EN;

		     //condition vs. expression
		     if((yyvsp[-8].types)->data_type() != string("bool"))
		       EN = (yyvsp[-8].types)->theExpression;
		     else
		       EN = (yyvsp[-8].types)->str();
		     
		     string ENC = my_not(EN);
		     string CC = '['+ EN +']';
		     actionADT AA = dummyE();
		     (yyval.TERSptr) = TERS(AA, FALSE, 0, 0, TRUE, CC.c_str());
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), T5, ";");
		     if((yyvsp[-5].ty_tel_lst)){
		       ty_tel_lst::iterator BB;
		       for(BB = (yyvsp[-5].ty_tel_lst)->begin(); BB != (yyvsp[-5].ty_tel_lst)->end(); BB++){

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
		     telADT LLL = TERScompose(LL, (yyvsp[-4].TERSptr), ";");
		     (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), LLL), "|");
		   }
		   delete (yyvsp[-6].ty_tel_lst);
		   if((yyvsp[-10].C_str) && (yyvsp[-1].actionptr)->label && strcmp_nocase((yyvsp[-10].C_str), (yyvsp[-1].actionptr)->label)==false)
		     err_msg((yyvsp[-1].actionptr)->label, " is not the if statement label.");
		 }
#line 7786 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 688:
#line 4097 "parser.y" /* yacc.c:1646  */
    { (yyval.ty_tel_lst) = 0; }
#line 7792 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 689:
#line 4099 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-4].ty_tel_lst)) (yyval.ty_tel_lst) = (yyvsp[-4].ty_tel_lst);
		   else (yyval.ty_tel_lst) = new ty_tel_lst;
		   (yyval.ty_tel_lst)->push_back(make_pair(*(yyvsp[-2].types), telcompose(*(yyvsp[0].ty_tel_lst))));
		   (yyval.ty_tel_lst)->back().first.theExpression = (yyvsp[-2].types)->theExpression;
		   delete (yyvsp[0].ty_tel_lst); delete (yyvsp[-2].types);
		 }
#line 7804 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 690:
#line 4108 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = 0;}
#line 7810 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 691:
#line 4110 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = telcompose(*(yyvsp[0].ty_tel_lst)); delete (yyvsp[0].ty_tel_lst);}
#line 7816 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 692:
#line 4113 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = 0; }
#line 7822 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 694:
#line 4117 "parser.y" /* yacc.c:1646  */
    {  (yyval.C_str) = (yyvsp[-1].actionptr)->label;	 }
#line 7828 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 695:
#line 4120 "parser.y" /* yacc.c:1646  */
    { (yyval.ty_tel_lst) = 0; }
#line 7834 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 696:
#line 4122 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-1].ty_tel_lst)) (yyval.ty_tel_lst) = (yyvsp[-1].ty_tel_lst);
		   else   (yyval.ty_tel_lst) = new list<pair<TYPES,telADT> >;
		   (yyval.ty_tel_lst)->push_back(*(yyvsp[0].ty_tel2));
		   delete (yyvsp[0].ty_tel2);
                 }
#line 7845 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 697:
#line 4132 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.TERSptr) = 0;
		   ty_tel_lst::iterator b;
		   string SS = table->lookup((yyvsp[-5].types)->get_string()).first;
		   for(b = (yyvsp[-3].ty_tel_lst)->begin(); b != (yyvsp[-3].ty_tel_lst)->end(); b++){
		     telADT tt = 0;
		     if(SS == "var")
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE);
		     else {
		       string ss='['+bool_relation((yyvsp[-5].types)->str(),b->first.str())+']';
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE, ss.c_str());
		     }
		     telADT T = TERScompose(tt,TERSrename(tt,b->second),";");
		     if((yyval.TERSptr))
		       (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), T), "|");
		     else
		       (yyval.TERSptr) = T;
		   }
//		   $$ = TERScompose(TERS(dummyE(),FALSE, 0, 0, TRUE), $$, "|");
		   delete (yyvsp[-3].ty_tel_lst);
		 }
#line 7871 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 698:
#line 4155 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.TERSptr) = 0;
		   ty_tel_lst::iterator b;
		   string SS = table->lookup((yyvsp[-6].types)->get_string()).first;
		   for(b = (yyvsp[-4].ty_tel_lst)->begin(); b != (yyvsp[-4].ty_tel_lst)->end(); b++){
		     telADT tt = 0;
		     if(SS == "var")
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE);
		     else {
		       string ss='['+bool_relation((yyvsp[-6].types)->str(),b->first.str())+']';
		       tt = TERS(dummyE(), FALSE, 0, 0, TRUE, ss.c_str());
		     }
		     telADT T = TERScompose(tt,TERSrename(tt,b->second),";");
		     if((yyval.TERSptr))
		       (yyval.TERSptr) = TERScompose((yyval.TERSptr), TERSrename((yyval.TERSptr), T), "|");
		     else
		       (yyval.TERSptr) = T;
		   }
//		   $$ = TERScompose(TERS(dummyE(),FALSE, 0, 0, TRUE), $$, "|");
		   delete (yyvsp[-4].ty_tel_lst);
		 }
#line 7897 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 699:
#line 4179 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.ty_tel_lst) = new list<pair<TYPES,telADT> >;
		   TYPES ty("uk", "bool");
		   if((yyvsp[-2].ty_lst)){
		     list<string> str_l;
		     for(list<TYPES>::iterator b = (yyvsp[-2].ty_lst)->begin();
			 b != (yyvsp[-2].ty_lst)->end(); b++)
		       str_l.push_back(b->str());
		     ty.set_str(logic(str_l, "|"));
		   }
		   else{
		     ty.set_grp(TYPES::Error);
		     ty.set_str("false");
		   }
		   (yyval.ty_tel_lst)->push_back(make_pair(ty, telcompose(*(yyvsp[0].ty_tel_lst))));
		   delete (yyvsp[-2].ty_lst); delete (yyvsp[0].ty_tel_lst);
		 }
#line 7919 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 700:
#line 4198 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.ty_tel_lst) = (yyvsp[-4].ty_tel_lst);
		   TYPES ty("uk", "bool");
		   if((yyvsp[-2].ty_lst)){
		     list<string> str_l;
		     for(list<TYPES>::iterator b = (yyvsp[-2].ty_lst)->begin();
			 b != (yyvsp[-2].ty_lst)->end(); b++)
		       str_l.push_back(b->str());
		     ty.set_str(logic(str_l, "|"));
		   }
		   else{
		     ty.set_grp(TYPES::Error);
		     ty.set_str("false");
		   }
		   (yyval.ty_tel_lst)->push_back(make_pair(ty, telcompose(*(yyvsp[0].ty_tel_lst))));
		   delete (yyvsp[-2].ty_lst); delete (yyvsp[0].ty_tel_lst);
		 }
#line 7941 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 701:
#line 4219 "parser.y" /* yacc.c:1646  */
    {
		   telADT T4 = telcompose(*(yyvsp[-3].ty_tel_lst)); delete (yyvsp[-3].ty_tel_lst);
		   if((yyvsp[-5].types) == 0)
		     (yyval.TERSptr) = TERSempty();
		   else{
		     actionADT a = dummyE();
		     string ss =  "[" + (yyvsp[-5].types)->str() + "]";
		     TERstructADT t3 = TERS(a, FALSE, 0, 0, TRUE, ss.c_str());
		     TERstructADT loop_body =
		       TERScompose(t3, TERSrename(t3, T4), ";");

		     loop_body = TERSmakeloop(loop_body);
		
		     // create the tel structure for loop exit.
		     string s = '['+ my_not((yyvsp[-5].types)->str()) + ']';
		     TERstructADT loop_exit = TERS(dummyE(), FALSE, 0, 0,
						   TRUE, s.c_str());
		     // make conflict between loop body and loop exit
		     (yyval.TERSptr) = TERScompose(loop_body,TERSrename(loop_body,
							   loop_exit), "|");
		       		
		     (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");
		   }
                 }
#line 7970 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 702:
#line 4245 "parser.y" /* yacc.c:1646  */
    {
		   telADT T4 = telcompose(*(yyvsp[-4].ty_tel_lst)); delete (yyvsp[-4].ty_tel_lst);
		   if((yyvsp[-6].types) == 0)
		     (yyval.TERSptr) = TERSempty();
		   else{
		     string ss =  "[" + (yyvsp[-6].types)->str() + "]";
		     telADT t3=TERS(dummyE(),FALSE,0,0,TRUE, ss.c_str());
		     telADT loop_body = TERScompose(t3,TERSrename(t3, T4),";");
		     loop_body = TERSmakeloop(loop_body);
		
		     // create the tel structure for loop exit.
		     string s = '[' + my_not((yyvsp[-6].types)->str()) + ']';
		     telADT loop_exit= TERS(dummyE(),FALSE,0,0,TRUE,s.c_str());
		     // make conflict between loop body and loop exit
		     (yyval.TERSptr) = TERScompose(loop_body,TERSrename(loop_body,
							   loop_exit), "|");
		       		
		     (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");

		     if((yyvsp[-7].C_str) && strcmp_nocase((yyvsp[-7].C_str), (yyvsp[-1].actionptr)->label)==false)
		       err_msg((yyvsp[-1].actionptr)->label, " is not the loop statement label.");
		   }
                 }
#line 7998 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 703:
#line 4272 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = new TYPES();
		   (yyval.types)->set_str("true");
		   (yyval.types)->set_data("bool");
		 }
#line 8008 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 704:
#line 4278 "parser.y" /* yacc.c:1646  */
    { 	
		   (yyval.types) = (yyvsp[0].types);
		   if(strcmp_nocase((yyvsp[0].types)->data_type(), "bool")==false){
		     warn_msg("non-boolean expression used as condition");
		     (yyval.types)->set_grp(TYPES::Error);
		     (yyval.types)->set_str("maybe");
   		   }
		 }
#line 8021 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 705:
#line 4286 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = 0; }
#line 8027 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 706:
#line 4289 "parser.y" /* yacc.c:1646  */
    {}
#line 8033 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 707:
#line 4290 "parser.y" /* yacc.c:1646  */
    { }
#line 8039 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 708:
#line 4291 "parser.y" /* yacc.c:1646  */
    {  }
#line 8045 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 709:
#line 4292 "parser.y" /* yacc.c:1646  */
    {}
#line 8051 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 710:
#line 4293 "parser.y" /* yacc.c:1646  */
    {}
#line 8057 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 711:
#line 4296 "parser.y" /* yacc.c:1646  */
    {}
#line 8063 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 712:
#line 4297 "parser.y" /* yacc.c:1646  */
    {}
#line 8069 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 713:
#line 4298 "parser.y" /* yacc.c:1646  */
    {}
#line 8075 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 714:
#line 4299 "parser.y" /* yacc.c:1646  */
    {}
#line 8081 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 715:
#line 4300 "parser.y" /* yacc.c:1646  */
    {}
#line 8087 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 716:
#line 4303 "parser.y" /* yacc.c:1646  */
    {}
#line 8093 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 717:
#line 4304 "parser.y" /* yacc.c:1646  */
    {}
#line 8099 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 718:
#line 4307 "parser.y" /* yacc.c:1646  */
    {}
#line 8105 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 719:
#line 4308 "parser.y" /* yacc.c:1646  */
    {}
#line 8111 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 720:
#line 4311 "parser.y" /* yacc.c:1646  */
    {}
#line 8117 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 721:
#line 4312 "parser.y" /* yacc.c:1646  */
    {}
#line 8123 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 722:
#line 4317 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr)=Guard(*(yyvsp[-2].str)); }
#line 8129 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 723:
#line 4318 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr)=0; }
#line 8135 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 724:
#line 4321 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = 0;}
#line 8141 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 725:
#line 4323 "parser.y" /* yacc.c:1646  */
    {(yyval.TERSptr) = Guard((yyvsp[0].types)->theExpression);}
#line 8147 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 726:
#line 4327 "parser.y" /* yacc.c:1646  */
    {(yyval.str) = (yyvsp[0].str);}
#line 8153 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 727:
#line 4329 "parser.y" /* yacc.c:1646  */
    {
		   if ((yyvsp[-2].str) && (yyvsp[0].str)) {
		     (yyval.str) = new string((*(yyvsp[-2].str)) + " & " + (*(yyvsp[0].str)));
		     delete (yyvsp[-2].str);
		     delete (yyvsp[0].str);
		   } else if ((yyvsp[-2].str)) {
		     (yyval.str) = (yyvsp[-2].str);
		   } else {
		     (yyval.str) = (yyvsp[0].str);
		   }
		 }
#line 8169 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 728:
#line 4344 "parser.y" /* yacc.c:1646  */
    {(yyval.str) = 0;}
#line 8175 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 729:
#line 4346 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.str) = new string((yyvsp[-2].types)->theExpression + ":=" + (yyvsp[0].types)->theExpression); 
		 }
#line 8183 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 731:
#line 4357 "parser.y" /* yacc.c:1646  */
    {
		   if((yyvsp[-2].str_bl_lst) && (yyvsp[0].str_bl_lst)){
		     (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst);
		     (yyval.str_bl_lst)->splice((yyval.str_bl_lst)->begin(), *(yyvsp[-2].str_bl_lst));
		     delete (yyvsp[-2].str_bl_lst);
		   }
		   else if((yyvsp[0].str_bl_lst))
		     (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst);
		   else
		     (yyval.str_bl_lst) = (yyvsp[-2].str_bl_lst);
		 }
#line 8199 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 732:
#line 4370 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0; }
#line 8205 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 733:
#line 4371 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst);  }
#line 8211 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 734:
#line 4372 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0;  }
#line 8217 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 735:
#line 4373 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0; }
#line 8223 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 736:
#line 4374 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst)= (yyvsp[0].str_bl_lst);  }
#line 8229 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 737:
#line 4376 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0;}
#line 8235 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 738:
#line 4377 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0;}
#line 8241 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 739:
#line 4381 "parser.y" /* yacc.c:1646  */
    {}
#line 8247 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 740:
#line 4387 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.str_bl_lst) = new list<pair<string, int> >;
		   for(list<string>::iterator b=(yyvsp[-4].str_list)->begin();b!=(yyvsp[-4].str_list)->end(); b++){
		     int init = 0;
		     if((yyvsp[0].types)->get_string() == "'1'")
		       init = 1;
		     string LL = (*b);
		     actionADT a = action(LL);
		     if(strcmp_nocase((yyvsp[-2].C_str), "in")){
		       makebooldecl(IN, a, init, NULL);
		       (yyval.str_bl_lst)->push_back(make_pair(LL, IN));
		     }
		     else{
		       makebooldecl(OUT, a, init, NULL);
		       (yyval.str_bl_lst)->push_back(make_pair(LL, OUT));
		     }
		   }
		 }
#line 8270 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 741:
#line 4408 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "in";}
#line 8276 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 742:
#line 4409 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "out";}
#line 8282 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 743:
#line 4410 "parser.y" /* yacc.c:1646  */
    {}
#line 8288 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 744:
#line 4417 "parser.y" /* yacc.c:1646  */
    {}
#line 8294 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 745:
#line 4419 "parser.y" /* yacc.c:1646  */
    {}
#line 8300 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 746:
#line 4424 "parser.y" /* yacc.c:1646  */
    {
		  //$4->set_obj("in");
		  (yyval.str_bl_lst) = new list<pair<string, int> >;
		  for(list<string>::iterator b=(yyvsp[-4].str_list)->begin();b!=(yyvsp[-4].str_list)->end(); b++){
		    int init = 0;
		    if((yyvsp[0].types)->get_string() == "'1'")
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
#line 8346 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 747:
#line 4467 "parser.y" /* yacc.c:1646  */
    {
		  /*if(strcmp_nocase($4, "in") == 0)
		    $5->set_obj("in");
		  else if(strcmp_nocase($4, "out") == 0)
		    $5->set_obj("out");
		  else
		  $5->set_obj("in");*/
		  (yyval.str_bl_lst) = new list<pair<string, int> >;
		  for(list<string>::iterator b=(yyvsp[-5].str_list)->begin();b!=(yyvsp[-5].str_list)->end(); b++){
		    int init = 0;
		    if((yyvsp[0].types)->get_string() == "'1'")
		      init = 1;
		    string LL = (*b);
		    actionADT a = action(LL);
		    if(strcmp_nocase((yyvsp[-3].C_str), "in")){
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
#line 8426 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 748:
#line 4546 "parser.y" /* yacc.c:1646  */
    {}
#line 8432 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 749:
#line 4549 "parser.y" /* yacc.c:1646  */
    {}
#line 8438 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 752:
#line 4558 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = new TYPES; (yyval.types)->set_str("'0'"); }
#line 8444 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 753:
#line 4560 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 8450 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 754:
#line 4565 "parser.y" /* yacc.c:1646  */
    {
		 //$3->set_obj("in");
		 (yyval.str_bl_lst) = new list<pair<string,int> >;
		 for(list<string>::iterator b=(yyvsp[-4].str_list)->begin(); b!=(yyvsp[-4].str_list)->end(); b++){
		   int init = 0;
		   if((yyvsp[0].types)->str() == "'1'")
		     init = 1;
		   const string LL = (*b);
		   actionADT a = action(LL);
		   makebooldecl(IN, a, init, NULL);
		   (yyval.str_bl_lst)->push_back(make_pair(LL, IN));
		 }
		 delete (yyvsp[-4].str_list);
	       }
#line 8469 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 755:
#line 4581 "parser.y" /* yacc.c:1646  */
    {
		 (yyval.str_bl_lst) = new list<pair<string,int> >;
		 list<string> *sufficies = new list<string>;
		 int type(IN);
		 string assigned((yyvsp[0].types)->get_string());
		 delayADT myBounds;
		 int rise_min(0), rise_max(0), fall_min(-1), fall_max(-1);
		 if("init_channel" == assigned ||
		    "active"  == assigned ||
		    "passive" == assigned ){
		   sufficies->push_back(SEND_SUFFIX);
		   sufficies->push_back(RECEIVE_SUFFIX);
		   TYPELIST arguments((yyvsp[0].types)->get_list());
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
		   if(strcmp_nocase((yyvsp[-3].C_str),"out")){
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
		 if((yyvsp[0].types)->str() == "'1'"){
		   init = 1;
		 }
		 for(list<string>::iterator b=(yyvsp[-5].str_list)->begin(); b!=(yyvsp[-5].str_list)->end(); b++){
		   (yyval.str_bl_lst)->push_back(make_pair((*b), type));
		   list<string>::iterator suffix=sufficies->begin();
		   while ( suffix != sufficies->end() ) {
		     makebooldecl(type,action((*b)+*suffix),init,
				  &myBounds);
		     suffix++;
		   }
		 }
		 delete sufficies;
		 delete (yyvsp[-5].str_list);
	       }
#line 8562 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 756:
#line 4672 "parser.y" /* yacc.c:1646  */
    {}
#line 8568 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 757:
#line 4676 "parser.y" /* yacc.c:1646  */
    {   (yyval.types)= (yyvsp[0].types);   }
#line 8574 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 758:
#line 4678 "parser.y" /* yacc.c:1646  */
    {   (yyval.types)= (yyvsp[0].types);   }
#line 8580 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 759:
#line 4681 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "in"; }
#line 8586 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 760:
#line 4681 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "out"; }
#line 8592 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 761:
#line 4682 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "out"; }
#line 8598 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 762:
#line 4682 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str)= "out"; }
#line 8604 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 763:
#line 4683 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "linkage"; }
#line 8610 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 764:
#line 4687 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.typelist) = (yyvsp[-1].typelist);
      		 }
#line 8618 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 766:
#line 4694 "parser.y" /* yacc.c:1646  */
    {
		   
		   (yyval.typelist) = (yyvsp[-2].typelist);
		   (yyval.typelist)->front().second.theExpression += ";" +
		     (yyvsp[0].typelist)->front().second.theExpression;
		   (yyval.typelist)->combine(*(yyvsp[0].typelist));
		   delete (yyvsp[0].typelist);
		 }
#line 8631 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 767:
#line 4705 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.typelist) = new TYPELIST(make_pair(string(), *(yyvsp[0].types)));
		   (yyval.typelist)->front().second.theExpression = (yyvsp[0].types)->theExpression;
		   delete (yyvsp[0].types);
		 }
#line 8641 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 768:
#line 4710 "parser.y" /* yacc.c:1646  */
    {  (yyval.typelist) = (yyvsp[0].typelist) ;  }
#line 8647 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 769:
#line 4712 "parser.y" /* yacc.c:1646  */
    {
 		   (yyval.typelist) = new TYPELIST(make_pair(*(yyvsp[-2].str)     , *(yyvsp[0].types)));
		   delete (yyvsp[-2].str);  delete (yyvsp[0].types);
		 }
#line 8656 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 770:
#line 4717 "parser.y" /* yacc.c:1646  */
    {
		   //$$= new TYPES("uk","uk", "uk");
		 }
#line 8664 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 771:
#line 4723 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.str_str_l) = new list<pair<string,string> >;
                   (yyval.str_str_l)->push_back(*(yyvsp[0].csp_pair));   delete (yyvsp[0].csp_pair);
		 }
#line 8673 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 772:
#line 4728 "parser.y" /* yacc.c:1646  */
    { (yyval.str_str_l) = (yyvsp[-2].str_str_l);   (yyval.str_str_l)->push_back(*(yyvsp[0].csp_pair)); delete (yyvsp[0].csp_pair); }
#line 8679 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 773:
#line 4732 "parser.y" /* yacc.c:1646  */
    { (yyval.csp_pair) = new pair<string,string>(string(), (yyvsp[0].types)->str()); delete (yyvsp[0].types); }
#line 8685 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 774:
#line 4734 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.csp_pair) = new pair<string,string>(*(yyvsp[-2].str), (yyvsp[0].types)->str());
		   delete (yyvsp[-2].str);  delete (yyvsp[0].types);
		 }
#line 8694 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 775:
#line 4740 "parser.y" /* yacc.c:1646  */
    { (yyval.str) = new string((yyvsp[0].types)->str());  delete (yyvsp[0].types); }
#line 8700 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 777:
#line 4744 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = new TYPES; }
#line 8706 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 778:
#line 4751 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-1].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[-1].types)->str(), (yyvsp[0].types)->str(), "&"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '&' +
		     (yyvsp[0].types)->theExpression;
		   delete (yyvsp[0].types);
		 }
#line 8719 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 779:
#line 4760 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-1].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[-1].types)->str(), (yyvsp[0].types)->str(), "|"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '|' +
		     (yyvsp[0].types)->theExpression;
		   delete (yyvsp[0].types);
		 }
#line 8732 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 780:
#line 4769 "parser.y" /* yacc.c:1646  */
    {
		   string one_bar=("~("+(yyvsp[-1].types)->str())+')';
		   string two_bar=("~("+(yyvsp[0].types)->str())+')';
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic(logic((yyvsp[-1].types)->str(), two_bar, "&"),
		     logic(one_bar, (yyvsp[0].types)->str(), "&"), "|"));
		   delete (yyvsp[0].types);
		   //$$= new TYPES1(check_logic($1, $2, "'xor'" ));
                   //$$->setTers(TERSempty());
                 }
#line 8747 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 781:
#line 4780 "parser.y" /* yacc.c:1646  */
    {
		   (yyvsp[-2].types)->set_str(("~("+(yyvsp[-2].types)->str())+')');
		   (yyvsp[0].types)->set_str(("~("+(yyvsp[0].types)->str())+')');
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[-2].types)->str(), (yyvsp[0].types)->str(), "|"));
		   delete (yyvsp[0].types);
		   //$$= new TYPES1(check_logic($1, $3, "'nand'"));
                   //$$->setTers( TERSempty());
                 }
#line 8761 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 782:
#line 4790 "parser.y" /* yacc.c:1646  */
    {
		   (yyvsp[-2].types)->set_str(("~("+(yyvsp[-2].types)->str())+')');
		   (yyvsp[0].types)->set_str(("~("+(yyvsp[0].types)->str())+')');
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[-2].types)->str(), (yyvsp[0].types)->str(), "&"));
		   delete (yyvsp[0].types);
		   //$$= new TYPES1(check_logic($1, $3, "'nor'"));
                   //$$->setTers( TERSempty());
                 }
#line 8775 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 783:
#line 4800 "parser.y" /* yacc.c:1646  */
    {
		   string one_bar=("~("+(yyvsp[-2].types)->str())+')';
		   string three_bar=("~("+(yyvsp[0].types)->str())+')';
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic(logic((yyvsp[-2].types)->str(), (yyvsp[0].types)->str(), "&"),
		     logic(one_bar, three_bar, "&"), "|"));
		   delete (yyvsp[0].types);
		   /* NOTE: should $1 be deleted also??? */
		   //$$= new TYPES1(check_logic($1, $3, "'xnor'"));
                   //$$->setTers(TERSempty());
                 }
#line 8791 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 784:
#line 4811 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 8797 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 785:
#line 4815 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-2].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[-2].types)->str(), (yyvsp[0].types)->str(), "&"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '&' +
		     (yyvsp[0].types)->theExpression;
		   delete (yyvsp[0].types);
                 }
#line 8810 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 786:
#line 4824 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[0].types);
		 }
#line 8818 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 787:
#line 4830 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-2].types);
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic((yyvsp[-2].types)->str(), (yyvsp[0].types)->str(), "|"));
		   (yyval.types)->theExpression = (yyval.types)->theExpression + '|' +
		     (yyvsp[0].types)->theExpression;
		   delete (yyvsp[0].types);
                 }
#line 8831 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 788:
#line 4838 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types);  }
#line 8837 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 789:
#line 4842 "parser.y" /* yacc.c:1646  */
    {
		   string one_bar=("~("+(yyvsp[-2].types)->str())+')';
		   string three_bar=("~("+(yyvsp[0].types)->str())+')';
		   (yyval.types)->set_obj("exp");
		   (yyval.types)->set_str(logic(logic((yyvsp[-2].types)->str(), three_bar, "&"),
		     logic(one_bar, (yyvsp[0].types)->str(), "&"), "|"));
		   delete (yyvsp[0].types);
		   //$$= new TYPES1(check_logic($1, $3, "'xor'"));
		 }
#line 8851 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 790:
#line 4851 "parser.y" /* yacc.c:1646  */
    {  (yyval.types) = (yyvsp[0].types);   }
#line 8857 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 791:
#line 4855 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 8863 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 792:
#line 4856 "parser.y" /* yacc.c:1646  */
    {
		 (yyval.types) = Probe((yyvsp[-1].types)->str());
	       }
#line 8871 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 793:
#line 4860 "parser.y" /* yacc.c:1646  */
    {
		   pair<string,TYPES> TT1 = table->lookup((yyvsp[-2].types)->str());
		   pair<string,TYPES> TT3 = table->lookup((yyvsp[0].types)->str());
		   /*if(TT1.first == "err" || TT3.first == "err"){
		     if(TT1.first == "err")
		       err_msg("undeclared identifier '",$1->get_string(),"'");
		     if(TT3.first == "err")
		       err_msg("undeclared identifier '",$3->get_string(),"'");
		   }
		   else*/{
		     (yyval.types) = new TYPES;
		     if (table->lookup((yyvsp[-2].types)->get_string()).first != "qty" &&
			 table->lookup((yyvsp[0].types)->get_string()).first != "qty")
		       (yyval.types)->set_data("bool");
		     else
		       (yyval.types)->set_data("expr");
		     
		     if(table->lookup((yyvsp[-2].types)->get_string()).first == "var" ||
			table->lookup((yyvsp[0].types)->get_string()).first == "var")
		       (yyval.types)->set_str("maybe");
       		     else
		       //$$->set_str($1->str() + $2 + $3->str());
		       (yyval.types)->set_str(bool_relation((yyvsp[-2].types)->theExpression, (yyvsp[0].types)->theExpression, (yyvsp[-1].C_str)));
		     (yyval.types)->theExpression += (yyvsp[-2].types)->theExpression + (yyvsp[-1].C_str) + (yyvsp[0].types)->theExpression;
		     //cout << "@relation " << $$->theExpression << endl;
		   }
		 }
#line 8903 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 794:
#line 4889 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "="; }
#line 8909 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 795:
#line 4889 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "/="; }
#line 8915 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 796:
#line 4889 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "<"; }
#line 8921 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 797:
#line 4890 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "<="; }
#line 8927 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 798:
#line 4890 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = ">"; }
#line 8933 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 799:
#line 4890 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = ">=";}
#line 8939 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 800:
#line 4893 "parser.y" /* yacc.c:1646  */
    {(yyval.types) = (yyvsp[0].types);}
#line 8945 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 801:
#line 4895 "parser.y" /* yacc.c:1646  */
    {  /*
		   if( !$3->isInt() ) {
                     TYPES *ty= new TYPES("ERROR", "ERROR", "ERROR");
		     $$= new TYPES1(ty);
		     err_msg("expecting an integer"); }
		   else
		     $$= $1;*/
		 }
#line 8958 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 809:
#line 4910 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = (yyvsp[0].types);
		  (yyval.types)->theExpression = (yyvsp[-1].C_str) + (yyval.types)->theExpression;
		}
#line 8967 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 810:
#line 4916 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "+";}
#line 8973 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 811:
#line 4917 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "-";}
#line 8979 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 812:
#line 4920 "parser.y" /* yacc.c:1646  */
    {(yyval.types) = (yyvsp[0].types);}
#line 8985 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 813:
#line 4922 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-2].types);
		   (yyval.types)->theExpression = (yyval.types)->theExpression + (yyvsp[-1].C_str)
		     + (yyvsp[0].types)->theExpression;
		 }
#line 8995 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 814:
#line 4929 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "+";}
#line 9001 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 815:
#line 4929 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "-";}
#line 9007 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 816:
#line 4929 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "&";}
#line 9013 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 817:
#line 4933 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = (yyvsp[-2].types);
		   (yyval.types)->theExpression = (yyval.types)->theExpression + (yyvsp[-1].C_str) +
		     (yyvsp[0].types)->theExpression;
		 }
#line 9023 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 818:
#line 4938 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types);  }
#line 9029 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 819:
#line 4941 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "*";}
#line 9035 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 820:
#line 4941 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "/";}
#line 9041 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 821:
#line 4942 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "%";}
#line 9047 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 822:
#line 4942 "parser.y" /* yacc.c:1646  */
    {(yyval.C_str) = "rem";}
#line 9053 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 825:
#line 4947 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[0].types); }
#line 9059 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 826:
#line 4949 "parser.y" /* yacc.c:1646  */
    {
		   (yyvsp[0].types)->set_str(("~("+(yyvsp[0].types)->theExpression)+')');
		   (yyval.types) = (yyvsp[0].types);
		   (yyval.types)->theExpression = "~(" + (yyvsp[0].types)->theExpression + ")";
		 }
#line 9069 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 827:
#line 4957 "parser.y" /* yacc.c:1646  */
    {
		     (yyval.types)=(yyvsp[0].types);
		     if((yyval.types)->get_string()=="true" || (yyval.types)->get_string()=="false"){
		       (yyval.types)->set_data("bool");
		     }
		   }
#line 9080 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 830:
#line 4967 "parser.y" /* yacc.c:1646  */
    { yyerror("Qualified expressions not supported."); }
#line 9086 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 831:
#line 4969 "parser.y" /* yacc.c:1646  */
    { yyerror("Allocators not supported."); }
#line 9092 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 832:
#line 4971 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = (yyvsp[-1].types); }
#line 9098 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 833:
#line 4974 "parser.y" /* yacc.c:1646  */
    {}
#line 9104 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 834:
#line 4975 "parser.y" /* yacc.c:1646  */
    {}
#line 9110 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 835:
#line 4976 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9116 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 837:
#line 4980 "parser.y" /* yacc.c:1646  */
    {}
#line 9122 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 838:
#line 4983 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[-1].types); }
#line 9128 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 839:
#line 4984 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9134 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 840:
#line 4988 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = new TYPES();
		   (yyval.types)->set_str((yyvsp[0].str_list)->front());
		   (yyval.types)->theExpression += (yyvsp[0].str_list)->front();
		 
                 }
#line 9145 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 841:
#line 5008 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types) = new TYPES();
		   (yyval.types)->set_str((yyvsp[-5].str_list)->front());
		   (yyval.types)->theExpression += (yyvsp[-5].str_list)->front() + ">="
		     + (yyvsp[-1].types)->theExpression;
		 }
#line 9156 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 842:
#line 5015 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9162 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 843:
#line 5019 "parser.y" /* yacc.c:1646  */
    { (yyval.str_list) = new list<string>;  (yyval.str_list)->push_back((yyvsp[0].actionptr)->label); }
#line 9168 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 845:
#line 5025 "parser.y" /* yacc.c:1646  */
    { (yyval.str_list) = (yyvsp[-2].str_list);  (yyval.str_list)->push_back((yyvsp[0].C_str)); }
#line 9174 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 846:
#line 5028 "parser.y" /* yacc.c:1646  */
    {  (yyval.types)= (yyvsp[0].types); }
#line 9180 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 847:
#line 5029 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9186 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 848:
#line 5033 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.types)= new TYPES;
		   //$$->set_str(strtok($1, "\""));
      		   //$$->theExpression += $1;
		   
		   char *chptr;
		   chptr = strpbrk((yyvsp[0].strlit), "\"");
		   chptr = strtok(chptr+1, "\"");
		   (yyval.types)->set_str(chptr);
		   (yyval.types)->theExpression += chptr;
		   //cout << "@STRLIT " << $$->theExpression << endl;
      		 }
#line 9203 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 849:
#line 5045 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9209 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 850:
#line 5048 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].actionptr)->label; }
#line 9215 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 851:
#line 5049 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = (yyvsp[0].charlit);  }
#line 9221 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 852:
#line 5050 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].strlit); }
#line 9227 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 853:
#line 5051 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str) = "all"; }
#line 9233 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 854:
#line 5060 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = new TYPES();
		  (yyval.types)->set_str((yyvsp[-2].str_list)->front());
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search(table,$1->label);
                  //$$= t; $$->setstring($1->label);
		}
#line 9245 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 855:
#line 5068 "parser.y" /* yacc.c:1646  */
    {
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search( table, $1->name );
		  //$$= t;
		}
#line 9255 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 856:
#line 5074 "parser.y" /* yacc.c:1646  */
    {
		  //yyerror("Attributes not supported.");
		  //TYPES *t=search( table, $1->name2->label );
		  //$$= t;
		}
#line 9265 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 857:
#line 5081 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[0].actionptr)->label; }
#line 9271 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 858:
#line 5083 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= (yyvsp[-3].actionptr)->label; }
#line 9277 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 859:
#line 5084 "parser.y" /* yacc.c:1646  */
    { (yyval.C_str)= "range"; }
#line 9283 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 860:
#line 5085 "parser.y" /* yacc.c:1646  */
    {}
#line 9289 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 861:
#line 5089 "parser.y" /* yacc.c:1646  */
    {}
#line 9295 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 862:
#line 5090 "parser.y" /* yacc.c:1646  */
    {}
#line 9301 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 863:
#line 5094 "parser.y" /* yacc.c:1646  */
    {}
#line 9307 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 865:
#line 5100 "parser.y" /* yacc.c:1646  */
    {
		  /*($$ = table->lookup($1->front());
		  if($$ == 0)
		    {
		      err_msg("'", $1->front(), "' undeclared");	
		      $$ = new TYPES("err", "err");
		      }*/
		  (yyval.types) = new TYPES;
		  (yyval.types)->set_list((yyvsp[0].typelist));
		  (yyval.types)->set_str((yyvsp[-1].str_list)->front());
		  if ((yyvsp[-1].str_list)->front()=="delay") {
		    (yyval.types)->theExpression =
		      "{" + (yyvsp[0].typelist)->front().second.theExpression + "}";
		  } else if ((yyvsp[-1].str_list)->front()=="selection") {
		    // ZHEN(Done): extract the first integer of $2 and subtract 1 from it
		    // "uniform(0," + $2->front().second.theExpression + ")";
		    //$$->theExpression ="uniform(0," + intToString(atoi((($2->front().second.theExpression).substr(0,($2->front().second.theExpression).find(";")).c_str()))-1) + ")";
		    // "floor(uniform(0," + $2->front().second.theExpression + "))";
		    (yyval.types)->theExpression ="floor(uniform(0," + intToString(atoi((((yyvsp[0].typelist)->front().second.theExpression).substr(0,((yyvsp[0].typelist)->front().second.theExpression).find(";")).c_str()))) + "))";
		    (yyval.types)->set_str((yyval.types)->theExpression);
		  } else if (((yyvsp[-1].str_list)->front()=="init_channel") ||
		             ((yyvsp[-1].str_list)->front()=="active") ||
                             ((yyvsp[-1].str_list)->front()=="passive") ||
                             ((yyvsp[-1].str_list)->front()=="timing")) {
		  } else {
		    (yyval.types)->theExpression =
		      "BIT(" + (yyvsp[-1].str_list)->front() + "," + (yyvsp[0].typelist)->front().second.theExpression + ")";
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
#line 9350 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 866:
#line 5139 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = new TYPES((yyvsp[-1].types));
		  //$$->formal_list= NULL;
                  //$$= $1;
                  (yyval.types)->set_list((yyvsp[0].typelist));
		}
#line 9361 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 867:
#line 5148 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = new TYPES("uk", "integer");
		  (yyval.types)->set_grp(TYPES::Integer);
		  (yyval.types)->set_int((yyvsp[0].intval));
		  (yyval.types)->theExpression += intToString((yyvsp[0].intval));
	          (yyval.types)->set_str((yyval.types)->theExpression);
                }
#line 9373 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 868:
#line 5156 "parser.y" /* yacc.c:1646  */
    {
                  (yyval.types)= new TYPES("uk", "real");
		  (yyval.types)->set_grp(TYPES::Real);
                  (yyval.types)->set_flt((yyvsp[0].floatval));
		  (yyval.types)->theExpression += numToString((yyvsp[0].floatval));
                }
#line 9384 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 869:
#line 5163 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = new TYPES("uk", "integer");
		  (yyval.types)->set_grp(TYPES::Integer);
		  (yyval.types)->set_int((yyvsp[0].basedlit));
		  (yyval.types)->theExpression += intToString((yyvsp[0].basedlit));
                }
#line 9395 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 870:
#line 5170 "parser.y" /* yacc.c:1646  */
    {		
		  (yyval.types) = new TYPES("chr", "uk");
		  (yyval.types)->set_string((yyvsp[0].charlit));
		  (yyval.types)->theExpression += (yyvsp[0].charlit);
		  // cout << "@CHARLIT " << $$->theExpression << endl;
		}
#line 9406 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 871:
#line 5177 "parser.y" /* yacc.c:1646  */
    {
                  (yyval.types)= new TYPES("uk", "BIT_VECTOR");
                  (yyval.types)->set_string((yyvsp[0].strlit));
		  (yyval.types)->theExpression += (yyvsp[0].strlit);
                }
#line 9416 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 872:
#line 5182 "parser.y" /* yacc.c:1646  */
    {}
#line 9422 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 873:
#line 5183 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9428 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 874:
#line 5186 "parser.y" /* yacc.c:1646  */
    { (yyval.typelist) = new TYPELIST(make_pair((yyvsp[0].actionptr)->label, TYPES())); }
#line 9434 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 875:
#line 5188 "parser.y" /* yacc.c:1646  */
    {
		  TYPES t("cst", "int");
		  t.set_int((yyvsp[-1].intval));
		  (yyval.typelist) = new TYPELIST(make_pair((yyvsp[0].actionptr)->label, t));
		}
#line 9444 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 876:
#line 5194 "parser.y" /* yacc.c:1646  */
    {
		  TYPES t("cst", "flt");
		  t.set_flt((yyvsp[-1].floatval));
		  (yyval.typelist) = new TYPELIST(make_pair((yyvsp[0].actionptr)->label, t));
		}
#line 9454 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 877:
#line 5200 "parser.y" /* yacc.c:1646  */
    {
		  TYPES t("cst", "int");
		  t.set_int((yyvsp[-1].basedlit));
		  (yyval.typelist) = new TYPELIST(make_pair((yyvsp[0].actionptr)->label, t));
		  //TYPES *temp= search(table,$2->label);
		  //$$= new TYPES("CONST", "uk", "uk");
		}
#line 9466 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 878:
#line 5210 "parser.y" /* yacc.c:1646  */
    {
		  //$$ = table->lookup($2->label);
		  (yyval.types) = new TYPES("cst", "int");
		  (yyval.types)->set_int((yyvsp[-1].intval));
		}
#line 9476 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 879:
#line 5216 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = new TYPES("cst", "flt");
		  (yyval.types)->set_flt((yyvsp[-1].floatval));
		  /*
		  TYPES *temp = table->lookup($2->label);
		  if(temp == NULL)
		    {
		      err_msg("'", $2->label, "' was not declared before;");
		      temp = new TYPES("err", "err", "err");
		    }
		  $$= new TYPES("CONST", temp->datatype(), temp->subtype());*/
		 }
#line 9493 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 880:
#line 5229 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.types) = new TYPES("cst", "int");
		  (yyval.types)->set_int((yyvsp[-1].basedlit));
		  /*
		  TYPES *temp = table->lookup($2->label);
		  if(temp == NULL)
		    {
		      err_msg("'", $2->label, "' was not declared before;");
		      temp = new TYPES("err", "err", "err");
		    }
		  $$= new TYPES("CONST", temp->datatype(), temp->subtype() );*/
		}
#line 9510 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 881:
#line 5244 "parser.y" /* yacc.c:1646  */
    {
		  /*if(strcmp_nocase($2->datatype(), $4->datatype())
		     && !($2->isError() || $4->isError()))
		    {
		      $$= new TYPES("err","err", "err");
		      err_msg("wrong types between '(' and ')'");
		    }
		  else
		  */
		    (yyval.types)= (yyvsp[-1].types);
		}
#line 9526 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 882:
#line 5256 "parser.y" /* yacc.c:1646  */
    {
		  //if($2->isUnknown())
		    (yyval.types)= (yyvsp[-1].types);
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
#line 9544 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 886:
#line 5289 "parser.y" /* yacc.c:1646  */
    {
		  //if($1->isUnknown())
		    (yyval.types)= (yyvsp[0].types);
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
#line 9562 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 887:
#line 5305 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.ty_lst) = (yyvsp[-2].ty_lst);
		  if((yyval.ty_lst) && (yyvsp[0].types)->grp_id() != TYPES::Error){
		    if((yyval.ty_lst)->back().grp_id() != (yyvsp[0].types)->grp_id()){
		      err_msg("type mismatch");
		      delete (yyvsp[-2].ty_lst);
		      (yyval.ty_lst) = 0;
		    }
		    else
		      (yyval.ty_lst)->push_back(*(yyvsp[0].types));
		  }
		}
#line 9579 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 888:
#line 5318 "parser.y" /* yacc.c:1646  */
    {
		  (yyval.ty_lst) = 0;
		  if((yyvsp[0].types)->grp_id() != TYPES::Error){
		    (yyval.ty_lst) = new list<TYPES>;
		    (yyval.ty_lst)->push_back(*(yyvsp[0].types));
		  }
		  delete (yyvsp[0].types);
		}
#line 9592 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 889:
#line 5328 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= (yyvsp[0].types); }
#line 9598 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 890:
#line 5329 "parser.y" /* yacc.c:1646  */
    { (yyval.types) = new TYPES(); }
#line 9604 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 891:
#line 5330 "parser.y" /* yacc.c:1646  */
    { (yyval.types)= new TYPES(); (yyval.types)->set_str("OTHERS"); }
#line 9610 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 893:
#line 5348 "parser.y" /* yacc.c:1646  */
    {}
#line 9616 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 895:
#line 5353 "parser.y" /* yacc.c:1646  */
    {
		    new_action_table((yyvsp[-1].actionptr)->label);
		    new_event_table((yyvsp[-1].actionptr)->label);
		    new_rule_table((yyvsp[-1].actionptr)->label);
		    new_conflict_table((yyvsp[-1].actionptr)->label);
		    tel_tb = new tels_table;
		  }
#line 9628 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 896:
#line 5361 "parser.y" /* yacc.c:1646  */
    {		
		    map<string,int> sig_list;
		    if((yyvsp[-2].str_bl_lst))
		      for(list<pair<string,int> >::iterator b = (yyvsp[-2].str_bl_lst)->begin();
			  b != (yyvsp[-2].str_bl_lst)->end(); b++)
			sig_list.insert(*b);
		    if((yyvsp[-1].str_bl_lst))
		      for(list<pair<string,int> >::iterator b = (yyvsp[-1].str_bl_lst)->begin();
			  b != (yyvsp[-1].str_bl_lst)->end(); b++)
			if(sig_list.find(b->first) == sig_list.end())
			  sig_list.insert(make_pair(b->first, OUT));
		    tel_tb->insert(sig_list);
		    tel_tb->set_id((yyvsp[-5].actionptr)->label);
		     tel_tb->set_type((yyvsp[-5].actionptr)->label);
		    (*open_module_list)[(yyvsp[-5].actionptr)->label] = make_pair(tel_tb, false);
		  }
#line 9649 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 897:
#line 5378 "parser.y" /* yacc.c:1646  */
    {
		    cur_file = *(yyvsp[-1].str);
		    switch_buffer(*(yyvsp[-1].str));
		    delete (yyvsp[-1].str);
		  }
#line 9659 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 898:
#line 5386 "parser.y" /* yacc.c:1646  */
    {
		    string f_name;
		    if(*(yyvsp[-2].str) != "error"){
		      if (toTEL)
			f_name = *(yyvsp[-2].str) + '/' + (yyvsp[0].id) + ".hse";
		      else
			f_name = *(yyvsp[-2].str) + '/' + (yyvsp[0].id) + ".csp";
		      (yyval.str) = new string(f_name);
		      delete (yyvsp[-2].str);
		    }
		    else
		      (yyval.str) = (yyvsp[-2].str);
		  }
#line 9677 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 899:
#line 5400 "parser.y" /* yacc.c:1646  */
    {
		    string f_name;
		    if (toTEL)
		      f_name = cur_dir + '/' + string((yyvsp[0].id)) + ".hse";
		    else
		      f_name = cur_dir + '/' + string((yyvsp[0].id)) + ".csp";
		    (yyval.str) = new string(f_name);
		  }
#line 9690 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 900:
#line 5411 "parser.y" /* yacc.c:1646  */
    {
		    string ss = *(yyvsp[-2].str) + '/' + (yyvsp[0].actionptr)->label;
		    (yyval.str) = new string(ss);
		  }
#line 9699 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 901:
#line 5416 "parser.y" /* yacc.c:1646  */
    { (yyval.str) = new string((yyvsp[0].actionptr)->label); }
#line 9705 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 902:
#line 5418 "parser.y" /* yacc.c:1646  */
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
#line 9720 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 903:
#line 5428 "parser.y" /* yacc.c:1646  */
    { (yyval.str) = &cur_dir; }
#line 9726 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 904:
#line 5433 "parser.y" /* yacc.c:1646  */
    { (yyval.id) = copy_string(((*name_mapping)[(yyvsp[0].actionptr)->label]).c_str()); }
#line 9732 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 905:
#line 5435 "parser.y" /* yacc.c:1646  */
    { (yyval.id) = "*"; }
#line 9738 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 906:
#line 5439 "parser.y" /* yacc.c:1646  */
    {
		    if((yyvsp[-1].str_bl_lst) && (yyvsp[0].str_bl_lst))
		      {
			(yyvsp[-1].str_bl_lst)->splice((yyvsp[-1].str_bl_lst)->begin(), *(yyvsp[0].str_bl_lst));
			(yyval.str_bl_lst) = (yyvsp[-1].str_bl_lst);
		      }
		    else if((yyvsp[-1].str_bl_lst))
		      (yyval.str_bl_lst) = (yyvsp[-1].str_bl_lst);
		    else
		      (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst);
		  }
#line 9754 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 907:
#line 5450 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0; }
#line 9760 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 911:
#line 5462 "parser.y" /* yacc.c:1646  */
    {
		    makedelaydecl((yyvsp[-3].actionptr),(yyvsp[-1].delayval));
		    (yyval.str_bl_lst) = 0;
		  }
#line 9769 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 912:
#line 5470 "parser.y" /* yacc.c:1646  */
    {
		    makedefinedecl((yyvsp[-3].actionptr),(yyvsp[-1].floatval));
		    (yyval.str_bl_lst) = 0;
		  }
#line 9778 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 913:
#line 5475 "parser.y" /* yacc.c:1646  */
    {
		    makedefinedecl((yyvsp[-4].actionptr),(-1.0)*(yyvsp[-1].floatval));
		    (yyval.str_bl_lst) = 0;
		  }
#line 9787 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 914:
#line 5483 "parser.y" /* yacc.c:1646  */
    {
		    initial_stateADT init_st(true, false, false);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[-1].action_list)->begin();
			b != (yyvsp[-1].action_list)->end(); b++){
		      (*b)->initial_state = init_st;
		      makebooldecl((yyvsp[-2].intval), *b, FALSE, NULL);
		      (yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[-2].intval)));
		    }
		  }
#line 9802 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 915:
#line 5494 "parser.y" /* yacc.c:1646  */
    {
		    initial_stateADT init_st(true, true, false);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[-5].action_list)->begin();
			b != (yyvsp[-5].action_list)->end(); b++) {
		      (*b)->initial_state = init_st;
		      makebooldecl((yyvsp[-6].intval), *b, (yyvsp[-2].boolval), NULL);
		      (yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[-6].intval)));
		    }
		  }
#line 9817 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 916:
#line 5505 "parser.y" /* yacc.c:1646  */
    {
		    initial_stateADT init_st(true, false, true);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[-5].action_list)->begin();
			b != (yyvsp[-5].action_list)->end(); b++)
		      {
			(*b)->initial_state = init_st;
			makebooldecl((yyvsp[-6].intval), *b, FALSE, &((yyvsp[-2].delayval)));
			(yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[-6].intval)));
		      }
		  }
#line 9833 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 917:
#line 5517 "parser.y" /* yacc.c:1646  */
    {
		    initial_stateADT init_st(true, true, true);
		    (yyval.str_bl_lst) = new list<pair<string,int> >;
		    for(list<actionADT>::iterator b = (yyvsp[-7].action_list)->begin();
			b != (yyvsp[-7].action_list)->end(); b++)
		      {
			(*b)->initial_state = init_st;
			makebooldecl((yyvsp[-8].intval), *b, (yyvsp[-4].boolval), &((yyvsp[-2].delayval)));
			(yyval.str_bl_lst)->push_back(make_pair((*b)->label, (yyvsp[-8].intval)));
		      }
		  }
#line 9849 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 918:
#line 5532 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.action_list) = new list<actionADT>;
                   (yyval.action_list)->push_back((yyvsp[0].actionptr));
		 }
#line 9858 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 919:
#line 5537 "parser.y" /* yacc.c:1646  */
    {
		   (yyval.action_list) = (yyvsp[-2].action_list);
                   (yyval.action_list)->push_back((yyvsp[0].actionptr));
		 }
#line 9867 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 920:
#line 5574 "parser.y" /* yacc.c:1646  */
    { assigndelays(&((yyval.delayval)),(yyvsp[-7].intval),(yyvsp[-5].intval),NULL,(yyvsp[-3].intval),(yyvsp[-1].intval),NULL); }
#line 9873 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 921:
#line 5577 "parser.y" /* yacc.c:1646  */
    { sprintf(distrib1,"%s(%g,%g)",(yyvsp[-17].actionptr)->label,(yyvsp[-15].floatval),(yyvsp[-13].floatval));
			  sprintf(distrib2,"%s(%g,%g)",(yyvsp[-6].actionptr)->label,(yyvsp[-4].floatval),(yyvsp[-2].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[-21].intval),(yyvsp[-19].intval),distrib1,(yyvsp[-10].intval),(yyvsp[-8].intval),distrib2);
                        }
#line 9882 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 922:
#line 5583 "parser.y" /* yacc.c:1646  */
    { sprintf(distrib1,"%s(%g,%g)",(yyvsp[-10].actionptr)->label,(yyvsp[-8].floatval),(yyvsp[-6].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[-14].intval),(yyvsp[-12].intval),distrib1,(yyvsp[-3].intval),(yyvsp[-1].intval),NULL);
                        }
#line 9890 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 923:
#line 5588 "parser.y" /* yacc.c:1646  */
    { sprintf(distrib2,"%s(%g,%g)",(yyvsp[-6].actionptr)->label,(yyvsp[-4].floatval),(yyvsp[-2].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[-14].intval),(yyvsp[-12].intval),NULL,(yyvsp[-10].intval),(yyvsp[-8].intval),distrib2);
                        }
#line 9898 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 924:
#line 5592 "parser.y" /* yacc.c:1646  */
    { assigndelays(&((yyval.delayval)),(yyvsp[-3].intval),(yyvsp[-1].intval),NULL,(yyvsp[-3].intval),(yyvsp[-1].intval),NULL); }
#line 9904 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 925:
#line 5594 "parser.y" /* yacc.c:1646  */
    { sprintf(distrib1,"%s(%g,%g)",(yyvsp[-6].actionptr)->label,(yyvsp[-4].floatval),(yyvsp[-2].floatval));
                          assigndelays(&((yyval.delayval)),(yyvsp[-10].intval),(yyvsp[-8].intval),distrib1,(yyvsp[-10].intval),(yyvsp[-8].intval),distrib1); }
#line 9911 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 926:
#line 5597 "parser.y" /* yacc.c:1646  */
    { checkdelay((yyvsp[0].actionptr)->label,(yyvsp[0].actionptr)->type);
			  assigndelays(&((yyval.delayval)),(yyvsp[0].actionptr)->delay.lr,(yyvsp[0].actionptr)->delay.ur,
				       (yyvsp[0].actionptr)->delay.distr,(yyvsp[0].actionptr)->delay.lf,
                                       (yyvsp[0].actionptr)->delay.uf,(yyvsp[0].actionptr)->delay.distf); }
#line 9920 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 928:
#line 5605 "parser.y" /* yacc.c:1646  */
    {
		if((yyvsp[-1].str_bl_lst) && (yyvsp[0].str_bl_lst)){
		  (yyval.str_bl_lst) = (yyvsp[-1].str_bl_lst);
		  (yyval.str_bl_lst)->splice((yyval.str_bl_lst)->begin(), *(yyvsp[0].str_bl_lst));
		}
		else if((yyvsp[-1].str_bl_lst))
		  (yyval.str_bl_lst) = (yyvsp[-1].str_bl_lst);
		else
		  (yyval.str_bl_lst) = (yyvsp[0].str_bl_lst);
	      }
#line 9935 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 929:
#line 5615 "parser.y" /* yacc.c:1646  */
    { (yyval.str_bl_lst) = 0; }
#line 9941 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 931:
#line 5621 "parser.y" /* yacc.c:1646  */
    {     		
		pair<tels_table*,bool> rp = (*open_module_list)[(yyvsp[-5].actionptr)->label];
		if(rp.first != 0){
		  (yyval.str_bl_lst) = new list<pair<string,int> >;
		  tels_table* rpc = new tels_table(rp.first);
		  rpc->set_id((yyvsp[-4].actionptr)->label);
		  rpc->set_type((yyvsp[-5].actionptr)->label);
		  rpc->insert(*(yyvsp[-2].csp_map));

		  my_list sig_mapping(rpc->portmap());
		  rpc->instantiate(&sig_mapping, (yyvsp[-4].actionptr)->label, (yyvsp[-5].actionptr)->label, true);
		  if(tel_tb->insert((yyvsp[-4].actionptr)->label, rpc) == false)
		    err_msg("duplicate component label '", (yyvsp[-4].actionptr)->label, "'");

		  const map<string,int> tmp = rpc->signals();
		  for(map<string,string>::iterator b = (yyvsp[-2].csp_map)->begin();
		      b != (yyvsp[-2].csp_map)->end(); b++){
		    if(tmp.find(b->first) != tmp.end())
		      (yyval.str_bl_lst)->push_back(make_pair(b->second,
					      tmp.find(b->first)->second));
		    else
		      err_msg("signal '", b->first, "' not found");
		  }
		}
		else {
		  err_msg("module '", (yyvsp[-5].actionptr)->label, "' not found");
		  delete (yyvsp[-2].csp_map);
		  return 1;
		}
		delete (yyvsp[-2].csp_map);
	      }
#line 9977 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 932:
#line 5656 "parser.y" /* yacc.c:1646  */
    { (yyval.csp_map) = (yyvsp[-2].csp_map);  (yyval.csp_map)->insert(*(yyvsp[0].csp_pair));  delete (yyvsp[0].csp_pair); }
#line 9983 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 933:
#line 5658 "parser.y" /* yacc.c:1646  */
    { (yyval.csp_map) = new map<string, string>;  (yyval.csp_map)->insert(*(yyvsp[0].csp_pair));  delete (yyvsp[0].csp_pair); }
#line 9989 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 934:
#line 5662 "parser.y" /* yacc.c:1646  */
    { (yyval.csp_pair) = new pair<string, string>((yyvsp[-2].actionptr)->label, (yyvsp[0].actionptr)->label); }
#line 9995 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 935:
#line 5666 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[-1].TERSptr),";");
		    if(tel_tb->insert((yyvsp[-3].actionptr)->label, (yyval.TERSptr))==false){
		      err_msg("duplicate process label '", (yyvsp[-3].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
		    if (1) {
		      printf("Compiled process %s\n",(yyvsp[-3].actionptr)->label);
		      fprintf(lg,"Compiled process %s\n",(yyvsp[-3].actionptr)->label);
		    } else
		      emitters(0, (yyvsp[-3].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
#line 10014 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 936:
#line 5681 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[-2].TERSptr),":");
		    if(tel_tb->insert((yyvsp[-4].actionptr)->label, (yyval.TERSptr))==false){
		      err_msg("duplicate process label '", (yyvsp[-4].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
		    if (1) {
		      printf("Compiled process %s\n",(yyvsp[-4].actionptr)->label);
		      fprintf(lg,"Compiled process %s\n",(yyvsp[-4].actionptr)->label);
		    } else
		      emitters(0, (yyvsp[-4].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
#line 10033 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 937:
#line 5696 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[-1].TERSptr),";");
		    if(tel_tb->insert((yyvsp[-3].actionptr)->label, (yyval.TERSptr), true)==false) {
		      err_msg("duplicate testbench label '", (yyvsp[-3].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    if (1) {
		      printf("Compiled testbench %s\n",(yyvsp[-3].actionptr)->label);
		      fprintf(lg,"Compiled testbench %s\n",(yyvsp[-3].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[-3].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
#line 10052 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 938:
#line 5711 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = TERSrepeat((yyvsp[-2].TERSptr),":");
		    if(tel_tb->insert((yyvsp[-4].actionptr)->label, (yyval.TERSptr), true)==false) {
		      err_msg("duplicate testbench label '", (yyvsp[-4].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    if (1) {
		      printf("Compiled testbench %s\n",(yyvsp[-4].actionptr)->label);
		      fprintf(lg,"Compiled testbench %s\n",(yyvsp[-4].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[-4].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
		  }
#line 10071 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 939:
#line 5726 "parser.y" /* yacc.c:1646  */
    {
                    (yyval.TERSptr) = TERSrepeat((yyvsp[-1].TERSptr),";");
		    if(tel_tb->insert((yyvsp[-3].actionptr)->label, (yyval.TERSptr))==false) {
		      err_msg("duplicate gate label '", (yyvsp[-3].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    if (1) {
		      printf("Compiled gate %s\n",(yyvsp[-3].actionptr)->label);
		      fprintf(lg,"Compiled gate %s\n",(yyvsp[-3].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[-3].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
                  }
#line 10090 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 940:
#line 5741 "parser.y" /* yacc.c:1646  */
    {
                    (yyval.TERSptr) = TERSrepeat((yyvsp[-1].TERSptr),";");
		    if(tel_tb->insert((yyvsp[-3].actionptr)->label, (yyval.TERSptr))==false) {
		      err_msg("duplicate constant label '", (yyvsp[-3].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    TERSmarkord((yyval.TERSptr));
                    if (1) {
		      printf("Compiled constraint %s\n",(yyvsp[-3].actionptr)->label);
		      fprintf(lg,"Compiled constraint %s\n",(yyvsp[-3].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[-3].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
                  }
#line 10110 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 941:
#line 5757 "parser.y" /* yacc.c:1646  */
    {
                    (yyval.TERSptr) = TERSrepeat((yyvsp[-1].TERSptr),";");
		    if(tel_tb->insert((yyvsp[-3].actionptr)->label, (yyval.TERSptr))==false) {
		      err_msg("duplicate assumption label '", (yyvsp[-3].actionptr)->label, "'");
		      delete (yyval.TERSptr);
		      return 1;
		    }
                    TERSmarkassump((yyval.TERSptr));
                    if (1) {
		      printf("Compiled assumption %s\n",(yyvsp[-3].actionptr)->label);
		      fprintf(lg,"Compiled assumption %s\n",(yyvsp[-3].actionptr)->label);
		    } else
		      emitters(outpath, (yyvsp[-3].actionptr)->label, (yyval.TERSptr));
		    TERSdelete((yyval.TERSptr));
                  }
#line 10130 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 944:
#line 5778 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-3].actionptr)->vacuous=FALSE;
		    (yyvsp[0].actionptr)->vacuous=FALSE;
		    string ss1 = '[' + *(yyvsp[-5].str) + ']' + 'd';
                    (yyval.TERSptr) = TERS((yyvsp[-3].actionptr), FALSE,(yyvsp[-3].actionptr)->lower,(yyvsp[-3].actionptr)->upper,FALSE,
			      ss1.c_str(), (yyvsp[-3].actionptr)->dist);
		    string ss2 = '[' + *(yyvsp[-2].str) + ']' + 'd';
		    if ((!((yyvsp[-3].actionptr)->initial) && !(strchr((yyvsp[-3].actionptr)->label,'+'))) ||
			((yyvsp[-3].actionptr)->initial && strchr((yyvsp[-3].actionptr)->label,'+'))) {
		      (yyval.TERSptr) = TERScompose(TERS((yyvsp[0].actionptr), FALSE,(yyvsp[0].actionptr)->lower,(yyvsp[0].actionptr)->upper,
					    FALSE,ss2.c_str(),(yyvsp[0].actionptr)->dist),(yyval.TERSptr),";");
		    } else {
		      (yyval.TERSptr) = TERScompose((yyval.TERSptr),TERS((yyvsp[0].actionptr), FALSE,(yyvsp[0].actionptr)->lower,(yyvsp[0].actionptr)->upper,
					       FALSE,ss2.c_str(),(yyvsp[0].actionptr)->dist),";");
		    }
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
                    delete (yyvsp[-5].str);
                    delete (yyvsp[-2].str);
		  }
#line 10154 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 945:
#line 5798 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-3].actionptr)->vacuous=FALSE;
		    (yyvsp[0].actionptr)->vacuous=FALSE;
                    (yyval.TERSptr) = TERScompose((yyvsp[-5].TERSptr),TERS((yyvsp[-3].actionptr), FALSE,(yyvsp[-3].actionptr)->lower,(yyvsp[-3].actionptr)->upper,
                                             FALSE,"[true]",(yyvsp[-3].actionptr)->dist),";");
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),TERSrename((yyval.TERSptr),(yyvsp[-2].TERSptr)),";");
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),TERS((yyvsp[0].actionptr), FALSE,(yyvsp[0].actionptr)->lower,(yyvsp[0].actionptr)->upper,
                                             FALSE,"[true]",(yyvsp[0].actionptr)->dist),";");
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
#line 10169 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 946:
#line 5811 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)),"|"); }
#line 10175 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 948:
#line 5815 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = Guard(*(yyvsp[-3].str));
                    delete (yyvsp[-3].str);
		    TERSmodifydist((yyval.TERSptr),(yyvsp[-1].floatval));
		  }
#line 10185 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 949:
#line 5823 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[0].actionptr)->vacuous=FALSE;
		    string ss1 = '[' + *(yyvsp[-2].str) + ']';// + 'd';
                    (yyval.TERSptr) = TERS((yyvsp[0].actionptr),FALSE,0,INFIN,FALSE,ss1.c_str(),(yyvsp[0].actionptr)->dist);
                    //$$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                    //                         FALSE, Cstr, $6->dist),";");
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
                    delete (yyvsp[-2].str);
		  }
#line 10199 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 950:
#line 5833 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[0].actionptr)->vacuous=FALSE;
		    string ss1 = '[' + *(yyvsp[-7].str) + ']';// + 'd';
                    (yyval.TERSptr) = TERS((yyvsp[0].actionptr),FALSE,(yyvsp[-4].intval),(yyvsp[-2].intval),FALSE,ss1.c_str(),(yyvsp[0].actionptr)->dist);
                    //$$ = TERScompose($$,TERS($6, FALSE,$6->lower,$6->upper,
                    //                         FALSE, Cstr, $6->dist),";");
                    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
                    delete (yyvsp[-7].str);
		  }
#line 10213 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 951:
#line 5843 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-2].actionptr)->vacuous=FALSE;
                    (yyval.TERSptr) = TERS((yyvsp[-2].actionptr),FALSE,0,INFIN,FALSE,"[true]",(yyvsp[-2].actionptr)->dist);
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),(yyvsp[0].TERSptr),";");
                    //$$ = TERSmakeloop($$);
		  }
#line 10224 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 952:
#line 5850 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-3].actionptr)->vacuous=TRUE;
                    (yyval.TERSptr) = TERS((yyvsp[-3].actionptr),FALSE,0,INFIN,FALSE,"[true]",(yyvsp[-3].actionptr)->dist);
                    (yyval.TERSptr) = TERScompose((yyval.TERSptr),(yyvsp[0].TERSptr),";");
                    //$$ = TERSmakeloop($$);
		  }
#line 10235 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 953:
#line 5859 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr),TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)),"#"); }
#line 10241 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 955:
#line 5864 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr),"||"); }
#line 10247 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 957:
#line 5869 "parser.y" /* yacc.c:1646  */
    { (yyvsp[0].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[0].actionptr), FALSE, (yyvsp[0].actionptr)->lower, (yyvsp[0].actionptr)->upper, FALSE,
			      "[true]",(yyvsp[0].actionptr)->dist);
		  }
#line 10256 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 958:
#line 5874 "parser.y" /* yacc.c:1646  */
    {
                    (yyval.TERSptr) = TERS(make_dummy((yyvsp[0].actionptr)->label),FALSE,(yyvsp[0].actionptr)->lower,(yyvsp[0].actionptr)->upper, 
			      FALSE,"[true]",(yyvsp[0].actionptr)->dist);
                  }
#line 10265 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 959:
#line 5879 "parser.y" /* yacc.c:1646  */
    { (yyvsp[0].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[0].actionptr), FALSE, (yyvsp[-4].intval), (yyvsp[-2].intval), FALSE); }
#line 10272 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 961:
#line 5884 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERSempty(); }
#line 10278 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 962:
#line 5888 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)), "|"); }
#line 10284 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 963:
#line 5890 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)), ";"); }
#line 10290 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 964:
#line 5892 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)), ":"); }
#line 10296 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 965:
#line 5894 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), (yyvsp[0].TERSptr), "||"); }
#line 10302 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 966:
#line 5895 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = (yyvsp[-1].TERSptr); }
#line 10308 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 969:
#line 5900 "parser.y" /* yacc.c:1646  */
    {  (yyval.TERSptr)=(yyvsp[-1].TERSptr);  (yyval.TERSptr) = TERSrepeat((yyval.TERSptr),";");  }
#line 10314 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 970:
#line 5901 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = (yyvsp[-1].TERSptr); }
#line 10320 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 971:
#line 5902 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr)=TERSmakeloop((yyvsp[-1].TERSptr)); }
#line 10326 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 972:
#line 5906 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)),"|"); }
#line 10332 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 974:
#line 5911 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr), TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)),";"); }
#line 10338 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 975:
#line 5913 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = TERScompose((yyvsp[-4].TERSptr), TERSrename((yyvsp[-4].TERSptr),(yyvsp[-2].TERSptr)),";");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
#line 10347 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 976:
#line 5918 "parser.y" /* yacc.c:1646  */
    {
		    TERSmodifydist((yyvsp[-5].TERSptr),(yyvsp[-3].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[-5].TERSptr), TERSrename((yyvsp[-5].TERSptr),(yyvsp[0].TERSptr)),";");
		  }
#line 10356 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 977:
#line 5923 "parser.y" /* yacc.c:1646  */
    {
		    TERSmodifydist((yyvsp[-7].TERSptr),(yyvsp[-5].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[-7].TERSptr), TERSrename((yyvsp[-7].TERSptr),(yyvsp[-2].TERSptr)),";");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
#line 10366 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 978:
#line 5929 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-3].TERSptr), TERSrename((yyvsp[-3].TERSptr),(yyvsp[0].TERSptr)),":"); }
#line 10372 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 979:
#line 5931 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = TERScompose((yyvsp[-5].TERSptr), TERSrename((yyvsp[-5].TERSptr),(yyvsp[-2].TERSptr)),":");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
#line 10381 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 980:
#line 5936 "parser.y" /* yacc.c:1646  */
    {
		    TERSmodifydist((yyvsp[-6].TERSptr),(yyvsp[-4].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[-6].TERSptr), TERSrename((yyvsp[-6].TERSptr),(yyvsp[0].TERSptr)),":");
		  }
#line 10390 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 981:
#line 5941 "parser.y" /* yacc.c:1646  */
    {
		    TERSmodifydist((yyvsp[-8].TERSptr),(yyvsp[-6].floatval));
		    (yyval.TERSptr) = TERScompose((yyvsp[-8].TERSptr), TERSrename((yyvsp[-8].TERSptr),(yyvsp[-2].TERSptr)),":");
		    (yyval.TERSptr) = TERSmakeloop((yyval.TERSptr));
		  }
#line 10400 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 982:
#line 5959 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[0].floatval); }
#line 10406 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 983:
#line 5960 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (double)(yyvsp[0].intval); }
#line 10412 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 984:
#line 5962 "parser.y" /* yacc.c:1646  */
    { checkdefine((yyvsp[0].actionptr)->label,(yyvsp[0].actionptr)->type);
		    (yyval.floatval) = (yyvsp[0].actionptr)->real_def; }
#line 10419 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 985:
#line 5964 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-3].floatval)+(yyvsp[-1].floatval); }
#line 10425 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 986:
#line 5965 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-3].floatval)-(yyvsp[-1].floatval); }
#line 10431 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 987:
#line 5966 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-2].floatval) * (yyvsp[0].floatval); }
#line 10437 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 988:
#line 5967 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-2].floatval) * (yyvsp[0].intval); }
#line 10443 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 989:
#line 5969 "parser.y" /* yacc.c:1646  */
    { checkdefine((yyvsp[0].actionptr)->label,(yyvsp[0].actionptr)->type);
		    (yyval.floatval) = (yyvsp[-2].floatval) * (yyvsp[0].actionptr)->real_def; }
#line 10450 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 990:
#line 5972 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-7].floatval)*((yyvsp[-3].floatval)+(yyvsp[-1].floatval)); }
#line 10456 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 991:
#line 5974 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-7].floatval)*((yyvsp[-3].floatval)-(yyvsp[-1].floatval)); }
#line 10462 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 992:
#line 5975 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-2].floatval) / (yyvsp[0].floatval); }
#line 10468 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 993:
#line 5976 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-2].floatval) / (yyvsp[0].intval); }
#line 10474 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 994:
#line 5978 "parser.y" /* yacc.c:1646  */
    { checkdefine((yyvsp[0].actionptr)->label,(yyvsp[0].actionptr)->type);
		    (yyval.floatval) = (yyvsp[-2].floatval) / (yyvsp[0].actionptr)->real_def; }
#line 10481 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 995:
#line 5981 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-7].floatval)/((yyvsp[-3].floatval)+(yyvsp[-1].floatval)); }
#line 10487 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 996:
#line 5983 "parser.y" /* yacc.c:1646  */
    { (yyval.floatval) = (yyvsp[-7].floatval)/((yyvsp[-3].floatval)-(yyvsp[-1].floatval)); }
#line 10493 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 998:
#line 5988 "parser.y" /* yacc.c:1646  */
    {
		    (yyval.TERSptr) = Guard(*(yyvsp[0].str));
                    delete (yyvsp[0].str);
		  }
#line 10502 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 999:
#line 5993 "parser.y" /* yacc.c:1646  */
    {
                    //char *t = new char[strlen($6) + 3];
                    //sprintf(t, "[%s]", $6);
		    string ss = '[' + *(yyvsp[0].str) + ']';
                    (yyval.TERSptr) = TERS(dummyE(), FALSE, (yyvsp[-4].intval), (yyvsp[-2].intval), FALSE, ss.c_str());
                    delete (yyvsp[0].str);
                  }
#line 10514 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1000:
#line 6003 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr),TERSrename((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr)),"#"); }
#line 10520 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1002:
#line 6008 "parser.y" /* yacc.c:1646  */
    {
		    string ss = *(yyvsp[-2].str) + '|' + *(yyvsp[0].str);
		    (yyval.str) = (yyval.str) = new string(ss);
                    delete (yyvsp[0].str);
		  }
#line 10530 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1004:
#line 6017 "parser.y" /* yacc.c:1646  */
    {
		    string ss = *(yyvsp[-2].str) + '&' + *(yyvsp[0].str);
		    (yyval.str) = new string(ss);
                    delete (yyvsp[0].str);
		  }
#line 10540 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1006:
#line 6025 "parser.y" /* yacc.c:1646  */
    { (yyval.str) = new string((yyvsp[0].actionptr)->label); }
#line 10546 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1007:
#line 6027 "parser.y" /* yacc.c:1646  */
    {
		    string ss = '~' + string((yyvsp[0].actionptr)->label);
		    (yyval.str) = new string(ss);
		  }
#line 10555 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1008:
#line 6032 "parser.y" /* yacc.c:1646  */
    {
		    string ss = '(' + *(yyvsp[-1].str) + ')';
		    (yyval.str) = new string( ss );
                  }
#line 10564 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1009:
#line 6037 "parser.y" /* yacc.c:1646  */
    {
		    string ss = '(' + *(yyvsp[-1].str) + ')';
		    ss = '~' + ss;
		    (yyval.str) = new string( ss );
                  }
#line 10574 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1010:
#line 6043 "parser.y" /* yacc.c:1646  */
    {
		    if((yyvsp[0].boolval) == TRUE)
		      (yyval.str) = new string("true");
		    else
		      (yyval.str) = new string("false");
		  }
#line 10585 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1011:
#line 6052 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = TERScompose((yyvsp[-2].TERSptr),(yyvsp[0].TERSptr),"||"); }
#line 10591 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1013:
#line 6057 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[0].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[0].actionptr),TRUE,(yyvsp[0].actionptr)->lower,(yyvsp[0].actionptr)->upper,FALSE,"[true]",
			      (yyvsp[0].actionptr)->dist);
		  }
#line 10601 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1014:
#line 6063 "parser.y" /* yacc.c:1646  */
    {
		    (yyvsp[-2].actionptr)->vacuous=FALSE;
		    (yyval.TERSptr) = TERS((yyvsp[-2].actionptr),TRUE,(yyvsp[-2].actionptr)->lower,(yyvsp[-2].actionptr)->upper,FALSE,"[true]",
			      (yyvsp[-2].actionptr)->dist,(yyvsp[0].intval));
		  }
#line 10611 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1015:
#line 6069 "parser.y" /* yacc.c:1646  */
    { (yyvsp[-1].actionptr)->vacuous=TRUE;
		    (yyval.TERSptr) = TERS((yyvsp[-1].actionptr),TRUE,(yyvsp[-1].actionptr)->lower,(yyvsp[-1].actionptr)->upper,FALSE,"[true]",
			      (yyvsp[-1].actionptr)->dist); }
#line 10619 "parser.tab.c" /* yacc.c:1646  */
    break;

  case 1016:
#line 6079 "parser.y" /* yacc.c:1646  */
    { (yyval.TERSptr) = (yyvsp[-1].TERSptr); }
#line 10625 "parser.tab.c" /* yacc.c:1646  */
    break;


#line 10629 "parser.tab.c" /* yacc.c:1646  */
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
#line 6084 "parser.y" /* yacc.c:1906  */





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
