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
/*****************************************************************************/
/* findcover.h                                                               */
/*****************************************************************************/

#ifndef _findcover_h
#define _findcover_h

#include "struct.h"

/*****************************************************************************/
/* Determine if a problem has only one possible solution, and if so, the     */
/*   signal associated with that solution is added to the guard and all      */
/*   problems solved by this signal are removed from the table.              */
/*****************************************************************************/

int distinguished(char * solutions,int nsignals);

/*****************************************************************************/
/* Determine if a problem has only a single dot and blanks elsewhere, then   */
/*   remove the row and column from the table.                               */
/*****************************************************************************/

int dotted(char * solutions,int nsignals);

/*****************************************************************************/
/* Remove a problem from a context rule table.                               */
/*****************************************************************************/

void rm_problem(contextADT *context_table,contextADT new_entry);

/*****************************************************************************/
/* Remove solved problems from a context rule table.                         */
/*****************************************************************************/

void rm_solved_problems(contextADT *context_table,int signal,
			char phase);

/*****************************************************************************/
/* Remove dotted problems from a context rule table.                         */
/*****************************************************************************/

void rm_dotted_problems(contextADT *context_table,int signal,
			char phase);

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   context rule and remove solved problems from the context rule table.    */
/*****************************************************************************/

void choose_essential_rows(FILE *fp,signalADT *signals,
                           contextADT *context_table,int i,int l,char * cover,
                           int nsignals,bool verbose);

/*****************************************************************************/
/* Determine if a row is dotted, and if so, remove dotted problems from the  */
/*   context rule table.                                                     */
/*****************************************************************************/

void eliminate_dotted_rows(contextADT *context_table,int nsignals);

/*****************************************************************************/
/* Determine if one column dominates another.                                */
/*****************************************************************************/

int dom_col(char * sol1,char * sol2,int nsignals);

/*****************************************************************************/
/* Remove dominating columns from a context rule table.                      */
/*****************************************************************************/

void rm_dominating_columns(contextADT *context_table,int nsignals);

/*****************************************************************************/
/* Determine if a context rule has a symmetric rule on the opposite          */
/*   transition                                                              */
/*****************************************************************************/

bool symmetric(regionADT *regions,int signal,int event);

int check_restrict_dom(char * cps,char * cqs,int ri,int rj,int nsignals);

bool find_dotted_col(contextADT *context_table,int ri,int rj,
                     contextADT cp,int nsignals);

/*****************************************************************************/
/* Determine number of sharing possibilities that are removed for a given    */
/*   context signal.                                                         */
/*****************************************************************************/

int share_cost(regionADT *regions,int signal,int event,char value,char * cover,
	       int nsignals);

/*****************************************************************************/
/* Remove a possible solution from a context rule table.                     */
/*****************************************************************************/

bool rm_solution(contextADT *context_table,int signal,bool *reduced);

/*****************************************************************************/
/* Remove dominated rows.                                                    */
/*****************************************************************************/
   
bool rm_dominated_rows(regionADT *regions,contextADT *context_table,
		       int i,int nsignals,char * enstate,
		       char * cover,bool burst,signalADT *signals,int nsignal);

/*****************************************************************************/
/* Remove dotted problems from a context rule table.                         */
/*****************************************************************************/

void rm_dotted_rows(contextADT *context_table,int nsignals);

/*****************************************************************************/
/* Determine the number of remaining problems to solve in a context rule     */
/*   table.                                                                  */
/*****************************************************************************/

int numprob(contextADT context_table,int nsignals);

void print_unresolved_conflicts(FILE *fp,signalADT *signals,
				contextADT context_table,int i,int l,
				int nsignals);

/*****************************************************************************/
/* Find cover.                                                               */
/*****************************************************************************/

bool find_cover(char command,designADT design);

#endif  /* findcover.h */

