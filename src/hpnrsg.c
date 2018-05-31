/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2003 by, Scott R. Little                                  */
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

/* When enabled no pruning of firing actions is done. */
/* #define __NO_PRUNE__ */

/* Debug defines */
/* #define __HPN_BASIC__ */

/* #define __HPN_TRANSFIRE__ */
/* #define __HPN_VAR_ASG__ */
/* #define __HPN_SHOW_FIRE__ */
/* #define __HPN_SHOW_POSS_ACTION__ */
/* #define __HPN_SHOW_WORKING_ACTION__ */

/* #define __HPN_ADVTIME__ */
/* #define __HPN_MAXCONTVAR__ */

/* #define __HPN_INVARIANT_DEBUG__ */
// #define __HPN_INEQ_DEBUG__ 
/* #define __HPN_ENABLINGS_DEBUG__ */
/* #define __HPN_PRED_DEBUG__ */

/* #define __HPN_ADD_ACTION__ */
/* #define __HPN_ADD_STATE__ */
/* #define __HPN_ADD_STATE2__ */
/* #define __HPN_ADD_ZONE__ */

/* #define __HPN_DELCLOCK__ */
/* #define __HPN_DISALLOW__ */
/* #define __HPN_SIMUL_ACTION__ */
/* #define __HPN_INTROCLOCK__ */
/* #define __HPN_FIRECLOCK__ */
/* #define __HPN_INTROVECTOR__ */

/* #define __HPN_STACK_DEBUG__ */
/* #define __HPN_NEW_STATE__ */
/* #define __HPN_STATE_INFO__ */

/* #define __HPN_IND_ACTION__ */
/* #define __HPN_CHRIS_SIM__ */
/* #define __HPN_DEAD_STATE__ */
/* #define __HPN_NO_ADD_STATE__ */
/* #define __HPN_NO_ADD_STATE2__ */
/* #define __UNIT_TEST__ */
/* #define __HPN_DEBUG_WARP__ */
/* #define __HPN_DEBUG__ */
/* #define __HPN_TRACE__ */
/* #define __HPN_VELOCITY__ */
/* #define __MEMSTATS__ */
/* #define __HPN_TEST__ */
/* #define __HPN_VALGRIND__ */

/* #define __HPN_INFORM__ */

#ifdef __HPN_BASIC__
#define __HPN_STACK_DEBUG__
#define __HPN_NEW_STATE__
#define __HPN_SHOW_FIRE__
#define __HPN_ADD_STATE__
#define __HPN_ADD_ZONE__
#define __HPN_SHOW_POSS_ACTION__
#define __HPN_SHOW_WORKING_ACTION__
#endif

#define __HPN_WARN__

#include <sys/times.h>
#include <unistd.h>
#include <limits.h>
#include "common_timing.h"
#include "findreg.h"
#include "findrsg.h"
#include "hpnrsg.h"
#include "color.h"
#include "srllpnrsg.h"
#include "displayADT.h"
#ifdef __UNIT_TEST__
#include "unitTest.h"
#endif
#include <sstream>
#include <set>

/*****************************************************************************
 * Perform the infinity optimization for [0,inf] clocks.
 *****************************************************************************/
bool infopt(cStateADT cState,ruleADT **rules,int i,int j,int nevents,
            int nplaces)
{
  if ((rules[i][j]->lower==0)&&(rules[i][j]->upper==INFIN)) {
    for (int k=nevents;k<nevents+nplaces;k++)
      if ((rules[k][j]->ruletype > 0) && (rules[k][j]->lower==0) &&
          (rules[k][j]->upper==INFIN) && (i!=k) &&
          (cState->firedClocks[rules[k][j]->marking] != 'T'))
        return true;
  }
  return false;
}

/*****************************************************************************
 * Output initial messages to the screen and open the output file if
 * verbose is turned on.
 *****************************************************************************/
FILE *hpnInitOutput(char *filename, signalADT *signals, int nsignals,
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
 * Delete the marking.
 *****************************************************************************/
void hpnDeleteMarking(dMarkingADT marking)
{
  if(marking == NULL) {
    cSetFg(RED);
    printf("WARNING: ");
    fprintf(lg,"WARNING: ");
    cSetAttr(NONE);
    printf("attempt to free NULL marking.\n");
    fprintf(lg,"attempt to free NULL marking.\n");
    return;
  }

  if(marking->marking) {
    free(marking->marking);
  }
  if(marking->enablings) {
    free(marking->enablings);
  }
  
  if(marking->state) {
    /* SRL: causing problems w/ new glibc... */
    free(marking->state);
  }
  
  free(marking);
}

/*****************************************************************************
 * Copy the marking.
 *****************************************************************************/
dMarkingADT hpnCopyMarking(dMarkingADT marking)
{
  dMarkingADT copyMarking = (dMarkingADT)GetBlock(sizeof(*copyMarking));
  copyMarking->marking = CopyString(marking->marking);
  copyMarking->enablings = CopyString(marking->enablings);
  copyMarking->state = CopyString(marking->state);
  return copyMarking;
}

/*****************************************************************************
 * Copy the startstate into a marking and error if start state is missing.
 *****************************************************************************/
bool hpnCopyStartState(dMarkingADT marking,char *startState,bool verbose)
{
  if(startState == NULL) {
    if(!verbose) {
      printf("Error: No initial state information.\n");
      fprintf(lg,"Error: No initial state information.\n");
    }
    printf("Error:  Need initial state information.\n");
    fprintf(lg,"Error:  Need initial state information.\n");
    hpnDeleteMarking(marking);
    return false;
  }
  else {
    strcpy(marking->state,startState);
    return true;
  }
}

/*****************************************************************************
 * Return true for a valid rule.
 *****************************************************************************/
bool hpnValidRule(int ruletype)
{
  return ((ruletype > NORULE) && (ruletype < REDUNDANT));
}

/*****************************************************************************
 * Return true for a rule with a discrete preset place.
 *****************************************************************************/
bool hpnDRule(char* arcType,int rule)
{
  if(arcType[rule] == 'D' || arcType[rule] == 'H') {
    return true;
  }
  return false;
}

/*****************************************************************************
 * Check if state is consistent w/ an enabled event.
 *****************************************************************************/
bool hpnCheckConsistency(char *event,char *state,int signal,int ninpsig,
                         bool verbose)
{
  if(1/*signal >= ninpsig*/) {
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
 * Mark all discretely enabled events.
 *****************************************************************************/
void hpnEventsDiscreteEn(eventADT *events,ruleADT **rules,markkeyADT *markkey,
                         dMarkingADT marking,char* arcType,int nrules,
                         int nevents)
{
#ifdef __HPN_TRACE__
  printf("hpnEventsDiscreteEn():Start\n");
#endif

  /* simple first pass */
  for(int i=1;i<nevents;i++) {
    if(events[i]->dropped) {
      marking->enablings[i]='F';
    }
    else {
      marking->enablings[i]='T';
    }
  }

#ifdef __HPN_DEBUG__
  int size = 0;
  printf("Initially enabled events: ");
  for(int i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      size++;
      if(size > 1) {
        printf(", %s [%d]",events[i]->event,i);
      }
      else {
        printf(" %s [%d]",events[i]->event,i);
      }
    }
  }
  printf("\n");
#endif

  for(int i=0;i<nrules;i++) {
    if(arcType[i] == 'D' || arcType[i] == 'H') {
      /* member of F_d or F_h */
      if(markkey[i]->enabled < nevents &&
         marking->enablings[markkey[i]->enabled] == 'T' &&
         check_rule(rules,markkey[i]->enabling,markkey[i]->enabled,
                    false,false)) {
        if(marking->marking[i] == 'F') {
          marking->enablings[markkey[i]->enabled] = 'F';
        }
      }
    }
  }

#ifdef __HPN_DEBUG__
  size = 0;
  printf("Enabled events: ");
  for(int i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      size++;
      if(size > 1) {
        printf(", %s [%d]",events[i]->event,i);
      }
      else {
        printf(" %s [%d]",events[i]->event,i);
      }
    }
  }
  printf("\n");
#endif
  
#ifdef __HPN_TRACE__
  printf("hpnEventsDiscreteEn():End\n");
#endif 
}

/*****************************************************************************
 * Initialize the arcType array - marks each rule with a type based on
 * the the type of place and transition
 * C=c->c,D=d->d,P=c->d,H=d->c
 * Note: This code assumes no implicit places are present.
 *****************************************************************************/
void hpnInitArcType(char** arcType,markkeyADT *markkey,eventADT *events,
                    int nrules)
{
  *arcType = (char *)GetBlock((nrules)*sizeof(char));

  (*arcType)[0] = 'D'; /* initialize reset rule */
  for(int i=0;i<nrules;i++) {
    if(events[markkey[i]->enabling]->type & CONT) {
      if(events[markkey[i]->enabled]->type & CONT) {
        (*arcType)[i] = 'C';
      }
      else {
        (*arcType)[i] = 'P';
      }
    }
    else {
      if(events[markkey[i]->enabled]->type & CONT) {
        (*arcType)[i] = 'H';
      }
      else {
        (*arcType)[i] = 'D';
      }
    }
  }

#ifdef __HPN_DEBUG__
  /* print out the results of hpnInitArcType */
  printf("nrules: %d\n",nrules);
  for(int i=0;i<nrules;i++) {
    printf("%d:Rule %s->%s is of type: %c\n",i,
           events[markkey[i]->enabling]->event,
           events[markkey[i]->enabled]->event,(*arcType)[i]);
  }
#endif
  
}

/*****************************************************************************
 * Prints out the fired clocks.
 *****************************************************************************/
void printFiredClocks(cStateADT cState,markkeyADT *markkey,eventADT *events,
                      int nrules)
{
  cSetFg(CYAN);
  printf("Fired clocks: ");
  cSetAttr(NONE);
  for(int i=0;i<nrules;i++) {
    if(cState->firedClocks[i] == 'T') {
      printf("%s->%s,",events[markkey[i]->enabling]->event,
             events[markkey[i]->enabled]->event);
    }
  }
  printf("\n");

  cSetFg(CYAN);
  printf("IntroClocks: ");
  cSetAttr(NONE);
  for(int i=0;i<nrules;i++) {
    if(cState->introClocks[i] == 'T') {
      printf("%s->%s,",events[markkey[i]->enabling]->event,
             events[markkey[i]->enabled]->event);
    }
  }
  printf("\n");
}

/*****************************************************************************
 * Prints the values of the continuous variables.
 *****************************************************************************/
void printContVars(hpnZoneADT zone,eventADT *events,cStateADT cState,
                   int nevents)
{
  cSetFg(GREEN);
  printf("Cont vars:");
  cSetAttr(NONE);
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling == -1) {
      printf("%s:%d-%d[%d] ",events[zone->curClocks[i].enabled]->event,
             zone->matrix[0][i]*-1,zone->matrix[i][0],
             cState->warp[zone->curClocks[i].enabled-nevents]);
    }
  }
  printf("\n");
}

/*****************************************************************************
 * Prints out a given zone.
 *****************************************************************************/
void printZone(hpnZoneADT zone,eventADT *events)
{
  cSetFg(GREEN);
  printf("Zone: ");
  cSetAttr(NONE);
  printf("dbmEnd:%d -- numClocks:%d\n",zone->dbmEnd,zone->numClocks);
  printf("Event ordering is: ");
  /* print out the transitions in the zone separated by tabs */
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling != -1) {
      printf("%s->%s,",events[zone->curClocks[i].enabling]->event,
             events[zone->curClocks[i].enabled]->event);
    }
    else {
      printf("%s,",events[zone->curClocks[i].enabled]->event);
    }
  }
  printf("\n");

  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling != -1) {
      printf("\t%d->%d",zone->curClocks[i].enabling,
             zone->curClocks[i].enabled);
    }
    else {
      printf("\t%d",zone->curClocks[i].enabled);
    }
  }
  printf("\n");

  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling != -1) {
      printf("%d->%d",zone->curClocks[i].enabling,zone->curClocks[i].enabled);
    }
    else {
      printf("%d",zone->curClocks[i].enabled);
    }
    for(int j=0;j<zone->numClocks;j++) {
      if(zone->matrix[i][j] == INFIN) {
        printf("\tU");
      }
      else if(zone->matrix[i][j] == INFIN * -1) {
        printf("\t-U");
      }
      else {
        printf("\t%d",zone->matrix[i][j]);
      }
    }
    printf("\n");
  }
}

/*****************************************************************************
 * Prints out a given zone with warp information
 *****************************************************************************/
void printZone(hpnZoneADT zone,eventADT *events,cStateADT cState,
               int nevents)
{
  printf("Warp: ");
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling == -1) {
      printf("%s[%d], ",events[zone->curClocks[i].enabled]->event,
             cState->warp[zone->curClocks[i].enabled-nevents]);
    }
  }
  printf("\n");
  printZone(zone,events);
}

/*****************************************************************************
 * Prints out a plain zone w/out strings, etc.
 *****************************************************************************/
void printPlainZone(hpnZoneADT zone)
{
  cSetFg(GREEN);
  printf("Zone: ");
  cSetAttr(NONE);
  printf("dbmEnd:%d -- numClocks:%d\n",zone->dbmEnd,zone->numClocks);
  
  /* print out the transitions in the zone separated by tabs */
  for(int i=0;i<zone->numClocks;i++) {
    printf("\t%d->%d",zone->curClocks[i].enabling,
           zone->curClocks[i].enabled);
  }
  printf("\n");

  for(int i=0;i<zone->numClocks;i++) {
    printf("%d->%d",zone->curClocks[i].enabling,
           zone->curClocks[i].enabled);
    for(int j=0;j<zone->numClocks;j++) {
      if(zone->matrix[i][j] == INFIN) {
        printf("\tU");
      }
      else {
        printf("\t%d",zone->matrix[i][j]);
      }
    }
    printf("\n");
  }
}

/*****************************************************************************
 * Verifies that the zone is consistent and valid.
 *****************************************************************************/
bool checkConsistentZone(hpnZoneADT zone,eventADT *events)
{
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->matrix[i][i] != 0) {
      cSetFg(RED);
      printf("Error: Zone is not consistent.\n");
      fprintf(lg,"Error: Zone is not consistent.\n");
      cSetAttr(NONE);
#ifdef __HPN_DEBUG__OFF
      printZone(zone,events);
#endif
      return false;
    }
  }
  return true;
}

/*****************************************************************************
 * Creates a new zone data structure.  numClocks is initialized to the
 * size of the zone and the matrix values are initialized to INFIN.
 *****************************************************************************/
hpnZoneADT createZone(int size)
{
#ifdef __HPN_TRACE__
  printf("createZone(%d):Start\n",size);
#endif
  
  hpnZoneADT zone = (hpnZoneADT)GetBlock(sizeof(*zone));
  
  zone->numClocks = size;
  zone->dbmEnd = size;
  if(size == 0) {
    cSetFg(RED);
    printf("WARNING: ");
    fprintf(lg,"WARNING: ");
    cSetAttr(NONE);
    printf("creating a zero size zone.\n");
    fprintf(lg,"creating a zero size zone.\n");
    zone->curClocks = NULL;
    zone->matrix = NULL;
    return zone;
  }

  /* allocate and initialize the information key for the current
     clocks */
  zone->curClocks = (clockkeyADT)GetBlock(size * sizeof(struct clockkey));
  for(int i=0;i<size;i++) {
    zone->curClocks[i].enabled = -1;
    zone->curClocks[i].enabling = -1;
    zone->curClocks[i].eventk_num = -1;
    zone->curClocks[i].causal = -1;
    zone->curClocks[i].anti = NULL;
  }
  
  /* allocate the 2D zone matrix */
  zone->matrix = (clocksADT)GetBlock(size * sizeof(int*));
  for(int i=0;i<size;i++) {
    zone->matrix[i] = (int*)GetBlock(size * sizeof(int*));
  }

  /* initialize the matrix to have all infinite bounds that can
     be tightened down later as appropriate */
  for(int i=0;i<size;i++) {
    for(int j=0;j<size;j++) {
      if(i == j) {
        zone->matrix[i][i] = 0;
      }
      else {
        zone->matrix[i][j] = INFIN;
      }
    }
  }
  
#ifdef __HPN_TRACE__
  printf("createZone(%d):End\n",size);
#endif
  return zone;
}

/*****************************************************************************
 * Checks for an all zero diagonal.
 *****************************************************************************/
bool validZone(hpnZoneADT zone)
{
  for(int i=0;i<zone->dbmEnd;i++) {
    for(int j=0;j<zone->dbmEnd;j++) {
      if(i == j && zone->matrix[i][j] != 0) {
        cSetFg(RED);
        printf("WARNING: ");
        fprintf(lg,"WARNING: ");
        cSetAttr(NONE);
        printf("invalid zone.\n");
        fprintf(lg,"invalid zone.\n");
        return false;
      }
    }
  }
  return true;
}

/*****************************************************************************
 * Uses Floyd's algorithm (naive version which in O(n^3)) to recanonicalize
 * the zone.
 *****************************************************************************/
void recanonZone(hpnZoneADT zone)
{
#ifdef __HPN_TRACE__
  printf("recanonZone:Start()\n");
#endif

  for(int i=0;i<zone->dbmEnd;i++) {
    for(int j=0;j<zone->dbmEnd;j++) {
      for(int k=0;k<zone->dbmEnd;k++) {
	
        if ((zone->matrix[j][i]!=INFIN)&&(zone->matrix[i][k]!=INFIN)&&
            (zone->matrix[j][i] > 0)&&(zone->matrix[i][k] > 0)&&
            (zone->matrix[j][i] > (INFIN - zone->matrix[i][k]))) {
          cSetFg(RED);
          printf("WARNING: ");
          fprintf(lg,"WARNING: ");
          cSetAttr(NONE);
          printf("Entry greater than infinity.\n");
          fprintf(lg,"Entry greater than infinity.\n");
        }
        if(zone->matrix[j][i] != INFIN && zone->matrix[i][k] != INFIN &&
           zone->matrix[j][k] > zone->matrix[j][i] + zone->matrix[i][k]) {
          zone->matrix[j][k] = zone->matrix[j][i] + zone->matrix[i][k];
        }
      }
    }
  }
  validZone(zone);
  
#ifdef __HPN_TRACE__
  printf("recanonZone:End()\n");
#endif
}

/*****************************************************************************
 * Frees the zone data structure.
 *****************************************************************************/
void freeZone(hpnZoneADT zone)
{
  if(zone == NULL) {
    cSetFg(RED);
    printf("WARNING: ");
    cSetAttr(NONE);
    printf("attempt to free a NULL zone.\n");
    return;
  }
  
  free(zone->curClocks);
  for(int i=0;i<zone->numClocks;i++) {
    free(zone->matrix[i]);
  }
  free(zone->matrix);
  free(zone);
}

/*****************************************************************************
 * Copies a zone and returns it.
 *****************************************************************************/
hpnZoneADT copyZone(hpnZoneADT zone)
{
  hpnZoneADT newZone = createZone(zone->numClocks);

  newZone->dbmEnd = zone->dbmEnd;
  for(int i=0;i<zone->numClocks;i++) {
    newZone->curClocks[i] = zone->curClocks[i];
    for(int j=0;j<zone->numClocks;j++) {
      newZone->matrix[i][j] = zone->matrix[i][j];
    }
  }
  return newZone;
}

/*****************************************************************************
 * lhs = rhs - this function does not copy numClocks - the idea is to
 * use the function to copy a smaller rhs into a larger lhs.
 *****************************************************************************/
void copyZone(hpnZoneADT lhs, hpnZoneADT rhs)
{
  lhs->dbmEnd = rhs->dbmEnd;
  for(int i=0;i<rhs->numClocks;i++) {
    lhs->curClocks[i] = rhs->curClocks[i];
    for(int j=0;j<rhs->numClocks;j++) {
      lhs->matrix[i][j] = rhs->matrix[i][j];
    }
  }
}

/*****************************************************************************
 * Swap the two items in the zone. t1 and t2 are zone based indices.
 *****************************************************************************/
void swapZone(hpnZoneADT *z,int t1,int t2)
{
#ifdef __HPN_TRACE__
  printf("hpnSwapZone(%d,%d):Start\n",t1,t2);
#endif
  hpnZoneADT zone = *z;
  hpnZoneADT oldZone = copyZone(zone);

  /* swap the clocks in the clock array */
  zone->curClocks[t1] = oldZone->curClocks[t2];
  zone->curClocks[t2] = oldZone->curClocks[t1];

  
  for(int i=0;i<zone->numClocks;i++) {
    if(t1 == i) {
      zone->matrix[t1][t1] = oldZone->matrix[t2][t2];
      zone->matrix[t1][t1] = oldZone->matrix[t2][t2];
    }
    else if(t2 == i) {
      zone->matrix[t1][t2] = oldZone->matrix[t2][t1];
      zone->matrix[t2][t1] = oldZone->matrix[t1][t2];
    }
    else {
      zone->matrix[t1][i] = oldZone->matrix[t2][i];
      zone->matrix[i][t1] = oldZone->matrix[i][t2];
      zone->matrix[t2][i] = oldZone->matrix[t1][i];
      zone->matrix[i][t2] = oldZone->matrix[i][t1];
    }
  }
  
  freeZone(oldZone);
#ifdef __HPN_TRACE__
  printf("hpnSwapZone(%d,%d):Start\n",t1,t2);
#endif
}

/*****************************************************************************
 * Returns a zone based index for the event based clock or continuous
 * place given.  -1 is returned and warning is printed if the clock or
 * continuous place is not found.
 *****************************************************************************/
int getZoneIndex(hpnZoneADT zone,int enabling,int enabled)
{
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling == enabling &&
       zone->curClocks[i].enabled == enabled) {
      return i;
    }
  }
  cSetFg(RED);
  printf("WARNING: ");
  fprintf(lg,"WARNING: ");
  cSetAttr(NONE);
  printf("the given clock or continuous place (%d,%d) was not found in the zone.\n",enabling,enabled);
  fprintf(lg,"the given clock or continuous place(%d,%d) was not found in the zone.\n",enabling,enabled);
  return -1;
}

/*****************************************************************************
 * Prints out a given set of zones.
 *****************************************************************************/
void printZoneS(hpnZoneSetADT zoneS,eventADT *events)
{
  hpnZoneSetADT pZoneS = zoneS;

  printf("Printing a set of zones.\n");
  while(pZoneS != NULL) {
    printZone(pZoneS->zone,events);
    pZoneS = pZoneS->next;
  }
  printf("Set is done printing.\n");
}

/*****************************************************************************
 * Adds a zone to a given set of zones.
 *****************************************************************************/
void addZoneS(hpnZoneADT zone,hpnZoneSetADT *zoneS)
{
#ifdef __HPN_TRACE__
  printf("addZoneS():Start\n");
#endif

  hpnZoneSetADT newZoneS = (hpnZoneSetADT)GetBlock(sizeof *newZoneS);
  newZoneS->zone = zone;
  newZoneS->next = *zoneS;
  *zoneS = newZoneS;

#ifdef __HPN_TRACE__
  printf("addZoneS():End\n");
#endif
}

/*****************************************************************************
 * Removes a zone from the given set of zones.
 *****************************************************************************/
hpnZoneADT removeZoneS(hpnZoneSetADT *zoneS)
{
#ifdef __SRL_TRACE__
  printf("removeZoneS():Start\n");
#endif
  
  if(*zoneS == NULL) {
    cSetFg(RED);
    printf("WARNING: ");
    cSetAttr(NONE);
    printf("attempt to remove a zone from a NULL set.\n");
    return NULL;
  }

  hpnZoneADT zone = (*zoneS)->zone;
  hpnZoneSetADT oldZoneS = *zoneS;
  *zoneS = (*zoneS)->next;
  free(oldZoneS);

#ifdef __SRL_TRACE__
  printf("removeZoneS():End\n");
#endif
  return zone;
}

/*****************************************************************************
 * Push an entry onto the state space stack.
 *****************************************************************************/
void hpnPushState(hpnStackADT *stateSpaceStack,dMarkingADT marking,
                  hpnZoneADT zone,cStateADT cState,hpnActionSetADT A)
{
#ifdef __HPN_TRACE__
  printf("hpnPushState():Start\n");
#endif
  
  hpnStackADT newStateSpaceStack = NULL;

  newStateSpaceStack = (hpnStackADT)GetBlock(sizeof(*newStateSpaceStack));
  newStateSpaceStack->marking = marking;
  newStateSpaceStack->zone = zone;
  newStateSpaceStack->cState = cState;
  newStateSpaceStack->A = A;
  newStateSpaceStack->next = *stateSpaceStack;
  *stateSpaceStack = newStateSpaceStack;
  
#ifdef __HPN_TRACE__
  printf("hpnPushState():End\n");
#endif
}

/*****************************************************************************
 * Pop an entry from the state space stack.
 *****************************************************************************/
void hpnPopState(hpnStackADT *stateSpaceStack,dMarkingADT *marking,
                 hpnZoneADT *zone,cStateADT *cState,hpnActionSetADT *A)
{
#ifdef __HPN_TRACE__
  printf("hpnPopState():Start\n");
#endif
  
  hpnStackADT oldStateSpaceStack;

  if(*stateSpaceStack != NULL) {
    *marking = (*stateSpaceStack)->marking;
    *zone = (*stateSpaceStack)->zone;
    *cState = (*stateSpaceStack)->cState;
    *A = (*stateSpaceStack)->A;
    oldStateSpaceStack = *stateSpaceStack;
    *stateSpaceStack = (*stateSpaceStack)->next;
    free(oldStateSpaceStack);
  }
  else {
    cSetFg(RED);
    printf("WARNING: ");
    fprintf(lg,"WARNING: ");
    cSetAttr(NONE);
    printf("attempt to pop an empty stack.\n");
    fprintf(lg,"attempt to pop an empty stack.\n");
    return;
  }
  
#ifdef __HPN_TRACE__
  printf("hpnPopState():End\n");
#endif
}

/*****************************************************************************
 * Create a new hpnActionSetADT.
 *****************************************************************************/
hpnActionSetADT createHpnActionSet()
{
  hpnActionSetADT a = (hpnActionSetADT)GetBlock(sizeof(*a));
  a->l = new list<hpnActionADT>();
  a->setCnt = 2;
  return a;
}

/*****************************************************************************
 * Add an action to an action set.
 *****************************************************************************/
void uncondAddActionSetItem(hpnActionADT a,hpnActionSetADT &aSet)
{
  aSet->l->push_front(a);
}

/*****************************************************************************
 * Take a possible action and decide whether to actually add it to the
 * action set or not.  Only add actions they are theorically possible
 * from the current state.
 *****************************************************************************/
void addActionSetItem(hpnActionSetADT &aSet,hpnActionADT A,ruleADT **rules,
                      cStateADT cState,int nevents,hpnZoneADT z,
                      eventADT *events,int nplaces,char* arcType)
{
#ifdef __HPN_TRACE__
  printf("addActionSetItem():Start\n");
#endif

#ifdef __HPN_ADD_ACTION__
  printf("Attempting to add: ");
  printAction(A,events);
  printZone(z,events,cState,nevents);
#endif
  
  hpnActionSetADT e = createHpnActionSet(); 
  bool firable = true;
  if(A->type == 'F') {  
    for(int k=nevents;k<nevents+nplaces;k++) {
#ifdef __HPN_ADD_ACTION__
      printf("k: %d - A->enabling: %d\n",k,A->enabling);
#endif
      if(rules[k][A->enabled]->ruletype > 0) {
        if(isMemberCf(k,A->enabled,cState,rules) ||
           checkFire(&aSet,k,A->enabled,events) ||
           k == A->enabling) {
        }
        else {
#ifdef __HPN_ADD_ACTION__
          printf("Not firable b/c %s->%s\n",events[k]->event,
                 events[A->enabled]->event);
#endif
          firable = false;
          break;
        }
      }
    }
    if(firable) {
      A->cls = 'F';
    }
  }
  
  bool possible = true;  
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
#ifdef __HPN_ADD_ACTION__
  printf("Checking against: ");
  printAction(*iter,events);
#endif
    int i;
    int j;
    if(A->type == 'V') {  
      i = getZoneIndex(z,-1,A->enabled);
    }
    else if(A->type == 'F') {
      i = getZoneIndex(z,A->enabling,A->enabled);
    }
    else {
      /* this should also work for 'J' and 'E' b/c the value is put in
         both enabling and enabled */
#ifdef __HPN_ADD_ACTION__
      printf("getZoneIndex for: ");
      printAction(A,events);
#endif
      i = getZoneIndex(z,-1,A->enabling);
    }

    if((*iter)->type == 'V') {
      j = getZoneIndex(z,-1,(*iter)->enabled);
    }
    else if((*iter)->type == 'F') {
      j = getZoneIndex(z,(*iter)->enabling,(*iter)->enabled);
    }
    else {
      /* this should also work for 'J' and 'E' b/c the value is put in
         both enabling and enabled */
      j = getZoneIndex(z,-1,(*iter)->enabling);
    }

    int rv1l;
    int rv1u;
    int rv2l;
    int rv2u;

    /* for deletions */
    if((*iter)->type == 'D') {
      if(cState->warp[((*iter)->enabling)-nevents] > 0) {
        rv1l = chkDiv(-1 * rules[(*iter)->enabling]
                      [(*iter)->enabled]->pupper,
                      cState->warp[((*iter)->enabling)-nevents],'C');
        if(!(z->matrix[0][j] > rv1l)) {
          rv1l = z->matrix[0][j];
        }
        rv1u = chkDiv(rules[(*iter)->enabling]
                      [(*iter)->enabled]->pupper,
                      cState->warp[((*iter)->enabling)-nevents],'C');
        if(!(z->matrix[j][0] < rv1u)) {
          rv1u = z->matrix[j][0];
        }
      }
      else {
        rv1l = chkDiv(-1 * rules[(*iter)->enabling]
                      [(*iter)->enabled]->plower,
                      cState->warp[((*iter)->enabling)-nevents],'C');
        if(!(z->matrix[0][j] > rv1l)) {
          rv1l = z->matrix[0][j];
        }
        rv1u = chkDiv(rules[(*iter)->enabling]
                      [(*iter)->enabled]->plower,
                      cState->warp[((*iter)->enabling)-nevents],'C');
        if(!(z->matrix[j][0] < rv1u)) {
          rv1u = z->matrix[j][0];
        }
      }
    }
    /* for delete predicate  and intro predicate */
    if((*iter)->type == 'E' || (*iter)->type == 'J') {
      rv1l = chkDiv(-1 * (*iter)->ineq->constant,
                    cState->warp[(*iter)->enabled-nevents],'C');
      rv1u = chkDiv((*iter)->ineq->constant,
                    cState->warp[(*iter)->enabled-nevents],'C');
    }
    /* for intros */
    else if((*iter)->type == 'I') {
      if(cState->warp[(*iter)->enabling-nevents] > 0) {
        rv1l = chkDiv((-1 * rules[(*iter)->enabling]
                       [(*iter)->enabled]->plower),
                      cState->warp[(*iter)->enabling-nevents],'C');
        if(!(z->matrix[0][j] > rv1l)) {
          rv1l = z->matrix[0][j];
        }
        rv1u = chkDiv((rules[(*iter)->enabling]
                       [(*iter)->enabled]->plower),
                      cState->warp[(*iter)->enabling-nevents],'C');
        if(!(z->matrix[j][0] < rv1u)) {
          rv1u = z->matrix[j][0];
        }
      }
      else {
        rv1l = chkDiv((-1 * rules[(*iter)->enabling]
                       [(*iter)->enabled]->pupper),
                      cState->warp[(*iter)->enabling-nevents],'C');
        if(!(z->matrix[0][j] > rv1l)) {
          rv1l = z->matrix[0][j];
        }
        rv1u = chkDiv((rules[(*iter)->enabling]
                       [(*iter)->enabled]->pupper),
                      cState->warp[(*iter)->enabling-nevents],'C');
        if(!(z->matrix[j][0] < rv1u)) {
          rv1u = z->matrix[j][0];
        }
      }
    }
    /* for delete variable */
    else if((*iter)->type == 'V') {
      if(cState->warp[(*iter)->enabled-nevents] > 0) {
        rv1l = chkDiv((-1 * events[(*iter)->enabled]->urange),
                      cState->warp[(*iter)->enabled-nevents],'C');
        if(!(z->matrix[0][j] > rv1l)) {
          rv1l = z->matrix[0][j];
        }
        rv1u = chkDiv((events[(*iter)->enabled]->urange),
                      cState->warp[(*iter)->enabled-nevents],'C');
        if(!(z->matrix[j][0] < rv1u)) {
          rv1u = z->matrix[j][0];
        }
      }
      else {
        rv1l = chkDiv((-1 * events[(*iter)->enabled]->lrange),
                      cState->warp[(*iter)->enabled-nevents],'C');
        if(!(z->matrix[0][j] > rv1l)) {
          rv1l = z->matrix[0][j];
        }
        rv1u = chkDiv((events[(*iter)->enabled]->lrange),
                      cState->warp[(*iter)->enabled-nevents],'C');
        if(!(z->matrix[j][0] < rv1u)) {
          rv1u = z->matrix[j][0];
        }
      }
    }
  
    /* for deletions */
    if(A->type == 'D') {
      if(cState->warp[(A->enabling)-nevents] > 0) {
        rv2l = chkDiv(-1 * rules[A->enabling][A->enabled]->pupper,
                      cState->warp[(A->enabling)-nevents],'C');
        if(!(z->matrix[0][i] > rv2l)) {
          rv2l = z->matrix[0][i];
        }
        rv2u = chkDiv(rules[A->enabling][A->enabled]->pupper,
                      cState->warp[(A->enabling)-nevents],'C');
        if(!(z->matrix[i][0] < rv2u)) {
          rv2u = z->matrix[i][0];
        }
      }
      else {
        rv2l = chkDiv(-1 * rules[A->enabling][A->enabled]->plower,
                      cState->warp[(A->enabling)-nevents],'C');
        if(!(z->matrix[0][i] > rv2l)) {
          rv2l = z->matrix[0][i];
        }
        rv2u = chkDiv(rules[A->enabling][A->enabled]->plower,
                      cState->warp[(A->enabling)-nevents],'C');
        if(!(z->matrix[i][0] < rv2u)) {
          rv2u = z->matrix[i][0];
        }
      }
    }
    /* for delete predicate and intro predicate */
    else if(A->type == 'E' || A->type == 'J') {
      rv2l = chkDiv(-1 * A->ineq->constant,
                    cState->warp[A->enabled-nevents],'C');
      rv2u = chkDiv(A->ineq->constant,
                    cState->warp[A->enabled-nevents],'C');
    }
    /* for intros */
    else if(A->type == 'I') {
      if(cState->warp[A->enabling-nevents] > 0) {
        rv2l = chkDiv(-1 * (rules[A->enabling][A->enabled]->plower),
                      cState->warp[A->enabling-nevents],'C');
        if(!(z->matrix[0][i] > rv2l)) {
          rv2l = z->matrix[0][i];
        }
        rv2u = chkDiv((rules[A->enabling][A->enabled]->plower),
                      cState->warp[A->enabling-nevents],'C');
        if(!(z->matrix[i][0] < rv2u)) {
          rv2u = z->matrix[i][0];
        }
      }
      else {
        rv2l = chkDiv(-1 * (rules[A->enabling][A->enabled]->pupper),
                      cState->warp[A->enabling-nevents],'C');
        if(!(z->matrix[0][i] > rv2l)) {
          rv2l = z->matrix[0][i];
        }
        rv2u = chkDiv((rules[A->enabling][A->enabled]->pupper),
                      cState->warp[A->enabling-nevents],'C');
        if(!(z->matrix[i][0] < rv2u)) {
          rv2u = z->matrix[i][0];
        }
      }
    }
    /* for delete variable */
    else if((*iter)->type == 'V') {
#ifdef __HPN_VALGRIND__
      printf("V: A->enabled-nevents: %d\n",A->enabled-nevents);
      printAction(A,events);
#endif
      if(cState->warp[A->enabled-nevents] > 0) {
        rv2l = chkDiv((-1 * events[A->enabled]->urange),
                      cState->warp[A->enabled-nevents],'C');
        if(!(z->matrix[0][j] > rv2l)) {
          rv2l = z->matrix[0][j];
        }
        rv2u = chkDiv((events[A->enabled]->urange),
                      cState->warp[A->enabled-nevents],'C');
        if(!(z->matrix[j][0] < rv2u)) {
          rv2u = z->matrix[j][0];
        }
      }
      else {
        rv2l = chkDiv((-1 * events[A->enabled]->lrange),
                      cState->warp[A->enabled-nevents],'C');
        if(!(z->matrix[0][j] > rv2l)) {
          rv2l = z->matrix[0][j];
        }
        rv2u = chkDiv((events[A->enabled]->lrange),
                      cState->warp[A->enabled-nevents],'C');
        if(!(z->matrix[j][0] < rv2u)) {
          rv2u = z->matrix[j][0];
        }
      }
    }

#ifdef __HPN_ADD_ACTION__
    printf("rv1l: %d rv1u: %d rv2l: %d rv2u: %d\n",rv1l,rv1u,rv2l,rv2u);
#endif

    
    /* Case 1: AT1 & AT2 */
    if(A->type != 'F' &&
       (*iter)->type != 'F') {
      /* Determine if the two actions are for the same place.  This is
         complicated b/c DEL_VAR events have the place stored in the
         enabled place.  */
      bool sameVar = false;
      if(A->type != 'V' && (*iter)->type != 'V') {
        if((*iter)->enabling == A->enabling) {
          sameVar = true;
        }
      }
      else {
        if(A->type == 'V') {
          if((*iter)->type == 'V') {
            /* TBD: Add a sanity check here */
            sameVar = false;
          }
          else {
            if(A->enabled == (*iter)->enabling) {
              sameVar = true;
            }
          }
        }
        else {
          if(A->enabling == (*iter)->enabled) {
            sameVar = true;
          }
        }
      }

#ifdef __HPN_ADD_ACTION__
      if(!sameVar) {
        printf("Not sameVar.  ");
        printAction(A,events);
        printAction(*iter,events);
      } 
#endif
        
      if(sameVar) {
        if(rv1l > rv2l) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 1\n");
#endif
          possible = false;
        }
        else if(rv2l > rv1l) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 2\n");
          printf("Adding to erase list: ");
          printAction(*iter,events);
          
#endif
          e->l->push_back(*iter);
        }
        else if (rv1u > rv2u) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 3\n");
          printf("Adding to erase list: ");
          printAction(*iter,events);
#endif
          e->l->push_back(*iter);
        }
        else if (rv2u > rv1u) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 4\n");
#endif
          possible = false;
        }
        else {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 5\n");
#endif
          A->set = (*iter)->set;
        }
      }
      else {
        if (rv1l > rv2l + z->matrix[i][j]) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 6\n");
#endif
          possible = false;
        }
        else if (rv2l > rv1l + z->matrix[j][i]) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 7\n");
          printf("Adding to erase list: ");
          printAction(*iter,events);
#endif
          e->l->push_back(*iter);
        }
        else if (rv1u > z->matrix[i][0] + z->matrix[j][i]) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 8\n");
          printf("Adding to erase list: ");
          printAction(*iter,events);
#endif
          e->l->push_back(*iter);
        }
        else if (rv2u > z->matrix[j][0] + z->matrix[i][j]) {
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 9\n");
#endif
          possible = false;
        }
        else if ((rv1l == rv2l + z->matrix[i][j]) &&
                 (rv2l == rv1l + z->matrix[j][i]) &&
                 (rv1u == rv2u + z->matrix[j][i]) &&
                 (rv2u == rv1u + z->matrix[i][j])) {
          A->set = (*iter)->set;
#ifdef __HPN_ADD_ACTION__
          printf("Add action case 10\n");
#endif
        }
        else {
          /* Interleave the events - this is the default as A->set and
             (*iter)->set are different values by default. */
#ifdef __HPN_ADD_ACTION__
          printf("Interleaving.\n");
#endif      
        }
      }
    }
    /* Case 2: FC1 & FC2 */
    else if(A->type == 'F' &&
            (*iter)->type == 'F') {
      if(rules[A->enabling][A->enabled]->lower ==
         rules[(*iter)->enabling][(*iter)->enabled]->lower &&
         rules[A->enabling][A->enabled]->upper ==
         rules[(*iter)->enabling][(*iter)->enabled]->upper &&
         z->matrix[j][i] == 0 &&
         z->matrix[i][j] == 0 &&
         A->cls != 'F' &&
         (*iter)->cls != 'F') {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 11 -- ");
        printAction(*iter,events);
#endif
        A->set = (*iter)->set;
      }
      else {
        /* Interleave the events - this is the default as A->set and
           (*iter)->set are different values by default. */
#ifdef __HPN_ADD_ACTION__
        printf("Interleaving.\n");
#endif      
      }
    }
    /* Case 3: FC1 & AT2 */
    else if((*iter)->type == 'F' &&
            A->type != 'F') {
      if(rv2l > -rules[(*iter)->enabling][(*iter)->enabled]->lower + 
         z->matrix[j][i]) {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 16\n");
        printf("Adding to erase list: ");
        printAction(*iter,events);
#endif
        e->l->push_back(*iter);
      } 
      else if (rv2u > z->matrix[j][0] + z->matrix[i][j]) {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 12\n");
#endif
        possible = false;
      }
      else if((-rules[(*iter)->enabling][(*iter)->enabled]->lower ==
               rv2l + z->matrix[i][j]) &&
              /* TODO: CHECK THIS */
              (rules[(*iter)->enabling][(*iter)->enabled]->upper ==
               rv2l + z->matrix[i][j]) &&
              (rv2l == -rules[(*iter)->enabling][(*iter)->enabled]->lower
               + z->matrix[j][i]) &&
              (z->matrix[j][0] == rv2u + z->matrix[j][i]) &&
              (rv2u == z->matrix[j][0] + z->matrix[i][j])) {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 13\n");
#endif
        A->set = (*iter)->set;
      }
    }
    
    /* Case 4: AC1 & FC2 */
    else if((*iter)->type != 'F' &&
            A->type == 'F') {
      if(rv1l > (((-1)*(rules[A->enabling][A->enabled]->lower)) + 
                 z->matrix[i][j])) {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 15\n");
#endif
        possible = false;
      }
      else if(rv1u > z->matrix[i][0] + z->matrix[j][i]) {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 14\n");
        printf("Adding to erase list: ");
        printAction(*iter,events);
#endif
        e->l->push_back(*iter);
      }
      else if((-rules[A->enabling][A->enabled]->lower == rv1l +
               z->matrix[j][i]) &&
              /* TODO: CHECK THIS */
              (rules[A->enabling][A->enabled]->upper == rv1l +
               z->matrix[j][i]) &&
              (rv1l == -rules[A->enabling][A->enabled]->lower +
               z->matrix[i][j]) &&
              (z->matrix[i][0] == rv1u + z->matrix[i][j]) &&
              (rv1u == z->matrix[i][0] + z->matrix[j][i])) {
#ifdef __HPN_ADD_ACTION__
        printf("Add action case 17\n");
#endif
        A->set = (*iter)->set;
      }
    }
  }
  
  if(possible) {
#ifdef __HPN_ADD_ACTION__
    printf("Adding action: ");
    printAction(A,events);
#endif
    aSet->l->push_front(A);
    for(list<hpnActionADT>::iterator iter=e->l->begin();
        iter!=e->l->end();iter++) {
      aSet->l->remove((*iter));
    }
  }
  else {
#ifdef __HPN_ADD_ACTION__
    printf("NOT adding action: \n");
    printAction(A,events);
#endif
    free(A);
    if(!e->l->empty()) {
#ifdef __HPN_WARN__
      cSetFg(RED);
      printf("WARNING: ");
      cSetAttr(NONE);
      printf("Erase list is not empty, but the action wasn't possible.\n");
#endif
#ifdef __HPN_ADD_ACTION__
      printf("Erase list is: ");
      for(list<hpnActionADT>::iterator iter=e->l->begin();iter!=e->l->end();
          iter++) {
        printAction(*iter,events);
      }
#endif
      /* for safety run the code to remove the erase list items b/c
         they are freed later */
      for(list<hpnActionADT>::iterator iter=e->l->begin();
          iter!=e->l->end();iter++) {
        aSet->l->remove((*iter));
      }
    }
  }

  //e->l->clear();
  freeActionSet(e);
  
#ifdef __HPN_ADD_ACTION__
  printZone(z,events,cState,nevents);
#endif
  
#ifdef __HPN_TRACE__
  printf("addActionSetItem():End\n");
#endif
}

/*****************************************************************************
 * Create the set of actions that will be done in a single step.
 *****************************************************************************/
hpnActionSetADT buildWorkingActions(hpnActionSetADT &aSet)
{
  hpnActionSetADT retSet = createHpnActionSet();
  hpnActionSetADT rmSet = createHpnActionSet();

  /* Go through the set and pick the highest set number to make this
     consistent with the lhpn code. */
  int max = 0;
  for(list<hpnActionADT>::iterator i=aSet->l->begin();i!=aSet->l->end();i++) {
    if((*i)->set > max) {
      max = (*i)->set;
    }
  }
  
  /* select an action to seed the set */
  hpnActionADT a;
  for(list<hpnActionADT>::iterator i=aSet->l->begin();i!=aSet->l->end();i++) {
    if((*i)->set == max) {
      retSet->l->push_front(*i);
      aSet->l->erase(i);
      a = retSet->l->front();
      break;
    }
  }

/*   hpnActionADT a = rmActionSetItem(aSet); */

/*   /\* select an action to seed the set *\/ */
/*   retSet->l->push_front(a); */
  
  int fireCnt = 0;
  if(a->cls == 'F') {
    fireCnt++;
  }


  /* determine how many actions in the same group as the seed action
     are going to result in a transition firing */
  for(list<hpnActionADT>::iterator i=aSet->l->begin();i!=aSet->l->end();i++) {
    if(a->set == (*i)->set && (*i)->cls == 'F') {
      fireCnt++;
    }
  }

  if(fireCnt < 2) {
    /* if there is only one event that results in a transition firing
       then proceed normally */
    for(list<hpnActionADT>::iterator i=aSet->l->begin();i!=aSet->l->end();
        i++) {
      if(a->set == (*i)->set) {
        rmSet->l->push_front(*i);
        retSet->l->push_front(*i);
      }
    }
  }
  else {
    /* if there are multiple events that result in a transition firing
       then only allow a single one into the working set */
    if(a->cls == 'F') {
      for(list<hpnActionADT>::iterator i=aSet->l->begin();i!=aSet->l->end();
          i++) {
        if(a->set == (*i)->set && (*i)->cls != 'F') {
          hpnActionADT newA = copyAction((*i));
          retSet->l->push_front(newA);
        }
      }
    }
    else {
      bool foundFire = false;
      for(list<hpnActionADT>::iterator i=aSet->l->begin();i!=aSet->l->end();
          i++) {
        if(a->set == (*i)->set && (*i)->cls != 'F') {
          hpnActionADT newA = copyAction((*i));
          retSet->l->push_front(newA);
        }
        else if(a->set == (*i)->set && (*i)->cls == 'F' && !foundFire) {
          foundFire = true;
          rmSet->l->push_front(*i);
          retSet->l->push_front(*i);
        }
      }
    }
  }

  /* remove the actions from the working set from the action set */
  for(list<hpnActionADT>::iterator i=rmSet->l->begin();i!=rmSet->l->end();
      i++) {
    aSet->l->remove(*i);
  }
  rmSet->l->clear();
  delete rmSet->l;
  free(rmSet);
  
  return retSet;
}

/*****************************************************************************
 * Perform set subtraction on a pair of action sets (a-b).
 *****************************************************************************/
hpnActionSetADT actionSetSubtract(hpnActionSetADT &a,hpnActionSetADT &b,
                                  eventADT *events)
{
  hpnActionSetADT e = createHpnActionSet();
  hpnActionSetADT c = copyActionSet(a);
  
  for(list<hpnActionADT>::iterator i=b->l->begin();i != b->l->end();
      i++) {
    for(list<hpnActionADT>::iterator j=c->l->begin();j != c->l->end();
        j++) {
      if((*i)->enabling == (*j)->enabling &&
         (*i)->enabled == (*j)->enabled &&
         (*i)->type == (*j)->type) {
#ifdef __HPN_GRAY_SET__
        printf("Removing??? ");
        printAction(*j,events);
#endif
        e->l->push_front(*j);
        break;
      }
    }
  }
  
  for(list<hpnActionADT>::iterator i=e->l->begin();i != e->l->end();
      i++) {
    c->l->remove(*i);
  }
  return c;
}

/*****************************************************************************
 * Union the given sets and return the result.
 *****************************************************************************/
hpnActionSetADT actionSetUnion(hpnActionSetADT &a,hpnActionSetADT &b)
{
  hpnActionSetADT c = createHpnActionSet();
  for(list<hpnActionADT>::iterator i = a->l->begin();i != a->l->end();
      i++){
    uncondAddActionSetItem(*i,c);
  }
  for(list<hpnActionADT>::iterator i = b->l->begin();i != b->l->end();
      i++){
    uncondAddActionSetItem(*i,c);
  }
  return c;
}

/*****************************************************************************
 * Remove an action from the item set.
 *****************************************************************************/
hpnActionADT rmActionSetItem(hpnActionSetADT &aSet)
{
#ifdef __HPN_TRACE__
  printf("rmActionSetItem():Start\n");
#endif
  hpnActionADT A;
  if(!aSet->l->empty()) {
    A = aSet->l->front();
    aSet->l->pop_front();
  }
  else {
    return NULL;
  }
#ifdef __HPN_TRACE__
  printf("rmActionSetItem():End\n");
#endif
  return A;
}

/*****************************************************************************
 * Copy an action set.
 *****************************************************************************/
hpnActionADT copyAction(hpnActionADT a)
{
#ifdef __HPN_TRACE__
  printf("copyActionSet():Start\n");
#endif
  hpnActionADT copyA;

  copyA = (hpnActionADT)GetBlock(sizeof(*a));
  copyA->enabling = a->enabling;
  copyA->enabled = a->enabled;
  copyA->type = a->type;
  copyA->set = a->set;
  copyA->cls = a->cls;
  copyA->ineq = a->ineq;
   
#ifdef __HPN_TRACE__
  printf("copyActionSet():End\n");
#endif
  return copyA;
}

/*****************************************************************************
 * Copy an action set.
 *****************************************************************************/
hpnActionSetADT copyActionSet(hpnActionSetADT aSet)
{
#ifdef __HPN_TRACE__
  printf("copyActionSet():Start\n");
#endif
  hpnActionSetADT copySet = createHpnActionSet();
  
  hpnActionADT action;

  for(list<hpnActionADT>::iterator i = aSet->l->begin();
      i != aSet->l->end();i++) {
    action = copyAction(*i);
    copySet->l->push_back(action);
  }
#ifdef __HPN_TRACE__
  printf("copyActionSet():End\n");
#endif
  copySet->setCnt = aSet->setCnt;
  
  return copySet;
}

/*****************************************************************************
 * Print an action set.
 *****************************************************************************/
void printActionSet(hpnActionSetADT aSet,eventADT *events)
{
  for(list<hpnActionADT>::iterator i = aSet->l->begin();
      i != aSet->l->end();i++) {
    printAction(*i,events);
  }
}

/*****************************************************************************
 * Free the action set.
 *****************************************************************************/
void freeActionSet(hpnActionSetADT aSet)
{
/*   hpnActionADT a; */
/*   while(!aSet->l->empty()) { */
/*     a = aSet->l->front(); */
/*     free(a); */
/*     aSet->l->pop_front(); */
/*   } */
  aSet->l->clear();
  delete aSet->l;
  free(aSet);
}

/*****************************************************************************
 * Print an action.
 *****************************************************************************/
void printAction(hpnActionADT a,eventADT *events)
{
  if(a->type == 'F') {
    printf("FIRE : ");
  }
  else if(a->type == 'I') {
    printf("INTRO : ");
  }
  else if(a->type == 'J') {
    printf("INTRO_PRED : ");
    printIneq(a->ineq,events);
    printf(" : ");
  }
  else if(a->type == 'D') {
    printf("DEL : ");
  }
  else if(a->type == 'E') {
    printf("DEL_PRED : ");
    printIneq(a->ineq,events);
    printf(" : ");
  }
  else if(a->type == 'V') {
    printf("DEL_VAR : ");
  }
  if(a->enabling == -1) {
    if(a->enabled == -1) {
    }
    else {
      printf("%s",events[a->enabled]->event);
    }
  }
  else {
    printf("%s->%s",events[a->enabling]->event,
           events[a->enabled]->event);
  }
  printf(" : %d", a->set);
  printf(" : %c",a->cls);
  printf("\n");
}

/*****************************************************************************
 * Print an action w/out the ending new line.
 *****************************************************************************/
void printActionSimple(hpnActionADT a,eventADT *events)
{
  if(a->type == 'F') {
    printf("FIRE : ");
  }
  else if(a->type == 'I') {
    printf("INTRO : ");
  }
  else if(a->type == 'J') {
    printf("INTRO_PRED : ");
  }
  else if(a->type == 'D') {
    printf("DEL : ");
  }
  else if(a->type == 'E') {
    printf("DEL_PRED : ");
  }
  else if(a->type == 'V') {
    printf("DEL_VAR : ");
  }
  if(a->enabling == -1) {
    printf("%s",events[a->enabled]->event);
  }
  else {
    printf("%s->%s",events[a->enabling]->event,
           events[a->enabled]->event);
  }
  printf(" : %d", a->set);
  printf(" : %c",a->cls);
}

/*****************************************************************************
 * Add n dimensions to the matrix.  The added dimensions are created
 * via createZone so the added dimensions are initialized to default
 * values.
 *****************************************************************************/
void hpnAddDim(hpnZoneADT *z,int n)
{
#ifdef __HPN_TRACE__
  printf("hpnAddDim(%d):Start\n",n);
#endif

  hpnZoneADT zone = *z;
  hpnZoneADT newZone = createZone(zone->numClocks+n);
  
  /* copy the old values and leave the new values in the initial state
     for now */
  copyZone(newZone,zone);
  
  freeZone(zone);
  *z = newZone;
  
#ifdef __HPN_TRACE__
  printf("hpnAddDim():End\n");
#endif
}

/*****************************************************************************
 * Move a continuous place from the active section of the zone into
 * the inactive places section of the zone.  The place passed
 * into the function is a zone based index.
 *****************************************************************************/
void hpnMakeInactive(hpnZoneADT *z, int p)
{
#ifdef __HPN_TRACE__
  printf("hpnMakeInactive(%d):Start\n",p);
#endif
  hpnZoneADT zone = *z;

  /* swap the place to the edge of the inactive section */
  swapZone(z,zone->dbmEnd,p);
  /* make the inactive section include the newly swapped variable */
  zone->dbmEnd--;
  
#ifdef __HPN_TRACE__
  printf("hpnMakeInactive(%d):End\n",p);
#endif
}

/*****************************************************************************
 * Add and initialize a clock for the given rule to the given zone.
 *****************************************************************************/
void dbmAdd(hpnZoneADT *z,int enabling,int enabled,hpnBounds clk,
            cStateADT cState,ruleADT **rules,int nevents,int nplaces)
{
  if(enabling!=(-1)) {
    if(infopt(cState,rules,enabling,enabled,nevents,nplaces)) {
      addCf(enabling,enabled,cState,rules);
#ifdef __HPN_ENABLINGS_DEBUG__
      printf("Infinity optimization.\n");
#endif
      return;
    }
  }
#ifdef __HPN_TRACE__
  printf("dbmAdd(clk):Start\n");
#endif
  int oldSize = (*z)->numClocks;
  int oldEnd = (*z)->dbmEnd;
  /* add space for the new dimension */
  hpnAddDim(z,1);
  hpnZoneADT zone = *z;
  
  /* fill in the details for the new dimension */
  zone->curClocks[oldSize].enabling = enabling;
  zone->curClocks[oldSize].enabled = enabled;

  zone->matrix[0][oldSize] = -1 * clk.lower;
  zone->matrix[oldSize][0] = clk.upper;
  /* adjust other timers in accordance with the new dimension
     remembering to take into account INFIN */
  for(int j=0;j<oldEnd;j++) {
    if(zone->matrix[0][j] != INFIN
       && clk.upper != INFIN) {
      zone->matrix[oldSize][j] = zone->matrix[0][j]
        + clk.upper;
    }
    if(zone->matrix[j][0] != INFIN
       && clk.lower != INFIN) {
      zone->matrix[j][oldSize] = zone->matrix[j][0]
        - clk.lower;
    }
  }

  /* There may be inactive variables in zone->matrix.  The new
     dimension gets added on to the end of the dbm which potentially
     makes the newly added dimension not a part of the dbm.  Use swap
     to guarantee that the new dimesion is a part of the zone. */
  if(oldSize != zone->dbmEnd) {
    swapZone(&zone,oldSize,zone->dbmEnd);
  }
  zone->dbmEnd++;
  
#ifdef __HPN_TRACE__
  printf("dbmAdd(clk):End\n");
#endif
}

/*****************************************************************************
 * Move an inactive clock into the active section of the zone.  p is an
 * event based index.
 *****************************************************************************/
void dbmAdd(hpnZoneADT *z,int p)
{
#ifdef __HPN_TRACE__
  printf("dbmAdd(var):Start\n");
#endif

  hpnZoneADT zone = *z;

  int i = getZoneIndex(zone,-1,p);
  /* TBD: add a warning here */
  if(i == -1) {
#ifdef __HPN_TRACE__
    printf("dbmAdd(var):End\n");
#endif
    return;
  }
  
  if(i != zone->dbmEnd) {
    swapZone(z,i,zone->dbmEnd);
  }
  zone->dbmEnd++;
  
#ifdef __HPN_TRACE__
  printf("dbmAdd(var):End\n");
#endif
}

/*****************************************************************************
 * Remove the specified event from the zone.
 *****************************************************************************/
void dbmRemove(hpnZoneADT *z,int enabling,int enabled)
{
#ifdef __HPN_TRACE__
  printf("dbmRemove():Start\n");
#endif
  
  hpnZoneADT zone = *z;
  int index = getZoneIndex(zone,enabling,enabled);
  if(index == -1) {
#ifdef __HPN_TRACE__
    printf("dbmRemove():End\n");
#endif
    return;
  }
  
  hpnZoneADT newZone;

#ifdef __HPN_DEBUG__OFF
  printPlainZone(zone);
#endif
  
  newZone = createZone(zone->numClocks-1);
  newZone->dbmEnd = zone->dbmEnd-1;

  if(index == 0) {
    for(int i=1;i<zone->numClocks;i++) {
      newZone->curClocks[i-1].enabling = zone->curClocks[i].enabling;
      newZone->curClocks[i-1].enabled = zone->curClocks[i].enabled;
      for(int j=1;j<zone->numClocks;j++) {
        newZone->matrix[i-1][j-1] = zone->matrix[i][j];
      }
    }
  }
  else {
    for(int i=0;i<zone->numClocks;i++) {
      if(i < index) {
        newZone->curClocks[i].enabling = zone->curClocks[i].enabling;
        newZone->curClocks[i].enabled = zone->curClocks[i].enabled;
        for(int j=0;j<zone->numClocks;j++) {
          if(j < index) {
            newZone->matrix[i][j] = zone->matrix[i][j];
          }
          else if(j > index) {
            newZone->matrix[i][j-1] = zone->matrix[i][j];
          }
        }
      }
      else if(i > index) {
        newZone->curClocks[i-1].enabling = zone->curClocks[i].enabling;
        newZone->curClocks[i-1].enabled = zone->curClocks[i].enabled;
        for(int j=0;j<zone->numClocks;j++) {
          if(j < index) {
            newZone->matrix[i-1][j] = zone->matrix[i][j];
          }
          else if(j > index) {
            newZone->matrix[i-1][j-1] = zone->matrix[i][j];
          }
        }
      }
    }
  } 
  freeZone(zone);

#ifdef __HPN_TRACE__
  printf("dbmRemove():End\n");
  printPlainZone(newZone);
#endif
  *z = newZone;
}

/*****************************************************************************
 * Move an active clock into the inactive section of the zone.  p is an
 * event based index.
 *****************************************************************************/
void dbmRemove(hpnZoneADT *z,int p)
{
#ifdef __HPN_TRACE__
  printf("dbmRemove(var):Start\n");
#endif

  hpnZoneADT zone = *z;

  int i = getZoneIndex(zone,-1,p);
  if(i == -1) {
    cSetFg(RED);
    printf("Error:");
    cSetAttr(NONE);
    printf("Invalid place given to dbmRemove().\n");
#ifdef __HPN_TRACE__
    printf("dbmRemove(var):End\n");
#endif
    return;
  }
  if(i >= zone->dbmEnd) {
    return;
  }
  
  if(i != zone->dbmEnd-1) {
    swapZone(z,i,zone->dbmEnd-1);
  }
  zone->dbmEnd--;

  /* since the variable is now inactive remove any relations to other
     variables */
  for(int j=1;j<zone->numClocks;j++) {
    if(zone->dbmEnd != j) {
      zone->matrix[zone->dbmEnd][j] = INFIN;
      zone->matrix[j][zone->dbmEnd] = INFIN;
    }
  }
  
#ifdef __HPN_TRACE__
  printf("dbmRemove(var):End\n");
#endif
}

/*****************************************************************************
 * Returns true if the given rule is found in the zone.  p and t are
 * event based indices.
 *****************************************************************************/
bool isMemberDBM(int p,int t,hpnZoneADT zone)
{
  for(int i=0;i<zone->dbmEnd;i++) {
    if(zone->curClocks[i].enabling == p && zone->curClocks[i].enabled == t) {
#ifdef __HPN_DEBUG__
      printf("In zone.\n");
#endif
      return true;
    }
  }
#ifdef __HPN_DEBUG__
  printf("NOT in zone.\n");
#endif
  return false;
}

/*****************************************************************************
 * Returns true if the given rule is found in the zone.  p and t are
 * event based indices.
 *****************************************************************************/
int indexDBM(int p,int t,hpnZoneADT zone)
{
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling == p && zone->curClocks[i].enabled == t) {
      return i;
    }
  }
  return -1;
}

/*****************************************************************************
 * Put the warp into the clockkey.
 *****************************************************************************/
void warpClockkey(cStateADT cState,hpnZoneADT zone,int nevents)
{
  for(int i=0;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling == -1) {
      if(isMemberDBM(-1,zone->curClocks[i].enabled,zone)) {
        zone->curClocks[i].eventk_num =
          cState->warp[zone->curClocks[i].enabled-nevents];
      }
      else {
        zone->curClocks[i].eventk_num = 0;
      }
    }
    else {
      zone->curClocks[i].eventk_num = 1;
    }
  }
}

/*****************************************************************************
 * Test to see if the given inequality is true or false.
 *****************************************************************************/
bool testIneq(ineqADT i,cStateADT cState,hpnZoneADT zone,
              eventADT *events,int nevents,ruleADT **rules,
              dMarkingADT marking)
{
#ifdef __HPN_INEQ_DEBUG__
  printf("In testIneq ");
  printIneq(i,events);
  printf("\n");
#endif
  if(events[i->place]->type & CONT) {
    int warpVal = chkDiv(i->constant,
                         cState->warp[i->place-nevents],'C');
    if (cState->warp[i->place-nevents] < 0) {
      warpVal = (-1)*warpVal;
    }
    int zoneI = getZoneIndex(zone,-1,i->place);
#ifdef __HPN_WARN__
    if(zoneI == -1) {
      cSetFg(RED);
      printf("WARNING: ");
      cSetAttr(NONE);
      printf("place %s not found in the zone.\n",events[i->place]->event);
#ifdef __HPN_INEQ_DEBUG__
      printZone(zone,events,cState,nevents);
#endif
    }
#endif
    if(cState->warp[i->place-nevents] >= 0) {
      if(i->type == 0) {
        if(!(zone->matrix[zoneI][0] >= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("1. %s[%d] > %d[%d] is not true.\n",
                 events[i->place]->event,zone->matrix[zoneI][0],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 1) {
        if(!(zone->matrix[zoneI][0] >= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("2. %s[%d] >= %d[%d] is not true.\n",
                 events[i->place]->event,zone->matrix[zoneI][0],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 2) {
        if(!((-1 * zone->matrix[0][zoneI]) <= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("3. %s[%d] < %d[%d] is not true.\n",
                 events[i->place]->event,-1*zone->matrix[0][zoneI],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 3) {
        if(!((-1 * zone->matrix[0][zoneI]) <= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("4. %s[%d] <= %d[%d] is not true.\n",
                 events[i->place]->event,-1*zone->matrix[0][zoneI],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 4) {
        if(!(zone->matrix[zoneI][0] == warpVal &&
             (-1 * zone->matrix[0][zoneI]) == warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("5. %s[%d,%d] = %d[%d] is not true.\n",
                 events[i->place]->event,zone->matrix[zoneI][0],
                 -1*zone->matrix[0][zoneI],i->constant,warpVal);
#endif
          return false;
        }
      }
      else {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("invalid inequality type.\n");
#endif
      }
    }
    else {
      if(i->type == 0) {
        if(!(zone->matrix[0][zoneI] >= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("6. %s[%d] > %d[%d] is not true.\n",
                 events[i->place]->event,zone->matrix[0][zoneI],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 1) {
        if(!(zone->matrix[0][zoneI] >= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("7. %s[%d] >= %d[%d] is not true.\n",
                 events[i->place]->event,zone->matrix[0][zoneI],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 2) {
        if(!((-1 * zone->matrix[zoneI][0]) <= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("8. %s[%d] < %d[%d] is not true.\n",
                 events[i->place]->event,-1*zone->matrix[zoneI][0],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 3) {
        if(!((-1 * zone->matrix[zoneI][0]) <= warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("9. %s[%d] <= %d[%d] is not true.\n",
                 events[i->place]->event,-1*zone->matrix[zoneI][0],
                 i->constant,warpVal);
#endif
          return false;
        }
      }
      else if(i->type == 4) {
        if(!((-1 * zone->matrix[zoneI][0]) == warpVal &&
             zone->matrix[0][zoneI] == warpVal)) {
#ifdef __HPN_INEQ_DEBUG__
          printf("10. %s[%d,%d] = %d[%d] is not true.\n",
                 events[i->place]->event,-1*zone->matrix[zoneI][0],
                 zone->matrix[0][zoneI],i->constant,warpVal);
#endif
          return false;
        }
      }
      else {
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("invalid inequality type.\n");
      }
    }
#ifdef __HPN_INEQ_DEBUG__
    printf("Returning true 1.\n");
#endif
    return true;
  }
  else {
    for(int j=0;j<nevents;j++) {
      if(rules[i->place][j]->ruletype > NORULE) { 
        if(marking->marking[rules[i->place][j]->marking] == 'T') {
#ifdef __HPN_INEQ_DEBUG__
          printf("Returning true 2.\n");
#endif
          return true;
        }
        else {
#ifdef __HPN_INEQ_DEBUG__
          printf("11. %s is not marked.\n",
                 events[i->place]->event);
#endif
          return false;
        }
      }
    }
    return false;
  }
}

/*****************************************************************************
* Checks to see if a rule is level satisfied.
******************************************************************************/
bool hpnLevelSatisfied(ruleADT **rules,char *state, int nsignals, 
                       int enabling,int enabled)
{
  int i;
  bool satisfied;
  level_exp curLevel;

  if(!rules[enabling][enabled]->level) {
    return true;
  }

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
    if(satisfied) {
      return true;
    }
  }
  return false;
}

/*****************************************************************************
 * Check to see if the given transition is enabled.
 *****************************************************************************/
bool enabledXition(int t,eventADT *events,cStateADT cState,dMarkingADT marking,
                   hpnZoneADT zone,int nevents,ruleADT **rules,
                   signalADT *signals,int nsignals,int nplaces)
{
  for(ineqADT i=events[t]->inequalities;i != NULL;i = i->next) {
    if(i->type <= 4 && (((events[i->place]->type & CONT) &&
                        (marking->state[i->signal]=='0')) ||
       (!(events[i->place]->type & CONT) &&  
        (!testIneq(i,cState,zone,events,nevents,rules,marking))))) {
#ifdef __HPN_INEQ_DEBUG__
      printf("Transition %s is NOT enabled.\n",events[t]->event);
#endif
      return false;
    }
  }
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(!hpnLevelSatisfied(rules,marking->state,nsignals,i,t)) {
#ifdef __HPN_INEQ_DEBUG__
      printf("Transition %s is NOT level satisfied.\n",events[t]->event);
#endif
      return false;
    }
  }
#ifdef __HPN_INEQ_DEBUG__
  printf("Transition %s is enabled.\n",events[t]->event);
#endif
  return true;
}

/*****************************************************************************
 * Do an integer divide with some extra checks and take the floor or
 * ceil of the answer based on the value given in func.  The possible
 * values are: 'F' = floor -- 'C' = ceiling
 *****************************************************************************/
int chkDiv(int a,int b,char func)
{
  int res;
  if(a == INFIN ||
     a == INFIN * -1) {
    if(b < 0) {
      return a * -1;
    }
    return a;
  }
  if(b == INFIN) {
    return 0;
  }
  if(b == 0) {
#ifdef __HPN_WARN__
    cSetFg(RED);
    printf("WARNING: ");
    cSetAttr(NONE);
    printf("divide by zero");
    printf(".\n");
#endif
    b = 1;
    //    return 0;
  }

  double Dres,Da,Db;
  Da = a;
  Db = b;
  Dres = Da/Db;
  if(func == 'C') {
    res = (int)ceil(Dres);
  }
  else if(func == 'F') {
    res = (int)floor(Dres);
  }
  return res;
}

/*****************************************************************************
 * Warp the dbm.
 *****************************************************************************/
void dbmWarp(hpnZoneADT *z,cStateADT cState,eventADT *events,ruleADT **rules,
             dMarkingADT marking,int nevents,int nplaces,signalADT *signals,
             int nsignals,int *&newWarpA)
{
#ifdef __HPN_TRACE__
  printf("dbmWarp():Start\n");
#endif
  
  hpnZoneADT zone = *z;

#ifdef __HPN_DEBUG_WARP__
  printf("Starting warp.\n");
  printZone(zone,events);
#endif
  
  for(int i=1;i<zone->dbmEnd;i++) {
    for(int j=i+1;j<zone->dbmEnd;j++) {
      double iVal = 0.0;
      double jVal = 0.0;
      int iWarp = 0;
      int jWarp = 0;
      int iXDot = 0;
      int jXDot = 0;
      
      /* deal w/ the fact that we might have continuous and discrete
         places */
#ifdef __HPN_DEBUG_WARP__
      printf("Working on %d->%d\n",i,j);
#endif
      if(zone->curClocks[i].enabling == -1) {
        iVal = fabs((double)cState->warp[zone->curClocks[i].enabled-nevents] /
                    (double)newWarpA[zone->curClocks[i].enabled-nevents]);
        iWarp = abs(cState->warp[zone->curClocks[i].enabled-nevents]);
        iXDot = abs(newWarpA[zone->curClocks[i].enabled-nevents]);
      }
      else {
        iVal = 1.0;
        iWarp = 1;
        iXDot = 1;
      }
      
      if(zone->curClocks[j].enabling == -1) {
        jVal = fabs((double)cState->warp[zone->curClocks[j].enabled-nevents] /
                    (double)newWarpA[zone->curClocks[j].enabled-nevents]);
        jWarp = abs(cState->warp[zone->curClocks[j].enabled-nevents]);
        jXDot = abs(newWarpA[zone->curClocks[j].enabled-nevents]);
      }
      else {
        jVal = 1.0;
        jWarp = 1;
        jXDot = 1;
      }

#ifdef __HPN_DEBUG_WARP__
      printf("iVal: %f, jVal: %f, iWarp: %d, jWarp: %d, iXDot: %d, jXDot: %d\n",iVal,jVal,iWarp,jWarp,iXDot,jXDot);
#endif
      
      if(iVal > jVal) {
        zone->matrix[i][j] =
          chkDiv((jWarp * zone->matrix[i][j]),jXDot,'C') +
          chkDiv((-1 * jWarp * zone->matrix[i][0]),jXDot,'C') +
          chkDiv((iWarp * zone->matrix[i][0]),iXDot,'C');
        zone->matrix[j][i] =
          chkDiv((jWarp * zone->matrix[j][i]),jXDot,'C') +
          chkDiv((-1 * jWarp * zone->matrix[0][i]),jXDot,'C') +
          chkDiv((iWarp * zone->matrix[0][i]),iXDot,'C');
      }
      else {
        zone->matrix[j][i] =
          chkDiv((iWarp * zone->matrix[j][i]),iXDot,'C') +
          chkDiv((-1 * iWarp * zone->matrix[j][0]),iXDot,'C') +
          chkDiv((jWarp * zone->matrix[j][0]),jXDot,'C');
        zone->matrix[i][j] =
          chkDiv((iWarp * zone->matrix[i][j]),iXDot,'C') +
          chkDiv((-1 * iWarp * zone->matrix[0][j]),iXDot,'C') +
          chkDiv((jWarp * zone->matrix[0][j]),jXDot,'C');
      }
#ifdef __HPN_DEBUG_WARP__
      printZone(zone,events);
#endif
    }
  }

#ifdef __HPN_DEBUG_WARP__
  printf("After fixing up initial warp conditions.\n");
  printZone(zone,events);
#endif
  
  for(int i=1;i<zone->dbmEnd;i++) {
    if(zone->curClocks[i].enabling == -1) {
      /* TBD: This is different than the algorithm that is written
         up.  Is there a good reason? */
      zone->matrix[0][i] =
        chkDiv((abs(cState->warp[zone->curClocks[i].enabled-nevents])
                * zone->matrix[0][i]),
               abs(newWarpA[zone->curClocks[i].enabled-nevents])
               ,'C');

      zone->matrix[i][0] =
        chkDiv((abs(cState->warp[zone->curClocks[i].enabled-nevents])
                * zone->matrix[i][0]),
               abs(newWarpA[zone->curClocks[i].enabled-nevents])
               ,'C');
    }
  }

#ifdef __HPN_DEBUG_WARP__
  printf("After fixing up places.\n");
  printZone(zone,events);
#endif
  
  for(int i=1;i<zone->dbmEnd;i++) {
    if(zone->curClocks[i].enabling == -1) {
#ifdef __HPN_DEBUG_WARP__
      printf("Warp: %d\n",cState->warp[zone->curClocks[i].enabled-nevents]);
#endif
      if(((float)cState->warp[zone->curClocks[i].enabled-nevents] /
          (float)newWarpA[zone->curClocks[i].enabled-nevents]) < 0.0) {
        /* swap */
        int temp = zone->matrix[0][i];
        zone->matrix[0][i] = zone->matrix[i][0];
        zone->matrix[i][0] = temp;
      
        for(int j=1;j<zone->dbmEnd;j++) {
          /* TBD: If i & j are both changing direction do we need to
             remove the warp info? */
          if(i != j) {
            zone->matrix[j][i] = INFIN;
            zone->matrix[i][j] = INFIN;
          }
        }
      }   
    }
  }

#ifdef __HPN_DEBUG_WARP__
  printf("After handling negative warps.\n");
  printZone(zone,events);
#endif
  
  for(int i=1;i<zone->dbmEnd;i++) {
    if(zone->curClocks[i].enabling == -1) {
      int newWarp = newWarpA[zone->curClocks[i].enabled-nevents];
      if(newWarp < 0) {
        if(cState->warp[zone->curClocks[i].enabled-nevents] > 0) {
#ifdef __HPN_INTROVECTOR__
          printf("resetIntroClocks1: %d/%d\n",i,nevents);
#endif
          resetIntroClocks(zone->curClocks[i].enabled,cState,rules,nevents,
                           zone);
        }
      }
      else {
        if(cState->warp[zone->curClocks[i].enabled-nevents] < 0) {
#ifdef __HPN_INTROVECTOR__
          printf("resetIntroClocks2: %d/%d\n",i,nevents);
#endif
          resetIntroClocks(zone->curClocks[i].enabled,cState,rules,nevents,
                           zone);
        }
      }
      cState->warp[zone->curClocks[i].enabled-nevents] = newWarp;
        
#ifdef __HPN_DEBUG_WARP__
      printf("New warp for %d: %d\n",i,cState->warp[zone->curClocks[i].enabled-nevents]);
#endif
    }
  }

#ifdef __HPN_DEBUG_WARP__
  printf("Before recanon.\n");
  printZone(zone,events);
#endif
  
  recanonZone(zone);
  
#ifdef __HPN_TRACE__
  printf("dbmWarp():End\n");
#endif
}

/*****************************************************************************
 * Find initial marking.
 *****************************************************************************/
dMarkingADT hpnFindInitialMarking(eventADT *events,ruleADT **rules,
                                  markkeyADT *markkey,char* arcType,
                                  int nevents,int nrules,int ninpsig,
                                  int nsignals,char * startState,
                                  bool verbose)
{
  /* allocate memory for the markingADT & its members */
  dMarkingADT marking=NULL;
  marking = (dMarkingADT)GetBlock(sizeof *marking);
  marking->marking = (char *)GetBlock((nrules+1)*sizeof(char));
  marking->enablings = (char *)GetBlock((nevents+1)*sizeof(char));
  marking->state = (char *)GetBlock((nsignals+1)*sizeof(char));
  
  marking->marking[nrules] = '\0';
  marking->enablings[nevents] = '\0';
  marking->state[nsignals] = '\0';
  
#ifdef __HPN_TRACE__
  printf("hpnFindInitialMarking():Start\n");
#endif

  /* find initially marked rules: valid rules && initially marked
     rules && dRules*/
  marking->marking[0] = 'F'; /* initialize reset rule */
  for(int i=0;i<nrules;i++) {
    if(hpnDRule(arcType,i)) {
      /* discrete place in the preset */
      if(hpnValidRule((rules[markkey[i]->enabling][markkey[i]->enabled])
                      ->ruletype)
         && markkey[i]->epsilon == 1) {
        marking->marking[i] = 'T';
      }
      else {
        marking->marking[i] = 'F';
      }
    }
    else {
      /* continuous place in the preset */
      marking->marking[i] = 'F';
    }
  }
  
  /* Copy startState as initial state, if missing abort */
  if(!hpnCopyStartState(marking,startState,verbose)) {
    return NULL;
  }

  marking->enablings[0] = 'F'; /* the reset rule isn't enabled */
  hpnEventsDiscreteEn(events,rules,markkey,marking,arcType,nrules,nevents);

  /* update the state for enabled events */
  for(int i=1;i<nevents;i++) {
    if(marking->enablings[i] == 'T' && events[i]->signal >= 0) {
      if(!hpnCheckConsistency(events[i]->event,marking->state,
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
  
#ifdef __HPN_TRACE__
  printf("hpnFindInitialMarking():End\n");
#endif 
  return marking;
}

/*****************************************************************************
 * Make the entire velocity cache dirty.
 *****************************************************************************/
void resetVelocity(cStateADT cState,int nevents)
{
  emptyBoolVector(cState->vValid,nevents);
}

/*****************************************************************************
 * Calculate the velocity.  t is an event based index.
 *****************************************************************************/
int velocity(hpnZoneADT zone,cStateADT cState,dMarkingADT marking,
             eventADT *events,ruleADT **rules,int t,int nevents,int nplaces,
             bool* D,signalADT *signals,int nsignals)
{
  /* check the cache first */
  if(cState->vValid[t]) {
#ifdef __HPN_VELOCITY__
    printf("Velocity[%s]cached: %d\n",events[t]->event,cState->vCache[t]);
#endif
    return cState->vCache[t];
  }

  /* if t is a member of D or is not discretely enabled then v is 0 */
  if(marking->enablings[t] == 'F' || D[t]) {
    cState->vCache[t] = 0;
    cState->vValid[t] = true;
    return 0;
  }
  int min = 1;

  /* check every continuous place with a marking of lrange */
  for(int p=nevents;p<nevents+nplaces;p++) {
    int index = indexDBM(-1,p,zone);
    if(rules[p][t]->ruletype > NORULE &&
       isMemberPc(p,events) &&
       zone->matrix[index][0] == -1 * chkDiv(events[p]->lrange,
                                             cState->warp[p-nevents],'C') &&
       zone->matrix[0][index] == chkDiv(events[p]->lrange,
                                        cState->warp[p-nevents],'C')) {
      /* foreach continuous transtion t1 in the preset of p */
      int num = 0;
      for(int t1=0;t1<nevents;t1++) {
        if(rules[t1][p]->ruletype > NORULE &&
           events[t1]->type & CONT) {
          D[t] = true;
          num += (rules[t1][p]->weight *
                  velocity(zone,cState,marking,events,rules,t1,nevents,nplaces,
                           D,signals,nsignals));
          D[t] = false;
        }
      }
      /* foreach continuous transition t2 in the postset of p */
      int den = 0;
      for(int t2=0;t2<nevents;t2++) {
        if(rules[p][t2]->ruletype > NORULE &&
           events[t2]->type & CONT) {
          den += (rules[p][t2]->weight * events[t2]->rate);
        }
      }
      if(den == 0) {
        cSetFg(RED);
        printf("Error: ");
        cSetAttr(NONE);
        printf("Division by zero in velocity calculation event %s and place %s\n",events[t]->event,events[p]->event);
      }
      if(num/den < min) {
        min = num/den;
      }
    }
  }

  /* check every continuous place with a marking of urange */
  for(int p=nevents;p<nevents+nplaces;p++) {
    int index = indexDBM(-1,p,zone);
    if(rules[t][p]->ruletype > NORULE &&
       isMemberPc(p,events) &&
       zone->matrix[index][0] == chkDiv(events[p]->urange,
                                        cState->warp[p-nevents],'C') &&
       zone->matrix[0][index] == -1 * chkDiv(events[p]->urange,
                                             cState->warp[p-nevents],'C')) {
      /* foreach continuous transtion t1 in the preset of p */
      int num = 0;
      for(int t1=0;t1<nevents;t1++) {
        if(rules[p][t1]->ruletype > NORULE &&
           events[t1]->type & CONT) {
          D[t] = true;
          num += (rules[p][t1]->weight *
                  velocity(zone,cState,marking,events,rules,t1,nevents,nplaces,
                           D,signals,nsignals));
          D[t] = false;
        }
      }
      /* foreach continuous transition t2 in the postset of p */
      int den = 0;
      for(int t2=0;t2<nevents;t2++) {
        if(rules[t2][p]->ruletype > NORULE &&
           events[t2]->type & CONT) {
          den += (rules[t2][p]->weight * events[t2]->rate);
        }
      }
      if(den == 0) {
        cSetFg(RED);
        printf("Error: ");
        cSetAttr(NONE);
        printf("Division by zero in velocity calculation event %s and place %s\n",events[t]->event,events[p]->event);
      }
      if(num/den < min) {
        min = num/den;
      }
    }
  }

#ifdef __HPN_VELOCITY__
  printf("t: %s min:%d rate:%d\n",events[t]->event,min,events[t]->rate);
#endif
  if(enabledXition(t,events,cState,marking,zone,nevents,rules,signals,
                   nsignals,nplaces)) {
    cState->vCache[t] = min * events[t]->rate;
    cState->vValid[t] = true;
  }
  else {
    cState->vCache[t] = 0;
    cState->vValid[t] = true;
  }
#ifdef __HPN_VELOCITY__
  printf("Velocity[%s]: %d\n",events[t]->event,cState->vCache[t]);
#endif
  return cState->vCache[t];
}

/*****************************************************************************
 * Calculate the velocity.  t is an event based index.
 *****************************************************************************/
int velocity(hpnZoneADT zone,cStateADT cState,dMarkingADT marking,
             eventADT *events,ruleADT **rules,int t,int nevents,int nplaces,
             signalADT *signals,int nsignals)
{
  /* check the cache first */
  if(cState->vValid[t]) {
#ifdef __HPN_VELOCITY__
    printf("Velocity[%s]cached: %d\n",events[t]->event,cState->vCache[t]);
#endif
    return cState->vCache[t];
  }

  /* do the calculation */
  bool* D = newBoolVector(nevents);
  int v = velocity(zone,cState,marking,events,rules,t,nevents,nplaces,D,
                   signals,nsignals);
  free(D);

#ifdef __HPN_VELOCITY__
  printf("Velocity[%s]: %d\n",events[t]->event,cState->vCache[t]);
#endif
  return v;
}

/*****************************************************************************
 * Calculate the instantaneous rate of change.  p is an event based index.
 *****************************************************************************/
int xdot(int p,eventADT *events,ruleADT **rules,dMarkingADT marking,
         hpnZoneADT zone,cStateADT cState,int nevents,int nplaces,
         signalADT *signals,int nsignals)
{
  int xdot = 0;

  if(isVar(p,events)) {
    return cState->warp[p-nevents];
  }
  
  if(!(isMemberPc(p,events))) {
    return 1;
  }
  
  for(int i=0;i<nevents;i++) {
    if(events[i]->type & CONT) {
      if(rules[i][p]->ruletype > NORULE) {
        xdot += rules[i][p]->weight * velocity(zone,cState,marking,events,
                                               rules,i,nevents,nplaces,
                                               signals,nsignals);
      }
      if(rules[p][i]->ruletype > NORULE) {
        xdot -= rules[p][i]->weight * velocity(zone,cState,marking,events,
                                               rules,i,nevents,nplaces,
                                               signals,nsignals);
      }
    }
  }
#ifdef __HPN_VELOCITY__
  printf("xdot[%s]:%d\n",events[p]->event,xdot);
#endif

  return xdot;
}

/*****************************************************************************
 * Copy the continous state.
 *****************************************************************************/
cStateADT copyCState(cStateADT cState,int nevents,int nplaces,int nrules)
{
  cStateADT newCState = (cStateADT)GetBlock(sizeof(*newCState));

  newCState->warp = (int*)GetBlock(sizeof(int) * nplaces);
  newCState->oldWarp = (int*)GetBlock(sizeof(int) * nplaces);
  for(int i=0;i<nplaces;i++) {
    newCState->warp[i] = cState->warp[i];
    newCState->oldWarp[i] = cState->oldWarp[i];
  }

  newCState->firedClocks = (char*)GetBlock((nrules+1) * sizeof(char));
  strncpy(newCState->firedClocks,cState->firedClocks,nrules+1);

  newCState->introClocks = (char*)GetBlock((nrules+1) * sizeof(char));
  strncpy(newCState->introClocks,cState->introClocks,nrules+1);

  newCState->vCache = (int*)GetBlock(nevents * sizeof(int));
  newCState->vValid = newBoolVector(nevents);
  for(int i=0;i<nevents;i++) {
    newCState->vCache[i] = cState->vCache[i];
    newCState->vValid[i] = cState->vValid[i];
  }
  return newCState;
}

/*****************************************************************************
 * Find initial continuous state.
 *****************************************************************************/
cStateADT hpnInitCState(eventADT *events,ruleADT **rules,dMarkingADT marking,
                        markkeyADT *markkey,char* arcType,int nevents,
                        int nplaces,int nrules)
{
  cStateADT cState = (cStateADT)GetBlock(sizeof *cState);
  cState->warp = (int *)GetBlock(nplaces*sizeof(int));
  cState->oldWarp = (int *)GetBlock(nplaces*sizeof(int));
  cState->firedClocks = (char *)GetBlock((nrules+1)*sizeof(char));
  cState->firedClocks[nrules] = '\0';
  cState->introClocks = (char *)GetBlock((nrules+1)*sizeof(char));
  cState->introClocks[nrules] = '\0';
  cState->vCache = (int *)GetBlock((nevents)*sizeof(int));
  cState->vValid = newBoolVector(nevents);

  /* initialize the firedClocks to all unfired */
  for(int i=0;i<nrules;i++) {
    cState->firedClocks[i] = 'F';
  }

  /* initialize the introClocks to all not introduced */
  for(int i=0;i<nrules;i++) {
    cState->introClocks[i] = 'F';
  }
  
  /* fill in the initial rates */
  for(int i=nevents;i<nevents+nplaces;i++) {
     if(events[i]->type & VAR) { 
       /* cState->warp[i-nevents] = events[i]->linitrate;  */
       cState->warp[i-nevents] = 1;
#ifdef __HPN_DEBUG__
       printf("place: %s:%d\n",events[i]->event,cState->warp[i-nevents]);
#endif
     } else if(events[i]->type & CONT) {
       cState->warp[i-nevents] = 1;
#ifdef __HPN_DEBUG__
       printf("place: %s:%d\n",events[i]->event,cState->warp[i-nevents]);
#endif
     } else {
       cState->warp[i-nevents] = 1;
     }
  }

  /* fill in the oldWarp vector */
  for(int i=0;i<nplaces;i++) {
    cState->oldWarp[i] = -1;
  }
  
  return cState;
}

/*****************************************************************************
 * Free memory allocated by the cStateADT.
 *****************************************************************************/
void freeCState(cStateADT cState)
{
  if(cState == NULL) {
    cSetFg(RED);
    printf("WARNING: ");
    fprintf(lg,"WARNING: ");
    cSetAttr(NONE);
    printf("attempting to free a NULL cState.\n");
    fprintf(lg,"attempting to free a NULL cState.\n");
    return;
  }
  
  free(cState->warp);
  free(cState->oldWarp);
  free(cState->firedClocks);
  free(cState->introClocks);
  free(cState->vCache);
  free(cState->vValid);
  free(cState);
}

/*****************************************************************************
 * Returns a data structure containing a list of inequalities in the design.
 *****************************************************************************/
hpnIneqListADT hpnInitIneqListADT(eventADT *events,int nevents)
{
  int numIneq = 0;
  for(int i=0;i<nevents;i++) {
    for(ineqADT j=events[i]->inequalities;j != NULL;j=j->next) {
      numIneq++;
    }
  }

  hpnIneqListADT hpnIneqL = (hpnIneqListADT)GetBlock(sizeof *hpnIneqL);
  hpnIneqL->ineqCnt = numIneq;
  hpnIneqL->l = (hpnIneqADT*)GetBlock(numIneq*sizeof(hpnIneqADT));
  numIneq = 0;
  for(int i=0;i<nevents;i++) {
    for(ineqADT j=events[i]->inequalities;j != NULL;j = j->next) {
      hpnIneqADT hpnIneq = (hpnIneqADT)GetBlock(sizeof *hpnIneq);
      hpnIneq->ineq = j;
      hpnIneq->xition = i;
      hpnIneqL->l[numIneq++] = hpnIneq;
    }
  }
  return hpnIneqL;
}

/*****************************************************************************
 * Free memory allocated by the hpnIneqADT.
 *****************************************************************************/
void freeHpnIneqListADT(hpnIneqListADT hpnIneqL)
{
  if(hpnIneqL == NULL) {
    cSetFg(RED);
    printf("WARNING: ");
    fprintf(lg,"WARNING: ");
    cSetAttr(NONE);
    printf("attempting to free a NULL hpnIneqListADT.\n");
    fprintf(lg,"attempting to free a NULL hpnIneqListADT.\n");
    return;
  }
  for(int i=0;i<hpnIneqL->ineqCnt;i++) {
    free(hpnIneqL->l[i]);
  }
  free(hpnIneqL->l);
  free(hpnIneqL);
}

/*****************************************************************************
 * Prints out an inequality.
 *****************************************************************************/
void printIneq(ineqADT i,eventADT *events)
{
  printf("%s",events[i->place]->event);
  if(i->type == 0) {
    printf(" > ");
  }
  else if(i->type == 1) {
    printf(" >= ");
  }
  else if(i->type == 2) {
    printf(" < ");
  }
  else if(i->type == 3) {
    printf(" <= ");
  }
  else if(i->type == 4) {
    printf(" = ");
  }
  else if(i->type == 5) {
    printf(" := ");
  }
  else if(i->type == 6) {
    printf(" dot:= ");
  }
  printf("%d",i->constant);
}

/*****************************************************************************
 * Print out an hpnIneqListADT.
 *****************************************************************************/
void printHpnIneqList(hpnIneqListADT hpnIneqL,eventADT *events)
{
  printf("ineqList: ");
  for(int i=0;i<hpnIneqL->ineqCnt;i++) {
    if(i > 0) {
      printf(",");
    }
    printf("%s:",events[hpnIneqL->l[i]->xition]->event);
    printIneq(hpnIneqL->l[i]->ineq,events);
  }
  printf("\n");
}

/*****************************************************************************
 * Set up the state vector with the initial predicates.
 *****************************************************************************/
void hpnSetInitialPreds(dMarkingADT marking,hpnIneqListADT hpnIneqL,
                        eventADT *events)
{
  for(int i=0;i < hpnIneqL->ineqCnt;i++) {
    ineqADT ineq = hpnIneqL->l[i]->ineq;
    if(ineq->signal >= 0) {
      if(hpnIneqL->l[i]->ineq->type == 0) {
        if(events[ineq->place]->lower > ineq->constant) {
          marking->state[ineq->signal] = '1';
        }
        else if(events[ineq->place]->upper > ineq->constant) {
#ifdef __HPN_WARN__
          cSetFg(RED);
          printf("WARNING: ");
          cSetAttr(NONE);
          printf("an initial bound is straddling a predicate.  This zone should be split, but will be marked and true for now.\n");
#endif
          marking->state[ineq->signal] = '1';               
        }
        else {
          marking->state[ineq->signal] = '0';
        }
      }
      else if(hpnIneqL->l[i]->ineq->type == 1) {
        if(events[ineq->place]->lower >= ineq->constant) {
          marking->state[ineq->signal] = '1';
        }
        else if(events[ineq->place]->upper >= ineq->constant) {
#ifdef __HPN_WARN__
          cSetFg(RED);
          printf("WARNING: ");
          cSetAttr(NONE);
          printf("an initial bound is straddling a predicate.  This zone should be split, but will be marked and true for now.\n");
#endif
          marking->state[ineq->signal] = '1';               
        }
        else {
          marking->state[ineq->signal] = '0';
        } 
      }
      else if(hpnIneqL->l[i]->ineq->type == 2) {
        if(events[ineq->place]->upper < ineq->constant) {
          marking->state[ineq->signal] = '1';
        }
        else if(events[ineq->place]->lower < ineq->constant) {
#ifdef __HPN_WARN__
          cSetFg(RED);
          printf("WARNING: ");
          cSetAttr(NONE);
          printf("an initial bound is straddling a predicate.  This zone should be split, but will be marked and true for now.\n");
#endif
          marking->state[ineq->signal] = '1';               
        }
        else {
          marking->state[ineq->signal] = '0';
        }
      }
      else if(hpnIneqL->l[i]->ineq->type == 3) {
        if(events[ineq->place]->upper <= ineq->constant) {
          marking->state[ineq->signal] = '1';
        }
        else if(events[ineq->place]->lower < ineq->constant) {
#ifdef __HPN_WARN__
          cSetFg(RED);
          printf("WARNING: ");
          cSetAttr(NONE);
          printf("an initial bound is straddling a predicate.  This zone should be split, but will be marked and true for now.\n");
#endif
          marking->state[ineq->signal] = '1';               
        }
        else {
          marking->state[ineq->signal] = '0';
        }
      }
      else if(hpnIneqL->l[i]->ineq->type == 4) {
        if(events[ineq->place]->upper == ineq->constant) {
          if(events[ineq->place]->lower == ineq->constant) {
            marking->state[ineq->signal] = '1';               
          }
          else {
#ifdef __HPN_WARN__
            cSetFg(RED);
            printf("WARNING: ");
            cSetAttr(NONE);
            printf("an initial bound is straddling a predicate.  This zone should be split, but will be marked and true for now.\n");
#endif
          }
        }
        else {
          marking->state[ineq->signal] = '0';
        }
      }
      else {
        marking->state[ineq->signal] = '0';
      }
    }  
  }
#ifdef __HPN_PRED_DEBUG__
  printf("After hpnSetInitPreds...\n");
  for(int i=0;i < hpnIneqL->ineqCnt;i++)
    if (hpnIneqL->l[i]->ineq->signal >= 0) {
      printIneq(hpnIneqL->l[i]->ineq,events);
      printf(" - %c\n",marking->state[hpnIneqL->l[i]->ineq->signal]);
    }
#endif
}

/*****************************************************************************
 * Add a clock to the set of introduced clocks.
 *****************************************************************************/
void addIntroClocks(int p,int t,cStateADT cState,ruleADT **rules)
{
  cState->introClocks[rules[p][t]->marking] = 'T';
}

/*****************************************************************************
 * Resets the correct clock introduction variables for the given
 * place.  p is an event based index.
 *****************************************************************************/
void resetIntroClocks(int p,cStateADT cState,ruleADT **rules,int nevents,
                      hpnZoneADT zone)
{
#ifdef __HPN_INTROVECTOR__
  cSetFg(YELLOW);
  printf("Resetting introClock[%d]",p);
  cSetAttr(NONE);
  printf("\n");
#endif
  for(int i=1;i<nevents;i++) {
    if(rules[p][i]->ruletype > NORULE &&
       (!isMemberDBM(p,i,zone) && !isMemberCf(p,i,cState,rules))) {
      cState->introClocks[rules[p][i]->marking] = 'F';
    }
  }
}

/*****************************************************************************
  * Returns true if given place is a continuous place.  p is an event
 * based index.
 *****************************************************************************/
bool isVar(int p,eventADT *events)
{
  if(events[p]->type & VAR) {
    return true;
  }
  return false;
}

/******************************************************************************
 * Returns true if given place is a continuous place.  p is an event
 * based index.
 *****************************************************************************/
bool isMemberPc(int p,eventADT *events)
{
  if(events[p]->type & CONT) {
    return true;
  }
  return false;
}

/*****************************************************************************
 * Returns true if the given rule is found in the fired clocks array.  p
 * and t are event based indices.
 *****************************************************************************/
bool isMemberCf(int p,int t,cStateADT cState,ruleADT **rules)
{
  if(cState->firedClocks[rules[p][t]->marking] == 'T') {
#ifdef __HPN_DEBUG__
    printf("In cf.\n");
#endif
    return true;
  }
#ifdef __HPN_DEBUG__
  printf("NOT in cf %d->%d.\n",p,t);
#endif
  return false;
}

/*****************************************************************************
 * Add a fired clock to the array of firedClocks.
 *****************************************************************************/
void addCf(int enabling,int enabled,cStateADT cState,ruleADT **rules)
{
  cState->firedClocks[rules[enabling][enabled]->marking] = 'T';
}

/*****************************************************************************
 * Remove a clock from the firedClocks array.
 *****************************************************************************/
void rmCf(int enabling,int enabled,cStateADT cState,ruleADT **rules)
{
  cState->firedClocks[rules[enabling][enabled]->marking] = 'F';
}

/*****************************************************************************
 * Update max advance based on invariants.
 *****************************************************************************/
int checkInvariant(int min,eventADT *events,int nevents,int nplaces,
                   dMarkingADT marking,markkeyADT *markkey,cStateADT cState,
                   ruleADT **rules,int nrules,hpnZoneADT zone,int i)
{
  for(int j=nevents;j<nevents+nplaces;j++) {
    for(ineqADT ineq = events[j]->inequalities;ineq != NULL;
        ineq = ineq->next) {
      /* if the invariant might affect the place in question then check
         it */
      int warpVal = chkDiv(ineq->constant,
                           cState->warp[zone->curClocks[i].enabled-nevents]
                           ,'C');

      if(ineq->place == zone->curClocks[i].enabled) {
#ifdef __HPN_INVARIANT_DEBUG__
        printf("Ineq: ");
        printIneq(ineq,events);
        printf(" warpVal: %d min: %d\n",warpVal,min);
        printf("\n");
#endif
        if(cState->warp[zone->curClocks[i].enabled-nevents] > 0) {
          if(ineq->type >= 2 && ineq->type <= 4) {
            if(min > warpVal) {
#ifdef __HPN_INVARIANT_DEBUG__
              printf("\n1a. Possible invariant change: %s\n",
                     events[ineq->place]->event);
#endif
              for(int k=0;k<nrules;k++) {
                if(marking->marking[k] == 'T') {
                  if(markkey[k]->enabling == j) {
#ifdef __HPN_INVARIANT_DEBUG__
                    printf("\nInvariants changed min. %d -> %d\n",
                           min,warpVal);
#endif
                    min = warpVal;
                    break;
                  }
                }
              }
            }
          }
/*           else if (ineq->type < 2) { */
/*             if(min < warpVal) { */
/* #ifdef __HPN_INVARIANT_DEBUG__ */
/*               printf("\n1b. Possible invariant change: %s\n", */
/*                      events[ineq->place]->event); */
/* #endif */
/*               for(int k=0;k<nrules;k++) { */
/*                 if(marking->marking[k] == 'T') { */
/*                   if(markkey[k]->enabling == j) { */
/* #ifdef __HPN_INVARIANT_DEBUG__ */
/*                     printf("\nInvariants changed min. %d -> %d\n", */
/*                            min,warpVal); */
/* #endif */
/*                     min = warpVal; */
/*                     break; */
/*                   } */
/*                 } */
/*               } */
/*             } */
/*           } */
        }
        else if(cState->warp[zone->curClocks[i].enabled-nevents] < 0) {
          if(ineq->type <= 1 || ineq->type == 4) {
            if(min > warpVal) {
#ifdef __HPN_INVARIANT_DEBUG__
              printf("\n2a. Possible invariant change: %s\n",
                     events[ineq->place]->event);
#endif
              for(int k=0;k<nrules;k++) {
                if(marking->marking[k] == 'T') {
                  if(markkey[k]->enabling == j) {
#ifdef __HPN_INVARIANT_DEBUG__
                    printf("\nInvariants changed min. %d -> %d\n",
                           min,warpVal);
#endif
                    min = warpVal;
                    break;
                  }
                }
              }
            }
          }
/*           else if(ineq->type >= 2 && ineq->type < 4) { */
/*             if(min < warpVal) { */
/* #ifdef __HPN_INVARIANT_DEBUG__ */
/*               printf("\n2b. Possible invariant change: %s\n", */
/*                      events[ineq->place]->event); */
/* #endif */
/*               for(int k=0;k<nrules;k++) { */
/*                 if(marking->marking[k] == 'T') { */
/*                   if(markkey[k]->enabling == j) { */
/* #ifdef __HPN_INVARIANT_DEBUG__ */
/*                     printf("\nInvariants changed min. %d -> %d\n", */
/*                            min,warpVal); */
/* #endif */
/*                     min = warpVal; */
/*                     break; */
/*                   } */
/*                 } */
/*               } */
/*             } */
/*           } */
        }
        else {
          cSetFg(RED);
          printf("WARNING: ");
          cSetAttr(NONE);
          printf("variable in the zone with a 0 warp.  Not checked for invariant validy.\n");
        }
      }
    }
  }
  return min;
}

/*****************************************************************************
 * Update max advance based on predicate values.
 *****************************************************************************/
int checkPreds(int min,int p,int zoneP,hpnIneqListADT hpnIneqL,hpnZoneADT zone,
               cStateADT cState,ruleADT **rules,int nevents,eventADT *events,
               dMarkingADT marking)
{
#ifdef __HPN_PRED_DEBUG__
  printf("Min entering checkPreds for %s: %d\n",events[p]->event,min);
  printZone(zone,events,cState,nevents);
#endif
  int newMin = min;
  for(int i=0;i<hpnIneqL->ineqCnt;i++) {
    if (hpnIneqL->l[i]->ineq->type > 4) continue;
#ifdef __HPN_PRED_DEBUG__
    printf("Checking ...");
    printIneq(hpnIneqL->l[i]->ineq,events);
    printf("\n");
#endif
    if(hpnIneqL->l[i]->ineq->place == p) {
      /* There is an inequality on the place being advanced right
         now. */
      if(hpnIneqL->l[i]->ineq->type <= 1) {
        /* Working on a > or >= ineq */
        if(cState->warp[p-nevents] > 0) {
          if(marking->state[hpnIneqL->l[i]->ineq->signal]=='1') {
            if(zone->matrix[zoneP][0] <
               chkDiv(hpnIneqL->l[i]->ineq->constant,
                      cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 1a\n");
#endif
              
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);
              printf("checkPreds: Impossible case 1.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else if((-1)*zone->matrix[0][zoneP] >
                    chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 2a\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              cState->warp[p-nevents],'F');
            }
            else {
              /* straddle case */
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 3a\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              cState->warp[p-nevents],'F');
            }
          }
          else {
            if(zone->matrix[zoneP][0] <
               chkDiv(hpnIneqL->l[i]->ineq->constant,
                      cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 4a -- min: %d\n",chkDiv(hpnIneqL->l[i]->ineq->constant,cState->warp[p-nevents],'F'));
#endif
              newMin = chkDiv(hpnIneqL->l[i]->ineq->constant,
                              cState->warp[p-nevents],'F');
            }
            else if((-1)*zone->matrix[0][zoneP] >
                    chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 5a\n");
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);              
              printf("checkPreds: Impossible case 3.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 6a -- min: %d\n",zone->matrix[zoneP][0]);
#endif
              /* straddle case */
              newMin = zone->matrix[zoneP][0];
            }
          }
        }
        else {
          /* warp <= 0 */
          if(marking->state[hpnIneqL->l[i]->ineq->signal]=='1') {
            if(zone->matrix[0][zoneP] <
               (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 7a\n");
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);              
              printf("checkPreds: Impossible case 2.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else if((-1)*zone->matrix[zoneP][0] >
                    (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                                cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 8a\n");
#endif
              newMin = chkDiv(hpnIneqL->l[i]->ineq->constant,
                              cState->warp[p-nevents],'F');
            }
            else {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 9a\n");
#endif
              /* straddle case */
              newMin = zone->matrix[zoneP][0];
            }
          }
          else {
            if(zone->matrix[0][zoneP] <
               (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 10a\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              cState->warp[p-nevents],'F');
            }
            else if((-1)*zone->matrix[zoneP][0] >
                    (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                                cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 11a\n");
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);              
              printf("checkPreds: Impossible case 4.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else {
              /* straddle case */
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 12a\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              cState->warp[p-nevents],'F');
            }
          }
        }
      }
      else {
        /* Working on a < or <= ineq */
        if(cState->warp[p-nevents] > 0) {
          if(marking->state[hpnIneqL->l[i]->ineq->signal]=='1') {
            if(zone->matrix[zoneP][0] <
               chkDiv(hpnIneqL->l[i]->ineq->constant,
                      cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 1b -- min: %d\n",chkDiv(hpnIneqL->l[i]->ineq->constant,cState->warp[p-nevents],'F'));
#endif
              newMin = chkDiv(hpnIneqL->l[i]->ineq->constant,
                              cState->warp[p-nevents],'F');
            }
            else if((-1)*zone->matrix[0][zoneP] >
                    chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 2b\n");
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);
              printf("checkPreds: Impossible case 5.\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              cState->warp[p-nevents],'F');
            }
            else {
              /* straddle case */
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 3b -- min: %d\n",zone->matrix[zoneP][0]);
#endif
              newMin = zone->matrix[zoneP][0];
            }
          }
          else {
            if(zone->matrix[zoneP][0] <
               chkDiv(hpnIneqL->l[i]->ineq->constant,
                      cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 4b\n");
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);
              printf("checkPreds: Impossible case 7.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else if((-1)*zone->matrix[0][zoneP] >
                    chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 5b\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              cState->warp[p-nevents],'F');
            }
            else {
              /* straddle case */
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 6b\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              cState->warp[p-nevents],'F');
            }
          }
        }
        else {
          /* warp <= 0 */
          if(marking->state[hpnIneqL->l[i]->ineq->signal]=='1') {
            if(zone->matrix[0][zoneP] <
               (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 7b\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              cState->warp[p-nevents],'F');
            }
            else if((-1)*zone->matrix[zoneP][0] >
                    (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                                cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 8b\n");
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);
              printf("checkPreds: Impossible case 8.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else {
              /* straddle case */
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 9b\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              cState->warp[p-nevents],'F');
            }
          }
          else {
            if(zone->matrix[0][zoneP] <
               chkDiv((-1)*hpnIneqL->l[i]->ineq->constant,
                           cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 10b\n");
              printf("zone: %d const: %d warp: %d chkDiv: %d\n",zone->matrix[0][zoneP],hpnIneqL->l[i]->ineq->constant,cState->warp[p-nevents],chkDiv((-1)*hpnIneqL->l[i]->ineq->constant,cState->warp[p-nevents],'F'));
#endif
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);
              printf("checkPreds: Impossible case 6.\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
            else if((-1)*zone->matrix[zoneP][0] >
                    (-1)*chkDiv(hpnIneqL->l[i]->ineq->constant,
                                cState->warp[p-nevents],'F')) {
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 11b\n");
#endif
              newMin = chkDiv(hpnIneqL->l[i]->ineq->constant,
                              cState->warp[p-nevents],'F');
            }
            else {
              /* straddle case */
#ifdef __HPN_PRED_DEBUG__
              printf("CP:case 12b\n");
#endif
              newMin = zone->matrix[zoneP][0];
            }
          }
        }
      }
    }
    if(newMin < min) {
      min = newMin;
    }
  }

#ifdef __HPN_PRED_DEBUG__
  printf("Min leaving checkPreds for %s: %d\n",events[p]->event,min);
#endif
  return min;
}

/*****************************************************************************
 * Determines the maximum possible value for the given continuous
 * variable p.  p and t are event based indices.
 *****************************************************************************/
int maxContVar(int p,int t,int zoneP,hpnZoneADT zone,cStateADT cState,
               ruleADT **rules,int nevents,eventADT *events)
{
  /* allow special case for pll rules to be introduced */
  if((rules[p][t]->plower == 0) &&
     (rules[p][t]->pupper == 0) &&
     (!isMemberDBM(p,t,zone)) &&
     (!isMemberCf(p,t,cState,rules)) &&
     (cState->warp[p-nevents] > 0)) {
    return INFIN;
  }
  
  int blw = chkDiv(rules[p][t]->plower,cState->warp[p-nevents],'F');
  int buw = chkDiv(rules[p][t]->pupper,cState->warp[p-nevents],'F');
  
#ifdef __HPN_MAXCONTVAR__
  printf("blw: %d -- buw: %d -- warp: %d\n",blw,buw,cState->warp[p-nevents]);
  if(rules[p][t]->pupper == INFIN) {
    printf("plower: %d -- pupper: INFIN\n",rules[p][t]->plower);
  }
  else {
    printf("plower: %d -- pupper: %d\n",rules[p][t]->plower,
           rules[p][t]->pupper);
  }
  if(zone->matrix[zoneP][0] == INFIN) {
    printf("zone[p][p0]: INFIN\n");
  }
  else {
    printf("zone[p][p0]: %d -- zone[p0][p]: %d\n",
           zone->matrix[zoneP][0],zone->matrix[0][zoneP]);
  }
#endif
  
  if(cState->warp[p-nevents] > 0) {
    if(!isMemberDBM(p,t,zone) &&
       !isMemberCf(p,t,cState,rules)) {
      if(zone->matrix[zoneP][0] < blw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 1:[%d].\n",blw);
#endif
        return blw;
      }
      else if(zone->matrix[0][zoneP] * -1 <= blw &&
              zone->matrix[zoneP][0] >= blw &&
              (zone->matrix[zoneP][0] < buw ||
               cState->introClocks[rules[p][t]->marking]=='F')) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 2:[%d].\n",zone->matrix[zoneP][0]);
#endif
        return zone->matrix[zoneP][0];
      }
      else if(zone->matrix[0][zoneP] * -1 <= blw &&
              zone->matrix[zoneP][0] >= blw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 2b:[%d] - urange.\n",
               chkDiv(events[p]->urange,cState->warp[p-nevents],'C'));
#endif
        return chkDiv(events[p]->urange,cState->warp[p-nevents],'C');
      }
      else if(zone->matrix[0][zoneP] * -1 > blw &&
              zone->matrix[zoneP][0] < buw) {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("impossible case 1 in advanceTime.\n");
#endif
        return zone->matrix[zoneP][0];
      }
      else {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 3:[%d] - urange.\n",
               chkDiv(events[p]->urange,cState->warp[p-nevents],'C'));
#endif
        return chkDiv(events[p]->urange,cState->warp[p-nevents],'C');
      }
    }
    else {
      if(zone->matrix[zoneP][0] < blw) {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("impossible case 2 in advanceTime.\n");
#endif
        return zone->matrix[zoneP][0];
      }
      else if(zone->matrix[0][zoneP] * -1 <= buw &&
              zone->matrix[zoneP][0] >= buw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 6:[%d].\n",zone->matrix[zoneP][0]);
#endif
        return zone->matrix[zoneP][0];        
      }
      else if(zone->matrix[0][zoneP] * -1 > blw &&
              zone->matrix[zoneP][0] < buw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 5:[%d].\n",buw);
#endif
        return buw;
      }
      else if(zone->matrix[0][zoneP] * -1 <= blw &&
              zone->matrix[zoneP][0] >= blw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 4:[%d].\n",buw);
#endif
        return buw;
      }
      else {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("impossible case 3 in advanceTime.\n");
#endif
        return zone->matrix[zoneP][0];
      }
    }
  }
  else if(cState->warp[p-nevents] < 0) {
    if(!isMemberDBM(p,t,zone) &&
       !isMemberCf(p,t,cState,rules)) {
      if(zone->matrix[zoneP][0] < buw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 7:[%d].\n",buw);
#endif
        return buw;
      }
      else if(zone->matrix[zoneP][0] >= buw &&
              zone->matrix[0][zoneP] * -1 <= buw &&
              (zone->matrix[zoneP][0] < blw ||
               cState->introClocks[rules[p][t]->marking]=='F')) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 8:[%d].\n",zone->matrix[zoneP][0]);
#endif
        return zone->matrix[zoneP][0];
      }
      else if(zone->matrix[zoneP][0] >= buw &&
              zone->matrix[0][zoneP] * -1 <= buw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 8b:[%d] - lrange.\n",1 *
               chkDiv(events[p]->lrange,cState->warp[p-nevents],'C'));
#endif
        return chkDiv(events[p]->lrange,cState->warp[p-nevents],'C');
      }
      else if(zone->matrix[zoneP][0] < blw &&
              zone->matrix[0][zoneP] * -1 > buw) {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("impossible case 4 in advanceTime.\n");
#endif
        return zone->matrix[zoneP][0];
      }
      else {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 9:[%d] - lrange.\n",1 *
               chkDiv(events[p]->lrange,cState->warp[p-nevents],'C'));
#endif
        return chkDiv(events[p]->lrange,cState->warp[p-nevents],'C');
      }
    }
    else {
      if(zone->matrix[zoneP][0] < buw) {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("impossible case 5 in advanceTime.\n");
#endif
        return zone->matrix[zoneP][0];
      }
      else if(zone->matrix[zoneP][0] >= blw &&
              zone->matrix[0][zoneP] * -1 <= blw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 12:[%d].\n",zone->matrix[zoneP][0]);
#endif
        return zone->matrix[zoneP][0];        
      }
      else if(zone->matrix[zoneP][0] < blw &&
              zone->matrix[0][zoneP] * -1 > buw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 11:[%d].\n",blw);
#endif
        return blw;
      }
      else if(zone->matrix[zoneP][0] >= buw &&
              zone->matrix[0][zoneP] * -1 <= buw) {
#ifdef __HPN_MAXCONTVAR__
        printf("advTime-Return 10:[%d].\n",zone->matrix[zoneP][0]);
#endif
        return blw;
      }
      else {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("impossible case 6 in advanceTime.\n");
#endif
        return zone->matrix[zoneP][0];
      }
    }
  }
  return 0;
}

/*****************************************************************************
 * Determines how time will advance.
 *****************************************************************************/
void advanceTime(hpnZoneADT zone,cStateADT cState,ruleADT **rules,
                 eventADT *events,int nevents,char *arcType,int nplaces,
                 int nrules,markkeyADT *markkey,dMarkingADT marking,
                 hpnIneqListADT hpnIneqL,int nsignals,signalADT *signals)
{
#ifdef __HPN_TRACE__
  printf("advanceTime():Start\n");
#endif

  /* set all rules to their upper bound */
  for(int i=1;i<zone->dbmEnd;i++) {
    /* find all (p,t) which is essentially everything in the zone that
       isn't a continuous place */
    if(zone->curClocks[i].enabling != -1) {
      zone->matrix[i][0] = rules[zone->curClocks[i].enabling]
        [zone->curClocks[i].enabled]->upper;
#ifdef __HPN_ADVTIME__
      printf("Upper for %s->%s is %d\n",
             events[zone->curClocks[i].enabling]->event,
             events[zone->curClocks[i].enabled]->event,
             rules[zone->curClocks[i].enabling]
             [zone->curClocks[i].enabled]->upper);
#endif
    }
  }

#ifdef __HPN_ADVTIME__
  printf("Zone entering advanceTime.\n");
  printZone(zone,events,cState,nevents);
  printConciseMarkingADT(marking,nevents,nrules,nsignals,signals,events,
                         markkey);
#endif
  
  for(int i=1;i<zone->dbmEnd;i++) {
    bool first = true;
    int min = 0;
    int newMin = 0;
#ifdef __HPN_ADVTIME__
    printf("Checking %d->%d\n",zone->curClocks[i].enabling,zone->curClocks[i].enabled);
#endif
    if(zone->curClocks[i].enabling == -1) {
      if(cState->warp[zone->curClocks[i].enabled-nevents] < 0) {
#ifdef __HPN_ADVTIME__
        printf("Setting min initially: %d.\n",1 *
               chkDiv(events[zone->curClocks[i].enabled]->lrange,
                      cState->warp[zone->curClocks[i].enabled-nevents],'C'));
#endif
        min = chkDiv(events[zone->curClocks[i].enabled]->lrange,
                     cState->warp[zone->curClocks[i].enabled-nevents],'C');
      }
      else {
#ifdef __HPN_ADVTIME__
        printf("Setting min initially: %d.\n",
               chkDiv(events[zone->curClocks[i].enabled]->urange,
                      cState->warp[zone->curClocks[i].enabled-nevents],'C'));
#endif 
        min = chkDiv(events[zone->curClocks[i].enabled]->urange,
                     cState->warp[zone->curClocks[i].enabled-nevents],'C');
      }
      for(int j=1;j<nevents;j++) {
        if(rules[zone->curClocks[i].enabled][j]->ruletype > NORULE &&
           (arcType[rules[zone->curClocks[i].enabled][j]->marking] == 'P' ||
            arcType[rules[zone->curClocks[i].enabled][j]->marking] == 'H')) {
#ifdef __HPN_MAXCONTVAR__
          printf("checking: rules[%s][%s]\n",
                 events[zone->curClocks[i].enabled]->event,events[j]->event);
#endif
          if(!first) {
            newMin = maxContVar(zone->curClocks[i].enabled,j,i,zone,cState,
                                rules,nevents,events);
            if(newMin < min) {
              min = newMin;
            }
          }
          else {
            min = maxContVar(zone->curClocks[i].enabled,j,i,zone,cState,
                             rules,nevents,events);
            first = false;
          }
        }
        
/* #ifdef __HPN_ADVTIME__ */
/*         printf("Min: %d.\n",min); */
/* #endif */
      }

      min = checkPreds(min,zone->curClocks[i].enabled,i,hpnIneqL,zone,cState,
                       rules,nevents,events,marking);
      
      /* This probably isn't the best place to add in the invariant
         enforcing code to advance time, but it should work. */
#ifdef __HPN_ADVTIME__
      if(min == INFIN) {
        printf("Min for %s is U [pre-invariant check]\n\n",
               events[zone->curClocks[i].enabled]->event);
      }
      else {  
        printf("Min for %s is %d [pre-invariant check]\n\n",
               events[zone->curClocks[i].enabled]->event,min);
      }
#endif
      
      min = checkInvariant(min,events,nevents,nplaces,marking,markkey,cState,
                           rules,nrules,zone,i);
      
#ifdef __HPN_ADVTIME__
      if(min == INFIN) {
        printf("Min for %s is U [post-invariant check]\n\n",
               events[zone->curClocks[i].enabled]->event);
      }
      else {  
        printf("Min for %s is %d [post-invariant check]\n\n",
               events[zone->curClocks[i].enabled]->event,min);
      } 
#endif
      zone->matrix[i][0] = min;
    }
  }
#ifdef __HPN_ADVTIME__
  printf("Zone leaving advanceTime.\n");
  printZone(zone,events,cState,nevents);
#endif
#ifdef __HPN_TRACE__
  printf("advanceTime():End\n");
#endif
}

/*****************************************************************************
 * Find the initial zone.
 *****************************************************************************/
hpnZoneADT hpnFindInitialZone(ruleADT **rules,markkeyADT *markkey,
                              eventADT *events,dMarkingADT marking,
                              cStateADT cState,int nrules,int nevents,
                              int nplaces,char *arcType,signalADT *signals,
                              hpnIneqListADT hpnIneqL,int nsignals)
{
#ifdef __HPN_TRACE__
  printf("hpnFindInitialZone():Start\n");
#endif
  
  hpnBounds clk;
  hpnZoneADT zone = createZone(1);

  /* add the dummy timer to the zone */
  zone->curClocks[0].enabled = 0;
  zone->curClocks[0].enabling = 0;

  /* add the continuos variables to the zone */
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(isMemberPc(i,events)) {
      clk.lower = events[i]->lower;
      clk.upper = events[i]->upper;
      dbmAdd(&zone,-1,i,clk,cState,rules,nevents,nplaces);
    }
  }

#ifdef __HPN_STATE_INFO__
  printf("After variable addition.\n");
  printZone(zone,events,cState,nevents);
  printFiredClocks(cState,markkey,events,nrules);
#endif
  
  clk.upper = 0;
  clk.lower = 0;
  
  /* find the enabled clocks (p,t) and add them to the zone */
  for(int i=0;i<nrules;i++) {
#ifdef __HPN_INTROVECTOR__
    printf("enabling:%s:%d - enabled:%s:%d - nevents:%d - arcType:%c - marking:%c",events[markkey[i]->enabling]->event,markkey[i]->enabling,
           events[markkey[i]->enabled]->event,markkey[i]->enabled,nevents,
           arcType[i],marking->marking[i]);
    if(enabledXition(markkey[i]->enabled,events,cState,marking,zone,nevents,
                     rules,signals,nsignals,nplaces)) {
      printf(" - enabledXition: T\n");
    }
    else {
      printf(" - enabledXition: T\n");
    }
    
#endif
    if(markkey[i]->enabling >= nevents &&
       arcType[i] == 'D' &&
       marking->marking[i] == 'T' &&
       enabledXition(markkey[i]->enabled,events,cState,marking,zone,
                     nevents,rules,signals,nsignals,nplaces)) {
      dbmAdd(&zone,markkey[i]->enabling,markkey[i]->enabled,clk,
             cState,rules,nevents,nplaces);
#ifdef __HPN_INTROVECTOR__
      cSetFg(YELLOW);
      printf("Adding introClock: ");
      cSetAttr(NONE);
      printf("%s->%s\n",events[markkey[i]->enabling]->event,
             events[markkey[i]->enabled]->event);
#endif
      addIntroClocks(markkey[i]->enabling,markkey[i]->enabled,cState,rules);
    }
    else if(markkey[i]->enabling >= nevents &&
            arcType[i] == 'P') {
      if(rules[markkey[i]->enabling][markkey[i]->enabled]->plower
         <=  rules[markkey[i]->enabling][markkey[i]->enabled]->epsilon &&
         rules[markkey[i]->enabling][markkey[i]->enabled]->pupper
         >= rules[markkey[i]->enabling][markkey[i]->enabled]->epsilon) {
        dbmAdd(&zone,markkey[i]->enabling,markkey[i]->enabled,clk,
               cState,rules,nevents,nplaces);
#ifdef __HPN_INTROVECTOR__
        cSetFg(YELLOW);
        printf("Adding introClock: ");
        cSetAttr(NONE);
        printf("%s->%s\n",events[markkey[i]->enabling]->event,
               events[markkey[i]->enabled]->event);
#endif
        addIntroClocks(markkey[i]->enabling,markkey[i]->enabled,cState,rules);
      }
    }
  }


  /* remove the inactive places */
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(isVar(i,events)) {
      if(events[i]->linitrate == 0) {
        dbmRemove(&zone,i);
      }
    } else {
      if(xdot(i,events,rules,marking,zone,cState,nevents,nplaces,signals,
              nsignals) == 0) {
        dbmRemove(&zone,i);
      }
    }
  }
  
  recanonZone(zone);
#ifdef __HPN_STATE_INFO__
  printf("Before warp.\n");
  printZone(zone,events,cState,nevents);
  printFiredClocks(cState,markkey,events,nrules);
#endif
  /* fill the newWarp array w/ the updated warp values */
  int* newWarp = (int*)GetBlock(sizeof(int) * nplaces);
  for(int i=nevents;i<nevents+nplaces;i++) {
    if (isVar(i,events)) {
      newWarp[i-nevents] = events[i]->linitrate;
    } else {
      newWarp[i-nevents] = xdot(i,events,rules,marking,zone,cState,
				nevents,nplaces,signals,nsignals);
    }
  }
#ifdef __HPN_DEBUG_WARP__
  printf("Warps in hpnFindInitialZone.\n");
  for(int i=nevents;i<nevents+nplaces;i++) {
    printf("%s:%d\n",events[i]->event,newWarp[i-nevents]);
  }
#endif
  dbmWarp(&zone,cState,events,rules,marking,nevents,nplaces,signals,nsignals,
          newWarp);
#ifdef __HPN_STATE_INFO__
  printf("After warp.\n");
  printZone(zone,events,cState,nevents);
#endif
  advanceTime(zone,cState,rules,events,nevents,arcType,nplaces,nrules,markkey,
              marking,hpnIneqL,nsignals,signals);
#ifdef __HPN_STATE_INFO__
  printf("After advanceTime().\n");
  printZone(zone,events);
#endif
  recanonZone(zone);
  checkConsistentZone(zone,events);
  
#ifdef __HPN_DEBUG__
  printf("The initial zone.\n");
  printZone(zone,events);
#endif

#ifdef __HPN_TRACE__
  printf("hpnFindInitialZone():End\n");
#endif
  return zone;
}

/*****************************************************************************
 * Find all possible actions based on the current state.
 *****************************************************************************/
hpnActionSetADT hpnFindPossibleActions(dMarkingADT marking,hpnZoneADT zone,
                                       cStateADT cState,eventADT *events,
                                       ruleADT **rules,int nevents,
                                       int nplaces,int nrules,
                                       markkeyADT *markkey,char* arcType,
                                       signalADT *signals,
                                       hpnIneqListADT hpnIneqL,int nsignals)
{
#ifdef __HPN_TRACE__
  printf("hpnFindPossibleActions():Start\n");
#endif
#ifdef __HPN_SHOW_POSS_ACTION__
  printf("Zone entering findPossibleActions.\n");
  printZone(zone,events,cState,nevents);
  printFiredClocks(cState,markkey,events,nrules);
#endif

  char func = 'F';
  hpnActionSetADT aSet = createHpnActionSet();
  hpnActionADT A = NULL;
  
  int p,xDot;    
  for(int i=1;i<zone->numClocks;i++) {
    if(zone->curClocks[i].enabling == -1) {
      /* only a place here */
      p = zone->curClocks[i].enabled;
      xDot = xdot(p,events,rules,marking,zone,cState,nevents,nplaces,signals,
                  nsignals);
      /* look to introduce or delete all (p,t) for the given p */
      for(int t=1;t<nevents;t++) {
        if(rules[p][t]->ruletype > NORULE &&
           (arcType[rules[p][t]->marking] == 'P')) {
#ifdef __HPN_IND_ACTION__
          if(xDot > 0) {  
            printf("delCheck: %s->%s xDot: %d pupper: %d\nZone[i][0]:%d chkDivRes:%d\n",
                   events[p]->event,events[t]->event,xDot,
                   rules[p][t]->pupper,zone->matrix[i][0],
                   chkDiv(rules[p][t]->pupper,xDot,func));
          }
          else if(xDot < 0) {  
            printf("delCheck: %s->%s xDot: %d plower: %d\nZone[i][0]:%d chkDivRes:%d\n",
                   events[p]->event,events[t]->event,xDot,
                   rules[p][t]->plower,zone->matrix[i][0],
                   chkDiv(rules[p][t]->plower,xDot,func));
          }
#endif
          if((isMemberDBM(p,t,zone) || isMemberCf(p,t,cState,rules)) &&
             ((rules[p][t]->pupper < events[p]->urange &&
               xDot > 0 &&
               zone->matrix[i][0] >= chkDiv(rules[p][t]->pupper,xDot,func))
              ||
              (rules[p][t]->plower > events[p]->lrange &&
               xDot < 0 &&
               zone->matrix[i][0] >= chkDiv(rules[p][t]->plower,xDot,func))
              )) {
            A = (hpnActionADT)GetBlock(sizeof(*A));
            A->enabling = p;
            A->enabled = t;
            A->type = DEL_CLK;
            A->set = aSet->setCnt++;
            A->cls = 'N';
            A->ineq = NULL;
            addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                             arcType);
          }
          if((!isMemberDBM(p,t,zone) && !isMemberCf(p,t,cState,rules)) &&
             (
              (xDot > 0 &&
               zone->matrix[i][0] >= chkDiv(rules[p][t]->plower,xDot,func) &&
               (zone->matrix[0][i] * -1) <= chkDiv(rules[p][t]->plower,xDot,
                                                   func))
              ||
              (xDot < 0 &&
               zone->matrix[i][0] >= chkDiv(rules[p][t]->pupper,xDot,func) &&
               (zone->matrix[0][i] * -1) <= chkDiv(rules[p][t]->pupper,xDot,
                                                   func))
              ) &&
             cState->introClocks[rules[p][t]->marking] == 'F' &&
             enabledXition(t,events,cState,marking,zone,nevents,rules,signals,
                           nsignals,nplaces)
             ) {
            A = (hpnActionADT)GetBlock(sizeof(*A));
            A->enabling = p;
            A->enabled = t;
            A->type = INTRO_CLK;
            A->set = aSet->setCnt++;
            A->cls = 'N';
            A->ineq = NULL;
            addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                             arcType);
          }
        }
      }
      /* look at making p inactive */
      if(zone->matrix[i][0] >= chkDiv(events[p]->lrange,
                                      cState->warp[p-nevents],'C') &&
         cState->warp[p-nevents] < 0 &&
         i < zone->dbmEnd) {
        A = (hpnActionADT)GetBlock(sizeof(*A));
        A->enabling = -1;
        A->enabled = p;
        A->type = DEL_VAR;
        A->set = aSet->setCnt++;
        A->cls = 'N';
        A->ineq = NULL;
        addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                         arcType);
      }
      if(zone->matrix[i][0] >= chkDiv(events[p]->urange,
                                      cState->warp[p-nevents],'C') &&
         cState->warp[p-nevents] > 0  &&
         i < zone->dbmEnd) {
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
        printf("zone: %d  - chkDiv: %d - warp: %d\n",zone->matrix[i][0],
               chkDiv(events[p]->urange,cState->warp[p-nevents],'C'),
               cState->warp[p-nevents]);
#endif
        A = (hpnActionADT)GetBlock(sizeof(*A));
        A->enabling = -1;
        A->enabled = p;
        A->type = DEL_VAR;
        A->set = aSet->setCnt++;
        A->cls = 'N';
        A->ineq = NULL;
        addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                         arcType);
      }
    }
    else {
      /* this is a (p,t) so handle it appropriately */
      p = zone->curClocks[i].enabling;
      int t = zone->curClocks[i].enabled;

#ifdef __HPN_SHOW_POSS_ACTION_OFF__
      printf("Considering fire clock: %s->%s ",events[p]->event,events[t]->event);
      printf("zone: %d - rules: %d\n",zone->matrix[i][0],rules[p][t]->lower);
#endif
      if(zone->matrix[i][0] >= rules[p][t]->lower) {
        A = (hpnActionADT)GetBlock(sizeof(*A));
        A->enabling = p;
        A->enabled = t;
        A->type = FIRE_CLK;
        A->set = aSet->setCnt++;
        A->cls = 'N';
        A->ineq = NULL;
        addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                         arcType);
      }
    }
  }

  /* check for predicates that need to be introduced or deleted */
  for(int i=0;i<hpnIneqL->ineqCnt;i++) {
    hpnIneqADT h = hpnIneqL->l[i];
    if(h->ineq->type <= 4 &&
       events[h->ineq->place]->type & CONT) {
      int zoneP = getZoneIndex(zone,-1,h->ineq->place);
      if(h->ineq->type > 1 && h->ineq->type <= 4) {
        /* p <= c */
        /* intro pred */
#ifdef __HPN_PRED_DEBUG__
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
      printf("Considering intro pred: ");
      printIneq(h->ineq,events);
      printf(" - state: %c - val: %d <= %d",marking->state[h->ineq->signal],-1 * zone->matrix[zoneP][0],(-1)*chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F'));
      printf(" - signal: %d",h->ineq->signal);
      printf("\n");
#endif
#endif      
        if(cState->warp[h->ineq->place-nevents] < 0 &&
           marking->state[h->ineq->signal] == '0') {
          if(((-1)*zone->matrix[zoneP][0]) <= (-1)*(chkDiv((h->ineq->constant),cState->warp[h->ineq->place-nevents],'F'))) {
            A = (hpnActionADT)GetBlock(sizeof(*A));
            A->enabling = h->ineq->place;
            A->enabled = h->ineq->place;
            A->type = INTRO_PRED;
            A->set = aSet->setCnt++;
            A->cls = 'N';
            A->ineq = h->ineq;
            addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                             arcType);
          }
        }
        /* del pred */
#ifdef __HPN_PRED_DEBUG__
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
      printf("Considering del pred: ");
      printIneq(h->ineq,events);
      printf(" - state: %c - val: %d <= %d",marking->state[h->ineq->signal],zone->matrix[zoneP][0],chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F'));
      printf(" - signal: %d",h->ineq->signal);
      printf("\n");
#endif
#endif      
        if(cState->warp[h->ineq->place-nevents] > 0 &&
           marking->state[h->ineq->signal] == '1') {
          if(zone->matrix[zoneP][0] >= chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F')) {

#ifdef __HPN_SHOW_POSS_ACTION_OFF__
            printf("del_pred <= possible: %d >= %d\n",chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F'),zone->matrix[zoneP][0]);
#endif 
            A = (hpnActionADT)GetBlock(sizeof(*A));
            A->enabling = h->ineq->place;
            A->enabled = h->ineq->place;
            A->type = DEL_PRED;
            A->set = aSet->setCnt++;
            A->cls = 'N';
            A->ineq = h->ineq;
            addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                             arcType);
          }
        }
      }
      else {
        /* p >= c */
        /* intro pred */
#ifdef __HPN_PRED_DEBUG__        
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
        printf("Considering intro pred: ");
        printIneq(h->ineq,events);
        printf(" - state: %c - val: %d >= %d",marking->state[h->ineq->signal],zone->matrix[zoneP][0],chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F'));
        printf(" - signal: %d",h->ineq->signal);
        printf("\n");
#endif
#endif        
        if(cState->warp[h->ineq->place-nevents] > 0 &&
           marking->state[h->ineq->signal] == '0') {
          if(zone->matrix[zoneP][0] >= chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F')) {
            A = (hpnActionADT)GetBlock(sizeof(*A));
            A->enabling = h->ineq->place;
            A->enabled = h->ineq->place;
            A->type = INTRO_PRED;
            A->set = aSet->setCnt++;
            A->cls = 'N';
            A->ineq = h->ineq;
            addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                             arcType);
          }
        }
        /* del pred */
#ifdef __HPN_PRED_DEBUG__        
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
        printf("Considering del pred: ");
        printIneq(h->ineq,events);
        printf(" - state: %c - val: %d >= %d",marking->state[h->ineq->signal],-1*zone->matrix[zoneP][0],(-1)*chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F'));
        printf(" - signal: %d",h->ineq->signal);
        printf("\n");
#endif
#endif        
        if(cState->warp[h->ineq->place-nevents] < 0 &&
           marking->state[h->ineq->signal] == '1') {
          if((-1)*zone->matrix[zoneP][0] <= (-1)*chkDiv(h->ineq->constant,cState->warp[h->ineq->place-nevents],'F')) {
            A = (hpnActionADT)GetBlock(sizeof(*A));
            A->enabling = h->ineq->place;
            A->enabled = h->ineq->place;
            A->type = DEL_PRED;
            A->set = aSet->setCnt++;
            A->cls = 'N';
            A->ineq = h->ineq;
            addActionSetItem(aSet,A,rules,cState,nevents,zone,events,nplaces,
                             arcType);
          }
        }
      }
    }
  }
  
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
  cSetFg(MAGENTA);
  printf("POSSIBLE ACTION LIST:\n");
  cSetAttr(NONE);
  printActionSet(aSet,events);
#endif

  /* Rip out delete events where a fire on the same event is also in
     the set. */
  hpnActionSetADT e = createHpnActionSet();
  for(list<hpnActionADT>::iterator i=aSet->l->begin();i != aSet->l->end();
      i++) {
    for(list<hpnActionADT>::iterator j=aSet->l->begin();j != aSet->l->end();
        j++) {
      if((*i)->type != (*j)->type &&
         (*i)->enabling == (*j)->enabling &&
         (*i)->enabled == (*j)->enabled) {
        if((*i)->type == 'D') {
          e->l->push_back(*i);
        }
        else if((*j)->type == 'D') {
          e->l->push_back(*j);
        }
        else {
          if((*i)->ineq == (*j)->ineq) {
#ifdef __HPN_WARN__
            cSetFg(RED);
            printf("WARNING: ");
            cSetAttr(NONE);
            printf("Two events on the same rule in an action set and one is not a delete action.\n");
#endif
          }
        }
      }
    }
    /* Do a final fire check */
    if((*i)->type == 'F' && (*i)->cls == 'F') {
      for(int j=nevents;j<nevents+nplaces;j++) {
        if(rules[j][(*i)->enabled]->ruletype > 0) {
          if(isMemberCf(j,(*i)->enabled,cState,rules) ||
             checkFireFinal(&aSet,j,(*i)->enabled,(*i)->set,events) ||
             j == (*i)->enabling) {
          }
          else {
#ifdef __HPN_SHOW_POSS_ACTION_OFF__
            printf("Removing a firing on %s->%s!\n",
                   events[(*i)->enabling]->event,events[(*i)->enabled]->event);
#endif
            (*i)->cls = 'N';
            break;
          }
        }
      }
    }
  }
  if(!e->l->empty()) {
    for(list<hpnActionADT>::iterator iter=e->l->begin();
        iter!=e->l->end();iter++) {
      aSet->l->remove((*iter));
    }
  }

  freeActionSet(e);
  
#ifdef __HPN_SHOW_POSS_ACTION__
  cSetFg(MAGENTA);
  printf("FINAL POSSIBLE ACTION LIST:\n");
  cSetAttr(NONE);
  printActionSet(aSet,events);
#endif
  
#ifdef __HPN_TRACE__
  printf("hpnFindPossibleActions():End\n");
#endif
  return aSet;
}

/*****************************************************************************
 * A quick check to help test firability.
 *****************************************************************************/
bool checkFire(hpnActionSetADT *A,int enabling,int enabled,eventADT *events)
{
  for(list<hpnActionADT>::iterator iter = (*A)->l->begin();
      iter != (*A)->l->end();iter++) {
    if((*iter)->enabling == enabling &&
       (*iter)->enabled == enabled) {
#ifdef __HPN_ADD_ACTION__
      printf("True b/c of: ");
      printAction((*iter),events);
#endif
      return true;
    }
  }
  return false;
}

/*****************************************************************************
 * A quick check to help test firability.
 *****************************************************************************/
bool checkFireFinal(hpnActionSetADT *A,int enabling,int enabled,int set,
                    eventADT *events)
{
  for(list<hpnActionADT>::iterator iter = (*A)->l->begin();
      iter != (*A)->l->end();iter++) {
    if((*iter)->enabling == enabling &&
       (*iter)->enabled == enabled &&
       (*iter)->set == set) {
#ifdef __HPN_ADD_ACTION__
      printf("True b/c of: ");
      printAction((*iter),events);
#endif
      return true;
    }
  }
  return false;
}

/*****************************************************************************
 * Print a warning message if an output event is disabled.
 *****************************************************************************/
void hpnDisabling(bool disabling,int i,char *event,bool verbose)
{
  if(disabling) {
    if(!disabled[i]) {
      printf("WARNING: Output event %s is disabled\n",event);
      fprintf(lg,"WARNING: Output event %s is disabled\n",event);
      disabled[i]=true;
    }
  }
  else {
    if(!verbose) {
      printf("Error!\n");
      fprintf(lg,"Error!\n");
    }
    printf("Error: Output event %s is disabled\n",event);
    fprintf(lg,"Error: Output event %s is disabled\n",event);
  }
}

/*****************************************************************************
 * Update the current state.
 *****************************************************************************/
bool hpnUpdateState(eventADT *events,ruleADT **rules,markkeyADT *markkey,
                    dMarkingADT marking,int nevents,int nrules,int nsignals,
                    int ninpsig,bool verbose, bool disabling,char *arcType,
                    int nSigs,int t)
{
#ifdef __HPN_TRACE__
  printf("hpnUpdateState():Start\n");
#endif
  char *oldEnablings;

  /* Clear signal enablings */
  for(int i=0;i<nSigs;i++) {  
    if(marking->state[i] == 'R') {
      marking->state[i] = '0';
    }
    else if(marking->state[i] == 'F') {
      marking->state[i] = '1';
    }
  }
  
  /* Find enabled events */
  oldEnablings=CopyString(marking->enablings);
  hpnEventsDiscreteEn(events,rules,markkey,marking,arcType,nrules,nevents);

  /* Update statevector with enablings */
  for(int i=1;i<nevents;i++) {
    if((!(events[i]->dropped)) && (marking->enablings[i] == 'T')) {
      if(events[i]->signal >= 0) {
        /* if the state isn't consistent return an error */
        if(!hpnCheckConsistency(events[i]->event,marking->state,
                                events[i]->signal,ninpsig,verbose)) {
          free(oldEnablings);
#ifdef __HPN_TRACE__
          printf("hpnUpdateState():End\n");
#endif
          return false;
        }
        if(strchr(events[i]->event,'+')) 
          marking->state[events[i]->signal] = 'R';
        else
          marking->state[events[i]->signal] = 'F';
      }
    }
    else {
      /* If there has been a disabling, report it and exit if a !disabling */
      if((events[i]->signal >= ninpsig) && (oldEnablings[i] == 'T') &&
         (i != t)) {
        hpnDisabling(disabling,i,events[i]->event,verbose);
        if(!disabling) {
          free(oldEnablings);
          return false;
        }
      }
    }
  }
  free(oldEnablings);
#ifdef __HPN_TRACE__
  printf("hpnUpdateState():End\n");
#endif
  return true;
}

/*****************************************************************************
 * Do the variable assignments based on transition firings in the net.
 *****************************************************************************/
void doVarAsgNet(hpnActionADT a,hpnZoneADT &zone,cStateADT &cState,
                 dMarkingADT &marking,ruleADT **rules,char *arcType,
                 eventADT *events,int nevents,int nplaces,int nsignals,
                 signalADT *signals,hpnIneqListADT hpnIneqL)
{
  for(int i=nevents;i<nevents+nplaces;i++) {
#ifdef __HPN_VAR_ASG__
    printf("Examining: %s->%s\n",events[a->enabled]->event,
           events[i]->event);
#endif
    if(rules[a->enabled][i]->ruletype > 0 &&
       isMemberPc(i,events)) {
#ifdef __HPN_VAR_ASG__
      printf("Var asg for %s->%s\n",events[a->enabled]->event,
             events[i]->event);
#endif
      dbmRemove(&zone,i);
#ifdef __HPN_VAR_ASG__
      printZone(zone,events,cState,nevents);
#endif
      int zIndex = getZoneIndex(zone,-1,i);
      zone->matrix[0][zIndex] = rules[a->enabled][i]->plower;
      zone->matrix[zIndex][0] = rules[a->enabled][i]->pupper;
      varAsgUpdate(zone,cState,i,rules,nevents,arcType,events,nplaces);

#ifdef __HPN_VAR_ASG__
      printZone(zone,events,cState,nevents);
#endif
      for(int i=0;i < hpnIneqL->ineqCnt;i++) {
        ineqADT ineq = hpnIneqL->l[i]->ineq;
	if (ineq->type > 4) continue;
	if((testIneq(ineq,cState,zone,events,nevents,rules,marking)) &&
	   (marking->state[ineq->signal]=='0')) {
	  marking->state[ineq->signal]='1';
	  /* Add clocks which are now running */
	  if (enabledXition(ineq->transition,events,cState,marking,zone,
			    nevents,rules,signals,nsignals,nplaces)) {
	    int j;
	    for(j=nevents;j<nevents+nplaces;j++) {
	      if(rules[j][ineq->transition]->ruletype > 0 &&
		 arcType[rules[j][ineq->transition]->marking] == 'D' &&
		 marking->marking[rules[j][ineq->transition]->marking]=='F')
		break;
	    }
	    if (j==nevents+nplaces) {
	      for(int j=nevents;j<nevents+nplaces;j++) {
		if(rules[j][ineq->transition]->ruletype > 0 &&
		   arcType[rules[j][ineq->transition]->marking] == 'D' &&
		   enabledXition(ineq->transition,events,cState,marking,
				 zone,nevents,rules,signals,nsignals,
				 nplaces)) {
		  hpnBounds clk;
		  clk.upper = 0;
		  clk.lower = 0;
		  dbmAdd(&zone,j,ineq->transition,clk,cState,rules,nevents,
			 nplaces);
		  addIntroClocks(i,j,cState,rules);
		}
	      }
	    }
	  }
	}
	else {
	  if((!testIneq(ineq,cState,zone,events,nevents,rules,marking))&&
	     (marking->state[ineq->signal]=='1')) {
	    marking->state[ineq->signal]='0';
	    /* Remove clocks which are no longer running */ 
	    for(int j=nevents;j<nevents+nplaces;j++) {
	      if(rules[j][ineq->transition]->ruletype > 0 &&
		 arcType[rules[j][ineq->transition]->marking] == 'D' &&
		 !(enabledXition(ineq->transition,events,cState,marking,
				 zone,nevents,rules,signals,nsignals,
				 nplaces))) {
		if(isMemberCf(j,ineq->transition,cState,rules)) {  
		  rmCf(j,ineq->transition,cState,rules);
		}
		if(isMemberDBM(j,ineq->transition,zone)) {
		  dbmRemove(&zone,j,ineq->transition);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*****************************************************************************
 * Do the variable assignments based on annotations to the variables.
 *****************************************************************************/
void doVarAsgVar(hpnActionADT a,hpnZoneADT &zone,cStateADT &cState,
                 dMarkingADT &marking,ruleADT **rules,char *arcType,
                 eventADT *events,int nevents,int nplaces,int nsignals,
                 signalADT *signals,hpnIneqListADT hpnIneqL)
{
  for(int i=0;i < hpnIneqL->ineqCnt;i++) {
    if(hpnIneqL->l[i]->xition == a->enabled &&
       hpnIneqL->l[i]->ineq->type == 5) {
      ineqADT ineq = hpnIneqL->l[i]->ineq;
      dbmRemove(&zone,ineq->place);
      int zIndex = getZoneIndex(zone,-1,ineq->place);
      zone->matrix[0][zIndex] = ineq->constant;
      zone->matrix[zIndex][0] = ineq->constant;
      varAsgUpdate(zone,cState,ineq->place,rules,nevents,arcType,events,
		   nplaces);

#ifdef __HPN_VAR_ASG__
      printZone(zone,events,cState,nevents);
#endif
      for(int i=0;i < hpnIneqL->ineqCnt;i++) {
        ineqADT ineq = hpnIneqL->l[i]->ineq;
	if (ineq->type > 4) continue;
#ifdef __HPN_VAR_ASG__
	printf("place=%s type=%d constant=%d transition=%s\n",
	       events[ineq->place]->event,
	       ineq->type,
	       ineq->constant,
	       events[ineq->transition]->event);
#endif
	if((testIneq(ineq,cState,zone,events,nevents,rules,marking)) &&
	   (marking->state[ineq->signal]=='0')) {
	  marking->state[ineq->signal]='1';
	  /* Add clocks which are now running */
	  if (enabledXition(ineq->transition,events,cState,marking,zone,
			    nevents,rules,signals,nsignals,nplaces)) {
#ifdef __HPN_VAR_ASG__
	    printf("HERE: %s is now enabled\n",
		   events[ineq->transition]->event);
#endif
	    int j;
	    for(j=nevents;j<nevents+nplaces;j++) {
	      if(rules[j][ineq->transition]->ruletype > 0 &&
		 arcType[rules[j][ineq->transition]->marking] == 'D' &&
		 marking->marking[rules[j][ineq->transition]->marking]=='F')
		break;
	    }
	    if (j==nevents+nplaces) {
	      for(int j=nevents;j<nevents+nplaces;j++) {
		if(rules[j][ineq->transition]->ruletype > 0 &&
		   arcType[rules[j][ineq->transition]->marking] == 'D' &&
		   enabledXition(ineq->transition,events,cState,marking,
				 zone,nevents,rules,signals,nsignals,
				 nplaces)) {
		  hpnBounds clk;
		  clk.upper = 0;
		  clk.lower = 0;
		  dbmAdd(&zone,j,ineq->transition,clk,cState,rules,nevents,
			 nplaces);
		  addIntroClocks(i,j,cState,rules);
		}
	      }
	    }
	  }
	}
	else {
	  if((!testIneq(ineq,cState,zone,events,nevents,rules,marking))&&
	     (marking->state[ineq->signal]=='1')) {
	    marking->state[ineq->signal]='0';
	    /* Remove clocks which are no longer running */ 
	    for(int j=nevents;j<nevents+nplaces;j++) {
	      if(rules[j][ineq->transition]->ruletype > 0 &&
		 arcType[rules[j][ineq->transition]->marking] == 'D' &&
		 !(enabledXition(ineq->transition,events,cState,marking,
				 zone,nevents,rules,signals,nsignals,
				 nplaces))) {
#ifdef __HPN_VAR_ASG__
		printf("HERE: %s is no longer enabled\n",
		       events[ineq->transition]->event);
#endif
		if(isMemberCf(j,ineq->transition,cState,rules)) {  
		  rmCf(j,ineq->transition,cState,rules);
		}
		if(isMemberDBM(j,ineq->transition,zone)) {
		  dbmRemove(&zone,j,ineq->transition);
		}
	      }
	    }
	  }
	}
#ifdef __HPN_VAR_ASG__
	printZone(zone,events,cState,nevents);
#endif
      }
    }
  }
}

/*****************************************************************************
 * Do the variable assignments based on annotations to the variables.
 *****************************************************************************/
void doRateAsg(hpnActionADT a,int nevents,hpnIneqListADT hpnIneqL,
               int *&newWarp)
{
  for(int i=0;i < hpnIneqL->ineqCnt;i++) {
    if(hpnIneqL->l[i]->xition == a->enabled &&
       hpnIneqL->l[i]->ineq->type == 6) {
      ineqADT ineq = hpnIneqL->l[i]->ineq;
      newWarp[ineq->place-nevents] = ineq->constant;
    }
  }
}

/*****************************************************************************
 * Fire the given transition.
 *****************************************************************************/
void fireTransition(hpnActionADT a,hpnZoneADT &zone,cStateADT &cState,
                    dMarkingADT &marking,ruleADT **rules,
                    markkeyADT *markkey,int nrules,char *arcType,
                    eventADT *events,int nevents,int nplaces,int nsignals,
                    int ninpsig,bool verbose,bool disabling,bool *transFire,
                    signalADT *signals,int nSigs,hpnIneqListADT hpnIneqL)
{
#ifdef __HPN_TRANSFIRE__
  printf("Zone upon entering fireTransition.\n");
  printZone(zone,events,cState,nevents);
#endif
  
#ifdef __HPN_SHOW_FIRE__
  if(a->cls == 'F') {
    printConciseMarkingADT(marking,nevents,nrules,nsignals,signals,events,
                           markkey);
    cSetFg(MAGENTA);
    printf("Firing: ");
    printf("%s\n",events[a->enabled]->event);
    cSetAttr(NONE);
  }
#endif
  
  if(a->cls == 'F') {
    /* Fire Transition */
    (*transFire)=true;
    for(int i=nevents;i<nevents+nplaces;i++) {
      /* remove the fired clock from the marking and fired clocks list */
      if(rules[i][a->enabled]->ruletype > 0 &&
         arcType[rules[i][a->enabled]->marking] == 'D') {
        for(int j=1;j<nevents;j++) {
          if(rules[i][j]->ruletype > 0) {
            marking->marking[rules[i][j]->marking] = 'F';
          }
        }
      }

      if(rules[a->enabled][i]->ruletype > 0 &&
         arcType[rules[a->enabled][i]->marking] == 'D') {
        for(int j=1;j<nevents;j++) {
          if(rules[i][j]->ruletype > 0) {
            if (marking->marking[rules[i][j]->marking]=='T') {
              printf("WARNING: safety violation for rule %s -> %s\n",
                     events[i]->event,events[j]->event);
              fprintf(lg,"WARNING: safety violation for rule %s -> %s\n",
                      events[i]->event,events[j]->event);
            }
            marking->marking[rules[i][j]->marking] = 'T';
          }
        }
      }

      if(rules[i][a->enabled]->ruletype > 0 &&
         arcType[rules[i][a->enabled]->marking] == 'D') {
        rmCf(i,a->enabled,cState,rules);
      }
    }

    /* clean up the zone and fired clocks */
    for(int i=nevents;i<nevents+nplaces;i++) {
      if(rules[i][a->enabled]->ruletype > 0 &&
         arcType[rules[i][a->enabled]->marking] == 'D') { 
        for(int j=1;j<nevents;j++) {
          if(rules[i][j]->ruletype > 0 &&
             arcType[rules[i][j]->marking] == 'D') {
            if(isMemberCf(i,j,cState,rules)) {  
              rmCf(i,j,cState,rules);
            }
            if(isMemberDBM(i,j,zone)) {
              dbmRemove(&zone,i,j);
            }
          }
        }
      }
    }

    /* do variable assignment */
    doVarAsgNet(a,zone,cState,marking,rules,arcType,events,nevents,nplaces,
                nsignals,signals,hpnIneqL);
    doVarAsgVar(a,zone,cState,marking,rules,arcType,events,nevents,nplaces,
                nsignals,signals,hpnIneqL);
    
    /* check the invariants and remove markings that shouldn't be
       possible */
    for(int i=nevents;i<nevents+nplaces;i++) {
      if(events[i]->inequalities != NULL) {
        bool possible = false;
        for(int j=1;j<nevents;j++) {
          if(rules[i][j]->ruletype > 0 &&
             marking->marking[rules[i][j]->marking] == 'T') {
            /* the given place is marked and has inequalities on it */
            possible = true;
            break;
          }
        }
        if(possible) {
          bool invalid = false;
          for(ineqADT ineq = events[i]->inequalities;ineq != NULL;
              ineq = ineq->next) {
	    if (ineq->type > 4) continue;
            if(!testIneq(ineq,cState,zone,events,nevents,rules,marking)) {
              invalid = true;
              break;
            }
            if(invalid) {
              /* unmark the offending place */
#ifdef __HPN_WARN__
              cSetFg(RED);
              printf("WARNING: ");
              cSetAttr(NONE);
              printf("unmarking: %s\n",events[i]->event);
#endif
              for(int j=1;j<nevents;j++) {
                if(rules[i][j]->ruletype > 0) {
                  marking->marking[rules[i][j]->marking] = 'F';
                }
              }
            }
          }
        }
      }
    }
    
#ifdef __HPN_FIRECLOCK__
    printf("Zone after transition fired.\n");
    printZone(zone,events,cState,nevents);
#endif
    if (events[a->enabled]->signal >= 0) {
      if (marking->state[events[a->enabled]->signal]=='R') {
        marking->state[events[a->enabled]->signal]='1';
      }
      else if (marking->state[events[a->enabled]->signal]=='F') {
        marking->state[events[a->enabled]->signal]='0';
      }
      else {
        printf("ERROR: Inconsistent state assignment\n");
        fprintf(lg,"ERROR: Inconsistent state assignment\n");
      }
    }

    if (!hpnUpdateState(events,rules,markkey,marking,nevents,nrules,nsignals,
                        ninpsig,verbose,disabling,arcType,nSigs,a->enabled)) {
      return;
    }
    
    /* add the new clocks */
    for(int i=nevents;i<nevents+nplaces;i++) {
     /*  if(rules[a->enabled][i]->ruletype > 0 && */
/*          arcType[rules[a->enabled][i]->marking] == 'D') { */
        for(int j=1;j<nevents;j++) {
          if(rules[i][j]->ruletype > 0 &&
             (arcType[rules[i][j]->marking] == 'D') &&
             marking->marking[rules[i][j]->marking] == 'T')
	    if (!isMemberDBM(i,j,zone) && !isMemberCf(i,j,cState,rules) &&
		enabledXition(j,events,cState,marking,zone,nevents,rules,
			      signals,nsignals,nplaces)) {
	      hpnBounds clk;
	      clk.upper = 0;
	      clk.lower = 0;
	      dbmAdd(&zone,i,j,clk,cState,rules,nevents,nplaces);
	      addIntroClocks(i,j,cState,rules);
	    } else { 
	      if ((isMemberDBM(i,j,zone) || isMemberCf(i,j,cState,rules)) &&
		  !enabledXition(j,events,cState,marking,zone,nevents,rules,
				 signals,nsignals,nplaces)) {
		if(isMemberCf(i,j,cState,rules)) {  
		  rmCf(i,j,cState,rules);
		}
		if(isMemberDBM(i,j,zone)) {
		  dbmRemove(&zone,i,j);
		}
	      }
	    }
        }
        //}
    }

#ifdef __HPN_FIRECLOCK__
    printf("Zone after new clocks have been added.\n");
    printZone(zone,events,cState,nevents);
#endif
    
    resetVelocity(cState,nevents);
    /* fill the newWarp array w/ the updated warp values */
    int* newWarp = (int*)GetBlock(sizeof(int) * nplaces);
    for(int i=nevents;i<nevents+nplaces;i++) {
      newWarp[i-nevents] = xdot(i,events,rules,marking,zone,cState,
                                nevents,nplaces,signals,nsignals);
    }
#ifdef __HPN_DEBUG_WARP__
  printf("Warps in fireTransition.\n");
  for(int i=nevents;i<nevents+nplaces;i++) {
    printf("%s:%d\n",events[i]->event,newWarp[i-nevents]);
  }
#endif
    
#ifdef __HPN_FIRECLOCK__
    printf("BEFORE: ");
    for(int i=nevents;i<nevents+nplaces;i++) {
      printf("%s:%d, ",events[i]->event,newWarp[i-nevents]);
    }
    printf("\n");
#endif
    doRateAsg(a,nevents,hpnIneqL,newWarp);
#ifdef __HPN_FIRECLOCK__
    printf("AFTER: ");
    for(int i=nevents;i<nevents+nplaces;i++) {
      printf("%s:%d, ",events[i]->event,newWarp[i-nevents]);
    }
    printf("\n");
#endif
    
    for(int i=nevents;i<nevents+nplaces;i++) {
      /* add the new places */
      if(isMemberPc(i,events)) {
#ifdef __HPN_FIRECLOCK__
        printf("Attempting to add: %s:%d:%d\n",events[i]->event,
               newWarp[i-nevents],isMemberDBM(-1,i,zone));
#endif
        if(newWarp[i-nevents] != 0 &&
           !isMemberDBM(-1,i,zone)) {
#ifdef __HPN_FIRECLOCK__
          printf("Adding: %s\n",events[i]->event);
#endif
          dbmAdd(&zone,i);
#ifdef __HPN_FIRECLOCK__
          printf("Zone after adding: %s.\n",events[i]->event);
          printZone(zone,events,cState,nevents);
#endif
          recanonZone(zone);
#ifdef __HPN_FIRECLOCK__
          printf("Zone after recanon: %s.\n",events[i]->event);
          printZone(zone,events,cState,nevents);
#endif
          cState->warp[i-nevents] = 1;

          int newWarpVal = newWarp[i-nevents];
          if(newWarpVal < 0) {
            if(cState->oldWarp[i-nevents] > 0) {
#ifdef __HPN_INTROVECTOR__
              printf("resetIntroClocks3: %d/%d\n",i,nevents);
#endif
              resetIntroClocks(i,cState,rules,nevents,zone);
            }
          }
          else {
            if(cState->oldWarp[i-nevents] < 0) {
#ifdef __HPN_INTROVECTOR__
              printf("resetIntroClocks4: %d/%d\n",i,nevents);
#endif
              resetIntroClocks(i,cState,rules,nevents,zone);
            }
          }
        }
        else if(newWarp[i-nevents] == 0 &&
                isMemberDBM(-1,i,zone)) {
#ifdef __HPN_INFORM__
          cSetFg(RED);
          printf("Inform: ");
          cSetAttr(NONE);
          printf("a variable [%s] is being removed due to an xdot of zero.\n",
                 events[i]->event);
#endif
          int zIndex = getZoneIndex(zone,-1,i);
          zone->matrix[0][zIndex] = zone->matrix[0][zIndex] *
            abs(cState->warp[i-nevents]);
          zone->matrix[zIndex][0] = zone->matrix[zIndex][0] *
            abs(cState->warp[i-nevents]);
          if(cState->warp[i-nevents] < 0) {
            int temp = zone->matrix[0][zIndex];
            zone->matrix[0][zIndex] = zone->matrix[zIndex][0];
            zone->matrix[zIndex][0] = temp;
          }
          cState->oldWarp[i-nevents] = cState->warp[i-nevents];
          cState->warp[i-nevents] = 1;
          
          dbmRemove(&zone,i);

          /* special case code for pll3.g */
          zIndex = getZoneIndex(zone,-1,i);
          for(int j=0;j<nevents;j++) {
            if((rules[i][j]->ruletype > NORULE) &&
               (rules[i][j]->plower == 0) &&
               (rules[i][j]->pupper == 0) &&
               (zone->matrix[0][zIndex]==0) &&
               (zone->matrix[zIndex][0]==0) &&
               (!isMemberDBM(i,j,zone)) && 
               (!isMemberCf(i,j,cState,rules))) {
              /*               printf("Firing: "); */
              /*               cSetAttr(NONE); */
              /*               printf("%s\n",events[a->enabled]->event); */
              hpnBounds clk;
              clk.lower = 0;
              clk.upper = 0;
              /*               printf("SPECIAL CASE 1\n"); */
              dbmAdd(&zone,i,j,clk,cState,rules,nevents,nplaces);
              addIntroClocks(i,j,cState,rules);
            }
          }
        }
      }
    }
#ifdef __HPN_FIRECLOCK__
    printf("Zone after new places have been added.\n");
    printZone(zone,events,cState,nevents);
#endif
    
    dbmWarp(&zone,cState,events,rules,marking,nevents,nplaces,signals,
            nsignals,newWarp);
  }
#ifdef __HPN_TRANSFIRE__
  printf("Zone upon leaving fireTransition.\n");
  printZone(zone,events,cState,nevents);
#endif
}

/*****************************************************************************
 * Update C->D clocks whose predicates may have changed state with the
 * recent variable assignment.
 *****************************************************************************/
void varAsgUpdate(hpnZoneADT &zone,cStateADT &cState,int p,ruleADT **rules,
                  int nevents,char* arcType,eventADT *events,int nplaces)
{
#ifdef __HPN_VAR_ASG__
  printf("Entering:varAsgUpdate()\n");
#endif
  int pZ = getZoneIndex(zone,-1,p);
  for(int i=0;i<nevents;i++) {
#ifdef __HPN_VAR_ASG__
    printf("Checking var asg: %s->%s -- %c\n",events[p]->event,
           events[i]->event,arcType[i]);
#endif
    if(rules[p][i]->ruletype > 0) {
#ifdef __HPN_VAR_ASG__
      printf("Updating var asg: %s->%s\n",events[p]->event,events[i]->event);
#endif
      if((-1 * zone->matrix[0][pZ]) > rules[p][i]->pupper ||
         zone->matrix[pZ][0] < rules[p][i]->plower) {
        if(isMemberDBM(p,i,zone)) {
          dbmRemove(&zone,p,i);
        }
        else if(isMemberCf(p,i,cState,rules)) {
          rmCf(p,i,cState,rules);
        }
      }
      else if(!isMemberDBM(p,i,zone) && !isMemberCf(p,i,cState,rules)) {
        hpnBounds clk;
        clk.upper = 0;
        clk.lower = 0;
        dbmAdd(&zone,p,i,clk,cState,rules,nevents,nplaces);
        addIntroClocks(p,i,cState,rules);
      }
    }
  }
#ifdef __HPN_VAR_ASG__
  printf("Leaving:varAsgUpdate()\n");
#endif
}

/*****************************************************************************
 * Perform a set of actions simultaneously.
 *****************************************************************************/
void simulActions(hpnActionSetADT &aSet,hpnZoneADT *newZone,
                  cStateADT *newCState,ruleADT **rules,eventADT *events,
                  int nevents,int nplaces,char *arcType,
                  dMarkingADT *newMarking,markkeyADT *markkey,int nrules,
                  int ninpsig,bool verbose,bool disabling,bool *transFire,
                  int nsignals,signalADT *signals,hpnIneqListADT hpnIneqL,
                  int nSigs)
{
  bool second_adv=false;

#ifdef __HPN_TRACE__
  printf("simulActions():begin\n");
#endif

  hpnZoneADT zone = *newZone;
  cStateADT cState = *newCState;
  dMarkingADT marking = *newMarking;
  

#ifdef __HPN_SIMUL_ACTION__
  printf("Before simulActions restrict.\n");
  printZone(zone,events,cState,nevents);
#endif

  /* Restrict the zone values for actions about to happen */
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'I' || (*iter)->type == 'D') {
      int zoneP = getZoneIndex(zone,-1,(*iter)->enabling);
      if(zoneP == -1) {
#ifdef __HPN_WARN__
        cSetFg(RED);
        printf("WARNING: ");
        cSetAttr(NONE);
        printf("In simulActions and recieved a non-clock.\n");
#endif
        return;
      }

      int ans;
    
      if((*iter)->type == 'I') {  
        if(cState->warp[(*iter)->enabling-nevents] > 0) {
          ans = chkDiv((-1*rules[(*iter)->enabling][(*iter)->enabled]->plower),
                       cState->warp[(*iter)->enabling-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans1i: %d -- plower: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->plower);
#endif
          if(zone->matrix[0][zoneP] > ans) {
            zone->matrix[0][zoneP] = ans;
          }
          ans = chkDiv((rules[(*iter)->enabling][(*iter)->enabled]->plower),
                       cState->warp[(*iter)->enabling-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans2i: %d -- plower: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->plower);
#endif
          if(zone->matrix[zoneP][0] < ans) {
            zone->matrix[zoneP][0] = ans;
          }
        }
        else {
          ans = chkDiv((-1*rules[(*iter)->enabling][(*iter)->enabled]->pupper),
                       cState->warp[(*iter)->enabling-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans3i: %d -- plower: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->plower);
#endif
          if(zone->matrix[0][zoneP] > ans) {
            zone->matrix[0][zoneP] = ans;
          }
          ans = chkDiv((rules[(*iter)->enabling][(*iter)->enabled]->pupper),
                       cState->warp[(*iter)->enabling-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans4i: %d -- plower: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->plower);
#endif
          if(zone->matrix[zoneP][0] < ans) {
            zone->matrix[zoneP][0] = ans;
          }
        }
      }
      else if((*iter)->type == 'D') {
        if(cState->warp[((*iter)->enabling)-nevents] > 0) {
          ans = chkDiv(-1 * rules[(*iter)->enabling][(*iter)->enabled]->pupper,
                       cState->warp[((*iter)->enabling)-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans1d: %d -- pupper: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->pupper);
#endif
          if(zone->matrix[0][zoneP] > ans) {
            zone->matrix[0][zoneP] = ans;
          }
          ans = chkDiv(rules[(*iter)->enabling][(*iter)->enabled]->pupper,
                       cState->warp[((*iter)->enabling)-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans2d: %d -- pupper: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->pupper);
#endif
          if(zone->matrix[zoneP][0] < ans) {
            zone->matrix[zoneP][0] = ans;
          }
        }
        else {
          ans = chkDiv(-1 * rules[(*iter)->enabling][(*iter)->enabled]->plower,
                       cState->warp[((*iter)->enabling)-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans3d: %d -- pupper: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->pupper);
#endif
          if(zone->matrix[0][zoneP] > ans) {
            zone->matrix[0][zoneP] = ans;
          }
          ans = chkDiv(rules[(*iter)->enabling][(*iter)->enabled]->plower,
                       cState->warp[((*iter)->enabling)-nevents],'C');
#ifdef __HPN_SIMUL_ACTION__
          printf("ans4d: %d -- pupper: %d\n",ans,
                 rules[(*iter)->enabling][(*iter)->enabled]->pupper);
#endif
          if(zone->matrix[zoneP][0] < ans) {
            zone->matrix[zoneP][0] = ans;
          }
        }
      }
    }
    else if((*iter)->type == 'F') {
      int i = getZoneIndex(zone,(*iter)->enabling,(*iter)->enabled);

      if(i == -1) {
        cSetFg(RED);
        printf("WARNING:");
        fprintf(lg,"WARNING:");
        cSetAttr(NONE);
        printf("The clock given to fire was not found in the zone.\n");
        fprintf(lg,"The clock given to fire was not found in the zone.\n");
      }
      zone->matrix[0][i] = -1 *
        rules[(*iter)->enabling][(*iter)->enabled]->lower;
    }
    else if((*iter)->type == 'V') {
      int zIndex = getZoneIndex(zone,(*iter)->enabling,(*iter)->enabled);
#ifdef __HPN_WARN__
      if(zIndex == -1) {
        cSetFg(RED);
        printf("Error:");
        cSetAttr(NONE);
        printf("Variable not in the zone.\n");
    
        return;
      }
#endif
      /* Restrict for the variable deletion */
      if(cState->warp[(*iter)->enabled-nevents] < 0) {
        zone->matrix[0][zIndex] =
          chkDiv(-1 * events[(*iter)->enabled]->lrange,
                 cState->warp[(*iter)->enabled-nevents],'C');
      }
      else {
        zone->matrix[0][zIndex] =
          chkDiv(-1 * events[(*iter)->enabled]->urange,
                 cState->warp[(*iter)->enabled-nevents],'C');
      }
    }
    else if((*iter)->type == 'J' || (*iter)->type == 'E') {
      int zIndex = getZoneIndex(zone,-1,(*iter)->enabled);
#ifdef __HPN_WARN__
      if(zIndex == -1) {
        cSetFg(RED);
        printf("Error: ");
        cSetAttr(NONE);
        printf("Predicate variable not in the zone.\n");
        return;
      }
#endif
      /* restrict appropriately for a change in predicate value */
#ifdef __HPN_SIMUL_ACTION__
      printf("Predicate restriction: const: %d warp: %d\n",(*iter)->ineq->constant,cState->warp[(*iter)->enabled-nevents]);
#endif
      zone->matrix[0][zIndex] =
        chkDiv(-1 * (*iter)->ineq->constant,
               cState->warp[(*iter)->enabled-nevents],'C');
      zone->matrix[zIndex][0] =
        chkDiv((*iter)->ineq->constant,
                 cState->warp[(*iter)->enabled-nevents],'C');
      second_adv=true;
    }

#ifdef __HPN_SIMUL_ACTION__
    printf("After simulAction restrict.\n");
    printZone(zone,events,cState,nevents);
#endif 
  }

  if (second_adv) {
    advanceTime(zone,cState,rules,events,nevents,arcType,nplaces,nrules,
		markkey,marking,hpnIneqL,nsignals,signals);
    recanonZone(zone);
  }
  
#ifdef __HPN_SIMUL_ACTION__
  printf("After simulAction restrict and recanon.\n");
  printZone(zone,events,cState,nevents);
#endif

  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'F') {
      dbmRemove(&zone,(*iter)->enabling,(*iter)->enabled);
      addCf((*iter)->enabling,(*iter)->enabled,cState,rules);
    }
  }
  
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->cls == 'F') {
#ifdef __HPN_TRANSFIRE__
      printf("Zone before entering fireTransition.\n");
      printZone(zone,events,cState,nevents);
#endif
      fireTransition(*iter,zone,cState,marking,rules,markkey,nrules,
                     arcType,events,nevents,nplaces,nsignals,ninpsig,verbose,
                     disabling,transFire,signals,nSigs,hpnIneqL);
    }
#ifdef __HPN_TRANSFIRE__
    printf("Zone after leaving fireTransition.\n");
    printZone(zone,events,cState,nevents);
#endif
  }
  
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'I') {
      hpnBounds clk;
      clk.lower = 0;
      clk.upper = 0;
      addIntroClocks((*iter)->enabling,(*iter)->enabled,cState,rules);
#ifdef __HPN_ENABLINGS_DEBUG__
      printf("Adding %s->%s\n",events[(*iter)->enabling]->event,
             events[(*iter)->enabled]->event);
#endif
      dbmAdd(&zone,(*iter)->enabling,(*iter)->enabled,clk,cState,rules,
             nevents,nplaces);
    }
  }
  
#ifdef __HPN_TRANSFIRE__
  printf("Zone intro/delete...\n");
  printZone(zone,events,cState,nevents);
#endif
  
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'V') {
#ifdef __HPN_SIMUL_ACTION__
      printf("Deleting a variable: %s.\n",events[(*iter)->enabled]->event);
#endif
      dbmRemove(&zone,(*iter)->enabled);
      cState->oldWarp[(*iter)->enabled-nevents] =
        cState->warp[(*iter)->enabled-nevents];
      cState->warp[(*iter)->enabled-nevents] = 1;
      /* move from warp space into 1 space */
      if(cState->oldWarp[(*iter)->enabled-nevents] != 1) {
        int zoneI = getZoneIndex(zone,(*iter)->enabling,(*iter)->enabled);
        if((zone->matrix[0][zoneI] != INFIN)&&
	   (zone->matrix[0][zoneI] != (-1)*INFIN)) {
          zone->matrix[0][zoneI] *= 
	    cState->oldWarp[(*iter)->enabled-nevents];
        }
        if((zone->matrix[zoneI][0] != INFIN)&&
	   (zone->matrix[zoneI][0] != (-1)*INFIN)) {
          zone->matrix[zoneI][0] *= cState->oldWarp[(*iter)->enabled-nevents];
        }  
        if(cState->oldWarp[(*iter)->enabled-nevents] < 0) {
          /* swap */
          int temp = zone->matrix[0][zoneI];
          zone->matrix[0][zoneI] = zone->matrix[zoneI][0];
          zone->matrix[zoneI][0] = temp;
        }
      }
#ifdef __HPN_SIMUL_ACTION__
      printf("Deleting a variable: %s...DONE\n",
             events[(*iter)->enabled]->event);
      printZone(zone,events,cState,nevents);
#endif
    
      /* remove places whose xdot has gone to zero */
      resetVelocity(cState,nevents);
      for(int i=1;i<zone->dbmEnd;i++) {
        if(zone->curClocks[i].enabling == -1) {
          if(xdot(zone->curClocks[i].enabled,events,rules,marking,zone,cState,
                  nevents,nplaces,signals,nsignals) == 0) {
#ifdef __HPN_SIMUL_ACTION__
            printf("Deleting a variable b/c its xdot is 0: %s.\n",
                   events[(*iter)->enabled]->event);
#endif
            zone->matrix[0][i] = zone->matrix[0][i] *
              abs(cState->warp[zone->curClocks[i].enabled-nevents]);
            zone->matrix[i][0] = zone->matrix[i][0] *
              abs(cState->warp[zone->curClocks[i].enabled-nevents]);
            if(cState->warp[zone->curClocks[i].enabled-nevents] < 0) {
              int temp = zone->matrix[0][i];
              zone->matrix[0][i] = zone->matrix[i][0];
              zone->matrix[i][0] = temp;
            }
            cState->oldWarp[zone->curClocks[i].enabled-nevents] =
              cState->warp[zone->curClocks[i].enabled-nevents];
            cState->warp[zone->curClocks[i].enabled-nevents] = 1;
            dbmRemove(&zone,zone->curClocks[i].enabled);
#ifdef __HPN_SIMUL_ACTION__
            printf("Deleting a variable b/c its xdot is 0: %s...DONE\n",
                   events[(*iter)->enabled]->event);
#endif
          }
        }
      }
      
      /* special case code for pll2.g */
      /*       for(int i=0;i<nevents;i++) { */
      /*         if((rules[(*iter)->enabled][i]->ruletype > NORULE) && */
      /*            (rules[(*iter)->enabled][i]->plower == 0) && */
      /*            (!isMemberDBM((*iter)->enabled,i,zone)) && */
      /*            (!isMemberCf((*iter)->enabled,i,cState,rules))) { */
      /* #ifdef __HPN_SIMUL_ACTION__ */
      /*           printf("Special case 2 code executing on %s.\n", */
      /*                  events[(*iter)->enabled]->event); */
      /* #endif */
      /*           hpnBounds clk; */
      /*           clk.lower = 0; */
      /*           clk.upper = 0; */
      /*           dbmAdd(&zone,(*iter)->enabled,i,clk,cState,rules, */
      /*                  nevents,nplaces); */
      /*           addIntroClocks((*iter)->enabled,i,cState,rules); */
      /* #ifdef __HPN_SIMUL_ACTION__ */
      /*           printf("Special case 2 code executing on %s...DONE.\n", */
      /*                  events[(*iter)->enabled]->event); */
      /* #endif */
      /*         } */
      /*       } */
    }
  }
    
  hpnZoneADT oldZone = copyZone(zone);
  cStateADT oldCState = copyCState(cState,nevents,nplaces,nrules);
  
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'D') {
      if(isMemberDBM((*iter)->enabling,(*iter)->enabled,oldZone)) {
        dbmRemove(&oldZone,(*iter)->enabling,(*iter)->enabled);
      }
      else {
        rmCf((*iter)->enabling,(*iter)->enabled,oldCState,rules);
      }
    }
  }

/*   advanceTime(oldZone,oldCState,rules,events,nevents,arcType,nplaces,nrules, */
/*               markkey,marking,hpnIneqL,nsignals,signals); */
/*   recanonZone(oldZone); */

/*   for(list<hpnActionADT>::iterator iter = aSet->l->begin(); */
/*       iter != aSet->l->end();iter++) { */
/*     if ((*iter)->type == 'D') { */
/*       int zoneP = getZoneIndex(oldZone,-1,(*iter)->enabling); */
/*       if (oldCState->warp[oldZone->curClocks[zoneP].enabled-nevents] > 0) { */
/* #ifdef __BAD_DELETE__ */
/*         printf("Checking %s -> %s, upper=%d warp=%d pupper=%d\n", */
/*                events[(*iter)->enabling]->event, */
/*                events[(*iter)->enabled]->event, */
/*                oldZone->matrix[zoneP][0], */
/*                oldCState->warp[oldZone->curClocks[zoneP].enabled-nevents], */
/*                rules[(*iter)->enabling][(*iter)->enabled]->pupper); */
/* #endif */
/*         if ((oldZone->matrix[zoneP][0]* */
/*              oldCState->warp[oldZone->curClocks[zoneP].enabled-nevents]) ==  */
/*             rules[(*iter)->enabling][(*iter)->enabled]->pupper) { */
/* #ifdef __BAD_DELETE__ */
/*           printf("%s -> %s BAD DELETE\n",events[(*iter)->enabling]->event, */
/*                  events[(*iter)->enabled]->event); */
/* #endif */
/*           (*iter)->type='X'; */
/*         } */
/*       } else { */
/* #ifdef __BAD_DELETE__ */
/*         printf("Checking %s -> %s, upper=%d warp=%d pupper=%d\n", */
/*                events[(*iter)->enabling]->event, */
/*                events[(*iter)->enabled]->event, */
/*                oldZone->matrix[zoneP][0], */
/*                oldCState->warp[oldZone->curClocks[zoneP].enabled-nevents], */
/*                rules[(*iter)->enabling][(*iter)->enabled]->plower); */
/* #endif */
/*         if ((oldZone->matrix[zoneP][0]* */
/*              oldCState->warp[oldZone->curClocks[zoneP].enabled-nevents]) ==  */
/*             rules[(*iter)->enabling][(*iter)->enabled]->plower) { */
/* #ifdef __BAD_DELETE__ */
/*           printf("%s -> %s BAD DELETE\n",events[(*iter)->enabling]->event, */
/*                  events[(*iter)->enabled]->event); */
/* #endif */
/*           (*iter)->type='X'; */
/*         } */
/*       } */
/*     } */
/*   } */
  
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'D') {
      if(isMemberDBM((*iter)->enabling,(*iter)->enabled,zone)) {
        dbmRemove(&zone,(*iter)->enabling,(*iter)->enabled);
      }
      else {
        rmCf((*iter)->enabling,(*iter)->enabled,cState,rules);
      }
    }
  }
  
  /* do the predicate actions */
#ifdef __HPN_SIMUL_ACTION__
  printf("Before predicate actions...\n");
  printZone(zone,events,cState,nevents);
#endif
  for(list<hpnActionADT>::iterator iter = aSet->l->begin();
      iter != aSet->l->end();iter++) {
    if((*iter)->type == 'J') {
#ifdef __HPN_SIMUL_ACTION__
      printf("Changing state bit from: %c to T\n",
             marking->state[(*iter)->ineq->signal]);
#endif
      marking->state[(*iter)->ineq->signal] = '1';
      /* add any newly enabled clocks */
      for(int i=nevents;i<nevents+nplaces;i++) {
        if(rules[i][(*iter)->ineq->transition]->ruletype > 0 &&
           arcType[rules[(*iter)->ineq->place][i]->marking] == 'D' &&
           enabledXition((*iter)->ineq->transition,events,cState,marking,zone,
                         nevents,rules,signals,nsignals,nplaces) &&
           marking->marking[rules[i][(*iter)->ineq->transition]->marking]=='T') {
          hpnBounds clk;
          clk.upper = 0;
          clk.lower = 0;
          dbmAdd(&zone,i,(*iter)->ineq->transition,clk,cState,rules,nevents,
                 nplaces);
          addIntroClocks(i,(*iter)->ineq->transition,cState,rules);
        }
      }
    }
    if((*iter)->type == 'E') {
#ifdef __HPN_SIMUL_ACTION__
      printf("Changing state bit from: %c to F\n",
             marking->state[(*iter)->ineq->signal]);
#endif
      marking->state[(*iter)->ineq->signal] = '0';
      /* remove clocks that are no longer enabled */
      for(int i=nevents;i<nevents+nplaces;i++) {
        if(rules[i][(*iter)->ineq->transition]->ruletype > 0 &&
           arcType[rules[(*iter)->ineq->place][i]->marking] == 'D' &&
           !(enabledXition((*iter)->ineq->transition,events,cState,marking,
                           zone,nevents,rules,signals,nsignals,nplaces)) &&
           marking->marking[rules[i][(*iter)->ineq->transition]->marking]=='T') {
          if(isMemberCf(i,(*iter)->ineq->transition,cState,rules)) {
            rmCf(i,(*iter)->ineq->transition,cState,rules);
          }
          if(isMemberDBM(i,(*iter)->ineq->transition,zone)) {
            dbmRemove(&zone,i,(*iter)->ineq->transition);
          }
        }
      }
    }
  }
#ifdef __HPN_SIMUL_ACTION__
  printf("After predicate actions...\n");
  printZone(zone,events,cState,nevents);
#endif
  advanceTime(zone,cState,rules,events,nevents,arcType,nplaces,nrules,markkey,
              marking,hpnIneqL,nsignals,signals);
  recanonZone(zone);

  *newZone = zone;
  *newCState = cState;
  *newMarking = marking;

  /* Clean-up the copies of the old stuff that was created */
  freeCState(oldCState);
  freeZone(oldZone);
  
#ifdef __HPN_TRACE__
  printf("simulActions():end\n");
#endif
}

/*****************************************************************************
 * Allow the simulator to select an action.
 *****************************************************************************/
hpnActionADT simSelectAction(int *simAction,hpnActionSetADT *aSet,
                             eventADT *events)
{
  hpnActionSetADT A = *aSet;
  hpnActionADT a = NULL;
  
  int i=0;
  int maxI = 0;
  if(A->l->size() > 1) {  
    printf("Possible action(s):\n");
    for(list<hpnActionADT>::iterator iter = A->l->begin();
        iter != A->l->end();
        i++,iter++) {
      printf("%d: ",i);
      printAction(*iter,events);
    }
    maxI = i-1;
  }
  else {
    a = A->l->front();
    printf("Possible action(s):\n");
    printf("0: ");
    printAction(a,events);
    maxI = 0;
  }

  int userA = *simAction;
  printf("simReach> ");
  char inS[1024];
  cin.getline(inS,1023);
  string inStr = inS;
  if(!inStr.empty()) {  
    istringstream convStr(inStr);
    convStr >> userA;
    *simAction = userA;
  }
  
  while(userA > maxI) {
    printf("%d is not a valid option.  Please try again.\n",userA);
    printf("simReach> ");
    cin.getline(inS,1023);
    string inStr = inS;
    if(!inStr.empty()) {  
      istringstream convStr(inStr);
      convStr >> userA;
      *simAction = userA;
    }
    *simAction = userA;
  }
  i = 0;
  for(list<hpnActionADT>::iterator iter = A->l->begin();
      iter != A->l->end();
      i++,iter++) {
    if(i == userA) {
      a = *iter;
      A->l->erase(iter);
      break;
    }
  }
  printf("You selected: ");
  printAction(a,events);
  
  return a;
}

/*****************************************************************************
 * A function that allows modification of the zone in the simulator.
 *****************************************************************************/
void modifyZone(hpnZoneADT zone)
{
  /* TBD: what if the input isn't an integer?  How does the conversion
     happen */
  int row = 0;
  char inS[1024];
  printf("Choose a row [0-%d]: ",zone->numClocks);
  cin.getline(inS,1023);
  string inStr = inS;
  if(!inStr.empty()) {  
    istringstream convStr(inStr);
    convStr >> row;
  }
  
  while(row > zone->numClocks) {
    printf("%d is not a valid option.  Please try again.\n",row);
    printf("Choose a row [0-%d]: ",zone->numClocks);
    cin.getline(inS,1023);
    string inStr = inS;
    if(!inStr.empty()) {  
      istringstream convStr(inStr);
      convStr >> row;
    }
  }

  int col = 0;
  printf("Choose a column [0-%d]: ",zone->numClocks-1);
  cin.getline(inS,1023);
  inStr = inS;
  if(!inStr.empty()) {  
    istringstream convStr(inStr);
    convStr >> col;
  }
  
  while(col >= zone->numClocks) {
    printf("%d is not a valid option.  Please try again.\n",col);
    printf("Choose a column [0-%d]: ",zone->numClocks);
    cin.getline(inS,1023);
    string inStr = inS;
    if(!inStr.empty()) {  
      istringstream convStr(inStr);
      convStr >> col;
    }
  }

  int val = 0;
  printf("Choose a new value: ");
  cin.getline(inS,1023);
  inStr = inS;
  if(!inStr.empty()) {  
    istringstream convStr(inStr);
    convStr >> val;
  }
  
  while(val > zone->numClocks) {
    printf("%d is not a valid option.  Please try again.\n",val);
    printf("Choose a new value: ");
    cin.getline(inS,1023);
    string inStr = inS;
    if(!inStr.empty()) {  
      istringstream convStr(inStr);
      convStr >> val;
    }
  }
  zone->matrix[row][col] = val;
}

/*****************************************************************************
 * Print the current zone in a format that is usable by the zone simulator.
 *****************************************************************************/
void simPrintZone(hpnZoneADT zone,eventADT *events)
{
  printf("Zone that can be used by the zone simulator:\n");
  printf("%d\n",zone->numClocks);
  for(int i=0;i<zone->numClocks;i++) {
    if(i == 0) {
      printf("c0 ");
    }
    else if(zone->curClocks[i].enabling != -1) {
      printf("%s.%s ",events[zone->curClocks[i].enabling]->event,
             events[zone->curClocks[i].enabled]->event);
    }
    else {
      printf("%s ",events[zone->curClocks[i].enabled]->event);
    }
    for(int j=0;j<zone->numClocks;j++) {
      printf("%d ",zone->matrix[i][j]);
    }
    printf("\n");
  }
}

/*****************************************************************************
 * Print the possible user options for the simulator.
 *****************************************************************************/
void simPrintHelp()
{
  printf("(a)ction - select an action to execute.\n");
  printf("(t)ransition - run until the next transition fires.\n");
  printf("(z)one - print the zone.\n");
  printf("(c)locks - print the fired clocks.\n");
  printf("(m)arking - print the current marking.\n");
  printf("(w)arp - print the current continous variables and their warp.\n");
  printf("m(o)dify - change a zone entry.\n");
  printf("(p)rint - print a zone in zone simulator format.\n");
  printf("(q)uit - quit the simulator.\n");
  printf("(h)elp - print this message.\n");
}

/*****************************************************************************
 * The basic simulator for the reachability analysis.
 *****************************************************************************/
hpnSimStruct simReach(string *simInput,int *simAction,hpnActionSetADT *aSet,
                      eventADT *events,hpnZoneADT zone,cStateADT cState,
                      markkeyADT *markkey,int nrules,int nevents,int nsignals,
                      dMarkingADT marking,bool transFire,signalADT *signals)
{
  hpnSimStruct s;
  
  while(true) {
#ifdef __HPN_CHRIS_SIM__
    printConciseMarkingADT(marking,nevents,nrules,nsignals,signals,events,
                           markkey);
#endif
    if(*simInput == "t" && !transFire) {
      s.action = rmActionSetItem(*aSet);
      s.quit = false;
      break;
    }
    else {
      printf("WARNING: The simulator is currently broken due to major changes which were made easier by breaking the simulator for now.\n");
      printf("simReach1> ");
      string input;
      char inS[1024];
      cin.getline(inS,1023);
      input = inS;
      if(input.empty()) {
        input = *simInput;
      }

      if(input == "a" || input == "action") {
        *simInput = "a";
        s.action = simSelectAction(simAction,aSet,events);
        s.quit = false;
        break;
      }
      else if(input == "t" || input == "transition") {
        *simInput = "t";
        s.action = rmActionSetItem(*aSet);
        s.quit = false;
        break;
      }
      else if(input == "z" || input == "zone") {
        *simInput = "z";
        printZone(zone,events);
      }
      else if(input == "c" || input == "clocks") {
        *simInput = "c";
        printFiredClocks(cState,markkey,events,nrules);
      }
      else if(input == "m" || input == "marking") {
        *simInput = "m";
        printConciseMarkingADT(marking,nevents,nrules,nsignals,signals,events,
                               markkey);
      }
      else if(input == "o" || input == "modify") {
        *simInput = "m";
        modifyZone(zone);
      }
      else if(input == "w" || input == "warp") {
        *simInput = "w";
        printContVars(zone,events,cState,nevents);
      }
      else if(input == "p" || input == "print") {
        *simInput = "p";
        simPrintZone(zone,events);
      }
      else if(input == "q" || input == "quit") {
        s.action = NULL;
        s.quit = true;
        break;
      }
      else if(input == "h" || input == "help") {
        *simInput = "h";
        simPrintHelp();
      }
      else {
        printf("Invalid option: %s -- Press 'h' for help.\n",input.c_str());
      }
    }
  }
  
  return s;
}

/*****************************************************************************
 * Finds the reachable states for an hpn.
 *****************************************************************************/
bool hpnRsg(char * filename,signalADT *signals,eventADT *events,
            ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
            int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
            char * startstate,bool si,bool verbose,bddADT bdd_state,
            bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
            int nord,bool noparg,bool search,bool useDot,bool poReduce,
            bool manual)
{
  FILE *fp=NULL;
  timeval t0,t1;
  dMarkingADT marking;
  char* arcType; /* type of each rule:
                    'D' - discrete
                    'C' - continuous
                    'H' - hybrid w/ Pd and Tc (hybrid rule)
                    'P' - hybrid w/ Pc and Td (predicate hybrid rule) */

  /* simulator "global" variables */
  string simInput = "a";
  /*   int simAction = 0; */

  set<int> sigSet;
  for(int i=0;i<nevents;i++) {
    if(events[i]->signal >= 0) {
      sigSet.insert(events[i]->signal);
    }
  }
  int nSigs = sigSet.size();
  
#ifdef __HPN_VALGRIND__
  printf("V: nSigs: %d - nsignals: %d\n",nSigs,nsignals);
  printf("START ENABLINGS/INVARIANTS...\n");
  for(int i=0;i<nevents+nplaces;i++) {
    for(ineqADT ineq = events[i]->inequalities;ineq != NULL;
        ineq = ineq->next) {
      printf("%s - ",events[i]->event);
      printIneq(ineq,events);
      if(events[i]->type & CONT) {
        printf(" - C");
      }
      else {
        printf(" - D");
      }
      printf("\n");
    }
  }
  printf("END ENABLINGS/INVARIANTS...\n");
  printf("START SIGNALS STUFF...\n");
  for(int i=0;i<nevents+nplaces;i++) {
    printf("%s/%d: %d\n",events[i]->event,i,events[i]->signal);
  }
  printf("END SIGNALS STUFF...\n");
#endif
  
#ifdef __UNIT_TEST__
  doTests();
#endif

#ifdef __HPN_ENABLINGS_DEBUG__
  printf("START ENABLINGS/INVARIANTS...\n");
  for(int i=0;i<nevents+nplaces;i++) {
    for(ineqADT ineq = events[i]->inequalities;ineq != NULL;
        ineq = ineq->next) {
      printf("%s - ",events[i]->event);
      printIneq(ineq,events);
      if(events[i]->type & CONT) {
        printf(" - C");
      }
      else {
        printf(" - D");
      }
      printf("\n");
    }
  }
  printf("END ENABLINGS/INVARIANTS...\n");
  printf("START SIGNALS STUFF...\n");
  for(int i=0;i<nevents+nplaces;i++) {
    printf("%d: %d\n",i,events[i]->signal);
  }
  printf("END SIGNALS STUFF...\n");
#endif
  
  /* check to see if the manual simulator has been requested */
  bool sim;
  if(manual) {
    sim = true;
  }
  else {
    sim = false;
  }
  
  gettimeofday(&t0, NULL);
  fp = hpnInitOutput(filename,signals,nsignals,ninpsig,verbose);
  if(verbose && !fp)
    return false;
  /* clear the state table */
  initialize_state_table(LOADED,FALSE,state_table);
  hpnInitArcType(&arcType,markkey,events,nrules);
  marking = hpnFindInitialMarking(events,rules,markkey,arcType,nevents,nrules,
                                  ninpsig,nsignals,startstate,verbose);

  hpnIneqListADT hpnIneqL = hpnInitIneqListADT(events,nevents);
#ifdef __HPN_INEQ_DEBUG__
  for(int i=0;i<hpnIneqL->ineqCnt;i++) {
    if(i > 0) {
      printf(",");
    }
    printf("%s-:%d:%d:-",events[hpnIneqL->l[i]->xition]->event,
           hpnIneqL->l[i]->xition,hpnIneqL->l[i]->ineq->transition);
    printIneq(hpnIneqL->l[i]->ineq,events);
  }
  printf("\n");
#endif
  hpnSetInitialPreds(marking,hpnIneqL,events);
  /* if no initial marking, there is problem, so bail out */
  if(marking == NULL) {
    if(verbose) fclose(fp);
    return(FALSE);
  }
#ifdef __HPN_INEQ_DEBUG__
  printf("nsignals: %d\n",nsignals);
  printHpnIneqList(hpnIneqL,events);
#endif
  cStateADT cState = hpnInitCState(events,rules,marking,markkey,arcType,
                                   nevents,nplaces,nrules);

  hpnZoneADT zone = hpnFindInitialZone(rules,markkey,events,marking,cState,
                                       nrules,nevents,nplaces,arcType,signals,
                                       hpnIneqL,nsignals);

#ifdef __HPN_DEBUG__
  printZone(zone,events);
  printf("Adding the initial state\n");
  cSetFg(CYAN);
  printf("New state: %s[0]\n",marking->state);
  cSetAttr(NONE);
#endif

#ifdef __HPN_NEW_STATE__
      cSetFg(YELLOW);
      printf("Adding initial state.\n");
      cSetAttr(NONE);
      printZone(zone,events,cState,nevents);
      printConciseMarkingADT(marking,nevents,nrules,nsignals,signals,events,
                             markkey);
      printFiredClocks(cState,markkey,events,nrules);
#endif
  /* Add the initial state to the state table */
  warpClockkey(cState,zone,nevents);
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0,NULL,marking->state,
            marking->marking,marking->enablings,nsignals,zone->matrix,
            zone->curClocks,zone->numClocks,0,verbose,1,nrules,
            zone->numClocks,zone->numClocks,bdd_state,use_bdd,markkey,
            timeopts,-1,-1,cState->firedClocks,rules,nevents+nplaces,1);
  int stackDepth = 1;
  int numStates = 1;
  int numZones = 1;
  int totZones = 1;
  int maxDepth = 0;
  int newState;

  hpnActionSetADT A = hpnFindPossibleActions(marking,zone,cState,events,rules,
                                             nevents,nplaces,nrules,markkey,
                                             arcType,signals,hpnIneqL,
                                             nsignals);
  
  if(A->l->empty()) {
    cSetFg(RED);
    printf("WARNING: ");
    cSetAttr(NONE);
    printf("Deadlock in the initial state.\n");
    return false;
  }
  
  bool done = false;
  hpnActionSetADT workSet = NULL;
  hpnActionADT a = NULL;
  hpnStackADT stateSpaceStack = NULL;
  hpnZoneADT newZone = NULL;
  dMarkingADT newMarking = NULL;
  cStateADT newCState = NULL;

  while(!done) {
    /*     /\* become a simulator *\/ */
    /*     if(sim) { */
    /*       hpnSimStruct s = simReach(&simInput,&simAction,&A,events,zone,cState, */
    /*                                 markkey,nrules,nevents,nsignals,marking, */
    /*                                 transFire); */
    /*       transFire = false; */

    /*       if(s.quit) { */
    /*         return true; */
    /*       } */
    /*       else { */
    /*         a = s.action; */
    /*       } */
    /*     } */
    /*     else {  */
    /*       /\* remove a random set of items from the set *\/ */
    /*       a = rmActionSetItem(A); */
    /*     } */
    bool transFire=false;
    workSet = buildWorkingActions(A); /* generate the work set by
                                         removing the simultaneous
                                         actions to be done now from A */
    
#ifdef __HPN_SHOW_WORKING_ACTION__
    for(list<hpnActionADT>::iterator i=workSet->l->begin();
        i!=workSet->l->end();i++) {
      hpnActionADT hA = (*i);
      if(hA->enabling == -1) {
        printf("Working on action: %s -- ",events[hA->enabled]->event);
      } 
      else {  
        printf("Working on action: %s->%s -- ",events[hA->enabling]->event,
               events[hA->enabled]->event);
      }
    
      cSetFg(YELLOW);
      if(hA->type == FIRE_CLK) {
        printf("Fire");
      }
      else if(hA->type == INTRO_CLK) {
        printf("Introduce");
      }
      else if(hA->type == DEL_CLK) {
        printf("Delete");
      }
      else if(hA->type == DEL_VAR) {
        printf("Deactivate variable");
      }
      else if(hA->type == INTRO_PRED) {
        printf("Introduce predicate");
      }
      else if(hA->type == DEL_PRED) {
        printf("Delete predicate");
      }
      cSetAttr(NONE);
      if(hA->cls == 'F') {
        printf(" [F]");
      }

      if(hA->type == INTRO_PRED || hA->type == DEL_PRED) {
        printf(" - ");
        printIneq(hA->ineq,events);
      }
      printf("\n");
    }
#endif

    /* if there are actions left not in the work set then put them on
       the stack for later */
    if(!A->l->empty()) {
      dMarkingADT cMarking = hpnCopyMarking(marking);
      hpnZoneADT cZone = copyZone(zone);
      cStateADT cCState = copyCState(cState,nevents,nplaces,nrules);
      hpnActionSetADT cA = copyActionSet(A);

#ifdef __HPN_STACK_DEBUG__
      cSetFg(BLUE);
      printf("%d - Pushing stack.\n",stackDepth);
      cSetAttr(NONE);
#endif
      hpnPushState(&stateSpaceStack,cMarking,cZone,cCState,cA);
      stackDepth++;
      if(stackDepth>maxDepth) {
        maxDepth=stackDepth;
      }
    }

    newZone = copyZone(zone);
    newCState = copyCState(cState,nevents,nplaces,nrules);
    newMarking = hpnCopyMarking(marking);

    /* execute the current set of actions */
    simulActions(workSet,&newZone,&newCState,rules,events,nevents,nplaces,
                 arcType,&newMarking,markkey,nrules,ninpsig,verbose,
                 timeopts.disabling || timeopts.nofail,&transFire,nsignals,
                 signals,hpnIneqL,nSigs);

    /* Fix up an action to be sent to add_state for acounting purposes
       in the state graph */
    a = *(workSet->l->begin());
    int count=0; 
    for(list<hpnActionADT>::iterator i=workSet->l->begin();
        i!=workSet->l->end();i++) { 
      count++;
      if (count > 1) {
        break;
      }
    }
    if (count > 1) {
      a->type = 'S';
    }
    
    
    if(newMarking) {
#ifdef __HPN_NEW_STATE__
      cSetFg(YELLOW);
      printf("Trying to add this zone\n");
      cSetAttr(NONE);
/*       printf("PREVIOUS\n"); */
/*       printZone(zone,events,cState,nevents); */
/*       printConciseMarkingADT(marking,nevents,nrules,nsignals,signals,events, */
/*                              markkey); */
/*       printFiredClocks(cState,markkey,events,nrules); */
      printf("NEW\n");
      printZone(newZone,events,newCState,nevents);
      printConciseMarkingADT(newMarking,nevents,nrules,nsignals,signals,events,
                             markkey);
      printFiredClocks(newCState,markkey,events,nrules);
#endif
      
      /* check to see if the new state information is really a new
         state */
      char action_type = a->type;
      if(transFire) {
        action_type=FIRE_TRANS;
      }
      warpClockkey(newCState,newZone,nevents);
      newState = add_state(fp,state_table,marking->state,
                           marking->marking,marking->enablings,zone->matrix,
                           zone->curClocks,zone->numClocks,cState->firedClocks,
                           newMarking->state,newMarking->marking,
                           newMarking->enablings,nsignals,newZone->matrix,
                           newZone->curClocks,newZone->numClocks,
                           numStates,verbose,1,nrules,newZone->numClocks,
                           newZone->numClocks,bdd_state,use_bdd,markkey,
                           timeopts,a->enabling,a->enabled,
                           newCState->firedClocks,rules,nevents+nplaces,1,
                           action_type);
    }
    else {
      newState = 0;
    }
    
    if(newState != 0) {
      if(newState > 0) {
        numZones++;
        totZones++;
      }
      else {
        numZones = numZones + newState + 1;
        totZones++;
      }
      
      if (newState == 1) {
#ifdef __HPN_NEW_STATE__
        cSetFg(CYAN);
        printf("New state: %s[%d] -- zone: %d -- totZones: %d\n",newMarking->state,numStates,numZones,totZones);
        cSetAttr(NONE);
#endif

#ifdef __HPN_ADD_STATE__
        printf("NEW STATE[%d]: %s -%s-> %s\n",numStates,marking->state,
               events[a->enabled]->event,newMarking->state);
        printConciseMarkingADT(newMarking,nevents,nrules,nsignals,signals,
                               events,markkey);
        for (int i=0;i<newZone->numClocks;i++)
          if (newZone->curClocks[i].enabling == -1 &&
              events[newZone->curClocks[i].enabled]->type & CONT) {
            if (i >= newZone->dbmEnd)
              cSetFg(RED);
            printf("%s [%d,%d] warp=%d ",
                   events[newZone->curClocks[i].enabled]->event,
                   newZone->matrix[0][i],
                   newZone->matrix[i][0],
                   newCState->warp[newZone->curClocks[i].enabled-nevents]);
            if (i >= newZone->dbmEnd)
              cSetAttr(NONE);
          }
        printf("\n");
#endif
#ifdef __HPN_ADD_STATE2__
	printZone(newZone,events,newCState,nevents);
        printf("NEW STATE[%d]: %s -%s-> %s\n",numStates,marking->state,
               events[a->enabled]->event,newMarking->state);
        printConciseMarkingADT(newMarking,nevents,nrules,nsignals,signals,
                               events,markkey);
        for (int i=0;i<newZone->numClocks;i++)
          if (newZone->curClocks[i].enabling == -1 &&
              events[newZone->curClocks[i].enabled]->type & CONT) {
            if (i >= newZone->dbmEnd)
              cSetFg(RED);
	    if (newCState->warp[newZone->curClocks[i].enabled-nevents] > 0) {
	      printf("%s [",events[newZone->curClocks[i].enabled]->event);
	      if (newZone->matrix[0][i]==INFIN) {
		printf("-U,");
	      } else {
		printf("%d,",(-1)*newZone->matrix[0][i]*
		       newCState->warp[newZone->curClocks[i].enabled-nevents]);
	      }
	      if (newZone->matrix[i][0]==INFIN) {
		printf("U]");
	      } else {
		printf("%d]",newZone->matrix[i][0]*
		       newCState->warp[newZone->curClocks[i].enabled-nevents]);
	      }
	      printf(" warp=%d ",
		     newCState->warp[newZone->curClocks[i].enabled-nevents]);
	    } else {
	      printf("%s [",events[newZone->curClocks[i].enabled]->event);
	      if (newZone->matrix[i][0]==INFIN) {
		printf("U,");
	      } else {
		printf("%d,",newZone->matrix[i][0]*
		       newCState->warp[newZone->curClocks[i].enabled-nevents]);
	      }
	      if (newZone->matrix[0][i]==INFIN) {
		printf("-U]");
	      } else {
		printf("%d]",(-1)*newZone->matrix[0][i]*
		       newCState->warp[newZone->curClocks[i].enabled-nevents]);
	      }
	      printf(" warp=%d ",
		     newCState->warp[newZone->curClocks[i].enabled-nevents]);
	    }
	    if (i >= newZone->dbmEnd)
              cSetAttr(NONE);
          }
        printf("\n");
#endif
        numStates++;
      }
      else {
#ifdef __HPN_NEW_STATE__
        cSetFg(CYAN);
        printf("New zone: %d -- %d -- states: %d -- totZones: %d",numZones,newState,numStates,totZones);
        cSetAttr(NONE);
        printf("\n");
#endif
#ifdef __HPN_ADD_ZONE__
        printf("NEW ZONE[%d]: ",numZones);
        for (int i=0;i<newZone->numClocks;i++)
          if (newZone->curClocks[i].enabling == -1 &&
              events[newZone->curClocks[i].enabled]->type & CONT) {
            if (i >= newZone->dbmEnd)
              cSetFg(RED);
            printf("%s [%d,%d] warp=%d ",
                   events[newZone->curClocks[i].enabled]->event,
                   newZone->matrix[0][i],
                   newZone->matrix[i][0],
                   newCState->warp[newZone->curClocks[i].enabled-nevents]);
            if (i >= newZone->dbmEnd)
              cSetAttr(NONE);
          }
        printf("stackSize: %d",stackDepth);
        printf("\n");
#endif
      }
      if(numZones % 100 == 0) {
        printf("Zones: %d -- Stack size: %d -- States: %d\n",
               numZones,stackDepth,numStates);
        fprintf(lg,"Zones: %d -- Stack size: %d -- States: %d\n",
                numZones,stackDepth,numStates);
#ifdef __MEMSTATS__
        struct mallinfo memuse;
        memuse=mallinfo();
        printf("memory: max=%d inuse=%d free=%d \n",
               memuse.arena,memuse.uordblks,memuse.fordblks);
        fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
                memuse.arena,memuse.uordblks,memuse.fordblks);
#endif
      }
      /* free up the memory */
      freeZone(zone);
      hpnDeleteMarking(marking);
      freeCState(cState);
      freeActionSet(workSet);
      freeActionSet(A);
      
      zone = newZone;
      marking = newMarking;
      cState = newCState;

      /* grab new actions based on the new state */
      A = hpnFindPossibleActions(marking,zone,cState,events,rules,
                                 nevents,nplaces,nrules,markkey,
                                 arcType,signals,hpnIneqL,nsignals);
      
      if(A->l->empty()) {
        cSetFg(RED);
        printf("WARNING: ");
        fprintf(lg,"WARNING: ");
        cSetAttr(NONE);
        printf("A is NULL!\n");
        fprintf(lg,"A is NULL!\n");
        printf("Error found after exploring %d zones\n",numZones);
        fprintf(lg,"Error found after exploring %d zones\n",numZones);
        printf("Error found after exploring %d states\n",numStates);
        fprintf(lg,"Error found after exploring %d states\n",numStates);
        
        if (!timeopts.nofail) {
          add_state(fp,state_table,marking->state,marking->marking,
                    marking->enablings,zone->matrix,zone->curClocks,
                    zone->numClocks,cState->firedClocks,NULL,NULL,
                    NULL,nsignals,NULL,NULL,0,numStates,verbose,true,
                    nrules,0,0,bdd_state,use_bdd,markkey,timeopts,-1,
                    nevents,NULL,rules,nevents,true);
          freeZone(zone);

          hpnDeleteMarking(marking);
          freeCState(cState);
          freeActionSet(A);
          return false;
        }
        
        if(stateSpaceStack != NULL) {
#ifdef __HPN_DEAD_STATE__
          printf("DEAD STATE: %s -%s-> %s\n",marking->state,
                 events[a->enabled]->event,newMarking->state);
          printConciseMarkingADT(newMarking,nevents,nrules,nsignals,signals,
                                 events,markkey);
          for (int i=0;i<newZone->numClocks;i++)
            if (newZone->curClocks[i].enabling == -1 &&
                events[newZone->curClocks[i].enabled]->type & CONT) {
              if (i >= newZone->dbmEnd)
                cSetFg(RED);
              printf("%s [%d,%d] warp=%d ",
                     events[newZone->curClocks[i].enabled]->event,
                     newZone->matrix[0][i],
                     newZone->matrix[i][0],
                     newCState->warp[newZone->curClocks[i].enabled-nevents]);
              if (i >= newZone->dbmEnd)
                cSetAttr(NONE);
            }
          printf("\n");
#endif

          stackDepth--;
#ifdef __HPN_STACK_DEBUG__
          cSetFg(BLUE);
          printf("%d - Popping stack[1]...\n",stackDepth);
          cSetAttr(NONE);
#endif
          hpnPopState(&stateSpaceStack,&marking,&zone,&cState,&A);
          
#ifdef __HPN_STATE_INFO__
          printZone(zone,events,cState,nevents);
          printActionSet(A,events);
          printFiredClocks(cState,markkey,events,nrules);
          printContVars(zone,events,cState,nevents);
#endif
        }
        else {
          done = true;
        }
      }
    }
    else {
#ifdef __HPN_NO_ADD_STATE__
      printf("SAME STATE: %s -%s-> %s\n",marking->state,
             events[a->enabled]->event,newMarking->state);
      printConciseMarkingADT(newMarking,nevents,nrules,nsignals,signals,
                             events,markkey);
      for (int i=0;i<newZone->numClocks;i++)
        if (newZone->curClocks[i].enabling == -1 &&
            events[newZone->curClocks[i].enabled]->type & CONT) {
          if (i >= newZone->dbmEnd)
            cSetFg(RED);
          printf("%s [%d,%d] warp=%d ",
                 events[newZone->curClocks[i].enabled]->event,
                 newZone->matrix[0][i],
                 newZone->matrix[i][0],
                 newCState->warp[newZone->curClocks[i].enabled-nevents]);
          if (i >= newZone->dbmEnd)
            cSetAttr(NONE);
        }
      printf("\n");
#endif
#ifdef __HPN_NO_ADD_STATE2__
      printf("SAME STATE: %s -%s-> %s\n",marking->state,
             events[a->enabled]->event,newMarking->state);
      printConciseMarkingADT(newMarking,nevents,nrules,nsignals,signals,
                             events,markkey);
      for (int i=0;i<newZone->numClocks;i++)
        if (newZone->curClocks[i].enabling == -1 &&
            events[newZone->curClocks[i].enabled]->type & CONT) {
          if (i >= newZone->dbmEnd)
            cSetFg(RED);
	  if (newCState->warp[newZone->curClocks[i].enabled-nevents] > 0) {
	    printf("%s [",events[newZone->curClocks[i].enabled]->event);
	    if (newZone->matrix[0][i]==INFIN) {
	      printf("-U,");
	    } else {
	      printf("%d,",(-1)*newZone->matrix[0][i]*
		     newCState->warp[newZone->curClocks[i].enabled-nevents]);
	    }
	    if (newZone->matrix[i][0]==INFIN) {
	      printf("U]");
	    } else {
	      printf("%d]",newZone->matrix[i][0]*
		     newCState->warp[newZone->curClocks[i].enabled-nevents]);
	    }
	    printf(" warp=%d ",
		   newCState->warp[newZone->curClocks[i].enabled-nevents]);
	  } else {
	    printf("%s [",events[newZone->curClocks[i].enabled]->event);
	    if (newZone->matrix[i][0]==INFIN) {
	      printf("U,");
	    } else {
	      printf("%d,",newZone->matrix[i][0]*
		     newCState->warp[newZone->curClocks[i].enabled-nevents]);
	    }
	    if (newZone->matrix[0][i]==INFIN) {
	      printf("-U]");
	    } else {
	      printf("%d]",(-1)*newZone->matrix[0][i]*
		     newCState->warp[newZone->curClocks[i].enabled-nevents]);
	    }
	    printf(" warp=%d ",
		   newCState->warp[newZone->curClocks[i].enabled-nevents]);
	  }
          if (i >= newZone->dbmEnd)
            cSetAttr(NONE);
        }
      printf("\n");
#endif
#ifdef __HPN_NEW_STATE__
      cSetFg(CYAN);
      printf("Same state -- %d\n",newState);
      cSetAttr(NONE);
#endif
      /* New marking is not a new state, so clean up and continue */
      freeZone(newZone);
      hpnDeleteMarking(newMarking);
      freeCState(newCState);
      freeZone(zone);
      hpnDeleteMarking(marking);
      freeCState(cState);
      freeActionSet(workSet);
      freeActionSet(A);

      if(stateSpaceStack != NULL) {
        stackDepth--;
#ifdef __HPN_STACK_DEBUG__
        cSetFg(BLUE);
        printf("%d - Popping stack[2]...\n",stackDepth);
        cSetAttr(NONE);
#endif
        hpnPopState(&stateSpaceStack,&marking,&zone,&cState,&A);
        
#ifdef __HPN_STATE_INFO__
        printZone(zone,events,cState,nevents);
        printActionSet(A,events);
        printFiredClocks(cState,markkey,events,nrules);
        printContVars(zone,events,cState,nevents);
#endif
      }
      else {
        done = true;
      }
    }
  }

#ifdef __HPN_STATE_INFO__
  cSetFg(RED);
  printf("Final state graph...\n");
  cSetAttr(NONE);
  my_print_graph(state_table,events,markkey,nrules);
#endif
  
  /* free up no longer needed memory */
  freeHpnIneqListADT(hpnIneqL);
  free(arcType);
  
  /* Statistics from here on out */
  struct tms tBuf;
  double time0, time1;
  double cpuTime = 0;
  int clkTicks = sysconf(_SC_CLK_TCK);
  gettimeofday(&t1,NULL);
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  
  if(times(&tBuf) >= 0) {
    cpuTime = (float)(tBuf.tms_utime + tBuf.tms_stime)/(float)clkTicks;
  }
  printf("Zones: %d\n",numZones);
  fprintf(lg,"Zones: %d\n",numZones);
  printf("States: %d\n",numStates);
  fprintf(lg,"States: %d\n",numStates);
#ifdef __MEMSTATS__
  struct mallinfo memuse;
  memuse=mallinfo();
  printf("memory: max=%d inuse=%d free=%d \n",
         memuse.arena,memuse.uordblks,memuse.fordblks);
  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
          memuse.arena,memuse.uordblks,memuse.fordblks);
#endif

  return true;
}
