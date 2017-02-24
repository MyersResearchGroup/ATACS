
%{
#include <stdio.h>
#include <string.h>
#include "ctllex.c"
#include "ctlchecker.h"

int yyerror(char*);
int yylex();

//Needed for Bison 2.2 
#ifdef OSX
//extern char yytext[]; 
#endif 
//extern char *yytext; 
%}

%union {
  ctlstmt *property;
  char *var;
}

%token <var> VARIABLE
%left '|' '&'
%nonassoc '~' EX EG EU AF AG TR AU AX EF

%type <property> statement

%% 
statement: VARIABLE {$$ = newctlstmt(OP_VAR, NULL, NULL, strdup(yytext)); }
      |    TR { $$ = newctlstmt(OP_TRUE, NULL, NULL, NULL);}
      |    '~' statement {$$ = newctlstmt(OP_NOT, $2, NULL, NULL);}
      |    statement '|' statement {$$ = newctlstmt(OP_OR, $1, $3, NULL);}
      |    statement '&' statement {$$ = newctlstmt(OP_AND, $1, $3, NULL);}
      |    statement EU statement {$$ = newctlstmt(OP_EU, $1, $3, NULL);}
      |    EF statement {$$ = newctlstmt(OP_EU, 
             newctlstmt(OP_TRUE, NULL, NULL, NULL), $2, NULL);}
      |    statement AU statement {$$ = newctlstmt(OP_AND, 
           newctlstmt(OP_NOT, newctlstmt(OP_EU, 
             newctlstmt(OP_NOT, $3, NULL, NULL),
             newctlstmt(OP_AND, newctlstmt(OP_NOT, $1, NULL, NULL),
                                newctlstmt(OP_NOT, $3, NULL, NULL),NULL), 
             NULL), NULL, NULL), 
           newctlstmt(OP_NOT, newctlstmt(OP_EG, 
			 newctlstmt(OP_NOT, $3, NULL, NULL), NULL, NULL),
           NULL, NULL), NULL);}
      |    EX statement {$$ = newctlstmt(OP_EX, $2, NULL, NULL);}
      |    AX statement {$$ = newctlstmt(OP_NOT, 
              newctlstmt(OP_EX, newctlstmt(OP_NOT, $2, NULL, NULL), 
                NULL, NULL), NULL, NULL);}
      |    EG statement {$$ = newctlstmt(OP_EG, $2, NULL, NULL);}
      |    AF statement {$$ = newctlstmt(OP_NOT, newctlstmt(OP_EG, newctlstmt(OP_NOT, $2, NULL, NULL), NULL, NULL), NULL, NULL);}
      |    AG statement {$$ = newctlstmt(OP_NOT, newctlstmt(OP_EU, newctlstmt(OP_TRUE, NULL, NULL, NULL), newctlstmt(OP_NOT, $2, NULL, NULL), NULL), NULL, NULL);}
      |    '(' statement ')' {$$ = $2;}
      ;

%%

int yyerror(char* s)
{
  fprintf(stderr, "%s\n", s);
  return 0;
}

