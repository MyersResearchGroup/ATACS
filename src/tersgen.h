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
/* tersgen.h - miscellaneous routines used in TERS generation.               */
/*****************************************************************************/
#ifndef _tersgen_h
#define _tersgen_h
#include <string>

/*****************************************************************************/
/* Create an empty timed event-rule structure.                               */
/*****************************************************************************/

TERstructADT TERSempty();

/*****************************************************************************/
/* Create a timed event-rule structure for an action.                        */
/*****************************************************************************/

TERstructADT TERS(actionADT action, bool type, int lower, 
		  int upper, bool vhdl,
		  const char *level = "[true]", char *dist = NULL,
		  int occur = (-1), const string&data = "");

/*****************************************************************************/
/* Create a timed event-rule structure for a probe.                          */
/*****************************************************************************/
/*
TERstructADT probe(actionADT action,int data);
*/

/*****************************************************************************/
/* Modify distribution on an event for choice probabilities.                 */
/*****************************************************************************/

void TERSmodifydist(TERstructADT S,double dist);

/*****************************************************************************/
/* Mark all rules in timed event-rule structure as ordering.                 */
/*****************************************************************************/

void TERSmarkord(TERstructADT S);

/*****************************************************************************/
/* Mark all rules in timed event-rule structure as assumptions.              */
/*****************************************************************************/

void TERSmarkassump(TERstructADT S);

/*****************************************************************************/
/* Copy a timed event-rule structure.                                        */
/*****************************************************************************/

TERstructADT TERScopy(TERstructADT S0);

/*****************************************************************************/
/* Delete a timed event-rule structures.                                     */
/*****************************************************************************/

void TERSdelete(TERstructADT S);

/*****************************************************************************/
/* Compose two timed event-rule structures.                                  */
/*****************************************************************************/

TERstructADT TERScompose(TERstructADT TERS1,TERstructADT TERS2, char* op);

/*****************************************************************************/
/* Rename a timed event-rule structures based on the events of another.      */
/*****************************************************************************/

TERstructADT TERSrename(TERstructADT TERS1,TERstructADT TERS2);

/*****************************************************************************/
/* Setup a timed event-rule structure to loop or repeat.                     */
/*****************************************************************************/

TERstructADT TERSmakeloop(TERstructADT S);

/*****************************************************************************/
/* Make a timed event-rule structure loop.                                   */
/*****************************************************************************/

TERstructADT TERSloop(TERstructADT S);

/*****************************************************************************/
/* Make a timed event-rule structure repetitive.                             */
/*****************************************************************************/

TERstructADT TERSrepeat(TERstructADT S0,char *op);

 
/*****************************************************************************/
/* Check if a timed event-rule structure is closed.                          */
/*****************************************************************************/

void checkclosed(TERstructADT S);

/*****************************************************************************/
/* Emit a timed event-rule structure.                                        */
/*****************************************************************************/
void emitters(const char* path, const char* filename, TERstructADT TERS);

// Print a rule set:
void emit_rule_set(ostream& out, bool ord, rulesetADT rules, int epsilon,
		   bool fail,const char *type="");

// The following function emits either the input or the output events:
void emit_some_events(ostream&out, TERstructADT S, int type, char*prefix,
		      bool dummy);

//print the declaration of event (A,i) followed by <delim>:
void declare_event(ostream&out, actionADT A, int i, char delim);

//print the declarations of (A,i) and (A->inverse,i) on one line:
void declare_pair (ostream&out, actionADT A, int i);

ostream& operator<<(ostream&out, eventsADT E);//Output an event

//Output a whole set of conflicts:
ostream& operator<<(ostream&out, conflictsetADT cur_conflict);

//Return the maximum occurrence of act and act->inverse (if act has an inverse)
int Max(actionADT act);


/*****************************************************************************/
/* Emit a timed event-rule structure.                                        */
/*****************************************************************************/

void print_ters(TERstructADT TERS);


int rule_cnt(telADT);
int event_cnt(telADT);

//void post_process(TERstructADT S, bool self_lp=false);

void replace(TERstructADT S);

/*****************************************************************************/
/* Load a timed event-rule structure from a file to a TERSstruct.            */
/*****************************************************************************/

void postprocess(char * filename,bool postproc,bool redchk,bool fromG,
		 bool verbose);

#endif /* tersgen.h */
