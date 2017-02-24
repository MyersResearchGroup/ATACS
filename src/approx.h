#include "orbitsrsg.h"
#include "common_timing.h"
#ifndef TIME_STRUCT
#include "time_struct.h"
#endif

/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int approx_mark_initial_region(ruleADT **rules, clocksADT clocks, 
			       markingADT marking,
			       clockkeyADT clockkey, int nevents, int nrules,
			       clocknumADT clocknums_iv, int nsigs); 

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void approx_restrict_region(eventADT *events, ruleADT **rules, 
			    markkeyADT *markkey, markingADT marking,
			    clocksADT clocks, int nevents, int nrules,
			    rule_info_listADT *info, int fire_enabling, 
			    int fire_enabled, int num_clocks);


/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void approx_recanon_restrict(clocksADT clocks, int num_clocks, int restricted);

/* Eliminate clocks that have been used in this firing. */

int approx_project_clocks(ruleADT **rules, 
			  clocksADT clocks, clockkeyADT clockkey, 
			  processADT process, processkeyADT processkey,
			  int nevents, clocknumADT clocknums_iv, firedADT fired_bv,
			  int fire_enabling, int fire_enabled, int num_clocks,
			  int *num_events, int event_fired, int rule_clocknum);

void approx_recanon_process(processADT process, int num_clocks);

int approx_remove_conflicts(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
			    processADT process, processkeyADT processkey,
			    int nevents, int fire_enabling, int fire_enabled, int num_clocks,
			    int *num_events, exp_enabledADT exp_enabed_bv, 
			    confl_removedADT confl_removed_bv, clocknumADT clocknums_iv,
			    firedADT fired_bv);


int approx_remove_enabling_conflicts(ruleADT **rules, clocksADT clocks, 
				     clockkeyADT clockkey, processADT process, 
				     processkeyADT processkey,
				     int nevents,  int fire_enabling,
				     int fire_enabled, 
				     int num_clocks, int *num_events,
				     firedADT fired_bv, clocknumADT clocknums_iv,
				     exp_enabledADT exp_enabled_bv,
				     confl_removedADT confl_removed_bv);		 

void approx_free_structures(markingADT marking, clocksADT clocks, clockkeyADT clockkey, 
			    processADT process, processkeyADT processkey,
			    int clock_size, int process_size, clocknumADT clocknums_iv,
			    exp_enabledADT exp_enabled_bv, confl_removedADT confl_removed_bv,
			    firedADT fired_bv, rule_contextADT context, int nrules,
			    int num_events, int num_clocks);

bool approx_rsg(char * filename,signalADT *signals,eventADT *events,
		mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
		stateADT *state_table, markkeyADT *markkey, int nevents,
		int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
		char * startstate,bool si,bool verbose,bddADT bdd_state,
		bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
		int ndummy,int *ncausal, int* nord, bool confl_approx,
		bool noparg);

