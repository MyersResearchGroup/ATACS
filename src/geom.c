#include "orbitsrsg.h"
#include "geom.h"
#include "findrsg.h"
#include "memaloc.h"
#include "def.h"
#include "findreg.h"

#ifndef OSX
#include <malloc.h>
#else
#include <malloc/malloc.h>
#endif

#define DEBUG
#define MIN_CLOCK_SIZE 5
#define CLOCK_MULT 2

int geom_project_ordering(ruleADT **rules,  markingADT marking,
			  clocksADT clocks, clockkeyADT clockkey, 
			  int nevents, int nrules,
			  firedADT fired_bv, clocknumADT clocknums_iv,
			  int num_clocks, int nsigs, int fire_enabled){

  int i;
  int new_num_clocks=num_clocks;
  
  i=1;
  while(i<= new_num_clocks){
    if ((rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype &
	 ORDERING) && (clockkey[i].enabled == fire_enabled)) {
      new_num_clocks=geom_project_clocks(rules, clocks, clockkey, nevents, 
					 clocknums_iv, fired_bv, 
					 clockkey[i].enabling, 
					 clockkey[i].enabled,
					 new_num_clocks, -1);
    }
    else{
      i++;
    }
  }
  return new_num_clocks;
}

    



/* Checks to see if all the rules in the clock key have thier */
/* level satisfied and removes those that don't               */

int check_enablings(ruleADT **rules,  markingADT marking,
		    clocksADT clocks, clockkeyADT clockkey, 
		    int nevents, int nrules, exp_enabledADT exp_enabled_bv,
		    firedADT fired_bv, clocknumADT clocknums_iv,
		    int num_clocks, int nsigs,markkeyADT *markkey){

  int i;
  int new_num_clocks=num_clocks;
  
  i=1;
  while(i<= new_num_clocks){
    if ((rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype &
	 DISABLING) &&
	(!level_satisfied(rules, marking, nsigs, clockkey[i].enabling,
			clockkey[i].enabled))) {
      new_num_clocks=geom_project_clocks(rules, clocks, clockkey, nevents, 
					 clocknums_iv, fired_bv, 
					 clockkey[i].enabling, 
					 clockkey[i].enabled,
					 new_num_clocks, -1);
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
	
int geom_clean_stack(geom_workstackADT *workstack,
		       clocksADT clocks, clockkeyADT clockkey,
		       int num_clocks, int pushed, int stack_depth,
		       markingADT marking){

  geom_workstackADT prev_workstack;
  geom_workstackADT curr_workstack;
  geom_workstackADT old_workstack;
  geom_workADT current_work;
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
	geom_free_structures(current_work->marking, current_work->clocks,
			     current_work->clockkey,
			     current_work->clock_size, 
			     current_work->clocknums,
			     current_work->exp_enabled, 
			     current_work->confl_removed, 
			     current_work->fired);
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
      
/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int geom_mark_initial_region(ruleADT **rules, clocksADT clocks, 
			     markingADT marking, 
			     clockkeyADT clockkey,
			     int nevents, int nrules,
			     clocknumADT clocknums_iv, int nsigs,
			     bool infopt){

   int i=0,j=0; 
   int min_in_set=INFIN;
   int clock_index=1;

   /* Look at everything that could cause each event and see if it's */
   /* marked find the maximum time before and event must happen      */
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
	 if((rules[j][i]->level!=NULL) || 
	    !((rules[j][i]->lower==0) && (rules[j][i]->upper==INFIN) &&
	      infopt)) {
	   clocknums_iv[rules[j][i]->marking]=clock_index;
	   clockkey[clock_index].enabled=i;
	   clockkey[clock_index].enabling=j;
	   clockkey[clock_index].causal=j;
	   clock_index++;
	   if (rules[j][i]->upper < min_in_set){
	     min_in_set=rules[j][i]->upper;
	   }
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


int ver_project_clocks(eventADT *events, ruleADT **rules, 
		       markkeyADT *markkey, markingADT marking,
		       clocksADT clocks, clockkeyADT clockkey, 
		       int nevents, int nrules, rule_info_listADT *info,
		       int spec_enabled, int num_clocks){

  int i;
  int current_num_clocks;
  
  current_num_clocks=num_clocks;
  i=1;
  while(i<=current_num_clocks){
    if(clockkey[i].enabled==spec_enabled){
      //      current_num_clocks=geom_project_clocks(events, rules, markkey, marking,
      //					     clocks, clockkey, nevents, nrules,
      //				     info, clockkey[i].enabling, 
      //			     spec_enabled, current_num_clocks);
    }
    else{
      i++;
    }
  }
  return 0;
}


// egm -- 09/15/00
// This actually removes clocks for rules that have
// a -1 value for the enabling event in the clockkey.  The function
// merge_conflicting_rules in common_timing.c does the work of 
// determining which rules should be removed.

/* This eliminates rules that have be jointly fired since they */
/* conflict with a firing rule */

int geom_conflict_fired_rules(ruleADT **rules, clocksADT clocks,
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
      geom_project_clocks(rules, clocks, clockkey, nevents, clocknums_iv, 
			  fired_bv, -1, -1, num_clocks-num_removed, i);
      num_removed++;
      // egm -- 09/15/00
      // Replaced placed the a=a-1 type expression with the preincrement
      // operator.
      --current_num_clocks;
    }
    else i++;
  }
  return num_removed;
}

/* Eliminate clocks that have been used in this firing. */

// This is specialized to be able to delete a clock according to its
// actual clock number, or its <i,j> value in the rules matrix using
// its marking index (clocknums_iv maps rules[i][j]->marking to clock
// indexs.
int geom_project_clocks(ruleADT **rules, clocksADT clocks, 
			clockkeyADT clockkey, 
			int nevents,  clocknumADT clocknums_iv, 
			firedADT fired_bv,
			int fire_enabling, int fire_enabled, int num_clocks, 
			int rule_clocknum){
  
  int i,j,k;
  
  i=j=k=0;
  if(rule_clocknum==(-1)){
    rule_clocknum=clocknums_iv[rules[fire_enabling][fire_enabled]->marking];
  }
  /* Compress the matrix horizontally */

  for(j=0;j<=num_clocks;j++){
    // egm -- 09/15/00 Removed the while looped that incremented k until 
    // it was equal to rule_clocknum and replaced it with a more efficient
    // assignment statement.
    k = rule_clocknum;
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
  }
  /* Compress the matrix vertically */
  k=0;
  for(j=0;j<num_clocks;j++){
    // egm -- 09/15/00 Removed the while looped that incremented k until 
    // it was equal to rule_clocknum and replaced it with a more efficient
    // assignment statement.
    k = rule_clocknum;
    while(k<=num_clocks){
      clocks[k][j]=clocks[k+1][j];
      /* Only compress the clockkey the first time through */
      if(j==0){
	clockkey[k].enabling=clockkey[k+1].enabling;
	clockkey[k].enabled=clockkey[k+1].enabled;
	clockkey[k].causal=clockkey[k+1].causal;
      }
      k++;
    }
  }
  if(fire_enabling!=(-1)){
    clocknums_iv[rules[fire_enabling][fire_enabled]->marking]=(-1); 
  }
  num_clocks--;
  return num_clocks;
}

/* Add clocks to the clock matrix that have been added by the new marking. */
// A clock is not added if a rule has a NULL level expression AND infopt is
// set AND the rule has [0,inf] bounds.
int geom_make_new_clocks(ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT *orig_clocks, clockkeyADT *orig_clockkey, 
			 int nevents, int nrules,  int firing_event, 
			 int num_clocks, int *clock_size,
			 int nsigs, exp_enabledADT exp_enabled_bv,
			 confl_removedADT confl_removed_bv, 
			 clocknumADT clocknums_iv, bool infopt){      
  int i,j;
  int current;
  clocksADT clocks;
  clockkeyADT clockkey;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;

  clocks=*orig_clocks;
  clockkey=*orig_clockkey;
  for(i=0;i<nrules;i++){
    // Is the rule marked?
    if(marking->marking[i]!='F'){
      // Is this a rule and is its level satisfied and is it not an
      // ordering rule with [0,inf] bounds?
      if(rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype &&
	 level_satisfied(rules, marking, nsigs, markkey[i]->enabling, 
			 markkey[i]->enabled) &&
	 !((rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype & 
	    ORDERING) &&
	   (rules[markkey[i]->enabling][markkey[i]->enabled]->lower == 0) &&
	   (rules[markkey[i]->enabling][markkey[i]->enabled]->upper == 
	    INFIN))){
	current=rules[markkey[i]->enabling][markkey[i]->enabled]->marking;
	// Is there currently not a clock in this location and is the rules
        // not fired and not had its conflicts cleaned up?
	if((clocknums_iv[current] <= -1) && (exp_enabled_bv[current] == 'F') &&
	   (confl_removed_bv[current] == 'F')){
	  // If its level expression is not NULL OR it is not a rules with
          // [0,inf] bounds with infopt set?
	  if((rules[markkey[i]->enabling][markkey[i]->enabled]->level != NULL)
	     ||
	     !((rules[markkey[i]->enabling][markkey[i]->enabled]->lower==0) &&
	       (rules[markkey[i]->enabling][markkey[i]->enabled]->upper==
		INFIN) && infopt)){
	    // Add a new clock into the clock information.
	    num_clocks++;
	    if(num_clocks > ((*clock_size)-2)){
	      new_clocks=resize_clocks(*clock_size,(num_clocks+2),
				       //((*clock_size)*CLOCK_MULT), 
				       *orig_clocks);
	      new_clockkey=resize_clockkey(*clock_size,(num_clocks+2),
					   //((*clock_size)*CLOCK_MULT),
					   *orig_clockkey);
	      free_region(clockkey, clocks, *clock_size);
	      *orig_clocks=new_clocks;
	      *orig_clockkey=new_clockkey;
	      clocks=new_clocks;
	      clockkey=new_clockkey;
	      *clock_size=(num_clocks+2);//(*clock_size*CLOCK_MULT);
	    }
	    clocknums_iv[current]=num_clocks;
	    clockkey[num_clocks].enabling=markkey[i]->enabling;
	    clockkey[num_clocks].enabled=markkey[i]->enabled;
	    clockkey[num_clocks].causal=firing_event;
	    /* Add additional column to the clock matrix using format */
	    /* on pg 148 of Tom's thesis.                           */
	    clocks[0][num_clocks]=0;
	    for(j=1;j<num_clocks;j++){
	      clocks[j][num_clocks]=clocks[j][0];
	    }
	    clocks[num_clocks][num_clocks]=0;
	    
	    /* Add additional row to the clock matrix */
	    clocks[num_clocks][0]=0;
	    for(j=1;j<num_clocks;j++){
	      clocks[num_clocks][j]=clocks[0][j];
	    }
	  }   
	}
      }
    }
  }
  return num_clocks;
}

// This function is called when you know that a rule firing <fire_enabling,
// fire_enabled> has enabled the event fire_enabled to fire.  If this is the
// the case, then this function will delete clocks associated with rules
// the conflict with those rules that enabled fire_enabled to fire.  This is
// the case when the rule <i,j> has i that does not conflict with fire_enabling
// but has j that does conflict with fire_enabled.  Since we are firing
// fire_enabled, j can no longer fire in this trace.  Delete the clock for
// <i,j> and mark the entry for <i,fire_enabled> in confl_removed_bv as 
// having been cleaned up.
// 
int geom_remove_conflicts(ruleADT **rules, clocksADT clocks, 
			  clockkeyADT clockkey,	int nevents, 	
			  int fire_enabling, int fire_enabled,   
			  int num_clocks, exp_enabledADT exp_enabled_bv, 
			  confl_removedADT confl_removed_bv, 
			  clocknumADT clocknums_iv, firedADT fired_bv){
  int i,j;
  int new_num_clocks;

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    if((!(rules[i][fire_enabling]->conflict)) && 
       (rules[i][fire_enabled]->ruletype)){
      // If i and fire_enabling DO NOT conflict and there is
      // a rule <i,fire_enabled> that is fired in Rf and marked as F in
      // confl_removed_bv
      if(((exp_enabled_bv[rules[i][fire_enabled]->marking]=='T') ||
	  (rules[i][fire_enabled]->ruletype & ORDERING)) && 
	 (confl_removed_bv[rules[i][fire_enabled]->marking]=='F')){
	for(j=0;j<nevents;j++){
	  // Find a rule <i,j> where j conflicts with fired_enabled.
	  if(rules[i][j]->ruletype){
	    if((rules[j][fire_enabled]->conflict) && 
	       clocknums_iv[rules[i][j]->marking] > (-1)){
	      // Delete the clock for rule <i,j> and update confl_removed_bv
              // to show that you removed rules that conflict with
              // <i,fire_enabled>.
	      confl_removed_bv[rules[i][fire_enabled]->marking]='T';
	      new_num_clocks=geom_project_clocks(rules,clocks,clockkey,nevents,
						 clocknums_iv, fired_bv, 
						 i, j, new_num_clocks, -1);
	    }
	    else if(rules[j][fire_enabled]->conflict){
	      exp_enabled_bv[rules[i][j]->marking]='F';
	    }
	  }
	}
      } 
    }
  }
  return new_num_clocks;
} 


// This related to an initial state where 2 rules that have conflicting
// enabling events but have common enabled events are marked (it is a little
// wierd yes, but so is looking for rules <i,k> where i has not fired!).  
// As per Wendy's comments, this function may be a candidate for removal,
// especially since it is called in every iteration of the state space 
// exploration loop.  Should only be called for the initial marking.
int geom_remove_enabling_conflicts(ruleADT **rules, 
				   clocksADT clocks, clockkeyADT clockkey, 
				   int nevents, int fire_enabling, 
				   int fire_enabled, 
				   int num_clocks, firedADT fired_bv, 
				   clocknumADT clocknums_iv,
				   exp_enabledADT exp_enabled_bv){
  int i,k;
  int new_num_clocks;
  int fire_encoding;

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    // if fire_enabling conflicts with i and i has not already fired. then
    if(rules[fire_enabling][i]->conflict && 
       !(bv_actual_fired(i, rules, nevents, fired_bv))){
      // if there is a rule between <i,fire_enabled> then
      if(rules[i][fire_enabled]->ruletype){
	for(k=0;k<nevents;k++){
	  // find a rule <i,k> that has an active clock and set it as fired
          // in Rf (exp_enabled_bv) and remove its clock from the clock matrix.
	  if(rules[i][k]->ruletype){
	    if(clocknums_iv[rules[i][k]->marking] > (-1)){   
	      exp_enabled_bv[rules[i][k]->marking]='T';
	      new_num_clocks=geom_project_clocks(rules, clocks,clockkey, 
						 nevents, clocknums_iv, 
						 fired_bv, i, k, 
						 new_num_clocks,
						 -1);
	      
	      if(k!=fire_enabled){
		fire_encoding=fire_enabled*1000;
		if(fire_encoding < nevents){
		    printf("ERROR: remove_enabling conflicts: more than 1000 events");
		}
		//clocknums_iv[rules[i][k]->marking]= -(fire_encoding);	
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

void geom_push_work(geom_workstackADT *workstack, markingADT marking, 
		    firinglistADT firinglist, clocksADT clocks, clockkeyADT clockkey,	
		    int num_clocks, int clock_size, int nrules, 
		    exp_enabledADT old_exp_enabled,
		    confl_removedADT old_confl_removed,
		    firedADT old_fired, clocknumADT old_clocknums){

  geom_workstackADT new_workstack=0;
  geom_workADT new_work=0;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;

  new_work=(geom_workADT)GetBlock(sizeof( *new_work));

  new_work->marking=(markingADT)GetBlock(sizeof *marking);
  new_work->marking->marking=CopyString(marking->marking);
  new_work->marking->enablings=CopyString(marking->enablings);
  new_work->marking->state=CopyString(marking->state);
  new_work->marking->up=CopyString(marking->up);
  new_work->marking->down=CopyString(marking->down);

  new_work->firinglist = firinglist;
  new_clocks=(clocksADT) copy_clocks(num_clocks+2, clocks);
  new_work->clocks = new_clocks;
  new_clockkey=(clockkeyADT) copy_clockkey(num_clocks+2,clockkey);
  new_work->clockkey = new_clockkey;
  new_work->exp_enabled=(exp_enabledADT) GetBlock((nrules+1) * (sizeof(char)));
  new_work->confl_removed=(confl_removedADT) GetBlock((nrules+1) * (sizeof(char)));
  new_work->fired=(firedADT) GetBlock((nrules+1) * (sizeof(char)));
  new_work->clocknums=(clocknumADT) GetBlock((nrules+1) * (sizeof(int)));
  strcpy(new_work->exp_enabled, old_exp_enabled);
  strcpy(new_work->confl_removed, old_confl_removed);
  strcpy(new_work->fired, old_fired);
  memcpy(new_work->clocknums, old_clocknums, (nrules * (sizeof(int))));
  new_work->clock_size = num_clocks+2;
  new_work->num_clocks=num_clocks;
  new_workstack = (geom_workstackADT)GetBlock(sizeof( *new_workstack));
  new_workstack->work=new_work;
  new_workstack->next=(*workstack);
  (*workstack)=new_workstack;
}

void ver_geom_push_work(verstackADT *workstack, 
			markingADT marking, markingADT imp_marking,
			firinglistADT firinglist, firinglistADT imp_firinglist,
			clocksADT clocks, clocksADT imp_clocks, 
			clockkeyADT clockkey, clockkeyADT imp_clockkey,
			int nevents,int nimp_events,int nrules,int nimp_rules, 
			rule_info_listADT *old_info, 
			rule_info_listADT *imp_old_info,
			int num_clocks, int imp_num_clocks, int clock_size,
			int imp_clock_size){
  verstackADT new_workstack=NULL;
  verworkADT new_work=NULL;
  rule_info_listADT *new_rule_info=NULL;
  rule_info_listADT *imp_new_rule_info=NULL;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;
  clocksADT imp_new_clocks;
  clockkeyADT imp_new_clockkey;


  new_work=(verworkADT)GetBlock(sizeof( *new_work));
  new_work->marking = marking;
  new_work->imp_marking=imp_marking;
  new_work->firinglist = firinglist;
  new_work->imp_firinglist = imp_firinglist;
  new_clocks=(clocksADT) copy_clocks(clock_size, clocks);
  imp_new_clocks = (clocksADT) copy_clocks(imp_clock_size, imp_clocks);
  new_work->clocks = new_clocks;
  new_work->imp_clocks=imp_new_clocks;
  new_clockkey=(clockkeyADT) copy_clockkey(clock_size,clockkey);
  imp_new_clockkey=(clockkeyADT) copy_clockkey(imp_clock_size,imp_clockkey);
  new_work->clockkey = new_clockkey;
  new_work->imp_clockkey=imp_new_clockkey;
  new_rule_info=(rule_info_listADT *)copy_rule_info(old_info, nevents);
  imp_new_rule_info=(rule_info_listADT *)copy_rule_info(imp_old_info, 
							nimp_events);
  new_work->rule_info = new_rule_info;
  new_work->imp_rule_info=imp_new_rule_info;
  new_work->clock_size = clock_size;
  new_work->imp_clock_size=imp_clock_size;
  new_work->num_clocks=num_clocks;
  new_work->imp_num_clocks=imp_num_clocks;
  new_workstack = (verstackADT)GetBlock(sizeof( *new_workstack));
  new_workstack->work=new_work;
  new_workstack->next=(*workstack);
  (*workstack)=new_workstack;
}

			 
void geom_free_structures(markingADT marking, clocksADT clocks, 
			  clockkeyADT clockkey,int clock_size,  
			  clocknumADT clocknums_iv, 
			  exp_enabledADT exp_enabled_bv, 
			  confl_removedADT confl_removed_bv,firedADT fired_bv)
{
  int i;

  for(i=0;i<(clock_size);i++){
    free(clocks[i]);
  }
  free(clocks);
  free(clockkey);
  free(clocknums_iv);
  free(exp_enabled_bv);
  free(confl_removed_bv);
  free(fired_bv);
  delete_marking(marking);
  free(marking);
}



bool geom_rsg(char * filename,signalADT *signals,eventADT *events,
	      mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	      stateADT *state_table, markkeyADT *markkey, int nevents,
	      int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
	      char * startstate,bool si,bool verbose,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
	      int ndummy,int *ncausal,int* nord,bool noparg)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  markingADT marking = NULL;
  markingADT old_marking = NULL;
  clocksADT clocks = NULL;
  clocksADT old_clocks=NULL;
  firinglistADT firinglist = NULL;
  firinglistADT temp = NULL;
  int num_clocks = 0;
  int old_num_clocks =0;
  geom_workstackADT workstack=NULL;
  geom_workADT current_work=NULL;
  clockkeyADT clockkey;
  clockkeyADT old_clockkey;
  int fire_enabling;
  int fire_enabled;
  bool event_fired=0;
  int sn=0;
  int new_state;
  int regions=0;
  exp_enabledADT exp_enabled_bv = NULL;
  exp_enabledADT old_exp_enabled_bv = NULL;
  confl_removedADT confl_removed_bv = NULL;
  clocknumADT clocknums_iv = NULL;
  rule_contextADT context_pv = NULL;
  firedADT fired_bv = NULL;
  int clock_size;
  int old_clock_size=0;
  timeval t0,t1;
  double time0, time1;
  int stack_depth=0;
  int stack_removed = 0;
  bool ordering=0;
  int pushed=0;
  int iter=0;
  int failing_rule = 0;
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
#endif
#endif

  initialize_state_table(LOADED,FALSE,state_table);

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
    printf("Finding reduced state graph ... ");
    fflush(stdout);
    fprintf(lg,"Finding reduced state graph ... ");
    fflush(lg);
  }
/*   printf("\nThere are %d events, %d signals, %d inpsig, %d nsigs, */
/*   %d rules\n", nevents,  nsignals, ninpsig,  nsigs, nrules); */
/*   print_events(nevents, events); */
   
   gettimeofday(&t0, NULL);
   init_rule_info(rules, nevents, nrules, &exp_enabled_bv, &confl_removed_bv,
		  &fired_bv, &clocknums_iv, &context_pv,ncausal, nord);
   old_exp_enabled_bv = (exp_enabledADT)GetBlock((nrules+1) * sizeof(char));
   strcpy(old_exp_enabled_bv, exp_enabled_bv);
   marking = (markingADT)find_initial_marking(events, rules, markkey, nevents, 
					      nrules, ninpsig, nsigs, 
					      startstate, verbose);
   if(marking==NULL){
     add_state(fp, state_table, NULL, NULL, NULL, NULL,
	       NULL, 0, NULL, NULL, NULL, NULL, nsigs, NULL, NULL,0, 0, 
	       verbose, TRUE, nrules, old_clock_size, 0,
	       bdd_state, use_bdd,marker,timeopts, -1, -1, exp_enabled_bv,
	       rules, nevents);
     return FALSE;
   }
   clock_size=mark_rules(rules, markkey, marking, nrules)+2;
   //init_time_sep(rules, nevents);
   if(clock_size<MIN_CLOCK_SIZE) clock_size=MIN_CLOCK_SIZE;
   clocks = create_clocks(clock_size);
   clockkey = create_clockkey(clock_size);
   old_clock_size=clock_size;
   //   gcd_normalize(rules, nevents, nrules); /* I TOOK THIS OUT */
   num_clocks=geom_mark_initial_region(rules,clocks,marking,clockkey,
				       nevents, nrules, clocknums_iv, nsigs,
				       timeopts.infopt);
   normalize(clocks, num_clocks, clockkey, rules, FALSE, NULL, 0);
   add_state(fp, state_table, NULL, NULL, NULL, NULL,
	     NULL, 0, NULL, marking->state,marking->marking,marking->enablings,
	     nsigs, clocks, clockkey, num_clocks, sn, verbose, 1, nrules,
	     old_clock_size, clock_size,bdd_state, use_bdd,marker,timeopts,
	     -1, -1, exp_enabled_bv, rules, nevents);
   sn++;
   regions++;

   old_clocks=copy_clocks(num_clocks+2, clocks);
   old_clockkey=copy_clockkey(num_clocks+2, clockkey);
   old_num_clocks=num_clocks;
   old_clock_size=num_clocks+2;
   strcpy(old_exp_enabled_bv, exp_enabled_bv);

   firinglist = (firinglistADT)get_t_enabled_list(rules, clocks, clockkey, 
						  marking, 
						  markkey, exp_enabled_bv, 
						  clocknums_iv,
						  confl_removed_bv,
						  &num_clocks, nevents, 
						  nrules, timeopts.interleav);
    
   /* Main orbits loop, exits when there are no unexplored states */
   while(1){ 
     //     print_firinglist(firinglist, events);
     iter++;
     
     // Make sure the system is not in a deadlock state
     if(firinglist==NULL){
       printf("System deadlocked in state %s\n", marking->state);
       return FALSE;
     }
     
     // Check all constraint rules that they have not exceeded their
     // upper bounds.  If they have, flag an error and return.
     failing_rule = check_maximums(rules, clocks, clockkey, num_clocks);
     if(failing_rule!=0){
       printf("Verification failure: \n");
       printf("Constraint rule [%s,%s] exceeded its upper bound. \n", 
	      events[clockkey[failing_rule].enabling]->event, 
	      events[clockkey[failing_rule].enabled]->event);
       return FALSE;
     }
     
     // Pop a rule off the front of the firing list.
     fire_enabling=firinglist->enabling;
     fire_enabled=firinglist->enabled;     

     // If the rule has a [0,inf] bound and a NULL level,
     // then ordering is set high.
     if((rules[fire_enabling][fire_enabled]->lower==0) && 
	(rules[fire_enabling][fire_enabled]->upper==INFIN) &&
	(rules[fire_enabling][fire_enabled]->level==NULL) &&
	(timeopts.infopt)) ordering=1;
     else ordering=0;

     // Save the head of the firing list to delete later and move to the
     // next entry.  merge_conflicting_rules is an optimization that 
     // removes unnecessary rule interleavings on free_choice places where
     // the shared branches of <fire_enabling,fire_enabled> are not causal
     // to their enabled events.
     temp=firinglist;
     firinglist=firinglist->next;
     firinglist=merge_conflicting_rules(clockkey, clocks, firinglist,
					num_clocks, nevents, exp_enabled_bv, 
					nrules, rules, fire_enabling, 
					fire_enabled, clocknums_iv);
     num_clocks =  num_clocks - geom_conflict_fired_rules(rules, clocks, clockkey, 
							  nevents,  clocknums_iv,
							  fired_bv,
							  fire_enabling, 
							  fire_enabled, num_clocks);
     free(temp);
     pushed=0;
     /*    printf("firing rule {%d,%d}\n", fire_enabling, fire_enabled);  */
     /* If there are unfired events, put this state and firinglist */
     /* on the stack to explore later.                            */
     
     // Push the rest of the firing_list on the stack to be handled later.
     if(firinglist!=NULL){
       geom_push_work(&workstack, marking, firinglist, clocks, 
		      clockkey, num_clocks, clock_size, nrules, exp_enabled_bv,
		      confl_removed_bv, fired_bv, clocknums_iv);   
       stack_depth++;
       pushed=1;
     } 
     

     // This is a check against specs that show up with
     // tokens on both branches of a merge conflict.  Only seen
     // intially marked rules from the compiler.  The compiler now
     // only marks a single branch.  This function can be removed.
     num_clocks=geom_remove_enabling_conflicts(rules, clocks, clockkey, 
					       nevents, fire_enabling, 
					       fire_enabled,
					       num_clocks, fired_bv,
					       clocknums_iv, exp_enabled_bv); 

     // Mark the rule <fire_enabled,fire_enabled> as fired in Rf!
     exp_enabled_bv[rules[fire_enabling][fire_enabled]->marking]='T';   

     /*     puts("trying to fire event");  */
     if(timeopts.genrg){
      old_marking=marking;
     }

     // Does firing <fire_enabling,fire_enabled> enable an event to fire?
     if(bv_enabled_event(fire_enabled, fire_enabling, rules, nevents,
			 exp_enabled_bv)){
       // Remove tokens from rules in Ren whose enabled events conflict
       // with fire_enabled and share an enabling event with fire_enabled.
       //dump( cout, nrules, marking->marking, markkey, events ) << endl;
       //cout << "BEFORE ";
       //printf("%s -> %s\n",events[fire_enabling]->event,
       //      events[fire_enabled]->event);
       //dump( cout, num_clocks, clockkey, events ) << endl;
       //dump( cout, clocks, num_clocks) << endl;

       num_clocks=geom_remove_conflicts(rules, clocks, clockkey, nevents, 
					fire_enabling,fire_enabled,num_clocks,
					exp_enabled_bv, confl_removed_bv,
					clocknums_iv, fired_bv);

       //cout << "AFTER" << endl;
       //dump( cout, num_clocks, clockkey, events ) << endl;
       //dump( cout, clocks, num_clocks) << endl;
       //       printf("Firing event: %s\n", events[fire_enabled]->event);

       // Get the new marking that results from firing this event.
       // This will check ORDERING rule violations that are an event fires 
       // and one of its ordering rules is not enabled; it checks
       // safety violations; and it checks output disablings.
       old_marking=marking;
       if(fire_enabled < (nevents-ndummy)){
	 marking = (markingADT) find_new_marking(events,rules,markkey,marking, 
						 fire_enabled, nevents,nrules, 
						 0, nsigs,ninpsig,
						 timeopts.disabling||
						 timeopts.nofail,noparg);
       }
       else{
	 marking = (markingADT) dummy_find_new_marking(events,rules,markkey,
						       marking,fire_enabled, 
						       nevents, nrules, 
						       0,nsigs,ninpsig,
						       timeopts.disabling||
						       timeopts.nofail,
						       noparg);
       }						      	

       // No new marking was found due to either an ORDERING rule violation
       // (an event fired with an ordering rule that was not enabled),
       // the net has a safety violation, or it has an output disabling.
       if(marking==NULL){
	 add_state(fp,state_table,old_marking->state,old_marking->marking,
		   old_marking->enablings,old_clocks,old_clockkey,
		   old_num_clocks,old_exp_enabled_bv,NULL,NULL,NULL,
		   nsigs,NULL,NULL,0,sn,verbose,TRUE,nrules,old_clock_size,
		   0,bdd_state,use_bdd,marker,timeopts,fire_enabling,
		   fire_enabled,exp_enabled_bv,rules, nevents);
	 return FALSE;
       }


       // Remove the rules in Rf that are in the preset of fire_enabled.
       // Mark all rules in fired_bv in the preset of fire_enabled in 
       // fired_bv as T.
       /*       print_marking(marking);  */
       bv_clear_event(fire_enabled, rules, exp_enabled_bv, confl_removed_bv, 
		      nevents, clocknums_iv, nrules);
       bv_mark_firing(fire_enabled, rules, nevents, fired_bv);
       event_fired=1;
     }
     else{
       event_fired = 0;
     }

     // Project out the clock for the fired rule iff the rule was not
     // an ordering rule of [0,inf] rule.
     if(!ordering){
       restrict_region(rules, clocks, clockkey, fire_enabling,fire_enabled,
		       num_clocks, 0, 0, clocknums_iv);
       /*     puts("restricted region");  */
       num_clocks=geom_project_clocks(rules, clocks, clockkey, nevents, 
				      clocknums_iv, fired_bv,fire_enabling, 
				      fire_enabled, num_clocks,-1);
     }

     // If any rules in Rf or in Ren have become disabled due to this
     // event firing, then if it is a disabling rule, remove its clock
     // and mark it unfired in Ren or mark it unfired in the Rf.  If 
     // ATACS is configured to fail on disablings, then it will fail in
     // the find_new_marking or add_state function calls.  This code is
     // to clean up disabled rules.   Responsibility division: Chris
     // checks and flags errors; and Wendy cleans up timing stuff, but
     // leaves error report to Chris.
     num_clocks=check_enablings(rules, marking, clocks,
				clockkey, nevents, nrules, exp_enabled_bv,
				fired_bv, clocknums_iv, num_clocks,nsigs,
				markkey);
     
     /*     puts("projected clocks\n"); */
     /*     printf("num_clocks=%d\n", num_clocks);  */

     // If we did fire an event, then check for verification failures (i.e.,
     // No ordering rules related to fire_enabled should be expired.  If any
     // ordering rule is expired, then report an error and return false.
     if(event_fired){
       failing_rule = check_minimums(rules, clocks, clockkey, num_clocks,
				     fire_enabled);
       if(failing_rule!=0){
	 printf("Verification failure: \n");
	 printf("Constraint rule [%s,%s] does not meet lower bound. \n", 
		events[clockkey[failing_rule].enabling]->event, 
		events[clockkey[failing_rule].enabled]->event);
	 return FALSE;
       }

       // As the event has fired, it is OK to now delete the ordering rules
       // and remove their clocks from M.
       num_clocks = geom_project_ordering(rules,  marking, clocks, clockkey, 
					  nevents, nrules, fired_bv, clocknums_iv,
					  num_clocks, nsigs, fire_enabled);

       num_clocks=geom_make_new_clocks(rules, markkey, marking, &clocks,
				       &clockkey, nevents, nrules,
				       fire_enabled, num_clocks, &clock_size, 
				       nsigs, exp_enabled_bv, confl_removed_bv,
				       clocknums_iv, timeopts.infopt);
     }

     // Advance time and normalize to keep regions from growning unboundly.
     /*     puts("made new clocks");  */
     bv_advance_time(rules, clocks, clockkey, num_clocks);
    /*     puts("advanced time");  */
     normalize(clocks, num_clocks, clockkey, rules, FALSE, NULL, 0);
     //     update_time_sep(rules, num_clocks, clockkey, clocks);
     /*    puts("normalized"); */    

     // If we fired an events or we are just makring a region graph, then
     // add the state into the state table with the old data.
     // Otherwise, just add the new state without the old data.
     if(event_fired || timeopts.genrg){
       new_state=add_state(fp, state_table, old_marking->state,
			   old_marking->marking, old_marking->enablings,
			   old_clocks, old_clockkey, old_num_clocks, 
			   old_exp_enabled_bv,
			   marking->state,marking->marking, marking->enablings,
			   nsigs, clocks, clockkey, num_clocks, sn,
			   verbose, 1, nrules, old_clock_size, clock_size,
			   bdd_state, use_bdd,marker,timeopts,
			   fire_enabling, fire_enabled, exp_enabled_bv,
			   rules, nevents);
       if(old_marking!=NULL && event_fired){
	 delete_marking(old_marking);
	 free(old_marking);
       }
     }
     else{
       new_state=add_state(fp, state_table, NULL, NULL, NULL, NULL,
			   NULL,0, NULL, marking->state, marking->marking,
			   marking->enablings, nsigs, clocks, clockkey,
			   num_clocks, sn, verbose, 1, nrules, 
			   old_clock_size, clock_size,
			   bdd_state, use_bdd,marker,timeopts,
			   -1, -1, exp_enabled_bv, rules, nevents);
     }
     free_region(old_clockkey, old_clocks, old_clock_size);    

     // If the added state was a new state, then increment the region count
     if( new_state != 0 ){

       // If this is a new timed state, then increment the region count.
       if( new_state > 0 ){
	 regions++;
       }

       // If supersets is on and we did remove regions 
       // (new_state is equal to -1 * removed_regions from find_state), 
       // so update the region count.
       else if ( timeopts.supersets ) {
	 regions=regions+new_state+1;
	 if ( timeopts.prune ) {
	   // Clean the stack if prune is on!
	   stack_removed=geom_clean_stack(&workstack, clocks, clockkey, 
					  num_clocks,  pushed, stack_depth,
					  marking);
	   stack_depth=stack_depth-stack_removed;
	 }
       }     

#ifdef LIMIT
       if ( regions > LIMIT ) {
	 printf("Too many regions (%d > max=%d). Bailing out!\n",
		regions, LIMIT );
	 fprintf( lg,"Too many regions (%d > max=%d). Bailing out!\n",
		  regions, LIMIT );
	 fclose( lg );
	 exit( regions );
       }
#endif
       if( ( regions % 1000 ) == 0 ) {
	printf("%d iterations %d regions, %d states (stack_depth = %d)\n", iter,regions, sn, stack_depth);
	fprintf(lg,"%d iterations %d regions, %d states (stack_depth = %d)\n", iter,regions, sn, stack_depth);

#ifdef DLONG
	 printf("%ld nodes used \n",bdd_total_size(bdd_state->bddm));
#endif
#ifdef MEMSTATS
#ifndef OSX
	 memuse=mallinfo();
	 printf("memory: max=%d inuse=%d free=%d \n",
		memuse.arena,memuse.uordblks,memuse.fordblks);
	 fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
		 memuse.arena,memuse.uordblks,memuse.fordblks);
#else
	 malloc_statistics_t t;
	 malloc_zone_statistics(NULL, &t);
	 printf("memory: max=%d inuse=%d allocated=%d\n",
		t.max_size_in_use,t.size_in_use,t.size_allocated);
	 fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		 t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
       }
       // We added a new untimed state, increment the untimed state count.
       if(new_state==1){
	 sn++;
       }
       /*Mark this rule as exp_enabled */
       old_clocks=copy_clocks(num_clocks+2, clocks);
       old_clockkey=copy_clockkey(num_clocks+2, clockkey);
       old_num_clocks=num_clocks;
       old_clock_size=num_clocks+2;
       strcpy(old_exp_enabled_bv, exp_enabled_bv);
       firinglist = (firinglistADT)get_t_enabled_list(rules, clocks, clockkey,
						      marking, markkey, 
						      exp_enabled_bv, 
						      clocknums_iv,
						      confl_removed_bv, 
						      &num_clocks, nevents, 
						      nrules, 
						      timeopts.interleav); 
     } // Matchs if(newstate)
     else {
       // We did not add a new state to the state table, so this state has
       // has alread been found and we can pop a new item off of the stack.
       current_work=geom_pop_work(&workstack);

       // The stack is empty, so we have found all reachable states.
       // Mark the tims and report the final region and state count.
       if(current_work==NULL){
	 //	 my_print_graph(state_table, events);  
	 gettimeofday(&t1,NULL);	
	 time0 = (t0.tv_sec+(t0.tv_usec*.000001));
	 time1 = (t1.tv_sec+(t1.tv_usec*.000001));
	 if (!verbose) printf("\n");
	 printf("%d regions explored, %d states explored in %g seconds\n",
		regions,sn, time1-time0);
	 if (!verbose) printf("done\n");
	 //	 count_discrete_states(state_table, rules, nevents);
	 //	 print_seperations(nevents, rules, events);
	 if (verbose) fclose(fp);
	 return(TRUE);
       }
       else{

	 // We have not found all of the state, so clean up memory and
         // set pointers from current work to the local scope.

	 /* don't free the first rules matrix since the rest of the */
	 /* program needs it */
	 geom_free_structures(marking, clocks, clockkey, clock_size, 
			      clocknums_iv,
			      exp_enabled_bv, confl_removed_bv, fired_bv); 

	 // update the new stack depth.
	 stack_depth--;
	 marking=current_work->marking;
	 firinglist=current_work->firinglist;
	 clocks=current_work->clocks;
	 clockkey=current_work->clockkey;
	 num_clocks=current_work->num_clocks;
	 clock_size=current_work->clock_size;
	 clocknums_iv=current_work->clocknums;
	 exp_enabled_bv=current_work->exp_enabled;
	 confl_removed_bv= current_work->confl_removed;
	 fired_bv = current_work->fired;
	 //	 puts("popping stack");

	 old_clocks=copy_clocks(num_clocks+2, clocks);
	 old_clockkey=copy_clockkey(num_clocks+2, clockkey);
	 old_num_clocks=num_clocks;
	 old_clock_size=num_clocks+2;
	 strcpy(old_exp_enabled_bv, exp_enabled_bv);

	 free(current_work);
       }
     }
   }
}

bool on_firinglist(int enabling, int enabled, firinglistADT firinglist){

  firinglistADT current;

  current=firinglist;
  while(current!=NULL){
    if((current->enabling == enabling ) && (current->enabled == enabled)){
      return TRUE;
    }
    current=current->next;
  }
  return FALSE;
}

/* Remove all rules that enable the fired_event from the firinglist */

int clean_firinglist(firinglistADT *firinglist, int fired_event){
  
  firinglistADT current;
  firinglistADT prev;
  int enabling=0;
  
  current=*firinglist;
  prev=*firinglist;
  while(current!=NULL){
    if(current->enabled==fired_event){
      if(current!=*firinglist){
	prev->next = current->next;
	enabling=current->enabling;
	current=current->next;
      }
      else{
	enabling=current->enabling;
	*firinglist=current->next;
	prev=*firinglist;
	current=*firinglist;	
      }	
    }
    else{
      prev=current;
      current=current->next;
    }
  }
  return enabling;
}
      

int ver_enabled_event(int imp_eventnum, firinglistADT firinglist,
		       ruleADT **rules, int nevents, rule_info_listADT *info,
		       signalADT *signals, signalADT *imp_signals, 
		       eventADT *events){

  int i,j;
  bool event_enabled=1;
  bool i_conflict=0;  
  int start_event=0;
  int switching_signal;
  int current_event=0;
  rule_infoADT *marked_info=NULL;
  bool up_down=0;                     /* Is signal transitioning up or down? */
                                           /* Up = 1 down = 0 */
  up_down=(imp_eventnum%2);
  switching_signal=(imp_eventnum-1)/2;
  start_event=signals[switching_signal]->event;
  if(up_down==0) start_event++;            /* Falling events are always even */
  current_event=start_event;
  while((current_event < nevents) && 
	(events[current_event]->signal==switching_signal)){
    for(i=0;i<nevents;i++){
      if(rules[i][current_event]->ruletype){
	marked_info=(rule_infoADT *)get_rule_info(info, i, current_event);
	if(!marked_info->exp_enabled){
	  if(!on_firinglist(i, current_event, firinglist)){
	    for(j=0;j<nevents;j++){
	      if(rules[i][j]->conflict && rules[j][current_event]->ruletype){
		marked_info=(rule_infoADT *)get_rule_info(info, j, 
							  current_event);
		if(marked_info->exp_enabled || on_firinglist(j, current_event, 
							     firinglist)){
		  i_conflict=1;
		}
	      }
	    }
	    if(!i_conflict){
	      event_enabled=0;
	      i_conflict=0;
	    }
	  }
	}
      }
    }
    if(event_enabled) return current_event;
    else current_event=current_event+2;
  }
  return 0;
}

void add_enablings(markingADT marking, ruleADT **rules, int nevents, 
		   int ninputs){

  int nsignals;
  int i;
  
  nsignals=(nevents-1)/2;
  for(i=ninputs+1;i<nevents;i++){
    if(rules[i][i+1]->ruletype){
      if(marking->marking[rules[i][i+1]->marking]!='F' && 
	 level_satisfied(rules, marking, nsignals, i, i+1)){
	if(marking->state[(i-1)/2]=='1')
	  marking->state[(i-1)/2]='F';
      }
    }
    else{
      if(marking->marking[rules[i][i-1]->marking]!='F' && 
	 level_satisfied(rules, marking, nsignals, i, i-1)){
	if(marking->state[(i-2)/2]=='0')
	  marking->state[(i-2)/2]='R';
      }
    }
  }
}

void ver_match_clocks(int enabled, clocksADT clocks, clockkeyADT clockkey, int num_clocks,
		      int lower, eventADT *events){

  int i;
  int j;
  int orig_upper;

  for(i=1;i<=num_clocks;i++){
    if(clockkey[i].enabled==enabled){
      orig_upper=clocks[0][i];
      if(orig_upper<lower){
	printf("Warning: lower bound on spec rule %s->%s not met.\n",
	       events[clockkey[i].enabling]->event, 
	       events[clockkey[i].enabled]->event);
	for(j=0;j<=num_clocks;j++){
	  clocks[j][i]=clocks[i][j]+(orig_upper+lower);
	}
	for(j=0;j<=num_clocks;j++){
	  clocks[i][j]=clocks[i][j]-(orig_upper-lower);  
	}
      }
    }
  }
}
  
  
  

bool geom_verify(char * filename,signalADT *signals,eventADT *events,
		 ruleADT **rules, stateADT *state_table, markkeyADT *markkey, 
		 regionADT *regions, int ninputs, int nevents, 
		 int ninpsig,int nsignals, int nrules,
		 char * startstate, bool si, bool verbose){



  signalADT *imp_signals;
  eventADT *imp_events;
  ruleADT **imp_rules;
  ver_stateADT ver_state_table[PRIME];
  markkeyADT *imp_markkey;
  int i,j,s;
  level_exp newlevel=NULL;
  regionADT current_region;
  int nimp_events;

  
  /* set up new structures for verification */

  nimp_events=(2*nsignals)+1;
  /* Same number of implementation signals as spec signals */
  imp_signals=new_signals(0, FALSE, 0, nsignals, NULL);
  /* Twice as many implementation events as signals (no multiple */
  /* events per signal */
  imp_events=new_events(0, FALSE, 0, nimp_events, NULL);
  imp_rules=new_rules(0, FALSE, 0, nimp_events, NULL);
  imp_markkey=new_markkey(0, FALSE, 0, nimp_events, NULL);


  /* Initialize the signals */
  for(i=0;i<nsignals;i++){
    imp_signals[i]->name=CopyString(signals[i]->name);
    imp_signals[i]->event=(2*i)+1;  

    /* rises and falls will change when right ones in signal list */

    imp_signals[i]->riselower=1;
    imp_signals[i]->riseupper=5;
    imp_signals[i]->falllower=1;
    imp_signals[i]->fallupper=5;    
    imp_signals[i]->maxoccurrence=1;
  }

  /* Initialize the events */

  for(i=1;i<nimp_events;i++){
    imp_events[i]->event=CopyString(imp_signals[(i-1)/2]->name);
    if((i % 2)!= 0){
      strcat(imp_events[i]->event, "+");
    }
    else{
      strcat(imp_events[i]->event, "-");
    }
    imp_events[i]->dropped=FALSE;
    imp_events[i]->signal=(i-1)/2;
  }

  for(i=0;i<nimp_events;i++){
    for(j=0;j<nimp_events;j++){
      /* In the implementation the only rules are between concecutive */
      /* events, up and down.                                         */
      if((abs(i-j) != 1) || (i<=ninputs) || (j<=ninputs) ||
	 (i%2 && (j<i)) || (j%2 && (i<j))   ){
	imp_rules[i][j]->epsilon=(-1);
	imp_rules[i][j]->lower=(-1);
	imp_rules[i][j]->upper=(-1);
	imp_rules[i][j]->ruletype=NORULE;
	imp_rules[i][j]->marking=(-1);
	imp_rules[i][j]->reachable=FALSE;
	imp_rules[i][j]->conflict=FALSE;
	imp_rules[i][j]->level=NULL;
      }
      else{
	imp_rules[i][j]->ruletype=3;
	if(i > j) {
	  /* This is the rising production rule, marked if */
	  /* initially low.                                */
	  if(startstate[j/2] == '0')
	    imp_rules[i][j]->epsilon=1;
	  else
	    imp_rules[i][j]->epsilon=0;
	  imp_rules[i][j]->lower=imp_signals[(i-1)/2]->riselower;
	  imp_rules[i][j]->upper=imp_signals[(i-1)/2]->riseupper;
	}
	else{
	  /* This is the falling production rule, marked if */
	  /* initially high.                                */
	  if(startstate[i/2] == '1')
	    imp_rules[i][j]->epsilon=1;
	  else
	    imp_rules[i][j]->epsilon=0;
	  imp_rules[i][j]->lower=imp_signals[(i-1)/2]->falllower;
	  imp_rules[i][j]->upper=imp_signals[(i-1)/2]->fallupper;  
	}
	imp_rules[i][j]->reachable=TRUE;
	imp_rules[i][j]->conflict=FALSE;
	newlevel=(level_exp)GetBlock(sizeof(*newlevel));
	newlevel->next=NULL;
	newlevel->product=(char *)GetBlock((nsignals+1) * sizeof(char));	  
	current_region=regions[j];
	while(current_region!=NULL){
	  for(s=0;s<nsignals;s++){
	    if(current_region->cover[s]=='Z')
	      newlevel->product[s]='X';
	    else
	      newlevel->product[s]=current_region->cover[s];
	  }
	  imp_rules[i][j]->level=newlevel;
	  current_region=regions[i]->link;
	  if(current_region!=NULL){
	    newlevel=(level_exp)GetBlock(sizeof(*newlevel));	      
	    newlevel->next=imp_rules[i][j]->level;
	    newlevel->product=(char *)GetBlock((nsignals+1) * sizeof(char));   
	  }
	}
      }
    }
  }
  for(i=0;i<2*(nsignals-ninpsig);i=i+2){
    imp_markkey[i]->enabling=(ninpsig*2)+i+1;
    imp_markkey[i]->enabled=(ninpsig*2)+i+2;
    imp_markkey[i]->epsilon=
      imp_rules[(ninpsig*2)+i+1][(ninpsig*2)+i+2]->epsilon;
    imp_markkey[i+1]->enabling=(ninpsig*2)+i+2;
    imp_markkey[i+1]->enabled=(ninpsig*2)+i+1;
    imp_markkey[i+1]->epsilon=
      imp_rules[(ninpsig*2)+i+2][(ninpsig*2)+i+1]->epsilon;
  }

  /* Change rules that were marked redundnat in synthesis back     */
  /* to normal                                                     */

  for (i=0;i<nevents;i++){
    for(j=0;j<nevents;j++){
      if(rules[i][j]->ruletype){
	rules[i][j]->ruletype=3;
      }
    }
  }
  printf("initializing done\n");
  geom_verify1(imp_signals, imp_events, imp_rules, ver_state_table,
	       imp_markkey, nimp_events, 2*(nsignals-ninpsig),
	       nsignals-ninpsig, signals, events,
	       rules, markkey,
	       ninputs,  nevents, ninpsig, nsignals, nrules,
	       startstate, si, verbose);
  return TRUE;
}

bool geom_verify1(signalADT *imp_signals, eventADT *imp_events, 
		  ruleADT **imp_rules,
		  ver_stateADT *state_table, markkeyADT *imp_markkey,
		  int nimp_events, int nimp_rules, int nimp_signals, 
		  signalADT *signals, eventADT *events, ruleADT **rules,
		  markkeyADT *markkey,
		  int ninputs, int nevents, int ninpsig,
		  int nsigs, int nrules,
		  char * startstate, bool si, bool verbose){



  return TRUE;

}
	  






