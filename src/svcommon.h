// svcommon.h -- file which spans both CTextTel and svhelpers
// created to break cyclic dependencies
#ifndef _svcommon_h
#define _svcommon_h

#include "struct.h"
#include <list>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

typedef list<stateADT> state_list;
typedef vector<state_list> state_list_cont;
typedef vector<int> event_index_cont;

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class CRuleInfo
{
public:
  eventADT event;
  char     initialMarking;
  string   expression;
  int      lowerBound;
  int      upperBound;
};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class CInsertionPoint
{
public:
  int naturalViolations;
  int causedViolations;
  char initialValue; 
  event_index_cont riseStart;
  event_index_cont riseEnd;
  event_index_cont riseWitnesses;
  event_index_cont fallStart;
  event_index_cont fallEnd;
  event_index_cont fallWitnesses;

  bool isValidInsertionPoint(eventADT* events);
};
ostream& operator<<(ostream& outstream, const CInsertionPoint& vip);
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
typedef vector<CRuleInfo> rules_cont;
class CStateVariableInfo
{
public:
  string signalName;
  string instance;
  char   initialValue;
  rules_cont rulesToRise;
  rules_cont rulesFromRise;
  rules_cont rulesToFall;
  rules_cont rulesFromFall;

  CStateVariableInfo(const string& sigName,
		     const string& inst,
		     const CInsertionPoint& vip,
		     eventADT events[], int nevents,
		     int lowerBound,
		     int upperBound,
		     cycleADT**** cycles, int ncycles);

};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#endif  // svcommon.h
