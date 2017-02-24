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
/* 15DEC08
   Added implementations of expression tree functions from struct.h
   Robert Thacker
 */
 
/*
  struct.cpp by Eric Robert Peskin
  
  This file contains C++ stream output functions for various data
  structures from struct.h.
  
  Questions?  mailto:peskin@vlsigroup.elen.utah.edu
  Bug reports?  http://www.async.elen.utah.edu/cgi-bin/atacs
*/

#include   "struct.h"
#include   "lhpnrsg.h"

/* The following operator outputs an entire design to a stream in TEL
   file format.  It's functionality is a hybrid of emitters() from
   tersgen.[hc] and store_er() from storeer.[hc]. */
ostream& operator<<(ostream&out, designADT design){
  if(!design){
    out << "# DESIGN IS A NULL POINTER!" << endl;
    return out;
  }
  int      nevents(design->nevents);  // just a local copy for convenience
  eventADT *events(design-> events);  // just a local copy for convenience
  char     * start("0"            );  // default start state
  if (design->startstate && design->startstate[0]){
    start = design->startstate;
  }
  out << "#  This Timed Event/Level structure contains:" << endl
      << ".e " <<	  nevents << " # events"	 << endl
      << ".i " << design->ninputs << " # input events"	 << endl
      << ".r " << design->ncausal << " # causal rules"	 << endl
      << ".n " << design->nord	  << " # ordering rules" << endl
      << ".d " << design->ndisj   << " # merges"         << endl
      << ".c " << design->nconf	  << " # conflicts"	 << endl
      << ".s " <<	  start	  << " # start state"	 << endl
      << "#"						 << endl
      << "# EVENTS:"					 << endl
      << "#"						 << endl
      << "reset"					 << endl;
  if(events){
    int e=1;
    out << "#Each of the following events has the format <action>/<occurrence>"
	<< endl
	<< "#   Input Events:" << endl;
    while ( e < design->ninputs                    ){
      out << events[e++] << ' ' ;
      out << events[e++] << endl;
    }
    out << "#   Output Events:" << endl;
    while ( e < nevents && events[e]->event[0]!='$'){
      out << events[e++] << ' ' ;
      out << events[e++] << endl;
    }
    out << "#   Dummy (Sequencing) Events:"  << endl;
    while ( e < nevents ){
      out << events[e++] << endl;
    }
  }
  if(design->rules){
    out << "#" << endl
	<< "# RULES, which have the following format:" << endl
	<< "#  <enabling> <enabled> [<expression>] <epsilon> <lower> <upper>"
	<< endl;
    // Output the rules -- first the causal rules, then the ordering rules:
    for (int flip=ORDERING;  flip>=0;  flip-=ORDERING){  // flip is an XOR mask
      out << "#   ";
      if (flip){
	out << "Causal (Behavioral)";
      }
      else{
	out << "Ordering (Constraint)";
      }
      out << " Rules:" << endl;
      for (int i=0;  i<nevents;  i++){
	if(design->rules[i]){
	  for (int j=0;  j<nevents;  j++){
	    ruleADT rule(design->rules[i][j]);
	    if(rule && rule->ruletype && // This is a valid rule, and has its
	       (flip ^ rule->ruletype & ORDERING )){// ORDERING bit in phase.
	      out << events[i] <<' '<< events[j] << rule << endl;
	    }
	  }
	}
      }
    }
  }
  if(design->merges){  // Output all the mergers:
    out << "#" << endl << "# MERGERS:" << endl << "#" << endl;
    for (int i=1;  i<nevents;  i++){  // Consider each event.
      mergeADT curmerge(design->merges[i]);  // list of mergers with this event
      if ( curmerge && curmerge->mergetype && curmerge->mevent > i ) {
	out << events[i];
	while ( curmerge ){
	  if ( curmerge->mergetype  && curmerge->mevent > i){
	    out << ' ' << events[curmerge->mevent];
	  }
	  curmerge = curmerge->link;
	}
	out << endl;
      }
    }
  }
  
  if(design->rules){// Output all the conflicts:
    out << "#" << endl << "# CONFLICTS:" << endl << "#" << endl;
    for (int i=0;  i<nevents;  i++){
      if(design->rules[i]){
	for (int j=0;  j<i      ;  j++){
	  ruleADT rule(design->rules[i][j]);
	  if (rule && rule->conflict){
	    out << events[i] <<' '<< events[j] << endl;
	  }
	}
      }
    }
  }
  return out;
}


//The following operator outputs the string associated with an event.
ostream& operator<<(ostream&out, eventADT event){ // Output one event.
  if(event && event->event){
    out << event->event;
  }
  return out;
}

//The following operator outputs the information about a rule EXCEPT its events
ostream& operator<<(ostream&out,  ruleADT rule){  // Output a rule.
  if(!rule){
    out << "# Rule is a NULL pointer!" << endl;
    return out;
  }
  char * exp(rule->expr);  // the string for the level expression for this rule
  if (exp && strcmp(exp,"[true]") && strcmp(exp,"[(true)]")){ // non-trivial
    out << ' ' << exp;
    if (rule->ruletype & ORDERING){
      out << 'd'; // use disabling semantics
    }
  }
  out << ' ' << rule->epsilon;  // Is this an initial rule?
  if (rule->ruletype & ASSUMPTION){
    out << " a a";
  }
  else {
    out << " " << rule->lower;  // lower timing bound
    if(INFIN == rule->upper){
      out << " inf"; // text symbol for INFIN
    }
    else{
      out << " " << rule->upper; // finite upper bound
    }
  }
  if(rule->epsilon){
    out << " # INITIAL";
  }
  return out;
}
