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
/* findreg.c                                                                 */
/*****************************************************************************/

#include "findreg.h"
#include "def.h"
#include "interface.h"
#include "memaloc.h"
#include "findrsg.h"
#include "addpers.h"
#include "connect.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* Find switching regions.                                                   */
/*****************************************************************************/

void find_switching(char * filename,signalADT *signals,stateADT *state_table,
		    regionADT *regions,int nsigs,int ninpsig)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,j,l;
  stateADT curstate1;
  statelistADT curstate2;
  regionADT cur_region;

  printf("Finding switch regions ... ");
  fprintf(lg,"Finding switch regions ... ");
  fflush(stdout);
  for (i=ninpsig;i<nsigs;i++) 
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link){
	cur_region->switching=(char *)GetBlock((nsigs+1)*sizeof(char));
	for (j=0;j<nsigs;j++) cur_region->switching[j]='Z';
	cur_region->switching[j]='\0';
      }
  for (i=0;i<PRIME;i++)
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link)
      for (curstate2=curstate1->succ;
	   (curstate2 != NULL && curstate2->stateptr->state != NULL);
	   (curstate2=curstate2->next))
	for (j=ninpsig;j<nsigs;j++) 
	  for (l=1;l<3;l++)
	    for (cur_region=regions[2*j+l];cur_region;
		 cur_region=cur_region->link)
	      if (((l==1) && 
		   ((curstate1->state[j]=='R')||(curstate1->state[j]=='U')) && 
		   (VAL(curstate2->stateptr->state[j])=='1')) ||
		  ((l==2) && 
		   ((curstate1->state[j]=='F')||(curstate1->state[j]=='D')) && 
		   (VAL(curstate2->stateptr->state[j])=='0')))
	      union_enstate(cur_region->switching,
			    curstate2->stateptr->state,nsigs);
  strcpy(outname,filename);
  strcat(outname,".sr");
  fp=Fopen(outname,"w");
  if (signals) 
    print_state_vector(fp,signals,nsigs,ninpsig);
  fprintf(fp,"EVENT : EXCITATION REGION : SWITCHING REGION\n");
  for (i=ninpsig;i<nsigs;i++) 
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link){
	if (l==1) fprintf(fp,"+"); else fprintf(fp,"-");
	fprintf(fp,"%s : %s : %s\n",signals[i]->name,
		cur_region->enstate,cur_region->switching);
      }
  fclose(fp);
  printf("done\n");
  fprintf(lg,"done\n");
}

/*****************************************************************************/
/* Print state vectoreader.                                                */
/*****************************************************************************/

void print_state_vector(FILE *fp,signalADT *signals,int nsignals,int ninpsig)
{
  int i;
  
  fprintf(fp,"STATEVECTOR:");
  for (i=0;i<nsignals;i++) {
    if (i<ninpsig)
      fprintf(fp,"INP ");
    fprintf(fp,"%s ",signals[i]->name);
  }
  fprintf(fp,"\n");
}

/*****************************************************************************/
/* Find value of a state bit (0 and R return 0 and 1 and F return 1).        */
/*****************************************************************************/

char VAL(char bit)
{
  if ((bit=='0') || (bit=='R')/*|| (bit=='U')*/) return('0');
  else if ((bit=='1') || (bit=='F')/*|| (bit=='D')*/) return('1');
  return('X');
}

/*****************************************************************************/
/* Find value of a state bit (0 and R return 0 and 1 and F return 1).        */
/*****************************************************************************/

char eval(char bit)
{
  if ((bit=='0') || (bit=='R') || (bit=='U')) return('0');
  else if ((bit=='1') || (bit=='F') || (bit=='D')) return('1');
  return('X');
}

bool enstate_includes(char * enstate,char * state,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if ((enstate[i]!='X') &&
	(enstate[i]!=VAL(state[i]))) return(FALSE);
  return(TRUE);
}

bool enstate_includes2(char * enstate,char * state,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if ((enstate[i]!='X') && (enstate[i]!='Z') && (VAL(state[i])!='X') &&
	(enstate[i]!=VAL(state[i]))) return(FALSE);
  return(TRUE);
}

void union_enstate(char * new_enstate,char * old_enstate,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++) {
    if ((new_enstate[i]=='Z') && (old_enstate[i]!='Z'))
      new_enstate[i]=VAL(old_enstate[i]);
    else if ((old_enstate[i]!='Z') && 
	     (VAL(new_enstate[i]) != VAL(old_enstate[i])))
      new_enstate[i]='X';
  }
}

void union_enstate2(char * new_enstate,char * old_enstate,int ninpsig,
		    int nsignals)
{
  int i;
  char oldbit,newbit;

  for (i=0;i<ninpsig;i++) {
    if ((new_enstate[i]=='Z') && (old_enstate[i]!='Z'))
      new_enstate[i]=VAL(old_enstate[i]);
    else if ((old_enstate[i]!='Z') && 
	     (VAL(new_enstate[i]) != VAL(old_enstate[i])))
      new_enstate[i]='X';
  }
  for (i=ninpsig;i<nsignals;i++) {
    if (old_enstate[i]=='D') oldbit='1';
    else if (old_enstate[i]=='U') oldbit='0';
    else oldbit=old_enstate[i];
    if (new_enstate[i]=='D') newbit='1';
    else if (new_enstate[i]=='U') newbit='0';
    else newbit=new_enstate[i];

    if ((newbit=='Z') && (oldbit!='Z'))
      new_enstate[i]=VAL(oldbit);
    else if ((oldbit!='Z') && (VAL(newbit) != VAL(oldbit))) 
      new_enstate[i]='X';
  }
}

void union_enablings(char * new_enablings,char * old_enablings,int nevents)
{
  int i;

  if (new_enablings && old_enablings)
    for (i=0;i<(nevents-1)/2;i++)
      if ((new_enablings[i]=='T') || (old_enablings[i]=='T'))
	new_enablings[i]='T';
}

/*****************************************************************************/
/* Delete a region from a list of regions.                                   */
/*****************************************************************************/

regionADT delete_region(regionADT first_region,regionADT prev_region,
			regionADT cur_region,regionADT next_region)
{
  free(cur_region->enstate);
  free(cur_region->enablings);
  free(cur_region->tsignals);
  free(cur_region->cover);
  free(cur_region);
  if (prev_region==NULL) return(next_region);
  else prev_region->link=next_region;
  return(first_region);
}

/*****************************************************************************/
/* Add a new region to list of regions.                                      */
/*****************************************************************************/

regionADT add_region(char * enstate,char * enablings,
		     char * tsignals,char * cover,regionADT next)
{
  regionADT new_region=NULL;

  new_region=(regionADT)GetBlock(sizeof(*new_region));
  new_region->enstate=CopyString(enstate);
  new_region->enablings=CopyString(enablings);
  new_region->tsignals=CopyString(tsignals);
  new_region->cover=CopyString(cover);
  new_region->covers=NULL;
  new_region->switching=NULL;
  new_region->primes=NULL;
  new_region->implied=NULL;
  new_region->nvprimes=0;
  new_region->context_table=NULL;
  new_region->decomp=NULL;
  new_region->link=next;
  return(new_region);
}

/*****************************************************************************/
/* Recompute enabled cube for split region.                                  */
/*****************************************************************************/

bool recompute_enstate(stateADT *state_table,int nsignals,char * new_region,
		       char * new_tsignals,regionADT cur_region,int k,int m)
{
  int i;
  //  int j;
  stateADT curstate;
  bool useful;

  useful=FALSE;
  for (i=0;i<nsignals;i++)
    new_region[i]='Z';
  new_region[nsignals]='\0';
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      if ((((curstate->state[k]=='R' || curstate->state[k]=='U') && m==1) || 
	   ((curstate->state[k]=='F' || curstate->state[k]=='D') && m==2)) && 
	  (enstate_includes2(cur_region->enstate,curstate->state,nsignals))
	  && (enstate_includes2(new_tsignals,curstate->state,nsignals))) {
	union_enstate(new_region,curstate->state,nsignals);
	//for (j=0;j<nsignals;j++)
	//  if (new_tsignals[j]!='Z')
	//    new_region[j]=new_tsignals[j];
	//if (new_region[k]=='X')
	//  if (m==1) new_region[k]='0';
	//  else new_region[k]='1';
	useful=TRUE;
      }
  return useful;
}

bool find_region(regionADT regions,char * new_region)
{
  regionADT cur_region;
  
  for (cur_region=regions;cur_region;cur_region=cur_region->link) 
    if (strcmp(cur_region->enstate,new_region)==0) 
      return TRUE;
  return FALSE;
}

bool subset_region(char * new_region,char * prime_region,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++) 
    if (((new_region[i]=='0')&&(prime_region[i]=='1')) ||
	((new_region[i]=='1')&&(prime_region[i]=='0')) ||
	((new_region[i]=='X')&&(prime_region[i]!='X')))
      return FALSE;
  return TRUE;
}
  
/*****************************************************************************/
/* Split region to solve non-persistency of trigger signals.                 */
/*****************************************************************************/

bool split_region(stateADT *state_table,regionADT *regions,int nsignals,
		  int k,int m,char * new_region,char * new_enablings,
		  char * new_tsignals,regionADT cur_region,char * prime_region)
{
  int i;
  char oldval;
  bool split;

  split=FALSE;
  strcpy(new_enablings,cur_region->enablings);
  for (i=0;i<nsignals;i++)
    if (cur_region->enstate[i]=='X') {
      oldval=new_tsignals[i];
      /*if (cur_region->tsignals[i]!='0') */{
	new_tsignals[i]='0';
	if (recompute_enstate(state_table,nsignals,new_region,new_tsignals,
			      cur_region,k,m)) {
	  split=TRUE;
	  if (/*(!prime_region)||
		(!subset_region(new_region,prime_region,nsignals))
		&&*/ (!find_region(regions[2*k+m],new_region))) {
	    regions[2*k+m]=add_region(new_region,new_enablings,new_tsignals,
				      new_tsignals,regions[2*k+m]);
	  }
	}
      }
      /*if (cur_region->tsignals[i]!='1') */{
	new_tsignals[i]='1';
	if (recompute_enstate(state_table,nsignals,new_region,new_tsignals,
			      cur_region,k,m)) {
	  split=TRUE;
	  if (/*(!prime_region)||
		(!subset_region(new_region,prime_region,nsignals))
		&&*/ (!find_region(regions[2*k+m],new_region))) {
	    regions[2*k+m]=add_region(new_region,new_enablings,new_tsignals,
				      new_tsignals,regions[2*k+m]);
	  }
	}
      }
      new_tsignals[i]=oldval;
    }
  return split;
}

/*****************************************************************************/
/* Check persistency of trigger signals.                                     */
/*****************************************************************************/

bool check_persistency(char * filename,signalADT *signals,eventADT *events,
		       ruleADT **rules,stateADT *state_table,
		       regionADT *regions,int ninpsig,int nsignals,int ninputs,
		       int nevents,bool verbose,char * new_region,
		       char * new_enablings,char * new_tsignals,bool multicube,
		       bool exception)
{
  char outname[FILENAMESIZE];
  FILE *errfp=NULL;
  int i,k,l,m;
  bool okay,split,useful;
  regionADT cur_region,prev_region,next_region;
  regionADT cregion,pregion,nregion;
  char * prime_region;

  prime_region=(char *)GetBlock((nsignals+1)*sizeof(char));
  okay=TRUE;
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++) {
      prev_region=NULL;
      cur_region=regions[2*i+l];
      while (cur_region) {
	split=FALSE;
	next_region=cur_region->link;
	for (k=0;k<nsignals;k++)
	  if ((cur_region->tsignals[k] != 'Z') &&
	      (cur_region->enstate[k] != cur_region->tsignals[k])) {
	    if (okay) {
	      if (!verbose) {
		printf("EXCEPTION!\n");
		fprintf(lg,"EXCEPTION!\n");
		printf("EXCEPTION:  Persistence rule violation!\n");
		fprintf(lg,"EXCEPTION:  Persistence rule violation!\n");
	      } else {
		strcpy(outname,filename);
		strcat(outname,".err");
		printf("EXCEPTION:  Storing persistency violations to:  %s\n",
		       outname);
	fprintf(lg,"EXCEPTION:  Storing persistency violations to:  %s\n",
			outname);
		errfp=Fopen(outname,"w");
	      }
	      okay=FALSE;
	    }
	    if (verbose) 
	      if (l==1)
		fprintf(errfp,"The signal %s is non-persistent for %s+\n",
			signals[k]->name,signals[i]->name);
	      else
		fprintf(errfp,"The signal %s is non-persistent for %s-\n",
			signals[k]->name,signals[i]->name);
	    split=TRUE;
	  }
	if ((split) && (!multicube && exception)) {
	  for (m=0;m<nsignals;m++)
	    new_tsignals[m]='Z';
	  new_tsignals[nsignals]='\0';
	  useful=recompute_enstate(state_table,nsignals,prime_region,
				   cur_region->tsignals,cur_region,i,l);
	  split_region(state_table,regions,nsignals,i,l,new_region,
		       new_enablings,new_tsignals,cur_region,prime_region);
	  strcpy(cur_region->enstate,prime_region);
	  if (!useful) {
	    pregion=NULL;
	    for (cregion=regions[2*i+l];cregion && cregion!=cur_region;
		 cregion=cregion->link) pregion=cregion;
	    if (cregion==cur_region) {
	      nregion=cregion->link;
	      regions[2*i+l]=delete_region(regions[2*i+l],pregion,
					   cregion,nregion);
	    }
	  }
	  /*
	  for (prev_region=regions[2*i+l];
	       prev_region && prev_region->link!=cur_region;
	       prev_region=prev_region->link);
	  regions[2*i+l]=delete_region(regions[2*i+l],prev_region,
				       cur_region,next_region);
	  */
	} 
	cur_region=next_region;
      }
    }
  if ((!okay) && (verbose)) fclose(errfp); 
  free(prime_region);
  return okay;
}

/*****************************************************************************/
/* Print enabled cubes and trigger cubes.                                    */
/*****************************************************************************/

void print_regions(FILE *fp,signalADT *signals,regionADT *regions,
		   int ninpsig,int nsignals) 
{
  regionADT cur_region;
  int i,l;

  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link){
	if (l==1) fprintf(fp,"+"); else fprintf(fp,"-");
	fprintf(fp,"%s : %s : %s\n",signals[i]->name,
		cur_region->enstate,cur_region->tsignals);
      } 
}

void check_levels(signalADT *signals,stateADT curstate,char * new_region,int k,
		  int nsignals)
{
  bool falling;
  statelistADT nextstate,nextnext;
  int n;

  falling=FALSE;
  if ((curstate->state[k]=='R')||(curstate->state[k]=='1')||
      (curstate->state[k]=='U')) 
    for (nextstate=curstate->succ;nextstate;
	 nextstate=nextstate->next)
      for (nextnext=nextstate->stateptr->succ;nextnext;
	   nextnext=nextnext->next)
	if ((nextnext->stateptr->state[k]=='F')||
	    (nextnext->stateptr->state[k]=='D')) {
	falling=TRUE;
	break;
      }
  if (falling)
    for (n=0;n<nsignals;n++)
      if (signals[n]->is_level)
	new_region[n]='X';
}

/*****************************************************************************/
/* Find regions (i.e., enabled states and trigger signals for each region).  */
/*****************************************************************************/

bool find_reg(char * filename,signalADT *signals,eventADT *events,
	      ruleADT **rules,stateADT *state_table,regionADT *regions,
	      int ninpsig,int nsignals,int ninputs,int nevents,bool verbose,
	      bool multicube,bool exception,bool burstgc,bool burst2l,
	      bool gatelevel,bool atomic,bool sync)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int i,j,k,l,m;
  stateADT curstate;
  statelistADT predstate;
  bool okay,deleted;
  char * new_region;
  char * new_enablings;
  char * new_tsignals;
  regionADT cur_region;
  regionADT prev_region;
  regionADT next_region;
  timeval t0,t1;
  double time0, time1;

  gettimeofday(&t0, NULL);
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".es");
    printf("Finding regions and storing to:  %s\n",outname);
    fprintf(lg,"Finding regions and storing to:  %s\n",outname);
    fp=Fopen(outname,"w"); 
    fprintf(fp,"REGIONS:\n");
    print_state_vector(fp,signals,nsignals,ninpsig);
    fprintf(fp,"EVENT : ENABLED CUBE : TRIGGER CUBE \n");
  } else {
    printf("Finding regions ... ");
    fflush(stdout);
    fprintf(lg,"Finding regions ... ");
    fflush(lg);
  }
  okay=TRUE;

  new_region=(char *)GetBlock((nsignals+1)*sizeof(char));
  new_enablings=(char *)GetBlock((((nevents-1)/2)+1)*sizeof(char));
  new_tsignals=(char *)GetBlock((nsignals+1)*sizeof(char));

  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) 
      for (k=ninpsig;k<nsignals;k++) { 
	if ((curstate->state[k]=='R') || (curstate->state[k]=='U') ||
	    ((!burst2l && !atomic && !sync) && 
	     ((curstate->state[k]=='F') || (curstate->state[k]=='D')))) {
	  if ((curstate->state[k]=='R')||(curstate->state[k]=='U')) 
	    m=1; 
	  else m=2;
	  for (l=0;l<nsignals;l++) 
	    if ((burstgc)&&(!sync)&&(l>=ninpsig)) {
	      if (curstate->state[l]=='U')
		new_region[l]='0';
	      else if (curstate->state[l]=='D')
		new_region[l]='1';
	      else
		new_region[l]=VAL(curstate->state[l]);
	    } else if (sync) {
	      if (curstate->state[l]=='U')
		new_region[l]='1';
	      else if (curstate->state[l]=='D')
		new_region[l]='0';
	      else
		new_region[l]=VAL(curstate->state[l]);
	    } else
	      new_region[l]=VAL(curstate->state[l]);
	  new_region[l]='\0';
	  /*
	  if (burst && gatelevel)
	    check_levels(signals,curstate,new_region,k,nsignals); 
	  */
	  for (l=0;l<(nevents-1)/2;l++) 
	    if (curstate->enablings)
	      new_enablings[l]=curstate->enablings[l];
	  new_enablings[l]='\0';
	  for (l=0;l<nsignals;l++) new_tsignals[l]='Z';
	  new_tsignals[l]='\0';
	  cur_region=regions[2*k+m];
	  prev_region=NULL;
	  if (cur_region) {
	    while (cur_region) {
	      deleted=FALSE;
	      next_region=cur_region->link;
	      if (enstate_includes(cur_region->enstate,
				   curstate->state,nsignals)) {
		if (burstgc || burst2l) {
		  union_enstate2(new_region,cur_region->enstate,ninpsig,
				 nsignals);
		} else
		  union_enstate(new_region,cur_region->enstate,nsignals);
		union_enablings(new_enablings,cur_region->enablings,nevents);
		union_enstate(new_tsignals,cur_region->tsignals,nsignals);
		for (predstate=curstate->pred;predstate;
		     predstate=predstate->next) {
		  if (predstate->stateptr->state[k]==curstate->state[k]) {
		    if (burstgc || burst2l) {
		      union_enstate2(new_region,predstate->stateptr->state,
				    ninpsig,nsignals);
		    } else
		      union_enstate(new_region,predstate->stateptr->state,
				    nsignals);
		    union_enablings(new_enablings,
				    predstate->stateptr->enablings,nevents);
		  } else 
		    for (l=0;l<nsignals;l++)
		      if (!events || 
			  !(events[signals[l]->event]-> type & DUMMY))
			if ((eval(curstate->state[l]) != 
			     eval(predstate->stateptr->state[l]))/* &&
			  (VAL(curstate->state[l]) != 'X') && 
			  (VAL(predstate->stateptr->state[l]) != 'X')*/)
			  new_tsignals[l]=VAL(curstate->state[l]);
/*		      else new_tsignals[l]=cur_region->tsignals[l]; */
		}
		deleted=TRUE;
		regions[2*k+m]=delete_region(regions[2*k+m],prev_region,
					     cur_region,next_region);
	      } else 
		for (predstate=curstate->pred;predstate;
		     predstate=predstate->next)
		  if (predstate->stateptr->state[k]==curstate->state[k]) {
		    if (enstate_includes(cur_region->enstate,
					 predstate->stateptr->state,nsignals)){
		      if (burstgc || burst2l) {
			union_enstate2(new_region,cur_region->enstate,ninpsig,
				       nsignals);
		      } else
			union_enstate(new_region,cur_region->enstate,nsignals);
		      union_enablings(new_enablings,cur_region->enablings,
				      nevents);
		      union_enstate(new_tsignals,cur_region->tsignals,
				    nsignals);
		      deleted=TRUE;
		      regions[2*k+m]=delete_region(regions[2*k+m],prev_region,
						   cur_region,next_region);
		      break;
		    } else {
		      if (burstgc || burst2l) {
			union_enstate2(new_region,predstate->stateptr->state,
				       ninpsig,nsignals);
		      } else
			union_enstate(new_region,predstate->stateptr->state,
				      nsignals);
		      union_enablings(new_enablings,
				      predstate->stateptr->enablings,nevents);
		    }
		  } else {
		    for (l=0;l<nsignals;l++)
		      if (!events || 
			  !(events[signals[l]->event]-> type & DUMMY))
			if ((eval(curstate->state[l]) != 
			     eval(predstate->stateptr->state[l])) &&
			    (predstate->stateptr->state[l]!='X')/*&&
			  (VAL(curstate->state[l]) != 'X') && 
			  (VAL(predstate->stateptr->state[l]) != 'X')*/)
			  new_tsignals[l]=VAL(curstate->state[l]);
		  }
	      if (!deleted) prev_region=cur_region;
	      cur_region=next_region;		      
	    }
	  } else 
	    for (predstate=curstate->pred;predstate;
		 predstate=predstate->next) {
	      if (predstate->stateptr->state[k]==curstate->state[k]) {
		if (burstgc || burst2l) {
		  union_enstate2(new_region,predstate->stateptr->state,
				 ninpsig,nsignals);
		} else
		  union_enstate(new_region,predstate->stateptr->state,
				nsignals);
		union_enablings(new_enablings,predstate->stateptr->enablings,
			      nevents);
	      } else 
		for (l=0;l<nsignals;l++)
		  if (!events || !(events[signals[l]->event]-> type & DUMMY))
		    if ((eval(curstate->state[l]) != 
			 eval(predstate->stateptr->state[l]))  &&
			(predstate->stateptr->state[l]!='X')/*&&
		      (VAL(curstate->state[l]) != 'X') && 
		      (VAL(predstate->stateptr->state[l]) != 'X')*/)
		      new_tsignals[l]=VAL(curstate->state[l]);
	    }
	  regions[2*k+m]=add_region(new_region,new_enablings,new_tsignals,
				    new_tsignals,regions[2*k+m]);
	} else { 
	  if (((burst2l)||(atomic)||(sync)) && 
	      (curstate->state[k]=='1')) {
	    for (l=0;l<nsignals;l++) 
	      if ((burstgc)&&(!sync)&&(l>=ninpsig)) {
		if (curstate->state[l]=='U')
		  new_region[l]='0';
		else if (curstate->state[l]=='D')
		  new_region[l]='1';
		else
		  new_region[l]=VAL(curstate->state[l]);
	      } else if (sync) {
		if (curstate->state[l]=='U')
		  new_region[l]='1';
		else if (curstate->state[l]=='D')
		  new_region[l]='0';
		else
		  new_region[l]=VAL(curstate->state[l]);
	      } else 
		new_region[l]=VAL(curstate->state[l]);
	    new_region[l]='\0';
	    /*
	    if (burst && gatelevel)
	      check_levels(signals,curstate,new_region,k,nsignals); 
	    */
	    for (l=0;l<(nevents-1)/2;l++) 
	      if (curstate->enablings)
		new_enablings[l]=curstate->enablings[l];
	    new_enablings[l]='\0';
	    for (l=0;l<nsignals;l++) new_tsignals[l]='Z';
	    new_tsignals[l]='\0';
	    if ((!(curstate->succ)) ||
		(curstate->succ->stateptr->state[k]!='D')) {
	      /*
	      for (predstate=curstate->pred;predstate;
		   predstate=predstate->next) 
		for (l=0;l<nsignals;l++)
		  if ((eval(curstate->state[l]) != 
		       eval(predstate->stateptr->state[l]))  &&
		      (predstate->stateptr->state[l]!='X'))
		    new_tsignals[l]=VAL(curstate->state[l]);
	      */
	      regions[2*k+1]=add_region(new_region,new_enablings,
					new_tsignals,new_tsignals,
					regions[2*k+1]);
	    } else {
	      for (l=0;l<nsignals/*ninpsig*/;l++)
		if ((curstate->state[l]=='U') && 
		    (curstate->succ->stateptr->state[l]=='1')) {
		  new_region[l]='0';
		  new_tsignals[l]='0';
		  regions[2*k+1]=add_region(new_region,new_enablings,
					    new_tsignals,new_tsignals,
					    regions[2*k+1]);
		  new_region[l]='X';
		  new_tsignals[l]='Z';
		} else if ((curstate->state[l]=='D') && 
			   (curstate->succ->stateptr->state[l]=='0')) {
		  new_region[l]='1';
		  new_tsignals[l]='1';
		  regions[2*k+1]=add_region(new_region,new_enablings,
					    new_tsignals,new_tsignals,
					    regions[2*k+1]);
		  new_region[l]='X';
		  new_tsignals[l]='Z';
		}
	    }
	  }
	}
      }
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++) {
      cur_region=regions[2*i+l];
      prev_region=NULL;
      while (cur_region) {
	deleted=FALSE;
	next_region=cur_region->link;
	if (cur_region->enstate[0]=='Z') {
	  regions[2*i+l]=delete_region(regions[2*i+l],prev_region,
				       cur_region,next_region);
	  deleted=TRUE;
	}
	if (!deleted) prev_region=cur_region;
	cur_region=next_region;		      
      } 
    }
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++) 
      for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link) {
	if ((!burst2l)&&(!atomic)) {
	  if (l==1) cur_region->enstate[i]='0';
	  else cur_region->enstate[i]='1';
	}
	for (j=0;j<nsignals;j++)
	  if (cur_region->tsignals[j]=='X') cur_region->tsignals[j]='Z';
      }
  okay=check_persistency(filename,signals,events,rules,state_table,regions,
			 ninpsig,nsignals,ninputs,nevents,verbose,
			 new_region,new_enablings,new_tsignals,
			 multicube,exception);
  if (verbose)
    print_regions(fp,signals,regions,ninpsig,nsignals); 

  /* REGION REDUNDANT RULE CHECK???
  first=TRUE;
  for (i=1;i<nevents;i++) 
    for (j=ninputs+1;j<nevents;j++)
      if (rules[i][j]->ruletype) {
	if ((j % 2)==1) l=1; else l=2;
	red=TRUE;
	if (events[j]->signal>=0)
	  for (cur_region=regions[2*events[j]->signal+l];cur_region;
	       cur_region=cur_region->link) {
	    k=events[i]->signal;
	    if (cur_region->tsignals[k] != 'Z') red=FALSE;
	  }
	if (red) {
	  rules[i][j]->ruletype=(rules[i][j]->ruletype | REDUNDANT);
	  if ((first) & (verbose)) {
	    strcpy(outname,filename);
	    strcat(outname,".rr");
	    printf("Storing redundant rules to:  %s\n",outname);
	    fprintf(lg,"Storing redundant rules to:  %s\n",outname);
	    errfp=Fopen(outname,"w");
	    first=FALSE;
	  }
	  if (verbose)
	    fprintf(errfp,"The rule between %s and %s is redundant\n",
		    events[i]->event,events[j]->event);
	}
      }
  if ((!first) && (verbose)) fclose(errfp); 
  */
  free(new_region);
  free(new_enablings);
  free(new_tsignals);

  if (verbose) fclose(fp);
  else if (okay) {
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("done (%f)\n",time1-time0);
    fprintf(lg,"done (%f)\n",time1-time0);
  }
  if (exception) 
    return TRUE;
  return(okay);
}

/*****************************************************************************/
/* Find regions.                                                             */
/*****************************************************************************/

bool find_regions(char command,designADT design)
{
  bool verbose;

  verbose=design->verbose;
  if (command==FINDREG) verbose=TRUE;
  if (!(design->status & FOUNDREG)) {
    if (!find_reg(design->filename,design->signals,design->events,
		  design->rules,design->state_table,design->regions,
		  design->ninpsig,design->nsignals,design->ninputs,
		  design->nevents,verbose,design->multicube,
		  design->exception,design->burstgc,design->burst2l,
		  design->gatelevel,design->atomic,
		  ((design->atomic)&&(design->burstgc)))) {
      if (design->multicube) return(TRUE);
      if ((command==ADDPERS) /* || (design->exception)*/) {
	verbose=design->verbose;
	if (command==ADDPERS) verbose=TRUE;
	if (!add_persistence_rules(design->filename,design->events,
				   design->rules,design->markkey,
				   design->cycles,design->nevents,
				   design->ninputs,&(design->nrules),
				   design->ncycles,design->maxgatedelay,
				   &(design->ncausal),
				   design->startstate,verbose)) return(FALSE);
	notreachable--;
	design->status=design->status | FOUNDRSG | FOUNDREG;
	reinit_design(command,design,TRUE);
	design->status=design->status & (LOADED | CONNECTED);
	if (command != ADDPERS)
	  design->status=process_command(SYNTHESIS,command,design,NULL,FALSE,
					 design->si);
	return(FALSE);
      } else {
	design->regions=new_regions(design->status,FALSE,
				    (2*design->nsignals)+1,
				    (2*design->nsignals)+1,design->regions);
	return(FALSE);
      }
    }
    design->status=design->status | FOUNDREG;
  }
  return(TRUE);
}

