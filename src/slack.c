/*****************************************************************************/
/* slack.c                                                                   */
/*****************************************************************************/

#include "slack.h"
#include "connect.h"

typedef struct slack_tag {
  int slack;
  int event;
  char * state;
  struct slack_tag *link;
} *slackADT;

bool untimed_enabled(eventADT *events,stateADT curstate,int e)
{
  if (((e % 2==1) && curstate->state[events[e]->signal]!='R') ||
      ((e % 2==0) && curstate->state[events[e]->signal]!='F'))
    return FALSE;
  return TRUE;
}

bool timed_enabled(eventADT *events,ruleADT **rules,stateADT curstate,
		   clocklistADT curclock,int e)
{
  int l;

  if ((((e % 2)==1) && curstate->state[events[e]->signal]!='R') ||
      (((e % 2)==0) && curstate->state[events[e]->signal]!='F'))
    return FALSE;
  for (l=1;l<=curclock->clocknum;l++)
    if (((curclock->clockkey[l].enabling)>=0) &&
	((curclock->clockkey[l].enabled)>=0)) 
      if ((curclock->clockkey[l].enabled==e) && 
	  (curclock->clocks[0][l] < rules[curclock->clockkey[l].enabling]
	   [curclock->clockkey[l].enabled]->lower))
	return FALSE;
  return TRUE;
}

bool always_ordered(eventADT *events,ruleADT **rules,stateADT curstate,
		    int i,int j,int e)
{
  clocklistADT curclock;
  int jc,ec;

  for (curclock=curstate->clocklist;curclock;curclock=curclock->next) {
    for (jc=0; jc<curclock->clocknum && curclock->clockkey[jc].enabled != j; 
	 jc++);
    if (curclock->clockkey[jc].enabled==j) {
      for (ec=0; ec<curclock->clocknum && curclock->clockkey[ec].enabled != e;
	   ec++);
      if (curclock->clockkey[ec].enabled==e) {
	if (curclock->clocks[0][jc] >= rules[i][j]->lower) return FALSE;
	if (curclock->clocks[0][ec] < 
	    rules[curclock->clockkey[ec].enabling][ec]->lower) return FALSE;
      }
    }
  }
  return TRUE;
}

/*****************************************************************************/
/* Delete space for a set of slacks.                                         */
/*****************************************************************************/

void delete_slacks(slackADT **slacks,int nevents)
{
  int i,j;
  slackADT tempslack;

  for (i=0;i<nevents;i++) {
    for (j=0;j<nevents;j++) 
      while(slacks[i][j]) {
	tempslack=slacks[i][j]->link;
	free(slacks[i][j]);
	slacks[i][j]=tempslack;
      }
    free(slacks[i]);
  }
  free(slacks);
}

/*****************************************************************************/
/* Create space to store a new set of slacks.                                */
/*****************************************************************************/

slackADT **new_llslacks(ruleADT **rules,int nevents)
{
  int i,j;
  slackADT **slacks=NULL;

  slacks=(slackADT**)GetBlock(nevents * sizeof(slacks[0]));
  for (i=0;i<nevents;i++) {
    slacks[i]=(slackADT *)GetBlock(nevents * sizeof(slacks[0][0]));
    for (j=0;j<nevents;j++) {
      slacks[i][j]=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      if (rules[i][j]->ruletype) {
	slacks[i][j]->slack=0; 
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      } else {
	slacks[i][j]->slack=(-1);
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      }
    }
  }
  return(slacks);
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

bool add_llslack(slackADT **slacks,int i,int j,int clock,int e,bool greater)
{
  slackADT tempslack=NULL;
  bool seen;

  if (slacks[i][j]->event==0) {
    slacks[i][j]->slack=clock;
    slacks[i][j]->event=e;
    return TRUE;
  } else {
    seen=FALSE;
    for (tempslack=slacks[i][j];tempslack;
	 tempslack=tempslack->link)
      if (tempslack->event==e) {
	seen=TRUE;
	if (((greater) && (clock > tempslack->slack)) ||
	    ((!greater) && (clock < tempslack->slack))) {
	  tempslack->slack=clock;
	  return TRUE;
	}
      }
    if (!seen) {
      tempslack=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      tempslack->slack=clock;
      tempslack->event=e;
      tempslack->link=slacks[i][j]->link;
      slacks[i][j]->link=tempslack;
      return TRUE;
    }
  }
  return FALSE;
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

void add_llslacks(ruleADT **rules,slackADT **slacks,clocklistADT curclock,
		 int i,int j,int clock,eventADT *events,stateADT curstate)
{
  int l,e;

  if (i>=0 && j>=0 && untimed_enabled(events,curstate,j))
    if (clock < rules[i][j]->lower) {
      for (l=1;l<=curclock->clocknum;l++)
	if (((curclock->clockkey[l].enabling)>=0) &&
	  ((curclock->clockkey[l].enabled)>=0)) {
	  e=curclock->clockkey[l].enabled;
	  if ((curclock->clocks[0][l] >= 
	       rules[curclock->clockkey[l].enabling][e]->lower) &&
	      timed_enabled(events,rules,curstate,curclock,e) &&
	      always_ordered(events,rules,curstate,i,j,e)) {
	    add_llslack(slacks,i,j,clock,e,TRUE);
/*	    printf("%s -> %s clock=%d e=%s\n",events[i]->event,
		   events[j]->event,clock,events[e]->event);*/
	  }
	}
    }
}

/*****************************************************************************/
/* Find slack on lower bound before a state is added.                        */
/*****************************************************************************/

slackADT **find_llslack(eventADT *events,ruleADT **rules,cycleADT ****cycles,
			stateADT *state_table,int nevents,int ncycles)
{
  int i,j,k;
  stateADT curstate;
  clocklistADT curclock;
  slackADT **slacks=NULL;
  slackADT tempslack=NULL;
  slackADT tempslack2=NULL;
  bool change;
/*  int s,e;*/

  slacks=new_llslacks(rules,nevents);

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      for (curclock=curstate->clocklist;curclock;curclock=curclock->next) 
	for (j=1;j<=curclock->clocknum;j++)
	  add_llslacks(rules,slacks,curclock,curclock->clockkey[j].enabling,
		      curclock->clockkey[j].enabled,curclock->clocks[0][j],
		      events,curstate);
    
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype)
	for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
	  if (tempslack->slack==0) 
	    tempslack->slack=rules[i][j]->lower;
	  else if ((rules[i][j]->lower-tempslack->slack) > 0)
	    tempslack->slack=rules[i][j]->lower-tempslack->slack-1; 

  do {
    change=FALSE;
    for (i=0;i<nevents;i++)
      for (j=0;j<nevents;j++)
	if (rules[i][j]->ruletype) {
	  for (k=0;k<nevents;k++)
	    if (rules[j][k]->ruletype) {
	      /*	      printf("%s -> %s -> %s\n",events[i]->event,
		     events[j]->event,events[k]->event);*/
	      for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
		for (tempslack2=slacks[j][k];tempslack2;
		     tempslack2=tempslack2->link) {
		  /*	  printf("%s -> %s clock=%d e=%s\n",events[i]->event,
			 events[j]->event,tempslack2->slack,
			 events[tempslack2->event]->event);*/
		  if ((tempslack->slack > tempslack2->slack) &&
		      (tempslack2->slack < rules[j][k]->lower) &&
		      (tempslack2->event > 0)) {  
		    if (reachable(cycles,nevents,ncycles,j,0,
				  tempslack2->event,0/*eps*/)!=1) {
		      if (add_llslack(slacks,i,j,tempslack2->slack,
				      tempslack2->event,FALSE)) 
			change=TRUE;
		    }
		  }
		}
	    }
	}
  } while (change);

  return slacks;
}

/*****************************************************************************/
/* Create space to store a new set of slacks.                                */
/*****************************************************************************/

slackADT **new_luslacks(ruleADT **rules,int nevents)
{
  int i,j;
  slackADT **slacks=NULL;

  slacks=(slackADT**)GetBlock(nevents * sizeof(slacks[0]));
  for (i=0;i<nevents;i++) {
    slacks[i]=(slackADT *)GetBlock(nevents * sizeof(slacks[0][0]));
    for (j=0;j<nevents;j++) {
      slacks[i][j]=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      if (rules[i][j]->ruletype) {
	slacks[i][j]->slack=INFIN; 
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      } else {
	slacks[i][j]->slack=(-1);
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      }
    }
  }
  return(slacks);
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

bool add_luslack(slackADT **slacks,int i,int j,int clock,int e,bool greater,
		 char * curstate)
{
  slackADT tempslack=NULL;
  bool seen;

  if (slacks[i][j]->event==0) {
    slacks[i][j]->slack=clock;
    slacks[i][j]->event=e;
    slacks[i][j]->state=CopyString(curstate);
    return TRUE;
  } else {
    seen=FALSE;
    for (tempslack=slacks[i][j];tempslack;
	 tempslack=tempslack->link)
      if (tempslack->event==e) {
	seen=TRUE;
	if (((greater) && (clock > tempslack->slack)) ||
	    ((!greater) && (clock < tempslack->slack))) {
	  tempslack->slack=clock;
	  if (tempslack->state)
	    strcpy(tempslack->state,curstate);
	  else
	    tempslack->state=CopyString(curstate);
	  return TRUE;
	}
      }
    if (!seen) {
      tempslack=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      tempslack->slack=clock;
      tempslack->event=e;
      tempslack->state=CopyString(curstate);
      tempslack->link=slacks[i][j]->link;
      slacks[i][j]->link=tempslack;
      return TRUE;
    }
  }
  return FALSE;
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

void add_luslacks(ruleADT **rules,slackADT **slacks,clocklistADT curclock,
		  int i,int j,int clock,eventADT *events,stateADT curstate,
		  int clk)
{
  int l,e,slk;
  char * tempstate;

  if (i>=0 && j>=0 && timed_enabled(events,rules,curstate,curclock,j))
    if (clock >= rules[i][j]->lower) {
      for (l=1;l<=curclock->clocknum;l++)
	if (((curclock->clockkey[l].enabling)>=0) &&
	    ((curclock->clockkey[l].enabled)>=0))
	  /*	  if (untimed_enabled(events,curstate,
			      curclock->clockkey[l].enabled)) {*/
	  if (timed_enabled(events,rules,curstate,curclock,
			    curclock->clockkey[l].enabled) &&
	      (!rules[j][curclock->clockkey[l].enabled]->conflict)) {
	    e=curclock->clockkey[l].enabled;
	    if (e != j) {
	      if (rules[curclock->clockkey[l].enabling]
		  [curclock->clockkey[l].enabled]->upper==INFIN)
		slk=INFIN;
	      else if (curclock->clocks[l][clk]==INFIN)
		slk=INFIN;
	      else
		slk=rules[curclock->clockkey[l].enabling]
		  [curclock->clockkey[l].enabled]->upper+
		  curclock->clocks[l][clk]-rules[i][j]->lower;
	      /*
	      printf("i=%s j=%s clock=%d lower=%d e=%s f=%s curclock=%d %d\n",
		     events[i]->event,events[j]->event,clock,
		     rules[i][j]->lower,
		     events[curclock->clockkey[l].enabling]->event,
		     events[curclock->clockkey[l].enabled]->event,
		     curclock->clocks[0][l],slk);
		     */		     
	      tempstate=CopyString(curstate->state);
	      if (tempstate)
		if (tempstate[events[j]->signal]=='R')
		  tempstate[events[j]->signal]='1';
		else
		  tempstate[events[j]->signal]='0';
	      add_luslack(slacks,i,j,slk,e,FALSE,tempstate);
	      free(tempstate);
	    }
	  }
    }
}

/*****************************************************************************/
/* Find slack on lower bound before a state is removed.                      */
/*****************************************************************************/

slackADT **find_luslack(eventADT *events,ruleADT **rules,cycleADT ****cycles,
			stateADT *state_table,int nevents,int ncycles)
{
  int i,j,k;
  stateADT curstate;
  clocklistADT curclock;
  slackADT **slacks=NULL;
  slackADT tempslack=NULL;
  slackADT tempslack2=NULL;
  bool change;
/*  int s,e;*/

  slacks=new_luslacks(rules,nevents);

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      for (curclock=curstate->clocklist;curclock;curclock=curclock->next) 
	for (j=1;j<=curclock->clocknum;j++)
	  add_luslacks(rules,slacks,curclock,curclock->clockkey[j].enabling,
		      curclock->clockkey[j].enabled,curclock->clocks[0][j],
		      events,curstate,j);
  /*    
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype)
	for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
	  if (tempslack->slack==0) 
	    tempslack->slack=rules[i][j]->lower;
	  else if ((rules[i][j]->lower-tempslack->slack) > 0)
	    tempslack->slack=rules[i][j]->lower-tempslack->slack-1; 
 */
  do {
    change=FALSE;
    for (i=0;i<nevents;i++)
      for (j=0;j<nevents;j++)
	if (rules[i][j]->ruletype) {
	  for (k=0;k<nevents;k++)
	    if (rules[j][k]->ruletype)
	      for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
		for (tempslack2=slacks[j][k];tempslack2;
		     tempslack2=tempslack2->link)
		  if ((tempslack->slack > tempslack2->slack) &&
		      /* (tempslack2->slack < rules[j][k]->lower) && */
		      (tempslack2->event > 0)) {  
		    if (reachable(cycles,nevents,ncycles,j,0,
				  tempslack2->event,0/*eps*/)!=1) {
		      if (add_luslack(slacks,i,j,tempslack2->slack,
				      tempslack2->event,FALSE,
				      tempslack2->state)) 
			change=TRUE;
		    }
		  }
	}
  } while (change);
  /*
  printf("SLACK IN INCREASING LOWER BOUNDS\n");
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype) {
	s=INFIN; e=0;
	for (tempslack=slacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < s) {
	    s=tempslack->slack;
	    e=tempslack->event;
	  }
	printf("<%s, %s, %d, %d> slack = %d event = %s\n",events[i]->event,
	       events[j]->event,rules[i][j]->lower,rules[i][j]->upper,
	       s,events[e]->event);
      }
      */
  return slacks;
}

/*****************************************************************************/
/* Create space to store a new set of slacks.                                */
/*****************************************************************************/

slackADT **new_ulslacks(ruleADT **rules,int nevents)
{
  int i,j;
  slackADT **slacks=NULL;

  slacks=(slackADT**)GetBlock(nevents * sizeof(slacks[0]));
  for (i=0;i<nevents;i++) {
    slacks[i]=(slackADT *)GetBlock(nevents * sizeof(slacks[0][0]));
    for (j=0;j<nevents;j++) {
      slacks[i][j]=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      if (rules[i][j]->ruletype) {
	slacks[i][j]->slack=INFIN;
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      } else {
	slacks[i][j]->slack=(-1);
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      }
    }
  }
  return(slacks);
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

bool add_ulslack(slackADT **slacks,int i,int j,int clock,int e,bool greater,
		 char * curstate)
{
  slackADT tempslack=NULL;
  bool seen;

  if (slacks[i][j]->event==0) {
    slacks[i][j]->slack=clock;
    slacks[i][j]->event=e;
    slacks[i][j]->state=CopyString(curstate);
    return TRUE;
  } else {
    seen=FALSE;
    for (tempslack=slacks[i][j];tempslack;
	 tempslack=tempslack->link)
      if (tempslack->event==e) {
	seen=TRUE;
	if (((greater) && (clock > tempslack->slack)) ||
	    ((!greater) && (clock < tempslack->slack))) {
	  tempslack->slack=clock;
	  if (tempslack->state)
	    strcpy(tempslack->state,curstate);
	  else
	    tempslack->state=CopyString(curstate);
	  return TRUE;
	}
      }
    if (!seen) {
      tempslack=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      tempslack->slack=clock;
      tempslack->event=e;
      tempslack->state=CopyString(curstate);
      tempslack->link=slacks[i][j]->link;
      slacks[i][j]->link=tempslack;
      return TRUE;
    }
  }
  return FALSE;
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

void add_ulslacks(ruleADT **rules,slackADT **slacks,clocklistADT curclock,
		  int i,int j,int clock,eventADT *events,stateADT curstate,
		  int clk)
{
  int l,e,slk;
  char * tempstate;

  if (i>=0 && j>=0 && timed_enabled(events,rules,curstate,curclock,j))
    if (clock >= rules[i][j]->lower) {
      for (l=1;l<=curclock->clocknum;l++)
	if (((curclock->clockkey[l].enabling)>=0) &&
	    ((curclock->clockkey[l].enabled)>=0))
	  /*	  if (untimed_enabled(events,curstate,
			      curclock->clockkey[l].enabled)) {*/
	  if (timed_enabled(events,rules,curstate,curclock,
			    curclock->clockkey[l].enabled) &&
	      (!rules[j][curclock->clockkey[l].enabled]->conflict)) {
	    e=curclock->clockkey[l].enabled;
	    if (e != j) {
	      if (rules[curclock->clockkey[l].enabling]
		  [curclock->clockkey[l].enabled]->lower==0)
		slk=rules[i][j]->upper;
	      /*	      else if (curclock->clocks[clk][l]==0)
		slk=rules[i][j]->upper;*/
	      else
		slk=rules[i][j]->upper-(rules[curclock->clockkey[l].enabling]
		  [curclock->clockkey[l].enabled]->lower+(-1)*
		  curclock->clocks[clk][l]);
	      if ((rules[i][j]->upper-slk)<0) slk=rules[i][j]->upper;
	      /*
	      printf("i=%s j=%s clock=%d lower=%d e=%s f=%s curclock=%d %d\n",
		     events[i]->event,events[j]->event,clock,
		     rules[i][j]->lower,
		     events[curclock->clockkey[l].enabling]->event,
		     events[curclock->clockkey[l].enabled]->event,
		     curclock->clocks[0][l],slk);
		     */
	      tempstate=CopyString(curstate->state);
	      if (tempstate)
		if (tempstate[events[e]->signal]=='R')
		  tempstate[events[e]->signal]='1';
		else
		  tempstate[events[e]->signal]='0';
	      add_ulslack(slacks,i,j,slk,e,FALSE,tempstate);
	      free(tempstate);
	    }
	  }
    }
}

/*****************************************************************************/
/* Find slack on lower bound before a state is removed.                      */
/*****************************************************************************/

slackADT **find_ulslack(eventADT *events,ruleADT **rules,cycleADT ****cycles,
			stateADT *state_table,int nevents,int ncycles)
{
  int i,j,k;
  stateADT curstate;
  clocklistADT curclock;
  slackADT **slacks=NULL;
  slackADT tempslack=NULL;
  slackADT tempslack2=NULL;
  bool change;
/*  int s,e;*/

  slacks=new_ulslacks(rules,nevents);

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      for (curclock=curstate->clocklist;curclock;curclock=curclock->next) 
	for (j=1;j<=curclock->clocknum;j++)
	  add_ulslacks(rules,slacks,curclock,curclock->clockkey[j].enabling,
		       curclock->clockkey[j].enabled,curclock->clocks[0][j],
		       events,curstate,j);
  /*    
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype)
	for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
	  if (tempslack->slack==0) 
	    tempslack->slack=rules[i][j]->lower;
	  else if ((rules[i][j]->lower-tempslack->slack) > 0)
	    tempslack->slack=rules[i][j]->lower-tempslack->slack-1; 
 */
  do {
    change=FALSE;
    for (i=0;i<nevents;i++)
      for (j=0;j<nevents;j++)
	if (rules[i][j]->ruletype) {
	  for (k=0;k<nevents;k++)
	    if (rules[j][k]->ruletype)
	      for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
		for (tempslack2=slacks[j][k];tempslack2;
		     tempslack2=tempslack2->link)
		  if ((tempslack->slack > tempslack2->slack) &&
		  /* (tempslack2->slack < rules[j][k]->lower) && */
		      (tempslack2->event > 0)) {  
		    if (reachable(cycles,nevents,ncycles,j,0,
				  tempslack2->event,0/*eps*/)!=1) {
		      if (add_ulslack(slacks,i,j,tempslack2->slack,
				      tempslack2->event,TRUE,
				      tempslack2->state)) 
			change=TRUE;
		    }
		  }
	}
  } while (change);

  /*
  printf("SLACK IN INCREASING LOWER BOUNDS\n");
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype) {
	s=INFIN; e=0;
	for (tempslack=slacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < s) {
	    s=tempslack->slack;
	    e=tempslack->event;
	  }
	printf("<%s, %s, %d, %d> slack = %d event = %s\n",events[i]->event,
	       events[j]->event,rules[i][j]->lower,rules[i][j]->upper,
	       s,events[e]->event);
      }
      */
  return slacks;
}

/*****************************************************************************/
/* Create space to store a new set of slacks.                                */
/*****************************************************************************/

slackADT **new_uuslacks(ruleADT **rules,int nevents)
{
  int i,j;
  slackADT **slacks=NULL;

  slacks=(slackADT**)GetBlock(nevents * sizeof(slacks[0]));
  for (i=0;i<nevents;i++) {
    slacks[i]=(slackADT *)GetBlock(nevents * sizeof(slacks[0][0]));
    for (j=0;j<nevents;j++) {
      slacks[i][j]=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      if (rules[i][j]->ruletype) {
	slacks[i][j]->slack=INFIN;
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      } else {
	slacks[i][j]->slack=(-1);
	slacks[i][j]->event=0;
	slacks[i][j]->link=NULL;
      }
    }
  }
  return(slacks);
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

bool add_uuslack(slackADT **slacks,int i,int j,int clock,int e,bool greater)
{
  slackADT tempslack=NULL;
  bool seen;

  if (slacks[i][j]->event==0) {
    slacks[i][j]->slack=clock;
    slacks[i][j]->event=e;
    return TRUE;
  } else {
    seen=FALSE;
    for (tempslack=slacks[i][j];tempslack;
	 tempslack=tempslack->link)
      if (tempslack->event==e) {
	seen=TRUE;
	if (((greater) && (clock > tempslack->slack)) ||
	    ((!greater) && (clock < tempslack->slack))) {
	  tempslack->slack=clock;
	  return TRUE;
	}
      }
    if (!seen) {
      tempslack=(slackADT)GetBlock(sizeof(*slacks[0][0]));
      tempslack->slack=clock;
      tempslack->event=e;
      tempslack->link=slacks[i][j]->link;
      slacks[i][j]->link=tempslack;
      return TRUE;
    }
  }
  return FALSE;
}

/*****************************************************************************/
/* Add a slack to the slack list for a rule.                                 */
/*****************************************************************************/

void add_uuslacks(ruleADT **rules,slackADT **slacks,clocklistADT curclock,
		 int i,int j,int clock,eventADT *events)
{
  int l;

  if (i>=0 && j>=0) 
    if (clock < rules[i][j]->lower) {
      for (l=1;l<=curclock->clocknum;l++)
	if (((curclock->clockkey[l].enabling)>=0) &&
	    ((curclock->clockkey[l].enabled)>=0)) 
	  if (curclock->clocks[0][l] >= 
	      rules[curclock->clockkey[l].enabling]
	      [curclock->clockkey[l].enabled]->lower) {
	    add_uuslack(slacks,curclock->clockkey[l].enabling,
			curclock->clockkey[l].enabled,
			rules[i][j]->lower-clock-1,j,FALSE);
	  }
    }
}

/*****************************************************************************/
/* Find rising and falling regions for state variable.                       */
/*****************************************************************************/

slackADT **find_uuslack(eventADT *events,ruleADT **rules,cycleADT ****cycles,
			stateADT *state_table,int nevents,int ncycles)
{
  int i,j,k;
  stateADT curstate;
  clocklistADT curclock;
  slackADT **slacks=NULL;
  slackADT tempslack=NULL;
  slackADT tempslack2=NULL;
  bool change;
/*  int s,e;*/

  slacks=new_uuslacks(rules,nevents);

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      for (curclock=curstate->clocklist;curclock;curclock=curclock->next) 
	for (j=1;j<=curclock->clocknum;j++)
	  add_uuslacks(rules,slacks,curclock,curclock->clockkey[j].enabling,
		       curclock->clockkey[j].enabled,curclock->clocks[0][j],
		       events);
  do {
    change=FALSE;
    for (i=0;i<nevents;i++)
      for (j=0;j<nevents;j++)
	if (rules[i][j]->ruletype) {
	  for (k=0;k<nevents;k++)
	    if (rules[j][k]->ruletype)
	      for (tempslack=slacks[i][j];tempslack;tempslack=tempslack->link)
		for (tempslack2=slacks[j][k];tempslack2;
		     tempslack2=tempslack2->link)
		  if ((tempslack->slack > tempslack2->slack)/* &&
		      (tempslack2->slack < rules[j][k]->lower) &&
		      (tempslack2->event > 0)*/) {  
		    if (reachable(cycles,nevents,ncycles,j,0,
				  tempslack2->event,0/*eps*/)!=1) {
		      if (add_uuslack(slacks,i,j,tempslack2->slack,
				     tempslack2->event,FALSE)) 
			change=TRUE;
		    }
		  }
	}
  } while (change);

  return slacks;
  /*
  printf("SLACK IN RAISING UPPER BOUNDS\n");
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype) {
	s=INFIN; e=0;
	for (tempslack=slacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < s) {
	    s=tempslack->slack;
	    e=tempslack->event;
	  }
	if (s==INFIN)
	  printf("<%s, %s, %d, %d> slack = inf event = %s\n",events[i]->event,
		 events[j]->event,rules[i][j]->lower,rules[i][j]->upper,
		 events[e]->event);
	else
	  printf("<%s, %s, %d, %d> slack = %d event = %s\n",events[i]->event,
		 events[j]->event,rules[i][j]->lower,rules[i][j]->upper,
		 s,events[e]->event);
      }
      */
}

/*****************************************************************************/
/* Print slack.                                                              */
/*****************************************************************************/

void print_slacks(FILE *fp,eventADT *events,ruleADT **rules,
		  slackADT **llslacks,slackADT **luslacks,slackADT **ulslacks,
		  slackADT **uuslacks,int nevents)
{
  int i,j;
  slackADT tempslack;
  int lls,lle,lus,lue,uls,ule,uus,uue;
  char * lstate=NULL;
  char * ustate=NULL;

  for (j=0;j<nevents;j++)
    for (i=0;i<nevents;i++)
      if (rules[i][j]->ruletype) {
	lls=INFIN; lle=0;
	for (tempslack=llslacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < lls) {
	    lls=tempslack->slack;
	    lle=tempslack->event;
	  }
	lus=INFIN; lue=0;
	for (tempslack=luslacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < lus) {
	    lus=tempslack->slack;
	    lue=tempslack->event;
	    lstate=tempslack->state;
	  }
	uls=INFIN; ule=0;
	for (tempslack=ulslacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < uls) {
	    uls=tempslack->slack;
	    ule=tempslack->event;
	    ustate=tempslack->state;
	  }
	uus=INFIN; uue=0;
	for (tempslack=uuslacks[i][j]; tempslack; tempslack=tempslack->link) 
	  if (tempslack->slack < uus) {
	    uus=tempslack->slack;
	    uue=tempslack->event;
	  }
	fprintf(fp,"<%s, %s, (%d-",events[i]->event,events[j]->event,
		rules[i][j]->lower-lls);
	if (lus==INFIN) fprintf(fp,"inf), (");
	else fprintf(fp,"%d), (",rules[i][j]->lower+lus);
	if ((rules[i][j]->upper-uls)<=0) fprintf(fp,"0-"); 
	else fprintf(fp,"%d-",rules[i][j]->upper-uls);
	if ((rules[i][j]->upper==INFIN) || (uus==INFIN)) 
	  fprintf(fp,"inf)>"); 
	else fprintf(fp,"%d)>",rules[i][j]->upper+uus);
	if (lstate) fprintf(fp," \t%s",lstate);
	else fprintf(fp,"\t");
	if (ustate) fprintf(fp," \t%s\n",ustate);
	else fprintf(fp,"\n");
      }
}

/*****************************************************************************/
/* Find slack on rules.                                                      */
/*****************************************************************************/

void slack_chk(char * filename,signalADT *signals,eventADT *events,
	       mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	       markkeyADT *markkey,stateADT *state_table,
	       int nevents,int ncycles,int ninputs,int initnrules,int maxstack,
	       int maxgatedelay,char * startstate,bool initial,
	       regionADT *lastregions,int status,regionADT *regions,
	       bool manual,int ninpsig,int nsignals,bool verbose)
{
  slackADT **llslacks;
  slackADT **luslacks;
  slackADT **ulslacks;
  slackADT **uuslacks;
  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,filename);
  strcat(outname,".slk");
  printf("Finding slacks in timing constraints and storing to:  %s\n",outname);
  fprintf(lg,"Finding slacks in timing constraints and storing to:  %s\n",
	  outname);
  fp=Fopen(outname,"w");
  llslacks=find_llslack(events,rules,cycles,state_table,nevents,ncycles);
  luslacks=find_luslack(events,rules,cycles,state_table,nevents,ncycles);
  ulslacks=find_ulslack(events,rules,cycles,state_table,nevents,ncycles);
  uuslacks=find_uuslack(events,rules,cycles,state_table,nevents,ncycles);
  print_slacks(fp,events,rules,llslacks,luslacks,ulslacks,uuslacks,nevents);
  delete_slacks(llslacks,nevents);
  delete_slacks(luslacks,nevents);
  delete_slacks(ulslacks,nevents);
  delete_slacks(uuslacks,nevents);
  fclose(fp);
}

