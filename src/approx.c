#include "approx.h"
#include "findrsg.h"
#include "findreg.h"
#include "def.h"
#include "memaloc.h"
#include "cpu_time_tracker.h"

#define DEBUG
#define MIN_CLOCK_SIZE 5
#define CLOCK_MULT 2
#define PROCESS_MULT 2

#ifndef OSX
#include <malloc.h>
#else
#include <malloc/malloc.h>
#endif

#ifdef DLONG
#include <memuser.h>
#endif

// Returns true IFF event processkey[startnum.event] is found 2 times in 
// the processkey!
bool is_duplicated(int startnum, processkeyADT processkey,
		   int num_events){

  int i = startnum;

  for(i=startnum+1;i<=num_events;i++){
    if((processkey[i].eventnum==processkey[startnum].eventnum) &&
       (startnum!=num_events))
      return true;
  }
  return false;
}


bool match_phase(markingADT marking, int event_num, eventADT *events){

  if(events[event_num]->event[0]=='$'){
    return TRUE;
  }
    
  if(marking->state[events[event_num]->signal]=='0' ||
     marking->state[events[event_num]->signal]=='R'){
    if(event_num%2 == 0){
      return TRUE;
    }
    if(event_num%2 == 1){
      return FALSE;
    }
  }
  
  if(marking->state[events[event_num]->signal]=='1' ||
     marking->state[events[event_num]->signal]=='F'){
    if(event_num%2 == 0){
      return FALSE;
    }
    if(event_num%2 == 1){
      return TRUE;
    }
  }  
  return TRUE;
}
    


/*  Checks if the firing rule conflicts with anything    */
int conflict_restrict(ruleADT **rules, markkeyADT *markkey, 
		      markingADT marking, clocksADT clocks, 
		      int nevents, int nrules, int fire_enabling, int fire_enabled, 
		      int nsigs, eventADT *events, clocknumADT clocknums_iv){

  int i; 
  for(i=1;i<nevents;i++){
    if(rules[i][fire_enabled]->conflict) 
      return 0;
  }
  return -1;
}
	


void free_anti_list(anti_causal_list anti){

  anti_causal_list current;
  anti_causal_list prev;

  if(anti==NULL) return;
  current=anti;
  prev=current;
  while(current!=NULL){
    current=current->next;
    free(prev);
    prev=current;
  }
}

/* void disabling_anti(rule_contextADT context, processkeyADT processkey,
		    processADT process, int num_events, markkeyADT *markkey,
		    int fired_event, ruleADT **rules, int nrules,
		    eventADT *events){

  int i,j;
  context_list current;

  for(i=1;i<=num_events;i++){
    for(j=0;j<nrules;j++)
      if (rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype &
	  DISABLING) {
	if(markkey[j]->enabled==processkey[i].eventnum){
	  current=context[j];
	  while(current!=NULL){
	     If the firing event controls the same signal as a 
	     context signal and they go in opposite directions 
	     The firing event will be disable the event in the 
	     processkey. Right now just for and.  Doesn't consider 
	    that another signal may have kept the rule enabled 
	    if(events[current->event]->signal==events[fired_event]->signal &&
	       ((current->event % 2) != (fired_event %2))){
	      process[i][num_events]=0;
	    }
	    current=current->next;
	  }
	}
      }
  }
} */

// Find any events in the processkey on the fired_event signal and if 
// the events are not in the same direction ( i.e., both rising or 
// falling ), and neither event is a seqencing event, then set 
// the seperation in the process matrix at [i][num_events] to 0 
// if there is a positive seperation already there.
void disabling_anti(rule_contextADT context, processkeyADT processkey,
		    processADT process, int num_events, markkeyADT *markkey,
		    int fired_event, ruleADT **rules, int nrules,
		    eventADT *events){
   int i;
   for(i=1;i<=num_events;i++){
     if(events[processkey[i].eventnum]->signal == events[fired_event]->signal &&
	(( processkey[i].eventnum % 2) != (fired_event %2)) &&
	(events[processkey[i].eventnum]->event[0]!='$') &&
	(events[fired_event]->event[0]!='$')){   
       if(process[i][num_events] > 0){
	 process[i][num_events]=0;
       }
     }
   }
}

anti_causal_list find_anti_causal(ruleADT **rules, markingADT marking, 
				  int nsigs, int rule_enabling,
				  int rule_enabled, int causal,
				  int nevents,  eventADT *events,
				  exp_enabledADT exp_enabled_bv){

  int i;
  anti_causal_list current=NULL;
  anti_causal_list head = NULL;
  char *unfired_state;
  char *prov_state;
  int causal_signal;
  markingADT prov_marking; /* Just a container to pass state to level_sat */

  // This is the simple case where rule_enabling is directly causal.
  if(rule_enabling == causal) return NULL;
  prov_state = (char *) GetBlock((nsigs+1) * (sizeof(char)));
  unfired_state = (char *) GetBlock((nsigs+1) * (sizeof(char)));
  prov_marking = (markingADT)GetBlock(sizeof(*prov_marking));
  strcpy(unfired_state, marking->state);
  causal_signal = events[causal]->signal;  
  if(causal_signal == (-1)){
    /* This is a sequencing event, cannot effect state*/
    return head;
  }
  /* Unfire the causal event */
  if((unfired_state[causal_signal]=='0') || 
     (unfired_state[causal_signal]=='R')){
    unfired_state[causal_signal]='1';
  }
  else{
    unfired_state[causal_signal]='0';
  }
  strcpy(prov_state, unfired_state);
  /* This code assumes that upgoing events are odd and downgoing */
  /* events are even.                                            */
  for(i=1;i<nevents; i++){
    // Is this event not a sequencing event?
    if(events[i]->event[0]!='$'){
      // Is this not the causal event?
      if(i!=causal){
	// Set the value of the signal to be high or low respectively
	if(i%2 == 1){
	  prov_state[events[i]->signal]='1';
	}
	else{
	  prov_state[events[i]->signal]='0';
	}
	prov_marking->state=prov_state;
	prov_marking->marking=NULL;
	// Is the level satisfied? If yes, then add it to the anti-causal list.
	if(level_satisfied(rules, prov_marking, nsigs, 
			   rule_enabling, rule_enabled)){
	  if(head == NULL){
	    head = (anti_causal_list)malloc(sizeof(struct anti));
	    head->event = i;
	    head->next = NULL;
	    current = head;
	  }
	  else{
	    current->next = (anti_causal_list)malloc(sizeof(struct anti));
	    current->next->event=i;
	    current->next->next = NULL;
	    current=current->next;
	  }
	}
	// Restore prov_state to check the next event.
	strcpy(prov_state, unfired_state);
      }
    }
  }
  free(unfired_state);
  free(prov_state);
  free(prov_marking);
  return head;
}

// builds a context list that is a list of events from the
// processkey that if not fired will disable a rule.  
// This is done by going through each event in the 
// processkey and setting the event to its old value in the state.  If 
// the given rule[rule_enabling][rule_enabled] is disabled in the
// new state, then the event is added to the context list.  
//
// NOTE: The state needs to be restored to its original value at the end
// of each iteration since it only wants to look at 1-bit change at a time
// (i.e., the present state with the event under consideration set to
// it previous value).
context_list find_context(ruleADT **rules, markingADT marking, 
			  int nsigs, int rule_enabling,
			  int rule_enabled, 
			  int nevents,  eventADT *events,
			  exp_enabledADT exp_enabled_bv,
			  processkeyADT processkey, 
			  int num_events){

  int i;
  context_list current=NULL;
  context_list head = NULL;
  char *prov_state;
  markingADT prov_marking; /* Just a container to pass state to level_sat */

  prov_state = (char *) malloc((nsigs+1) * (sizeof(char)));
  prov_marking = (markingADT)malloc(sizeof(*prov_marking));
  // Copy the current marking state into the prov_state
  strcpy(prov_state, marking->state);

  for(i=1; i<=num_events; i++){
    // Look at each fired event that is not a dummy event
    if(events[processkey[i].eventnum]->event[0]!='$'){
      // Set the prov_state bit for the given event number to its previous
      // value.  This means that if you had a a+/1 event, then you would set
      // the 'a' bit to 0.
      if((processkey[i].eventnum % 2) == 1){
	prov_state[events[processkey[i].eventnum]->signal]='0';
      }
      else{
	prov_state[events[processkey[i].eventnum]->signal]='1';
      }
      // Check to see if the boolean condition on the rule is enabled by
      // the prov_state vector.  If the rule is not enabled, then add
      // the fired event from the process_key to the next context_list.
      prov_marking->state=prov_state;
      prov_marking->marking=NULL;
      if(!level_satisfied(rules, prov_marking, nsigs, 
			  rule_enabling, rule_enabled)){
	if(head == NULL){
	  head = (context_list)malloc(sizeof(struct context));
	  head->event = processkey[i].eventnum;
	  head->next = NULL;
	  current = head;
	}
	else{
	  current->next = (context_list)malloc(sizeof(struct context));
	  current->next->event=processkey[i].eventnum;
	  current->next->next = NULL;
	  current=current->next;
	}
      }
      // Restore prov_state to the unchanged marking->state
      strcpy(prov_state, marking->state);
    }
  }
  free(prov_state);
  free(prov_marking);
  return head;  
}

// init_context goes through each rule in the clockkey and builds a
// a context list for it.  The context list is a list of events from
// the processkey that must be fired for the rule to be enabled.
void init_context(ruleADT **rules, markingADT marking, 
		  int nsigs, int nevents,  eventADT *events,
		  exp_enabledADT exp_enabled_bv,
		  processkeyADT processkey, 
		  int num_events, clockkeyADT clockkey,
		  int num_clocks, rule_contextADT context){

  int i;
  
  for(i=1;i<=num_clocks;i++){
    context[rules[clockkey[i].enabling][clockkey[i].enabled]->marking]=
      find_context(rules, marking, nsigs, clockkey[i].enabling, clockkey[i].enabled,
		   nevents, events, exp_enabled_bv, processkey, num_events);
  }
}

void restrict_context_causal(int enabling, int enabled, int causal,
			     ruleADT **rules, rule_contextADT context,
			     processkeyADT processkey, processADT process,
			     int num_events){

  int i;
  context_list current;

  current=context[rules[enabling][enabled]->marking];
  if(current==NULL) return;
  while(current!=NULL){
    for(i=1;i<=num_events;i++){
      /* Check if this event is context. The enabling event is context
         too */
      if((processkey[i].eventnum==current->event) || 
	 (processkey[i].eventnum==enabling)){
	if (process[i][num_events]>0){
	  process[i][num_events]=0;
	}
      }
    }
    current=current->next;
  }
}
  
// Removes context lists for events that are nolonger in the processkey.
void clean_context(rule_contextADT context, int nrules,
		   markkeyADT *markkey, processkeyADT processkey,
		   int num_events){

  int i,j, found;
  context_list current;
  context_list prev;

  for(i=0; i<nrules; i++){
    if(context[i]!=NULL){
      found=0;
      for(j=1;j<=num_events;j++){
	if(processkey[j].eventnum==markkey[i]->enabled){
	  found=1;
	  break;
	}
      }
      if(found!=1){
	if(markkey[i]->enabled==j){
	  current=context[i];
	  prev=context[i];
	  while(current!=NULL){
	    current=current->next;
	    free(prev);
	    prev=current;
	  }
	  context[i]=NULL;
	}
      }
    }
  }
}

// This functions looks at the markkey to find any rule that enables the 
// fired event and if that rules enabling event conflicts with the passed
// in enabling_event (fired_enabling from approx_rsg), then delete
// the context list for rule i (markkey maps i to an actual rule).
void clean_context_only_conflicts(rule_contextADT context, int fired_event,
				  int nrules, markkeyADT *markkey,
				  ruleADT **rules, int enabling_event){

  int i;
  context_list current;
  context_list prev; 
  
  for(i=0; i<nrules; i++){
    if((markkey[i]->enabled==fired_event) && 
       (rules[markkey[i]->enabling][enabling_event]->conflict)){
      current=context[i];
      prev=context[i];
      while(current!=NULL){
	current=current->next;
        free(prev);
	prev=current;
      }
      context[i]=NULL;
    }
  }
}
    
/* No longer used */

anti_causal_list retrieve_anti_causal(clockkeyADT clockkey, int fire_enabled,
				      int fire_enabling, int num_clocks){

  int i;
  anti_causal_list old_current;
  anti_causal_list new_current;
  anti_causal_list head = NULL;

  for(i=1;i<=num_clocks; i++){
    if((clockkey[i].enabling==fire_enabling) && (clockkey[i].enabled==fire_enabled)){
      if(clockkey[i].anti==NULL){
	return NULL;
      }
      else{
	head = (anti_causal_list)malloc(sizeof(struct anti));
	new_current=head;
	old_current=clockkey[i].anti;
	new_current->event= old_current->event;
	new_current->next = NULL;
	old_current=old_current->next;
	while(old_current!=NULL){
	  new_current->next = (anti_causal_list)malloc(sizeof(struct anti));
	  new_current = new_current->next;
	  new_current->event= old_current->event;
	  new_current->next = NULL;
	  old_current=old_current->next;
	}
	return head;
      }
    }
  }
  printf("Error getting anti-causal list, rule does not exist\n");
  return NULL;
}

rule_contextADT copy_context(rule_contextADT old_context, int nrules){

  int i;
  context_list new_current;
  context_list old_current;
  context_list head=NULL;
  rule_contextADT new_context;

  new_context=(rule_contextADT)GetBlock((nrules+1)* (sizeof(context_list)));
  for(i=0;i<nrules;i++){
    if(old_context[i]==NULL) {
      new_context[i]=NULL;
    }
    else{
      head=(context_list)GetBlock(sizeof(struct context));
      new_current=head;
      old_current=old_context[i];
      new_current->event=old_current->event;
      new_current->next=NULL;
      old_current=old_current->next;
      while(old_current!=NULL){
	new_current->next = (context_list)GetBlock(sizeof(struct context));
	new_current = new_current->next;
	new_current->event= old_current->event;
	new_current->next = NULL;
	old_current=old_current->next;
      }
      new_context[i]=head;
    }
  }
  return new_context;
}

bool is_anti_causal(anti_causal_list anti, int event){
  
  anti_causal_list current;

  current=anti;
  while(current!=NULL){
    if(current->event == event) return TRUE;
    current=current->next;
  }
  return FALSE;
}

bool is_context_rule(rule_contextADT context, int enabling, int enabled,
		     int test_event,  ruleADT **rules){

  context_list current;
  
  current=context[rules[enabling][enabled]->marking];
  if( !(rules[enabling][enabled]->ruletype & ORDERING) && (test_event==enabling)){
    return TRUE;  // The enabling event is also context
  }
  while(current!=NULL){
    if(current->event == test_event){
      return TRUE;
    }
    else{
      current=current->next;
    }
  }
  return FALSE;
}

/* This returns the delay that should be used to set up the minimum */
/* if the test event is context, and -1 otherwise */
int is_context_event(rule_contextADT context, int enabling,
		      int event_fired, int test_event, ruleADT **rules,
		      int nrules, markkeyADT *markkey){

  int i;
  int max_delay=-1;
  
  for(i=0;i<nrules;i++){
    // Is this a rule that enabled event_fired that is not a constraint rule?
    if((markkey[i]->enabled==event_fired) && 
       (!(rules[markkey[i]->enabling][enabling]->conflict))){
      // Is this a context rule, meaning that its enabling event is the
      // test_event or the test_event is found in its context list. 
      if(is_context_rule(context, markkey[i]->enabling, 
			 markkey[i]->enabled, test_event, rules)){
	// Find the max lower bound on these context rules.
	if(rules[markkey[i]->enabling][markkey[i]->enabled]->lower > max_delay){
	  max_delay=rules[markkey[i]->enabling][markkey[i]->enabled]->lower;
	}
      }
    }
  }
  return max_delay;
}

// add_to_anti_list finds the entry in the processkey for the causal
// event and adds to its anti list the entries in the new_list.
void add_to_anti_list(int causal, anti_causal_list new_list, 
		      processkeyADT processkey, int num_events){
  
  int i;
  anti_causal_list current_new;
  anti_causal_list current_old;
  
  if(new_list==NULL) return;
  current_new=new_list;
  for(i=1;i<=num_events;i++){
    if(processkey[i].eventnum==causal){
      current_old=processkey[i].anti;
      if(current_old!=NULL){
	while(current_old->next!=NULL){
	  current_old=current_old->next;
	}
      }
      if(current_old==NULL){
	current_old=(anti_causal_list)malloc(sizeof(struct anti));
	processkey[i].anti=current_old;
	current_old->event=current_new->event;
	current_old->next=NULL;
	current_new=current_new->next;
      }
      while(current_new!=NULL){
	current_old->next=(anti_causal_list)malloc(sizeof(struct anti));
	current_old=current_old->next;
	current_old->event=current_new->event;
	current_old->next=NULL;
	current_new=current_new->next;
      }
      return;
    }
  }
  printf("error, did not find causal event\n");
}
	
      
  
  
bool check_interleavings(firinglistADT firinglist, ruleADT **rules, 
			 int nevents, exp_enabledADT exp_enabled_bv,
			 int nrules){

  firinglistADT current = NULL;
  exp_enabledADT prov_exp_enabled;
  
  current=firinglist;
  prov_exp_enabled=(exp_enabledADT) GetBlock((nrules+1) * (sizeof(char)));
  strcpy(prov_exp_enabled, exp_enabled_bv);
  while(current!=NULL){
    strcpy(prov_exp_enabled, exp_enabled_bv);
    prov_exp_enabled[rules[current->enabling][current->enabled]->marking]='T';
    if(bv_enabled_event(current->enabled, current->enabling,
			rules, nevents, prov_exp_enabled)){
      free(prov_exp_enabled);
      return 1;
    }
    current=current->next;
  }
  free(prov_exp_enabled);
  return 0;
}

int approx_clean_stack(approx_workstackADT *workstack,
		       clocksADT clocks, clockkeyADT clockkey,
		       int num_clocks, int pushed, int stack_depth,
		       markingADT marking, int nrules){

  approx_workstackADT prev_workstack;
  approx_workstackADT curr_workstack;
  approx_workstackADT old_workstack;
  approx_workADT current_work;
  int x,i,j;
  int *match;
  int matches=0;
  bool superset=0;
  clocksADT stack_clocks;
  clockkeyADT stack_clockkey;
  int stack_num_clocks;  
  int removed_regions=0;
  markingADT stack_marking;
  firinglistADT cur_firing_list,next_firing_list;

  if(*workstack == NULL) return 0;
  prev_workstack=(*workstack);
  curr_workstack=(*workstack);
  if(pushed && (stack_depth > 1)){
    curr_workstack=(*workstack)->next;
  }
  if(pushed && (stack_depth==1)) return 0;
  match=(int *)GetBlock((num_clocks+1)*sizeof(int));
  match[0]=0;
  for(x=0;x<(stack_depth-pushed);x++){
    superset=1;
    current_work=curr_workstack->work;
    stack_clocks=current_work->clocks;
    stack_clockkey=current_work->clockkey;
    stack_num_clocks=current_work->num_clocks;
    stack_marking=current_work->marking;
    if((strcmp(stack_marking->state, marking->state)!=0) ||
       (strcmp(stack_marking->marking, marking->marking)!=0)){
      superset=0;
    }
    if (stack_num_clocks!=num_clocks) superset = 0;
    if(superset){
      matches=0;
      for(i=1;i<=num_clocks;i++){
	for(j=1;j<=num_clocks;j++){
	  if((clockkey[i].enabling == stack_clockkey[j].enabling) &&
	     (clockkey[i].enabled == stack_clockkey[j].enabled)){
	    match[i]=j;
	    matches++;
	    break;
	  }
	}
      }
      if(matches!=num_clocks) superset = 0;
      if(superset){
	for(i=0;i<=num_clocks;i++){
	  for(j=0;j<=num_clocks;j++){
	    if (clocks[i][j] < stack_clocks[match[i]][match[j]]){
	      superset=0;
	      break;
	    }
	  }
	  if(superset==0) break;
	}
      }
      if(superset){
	if(curr_workstack==(*workstack)){
	  old_workstack=(*workstack);
	  (*workstack)=(*workstack)->next;
	  prev_workstack=(*workstack);
	  curr_workstack=(*workstack);
	  free(old_workstack);
	}
	else{
	  prev_workstack->next=curr_workstack->next;
	  old_workstack=curr_workstack;
	  curr_workstack=curr_workstack->next;
	  free(old_workstack);
	}
	cur_firing_list=current_work->firinglist;
	while (cur_firing_list) {
	  next_firing_list=cur_firing_list->next;
	  free(cur_firing_list);
	  cur_firing_list=next_firing_list;
	}
	approx_free_structures(current_work->marking, current_work->clocks,
			       current_work->clockkey, current_work->process,
			       current_work->processkey, 
			       current_work->clock_size, 
			       current_work->process_size,
			       current_work->clocknums,
			       current_work->exp_enabled, 
			       current_work->confl_removed, 
			       current_work->fired,
			       current_work->context, nrules,
			       current_work->num_events,
			       current_work->num_clocks);
	free(current_work);
	removed_regions++;
      }
    }
    if(!superset){
      prev_workstack=curr_workstack;
      curr_workstack=curr_workstack->next;
    }
  }
  free(match);
  return removed_regions;
}
	  
/* Checks to see if all the rules in the clock key have thier */
/* level satisfied and removes those that don't               */

int approx_check_enablings(eventADT *events,ruleADT **rules,markingADT marking,
			   clocksADT clocks, clockkeyADT clockkey, 
			   processADT process, processkeyADT processkey,
			   int nevents, int nrules,
			   exp_enabledADT exp_enabled_bv,
			   firedADT fired_bv, clocknumADT clocknums_iv,
			   int num_clocks, int *num_events, int nsigs,
			   int fire_enabled){

  int i;
  int new_num_clocks=num_clocks;
  
  i=1;
  // This handles rules that have not yet fired, but have been disabled
  // by the change in state.
  while(i<= new_num_clocks){
    // Is there a rule in the clock key with disabling semantics whose 
    // no longer satisfied?
    if ((rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype &
	 DISABLING) &&
	(!level_satisfied(rules, marking, nsigs, clockkey[i].enabling,
			  clockkey[i].enabled) &&
	 (clockkey[i].enabled!=fire_enabled))) {
      /*      printf("WARNING: The rule %s -> %s has been disabled in state %s!\n",
	     events[clockkey[i].enabling]->event,
	     events[clockkey[i].enabled]->event,marking->state);
      fprintf(lg,"WARNING: The rule %s -> %s has been disabled in state %s!\n",
	      events[clockkey[i].enabling]->event,
	      events[clockkey[i].enabled]->event,marking->state);*/
      new_num_clocks=approx_project_clocks(rules, clocks, clockkey,
					   process, processkey, nevents, 
					   clocknums_iv, fired_bv, 
					   clockkey[i].enabling, 
					   clockkey[i].enabled,
					   new_num_clocks, num_events,0,-1, NULL,
					   NULL);
    }
    else{
      i++;
    }
  }

  // If any rules with DISABLING semantics that are fired have level 
  // expressions that are now false due to the change of state, then
  // remove those rules from the fired lists.
  update_disabled_rules_in_Rf( rules, marking, nevents, exp_enabled_bv,
			       fired_bv, nsigs );

  return new_num_clocks;
}

// Removes clocks on satisfied constraint rules whose enabled event has
// fired.
int approx_project_ordering(eventADT *events,ruleADT **rules,markingADT marking,
			    clocksADT clocks, clockkeyADT clockkey, 
			    processADT process, processkeyADT processkey,
			    int nevents, int nrules,
			    exp_enabledADT exp_enabled_bv,
			    firedADT fired_bv, clocknumADT clocknums_iv,
			    int num_clocks, int *num_events, int nsigs,
			    int fire_enabled){

  int i;
  int new_num_clocks=num_clocks;
  
  i=1;
  while(i<= new_num_clocks){
    if ((rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype &
	 ORDERING) && (clockkey[i].enabled == fire_enabled)) {
      new_num_clocks=approx_project_clocks(rules, clocks, clockkey,
					   process, processkey, nevents, 
					   clocknums_iv, fired_bv, 
					   clockkey[i].enabling, 
					   clockkey[i].enabled,
					   new_num_clocks, num_events,1,-1, marking,
					   events);
    }
    else{
      i++;
    }
  }
  return new_num_clocks;
}


/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int approx_mark_initial_region(ruleADT **rules, clocksADT clocks, 
			       markingADT marking,
			       clockkeyADT clockkey, int nevents, int nrules, 
			       clocknumADT clocknums_iv, int nsigs){

   int i=0,j=0; 
   int min_in_set=INFIN;
   int clock_index=1;

   /* Look at everything that could cause each event and see if it's */
   /* marked find the maximum time before an event must happen       */
   /* The event referenced in the first row causes the event in the  */
   /* second row.                                                    */

   for(i=0;i<nevents;i++){
     for(j=0;j<nevents;j++){
       if((rules[j][i]->ruletype) && 
	  (marking->marking[rules[j][i]->marking]!='F') && 
	  level_satisfied(rules,marking, nsigs, j, i) &&
	  !((rules[j][i]->ruletype & ORDERING) &&
	    (rules[j][i]->lower == 0) &&
	    (rules[j][i]->upper == INFIN))){
	 clocknums_iv[rules[j][i]->marking]=clock_index;
	 clockkey[clock_index].enabled=i;
	 clockkey[clock_index].enabling=j;
	 clockkey[clock_index].causal=j;
	 clockkey[clock_index].anti = NULL;
	 clock_index++;
	 if (rules[j][i]->upper < min_in_set){
	   min_in_set=rules[j][i]->upper;
	 }
       }       
     }
   }

   /* Set up the first row of the clock matrix to contain the max.   */
   /* time until something must fire, and set the rest of the clock  */
   /* differences to 0.                                              */

   for(i=0;i<clock_index;i++){
     for(j=0;j<clock_index;j++){
       if(i==0){
	 if(j!=0){
	   clocks[i][j]=min_in_set;
	 }
       }
       else{
	 clocks[i][j]=0;
       }
     }
   }
   return clock_index-1;
 }
 

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


int approx_restrict_region(ruleADT **rules, clocknumADT clocknums_iv,
			    markkeyADT *markkey, markingADT marking,
			    clocksADT clocks, int nevents, int nrules,
			    int fire_enabling, 
			    int fire_enabled, int num_clocks){
  
  int restrict_clocknum;

  restrict_clocknum=clocknums_iv[rules[fire_enabling][fire_enabled]->marking];
  clocks[restrict_clocknum][0]= -(rules[fire_enabling][fire_enabled]->lower);
  approx_recanon_restrict(clocks, num_clocks, restrict_clocknum);
  return (rules[fire_enabling][fire_enabled]->lower);
}  


/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void approx_recanon_restrict(clocksADT clocks, int num_clocks, int restricted){

  int i,j;

  for(j=0;j<=num_clocks;j++){
    if(clocks[restricted][j] > (clocks[restricted][0] + clocks[0][j])){
      if((clocks[restricted][0]!=INFIN) && (clocks[0][j]!=INFIN)){
	clocks[restricted][j] = clocks[restricted][0] + clocks[0][j];
      }
    }
  }
  for(i=0;i<=num_clocks;i++){
    if(clocks[i][0] > (clocks[i][restricted] + clocks[restricted][0])){
      if((clocks[i][restricted]!=INFIN) && 
	 (clocks[restricted][0]!=INFIN)){      
	clocks[i][0] = clocks[i][restricted] + clocks[restricted][0];
	for(j=0;j<=num_clocks;j++){
	  if( clocks[i][j] > (clocks[i][0] + clocks[0][j])){
	    if((clocks[i][0]!=INFIN) && (clocks[0][j]!=INFIN)){
	      clocks[i][j] = clocks[i][0] + clocks[0][j];
	    }
	  }
	}
      }
    }
  }
}

 
int approx_conflict_fired_rules(ruleADT **rules, clocksADT clocks,
			      clockkeyADT clockkey, 
			      int nevents,  clocknumADT clocknums_iv, 
			      firedADT fired_bv,
			      int fire_enabling, int fire_enabled, 
			      int num_clocks){
  int i=1;
  int num_removed=0;
  int current_num_clocks=num_clocks;

  while(i<=current_num_clocks){
    if(clockkey[i].enabling==(-1)){ 
      approx_project_clocks(rules, clocks, clockkey, NULL, NULL, 
			    0, clocknums_iv, 
			    fired_bv, -1, -1, num_clocks-num_removed, NULL,
			    0, i, NULL, NULL);
      num_removed++;
      current_num_clocks=current_num_clocks-1;
    }
    else i++;
  }
  return num_removed;
}


/* Eliminate clocks that have been used in this firing. */
// egm 08/17/00 --
// This function seems to do several things depending on how it is called.
// Here are the modes that I think I understand:
// 
//      1) rule_clock_num == -1 : remove the clock associated with 
//         the rule <fire_enabling,fire_enabled>
//      2) rule_clock_num != -1 : remove the clock given by rule_clock_num
//      3) event_fired == 1 : clean up the processkey
//
int approx_project_clocks(ruleADT **rules, clocksADT clocks, 
			  clockkeyADT clockkey, processADT process, 
			  processkeyADT processkey, int nevents, 
			  clocknumADT clocknums_iv, firedADT fired_bv,
			  int fire_enabling, int fire_enabled, int num_clocks,
			  int *num_events, int event_fired, int rule_clocknum,
			  markingADT marking, eventADT *events){

  int i,j,k,l,p;
  int removed_event_num;
  bool duplicated;
  
  i=j=k=0;
  l=p=1;


  /* Look to see if this is the last rule  with this enabling */

  if(rule_clocknum==(-1)){
    if(fire_enabled!=fire_enabling){
      rule_clocknum=clocknums_iv[rules[fire_enabling][fire_enabled]->marking];
    }
    /* Special case if events enable themselves, want to project the first */
    /* instance. */
    else{
      for(i=1;i<=num_clocks;i++){
	if(clockkey[i].enabling==fire_enabling){
	  rule_clocknum=i;
	  break;
	}
      }
    }
  }
  if(event_fired){
    while(p<=(*num_events)){
      // Returns true IFF there are 2 duplicate events in the processkey
      duplicated = is_duplicated(p, processkey, (*num_events));
      // Is the event at this index not equal to fire_enabled or
      // is fire_enabled equal to fire_enabling, or is this event duplicate
      // in the processkey?
      if ((processkey[p].eventnum!=fire_enabled) ||
	  (fire_enabled == fire_enabling) ||
	  duplicated) { 
	processkey[p].instance=bv_enabling_done(rules, processkey[p].eventnum,
						fire_enabled, nevents,fired_bv,
						num_clocks, clockkey, 
						processkey[p].root, duplicated); 
      }        
      // Remove an event if it is not causal with bv_enabling_done value of 2,
      // or its bv_enabling_done value is 1 and it is not duplicated and 
      // does not match_phase.
      if((not_causal(clockkey, processkey[p].root , 
		     processkey[p].eventnum, rules,nevents) && 
	  processkey[p].instance==2) || (processkey[p].instance==1)
	 && (duplicated || !(match_phase(marking, processkey[p].eventnum, events)))){
#ifdef __DUMP__
	cout << "Removing " << events[processkey[p].eventnum]  
	     << " from the POSET because ";
        if ( not_causal(clockkey, processkey[p].root , 
			processkey[p].eventnum, rules,nevents) && 
	     processkey[p].instance==2 ) {
	  cout << "it is not used in a causal assignment";
	}
	if ( ( processkey[p].instance == 1 ) &&  duplicated ) {
	  cout << "it is noncausal and it is duplicated in the POSET";
	}
        if ( ( processkey[p].instance == 1 ) &&
	     !match_phase(marking, processkey[p].eventnum, events) ) {
	  cout << "it is noncausal and it is a sequencing event or does not match phase in state.";
	}
        cout << endl;
#endif
	/* Make sure that the event we are removing does not match */
	/* the current phase of the signal.   */
	for(j=0;j<nevents;j++){
	  if(rules[processkey[p].eventnum][j]->ruletype){
	    fired_bv[rules[processkey[p].eventnum][j]->marking]='F';
	  }
	}
	for(j=1;j<=(*num_events);j++){
	  while(processkey[l].eventnum!=processkey[p].eventnum){
	    l++;
	  }
	    removed_event_num=l;
	    while(l<=(*num_events)){
	      process[j][l]=process[j][l+1];
	      l++;
	    }
	    l=1;
	}
	l=1;
	free_anti_list(processkey[removed_event_num].anti);	
	for(j=1;j<=(*num_events);j++){
	  l=removed_event_num;
	  while(l<=(*num_events)){
	    process[l][j]=process[l+1][j];
	    /* Only compress the processkey the first time through */
	    if(j==1){
	      processkey[l].eventnum=processkey[l+1].eventnum;
	      processkey[l].instance=processkey[l+1].instance;
	      processkey[l].root=processkey[l+1].root;	
	      processkey[l].anti=processkey[l+1].anti;
	    }
	    l++;
	  }
	  l=1;
	}
	for(j=0;j<=num_clocks;j++){
	  if(clockkey[j].eventk_num>=removed_event_num){
	    clockkey[j].eventk_num--;
	  }
	}
	(*num_events)--;
      }
      else p++;
    }
  }    
  /* Compress the matrix horizontally */
  for(j=0;j<=num_clocks;j++){
    k=rule_clocknum;
    while(k<=num_clocks){
      clocks[j][k]=clocks[j][k+1];
      /* Only decrement the clock index the first time through. */
      
      if((k<num_clocks) && (j==0)){
	if((clockkey[k+1].enabling!=(-1)) && (clockkey[k+1].enabled!=(-1))){	
	  clocknums_iv[rules[clockkey[k+1].enabling][clockkey[k+1].enabled]->marking]--;
	}
      }
      k++;
    }
    k=0;
  }
  /* Compress the matrix vertically */
  k=0;
  free_anti_list(clockkey[rule_clocknum].anti);  
  for(j=0;j<num_clocks;j++){
    k=rule_clocknum;
    while(k<=num_clocks){
      clocks[k][j]=clocks[k+1][j];
      /* Only compress the clockkey the first time through */
      if(j==0){
	clockkey[k].enabling=clockkey[k+1].enabling;
	clockkey[k].enabled=clockkey[k+1].enabled;
	clockkey[k].eventk_num=clockkey[k+1].eventk_num;
	clockkey[k].causal=clockkey[k+1].causal;
	clockkey[k].anti=clockkey[k+1].anti;
      }
      k++;
    }
    k=0;
  }
  if (((fire_enabling!=fire_enabled) || (!event_fired)) &&
      (fire_enabling >= 0) && (fire_enabled >= 0)) {
    clocknums_iv[rules[fire_enabling][fire_enabled]->marking]=-1;
  }
  num_clocks--;
  if(event_fired){
    for(i=1;i<=(*num_events);i++){
      if((processkey[i].root>=rule_clocknum) || processkey[i].root>num_clocks){
	processkey[i].root--;
      }
    }
  }
  return num_clocks;
}

  

/* Add clocks to the clock matrix that have been added by the new marking. */

int approx_make_new_clocks(ruleADT **rules, markkeyADT *markkey, 
			   markingADT marking, markingADT old_marking, 
			   clocksADT *orig_clocks, 
			   clockkeyADT *orig_clockkey, 
			   processADT *orig_process, 
			   processkeyADT *orig_processkey, int nevents, 
			   int nrules, int fire_enabling, int fire_enabled, 
			   int num_clocks, int *num_events, int *clock_size, 
			   int *process_size, clocknumADT clocknums_iv,
			   exp_enabledADT exp_enabled_bv, 
			   confl_removedADT confl_removed_bv, int causal, 
			   int nsigs, eventADT *events, rule_contextADT context,
			   bool confl_approx){      
  int i,j,f,x,y;
  int cycled;
  clocksADT clocks;
  clockkeyADT clockkey;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;
  processADT process;
  processkeyADT processkey;
  processADT new_process;
  processkeyADT new_processkey;
  int current;
  int orig_num_clocks;
  int context_delay;
  int conflict_restriction;
  anti_causal_list anti=NULL;
  int max_causal_age;  /* The maximum age of the causal rule in this firing sequence */
  context_list curr;
  context_list prev;
  

  orig_num_clocks=num_clocks;
  cycled=0;
  clocks=*orig_clocks;
  clockkey=*orig_clockkey;
  process=*orig_process;
  processkey=*orig_processkey;

  (*num_events)++;
  if((*num_events) > ((*process_size)-2)){
    new_process=resize_process(*process_size, 
			       (*num_events)+2,
			       *orig_process);
    new_processkey=resize_processkey(*process_size, 
				     (*num_events)+2,
				     *orig_processkey);	    
    
    for(f=0;f<(*process_size);f++){
      free(process[f]);
      free_anti_list(processkey[f].anti);
    }
    free(process);
    free(processkey);
    *orig_process=new_process;
    *orig_processkey=new_processkey;
    process=new_process;
    processkey=new_processkey;
    //    *process_size=(*process_size*PROCESS_MULT);
    *process_size=(*num_events)+2;
  }  
  processkey[(*num_events)].eventnum=fire_enabled;
 

  // This code sets the seperations in the row of the process matrix
  // for the new event that has been added.

  /* Instance now keeps track of whether this event is causal to anything */
  /* 0=directly causal 1=not causal 2=causal only through a level         */
  processkey[(*num_events)].instance=0;
  for(j=1;j<=(*num_events);j++){
    // Find the causal event in the process key
    if(processkey[j].eventnum==causal){ 
      // The max seperation between the causal event and fire_enabled is
      // the upper bound on the rule <fire_enabling, fire_enabled>
      // I am assuming this is here for the conf_approx, since it copies
      // from the process to the clocks matrix.  Otherwise it can be
      // removed since it is set below.
      process[*num_events][j] = (rules[fire_enabling][fire_enabled]->upper);
      conflict_restriction=(-1);
      if(!confl_approx){
	conflict_restriction=conflict_restrict(rules, markkey, old_marking, clocks, 
					       nevents, nrules, fire_enabling, 
					       fire_enabled, nsigs, events,
					       clocknums_iv); 
      }
      if(conflict_restriction!= (-1)){
	/* Unnormalize before computing max_causal_age the clocks matrix  */
	/* overconstrainted.                                */
	for(x=1;x<=num_clocks;x++){
	  for(y=1;y<=num_clocks;y++){
	      clocks[x][y]=process[clockkey[x].eventk_num][clockkey[y].eventk_num];
	  }
	}	
	bv_advance_time(rules, clocks, clockkey, num_clocks);
	/* max_causal_age is the maximum age allowed for this rule in the current  */
	/* firing sequence. */
        // Constrain this value to the max time allowed by the firing sequence.
	max_causal_age = 
	  clocks[0][clocknums_iv[rules[fire_enabling][fire_enabled]->marking]];	
	process[*num_events][j]=max_causal_age;
      }
    }
    else{ 
      process[*num_events][j]=INFIN;     
    } 
  }	

  // This code sets the separations in the process matrix for the col
  // of the event that has been added.
  for(j=1;j<=(*num_events);j++){ 
    // Is the eventnum the fire_enabling event or is this the causal
    // event AND is the rule <j,fire_enabled> not a constraint rule.
    if(((processkey[j].eventnum==fire_enabling) || (processkey[j].eventnum==causal))
       && !(rules[processkey[j].eventnum][fire_enabled]->ruletype & ORDERING)){
      // Seperation between j and new event is at least the lower bound
      // on the rule <fire_enabling, fire_enabled>
      process[j][*num_events] = -(rules[fire_enabling][fire_enabled]->lower);
    }
    else{
      if(rules[processkey[j].eventnum][fire_enabled]->ruletype &&
	 (!(rules[processkey[j].eventnum][fire_enabled]->ruletype & ORDERING))){ 
	// if there is a rule <eventnum,fire_enabled> that is not an ordering
        // rule then the seperation on j and fire_enabled is at least the
        // lower bound on the rule <eventnum, fire_enabled>
	process[j][*num_events]=
	  -(rules[processkey[j].eventnum][fire_enabled]->lower);
      }
      else{
	// If a context event exists, find it and set the delay to
        // the max lower bounds of all exisiting context events.
	context_delay=is_context_event(context, fire_enabling, fire_enabled,
				       processkey[j].eventnum, rules, nrules, 
				       markkey); 
	if(context_delay!=(-1)){
	  process[j][*num_events]=-(context_delay);
	}
	else{
	  // If it is anti_causal, then seperation is 0.
	  if(is_anti_causal(processkey[j].anti, fire_enabled)){
	    process[j][*num_events]=0;
	  }
	  else{
	    // Unconstrained seperation.
	    process[j][*num_events]=INFIN;    
	  }
	}
      }
    }
  }  
  process[*num_events][*num_events]=0;   

  disabling_anti(context, processkey, process, *num_events,
		 markkey, fire_enabled, rules, nrules, events);

  // This code updates the clocks matrix with the information from the
  // process matrix, as well as the new clocks added to the clocks matrix
  // newly enabled by the firing of the fire_enabled event.
  for(i=0;i<nrules;i++){
    // Is the rule marked?
    if(marking->marking[i]!='F'){
      // Is there a rule at this location, that has its expression enabled 
      // (i.e., enabled), that is not a constraint rule with [0,inf] bounds?
      if(rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype &&
	 level_satisfied(rules, marking, nsigs, markkey[i]->enabling, 
			 markkey[i]->enabled) &&
	 !((rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype & ORDERING) &&
	   (rules[markkey[i]->enabling][markkey[i]->enabled]->lower == 0) &&
	   (rules[markkey[i]->enabling][markkey[i]->enabled]->upper == INFIN)))
	{
	current=rules[markkey[i]->enabling][markkey[i]->enabled]->marking;  
	// Is the rule not in the clocks matrix, not marked as fire, and
        // not marked as confl_removed_bv; or is the enabling event equal
        // to the enabled event that is equal to the fire_enabled event
        // (i.e., a self loop rule).
	if((clocknums_iv[current] <= -1) && (exp_enabled_bv[current] == 'F') &&
	   (confl_removed_bv[current] == 'F') || 
	   ((markkey[i]->enabling == markkey[i]->enabled) && 
	    markkey[i]->enabled==fire_enabled)){
	  num_clocks++;
	  // If necessary, resize the clock matrix and clockkey, copy the
          // the data, and free the old stuff.
	  if(num_clocks > ((*clock_size)-2)){
	    new_clocks=resize_clocks(*clock_size, 
				     (num_clocks)+3,
				       *orig_clocks);
	    new_clockkey=resize_clockkey(*clock_size, 
					   (num_clocks)+3,
					 *orig_clockkey);
	    free_region(clockkey, clocks, *clock_size);
	    *orig_clocks=new_clocks;
	    *orig_clockkey=new_clockkey;
	    clocks=new_clocks;
	    clockkey=new_clockkey;
	    *clock_size=(num_clocks)+2;
	  }
	  clocknums_iv[current]=num_clocks;
	  if (context[current]) {
	    curr=context[current];
	    prev=context[current];
	    while(curr!=NULL){
	      curr=curr->next;
	      free(prev);
	      prev=curr;
	    }
	  }
	  // Find the context list for the new clock.
	  context[current]=find_context(rules, marking, nsigs,  
					markkey[i]->enabling,
					markkey[i]->enabled,	
					nevents, events,
					exp_enabled_bv,
					processkey, (*num_events));
  /* Restricts the process so the minimum separation between the causal */
	  /* event of the newly enabled rule and any context event is 0.  */
	  /* This prevents inconsistancy.    */
	  restrict_context_causal(markkey[i]->enabling, markkey[i]->enabled,
				  fire_enabled, rules, context, processkey, 
				  process, (*num_events));
	  clockkey[num_clocks].eventk_num=(*num_events);
	  clockkey[num_clocks].enabling=markkey[i]->enabling;
	  clockkey[num_clocks].enabled=markkey[i]->enabled; 
	  clockkey[num_clocks].causal=fire_enabled;
	  if (anti) free_anti_list(anti);
	  // The anti causal list is any event that when fired in the
          // current state satisfies the level expression on the rule 
          // <enabling,enabled> IF the causal rule is unfired
          // in the state.
	  anti = find_anti_causal(rules, marking, nsigs, 
				  markkey[i]->enabling,
				  markkey[i]->enabled,
				  fire_enabled, 
				  nevents, events,
				  exp_enabled_bv);
	  add_to_anti_list(clockkey[num_clocks].causal, 
			   anti, processkey, (*num_events));
	}
      }
    }
  }
  // Clean up the anti list if it was created.
  if (anti) free_anti_list(anti);
  // Recannonicalize the process matrix.
  approx_recanon_process(process, (*num_events));
  // Copy in the separations from the processkey to the clockkey
  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      if(is_context_rule(context, clockkey[j].enabling, clockkey[j].enabled,
		    clockkey[i].causal, rules) && 
	 (process[clockkey[i].eventk_num][clockkey[j].eventk_num] > 0)){
	clocks[i][j]=0;
      }
      else{
	clocks[i][j]=process[clockkey[i].eventk_num][clockkey[j].eventk_num];
      }
    }
  }
  for(i=0; i<=num_clocks;i++){
      clocks[i][0]=0;
  }
  //  limit_region(clocks, clockkey, num_clocks, 
  //       process, processkey, (*num_events), rules);
  /* Root of the maximum rule index that this event can be causal to      */
  processkey[(*num_events)].root=num_clocks;
  return num_clocks;
}    

void approx_recanon_process(processADT process, int num_clocks){

  int i, j, k;

  for(k=1;k<=num_clocks;k++){
    for(i=1;i<=num_clocks;i++){
      if(process[i][num_clocks] > (process[i][k]+process[k][num_clocks])){
	if((process[i][k]!=INFIN) && (process[k][num_clocks]!=INFIN)){
	  process[i][num_clocks]= (process[i][k]+process[k][num_clocks]);
	}
      }
    }
    for(j=1;j<=num_clocks;j++){
      if(process[num_clocks][j] > (process[num_clocks][k] + process[k][j])){
	if((process[num_clocks][k]!=INFIN) && (process[k][j]!=INFIN)){	
	  process[num_clocks][j] = (process[num_clocks][k] + process[k][j]);	
	}
      }
    }
  }
  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      if(process[i][j] > ( process[i][num_clocks] + process[num_clocks][j])){
	if((process[i][num_clocks]!=INFIN) && 
	   (process[num_clocks][j]!=INFIN)){	
	  process[i][j] = ( process[i][num_clocks] + process[num_clocks][j]);	  
	}
      }
    }
  }
}

// NOTE: Draw the diagram of rules to better understand what it is doing.
// A rule <i,j> is removed from the clock matrix if it exists there and
// is marked with 'F' in exp_enabled_bv iff:
//
//  1) its enabling event i does not conflict with fire_enabling and
//     there is a rule <i,fire_enabled>
//  2) The rule <i,fire_enabled> if marked as fired and
//     it marked as F in confl_removed_bv
//  3) j conflicts with fire_enabled
//
int approx_remove_conflicts(ruleADT **rules, clocksADT clocks, 
			    clockkeyADT clockkey, processADT process, 
			    processkeyADT processkey, int nevents, 
			    int fire_enabling, int fire_enabled, 
			    int num_clocks, int *num_events, 
			    exp_enabledADT exp_enabled_bv, 
			    confl_removedADT confl_removed_bv, 
			    clocknumADT clocknums_iv, firedADT fired_bv){
  int i,j;
  int new_num_clocks;

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    // Is there no conflict between i and fire_enabling and is there a rule
    // between i and fire_enabled?
    if((!(rules[i][fire_enabling]->conflict)) && 
       (rules[i][fire_enabled]->ruletype)){
      // Is the rule <i,fire_enabled> marked as fired in exp_enabled_bv and
      // not marked in confl_removed_bv?
      if(((exp_enabled_bv[rules[i][fire_enabled]->marking]=='T') ||
	  (rules[i][fire_enabled]->ruletype & ORDERING)) && 
	 (confl_removed_bv[rules[i][fire_enabled]->marking]=='F')){
	for(j=0;j<nevents;j++){
	  // Is there a rule <i,j>?
	  if(rules[i][j]->ruletype){
	    // Does j conflict with fire_enabled?
	    if((rules[j][fire_enabled]->conflict) && 
	       clocknums_iv[rules[i][j]->marking] > (-1)){
	      // Mark <i,fire_enabled> as removed in conf_removed_bv
              // and wonk its clock.
	      confl_removed_bv[rules[i][fire_enabled]->marking]='T';
	      new_num_clocks=approx_project_clocks(rules, clocks, clockkey, 
						   process, processkey, 
						   nevents, clocknums_iv, 
						   fired_bv, i, j, 
						   new_num_clocks, 
						   num_events,0, -1, NULL, NULL);
	    }
	    // mark rule<i,j> as not fired if j#fire_enabled and
            // <i,j> is a rule.
	    else if((rules[j][fire_enabled]->conflict)){
	      exp_enabled_bv[rules[i][j]->marking]='F';
	    }
	  }
	}
      } 
    }
  }
  return new_num_clocks;
}




int approx_remove_enabling_conflicts(ruleADT **rules, clocksADT clocks, 
				     clockkeyADT clockkey, processADT process, 
				     processkeyADT processkey,
				     int nevents,  int fire_enabling,
				     int fire_enabled, 
				     int num_clocks, int *num_events,
				     firedADT fired_bv, clocknumADT clocknums_iv,
				     exp_enabledADT exp_enabled_bv,
				     confl_removedADT confl_removed_bv){
  int i,k;
  int new_num_clocks;
  int fire_encoding;   /* used to record which event fired that caused */
                       /* a given rule to loose its enabling.  Made    */
  /* large enough so it doesn't overlap with any real event.           */

  /* mark as fired, any rules whose enabling event conflicts with */
  /* the enabling event of the firing rule and which enables the enabled */
  /* event of the firing rule */

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    // Is fire_enabling and i in conflict and a rule enabling i has not fired?
    if(rules[fire_enabling][i]->conflict && 
       !(bv_actual_fired(i, rules, nevents, fired_bv))){
      // Is there a rule between i and fire_enabled?
      if(rules[i][fire_enabled]->ruletype){
	for(k=0;k<nevents;k++){
	  // If there a rule between i and k where i is not equal to k?
	  if(rules[i][k]->ruletype &&
	     (i != k)){	
	    // Is this rule in the clockkey and is it exp_enabled_bv marked?
	    if(clocknums_iv[rules[i][k]->marking] > (-1)){
	      exp_enabled_bv[rules[i][k]->marking]='T';
	      // Delete the clock for the rule <i,k>
	      new_num_clocks=approx_project_clocks(rules, clocks, 
						   clockkey, process, 
						   processkey, nevents, 
						   clocknums_iv, fired_bv,
						   i, k, new_num_clocks,
						   num_events,0, -1, NULL, NULL);
	      if(k!=fire_enabled){
		fire_encoding=fire_enabled*1000;
		if(fire_encoding < nevents){
		  printf("ERROR: remove_enabling conflicts: more than 1000 events");
		}
		clocknums_iv[rules[i][k]->marking]= -(fire_encoding);
	      }
	    }
	  }
	}
      }
    }
  }          
  return new_num_clocks;
}

  
/* Put a unit of work on the workstack.    */

void approx_push_work(approx_workstackADT *workstack, 
		      markingADT marking, firinglistADT firinglist,
		      clocksADT clocks, clockkeyADT clockkey,
		      processADT process, processkeyADT processkey,      
		      int num_clocks, int num_events, int clock_size,
		      int process_size, int nrules, 
		      exp_enabledADT old_exp_enabled,
		      confl_removedADT old_confl_removed,
		      firedADT old_fired, clocknumADT old_clocknums,
		      rule_contextADT context){
  
  approx_workstackADT new_workstack=0;
  approx_workADT new_work=0;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;
  processADT new_process;
  processkeyADT new_processkey;

  //  printf("Num Events = %d; Process Size = %d\n",num_events,process_size);

  new_work=(approx_workADT)GetBlock(sizeof( *new_work));

  new_work->marking=(markingADT)GetBlock(sizeof *marking);
  new_work->marking->marking=CopyString(marking->marking);
  new_work->marking->enablings=CopyString(marking->enablings);
  new_work->marking->state=CopyString(marking->state);
  new_work->marking->up=CopyString(marking->up);
  new_work->marking->down=CopyString(marking->down);

  new_work->firinglist = firinglist;
  new_clocks=(clocksADT) copy_clocks(clock_size, clocks);
  new_work->clocks = new_clocks;
  new_process=(processADT) copy_process(process_size, process);
  new_work->process=new_process;
  new_clockkey=(clockkeyADT) copy_clockkey(clock_size,clockkey);
  new_work->clockkey = new_clockkey;
  new_processkey= (processkeyADT) copy_processkey(process_size, processkey);
  new_work->processkey=new_processkey;
  new_work->exp_enabled=(exp_enabledADT) GetBlock((nrules+1) * (sizeof(char)));
  new_work->confl_removed=(confl_removedADT) GetBlock((nrules+1) * (sizeof(char)));
  new_work->fired=(firedADT) GetBlock((nrules+1) * (sizeof(char)));
  new_work->clocknums=(clocknumADT) GetBlock((nrules+1) * (sizeof(int)));
  new_work->context=copy_context(context, nrules);
  strcpy(new_work->exp_enabled, old_exp_enabled);
  strcpy(new_work->confl_removed, old_confl_removed);
  strcpy(new_work->fired, old_fired);
  memcpy(new_work->clocknums, old_clocknums, (nrules * (sizeof(int))));
  new_work->clock_size = clock_size;
  new_work->process_size=process_size;
  new_work->num_clocks=num_clocks;
  new_work->num_events=num_events;
  new_workstack = (approx_workstackADT)GetBlock(sizeof( *new_workstack));
  new_workstack->work=new_work;
  new_workstack->next=(*workstack);
  (*workstack)=new_workstack;
}
		 
void approx_free_structures(markingADT marking, clocksADT clocks, clockkeyADT clockkey, 
			    processADT process, processkeyADT processkey,
			    int clock_size, int process_size, clocknumADT clocknums_iv,
			    exp_enabledADT exp_enabled_bv, confl_removedADT confl_removed_bv,
			    firedADT fired_bv, rule_contextADT context, int nrules,
			    int num_events, int num_clocks){

  int i;
  context_list current;
  context_list prev;

  for(i=0;i<(clock_size);i++){
    free(clocks[i]);
  }
  for(i=0;i<(process_size);i++){
    free(process[i]);
  }
  for(i=0;i<nrules;i++){
    current=context[i];
    prev=context[i];
    while(current!=NULL){
      current=current->next;
      free(prev);
      prev=current;
    }
  }
  free(context);
  free(clocks);
  free(process);
  for(i=1;i<=num_events;i++){
    free_anti_list(processkey[i].anti);
  }
  free(processkey);
  free(clockkey);
  free(clocknums_iv);
  free(exp_enabled_bv);
  free(confl_removed_bv);
  free(fired_bv);
  delete_marking(marking);
  free(marking);

}


// egm 08/16/00 -- I have replaced the call to this function with a direct 
// reference to the clocknums_iv structure that maps a rules marking to its 
// index in the clockkey.  If the optimization is safe, remove this function!
// NOTE: The optimization does not check that clocknums_iv returns a sensible
// clock index, but it does pass the regression suite!

// Returns causal event for the rule between fire_enabling and fire_enabled
int approx_get_causal_event(clockkeyADT clockkey, int fire_enabling, int fire_enabled,
			    int num_clocks){

  int i;
  for(i=1;i<=num_clocks;i++){
    if((clockkey[i].enabling==fire_enabling) && (clockkey[i].enabled==fire_enabled)){
      return clockkey[i].causal;
    }
  }
  printf("Error, rule not in clockkey\n");
  return 0;
}


bool approx_rsg(char * filename,signalADT *signals,eventADT *events,
		mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
		stateADT *state_table, markkeyADT *markkey, int nevents,
		int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
		char * startstate,bool si,bool verbose,bddADT bdd_state,
		bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
		int ndummy,int *ncausal, int* nord, bool confl_approx,
		bool noparg)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  markingADT marking = NULL;
  markingADT old_marking = NULL;
  clocksADT clocks = NULL;
  clocksADT old_clocks=NULL;
  processADT process = NULL;
  firinglistADT firinglist = NULL;
  firinglistADT temp = NULL;
  exp_enabledADT exp_enabled_bv = NULL;
  exp_enabledADT old_exp_enabled_bv=NULL;
  confl_removedADT confl_removed_bv = NULL;
  clocknumADT clocknums_iv = NULL;
  rule_contextADT context_pv = NULL;
  firedADT fired_bv = NULL;
  int num_clocks = 0;
  int old_num_clocks =0;
  approx_workstackADT workstack=NULL;
  approx_workADT current_work=NULL;
  clockkeyADT clockkey;
  clockkeyADT old_clockkey;
  processkeyADT processkey=NULL;
  int fire_enabling;
  int fire_enabled;
  bool event_fired=0;
  int sn=0;
  int new_state;
  int regions=0;
  int num_events=0;
  int iter=0;
  int clock_size;
  int old_clock_size=0;
  int process_size;
  timeval t0,t1;
  double time0, time1;
  int stack_depth = 0;
  int stack_removed = 0;
  int pushed =0;
  initialize_state_table(LOADED,FALSE,state_table);
  int new_reg=0;
  int causal;
  int failing_rule=0;

  // 12/15/00 -- find the maximum stack depth from the exploration.
  unsigned int max_stack_depth = 0;
  // 12/18/00 -- Record time with the tracker.
  cpu_time_tracker time;

#if defined( MEMSTATS ) | defined( MATSTATS )
#ifndef OSX
  struct mallinfo memuse;
#else	
  malloc_statistics_t t;
#endif
#endif

  /* Initial setup stuff */

   if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".rsg");
    printf("Finding reduced state graph and storing to:  %s\n",outname);
    fprintf(lg,"Finding reduced state graph and storing to:  %s\n",outname);
    fp=Fopen(outname,"w");
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,nsigs,ninpsig);
    fprintf(fp,"STATES:\n");
  } else {
    fp = NULL;
    printf("Finding reduced state graph ... ");
    fflush(stdout);
    fprintf(lg,"Finding reduced state graph ... ");
    fflush(lg);
  }

//printf("There are %d events, %d signals, %d inpsig, %d nsigs, %d rules\n",
//nevents,  nsignals, ninpsig,  nsigs, nrules); 
  gettimeofday(&t0, NULL);
  // Rf = empty_set
  init_rule_info(rules, nevents, nrules, &exp_enabled_bv, &confl_removed_bv,
		 &fired_bv, &clocknums_iv, &context_pv,ncausal, nord);
  old_exp_enabled_bv=(char *)GetBlock((nrules+1) * sizeof(char));
  strcpy(old_exp_enabled_bv, exp_enabled_bv);

  // Rm = Ro
  marking = (markingADT)find_initial_marking(events, rules, markkey, nevents, 
					     nrules, ninpsig, nsigs, 
					     startstate, verbose);
  // end

  if(marking==NULL){  
    add_state(fp, state_table, NULL, NULL, NULL, NULL,
	      NULL, 0, NULL, NULL, NULL, NULL, nsigs, NULL, NULL,0, 0, 
	      verbose, TRUE, nrules, old_clock_size, 0,
	      bdd_state, use_bdd,marker,timeopts, -1, -1, exp_enabled_bv,
	      rules, nevents);
      return FALSE;
  }

  // M = initialize_timing( Ren, TEL )
  clock_size=mark_rules(rules, markkey, marking, nrules)+2;
  if(clock_size<MIN_CLOCK_SIZE) clock_size=MIN_CLOCK_SIZE;
  clockkey = create_clockkey(clock_size);  
  clocks = create_clocks(clock_size);
  process = create_process(clock_size);
  processkey = create_processkey(clock_size);
  process_size=clock_size;
  old_clock_size=clock_size;

    // Ren = { (e, f, l, u, b) in Rm : b(sc) };
    // Generate the initial region from enabled rules.
  num_clocks=approx_mark_initial_region(rules, clocks, marking, clockkey, 
					nevents, nrules, clocknums_iv, nsigs);
    // end
  
  // Add seperations from marked and enabled rules to process matrix
  num_events=mark_process(rules, process, processkey, clocks, 
			  clockkey, num_clocks, marking, markkey, 
			  nrules);
  //end 

  // Mark rules enabled by events in the process key as fired in fired_bv
  // if they are not initially marked rules.
  mark_no_epsilon_bv(rules, processkey, num_events, nevents, fired_bv);

  // sc = so;
  // TS = Ren x Rm x sc x Rf x M;
  // set_of_states phi = {TS};
  // set_of_transitions gamma = empty_set
  add_state(fp, state_table, NULL, NULL, NULL, NULL,
	    NULL, 0, NULL, marking->state, marking->marking,
	    marking->enablings, nsigs, clocks, clockkey, num_clocks,
	    sn, verbose, 1, nrules, old_clock_size, clock_size,
	    bdd_state, use_bdd,marker,timeopts, -1, -1, exp_enabled_bv,
	    rules, nevents);
  
  
  sn++;
  regions++;
  new_reg++;

  old_clocks=copy_clocks(num_clocks+2, clocks);
  old_clockkey=copy_clockkey(num_clocks+2, clockkey);
  old_num_clocks=num_clocks;
  old_clock_size=num_clocks+2;
  strcpy(old_exp_enabled_bv, exp_enabled_bv); 
  // rule_list RL = find_timed_enabled( TS, TEL )
  firinglist = (firinglistADT)get_t_enabled_list(rules, clocks, 
						 clockkey, marking,
						 markkey, exp_enabled_bv,
						 clocknums_iv,
						 confl_removed_bv, 
						 &num_clocks, nevents, 
						 nrules, timeopts.interleav);
#ifdef __DUMP__
  cout << "Rs: ";
  dump( cout, firinglist, events ) << endl << endl;
#endif
  // Builds context_lists for each rule in the clockkey.
  init_context(rules, marking, nsigs, nevents, events, exp_enabled_bv,
	       processkey, num_events, clockkey, num_clocks, context_pv);

#ifdef __DUMP__
    if ( event_fired )
      cout << endl << "Adding timed state -> " << endl;
    else
      cout << endl << "Adding zone -> " << endl;
    cout << "s: " << marking->state << endl << "Rm: ";
    dump( cout, nrules, marking->marking, markkey, events ) << endl;
    cout << "Rf: ";
    dump( cout, nrules, exp_enabled_bv, markkey, events ) << endl;
    cout << "Me:" << endl;
    dump( cout, num_events, processkey, process, events ) << endl;
    cout << "zone: " << num_clocks << endl;
    dump( cout, num_clocks, clockkey, events ) << endl;
    dump( cout, clocks, num_clocks) << endl;
#endif

  /* Main orbits loop, exits when there are no unexplored states */
  // while( ~done ) {
  while(1){ 
    //  print_firinglist(firinglist, events); 
 /*     print_processkey(num_events,processkey); */
    /* BREAK1*/
    if(firinglist==NULL){
      printf("System deadlocked in state %s\n", marking->state);
      free(old_exp_enabled_bv);
      return FALSE;
    }
    failing_rule = check_maximums(rules, clocks, clockkey, num_clocks);
    if(failing_rule!=0){
      printf("Verification failure: \n");
      printf("Constraint rule [%s,%s] exceeded its upper bound. \n", 
	     events[clockkey[failing_rule].enabling]->event, 
	     events[clockkey[failing_rule].enabled]->event);
      return FALSE;
    }
    fire_enabling=firinglist->enabling; // rule r = <e,f,l,u,b> = head(RL);
    fire_enabled=firinglist->enabled;
    /*    if (fire_enabled==1){
      print_clocks(clocks, num_clocks);
      print_clockkey(num_clocks, clockkey, events);
      my_print_process(process, num_events);
      print_processkey(num_events, processkey, events);
      }*/

    // egm 08/16/00 -- Optimizes this code a bit but not calling 
    // approx_get_causal_event that loops through every clock in the 
    // clockkey looking for the given rule.
    // OLD CODE:
    //   causal=approx_get_causal_event(clockkey, fire_enabling, fire_enabled,
    //                                  num_clocks);
    //
    causal=clockkey[clocknums_iv[rules[fire_enabling][fire_enabled]->marking]]
           .causal;

    // Pops head off of list and completes: rule r = <e,f,l,u,b> = head(RL);
    temp=firinglist; 
    firinglist=firinglist->next;
    free(temp); 

    // Wonk conflicting rules with the same enabling and mark them as wonked 
    // in the clockkey.
    firinglist=merge_conflicting_rules(clockkey, clocks, firinglist,
				       num_clocks, nevents, exp_enabled_bv, 
				       nrules, rules, fire_enabling, 
				       fire_enabled, clocknums_iv); 
    // Wonk clocks for conflicting rules that were removed from firinglist
    num_clocks = num_clocks - approx_conflict_fired_rules(rules, clocks, 
							  clockkey, nevents,
							  clocknums_iv, 
							  fired_bv,
							  fire_enabling, 
							  fire_enabled,
							  num_clocks); 
							  

/*     printf("firing rule {%d,%d}\n", fire_enabling, fire_enabled);   */  
    /* If there are unfired events, put this state and firinglist */
    /* on the stack to explore later.                            */
    pushed=0; 
    if(firinglist!=NULL){ // push( TS, tail( RL ) );
      approx_push_work(&workstack,  marking, firinglist, clocks, 
		       clockkey, process, processkey, 
		       num_clocks, num_events, clock_size, 
		       process_size, nrules, exp_enabled_bv,
		       confl_removed_bv, fired_bv, clocknums_iv,
		       context_pv);   
      stack_depth++;
      // Update the max depth correctly.
      if ( stack_depth > (int)max_stack_depth ) {
	max_stack_depth = stack_depth;
      }
      pushed=1;
    } 
    num_clocks=approx_remove_enabling_conflicts(rules, clocks, clockkey, 
						process, processkey,
						nevents, fire_enabling, 
						fire_enabled, 
						num_clocks, &num_events,
						fired_bv, clocknums_iv,
						exp_enabled_bv,
						confl_removed_bv);
     /*Mark this rule as exp_enabled */
    // i.e., mark the rule as fired!
    exp_enabled_bv[rules[fire_enabling][fire_enabled]->marking]='T';
    if(timeopts.genrg){
      old_marking=marking;
    }

    // Does the firing of this rule enabled an event to fire?
    // If ( forall r=<ei,f,li,ui.bi> in R, ri in Rf or exist rj in Rf such
    // that ei # ej
    if(bv_enabled_event(fire_enabled, fire_enabling, rules, nevents, 
			exp_enabled_bv)){
#ifdef __DUMP__
      // Output the timed state at this point where the event is firing.
      cout << endl << "Firing: " << events[fire_enabled]->event 
	   << "-> " << endl;
      cout << "Rc: (" << events[fire_enabling]->event << ","
	   << events[fire_enabled]->event << ")" << endl;
      cout << "Causal: " << events[causal] << endl;
      cout << "s: " << marking->state << endl << "Rm: ";
      dump( cout, nrules, marking->marking, markkey, events ) << endl;
      cout << "Rf: ";
      dump( cout, nrules, exp_enabled_bv, markkey, events ) << endl;
      cout << "Me:" << endl;
      dump( cout, num_events, processkey, process, events ) << endl;
#endif
      // Remove any disabled rules in the conflict relations.
      // Rm = Rm - {rj in R : f in choic_set(rj)}
      num_clocks=approx_remove_conflicts(rules, clocks, clockkey, process, 
					 processkey, nevents, fire_enabling, 
					 fire_enabled, num_clocks, 
					 &num_events, exp_enabled_bv,
					 confl_removed_bv, clocknums_iv,
					 fired_bv);
      old_marking=marking;
      //firing_signal=(fire_enabled-1)/2;
      // Is this a non-sequencing event?
      if(fire_enabled < (nevents-ndummy)){  
	// Rm = Rm U {<ei,fi,li,ui,bi> in R : ei = f
	// This code also checks rules that are disabled and flags
        // errors if they use diabling semantics.
	marking = (markingADT) find_new_marking(events, rules, markkey, 
						marking, fire_enabled, 
						nevents, nrules, 0, nsigs, 
						ninpsig,timeopts.disabling||
						timeopts.nofail,
						noparg);
      }
      else{
        // Rm = Rm U {<ei,fi,li,ui,bi> in R : ei = f
	// This code also checks rules that are disabled and flags
        // errors if they use diabling semantics.
        //   if (fail_on_diable) return fail;
        //   else Ren = Ren - ri;
	marking =  (markingADT) dummy_find_new_marking(events, rules, markkey,
						       marking, fire_enabled, 
						       nevents, nrules, 
						       0, nsigs,ninpsig,
						       timeopts.disabling||
						       timeopts.nofail,
						       noparg);
      }
      if(marking==NULL){
	add_state(fp,state_table,old_marking->state,old_marking->marking,
		  old_marking->enablings,old_clocks,old_clockkey,
		  old_num_clocks,old_exp_enabled_bv,NULL,NULL,NULL,
		  nsigs,NULL,NULL,0,sn,verbose,TRUE,nrules,old_clock_size,
		  0,bdd_state,use_bdd,marker,timeopts,fire_enabling,
		  fire_enabled,exp_enabled_bv,rules, nevents);
	return FALSE;
      }
      /*       print_marking(marking);  */
      // Rf = Rf - {<ei,fi,li,ui,bi> in Rf : fi = f
      bv_clear_event(fire_enabled, rules, exp_enabled_bv, confl_removed_bv, 
		     nevents, clocknums_iv, nrules);
      // Marks all rules enabling fire_enabled in fired_bv with 'T'
      bv_mark_firing(fire_enabled, rules, nevents, fired_bv);
      event_fired=1;
      //      printf("firing event %s\n", events[fire_enabled]->event);
    }
    else{ 
      event_fired=0;
    }
    // Mark any rules that use disabling semantics and that are now disabled
    // as not fired in exp_enabled_bv and fired_bv.
    // foreach (ri=<ei,fi,li,ui,bi> in Ren U Rf : ri is disabling
    //   if (!bi(sc))
    // The rest of this code is finished in find_new_marking where it
    // errors when rules are disabled.
    num_clocks=approx_check_enablings(events,rules, marking, clocks,
				      clockkey, process, processkey,
				      nevents, nrules, exp_enabled_bv,
				      fired_bv, clocknums_iv, num_clocks, 
				      &num_events, nsigs, fire_enabled);
    // If no event fired, remove the clock associate with 
    // <fired_enabling,fired_enabled> from the clock matrix, otherwise
    // add in new clocks for rules enabled by the fired event.
    if(!event_fired){
      num_clocks=approx_project_clocks(rules, clocks, clockkey, process,
				       processkey, nevents, clocknums_iv,
				       fired_bv, fire_enabling, fire_enabled, 
				       num_clocks, &num_events,0, -1, NULL, 
				       NULL);
#ifdef __DUMP__
      cout << "Fired rule: (" << events[fire_enabling]->event << ","
	   << events[fire_enabled]->event << ") -> " << endl;
      cout << "Rf: ";
      dump( cout, nrules, exp_enabled_bv, 
	    markkey, events ) << endl;
      cout << "zone: " << num_clocks << endl;
      dump( cout, num_clocks, clockkey, events ) << endl;
      dump( cout, clocks, num_clocks) << endl;
#endif
    }
    else{
      // Add in a new separation in the process matrix from the fire_enabled
      // event, add new clocks to the clocks matrix from newly enabled rules,
      // and update the separations in the clocks matrix with the separations
      // in the process matrix.
      num_clocks=approx_make_new_clocks(rules, markkey, marking, old_marking,
					&clocks, &clockkey, &process, 
					&processkey, nevents, nrules, 
					fire_enabling, fire_enabled, 
					num_clocks, &num_events, &clock_size, 
					&process_size, clocknums_iv,
					exp_enabled_bv, confl_removed_bv,
					causal, nsigs, events, context_pv,
					confl_approx);  
      // Delete any context lists for rules whose rule's enable fire_enabled 
      // but have enabling events that conflict with fire_enabling.
      clean_context_only_conflicts(context_pv, fire_enabled, nrules, markkey,
				   rules, fire_enabling);
      // Remove any clocks from the processkey that are no longer causal
      // to rules in the clockkey.
      num_clocks=approx_project_clocks(rules, clocks, clockkey, process, 
				       processkey, nevents, clocknums_iv,
				       fired_bv, fire_enabling, fire_enabled, 
				       num_clocks, &num_events, 1, -1, marking,
				       events);
    }  
    // Delete context lists for events removed from the processkey
    clean_context(context_pv, nrules, markkey, processkey, num_events);

    // M = update( TEL, M, r, Ren-Rold, event_fired);
    bv_advance_time(rules, clocks, clockkey, num_clocks);
    if(event_fired){
      calculate_mins(clocks, num_clocks);
      // Find a failing rule if one exists
      failing_rule = check_minimums(rules, clocks, clockkey, num_clocks,
				    fire_enabled);
      if(failing_rule!=0){
	printf("Verification failure: \n");
	printf("Constraint rule [%s,%s] does not meet lower bound. \n", 
	       events[clockkey[failing_rule].enabling]->event, 
	       events[clockkey[failing_rule].enabled]->event);
	 return FALSE;
      }
      // Removed constraint rules that are associated wih this 
      // fire_enabled event (i.e., fire_enabled is their enabled
      // event ).
      num_clocks = approx_project_ordering(events, rules, marking,
					   clocks, clockkey, process, processkey,
					   nevents, nrules, exp_enabled_bv,
					   fired_bv, clocknums_iv,
					   num_clocks, &num_events, nsigs, 
					   fire_enabled);
    }
    normalize(clocks, num_clocks, clockkey, rules, FALSE, NULL, 0); 
    limit_region(clocks, clockkey, num_clocks, 
		 process, processkey, num_events, rules);
    // END M = update( TEL, M, r, Ren, Ren-Rold, event_fired );

#ifdef __DUMP__
    if ( event_fired )
      cout << endl << "Adding timed state -> " << endl;
    else
      cout << endl << "Adding zone -> " << endl;
    cout << "s: " << marking->state << endl << "Rm: ";
    dump( cout, nrules, marking->marking, markkey, events ) << endl;
    cout << "Rf: ";
    dump( cout, nrules, exp_enabled_bv, markkey, events ) << endl;
    cout << "Me:" << endl;
    dump( cout, num_events, processkey, process, events ) << endl;
    cout << "zone: " << num_clocks << endl;
    dump( cout, num_clocks, clockkey, events ) << endl;
    dump( cout, clocks, num_clocks) << endl;
#endif
    
    if(event_fired || timeopts.genrg){
      // Add in the new untimed state and associated zone.
      new_state=add_state(fp, state_table, old_marking->state,
			  old_marking->marking, old_marking->enablings,
			  old_clocks, old_clockkey, old_num_clocks, 
			  old_exp_enabled_bv,
			  marking->state, marking->marking, marking->enablings,
			  nsigs, clocks, clockkey, num_clocks, sn,
			  verbose, 1, nrules, old_clock_size, clock_size,
			  bdd_state, use_bdd,marker,timeopts,
			  fire_enabling, fire_enabled, exp_enabled_bv, rules,
			  nevents);
      if(old_marking!=NULL && event_fired){
	delete_marking(old_marking);
	free(old_marking);
      }
      iter++;
    }
    // THIS IS THE NON-GENRG CODE
    else{
      //print_clockkey(num_clocks,clockkey,events);
      new_state=add_state(fp, state_table, NULL, NULL, NULL, NULL,
			  NULL,0, NULL, marking->state, marking->marking,
			  marking->enablings, nsigs, clocks, clockkey,
			  num_clocks, sn, verbose, 1, nrules, old_clock_size,
			  clock_size,bdd_state, use_bdd,marker,timeopts,
			  -1, -1, exp_enabled_bv, rules, nevents);       
      iter++;
    }
    // END ELSE
    free_region(old_clockkey, old_clocks, old_clock_size);

    // if (TS no in Phi) then
    //   Phi = S U {TS}; Gamma = Gamma U {(TSold,TS)};
    if(new_state){
      if(new_state>0){
	regions++;
#ifdef __DUMP__
	if ( event_fired )
	  cout << endl << "Adding timed state -> " << endl;
	else
	  cout << endl << "Adding zone -> " << endl;
	cout << "s: " << marking->state << endl << "Rm: ";
	dump( cout, nrules, marking->marking, markkey, events ) << endl;
	cout << "Rf: ";
	dump( cout, nrules, exp_enabled_bv, markkey, events ) << endl;
	cout << "Me:" << endl;
	dump( cout, num_events, processkey, process, events ) << endl;
	cout << "zone: " << num_clocks << endl;
	dump( cout, num_clocks, clockkey, events ) << endl;
	dump( cout, clocks, num_clocks) << endl;
#endif
      }
      else if (timeopts.supersets) {
	regions=regions+new_state+1;
	if ( timeopts.prune ) {
	  stack_removed=approx_clean_stack(&workstack, clocks, clockkey, 
					   num_clocks,  pushed, stack_depth,
					   marking, nrules);
	  stack_depth=stack_depth-stack_removed;
	}
      }
      if(event_fired) new_reg++;
#ifdef LIMIT
  if (regions > LIMIT) {
    printf (   "Too many regions (%d > max=%d). Bailing out!\n",regions,LIMIT);
    fprintf(lg,"Too many regions (%d > max=%d). Bailing out!\n",regions,LIMIT);
    fclose (lg                                                               );
    exit   (                                                    regions      );
  }
#endif
      if((regions%1000)==0){
	printf("%d iterations %d regions, %d states (stack_depth = %d)\n", iter,regions, sn, stack_depth);
	fprintf(lg,"%d iterations %d regions, %d states (stack_depth = %d)\n", iter,regions, sn, stack_depth);
#ifdef MEMSTATS
	if (use_bdd) {
#ifdef DLONG
	  printf("%ld(S),%ld(N),%ld(R),%ld(TOT)\n",
		 bdd_size(bdd_state->bddm,bdd_state->S,1),
		 bdd_size(bdd_state->bddm,bdd_state->N,1),
		 bdd_size(bdd_state->bddm,bdd_state->Reg,1),
		 bdd_total_size(bdd_state->bddm));
	  fprintf(lg,"%ld(S),%ld(N),%ld(R),%ld(TOT)\n",
		  bdd_size(bdd_state->bddm,bdd_state->S,1),
		  bdd_size(bdd_state->bddm,bdd_state->N,1),
		  bdd_size(bdd_state->bddm,bdd_state->Reg,1),
		  bdd_total_size(bdd_state->bddm));
#elsif CUDD
	  printf("nodes = %ld, dead = %d\n",bdd_state->mgr->ReadNodeCount(),
		 bdd_state->mgr->ReadDead());
	  fprintf(lg,"nodes = %ld, dead = %d\n",
		  bdd_state->mgr->ReadNodeCount(),
		  bdd_state->mgr->ReadDead());
	/*if (bdd_state->mgr->ReadDead() > bdd_state->maxdead)
	  bdd_state->maxdead = bdd_state->mgr->ReadDead();
	*/
#endif 
	}
#ifndef OSX
	memuse=mallinfo();
	printf("memory: max=%d inuse=%d free=%d \n",
	       memuse.arena,memuse.uordblks,memuse.fordblks);
	fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
		memuse.arena,memuse.uordblks,memuse.fordblks);
#else
  malloc_zone_statistics(NULL, &t);
  printf("memory: max=%d inuse=%d allocated=%d\n",
	 t.max_size_in_use,t.size_in_use,t.size_allocated);
  fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
	  t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
#ifdef MATSTATS
#ifndef OSX
	memuse=mallinfo();
#ifdef DLONG
	printf("%d %d %d %d %ld %ld %ld %ld %d %d\n",
	       iter,regions,sn,stack_depth,
	       bdd_size(bdd_state->bddm,bdd_state->S,1),
	       bdd_size(bdd_state->bddm,bdd_state->N,1),
	       bdd_size(bdd_state->bddm,bdd_state->Reg,1),
	       bdd_total_size(bdd_state->bddm),
	       memuse.arena,memuse.uordblks);
	fprintf(lg,"%d %d %d %d %ld %ld %ld %ld %d %d\n",
		iter,regions,sn,stack_depth,
		bdd_size(bdd_state->bddm,bdd_state->S,1),
		bdd_size(bdd_state->bddm,bdd_state->N,1),
		bdd_size(bdd_state->bddm,bdd_state->Reg,1),
		bdd_total_size(bdd_state->bddm),
		memuse.arena,memuse.uordblks);
#endif
#endif
#endif
      } // END if((regions%1000)==0)
      if ((regions%1000)==0) {
	bdd_nukit(bdd_state,0);
      }
      if((new_state==1) && (event_fired)){
	sn++;
      }
      // Update the fire list with newly enabled events
      // RL = find_timed_enabled( TS, TEL );
#ifdef __DUMP__
      cout << "Before get_t_enabled zone: " << num_clocks << endl;
      dump( cout, num_events, processkey, process, events ) << endl;
      dump( cout, num_clocks, clockkey, events ) << endl;
      dump( cout, clocks, num_clocks) << endl;
#endif
      old_clocks=copy_clocks(num_clocks+2, clocks);
      old_clockkey=copy_clockkey(num_clocks+2, clockkey);
      old_num_clocks=num_clocks;
      old_clock_size=num_clocks+2;
      strcpy(old_exp_enabled_bv, exp_enabled_bv); 
      firinglist = (firinglistADT)get_t_enabled_list(rules, clocks, 
						     clockkey, marking,
						     markkey, exp_enabled_bv,
						     clocknums_iv, 
						     confl_removed_bv,
						     &num_clocks, nevents, 
						     nrules,
						     timeopts.interleav);
#ifdef __DUMP__
      cout << "After get_t_enabled zone: " << num_clocks << endl;
      dump( cout, num_clocks, clockkey, events ) << endl;
      dump( cout, clocks, num_clocks) << endl;
      cout << "Rs: ";
      dump( cout, firinglist, events ) << endl << endl;
#endif
    } // END if (new_state)
    else{
#ifdef __DUMP__
      if ( event_fired )
	cout << endl << "Failed to add timed state -> " <<  new_state << endl;
      else
	cout << endl << "Failed to add zone -> " << new_state << endl;
      cout << "s: " << marking->state << endl << "Rm: ";
      dump( cout, nrules, marking->marking, markkey, events ) << endl;
      cout << "Rf: ";
      dump( cout, nrules, exp_enabled_bv, markkey, events ) << endl;
      cout << "Me:" << endl;
      dump( cout, num_events, processkey, process, events ) << endl;
      cout << "zone: " << num_clocks << endl;
      dump( cout, num_clocks, clockkey, events ) << endl;
      dump( cout, clocks, num_clocks) << endl;
#endif
      // printf("got new work\n"); 
      // if ( stack is not empty then (TS,AL) = pop();
      // else done == true;
      current_work=approx_pop_work(&workstack);
      // Pop Stack and check a different firing sequence
      stack_depth--;
      if(current_work==NULL){
	//	my_print_graph(state_table, events);  
	gettimeofday(&t1,NULL);	
	time0 = (t0.tv_sec+(t0.tv_usec*.000001));
	time1 = (t1.tv_sec+(t1.tv_usec*.000001));
#ifdef MEMSTATS
#ifdef DLONG
	printf("%ld(S),%ld(N),%ld(R),%ld(TOT)\n",
	       bdd_size(bdd_state->bddm,bdd_state->S,1),
	       bdd_size(bdd_state->bddm,bdd_state->N,1),
	       bdd_size(bdd_state->bddm,bdd_state->Reg,1),
	       bdd_total_size(bdd_state->bddm));
	fprintf(lg,"%ld(S),%ld(N),%ld(R),%ld(TOT)\n",
		bdd_size(bdd_state->bddm,bdd_state->S,1),
		bdd_size(bdd_state->bddm,bdd_state->N,1),
		bdd_size(bdd_state->bddm,bdd_state->Reg,1),
		bdd_total_size(bdd_state->bddm));
#elsif CUDD
	printf("nodes = %ld, dead = %d\n",bdd_state->mgr->ReadNodeCount(),
	       bdd_state->mgr->ReadDead());
	fprintf(lg,"nodes = %ld, dead = %d\n",bdd_state->mgr->ReadNodeCount(),
	       bdd_state->mgr->ReadDead());
	/*if (bdd_state->mgr->ReadDead() > bdd_state->maxdead)
	  bdd_state->maxdead = bdd_state->mgr->ReadDead();
	*/
#endif
#ifndef OSX
	memuse=mallinfo();
	printf("memory: max=%d inuse=%d free=%d \n",
	       memuse.arena,memuse.uordblks,
	       memuse.fordblks);
	fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
		memuse.arena,memuse.uordblks,
		memuse.fordblks);
#else
	malloc_zone_statistics(NULL, &t);
	printf("memory: max=%d inuse=%d allocated=%d\n",
	       t.max_size_in_use,t.size_in_use,t.size_allocated);
	fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
	bdd_nukit(bdd_state,0);
#endif
	printf("done\n");
	printf("%d iterations, %d regions, %d new_regions, %d states in %g seconds\n",
	       iter,regions, new_reg, sn, time1-time0);
	fprintf(lg,"done\n");
	fprintf(lg,"%d iterations, %d regions, %d new_regions, %d states in %g seconds\n",
	       iter,regions, new_reg, sn, time1-time0);

#ifdef MEMSTATS
#ifdef DLONG
	printf("%ld(S),%ld(N),%ld(R),%ld(TOT)\n",
	       bdd_size(bdd_state->bddm,bdd_state->S,1),
	       bdd_size(bdd_state->bddm,bdd_state->N,1),
	       bdd_size(bdd_state->bddm,bdd_state->Reg,1),
	       bdd_total_size(bdd_state->bddm));
	fprintf(lg,"%ld(S),%ld(N),%ld(R),%ld(TOT)\n",
		bdd_size(bdd_state->bddm,bdd_state->S,1),
		bdd_size(bdd_state->bddm,bdd_state->N,1),
		bdd_size(bdd_state->bddm,bdd_state->Reg,1),
		bdd_total_size(bdd_state->bddm));
#elsif CUDD
	if (use_bdd) bdd_state->mgr->info();
	printf("nodes in use = %ld, dead = %d\n",
	       bdd_state->mgr->ReadNodeCount(),
	       bdd_state->mgr->ReadDead());
	fprintf(lg,"nodes in use = %ld, dead = %d\n",
	       bdd_state->mgr->ReadNodeCount(),
	       bdd_state->mgr->ReadDead());
	/*printf("maxdead = %d\n",bdd_state->maxdead);*/
#endif
#ifndef OSX
	memuse=mallinfo();
	printf("memory: max=%d inuse=%d free=%d\n",
	       memuse.arena,memuse.uordblks,
	       memuse.fordblks);
	fprintf(lg,"memory: max=%d inuse=%d free=%d\n",
		memuse.arena,memuse.uordblks,
		memuse.fordblks);
#else
	malloc_zone_statistics(NULL, &t);
	printf("memory: max=%d inuse=%d allocated=%d\n",
	       t.max_size_in_use,t.size_in_use,t.size_allocated);
	fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
#ifdef MATSTATS
	memuse=mallinfo();
#ifdef DLONG
	printf("%d %d %d %d %ld %ld %ld %ld %d %d\n",
	       iter,regions,sn,stack_depth,
	       bdd_size(bdd_state->bddm,bdd_state->S,1),
	       bdd_size(bdd_state->bddm,bdd_state->N,1),
	       bdd_size(bdd_state->bddm,bdd_state->Reg,1),
	       bdd_total_size(bdd_state->bddm),
	       memuse.arena,memuse.uordblks);
	fprintf(lg,"%d %d %d %d %ld %ld %ld %ld %d %d\n",
		iter,regions,sn,stack_depth,
		bdd_size(bdd_state->bddm,bdd_state->S,1),
		bdd_size(bdd_state->bddm,bdd_state->N,1),
		bdd_size(bdd_state->bddm,bdd_state->Reg,1),
		bdd_total_size(bdd_state->bddm),
		memuse.arena,memuse.uordblks);
#endif
#endif
	// Clean up memory
	approx_free_structures(marking, clocks, clockkey, process, processkey, 
			       clock_size, process_size, clocknums_iv,
			       exp_enabled_bv, confl_removed_bv, fired_bv,
			       context_pv, nrules, num_events, num_clocks);
	if(verbose) fclose(fp);
	free(old_exp_enabled_bv);

	// Standard report that matches bap output.  This is used for 
	// awk script to grab results from the 2 methods.
	time.mark();
#ifdef MEMSTATS
#ifndef OSX
	report( lg, filename, regions, sn, max_stack_depth, 
		memuse.arena, (time.resource_time()).first );
#endif
#endif
	// We are done!
	return(TRUE);	
      }
      else{
	/* don't free the first rules matrix since the rest of the */
	/* program needs it */
	approx_free_structures(marking, clocks, clockkey, process, processkey, 
			       clock_size, process_size, clocknums_iv,
			       exp_enabled_bv, confl_removed_bv, fired_bv,
			       context_pv, nrules, num_events, num_clocks);
	// Update the everything with what was poped off the stack!
	marking=current_work->marking;
	firinglist=current_work->firinglist;
	clocks=current_work->clocks;
	clockkey=current_work->clockkey;
	processkey=current_work->processkey;
	process=current_work->process;
	num_clocks=current_work->num_clocks;
	num_events=current_work->num_events;
	clock_size=current_work->clock_size;
	process_size=current_work->process_size;
	clocknums_iv=current_work->clocknums;
	exp_enabled_bv=current_work->exp_enabled;
	confl_removed_bv= current_work->confl_removed;
	context_pv=current_work->context;
	fired_bv = current_work->fired;
	old_clocks=copy_clocks(num_clocks+2, clocks);
	old_clockkey=copy_clockkey(num_clocks+2, clockkey);
	old_num_clocks=num_clocks;
	old_clock_size=num_clocks+2;
	strcpy(old_exp_enabled_bv, exp_enabled_bv); 
	free(current_work);
#ifdef __DUMP__
	cout << endl << "Popping timed state -> " << endl;
	cout << "s: " << marking->state << endl << "Rm: ";
	dump( cout, nrules, marking->marking, markkey, events ) << endl;
	cout << "Rf: ";
	dump( cout, nrules, exp_enabled_bv, markkey, events ) << endl;
	cout << "Me:" << endl;
	dump( cout, num_events, processkey, process, events ) << endl;
	cout << "zone: " << num_clocks << endl;
	dump( cout, num_clocks, clockkey, events ) << endl;
	dump( cout, clocks, num_clocks) << endl;
	cout << "Rs: ";
	dump( cout, firinglist, events ) << endl << endl;
#endif
	
/* 	puts("got more work, current clock matrix is:");  */
/* 	print_clocks(clocks, num_clocks); */
/*  	print_clockkey(num_clocks, clockkey); */
      }
    }
  }
}




