#ifndef __common_timing_h__
#define __common_timing_h__

#include "orbitsrsg.h"
#ifndef TIME_STRUCT
#include "time_struct.h"
#endif

#include <iostream>
#include <iomanip>
#include "struct.h"
#include "limits.h"

//////////////////////////////////////////////////////////////////////////
// Prints the rules in the firinglist Rf
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, firinglistADT Rf, eventADT *events );

//////////////////////////////////////////////////////////////////////////
// Prints the POSET in matrix form with human readable labels
//////////////////////////////////////////////////////////////////////////
ostream& dump(ostream& s, int size, 
	      processkeyADT processkey, processADT dbm, 
 	      eventADT *events, bool zero_based = false);

//////////////////////////////////////////////////////////////////////////
// prints the clocks key in a more human readable format that follows
// the clocks matrix better.
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, int num_clocks, 
	       clockkeyADT clockkey, eventADT *events );

//////////////////////////////////////////////////////////////////////////
// prints the clocks in a more human readable format.
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, clocksADT clocks, int num_clocks);

//////////////////////////////////////////////////////////////////////////
// prints marking by printing rules
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, 
	       int marking_size,
	       const char* Rf, 
	       markkeyADT *markkey,
	       eventADT *events  );

//////////////////////////////////////////////////////////////////////////
// Generate a report to stdout and lg that looks similar to that published
// by the bag routine.
//////////////////////////////////////////////////////////////////////////
void report( FILE* lg, 
	     char* fname,
	     int zone_count, 
	     int state_count, 
	     unsigned int max_stack_depth,
	     int max_memory, 
	     double user_time );

//////////////////////////////////////////////////////////////////////////
// This function removes rules from the Rf lists exp_enabled_bv and
// fired_bv iff:
//
//     1) The rules uses DISABLING semantics;
//     2) The rules if marked as fired in exp_enabled_bv
//     3) The rules level expression in no longer satisfied in 
//        the current state.
//
// If these 3 conditions hold, then the rule is simply marked unfired.
//////////////////////////////////////////////////////////////////////////
void update_disabled_rules_in_Rf( ruleADT **rules,  
				  markingADT marking,
				  int nevents, 
				  exp_enabledADT exp_enabled_bv,
				  firedADT fired_bv, 
				  int nsigs );

void calculate_mins(clocksADT clocks, int num_clocks);

int check_maximums(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
		       int num_clocks);
int check_minimums(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
		       int num_clocks, int event);


int get_partial_order_minimum(clocksADT clocks, int num_clocks);

rule_info_listADT *make_rule_info(ruleADT **old_rules, int nevents);

rule_info_listADT *copy_rule_info(rule_info_listADT *old_info, int nevents);

path_listADT *copy_path_list(path_listADT *old_path);

void add_event_to_path(int from, int to, rule_info_list *info);

int is_path(int from, int to, rule_info_list *info);
 
rule_infoADT *get_rule_info(rule_info_listADT *info, int enabling, int enabled);
 

/* Let time advance to the maxixmum allowed by the delay ranges. */
	
void advance_time(eventADT *events, ruleADT **rules, 
		  markkeyADT *markkey, markingADT marking,
		  clocksADT clocks, clockkeyADT clockkey, 
		  int nevents, int nrules, rule_info_listADT *info,
		  int firing_event, int num_clocks);

void bv_advance_time(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
		     int num_clocks);  
/* Recanonicalization for advancing time.  */
     
void recanon_advance (clocksADT clocks, int num_clocks);

/* Normalize the clock matrix with the algorithm on pg 153. */

void normalize (clocksADT clocks, int num_clocks, clockkeyADT clockkey,
		ruleADT **rules, bool spec_verify, ruleADT **imp_rules,
		int ninputs);


void print_sigs (int nsignals, signalADT *signals);

void print_events (int nevents, eventADT *events);

void print_rules (int nevents, ruleADT **rules);


void print_marking(markingADT marking);

void print_clocks(clocksADT clocks, int num_clocks);

void print_firinglist(firinglistADT firinglist, eventADT *events);

void print_clockkey(int num_clocks, clockkeyADT clockkey, eventADT *events);
  

void my_print_process(processADT process, int num_clocks);


void print_processkey(int num_clocks, processkeyADT processkey, eventADT *events);


/* Set up the marking field in the rule matrix so it contains    */
/* the index of the position in the marking string that contains */
/* this rule's marking.                                          */

int mark_rules(ruleADT **rules, markkeyADT *markkey, markingADT marking, int nrules);

/* Create and initialize an nrule x nrules clock matrix */

clocksADT create_clocks(int nrules);


processADT create_process(int nrules);


clocksADT copy_clocks(int nrules, clocksADT old_clocks);


clocksADT resize_clocks(int old_size, int new_size, clocksADT old_clocks);


clockkeyADT resize_clockkey(int old_size, int new_size, clockkeyADT old_clockkey); 

  
processADT copy_process(int nrules, processADT old_process);


clocksADT resize_process(int old_size, int new_size, processADT old_process);


processkeyADT resize_processkey(int old_size, int new_size, processkeyADT old_processkey);


clockkeyADT create_clockkey(int nrules);


processkeyADT create_processkey(int nrules);

clockkeyADT copy_clockkey(int nrules, clockkeyADT old_clockkey);
 
processkeyADT copy_processkey(int nrules, processkeyADT old_processkey);
 
/* Return a list of timed enabled events.    */
firinglistADT get_t_enabled_list(ruleADT **rules, clocksADT clocks, 
				 clockkeyADT clockkey, markingADT marking,
				 markkeyADT *markkey, exp_enabledADT exp_enabled_bv, 
				 clocknumADT clocknums_iv, confl_removedADT confl_removed_bv,
				 int *num_clocks, int nevents, int nrules, bool
				 interleave);

firinglistADT old_get_t_enabled_list(ruleADT **rules, clocksADT clocks, 
				     clockkeyADT clockkey, 
				     int num_clocks, int nevents, int nrules);

/* Determine if the given event is enabled. */

bool enabled_event(int eventnum, int enabling, ruleADT **rules, int nevents,
		   rule_info_listADT *info);

bool bv_enabled_event(int eventnum, int enabling, ruleADT **rules, int nevents,
		      exp_enabledADT exp_enabled_bv);
/* Clear all the exp_enabled bits for an event, since it has fired. */

void clear_event(int eventnum, ruleADT **rules, rule_info_listADT *info, int nevents);
void bv_clear_event(int eventnum, ruleADT **rules, exp_enabledADT exp_enabled_bv,
		    confl_removedADT confl_removed_bv, int nevents,
		    clocknumADT clocknums_iv, int nrules);

workADT pop_work(workstackADT *workstack);
approx_workADT approx_pop_work(approx_workstackADT *workstack);
geom_workADT geom_pop_work(geom_workstackADT *workstack);

int mark_process(ruleADT **rules, 
		 processADT process, processkeyADT processkey, 
		 clocksADT clocks,
		 clockkeyADT clockkey, int num_clocks,
		 markingADT marking, markkeyADT *markkey, 
		 int nrules);


void mark_firing(int fired, ruleADT **rules, int nevents,
		 rule_info_listADT *info);
void bv_mark_firing(int fired, ruleADT **rules, int nevents, firedADT fired_bv);

int event_exists(int num_clocks, processkeyADT processkey, 
		 clockkeyADT clockkey,
		 int fire_enabling, int fire_enabled);


int enabling_done(ruleADT **rules, int num_clocks, processkeyADT processkey, 
		  clockkeyADT clockkey,
		  int fire_enabling, int fire_enabled, int nevents,
		  rule_info_listADT *info);

int bv_enabling_done(ruleADT **rules, int fire_enabling, int fire_enabled,
		     int nevents, firedADT fired_bv, int num_clocks, 
		     clockkeyADT clockkey, int max_causal, bool duplicated);

bool actual_fired(int fired, ruleADT **rules, int nevents, 
		  rule_info_listADT *info);

bool bv_actual_fired(int fired, ruleADT **rules, int nevents,
		     firedADT fired_bv);

void mark_no_epsilon(ruleADT **rules, processkeyADT processkey, 
		     int num_events, int nevents, rule_info_listADT *info);

void mark_no_epsilon_bv(ruleADT **rules, processkeyADT processkey, 
			int num_events, int nevents, firedADT fired_bv);
  
void print_covers(regionADT *regions, int nsigs);

firinglistADT ver_get_t_enabled_list(ruleADT **rules, clocksADT clocks, 
				     clockkeyADT clockkey, 
				     int num_clocks, int nevents, int nrules,
				     int ninputs);

verworkADT ver_pop_work(verstackADT *workstack);

bool ver_enabled_event(int eventnum, int enabling, ruleADT **rules, 
		       int nevents, rule_info_listADT *info, int ninputs,
		       markingADT marking);

void free_region(clockkeyADT clockkey, clocksADT clocks, int num_clocks);

void restrict_region(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey, 
		     int fire_enabling,  int fire_enabled, int num_clocks,
		     int spec_verify, int lower_bound, clocknumADT clocknums_iv);

void recanon_restrict(clocksADT clocks, int num_clocks, int restricted);
void recanon_process(processADT process, int num_clocks);
void restrict_to_causal_rule(clocksADT clocks, clockkeyADT clockkey,
			     int num_clocks, int fire_enabling, 
			     int fire_enabled, int causal_clocknum, 
			     int lower, int upper);
void init_rule_info(ruleADT **rules, int nevents, int nrules,
		    exp_enabledADT *exp_enabled_bv,
		    confl_removedADT *conflict_removed_bv, 
		    firedADT *fired_bv, clocknumADT *clocknums_iv,
		    rule_contextADT *context_pv,int *ncausal, int* nord);


void gcd_normalize(ruleADT **rules, int nevents, int nrules);
int find_gcd(int *bounds, int nbounds);
int euclid_gcd(int a, int b);
int count_discrete_states(stateADT *state_table, ruleADT **rules, 
			  int nevents);
int count_states(clocksADT clocks, int vectorsize, int *vector, int index,
		 int max);
bool check_point(int *vector, clocksADT clocks, int vectorsize);
bool level_satisfied(ruleADT **rules,
		    markingADT marking, int nsigs, 
		    int enabling, int enabled);

void limit_region(clocksADT clocks, clockkeyADT clockkey, int num_clocks,
		  processADT process, processkeyADT processkey, 
		  int num_events, ruleADT **rules);

bool not_causal(clockkeyADT clockkey, int num_clocks, int event,
		ruleADT **rules,int nevents);


firinglistADT merge_conflicting_rules(clockkeyADT clockkey, clocksADT clocks,
				      firinglistADT firinglist, int num_clocks,
				      int nevents, 
				      exp_enabledADT exp_enabled_bv,
				      int nrules, ruleADT **rules,
				      int fire_enabling, int fire_enabled,
				      clocknumADT clocknums_iv);

bool constraining(ruleADT **rules, markingADT marking, markkeyADT *markkey,
		  int fire_enabled, int fire_enabling, int nevents);

extern int approx_project_clocks(ruleADT **rules, 
				 clocksADT clocks, clockkeyADT clockkey, 
				 processADT process, processkeyADT processkey,
				 int nevents, clocknumADT clocknums_iv, firedADT fired_bv,
				 int fire_enabling, int fire_enabled, int num_clocks,
				 int *num_events, int event_fired, int rule_clocknum,
				 markingADT marking, eventADT *events);

void init_time_sep(ruleADT **rules, int nrules);
void print_seperations(int nevents, ruleADT **rules, eventADT *events);
void update_time_sep(ruleADT **rules, int num_clocks, clockkeyADT clockkey,
		     clocksADT clocks);

#endif /* common_timing.h */
