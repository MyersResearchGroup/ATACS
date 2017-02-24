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
/* breakup.c                                                                 */
/*****************************************************************************/

#include "breakup.h"
#include "findwctd.h"
#include "interface.h"

/*****************************************************************************/
/* Output event list.                                                        */
/*****************************************************************************/

void output_events(FILE *fpI,FILE *fpO,eventADT *events,
		   int nevents,int ninputs)
{
  int i;

  for (i=1;i<=ninputs;i+=2)
    fprintf(fpI,"%s %s\n",events[i]->event,events[i+1]->event);
  for (i=ninputs+1;i<nevents;i+=2)
    fprintf(fpO,"%s %s\n",events[i]->event,events[i+1]->event);
}

/*****************************************************************************/
/* Output merger list.                                                       */
/*****************************************************************************/

int output_mergers(FILE *fpD,eventADT *events,mergeADT *merges,int nevents)
{
  int i;
  int ndisj;
  mergeADT curmerge;

  ndisj=0;
  for (i=1;i<nevents;i++) {
    for (curmerge=merges[i]; curmerge != NULL; curmerge=curmerge->link)
      if ((curmerge->mergetype==CONJMERGE) || 
	  (curmerge->mergetype==DISJMERGE)) 
	if (curmerge->mevent > i) {
	  fprintf(fpD,"%s %s\n",events[i]->event,
		  events[curmerge->mevent]->event);
	  ndisj++;
	}
  }
  return(ndisj);
}

/*****************************************************************************/
/* Find region associated with an event.                                     */
/*****************************************************************************/

regionADT event2region(regionADT *regions,int event,int signal)
{
  regionADT cur_region;
  
  for (cur_region=regions[2*signal+(2-(event % 2))];
       cur_region; cur_region=cur_region->link) {
    if (cur_region->enablings[(event-1)/2]=='T')
      return(cur_region);
  }
  return(NULL);
}

/*****************************************************************************/
/* Find region associated with an event.                                     */
/*****************************************************************************/

regionADT next_event2region(regionADT region,int event)
{
  regionADT cur_region;
  
  for (cur_region=region->link;cur_region; cur_region=cur_region->link) {
    if (cur_region->enablings[(event-1)/2]=='T')
      return(cur_region);
  }
  return(NULL);
}

/*****************************************************************************/
/* Check if a decomposition is redundant.                                    */
/*****************************************************************************/

bool redundant_decomp(int j,eventADT *events,
		      ruleADT **rules,cycleADT ****cycles,
		      int nevents,int ninputs,int ncycles,
		      regionADT cur_region,char * startstate)
{
  boundADT timediff=NULL;
  int i,k,eps,eps2,curliteral;
  bool last;
  char newvar[MAXTOKEN];

/*  cur_region=event2region(regions,j,events[j]->signal); */
  if ((j > ninputs) && (cur_region->decomp->statevar)) 
    if (strchr(events[j]->event,'/'))
      sprintf(newvar,"%s%s",cur_region->decomp->statevar,
	      strchr(events[j]->event,'/'));
    else sprintf(newvar,"%s",cur_region->decomp->statevar);
  else newvar[0]='\0';
  curliteral=1;
  for (i=1;i<nevents;i++)
    if ((((rules[i][j]->ruletype & TRIGGER) ||
	  (rules[i][j]->ruletype & PERSISTENCE)) &&
	 (!(rules[i][j]->ruletype & REDUNDANT))) || 
	((((cur_region->cover[events[i]->signal]=='0') && (i % 2==0)) ||
	  (cur_region->cover[events[i]->signal]=='1') && (i % 2==1)))) {
      if ((newvar[0]!='\0') && (cur_region->decomp->curdecomp & curliteral)) {
	  eps=rules[i][j]->epsilon;
	  if (eps==(-1))
	    if (cur_region->cover[events[i]->signal]==
		startstate[events[i]->signal])
	      eps=1;
	  for (k=i+1;k<nevents;k++)
	    if ((((rules[k][j]->ruletype & TRIGGER) ||
		  (rules[k][j]->ruletype & PERSISTENCE)) &&
		 (!(rules[k][j]->ruletype & REDUNDANT))) || 
		((((cur_region->cover[events[k]->signal]=='0') && (k % 2==0))||
		  (cur_region->cover[events[k]->signal]=='1') && (k % 2==1))))
	      if ((newvar[0]!='\0') && (cur_region->decomp->curdecomp & 
					(2*curliteral))) {
		eps2=rules[k][j]->epsilon;
		if (eps2==(-1))
		  if (cur_region->cover[events[k]->signal]==
		      startstate[events[k]->signal])
		    eps2=1;
		if (((eps==1) || (eps2==0)) && (eps-eps2 > 0))
		  if (reachable(cycles,nevents,ncycles,i,0,k,eps-eps2)==1)
		    return(TRUE);
		if (((eps2==1) || (eps==0)) && (eps2-eps > 0))
		  if (reachable(cycles,nevents,ncycles,k,0,i,eps2-eps)==1)
		    return(TRUE);
		if (eps >= eps2)
		  timediff=WCTimeDiff(rules,cycles,nevents,ncycles,i,k,
				      eps-eps2);
		else
		  timediff=WCTimeDiff(rules,cycles,nevents,ncycles,k,i,
				      eps2-eps);
		if ((timediff->U < 0) || (timediff->L > 0)){
		  free(timediff); 
		  return(TRUE);
		} 
		free(timediff);
	      }
	}
      last=TRUE;
/*
      for (k=u+1;k<nevents;k++)
	if ((rules[k][j]->ruletype != NORULE) &&
	    (!(rules[k][j]->ruletype & REDUNDANT)) &&
	    are_merged(merges,u,k)) last=FALSE;
*/
      if (last) curliteral=2*curliteral;
    }
  return(FALSE);
}

/*****************************************************************************/
/* Check if a decomposition is valid.  A decomposition is valid if it        */
/*   contains at least 2 literals, contains no more than "maxstack" literals,*/
/*   and it doesn't include all the causal signals of the gate being         */
/*   decomposed.                                                             */
/*****************************************************************************/

bool valid_decomp(bool context,int curdecomp,int numliterals,int causal,
		  int maxstack,eventADT *events,ruleADT **rules,
		  cycleADT ****cycles,regionADT cur_region,int nevents,
		  int ninputs,int ncycles,int j,char * startstate)
{
  int i,k;
  int literals;
  int curlit;
  bool nonred;

  literals=0;
  curlit=1;
  if ((curdecomp & causal)==causal) {
/*    printf("%d all causal\n",curdecomp); */
    return(FALSE);
  }
  if (context && (curdecomp & causal)) {
/*    printf("%d context only\n",curdecomp); */
    return(FALSE);
  }
  for (i=0;i<numliterals;i++) {
    if (curlit & curdecomp) literals++;
/* ???
    if (literals > maxstack) {
      printf("%d greater than maxsize\n",curdecomp); 
      return(FALSE); 
    }
*/
    curlit=2*curlit;
  }
  if (numliterals <= maxstack) return(TRUE);

  if ((!context) && ((curdecomp | causal) != causal)) {
/*    printf("%d causal only\n",curdecomp); */
    return(FALSE);
  }

  if (literals < 1) return(FALSE);
  nonred=FALSE;
  for (k=j;(k<nevents && events[k]->signal==events[j]->signal);k+=2)
    if (cur_region->enablings[(k-1)/2]=='T')
      if (!redundant_decomp(k,events,rules,cycles,nevents,ninputs,ncycles,
			    cur_region,startstate))
	nonred=TRUE;
  if (!nonred) return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Initialize a decomposition structure: allocate memory, determine number   */
/*   of literals in initial guard, and record which are causal signals.      */
/*****************************************************************************/

costADT init_decomp1(int ninpsig,int nsignals,regionADT *regions,int maxstack,
		     int orignsigs,regionADT *origregions)
{
  int i,j,l;
  int cursignal;
  regionADT cur_region;
  costADT cost=NULL;

  cost=(costADT)GetBlock(sizeof *cost);
  cost->maxfanin=0;
  cost->gates=0;
  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
           cur_region=cur_region->link) {
	cur_region->decomp=(decompADT)GetBlock(sizeof(*(cur_region->decomp)));
	cur_region->decomp->numliterals=0;
	cur_region->decomp->curdecomp=1;
	cur_region->decomp->bestdecomp=0;
	cur_region->decomp->bestliterals=0;
	cur_region->decomp->causal=0;
	cur_region->decomp->statevar=NULL;
	cur_region->decomp->statesig=(-1);
	cur_region->decomp->enabling=NULL;
	cursignal=1;
	for (j=0;j<nsignals;j++)
	  if ((cur_region->cover[j]=='0') ||
	      (cur_region->cover[j]=='1')) {
	    cur_region->decomp->numliterals++;
	    if ((cur_region->tsignals[j]=='0') ||
		(cur_region->tsignals[j]=='1'))
	      cur_region->decomp->causal=cur_region->decomp->causal+cursignal;
	    cursignal=2*cursignal;
	  }
	cur_region->decomp->bestliterals=cur_region->decomp->numliterals;
	if (cur_region->decomp->numliterals > cost->maxfanin)
	  cost->maxfanin=cur_region->decomp->numliterals;
	if (cur_region->decomp->numliterals > maxstack) 
	  cost->gates++;
      }
  for (i=ninpsig;i<orignsigs;i++)
    for (l=1;l<3;l++)
      for (cur_region=origregions[2*i+l];cur_region;
           cur_region=cur_region->link) 
	if ((regions[2*i+l]->decomp->numliterals <
	     cur_region->decomp->bestliterals) &&
	    (regions[2*(cur_region->decomp->statesig)+l]->decomp->numliterals <
	     cur_region->decomp->bestliterals)) {
	  cur_region->decomp->bestdecomp=cur_region->decomp->curdecomp;
	  if (regions[2*i+l]->decomp->numliterals >
	      regions[2*(cur_region->decomp->statesig)+l]->decomp->numliterals)
	    cur_region->decomp->bestliterals=
	      regions[2*i+l]->decomp->numliterals;
	  else 
	    cur_region->decomp->bestliterals=
	      regions[2*(cur_region->decomp->statesig)+l]->decomp->numliterals;
	}
  return(cost);
}

/*****************************************************************************/
/* Find the next valid decomposition.                                        */
/*****************************************************************************/

bool find_next_valid(bool context,decompADT decomp,int maxstack,
		     eventADT *events,ruleADT **rules,cycleADT ****cycles,
		     regionADT cur_region,int nevents,int ninputs,int ncycles,
		     int j,char * startstate)
{
  while ((!valid_decomp(context,decomp->curdecomp,decomp->numliterals,
			decomp->causal,maxstack,events,rules,cycles,
			cur_region,nevents,ninputs,ncycles,j,startstate))
	 && (decomp->curdecomp < (1 << (decomp->numliterals))))
    decomp->curdecomp++;
  if (decomp->curdecomp >= (1 << (decomp->numliterals))) return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Find initial decomposition.                                               */
/*****************************************************************************/
  
void init_curdecomp(bool context,decompADT decomp,int maxstack,
		    eventADT *events,ruleADT **rules,cycleADT ****cycles,
		    regionADT cur_region,int nevents,int ninputs,int ncycles,
		    int j,char * startstate)
{
  decomp->curdecomp=1;
  if (!find_next_valid(context,decomp,maxstack,events,rules,cycles,
		       cur_region,nevents,ninputs,ncycles,j,startstate)){
    decomp->curdecomp=1;
    if ((!context) || (!find_next_valid(FALSE,decomp,maxstack,events,rules,
					cycles,cur_region,nevents,ninputs,
					ncycles,j,startstate)))
      decomp->curdecomp=(1 << (decomp->numliterals))-1;
  }
  decomp->prevdecomp=decomp->curdecomp;
}
 
/*****************************************************************************/
/* Return true if a decomposition includes only context signals.             */
/*****************************************************************************/

bool contextdecomp(regionADT *regions,int ninpsig,int nsignals,int maxstack)
{
  int i,l;
  regionADT cur_region;

  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
           cur_region=cur_region->link) 
	if (cur_region->decomp->statevar)
	  if (((strchr(cur_region->decomp->statevar,'-')==NULL) ||
	      (cur_region->decomp->numliterals > maxstack)) &&
	      (cur_region->decomp->curdecomp & cur_region->decomp->causal)) 
	    return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Initialize all decompositions.                                            */
/*****************************************************************************/
  
void init_decomp2(bool context,eventADT *events,
		  regionADT *regions,regionADT *lastregions,
		  int ninputs,int maxstack,signalADT *signals,
		  ruleADT **rules,cycleADT ****cycles,
		  int nevents,int ncycles,int ninpsig,int nsignals,
		  char * startstate)
{
  int i,l;
  regionADT cur_region;

  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
           cur_region=cur_region->link) 
	if (cur_region->decomp->statevar) {
	  if ((strchr(cur_region->decomp->statevar,'-')==NULL) ||
	      (cur_region->decomp->numliterals > maxstack)) {
	    if ((lastregions==NULL) || 
		(lastregions[2*i+l]->decomp->numliterals > maxstack) ||
                (lastregions[2*(cur_region->decomp->statesig)+l]->
		 decomp->numliterals > maxstack))
	      init_curdecomp(context,cur_region->decomp,maxstack,
			     events,rules,cycles,cur_region,
			     nevents,ninputs,ncycles,signals[i]->event+l-1,
			     startstate);
	  } else cur_region->decomp->curdecomp=cur_region->decomp->causal; 
	}
}

/*****************************************************************************/
/* Update decomposition to next valid one to be tried.                       */
/*****************************************************************************/

bool update_decomp(bool *context,regionADT *regions,int ninputs,int maxstack,
		   regionADT *lastregions,eventADT *events,
		   int status,signalADT *signals,ruleADT **rules,
		   cycleADT ****cycles,int nevents,
		   int ncycles,int ninpsig,int nsignals,char * startstate)
{
  int i,l;
  regionADT cur_region;
  bool returnval;
  
/*****************************************************************************/
/* Comment this to search slower if exception other than > maxstack          */
/*****************************************************************************/
/*  status=status | FOUNDCOVER; */

  returnval=FALSE;

  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
	   cur_region=cur_region->link) 
	if (cur_region->decomp->statevar)
	  if ((strchr(cur_region->decomp->statevar,'-')==NULL) ||
	      (cur_region->decomp->numliterals > maxstack))
	    if ((lastregions==NULL) || 
		(lastregions[2*i+l]->decomp==NULL) ||
		(lastregions[2*i+l]->decomp->numliterals > maxstack) ||
                (lastregions[2*(cur_region->decomp->statesig)+l]->decomp==NULL)
		|| (lastregions[2*(cur_region->decomp->statesig)+l]
		    ->decomp->numliterals > maxstack)) {
	      cur_region->decomp->curdecomp++;
	      if (!find_next_valid(*context,cur_region->decomp,maxstack,
				   events,rules,cycles,cur_region,
				   nevents,ninputs,ncycles,
				   signals[i]->event+l-1,startstate)) {
		if (status & FOUNDCOVER) 
		  cur_region->decomp->curdecomp=cur_region->decomp->prevdecomp;
		else init_curdecomp(*context,cur_region->decomp,maxstack,
				    events,rules,cycles,cur_region,
				    nevents,ninputs,ncycles,
				    signals[i]->event+l-1,startstate);
	      } else if (status & FOUNDCOVER) returnval=TRUE; 
	      else return(TRUE);
	      cur_region->decomp->prevdecomp=cur_region->decomp->curdecomp;
	      /*?*/
	    }
  if (returnval) return(TRUE);
  if (*context) {
    (*context)=FALSE;
    init_decomp2(*context,events,regions,lastregions,ninputs,
		 maxstack,signals,rules,cycles,nevents,ncycles,ninpsig,
		 nsignals,startstate);
    return(TRUE);
  } 
  return(FALSE);
}

/*****************************************************************************/
/* Determine what is the next new signal to add.                             */
/*****************************************************************************/

int find_next_new_signal(eventADT *events,int nevents)
{
  int newsigs;
  char newsignal1[MAXTOKEN];
  char newsignal2[MAXTOKEN];

  newsigs=1;
  sprintf(newsignal1,"x%d+",newsigs);
  sprintf(newsignal2,"x%d+/1",newsigs);
  while ((which_event(nevents,events,newsignal1) != (-1)) ||
	 (which_event(nevents,events,newsignal2) != (-1))) {
    newsigs++;
    sprintf(newsignal1,"x%d+",newsigs);
    sprintf(newsignal2,"x%d+/1",newsigs);
  }
  return(newsigs);
}

/*****************************************************************************/
/* Create new signals which are to be added to breakup gates.                */
/*****************************************************************************/

int create_new_signals(FILE *fpO,FILE *fpD,signalADT *signals,
		       eventADT *events,int nevents,int *ndisj,
		       char * startstate,int maxstack,regionADT *regions,
		       int ninpsig,int nsignals)
{
  int i,j,k,l;
  int newsigs,nnewsigs;
  int newnevents;
  regionADT cur_region;
  bool decompose;
  char newsignal[MAXTOKEN];
  char newsignal2[MAXTOKEN];

  nnewsigs=0;
  newsigs=find_next_new_signal(events,nevents);
  newnevents=nevents;
  for (i=ninpsig;i<nsignals;i++) {
    decompose=FALSE;
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
           cur_region=cur_region->link)
	if (cur_region->decomp->numliterals > maxstack)
	  decompose=TRUE;
    if (decompose) {
      sprintf(newsignal,"x%d+",newsigs);
      for (cur_region=regions[2*i+1];cur_region;
	   cur_region=cur_region->link) { 
	cur_region->decomp->statevar=CopyString(newsignal);
	cur_region->decomp->statesig=nsignals+nnewsigs;
      }
      sprintf(newsignal,"x%d-",newsigs);
      for (cur_region=regions[2*i+2];cur_region;
	   cur_region=cur_region->link) {
	cur_region->decomp->statevar=CopyString(newsignal);
	cur_region->decomp->statesig=nsignals+nnewsigs;
      }
      for (j=1;j<=signals[i]->maxoccurrence;j++) {
	sprintf(newsignal,"x%d+/%d",newsigs,j);
	fprintf(fpO,"%s ",newsignal);
	sprintf(newsignal,"x%d-/%d",newsigs,j);
	fprintf(fpO,"%s\n",newsignal);
      }
      if (startstate != NULL) {
	startstate[nsignals+nnewsigs]=startstate[i];
	nnewsigs++;
	startstate[nsignals+nnewsigs]='\0';
      }
      newnevents=newnevents+(2*(signals[i]->maxoccurrence));
      if (signals[i]->maxoccurrence > 1)
	for (j=1;j<signals[i]->maxoccurrence;j++) {
	  if (startstate[i]=='1') sprintf(newsignal,"x%d-/%d",newsigs,j);
	  else sprintf(newsignal,"x%d+/%d",newsigs,j);
	  for (k=j+1;k<=signals[i]->maxoccurrence;k++) {
	    if (startstate[i]=='1') sprintf(newsignal2,"x%d-/%d",newsigs,k);
	    else sprintf(newsignal2,"x%d+/%d",newsigs,k);
	    fprintf(fpD,"%s %s\n",newsignal,newsignal2);
	    (*ndisj)++;
	  }
	}
      newsigs++;
    }
  }
  return(newnevents);
}

/*****************************************************************************/
/* Output a causal rule.                                                     */
/*****************************************************************************/

int output_rule(FILE *fpR,char * enabling,char * enabled,int epsilon,
		int lower,int upper,int resetlower,int resetupper,int nrules)
{
  fprintf(fpR,"%s %s %d %d ",enabling,enabled,epsilon,lower);
  printdelay(fpR,upper);
  fprintf(fpR,"\n");
  nrules++;
  if (epsilon==1) {
    fprintf(fpR,"reset %s 0 %d ",enabled,resetlower);
    printdelay(fpR,resetupper);
    fprintf(fpR,"\n");
    nrules++;
  }
  return(nrules);
}

/*****************************************************************************/
/* Output conflicts associated with a given decomposition.                   */
/*****************************************************************************/

int output_conf(FILE *fpC,int j,char * newvar,eventADT *events,
		ruleADT **rules,int nevents,int nconf)
{
  int i;

  for (i=1;i<nevents;i++)
    if (rules[i][j]->conflict) {
      nconf++;
      fprintf(fpC,"%s %s\n",events[i]->event,newvar);
    }
  return(nconf);
}

/*****************************************************************************/
/* Find enabling associated with a given decomposition.                      */
/*****************************************************************************/

void find_enabling(eventADT *events,ruleADT **rules,int ninputs,int nevents,
		   regionADT *regions)
{
  int i,j;
  int curliteral;
  int last;
  regionADT cur_region;
  bool trigger;

  for (j=1;j<nevents;j++) 
    if (j > ninputs) {
      cur_region=event2region(regions,j,events[j]->signal);
      if (cur_region) {
	do {
	  cur_region->decomp->enabling=
	    (char *)GetBlock((nevents+1)*sizeof(char));
	  for (i=1;i<nevents;i++) cur_region->decomp->enabling[i]='F';
	  cur_region->decomp->enabling[i]='\0';
	  curliteral=1;
	  last=(-1);
	  if (cur_region->decomp->statevar) {
	    trigger=FALSE;
	    for (i=1;i<nevents;i++) {
	      if ((((rules[i][j]->ruletype & TRIGGER) ||
		    (rules[i][j]->ruletype & PERSISTENCE))
		   && (!(rules[i][j]->ruletype & REDUNDANT))) || 
		  ((cur_region->cover[events[i]->signal]=='0') && 
		   (i % 2==0)) || 
		  ((cur_region->cover[events[i]->signal]=='1') && 
		   (i % 2==1))) {
		if ((last!=(-1)) && (events[i]->signal!=last)) 
		  curliteral=2*curliteral;
		last=events[i]->signal;
		if (cur_region->decomp->curdecomp & curliteral) {
		  if ((cur_region->tsignals[events[i]->signal]=='0')||
		      (cur_region->tsignals[events[i]->signal]=='1'))
		    trigger=TRUE;
		  if (!events[i]->dropped)
		    if ((!trigger) || (rules[i][j]->ruletype & TRIGGER) || 
			(rules[i][j]->ruletype & PERSISTENCE))
		      cur_region->decomp->enabling[i]='T';
		}
	      }
	    }
	  }
	  if (cur_region) cur_region=next_event2region(cur_region,j);
	} while (cur_region);
      }
    }
}

/*****************************************************************************/
/* Output rules associated with a given decomposition.                       */
/*****************************************************************************/

int output_rules(FILE *fpR,FILE *fpN,FILE *fpC,eventADT *events,
		 ruleADT **rules,int ninputs,int nevents,int *nord,
		 int *nconf,regionADT *regions,int maxgatedelay,
		 char * startstate)
{
  int i,j,k,l,m;
  int nrules;
  int curliteral;
  int last,eps;
  char newvar[MAXTOKEN];
  char newvar2[MAXTOKEN];
  regionADT cur_region;
  regionADT cur_region2;
  bool trigger;
  char * enabling;
  char * enabling2;
  bool add;

  nrules=0;
  enabling=(char *)GetBlock((nevents+1)*sizeof(char));
  enabling2=(char *)GetBlock((nevents+1)*sizeof(char));
  find_enabling(events,rules,ninputs,nevents,regions);
  for (j=1;j<nevents;j++) {
    if (j > ninputs)
      cur_region=event2region(regions,j,events[j]->signal);
    else cur_region=NULL;
    for (i=1;i<nevents;i++) enabling[i]='F';
    enabling[i]='\0';
    do {
      curliteral=1;
      last=(-1);
      if ((cur_region) && (cur_region->decomp->statevar)) 
	if (strchr(events[j]->event,'/'))
	  sprintf(newvar,"%s%s",cur_region->decomp->statevar,
		  strchr(events[j]->event,'/'));
	else sprintf(newvar,"%s",cur_region->decomp->statevar);
      else newvar[0]='\0';
      
      trigger=FALSE;
      for (i=1;i<nevents;i++) {
	if (((!cur_region) && ((rules[i][j]->ruletype & TRIGGER) ||
			       (rules[i][j]->ruletype & PERSISTENCE)) &&
	     (!(rules[i][j]->ruletype & REDUNDANT))) || 
	    ((cur_region) && (((cur_region->cover[events[i]->signal]=='0') && 
			       (i % 2==0)) || 
			      (cur_region->cover[events[i]->signal]=='1') && 
			      (i % 2==1)))) {
	  if ((last!=(-1)) && (events[i]->signal!=last)) 
	    curliteral=2*curliteral;
	  last=events[i]->signal;
	  if ((cur_region) && (newvar[0]!='\0') &&
	      (cur_region->decomp->curdecomp & curliteral)) {
	    eps=rules[i][j]->epsilon;
	    if (eps==(-1))
	      if (cur_region->cover[events[i]->signal]==
		  startstate[events[i]->signal])
		eps=1;
	      else eps=0;
	    if ((cur_region) && 
		((cur_region->tsignals[events[i]->signal]=='0') || 
		 (cur_region->tsignals[events[i]->signal]=='1')))
	      trigger=TRUE;
	    if (!events[i]->dropped)
	      if ((!trigger) || (rules[i][j]->ruletype & TRIGGER) ||
		  (rules[i][j]->ruletype & PERSISTENCE)) {
		nrules=output_rule(fpR,events[i]->event,newvar,eps,0,
				   maxgatedelay,0,maxgatedelay,nrules);
		enabling[i]='T';
	      }
	    if (!trigger)
	      (*nconf)=output_conf(fpC,i,newvar,events,rules,nevents,*nconf);
	  } else if ((rules[i][j]->ruletype & TRIGGER) ||
		     (rules[i][j]->ruletype & PERSISTENCE))
	    nrules=output_rule(fpR,events[i]->event,events[j]->event,
			       rules[i][j]->epsilon,rules[i][j]->lower,
			       rules[i][j]->upper,rules[0][j]->lower,
			       rules[0][j]->upper,nrules);
	  else if ((rules[i][j]->ruletype & ORDERING) && 
		   (rules[i][j]->ruletype & SPECIFIED))
	    (*nord)=output_rule(fpN,events[i]->event,events[j]->event,
				rules[i][j]->epsilon,0,0,0,0,*nord);
	} else if ((rules[i][j]->ruletype & TRIGGER) ||
		   (rules[i][j]->ruletype & PERSISTENCE)) {
	  nrules=output_rule(fpR,events[i]->event,events[j]->event,
			     rules[i][j]->epsilon,rules[i][j]->lower,
			     rules[i][j]->upper,rules[0][j]->lower,
			     rules[0][j]->upper,nrules);
	} else if ((rules[i][j]->ruletype & ORDERING) && 
		   (rules[i][j]->ruletype & SPECIFIED))
	  (*nord)=output_rule(fpN,events[i]->event,events[j]->event,
			      rules[i][j]->epsilon,0,0,0,0,*nord);
      }      
      if (cur_region)
	if ((newvar[0]!='\0') && 
	    (cur_region->decomp->curdecomp > 0)) {

	  if (trigger) {
	    (*nconf)=output_conf(fpC,j,newvar,events,rules,nevents,*nconf);
	    fprintf(fpR,"%s %s 0 0 ",newvar,events[j]->event);
	    printdelay(fpR,maxgatedelay);
	    fprintf(fpR,"\n");
	    nrules++;
	  } else {
	    fprintf(fpN,"%s %s 0 0 0\n",newvar,events[j]->event);
	    (*nord)++;
	  }

	  for (i=1;i<nevents;i++)
	    if (enabling[i]=='T')
	      if ((rules[i][j]->ruletype & TRIGGER) ||
		  (rules[i][j]->ruletype & PERSISTENCE))
		for (k=1;k<nevents;k++)
		  if ((rules[k][j]->conflict) && 
		      ((rules[i][k]->ruletype & TRIGGER) ||
		       (rules[i][k]->ruletype & PERSISTENCE))) {
		    add=TRUE;
		    for (l=1;l<nevents;l++)
		      if ((enabling[l]=='T') &&
			  (rules[k][l]->conflict)) add=FALSE;
		    if (add)
		      nrules=output_rule(fpR,newvar,events[k]->event,0,0,
					 maxgatedelay,0,maxgatedelay,nrules);
		  }

	}  
      if (cur_region)
	cur_region=next_event2region(cur_region,j);
    } while ((j > ninputs) && (cur_region));
    if (newvar[0]!='\0')
      for (k=j+1;k<nevents;k++) 
	if (k > ninputs) {
	  cur_region2=event2region(regions,k,events[k]->signal);
	  for (i=1;i<nevents;i++) enabling2[i]='F';
	  enabling2[i]='\0';
	  if (cur_region2) {
	    do {
	      curliteral=1;
	      last=(-1);
	      if (cur_region2->decomp->statevar) {
		if (strchr(events[k]->event,'/'))
		  sprintf(newvar2,"%s%s",cur_region2->decomp->statevar,
			  strchr(events[k]->event,'/'));
		else sprintf(newvar2,"%s",cur_region2->decomp->statevar);
		trigger=FALSE;
		for (i=1;i<nevents;i++) {
		  if (cur_region2->decomp->enabling[i]=='T')
		    enabling2[i]='T';
/*
		  if ((((rules[i][k]->ruletype & TRIGGER) ||
			(rules[i][k]->ruletype & PERSISTENCE))
		       && (!(rules[i][k]->ruletype & REDUNDANT))) || 
		      ((cur_region2->cover[events[i]->signal]=='0') && 
		       (i % 2==0)) || 
		      ((cur_region2->cover[events[i]->signal]=='1') && 
		       (i % 2==1))) {
		    if ((last!=(-1)) && (events[i]->signal!=last)) 
		      curliteral=2*curliteral;
		    last=events[i]->signal;
		    if (cur_region2->decomp->curdecomp & curliteral) {
		      if ((cur_region2->tsignals[events[i]->signal]=='0')||
			  (cur_region2->tsignals[events[i]->signal]=='1'))
			trigger=TRUE;
		      if (!events[i]->dropped)
			if ((!trigger) || (rules[i][k]->ruletype & TRIGGER)
			    || (rules[i][k]->ruletype & PERSISTENCE))
			  enabling2[i]='T';
		    }
		  }
*/
		}
	      }
	      if (cur_region2)
		cur_region2=next_event2region(cur_region2,k);
	    } while (cur_region2);
	    for (i=1;i<nevents;i++)
	      for (l=1;l<nevents;l++)
		if ((enabling[i]=='T') && (enabling2[l]=='T') &&
		    (rules[i][l]->conflict) && 
		    (enabling[l]=='F') && (enabling2[i]=='F')) {
	/* printf("%d %s %s %d %s %s\n",i,enabling,newvar,l,enabling2,
			 newvar2); */
		  add=TRUE;
		  for (m=1;m<nevents;m++)
		    if ((enabling[m]=='T') && (rules[i][m]->conflict) &&
			(!rules[m][l]->conflict))
		      add=FALSE;
		  if (add)
		    for (m=1;m<nevents;m++)
		      if ((enabling2[m]=='T') && (rules[l][m]->conflict) &&
			  (!rules[m][i]->conflict))
		      add=FALSE;
		  if (add) {
		    fprintf(fpC,"%s %s\n",newvar,newvar2);
		    (*nconf)++;
		  }
		}
	  }
	}
  }
  free(enabling);
  free(enabling2);
  return(nrules);
}

/*****************************************************************************/
/* Output conflicts associated with a given decomposition.                   */
/*****************************************************************************/

int output_conflicts(FILE *fpC,eventADT *events,ruleADT **rules,
		     int nevents,int nconf)
{
  int i,j;

  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if (rules[i][j]->conflict) {
	nconf++;
	fprintf(fpC,"%s %s\n",events[i]->event,events[j]->event);
      }
  return(nconf);
}
 
/*****************************************************************************/
/* Breakup gates which have guards with more literals than "maxstack".       */
/*****************************************************************************/

bool breakup_gates(char * filename,signalADT *signals,eventADT *events,
		   mergeADT *merges,
		   ruleADT **rules,cycleADT ****cycles,int nevents,int ncycles,
		   int ninputs,int maxstack,int maxgatedelay,char * startstate,
		   bool initial,regionADT *lastregions,int status,
		   regionADT *regions,bool manual,int ninpsig,int nsignals,
		   bool *error)
{
  int nrules,nconf;
  static int ndisj;
  int nord;
  static int newnevents,oldnevents=0;
  char outname[FILENAMESIZE];
  FILE *fpI,*fpO,*fpR,*fpN,*fpD,*fpC;
  static bool context;
  static char * newstartstate=NULL;
  static int iteration;
  int i,l;
  int token;
  char tokvalue[MAXTOKEN];
  regionADT cur_region;
  costADT cost;
  bool retval;

  nord=0;
  nconf=0;
  retval=TRUE;
  (*error)=FALSE;
  strcat(filename,"BRK");
  printf("Breaking up gates ... ");
  fprintf(lg,"Breaking up gates ... "); 
  fflush(stdout);
  if (initial) {
/*****************************************************************************/
/* Pull out context signals first?                                           */
/*****************************************************************************/
    context=TRUE;
    iteration=0;
    strcpy(outname,filename);
    strcat(outname,".inp");
    fpI=Fopen(outname,"w");
    strcpy(outname,filename);
    strcat(outname,".out");
    fpO=Fopen(outname,"w");
    strcpy(outname,filename);
    strcat(outname,".merg");
    fpD=Fopen(outname,"w");
    output_events(fpI,fpO,events,nevents,ninputs);
    ndisj=output_mergers(fpD,events,merges,nevents);
    oldnevents=nevents;
    cost=init_decomp1(ninpsig,nsignals,regions,maxstack,0,NULL);
/*
    printf("\nCOST: %d high-fanin gates with a maximum fanin of %d\n",
	   cost->gates,cost->maxfanin); */
    free(cost);
    if (newstartstate != NULL) {
      free(newstartstate);
      newstartstate=NULL;
    }
    if (startstate != NULL) {
      newstartstate=(char *)GetBlock(MAXSIGNALS);
      strcpy(newstartstate,startstate);
    }
    newnevents=create_new_signals(fpO,fpD,signals,events,nevents,&ndisj,
				  newstartstate,maxstack,regions,ninpsig,
				  nsignals);
    init_decomp2(context,events,regions,lastregions,ninputs,
		 maxstack,signals,rules,cycles,nevents,ncycles,ninpsig,
		 nsignals,startstate);
    if (context && !contextdecomp(regions,ninpsig,nsignals,maxstack)) {
      context=FALSE;
      init_decomp2(context,events,regions,lastregions,ninputs,
		   maxstack,signals,rules,cycles,nevents,ncycles,
		   ninpsig,nsignals,startstate);
    }
  } else
    do {
      if (!update_decomp(&context,regions,ninputs,maxstack,lastregions,events,
			 status,signals,rules,cycles,nevents,ncycles,
			 ninpsig,nsignals,startstate)) {
	for (i=ninpsig;i<nsignals;i++)
	  for (l=1;l<3;l++)
	    for (cur_region=regions[2*i+l];cur_region;
		 cur_region=cur_region->link)
	      if ((cur_region->decomp->statevar) &&
		  (cur_region->decomp->numliterals > maxstack)) {
		cur_region->decomp->curdecomp=cur_region->decomp->bestdecomp;
		if (cur_region->decomp->curdecomp==0) {
		  printf("\n");
		  fprintf(lg,"\n");
		  printf("EXCEPTION:  Unable to breakup gates!\n");
		  fprintf(lg,"EXCEPTION:  Unable to breakup gates!\n");
		  (*error)=TRUE;
		  return(FALSE);
		}
	      }
	printf("\n");
	fprintf(lg,"\n");
	printf("EXCEPTION:  Multi-level decomposition necessary!");
	fprintf(lg,"EXCEPTION:  Multi-level decomposition necessary!");
	retval=FALSE;
	break;
      }
    } while (!context && contextdecomp(regions,ninpsig,nsignals,maxstack));
  printf("\n");
  fprintf(lg,"\n");
  iteration++;
  printf("Iteration #%d\n",iteration);
  fprintf(lg,"Iteration #%d\n",iteration);
  for (i=ninpsig;i<nsignals;i++)
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;
           cur_region=cur_region->link)
	if (cur_region->decomp->statevar) {
	  if (!manual) {
	    printf("%s : current = %d maximum = %d\n",signals[i]->name,
		   cur_region->decomp->curdecomp,
		   (1 << (cur_region->decomp->numliterals)));
	    fprintf(lg,"%s : current = %d maximum = %d\n",signals[i]->name,
		   cur_region->decomp->curdecomp,
		   (1 << (cur_region->decomp->numliterals)));
	  } else {
	    do {
	      printf("%s : current = %d maximum = %d\n",signals[i]->name,
		     cur_region->decomp->curdecomp,
		     (1 << (cur_region->decomp->numliterals)));
	      while ((token=gettok(tokvalue,MAXTOKEN)) != END_OF_LINE)
		if (token==WORD) cur_region->decomp->curdecomp=atoi(tokvalue); 
	    } while (cur_region->decomp->curdecomp >= 
		     (1 << (cur_region->decomp->numliterals)));
	    printf("%s : current = %d maximum = %d\n",signals[i]->name,
		   cur_region->decomp->curdecomp,
		   (1 << (cur_region->decomp->numliterals)));
	    fprintf(lg,"%s : current = %d maximum = %d\n",events[i]->event,
		    cur_region->decomp->curdecomp,
		    (1 << (cur_region->decomp->numliterals)));
	  }
	}
  strcpy(outname,filename);
  strcat(outname,".rule");
  fpR=Fopen(outname,"w");
  strcpy(outname,filename);
  strcat(outname,".ord");
  fpN=Fopen(outname,"w");
  strcpy(outname,filename);
  strcat(outname,".conf");
  fpC=Fopen(outname,"w");
/*  if (!context) */
  nrules=output_rules(fpR,fpN,fpC,events,rules,ninputs,nevents,&nord,
		      &nconf,regions,maxgatedelay,startstate); 
/*  else 
    nrules=output_rules2(fpR,fpN,events,rules,merges,nevents,&nord,
			 regions,maxgatedelay,cycles,ncycles,startstate,
			 maxstack); */
  nconf=output_conflicts(fpC,events,rules,nevents,nconf);
  fclose(fpR);
  fclose(fpN);
  fclose(fpC);
  if (initial) {
    fclose(fpI);
    fclose(fpO);
    fclose(fpD);
  }
  make_er(filename,newnevents,ninputs,0,nrules,nord,ndisj,nconf,(char*)NULL,
	  newstartstate);
  return(retval);
}

