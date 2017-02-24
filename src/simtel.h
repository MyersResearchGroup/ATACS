/*****************************************************************************/
/* simtel.h                                                                  */
/*****************************************************************************/
#ifndef _simtel_h
#define _simtel_h

#include "def.h"

#include <vector>
#include <utility>
#include <list>
#include <algorithm>
#include <signal.h>
#include <setjmp.h>
#ifdef __GCC3_INC__
#include <ostream>
#else
#include <ostream.h>
#endif

#include "misclib.h"
#include "findrsg.h"
#include "common_timing.h"
#include "graph_tracker.h"
#include "cpu_time_tracker.h"
#include <string>
#include <limits.h>

///////////////////////////////////////////////////////////////////////////////
//
template<class tracker>
class generic_analysis_control : public analysis_control {
public:
 
  /////////////////////////////////////////////////////////////////////////////
  // Constructor that sets the maximum iteration count max_iter, the tolerance,
  // tol, and the convergence func f.  NOTE: This doesn't set the initial prob_
  // vector because it may not be known or available at this time.
  /////////////////////////////////////////////////////////////////////////////
  generic_analysis_control( const unsigned long max_iter, 
			    const double& tol,
			    const convergence_func& f ) 
    : analysis_control( max_iter, tol, f ) {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Copy Constructor
  /////////////////////////////////////////////////////////////////////////////
  generic_analysis_control( const generic_analysis_control& c )
    : analysis_control( c ) {
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Call base class destructor
  /////////////////////////////////////////////////////////////////////////////
  ~generic_analysis_control() {
    // Nothing needs to be done.
  }

  /////////////////////////////////////////////////////////////////////////////
  // New operator which only asks for the prob_vector when it actually needs
  // it to check the convergence rate.  After it has the vector, it calls the
  // convergence function in the base class.
  // NOTE: The class passed in the template parameter must have a function
  // called probability which returns a prob_vector for this to work.
  /////////////////////////////////////////////////////////////////////////////
  bool operator()( const tracker& g ) {
    if ( !(m_conv( ++m_iteration, ++m_distance )) ) return( false );
    
    if ( m_iteration >= _MAX_ITERATION_ ) {
      cout << " -- reached maximum iteration count of " 
	   << _MAX_ITERATION_ << " -- ";      
      return( true );
    }
    
    prob_vector p = g.probability();
    m_distance = 0;
    return( convergence( p ) );
  } 
  
};
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// char_bv_tf is character representation of a bit vector where '1` is defined
// a 'T' and '0' is defined to be 'F'.
///////////////////////////////////////////////////////////////////////////////
typedef vector<char> char_bv_tf;

///////////////////////////////////////////////////////////////////////////////
// ren_list is used to store a rule number and a firing time.  The rule number
// is an index into the markkeyADT, from which the index into the rulesADT
// can be extracted, and the fire time is the time at which the rule has been 
// scheduled to fire in the monte-carlo simulation.
///////////////////////////////////////////////////////////////////////////////
typedef list<pair<double,int> > ren_list;

///////////////////////////////////////////////////////////////////////////////
// ostream operator<< that correctly formats and output a ren_list::value_type
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const ren_list::value_type& a );

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// time_advance is a struct used in conjunction with an stl algorithm, it is
// a mutating function object that can be used with transform.  It subjects 
// a value from the first entry in a pair<double,int>
///////////////////////////////////////////////////////////////////////////////
struct time_advance:public unary_function<pair<double,int>,pair<double,int> >{
  
  double m_time; // Specifies the amount of time to advance

  /////////////////////////////////////////////////////////////////////////////
  // Construct a time_advance object and set the class member m_time to time.
  /////////////////////////////////////////////////////////////////////////////
  time_advance( const double& time );

  /////////////////////////////////////////////////////////////////////////////
  // Return a new pair<double,int> were first = a.first - m_timer and 
  // second = a.second
  /////////////////////////////////////////////////////////////////////////////
  pair<double,int> operator()( const pair<double,int>& a );

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// If a.second == b.second return True otherwise return False
///////////////////////////////////////////////////////////////////////////////
struct rule_comp : public unary_function<int,bool> {
  int m_index;
  rule_comp( const int & i );
  bool operator()( const pair<double,int>& a );
};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// NOTE: This class is used in conjuction with an STL algorithm that iterates
// through each marking and calls operator() on each step of the iteration.
///////////////////////////////////////////////////////////////////////////////
class mark_update {
protected:
  markingADT m_mark;               // Current Marking for the TEL
  markkeyADT* m_mark_key;          // Maps the index in the marking rules
  ruleADT** m_rules;               // Rules matrix
  int m_nsignals;                  // Number of signals
  ren_list& m_Ren;                 // Current enabled list of rules
  char_bv_tf::const_iterator m_Rf; // List of rules that have fired
  int m_index;                     // Current index into m_Rf and m_mark

public:

  /////////////////////////////////////////////////////////////////////////////
  // Baisc constructor
  /////////////////////////////////////////////////////////////////////////////
  mark_update( markingADT mark, 
	       markkeyADT* mark_key,
	       ruleADT** rules,
	       const int& nsignals,
	       ren_list& Ren,
	       char_bv_tf::const_iterator Rf );

  /////////////////////////////////////////////////////////////////////////////
  // Each time this function is called, it checks to see if c is 'T'.  If it is
  // 'T' and the rule has not fired ( check m_Rf[m_index] ) and rule is level
  // enabled, than go ahead and add it to the m_Ren list. Otherwise, remove the
  // rule from the m_Ren list.  Update m_index. NOTE: m_mark_key is used with 
  // to find the actual rule that is added to m_Ren. 
  /////////////////////////////////////////////////////////////////////////////
  void operator()( const char& c );

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Used in conjunction with an STL algorithm, this class with identify any
// rules in m_mark_key that have an enabled event matching m_event.
///////////////////////////////////////////////////////////////////////////////
class rf_update {
protected:

  ruleADT**   m_rules;
  markkeyADT* m_mark_key; // maps indexs to rule's enabling and enabled events 

  int m_event;            // Event value to try and match
 
  int m_index;            // Current index into m_mark_key

public:

  /////////////////////////////////////////////////////////////////////////////
  // Basic constructor that iniitializes all class members
  /////////////////////////////////////////////////////////////////////////////
  rf_update( ruleADT** rules, markkeyADT* mark_key, int event );

  /////////////////////////////////////////////////////////////////////////////
  // Returns true iff the rule at m_index in the mark_key has the same enabled
  // event as m_event. NOTE: Used with stl algorithms to iterate on a list and
  // identify rules that have the an enabled event that matches m_event.
  /////////////////////////////////////////////////////////////////////////////
  bool operator()( const char& c );

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Used in conjuctions with an STL algorithm such as for_each, this will 
// find the minimum first value of a list of pair<double,int>.  NOTE: This
// would be a good function to templatize.
///////////////////////////////////////////////////////////////////////////////
class min_find {

  typedef pair<double,int> rule_type;
  typedef vector<rule_type> rule_vec;

  //pair<double,int> m_rule;  // Contains the rule with minimum first value
  rule_vec m_rule;

public:

  /////////////////////////////////////////////////////////////////////////////
  // Sets m_rule.first to MAXDOUBLE and m_rule.second to -1
  /////////////////////////////////////////////////////////////////////////////
  min_find();

  /////////////////////////////////////////////////////////////////////////////
  // Set m_rule equal to a iff a.first is < m_rule.first
  /////////////////////////////////////////////////////////////////////////////
  void operator()( const pair<double,int>& a );

  /////////////////////////////////////////////////////////////////////////////
  // Resolves conflict by considering all events that are scheduled to fire
  // at the same time.  This is done by consulting the upsilon values for
  // each of the rules stored in the ruleADT.  They are normalized and then
  // a Uniform random number between 0 and 1 is obtained.  The function then
  // Picks a rule based upon the normalized distribution and the uniform 
  // distribution.
  /////////////////////////////////////////////////////////////////////////////
  void resolve_conflict( markkeyADT* markkey, ruleADT** rules );

  /////////////////////////////////////////////////////////////////////////////
  // Returns m_rule
  /////////////////////////////////////////////////////////////////////////////
  pair<double,int> rule() const;

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Performs a monte-carlo simulation of a TEL structure. Template parameter
// specifies the object to be used to test for convergence.  Current objects
// are a generic_analysis_control, which is templatized to support a variety
// of graph_trackers.  The graph_tracker must support a probability function
// that returns a prob_vector.  The graph_tracker class is used to determine
// what probabilities are being calculated.  Current options include trackers
// that react to event or rule firings and calculate state or transition 
// probabilities.
// NOTE: This cannot be run without an existing state graph.
///////////////////////////////////////////////////////////////////////////////
template<class anal_control>
bool sim_tel( signalADT *signals,
	      eventADT *events,
	      ruleADT **rules,
	      markkeyADT *markkey,
	      int nevents, 
	      int ninpsig,
	      int nsignals,
	      int nrules,
	      char * startstate,
	      bool verbose,
	      int ndummy,
	      graph_tracker* g,
	      anal_control done,
	      bool noparg,
	      timeoptsADT timeopts);

///////////////////////////////////////////////////////////////////////////////
// This function sets up and calls a monte-carlo simulation of the current
// design.  The function uses the genrg flag to create a specific type of
// graph_tracker to be used in the simlation of the TEL.  graph_trackers can
// be created to react to rule or event firings.  They can find transition 
// or state probabilities. The system will store the results of the simulation
// in the state_table.
// NOTE: There needs to be a flag added to designADT that specifies what type
// the user wants from the simulation, transition or state probabilities.
// This will determine what type of graph_tracker is sent to simtel and where
// the final results are stored.
///////////////////////////////////////////////////////////////////////////////
bool simulate_tel( designADT design, graph_tracker* g );

#include "pbsuuaad.h"
typedef cycle_analysis_control anal_cont;
bool simulate_tel( designADT design, graph_tracker* g, anal_cont a );
//
///////////////////////////////////////////////////////////////////////////////
#endif  /* simtel.h */
