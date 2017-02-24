// State Variable Helper Classes header file
#ifndef _svhelpers_h
#define _svhelpers_h

#include "struct.h"
#include <list>
#include <vector>
#include <functional>
#include <string>
#include <deque>
#include "CTextTel.h"
#include "svcommon.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
typedef list<CInsertionPoint> solution_list;
class CBestSolutions
{
private:
  solution_list solutions;
  int lowerViolationBound, upperViolationBound;
  unsigned int maxNumberOfSolutions;
  CTextTel textTel;
  eventADT* events;

public:
  CBestSolutions(int numberOfInitialViolations, int numberOfSolutionsToKeep,
		 const CTextTel& tel, eventADT events[]);
  CBestSolutions(const CBestSolutions& b);
  CBestSolutions& operator=( const CBestSolutions& b);  
  CInsertionPoint nextBestSolution();
  solution_list& allSolutions();

  bool consider(CInsertionPoint& solution);
  int size();
  const CTextTel& revertTel();
  int nextTotalViolations();
};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
typedef vector<int> concurrencyRow;
typedef vector< concurrencyRow > concurrencyMatrix;
class CConcurrency
{
private:
  concurrencyMatrix concurrencyTable;
  ruleADT **rules;

public:
  CConcurrency();
  CConcurrency(int nevents, stateADT state_table[], ruleADT *rules_matrix[]);

  bool isConcurrent(event_index_cont& events);
  bool conflict(const event_index_cont& startEvents, const event_index_cont& endEvents);
  bool isRule(int event1,int event2);

  friend ostream& operator<<(ostream& outstream, const CConcurrency& c);
};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class CTreeNode
{
 public:
  event_index_cont startEvents;  
  event_index_cont endEvents;
  event_index_cont availableEvents;

public:
  CTreeNode();
  CTreeNode(const CTreeNode &other);
  event_index_cont& start() { return startEvents; }
  event_index_cont& finish() { return endEvents; }
  event_index_cont& available() { return availableEvents; }

  bool isTerminal(); // no more available events for growth, therefore terminal
  bool isValidTerminalNode(); // not empty start, etc
  bool isValidNode(CConcurrency& concurrency); // concurrent, no # problems, etc
  bool isValidforTel(eventADT* events);
};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
typedef deque<CTreeNode> treeQueue;  
class CIPGenerator
{
private:
  event_index_cont allEvents;
  treeQueue riseQueue;
  treeQueue fallQueue;
  CConcurrency concurrency;
  eventADT* events;
  int nInputSignals;
  int nevents;

public:
  CIPGenerator();    
  CIPGenerator(int nevents, int ninpsigs, eventADT events[], stateADT state_table[], ruleADT *rules[]);
  ~CIPGenerator();   
  void prime(treeQueue& theQueue);
  bool nextValidInsertionPoint(CInsertionPoint& vip);
  bool nextValidTransitionPoint(bool isRise,
					      event_index_cont& riseStart,
					      event_index_cont& riseEnd);
  bool nextValidTransitionPoint2(bool isRise,
					       event_index_cont& riseStart,
					       event_index_cont& riseEnd);

};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#endif  /* svhelpers.h */
