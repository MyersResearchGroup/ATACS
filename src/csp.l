%{
/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1993 by, Chris J. Myers                                   */
/*   Center for Integrated Systems,                                          */
/*   Stanford University                                                     */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of Stanford University may not be used in advertising or   */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Stanford.                         */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL STANFORD OR THE AUTHORS OF THIS SOFTWARE BE LIABLE    */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/

#include "ly.h"
#include "actions.h"

/* y.tab.h is created by  "yacc -vd csp.y".
 * It is included so the Lexical Analyzer knows the definitions
 * of the "terminals" (%token) in the grammar and the %union types.
 */

#include "csp.tab.h"

/* commentdepth keeps track of nested comments */
int		commentdepth	= 0;

extern int linenumber;

void count();
%}

%start		norm comment

LETTER		[a-zA-Z]
DIGIT		[0-9]
ID 	        ({LETTER}({LETTER}|{DIGIT})*)
DECINT	        ({DIGIT}+)
MOD             ("module")
ENDMOD          ("endmodule")
PROC            ("process")
ENDPROC         ("endprocess")
PAS             ("passive")
ACT             ("active")
BOOLEAN         ("boolean")
INPUT           ("input")
OUTPUT          ("output")
DEL             ("delay")
T               ("true")
F               ("false")
INF             ("inf"|"infinity")
SKIP            ("skip")
SLASH           ("\\")
%%
<norm>[\n]		{ linenumber++; count(); }	/* ignore whitespace */

<norm>[ \t]		{ count(); }	/* ignore whitespace */

<norm>{DECINT}	        {
		    	    yylval.intval=
				(int) atoi(yytext);
		    	    count(); return(INT);
			}
<norm>{INF}	        {
		    	    yylval.intval=INFIN;
		    	    count(); return(INT);
			}
<norm>{SLASH}           {   count(); return(SLASH); }
<norm>[?!#/&*=<>,;:|([{}\])\~] 	{ count(); return(yytext[0]); }
<norm>"->"		{ count(); return(ARROW); }
<norm>"||"		{ count(); return(PARA); }

"/*"			{/* Note: this is always active, because of nesting. */
			   BEGIN comment;
			   ++commentdepth;
                           count();
			}
<norm>{MOD}		{
		            count(); return(MOD);
			}
<norm>{ENDMOD}		{
		            count(); return(ENDMOD);
			}
<norm>{PROC}		{
		            count(); return(PROC);
			}
<norm>{ENDPROC}		{
		            count(); return(ENDPROC);
			}
<norm>{DEL}		{
		            count(); return(DEL);
			}
<norm>{PAS}		{
		            yylval.intval = PASSIVE;
		            count(); return(PORT);
			}
<norm>{ACT}		{
		            yylval.intval = ACTIVE;
		            count(); return(PORT);
			}
<norm>{BOOLEAN}		{
		            count(); return(BOOLEAN);
			}
<norm>{INPUT}		{
		            yylval.intval = IN;
		            count(); return(BOOL);
			}
<norm>{OUTPUT}		{
		            yylval.intval = OUT;
		            count(); return(BOOL);
			}
<norm>{SKIP}		{
		            count(); return(SKIP);
			}
<norm>{T}		{
		            yylval.boolval = TRUE;
		            count(); return(INIT);
			}
<norm>{F}		{
		            yylval.boolval = FALSE;
		            count(); return(INIT);
			}
<norm>{ID}		{
		            yylval.actionptr = action(yytext, yyleng);
			    count(); return(ID); 
			}
<norm>{ID}"+"	        {
		            yylval.actionptr = action(yytext, yyleng);
			    count(); return(ID); 
			}
<norm>{ID}"-"	        {
		            yylval.actionptr = action(yytext, yyleng);
			    count(); return(ID); 
			}
<comment>(.|\n)		{  count(); } /* discard everything within a comment */
<comment>"*/"		{
                          count();
			   if (--commentdepth == 0)
			      BEGIN norm;
			}

<norm>.			{ /* No other rule applies, so it's a bad token */
		   	   fprintf(stderr,"Bad input char `%c' on line %d\n",
			      yytext[0],linenumber);
                          count();
			}
%%

/* lexinit - starts the Lexical Analyzer off in the right start condition */
lexinit()
{
   BEGIN norm;
}

/* does this really need to be here? */
yywrap()
{
   return 1;
}

#define	MAX_LINE	1024
int	column	= 0;
int	g_line	= 1;
char	g_lline[MAX_LINE];

void
count()
{
    int	i;
    int	j;

    for (i = 0; yytext[i] != '\0'; i++)
	if (yytext[i] == '\n')
	{
	    column	= 0;
	    g_line++;
	}
	else if (yytext[i] == '\t')
	{
	    for ( j = column; j < 8 - (column % 8); j++ )
		if ( j < MAX_LINE )
		    g_lline[j]	= ' ';
	    column	+= 8 - (column % 8);
	}
	else
	    if ( column < MAX_LINE )
		g_lline[column++]	= yytext[i];

    g_lline[column]	= '\0';
}
