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
/* rules.h - miscellaneous routines used to manipulate rule sets.            */
/*****************************************************************************/
#ifndef _rules_h
#define _rules_h

#include <list>
#include <map>
#include "ly.h"
#include "hash.h"
#include "events.h"
#include <string>

// Find a hash table for the given entity.
void new_rule_table(const char *name);

/*****************************************************************************/
/* Initialize hash table used for rules.                                     */
/*****************************************************************************/

void init_rule_table(void);

/*****************************************************************************/
/* Free hash table used for rules.                                           */
/*****************************************************************************/

void free_rule_table(void);

/*****************************************************************************/
/* Enter rule into rule table, and initialize rule.                          */
/*****************************************************************************/

rulesADT rule(eventsADT e, eventsADT f, int l, int u,int type, bool rgulr,
	      char *dist, const char *level = "[true]",
	      const char *assign = "[true]");

/*****************************************************************************/
/* Create an rule set R with rule "r" as the only member (i.e., R:={r}).     */
/*****************************************************************************/

rulesetADT create_rule_set(rulesADT r);

/*****************************************************************************/
/* Find rule in table and add to the rule set R.                             */
/*****************************************************************************/

rulesetADT add_rule(rulesetADT R, eventsADT e, eventsADT f, int l, int u,
		    int type, char *dist, const char *level = "[true]",
		    const char *assign = "[true]");

rulesetADT add_rule(rulesetADT R, rulesADT r);

rulesADT check_rule(rulesetADT R, eventsADT e, eventsADT f);
/*****************************************************************************/
/* Create an rule set R with rule "r" as the only member (i.e., R:={r}).     */
/*****************************************************************************/

rulesetADT create_rule_set(rulesADT r);

/*****************************************************************************/
/* Delete a rule set R.                                                      */
/*****************************************************************************/

void delete_rule_set(rulesetADT R);

/*****************************************************************************/
/* Copy a set of rules (i.e., R1:=R0).                                       */
/*****************************************************************************/

rulesetADT copy_rules(rulesetADT R0);

/*****************************************************************************/
/* Check if a rule r is in a set of rules R.                                 */
/*****************************************************************************/

bool in_rule_set(rulesADT r,rulesetADT R);

/*****************************************************************************/
/* Take the union of two sets of rules.                                    */
/*****************************************************************************/

rulesetADT union_rules(rulesetADT R0,rulesetADT R1);

/*****************************************************************************/
/* Subtract one set of rules from another.                                  */
/*****************************************************************************/

rulesetADT subtract_rules(rulesetADT R0,rulesetADT R1);
rulesetADT rm_rules(rulesetADT R0, rulesADT r);

/*****************************************************************************/
/* Rename events in a set of rules R1 so as not to clash with event set E0.  */
/*****************************************************************************/
rulesetADT rename_rule_set(eventsetADT E0,rulesetADT R1);

typedef list<pair<rulesADT, bool> > rule_lst;

// Find all rules enabling the 'dummy' and rules enabled by 'dummy' in the 
// rule set of 'tel'.
// E: dummy event;
// fanin: return value. rules enabling the 'dummy';
// fanout: return value. rules enabled by the 'dummy'. 
void find_rules(eventsADT E, telADT tel,
		rule_lst& fanin, rule_lst& fanout,
		int& fin_size, int& fout_size);

void find_rules(eventsADT E, telADT tel,
		rule_lst& fanin, rule_lst& fanout);
#endif /* rules.h */

