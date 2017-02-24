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
/* memaloc.c                                                                 */
/*****************************************************************************/

#include <stdio.h>
#include <cmath>
#include "struct.h"
#include "def.h"
#include "connect.h"
#include "merges.h"
#include "memaloc.h"
#include "common_timing.h"
#include "cudd_synth.h"
#include "markov_types.h"
#include "displayADT.h"

///////////////////////////////////////////////////////////////////////////////
// This function will initialize all member variables of the state_list_tag
// to be NULL or zero. 
///////////////////////////////////////////////////////////////////////////////
void init( statelistADT s ) {
  s->stateptr = NULL;
  s->enabling = -1;
  s->enabled = -1;
  s->iteration = 0;
  s->next = NULL;
  s->probability = 0;
}

/*****************************************************************************/
/* Free space used by old set of signals and                                 */
/*   create space for new set of signals.                                    */
/*****************************************************************************/

signalADT *new_signals(int status,bool error,int oldnsignals,int nsignals,
		       signalADT *oldsignals)
{
  signalADT *signals=NULL;
  int a;

  if (((status & LOADED) || (error==TRUE)) && (oldsignals != NULL)) {
    for (a=0;a<oldnsignals;a++)
      if (oldsignals[a]->name != NULL) free(oldsignals[a]->name);
    for (a=0;a<oldnsignals;a++)
      free(oldsignals[a]);
    free(oldsignals);
  }
  if (error==FALSE) {
    signals=(signalADT*)GetBlock((nsignals+1) * sizeof(signals[0]));
    for (a=0;a<nsignals;a++)
      signals[a]=(signalADT)GetBlock(sizeof(*signals[0]));
    for (a=0;a<nsignals;a++) {
      signals[a]->name=NULL;
      signals[a]->is_level=FALSE;
      signals[a]->event=0;
      signals[a]->riselower=0;
      signals[a]->riseupper=0;
      signals[a]->falllower=0;
      signals[a]->fallupper=0;
      signals[a]->maxoccurrence=0;
    }
    return(signals);
  }
  return(NULL);
}

/*****************************************************************************/
/* Free space used by old set of integers and                                 */
/*   create space for new set of integers.                                    */
/*****************************************************************************/

variableADT *new_variables(int status,bool error,int oldnvariables,
			   int nvariables, variableADT *oldvariables)
{
  variableADT *variables=NULL;
  int a;

  if (((status & LOADED) || (error==TRUE)) && (oldvariables != NULL)) {
    for (a=0;a<oldnvariables;a++)
      if (oldvariables[a]->name != NULL) free(oldvariables[a]->name);
    for (a=0;a<oldnvariables;a++)
      free(oldvariables[a]);
    free(oldvariables);
  }
  if (error==FALSE) {
    variables=(variableADT*)GetBlock((nvariables+1) * sizeof(variables[0]));
    for (a=0;a<nvariables;a++)
      variables[a]=(variableADT)GetBlock(sizeof(*variables[0]));
    for (a=0;a<nvariables;a++) {
      variables[a]->name=NULL;
      variables[a]->lInitVal=-INFIN;
      variables[a]->uInitVal=INFIN;
    }
    return(variables);
  }
  return(NULL);
}

/*****************************************************************************/
/* Free space used by old set of events and                                  */
/*   create space for new set of events.                                     */
/*****************************************************************************/

eventADT *new_events(int status,bool error,int oldnevents,int nevents,
                     eventADT *oldevents)
{
  eventADT *events=NULL;
  int a;

  if (((status & LOADED) || (error==TRUE)) && (oldevents != NULL)) {
    for (a=0;a<oldnevents;a++) {
      if (oldevents[a]->event != NULL) free(oldevents[a]->event);
/*       if(oldevents[a]->data != NULL) { */
/*         free(oldevents[a]->data); */
/*       } */
      //SRL: These are copies...why is this a problem?
/*       if(oldevents[a]->hsl != NULL) { */
/*         free(oldevents[a]->hsl); */
/*       } */
/*       if(oldevents[a]->SOP != NULL) { */
/*         level_exp curLevel, nextLevel; */
/*         curLevel = oldevents[a]->SOP; */
/*         while(curLevel) { */
/*           nextLevel = curLevel->next; */
/*           if(curLevel->product != NULL) { */
/*             free(curLevel->product); */
/*           } */
/*           free(curLevel); */
/*           curLevel = nextLevel; */
/*         } */
/*       } */
      //SRL: Ineqs aren't copies?
/*       if(oldevents[a]->inequalities != NULL) { */
/*         ineqADT curIneq, nextIneq; */
/*         curIneq = oldevents[a]->inequalities; */
/*         while(curIneq) { */
/*           nextIneq = curIneq->next; */
/*           free(nextIneq); */
/*           curIneq = nextIneq; */
/*         } */
/*       } */
    }
    for (a=0;a<oldnevents;a++)
      free(oldevents[a]);
    free(oldevents);
  }
  if (error==FALSE) {
    events=(eventADT*)GetBlock(nevents * sizeof(events[0]));
    for (a=0;a<nevents;a++)
      events[a]=(eventADT)GetBlock(sizeof(*events[0]));
    for (a=0;a<nevents;a++) {
      events[a]->event=NULL;
      events[a]->hsl=NULL;
      events[a]->SOP=NULL;
      events[a]->dropped=FALSE;
      events[a]->immediate=FALSE;
      events[a]->signal=(-1);
      events[a]->color=0;
      events[a]->lower=0;
      events[a]->upper=INFIN;
      events[a]->lrate=0;
      events[a]->urate=0;
      events[a]->lrange=0;
      events[a]->urange=0;
      events[a]->linitrate=0;
      events[a]->uinitrate=0;
      events[a]->failtrans=FALSE;
      events[a]->rate=1;
      events[a]->process=0;
      events[a]->type=UNKNOWN;
      events[a]->data="";
      events[a]->inequalities=NULL;
      events[a]->transRate=NULL;
      events[a]->delayExpr=NULL;
      events[a]->delayExprTree=NULL;
      events[a]->priorityExpr=NULL;
      events[a]->nondisabling=FALSE;
    }
    return(events);
  }
  return(NULL);
}

/*****************************************************************************/
/* Copy an eventADT                                                          */
/*****************************************************************************/
eventADT copy_event(eventADT event)
{
  eventADT newEvent = (eventADT)GetBlock(sizeof(*event));
  if(event->event != NULL) {
    newEvent->event = (char*)GetBlock(strlen(event->event)+1 * sizeof(char));
    strncpy(newEvent->event,event->event,strlen(event->event)+1);
  }

  newEvent->dropped = event->dropped;
  newEvent->immediate = event->immediate;
  newEvent->color = event->color;
  newEvent->signal = event->signal;
  newEvent->lower = event->lower;
  newEvent->upper = event->upper;
  newEvent->process = event->process;
  newEvent->type = event->type;

  if(event->data != NULL) {
    newEvent->data = (char*)GetBlock(strlen(event->data)+1 * sizeof(char));
    strncpy(newEvent->data,event->data,strlen(event->data)+1);
  }

  if(event->hsl != NULL) {
    newEvent->hsl = (char*)GetBlock(strlen(event->hsl)+1 * sizeof(char));
    strncpy(newEvent->hsl,event->hsl,strlen(event->hsl)+1);
  }
  newEvent->EXP = NULL;

  newEvent->delayExpr = CopyString(event->delayExpr);
  newEvent->transRate = CopyString(event->transRate);
  newEvent->priorityExpr = CopyString(event->priorityExpr);
  newEvent->delayExprTree = NULL;
  newEvent->nondisabling = event->nondisabling;

  newEvent->rate = event->rate;
  newEvent->lrate = event->lrate;
  newEvent->urate = event->urate;
  newEvent->lrange = event->lrange;
  newEvent->urange = event->urange;
  newEvent->linitrate = event->linitrate;
  newEvent->uinitrate = event->uinitrate;
  newEvent->failtrans = event->failtrans;

  if(event->SOP != NULL) {
    level_exp curLevel = event->SOP;
    level_exp newCurLevel = newEvent->SOP;
    level_exp newPrevLevel = newEvent->SOP;
    newCurLevel = (level_exp) GetBlock(sizeof(bool_exp));
    newEvent->SOP = newCurLevel;
    newCurLevel->product = (char*) GetBlock(strlen(curLevel->product)+1 *
                                            sizeof(char));
    strncpy(newCurLevel->product,curLevel->product,
            strlen(curLevel->product)+1);
    newCurLevel->next = NULL;
    newPrevLevel = newCurLevel;
    curLevel = curLevel->next;
    while(curLevel != NULL) {
      newCurLevel = (level_exp) GetBlock(sizeof(bool_exp));
      newCurLevel->product = (char*) GetBlock(strlen(curLevel->product)+1 *
                                              sizeof(char));
      strncpy(newCurLevel->product,curLevel->product,
              strlen(curLevel->product)+1);
      newPrevLevel->next = newCurLevel;
      newPrevLevel = newCurLevel;
      curLevel = curLevel->next;
    }
    newPrevLevel->next = NULL;
  }
  else {
    newEvent->SOP = NULL;
  }
  if(event->inequalities != NULL) {
    ineqADT curIneq = event->inequalities;
    ineqADT newCurIneq = newEvent->inequalities;
    ineqADT newPrevIneq = newEvent->inequalities;
    newCurIneq = (ineqADT) GetBlock(sizeof(ineq_tag));
    newCurIneq->place = curIneq->place;
    newCurIneq->type = curIneq->type;
    newCurIneq->uconstant = curIneq->uconstant;
    newCurIneq->constant = curIneq->constant;
    newCurIneq->expr = CopyString(curIneq->expr);
    newCurIneq->tree = NULL;
    newCurIneq->signal = curIneq->signal;
    newCurIneq->transition = curIneq->transition;
    newCurIneq->next = NULL;
    newPrevIneq = newCurIneq;
    curIneq = curIneq->next;
    newEvent->inequalities = newCurIneq;
    while(curIneq != NULL) {
      newCurIneq = (ineqADT) GetBlock(sizeof(ineq_tag));
      newCurIneq->place = curIneq->place;
      newCurIneq->type = curIneq->type;
      newCurIneq->uconstant = curIneq->uconstant;
      newCurIneq->constant = curIneq->constant;
      newCurIneq->expr = CopyString(curIneq->expr);
      newCurIneq->tree = NULL;
      newCurIneq->signal = curIneq->signal;
      newCurIneq->transition = curIneq->transition;
      newPrevIneq->next = newCurIneq;
      newPrevIneq = newCurIneq;
      curIneq = curIneq->next;
    }
    newPrevIneq->next = NULL;
  }
  else {
    newEvent->inequalities = NULL;
  }
  return newEvent;
}

/*****************************************************************************/
/* Free space used by old set of rules and                                   */
/*   create space for new set of rules.                                      */
/*****************************************************************************/

ruleADT **new_rules(int status,bool error,int oldnevents,int nevents,
                   ruleADT **oldrules)
{
  ruleADT **rules=NULL;
  int a,b;
  level_exp curlevel,nextlevel;
  ineqADT curineq,nextineq;

  if (((status & LOADED) || (error==TRUE)) && (oldrules != NULL)) {
    for (a=0;a<oldnevents;a++) {
      for (b=0;b<oldnevents;b++) {
        curlevel=oldrules[a][b]->level;
        while(curlevel && curlevel->product) {
          nextlevel=curlevel->next;
          free(curlevel->product);
          free(curlevel);
          curlevel=nextlevel;
        }
        curlevel=oldrules[a][b]->POS;
        while(curlevel && curlevel->product) {
          nextlevel=curlevel->next;
          free(curlevel->product);
          free(curlevel);
          curlevel=nextlevel;
        }
        curineq=oldrules[a][b]->inequalities;
        while(curineq) {
          nextineq=curineq->next;
          free(curineq);
          curineq=nextineq;
        }
        if (oldrules[a][b]->expr) free(oldrules[a][b]->expr);
        if (oldrules[a][b]->dist) delete oldrules[a][b]->dist;
        free(oldrules[a][b]);
      }
    }
    for (a=0;a<oldnevents;a++) {
      free(oldrules[a]);
    }
    free(oldrules);
  }
  if (error==FALSE) {
    rules=(ruleADT**)GetBlock(nevents * sizeof(rules[0]));
    for (a=0;a<nevents;a++)
      rules[a]=(ruleADT*)GetBlock(nevents * sizeof(rules[0][0]));
    for (a=0;a<nevents;a++)
      for (b=0;b<nevents;b++) {
        rules[a][b]=(ruleADT)GetBlock(sizeof(*rules[0][0]));
        rules[a][b]->concur=false;
        rules[a][b]->markedConcur=false;
        rules[a][b]->epsilon=(-1);
        rules[a][b]->lower=0;
        rules[a][b]->upper=0;
        rules[a][b]->plower=0;
        rules[a][b]->pupper=0;
        rules[a][b]->weight=0;
        rules[a][b]->ruletype=NORULE;
        rules[a][b]->marking=0;
        rules[a][b]->maximum_sep=0;
        rules[a][b]->reachable=false;
        rules[a][b]->conflict=NOCONFLICT;
        rules[a][b]->level=NULL;
        rules[a][b]->POS=NULL;
        rules[a][b]->expr=NULL;
        rules[a][b]->dist=NULL;
        rules[a][b]->rate=0.0;
        rules[a][b]->inequalities=NULL;
      }
    return(rules);
  }
  return(NULL);
}

/*****************************************************************************/
/* Copy an ruleADT                                                           */
/*****************************************************************************/
ruleADT copy_rule(ruleADT rule)
{
  ruleADT newRule = (ruleADT)GetBlock(sizeof(rule_struct));
  newRule->concur = rule->concur;
  newRule->markedConcur = rule->markedConcur;
  newRule->epsilon = rule->epsilon;
  newRule->lower = rule->lower;
  newRule->upper = rule->upper;
  newRule->weight = rule->weight;
  newRule->plower = rule->plower;
  newRule->pupper = rule->pupper;
  newRule->predtype = rule->predtype;
  newRule->ruletype = rule->ruletype;
  newRule->marking = rule->marking;
  newRule->maximum_sep = rule->maximum_sep;
  newRule->reachable = rule->reachable;
  newRule->conflict = rule->conflict;

  /*TODO: Create a copy level_exp function */
  if(rule->level != NULL) {
    level_exp curLevel = rule->level;
    level_exp newCurLevel = newRule->level;
    level_exp newPrevLevel = newRule->level;
    newCurLevel = (level_exp) GetBlock(sizeof(bool_exp));
    newCurLevel->product = (char*) GetBlock(strlen(curLevel->product)+1 *
                                            sizeof(char));
    strncpy(newCurLevel->product,curLevel->product,
            strlen(curLevel->product)+1);
    newCurLevel->next = NULL;
    newPrevLevel = newCurLevel;
    curLevel = curLevel->next;
    while(curLevel != NULL) {
      newCurLevel = (level_exp) GetBlock(sizeof(bool_exp));
      newCurLevel->product = (char*) GetBlock(strlen(curLevel->product)+1 *
                                              sizeof(char));
      strncpy(newCurLevel->product,curLevel->product,
              strlen(curLevel->product)+1);
      newPrevLevel->next = newCurLevel;
      newPrevLevel = newCurLevel;
      curLevel = curLevel->next;
    }
    newPrevLevel->next = NULL;
  }
  else {
    newRule->level = NULL;
  }
  
  if(rule->POS != NULL) {
    level_exp curLevel = rule->POS;
    level_exp newCurLevel = newRule->POS;
    level_exp newPrevLevel = newRule->POS;
    newCurLevel = (level_exp) GetBlock(sizeof(bool_exp));
    newCurLevel->product = (char*) GetBlock(strlen(curLevel->product)+1 *
                                            sizeof(char));
    strncpy(newCurLevel->product,curLevel->product,
            strlen(curLevel->product)+1);
    newCurLevel->next = NULL;
    newPrevLevel = newCurLevel;
    curLevel = curLevel->next;
    while(curLevel != NULL) {
      newCurLevel = (level_exp) GetBlock(sizeof(bool_exp));
      newCurLevel->product = (char*) GetBlock(strlen(curLevel->product)+1 *
                                              sizeof(char));
      strncpy(newCurLevel->product,curLevel->product,
              strlen(curLevel->product)+1);
      newPrevLevel->next = newCurLevel;
      newPrevLevel = newCurLevel;
      curLevel = curLevel->next;
    }
    newPrevLevel->next = NULL;
  }
  else {
    newRule->POS = NULL;
  }

  if(rule->expr != NULL) {
    newRule->expr = (char*)GetBlock(strlen(rule->expr)+1 * sizeof(char));
    strncpy(newRule->expr,rule->expr,strlen(rule->expr)+1);
  }

  newRule->dist = rule->dist;
  newRule->rate = rule->rate;

  if(rule->inequalities != NULL) {
    ineqADT curIneq = rule->inequalities;
    ineqADT newCurIneq = newRule->inequalities;
    ineqADT newPrevIneq = newRule->inequalities;
    newCurIneq = (ineqADT) GetBlock(sizeof(ineqADT));
    newCurIneq->place = curIneq->place;
    newCurIneq->type = curIneq->type;
    newCurIneq->uconstant = curIneq->uconstant;
    newCurIneq->constant = curIneq->constant;
    newCurIneq->expr = NULL;
    newCurIneq->signal = curIneq->signal;
    newCurIneq->transition = curIneq->transition;
    newCurIneq->next = NULL;
    newPrevIneq = newCurIneq;
    curIneq = curIneq->next;
    while(curIneq != NULL) {
      newCurIneq = (ineqADT) GetBlock(sizeof(ineqADT));
      newCurIneq->place = curIneq->place;
      newCurIneq->type = curIneq->type;
      newCurIneq->uconstant = curIneq->uconstant;
      newCurIneq->constant = curIneq->constant;
      newCurIneq->expr = NULL;
      newCurIneq->signal = curIneq->signal;
      newCurIneq->transition = curIneq->transition;
      newPrevIneq->next = newCurIneq;
      newPrevIneq = newCurIneq;
      curIneq = curIneq->next;
    }
    newPrevIneq->next = NULL;
  }
  else {
    newRule->inequalities = NULL;
  }
  return newRule;
}

/*****************************************************************************/
/* Remove added rules.                                                       */
/*****************************************************************************/

void reinit_rules(int nevents,ruleADT **rules,int *nrules,int *ncausal,
		  int *nord)
{
  int i,j;
   
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if (rules[i][j]->ruletype != NORULE) {
	if (rules[i][j]->ruletype & REDUNDANT)
	  rules[i][j]->ruletype=rules[i][j]->ruletype-REDUNDANT; 
	if (!(rules[i][j]->ruletype & SPECIFIED)) {
	  (*nrules)--;
	  if (rules[i][j]->ruletype & ORDERING) (*nord)--;
	  else if (rules[i][j]->ruletype < ORDERING) (*ncausal)++;
	  rules[i][j]->epsilon=(-1);
	  rules[i][j]->lower=0;
	  rules[i][j]->upper=INFIN;
	  rules[i][j]->ruletype=NORULE;
/*	  rules[i][j]->conflict=FALSE; */
	}
      }
}

/*****************************************************************************/
/* Free space used by old marking key and                                    */
/*   create space for new marking key.                                       */
/*****************************************************************************/

markkeyADT *new_markkey(int status,bool error,int oldnmarkings,int nmarkings,
		     markkeyADT *oldmarkkey)
{
  markkeyADT *markkey=NULL;
  int a;

  if (((status & LOADED) || (error==TRUE)) && (oldmarkkey != NULL)){
    for (a=0;a<oldnmarkings;a++)
      free(oldmarkkey[a]);
    //free(oldmarkkey);
  }
  if (error==FALSE) {
    markkey=(markkeyADT*)GetBlock(nmarkings * sizeof(markkey[0]));
    for (a=0;a<nmarkings;a++)
      markkey[a]=(markkeyADT)GetBlock(sizeof(*markkey[0]));
    for (a=0;a<nmarkings;a++) {
      markkey[a]->enabling=(-1);
      markkey[a]->enabled=(-1);
    }
    return(markkey);
  }
  return(NULL);
}

/*****************************************************************************/
/* Initialize cycle graph.                                                   */
/*****************************************************************************/

void rulecpy(int nevents,int ncycles,cycleADT ****cycles,ruleADT **rules,
	     bool initreach)
{    
  int a,b,c,d;

  if (!cycles) return;
  if (initreach) notreachable=(-1);
  for (a=0;a<nevents;a++)
    for (b=0;b<nevents;b++)
      for (c=0;c<ncycles;c++)
	for (d=0;d<ncycles;d++) {
	  if (initreach) cycles[a][b][c][d]->reachable=0;
	  cycles[a][b][c][d]->Ui=NaN;
	  cycles[a][b][c][d]->Uj=NaN;
	  if ((rules[a][b]->epsilon>=0) && (d==c+rules[a][b]->epsilon) && 
	      ((a!=0) || (c==0))) {
	    cycles[a][b][c][d]->rule=TRUE;
	  } else {
	    cycles[a][b][c][d]->rule=FALSE;
	  }
	}
}

/*****************************************************************************/
/* Free space used by old cycle graph and                                    */
/*   create space for new cycle graph.                                       */
/*****************************************************************************/

cycleADT ****new_cycles(int status,bool error,int oldncycles,int ncycles,
			int oldnevents,int nevents,ruleADT **rules,
			cycleADT ****oldcycles)
{
  cycleADT ****cycles=NULL;
  int a,b,c,d;

  if (((status & LOADED) || (error==TRUE)) && (oldcycles != NULL)) {
    for (a=0;a<oldnevents;a++)
      for (b=0;b<oldnevents;b++)
        for (c=0;c<oldncycles;c++)
          for (d=0;d<oldncycles;d++)
            free(oldcycles[a][b][c][d]);
    for (a=0;a<oldnevents;a++)
      for (b=0;b<oldnevents;b++)
        for (c=0;c<oldncycles;c++)
          free(oldcycles[a][b][c]);
    for (a=0;a<oldnevents;a++)
      for (b=0;b<oldnevents;b++)
        free(oldcycles[a][b]);
    for (a=0;a<oldnevents;a++)
      free(oldcycles[a]);
    free(oldcycles);
  }
  if (error==FALSE) {
    cycles=(cycleADT****)GetBlock(nevents * sizeof(cycles[0]));
    for (a=0;a<nevents;a++)
      cycles[a]=(cycleADT***)GetBlock(nevents * sizeof(cycles[0][0]));
    for (a=0;a<nevents;a++)
      for (b=0;b<nevents;b++)
        cycles[a][b]=(cycleADT**)GetBlock(ncycles * sizeof(cycles[0][0][0]));
    for (a=0;a<nevents;a++)
      for (b=0;b<nevents;b++)
        for (c=0;c<ncycles;c++)
          cycles[a][b][c]=(cycleADT*)
            GetBlock(ncycles * sizeof(cycles[0][0][0][0]));
    for (a=0;a<nevents;a++)
      for (b=0;b<nevents;b++)
        for (c=0;c<ncycles;c++)
          for (d=0;d<ncycles;d++)
            cycles[a][b][c][d]=(cycleADT)GetBlock(sizeof(*cycles[0][0][0][0]));
    rulecpy(nevents,ncycles,cycles,rules,TRUE);
    return(cycles);
  }
  return(NULL);
}

/*****************************************************************************/
/* Free space used by old state graph and initialize state table.            */
/*****************************************************************************/

void initialize_state_table(int status,bool error,stateADT *state_table)
{
  int p;
  stateADT curstate;
  stateADT nextstate;
  statelistADT cur_pred;
  statelistADT next_pred;
  clocklistADT clockptr;
  clocklistADT next_clockptr;

  if ((status & LOADED) || (error==TRUE)) {
    for (p=0;p<PRIME;p++) {
      curstate=state_table[p];
      while (curstate != NULL && curstate->state != NULL) {
	nextstate=curstate->link;
	if (curstate->state) free(curstate->state);
	if (curstate->marking) free(curstate->marking);
	if (curstate->enablings) free(curstate->enablings);
	clockptr=curstate->clocklist;
	while(clockptr!=NULL){
	  next_clockptr=clockptr->next;
	  free_region(clockptr->clockkey, clockptr->clocks, 
		      clockptr->clocksize);
	  if ( clockptr->exp_enabled != NULL )
	    free(clockptr->exp_enabled);
	  if ( clockptr != NULL )
	    free(clockptr);
	  clockptr=next_clockptr;
	}
 	cur_pred=curstate->pred;
	while (cur_pred) {
	  next_pred=cur_pred->next;
	  free(cur_pred);
	  cur_pred=next_pred;
	}
 	cur_pred=curstate->succ;
	while (cur_pred) {
	  next_pred=cur_pred->next;
	  free(cur_pred);
	  cur_pred=next_pred;
	}
	free(curstate);
	curstate=nextstate;
      }
      if (curstate) free(curstate);
      state_table[p]=NULL;
    }
  }
  if (error==FALSE) {
    for (p=0;p<PRIME;p++) {
      state_table[p]=(stateADT)GetBlock(sizeof (*(state_table[p])));
      state_table[p]->state=NULL;
      state_table[p]->marking=NULL;
      state_table[p]->enablings=NULL;
      state_table[p]->number=0;
      state_table[p]->pred=NULL;
      state_table[p]->succ=NULL;
      state_table[p]->link=NULL;
      state_table[p]->num_clocks=0;
      state_table[p]->clocklist=NULL;
      state_table[p]->highlight=0;
      state_table[p]->color=0;
      state_table[p]->probability=0;
    }
  }
}

void initialize_ver_state_table(ver_stateADT *state_table){

  int p;
  for (p=0;p<PRIME;p++) {
    state_table[p]=(ver_stateADT)GetBlock(sizeof (*(state_table[p])));
    state_table[p]->state=NULL;
    state_table[p]->marking=NULL;
    state_table[p]->enablings=NULL;
    state_table[p]->imp_state=NULL;
    state_table[p]->imp_marking=NULL;
    state_table[p]->imp_enablings=NULL;
    state_table[p]->next=NULL;
    state_table[p]->clocklist=NULL;
  }
}

/*****************************************************************************/
/* Mop up loose BDD nodes.                                                   */
/*****************************************************************************/
#ifdef DLONG
void bdd_nukit(bddADT bdd_state,bool synth){
  
  int i;
  
  bdd_clear_refs(bdd_state->bddm);
  bdd_unfree(bdd_state->bddm,bdd_state->S);
  bdd_unfree(bdd_state->bddm,bdd_state->N);
  for (i = 0;i<bdd_state->nsigs;i++){
    bdd_unfree(bdd_state->bddm,bdd_state->lits[i]);
    bdd_unfree(bdd_state->bddm,bdd_state->newlits[i]);
  }
  if (!synth){
    bdd_unfree(bdd_state->bddm,bdd_state->M);
    bdd_unfree(bdd_state->bddm,bdd_state->Reg);
    for (i = 0;i<=bdd_state->nrules;i++){
      bdd_unfree(bdd_state->bddm,bdd_state->R[i]);
      bdd_unfree(bdd_state->bddm,bdd_state->C[i]);
    }
    for (i = 0;i<bdd_state->maxlinks;i++){
      bdd_unfree(bdd_state->bddm,bdd_state->L[i]);
      bdd_unfree(bdd_state->bddm,bdd_state->SL[i]);
    }
    for (i = 0;i<bdd_state->nlog;i++){
      bdd_unfree(bdd_state->bddm,bdd_state->rows[i]);
      bdd_unfree(bdd_state->bddm,bdd_state->cols[i]);
    }
    for (i = 0;i<bdd_state->nrules;i++){
      bdd_unfree(bdd_state->bddm,bdd_state->bdd_mark[i]);
    }
    for (i = 0;i<bdd_state->linksize;i++){
      bdd_unfree(bdd_state->bddm,bdd_state->links[i]);
    }
  }
  bdd_gc(bdd_state->bddm);
}
#else
//CUDD
void bdd_nukit(bddADT bdd_state,bool synth){
}
#endif

/*****************************************************************************/
/* Free space used by old BDD state graph and initialize state table.        */
/*****************************************************************************/
#ifdef DLONG
void initialize_bdd_state_table(int status,bool error,bddADT bdd_state,
				signalADT *signals,int nsigs,int nrules)
{
  int i,j,flag,nlog,max;
  bdd s,t,u,v,w,x;
  //  block mark_vars,link_vars,rc_vars,region_vars;

  if (((status & LOADED) || (error==TRUE)) && (signals != NULL)) {
    /* destroy manager */
    bdd_quit(bdd_state->bddm);
    /* deallocate arrays */
    free(bdd_state->L);
    free(bdd_state->SL);
    free(bdd_state->R);
    free(bdd_state->C);
    free(bdd_state->lits);
    free(bdd_state->newlits);
    free(bdd_state->rows);
    free(bdd_state->cols) ;
    free(bdd_state->bdd_mark);
    free(bdd_state->links);

  } 
  if (error==FALSE) {
    /* create new manager */
    nlog = (int)ceil(log(nrules+1)/log(2));
    bdd_state->nlog = nlog;
    bdd_state->nsigs = nsigs;
    bdd_state->nrules = nrules;
  
    bdd_state->bddm = bdd_init();
    //bdd_node_limit(bdd_state->bddm,0);
    //    bdd_dynamic_reordering(bdd_state->bddm,bdd_reorder_stable_window3);
    //bdd_reorder( bdd_state->bddm);
    /* initialize main bdds */
    bdd_state->S = bdd_zero(bdd_state->bddm);
    bdd_state->M = bdd_zero(bdd_state->bddm);
    bdd_state->N = bdd_zero(bdd_state->bddm);
    bdd_state->Reg = bdd_zero(bdd_state->bddm);
    /* allocate arrays */    
    bdd_state->lits = (bdd*)GetBlock((nsigs+1)*sizeof(bdd));
    bdd_state->newlits = (bdd*)GetBlock((nsigs+1)*sizeof(bdd));
    bdd_state->rows = (bdd*)GetBlock((nlog+1)*sizeof(bdd));
    bdd_state->cols = (bdd*)GetBlock((nlog+1)*sizeof(bdd));
    bdd_state->links = (bdd*)GetBlock((bdd_state->linksize+1)*sizeof(bdd));
    bdd_state->L = (bdd*)GetBlock((bdd_state->maxlinks)*sizeof(bdd));
    bdd_state->SL= (bdd*)GetBlock((bdd_state->maxlinks)*sizeof(bdd));
    bdd_state->R = (bdd*)GetBlock((nrules+1)*sizeof(bdd));
    bdd_state->C = (bdd*)GetBlock((nrules+1)*sizeof(bdd));
    bdd_state->bdd_mark = (bdd*)GetBlock((nrules+1)*sizeof(bdd));
    /* create signal atoms */
    for (i=0;i<nsigs;i++){
      bdd_state->lits[i] = bdd_new_var_last(bdd_state->bddm);
      bdd_state->newlits[i] = bdd_new_var_last(bdd_state->bddm);
    }
    /* create rule atoms */
    for (i=0;i<nrules;i++)
      bdd_state->bdd_mark[i] = bdd_new_var_last(bdd_state->bddm);
    bdd_state->bdd_mark[nrules] = NULL;
//     mark_vars = bdd_new_var_block(bdd_state->bddm,bdd_state->bdd_mark[0],
// 				  nrules-1);
//     bdd_var_block_reorderable(bdd_state->bddm,mark_vars,0);
    /* create link bits */
/* OLD VERSION WITH LINKS LOW-ORDER BIT FIRST */
//    for (i=0;i<bdd_state->linksize;i++)
//      bdd_state->links[i] = bdd_new_var_last(bdd_state->bddm);
    for (i=bdd_state->linksize-1;i>=0;i--)
      bdd_state->links[i] = bdd_new_var_last(bdd_state->bddm);
    bdd_state->links[bdd_state->linksize] = NULL;
//     link_vars = bdd_new_var_block(bdd_state->bddm,bdd_state->links[0],
// 				  bdd_state->linksize-1);
//     bdd_var_block_reorderable(bdd_state->bddm,link_vars,0);

    /* build link numbers */
    for (i = 0; i<bdd_state->maxlinks; i++){
      u = bdd_one(bdd_state->bddm);
      for (j = 0; j<bdd_state->linksize;j++){
	flag = 0;
	if (i & 1 << j){
	  t = bdd_state->links[j];
	}else{
	  flag = 1;
	  t = bdd_not(bdd_state->bddm,bdd_state->links[j]);
	}
	v = bdd_and(bdd_state->bddm,u,t);
	if (flag)
	  bdd_free(bdd_state->bddm,t);
	bdd_free(bdd_state->bddm,u);
	u = v;
      }
      bdd_state->L[i] = u;
    }
    /* build short link numbers */
    max=0;
    for (i = 0; i<bdd_state->maxlinks; i++){
      u = bdd_one(bdd_state->bddm);
      if (is_power_of_2_minus_1(i)>=0) max++;
      for (j = 0; j<max;j++){
	flag = 0;
	if (i & 1 << j){
	  t = bdd_state->links[j];
	}else{
	  flag = 1;
	  t = bdd_not(bdd_state->bddm,bdd_state->links[j]);
	}
	v = bdd_and(bdd_state->bddm,u,t);
	if (flag)
	  bdd_free(bdd_state->bddm,t);
	bdd_free(bdd_state->bddm,u);
	u = v;
      }
      bdd_state->SL[i] = u;
    }
    /* create index bits */
    for (i=0;i<nlog;i++){
      bdd_state->rows[i] = bdd_new_var_last(bdd_state->bddm);
      bdd_state->cols[i] = bdd_new_var_last(bdd_state->bddm);
    }
//     rc_vars = bdd_new_var_block(bdd_state->bddm,bdd_state->rows[0],
// 				  (2*nlog)-1);
//     bdd_var_block_reorderable(bdd_state->bddm,rc_vars,1);
//     region_vars=bdd_new_var_block(bdd_state->bddm,bdd_state->bdd_mark[0],
// 				  (nrules+bdd_state->linksize+(2*nlog))-1);
//     bdd_var_block_reorderable(bdd_state->bddm,rc_vars,1);
    /* build index numbers */
    for (i = 0;i<=nrules;i++){
      u = bdd_one(bdd_state->bddm);
      v = bdd_one(bdd_state->bddm);
      for (j = 0; j<nlog;j++){
	flag = 0;
	if (i & 1 << j){
	  t = bdd_state->rows[j];
	  s = bdd_state->cols[j];
	}else{
	  flag = 1;
	  t = bdd_not(bdd_state->bddm,bdd_state->rows[j]);
	  s = bdd_not(bdd_state->bddm,bdd_state->cols[j]);
	}
	w = bdd_and(bdd_state->bddm,u,t);
	x = bdd_and(bdd_state->bddm,v,s);
	if (flag){
	  bdd_free(bdd_state->bddm,s);
	  bdd_free(bdd_state->bddm,t);
	}
	bdd_free(bdd_state->bddm,u);
	bdd_free(bdd_state->bddm,v);
	u = w;
	v = x;
      }
      bdd_state->R[i] = u;
      bdd_state->C[i] = v;
      //  	fprintf(junk,"row %d:\n",i);
      //  	bdd_print_bdd(bddm,R[i],bdd_naming_fn_none,
      //  		      bdd_terminal_id_fn_none,NULL,junk);
      //  	fprintf(junk,"col %d:\n",i);
      //  	bdd_print_bdd(bddm,C[i],bdd_naming_fn_none,
      //  		      bdd_terminal_id_fn_none,NULL,junk);
    }
    /* null terminate arrays */
    bdd_state->lits[nsigs] = bdd_state->newlits[nsigs] = 
      bdd_state->rows[nlog] = bdd_state->cols[nlog] = 
      bdd_state->links[bdd_state->linksize]=NULL;
    /* create associations */
    bdd_state->marks = bdd_new_assoc(bdd_state->bddm,bdd_state->bdd_mark,0);
    bdd_state->lks = bdd_new_assoc(bdd_state->bddm,bdd_state->links,0);
  }
}
#else
//CUDD
bddADT new_bdd_state_table(int status,bool error,bddADT old_bdd_state,
			   signalADT *signals,int nsigs, int ninpsigs, 
			   int nrules, char * filename) 
{
#ifdef CUDD
  Cudd *mgr;
  bddADT bdd_state=NULL;
  add_list *list_temp,*front;
  ADD R,C,mvars;
  int r,c;
  BDD vars,pvars,invars,outvars,pinvars,poutvars,tmp_bdd;
  BDDvector *v,*pv;
  DdNode **vn,**pvn;
  int i,j,nlog;
  if (nsigs == 0) nsigs = 1;

  if (((status & LOADED) || (error==TRUE)) && (old_bdd_state != NULL)) {
    mgr=old_bdd_state->mgr;
    delete[] old_bdd_state->ERplus;
    delete[] old_bdd_state->ERminus;
    delete[] old_bdd_state->ESplus;
    delete[] old_bdd_state->ESminus;
    delete[] old_bdd_state->QSplus;
    delete[] old_bdd_state->QSminus;
    delete[] old_bdd_state->CCplus;
    delete[] old_bdd_state->CCminus;
    delete[] old_bdd_state->SSplus;
    delete[] old_bdd_state->SSminus;
    delete[] old_bdd_state->MAPplus;
    delete[] old_bdd_state->MAPminus;
    delete[] old_bdd_state->CCpsize;
    delete[] old_bdd_state->CCmsize;
    delete[] old_bdd_state->MAPpsize;
    delete[] old_bdd_state->MAPmsize;
    delete[] old_bdd_state->VCplus;
    delete[] old_bdd_state->VCminus;
    delete[] old_bdd_state->row;
    delete[] old_bdd_state->col;
    delete old_bdd_state->v;
    delete old_bdd_state->pv;
    delete old_bdd_state->covmasks;
    if (old_bdd_state->sigmask){
      delete[](old_bdd_state->sigmask);
      delete[](old_bdd_state->nmask);
      delete[](old_bdd_state->nomask);
      delete[](old_bdd_state->primemask);
      delete[](old_bdd_state->covmask);
    }
    front = old_bdd_state->front;
    while (front){
      list_temp = front->next;
      delete front;
      front = list_temp;
    }
    free(old_bdd_state->filename);
    delete old_bdd_state;
    delete mgr;
  } 

  if (error == FALSE){
  /* initialize CUDD bdd_state here */
    nlog = (int)ceil(log((float)nrules+1)/log((float)2));
    bdd_state=new bdd_data;
    bdd_state->mgr = new Cudd();//(0,0,32,1000,0);//512,262144
    mgr = bdd_state->mgr;
    //mgr->AutodynEnable(CUDD_REORDER_RANDOM_PIVOT);
    //mgr->SetMaxCacheHard(1000);
    bdd_state->maxdead = 0;

    /* initially empty state space */
    bdd_state->S = bdd_state->mgr->bddZero(); 
    bdd_state->N = bdd_state->mgr->bddZero();
    bdd_state->M = bdd_state->mgr->addZero();
    
    /* initialize size data */
    bdd_state->nsigs = nsigs;
    bdd_state->ninpsigs = ninpsigs;
    bdd_state->nrules = nrules;
    bdd_state->max_cubes = 16;

    /* initialize looping constructs. */
    /* ASSUMPTION: primes are immediately after sigs */
    bdd_state->unused.start = 0;
    bdd_state->unused.stop = bdd_state->max_cubes;
    bdd_state->unused.step = 1;
    bdd_state->sigs.start = bdd_state->unused.stop;
    bdd_state->sigs.step = 2 + 2 * bdd_state->max_cubes;
    bdd_state->sigs.stop = bdd_state->sigs.start + 
      nsigs * bdd_state->sigs.step;
    bdd_state->cubestep = 2; 
    bdd_state->covs.start = bdd_state->unused.stop+2;
    bdd_state->covs.step = 2 + 2 * bdd_state->max_cubes;
    bdd_state->covs.stop = bdd_state->covs.start+
      nsigs * bdd_state->covs.step;
    bdd_state->rules.start = bdd_state->sigs.stop;
    bdd_state->rules.stop = bdd_state->rules.start + nrules;
    bdd_state->rules.step = 1;
    bdd_state->rows.start = bdd_state->rules.stop;
    bdd_state->rows.stop = bdd_state->rows.start+ (2*nlog);
    bdd_state->rows.step = 2;
    bdd_state->cols.start = bdd_state->rules.stop+1;
    bdd_state->cols.stop = bdd_state->cols.start + (2*nlog);
    bdd_state->cols.step = 2;
    bdd_state->lastvar = bdd_state->rows.stop;

    /* copy filename and signal vector (mostly for CSC printouts. */
    bdd_state->filename = CopyString(filename);
    bdd_state->signals = signals;
 
    /* build var sets as BDDs*/
    vars=bdd_state->mgr->bddOne();
    pvars=bdd_state->mgr->bddOne();
    for (i = bdd_state->sigs.start; 
	 i < bdd_state->sigs.stop; 
	 i += bdd_state->sigs.step){
      vars *= mgr->bddVar(i);
      pvars *= mgr->bddVar(i+1);
    }
    mvars = vars.Add();;
    for (i = bdd_state->rules.start;
	 i < bdd_state->rules.stop;
	 i += bdd_state->rules.step){
      mvars *= mgr->addVar(i);
    }
    bdd_state->vars = vars;
    bdd_state->pvars = pvars;
    bdd_state->mvars = mvars;
    bdd_state->cvars = mgr->bddOne();
    for (i=0;i<bdd_state->max_cubes;i++)
      for (j = bdd_state->covs.start;
	   j < bdd_state->covs.stop;
	   j += bdd_state->covs.step){
	bdd_state->cvars *= mgr->bddVar(j+i*bdd_state->cubestep) * 
	  mgr->bddVar(j+1+i*bdd_state->cubestep);
      }
    /* build var sets as BDDvectors */
    vn = new DdNode*[nsigs];
    pvn = new DdNode*[nsigs];
    for (i = bdd_state->sigs.start,j=0;
	 i < bdd_state->sigs.stop; 
	 i += bdd_state->sigs.step,j++){
      vn[j] = (DdNode*)((mgr->bddVar(i)).getNode());
      pvn[j] = (DdNode*)((mgr->bddVar(i+1)).getNode());
    }
    v = new BDDvector(nsigs,mgr,vn);
    pv = new BDDvector(nsigs,mgr,pvn);
    bdd_state->v = v;
    bdd_state->pv = pv;
    delete[] vn;
    delete[] pvn;
    /* create empty arrays */
    bdd_state->ERplus = new bdd_list[nsigs-ninpsigs];
    bdd_state->ERminus = new bdd_list[nsigs-ninpsigs];
    bdd_state->VCplus = new bdd_list[nsigs-ninpsigs];
    bdd_state->VCminus = new bdd_list[nsigs-ninpsigs];
    bdd_state->ESplus = new BDD[nsigs-ninpsigs];
    bdd_state->ESminus = new BDD[nsigs-ninpsigs];
    bdd_state->QSplus = new BDD[nsigs-ninpsigs];
    bdd_state->QSminus = new BDD[nsigs-ninpsigs];
    bdd_state->CCplus = new BDD[nsigs-ninpsigs];
    bdd_state->CCminus = new BDD[nsigs-ninpsigs];
    bdd_state->SSplus = new BDD[nsigs-ninpsigs];
    bdd_state->SSminus = new BDD[nsigs-ninpsigs];
    bdd_state->CCpsize = new int[nsigs-ninpsigs];
    bdd_state->CCmsize = new int[nsigs-ninpsigs];
    bdd_state->MAPplus = new BDD[nsigs-ninpsigs];
    bdd_state->MAPminus = new BDD[nsigs-ninpsigs];
    bdd_state->MAPpsize = new int[nsigs-ninpsigs];
    bdd_state->MAPmsize = new int[nsigs-ninpsigs];


    /* create masks */
    bdd_state->sigmask = NULL;
    //bdd_state->primemask = NULL;
    //bdd_state->nmask = NULL;
    //bdd_state->nomask = NULL;
    //bdd_state->covmask = NULL;
    bdd_state->covmasks = new mask_list();
    bdd_state->n_cubes = 0;
    init_mask(bdd_state);

    /* add_list cleanup junk */
    bdd_state->front = NULL;
    bdd_state->back = NULL;
    

    /* make matrix indices */
    bdd_state->row = new ADD[nrules +1];
    bdd_state->col = new ADD[nrules +1];
    for (i = 0;i<=nrules;i++){
      R = mgr->addOne();
      C = mgr->addOne();
      for (j = 0,r=bdd_state->rows.stop-bdd_state->rows.step,
	     c=bdd_state->cols.stop-bdd_state->cols.step; 
	   j<nlog;r-=bdd_state->rows.step,c-=bdd_state->cols.step,j++)
	if (i& 1<<j){
	  R *= mgr->addVar(r);
	  C *= mgr->addVar(c);
	}
	else{
	  R *= ~mgr->addVar(r);
	  C *= ~mgr->addVar(c);
	}
      bdd_state->row[i] = R;
      bdd_state->col[i] = C;
    }
    
    /* CSC is valid until proven otherwise. */
    bdd_state->hasCSC = TRUE;
  }
  return bdd_state;
#else 
  return NULL;
#endif
}
#endif
/*****************************************************************************/
/* Free space used by old context signal tables.                             */
/*****************************************************************************/

void free_context_table(regionADT region)
{
  contextADT cur_problem=NULL;
  contextADT next_problem=NULL;

  cur_problem=region->context_table;
  while (cur_problem != NULL) {
    next_problem=cur_problem->next;
    free(cur_problem->state);
    free(cur_problem->solutions);
    free(cur_problem);
    cur_problem=next_problem;
  }
}

/*****************************************************************************/
/* Free space used by old set of enabled states and                          */
/*   create space for new set of enabled states.                             */
/*****************************************************************************/

regionADT *new_regions(int status,bool error,int oldntrans,int ntrans,
		       regionADT *oldregions)
{
  regionADT *regions=NULL;
  regionADT cur_region;
  regionADT next_region;
  int a,b;
  coverlistADT cur_cover;
  coverlistADT next_cover;

  if (((status & LOADED) || (error==TRUE)) && (oldregions != NULL)) {
    for (a=0;a<oldntrans;a++) {
      cur_region=oldregions[a];
      while (cur_region) {
	next_region=cur_region->link;
	if (cur_region->enstate != NULL) free(cur_region->enstate);
	if (cur_region->enablings != NULL) free(cur_region->enablings);
	if (cur_region->tsignals != NULL) free(cur_region->tsignals);
	if (cur_region->cover != NULL) free(cur_region->cover);
	cur_cover=cur_region->covers;
	while (cur_cover) {
	  next_cover=cur_cover->link;
	  if (cur_cover->cover) free(cur_cover->cover);
	  free(cur_cover);
	  cur_cover=next_cover;
	}
	if (cur_region->decomp != NULL) free(cur_region->decomp);
	if (cur_region->primes != NULL) 
	  for (b=0;b<cur_region->nprimes;b++)
	    free(cur_region->primes[b]);
	if (cur_region->primes) free(cur_region->primes);
	if (cur_region->implied != NULL) free(cur_region->implied);
	free_context_table(cur_region);
	free(cur_region);
	cur_region=next_region;
      }
    }
    free(oldregions);
  }
  if (error==FALSE) {
    regions=(regionADT*)GetBlock(ntrans * sizeof(regions[0]));
    for (a=0;a<ntrans;a++)
      regions[a]=NULL;
    return(regions);
  }
  return(NULL);
}

/*****************************************************************************/
/* Initialize implementation.                                                */
/*****************************************************************************/

void new_impl(designADT design,int ntrans,bool burst)
{
  regionADT cur_region;
  int a;

  if (burst) {
    if (design->status & FOUNDREG)
      design->regions=new_regions(design->status,FALSE,(2*design->nsignals)+1,
				  (2*design->nsignals)+1,design->regions);
  } else {
    for (a=0;a<ntrans;a++) {
      cur_region=design->regions[a];
      while (cur_region) {
	if (cur_region->cover != NULL) free(cur_region->cover);
	if (cur_region->tsignals != NULL) 
	  cur_region->cover=CopyString(cur_region->tsignals);
	free_context_table(cur_region);
	cur_region->context_table=NULL;
	cur_region=cur_region->link;
      }
    }
  }
}

/*****************************************************************************/
/* Initialize structure used for a design.                                   */
/*****************************************************************************/

designADT initialize_design(void)
{
  designADT design=NULL;

  design=(designADT)GetBlock(sizeof *design);
  design->status=RESET;
  design->filename[0]='\0';
  design->component[0]='\0';
  design->properties=NULL;
  design->signals=NULL;
  design->variables=NULL;
  design->events=NULL;
  design->merges=NULL;
  design->rules=NULL;
  design->cycles=NULL;
  design->markkey=NULL;
  initialize_state_table(design->status,FALSE,design->state_table);
#ifdef DLONG
  design->bdd_state=(bddADT)GetBlock(sizeof *(design->bdd_state));
  design->bdd_state->linksize=LOG_MAX_REGIONS;
  design->bdd_state->maxlinks=MAX_REGIONS;
  design->bdd_state->oldlinksize=LOG_MAX_REGIONS;
  design->bdd_state->oldmaxlinks=MAX_REGIONS;
#else
  // CUDD
  design->bdd_state = NULL;
#endif
  design->regions=NULL;

  // Allocate memory for an empty prob_vector to assign to cycle_period.
  // This will make the memory for cycle_period ALWAYS valid.  From here
  // on out it can be dereferenced, and any of its functions can be called.
  // However, if you use the [] operator with an index beyond its actual
  // size, you get what you deserve.
  design->cycle_period = new prob_vector();

  // Allocate memory for an empty trigger probability vector
  design->trigger_probs = new prob_vector();

  design->startstate=NULL;
  design->nevents=0;
  design->nplaces=0;
  design->ninputs=0;
  design->ninpsig=0;
  design->nrules=0;
  design->nsignals=0;
  design->nvars=0;
  design->nineqs=0;
  design->ndisj=0;
  design->ndummy=0;
  design->nconf=0;
  design->niconf=0;
  design->noconf=0;
  design->nord=0;
  design->ncausal=0;
  design->fromCSP=FALSE;
  design->fromVHDL=FALSE;
  design->fromHSE=FALSE;
  design->fromER=FALSE;
  design->fromTEL=FALSE;
  design->fromG=FALSE;
  design->fromLPN=FALSE;
  design->fromUNC=FALSE;
  design->fromRSG=FALSE;
  design->fromDATA=FALSE;
  design->closure=FALSE;
  design->si=FALSE;
  design->sis=FALSE;
  design->direct=FALSE;
  design->syn=FALSE;
  design->bdd=FALSE;
  design->orbits=FALSE;
  design->geometric=FALSE;
  design->srl=FALSE;
  design->hpn=FALSE;
  design->lhpndbm=FALSE;
  design->lhpnbdd=FALSE;
  design->lhpnsmt=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  design->untimed=TRUE;
  design->compress=FALSE;
  design->heuristic=FALSE;
  design->single=TRUE;
  design->manual=FALSE;
  design->verbose=FALSE;
  design->burstgc=FALSE;
  design->burst2l=FALSE;
  design->inponly=FALSE;
  design->abstract=FALSE;
  design->postproc=TRUE;
  design->newtab=FALSE;
  design->redchk=TRUE;
  design->expandRate=FALSE;
  design->xform2=TRUE;
  design->atomic=FALSE;
  design->gC=TRUE;
  design->gatelevel=FALSE;
  design->exact=TRUE;
  design->exception=TRUE;
  design->noparg=FALSE;
  design->nochecks=FALSE;
  design->multicube=FALSE;
  design->combo=TRUE;
  design->dot=FALSE;
  design->sharegates=TRUE;
  design->tolerance=0.01;
  design->brk_exception=FALSE;
  design->iterations=ITERATIONDEF;
  design->olditerations=ITERATIONDEF;
  design->maxsize=MAXSTACKSIZE;
  design->maxgatedelay=MAXGATEDELAY;
  design->ncycles=NUMCYCLES;
  design->oldmaxsize=MAXSTACKSIZE;
  design->oldmaxgatedelay=MAXGATEDELAY;
  design->oldncycles=NUMCYCLES;
  design->oldnevents=0;
  design->oldnsignals=0;
  design->oldnvars=0;
  design->timeopts.genrg=FALSE;
  design->timeopts.disabling=FALSE;
  design->timeopts.nofail=FALSE;
  design->timeopts.noproj=FALSE;
  design->timeopts.keepgoing=FALSE;
  design->timeopts.subsets=TRUE;
  design->timeopts.supersets=TRUE;
  design->timeopts.infopt=TRUE;
  design->timeopts.orbmatch=TRUE;
  design->timeopts.interleav=TRUE;
  design->timeopts.prune=TRUE;

  // 04/23/02 -- egm
  // This directs bap to use time dependent, not independent, choice 
  // semantics
  design->timeopts.TDC = false;

  // 05/16/02 -- egm
  // This enables partial order reduction in posets and bap
  /* enables POSET timing with tS (future variables) */
  design->timeopts.PO_reduce = false;

  /* runs the LPN code instead of LTN code */
  design->timeopts.lpn = false;

  /* running hpn reachability code */
  design->timeopts.hpn = false;
  
  design->comb=NULL;
  design->level=FALSE;

  // egm 08/30/00 -- New timing method based on geometric
  design->bag = FALSE;

  // egm 05/10/01 -- New timing method based on POSETs
  design->bap = FALSE;

  design->reduction = false;  // Don't attempt automatic concurrency reduction.
  design->pruning.not_first = false;  // Don't stop after first solution.
  design->pruning.preserve = false;//Don't preserve user-specified concurrency.
  design->pruning.ordered = false;//Allow rules between time-ordered events.
  design->pruning.subset = false;//Don't assume rule subset => state superset
  design->pruning.unsafe = true;  // Prune unsafe choice.
  design->pruning.expensive = false;//Don't use branch and bound.
  design->pruning.conflict = true;// Prune rules between conflicting events.
  design->pruning.reachable = true;// Prune redundant rules by reachability.
  design->pruning.dumb = true;// Prune rules to dummy events or from dummy 
                             //    events that have no fanin expression.
  
  design->insertion = true;   // Attempt automatic state-variable insertion.
  design->minins = false;   // Limit size of transition points to 1

  design->total_lits = 0;
  design->area = 0;

  design->inequalities = NULL;
  design->assignments = NULL;

  return(design);
}

/*****************************************************************************/
/* Reinitialize data structure used for a design.                            */
/*****************************************************************************/

void reinit_design(char command,designADT design,bool initreach)
{
  if (design->status & LOADED) {
    if (!initreach)
      reinit_rules(design->nevents,design->rules,&(design->nrules),
		   &(design->ncausal),&(design->nord));
    if (command == CYCLES)
      design->cycles=new_cycles(design->status,FALSE,design->oldncycles,
				design->ncycles,design->nevents,
				design->nevents,design->rules,design->cycles);
    else
      rulecpy(design->nevents,design->ncycles,design->cycles,design->rules,
              initreach);
  }
#ifdef DLONG
  initialize_bdd_state_table(design->status,FALSE,design->bdd_state,
			     design->signals,design->nsignals,
			     design->nrules);
#else
  //CUDD
  if (design->status & FOUNDRSG)
    design->bdd_state=new_bdd_state_table(design->status,FALSE,
					  design->bdd_state,design->signals,
					  design->nsignals,design->ninpsig,
					  design->nrules, design->filename);
#endif 
  if (design->status & FOUNDRSG){
    initialize_state_table(design->status,FALSE,design->state_table);
  }
  if (design->status & FOUNDREG)
    design->regions=new_regions(design->status,FALSE,(2*design->nsignals)+1,
				(2*design->nsignals)+1,design->regions);
  if (design->comb) free(design->comb);
  design->comb=NULL;
  // I'm sure what this is suppose to do, but I think it needs to zonk
  // cycle_period back to its initial state.
  design->cycle_period->erase( design->cycle_period->begin(),
			       design->cycle_period->end() );

  // I'm sure what this is suppose to do, but I think it needs to zonk
  // trigger_probs vector back to its initial state.
  design->trigger_probs->erase( design->trigger_probs->begin(),
			       design->trigger_probs->end() );
  
}

/*****************************************************************************/
/* Exchange design context.                                                  */
/*****************************************************************************/

void context_swap(designADT cs_design,designADT design,char *cs_command,
		  char command,int *cs_notreachable,int notreachable,
		  bool newevents)
{
  strcpy(cs_design->filename,design->filename);
  strcpy(cs_design->component,design->component);
  if (newevents) cs_design->events=design->events;
  cs_design->properties=design->properties;
  cs_design->signals=design->signals;
  cs_design->variables=design->variables;
  cs_design->rules=design->rules;
  cs_design->merges=design->merges;
  cs_design->cycles=design->cycles;
  cs_design->markkey=NULL;
  cs_design->regions=design->regions;

  // Move the contents of the old cycle_period in design into cs_design
  *(cs_design->cycle_period) = *(design->cycle_period);

  // Move the contents of the old trigger_progs in design into cs_design
  *(cs_design->trigger_probs) = *(design->trigger_probs);

  cs_design->startstate=design->startstate;
  cs_design->nevents=design->nevents;
  cs_design->nplaces=design->nplaces;
  cs_design->ninputs=design->ninputs;
  cs_design->nrules=design->nrules;
  cs_design->ninpsig=design->ninpsig;
  cs_design->nsignals=design->nsignals;
  cs_design->nvars=design->nvars;
  cs_design->nineqs=design->nineqs;
  cs_design->ndummy=design->ndummy;
  cs_design->ndisj=design->ndisj;
  cs_design->nconf=design->nconf;
  cs_design->nord=design->nord;
  cs_design->ncycles=design->ncycles;
  cs_design->ncausal=design->ncausal;
  cs_design->si=design->si;
  cs_design->syn=design->syn;
  cs_design->orbits=design->orbits;
  cs_design->manual=design->manual;
  cs_design->verbose=design->verbose;
  cs_design->exception=design->exception;
  cs_design->brk_exception=design->brk_exception;
  cs_design->iterations=design->iterations;
  cs_design->olditerations=design->olditerations;
  cs_design->maxsize=design->maxsize;
  cs_design->maxgatedelay=design->maxgatedelay;
  cs_design->oldnsignals=design->oldnsignals;
  cs_design->oldnvars=design->oldnvars;
  cs_design->oldmaxsize=design->oldmaxsize;
  cs_design->oldmaxgatedelay=design->oldmaxgatedelay;
  cs_design->oldnevents=design->oldnevents;
  cs_design->oldncycles=design->oldncycles;
  cs_design->status=design->status;
  cs_design->assignments=design->assignments;
  if (newevents) design->events=NULL;
  design->signals=NULL;
  design->variables=NULL;
  design->merges=NULL;
  design->rules=NULL;
  design->cycles=NULL;
  design->regions=NULL;
  (*cs_command)=command;
  (*cs_notreachable)=notreachable;
}

/*****************************************************************************/
/* Clear out a design.                                                       */
/*****************************************************************************/

void new_design(designADT design,bool freemem)
{
/*  design->events=new_events(design->status,TRUE,design->nevents,
			      design->nevents,design->events); */
  design->merges=new_merges(design->status,TRUE,design->nevents,
                              design->nevents,design->merges);
  design->rules=new_rules(design->status,TRUE,design->nevents,design->nevents,
                          design->rules);
  design->markkey=new_markkey(design->status,TRUE,
			      (design->nevents)*(design->nevents),
                              (design->nevents)*(design->nevents),
                              design->markkey);
  design->cycles=new_cycles(design->status,TRUE,design->ncycles,
			    design->ncycles,design->nevents,design->nevents,
			    design->rules,design->cycles);
  initialize_state_table(design->status,TRUE,design->state_table);
#ifdef DLONG
  initialize_bdd_state_table(design->status,TRUE, design->bdd_state,
			     design->signals,design->nsignals,
			     design->nrules);
  if (freemem) {
    free(design->bdd_state);
    design->bdd_state = NULL;
  }
#else
  //CUDD
  design->bdd_state=new_bdd_state_table(design->status,TRUE,design->bdd_state,
					design->signals,design->nsignals,
					design->ninpsig,design->nrules,
					design->filename);
#endif 
  if (freemem) {
    free(design);
  }
  
/*
  design->regions=new_regions(design->status,TRUE,(2*design->nsignals)+1,
			      (2*design->nsignals)+1,design->regions);
*/
}

ineqADT new_inequalities(ineqADT inequalities) 
{
  ineqADT curIneq,nextIneq;
  curIneq=inequalities;
  while(curIneq) {
    nextIneq=curIneq->next;
    free(curIneq);
    curIneq = nextIneq;
  }
  return NULL;
}

/*****************************************************************************/
/* Delete a design completely.                                               */
/*****************************************************************************/

void delete_design(designADT design)
{
#ifdef DLONG
  initialize_bdd_state_table(design->status,TRUE, design->bdd_state,
			     design->signals,design->nsignals,
			     design->nrules);
  free(design->bdd_state);
#else
  //CUDD
  design->bdd_state=new_bdd_state_table(design->status,TRUE,design->bdd_state,
					design->signals,design->nsignals,
					design->ninpsig,design->nrules,
					design->filename);
#endif 
  design->signals=new_signals(design->status,TRUE,design->nsignals,
			      design->nsignals,design->signals);
  design->variables=new_variables(design->status,TRUE,design->nvars,
				design->nvars,design->variables);
  design->events=new_events(design->status,TRUE,
			    design->nevents+design->nplaces,
			    design->nevents+design->nplaces,design->events);
  design->merges=new_merges(design->status,TRUE,
			    design->nevents,
			    design->nevents,design->merges);
  design->rules=new_rules(design->status,TRUE,design->nevents+design->nplaces,
			  design->nevents+design->nplaces,design->rules);
  design->markkey=new_markkey(design->status,TRUE,
			      (design->nevents+design->nplaces)*
			      (design->nevents+design->nplaces),
                              (design->nevents+design->nplaces)*
			      (design->nevents+design->nplaces),
                              design->markkey);
/*   design->cycles=new_cycles(design->status,TRUE,design->ncycles, */
/* 			    design->ncycles,design->nevents,design->nevents, */
/* 			    design->rules,design->cycles); */
/*   design->regions=new_regions(design->status,TRUE,(2*design->nsignals)+1, */
/* 			      (2*design->nsignals)+1,design->regions); */
  if (design->comb) free(design->comb);
  design->comb=NULL;
  // Make the call to clean up the cycle_period
  delete design->cycle_period;
  
  // Clean up the trigger_probs
  delete design->trigger_probs;

  initialize_state_table(design->status,TRUE,design->state_table);
  if (design->startstate) free(design->startstate);

  design->inequalities=new_inequalities(design->inequalities);

  free(design);
}
