///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1998 by Eric G. Mercer
//
// @author Eric G. Mercer
//                    
// Permission to use, copy, modify and/or distribute, but not sell, this  
// software and its documentation for any purpose is hereby granted       
// without fee, subject to the following terms and conditions:            
//                                                                          
// 1. The above copyright notice and this permission notice must         
// appear in all copies of the software and related documentation.        
//                                                                          
// 2. The name of University of Utah may not be used in advertising or  
// publicity pertaining to distribution of the software without the       
// specific, prior written permission of Univsersity of Utah.
//                                                                          
// 3. This software may not be called "Taco" if it has been modified    
// in any way, without the specific prior written permission of           
// Eric G. Mercer                                
//                                                                          
// 4. THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY       
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.       
//                                                                          
// IN NO EVENT SHALL THE UNIVERSITY OF UTAH OR THE AUTHORS OF THIS 
// SOFTWARE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL 
// DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
// DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE  
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////
#ifndef __INTERNAL_CYCPER_H
#define __INTERNAL_CYCPER_H

#include <iterator>

#include "struct.h"
#include "markov_types.h"
#include "trigger_probs.h"
#include "graph_tracker.h"

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// binary_construct is my solution or augmentation to binary_compose.  Since
// binary_compose implements the following: f(g(x),h(x)), which takes three
// unary functions and composes them into a single unary function.  
// binary_construct implements the following: f(g(x),h(y)), which is different
// in that it takes two unary function and a binary function to compose a
// single binary function that first applies one of each of the  unary 
// functions to the left and right operands.
/////////////////////////////////////////////////////////////////////////////
template <class Operation1, class Operation2, class Operation3>
class binary_construct
  : public unary_function<typename Operation2::argument_type,
                          typename Operation1::result_type> {
protected:
  Operation1 op1;
  Operation2 op2;
  Operation3 op3;
public:
  binary_construct(const Operation1& x, 
		   const Operation2& y, 
		   const Operation3& z) : op1(x), op2(y), op3(z) { }
  typename Operation1::result_type
  operator()(const typename Operation2::argument_type& x,
	     const typename Operation3::argument_type& y ) const {
    return( op1( op2( x ), op3( y ) ) );
  }
};

/////////////////////////////////////////////////////////////////////////////
// construct2 is a nice function that will take care of the type binding
// for you and save the pain of having to specify the template parameters 
// yourself.
/////////////////////////////////////////////////////////////////////////////
template <class Operation1, class Operation2, class Operation3>
inline binary_construct<Operation1, Operation2, Operation3> 
construct2(const Operation1& op1, 
	   const Operation2& op2, 
	   const Operation3& op3) {
  return(binary_construct<Operation1, Operation2, Operation3>(op1, op2, op3));
}

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// cycle_period is a class that when used with a simulation object, will 
// track the evolving cycle_period.  The object represents a symbolic function
// by storing a coefficient for each variable.  This particular application
// models the cycle_period for each event in a TER structure.  The variables
// in each equation represent delay values that are dependent on the trigger
// for a gate.  A trigger for a gate represents a specific rule being the 
// causal rule in a transition.  Therefore, this object tracks an equation that
// has a number of variables equal to the number of rules in the TER, for a 
// total of equations equal to the number of events in the system.  
// cycle_period::probability() will return a equation representing the 
// average cycle period for the system that is independent of a specific 
// event.  The object does us a specific event designated in the constructor
// to calculate this period as the difference between consecutive occurances
// of the event.  The operator() can be used to fire events and update the
// equation set.  The trigger profile is necessary to determine which rules
// are actually used in the implementation ( trigger rule set ).  Indexes
// in the cycle_period vector are related to rules through the ruleADT.marking
// field.
// NOTE: This is basically an implementation of Steve Burns' timing simulation
// as described in his PhD thesis, but instead of using the a max function
// to calculate the firing time of something that can be triggered by more
// than one signal, this uses an expected value or weighted distribution that
// is obtained from the trigger profile.  For a c-element:
//  t( c+ ) = P(a+,c+)(t(a+)+delay(a+,c+) ) + P(b+,c+)(t(b+)+delay(b+,c+) )
// The TER can be repeatly unwinde until the difference between two calls
// divided by the number of lapsed cycles converges.
/////////////////////////////////////////////////////////////////////////////
class cycle_period {
protected:

  ruleADT**           m_rules;          // Rule matrix for (x,y)->i conversion
  vector<prob_vector> m_eqns;           // Equations set nevents by nrules
  unsigned int        m_period_count;   // Total number of completed cycles
  unsigned int        m_event;          // Index event used to calculate period
  trigger_profile     m_trig_prof;      // List of actual trigger rules 
  prob_vector         m_base;           // Vector for event offset in cycle

public:

  /////////////////////////////////////////////////////////////////////////////
  // conststucts a cycle period using rules as the mapping to change cartisan
  // (x,y) coordinates into flat i coord in a one-dimensional array.  This
  // relates the rules to a markkey.  The equation set m_eqns is constructed to
  // be nevents x nrules and used to track the evolving cycle period as events
  // are fired.  event is the event index to use in for the m_base vector and
  // to calculate the actual cycle_period defined as the difference in 
  // coefficients between the current and base vectors divided by the period
  // count minus 1.  tp is the list of trigger rules that actually do will
  // appear in the gate implementation.
  /////////////////////////////////////////////////////////////////////////////
  cycle_period( ruleADT* rules[],
		const unsigned int& nevents,
		const unsigned int& nrules,
		const unsigned int& event,
		const trigger_profile& tp );

  /////////////////////////////////////////////////////////////////////////////
  // This function calculates the actual cycle period given the current state
  // of the m_eqns matrix.  The coefficent values from the last completed 
  // cycle ( last call to increment ) are stored in row zero of the matrix.
  // The cycle period computed as the difference between coefficient values
  // of the last completed cycle minus the values of the base cycle  ( first
  // completed cycle ) divided by the number of completed cycles minus 1.
  // NOTE: This function is required in this form to be used with the 
  // generic_analysis_control object
  /////////////////////////////////////////////////////////////////////////////
  prob_vector probability() const;

  /////////////////////////////////////////////////////////////////////////////
  // operator() is used to actually fire an event and update the corresponding
  // time values in the m_eqns matrix.  The event entry in the m_eqns is 
  // updated as the wieghted sum of the time of each of its trigger signal
  // fire times plus the weighted sum of the events delay based on its 
  // trigger rule.
  /////////////////////////////////////////////////////////////////////////////
  void operator()( const unsigned int& event );

  /////////////////////////////////////////////////////////////////////////////
  // increment() will snapshot the row specified m_event into row 0 and 
  // increment the current value of m_period_count one time.  In this way, the
  // correct cycle period can be found with a call to probability, until the
  // next cycle is completed.
  /////////////////////////////////////////////////////////////////////////////
  void increment();

  /////////////////////////////////////////////////////////////////////////////
  // Copies the m_eqns matrix to the stream s.
  /////////////////////////////////////////////////////////////////////////////
  void write_string( ostream& s );

};
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
// simulate_cycle_period will repetively unroll a TER structor by doing a 
// timing simulation as described by Burns in his PhD.  The unrolling stops
// when the cycle period converges to a within the specified tolerance or 
// achieves the maximum number of iterations allowed in the unrolling.  The
// resultant cycle period is assigned to i_out as an array of coefficients
// whose indexes are associated with the ruleADT->marking field in the
// rule matrix rules.  The trigger_profile is necesary to know those
// rules which actually are used as triggers in the imlementation.  When
// this completes, the reachable and color fileds in rules and events 
// repectively, will have the same boolean value.
/////////////////////////////////////////////////////////////////////////////
bool simulate_cycle_period( const trigger_profile& tp,
			    eventADT events[],
			    ruleADT* rules[],
			    const unsigned int& nevents,
			    const unsigned int& nrules,
			    const CPdf::real_type& tolerance,
			    back_insert_iterator<prob_vector> i_out,
			    bool prompt = true );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
class cycle_period_tracker : public graph_tracker {

  typedef prob_type real_type;

  // Number of event instances observed thus far
  unsigned int m_iterations;
  
  // Records the amount of time that elapses between the consecutive
  // occurances of the event m_event_index.  Cycle_time is measured 
  // as the time between the ith and (i+1)th occurence of m_event_index
  real_type m_time;

  // The notes when the simulation should stop.  This is the cycle_index
  // for m_event_index.  When m_event_index reaches it m_stop_index firing
  // then the simulation will terminate
  unsigned int m_stop_index;

  // The index of the event that is being tracked in this simulation
  unsigned int m_event_index;

  // Denotes the number of cycle to wait before starting to record observations
  // of the cycle time.
  unsigned int m_start_index;

  // Records each obeservation of the cycle-time for m_event_index.
  prob_vector* m_vals;

protected:

  /////////////////////////////////////////////////////////////////////////////
  // Returns a prob_vector of size 1 with the entry set to 0 or 1.
  // prob_vector[1] == 1 iff m_iterations == m_stop_index
  /////////////////////////////////////////////////////////////////////////////
  virtual prob_vector find_prob() const;

  /////////////////////////////////////////////////////////////////////////////
  // Required by the graph_tracker interface and not used in this class
  /////////////////////////////////////////////////////////////////////////////
  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns ); 
public:

  /////////////////////////////////////////////////////////////////////////////
  // Contructor: event_index -> index of the event that you are going to
  //                            track in this simulation.
  //             reps        -> The number of obeservations to use in 
  //                            calculation the cycle_metric (the average
  //                            cycle-time of m_event_index).
  //             start_index -> The number of occurences of m_event_index
  //                            that you are going to wait before you start
  //                            recording cycle-times on m_event_index.
  /////////////////////////////////////////////////////////////////////////////
  cycle_period_tracker( unsigned int event_index,
                        unsigned int reps,
                        unsigned int start_index );
                        
  
  ~cycle_period_tracker();
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns the value of the average cycle-time.  If this is called before
  // the simulation function has completed or before the simulation function
  // is called, then all the unseen observations have a zero default value
  // and the resulting average cycle-time will be incorrect.
  /////////////////////////////////////////////////////////////////////////////
  real_type cycle_metric() const;


  /////////////////////////////////////////////////////////////////////////////
  // This function returns the current vector of observations seen thus far.
  // If start_index is set to 5, then the value in 0 location is the amount of
  // time that elapsed between the 5th and 6th firing of the event as specified
  // by the event_index in the constructor.  The vector is the same size
  // as reps (i.e., 1 entry for each observation used in the calculation of
  // the average cycle-time (cycle_metric).
  /////////////////////////////////////////////////////////////////////////////
  const prob_vector* vals() const {
    return( m_vals );
  }

  /////////////////////////////////////////////////////////////////////////////
  // This is the interface function to the simulator.  This is called on
  // every event that is fired.  The enabled event is observed, and if it
  // is equal to the event_index specified in the constructor, then 
  // it is recorded as an instance of the event.  When start_index number
  // of instances are seen, then the fire function will begin recording
  // obeservations of the cycle-time until reps number of observations are
  // recorded.
  /////////////////////////////////////////////////////////////////////////////
  bool fire( const double& weight,
	     const int& enabled,
	     const int& enabling = 0 );
  
  /////////////////////////////////////////////////////////////////////////////
  // Resets 'this' object to be ready for another simulation cycle.
  // This needs to be called between successive calls to simulate.
  /////////////////////////////////////////////////////////////////////////////
  void clear();

  /////////////////////////////////////////////////////////////////////////////
  // This function is not implemented by this class, but it is recorded by the
  // graph_tracker interface.
  /////////////////////////////////////////////////////////////////////////////
  virtual void store_results( stateADT* s ) const;

};

//
/////////////////////////////////////////////////////////////////////////////
#endif
