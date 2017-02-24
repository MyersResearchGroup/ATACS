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
/* actions.h - miscellaneous routines to manipulate sets of actions.         */
/*****************************************************************************/
#ifndef _actions_h
#define _actions_h

#include "ly.h"
#include "hash.h"
#include <string>

// Find a hash table for the given entity.
void new_action_table(const char *name);

/*****************************************************************************/
/* Initialize hash table used for actions.                                   */
/*****************************************************************************/

void init_action_table(void);

/*****************************************************************************/
/* Free hash table used for actions.                                         */
/*****************************************************************************/

void free_action_table(void);

/*****************************************************************************/
/* Enter action into action table, and initialize action.                    */
/*****************************************************************************/

assignList addAssign(assignList list_assigns, string var, string assign);
actionADT action(const char* a, int len);
actionADT action(const string&);

/*****************************************************************************/
/* Create an action set A with action "a" as the only member (i.e., A:={a}). */
/*****************************************************************************/

actionsetADT create_action_set(actionADT a);

/*****************************************************************************/
/* Delete an action set A.                                                   */
/*****************************************************************************/

void delete_action_set(actionsetADT A);
actionsetADT rm_action(actionsetADT Aset, actionADT A);

/*****************************************************************************/
/* Copy a set of actions (i.e., A1:=A0).                                     */
/*****************************************************************************/

actionsetADT copy_actions(actionsetADT A0);

/*****************************************************************************/
/* Check if an action a is in a set of actions A.                            */
/*****************************************************************************/

bool in_action_set(actionADT a,actionsetADT A);

/*****************************************************************************/
/* Take the union of two sets of actions.                                    */
/*****************************************************************************/

actionsetADT union_actions(actionsetADT A0,actionsetADT A1);

#endif /* actions.h */
