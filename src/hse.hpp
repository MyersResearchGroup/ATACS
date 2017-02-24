/*

 Concurrency Reduction In Timed Integrated Circuits (CRITIC)

   Copyright (C) 2000-2002, by Eric R. Peskin
   Center for Asynchronous Circuit and System Design
   University of Utah

   Permission to use, copy, modify and/or distribute, but not sell,
   this software and its documentation for any purpose is hereby
   granted without fee, subject to the following terms and conditions:

   1.  The above copyright notice and this permission notice must
   appear in all copies of the software and related documentation.

   2.  The name of University Of Utah may not be used in advertising
   or publicity pertaining to distribution of the software without the
   specific, prior written permission of University Of Utah and Eric
   R. Peskin

   3.  This software may not be called "CRITIC" if it has been modified
   in any way, without the specific prior written permission of Eric
   R. Peskin.

   Questions?  Comments?  mailto:peskin@vlsigroup.ece.utah.edu
   Bug reports?  http://www.async.ece.utah.edu/cgi-bin/atacs

*/

#ifndef _hse_hpp
#define _hse_hpp

#include <float.h>
#include <fstream>
// gcc296 method
// #include <stdiostream>
#include <sstream>
#include <iterator>
#include <list>

#include "connect.h"
#include "def.h"
#include "findrsg.h"
#include "findstats.h"
#include "interface.h"
#include "loader.h"
#include "ly.h"
#include "memaloc.h"
#include "stats.h"

using namespace std;

const string SEND_SUFFIX = "_send";
const string RECEIVE_SUFFIX = "_rcv";

string prefix(const char* action);
string suffix(const char* action);

// Expand communication actions to handshaking signals:
bool handshaking_expansion(char command, designADT design);

template <class element> // Print any list:
ostream& operator<<(ostream& out, const list<element>& List){
  out << endl;
  copy(List.begin(), List.end(), ostream_iterator<element>(out));
  return(out);
}

template <class element> // Print any set:
ostream& operator<<(ostream& out, const set<element>& Set){
  copy(Set.begin(), Set.end(), ostream_iterator<element>(out));
  return(out << endl);
}

template <class element> // Print any pair of elements:
ostream& operator<<(ostream& out, const pair<element, element>& Pair){
  return(out << Pair.first << Pair.second << endl);
}

template <class element, class compare> // Are X and Y disjoint?
bool disjoint(const set<element, compare>& X, const set<element, compare>& Y){
  set<element, compare> intersection;
  set_intersection(X.begin(), X.end(), Y.begin(), Y.end(),
		   inserter(intersection, intersection.begin()));
  return intersection.empty();
}

template <class element, class compare> // X = X U Y
set<element, compare>& operator|=(set<element, compare>& X,
				  const set<element, compare>& Y){
  X.insert(Y.begin(), Y.end());
  return X;
}

template <class element, class compare> // X = intersection(X,Y)
set<element, compare>& operator&=(set<element, compare>& X,
				  const set<element, compare>& Y){
  set<element, compare> intersection;
  set_intersection(X.begin(), X.end(), Y.begin(), Y.end(),
		   inserter(intersection, intersection.begin()));
  X = intersection;
  return X;
}

class loggedStream{//A loggedStream copies anything sent to it to two streams//
public:
  // Construct a loggedStream from a C++ stream and a C file pointer:
  loggedStream(ostream& Stream, FILE* log);

  ~loggedStream();  // Destroy this loggedStream.

  /* The following applies manip to each of the two streams.  manip
     could be any manipulator that works with ostreams (e.g., endl):  */
  loggedStream& operator<<(ostream& (*manip)(ostream&));

  /* The following sends any data to each of the two streams.  data
     cannot be a function pointer, but it can be any object that could
     be sent to an ostream.  There is no need to write operators for
     each class to specifically output objects to loggedStreams.  As
     long as there is an operator to send the object to an ostream,
     this operator will forward it to both ostreams:  */
  template <class type> loggedStream& operator<<(type data){
    interactive << data;
// #ifdef __GCC_OLD__
//     logFile << data;
// #else
    ostringstream oss;
    oss << data;
    fprintf(logFile,oss.str().c_str());
// #endif
    return *this;
  }
protected:
  ostream& interactive;  // a normal C++ ostream
  FILE* logFile;  // a stream interface to a C file-pointer
};/////////////////////////////////////////////////////////////////////////////


class Event;  // Rules need Events, but Events need Rules.

class Rule{ // Represents a rule of a TEL structure ///////////////////////////
public:
  /* Each constructor creates a new rule from *Enabling to *Enabled,
     and inserts the new rule into the fan-out of *Enabling and the
     fan-in of *Enabled:  */
  Rule(Event* Enabling, Event* Enabled, int Epsilon, const string& Exp="",bool fromVisit=false);
  Rule(Event* Enabling, Event* Enabled, ruleADT Rule); // use info from ruleADT

  /* The destructor removes the rule from:  the fan-out of its enabling
     event, the fan-in of its enabled event, and existence:  */
  ~Rule();

  /* The following functions deal with the expression of the Rule.
     Each expression is represented as a string withOUT the enclosing
     brackets.  */
  void copyExpression(Rule* that);  // Copy expression from that Rule.
  const string& Expression(void) const;  // returns the expression on the Rule
  void clearExpression(void);  // removes the expression from the Rule
  void pushExpression(const string& exp);  // My expression &&= exp
  bool uses(const string& signal) const; //Does expression involve this signal?

  // The following functions deal with the Events associated with the Rule:
  Event* Enabling(void) const;  // Return a pointer to the enabling Event.
  Event* Enabled(void) const;  // Return a pointer to the enabled Event.
  void setEnabling(Event* New); //Change enabling event.  Update event Fanouts.
  void setEnabled(Event* New);  // Change enabled event.  Update event Fanins.

  void clearTiming(void);

  int Epsilon(void) const;  // Return epsilon value of this rule.
  void setEpsilon(const int value);  // Set epsilon value of this rule.

  bool operator<(const Rule& that) const;
  struct LT
  {
    bool operator()(const Rule* r1, const Rule* r2) const
    {
      return *r1 < *r2;
    }
  };

  typedef set<Rule*> Set;

  void grabTiming(Set fanin);
  
  friend ostream& operator<<(ostream& out, const Rule& rule);  // Print rule.
  friend ostream& operator<<(ostream& out, const Rule* rule);  // Print *rule.

  static int gateLower;  // minimum gate delay
  static int gateUpper;  // maximum gate delay
  bool addedByVisit;
protected:
  Event* enabling;  // a pointer to the enabling Event
  Event* enabled;  // a pointer to the enabled Event
  int epsilon;  // the epsilon value on this rule
  int lower;  // the lower timing bound on this Rule
  int upper;  // the upper timing bound on this Rule
  int type;  // the ruletype from ruleADT (see struct.h)
  string expression;  // the expression on the Rule, withOUT brackets
};/////////////////////////////////////////////////////////////////////////////

class Event{ // Represents an event of a TEL structure ////////////////////////
public:
  Event();  // Create a new dummy event named $HSE<nextIndex>.
  Event(const string& name);  // Create a new dummy event named $<name>
  Event(const string& Signal, char direction);  // Create a new output event.
  Event(eventADT Event);  // Build an event from the old eventADT.
  ~Event();  // Delete this event and all rules that involve it.

  bool isCommunication(void) const; //Is event part of a channel communication?
  bool passive(void) const; //Is event part of a passive channel communication?
  bool dumb(void) const;  // Is this event a dummy event?
  bool uses(const string& Signal) const; //Does event or its Fanout use Signal?
  bool isMerge(void) const;  // Does this event have conflicting enablings?
  bool reset(void) const;  // Am I part of the reset phase of communication?

  void eatFanout(void);  // Pop expressions from Fanout rules.  Store the AND.
  void produceFanin(void);  // Push stored expression onto each Fanin rule.
  const string& fanoutExpression(void);//product of expressions on fanout rules
  const string& faninExpression(void); // product of expressions on fanin rules
  const string& Signal(void) const;  // the signal of this event
  string channel(void) const;

  void markUnseen(void);  // Mark this event as unseen

  /* On each path from this Event, find the first Event that uses the
     same signal as does loose.  Create a new rule from loose to the
     event that was found.  This rule will have the expression exp.
     The rule should be initially marked if the input epsilon is
     greater than 0 or if an initially marked rule is encountered
     along the path.  If the added rule is to be initially marked, the
     corresponding reset rule is also added.  The argument seen is
     used to distinguish distinct depth first searches.  (When visit()
     calls itself, it keeps seen the same.  But each new, external
     call to visit() should use a distinct value of seen.) */
  void visit(int seen, Event* loose, const string& exp, Event* reset,
	     int epsilon=0);

  // Can I reach target going through at most allow tokens?:
  bool reaches(Event* target, int allow, int allowCommunications=INFIN);

  /* addDescendants appends the descendants of this Event on to the
     back of the given list found, IN DEPTH-FIRST-SEARCH ORDER.
     markUnseen() must be called on each event prior to each external
     call to addDescendants().  */
  void addDescendants(list<Event*>& found);  // found |= my descendants
  void computeClique(void);

  bool operator<(const Event& that) const;
  struct LT
  {
    bool operator()(const Event* e1, const Event* e2) const
    {
      return *e1 < *e2;
    }
  };
  typedef set<Event*> Set;

  Event::Set predecessors(void) const;
  Event::Set successors(void) const;

  friend ostream& operator<<(ostream& out, const Event& event); // print event
  friend ostream& operator<<(ostream& out, const Event* event); // print *event

  Rule::Set Fanin;  // These causal rules enable this Event.
  Rule::Set Fanout;  // This Event enables these causal rules.
  Rule::Set inConstraints;  // These constraint rules enable this Event.
  Rule::Set outConstraints;  // This Event enables these constraint rules.
  Rule::Set added;
  Rule* Rin;
  Rule* Rout;
  Event* next;
  string exp;
  Event::Set conflicts;  // the Events with which this event conflicts
  Event::Set concurs;  // the Events with which this event is concurrent
  Event::Set clique;  // the Events in this branch of a conflict
  Event *communication;  // Rise of communication of which this event is part
protected:
  // Can I reach target going through at most allow tokens?:
  bool reaches(Event* target, int allow, int seen, int allowCommunications);
  string action;  // complete action of the event: <signal>+-
  int occurrence;  // unique index if this event was created during HSE
  int type;  // See ly.h.
  string data;  // the data variable being sent or received
  string signal;  // just the <signal> name
  string expression;  // This stores the expression computed by eatFanout().
  int status;  // Used for bookkeeping during Depth-First Searches of the TEL
  static int nextIndex;  // the next available index
};/////////////////////////////////////////////////////////////////////////////

class Candidate{
public:
  Candidate(Event* Enabling, Event* Enabled, int Epsilon);
  ~Candidate();
  void add(void);
  void remove(void);
  bool redundant(void);  // Is this candidate's rule now redundant?
  bool Promising(void) const;  // Is this candidate promising?
  Event* Enabling(void) const;  // Return a pointer to the enabling Event.
  Event* Enabled(void) const;  // Return a pointer to the enabled Event.
  int Epsilon(void) const;  // Return epsilon value of this rule.
  friend ostream& operator<<(ostream& out, const Candidate& candidate); //print
  set<Event::Set> conflictingCliques;

  struct isPromising
  {
    bool operator()(const Candidate &c) const
    {
      return c.Promising();
    }
  };

protected:
  Event* enabling;  // a pointer to the enabling Event
  Event* enabled;  // a pointer to the enabled Event
  int epsilon;  // the epsilon value on this rule
  Rule* rule;
  Event::Set conflicts;
  bool promising;
};

class Communication{ // Represents a communication (a Send or a receive) ///
public:
  Communication(Event*Rise, Event*Fall);//Build a communication from its events
  ~Communication();  // Destroy this communication.

  bool operator<(const Communication& that) const;  // Compare this vs. that.
  void eatFanout(void) const;  // Apply eatFanout to each event.
  void produceFanin(void) const;  // Apply produceFanin to each event.

  // Print out this communication:
  friend ostream& operator<<(ostream& out, const Communication& communication);

  Event* rise; // The rising event of the output signal of this communication.
  Event* fall; // The falling event of the output signal of this communication.
};/////////////////////////////////////////////////////////////////////////////

bool conflictFree(const Event::Set& theEvents);

class Possibility{
public:
  Possibility(const prob_vector& cost, const string& fileName,
	      const string& rules);
  ~Possibility();
  void setLiterals(const int lits);
  void setTransistors(const int area);
  void setCost(const prob_vector& cost);
  void setName(char * newName);
  void setUnique(const string& uniqueName);

  const int Literals(void) const;
  const int Transistors(void) const;
  
  bool operator<(const Possibility& that) const;
  friend ostream& operator<<(ostream& out, const Possibility& p);
  prob_type ave, adev;
  string name;
protected:
  string added;
  string unique;
  struct absDiff : public binary_function<prob_type,prob_type,prob_type>{
    prob_type operator()(prob_type x, prob_type y){return fabs(x-y);}
  };
  prob_vector period;
  prob_type sdev, var, skew, curt;
  int literals;
  int transistors;
};


class sparseTEL{ // Represents a Timed Event-Level structure //////////////////
public:
  sparseTEL(designADT design, bool annotate=false);  // Build a sparseTEL from a designADT.
  ~sparseTEL();  // Destroy this sparseTEL.
  
  void passify(void);  // Make passive communications truly passive.
  void interleave(void);  // Make communications maximally concurrent.
  void fatten(void);  // Insert dummy events as needed
  list<Candidate> candidates(void);
  void enumerate(list<Candidate>::iterator start,
		 list<Candidate>::iterator level,
		 list<Candidate>::iterator stop,
		 const string& decided,
		 Possibility current);


  // Redirect all rules in fanin to a new dummy event, with one rule to sink:
  Event * split(Rule::Set fanin, Event* sink, bool slideToken=true);

  void store(const string& baseName);  // Store TEL to <baseName>.tel
  friend ostream& operator<<(ostream& out, sparseTEL& tel); //print a sparseTEL

  int calls;  // the number of times enumerate has been called
  int attempts;
  int checks;
  int simulations;
  int solutions;
  set<Possibility> good;
  set<string> blacklist;
  set<string> explored;
  set<pair<prob_type, string> > leaves;
  string lastStored;  // stores the last fileName used by store()
  bool infinite;  // Is this untimed, or does it contain any infinite bounds?
  Possibility best;
  string filters;

protected:
  string startState;  // the start state of the TEL
  unsigned int ninputs;  // the number of input events
  unsigned int nsmart;
  vector<Event*> Events;  // list of all the Events of the TEL
  // Note: Each Event keeps track of the Rules associated with it.
  set<list<Event*> > Merges;  // lists of rising events on the same signal

  // The following three sets are derived from the above information:
  set<Communication> Communications;  // all communications in the TEL
  set<Communication> Passives;  // just the passive communications
  set<Communication> outputCommunications;  // just the output communications

  loggedStream messages;  // used for normal status messages
  bool verbose;  // Should operations on this TEL be verbose?
  string name;  // the name of this TEL
  designADT design;  // Just keep this for enumerate()
  int oldStatus;
  pruneType pruning;
  bool addVariable;
  prob_type maxPeriod;
  unsigned int root;  // index of the root of the output process
};/////////////////////////////////////////////////////////////////////////////
#endif // hse.hpp
