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
#ifndef __lhpnrsg_h__
#define __lhpnrsg_h__

#include "struct.h"
#include "def.h"
#include "memaloc.h"
#include <stack>
#include <set>
#include <vector>
#include <list>

/* The basic LHPN data structure for tracking the discrete marking
   information..  It is essentially a simplified form of markingADT */
typedef struct lhpnMarkingStruct 
{
  char* marking; /* an array nrules long that give the status of the rule
                    'T' - if marked (discrete place is the preset)
                    'F' - if not marked (discrete place is the preset) */
  char* enablings; /* an array nevents long that is true if the
                      transition is enabled and false otherwise
                      'T' - enabled
                      'F' - not enabled */
  char* state; /* an array nsignals long that shows the state of each
                  signal
                  '1' - the signal is true
                  '0' - the signal is false
                  'R' - the signal is rising
                  'F' - the signal is falling 
                  'X' - the signal is indeterminate */
} *lhpnMarkingADT;

/* This is the basic LHPN zone ADT.  The zone contains enabled
   transitions, active continuous variables, and inactive continuous
   variables.  The dbmEnd variable is used to separate the active and
   enabled items from the inactive items. */
typedef struct lhpnZoneStruct
{
  clockkeyADT curClocks; /* an index based on nevents -- enabling is
                            set to -2 for continuous places in the
                            zone */
  clocksADT matrix; /* 2D matrix that contains the separations */
  int size; /* number of items in the zone */
  int dbmEnd; /* the end of the dbm...this index and everything to the
                 right of this index is an inactive continuous
                 variable */
} *lhpnZoneADT;

typedef struct lhpnBoundStruct
{
  int lower;
  int upper;
  int current;
} *lhpnBoundADT;

/* The continuous state that we need and isn't found in the zone */
typedef struct lhpnRateStruct
{
  lhpnBoundADT bound; /* rate bounds for each place - an array that is
                nplaces long and uses event based indexing */
  
  lhpnBoundADT oldBound; /* rate bounds for an inactive place before
                   it was deactivated - an array that is nplaces long
                   and used event based indexing */
} *lhpnRateADT;

typedef struct lhpnEventStruct
{
  int t; /* the transition to which the event applies.  t is set to -1
            if the event is a pred change event */
  int ineq; /* an index into the ineqL.  ineq is set to -1 if the
               event is a firing event */
  int rate; /* an index to a continuous variable for a rate event */
  char cur_value; /* If event is a predicate change, this stores it current
		     value */
} *lhpnEventADT;

typedef set<lhpnEventADT> eventSet;
typedef list<eventSet> eventSets;
typedef vector<ineqADT> ineqList;

typedef struct lhpnStateStruct
{
  lhpnMarkingADT m;
  lhpnZoneADT z;
  lhpnRateADT r;
  eventSets E;
} *lhpnStateADT;

void updateM(lhpnMarkingADT m,eventADT *events,ruleADT **rules,
             markkeyADT *markkey,int nrules,int nevents);

lhpnBoundADT initBound();
lhpnBoundADT initBound(int upper,int lower);
void freeBound(lhpnBoundADT r);
lhpnBoundADT copyBound(lhpnBoundADT r);

void printZ(lhpnZoneADT z);
void printZ(lhpnZoneADT z,eventADT* events);
void printZ(lhpnZoneADT z,eventADT* events,int nevents,lhpnRateADT state);
void printEventSets(eventSets &E,eventADT *events,ineqList &ineqL);
void printEventSet(const eventSet &E,eventADT *events,ineqList &ineqL);

lhpnMarkingADT initM(int nrules,int nevents,int nsignals,markkeyADT *markkey,
                     char *startState,eventADT *events,ruleADT **rules);
void freeM(lhpnMarkingADT m);
lhpnMarkingADT copyM(lhpnMarkingADT m);
void updateM(lhpnMarkingADT m,eventADT *events,ruleADT **rules,
             markkeyADT *markkey,int nrules,int nevents);
void printM(lhpnMarkingADT m,eventADT *events,signalADT *signals,
            markkeyADT *markkey,ineqList &ineqL,int nevents,int nrules,
            int nsignals);
ineqList initIneqList(ineqADT inequalities,eventADT *events,int nevents,
                      int nplaces);


bool lhpnRsg(char * filename,signalADT *signals,eventADT *events,
             ruleADT **rules,stateADT *state_table,markkeyADT *markkey,
             ineqADT inequalities,int nevents,int nplaces,int nsignals,
             int ninpsig,int nrules,char * startstate,bool si,bool verbose,
             bddADT bdd_state,bool use_bdd,timeoptsADT timeopts,int ndummy,
             int ncausal,int nord,bool noparg,bool search,bool useDot,
             bool poReduce,bool manual);
/*****************************************************************************/
/* integer expression parse tree class.                                      */
/*****************************************************************************/
class exprsn{
 public:
  string op;
  char isit;  // b=Boolean, i=Integer, c=Continuous, n=Number, t=Truth value, 
              //w=bitWise, a=Arithmetic, r=Relational, l=Logical
  int lvalue,uvalue;
  int index;
  double real;
  bool logical;
  exprsn *r1, *r2;
  exprsn(char willbe, int lNV, int uNV, int ptr);
  exprsn(exprsn *nr1, exprsn* nr2, char *nop, char willbe);
  ~exprsn();
  void op_set(char* new_op);
  void eval(lhpnStateADT cur_state,int nevents);
  string out_string(signalADT *signals, eventADT *events);
};

#endif /* lhpnrsg.h */
