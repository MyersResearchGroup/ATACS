#include "svclasses.h"
#include "connect.h"
#include "findwctd.h"
#include "def.h"
#include "interface.h"
#include "findrsg.h"

#include <algorithm>
#include <queue>
#include <iterator>

using namespace std;

//=========================================================================
// class svConcurrency
//=========================================================================
svConcurrency::svConcurrency(designADT d)
   : concurrencyTable(d->nevents), rules(d->rules), cycles(d->cycles),
     nevents(d->nevents), ncycles(d->ncycles), svMaxDelay(d->maxgatedelay)
{
   for (int i=0; i < PRIME; i++) // for every state bin
   {
      for (stateADT curstate = d->state_table[i]; // for every state in bin
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link)
      {
 	 sv_event_index_cont concurrentlyEnabledEvents;
	 // for every next state from this state
	 for (statelistADT nextNode = curstate->succ;
	      nextNode != NULL;  nextNode = nextNode->next)
	 { // add the event to the list of events concurrent in this state
	    concurrentlyEnabledEvents.push_back(nextNode->enabled);
	 }
	 
	 // if there are concurrent events, fill in the table accordingly
	 if (concurrentlyEnabledEvents.size() > 1)
	 {
	    for (sv_event_index_cont::iterator i_firstEvent
		   = concurrentlyEnabledEvents.begin();
		 i_firstEvent != concurrentlyEnabledEvents.end();
		 i_firstEvent++)
	    {
	       for (sv_event_index_cont::iterator i_secondEvent=
		      concurrentlyEnabledEvents.begin();
		    i_secondEvent != concurrentlyEnabledEvents.end();
		    i_secondEvent++)
	       {
		  if (i_firstEvent != i_secondEvent)
		  {
		     concurrencyTable[*i_firstEvent].insert(*i_secondEvent);
		  }
	       }
	    }
	 }
      }
   }

//     cerr << "Concurrency Table Begin" << endl;
//     for (vector<concurrencyRow>::iterator row = concurrencyTable.begin();
//  	row != concurrencyTable.end(); row++)
//     {
//        cerr << ": ";
//        copy((*row).begin(), (*row).end(), ostream_iterator<int>(cerr, "\t - \t"));
//        cerr << " :" << endl;
//     }
//     cerr << "Concurrency Table End" << endl;


}

svConcurrency::~svConcurrency()
{
}

//======================================================================
concurrency_status
svConcurrency::eventsAreConcurrent(const sv_event_index_cont& events) const
{
   for (size_t i = 0; i < events.size(); i++) // for each event
   {
      for (size_t j = i; j < events.size(); j++) // for each other event
      {
	 if ( i != j)
	 {
	    // handle explict conflict
	    if (rules[events[i]][events[j]]->conflict) return eConflicts;
	    
	    // check timing based concurrency
	    if (concurrencyTable[events[i]].find(events[j])
		== concurrencyTable[events[i]].end()) 
	    {
	       return eSerial;
	    }
	 }
      }
   }

   return eConcurrent;
}

//======================================================================
concurrency_status
svConcurrency::eventsAreConcurrent(const sv_event_index_cont& events,
				   sv_event_index e) const
{
   //
   // assumes:
   //         e !element_of events
   //         all elements of events are concurrent
   //
   for (size_t i = 0; i < events.size(); i++) // for each event
   {
      // handle explict conflict
      if (rules[events[i]][e]->conflict) return eConflicts;
      
      // check timing based concurrency
      if (concurrencyTable[events[i]].find(e)
	  == concurrencyTable[events[i]].end())
      {
	 return eSerial;
      }
   }

   return eConcurrent;
}

//======================================================================
bool
svConcurrency::hasConflict(const svTP& tp) const
{
   // assumes that the start and end set don't conflict within themselves
   for (sv_event_index_cont::const_iterator iS = tp.startEvents().begin();
	iS != tp.startEvents().end(); iS++) // for each event in start set
   {
      for (sv_event_index_cont::const_iterator iE = tp.endEvents().begin();
	   iE != tp.endEvents().end(); iE++) // for each event in end set
      {
	 if (rules[*iS][*iE]->conflict) return true;
      }
   }
   return false;
}

//======================================================================
bool
svConcurrency::hasConflict(const sv_event_index_cont& events, sv_event_index e) const
{
  for (sv_event_index_cont::const_iterator iE = events.begin();
       iE != events.end(); iE++) // for each event given
  {
     if (rules[*iE][e]->conflict) return true;
  }
  return false;
}

//======================================================================
bool
svConcurrency::isWitness(const sv_event_index_cont& events, sv_event_index e) const
{
   // check that we have paths from startEvents to possible witness event
   for (sv_event_index_cont::const_iterator i_ss = events.begin();
	i_ss != events.end(); i_ss++)
   {
      if (!isWitness(*i_ss, e)) return false;
   }

   return true;
}

//======================================================================
bool
svConcurrency::isWitness(const sv_event_index s, const sv_event_index e) const
{
   // check that we have paths from startEvents to possible witness event
   int cyclesBetween(0);
   if (reachable(cycles, nevents, ncycles, s, 0, e, 0) == notreachable)
   {
      if (reachable(cycles, nevents, ncycles, s, 0, e, 1) == notreachable)
      {
	 // we don't have all the necessary paths, so this isn't a witness
//   	 cerr << e << " is not a reachable witness for " << s << endl;
	 return false;
      }
      else cyclesBetween = 1;
   }

   // check that the timing allows this to be a witness
   boundADT timeDiff = WCTimeDiff(rules, cycles, nevents, ncycles,
				  s, e, cyclesBetween);
   if (timeDiff->L <= svMaxDelay)
   {
      // timing says this isn't a witness transition
//        cerr << e << " is not a wctd witness for start element " << s << endl;
//        cerr << timeDiff->L << " is the lower bound, and the gatedelay is "
//     << svMaxDelay << endl;
      free(timeDiff);
      return false;
   }
   free(timeDiff);

   return true;
}

//=========================================================================
// class svTP
//=========================================================================
svTP::svTP()
{
}

svTP::~svTP()
{
}

ostream& operator<<(ostream& o, const svTP& tp)
{ 
   o << "<--- TP --->" << endl;
   o << "Start: ";
   copy(tp.st.begin(), tp.st.end(), ostream_iterator<int>(o, " "));
   o << "\nWitness: ";
   copy(tp.wt.begin(), tp.wt.end(), ostream_iterator<int>(o, " "));
   o << "\nEnd: ";
   copy(tp.et.begin(), tp.et.end(), ostream_iterator<int>(o, " "));
   o << endl;
   return o;
}

//=========================================================================
// class svIP
//=========================================================================
// If the colored cost has alread been found, return it
// if the cost of this IP, with coloring considered, has not been found,
// find it now, assuming that this IP maps back to the currently installed system
// and that the state graph has been found
int
svIP::cost() const
{
  if (coloredCost == -1)
  {
    // true to violations means consider coloring 
    pair<int, int> violations = sys->violations(true);
    // TODO HARDCODE, use a better formula
    coloredCost = static_cast<int> (violations.first + 0.51 * violations.second);
  }

  return coloredCost;
}

bool
svIP::operator< (const svIP& o) const
{
  if (cost() == o.cost())
  {
    // rely on secondary factors to compare the IPs
    // try the size of the end transition sets
    // this will give the total number of events which could
    // be delayed by the insertion of this state signal
    int delayedEvents = rTP.endEvents().size() + fTP.endEvents().size();
    int otherDelayedEvents = o.rTP.endEvents().size() + o.fTP.endEvents().size();
    if (delayedEvents == otherDelayedEvents)
    {
      // rely on tertiary factors
      // try the number of events in the start events set
      // this is the size of the support set for the state signal logic
      // smaller means fewer inputs to the C element, hopefully
      int supportSize = rTP.startEvents().size() + fTP.startEvents().size();
      int otherSupportSize = o.rTP.startEvents().size() + o.fTP.startEvents().size();

      return ! (supportSize > otherSupportSize);
    }

    return delayedEvents < otherDelayedEvents;
  }

  return cost() < o.cost();
}

ostream& operator<<(ostream& stream, const svIP& ip)
{
  stream << "Rising TP\n" << ip.rTP << "Falling TP\n" << ip.fTP
	 << "System Ptr: " << ip.sys
	 << " - Initial State " << ip.initialValue << endl;
  return stream;
}

bool
svIP::compatible() const
{

  // this is just a quick check to make sure that
  // the IP is not obviously inconsistent
  // defined by krieger to be st_TP_R intersect st_TP_F = emptyset
  //                       && et_TP_R intersect et_TP_F = emptyset

  sv_event_index_cont start_intersection;
  set_intersection(rTP.startEvents().begin(),
		   rTP.startEvents().end(),
		   fTP.startEvents().begin(),
		   fTP.startEvents().end(),
		   inserter(start_intersection, start_intersection.begin())
		   );
  if (! start_intersection.empty()) return false;

  // union the witness and end events together
  // then check the intersection
  sv_event_index_cont rise_enders, fall_enders;
  merge(rTP.witnessEvents().begin(),
	rTP.witnessEvents().end(),
	rTP.endEvents().begin(),
	rTP.endEvents().end(),
	inserter(rise_enders, rise_enders.begin()) );

  merge(fTP.witnessEvents().begin(),
	fTP.witnessEvents().end(),
	fTP.endEvents().begin(),
	fTP.endEvents().end(),
	inserter(fall_enders, fall_enders.begin()) );

  sv_event_index_cont ender_intersection;
  set_intersection(rise_enders.begin(),
		   rise_enders.end(),
		   fall_enders.begin(),
		   fall_enders.end(),
		   inserter(ender_intersection, ender_intersection.begin()));

  return (ender_intersection.empty());

}

bool
svIP::consistent()
{
  // intersect the SR's of the rise start events
  state_list riseStartStates =
    sys->intersectRegions(rTP.startEvents(),
  			  sys->switchingRegionStates());

  // union the ER's of the rise end events
  state_list riseEndStates =
    sys->unionRegions(rTP.endEvents(),
		      sys->excitationRegionStates());
  
  // union the ER's of the rise witness events
  state_list riseWitnessStates = 
    sys->unionRegions(rTP.witnessEvents(),
		      sys->excitationRegionStates());

  // merge the rise end and rise witness states
  copy(riseWitnessStates.begin(), riseWitnessStates.end(),
       back_inserter(riseEndStates));

  // sort them, so that unique works
  riseEndStates.sort();
  riseEndStates.erase(unique(riseEndStates.begin(), riseEndStates.end()),
		      riseEndStates.end());

  // intersect the SR's for the fall start events
  state_list fallStartStates =
    sys->intersectRegions(fTP.startEvents(),
			  sys->switchingRegionStates());

  // union the ER's of the fall end events
  state_list fallEndStates =
    sys->unionRegions(fTP.endEvents(),
		      sys->excitationRegionStates());

  // union the ER's of the fall witness events
  state_list fallWitnessStates =
    sys->unionRegions(fTP.witnessEvents(),
		      sys->excitationRegionStates());

  // merge the fall end and fall witness states
  copy(fallWitnessStates.begin(), fallWitnessStates.end(),
       back_inserter(fallEndStates));

  // sort and unique-ify
  fallEndStates.sort();
  fallEndStates.erase(unique(fallEndStates.begin(), fallEndStates.end()),
		      fallEndStates.end());

//   cerr << "Here we go with the IP" << endl;
//   cerr << rTP << endl << fTP << endl;
//   cerr << endl;

  // color the state graph
  bool coloredOK = sys->colorStateGraph(riseStartStates, riseEndStates,
					fallStartStates, fallEndStates);

  return coloredOK;
}

//=========================================================================
// class svSystem
//=========================================================================

// constructors
svSystem::svSystem(designADT d)
  : design(d), concurrency(d), erStates(), srStates(),
    color_field_offset(0), textTel(d),
    variableNumber(0)
{
  // find states in the er and sr of every event
  // these states will then be used for coloring the state graph
  findAllRegions(back_inserter(erStates),
		 back_inserter(srStates)); // find excitation and switching

  // DEBUG
  // dump out the ER and SR for each event
//   for (int i = 0; i < design->nevents; i++)
//   {
//     cerr << "ER for event " << i << endl;
//     for (state_list::const_iterator i_er = erStates[i].begin();
// 	 i_er != erStates[i].end(); i_er++)
//     {
//       cerr << "State " << (*i_er)->state << endl;
//     }

//     cerr << "SR for event " << i << endl;
//     for (state_list::const_iterator i_sr = srStates[i].begin();
// 	 i_sr != srStates[i].end(); i_sr++)
//     {
//       cerr << "State " << (*i_sr)->state << endl;
//     }
//   }

  generateColorMask();
  // set offset to impossibly large value, to force a clear on
  // first call to clearColors -- clearing now wastes a bitfield entry
  color_field_offset = sizeof(unsigned int) * 8;
}

// destructor
svSystem::~svSystem()
{
}

/****************************************************************************/
/* Find the ER and SR for all events                                        */
/* This function uses two different types of back insert iterators,         */
/* one is for containers of states, the other is for containers of          */
/* containers of states. Thus, findAllRegions finds a region in tempRegion  */
/* then adds that temp container to the container of all ers or the         */
/* container of all srs. They are sorted to satisfy set_intersection        */
/****************************************************************************/
template<class back_insert_iter>
bool
svSystem::findAllRegions(back_insert_iter bii_er, back_insert_iter bii_sr)
{
  bool result(true);
  state_list tempRegion;
  back_insert_iterator<state_list> bii_tempRegion(tempRegion);
  
  // push on empty lists for the ER and SR of the reset event
  *bii_sr = tempRegion;
  *bii_er = tempRegion;

  for (int eventIndex = 1; // skip 0, the reset event
       eventIndex < design->nevents && result;
       ++eventIndex)
  {
    if (result = findER(eventIndex, bii_tempRegion))
    {
      tempRegion.sort();
      *bii_er = tempRegion;
//       cerr << "Pushing on an ER for event " << eventIndex << endl;
//       cerr << "ER has " << tempRegion.size() << " states" << endl;
      tempRegion.clear();
    }

    if (!result) cerr << "RESULT HOSED BY ER for event " << eventIndex << endl;

    if (result = result && findSR(eventIndex, bii_tempRegion))
    {
      tempRegion.sort();
      *bii_sr = tempRegion;
//       cerr << "Pushing on an SR for event " << eventIndex << endl;
//       cerr << "SR has " << tempRegion.size() << " states" << endl;
      tempRegion.clear();
    }
  }
  
  return result;
}


bool
svSystem::findER(int eventIndex, back_insert_iterator<state_list> bii_ER)
{
  // check each state in the state table to see if
  // the enabling string contains an R or F, as appropriate,
  // in the correct location
  // if yes, make sure the event is correct
  // then insert this state pointer into the ER list

  stateADT* state_table(design->state_table);
  eventADT* events(design->events);

  stateADT curstate;
  char foundChar;
  state_list thisGenerationList;
  state_list nextGenerationList;
  stateADT thisState;
  bool ER_inclusion_known=false;

  eventADT event(events[eventIndex]);
  string eventString(event->event);

  // if rising event, look for R, falling look for F
  char targetChar = (eventString.find("+") != string::npos) ? 'R' : 'F';

  // figure out what this event looks like textually
  size_t slashPoint = eventString.find('/');
  string eventNumber = eventString.substr(slashPoint+1, string::npos);

  bool ER_non_empty = false;
  for (int i=0; i < PRIME; i++)
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
	      thisState = thisGenerationList.back();
	      thisGenerationList.pop_back();
	      // look at each of this state's successors
	      for (statelistADT nextNode=thisState->succ;
		   nextNode != NULL;
		   nextNode=nextNode->next)
	      {
		if (nextNode->enabled==eventIndex)
		  // if the correct event brought us here
		{ // curstate IS in the ER
		  *bii_ER=curstate;
		  ER_inclusion_known=true;
		  ER_non_empty = true;
		}
		// if enter by a different event on same signal
		else if (events[nextNode->enabled]->signal==event->signal)
		{ // curstate is NOT in the ER
		  ER_inclusion_known=true;
		}
	      } // end for each successor
	      // this state's arcs aren't conclusive,
	      //  so add its succs, just in case no other state
	      // in this generation is conclusive
	      if (!ER_inclusion_known)
	      {
		for (statelistADT nextNode=thisState->succ;
		     nextNode != NULL;
		     nextNode=nextNode->next)
		{
		  nextGenerationList.push_back(nextNode->stateptr);
		}
	      }
	    } // end while for a single generation
	    // inconclusive, so shift down a generation
	    thisGenerationList=nextGenerationList;
	    nextGenerationList.clear();
	  } // end while which goes until we know one way or the other
	} // end if for only promising candidate states
      } // end for each state in graph
  
  return ER_non_empty;
}

/*****************************************************************************/
/*   Fill a container of states representing switching region SR(event)      */
/*****************************************************************************/
bool
svSystem::findSR(int eventIndex,
		 back_insert_iterator<state_list> bii_SR)
{
  // check each possible state
  // if the value of the signal is correct (1,0)
  // look through the pred list for a transition with event on it
  // if found, add this to SR

  bool result(false);
  eventADT event = design->events[eventIndex]; 
  
  string eventString(event->event);
  statelistADT predNode;

  char targetValChar;
  char targetEnablingChar;
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

  for (int i=0; i<PRIME; i++) // for every state
    for (stateADT curstate = design->state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate = curstate->link)
    { 
      char foundChar=(curstate->state)[event->signal];
      if (foundChar == targetValChar || foundChar == targetEnablingChar)
	  // if correct post transitional value 
      {
	// look at each pred and see if it connects to this state
	// through the targetEvent
	bool done;
	for (predNode=curstate->pred, done=false;
	     (predNode != NULL) && !done;
	     predNode=predNode->next)
	{
	  if (predNode->enabled==eventIndex)
	    // if the correct event brought us here
	   {
	     *bii_SR = curstate; // add the state to the SR
	     result = true;
	     done = true;
	   }
	}
      }
    }
  
  return result;
}

/****************************************************************************/
/* intersect the SR's of the start events                                   */
/****************************************************************************/
state_list
svSystem::intersectRegions(const sv_event_index_cont& events,
			   const state_list_cont& regionStates) const
{
  state_list intersection;

  if (events.empty()) return intersection;

  state_list partialIntersection;
  back_insert_iterator<state_list> bii_partIntersection(partialIntersection);

  // seed the intersection with the states in the first region
  intersection = regionStates[events.front()];

  // intersect this with the states for each of the other regions
  if (events.size() > 1)
    for (sv_event_index_cont::const_iterator i_event=events.begin()+1;
	 i_event != events.end(); i_event++)
    {
      set_intersection(intersection.begin(), intersection.end(),
		       regionStates[*i_event].begin(),
		       regionStates[*i_event].end(),
		       bii_partIntersection);
	
      intersection = partialIntersection;
      partialIntersection.clear();
    }

  return intersection;
}

/****************************************************************************/
// Union the ER's of the end/witness events
/****************************************************************************/
state_list
svSystem::unionRegions(const sv_event_index_cont& events,
		       const state_list_cont& regionStates) const
{
  state_list the_union;

  if (events.empty()) return the_union;

  state_list partialUnion;
  back_insert_iterator<state_list> bii_partUnion(partialUnion);

  // seed the union with the states in the first region
  the_union = regionStates[events.front()];

  // union this with the states for each of the other regions
  if (events.size() > 1)
    for (sv_event_index_cont::const_iterator i_event=events.begin()+1;
	 i_event != events.end(); i_event++)
      {
	set_union(the_union.begin(), the_union.end(),
		  regionStates[*i_event].begin(),
		  regionStates[*i_event].end(),
		  bii_partUnion);
	
	the_union = partialUnion;
	partialUnion.clear();
      }

  return the_union;
}

///////////////////////////////////////////////////////////////////////////
// color the whole graph
///////////////////////////////////////////////////////////////////////////
bool
svSystem::colorStateGraph(const state_list& riseStart,
			  const state_list& riseEnd,
			  const state_list& fallStart,
			  const state_list& fallEnd)
{
  state_list seedHigh;
  state_list seedLow;

  bool isConsistent = true;
  
  clearColors(COLOR_BLANK);

  // make sure that the intersections of the regions are not empty
  if ( riseStart.empty() || fallStart.empty()
      || riseEnd.empty() || fallEnd.empty()) return false;

  isConsistent = colorRise(riseStart, riseEnd, seedHigh);

  isConsistent = isConsistent && colorFall(fallStart, fallEnd, seedLow);

  isConsistent = isConsistent && colorHigh(seedHigh);

  isConsistent = isConsistent && colorLow(seedLow);

  // DEBUG
//   if (isConsistent)
//   {
//     cerr << "=========================START ============" << endl;
//   for (int i=0; i < PRIME; i++)
//     for (stateADT curstate=design->state_table[i];
// 	 curstate != NULL && curstate->state != NULL;
// 	 curstate=curstate->link)
//     {
//       cerr << "State: " << curstate->state
// 	   << " Color: " << getColor(curstate) << endl;
//     }

//     for (state_list::const_iterator i_startState = riseStart.begin();
//        i_startState != riseStart.end();
//        i_startState++)
//     {
//       cerr << "The rise start included " << (*i_startState)->state << endl;
//     }
//     for (state_list::const_iterator i_endState = riseEnd.begin();
//        i_endState != riseEnd.end();
//        i_endState++)
//     {
//       cerr << "The rise end included " << (*i_endState)->state << endl;
//     }

//     for (state_list::const_iterator i_startState = fallStart.begin();
//        i_startState != fallStart.end();
//        i_startState++)
//     {
//       cerr << "The fall start included " << (*i_startState)->state << endl;
//     }

//     for (state_list::const_iterator i_endState = fallEnd.begin();
//        i_endState != fallEnd.end();
//        i_endState++)
//     {
//       cerr << "The fall end included " << (*i_endState)->state << endl;
//     }
//     cerr << "=========================== END ============" << endl;
//   }

  return isConsistent;
}

int
svSystem::getColor(const stateADT state) const
{
  return (state->color >> color_field_offset) & color_mask;
}

void
svSystem::setColor(stateADT state, int color)
{
  int blanked_color = state->color & ~(color_mask << color_field_offset);
  state->color = (color << color_field_offset) | blanked_color;
}

void
svSystem::generateColorMask()
{
  // make a BITS_PER_COLOR wide bitmask at the starting position
  // this will never change after compilation
  // an example would be binary 00000111
  color_mask = 0;
  for (unsigned int i = 0; i < BITS_PER_COLOR; i++)
  {
    color_mask = color_mask << 1;
    color_mask |= 0x1;
  }
}

/*****************************************************************************/
/*   color the entire state graph with a blank color                         */
/*****************************************************************************/
void
svSystem::clearColors(int color, bool reset)
{
  // shift to a new, clear bit sequence in the color unsigned int bitfield
  // if we've reached the end of the bitfield, reset offset to the beginning
  // and set the whole bitfield to color color color ... 

  static const unsigned int bitfield_max = sizeof(unsigned int) * 8 - 1;
  stateADT curstate;
  stateADT* state_table(design->state_table);

  if ( (color_field_offset + BITS_PER_COLOR + (BITS_PER_COLOR-1)
	> bitfield_max) || reset )
  {
    // generate the clear pattern
    // this doesn't change after compilation unless
    // you want to clear to a color other than COLOR_BLANK
    int blanking_pattern = 0;
    for (unsigned int i = 0; i < bitfield_max; i += BITS_PER_COLOR)
    {
      blanking_pattern |= color;
      color = color << BITS_PER_COLOR;
    }

    // set the member data offset to 0
    color_field_offset = 0;

    // set every state to clear
    for (int i=0; i < PRIME; i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link)
      {
	curstate->color = blanking_pattern;
      }
  }
  else // don't really clear, just shift into the next bit position
  {
    color_field_offset += BITS_PER_COLOR;
  }
}

/*****************************************************************************/
/*   color the states of a rising transition with R                          */
/*   keep track of the rising states so we know where to start coloring high */
/*****************************************************************************/
bool
svSystem::colorRise(const state_list& startStates, const state_list& endStates,
		    state_list& highSeed)
{
  queue<stateADT> nextStateQueue;
  stateADT thisState;

  // first color the endStates COLOR_R, so we'll know
  // to stop when we reach them
  for (state_list::const_iterator i_endState=endStates.begin();
       i_endState != endStates.end(); i_endState++)
  {
    // this line should be unnecessary, but...
    if (getColor(*i_endState) != COLOR_BLANK)
    {
      cout << "colorRise: graph in illegal initial state" << endl;
      cout << "Color is " << oct << getColor(*i_endState) << dec << endl;
      cout << "State is " << *i_endState << endl;
      return false;
    }
    setColor(*i_endState, COLOR_R);
    highSeed.push_back(*i_endState);
  }
  
  // prime the queue with the startStates TODO use STL copy
  for (state_list::const_iterator i_startState = startStates.begin();
       i_startState != startStates.end();
       i_startState++)
    {
      nextStateQueue.push(*i_startState);
    }

  // now sweep from the start of the rise region to the end of the rise region
  while ( ! nextStateQueue.empty() )
    {
      // take the next state out of the nextStateQueue
      thisState = nextStateQueue.front();
      nextStateQueue.pop();

      // process this state
      switch( getColor(thisState) )
	{
	case COLOR_0:
	case COLOR_1:
	case COLOR_F:
	  // this error shouldn't occur,
	  // since colorRise is the first of the coloring
	  // subfunctions to be called
	  cerr << "Oddly, colorRise hits 0/F/1 color in state "
	       << thisState->state
	       << " color is " << getColor(thisState) << endl;
	  assert(false);
	  break; // this break should never be reached
	  
	case COLOR_R:
	  // if state is already colored, just continue
	  // this assumes that whoever colored it in the first
	  // place will take care of its successors
	  // or this state was colored during the endStates pass above
	  break;

	case COLOR_BLANK:
	  // color this state
	  setColor(thisState, COLOR_R);
	  // add this state to the list,
	  // for use as a starting seed for colorHigh()
	  highSeed.push_back(thisState);
			  
	  // now add its successor states to the queue
	  for (statelistADT nextState=(thisState)->succ;
	       nextState != NULL;
	       nextState = nextState->next)
	  {
	    nextStateQueue.push(nextState->stateptr);
	  }
	  break;

	default:
	  cerr << "findsv: Bogus color detected by colorRise in "
	       << thisState->state << endl;
	  cerr << "Color is " << getColor(thisState) << endl;
	  assert(false);
	  break;
	}
    } // end while 

  return true;
}

/*****************************************************************************/
/*   color states in the falling transition with F                           */
/*****************************************************************************/
bool
svSystem::colorFall(const state_list& startStates,
		    const state_list& endStates,
		    state_list& seedLow)
{
  queue<stateADT> nextStateQueue;
  stateADT thisState;

  // first color the endStates COLOR_F,
  // so we'll know to stop when we reach them
  for (state_list::const_iterator i_endState=endStates.begin();
       i_endState != endStates.end();
       i_endState++)
  {
    if (getColor(*i_endState) != COLOR_BLANK)
    {
      return false;
    }
    setColor(*i_endState, COLOR_F);
    seedLow.push_back(*i_endState);
  }
  
  // prime the queue with the startStates
  for (state_list::const_iterator i_startState=startStates.begin();
       i_startState != startStates.end();
       i_startState++)
    {
      nextStateQueue.push(*i_startState);
    }
 
  // now move through the queue until it is empty
  while ( ! nextStateQueue.empty() )
    {
      // take the next queue entry
      thisState = nextStateQueue.front();
      nextStateQueue.pop();

      switch( getColor(thisState) )
	{
	case COLOR_1:
	case COLOR_0:
	  cerr << "Invalid state graph coloring in colorFall" << endl;
	  assert(false);
	  break;

	case COLOR_R:
	  // this coloring is inconsistent
	  return false;
	  break; // this break should never be reached
	      
	case COLOR_F:
	  // if state is already colored, just continue
	  // this assumes that whoever colored it in the first
	  // place will take care of its successors
	  break;
	  
	case COLOR_BLANK:
	  setColor(thisState, COLOR_F);
	  seedLow.push_back(thisState);
	  // now add successors 
	  for (statelistADT nextState = thisState->succ;
	       nextState != NULL;
	       nextState=nextState->next)
	  {
	    nextStateQueue.push(nextState->stateptr);
	  }
	  break;

	default:
	  cerr << "findsv: bogus color detected by colorFall in "
	       << thisState->state << endl;
	  cerr << "Color is " << getColor(thisState) << endl;
	  assert(false);
	  break;
	}
    } // end while 

  return true;
}

/*****************************************************************************/
/*   color with 1, returning false if inconsistent                           */
/*****************************************************************************/
bool
svSystem::colorHigh(state_list& riseRegion)
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
	  if (getColor(nextState->stateptr) != COLOR_R)
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
	  switch( getColor(*i_state) )
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
	      return false;
	      break; // this break should never be reached

	    case COLOR_BLANK:
	      setColor(*i_state, COLOR_1);
	      for (statelistADT nextState=(*i_state)->succ;
		   nextState != NULL;
		   nextState=nextState->next)
	      {
		nextList.push_back(nextState->stateptr);
	      }
	      break;

	    default:
	      cerr << "ColorHigh has found unexpected color "
		   << getColor(*i_state) << endl;
	      assert(false);
	      break;
	    }
	} // end for

      thisList = nextList;
      nextList.clear();
    } // end while 

  return true;
}

/*****************************************************************************/
/*   color with 0, returning false if inconsistent                           */
/*****************************************************************************/
bool
svSystem::colorLow(state_list& fallRegion)
{
  state_list thisList;
  state_list nextList;

  // this assumes that the riseRegion has already been painted with R
  // and fallRegion with F

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
	  if (getColor(nextState->stateptr) != COLOR_F)
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
	  switch( getColor(*i_state) )
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
	      return false;
	      break; // this break should never be reached

	    case COLOR_BLANK:
	      setColor(*i_state, COLOR_0);
	      for (statelistADT nextState=(*i_state)->succ;
		   nextState != NULL;
		   nextState=nextState->next)
		{
		  nextList.push_back(nextState->stateptr);
		}
	      break;

	    default:
	      cerr << "colorLow has found unexpected color "
		   << getColor(*i_state) << endl;
	      break;
	    }
	} // end for

      thisList = nextList;
      nextList.clear();
    } // end while 

  return true;
}

///////////////////////////////////////////////////////////////////////////
// return the actual state value, with 0/1 only, no enablings like R/F
string
svSystem::stateVal(string& enabling)
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
int
svSystem::colorVal(int color)
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
// put this system into the normal atacs designADT space
///////////////////////////////////////////////////////////////////////////
bool
svSystem::install(bool refindStateSpace) // defaults to true
{
  // write out the CTextTel to disk
  if (!textTel.writeToFile(design->filename, design->level))
  {
    return false; 
  }

  // reload the system from disk into memory
  bool verbose = design->verbose;
  design->verbose = false;
  if (design->level)
    design->status = process_command(LOAD, TEL, design, NULL, TRUE, design->si);
  else
    design->status = process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);

  if (!(design->status & LOADED))
  {
    cerr << "findsv: Failed to read modified TER/TEL" << endl;
    design->verbose = verbose;
    return false;
  }

  // refind the state space of this system
  // this is costly, but necessary in most cases if further work
  // is to be done with a partially solved system
  if (refindStateSpace == false) return true;

  // find rsg
  design->status =
    process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, design->si);

  design->verbose = verbose;

//   if ((design->status & FOUNDRSG) == 0)
//   {
//     cerr << "findsv: Deadlocked during installation of modified TER/TEL" << endl;
//     return false;
//   }

  // Update the system's pointers into atacs and the concurrency object

  // redo the concurrency table
  concurrency = svConcurrency(design);

  // redo the excitation and switching region state lists
  erStates.clear();
  srStates.clear();
  findAllRegions(back_inserter(erStates),
		 back_inserter(srStates)); // find excitation and switching

  // set the offset to the max, to force a reset on the next clearColors call
  // this is because the color field of each state is now in an unknown condition
  color_field_offset = sizeof(unsigned int) * 8;

  
  return true;
  
}

///////////////////////////////////////////////////////////////////////////
// pick the next variable name that doesn't conflict with an existing name
///////////////////////////////////////////////////////////////////////////
string
svSystem::nextCSCVariableName()
{
  bool nameConflict;
  string signalName;
  do
  {
    nameConflict = false;
    signalName = "CSC";
    // fill in the signal name
    char intAsString[20]; // oh man static buffers are weak TODO
    sprintf(intAsString,"%d",variableNumber);
    signalName += intAsString;
    for (int s = 0; s < design->nsignals && !nameConflict; s++)
    {
      string existingSignalName(design->signals[s]->name);
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
// walk the state graph, looking for the initial state
// once found, see if the CSC signal (represented by the coloring)
// should be 0 or 1. This often is overly simplistic, so the coloring
// has to be changed based on the initial marking of the state variable's
// outgoing rules. See findInitialMarking()
///////////////////////////////////////////////////////////////////////////
int
svSystem::findInitialColor() const
{
  // find the initial state and retrieve the coloring from it.
  // This assumes that the state graph in fact has been colored
  
  for (int i=0; i<PRIME; i++)
    for (stateADT curstate=design->state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
    {
      if (curstate->number == 0)
      {
// 	cerr << "In state " << curstate->state
// 	     << " where color is " << getColor(curstate) << endl;
	return getColor(curstate);
      }
    }

  return -1;
}

//***************************************************************************
// search the state graph to determine if outgoing arcs are marked initially 
//***************************************************************************
char
svSystem::findInitialMarking(const svTP& tp, const sv_event_index e) const
{
  // the current policy for initial marking is:
  // if any element of the SR intersection can be reached from ER element
  // without passing through a token, then a token will naturally arrive
  // here as part of the initial system. If no element of the SR intersection
  // can be reached without going through a token, the system will deadlock without
  // an initial token being placed on the outgoing rule as well

  // ... see if any (starting) SR event is reachable w/o traversing a token
  for (sv_event_index_cont::const_iterator i_preceedingEvent = tp.startEvents().begin();
       i_preceedingEvent != tp.startEvents().end();
       i_preceedingEvent++)
  {
      // check to see if any starting (SR) event is reachable
    if (reachable(design->cycles,
		  design->nevents,
		  design->ncycles,
		  *i_preceedingEvent, 0, // 0 is epsilon (unmarked)
		  e, 0) == 1)
    {
      return '0'; // if so, a token will get here naturally, so don't place one here
    }
  }
  return '1'; 
}

///////////////////////////////////////////////////////////////////////////
// insert a state variable into the given insertion point
///////////////////////////////////////////////////////////////////////////
bool
svSystem::insertStateVar(const svIP& ip, string signalName)
{
  // create a unique name that doesn't already exist in the system
  if (signalName.empty())
      signalName = nextCSCVariableName();
  
  bool success(true);
  string defaultExpression("");
  int defaultLowerBound(design->mingatedelay);
  int defaultUpperBound(design->maxgatedelay);
  char defaultInitialMarking('0');
  string instance("1"); // only 1 instance of a signal (TODO more would be cool)

  // extend the startstate to include another signal at its initial value
  // to find whether this should be 1 or 0, we check what the color of the
  // csc signal was in state #1, the initial state. Under some circumstances,
  // this has to be adjusted to avoid deadlock.
  char initVal = ip.initialSignalValue();
  if (initVal == '?')
  {
    cerr << "Can't determine initial signal value. "
	 << "Defaulting to 0" << endl;
    initVal = '0';
  }

  // add event to the output event list
  /* TOMOHIRO:
  textTel.addEvent(signalName, instance, false);*/
  textTel.addEvent(signalName, instance);

  // first handle the + transition insertion
  string newEventString = signalName + "+/" + instance;

  // add rules going TO the CSC+ transition
  for (sv_event_index_cont::const_iterator i_riseStarters = ip.risingTP().startEvents().begin();
       i_riseStarters != ip.risingTP().startEvents().end();
       ++i_riseStarters)
  {
    success = success && textTel.addRule(design->events[*i_riseStarters]->event,
					 newEventString,
					 defaultInitialMarking,
					 defaultExpression,
					 defaultLowerBound,
					 defaultUpperBound);
  }

  // add rules going FROM the CSC+ transition
  for (sv_event_index_cont::const_iterator i_riseEnders
	 = ip.risingTP().endEvents().begin();
       i_riseEnders != ip.risingTP().endEvents().end();
       ++i_riseEnders)
  {
    char initialMarking = findInitialMarking(ip.risingTP(),*i_riseEnders);
    if (initialMarking == '1') initVal = '1';
    success = success && textTel.addRule(newEventString,
					 design->events[*i_riseEnders]->event,
					 initialMarking,
					 defaultExpression,
					 defaultLowerBound,
					 defaultUpperBound);
  }

  // check whether witness "rules" going FROM the CSC+ transition would
  // alter the initial condition of the state signal
  for (sv_event_index_cont::const_iterator i_riseWitness
	 = ip.risingTP().witnessEvents().begin();
       i_riseWitness != ip.risingTP().witnessEvents().end();
       ++i_riseWitness)
  {
    char initialMarking = findInitialMarking(ip.risingTP(),*i_riseWitness);
    if (initialMarking == '1') initVal = '1';
  }

  // now handle the - transition
  newEventString = signalName + "-/" + instance;

  // add rules going TO the CSC- transition
  for (sv_event_index_cont::const_iterator i_fallStarters
	 = ip.fallingTP().startEvents().begin();
       i_fallStarters != ip.fallingTP().startEvents().end();
       ++i_fallStarters)
  {
    success = success && textTel.addRule(design->events[*i_fallStarters]->event,
					 newEventString,
					 defaultInitialMarking,
					 defaultExpression,
					 defaultLowerBound,
					 defaultUpperBound);
  }

  // add rules going FROM the CSC- transition
  for (sv_event_index_cont::const_iterator i_fallEnders
	 = ip.fallingTP().endEvents().begin();
       i_fallEnders != ip.fallingTP().endEvents().end();
       ++i_fallEnders)
  {
    char initialMarking = findInitialMarking(ip.fallingTP(),*i_fallEnders);
    if (initialMarking == '1') initVal = '0';
    success = success && textTel.addRule(newEventString,
					 design->events[*i_fallEnders]->event,
					 findInitialMarking(ip.fallingTP(),
							    *i_fallEnders),
					 defaultExpression,
					 defaultLowerBound,
					 defaultUpperBound);
  }

  // check whether witness "rules" going FROM the CSC- transition would
  // alter the initial condition of the state signal
  for (sv_event_index_cont::const_iterator i_fallWitness
	 = ip.fallingTP().witnessEvents().begin();
       i_fallWitness != ip.fallingTP().witnessEvents().end();
       ++i_fallWitness)
  {
    char initialMarking = findInitialMarking(ip.fallingTP(),*i_fallWitness);
    if (initialMarking == '1') initVal = '0';
  }

  // now we know what the initVal should be, so set it
  textTel.extendState(initVal);

  return success;
}

// call atacs to do state space exploration on the currently installed system
bool
svSystem::exploreStateSpace()
{
  design->status=
    process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, design->si);

  return ((design->status & FOUNDRSG) != 0);

}

// return a container of unique TPs
tps_container
svSystem::findIrredundantTPs() const
{
   tps_container TPs;
   svTP start_tp;
   sv_event_index_cont all_events;
   
   for (int i=0; i < design->nevents; i++)
   {
      cerr << "index " << i << " is " << design->events[i] << endl;
   }
   
   // start with 1 to avoid the reset event (event index = 0)
   for (int i = 1; i < design->nevents; i++)
   {
       all_events.push_back(i);
   }
   rFindIrredundantTPs(all_events, start_tp, TPs);
   
   return TPs;
}

///////////////////////////////////////////////////////////////////////////
void
svSystem::rFindIrredundantTPs(sv_event_index_cont avail_events,
			      svTP current, tps_container& TPs) const
{
   // this method must:
   // ensure that et intersect st = 0 --  v/ OK
   // not put input transitions into et -- v/ OK
   
   // all elements of st must be timed concurrent with all others v/ OK
   // all elements of et must be timed concurrent with all others v/ OK
   
   // all elements of st must NOT conflict with elements of et v/ OK
   
   // check that all elements of et are not witnesses (put them in wt instead) v/ OK
   
   if (!avail_events.empty())
   {
     // get next event
      sv_event_index e = avail_events.back();
      avail_events.pop_back();

      // Get the concurrency info for use below
      concurrency_status eventStatusWithStartSet =
	 concurrency.eventsAreConcurrent(current.startEvents(), e);
      concurrency_status eventStatusWithWitnessSet =
	concurrency.eventsAreConcurrent(current.witnessEvents(), e);
      concurrency_status eventStatusWithEndSet =
	 concurrency.eventsAreConcurrent(current.endEvents(), e);
      //
      // NOT INCLUDED CASE
      // handle case where this event does not participate
      //
      rFindIrredundantTPs(avail_events, current, TPs);
	
      //
      // START CASE
      // handle case where this event is in start set
      //
      if (eventStatusWithStartSet == eConcurrent
  	  && ! concurrency.hasConflict(current.witnessEvents(), e)
	  && ! concurrency.hasConflict(current.endEvents(), e)
	  )
      {
	 current.addStartEvent(e);
	 
	 // by adding e to the start set, the previously found witness transitions
	 // may no longer be valid. Walk through the witnesses,
	 // demoting any that no longer work from witnesses
	 // into being elements of et
	 bool prune(false);

 	 for (size_t witdex = 0;
 	      witdex < current.witnessEvents().size() && !prune;
 	      witdex++)
 	 {
 	    sv_event_index witness_event = current.witnessEvents()[witdex];
	    
 	    // no longer a witness ?
 	    if ( !concurrency.isWitness(e, witness_event) )
 	    {
	      // pull out of witnesses
	      current.eraseNthWitnessEvent(witdex);
	      witdex--;

 	       // add to et if an output
 	       if (isOutput(witness_event)
//  		   && eventStatusWithEndSet == eConcurrent
//  		   && eventStatusWithWitnessSet == eConcurrent)
		   )
 	       {
   		  current.addEndEvent(witness_event);
 	       }
  	       else // an input, this whole tree is invalid
  	       {
  		  prune=true; // this event is no longer a witness,
 		              // and can't be in the end set
  		              // so prune this branch of exploration
  	       }
 	    } // end if
 	 } // end for

 	 if (!prune) rFindIrredundantTPs(avail_events, current, TPs);
	 current.popLastStartEvent();
      } // end if ok for start set

      // STOP CASE
      // handle case where this event is in the end set
      // only put output events into the end set, as
      // an input signal here may be delayed by the state signal,
      // which would violate the environment relationship

      if (eventStatusWithEndSet == eConcurrent
  	  && eventStatusWithWitnessSet == eConcurrent
	  && ! concurrency.hasConflict(current.startEvents(), e) )
      {
	 //
	 // at this point, we know that this event is timed concurrent
	 // with the other end and witness events
	 //

	 // can it be a witness, or must it be in the et?
  	 if (concurrency.isWitness(current.startEvents(), e))
  	 {
  	   current.addWitnessEvent(e);
  	   rFindIrredundantTPs(avail_events, current, TPs);
  	 }
	 // nope, must be in et, so make sure not an input event
  	 else if (isOutput(e))
	 {
	   current.addEndEvent(e);
	   rFindIrredundantTPs(avail_events, current, TPs);
	 }
 	 else
 	   {
//  	     cerr << "found something that should be in wt/et but dumping it" << endl;
//  	     cerr << "current is " << current << endl;
//  	     cerr << "e is " << e << endl;
 	   }
      }
//       else
//       {
//  	cerr << "++++ event " << e << " not in end/wit due to concurrency/conflict" << endl;
//  	cerr << "The current situation is " << endl << current << endl;
//  	cerr << "eventStatuswithEnd is " << eventStatusWithEndSet << endl;
//  	cerr << "eventStatusWithWit is " << eventStatusWithWitnessSet << endl;
//  	cerr << "eventStatuswithstart is " << eventStatusWithStartSet << endl;
//       }
   } // end if events avail

   else // no more events, thus this is a complete TP
   {
     if (current.hasStartEvents()
	 && (current.hasEndEvents() || current.hasWitnessEvents()) )
	                            // there are 2^n pathelogic cases
      {                             // in which the start set is empty,
	                            // where n is number of transitions
	                            // there must also be some type of end
	 for (sv_event_index_cont::const_iterator end_it =
		current.endEvents().begin();
	      end_it < current.endEvents().end(); end_it++)
	 {
	   // is this end event really better as a witness ?
	   if (concurrency.isWitness(current.startEvents(), *end_it) )
	   {
	     current.addWitnessEvent(*end_it);
	   }

	 }

	TPs.push_back(current);
      }
   }
}

///////////////////////////////////////////////////////////////////////////
string
svSystem::stateVal(const string& enabling) const
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
int
svSystem::coloredState(int color) const
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
// returns the number of state code violations
pair<int, int>
svSystem::violations(bool considerColoring) const
{
   stateADT* state_table = design->state_table;
   int ninpsig = design->ninpsig;
   int natural(0), caused(0);

   for (int i=0; i < PRIME; i++) // for each hash bin
   {
      // for each state in the hash bin
      for (stateADT curState=state_table[i];
	   curState != NULL && curState->state != NULL;
	   curState=curState->link)
      {
	 // construct a string for this state code
	 string curString(curState->state);
	 
	 for (stateADT someState=curState->link;
	      someState != NULL && someState->state != NULL;
	      someState=someState->link)
	 {
	    // make strings for someState's state code
	    string someString(someState->state);
	    
	    // if curstate's state code (as 0/1, not enablings) matches somestate's
	    if (stateVal(curString) == stateVal(someString))
	    {
	       // CSC ?
	       // check _output_ enablings for disparity
	       if (curString.substr(ninpsig, string::npos)
		   != someString.substr(ninpsig, string::npos))
		  // violation found
	       {
		  // check colorings for possible save
		  if ( ! considerColoring
		       ||
		       // out enablings differ, but so does coloring
		       !(
			 (getColor(someState)==COLOR_1
			  && getColor(curState)==COLOR_0)
			 ||
			 (getColor(someState)==COLOR_0
			  && getColor(curState)==COLOR_1)
			 )
		      )
		  {
		     // no save or color not considered, so CSC violation unresolved
// 		    cerr << "Coloring couldn't save " << curString << " "
// 			 << someString << endl;
		     natural++;
		  }
// 		  else
// 		  {
// 		    cerr << "Coloring saved " << curString << " "
// 			 << someString << endl;
// 		  }
	       }
	       // else if output enablings did match, check USC->CSC
	       else // USC ?
	       {
		 // if state (ie 0/1) of the state var, as predicted by coloring
		 // is the same in both states, and the enablings don't match
		 if ( considerColoring
		      &&
		      (coloredState(getColor(curState))
		       == coloredState(getColor(someState))
		       && getColor(curState) != getColor(someState)) )
		 {
		   caused++;
// 		    cerr << "Coloring caused " << curString << " "
// 			 << someString << endl;
		 }
	       } // end if USC
	    } // end if potential problem
	 } // end for other states in bin
      } // end for each state in bin
   } // end for each bin
   
  return make_pair(natural, caused);

} // end function

//=========================================================================
// class svSolution
//=========================================================================
svSolution::svSolution(const svSystem& s)
   : svSystem(s)
{
}

// TODO this is the solution cost function TODO
unsigned int
svSolution::cost() const
{
   return 0;
}
















