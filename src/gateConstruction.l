%{
/*  
    This file generates a parser for the reading of
    gate building data.
    
    Author: Nam Nguyen
    School: University of Utah
    Date:   1/12/07
*/
#include "gateParse.h"	/* token codes from the parser */

extern int yylval;
int token = -1;

%}
%x COMMENTS
%%
[\t ]+
#		{BEGIN COMMENTS;}
<COMMENTS>.*
<COMMENTS>\n	{BEGIN 0;}	
[0-9]+		{yylval=atoi(yytext); return NUMBER;}
,		{return COMMA;}
\{		{return LEFT_CURLY;}
\}		{return RIGHT_CURLY;}
\[		{return LEFT_BRACE;}
\]		{return RIGHT_BRACE;}
\n		{}
^GATE		{return GATE;}
^LATCH		{return LATCH;}
\"[^"\n]*["\n]	{return TEXT;}
.		{return UNKNOWN_TOKEN;}
%%

int yywrap () {
    return 1;
}
