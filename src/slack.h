/*****************************************************************************/
/* slack.h                                                                   */
/*****************************************************************************/

#ifndef _slack_h
#define _slack_h

#include "struct.h"

/*****************************************************************************/
/* Find slack.                                                               */
/*****************************************************************************/

void slack_chk(char * filename,signalADT *signals,eventADT *events,
	       mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	       markkeyADT *markkey,stateADT *state_table,int nevents,
	       int ncycles,int ninputs,int nrules,
	       int maxstack,int maxgatedelay,char * startstate,
	       bool initial,regionADT *lastregions,int status,
	       regionADT *regions,bool manual,int ninpsig,int nsignals,
	       bool verbose);

#endif /* slack.h */
