%{
#include "symbolic.h"
#include "tctlparse.h"

#define YY_NO_UNPUT

#undef YY_INPUT
#define YY_INPUT(b, r, ms) (r = tctl_yyinput(b, ms))
int tctl_yyinput(char *buf, int max_size);
%}


%%
[ \t] ;              /* ignore whitespace */
\n      return 0;    /* logical EOF */
"EU"    return EU;
"AU"    return AU;
"EF"    return EF;
"AG"    return AG;
"AF"    return AF;
"EG"    return EG;
"true"  return TR;
"false" return FA;
">="   return GEQ;
">"    return GT;
"<="   return LEQ;
"<"    return LT;
"imp"   return IMP;
-?(([0-9]+)|([0-9]*\.[0-9]+)) {
  yylval.number = atof(yytext);
  return CONSTANT;
}
[a-zA-Z][a-zA-Z0-9_]* {
  yylval.var = strdup(yytext);
  return VARIABLE;
}
.     return yytext[0];

%%

extern char tctlprop[];
extern char *tctlpropptr; /* current position in tctlprop */
extern int tctlproplim; /*end of data */

int tctl_yyinput(char *buf, int max_size) {
  int n = max_size;
#ifdef CUDD
  if (max_size > (intptr_t)tctlproplim - (intptr_t)tctlpropptr)
    n = (intptr_t)tctlproplim - (intptr_t)tctlpropptr;

  if (n > 0) {
    memcpy(buf, tctlpropptr, n);
    tctlpropptr += n;
  }
#endif
  return n;
}

int yywrap() {
  return 1;
}
