/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1996 by, Chris J. Myers                                   */
/*   Electrical Engineering Department                                       */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Univeristy of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL U. OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE LIABLE  */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* project.c                                                                 */
/*****************************************************************************/

#include "project.h"
#include "findreg.h"
#include "findrsg.h"
#include "printrsg.h"
#include "common_timing.h"
#include "memaloc.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* Determine if two states are equal.                                        */
/*****************************************************************************/

bool statecmp(signalADT *signals,eventADT *events,stateADT state1,
	      stateADT state2,int nsignals)
{
  int i;
  /*
  statelistADT predstate,predstate2;
  statelistADT nextstate,nextstate2;
  */

  if ((!(state1->state) || !(state2->state))) return FALSE;
  for (i=0;i<nsignals;i++)
    if (!(events[signals[i]->event]->type & DUMMY)) 
      if (VAL(state1->state[i])!=VAL(state2->state[i])) return FALSE;

  /* forall (s',s2) in gamma . (s'=s1) or (s',s1) in gamma */  
  /*  
  for (predstate=state2->pred;predstate;predstate=predstate->next) {
    if (predstate->stateptr!=state1) {
      for (predstate2=state1->pred;predstate2;predstate2=predstate2->next) 
	if (predstate->stateptr==predstate2->stateptr) {
	  break;
	}
      if (!predstate2) {
	break;
      }
    }
  }
  if (!predstate) {
    return TRUE;
  }
  */

  /* forall (s1,s') in gamma . (s'=s2) or (s2,s') in gamma */  
  /*
  for (nextstate=state1->succ;nextstate;nextstate=nextstate->next) {
    if (nextstate->stateptr!=state2) {
      for (nextstate2=state2->succ;nextstate2;nextstate2=nextstate2->next) 
	if (nextstate->stateptr==nextstate2->stateptr) {
	  break;
	}
      if (!nextstate2) {
	return FALSE;
      }
    }
  }
  */
  return TRUE;
}

/*****************************************************************************/
/* Merge enablings of two states before projection.                          */
/*****************************************************************************/

void merge_enablings(char * state1,char * state2,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if ((state2[i]=='R') || (state2[i]=='F'))
      state1[i]=state2[i];
}

/*****************************************************************************/
/* Check if state is in a state list.                                        */
/*****************************************************************************/

bool in_state_list(statelistADT curlist,stateADT curstate,int event)
{
  statelistADT templist;

  for (templist=curlist;templist->next;templist=templist->next) {
    if ((templist->stateptr==curstate) && (templist->enabled==event)) {
      return TRUE;
    }
  }
  if (templist->stateptr==curstate) return TRUE;
  return FALSE;
}

/*****************************************************************************/
/* Remove successor links.                                                   */
/*****************************************************************************/

void rm_succ_links(stateADT nextstate,stateADT curstate,
		   stateADT *state_table,eventADT *events)
{
  statelistADT tempstate,tempstate2,last;
  bool insert;

  for (tempstate=nextstate->pred;tempstate;
       tempstate=tempstate->next) { 
    if (tempstate->stateptr != curstate) {
      insert=TRUE;
      for (tempstate2=tempstate->stateptr->succ;tempstate2;
	   tempstate2=tempstate2->next) {
	if ((tempstate2->stateptr==curstate) && 
	    (tempstate2->enabled==tempstate->enabled)) { 
	  insert=FALSE;
	  break;
	}
      }
      if (insert)
	for (tempstate2=tempstate->stateptr->succ;tempstate2;
	     tempstate2=tempstate2->next) {
	  if (tempstate2->stateptr==nextstate) { 
	    tempstate2->stateptr=curstate;
	  }
	}
    } 
  }
  if (curstate->succ) {
    for (tempstate=nextstate->succ;tempstate;
	 tempstate=tempstate->next) {
      insert=TRUE;
      for (tempstate2=curstate->succ;tempstate2->next;
	   tempstate2=tempstate2->next) {
	  if ((tempstate2->stateptr==tempstate->stateptr)&&
	      (tempstate2->enabled==tempstate->enabled)) {
	    insert=FALSE;
	    break;
	  }
      }
      if (((tempstate2->stateptr==tempstate->stateptr)&&
	   (tempstate2->enabled==tempstate->enabled))) insert=FALSE;
      if ((insert) && 
	  ((tempstate->stateptr!=curstate)||
	   ((tempstate->iteration==FIRE_TRANS) &&
	    (events[tempstate->enabled]->type & KEEP)))) {
	tempstate2->next=(statelistADT)GetBlock(sizeof (*(curstate->succ)));
	init( tempstate2->next );
	tempstate2->next->stateptr=tempstate->stateptr;
	tempstate2->next->enabling=tempstate->enabling;
	tempstate2->next->enabled=tempstate->enabled;
	tempstate2->next->iteration=tempstate->iteration;
	tempstate2->next->next=NULL;
      }
    }
  }
  /* ??? I DON'T THINK THIS IS NECESSARY
  else curstate->succ=nextstate->succ; */
  last=NULL;
  for (tempstate=curstate->succ;tempstate;tempstate=tempstate->next) {
    if (tempstate->stateptr==nextstate) { 
      if (tempstate->next) {
	tempstate->stateptr=tempstate->next->stateptr;
	tempstate->enabling=tempstate->next->enabling;
	tempstate->enabled=tempstate->next->enabled;
	tempstate->iteration=tempstate->next->iteration;
	tempstate2=tempstate->next;
	tempstate->next=tempstate->next->next;
	free(tempstate2);
      } else {
	if (last) {
	  last->next=NULL;
	} else { 
	  curstate->succ=NULL;
	}
      }
      break;
    }
    last=tempstate;
  }
}

/*****************************************************************************/
/* Remove successor links.                                                   */
/*****************************************************************************/

void mv_succ_links(stateADT nextstate,stateADT curstate)
{
  statelistADT tempstate,tempstate2;

  for (tempstate=nextstate->pred;tempstate;
       tempstate=tempstate->next) { 
    for (tempstate2=tempstate->stateptr->succ;tempstate2;
	 tempstate2=tempstate2->next) {
      if (tempstate2->stateptr==nextstate) { 
	tempstate2->stateptr=curstate;
      }
    }
  }
}

/*****************************************************************************/
/* Remove predecessor links.                                                 */
/*****************************************************************************/

void rm_pred_links(stateADT nextstate,stateADT curstate,eventADT *events)
{
  statelistADT tempstate,tempstate2,last,next;
  bool insert;

  for (tempstate=nextstate->succ;tempstate;
       tempstate=tempstate->next) { 
    last=NULL;
    next=NULL;
    for (tempstate2=tempstate->stateptr->pred;tempstate2;
	 tempstate2=next) {
      if (tempstate2->stateptr==nextstate) { 
	if ((tempstate->stateptr != curstate) && 
	    (!in_state_list(tempstate->stateptr->pred,curstate,
			    tempstate2->enabled))) {
	  tempstate2->stateptr=curstate;
	  last=tempstate2;
	  if (tempstate2) next=tempstate2->next;
	  else next=NULL;
	} else {
	  if (last) {
	    last->next=tempstate2->next;
	    free(tempstate2);
	  } else {
	    tempstate->stateptr->pred=tempstate2->next;
	    free(tempstate2);
	  }
	  if (last) next=last->next;
	  else next=NULL;
	}
      } else {
	last=tempstate2;
	if (tempstate2) next=tempstate2->next;
	else next=NULL;
      }
    }
  }

  if (curstate->pred) {
    for (tempstate=nextstate->pred;tempstate;
	 tempstate=tempstate->next) {
      insert=TRUE;
      for (tempstate2=curstate->pred;tempstate2->next;
	   tempstate2=tempstate2->next) {
	  if (((tempstate2->stateptr==tempstate->stateptr) &&
	       (tempstate2->enabled==tempstate->enabled))) {
	    insert=FALSE;
	    break;
	  }
      }
      if (((tempstate2->stateptr==tempstate->stateptr) &&
	   (tempstate2->enabled==tempstate->enabled))) insert=FALSE;
      if ((insert) &&
	  ((tempstate->stateptr!=curstate)||
	   ((tempstate->iteration==FIRE_TRANS) &&
	    (events[tempstate->enabled]->type & KEEP)))) {
	tempstate2->next=(statelistADT)GetBlock(sizeof (*(curstate->pred)));
	init( tempstate2->next );
	tempstate2->next->stateptr=tempstate->stateptr;
	tempstate2->next->enabling=tempstate->enabling;
	tempstate2->next->enabled=tempstate->enabled;
	tempstate2->next->iteration=tempstate->iteration;
	tempstate2->next->next=NULL;
      }
    }
  } 
  /* ??? I DON'T THINK THIS IS NECESSARY
  else curstate->pred=nextstate->pred; */
}

/*****************************************************************************/
/* Remove predecessor links.                                                 */
/*****************************************************************************/

void mv_pred_links(stateADT nextstate,stateADT curstate)
{
  statelistADT tempstate,tempstate2;

  for (tempstate=nextstate->succ;tempstate;
       tempstate=tempstate->next) { 
    for (tempstate2=tempstate->stateptr->pred;tempstate2;
	 tempstate2=tempstate2->next) {
      if (tempstate2->stateptr==nextstate) { 
	  tempstate2->stateptr=curstate;
      } 
    }
  }
}

/*****************************************************************************/
/* Remove from predecessors.                                                 */
/*****************************************************************************/
void rm_from_pred(stateADT prevstate,stateADT curstate,int event)
{
  statelistADT predstate,tmpstate;

  tmpstate=NULL;
  predstate=prevstate->pred;
  while (predstate) {
    if ((predstate->stateptr==curstate)&&(predstate->enabled==event)) {
      if (tmpstate) {
	tmpstate->next=predstate->next;
	free(predstate);
	predstate=tmpstate->next;
      } else {
	prevstate->pred=predstate->next;
	free(predstate);
	predstate=prevstate->pred;
      }
    } else {
      tmpstate=predstate;
      predstate=predstate->next;
    }
  }
}

/*****************************************************************************/
/* Remove nondeterminism.                                                    */
/*****************************************************************************/
void rm_nondeterminism(stateADT *state_table)
{
  int k;
  stateADT curstate;
  statelistADT prevstate,nextstate,nextstate2;

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      prevstate=NULL;
      nextstate=curstate->succ;
      while (nextstate) {
	for (nextstate2=curstate->succ;nextstate2;nextstate2=nextstate2->next)
	  if ((nextstate2->stateptr->color >=0) && (nextstate!=nextstate2) &&
	      (nextstate->enabled==nextstate2->enabled) &&
	      (strcmp(nextstate2->stateptr->state,"FAIL")==0)) {
	    if (prevstate) {
	      rm_from_pred(nextstate->stateptr,curstate,nextstate->enabled);
	      prevstate->next=nextstate->next;
	      free(nextstate);
	      nextstate=prevstate->next;
	    } else {
	      rm_from_pred(nextstate->stateptr,curstate,nextstate->enabled);
	      curstate->succ=nextstate->next;
	      free(nextstate);
	      nextstate=curstate->succ;
	    }
	    break;
	  }
	if (!nextstate2) {
	  prevstate=nextstate;
	  nextstate=nextstate->next;
	}
      }
    }
}

/*****************************************************************************/
/* Remove nondeterminism.                                                    */
/*****************************************************************************/
void rm_fail_succs(stateADT *state_table)
{
  int k;
  stateADT curstate;
  statelistADT prevstate,nextstate;

  for (k=0;k<PRIME;k++)
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) 
      if (strcmp(curstate->state,"FAIL")==0) {
	nextstate=curstate->succ;
	while (nextstate) {
	  rm_from_pred(nextstate->stateptr,curstate,nextstate->enabled);
	  prevstate=nextstate->next;
	  free(nextstate);
	  nextstate=prevstate;
	}
	curstate->succ=NULL;
	break;
      }
}

/*****************************************************************************/
/* Remove nondeterminism.                                                    */
/*****************************************************************************/
void rm_states_no_preds(stateADT *state_table)
{
  int k;
  stateADT curstate;
  statelistADT succs;

  for (k=0;k<PRIME;k++)
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) 
      if (!curstate->pred) {
	curstate->color=(-1);
	for (succs=curstate->succ;succs;succs=succs->next)
	  rm_from_pred(succs->stateptr,curstate,succs->enabled);
      }
}

/*****************************************************************************/
/* Project out dummy transitions in the state graph.                         */
/*****************************************************************************/
bool project(char * filename,signalADT *signals,stateADT *state_table,
	     int ninpsig,int *nsignals,bool verbose,eventADT *events,
	     int nevents,char *startstate,int nineqs)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int k;
  stateADT curstate,last,nextlink;
  statelistADT nextstate,next,old;
  double total;
  bool done;
  clocklistADT clockptr;
  clocklistADT next_clockptr;
  timeval t0,t1;
  double time0, time1;

  (*nsignals)=(*nsignals)-nineqs;
  gettimeofday(&t0, NULL);
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".rsg");
    printf("Projecting out dummy transitions and storing to:  %s\n",outname);
    fprintf(lg,"Projecting out dummy transitions and storing to:  %s\n",
	    outname);
    fp=Fopen(outname,"w");
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,*nsignals,ninpsig);
    fprintf(fp,"STATES:\n");
  } else {
    printf("Projecting out dummy transitions ... ");
    fflush(stdout);
    fprintf(lg,"Projecting out dummy transitions ... ");
    fflush(lg);
  }
  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      curstate->color=1;
    }

  do {
    done=TRUE;
    for (k=0;k<PRIME;k++) 
      for (curstate=state_table[k];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	if (curstate->color >= 0) {
	  nextstate=curstate->succ;
	  while (nextstate) {
	    if ((curstate!=nextstate->stateptr) &&
		(statecmp(signals,events,curstate,nextstate->stateptr,
			  *nsignals)) &&
		 (nextstate->stateptr->color > 0) &&
		(!((nextstate->enabled>=0) && 
		   (nextstate->iteration==FIRE_TRANS) &&
		   (events[nextstate->enabled]->type & KEEP) &&
		   (!(events[nextstate->enabled]->type & ABSTRAC))))) {
	      done=FALSE;
	      if (strcmp(nextstate->stateptr->state,"FAIL")==0) {
		free(curstate->state);
		curstate->state=CopyString("FAIL");
	      } else if (strcmp(curstate->state,"FAIL")!=0)
		merge_enablings(curstate->state,nextstate->stateptr->state,
				*nsignals);
	      if (nextstate->stateptr->number==0)
		curstate->number=0;
	    //	    curstate->color++;
	      nextstate->stateptr->color=(-1);
	    //	    curstate->probability=curstate->probability+
	    //  nextstate->stateptr->probability;

	      rm_pred_links(nextstate->stateptr,curstate,events);
	      rm_succ_links(nextstate->stateptr,curstate,state_table,events);

	      //nextstate->stateptr->pred=NULL;
	      //nextstate->stateptr->succ=NULL;
	      nextstate=curstate->succ;
	    /* Free memory used by this state */
	    } else {
	      /*
	      if ((((nextstate->enabled>=0) && 
		   (events[nextstate->enabled]->type & KEEP) &&
		   (!(events[nextstate->enabled]->type & ABSTRAC)) &&
		   (nextstate->iteration==FIRE_TRANS)))) {
		printf("Keeping %s-%s(%d)%c->%s\n",
		       curstate->state,events[nextstate->enabled]->event,
		       events[nextstate->enabled]->type,
		       nextstate->iteration,nextstate->stateptr->state);
		       }*/
	      nextstate=nextstate->next;
	    }
	  }
	}
      }
/*
    print_rsg(filename,signals,events,state_table,nsignals,TRUE,FALSE,
	      ???,FALSE); */
  } while (!done);

  //rm_nondeterminism(state_table);
  //rm_fail_succs(state_table);
  //rm_states_no_preds(state_table);

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      nextstate=curstate->succ;
      old=NULL;
      while (nextstate) {
	if (nextstate->stateptr->color < 0) {
	  /* printf("%s -> %s %d\n",curstate->state,nextstate->stateptr->state,
	     nextstate->stateptr->color);*/
	  if (!old) 
	    curstate->succ=nextstate->next;
	  else
	    old->next=nextstate->next;
	  if (nextstate->next)
	    nextstate=nextstate->next;
	  else
	    nextstate=NULL;
	} else {
	  old=nextstate;
	  nextstate=nextstate->next;
	}
      }
    }

  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      nextstate=curstate->succ;
      while (nextstate) {
	for (int i=0; i < strlen(curstate->state); i++) {
	  if (curstate->state[i]=='0' && 
	      (nextstate->stateptr->state[i]=='1' || nextstate->stateptr->state[i]=='F')) {
	    curstate->state[i]='R';
	  } else if (curstate->state[i]=='1' && 
	      (nextstate->stateptr->state[i]=='0' || nextstate->stateptr->state[i]=='R')) {
	    curstate->state[i]='F';
	  }
	}
	nextstate=nextstate->next;
      }
    }


  for (k=0;k<PRIME;k++) { 
    curstate=state_table[k];
    last=NULL;
    while (curstate != NULL && curstate->state != NULL) {
      nextlink=curstate->link;
      if (curstate->color < 0) {
	if (last) {
	  last->link=curstate->link;
	} else {
	  state_table[k]=curstate->link;
	  //last=state_table[k];
	  nextlink=state_table[k];
	}

	clockptr=curstate->clocklist;
	while(clockptr!=NULL){
	  next_clockptr=clockptr->next;
	  free_region(clockptr->clockkey, clockptr->clocks, 
		      clockptr->clocksize);
	  if ( clockptr->exp_enabled != NULL ) 
	    free(clockptr->exp_enabled);
	  free(clockptr);
	  clockptr=next_clockptr;
	}
	next=curstate->pred;
	while (next) {
	  nextstate=next;
	  if (next) next=nextstate->next;
	  free(nextstate);
	}
	next=curstate->succ;
	while (next) {
	  nextstate=next;
	  if (next) next=nextstate->next;
	  free(nextstate);
	}
	free(curstate->state);
	free(curstate->marking);
	free(curstate->enablings);
	if ((last) || (nextlink)) free(curstate);
	else curstate->state=NULL;

      } else
	last=curstate;
      curstate=nextlink;
    }
  }
  total=0;
  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      if (curstate->color) {
	curstate->probability=curstate->probability/curstate->color;
	total=total+curstate->probability;
      } 
    }
  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      if (total)
	curstate->probability=curstate->probability/total;
    }

  for (k=0;k<nevents;k++)
    if (events[k]->type & DUMMY) {
      if (events[k]->signal >= 0) {
	//printf("nsignals = %d changing to %d\n",*nsignals,
	//       events[k]->signal);
	(*nsignals) = events[k]->signal;
      }
      break;
    }
  int state_cnt=0;
  for (k=0;k<PRIME;k++) 
    for (curstate=state_table[k];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      state_cnt++;
      if (strcmp(curstate->state,"FAIL")!=0)
	curstate->state[*nsignals]='\0';
    }
  if (verbose) {
    for (k=0;k<PRIME;k++) { 
      last=NULL;
      for (curstate=state_table[k];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) { 
	//if (!curstate->pred) fprintf(fp,"ERROR: ");
	fprintf(fp,"%s\n",curstate->state);
      }
    }
    fclose(fp);
  } else {
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("done (states=%d time=%f)\n",state_cnt,time1-time0);
    fprintf(lg,"done (states=%d time=%f)\n",state_cnt,time1-time0);
  }
  return TRUE;
}
