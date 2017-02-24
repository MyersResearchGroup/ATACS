/*****************************************************************************/
/* project.h                                                               */
/*****************************************************************************/

#ifndef _project_h
#define _project_h

#include "struct.h"
#include "def.h"

/*****************************************************************************/
/* Project out dummy transitions in the state graph.                         */
/*****************************************************************************/
bool project(char * filename,signalADT *signals,stateADT *state_table,
	     int ninpsig,int *nsignals,bool verbose,eventADT *events,
	     int nevents,char *startstate,int nineqs);

#endif /* project.h */
