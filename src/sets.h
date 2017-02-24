#include "orbitsrsg.h"
#include "common_timing.h"
#ifndef TIME_STRUCT
#include "time_struct.h"
#endif
constraintADT *sets_create_constraints(int nevents);

processkeyADT sets_create_processkey(int nrules);
 
  
 
processkeyADT sets_copy_processkey(int nrules, processkeyADT old_processkey);

processkeyADT sets_resize_processkey(int old_size, int new_size, processkeyADT old_processkey);

 
constraintADT sets_add_constraints(constraintADT new_const, constraintADT add);


/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int sets_mark_initial_region(ruleADT **rules,  
			markkeyADT *markkey, markingADT marking, 
			clocksADT clocks, clockkeyADT clockkey,
			int nevents, int nrules, rule_info_listADT *info);
 
int  sets_mark_process(ruleADT **rules, 
		       processADT process, processkeyADT processkey, 
		       clocksADT clocks,
		       clockkeyADT clockkey, int num_clocks);

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void sets_restrict_region(eventADT *events, ruleADT **rules, 
			  markkeyADT *markkey, markingADT marking,
			  clocksADT clocks, int nevents, int nrules,
			  rule_info_listADT *info, int fire_enabling, 
			  int fire_enabled, int num_clocks);


/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void sets_recanon_restrict(clocksADT clocks, int num_clocks, int restricted);


/* Eliminate clocks that have been used in this firing. */

int sets_project_clocks(eventADT *events, ruleADT **rules, 
			markkeyADT *markkey, markingADT marking,
			clocksADT clocks, clockkeyADT clockkey, 
			processADT process, processkeyADT processkey,
			int nevents, int nrules, rule_info_listADT *info,
			int fire_enabling, int fire_enabled, int num_clocks,
			int *num_events, int event_fired, int *num_sets,
			constraintADT *constraints);

int sets_increment_sets(processkeyADT processkey, int *num_events);
   
/* Make row computes a new row using the sets specified in processkey.  If it's a valid */
/* row (made from a consistant sets) make_row returns 1, otherwise it returns 0.        */  
int  sets_make_row(ruleADT **rules, processkeyADT processkey,
		  processADT process, int new_event, 
		  int row_number, int *num_events, depend_listADT list, int *num_sets,
		  processADT final_process);

void sets_make_column(ruleADT **rules, processkeyADT processkey,
				processADT process, int new_event, 
				int col_number, int *num_events, depend_listADT list, 
				int *num_sets, processADT final_process);




/* Check for inconsistant maximums and split out consistant sets, returns the number */
/* of consistant sets derived from this row.                                         */

int sets_make_consistant(ruleADT **rules, processADT process, processkeyADT processkey,
			 int *num_events, int new_event, int row_number, depend_listADT conflicts,
			 int *num_sets, processADT final_process, constraintADT *constraints,
			 constraintADT con_list, int num_clocks, clockkeyADT clockkey, 
			 int nevents, rule_info_listADT *info);
	
void sets_project_set(processADT process, int remove_set, int sets);


depend_listADT sets_make_set(ruleADT **rules, processkeyADT processkey,
			     processADT process,  constraintADT *constraints,
			     int new_event, int row_number, int *num_events, 
			     int *num_sets, int *valid_set, processADT final_process);



void sets_add_event(eventADT *events, ruleADT **rules, 
		   markkeyADT *markkey, markingADT marking,
		   clocksADT clocks, clockkeyADT clockkey, 
		   processADT *orig_process, processkeyADT *orig_processkey, 
		   constraintADT *constraints,
		   int nevents, int nrules,
		   int num_clocks, int *num_events, 
		   int new_event, int *num_sets, rule_info_listADT *info,
		   int *process_size);
      
  
  

/* Add clocks to the clock matrix that have been added by the new marking. */

int sets_make_new_clocks(eventADT *events, ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT *orig_clocks, clockkeyADT *orig_clockkey, 
			 processADT *orig_process, processkeyADT *orig_processkey,
			 constraintADT *constraints,
			 int nevents, int nrules, rule_info_listADT *info,
			 int fire_enabling, int num_clocks, int *num_events, int *num_sets,
			 int *clock_size, int *process_size);

void sets_recanon_process(processADT final_process, processADT process, int num_clocks, 
			  processkeyADT processkey,  int row_number);
  



int sets_remove_conflicts(eventADT *events, ruleADT **rules, 
		     markkeyADT *markkey, markingADT marking,
		     clocksADT clocks, clockkeyADT clockkey,
		     processADT process, processkeyADT processkey,
		     int nevents, int nrules, rule_info_listADT *info,
		     int fire_enabling, int fire_enabled, int num_clocks,
		     int *num_events, int *num_sets);




int sets_remove_enabling_conflicts(eventADT *events, ruleADT **rules, 
				   markkeyADT *markkey, markingADT marking,
				   clocksADT clocks, clockkeyADT clockkey, 
				   processADT process, processkeyADT processkey,
				   int nevents, int nrules, rule_info_listADT *info,
				   int fire_enabling, int fire_enabled, 
				   int num_clocks, int *num_events, int *num_sets);

  
/* Put a unit of work on the workstack.    */

void sets_push_work(workstackADT *workstack, markkeyADT *markkey, 
		    markingADT marking, firinglistADT firinglist,
		    clocksADT clocks, clockkeyADT clockkey,
		    processADT process, processkeyADT processkey,
		    char * state, int nevents, int nrules, 
		    rule_info_listADT *old_info, int num_clocks, 
		    int num_events, int num_sets, constraintADT *constraints,
		    int clock_size, int process_size);
			 
void sets_free_structures(markkeyADT *markkey, 
			  markingADT marking, firinglistADT firinglist,
			  clocksADT clocks, clockkeyADT clockkey, 
			  processADT process, processkeyADT processkey,
			  rule_info_listADT *info, int nrules, int nevents,
			  int clock_size, int process_size);

constraintADT *sets_copy_constraints(int nevents, constraintADT *old_constraints);

/*****************************************************************************/
/* Use the new, improved C version of orbits to find state graph             */
/*****************************************************************************/

bool sets_rsg(char * filename,signalADT *signals,eventADT *events,
	      mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	      stateADT *state_table, markkeyADT *markkey, int nevents,
	      int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
	      char * startstate,bool si,bool verbose,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,bool subsets, bool genrg,
	      int ndummy);




