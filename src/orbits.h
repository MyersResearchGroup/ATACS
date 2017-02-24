#include "orbitsrsg.h"

/* Moved to struct.h */

/* typedef int **clocksADT; */

/* typedef struct clockkey{ */
/*   int enabled; */
/*   int enabling; */
/* } *clockkeyADT; */


/* A list of timed enabled events that haven't been fired and explored */
/* for a given marking.                                               */

typedef struct firinglist{        /* list of rules timed enabled to fire */
  int enabling;
  int enabled; 
  struct firinglist *next;
} *firinglistADT;

/* The starting point for work, a marking and list of unfired timed   */
/* enabled events.                                                    */  

typedef struct{
  markkeyADT *markkey;
  markingADT marking;
  firinglistADT firinglist;
  clocksADT clocks;
  clockkeyADT clockkey;
  rule_info_listADT *rule_info;
  string state;
  int nrules;
  int nevents;
  int num_clocks;
} *workADT;

/* Stack of all the pending work. */

typedef struct workstack{
  workADT work;
  struct workstack *next;
} *workstackADT;


/* Print functions, self-explanatory */

void print_sigs (int nsignals, signalADT *signals);
void print_events (int nevents, eventADT *events);
void print_rules (int nevents, ruleADT **rules);
void print_marking(markingADT marking);
void print_clocks (clocksADT clocks, int nrules);
void print_firinglist(firinglistADT firinglist);


/* Push and pop work off the stack */

workADT pop_workstack(workstackADT workstack);
void push_workstack(workADT work, workstackADT workstack);

/* Create and copy functions for the clock matrix */

clocksADT create_clocks(int nrules);
clockkeyADT create_clockkey(int nrules);
clockkeyADT copy_clockkey(int nrules, clockkeyADT old_clockkey);
clocksADT copy_clocks(int nrules, clocksADT old_clocks);


/* Set up the clock matrix for a given initial marking */

int mark_initial_region(ruleADT **rules,  
			markkeyADT *markkey, markingADT marking, 
			clocksADT clocks, clockkeyADT clockkey,
			int nevents, int nrules, rule_info_listADT *info);

/* Given a marking and a clock matrix make a list of timed */
/* enabled events.                                         */

firinglistADT get_t_enabled_list(eventADT *events, ruleADT **rules, 
				 markkeyADT *markkey, markingADT marking,
				 clocksADT clocks, int nevents, int nrules,
				 rule_info_listADT *info);

/*Push and pop operations on the workstack */

void push_work(workstackADT *workstack, markkeyADT *markkey, 
	       markingADT marking, firinglistADT firinglist,
	       clocksADT clocks, clockkeyADT clockkey, 
	       string state, int nevents, int nrules, 
	       rule_info_listADT *old_info, int num_clocks);

workADT pop_work(workstackADT *workstack);

/* Set up the marking field in the rule matrix so it contains    */
/* the index of the position in the marking string that contains */
/* this rule's marking.                                          */

void mark_rules(ruleADT **rules, markkeyADT *markkey, int nrules);
ruleADT **make_new_rules(ruleADT **old_rules, int nevents);

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void restrict_region(eventADT *events, ruleADT **rules, 
		     markkeyADT *markkey, markingADT marking,
		     clocksADT clocks, int nevents, int nrules,
		     rule_info_listADT *info,
		     int fire_enabling, int fire_enabled, int num_clocks);

/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void recanon_restrict(clocksADT clocks, int num_clocks, int restricted);

/* Add clocks to the clock matrix that have been added by the new marking. */

int make_new_clocks(eventADT *events, ruleADT **rules, 
		     markkeyADT *markkey, markingADT marking,
		     clocksADT clocks, clockkeyADT clockkey, 
		     int nevents, int nrules, rule_info_listADT *info,
		     int fir, int num_clocks);

/* Let time advance to the maxixmum allowed by the delay ranges. */

	
void advance_time(eventADT *events, ruleADT **rules, 
		  markkeyADT *markkey, markingADT marking,
		  clocksADT clocks, clockkeyADT clockkey, 
		  int nevents, int nrules, rule_info_listADT *info,
		  int firing_event, int num_clocks);

/* Recanonicalization for advancing time.  */
     
void recanon_advance (clocksADT clocks, int num_clocks);

/* Normalize the clock matrix with the algorithm on pg 153. */

void normalize (clocksADT clocks, int num_clocks, clockkeyADT clockkey,
		ruleADT **rules);

/* Determine if the given event is enabled. */

bool enabled_event(int eventnum, int enabling, ruleADT **rules, int nevents,
		   rule_info_listADT *info);

/* Clear all the exp_enabled bits for an event, since it has fired. */

void clear_event(int eventnum, ruleADT **rules, rule_info_listADT *info, 
		 int nevents);

/* Eliminate clocks that have been used in this firing. */

int project_clocks(eventADT *events, ruleADT **rules, 
		   markkeyADT *markkey, markingADT marking,
		   clocksADT clocks, clockkeyADT clockkey, 
		   int nevents, int nrules, rule_info_listADT *info,
		   int fire_enabling, int fire_enabled, int num_clocks);

void free_structures(markkeyADT *markkey, 
		     markingADT marking, firinglistADT firinglist,
		     clocksADT clocks, clockkeyADT clockkey, rule_info_listADT *info,
		     int nrules, int nevents);

/* Check for clocks that conflict with the closen firing and project */
/* them out.                                                         */

int remove_conflicts(eventADT *events, ruleADT **rules, 
		      markkeyADT *markkey, markingADT marking,
		      clocksADT clocks, clockkeyADT clockkey, 
		      int nevents, int nrules, rule_info_listADT *info,
		      int fire_enabling, int fire_enabled, int num_clocks);

int remove_enabling_conflicts(eventADT *events, ruleADT **rules, 
		     markkeyADT *markkey, markingADT marking,
		     clocksADT clocks, clockkeyADT clockkey, 
		     int nevents, int nrules, rule_info_listADT *info,
		     int fire_enabling, int fire_enabled, int num_clocks);

bool geom_rsg(string filename,signalADT *signals,eventADT *events,
		mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
		stateADT *state_table, markkeyADT *markkey, int nevents,
		int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
		string startstate,bool si,bool verbose);

clocksADT copy_clocks(int nrules, clocksADT old_clocks);

clockkeyADT copy_clockkey(int nrules, clockkeyADT old_clockkey);

void print_clockkey(int num_clocks, clockkeyADT clockkey);
rule_infoADT *get_rule_info(rule_info_listADT *info, int enabling, int enabled);
rule_info_listADT *copy_rule_info(rule_info_listADT *old_info, int nevents);
