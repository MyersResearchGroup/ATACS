/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
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
/*****************************************************************************/
/* conflicts.h - miscellaneous routines used to manipulate conflict sets.    */
/*****************************************************************************/
#ifndef _conflicts_h
#define _conflicts_h

#include "ly.h"
#include "hash.h"
#include "events.h"
#include "struct.h"

// Find the conflict table with the given name
void new_conflict_table(const char *name);

/*****************************************************************************/
/* Initialize hash table used for conflicts.                                 */
/*****************************************************************************/

void init_conflict_table(void);

/*****************************************************************************/
/* Free hash table used for conflicts.                                       */
/*****************************************************************************/

void free_conflict_table(void);

/*****************************************************************************/
/* Enter conflict into conflict table, and initialize conflict.              */
/*****************************************************************************/

conflictsADT conflict(eventsADT l,eventsADT r,
		      int conftype=(ICONFLICT | OCONFLICT));

// Check if there is a conflict in C between l and r. 
// If does, return that conflict;
// If not, return 0.
conflictsADT check_C(conflictsetADT C, eventsADT l, eventsADT r);

conflictsADT check(eventsADT l, eventsADT r);


/*****************************************************************************/
/* Create a conflict set C with conflict "c" as the only member.             */
/*****************************************************************************/

conflictsetADT create_conflict_set(conflictsADT c);

/*****************************************************************************/
/* Find a conflict in the table and add to the conflict set C.               */
/*****************************************************************************/

conflictsetADT add_conflict(conflictsetADT C,eventsADT l,eventsADT r,
			    int conftype = (ICONFLICT | OCONFLICT));
conflictsetADT add_conflict(conflictsetADT C, conflictsADT c);

// find all conflict containing event 'E' from conflict set 'C'.
conflictsetADT find_conflict(conflictsetADT C, eventsADT E);

/*****************************************************************************/
/* Create a conflict set C with conflict "c" as the only member.             */
/*****************************************************************************/

conflictsetADT create_conflict_set(conflictsADT c);

/*****************************************************************************/
/* Delete a conflict set C.                                                  */
/*****************************************************************************/

void delete_conflict_set(conflictsetADT C);

/*****************************************************************************/
/* Copy a set of conflicts (i.e., C1:=C0).                                   */
/*****************************************************************************/

conflictsetADT copy_conflicts(conflictsetADT C0);

/*****************************************************************************/
/* Check if a conflict c is in a set of conflicts C.                         */
/*****************************************************************************/

bool in_conflict_set(conflictsADT c,conflictsetADT C);

/*****************************************************************************/
/* Take the union of two sets of conflicts.                                  */
/*****************************************************************************/

conflictsetADT union_conflicts(conflictsetADT C0,conflictsetADT C1);
conflictsetADT link_conflict(conflictsetADT C0,conflictsetADT C1);

/*****************************************************************************/
/* Subtract one set of conflicts from another.                               */
/*****************************************************************************/

conflictsetADT subtract_conflicts(conflictsetADT C0,conflictsetADT C1);

/*****************************************************************************/
/* Rename events in a set of conflicts C1 so as not to clash with set E0.    */
/*****************************************************************************/

conflictsetADT rename_conflict_set(eventsetADT E0,conflictsetADT C1);

void print_c(void);

void print_c(conflictsetADT C);
#endif /* conflicts.h */
