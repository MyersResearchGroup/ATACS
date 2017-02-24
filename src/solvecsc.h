/*****************************************************************************/
/* solvecsc.h                                                                */
/*****************************************************************************/

#ifndef _solvecsc_h
#define _solvecsc_h

#include "struct.h"
#include "findrsg.h"
#include "connect.h"
#include "findreg.h"
#include "findtd.h"
#include "compile.h"

/*****************************************************************************/
/* Solve CSC problems.                                                       */
/*****************************************************************************/

bool solve_csc(char* filename,signalADT *signals,eventADT *events,
	       mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	       markkeyADT *markkey,stateADT *state_table,int nevents,
	       int ncycles,int ninputs,int nrules,
	       int maxstack,int maxgatedelay,char* startstate,
	       bool initial,regionADT *lastregions,int status,
	       regionADT *regions,bool manual,int ninpsig,int nsignals,
	       bool verbose,bool fromER,bool fromTEL);

#endif /* solvecsc.h */
