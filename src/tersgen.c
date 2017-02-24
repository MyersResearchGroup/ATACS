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
/* tersgen.c - miscellaneous routines used in timed event-rule structure     */
/*             generation.                                                   */
/*****************************************************************************/
#include <fstream>
#include <iostream>
#include <string>
#include "actions.h"
#include "auxilary.h"
#include "compile.h"
#include "conflicts.h"
#include "events.h"
#include "hse.hpp"
#include "loader.h"
#include "ly.h"
#include "postproc.h"
#include "rules.h"
#include "struct.h"
#include "tersgen.h"
#include "types.h"

extern bool toTEL;
extern int problems;

/*****************************************************************************/
/* Create an empty timed event-rule structure.                               */
/*****************************************************************************/

TERstructADT TERSempty()
{
  TERstructADT S=NULL;

  S = (TERstructADT)GetBlock(sizeof(*S));
  S->A=NULL;
  S->I=NULL;
  S->O=NULL;
  S->first=NULL;
  S->last=NULL;
  S->loop=NULL;
  S->R=NULL;
  S->Rp=NULL;
  S->C=NULL;
  S->Cp=NULL;
  
  //AMS Additions
  S->CT=NULL;
  S->CP=NULL;
  S->DPCT=NULL;
  S->CPCT=NULL;
  S->CTCP=NULL;
  S->CPDT=NULL;
  S->DTCP=NULL;
  
  //S->exp = new char[7];
  //S->expr = new char[8];
  //strcpy(S->exp, "[true]");
  //strcpy(S->expr, "[false]");

  return(S);
}

/*****************************************************************************/
/* Create a timed event-rule structure with a single action.                 */
/*****************************************************************************/

TERstructADT TERSaction(actionADT a, bool input,
			int lower, int upper, bool vhdl,int occur,
			const char *level = "[true]", char *dist = NULL,
			const string&data = "")
{
  
  TERstructADT S;
  bool fixed=TRUE;

  vhdl = FALSE;
 
  S = TERSempty();

/*  if ((input) && (a->vacuous)) return(S); */

  a->lower = lower;
  a->upper = upper;
  a->dist = CopyString(dist);
  S->A = create_action_set(a);
  if (occur==(-1)) {
    fixed=FALSE;
    occur=1;
  }

  if(input) 
    S->I = create_event_set(event(a,occur,lower,upper,dist,level,fixed,data));
  else 
    S->O = create_event_set(event(a,occur,lower,upper,dist,level,fixed,data));
  
  if(input == TRUE) 
    S->first = NULL;
  else 
    S->first=
      create_event_set(event(a,occur,lower,upper,dist,level,fixed,data));
  
  if(input || !vhdl) 
    S->last=create_event_set(event(a,occur,lower,upper,dist,level,fixed,data));

  /*
  if ((a) && (a->inverse))
    a->inverse->vacuous = FALSE;
  */
/*
  if ((!input) || (!(a->vacuous))) {
    if ((a) && (a->inverse))
      a->inverse->vacuous=FALSE;
    else a->vacuous=FALSE;
  }
*/
/*TEMP*/
  
  if (a->maxoccur==0) 
    a->maxoccur = 1;

  return(S);
}

/*****************************************************************************/
/* Creates a continuous timed event rules structure                          */
/*****************************************************************************/

TERstructADT TERSCont(actionADT cp, bool input,
		      int lrate, int urate, bool vhdl,int occur,
		      const char *level = "[true]", char *dist = NULL,
		      const string&data = "") {
  TERstructADT S;

  S = TERSempty();
  S->A = create_action_set(cp);
  if (occur==(-1)) {
    occur=1;
  }
  
  S->CP = create_event_set(event(cp,occur,lrate,urate,dist,level));
  /*string ctlabel = cp->label;
  actionADT ct = action(ctlabel + "T");
  actionsetADT ctset = create_action_set(ct);
  S->A = union_actions(S->A,ctset);
  delete_action_set(ctset);
  ct->type = CONT;
  
  S->CT = create_event_set(event(ct,occur,lrate,urate,dist,level));
  */
  if (lrate < 0) {
    lrate = lrate * -1;
    urate = urate * -1;
    //S->CPCT = add_rule(S->CPCT, S->CP->event, S->CT->event, lrate, urate,
    //       CONT, dist, level);
  }
  else {
    //S->CTCP = add_rule(S->CTCP, S->CT->event, S->CP->event, lrate, urate,
    //     CONT, dist, level);
  }
    
  //S->first = create_event_set(event(ct,occur,lrate,urate,dist,level));

  return S;
}

/*****************************************************************************/
/* Creates a TER Structure for a DTCP Assignment Arc                         */
/*****************************************************************************/

TERstructADT TERSassign(actionADT cp, bool input,
			int lrate, int urate, bool vhdl,int occur,
			const char *level = "[true]", char *dist = NULL,
			const string&data = "") {
  TERstructADT S;

  S = TERSempty();
  S->A = create_action_set(cp);
  if (occur==(-1)) {
    occur=1;
  }

  S->CP = create_event_set(event(cp,occur,lrate,urate,dist,level));
  //actionsetADT trans = create_action_set(dummyE());
  //S->A = union_actions(S->A,trans);
  //delete_action_set(trans);

  //S->first = create_event_set(event(cp,occur,lrate,urate,dist,level));
  
  //S->DTCP = add_rule(S->DTCP, ALLEN->last->event, S->CP->event, lrate, urate,
  //	     CONT, dist, level);

  return S;

}
/*****************************************************************************/
/* Copy a timed event-rule structure.                                        */
/*****************************************************************************/

TERstructADT TERScopy(TERstructADT S0)
{
  TERstructADT S1=NULL;

  if(S0 == NULL)
    return TERSempty();

  S1 = (TERstructADT)GetBlock(sizeof(*S1));
  S1->A = copy_actions(S0->A);
  S1->I = copy_events(S0->I);
  S1->O = copy_events(S0->O);
  S1->first = copy_events(S0->first);
  S1->last = copy_events(S0->last);
  S1->loop = copy_events(S0->loop);

  //AMS Additions
  S1->CT = copy_events(S0->CT);
  S1->CP = copy_events(S0->CP);
  S1->DPCT = copy_rules(S0->DPCT);
  S1->CPCT = copy_rules(S0->CPCT);
  S1->CTCP = copy_rules(S0->CTCP);
  S1->CPDT = copy_rules(S0->CPDT);
  S1->DTCP = copy_rules(S0->DTCP);

  S1->R = copy_rules(S0->R);
  S1->Rp = copy_rules(S0->Rp);
  S1->C = copy_conflicts(S0->C);
  S1->Cp = copy_conflicts(S0->Cp);
  //  S1->exp = new char[strlen(S0->exp)+1];
  //  strcpy(S1->exp, S0->exp);

  return(S1);
}

/*****************************************************************************/
/* Check if an enabling event is vacuous.                                    */
/*****************************************************************************/
bool vacuous(eventsADT e,actionsetADT A)
{
/*  char inverse[MAXTOKEN];*/
/*  int len; */

  return(e->action->vacuous);
/*
  len=strlen(e->action->label);
  if ((e->action->initial) && (e->action->label[len-1]=='+')) {
    strcpy(inverse,e->action->label);
    inverse[len-1]='-';
    if (!in_action_set(action(inverse,len),A)) return(TRUE);
  } else if ((!(e->action->initial)) && (e->action->label[len-1]=='-')) {
    strcpy(inverse,e->action->label);
    inverse[len-1]='+';
    if (!in_action_set(action(inverse,len),A)) return(TRUE);
  }
  return(FALSE);
*/
}

/*****************************************************************************/
/* Add reset rules created by sequencing.                                    */
/*****************************************************************************/

rulesetADT add_reset_rules(rulesetADT R,TERstructADT S0,TERstructADT S1,
			   bool Rp)
{
  eventsetADT E,F;
  eventsADT reset;

  reset = event(action("reset", 5), 0, 0, 0, NULL);
  for(E = S0->last; E; E = E->link)
    for(F = S1->first; F; F = F->link) 
      if(((Rp) && (vacuous(E->event,S0->A))) ||
	 ((!Rp) && (!vacuous(E->event,S0->A)))) 
	R = add_rule(R, reset, F->event, F->event->lower, F->event->upper,
		     TRIGGER,F->event->dist,F->event->exp,F->event->assign);
  
  if (Rp)
    for(E = S1->loop; E; E = E->link)
      for(F = S1->first; F; F=F->link) 
	R = add_rule(R, reset, F->event,F->event->lower, F->event->upper,
		     TRIGGER,F->event->dist,F->event->exp,F->event->assign);
  return(R);
}

/*****************************************************************************/
/* Add rules created by sequencing.                                          */
/*****************************************************************************/

rulesetADT add_rules(rulesetADT R, TERstructADT S0, TERstructADT S1, bool Rp,
		     bool ifloop = FALSE)
{
  eventsetADT E,F;

  for(E = S0->last; E; E = E->link) {
    for(F = S1->first; F; F = F->link) {
      if(((Rp) && (vacuous(E->event,S0->A))) ||
	 ((!Rp) && (!vacuous(E->event,S0->A)))) {
	if (!ifloop ||
	    (strcmp(E->event->action->label,F->event->action->label) != 0 &&
	     strcmp(E->event->exp,F->event->exp) != 0)) {
	  R = add_rule(R, E->event, F->event, 
		       F->event->lower, 
		       F->event->upper,TRIGGER,
		       F->event->dist,F->event->exp,F->event->assign);
	}
      }
    }
  }
   
  if(Rp) {
    for(E = S1->loop; E; E = E->link) {
      for(F = S1->first; F; F=F->link) {
	if (!ifloop ||
	    strcmp(E->event->action->label,F->event->action->label) != 0 ||
	    strcmp(E->event->exp,F->event->exp) != 0) {
	  R = add_rule(R, E->event, F->event, 
		       F->event->lower, 
		       F->event->upper,TRIGGER,
		       F->event->dist,F->event->exp,F->event->assign);
	}
      }
    }
  }
  
  return(R);
}

/*****************************************************************************/
/* Add conflicts created by guarded command.                                 */
/*****************************************************************************/

conflictsetADT add_conflicts(conflictsetADT C,TERstructADT S0,TERstructADT S1)
{
  eventsetADT L,R;

  for (L=S0->O;L;L=L->link) {
    for (R=S1->O;R;R=R->link)
      C=add_conflict(C, L->event, R->event);
  }
  return(C);
}

/*****************************************************************************/
/* Add conflicts created by guarded command.                                 */
/*****************************************************************************/

conflictsetADT add_conflicts_wait(conflictsetADT C,TERstructADT S0,
				  TERstructADT S1)
{
  eventsetADT L,R;

  for (L=S0->I;L;L=L->link) {
    for (R=S1->I;R;R=R->link)
      C=add_conflict(C, L->event, R->event);
  }
  return(C);
}

/*****************************************************************************/
/* Add conflicts created by a loop.                                          */
/*****************************************************************************/

conflictsetADT add_loop_conflicts(conflictsetADT Cp,TERstructADT S0,
				  TERstructADT S1)
{
  eventsetADT L,R;

  for (L=S0->last;L;L=L->link) 
    for (R=S1->loop;R;R=R->link) 
      Cp=add_conflict(Cp,L->event,R->event);
  return(Cp);
}
  
/*****************************************************************************/
/* Delete a timed event-rule structures.                                     */
/*****************************************************************************/

void TERSdelete(TERstructADT S)
{
  if (S)
    {
      delete_action_set(S->A);
      delete_event_set(S->I);
      delete_event_set(S->O);
      delete_event_set(S->first);
      delete_event_set(S->last);
      delete_event_set(S->loop);

      //AMS additions
      delete_event_set(S->CT);
      delete_event_set(S->CP);
      delete_rule_set(S->DPCT);
      delete_rule_set(S->CPCT);
      delete_rule_set(S->CTCP);
      delete_rule_set(S->CPDT);
      delete_rule_set(S->DTCP);

      delete_rule_set(S->R);
      delete_rule_set(S->Rp);
      delete_conflict_set(S->C);      
      //delete_conflict_set(S->Cp);
      free(S);
    }
}
  
/*****************************************************************************/
/* Compose two timed event-rule structures.                                  */
/*****************************************************************************/

TERstructADT TERScompose(TERstructADT S0, TERstructADT S1, char* op) 
{
  TERstructADT S;
  eventsetADT tempE;


  /*
  //Do some DTCP stuff  **AMS**
  if (S1->CP && !S1->CT) {
    S1->DTCP = add_rule(S1->DTCP, S0->last->event, S1->CP->event,
		       S1->CP->event->lower, S1->CP->event->upper,
		       CONT, S1->CP->event->dist, "");

		       }*/
  
  if(S0 == 0)  return S1;
  if(S1 == 0)  return S0;

  S=TERSempty();
  S->A = union_actions(S0->A,S1->A);
  S->O = union_events(S0->O,S1->O);
  tempE = union_events(S0->I,S1->I); 
  S->I = subtract_events(tempE,S->O);
  delete_event_set(tempE);

  //AMS additions
  S->CT = union_events(S0->CT,S1->CT);
  S->CP = union_events(S0->CP,S1->CP);
  S->DPCT = union_rules(S0->DPCT,S1->DPCT);
  S->CPCT = union_rules(S0->CPCT,S1->CPCT);
  S->CTCP = union_rules(S0->CTCP,S1->CTCP);
  S->CPDT = union_rules(S0->CPDT,S1->CPDT);	
  S->DTCP = union_rules(S0->DTCP,S1->DTCP);
  
  S->R = union_rules(S0->R,S1->R);
  S->Rp = union_rules(S0->Rp,S1->Rp);
  S->C = union_conflicts(S0->C,S1->C);
  S->Cp = union_conflicts(S0->Cp,S1->Cp);
  
  if(strcmp(op,";") == 0 || strcmp(op, "->") == 0 ||
     strcmp(op,"s") == 0) {
    if (strcmp(op,"s") == 0) {
      S->R = add_rules(S->R, S0, S1, FALSE,TRUE);
      S->Rp = add_rules(S->Rp,S0,S1,TRUE,TRUE);
      //take the stuff out of initial marked that isn't
      S->R = new ruleset_tag;
      if (S->Rp) 
	S->R = S->Rp;
       
      for (rulesetADT step = S->R; step; step = step->link) {
	if (step->link && !step->link->link) {
	  S->Rp = step->link;
	  step->link = NULL;
	  break;
	}
      }
    }
    else {
      S->R = add_rules(S->R, S0, S1, FALSE);
      S->Rp = add_rules(S->Rp,S0,S1,TRUE);
    }
    S->R = add_reset_rules(S->R,S0,S1,TRUE);
    S->Cp = add_loop_conflicts(S->Cp,S0,S1);    

    if(!S0->last)// && !S1->last)
      S->first = union_events(S0->first, S1->first);
    else if(S0->first) 
      S->first = copy_events(S0->first);
    else 
      S->first = copy_events(S1->first);
    
    if(strcmp(op, "->") == 0)
      S->last = copy_events(S0->last);
    else if ((S1->first) && (S1->last)) 
      S->last = copy_events(S1->last);
    else 
      S->last = union_events(S0->last,S1->last); 

    S->loop = copy_events(S0->loop);
    //S->loop = copy_events(S1->loop);
  }
  else if (strcmp(op,":") == 0) {
    S->R = add_rules(S->R, S0, S1, FALSE);
    S->Rp = add_rules(S->Rp,S0,S1,TRUE);
    S->R = add_reset_rules(S->R,S0,S1,TRUE);
    S->Cp = add_loop_conflicts(S->Cp,S0,S1);

    S->first = copy_events(S0->first);
    S->last = copy_events(S1->last);

    S->loop = copy_events(S0->loop);
  }
  else {
    if(strcmp(op, "|") == 0) 
      S->C = add_conflicts(S->C, S0, S1);
    if(strcmp(op, "#") == 0) 
      S->C = add_conflicts_wait(S->C, S0, S1);
    S->first = union_events(S0->first, S1->first);
    S->last = union_events(S0->last, S1->last);
    S->loop = union_events(S0->loop, S1->loop);
  }
  
  TERSdelete(S0);
  TERSdelete(S1);

  return (S);
}

/*****************************************************************************/
/* Rename a timed event-rule structures based on the events of another.      */
/*****************************************************************************/

TERstructADT TERSrename(TERstructADT S0,TERstructADT S1) 
{
  if(S0 == 0)
    S0 = TERSempty();
  if(S1 == 0)
    S1 = TERSempty();
  
  eventsetADT E0;
  
  E0=union_events(S0->I,S0->O);
  //E0=union_events(E0, S0->CP);
  E0=union_events(E0, S0->CT);
  S1->I=rename_event_set(E0,S1->I); 
  S1->O=rename_event_set(E0,S1->O); 
/*  S1->I=rename_event_set(S0->I,S1->I); 
  S1->O=rename_event_set(E0,S1->O); */
  S1->first=rename_event_set(E0,S1->first);
  S1->last=rename_event_set(E0,S1->last); 
/*  S1->last=rename_event_set(S0->I,S1->last); */

  //AMS Additions
  S1->CT=rename_event_set(E0,S1->CT);
  //S1->CP=rename_event_set(E0,S1->CP);  
  S1->DPCT=rename_rule_set(E0,S1->DPCT);
  S1->CPCT=rename_rule_set(E0,S1->CPCT);
  S1->CTCP=rename_rule_set(E0,S1->CTCP);
  S1->CPDT=rename_rule_set(E0,S1->CPDT);
  S1->DTCP=rename_rule_set(E0,S1->DTCP);  
 
  S1->loop=rename_event_set(E0,S1->loop);
  S1->R=rename_rule_set(E0,S1->R);
  S1->Rp=rename_rule_set(E0,S1->Rp);
  S1->C=rename_conflict_set(E0,S1->C);
  S1->Cp=rename_conflict_set(E0,S1->Cp);
  delete_event_set(E0);

  return(S1);
}

/*****************************************************************************/
/* Create a timed event-rule structure for an action.                        */
/*****************************************************************************/

TERstructADT TERS(actionADT A, bool input,
		  int lower, int upper, bool is_vhdl,
		  const char *level, char *dist,
		  int occur, const string&data)
{
  char errmsg[80];
  TERstructADT S;
  
  if(level == NULL)
    level = "[true]";
  
  if ((A->type&(IN|OUT|DUMMY))) 
    S = TERSaction(A, input, lower, upper, is_vhdl, occur, level, dist,data);
  //Regular CP and CT thing
  else if(A->type & CONT && !input)
    S = TERSCont(A, input, lower, upper, is_vhdl, occur, level, dist, data);
  //An assign arc, input means nothing for CONTS
  else if(A->type & CONT && input)
    S = TERSassign(A, input, lower, upper, is_vhdl, occur, level, dist, data);
  else if(A->type == ACTIVE)
    return(TERSempty()); 
  else if(A->type == PASSIVE)
    return(TERSempty());
  else{
    if (strcmp(A->label,"error")==0) {
      sprintf(errmsg,"construct on this line is not supported.");
      err_msg("construct on this line is not supported");
      return(TERSempty());
    } else {
      sprintf(errmsg,"%s is an undefined action.",A->label);
      err_msg("undefined action '", A->label, "'");
      return(TERSempty());
    }
  }
  return(S);
}

/*****************************************************************************/
/* Create a timed event-rule structure for a probe.                          */
/*****************************************************************************/
/*
TERstructADT probe(actionADT A,int data)
{
  char errmsg[80];
  TERstructADT S;
  char label[MAXTOKEN];

  if (A->type==PASSIVE) {
    if (data >= A->idata) {
      sprintf(errmsg,"Data value of %d is out of range for %s.",data,A->label);
      //yyerror(errmsg);
      return(TERSempty());
    } else {
      sprintf(label,"%s%di+",A->label,data);
      S=TERSaction(action(label,strlen(label)),FALSE,(-1),(-1),FALSE);
    }
  } else if (A->type==UNKNOWN) {
    sprintf(errmsg,"%s is an undefined action.",A->label);
    //yyerror(errmsg);
    return(TERSempty());
  } else {
    sprintf(errmsg,"%s must be passive to be probed.",A->label);
    //yyerror(errmsg);
    return(TERSempty());
  }
  return(S);
}
*/

/*****************************************************************************/
/* Modify distribution on an event for choice probabilities.                 */
/*****************************************************************************/

void TERSmodifydist(TERstructADT S,double dist)
{ 
  eventsetADT cur_event;
  char newdist[128];

  sprintf(newdist,"S(%g)",dist);
  for (cur_event=S->O;cur_event;cur_event=cur_event->link) {
    if (cur_event->event->dist) free(cur_event->event->dist);
    cur_event->event->dist=CopyString(newdist);
  }
}

/*****************************************************************************/
/* Mark all rules in timed event-rule structure as ordering.                 */
/*****************************************************************************/

void TERSmarkord(TERstructADT S)
{ 
  rulesetADT cur_rule;

  for (cur_rule=S->R; cur_rule; cur_rule=cur_rule->link) 
    cur_rule->rule->type=ORDERING;
  for (cur_rule=S->Rp; cur_rule; cur_rule=cur_rule->link) 
    cur_rule->rule->type=ORDERING;
}

/*****************************************************************************/
/* Mark all rules in timed event-rule structure as assumptions.              */
/*****************************************************************************/

void TERSmarkassump(TERstructADT S)
{ 
  rulesetADT cur_rule;

  for (cur_rule=S->R; cur_rule; cur_rule=cur_rule->link) 
    cur_rule->rule->type=ASSUMPTION;
  for (cur_rule=S->Rp; cur_rule; cur_rule=cur_rule->link) 
    cur_rule->rule->type=ASSUMPTION;
}

/*****************************************************************************/
/* Make a timed event-rule structure loop.                                   */
/*****************************************************************************/

TERstructADT TERSmakeloop(TERstructADT S)
{ 
  if(S) {
    S->loop = S->last;
    S->last = NULL;
  }
  return(S);
}

/*****************************************************************************/
/* Make a timed event-rule structure repetitive.                             */
/*****************************************************************************/

TERstructADT TERSrepeat(TERstructADT S0,char* op) 
{
  TERstructADT S1;
  eventsetADT last;

/*
  TERstructADT S,Sp,S2,S3;
  eventsADT e,f;
  rulesetADT cur_rule
  eventsetADT E0;
  eventsADT l,r;
  conflictsetADT cur_conflict;
*/
  if(S0) {
    last = S0->last;
    S0->last = NULL;
    S1 = TERScopy(S0); 
    S0 = TERScompose(S0, S1, op);
    S0->last = last;
    //S0->loop = NULL;
  }

/*
  S=TERScopy(S0);

  Sp=TERScopy(S0);
  E0=union_events(Sp->I,Sp->O);
  for (cur_rule=Sp->Rp;cur_rule;cur_rule=cur_rule->link) {
    e=cur_rule->rule->enabling;
    f=rename_event(E0,cur_rule->rule->enabled);
    cur_rule->rule=rule(e,f,cur_rule->rule->lower,cur_rule->rule->upper);
  }
  for (cur_conflict=Sp->Cp;cur_conflict;cur_conflict=cur_conflict->link) {
    l=rename_event(E0,cur_conflict->conflict->left);
    r=cur_conflict->conflict->right;
    cur_conflict->conflict=conflict(l,r);
  }
  S0=TERScopy(S);
  S1=TERScopy(S);
  S1=TERScompose(S0,TERSrename(S0,S1),"||");

  S1->R=union_rules(S1->R,Sp->Rp);
  S1->Rp=NULL;
  S1->C=union_conflicts(S1->C,Sp->Cp);
  S1->Cp=NULL;

  S0=TERScopy(S);
  S2=TERScompose(S0,TERSrename(S0,S1),"||");

  S2->R=union_rules(S2->R,Sp->Rp);
  S2->Rp=NULL;
  S2->C=union_conflicts(S2->C,Sp->Cp);
  S2->Cp=NULL;
*/

  return(S0);
}

/*****************************************************************************/
/* Emit header for a timed event-rule structure.                             */
/*****************************************************************************/

void emit_header(ostream&out,TERstructADT S)
{
  actionsetADT cur_action;
/*  eventsetADT cur_event; */
  rulesetADT cur_rule;
  conflictsetADT cur_conflict;
  int i,max,nevents,ninputs,nrules,ndisj,nconf,niconf,noconf,nord;
  char* startstate;

/*
  ninputs=0;
  nevents=1;
  for (cur_event=S->I; cur_event; cur_event=cur_event->link) 
    if (cur_event->event->action->type&IN) ninputs++; else nevents++;
  for (cur_event=S->O; cur_event; cur_event=cur_event->link) 
    if (cur_event->event->action->type&IN) ninputs++; else nevents++;
  nevents=nevents+ninputs;
*/
  ninputs=0;
  nevents=1;
  for (cur_action=S->A; cur_action; cur_action=cur_action->link) {
    if ((cur_action->action->type & IN) &&
	!(cur_action->action->type & DUMMY)) {
      if (cur_action->action->label[strlen(cur_action->action->label)-1]=='+'){
	max = Max(cur_action->action);
	for(i=0; i<max; i++)
	  ninputs+=2;
      }
      else if (cur_action->action->inverse && 
	       cur_action->action->inverse->maxoccur==0)
	ninputs=ninputs+(2*cur_action->action->maxoccur);
    }
  }

  for(cur_action=S->A; cur_action; cur_action=cur_action->link) {
    if ((cur_action->action->type & OUT) &&
	!(cur_action->action->type & DUMMY)) {
      if(cur_action->action->label[strlen(cur_action->action->label)-1]=='+'){
	max = Max(cur_action->action);
	for(i=0; i<max; i++)
	  nevents+=2;
      }
      else if (cur_action->action->inverse && 
	       cur_action->action->inverse->maxoccur==0)
	nevents=nevents+(2*cur_action->action->maxoccur);
    }
  }
  for(cur_action=S->A; cur_action; cur_action=cur_action->link) {
    if (((cur_action->action->type & IN) || 
	 (cur_action->action->type & OUT)) &&
	(cur_action->action->type & DUMMY)) {
      if(cur_action->action->label[strlen(cur_action->action->label)-1]=='+'){
	max = Max(cur_action->action);
	for(i=0; i<max; i++)
	  nevents+=2;
      }
      else if (cur_action->action->inverse && 
	       cur_action->action->inverse->maxoccur==0)
	nevents=nevents+(2*cur_action->action->maxoccur);
    }
  }

  //Compute the number of dummy events.
  for(cur_action=S->A; cur_action; cur_action=cur_action->link) {
    if(cur_action->action->type==DUMMY || cur_action->action->type & CONT) {
      max = cur_action->action->maxoccur;
      for(i=0; i<max; i++)
	nevents++;
    }
  }
  nevents = nevents + ninputs;
  nrules=0;
  nord=0;
  for (cur_rule=S->R; cur_rule; cur_rule=cur_rule->link)
    if (cur_rule->rule->type==ORDERING)
      nord++;
    else
      nrules++;
  for (cur_rule=S->Rp; cur_rule; cur_rule=cur_rule->link)
    if (cur_rule->rule->type==ORDERING)
      nord++;
    else
      nrules++;
  //count the cont rules
  for (cur_rule=S->CTCP; cur_rule; cur_rule=cur_rule->link)
      nrules++;
  for (cur_rule=S->CPCT; cur_rule; cur_rule=cur_rule->link)
      nrules++;
  for (cur_rule=S->DTCP; cur_rule; cur_rule=cur_rule->link)
      nrules++;

  ndisj=0;
  for (cur_action=S->A; cur_action; cur_action=cur_action->link)
    if(((cur_action->action->label[strlen(cur_action->action->label)-1]=='-')
	&& (cur_action->action->initial)) ||
       ((cur_action->action->label[strlen(cur_action->action->label)-1]=='+')
	&& (!(cur_action->action->initial)))) {
      max = Max(cur_action->action);
      if ((!(cur_action->action->type==DUMMY)) && (max > 1)) 
	ndisj++;
    }

  startstate = (char*)GetBlock(nevents*sizeof(char)+1);
  string *signalList = new string[nevents];
  int nsignals;
  i=0;
  for (cur_action=S->A; cur_action; cur_action=cur_action->link) 
    if (cur_action->action->label[strlen(cur_action->action->label)-1]=='+') {
      if ((cur_action->action->type & IN) &&
	  !(cur_action->action->type & DUMMY)) {
	signalList[i]=cur_action->action->label;
	signalList[i].erase(signalList[i].length()-1);
	if (cur_action->action->initial) 
	  startstate[i] = '1';
	else 	
	  startstate[i] = '0';
	i++;
      }
    } else if(cur_action->action->inverse && 
	      cur_action->action->inverse->maxoccur==0) 
      {
	if ((cur_action->action->type & IN) &&
	    !(cur_action->action->type & DUMMY)) {
	  signalList[i]=cur_action->action->label;
	  signalList[i].erase(signalList[i].length()-1);
	  if (cur_action->action->initial) 
	    startstate[i] = '1';
	  else 	
	    startstate[i] = '0';
	  i++;
	}
      }

  for (cur_action=S->A; cur_action; cur_action=cur_action->link) 
    if (cur_action->action->label[strlen(cur_action->action->label)-1]=='+') {
      if ((cur_action->action->type & OUT) &&
	  !(cur_action->action->type & DUMMY)) {
	signalList[i]=cur_action->action->label;
	signalList[i].erase(signalList[i].length()-1);
	if (cur_action->action->initial) 
	  startstate[i] = '1';
	else 
	  startstate[i] = '0';
	i++;
      }
    } 
    else if(cur_action->action->inverse && 
	    cur_action->action->inverse->maxoccur==0)
      {
	if ((cur_action->action->type & OUT) &&
	    !(cur_action->action->type & DUMMY)) {
	  signalList[i]=cur_action->action->label;
	  signalList[i].erase(signalList[i].length()-1);
	  if (cur_action->action->initial) 
	    startstate[i] = '1';
	  else 
	    startstate[i] = '0';
	  i++;
	}
      }

  for (cur_action=S->A; cur_action; cur_action=cur_action->link) 
    if (cur_action->action->label[strlen(cur_action->action->label)-1]=='+') {
      if ((cur_action->action->type & IN) &&
	  (cur_action->action->type & DUMMY)) {
	signalList[i]=cur_action->action->label;
	signalList[i].erase(signalList[i].length()-1);
	if (cur_action->action->initial) 
	  startstate[i] = '1';
	else 	
	  startstate[i] = '0';
	i++;
      }
    } else if(cur_action->action->inverse && 
	      cur_action->action->inverse->maxoccur==0) 
      {
	if ((cur_action->action->type & IN) &&
	    (cur_action->action->type & DUMMY)) {
	  signalList[i]=cur_action->action->label;
	  signalList[i].erase(signalList[i].length()-1);
	  if (cur_action->action->initial) 
	    startstate[i] = '1';
	  else 	
	    startstate[i] = '0';
	  i++;
	}
      }

  for (cur_action=S->A; cur_action; cur_action=cur_action->link) 
    if (cur_action->action->label[strlen(cur_action->action->label)-1]=='+') {
      if ((cur_action->action->type & OUT) &&
	  (cur_action->action->type & DUMMY)) {
	signalList[i]=cur_action->action->label;
	signalList[i].erase(signalList[i].length()-1);
	if (cur_action->action->initial) 
	  startstate[i] = '1';
	else 
	  startstate[i] = '0';
	i++;
      }
    } 
    else if(cur_action->action->inverse && 
	    cur_action->action->inverse->maxoccur==0)
      {
	if ((cur_action->action->type & OUT) &&
	    (cur_action->action->type & DUMMY)) {
	  signalList[i]=cur_action->action->label;
	  signalList[i].erase(signalList[i].length()-1);
	  if (cur_action->action->initial) 
	    startstate[i] = '1';
	  else 
	    startstate[i] = '0';
	  i++;
	}
      }
  startstate[i]='\0';
  nsignals=i;
  niconf=0;
  noconf=0;
  nconf=0;
  for (cur_conflict=S->C; cur_conflict; cur_conflict=cur_conflict->link)
    if (cur_conflict->conflict->conftype==(ICONFLICT | OCONFLICT))
      nconf++;
    else if (cur_conflict->conflict->conftype==ICONFLICT)
      niconf++;
    else
      noconf++;
  for (cur_conflict=S->Cp; cur_conflict; cur_conflict=cur_conflict->link)
    if (cur_conflict->conflict->conftype==(ICONFLICT | OCONFLICT))
      nconf++;
    else if (cur_conflict->conflict->conftype==ICONFLICT)
      niconf++;
    else
      noconf++;

  //This statement will extract the initial rates from the structure
  //and remove the single looping rules
  list<rulesADT> toRemove;
  list<rulesADT> toRemoveR;
  list<actionADT> toRemoveA;
  for (rulesetADT initial = S->Rp; initial; initial = initial->link) {
    string var = initial->rule->enabling->exp;
    string lvalue, uvalue;
    if (var.find(":=") != string::npos) {
      if (strcmp(initial->rule->enabling->action->label,
		 initial->rule->enabled->action->label) == 0) {
	if (var.find("'") != string::npos) {
	  //remove rule
	  /*	  printf("Rp:%s - %s -> %s\n",
		 initial->rule->enabling->action->label,
		 initial->rule->enabling->exp,
		 initial->rule->enabled->action->label);*/
	  toRemove.push_back(initial->rule);
	  toRemoveA.push_back(initial->rule->enabling->action);
	  for (rulesetADT reset = S->R; reset; reset = reset->link) {
	    /*	    printf("R:%s - %s -> %s\n",
		   reset->rule->enabling->action->label,
		   reset->rule->enabled->exp,
		   reset->rule->enabled->action->label);*/
	    if ((strcmp(reset->rule->enabling->action->label,"reset")==0)&&
		(strcmp(reset->rule->enabled->action->label,
			initial->rule->enabled->action->label) == 0)&&
		(strcmp(reset->rule->enabled->exp,
			initial->rule->enabling->exp)==0)) {
	      /*	      printf("Removing %s -> %s\n",
		     reset->rule->enabling->action->label,
		     reset->rule->enabled->action->label);*/
	      toRemoveR.push_back(reset->rule);
	    }
	  }
	}
	if (var.find(")") != string::npos) {
	  var = var.substr(var.find(")")+4, string::npos);
	  var = var.substr(0,var.find("'"));
	} else {
	  var = var.substr(var.find("[")+1, string::npos);
	  var = var.substr(0,var.find("'"));
	}
      }
      else {
	var = var.substr(var.rfind(" & ")+3, string::npos);
	var = var.substr(0, var.find("'"));
      }
      lvalue = initial->rule->enabling->exp;
      lvalue = lvalue.substr(lvalue.find(":=")+2,string::npos);
      if (lvalue.find(";") == string::npos) {
	lvalue = lvalue.substr(0,lvalue.find("]"));
	uvalue = lvalue;
      }
      else {
	lvalue = lvalue.substr(lvalue.find("{")+1,string::npos);
	lvalue = lvalue.substr(0,lvalue.find(";"));
	uvalue = initial->rule->enabling->exp;
	uvalue = uvalue.substr(uvalue.find(";")+1,string::npos);
	uvalue = uvalue.substr(0,uvalue.find("}"));
      }
      for (actionsetADT cur_action = S->A; cur_action;
	   cur_action=cur_action->link) {
	if (strcmp(cur_action->action->label,var.c_str()) == 0) {
	  cur_action->action->linitrate = atoi(lvalue.c_str());
	  cur_action->action->uinitrate = atoi(uvalue.c_str());
	  break;
	}
      }
    }
  }
  for (list<rulesADT>::iterator remove = toRemove.begin();
       remove != toRemove.end(); remove++) {
    S->Rp = subtract_rules(S->Rp, create_rule_set(*remove));
    nrules--;
  }
  for (list<rulesADT>::iterator remove = toRemoveR.begin();
       remove != toRemoveR.end(); remove++) {
    S->R = subtract_rules(S->R, create_rule_set(*remove));
    nrules--;
  }
  for (list<actionADT>::iterator remove = toRemoveA.begin();
       remove != toRemoveA.end(); remove++) {
    S->A = rm_action(S->A, (*remove));
    nevents--;
  }

  /* Find initial rate values */
  for (rulesetADT initial = S->Rp; initial; initial = initial->link) {
    string var = initial->rule->enabled->exp;
    string lvalue, uvalue;
    if ((var.find(":=") != string::npos) && (var.find("'") != string::npos)) {
      //cout << "Working on " << var << endl;
      bool change;
      do {
	change=false;
	for (int s=0;s<nsignals;s++) 
	  if (signalList[s].find("dot")==string::npos) {
	    if (var.find(signalList[s])!=string::npos) {
	      if (startstate[s]=='0')
		var.replace(var.find(signalList[s]),
			    signalList[s].length(),"0");
	      else 
		var.replace(var.find(signalList[s]),
			    signalList[s].length(),"1");
	      change=true;
	      //cout << "New string " << var << endl;
	    }
	  } else {
	    if (var.find(signalList[s]+":=TRUE")!=string::npos) {
	      var.replace(var.find(signalList[s]+":=TRUE"),
			  signalList[s].length()+6,"1");
	      change=true;
	    }
	    if (var.find(signalList[s]+":=FALSE")!=string::npos) {
	      var.replace(var.find(signalList[s]+":=FALSE"),
			  signalList[s].length()+7,"1");
	      change=true;
	    }
	    if (var.find("~("+signalList[s]+")")!=string::npos) {
	      var.replace(var.find("~("+signalList[s]+")"),
			  signalList[s].length()+3,"1");
	      change=true;
	    }
	    //cout << "New string " << var << endl;
	  }
	for (cur_action=S->A;cur_action;cur_action=cur_action->link){
	  if (cur_action->action->type & CONT + VAR) {
	    string label=cur_action->action->label;
	    while (var.find(label+">=")!=string::npos) {
	      string value=
		var.substr(var.find(label+">=")+label.length()+2,
			   var.find_first_of(")&",var.find(label+">="))-
			   (var.find(label+">=")+label.length()+2));
	      int intval=atoi(value.c_str());
	      if (cur_action->action->linitval >= intval) {
		var.replace(var.find(label+">="),
			    label.length()+2+value.length(),"1");
	      } else if (cur_action->action->uinitval <= intval) {
		var.replace(var.find(label+">="),
			    label.length()+2+value.length(),"0");
	      } else {
		var.replace(var.find(label+">="),
			    label.length()+2+value.length(),"?");
		printf("WARNING: Cannot determine %s>=%s value in the initial state",label.c_str(),value.c_str());
		fprintf(lg,"WARNING: Cannot determine %s>=%s value in the initial state",label.c_str(),value.c_str());
	      }
	    }
	  }
	}
	while (var.find("~(0)")!=string::npos) {
	  var.replace(var.find("~(0)"),4,"1");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("(0)")!=string::npos) {
	  var.replace(var.find("(0)"),3,"0");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("~(1)")!=string::npos) {
	  var.replace(var.find("~(1)"),4,"0");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("(1)")!=string::npos) {
	  var.replace(var.find("(1)"),3,"1");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("0&")!=string::npos) {
	  var.replace(var.find("0&"),2,"0 &");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("1&")!=string::npos) {
	  var.replace(var.find("1&"),2,"1 &");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("&0")!=string::npos) {
	  var.replace(var.find("&0"),2,"& 0");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("&1")!=string::npos) {
	  var.replace(var.find("&1"),2,"& 1");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("1 & ")!=string::npos) {
	  var.replace(var.find("1 & "),4,"");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find(" & 1")!=string::npos) {
	  var.replace(var.find(" & 1"),4,"");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("0 & 0")!=string::npos) {
	  var.replace(var.find("0 & 0"),5,"0");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("0 & 1")!=string::npos) {
	  var.replace(var.find("0 & 1"),5,"0");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("1 & 0")!=string::npos) {
	  var.replace(var.find("1 & 0"),5,"0");
	  change=true;
	  //cout << "New string " << var << endl;
	}
	while (var.find("1 & 1")!=string::npos) {
	  var.replace(var.find("1 & 1"),5,"1");
	  change=true;
	  //cout << "New string " << var << endl;
	}
      } while (change);
      //      cout << "Final " << var << endl;
      string variable;
      string repl;
      if (var.find("0 &")==string::npos) {
	while (var.find(":=")!=string::npos) {
	  lvalue = var.substr(var.find(":=")+2,string::npos);
	  variable = 
	    var.substr(var.find_first_not_of("()|",var.find_first_of("(")),
		       var.find("'")-
		       var.find_first_not_of("()|",var.find_first_of("(")));
	  //cout << "lvalue=" << lvalue << endl;
	  if (lvalue.find(";") == string::npos) {
	    lvalue = lvalue.substr(0,lvalue.find(")"));
	    uvalue = lvalue;
	    repl=variable+"'dot:="+lvalue;
	  }
	  else {
	    uvalue = lvalue;
	    lvalue = lvalue.substr(lvalue.find("{")+1,string::npos);
	    lvalue = lvalue.substr(0,lvalue.find(";"));
	    uvalue = uvalue.substr(uvalue.find(";")+1,string::npos);
	    uvalue = uvalue.substr(0,uvalue.find("}"));
	    repl=variable+"'dot:={"+lvalue + ";" + uvalue + "}";
	  }
	  
	  string temp;
	  temp = variable + "dot_";
	  if (atoi(lvalue.c_str()) < 0) temp += "m";
	  temp += lvalue;
	  if (atoi(lvalue.c_str()) != atoi(uvalue.c_str())) {
	    temp += "_";
	    if (atoi(uvalue.c_str()) < 0) temp += "m";
	    temp += uvalue;
	  } 
	  for (int s=0;s<nsignals;s++)
	    if (signalList[s]==temp) {
	      startstate[s]='1';
	    }
	  for (actionsetADT cur_action = S->A; cur_action;
	       cur_action=cur_action->link) {
	    if (strcmp(cur_action->action->label,variable.c_str()) == 0) {
	      cur_action->action->linitrate = atoi(lvalue.c_str());
	      cur_action->action->uinitrate = atoi(uvalue.c_str());
	      break;
	    }
	  }
	  var=var.replace(var.find(repl),repl.length(),"");
	  //cout << "current var = " << var << endl;
	}
      }
    }
  }

  out << ".e " << nevents << endl
      << ".i " << ninputs << endl
      << ".r " << nrules  << endl
      << ".n " << nord	  << endl
      << ".d " << ndisj	  << endl
      << ".m " << niconf  << endl
      << ".o " << noconf  << endl
      << ".c " << nconf	  << endl;
  if (startstate[0]) out << ".s " << startstate << endl;
  else out << ".s 0" << endl;
 
  bool continuous = FALSE;
  for (cur_action=S->A;cur_action;cur_action=cur_action->link){
    if (cur_action->action->type & CONT){
      continuous = TRUE;
      break;
    }
  }
  if (continuous) {
    out << ".q ";
    for (cur_action=S->A;cur_action;cur_action=cur_action->link){
      if (cur_action->action->type & CONT + VAR) {
	out << "<" << cur_action->action->label << "=";
	if (cur_action->action->uinitval == cur_action->action->linitval) {
	  out << cur_action->action->uinitval;
	}
	else {
	  out << "{" << cur_action->action->linitval << ";"
	      << cur_action->action->uinitval << "}";
	}
	out << ">";
      }
    }
    out << "\n.v ";
    
    for (cur_action=S->A;cur_action;cur_action=cur_action->link){
      if (cur_action->action->type & CONT + VAR){
	out << "<" << cur_action->action->label << "=";
	if (cur_action->action->uinitrate == cur_action->action->linitrate) {
	  out << cur_action->action->uinitrate;
	}
	else {
	  out << "{" << cur_action->action->linitrate << ";"
	      << cur_action->action->uinitrate << "}";
	}
	out << ">";
	
      }
    }
    out << endl;
  }
  out << "reset" << endl;
  free(startstate);
}

// ZHEN(Done): emit_assignments should walk action set S->A, see below, for each action that has a non-empty
// set of assignments, it should output a line of the form to the screen:
// #@.assignments {<label=[lhs:=rhs]>...}
/*****************************************************************************/
/* Emit assignments for a timed event-rule structure.                   */
/*****************************************************************************/

void emit_assignments(ostream&out,TERstructADT S)
{
  // ZHEN(Done): Send to out instead of cout
  //out << "#"                      << endl
  //    << "# " << "ASSIGNMENTS:" << endl
  //    << "#"                      << endl;
  out << "@";
  for (actionsetADT cur_action(S->A);cur_action;cur_action=cur_action->link){
    if (cur_action->action->type==DUMMY) {
      assignList curr_list_assigns = cur_action->action->list_assigns;
      while (curr_list_assigns != NULL) {
	// ZHEN(Done): Change label to "d_" + labal + "1"
	char* new_label_tmp = cur_action->action->label;
	// Remove the $ sign at the beginning of the label. Need to add "d_" and "1" to new_label
	char* new_label = (char*) GetBlock(strlen(new_label_tmp)+4);
	strncpy(new_label, new_label_tmp+1, strlen(new_label_tmp));
	char tmp1[255] ="d_";
	char tmp2[2] ="1";
       	strcat(new_label, tmp2);
	new_label = strcat(tmp1, new_label);
	for (int i = 0; i < strlen(new_label); i++)
	  if (new_label[i]=='+') new_label[i]='P';
	out  << "<"
	     << new_label
	     << "="
	     << "[" << curr_list_assigns->var 
             << ":="
             << curr_list_assigns->assign << "]>";
	curr_list_assigns = curr_list_assigns->next;
      }
    }
  }
}

/*****************************************************************************/
/* Emit events for a timed event-rule structure.                             */
/*****************************************************************************/

void emit_events(ostream&out,TERstructADT S)
{
  emit_some_events(out, S, IN , "IN", false);
  emit_some_events(out, S, OUT, "OUT", false);
  emit_some_events(out, S, IN , "DUMMY IN", true);
  emit_some_events(out, S, OUT, "DUMMY OUT", true);
  out << "#"               << endl
      << "# DUMMY EVENTS:" << endl
      << "#"               << endl;
  for (actionsetADT cur_action(S->A);cur_action;cur_action=cur_action->link){
    if (cur_action->action->type==DUMMY || cur_action->action->type & CONT){
      for(int i(1); i<=Max(cur_action->action); i++){
	declare_event(out, cur_action->action, i, '\n');
      }
    }
  }
}

// Print a rule set:
void emit_rule_set(ostream& out, bool ord, rulesetADT rules, int epsilon,
		   bool fail,const char *type){
  out << "#"                      << endl
      << "# " << type << "RULES:" << endl
      << "#"                      << endl;
  for (rulesetADT cur_rule(rules); cur_rule; cur_rule=cur_rule->link){
    rulesADT rule(cur_rule->rule);
    if ((rule->type==TRIGGER && !ord) || 
	(rule->type==ASSUMPTION && !ord) || 
	(rule->type==ORDERING && ord)) {
      out << rule->enabling << ' ' << rule->enabled << ' ';
      if(strcmp(rule->exp, "[true]") &&
	 strcmp(rule->exp, "[(true)]")){
	if (fail) {
	  out << "[~fail&";
	  rule->exp[0]='(';
	  rule->exp[strlen(rule->exp)-1]=')';
	}
	out << rule->exp;
	if (fail) {
	  out << "]";
	}
	if ((rule->type==ORDERING)&&(rule->exp[strlen(rule->exp)-1]!='d')) {
	  out << 'd';
	}
      } else if (fail) {
	out << "[~fail]";
      }
      out << ' ' << epsilon;
      if (rule->type==ASSUMPTION){
	out << " a a";
      }
      else {
	out << ' ' << rule->lower;
	if(rule->upper==INFIN) 
	  out << " inf";
	else 
	  out << ' ' << rule->upper;
      }
      if (rule->dist)
	out << ' ' << rule->dist;
      /*if(strcmp(rule->assign, "[true]") &&
	 strcmp(rule->assign, "[(true)]"))
	 out << " #" << rule->assign;*/
      out << endl;
    }
  }
}

/*****************************************************************************/
/* Emit rules for a timed event-rule structure.                              */
/*****************************************************************************/
void emit_rules(ostream&out,TERstructADT S,bool ord)
{
  bool fail=false;
  for (actionsetADT cur_action(S->A);cur_action;cur_action=cur_action->link)
    if (strcmp(cur_action->action->label,"fail+")==0)
      fail=true;
  emit_rule_set(out,ord,S->R ,0,fail);
  emit_rule_set(out,ord,S->Rp,1,fail,"INITIAL ");
}

// Print a continuous rule set:
void emit_cont_rule_set(ostream& out, rulesetADT rules, int epsilon,
			const char *type) {
  out << "#"                      << endl
      << "# " << type << "RULES:" << endl
      << "#"                      << endl;
  for (rulesetADT cur_rule(rules); cur_rule; cur_rule=cur_rule->link){
    rulesADT rule(cur_rule->rule);
 
    out << rule->enabling << ' ' << rule->enabled << ' ';
    if(strcmp(rule->exp, "[true]") &&
       strcmp(rule->exp, "[(true)]")){
      out << rule->exp;
      if ((rule->type==ORDERING)&&(rule->exp[strlen(rule->exp)-1]!='d')) {
	out << 'd';
      }
    }
    out << ' ' << epsilon;
    if (rule->type==ASSUMPTION){
      out << " a a";
    }
    else {
      out << ' ' << rule->lower;
      if(rule->upper==INFIN) 
	out << " inf";
      else 
	out << ' ' << rule->upper;
    }
    if (rule->dist)
      out << ' ' << rule->dist;
    out << endl;
      
  }
}

/*****************************************************************************/
/* Emit continuous rules for a timed event-rule structure.                   */
/*****************************************************************************/
void emit_cont_rules(ostream&out,TERstructADT S)
{
  emit_cont_rule_set(out,S->CPCT,0,"CPCT ");
  emit_cont_rule_set(out,S->CTCP,0,"CTCP ");
  emit_cont_rule_set(out,S->DTCP,0,"DTCP ");
}

/*****************************************************************************/
/* Emit mergers for a timed event-rule structure.                            */
/*****************************************************************************/

void emit_mergers(ostream&out,TERstructADT S)
{
  out << "# "         << endl
      << "# MERGERS:" << endl
      << "# "         << endl;
  for(actionsetADT cur_action(S->A); cur_action; cur_action=cur_action->link){
    actionADT act(cur_action->action);
    char last(act->label[strlen(act->label)-1]);
    if (((('-'==last) &&   (act->initial)) ||
	 (('+'==last) && (!(act->initial)))) && !(act->type==DUMMY)) {
      int max(Max(act));
      if (max > 1) {
	for (int i(1);i<=max;i++){
	  out << act->label << '/' << i << ' ';
	}
	out << endl;
      }
    }
  }
}

//Output a whole set of conflicts:
ostream& operator<<(ostream&out,conflictsetADT cur_conflict){
  out << endl << "#" << endl;
  while(cur_conflict){
    conflictsADT conflict(cur_conflict->conflict);
    out << conflict->left << ' ' << conflict->right << endl;
    cur_conflict = cur_conflict->link;
  }
  return out;
}

void output_conflicts(ostream&out,int type,conflictsetADT cur_conflict,
			  char * com) 
{
  out << "#" << endl << "# " << com << " CONFLICTS:" << endl << "#" << endl;
  while(cur_conflict){
    conflictsADT conflict(cur_conflict->conflict);
    if (conflict->conftype==type) 
      out << conflict->left << ' ' << conflict->right << endl;
    cur_conflict = cur_conflict->link;
  }
}

void emit_conflicts(ostream&out,TERstructADT S)
{
  output_conflicts(out,ICONFLICT,S->C,"INPUT");
  output_conflicts(out,ICONFLICT,S->Cp,"LOOPING INPUT");
  output_conflicts(out,OCONFLICT,S->C,"OUTPUT");
  output_conflicts(out,OCONFLICT,S->Cp,"LOOPING OUTPUT");
  output_conflicts(out,(ICONFLICT | OCONFLICT),S->C,"");
  output_conflicts(out,(ICONFLICT | OCONFLICT),S->Cp,"LOOPING");
}

/*****************************************************************************/
/* Check if a timed event-rule structure is closed.                          */
/*****************************************************************************/

void checkclosed(TERstructADT S)
{
  eventsetADT cur_event;

  if (S->I) {
    printf("WARNING: Timed event-rule structure is not closed!\n");
    printf("         The behavior of these events are not specified:\n");
    for (cur_event=S->I;cur_event;cur_event=cur_event->link) 
      printf("       (%s,%d)\n",cur_event->event->action->label,
	     cur_event->event->occurrence);
  }
}

void delete_conflicts(eventsADT E, TERstructADT S)
{
  /*  conflictsetADT tmp = 0;

  if(S)
    {
      for(eventsetADT cur_event=S->I; cur_event; cur_event = cur_event->link)
	{
	  conflictsADT CC1 = check(E, cur_event->event);
	  conflictsADT CC2 = check(cur_event->event, E);
	  if(CC1)
	    add_conflict(tmp, CC1);
	  if(CC2)
	    add_conflict(tmp, CC2);
	}
      for(eventsetADT cur_event=S->O; cur_event; cur_event = cur_event->link)
	{
	  conflictsADT CC1 = check(E, cur_event->event);
	  conflictsADT CC2 = check(cur_event->event, E);
	  if(CC1)
	    add_conflict(tmp, CC1);
	  if(CC2)
	    add_conflict(tmp, CC2);
	}
      
      conflictsetADT new_C, new_Cp;
      new_C = subtract_conflicts(S->C, tmp);
      new_Cp = subtract_conflicts(S->Cp, tmp);

      delete_conflict_set(tmp);
      delete_conflict_set(S->C);
      delete_conflict_set(S->Cp);

      S->C = new_C;
      S->Cp = new_Cp;
      }*/
}

eventsADT check_event(const string& a_dummy, int occur, const TERstructADT S)
{
  eventsetADT cur_event;
  for(cur_event = S->I; cur_event; cur_event = cur_event->link) {
    if(cur_event->event->action->label==a_dummy && 
       cur_event->event->occurrence==occur)
      return cur_event->event;
  }

  cur_event=S->O;
  for(cur_event = S->O; cur_event; cur_event = cur_event->link) {
    if(cur_event->event->action->label==a_dummy && 
       cur_event->event->occurrence==occur)
      return cur_event->event;
  }
  return 0;
}
    

// Find out if there is conflict in the event_set. If so , 'cnflct' gets
// 'true'; otherwise, 'cnflct' gets false. And the set of exps are returned.
set<string> find_cnflct(char attr, const rule_lst* event_set, 
			const TERstructADT S,
			bool& cnflct)
{
  set<string> ret_val;

  eventsADT tmp_e1 = NULL;
  eventsADT tmp_e2 = NULL;
  cnflct = false;

  for(rule_lst::const_iterator iter = event_set->begin();
      iter != event_set->end(); iter++){

    if(attr=='p')
      tmp_e1=iter->first->enabling;
    else if(attr=='s')
      tmp_e1=iter->first->enabled;

    if(iter->first->exp!=string("[true]"))
      ret_val.insert(iter->first->exp);

    for(rule_lst::const_iterator iter1 = event_set->begin();
	iter1 != event_set->end(); iter1++){
      if(attr=='p')
	tmp_e2=iter1->first->enabling;
      else if(attr=='s')
	tmp_e2=iter1->first->enabled;

      if(tmp_e1==tmp_e2)
	continue;
      else if(check(tmp_e1, tmp_e2))
	cnflct=true;
    }
  }

  return ret_val;
}

// If there is no conflict in e_pred and e_succ, then it maybe safe to remove
// a_dummy.
bool chk_dummy_1(const string& a_dummy, const map<string,rule_lst*>& e_pred, 
		 const map<string,rule_lst*>& e_succ,
		 const TERstructADT S)
{
  set<string> pred_exp, succ_exp;
  bool pred_cnflct, succ_cnflct;

  pred_exp = find_cnflct('p', e_pred.find(a_dummy)->second, S, pred_cnflct);
  succ_exp = find_cnflct('s', e_succ.find(a_dummy)->second, S, succ_cnflct);

  if(pred_cnflct || succ_cnflct){
    
    if(!pred_exp.size() || !succ_exp.size())
      return true;

    return false;
  }
  else
    return true;

  /*  list<list<string> > non_cnflct_set;

  for(rule_lst::const_iterator iter = e_pred[a_dummy]->begin();
      iter != e_pred[a_dummy]->end(); iter++){
    if(iter->first->exp!=string("[true]")){
      if(!non_cnflct_set.size())
	non_cnflct_set = find_cnflct_set(e_succ[a_dummy], S);
      if(is_rvmable(non_cnflct_set))
    }
  }
    
  return true;*/
}

/*
bool check_dummy(const string& a_dummy, map<string,rule_lst*>& e_pred, 
		 map<string,rule_lst*>& e_succ,
		 const TERstructADT S)
{

  //if(chk_dummy_1(a_dummy, e_pred, e_succ, S)==false)
  //return false;

  eventsADT e_dummy=check_event(a_dummy,1,S);
#ifdef DDEBUG
  if(!e_dummy) exit(1);
#endif
  set<eventsADT> pred_dummy;
  for(rule_lst::const_iterator iter = e_pred[a_dummy]->begin();
      iter != e_pred[a_dummy]->end(); iter++)
    pred_dummy.insert(iter->first->enabling);

  // create a set of lower and upper delay, and a set of distribution of the 
  // rules where the a_dummy is the enabling.
  set<pair<int,int> > succ_timing;
  set<string> succ_dist;
  
  for(rule_lst::iterator iter = e_succ[a_dummy]->begin();
      iter != e_succ[a_dummy]->end(); iter++){
    succ_timing.insert(make_pair(iter->first->lower, iter->first->upper));
    if(iter->first->dist)
      succ_dist.insert(iter->first->dist);
  }
  
  // Check all dummy events that conflict with a_dummy:
  // if they have the same sets of enabling events and all the delays of the
  // rules where the a_dummy is enabling are the same, then return true. 
  bool postproc=true;
  for(map<string,rule_lst*>::iterator iter=e_pred.begin(); iter!=e_pred.end();
      iter++){
    eventsADT dummy1=check_event(iter->first,1,S);
#ifdef DDEBUG
    if(!dummy1) exit(1);
#endif
    if(dummy1==e_dummy)
      continue;
    
    if(check(e_dummy, dummy1)){
      // convert list of rules to a set of their enabling events.
      set<eventsADT> tmp_set;
      for(rule_lst::iterator iter1=iter->second->begin(); 
	  iter1!=iter->second->end(); iter1++)
	tmp_set.insert(iter1->first->enabling);

      if(includes(pred_dummy.begin(),pred_dummy.end(),
		  tmp_set.begin(),tmp_set.end()) &&
	 includes(tmp_set.begin(),tmp_set.end(),
		  pred_dummy.begin(),pred_dummy.end())){

	//create a set of lower and upper delay, and a set of probability of 
	//the rules where the dummy1 is the enabling.
	set<pair<int,int> > tmp_timing;
	set<string> tmp_dist;

	for(rule_lst::iterator iter_1 = e_succ[iter->first]->begin();
	    iter_1 != e_succ[iter->first]->end(); iter_1++){
	  tmp_timing.insert(make_pair(iter_1->first->lower, 
				      iter_1->first->upper));
	  if(iter_1->first->dist)
	    tmp_dist.insert(iter_1->first->dist);
	}
	
	if(!(includes(succ_timing.begin(),succ_timing.end(),
		    tmp_timing.begin(),tmp_timing.end()) &&
	   includes(tmp_timing.begin(),tmp_timing.end(),
		    succ_timing.begin(),succ_timing.end()) &&
	   includes(succ_dist.begin(),succ_dist.end(),
		    tmp_dist.begin(),tmp_dist.end()) &&
	   includes(tmp_dist.begin(),tmp_dist.end(),
		    succ_dist.begin(),succ_dist.end())))
	  postproc=false;
      }
    }
  }
  return postproc;
}
*/

/*
void post_process(TERstructADT S, bool self_lp=false)
{
  // for each dummy event, do
  // 1	find all rules that the dummy events are enabled events and the level
  //	expression is "[true]", and stores them to R1.
  // 2	find all rules that the enabling events is the same as the enabled
  //	events and stores them to R2.
  // 3	create new rules that the enabling events are the enabling events of 
  //	the rules of R1, and the enabled events are the enabled events of 
  //	rules of R2, and the level expression is the one of rules of R2.
  // 4	add these new rules into ruleset if both rules from R1 and R2 are from
  //	ruleset, otherwise add these new rules into initial ruleset.

  // subtract these rules from the ruleset or initial ruleset

  // repeat the process until no new rules are generated.
  rulesADT r;

  if(S == 0)
    return;
 
  map<string,rule_lst*> e_pred, e_succ;
  actionsetADT cur_action;
  for(cur_action = S->A; cur_action; cur_action = cur_action->link){
    if(cur_action->action->type & DUMMY){
      pair<rule_lst*,rule_lst*> ed_en = find_rules(cur_action->action, S);
      if(ed_en.second && ed_en.first){
	e_succ[cur_action->action->label]=(ed_en.second);
	e_pred[cur_action->action->label]=(ed_en.first);    
      }
    }
  }
  
  list<string> red_dummy;
  for(map<string,rule_lst*>::iterator iter=e_pred.begin(); iter!=e_pred.end();
      iter++){
    if((e_pred[iter->first])->size()==1 || (e_pred[iter->first])->size()==1)
      if(check_dummy(iter->first, e_pred, e_succ, S))
	red_dummy.push_back(iter->first);
  }

  int f_tag = 0;
  char fn[100];
  sprintf(fn, "t%d", f_tag);
  emitters(0, fn, S);
  set<pair<rulesADT, bool> > junk_rules_1, junk_rules_2;
  
  for(list<string>::iterator iter=red_dummy.begin(); iter!=red_dummy.end();
      iter++, f_tag++){
    // R_enabled and R_enabling store the rules in which the dummy events are 
    // enabled and enabling events, respectively.
    // R_enabled = e_pred[iter->first];
    // R_enabling = e_succ[iter->first];
    char fn[100];
    sprintf(fn, "t%d", f_tag);
    
    if(e_pred[*iter]->empty() || e_succ[*iter]->empty()){
      rule_lst *LT = e_pred[*iter];
      if(e_pred[*iter]->empty()) LT = e_succ[*iter];
      for(rule_lst::iterator b = LT->begin(); b != LT->end(); b++)
	if(b->second == true){
	  S->Rp = subtract_rules(S->Rp, create_rule_set(b->first));
	}
	else{
	  S->R = subtract_rules(S->R, create_rule_set(b->first));
	}
      continue;
    }      
    
    // Check if the cur dummy event conflict with with other dummy events. 
    // if so, check if the cur dummy event can be removed.
    
    // Removing the cur dummy event won't change the semantics.
    bool junky=false;
    set<pair<rulesADT,bool> > new_rules; 
    
    for(rule_lst::iterator r1 = (e_succ[*iter])->begin(); 
	r1 != (e_succ[*iter])->end(); r1++){
      
      if(r1->first->enabling == r1->first->enabled)
	continue;
      if(string(r1->first->exp) == "[true]")
	junky=true;

      bool junky_1=false;
      
      for(rule_lst::iterator r2=(e_pred[*iter])->begin(); 
	  r2!=(e_pred[*iter])->end(); r2++){
	
	if(r2->first->enabling == r2->first->enabled)	  
	  continue;

	if(r2->first->enabling == r1->first->enabled && self_lp)
	  continue;

	if(junky || string(r2->first->exp) == "[true]"){
	  rulesADT RR1 = r1->first;
	  rulesADT RR2 = r2->first;
	  int lower = 0; int upper = 0;
	  if(RR1->lower == INFIN || RR2->lower == INFIN)
	    lower = INFIN;
	  else
	    lower = RR1->lower + RR2->lower;
	  if(RR1->upper == INFIN || RR2->upper == INFIN)
	    upper = INFIN;
	  else
	    upper = RR1->upper + RR2->upper;
	
	  string exp = logic(RR1->exp, RR2->exp);
	  if(((r1->second==true && r2->second==false) ||
	      (r1->second==false && r2->second==true))&&
	     RR2->enabling->action->label != string("reset")) {
	    r = rule(RR2->enabling, RR1->enabled, lower, upper, 
		     RR2->type, false, RR2->dist, exp.c_str());
	    if(!in_rule_set(r, S->Rp)){
	      S->Rp = add_rule(S->Rp,RR2->enabling,RR1->enabled,
			       lower,upper,RR2->type,RR1->dist, 
			       exp.c_str());
	      rulesADT newr=check_rule(S->Rp,RR2->enabling,RR1->enabled);
#ifdef DDEBUG
	      if(!newr){
		cout << "Unexpected error!!!!!" << endl;
		exit(1);
	      }
#endif
	      new_rules.insert(make_pair(newr,true));
	    }
	  }
	  else {
	    r = rule(RR2->enabling, RR1->enabled, lower, upper, 
		     RR2->type, false, RR2->dist, exp.c_str());
	    if(!in_rule_set(r, S->R) && !in_rule_set(r, S->Rp)){
	      S->R = add_rule(S->R,RR2->enabling,RR1->enabled,lower, 
			      upper, RR2->type, RR1->dist, exp.c_str());
	      rulesADT newr=check_rule(S->R,RR2->enabling,RR1->enabled);
#ifdef DDEBUG
	      if(!newr){
		cout<< "Unexpected error!!!!!" << endl;
		exit(1);
	      }
#endif
	      new_rules.insert(make_pair(newr,false));
	    }
	  }
	}
	junky_1=false;
      }
      junky=false;
    }

    // Decide which rules can be removed from the TEL.
    // If the rule->exp != "[true]", then it is stored in junk_r;
    // Otherwise, (pred_cnt/succ_cnt)++;
    // if(pred_cnt==e_pred.size()) then all rules in junk_sr are removed;
    // if(succ_cnt==e_succ.size()) then all rules in junk_pr are removed;
    set<pair<rulesADT,bool> > junk_pr, junk_sr, junk_rules;
    set<pair<rulesADT,bool> >::size_type pred_cnt, succ_cnt;
    
    pred_cnt=succ_cnt=0;
    for(rule_lst::iterator r1 = (e_succ[*iter])->begin(); 
	r1 != (e_succ[*iter])->end(); r1++)
      if(string(r1->first->exp) == "[true]"){
	if(r1->second == true)
	  S->Rp = subtract_rules(S->Rp, create_rule_set(r1->first));
	else
	  S->R = subtract_rules(S->R, create_rule_set(r1->first));
	junk_rules.insert(*r1);
	succ_cnt++;
      }
      else {
	junk_sr.insert(*r1);
      }
    
    for(rule_lst::iterator r1 = (e_pred[*iter])->begin(); 
	r1 != (e_pred[*iter])->end(); r1++)
      if(string(r1->first->exp) == "[true]"){
	if(r1->second == true)
	  S->Rp = subtract_rules(S->Rp, create_rule_set(r1->first));
	else
	  S->R = subtract_rules(S->R, create_rule_set(r1->first));
	junk_rules.insert(*r1);
	pred_cnt++;
      }
      else{
	junk_pr.insert(*r1);
      }

    if(pred_cnt==e_pred[*iter]->size())
      for(set<pair<rulesADT,bool> >::iterator r1 = junk_sr.begin(); 
	  r1 != junk_sr.end(); r1++){
	if(r1->second == true)
	  S->Rp = subtract_rules(S->Rp, create_rule_set(r1->first));
	else
	  S->R = subtract_rules(S->R, create_rule_set(r1->first));
	junk_rules.insert(*r1);
      }
    
    if(succ_cnt==e_succ[*iter]->size())
      for(set<pair<rulesADT,bool> >::iterator r1 = junk_pr.begin(); 
	  r1 != junk_pr.end(); r1++){
	if(r1->second == true)
	  S->Rp = subtract_rules(S->Rp, create_rule_set(r1->first));
	else
	  S->R = subtract_rules(S->R, create_rule_set(r1->first));
	junk_rules.insert(*r1);
      }

    for(set<pair<rulesADT, bool> >::iterator iter2=junk_rules.begin(); 
	iter2!=junk_rules.end(); iter2++){
      for(map<string,rule_lst*>::iterator r1 = e_succ.begin();r1!=e_succ.end();
	  r1++)
	r1->second->remove(*iter2);
      
      for(map<string,rule_lst*>::iterator r2=e_pred.begin();r2!=e_pred.end();
	  r2++)
	r2->second->remove(*iter2);
    }

    // Use the newly created rules to update the predecessor and successor info
    // for each dummy node.
    for(set<pair<rulesADT, bool> >::iterator iter2=new_rules.begin(); 
	iter2!=new_rules.end(); iter2++){
      string dummy_en, dummy_ed;

      if(iter2->first->enabling->action->type&DUMMY)
	dummy_en=iter2->first->enabling->action->label;

      if(iter2->first->enabled->action->type&DUMMY)
	dummy_ed=iter2->first->enabled->action->label;
	
      if(dummy_en.size())
	e_succ[dummy_en]->push_back(*iter2);

      if(dummy_ed.size())
	e_pred[dummy_ed]->push_back(*iter2);
    }
    emitters(0, fn, S);
  }  
}
*/


/*****************************************************************************/
/* Emit a timed event-rule structure.                                        */
/*****************************************************************************/
void emitters(const char* path, const char* filename,TERstructADT S)
{
  char outname[256];
  TERstructADT tmp = NULL;

  if(S == 0)
    tmp = S = TERSempty();
  
  if(path){
    strcpy(outname,path);
    if (outname[0] != '\0') strcat(outname,"/");
    strcat(outname,filename);
  }
  else
    strcpy(outname,filename);

  if (toTEL)
    strcat(outname,".tel");
  else
    strcat(outname,".er");
  ofstream out(outname);
  if(!out){
    loggedStream errors(cerr,lg);
    //errors << "ERROR: Cannot open file '" << outname << "'.\n";
    problems++;
    return;
  }
  printf("Storing module %s to %s\n",filename,outname);
  emit_header(out,S);
  emit_events(out,S);
  emit_rules(out,S,FALSE);
  emit_cont_rules(out,S);
  emit_rules(out,S,TRUE);
  emit_mergers(out,S);
  emit_conflicts(out,S);
  // ZHEN(Done): add emit_assignments(out,S) function
  emit_assignments(out,S);
  TERSdelete(tmp);
}



void replace(TERstructADT S)
{
  actionADT A = S->first->event->action;
  eventsADT first_event = S->first->event;
  eventsetADT cur_event;
  for(cur_event = S->first; cur_event; cur_event = cur_event->link)
    {
      rulesetADT rs = S->R;
      while(rs)
	{  
	  if(strcmp(rs->rule->enabling->action->label, 
		    cur_event->event->action->label) == 0 &&
	     rs->rule->enabling->occurrence == cur_event->event->occurrence)
	    rs->rule->enabling->action = A;
	  
	  rs = rs->link;
	}

      rs = S->Rp;
      while(rs)
	{  
	  if(strcmp(rs->rule->enabling->action->label, 
		    cur_event->event->action->label) == 0 &&
	     rs->rule->enabling->occurrence == cur_event->event->occurrence)
	    rs->rule->enabling->action = A;
	  
	  rs = rs->link;
	}
    }

  eventsetADT et = create_event_set(first_event);
  delete_event_set(S->first);
  S->first = et;
}


int rule_cnt(telADT S)
{
  int total=0;
  rulesetADT cur_rule;
  
  for(cur_rule=S->R; cur_rule; cur_rule=cur_rule->link)
    total++;

  for(cur_rule=S->Rp; cur_rule; cur_rule=cur_rule->link)
    total++;
  
  return total;
}


int event_cnt(telADT S)
{
  int total =0;
  eventsetADT cur_event;

  for(cur_event = S->I; cur_event; cur_event = cur_event->link)
    total++;

  for(cur_event = S->O; cur_event; cur_event = cur_event->link)
    total++;

  return total;
}

/*****************************************************************************/
/* Load merger lists.                                                        */
/*****************************************************************************/

bool load_TERS_mergers(FILE *fp,int nmergers,int *linenum)
{
  int i,token;
  char tokvalue[MAXTOKEN];

  for (i=0;i<nmergers;i++) {
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != WORD)
      if (token==END_OF_FILE) {
	printf("ERROR:  Unexpected end of file!\n");
	fprintf(lg,"ERROR:  Unexpected end of file!\n");
	return(FALSE); 
      }
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != END_OF_LINE) {
      if (token==END_OF_FILE) {
	printf("ERROR:  Unexpected end of file!\n");
	fprintf(lg,"ERROR:  Unexpected end of file!\n");
	return(FALSE);
      }
    }
  }
  return(TRUE);
}

eventsADT find_TERS_event(char *e,eventsetADT I,eventsetADT O) 
{
  eventsetADT cur_event;
  int occur;
  char * slash;

  if (strchr(e,'/')) {
    slash=strchr(e,'/');
    occur = atoi(slash+1);
    (*slash) = '\0';
  } else {
    occur = 1;
  }
  for(cur_event = I; cur_event; cur_event = cur_event->link)  
    if ((strcmp(cur_event->event->action->label,e)==0) &&
	(cur_event->event->occurrence==occur))
    return cur_event->event;
  for(cur_event = O; cur_event; cur_event = cur_event->link)  
    if ((strcmp(cur_event->event->action->label,e)==0) &&
	(cur_event->event->occurrence==occur))
    return cur_event->event;
  return NULL;
}

/*****************************************************************************/
/* Load conflict list.                                                       */
/*****************************************************************************/

bool load_TERS_conflicts(FILE *fp,TERstructADT S,int *nconf,int *linenum,
			 int conftype)
{
  int i,token,count;
  char tokvalue[MAXTOKEN];
  bool warning;
  eventsADT e,f;

  count=(*nconf);
  warning=FALSE;
  for (i=0;i<count;i++) {
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != WORD)
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE); 
      }
    if ((e=find_TERS_event(tokvalue,S->I,S->O))==NULL) return(FALSE);
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != END_OF_LINE) {
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE);
      }
      if (token==WORD) {
	if ((f=find_TERS_event(tokvalue,S->I,S->O))==NULL) return(FALSE);
	S->C=add_conflict(S->C, e, f, conftype);
      }
    }
  }
  return(TRUE);
}

bool S2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN]);

bool R2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN]);

bool U2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN])
{
  switch (*token) {
  case WORD:
    strcat(expr,tokvalue);

    if (strcmp(tokvalue,"false")==0) {
      (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      return TRUE;
    }
    if (strcmp(tokvalue,"true")==0) {
      (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      return TRUE;
    }
    if (strncmp(tokvalue,"maybe",5)==0) {
      (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      return TRUE;
    }
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    break;
  case '(':
    strcat(expr,"(");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!R2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    if ((*token) != ')') {
      printf("ERROR:%d:  Expected a )!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expected a )!\n",*linenum);
    }
    strcat(expr,")");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    break;
  default:
    printf("ERROR:%d:  Expected a signal or a (!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal or a (!\n",*linenum);
    return FALSE;
  }
  return TRUE;
}

bool T2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN])
{
  switch (*token) {
  case WORD:
  case '(':
    if (!U2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    break;
  case '~':
    strcat(expr,"~");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!U2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return FALSE;
  }
  return TRUE;
}

bool C2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN])
{
  switch (*token) {
  case '&':
    strcat(expr,"&");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!T2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    if (!C2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    break;
  case '|':
  case ')':
  case ']':
    break;
  default:
    printf("ERROR:%d:  Expected a |, &, ), or a ]!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a |, &, ), or a ]!\n",*linenum);
    return FALSE;
  }
  return TRUE;
}

bool B2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN])
{
  switch (*token) {
  case '|':
    strcat(expr,"|");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!S2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    if (!B2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    break;
  case ')':
  case ']':
    break;
  default:
    printf("ERROR:%d:  Expected a |, ), or a ]!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a |, ), or a ]!\n",*linenum);
    return FALSE;
  }
  return TRUE;
}

bool S2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN])
{
  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!T2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    if (!C2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return FALSE;
  }
  return TRUE;
}

bool R2(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
	char expr[MAXTOKEN])
{
  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!S2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    if (!B2(token,fp,tokvalue,linenum,expr))
      return FALSE;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return FALSE;
  }
  return TRUE;
}

/*****************************************************************************/
/* Load rules.                                                               */
/*****************************************************************************/

bool load_TERS_rules(FILE *fp,TERstructADT S,int nevents,int *nrules,
		     int *number,int ruletype,int *linenum,bool *level)
{
  int i,count,eps,lower,upper,token;
  char tokvalue[MAXTOKEN];
  char dist[MAXTOKEN];
  char expr[MAXTOKEN];
  double x,y;
  bool disabling,assumption;
  eventsADT e,f;
  eventsADT reset;

  reset = event(action("reset", 5), 0, 0, 0, NULL);
  count=(*number);
  token=WORD;
  for (i=0;i<count;i++) {

    /* Get enabling event */

    while ((token != END_OF_LINE) && (token != COMMENT)) {
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE); 
      } 
      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    } 
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    e=find_TERS_event(tokvalue,S->I,S->O);

    /* Get enabled event */

    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    f=find_TERS_event(tokvalue,S->I,S->O);

    /* Get boolean expression */

    expr[0]='\0';
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    disabling=FALSE;
    if (token=='[') {
      (*level)=TRUE;
      strcat(expr,"[");
      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      if (!R2(&token,fp,tokvalue,linenum,expr)) return FALSE;
      if (token!=']') {
	printf("ERROR:%d:  Expecting a ']'!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Expecting a ']'!\n",*linenum);
	return(FALSE);
      }
      strcat(expr,"]");
      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      if (strcmp(tokvalue,"d")==0) {
	strcat(expr,"d");
	disabling=TRUE;
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      }
    } 

    /* Get epsilon value */
    
    eps=atoi(tokvalue);

    /* Get lower bound */
    assumption=FALSE;
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    if ((strcmp(tokvalue,"inf")==0) || (strcmp(tokvalue,"infinity")==0)) {
      printf("ERROR:%d:  A lower bound of infinity is not allowed!\n",
	     *linenum);
      fprintf(lg,"ERROR:%d:  A lower bound of infinity is not allowed!\n",
	      *linenum);
      return(FALSE);
    } else if (strcmp(tokvalue,"a")==0) {
      assumption=TRUE;
      lower=0;
    } else lower=atoi(tokvalue);
    if (lower < 0) {
      printf("ERROR:%d:  Negative timing constraints not allowed!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Negative timing constraints not allowed!\n",
	      *linenum);
      return(FALSE);
    }
    if (lower > INFIN) {
      printf("ERROR:%d:  All timing constraints must be less than %d!\n",
	     *linenum,INFIN);
      fprintf(lg,"ERROR:%d:  All timing constraints must be less than %d!\n",
	      *linenum,INFIN);
      return(FALSE);
    }

    /* Get upper bound */ 

    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    if ((strcmp(tokvalue,"inf")==0) || (strcmp(tokvalue,"infinity")==0)) {
      upper=INFIN;
    } else if (strcmp(tokvalue,"a")==0) {
      assumption=TRUE;
      upper=0;
    } else upper=atoi(tokvalue);
    if (upper < 0) {
      printf("ERROR:%d:  Negative timing constraints are not allowed!\n",
	     *linenum);
      fprintf(lg,"ERROR:%d:  Negative timing constraints are not allowed!\n",
	      *linenum);
      return(FALSE);
    }
    if (upper > INFIN) {
      printf("ERROR:%d:  All timing constraints must be less than %d!\n",
	     *linenum,INFIN);
      fprintf(lg,"ERROR:%d:  All timing constraints must be less than %d!\n",
	      *linenum,INFIN);
      return(FALSE);
    }
    if (upper < lower) {
      printf("ERROR:%d:  Lower bound larger than upper bound!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Lower bound larger than upper bound!\n",*linenum);
      return(FALSE);
    }

    /* Get probability distribution */

    token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (token==WORD) {
      if (strcmp(tokvalue,"N")==0) {
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!='(') {
	  printf("ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  return(FALSE);
	}
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	x=atof(tokvalue);
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!=',') {
	  printf("ERROR:%d:  Expected a \',\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \',\'!\n",*linenum);
	  return(FALSE);
	}
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	y=atof(tokvalue);
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!=')') {
	  printf("ERROR:%d:  Expected a \')\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \')\'!\n",*linenum);
	  return(FALSE);
	}
	sprintf(dist,"N(%f,%f)",x,y);
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      } else if (strcmp(tokvalue,"U")==0) {
	sprintf(dist,"U");
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      } else if (strcmp(tokvalue,"S")==0) {
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!='(') {
	  printf("ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  return(FALSE);
	}
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	x=atof(tokvalue);
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!=')') {
	  printf("ERROR:%d:  Expected a \')\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \')\'!\n",*linenum);
	  return(FALSE);
	}
	sprintf(dist,"S(%f)",x);
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      }
    } else {
      if ( lower == upper )
	strcpy(dist,"");
      else
	strcpy(dist,"");
    }

    /* Store rule */
    if (eps==1) {
      if (e != NULL && f != NULL) {
	if (expr[0]=='\0') {
	  S->Rp = add_rule(S->Rp, e, f, lower, upper, ruletype, dist);
	  S->R = add_rule(S->R, reset, f, lower, upper, ruletype, dist);
	} else {
	  S->Rp = add_rule(S->Rp, e, f, lower, upper, ruletype, dist, expr);
	  S->R = add_rule(S->R, reset, f, lower, upper, ruletype, dist, expr);
	}
      }
    } else {
      if (e != NULL && f != NULL)
	if (expr[0]=='\0') {
	  S->R = add_rule(S->R, e, f, lower, upper, ruletype, dist);
	} else {
	  S->R = add_rule(S->R, e, f, lower, upper, ruletype, dist, expr);
	}
    }
  }    
  return(TRUE);
}

/*****************************************************************************/
/* Load event list.                                                          */
/*****************************************************************************/

bool load_TERS_events(FILE *fp,int nevents,char * first,int ninputs,
		      int *linenum,TERstructADT S,char *startstate)
{
  int i,len,token,occur,sig;
  char e[MAXTOKEN];
  char * slash;
  actionADT A,Ai;

  strcpy(e,first);
  sig=0;
  for (i=1;i<nevents;i++) {
    while ((token=fgettok(fp,e,MAXTOKEN,linenum)) != WORD)
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE);
      }
    if (strchr(e,'/')) {
      slash=strchr(e,'/');
      occur = atoi(slash+1);
      (*slash) = '\0';
      len=strlen(e);
      A=action(e,len);
    } else {
      len=strlen(e);
      A=action(e,len);
      occur = 1;
    }
    S->A = union_actions(S->A,create_action_set(A));

    Ai = NULL;
    if (e[0] != '$') {
      i++;
      while ((token=fgettok(fp,e,MAXTOKEN,linenum)) != WORD)
	if (token==END_OF_FILE) {
	  printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	  return(FALSE);
	}
      if (strchr(e,'/')) {
	slash=strchr(e,'/');
	occur = atoi(slash+1);
	(*slash) = '\0';
	len=strlen(e);
	Ai=action(e,len);
      } else {
	len=strlen(e);
	Ai=action(e,len);
	occur = 1;
      }
      S->A = union_actions(S->A,create_action_set(Ai));
    }
    A->maxoccur = occur;
    A->inverse = Ai;
    A->initial = FALSE; 
    if ((e[0] != '$') && (occur == 1) && ((unsigned)sig < strlen(startstate))){
      if (startstate[sig] == '1') A->initial = TRUE;
      sig++;
    }
    if (Ai) {
      Ai->maxoccur = occur;
      Ai->inverse = A;
      Ai->initial = FALSE;
    }
    if (e[0] == '$') {
      A->type = DUMMY;
      S->O=union_events(S->O,create_event_set(event(A,occur,0,INFIN,NULL)));
    } else if (i <= ninputs) {
      A->type = IN;
      S->I=union_events(S->I,create_event_set(event(A,occur,0,INFIN,NULL)));
      Ai->type = IN;
      S->I=union_events(S->I,create_event_set(event(Ai,occur,0,INFIN,
						    NULL)));
    } else {
      A->type = OUT;
      S->O=union_events(S->O,create_event_set(event(A,occur,0,INFIN,NULL)));
      Ai->type = OUT;
      S->O=union_events(S->O,create_event_set(event(Ai,occur,0,INFIN,
						    NULL)));
    }
  } 
  return(TRUE);
}

/*****************************************************************************/
/* Load a timed event-rule structure from a file to a TERSstruct.            */
/*****************************************************************************/

/* MEMORY LEAKS IN THIS CODE */

TERstructADT load_ters(char * filename)
{
  char inname[FILENAMESIZE];
  char *event;
  char *startstate;
  char *initval;
  char *initrate;
  int nevents,ninputs,ndisj;
  int ncausal,nconf,niconf,noconf,nord,nrules;
  FILE *fp;
  bool level;
  int linenum;
  TERstructADT S;
  
  strcpy(inname,filename);
  if (1)
    strcat(inname,".er");
  else
    strcat(inname,".tel");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE); 
  }
  if (1) {
    printf("Loading timed event-rule structure from:  %s\n",inname);
    fprintf(lg,"Loading timed event-rule structure from:  %s\n",inname);
  } else {
    printf("Loading timed event/level structure from:  %s\n",inname);
    fprintf(lg,"Loading timed event/level structure from:  %s\n",inname);
  }
  linenum=1;
  S = TERSempty();
  event=load_header(fp,&nevents,&ninputs,&ncausal,&ndisj,&nconf,&niconf,
		    &noconf,&nord,&startstate,&initval,&initrate,&linenum);
  if (event==NULL) {
    free(event);
    fclose(fp);
    return S;
  }
  if (!load_TERS_events(fp,nevents,event,ninputs,&linenum,S,startstate)) {
    free(event);
    fclose(fp);
    return S;
  }
  if (!load_TERS_rules(fp,S,nevents,&nrules,&ncausal,TRIGGER,&linenum,
		       &level)) {
    free(event);
    fclose(fp);
    return S;
  }
  if (!load_TERS_rules(fp,S,nevents,&nrules,&nord,ORDERING,&linenum,
		       &level)) {
    free(event);
    fclose(fp);
    return S;
  }
  if (!load_TERS_mergers(fp,ndisj,&linenum)) {
    free(event);
    fclose(fp);
    return S;
  }
  /* CONFLICTS: NEED NICONF AND NOCONF HERE! */
  if (!load_TERS_conflicts(fp,S,&niconf,&linenum,ICONFLICT)) {
    free(event);
    fclose(fp);
    return S;
  }
  if (!load_TERS_conflicts(fp,S,&noconf,&linenum,OCONFLICT)) {
    free(event);
    fclose(fp);
    return S;
  }
  if (!load_TERS_conflicts(fp,S,&nconf,&linenum,(ICONFLICT|OCONFLICT))) {
    free(event);
    fclose(fp);
    return S;
  }
  free(event);
  fclose(fp);
  return S;
}

void postprocess(char * filename,bool postproc,bool redchk,bool fromG,
		 bool verbose)
{
  TERstructADT S;
  
  if (postproc || redchk) {
    initialize_parser(filename, true);
    S = load_ters(filename);
    if ((S) && (postproc)) {
      if (redchk) rm_red_top(S, verbose, 0, false);
      post_proc(S, redchk, verbose, fromG, false);
    }
    if ((S) && (redchk)) rm_red_top(S, verbose, 0, true);
    if (S)
      emitters(NULL,filename,S);
  //emitters(outpath, dir_file.second.c_str(), S);
    //TERSdelete(S);
  }
}

//print the declaration of event (A,i) followed by <delim>:
void declare_event(ostream&out, actionADT A, int i, char delim){
  out << A->label << '/'<< i << ':' << A->type << delim;
}

//print the declarations of (A,i) and (A->inverse,i) on one line:
void declare_pair(ostream&out, actionADT A, int i){
  declare_event(out, A, i, ' ');
  if(A->inverse){
    declare_event(out, A->inverse, i, '\n');
  } else {
    char *temp = CopyString(A->label);
    temp[strlen(A->label)-1] = '-';
    out << temp << '/'<< i << ':' << A->type << endl;
    free(temp);
  }/*
  else{
    loggedStream errors(cerr,lg);
    errors << __FILE__ << ':' << __LINE__ << ':'
	   << A->label << " has no inverse!" << endl;
    problems++;
    }*/
}

ostream& operator<<(ostream&out, eventsADT E){//Output an event
  out << E->action->label;
  if(strcmp(E->action->label,"reset")){
    out << '/' << E->occurrence;
    if(!E->data.empty()){
      out << ':' << E->data;
    }
  }
  return out;
}

// The following function emits either the input or the output events:
void emit_some_events(ostream&out,TERstructADT S, int type, char * prefix,
		      bool dummy){
  out << "#"                             << endl
      << "# " << prefix << "PUT EVENTS:" << endl
      << "#"                             << endl;
  for (actionsetADT cur_action(S->A);cur_action;cur_action=cur_action->link){
    actionADT act(cur_action->action);
    if ((act->type & type) && 
	((dummy && (act->type & DUMMY))||
	 (!dummy && !(act->type & DUMMY)))) {
      int max(Max(act));
      if (act->label[strlen(act->label)-1]=='+'){
	for (int i(1); i<=max; i++){
	  if(!(act->type==DUMMY)){
	    declare_pair(out, act, i);
	  }
	}
      }
      else if(act->inverse && act->inverse->maxoccur==0){
	for (int i(1); i<=max; i++){
	  declare_pair(out, act->inverse, i);
	}
      }
    }
  }
}

//Return the maximum occurrence of act and act->inverse (if act has an inverse)
int Max(actionADT act){
  int max(act->maxoccur);
  if((!(act->type==DUMMY)) && (act->inverse && act->inverse->maxoccur > max)){
    max = act->inverse->maxoccur;
  }
  return max;
}
