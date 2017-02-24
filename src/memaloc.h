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
/* memaloc.h                                                                 */
/*****************************************************************************/

#ifndef _memaloc_h
#define _memaloc_h

#include "struct.h"

///////////////////////////////////////////////////////////////////////////////
// This function will intialize all member variables of the state_list_tag
// to be NULL or zero. 
///////////////////////////////////////////////////////////////////////////////
void init( statelistADT s );

/*****************************************************************************/
/* Free space used by old set of signals and                                 */
/*   create space for new set of signals.                                    */
/*****************************************************************************/

signalADT *new_signals(int status,bool error,int oldnsignals,int nsignals,
                       signalADT *oldsignals);

/*****************************************************************************/
/* Free space used by old set of variables and                               */
/*   create space for new set of variables.                                  */
/*****************************************************************************/

variableADT *new_variables(int status,bool error,int oldnvars,int nvars,
                       variableADT *oldvariables);

/*****************************************************************************/
/* Free space used by old set of events and                                  */
/*   create space for new set of events.                                     */
/*****************************************************************************/

eventADT *new_events(int status,bool error,int oldnevents,int nevents,
		     eventADT *oldevents);

/*****************************************************************************/
/* Perform a deep copy of the given eventADT                                 */
/*****************************************************************************/
eventADT copy_event(eventADT event);

/*****************************************************************************/
/* Free space used by old set of rules and                                   */
/*   create space for new set of rules.                                      */
/*****************************************************************************/

ruleADT **new_rules(int status,bool error,int oldnevents,int nevents,
                   ruleADT **oldrules);

/*****************************************************************************/
/* Perform a deep copy of the given ruleADT                                  */
/*****************************************************************************/
ruleADT copy_rule(ruleADT rule);

/*****************************************************************************/
/* Remove added rules.                                                       */
/*****************************************************************************/

void reinit_rules(int nevents,ruleADT **rules,int *nrules,int *ncausal,
		  int *nord);

/*****************************************************************************/
/* Free space used by old marking key and                                    */
/*   create space for new marking key.                                       */
/*****************************************************************************/

markkeyADT *new_markkey(int status,bool error,int oldnmarkings,int nmarkings,
		     markkeyADT *oldmarkkey);

/*****************************************************************************/
/* Initialize cycle graph.                                                   */
/*****************************************************************************/

void rulecpy(int nevents,int ncycles,cycleADT ****cycles,ruleADT **rules,
	     bool initreach);

/*****************************************************************************/
/* Free space used by old cycle graph and                                    */
/*   create space for new cycle graph.                                       */
/*****************************************************************************/

cycleADT ****new_cycles(int status,bool error,int oldncycles,int ncycles,
			int oldnevents,int nevents,ruleADT **rules,
			cycleADT ****oldcycles);

/*****************************************************************************/
/* Free space used by old state graph and initialize state table.            */
/*****************************************************************************/

void initialize_state_table(int status,bool error,stateADT *state_table);

/*****************************************************************************/
/* Mop up loose BDD nodes.                                                   */
/*****************************************************************************/

void bdd_nukit(bddADT bdd_state,bool synth);
/*****************************************************************************/
/* Free space used by old BDD state graph and initialize state table.        */
/*****************************************************************************/
#ifdef DLONG
void initialize_bdd_state_table(int status,bool error,bddADT bdd_state,
				signalADT *signals,int nsigs,int nrules);
#else
//CUDD
bddADT new_bdd_state_table(int status,bool error,bddADT old_bdd_state,
			   signalADT *signals, int nsigs, int ninpsigs, 
			   int nrules, char * filename);
#endif

/*****************************************************************************/
/* Free space used by old set of enabled states and                          */
/*   create space for new set of enabled states.                             */
/*****************************************************************************/

regionADT *new_regions(int status,bool error,int oldntrans,int ntrans,
                       regionADT *oldregions);

/*****************************************************************************/
/* Initialize implementation.                                                */
/*****************************************************************************/

void new_impl(designADT design,int ntrans,bool burst);

/*****************************************************************************/
/* Free space used by old context signal tables and                          */
/*   create space for new context signal tables.                             */
/*****************************************************************************/

contextADT *new_context_tables(int status,bool error,int oldnevents,
			       int nevents,contextADT *oldcontext_tables);

/*****************************************************************************/
/* Reinitialize data structure used for a design.                            */
/*****************************************************************************/

void reinit_design(char command,designADT design,bool initreach);

/*****************************************************************************/
/* Initialize structure used for a design.                                   */
/*****************************************************************************/

designADT initialize_design(void);

/*****************************************************************************/
/* Exchange design context.                                                  */
/*****************************************************************************/

void context_swap(designADT cs_design,designADT design,char *cs_command,
		  char command,int *cs_notreachable,int notreachable,
		  bool newevents);

/*****************************************************************************/
/* Clear out a design.                                                       */
/*****************************************************************************/

void new_design(designADT design,bool freemem);

/*****************************************************************************/
/* Delete a design completely.                                               */
/*****************************************************************************/

void delete_design(designADT design);

/*****************************************************************************/
/* Delete a list of inequalities.                                            */
/*****************************************************************************/

ineqADT new_inequalities(ineqADT inequalities);

void initialize_ver_state_table(ver_stateADT *state_table);

#endif  /* memaloc.h */
