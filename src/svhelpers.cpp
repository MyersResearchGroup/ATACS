#include "svhelpers.h"
#include "stateasgn.h"
#include <algorithm>
#include <iterator>

///////////////////////////////////////////////////////////////////////////
// CStateVariableInfo
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// standard constructor
///////////////////////////////////////////////////////////////////////////
CStateVariableInfo::CStateVariableInfo(const string& sigName,
				       const string& inst,
				       const CInsertionPoint& vip,
				       eventADT events[], int nevents,
				       int lowerBound,
				       int upperBound,
				       cycleADT**** cycles, int ncycles)
  : signalName(sigName), instance(inst), initialValue(vip.initialValue)
{
  CRuleInfo ruleInfo;

  // set all the common fields of the ruleInfo object
  // these values apply to rules leading TO a transition on the sv
  // since only outgoing arcs are initially marked, these are all 0
  ruleInfo.initialMarking = '0';
  // for now, no expression is supported
  // if there is an expression on an arc, this arc will be in parallel
  // with it so the expression will be preserved
  ruleInfo.expression = "";
  // set the lowerBound to the passed in lowerBound value (usually 0)
  ruleInfo.lowerBound = lowerBound;
  // set the upperBound to the passed in upperBound value (usually defaultgatedelay)
  ruleInfo.upperBound = upperBound;
  // insert the rule into the CStateVariableInfo rulesToRise
  // for each event in the riseStart
  for (event_index_cont::const_iterator i_riseStart = vip.riseStart.begin();
       i_riseStart != vip.riseStart.end(); i_riseStart++)
    {
      // modify the ruleInfo object to reflect this particular rule
      ruleInfo.event = events[*i_riseStart];
      rulesToRise.push_back(ruleInfo);
    }

  // for each event in the fallStart
  for (event_index_cont::const_iterator i_fallStart = vip.fallStart.begin();
       i_fallStart != vip.fallStart.end(); i_fallStart++)
    {
      // modify the ruleInfo object to reflect this particular rule
      ruleInfo.event = events[*i_fallStart];
      rulesToFall.push_back(ruleInfo);
    }

  // now we need to consider the ending events (those following the sv transition)
  // these are trickier because they may be initially marked and require unique timing

  // for each event in the riseEnd
  for (event_index_cont::const_iterator i_riseEnd = vip.riseEnd.begin();
       i_riseEnd != vip.riseEnd.end(); i_riseEnd++)
    {
      // modify the ruleInfo object to reflect this particular rule
      ruleInfo.event = events[*i_riseEnd];
      // set the lowerBound 
      ruleInfo.lowerBound = events[*i_riseEnd]->lower;
      // set the upperBound 
      ruleInfo.upperBound = events[*i_riseEnd]->upper;

      ruleInfo.initialMarking =
	svInitialMarking(cycles, ncycles, nevents, vip.riseStart, *i_riseEnd);
      rulesFromRise.push_back(ruleInfo);
      // if we initially mark as if this transition had happened, set the initial state
      // to also reflect that this has happened. Otherwise we get "trying to go from
      // 0 to 0" errors
      if (ruleInfo.initialMarking == '1')
	{
	  initialValue = '1';
	}
    }

  // for each event in the fallEnd
  for (event_index_cont::const_iterator i_fallEnd = vip.fallEnd.begin();
       i_fallEnd != vip.fallEnd.end(); i_fallEnd++)
    {
      // modify the ruleInfo object to reflect this particular rule
      ruleInfo.event = events[*i_fallEnd];
      // set the lowerBound 
      ruleInfo.lowerBound = events[*i_fallEnd]->lower;
      // set the upperBound 
      ruleInfo.upperBound = events[*i_fallEnd]->upper;
      ruleInfo.initialMarking =
	svInitialMarking(cycles, ncycles, nevents, vip.fallStart, *i_fallEnd);
      rulesFromFall.push_back(ruleInfo);
      // if we initially mark as if this transition has happened, set the initial state
      // to also reflect that this has happened. Otherwise we get "trying to go from
      // 1 to 1" errors
      if (ruleInfo.initialMarking == '1')
	{
	  initialValue = '0';
	}
    }

  // now, for something I think you'll really enjoy
  // let's check the witnesses to see if we need to change the initial marking
  // based on initially marking arcs which aren't there due to timing
  // for each event in the riseWitness set
//   for (event_index_cont::const_iterator i_riseWitness = vip.riseWitnesses.begin();
//        i_riseWitness != vip.riseWitnesses.end(); i_riseWitness++)
//     {
//       if (svInitialMarking(cycles, ncycles, nevents, vip.riseStart, *i_riseWitness) == '1')
// 	{
// 	  //	  cout << "Initial Value changed to high due to witness testimony" << endl;
// 	  //	  initialValue = '1';
// 	}
//     }
//   // for each event in the fallWitness set
//   for (event_index_cont::const_iterator i_fallWitness = vip.fallWitnesses.begin();
//        i_fallWitness != vip.fallWitnesses.end(); i_fallWitness++)
//     {
//       if (svInitialMarking(cycles, ncycles, nevents, vip.riseStart, *i_fallWitness) == '1')
// 	{
// 	  //	  cout << "Initial Value changed to low due to witness testimony" << endl;
// 	  //	  initialValue = '0';
// 	}
//     }

}
///////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////
// CInsertionPoint
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// return true if the two transition points in this insertion point can coexist
///////////////////////////////////////////////////////////////////////////////
bool CInsertionPoint::isValidInsertionPoint(eventADT* events)
{
  event_index_cont intersection;
  back_insert_iterator<event_index_cont> bii_intersection(intersection);

  for(event_index_cont::size_type i = 0; i < riseStart.size() ; ++i)
    for(event_index_cont::size_type j = 0; j < fallStart.size() ; ++j)
      if(events[riseStart[i]]->process != events[fallStart[j]]->process)
	return false;

  
  /*  for(int i = 0; i < riseEnd.size() ; ++i)
    for(int j = 0; j < fallEnd.size() ; ++j)
      if(events[riseEnd[i]]->process != events[fallEnd[j]]->process)
	return false;
  */
  // this needs to be redone so that it will be faster
  // it does work for the moment
  // maybe we need to do order-preserving inserts
  sort(riseStart.begin(),riseStart.end());
  sort(riseEnd.begin(),riseEnd.end());
  sort(fallStart.begin(),fallStart.end());
  sort(fallEnd.begin(),fallEnd.end());


  // if rise and fall start after the same events, reject
  // in other words, if SR(+) intersect SR(-) != null set, reject
  set_intersection(riseStart.begin(),
		   riseStart.end(),
		   fallStart.begin(),
		   fallStart.end(),
		   bii_intersection);

  if ( ! intersection.empty()) return false;
  
  // if ER(+) intersect ER(-) != null set, reject
  // in other words, if rise and fall end after the same events, reject
  set_intersection(riseEnd.begin(),
		   riseEnd.end(),
		   fallEnd.begin(),
		   fallEnd.end(),
		   bii_intersection);

  return ( intersection.empty() );
}
///////////////////////////////////////////////////////////////////////////
// stream operator for insertion points (should convert to event names)
ostream& operator<<(ostream& outstream, const CInsertionPoint& vip)
{
  copy(vip.riseStart.begin(), vip.riseStart.end(),
       ostream_iterator<int>(outstream, " "));
  outstream << "<-> ";
  copy(vip.riseEnd.begin(), vip.riseEnd.end(),
       ostream_iterator<int>(outstream, " "));
  outstream << " ===== ";
  copy(vip.fallStart.begin(), vip.fallStart.end(),
       ostream_iterator<int>(outstream, " "));
  outstream << "<-> ";
  copy(vip.fallEnd.begin(), vip.fallEnd.end(),
       ostream_iterator<int>(outstream, " "));
  outstream << ": " << vip.naturalViolations << "+" << vip.causedViolations;
  outstream << " [" << vip.initialValue << "]";
  

  return outstream;
}
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// CTreeNode
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Default constructor for CTreeNode
///////////////////////////////////////////////////////////////////////////
CTreeNode::CTreeNode()
  :startEvents(), endEvents(), availableEvents()
{
}
///////////////////////////////////////////////////////////////////////////
// Copy constructor for CTreeNode
///////////////////////////////////////////////////////////////////////////
CTreeNode::CTreeNode(const CTreeNode& a)
  : startEvents(a.startEvents), endEvents(a.endEvents),
    availableEvents(a.availableEvents)
{
}
///////////////////////////////////////////////////////////////////////////
bool CTreeNode::isTerminal()
{
  return (availableEvents.empty()); // if no growth is possible, must be terminal
}
///////////////////////////////////////////////////////////////////////////
bool CTreeNode::isValidTerminalNode() // not empty start
{
  return (! startEvents.empty() );
}

///////////////////////////////////////////////////////////////////////////
bool CTreeNode::isValidNode(CConcurrency& concurrency)
{
  // no element of start can be an element of end
  // in other words, if ER intersect SR != null set, reject
  // this above constraint is met by the way the queue is fed

  // no two elements of start can conflict with each other
  // no two elements of end can conflict with each other
  // the above two constraints are checked as part of isConcurrent

  // no element of start # any element of end
  if (concurrency.conflict(startEvents, endEvents)) return false;

  // all elements of start must be concurrent (not #, concurrently enabled)
  if (! concurrency.isConcurrent(startEvents)) return false;

  // all elements of end must be concurrent (not #, concurrently enabled)
  if (! concurrency.isConcurrent(endEvents)) return false;

  return ( true );
}
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// CIPGenerator
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Default constructor for CTreeNode
///////////////////////////////////////////////////////////////////////////
CIPGenerator::CIPGenerator()
{
}
///////////////////////////////////////////////////////////////////////////
CIPGenerator::CIPGenerator(int nevents, int ninpsigs, eventADT theEvents[],
			   stateADT state_table[], ruleADT *rules[])
  : concurrency(nevents, state_table, rules), events(theEvents),nInputSignals(ninpsigs),nevents(nevents)
{
  // iterate through the n events, excluding the reset (n==0) event
  // and add their index to an event_index_cont
  for (int i = 1; i < nevents; i++)
    allEvents.push_back(i);
  prime(riseQueue);
  prime(fallQueue);
}
///////////////////////////////////////////////////////////////////////////
CIPGenerator::~CIPGenerator()
{
}
///////////////////////////////////////////////////////////////////////////
void CIPGenerator::prime(treeQueue& theQueue)
{
  CTreeNode seedNode;
  seedNode.available() = allEvents;
  theQueue.push_back(seedNode);
}
///////////////////////////////////////////////////////////////////////////
bool CIPGenerator::nextValidInsertionPoint(CInsertionPoint& ip)
{
  bool done(false);

  do // keep picking fall and rise until they are compatible or we're done
    {

      // finished all combos with last rise ...
#ifdef __SIMP_INS__
      if ( nextValidTransitionPoint2(false, ip.fallStart, ip.fallEnd) )
	{ // ... so get a new rise point
	  done = nextValidTransitionPoint2(true, ip.riseStart, ip.riseEnd);
	}
#else
      if ( nextValidTransitionPoint(false, ip.fallStart, ip.fallEnd) )
	{ // ... so get a new rise point
	  done = nextValidTransitionPoint(true, ip.riseStart, ip.riseEnd);
	}
#endif
      
    } while ( !done && ! ip.isValidInsertionPoint(events)); // & isValidforTel(ip) );

  return (! done); 
}


bool CTreeNode::isValidforTel(eventADT* events) {
  
  // MY CODE 
  for(event_index_cont::size_type i = 0; i < start().size(); ++i)
    for(event_index_cont::size_type j = 0;j < finish().size(); ++j) {
      
      if(events[start()[i]]->process != events[finish()[j]]->process)
      	return false;
    }
  
  return true;

  /*   for(int i = 0; i < ip.riseStart.size(); ++i)
	for(int j = 0; j < ip.fallStart.size(); ++j)
	if(events[ip.riseStart[i]]->process != events[ip.fallStart[j]]->process)
	return false;
    return true;
  */
}


///////////////////////////////////////////////////////////////////////////
bool CIPGenerator::nextValidTransitionPoint(bool isRise,
					    event_index_cont& start,
					    event_index_cont& end)
{
 treeQueue& theQueue = (isRise) ? riseQueue : fallQueue;
  CTreeNode node;
  bool cycled(false);

  /*deque<CTreeNode>::iterator ii ;
  
  //  cout << "The size of the queue is " << theQueue.size() << endl; 

    for(ii = theQueue.begin();ii!= theQueue.end(); ++ii) {
    
    for(int j = 0; j < ii->start().size(); ++j)
      cout << (ii->start())[j] << " " ;
    cout <<  endl;
    
    for(int j = 0; j < ii->finish().size(); ++j)
      cout << (ii->finish())[j] << " " ;
    cout <<  endl;
    
    for(int j = 0; j < ii->available().size(); ++j)
      cout << (ii->available())[j] << " " ;
    cout <<  endl;

  }
  
  */
    
    do
      {
	if (theQueue.empty()) // if we drained the queue, refill it
	  {
	    prime(theQueue);
	    cycled = true;
	  }
	
	while (!theQueue.empty())  
	  {
	    //    if(theQueue.size() == 72)
	    //cout << "Here we are \n";
	    node = theQueue.front(); // look at a node in the queue
	    theQueue.pop_front(); // remove it from the queue
	    
	    //	    cout << "The size of the queue is " << theQueue.size() << endl; 
	    
	    //THESE ARE MY COMMENTS FROM HERE ON
	    /*     
	    for(int i = 0; i < node.availableEvents.size() ; ++i) {
	      cout << node.availableEvents[i] <<  "  " << endl;
	      cout << events[i]->event << "  " << endl;
	      }*/
	    /* TOMOHIRO: Uncomment to restrict size 
	    if ((node.startEvents.size() > 1)||
	    (node.endEvents.size() > 1)) continue; */

	    if ( node.isTerminal() ) // if terminal node
	      {
		/* TOMOHIRO: also uncomment to restrict size
		if ((node.startEvents.size() != 1)||
		(node.endEvents.size() != 1)) continue; */

		/*
		cout << "START: ";
		for(int i = 0; i < node.startEvents.size() ; ++i)
		  cout << events[node.startEvents[i]]->event <<  "  ";
		  for(int i = 0; i < node.availableEvents.size() ; ++i)
		  cout << node.availableEvents[i] <<  "  " << endl;
		cout << "END: ";
		for(int i = 0; i < node.endEvents.size() ; ++i)
		  cout << events[node.endEvents[i]]->event <<  "  ";
		*/
		if ( node.isValidTerminalNode() && node.isValidforTel(events)) // if valid solution
		  {
		    //		    cout << "VALID" << endl;
		    start = node.start();
		    end   = node.finish();

		    return cycled;
		  }
		//		cout << "INVALID" << endl;
	      }
	    else // not a terminal node
	      {
		
		// take the last event from the available events
		
		int flag = 1,newEventIndex;

		// MY CODE 
		while(flag) {
		  newEventIndex = node.available().back();
		  node.available().pop_back(); // and erase it
		  if(events[newEventIndex]->event[0] != '$')
		    flag = 0;

		    } 

		//newEventIndex = node.available().back();
		//node.available().pop_back(); // and erase it
		CTreeNode newNode(node); // copy node
		// create left node (start with the new event)
		newNode.start().push_back(newEventIndex);
		if (newNode.isValidNode(concurrency))
		  {
		    theQueue.push_back(newNode);
		  }
		// create the middle node (new event omitted)
		if (node.isValidNode(concurrency))
		  {
		    theQueue.push_back(node);
		  }
		// create the right node (new event in end set)
		newNode = node;
		newNode.finish().push_back(newEventIndex);
                // only use outputs here
		if ((events[newEventIndex]->signal >= nInputSignals) || 
		    (events[newEventIndex]->type & NONINP)) 
		  if (newNode.isValidNode(concurrency))
		    {
		      theQueue.push_back(newNode);
		    }
	      }
	} // end while
	
      } while ( ! cycled );
    
    //  cout << "There are no valid transition points in the system" << endl;
    
    return cycled;
}
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
bool CIPGenerator::nextValidTransitionPoint2(bool isRise,
					     event_index_cont& start,
					     event_index_cont& end)
{
 treeQueue& theQueue = (isRise) ? riseQueue : fallQueue;
  CTreeNode node;
  bool cycled(false);
  do
    {
      if (theQueue.empty()) // if we drained the queue, refill it
	{
	  prime(theQueue);
	  cycled = true;
	}
      while (!theQueue.empty())  
	{
	  node = theQueue.front(); // look at a node in the queue
	  theQueue.pop_front(); // remove it from the queue
	  if ( node.isTerminal() ) // if terminal node
	    {
	      /*
		cout << "START: ";
		for(int i = 0; i < node.startEvents.size() ; ++i)
		  cout << events[node.startEvents[i]]->event <<  "  ";
		  for(int i = 0; i < node.availableEvents.size() ; ++i)
		  cout << node.availableEvents[i] <<  "  " << endl;
		cout << "END: ";
		for(int i = 0; i < node.endEvents.size() ; ++i)
		  cout << events[node.endEvents[i]]->event <<  "  ";
		cout << endl;
		cout << "AVAILABLE: ";
		  for(int i = 0; i < node.availableEvents.size() ; ++i) {
	      cout << node.availableEvents[i] <<  "  " << endl;
	      cout << events[i]->event << "  " << endl;
	      }*/
	      if ( node.isValidTerminalNode() && 
		   node.isValidforTel(events)) // if valid solution
		{
		  start = node.start();
		  end   = node.finish();
		  return cycled;
		}
	    }
	  else // not a terminal node
	    {
	      // take the last event from the available events
	      int flag = 1,newEventIndex,newEventIndex2;
	      // MY CODE 
	      while(flag) {
		newEventIndex = node.available().back();
		node.available().pop_back(); // and erase it
		if(events[newEventIndex]->event[0] != '$')
		  flag = 0;
	      } 
	      CTreeNode newNode(node); // copy node
	      // create left node (start with the new event)
	      newNode.start().push_back(newEventIndex);
	      if (newNode.isValidNode(concurrency))
		{
		  while (!(newNode.isTerminal())) {
		    newEventIndex2=newNode.available().back();
		    newNode.available().pop_back();
		  }
		  for (newEventIndex2=1;newEventIndex2<nevents;
		       newEventIndex2++) {
		    if ((newEventIndex!=newEventIndex2) &&
			(concurrency.isRule(newEventIndex,newEventIndex2))&&
			((events[newEventIndex2]->signal >= nInputSignals) || 
			 (events[newEventIndex2]->type & NONINP))) 
		      newNode.finish().push_back(newEventIndex2);
		  }
		  theQueue.push_back(newNode);
		}
	      // create the middle node (new event omitted)
	      if (node.isValidNode(concurrency))
		{
		  theQueue.push_back(node);
		}
	      // create the right node (new event in end set)
	      newNode = node;
	      newNode.finish().push_back(newEventIndex);
	      // only use outputs here
	      if ((events[newEventIndex]->signal >= nInputSignals) || 
		  (events[newEventIndex]->type & NONINP)) 
		if (newNode.isValidNode(concurrency))
		  {
		    while (!(newNode.isTerminal())) {
		      newEventIndex2=newNode.available().back();
		      newNode.available().pop_back();
		    }
		    for (newEventIndex2=1;newEventIndex2<nevents;
			 newEventIndex2++) {
		      if ((newEventIndex!=newEventIndex2) &&
			  concurrency.isRule(newEventIndex2,newEventIndex))
			newNode.start().push_back(newEventIndex2);
		    }
		    theQueue.push_back(newNode);
		  }
	    }
	} // end while
	
    } while ( ! cycled );
  return cycled;
}
///////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////
// CBestSolutions
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// constructor
CBestSolutions::CBestSolutions(int numberOfInitialViolations, int numberOfSolutionsToKeep,
			       const CTextTel& theTel, eventADT theEvents[])
  : lowerViolationBound(numberOfInitialViolations),
    upperViolationBound(numberOfInitialViolations),
    maxNumberOfSolutions(numberOfSolutionsToKeep),
    textTel(theTel), events(theEvents)
{
}
///////////////////////////////////////////////////////////////////////////
// copy constructor
CBestSolutions::CBestSolutions(const CBestSolutions& b) 
  :     solutions(b.solutions),
	lowerViolationBound(b.lowerViolationBound),
	upperViolationBound(b.upperViolationBound),
	maxNumberOfSolutions(b.maxNumberOfSolutions),
	textTel(b.textTel), events(b.events)
{
}
///////////////////////////////////////////////////////////////////////////
// operator =
CBestSolutions& CBestSolutions::operator=( const CBestSolutions& b )
 {
   if ( this == &b ) return( *this );
   lowerViolationBound = b.lowerViolationBound;
   upperViolationBound = b.upperViolationBound;
   maxNumberOfSolutions = b.maxNumberOfSolutions;
   solutions = b.solutions;
   textTel = b.textTel;
   events = b.events;

   return( *this );
 }
///////////////////////////////////////////////////////////////////////////
// return the tel for which these solutions are valid
const CTextTel& CBestSolutions::revertTel()
{
  return textTel;
}
///////////////////////////////////////////////////////////////////////////
// return the best solution which hasn't been taken yet
CInsertionPoint CBestSolutions::nextBestSolution()
{
  // the operation is basically a rotate, then return the former first entry
  solutions.push_back(solutions.front());
  solutions.pop_front();
  return solutions.back();
}
///////////////////////////////////////////////////////////////////////////
// return all solutions
solution_list& CBestSolutions::allSolutions()
{
  return solutions;
}
///////////////////////////////////////////////////////////////////////////
int CBestSolutions::size()
{
  return solutions.size();
}

///////////////////////////////////////////////////////////////////////////
// return the total number of violations in the now best solution
int CBestSolutions::nextTotalViolations()
{
  return (solutions.front().causedViolations +
	  solutions.front().naturalViolations);
}

///////////////////////////////////////////////////////////////////////////
// look at this solution and keep it if it deserves consideration
// return true if the solution was kept
bool CBestSolutions::consider(CInsertionPoint& solution)
{
  int totalViolations = solution.naturalViolations + solution.causedViolations;
  bool isKeeper = false;
  solution_list::iterator i_insertSpot;

  // if this solution is just not very good, and we have enough good sols, 
  //   reject it right off
  //if (totalViolations > upperViolationBound && solutions.size() == 
  //   maxNumberOfSolutions) return false;

  // now see if this solution is <, in which case we insert, or == in which 
  // case we think some more
  for (solution_list::iterator i_goodSolution = solutions.begin();
       i_goodSolution != solutions.end() && !isKeeper; i_goodSolution++)
    {
      if (totalViolations < (*i_goodSolution).naturalViolations +
	  (*i_goodSolution).causedViolations )
	{
	  i_insertSpot = i_goodSolution;
	  isKeeper = true;
	}
      else 	    // SECONDARY COST FUNCTION 
	if (totalViolations == (*i_goodSolution).naturalViolations
	    + (*i_goodSolution).causedViolations )
	  {
	    // need to consider less significant things, e.g.:
	    // ... some of the CSC's are caused, and should be easy to solve???
	    if ((*i_goodSolution).causedViolations < solution.causedViolations)
	      {
//cout << *i_goodSolution << " bumped by more caused " << solution << endl;
		i_insertSpot = i_goodSolution;
		isKeeper = true;
	      }
	    // ... have fewer elements in the ER intersection set
	    else if ((*i_goodSolution).causedViolations == 
		     solution.causedViolations
		     && solution.riseEnd.size() + solution.fallEnd.size()
		     < ( (*i_goodSolution).riseEnd.size() + 
			 (*i_goodSolution).fallEnd.size() ) )
	      {
//	cout << *i_goodSolution << " bumped by smaller " << solution << endl;
		i_insertSpot = i_goodSolution;
		isKeeper = true;
	      }
	    // ... have fewer elements in the SR intersection set
	    else if ((*i_goodSolution).causedViolations == 
		     solution.causedViolations
		     && solution.riseEnd.size() + solution.fallEnd.size()
		     == ( (*i_goodSolution).riseEnd.size() + 
			  (*i_goodSolution).fallEnd.size() ) 
		     && (solution.riseStart.size() + solution.fallStart.size())
		     < ( (*i_goodSolution).riseStart.size() + 
			 (*i_goodSolution).fallStart.size() ) )
	      {
//	cout << *i_goodSolution << " bumped by smaller " << solution << endl;
		i_insertSpot = i_goodSolution;
		isKeeper = true;
	      }
	    // have an initial value of low for the inserted state signal?
	    // others must be considered as well
	  }
    } // end for

  if ( !isKeeper && solutions.size() < maxNumberOfSolutions)
    {
      //      cout << solution << " used initially" << endl;
      i_insertSpot = solutions.end();
      isKeeper = true;
    }
  
  if (isKeeper)
    {
      //cout << solution << endl; 
      // stream the solutions down the screen for now
      /*
      for (event_index_cont::iterator i_rs = solution.riseStart.begin();
	   i_rs != solution.riseStart.end(); i_rs++)
	{
	  cout << events[*i_rs]->event << " ";
	}
      cout << "<-> ";
      for (event_index_cont::iterator i_re = solution.riseEnd.begin();
	   i_re != solution.riseEnd.end(); i_re++)
	{
	  cout << events[*i_re]->event << " ";
	}
      cout << " ===== ";
      for (event_index_cont::iterator i_fs = solution.fallStart.begin();
	   i_fs != solution.fallStart.end(); i_fs++)
	{
	  cout << events[*i_fs]->event << " ";
	}
      cout << "<-> ";
      for (event_index_cont::iterator i_fe = solution.fallEnd.begin();
	   i_fe != solution.fallEnd.end(); i_fe++)
	{
	  cout << events[*i_fe]->event << " ";
	}
      cout << ": " << solution.naturalViolations << "+" 
      << solution.causedViolations;
      cout << " [" << solution.initialValue << "]";
      cout << endl;
      */      
      solutions.insert(i_insertSpot, solution); // add this solution
      if (solutions.size() > maxNumberOfSolutions)
	solutions.pop_back(); // get rid of the now-not-good-enough solution
      upperViolationBound = solutions.back().naturalViolations
	+ solutions.back().causedViolations;
      lowerViolationBound = solutions.front().naturalViolations
	+ solutions.front().causedViolations;
      return true;
    }

  return false;
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////
// CConcurrency
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Default constructor
CConcurrency::CConcurrency()
{
}

///////////////////////////////////////////////////////////////////////////////
// standard constructor
CConcurrency::CConcurrency(int nevents, stateADT state_table[], ruleADT *rules_matrix[])
  : rules(rules_matrix)
{

  // create an empty concurrencyTable
  concurrencyRow emptyRow;
  for (int j=0; j < nevents; j++)
    {
      concurrencyTable.push_back(emptyRow);
    }

  for (int i=0; i < PRIME; i++) // for every state bin
    for (stateADT curstate = state_table[i]; // for every state in bin
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      {
	event_index_cont concurrentlyEnabledEvents;
	for (statelistADT nextNode = curstate->succ; // for every next state from this state
	     nextNode != NULL;  nextNode = nextNode->next)
	  { // add the event to the list of events which are concurrent in this state
	    concurrentlyEnabledEvents.push_back(nextNode->enabled);
	  }

	// if there are concurrent events, fill in the table accordingly
	if (concurrentlyEnabledEvents.size() > 1)
	  {
	    for (concurrencyRow::iterator i_firstEvent = concurrentlyEnabledEvents.begin();
		 i_firstEvent != concurrentlyEnabledEvents.end(); i_firstEvent++)
	      {
		for (concurrencyRow::iterator i_secondEvent= concurrentlyEnabledEvents.begin();
		     i_secondEvent != concurrentlyEnabledEvents.end(); i_secondEvent++)
		  {
		    if (i_firstEvent != i_secondEvent)
		      {
			// make this unique later
			concurrencyTable[*i_firstEvent].push_back(*i_secondEvent);
		      }
		  }
	      }
	  }
      }
}
///////////////////////////////////////////////////////////////////////////////
bool CConcurrency::isConcurrent(event_index_cont& events)
{
  for (unsigned int i = 0; i < events.size(); i++) // for each event
    {
      for (unsigned int j = i; j < events.size(); j++) // for each other event
	{
	  if ( i != j)
	    {
	      // handle explict conflict
	      if (rules[events[i]][events[j]]->conflict) return false;

	      // check timing based concurrency
	      if (find(concurrencyTable[events[i]].begin(),// if not in table
		       concurrencyTable[events[i]].end(), events[j])
		  == concurrencyTable[events[i]].end()) 
		{
		  return false;
		}
	    }
	}
    }
  return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CConcurrency::conflict(const event_index_cont& startEvents, const event_index_cont& endEvents)
{
  event_index_cont allEvents(startEvents);
  for (event_index_cont::const_iterator i_endEvent = endEvents.begin();
       i_endEvent != endEvents.end(); i_endEvent++)
    {
      allEvents.push_back(*i_endEvent);
    }
  
  for (unsigned int i = 0; i < allEvents.size(); i++) // for each event
    {
      for (unsigned int j = i; j < allEvents.size(); j++) // for each other event
	{
	  if ( i != j)
	    {
	      if (rules[allEvents[i]][allEvents[j]]->conflict) return true;
	    }
	}
    }
  return false;
}

bool CConcurrency::isRule(int event1,int event2)
{
  return (rules[event1][event2]->ruletype & SPECIFIED);
}

///////////////////////////////////////////////////////////////////////////////
// following code dumps out the concurrency table
ostream& operator<<(ostream& outstream, const CConcurrency& c)
{
  int i = 1;
  outstream << "Concurrency Table" << endl;
  for(concurrencyMatrix::const_iterator i_row = c.concurrencyTable.begin();
      i_row != c.concurrencyTable.end(); i_row++)
    {
      outstream << i++ << ": ";
      copy((*i_row).begin(), (*i_row).end(), ostream_iterator<int>(outstream, " "));
      outstream << endl;
    }
  return outstream;
}
///////////////////////////////////////////////////////////////////////////////
