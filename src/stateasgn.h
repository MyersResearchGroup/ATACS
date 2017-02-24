/*****************************************************************************/
/* stateasgn.h                                                               */
/* Solve state assignment problems.                                          */
/*****************************************************************************/
#ifndef _stateasgn_h
#define _stateasgn_h

#include "struct.h"
#include "svhelpers.h"
#undef EXTERN
#ifdef X11
#include <tk.h>
#endif

#include <list>
#include <vector>
#include <functional>
#include <string>

#define COLOR_0 0
#define COLOR_1 1
#define COLOR_R 2
#define COLOR_F 3
#define COLOR_BLANK 4
#define COLOR_END_RISE 5
#define COLOR_END_FALL 6

///////////////////////////////////////////////////////////////////////////

#ifndef __TEMPLATE_PATCH
template<class back_insert_iter>
#else
typedef back_insert_iterator<state_list> back_insert_iter;
#endif 
bool findER(stateADT state_table[], eventADT events[],
	    int nevents, int eventIndex, back_insert_iter bii_ER);

#ifndef __TEMPLATE_PATCH
template<class back_insert_iter>
#else
typedef back_insert_iterator<state_list> back_insert_iter;
#endif 
bool findSR(stateADT state_table[], eventADT event,
	    int eventIndex, back_insert_iter bii_SR);

char svInitialValue(stateADT state_table[]);

char svInitialMarking(cycleADT**** cycles, int ncycles, int nevents,
		      const event_index_cont& eventsBeforeTransition,
		      int endEventIndex);

void clearColors(stateADT state_table[], int color=COLOR_BLANK);

bool colorRise(state_list& startStates, const state_list& endStates, 
	       const event_index_cont& startEvents,
	       const event_index_cont& endEvents,
	       ruleADT** rules,
	       cycleADT**** cycles, int nevents, int ncycles, int cscUpperBound);

bool colorFall(state_list& startStates, const state_list& endStates,
	       const event_index_cont& startEvents,
	       const event_index_cont& endEvents,
	       ruleADT** rules,
	       cycleADT**** cycles, int nevents, int ncycles, int cscUpperBound);

bool colorHigh(state_list& riseRegion);

bool colorLow(state_list& fallRegion);


bool colorStateGraph(stateADT state_table[],
		     state_list& riseStart, state_list& riseEnd,
		     const event_index_cont& riseStartEvents,
		     const event_index_cont& riseEndEvents,
		     state_list& fallStart, state_list& fallEnd,
		     const event_index_cont& fallStartEvents,
		     const event_index_cont& fallEndEvents,
		     ruleADT** rules, cycleADT**** cycles,
		     int nevents, int ncycles, int cscUpperBound);

string stateVal(string& enabling);

int colorVal(int color);

int countCSCViolations(stateADT state_table[],int ninpsig,
			int& total, int& caused);

int evaluateSVInsertionPoint(stateADT state_table[], int ninpsig);

template<class back_insert_iter>
bool findAllRegions(stateADT state_table[], int nevents, eventADT events[],
		    back_insert_iter bii_er, back_insert_iter bii_sr);

state_list intersectRegions(event_index_cont& events,
			    state_list_cont& regionStates);

int find_good_insertion_points(stateADT state_table[],
			       int ninputsigs,
			       eventADT events[],
			       int nevents,
			       ruleADT* rules[],
			       cycleADT**** cycles,
			       int ncycles,
			       int cscUpperBound,
			       int goodSetSize, // keep n best solutions
			       CBestSolutions& bestInsertionPoints);

string insertionString(eventADT events[], const CInsertionPoint& IP);

string nextCSCVariableName(int& variableNumber, signalADT signals[], 
			   int nsignals);

bool find_sv(char * filename,signalADT signals[],eventADT events[],
	     mergeADT merges[],ruleADT *rules[],markkeyADT markkey[],
	     cycleADT ****cycles, int ncycles,
	     stateADT state_table[],int nevents,int ninputs,
	     int nrules,int maxgatedelay,char * startstate,
	     int status,regionADT regions[],int ninputsigs, int nsignals,
	     int ndisj, int nconf, int nord, bool verbose,
	     int initialCost, int& variableNumber, bool systemIsTimed,
	     CBestSolutions& bestSolutions,bool minins);

bool find_state_assignment(char command, designADT design);

///////////////////////////////////////////////////////////////////////////

#endif  /* stateasgn.h */
