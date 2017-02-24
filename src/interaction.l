%{
/*  
    This file generates a parser for the reading of
    interaction data.
    
    Author: Nam Nguyen
    School: University of Utah
    Date:   1/12/07
*/
#include "interactParse.h"	/* token codes from the parser */

extern int yylval;

%}
%x COMMENTS
%%
[\t ]+
#		{BEGIN COMMENTS;}
<COMMENTS>[\n\r] {BEGIN 0;}
<COMMENTS>.*	{}	
[0-9]+		{yylval=atoi(yytext); return NUMBER;}
,		{return COMMA;}
\{		{return LEFT_CURLY;}
\}		{return RIGHT_CURLY;}
\[		{return LEFT_BRACE;}
\]		{return RIGHT_BRACE;}
\n		{}
^PROMOTER	{return PROMOTER;}
^GENE		{return GENE;}
^INTERACTION	{return INTERACTION;}
\"[^"\n]*["\n]	{return TEXT;}
.		{ECHO; return UNKNOWN_TOKEN;}
%%

int yywrap () {
    return 1;
}
