/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
