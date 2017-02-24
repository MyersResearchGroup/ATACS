#include "orbitsrsg.h"
#include "common_timing.h"
#ifndef TIME_STRUCT
#include "time_struct.h"
#endif

/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int pot_mark_initial_region(ruleADT **rules,  
			markkeyADT *markkey, markingADT marking, 
			clocksADT clocks, clockkeyADT clockkey,
			int nevents, int nrules,
			rule_info_listADT *info);

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void pot_restrict_region(eventADT *events, ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT clocks, int nevents, int nrules, 
			 rule_info_listADT *info, int fire_enabling, int fire_enabled, 
			 int num_clocks);

/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void pot_recanon_restrict(clocksADT clocks, int num_clocks, int restricted);
 
 
/* Eliminate clocks that have been used in this firing. */

int pot_project_clocks(eventADT *events, ruleADT **rules, 
		       markkeyADT *markkey, markingADT marking,
		       clocksADT clocks, clockkeyADT clockkey, 
		       processADT process, processkeyADT processkey,
		       int nevents, int nrules, rule_info_listADT *info,
		       int fire_enabling, int fire_enabled, int num_clocks,
		       int *num_events, int projecting_event);

/* Returns the max(t(b) - t(a))*/

int MaxDiff(ruleADT **rules, processADT process, processkeyADT processkey, 
	    int *num_events, int nevents, int a, int instance_a, int b,
	    int instance_b, int progress, rule_info_listADT *info);
  

/* Add clocks to the clock matrix that have been added by the new marking. */

int pot_make_new_clocks(eventADT *events, ruleADT **rules, 
			markkeyADT *markkey, markingADT marking,
			clocksADT *orig_clocks, clockkeyADT *orig_clockkey, 
			processADT *orig_process, processkeyADT *orig_processkey,
			int nevents, int nrules, rule_info_listADT *info,
			int fire_enabling, int num_clocks, int *num_events,
			int *max_unwind, int *clock_size, int *process_size);

void pot_recanon_process(processADT process, int num_clocks);

int pot_remove_conflicts(eventADT *events, ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT clocks, clockkeyADT clockkey,
			 processADT process, processkeyADT processkey,
			 int nevents, int nrules,  rule_info_listADT *info,
			 int fire_enabling, int fire_enabled, int num_clocks,
			 int *num_events);




int pot_remove_enabling_conflicts(eventADT *events, ruleADT **rules, 
				  markkeyADT *markkey, markingADT marking,
				  clocksADT clocks, clockkeyADT clockkey, 
				  processADT process, processkeyADT processkey,
				  int nevents, int nrules,  rule_info_listADT *info,
				  int fire_enabling, int fire_enabled, 
				  int num_clocks, int *num_events);

  
/* Put a unit of work on the workstack.    */

void pot_push_work(workstackADT *workstack, markkeyADT *markkey, 
	       markingADT marking, firinglistADT firinglist,
	       clocksADT clocks, clockkeyADT clockkey,
	       processADT process, processkeyADT processkey,
	       char * state, int nevents, int nrules, rule_info_listADT *old_info,
	       int num_clocks, int num_events, int clock_size, int process_size);

		 
void pot_free_structures(markkeyADT *markkey, 
			 markingADT marking, firinglistADT firinglist,
			 clocksADT clocks, clockkeyADT clockkey, 
			 processADT process, processkeyADT processkey,
			 rule_info_listADT *info, int nrules, int nevents,
			 int clock_size, int process_size);


bool pot_rsg(char * filename,signalADT *signals,eventADT *events,
	     mergeADT *merges,ruleADT **old_rules,cycleADT ****cycles,
	     stateADT *state_table, markkeyADT *markkey, int nevents,
	     int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
	     char * startstate,bool si,bool verbose,bddADT bdd_state,
	     bool use_bdd,markkeyADT *marker,bool subsets, bool genrg,
	     int ndummy);


