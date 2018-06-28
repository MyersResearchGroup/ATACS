/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1993 by, Chris J. Myers                                   */
/*   Center for Integrated Systems,                                          */
/*   Stanford University                                                     */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of Stanford University may not be used in advertising or   */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Stanford.                         */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL STANFORD OR THE AUTHORS OF THIS SOFTWARE BE LIABLE    */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* findconf.c                                                                */
/*****************************************************************************/

#include "findconf.h"
#include "findrsg.h"
#include "findreg.h"
#include "def.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* Returns TRUE if given a set of rules a particular transition is not       */
/*   prevented in a given state.                                             */
/*****************************************************************************/

bool pre_problem(char * state,char * cover,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if (((cover[i]=='0') || (cover[i]=='1')) && (VAL(state[i]) != cover[i]))
      return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Returns TRUE if given a set of rules a particular transition is not       */
/*   prevented in a given state cube.                                        */
/*****************************************************************************/

bool intersect(char * state,char * cover,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if (((cover[i]=='0') || (cover[i]=='1')) && (VAL(state[i]) != cover[i])
      && (VAL(state[i])!='X'))
      return(FALSE);
  return(TRUE);
}

bool subset(char * state,char * enstate,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if ((enstate[i]!='X') && (enstate[i]!='Z') &&
	(enstate[i]!=VAL(state[i]))) return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Returns TRUE if given a set of rules a particular transition is not       */
/*   prevented in a given state.                                             */
/*****************************************************************************/

bool problem(char * state,char * cover,int nsignals)
{
  int i;
  bool final,compressed;

  final=TRUE;
  compressed=FALSE;
  for (i=0;i<nsignals;i++) {
    if (((cover[i]=='0') || (cover[i]=='1')) && (VAL(state[i]) != cover[i]) &&
	(state[i]!='U') && (state[i]!='D') && (state[i]!='X'))
      return(FALSE);
    if ((state[i]=='U')||(state[i]=='D')||(state[i]=='X')){
      compressed=TRUE;
      if ((state[i]=='U')&&(cover[i]!='1')||(state[i]=='D')&&(cover[i]!='0'))
	final=FALSE;
    }
  }
  if (!compressed || !final) 
    return(TRUE);
  return(FALSE);
}

/*****************************************************************************/
/* Returns TRUE if a cover and a state intersect the enstate.                */
/*****************************************************************************/

bool check_cover(char * state,char * enstate,char * cover,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++) 
    if (cover[i]=='0') {
      if (enstate[i]=='1') return FALSE;
    } else if (cover[i]=='1') {
      if (enstate[i]=='0') return FALSE;
    } else {
      if ((state[i]=='0')||(state[i]=='R')) {
	if (enstate[i]=='1') return FALSE;
      } else if ((state[i]=='1')||(state[i]=='F')) {
	if (enstate[i]=='0') return FALSE;
      } else if (enstate[i]!='X') return FALSE;
    }
  return TRUE;
}

/*****************************************************************************/
/* Returns a bitvector as a string in which a bit has a 1 if the positive    */
/*   phase can be used as a context signal to solve the given problem, a     */
/*   0 if the negative phase can be used, and an X if neither phase can be   */
/*   used.                                                                   */
/*****************************************************************************/

char * pre_solution(char * state,char * enstate,int nsignals,int ninpsig,
		    bool inponly,bool split)
{
  int l,n;
  char * solutions;

  if (inponly) n=ninpsig;
  else n=nsignals;
  solutions=(char *)GetBlock(nsignals+2);
  for (l=0;l<n;l++)
    solutions[l]='X';
  for (l=0;l<n;l++)
    if ((VAL(state[l]) !=  VAL(enstate[l])) && (enstate[l] != 'X'))
      solutions[l]=VAL(enstate[l]);
    else if (split) {
      if (VAL(state[l]) == '0')
	solutions[l]='1';
      else if (VAL(state[l]) == '1')
	solutions[l]='0';
    }
  for (l=n;l<nsignals;l++)
    solutions[l]='X';
  solutions[nsignals]='\0';
  return(solutions);
}

/*****************************************************************************/
/* Returns a bitvector as a string in which a bit has a 1 if the positive    */
/*   phase can be used as a context signal to solve the given problem, a     */
/*   0 if the negative phase can be used, and an X if neither phase can be   */
/*   used.                                                                   */
/*****************************************************************************/

char * solution(char * state,char * enstate,int nsignals,int ninpsig,
		bool inponly,bool split)
{
  int l,n;
  char * solutions;

  if (inponly) n=ninpsig;
  else n=nsignals;
  solutions=(char *)GetBlock(nsignals+2);
  for (l=0;l<n;l++)
    solutions[l]='X';
  for (l=0;l<n;l++)
    if ((VAL(state[l]) !=  VAL(enstate[l])) && (enstate[l] != 'X') &&
	(state[l]!='U') && (state[l]!='D') && (state[l]!='X'))
      solutions[l]=VAL(enstate[l]);
    else if (split) {
      if (VAL(state[l]) == '0')
	solutions[l]='1';
      else if (VAL(state[l]) == '1')
	solutions[l]='0';
    }
  for (l=n;l<nsignals;l++)
    solutions[l]='X';
  solutions[nsignals]='\0';
  return(solutions);
}

/*****************************************************************************/
/* Print context rule tables.                                                */
/*****************************************************************************/

int print_conflicts(FILE *fp,char * filename,signalADT *signals,
		    regionADT *regions,int nevents,int ninpsig,int nsignals,
		    bool multicube,stateADT *state_table,bool verbose,
		    eventADT *events)
{
  int i,k,l;
  contextADT new_entry=NULL;
  char outname[FILENAMESIZE];
  FILE *errfp=NULL;
  bool split;
  int result; /* 0 okay, 1 unresolvable, 2 splitting failed */
  bool unresolvable;
  bool first,deleted;
  regionADT cur_region;
  regionADT prev_region;
  regionADT next_region;

  char * new_region;
  char * new_enablings;
  char * new_tsignals;

  new_region=(char *)GetBlock((nsignals+1)*sizeof(char));
  new_enablings=(char *)GetBlock((((nevents-1)/2)+1)*sizeof(char));
  new_tsignals=(char *)GetBlock((nsignals+1)*sizeof(char));

  result=0;
  if (verbose) {
    fprintf(fp,"CONTEXT SIGNAL TABLES:\n");
    print_state_vector(fp,signals,nsignals,ninpsig);
  }
  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
	   cur_region=cur_region->link) {
	first=TRUE;
	if ((verbose) && (cur_region->context_table != NULL)) {
	  fprintf(fp,"Context Rule Table for %s",signals[i]->name);
	  if (l==1) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
	}
	for (new_entry=cur_region->context_table;
	     new_entry != NULL;
	     new_entry=new_entry->next) {
	  if (verbose)
	    fprintf(fp,"%s:",new_entry->state);
	  unresolvable=TRUE;
	  for (k=0;k<nsignals;k++) {
	    if (events && events[signals[k]->event]->type & DUMMY) 
	      new_entry->solutions[k]='X';
	    if (new_entry->solutions[k]=='1') {
	      if (verbose)
		fprintf(fp," %s",signals[k]->name);
	      unresolvable=FALSE;
	    } else if (new_entry->solutions[k]=='0') {
	      if (verbose)
		fprintf(fp," ~%s",signals[k]->name);
	      unresolvable=FALSE;
	    } else if (new_entry->solutions[k]==('1'+2)) {
	      if (verbose)
		fprintf(fp," %s(O)",signals[k]->name);
	      unresolvable=FALSE;
	    } else if (new_entry->solutions[k]==('0'+2)) {
	      if (verbose)
		fprintf(fp," ~%s(O)",signals[k]->name);
	      unresolvable=FALSE;
	    } 
	  }
	  if (verbose)
	    fprintf(fp,"\n");
	  if (unresolvable) {
	    if (result==0) {
	      if (verbose) {
		strcpy(outname,filename);
		strcat(outname,".err");
		printf("EXCEPTION:  Storing unresolvable conflicts to:  %s\n",
		       outname);
		fprintf(lg,"EXCEPTION:  Storing unresolvable conflicts to:  %s\n",
			outname);
		errfp=Fopen(outname,"w");
		fprintf(errfp,"UNRESOLVABLE CONFLICTS:\n");
		print_state_vector(errfp,signals,nsignals,ninpsig);
	      }
	      result=2;
	    }
	    if (first) {
	      if (verbose) {
		fprintf(errfp,"Unresolvable conflicts for %s",
			signals[i]->name);
		if (l==1) fprintf(errfp,"+"); else fprintf(errfp,"-");
		fprintf(errfp," (%s)\n",cur_region->enstate);
	      }
	      first=FALSE;
	      split=FALSE;
	      if (!multicube) {
		strcpy(new_tsignals,cur_region->tsignals);
		split=split_region(state_table,regions,nsignals,i,l,new_region,
				   new_enablings,new_tsignals,cur_region,NULL);
	      }
	      if (split) {
		cur_region->enstate[0]='K';
		result=1;
	      }
	    }
	    if (verbose)
	      fprintf(errfp,"%s\n",new_entry->state);
	  }
	}
      }
  if ((result!=0) && !multicube) {
    for (i=ninpsig;i<nsignals;i++)
      for (l=1;l<3;l++) {
	prev_region=NULL;
	cur_region=regions[2*i+l];
	while (cur_region) {
	  deleted=FALSE;
	  next_region=cur_region->link;
	  if (cur_region->enstate[0]=='K') {
	    regions[2*i+l]=delete_region(regions[2*i+l],prev_region,
			     	 cur_region,next_region);
	    deleted=TRUE;
	  }
	  if (!deleted) prev_region=cur_region;
	  cur_region=next_region;		      
	}
      }
  }
  if ((result!=0) && !multicube) {
    for (i=ninpsig;i<nsignals;i++)
      for (l=1;l<3;l++)
	for (cur_region=regions[2*i+l];cur_region;
	     cur_region=cur_region->link)
	  cur_region->context_table=NULL;
  }
  free(new_region);
  free(new_enablings);
  free(new_tsignals);

  if ((result!=0) && (verbose)) fclose(errfp);
  if ((result!=0) && (!verbose)) {
    printf("EXCEPTION!\n");
    fprintf(lg,"EXCEPTION!\n");
    printf("EXCEPTION:  Unresolvable conflicts!\n");
    fprintf(lg,"EXCEPTION:  Unresolvable conflicts!\n");
  }
  return(result);
}

/*****************************************************************************/
/* Check for unresolvable conflicts.                                         */
/*****************************************************************************/

bool check_conflicts(regionADT *regions,int ninpsig,int nsignals)
{
  int i,k,l;
  contextADT new_entry=NULL;
  bool unresolvable;
  regionADT cur_region;

  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
	   cur_region=cur_region->link) {
	for (new_entry=cur_region->context_table;
	     new_entry != NULL;
	     new_entry=new_entry->next) {
	  unresolvable=TRUE;
	  for (k=0;k<nsignals;k++) {
	    if (new_entry->solutions[k]=='1') unresolvable=FALSE;
	    else if (new_entry->solutions[k]=='0') unresolvable=FALSE;
	    else if (new_entry->solutions[k]=='1'+2) unresolvable=FALSE;
	    else if (new_entry->solutions[k]=='0'+2) unresolvable=FALSE;
	  }
	  if (unresolvable) {
	    printf("EXCEPTION!\n");
	    fprintf(lg,"EXCEPTION!\n");
	    printf("EXCEPTION:  Unresolvable conflicts!\n");
	    fprintf(lg,"EXCEPTION:  Unresolvable conflicts!\n");
	    return(FALSE);
	  }
	}
      }
  return(TRUE);
}

/*****************************************************************************/
/* Add entry to prime implicant table.                                       */
/*****************************************************************************/

contextADT add_entry_to_crt(regionADT region,char * state,char * solution)
{
  contextADT new_entry=NULL;

  new_entry=(contextADT)GetBlock(sizeof *new_entry);
  new_entry->next=region->context_table;
  new_entry->prev=NULL;
  if (region->context_table != NULL)
    region->context_table->prev=new_entry;
  region->context_table=new_entry;
  new_entry->state=CopyString(state);
  new_entry->solutions=solution;
  return(new_entry);
}

/*****************************************************************************/
/* Check covering constraint.                                                */
/*****************************************************************************/

void chk_covering(stateADT curstate,regionADT *regions,int k,int nsignals,
		  int ninpsig,bool inponly,bool split)
{
  char * cursoln;
  char * tmpstate;
  regionADT cur_region,cur_region2;
  int i,offset;
  bool compressed,splitit;
  statelistADT nextstate;

  tmpstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  if ((curstate->state[k]=='0') || (curstate->state[k]=='F')
      || (curstate->state[k]=='D')) offset=1;
  else offset=2;
  for (cur_region=regions[k*2+offset]; cur_region; 
       cur_region=cur_region->link) {
    if (problem(curstate->state,cur_region->tsignals,nsignals)) {

      //      if (!check_cover(curstate->state,cur_region->enstate,
      //		       cur_region->tsignals,nsignals)) {
      strcpy(tmpstate,curstate->state);
      compressed=FALSE;
      splitit=FALSE;
      for (i=0;i<nsignals;i++) 
	if (tmpstate[i]=='U') {
	  compressed=TRUE;
	  tmpstate[i]='1';
	} else if (tmpstate[i]=='D') {
	  compressed=TRUE;
	  tmpstate[i]='0';
	}	
      if (compressed) {
	for (cur_region2=regions[k*2+offset]; cur_region2; 
	     cur_region2=cur_region2->link) 
	  if (intersect(tmpstate,cur_region2->enstate,nsignals)) { 
	    splitit=TRUE;
	    break;
	  }
      }
      if (splitit) {
	for (nextstate=curstate->succ;nextstate;nextstate=nextstate->next) {
	  strcpy(tmpstate,curstate->state);
	  for (i=0;i<nsignals;i++) 
	    if ((curstate->state[i]=='U') && 
		(nextstate->stateptr->state[i]=='1')) {
	      tmpstate[i]='0';
	      if (intersect(tmpstate,cur_region->tsignals,nsignals)) {
		cursoln=solution(tmpstate,cur_region->enstate,
				 nsignals,ninpsig,inponly,split);
		add_entry_to_crt(cur_region,tmpstate/*curstate->state*/,
				 cursoln);
	      }
	      tmpstate[i]='U';
	    } else if ((curstate->state[i]=='D') &&
		       (nextstate->stateptr->state[i]=='0')) {
	      tmpstate[i]='1';
	      if (intersect(tmpstate,cur_region->tsignals,nsignals)) {
		cursoln=solution(tmpstate,cur_region->enstate,
				 nsignals,ninpsig,inponly,split);
		add_entry_to_crt(cur_region,tmpstate/*curstate->state*/,
				 cursoln);
	      }
	      tmpstate[i]='D';
	    }  
	}
      } else {
	cursoln=solution(curstate->state,cur_region->enstate,
			 nsignals,ninpsig,inponly,split);
	add_entry_to_crt(cur_region,curstate->state,cursoln);
      }
    }
  }
  free(tmpstate);
}

/*****************************************************************************/
/* Check synchronous covering constraint.                                    */
/*****************************************************************************/

void chk_sync_covering(stateADT curstate,regionADT *regions,int k,int nsignals,
		       int ninpsig,bool inponly,bool split)
{
  char * cursoln;
  char * tmpstate;
  regionADT cur_region;
  int i,offset;
  bool okay;
  statelistADT nextstate;

  tmpstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  if ((curstate->state[k]=='0') || (curstate->state[k]=='F')
      || (curstate->state[k]=='D')) offset=1;
  else return;
  for (cur_region=regions[k*2+offset]; cur_region; 
       cur_region=cur_region->link) {
    if (problem(curstate->state,cur_region->tsignals,nsignals)) {
      strcpy(tmpstate,curstate->state);
      for (i=0;i<nsignals;i++) 
	if (tmpstate[i]=='U') {
	  tmpstate[i]='1';
	} else if (tmpstate[i]=='D') {
	  tmpstate[i]='0';
	}	
      okay=TRUE;
      for (nextstate=curstate->succ;nextstate;nextstate=nextstate->next)
	if (nextstate->stateptr->state[k]!='U') okay=FALSE;
      if (!okay) {
	cursoln=solution(tmpstate,cur_region->enstate,
			 nsignals,ninpsig,inponly,split);
	add_entry_to_crt(cur_region,tmpstate,cursoln);
      }
    }
  }
  free(tmpstate);
}

/*****************************************************************************/
/* Check synchronous covering constraint.                                    */
/*****************************************************************************/
/*
void chk_sync_covering(stateADT curstate,regionADT *regions,int k,int nsignals,
		       int ninpsig,bool inponly,bool split)
{
  char * cursoln;
  char * tmpstate;
  regionADT cur_region;
  int i,offset;

  tmpstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  if ((curstate->state[k]=='0') || (curstate->state[k]=='F')
      || (curstate->state[k]=='D')) offset=1;
  else return;
  for (cur_region=regions[k*2+offset]; cur_region; 
       cur_region=cur_region->link) {
    strcpy(tmpstate,curstate->state);
    for (i=0;i<nsignals;i++) 
      if (tmpstate[i]=='U')
	tmpstate[i]='1';
      else if (tmpstate[i]=='D')
	tmpstate[i]='0';
    cursoln=solution(tmpstate,cur_region->enstate,nsignals,ninpsig,inponly,
		     split);
    add_entry_to_crt(cur_region,tmpstate,cursoln);
  }
  free(tmpstate);
}
*/

/*****************************************************************************/
/* Check if a state is a final state.                                        */
/* (i.e., all enabled events state1 are fired or disabled in state2)         */
/*****************************************************************************/
bool is_final_state(char * state1,char * state2,int nsignals)
{
  int j; 
  
  for (j=0;j<nsignals;j++) {
    if ((state1[j]=='R') && (state2[j]=='R')) return FALSE;
    else if ((state1[j]=='F') && (state2[j]=='F')) return FALSE;
  }
  return TRUE;
}

/*****************************************************************************/
/* Convert R to 0 and F to 1 for all bits in a state.                        */
/*****************************************************************************/

char * SVAL2(char * state,int nsignals)
{
  int k;
  char * vstate;

  vstate=(char *)GetBlock(nsignals+2);
  for (k=0;k<nsignals;k++) {
    if (state[k]=='U')
      vstate[k]='1';
    else if (state[k]=='D')
      vstate[k]='0';
    else
      vstate[k]=VAL(state[k]);
  }
  vstate[k]='\0';
  return(vstate);
}

/*****************************************************************************/
/* Find a list of final states for a burst                                   */
/* (i.e., fire sets of non-conflicting events to reach stable state          */
/*****************************************************************************/

bool final_state(stateADT *state_table,stateADT cur_state,int nsignals,
		 char * result)
{
  int j;
  statelistADT nextstate;
  bool fin,new_en;

  for (nextstate=cur_state->succ;nextstate;nextstate=nextstate->next) {
    fin=TRUE;
    new_en=FALSE;
    for (j=0;j<nsignals;j++) {
      if (((cur_state->state[j]=='R')&&(nextstate->stateptr->state[j]=='R'))||
	  ((cur_state->state[j]=='F')&&(nextstate->stateptr->state[j]=='F'))) 
	fin=FALSE;
      if (((cur_state->state[j]!='R')&&(nextstate->stateptr->state[j]=='R'))|| 
	  ((cur_state->state[j]!='F')&&(nextstate->stateptr->state[j]=='F'))) 
	new_en=TRUE;
    }
    if (fin) {
      for (j=0;j<nsignals;j++)
	if ((nextstate->stateptr->state[j]=='R')||
	    (nextstate->stateptr->state[j]=='F'))
	  result[j]=nextstate->stateptr->state[j];
    } else if (new_en) {
      /*printf("ERROR: State graph is not burst-mode.\n");
	fprintf(lg,"ERROR: State graph is not burst-mode.\n");*/
      return FALSE;
    } else {
      if (!final_state(state_table,nextstate->stateptr,nsignals,result))
	return FALSE;
    }
  }
  return TRUE;
}

/*****************************************************************************/
/* Find a list of final states for a burst                                   */
/* (i.e., fire sets of non-conflicting events to reach stable state          */
/*****************************************************************************/

char final(stateADT *state_table,char * curstate,int signal,int nsignals,
	   stateADT cur_state) 
{
  int j;
  char result,newresult;
  stateADT e;
  statelistADT nextstate;
  bool fin,new_en,fin_found;

  fin_found=FALSE;
  result='Z';
  for (nextstate=cur_state->succ;nextstate;nextstate=nextstate->next) {
    e=nextstate->stateptr;
    fin=TRUE;
    new_en=FALSE;
    for (j=0;j<nsignals;j++) {
      if ((curstate[j]=='R') && (e->state[j]=='R')) fin=FALSE;
      else if ((curstate[j]=='F') && (e->state[j]=='F')) fin=FALSE;
      if ((curstate[j]!='R') && (e->state[j]=='R')) new_en=TRUE;
      else if ((curstate[j]!='F') && (e->state[j]=='F')) new_en=TRUE;
    }
    if ((fin) && ((e->state[signal]=='R')||(e->state[signal]=='F'))) {
      //	    printf("Found final state %s\n",e->state);
      result=e->state[signal];
    } else if (fin) {
      //printf("Found final state %s\n",e->state);
      ;
    } else if (new_en) {
      printf("ERROR: State graph is not burst-mode.\n");
      fprintf(lg,"ERROR: State graph is not burst-mode.\n");
      return 'X';
    } else {
      newresult=final(state_table,e->state,signal,nsignals,e);
      if (newresult=='X')
	return 'X';
      else if (result=='Z')
	newresult=result;
    }
  }
  return result;
}

/*****************************************************************************/
/* Find a list of final states for a burst                                   */
/* (i.e., fire sets of non-conflicting events to reach stable state          */
/*****************************************************************************/

char old_final(stateADT *state_table,char * curstate,int signal,int nsignals,
	       stateADT cur_state) 
{
  int i,j;
  char * state;
  char * state2;
  char * state3;
  char result;
  stateADT e;
  statelistADT nextstate;
  bool fin,new_en,fin_found;

  fin_found=FALSE;
  state=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (i=0;i<nsignals;i++)
    if ((curstate[i]=='R') || (curstate[i]=='F')) {
      strcpy(state,curstate);
      if (curstate[i]=='R')
	state[i]='1';
      else if (curstate[i]=='F')
	state[i]='0';
      state3=SVAL2(state,nsignals);

      /*
      position=hashpjw(state3);
      for (e = state_table[position]; e->link != NULL;e = e->link) {
      */
      for (nextstate=cur_state->succ;nextstate;nextstate=nextstate->next) {
	e=nextstate->stateptr;

	state2=SVAL2(e->state,nsignals);
	if (strcmp(state3,state2)==0) {
	  fin=TRUE;
	  new_en=FALSE;
	  for (j=0;j<nsignals;j++) {
	    if ((curstate[j]=='R') && (e->state[j]=='R')) fin=FALSE;
	    else if ((curstate[j]=='F') && (e->state[j]=='F')) fin=FALSE;
	    if ((curstate[j]!='R') && (e->state[j]=='R')) new_en=TRUE;
	    else if ((curstate[j]!='F') && (e->state[j]=='F')) new_en=TRUE;
	  }
	  if ((fin) && ((e->state[signal]=='R')||(e->state[signal]=='F'))) {
	    free(state);
	    free(state2);
	    free(state3);
	    //	    printf("Found final state %s\n",e->state);
	    return e->state[signal];
	  } else if (fin) {
	    //printf("Found final state %s\n",e->state);
	    fin_found=TRUE;
	  } else if (new_en) {
	    printf("ERROR: State graph is not burst-mode.\n");
	    fprintf(lg,"ERROR: State graph is not burst-mode.\n");
	    free(state);
	    free(state2);
	    free(state3);
	    return 'X';
	  } else {
	    result=final(state_table,e->state,signal,nsignals,e);
	    if ((result=='R')||(result=='F')||(result=='X')) {
	      free(state);
	      free(state2);
	      free(state3);
	      return result;
	    } else
	      fin_found=TRUE;
	  }
	}
	free(state2);
      }
      free(state3);
    }
  free(state);
  if (fin_found) return 'Z';
  return 'X';
}
/*
char final(stateADT *state_table,char * curstate,int signal,int nsignals) 
{
  int i,position;
  char * state;
  char * state2;
  char result;
  stateADT e;

  state=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (i=0;i<nsignals;i++)
    if (curstate[i]=='R')
      state[i]='1';
    else if (curstate[i]=='F')
      state[i]='0';
    else 
      state[i]=curstate[i];
  state[nsignals]='\0';
  position=hashpjw(state);
  result='X';
  for (e = state_table[position]; e->link != NULL;e = e->link) {
    state2=SVAL(e->state,nsignals);
    if (strcmp(state,state2)==0) 
      result=e->state[signal];
    free(state2);
  }
  free(state);
  return(result);
}
*/

/*****************************************************************************/
/* Check entrance constraint.                                                */
/*****************************************************************************/

void old_chk_entrance(stateADT *state_table,stateADT curstate,
		      regionADT *regions,
		      int k,int nsignals,int ninpsig,bool inponly,bool burst,
		      bool split)
{
  int l;
  statelistADT predstate;
  contextADT new_entry=NULL;
  char * predsoln;
  char * cursoln;
  bool diff;
  regionADT cur_region;
  int offset;
  char bit;

  if (curstate->state[k]=='1') offset=1;
  else if (curstate->state[k]=='0') offset=2;
  else return;
  for (cur_region=regions[k*2+offset]; cur_region;cur_region=cur_region->link) 
    if (problem(curstate->state,cur_region->tsignals,nsignals)) {
      
      /* Check self for compressed state graphs */
      /*
      if (((curstate->state[k]!='R') && (offset==1)) ||
	  ((curstate->state[k]!='F') && (offset==2)) ||
	  (!pre_problem(curstate->state,cur_region->enstate,nsignals))) {
	predsoln=pre_solution(curstate->state,cur_region->enstate,nsignals,
			      ninpsig,inponly,split);
	cursoln=solution(curstate->state,cur_region->enstate,
			 nsignals,ninpsig,inponly,split);
	diff=FALSE;
	for (l=0;l<nsignals;l++)
	  if ((cursoln[l] != predsoln[l]) &&
	      ((predsoln[l]=='0') || (predsoln[l]=='1')))
	    diff=TRUE;

	if ((diff) && (burst)) {
	  if (is_final_state(curstate->state,curstate->state,nsignals)){
	    bit=curstate->state[k];
	  } else 
	    bit=final(state_table,curstate->state,k,nsignals,curstate);
	  if (((offset==1) && (bit!='F')) || ((offset==2) && (bit!='R')))
	    diff=FALSE;
	  if ((!diff) && (bit=='X'))
	    printf("ERROR: Could not find final state for %s\n",
		   curstate->state);
	}

	if (diff) {
	  new_entry=add_entry_to_crt(cur_region,curstate->state,cursoln);
	  if (pre_problem(curstate->state,
			  cur_region->tsignals,nsignals)) {
	    for (l=0;l<nsignals;l++)
	      if ((new_entry->solutions[l] != predsoln[l]) &&
		  ((predsoln[l]=='0') || (predsoln[l]=='1')))
		new_entry->solutions[l]=predsoln[l]+2;
	  }
	} else free(cursoln);
	free(predsoln);
      }
      */
      for (predstate=curstate->pred;predstate;predstate=predstate->next)
	if (((predstate->stateptr->state[k]!='R') && (offset==1)) ||
	    ((predstate->stateptr->state[k]!='F') && (offset==2)) ||
	    (!problem(predstate->stateptr->state,
		      cur_region->enstate,nsignals))) {
	  predsoln=solution(predstate->stateptr->state,
			    cur_region->enstate,nsignals,ninpsig,inponly,
			    split);
	  cursoln=solution(curstate->state,cur_region->enstate,
			   nsignals,ninpsig,inponly,split);
	  diff=FALSE;
	  for (l=0;l<nsignals;l++)
	    if ((cursoln[l] != predsoln[l]) &&
		((predsoln[l]=='0') || (predsoln[l]=='1')))
	      diff=TRUE;

	  if ((diff) && (burst)) {
	    //printf("Finding final state for %s->%s\n",
	    //	   predstate->stateptr->state,curstate->state);
	    if (is_final_state(predstate->stateptr->state,curstate->state,nsignals)){
	      //  printf("%s is the final state\n",curstate->state);
	      bit=curstate->state[k];
	    } else 
	      bit=final(state_table,curstate->state,k,nsignals,curstate);
	    if (((offset==1) && (bit!='F')) || ((offset==2) && (bit!='R')))
	      diff=FALSE;
	    if ((!diff) && (bit=='X'))
	      printf("ERROR: Could not find final state for %s\n",
		     predstate->stateptr->state);
	  }

	  if (diff) {
	    new_entry=add_entry_to_crt(cur_region,curstate->state,cursoln);
	    if (problem(predstate->stateptr->state,
			cur_region->tsignals,nsignals)) {
	      for (l=0;l<nsignals;l++)
		if ((new_entry->solutions[l] != predsoln[l]) &&
		    ((predsoln[l]=='0') || (predsoln[l]=='1')))
		  new_entry->solutions[l]=predsoln[l]+2;
	    }
	  } else free(cursoln);
	  free(predsoln);
	}
    } 
}

/*****************************************************************************/
/* Check entrance constraint.                                                */
/*****************************************************************************/

void chk_entrance(stateADT *state_table,stateADT curstate,regionADT *regions,
		  int k,int nsignals,int ninpsig,bool inponly,
		  bool split/*,bool fin,char * finalstate*/)
{
  int l;
  int v=0;
  statelistADT predstate;
  contextADT new_entry=NULL;
  char * cursoln;
  //char *finstate;
  bool diff;
  regionADT cur_region;
  int offset;

  /* finstate=(char *)GetBlock((nsignals+1)*sizeof(char));*/
  if (curstate->state[k]=='1') offset=1;
  else if (curstate->state[k]=='0') offset=2;
  else return;

  /* Check self for compressed state graphs */
  /*
  if (burst) {
    strcpy(finstate,finalstate);
    if (!fin)
      printf("ERROR: Could not find final state for %s.\n",
	     curstate->state);
  }
  */
  for (cur_region=regions[k*2+offset];cur_region;cur_region=cur_region->link)
    if (problem(curstate->state,cur_region->tsignals,nsignals)) {
      if (((curstate->state[k]!='R') && (offset==1)) ||
	  ((curstate->state[k]!='F') && (offset==2)) ||
	  (!problem(curstate->state,cur_region->enstate,nsignals))) {
	diff=TRUE;
        /*
	if ((diff) && (burst)) {
	  if (((offset==1) && (finstate[k]!='F')) || 
	      ((offset==2) && (finstate[k]!='R')))
	    diff=FALSE;
	}
        */
	if (diff) {
	  for (l=0;l<nsignals;l++)
	    if (((curstate->state[l]=='U')&&(cur_region->enstate[l]=='1')) ||
		((curstate->state[l]=='D')&&(cur_region->enstate[l]=='0')) ||
		((curstate->state[l]=='X')&&(cur_region->enstate[l]!='X'))) {
	      cursoln=solution(curstate->state,cur_region->enstate,
			       nsignals,ninpsig,inponly,split);
	      new_entry=add_entry_to_crt(cur_region,curstate->state,cursoln);
	      if (cur_region->tsignals[l]=='Z')
		new_entry->solutions[l]=cur_region->enstate[l]+2;
	    }
	} 
      }
    } 

  for (predstate=curstate->pred;predstate;predstate=predstate->next) {
    /*
    if (burst) {
      if (is_final_state(predstate->stateptr->state,curstate->state,nsignals)) {
	for (l=0;l<nsignals;l++)
	  if ((curstate->state[l]=='R')||(curstate->state[l]=='F'))
	    finstate[l]=curstate->state[l];
      } else {
	strcpy(finstate,finalstate);
	if (!fin)
	  printf("ERROR: Could not find final state for %s->%s.\n",
		 predstate->stateptr->state,curstate->state);
      } 
    }
    */
    for (cur_region=regions[k*2+offset];cur_region;cur_region=cur_region->link)
      if (problem(curstate->state,cur_region->tsignals,nsignals)) {
	if (((predstate->stateptr->state[k]!='R') && (offset==1)) ||
	    ((predstate->stateptr->state[k]!='F') && (offset==2)) ||
	    (!problem(predstate->stateptr->state,
		      cur_region->enstate,nsignals))) {

	  diff=FALSE;
	  for (l=0;l<nsignals;l++)
	    if (VAL(predstate->stateptr->state[l])!=
		VAL(curstate->state[l])) {
	      if (cur_region->enstate[l]==VAL(curstate->state[l])) {
		diff=TRUE;
		v=l;
	      }
	      break;
	    }
	  /*
	  if ((diff) && (burst)) {
	    if (((offset==1) && (finstate[k]!='F')) || 
		((offset==2) && (finstate[k]!='R')))
	      diff=FALSE;
	  }
	  */
	  if (diff) {
	    cursoln=solution(curstate->state,cur_region->enstate,
			     nsignals,ninpsig,inponly,split);
	    new_entry=add_entry_to_crt(cur_region,curstate->state,cursoln);
	    if (problem(predstate->stateptr->state,
			cur_region->tsignals,nsignals)) 
	      new_entry->solutions[v]=cur_region->enstate[v]+2;
	  } 
	}
    } 
  }
  /*free(finstate);*/
}

/*****************************************************************************/
/* Check if gates can be shared for multiple excitation regions.             */
/*****************************************************************************/

void chk_share(stateADT curstate,regionADT *regions,int k,int nsignals,
	       int ninpsig,bool inponly,bool split)
{
  int m,offset;
  contextADT new_entry=NULL;
  char * cursoln;
  regionADT cur_region;
  regionADT cur_region2;

  if (curstate->state[k]=='R') offset=1;
  else if (curstate->state[k]=='F') offset=2;
  else return;
  for (cur_region=regions[k*2+offset]; cur_region;cur_region=cur_region->link) 
    if ((!problem(curstate->state,cur_region->enstate,nsignals)) &&
	(problem(curstate->state,cur_region->tsignals,nsignals))) {
      for (cur_region2=regions[k*2+offset]; cur_region2;
	   cur_region2=cur_region2->link)
	if (problem(curstate->state,cur_region2->enstate,nsignals)){
	  if (!problem(cur_region2->enstate,cur_region->tsignals,nsignals)) {
	    cursoln=solution(curstate->state,cur_region->enstate,nsignals,
			     ninpsig,inponly,split);
	    add_entry_to_crt(cur_region,curstate->state,cursoln);
	  } else {
	    for (m=0;m<nsignals;m++) {
	      if ((cur_region->enstate[m] != cur_region2->enstate[m]) && 
		  (cur_region->enstate[m] != 'X') &&
		  (VAL(curstate->state[m])==VAL(cur_region->enstate[m]))) {
		cursoln=solution(curstate->state,cur_region->enstate,nsignals,
				 ninpsig,inponly,split);
		new_entry=add_entry_to_crt(cur_region,curstate->state,cursoln);
		new_entry->solutions[m]=VAL(cur_region->enstate[m])+2;
	      }
	    }
	  } 
	} 
    }
}

/*****************************************************************************/
/* Check SC covering constraint.                                             */
/*****************************************************************************/

void chk_SC_covering(stateADT curstate,regionADT *regions,int k,int nsignals,
		     int ninpsig,bool inponly,bool split)
{
  int reg,offset;
  char * cursoln;
  regionADT cur_region;

  if (curstate->state[k]=='R') offset=1; 
  else if (curstate->state[k]=='F') offset=2;
  else return;
  for (cur_region=regions[k*2+offset]; cur_region;cur_region=cur_region->link) 
    if ((!problem(curstate->state,cur_region->enstate,nsignals)) &&
	(problem(curstate->state,cur_region->tsignals,nsignals))) {
      cursoln=solution(curstate->state,cur_region->enstate,nsignals,ninpsig,
		       inponly,split);
      add_entry_to_crt(cur_region,curstate->state,cursoln);
    }
  reg=0;
  for (cur_region=regions[k*2+offset]; cur_region;cur_region=cur_region->link) 
    if (problem(curstate->state,cur_region->enstate,nsignals)) reg++;
  if (reg > 1) printf("NON-DISJOINT ENABLED CUBES\n");
}

/*****************************************************************************/
/* Check burst-mode start cube for 1-0 hazard.                               */
/*****************************************************************************/

void chk_start_cube(stateADT curstate,regionADT *regions,int k,
		    int nsignals,int ninpsig,bool inponly,bool split)
{
  int i,m,offset;
  char * cursoln;
  char * statecube;
  char * startstate;
  regionADT cur_region;
  contextADT new_entry=NULL;
  statelistADT nextstate;
  bool dynamic;

  if (curstate->state[k]=='1') offset=1; 
  /*  else if (curstate->state[k]=='0') offset=2;*/
  else return;
  dynamic=FALSE;
  for (nextstate=curstate->succ;nextstate;nextstate=nextstate->next)
    if (((curstate->state[k]=='1') && 
	 ((nextstate->stateptr->state[k]=='F')||
	  (nextstate->stateptr->state[k]=='D'))) /*|| 
        ((curstate->state[k]=='0') && 
	 ((nextstate->stateptr->state[k]=='R')||
	 (nextstate->stateptr->state[k]=='U')))*/) {
      dynamic=TRUE;
      break;
    }
  if (!dynamic) return;
  startstate=CopyString(curstate->state);
  for (i=0;i<nsignals/*ninpsig*/;i++)
    if (startstate[i]=='U')
      startstate[i]='0';
    else if (startstate[i]=='D')
      startstate[i]='1';
  /* NEED TO DEAL WITH CONDITIONAL SIGNALS */
  statecube=CopyString(curstate->state);
  for (cur_region=regions[k*2+offset];cur_region;cur_region=cur_region->link){
    if (intersect(statecube,cur_region->tsignals,nsignals)) {
      if (!intersect(startstate,cur_region->tsignals,nsignals)) {
	cursoln=solution(statecube,cur_region->enstate,nsignals,ninpsig,
			 inponly,split);
	add_entry_to_crt(cur_region,statecube,cursoln);
      } else {
	for (m=0;m<nsignals;m++) {
	  if ((cur_region->enstate[m] != 'X') && 
	      (cur_region->tsignals[m]=='Z') &&
	      (VAL(statecube[m])=='X') && 
	      (startstate[m]!=cur_region->enstate[m])) {
	    cursoln=solution(statecube,cur_region->enstate,nsignals,
			     ninpsig,inponly,split);
	    new_entry=add_entry_to_crt(cur_region,statecube,cursoln);
	    new_entry->solutions[m]=VAL(cur_region->enstate[m])+2;
	  }
	}
      }
    }
  }
  free(statecube);
  free(startstate);
}

/*****************************************************************************/
/* Check burst-mode end cube 0-1 hazard.                                     */
/*****************************************************************************/

void chk_end_cube(stateADT curstate,regionADT *regions,int k,
		  int nsignals,int ninpsig,bool inponly,bool split)
{
  int i,m,offset;
  char * cursoln;
  char * statecube;
  char * endstate;
  regionADT cur_region;
  contextADT new_entry=NULL;
  bool level;

  if (curstate->state[k]=='U') offset=1; 
  else if (curstate->state[k]=='D') offset=2;
  else return;
  endstate=CopyString(curstate->state);
  level=FALSE;
  for (i=0;i<ninpsig;i++) 
    if (endstate[i]=='U')
      endstate[i]='1';
    else if (endstate[i]=='D')
      endstate[i]='0';
    else if (endstate[i]=='X')
      level=TRUE;
  if ((!level) && (strcmp(endstate,curstate->state)==0)) { 
    free(endstate); 
    return; 
  }
  statecube=CopyString(curstate->state);
  for (i=ninpsig;i<nsignals;i++) {
    if (endstate[i]=='U')
      endstate[i]='0';
    else if (endstate[i]=='D')
      endstate[i]='1';
    if (statecube[i]=='U')
      statecube[i]='0';
    else if (statecube[i]=='D')
      statecube[i]='1';
  }    
  for (cur_region=regions[k*2+offset]; cur_region;cur_region=cur_region->link){
    if (intersect(statecube,cur_region->tsignals,nsignals)) {
      if (!intersect(endstate,cur_region->tsignals,nsignals)) {
	cursoln=solution(statecube,cur_region->enstate,nsignals,ninpsig,
			 inponly,split);
	add_entry_to_crt(cur_region,statecube,cursoln);
      } else {
	for (m=0;m<ninpsig;m++) {
	  if ((cur_region->enstate[m] != 'X') && 
	      (cur_region->tsignals[m]=='Z') &&
	      (VAL(statecube[m])=='X') && 
	      (endstate[m]!=cur_region->enstate[m])) {
	    cursoln=solution(statecube,cur_region->enstate,nsignals,
			     ninpsig,inponly,split);
	    new_entry=add_entry_to_crt(cur_region,statecube,cursoln);
	    new_entry->solutions[m]=VAL(cur_region->enstate[m])+2;
	  }
	}
      }
    }
  }
  free(statecube);
  free(endstate);
}

/*****************************************************************************/
/* Build context rule tables.                                                */
/*****************************************************************************/

void build_cr_tables(stateADT *state_table,regionADT *regions,int ninpsig,
		     int nsignals,bool inponly,bool gatelevel,bool sharegates,
		     bool burstgc,bool burst2l,bool split,bool sync)
{
  int i,k/*,l*/;
  stateADT curstate;
  /* bool fin;
  char * finalstate; */

  /* finalstate=(char *)GetBlock((nsignals+1)*sizeof(char)); */
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      /*
      if (burst) {
	for (l=0;l<nsignals;l++)
	  finalstate[l]='Z';
	finalstate[nsignals]='\0';
	fin=final_state(state_table,curstate,nsignals,finalstate);
      }
      */
      for (k=ninpsig;k<nsignals;k++) { 
	if (sync)
	  chk_sync_covering(curstate,regions,k,nsignals,ninpsig,inponly,split);
	else
	  chk_covering(curstate,regions,k,nsignals,ninpsig,inponly,split);
	if ((gatelevel || burst2l || burstgc) && (!sync)) {
	  if (gatelevel)
	    chk_entrance(state_table,curstate,regions,k,nsignals,ninpsig,
			 inponly,split/*,fin,finalstate*/);
	  if (burstgc || burst2l) {
	    if (burst2l)
	      chk_start_cube(curstate,regions,k,nsignals,ninpsig,inponly,
			     split);
	    chk_end_cube(curstate,regions,k,nsignals,ninpsig,inponly,split);
	  } else if (sharegates) chk_share(curstate,regions,k,nsignals,ninpsig,
					   inponly,split);
	  else chk_SC_covering(curstate,regions,k,nsignals,ninpsig,inponly,
			       split);
	}
      }
    }
  /*free(finalstate);*/
}

/*****************************************************************************/
/* Find conflict problems and possible context signal solutions.             */
/*****************************************************************************/

bool find_conf(char * filename,signalADT *signals,stateADT *state_table, 
	       regionADT *regions,int ninpsig,int nsignals,bool verbose,
	       bool gatelevel,bool sharegates,bool burstgc,bool burst2l,
	       bool multicube,int nevents,bool exception,bool inponly,
	       bool sync,eventADT *events)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int result;
  timeval t0,t1;
  double time0, time1;

  gettimeofday(&t0, NULL);
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".crt");
    printf("Finding conflicts and storing context rule tables to:  %s\n",
	   outname);
    fprintf(lg,"Finding conflicts and storing context rule tables to:  %s\n",
	    outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Finding conflicts ... ");
    fflush(stdout);
    fprintf(lg,"Finding conflicts ... ");
    fflush(lg);
  }    
  build_cr_tables(state_table,regions,ninpsig,nsignals,inponly,gatelevel,
		  sharegates,burstgc,burst2l,FALSE,sync);
  result=print_conflicts(fp,filename,signals,regions,nevents,ninpsig,nsignals,
			 FALSE,state_table,verbose,events);
  /* okay=check_conflicts(regions,ninpsig,nsignals); */ 


  if (exception) {
    while (result==1) {
      if (verbose) {
	fclose(fp);
	strcpy(outname,filename);
	strcat(outname,".es");
	fp=Fopen(outname,"w");
	printf("Splitting regions and storing to:  %s\n",outname);
	fprintf(lg,"Splitting regions and storing to:  %s\n",outname);
	print_regions(fp,signals,regions,ninpsig,nsignals);
	fclose(fp);
	strcpy(outname,filename);
	strcat(outname,".crt");
	fp=Fopen(outname,"w");
      } else {
	printf("Splitting regions ... ");
	fprintf(lg,"Splitting regions ... ");
      }
      build_cr_tables(state_table,regions,ninpsig,nsignals,inponly,gatelevel,
		      sharegates,burstgc,burst2l,FALSE,sync);
      result=print_conflicts(fp,filename,signals,regions,nevents,ninpsig,
			     nsignals,FALSE,state_table,verbose,events);
      /* okay=check_conflicts(regions,ninpsig,nsignals); */ 
    }
    if (result==2) {
      printf("ERROR: Splitting regions failed to produce a result.\n");
      fprintf(lg,"ERROR: Splitting regions failed to produce a result.\n");
    }
  }
  if (verbose) fclose(fp);
  else if (result==0) {
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("done (%f)\n",time1-time0);
    fprintf(lg,"done (%f)\n",time1-time0);
  }    
  if (result==0) 
    return TRUE;
  return FALSE;
}

/*****************************************************************************/
/* Find conflicts.                                                           */
/*****************************************************************************/

bool find_conflicts(char command,designADT design)
{
  bool verbose;

  if (!(design->status & FOUNDCONF)) {
    verbose=design->verbose;
    if (command==FINDVIOL) verbose=TRUE;
    if (!find_conf(design->filename,design->signals,design->state_table,
		   design->regions,design->ninpsig,design->nsignals,
		   verbose,design->gatelevel,design->sharegates,
		   design->burstgc,design->burst2l,design->multicube,
		   design->nevents,design->exception,design->inponly,
		   ((design->atomic)&&(design->burstgc)),
		   design->events)) {
/* DELATE CONFLICT TABLES HERE */
      return(FALSE);
    }
    design->status=design->status | FOUNDCONF;
  }
  return(TRUE);
}
