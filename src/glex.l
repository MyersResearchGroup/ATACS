%{
#define YY_NO_UNPUT
     /* need this for the call to atof() below */

#include "gparse.h"

extern Node *yylval;
%} 

dot      [.]
digit    [0-9X]
symbol   [!?]
delim    [ \t]+
letter   [A-Za-z"_"]
id	 ({letter}|{symbol})({letter}|{digit}|{symbol}|{dot})*
number   {digit}+(\.{digit}+)?(E[+\-]?{digit}+)?
comments {delim}*#[^@\n]*
atacs {delim}*#@
/* comments {delim}*#.* */
integer  {digit}+
real     ({integer}\.{integer}|{integer}e{integer}|{integer}e-{integer}|{integer}E{integer}|{integer}E-{integer}|{integer}\.{integer}e{integer}|{integer}\.{integer}e-{integer}|{integer}\.{integer}E{integer}|{integer}\.{integer}E-{integer}|{integer}\.{integer}e\+{integer}|{integer}\.{integer}E\+{integer})

%%


{delim}			{}
{comments}		{}
{atacs}                 { return ATACS; }
\.end|\.END		{ return END; }
\.inputs|\.INPUTS 	{ return INPUTS; }
\.init\_state             { return INIT_STATE; }
\.dummy|\.DUMMY 	{ return DUMMYS; }
\.failtrans|\.FAILTRANS	{ return FAILTRANS; }
\.non_disabling|\.NON_DISABLING	{ return NONDISABLING; }
\.variables|\.VARIABLES { return VARIABLES; }
\.integers|\.INTEGERS   { return INTEGERS; }
\.noninp|\.NONINP       { return NONINPS; }
\.keep|\.KEEP 	        { return KEEPS; }
\.abstract|\.ABSTRACT   { return ABSTRACTS; }
\.continuous|\.CONTINUOUS { return CONTINUOUS; }
\.rates|\.RATES         { return RATES; }
\.init_rates|\.INIT_RATES   { return INITRATES; }
\.init_vals|\.INIT_VALS     { return INITVALS; }
\.init_ints|\.INIT_INTS     { return INITINTS; }
\.invariants|\.INVARIANTS { return INVARIANTS; }
\.enablings|\.ENABLINGS   { return ENABLINGS; }
\.assignments|\.ASSIGNMENTS { return ASSIGNS; }
\.transition_rates|\.TRANSITION_RATES  { return TRANS_RATES;}
\.rate_assignments|\.RATE_ASSIGNMENTS { return RATE_ASSIGNS; }
\.int_assignments|\.INT_ASSIGNMENTS { return INT_ASSIGNS; }
\.delay_assignments|\.DELAY_ASSIGNMENTS { return DELAY_ASSIGNS; }
\.priority_assignments|\.PRIORITY_ASSIGNMENTS { return PRIORITY_ASSIGNS; }
\.boolean_assignments|\.BOOLEAN_ASSIGNMENTS { return BOOL_ASSIGNS; }
\.outputs|\.OUTPUTS 	{ return OUTPUTS; }
\.internal|\.INTERNAL 	{ return INTERNAL; }
\.graph|\.GRAPH   	{ return GRAPH; }
\.property|\.PROPERTY	{ return PROPERTY; }
\.marking|\.MARKING	{ return MARKING; }
\.name|\.NAME		{ return NAME; }
"uniform"               { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return UNIFORM; }
"normal"                { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return NORMAL; }   
"exponential"           { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return EXPONENTIAL; }   
"gamma"                 { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return GAMMA; }    
"lognormal"             { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return LOGNORMAL; }   
"chisq"                 { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return CHISQ; }   
"laplace"               { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return LAPLACE; }   
"cauchy"                { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return CAUCHY; }   
"rayleigh"              { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return RAYLEIGH; }   
"poisson"               { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return POISSON; }   
"binomial"              { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return BINOMIAL; }   
"bernoulli"             { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return BERNOULLI; }   
"NOT"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return NOT; }   
"OR"                    { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return OR; }   
"AND"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return AND; }   
"XOR"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return XOR; }   
"INT"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return INT; }   
"BOOL"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return BOOL; }   
"BIT"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return BIT; }   
"rate"                  { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return RATE; }   
  /*"round"                 { yylval = (Node*)GetBlock (strlen(yytext)+1);*/
  /*			  strcpy ( (char*)yylval, yytext );*/
  /*                          return ROUND; } */  
"ceil"                  { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  			  strcpy ( (char*)yylval, yytext );
                          return CEIL; }   
"floor"                 { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  			  strcpy ( (char*)yylval, yytext );
                          return FLOOR; }
"min"                 { yylval = (Node*)GetBlock (strlen(yytext)+1);
	  	  			    strcpy ( (char*)yylval, yytext );
                        return MINF; }
"max"                 { yylval = (Node*)GetBlock (strlen(yytext)+1);
	  	  			    strcpy ( (char*)yylval, yytext );
                        return MAXF; }                       
"idiv"                { yylval = (Node*)GetBlock (strlen(yytext)+1);
	  	        strcpy ( (char*)yylval, yytext );
                        return IDIV; }                       
"verify"                { return VERIFY; }   
"search"                { return SEARCH; }
":="                    { return ASSIGN; }
"=>"                    { return IMPLIC; }
","                     { return yytext[0]; } 
"?"                     { return yytext[0]; }  
"="                     { return yytext[0]; }  
"{"                     { return yytext[0]; }   
"<"                     { return yytext[0]; }
"FALSE"                 { return BOOL_FALSE; }
"TRUE"                  { return BOOL_TRUE; }
"AU"                    { return AU; }
"EU"                    { return EU; }
"EG"                    { return EG; }
"EF"                    { return EF; }
"AG"                    { return AG; }
"AF"                    { return AF; }
"PG"                    { return PG; }
"PF"                    { return PF; }
"PU"                    { return PU; }
"PX"			{ return PX; }
"Pr"			{ return Pr; }
"St"			{ return Ss; }
"["                     { return yytext[0]; }   
"]"                     { return yytext[0]; }   
">"                     { return yytext[0]; }
"}"                     { return yytext[0]; }   
"-"                     { return yytext[0]; }   
")d"                    { return DISABLE; }   
"inf"                   { yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
                          return INF; }   
{id}	    		{ yylval = (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext ); 
		 	  return ID; }
{real}		        { yylval =  (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
			  return REAL;
			}	  		
{integer}		{ yylval =  (Node*)GetBlock (strlen(yytext)+1);
			  strcpy ( (char*)yylval, yytext );
			  return INTEGER;
			}	  		
\n		   	{ line_num ++; return yytext[0];}
. 			{ return yytext[0]; }
%%

int yywrap () {
    return 1;
}


