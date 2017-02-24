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
#ifndef __srllpnrsg_h__
#define __srllpnrsg_h__

#include "struct.h"
#include "def.h"
#include "memaloc.h"

extern bool *disabled;

typedef struct resDirtyStruct {
  bool *res;
  bool *dirty;
} resDirtyADT;

/* holds all of the information required for a zone */
typedef struct srlZone
{
  clockkeyADT curTransitions; /* an array of clockkey structs that is
				 nevents long.  The enabled member
				 holds a numeric value for all
				 transitions currently in the zone */
  clocksADT matrix; /* a simple 2D matrix that represents the values
		       of the clock timers in the zone */
  int numTransitions; /* number of transitions currently in the zone */
  int pastFutureSep; /* past variables (transitions that have already
			occurred) are stored in the left side of the
			zone.  This number represents the first future
			variable in the zone */
  struct tpADT *tP; /* the list of possible true parents */
} *srlZoneADT;

/* a convenient way to pass around the lower and upper bounds for a
   specific timer */
typedef struct bounds
{
  int upper;
  int lower;
} bounds;

/* holds a linear array of data with information about the number of
   current items in the array and the current position of operation */
typedef struct srlList
{
  int pos; /* position w/in the array for the next operation */
  int tot; /* total items in the array */
  int *array; /* simple array */
} *srlListADT;

/* a linked list style data structure intended to hold the concrete
   trace that is found */
typedef struct traceList
{
  int event;
  struct traceList *next;
} *traceListADT;

/* holds state space exploration information during the state space.
   A stack is used to store information as the search proceeds
   throughout the tree-like state space */
typedef struct lpnStackItem
{
  markingADT marking;
  char *firelist;
  srlZoneADT zone;
  struct lpnStackItem *next; /* pointer used in stack creation */
} *srlStackADT;

typedef struct abStackItem
{
  markingADT marking; 
  char *firelist;
  srlZoneADT zone;
  srlListADT abTrace; /* the original abstracted trace data structure */
  srlListADT Eset;  /* the set E */
  traceListADT concTrace; /* the current possible concrete trace */
  struct abStackItem *next; /* pointer used in stack creation */
} *abStackADT;

/* holds an event and the corresponding minimum firing time.  this is
   used in the necessary calculation */
typedef struct eventTimePair
{
  bool event;
  int minBound;
} eventTimePairADT;

/* gives information about which events are newly enabled and which
   events are the parents of newly enabled events */
typedef struct parentADT
{
  bool event;
  bool *parents;
} parentADT;

/* used to create a stack of zones which allows functions to actually
   return sets of zones */
typedef struct zoneSet
{
  srlZoneADT zone;
  struct zoneSet *next;
} *srlZoneSet;

typedef struct tpADT 
{
  int tp; /* this will be an index into the zone for the true parent */
  int tn; /* this will be an index based on nevents for the newly
             enabled event...this is b/c at the time when this variable will be
             set the newly enabled event is not yet in the zone */
  struct  tpADT *next;
} *tpList;

bool *newBoolVector(int size);
void emptyBoolVector(bool *boolVec,int size);
int sizeBoolVector(bool *boolVec,int size);
void unionBoolVector(bool *boolVec1, bool *boolVec2,int size);
bool isEmptyBoolVector(bool *boolVec,int size);
void copyBoolVector(bool *boolVec1,bool *boolVec2,int size);

/*****************************************************************************/
/* Find reachable states for a given LPN.                                    */
/*****************************************************************************/
bool lpnRsg(char * filename,signalADT *signals,eventADT *events,
            ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
            int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
            char * startstate,bool si,bool verbose,bddADT bdd_state,
            bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
            int nord,bool noparg,bool search,bool useDot,bool poReduce,
            bool lpn);

/*****************************************************************************
* Finds the possible false negatives from abstracted nets.
******************************************************************************/
traceListADT findTrace(char *filename,signalADT *signals,eventADT *events,
                       ruleADT **rules,stateADT *state_table,
                       markkeyADT *markkey,int nevents,int nplaces,
                       int nsignals,int ninpsig,int nrules,char * startstate,
                       bool nofail,bool verbose);

/* declarations needed for srllpnrsg.c */
eventTimePairADT *lpnTraceBack(bool *TD,int t,markingADT marking,
                               eventADT *events,ruleADT **rules,
                               int nevents,int nplaces);

void lpnPushMarking(srlStackADT *stateSpaceStack, char *firelist,
                    srlZoneADT zone, markingADT marking);


#endif /* srllpnrsg.h */
