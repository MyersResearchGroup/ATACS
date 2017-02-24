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
%{

#include "ly.h"
#include "makedecl.h"
#include "tersgen.h"

int yylex ();

int yyerror(string s);

extern FILE *yyin;

string msbinvalid(string S)
{
  return(NULL);
}

%}

%start module

%union		yystacktype
 	  	{
 	  	   int		intval;
 	  	   double	floatval;
 	  	   char		*stringval;
                   actionADT    actionptr;
                   TERstructADT TERSptr;
		   delayADT     delayval;
		   bool         boolval;
 	  	}

%token	ID DELAY INT
%token	INT INIT MOD ENDMOD PROC ENDPROC BOOL PORT
%token	ARROW PARA DEL PAS ACT BOOLEAN SLASH SKIP

%type         <TERSptr> processes process body /* statement */ action skip
%type         <TERSptr> guardstruct guardcmdset guardcmd 
%type         <TERSptr> expr conjunct literal
%type        <delayval> delay
%type	       <intval> INT BOOL PORT
%type         <boolval> INIT 
%nonassoc   <actionptr> ID
%left '|' PARA
%left ';'
%left ARROW

%%

module          :       MOD ID ';' decls processes ENDMOD
                        { checkclosed($5);
                          emitters($2->label,$5); 
                          TERSdelete($5); }
                ;
decls		:	decls decl
		|	decl
		;
decl            :       delaydecl
                |       booldecl
                |       portdecl
		;
delaydecl       :       DEL ID '=' delay ';'
                        { makedelaydecl($2,$4); }
		;
booldecl        :       BOOL ID ';'
                        { makebooldecl($1,$2,FALSE,0,INFIN,0,INFIN); }
                |       BOOL ID '=' '{' INIT '}' ';'
                        { makebooldecl($1,$2,$5,0,INFIN,0,INFIN); }
                |       BOOL ID '=' '{' delay '}' ';'
			{ makebooldecl($1,$2,FALSE,$5.lr,$5.ur,$5.lf,$5.uf);}
                |       BOOL ID '=' '{' INIT ',' delay '}' ';'
			{ makebooldecl($1,$2,$5,$7.lr,$7.ur,$7.lf,$7.uf); }
		;
portdecl        :       PORT ID ';'
                        { makeportdecl($1,$2,FALSE,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,1,1); }
                |       PORT ID '=' '{' INIT '}' ';'
			{ makeportdecl($1,$2,$5,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,1,1); }
                |       PORT ID '=' '{' delay ',' delay '}' ';'
			{ makeportdecl($1,$2,FALSE,$5.lr,$5.ur,$5.lf,$5.uf,
				       $7.lr,$7.ur,$7.lf,$7.uf,1,1); }
                |       PORT ID '=' '{' '(' INT ',' INT ')' '}' ';'
                        { makeportdecl($1,$2,FALSE,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,$6,$8); }
                |       PORT ID '=' '{' INIT ',' delay ',' delay '}' ';'
			{ makeportdecl($1,$2,$5,$7.lr,$7.ur,$7.lf,$7.uf,
				       $9.lr,$9.ur,$9.lf,$9.uf,1,1); }
                |       PORT ID '=' '{' INIT ',' '(' INT ',' INT ')' '}' ';'
			{ makeportdecl($1,$2,$5,0,INFIN,0,INFIN,
				       0,INFIN,0,INFIN,$8,$10); }
                |       PORT ID '=' '{' delay ',' delay ',' 
                                        '(' INT ',' INT ')' '}' ';'
			{ makeportdecl($1,$2,FALSE,$5.lr,$5.ur,$5.lf,$5.uf,
				       $7.lr,$7.ur,$7.lf,$7.uf,$10,$12); }
                |       PORT ID '=' '{' INIT ',' delay ',' delay ',' 
                                        '(' INT ',' INT ')' '}' ';'
			{ makeportdecl($1,$2,$5,$7.lr,$7.ur,$7.lf,$7.uf,
				       $9.lr,$9.ur,$9.lf,$9.uf,$12,$14); }
		;
delay		:	'<' INT ',' INT ';' INT ',' INT '>'
			{ assigndelays(&($$),$2,$4,$6,$8); }
		|	'<' INT ',' INT '>'
			{ assigndelays(&($$),$2,$4,$2,$4); }
                |       ID
                        { checkdelay($1->label,$1->type);
                          assigndelays(&($$),$1->delay.lr,$1->delay.ur,
                                       $1->delay.lf,$1->delay.uf); }
		;
processes	:	processes process
                        { $$ = TERScompose($1,$2,"||"); }
		|	process
		;
process         :       PROC ID ';' body ENDPROC
                        { $$ = TERSrepeat($4);
                          emitters($2->label,$$); }
                ;
/*
body            :       body ';' statement
                        { $$ = TERScompose($1,TERSrename($1,$3),";"); }
                |       statement
                ;
*/
action          :       ID
                        { $$ = TERS($1,FALSE,$1->lower,$1->upper); }
                |       '<' INT ',' INT '>' ID
                        { $$ = TERS($6,FALSE,$2,$4); }
                |       skip
                ;
skip            :       SKIP
                        { $$ = TERSempty(); }
                ;
body            :	body '|' body
                        { $$ = TERScompose($1,TERSrename($1,$3),"|"); }
                |	body ';' body
                        { $$ = TERScompose($1,TERSrename($1,$3),";"); }
                |	body PARA body
                        { $$ = TERScompose($1,$3,"||");}
                |       '(' body ')'
                        { $$ = $2; } 
                |       action
		|	guardstruct
		;
guardstruct	:	'[' guardcmdset ']' 
                        { $$=$2; }
                |       '[' expr ']'
                        { $$ = $2; }
                |       '*' '[' body ']'
                        { $$=TERSmakeloop($3); }
                ; 
guardcmdset	:	guardcmdset '|' guardcmd
                        { $$ = TERScompose($1,TERSrename($1,$3),"|"); }
		|	guardcmd
		;
guardcmd	:	expr ARROW body
                        { $$ = TERScompose($1,TERSrename($1,$3),";"); }
                |	expr ARROW body ';' '*'
                        { $$ = TERScompose($1,TERSrename($1,$3),";");
                          $$ = TERSmakeloop($$); }
		;
expr            :       expr '|' conjunct
                        { $$ = TERScompose($1,TERSrename($1,$3),"|"); }
                |       conjunct
                ;
conjunct        :       conjunct '&' literal
                        { $$ = TERScompose($1,$3,"||"); }
                |       literal
                ;
literal         :       ID 
                        { $$ = TERS($1,TRUE,$1->lower,$1->upper); }
                |       ID '#'
                        { $$ = probe($1,0); }
                |       ID '(' INT ')' '#'
                        { $$ = probe($1,$3); }
                |       '(' expr ')'
                        { $$ = $2; }
                |       skip
                ;

%%

