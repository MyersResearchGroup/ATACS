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
/* verify.h                                                                  */
/*****************************************************************************/

#ifndef _verify_h
#define _verify_h

#include "struct.h"

/*****************************************************************************/
/* Check if a literal is duplicated in a guard.                              */
/*****************************************************************************/

bool duplicated(mergeADT *merges,ruleADT **rules,int nevents,int i,int j);

/*****************************************************************************/
/* Print header for verification file.                                       */
/*****************************************************************************/

void print_header(FILE *fp,char * filename);

/*****************************************************************************/
/* Print initial state information.                                          */
/*****************************************************************************/

void print_initial_state(FILE *fp,eventADT *events,ruleADT **rules,
			 cycleADT ****cycles,int nevents,int ndummy,
			 int ncycles,char * startstate,bool internal);

/*****************************************************************************/
/* Print the specification.                                                  */
/*****************************************************************************/

void print_specification(FILE *fp,signalADT *signals,eventADT *events,
			 mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
			 char * filename,int ninpsig,int nsignals,int ninputs,
			 int nevents,int ndummy,int ncycles,
			 char * startstate,bool si,bool instances);

/*****************************************************************************/
/* Parse a dead trace.                                                       */
/*****************************************************************************/

void parse_dead_trace();

/*****************************************************************************/
/* Store a file to be used by Rokicki's Timed Circuit Verifier Orbit.        */
/*****************************************************************************/

void verify(char * filename,signalADT *signals,eventADT *events,
            mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	    regionADT *regions,int ninputs,int nevents,
	    int ncycles,int ninpsig,int nsignals,char * startstate,bool si,
	    bool verbose,bool gatelevel,bool combo,markkeyADT *markkey,
	    int nrules,bool level,int ndummy);


#endif  /* verify.h */

