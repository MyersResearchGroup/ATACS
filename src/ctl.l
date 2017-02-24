%{
#define YY_NO_UNPUT
#include "ctlchecker.h"
#include "ctlparse.h"
%}

%{
#undef YY_INPUT
#define YY_INPUT(b, r, ms) (r = my_yyinput(b, ms))
int my_yyinput(char *buf, int max_size);
%}

LETTER		[a-zA-Z]
DIGIT		[0-9]
ID 	        ({LETTER}({LETTER}|{DIGIT}|_)*)

%%
[ \t];                         /* ignore whitespace */
\n      return 0;              /* logical EOF */
"EX"    return EX;
"EG"    return EG;
"EF"    return EF;
"EU"    return EU;
"AU"    return AU;
"AX"    return AX;
"AF"    return AF;
"AG"    return AG;
"true"  return TR;
{ID}    return VARIABLE;
.       return yytext[0];

%%

extern char prop[];
extern char *propptr; /* current position in myinput */
extern int proplim;  /* end of data */

int yywrap() {
  return 1;
}

int my_yyinput(char *buf, int max_size) 
{
  int n = max_size;
  if (max_size > (intptr_t)proplim - (intptr_t)propptr)
    n = (intptr_t)proplim - (intptr_t)propptr;
  
  if (n > 0) {
    memcpy(buf, propptr, n);
    propptr += n;
  }
  return n;
}
