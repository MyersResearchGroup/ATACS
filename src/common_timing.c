#include <sstream>

#include "common_timing.h"
#define DEBUG
#define EMPTY -10000
#define CLOCK_MULT 2
#define PROCESS_MULT 2

int state_count=0;
int old_state_count=0;

int _UNBOUNDED_ = INT_MAX - 1;

//////////////////////////////////////////////////////////////////////////
// Prints the rules in the firinglist Rf
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, firinglistADT Rf, eventADT *events ) {
  firinglistADT i = Rf;
  s << "{";
  while ( i != NULL ) {
    s << "(" << events[i->enabling]->event << ","
      << events[i->enabled]->event << ")";
    if ( (i = i->next) != NULL ) {
      s << ",";
    } 
  }
  s << "}";
  return( s );
}

//////////////////////////////////////////////////////////////////////////
// returns the length of the longest event name in the POSET
//////////////////////////////////////////////////////////////////////////
unsigned int max_size( int size, processkeyADT processkey, 
                       eventADT *events, bool zero_based = false) {
  unsigned int max = 0;
  int end = (zero_based) ? size - 1 : size;
  for ( int i = (zero_based) ? 0 : 1 ; i <= end ; ++i ) {
    unsigned int cur = strlen( events[processkey[i].eventnum]->event );
    if ( cur > max )
      max = cur;
  }
  return( max );
}

//////////////////////////////////////////////////////////////////////////
// Prints the POSET in matrix form with human readable labels
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, int size, 
	       processkeyADT processkey, processADT dbm, 
	       eventADT *events, bool zero_based){
  /* if ( size == 0 ) { */
  /*   return( s ); */
  /* } */
  /* int end = (zero_based) ? size - 1 : size; */
  /* unsigned int w = max_size( size, processkey, events, zero_based ) + 1; */
  /* // Output the column labels */
  /* s << setw( w+1 ) << " ";  */
  /* for ( int i = (zero_based) ? 0 : 1 ; i <= end ; ++i ) { */
  /*   s << setw( w ) << events[processkey[i].eventnum]->event; */
  /* } */
  /* s << endl << setw( w+1 ) << " "; */
  /* for ( int i = (zero_based) ? 0 : 1 ; i <= end ; ++i ) { */
  /*   s << setw( w ) << setfill( '-' ) << "-" << setfill( ' ' ); */
  /* } */
  /* // Print the matrix with Row identifiers. */
  /* s << endl; */
  /* for ( int i = (zero_based) ? 0 : 1 ; i <= end ; ++i ) { */
  /*   if ( i != ((zero_based) ? 0 : 1) ) s << endl; */
  /*   s << setw( w ) << events[processkey[i].eventnum]->event << "|"; */
  /*   for ( int j = (zero_based) ? 0 : 1 ; j <= end ; ++j ) { */
  /*     if ( dbm[i][j] == INFIN || dbm[i][j] == _UNBOUNDED_ ) { */
  /* 	s << setw( w ) << "U"; */
  /*     } */
  /*     else { */
  /* 	s << setw( w ) << dbm[i][j]; */
  /*     } */
  /*   } */
  /* } */
  /* s << endl; */
  return( s );
}

//////////////////////////////////////////////////////////////////////////
// prints the clocks key in a more human readable format that follows
// the clocks matrix better.
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, int num_clocks, 
	       clockkeyADT clockkey, eventADT *events ) {
  for( int i = 0 ; i <= num_clocks ; ) {
    if ((clockkey[i].enabling >= 0) && 
        (clockkey[i].enabled >= 0)) {
      cout << "r" << i << ": (" 
           << events[clockkey[i].enabling]->event << ","
           << events[clockkey[i].enabled]->event  << ")";
    } else if (clockkey[i].enabled >= 0) {
      cout << "r" << i << ": (" 
           << events[clockkey[i].enabled]->event  << ")";
    }
    if ( ++i <= num_clocks )
      cout << endl;
  }
  return( s );
}

//////////////////////////////////////////////////////////////////////////
// prints the clocks in a more human readable format.
//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, clocksADT dbm, int num_clocks){
  if ( num_clocks == 0 ) {
    return( s );
  }
  /* unsigned int w = 6; */
  /* // Output the column labels */
  /* s << setw( w+1 ) << " ";  */
  /* for ( int i = 0 ; i <= num_clocks ; ++i ) { */
  /*   ostringstream tmp; */
  /*   tmp << "r" << i << ends; */
  /*   s << setw( w ) << tmp.str(); */
  /* } */
  /* s << endl << setw( w+1 ) << " "; */
  /* for ( int i = 0 ; i <= num_clocks ; ++i ) { */
  /*   s << setw( w ) << setfill( '-' ) << "-" << setfill( ' ' ); */
  /* } */
  /* // Print the matrix with Row identifiers. */
  /* s << endl; */
  /* for ( int i = 0 ; i <= num_clocks ; ++i ) { */
  /*   if ( i ) s << endl; */
  /*   ostringstream tmp; */
  /*   tmp << "r" << i << ends; */
  /*   s << setw( w ) << tmp.str() << "|"; */
  /*   for ( int j = 0 ; j <= num_clocks ; ++j ) { */
  /*     if ( dbm[i][j] == INFIN || dbm[i][j] == _UNBOUNDED_ ) { */
  /*       s << setw( w ) << "U"; */
  /*     } */
  /*     else { */
  /*       s << setw( w ) << dbm[i][j]; */
  /*     } */
  /*   } */
  /* } */
  /* s << endl; */
  return( s );
}  

ostream& dump( ostream& s, 
               int marking_size, 
               const char* Rf, 
               markkeyADT *markkey,
               eventADT *events  ) {
  s << "{";
  bool comma = false;
  for ( int i = 0 ; i < marking_size ; ++i ) {
    if ( Rf[i] != 'F' ) {
      if ( comma ) {
        s << ",";
      }
      s << "(" << events[markkey[i]->enabling]
        << "," << events[markkey[i]->enabled]
        << ")";
      comma = true;
    }
  }
  s << "}";
  return( s );
}


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
	     double user_time ) {
  fprintf( stdout, "Found %d zones in %d states for %s ", 
	   zone_count, state_count, fname );
  fprintf( lg, "Found %d zones in %d states for %s ", 
	   zone_count, state_count, fname );
  fprintf( stdout, "(%dd %dm %gu)\n", max_stack_depth, max_memory, user_time );
  fprintf( lg, "(%dd %dm %gu)\n", max_stack_depth, max_memory, user_time );
}

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
				  int nsigs ) {
  // This handles fired rules that are disabled by the change of state.
  for( int i=0;i< nevents;i++){
    for( int j=0;j< nevents ;j++){
      // Is there a rule <i,j>?
      if(rules[i][j]->ruletype){
	// Is this ruled fired?
	if(exp_enabled_bv[rules[i][j]->marking]=='T'){
	  // Is the rule <i,j> using disabling semantics and is its
          // level nolonger enabled?
	  if ((rules[i][j]->ruletype & DISABLING) &&
	      (!level_satisfied(rules, marking, nsigs,i,j))) {
	    // Mark the rule as not fired.
	    exp_enabled_bv[rules[i][j]->marking]='F';
	    fired_bv[rules[i][j]->marking]='F';
	  }
	}
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////

/* This function checks to see if a rule has no chance of being a constraining 
   causal rule in the current rule firing sequence.  This is the case if a 
   rule r enables an event e, at least one other rule that enables e is not 
   marked, and all of the rules that enable e have the same time bounds.     */

bool constraining(ruleADT **rules, markingADT marking, markkeyADT *markkey,
		  int fire_enabled, int fire_enabling, int nrules){

  int i,j;
  bool all_marked=TRUE;
  bool constraining=TRUE;
  
  for(i=0;i<nrules;i++){
    // Is this rule <enabling,fire_enabled> is not marked with no conflicts
    // between i.enabling and fire_enabling
    if( markkey[i]->enabled==fire_enabled &&
	markkey[i]->enabling!=0 && 
	marking->marking[i]=='F' && 
	!(rules[markkey[i]->enabling][fire_enabling]->conflict) ) {
      all_marked=FALSE;
      // Set all_marked to FALSE and find another rule <j,fire_enabled>
      // that is marked and does conflict at i.enabling and j.enabling.
      // If such a rule is found, set all_marked back to TRUE.
      for(j=0;j<nrules;j++){
	if( markkey[j]->enabled==fire_enabled &&
	    markkey[j]->enabling!=0 && 
	    marking->marking[j]!='F' && 
	    rules[markkey[i]->enabling][markkey[j]->enabling]->conflict) {
	  all_marked=TRUE;
	  // egm -- 09/19/00 Added early exit after you set all_marked to
          // TRUE.
	  break;
	}
      }
    }
    // egm -- 09/18/00 Added early exit for when all_marked does not get
    // set to TRUE:
    if ( !all_marked ) break;
  }
  if (!all_marked){
    for(i=0;i<nrules;i++){
      // All rules are not marked, so check all rules that enable
      // fire_enabled and if you find a rule <i,fire_enabled> that has
      // a different upper or lower bound than <fire_enabling,fire_enabled> 
      // then break  out of loop and return TRUE.  Otherwise, return false.
      if ( ( markkey[i]->enabled==fire_enabled ) && 
	   ( rules[markkey[i]->enabling][fire_enabled]->upper!=
	     rules[fire_enabling][fire_enabled]->upper || 
	     rules[markkey[i]->enabling][fire_enabled]->lower!=
	     rules[fire_enabling][fire_enabled]->lower ) ){
	break;
      }
    }
    if(i==nrules) constraining=FALSE;
  }
  return constraining;
}

// This function deletes from the firing list any rules that have identical 
// enabling events with conflicting enabled events, that do not have [0,inf] 
// bounds, have a seperation of 0 in the clock matrix, have identical [l,u] 
// values, and when the other rule fires, it does not enable its enabled 
// event to fire.
// MORE CLEAR EXPLANATION (I hope) 09/19/00:
// This is an optimization for choice places.  If 2 rules at a choice place
// (i.e., conflicting enabled events and common enabling event) have 
// identical (which they should) bounded clocks and <fire_enabling,
// fire_enablied> is a rule in that conflict place, then if firing the other
// rule does not enable its enabled event to fire, then lets fire that too
// by marking it in Rf (exp_enabled_bv), and scheduling its clock for 
// deletion.  This will remove an interleaving from the trace set because the
// interleaving of the 2 rules is no longer explored.  This only works if
// the restrictions are enough to ensure that the rule being fired with 
// <fire_enabling,fire_enabled> is not causal.
firinglistADT merge_conflicting_rules(clockkeyADT clockkey, clocksADT clocks,
				      firinglistADT firinglist, int num_clocks,
				      int nevents, 
				      exp_enabledADT exp_enabled_bv,
				      int nrules, ruleADT **rules,
				      int fire_enabling, int fire_enabled,
				      clocknumADT clocknums_iv){
  
  firinglistADT current = NULL;
  firinglistADT prev = NULL;
  firinglistADT dead = NULL;
  exp_enabledADT prov_exp_enabled;
  int fired_clock_index;
  int current_clock_index;
  bool merged=0;

  // egm 08/16/00 -- added early exit for when firinglist starts out NULL
  if ( firinglist == NULL )
    return( firinglist );

  prov_exp_enabled=(exp_enabledADT) GetBlock((nrules+1) * (sizeof(char)));
  
  // egm 08/16/00 -- These 2 lines do not do anything that is not replicated
  // inside the while loop before they are needed. 
  // strcpy(prov_exp_enabled, exp_enabled_bv);
  // prov_exp_enabled[rules[fire_enabling][fire_enabled]->marking]='T';

  current=firinglist;
  prev=firinglist;
  fired_clock_index=clocknums_iv[rules[fire_enabling][fire_enabled]->marking];
  while(current!=NULL){
    merged=0;
    // egm 08/16/00 -- moved the string copy to where it is actually need, 
    // since it is only used when a conflicting rule actually needs to be 
    // removed.
    // strcpy(prov_exp_enabled, exp_enabled_bv);

    // Does the current rule have the same enabling as the rule to fire and
    // their enabled events conflict?
    if((current->enabling == fire_enabling) && 
       (rules[fire_enabled][current->enabled]->conflict)){
      current_clock_index=
	clocknums_iv[rules[current->enabling][current->enabled]->marking];
      /* If one rule is 0 infinity and the other isn't, they can't be eliminated */
      if((fired_clock_index <= (-1)) && (current_clock_index <= (-1))) {
	current=current->next;
	continue;
      }

      // egm 08/16/00 -- This code is never called since its boolean 
      // condition is identical to the one above and the continue statement 
      // will always bypass it.
      // NOTE: I do not understand the comments, since they both check the 
      // same thing event though the comments suggest they do something 
      // different.
      /* Both of them are 0 infinity, so they can be eliminated, set them
         both to 0 so the next test will always pass. */
      //  if((fired_clock_index <= (-1)) && (current_clock_index <= (-1))){
      //    fired_clock_index=0;
      //    current_clock_index=0;
      //  }

      // Do the 2 rules have 0 seperation on their clocks with identical 
      // [l,u] values?
      if((clocks[fired_clock_index][current_clock_index]==0) &&
	 (clocks[current_clock_index][fired_clock_index]==0) &&
	 (rules[current->enabling][current->enabled]->lower == 
	  rules[fire_enabling][fire_enabled]->lower) &&
	 (rules[current->enabling][current->enabled]->upper == 
	  rules[fire_enabling][fire_enabled]->upper)){
        
        // egm 08/16/00 New location of string copy.
        strcpy(prov_exp_enabled, exp_enabled_bv);
	prov_exp_enabled[rules[current->enabling][current->enabled]->marking]='T';
        // Does firing the current rule enable an event?
	if(!bv_enabled_event(current->enabled, current->enabling,
			     rules, nevents, prov_exp_enabled)){
	  exp_enabled_bv[rules[current->enabling][current->enabled]->marking]='T';
	  clocknums_iv[rules[current->enabling][current->enabled]->marking]=(-1);
	  clockkey[current_clock_index].enabling=(-1);
	  clockkey[current_clock_index].enabled=(-1);
	  merged=1;
	  if(prev!=current){
	    prev->next=current->next;
	    dead=current;
	    current=current->next;
	    free(dead);
	  }
	  else{
	    dead=current;
	    firinglist=firinglist->next;
	    current=current->next;
	    prev=current;
	    free(dead);
	  }
	}
      }
    }
    if(!merged){
      prev=current;
      current=current->next;
    }
  }
  free(prov_exp_enabled);
  return firinglist;
}
      
  

int get_partial_order_minimum(clocksADT clocks, int num_clocks){
  int i;
  int maximum;

  maximum=clocks[1][num_clocks];
  for(i=1;i<num_clocks; i++){
    if(clocks[i][num_clocks]>maximum){
      maximum=clocks[i][num_clocks];
    }
  }
  return maximum;
}



void free_region(clockkeyADT clockkey, clocksADT clocks, int num_clocks){

  int i;
  anti_causal_list current;
  anti_causal_list prev;

  if ((!clockkey)||(!clocks)) return;
  for(i=0;i<(num_clocks);i++){
    free(clocks[i]);
    current=clockkey[i].anti;
    prev=current;
    while(current!= NULL){
      current=current->next;
      free(prev);
      prev=current;
    }
  }
  free(clocks);
  free(clockkey);
}

void init_rule_info( ruleADT **rules, int nevents, int nrules,
		     exp_enabledADT *exp_enabled_bv,
		     confl_removedADT *confl_removed_bv, 
		     firedADT *fired_bv, clocknumADT *clocknums_iv,
		     rule_contextADT *context_pv,int *ncausal, int *nord ){
  int i;
  
  *exp_enabled_bv = (exp_enabledADT) GetBlock((nrules+1) * (sizeof(char)));
  *confl_removed_bv = (confl_removedADT) GetBlock((nrules+1) * (sizeof(char)));
  *fired_bv = (firedADT) GetBlock((nrules+1) * (sizeof(char)));
  *clocknums_iv = (clocknumADT) GetBlock((nrules+1) * (sizeof(int)));
  *context_pv = (rule_contextADT) GetBlock((nrules+1) * (sizeof(context_list)));
  for(i=0;i<nrules;i++){
    (*exp_enabled_bv)[i]='F';
    (*confl_removed_bv)[i]='F';
    (*fired_bv)[i]='F';
    (*clocknums_iv)[i]=(-1);
    (*context_pv)[i]=NULL;
    /* Don't want to deal with reset rules, so remove them all */
    if ( ( i < nevents ) && ( rules[0][i]->ruletype ) ) {
      // 2 types of rules may be removed by this code: Behavioral or Constraint
      // If a constraint rule is removed, then decrement nord, otherwise,
      // decrement ncausal.
      if ( (rules[0][i]->ruletype) & ORDERING ) {
	(*nord)--;
      }
      else {
	(*ncausal)--;
      }
      rules[0][i]->ruletype=NORULE;
    }
  }
  (*exp_enabled_bv)[nrules]=0;
  (*confl_removed_bv)[nrules]=0;
  (*fired_bv)[nrules]=0;
}
    

rule_info_listADT *make_rule_info(ruleADT **old_rules, int nevents){

  rule_info_listADT *new_rule_info;
  int i,j, num_rules=0, index=0;

  new_rule_info=
    (rule_info_listADT *)GetBlock(nevents * (sizeof(struct rule_info_list)));
  for(i=0;i<nevents;i++){
    num_rules=0;
    for(j=0;j<nevents;j++){
      if((old_rules[i][j]->ruletype) && (i!=0)) num_rules++;
    }
    new_rule_info[i].num_rules=num_rules;
    new_rule_info[i].path_list=NULL;
    if(i!=0){
      new_rule_info[i].list=
	(rule_infoADT **)GetBlock(num_rules * sizeof(struct rule_info *));
    }
    else{
      new_rule_info[i].list=
	(rule_infoADT **)GetBlock(sizeof(struct rule_info *));  
    }   
    index=0;
    for(j=0;j<nevents;j++){
      if((old_rules[i][j]->ruletype) && (i!=0)){
	new_rule_info[i].list[index]=
	  (rule_infoADT *)GetBlock(num_rules * sizeof(struct rule_info));
	new_rule_info[i].list[index]->enabled_event=j;
	new_rule_info[i].list[index]->enabling_event=i;
	new_rule_info[i].list[index]->exp_enabled=0;
	new_rule_info[i].list[index]->confl_removed=0;
	new_rule_info[i].list[index]->clocknum=(-1);
	new_rule_info[i].list[index]->fired=0;
	index++;
      }
      if(i==0){
	new_rule_info[0].list[0]=(rule_infoADT *)GetBlock(sizeof(struct rule_info));
	old_rules[i][j]->ruletype=0;
      }
    }
  }
  return new_rule_info;
}

rule_info_listADT *copy_rule_info(rule_info_listADT *old_info, int nevents){

  rule_info_listADT *new_rule_info;
  int i,j;

  new_rule_info=
    (rule_info_listADT *)GetBlock(nevents * (sizeof(struct rule_info_list)));
  new_rule_info[0].list=(rule_infoADT **)GetBlock(sizeof(struct rule_info *));
  new_rule_info[0].list[0]=(rule_infoADT *)GetBlock(sizeof(struct rule_info));
  new_rule_info[0].num_rules=0;
  for(i=1;i<nevents;i++){
    new_rule_info[i].num_rules=old_info[i].num_rules;
    new_rule_info[i].list=
      (rule_infoADT **)GetBlock(old_info[i].num_rules * sizeof(struct rule_info *));
      if(old_info[i].path_list!=NULL){
	new_rule_info[i].path_list=(path_listADT *)copy_path_list(old_info[i].path_list);
      }
      else{
	new_rule_info[i].path_list=NULL;
      }
    for(j=0;j<old_info[i].num_rules;j++){
      new_rule_info[i].list[j]=
	(rule_infoADT *)GetBlock(old_info[i].num_rules * sizeof(struct rule_info));     
      new_rule_info[i].list[j]->enabled_event=
	old_info[i].list[j]->enabled_event;
      new_rule_info[i].list[j]->enabling_event=
	old_info[i].list[j]->enabling_event;
      new_rule_info[i].list[j]->exp_enabled=
	old_info[i].list[j]->exp_enabled;
      new_rule_info[i].list[j]->confl_removed=
	old_info[i].list[j]->confl_removed;        
      new_rule_info[i].list[j]->clocknum=
	old_info[i].list[j]->clocknum;
      new_rule_info[i].list[j]->fired=
	old_info[i].list[j]->fired;
    }
  }
  return new_rule_info;
}  

path_listADT *copy_path_list(path_listADT *old_path){
 
  path_listADT *old_current;
  path_listADT *new_current;
  path_listADT *new_first;

  old_current=old_path;
  new_first=(path_listADT *)GetBlock(sizeof(struct path_list));
  new_current=new_first;
  while(old_current!=NULL){
    new_current->event=old_current->event;
    new_current->next=NULL;
    old_current=old_current->next;
    if(old_current!=NULL){
      new_current->next=(path_listADT *)GetBlock(sizeof(struct path_list));
      new_current=new_current->next;
    }
  }
  return new_first;
}

void add_event_to_path(int from, int to, rule_info_list *info){
  
  path_listADT *current;
  
  current=info[from].path_list;
  if(current==NULL){
    current=(path_listADT *)GetBlock(sizeof(struct path_list));
    current->event=to;
    current->next=NULL;
    info[from].path_list=current;
  }
  else{
    while(current->next!=NULL){
      current=current->next;
    }
    current->next=(path_listADT *)GetBlock(sizeof(struct path_list));
    current->next->event=to;
    current->next->next=NULL;
  }
}

int is_path(int from, int to, rule_info_list *info){

  path_listADT *current;

  current=info[from].path_list;
  while(current!=NULL){
    if(current->event==to) return 1;
    else current=current->next;
  }
  return 0;
}
 
rule_infoADT *get_rule_info(rule_info_listADT *info, int enabling, int enabled){
  int i=0;

  while((info[enabling].list[i]->enabled_event != enabled) &&
	(i<info[enabling].num_rules)) i++;
  if(i==info[enabling].num_rules){
    printf("enabling=%d, enabled=%d\n", enabling, enabled);
    printf("error in rule info");
    exit(0);
  }
  return info[enabling].list[i];
}

void print_expired(rule_info_listADT *info, int num_events){
  int i=0;
  int j=0;

  printf("Expired Enabled: ");
  for(i=1;i<=num_events;i++){
    while(j<info[i].num_rules){
      if(info[i].list[j]->exp_enabled==1){
	printf("{%d, %d} ",
	       info[i].list[j]->enabling_event, info[i].list[j]->enabled_event);
      }
      j++;
    }
    j=0;
  }
  printf("\n");
}

void my_print_conflicts(ruleADT **rules, int nevents){
  int i,j;

  printf("Conflicts: ");
  for(i=0;i<nevents;i++){
    for(j=0;j<i;j++){
      if(rules[i][j]->conflict){
	printf("{%d, %d} ", i,j);
      }
    }
  }
  printf("\n");
}
 

/* Let time advance to the maxixmum allowed by the delay ranges. */
	
void advance_time(eventADT *events, ruleADT **rules, 
		  markkeyADT *markkey, markingADT marking,
		  clocksADT clocks, clockkeyADT clockkey, 
		  int nevents, int nrules, rule_info_listADT *info,
		  int firing_event, int num_clocks){
   int i=0,j=0; 

   for(i=0;i<nevents;i++){
     for(j=0;j<info[i].num_rules;j++){
       if(info[i].list[j]->clocknum!=-1) {
	 clocks[0][info[i].list[j]->clocknum]=
	   rules[info[i].list[j]->enabling_event][info[i].list[j]->enabled_event]->upper;
       }
     }
   }     
   recanon_advance(clocks, num_clocks);
}

// check_maximums looks at the timer for each constraint rule in the clockkey 
// and checks that the timer on the constraint rule has not exceeded its 
// upper bound.  If a constraint rule has exceeded its upper bound, then
// the index of its timer in the clockkey is returned.  This is a failure
// condition under the current failure semantics.
//
// NOTE: ORDERING denotes a constraint rule in the ruleADT
int check_maximums(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
		       int num_clocks){
  int i=0; 

  for(i=1;i<=num_clocks;i++){
    if((rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype & ORDERING) &&
       (clocks[0][i] > rules[clockkey[i].enabling][clockkey[i].enabled]->upper)){
      return i;
    }
  }
  return 0;
}

// Checks that the lower bound on timed constraint rules on a given event
// are satisfied when the clock fired.  Returns 0 if all constrain rules
// pass, otherwise it returns the index of the faling rule.
int check_minimums(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
		       int num_clocks, int event){
  int i=0; 

  for(i=1;i<=num_clocks;i++){
    if((rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype & ORDERING) &&
       (clockkey[i].enabled == event) &&
       (clocks[i][0] > -rules[clockkey[i].enabling][clockkey[i].enabled]->lower)){
      return i;
    }
  }
  return 0;
}

// Advance all clocks to their upper bounds.  If a clock belongs to a 
// constraint rule, then set its upper bound to infinity.  This way it
// will not affect when things actually fire.
void bv_advance_time(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey,
		     int num_clocks){
   int i=0;

   for(i=1;i<=num_clocks;i++){
     if(rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype & ORDERING){
       clocks[0][i]=INFIN;
     }
     else{
       clocks[0][i]=rules[clockkey[i].enabling][clockkey[i].enabled]->upper;
     }
   }    
   recanon_advance(clocks, num_clocks);
}
  
/* Recanonicalization for advancing time.  */
     
void recanon_advance (clocksADT clocks, int num_clocks){
  int k,j;

  for(k=0;k<=num_clocks;k++){
    for(j=0;j<=num_clocks;j++){
      if((clocks[0][k]!=INFIN) && (clocks[k][j]!=INFIN)){
	if(clocks[0][j] > (clocks[0][k] + clocks[k][j])){
	  clocks[0][j]=(clocks[0][k] + clocks[k][j]);
	  if(clocks[0][j]<0){
	    clocks[0][j]=-1;
	  }
	}
      }
    }
  }
  if(clocks[0][0]!=0){
    printf("Warning, Matrix is invalid\n");
    clocks[0][0]=0;
  }
}

void calculate_mins(clocksADT clocks, int num_clocks){
  int k,j;

  for(k=0;k<=num_clocks;k++){
    for(j=0;j<=num_clocks;j++){
      if(clocks[j][0] > (clocks[k][0] + clocks[j][k])){
	clocks[j][0]=(clocks[k][0] + clocks[j][k]);
      }
    }
  }
}

/* Normalize the clock matrix with the algorithm on pg 153. */
// egm 08/18/00 -- The refernce is to Tom Rockicki's Ph.D. thesis
// "Representing and Modeling Digital Circuit" from Stanford University
void normalize (clocksADT clocks, int num_clocks, clockkeyADT clockkey,
		ruleADT **rules, bool spec_verify, ruleADT **imp_rules,
		int ninputs){

  int i,t;
  int j=0;
  int delta,min, fixindex;
  int *premax;
  int *fixup;

  premax = (int *)GetBlock((num_clocks+1) * (sizeof(int)));
  fixup = (int *)GetBlock((num_clocks+1) * (sizeof(int)));  
  fixindex=0;
  premax[0]=0;
  if(!spec_verify){
    for(i=1; i<=num_clocks;i++){
      if(rules[clockkey[i].enabling][clockkey[i].enabled]->upper == INFIN){
	premax[i]=rules[clockkey[i].enabling][clockkey[i].enabled]->lower;
      }
      else{
	premax[i]=rules[clockkey[i].enabling][clockkey[i].enabled]->upper+1;
      }
    }
  }
  else{
    for(i=1; i<=num_clocks;i++){
      if(clockkey[i].enabled<=ninputs){
	if(rules[clockkey[i].enabling][clockkey[i].enabled]->upper == INFIN){
	  premax[i]=rules[clockkey[i].enabling][clockkey[i].enabled]->lower;
	}
	else{
	  premax[i]=rules[clockkey[i].enabling][clockkey[i].enabled]->upper+1;
	}
      }
      else{
	if(rules[clockkey[i].enabling][clockkey[i].enabled]->upper == INFIN){
	  if((clockkey[i].enabled%2)==0){
	    premax[i]=imp_rules[clockkey[i].enabled-1][clockkey[i].enabled]->lower;
	  }
	  else{
	    premax[i]=imp_rules[clockkey[i].enabled+1][clockkey[i].enabled]->lower;     
	  }
	}
	else{
	  if((clockkey[i].enabled%2)==0){
	    premax[i]=imp_rules[clockkey[i].enabled-1][clockkey[i].enabled]->upper+1;
	  }
	  else{
	    premax[i]=imp_rules[clockkey[i].enabled+1][clockkey[i].enabled]->upper+1;     
	  }	   
	}
      }	 
    }
  }     
  for(i=0;i<=num_clocks;i++){
    fixup[i]=-1;
  }
  for(i=1; i<=num_clocks;i++){  
    if((clocks[i][0] + premax[i]) < 0){
      delta = clocks[i][0] + premax[i];
      for(j=0; j<=num_clocks; j++){
	if(clocks[i][j]!=INFIN){
	  clocks[i][j] = clocks[i][j] - delta;
	}
	if(clocks[j][i]!=INFIN){
	  clocks[j][i] = clocks[j][i] + delta;
	}
      }
    }
  }
  for(i=1;i<=num_clocks;i++){
    if(clocks[0][i] > premax[i]){
      t=premax[i];
      for(j=1;j<=num_clocks;j++){
	if(clocks[0][j] <= premax[j]){
	  min=clocks[0][j];
	}
	else{
	  min=premax[j];
	}
	if(clocks[i][j] != INFIN){
	  if(min > (t+clocks[i][j])){
	    t = min - clocks [i][j];
	  }
	}
      }
      if (t < clocks[0][i] ){
	fixup[fixindex] = i;
	clocks[0][i]=t;
	fixindex++;
      }
    }
  }
  if(fixindex != 0){
    for(i=1;i<=num_clocks;i++){
      while(j<fixindex){
	if ((clocks[i][0] + clocks[0][fixup[j]]) < clocks[i][fixup[j]]){
	  clocks[i][fixup[j]] = (clocks[i][0]+clocks[0][fixup[j]]);
	}
	j++;
      }
    }
  }
  free(premax);
  free(fixup);
}


void print_sigs (int nsignals, signalADT *signals){
  int i;
#ifdef DEBUG
  printf ("SIGNALS\n");  
  for(i=0;i<nsignals;i++){
    printf("signal name %s, event %d, riselower %d, riseupper %d,falllower %d, fallupper %d, max %d\n",signals[i]->name, signals[i]->event, signals[i]->riselower,
      signals[i]->riseupper, signals[i]->falllower, signals[i]->fallupper,
      signals[i]->maxoccurrence);
  }
#endif
}

void print_events (int nevents, eventADT *events){
  int i;
#ifdef DEBUG
  puts("EVENTS");  
  for(i=0;i<nevents;i++){
    printf("event name %s, signal %d\n", events[i]->event, events[i]->signal);
  }  
#endif
}

void print_rules (int nevents, ruleADT **rules){
  int i,j;
#ifdef DEBUG
  puts("RULES");
  for(i=0;i<nevents;i++){
    for(j=0;j<nevents;j++){
      if(rules[i][j]->ruletype){
	printf("{%d,%d}: ep=%d, lower=%d, upper=%d, ruletype=%d,reachable=%d\n",i,j,rules[i][j]->epsilon, rules[i][j]->lower, 
	       rules[i][j]->upper, 
	       rules[i][j]->ruletype, rules[i][j]->reachable);     
      } 
    }
  }
#endif
}

void print_seperations(int nevents, ruleADT **rules, eventADT *events){
  int i, j;
  puts("SEPERATIONS");
  for(i=0;i<nevents;i++){
    for(j=0;j<nevents;j++){
      if(rules[i][j]->maximum_sep!= -(INFIN)){
	printf("Maximum seperation between %s and %s is %d.\n", events[i]->event, events[j]->event,
	       rules[i][j]->maximum_sep);
      }
    }
  }
}

void print_marking(markingADT marking){
#ifdef DEBUG
  puts("Rm: ");
  printf("marking=%s enablings=%s state=%s up=%s down=%s\n", marking->marking, 
	 marking->enablings, marking->state, marking->up, marking->down);
#endif
}  

void print_clocks(clocksADT clocks, int num_clocks){
  int i,j;
#ifdef DEBUG
  for(i=0;i<num_clocks+2;i++){
    for(j=0;j<num_clocks+2;j++){
      if(clocks[i][j]!=INFIN){
	printf("%d ", clocks[i][j]);
      }
      else{
	printf("INF ");
      }
    }
    printf("\n");
  }
#endif
}

void print_firinglist(firinglistADT firinglist, eventADT *events){

  firinglistADT current;

  current=firinglist;
  printf("Firing list: ");
  while(current!=NULL){
    printf("{%s, %s} ",events[current->enabling]->event, events[current->enabled]->event);
    current=current->next;
  }
  printf("\n");

}

void print_clockkey(int num_clocks, clockkeyADT clockkey, eventADT *events){
#ifdef DEBUG  
  int i;
  anti_causal_list current;

  printf("the clockkey is:\n");
  for(i=0;i<num_clocks+1;i++){
    printf("i=%d: enabled=%s enabling=%s", i, events[clockkey[i].enabled]->event, 
	   events[clockkey[i].enabling]->event);
    if(clockkey[i].causal > 0){
      printf(" causal=%s\n",  events[clockkey[i].causal]->event);
    }
    else{
      printf("\n");
    }
    printf("Anti-causal list: ");
    current=clockkey[i].anti;
    while(current!=NULL){
      printf("%s ", events[current->event]->event);
      current=current->next;
    }
    printf("\n");
  }
#endif
}

void print_context(int nrules, rule_contextADT context, markkeyADT *markkey,
		   eventADT *events){
  
  int i;
  context_list current;
  
  for(i=0;i<nrules;i++){
    if(context[i]!=NULL){
      printf("Rule %s, %s has context: ", 
	     events[markkey[i]->enabling]->event, 
	     events[markkey[i]->enabled]->event);
      current=context[i];
      while(current!=NULL){
	printf("%s ", events[current->event]->event);
	current=current->next;
      }
      printf("\n");
    }
  }
}

  

void my_print_process(processADT process, int num_clocks){
  int i,j;
#ifdef DEBUG
  puts("PROCESS");
  for(i=0;i<num_clocks+2;i++){
    for(j=0;j<num_clocks+2;j++){
      if((process[i][j]!=INFIN) && (process[i][j]!=EMPTY)){
	printf("%d ", process[i][j]);
      }
      else if (process[i][j]==INFIN){
	printf("INF ");	
      }
      else{
	printf ("X ");
      }
    }
    printf("\n");
  }
#endif
}


void print_processkey(int num_clocks, processkeyADT processkey, 
		      eventADT *events){
#ifdef DEBUG  
  int i;
  anti_causal_list current;

  printf("the processkey is:\n");
  for(i=0;i<num_clocks+2;i++){
    if (processkey[i].eventnum!=-1){
      printf("i=%d: event=%s root=%d instance=%d\n", i, 
	     events[processkey[i].eventnum]->event,
	     processkey[i].root, processkey[i].instance);
      printf("Anti-causal list: ");
      current=processkey[i].anti;
      while(current!=NULL){
	printf("%s ", events[current->event]->event);
	current=current->next;
      }
      printf("\n");
    }
    else{
      printf("i=%d: event=%d root=%d instance=%d\n", i, 
	     processkey[i].eventnum,
	     processkey[i].root, processkey[i].instance);      
    
    }
  }
#endif
}


/* Set up the marking field in the rule matrix so it contains    */
/* the index of the position in the marking string that contains */
/* this rule's marking.                                          */

int mark_rules(ruleADT **rules, markkeyADT *markkey, markingADT marking, int nrules){

  int i,j=0;

  for(i=0;i<nrules;i++){
    rules[markkey[i]->enabling][markkey[i]->enabled]->marking=i;
    if(marking->marking[i]!='F'){
      j++;
      rules[markkey[i]->enabling][markkey[i]->enabled]->epsilon=1;
    }
    else{
      //      rules[markkey[i]->enabling][markkey[i]->enabled]->epsilon=0;
    }     
  }
  return j;
}

/* Create and initialize an nrule x nrules clock matrix */

clocksADT create_clocks(int nrules){

  clocksADT new_clocks;
  int i,j, num_clocks;

  num_clocks=nrules;
  new_clocks = (clocksADT)GetBlock(num_clocks * (sizeof(int *)));
  for(i=0;i<num_clocks;i++){
    new_clocks[i]=(int *)GetBlock(num_clocks*(sizeof(int)));
  }
  for(i=0;i<num_clocks;i++){
    for(j=0;j<num_clocks;j++){
      if(i==j) 
	new_clocks[i][j]=0;
      else	
	new_clocks[i][j]=-1;
    }
  }

  return new_clocks;
}


processADT create_process(int nrules){

  processADT new_process;
  int i,j, num_clocks;

  num_clocks=nrules;
  new_process = (processADT)GetBlock(num_clocks * (sizeof(int *)));
  for(i=0;i<num_clocks;i++){
    new_process[i]=(int *)GetBlock(num_clocks*(sizeof(int)));
  }
  for(i=0;i<num_clocks;i++){
    for(j=0;j<num_clocks;j++){
      if(i==j) 
	new_process[i][j]=0;
      else	
	new_process[i][j]=-1;
    }
  }

  return new_process;
}  


clocksADT copy_clocks(int nrules, clocksADT old_clocks){

  int i, j, num_clocks;
  clocksADT new_clocks;

  if (!old_clocks) return NULL;
  num_clocks=nrules;
  new_clocks = (clocksADT)GetBlock(num_clocks * (sizeof(int *)));
  for(i=0;i<num_clocks;i++){
    new_clocks[i]=(int *)GetBlock(num_clocks*(sizeof(int)));
  }
  for(i=0;i<num_clocks;i++){
    for(j=0;j<num_clocks;j++){
			new_clocks[i][j]=old_clocks[i][j];
    }
  }
  return new_clocks;
}


clocksADT resize_clocks(int old_size, int new_size, clocksADT old_clocks){
  int i, j;
  clocksADT new_clocks;

  new_clocks = (clocksADT)GetBlock(new_size * (sizeof(int *)));
  for(i=0;i<new_size;i++){
    new_clocks[i]=(int *)GetBlock(new_size*(sizeof(int)));
  }
  for(i=0;i<new_size;i++){
    for(j=0;j<new_size;j++){
      if((i<old_size) && (j<old_size)){
	new_clocks[i][j]=old_clocks[i][j];
      }
      else{
	new_clocks[i][j]=(-1);
      }
    }
  }
  return new_clocks;
}


clockkeyADT resize_clockkey(int old_size, int new_size, clockkeyADT old_clockkey){
  
  clockkeyADT new_clockkey;
  anti_causal_list old_current;
  anti_causal_list new_current;
  int i;


  new_clockkey = (clockkeyADT)GetBlock (new_size * sizeof (struct clockkey));
  for(i=0;i<new_size;i++){
    if(i<old_size){
      new_clockkey[i].enabled=old_clockkey[i].enabled;
      new_clockkey[i].enabling=old_clockkey[i].enabling;
      new_clockkey[i].eventk_num=old_clockkey[i].eventk_num;
      new_clockkey[i].causal=old_clockkey[i].causal;      
      if(old_clockkey[i].anti == NULL){
	new_clockkey[i].anti = NULL;
      }
      else{
	new_clockkey[i].anti = (anti_causal_list)malloc(sizeof(struct anti));
	new_current = new_clockkey[i].anti;
	old_current = old_clockkey[i].anti;
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
      }
    }
    else{
      new_clockkey[i].enabled=(-1);
      new_clockkey[i].enabling=(-1);   
      new_clockkey[i].eventk_num=(-1);
      new_clockkey[i].causal=(-1);      
      new_clockkey[i].anti=NULL;
    }   
  }
  return new_clockkey;
}  

  
processADT copy_process(int nrules, processADT old_process){

  int i, j, num_clocks;
  processADT new_process;

  num_clocks=nrules;
  new_process = (processADT)GetBlock(num_clocks * (sizeof(int *)));
  for(i=0;i<num_clocks;i++){
    new_process[i]=(int *)GetBlock(num_clocks*(sizeof(int)));
  }
  for(i=0;i<num_clocks;i++){
    for(j=0;j<num_clocks;j++){
	new_process[i][j]=old_process[i][j];
    }
  }
  return new_process;
}


clocksADT resize_process(int old_size, int new_size, processADT old_process){
  int i, j;
  processADT new_process;

  new_process = (processADT)GetBlock(new_size * (sizeof(int *)));
  for(i=0;i<new_size;i++){
    new_process[i]=(int *)GetBlock(new_size*(sizeof(int)));
  }
  for(i=0;i<new_size;i++){
    for(j=0;j<new_size;j++){
      if((i<old_size) && (j<old_size)){
	new_process[i][j]=old_process[i][j];
      }
      else{
	new_process[i][j]=(-1);
      }
    }
  }
  return new_process;
}


processkeyADT resize_processkey(int old_size, int new_size, processkeyADT old_processkey){
  
  processkeyADT new_processkey;
  anti_causal_list old_current;
  anti_causal_list new_current;  
  int i;


  new_processkey = (processkeyADT)GetBlock (new_size * sizeof (struct processkey));
  for(i=0;i<new_size;i++){
    if(i<old_size){
      new_processkey[i].eventnum=old_processkey[i].eventnum;
      new_processkey[i].root=old_processkey[i].root;
      new_processkey[i].instance=old_processkey[i].instance;
      if(old_processkey[i].anti == NULL){
	new_processkey[i].anti = NULL;
      }
      else{
	new_processkey[i].anti = (anti_causal_list)malloc(sizeof(struct anti));
	new_current = new_processkey[i].anti;
	old_current = old_processkey[i].anti;
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
      }
    }
    else{
      new_processkey[i].eventnum=(-1);
      new_processkey[i].root=(-1);
      new_processkey[i].instance=(-1);
      new_processkey[i].anti=NULL;
    }   
  }
  return new_processkey;
}  


clockkeyADT create_clockkey(int nrules){

  clockkeyADT new_clockkey;
  int i, num_clocks;

  num_clocks=nrules;
  new_clockkey = (clockkeyADT)GetBlock (num_clocks * sizeof (struct clockkey));
  for(i=0;i<num_clocks;i++){
    new_clockkey[i].enabled=(-1);
    new_clockkey[i].enabling=(-1);
    new_clockkey[i].eventk_num=(-1);
    new_clockkey[i].causal=(-1);
    new_clockkey[i].anti = NULL;
  }
  return new_clockkey;
}


processkeyADT create_processkey(int nrules){

  processkeyADT new_processkey;
  int i, num_clocks;

  num_clocks=nrules;
  new_processkey = (processkeyADT)GetBlock (num_clocks * sizeof (struct processkey));
  for(i=0;i<num_clocks;i++){
    new_processkey[i].eventnum=(-1);
    new_processkey[i].root=(-1);
    new_processkey[i].instance=(-1);
    new_processkey[i].anti=NULL;
  }
  return new_processkey;
}

clockkeyADT copy_clockkey(int nrules, clockkeyADT old_clockkey){

  clockkeyADT new_clockkey;
  int i, num_clocks;
  anti_causal_list old_current;
  anti_causal_list new_current;

  if (!old_clockkey) return NULL;
  num_clocks=nrules;
  new_clockkey = (clockkeyADT)GetBlock (num_clocks * sizeof (struct clockkey));
  for(i=0;i<num_clocks;i++){
    new_clockkey[i].enabled=old_clockkey[i].enabled;
    new_clockkey[i].enabling=old_clockkey[i].enabling;
    new_clockkey[i].eventk_num=old_clockkey[i].eventk_num;
    new_clockkey[i].causal=old_clockkey[i].causal;
    if(old_clockkey[i].anti == NULL){
      new_clockkey[i].anti = NULL;
    }
    else{
      new_clockkey[i].anti = (anti_causal_list)malloc(sizeof(struct anti));
      new_current = new_clockkey[i].anti;
      old_current = old_clockkey[i].anti;
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
    }
		new_clockkey[i].lrate=old_clockkey[i].lrate;
		new_clockkey[i].urate=old_clockkey[i].urate;
  }
  return new_clockkey;
}
 
processkeyADT copy_processkey(int nrules, processkeyADT old_processkey){
  
  processkeyADT new_processkey;
  int i, num_clocks;
  anti_causal_list old_current;
  anti_causal_list new_current;

  num_clocks=nrules;
  new_processkey = (processkeyADT)GetBlock (num_clocks * sizeof (struct processkey));
  for(i=0;i<num_clocks;i++){
    new_processkey[i].eventnum=old_processkey[i].eventnum;
    new_processkey[i].root=old_processkey[i].root;    
    new_processkey[i].instance=old_processkey[i].instance;
    if(old_processkey[i].anti == NULL){
      new_processkey[i].anti = NULL;
    }
    else{
      new_processkey[i].anti = (anti_causal_list)malloc(sizeof(struct anti));
      new_current = new_processkey[i].anti;
      old_current = old_processkey[i].anti;
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
    }    
  }
  return new_processkey;
}
 
firinglistADT get_t_enabled_list(ruleADT **rules, clocksADT clocks, 
				 clockkeyADT clockkey, markingADT marking,
				 markkeyADT *markkey, exp_enabledADT exp_enabled_bv, 
				 clocknumADT clocknums_iv,
				 confl_removedADT confl_removed_bv,
				 int *num_clocks, int nevents, int nrules, 
				 bool interleave){
  int i=1;
  firinglistADT new_rule = NULL;
  firinglistADT first_rule=NULL;
  firinglistADT current = NULL;
  char exp_enabled;
  int clocknum;
  char confl_removed;

  /* An event is timed enabled either if it is marked and has met its  */
  /* lower bound in the clock matrix or it is marked and its bounds    */
  /* are 0 to INF.  If the bounds are 0 to INF, it is not in the clock */
  /* matrix because it is just and ordering rule.                      */

  // egm 08/15/00 -- for a rule to not be added to the clock matrix, 
  // 3 conditions must hold:
  //
  //     1) ruletype & ORDERING == 1  (i.e., it is a constraint rule)
  //     2) lower bound is 0
  //     3) upper bound is infinite
  // 
  //  These are derived from approx_mark_intial_region in approx.c
  //  Need to verify this with Wendy!

  // egm 08/15/00 -- I pulled the interleave flag check at the beginning 
  // to avoid all of the non-important work that is done when interleave 
  // is false.
  if ( interleave ) {
    while(i<=(*num_clocks)){
      if((clocks[0][i]>=rules[clockkey[i].enabling][clockkey[i].enabled]->lower) &&
	 !(rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype & ORDERING) &&
	 !constraining(rules, marking, markkey, clockkey[i].enabled, clockkey[i].enabling,
			nrules)){
	exp_enabled_bv[rules[clockkey[i].enabling][clockkey[i].enabled]->marking]='T';
	approx_project_clocks(rules, clocks, clockkey, NULL,
			      NULL, nevents, clocknums_iv,
			      NULL, clockkey[i].enabling, 
			      clockkey[i].enabled, *num_clocks, 
			      NULL, 0, -1, NULL, NULL);	
	(*num_clocks)--;
      }
      else{
	i++;
      }
    }
  }

  bv_advance_time(rules, clocks, clockkey, *num_clocks);
  normalize(clocks, *num_clocks, clockkey, rules, FALSE, NULL, 0);
  for(i=1;i<=(*num_clocks);i++){  
    if((clocks[0][i]>=rules[clockkey[i].enabling][clockkey[i].enabled]->lower) &&
       !(rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype & ORDERING)){
      new_rule = (firinglistADT)GetBlock(sizeof(struct firinglist));
      new_rule->enabling=clockkey[i].enabling;
      new_rule->enabled=clockkey[i].enabled;
      new_rule->next=NULL;
      if (first_rule==NULL){
	first_rule=new_rule;
      }
      else{
	current=first_rule;
	while(current->next!=NULL){
	  current=current->next;
	}
	current->next=new_rule;
      }
    }
  }
  /* This code checks for 0/infinity rules that won't be in the */
  /* clock matrix for geometric.                                */
  for(i=0;i<nrules;i++){
    if((marking->marking[i]!='F') &&
       rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype){
      if((rules[markkey[i]->enabling][markkey[i]->enabled]->expr != NULL) ||
	 (rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype & ORDERING) ||
	 !((rules[markkey[i]->enabling][markkey[i]->enabled]->lower==0) &&
	   (rules[markkey[i]->enabling][markkey[i]->enabled]->upper==INFIN)))
	continue;
      else{
	exp_enabled=
	  exp_enabled_bv[rules[markkey[i]->enabling][markkey[i]->enabled]->marking];
	clocknum=clocknums_iv[rules[markkey[i]->enabling][markkey[i]->enabled]->marking];
	confl_removed=
	  confl_removed_bv[rules[markkey[i]->enabling][markkey[i]->enabled]->marking];
	if((clocknum <= (-1)) && (exp_enabled=='F') && (confl_removed=='F')){
	  new_rule = (firinglistADT)GetBlock(sizeof(struct firinglist));
	  new_rule->enabling=markkey[i]->enabling;
	  new_rule->enabled=markkey[i]->enabled;
	  new_rule->next=NULL;
	  if (first_rule==NULL){
	    first_rule=new_rule;
	  }
	  else{
	    current=first_rule;
	    while(current->next!=NULL){
	      current=current->next;
	    }
	    current->next=new_rule;
	  }
	}
      }
    }
  }
  return first_rule;
}

firinglistADT old_get_t_enabled_list(ruleADT **rules, clocksADT clocks, 
				     clockkeyADT clockkey, 
				     int num_clocks, int nevents, int nrules){
  int i;
  firinglistADT new_rule = NULL;
  firinglistADT first_rule=NULL;
  firinglistADT current = NULL;


  for(i=1;i<=num_clocks;i++){

    /* If the a rule is marked, exists, and has an old enough clock, it is timed */
    /* enabled. */
    if(clocks[0][i]>=rules[clockkey[i].enabling][clockkey[i].enabled]->lower)
    {      
      /* This event is timed enabled, add it to the event list */

      new_rule = (firinglistADT)GetBlock(sizeof(struct firinglist));
      new_rule->enabling=clockkey[i].enabling;
      new_rule->enabled=clockkey[i].enabled;
      new_rule->next=NULL;
      if (first_rule==NULL){
	first_rule=new_rule;
      }
      else{
	current=first_rule;
	while(current->next!=NULL){
	  current=current->next;
	}
	current->next=new_rule;
      }
    }
  }
  return first_rule;
}

firinglistADT ver_get_t_enabled_list(ruleADT **rules, clocksADT clocks, 
				      clockkeyADT clockkey, 
				      int num_clocks, int nevents, int nrules,
				      int ninputs){
  
  int i;
  firinglistADT new_rule = NULL;
  firinglistADT first_rule=NULL;
  firinglistADT current = NULL;


  for(i=1;i<=num_clocks;i++){

    /* If the a rule is marked, exists, and has an old enough clock, it is timed */
    /* enabled.  Or if it enabled by an input for verification */
    if((clocks[0][i]>=rules[clockkey[i].enabling][clockkey[i].enabled]->lower) ||
       (clockkey[i].enabling <= ninputs))
    {      
      /* This event is timed enabled, add it to the event list */

      new_rule = (firinglistADT)GetBlock(sizeof(struct firinglist));
      new_rule->enabling=clockkey[i].enabling;
      new_rule->enabled=clockkey[i].enabled;
      new_rule->next=NULL;
      if (first_rule==NULL){
	first_rule=new_rule;
      }
      else{
	current=first_rule;
	while(current->next!=NULL){
	  current=current->next;
	}
	current->next=new_rule;
      }
    }
  }
  return first_rule;
}


/* Determine if the given event is enabled. */

bool enabled_event(int eventnum, int enabling, ruleADT **rules, int nevents,
		   rule_info_listADT *info){

  int i,j;
  bool event_enabled=1;
  bool i_conflicts=0;
  rule_infoADT *marked_info=NULL;

  for(i=0;i<nevents;i++){
    i_conflicts=0;
    if((rules[i][eventnum]->ruletype) && (!(rules[i][eventnum]->ruletype & ORDERING)) &&
       (rules[i][enabling]->conflict!=1) && (rules[i][eventnum]->reachable)){
      marked_info=(rule_infoADT *)get_rule_info(info, i, eventnum);
      if(!marked_info->exp_enabled){
	for(j=0;j<nevents;j++){
	  if(rules[i][j]->conflict && rules[j][eventnum]->ruletype){
	    marked_info=(rule_infoADT *)get_rule_info(info, j, eventnum);
	    if(marked_info->exp_enabled){
	      i_conflicts=1;
	    }
	  }
	}
	if(!i_conflicts){
	  event_enabled=0;
	  i_conflicts=0;
	}
      }
    }
  }
  return event_enabled;
}

// egm 08/16/00 -- I added an early exit from this function and I removed 
// the need for the event_enabled flag.
// NOTE: This function returns true iff eventnum can fire.  If the 
// comments do not help in understanding the function, then draw the 
// dependency picture.  It will help clarify the comments.
bool bv_enabled_event(int eventnum, int enabling, ruleADT **rules, int nevents,
		      exp_enabledADT exp_enabled_bv){

  int i,j;
  bool i_conflicts = false;

  for(i=1;i<nevents;i++){
    i_conflicts = false;
    // Is there a rule <i,eventnum> that is not a constraint rule and does not 
    // conflict with enabling (i.e., it is not a merge on conflicting 
    // paths).
    if( rules[i][eventnum]->ruletype                 &&
	!( rules[i][eventnum]->ruletype & ORDERING ) &&
        rules[i][enabling]->conflict != 1 ) {
      // Is this rule not fired?
      if(exp_enabled_bv[rules[i][eventnum]->marking]=='F'){
	for(j=1;j<nevents;j++){
	  // The rule is not fired, so is there another rule <j,eventnum> 
          // that is fired and conflicts with i?
	  if( rules[i][j]->conflict && 
	      rules[j][eventnum]->ruletype &&
	      !( rules[j][eventnum]->ruletype & ORDERING ) ){
            // Is this rule fired?
	    if(exp_enabled_bv[rules[j][eventnum]->marking]=='T'){
	      i_conflicts = true;  // The rule event that conflicts with i is 
	      break;               // fired so we can still fire eventnum.
	    }                 
	  }
	}
        // Not enough rules have been fired to enable eventnum!
	if(!i_conflicts) {
	  return( false );
	}
      }
    }
  }
  return( true );
}


bool ver_enabled_event(int eventnum, int enabling, ruleADT **rules, int nevents,
		       rule_info_listADT *info, int ninputs, markingADT marking){

  int i,j;
  bool event_enabled=1;
  bool i_conflicts=0;
  rule_infoADT *marked_info=NULL;

  for(i=0;i<nevents;i++){
    i_conflicts=0;
    if((rules[i][eventnum]->ruletype) &&
       (rules[i][enabling]->conflict!=1) && (rules[i][eventnum]->reachable)){
      marked_info=(rule_infoADT *)get_rule_info(info, i, eventnum);
      if(!((marked_info->exp_enabled) || 
	   (enabling>=ninputs && 
	    marking->marking[rules[i][eventnum]->marking]!='F'))){
	for(j=0;j<nevents;j++){
	  if(rules[i][j]->conflict && rules[j][eventnum]->ruletype){
	    marked_info=(rule_infoADT *)get_rule_info(info, j, eventnum);
	    if(marked_info->exp_enabled || 
	       (enabling>=ninputs && 
		marking->marking[rules[j][eventnum]->marking]!='F')){
	      i_conflicts=1;
	    }
	  }
	}
	if(!i_conflicts){
	  event_enabled=0;
	  i_conflicts=0;
	}
      }
    }
  }
  return event_enabled;
}



void bv_clear_event(int eventnum, ruleADT **rules, exp_enabledADT exp_enabled_bv,
		    confl_removedADT confl_removed_bv, int nevents,
		    clocknumADT clocknums_iv, int nrules){
  
  int i;
  
  for(i=0;i<nrules;i++){
    /* check to see if this firing was the enabled event for any  */
    /* rule firing which caused a rule to be marked fired         */
    /* due to an enabling conflict.  This is stored by multiplying*/
    /* the event number by 1000.                                  */

    if(clocknums_iv[i]/1000==(-eventnum) && (clocknums_iv[i]/1000 != 0)){
      exp_enabled_bv[i]='F';
      clocknums_iv[i]=(-1);
    }
  }
  for(i=0;i<nevents;i++){
    if(rules[i][eventnum]->ruletype){
      exp_enabled_bv[rules[i][eventnum]->marking]='F';
      if(clocknums_iv[rules[i][eventnum]->marking]<0){
	clocknums_iv[rules[i][eventnum]->marking]=(-1);
      }
    }
  }
  for(i=0;i<nevents;i++){
    if(rules[eventnum][i]->ruletype){
      confl_removed_bv[rules[eventnum][i]->marking]='F';
    }
  }
}


/* Get a unit of work off the workstack */

workADT pop_work(workstackADT *workstack){

  workADT work;
  workstackADT old_workstack;


  if(*workstack == NULL){
    return NULL;
  }
  work=(*workstack)->work;
  old_workstack=(*workstack);
  (*workstack)=(*workstack)->next;
  free(old_workstack);
  return work;
}

approx_workADT approx_pop_work(approx_workstackADT *workstack){

  approx_workADT work;
  approx_workstackADT old_workstack;

  if(*workstack == NULL){
    return NULL;
  }
  work=(*workstack)->work;
  old_workstack=(*workstack);
  (*workstack)=(*workstack)->next;
  free(old_workstack);
  return work;
}

geom_workADT geom_pop_work(geom_workstackADT *workstack){

  geom_workADT work;
  geom_workstackADT old_workstack;

  if(*workstack == NULL){
    return NULL;
  }
  work=(*workstack)->work;
  old_workstack=(*workstack);
  (*workstack)=(*workstack)->next;
  free(old_workstack);
  return work;
}


verworkADT ver_pop_work(verstackADT *workstack){

  verworkADT work;
  verstackADT old_workstack;
   if(*workstack == NULL){
    return NULL;
  }
  work=(*workstack)->work;
  old_workstack=(*workstack);
  (*workstack)=(*workstack)->next;
  return work;
} 

void mark_no_epsilon(ruleADT **rules, processkeyADT processkey, 
		     int num_events, int nevents,
		     rule_info_listADT *info){

  int i,j;
  rule_infoADT *current;

  for(i=1;i<=num_events; i++){
    for(j=1;j<nevents;j++){
      if((rules[processkey[i].eventnum][j]->ruletype) &&
	 (rules[processkey[i].eventnum][j]->epsilon==0)){
	current=(rule_infoADT *)get_rule_info(info, processkey[i].eventnum, j);
	current->fired=1;
      }
    }
  }
}

// For all non-initially marked rules that have enabling events in the 
// process_key, mark them as fired in the fired_bv vector.
void mark_no_epsilon_bv(ruleADT **rules, processkeyADT processkey, 
			int num_events, int nevents, firedADT fired_bv){
  int i,j;
  for(i=1;i<=num_events; i++){
    for(j=1;j<nevents;j++){
      if((rules[processkey[i].eventnum][j]->ruletype) &&
	 (rules[processkey[i].eventnum][j]->epsilon==0) &&
	 (!(rules[processkey[i].eventnum][j]->ruletype & ORDERING))){
	fired_bv[rules[processkey[i].eventnum][j]->marking]='T';
      }
    }
  }
}  
    
// NOTE: This function looks at the clock key and adds enabling events
// from enabled rules in the clockkey to the processkey.  It also looks 
// at the marking and adds the enabling of a rule to the processkey if 
// the rule is marked but not enabled.  All initial seperations in the
// process matrix are 0.
int mark_process(ruleADT **rules, 
		 processADT process, processkeyADT processkey, 
		 clocksADT clocks,
		 clockkeyADT clockkey, int num_clocks,
		 markingADT marking, markkeyADT *markkey, 
		 int nrules){
  
  int i, j;
  int found=0;
  int num_events=0;

  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      if(processkey[j].eventnum==clockkey[i].enabling){
	clockkey[i].eventk_num=j;  // What is eventk_num used for?
	found=1;                   // Looks like a mapping between clockkey
      }                            // and process key.
    }
    if(!found){
      num_events++;
      processkey[num_events].eventnum=clockkey[i].enabling;
      processkey[num_events].root=num_clocks;
      processkey[num_events].instance=(-1);
      processkey[num_events].anti=NULL;
      clockkey[i].eventk_num=num_events;
    }
    found=0;
  }

  /* Add enabling events of rules not in the clockkey since */
  /* their level expressions are false.                     */

  for(i=0;i<nrules;i++){
    if(marking->marking[i]!='F' &&
       !((rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype & ORDERING) &&
	 (rules[markkey[i]->enabling][markkey[i]->enabled]->lower == 0) &&
	 (rules[markkey[i]->enabling][markkey[i]->enabled]->upper == INFIN))){
      found=0;      
      for(j=1;j<=num_events;j++){
	if(processkey[j].eventnum==markkey[i]->enabling){
	  found=1;
	}
      }
      if(found==0){
	num_events++;
	processkey[num_events].eventnum=markkey[i]->enabling;
	processkey[num_events].root=num_clocks;
	processkey[num_events].instance=(-1);
	processkey[num_events].anti=NULL;
      }
    }
  }      
  for(i=1;i<=num_events;i++){
    for(j=1;j<=num_events;j++){
      process[i][j]=0;
    }
  }
  return num_events;
}


/* Mark the given event as fired in the rules matrix */

void mark_firing(int fired, ruleADT **rules, int nevents, rule_info_listADT *info){

  int i;
  rule_infoADT *current;

  for(i=0;i<nevents;i++){
    if(rules[i][fired]->ruletype){
      current=(rule_infoADT *)get_rule_info(info, i, fired);
      current->fired=1;
    }
  }
}

// Marks all rules enabling fired in fired_bv as true
void bv_mark_firing(int fired, ruleADT **rules, int nevents, firedADT fired_bv){

  int i;

  for(i=0;i<nevents;i++){
    if(rules[i][fired]->ruletype && 
       (!(rules[i][fired]->ruletype & ORDERING))){
      fired_bv[rules[i][fired]->marking]='T';
    }
  }
}

bool actual_fired(int fired, ruleADT **rules, int nevents, 
		  rule_info_listADT *info){
  
  int i;
  rule_infoADT *current;

  for(i=0;i<nevents; i++){
    if(rules[i][fired]->ruletype){
      current=(rule_infoADT *)get_rule_info(info, i, fired);    
      if(current->fired){
	return 1;
      }
    }
  }
  return 0;
}

// Returns rules true there is a fired rule enabling fired in fired_bv.
bool bv_actual_fired(int fired, ruleADT **rules, int nevents,
		     firedADT fired_bv){
  int i;
  for(i=0;i<nevents; i++){
    if(rules[i][fired]->ruletype && fired_bv[rules[i][fired]->marking]=='T'){ 
      return 1;
    }
  }
  return 0;
}

int event_exists(int num_clocks, processkeyADT processkey,
		 clockkeyADT clockkey,
		 int fire_enabling, int fire_enabled){

  int l; int exists=0;
  int i=0;
  for(l=0;l<=num_clocks;l++){
    if((clockkey[l].enabling==fire_enabling) && 
       (clockkey[l].enabled!=fire_enabled)){
      while(1){
	if(processkey[i].eventnum==fire_enabling )
	{
	  exists=i;
	  return exists;
	}
	else
	  i++;
      } 
    }
  }
  return exists;
}
 
int enabling_done(ruleADT **rules, int num_clocks, processkeyADT processkey, 
		      clockkeyADT clockkey,
		      int fire_enabling, int fire_enabled, int nevents,
		      rule_info_listADT *info){

  int i, j;
  int event_done=1;
  int i_conflicts;
  rule_infoADT *current_f_i=NULL;
  rule_infoADT *current_f_j=NULL;

  for(i=0;i<nevents;i++){
    i_conflicts=0;
    if((rules[fire_enabling][i]->ruletype) && 
       (rules[i][fire_enabled]->conflict!=1)){
      current_f_i=(rule_infoADT *)get_rule_info(info, fire_enabling, i);
      if(!current_f_i->fired){
	for(j=0;j<nevents;j++){
	  if((rules[i][j]->conflict && rules[fire_enabling][j]->ruletype)){
	    current_f_j=(rule_infoADT *)get_rule_info(info, fire_enabling, j);
	    if(current_f_j->fired){
	      i_conflicts=1;
	    }
	  }
	}
	if(!i_conflicts){
	  event_done=0;
	  i_conflicts=0;
	}
      }
    }
  }
  return event_done;
}

bool in_clockkey(clockkeyADT clockkey, int num_clocks,
		int fire_enabling, int fire_enabled){

  int i;

  for(i=1;i<=num_clocks;i++){
    if(clockkey[i].enabling == fire_enabling &&
       (clockkey[i].enabled == fire_enabled)){
      return true;
    }
  }
  return false;
}

// Returns the instance number used in the processkey to keep track of
// event instances.
//
//    0: This event enables an unresolved conflict between 2 rules <i,j>
//       and <fire_enabling,i> where i conflicts with j, but i does not
//       conflict with fire_enabled and <i,j> nor <fire_enabling,i> is 
//       marked.
//       OR
//       There is a constraint rule in the clock key that uses with
//       fire_enabling as its enabling event and fire_enabling is not
//       duplicated in the processkey.
//    1: Not 0 or 2!
//    2: fire_enabling is the causal event for some clock i in the clocks
//       matrix that is not a rule that is a self_loop and fire_enabling
//       is duplicated in the processkey.
int bv_enabling_done(ruleADT **rules, int fire_enabling, int fire_enabled,
		     int nevents, firedADT fired_bv, int num_clocks, 
		     clockkeyADT clockkey, int max_causal, bool duplicated){

  int i, j;
  int event_done=1;
  int i_conflicts;


  for(i=0;i<nevents;i++){
    i_conflicts=0;
    // Is there a rule <fire_enabling,i> where i does not conflict with
    // fire enabled that is not a constraint rule?
    if((fire_enabling != fire_enabled) && 
       (rules[fire_enabling][i]->ruletype) && 
       (rules[i][fire_enabled]->conflict!=1) &&
       (!rules[fire_enabling][i]->ruletype & ORDERING)){
      // If this rule is not marked, then is there another rule
      // <i,j> that is marked where j conflicts with i and is not
      // a constraint rule.  If so, then set i_conflicts to 1.
      if(!(fired_bv[rules[fire_enabling][i]->marking]=='T'))
	for(j=0;j<nevents;j++){
	  if((rules[i][j]->conflict && rules[fire_enabling][j]->ruletype) &&
	     (!(rules[fire_enabling][j]->ruletype & ORDERING))){
	    if(fired_bv[rules[fire_enabling][j]->marking] == 'T'){
	      i_conflicts=1;
	    }
	  }
	}
      if(!i_conflicts){
	// egm 08/18/00 -- added early exit here, since once event_done
        // is set to 0, it cannot be set to 1 and nothing is every changed.
        //
	// OLD STUFF:
	//event_done=0;
	//i_conflicts=0;
	return 0;
      }
    }
    // If this is a constraint rule in the clockkey that is not
    // duplicate, then event is not done!
    if((event_done==1) &&
       (rules[fire_enabling][i]->ruletype & ORDERING) &&
       in_clockkey(clockkey, num_clocks, fire_enabling,i) &&
       !duplicated){
      // egm 08/18/00 -- added early exit
      //
      // OLD STUFF:
      //event_done=0;
      return 0;
    }
  }
  if(event_done){
    // If the causal event for clock i is fire_enabling and this is
    // not the rule we just fired and this rule is duplicated, then
    // event_done is 2.
    for(i=1;i<=max_causal;i++){
      if ((clockkey[i].causal==fire_enabling) && 
	  !((clockkey[i].enabling==fire_enabling) && 
	    (clockkey[i].enabled==fire_enabled) && duplicated)){
	// egm 08/18/00 -- added early exit
	//
	// OLD STUFF:
	//event_done=2;
	return 2;
      }
    }
  }    
  return event_done;
}

//  int  DIS_bv_enabling_done(ruleADT **rules, int fire_enabling, int fire_enabled,
//  			  int nevents, firedADT fired_bv, int num_clocks, 
//  			  clockkeyADT clockkey, int max_causal){

//    int i;
//    int event_done=1;
//    int this_event_done=0;

//    for(i=1;i<nevents;i++){
//      this_event_done=bv_enabling_done(rules, fire_enabling, i, nevents,
//  				     fired_bv, num_clocks, clockkey, max_causal);
//      if(this_event_done==0) return 0;
//      if(this_event_done==2) event_done=2;
//    }
//    return event_done;
//  }
    
 

void print_depend(depend_listADT list, int num_events){
  int i;
  
  puts("Row dependencies");
  for(i=1;i<=(num_events);i++){
    printf("clock1=%d clock2=%d, separation=%d conflict=%d\n", 
	   list[i].clock1, list[i].clock2, list[i].separation, list[i].conflict);
  }
  puts("Column dependencies");
  for(i=(num_events+1);i<=(num_events)*2;i++){
    printf("clock1=%d clock2=%d, separation=%d conflict=%d\n", 
	   list[i].clock1, list[i].clock2, list[i].separation, list[i].conflict);
  }   
}


void print_constraints(constraintADT *constraints, int num_sets){
  
  int i;
  constraintADT current;
  for(i=1;i<=num_sets;i++){
    printf("%d: \n",i);
    current=constraints[i];
    while(current!=NULL){
      printf("event1=%d event2=%d separation=%d \n", current->event1, 
	     current->event2, current->separation);
      current=current->next;
    }
  }
}

void print_covers(regionADT *regions, int nsigs){
  
  regionADT current_region;
  int i;

  for(i=0;i<=(nsigs*2);i++){
    current_region=regions[i];
    printf("i=%d\n", i);
    while (current_region!=NULL){
      printf("enstate = %s\n", current_region->enstate);
      printf("switching = %s\n", current_region->switching);
      printf("enablings = %s\n", current_region->enablings);
      printf("tsignals = %s\n", current_region->tsignals);
      printf("cover = %s\n", current_region->cover);
      printf("primes = %s\n", *(current_region->primes));
      printf("implied = %d\n", *(current_region->implied));
      printf("nvprimes = %d\n", current_region->nvprimes);
      printf("\n");
      current_region=current_region->link;
    }
  }
}

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void restrict_region(ruleADT **rules, clocksADT clocks, clockkeyADT clockkey, 
		     int fire_enabling,  int fire_enabled, int num_clocks,
		     int spec_verify, int lower_bound, clocknumADT clocknums_iv){
  
  int i;
  int current;

  if(!spec_verify){
    current=clocknums_iv[rules[fire_enabling][fire_enabled]->marking];
    clocks[current][0]= -(rules[fire_enabling][fire_enabled]->lower);
    recanon_restrict(clocks, num_clocks, current);
  }
  else{
    for(i=1;i<=num_clocks;i++){
      if(clockkey[i].enabled==fire_enabled){
	current=clocknums_iv[rules[fire_enabling][fire_enabled]->marking];	
	clocks[current][0]= (-lower_bound);  
	recanon_restrict(clocks, num_clocks, current);
      }
    }
  }
}  


/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void recanon_restrict(clocksADT clocks, int num_clocks, int restricted){

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


void recanon_process(processADT process, int num_clocks){

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

void restrict_to_causal_rule(clocksADT clocks, clockkeyADT clockkey,
			     int num_clocks, int fire_enabling, int fire_enabled,
			     int causal_clocknum, int lower, int upper){

  int i;
  for(i=1;i<=num_clocks;i++){
    if(clockkey[i].enabling==fire_enabled){
      clocks[causal_clocknum][i]=-(lower);
      clocks[i][causal_clocknum]=upper;
    }
  }
  recanon_process(clocks, num_clocks);
}

int euclid_gcd(int a, int b){
  

  if((b==INFIN) || (b==0)){
    return a;
  }
  if((a==INFIN) || (a==0)){
    return b;
  }
  while(a!=b){
    if(a>b){
      a=a-b;
    }
    else{
      b=b-a;
    }
  }
  return a;
}

int find_gcd(int *bounds, int nbounds){

  int i;
  
  if(nbounds==1){
    if(bounds[0]) return bounds[0];
    else return 1;
  }
  else{
    bounds[0]=euclid_gcd(bounds[0], bounds[1]);
    for(i=1;i<nbounds;i++){
      bounds[i]=bounds[i+1];
    }
    find_gcd(bounds, nbounds-1);
  }
  return 1;
}

void gcd_normalize(ruleADT **rules, int nevents, int nrules){

  int i, j, gcd;
  int *bounds;
  int k = 0;
  

  bounds = (int *) GetBlock((nrules+1)*2*(sizeof(int)));

  for(i=1; i<nevents;i++){
    for(j=1; j<nevents;j++){
      if(rules[i][j]->ruletype >= 3){
	bounds[k]=rules[i][j]->lower;
	bounds[k+1]=rules[i][j]->upper;
	k=k+2;
      }
    }
  }
  gcd=find_gcd(bounds, k);
  printf("gcd is %d\n", gcd);
  for(i=1; i<nevents;i++){
    for(j=1; j<nevents;j++){
      rules[i][j]->lower=(rules[i][j]->lower)/gcd;
      if(rules[i][j]->upper!=INFIN){
	rules[i][j]->upper=(rules[i][j]->upper)/gcd;
      }
    }
  }
  free(bounds);
}

int count_discrete_states(stateADT *state_table, ruleADT **rules, 
			  int nevents){
  int i, j;
  int max_bound=0;
  int num_states=0;
  clocklistADT region;
  int *vector;
  stateADT e;

  printf("Counting discrete states:");
  for(i=1;i<nevents;i++){
    for(j=1;j<nevents;j++){    
      if((rules[i][j]->upper!=INFIN) && (rules[i][j]->upper > max_bound)){
	max_bound=rules[i][j]->upper;
      }
    }
  }
  for(i=0;i<PRIME;i++){
    if(state_table[i]->state!=NULL){
       printf("In position %d\n", i);
      e=state_table[i];
      while(e->state!=NULL){
	printf("%s %s %s \n", e->state, e->marking, e->enablings);
	printf("This state has regions:\n");
	region=e->clocklist;
	while(region!=NULL){
	  vector=(int *)GetBlock((region->clocknum) * sizeof(int));
	  for(j=0;j<region->clocknum;j++) vector[j]=0;
	  num_states=num_states+count_states(region->clocks, region->clocknum,
					     vector,0, max_bound);
	  
	  region=region->next;
	}
	e=e->link;
      }
    }
  }
  printf("%d discrete states found\n", num_states);
  return num_states;
}

bool check_point(int *vector, clocksADT clocks, int vectorsize){

  int i;

  for(i=1;i<=vectorsize;i++){
    if((vector[i-1] < -clocks[i][0]) || (vector[i-1] > clocks[0][i] )){
      return FALSE;
    }
  }
  return TRUE;
}

int count_states(clocksADT clocks, int vectorsize, int *vector, int index,
		 int max){

  int i,j;
  int num_states=0;
  int *new_vector;

  if(((state_count % 100) == 0) && state_count!=old_state_count){
       printf("%d states found\n", state_count);
  }
  if(index==(vectorsize-1)){
    for(i=0;i<=max;i++){
      vector[index]=i;
      if(check_point(vector, clocks, vectorsize)){
	num_states++;
	old_state_count=state_count;
	state_count++;	
      }
    }
    free(vector);
    return num_states;
  }
  else{
    for(i=0;i<=max;i++){
      new_vector=(int *) GetBlock(vectorsize * (sizeof(int)));
      for(j=0;j<vectorsize;j++) new_vector[j]=vector[j];
      new_vector[index]=i;
      num_states=num_states+count_states(clocks, vectorsize, new_vector,
					 index+1, max);
    }
  }
  return num_states;
}
  
/* Checks to see if the rule indicated by enabling and enabled has */
/* its level satisfied.                                            */

bool level_satisfied(ruleADT **rules,
		    markingADT marking, int nsigs, 
		    int enabling, int enabled){

  char * state;
  level_exp current_level;
  int failed=0;
  int i;

  if ((marking->marking) &&
      (marking->marking[rules[enabling][enabled]->marking] == 'S')) return 1;

  state = marking->state;
  current_level = rules[enabling][enabled]->level;
  if(current_level == NULL){
    return 0;
  }
  while(current_level != NULL){
    failed = 0;
    for(i=0;i<nsigs;i++){
      if((current_level->product[i]=='1' && (state[i]=='0' || state[i]=='R'))||
	 (current_level->product[i]=='0' && (state[i]=='1' || state[i]=='F'))){
	failed = 1;
	break;
      }
    }
    if(failed){
      current_level=current_level->next;
//      if(current_level==NULL)
//	return 0;
    }
    if(!failed){
      return 1;
    }
  }
  return 0;
}

// An event is causal if it is the causal event for a clock in the
// clockkey with valid enabling and enabled values and it is not
// an ordering rule.
bool not_causal(clockkeyADT clockkey, int num_clocks, int event,
		ruleADT **rules,int nevents){
  int i;
  for(i=1;i<=num_clocks;i++){
    if (clockkey[i].causal==event && (clockkey[i].enabling >= 0) && 
	(clockkey[i].enabled >= 0) && (clockkey[i].enabling < nevents) && 
	(clockkey[i].enabled < nevents) && 
       !(rules[clockkey[i].enabling][clockkey[i].enabled]->ruletype & ORDERING)){
      return 0;
    }
  }
  return 1;
}

void init_time_sep(ruleADT **rules, int nevents){

  int i;
  int j;
  for(i=0;i<nevents;i++){
    for(j=0;j<nevents;j++){
      rules[i][j]->maximum_sep=-(INFIN);
    }
  }
}

void update_time_sep(ruleADT **rules, int num_clocks, clockkeyADT clockkey,
		     clocksADT clocks){

  int i;
  int j;
  int current_seperation;
  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      current_seperation=clocks[i][j];
      if(current_seperation > rules[clockkey[i].enabling][clockkey[j].enabling]->maximum_sep){
	rules[clockkey[i].enabling][clockkey[j].enabling]->maximum_sep=current_seperation;
      }
    }
  }
}

void limit_region(clocksADT clocks, clockkeyADT clockkey, int num_clocks,
		  processADT process, processkeyADT processkey, 
		  int num_events, ruleADT **rules){

  int i;
  int j;
  // int current_event;
  // int max_age;
  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      if(clocks[j][i]>clocks[0][i] && 
	 (rules[clockkey[i].enabling][clockkey[i].enabled]->upper != INFIN)){
	clocks[j][i]=clocks[0][i];
      }
    }
  }
  /*  for(i=1;i<=num_events;i++){
    current_event=processkey[i].eventnum;
    max_age=0;


    for(j=1;j<=num_clocks;j++){
      if((clockkey[j].enabling==current_event) && 
	 rules[clockkey[j].enabling][clockkey[j].enabled]->upper > max_age){
	max_age=rules[clockkey[j].enabling][clockkey[j].enabled]->upper;
      }
    }
    for(j=1;j<=num_events;j++){
      if(process[j][i]>max_age) process[j][i]=max_age;
    }
  }*/
}




