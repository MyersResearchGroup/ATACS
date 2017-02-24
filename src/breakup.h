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
/* breakup.h                                                                 */
/*****************************************************************************/

#ifndef _breakup_h
#define _breakup_h

#include "struct.h"
#include "def.h"
#include "compile.h"
#include "connect.h"
#include "findtd.h"
#include "findrsg.h"
#include "storeprs.h"

typedef struct cost_tag {
  int maxfanin;
  int gates;
} *costADT;

/*****************************************************************************/
/* Initialize a decomposition structure: allocate memory, determine number   */
/*   of literals in initial guard, and record which are causal signals.      */
/*****************************************************************************/

costADT init_decomp1(int ninpsig,int nsignals,regionADT *regions,int maxstack,
		     int orignsigs,regionADT *origregions);

/*****************************************************************************/
/* Breakup gates which have guards with more literals than "maxstack".       */
/*****************************************************************************/

bool breakup_gates(char * filename,signalADT *signals,eventADT *events,
		   mergeADT *merges,
		   ruleADT **rules,cycleADT ****cycles,int nevents,int ncycles,
		   int ninputs,int maxstack,int maxgatedelay,char * startstate,
		   bool initial,regionADT *lastregions,int status,
		   regionADT *regions,bool manual,int ninpsig,int nsignals,
		   bool *error);

#endif  /* breakup.h */



