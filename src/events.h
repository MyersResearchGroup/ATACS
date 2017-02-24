/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1993 by, Chris J. Myers                                   */
/*   Center for Integrated Systems,                                          */
/*   Stanford University                                                     */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of Stanford University may not be used in advertising or   */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Stanford.                         */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL STANFORD OR THE AUTHORS OF THIS SOFTWARE BE LIABLE    */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* events.h - miscellaneous routines used to manipulate event sets.          */
/*****************************************************************************/
#ifndef _events_h
#define _events_h

#include "ly.h"
#include "hash.h"
#include "actions.h"
#include <string>

// Find the event table with the given name
void new_event_table(const char *name);


/*****************************************************************************/
/* Initialize hash table used for events.                                    */
/*****************************************************************************/

void init_event_table(void);

/*****************************************************************************/
/* Free hash table used for events.                                          */
/*****************************************************************************/

void free_event_table(void);

/*****************************************************************************/
/* Enter event into event table, and initialize event.                       */
/*****************************************************************************/

eventsADT event(actionADT a,int occur,int lower,int upper,char *dist, 
		const char *level = "[true]",bool fixed_occur = FALSE,
		const string& data="");
//eventsADT check_event(const string& a,int occur);

// Compare two events beased on their label and ocuurence number.
// true is returned if e1 is the same as e2; otherwise, false is returned.
bool event_cmp(eventsADT e1, eventsADT e2);

/*****************************************************************************/
/* Override the default delay for an event.                                  */
/*****************************************************************************/

void delay_override(eventsADT e,int lower,int upper);

/*****************************************************************************/
/* Create an event set A with event "e" as the only member (i.e., E:={e}).   */
/*****************************************************************************/

eventsetADT create_event_set(eventsADT e);

/*****************************************************************************/
/* Delete an event set E.                                                    */
/*****************************************************************************/

void delete_event_set(eventsetADT E);

/*****************************************************************************/
/* Copy a set of events (i.e., E1:=E0).                                      */
/*****************************************************************************/

eventsetADT copy_events(eventsetADT E0);

/*****************************************************************************/
/* Check if an event a is in a set of events E.                              */
/*****************************************************************************/

bool in_event_set(eventsADT e,eventsetADT E);

/*****************************************************************************/
/* Take the union of two sets of events.                                    */
/*****************************************************************************/

eventsetADT union_events(eventsetADT E0,eventsetADT E1);
eventsetADT link_event(eventsetADT R0, eventsetADT R1);

/*****************************************************************************/
/* Subtract one set of events from another.                                  */
/*****************************************************************************/

eventsetADT subtract_events(eventsetADT E0,eventsetADT E1);

/*****************************************************************************/
/* Check if an event is in a set E, and if so rename it.                     */
/*****************************************************************************/

eventsADT rename_event(eventsetADT E,eventsADT e);

/*****************************************************************************/
/* Rename a set of events E1 so as not to clash with a set E0                */
/*****************************************************************************/

eventsetADT rename_event_set(eventsetADT E0,eventsetADT E1);

#endif /* events.h */
