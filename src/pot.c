#include "pot.h"
#include "findrsg.h"
#define DEBUG
#define MIN_CLOCK_SIZE 5
#define CLOCK_MULT 2
#define PROCESS_MULT 2

/* Sets up the clock region to reflect the initial marking */
/* Looks at the initial marking and finds the maximum time */
/* that can pass before some rule has to fire.             */


int pot_mark_initial_region(ruleADT **rules,  
			    markkeyADT *markkey, markingADT marking, 
			    clocksADT clocks, clockkeyADT clockkey,
			    int nevents, int nrules,
			    rule_info_listADT *info){

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
       if(rules[j][i]->ruletype) add_event_to_path(j, i, info);
       if((rules[j][i]->epsilon==1) && (rules[j][i]->ruletype)){
	 add_event_to_path(j, i, info);
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
 
/* Eliminate clocks that have been used in this firing. */

int pot_project_clocks(eventADT *events, ruleADT **rules, 
		       markkeyADT *markkey, markingADT marking,
		       clocksADT clocks, clockkeyADT clockkey, 
		       processADT process, processkeyADT processkey,
		       int nevents, int nrules, rule_info_listADT *info,
		       int fire_enabling, int fire_enabled, int num_clocks,
		       int *num_events, int projecting_event){

  int i,j,k,l,p;
  rule_infoADT *current_info_this_rule;
  rule_infoADT *current_info;
  int project_event=0;
  int removed_event_num;
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
  if(projecting_event){ 
    for(j=1;j<=(*num_events);j++){
      while(processkey[l].eventnum!=fire_enabling){
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
    for(j=1;j<=(*num_events);j++){
      l=removed_event_num;
      while(l<=(*num_events)){
	process[l][j]=process[l+1][j];
	/* Only compress the processkey the first time through */
	if(j==1){
	  processkey[l].eventnum=processkey[l+1].eventnum;
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

/* Returns the max(t(b) - t(a))*/

int MaxDiff(ruleADT **rules, processADT process, processkeyADT processkey, 
	    int *num_events, int nevents, int a, int instance_a, int b,
	    int instance_b, int progress, rule_info_listADT *info){

  int maxdiff=-(INFIN);
  int currdiff=0;
  int i,j,k=0;
  int index_a, index_b;

  if((a==b) && (instance_a == instance_b)) return 0;
  for(i=1;i<=(*num_events);i++){
    if ((processkey[i].eventnum==a) && (processkey[i].instance==instance_a)) 
      index_a=i;
    if ((processkey[i].eventnum==b) && (processkey[i].instance==instance_b))
      index_b=i;
  }
  if ((index_a!=(*num_events)) && (index_b!=(*num_events)))
    return process[index_b][index_a];
  if ( (index_a==(*num_events)) && index_b < progress) 
     return process[index_b][index_a];    
  if ( (index_b==(*num_events)) && index_a < progress)  
     return process[index_b][index_a];     
  if(processkey[index_b].root==1){
    if (((is_path(b, a, info)) && (processkey[index_a].root!=1))|| 
	(processkey[index_b].instance < processkey[index_a].instance))
      maxdiff=0;  
    else
      maxdiff=-(INFIN);       
    }
  else{
    for(i=0;i<nevents;i++){
      if(rules[i][b]->ruletype){
	j=1;
	while(j<(index_b)){
	  if((processkey[j].eventnum==i) && (processkey[j].instance == instance_b) 
	     && (!processkey[index_b].root) && !((processkey[j].root) && 
						 !(rules[i][b]->epsilon))){
	    break;
	  }
	  if((processkey[j].eventnum==i) && (processkey[j].instance == instance_b-1) 
	     && (processkey[index_b].root)){  
	    break;
	  }
	  if((processkey[j].eventnum==i) && (processkey[j].instance == instance_b+1) &&
	     (processkey[j].root) && !(rules[i][b]->epsilon)){
	    break;
	  }
	  j++;
	}	
	if(j<(index_b)){
	  currdiff=MaxDiff(rules, process, processkey, num_events, nevents, a,
			   processkey[index_a].instance, i, processkey[j].instance,
			   progress, info);
	  if((currdiff!=INFIN) && (rules[i][b]->upper!=INFIN)){
	    currdiff=currdiff + rules[i][b]->upper;
	  }
	  else{
	    currdiff=INFIN;
	  }
	  if(currdiff > maxdiff)
	    maxdiff=currdiff;
	}
      }
    }
  }

  if (((is_path(b,a,info)) && (processkey[index_a].root!=1))|| 
     (processkey[index_b].instance < processkey[index_a].instance)){
    for(i=0;i<nevents;i++){
      if(rules[i][a]->ruletype){
	j=1;
	while(j<(index_a)){
	  if((processkey[j].eventnum==i) && (processkey[j].instance == instance_a) 
	     && (!processkey[index_a].root) &&
	     !((processkey[j].root) && !(rules[i][a]->epsilon))){
	    break;
	  }
	  if((processkey[j].eventnum==i) && (processkey[j].instance == 
					     instance_a-1) 
	     && (processkey[index_a].root)){  
	    break;
	  }
	  if((processkey[j].eventnum==i) && (processkey[j].instance == instance_a+1) &&
	     (processkey[j].root) && !(rules[i][a]->epsilon)){
	    break;
	  }	  
	  j++;
	}	
	if(j<(index_a)){
	  currdiff=MaxDiff(rules, process, processkey, num_events, nevents, i,
			   processkey[j].instance, b,  processkey[index_b].instance,
			   progress, info);
	  if(currdiff!=INFIN){
	    currdiff=currdiff - rules[i][a]->lower;
	  }
	  else{
	    currdiff=INFIN;
	  }
	  if(currdiff<maxdiff) maxdiff=currdiff;
	}
      }
    }
    if(processkey[index_b].root==1){
      for(i=1;processkey[i].root==1;i++){
	process[i][(*num_events)]=maxdiff;
      }
    }
  }
  return maxdiff;
}
  

/* Add clocks to the clock matrix that have been added by the new marking. */

int pot_make_new_clocks(eventADT *events, ruleADT **rules, 
			markkeyADT *markkey, markingADT marking,
			clocksADT *orig_clocks, clockkeyADT *orig_clockkey, 
			processADT *orig_process, processkeyADT *orig_processkey,
			int nevents, int nrules, rule_info_listADT *info,
			int fire_enabling, int num_clocks, int *num_events,
			int *max_unwind, int *clock_size, int *process_size){      
  int i,j,k,p,f,s;
  int ev_exists;
  int enabling1, enabling2;
  int max_sep, min_sep;
  int first=1;
  int instance;
  int cycled;
  int done_roots=0;
  int min, orig_num_clocks;
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
  orig_num_clocks=num_clocks;
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
	  if(first){
	    for(j=1;j<=(*num_events);j++){
	      if(processkey[j].eventnum==markkey[i]->enabling){
		cycled=1;
		break;
	      }
	    }
	  }
	  if(!cycled){
	    for(j=1;j<nevents;j++){
	      if(is_path(j, markkey[i]->enabling, info)){
		add_event_to_path(j, markkey[i]->enabled, info);
	      }
	    }
	  }
	  current->clocknum=num_clocks;
	  clockkey[num_clocks].enabling=markkey[i]->enabling;
	  clockkey[num_clocks].enabled=markkey[i]->enabled;
	  if(first){
	    (*num_events)++;
	    if((*num_events) > ((*process_size)-2)){
	      new_process=resize_process(*process_size, ((*process_size)*PROCESS_MULT),
					 *orig_process);
	      new_processkey=resize_processkey(*process_size, ((*process_size)*PROCESS_MULT),
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
	    instance=1;
	    processkey[(*num_events)].eventnum=markkey[i]->enabling;
	    if(rules[markkey[i]->enabling][markkey[i]->enabled]->epsilon){
	      processkey[(*num_events)].root=2;
	    }
	    else{
	      for(s=0;s<nevents;s++){
		if(rules[markkey[i]->enabling][s]->epsilon==1){
		  processkey[(*num_events)].root=2;
		  break;
		}
	      }
	      if(s==nevents) processkey[(*num_events)].root=0;
	    } 
	    for(j=((*num_events)-1);j>=1;j--){
	      if(processkey[j].eventnum==markkey[i]->enabling){
		instance=processkey[j].instance+1;
	      }	 
	    }
	    for(j=((*num_events)-1);j>=1;j--){
	      if(rules[processkey[j].eventnum][markkey[i]->enabling]->ruletype){
		if((processkey[j].instance >= instance) && (!(processkey[j].root) ||
		   (rules[processkey[j].eventnum][markkey[i]->enabling]->epsilon)))
		  {
		  instance=processkey[j].instance;	     
		  if(processkey[(*num_events)].root){
		    instance++;
		  }
		}
	      }
	    }
	    processkey[(*num_events)].instance=instance;
	    if(instance > (*max_unwind)) *max_unwind=instance;
	    for(j=1;j<=(*num_events);j++){
	      if(!done_roots || (processkey[j].root!=1)){
		process[j][(*num_events)]=MaxDiff(rules, process, processkey, 
						  num_events, nevents, 
						  processkey[(*num_events)].
						  eventnum, processkey[(*num_events)].
						  instance, processkey[j].eventnum,
						  processkey[j].instance, j, info);
	      }
	      if((processkey[j].root==1) && (process[j][(*num_events)]!=(-INFIN))){
		done_roots=1;
	      }
	      process[(*num_events)][j]=MaxDiff(rules, process, processkey, 
						num_events, nevents, processkey[j].
						eventnum, processkey[j].instance,
						processkey[(*num_events)].eventnum,
						processkey[(*num_events)].instance,
						j, info);
	    }
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
    for(j=orig_num_clocks;j<=num_clocks;j++){
	clocks[i][j]=process[clockkey[i].eventk_num][clockkey[j].eventk_num];
	clocks[j][i]=process[clockkey[j].eventk_num][clockkey[i].eventk_num];
    }
  }
  min=get_partial_order_minimum(clocks, num_clocks);
  for(i=orig_num_clocks+1; i<=num_clocks;i++){
      clocks[i][0]=0;
  }
  return num_clocks;
}    

int pot_remove_conflicts(eventADT *events, ruleADT **rules, 
			 markkeyADT *markkey, markingADT marking,
			 clocksADT clocks, clockkeyADT clockkey,
			 processADT process, processkeyADT processkey,
			 int nevents, int nrules,  rule_info_listADT *info,
			 int fire_enabling, int fire_enabled, int num_clocks,
			 int *num_events){
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
	      new_num_clocks=pot_project_clocks(events, rules, markkey, marking,
						clocks, clockkey, process, processkey,
						nevents, nrules, info,
						i, j, new_num_clocks, num_events,0);
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




int pot_remove_enabling_conflicts(eventADT *events, ruleADT **rules, 
				  markkeyADT *markkey, markingADT marking,
				  clocksADT clocks, clockkeyADT clockkey, 
				  processADT process, processkeyADT processkey,
				  int nevents, int nrules,  rule_info_listADT *info,
				  int fire_enabling, int fire_enabled, 
				  int num_clocks, int *num_events){
  int i,j,k;
  int new_num_clocks;
  int first=1;
  rule_infoADT *checked_rule;

  new_num_clocks=num_clocks;
  for(i=0;i<nevents;i++){
    if(rules[fire_enabling][i]->conflict==1 && !(actual_fired(i, rules, nevents, info))){
      first=1;
      for(j=0;j<nevents;j++){
	if(rules[i][j]->ruletype && (j==fire_enabled)){
	  for(k=0;k<nevents;k++){
	    if(rules[i][k]->ruletype){
	      checked_rule=(rule_infoADT *)get_rule_info(info, i, k);	    
	      if(checked_rule->clocknum!=(-1)){     
		checked_rule->exp_enabled=1;
		new_num_clocks=pot_project_clocks(events, rules, markkey, marking,
						  clocks, clockkey, process, processkey,
						  nevents, nrules, info,
						  i, k, new_num_clocks,
						  num_events,first);
		first=0;
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

void pot_push_work(workstackADT *workstack, markkeyADT *markkey, 
		   markingADT marking, firinglistADT firinglist,
		   clocksADT clocks, clockkeyADT clockkey,
		   processADT process, processkeyADT processkey,
		   string state, int nevents, int nrules, rule_info_listADT *old_info,
		   int num_clocks, int num_events, int clock_size, int process_size){

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
  new_processkey= (processkeyADT) copy_processkey(process_size, processkey);
  new_work->processkey=new_processkey;
  new_rule_info=(rule_info_listADT *)copy_rule_info(old_info, nevents);
  new_work->rule_info = new_rule_info;
  new_work->state = state;
  new_work->clock_size=clock_size;
  new_work->process_size=process_size;
  new_work->num_clocks=num_clocks;
  new_work->num_events=num_events;
  new_workstack = (workstackADT)GetBlock(sizeof( *new_workstack));
  new_workstack->work=new_work;
  new_workstack->next=(*workstack);
  (*workstack)=new_workstack;
}

		 
void pot_free_structures(markkeyADT *markkey, 
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
  free(info[0].list[0]);
  free(info[0].list);
  for(i=1;i<nevents;i++){
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


bool pot_rsg(string filename,signalADT *signals,eventADT *events,
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
  int fire_enabling;
  int fire_enabled;
  bool event_fired=0;
  int sn=0;
  int new_state;
  int first=1;
  int regions=0;
  int num_events=0;
  int max_unwind=0;
  rule_info_listADT *rule_info;
  rule_infoADT *current_rule_info;
  int clock_size;
  int old_clock_size;
  int process_size;
  initialize_state_table(LOADED,FALSE,state_table);
  timeval t0,t1;
  double time0, time1;

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
   processkey = create_processkey(clock_size);
   process_size=clock_size;
   old_clock_size=clock_size;
   num_clocks=pot_mark_initial_region(rules, markkey, marking, clocks, clockkey,
				      nevents, nrules, rule_info);
   num_events=mark_process(rules, process, processkey, clocks, 
			   clockkey, num_clocks);

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
/*       print_firinglist(firinglist); */
    if(firinglist==NULL){
      printf("System deadlocked in state %s\n", marking->state);
      return FALSE;
    }
     fire_enabling=firinglist->enabling; 
     fire_enabled=firinglist->enabled;
     temp=firinglist;
     firinglist=firinglist->next;
     free(temp);
     /*      printf("firing rule {%d,%d}\n", fire_enabling, fire_enabled);   */  
     /* If there are unfired events, put this state and firinglist */
     /* on the stack to explore later.                            */
     
     if(firinglist!=NULL){
       pot_push_work(&workstack, markkey, marking, firinglist, clocks, 
		     clockkey, process, processkey, startstate, nevents, nrules, 
		     rule_info, num_clocks, num_events, clock_size, process_size);   
    } 
     old_clocks=copy_clocks(clock_size, clocks);
     old_clockkey=copy_clockkey(clock_size, clockkey);
     old_num_clocks=num_clocks;
     old_clock_size=clock_size;     
     /*Mark this rule as exp_enabled */
     num_clocks=pot_remove_enabling_conflicts(events, rules, markkey, marking, 
					      clocks, clockkey, process, processkey,
					      nevents, nrules, rule_info, fire_enabling,
					      fire_enabled, num_clocks, 
					      &num_events);
     current_rule_info=(rule_infoADT *)get_rule_info(rule_info, fire_enabling, fire_enabled);
     current_rule_info->exp_enabled=1;     
     if(genrg){
      old_marking=marking;
     }
     if(enabled_event(fire_enabled, fire_enabling, rules, nevents, rule_info)){
       printf("Firing event: %s Enabled by event: %s \n", events[fire_enabled]->event,
	      events[fire_enabling]->event);
       num_clocks=pot_remove_conflicts(events, rules, markkey, marking, clocks,
				       clockkey, process, processkey,
				       nevents, nrules, rule_info, fire_enabling,
				       fire_enabled, num_clocks, &num_events);
       old_marking=marking;
       //firing_signal=(fire_enabled-1)/2;
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
       num_clocks=pot_project_clocks(events, rules, markkey, marking, clocks,
				     clockkey, process, processkey, 
				     nevents, nrules, rule_info, fire_enabling,
				     fire_enabled, num_clocks, &num_events,0);
       
     }
     else{
       num_clocks=pot_make_new_clocks(events, rules, markkey, marking, &clocks,
				      &clockkey, &process, &processkey, nevents, 
				      nrules, rule_info, fire_enabling, num_clocks, 
				      &num_events, &max_unwind, &clock_size, &process_size);
       num_clocks=pot_project_clocks(events, rules, markkey, marking, clocks,
				     clockkey, process, processkey, 
				     nevents, nrules, rule_info, fire_enabling,
				     fire_enabled, num_clocks, &num_events, 0);
     }
     
     advance_time(events, rules, markkey, marking, clocks,
		  clockkey, nevents, nrules, rule_info,
		  firing_event, num_clocks);
     normalize(clocks, num_clocks, clockkey, rules, FALSE, NULL, 0); 
     /*    puts("normalized"); */    
     if(event_fired || genrg){
       new_state=add_state(fp, state_table, old_marking->state,
			   old_marking->marking, old_marking->enablings,
			   old_clocks, old_clockkey, old_num_clocks, NULL, 
			   marking->state, marking->marking,marking->enablings,
			   nsigs, clocks, clockkey, num_clocks, sn,
			   verbose, 1, nrules, old_clock_size, clock_size,
			   bdd_state, use_bdd,marker,subsets, genrg, fire_enabling,
			   fire_enabled, NULL, NULL);
     }
     else{
       new_state=add_state(fp, state_table, NULL, NULL, NULL, NULL,
			   NULL,0, NULL, marking->state, marking->marking,
			   marking->enablings, nsigs, clocks, clockkey,
			   num_clocks, sn, verbose, 1, nrules, old_clock_size,
			   clock_size, bdd_state, use_bdd,marker,subsets, 
			   genrg, -1, -1, NULL, NULL);
     }
     if(new_state){
       regions++;
       if((regions%50)==0){
	 printf("%d regions \n", regions);
       }
       if(new_state==1){
	 sn++;
       }
       firinglist = (firinglistADT)old_get_t_enabled_list(rules, clocks, clockkey, num_clocks,
							  nevents, nrules);
     }
     else{
       current_work=pop_work(&workstack);
/*        printf("getting new work\n");  */
       if(current_work==NULL){
// 	 my_print_graph(state_table);  
	 gettimeofday(&t1,NULL);	
	 time0 = (t0.tv_sec+(t0.tv_usec*.000001));
	 time1 = (t1.tv_sec+(t1.tv_usec*.000001));
	 printf("\n%d regions explored, %d states explored in %g seconds \n",
		regions, sn, time1-time0);
	 printf("Graph unwound %d times\n", max_unwind);
	 printf("done\n");
	 if(verbose) fclose(fp);
	 return(TRUE);	
       }
       else{
	 /* don't free the first rules matrix since the rest of the */
	 /* program needs it */
	 if(!first){
     /*	   pot_free_structures(markkey, marking, firinglist, clocks, clockkey,
			       process, processkey, rule_info, nrules, nevents,
			       clock_size, process_size); */
	 }
	 first=0;
//	 markkey=current_work->markkey;
	 marking=current_work->marking;
	 firinglist=current_work->firinglist;
	 clocks=current_work->clocks;
	 clockkey=current_work->clockkey;
	 rule_info=current_work->rule_info;
	 processkey=current_work->processkey;
	 process=current_work->process;
	 num_clocks=current_work->num_clocks;
	 num_events=current_work->num_events;
	 clock_size=current_work->clock_size;
	 process_size=current_work->process_size;
	 /* 	puts("got more work, current clock matrix is:");  */
	 /* 	print_clocks(clocks, num_clocks); */
	 /*  	print_clockkey(num_clocks, clockkey); */
       }
     }
   }
  }





