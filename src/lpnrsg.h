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
#ifndef _lpnrsg_h
#define _lpnrsg_h

#include "struct.h"
#include "def.h"
#include "memaloc.h"

extern bool *disabled;

typedef struct res_dirty_struct {
  bool *res;
  bool *dirty;
} res_dirtyADT;

typedef struct zone {
  clockkeyADT clockkey;
  clocksADT clocks;
  int pastsize;
  int clocksize;
} zoneADT;

typedef struct lpn_stack_tag {
  markingADT marking;
  char * firelist;
  zoneADT zone;
  struct lpn_stack_tag *link;
} *lpn_stackADT;

/*****************************************************************************/
/* Get necessary set.                                                        */
/*****************************************************************************/

res_dirtyADT lpn_get_necessary_set(signalADT *signals,eventADT *events,
				   ruleADT **rules,int nevents,int nplaces,
				   bool **nec_cache,markingADT s,int t,
				   bool *TD,int depth,int nsignals);

/*****************************************************************************/
/* Find reachable states for a given LPN.                                    */
/*****************************************************************************/

bool lpn_rsg(char * filename,signalADT *signals,eventADT *events,
	     ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
	     int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
	     char * startstate,bool si,bool verbose,bddADT bdd_state,
	     bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
	     int nord,bool noparg,bool search,bool use_dot,bool bap);

/*****************************************************************************/
/* Consider all possible initial input states and count event frequencies.   */
/*****************************************************************************/

bool bio_rsg(char * filename,signalADT *signals,eventADT *events,
	     ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
	     int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
	     char * startstate,bool si,bool verbose,bddADT bdd_state,
	     bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
	     int nord,bool noparg,bool search,bool use_dot,bool bap,
	     double tolerance);

#endif /* lpnrsg.h */
