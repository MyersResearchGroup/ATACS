/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2002 by, Scott R. Little                                  */
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
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/

/* TBD: Fix up my pruning algorithm to make it come in line w/
   Tomohiro's algorithm */

//#define __SRL_DEBUG__
//#define __SRL_TRACE__
//#define __POSET_DEBUG__
//#define __LVL_DEBUG__
//#define __TEST_DISPLAY__
#define __TRACE_DEBUG__
//#define __CONSTRAINT_DEBUG__
#define TRACE_SIZE 4096

#include <sys/times.h>
#include <unistd.h>
#include <limits.h>
#include "srllpnrsg.h"
#include "findrsg.h"
#include "findreg.h"
#include "common_timing.h"
#include "printlpn.h"
#include "color.h"
#ifdef __TEST_DISPLAY__
#include "displayADT.h"
#endif

bool ltn;

/*****************************************************************************
* Output initial messages to the screen and open the output file if
* verbose is turned on.
******************************************************************************/
FILE *lpnInitOutput(char *filename, signalADT *signals, int nsignals,
		      int ninpsig, bool verbose)
{
  FILE *ofp = NULL;
  char outFilename[FILENAMESIZE];

  if(verbose) {
    strcpy(outFilename,filename);
    strcat(outFilename,".rsg");
    printf("Finding reduced state graph; storing to:  %s\n",outFilename);
    fprintf(lg,"Finding reduced state graph; storing to:  %s\n",outFilename);
    ofp=fopen(outFilename,"w"); 
    if (!ofp) {
      printf("Error: Could not open file %s\n",outFilename);
      fprintf(lg,"Error: Could not open file %s\n",outFilename);
      return NULL;
    }
    fprintf(ofp,"SG:\n");
    print_state_vector(ofp,signals,nsignals,ninpsig);
    fprintf(ofp,"STATES:\n");
  }
  else {
    printf("Finding reduced state graph...\n");
    fflush(stdout);
    fprintf(lg,"Finding reduced state graph...\n");
    fflush(lg);
  }
  return ofp;
}

/*****************************************************************************
* Copy the marking.
******************************************************************************/
markingADT lpnCopyMarking(markingADT oldMarking) 
{
  /* Create a copy of the marking */
  markingADT marking = (markingADT)GetBlock(sizeof *marking);
  marking->marking = CopyString(oldMarking->marking);
  marking->state = CopyString(oldMarking->state);
  marking->enablings = CopyString(oldMarking->enablings);
  marking->up = NULL;
  marking->down = NULL;
  marking->ref_count = 0;
  return marking;
}

/*****************************************************************************
* Delete the marking.
******************************************************************************/
void lpnDeleteMarking(markingADT marking)
{
  if (marking->marking) free(marking->marking);
  if (marking->enablings) free(marking->enablings);
  if (marking->state) free(marking->state);
  if (marking->up) free(marking->up);
  if (marking->down) free(marking->down);
}

/*****************************************************************************
* Copy the startstate into a marking and error if start state is missing.
******************************************************************************/
bool lpnCopyStartState(markingADT marking,char *startState,bool verbose)
{
  if(startState == NULL) { 
    if(!verbose) {
      printf("Error: No initial state information.\n");
      fprintf(lg,"Error: No initial state information.\n");
    }
    printf("Error:  Need initial state information.\n");
    fprintf(lg,"Error:  Need initial state information.\n");
    lpnDeleteMarking(marking);
    free(marking);
    return false;
  }
  else {
    strcpy(marking->state,startState);
    return true;
  }
}

/*****************************************************************************
* Check if state is consistent w/ an enabled event.
******************************************************************************/
bool lpnCheckConsistency(char *event,char *state,int signal,int ninpsig,
			   bool verbose)
{
  if(signal >= ninpsig) {
    if(((strchr(event,'+')) && (state[signal]!='0') && (state[signal]!='R'))||
       ((strchr(event,'-')) && (state[signal]!='1') && (state[signal]!='F'))){
      if(!verbose) {
	printf("Error!\n");
	fprintf(lg,"Error!\n");
      }
      printf("Error: %s enabled inconsistently in state %s\n",event,state);
      fprintf(lg,"Error: %s enabled inconsistently in state %s\n",event,state);
      return false;
    }
  }
  return true;
}

/*****************************************************************************
* Checks to see if a rule is level satisfied.
******************************************************************************/
bool lpnLevelSatisfied(ruleADT **rules,char *state, int nsignals, 
			 int enabling,int enabled)
{
  /* currently using level -- would using POS be better? */
  int i;
  bool satisfied;
  level_exp curLevel;

  if(!rules[enabling][enabled]->level) return false;

  for(curLevel=rules[enabling][enabled]->level;curLevel;
      curLevel=curLevel->next) {
    satisfied=true;
    for(i=0;i<nsignals;i++) {
      if((curLevel->product[i]=='1' && (state[i]=='0' || state[i]=='R')) ||
	 (curLevel->product[i]=='0' && (state[i]=='1' || state[i]=='F'))) {
	satisfied=false;
	break;
      }
    }
    if(satisfied) return true;
  }
  return false;
}

/*****************************************************************************
* Check if an event is enabled in a given marking
******************************************************************************/
bool lpnCheckEventEnabled(eventADT *events,ruleADT **rules,markingADT marking,
                          int event,int nevents,int nplaces,int nsignals,
                          bool chkord,bool chkassump,bool verbose,
                          bool nofail)
{
  int i;
  
  /* if it isn't a real event then it can't be enabled */
  if(events[event]->dropped) {
    return false;
  }
  
  /* check all rules for the specified event */
  for(i=0;i<nevents+nplaces;i++) {
    /* Check if valid rule considering whether we wish to check ordering
       rules and assumption rules */
    if(check_rule(rules,i,event,chkord,chkassump)) 
      /* Rule is either not marking or level satisfied so output an error */
      if((marking->marking[rules[i][event]->marking]=='F') ||
         (!lpnLevelSatisfied(rules,marking->state,nsignals,i,event))) {
	
        /* If checking ordering rules and a rule is not marking level satisfied
           it must be an ordering rule so there is a failure */
        if(chkord) {
          if(!verbose) {
            if(nofail) {
              printf("Warning!\n");
              fprintf(lg,"Warning!\n");
            }
            else {
              printf("Error!\n");
              fprintf(lg,"Error!\n");
            }
          }
          if(nofail) {
            printf("Warning: constraint (%s,%s) not satisfied in state %s.\n",
                   events[i]->event,events[event]->event,marking->state);
            fprintf(lg,"Warning: constraint (%s,%s) not satsified in state %s.\n",events[i]->event,events[event]->event,marking->state);
          }
          else {
            printf("Error: constraint (%s,%s) not satisfied in state %s.\n",
                   events[i]->event,events[event]->event,marking->state);
            fprintf(lg,"Error: constraint (%s,%s) not satsified in state %s.\n"
                    ,events[i]->event,events[event]->event,marking->state);
	    return false;
          }
        }
      }
  }  
  
  return true;
}

/*****************************************************************************
* Mark all currently enabled events true.
******************************************************************************/
void lpnEventsEnabled(eventADT *events,ruleADT **rules,markkeyADT *markkey,
		      markingADT marking,int nrules,int nsignals,int nevents,
		      bool chkassump)
{
  /* see pages 21-3 in egm thesis */
  /* an event is enabled if it is marking, level, and time satisfied */
  /* Note: time satisfied is not yet implemented */
  int i;
#ifdef __SRL_DEBUG__
  int size;
#endif
  
  
#ifdef __SRL_TRACE__
  printf("lpnEventsEnabled():Start\n");
#endif
  for(i=1;i<nevents;i++) {
    if(events[i]->dropped) {
      marking->enablings[i]='F';
    }
    else {
      marking->enablings[i]='T';
    }
  }

#ifdef __SRL_DEBUG__OFF
  size = 0;
  printf("Initial enabling: ");
  for(i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      size++;
      if(size > 1) {
        printf(",%s [%d]",events[i]->event,i);
      }
      else {
        printf(" %s [%d]",events[i]->event,i);
      }
    }
  }
  printf("\n");
#endif
  
  for(i=0;i<nrules;i++) {
    if((markkey[i]->enabled < nevents) &&
       (marking->enablings[markkey[i]->enabled]=='T') &&
       (check_rule(rules,markkey[i]->enabling,markkey[i]->enabled,false,
                   chkassump)))
      if((marking->marking[i]=='F') ||
         (!lpnLevelSatisfied(rules,marking->state,nsignals,
                             markkey[i]->enabling,
                             markkey[i]->enabled))) {
        marking->enablings[markkey[i]->enabled]='F';
      }
  }

#ifdef __SRL_DEBUG__
  size = 0;
  printf("Enabled events: ");
  for(i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      size++;
      if(size > 1) {
        printf(" ,%s [%d]",events[i]->event,i);
      }
      else {
        printf(" %s [%d]",events[i]->event,i);
      }
    }
  }
  printf("\n");
#endif
  
#ifdef __SRL_TRACE__
  printf("lpnEventsEnabled():End\n");
#endif
}

/*****************************************************************************
* Return true for a valid rule.
******************************************************************************/
bool lpnValidRule(int ruletype)
{
  return ((ruletype > NORULE) && (ruletype < REDUNDANT));
}

/*****************************************************************************/
/* Check if valid causal rule.                                               */
/*****************************************************************************/
bool lpnCausalRule(int ruletype)
{
  return ((ruletype > NORULE) && (!(ruletype & ORDERING)));
} 

/*****************************************************************************
* Find initial marking.
******************************************************************************/
markingADT lpnFindInitialMarking(eventADT *events,ruleADT **rules,
				 markkeyADT *markkey,int nevents,
				 int nrules,int ninpsig,int nsignals,
				 char * startState,bool verbose)
{
  markingADT marking = NULL;
  int i;
  
  /* allocate memory for the markingADT & its members */
  marking = (markingADT)GetBlock(sizeof *marking);
  marking->marking = (char *)GetBlock((nrules+1)*sizeof(char));
  marking->state = (char *)GetBlock((nsignals+1)*sizeof(char));
  marking->enablings = (char *)GetBlock((nevents+1)*sizeof(char));
  marking->up = NULL;
  marking->down = NULL;
  marking->ref_count = 0;

#ifdef __SRL_TRACE__
  printf("lpnFindInitialMarking():Start\n");
#endif
  
  /* find initially marked rules: valid rules && initially marked rules */
  for(i=0;i<nrules;i++) {
    if((lpnValidRule((rules[markkey[i]->enabling][markkey[i]->enabled])
		     ->ruletype))
       && (markkey[i]->epsilon == 1)) {
      marking->marking[i] = 'T';
    }
    else {
      marking->marking[i] = 'F';
    }
  }
  marking->marking[i] = '\0';

  /* marking is not initialized here b/c all values are assigned in
     lpnEventsEnabled() */
  marking->enablings[0] = 'F';
  marking->enablings[nevents]='\0';
  
  /* Copy startState as initial state, if missing abort */
  if(!lpnCopyStartState(marking,startState,verbose)) {
    return NULL;
  }
  
  lpnEventsEnabled(events,rules,markkey,marking,nrules,nsignals,nevents,
                   false);

  /* update the state for enabled events */
  for(i=1;i<nevents;i++) {
    if(marking->enablings[i] == 'T' && events[i]->signal >= 0) {
      if(!lpnCheckConsistency(events[i]->event,marking->state,
                              events[i]->signal,ninpsig,verbose)) {
        return NULL;
      }
      if(strchr(events[i]->event,'+')) {
        marking->state[events[i]->signal] = 'R';
      }
      else {
        marking->state[events[i]->signal] = 'F';
      }
    }
  }
#ifdef __SRL_TRACE__
  printf("lpnFindInitialMarking():End\n");
#endif
  return marking;
}

/*****************************************************************************
* Helper functions for the boolean vectors that are treated like a
* boolean set.
******************************************************************************/
bool *newBoolVector(int size) 
{
  bool *boolVec=(bool *)GetBlock(size*sizeof(bool));
  for (int i=0;i<size;i++)
    boolVec[i]=false;
  return boolVec;
}

void emptyBoolVector(bool *boolVec,int size) 
{
  for (int i=0;i<size;i++)
    boolVec[i]=false;
}

int sizeBoolVector(bool *boolVec,int size) 
{
  int result = 0;
  for(int i=0;i<size;i++) {
    if(boolVec[i]) {
      result++;
    }
  }
  return result;
}

void unionBoolVector(bool *boolVec1, bool *boolVec2,int size) 
{
  int i;
  for(i=0;i<size;i++) {
    if(boolVec2[i]){
      boolVec1[i] = true;
    }
  }
}

bool isEmptyBoolVector(bool *boolVec,int size) 
{
  int i;
  for(i=0;i<size;i++) {
    if(boolVec[i]) {
      return false;
    }
  }
  return true;
}

void copyBoolVector(bool *boolVec1,bool *boolVec2,int size) 
{
  int i;
  for(i=0;i<size;i++) {
    boolVec1[i] = boolVec2[i];
  }
}

/*****************************************************************************
* Helper functions for the eventTimePair vectors that are used in the
* trace back calculations.
******************************************************************************/
eventTimePairADT *newETVector(int size) 
{
  int i;
  
  eventTimePairADT *etVec =
    (eventTimePairADT *)GetBlock(size*sizeof(eventTimePairADT));
  for(i=0;i<size;i++) {
    etVec[i].event = false;
    etVec[i].minBound = 0;
  }
  return etVec;
}

void emptyETVector(eventTimePairADT *etVec,int size) 
{
  int i;
  for(i=0;i<size;i++) {
    etVec[i].event = false;
    etVec[i].minBound = 0;
  }
}

int sizeETVector(eventTimePairADT *etVec,int size) 
{
  int i,count = 0;
  for(i=0;i<size;i++) {
    if(etVec[i].event) {
      count++;
    }
  }
  return count;
}

void unionETVector(eventTimePairADT *etVec1,eventTimePairADT *etVec2,
                          int size) 
{
  int i;
  for(i=0;i<size;i++) {
    if(etVec2[i].event) {
      etVec1[i].event = true;
      etVec1[i].minBound = etVec2[i].minBound;
    }
  }
}

bool isEmptyETVector(eventTimePairADT *etVec,int size) 
{
  int i;
  for(i=0;i<size;i++) {
    if(etVec[i].event) {
      return false;
    }
  }
  return true;
}

void copyETVector(eventTimePairADT *etVec1,eventTimePairADT *etVec2,
                         int size) 
{
  int i;
  for(i=0;i<size;i++) {
    etVec1[i].event = etVec2[i].event;
    etVec1[i].minBound = etVec2[i].minBound;
  }
}

void printETVector(eventTimePairADT *etVec,int size,eventADT *events) 
{
  int i;
  for(i=0;i<size;i++) {
    if(etVec[i].event) {
      printf("e:%s mB:%d, ",events[i]->event,etVec[i].minBound);
    }
  }
  printf("\n");
}

/*****************************************************************************
* Print a warning message if an output event is disabled.
******************************************************************************/
void lpnDisabling(bool disabling,int j,char *event,bool verbose)
{
  if(disabling) {
    if (!disabled[j]) {
      printf("Warning: Output event %s is disabled\n",event);
      fprintf(lg,"Warning: Output event %s is disabled\n",event);
      disabled[j]=true;
    }
  } else {
    if(!verbose) {
      printf("Error!\n");
      fprintf(lg,"Error!\n");
    }
    printf("Error: Output event %s is disabled\n",event);
    fprintf(lg,"Error: Output event %s is disabled\n",event);
  }
}

/*****************************************************************************
* Update state after the event has fired.
* Returns true on success and false on failure.
******************************************************************************/
bool lpnUpdateState(eventADT *events,ruleADT **rules,
		    markkeyADT *markkey,markingADT marking,
		    int nevents,int nrules,int nsignals,
		    int ninpsig,bool verbose,bool disabling)
{
  int j;
  char *oldEnablings;

  /* Clear signal enablings */
  for(j=0;j<nsignals;j++)
    if(marking->state[j] == 'R')
      marking->state[j] = '0';
    else if(marking->state[j] == 'F')
      marking->state[j] = '1';

  /* Find enabled events */
  oldEnablings=CopyString(marking->enablings);
  lpnEventsEnabled(events,rules,markkey,marking,nrules,nsignals,nevents,
		   false);

  /* Update statevector with enablings */
  for(j=1;j<nevents;j++) {
    if(marking->enablings[j] == 'T') {
      if(events[j]->signal >= 0) {
        /* if the state isn't consistent return an error */
        if(!lpnCheckConsistency(events[j]->event,marking->state,
                                events[j]->signal,ninpsig,verbose)) {
          free(oldEnablings);
          return false;
        }
        if(strchr(events[j]->event,'+')) 
          marking->state[events[j]->signal] = 'R';
        else
          marking->state[events[j]->signal] = 'F';
      }
    }
    else {
      /* If there has been a disabling, report it and exit if a !disabling */
      if((events[j]->signal >= ninpsig) && (oldEnablings[j]=='T')) {
        lpnDisabling(disabling,j,events[j]->event,verbose);
        if(!disabling) {
          free(oldEnablings);
          return false;
        }
      }
    }
  }
  free(oldEnablings);
  return true;
}

/*****************************************************************************
* Display state count and memory statistics.                                
******************************************************************************/
void lpnDisplayInfo(int iter,int sn,int regions,int stack_depth,bool bap)
{
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
#else
  malloc_statistics_t t;
#endif
#endif

  if ((bap && regions % 1000==0) || (!bap && sn % 1000 == 0)) {
    if (bap) {
      printf("%d iterations %d zones %d states (stack_depth = %d)\n",iter,
	     regions,sn,stack_depth);
      fprintf(lg,"%d iterations %d zones %d states (stack_depth = %d)\n",iter,
	      regions,sn,stack_depth);
    } else {
      printf("%d iterations %d states (stack_depth = %d)\n",iter,
	     sn,stack_depth);
      fprintf(lg,"%d iterations %d states (stack_depth = %d)\n",iter,
	      sn,stack_depth);
    }
#ifdef MEMSTATS
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
  }
}

/*****************************************************************************
* Print an error message for the safety failure that was found.
******************************************************************************/
void lpnSafetyFailure(char *e,char *f,bool verbose)
{
  if (!verbose) {
    printf("Error!\n");
    fprintf(lg,"Error!\n");
  }
  printf("Error: Safety violation for rule %s -> %s.\n",e,f);
  fprintf(lg,"Error: Safety violation for rule %s -> %s.\n",e,f);
}

/*****************************************************************************
* Print an error message for the vacuous assignment failure that was found.
******************************************************************************/
void lpnVacuousFailure(char *e,bool verbose)
{
  if (!verbose) {
    printf("Error!\n");
    fprintf(lg,"Error!\n");
  }
  printf("Error: Firing of %s is a vacuous transition.\n",e);
  fprintf(lg,"Error: Firing of %s is a vacuous transition.\n",e);
}

/*****************************************************************************
* Find a new marking after signal i has fired.
* Returns true on success and false on failure.
******************************************************************************/
bool lpnFireTransition(eventADT *events,ruleADT **rules,markingADT marking,
		       int i,int nevents,int nplaces,int nsignals,
		       bool verbose,bool nofail)
{
  int j,k;
  
  /* Check if fired transition enabled, if not constraint failure */
  if(!lpnCheckEventEnabled(events,rules,marking,i,nevents,nplaces,
			   nsignals,TRUE,TRUE,verbose,nofail)) {
    if(!nofail) {
      return false;
    }
  }

  /* Remove tokens from fanin places */
  for(j=1;j<nevents;j++) { 
    if(lpnValidRule(rules[j][i]->ruletype)) {
      marking->marking[rules[j][i]->marking]='F';
    }
  }
  /* remove all tokens from rules i.e. choice is made*/
  for(j=nevents;j<nevents+nplaces;j++) 
    if(lpnValidRule(rules[j][i]->ruletype)) 
      for(k=1;k<nevents;k++)  
	if(lpnValidRule(rules[j][k]->ruletype)) {
	  marking->marking[rules[j][k]->marking]='F';
	}

  /* Add tokens to fanout places, check safety */
  for(j=1;j<nevents;j++) 
    if(lpnValidRule(rules[i][j]->ruletype)) {
      if(marking->marking[rules[i][j]->marking]=='F')
	marking->marking[rules[i][j]->marking]='T';
      else {
	lpnSafetyFailure(events[i]->event,events[j]->event,verbose);
	return false;
      }
    }
  for(j=nevents;j<nevents+nplaces;j++)
    if(lpnValidRule(rules[i][j]->ruletype)) {
      for(k=1;k<nevents;k++) {
	if(lpnValidRule(rules[j][k]->ruletype)) {
	  if(marking->marking[rules[j][k]->marking]=='F')
	    marking->marking[rules[j][k]->marking]='T';
	  else {
	    lpnSafetyFailure(events[j]->event,events[k]->event,verbose);
	    return false;
	  }
	}
      }
    }
  /* Fired event is no longer enabled */
  marking->enablings[i]='F';
  /* If transition on a signal, update the state vector, check vacuous */
  if(events[i]->signal >= 0) {
    if(marking->state[events[i]->signal] == 'R') 
      marking->state[events[i]->signal] = '1';
    else if(marking->state[events[i]->signal] == 'F') 
      marking->state[events[i]->signal] = '0';
    else {
      lpnVacuousFailure(events[i]->event,verbose);
      return false;
    }
  }
  return true;
}

/*****************************************************************************
* Find a new marking and enablings after signal i has fired.
******************************************************************************/
markingADT lpnFindNewMarking(signalADT *signals,
			     eventADT *events,ruleADT **rules,
			     markkeyADT *markkey,markingADT oldMarking,
			     int i,int nevents,int nplaces,int nrules,
			     bool verbose,int nsignals,int ninpsig,
			     bool disabling,bool noparg,bool nofail)
{

  /* Create a copy of the marking */
  markingADT marking = (markingADT)GetBlock(sizeof *marking);
  marking->marking = CopyString(oldMarking->marking);
  marking->state = CopyString(oldMarking->state);
  marking->enablings = CopyString(oldMarking->enablings);
  marking->up = NULL;
  marking->down = NULL;
  marking->ref_count = 0;

#ifdef __SRL_TRACE__
  printf("lpnFindNewMarking():Start\n");
#endif
  
  /* Fire transition and update the marking and state */
  if(!lpnFireTransition(events,rules,marking,i,nevents,nplaces,nsignals,
                        verbose,nofail)) {
    print_lpn("deadlock",signals,events,rules,nevents,nplaces,TRUE,marking,
	      false,noparg,false,NULL);
#ifdef __SRL_TRACE__
    printf("lpnFindNewMarking():End\n");
#endif
    return NULL;
  }
  /* Update the enablings and state vector enablings */
  if(!lpnUpdateState(events,rules,markkey,marking,nevents,nrules,
                     nsignals,ninpsig,verbose,disabling)) {
    print_lpn("deadlock",signals,events,rules,nevents,nplaces,TRUE,marking,
	      false,noparg,false,NULL);
#ifdef __SRL_TRACE__
    printf("lpnFindNewMarking():End\n");
#endif
    return NULL;
  }

#ifdef __SRL_TRACE__
  printf("lpnFindNewMarking():End\n");
#endif
  return marking;
}

/*****************************************************************************
* Look for deadlocks and display an error message.
* Not currently implemented.
******************************************************************************/
bool lpnFoundDeadlock(eventADT *events,ruleADT **rules,markkeyADT *markkey,
		      markingADT marking,int nevents,int nplaces,int nrules,
		      bool verbose,bool nofail,bool search,int nsignals)
{
  return false;
}

/*****************************************************************************
* Get the necessary set for the dependent transition.
* Based upon Fig. 4.24 in egm's thesis.
******************************************************************************/
bool* lpnGetNecessarySet(signalADT *signals,eventADT *events,ruleADT **rules,
			 markingADT marking,int nevents,int nplaces,
			 int t,bool *TD,int depth,int nsignals)
{

  bool *result = newBoolVector(nevents);
  int i;
  
  /* if the transition is already in the visited set then return nothing
     b/c we have hit a cycle */
  if(TD[t] == true) {
    return result;
  }

  /* if the transition is marked then simply return the transition
     Note: this should only happen on a recursive call -- not an initial
     call from lpnGetDependentSet() */
  if(marking->enablings[t]=='T') {
    result[t] = true;
    return result;
  }

  /* add all transitions w/ the same level to the visited set */
  if(events[t]->signal >= 0) {
    for(i=signals[events[t]->signal]->event;
	i<nevents && events[i]->signal == events[t]->signal;i++) {
      TD[i] = true;
    }
  }
  else {
    
  }
  return result;
}

/*****************************************************************************
* Get the dependent set for the current ample set.
* Based upon Def. 5.9 from egm's thesis
******************************************************************************/
void lpnGetDependentSet(signalADT *signals,eventADT *events,ruleADT **rules,
                        markingADT marking,int nevents,int nplaces,int t,
                        char *curAmple,int *curSize, int nsignals)
{
  int i,depth=0;
  int tempCnt=0;
  bool *TD;
  
  for(i=1;i<nevents;i++) {
    /* check to see if j is a member of relevant */
    if(rules[t][i]->conflict == 'T' && OCONFLICT) {
      /* if it is marking & level satisfied (enabled) do the easy thing
         and add it to the dependent and call dependent on that transition */
      if(marking->enablings[i] == 'T') {
        if(curAmple[i] != 'T') {
          curAmple[i] = 'T';
          *curSize++;
          lpnGetDependentSet(signals,events,rules,marking,nevents,nplaces,i,
                             curAmple,curSize,nsignals);
        }
      }
      /* else find the transition necessary to enable j that is currently
         enabled */
      else {
        /* bail for now by making the curAmple equal to the current
           marking -- this will be changed when lpnGetNecessary can
           actually do something useful */
        curAmple = CopyString(marking->enablings);
        for(i=0;i<nevents;i++) {
          if(curAmple[i] == 'T') {
            tempCnt++;
          }
        }
        *curSize = tempCnt;
        break;

        TD=newBoolVector(nevents+nplaces);
	
        /* populate TD w/ the possible visited transitions */
        /* should we do this now or later? */
        lpnGetNecessarySet(signals,events,rules,marking,nevents,nplaces,i,
                           TD,depth,nsignals);
      }
    }
  } 
}

/*****************************************************************************
* Get an ample set for the partial order method.
* Based upon Def. 5.10 from egm's thesis
*  -Note that the portions for timing are NOT yet implemented
******************************************************************************/
char *lpnGetAmpleSet(signalADT *signals,eventADT *events,ruleADT **rules,
		     markingADT marking,int nevents,int nplaces,bool PO_reduce,
		     int nsignals)
{
  /* see egm's thesis pg. 161 or Def. 5.10 */
  char *curAmple;
  char *bestAmple;
  int bestSize,curSize,i,j;

#ifdef __SRL_TRACE__
  printf("lpnGetAmpleSet():Start\n");
#endif
  
#ifdef __PO_DEBUG__
  printf("Getting ample set for %s\n",marking->state);
  printf("Enabled:\n");
  for (i=1;i<nevents;i++) {
    if (marking->enablings[i]=='T') {
      printf("%s\n",events[i]->event);
    }
  }
#endif
  
  /* Create initial best ample set with all enabled signals */
  bestAmple=CopyString(marking->enablings);
  /* Exit if not using partial order method */
  if (!PO_reduce) {
#ifdef __SRL_DEBUG__
    printf("PO_reduce isn't on so bail out\n");
#endif
    return bestAmple;
  }
    
  /* Compute size of the initial ample set */
  bestSize=0;
  for(i=1;i<nevents;i++) {
    if(bestAmple[i]=='T') bestSize++;
  }
  if(bestSize==1) {
    return bestAmple;
  }

  curAmple = (char *)GetBlock((nevents+1)*sizeof(char));
  curAmple[nevents] = '\0';
  
  for(i=1;i<nevents;i++) {
    if(marking->enablings[i]=='T') {
      curSize = 0;
      for(j=1;j<nevents;j++) {
	curAmple[j] = 'F';
      }
      lpnGetDependentSet(signals,events,rules,marking,nevents,nplaces,i,
			 curAmple,&curSize,nsignals);

      if(curSize < bestSize) {
	bestAmple = curAmple;
      }
    }
  }
  
  free(curAmple);
#ifdef __PO_DEBUG__
  printf("Best Ample:\n");
  for(i=1;i<nevents;i++) {
    if(bestAmple[i]=='T') {
      printf("%s\n",events[i]->event);
    }
  }
#endif
  
  return bestAmple;
}

/*****************************************************************************
* Finds the bounds on the enabling rule for an event.
******************************************************************************/
struct bounds lpnFindBounds(int event,ruleADT **rules,int nevents,int nplaces)
{
  struct bounds cur;
  int i;
  int found = 0;
  
  cur.upper = -1;
  cur.lower = -1;

  for(i=0;i<nevents+nplaces;i++) {
    if((lpnCausalRule(rules[i][event]->ruletype))) {
      if(found) {
        if(cur.upper != rules[i][event]->upper
           || cur.lower != rules[i][event]->lower) {
	  if (ltn) {
	    cSetFg(RED);
	    printf("Warning: ");
	    cSetAttr(NONE);
	    printf("This is not an LTN.  Behavior may be conservative.\n");
	    ltn=false;
	  }
          if (rules[i][event]->lower < cur.lower) 
            cur.lower=rules[i][event]->lower;
          if (rules[i][event]->upper > cur.upper) 
            cur.upper=rules[i][event]->upper;
        }
      }
      else {
        cur.upper = rules[i][event]->upper;
        cur.lower = rules[i][event]->lower;
        found = 1;
#ifdef __SRL_DEBUG__OFF
        printf("Bounds for %d are: %d, %d\n",event,cur.upper,cur.lower);
#endif
      }
    }
  }

  if(!found) {
#ifdef __SRL_DEBUG__
    cSetFg(RED);
    printf("Warning: ");
    cSetAttr(NONE);
    printf("Event (%d) is enabled, but has 0 rules!!\n",event);
#endif
    cur.upper = 0;
    cur.lower = 0;
  }
  
  return cur;
}

/*****************************************************************************
* Prints out a given zone.
******************************************************************************/
void printZone(eventADT *events,srlZoneADT zone)
{
  int i,j;
  cSetFg(GREEN);
  printf("Zone: ");
  cSetAttr(NONE);
  printf("pastFutureSep: %d -- numTransitions: %d\n",
         zone->pastFutureSep,zone->numTransitions);
  
  /* TBD: Add a way to print out the true parents */
  
  /* print out the transitions in the zone separated by tabs */
  for(i=0;i<zone->numTransitions;i++) {
    printf("\t%s",events[zone->curTransitions[i].enabled]->event);
  }
  printf("\n");

  for(i=0;i<zone->numTransitions;i++) {
    printf("%s",events[zone->curTransitions[i].enabled]->event);
    for(j=0;j<zone->numTransitions;j++) {
      if(zone->matrix[i][j] == INFIN) {
        printf("\tinf");
      }
      else {
        printf("\t%d",zone->matrix[i][j]);
      }
    }
    printf("\n");
  }
}

/*****************************************************************************
* Prints out a plain zone w/out strings, etc.
******************************************************************************/
void printPlainZone(eventADT *events,srlZoneADT zone) 
{
  int i,j;
  cSetFg(GREEN);
  printf("Zone: ");
  cSetAttr(NONE);
  printf("pastFutureSep: %d -- numTransitions: %d\n",
         zone->pastFutureSep,zone->numTransitions);
  
  /* TBD: Add a way to print out the true parents */
  
  /* print out the transitions in the zone separated by tabs */
  for(i=0;i<zone->numTransitions;i++) {
    printf("\t%d",zone->curTransitions[i].enabled);
  }
  printf("\n");

  for(i=0;i<zone->numTransitions;i++) {
    printf("%d",zone->curTransitions[i].enabled);
    for(j=0;j<zone->numTransitions;j++) {
      if(zone->matrix[i][j] == INFIN) {
        printf("\tinf");
      }
      else {
        printf("\t%d",zone->matrix[i][j]);
      }
    }
    printf("\n");
  }
}

/*****************************************************************************
* Prints out a given set of zones.
******************************************************************************/
void printZoneS(eventADT *events,srlZoneSet zoneS) 
{
  srlZoneSet pZoneS = zoneS;

  printf("Printing a set of zones.\n");
  while(pZoneS != NULL) {
    printZone(events,pZoneS->zone);
    pZoneS = pZoneS->next;
  }
  printf("Set is done printing.\n");
  
}

/*****************************************************************************
* Adds a zone to a given set of zones.
******************************************************************************/
void addZoneS(srlZoneADT zone,srlZoneSet *zoneS) 
{
  srlZoneSet newZoneS = NULL;

#ifdef __SRL_TRACE__
  printf("addZoneS():Start\n");
#endif
  
  newZoneS = (srlZoneSet)GetBlock(sizeof *newZoneS);
  newZoneS->zone = zone;
  newZoneS->next = *zoneS;
  *zoneS = newZoneS;

#ifdef __SRL_TRACE__
  printf("addZoneS():End\n");
#endif
}

/*****************************************************************************
* Removes a zone from the given set of zones.
******************************************************************************/
srlZoneADT removeZoneS(srlZoneSet *zoneS) 
{
  srlZoneADT zone = NULL;
  srlZoneSet oldZoneS;

#ifdef __SRL_TRACE__
  printf("removeZoneS():Start\n");
#endif
  
  if(*zoneS == NULL) {
    return NULL;
  }

  zone = (*zoneS)->zone;
  oldZoneS = *zoneS;
  *zoneS = (*zoneS)->next;
  free(oldZoneS);

#ifdef __SRL_TRACE__
  printf("removeZoneS():End\n");
#endif
  
  return zone;
}

/*****************************************************************************
* Adds a tpADT to the tpList in the given zone.
******************************************************************************/
void addTP(srlZoneADT zone,tpList tp) 
{
  tp->next = zone->tP;
  zone->tP = tp;
}

/*****************************************************************************
* Adds a tpADT to the tpList in the given zone.
******************************************************************************/
void addTP(srlZoneADT zone,int event,int trueP) 
{
#ifdef __SRL_TRACE__
  printf("addTP():Start\n");
#endif
  tpList tp = (tpList)GetBlock(sizeof (*tp));
  tp->tp = trueP;
  tp->tn = event;
  
  tp->next = zone->tP;
  zone->tP = tp;
#ifdef __SRL_TRACE__
  printf("addTP():End\n");
#endif
}

/*****************************************************************************
* Removes a tpADT from the given zone.
******************************************************************************/
tpList removeTP(srlZoneADT zone) 
{
#ifdef __SRL_TRACE__
  printf("removeTP():Start\n");
#endif
  tpList remTP = zone->tP;
  if(remTP == NULL) {
#ifdef __SRL_TRACE__
    printf("removeTP(NULL):End\n");
#endif
    return NULL;
  }
  zone->tP = remTP->next;
#ifdef __SRL_TRACE__
  printf("removeTP():End\n");
#endif
  return remTP;
}

void printTP(tpList tp) 
{
  if(tp == NULL) {
    printf("tp is NULL!\n");
    return;
  }  
  printf("tp->tp:%d, tp->tn:%d, tp->next:%p\n",tp->tp,tp->tn,tp->next);
}

/*****************************************************************************
* Verifies that the zone is consistent.
******************************************************************************/
int checkConsistentZone(eventADT *events,srlZoneADT zone)
{
  int i,j;
  for(i=0;i<zone->numTransitions;i++) {
    if(zone->matrix[i][i] != 0) {
/* #ifdef __SRL_DEBUG__ */
      cSetFg(RED);
      printf("Error: Zone is not consistent.\n");
      cSetAttr(NONE);
/* #endif */
#ifdef __SRL_DEBUG__OFF
      printZone(events, zone);
#endif
      return 0;
    }
  }
  for(i=zone->pastFutureSep;i<zone->numTransitions;i++) 
    for(j=0;j<zone->pastFutureSep;j++) {
      if(zone->matrix[i][j] < 0) {
#ifdef __SRL_DEBUG__
      cSetFg(RED);
      printf("Error: Zone is not valid.\n");
      cSetAttr(NONE);
#endif
#ifdef __SRL_DEBUG__OFF
      printZone(events, zone);
#endif
      return 0;
    }
  }
  return 1;
}

/*****************************************************************************
* This function doesn't return true for equivalent zones.  It returns
* true for zones that are equivalent and ordered the same.  It should
* probably be extended to check for equivalent zones in the future.
******************************************************************************/
int equalZone(srlZoneADT lhs,srlZoneADT rhs)
{
  int i,j;

  if(lhs->numTransitions != rhs->numTransitions) {
    return 0;
  }

  if(lhs->pastFutureSep != rhs->pastFutureSep) {
    return 0;
  }
  
  for(i=0;i<lhs->numTransitions;i++) {
    if(lhs->curTransitions[i].enabled != rhs->curTransitions[i].enabled) {
      return 0;
    }
    for(j=0;j<lhs->numTransitions;j++) {
      if(lhs->matrix[i][j] != rhs->matrix[i][j]) {
        return 0;
      }
    }
  }
  return 1;
}

/*****************************************************************************
* Creates a new zone data structure.  The numTransitions and
* pastFutureSep variables are initialized to size and 0 respectively.
******************************************************************************/
srlZoneADT lpnCreateZone(int size)
{
  srlZoneADT zone;
  int i,j;

#ifdef __SRL_DEBUG__
  printf("lpnCreateZone(%d):Start\n",size);
#endif
  
  zone = (srlZoneADT)GetBlock(sizeof(*zone));
  
  zone->pastFutureSep = 0; /* all future variables by default */
  zone->tP = NULL; /* no list yet */
  zone->numTransitions = size;

  if(size == 0) {
    cSetFg(RED);
    printf("Warning: ");
    cSetAttr(NONE);
    printf("creating a zero size zone\n");
    zone->curTransitions = NULL;
    zone->matrix = NULL;
    return zone;
  }

  /* allocate and initialize the information key for the current
     transitions */
  zone->curTransitions =
    (clockkeyADT) GetBlock(size * sizeof(struct clockkey));
  for(i=0;i<size;i++) {
    zone->curTransitions[i].enabled = -1;
    zone->curTransitions[i].enabling = -1;
    zone->curTransitions[i].eventk_num = -1;
    zone->curTransitions[i].causal = -1;
    zone->curTransitions[i].anti = NULL;
  }
  
  /* allocate the 2D zone matrix */
  zone->matrix = (clocksADT) GetBlock(size * sizeof(int *));
  for(i=0;i<size;i++) {
    zone->matrix[i] = (int *) GetBlock(size * sizeof(int *));
  }

  /* initialize the matrix to have all infinite bounds that can
     be tightened down later as appropriate */
  for(i=0;i<size;i++) {
    for(j=0;j<size;j++) {
      zone->matrix[i][j] = INFIN;
    }
  }
  
  /* make the zone consistent by zeroing out the diagonal */
  for(i=0;i<size;i++) {
    zone->matrix[i][i] = 0;
  }
  
#ifdef __SRL_DEBUG__
  printf("lpnCreateZone(%d):End\n",size);
#endif
  return zone;
}

/*****************************************************************************
* Frees the zone data structure.
******************************************************************************/
void lpnFreeZone(srlZoneADT zone)
{
  int i;
  tpList TP;

  free(zone->curTransitions);
  for(i=0;i<zone->numTransitions;i++) {
    free(zone->matrix[i]);
  }
  free(zone->matrix);
  while(zone->tP != NULL) {
    TP = removeTP(zone);
    if(TP) {
      free(TP);
    }
  }
  free(zone);
}

/*****************************************************************************
* Copies a zone and returns it.
******************************************************************************/
srlZoneADT copyZone(srlZoneADT zone)
{
  int i,j;
  srlZoneADT newZone = NULL;
  tpList TP = NULL;
  
#ifdef __SRL_TRACE__
  printf("copyZone():Start\n");
#endif

  newZone = lpnCreateZone(zone->numTransitions);

  for(i=0;i<zone->numTransitions;i++) {
    newZone->curTransitions[i].enabled = zone->curTransitions[i].enabled;
    for(j=0;j<zone->numTransitions;j++) {
      newZone->matrix[i][j] = zone->matrix[i][j];
    }
  }
  newZone->pastFutureSep = zone->pastFutureSep;

  TP = zone->tP;
  while(TP != NULL) {
    addTP(newZone,TP->tn,TP->tp);
    TP = TP->next;
  }
  
#ifdef __SRL_TRACE__
  printf("copyZone():End\n");
#endif
  
  return newZone;
}

/*****************************************************************************
* Copies a list and returns it.
******************************************************************************/
srlListADT copyList(srlListADT list)
{
  int i;
  srlListADT newList = NULL;
  
#ifdef __SRL_TRACE__
  printf("copyList():Start\n");
#endif

  newList = (srlListADT)GetBlock(sizeof(*newList));

  newList->tot = list->tot;
  newList->pos = list->pos;

  newList->array = (int *)GetBlock((sizeof i) * list->tot);

  for(i=0;i<list->tot;i++) {
    newList->array[i] = list->array[i];
  }
  
#ifdef __SRL_TRACE__
  printf("copyList():End\n");
#endif
  
  return newList;
}

/*****************************************************************************
* Creates a new parent array and initializes it.
******************************************************************************/
parentADT *lpnCreateParent(int nevents) 
{
  int i,j;
  parentADT *trueP;
  
  trueP = (parentADT *)GetBlock(nevents * sizeof(parentADT));
  for(i=0;i<nevents;i++) {
    trueP[i].parents = (bool *)GetBlock(nevents * sizeof(bool));
  }

  for(i=0;i<nevents;i++) {
    trueP[i].event = false;
    for(j=0;j<nevents;j++) {
      trueP[i].parents[j] = false;
    }
  }
  
  return trueP;
}

/*****************************************************************************
* Frees the parentADT.
******************************************************************************/
void lpnFreeParent(parentADT *trueP,int nevents) 
{
  int i;

  for(i=0;i<nevents;i++) {
    free(trueP[i].parents);
  }
  free(trueP);
}

/*****************************************************************************
* Removes a single transition from the zone.  The transition passed
* into the function is an index into the curTransitions array that is
* contained within the zone data structure.
******************************************************************************/
srlZoneADT lpnDeleteTransition(srlZoneADT zone, int transition)
{
  int i,j;
  srlZoneADT newZone = lpnCreateZone(zone->numTransitions-1);

#ifdef __SRL_TRACE__
  printf("lpnDeleteTransition():start\n");
#endif
  
  if(transition == 0) {    
    for(i=1;i<zone->numTransitions;i++) {
      newZone->curTransitions[i-1] = zone->curTransitions[i];
      for(j=1;j<zone->numTransitions;j++) {
        newZone->matrix[i-1][j-1] = zone->matrix[i][j];
      }
    }
  }
  else {
    for(i=0;i<zone->numTransitions;i++) {
      if(i < transition) {
        newZone->curTransitions[i] = zone->curTransitions[i];
        for(j=0;j<zone->numTransitions;j++) {
          if(j < transition) {
            newZone->matrix[i][j] = zone->matrix[i][j];
          }
          else if(j > transition) {
            newZone->matrix[i][j-1] = zone->matrix[i][j];
          }
        }
      }
      else if(i > transition) {
        newZone->curTransitions[i-1] = zone->curTransitions[i];
        for(j=0;j<zone->numTransitions;j++) {
          if(j < transition) {
            newZone->matrix[i-1][j] = zone->matrix[i][j];
          }
          else if(j > transition) {
            newZone->matrix[i-1][j-1] = zone->matrix[i][j];
          }
        }
      }
    }
  }

  if(transition < zone->pastFutureSep) {
    newZone->pastFutureSep = zone->pastFutureSep-1;
  }
  else {
    newZone->pastFutureSep = zone->pastFutureSep;
  }
  
#ifdef __SRL_TRACE__
  printf("lpnDeleteTransition():end\n");
#endif
  return newZone;
}  

/*****************************************************************************
* Add n dimensions to the matrix.
******************************************************************************/
srlZoneADT lpnAddDim(srlZoneADT zone,int dim)
{
  int i,j;
  srlZoneADT newZone;
  tpList TP;
  
#ifdef __SRL_TRACE__
  printf("lpnAddDim(%d):Start\n",dim);
#endif
  
  newZone = lpnCreateZone(zone->numTransitions+dim);
  
  newZone->pastFutureSep = zone->pastFutureSep;
  TP = zone->tP;
  while(TP != NULL) {
    addTP(newZone,TP->tn,TP->tp);
    TP = TP->next;
  }
  for(i=0;i<zone->numTransitions;i++) {
    newZone->curTransitions[i].enabled = zone->curTransitions[i].enabled;
    for(j=0;j<zone->numTransitions;j++) {
      newZone->matrix[i][j] = zone->matrix[i][j];
    }
  }

  for(i=0;i<dim;i++) {
    for(j=0;j<dim;j++) {
      if(i==j) {
        newZone->matrix[zone->numTransitions+i][zone->numTransitions+j]=0;
      }
      else {
        newZone->matrix[zone->numTransitions+i][zone->numTransitions+j]=INFIN;
      }
    }
  }
  
#ifdef __SRL_TRACE__
  printf("lpnAddDim():End\n");
#endif
  return newZone;
}

/*****************************************************************************
* Move transition into the past variables section of the zone.  The
* transition passed into the function is an index into the
* curTransitions array.
******************************************************************************/
srlZoneADT lpnMakePast(srlZoneADT zone, int *transition)
{
  int i,j,temp;
  int index = *transition;
  srlZoneADT newZone;

#ifdef __SRL_TRACE__
  printf("lpnMakePast(%d):Start\n",index);
#endif
  
  newZone = copyZone(zone);
  
  /* check the easy case first */
  /* this is where the variable to be made a past variable is
     currently on the past/future boundary */
#ifdef __SRL_DEBUG__
  printf("nZ.pFSep: %d, index: %d\n",newZone->pastFutureSep,index);
#endif
  if(newZone->pastFutureSep >= index) {
    if(newZone->pastFutureSep > index) {
#ifdef __SRL_DEBUG__
      cSetFg(YELLOW);
      printf("Efficiency warning: ");
      cSetAttr(NONE);
      printf("lpnMakePast was called on a variable that is already a past variable.\n");
#endif
      return newZone;
    }
    newZone->pastFutureSep++;
    return newZone;
  }
  else {
    /* swap index and pastFutureSep in the curTransitions list */
    temp = zone->curTransitions[index].enabled;
    newZone->curTransitions[index].enabled =
      zone->curTransitions[zone->pastFutureSep].enabled;
    newZone->curTransitions[zone->pastFutureSep].enabled = temp;
    /* fix up the index for the findNewZone function */
    *transition = zone->pastFutureSep;
    
    /* not the easy case so now exchange the matrix values of
       index with pastFutureSep.  This will effectively swap the
       two transitions in the zone */
    for(i=0;i<zone->numTransitions;i++) {
      if(i == index) {
        for(j=0;j<zone->numTransitions;j++) {
          if(j == index) {
            newZone->matrix[zone->pastFutureSep][zone->pastFutureSep] =
              zone->matrix[i][j];
          }
          else if(j == zone->pastFutureSep) {
            newZone->matrix[zone->pastFutureSep][index] = zone->matrix[i][j];
          }
          else {
            newZone->matrix[zone->pastFutureSep][j] = zone->matrix[i][j];
          }
        }
      }
      else if(i == zone->pastFutureSep) {
        for(j=0;j<zone->numTransitions;j++) {
          if(j == index) {
            newZone->matrix[index][zone->pastFutureSep] = zone->matrix[i][j];
          }
          else if(j == zone->pastFutureSep) {
            newZone->matrix[index][index] = zone->matrix[i][j];
          }
          else {
            newZone->matrix[index][j] = zone->matrix[i][j];
          }
        }
      }
      else {
        for(j=0;j<zone->numTransitions;j++) {
          if(j == index) {
            newZone->matrix[i][zone->pastFutureSep] = zone->matrix[i][j];
          }
          else if(j == zone->pastFutureSep) {
            newZone->matrix[i][index] = zone->matrix[i][j];
          }
          else {
            newZone->matrix[i][j] = zone->matrix[i][j];
          }
        }
      }
    }
  }
  newZone->pastFutureSep++;
#ifdef __SRL_TRACE__
  printf("lpnMakePast(%d):End\n",index);
#endif
  return newZone;
}

/*****************************************************************************
* Uses Floyd's algorithm (naive version which in O(n^3)) to recanonicalize
* the zone.
******************************************************************************/
void lpnRecanonZone(srlZoneADT zone)
{
  int i, j, k;
#ifdef __SRL_TRACE__
  printf("lpnRecanonZone:Start()\n");
#endif
  for(i=0;i<zone->numTransitions;i++) {
    for(j=0;j<zone->numTransitions;j++) {
      for(k=0;k<zone->numTransitions;k++) {
	if(zone->matrix[j][i] != INFIN && zone->matrix[i][k] != INFIN &&
	   zone->matrix[j][k] > zone->matrix[j][i] + zone->matrix[i][k]) {
	  zone->matrix[j][k] = zone->matrix[j][i] + zone->matrix[i][k];
	}
      }
    }
  }
#ifdef __SRL_TRACE__
  printf("lpnRecanonZone:End()\n");
#endif
  return;
}

/*****************************************************************************
* This is bassicaly getNecessary from Tomohiro.
******************************************************************************/
eventTimePairADT *lpnTraceBack(bool *TD,int t,markingADT marking,
                               eventADT *events,ruleADT **rules,
                               int nevents,int nplaces,bool first)
{
  int i,p;
  eventTimePairADT *result = newETVector(nevents);
  eventTimePairADT *temp = NULL;
  eventTimePairADT *temp2 = NULL;
  bounds cur;
  
  /* check for cycles first */
  if(TD[t]) {
#ifdef __POSET_DEBUG__OFF
    printf("Cycle found in lpnTraceBack.  Returning.\n");
#endif
    /* basically returning NULL */
    return result;
  }

  /* check to see if the event is currently enabled */
  if(marking->enablings[t] == 'T') {
#ifdef __POSET_DEBUG__OFF
    printf("Event: %s:%d is enabled.  Returning.\n",
           events[t]->event,t);
#endif
    result[t].event = true;
    result[t].minBound = 0;
    return result;
  }

  /* add t to the visited set (TD) */
  TD[t] = true;

  /* foreach preset place of t that isn't marked */
  for(p=1;p<nevents+nplaces;p++) {
    if((lpnValidRule(rules[p][t]->ruletype)) &&
       lpnCausalRule(rules[p][t]->ruletype) &&
       (marking->marking[rules[p][t]->marking] == 'F')) {
      /* foreach t' that is in the preset of the place p */
      if(p<nevents) {
        /* the model removes the place if it is a single fan-in or fan-out
           place so if p is an event instead of a place then we know it is
           a single fan-in or fan-out place */
        temp = lpnTraceBack(TD,p,marking,events,rules,nevents,nplaces,false);
      }
      else {
        /* multiple fan-ins that must be unioned */
        temp = newETVector(nevents);
        for(i=0;i<nevents;i++){
          if(lpnValidRule(rules[i][p]->ruletype)
             && lpnCausalRule(rules[i][p]->ruletype)) {
            temp2 = lpnTraceBack(TD,i,marking,events,rules,nevents,nplaces,
				 false);
            if(temp2) {
              unionETVector(temp,temp2,nevents);
              free(temp2);
            }
          }
        }
      }
      /* I am not sure that this the right place to add up the Eft, but
         lets give it a whirl */
      if(temp) {
        cur = lpnFindBounds(t,rules,nevents,nplaces);
        for(i=0;i<nevents;i++) {
          if((temp[i].event)&&(!first)) {
            temp[i].minBound += cur.lower;
          }
        }
        /* find the minimal set */
        if(isEmptyETVector(result,nevents) || (sizeETVector(result,nevents) <
                                               sizeETVector(temp,nevents))) {
          copyETVector(result,temp,nevents);
        }
        free(temp);
      }
    } 
  }
  
  return result;
}

/*****************************************************************************
* Creates a list of possible true parents based on level expressions.
******************************************************************************/
void lpnVRuler(int b,srlZoneADT zone,bool *possibleParent,
              int nevents,int nplaces,ruleADT **rules) 
{
  
}

/*****************************************************************************
* Find the true parent of a specified event based on the level expressions.
******************************************************************************/
void lpnTrueParentCond(srlZoneADT zone,level_exp f,markingADT marking,
                       eventADT *events,ruleADT **rules,int nevents,
                       int nplaces,int nsignals,bool *keepers) 
{
  int i;
  int zeros = 0;
  int ones = 0;
  level_exp l = (level_exp)GetBlock(sizeof(level_exp));
  l->product = (char *)GetBlock(sizeof(char)*nsignals);
  for(i=0;i<nsignals;i++) {
    l->product[i] = 'X';
  }
  l->next = NULL;
  
  /* only product term(s) */
  if(!f->next) {
    for(i=0;i<nsignals;i++) {
      if(f->product[i] == '0') {
        zeros++;
      }
      else if(f->product[i] == '1') {
        ones++;
      }
    }
    if(zeros+ones < 1) {
      return;
    }
    else if(zeros+ones == 1) {
      if(zeros) {
        lpnVRuler(0,zone,keepers,nevents,nplaces,rules);
      }
      else {
        lpnVRuler(1,zone,keepers,nevents,nplaces,rules);
      }
    }
    else {
      for(i=0;i<nsignals;i++) {
        if(f->product[i] == '0') {
          l->product[i] = '0';
          lpnTrueParentCond(zone,l,marking,events,rules,nevents,nplaces,
                            nsignals,keepers);
          l->product[i] = 'X';
        }
        else if(f->product[i] == '1') {
          l->product[i] = '1';
          lpnTrueParentCond(zone,l,marking,events,rules,nevents,nplaces,
                            nsignals,keepers);
          l->product[i] = 'X';
        }
      }
    }
  }
  /* this is the or case */
  else {
  }
}

/*****************************************************************************
* Creates a list of possible true parents.
******************************************************************************/
void lpnRuler(int event,srlZoneADT zone,bool *possibleParent,
              int nevents,int nplaces,ruleADT **rules,eventADT *events)
{
  /* TBD: Switch this to a list of integers that doesn't have to be
     searched through as a performance optimization */
  int i,j,found;

#ifdef __SRL_TRACE__
  printf("lpnRuler(%s):Start\n",events[event]->event);
#endif
  
  /* find all grandparents for the given event -- this is easy b/c in
     the model if there is only a single enabling event then the place
     is removed so if an event is found in the rules matrix then it
     is the grandparent transition */
  for(i=1;i<nevents+nplaces;i++) {
    if(lpnValidRule(rules[i][event]->ruletype)
       && lpnCausalRule(rules[i][event]->ruletype)) {
      if(i>=nevents) {
        /* the source place for transition i has more than one
           enabling event so it is still present in the graph.  We
           must now find all of these events for that trace and add
           them to the list */
        for(j=1;j<nevents;j++) {
          if(lpnValidRule(rules[j][i]->ruletype)
             && lpnCausalRule(rules[j][i]->ruletype)) {
            if(j >= nevents) {
              cSetFg(RED);
              printf("Error: ");
              cSetAttr(NONE);
              printf("A place was found to be a grandparent transition in lpnRuler().\n");
            }
#ifdef __POSET_DEBUG__
            printf("enabling1:%d\n",j);
#endif
            possibleParent[j] = true;
          }
        }
      }
      else {
#ifdef __POSET_DEBUG__
        printf("enabling2:%d from rules[%d][%d]->%d\n",i,i,event,
               rules[i][event]->ruletype);
#endif
        possibleParent[i] = true;
      }
    }
  }

#ifdef __POSET_DEBUG__
  printf("possibleParent before intersecting w/ the zone.\n");
  for(i=0;i<nevents;i++) {
    printf("\t%s",events[i]->event);
  }
  printf("\n");
  for(i=0;i<nevents;i++) {
    printf("\t%d",possibleParent[i]);
  }
  printf("\n");
#endif
  
  /* intersect the grandparents w/ the transitions in the zone */
  for(i=0;i<nevents;i++) {
    found = 0;
    if(possibleParent[i]) {
      for(j=0;j<zone->numTransitions;j++) {
        if(zone->curTransitions[j].enabled == i) {
          found = 1;
          break;
        }
      }
      if(!found) {
        possibleParent[i] = false;
      }
    }
  }
#ifdef __POSET_DEBUG__
  printf("possibleParent after intersecting w/ the zone.\n");
  for(i=0;i<nevents;i++) {
    printf("\t%s",events[i]->event);
  }
  printf("\n");
  for(i=0;i<nevents;i++) {
    printf("\t%d",possibleParent[i]);
  }
  printf("\n");
#endif
#ifdef __SRL_TRACE__
  printf("lpnRuler():End\n");
#endif
}

/*****************************************************************************
* Try all possible combinations of true parents.  event tells the
* function where to start looking for more enabled events.
******************************************************************************/
bool lpnTrueParent(srlZoneADT zone,parentADT *trueP,markingADT marking,
                   eventADT *events,ruleADT **rules,int nevents,
                   int nplaces,char *firelist,srlZoneSet *zoneS,
                   int event)
{
  /* TBD: if this is expensive it could probably benefit from
     caching...there will be a lot of repeat computation based on the way
     I am implementing this */

  int i,j;
  srlZoneADT cZone;
  
#ifdef __SRL_TRACE__
  printf("lpnTrueParent(%d of %d):Start\n",event,nevents);
#endif
  /* find the next event to start working on */
  for(;event<nevents;event++) {
    if(trueP[event].event) {
#ifdef __POSET_DEBUG__
      printf("Found tn %s:%d\n",events[event]->event,event);
#endif
      break;
    }
  }

  /* no more events to work on so bail and let the calling function
     know that we bailed b/c there aren't anymore newly enabled events
     to explore */
  if(event == nevents) {
#ifdef __SRL_TRACE__
  printf("lpnTrueParent():End:1\n");
#endif
    return true;
  }
  
  /* get the list of possible parents for the current event */
  lpnRuler(event,zone,trueP[event].parents,nevents,nplaces,rules,events);

#ifdef __POSET_DEBUG__
  printf("Rulers: ");
  for(i=0;i<nevents;i++) {
    if(trueP[event].parents[i]) {
      printf("%d, ",i);
    }
  }
  printf("\n");
#endif
  /* try all possible parents for the current event */
  for(i=0;i<nevents;i++) {
    if(trueP[event].parents[i]) {
      /* see if the possible parent is actually a past variable in the zone */
      for(j=0;j<zone->pastFutureSep;j++) {
        if(zone->curTransitions[j].enabled == i) {
#ifdef __POSET_DEBUG__
          cSetFg(BLUE);
          printf("True parent found for %s is %s.\n",events[event]->event,
                 events[i]->event);
          cSetAttr(NONE);
#endif
          cZone = copyZone(zone);
          addTP(cZone,event,j);
       

#ifdef __POSET_DEBUG__
          printf("Calling trueParent(%d)\n",event);
#endif
          /* if we ran out of newly enabled events then this is a
             complete zone that is good so add it to the stack...if not
             then we are recursing back and the zone is no longer
             needed */
          if(lpnTrueParent(cZone,trueP,marking,events,rules,nevents,nplaces,
                           firelist,zoneS,event+1)) {
          
#ifdef __POSET_DEBUG__
            cSetFg(BLUE);
            printf("Adding a possible zone from trueParent -- %p.\n",cZone);
            cSetAttr(NONE);
            printZone(events,cZone);
#endif
            addZoneS(cZone,zoneS);
          }
          else {
            lpnFreeZone(cZone);
          }
          break;
        }
      }      
    }
  }
  
#ifdef __SRL_TRACE__
  printf("lpnTrueParent():End:2\n");
#endif
  /* tell the calling function that we returned b/c we ran out of
     possible parents not newly enabled events */
  return false;
}

/*****************************************************************************
* Remove past variables where nothing in their post set is in the
* current marking.
******************************************************************************/
void lpnPruneToMarking(srlZoneADT zone,bool *keepers,markingADT marking,
                       ruleADT **rules,int nevents,int nplaces) 
{
  int i,j,k;
  int marked;

#ifdef __SRL_TRACE__
  printf("lpnPruneToMarking():Start\n");
#endif

#ifdef __POSET_DEBUG__OFF
  printf("marking: ");
  for(i=0;i<nevents;i++) {
    printf("%d:%c ",i,marking->marking[i]);
  }
  printf("\n");
#endif
  
  /* TBD: would a goto work well here?? */
  for(i=0;i<zone->pastFutureSep;i++) {
    marked = 0;
    for(j=0;j<nevents+nplaces;j++) {
      if(lpnValidRule(rules[zone->curTransitions[i].enabled][j]->ruletype)) {
        /* push through the place */
        if(j >= nevents) {
          /* place to place rules aren't allowed so just search
             through the events */
          for(k=0;k<nevents;k++) {
            if(lpnValidRule(rules[j][k]->ruletype)) {
              if(marking->marking[rules[j][k]->marking] == 'T') {
#ifdef __POSET_DEBUG__OFF
                printf("event %d is okay b/c %d is marked (place between them)\n",zone->curTransitions[i].enabled,k);
#endif
                marked = 1;
                break;
              }
            }
          }
          if(marked) {
            break;
          }
        }
        else {
          if(marking
	     ->marking[rules[zone->curTransitions[i].enabled][j]->marking] 
	     == 'T') {
#ifdef __POSET_DEBUG__OFF
            printf("event %d is okay b/c %d is marked (place between them)\n",
                   zone->curTransitions[i].enabled,j);
#endif
            marked = 1;
            break;
          }
        }
      }
    }
    if(!marked) {
#ifdef __POSET_DEBUG__
      printf("Invalidating: %d\n",zone->curTransitions[i].enabled);
#endif
      keepers[zone->curTransitions[i].enabled] = false;
    }
  }

#ifdef __SRL_TRACE__
  printf("lpnPruneToMarking():End\n");
#endif
}

/*****************************************************************************
* Determine which of the true parent variables might possibly fire last.
******************************************************************************/
void lpnLast(srlZoneADT zone,bool *keepers,bool *parents) 
{
  int i,j;
  
  for(i=0;i<zone->pastFutureSep;i++) {
    for(j=0;j<zone->pastFutureSep;j++) {
      if(parents[zone->curTransitions[i].enabled] && 
	 parents[zone->curTransitions[j].enabled] && 
	 zone->matrix[j][i] < 0) {
#ifdef __POSET_DEBUG__
        printf("lpnLast: Invalidating %d\n",zone->curTransitions[j].enabled);
#endif
        keepers[zone->curTransitions[j].enabled] = false;
      }
    }
  }
}

/*****************************************************************************
* Determine which of the true parent variables might possibly fire last.
******************************************************************************/
void lpnCanFireLast(srlZoneADT zone,bool *TD,int t,markingADT marking,
                    eventADT *events,ruleADT **rules,int nevents,int nplaces,
                    bool *keepers)
{
  eventTimePairADT *etVec;
  int i,j,k;
  
  etVec = lpnTraceBack(TD,t,marking,events,rules,nevents,nplaces,true);

#ifdef __POSET_DEBUG__
  /* traceBack should only return events that are enabled so check
     that here. */
  for(i=1;i<nevents;i++) {
    if(etVec[i].event) {
      if(!marking->enablings[i]) {
        cSetFg(RED);
        printf("Error: ");
        cSetAttr(NONE);
        printf("lpnTraceBack() returned an event that is not currently enabled.\n");
        break;
      } 
    }
  }
  
#endif
  
#ifdef __POSET_DEBUG__
  printf("marking: ");
  for(i=0;i<nevents;i++) {
    printf("%s:%c ",events[i]->event,marking->enablings[i]);
  }
  printf("\n");
  printf("necessary set for event %s in canFireLast follows...\n",
         events[t]->event);
  printETVector(etVec,nevents,events);
#endif

  /* TBD: it appears that I don't need the minbound information
     since it should already be contained in the zone...is this
     true? */

  /* look in the zone to determine if the necessary events must fire
     after past variables...if they do then remove them from those
     events required to keep */
  for(k=0;k<zone->pastFutureSep;k++) {
    if(keepers[zone->curTransitions[k].enabled]) {
      bool keepit=false;
      for(i=0;i<nevents;i++) {
	if(etVec[i].event) {
	  for(j=zone->pastFutureSep;j<zone->numTransitions;j++) {
	    if ((zone->curTransitions[j].enabled == i) &&
		(zone->matrix[k][j] >= etVec[i].minBound)) {
	      keepit=true;
	      break;
            }
          }
	  if (keepit) break;
	}
      }
      if (!keepit) {
#ifdef __POSET_DEBUG__
	printf("lpnCanFireLast: Invalidating %d\n",
	       zone->curTransitions[j].enabled);
#endif
	keepers[zone->curTransitions[k].enabled] = false;
      }
    }
  }
  free(etVec);
}

/*****************************************************************************
* Determine which past variables still need to be kept around to help
* handle future causality decisions.
******************************************************************************/
void lpnNeedToKeep(srlZoneADT zone,int event,markingADT marking,
                   eventADT *events,ruleADT **rules,int nevents,
                   int nplaces,bool *keepers)
{
  bool *TD = newBoolVector(nevents);
  bool *parents = newBoolVector(nevents);
  
#ifdef __SRL_TRACE__
  printf("lpnNeedToKeep():Start\n");
  printZone(events,zone);
  printf("Checking %s\n",events[event]->event);
#endif
  
  lpnRuler(event,zone,keepers,nevents,nplaces,rules,events);
  copyBoolVector(parents,keepers,nevents);
  lpnPruneToMarking(zone,keepers,marking,rules,nevents,nplaces); 
  lpnLast(zone,keepers,parents);
  lpnCanFireLast(zone,TD,event,marking,events,rules,nevents,nplaces,keepers);

#ifdef __POSET_DEBUG__
  cSetFg(YELLOW);
  printf("keepers: ");
  for(int i=0;i<nevents;i++) {
    if(keepers[i]) {
      printf("%s, ",events[i]->event);
    }
  }
  printf("\n");
  cSetAttr(NONE);
#endif
  
#ifdef __SRL_TRACE__
  printf("lpnNeedToKeep():End\n");
#endif
  free(TD);
}  

/*****************************************************************************
* Check for violations of a minimum constraint bound.
* postset and event are the same transition, but event indexes into
* the rules matrix and postset indexes into zone->matrix
******************************************************************************/
bool lpnChkMinConstraint(int event,int postset,ruleADT **rules,int nevents,
                         int nplaces,srlZoneADT zone,eventADT *events,
                         markingADT marking,bool nofail) 
{
  int i,j,preset;
  int found = 0;
  
#ifdef __CONSTRAINT_DEBUG__
  printf("lpnChkMinConstraint(%s):Start\n",
         events[event]->event);
#endif
  
  for(i=0;i<nevents+nplaces;i++) {
    if(rules[i][event]->ruletype & ORDERING) {
      /* okay w/ found a constraint rule w/ index in the postset so
         now we need to check to see if the constraint rule holds */
#ifdef __CONSTRAINT_DEBUG__
      printf("Working on rule %s->%s w/ lower bound of %d\n",events[i]->event,
             events[event]->event,rules[i][event]->lower);
#endif
      for(j=0;j<zone->pastFutureSep;j++) {
        if(zone->curTransitions[j].enabled == i) {
          preset = j;
          found = 1;
          break;
        }
      }
      if(!found) {
#ifdef __CONSTRAINT_DEBUG__
        cSetFg(RED);
        printf("Error: ");
        cSetAttr(NONE);
        printf("A constraint rule fired, but the preset event was not found in the zone.  It is a possibility that it was prematurely pruned.\n");
#endif
        return true;
      }

      
      
      if(abs(zone->matrix[preset][postset]) < rules[i][event]->lower) {
#ifdef __CONSTRAINT_DEBUG__
        printf("Constraint rule bound is: %d  Zone lower bound is: %d\n",
               rules[i][event]->lower,abs(zone->matrix[preset][postset]));
        printZone(events,zone);
#endif
        /* constraint failure */
        if(nofail) {
          printf("Warning: lower bound of constraint [%s,%s] was violated in state %s.\n",events[i]->event,events[event]->event,marking->state);
          fprintf(lg,"Warning: lower bound of constraint [%s,%s] was violated in state %s.\n",events[i]->event,events[event]->event,marking->state);
        }
        else {
          printf("Error: lower bound of constraint [%s,%s] was violated in state %s.\n",events[i]->event,events[event]->event,marking->state);
          fprintf(lg,"Error: lower bound of constraint [%s,%s] was violated in state %s.\n",events[i]->event,events[event]->event,marking->state);
	  return false;
        }
      }
    }
  }

  return true;
#ifdef __CONSTRAINT_DEBUG_
  printf("lpnChkMinConstraint():End\n");
#endif
}

/*****************************************************************************
* Checks constraint rules to see if any maximum constraints have been
* violated.  Returns true if all rules are properly constrained and
* false on a constraint failure.
******************************************************************************/
bool lpnChkMaxConstraint(ruleADT **rules,int nevents,int nplaces,
                         srlZoneADT zone,eventADT *events,markingADT marking,
                         bool nofail) 
{
  int i,j,k;
  int found = 0;
  int min;
  min = INFIN;
  
#ifdef __CONSTRAINT_DEBUG__
  printf("lpnChkMaxConstraint():Start\n");
  printZone(events,zone);
#endif
  
  /* we are just going to check all past variables for constraint
  violations b/c the rule must be active for any possibility of a
  violation (i.e. the preset has fired) */
  for(i=0;i<zone->pastFutureSep;i++) {
    for(j=1;j<nevents+nplaces;j++) {
      if(rules[zone->curTransitions[i].enabled][j]->ruletype & ORDERING) {

#ifdef __CONSTRAINT_DEBUG__
        cSetFg(YELLOW);
        printf("Checking:");
        cSetAttr(NONE);
        printf("rules[%s][%s]\n",events[zone->curTransitions[i].enabled]->event
               ,events[j]->event);
#endif
        /* check to see if the postset event is enabled */
        found = 0;
        for(k=zone->pastFutureSep;k<zone->numTransitions;k++) {
          if(zone->curTransitions[k].enabled == j) {
            found = 1;
#ifdef __CONSTRAINT_DEBUG__
            cSetFg(YELLOW);
            printf("Post-set is enabled...checking the rule for a violation.\n");
            printZone(events,zone);
            cSetAttr(NONE);
#endif
            if(zone->matrix[k][i] >
               rules[zone->curTransitions[i].enabled][j]->upper) {
              /* constraint violation */
#ifdef __CONSTRAINT_DEBUG__
              printf("zone: %d -- upper bound: %d\n",zone->matrix[k][i],
                     rules[zone->curTransitions[i].enabled][j]->upper);
#endif
              if(nofail) {
                printf("Warning: upper bound of constraint (%s,%s) was violated in state %s.\n",events[i]->event,events[j]->event,marking->state);
                fprintf(lg,"Warning: upper bound of constraint (%s,%s) was violated in state %s.\n",events[zone->curTransitions[i].enabled]->event,
                        events[j]->event,marking->state);
              }
              else {
                printf("Error: upper bound of constraint (%s,%s) was violated in state %s.\n",events[zone->curTransitions[i].enabled]->event,
                       events[j]->event,marking->state);
                fprintf(lg,"Error: upper bound of constraint (%s,%s) was violated in state %s.\n",events[zone->curTransitions[i].enabled]->event,events[j]->event,marking->state);
		return false;
              }
#ifdef __CONSTRAINT_DEBUG__
              printf("lpnChkMaxConstraint():End0 - in zone\n");
#endif
            }
            break;
          }
        }
        
        /* okay, the postset event isn't enabled so we have to figure
           out the violations by looking at what is currently enabled
           -- this is mostly redundant from above...how can we do this
           better? */
        if(!found) {
#ifdef __CONSTRAINT_DEBUG__
            cSetFg(YELLOW);
            printf("Post-set is NOT enabled...checking the entire zone for violations.\n");
            printZone(events,zone);
            cSetAttr(NONE);
#endif
          for(k=zone->pastFutureSep;k<zone->numTransitions;k++) {
            if(zone->matrix[k][i] < min) {
              min = zone->matrix[k][i];
            }
          }
          if(min > rules[zone->curTransitions[i].enabled][j]->upper) {
            /* constraint violation */
#ifdef __CONSTRAINT_DEBUG__
            printf("zone: %d -- upper bound: %i\n",min,
                   rules[zone->curTransitions[i].enabled][j]->upper);
#endif
            if(nofail) {
              printf("Warning: upper bound of constraint (%s,%s) was violated in state %s.\n",events[i]->event,events[j]->event,marking->state);
              fprintf(lg,"Warning: upper bound of constraint (%s,%s) was violated in state %s.\n",events[zone->curTransitions[i].enabled]->event,
                      events[j]->event,marking->state);
            }
            else {
              printf("Error: upper bound of constraint (%s,%s) was violated in state %s.\n",events[zone->curTransitions[i].enabled]->event,
                     events[j]->event,marking->state);
              fprintf(lg,"Error: upper bound of constraint (%s,%s) was violated in state %s.\n",events[zone->curTransitions[i].enabled]->event,events[j]->event,marking->state);
	      return false;
            }
#ifdef __CONSTRAINT_DEBUG__
            printf("lpnChkMaxConstraint():End0 - not in zone\n");
#endif
          }
        } 
      }
    }
  }
#ifdef __CONSTRAINT_DEBUG__
  printf("lpnChkMaxConstraint():End1\n");
#endif
  return true;
}

/*****************************************************************************
* Return true if the given event should still be kept around to allow
* the checking of constraint rules.
* event is an index into zone->curTransitions
******************************************************************************/
bool lpnConstraintMarked(srlZoneADT zone,int event,markingADT marking,
                         ruleADT **rules,int nevents,int nplaces,
                         eventADT *events) 
{
  int i,index;

#ifdef __CONSTRAINT_DEBUG__
  printf("lpnConstraintMarked():Start\n");
#endif

#ifdef __CONSTRAINT_DEBUG__

   printf("Examining: %s\n",events[zone->curTransitions[event].enabled]->event);
#endif

  index = zone->curTransitions[event].enabled;
  
  for(i=1;i<nevents+nplaces;i++) {
    if(rules[index][i]->ruletype & ORDERING) {
#ifdef __CONSTRAINT_DEBUG__
      cSetFg(RED);
      printf("Constraint rule from %s to %s\n",events[index]->event,
             events[i]->event);
      cSetAttr(NONE);
#endif
      if(marking->marking[rules[index][i]->marking] == 'T') {
#ifdef __CONSTRAINT_DEBUG__
        printf("lpnConstraintMarked():End(1)\n");
#endif
        return true;
      }
    }
  }

#ifdef __CONSTRAINT_DEBUG__
  printf("lpnConstraintMarked():End(0)\n");
#endif
  
  return false;
}

/*****************************************************************************
* Project out events that are no longer needed in the zone.
******************************************************************************/
srlZoneADT lpnProjectZone(srlZoneADT zone,int event,markingADT marking,
                          bool poReduce,eventADT *events,ruleADT **rules,
                          int nevents,int nplaces)
{
  /* TBD: Remove the duplicate code from this function and possibly
     reorder the total order deletion stuff to do this
     This code can also be more efficient...change the code to avoid
     looping as often...do everything when we initially find the
     disabled events...this should work I think */
  int i,j,count = 0;
  int disabled[zone->numTransitions];
  srlZoneADT finalZone;
  srlZoneADT newZone;
  bool *keepers = newBoolVector(nevents);
  bool *finalKeepers = newBoolVector(nevents);
  bool *toDelete = newBoolVector(nevents);
  int found;
  
#ifdef __SRL_TRACE__
  printf("lpnProjectZone():Start\n");
  printZone(events,zone);
#endif
  
  /* find events that aren't enabled and mark them */
  for(i=0;i<zone->numTransitions;i++) {
    if(marking->enablings[zone->curTransitions[i].enabled] == 'F') {
      disabled[count] = zone->curTransitions[i].enabled;
      count++;
    }
  }
  
  if(!poReduce) {
    finalZone = copyZone(zone);
    /* CONSTRAINTS -- go through all of the past variables to see
       which ones are still needed for constraints and which ones can
       be safely deleted */
    for(i=0;i<finalZone->pastFutureSep;i++) {
#ifdef __CONSTRAINT_DEBUG__OFF
      printf("Working on: %s\n",
             events[finalZone->curTransitions[i].enabled]->event);
#endif
      if(!lpnConstraintMarked(finalZone,i,marking,rules,nevents,nplaces,
                              events)) {
        newZone = lpnDeleteTransition(finalZone,i);
        lpnFreeZone(finalZone);
        finalZone = copyZone(newZone);
        lpnFreeZone(newZone);
        i--; /* decrement i to compensate for the event that was just
                removed */
#ifdef __CONSTRAINT_DEBUG__OFF
        printf("Deletion occurred.\n");
        printZone(events,finalZone);
#endif
      }
    }
    
    /* event has been deleted -- now look for other events that have
       been disabled, but are still in the zone and remove them as
       well */
    for(i=0;i<count;i++) {
      for(j=finalZone->pastFutureSep;j<finalZone->numTransitions;j++) {
        if(disabled[i] == finalZone->curTransitions[j].enabled) {
          /* this looks ugly and it is, but it helps prevent a meory
             leak here.  There may be a better way, but I can't see it
             right now */
          newZone = lpnDeleteTransition(finalZone,j);
          lpnFreeZone(finalZone);
          finalZone = copyZone(newZone);
          lpnFreeZone(newZone);
          break;
        }
      }
    }
  }
  else {
    finalZone = copyZone(zone);
    /* look for events that have been disabled and are NOT past
       variables, but are still in the zone and remove them */
    for(i=0;i<count;i++) {
      for(j=finalZone->pastFutureSep;j<finalZone->numTransitions;j++) {
        if(disabled[i] == finalZone->curTransitions[j].enabled) {
          /* this looks ugly and it is, but it helps prevent a meory
             leak here.  There may be a better way, but I can't see it
             right now */
#ifdef __POSET_DEBUG__
          printf("deleting: %d\n",j);
#endif
          newZone = lpnDeleteTransition(finalZone,j);
          lpnFreeZone(finalZone);
          finalZone = copyZone(newZone);
          lpnFreeZone(newZone);
          break;
        }
      }
    }
    /* step through the not enabled events and see if any of those events
       cause one of the past variables to be kept around */
#ifdef __POSET_DEBUG__
    cSetFg(RED);
    printf("not enabled check:start\n");
    cSetAttr(NONE);
#endif
    for(i=1;i<nevents;i++) {
      if(marking->enablings[i] == 'F') {
        lpnNeedToKeep(finalZone,i,marking,events,rules,
                      nevents,nplaces,keepers);
        unionBoolVector(finalKeepers,keepers,nevents);
        emptyBoolVector(keepers,nevents);
      }
    }
#ifdef __POSET_DEBUG__
    cSetFg(RED);
    printf("not enabled check:end\n");
    cSetAttr(NONE);
    printf("About to examine up to event %d\n",finalZone->pastFutureSep);
#endif
    /* TBD: there is a better way to do this.  think about it later
       when performance matters */
    /* determine which transitions to delete */
    for(i=0;i<finalZone->pastFutureSep;i++) {
#ifdef __POSET_DEBUG__
      printf("Examining...%s\n",events[finalZone->curTransitions[i].enabled]->
             event);
#endif
      if(!finalKeepers[finalZone->curTransitions[i].enabled]) {
#ifdef __POSET_DEBUG__
        printf("%s:%d is not a keeper.\n",
               events[finalZone->curTransitions[i].enabled]->event,
               finalZone->curTransitions[i].enabled);
#endif
        toDelete[finalZone->curTransitions[i].enabled] = true;
      }
#ifdef __POSET_DEBUG__
      else {
        printf("keeping %s:%d\n",
               events[finalZone->curTransitions[i].enabled]->event,
               finalZone->curTransitions[i].enabled);
      }
#endif
    }

    /* CONSTRAINTS -- add another for loop here to check and make sure
       that events that are past variables and marked for deletion are
       no longer marked for deletion */
    
    /* now actually delete the transitions */
    for(i=0;i<nevents;i++) {
      if(toDelete[i]) {
        found = 0;
        for(j=0;j<finalZone->pastFutureSep;j++) {
          if(finalZone->curTransitions[j].enabled == i) {
            found = 1;
            if(!lpnConstraintMarked(finalZone,j,marking,rules,nevents,nplaces,
                                    events)) {
              newZone = lpnDeleteTransition(finalZone,j);
              lpnFreeZone(finalZone);
              finalZone = copyZone(newZone);
              lpnFreeZone(newZone);
            }
          }
        }
        if(!found) {
          cSetFg(RED);
          printf("Error: ");
          cSetAttr(NONE);
          printf("lpnNeedToKeep indicated that a transition should be deleted, but it wasn't found in the zone.\n");
        }
      }
    }
  }
#ifdef __SRL_TRACE__
  printf("lpnProjectZone():End\n");
#endif
  
  free(keepers);
  free(finalKeepers);
  free(toDelete);
  return finalZone;
}

/*****************************************************************************
* Creates the initial zone.
******************************************************************************/
srlZoneADT lpnFindInitialZone(ruleADT **rules,markkeyADT *markkey,
			      eventADT *events,int nevents,int nrules,
			      int nplaces,markingADT marking,bool poReduce)
{
  srlZoneADT zone;
  srlZoneADT newZone;
  struct bounds time;
  int i,j,event;
  int size = 0;
  int constraintCnt = 0;
  bool *constraint;
  
#ifdef __SRL_DEBUG__
  printf("lpnFindInitialZone():Start\n");
  cSetFg(GREEN);
  printf("Initial Marking: ");
  cSetAttr(NONE);
#endif
  
  /* determine the size for the initial zone */
  for(i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      size++;
#ifdef __SRL_DEBUG__
      if(size > 1) {
	printf(", %s [%d]",events[i]->event,i);
      }
      else {
	printf("%s [%d]",events[i]->event,i);
      }
#endif
    }
  }
#ifdef __SRL_DEBUG__
  printf("\n");
#endif

  /* it is size+1 b/c we use the reset event as an initial dummy timer
     to setup the zone properly */
  zone = lpnCreateZone(size+1);

  /* add the reset event into the zone and have that reflect in the
     size variable */
  zone->pastFutureSep = 1;
  zone->curTransitions[0].enabled = 0;
  size = 1;
  constraint = newBoolVector(nevents);
  
  /* add the initial transitions and bounds into the zone */
  for(i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      zone->curTransitions[size].enabled = i;
      constraint[i] = false;
      time = lpnFindBounds(i,rules,nevents,nplaces);
      zone->matrix[0][size] = -1 * time.lower;
      zone->matrix[size][0] = time.upper;
      size++;
    }
  }
  
  /* add in the events required to verify correct constraints -- this
     means adding the events that are in the preset of the constraint
     rules for the initially enabled events */
  for(i=1;i<zone->numTransitions;i++) {
#ifdef __CONSTRAINT_DEBUG__
    printf("Working on... %s.\n",
           events[zone->curTransitions[i].enabled]->event);
#endif
    for(j=1;j<nevents+nplaces;j++) {
      if(rules[j][zone->curTransitions[i].enabled]->ruletype & ORDERING) {
        if(!constraint[j]) {
          constraintCnt++;
          constraint[j] = true;
#ifdef __CONSTRAINT_DEBUG__
          printf("%s should be added to the init zone.\n",
                 events[j]->event);
#endif
        }
      }
    }
  }

  /* unmark events already in the zone */
  for(i=1;i<zone->numTransitions;i++) {
    constraint[zone->curTransitions[i].enabled] = false;
  }

#ifdef __CONSTRAINT_DEBUG__
  printf("Adding %d events to the initial zone.\n",constraintCnt);
#endif
  /* add the new events */
  newZone = lpnAddDim(zone,constraintCnt);
  size = zone->numTransitions;
  for(i=0;i<nevents;i++) {
    if(constraint[i]) {
      newZone->curTransitions[size].enabled = i;
      time = lpnFindBounds(i,rules,nevents,nplaces);
      newZone->matrix[0][size] = -1 * time.lower;
      newZone->matrix[size][0] = time.upper;
      lpnFreeZone(zone);
      event = size;
      /* make the events added solely for the purpose of satisfying
         constraints past events b/c this is what they really are */
      zone = lpnMakePast(newZone,&event);
      lpnFreeZone(newZone);
      newZone = copyZone(zone);
      size++;
    }
  }
  
#ifdef __SRL_DEBUG__
  printZone(events,newZone);
#endif
  
  lpnRecanonZone(newZone);
  checkConsistentZone(events,newZone);

#ifdef __SRL_DEBUG__
  printf("size: %d\n",size);
  printf("Result of recanon.\n");
  printZone(events,newZone);
#endif
  
  /* the reset event has served its purpose so "fire" it and allow it
     to be projected out */
  lpnFreeZone(zone);
  zone = lpnProjectZone(newZone,0,marking,poReduce,events,rules,nevents,
                           nplaces);
  lpnFreeZone(newZone);
  
#ifdef __SRL_DEBUG__
  printf("Result of lpnFindInitialZone().\n");
  printZone(events,zone);
#endif
  
  return zone;
}

/*****************************************************************************
* Find a new zone based on the firing of event trans.
******************************************************************************/
srlZoneSet lpnFindNewZone(ruleADT **rules,markkeyADT *markkey,
                          eventADT *events,int nevents,int nrules,
                          int nplaces,markingADT marking,int trans,
                          srlZoneADT zone,bool poReduce,char *firelist,
                          markingADT oldMarking,bool nofail)
{
  int i,j,k,found;
  int index = -1;
  int newTrans = 0;
  bounds newBound;
  srlZoneADT newZone = NULL;
  srlZoneADT finalZone = NULL;
  srlZoneADT swapZone = NULL;
  srlZoneSet zoneS = NULL;
  srlZoneSet finalZoneS = NULL;
  parentADT *trueP;
  tpList TP;
#ifdef __POSET_DEBUG__OFF
  srlZoneADT recZone;
#endif

#ifdef __SRL_TRACE__
  printf("lpnFindNewZone():start\n");
#endif
  
  /* allocate the true parent array */
  if(poReduce) {
    trueP = lpnCreateParent(nevents);
  }  

  /* find the index in the zone->curTransitions matrix for the fired
     event so that this information can be passed to the
     lpnDeleteTransition function */
  for(i=0;i<zone->numTransitions;i++) {
    if(trans == zone->curTransitions[i].enabled) {
      index = i;
    }
  }

  if(index < 0) {
    if(trans < 0) {
      cSetFg(RED);
      printf("Warning: ");
      cSetAttr(NONE);
      printf("The transition passed to lpnFindNewZone is invalid.(%d)",trans);
    }
    else { 
      cSetFg(RED);
      printf("Warning: ");
      cSetAttr(NONE);
      printf("The transition (%d of %d) that just fired was not found in the zone.  Adjusting internal variables and continuing.\n",trans,nevents);
#ifdef __SRL_DEBUG__
      printZone(events,zone);
#endif
      /* set index to 0 to avoid seg faults */
      index = 0;
    }
  }

  /* CONSTRAINTS -- here we need to check to see if a constraint
     failure occurred upon firing the rule.  A function should be
     created to do this. */
  if(!lpnChkMinConstraint(trans,index,rules,nevents,nplaces,zone,events,
                          marking,nofail)) {
    lpnFreeZone(zone);
    return NULL;
  }
  
  if(poReduce) {
#ifdef __POSET_DEBUG__OFF
    printf("Original zone.\n");
    printZone(events, zone);
    printf("pre-index: %d\n",index);
    printf("%s should be swapped to %d.\n",
           events[zone->curTransitions[index].enabled]->event,
           zone->pastFutureSep);
#endif

    /* step 4 in Tomohiro's algorithm */
    swapZone = lpnMakePast(zone, &index);

#ifdef __POSET_DEBUG__OFF
    printf("post-index: %d\n",index);
    printZone(events, swapZone);
    recZone = copyZone(swapZone);
    lpnRecanonZone(recZone);
    if(!equalZone(recZone, swapZone)) {
      cSetFg(RED);
      printf("Warning: ");
      cSetAttr(NONE);
      printf("Recanonicalize changed the zone.  Does this mean that the swap happened incorrectly?\n");
    }
    lpnFreeZone(recZone);
#endif
  }
  else {
    swapZone = lpnMakePast(zone, &index);
  }
  
  /* compare the currently enabled transitions w/ the previously
  enabled transitions and determine how many newly enabled transitions
  are to be added to the zone */
#ifdef __POSET_DEBUG__
  printf("Looking for newly enabled events.\n");
#endif
  for(i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      found = 0;
      for(j=0;j<swapZone->numTransitions;j++) {
        if(i == swapZone->curTransitions[j].enabled) {
          found = 1;
          break;
        }
      }
      if(!found) {
        newTrans++;
        if(poReduce) {
#ifdef __POSET_DEBUG__
          printf("Newly enabled event: %s\n",events[i]->event);
#endif
          trueP[i].event = true;
        }
      }
    }
  }
  
  if(poReduce) {
    if(swapZone->tP != NULL) {
      cSetFg(RED);
      printf("Warning: ");
      cSetAttr(NONE);
      printf("This zone has a true parent set before lpnTrueParent() is actually called.\n");
      /* we should remove all true parents before calling true parent
         so go ahead and do that here */
      TP = removeTP(swapZone);
      while(TP != NULL) {
        free(TP);
        TP = removeTP(swapZone);
      }
    }
    lpnTrueParent(swapZone,trueP,oldMarking,events,rules,nevents,nplaces,
                  firelist,&zoneS,0);
    /* this happens if there are not newly enabled transitions and
       poReduce is turned on */
    if(zoneS == NULL) {
#ifdef __SRL_DEBUG__
      cSetFg(RED);
      printf("zoneS is NULL!!! -- %p\n",zoneS);
      cSetAttr(NONE);
#endif
      addZoneS(swapZone,&zoneS);
    }
    else {
      lpnFreeZone(swapZone);
    }
  }
  else {
    /* TBD: the zero is to prevent segfaults -- a more elegant
       solution would be nice */
    addTP(swapZone,0,index);
    addZoneS(swapZone,&zoneS);
  }
  
  while(zoneS != NULL) {
    swapZone = removeZoneS(&zoneS);
    
    /* increase the size of the zone if needed */
    if(newTrans == 0) {
      /* the delete code is based on having a newZone, but nothing
         changes here so a simple copy is used */
      newZone = copyZone(swapZone);
      if(!poReduce) {
        TP = removeTP(newZone);
      }
    }
    else {
#ifdef __SRL_DEBUG__
      printf("NewTrans: %d\n",newTrans);
      printZone(events,swapZone);
#endif
      newZone = lpnAddDim(swapZone,newTrans);
#ifdef __SRL_DEBUG__OFF
      printPlainZone(events,newZone);
#endif
      /* add the newly enabled transitions to the zone */
      newTrans = 0;
      for(i=0;i<nevents;i++) {
        if(marking->enablings[i] == 'T') {
#ifdef __POSET_DEBUG__
          printf("Working on %s -- %d.\n",events[i]->event,i);
#endif
          found = 0;
          for(j=0;j<swapZone->numTransitions;j++) {
            if(i == swapZone->curTransitions[j].enabled) {
#ifdef __POSET_DEBUG__
              printf("Found %s in position %d.\n",events[i]->event,j);
#endif
              found = 1;
            }
          }
#ifdef __POSET_DEBUG__
          printf("Found: %d.\n",found);
#endif
          if(!found) {
#ifdef __SRL_DEBUG__
            printf("setting %d to %d\n",swapZone->numTransitions+newTrans,i);
            printf("event:%s\n",events[i]->event);
#endif
            newZone->curTransitions[swapZone->numTransitions+newTrans].enabled
              = i;
            newTrans++;
          }
        }
      }
#ifdef __SRL_DEBUG__OFF
      printPlainZone(events,newZone);
#endif
      
      TP = removeTP(newZone);
      while(TP != NULL) {        
#ifdef __SRL_DEBUG__
        printf("True parent is %s for %s\n",
               events[newZone->curTransitions[TP->tp].enabled]->event,
               events[TP->tn]->event);
#endif
        /* add in the bounds between the new transitions and their true
           parents if a true parent exists */
        if(TP->tp >= 0) {
          if(poReduce) {
#ifdef __POSET_DEBUG__
            printf("swapZone:%d newZone:%d\n",swapZone->numTransitions,
                   newZone->numTransitions);
#endif
            for(i=swapZone->numTransitions;i<newZone->numTransitions;i++) {
              if(newZone->curTransitions[i].enabled == TP->tn) {
              
                newBound = lpnFindBounds(newZone->curTransitions[i].enabled,
                                         rules,nevents,nplaces);
#ifdef __POSET_DEBUG__
                printf("setting matrix[%d][%d]\n",TP->tp,i);
#endif
                newZone->matrix[TP->tp][i] = -1 * newBound.lower;
                newZone->matrix[i][TP->tp] = newBound.upper;
                /* add in Eft contraints between all possible parents and the
                   new transitions */
                for(j=0;j<nevents;j++) {
                  if(trueP[newZone->curTransitions[i].enabled].parents[j]) {
                    for(k=0;k<newZone->numTransitions;k++) {
                      if(newZone->curTransitions[k].enabled == j) {
#ifdef __POSET_DEBUG__
                        printf("setting matrix[%d][%d] (Eft only)\n",
                               TP->tp,k);
#endif
                        /* keep the zone consistent */
                        if(TP->tp != k) {
                          newZone->matrix[k][i] = -1 * newBound.lower;
                        }
                      }
                    }
                  }
                }
                break;
              }
            }
          }
          else {
            /* add constraint that transition fired before all others in
               zone */
#ifdef __SRL_DEBUG__
            printf("Constraining the zone around %d.\n",TP->tp);
#endif
            for(i=swapZone->numTransitions;i<newZone->numTransitions;i++) {
              newBound = lpnFindBounds(newZone->curTransitions[i].enabled,
                                       rules,nevents,nplaces);
              newZone->matrix[TP->tp][i] = -1 * newBound.lower;
              newZone->matrix[i][TP->tp] = newBound.upper;
            }
          }
        }
        free(TP);
        TP = removeTP(newZone);
      }
    }

    lpnFreeZone(swapZone);

    if(!poReduce) {
      for(i=newZone->pastFutureSep;i<newZone->numTransitions;i++) {
        if(newZone->matrix[index][i] > 0) {
          newZone->matrix[index][i] = 0;
        }
      }
    }
    
#ifdef __SRL_DEBUG__
    printf("New zone that has been increased by %d transition(s) before calling recanonicalize.\n",newTrans);
    printZone(events,newZone);
#endif
  
    /* recanonicalize and check consistency then be done */
    lpnRecanonZone(newZone);
  
#ifdef __SRL_DEBUG__
    printf("After recanonicalization.\n");
    printZone(events,newZone);
#endif
  
    checkConsistentZone(events, newZone);
   
#ifdef __SRL_DEBUG__
    printf("Deleting transition: %d very soon\n",index);
    printZone(events,newZone);
#endif
  
    finalZone = lpnProjectZone(newZone,index,marking,poReduce,events,rules,
                               nevents,nplaces);
    lpnFreeZone(newZone);
  
#ifdef __SRL_DEBUG__
    printf("New zone found by lpnFindNewZone().\n");
    printZone(events,finalZone);
#endif

    if(!poReduce) {
      addZoneS(finalZone,&finalZoneS);
    }
    else {
      /* if the zone is no longer consistent then this indicates that
         it is not possible for that variable to be the true parent so
         delete the invalid zone and move on */
      if(!checkConsistentZone(events,finalZone)) {
#ifdef __POSET_DEBUG__
        cSetFg(BLUE);
        printf("True parent created an inconsistent zone.\n");
        cSetAttr(NONE);
#endif
      
        lpnFreeZone(finalZone);
      }
      else {
        addZoneS(finalZone,&finalZoneS);
      }
    }
  }

  /* free the true parent array */
  if(poReduce) {
    lpnFreeParent(trueP,nevents);
  }
  
#ifdef __SRL_TRACE__
  printf("lpnFindNewZone:end\n");
#endif
  
  return finalZoneS;
}

/*****************************************************************************
 * Determine which enabled event(s) are actually fireable under timing.
******************************************************************************/
char *lpnCanFireFirst(markingADT marking, srlZoneADT zone, int nevents,
		     ruleADT **rules,eventADT *events)
{
#ifdef __SRL_TRACE__
  printf("lpnCanFireFirst():start\n");
#endif
  int i,j;
  char *fireable = (char *)GetBlock((nevents+1)*sizeof(char));
  fireable[nevents] = '\0';

#ifdef __SRL_DEBUG__OFF
  printZone(events,zone);
#endif

  fireable[0] = 'F';
  for(i=1;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      fireable[i] = 'T';
    }
    else {
      fireable[i] = 'F';
    }
  }

  /* nullify those transitions that aren't firable first */
  /*   for(i=0;i<zone->numTransitions;i++) { */
  for(i=zone->pastFutureSep;i<zone->numTransitions;i++) {
    /*     for(j=0;j<zone->numTransitions;j++) { */
    for(j=zone->pastFutureSep;j<zone->numTransitions;j++) {
      if(zone->matrix[i][j] < 0) {
#ifdef __SRL_DEBUG__
        printf("nevents: %d, event: %d\n",nevents, zone->curTransitions[j].enabled);
#endif
        fireable[zone->curTransitions[j].enabled] = 'F';
      }
    }
  }

#ifdef __SRL_DEBUG__
  printf("Fireable list: ");
  for(i=1;i<nevents;i++) {
    if(fireable[i] == 'T') {
      printf("%s[%d] ",events[i]->event,i);
    }
  }
  printf("\n");
#endif 
  
#ifdef __SRL_TRACE__
  printf("lpnCanFireFirst():end\n");
#endif
  return fireable;
}

/*****************************************************************************
* Push an item onto the state space stack.
******************************************************************************/
void lpnPushMarking(srlStackADT *stateSpaceStack, char *firelist,
		    srlZoneADT zone, markingADT marking)
{
#ifdef __SRL_TRACE__
  printf("lpnPushMarking():start\n");
#endif
  
  srlStackADT newStateSpaceStack = NULL;

  newStateSpaceStack = (srlStackADT)GetBlock(sizeof *newStateSpaceStack);
  newStateSpaceStack->firelist = firelist;
  newStateSpaceStack->zone = zone;
  newStateSpaceStack->marking = marking;
  newStateSpaceStack->next = *stateSpaceStack;
  *stateSpaceStack = newStateSpaceStack;

#ifdef __SRL_DEBUG__
  cSetFg(YELLOW);
  printf("Pushing....\n");
  printf("state: %s\n",marking->state);
  printf("firelist: %s\n",firelist);
  cSetAttr(NONE);
#endif
  
#ifdef __SRL_TRACE__
  printf("lpnPushMarking():end\n");
#endif
}

/*****************************************************************************
* Pop an entry from the state space stack.
******************************************************************************/
markingADT lpnPopMarking(srlStackADT *stateSpaceStack, char **firelist,
			 srlZoneADT *zone)
{
#ifdef __SRL_TRACE__
  printf("lpnPopMarking():start\n");
#endif
  
  markingADT marking;
  srlStackADT oldStateSpaceStack;

  if(*stateSpaceStack == NULL) {
#ifdef __SRL_DEBUG__
    printf("pop is returning NULL.\n");
#endif
    return NULL;
  }

  marking = (*stateSpaceStack)->marking;
  *firelist = (*stateSpaceStack)->firelist;
  *zone = (*stateSpaceStack)->zone;
  oldStateSpaceStack = *stateSpaceStack;
  *stateSpaceStack = (*stateSpaceStack)->next;
  free(oldStateSpaceStack);

#ifdef __SRL_DEBUG__
  if(marking == NULL) {
    cSetFg(RED);
    printf("Note: ");
    cSetAttr(NONE);
    printf("pop is returning NULL.\n");
  }
#endif
  
#ifdef __SRL_TRACE__
  printf("lpnPopMarking():end\n");
#endif
  
  return marking;
}

/*****************************************************************************
* Add an event to the traceList.
******************************************************************************/
void abAddTraceList(int event,traceListADT *concTrace) 
{
#ifdef __SRL_TRACE__
  printf("abAddTraceList():start\n");
#endif
  
  traceListADT newItem = NULL;
  traceListADT iList;
  
  newItem = (traceListADT)GetBlock(sizeof *newItem);
  newItem->event = event;
  newItem->next = NULL;
  
  iList = *concTrace;
  
  if(!iList) {
    *concTrace = newItem;
  }
  else {
    
    while(iList->next != NULL) {
      iList = iList->next;
    }

    iList->next = newItem;
  }
  
#ifdef __SRL_TRACE__
  printf("abAddTraceList():end\n");
#endif
}

/*****************************************************************************
* Add an event to the traceList.
******************************************************************************/
traceListADT abCopyTraceList(traceListADT concTrace) 
{
  traceListADT newConcTrace;
  traceListADT item;

  item = concTrace;
  
  while(item->next) {  
    abAddTraceList(item->event,&newConcTrace);
    item = item->next;
  }

  return newConcTrace;
}

/*****************************************************************************
* Free the entire traceList.
******************************************************************************/
void abFreeTraceList(traceListADT concTrace) 
{
  traceListADT iListPrev;
  traceListADT iList;

  iList = concTrace->next;
  free(concTrace);
  while(iList) {
    iListPrev = iList;
    iList = iListPrev->next;
    free(iListPrev);
  }
}

/*****************************************************************************
* Print all events in the trace list.
******************************************************************************/
void abPrintTraceList(traceListADT concTrace, eventADT *events) 
{
  traceListADT iList;

  printf("Error trace: ");
  fprintf(lg,"Error trace: ");
  iList = concTrace;
  while(iList) {
    printf("%s, ",events[iList->event]->event);
    fprintf(lg,"%s, ",events[iList->event]->event);
    iList = iList->next;
  }
  printf("\n");
}

/*****************************************************************************
* Copy an srlListADT
******************************************************************************/
srlListADT abCopyListADT(srlListADT oldList) 
{
  int i;
  srlListADT newList = (srlListADT)GetBlock(sizeof(*newList));
  newList->pos = oldList->pos;
  newList->tot = oldList->tot;

  newList->array = (int *)GetBlock((sizeof i) * newList->tot);
  
  for(i=0;i<newList->tot;i++) {
    newList->array[i] = oldList->array[i];
  }
  return newList;
}

/*****************************************************************************
* Push an item onto the state space stack used w/ abstraction traces.
******************************************************************************/
void abPushMarking(abStackADT *abStack, char *firelist,srlZoneADT zone,
                   markingADT marking, srlListADT abTrace, srlListADT Eset,
                   traceListADT concTrace)
{
#ifdef __SRL_TRACE__
  printf("abPushMarking():start\n");
#endif
  
  abStackADT newAbStack = NULL;

  newAbStack = (abStackADT)GetBlock(sizeof *newAbStack);
  newAbStack->firelist = firelist;
#ifdef __TRACE_DEBUG__
  printf("Pushing zone [%p] w/ %d transitions.\n",zone,zone->numTransitions);
#endif
  newAbStack->zone = zone;
  newAbStack->marking = marking;
  newAbStack->abTrace = abTrace;
  newAbStack->Eset = Eset;
  newAbStack->concTrace = concTrace;
  newAbStack->next = *abStack;
  *abStack = newAbStack;

#ifdef __SRL_DEBUG__
  cSetFg(YELLOW);
  printf("Pushing....\n");
  printf("state: %s\n",marking->state);
  printf("firelist: %s\n",firelist);
  cSetAttr(NONE);
#endif
  
#ifdef __SRL_TRACE__
  printf("abPushMarking():end\n");
#endif
}

/*****************************************************************************
* Pop an entry from the state space stack used w/ abstraction traces.
******************************************************************************/
markingADT abPopMarking(abStackADT *abStack, char **firelist,srlZoneADT *zone,
                        srlListADT *abTrace, srlListADT *Eset,
                        traceListADT *concTrace)
{
#ifdef __SRL_TRACE__
  printf("abPopMarking():start\n");
#endif
  
  markingADT marking;
  abStackADT oldAbStack;

  if(*abStack == NULL) {
#ifdef __SRL_DEBUG__
    printf("pop is returning NULL.\n");
#endif
    return NULL;
  }

  marking = (*abStack)->marking;
  *firelist = (*abStack)->firelist;
  *zone = (*abStack)->zone;
#ifdef __TRACE_DEBUG__
  printf("Popped zone [%p] w/ %d transitions.\n",*zone,
         (*zone)->numTransitions);
#endif
  *abTrace = (*abStack)->abTrace;
  *Eset = (*abStack)->Eset;
  *concTrace = (*abStack)->concTrace;
  oldAbStack = *abStack;
  *abStack = (*abStack)->next;
  free(oldAbStack);

#ifdef __SRL_DEBUG__
  if(marking == NULL) {
    cSetFg(RED);
    printf("Note: ");
    cSetAttr(NONE);
    printf("pop is returning NULL.\n");
  }
#endif
  
#ifdef __SRL_TRACE__
  printf("lpnPopMarking():end\n");
#endif
  
  return marking;
}

/*****************************************************************************
* Finds the reachable states for an lpn.
******************************************************************************/
bool lpnRsg(char * filename,signalADT *signals,eventADT *events,
            ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
            int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
            char * startstate,bool si,bool verbose,bddADT bdd_state,
            bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
            int nord,bool noparg,bool search,bool useDot,bool poReduce,
            bool lpn)
{
  FILE *fp=NULL;
  markingADT marking;
  markingADT newMarking;
  markingADT pushMarking;
  /*   markingsADT markingStack=NULL; */
  srlStackADT stateSpaceStack = NULL;
  srlZoneADT curZone;
  srlZoneADT newZone;
  srlZoneSet newZoneS;
  int i,numStates,zones=0;
  bool stuck;
  char *firelist;
  char *newFirelist;
  timeval t0,t1;
  double time0, time1;
  int iter=0;
  int stackDepth=0;
  int maxDepth=0;
  double cpuTime = 0;
  int clkTicks = sysconf(_SC_CLK_TCK);
  struct tms tBuf;
  int newState;

  ltn=true;
  
  gettimeofday(&t0, NULL);
  fp = lpnInitOutput(filename,signals,nsignals,ninpsig,verbose);
  if(verbose && !fp)
    return false;
  /* clear the state table */
  initialize_state_table(LOADED,FALSE,state_table);
  
  marking = lpnFindInitialMarking(events,rules,markkey,nevents,nrules,
                                  ninpsig,nsignals,startstate,verbose);
  
  /* if no initial marking, there is problem, so bail out */
  if(marking == NULL) {
    if(verbose) fclose(fp);
    return(FALSE);
  }
  
  curZone = lpnFindInitialZone(rules,markkey,events,nevents,nrules,
                               nplaces,marking,poReduce);
  
#ifdef __TEST_DISPLAY__
  printRuleADT(rules,nevents,nplaces,events);
  printMarkingADT(marking,nevents,nrules,nsignals,events);
  printMarkkeyADT(markkey,nrules,events);
  printEventADT(events,nevents,nplaces);
  printSignalADT(signals,nsignals);
  printZone(events,curZone);
#endif
  
  /*   /\* call getAmple to get the fireable set of transitions *\/ */
  /* firelist = lpnGetAmpleSet(signals,events,rules,marking,nevents,nplaces, */
  /* 			    timeopts.PO_reduce,nsignals); */
  
  firelist = lpnCanFireFirst(marking,curZone,nevents,rules,events);
  
  /*   push_marking(&markingStack,marking,firelist); */
  lpnPushMarking(&stateSpaceStack,firelist,curZone,marking);
  
  stackDepth++;
  if(stackDepth > maxDepth) {
    maxDepth = stackDepth;
  }
  
#ifdef __SRL_DEBUG__
  printf("Adding the initial state\n");
#endif
  /* Add the initial state to the state table */
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL,
            marking->state,marking->marking,
            marking->enablings,nsignals, curZone->matrix,
            curZone->curTransitions,curZone->numTransitions,0,verbose,1,nrules,
            curZone->numTransitions,curZone->numTransitions,bdd_state,use_bdd,
            markkey,timeopts, -1, -1, NULL, rules,nevents+nplaces,1);
  numStates = 1;
  zones = 1;
  
  /* Repeat while stack is not empty */
  /*   while((marking = pop_marking(&markingStack,&firelist)) != NULL) {
   */
  while((marking = lpnPopMarking(&stateSpaceStack,&firelist,&curZone))
        != NULL) {
#ifdef __SRL_DEBUG__
    cSetFg(MAGENTA);
    printf("Firelist: ");
    for(i=1;i<nevents;i++) {
      if(firelist[i] == 'T') {
        printf("%s[%d] ",events[i]->event,i);
      }
    }
    printf("\n");
    cSetAttr(NONE);
#endif
    stackDepth--;
    stuck = TRUE;
    
    /* Fire each fireable event in turn */
    for(i=1;i<nevents;i++) {
      if(firelist[i] == 'T') {
#ifdef __SRL_DEBUG__
        cSetFg(BLUE);
        printf("Firing: ");
        cSetAttr(NONE);
        printf("%s from state %s\n",events[i]->event,marking->state);
        printZone(events,curZone);
#endif
        stuck=FALSE;  /* There exists a fireable event so no deadlock */
        iter++;

        /* check for max constraint violations each time through */
        if(lpnChkMaxConstraint(rules,nevents,nplaces,curZone,events,marking,
                               timeopts.nofail)) {
          
        
          /* Find new marking after firing event i */
          newMarking = lpnFindNewMarking(signals,events,rules,markkey,marking,
					 i,nevents,nplaces,nrules,verbose,
                                         nsignals,ninpsig,
                                         timeopts.disabling||timeopts.nofail,
                                         noparg,timeopts.nofail);
        }
        else {
          newMarking = NULL;
        }
        
        /* Find new zone */
        if(newMarking) {
#ifdef __SRL_DEBUG__
          cSetFg(BLUE);
          printf("New state:");
          cSetAttr(NONE);
          printf(" %s[%d]\n",newMarking->state,numStates);
          printZone(events,curZone);
#endif
          newZoneS = lpnFindNewZone(rules,markkey,events,nevents,nrules,
                                    nplaces,newMarking,i,curZone,poReduce,
                                    firelist,marking,timeopts.nofail);

#ifdef __SRL_DEBUG__
          if(newZoneS == NULL) {
            cSetFg(RED);
            printf("newZoneS is NULL!!!\n");
            cSetAttr(NONE);
          }
#endif
          
        }        
        /* No new marking found, so something went wrong, bail out */
        if(!newMarking || !newZoneS) {
/* printf("Error: No new marking or new zone found!\n"); */
          newState = add_state(fp,state_table,marking->state,marking->marking,
                               marking->enablings,NULL,NULL,0,NULL,NULL,NULL,
                               NULL,nsignals,NULL,NULL,0,numStates,verbose,
                               FALSE,nrules,0,0,bdd_state,use_bdd,markkey,
                               timeopts,-1,i,NULL,rules,nevents);
          if(newState) {
            numStates++;
            zones++;
          }
          if((!timeopts.nofail) && (!timeopts.keepgoing)) {
            return false;
          }
        }

        while(newZoneS != NULL) {
          newZone = removeZoneS(&newZoneS);
          
          /* Check if new marking is a new state */
          if(newMarking) {     
#ifdef __SRL_DEBUG__
            cSetFg(YELLOW);
            printf("Trying to add this zone\n");
            cSetAttr(NONE);
            printZone(events,newZone);
#endif
            /* TBD: We need to step through all the zones in the set
               here! */
            newState = add_state(fp,state_table,marking->state,
                                 marking->marking,marking->enablings,
                                 curZone->matrix,curZone->curTransitions,
                                 curZone->numTransitions,NULL,
                                 newMarking->state,newMarking->marking,
                                 newMarking->enablings,nsignals,
                                 newZone->matrix,newZone->curTransitions,
                                 newZone->numTransitions,
                                 numStates,verbose,1,nrules,
                                 newZone->numTransitions,
                                 newZone->numTransitions,
                                 bdd_state,use_bdd,markkey,timeopts,-1,i,NULL,
                                 rules,nevents+nplaces,1);
          }
          else {
            newState = 0;
          }
        
          if(newState != 0) {
            if(newState > 0)
              zones++;
            else { 
              zones = zones+newState+1;
            }
          
            if (newState == 1) {
#ifdef __SRL_DEBUG__
              cSetFg(CYAN);
              printf("New state: %d\n",numStates);
              cSetAttr(NONE);
#endif
              numStates++;
            } else {
#ifdef __SRL_DEBUG__
              cSetFg(CYAN);
              printf("New zone\n");
              cSetAttr(NONE);
#endif
            }
          
            /* Display state counts and memory usage */
            lpnDisplayInfo(iter,numStates,zones,stackDepth,1);
            /* Find new fireable event list */
            /* Push new marking and fireable list to the stack */
            /*newFirelist=lpnGetAmpleSet(signals,events,rules,newMarking,nevents,
              nplaces,timeopts.PO_reduce,nsignals);*/
            newFirelist = lpnCanFireFirst(newMarking,newZone,nevents,rules,
                                          events);

            /* since the newMarking was created outside the while loop
               we need to create a new one inside the loop */
            pushMarking = lpnCopyMarking(newMarking);
            
            /* 	  push_marking(&markingStack,newMarking,newFirelist); */
            lpnPushMarking(&stateSpaceStack,newFirelist,newZone,pushMarking);
          
            stackDepth++;
            if(stackDepth>maxDepth) {
              maxDepth=stackDepth;
            }
          }
          else {
#ifdef __SRL_DEBUG__
            cSetFg(CYAN);
            printf("Same state.\n");
            cSetAttr(NONE);
#endif
            /* New marking is not a new state, so clean up and continue */
            if(newZone) {
              lpnFreeZone(newZone);
            }
          }
        } /* while(newZoneS != NULL) */
        /* we are done w/ the newMarking so free it.  This is safe b/c
           each time it was pushed on the stack a new copy was made */
        if(newMarking) {
          lpnDeleteMarking(newMarking);
          free(newMarking);
        } 
      }
      
      /* No events fireable from this marking, so deadlock has been found */
    }
    if(stuck) {
      if(lpnFoundDeadlock(events,rules,markkey,marking,nevents,nplaces,
                          nrules,verbose,timeopts.nofail,search,nsignals)) {
        add_state(fp,state_table,marking->state,marking->marking,
                  marking->enablings,NULL,NULL,0,NULL,NULL,NULL,NULL,
                  nsignals,NULL,NULL,0,numStates,verbose,FALSE,nrules,0,0,
                  bdd_state,use_bdd,markkey,timeopts,-1,nevents,NULL,rules,
                  nevents);
        if (verbose) fclose(fp);
        return FALSE;
      }
    }
    /* Cleanup memory before looping back */
    lpnDeleteMarking(marking);
    free(marking);
    free(firelist);
    lpnFreeZone(curZone);
  }
  
  /* Statistics from here on out */
  gettimeofday(&t1,NULL);
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  
  if(times(&tBuf) >= 0) {
    cpuTime = (float)(tBuf.tms_utime + tBuf.tms_stime)/(float)clkTicks;
  }
  
  if(verbose) {
    printf("%d states explored in %f clock ticks\n",numStates,cpuTime);
    fprintf(lg,"%d states explored in %f clock ticks\n",numStates,cpuTime);
    fclose(fp);
  } else {
    printf("Done (zones: %d states: %d  cpuTime: %f wallClkTime: %f)\n",zones,
           numStates,cpuTime,time1-time0);
    fprintf(lg,"Done (zones: %d states: %d cpuTime: %f wallClkTime: %f)\n",
            zones,numStates,cpuTime,time1-time0);
  }
  return TRUE;
}

/*****************************************************************************
* Parses the trace output file and creates a linked list of the
* abstracted error trace.
******************************************************************************/
bool parseFile(char *filename,eventADT *events,int nevents,srlListADT abTrace,
	       bool verbose)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  char delim = ' ';
  char *token;
  char **tokenList = NULL;
  char *trace;
  int size;
  char *chr;
  char *prevChr;
  int tokenCnt = 0;
  int found;
  int i,j;
  int *traceList;
  int traceCnt;
  
  strncpy(outname,filename,FILENAMESIZE);
  strcat(outname,".trace");
  if ((fp = fopen(outname,"r"))==NULL) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Unable to open %s.trace.\n",filename);
    fprintf(lg,"ERROR: Unable to open %s.trace.\n",filename);
   }

  /* clear the trace array and then read it in */
  trace = (char*)GetBlock((sizeof delim) * TRACE_SIZE);
  memset((void *)trace,'\0',TRACE_SIZE);
  size = fread(trace,1,TRACE_SIZE-1,fp);
  fclose(fp);
  /* the trace is longer than the input array -- we error out right
     now -- later on we should probably do something useful here like
     figuring out the file size and basing the array on that */
  if(!(size < TRACE_SIZE)) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Trace is too large for the static array being used.\n");
    fprintf(lg,"ERROR: Trace is too large for the static array being used.\n");
    return false;
  }

#ifdef __SRL_DEBUG__
  printf("Read: %s",trace);
#endif

  /* determine how many tokens we will find so an appropriate array
     can be created */
  chr = strchr(trace,delim);
  tokenCnt++;
  
  while(chr) {
    prevChr = chr+1;
    chr = strchr(prevChr,delim);
    tokenCnt++;
  }

  /* create the array and parse the string */
  tokenList = (char**)GetBlock((sizeof token) * tokenCnt);
  tokenCnt = 0;
  
  chr = strchr(trace,delim);
  token = (char*)GetBlock((sizeof delim) * ((chr-trace)+1));
  strncpy(token,trace,(chr-trace));
  token[chr-trace] = '\0';
  tokenList[tokenCnt] = token;
  tokenCnt++;

  while(1) {
    prevChr = chr+1;
    chr = strchr(prevChr,delim);
    if(!chr) {
      break;
    } 
    token = (char*)GetBlock((sizeof delim) * ((chr-prevChr)+1));
    strncpy(token,prevChr,(chr-prevChr));
    token[chr-prevChr] = '\0';
    tokenList[tokenCnt] = token;
    tokenCnt++;
  }
  
#ifdef __SRL_DEBUG__OFF
  printf("Found %d tokens.\n",tokenCnt);
  for(i=0;i<tokenCnt;i++) {
    printf("%d:%s:\n",i,tokenList[i]);
  }
#endif

  traceList = (int*)GetBlock((sizeof found) * (tokenCnt/2));
  traceCnt = 0;
  
  for(i=0;i<tokenCnt;i+=2) {
    found = 0;
    for(j=1;j<nevents;j++) {
      if(strcmp(tokenList[i],events[j]->event) == 0) {
        traceList[traceCnt] = j;
        traceCnt++;
        found = 1;
        break;
      }
    }
    if(!found) {
      if (!verbose) {
	printf("ERROR!\n");
	fprintf(lg,"ERROR!\n");
      }
      printf("ERROR: %s was not found in the events.\n",tokenList[i]);
      fprintf(lg,"ERROR: %s was not found in the events.\n",tokenList[i]);
      return false;
    }
  }

  abTrace->pos = 0;
  abTrace->tot = tokenCnt/2;
  abTrace->array = traceList;

  /* we need to free the tokenList here */
  for(i=0;i<tokenCnt;i++) {
    free(tokenList[i]);
  }
  free(tokenList);
  free(trace);
  
  return true;
}

/*****************************************************************************
* Handle the freeing of the memory allocated by findTrace
******************************************************************************/
void cleanupFindTrace(srlListADT abTrace,srlListADT abEvents,
                      markingADT marking,srlZoneADT curZone,char *firelist,
                      srlListADT Eset,traceListADT concTrace,
                      abStackADT abStack)
{
  traceListADT prevConcTrace = NULL;
  
  if(abTrace) {
    free(abTrace->array);
    free(abTrace);
  }

  if(abEvents) {
    free(abEvents->array);
    free(abEvents);
  }

  if(marking) {
    lpnDeleteMarking(marking);
    free(marking);
  }
  
  if(curZone) {
    lpnFreeZone(curZone);
  }

  if(firelist) {
    free(firelist);
  }

  if(Eset) {
    free(Eset->array);
    free(Eset);
  }

  while(concTrace) {
    prevConcTrace = concTrace;
    concTrace = concTrace->next;
    free(prevConcTrace);
  }

#ifdef __TRACE_DEBUG__
  printf("Working on the stack now.\n");
#endif
  
  while(abStack) {
    marking = abPopMarking(&abStack,&firelist,&curZone,&abTrace,&Eset,
                           &concTrace);
    lpnDeleteMarking(marking);
    free(marking);
    free(firelist);
    lpnFreeZone(curZone);
    free(abTrace->array);
    free(abTrace);
    free(Eset->array);
    free(Eset);
    while(concTrace) {
      prevConcTrace = concTrace;
      concTrace = concTrace->next;
      free(prevConcTrace);
    }
  }
  
}

/*****************************************************************************
* Finds the possible false negatives from abstracted nets.
******************************************************************************/
traceListADT findTrace(char *filename,signalADT *signals,eventADT *events,
                       ruleADT **rules,stateADT *state_table,
                       markkeyADT *markkey,int nevents,int nplaces,
                       int nsignals,int ninpsig,int nrules,char * startstate,
                       bool nofail,bool verbose) 
{
  srlListADT abTrace; /* L in the description below */
  srlListADT newAbTrace;
  srlListADT abEvents; /* T in the description below */
  int i,j,cnt,found,fireCnt;
  int t; /* t in the description below */
  int finalEvent = 0;
  char *firelist;
  char *newFirelist;
  markingADT marking;
  markingADT newMarking;
  srlZoneADT curZone;
  srlZoneSet newZoneS;
  srlZoneADT newZone;
  eventTimePairADT *etVec;
  srlListADT Eset = NULL; /* E in the description below */
  srlListADT newEset = NULL;
  bool *TD;
  abStackADT abStack = NULL;
  traceListADT concTrace = NULL; /* the possible concrete error trace */
  traceListADT newConcTrace;

  
  /*  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".csg");
    printf("Finding compressed state graph and storing to:  %s\n",outname);
    fprintf(lg,"Finding compressed state graph and storing to:  %s\n",outname);
    fp=Fopen(outname,"w"); 
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,nsigs,ninpsig);
    fprintf(fp,"STATES:\n");
    } else */ {
    printf("Finding a concrete error trace ... ");
    fflush(stdout);
    fprintf(lg,"Finding a concrete error trace ... ");
    fflush(lg);
  }
#ifdef __TRACE_DEBUG__
  printf("findTrace():Start\n");
#endif

  abEvents = (srlListADT)GetBlock(sizeof(*abEvents));
  cnt = 0;
  for(i=1;i<nevents;i++) {
    if(!(events[i]->type & ABSTRAC)) {
#ifdef __TRACE_DEBUG__
      printf("Event %s is in the abstracted net.\n",events[i]->event);
#endif
      cnt++;
    }
  }

  abEvents->tot = cnt;
  abEvents->pos = 0;
  abEvents->array = (int *)GetBlock((sizeof cnt) * cnt);

  cnt = 0;
  for(i=1;i<nevents;i++) {
    if(!(events[i]->type & ABSTRAC)) {
      abEvents->array[cnt] = i;
      cnt++;
    }
  }
  
  abTrace = (srlListADT)GetBlock(sizeof(*abTrace));
  
  if(!parseFile(filename,events,nevents,abTrace,verbose)) {
    /* Note: error message is printed in parseFile */
    cleanupFindTrace(abTrace,abEvents,NULL,NULL,NULL,NULL,NULL,NULL);
    return NULL;
  }

#ifdef __TRACE_DEBUG__
  printf("Error trace...\n");
  for(i=0;i<abTrace->tot;i++) {
    printf("%d:%s:%d\n",i,events[abTrace->array[i]]->event,abTrace->array[i]);
  }
#endif
  
  /* clear the state table */
  initialize_state_table(LOADED,FALSE,state_table);
  
  marking = lpnFindInitialMarking(events,rules,markkey,nevents,nrules,
                                  ninpsig,nsignals,startstate,false);
  
  /* if no initial marking, there is problem, so bail out */
  if(marking == NULL) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: No initial marking was found.\n");
    fprintf(lg,"ERROR: No initial marking was found.\n");
    return NULL;
  }
  
  curZone = lpnFindInitialZone(rules,markkey,events,nevents,nrules,
                               nplaces,marking,false);

  /* see if t is enabled and firable -- we are going to cheat and
     just check firable b/c firable is a subset of enabled */
  firelist = lpnCanFireFirst(marking,curZone,nevents,rules,events);
  /* search for a valid error trace */
  while(1) {
    t = abTrace->array[abTrace->pos];
#ifdef __TRACE_DEBUG__
    if (t < nevents)
      printf("New t -- %s\n",events[t]->event);
#endif
    
    while(firelist[t] == 'F') {
      /* t isn't firable so figure out what needs to fire to make it
         firable */
#ifdef __TRACE_DEBUG__
      printf("%s isn't firable now...looking for other events.\n",events[t]->event);
#endif
      TD = newBoolVector(nevents);
      etVec = lpnTraceBack(TD,t,marking,events,rules,nevents,nplaces,true);
      free(TD);

      /* check to see if the event we are trying to fire is enabled,
         but not firable.  if this is the case then fire events from
         the firelist until that event becomes enabled and move on */
      if(etVec[t].event) {
        while(firelist[t] == 'F') {
          found = 0;
          for(j=1;j<nevents;j++) {
            if(firelist[j] == 'T') {

	      bool dontfire=false;
	      for(int k=0;k<abEvents->tot;k++) {
		if (abEvents->array[k]==j) {
		  dontfire=true;
		  break;
		}
	      }
	      if (dontfire) continue;

              found = 1;
              /* fire the event */
#ifdef __TRACE_DEBUG__
              printf("Firing(1): %s:%d\n",events[j]->event,j);
#endif
              newMarking = lpnFindNewMarking(signals,events,rules,markkey,
					     marking,j,
                                             nevents,nplaces,nrules,false,
                                             nsignals,ninpsig,false,false,
                                             false);
              if(newMarking) {
                newZoneS = lpnFindNewZone(rules,markkey,events,nevents,nrules,
                                          nplaces,newMarking,j,curZone,false,
                                          firelist,marking,nofail);
              }
              else {
		if (!verbose) {
		  printf("ERROR!\n");
		  fprintf(lg,"ERROR!\n");
		}
                printf("ERROR: No new marking was found.\n");
                fprintf(lg,"ERROR: No new marking was found.\n");
                cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                                 Eset,concTrace,abStack);
                return NULL;
              }

              /* using total order there will only ever be 1 zone */
              newZone = removeZoneS(&newZoneS);
              newFirelist = lpnCanFireFirst(newMarking,newZone,nevents,rules,
                                            events);
      
              /* add the fired event to the trace */
              abAddTraceList(j,&concTrace);

#ifdef __TRACE_DEBUG__
              abPrintTraceList(concTrace,events);
#endif
              lpnDeleteMarking(marking);
              free(marking);
              lpnFreeZone(curZone);
              free(firelist);
              
              firelist = newFirelist;
              marking = newMarking;
              curZone = newZone;
              /* if we fired one event break out of the for loop and
                 start the process over again */
              break;
            }
          }
          if(!found) {
	    if (!verbose) {
	      printf("ERROR!\n");
	      fprintf(lg,"ERROR!\n");
	    }
            printf("ERROR: Nothing is firable.\n");
            fprintf(lg,"ERROR: Nothing is firable.\n");
            cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                             Eset,concTrace,abStack);
            return NULL;
          }
        }
        /* we have now made t firable so break out of the while loop
           that is trying to make t firable */
        break;
      }
      
#ifdef __TRACE_DEBUG__
      printf("traceBack found: ");
      for(j=1;j<nevents;j++) {
        if(etVec[j].event) {
          printf("%s, ",events[j]->event);
        }
      }
      printf("\n");
#endif
      
      /* remove the events from etVec that are in T */
      for(j=0;j<abEvents->tot;j++) {
        etVec[abEvents->array[j]].event = false;
      }
      
      /* move etVec into a better data structure for this code... */
      cnt = 0;
      fireCnt = 0;
      for(j=1;j<nevents;j++) {
        if(etVec[j].event) {
          cnt++;
          /* keep a separate fire count b/c for efficiency we are
             going to look at firable events first */
          if(firelist[j] == 'T') {
            fireCnt++;
          }
        }
      }

      printf("cnt: %d\n",cnt);
      
      
      Eset = (srlListADT)GetBlock(sizeof(*Eset));
      Eset->pos = 0;
      Eset->tot = cnt;
      /* if knowing this array size ahead of time is painful then we
         can always create a static array that is nevents long and use -1
         or the size as an end of array marker. */
      Eset->array = (int*)GetBlock(sizeof(j) * cnt);

      for(j=1;j<nevents;j++) {
        /* only add the event if it is enabled and firable */
        if(etVec[j].event && (firelist[j] == 'T')) {
#ifdef __TRACE_DEBUG__
          printf("Adding %s to E [firable].\n",events[j]->event);
#endif
          Eset->array[Eset->pos] = j;
          Eset->pos++;
        }
        else if(etVec[j].event) {
#ifdef __TRACE_DEBUG__
          printf("Adding %s to E [NOT firable] in pos %d.\n",events[j]->event,
		 fireCnt);
#endif
          Eset->array[fireCnt] = j;
          fireCnt++;
        }
      }
      Eset->pos = 0;

      /* we are done w/ etVec for this loop iteration so free it */
      free(etVec);
      
      if(cnt == 0) {
        printf("WARNING: cnt is 0!\n");
        /* stack is empty */
        if(!abStack) {
          cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                           Eset,concTrace,abStack);
	  if (!verbose) {
	    printf("ERROR!\n");
	    fprintf(lg,"ERROR!\n");
	  }
          printf("FALSE NEGATIVE!\n");
          fprintf(lg,"FALSE NEGATIVE!\n");
          return NULL;
        }
        else {
          /* free before overwriting */
          cleanupFindTrace(abTrace,NULL,marking,curZone,firelist,Eset,
                           concTrace,NULL);
          
          marking = abPopMarking(&abStack,&firelist,&curZone,&abTrace,&Eset,
                                 &concTrace);
          /* check to see if there is anything left to explore.  if
             not then keep popping until something is found or if we
             are done then return */
          while(Eset->pos == Eset->tot) {
            if(abStack) {
              /* free before overwriting */
              cleanupFindTrace(abTrace,NULL,marking,curZone,firelist,Eset,
                               concTrace,NULL);
              
              marking = abPopMarking(&abStack,&firelist,&curZone,&abTrace,
                                     &Eset,&concTrace);
            }
            else {
	      if (!verbose) {
		printf("ERROR!\n");
		fprintf(lg,"ERROR!\n");
	      }
              printf("FALSE NEGATIVE!\n");
              fprintf(lg,"FALSE NEGATIVE!\n");
              cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                               Eset,concTrace,abStack);
              return NULL;
            }
          }
        }
      }

      /* fire an event that contributes to the firing of t */
      /* check if the event is firable and progress acccordingly */
      if(firelist[Eset->array[Eset->pos]] == 'F') {
        while(firelist[Eset->array[Eset->pos]] == 'F') {
          found = 0;
          for(j=1;j<nevents;j++) {
            if(firelist[j] == 'T') {
              found = 1;
              /* fire the event */
#ifdef __TRACE_DEBUG__
              printf("Firing(3): %s:%d\n",events[j]->event,j);
#endif
              newMarking = lpnFindNewMarking(signals,
					     events,rules,markkey,marking,j,
                                             nevents,nplaces,nrules,false,
                                             nsignals,ninpsig,false,false,
                                             false);
              if(newMarking) {
                newZoneS = lpnFindNewZone(rules,markkey,events,nevents,nrules,
                                          nplaces,newMarking,j,curZone,false,
                                          firelist,marking,nofail);
              }
              else {
		if (!verbose) {
		  printf("ERROR!\n");
		  fprintf(lg,"ERROR!\n");
		}
                printf("ERROR: No new marking was found.\n");
                fprintf(lg,"ERROR: No new marking was found.\n");
                cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                                 Eset,concTrace,abStack);
                return NULL;
              }

              /* using total order there will only ever be 1 zone */
              newZone = removeZoneS(&newZoneS);
              newFirelist = lpnCanFireFirst(newMarking,newZone,nevents,rules,
                                            events);
      
              /* add the fired event to the trace */
              abAddTraceList(j,&concTrace);

#ifdef __TRACE_DEBUG__
              abPrintTraceList(concTrace,events);
#endif

              lpnDeleteMarking(marking);
              free(marking);
              lpnFreeZone(curZone);
              free(firelist);
              
              firelist = newFirelist;
              marking = newMarking;
              curZone = newZone;
              /* if we fired one event break out of the for loop and
                 start the process over again */
              break;
            }
          }
          if(!found) {
            printf("Error: Nothing is firable.\n");
            cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                             Eset,concTrace,abStack);
            return NULL;
          }
        }
      }
#ifdef __TRACE_DEBUG__
      printf("Firing(2): %s:%d\n",events[Eset->array[Eset->pos]]->event,
             Eset->array[Eset->pos]);
#endif
      newMarking = lpnFindNewMarking(signals,events,rules,markkey,marking,
                                     Eset->array[Eset->pos],nevents,nplaces,
                                     nrules,false,nsignals,ninpsig,
                                     false,false,false);
      if(newMarking) {
        newZoneS = lpnFindNewZone(rules,markkey,events,nevents,nrules,nplaces,
                                  newMarking,Eset->array[Eset->pos],curZone,
                                  false,firelist,marking,nofail);
      }
      else {
	if (!verbose) {
	  printf("ERROR!\n");
	  fprintf(lg,"ERROR!\n");
	}
        printf("ERROR: No new marking was found.\n");
        fprintf(lg,"ERROR: No new marking was found.\n");
        cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                         Eset,concTrace,abStack);
        return NULL;
      }

      /* using total order there will only ever be 1 zone */
      newZone = removeZoneS(&newZoneS);
      newFirelist = lpnCanFireFirst(newMarking,newZone,nevents,rules,
                                    events);
      
      /* add the fired event to the trace */
      abAddTraceList(Eset->array[Eset->pos],&concTrace);

#ifdef __TRACE_DEBUG__
      abPrintTraceList(concTrace,events);
#endif
      
      Eset->pos++;

      /* we need to copy abTrace, Eset, and concTrace before pushing
         them on the stack */
      newAbTrace = abCopyListADT(abTrace);
      newEset = abCopyListADT(Eset);
      newConcTrace = abCopyTraceList(concTrace);
    
      abPushMarking(&abStack,firelist,curZone,marking,newAbTrace,newEset,
                    concTrace);
    
      firelist = newFirelist;
      marking = newMarking;
      curZone = newZone;
    }
    /* t is firable so fire it and get the new state */
#ifdef __TRACE_DEBUG__
    if (t < nevents)
      printf("t[%s] is firable!\n",events[t]->event);
#endif
    
    newMarking = lpnFindNewMarking(signals,events,rules,markkey,marking,
                                   t,nevents,nplaces,nrules,false,nsignals,
                                   ninpsig,false,false,false);

    if(finalEvent && (newMarking == NULL)) {
      abAddTraceList(t,&concTrace);
      printf("done.\n");
      fprintf(lg,"done.\n");
      abPrintTraceList(concTrace,events);
#ifdef __TRACE_DEBUG__
      printf("Done!  A concrete error trace was found.  Returning.\n");
#endif
      /* actually before we return we need to pop everything off the
         stack and clean it all up */
      cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                       Eset,NULL,abStack);
      return concTrace;
    }
    
    if(newMarking) {
      newZoneS = lpnFindNewZone(rules,markkey,events,nevents,nrules,nplaces,
                                newMarking,t,curZone,false,firelist,marking,
                                nofail);
      if (newZoneS == NULL) {
	abAddTraceList(t,&concTrace);
	printf("done.\n");
	fprintf(lg,"done.\n");
	abPrintTraceList(concTrace,events);
#ifdef __TRACE_DEBUG__
	printf("Done!  A concrete error trace was found.  Returning.\n");
#endif
	/* actually before we return we need to pop everything off the
	   stack and clean it all up */
	cleanupFindTrace(abTrace,abEvents,marking,NULL,firelist,
			 Eset,NULL,abStack);
	return concTrace;
      }
    }
    else {
      if (!verbose) {
	printf("ERROR!\n");
	fprintf(lg,"ERROR!\n");
      }
      printf("ERROR: No new marking was found.\n");
      fprintf(lg,"ERROR: No new marking was found.\n");
      cleanupFindTrace(abTrace,abEvents,marking,curZone,firelist,
                       Eset,concTrace,abStack);
      return NULL;
    }

    /* using total order there will only ever be 1 zone */
    newZone = removeZoneS(&newZoneS);
    newFirelist = lpnCanFireFirst(newMarking,newZone,nevents,rules,
                                  events);

    /* add the fired event to the trace */
    abAddTraceList(t,&concTrace);
    abTrace->pos++;
    if(abTrace->pos == abTrace->tot-1) {
      finalEvent = 1;
    }

#ifdef __TRACE_DEBUG__
    abPrintTraceList(concTrace,events);
#endif
    
    lpnDeleteMarking(marking);
    free(marking);
    lpnFreeZone(curZone);
    free(firelist);
      
    firelist = newFirelist;
    marking = newMarking;
    curZone = newZone;    
  }
}

/*
  Assume that I have marked in the events structure the set of transitions 
  in the abstracted net, T.  Assume also that I have given you a linked list 
  of event indices representing the abstracted trace, L.  This list is to be 
  used to direct a guided simulation (state space exploration) on the concrete 
  net.  Start with the initial state, s, and first transition, t in L:

  1) If in s, t is enabled and firable, then fire it and go to step 5.

  2) If t is not enabled or firable, use the necessary set algorithm to find 
  a set E of abstracted transitions (i.e., ones not in T) which you can 
  fire that will contribute towards the enabling of the first transition in 
  the trace.  

  3) If E is not empty, fire one of the transitions that will contribute 
  towards the enabling and set s to this new state, push the current state 
  and the remaining transitions on the stack, and go to step 1. 

  4) If E is empty and the stack is not empty, backtrack by popping an entry 
  off the stack and go to step 3, else report trace as false negative and 
  STOP.

  5) Set s to the new state after firing t.

  6) If the list is not empty, set t to the next transition in L and go to 
  step 1, else report the concrete trace and STOP.
*/

/*
  For the work that needs to be done see comments preceded by CONSTRAINTS.
  
  2) Constraints

  I don't believe that timing on constraints is currently being checked.  
  Try changing the timing on the attached example using:

  atacs -oqtsva srl_srdandNEW.g
  atacs -oqtSllva srl_srdandNEW.g

  To check constraints, you must keep past variables for transitions in the 
  preset of constraint places until the transitions in their postset fire.  
  For example:

  a+ b+ #@ {C} [3,6]
  x+ b+ #@ [1,2]

  After a+ fires and until b+ fires, you must keep a+ around as a past 
  variable.  When you fire b+, you must check the separation between a+ and 
  b+ and if it does not exceed the 3 then you report a lower bound 
  constraint failure.  Before firing any transition, you must check that 
  there exists at least one enabled output transition in the zone that has a 
  maximum separation with a+ that does not exceed 6.  These implies that 
  that transition (which may not be b+) must fire before time can advance 6 
  time units after a+.  If there does not exist such a transition, then time 
  can advance beyond 6 time units and there is an upper bound constraint 
  failure. 
*/
