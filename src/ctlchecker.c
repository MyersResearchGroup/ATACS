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

#include "ctlchecker.h"
#include "color.h"
#include "hpnrsg.h"
#include "findrsg.h"
#include <map>
#include <set>
#include <string>

using namespace std;

char prop[MAXTOKEN];
char*propptr; /* current position in prop */
int proplim;  /* end of data */

// #define __CTL_DEBUG__
// #define __FAIR_DEBUG__
// #define __SCC_DEBUG__

extern int ctlparse();
extern int ctlerror(char* s);
extern FILE *ctlin;

ctlstmt* ctlroot = NULL;

ctlstmt* newctlstmt(enum ops op, ctlstmt* s1, ctlstmt* s2, char* var)  {
   ctlstmt* cs = (ctlstmt*) malloc(sizeof(ctlstmt));
   cs->op = op;
   cs->s1 = s1;
   cs->s2 = s2;
   cs->var = var;
   ctlroot = cs;
   return cs;
 }

void printctlstmt(ctlstmt* root) {
  switch (root->op) {
  case OP_VAR:
    printf("%s", root->var);
    break;
  case OP_TRUE:
    printf("true");
    break;
  case OP_NOT:
    printf("~");
    printctlstmt(root->s1);
    break;
  case OP_EX:
    printf("EX ");
    printctlstmt(root->s1);
    break;
  case OP_EG:
    printf("EG ");
    printctlstmt(root->s1);
    break;
  case OP_OR:
    printctlstmt(root->s1);
    printf(" | ");
    printctlstmt(root->s2);
    break;
  case OP_AND:
    printctlstmt(root->s1);
    printf(" & ");
    printctlstmt(root->s2);
    break;
  case OP_EU:
    printf("E[");
    printctlstmt(root->s1);
    printf(" U ");
    printctlstmt(root->s2);
    printf("]");
  }
}

/*****************************************************************************
* Check a CTL property.
******************************************************************************/
bool checkCTL(stateADT *state_table,signalADT *signals,int nsignals,
              int nevents,int nplaces,eventADT *events,ruleADT **rules,
              char *property)
{
  //Make states w/out a succ into self loops
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
    }  
  }
  
  
  // Construct a map of state variable names to signal numbers
  // This is useful when determing a set of states where a signal is true.
  map<string,int> stateVec;
  for(int i=0;i<nsignals;i++) {
    stateVec[signals[i]->name] = i;
  }
#ifdef __CTL_DEBUG__
  int signal = 0;
  for(int i=0;i<nsignals;i++) {
    signal = stateVec[signals[i]->name];
    /* talk about the problem here... */
    if(1) {
      set<stateADT> s = atomicGetStates(state_table,signal);
      printf("States for %s: ",signals[i]->name);
      printSet(s);
    }
  }
#endif

  // Construct a set containing all states in the state graph.  This
  // is useful for taking the negation of a property.
  set<stateADT> U = findUniverse(state_table);

  stateADT initialState = NULL;
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      if (curstate->number == 0) {
        initialState = curstate;
        break;
      }
    }
    if (initialState) {
      break;
    }
  }
  
#ifdef __CTL_DEBUG__
  printf("Universe: ");
  printSet(U);
#endif

  // Debugging code for printing info about entire state graph.
#ifdef __CTL_DEBUG__
  statelistADT list;
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      /*
       * When reading in an rsg from a file...
       * Marking is NULL
       * Colorvec is NULL
       */
      printf("State: %s\n",curstate->state);
      printf("Number: %d\n",curstate->number);
      printf("Color: %d\n",curstate->color);
      printf("Pred: ");
      list = curstate->pred;
      while(list != NULL) {
        printf("%s[%d] ",list->stateptr->state,list->stateptr->number);
        list = list->next;
      }
      printf("\n");
      printf("Succ: ");
      list = curstate->succ;
      while(list != NULL) {
        printf("%s[%d] ",list->stateptr->state,list->stateptr->number);
        list = list->next;
      }
      printf("\n");        
      printf("\n");
    }
  }
#endif
  
  // Now actually perform the check
  // Read in a CTL statement

  if (!property) {
    printf("Enter a CTL property: ");
    scanf("%s",prop);
  } else {
    strcpy(prop,property);
  }
  printf("Checking CTL property: %s\n",prop);
  propptr=prop;
  proplim=(intptr_t)propptr+strlen(prop);

  ctlparse();
  printf("CTL Statement read in was: ");
  printctlstmt(ctlroot);
  printf("\n");
    
  set<stateADT> result = checkCTLRec(ctlroot, state_table,
				     stateVec, U, nevents,nplaces,events,
				     rules,signals,nsignals);
  printf("Result: ");
  if (result.find(initialState) != result.end()) {
    printf ("Property satisified in initial state.\n");
    return true;
  }
  else {
    printf("Property not satisfied in %i out of %i states.\n",
           (int) (U.size() - result.size()), (int) U.size());
    printf("Counterexample: ");
    set<stateADT> example = witnessCTLRec(newctlstmt(OP_NOT, ctlroot, NULL, NULL), state_table, stateVec, U);
    // Color the graph with resulting witness so that the error
    // trace can be displayed graphically.
    // reset colors of all nodes in graph.
    for(int i=0;i<PRIME;i++) {
      for (stateADT curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	curstate->color = -1;
      }
    }
    for (set<stateADT>::iterator i=example.begin(); i!=example.end(); i++) {
      (*i)->color = 1;
    }
    printSet(example);
    
  }
  //printSet(result);
  //printf("\n");
  prepColorSubGraph(result,state_table);
  return false;
}


/*****************************************************************************
* Recursively traverses the ctl statement.
******************************************************************************/
set<stateADT> checkCTLRec(ctlstmt* c, stateADT *state_table,
                          map<string,int>& stateVec, set<stateADT>& U,
                          int nevents,int nplaces,eventADT *events,
                          ruleADT **rules,signalADT *signals,int nsignals)
{
  set<stateADT> set1, set2, result;
  switch (c->op) {
  case OP_TRUE:
    result = U;
#ifdef __CTL_DEBUG__
    printf("TRUE: ");
    printSet(result);
#endif
    return result;
    break;
  case OP_VAR:
    result = atomicGetStates(state_table, stateVec[c->var]);
#ifdef __CTL_DEBUG__
    printf("VAR %s: ", c->var);
    printSet(result);
#endif
    return result;
    break;
  case OP_NOT:
    set1 = checkCTLRec(c->s1,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    result = checkNot(set1, U);
#ifdef __CTL_DEBUG__
    printf("NOT: ");
    printSet(result);
#endif
    return result;
    break;
  case OP_EX:
    set1 = checkCTLRec(c->s1,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    result = checkEX(set1);
#ifdef __CTL_DEBUG__
    printf("EX: ");
    printSet(result);
#endif
    return result;
    break;
  case OP_EG:
    set1 = checkCTLRec(c->s1,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    result = checkEG(set1,state_table,nevents,nplaces,events,rules,signals,
		     nsignals);
#ifdef __CTL_DEBUG__
    printf("EG: ");
    printSet(result);
#endif
    return result;
    break;
  case OP_OR:
    set1 = checkCTLRec(c->s1,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    set2 = checkCTLRec(c->s2,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    result = checkOr(set1, set2);
#ifdef __CTL_DEBUG__
    printf("OR: ");
    printSet(result);
#endif
    return result;
    break;
  case OP_AND:
    set1 = checkCTLRec(c->s1,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    set2 = checkCTLRec(c->s2,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    result = checkAnd(set1, set2);
#ifdef __CTL_DEBUG__
    printf("AND: ");
    printSet(result);
#endif
    return result;
    break;
  case OP_EU:
    set1 = checkCTLRec(c->s1,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    set2 = checkCTLRec(c->s2,state_table,stateVec,U,nevents,nplaces,events,
                       rules,signals,nsignals);
    result = checkEU(set1, set2);
#ifdef __CTL_DEBUG__
    printf("EU: ");
    printSet(result);
#endif
    return result;
    break;
  default:
    printf("FATAL ERROR IN CHECKCTLREC: UNHANDLED NODE TYPE.");
    return set1;
    break;
  }
  return set1;
}

/*****************************************************************************
* Recursively traverses the ctl statement to find a witness.
******************************************************************************/
set<stateADT> witnessCTLRec(ctlstmt* c, stateADT *state_table,
                            map<string,int>& stateVec, set<stateADT>& U) 
{
  set<stateADT> set1, set2, result;
  switch (c->op) {
  case OP_TRUE:
    result = U;
    return result;
    break;
  case OP_VAR:
    result = atomicGetStates(state_table, stateVec[c->var]);
    return result;
    break;
  case OP_NOT:
    set1 = witnessCTLRec(c->s1, state_table, stateVec, U);
    result = checkNot(set1, U);
    return result;
    break;
  case OP_EX:
    set1 = witnessCTLRec(c->s1, state_table, stateVec, U);
    result = witnessEX(set1);
    return result;
    break;
  case OP_EG:
    set1 = witnessCTLRec(c->s1, state_table, stateVec, U);
    result = witnessEG(set1, state_table);
    return result;
    break;
  case OP_OR:
    set1 = witnessCTLRec(c->s1, state_table, stateVec, U);
    set2 = witnessCTLRec(c->s2, state_table, stateVec, U);
    result = checkOr(set1, set2);
    return result;
    break;
  case OP_AND:
    set1 = witnessCTLRec(c->s1, state_table, stateVec, U);
    set2 = witnessCTLRec(c->s2, state_table, stateVec, U);
    result = checkAnd(set1, set2);
    return result;
    break;
  case OP_EU:
    set1 = witnessCTLRec(c->s1, state_table, stateVec, U);
    set2 = witnessCTLRec(c->s2, state_table, stateVec, U);
    result = witnessEU(set1, set2);
    return result;
    break;
  default:
    printf("FATAL ERROR IN WITNESSCTLREC: UNHANDLED NODE TYPE.");
    return set1;
    break;
  }
  return set1;
}

/*****************************************************************************
* Get the index into the clocks matrix for the event based place p.
******************************************************************************/
int getPIndex(int p,clocklistADT c)
{
  for(int i=0;i<c->clocknum;i++) {
    if(c->clockkey[i].enabled == p) {
      return i;
    }
  }
#ifdef __FAIR_DEBUG__
  cSetFg(RED);
  printf("Warning: ");
  cSetAttr(NONE);
  printf("p is not found in the clocks matrix!");
#endif
  return -1;
}

/*****************************************************************************
* Check that every zone in the given set of states has p increasing
* and less than or equal to the upper bound.
******************************************************************************/
bool chkInc(int p,int t,set<stateADT> f1,stateADT *state_table,int upper,
            eventADT *events)
{
  for(set<stateADT>::iterator s = f1.begin();s != f1.end();s++) {
    clocklistADT c = (*s)->clocklist;
    while(c != NULL) {
      int pInd = getPIndex(p,c);
      if(!(c->clockkey[pInd].eventk_num > 0 &&
           c->clocks[pInd][0] <= chkDiv(upper,c->clockkey[pInd].eventk_num,
                                        'C'))) {
#ifdef __FAIR_DEBUG__
        printf("NOT enabled -- rule[%s][%s] warp:%d zoneUpperBound:%d upper:%d\n",
               events[p]->event,events[t]->event,c->clockkey[pInd].eventk_num,
               c->clocks[pInd][0],upper);
#endif
        return false;
      }
      c = c->next;
    }
  }
  return true;
}

/*****************************************************************************
* Check that every zone in the given set of states has p decreasing
* and is greater than or equal to the lower bound.
******************************************************************************/
bool chkDec(int p,int t,set<stateADT> f1,stateADT *state_table,int lower,
            eventADT *events)
{
  for(set<stateADT>::iterator s = f1.begin();s != f1.end();s++) {
    clocklistADT c = (*s)->clocklist;
    int i=0;
    while(c != NULL) {
      int pInd = getPIndex(p,c);
      if(!(c->clockkey[pInd].eventk_num < 0 &&
           abs(c->clocks[pInd][0]) >=
           abs(chkDiv(lower,c->clockkey[pInd].eventk_num,'C')))) {
#ifdef __FAIR_DEBUG__
        printf("NOT enabled -- rule[%s][%s] warp:%d zoneUpperBound:%d lower:%d\n",
               events[p]->event,events[t]->event,c->clockkey[pInd].eventk_num,
               c->clocks[pInd][0],lower);
#endif
        return false;
      }
      c = c->next;
      i++;
    }
  }
  return true;
}

/*****************************************************************************
* Check that every zone in the given set of states has p stable and is
* less than or equal to the upper bound and greater than or equal to
* the lower bound.
******************************************************************************/
bool chkStable(int p,int t,set<stateADT> f1,stateADT *state_table,int upper,
               int lower,eventADT *events)
{
  for(set<stateADT>::iterator s = f1.begin();s != f1.end();s++) {
        clocklistADT c = (*s)->clocklist;
    while(c != NULL) {
      int pInd = getPIndex(p,c);
      if(!(c->clockkey[pInd].eventk_num == 0 &&
           c->clocks[pInd][0] <= upper && c->clocks[pInd][0] >= lower)) {
#ifdef __FAIR_DEBUG__
        printf("NOT enabled -- rule[%s][%s] warp:%d zoneUpperBound:%d upper:%d lower:%d\n",
               events[p]->event,events[t]->event,c->clockkey[pInd].eventk_num,
               c->clocks[pInd][0],upper,lower);
#endif
        return false;
      }
      c = c->next;
    }
  }
  return true;
}

/*****************************************************************************
* Returns true if the same transition is enabled in every state of the
* given set of states.
******************************************************************************/
bool isEnabled(int t,set<stateADT> f1,stateADT *state_table,int nevents,
               int nplaces,eventADT *events,ruleADT **rules,int nsignals)
{
  int i;
  bool satisfied;
  level_exp curLevel;

  /* check if hsl formula is satisified */
  if(!events[t]->SOP) {
    return false;
  }
  for (set<stateADT>::iterator s = f1.begin();s != f1.end(); s++) {
    for(curLevel=events[t]->SOP;curLevel;curLevel=curLevel->next) {
      satisfied=true;
      for(i=0;i<nsignals;i++) {
	if((curLevel->product[i]=='1' &&
	    ((*s)->state[i]=='0' || (*s)->state[i]=='R')) ||
	   (curLevel->product[i]=='0' &&
	    ((*s)->state[i]=='1' || (*s)->state[i]=='F'))) {
	  satisfied=false;
	  break;
	}
      }
    }
    if(satisfied) {
      return true;
    }
  }
  return false;

  
  /* see if the transition depends on any continuous transitions...if
     it does not then it enabled == discretely enabled */
  bool discreteRule = true;
  for(int p=nevents;p<nevents+nplaces;p++) {
    if(rules[p][t]->ruletype > NORULE &&
       events[p]->type & CONT) {
      discreteRule = false;
      break;
    }
  }
  if(discreteRule) {
    return true;
  }

  /* check to see if the transition must become enabled in all states
     in the given set */
  for(int p=nevents;p<nevents+nplaces;p++) {
    if(rules[p][t]->ruletype > NORULE &&
       events[p]->type & CONT) {
      for (set<stateADT>::iterator s = f1.begin();s != f1.end(); s++) {
	int pInd = getPIndex(p,(*s)->clocklist);
	if((*s)->clocklist->clockkey[pInd].eventk_num > 0) {
	  if(!chkInc(p,t,f1,state_table,rules[p][t]->pupper,events)) {
	    return false;
	  }
	}
	else if((*s)->clocklist->clockkey[pInd].eventk_num < 0) {
	  if(!chkDec(p,t,f1,state_table,rules[p][t]->plower,events)) {
	    return false;
	  }
	}
	else {
	  if(!chkStable(p,t,f1,state_table,rules[p][t]->pupper,
			rules[p][t]->plower,events)) {
	    return false;
	  }
	} 
      }
    }
  }
  return true;
}

/*****************************************************************************
* Returns true if the given path is fair.
******************************************************************************/
bool fairPath(set<stateADT> f1,stateADT *state_table,int nevents,
              int nplaces,eventADT *events,ruleADT **rules,
	      signalADT *signals,int nsignals)
{
  bool enabled;

  /* Find an event that is always enabled in the SCC */
  for (int i=1;i<nevents+nplaces;i++) {
    for(set<stateADT>::iterator s = f1.begin();s != f1.end();s++) {
      enabled=false;
      for (statelistADT succ=(*s)->succ;succ;succ=succ->next) 
	if ((succ->iteration != 'I')&&(succ->enabled==i)) {
	  enabled=true;
	  break;
	}
      /* Make sure enabled transition strictly exits the SCC */
      if (enabled) {
	for(set<stateADT>::iterator s2 = f1.begin();s2 != f1.end();s2++) {
	  for (statelistADT succ=(*s)->succ;succ;succ=succ->next) 
	    if ((succ->iteration != 'I')&&(succ->enabled==i)&&
		(succ->stateptr==(*s2))) {
	      enabled=false;
	    }
	}
      } 
      if (!enabled) break;
    }
    if (enabled) {
#ifdef __FAIR_DEBUG__
      printf("%s[%d] is always enabled\n",events[i]->event,i);
#endif
      return false;
    }
  }
  /* Find a predicate that is always enabled in the SCC */
  for (int i=0;i<nsignals;i++) {
    for(set<stateADT>::iterator s = f1.begin();s != f1.end();s++) {
      enabled=false;
      for (statelistADT succ=(*s)->succ;succ;succ=succ->next) 
	if ((succ->iteration == 'I')&&(succ->enabled==i)) {
	  enabled=true;
	  break;
	}
      /* Make sure enabled transition strictly exits the SCC */
      if (enabled) {
	for(set<stateADT>::iterator s2 = f1.begin();s2 != f1.end();s2++) {
	  for (statelistADT succ=(*s)->succ;succ;succ=succ->next) 
	    if ((succ->iteration == 'I')&&(succ->enabled==i)&&
		(succ->stateptr==(*s2))) {
	      enabled=false;
	    }
	}
      } 
      if (!enabled) break;
    }
    if (enabled) {
#ifdef __FAIR_DEBUG__
      printf("%s[%d] is always enabled\n",signals[i]->name,i);
#endif
      return false;
    }
  }
#ifdef __FAIR_DEBUG__
  printf("FAIR PATH\n");
#endif
  return true;

//   for(int i=1;i<nevents;i++) {
//     bool flag=false;
//     /* foreach state s in f1 */
//     for(set<stateADT>::iterator s = f1.begin();s != f1.end();s++) {
//       if ((*s)->enablings[i]=='F') {
//         flag=true;
//         break;
//       }
//     }
//     if (!flag) {
// #ifdef __FAIR_DEBUG__
//       printf("%s[%d] is always discretely enabled\n",events[i]->event,i);
// #endif
//       if(isEnabled(i,f1,state_table,nevents,nplaces,events,rules,nsignals)) {
// #ifdef __FAIR_DEBUG__
//         printf("%s[%d] is always enabled\n",events[i]->event,i);
// #endif
//         return false;
//       }
// #ifdef __FAIR_DEBUG__
//       printf("%s[%d] is NOT always enabled\n",events[i]->event,i);
// #endif
//     }
//   }
//   return true;
}

/*****************************************************************************
* Finds the universe set which is useful for negation.
******************************************************************************/
set<stateADT> findUniverse(stateADT *state_table)
{
  set<stateADT> U;
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      U.insert(curstate);
    }
  }
  return U;
}

/*****************************************************************************
* Finds all states where an atomic proposition is true.
******************************************************************************/
set<stateADT> atomicGetStates(stateADT *state_table,int signal) 
{
  set<stateADT> states;
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      if(curstate->state[signal] == '1' ||
         curstate->state[signal] == 'F') {
        states.insert(curstate);
      }
    }
  }
  return states;
}

/*****************************************************************************
* All states in state graph with those in f1 removed.
******************************************************************************/
set<stateADT> checkNot(set<stateADT> f1, set<stateADT> U) 
{
  set<stateADT> result = U;
  for (set<stateADT>::iterator i = f1.begin(); i != f1.end(); i++) {
    result.erase(result.find(*i));
  }  
  return result;
}

/*****************************************************************************
* Used in finding a witness... Invert each set within f1.
******************************************************************************/
set<set<stateADT> > witnessNot(set<set<stateADT> > f1, set<stateADT> U) 
{
  set<set<stateADT> > finalResult;
  for (set<set<stateADT> >::iterator i = f1.begin(); i != f1.end(); i++) {
    set<stateADT> result = U;
    for (set<stateADT>::iterator j = i->begin(); j != i->end(); j++) {
      result.erase(result.find(*j));
    }
    finalResult.insert(result);
  }
  return finalResult;
}

/*****************************************************************************
* Combination of all states in f1 and f2. (Union of f1 and f2).
******************************************************************************/
set<stateADT> checkOr(set<stateADT> f1, set<stateADT> f2)
{
  set<stateADT> result;
  for (set<stateADT>::iterator i = f1.begin(); i != f1.end(); i++) {
    result.insert(*i);
  }
  for (set<stateADT>::iterator i = f2.begin(); i != f2.end(); i++) {
    result.insert(*i);
  }
  return result;
}

/*****************************************************************************
* Combination of all states in f1 and f2. (Union of each set in f1
* with each set in f2).
******************************************************************************/
set<set<stateADT> > witnessOr(set<set<stateADT> > f1, set<set<stateADT> > f2)
{
  set<set<stateADT> > finalResult;
  for (set<set<stateADT> >::iterator j = f1.begin(); j != f1.end(); j++) {
    for (set<set<stateADT> >::iterator k = f2.begin(); k != f2.end(); k++) {
      set<stateADT> result;
      for (set<stateADT>::iterator i = j->begin(); i != j->end(); i++) {
        result.insert(*i);
      }
      for (set<stateADT>::iterator i = k->begin(); i != k->end(); i++) {
        result.insert(*i);
      }
      finalResult.insert(result);
    }
  }
  return finalResult;
}

/*****************************************************************************
* Intersection of all states in f1 and f2. 
******************************************************************************/
set<stateADT> checkAnd(set<stateADT> f1, set<stateADT> f2)
{
  set<stateADT> result;
  for (set<stateADT>::iterator i = f1.begin(); i != f1.end(); i++) {
    for (set<stateADT>::iterator j = f2.begin(); j != f2.end(); j++) {
      if (*i == *j) {
        result.insert(*i);
        break;
      }
    }
  }
  return result;
}

/*****************************************************************************
* Intersection of all states in each set in f1 with all states in each
* set in f2.
******************************************************************************/
set<set<stateADT> > witnessAnd(set<set<stateADT> >f1, set<set<stateADT> >f2)
{
  set<set<stateADT> > finalResult;
  for (set<set<stateADT> >::iterator k = f1.begin(); k != f1.end(); k++) {
    for (set<set<stateADT> >::iterator l = f2.begin(); l != f2.end(); l++) {
      set<stateADT> result;
      for (set<stateADT>::iterator i = k->begin(); i != k->end(); i++) {
        for (set<stateADT>::iterator j = l->begin(); j != l->end(); j++) {
          if (*i == *j) {
            result.insert(*i);
            break;
          }
        }
      }
      finalResult.insert(result);
    }
  }
  return finalResult;
}

/*****************************************************************************
* Returns the set of states who have a successor to a state in f1.
******************************************************************************/
set<stateADT> checkEX(set<stateADT> f1) 
{
  set<stateADT> result;
  for(set<stateADT>::iterator sIter = f1.begin();sIter != f1.end();sIter++) {
    stateADT s = *sIter;
    if(s) {  
      statelistADT list = s->pred;
      while(list != NULL) {
        result.insert(list->stateptr);
        list = list->next;
      }
    }
  }
  return result;
}

/*****************************************************************************
* Returns a state that is a predecessor to a state in f1
******************************************************************************/
set<stateADT> witnessEX(set<stateADT> f1) 
{
  set<stateADT> result;
  for(set<stateADT>::iterator sIter = f1.begin();sIter != f1.end();sIter++) {
    stateADT s = *sIter;
    if(s) {  
      statelistADT list = s->pred;
      if(list != NULL) {
        result.insert(list->stateptr);
        return result;
      }
    }
  }
  return result;
}

/*****************************************************************************
* Returns the set of states that satisfy E[f1 U f2].
******************************************************************************/
set<stateADT> checkEU(set<stateADT> f1,set<stateADT> f2)
{
  set<stateADT> T = f2;
  set<stateADT> EU = f2;
  
  while(!T.empty()) {
    //printf(" T: ");
    //printSet(T);
    set<stateADT >::iterator tIter = T.begin();
    stateADT s = *tIter;
    statelistADT pList = s->pred;
    while(pList != NULL) {
      set<stateADT>::iterator EUIter = EU.find(pList->stateptr);
      set<stateADT>::iterator f1Iter = f1.find(pList->stateptr);
      //printf(" EU: ");
      //printSet(EU);
      //printf(" f1: ");
      //printSet(f1);
      //printf(" Checking %i\n", pList->stateptr->number);
      if(EUIter == EU.end() && f1Iter != f1.end()) {
        //printf("Adding %i to EU\n", pList->stateptr->number);
        EU.insert(pList->stateptr);
        T.insert(pList->stateptr);
      }
      pList = pList->next;
    }
    T.erase(*tIter);
  }
  return EU;
}

/*****************************************************************************
* Returns the set of states that form a single path satisfying E[f1 U f2].
******************************************************************************/
set<stateADT> witnessEU(set<stateADT> f1, set<stateADT> f2)
{
  set<stateADT> T = f2;
  set<stateADT> EU = f2;
  
  while(!T.empty()) {
    //printf(" T: ");
    //printSet(T);
    set<stateADT >::iterator tIter = T.begin();
    stateADT s = *tIter;
    statelistADT pList = s->pred;
    while(pList != NULL) {
      set<stateADT>::iterator EUIter = EU.find(pList->stateptr);
      set<stateADT>::iterator f1Iter = f1.find(pList->stateptr);
      //printf(" EU: ");
      //printSet(EU);
      //printf(" f1: ");
      //printSet(f1);
      //printf(" Checking %i\n", pList->stateptr->number);
      if(EUIter == EU.end() && f1Iter != f1.end()) {
        //printf("Adding %i to EU\n", pList->stateptr->number);
        EU.insert(pList->stateptr);
        T.insert(pList->stateptr);
      }
      pList = pList->next;
    }
    T.erase(*tIter);
  }
  return EU;
}

/*****************************************************************************
* Returns the set of states that satisfy EG f1.
******************************************************************************/
set<stateADT> checkEG(set<stateADT> f1,stateADT *state_table,int nevents,
                      int nplaces,eventADT *events,ruleADT **rules,
		      signalADT *signals,int nsignals)
{
  //printf("In EG\n");
  createSubGraph(f1,state_table);
  set<set<stateADT> > tempT = stateSCC(state_table);
  set<stateADT> T; // = stateSCC(state_table);
  
  for (set<set<stateADT> >::iterator i=tempT.begin(); i != tempT.end(); i++) {
#ifdef __FAIR_DEBUG__
    printf("Checking:");
    printSet((*i));
#endif  
    if(fairPath((*i),state_table,nevents,nplaces,events,rules,signals,
		nsignals)) {
      T.insert(i->begin(), i->end());
#ifdef __FAIR_DEBUG__
      printf("FAIR\n");
#endif  
    }
  }

  set<stateADT> EG = T;

  while(!T.empty()) {
    set<stateADT>::iterator tIter = T.begin();
    stateADT s = *tIter;
    statelistADT pList = s->pred;
    while(pList != NULL) {
      set<stateADT>::iterator subIter = f1.find(pList->stateptr);
      set<stateADT>::iterator EGIter = EG.find(pList->stateptr);
      if(subIter != f1.end() && EGIter == EG.end()) {
        EG.insert(pList->stateptr);
        T.insert(pList->stateptr);
      }
      pList = pList->next;
    }
    T.erase(*tIter);
  }
  //printf("Leaving EG\n");
  return EG;
}

/*****************************************************************************
* Returns the set of states that satisfy EG f1.  Modified to find a
* single witness
******************************************************************************/
set<stateADT> witnessEG(set<stateADT> f1,stateADT *state_table)
{
  //printf("In EG\n");
  createSubGraph(f1,state_table);
  set<set<stateADT> > tempT = stateSCC(state_table);
  set<stateADT> T; // = stateSCC(state_table);

  if (tempT.size() != 0) {
    set<stateADT> smallest = *(tempT.begin());
    for (set< set<stateADT> >::iterator i = tempT.begin();
         i != tempT.end(); i++) {
      if (i->size() < smallest.size()) {
        smallest = *i;
      }
    }
    T = smallest;
    printf("size of SCC used: %i\n", (int) smallest.size());
  }
  
    
  set<stateADT> EG = T;

  while(!T.empty()) {
    set<stateADT>::iterator tIter = T.begin();
    stateADT s = *tIter;
    statelistADT pList = s->pred;
    while(pList != NULL) {
      set<stateADT>::iterator subIter = f1.find(pList->stateptr);
      set<stateADT>::iterator EGIter = EG.find(pList->stateptr);
      if(subIter != f1.end() && EGIter == EG.end()) {
        EG.insert(pList->stateptr);
        T.insert(pList->stateptr);
      }
      pList = pList->next;
    }
    T.erase(*tIter);
  }
  //printf("Leaving EG\n");
  return EG;
}

/*****************************************************************************
* Prints out the given set.
******************************************************************************/
void printSet(set<stateADT> s)
{
  for(set<stateADT>::iterator sIter = s.begin();sIter != s.end();sIter++) {
    printf("%d ",(*sIter)->number);
  }
  printf("\n");
}

/*****************************************************************************
* Prints out the given set of sets.
******************************************************************************/
void printSetOfSets(set<set<stateADT> > s)
{
  for(set<set<stateADT> >::iterator sIter = s.begin();sIter != s.end();
      sIter++) {
    printSet(*sIter);
  }
  printf("\n");
}

/*****************************************************************************
* Returns true if the given state contains a self loop.  This function
* is used to detect non-trivial connected components.
******************************************************************************/
bool hasSelfLoop(stateADT s) 
{
  if(s->succ != NULL) {
    statelistADT sl = s->succ;
    while(1) {
      if(s == sl->stateptr && sl->iteration == FIRE_TRANS) {
#ifdef __SCC_DEBUG__
        printf("\nState %d has a self loop.\n",s->number);
#endif
        return true;
      }
      sl = sl->next;
      if(sl == NULL) {
        break;
      }
    }
  }
  return false;
}

/****************************************************************************
 * Performs a depth first search of the state graph using the
 * algorithm in "Intro to Algorithms" by Cormn, Leiserson, and Rivest
 *
 * * ONLY STATES WHERE VISITED==TRUE ARE CONSIDERED PART OF THE GRAPH
 *
 ****************************************************************************/
void stateDFS(stateADT *state_table)
{
  // Initialize the color of all the state nodes to 1.
  for (int i = 0; i < PRIME; i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      curstate->color = 1;
      curstate->highlight = -1;
    }  
  }
  int time = 0;
  // Visit each vertex and if it hasn't already been visited, perform a
  // depth first earch on it.
  for (int i = 0; i < PRIME; i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      if (curstate->color == 1 && curstate->visited) {
        stateDFSVisit(curstate, time);
      }  
    }
  }
}

/****************************************************************************
 * Helper function for stateDFS
 *
 * * ONLY STATES WHERE VISITED==TRUE ARE CONSIDERED PART OF THE GRAPH
 *
 ****************************************************************************/
void stateDFSVisit(stateADT u, int &time)
{
  u->color = 2;  // a vertex with color 2 has just been discovered.
  // I could mark the discovery time here but we don't need it and
  // there is not variable to store that value.
  time++;
  
  for (statelistADT v = u->succ; v != NULL; v = v->next) {
    if (v->stateptr->color == 1 && v->stateptr->visited) {
      stateDFSVisit(v->stateptr, time);
    }
  }
  u->color = 3; // a vertex with color 3 has been finished.
  u->highlight = time; // Mark the finishing time of this node.
  time++;
}

/******************************************************************************
 * Constructs a set of all strongly connected components for the state
 * graph.  Uses stateDFS and stateDFSVisit.  Follows the algorithm in
 * "Intro to Algorithms" by Cormn, Leiserson, and Rivest.
 *
 * * ONLY STATES WHERE VISITED==TRUE ARE CONSIDERED PART OF THE GRAPH
 *
 *****************************************************************************/
set<set<stateADT> > stateSCC(stateADT *state_table) 
{
  // First call stateDFS to calculate the finishing time for each node.
  stateDFS(state_table);
  set<set<stateADT> > T;
  
  // Initialize the color of all the state nodes to 1.
  for (int i = 0; i < PRIME; i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      curstate->color = 1;
    }  
  }
  // Perform a DFS search through the transverse of the graph going in
  // the order of decreasing finishing times to generate the list of
  // strongly connected components.
  bool done = false;
  while (!done) {
    done = true;
    int high = -1;
    stateADT highi = NULL;
    for (int i = 0; i < PRIME; i++) {
      for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
        if (curstate->visited &&
            curstate->color == 1 && curstate->highlight > high) {
          high = curstate->highlight;
          highi = curstate;
          done = false;
        }
      }
    }
    if (done)
      break;
    //printf("SCC: ");
    set<stateADT> addT;
    stateSCCHelp(highi, addT);
    // Make sure the SCC is non-trivial.
    if (addT.size() == 1) {
      stateADT s = *(addT.begin());
      if (hasSelfLoop(s)) {
#ifdef __SCC_DEBUG__
        printf("Adding SCC singleton set:");
        printSet(addT);
#endif
        T.insert(addT);
        //T.insert(s);
      }
      
    }
    else {
#ifdef __SCC_DEBUG__
        printf("Adding SCC set:");
        printSet(addT);
#endif
      T.insert(addT);
      //T.insert(addT.begin(), addT.end());
    }
    
    //printf("\n");
  }
#ifdef __SCC_DEBUG__
  printf("Returning SCC...\n");
  printSetOfSets(T);
#endif
  return T;
}

/****************************************************************************
 * Helper function for stateSCC
 *
 * * ONLY STATES WHERE VISITED==TRUE ARE CONSIDERED PART OF THE GRAPH
 *
 ****************************************************************************/
void stateSCCHelp(stateADT u, set<stateADT> &T)
{
  u->color = 2;  // a vertex with color 2 has just been discovered.
  for (statelistADT v = u->pred; v != NULL; v = v->next) {
    if (v->stateptr->color == 1 && v->stateptr->visited) {
      stateSCCHelp(v->stateptr, T);
    }
  }
  T.insert(u);  
  //printf("%i ", u->number);
  u->color = 3; // a vertex with color 3 has been finished.
}  

/*****************************************************************************
* Creates a subgraph as defined by stateSCC of states in the given set.
******************************************************************************/
void createSubGraph(set<stateADT> S,stateADT *state_table)
{
  /* reset any bools currently set */
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      curstate->visited = false;
    }
  }
  for(set<stateADT>::iterator sIter = S.begin();sIter != S.end();sIter++) {
    (*sIter)->visited = true;
  }
}

/*****************************************************************************
* Color the states in the set red when a dot graph is printed.
******************************************************************************/
void prepColorSubGraph(set<stateADT> S,stateADT *state_table)
{
  /* reset any highlighting currently set */
  for(int i=0;i<PRIME;i++) {
    for (stateADT curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      curstate->highlight = 0;
    }
  }
  for(set<stateADT>::iterator sIter = S.begin();sIter != S.end();sIter++) {
    (*sIter)->highlight = 1;
  }
}
