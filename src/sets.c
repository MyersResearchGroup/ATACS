#include "sets.h"
#include "findrsg.h"
#include "cmath"

#define DEBUG
#define MIN_CLOCK_SIZE 5
#define CLOCK_MULT 2
#define PROCESS_MULT 2
#define CONNUM 40

int EMPTY=-(10000);


 
constraintADT *sets_create_constraints(int nevents){

  int i;
  constraintADT *constraints;
  
  constraints = (constraintADT *)GetBlock(CONNUM * (sizeof(struct constraint *)));
  for(i=0;i<CONNUM;i++){
    constraints[i]=NULL;
  }
  return constraints;
}
 
constraintADT *sets_copy_constraints(int nevents, constraintADT *old_constraints){

  int i;
  constraintADT *new_constraints;

  new_constraints=sets_create_constraints(nevents);
  for(i=0;i<CONNUM;i++){
    if(old_constraints[i]!=NULL){
      new_constraints[i]=sets_add_constraints(new_constraints[i], old_constraints[i]);
    }
  }
  return new_constraints;
}


processkeyADT sets_create_processkey(int nrules){

  processkeyADT new_processkey;
  int i, num_clocks;

  num_clocks=nrules;
  new_processkey = (processkeyADT)GetBlock (nrules * sizeof (struct processkey));
  for(i=0;i<num_clocks;i++){
    new_processkey[i].eventnum=(-1);
    new_processkey[i].root=(-1);
    new_processkey[i].instance=(-1);
    new_processkey[i].sets=(-1);
    new_processkey[i].start=(-1);
    new_processkey[i].index=(-1);
    new_processkey[i].max_index_row=NULL;
    new_processkey[i].max_index_column=NULL;
  }
  return new_processkey;
}
 
  
 
processkeyADT sets_copy_processkey(int nrules, processkeyADT old_processkey){
  
  processkeyADT new_processkey;
  int i, num_clocks;

  num_clocks=nrules;
  new_processkey = (processkeyADT)GetBlock (num_clocks * sizeof (struct processkey));
  for(i=0;i<num_clocks;i++){
    new_processkey[i].eventnum=old_processkey[i].eventnum;
    new_processkey[i].root=old_processkey[i].root;    
    new_processkey[i].instance=old_processkey[i].instance;
    new_processkey[i].sets=old_processkey[i].sets;
    new_processkey[i].start=old_processkey[i].start;    
    new_processkey[i].index=old_processkey[i].index;
    new_processkey[i].max_index_row=old_processkey[i].max_index_row;
    new_processkey[i].max_index_column=old_processkey[i].max_index_column;
  }
  return new_processkey;
}

processkeyADT sets_resize_processkey(int old_size, int new_size, processkeyADT old_processkey){
  
  processkeyADT new_processkey;
  int i;


  new_processkey = (processkeyADT)GetBlock (new_size * sizeof (struct processkey));
  for(i=0;i<new_size;i++){
    if(i<old_size){
      new_processkey[i].eventnum=old_processkey[i].eventnum;
      new_processkey[i].root=old_processkey[i].root;
      new_processkey[i].instance=old_processkey[i].instance;
      new_processkey[i].sets=old_processkey[i].sets;
      new_processkey[i].start=old_processkey[i].start;    
      new_processkey[i].index=old_processkey[i].index;
      new_processkey[i].max_index_row=old_processkey[i].max_index_row;
      new_processkey[i].max_index_column=old_processkey[i].max_index_column;
    }
    else{
      new_processkey[i].eventnum=(-1);
      new_processkey[i].root=(-1);
      new_processkey[i].instance=(-1);
      new_processkey[i].sets=(-1);
      new_processkey[i].start=(-1);    
      new_processkey[i].index=(-1);
      new_processkey[i].max_index_row=NULL;
      new_processkey[i].max_index_column=NULL;
    }   
  }
  return new_processkey;
}  

 
constraintADT sets_add_constraints(constraintADT new_const, constraintADT add){
  
  constraintADT new_current;
  constraintADT add_current;
  constraintADT prev;
  
  add_current=add;
  if(new_const==NULL){
    new_const=(constraintADT)GetBlock(sizeof(struct constraint));
    new_current=new_const;
  }
  else{
    prev=new_const;
    new_current=new_const;
    while(new_current!=NULL){
      prev=new_current;
      new_current=new_current->next;
    }
    new_current=prev;
    prev->next=(constraintADT)GetBlock(sizeof(struct constraint));
    new_current=prev->next;
  }
  while(add_current!=NULL){
    new_current->event1=add_current->event1;
    new_current->event2=add_current->event2;
    new_current->separation=add_current->separation;
    if(add_current->next!=NULL){
      new_current->next=(constraintADT)GetBlock(sizeof(struct constraint));
    }
    else{
      new_current->next=NULL;
    }
    new_current=new_current->next;
    add_current=add_current->next;
  }
  return new_const;
}


/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int sets_mark_initial_region(ruleADT **rules,  
			markkeyADT *markkey, markingADT marking, 
			clocksADT clocks, clockkeyADT clockkey,
			int nevents, int nrules, rule_info_listADT *info){

   int i=0,j=0; 
   int min_in_set=INFIN;
   int clock_index=1;
   rule_infoADT *rule_inf=NULL;


   /* Look at everything that could cause each event and see if it's */
   /* marked find the maximum time before and event must happen      */
   /* The event referenced in the first row causes the event in the  */
   /* second row.                                                    */

   for(i=0;i<nevents;i++){
     for(j=0;j<nevents;j++){
       if((rules[j][i]->epsilon==1) && (rules[j][i]->ruletype)){
	 rule_inf=(rule_infoADT *)get_rule_info(info, j, i);
	 rule_inf->clocknum=clock_index;
	 clockkey[clock_index].enabled=i;
	 clockkey[clock_index].enabling=j;
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
 
int  sets_mark_process(ruleADT **rules, 
		       processADT process, processkeyADT processkey, 
		       clocksADT clocks,
		       clockkeyADT clockkey, int num_clocks){
  
  int i, j;
  int found=0;
  int num_events=0;

  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      if(processkey[j].eventnum==clockkey[i].enabling){
	clockkey[i].eventk_num=j;
	found=1;
      }
    }
    if(!found){
      num_events++;
      processkey[num_events].eventnum=clockkey[i].enabling;
      processkey[num_events].root=1;
      processkey[num_events].instance=1;
      processkey[num_events].start=num_events;
      processkey[num_events].sets=1;
      processkey[num_events].index=1;
      clockkey[i].eventk_num=num_events;
    }
    found=0;
  }
  for(i=1;i<=num_events;i++){
    for(j=1;j<=num_events;j++){
      process[i][j]=0;
    }
  }
  return num_events;
}

/* Restrict the timing firing region to only that region in */
/* which the chosen even could have fired.                  */


void sets_restrict_region(eventADT *events, ruleADT **rules, 
			  markkeyADT *markkey, markingADT marking,
			  clocksADT clocks, int nevents, int nrules,
			  rule_info_listADT *info, int fire_enabling, 
			  int fire_enabled, int num_clocks){
  
  int i;
  int max_min_firing=0;
  rule_infoADT *current=NULL;
    
    
  current=(rule_infoADT *)get_rule_info(info, fire_enabling, fire_enabled);  
  clocks[current->clocknum][0]=
    -(rules[fire_enabling][fire_enabled]->lower);
  sets_recanon_restrict(clocks, num_clocks, current->clocknum);
}  


/* Recanonicalize the matrix given that clock number "restricted"  */
/* has been restricted.                                            */
      
void sets_recanon_restrict(clocksADT clocks, int num_clocks, int restricted){

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


/* Eliminate clocks that have been used in this firing. */

int sets_project_clocks(eventADT *events, ruleADT **rules, 
			markkeyADT *markkey, markingADT marking,
			clocksADT clocks, clockkeyADT clockkey, 
			processADT process, processkeyADT processkey,
			int nevents, int nrules, rule_info_listADT *info,
			int fire_enabling, int fire_enabled, int num_clocks,
			int *num_events, int event_fired, int *num_sets,
			constraintADT *constraints){

  int i,j,k,l,p,s;
  int project_event=0;
  int removed_event_num;
  int removed_set_num;
  int removed_sets;
  rule_infoADT *current_info_this_rule;
  rule_infoADT *current_info;
  rule_infoADT *rule_e_j;
  int rule_clocknum;
  
  i=j=k=0;
  l=p=1;

#ifdef DEBUG
/*   printf("num_clocks=%d\n",num_clocks); */
/*   printf("compressing for rule {%d, %d}\n", fire_enabling, fire_enabled); */
/*   printf("rule {%d, %d} -> clock_num=%d\n", fire_enabling, fire_enabled,  */
/* 	     rules[fire_enabling][fire_enabled]->clocknum); */
#endif 
  /* Look to see if this is the last rule  with this enabling */


  current_info_this_rule=(rule_infoADT *)get_rule_info(info, fire_enabling, fire_enabled);
  rule_clocknum=current_info_this_rule->clocknum;
  if(event_fired){
    while(p<=(*num_events)){
      project_event=0;
      if(rules[processkey[p].eventnum][fire_enabled]->ruletype){
	project_event=enabling_done(rules, num_clocks, processkey, clockkey,
				    processkey[p].eventnum, fire_enabled, nevents,
				    info); 
      }  
      if(project_event){
	for(j=0;j<nevents;j++){
	  if(rules[processkey[p].eventnum][j]->ruletype){
	    rule_e_j=(rule_infoADT *)get_rule_info(info, processkey[p].eventnum, j);
	    rule_e_j->fired=0;
	  }
	}
	l=1;
	while(processkey[l].eventnum!=processkey[p].eventnum){
	  l++;
	}
	removed_event_num=l;
	removed_set_num=processkey[l].start;
	removed_sets=processkey[l].sets;
	for(j=1;j<=(*num_sets);j++){
	  s=removed_set_num;
	  while(s<=(*num_sets)){
	    process[j][s]=process[j][s+processkey[removed_event_num].sets];
	    s++;
	  }
	}
	for(j=1;j<=(*num_sets);j++){
	  s=removed_set_num;
	  while(s<=(*num_sets)){
	    process[s][j]=process[s+processkey[removed_event_num].sets][j];
	    if(j==1){
	      constraints[s]=constraints[s+processkey[removed_event_num].sets];
	    }
	    s++;
	  }
	}
	for(l=removed_event_num;l<=(*num_events);l++){
	  processkey[l].eventnum=processkey[l+1].eventnum;
	  processkey[l].root=processkey[l+1].root;
	  processkey[l].sets=processkey[l+1].sets;
	  processkey[l].start=processkey[l+1].start-removed_sets;
	  processkey[l].index=processkey[l+1].index;
	}
	for(j=0;j<=num_clocks;j++){
	  if(clockkey[j].eventk_num>=removed_event_num){
	    clockkey[j].eventk_num--;
	  }
	}
	(*num_events)--;
	(*num_sets)=(*num_sets)-removed_sets;    
      }
      else p++;
    }
  }    

  /* Compress the matrix horizontally */
  for(j=0;j<=num_clocks;j++){
    while(k<rule_clocknum){
	  k++;
	}
    while(k<=num_clocks){
      clocks[j][k]=clocks[j][k+1];
      /* Only decrement the clock index the first time through. */
      
      if((k<num_clocks) && (j==0)){
	current_info=
	  (rule_infoADT *)get_rule_info(info, clockkey[k+1].enabling, clockkey[k+1].enabled);
	current_info->clocknum--;
      }
      k++;
    }
    k=0;
  }
  /* Compress the matrix vertically */
  k=0;
  for(j=0;j<num_clocks;j++){
    while(k<rule_clocknum){
      k++;
    }
    while(k<=num_clocks){
      clocks[k][j]=clocks[k+1][j];
      /* Only compress the clockkey the first time through */
      if(j==0){
	clockkey[k].enabling=clockkey[k+1].enabling;
	clockkey[k].enabled=clockkey[k+1].enabled;
	clockkey[k].eventk_num=clockkey[k+1].eventk_num;
      }
      k++;
    }
    k=0;
  }
  current_info_this_rule->clocknum=-1;
  num_clocks--;
  return num_clocks;
}

int sets_increment_sets(processkeyADT processkey, int *num_events){
  int i,done;

  i=(*num_events)-1;
  done=0;
  while(i>0){
    if(processkey[i].index!=processkey[i].sets){
      processkey[i].index++;
      break;
    }
    i--;
  }
  if(i==0) done=1;
  i++;
  while(i<=(*num_events-1)){
    processkey[i].index=1;
    i++;
  }
  if(done) return 0;
  else return 1;
}
   
/* Make row computes a new row using the sets specified in processkey.  If it's a valid */
/* row (made from a consistant sets) make_row returns 1, otherwise it returns 0.        */  
int sets_make_row(ruleADT **rules, processkeyADT processkey,
		  processADT process, int new_event, 
		  int row_number, int *num_events, depend_listADT list, int *num_sets,
		  processADT final_process){


  int j, k, p, max_sep;
  int set_index_j;
  int set_index_k;
  int set_index_p;


  for(j=1;j<(*num_events);j++){
    set_index_j=processkey[j].start + processkey[j].index-1;
    /* Check to make sure the row is still valid */
    for(k=1;k<(*num_events);k++){
      set_index_k=processkey[k].start + processkey[k].index-1;
      if(process[set_index_k][set_index_j] == EMPTY){
	return 0;
      }
    }	
    if(rules[processkey[j].eventnum][new_event]->ruletype){ 
      max_sep= (rules[processkey[j].eventnum]
		[new_event]->upper); 
      list[j].clock1=j;
      list[j].clock2=j;
      list[j].separation=0;
      list[j].conflict=0;
      for(p=1;p<=(*num_events-1);p++){ 
	set_index_p=processkey[p].start+processkey[p].index-1;
	if(rules[processkey[p].eventnum][new_event]->ruletype){ 
	  if((process[set_index_p][set_index_j]==INFIN) || 
	     (rules[processkey[p].eventnum][new_event]->upper == INFIN)){ 
	    max_sep=INFIN; 
	    list[j].clock1=p;
	    list[j].clock2=j;
	    list[j].separation=process[set_index_p][set_index_j];
	    list[j].conflict=0;
	  } 
	  else{ 
	    if (process[set_index_p][set_index_j]+
		rules[processkey[p].eventnum][new_event]->upper > max_sep){
	      max_sep=process[set_index_p][set_index_j]+rules[processkey[p].eventnum] 
		[new_event]->upper; 
	      list[j].clock1=p;
	      list[j].clock2=j;
	      list[j].separation=process[processkey[p].start + processkey[p].index-1][j];
	      list[j].conflict=0;
	    } 
	  } 
	}
      } 
      final_process[row_number][set_index_j]=max_sep; 
    } 
    else{ 
      list[j].clock1=-1;
      list[j].clock2=-1;
      list[j].separation=0;
      list[j].conflict=0;
      final_process[row_number][set_index_j]=INFIN;     
    }
    /* Put in the empty marker for all sets not used in the computation of this row. */

    for(k=1;k<=processkey[j].sets;k++){
      if(k!=processkey[j].index){
	final_process[row_number][processkey[j].start+k-1]=EMPTY;
      }
    }
  }
  k=processkey[(*num_events)].start;
  while(k<=row_number){
    final_process[row_number][k]=0;
    k++;
  }
  return 1;
} 	

void sets_make_column(ruleADT **rules, processkeyADT processkey,
				processADT process, int new_event, 
				int col_number, int *num_events, depend_listADT list, int *num_sets,
				processADT final_process){
  int j,k, p,min_sep;
  int set_index_j;
  int set_index_p;

  for(j=1;j<(*num_events);j++){
    set_index_j=processkey[j].start+processkey[j].index-1;
    if(rules[processkey[j].eventnum][new_event]->ruletype){ 
      min_sep= -(rules[processkey[j].eventnum]
		 [new_event]->lower); 
      list[(*num_events)+j].clock1=j;
      list[(*num_events)+j].clock2=j;
      list[(*num_events)+j].separation=0;
      list[(*num_events)+j].conflict=0;
      for(p=1;p<(*num_events);p++){ 
	set_index_p=processkey[p].start+processkey[p].index-1;
	if(rules[processkey[p].eventnum][new_event]->ruletype){ 
	  if((process[set_index_j][set_index_p]!=INFIN) && 
	     (process[set_index_j][set_index_p]!=(-INFIN))){
	    if (process[set_index_j][set_index_p]
		-rules[processkey[p].eventnum][new_event]->lower < min_sep){
	      min_sep=process[set_index_j][set_index_p]
		-rules[processkey[p].eventnum][new_event]->lower; 
	      list[(*num_events)+j].clock1=p;
	      list[(*num_events)+j].clock2=j;
	      list[(*num_events)+j].separation=process[j][p];
	      list[(*num_events)+j].conflict=0;
	    } 
	  }
	  else if(process[set_index_j][set_index_p]==(-INFIN)){
	    min_sep=-(INFIN);
	  }
	} 
      }
      final_process[processkey[j].start+processkey[j].index-1][col_number]=min_sep;
    }
    else{ 
      list[(*num_events)+j].clock1=-1;
      list[(*num_events)+j].clock2=-1;
      list[(*num_events)+j].separation=0;
      list[(*num_events)+j].conflict=0;
      final_process[processkey[j].start+processkey[j].index-1][col_number]=INFIN;     
    }
     for(k=1;k<=processkey[j].sets;k++){
      if(k!=processkey[j].index){
	final_process[processkey[j].start+k-1][col_number]=EMPTY;
      }
    }   
  }
  k=processkey[(*num_events)].start;
  while(k<=col_number){
    final_process[k][col_number]=0;
    k++;
  }
} 	




/* Check for inconsistant maximums and split out consistant sets, returns the number */
/* of consistant sets derived from this row.                                         */

int sets_make_consistant(ruleADT **rules, processADT process, processkeyADT processkey,
			 int *num_events, int new_event, int row_number, depend_listADT conflicts,
			 int *num_sets, processADT final_process, constraintADT *constraints,
			 constraintADT con_list, int num_clocks, clockkeyADT clockkey, 
			 int nevents, rule_info_listADT *info){

  int k, i, j, rc, sets_x, sets_y;
  processADT process_x, process_y;
  depend_listADT conflicts_x, conflicts_y;
  int valid_set;
  constraintADT new_constraint;
  constraintADT x_con_list=NULL;
  constraintADT y_con_list=NULL;
  constraintADT current;
  int resolve_x=1;
  int resolve_y=1;
  int project_conflict_k=0;
  int project_conflict_i=0;

  for(rc=0;rc<=1;rc++){
    for(k=(rc*(*num_events)+1);k<=((rc*(*num_events)+(*num_events)));k++){
      if((conflicts[k].clock1!=conflicts[k].clock2) && conflicts[k].clock1!=(-1)){
	for(i=(rc*(*num_events)+1);i<=((rc*(*num_events)+(*num_events)));i++){
	  if((conflicts[i].clock1!=conflicts[i].clock2) && conflicts[i].clock1!=(-1) && (k!=i) &&
	     (conflicts[i].separation)!=(-conflicts[k].separation)){
	    if((conflicts[i].clock1==conflicts[k].clock2) && 
	       (conflicts[i].clock2==conflicts[k].clock1)){
	      conflicts[i].conflict=k;
	      conflicts[k].conflict=i;
	      current=constraints[row_number];
	      while(current!=NULL){
		if(((current->event1==processkey[i].eventnum) && 
		    (current->event2==processkey[k].eventnum)) ||
		   ((current->event1==processkey[k].eventnum) &&
		    (current->event2==processkey[i].eventnum))){
		  if(current->separation==
		     process[processkey[k].start+processkey[k].index-1]
		     [processkey[i].start+processkey[i].index-1]){
		    resolve_x=0;
		  }
		  else{
		    resolve_y=0;
		  }
		  break;
		}
		current=current->next;
	      }
	      if(resolve_x){
		process_x=copy_process((*num_sets)+3, process);
		process_x[processkey[i].start+processkey[i].index-1]
		  [processkey[k].start+processkey[k].index-1]=
		  -(process_x[processkey[k].start+processkey[k].index-1]
		    [processkey[i].start+processkey[i].index-1]);	      	      
		conflicts_x=(depend_listADT)sets_make_set(rules, processkey, process_x, 
							  constraints, new_event, row_number, 
							  num_events, num_sets, &valid_set, 
							  final_process); 
		if(resolve_y){
		  new_constraint=(constraintADT)GetBlock(sizeof(struct constraint));
		  new_constraint->next=NULL;
		  new_constraint->event1=processkey[i].eventnum;
		  new_constraint->event2=processkey[k].eventnum;
		  new_constraint->separation=-process[processkey[i].start+
						     processkey[i].index-1]
		    [processkey[k].start+processkey[k].index-1];
		  x_con_list=sets_add_constraints(con_list, new_constraint);
		  constraints[row_number]=sets_add_constraints(constraints[row_number], x_con_list);
		}
		sets_x=sets_make_consistant(rules, process_x, processkey, num_events, new_event,
					    row_number, conflicts_x, num_sets, process, 
					    constraints,x_con_list, num_clocks, clockkey, 
					    nevents, info);
	      }
	      else{
		sets_x=0;
	      }
	      if(resolve_y){
		process_y=copy_process((*num_sets)+3, process);	    
		  process_y[processkey[k].start+processkey[k].index-1]
		    [processkey[i].start+processkey[i].index-1]=
		    -(process_y[processkey[i].start+processkey[i].index-1]
		      [processkey[k].start+processkey[k].index-1]);
		  conflicts_y=(depend_listADT)sets_make_set(rules, processkey, process_y, 
							    constraints, new_event,
							    row_number+sets_x, num_events, num_sets,
							    &valid_set, final_process);
		  if(resolve_x){
		    new_constraint=(constraintADT)GetBlock(sizeof(struct constraint));
		    new_constraint->next=NULL;
		    new_constraint->event1=processkey[i].eventnum;
		    new_constraint->event2=processkey[k].eventnum;
		    new_constraint->separation=
		      (process[processkey[k].start+processkey[k].index-1]
		       [processkey[i].start+processkey[i].index-1]);
		    y_con_list=sets_add_constraints(con_list, new_constraint);	
		    constraints[row_number+sets_x]=
		      sets_add_constraints(constraints[row_number+sets_x],y_con_list);
		  }
		  sets_y=sets_make_consistant(rules, process_y, processkey, num_events, new_event,
					      row_number, conflicts_y, num_sets, process, constraints,
					      y_con_list, num_clocks, clockkey, nevents, info);      
	      }
	      else{
		sets_y=0;
	      }
	      for(j=0;i<((*num_sets)+3);i++){
		if (resolve_x) free(process_x[j]); 
		if (resolve_y) free(process_y[j]); 
	      }    
	      if(resolve_x) free(process_x);
	      if(resolve_y) free(process_y); 
	      if(resolve_x) free(conflicts_x); 
	      if(resolve_y) free(conflicts_y);
		
	      return (sets_x+sets_y);	    
	    }
	  }
	}
      }
    }   
  }
  return 1;
}

void sets_project_set(processADT process, int remove_set, int sets){

  int i,j;

  for(i=remove_set;i<=sets;i++){
    for(j=1;j<=sets;j++){
      process[i][j]=process[i+1][j];     
    }
  }
  for(i=remove_set;i<=sets;i++){
    for(j=1;j<=sets;j++){
      process[j][i]=process[j][i+1];
    }
  }
}


depend_listADT sets_make_set(ruleADT **rules, processkeyADT processkey,
			     processADT process,  constraintADT *constraints,
			     int new_event, int row_number, int *num_events, 
			     int *num_sets, int *valid_set, processADT final_process){

  depend_listADT list;
  constraintADT current;
  constraintADT prev;
  
  list = (depend_listADT)GetBlock (((*num_events)*2+1) * sizeof(struct depend_list));

  *valid_set = sets_make_row(rules, processkey, process,  new_event, 
			row_number, num_events, list, num_sets, final_process);
  if(*valid_set){
    sets_make_column(rules, processkey, process, new_event, row_number, num_events,
		list, num_sets, final_process);
    final_process[row_number][row_number]=0; 
    sets_recanon_process(final_process, process, *num_events, processkey, row_number); 
    if(constraints[row_number]!=NULL){
      current=constraints[row_number]; 
      prev=constraints[row_number]; 
       while(current!=NULL){ 
 	prev=current; 
 	current=current->next; 
 	free(prev); 
       } 
      constraints[row_number]=NULL;
    }   

    /* copy over the constraints for the previous event from the set we are using in this */
    /* combination. */


    if(constraints[processkey[(*num_events-1)].start+processkey[(*num_events-1)].index-1]!=NULL){
      constraints[row_number]=sets_add_constraints(constraints[row_number], 
						   constraints[processkey[(*num_events-1)].start+
							      processkey[(*num_events-1)].index-1]);
    }
 
  }
  return list;
}

int count_contraints(constraintADT constraints){

  int count=0;
  constraintADT current;
  
  current = constraints;
  while(current!=NULL){
    count++;
    current=current->next;
  }
  return count;
}

void remove_constraints(int event1, int event2, constraintADT *constraints, int start, int end){

  int i;
  constraintADT prev;
  constraintADT current;

  for(i=start;i<start+end;i++){
    current=constraints[i];
    prev=constraints[i];
    while((current->event1!=event1) && (current->event2!=event2)){
      prev=current;
      current=current->next;
    }
    if((prev==current) && (current->next==NULL)){
      constraints[i]=NULL;
    }
    else{
      prev->next=current->next;
      free(current);
    }
  }
}


int sets_remove_projections(ruleADT **rules, processADT process, processkeyADT processkey, int new_sets, 
			    int num_events, int num_clocks, clockkeyADT clockkey, int nevents,
			    rule_info_listADT *info, constraintADT *constraints, int old_constraints, 
			    int total_sets){

  int i, j;
  constraintADT this_constraint;
  constraintADT count_constraint;
  constraintADT start_constraint;
  int project_conflict_1=0;
  int project_conflict_2=0;
  int num_new_constraints=0;
  int current_constraint;
  int stride, added_sets;
  int num_projected=0;

  this_constraint=constraints[processkey[num_events].start];
  for(i=0;i<old_constraints;i++){
    this_constraint=this_constraint->next;
  }
  count_constraint=this_constraint;
  while(count_constraint!=NULL){
    num_new_constraints++;
    count_constraint=count_constraint->next;
  }
  if (num_new_constraints == 0) return 0;
  current_constraint=1;
  start_constraint=this_constraint;
  while(this_constraint!=NULL){
    project_conflict_1=enabling_done(rules, num_clocks, processkey, clockkey, this_constraint->event1,
				     processkey[(num_events)].eventnum, nevents, info);
    project_conflict_2=enabling_done(rules, num_clocks, processkey, clockkey, this_constraint->event2,
				     processkey[(num_events)].eventnum, nevents, info);
    if(project_conflict_1 && project_conflict_2){
      stride=(int)pow((double)2, (double)(num_new_constraints-current_constraint));
      added_sets=(int)pow((double)2, (double)(num_new_constraints-1));
      for(i=0;i<(added_sets);i++){
	for(j=1;j<=total_sets;j++){
	  if(process[processkey[(num_events)].start + i][j] < 
	     process[processkey[(num_events)].start + i + stride][j]){
	    process[processkey[(num_events)].start + i][j]=process[processkey[(num_events)].start + i + stride][j];
	  }
	  if(process[j][processkey[(num_events)].start+i] <
	     process[j][processkey[(num_events)].start + i + stride]){
	    process[j][processkey[(num_events)].start+i] = process[j][processkey[(num_events)].start + i + stride];
	  }
	}
      }
      current_constraint++;
    }
    this_constraint=this_constraint->next;
  }
  this_constraint=start_constraint;
  current_constraint=1;
  num_projected=0;
  while(this_constraint!=NULL){
    project_conflict_1=enabling_done(rules, num_clocks, processkey, clockkey, this_constraint->event1,
				     processkey[(num_events)].eventnum, nevents, info);
    project_conflict_2=enabling_done(rules, num_clocks, processkey, clockkey, this_constraint->event2,
				     processkey[(num_events)].eventnum, nevents, info);
    if(project_conflict_1 && project_conflict_2){     
      stride=(int)pow((double)2, (double)(num_new_constraints-current_constraint));
      added_sets=(int)pow((double)2, (double)(num_new_constraints-1));     
      for(i=0;i<(added_sets);i++){
	sets_project_set(process, processkey[(num_events)].start + stride + i - num_projected, total_sets);
	for(j=processkey[(num_events)].start + stride + i - num_projected; j<total_sets;j++){
	  constraints[j]=constraints[j+1];
	}
	  num_projected++;
      }
      remove_constraints(this_constraint->event1, this_constraint->event2, constraints, 
			processkey[(num_events)].start, new_sets);
    }
    this_constraint=this_constraint->next;
  }
  return num_projected;
}



void sets_add_event(eventADT *events, ruleADT **rules, 
		   markkeyADT *markkey, markingADT marking,
		   clocksADT clocks, clockkeyADT clockkey, 
		   processADT *orig_process, processkeyADT *orig_processkey, 
		   constraintADT *constraints,
		   int nevents, int nrules,
		   int num_clocks, int *num_events, 
		   int new_event, int *num_sets, rule_info_listADT *info,
		   int *process_size){
  
  depend_listADT conflicts;
  int sets,temp;
  int tot_sets,i,j,max, old_sets;
  int *max_array;
  int *project_array;
  int projected=0;
  int valid_set=0;
  int *max_array_row;
  int *max_array_column;    
  processADT process;
  processkeyADT processkey;
  processADT new_process;
  processkeyADT new_processkey;
  int f;
  int num_constraints;

  process=*orig_process;
  processkey=*orig_processkey;
  num_constraints=count_contraints(constraints[processkey[(*num_events-1)].start+processkey[(*num_events-1)].index-1]);
  conflicts = sets_make_set(rules, processkey,  process, constraints, new_event,
			    processkey[(*num_events)].start, num_events, num_sets, 
			    &valid_set, process);
  if(valid_set){
    sets=sets_make_consistant(rules, process, processkey,
			      num_events, new_event, processkey[(*num_events)].start, 
			      conflicts, num_sets, process, constraints, NULL, num_clocks,
			      clockkey, nevents, info);
    sets=sets - sets_remove_projections(rules, process, processkey, sets, *num_events, num_clocks, clockkey, 
					nevents, info, constraints, num_constraints, *num_sets + sets);
    if(((*num_sets)+sets) > (*process_size-2)){
      new_process=resize_process(*process_size, 
				 ((*process_size)*PROCESS_MULT),
				 *orig_process);
      new_processkey=sets_resize_processkey(*process_size, 
					    ((*process_size)*PROCESS_MULT),
					    *orig_processkey);	    
      
      for(f=0;f<(*process_size);f++){
	free(process[f]);
      }
      free(process);
      free(processkey);
      *orig_process=new_process;
      *orig_processkey=new_processkey;
      process=new_process;
      processkey=new_processkey;
      *process_size=(*process_size*PROCESS_MULT);
    }      
  }
  while(sets_increment_sets(processkey, num_events)){
    num_constraints=count_contraints
      (constraints[processkey[(*num_events-1)].start+processkey[(*num_events-1)].index-1]);
    conflicts = sets_make_set(rules, processkey,  process, constraints, new_event,
			      processkey[(*num_events)].start+sets, num_events, num_sets, 
			      &valid_set, process);
    if(valid_set){
      sets=sets+sets_make_consistant(rules, process, processkey, num_events, 
				     new_event, processkey[(*num_events)].start+sets,
				     conflicts, num_sets, process, constraints, 
				     NULL, num_clocks, clockkey, nevents, info);
      sets=sets - sets_remove_projections(rules, process, processkey, sets, *num_events, num_clocks, clockkey, 
					  nevents, info, constraints, num_constraints, *num_sets + sets);
      if(((*num_sets)+sets) > (*process_size-2)){
	new_process=resize_process(*process_size, 
				   ((*process_size)*PROCESS_MULT),
				   *orig_process);
	new_processkey=sets_resize_processkey(*process_size, 
					      ((*process_size)*PROCESS_MULT),
					      *orig_processkey);	    
      
	for(f=0;f<(*process_size);f++){
	  free(process[f]);
	}
	free(process);
	free(processkey);
	*orig_process=new_process;
	*orig_processkey=new_processkey;
	process=new_process;
	processkey=new_processkey;
	*process_size=(*process_size*PROCESS_MULT);
      }           
    }
  }
  old_sets=(*num_sets);
  *num_sets=(*num_sets)+sets;
  /* Get rid of sets that are contained in other sets. */
  max_array_row=(int *)GetBlock(((*num_sets)+1) * (sizeof(int)));
  max_array_column=(int *)GetBlock(((*num_sets)+1) * (sizeof(int))); 
  if(sets>1){    
    project_array=(int *) GetBlock(sets * sizeof(int));
    for(i=0;i<sets;i++){
      project_array[i]=1;
    }
    for(i=1;i<=old_sets;i++){
      max=(-INFIN);
      for(j=0;j<sets;j++){
	if(process[(*num_events)+j][i] > max){
	  max_array_row[i]=j;
	  max=process[(*num_events)+j][i];
	}	
      }
      project_array[max_array_row[i]]=0;	
    }
    for(i=1;i<=old_sets;i++){
      max=(-INFIN);
      for(j=0;j<sets;j++){
	if(process[i][(*num_events)+j] > max){
	  max_array_column[i]=j;
	  max=process[i][(*num_events)+j];	  
	}	
      }
      project_array[max_array_column[i]]=0;
    }    
    for(i=0;i<sets;i++){
      if(project_array[i]){
	sets_project_set(process, (*num_events)+i-projected, *num_sets);
	for(j=1;j<=(*num_sets);j++){
	  if(max_array_row[j]>i) max_array_row[j]--;
	  if(max_array_column[j]>i) max_array_column[j]--;
	}
	projected++;
	sets--;
	(*num_sets)--;
      }
    }
  }        
  processkey[(*num_events)].sets=sets;
}
      
  
  

/* Add clocks to the clock matrix that have been added by the new marking. */

int sets_make_new_clocks(eventADT *events, ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT *orig_clocks, clockkeyADT *orig_clockkey, 
			 processADT *orig_process, processkeyADT *orig_processkey,
			 constraintADT *constraints,
			 int nevents, int nrules, rule_info_listADT *info,
			 int fire_enabling, int num_clocks, int *num_events, int *num_sets,
			 int *clock_size, int *process_size){      
  int i,j,k,p,f;
  int ev_exists;
  int enabling1, enabling2;
  int max_sep, min_sep;
  int first=1;
  int instance;
  int cycled;
  int index_i;
  int index_j;
  clocksADT clocks;
  clockkeyADT clockkey;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;
  processADT process;
  processkeyADT processkey;
  processADT new_process;
  processkeyADT new_processkey;
  rule_infoADT *current;

  cycled=0;
  clocks=*orig_clocks;
  clockkey=*orig_clockkey;
  process=*orig_process;
  processkey=*orig_processkey;
  for(i=0;i<nrules;i++){
    if(marking->marking[i]!='F'){
      if(rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype){
	current=
	  (rule_infoADT *)get_rule_info(info, markkey[i]->enabling, markkey[i]->enabled);      
	if((current->clocknum == -1) && (current->exp_enabled == 0) &&
	   (current->confl_removed == 0)){
	  num_clocks++;
	  if(num_clocks > ((*clock_size)-2)){
	    new_clocks=resize_clocks(*clock_size, ((*clock_size)*CLOCK_MULT), 
				     *orig_clocks);
	    new_clockkey=resize_clockkey(*clock_size, ((*clock_size)*CLOCK_MULT),
					 *orig_clockkey);
	    for(f=0;f<(*clock_size);f++){
	      free(clocks[f]);
	    }
	    free(clocks);
	    free(clockkey);
	    *orig_clocks=new_clocks;
	    *orig_clockkey=new_clockkey;
	    clocks=new_clocks;
	    clockkey=new_clockkey;
	    *clock_size=(*clock_size*CLOCK_MULT);
	  }
	  current->clocknum=num_clocks;
	  clockkey[num_clocks].enabling=markkey[i]->enabling;
	  clockkey[num_clocks].enabled=markkey[i]->enabled; 
	  if(first){
	    (*num_events)++;
	    if((*num_events) > ((*process_size)-2)){
	      new_process=resize_process(*process_size, ((*process_size)*PROCESS_MULT),
					 *orig_process);
	      new_processkey=sets_resize_processkey(*process_size, 
						    ((*process_size)*PROCESS_MULT),
						    *orig_processkey);	    
	      
	      for(f=0;f<(*process_size);f++){
		free(process[f]);
	      }
	      free(process);
	      free(processkey);
	      *orig_process=new_process;
	      *orig_processkey=new_processkey;
	      process=new_process;
	      processkey=new_processkey;
	      *process_size=(*process_size*PROCESS_MULT);
	    }
	    clockkey[num_clocks].eventk_num=(*num_events);
	    processkey[(*num_events)].eventnum=markkey[i]->enabling;
	    processkey[(*num_events)].start=
	      processkey[(*num_events)-1].start+processkey[(*num_events)-1].sets;	
	    processkey[(*num_events)].index=1;
	    sets_add_event(events, rules, markkey,  marking, clocks, clockkey, 
			   orig_process, orig_processkey, constraints, nevents, nrules,  
			   num_clocks, num_events, markkey[i]->enabling, num_sets,
			   info, process_size);	  
	    process=*orig_process;
	    processkey=*orig_processkey;
	    first=0;
	  }	
	  else{
	    clockkey[num_clocks].eventk_num=(*num_events);
	  }
	}
      }
    }
  }
  for(i=1;i<=num_clocks;i++){
    index_i=clockkey[i].eventk_num;
    for(j=0;j<=num_clocks;j++){
      index_j=clockkey[j].eventk_num;
      if(j!=0){
	clocks[i][j]=-(INFIN);
	for(k=1;k<=processkey[index_i].sets;k++){
	  for(p=1;p<=processkey[index_j].sets;p++){
	    if(process[processkey[index_i].start+k-1][processkey[index_j].start+p-1] >
	       clocks[i][j]){
	      clocks[i][j]=process[processkey[index_i].start+k-1][processkey[index_j].start+p-1];
	    }
	  }
	}
      }
      else clocks[i][j]=0;
    }
  }
  return num_clocks;
  }    

void sets_recanon_process(processADT final_process, processADT process, int num_clocks, 
			  processkeyADT processkey,  int row_number){
  
  int i, j, k;
  int temp;
  int set_index_k;
  int set_index_j;
  int set_index_i;
  int i_k;
  int k_j;

  for(k=1;k<=num_clocks;k++){
    if(k!=num_clocks){
      set_index_k=processkey[k].start+processkey[k].index-1;      
    }
    else set_index_k=row_number;
    for(i=1;i<=num_clocks;i++){
      if(i!=num_clocks){
	set_index_i=processkey[i].start+processkey[i].index-1; 	
      }
      else set_index_i=row_number;
      if((set_index_i==row_number) || (set_index_k==row_number)){
	i_k=final_process[set_index_i][set_index_k];
      }
      else{
	i_k=process[set_index_i][set_index_k];
      }
      if((i_k!=INFIN) && 
	 (final_process[set_index_k][row_number]!=INFIN)){
	if((i_k==(-INFIN)) || 
	   (final_process[set_index_k][row_number]==(-INFIN)))
	  final_process[set_index_i][row_number]=(-INFIN);
	else{
	  if(final_process[set_index_i][row_number] > 
	     (i_k+final_process[set_index_k][row_number])){
	    final_process[set_index_i][row_number] = 
	      (i_k+final_process[set_index_k][row_number]);
	  }
	}
      }
      else if((i_k==(-INFIN)) || 
	      (final_process[set_index_k][row_number]==(-INFIN))){
	if(final_process[set_index_i][row_number] > 0){
	  final_process[set_index_i][row_number]=0;
	}
      }
    }
    
    for(j=1;j<=num_clocks;j++){
      if(j!=num_clocks){
	set_index_j=processkey[j].start+processkey[j].index-1;
      }
      else set_index_j=row_number;
      if((set_index_i==row_number) || (set_index_j==row_number)){
	k_j=final_process[set_index_k][set_index_j];
      }
      else{
	k_j=process[set_index_k][set_index_j];
      }      
      if((final_process[row_number][set_index_k]!=INFIN) &&
	 (k_j!=INFIN)){
	if((final_process[row_number][set_index_k]==(-INFIN)) || 
	   (k_j==(-INFIN)))
	  final_process[row_number][ set_index_j]=(-INFIN);
	else{
	  if(final_process[row_number][set_index_j] > 
	     (final_process[row_number][set_index_k] + k_j)){
	    final_process[row_number][ set_index_j] = 
	      (final_process[row_number][set_index_k] + k_j);	
	  }
	}
      }
      else if((final_process[row_number][set_index_k]==(-INFIN)) || 
	      (k_j==(-INFIN))){
	if(final_process[row_number][set_index_j] > 0)
	  final_process[row_number][set_index_j] = 0; 	
      }
    }
  }
}
  



int sets_remove_conflicts(eventADT *events, ruleADT **rules, 
		     markkeyADT *markkey, markingADT marking,
		     clocksADT clocks, clockkeyADT clockkey,
		     processADT process, processkeyADT processkey,
		     int nevents, int nrules, rule_info_listADT *info,
		     int fire_enabling, int fire_enabled, int num_clocks,
		     int *num_events, int *num_sets){
  int i,j;
  int new_num_clocks;
  rule_infoADT *check_rule_i_fe=NULL;
  rule_infoADT *check_rule_j_fe=NULL;
  rule_infoADT *check_rule_i_j=NULL;

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    if((rules[i][fire_enabling]->conflict==0) && (rules[i][fire_enabled]->ruletype)){
      check_rule_i_fe=(rule_infoADT *)get_rule_info(info, i, fire_enabled);
      if((check_rule_i_fe->exp_enabled==1) && (check_rule_i_fe->confl_removed==0)){
	for(j=0;j<nevents;j++){
	  if(rules[i][j]->ruletype){
	    check_rule_i_j=(rule_infoADT *)get_rule_info(info, i,j);
	    if((rules[j][fire_enabled]->conflict==1) && check_rule_i_j->clocknum!=(-1)){
	      check_rule_i_fe->confl_removed=1;
	      new_num_clocks=sets_project_clocks(events, rules, markkey, marking,
						 clocks, clockkey, process, processkey,
						 nevents, nrules, info,
						 i, j, new_num_clocks, num_events,0, num_sets,
						 NULL);
	}
	    else if((rules[j][fire_enabled]->conflict==1)){
	      check_rule_i_j=(rule_infoADT *)get_rule_info(info, i, j);
	      check_rule_i_j->exp_enabled=0;	
	    }
	  }
	}
      } 
    }
  }
  return new_num_clocks;
}




int sets_remove_enabling_conflicts(eventADT *events, ruleADT **rules, 
				   markkeyADT *markkey, markingADT marking,
				   clocksADT clocks, clockkeyADT clockkey, 
				   processADT process, processkeyADT processkey,
				   int nevents, int nrules, rule_info_listADT *info,
				   int fire_enabling, int fire_enabled, 
				   int num_clocks, int *num_events, int *num_sets){
  int i,j,k;
  int new_num_clocks;
  rule_infoADT *checked_rule;

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    if(rules[fire_enabling][i]->conflict==1 && !(actual_fired(i, rules, nevents, info))){
      for(j=0;j<nevents;j++){
	if(rules[i][j]->ruletype && (j==fire_enabled)){
	  for(k=0;k<nevents;k++){
	    if(rules[i][k]->ruletype){
	      checked_rule=(rule_infoADT *)get_rule_info(info, i, k);
	      if(checked_rule->clocknum!=(-1)){     
		checked_rule->exp_enabled=1;
		new_num_clocks=sets_project_clocks(events, rules, markkey, marking,
						   clocks, clockkey, process, processkey,
						   nevents, nrules, info,
						   i, k, new_num_clocks,
						   num_events,0, num_sets, NULL);
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

void sets_push_work(workstackADT *workstack, markkeyADT *markkey, 
		    markingADT marking, firinglistADT firinglist,
		    clocksADT clocks, clockkeyADT clockkey,
		    processADT process, processkeyADT processkey,
		    string state, int nevents, int nrules, 
		    rule_info_listADT *old_info, int num_clocks, 
		    int num_events, int num_sets, constraintADT *constraints,
		    int clock_size, int process_size){
  
  workstackADT new_workstack=0;
  workADT new_work=0;
  rule_info_listADT *new_rule_info=NULL;
  clocksADT new_clocks;
  clockkeyADT new_clockkey;
  processADT new_process;
  processkeyADT new_processkey;

  new_work=(workADT)GetBlock(sizeof( *new_work));
//  new_work->markkey = markkey;
  new_work->marking = marking;
  new_work->firinglist = firinglist;
  new_clocks=(clocksADT) copy_clocks(clock_size, clocks);
  new_work->clocks = new_clocks;
  new_process=(processADT) copy_process(process_size, process);
  new_work->process=new_process;
  new_clockkey=(clockkeyADT) copy_clockkey(clock_size,clockkey);
  new_work->clockkey = new_clockkey;
  new_processkey= (processkeyADT) sets_copy_processkey(process_size, processkey);
  new_work->processkey=new_processkey;
  new_rule_info=(rule_info_listADT *)copy_rule_info(old_info, nevents);
  new_work->rule_info = new_rule_info;
  new_work->state = state;
  new_work->num_clocks=num_clocks;
  new_work->num_events=num_events;
  new_work->num_sets=num_sets;
  new_work->clock_size = clock_size;
  new_work->process_size=process_size;
  new_work->constraints=sets_copy_constraints(nevents, constraints);
  new_workstack = (workstackADT)GetBlock(sizeof( *new_workstack));
  new_workstack->work=new_work;
  new_workstack->next=(*workstack);
  (*workstack)=new_workstack;
}

			 
void sets_free_structures(markkeyADT *markkey, 
			  markingADT marking, firinglistADT firinglist,
			  clocksADT clocks, clockkeyADT clockkey, 
			  processADT process, processkeyADT processkey,
			  rule_info_listADT *info, int nrules, int nevents,
			  int clock_size, int process_size){

  int i, j;
  path_listADT *current;
  path_listADT *prev;

  for(i=0;i<(clock_size);i++){
    free(clocks[i]);
  }
  for(i=0;i<(process_size);i++){
    free(process[i]);
  }
  free(clocks);
  free(process);
  free(processkey);
  free(clockkey);
  for(i=0;i<nevents;i++){
    for(j=0;j<info[i].num_rules;j++){
      free(info[i].list[j]);
    }
    current=info[i].path_list;
    while(current!=NULL){
      prev=current;
      current=current->next;
      free(prev);
    }
  }
  free(info);
}

/*****************************************************************************/
/* Use the new, improved C version of orbits to find state graph             */
/*****************************************************************************/

bool sets_rsg(string filename,signalADT *signals,eventADT *events,
	      mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	      stateADT *state_table, markkeyADT *markkey, int nevents,
	      int ncycles,int nsignals,int ninpsig,int nsigs, int nrules, 
	      string startstate,bool si,bool verbose,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,bool subsets, bool genrg,
	      int ndummy)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int i,j;
  markingADT marking = NULL;
  markingADT old_marking = NULL;
  clocksADT clocks = NULL;
  clocksADT old_clocks=NULL;
  processADT process = NULL;
  processADT old_process = NULL;
  firinglistADT firinglist = NULL;
  firinglistADT temp = NULL;
  int num_clocks = 0;
  int old_num_clocks =0;
  workstackADT workstack=NULL;
  workADT current_work=NULL;
  int firing_event, firing_signal;
  clockkeyADT clockkey;
  clockkeyADT old_clockkey;
  processkeyADT processkey=NULL;
  processkeyADT old_processkey=NULL;
  constraintADT *constraints;
  int fire_enabling;
  int fire_enabled;
  bool event_fired=0;
  int sn=0;
  int new_state;
  int first=1;
  int regions=0;
  int num_events=0;
  int num_sets=0;
  rule_info_listADT *rule_info;
  rule_infoADT *current_rule_info;
  int clock_size;
  int old_clock_size;
  int process_size;
  timeval t0,t1;
  double time0, time1;

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
/*   printf("There are %d events, %d signals, %d inpsig, %d nsigs, %d rules\n",  */
/* 	 nevents,  nsignals, ninpsig,  nsigs, nrules); */
/*   print_events(nevents, events); */
  gettimeofday(&t0, NULL);
  rule_info=make_rule_info(rules, nevents);
  marking = (markingADT)find_initial_marking(events, rules, markkey, nevents, 
					     nrules, nsigs, startstate, 
					     verbose);
  clock_size=mark_rules(rules, markkey, marking, nrules)+2;
  if(clock_size<MIN_CLOCK_SIZE) clock_size=MIN_CLOCK_SIZE;
  clockkey = create_clockkey(clock_size);  
  clocks = create_clocks(clock_size);
  process = create_process(clock_size);
  processkey = sets_create_processkey(clock_size);
  process_size=clock_size;
  old_clock_size=clock_size;
  constraints = sets_create_constraints(nevents);
  num_clocks=sets_mark_initial_region(rules, markkey, marking, clocks, clockkey,
				      nevents, nrules, rule_info);
  num_events=sets_mark_process(rules, process, processkey, clocks, 
			       clockkey, num_clocks);
  num_sets=num_events;
  add_state(fp, state_table, NULL, NULL, NULL, NULL,
	    NULL, 0, NULL, marking->state, marking->marking, marking->enablings,
	    nsigs, clocks, clockkey, num_clocks, sn, verbose, 1, nrules,
	    old_clock_size, clock_size,bdd_state, use_bdd,marker,subsets, genrg,
	    -1, -1, NULL, NULL);
  sn++;
  regions++;
  firinglist = (firinglistADT)old_get_t_enabled_list(rules, clocks, clockkey,
  						 num_clocks, nevents, nrules);

  /* Main orbits loop, exits when there are no unexplored states */
  while(1){ 
/*     print_firinglist(firinglist);   */
    fire_enabling=firinglist->enabling;
    fire_enabled=firinglist->enabled;
    temp=firinglist;
    firinglist=firinglist->next;
    free(temp);
/*     printf("firing rule {%d,%d}\n", fire_enabling, fire_enabled);   */  
    /* If there are unfired events, put this state and firinglist */
    /* on the stack to explore later.                            */
    
    if(firinglist!=NULL){
      sets_push_work(&workstack, markkey, marking, firinglist, clocks, 
		     clockkey, process, processkey, startstate, nevents, nrules, 
		     rule_info, num_clocks, num_events, num_sets, constraints,
		     clock_size, process_size);   
    } 
    old_clocks=copy_clocks(clock_size, clocks);
    old_clockkey=copy_clockkey(clock_size, clockkey);
    old_num_clocks=num_clocks;
    old_clock_size=clock_size;    
     /*Mark this rule as exp_enabled */
    num_clocks=sets_remove_enabling_conflicts(events, rules, markkey, marking, 
					      clocks, clockkey, process, processkey,
					      nevents, nrules, rule_info, fire_enabling,
					      fire_enabled, num_clocks, 
					      &num_events, &num_sets);
    current_rule_info=(rule_infoADT *)get_rule_info(rule_info, fire_enabling, fire_enabled);
    current_rule_info->exp_enabled=1;   
/*     puts("trying to fire event");  */
     if(genrg){
      old_marking=marking;
     }
    if(enabled_event(fire_enabled, fire_enabling, rules, nevents, rule_info)){
      num_clocks=sets_remove_conflicts(events, rules, markkey, marking, clocks,
				       clockkey, process, processkey,
				       nevents, nrules, rule_info, fire_enabling,
				       fire_enabled, num_clocks, &num_events, &num_sets);
      old_marking=marking;
//      firing_signal=(fire_enabled-1)/2;
/*        printf("firing signal = %d\n", firing_signal);  */
/*       print_marking(marking); */
      if(fire_enabled < (nevents-ndummy)){
	marking = (markingADT) find_new_marking(events, rules, markkey, marking, 
						fire_enabled, nevents, nrules, 
						0, nsigs, FALSE);
      }
      else{
	marking = (markingADT) dummy_find_new_marking(events, rules, markkey, marking, 
						fire_enabled, nevents, nrules, 
						0, nsigs, FALSE);
      }
/*       print_marking(marking);  */
      clear_event(fire_enabled, rules, rule_info, nevents);
      mark_firing(fire_enabled, rules, nevents, rule_info);
      event_fired=1;
    }
    else{ 
      event_fired=0;
    }

    if(!event_fired){
      num_clocks=sets_project_clocks(events, rules, markkey, marking, clocks,
				     clockkey, process, processkey, 
				     nevents, nrules, rule_info, fire_enabling,
				     fire_enabled, num_clocks, &num_events,0, &num_sets,
				     constraints);

    }
    else{
      num_clocks=sets_make_new_clocks(events, rules, markkey, marking, &clocks,
				      &clockkey, &process, &processkey, constraints, nevents, 
				      nrules, rule_info, fire_enabling, num_clocks, 
				      &num_events, &num_sets, &clock_size, &process_size);
      num_clocks=sets_project_clocks(events, rules, markkey, marking, clocks,
				     clockkey, process, processkey, 
				     nevents, nrules, rule_info, fire_enabling,
				     fire_enabled, num_clocks, &num_events, 1, &num_sets,
				     constraints);
    }

    advance_time(events, rules, markkey, marking, clocks,
		 clockkey, nevents, nrules, 
		 rule_info, firing_event, num_clocks);
    normalize(clocks, num_clocks, clockkey, rules, FALSE, NULL, 0); 
 /*    puts("normalized"); */    
    if(event_fired || genrg){
      new_state=add_state(fp, state_table, old_marking->state,
			  old_marking->marking, old_marking->enablings,
			  old_clocks, old_clockkey, old_num_clocks, NULL, 
			  marking->state, marking->marking, marking->enablings,
			  nsigs, clocks, clockkey, num_clocks, sn,
			  verbose, 1, nrules, old_clock_size, clock_size,
			  bdd_state, use_bdd,marker,subsets, genrg,
			  fire_enabling, fire_enabled, NULL, NULL);
    }
    else{
      new_state=add_state(fp, state_table, NULL, NULL, NULL, NULL,
			  NULL,0, NULL, marking->state, marking->marking,
			  marking->enablings, nsigs, clocks, clockkey,
			  num_clocks, sn, verbose, 1, nrules, old_clock_size, 
			  clock_size,bdd_state, use_bdd,marker,subsets, genrg,
			  -1, -1, NULL, NULL);
    }
    if(new_state){
      regions++;
      if((regions%50)==0){
	printf("%d regions \n", regions);
      }
      if(new_state==1){
	sn++;
      }
      //   firinglist = (firinglistADT)get_t_enabled_list(rules, clocks, clockkey, num_clocks,
      //						     nevents, nrules);
    }
    else{
      current_work=pop_work(&workstack);
      if(current_work==NULL){
/* 	my_print_graph(state_table); */
	gettimeofday(&t1,NULL);	
	time0 = (t0.tv_sec+(t0.tv_usec*.000001));
	time1 = (t1.tv_sec+(t1.tv_usec*.000001));
	printf("%d regions explored, %d states explored in %g seconds\n",regions, sn,
	       time1-time0);
	printf("done\n");
	if(verbose) fclose(fp);
	return(TRUE);	
      }
      else{
	/* don't free the first rules matrix since the rest of the */
	/* program needs it */
	if(!first){
	  sets_free_structures(markkey, marking, firinglist, clocks, clockkey,
			       process, processkey, rule_info, nrules, nevents,
			       clock_size, process_size);
	}
	first=0;
//	markkey=current_work->markkey;
	marking=current_work->marking;
	firinglist=current_work->firinglist;
	clocks=current_work->clocks;
	clockkey=current_work->clockkey;
	rule_info=current_work->rule_info;
	processkey=current_work->processkey;
	process=current_work->process;
	num_clocks=current_work->num_clocks;
	num_events=current_work->num_events;
	num_sets=current_work->num_sets;
	constraints=current_work->constraints;
	clock_size=current_work->clock_size;
	process_size=current_work->process_size;
/* 	puts("getting new work");  */
/* 	print_clocks(clocks, num_clocks); */
/*  	print_clockkey(num_clocks, clockkey); */
      }
    }
  }
}






