/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* loader.h                                                                  */
/*****************************************************************************/

#ifndef _loader_h
#define _loader_h

#include "struct.h"

/*****************************************************************************/
/* Defining token types needed for ER system files.                          */
/*****************************************************************************/

#define WORD 1          /* strings, characters */
#define DOT 2           /* the symbol "."char */
#define COMMENT 3       /* the symbol "#"etc. */
#define END_OF_FILE 4   /* eof character */
#define END_OF_LINE 5   /* return character "\n" */

/*****************************************************************************/
/* Get a token from a file.  Used for loading timed event-rule structures.   */
/*****************************************************************************/

int fgettok(FILE *fp,char *tokvalue, int maxtok,int *linenum);

/*****************************************************************************/
/* Get a field of a rule.                                                    */
/*****************************************************************************/

bool get_field(FILE *fp,char * tokvalue,int *token,int *linenum);

/*****************************************************************************/
/* Find event in event list and return its position.                         */
/*****************************************************************************/

int find_event(int nevents,eventADT *events,char * event,int linenum);

/*****************************************************************************/
/* Load header info.                                                         */
/*****************************************************************************/

char * load_header(FILE *fp,int *nevents,int *ninputs,int *nrules,int *ndisj,
		   int *nconf,int *niconf,int *noconf,int *nord,
		   char * *startstate,char * *initval,char * *initrate,
		   int *linenum);

/* Return if string1 absorbs string2 */
bool absorbed(char *string1,char *string2);

level_exp invert_expr(char * product,level_exp oldlist,level_exp newlist,
		      int nsignals);

level_exp and_expr(level_exp expr1,level_exp expr2,int nsignals,int *linenum);

level_exp or_expr(level_exp expr1,level_exp expr2,int nsignals,int *linenum);

void absorb_expr(ruleADT **rules,int e,int f,bool POS);

void SOP_2_POS(ruleADT **rules,int e,int f,level_exp product,char *sum,
	       int nsignals);

/*****************************************************************************/
/* Load event rule system and setup appropriate data structures.             */
/*****************************************************************************/

bool load_event_rule_system(char menu,char command,designADT design,
			    bool sifile,bool newfile);

#endif /* loader.h */





