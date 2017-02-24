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
/* storeprs.h                                                                */
/*****************************************************************************/

#ifndef _storeprs_h
#define _storeprs_h

#include "struct.h"
#include "bcp.h"

tableSolADT find_minimal_prs(regionADT region,int nsignals,bool exact);

bool check_combo(stateADT *state_table,regionADT *regions,int nsignals,
		bool combo,bool exact,tableSolADT NewSol,int k,int l);

/*****************************************************************************/
/* Remove unnecessary C-elements.                                            */
/*****************************************************************************/

bool *check_combinational(stateADT *state_table,regionADT *regions,
                           int ninputs,int nsignals,bool combo);

/*****************************************************************************/
/* Check if current guard is symmetric to an upcoming guard.                 */
/*****************************************************************************/

bool symmetric_guard(regionADT start,regionADT region,int nsignals);

/*****************************************************************************/
/* Check if guards are disjoint.                                             */
/*****************************************************************************/

bool disjoint_check(char * filename,signalADT *signals,stateADT *state_table,
		    regionADT *regions,bool *comb,int ninpsig,int nsignals);

/*****************************************************************************/
/* Store production rules.                                                   */
/*****************************************************************************/
bool store_production_rules(char command,designADT design);

/*****************************************************************************/
/* Prints covers for a signal transition selected by NewSol.  Prints all    */
/*   NewSol is NULL.     .                                                   */
/*****************************************************************************/

bool print_covers(FILE *fp,char * filename,signalADT *signals,
		  regionADT *regions,bool exact,int *fanout,bool comb1,
		  bool comb2,int i,int nsignals,int maxsize,bool *first,
		  bool *okay,int *area,int *total_lits,FILE **errfp,
		  tableSolADT NewSol);

#endif  /* storeprs.h */

