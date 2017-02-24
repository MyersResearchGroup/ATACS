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
#ifndef __hpnrsg_h__
#define __hpnrsg_h__

#include "struct.h"
#include "def.h"
#include "memaloc.h"
#include <list>

/* The basic HPN marking ADT.  It is essentially a simplified form of
   markingADT */
typedef struct dMarkingStruct 
{
  char* marking; /* an array nrules long that give the status of the rule
                    'T' - if marked (discrete place is the preset)
                    'F' - if not marked (discrete place is the preset) 
                    'C' - if a continuous place is the preset */
  char* enablings; /* an array nevents long that gives the state of
                      the transition
                      'D' - discretely enabled
                      'E' - enabled
                      'F' - not enabled */
  char* state; /* the state of each signal */
} *dMarkingADT;

/* This is the basic HPN zone ADT.  The zone contains inactive
   continuous variables, past variables, and future variables.  These
   different types of entries are grouped left to right in the order
   previously specified.  The sep variables let the user know where
   the separations between the variables are. */
typedef struct hpnZoneStruct
{
  clockkeyADT curClocks; /* an index based on nevents -- a clockkey is
                            used b/c the add_state function requires a
                            clockkey as an input -- enabling is set to
                            -1 for continuous places in the zone */
  clocksADT matrix; /* 2D matrix that contains the separations */
  int numClocks; /* number of transitions in the zone */
  int dbmEnd; /* the end of the dbm...this index and everything to the
                 right of this index is an inactive continuous
                 variable -- this index always points 1 past the last
                 valid dimension of the dbm */
} *hpnZoneADT;

/* The continuous state that we need and isn't found in the zone */
typedef struct cStateStruct
{
  int* warp; /* instantaneous rate for each place - to get an event
                index just add or subtract nevents as appropriate -
                the array is nplaces long*/
  
  int* oldWarp; /* this is a vector used to store the value of warp
                   before a variable is deactivated.  The value of the
                   warp before being deactivated is useful is several
                   situations, but easily kept in the zone. This
                   variable is also nplaces long. */

/*   int* newWarp; */ /* this is a vector used to store the value of warp
                   before a variable is deactivated.  The value of the
                   warp before being deactivated is useful is several
                   situations, but easily kept in the zone. This
                   variable is also nplaces long. */
  
  char* firedClocks; /* an char* array nrules+1 long with either 'T'
                        if the clock has fired or 'F' if the clock
                        hasn't been fired */

  char* introClocks; /* an char* array nrules+1 long with either 'T'
                        if the clock has been introduced or 'F' if the
                        clock hasn't been introduced */
  
  int* vCache; /* cache of velocities already calculated for each
                  continuous event (nevents long) */
  bool* vValid; /* is the vCache value valid or dirty? */
} *cStateADT;

/* An ADT to hold the information required for an action which is
   (p,t) and the action type */
typedef struct hpnActionStruct 
{
  int enabling; /* the place */
  int enabled; /* the transition */
  char type; /* the type of desired action
                'F' - fire
                'I' - intro
                'J' - intro predicate
                'D' - del
                'E' - del predicate
                'V' - variable inactive -- Note: in the variable
                inactive case enabling will be -1 and enabled will be
                p */
  int set; /* an integer used to group this action with actions that
              can happen simultaneously. */
  char cls; /* determines what class of action we have 
               'N' - nonfirable
               'F' - firable */
  ineqADT ineq; /* an ineqADT which is NULL for non-predicate actions */ 
} *hpnActionADT;

/* Holds a the set of actions */
typedef struct hpnActionSetStruct 
{
  list<hpnActionADT> *l; /* the list of possible actions in the set */
  int setCnt; /* the next set number for a new action */
} *hpnActionSetADT;

/* holds state space exploration information during the state space.
   A stack is used to store information as the search proceeds
   throughout the tree-like state space */
typedef struct hpnStackItem
{
  dMarkingADT marking;
  hpnZoneADT zone;
  cStateADT cState;
  hpnActionSetADT A;
  struct hpnStackItem *next; /* pointer used in stack creation */
} *hpnStackADT;

/* used to create a stack of zones which allows functions to actually
   return sets of zones */
typedef struct hpnZoneSetStruct
{
  hpnZoneADT zone;
  struct hpnZoneSetStruct *next;
} *hpnZoneSetADT;

typedef struct hpnSimStruct
{
  hpnActionADT action;
  bool quit;
} *hpnSimADT;

typedef struct hpnIneqStruct
{
  ineqADT ineq;
  int xition;
} *hpnIneqADT;

typedef struct hpnIneqListStruct
{
  hpnIneqADT* l;
  int ineqCnt;
} *hpnIneqListADT;

typedef struct hpnBounds
{
  int upper;
  int lower;
} hpnBounds;

bool hpnRsg(char * filename,signalADT *signals,eventADT *events,
            ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
            int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
            char * startstate,bool si,bool verbose,bddADT bdd_state,
            bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
            int nord,bool noparg,bool search,bool useDot,bool poReduce,
            bool manual);
hpnActionADT rmActionSetItem(hpnActionSetADT *aSet);
hpnActionADT copyAction(hpnActionADT a);
hpnActionSetADT copyActionSet(hpnActionSetADT aSet);
void freeActionSet(hpnActionSetADT aSet);
void addCf(int enabling,int enabled,cStateADT cState,ruleADT **rules);
void rmCf(int enabling,int enabled,cStateADT cState,ruleADT **rules);
int xdot(int p,eventADT *events,ruleADT **rules,dMarkingADT marking,
         hpnZoneADT zone,cStateADT cState,int nevents,int nplaces,
         signalADT *signals,int nsignals);
bool isMemberPc(int p,eventADT *events);
bool isVar(int p,eventADT *events);
void varAsgUpdate(hpnZoneADT &zone,cStateADT &cState,int p,ruleADT **rules,
                  int nevents,char* arcType,eventADT *events,int nplaces);
void introduceClock(hpnActionADT a,hpnZoneADT *newZone,cStateADT *newCState,
                    ruleADT **rules,eventADT *events,int nevents,
                    char *arcType,markkeyADT *markkey,int nrules);
void deleteClock(hpnActionADT a,hpnZoneADT *newZone,cStateADT *newCState,
                 ruleADT **rules,eventADT *events,int nevents,char *arcType);
void deleteVariable(hpnActionADT a,hpnZoneADT *newZone,cStateADT *newCState,
                    dMarkingADT *newMarking,ruleADT **rules,eventADT *events,
                    int nevents,int nplaces,char *arcType);
int chkDiv(int a,int b,char func);

/* print functions */
void printFiredClocks(cStateADT cState,markkeyADT *markkey,eventADT *events,
                      int nrules);
void printContVars(hpnZoneADT zone,eventADT *events,cStateADT cState,
                   int nevents);
void printZone(hpnZoneADT zone,eventADT *events);
void printZone(hpnZoneADT zone,eventADT *events,cStateADT cState,int nevents);
void printPlainZone(hpnZoneADT zone);
void printZoneS(hpnZoneSetADT zoneS,eventADT *events);
void printActionSet(hpnActionSetADT aSet,eventADT *events);
void printAction(hpnActionADT a,eventADT *events);
void printActionSimple(hpnActionADT a,eventADT *events);
void setMustBit(hpnActionSetADT &aSet);

void addIntroClocks(int p,int t,cStateADT cState,ruleADT **rules);
void resetIntroClocks(int p,cStateADT cState,ruleADT **rules,int nevents,
                      hpnZoneADT zone);

bool isMemberCf(int p,int t,cStateADT cState,ruleADT **rules);
void pruneSet(hpnActionSetADT *A,hpnZoneADT zone,
              cStateADT cState,ruleADT **rules,char *arcType,eventADT *events,
              int nevents,int nplaces);
void uncondAddActionSetItem(hpnActionADT a,hpnActionSetADT &aSet);
hpnActionADT rmActionSetItem(hpnActionSetADT &aSet);
bool checkFire(hpnActionSetADT *A,int enabling,int enabled,eventADT *events);
bool checkFireFinal(hpnActionSetADT *A,int enabling,int enabled,int set,
                    eventADT *events);
void printIneq(ineqADT i,eventADT *events);
#endif /* hpnrsg.h */
