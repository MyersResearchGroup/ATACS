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
/* findrsg.h                                                                  */
/*****************************************************************************/

#ifndef _findrsg_h
#define _findrsg_h

#include "struct.h"

#define EOS '\0'

#define FIRE_TRANS  'f'
#define FIRE_CLK  'F'
#define INTRO_CLK 'I'
#define DEL_CLK   'D'
#define DEL_VAR   'V'
#define INTRO_PRED 'J'
#define DEL_PRED 'E'

/*****************************************************************************/
/* Check if a signal is enabled given a marking.                             */
/*****************************************************************************/

bool event_enabled(eventADT *events,ruleADT **rules,markkeyADT *markkey, 
		   char * marking,
		   int event,int nrules,bool verbose,markingADT marking_struct,
		   int nsigs, bool currently_enabled,bool chkord,
		   bool chkassump);

/*****************************************************************************/
/* Implementation of Hash Function from Aho, Sethi, and Ullman, p.436        */
/*****************************************************************************/

int hashpjw(char * s);

/*****************************************************************************/
/* Convert R to 0 and F to 1 for all bits in a state.                        */
/*****************************************************************************/
 
char * SVAL(char * state,int nsignals);

/*****************************************************************************/
/* Find state in state table, return where it goes if it's not there.        */
/*****************************************************************************/

stateADT find_state(stateADT *state_table,char * state,char * marking,
		    char * enablings, clocksADT clocks, clockkeyADT clockkey,
		    int nsignals, int num_clocks, bool timed,int *timing_match,
		    int *is_new_state, int nrules, int clock_size,
		    timeoptsADT timeopts,exp_enabledADT exp_enabled,
		    int *removed_regions,ruleADT **rules, bool bap );

/*****************************************************************************/
/* Check if state is in hashtable, if so, return true, else add to table and */
/*   return false.                                                           */
/*****************************************************************************/

int add_state(FILE* fp,stateADT *state_table,char * old_state,
	      char * old_marking,char * old_enablings,
	      clocksADT old_clocks, clockkeyADT old_clockkey,
	      int old_num_clocks, exp_enabledADT old_exp_enabled,
	      char * new_state,char * new_marking,
	      char * new_enablings,int nsignals,
	      clocksADT new_clocks, clockkeyADT new_clockkey, 
	      int new_num_clocks, int sn,bool verbose, bool timed, int nrules,
	      int old_clock_size, int new_clock_size,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
	      int fire_enabling, int fire_enabled, 
	      exp_enabledADT new_exp_enabled,ruleADT **rules, int nevents,
	      bool bap = false, int action_type = FIRE_TRANS);

/*****************************************************************************/
/* Check if two states have same code but different outputs are enabled.     */
/*****************************************************************************/

bool csc_violation(char * state1,char * state2,int ninpsig,int nsignals);

/*****************************************************************************/
/* Check that state graph satisfies CSC.                                     */
/*****************************************************************************/

bool check_csc(char * filename,signalADT *signals,stateADT *state_table,
	       int nsigs,int ninpsig,bool verbose);

/*****************************************************************************/
/* Find reduced state graph.                                                 */
/*****************************************************************************/

bool find_reduced_state_graph(char menu,char command,designADT design);

markingADT find_new_marking(eventADT *events,ruleADT **rules,
			    markkeyADT *markkey,markingADT old_marking,
			    int i,int nevents,int nrules,bool verbose,
			    int nsigs,int ninpsig,bool disabling,bool noparg);

markingADT dummy_find_new_marking(eventADT *events,ruleADT **rules,
			    markkeyADT *markkey,markingADT old_marking,
			    int i,int nevents,int nrules,bool verbose,
			    int nsigs,int ninpsig,bool disabling,bool noparg);

markingADT find_initial_marking(eventADT *events,ruleADT **rules,
				markkeyADT *markkey,int nevents,int nrules,
				int ninpsig,int nsignals,char * startstate,
				bool verbose);

void my_print_graph(stateADT *state_table, eventADT *events,
                    markkeyADT *markkey, int nrules);

int ver_add_state(ver_stateADT *state_table, char * new_state,
		  char * new_marking, int nsignals, clocksADT new_clocks,
		  clockkeyADT new_clockkey, int new_num_clocks, int nrules, 
		  int sn,int new_clock_size, char * imp_new_state,
		  char * imp_new_marking, clocksADT imp_new_clocks,
		  clockkeyADT imp_new_clockkey, int imp_new_num_clocks, 
		  int imp_new_clock_size, int nimp_rules);

ver_stateADT ver_find_state(ver_stateADT *state_table, char * state, 
			    char * marking, int nsignals,
			    clocksADT clocks, clockkeyADT clockkey,
			    int num_clocks, int clock_size, 
			    char * imp_state, char * imp_marking, 
			    int nimp_signals, clocksADT imp_clocks, 
			    clockkeyADT imp_clockkey, 
			    int imp_num_clocks,  int imp_clock_size, 
			    int *timing_match, int *is_new_state);

markingADT update_imp_marking(markingADT old_marking, int fire_enabled, ruleADT **rules,
			    int nimp_events, int ninputs, eventADT *events);

void delete_marking(markingADT marking);

void print_stuck_info(eventADT *events,ruleADT **rules,
		      markkeyADT *markkey,markingADT marking,
		      int nevents,int nrules,bool verbose,
		      int nsigs, bool verification);

/*****************************************************************************/
/* Push marking onto stack of markings.                                      */
/*****************************************************************************/

void push_marking(markingsADT *marking_stack,markingADT marking,
		  char * firelist);

/*****************************************************************************/
/* Pop marking onto stack of markings.                                       */
/*****************************************************************************/

markingADT pop_marking(markingsADT *marking_stack,char * *firelist);

bool check_rule(ruleADT **rules,int i,int j,bool chkord,bool chkassump);

#endif  /* findrsg.h */
