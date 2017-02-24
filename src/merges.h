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
/* merges.h                                                                  */
/*****************************************************************************/

#ifndef _merges_h
#define _merges_h

#include "struct.h"

/*****************************************************************************/
/* Print a signal name.                                                      */
/*****************************************************************************/

void print_signal_name(FILE *fp,eventADT *events,int i);

/*****************************************************************************/
/* Check if two signals are merged.                                          */
/*****************************************************************************/

bool are_merged(mergeADT *merges,int i,int j);

/*****************************************************************************/
/* Add a context rule to the timed event-rule structure.                     */
/*****************************************************************************/

bool add_context_rule(FILE *fp, eventADT *events,ruleADT **rules,
		      markkeyADT *markkey,cycleADT ****cycles,
		      char phase,int enabling_signal,int enabled,int *nrules,
		      int nevents,int ncycles,bool verbose);

/*****************************************************************************/
/* Free space used by old set of merges and                                  */
/*   create space for new set of merges.                                     */
/*****************************************************************************/

mergeADT *new_merges(int status,bool error,int oldnevents,int nevents,
		     mergeADT *oldmerges);

/*****************************************************************************/
/* Add merger to a list.                                                     */
/*****************************************************************************/

void add_merger(mergeADT *merges,int e,int f,int mergetype);

/*****************************************************************************/
/* Add mergers.                                                              */
/*****************************************************************************/

void add_mergers(mergeADT *merges,int e,int f,int mergetype);

/*****************************************************************************/
/* Print mergers.  FOR TESTING PURPOSES!!!                                   */
/*****************************************************************************/

void print_mergers(eventADT *events,mergeADT *merges,int nevents);

/*****************************************************************************/
/* Load merger lists.                                                        */
/*****************************************************************************/

bool load_mergers(FILE *fp,int nevents,int nmergers,eventADT *events,
		 mergeADT *merges,int *linenum);

/*****************************************************************************/
/* Find flat event in event list and return its position.                    */
/*****************************************************************************/

int find_flat_event(eventADT *events,mergeADT *merges,int nevents,
		    char * event);

#endif  /* merges.h */
