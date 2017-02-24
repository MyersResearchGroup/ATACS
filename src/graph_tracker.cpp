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
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE  
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////
#include "graph_tracker.h"
#include "cpu_time_tracker.h"

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Searches the state_table s and finds the initial state which is uniquely 
// defined to have state number 0.  This function returns the total number
// of states in the system.
///////////////////////////////////////////////////////////////////////////////
index_type graph_tracker::state_initialize( stateADT* s ) {
  if ( s == NULL ) {
    m_ps = NULL;
    return( 0 );
  }
  int state_count = -1;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT curstate = s[k] ; 
	  curstate != NULL && curstate->state != NULL;
	  curstate = curstate->link ) {
      if ( curstate->number == 0 )
	graph_tracker::m_ps = curstate;
      if ( curstate->number > state_count )
	state_count = curstate->number;
    }
  }
  return( ++state_count );
}

/////////////////////////////////////////////////////////////////////////////
// Simple constructor that gives initial value to all data members.
/////////////////////////////////////////////////////////////////////////////
graph_tracker::graph_tracker() 
  : m_fire( event ),
    m_ps( NULL ),
    m_state_count( 0 ) {
}

///////////////////////////////////////////////////////////////////////////////
// Simple constructor initializes all member variables.
///////////////////////////////////////////////////////////////////////////////
graph_tracker::graph_tracker( stateADT* s, const fire_type& fire )
  : m_fire( fire ),
    m_state_count( state_initialize( s ) ) {
  // DO NOTHING
}

///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
graph_tracker::~graph_tracker() {
  // Do Nothing
}

///////////////////////////////////////////////////////////////////////////////
// Returns the value of m_fire that deterimines if the graph_tracker is 
// reponding to rule or event firings.
///////////////////////////////////////////////////////////////////////////////
graph_tracker::fire_type graph_tracker::mode() const {
  return( m_fire );
}

///////////////////////////////////////////////////////////////////////////////
// bool fire tacks the value of an enabled and enabling event and move the
// m_ps object to point to the new present state.  This function also makes
// a call to update_counts(..) which lets subclasses update their accounting
// on different values that they are tracking.  
///////////////////////////////////////////////////////////////////////////////
bool graph_tracker::fire( const double& weight,
			  const int& enabled, 
			  const int& enabling ) {
  if ( m_ps == NULL ) return( false );
  for ( statelistADT ns = m_ps->succ ; ns != NULL ; ns = ns->next ) {
    assert( ns->stateptr != NULL );
    if ( m_fire == rule ) {
      if ( ns->enabling == enabling && ns->enabled == enabled ) {
	update_counts( weight, m_ps->number, ns->stateptr->number );
	m_ps = ns->stateptr;
	return( true );
      }
    }
    else if ( ns->enabled == enabled ) {
      update_counts( weight, m_ps->number, ns->stateptr->number );
      m_ps = ns->stateptr;
      return( true );
    }	  
  }
  return( false );
}

///////////////////////////////////////////////////////////////////////////////
// Simple obeserver function which returns the value of find_prob(). 
// find_prob() is defined by subclasses to compute the desired probability
// for a specialized graph_tracker.
///////////////////////////////////////////////////////////////////////////////
prob_vector graph_tracker::probability() const {
  return( find_prob() );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// find_prob() Performs the actual probability calculation for the state
// probabilities.  The calculation is done as follows:
// p[state_number] = # times moved to state_number  / # of state transitions
///////////////////////////////////////////////////////////////////////////////
prob_vector state_tracker::find_prob() const {
  prob_vector p( m_state_count ); 
  ::transform( m_visit.begin(), m_visit.end(), p.begin(), 
	       bind2nd( divides<prob_type>(), (prob_type)m_iteration ) );
  return( p );
}

///////////////////////////////////////////////////////////////////////////////
// Must be called every time a rule or event is fired in the TEL system.  This
// records the total number of state transitions, and how many times a 
// specific state is visited.
///////////////////////////////////////////////////////////////////////////////
void state_tracker::update_counts( const double& weight,
				   const int& ps, 
				   const int& ns ) { 
  ++(m_visit[ns]);
  ++m_iteration;
}

///////////////////////////////////////////////////////////////////////////////
// Simple constructor intializes all member values.
///////////////////////////////////////////////////////////////////////////////
state_tracker::state_tracker( stateADT* s, const fire_type f ) 
  : graph_tracker( s, f ),
    m_visit( m_state_count, (index_type)0 ),
    m_iteration(0) {
  // DO NOTHING
}

///////////////////////////////////////////////////////////////////////////////
// Simple destructor required by interface.
///////////////////////////////////////////////////////////////////////////////
state_tracker::~state_tracker() {
}

///////////////////////////////////////////////////////////////////////////////
// state_tracker stores its results in the proability field in each stateADT
// found in the stateADT* s.  The index into the prob_vector for the state
// probability coresponds to the stateADT->number field.
///////////////////////////////////////////////////////////////////////////////
void state_tracker::store_results( stateADT* s ) const {
  cout << "Storing steady state probabilities to state graph ... ";
  cpu_time_tracker time;
  prob_vector p = find_prob(); 
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT curstate = s[k] ; 
	  curstate != NULL && curstate->state != NULL;
	  curstate = curstate->link ) {
      curstate->probability = p[curstate->number];
    }
  }
  time.mark();
  cout << "done: " << time << endl;
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function used to for three purposes:
//   1) Initialize m_tlist with the correct edges.  m_tlist is indexed by 
//      the state number and contains a list of edges and the count of how
//      many times that state has been visited and how many times each edge
//      in that state has been used.
//   2) Initialize all counts to 0.
//   3) Count the number of edges in the system that have the following
//      property: The edge is not the only edge leaving a state ( i.e. Count
//      an edge if it is an edge in a state where other edges exist. ).
///////////////////////////////////////////////////////////////////////////////
index_type transition_tracker::intialize_tlist( stateADT* s ) {
  index_type tlist_count = 0;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
	  ps != NULL && ps->state != NULL;
	  ps = ps->link ) {
      m_tlist[ps->number].first = 0;
      int i = 0;
      for ( statelistADT ns = ps->succ ; ns != NULL ; ns = ns->next ) {
	assert( ns->stateptr != NULL );
	m_tlist[ps->number].second.insert(::make_pair(ns->stateptr->number,0));
	++i;
      }
      if ( i > 1 )
	tlist_count += i;
    }
  }
  return( tlist_count );
}

///////////////////////////////////////////////////////////////////////////////
// Calculates the local proability of each edge in the system.  Note: This
// is note a long term transition probabilites, this is the probabilite of
// using an edge GIVEN that the system is in a particular state j.  (i.e. 
// this is a conditional probability statisfying the markov memoryless 
// property. )
// The function returns a list of probabilites of edges which makes no sense
// to anyone in this form.  Don't try and figure it out, use store_results
// to put the probabilities into the stateADT* and look at the results there.
// NOTE: Probabilities are only calculated for states that have more than 
// one outgoing edge.
///////////////////////////////////////////////////////////////////////////////
prob_vector transition_tracker::find_prob() const {
  prob_vector p( m_tlist_count);
  ::for_each( m_tlist.begin(), m_tlist.end(), prob_helper( p.begin() ) );
  return( p );
}

///////////////////////////////////////////////////////////////////////////////
// The accounting for transition tracking is done as follows:
//     1) If this state ps has a single out-going edge, return immediately.
//     2) Increment the number of times this state ps has been visited.
//     3) Find the edge taking the system to ns in the edge_list for ps.
//     4) Increment the number of times that edge has been used.
///////////////////////////////////////////////////////////////////////////////
void transition_tracker::update_counts( const double& weight,
					const int& ps, 
					const int& ns ) {
  if ( m_tlist[ps].second.size() == 1 )
    return;
  ++(m_tlist[ps].first);
  edge_list::iterator i = m_tlist[ps].second.find( ns );
  assert( i != m_tlist[ps].second.end() );
  ++((*i).second);
}

///////////////////////////////////////////////////////////////////////////////
// Simple constructor initializes all member vars.
///////////////////////////////////////////////////////////////////////////////
transition_tracker::transition_tracker( stateADT* s, const fire_type f ) 
  : graph_tracker( s, f ),
    m_tlist( m_state_count ),
    m_tlist_count( intialize_tlist( s ) ) {
  // DO NOTHING
}

///////////////////////////////////////////////////////////////////////////////
// Destructor required by the interface.
///////////////////////////////////////////////////////////////////////////////
transition_tracker::~transition_tracker() {
}

///////////////////////////////////////////////////////////////////////////////
// Stores the conditional transition probabilities in the stateADT* on the
// statelistADT found in each stateADT in stateADT*.  (i.e. Each state has
// a set of edges, and this function puts the corresponding transition 
// probability on each edge. )
///////////////////////////////////////////////////////////////////////////////
void transition_tracker::store_results( stateADT* s ) const {
  div_help help;
  cout << "Storing transition probabilities to state graph ... ";
  cpu_time_tracker time;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
 	  ps != NULL && ps->state != NULL;
 	  ps = ps->link ) {
      for ( statelistADT ns = ps->succ ; ns != NULL ; ns = ns->next ) {
 	if ( m_tlist[ps->number].second.size() == 1 ) 
 	  ns->probability = 1;
 	else {
	  edge_list::const_iterator i = 
	    m_tlist[ps->number].second.find( ns->stateptr->number );
 	  assert( i != m_tlist[ps->number].second.end() );
 	  ns->probability = help( (*i), (double)m_tlist[ps->number].first );
 	}
      }
      for ( statelistADT ns = ps->pred ; ns != NULL ; ns = ns->next ) {
 	assert( ns->stateptr != NULL );
	if ( m_tlist[ns->stateptr->number].second.size() == 1 ) 
 	  ns->probability = 1;
 	else {
	  edge_list::const_iterator i = 
	    m_tlist[ns->stateptr->number].second.find( ps->number );
 	  assert( i != m_tlist[ns->stateptr->number].second.end() );
 	  ns->probability = 
	    help( (*i), (double)m_tlist[ns->stateptr->number].first );
 	}
      }
    }
  }
  time.mark();
  cout << "done: " << time << endl;
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Calculates the state probability as the total time spent in a state 
// divided by the total simulation time of the system.
//   
//              sojourn time / total time
///////////////////////////////////////////////////////////////////////////////
prob_vector time_tracker::find_prob() const {
  prob_vector p( m_state_count ); 
  ::transform( m_sojourn.begin(), m_sojourn.end(), p.begin(), 
	       bind2nd( divides<prob_type>(), (prob_type)m_total ) );
  return( p );
}

///////////////////////////////////////////////////////////////////////////////
// Updates the time that was spent in the present state by going to the 
// sojourn list and adding weight to it.  Updates the total simulation time
// by adding weight to the m_total member variable.
///////////////////////////////////////////////////////////////////////////////
void time_tracker::update_counts( const double& weight,
				  const int& ps, 
				  const int& ns ) {
  m_sojourn[ps] += weight;
  m_total += weight;
}
  
/////////////////////////////////////////////////////////////////////////////
// Simple constructor initializes all member vars.
/////////////////////////////////////////////////////////////////////////////
time_tracker::time_tracker( stateADT* s, const fire_type f ) 
  : graph_tracker( s, f ),
    m_sojourn( m_state_count, (time_type)0.0 ),
    m_total( (time_type)0.0 ) {
}
  
/////////////////////////////////////////////////////////////////////////////
// Destructor required by the interface.
/////////////////////////////////////////////////////////////////////////////
time_tracker::~time_tracker(){
}
  
/////////////////////////////////////////////////////////////////////////////
// Stores the conditional transition probabilities in the stateADT* on the
// statelistADT found in each stateADT in stateADT*.  (i.e. Each state has
// a set of edges, and this function puts the corresponding transition 
// probability on each edge. )
/////////////////////////////////////////////////////////////////////////////
void time_tracker::store_results( stateADT* s ) const {
  cout << "Storing steady state probabilities to state graph ... ";
  cpu_time_tracker time;
  prob_vector p = find_prob(); 
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT curstate = s[k] ; 
	  curstate != NULL && curstate->state != NULL;
	  curstate = curstate->link ) {
      curstate->probability = p[curstate->number];
    }
  }
  time.mark();
  cout << "done: " << time << endl;
}

//
///////////////////////////////////////////////////////////////////////////////
