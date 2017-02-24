///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1997 by Eric G. Mercer
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
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////
#ifndef __INTERNAL_GRAPH_TRACKER_H
#define __INTERNAL_GRAPH_TRACKER_H

#include <vector>
#include <iostream>

#include "struct.h"
#include "markov_matrix_func.h"
#include "markov_chain_analysis.h"

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// graph_tracker is an object used to follow a state graph as rules or events
// fire in a TEL system.  This object can respond to event or rule firings 
// and is used in monte-carlo simulations of a TEL object.
// NOTE: This is an abstract interface class and must be subclassed to be 
// be used.  The subclass is responsible to define what actions in the 
// simulation must be counted, how the probabilities are calculated and how
// or where to store the results in state_table as defined in ATACS.
///////////////////////////////////////////////////////////////////////////////
class graph_tracker {
public:

  enum fire_type { rule = 0, event = 1, both = 2 };

protected:

  fire_type m_fire;         // Are we reacting to rules or events

  stateADT m_ps;            // What is the present state in the state table

  int m_state_count;        // How many state exist in the system.

protected:

  /////////////////////////////////////////////////////////////////////////////
  // Searches the state_table s and finds the initial state which is uniquely 
  // defined to have state number 0.  This function returns the total number
  // of states in the system.
  /////////////////////////////////////////////////////////////////////////////
  index_type state_initialize( stateADT* s );

  /////////////////////////////////////////////////////////////////////////////
  // Required interface function for sub-classes.  This defines how to 
  // calculate the probability for specialized graph_tracker objects.
  /////////////////////////////////////////////////////////////////////////////
  virtual prob_vector find_prob() const = 0;
  
  /////////////////////////////////////////////////////////////////////////////
  // This function is used for accounting in subclasses.  Any subclass must
  // define what it needs to count when a rule or event is fired.
  /////////////////////////////////////////////////////////////////////////////
  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns ) = 0;

public:

  /////////////////////////////////////////////////////////////////////////////
  // Simple constructor that gives initial value to all data members.
  /////////////////////////////////////////////////////////////////////////////
  graph_tracker();

  /////////////////////////////////////////////////////////////////////////////
  // Simple constructor initializes all member variables.
  /////////////////////////////////////////////////////////////////////////////
  graph_tracker( stateADT* s, const fire_type& fire = event );

  /////////////////////////////////////////////////////////////////////////////
  // pure virtual destructor, makes all subclasses create a destructor.
  /////////////////////////////////////////////////////////////////////////////
  virtual ~graph_tracker() = 0;

  /////////////////////////////////////////////////////////////////////////////
  // Returns the value of m_fire that deterimines if the graph_tracker is 
  // reponding to rule or event firings.
  /////////////////////////////////////////////////////////////////////////////
  fire_type mode() const;
  
  /////////////////////////////////////////////////////////////////////////////
  // bool fire tacks the value of an enabled and enabling event and moves the
  // m_ps object to point to the new present state.  This function also makes
  // a call to update_counts(..), which lets subclasses update their accounting
  // on different values that they are tracking.  NOTE: Enabling defaults to
  // 0 so that this works for event or rule tracking systems.
  /////////////////////////////////////////////////////////////////////////////
  virtual bool fire( const double& weight,
	             const int& enabled,
	             const int& enabling = 0 );

  /////////////////////////////////////////////////////////////////////////////
  // Simple obeserver function which returns the value of find_prob(). 
  // find_prob() is defined by subclasses to compute the desired probability
  // for a specialized graph_tracker.
  /////////////////////////////////////////////////////////////////////////////
  prob_vector probability() const;

  /////////////////////////////////////////////////////////////////////////////
  // Defines how and where results are stored in the stateADT from ATACS.  
  // This can only be defined by subclasses.
  /////////////////////////////////////////////////////////////////////////////
  virtual void store_results( stateADT* s ) const = 0;

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// state_tracker when used with a monte-carlo simulation will caculate the 
// steady-state or long term probabilities of states in a state graph.  This
// object can be used with either a region graph or a reduced state graph.  
// Its results are stored in the stateADT object in the stateADT* state
// table in atacs.
///////////////////////////////////////////////////////////////////////////////
class state_tracker : public graph_tracker {
protected:
  
  typedef vector<index_type> visit_list;  // Tracks the number of times a state
                                          // is visited in the system.
  visit_list m_visit;                     // Increments on entry to the state.

  index_type m_iteration;                 // How many state transitions have 
                                          // occured on total.
protected:

  /////////////////////////////////////////////////////////////////////////////
  // find_prob() Performs the actual probability calculation for the state
  // probabilities.  The calculation is done as follows:
  // p[state_number] = # times moved to state_number  / # of state transitions
  /////////////////////////////////////////////////////////////////////////////
  virtual prob_vector find_prob() const;

  /////////////////////////////////////////////////////////////////////////////
  // Must be called every time a rule or event is fired in the TEL system. This
  // records the total number of state transitions, and how many times a 
  // specific state is visited.
  /////////////////////////////////////////////////////////////////////////////
  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns );

public:
  
  /////////////////////////////////////////////////////////////////////////////
  // Simple constructor intializes all member values.
  /////////////////////////////////////////////////////////////////////////////
  state_tracker( stateADT* s, const fire_type f = event );
  
  /////////////////////////////////////////////////////////////////////////////
  // Simple destructor required by interface.
  /////////////////////////////////////////////////////////////////////////////
  ~state_tracker();
  
  /////////////////////////////////////////////////////////////////////////////
  // state_tracker stores its results in the proability field in each stateADT
  // found in the stateADT* s.  The index into the prob_vector for the state
  // probability coresponds to the stateADT->number field.
  /////////////////////////////////////////////////////////////////////////////
  virtual void store_results( stateADT* s ) const;
  
};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// transition_tracker when used in a monte-carlo simulation of a TEL will 
// calculate transition probabilities in a stateADT*.  The edges in the 
// stateADT* can be rules or events.  The results are stored in the
// state_listADT which is a list of edges leaving each stateADT.
///////////////////////////////////////////////////////////////////////////////
class transition_tracker : public graph_tracker {
protected:
  
  typedef map<index_type,index_type> edge_list;

  typedef vector<pair<index_type,edge_list> > transition_list;
                            
  transition_list m_tlist;  // Vector indexed by state number and each entry  
                            // has a list of edges and count information.
  index_type m_tlist_count; // Number of non-single out-going edges in sys.

  typedef edge_list::value_type value_type;

  /////////////////////////////////////////////////////////////////////////////
  // div_help exists solely to help calculate the transition probabilites.  
  // This is used with the transform algorithm to calculate actual transition
  // probabilities.  This takes a edge_list::value_type and returns
  // edge_list::value_type.second divided by y.
  /////////////////////////////////////////////////////////////////////////////
  struct div_help : public binary_function<value_type,prob_type,prob_type> {
    prob_type operator()(const value_type& x, const prob_type& y) const {
      if ( y == 0 || x.second == 0 ) return( 0 );
      return( (double)(x.second) / y );
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  // This struct exists solely to aid in the computation of the transition 
  // proabilities.  This can be used with a for_each function to loop through
  // each state in m_tlist and calculate the probability of each edge leaving
  // that state.  The results of the calculation (found using div_help) 
  // are sent to the output iterator m_out.  Probabilities are found as:
  // *m_out++ = # times an edge is used in a state / # times sys in that state
  /////////////////////////////////////////////////////////////////////////////
  struct prob_helper {
    typedef prob_vector::iterator output_iter;
    output_iter m_out;
    prob_helper( output_iter out ) : m_out( out ) {}
    void operator()( const pair<index_type,edge_list>& m ) {
      if ( m.second.size() == 1 )
	return;
      m_out = ::transform( m.second.begin(), m.second.end(), m_out,
			   bind2nd( div_help(), (double)m.first ) );
    }
  };

protected:

  /////////////////////////////////////////////////////////////////////////////
  // This function used to for three purposes:
  //   1) Initialize m_tlist with the correct edges.  m_tlist is indexed by 
  //      the state number and contains a list of edges and the count of how
  //      many times that state has been visited and how many times each edge
  //      in that state has been used.
  //   2) Initialize all counts to 0.
  //   3) Count the number of edges in the system that have the following
  //      property: The edge is not the only edge leaving a state ( i.e. Count
  //      an edge if it is an edge in a state where other edges exist. ).
  /////////////////////////////////////////////////////////////////////////////
  index_type intialize_tlist( stateADT* s );

  /////////////////////////////////////////////////////////////////////////////
  // Calculates the local proability of each edge in the system.  Note: This
  // is note a long term transition probability, this is the probability of
  // using an edge GIVEN that the system is in a particular state j.  (i.e. 
  // this is a conditional probability statisfying the markov memoryless 
  // property. )
  // The function returns a list of probabilites of edges which makes no sense
  // to anyone in this form.  Don't try and figure it out, use store_results
  // to put the probabilities into the stateADT* and look at the results there.
  // NOTE: Probabilities are only calculated for states that have more than 
  // one outgoing edge.
  /////////////////////////////////////////////////////////////////////////////
  virtual prob_vector find_prob() const;

  /////////////////////////////////////////////////////////////////////////////
  // Does the accounting in the system.  It first increments the number number 
  // of times ns has been visited.  It then updates the number of times the
  // edge between ps and ns has been used.  And finally it calls update_count
  // in the base class to increment the number of state transitions that have 
  // occured in the system.
  // Note: The call to the base class is not needed, but recored here because
  // the class could be extended to find both the conditional transition
  // probabilities AND the long term state probabilities by simply redefining
  // the find_prob and store_results functions.
  /////////////////////////////////////////////////////////////////////////////
  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns ); 

public:
  
  /////////////////////////////////////////////////////////////////////////////
  // Simple constructor initializes all member vars.
  /////////////////////////////////////////////////////////////////////////////
  transition_tracker( stateADT* s, const fire_type f = event );
  
  /////////////////////////////////////////////////////////////////////////////
  // Destructor required by the interface.
  /////////////////////////////////////////////////////////////////////////////
  ~transition_tracker();
  
  /////////////////////////////////////////////////////////////////////////////
  // Stores the conditional transition probabilities in the stateADT* on the
  // statelistADT found in each stateADT in stateADT*.  (i.e. Each state has
  // a set of edges, and this function puts the corresponding transition 
  // probability on each edge. )
  /////////////////////////////////////////////////////////////////////////////
  virtual void store_results( stateADT* s ) const;

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// time_tracker will compute the steady state of the ER based completely
// on the amount of time spent in each state and the total amount time which 
// has transpired in the system.  When the ratio of time in state to total time
// settles to a value that is within the system tolerance between successive
// simulation cycles, then the simulation is terminated and the results are
// returned.
///////////////////////////////////////////////////////////////////////////////
class time_tracker : public graph_tracker {
protected:

  typedef double time_type;

  typedef vector<time_type> visit_list;  // Tracks sojourn time in a state
              
  visit_list m_sojourn;                  // Grows according to time spent in
                                         // the state
  time_type m_total;                     // Records total simulation time
                                         // of the system

protected:

  /////////////////////////////////////////////////////////////////////////////
  // Calculates the state probability as the total time spent in a state 
  // divided by the total simulation time of the system.
  //   
  //              sojourn time / total time
  /////////////////////////////////////////////////////////////////////////////
  virtual prob_vector find_prob() const;

  /////////////////////////////////////////////////////////////////////////////
  // Updates the time that was spent in the present state by going to the 
  // sojourn list and adding weight to it.  Updates the total simulation time
  // by adding weight to the m_total member variable.
  /////////////////////////////////////////////////////////////////////////////
  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns );
		
public:
  
  /////////////////////////////////////////////////////////////////////////////
  // Simple constructor initializes all member vars.
  /////////////////////////////////////////////////////////////////////////////
  time_tracker( stateADT* s, const fire_type f = event );
  
  /////////////////////////////////////////////////////////////////////////////
  // Destructor required by the interface.
  /////////////////////////////////////////////////////////////////////////////
  ~time_tracker();
  
  /////////////////////////////////////////////////////////////////////////////
  // Stores the conditional transition probabilities in the stateADT* on the
  // statelistADT found in each stateADT in stateADT*.  (i.e. Each state has
  // a set of edges, and this function puts the corresponding transition 
  // probability on each edge. )
  /////////////////////////////////////////////////////////////////////////////
  virtual void store_results( stateADT* s ) const;

};

//
///////////////////////////////////////////////////////////////////////////////

#endif // __INTERNAL_GRAPH_TRACKER_H
