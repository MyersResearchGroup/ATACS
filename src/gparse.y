%{
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
%}


%token IDIV MINF MAXF FLOOR CEIL COMMENTS ID INPUTS OUTPUTS INTERNAL GRAPH END INTEGER MARKING NAME INIT_STATE DUMMYS ATACS DISABLE NOT VERIFY SEARCH REAL INF KEEPS NONINPS ABSTRACTS CONTINUOUS RATES PROPERTY INVARIANTS ENABLINGS ASSIGNS RATE_ASSIGNS DELAY_ASSIGNS PRIORITY_ASSIGNS BOOL_ASSIGNS INT_ASSIGNS BOOL_FALSE BOOL_TRUE ASSIGN VARIABLES INITRATES INITVALS INITINTS Pr Ss AU EU EG EF AG AF PG PF PU PX INTEGERS UNIFORM NORMAL EXPONENTIAL GAMMA LOGNORMAL CHISQ LAPLACE CAUCHY RAYLEIGH POISSON BINOMIAL BERNOULLI AND OR XOR IMPLIC TRANS_RATES FAILTRANS INT BIT RATE BOOL NONDISABLING


%%
input		: input line
                | line 
		;


line		: '\n'	
		| END '\n'	
		| GRAPH '\n'	
		| edges '\n'
                | node '\n' /* Transitionless place */
                | NAME ID '\n' { free($2); }
		| INPUTS inputs '\n'
		| OUTPUTS outputs '\n'
		| INTERNAL internals '\n'
		| ATACS VARIABLES variables '\n'
                | DUMMYS dummys '\n'
                | ATACS DUMMYS dummys '\n'
                | ATACS FAILTRANS failtrans '\n'
                | ATACS NONDISABLING nondisabling '\n'
                | ATACS KEEPS keeps '\n'
                | ATACS ABSTRACTS abstracts '\n'
                | ATACS NONINPS noninps '\n'
                | ATACS CONTINUOUS continuous '\n'
                | ATACS PROPERTY property '\n'
                | ATACS INIT_STATE init_state '\n' 
		| ATACS MARKING marking '\n'
		| ATACS INITRATES init_rates '\n'
		| ATACS INITVALS init_vals '\n'
		| ATACS RATES rates '\n'
		| ATACS INVARIANTS invariants '\n'
		| ATACS ENABLINGS enables '\n'
		| ATACS ASSIGNS assigns '\n'
		| ATACS RATE_ASSIGNS rate_assigns '\n'
		| ATACS DELAY_ASSIGNS delay_assigns '\n'
		| ATACS PRIORITY_ASSIGNS priority_assigns '\n'
		| ATACS TRANS_RATES transition_rates '\n'
		| ATACS BOOL_ASSIGNS bool_assigns '\n'
		| MARKING marking '\n'
                | ATACS VERIFY '{' vevents '}' '\n'
                | ATACS SEARCH 
                  {
		    $$ = Enter_IO (DUMMY_NODE, "dummy");
		    Node_Ptr n = Enter_Place ("dummy_p");
		    Enter_Pred (n, $$);
		    Enter_Succ (n,$$,0,INFIN,0,(-1)*INFIN,INFIN,NULL,
				NULL,NULL,false,0.0,1);
		  }       '{' sevents '}' '\n'
		;

inputs		: 
		| inputs ID
	          {
		    $$ = Enter_IO (INPUT_NODE, (char*)$2);
		  }
		;

outputs		:
		| outputs ID
	          {
		    $$ = Enter_IO (OUTPUT_NODE, (char*)$2);
		  }
		;

internals	:
		| internals ID
	          {
		    $$ = Enter_IO (INTERNAL_NODE, (char*)$2);
		  }
		;

variables       :
                | variables ID
                  {
		    $$ = Enter_Variable((char*)$2);
		  }
                ;

variable        : ID
                  {
		    $$ = Enter_Variable((char*)$1);
		  }
                ;

dummys		: 
		| dummys ID
	          {
		    $$ = Enter_IO (DUMMY_NODE, (char*)$2);
		  }
		| dummys ID '+'
	          {
		    $$ = Enter_IO (DUMMY_NODE, strcat((char*)$2,"+"));
		  }
		| dummys ID '-'
	          {
		    $$ = Enter_IO (DUMMY_NODE, strcat((char*)$2,"-"));
		  }
		;

failtrans	: 
		| failtrans ID
	          {
		    Enter_FailTrans ((char*)$2);
		  }
                ;

nondisabling	: 
		| nondisabling ID
	          {
		    Enter_NonDisabling ((char*)$2);
		  }
                ;

keeps		: 
		| keeps ID
	          {
		    Enter_Keep ((char*)$2);
		    free($2);
		  }
		;

abstracts	: 
		| abstracts ID
	          {
		    Enter_Abstract ((char*)$2);
		    free($2);
		  }
		;

noninps		: 
		| noninps ID
	          {
		    Enter_NonInp ((char*)$2);
		    free($2);
		  }
		;

continuous	: 
		| continuous ID
	          {
		    Enter_Cont ((char*)$2,(-1)*INFIN,INFIN);
		    free($2);
		  }
		| continuous ID '[' '-' INF ',' '-' INTEGER ']'
                  {
		    Enter_Cont ((char*)$2,(-1)*INFIN,(-1)*atoi((char*)$8));
		    free($2);
		    free($8);
		  }
		| continuous ID '[' '-' INTEGER ',' '-' INTEGER ']'
	          {
		    Enter_Cont ((char*)$2,(-1)*atoi((char*)$5),
				(-1)*atoi((char*)$8));
		    free($2);
		    free($5);
		    free($8);
		  }
		| continuous ID '[' '-' INF ',' INTEGER ']'
	          {
		    Enter_Cont ((char*)$2,(-1)*INFIN,atoi((char*)$7));
		    free($2);
		    free($7);
		  }
		| continuous ID '[' '-' INTEGER ',' INTEGER ']'
	          {
		    Enter_Cont ((char*)$2,(-1)*atoi((char*)$5),
				atoi((char*)$7));
		    free($2);
		    free($5);
		    free($7);
		  }
		| continuous ID '[' INTEGER ',' INTEGER ']'
	          {
		    Enter_Cont ((char*)$2,atoi((char*)$4),atoi((char*)$6));
		    free($2);
		    free($4);
		    free($6);
		  }
		| continuous ID '[' '-' INF ',' INF ']'
	          {
		    Enter_Cont ((char*)$2,(-1)*INFIN,INFIN);
		    free($2);
		  }
		| continuous ID '[' '-' INTEGER ',' INF ']'
	          {
		    Enter_Cont ((char*)$2,(-1)*atoi((char*)$5),INFIN);
		    free($2);
		    free($5);
		  }
		| continuous ID '[' INTEGER ',' INF ']'
	          {
		    Enter_Cont ((char*)$2,atoi((char*)$4),INFIN);
		    free($2);
		    free($4);
		  }
		;
property	: probproperty
	          {
		    Enter_Prop((char*)$1);
		    free($1);
		  }
		| props
	          {
		    Enter_Prop((char*)$1);
		    free($1);
		  }
		;

probproperty	: hsf
                | Pr relop REAL '{' probprop '}'
		{
		  $$ = (Node*)GetBlock (strlen((char*)$2)+strlen((char*)$3)+
					strlen((char*)$5)+5);
		  strcpy((char*)$$,"Pr");
		  strcat((char*)$$, (char*)$2);
		  strcat((char*)$$, (char*)$3);
		  strcat((char*)$$, "{");
		  strcat((char*)$$, (char*)$5);
		  strcat((char*)$$, "}");
		  free($2);
		  free($3);
		  free($5); 
		}
		| Pr '=' '?' '{' probprop '}'
		{
		  $$ = (Node*)GetBlock (strlen((char*)$5)+7);
		  strcpy((char*)$$,"Pr");
		  strcat((char*)$$,"=");
		  strcat((char*)$$,"?");
		  strcat((char*)$$, "{");
		  strcat((char*)$$, (char*)$5);
		  strcat((char*)$$, "}");
		  free($5); 
		}
		| Ss relop REAL '{' probproperty '}'
		{
		  $$ = (Node*)GetBlock (strlen((char*)$2)+strlen((char*)$3)+
					strlen((char*)$5)+5);
		  strcpy((char*)$$,"SS");
		  strcat((char*)$$, (char*)$2);
		  strcat((char*)$$, (char*)$3);
		  strcat((char*)$$, "{");
		  strcat((char*)$$, (char*)$5);
		  strcat((char*)$$, "}");
		  free($2);
		  free($3);
		  free($5); 
		}
		| Ss '=' '?' '{' probproperty '}'
		{
		  $$ = (Node*)GetBlock (strlen((char*)$5)+7);
		  strcpy((char*)$$,"SS");
		  strcat((char*)$$,"=");
		  strcat((char*)$$,"?");
		  strcat((char*)$$, "{");
		  strcat((char*)$$, (char*)$5);
		  strcat((char*)$$, "}");
		  free($5); 
		}
		;

probprop        : PG bound '(' probproperty ')'
                  {
		    int boundlen = 0;
		    if ($2) boundlen = strlen((char*)$2);
		    $$ = (Node*)GetBlock(boundlen+
					 strlen((char*)$4)+5);
		    strcat((char*)$$,"PG(");
		    if ($2) strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,(char*)$4);
		    strcat((char*)$$,")");
		    if ($2) free($2);
		    free($4);
		  }
                | PF bound '(' probproperty ')'
                  {
		    int boundlen = 0;
		    if ($2) boundlen = strlen((char*)$2);
		    $$ = (Node*)GetBlock(boundlen+
					 strlen((char*)$4)+5);
		    strcat((char*)$$,"PF(");
		    if ($2) strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,(char*)$4);
		    strcat((char*)$$,")");
		    if ($2) free($2);
		    free($4);
		  }
		| PX '(' probproperty ')'
                  {
		    $$ = (Node*)GetBlock(strlen((char*)$3)+5);
		    strcat((char*)$$,"PX(");
		    strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,")");
		    free($3);
		  }  
                | probproperty PU bound probproperty 
                  {
		    int boundlen = 0;
		    if ($3) boundlen = strlen((char*)$3);
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  boundlen+4+
					  strlen((char*)$4));
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"PU");
		    if ($3) strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,(char*)$4);
		    free($1);
		    if ($3) free($3);
		    free($4);
		  }
                ;

props           : prop andprop
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"&");
		    strcat((char*)$$,(char*)$2);
		    free($1);
		    free($2);
		  }
                | prop orprop
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"|");
		    strcat((char*)$$,(char*)$2);
		    free($1);
		    free($2);
		  }
		| prop impliesprop
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+3);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"->");
		    strcat((char*)$$,(char*)$2);
		    free($1);
		    free($2);
		  }
                | prop
                ;

andprop         : andprop '&' prop
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$3)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"&");
		    strcat((char*)$$,(char*)$3);
		    free($1);
		    free($3);
		  }
                | '&' prop
                  {
		   $$ = $2;
		  }
                ;
orprop          : orprop '|' prop
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$3)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"|");
		    strcat((char*)$$,(char*)$3);
		    free($1);
		    free($3);
		  }
                | '|' prop
                  {
		   $$ = $2;
		  }
                ;

impliesprop     : impliesprop '-' '>' prop
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$4)+3);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"->");
		    strcat((char*)$$,(char*)$4);
		    free($1);
		    free($4);
                  }
                | '-' '>' prop
                  {
		    $$ = $3;
		  }
                ;



prop            : fronttype bound '(' prop ')'
                  {
		    int boundlen = 0;
		    if ($2) boundlen = strlen((char*)$2);
		    $$ = (Node*)GetBlock(strlen((char*)$1)+
					 boundlen+
					 strlen((char*)$4)+3);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"(");
		    if ($2) strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,(char*)$4);
		    strcat((char*)$$,")");
		    if ($2) free($2);
		    free($4);
		  }
                | fronttype bound '('  hsf ')'
		  {
		    
		    int boundlen = 0;
		    if ($2) boundlen = strlen((char*)$2);
		    $$ = (Node*)GetBlock(strlen((char*)$1)+
					 boundlen+
					 strlen((char*)$4)+3);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"(");
		    if ($2) strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,(char*)$4);
		    strcat((char*)$$,")");
		    if($2) free($2);
		    free($4);
		  }
                | midprop
		;

midprop         : '{' propinner '}'
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$2)+3);
		    strcpy((char*)$$,"{");
		    strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,"}");
		    free($2);
		  }
                ;

propinner       : prop midtype bound prop
                  {
		    int boundlen = 0;
		    if ($3) boundlen = strlen((char*)$3);
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+
					  boundlen+1+
					  strlen((char*)$4));
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,(char*)$2);
		    if ($3) strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,(char*)$4);
		    free($1);
		    if ($3) free($3);
		    free($4);
		  }
                | prop midtype bound hsf
                  {
		    int boundlen = 0;
		    if ($3) boundlen = strlen((char*)$3);
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+
					  boundlen+1+
					  strlen((char*)$4));
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,(char*)$2);
		    if ($3) strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,(char*)$4);
		    free($1);
		    if ($3) free($3);
		    free($4);
		  }
                | hsf midtype bound prop
                  {
		    int boundlen = 0;
		    if ($3) boundlen = strlen((char*)$3);
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+
					  boundlen+1+
					  strlen((char*)$4));

		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,(char*)$2);
		    if ($3) strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,(char*)$4);
		    free($1);
		    if ($3) free($3);
		    free($4);
		  }
                | hsf midtype bound hsf
                  {
		    int boundlen = 0;
		    if ($3) boundlen = strlen((char*)$3);
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$2)+
					  boundlen+1+
					  strlen((char*)$4));
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,(char*)$2);
		    if ($3) strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,(char*)$4);
		    free($1);
		    if ($3) free($3);
		    free($4);
		  }
                ;

fronttype       : AG {$$=(Node*)CopyString("AG");}
                | AF {$$=(Node*)CopyString("AF");}
                | EG {$$=(Node*)CopyString("EG");}
                | EF {$$=(Node*)CopyString("EF");}
                ;

midtype         : AU {$$=(Node*)CopyString("AU");} 
                | EU {$$=(Node*)CopyString("EU");}
                ;

bound           : '[' relop hsf ']'
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$2)+
					  strlen((char*)$3)+3);
		    strcpy((char*)$$,"[");
		    strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,"]");
		    free($2);
		    free($3);
		  }
                | '[' hsf ',' hsf ']'
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$2)+
					  strlen((char*)$4)+4);
		    strcpy((char*)$$,"[");
		    strcat((char*)$$,(char*)$2);
		    strcpy((char*)$$,",");
		    strcat((char*)$$,(char*)$4);
		    strcat((char*)$$,"]");
		    free($2);
		    free($4);
		  }
                | { $$ = NULL;}
                ; 

init_state      : '[' INTEGER ']'
	          {
		    Set_Initial_State((char*)$2);
		    free($2);
		  }
                | '[' ID ']'
	          {
		    Set_Initial_State((char*)$2);
		    free($2);
		  }
                | '[' ']'
		;

place		: ID
	          {
		    $$ = Enter_Place ((char*)$1);
		  }
		;

marking		: '{' marking_set '}'
		;

marking_set	: marking_set mark
		| mark
		;
	
mark		: '<' signal_trans ',' signal_trans '>'
	          {
		    Enter_Mark (&initial_marking, EDGE_MARK, $2, $4, 1, 1);
		  }
		| '<' place ',' signal_trans '>'
	          {
		    Enter_Mark (&initial_marking, EDGE_MARK, $2, $4, 1, 1);
		  }
		| '<' signal_trans ',' place '>'
	          {
		    Enter_Mark (&initial_marking, EDGE_MARK, $2, $4, 1, 1);
		  }
		| '<' place ',' place '>'
	          {
		    Enter_Mark (&initial_marking, EDGE_MARK, $2, $4, 1, 1);
		  }
		| place 
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL, $1, 1, 1);
		  }
		| '<' place '=' INTEGER '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				$2,atoi((char*)$4),
				atoi((char*)$4));
		    free($4);
		  }
		| '<' place '=' '-' INTEGER '>'
                  {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,$2,
				(-1)*atoi((char*)$4),(-1)*atoi((char*)$4));
		    free($4);
		  }
		| '<' place '=' '[' '-' INF ',' '-' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				$2,(-1)*INFIN,
				(-1)*atoi((char*)$9));
		    free($9);
		  }
		| '<' place '=' '[' '-' INTEGER ',' '-' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,$2,
				(-1)*atoi((char*)$6),(-1)*atoi((char*)$9));
		    free($6);
		    free($9);
		  }
		| '<' place '=' '[' '-' INF ',' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				$2,(-1)*INFIN,
				(-1)*atoi((char*)$8));
		    free($8);
		  }
		| '<' place '=' '[' '-' INTEGER ',' INTEGER ']' '>'
                  {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,$2,
				(-1)*atoi((char*)$6),(-1)*atoi((char*)$8));
		    free($6);
		    free($8);
		  }
		| '<' place '=' '[' INTEGER ',' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				$2,atoi((char*)$5),
				atoi((char*)$7));
		    free($5);
		    free($7);
		  }
		| '<' place '=' '[' '-' INF ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,$2,
				(-1)*INFIN,INFIN);
		  }
		| '<' place '=' '[' '-' INTEGER ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,$2,
				(-1)*atoi((char*)$6),INFIN);
		    free($6);
		  }

		| '<' place '=' '[' INTEGER ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, PLACE_MARK, NULL,
				$2,atoi((char*)$5),
				INFIN);
		    free($5);
		  }
		;

init_rates      : '{' init_rates_set '}'
                ;

init_rates_set  : init_rates_set init_rate
| /*init_rate*/
                ;

init_rate       : '<' variable '=' INTEGER '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				$2,atoi((char*)$4),atoi((char*)$4));
		    free($4);
		  }
		| '<' variable '=' '-' INTEGER '>'
                  {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,$2,
				(-1)*atoi((char*)$5),(-1)*atoi((char*)$5));
		    free($4);
		  }
		| '<' variable '=' '[' '-' INF ',' '-' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				$2,(-1)*INFIN,(-1)*atoi((char*)$9));
		    free($9);
		  }
		| '<' variable '=' '[' '-' INTEGER ',' '-' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,$2,
				(-1)*atoi((char*)$6),(-1)*atoi((char*)$9));
		    free($6);
		    free($9);
		  }
		| '<' variable '=' '[' '-' INF ',' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				$2,(-1)*INFIN,(-1)*atoi((char*)$8));
		    free($8);
		  }
		| '<' variable '=' '[' '-' INTEGER ',' INTEGER ']' '>'
                  {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,$2,
				(-1)*atoi((char*)$6),(-1)*atoi((char*)$8));
		    free($6);
		    free($8);
		  }
		| '<' variable '=' '[' INTEGER ',' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				$2,atoi((char*)$5),atoi((char*)$7));
		    free($5);
		    free($7);
		  }
		| '<' variable '=' '[' '-' INF ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,$2,
				(-1)*INFIN,INFIN);
		  }
		| '<' variable '=' '[' '-' INTEGER ':' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,$2,
				(-1)*atoi((char*)$6),INFIN);
		    free($6);
		  }

		| '<' variable '=' '[' INTEGER ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, RATE_MARK, NULL,
				$2,atoi((char*)$5),INFIN);
		    free($5);
		  }

                ;

init_vals       : '{' init_vals_set '}'
                ;

init_vals_set   : init_vals_set init_val
| /*init_val*/
                ;

init_val        : '<' variable '=' INTEGER '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				$2,atoi((char*)$4),
				atoi((char*)$4));
		    free($4);
		  }
		| '<' variable '=' '-' INTEGER '>'
                  {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,$2,
				(-1)*atoi((char*)$5),(-1)*atoi((char*)$5));
		    free($4);
		  }
		| '<' variable '=' '[' '-' INF ',' '-' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				$2,(-1)*INFIN,
				(-1)*atoi((char*)$9));
		    free($9);
		  }
		| '<' variable '=' '[' '-' INTEGER ',' '-' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,$2,
				(-1)*atoi((char*)$6),(-1)*atoi((char*)$9));
		    free($6);
		    free($9);
		  }
		| '<' variable '=' '[' '-' INF ',' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				$2,(-1)*INFIN,
				(-1)*atoi((char*)$8));
		    free($8);
		  }
		| '<' variable '=' '[' '-' INTEGER ',' INTEGER ']' '>'
                  {
		    Enter_Mark (&initial_marking,VAR_MARK, NULL,$2,
				(-1)*atoi((char*)$6),(-1)*atoi((char*)$8));
		    free($6);
		    free($8);
		  }
		| '<' variable '=' '[' INTEGER ',' INTEGER ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				$2,atoi((char*)$5),
				atoi((char*)$7));
		    free($5);
		    free($7);
		  }
		| '<' variable '=' '[' '-' INF ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,$2,
				(-1)*INFIN,INFIN);
		  }
		| '<' variable '=' '[' '-' INTEGER ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,$2,
				(-1)*atoi((char*)$6),INFIN);
		    free($6);
		  }

		| '<' variable '=' '[' INTEGER ',' INF ']' '>'
	          {
		    Enter_Mark (&initial_marking, VAR_MARK, NULL,
				$2,atoi((char*)$5),
				INFIN);
		    free($5);
		  }
                ;

rates		: '{' rate_set '}'
		;

rate_set	: rate_set trate
| /*trate*/
		;
	
trate		: '<' place '=' INTEGER '>'
	          {
		    Enter_Trate ($2,atoi((char*)$4),atoi((char*)$4));
		    free($4);
		  } 
                | '<' place '=' '[' INTEGER ',' INTEGER ']' '>'
	          {
		    Enter_Trate ($2,atoi((char*)$5),atoi((char*)$7));
		    free($5);
		    free($7);
		  }
		;

invariants	: '{' invariant_set '}'
		;

invariant_set	: invariant_set invariant
| /*invariant*/
		;
	
invariant	: '<' place '=' '[' hsf_ineqs']' '>'
                { 
		   string hsfs = (char*)$5;
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
		  if (!$2->inequalities) {
		    $2->inequalities = ineqs;
		  }
		  else {
		    for (ineqADT step = $2->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = temp;
			break;
		      }
		    }
		  }
		  Enter_Hsl($2,(char*)$5);
		  free($5);
	        }
                ;

edges		: edge
	          {
		  }	    
		;

edge		: edge node
	          {
		    $$ = $1;
		    Enter_Pred ($1, $2);
		    Enter_Succ ($1, $2, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL,
				NULL,NULL,false, 1.0, 1);
		  }
		| node node 
	          {
		    $$ = $1;
		    Enter_Pred ($1, $2);
		    Enter_Succ ($1, $2, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL,
				NULL,NULL, false, 1.0, 1);
		  }
		| node node ATACS cons expr plow pup lower upper rate weight
	          {
		    $$ = $1;
		    int predtype=0;
		    int plower=-INFIN;
		    if ($6) {
		      if (strchr((char*)$6,'\\'))
			predtype=2;
		      *(((char*)$6)+strlen((char*)$6)-1)='\0';
		      if (strcmp((char*)$6,"-inf")!=0)
			plower=atoi((char*)$6);
		      free($6);
		    }
		    int pupper=INFIN;
		    if ($7) { 
		      if (strchr((char*)$7,'/'))
			predtype=predtype+1;
		      *(((char*)$7)+strlen((char*)$7)-1)='\0';
		      if (strcmp((char*)$7,"inf")!=0)
			pupper=atoi((char*)$7);
		      free($7);
		    }
		    int lower=0;
		    if ($8) {
		      lower=atoi((char*)$8);
		      free($8);
		    }
		    int upper=INFIN;
		    if ($9) {
		      if (strcmp((char*)$9,"inf")!=0)
			upper=atoi((char*)$9);
		      free($9);
		    }
		    double rate=1.0;
		    if ($10) {
		      rate=atof((char*)$10);
		      free($10);
		    }
		    int weight=1;
		    if ($11) {
		      weight=atoi((char*)$11);
		      free($11);
		    }
		    Enter_Pred ($1, $2);
		    Enter_Succ ($1, $2,lower,upper,predtype,plower,pupper,
				(char*)$5, NULL,(char*)$4,false,rate,weight);
		  }
		| node node ATACS cons exprd plow pup lower upper rate weight
	          {
		    $$ = $1;
		    int predtype=0;
		    int plower=-INFIN;
		    if ($6) {
		      if (strchr((char*)$6,'\\'))
			predtype=2;
		      *(((char*)$6)+strlen((char*)$6)-1)='\0';
		      if (strcmp((char*)$6,"-inf")!=0)
			plower=atoi((char*)$6);
		      free($6);
		    }
		    int pupper=INFIN;
		    if ($7) {
		      if (strchr((char*)$7,'/'))
			predtype=predtype+1;
		      *(((char*)$7)+strlen((char*)$7)-1)='\0';
		      if (strcmp((char*)$7,"inf")!=0)
			pupper=atoi((char*)$7);
		      free($7);
		    }
		    int lower=0;
		    if ($8) {
		      lower=atoi((char*)$8);
		      free($8);
		    }
		    int upper=INFIN;
		    if ($9) {
		      if (strcmp((char*)$9,"inf")!=0)
			upper=atoi((char*)$9);
		      free($9);
		    }
		    double rate=1.0;
		    if ($10) {
		      rate=atof((char*)$10);
		      free($10);
		    }
		    int weight=1;
		    if ($11) {
		      weight=atoi((char*)$11);
		      free($11);
		    }
		    Enter_Pred ($1, $2);
		    Enter_Succ ($1, $2,lower,upper,predtype,plower,pupper,
				(char*)$5,NULL,(char*)$4,true,rate,weight);
		  }
		;

cons            : { $$ = NULL; }
                | '{' ID '}' { $$ = $2; }
                ;
expr            : { $$ = NULL; }
                | '(' hsf ')' { $$ = $2; }
/* | '<' ID ';' ineqs '>' { $$ = $2; } */
                ;

hsf             : hsf '|' andexpr 
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$3)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"|");
		    strcat((char*)$$,(char*)$3);
		    free($1);
		    free($3);
		  }
                | hsf IMPLIC andexpr
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$3)+3);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"->");
		    strcat((char*)$$,(char*)$3);
		    free($1);
		    free($3);
		  }
                | andexpr
                ;

andexpr        : andexpr '&' relation
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+2);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"&");
		   strcat((char*)$$,(char*)$3);
		   free($1);
		   free($3);
                 }
               | relation 
               ;

relation        : '~' relation
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$2)+2);
		    strcpy((char*)$$,"~");
		    strcat((char*)$$,(char*)$2);
		    free($2);
		  }
                | BIT '(' arithexpr ',' arithexpr ')'
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$3)+
					 strlen((char*)$5)+7);
		   strcpy((char*)$$,"BIT(");
		   strcat((char*)$$,(char*)$3);
		   strcat((char*)$$,",");
		   strcat((char*)$$,(char*)$5);
		   strcat((char*)$$,")");
		   free($3);
		   free($5);
                 }
                | arithexpr relop arithexpr
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$2)+
					 strlen((char*)$3)+1);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,(char*)$2);
		   strcat((char*)$$,(char*)$3);
		   free($1);
		   free($3);
                 }
                | arithexpr
		;

arithexpr	: arithexpr '+' multexpr
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$3)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"+");
		    strcat((char*)$$,(char*)$3);
		    free($1);
		    free($3);
		  }
                | arithexpr '-' multexpr
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$1)+
					  strlen((char*)$3)+2);
		    strcpy((char*)$$,(char*)$1);
		    strcat((char*)$$,"-");
		    strcat((char*)$$,(char*)$3);
		    free($1);
		    free($3);
		  }
                | multexpr
                ;

multexpr        : multexpr ID //term
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$2)+2);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"*");
		   strcat((char*)$$,(char*)$2);
		   free($1);
		   free($2);
                 }
               | multexpr '*' term
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+2);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"*");
		   strcat((char*)$$,(char*)$3);
		   free($1);
		   free($3);
                 }
               | multexpr '/' term
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+2);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"/");
		   strcat((char*)$$,(char*)$3);
		   free($1);
		   free($3);
                 }
               | multexpr '%' term
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+2);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"%");
		   strcat((char*)$$,(char*)$3);
		   free($1);
		   free($3);
                 }
               | multexpr '^' term
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+2);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"^");
		   strcat((char*)$$,(char*)$3);
		   free($1);
		   free($3);
                 }
               | term
               ;

term            :'(' hsf ')'
		  {
		    $$ = (Node*)GetBlock (strlen((char*)$2)+3);
		    strcpy((char*)$$,"(");
		    strcat((char*)$$,(char*)$2);
		    strcat((char*)$$,")");
		    free($2);
		  }
                | RATE '(' ID ')'
		  {
		    $$ = (Node*)GetBlock (strlen((char*)$3)+7);
		    strcpy((char*)$$,"rate(");
		    strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,")");
		    free($3);
		  }
                | INT '(' hsf ')'
		  {
		    $$ = (Node*)GetBlock (strlen((char*)$3)+6);
		    strcpy((char*)$$,"INT(");
		    strcat((char*)$$,(char*)$3);
		    strcat((char*)$$,")");
		    free($3);
		  }
                | ID
		| '-' term 
                  {
		    $$ = (Node*)GetBlock (strlen((char*)$2)+2);
		    strcpy((char*)$$,"-");
		    strcat((char*)$$,(char*)$2);
		    free($2);
		  }
                | BOOL_FALSE
                  {
		    $$ = (Node*)GetBlock(6);
		    strcpy((char*)$$,"false");
                  }
		| BOOL_TRUE
                  {
		    $$ = (Node*)GetBlock(5);
		    strcpy((char*)$$,"true");
                  } 
                | unop '(' arithexpr ')'
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+3);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"(");
		   strcat((char*)$$,(char*)$3);
		   strcat((char*)$$,")");
		   free($1);
		   free($3);
                 }
                | binop '(' arithexpr ',' arithexpr ')'
                 {
		   $$ = (Node*)GetBlock (strlen((char*)$1)+
					 strlen((char*)$3)+
					 strlen((char*)$5)+4);
		   strcpy((char*)$$,(char*)$1);
		   strcat((char*)$$,"(");
		   strcat((char*)$$,(char*)$3);
		   strcat((char*)$$,",");
		   strcat((char*)$$,(char*)$5);
		   strcat((char*)$$,")");
		   free($1);
		   free($3);
		   free($5);
                 }
                | INTEGER
		| REAL
		| INF
                ;


relop           : '='     { $$ = (Node*)CopyString("=");  }
                | '<'     { $$ = (Node*)CopyString("<");  }
                | '<' '=' { $$ = (Node*)CopyString("<="); }
                | '>'     { $$ = (Node*)CopyString(">");  }
                | '>' '=' { $$ = (Node*)CopyString(">="); }
                ;

unop            : NOT
                | EXPONENTIAL
                | CHISQ
                | LAPLACE
                | CAUCHY
                | RAYLEIGH
                | POISSON
                | BERNOULLI
                | BOOL
                | FLOOR
                | CEIL
                ;

binop           : OR
                | AND
                | XOR
                | UNIFORM
                | NORMAL
                | GAMMA
                | LOGNORMAL
                | BINOMIAL
                | MINF
                | MAXF
                | IDIV
                ;


ineqs           : ineqs ';' ineq
                { 
		  $$ = $3;
		  ((ineqADT)$3)->next = (ineqADT)$1;
		}
                | ineq
                { $$ = $1; }
                ;

hsf_ineqs       : hsf_ineqs ';' hsf
                {
		  $$ = (Node*)GetBlock(strlen((char*)$1)+
				       strlen((char*)$3)+2);
		  strcpy((char*)$$, (char*)$1);
		  strcat((char*)$$, "&");
		  strcat((char*)$$, (char*)$3);
		  //free($1);
		  //free($3);
		}
                | hsf
                ;


rate_ineqs      : rate_ineqs ';' rate_ineq
                { 
		  $$ = $3;
		  ((ineqADT)$3)->next = (ineqADT)$1;
		}
                | rate_ineq
                {
		  $$ = $1;
		}
                ;

ineq            : place ASSIGN hsf
                {
		  $$ = (Node*)Enter_Inequality($1,5,-INFIN,INFIN,(char*)$3);
                }
                ;

rate_ineq       : place ASSIGN hsf
                {
		  $$ = (Node*)Enter_Inequality($1,6,-INFIN,INFIN,(char*)$3);
                }
                ;


exprd           : '(' hsf DISABLE { $$ = $2; }
                ;
plow            : { $$ = NULL; }
                | '/' INTEGER 
                {
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+2);
		  strcpy((char*)$$,(char*)$2);
                  strcat((char*)$$,"/");
                  free($2);
   	        }
                | '\\' INTEGER
                {
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+2);
		  strcpy((char*)$$,(char*)$2);
                  strcat((char*)$$,"\\");
                  free($2);
   	        }
                | '/' '-' INTEGER 
                { 
                  $$ =  (Node*)GetBlock (strlen((char*)$3)+3);
                  strcpy((char*)$$,"-");
		  strcat((char*)$$,(char*)$3);
                  strcat((char*)$$,"/");
                  free($3);
                }
                | '\\' '-' INTEGER 
                { 
                  $$ =  (Node*)GetBlock (strlen((char*)$3)+3);
                  strcpy((char*)$$,"-");
		  strcat((char*)$$,(char*)$3);
                  strcat((char*)$$,"\\");
                  free($3);
                }
                | '/' '-' INF
                { 
                  $$ =  (Node*)GetBlock (strlen((char*)$3)+3);
                  strcpy((char*)$$,"-");
		  strcat((char*)$$,(char*)$3);
                  strcat((char*)$$,"/");
                  free($3);
                }
                | '\\' '-' INF
                { 
                  $$ =  (Node*)GetBlock (strlen((char*)$3)+3);
                  strcpy((char*)$$,"-");
		  strcat((char*)$$,(char*)$3);
                  strcat((char*)$$,"\\");
                  free($3);
                }
                ;
pup             : { $$ = NULL; }
                | ';' '-' INTEGER '\\' 
                { 
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+3);
                  strcpy((char*)$$,"-");
		  strcat((char*)$$,(char*)$3);
                  strcat((char*)$$,"\\");
                  free($3);
                }
                | ';' '-' INTEGER '/' 
                { 
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+3);
                  strcpy((char*)$$,"-");
		  strcat((char*)$$,(char*)$3);
                  strcat((char*)$$,"/");
                  free($3);
                }
                | ';' INTEGER '\\' 
                {
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+2);
		  strcpy((char*)$$,(char*)$2);
                  strcat((char*)$$,"\\");
                  free($2);
   	        }
                | ';' INTEGER '/' 
                {
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+2);
		  strcpy((char*)$$,(char*)$2);
                  strcat((char*)$$,"/");
                  free($2);
   	        }
                | ';' INF '\\'
                {
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+2);
		  strcpy((char*)$$,(char*)$2);
                  strcat((char*)$$,"\\");
                  free($2);
   	        }
                | ';' INF '/' 
                {
                  $$ =  (Node*)GetBlock (strlen((char*)$2)+2);
		  strcpy((char*)$$,(char*)$2);
                  strcat((char*)$$,"/");
                  free($2);
   	        }
                ;
lower           : { $$ = NULL; }
                | '[' INTEGER { $$ = $2; }
                ;
upper           : { $$ = NULL; }
                | ',' INTEGER ']' { $$ = $2; }
                | ',' INF ']' { $$ = $2; }
                ;
rate            : { $$ = NULL; }
                | REAL { $$ = $1; }
                ;
weight          : { $$ = NULL; }
                | INTEGER { $$ = $1; }
                ;

node		: signal_trans
	          {
		    $$ = $1;
		  }
		| place
	          {
		    $$ = $1;
		  }
		;
	

signal_trans	: ID '+'	
	          {
		    $$ = Enter_Trans ((char*)$1, "", '+');
		    free($1);
		  }
		| ID '+' '/' INTEGER
	          {
		    $$ = Enter_Trans ((char*)$1, (char*)$4, '+');
		    free($1);
		    free($4);
		  }
		| ID '-'
	          {
		    $$ = Enter_Trans ((char*)$1, "", '-');
		    free($1);
		  }
		| ID '-' '/' INTEGER
	          {
		    $$ = Enter_Trans ((char*)$1, (char*)$4, '-');
		    free($1);
		    free($4);
		  }
		;

enables         : '{' enable_set '}'
		;

enable_set      : enable_set enable
| /*enable*/
		;
	
enable	        : '<' node '=' '[' hsf_ineqs ']' '>'
                {
		  string hsfs = (char*)$5;
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
		  if (!$2->inequalities) {
		    $2->inequalities = ineqs;
		  }
		  else {
		    for (ineqADT step = $2->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = temp;
			break;
		      }
		    }
		  }
		  Enter_Hsl($2,(char*)$5);
		  free($5);
		}
		;

assigns         : '{' assign_set '}'
		;

assign_set      : assign_set assign
| /*assign*/
		;
	
assign	        : '<' node '=' '[' ineqs ']' '>'
                {
		  if (!$2->inequalities) {
		    $2->inequalities = (ineqADT)$5;
		  }
		  else {
		    for (ineqADT step = $2->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = (ineqADT)$5;
			break;
		      }
		    }
		  }
		}
		;


rate_assigns    : '{' rate_assign_set '}'
                ;

rate_assign_set : rate_assign_set rate_assign
| /*rate_assign*/
                ;

rate_assign     : '<' node '=' '[' rate_ineqs ']' '>'
                {
		  if (!$2->inequalities) {
		    $2->inequalities = (ineqADT)$5;
		  }
		  else {
		    for (ineqADT step = $2->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = (ineqADT)$5;
			break;
		      }
		    }
		  }
                }

delay_assigns    : '{' delay_assign_set '}'
                 ;

delay_assign_set : delay_assign_set delay_assign
| /*delay_assign*/
                 ;

delay_assign     : '<' node '=' UNIFORM '(' INTEGER ',' INTEGER ')' '>'
                 {
		   Enter_Delay($2,atoi((char*)$6),atoi((char*)$8));
		 }
                 | '<' node '=' UNIFORM '(' INTEGER ',' INF ')' '>'
                 {
		   Enter_Delay($2,atoi((char*)$6),INFIN);
		 }
                 | '<' node '=' EXPONENTIAL '(' hsf ')' '>'
                 {
		   Enter_TransRate($2,(char*)$6);
		   free($5);
		 }
                 | '<' node '=' '[' INTEGER ',' INTEGER ']' '>'
                 {
		   Enter_Delay($2,atoi((char*)$5),atoi((char*)$7));
		 }
                 | '<' node '=' '[' INTEGER ',' INF ']' '>'
                 {
		   Enter_Delay($2,atoi((char*)$5),INFIN);
		 }
                 | '<' node '=' INTEGER '>'
                 {
		   Enter_Delay($2,atoi((char*)$4),atoi((char*)$4));
		 }
                 | '<' node '=' '[' hsf ']' '>'
                 {
		   Enter_DelayExpr($2,(char*)$5);
		 }
                 ;

priority_assigns : '{' priority_assign_set '}'
                 ;

priority_assign_set : priority_assign_set priority_assign
| /*priority_assign*/
                 ;

priority_assign  : '<' node '=' '[' hsf ']' '>'
                 {
		   Enter_PriorityExpr($2,(char*)$5);
		 }
                 ;

transition_rates : '{' transition_rate_set '}'
                 ;

transition_rate_set : transition_rate_set transition_rate
| /*transition_rate*/
                 ;

transition_rate  : '<' node '=' '[' hsf ']' '>'
                 {
		   Enter_TransRate($2,(char*)$5);
		   free($5);
		 }
                 ;

bool_assigns    : '{' bool_assign_set '}'
                ;

bool_assign_set : bool_assign_set bool_assign
| /* bool_assign */
                ;

bool_assign     : '<' node '=' '[' bool_ineqs ']' '>'
                {
		  if (!$2->inequalities) {
		    $2->inequalities = (ineqADT)$5;
		  }
		  else {
		    for (ineqADT step = $2->inequalities; step;
			 step = step->next) {
		      if (!step->next) {
			step->next = (ineqADT)$5;
			break;
		      }
		    }
		  }
                }

bool_ineqs      : bool_ineqs ';' bool_ineq
                { 
		  $$ = $3;
		  ((ineqADT)$3)->next = (ineqADT)$1;
		}
                | bool_ineq
                {
		  $$ = $1;
		}
                ;

bool_ineq       : ID ASSIGN hsf
                {
		  Name_Ptr nptr;
		  nptr = Name_Find_Str((char*)$1);
		  
		  if ( nptr == NULL ) {
		    printf ("Boolean %s doesn't exist in list.\n",
			    (char*)$1);
		    gerror("ERROR");
		  }
		  else {
		    $$ = (Node*)Enter_Inequality(Name_Node(nptr),7,-INFIN,INFIN,(char*)$3);
		  }
                }
                ;


vevents         : vevents ',' vevent
                | vevent
                ;

vevent          : ID 
                {
		  $$ = Enter_Trans ((char*)$1, "", '$');
		  Node_Ptr n = Enter_Place (strcat((char*)$1,"_p"));
		  Enter_Pred (n, $$);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL, "C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
                | NOT ID 
                {
		  $$ = Enter_Trans ((char*)$2, "", '$');
		  Node_Ptr n = Enter_Place (strcat((char*)$2,"_p"));
		  Enter_Pred (n, $$);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL, "C", false, 1.0, 1);
		}
                | signal_trans
                {
		  Node_Ptr n = Enter_Place (strcat((char*)$1,"_p"));
		  Enter_Pred (n, $$);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
                | NOT signal_trans
                {
		  Node_Ptr n = Enter_Place (strcat((char*)$2,"_p"));
		  Enter_Pred (n, $$);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		}
                ;

sevents         : sevents ',' sevent
                | sevent
                ;

sevent          : ID 
                {
		  $$ = Enter_Trans ((char*)$1, "", '$');
		  Node_Ptr n = Enter_Trans ("dummy", "", '$');
		  Enter_Pred ($$, n);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		}
                | NOT ID 
                {
		  $$ = Enter_Trans ((char*)$2, "", '$');
		  Node_Ptr n = Enter_Place (strcat((char*)$2,"_p"));
		  Enter_Pred (n, $$);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
                | signal_trans
                {
		  Node_Ptr n = Enter_Trans ("dummy", "", '$');
		  Enter_Pred ($$, n);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		}
                | NOT signal_trans
                {
		  Node_Ptr n = Enter_Place (strcat((char*)$2,"_p"));
		  Enter_Pred (n, $$);
		  Enter_Succ (n, $$, 0, INFIN, 0, (-1)*INFIN, INFIN, NULL, 
			      NULL,"C", false, 1.0, 1);
		  Enter_Mark (&initial_marking, PLACE_MARK, NULL, n, 1, 1);
		}
                ;
%%
		
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



