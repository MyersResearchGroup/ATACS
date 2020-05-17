/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

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
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 133 "parser.y" /* yacc.c:1909  */

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

#line 268 "parser.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
