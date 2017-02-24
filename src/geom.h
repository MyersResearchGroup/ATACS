#include "common_timing.h"

int geom_conflict_fired_rules(ruleADT **rules, clocksADT clocks,
			      clockkeyADT clockkey, 
			      int nevents,  clocknumADT clocknums_iv, 
			      firedADT fired_bv,
			      int fire_enabling, int fire_enabled, 
			      int num_clocks);

int check_enablings(ruleADT **rules,  markingADT marking,
		    clocksADT clocks, clockkeyADT clockkey, 
		    int nevents, int nrules, 
		    exp_enabledADT exp_enabled_bv,
		    firedADT fired_bv, clocknumADT clocknums_iv,
		    int num_clocks, int nsigs,markkeyADT *markkey);

int geom_project_ordering(ruleADT **rules,  markingADT marking,
			  clocksADT clocks, clockkeyADT clockkey, 
			  int nevents, int nrules,
			  firedADT fired_bv, clocknumADT clocknums_iv,
			  int num_clocks, int nsigs, int fire_enabled);

int geom_clean_stack(geom_workstackADT *workstack,
		     clocksADT clocks, clockkeyADT clockkey,
		     int num_clocks, int pushed, int stack_depth,
		     markingADT marking);

/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */

int geom_mark_initial_region(ruleADT **rules, clocksADT clocks, 
			     markingADT marking, 
			     clockkeyADT clockkey,
			     int nevents, int nrules,
			     clocknumADT clocknums_iv, int nsigs,
			     bool infopt);

  
/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void geom_restrict_region(eventADT *events, ruleADT **rules, 
			  markkeyADT *markkey, markingADT marking,
			  clocksADT clocks, clockkeyADT clockkey, 
			  int nevents, int nrules,   rule_info_listADT *info, 
			  int fire_enabling,  int fire_enabled, int num_clocks, 
			  int spec_verify, int lower_bound);


/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void geom_recanon_restrict(clocksADT clocks, int num_clocks, int restricted);

/* Eliminate clocks that have been used in this firing. */

int geom_project_clocks(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey, 
			int nevents,  clocknumADT clocknums_iv, firedADT fired_bv,
			int fire_enabling, int fire_enabled, int num_clocks,
			int rule_clocknum);

/* Add clocks to the clock matrix that have been added by the new marking. */

int geom_make_new_clocks(ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT *orig_clocks, clockkeyADT *orig_clockkey, 
			 int nevents, int nrules,  int firing_event, 
			 int num_clocks, int *clock_size,
			 int nsigs, exp_enabledADT exp_enabled_bv,
			 confl_removedADT confl_removed_bv, 
			 clocknumADT clocknums_iv, bool infopt);

int geom_remove_conflicts(ruleADT **rules, clocksADT clocks, 
			  clockkeyADT clockkey,	int nevents, 	
			  int fire_enabling, int fire_enabled,   
			  int num_clocks, exp_enabledADT exp_enabled_bv, 
			  confl_removedADT confl_removed_bv, 
			  clocknumADT clocknums_iv, firedADT fired_bv);

int geom_remove_enabling_conflicts(ruleADT **rules, 
				   clocksADT clocks, clockkeyADT clockkey, 
				   int nevents, int fire_enabling, int fire_enabled, 
				   int num_clocks, firedADT fired_bv, 
				   clocknumADT clocknums_iv,
				   exp_enabledADT exp_enabled_bv);

/* Put a unit of work on the workstack.    */

void geom_push_work(geom_workstackADT *workstack, markingADT marking, 
		    firinglistADT firinglist, clocksADT clocks, clockkeyADT clockkey,	
		    int num_clocks, int clock_size, int nrules, 
		    exp_enabledADT old_exp_enabled,
		    confl_removedADT old_confl_removed,
		    firedADT old_fired, clocknumADT old_clocknums);
			 
void geom_free_structures(markingADT marking, clocksADT clocks, clockkeyADT clockkey, 
			  int clock_size,  clocknumADT clocknums_iv, 
			  exp_enabledADT exp_enabled_bv, 
			  confl_removedADT confl_removed_bv, firedADT fired_bv);

bool geom_rsg(char * filename,signalADT *signals,eventADT *events,
	      mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	      stateADT *state_table, markkeyADT *markkey, int nevents,
	      int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
	      char * startstate,bool si,bool verbose,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
	      int ndummy,int *ncausal,int* nord,bool noparg);


bool geom_verify(char * filename,signalADT *signals,eventADT *events,
		 ruleADT **rules, stateADT *state_table, markkeyADT *markkey,
		 regionADT *regions, int ninputs, int nevents, 
		 int ninpsig,int nsignals, int nrules, char * startstate,
		 bool si, bool verbose);

bool geom_verify1(signalADT *imp_signals, eventADT *imp_events, ruleADT **imp_rules,
		 ver_stateADT *ver_state_table, markkeyADT *imp_markkey,
		 int nimp_events, int nimp_rules, int nimp_signals, 
		 signalADT *signals, eventADT *events, ruleADT **rules,
		 markkeyADT *markkey, 
		 int ninputs, int nevents, int ninpsig,
		 int nsignals, int nrules,
		  char * startstate, bool si, bool verbose);

void ver_geom_push_work(verstackADT *workstatck, 
			markingADT marking, markingADT imp_marking,
			firinglistADT firinglist, firinglistADT imp_firinglist,
			clocksADT clocks, clocksADT imp_clocks, 
			clockkeyADT clockkey, clocksADT imp_clockkey,
			int nevents, int nimp_events, int nrules, int nimp_rules, 
			rule_info_listADT *old_info, rule_info_listADT *imp_old_info,
			int num_clocks, int imp_num_clocks, int clock_size,
			int imp_clock_size);

int ver_enabled_event(int imp_eventnum,  firinglistADT firinglist,
		       ruleADT **rules, int nevents, rule_info_listADT *info,
		       signalADT *signals, signalADT *imp_signals, 
		       eventADT *events);

bool on_firinglist(int enabling, int enabled, firinglistADT firinglist);

void add_enablings(markingADT marking, ruleADT **rules, int nevents, int ninputs);

