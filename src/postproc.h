#ifndef POSTPROC_H
#define POSTPROC_H

#include <string>
#include <list>
#include <algorithm>
#include "ly.h"
#include "rules.h"

// Remove all dummy events from 'tel' depending on certain conditions, or
// remove the given 'dummy' for debugging purposes.  
telADT post_proc(telADT tel, bool redchk, bool verbose, bool fromG, 
		 bool xform2, bool VHDLorHSE = false);


// Remove the dummy action, event, and all rules containing the dummy event
// from the 'tel'. 
// Add news rules from 'pred' enabling -> 'succ' enabled.
bool update(telADT tel, eventsADT dummy, const rule_lst& fanin,
	    const rule_lst& fanout, int mode, bool fromG, bool verbose);

#endif 
