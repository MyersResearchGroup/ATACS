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

#include <sys/times.h>
#include <unistd.h>
#include <limits.h>
#include <sstream>
#include "common_timing.h"
#include "findreg.h"
#include "findrsg.h"
#include "lhpnrsg.h"
#include "color.h"
#include "hpnrsg.h"
#include "displayADT.h"

// #define __LHPN_INIT__ 
// #define __LHPN_DEBUG_WARP__ 
// #define __LHPN_PRED_DEBUG__
// #define __LHPN_TRACE__ 
// #define __LHPN_UPDATE_STATE__ 
// #define __LHPN_ASG__
// #define __LHPN_EN__
// #define __LHPN_FIRET__ 
// #define __LHPN_INIT_INEQ__
// #define __LHPN_STACK__
// #define __LHPN_EVENTS__
// #define __LHPN_NEW_STATE__
// #define __LHPN_ADD_STATE__
// #define __LHPN_DELAYS__
// #define __LHPN_DEFENSIVE__
// #define __INTEXPR__
// #define __LHPN_ADD_ACTION__ 
// #define __LHPN_EVAL__
// #define __LHPN_WARN__
// #define __LHPN_RATE_EVENT__

/*****************************************************************************
 *****************************************************************************
 * Unrelated helper functions.
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Output a red warning message.
 *****************************************************************************/
void warn(char* msg)
{
  cSetFg(RED);
  printf("WARNING: ");
  cSetAttr(NONE);
  fprintf(lg,"WARNING: ");
  printf("%s\n",msg);
  fprintf(lg,"%s\n",msg);
}

/*****************************************************************************
 * Output a colored message.
 *****************************************************************************/
void colorMsg(int color,char* msg)
{
  cSetFg(color);
  printf("%s",msg);
  cSetAttr(NONE);
}

/*****************************************************************************
 * 
 *****************************************************************************/
/* ineqADT copyI(ineqADT i) */
/* { */
/*   ineqADT copyI = (ineqADT)GetBlock(sizeof(*copyI)); */
/*   copyI->place = i->place; */
/*   copyI->type = i->type; */
/*   copyI->uconstant = i->constant; */
/*   copyI->signal = i->signal; */
/*   copyI->transition = i->transition; */
/*   copyI->next = i->next; */
/*   return copyI; */
/* } */

/*****************************************************************************
 * 
 *****************************************************************************/
void printI(ineqADT i,eventADT *events)
{
  if(i->type != 7) {
    printf("%s",events[i->place]->event);
  }
  else {
    printf("Boolean asg.\n");
  }
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
  else if(i->type == 5 || i->type == 7) {
    printf(" := ");
  }
  else if(i->type == 6) {
    printf(" dot:= ");
  }
  printf("%s",i->expr);
  printf("(%d,%d)",i->constant,i->uconstant);
  printf(" [%d]",i->signal);
}

/*****************************************************************************
 * 
 *****************************************************************************/
void printEv(eventADT *events,int nevents,int nplaces) 
{
  for(int i=0;i<nevents+nplaces;i++) { 
    printf("%s: lower:%d upper:%d linitrate:%d uinitrate:%d\n",events[i]->event,events[i]->lower,events[i]->upper,events[i]->linitrate,events[i]->uinitrate);
    printf("hsl:%s\n",events[i]->hsl);
    printf("ineq:");
    for(ineqADT j=events[i]->inequalities;j!=NULL;j=j->next) {
      printI(j,events);
      printf(" ");
    }
    printf("\n");
    printf("sop:");
    for(level_exp j=events[i]->SOP;j!=NULL;j=j->next) {
      printf("%s ",j->product);
    }
    printf("\n--------\n");
  }
}

/*****************************************************************************
 *****************************************************************************
 * lhpnZoneADT operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Creates a new zone data structure.  numClocks is initialized to the
 * size of the zone and the matrix values are initialized to INFIN.
 *****************************************************************************/
lhpnZoneADT initZ(int size) 
{
  lhpnZoneADT z = (lhpnZoneADT)GetBlock(sizeof(*z));

  z->size = size;
  z->dbmEnd = size;
  if(size == 0) {
    warn("creating a zero size zone.");
    z->curClocks = NULL;
    z->matrix = NULL;
    return z;
  }
  z->curClocks = (clockkeyADT)GetBlock(size * sizeof(struct clockkey));
  for(int i=0;i<size;i++) {
    z->curClocks[i].enabled = -1;
    z->curClocks[i].enabling = -1;
    z->curClocks[i].eventk_num = -1;
    z->curClocks[i].causal = -1;
    z->curClocks[i].anti = NULL;
  }
  
  /* allocate the matrix and initialize the values */
  z->matrix = (int**)GetBlock(size * sizeof(int*));
  for(int i=0;i<size;i++) {
    z->matrix[i] = (int*)GetBlock(size * sizeof(int*));
    for(int j=0;j<size;j++) {
      if(i == j) {
        z->matrix[i][i] = 0;
      }
      else {
        z->matrix[i][j] = INFIN;
      }
    }
  }
  return z;
}

/*****************************************************************************
 * Free the memory allocated for the given zone.
 *****************************************************************************/
void freeZ(lhpnZoneADT z)
{
  if(z == NULL) {
    warn("Attempting to free a NULL zone.");
  }
  free(z->curClocks);
  for(int i=0;i<z->size;i++) {
    free(z->matrix[i]);
  }
  free(z->matrix);
  free(z);
}

/*****************************************************************************
 * Return a copy of the given zone.
 *****************************************************************************/
lhpnZoneADT copyZ(lhpnZoneADT z)
{
  lhpnZoneADT newZ = initZ(z->size);

  newZ->dbmEnd = z->dbmEnd;
  for(int i=0;i<z->size;i++) {
    newZ->curClocks[i] = z->curClocks[i];
    for(int j=0;j<z->size;j++) {
      newZ->matrix[i][j] = z->matrix[i][j];
    }
  }
  return newZ;
}

/*****************************************************************************
 * lhs = rhs - the idea is to use the function to copy a smaller rhs
 * into a larger lhs.
 *****************************************************************************/
void copyValZ(lhpnZoneADT lhs,lhpnZoneADT rhs)
{
  lhs->dbmEnd = rhs->dbmEnd;
  for(int i=0;i<rhs->size;i++) {
    lhs->curClocks[i].enabled = rhs->curClocks[i].enabled;
    lhs->curClocks[i].enabling = rhs->curClocks[i].enabling;
    for(int j=0;j<rhs->size;j++) {
      lhs->matrix[i][j] = rhs->matrix[i][j];
    }
  }
}

/*****************************************************************************
 * Add n dimensions to the matrix.  The added dimensions are created
 * via createZone so the added dimensions are initialized to default
 * values.
 *****************************************************************************/
void addDimZ(lhpnZoneADT &z,int n)
{
  lhpnZoneADT newZ = initZ(z->size+n);
  copyValZ(newZ,z);
  
  freeZ(z);
  z = newZ;
}

/*****************************************************************************
 * Verify that the zone is consistent by checking that the diagonal is
 * still zeroes.
 *****************************************************************************/
bool validZ(lhpnZoneADT z)
{
  for(int i=0;i<z->size;i++) {
    if(z->matrix[i][i] != 0) { 
      warn("Inconsistent zone found.");
      return false;
    }   
  }
  return true;
}

/*****************************************************************************
 * Uses Floyd's algorithm (naive version which in O(n^3)) to recanonicalize
 * the zone.
 *****************************************************************************/
void recanonZ(lhpnZoneADT z)
{
  for(int i=0;i<z->dbmEnd;i++) {
    for(int j=0;j<z->dbmEnd;j++) {
      for(int k=0;k<z->dbmEnd;k++) {
        if ((z->matrix[j][i]!=INFIN)&&(z->matrix[i][k]!=INFIN)&&
            (z->matrix[j][i] > 0)&&(z->matrix[i][k] > 0)&&
            (z->matrix[j][i] > (INFIN - z->matrix[i][k]))) {
          warn("Entry greater than infinity.");
        }
        if(z->matrix[j][i] != INFIN && z->matrix[i][k] != INFIN &&
           z->matrix[j][k] > z->matrix[j][i] + z->matrix[i][k]) {
          z->matrix[j][k] = z->matrix[j][i] + z->matrix[i][k];
        }
      }
    }
  }
  validZ(z);
}

/*****************************************************************************
 * Is a given variable or transition in the zone?
 *****************************************************************************/
bool isMemberZ(int enabling,int enabled,lhpnZoneADT z)
{
  for(int i=0;i<z->size;i++) {
    if(z->curClocks[i].enabling == enabling &&
       z->curClocks[i].enabled == enabled) {
      return true;
    }
  }
  return false;
}

/*****************************************************************************
 * Given an event based index return the zone based index.
 *****************************************************************************/
int getIndexZ(lhpnZoneADT z,int enabling,int enabled)
{
  for(int i=0;i<z->size;i++){
    if(z->curClocks[i].enabling == enabling &&
       z->curClocks[i].enabled == enabled) {
      return i;
    }
  }
  char msg[255];
  sprintf(msg,"The given event (%d,%d) was not found in the zone.",enabling,
          enabled);
  warn(msg);
  return -1;
}

/*****************************************************************************
 * Print a zone.
 *****************************************************************************/
void printZ(lhpnZoneADT z)
{
  colorMsg(GREEN,"Zone: ");
  printf("dbmEnd: %d -- size: %d\n",z->dbmEnd,z->size);
  
  printf("Transitions: ");
  for(int i=0;i<z->size;i++) {
    printf("%d ",i);
  }
  printf("\n");
  
  for(int i=0;i<z->size;i++) {
      printf("\t%d",z->curClocks[i].enabled);
  }
  printf("\n");

  for(int i=0;i<z->size;i++) {
    printf("%d",z->curClocks[i].enabled);
    for(int j=0;j<z->size;j++) {
      if(z->matrix[i][j] == INFIN) {
        printf("\tU");
      }
      else if(z->matrix[i][j] == -1*INFIN) {
        printf("\t-U");
      }
      else {
        printf("\t%d",z->matrix[i][j]);
      }
    }
    printf("\n");
  }
}

/*****************************************************************************
 * Print a zone.
 *****************************************************************************/
void printZ(lhpnZoneADT z,eventADT *events)
{
  colorMsg(GREEN,"Zone: ");
  printf("dbmEnd: %d -- size: %d\n",z->dbmEnd,z->size);
  
  printf("Transitions: ");
  for(int i=0;i<z->size;i++) {
    printf("%s ",events[z->curClocks[i].enabled]->event);
  }
  printf("\n");
  
  for(int i=0;i<z->size;i++) {
      printf("\t%d",z->curClocks[i].enabled);
  }
  printf("\n");

  for(int i=0;i<z->size;i++) {
    printf("%d",z->curClocks[i].enabled);
    for(int j=0;j<z->size;j++) {
      if(z->matrix[i][j] == INFIN) {
        printf("\tU");
      }
      else if(z->matrix[i][j] == -1*INFIN) {
        printf("\t-U");
      }
      else {
        printf("\t%d",z->matrix[i][j]);
      }
    }
    printf("\n");
  }
}

/*****************************************************************************
 * Print a zone with rate information.
 *****************************************************************************/
void printZ(lhpnZoneADT z,eventADT *events,int nevents,lhpnRateADT r)
{
  printf("Rates: ");
  for(int i=0;i<z->size;i++) {
    if(isVar(z->curClocks[i].enabled,events)) {
      printf("%s[%d,%d]%d ",events[z->curClocks[i].enabled]->event,
             r->bound[z->curClocks[i].enabled-nevents].lower,
             r->bound[z->curClocks[i].enabled-nevents].upper,
             r->bound[z->curClocks[i].enabled-nevents].current);
    }
  }
  printf("\n");
  printf("Size: %d\n",z->size);
  printZ(z,events);
}

/*****************************************************************************
 * Swap the two dimension in the zone. d1 and d2 are zone based indices.
 *****************************************************************************/
void swapDimZ(lhpnZoneADT z,int d1,int d2)
{
  lhpnZoneADT oldZ = copyZ(z);

  /* swap the clocks in the clock array */
  z->curClocks[d1] = oldZ->curClocks[d2];
  z->curClocks[d2] = oldZ->curClocks[d1];

  
  for(int i=0;i<z->size;i++) {
    if(d1 == i) {
      z->matrix[d1][d1] = oldZ->matrix[d2][d2];
      z->matrix[d1][d1] = oldZ->matrix[d2][d2];
    }
    else if(d2 == i) {
      z->matrix[d1][d2] = oldZ->matrix[d2][d1];
      z->matrix[d2][d1] = oldZ->matrix[d1][d2];
    }
    else {
      z->matrix[d1][i] = oldZ->matrix[d2][i];
      z->matrix[i][d1] = oldZ->matrix[i][d2];
      z->matrix[d2][i] = oldZ->matrix[d1][i];
      z->matrix[i][d2] = oldZ->matrix[i][d1];
    }
  }
  freeZ(oldZ);
}

/*****************************************************************************
 * Add the given transition to the given zone.
 *****************************************************************************/
void addTransZ(lhpnZoneADT &z,int enabled,int lower,int upper) 
{
  int oldSize = z->size;
  int oldEnd = z->dbmEnd;

  addDimZ(z,1);
  z->curClocks[oldSize].enabling = -1;
  z->curClocks[oldSize].enabled = enabled;

  z->matrix[0][oldSize] = -1 * lower;
  z->matrix[oldSize][0] = upper;

  for(int i=0;i<oldEnd;i++) {
    if(z->matrix[0][i] != INFIN && upper != INFIN) {
      z->matrix[oldSize][i] = z->matrix[0][i] + upper;
    }
    if(z->matrix[i][0] != INFIN && lower != INFIN) {
      z->matrix[i][oldSize] = z->matrix[i][0] - lower;
    }
  }
  if(oldSize != z->dbmEnd) {
    swapDimZ(z,oldSize,z->dbmEnd);
  }
  z->dbmEnd++;
}

/*****************************************************************************
 * Remove the given transition from the given zone.
 *****************************************************************************/
void rmTransZ(lhpnZoneADT &z,int enabled)
{
  int index = getIndexZ(z,-1,enabled);
  if(index == -1) {
    warn("Attempted to remove a nonexistent transition from the zone.");
    return;
  }

  lhpnZoneADT newZ = initZ(z->size-1);
  newZ->dbmEnd = z->dbmEnd-1;
  for(int i=0;i<z->size;i++) {
    if(i < index) {
      newZ->curClocks[i].enabling = z->curClocks[i].enabling;
      newZ->curClocks[i].enabled = z->curClocks[i].enabled;
      for(int j=0;j<z->size;j++) {
        if(j < index) {
          newZ->matrix[i][j] = z->matrix[i][j];
        }
        else if(j > index) {
          newZ->matrix[i][j-1] = z->matrix[i][j];
        }
      }
    }
    else if(i > index) {
      newZ->curClocks[i-1].enabling = z->curClocks[i].enabling;
      newZ->curClocks[i-1].enabled = z->curClocks[i].enabled;
      for(int j=0;j<z->size;j++) {
        if(j < index) {
          newZ->matrix[i-1][j] = z->matrix[i][j];
        }
        else if(j > index) {
          newZ->matrix[i-1][j-1] = z->matrix[i][j];
        }
      }
    }
  }
  freeZ(z);
  z = newZ;
}

/*****************************************************************************
 * Add the given variable to the given zone.  This involves moving an
 * inactive variable into the active section.
 *****************************************************************************/
void addVarZ(lhpnZoneADT z,int enabled)
{
  int i = getIndexZ(z,-2,enabled);
  if(i == -1) {
    warn("Attempted to add a nonexistant place to the zone.");
  }

  if(i != z->dbmEnd) {
    swapDimZ(z,i,z->dbmEnd);
  }
  z->dbmEnd++;
}

/*****************************************************************************
 * Add the given variable to the given zone.  This is only used during
 * initialization.
 *****************************************************************************/
void addVarZ(lhpnZoneADT &z,int enabled,eventADT *events)
{
  int oldSize = z->size;
  
  addDimZ(z,1);
  z->curClocks[oldSize].enabling = -2;
  z->curClocks[oldSize].enabled = enabled;

  z->matrix[0][oldSize] = -1 * events[enabled]->lower;
  z->matrix[oldSize][0] = events[enabled]->upper;

  /* adjust other timers in accordance with the new dimension
     remembering to take into account INFIN */
  for(int j=0;j<oldSize;j++) {
    if(z->matrix[0][j] != INFIN
       && events[enabled]->upper != INFIN) {
      z->matrix[oldSize][j] = z->matrix[0][j]
        + events[enabled]->upper;
    }
    if(z->matrix[j][0] != INFIN
       && events[enabled]->lower != INFIN) {
      z->matrix[j][oldSize] = z->matrix[j][0]
        - events[enabled]->lower;
    }
  }
  
  z->dbmEnd++;
}

/*****************************************************************************
 * Remove the given variable from the given zone.  This involves
 * moving an active variable into the inactive section.
 *****************************************************************************/
void rmVarZ(lhpnZoneADT z,int enabled)
{
  int i = getIndexZ(z,-2,enabled);
  if(i == -1) {
    warn("Attempted to remove a nonexistant place from the zone.");
  }
  if(i >= z->dbmEnd) {
    warn("Attempted to remove an inactive place from the zone.");
    return;
  }

  z->dbmEnd--;
  if(i != z->dbmEnd) {
    swapDimZ(z,i,z->dbmEnd);
  }
  for(int j=1;j<z->size;j++) {
    if(z->dbmEnd != j) {
      z->matrix[z->dbmEnd][j] = INFIN;
      z->matrix[j][z->dbmEnd] = INFIN;
    }
  }
}
/*****************************************************************************
 * Update inequality values 
 *****************************************************************************/
void ineq_update(ineqADT i, lhpnStateADT s, int nevents){
  if (i->tree){
    i->tree->eval(s,nevents);
    i->constant=i->tree->lvalue;
    i->uconstant=i->tree->uvalue;
/*     if (i->constant!=i->uconstant){ */
/*       printf("Inequality RHS evaluates to a range\n"); */
/*     } */
  }
}


/*****************************************************************************
 * 
 *****************************************************************************/
void restrictZ(lhpnZoneADT z,eventSet &E,eventADT *events,ineqList &ineqL,
               lhpnRateADT r,int nevents,lhpnStateADT cur_state)
{
  int ans;

  for(eventSet::iterator i=E.begin();i!=E.end();i++) {
    if((*i)->ineq >= 0) {
      int j = getIndexZ(z,-2,ineqL[(*i)->ineq]->place);
      if(j == -1) {
        warn("Place requested for restrict is not in the zone.");
      }
      ineq_update(ineqL[(*i)->ineq],cur_state,nevents);
      z->matrix[0][j] = chkDiv(-1 * ineqL[(*i)->ineq]->constant,
                               r->bound[z->curClocks[j].enabled-nevents].current,
                               'C');
      ans = chkDiv(ineqL[(*i)->ineq]->constant,
		   r->bound[z->curClocks[j].enabled-nevents].current,
                               'C');
      if (z->matrix[j][0] < ans) {
	z->matrix[j][0] = ans;
      }
    }
    else if((*i)->t >= 0) {
      int j = getIndexZ(z,-1,(*i)->t);
      if(j == -1) {
        warn("Transition requested for restrict is not in the zone.");
      }
      z->matrix[0][j] = -1 * events[(*i)->t]->lower;
    }
  }
}

/*****************************************************************************
 *****************************************************************************
 * lhpnMarkingADT operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Check the ruletype to ensure that it is a valid rule.
 *****************************************************************************/
bool validRule(int ruletype)
{
  return ((ruletype > NORULE) && (ruletype < REDUNDANT));
}

/*****************************************************************************
 * Initialize the marking.
 *****************************************************************************/
lhpnMarkingADT initM(int nrules,int nevents,int nsignals,markkeyADT *markkey,
                     char *startState,eventADT *events,ruleADT **rules)
{
  lhpnMarkingADT m = (lhpnMarkingADT)GetBlock(sizeof *m);
  m->marking = (char*)GetBlock((nrules+1)*sizeof(char));
  m->marking[nrules] = '\0';
  for(int i=0;i<nrules;i++) {
    if(validRule((rules[markkey[i]->enabling][markkey[i]->enabled])->ruletype)
       && markkey[i]->epsilon == 1){
      m->marking[i] = 'T';
    }
    else {
      m->marking[i] = 'F';
    }
  }
  
  m->enablings = (char*)GetBlock((nevents+1)*sizeof(char));
  m->enablings[nevents] = '\0';
  m->enablings[0] = 'F';
  updateM(m,events,rules,markkey,nrules,nevents);
  
  m->state = (char*)GetBlock((nsignals+1)*sizeof(char));
  m->state[nsignals] = '\0';
  if(startState == NULL) {
    warn("No initial state information.");
  }
  else {
#ifdef __LHPN_INIT__
    printf("Startstate:%s size:%d\n",startState,(int)strlen(startState));
#endif
    for(int i=0;i<(int)strlen(startState);i++) {
      m->state[i] = startState[i];
    }
  }
  return m;
}

/*****************************************************************************
 * Free the given marking.
 *****************************************************************************/
void freeM(lhpnMarkingADT m) 
{
  if(m == NULL) {
    warn("Attempted to free a NULL marking.");
  }
  free(m->marking);
  free(m->enablings);
  free(m->state);
  free(m);
}

/*****************************************************************************
 * Make a copy of the marking.
 *****************************************************************************/
lhpnMarkingADT copyM(lhpnMarkingADT m)
{
  lhpnMarkingADT copyM = (lhpnMarkingADT)GetBlock(sizeof(*copyM));
  copyM->marking = CopyString(m->marking);
  copyM->enablings = CopyString(m->enablings);
  copyM->state = CopyString(m->state);
  return copyM;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void updateM(lhpnMarkingADT m,eventADT *events,ruleADT **rules,
             markkeyADT *markkey,int nrules,int nevents)
{
  for(int i=1;i<nevents;i++) {
    if(events[i]->dropped) {
      m->enablings[i] = 'F';
    }
    else {
      m->enablings[i] = 'T';
    }
  }

  for(int i=0;i<nrules;i++) {
    if(markkey[i]->enabled < nevents &&
       m->enablings[markkey[i]->enabled] == 'T' &&
       check_rule(rules,markkey[i]->enabling,markkey[i]->enabled,
                  false,false)) {
#ifdef __LHPN_FIRET__
      printf("marking of %s is: %c\n",events[markkey[i]->enabled]->event,
             m->marking[i]);
#endif
      if(m->marking[i] == 'F') {
#ifdef __LHPN_FIRET__
        printf("Disabling: %s\n",events[markkey[i]->enabled]->event);
#endif
        m->enablings[markkey[i]->enabled] = 'F';
      }
    }
  }
}

/*****************************************************************************
 * Pring the marking.
 *****************************************************************************/
void printM(lhpnMarkingADT m,eventADT *events,signalADT *signals,
            markkeyADT *markkey,ineqList &ineqL,int nevents,int nrules,
            int nsignals)
{
  printf("Marking...\n");
  printf("m->marking: ");
  for(int i=0;i<nrules;i++) {
    printf("%s->%s:%c ",events[markkey[i]->enabling]->event,
           events[markkey[i]->enabled]->event,m->marking[i]);
  }
  printf("\n");
  
  printf("m->enablings: ");
  for(int i=0;i<nevents;i++) {
    printf("%s:%c ",events[i]->event,m->enablings[i]);
  }
  printf("\n");
  
  printf("m->state: ");
  for(int i=0;i<nsignals;i++) {
    if(signals[i]->event != 0) {
      printf("%s:%c ",events[signals[i]->event]->event,m->state[i]);
    }
    else {
      bool found = false;
      for(unsigned j=0;j<ineqL.size();j++) {
        if(ineqL[j]->signal == i) {
          found = true;
          printI(ineqL[j],events);
          printf(":%c ",m->state[i]);
        }
      }
      if(!found) {
        printf("f-%s:%c ",signals[i]->name,m->state[i]);
      }
    }    
  }
  printf("\n");
}

/*****************************************************************************
 *****************************************************************************
 * lhpnRateADT  operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnRateADT initLhpnRate(eventADT *events,int nevents,int nplaces)
{
  lhpnRateADT r = (lhpnRateADT)GetBlock(sizeof(*r));
  r->bound = (lhpnBoundADT)GetBlock(nplaces*sizeof(struct lhpnBoundStruct));
  r->oldBound = (lhpnBoundADT)GetBlock(nplaces*sizeof(struct lhpnBoundStruct));
  
  for(int i=0;i<nplaces;i++) {
    r->bound[i].current = events[i+nevents]->linitrate;
    r->oldBound[i].current = 1;
    r->bound[i].lower = events[i+nevents]->linitrate;
    r->oldBound[i].lower = 1;
    r->bound[i].upper = events[i+nevents]->uinitrate;
    r->oldBound[i].upper = 1;
  }
  return r;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void freeLhpnRate(lhpnRateADT r,int nplaces)
{
  if(r == NULL) {
    warn("Trying to free a NULL lhpnRate object.");
  }

  freeBound(r->bound);
  freeBound(r->oldBound);
  free(r);
}

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnRateADT copyLhpnRate(lhpnRateADT r,int nplaces)
{
  lhpnRateADT newR = (lhpnRateADT)GetBlock(sizeof *r);
  newR->bound = (lhpnBoundADT)GetBlock(nplaces*sizeof(struct lhpnBoundStruct));
  newR->oldBound = (lhpnBoundADT)GetBlock(nplaces*sizeof(struct lhpnBoundStruct));
  
  for(int i=0;i<nplaces;i++) {
    newR->bound[i].current = r->bound[i].current;
    newR->oldBound[i].current = r->oldBound[i].current;
    newR->bound[i].lower = r->bound[i].lower;
    newR->oldBound[i].lower = r->oldBound[i].lower;
    newR->bound[i].upper = r->bound[i].upper;
    newR->oldBound[i].upper = r->oldBound[i].upper;
  }
  return newR;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void printLhpnRate(lhpnRateADT r,eventADT *events,int nevents,int nplaces)
{
  printf("lhpnRate...\n");
  printf("rate: ");
  for(int i=0;i<nplaces;i++) {
    printf("%s:[%d,%d]%d ",events[i+nevents]->event,r->bound[i].lower,
           r->bound[i].upper,r->bound[i].current);
  }
  printf("\n");
  
  printf("oldRate: ");
  for(int i=0;i<nplaces;i++) {
    printf("%s:[%d,%d]%d ",events[i+nevents]->event,r->oldBound[i].lower,
           r->oldBound[i].upper,r->oldBound[i].current);
  }
  printf("\n");
}

/*****************************************************************************
 *****************************************************************************
 * lhpnBoundADT  operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnBoundADT initBound()
{
  lhpnBoundADT b = (lhpnBoundADT)GetBlock(sizeof *b);
  b->upper = 0;
  b->lower = 0;
  b->current = 0;
  return b;
}

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnBoundADT initBound(int upper,int lower)
{
  lhpnBoundADT b = (lhpnBoundADT)GetBlock(sizeof *b);
  b->upper = upper;
  b->lower = lower;
  b->current = lower;
  return b;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void freeBound(lhpnBoundADT b)
{
  free(b);
}

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnBoundADT copyBound(lhpnBoundADT b)
{
  lhpnBoundADT newB = (lhpnBoundADT)GetBlock(sizeof *newB);
  newB->upper = b->upper;
  newB->lower = b->lower;
  newB->current = b->current;
  return newB;
}

/*****************************************************************************
 *****************************************************************************
 * lhpnEventADT  operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnEventADT initEvent(int t,int ineq,int rate,char cur_value)
{
  lhpnEventADT e = new lhpnEventStruct();
  e->t = t;
  e->ineq = ineq;
  e->rate = rate;
  e->cur_value = cur_value;
  return e;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void freeEvent(lhpnEventADT e)
{
  delete e;
}

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnEventADT copyEvent(lhpnEventADT e)
{
  lhpnEventADT newE = new lhpnEventStruct();
  newE->t = e->t;
  newE->ineq = e->ineq;
  newE->rate = e->rate;
  newE->cur_value = e->cur_value;
  return newE;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void printLhpnEvent(lhpnEventADT e,eventADT *events,ineqList &ineqL)
{
  if(e->t >= 0) {
    printf("Firing: ");
    printf("%s\n",events[e->t]->event);
  }
  else if (e->ineq >=0) {
    printf("Predicate change: ");
    if (e->cur_value=='1') {
      printf("~(");
    }
    printI(ineqL[e->ineq],events);
    if (e->cur_value=='1') {
      printf(")");
    }
    printf("\n");
  } else {
    printf("Rate change: %s\n",events[e->rate]->event);
  }
}

/*****************************************************************************
 *****************************************************************************
 * eventSetADT  operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * 
 *****************************************************************************/
void freeEventSet(const eventSet &E)
{
  if(!E.empty()) {
    for(eventSet::iterator i=E.begin();i!=E.end();i++) {
      freeEvent(*i);
    }
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
eventSet copyEventSet(const eventSet &E,eventADT *events,ineqList &ineqL)
{
  eventSet newE;
  int count = 0;
  for(eventSet::iterator i=E.begin();i!=E.end();i++) {
    lhpnEventADT e = copyEvent(*i);
    newE.insert(e);
    count++;
  }
  return newE;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void printEventSet(const eventSet &E,eventADT *events,ineqList &ineqL)
{
  for(eventSet::iterator i=E.begin();i!=E.end();i++) {
    printLhpnEvent(*i,events,ineqL);
  }
}

/*****************************************************************************
 *****************************************************************************
 * eventSetsADT  operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * 
 *****************************************************************************/
void freeEventSets(eventSets &E)
{
  for(eventSets::iterator i=E.begin();i!=E.end();i++) {
    freeEventSet(*i);
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
eventSets copyEventSets(eventSets &E,eventADT *events,ineqList &ineqL)
{
  eventSets newE;
  for(eventSets::iterator i=E.begin();i!=E.end();i++) {
    eventSet e = copyEventSet(*i,events,ineqL);
    newE.push_back(e);
  }
  return newE;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void printEventSets(eventSets &E,eventADT *events,ineqList &ineqL)
{
  int count=0;
  for(eventSets::iterator i=E.begin();i!=E.end();i++) {
    printf("Set %d:\n",count);
    printEventSet(*i,events,ineqL);
    count++;
  }
}

/*****************************************************************************
 *****************************************************************************
 * lhpnStateADT  operations
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnStateADT initState(lhpnMarkingADT m,lhpnZoneADT z,lhpnRateADT r,
                       eventSets &E)
{
  lhpnStateADT s = new lhpnStateStruct();
  s->m = m;
  s->z = z;
  s->r = r;
  s->E = E;
  return s;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void freeState(lhpnStateADT s,int nplaces)
{
  freeM(s->m);
  freeZ(s->z);
  freeLhpnRate(s->r,nplaces);
  freeEventSets(s->E);
  delete s;
}

/*****************************************************************************
 * 
 *****************************************************************************/
lhpnStateADT copyState(lhpnStateADT s,eventADT *events,ineqList &ineqL,
                       int nplaces)
{
  lhpnStateADT newS = new lhpnStateStruct();
  newS->m = copyM(s->m);
  newS->z = copyZ(s->z);
  newS->r = copyLhpnRate(s->r,nplaces);
  newS->E = copyEventSets(s->E,events,ineqL); 
  return newS;
}

/*****************************************************************************
 *****************************************************************************
 * expression class implementation
 *****************************************************************************
 *****************************************************************************/

exprsn::exprsn(char willbe, int lNV, int uNV, int ptr){
  op = "";
  r1 = NULL;
  r2 = NULL;
  isit = willbe;
  if ((isit== 'b')||(isit=='t'))
    logical = true;
  else
    logical = false;
  uvalue = uNV;
  lvalue = lNV;
  index = ptr;
  real = 0;
};

exprsn::exprsn(exprsn *nr1, exprsn* nr2, char *nop, char willbe){
  //printf ("making an exprsn with %s and %c\n",nop,willbe);
  op = nop;
  r1 = nr1;
  r2 = nr2;
  isit = willbe;
  if ((isit=='r')||(isit=='l')){
    logical = true;
    uvalue = 1;
    lvalue = 0;
  }
  else{
    logical = false;
    uvalue = INFIN;
    lvalue = -INFIN;
  }
  index = -1;
};

exprsn::~exprsn(){
  if (r1 != NULL){
    delete r1;
    if (r2 != NULL)
      delete r2;
  }
};

void exprsn::op_set(char* new_op){
  op = new_op;
};

string exprsn::out_string(signalADT *signals, eventADT *events){
  string temp = "unimplemented";
  //cout << op <<","<< isit <<endl;

  if (op!=""){
    //cout << "in if \n";
    if (op=="||"){
      temp = string("(") +r1->out_string(signals,events) + string("|") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="&&"){
      temp = string("(") +r1->out_string(signals,events) + string("&") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="->"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="!"){
      temp = string("(~") +r1->out_string(signals,events) + string(")");
    }else if(op=="=="){
      temp = string("(") +r1->out_string(signals,events) + string("=") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op==">"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op==">="){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="<"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="<="){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="[]"){
      temp = string("BIT(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if (op=="+"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="-"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="*"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="^"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="u"){
      temp = string("uniform(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="/"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="%"){
      temp = string("(") +r1->out_string(signals,events) + op + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="U-"){
      temp = string("(") + op + r1->out_string(signals,events)+ string(")");
    }else if(op=="m"){
      temp = string("min(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="M"){
      temp = string("max(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="i"){
      temp = string("idiv(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="&"){
      temp = string("AND(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="|"){
      temp = string("OR(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="X"){
      temp = string("XOR(") +r1->out_string(signals,events) + string(",") + 
	r2->out_string(signals,events)+ string(")");
    }else if(op=="~"){
      temp = string("NOT(") +r1->out_string(signals,events) + string(")");
    }else if(op=="INT"){
      temp = string("INT(") +r1->out_string(signals,events) + string(")");
    }else if(op=="BOOL"){
      temp = string("BOOL(") +r1->out_string(signals,events) + string(")");
    } else{
      temp = "unmatched";
    }
  }
  else { 
    //cout << "op was null! (in else)\n";
    if(isit == 'd'){
      return string("rate(") + string(events[index]->event) + string(")");
    }else{
      if ((isit == 'i')||(isit == 'c')){
	temp = events[index]->event;
      }else if (isit == 'b'){
	temp = signals[index]->name;
      }else if (isit == 'n'){
	std::stringstream outl, outu;
	  outl << lvalue;
	  outu << uvalue;
	  temp = '[' + outl.str() + ',' + outu.str() +']';
      }else if (isit == 't'){
	if (lvalue == 1)
	  temp = string("true");
	else if (uvalue == 0)
	  temp = string("false");
	else
	  temp = string("undef");
      }
    }
  }
  //cout << "gonna return\n";
  return temp;
};

void exprsn::eval(lhpnStateADT cur_state,int nevents){
  char log_val;
  int tl1,tl2,tu1,tu2,i,j,k;
  int preciser = 1;

  if (op!=""){
    //printf("%s, eval left child\n",op.c_str());
    r1->eval(cur_state,nevents);
    if ((r1->lvalue == -INFIN)||(r1->uvalue == INFIN)){
      lvalue = -INFIN;
      uvalue = INFIN;
      return;
    }
    if (r2){
      //printf("eval right child\n");
      r2->eval(cur_state,nevents);
      if ((r2->lvalue == -INFIN)||(r2->uvalue == INFIN)){
      lvalue = -INFIN;
      uvalue = INFIN;
      return;
    }
    }
    if (op=="||"){
      // logical OR
      if (r1->logical){
	tl1 = r1->lvalue;
	tu1 = r1->uvalue;
      }
      else{//convert numeric r1 to boolean
	if ((r1->lvalue == 0)&&(r1->uvalue == 0)){//false
	  tl1 = tu1 = 0;
	}
	else if ((r1->lvalue < 0)&&(r1->uvalue < 0)||
		 (r1->lvalue > 0)&&(r1->uvalue > 0)){//true
	  tl1 = tu1 = 1;
	}
	else{
	  tl1 = 0;
	  tu1 = 1;
	}
      }
      if (r2->logical){
	tl2 = r2->lvalue;
	tu2 = r2->uvalue;
      }
      else{//convert numeric r2 to boolean
	if ((r2->lvalue == 0)&&(r2->uvalue == 0)){//false
	  tl2 = tu2 = 0;
	}
	else if ((r2->lvalue < 0)&&(r2->uvalue < 0)||
		 (r2->lvalue > 0)&&(r2->uvalue > 0)){//true
	  tl2 = tu2 = 1;
	}
	else{
	  tl2 = 0;
	  tu2 = 1;
	}
      }
      lvalue = tl1 || tl2;
      uvalue = tu1 || tu2;
    }else if(op=="&&"){
      // logical AND
      if (r1->logical){
	tl1 = r1->lvalue;
	tu1 = r1->uvalue;
      }
      else{//convert numeric r1 to boolean
	if ((r1->lvalue == 0)&&(r1->uvalue == 0)){//false
	  tl1 = tu1 = 0;
	}
	else if ((r1->lvalue < 0)&&(r1->uvalue < 0)||
		 (r1->lvalue > 0)&&(r1->uvalue > 0)){//true
	  tl1 = tu1 = 1;
	}
	else{
	  tl1 = 0;
	  tu1 = 1;
	}
      }
      if (r2->logical){
	tl2 = r2->lvalue;
	tu2 = r2->uvalue;
      }
      else{//convert numeric r2 to boolean
	if ((r2->lvalue == 0)&&(r2->uvalue == 0)){//false
	  tl2 = tu2 = 0;
	}
	else if ((r2->lvalue < 0)&&(r2->uvalue < 0)||
		 (r2->lvalue > 0)&&(r2->uvalue > 0)){//true
	  tl2 = tu2 = 1;
	}
	else{
	  tl2 = 0;
	  tu2 = 1;
	}
      }
      lvalue = tl1 && tl2;
      uvalue = tu1 && tu2;
#ifdef __LHPN_EVAL__
      printf("and: [%d,%d](%c)&[%d,%d](%c) = [%d,%d]\n",r1->lvalue,
	     r1->uvalue,r1->isit,r2->lvalue,r2->uvalue,r2->isit,lvalue,uvalue);
#endif
    }else if(op=="->"){
      // implication operator
      if (r1->logical){
	tl1 = r1->lvalue;
	tu1 = r1->uvalue;
      }
      else{//convert numeric r1 to boolean
	if ((r1->lvalue == 0)&&(r1->uvalue == 0)){//false
	  tl1 = tu1 = 0;
	}
	else if ((r1->lvalue < 0)&&(r1->uvalue < 0)||
		 (r1->lvalue > 0)&&(r1->uvalue > 0)){//true
	  tl1 = tu1 = 1;
	}
	else{
	  tl1 = 0;
	  tu1 = 1;
	}
      }
      if (r2->logical){
	tl2 = r2->lvalue;
	tu2 = r2->uvalue;
      }
      else{//convert numeric r2 to boolean
	if ((r2->lvalue == 0)&&(r2->uvalue == 0)){//false
	  tl2 = tu2 = 0;
	}
	else if ((r2->lvalue < 0)&&(r2->uvalue < 0)||
		 (r2->lvalue > 0)&&(r2->uvalue > 0)){//true
	  tl2 = tu2 = 1;
	}
	else{
	  tl2 = 0;
	  tu2 = 1;
	}
      }
      lvalue = tl1 || !tl2;
      uvalue = tu1 || !tu2;
    }else if(op=="!"){
      // logical NOT
      if (r1->logical){
	tl1 = r1->lvalue;
	tu1 = r1->uvalue;
      }
      else{//convert numeric r1 to boolean
	if ((r1->lvalue == 0)&&(r1->uvalue == 0)){//false
	  tl1 = tu1 = 0;
	}
	else if ((r1->lvalue < 0)&&(r1->uvalue < 0)||
		 (r1->lvalue > 0)&&(r1->uvalue > 0)){//true
	  tl1 = tu1 = 1;
	}
	else{
	  tl1 = 0;
	  tu1 = 1;
	}
      }
      if (tl1 == tu1){
	lvalue = 1- tl1;
	uvalue = 1- tl1;
      }
#ifdef __LHPN_EVAL__
      printf("not: [%d,%d](%c) = [%d,%d]\n",r1->lvalue,
	     r1->uvalue,r1->isit,lvalue,uvalue);
#endif
      //printf("negation: ~[%d,%d] = [%d,%d]\n",r1->lvalue,r1->uvalue,
      // lvalue,uvalue);
    }else if(op=="=="){
      // "equality" operator
      // true if same point value
      if ((r1->lvalue == r1->uvalue) && (r2->lvalue == r2->uvalue) &&
	  (r1->lvalue == r2->uvalue))
	lvalue = uvalue = 1;
      // false if no overlap
      else if ((r1->lvalue > r2->uvalue)||(r2->lvalue > r1->uvalue))
	lvalue = uvalue = 0;
      // maybe if overlap
      else{
	lvalue = 0;
	uvalue = 1;
      }
#ifdef __LHPN_EVAL__
      printf("[%d,%d]==[%d,%d]=[%d,%d]\n",r1->lvalue,r1->uvalue ,r2->lvalue,r2->uvalue,lvalue,uvalue);  
#endif   
    }else if(op==">"){
      // "greater than" operator
      //true if lower1 > upper2
      if (r1->lvalue > r2->uvalue)
	lvalue = uvalue = 1;
      //false if lower2 >= upper1
      else if (r2->lvalue >= r1->uvalue)
	lvalue = uvalue = 0;
      // maybe if overlap
      else{
	lvalue = 0;
	uvalue = 1;
      }
    }else if(op==">="){
      // "greater than or equal" operator
      //true if lower1 >= upper2
      if (r1->lvalue >= r2->uvalue)
	lvalue = uvalue = 1;
      //false if lower2 > upper1
      else if (r2->lvalue > r1->uvalue)
	lvalue = uvalue = 0;
      // maybe if overlap
      else{
	lvalue = 0;
	uvalue = 1;
      }
    }else if(op=="<"){
      // "less than" operator
      //true if lower2 > upper1
      if (r2->lvalue > r1->uvalue)
	lvalue = uvalue = 1;
      //false if lower1 >= upper2
      else if (r1->lvalue >= r2->uvalue)
	lvalue = uvalue = 0;
      // maybe if overlap
      else{
	lvalue = 0;
	uvalue = 1;
      }
    }else if(op=="<="){
      // "less than or equal" operator
      //true if lower2 >= upper1
      if (r2->lvalue >= r1->uvalue)
	lvalue = uvalue = 1;
      //false if lower1 > upper2
      else if (r1->lvalue > r2->uvalue)
	lvalue = uvalue = 0;
      // maybe if overlap
      else{
	lvalue = 0;
	uvalue = 1;
      }
#ifdef __LHPN_EVAL__
      printf("[%d,%d]<=[%d,%d]=[%d,%d]\n",r1->lvalue,r1->uvalue ,r2->lvalue,r2->uvalue,lvalue,uvalue);  
#endif   
    }else if(op=="[]"){//NEEDS WORK
      // bit extraction operator
      // Only extract if both are point values.  
      if ((r1->lvalue == r1->uvalue)&&(r2->lvalue == r2->uvalue)){
	lvalue = uvalue = (r1->lvalue >> r2->uvalue) & 1;
      }
      else {
	if (!preciser)
	  {
	    lvalue = 0;
	    uvalue = 1;
	  }
	else {
	  uvalue = 0;
	  lvalue = 1;
	  for (i = r1->lvalue;i<=r1->uvalue;i++)
	    for (j = r2->lvalue;j<=r2->uvalue;j++){
	      k = (i >> j) & 1;
	      lvalue &= k;
	      uvalue |= k;
	      if (lvalue < uvalue)
		return;
	    }
	}
      }
    }else if(op=="+"){
      lvalue = r1->lvalue + r2->lvalue;
      uvalue = r1->uvalue + r2->uvalue;
    }else if(op=="-"){
      lvalue = r1->lvalue - r2->uvalue;
      uvalue = r1->uvalue - r2->lvalue;
    }else if(op=="*"){
      tl1 = r1->lvalue * r2->lvalue;
      tl2 = r1->uvalue * r2->uvalue;
      tu1 = r1->lvalue * r2->uvalue;
      tu2 = r1->uvalue * r2->lvalue;
      lvalue = min(min(min(tl1,tl2),tu1),tu2);
      uvalue = max(max(max(tl1,tl2),tu1),tu2);
    }else if(op=="^"){
      tl1 = pow((double)r1->lvalue,(double)r2->lvalue);
      tl2 = pow((double)r1->uvalue,(double)r2->uvalue);
      tu1 = pow((double)r1->lvalue,(double)r2->uvalue);
      tu2 = pow((double)r1->uvalue,(double)r2->lvalue);
      lvalue = min(min(min(tl1,tl2),tu1),tu2);
      uvalue = max(max(max(tl1,tl2),tu1),tu2);
    }else if(op=="u"){
      lvalue = r1->lvalue;
      uvalue = r2->uvalue;
    }else if(op=="/"){
      //ropughly integer division.  
      //DON"T KNOW WHAT FLOATING POINT PART IS!!!!!
      tl1 = floor(r1->lvalue / r2->lvalue);
      tl2 = floor(r1->uvalue / r2->uvalue);
      tu1 = floor(r1->lvalue / r2->uvalue);
      tu2 = floor(r1->uvalue / r2->lvalue);
      lvalue = min(min(min(tl1,tl2),tu1),tu2);
      tl1 = ceil(r1->lvalue / r2->lvalue);
      tl2 = ceil(r1->uvalue / r2->uvalue);
      tu1 = ceil(r1->lvalue / r2->uvalue);
      tu2 = ceil(r1->uvalue / r2->lvalue);
      uvalue = max(max(max(tl1,tl2),tu1),tu2);
    }else if(op=="%"){//NEEDS WORK
      if (!preciser){
	// Only calculate if both are point values.  
	if ((r1->lvalue == r1->uvalue)&&(r2->lvalue == r2->uvalue)){
	  lvalue = uvalue = r1->lvalue % r2->uvalue;
	}
	else{
	  lvalue = min(0,max(-(max(abs(r2->lvalue),abs(r2->lvalue))-1),r1->lvalue));
	  uvalue = max(0,min(max(abs(r2->lvalue),abs(r2->uvalue))-1,r1->uvalue));
	}
      }
      else{
	uvalue = -INFIN;
	lvalue = INFIN;
	for (i = r1->lvalue;i<=r1->uvalue;i++)
	  for (j = r2->lvalue;j<=r2->uvalue;j++){
	    k = i%j;
	    if (k < lvalue)
	      lvalue = k;
	    if (k > uvalue)
	      uvalue = k;
	  }
      }
    }else if(op=="U-"){
      lvalue = -(r1->uvalue);
      uvalue = -(r1->lvalue);
    }else if(op=="INT"){
      lvalue = r1->lvalue;
      uvalue = r1->uvalue;
    }else if(op=="BOOL"){
      if ((r1->lvalue == 0)&& (r1->uvalue == 0))
	lvalue = uvalue = 0;
      else if (((r1->lvalue > 0) && (r1->uvalue > 0))||
	       ((r1->lvalue < 0) && (r1->uvalue < 0)))
	lvalue = uvalue = 1;
      else {
	lvalue = 0;
	uvalue = 1;
      }
    }else if(op=="&"){
      if ((r1->lvalue!=r1->uvalue)||(r2->lvalue!=r2->uvalue)) {
	if (!preciser){
	  lvalue = min(r1->lvalue+r2->lvalue,0);
	  uvalue = max((r1->uvalue),(r2->uvalue));
	}
	else{
	  uvalue = -INFIN;
	  lvalue = INFIN;
	  for (i = r1->lvalue;i<=r1->uvalue;i++)
	    for (j = r2->lvalue;j<=r2->uvalue;j++){
	      k = i&j;
	      if (k < lvalue)
		lvalue = k;
	      if (k > uvalue)
		uvalue = k;
	    }
	}
      }
      else {
	lvalue = (r1->lvalue & r2->lvalue);
	uvalue = (r1->lvalue & r2->lvalue);
      }
#ifdef __LHPN_EVAL__
      printf("BITWISE AND: [%d,%d](%c)&[%d,%d](%c) = [%d,%d]\n",r1->lvalue,
	     r1->uvalue,r1->isit,r2->lvalue,r2->uvalue,r2->isit,lvalue,uvalue);
#endif
    }else if(op=="|"){
      if ((r1->lvalue!=r1->uvalue)||(r2->lvalue!=r2->uvalue)) {
	lvalue = min(r1->lvalue,r2->lvalue);
	uvalue = max(r1->uvalue + r2->uvalue,-1);
      } else {
	lvalue = (r1->lvalue | r2->lvalue);
	uvalue = (r1->lvalue | r2->lvalue);
      }
    }else if(op=="m"){
      lvalue = min(r1->lvalue,r2->lvalue);
      uvalue = min(r1->uvalue,r2->uvalue);
    }else if(op=="M"){
      lvalue = max(r1->lvalue,r2->lvalue);
      uvalue = max(r1->uvalue,r2->uvalue);
    }else if(op=="i"){
      tl1 = r1->lvalue / r2->lvalue;
      tl2 = r1->uvalue / r2->uvalue;
      tu1 = r1->lvalue / r2->uvalue;
      tu2 = r1->uvalue / r2->lvalue;
      lvalue = min(min(min(tl1,tl2),tu1),tu2);
      uvalue = max(max(max(tl1,tl2),tu1),tu2);
    }else if(op=="X"){
      lvalue = min(min(r1->lvalue-r2->uvalue,r2->lvalue-r1->uvalue),0);
      uvalue = max(max(r1->uvalue + r2->uvalue,-(r1->lvalue + r2->lvalue)),-1);
//     }else if(op=="floor"){
//       lvalue = floor(r1->lvalue);
//       uvalue = floor(r1->uvalue);
//     }else if(op=="round"){
//       lvalue = round(r1->lvalue);
//       uvalue = round(r1->uvalue);
//     }else if(op=="ceil"){
//       lvalue = ceil(r1->lvalue);
//       uvalue = ceil(r1->uvalue);
    }else if(op=="~"){
      //bitwise negation operator (1's complement)
      lvalue = -((r1->uvalue)+1);
      uvalue = -((r1->lvalue)+1);
     }
  }else if(isit == 'd'){
      for (i = 1;i<cur_state->z->size;i++){
	if (cur_state->z->curClocks[i].enabled == index){
	  lvalue = cur_state->r->bound[cur_state->z->curClocks[i].enabled-nevents].lower;
	  uvalue = cur_state->r->bound[cur_state->z->curClocks[i].enabled-nevents].upper;
#ifdef __LHPN_EVAL__
	  printf("lv=%d,uv=%d,index=%d,i=%d\n",lvalue, uvalue,index,i);
#endif
	  break;
	}
      }
  }else{
    if ((isit == 'i')||(isit == 'c')){
      for (i = 1;i<cur_state->z->size;i++){
	if (cur_state->z->curClocks[i].enabled == index){
	  if (i>=cur_state->z->dbmEnd){
	    lvalue = -1*(cur_state->z->matrix[0][i]);
	    uvalue = cur_state->z->matrix[i][0];
	  }
	  else{// uses lower rate bound for both????
	    lvalue = -1*(cur_state->z->matrix[0][i])*
	      cur_state->r->bound[cur_state->z->curClocks[i].enabled-nevents].current;
	    uvalue = cur_state->z->matrix[i][0]*
	      cur_state->r->bound[cur_state->z->curClocks[i].enabled-nevents].current;
	  }
#ifdef __LHPN_EVAL__
	  printf("lv=%d,uv=%d,index=%d,i=%d\n",lvalue, uvalue,index,i);
#endif
	  break;
	}
      }
    }else if (isit == 'b'){
      log_val = cur_state->m->state[index];
      if (log_val == '1'){
	lvalue = 1;
	uvalue = 1;
      } else if (log_val == 'X'){
	lvalue = 0;
	uvalue = 1;
      } else if (log_val == '0'){
	lvalue = 0;
	uvalue = 0;
      }
#ifdef __LHPN_EVAL__
      printf("successful lookup of boolean %d,%c[%d,%d]\n",index,
	     cur_state->m->state[index],lvalue,uvalue);
#endif
    }else if ((isit == 'n')||(isit == 't')){
      // values already stored, no need to evaluate!
    }
  }    
};




/*****************************************************************************
 *****************************************************************************
 * Analysis algorithm functions
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Output initial messages to the screen and open the output file if
 * verbose is turned on.
 *****************************************************************************/
FILE *lhpnInitOutput(char *filename, signalADT *signals, int nsignals,
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
 * 
 *****************************************************************************/
void lhpnSetInitialPreds(signalADT *signals,eventADT *events,ineqList &ineqL,
			 int nevents,int nplaces,lhpnStateADT s)
{
  for(unsigned i=0;i<ineqL.size();i++) {
      ineq_update(ineqL[i],s,nevents);      
    /* > & >= */
    if(ineqL[i]->type == 0 || ineqL[i]->type == 1) {
      if(events[ineqL[i]->place]->lower >= ineqL[i]->constant) {
        s->m->state[ineqL[i]->signal] = '1';
      }
      else if(events[ineqL[i]->place]->upper >= ineqL[i]->constant) {
        warn("A predicate straddles an initial bound.");
        s->m->state[ineqL[i]->signal] = '1';
      }
      else {
        s->m->state[ineqL[i]->signal] = '0';
      }
    }
    /* < & <= */
    else if(ineqL[i]->type == 2 || ineqL[i]->type == 3) {
      if(events[ineqL[i]->place]->upper <= ineqL[i]->constant) {
        s->m->state[ineqL[i]->signal] = '1';
      }
      else if(events[ineqL[i]->place]->lower <= ineqL[i]->constant) {
        warn("A predicate straddles an initial bound.");
        s->m->state[ineqL[i]->signal] = '1';
      }
      else {
        s->m->state[ineqL[i]->signal] = '0';
      }
    }
    /* = */
    else if(ineqL[i]->type == 4) {
      s->m->state[ineqL[i]->signal] = '0';
      colorMsg(YELLOW,"NOTE: ");
      printf("inequality '=' found.\n");
    }
    /* > & >= */
    else if(ineqL[i]->type == 8 || ineqL[i]->type == 9) {
      /* TODO: push these statements into a function... */
      if(s->r->bound[ineqL[i]->place-nevents].current >= ineqL[i]->constant) {
        s->m->state[ineqL[i]->signal] = '1';
      }
      else if(s->r->bound[ineqL[i]->place-nevents].current >= ineqL[i]->constant){
        warn("A predicate straddles an initial bound.");
        s->m->state[ineqL[i]->signal] = '1';
      }
      else {
        s->m->state[ineqL[i]->signal] = '0';
      }
    }
    /* < & <= */
    else if(ineqL[i]->type == 10 || ineqL[i]->type == 11) {
      if(s->r->bound[ineqL[i]->place-nevents].current <= ineqL[i]->constant) {
        s->m->state[ineqL[i]->signal] = '1';
      }
      else if(s->r->bound[ineqL[i]->place-nevents].current <= ineqL[i]->constant){
        warn("A predicate straddles an initial bound.");
        s->m->state[ineqL[i]->signal] = '1';
      }
      else {
        s->m->state[ineqL[i]->signal] = '0';
      }
    }
      
#ifdef __LHPN_INIT__
    if (ineqL[i]->signal >= 0)
      printf("%s is initially %c\n",signals[ineqL[i]->signal]->name,
             s->m->state[ineqL[i]->signal]);
#endif
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
/* bool lhpnEnabledT(ruleADT **rules,lhpnMarkingADT m,int nevents,int nplaces, */
/*                   int t)  */
/* { */
/*   for(int i=nevents;i<nevents+nplaces;i++) { */
/*     if(validRule(rules[i][t]->ruletype) && */
/*        m->marking[rules[i][t]->marking] == 'F') { */
/*       return false; */
/*     } */
/*   } */
/*   return true; */
/* } */

/*****************************************************************************
 * Checks to see if a transition is enabled.
 *****************************************************************************/
int lhpnEnablingSatisfied(eventADT *events,int nsignals,int t,lhpnStateADT s,
			  int nevents)
{
  int i;
  bool satisfied;
  level_exp curLevel;

  if (events[t]->EXP!=NULL){
    events[t]->EXP->eval(s,nevents);
#ifdef __LHPN_EN__
    printf("%s\n",events[t]->event);
    printf("%s\n",events[t]->hsl);
    printf("%s,%d,%d\n",events[t]->event,events[t]->EXP->lvalue,events[t]->EXP->uvalue);
#endif
    if ((events[t]->EXP->lvalue==0)&&(events[t]->EXP->uvalue==0)){
      return 0;
    }
    if ((events[t]->EXP->lvalue==0)&&(events[t]->EXP->uvalue==1)){
      return -1;
    }
    if ((events[t]->EXP->lvalue==1)&&(events[t]->EXP->uvalue==1)){
      return 1;
    }
  }
  return 1;

  if(!events[t]->SOP) {
    return false;
  }

  for(curLevel=events[t]->SOP;curLevel;curLevel=curLevel->next) {
    satisfied=true;
    for(i=0;i<nsignals;i++) {
      if((curLevel->product[i]=='1' &&
          (s->m->state[i]=='0' || s->m->state[i]=='R')) ||
         (curLevel->product[i]=='0' &&
          (s->m->state[i]=='1' || s->m->state[i]=='F'))) {
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
 * 
 *****************************************************************************/
int lhpnEnabled(eventADT *events,int nsignals,int t,lhpnStateADT s,int nevents)
{
#ifdef __LHPN_EN__
  printf("Checking enabling of: %s - %c\n",events[t]->event,s->m->enablings[t]);
#endif
  if(s->m->enablings[t] == 'T') {	    
    //events[i]->nondisabling &&
    return lhpnEnablingSatisfied(events,nsignals,t,s,nevents);
  }
  else {
    return false;
  }
}

/*****************************************************************************
 * Recompute event delay
 *****************************************************************************/
void recomputeDelays(eventADT *events, int i, lhpnStateADT s, int nevents){
  if (events[i]->delayExprTree){
    events[i]->delayExprTree->eval(s,nevents);
    events[i]->lower=events[i]->delayExprTree->lvalue;
    events[i]->upper=events[i]->delayExprTree->uvalue;
#ifdef __LHPN_DELAYS__
    printf("%s [%d,%d]\n",events[i]->event,events[i]->lower,events[i]->upper);
#endif
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnFindInitialZ(ruleADT **rules,eventADT *events,int nevents,int nplaces,
		      int nsignals,lhpnStateADT s,ineqList ineqL,
		      signalADT *signals)
{
  s->z = initZ(1);
#ifdef __LHPN_EVAL__
  for (int i = 0;i<nsignals;i++){
    printf("signal %d = %s\n",i,signals[i]->name);
  }
#endif

  /* add the dummy transition */
  s->z->curClocks[0].enabled = 0;
  s->z->curClocks[0].enabling = 0;

  /* Add the continuous variables */
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(isVar(i,events)/* &&
			 r->bound[i-nevents].lower != 0*/) {
      addVarZ(s->z,i,events);
    }
  }
  for(int i=0;i<s->z->size;i++) {
    if(s->z->curClocks[i].enabling == -2) {
      if(i < s->z->dbmEnd &&
	 s->r->bound[s->z->curClocks[i].enabled-nevents].current == 0) {
        rmVarZ(s->z,s->z->curClocks[i].enabled);
	i--;
      }
    }
  }

#ifdef __LHPN_INIT__
  printf("Added the continuous variables\n");
  printZ(s->z,events);
#endif
  lhpnSetInitialPreds(signals,events,ineqL,nevents,nplaces,s);

  /* Add the initially enabled transitions, but ignore the reset transition. */
  for(int i=1;i<nevents;i++) {
    if(lhpnEnabled(events,nsignals,i,s,nevents)) {
      recomputeDelays(events,i,s,nevents);
      addTransZ(s->z,i,0,0);
    }
  }

#ifdef __LHPN_INIT__
  printf("Added the intially enabled transitions\n");
  printZ(s->z,events);
#endif
}

/*****************************************************************************
 * 
 *****************************************************************************/
void addRateState(lhpnRateADT r,lhpnZoneADT z,int nevents)
{
  for(int i=0;i<z->size;i++) {
    if(z->curClocks[i].enabling == -2) {
      z->curClocks[i].lrate = r->bound[z->curClocks[i].enabled-nevents].lower;
      z->curClocks[i].urate = r->bound[z->curClocks[i].enabled-nevents].upper;
    }
    else {
      z->curClocks[i].lrate = 1;
      z->curClocks[i].urate = 1;
    }
  }
}

/*****************************************************************************
 * rate  = rate of increase of common continuous variable
   type1,type2 = type of first inequality and second inequality, respectively
             0 = ">" 
             1 = ">="
             2 = "<"
             3 = "<="
             4 = "="
   const1,const2 = constant in the two inequalities
   returns 0 if first inequality comes first, 1 if second inequality comes
     first and 2 if they occur simultaneously
 *****************************************************************************/
int lhpnIsFirst(int rate,int type1,int const1,char val1,
		int type2,int const2,char val2) {

  /* TODO: Probably should not bother with equals */
  int incr;

  if (rate > 0) incr=1;
  else incr=0;

  if (const1 > const2) return incr;

  if (const2 > const1) return (1-incr);

  switch(type1) {
  case 0:
    switch(type2) {
    case 0:
    case 3:
      return 2;
    case 1:
    case 2:
      return incr;
    case 4:
      if ((incr==1) && (val2=='0'))
	return 1;
      if (((incr==1) && (val2=='1'))||
	  ((incr==0) && (val2=='0')))
	return 2;
    default:
      printf("WARNING: Unhandled inequality type %d\n",type2);
      return 2;
    }
  case 1:
    switch(type2) {
    case 0:
    case 3:
      return (1-incr);
    case 1:
    case 2:
      return 2;
    case 4:
      return 2;
    default:
      printf("WARNING: Unhandled inequality type %d\n",type2);
      return 2;
    }
  case 2:
    switch(type2) {
    case 0:
    case 3:
      return (1-incr);
    case 1:
    case 2:
      return 2;
    case 4:
      if ((incr==0) && (val2=='0'))
	return 1;
      if (((incr==1) && (val2=='0'))||
	  ((incr==0) && (val2=='1')))
	return 2;
    default:
      printf("WARNING: Unhandled inequality type %d\n",type2);
      return 2;
    }
  case 3:
    switch(type2) {
    case 0:
    case 3:
      return 2;
    case 1:
    case 2:
      return incr;
    case 4:
      return 2;
    default:
      printf("WARNING: Unhandled inequality type %d\n",type2);
      return 2;
    }
  case 4:
    switch(type2) {
    case 1:
    case 3:
    case 4:
      return 2;
    case 0:
      if ((incr==1) && (val1=='0'))
	return 0;
      if (((incr==1) && (val1=='1'))||
	  ((incr==0) && (val1=='0')))
	return 2;
    case 2:
      if ((incr==0) && (val1=='0'))
	return 0;
      if (((incr==1) && (val1=='0'))||
	  ((incr==0) && (val1=='1')))
	return 2;
    }
  default:
    printf("WARNING: Unhandled inequality type %d\n",type1);
    return 2;
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnAddSetItem(eventSets &E,lhpnEventADT e,ineqList &ineqL,lhpnZoneADT z,
                    lhpnRateADT r,eventADT *events,int nevents,
		    lhpnStateADT cur_state)
{
  int rv1l,rv1u,rv2l,rv2u,iZ,jZ;

#ifdef __LHPN_TRACE__
  printf("lhpnAddSetItem:begin()\n");
#endif
#ifdef __LHPN_ADD_ACTION__
  printf("Event sets entering:\n");
  printEventSets(E,events,ineqL);
  printf("Examining event: ");
  printLhpnEvent(e,events,ineqL);
#endif
  eventSet* eSet = new eventSet();
  eventSets* newE = new eventSets();
  bool done = false;
  bool possible = true;
  eventSets::iterator i;

  if ((e->t == -1) && (e->ineq == -1)) {
    eSet->insert(e);
    newE->push_back(*eSet);
    E.clear();
    E = *newE;
#ifdef __LHPN_ADD_ACTION__
    printf("Event sets leaving:\n");
    printEventSets(E,events,ineqL);
#endif
#ifdef __LHPN_TRACE__
    printf("lhpnAddSetItem:end()\n");
#endif
    return;
  }
  if (e->t == -1) {
    ineq_update(ineqL[e->ineq],cur_state,nevents);
    rv2l = chkDiv(-1 * ineqL[e->ineq]->constant,
                  r->bound[ineqL[e->ineq]->place-nevents].current,'C');
    rv2u = chkDiv(ineqL[e->ineq]->constant,
                  r->bound[ineqL[e->ineq]->place-nevents].current,'C');
    iZ = getIndexZ(z,-2,ineqL[e->ineq]->place);
  } else {
    iZ = getIndexZ(z,-1,e->t);
  }
  for(i=E.begin();i!=E.end()&&!done;i++) {
    eventSet* workSet = new eventSet();
    *workSet = copyEventSet(*i,events,ineqL);
    for(eventSet::iterator j=workSet->begin();j!=workSet->end();j++) {
      if (((*j)->t == -1) && ((*j)->ineq == -1)) continue;
      if ((*j)->t == -1) {
	ineq_update(ineqL[(*j)->ineq],cur_state,nevents);
        rv1l = chkDiv(-1 * ineqL[(*j)->ineq]->constant,
                      r->bound[ineqL[(*j)->ineq]->place-nevents].current,'C');
        rv1u = chkDiv(ineqL[(*j)->ineq]->constant,
                      r->bound[ineqL[(*j)->ineq]->place-nevents].current,'C');
        jZ = getIndexZ(z,-2,ineqL[(*j)->ineq]->place);
      } else {
        jZ = getIndexZ(z,-1,(*j)->t);
      }
      /* Both actions are predicate changes */
      if((e->t == -1) && ((*j)->t == -1)) {
        /* Both predicates are on the same variable */
        if(ineqL[(*j)->ineq]->place == ineqL[e->ineq]->place) {
          if (rv1l > rv2l) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 1a\n");
#endif
            possible = false;
          } else if (rv2l > rv1l) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 1b\n");
            printf("Adding to erase list: \n");
            printLhpnEvent(*j,events,ineqL);
#endif
            workSet->erase(*j);
          } else if (rv1u > rv2u) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 1c\n");
            printf("Adding to erase list: \n");
            printLhpnEvent(*j,events,ineqL);
#endif
            workSet->erase(*j);
          } else {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 1d\n");
#endif
            workSet->insert(e);
            done = true;
          }

        } 
        /* Predicates are on different variables */
        else {
          if(rv1l > rv2l + z->matrix[iZ][jZ]) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 2\n");
#endif
            possible = false;
          }
          else if(rv2l > rv1l + z->matrix[jZ][iZ]) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 3\n");
            printf("Adding to erase list: ");
            printLhpnEvent(*j,events,ineqL);
#endif
            workSet->erase(*j);
            //	    workSet->insert(e);
          }
          else if(rv1u > z->matrix[iZ][0] + z->matrix[jZ][iZ]) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 4\n");
            printf("Adding to erase list: \n");
            printLhpnEvent(*j,events,ineqL);
#endif
            workSet->erase(*j);
            //workSet->insert(e);
          }
          else if(rv2u > z->matrix[jZ][0] + z->matrix[iZ][jZ]) {
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 5\n");
#endif
            possible = false;
          }
          else if((rv1l == rv2l + z->matrix[iZ][jZ]) &&
                  (rv2l == rv1l + z->matrix[jZ][iZ]) &&
                  (rv1u == rv2u + z->matrix[jZ][iZ]) &&
                  (rv2u == rv1u + z->matrix[iZ][jZ])) {
            workSet->insert(e);
            done = true;
#ifdef __LHPN_ADD_ACTION__
            printf("Add action case 6\n");
#endif
          }
        }
        /* New action is predicate change, old is transition firing (case 3) */
      } else if((e->t == -1) && ((*j)->t != -1)) {
        if (rv2l > -events[(*j)->t]->lower + z->matrix[jZ][iZ]) {
#ifdef __LHPN_ADD_ACTION__
          printf("Add action case 6\n");
          printf("Adding to erase list: \n");
          printLhpnEvent(*j,events,ineqL);
#endif
          workSet->erase(*j);
        } else if (rv2u > z->matrix[jZ][0] + z->matrix[iZ][jZ]) {
#ifdef __LHPN_ADD_ACTION__
          printf("Add action case 7\n");
#endif
          possible = false;
        }
        /* TODO: One more ugly case, is it needed? */
        /* New action is transition firing, old is predicate change (case 4) */
      } else if((e->t != -1) && ((*j)->t == -1)) {
        if (rv1l > (((-1)*(events[e->t]->lower)) + z->matrix[iZ][jZ])) {
#ifdef __LHPN_ADD_ACTION__
          printf("Add action case 8\n");
#endif
          possible = false;
        } else if (rv1u > z->matrix[iZ][0] + z->matrix[jZ][iZ]) {
#ifdef __LHPN_ADD_ACTION__
          printf("Add action case 9\n");
          printf("Adding to erase list: \n");
          printLhpnEvent(*j,events,ineqL);
#endif
          workSet->erase(*j);
        } 
        /* TODO: one more ugly case, is it needed? */
      }
    }
    if (!(workSet->empty())) {
      newE->push_back(*workSet);
    }
  }
#ifdef __LHPN_ADD_ACTION__
  printf("At new for loop...\n");
#endif
  for(;i!=E.end();i++) {
    eventSet* addSet = new eventSet();
    *addSet = copyEventSet(*i,events,ineqL);
    newE->push_back(*addSet);
  }
#ifdef __LHPN_ADD_ACTION__
  printf("At done & possible...\n");
#endif
  if(!done && possible) {
    eSet->insert(e);
    newE->push_back(*eSet);
  }
  E.clear();
  E = *newE;
#ifdef __LHPN_ADD_ACTION__
  printf("Event sets leaving:\n");
  printEventSets(E,events,ineqL);
#endif
#ifdef __LHPN_TRACE__
  printf("lhpnAddSetItem:end()\n");
#endif
}

/*****************************************************************************
 * 
 *****************************************************************************/
bool lhpnPredCanChange(ineqADT ineq,lhpnZoneADT z,lhpnRateADT r,
                       lhpnMarkingADT m,eventADT *events,int nevents,
		       lhpnStateADT s)
{
  ineq_update(ineq,s,nevents);
  

#ifdef __LHPN_TRACE__
  printf("lhpnPredCanChange:begin()\n");
#endif
#ifdef __LHPN_PRED_DEBUG__
  printf("lhpnPredCanChange Examining: ");
  printI(ineq,events);
  printf("signal = %c, %d",s->m->state[ineq->signal],r->bound[ineq->place-nevents].current);
  printf("\n");
  if (r->bound[ineq->place-nevents].current != 0)
    printf("divRes: %d\n",chkDiv(ineq->constant,
				 r->bound[ineq->place-nevents].current,'F'));
#endif
  /* > or >= */
  if(ineq->type == 0 || ineq->type == 1) {
    int zoneP = getIndexZ(z,-2,ineq->place);
    if(zoneP == -1) {
      warn("An inequality produced a place not in the zone.");
      return false;
    }
    if(r->bound[ineq->place-nevents].current < 0 &&
       m->state[ineq->signal] == '1') {
      if((-1)*z->matrix[zoneP][0] <=
         (-1)*chkDiv(ineq->constant,r->bound[ineq->place-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCanChange:1\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return true;
      } else {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCannotChange:1\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return false;
      }
    }
    else if(r->bound[ineq->place-nevents].current > 0 &&
            m->state[ineq->signal] == '0') {
      if(z->matrix[zoneP][0] >=
         chkDiv(ineq->constant,r->bound[ineq->place-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCanChange:2\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return true;
      } else {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCannotChange:2\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
	return false;
      }
    }
    else {
#ifdef __LHPN_PRED_DEBUG__
      printf("predCannotChange:3\n");
      printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
      return false;
    }
  }
  /* < or <= */
  else if(ineq->type == 2 || ineq->type == 3) {
    int zoneP = getIndexZ(z,-2,ineq->place);
    if(zoneP == -1) {
      warn("An inequality produced a place not in the zone.");
      return false;
    }
    if(r->bound[ineq->place-nevents].current < 0 &&
       m->state[ineq->signal] == '0') {
      if((-1)*z->matrix[zoneP][0] <=
         (-1)*chkDiv(ineq->constant,r->bound[ineq->place-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCanChange:4\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return true;
      } else {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCannotChange:4\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return false;
      }
    }
    else if(r->bound[ineq->place-nevents].current > 0 &&
            m->state[ineq->signal] == '1') {
      if(z->matrix[zoneP][0] >=
         chkDiv(ineq->constant,r->bound[ineq->place-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCanChange:5\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return true;
      } else {
#ifdef __LHPN_PRED_DEBUG__
        printf("predCannotChange:5\n");
        printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
        return false;
      }
    }
    else {
#ifdef __LHPN_PRED_DEBUG__
      printf("predCanChange:6\n");
      printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
             m->state[ineq->signal]);
#endif
      return false;
    }
  }
#ifdef __LHPN_PRED_DEBUG__
  printf("predCanChange:7\n");
  printf("rate: %d state: %c\n",r->bound[ineq->place-nevents].current,
         m->state[ineq->signal]);
#endif
  return false;
}

/*****************************************************************************
 * 
 *****************************************************************************/
eventSets lhpnFindPossibleEvents(lhpnZoneADT z,lhpnRateADT r,
                                 lhpnMarkingADT m,eventADT *events,
                                 ineqList &ineqL,int nevents,
				 lhpnStateADT cur_state)
{
#ifdef __LHPN_TRACE__
  printf("lhpnFindPossibleEvents:begin()\n");
  printZ(z,events,nevents,r);
#endif
  eventSets E;

  /* Find rate events */
  for(int i=0;i<z->size;i++) {
    if(z->curClocks[i].enabling == -2 &&
       r->bound[z->curClocks[i].enabled-nevents].current != r->bound[z->curClocks[i].enabled-nevents].upper) {
      lhpnEventADT e = initEvent(-1,-1,z->curClocks[i].enabled,'r');
      lhpnAddSetItem(E,e,ineqL,z,r,events,nevents,cur_state);
    }
  }

  /* Find transition events */
  for(int i=0;i<z->dbmEnd;i++) {
    if(z->curClocks[i].enabling == -1 &&
       z->matrix[i][0] >= events[z->curClocks[i].enabled]->lower) {
      lhpnEventADT e = initEvent(z->curClocks[i].enabled,-1,-1,'T');
      lhpnAddSetItem(E,e,ineqL,z,r,events,nevents,cur_state);
    }
  }

  /* Find inequality events */
  for(unsigned i=0;i<ineqL.size();i++) {
    if(lhpnPredCanChange(ineqL[i],z,r,m,events,nevents,cur_state)) {
      lhpnEventADT e = initEvent(-1,i,-1,VAL(m->state[ineqL[i]->signal]));
      lhpnAddSetItem(E,e,ineqL,z,r,events,nevents,cur_state);
    }
  }
  
  return E;
}

/*****************************************************************************
 * 
 *****************************************************************************/
eventSet selectEvent(eventSets &E,eventADT *events,ineqList &ineqL)
{
#ifdef __LHPN_TRACE__
  printf("selectEvent:begin()\n");
#endif
#ifdef __LHPN_EVENTS__
  colorMsg(YELLOW,"Possible Events:\n");
  printEventSets(E,events,ineqL);
#endif
  eventSet selE;
  if(!E.empty()) {
/*     eventSets::iterator i=E.begin(); */
/*     selE = *i; */
/*     E.erase(i); */
    selE = E.back();
    E.pop_back();
  }
  return selE;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnDoAsgn(lhpnStateADT s,int t,ineqList &ineqL,eventADT *events,
                int nevents, int nplaces)
{// MINE: update to use new expressions. COPY STATE!!!!
#ifdef __LHPN_TRACE__
  printf("lhpnDoAsgn:begin()\n");
#endif
  lhpnStateADT sCopy = copyState(s,events,ineqL,nplaces);
  int lower,upper;

  for(ineqADT i=events[t]->inequalities;i!=NULL;i=i->next) {
    ineq_update(i,sCopy,nevents);
    /* variable asg */
    // should be removed from zone?????
    if(i->type == 5) {
      int j = getIndexZ(s->z,-2,i->place);
      if(j == -1) {
	printZ(s->z,events,nevents,s->r);
	printf("event:%s,ineq:%s\n",events[i->place]->event,i->expr);
        warn("Attempting to assign to  a variable not in the zone.");
        return;
      }
      /* TODO: Is it this simple for negative numbers & rates? */
      s->z->matrix[0][j] = -1 * i->constant;
      s->z->matrix[j][0] = i->uconstant;
      // set rates to 1
      s->r->oldBound[i->place-nevents].current = 1;
      s->r->oldBound[i->place-nevents].lower = 1;
      s->r->oldBound[i->place-nevents].upper = 1;

      /* Remove any relationships that may exist w/ other variables in
         the zone */
      for(int k=1;k<s->z->dbmEnd;k++) {
        if(j != k) {
          s->z->matrix[j][k] = INFIN;
          s->z->matrix[k][j] = INFIN;
        }
      }
#ifdef __LHPN_ASG__
      printf("Zone after variable assignment.\n");
      printZ(s->z,events,nevents,s->r);
#endif
    }
    /* boolean asg */
    else if(i->type == 7) {
      //printf("event:%s\n",events[t]->event);
      //MINE: calculate variables...
      if ((i->constant == 0) && (i->uconstant == 0)){
        s->m->state[i->place] = '0';
      }
      else if (((i->constant > 0) && (i->uconstant > 0))||
	       ((i->constant < 0) && (i->uconstant < 0))){
        s->m->state[i->place] = '1';
      }
      else {
        s->m->state[i->place] = 'X';
      }
    }
  }

  /* rate asg */
  for(ineqADT i=events[t]->inequalities;i!=NULL;i=i->next) {
    if(i->type == 6) {
      if(s->r->bound[i->place-nevents].current != i->constant) {
        s->r->bound[i->place-nevents].current = i->constant;
      }
      if(s->r->bound[i->place-nevents].lower != i->constant) {
        s->r->bound[i->place-nevents].lower = i->constant;
      }
      if(s->r->bound[i->place-nevents].upper != i->uconstant) {
        s->r->bound[i->place-nevents].upper = i->uconstant;
      }
    }
  }

  /* adjust the predicates for changes in variable values */
  for(unsigned k=0;k<ineqL.size();k++) {
    if(ineqL[k]->type >= 0 && ineqL[k]->type <= 3) {
      //get place from zone, dewarp
      int j = getIndexZ(s->z,-2,ineqL[k]->place);
      if(j == -1) {
	printZ(s->z,events,nevents,s->r);
	printf("event:%s,ineq:%s\n",
	       events[ineqL[k]->place]->event,ineqL[k]->expr);
        warn("Attempting to assign to  a variable not in the zone.");
        return;
      }
      // uses lower rate bound for both????
      if (j>=s->z->dbmEnd){
	lower = -1*(s->z->matrix[0][j]);
	upper = s->z->matrix[j][0];
      }
      else{// uses lower rate bound for both????
	lower = -1*(s->z->matrix[0][j])*//dewarp lower
	  s->r->oldBound[s->z->curClocks[j].enabled-nevents].current;
	upper = (s->z->matrix[j][0])*s//dewarp upper
	  ->r->oldBound[s->z->curClocks[j].enabled-nevents].current;
      }
      ineq_update(ineqL[k],sCopy,nevents);
      int old_constant = ineqL[k]->constant;
      int old_uconstant = ineqL[k]->uconstant;
      ineq_update(ineqL[k],s,nevents);
      if ((old_constant != ineqL[k]->constant)||
	  (old_uconstant != ineqL[k]->uconstant)||
	  (s->z->matrix[0][j] != sCopy->z->matrix[0][j])||
	  (s->z->matrix[j][0] != sCopy->z->matrix[j][0])){
	/* > & >= */
	if(ineqL[k]->type == 0 || ineqL[k]->type == 1) {
	  if(lower >= ineqL[k]->uconstant) {
	    s->m->state[ineqL[k]->signal] = '1';
	  }
	  else if (ineqL[k]->constant >= upper){
	    s->m->state[ineqL[k]->signal] = '0';
	  }
	  else {
	    s->m->state[ineqL[k]->signal] = 'X';
	  }
	}

	/* < & <= */
	else if(ineqL[k]->type == 2 || ineqL[k]->type == 3) {
	  if(upper <= ineqL[k]->constant) {
	    s->m->state[ineqL[k]->signal] = '1';
	  }
	  else if (ineqL[k]->uconstant <= lower){
	    s->m->state[ineqL[k]->signal] = '0';
	  }
	  else {
	    s->m->state[ineqL[k]->signal] = 'X';
	  }
	}
      }
    }
  }

  freeState(sCopy,nplaces);
  recanonZ(s->z);
#ifdef __LHPN_ASG__
  printf("Zone leaving lhpnDoAsgn.\n");
  printZ(s->z,events,nevents,s->r);
#endif
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnDbmWarp(lhpnStateADT s,eventADT *events,int nevents)
{
#ifdef __LHPN_TRACE__
  printf("lhpnDbmWarp:begin()\n");
#endif
  /* TODO: This appears to NOT work when INFIN is in the bounds?
     Should I have to worry about this case? */
  for(int i=1;i<s->z->dbmEnd;i++) {
    for(int j=i+1;j<s->z->dbmEnd;j++) {
      double iVal = 0.0;
      double jVal = 0.0;
      double iWarp = 0;
      double jWarp = 0;
      double iXDot = 0;
      double jXDot = 0;

      /* deal w/ the fact that we might have continuous and discrete
         places */
#ifdef __LHPN_DEBUG_WARP__
      printf("Working on %d->%d\n",i,j);
#endif
      if(s->z->curClocks[i].enabling == -2) {
        iVal = fabs((double)s->r->oldBound[s->z->curClocks[i].enabled-nevents].current /
                    (double)s->r->bound[s->z->curClocks[i].enabled-nevents].current);
        iWarp = fabs((double)s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
        iXDot = fabs((double)s->r->bound[s->z->curClocks[i].enabled-nevents].current);
      }
      else {
        iVal = 1.0;
        iWarp = 1.0;
        iXDot = 1.0;
      }
      
      if(s->z->curClocks[j].enabling == -2) {
        jVal = fabs((double)s->r->oldBound[s->z->curClocks[j].enabled-nevents].current /
                    (double)s->r->bound[s->z->curClocks[j].enabled-nevents].current);
        jWarp = fabs((double)s->r->oldBound[s->z->curClocks[j].enabled-nevents].current);
        jXDot = fabs((double)s->r->bound[s->z->curClocks[j].enabled-nevents].current);
      }
      else {
        jVal = 1.0;
        jWarp = 1.0;
        jXDot = 1.0;
      }

#ifdef __LHPN_DEBUG_WARP__
      printf("iVal: %f, jVal: %f, iWarp: %f, jWarp: %f, iXDot: %f, jXDot: %f\n",iVal,jVal,iWarp,jWarp,iXDot,jXDot);
/*       printf("calc1- jWarp:%d * s->z->matrix[i][j]:%d / jXDot:%d + (-1 * jWarp:%d * s->z->matrix[i][0]:%d) / jXDot:%d + (iWarp:%d * s->z->matrix[i][0]:%d) / iXDot:%d = %d 1:%d 2:%d 3:%d -- %d\n", jWarp,s->z->matrix[i][j],jXDot,jWarp,s->z->matrix[i][0],jXDot,iWarp,s->z->matrix[i][0],iXDot,(chkDiv((jWarp * s->z->matrix[i][j]),jXDot,'C') + chkDiv((-1 * jWarp * s->z->matrix[i][0]),jXDot,'C') + chkDiv((iWarp * s->z->matrix[i][0]),iXDot,'C')),chkDiv((jWarp * s->z->matrix[i][j]),jXDot,'C'),chkDiv((-1 * jWarp * s->z->matrix[i][0]),jXDot,'C'),chkDiv((iWarp * s->z->matrix[i][0]),iXDot,'C'),(int)ceil(((jWarp * s->z->matrix[i][j])/jXDot) +((-1 * jWarp * s->z->matrix[i][0])/jXDot) + ((iWarp * s->z->matrix[i][0])/iXDot))); */
/*       printf("calc2-jWarp:%f * s->z->matrix[j][i]):%d/jXDot:%f) + ((-1 * jWarp:%f * s->z->matrix[0][i]:%d)/jXDot:%f) + ((iWarp:%f * s->z->matrix[0][i]):%d,iXDot:%f)) = %d 1:%f 2:%f 3:%f\n",jWarp,s->z->matrix[j][i],jXDot,jWarp,s->z->matrix[0][i],jXDot,iWarp,s->z->matrix[0][i],iXDot,(int) ceil(((jWarp * s->z->matrix[j][i])/jXDot) + ((-1 * jWarp * s->z->matrix[0][i])/jXDot) + ((iWarp * s->z->matrix[0][i]),iXDot)),((jWarp * (double)s->z->matrix[j][i])/jXDot),((-1 * jWarp * (double)s->z->matrix[0][i])/jXDot),(iWarp * (double)s->z->matrix[0][i])/iXDot); */
#endif
      
      if(iVal > jVal) {
/*         s->z->matrix[i][j] = */
/*           chkDiv((jWarp * s->z->matrix[i][j]),jXDot,'C') + */
/*           chkDiv((-1 * jWarp * s->z->matrix[i][0]),jXDot,'C') + */
/*           chkDiv((iWarp * s->z->matrix[i][0]),iXDot,'C'); */
/*         s->z->matrix[j][i] = */
/*           chkDiv((jWarp * s->z->matrix[j][i]),jXDot,'C') + */
/*           chkDiv((-1 * jWarp * s->z->matrix[0][i]),jXDot,'C') + */
/*           chkDiv((iWarp * s->z->matrix[0][i]),iXDot,'C'); */
        s->z->matrix[i][j] = (int)
          ceil(((jWarp * s->z->matrix[i][j])/jXDot) +
               ((-1 * jWarp * s->z->matrix[i][0])/jXDot) +
               ((iWarp * s->z->matrix[i][0])/iXDot));
        s->z->matrix[j][i] = (int)
          ceil(((jWarp * s->z->matrix[j][i])/jXDot) +
               ((-1 * jWarp * s->z->matrix[0][i])/jXDot) +
               ((iWarp * s->z->matrix[0][i])/iXDot));
      }
      else {
/*         s->z->matrix[j][i] = */
/*           chkDiv((iWarp * s->z->matrix[j][i]),iXDot,'C') + */
/*           chkDiv((-1 * iWarp * s->z->matrix[j][0]),iXDot,'C') + */
/*           chkDiv((jWarp * s->z->matrix[j][0]),jXDot,'C'); */
/*         s->z->matrix[i][j] = */
/*           chkDiv((iWarp * s->z->matrix[i][j]),iXDot,'C') + */
/*           chkDiv((-1 * iWarp * s->z->matrix[0][j]),iXDot,'C') + */
/*           chkDiv((jWarp * s->z->matrix[0][j]),jXDot,'C'); */
        s->z->matrix[j][i] = (int)
          ceil(((iWarp * s->z->matrix[j][i])/iXDot) +
               ((-1 * iWarp * s->z->matrix[j][0])/iXDot) +
               ((jWarp * s->z->matrix[j][0])/jXDot));
        s->z->matrix[i][j] = (int)
          ceil(((iWarp * s->z->matrix[i][j])/iXDot) +
               ((-1 * iWarp * s->z->matrix[0][j])/iXDot) +
               ((jWarp * s->z->matrix[0][j])/jXDot));
      }
    }
  }

#ifdef __LHPN_DEBUG_WARP__
  printf("After fixing up initial warp conditions.\n");
  printZ(s->z,events,nevents,s->r);
#endif
  
  for(int i=1;i<s->z->dbmEnd;i++) {
    if(s->z->curClocks[i].enabling == -2) {
#ifdef __LHPN_DEBUG_WARP__
      printf("old:%d new:%d v1:%d v2:%d\n",s->r->oldBound[s->z->curClocks[i].enabled-nevents].current,s->r->bound[s->z->curClocks[i].enabled-nevents].current,s->z->matrix[0][i],s->z->matrix[i][0]);
#endif
      if(abs(s->z->matrix[0][i]) != INFIN) {
      s->z->matrix[0][i] =
        chkDiv((abs(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current)
                * s->z->matrix[0][i]),
               abs(s->r->bound[s->z->curClocks[i].enabled-nevents].current)
               ,'C');
      }
      if(abs(s->z->matrix[i][0]) != INFIN) {
      s->z->matrix[i][0] =
        chkDiv((abs(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current)
                * s->z->matrix[i][0]),
               abs(s->r->bound[s->z->curClocks[i].enabled-nevents].current)
               ,'C');
      }
    }
  }

#ifdef __LHPN_DEBUG_WARP__
  printf("After fixing up places.\n");
  printZ(s->z,events,nevents,s->r);
#endif

  for(int i=1;i<s->z->dbmEnd;i++) {
    if(s->z->curClocks[i].enabling == -2) {
#ifdef __LHPN_DEBUG_WARP__
      printf("Warp: %d\n",s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
#endif
      if(((float)s->r->oldBound[s->z->curClocks[i].enabled-nevents].current /
          (float)s->r->bound[s->z->curClocks[i].enabled-nevents].current) < 0.0) {
        /* swap */
        int temp = s->z->matrix[0][i];
        s->z->matrix[0][i] = s->z->matrix[i][0];
        s->z->matrix[i][0] = temp;
      
        for(int j=1;j<s->z->dbmEnd;j++) {
          /* TBD: If i & j are both changing direction do we need to
             remove the warp info? */
          if(i != j) {
            s->z->matrix[j][i] = INFIN;
            s->z->matrix[i][j] = INFIN;
          }
        }
      }   
    }
  }

#ifdef __LHPN_DEBUG_WARP__
  printf("After handling negative warps.\n");
  printZ(s->z,events,nevents,s->r);
#endif

    for(int i=1;i<s->z->dbmEnd;i++) {
    if(s->z->curClocks[i].enabling == -2) {
      int newCwarp = s->r->bound[s->z->curClocks[i].enabled-nevents].current;
      int newLwarp = s->r->bound[s->z->curClocks[i].enabled-nevents].lower;
      int newUwarp = s->r->bound[s->z->curClocks[i].enabled-nevents].upper;
      s->r->oldBound[s->z->curClocks[i].enabled-nevents].current = newCwarp;
      s->r->oldBound[s->z->curClocks[i].enabled-nevents].lower = newLwarp;
      s->r->oldBound[s->z->curClocks[i].enabled-nevents].upper = newUwarp;
        
#ifdef __LHPN_DEBUG_WARP__
      printf("New warp for %d: %d\n",i,s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
#endif
    }
  }

#ifdef __LHPN_DEBUG_WARP__
  printf("Before recanon.\n");
  printZ(s->z,events,nevents,s->r);
#endif
  recanonZ(s->z);
#ifdef __LHPN_DEBUG_WARP__
  printf("After recanon.\n");
  printZ(s->z,events,nevents,s->r);
#endif
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnFireTransition(lhpnStateADT s,int t,ineqList &ineqL,eventADT *events,
                        ruleADT **rules,markkeyADT *markkey,int nevents,
                        int nplaces,int nrules,int nsignals)
{
#ifdef __LHPN_TRACE__
  printf("lhpnFireTransition:begin()\n");
#endif
#ifdef __LHPN_FIRET__
  printf("Firing:%s\n",events[t]->event);
#endif
  for(int i=0;i<nevents+nplaces;i++) {
    /* Remove marking from places rules losing a token */
    if(rules[i][t]->ruletype > NORULE) {
      for(int j=1;j<nevents;j++) {
        if(rules[i][j]->ruletype > NORULE) {
/* #ifdef __LHPN_FIRET__ */
/*           printf("Unmarking %s->%s\n",events[i]->event, */
/*                  events[j]->event); */
/* #endif */
          s->m->marking[rules[i][j]->marking] = 'F';
        }
      }
    }
  }
  updateM(s->m,events,rules,markkey,nrules,nevents);
  for(int i=1;i<nevents;i++) {
    if(isMemberZ(-1,i,s->z) &&
       ((s->m->enablings[i] != 'T') || 
	(!events[i]->nondisabling && !lhpnEnabled(events,nsignals,i,s,nevents)))) {
#ifdef __LHPN_UPDATE_STATE__
      printf("Removing transition: %s\n",events[i]->event);
#endif
      rmTransZ(s->z,i);
    }
  }
  for(int i=0;i<nevents+nplaces;i++) {
    /* Add marking to the places gaining a token */
    if(rules[t][i]->ruletype > NORULE) {
      for(int j=1;j<nevents;j++) {
        if(rules[i][j]->ruletype > NORULE) {
          if(s->m->marking[rules[i][j]->marking] == 'T') {
            warn("Safety violation.");
          }
/* #ifdef __LHPN_FIRET__ */
/*           printf("Marking %s->%s\n",events[i]->event,events[j]->event); */
/* #endif */
          s->m->marking[rules[i][j]->marking] = 'T';
        }
      }
    }
  }
  updateM(s->m,events,rules,markkey,nrules,nevents);
  //rmTransZ(s->z,t);
  lhpnDoAsgn(s,t,ineqL,events,nevents,nplaces);

  /* Reset current rates to their lower bounds */
  for(int i=0;i<s->z->size;i++) {
    if(s->z->curClocks[i].enabling == -2) {
      s->r->bound[s->z->curClocks[i].enabled-nevents].current =
	s->r->bound[s->z->curClocks[i].enabled-nevents].lower;
    }
  } 

  for(int i=0;i<s->z->size;i++) {
    if(s->z->curClocks[i].enabling == -2) {
      if(i >= s->z->dbmEnd &&
         s->r->bound[s->z->curClocks[i].enabled-nevents].current != 0) {
        s->r->oldBound[s->z->curClocks[i].enabled-nevents].current=1;
        s->r->oldBound[s->z->curClocks[i].enabled-nevents].lower=1;
        s->r->oldBound[s->z->curClocks[i].enabled-nevents].upper=1;
        addVarZ(s->z,s->z->curClocks[i].enabled);
        i--;
      }
      else if(i < s->z->dbmEnd &&
              s->r->bound[s->z->curClocks[i].enabled-nevents].current == 0) {
#ifdef __LHPN_FIRET__
        printf("Old rate:%d Current rate:%d\n",s->r->oldBound[s->z->curClocks[i].enabled-nevents].current,
	       s->r->bound[s->z->curClocks[i].enabled-nevents].current);
        
        printf("VAR: Removing var: %s\n",events[s->z->curClocks[i].enabled]->event);
#endif
        /* Before removing the item from the zone update the rate b/c
           dbmWarp won't handle it if the variable is out of the
           zone...also, swap the values for negative rates b/c the
           rmVarZ function is dumb and doesn't do that */
        s->z->matrix[i][0] *= abs(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
        s->z->matrix[0][i] *= abs(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
        if(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current < 0) {
          int rTemp = s->z->matrix[i][0];
          s->z->matrix[i][0] = s->z->matrix[0][i];
          s->z->matrix[0][i] = rTemp;
        }
        rmVarZ(s->z,s->z->curClocks[i].enabled);
        i--;
      }
    }
  }
  /* TODO: only strictly necessary if some variable is added */
  recanonZ(s->z);
#ifdef __LHPN_FIRET__
  printf("preWarp\n");
  printZ(s->z,events,nevents,s->r);
#endif
  lhpnDbmWarp(s,events,nevents);
#ifdef __LHPN_FIRET__
  printf("postWarp\n");
  printZ(s->z,events,nevents,s->r);
#endif
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnUpdatePred(lhpnStateADT s,int i,ineqList &ineqL)
{
#ifdef __LHPN_TRACE__
  printf("lhpnUpdatePred:begin()\n");
#endif
  if(s->m->state[ineqL[i]->signal] == '0') {
    s->m->state[ineqL[i]->signal] = '1';
  }
  else {
    s->m->state[ineqL[i]->signal] = '0';
  }
}

/*****************************************************************************
 * Fire a rate event
 *****************************************************************************/
void lhpnRateEvent(lhpnStateADT s,int rate,ineqList &ineqL,eventADT *events,
		   int nevents,int nplaces)
{
  lhpnStateADT sCopy = copyState(s,events,ineqL,nplaces);
  int lower,upper;

#ifdef __LHPN_TRACE__
  printf("lhpnRateEvent:begin()\n");
#endif
#ifdef __LHPN_RATE_EVENT__
  printf("Firing rate event: %s\n",events[rate]->event);
#endif

  /* Set current rates to their upper bounds */
  /*
  for(int i=0;i<s->z->size;i++) {
    if(s->z->curClocks[i].enabling == -2) {
      s->r->bound[s->z->curClocks[i].enabled-nevents].current =
	s->r->bound[s->z->curClocks[i].enabled-nevents].upper;
    }
    } 
  */
  s->r->bound[rate-nevents].current = s->r->bound[rate-nevents].upper;
  /* adjust the predicates for changes in variable values */
  for(unsigned k=0;k<ineqL.size();k++) {
    if(ineqL[k]->type >= 0 && ineqL[k]->type <= 3) {
      //get place from zone, dewarp
      int j = getIndexZ(s->z,-2,ineqL[k]->place);
      if(j == -1) {
	printZ(s->z,events,nevents,s->r);
	printf("event:%s,ineq:%s\n",
	       events[ineqL[k]->place]->event,ineqL[k]->expr);
        warn("Attempting to assign to  a variable not in the zone.");
        return;
      }
      if (j>=s->z->dbmEnd){
	lower = -1*(s->z->matrix[0][j]);
	upper = s->z->matrix[j][0];
      }
      else{
	lower = -1*(s->z->matrix[0][j])*//dewarp lower
	  s->r->oldBound[s->z->curClocks[j].enabled-nevents].current;
	upper = (s->z->matrix[j][0])*s//dewarp upper
	  ->r->oldBound[s->z->curClocks[j].enabled-nevents].current;
      }
      ineq_update(ineqL[k],sCopy,nevents);
      int old_constant = ineqL[k]->constant;
      int old_uconstant = ineqL[k]->uconstant;
      ineq_update(ineqL[k],s,nevents);
      if ((old_constant != ineqL[k]->constant)||
	  (old_uconstant != ineqL[k]->uconstant)){
	/* > & >= */
	if(ineqL[k]->type == 0 || ineqL[k]->type == 1) {
	  if(lower >= ineqL[k]->uconstant) {
	    s->m->state[ineqL[k]->signal] = '1';
	  }
	  else if (ineqL[k]->constant >= upper){
	    s->m->state[ineqL[k]->signal] = '0';
	  }
	  else {
	    s->m->state[ineqL[k]->signal] = 'X';
	  }
	}

	/* < & <= */
	else if(ineqL[k]->type == 2 || ineqL[k]->type == 3) {
	  if(upper <= ineqL[k]->constant) {
	    s->m->state[ineqL[k]->signal] = '1';
	  }
	  else if (ineqL[k]->uconstant <= lower){
	    s->m->state[ineqL[k]->signal] = '0';
	  }
	  else {
	    s->m->state[ineqL[k]->signal] = 'X';
	  }
	}
      }
    }
  }
  //lhpnDoAsgn(s,t,ineqL,events,nevents,nplaces);

  for(int i=0;i<s->z->size;i++) {
    if(s->z->curClocks[i].enabling == -2) {
      if(i >= s->z->dbmEnd &&
         s->r->bound[s->z->curClocks[i].enabled-nevents].current != 0) {
        s->r->oldBound[s->z->curClocks[i].enabled-nevents].current=1;
        s->r->oldBound[s->z->curClocks[i].enabled-nevents].lower=1;
        s->r->oldBound[s->z->curClocks[i].enabled-nevents].upper=1;
        addVarZ(s->z,s->z->curClocks[i].enabled);
        i--;
      }
      else if(i < s->z->dbmEnd &&
              s->r->bound[s->z->curClocks[i].enabled-nevents].current == 0) {
#ifdef __LHPN_RATE_EVENT__
        printf("Old rate:%d Current rate:%d\n",s->r->oldBound[s->z->curClocks[i].enabled-nevents].current,
	       s->r->bound[s->z->curClocks[i].enabled-nevents].current);
        
        printf("VAR: Removing var: %s\n",events[s->z->curClocks[i].enabled]->event);
#endif
        /* Before removing the item from the zone update the rate b/c
           dbmWarp won't handle it if the variable is out of the
           zone...also, swap the values for negative rates b/c the
           rmVarZ function is dumb and doesn't do that */
        s->z->matrix[i][0] *= abs(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
        s->z->matrix[0][i] *= abs(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current);
        if(s->r->oldBound[s->z->curClocks[i].enabled-nevents].current < 0) {
          int rTemp = s->z->matrix[i][0];
          s->z->matrix[i][0] = s->z->matrix[0][i];
          s->z->matrix[0][i] = rTemp;
        }
        rmVarZ(s->z,s->z->curClocks[i].enabled);
        i--;
      }
    }
  }
  /* TODO: only strictly necessary if some variable is added */
  recanonZ(s->z);
#ifdef __LHPN_RATE_EVENT__
  printf("preWarp\n");
  printZ(s->z,events,nevents,s->r);
#endif
  lhpnDbmWarp(s,events,nevents);
#ifdef __LHPN_RATE_EVENT__
  printf("postWarp\n");
  printZ(s->z,events,nevents,s->r);
#endif
}

/*****************************************************************************
 * Update max advance based on predicate values.
 *****************************************************************************/
int lhpnCheckPreds(int p,ineqList &ineqL,lhpnStateADT s,ruleADT **rules,
                   int nevents,eventADT *events)
{
#ifdef __LHPN_TRACE__
  printf("lhpnCheckPreds:begin()\n");
#endif
  int min = INFIN;
  int newMin = INFIN;
  int zoneP = getIndexZ(s->z,-2,p);
  for(unsigned i=0;i<ineqL.size();i++) {
    if(ineqL[i]->type > 4) {
      continue;
    }
#ifdef __LHPN_PRED_DEBUG__
    printf("Zone to check...\n");
    printZ(s->z,events,nevents,s->r);
    printf("Checking ...");
    printI(ineqL[i],events);
    printf("\n");
#endif
    if(ineqL[i]->place == p) {
      ineq_update(ineqL[i],s,nevents);
      if(ineqL[i]->type <= 1) {
        /* Working on a > or >= ineq */
        if(s->r->bound[p-nevents].current > 0) {
          if(s->m->state[ineqL[i]->signal]=='1') {
            if(s->z->matrix[zoneP][0] <
               chkDiv(ineqL[i]->constant,
                      s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 1a\n");
#endif
              
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 1.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else if((-1)*s->z->matrix[0][zoneP] >
                    chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 2a\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              s->r->bound[p-nevents].current,'F');
            }
            else {
              /* straddle case */
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 3a\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              s->r->bound[p-nevents].current,'F');
            }
          }
          else {
            if(s->z->matrix[zoneP][0] <
               chkDiv(ineqL[i]->constant,
                      s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 4a -- min: %d\n",chkDiv(ineqL[i]->constant,s->r->bound[p-nevents].current,'F'));
#endif
              newMin = chkDiv(ineqL[i]->constant,
                              s->r->bound[p-nevents].current,'F');
            }
            else if((-1)*s->z->matrix[0][zoneP] >
                    chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 5a\n");
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 3.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 6a -- min: %d\n",s->z->matrix[zoneP][0]);
#endif
              /* straddle case */
              newMin = s->z->matrix[zoneP][0];
            }
          }
        }
        else {
          /* warp <= 0 */
          if(s->m->state[ineqL[i]->signal]=='1') {
            if(s->z->matrix[0][zoneP] <
               (-1)*chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 7a\n");
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 2.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else if((-1)*s->z->matrix[zoneP][0] >
                    (-1)*chkDiv(ineqL[i]->constant,
                                s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 8a\n");
#endif
              newMin = chkDiv(ineqL[i]->constant,
                              s->r->bound[p-nevents].current,'F');
            }
            else {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 9a\n");
#endif
              /* straddle case */
              newMin = s->z->matrix[zoneP][0];
            }
          }
          else {
            if(s->z->matrix[0][zoneP] <
               (-1)*chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 10a\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              s->r->bound[p-nevents].current,'F');
            }
            else if((-1)*s->z->matrix[zoneP][0] >
                    (-1)*chkDiv(ineqL[i]->constant,
                                s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 11a\n");
	      printf("z=%d c=%d b=%d\n",
		     s->z->matrix[zoneP][0],
		     ineqL[i]->constant,
		     s->r->bound[p-nevents].current);
		     
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 4.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else {
              /* straddle case */
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 12a\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              s->r->bound[p-nevents].current,'F');
            }
          }
        }
      }
      else {
        /* Working on a < or <= ineq */
        if(s->r->bound[p-nevents].current > 0) {
          if(s->m->state[ineqL[i]->signal]=='1') {
            if(s->z->matrix[zoneP][0] <
               chkDiv(ineqL[i]->constant,
                      s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 1b -- min: %d\n",chkDiv(ineqL[i]->constant,s->r->bound[p-nevents].current,'F'));
#endif
              newMin = chkDiv(ineqL[i]->constant,
                              s->r->bound[p-nevents].current,'F');
            }
            else if((-1)*s->z->matrix[0][zoneP] >
                    chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 2b\n");
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 5.\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              s->r->bound[p-nevents].current,'F');
            }
            else {
              /* straddle case */
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 3b -- min: %d\n",s->z->matrix[zoneP][0]);
#endif
              newMin = s->z->matrix[zoneP][0];
            }
          }
          else {
            if(s->z->matrix[zoneP][0] <
               chkDiv(ineqL[i]->constant,
                      s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 4b\n");
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 7.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else if((-1)*s->z->matrix[0][zoneP] >
                    chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 5b\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              s->r->bound[p-nevents].current,'F');
            }
            else {
              /* straddle case */
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 6b\n");
#endif
              newMin = chkDiv(events[p]->urange,
                              s->r->bound[p-nevents].current,'F');
            }
          }
        }
        else {
          /* warp <= 0 */
          if(s->m->state[ineqL[i]->signal]=='1') {
            if(s->z->matrix[0][zoneP] <
               (-1)*chkDiv(ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 7b\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              s->r->bound[p-nevents].current,'F');
            }
            else if((-1)*s->z->matrix[zoneP][0] >
                    (-1)*chkDiv(ineqL[i]->constant,
                                s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 8b\n");
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 8.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else {
              /* straddle case */
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 9b\n");
#endif
              newMin = chkDiv(events[p]->lrange,
                              s->r->bound[p-nevents].current,'F');
            }
          }
          else {
            if(s->z->matrix[0][zoneP] <
               chkDiv((-1)*ineqL[i]->constant,
                           s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 10b\n");
              printf("zone: %d const: %d warp: %d chkDiv: %d\n",s->z->matrix[0][zoneP],ineqL[i]->constant,s->r->bound[p-nevents].current,chkDiv((-1)*ineqL[i]->constant,s->r->bound[p-nevents].current,'F'));
#endif
#ifdef __LHPN_WARN__
              warn("checkPreds: Impossible case 6.\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
            else if((-1)*s->z->matrix[zoneP][0] >
                    (-1)*chkDiv(ineqL[i]->constant,
                                s->r->bound[p-nevents].current,'F')) {
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 11b\n");
#endif
              newMin = chkDiv(ineqL[i]->constant,
                              s->r->bound[p-nevents].current,'F');
            }
            else {
              /* straddle case */
#ifdef __LHPN_PRED_DEBUG__
              printf("CP:case 12b\n");
#endif
              newMin = s->z->matrix[zoneP][0];
            }
          }
        }
      }
    }
    if(newMin < min) {
      min = newMin;
    }
  }

#ifdef __LHPN_PRED_DEBUG__
  printf("Min leaving checkPreds for %s: %d\n",events[p]->event,min);
#endif
  return min;
}

/*****************************************************************************
 * 
 *****************************************************************************/
void advanceTime(lhpnStateADT s,eventADT *events,ruleADT **rules,
                 ineqList &ineqL,int nevents)
{
#ifdef __LHPN_TRACE__
  printf("advanceTime:begin()\n");
#endif
  for(int i=1;i<s->z->dbmEnd;i++) {
    if(s->z->curClocks[i].enabling == -1) {
      s->z->matrix[i][0] = events[s->z->curClocks[i].enabled]->upper;
    }
    else if(s->z->curClocks[i].enabling == -2) {
      s->z->matrix[i][0] = lhpnCheckPreds(s->z->curClocks[i].enabled,ineqL,s,
                                          rules,nevents,events);
    }
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnDoEvents(lhpnStateADT s,eventSet &E,eventADT *events,
                  ruleADT **rules,ineqList &ineqL,markkeyADT *markkey,
                  int nevents,int nplaces,int nrules,int nsignals)
{
  int ans;

#ifdef __LHPN_TRACE__
  printf("lhpnDoEvents:begin()\n");
#endif
  for(eventSet::iterator i=E.begin();i!=E.end();i++) {
    if((*i)->ineq >= 0) {
      ineq_update(ineqL[(*i)->ineq],s,nevents);

      int j = getIndexZ(s->z,-2,ineqL[(*i)->ineq]->place);
      s->z->matrix[0][j] = chkDiv(-1 * ineqL[(*i)->ineq]->constant,
                                  s->r->bound[ineqL[(*i)->ineq]->place-nevents].current,'C');
      ans = chkDiv(ineqL[(*i)->ineq]->constant,
                   s->r->bound[ineqL[(*i)->ineq]->place-nevents].current,'C');
      if (s->z->matrix[j][0] < ans) {
        s->z->matrix[j][0] = ans;
      }
    }
  }
  advanceTime(s,events,rules,ineqL,nevents);
  recanonZ(s->z);

  for(eventSet::iterator i=E.begin();i!=E.end();i++) {
    if((*i)->ineq >= 0) {
      lhpnUpdatePred(s,(*i)->ineq,ineqL);
    }
    else if((*i)->t >= 0) {
      lhpnFireTransition(s,(*i)->t,ineqL,events,rules,markkey,nevents,nplaces,nrules,nsignals);
    } else {
      lhpnRateEvent(s,(*i)->rate,ineqL,events,nevents,nplaces);
    }
  }
}

/*****************************************************************************
 * 
 *****************************************************************************/
void lhpnUpdateState(lhpnStateADT s,eventSet &E,eventADT *events,
                     ruleADT **rules,ineqList &ineqL,markkeyADT *markkey,
                     signalADT *signals,int nevents,int nplaces,int nsignals,
                     int nrules)
{//MINE: update to use new expressions
#ifdef __LHPN_TRACE__
  printf("lhpnUpdateState:begin()\n");
#endif
#ifdef __LHPN_UPDATE_STATE__
  printM(s->m,events,signals,markkey,ineqL,nevents,nrules,nsignals);
  printZ(s->z,events,nevents,s->r);
#endif
  restrictZ(s->z,E,events,ineqL,s->r,nevents,s);
  recanonZ(s->z);
#ifdef __LHPN_UPDATE_STATE__
  printf("Zone after restrict.\n");
  printZ(s->z,events,nevents,s->r);
#endif
  lhpnDoEvents(s,E,events,rules,ineqL,markkey,nevents,nplaces,nrules,nsignals);
#ifdef __LHPN_UPDATE_STATE__
  printf("Zone after lhpnDoEvents.\n");
  printZ(s->z,events,nevents,s->r);
#endif
  for(int i=1;i<nevents;i++) {
    if(!isMemberZ(-1,i,s->z) && 
       lhpnEnabled(events,nsignals,i,s,nevents)) {
#ifdef __LHPN_UPDATE_STATE__
      printf("Adding transition: %s\n",events[i]->event);
#endif
      recomputeDelays(events,i,s,nevents);
      addTransZ(s->z,i,0,0);
    }
    else if(isMemberZ(-1,i,s->z) &&
	    ((s->m->enablings[i] != 'T') || 
	     (!events[i]->nondisabling && !lhpnEnabled(events,nsignals,i,s,nevents)))) {
#ifdef __LHPN_UPDATE_STATE__
      printf("Removing transition: %s\n",events[i]->event);
#endif
      rmTransZ(s->z,i);
    }
  }
#ifdef __LHPN_UPDATE_STATE__
  printf("Zone after add/remove transitions.\n");
  printZ(s->z,events,nevents,s->r);
#endif
  advanceTime(s,events,rules,ineqL,nevents);
#ifdef __LHPN_UPDATE_STATE__
  printf("Zone after advanceTime\n");
  printZ(s->z,events,nevents,s->r);
#endif
  recanonZ(s->z);
#ifdef __LHPN_UPDATE_STATE__
  printf("Zone after recanon\n");
  printZ(s->z,events,nevents,s->r);
  printM(s->m,events,signals,markkey,ineqL,nevents,nrules,nsignals);
#endif
}

/*****************************************************************************
 * Create a list of all of the inequalities in the net.
 *****************************************************************************/
ineqList initIneqList(ineqADT inequalities,eventADT *events,int nevents,
                      int nplaces)
{
  ineqList ineqL;
  for(ineqADT i=inequalities;i!=NULL;i=i->next) {
    if(strstr(events[i->place]->event,"dot") != NULL) {
#ifdef __LHPN_INIT_INEQ__
      printf("'dot Ineq found!\n");
      printf("%s %d %d\n",events[i->place]->event,i->type,i->constant);
#endif
      i->type += 8;
      for(int j=nevents;j<nevents+nplaces;j++) {
        /* This is a big hack, but it will probably work.  The idea is
           to only analyze place names.  Match the characters in each
           place name from the beginning of the string.  If the
           difference between the total string length and the number
           of matched characters is 3 (the length of dot) and the
           string length is > 3 (to make sure it has a name + dot)
           then we have a match.  It isn't robust, but should be good
           enough until the parser is fixed. */
        if(strlen(events[i->place]->event) > 3 &&
           strlen(events[i->place]->event) -
           strspn(events[j]->event,events[i->place]->event) == 3) {
#ifdef __LHPN_INIT_INEQ__
          printf("%s matches %s\n",events[j]->event,events[i->place]->event);
#endif
          i->place = j;
        }
      }
    }
    ineqL.push_back(i);
  }
  return ineqL;
}

/*****************************************************************************
 * 
 *****************************************************************************/
bool lhpnRsg(char * filename,signalADT *signals,eventADT *events,
             ruleADT **rules,stateADT *state_table,markkeyADT *markkey,
             ineqADT inequalities,int nevents,int nplaces,int nsignals,
             int ninpsig,int nrules,char * startstate,bool si,bool verbose,
             bddADT bdd_state,bool use_bdd,timeoptsADT timeopts,int ndummy,
             int ncausal,int nord,bool noparg,bool search,bool useDot,
             bool poReduce,bool manual)
{
  FILE *fp = NULL;
  timeval t0,t1;
  
  int numStates = 1;
  int numZones = 1;
  int totZones = 1;
  int newState = 0;
  int stackDepth = 0;

  ineqList ineqL = initIneqList(inequalities,events,nevents,nplaces);
  for (int i=nevents;i<nevents+nplaces;i++)
    if (strstr(events[i]->event,"dot")!=NULL)
      events[i]->type = events[i]->type & (!VAR) & (!CONT);
  
#ifdef __LHPN_INIT__
  printSignalADT(signals,nsignals);
  printEv(events, nevents, nplaces);
  printIneqList(ineqL,events,signals);
#endif
  
  gettimeofday(&t0, NULL);
  fp = lhpnInitOutput(filename,signals,nsignals,ninpsig,verbose);
  if(verbose && !fp) {
    return false;
  }
  initialize_state_table(LOADED,FALSE,state_table);
  lhpnRateADT r = initLhpnRate(events,nevents,nplaces);
  lhpnMarkingADT m = initM(nrules,nevents,nsignals,markkey,startstate,events,
                           rules);
  /* if no initial marking, there is problem, so bail out */
  if(m == NULL) {
    if(verbose) {
      fclose(fp);
    }
    return FALSE;
  }
  
  // moved to have s available for lhpnFindInitialZ***
  eventSets E;
  eventSet e;
  lhpnStateADT s = initState(m,NULL,r,E);

  //lhpnSetInitialPreds(signals,events,r,m,ineqL,nevents,nplaces,s);
  lhpnFindInitialZ(rules,events,nevents,nplaces,nsignals,s,ineqL,signals);

#ifdef __LHPN_INIT__
  printf("After initialZ\n");
  printZ(s->z,events,nevents,r);
#endif
  
  // ***used to be here
  lhpnDbmWarp(s,events,nevents);
#ifdef __LHPN_INIT__
  printf("After dbmWarp\n");
  printZ(s->z,events,nevents,r);
#endif
  advanceTime(s,events,rules,ineqL,nevents);
#ifdef __LHPN_INIT__
  printf("After advanceTime\n");
  printZ(s->z,events,nevents,r);
#endif
  recanonZ(s->z);
  
#ifdef __LHPN_NEW_STATE__
  printf("Initial state...\n");
  printM(m,events,signals,markkey,ineqL,nevents,nrules,nsignals);
  printLhpnRate(r,events,nevents,nplaces);
  printZ(s->z,events,nevents,r);
#endif

  addRateState(r,s->z,nevents);
  add_state(fp,state_table,NULL,NULL,NULL,NULL,NULL,0,NULL,m->state,m->marking,
            m->enablings,nsignals,s->z->matrix,s->z->curClocks,s->z->size,0,
	    verbose,1,nrules,s->z->size,s->z->size,bdd_state,use_bdd,markkey,
	    timeopts,-1,-1,NULL,rules,nevents+nplaces,1);
  
  bool done = false;
  lhpnStateADT sNew = NULL;
  lhpnStateADT sCopy = NULL;
  stack<lhpnStateADT> *S = new stack<lhpnStateADT>();

  s->E = lhpnFindPossibleEvents(s->z,r,m,events,ineqL,nevents,s);
  if(s->E.empty()) {
    warn("No events found in the initial state.\n");
    return false;
  }
  while(!done) {
    do {
      if(s->E.empty()) {
        warn("Event set is empty.\n");
        return false;
      }
      e = selectEvent(s->E,events,ineqL);
      if (e.begin()==e.end()) {
        warn("Found an empty event set.\n");
      }
    } while (e.begin() == e.end());
#ifdef __LHPN_EVENTS__
    colorMsg(YELLOW,"Selected eventSet -- e:\n");
    printEventSet(e,events,ineqL);
    printf("\n");
#endif
    if(!s->E.empty()) {
#ifdef __LHPN_STACK__
      printf("Pushing stack!\n");
#endif
      sCopy = copyState(s,events,ineqL,nplaces);
      S->push(sCopy);
      stackDepth++;
    }
    sNew = copyState(s,events,ineqL,nplaces);
#ifdef __LHPN_INIT__
    printf("Zone before updateState.\n");
    printZ(sNew->z,events,nevents,sNew->r);
#endif
    lhpnUpdateState(sNew,e,events,rules,ineqL,markkey,signals,nevents,nplaces,
                    nsignals,nrules);
#ifdef __LHPN_INIT__
    printf("Zone after updateState.\n");
    printZ(sNew->z,events,nevents,sNew->r);
#endif
    
    if(sNew->m) {
#ifdef __LHPN_NEW_STATE__
      colorMsg(YELLOW,"Trying to add this zone\n");
      printZ(sNew->z,events,nevents,sNew->r);
#endif
      addRateState(sNew->r,sNew->z,nevents);
      /* Figure out how to fix the enabling & enabled section of this
         call as well as the action_type */
      char type;
      int enabled;
			int enabling; /* use the enabling to store the direction of
											 predicate change */
      eventSet::iterator i=e.begin();
      if (i != e.end()) {
        if ((*i)->t >= 0) {
          type = 'f';
	  enabling = -1;
          enabled = (*i)->t;
        } else if ((*i)->ineq >= 0) {
          type = 'I';
          enabled = ineqL[(*i)->ineq]->signal;
					if((*i)->cur_value == '1') {
						enabling = -3; //Predicate changes to false from true
					}
					else {
						enabling = -2;
					}
        } else {
	  type = 'R';
	  enabling = -4;
	  enabled = (*i)->rate;
	}
      } else {
        type = 'X';
        enabled=nevents+nplaces;
      }
      newState = add_state(fp,state_table,s->m->state,s->m->marking,
			   s->m->enablings,s->z->matrix,s->z->curClocks,
			   s->z->size,NULL,sNew->m->state,sNew->m->marking,
			   sNew->m->enablings,nsignals,sNew->z->matrix,
			   sNew->z->curClocks,sNew->z->size,numStates,verbose,
			   1,nrules,sNew->z->size,sNew->z->size,bdd_state,
			   use_bdd,markkey,timeopts,enabling,enabled,NULL,rules,nevents+nplaces,1,type);
      if ((enabling==-1) && (enabled > 0) && (enabled < nevents+nplaces) && (events[enabled]->failtrans)) {
        printf("Fired failure transition %s.\n",events[enabled]->event);
        fprintf(lg,"Fired failure transition %s.\n",events[enabled]->event);
	if (!timeopts.nofail) {
          printf("Zones: %d\n",numZones);
          fprintf(lg,"Zones: %d\n",numZones);
          printf("States: %d\n",numStates);
          fprintf(lg,"States: %d\n",numStates);
          return false;
        }
      }
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
#ifdef __LHPN_NEW_STATE__
        cSetFg(CYAN);
        printf("New state: %d -- zone: %d -- totZones: %d\n",numStates,numZones,totZones);
        cSetAttr(NONE);
#endif

#ifdef __LHPN_ADD_STATE__
        printf("NEW STATE[%d]: %s\n",numStates,sNew->m->state);
#endif
        numStates++;
      }
      else {
#ifdef __LHPN_NEW_STATE__
        cSetFg(CYAN);
        printf("New zone: %d -- %d -- states: %d -- totZones: %d",numZones,newState,numStates,totZones);
        cSetAttr(NONE);
        printf("\n");
#endif
      }
      if(numZones % 100 == 0) {
        printf("Zones: %d -- States: %d -- Stack depth: %d\n",numZones,numStates,stackDepth);
        fprintf(lg,"Zones: %d -- States: %d -- Stack depth: %d\n",numZones,numStates,stackDepth);
#ifdef MEMSTATS
#ifndef OSX
        struct mallinfo memuse;
        memuse=mallinfo();
	printf("memory: max=%g Mb inuse=%g Mb free=%g Mb\n",
	       memuse.arena / (1024.0*1024.0),
	       memuse.uordblks / (1024.0*1024.0),
	       memuse.fordblks / (1024.0*1024.0));
	fprintf(lg,"memory: max=%g Mb inuse=%g Mb free=%g Mb\n",
		memuse.arena / (1024.0*1024.0),
		memuse.uordblks / (1024.0*1024.0),
		memuse.fordblks / (1024.0*1024.0));
#else
	malloc_statistics_t t;
	malloc_zone_statistics(NULL, &t);
	printf("memory: max=%g Mb inuse=%g Mb allocated=%g Mb\n",
	       t.max_size_in_use / (1024.0*1024.0),
	       t.size_in_use / (1024.0*1024.0),
	       t.size_allocated / (1024.0*1024.0));
	fprintf(lg,"memory: max=%g Mb inuse=%g Mb allocated=%g Mb\n",
		t.max_size_in_use / (1024.0*1024.0),
		t.size_in_use / (1024.0*1024.0),
		t.size_allocated / (1024.0*1024.0));
#endif
#endif
      }
      freeState(s,nplaces);
      s = sNew;
#ifdef __LHPN_INIT__
      printf("Zone before findPossible.\n");
      printZ(s->z,events,nevents,s->r);
#endif
      s->E = lhpnFindPossibleEvents(s->z,s->r,s->m,events,ineqL,nevents,s);
      if(s->E.empty()) {
        warn("Empty event set.");
 	//printZ(s->z,events,nevents,s->r);
	if (!timeopts.nofail) {
          add_state(fp,state_table,sNew->m->state,sNew->m->marking,
                    sNew->m->enablings,sNew->z->matrix,sNew->z->curClocks,
                    sNew->z->size,NULL,NULL,NULL,
                    NULL,nsignals,NULL,NULL,0,numStates,verbose,true,
                    nrules,0,0,bdd_state,use_bdd,markkey,timeopts,-1,
                    nevents+nplaces,NULL,rules,nevents,true);
          /*
            freeZone(zone);
            hpnDeleteMarking(marking);
            freeCState(cState);
            freeActionSet(A);*/
          printf("Zones: %d\n",numZones);
          fprintf(lg,"Zones: %d\n",numZones);
          printf("States: %d\n",numStates);
          fprintf(lg,"States: %d\n",numStates);
          return false;
        }
      }
#ifdef __LHPN_INIT__
      printf("Zone after findPossible.\n");
      printZ(s->z,events,nevents,s->r);
#endif
#ifdef __LHPN_EVENTS__
      printf("After findPossible EventSets:\n");
      printEventSets(E,events,ineqL);
#endif
    }
    else {
#ifdef __LHPN_NEW_STATE__
      cSetFg(CYAN);
      printf("Same state -- %d\n",newState);
      cSetAttr(NONE);
#endif
      freeState(s,nplaces);
      if(!S->empty()) {
        s = S->top();
        S->pop();
	stackDepth--;
#ifdef __LHPN_STACK__
        printf("Popped the state space stack.\n");
	//printM(s->m,events,signals,markkey,ineqL,nevents,nrules,nsignals);
	//printZ(s->z,events);
#endif
      }
      else {
        done = true;
      }
    }  
  }
  
  delete S;
  /* freeZ(z); */
  /* freeM(m); */
  /* freeLhpnRate(r,nplaces); */
  
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
  printf("CPU Time: %g\n",cpuTime);
  fprintf(lg,"CPU Time: %g\n",cpuTime);
  printf("Real Time: %g\n",time1-time0);
  fprintf(lg,"Real Time: %g\n",time1-time0);
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
  memuse=mallinfo();
  printf("memory: max=%g Mb inuse=%g Mb free=%g Mb\n",
	 memuse.arena / (1024.0*1024.0),
	 memuse.uordblks / (1024.0*1024.0),
	 memuse.fordblks / (1024.0*1024.0));
  fprintf(lg,"memory: max=%g Mb inuse=%g Mb free=%g Mb\n",
	  memuse.arena / (1024.0*1024.0),
	  memuse.uordblks / (1024.0*1024.0),
	  memuse.fordblks / (1024.0*1024.0));
#else
  malloc_statistics_t t;
  malloc_zone_statistics(NULL, &t);
  printf("memory: max=%g Mb inuse=%g Mb allocated=%g Mb\n",
	 t.max_size_in_use / (1024.0*1024.0),
	 t.size_in_use / (1024.0*1024.0),
	 t.size_allocated / (1024.0*1024.0));
  fprintf(lg,"memory: max=%g Mb inuse=%g Mb allocated=%g Mb\n",
	  t.max_size_in_use / (1024.0*1024.0),
	  t.size_in_use / (1024.0*1024.0),
	  t.size_allocated / (1024.0*1024.0));
#endif
#endif
  
  return true;
}
