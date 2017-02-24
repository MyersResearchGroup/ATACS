/*****************************************************************************/
/* stateasgn.c                                                               */
/*****************************************************************************/
#ifndef OSX
#ifndef NT
#include <values.h>
#endif
#endif

#include "struct.h"
#include "compile.h"
#include "connect.h"
#include "interface.h"
#include <iostream>
#include "findrsg.h"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include "findwctd.h"

#include "CTextTel.h"
#include "stateasgn.h"
#include "svhelpers.h"
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <queue> // for queue used in colorRise, colorFall
#include "hse.hpp"

#ifndef __TEMPLATE_PATCH
template<class back_insert_iter>
#endif

bool findER(stateADT state_table[], eventADT events[],
	    int nevents, int eventIndex, back_insert_iter bii_ER)
{
  // check each state in the state table to see if
  // the enabling string contains an R or F, as appropriate,
  // in the correct location
  // if yes, make sure the event is correct
  // then insert this state pointer into the ER list
  int i;
  stateADT curstate;
  char foundChar;
  char targetChar;
  state_list thisGenerationList;
  state_list nextGenerationList;
  stateADT thisState;
  statelistADT nextNode;
  bool ER_inclusion_known=false;
  bool found=false;
  list<int> otherOccuranceList;
  string targetAfterSlash;
  string afterSlash;
  string occuranceString;
  unsigned int slashPoint;

  eventADT event(events[eventIndex]);
  string eventString(event->event);
  
  // if rising event, look for R, falling look for F
  if (eventString.find("+") != string::npos)
    {
      targetChar='R'; 
    }
  else
    {
      targetChar='F'; 
    }
  
  // figure out what this event looks like textually
  slashPoint=eventString.find('/');
  targetAfterSlash=eventString.substr(slashPoint+1,string::npos);
  
  // fill the other occurance list with other occurances of the same event
  for (i=0; i<nevents;i++)
    {
      if (events[i]->signal == event->signal)
	{
	  occuranceString=events[i]->event;  // put the c_str into a string
	  slashPoint=occuranceString.find('/');
	  afterSlash=occuranceString.substr(slashPoint+1,string::npos);
	  if(afterSlash != targetAfterSlash)
	    {
	      otherOccuranceList.push_back(i);
	      // cout << "Adding " << events[i]->event 
	      // << " to others list" << endl;
	    }
	}
    }
  found=false;
  for (i=0; i < PRIME; i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      { 
	ER_inclusion_known=false;
	foundChar=(curstate->state)[event->signal];
	if (foundChar==targetChar) // signal enabling correct in this state
	  {
	    // seed the initial list with the cur state
	    thisGenerationList.push_back(curstate);
	    while (!ER_inclusion_known)
	      {
		// see if any state at this generation decides it
		while (!thisGenerationList.empty() && !ER_inclusion_known)
		  {
		    // take a state from this generation
		    thisState=thisGenerationList.back();
		    thisGenerationList.pop_back();
		    // look at each of this state's succs
		    for (nextNode=thisState->succ;
			 nextNode != NULL;
			 nextNode=nextNode->next)
		      {
			if (nextNode->enabled==eventIndex)
			  // if the correct event brought us here
			  { // curstate IS in the ER
			    *bii_ER++=curstate;
			    ER_inclusion_known=true;
			    found=true;
			  }
			// if enter by a different event on same signal
			else if (events[nextNode->enabled]->signal==
				 event->signal)
			  /*
			    else if (find(otherOccuranceList.begin(),
			    otherOccuranceList.end(),
			    nextNode->enabled)
			    != otherOccuranceList.end())*/ 
			  { // curstate is NOT in the ER
			    ER_inclusion_known=true;
			    //found=false;
			  }
		      } // end for
		    // this state's arcs aren't conclusive,
		    //  so add its succs, just in case no other state
		    // in this generation is conclusive
		    if (!ER_inclusion_known)
		      {
			for (nextNode=thisState->succ;
			     nextNode != NULL;
			     nextNode=nextNode->next)
			  {
			    nextGenerationList.push_back(nextNode->stateptr);
			  }
		      }
		  } // end while for a single generation
		// inconclusive, so shift down a generation
		thisGenerationList=nextGenerationList;
		nextGenerationList.erase(nextGenerationList.begin(),
					 nextGenerationList.end());
	      } // end while which goes until we know one way or the other
	    
	  } // end if for only promising candidate states
      } // end for each state in graph
  
  return found;
}

/*****************************************************************************/
/*   Fill a container of states representing switching region SR(event)      */
/*****************************************************************************/
#ifndef __TEMPLATE_PATCH
template<class back_insert_iter>
#endif
bool findSR(stateADT state_table[], eventADT event, int eventIndex,
	    back_insert_iter bii_SR)
{
  // check each possible state
  // if the value of the signal is correct (1,0)
  // look through the pred list for a transition with event on it
  // if found, add this to SR

  int i;
  stateADT curstate;
  char foundChar;
  char targetValChar;
  char targetEnablingChar;
  
  string eventString(event->event);
  statelistADT predNode;
  bool done;

  // if this SR is for a rising transition, look for R to 1/F transitions
  if (eventString.find("+") != string::npos)
    {
      targetValChar = '1'; // R to 1
      targetEnablingChar = 'F'; // R to F
    }
  // if this SR is for a falling transition look for F to 0/R transitions
  else
    {
      targetValChar = '0'; // F to 0
      targetEnablingChar = 'R'; // F to R
    }

  for (i=0;i<PRIME;i++) // for every state
    for (curstate = state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate = curstate->link)
      { 
	foundChar=(curstate->state)[event->signal];
	if (foundChar == targetValChar || foundChar == targetEnablingChar)
	  // if correct post transitional value 
	  {
	    // look at each pred and see if it connects to this state
	    // through the targetEvent
	    for (predNode=curstate->pred, done=false;
		 (predNode != NULL) && !done;
		 predNode=predNode->next)
	      {
		if (predNode->enabled==eventIndex)
		  // if the correct event brought us here
		  {
		    *bii_SR++ = curstate; // add the state to the SR
		    done = true;
		  }
	      }
	  }
      }

  return true;
}

/*****************************************************************************/
/*   search the state graph for the initial state, return converted color    */
/*****************************************************************************/
char svInitialValue(stateADT state_table[])
{
  string symbols("01RFBIA");
  for (int i=0;i<PRIME;i++)
    for (stateADT curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      { 
	if (curstate->number == 0)
	  {
	    switch(curstate->color)
	      {
	      case COLOR_R:
	      case COLOR_0:
		return '0';
		break;
		
	      case COLOR_F:
	      case COLOR_1:
		return '1';
		break;
		
	      default:
		return symbols[curstate->color];
		break;
	      }
	  }
      }
  // if X is returned, there is a problem with the coloring algorithm
  return '?'; 
}

/*****************************************************************************/
/* search the state graph to determine if outgoing arcs are marked initially */
/*****************************************************************************/
char svInitialMarking(cycleADT**** cycles, int ncycles, int nevents,
		      const event_index_cont& eventsBeforeTransition,
		      int endEventIndex)
{
  // the current policy for initial marking is:
  // if any element of the SR intersection can be reached from ER element
  // without passing through a token, then a token will naturally arrive
  // here as part of the initial system. If no element of the SR intersection
  // can be reached without going through a token, the system will deadlock without
  // an initial token being placed on the outgoing rule as well

  // ... see if any SR event is reachable w/o traversing a token
  for (event_index_cont::const_iterator i_preceedingEvent =
	 eventsBeforeTransition.begin();
       i_preceedingEvent != eventsBeforeTransition.end();
       i_preceedingEvent++)
    {
      // check to see if any SR event is reachable
      if (reachable(cycles, nevents, ncycles,
		    *i_preceedingEvent, 0, // 0 is epsilon (unmarked)
		    endEventIndex, 0) == 1)
	{
	  return '0'; // if so, a token will get here naturally,
	              // so don't place one here
	}
    }
  return '1'; 
}

/*****************************************************************************/
/*   color the entire state graph with a blank color                         */
/*****************************************************************************/
void clearColors(stateADT state_table[], int color)
{
  stateADT curstate;

  for (int i=0; i < PRIME; i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      { 
	curstate->color=color;
      }
}
/*****************************************************************************/
/*   color the states of a rising transition with R                          */
/*****************************************************************************/
bool colorRise(state_list& startStates, const state_list& endStates,
	       const event_index_cont& startEvents, const event_index_cont& endEvents,
	       event_index_cont& riseWitnesses,
	       ruleADT** rules, cycleADT**** cycles, int nevents, int ncycles, int cscUpperBound,
	       bool systemIsTimed)
{
  queue<stateADT> nextStateQueue;
  stateADT thisState;

//   // first color the endStates COLOR_END_FALL, so we'll know to stop when we reach them
//   for (state_list::const_iterator i_endState=endStates.begin();
//        i_endState != endStates.end();
//        i_endState++)
//     {
//       // this line should be unnecessary, but...
//       if ((*i_endState)->color == COLOR_F
// 	  ||(*i_endState)->color == COLOR_1)
// 	{
// 	  cout << "colorRise: graph not cleared for takeoff" << endl;
// 	  return false;
// 	}
//       (*i_endState)->color = COLOR_END_RISE;
//       //(*i_endState)->color = COLOR_R;
//     }
  
  // prime the queue with the startStates
  for (state_list::iterator i_startState = startStates.begin();
       i_startState != startStates.end();
       i_startState++)
    {
      nextStateQueue.push(*i_startState);
    }

  // now empty the startStates, as the startStates currently there will
  // be added as they are pulled from the queue
  startStates.erase(startStates.begin(), startStates.end());
  
  // now sweep from the start of the rise region to the end of the rise region
  while ( ! nextStateQueue.empty() )
    {
      // take the next state out of the nextStateQueue
      thisState = nextStateQueue.front();
      nextStateQueue.pop();
	  // process this state
      switch( thisState->color )
	{
	case COLOR_1:
	case COLOR_F:
	  // this error shouldn't occur, since colorRise is the first of the coloring
	  // subfunctions to be called
	  //cout << "Oddly, colorRise hits F/1 color in state " << thisState->state << endl;
	  return false;
	  break; // this break should never be reached
	  
	case COLOR_R:
	  // if state is already colored, just continue
	  // this assumes that whoever colored it in the first
	  // place will take care of its successors
	  break;
	  
// 	case COLOR_END_RISE:
	  // 	  // this is the last state that should be colored on this branch
// 	  thisState->color = COLOR_R;
// 	  startStates.push_back(thisState);
// 	  // don't queue the successors, UNLESS THE SUCCESSORS ARE ALSO IN END_RISE
// 	  for (statelistADT nextState=(thisState)->succ;
// 	       nextState != NULL;
// 	       nextState = nextState->next)
// 	    {
// 	      if (nextState->stateptr->color == COLOR_END_RISE)
// 		{
// 		  nextStateQueue.push(nextState->stateptr);
// 		}
// 	    }
// 	  break;
	  
	  //	case COLOR_0: // should never be this
	case COLOR_BLANK:
	  //	case COLOR_END_FALL: // should never be this
	  // color this state
	  thisState->color = COLOR_R;
	  // add this state to the list, for use as a starting seed for colorHigh()
	  startStates.push_back(thisState);
			  
	  // now consider adding its successor states to the queue
	  for (statelistADT nextState=(thisState)->succ;
	       nextState != NULL;
	       nextState = nextState->next)
	    {
	      // if we can determine that the state signal has switched before
	      // the next state, don't queue it
	      // this is determined if:
	      // for all u in ss, there exists a path (u,w)
	      // where w is the event which takes us to nextState
	      // if the above is true, then we need to consider
	      // for all u element of ss, is worst-case time diff between
	      // u and w such that the lower bound is greater than the upper bound
	      // on CSC (usually maxgatedelay)? If this is true, the CSC signal
	      // will always occur before this next state, so don't queue it

	      if (systemIsTimed)
		{
		  // assume this next state is reached through a witness transition
		  bool canExcludeNextState = true;
		  // check that we have paths from startEvents to possible witness events
		  for(event_index_cont::const_iterator i_ss = startEvents.begin();
		      i_ss != startEvents.end() && canExcludeNextState; i_ss++)
		    {
		      if (reachable(cycles, nevents, ncycles, *i_ss, 0,
				    nextState->enabled, 0) == notreachable)
			{
			  // we don't have enough paths, so can't use witness events
			  canExcludeNextState = false;
			}
		    }
		  // paths exist, but is the timing right ?
		  for(event_index_cont::const_iterator i_ss = startEvents.begin();
		      i_ss != startEvents.end() && canExcludeNextState; i_ss++)
		    {
		      boundADT timeDiff=
			WCTimeDiff(rules, cycles, nevents, ncycles,
				   *i_ss, nextState->enabled, 0);
		      if (timeDiff->L <= cscUpperBound)
			{
			  // timing says this isn't a witness transition
			  canExcludeNextState = false;
			}
		      free(timeDiff);
		    }
		  // if we can't exclude the next state, better check for explicit ex'ing
		  if (! canExcludeNextState)
		    {
		      // if this transition is not in endEvents, queue up the next state
		      if (find(endEvents.begin(), endEvents.end(), nextState->enabled) == endEvents.end()
			  || endEvents.empty() )
			{
			  nextStateQueue.push(nextState->stateptr);
			}
		    }
		  else // witness transitions come through for us
		    {
		      // keep track of this event, so we can mark as though this arc were really there
		      riseWitnesses.push_back(nextState -> enabled);
		    }
		} // end timed if
	      else // untimed
		{
		  // if this transition is not in endEvents, queue up the next state
		  if (find(endEvents.begin(), endEvents.end(), nextState->enabled) == endEvents.end()
		      || endEvents.empty())
		    {
		      nextStateQueue.push(nextState->stateptr);
		    }
		}
	    } // end for each successor
	  break;
	  
	default:
	  //cout << "findsv: Bogus color detected by colorRise in " << thisState->state << endl;
	  //cout << "Color is " << thisState->color << endl;
	  break;
	}
    } // end while 

  return true;
}


/*****************************************************************************/
/*   color states in the falling transition with F                           */
/*****************************************************************************/
bool colorFall(state_list& startStates, const state_list& endStates,
	       const event_index_cont& startEvents, const event_index_cont& endEvents,
	       event_index_cont& fallWitnesses,
	       ruleADT** rules,
	       cycleADT**** cycles, int nevents, int ncycles, int cscUpperBound,
	       bool systemIsTimed)
{
  queue<stateADT> nextStateQueue;
  stateADT thisState;

//  // first color the endStates COLOR_END_FALL, so we'll know to stop when we reach them
//   for (state_list::const_iterator i_endState=endStates.begin();
//        i_endState != endStates.end();
//        i_endState++)
//     {
//       if ((*i_endState)->color == COLOR_R 
// 	  || (*i_endState)->color == COLOR_0)
// 	{
// 	  return false;
// 	}
//       (*i_endState)->color = COLOR_END_FALL;
//       //(*i_endState)->color = COLOR_F;
//     }
  
  // prime the queue with the startStates
  for (state_list::iterator i_startState=startStates.begin();
       i_startState != startStates.end();
       i_startState++)
    {
      nextStateQueue.push(*i_startState);
    }
 
  // now empty the startStates, as the startStates currently there will
  // be added as they are pulled from the queue
  startStates.erase(startStates.begin(), startStates.end());
       
  // now move through the queue until it is empty
  while ( ! nextStateQueue.empty() )
    {
      // take the next queue entry
      thisState = nextStateQueue.front();
      nextStateQueue.pop();

      switch( thisState->color)
	{
	case COLOR_0:
	case COLOR_R:
	  // this coloring is inconsistent
	  //	  cout << "ColorFall hits R/0 color in state " << thisState->state << endl;
	  return false;
	  break; // this break should never be reached
	      
	case COLOR_F:
	  // if state is already colored, just continue
	  // this assumes that whoever colored it in the first
	  // place will take care of its successors
	  break;
	  
// 	case COLOR_END_FALL:
// 	  // this is the last state that should be colored on this branch
// 	  thisState->color = COLOR_F;
// 	  startStates.push_back(thisState);
// 	  // don't queue the successors, UNLESS THE SUCCESSORS ARE ALSO IN END_FALL
// 	  for (statelistADT nextState=(thisState)->succ;
// 	       nextState != NULL;
// 	       nextState = nextState->next)
// 	    {
// 	      if (nextState->stateptr->color == COLOR_END_FALL)
// 		{
// 		  nextStateQueue.push(nextState->stateptr);
// 		}
// 	    }
// 	  break;

	case COLOR_1:
	case COLOR_BLANK:
	  //	case COLOR_END_RISE:
	  (thisState)->color = COLOR_F;
	  startStates.push_back(thisState);
	  // now consider adding successors 
	  for (statelistADT nextState = thisState->succ;
	       nextState != NULL;
	       nextState=nextState->next)
	    {
	      // if we can determine that the state signal has switched before
	      // the next state, don't queue it
	      // this is determined if:
	      // for all u in ss, there exists a rule (u,w)
	      // where w is the event which takes us to nextState
	      // if the above is true, then we need to consider
	      // for all u element of ss, is worst-case time diff between
	      // u and w such that the lower bound is greater than the upper bound
	      // on CSC (usually maxgatedelay)? If this is true, the CSC signal
	      // will always occur before this next state, so don't queue it
	      if (systemIsTimed)
		{
		  bool canExcludeNextState = true;
		  for(event_index_cont::const_iterator i_ss = startEvents.begin();
		      i_ss != startEvents.end() && canExcludeNextState; i_ss++)
		    {
		      if (reachable(cycles, nevents, ncycles,
				    *i_ss, 0,
				    nextState->enabled, 0) == notreachable)
			{
			  // can't tell if this is a witness transition,
			  // so can't exclude next state
			  canExcludeNextState = false;
			}
		    }
		  for(event_index_cont::const_iterator i_ss = startEvents.begin();
		      i_ss != startEvents.end() && canExcludeNextState; i_ss++)
		    {
		      boundADT timeDiff=
			WCTimeDiff(rules, cycles, nevents, ncycles,
			       *i_ss, nextState->enabled, 0);
		      if (timeDiff->L <= cscUpperBound)
			{
			  // can see that this is NOT a witness transition,
			  // so don't exclude next state
			  canExcludeNextState = false;
			}
		      free(timeDiff);
		    }
		  if (! canExcludeNextState) // if not ex'd by a witness, is it ex'd by an explicit?
		    {
		      if (find(endEvents.begin(), endEvents.end(), nextState->enabled) == endEvents.end()
			  || endEvents.empty())
			{
			  nextStateQueue.push(nextState->stateptr);
			}
		    }
		  else // witness transitions come through for us
		    {
		      // keep track of this event, so we can mark as though this arc were really there
		      fallWitnesses.push_back(nextState -> enabled);
		    }
		}
	      else // untimed
		{
		  // if this transition is not in endEvents, queue up the next state
		  if (find(endEvents.begin(), endEvents.end(), nextState->enabled) == endEvents.end()
		      || endEvents.empty())
		    {
		      nextStateQueue.push(nextState->stateptr);
		    }
		} // end else
	    } // end for
	  break;

	default:
	  //cout << "findsv: bogus color detected by colorFall in " << thisState->state << endl;
	  //cout << "Color is " << thisState->color << endl;
	  break;
	}
    } // end while 

  return true;
}

/*****************************************************************************/
/*   color with 1, returning false if inconsistent                           */
/*****************************************************************************/
bool colorHigh(state_list& riseRegion)
{
  state_list thisList;
  state_list nextList;

  // this assumes that the riseRegion has already been painted with R
  // and fallRegion with F

  // prime the list with the rise region's next states
  for (state_list::iterator i_state=riseRegion.begin();
       i_state != riseRegion.end();
       i_state++)
    {
      for (statelistADT nextState=(*i_state)->succ;
	   nextState != NULL;
	   nextState=nextState->next)
	{
	  // if a state becomes enabled and stays enabled for
	  // several states, the coloring algo will think it is
	  // inconsistent. By removing seed states which are the direct
	  // successors of other seed states, this problem is avoided
	  if (nextState->stateptr->color != COLOR_R)
	    thisList.push_back(nextState->stateptr);
	}
    }
      
  while (! thisList.empty())
    {
      // for all states in the list
      for (state_list::iterator i_state=thisList.begin();
	   i_state != thisList.end();
	   i_state++)
	{
	  switch( (*i_state)->color)
	    {
	    case COLOR_1:
	    case COLOR_F:
	      // if state is already colored, just continue
	      // this assumes that whoever colored it in the first
	      // place will take care of its successors
	      break;
	      
	    case COLOR_0:
	    case COLOR_R:
	      // this coloring is inconsistent
// 	      cout << "colorHigh hits color " << (*i_state)->color
// 		    << " in state " << (*i_state)->state << endl;
	      return false;
	      break; // this break should never be reached

	    case COLOR_BLANK:
//	    case COLOR_END_RISE:
//	    case COLOR_END_FALL:
	      (*i_state)->color=COLOR_1;
	      for (statelistADT nextState=(*i_state)->succ;
		   nextState != NULL;
		   nextState=nextState->next)
		{
		  nextList.push_back(nextState->stateptr);
		}
	      break;

	    default:
	      cout << "ColorHigh has found unexpected color " << (*i_state)->color;
	      break;
	    }
	} // end for

      thisList=nextList;
      nextList.erase(nextList.begin(), nextList.end());
    } // end while 

  return true;
}


/*****************************************************************************/
/*   color with 0, returning false if inconsistent                           */
/*****************************************************************************/
bool colorLow(state_list& fallRegion)
{
  state_list thisList;
  state_list nextList;

  // this assumes that the fallRegion has already been painted with R
  // and fallRegion with F -- 2 calls to colorRegion

  // prime the list with the rise region's next states
  for (state_list::iterator i_state=fallRegion.begin();
       i_state != fallRegion.end();
       i_state++)
    {
      for (statelistADT nextState=(*i_state)->succ;
	   nextState != NULL;
	   nextState=nextState->next)
	{
	  // if a state becomes enabled and stays enabled for
	  // several states, the coloring algo will think it is
	  // inconsistent. By removing seed states which are the direct
	  // successors of other seed states, this problem is avoided
	  if (nextState->stateptr->color != COLOR_F)
	      thisList.push_back(nextState->stateptr);
	}
    }
      
  while (! thisList.empty())
    {
      // for all states in the list
      for (state_list::iterator i_state=thisList.begin();
	   i_state != thisList.end();
	   i_state++)
	{
	  switch( (*i_state)->color)
	    {
	    case COLOR_0:
	    case COLOR_R:
	      // if state is already colored, just continue
	      // this assumes that whoever colored it in the first
	      // place will take care of its successors
	      break;
	      
	    case COLOR_1:
	    case COLOR_F:
	      // this coloring is inconsistent
// 	      cout << "colorLow hits color " << (*i_state)->color
// 		   << " in state " << (*i_state)->state << endl;
	      return false;
	      break; // this break should never be reached

	    case COLOR_BLANK:
// 	    case COLOR_END_RISE:
// 	    case COLOR_END_FALL:
	      (*i_state)->color=COLOR_0;
	      for (statelistADT nextState=(*i_state)->succ;
		   nextState != NULL;
		   nextState=nextState->next)
		{
		  nextList.push_back(nextState->stateptr);
		}
	      break;

	    default:
	      cout << "ColorLow has found unexpected color " << (*i_state)->color;
	      break;
	    }
	} // end for

      thisList=nextList;
      nextList.erase(nextList.begin(), nextList.end());
    } // end while 

  return true;
}

///////////////////////////////////////////////////////////////////////////
// color the whole graph
///////////////////////////////////////////////////////////////////////////

int funct(stateADT root) {
	       
  int i = 0,colors[100],flag0 = 0,flag1 = 0,flagF = 0,flagR = 0;

  struct state_list_tag* p = root->succ;
		
  
  while(p != NULL) {
    //cout << (p->stateptr)->state <<  "  " << (p->stateptr)->color << endl ;
    
    if(p->stateptr->color == COLOR_BLANK) {
      p->stateptr->color = colors[i++] = funct(p->stateptr);
      p = p->next;
    }
    else 
      return p->stateptr->color; 
    
  }
		  
  for(--i; i >=0 ;--i) {
    if(colors[i] == 0) 
      flag0 = 1;
    else if(colors[i] == 1) 
      flag1 = 1;
    else if(colors[i] == COLOR_R)
      flagR = 1;
    else if(colors[i] == COLOR_F)
      flagF = 1;
    
    if((flag0 == 1 && flagF == 1) || (flagR == 1 && flag1 == 1) || (flag0 == 1 && flag1 == 1) || (flagF == 1 && flagR == 1))
       break;
  }
		  
    if(i >= 0)
      return -1;
    else {
      if(flag1 == 1 || flagF == 1)
	return 1;
      else if(flag0 == 1 || flagR == 1 )
	return 0;
    }
    //cout << "YIKES" << endl;
    return 0;
}



bool colorStateGraph(stateADT state_table[],
		     state_list& riseStart, state_list& riseEnd,
		     const event_index_cont& riseStartEvents,
		     const event_index_cont& riseEndEvents,
		     event_index_cont& riseWitnesses,
		     state_list& fallStart, state_list& fallEnd,
		     const event_index_cont& fallStartEvents,
		     const event_index_cont& fallEndEvents,
		     event_index_cont& fallWitnesses,
		     ruleADT** rules, cycleADT**** cycles,
		     int nevents, int ncycles, int cscUpperBound,
		     bool systemIsTimed)
{
  bool isConsistent = true;
  // note that this function alters the contents of riseStart and fallStart
  
/*    if(riseStartEvents.size() == 1 && riseStartEvents[0] == 7  && riseEndEvents.size() == 0 && fallStartEvents.size() == 1 && fallStartEvents[1] == 8  && fallEndEvents.size() == 0) { */
/*      cout << "This is the insertion point\n"; */
/*      cout << riseStartEvents[0] << endl; */
/*    } */

  // make sure that the intersections of the SR's are not empty
  if (riseStart.empty() || fallStart.empty()) return false;
  // clear out the witnesses for this new attempt
  riseWitnesses.erase(riseWitnesses.begin(), riseWitnesses.end());
  fallWitnesses.erase(fallWitnesses.begin(), fallWitnesses.end());

  clearColors(state_table);

  isConsistent = colorRise(riseStart, riseEnd,
			   riseStartEvents,riseEndEvents,
			   riseWitnesses,
			   rules, cycles, nevents, ncycles, cscUpperBound, 
			   systemIsTimed);

  isConsistent = isConsistent && colorFall(fallStart, fallEnd,
					   fallStartEvents,fallEndEvents,
					   fallWitnesses,rules, cycles, 
					   nevents, ncycles, cscUpperBound, 
					   systemIsTimed);

  if (!isConsistent) return false;

  isConsistent = isConsistent && colorHigh(riseStart);

  if (!isConsistent) return false;

  isConsistent = isConsistent && colorLow(fallStart);

    if (isConsistent)
    {
      for (int i=0; i < PRIME; i++)
  	for (stateADT curstate = state_table[i];
  	     curstate != NULL && curstate->state != NULL;
  	     curstate=curstate->link)
  	  {

	    
	    
  	    //struct state_list_tag* p = curstate->succ; 
	    
	    

  	    if (curstate->color == COLOR_BLANK)
	    {
	      //cout << curstate->state << " not colored" << endl;
		
	      int retval;

	      if(curstate->pred == NULL) {
		// So the uncolored state is a start state.
		
/*  		  cout << "Successors \n"; */
/*  		  p = curstate->succ; */

/*  		  while(p != NULL) { */
/*  		    cout << (p->stateptr)->state <<  "  " << endl ; */
/*  		    p = p->next; */
/*  		  } */


		  if((retval = funct(curstate)) != -1) {
		    curstate->color = retval;
		    //	    cout << "Color of " << curstate->state << " is " 
		    //	 << curstate->color << endl; 
		  } else { 
		    isConsistent = false;
		    //cout << "Color of " << curstate->state 
		    //	 << " is inconsistent" << endl; 
		  }
	      }
	    }
	  }
 //       cout << riseStartEvents.size() << " " << riseEndEvents.size() << endl;
   //     cout << fallStartEvents.size() << " " << fallEndEvents.size() << endl;
     //   cout << endl << endl;
     }
  
//    if (isConsistent)
//      {
//        for (int i=0; i < PRIME; i++)
//   	for (stateADT curstate = state_table[i];
//   	     curstate != NULL && curstate->state != NULL;
//   	     curstate=curstate->link)
//   	  {
//   	    if (curstate->color == COLOR_BLANK)
   //   	      {
   //   		cout << curstate->state << " not colored" << endl;
   //   	      }
   //   	  }
   //         cout << riseStartEvents.size() << " " << riseEndEvents.size() << endl;
   //         cout << fallStartEvents.size() << " " << fallEndEvents.size() << endl;
   //         cout << endl << endl;
   //      }

   return isConsistent;
}

 

///////////////////////////////////////////////////////////////////////////
string stateVal(string& enabling)
{
  string val(enabling);

  for (unsigned int i=0; i < enabling.length(); i++)
    {
      switch(enabling[i])
	{
	case 'R':
	  val[i]='0';
	  break;

	case 'F':
	  val[i]='1';
	  break;
	}
    }
  return val;
}
///////////////////////////////////////////////////////////////////////////
int colorVal(int color)
{
  switch(color)
    {
    case COLOR_0:
    case COLOR_R:
      return COLOR_0;
      break;

    case COLOR_1:
    case COLOR_F:
      return COLOR_1;
      break;
    }
  return COLOR_BLANK;
}

///////////////////////////////////////////////////////////////////////////
int countCSCViolations(stateADT state_table[],int ninpsig,
			int& natural, int& caused)
{
  stateADT someState;
  stateADT curState;
  int i;

  // init the counters which are passed into the function
  natural=0;
  caused=0;

  for (i=0; i < PRIME; i++) // for each hash bin
    {
      // for each state in the hash bin
      for (curState=state_table[i];
	   curState != NULL && curState->state != NULL;
	   curState=curState->link)
	{
	  // construct a string for this state code
	  string curString(curState->state);

	  for (someState=curState->link;
	       someState != NULL && someState->state != NULL;
	       someState=someState->link)
	    {
	      // make strings for someState's state code
	      string someString(someState->state);

	      // if curstate's state code matches somestate's state code
	      if (stateVal(curString)==stateVal(someString))
		{
		  // check output enablings for disparity
		  // CSC ?
		  if (curString.substr(ninpsig, string::npos)
		      != someString.substr(ninpsig, string::npos))
		    // violation found
		    {
		      // check colorings for possible save
		      if ( !(
			     (someState->color==COLOR_1
			      && curState->color==COLOR_0)
			     ||
			     (someState->color==COLOR_0
			      && curState->color==COLOR_1)
			     ))
			{
			  // no save, so CSC violation unresolved
// 			  cout << "CSC: " << curString << " " <<
// 			    someString << endl;
			  natural++;
			}
		      if ( !(
			     (someState->color==COLOR_1
			      || curState->color==COLOR_0)
			     ||
			     (someState->color==COLOR_0
			      || curState->color==COLOR_1)
			     ))
			{
			  // no save, so CSC violation unresolved
// 			  cout << "CSC: " << curString << " " <<
// 			    someString << endl;
			  natural++;
			}
// 		      else cout << "Coloring Saved: "
// 				<< someString << " "
// 				<< curString << endl;
		    }
		  // else if output enablings did match, check USC->CSC
		  else // USC ?
		    {
		      if ( (colorVal(curState->color)
			   == colorVal(someState->color)
			   && curState->color != someState->color)
			  ||
			  (colorVal(curState->color) != curState->color
			   &&
			   colorVal(someState->color) != someState->color)
			   &&
			   curState->color != someState->color
			  )
			{
//  			  cout << "USC-> CSC: " << curString << " " <<
//  			    someString << endl;
// 			  cout << curState->color << " "
// 			       << someState->color << endl;
			  caused++;
			}
// 		      else
// 			cout << "benign USC Detected: "
// 			     << someString << " " << curString << endl;
		    } // end if USC
		} // end if potential problem
	    } // end for other states in bin
	} // end for each state in bin
    } // end for each bin

  /* TOMOHIRO
  caused=0; */

  return natural + caused;

} // end function


/****************************************************************************/
/* Find the ER and SR for all events                                        */
/* This function uses two different types of back insert iterators,         */
/* one is for containers of states, the other is for containers of          */
/* containers of states. Thus, findAllRegions finds a region in tempRegion  */
/* then adds that temp container to the container of all ers or the         */
/* container of all srs.                                                    */
/****************************************************************************/
template<class back_insert_iter>
bool findAllRegions(stateADT state_table[], int nevents, eventADT events[],
		    back_insert_iter bii_er, back_insert_iter bii_sr)
{
  bool result=true;
  state_list tempRegion;
  back_insert_iterator<state_list> bii_tempRegion(tempRegion);
  int eventIndex;
  
  for (eventIndex=0; eventIndex < nevents; eventIndex++)
    {
      result = result & findER(state_table, events, nevents, eventIndex,
			       bii_tempRegion);
      *bii_er++=tempRegion;
      tempRegion.erase(tempRegion.begin(),tempRegion.end());

      result = result &	findSR(state_table, events[eventIndex], eventIndex,
			       bii_tempRegion);
      *bii_sr++=tempRegion;
      tempRegion.erase(tempRegion.begin(),tempRegion.end());
    }
  
  return result;
}

/****************************************************************************/
/*   Intersect the specified regions and store their states                 */
/****************************************************************************/
state_list intersectRegions(event_index_cont& events,
			    state_list_cont& regionStates)
{
  state_list intersection;
  state_list partialIntersection;
  back_insert_iterator<state_list> bii_partIntersection(partialIntersection);

  // seed the intersection with the states in the first region
  intersection = regionStates[*(events.begin())];

  // intersect this with the states for each of the other regions
  if (events.size() > 1)
    for (event_index_cont::iterator i_event=events.begin()+1;
	 i_event != events.end(); i_event++)
      {
	set_intersection(intersection.begin(), intersection.end(),
			 regionStates[*i_event].begin(),
			 regionStates[*i_event].end(),
			 bii_partIntersection);
	
	intersection = partialIntersection;
	partialIntersection.erase(partialIntersection.begin(),
				  partialIntersection.end());
      }

  return intersection;
}

//***************************************************************************
// create a cstringolding an insertion point using event labels
//***************************************************************************
string insertionString(eventADT events[], const CInsertionPoint& IP)
{
  string text;

  //      cout << solution << endl; // stream the solutions down the screen for now
  for (event_index_cont::const_iterator i_rs = IP.riseStart.begin();
       i_rs != IP.riseStart.end(); i_rs++)
    {
      text += events[*i_rs]->event;
      text += " ";
    }
  text += "<-> ";
  for (event_index_cont::const_iterator i_re = IP.riseEnd.begin();
       i_re != IP.riseEnd.end(); i_re++)
    {
      text += events[*i_re]->event;
      text += " ";
    }
  text += " ===== ";
  for (event_index_cont::const_iterator i_fs = IP.fallStart.begin();
       i_fs != IP.fallStart.end(); i_fs++)
    {
      text += events[*i_fs]->event;
      text += " ";
    }
  text += "<-> ";
  for (event_index_cont::const_iterator i_fe = IP.fallEnd.begin();
       i_fe != IP.fallEnd.end(); i_fe++)
    {
      text += events[*i_fe]->event;
      text += " ";
    }

  return text;
}    



/****************************************************************************/
/*   Create the name string for the next valid CSC variable name            */
/****************************************************************************/
string nextCSCVariableName(int& variableNumber, signalADT signals[], int nsignals)
  {
    bool nameConflict;
    string signalName;
    do
      {
	nameConflict = false;
	signalName = "CSC";
	// fill in the signal name
	char intAsString[5];
	sprintf(intAsString,"%d",variableNumber);
	signalName += intAsString; 
	for (int s = 0; s < nsignals && !nameConflict; s++)
	  {
	    string existingSignalName(signals[s]->name);
	    if (signalName == existingSignalName)
	      {
		nameConflict = true;
		variableNumber++;
	      }
	  }
      } while (nameConflict);

    // advance the variableNumber so it is correct for the next iteration
    variableNumber++;

    return signalName;
  }

///////////////////////////////////////////////////////////////////////////
int find_good_insertion_points(stateADT state_table[],
			       int ninputsigs,
			       eventADT events[],
			       int nevents,
			       ruleADT* rules[],
			       cycleADT**** cycles,
			       int ncycles,
			       int cscUpperBound,
			       bool systemIsTimed,
			       int goodSetSize, // keep n best solutions
			       CBestSolutions& bestSolutions,bool minins)
{
  state_list riseStartStates;
  state_list riseEndStates;
  state_list fallStartStates;
  state_list fallEndStates;

  CInsertionPoint vip; // some valid insertion point
      
  // the CIPGenerator spits out all acceptable sols
  CIPGenerator ipGenerator(nevents, ninputsigs, events, state_table, rules);

  //for(int i = 0; i <= 4; ++i)
  //  cout << i << ": " << events[i]->event << endl;

  // find states in the er and sr of every event
  // these states will then be used for coloring the state graph
  state_list_cont erStates; // erStates[event#]=list of ptrs to states in ER
  state_list_cont srStates; // srStates[event#]=list of ptrs to states in SR
  back_insert_iterator<state_list_cont> bii_erStates(erStates);
  back_insert_iterator<state_list_cont> bii_srStates(srStates);
  findAllRegions(state_table, nevents, events, bii_erStates, bii_srStates);

    
  // sort the states in the region lists -- critical for set_intersection
  for (state_list_cont::iterator i_regionStates = srStates.begin();
       i_regionStates != srStates.end(); i_regionStates++)
    {
      (*i_regionStates).sort();
    }

    for(int j = 0;j < nevents; ++j) { 
      list<stateADT> i_regionStates = erStates[j]; 
    } 

    for(int j = 0;j < nevents; ++j) { 
      list<stateADT> i_regionStates = srStates[j]; 
    } 
  

  //   for (state_list_cont::iterator i_regionStates = erStates.begin();
  //        i_regionStates != erStates.end(); i_regionStates++)
  //     {
  //       (*i_regionStates).sort();
  //     }

  // do initial priming of the ipGenerator

    //  for(deque<CTreeNode>::iterator ii = ipGenerator.riseQueue.begin(); ii != ipGenerator.riseQueue.end(); ++ii)
    // (*ii).

    if (minins)
      ipGenerator.nextValidTransitionPoint2(true,vip.riseStart, vip.riseEnd);
    else
      ipGenerator.nextValidTransitionPoint(true,vip.riseStart, vip.riseEnd);
  
  // for(int i = 0;i < vip.riseStart.size(); ++i)
  //   cout << vip.riseStart[i] << "  " ;

  // while the generator can produce another insertion point, keep going
  //int k = 0; 
  while ( ipGenerator.nextValidInsertionPoint(vip) )
    {
  
      // create lists of the states in the intersection of the SR's and ER's
      // if this is a hanging state, the ER set will be empty
      riseStartStates = intersectRegions(vip.riseStart, srStates);
      
      //       if (vip.riseEnd.size() > 0)
      // 	{
      // 	  riseEndStates = unionRegions(vip.riseEnd, erStates);
      // 	}
      //       else
      // 	{
      // 	  riseEndStates.erase(riseEndStates.begin(), riseEndStates.end());
      // 	}
      fallStartStates = intersectRegions(vip.fallStart, srStates);
      //       if (vip.fallEnd.size() > 0)
// 	{
// 	  fallEndStates = unionRegions(vip.fallEnd, erStates);
// 	}
//       else
// 	{
// 	  riseEndStates.erase(fallEndStates.begin(), fallEndStates.end());
// 	}
//       cout << "TRYING: RS=";
//       for(int i = 0;i < vip.riseStart.size(); ++i)
// 	cout << events[vip.riseStart[i]]->event << "  " ;
//       cout << " RE=";
//       for(int i = 0;i < vip.riseEnd.size(); ++i)
// 	cout << events[vip.riseEnd[i]]->event << "  " ;
//       cout << " FS=";
//       for(int i = 0;i < vip.fallStart.size(); ++i)
// 	cout << events[vip.fallStart[i]]->event << "  " ;
//       cout << " FE=";
//       for(int i = 0;i < vip.fallEnd.size(); ++i)
// 	cout << events[vip.fallEnd[i]]->event << "  " ;
      
      // color the graph

     if (
	 colorStateGraph(state_table,riseStartStates, riseEndStates, 
			 vip.riseStart, vip.riseEnd, vip.riseWitnesses,
			 fallStartStates, fallEndStates, vip.fallStart, 
			 vip.fallEnd, vip.fallWitnesses,
			 rules, cycles, nevents, ncycles, cscUpperBound,
			 systemIsTimed)
	 )
       {
	 //cout << "Color is okay" << endl;
	 
	  // set the initial value for this insertion point
	  
	  //  cout << "The value of k is " << k << endl;
	  vip.initialValue = svInitialValue(state_table);
	  // if however, we initially mark any outgoing rules, then we need to 
	  // set the initialValue to the value it would be set to by the transition
	  // from which the arc leaves (done in CStateVarInfo constructor)
	  // determine the number of violations
	  
	  countCSCViolations(state_table, ninputsigs, vip.naturalViolations, vip.causedViolations);
	  
	  // consider this solution as a contender for the best solutions
// 	  cout << " Nat=" << vip.naturalViolations
// 	       << " Cau=" << vip.causedViolations << endl;
// 	  vip.causedViolations=0;

	  bestSolutions.consider(vip);
	}
//     else 
//       cout << " INVALID" << endl;
    } // end while
  return bestSolutions.size();
}

/***************************************************************************/
/*   Find a good state variable insertion point and insert a state signal   */
/****************************************************************************/
bool find_sv(char * filename,signalADT signals[],eventADT events[],
	     mergeADT merges[],ruleADT *rules[],markkeyADT markkey[],
	     cycleADT ****cycles, int ncycles,
	     stateADT state_table[],int nevents,int ninputs,
	     int nrules,int maxgatedelay,char * startstate,
	     int status,regionADT regions[],int ninputsigs, int nsignals,
	     int ndisj, int nconf, int nord, bool verbose,
	     int initialCost, int& variableNumber, bool systemIsTimed,
	     CBestSolutions& bestSolutions,bool minins)
{
  cout << "Finding State Variables to solve " << initialCost
       << " CSC violation(s)" << endl;

  // SEARCH the valid solution space
  if (find_good_insertion_points(state_table,
				 ninputsigs,
				 events,
				 nevents,
				 rules, cycles, ncycles,
				 maxgatedelay,
				 systemIsTimed,
				 100, // # solutions to keep
				 bestSolutions,minins) == 0)
    {
      cout << "No solution found" << endl;
      return false;
    }


  return true;
}

/****************************************************************************/
/* main function which tries to solve state assignment problems.            */
/****************************************************************************/
bool find_state_assignment(char command, designADT design)
{
  //  bool csc_present;
  bool state_graph_empty=true;
  int  i;
  stateADT curstate;
  bool success=false;
  int initialCausedViolations, initialNaturalViolations;
  int causedViolations, naturalViolations;
  int variableNumber = 0;
  bool punt(false); // not making progress, give up
  int initialCost;

  /*
  if(design->untimed && design->level){
    loggedStream errors(cerr,lg);
    errors << "ERROR: Cannot use state assignment on untimed level-based specifications." << endl;
    return false;
  }
  */
  if (design->fromRSG) {
    printf("ERROR: Cannot use state assignment on RSG specifications.\n");
    fprintf(lg,"ERROR: Cannot use state assignment on RSG specifications.\n"); 
    return false;
  }
  // first make sure the rsg has been found
  for (i=0;i<PRIME;i++) {
    for (curstate=design->state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      { 
	state_graph_empty=FALSE;
	/*
	cout << "state" << curstate->state << endl;  
  	for(struct state_list_tag *p = curstate->pred; p!= NULL; p = p->next) 
	  cout<< "pred: "<< p->stateptr->state << "  "; 
  	for(struct state_list_tag *p = curstate->succ; p!= NULL; p = p->next) 
  	    cout<< "succ: "<< p->stateptr->state << "  "; 
	    cout << endl; */
      }
 
    /*    cout << endl;*/
  }
  if (state_graph_empty)
    {
      cout << "findsv: No state graph present" << endl;
      return false;
    }
  
  // check that CSC isn't already present
  initialCost = countCSCViolations(design->state_table,
				   design->ninpsig,
				   initialNaturalViolations,
				   initialCausedViolations);
  if (initialCost == 0)
    {
      cout << "findsv: Graph has no CSC violations" << endl;
      return true;
    }

  // the game is afoot

  // keep adding variables until no CSCV's remain or we give up
  while ((countCSCViolations(design->state_table, // still violations
			     design->ninpsig,
			     naturalViolations,
			     causedViolations) != 0)
	 && !punt) // still hope
    {
      // create a textTel for the current tel
      CTextTel textTel(design->nevents,design->events, design->ninputs, 
		       design->ncausal, design->rules,
		       design->merges, design->startstate, design->ndisj, 
		       design->nconf, design->nord);
      
      // create a container for the reversion and the best solutions
      CBestSolutions bestSolutions(initialCost, 100, textTel, design->events); 
      
      success=find_sv(design->filename,design->signals,design->events,
		      design->merges,design->rules,design->markkey,
		      design->cycles, design->ncycles,
		      design->state_table,design->nevents,design->ninputs,
		      design->ncausal, // used to be nrules
		      design->maxgatedelay,design->startstate,
		      design->status,design->regions,design->ninpsig,
		      design->nsignals,
		      design->ndisj, design->nconf, design->nord,
		      design->verbose, naturalViolations,
		      variableNumber, !(design->untimed),
		      bestSolutions,design->minins);

      // if a strange error occured, exit findsv (CURRENTLY BOGUS)
      if (! success)
	{
	  cout << "findsv: Unable to solve CSC -- Possible error" << endl;
	  return false;
	}

      // SELECT THE BEST SOLUTION
      int userSelection;

      if (design->manual) {
	cout << endl << endl;
	cout << "Best IPs: " << endl;
	int solNumber(0);
	for (solution_list::iterator i_sol = 
	       bestSolutions.allSolutions().begin();
	     i_sol != bestSolutions.allSolutions().end(); i_sol++, solNumber++)
	  {
	    cout << solNumber << " " << 
	      insertionString(design->events, *i_sol);
	    cout << " : " << (*i_sol).naturalViolations << "+" << 
	      (*i_sol).causedViolations;
	    cout << " [" << (*i_sol).initialValue << "]" << endl;
	  }
	cout << "Pick a solution ";
	cin >> userSelection;
      } else 
	userSelection=0;

      solution_list::iterator i_sol = bestSolutions.allSolutions().begin();
      advance(i_sol, userSelection);
      
      CInsertionPoint insertionPoint = *i_sol;
      
      // PACKAGE the insertion point into a new CStateVariableInfo
      // create a unique name that doesn't already exist in the system
      string variableName(nextCSCVariableName(variableNumber, design->signals,
					      design->nsignals));
      
      // create a CStateVariableInfo object
      CStateVariableInfo svInfo(variableName,
				"1", // instance, for now always 1
				insertionPoint,  
				// the graph location of the new sv
				design->events, design->nevents, 
				// for reachable() calls
				0, // lower bound
				design->maxgatedelay, // upper bound
				design->cycles, design->ncycles);
      // for reachable,
  
      cout << "Solution costing " << insertionPoint.naturalViolations
	   << " + " << insertionPoint.causedViolations << " used" << endl;
      
      cout << "Signal named " << svInfo.signalName << " being added" << endl;

      cout << "IP: " << insertionString(design->events, insertionPoint) << 
	endl << endl;;


      if (1/*!design->level*/) {
	// INSERT the state variable
	// add the state variable to the CTextTel
	if ((design->fromER || design->fromTEL) && 
	    ((strlen(design->filename) < 3) ||
	     (design->filename[strlen(design->filename)-3]!='C') || 
	     (design->filename[strlen(design->filename)-2]!='S') ||
	     (design->filename[strlen(design->filename)-1]!='C')))
	  strcat(design->filename,"CSC");
	success = textTel.addStateVariable(svInfo);
	// dump to disk
	if (!textTel.writeToFile(design->filename,design->level)) {
	  return false; 
	}
	// reload the modified TEL file into memory
	if (design->level)
	  design->status=
	    process_command(LOAD, TEL, design, NULL, TRUE, design->si);
	else
	  design->status=
	    process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);
	//if (!(design->status & LOADED)) return false;
	// check for liveness
	design->status=
	  process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, design->si);

	// if graph deadlocks
	int cscIndex = -1;
	if((design->status & FOUNDSTATES) == 0)
	  {
	    cout << "Fixing marking of first CSC transition" << endl;
	    // determine the event index for the first CSC transition
	    /* TOMOHIRO:
	    if (design->startstate[design->nsignals-2] == '0')
	      cscIndex = design->nevents - 4;
	    else
	      cscIndex = design->nevents - 3; */
	    if (design->startstate[design->nsignals-1] == '0')
	      cscIndex = design->nevents - 2;
	    else
	      cscIndex = design->nevents - 1;

	    // check each rule entering the first CSC transition
	    for (int i=0; i < design->nevents; i++)
	      {
		if (design->rules[i][cscIndex]->ruletype != NORULE)
		  if (design->rules[i][cscIndex]->reachable == FALSE)
		    {
		      design->rules[i][cscIndex]->epsilon = 1;
		      cout << design->events[i]->event << "->" 
			   << design->events[cscIndex]->event << endl;
		    }
	      }
	  }

	// now that we've tried to fix it, did we?
	// check for liveness
	if (design->level) {
	  design->status=
	    process_command(STORE, TEL, design, NULL, FALSE, design->si);
	  design->status=
	    process_command(LOAD, TEL, design, NULL, TRUE, design->si);
	} else {
	  design->status=
	    process_command(STORE, TIMEDER, design, NULL, FALSE, design->si);
	  design->status=
	    process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);
	}
	//if (!(design->status & LOADED)) return false;
	design->status=
	  process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, design->si);
	
	// if still not live, mess with the other transition
	if((design->status & FOUNDSTATES) == 0)
	  {
	    cout << "Fixing marking of second CSC transition" << endl;
	    // determine the event index for the second CSC transition
	    /* TOMOHIRO:
	    if (design->startstate[design->nsignals-1] == '0')
	      cscIndex = design->nevents - 3; // these are swapped from above
	    else
	      cscIndex = design->nevents - 4; */
	    if (design->startstate[design->nsignals] == '0')
	      cscIndex = design->nevents - 1; // these are swapped from above
	    else
	      cscIndex = design->nevents - 2;

	    // check each rule entering the second CSC transition
	    for (int i=0; i < design->nevents; i++)
	      {
		if (design->rules[i][cscIndex]->ruletype != NORULE)
		  if (design->rules[i][cscIndex]->reachable == FALSE)
		    {
		    design->rules[i][cscIndex]->epsilon = 1;
		    cout << design->events[i]->event << "->" 
			 << design->events[cscIndex]->event << endl;
		  }
	    }
 
	    // check one final time for liveness
	    if (design->level) {
	      design->status=
		process_command(STORE, TEL, design, NULL, FALSE, design->si);
	      design->status=
		process_command(LOAD, TEL, design, NULL, TRUE, design->si);
	    } else {
	      design->status=
		process_command(STORE, TIMEDER, design, NULL, FALSE, 
				design->si);
	      design->status=
		process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);
	    }
	    //if (!(design->status & LOADED)) return false;
	    design->status=
	      process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, 
			      design->si);
	    if((design->status & FOUNDSTATES) != 0 && cscIndex != -1)
	      {
		cout << "Graph brought back to life" << endl;;
	      }
	    
	    // find the new state graph
	    /* CHRIS M. - Don't see why this is necessary
	    design->status =
	      process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, 
	      design->si); */
	  }

      // rate the new state graph
	int newNaturalViolations;
	int newCausedViolations;
	countCSCViolations(design->state_table,
			   design->ninpsig,
			   newNaturalViolations,
			   newCausedViolations);
	cout << "Initial: " << initialNaturalViolations << " + " << initialCausedViolations << endl;
	cout << "Predicted: " << insertionPoint.naturalViolations << " + "
	     << insertionPoint.causedViolations << endl;
	cout << "Actual: " << newNaturalViolations << " + " << newCausedViolations << endl;

	if (design->manual) {
	  cout << "E(x)it, (C)ontinue adding variables " << endl;
	  char userInput;
	  cin >> userInput;
	  if (userInput == 'x') punt = true;
	}

	// all done, so ...
	// pass control to the next correct function in the function chain
	// if part of a larger command
	if (!(design->status & FOUNDSTATES)) return false;
	if (command != FINDSV) 
	  {
	    design->status=process_command(SYNTHESIS, command, design,
					  NULL, FALSE, design->si);
	  }
      } else
	return false;
    }
  return true;
}

//////////////////////////////////////////////////

//   design->status=design->status | FOUNDSTATES;






