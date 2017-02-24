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
#include "def.h"

#include <cassert>
#include <iostream>
#include <algorithm>
#include <numeric>
//#ifdef __GCC3_INC__
//#include <ext/functional>
//#else
//#include <function.h>
//#endif

#include "cycper.h"
#include "bm_trans_probs.h"  // Has the front_queue container adapter
#include "simtel.h"

//#ifdef __GCC3_INC__
//using namespace __gnu_cxx;
//#endif

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Minus divide is a function object that implements a binary function that
// will subtrack the right operand from the left operand and return that 
// result divided by m_div.
/////////////////////////////////////////////////////////////////////////////
struct minus_divide 
  : public binary_function<prob_type,prob_type,prob_type>{
  
  prob_type m_div;

  /////////////////////////////////////////////////////////////////////////////
  // Constructor to capture value of m_div
  /////////////////////////////////////////////////////////////////////////////
  minus_divide( const prob_type& p ) : m_div( p ) {
    if ( p == 0 )
      m_div = 1;
  }

  /////////////////////////////////////////////////////////////////////////////
  // standard stl function object interface, a -> left operand and b ->
  // right operand.
  /////////////////////////////////////////////////////////////////////////////
  prob_type operator()( const prob_type& a,
			const prob_type& b ) {
    return( ( a - b ) / m_div );
  }

};

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// conststucts a cycle period using rules as the mapping to change cartisan
// (x,y) coordinates into flat i coordinates in a one-dimensional array.  This
// relates the rules to a markkey.  The equation set m_eqns is constructed to
// be nevents x nrules and used to track the evolving cycle period as events
// are fired.  event is the event index to use in for the m_base vector and
// to calculate the actual cycle_period defined as the difference in 
// coefficients between the current and base vectors divided by the period
// count minus 1.  tp is the list of trigger rules that actually do will
// appear in the gate implementation.
/////////////////////////////////////////////////////////////////////////////
cycle_period::cycle_period( ruleADT* rules[],
			    const unsigned int& nevents,
			    const unsigned int& nrules,
			    const unsigned int& event,
			    const trigger_profile& tp )
  : m_rules( rules ),
    m_eqns( nevents ), 
    m_period_count( 0 ), 
    m_event( event ),
    m_trig_prof( tp ),
    m_base( nrules, 0 ) {
#ifndef __RELEASE
  assert( event < nevents );                // Do a little size checking
  assert( m_trig_prof.size() == nevents );
#endif
  prob_vector tmp( nrules, 0 );             // Allocate memory for each entry
  for ( vector<prob_vector>::iterator i = m_eqns.begin() ; 
	i != m_eqns.end() ; 
	i++ ) {
    (*i) = tmp;                             // Builds the matrix
  }
}

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
prob_vector cycle_period::probability() const {
  prob_vector retVal( m_base.size(), 0 );
  if ( m_period_count <= 1 ) return( retVal );
  ::transform( m_eqns[0].begin(), m_eqns[0].end(),
	       m_base.begin(), retVal.begin(),
	       minus_divide( m_period_count - 1 ) );
  
  return( retVal );
}

/////////////////////////////////////////////////////////////////////////////
// operator() is used to actually fire an event and update the corresponding
// time values in the m_eqns matrix.  The event entry in the m_eqns is 
// updated as the wieghted sum of the time of each of its trigger signal
// fire times plus the weighted sum of the events delay based on its 
// trigger rule.
/////////////////////////////////////////////////////////////////////////////
void cycle_period::operator()( const unsigned int& event ) {
  ::fill( m_eqns[event].begin(), m_eqns[event].end(), 0 );
  for ( trigger_list::const_iterator i = m_trig_prof[event].begin() ;
	i != m_trig_prof[event].end() ;
	i++ ) {
    // Find the column or index into the matrix
    unsigned int col = m_rules[(*i).first][event]->marking;
#ifndef __RELEASE
    assert( col < m_eqns[event].size() );
#endif
    // For each trigger rule, copy the weight values into event's row
    // CJM: COMMENTED OUT BELOW, HACK TO WORK ON MAC
    // ::transform( m_eqns[(*i).first].begin(), m_eqns[(*i).first].end(),
    // 		 m_eqns[event].begin(), m_eqns[event].begin(),
    // 		 construct2( plus<prob_type>(),
    // 			     bind2nd( multiplies<prob_type>(), (*i).second ),
    // 			     identity<prob_type>() ) );
    // Add to the existing value,  the weighted delay added by event.
    m_eqns[event][col] += (*i).second;
  }
//   cout << endl << endl << "operator(" << event << ") -> ";
//   cout << endl;
  //   write_string( cout );
}

/////////////////////////////////////////////////////////////////////////////
// increment() will snapshot the row specified m_event into row 0 and 
// increment the current value of m_period_count one time.  In this way, the
// correct cycle period can be found with a call to probability, until the
// next cycle is completed.
/////////////////////////////////////////////////////////////////////////////
void cycle_period::increment() {
  if ( m_period_count == 0 ) 
    copy( m_eqns[m_event].begin(), m_eqns[m_event].end(), m_base.begin() );
  copy( m_eqns[m_event].begin(), m_eqns[m_event].end(), m_eqns[0].begin() );
  ++m_period_count;
}

/////////////////////////////////////////////////////////////////////////////
// Copies the m_eqns matrix to the stream s.
/////////////////////////////////////////////////////////////////////////////
void cycle_period::write_string( ostream& s ) {
  for ( vector<prob_vector>::const_iterator i = m_eqns.begin() ; 
	i != m_eqns.end() ;  ) {
    for ( prob_vector::const_iterator j = (*i).begin() ; 
	  j != (*i).end() ; 
	  j++ ) {
      s.width( 5 );
      s << *j;
    }
    if ( ++i != m_eqns.end() )
      s << endl;
  }
}


//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
// clearColors sets the ruleADT->reachable and eventADT->color fields to 
// color.  Color can only be zero or one.
/////////////////////////////////////////////////////////////////////////////
void clearColors( ruleADT* rules[], 
		  eventADT events[],
		  const unsigned int& nevents, 
		  const unsigned int& color ) {
  for ( unsigned int i = 0 ; i < nevents ; i++ ) {
    events[i]->color = color;
    for ( unsigned int j = 0 ; j < nevents ; j++ ) {
      rules[i][j]->reachable = color;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// find_initial_event will just return 0 for now.  In this application, the
// timing simulation will always start with the reset event, which is initially
// fired, and events that become enabled are then queued up to be fired.
/////////////////////////////////////////////////////////////////////////////
unsigned int find_initial_event( ruleADT* rules[], 
				 const unsigned int& nevents ) {
  return( 0 );
}

/////////////////////////////////////////////////////////////////////////////
// fire_event traverses the row in the rule matrix corresponding to event
// and toggles the ruleADT->reachable bit to reflect the fact that the event
// has fired and all rules that it enables have fired too.
/////////////////////////////////////////////////////////////////////////////
void fire_event( const unsigned int& event,
		 ruleADT* rules[],
		 const unsigned int& nevents ) {
  for ( unsigned int i = 0 ; i < nevents ; i++ ) {
    rules[event][i]->reachable ^= 1;
  }
}

/////////////////////////////////////////////////////////////////////////////
// rule_exists traverses a column corresponding to event in the rules matrix
// and returns true iff the column contains a valid rule entry, otherwise it
// returns false.
/////////////////////////////////////////////////////////////////////////////
bool rule_exists( const unsigned int& event,
		  ruleADT* rules[],
		  const unsigned int& nevents ) {
  for ( unsigned int i = 1 ; i < nevents ; i++ ) {
    if ( rules[i][event]->ruletype ) {
      return( true );
    }
  }
  return( false );
}

/////////////////////////////////////////////////////////////////////////////
// conflict_status is yet, an unchecked function taken directly from 
// the liveness check in connect.c.  This function, in theory, returns true
// if an event can be fired by an initial marking or a conflict sitution.
/////////////////////////////////////////////////////////////////////////////
bool conflict_status( const unsigned int& enabling,
		      const unsigned int& enabled,
		      ruleADT* rules[],
		      const unsigned int& nevents, 
		      bool fired) {

  for ( unsigned int k = 1 ; k < nevents ; k++ ) {
    if ( ( rules[enabling][k]->conflict ) && 
	 ( rules[k][enabled]->ruletype ) && 
	 ( ( rules[k][enabled]->reachable == fired) /*|| 
	      ( rules[k][enabled]->epsilon == 1 )*/ ) )
      return( true );
  }
  return( false );
}

/////////////////////////////////////////////////////////////////////////////
// run_simulation_cycle will take an initially marked TER and fire events
// and flow tokens until all events the system have fired ( although,
// I'm not sure what happens with conflicts ).  Each time an event in the
// TER is fired, the timing simulation in the cycle_period object cp is updated
// with the newly fired event.
/////////////////////////////////////////////////////////////////////////////
void run_simulation_cycle( ruleADT* rules[],
			   eventADT events[],
			   const unsigned int& nevents,
			   bool fired,
			   cycle_period& cp ) {
  bool still_firing_events = false;
  do {
    still_firing_events = false;
    
    // Loop through the entire rule matrix
    for ( unsigned int j = 1 ; j < nevents ; j++ ) {
      bool fireable = rule_exists( j, rules, nevents );
      for ( unsigned int i = 1 ; i < nevents && fireable; i++ ) {

	int eps=rules[i][j]->epsilon;
	for (unsigned int k=1;k<nevents;k++)
	  if ((rules[i][k]->conflict) &&
	      (rules[k][j]->ruletype != NORULE) &&
	      (rules[k][j]->ruletype < ORDERING)) 
	    if (rules[k][j]->epsilon==0) eps=0; 

	if ( rules[i][j]->ruletype ) {
	  if ( ( rules[i][j]->reachable != fired ) && 
	       ( /*rules[i][j]->epsilon*/ eps == 0 ) ) {
	    // Check the conflict and initial marked status
	    fireable = conflict_status( i, j, rules, nevents, fired );
	  }
	}
      }
      // Must be a good event to fire iff it hasn't already been fired.
      if ( fireable && events[j]->color != fired ) {
	//cout << endl << "fire_event( rules, nevents, " << events[j]->event 
	//     << ")";
	events[j]->color ^= 1;
	fire_event( j, rules, nevents );
	cp( j );
	still_firing_events = true;
      }
    }
  } while( still_firing_events ); // Fired all events, time to stop trying
}

/////////////////////////////////////////////////////////////////////////////
// Returns the index of an event to use in the cycle period calculation which
// does not conflict with any other event in the system.
/////////////////////////////////////////////////////////////////////////////
unsigned int find_cycle_event( ruleADT* rules[], 
			       const unsigned int& nevents,
			       bool prompt ) {
  if ( prompt ) {
    unsigned int retVal = 0;
    cout << endl << "Enter an event to use as a cycle marker: ";
    cin >> retVal;
    return( retVal );
  }
  for ( unsigned int i = 1 ; i < nevents ; i++ ) {
    bool good = true;
    bool rule_exists = false;
    for ( unsigned int j = 1 ; j < nevents ; j++ ) {
      if ( rules[i][j]->conflict ) {
	good = false;
	break;
      }
      if ( rules[i][j]->ruletype )
	rule_exists = true;
    }
    if ( good && rule_exists )
      return ( i );
  }
  return( 0 );
}

/////////////////////////////////////////////////////////////////////////////
// simulate_cycle_period will repetively unroll a TER structor by doing a 
// timing simulation as described by Burns in his PhD.  The unrolling stops
// when the cycle period converges to a within the specified tolerance or 
// achieves the maximum number of iterations allowed in the unrolling.  The
// resultant cycle period is assigned to i_out as an array of coefficients
// whose indexes are associated with the ruleADT->marking field in the
// rule matrix rules.  The trigger_profile is necesary to know those
// rules which actually are used as triggers in the imlementation.  When
// this completes, the reachable and color fields in rules and events 
// repectively, will have the same boolean value.
/////////////////////////////////////////////////////////////////////////////
bool simulate_cycle_period( const trigger_profile& tp,
			    eventADT events[],
			    ruleADT* rules[],
			    const unsigned int& nevents,
			    const unsigned int& nrules,
			    const CPdf::real_type& tolerance,
			    back_insert_iterator<prob_vector> i_out,
			    bool prompt ) {
  int current_color = 1;
  clearColors( rules, events, nevents, !current_color );

  // Its important to use a conflict free event to compute the cycle
  // period.  find_cycle_event returns the index of the first event
  // which doesn't conflict with anything.
  unsigned int cycle_event = find_cycle_event( rules, nevents, prompt ); 
  if ( !cycle_event ) {
    cout << " -- Error, can't find a conflict-free event -- ";
    return( false );
  }
  cycle_period cycle( rules, nevents, nrules, cycle_event, tp );

  // The default convergence func will check every five cycles for to see
  // if the cycle_period has actually converged as long as the iteration count
  // is under 100.  As finer grain custom convergence function may be wanted
  // in this application.
  convergence_func f = convergence_func();
  generic_analysis_control<cycle_period> done( 50000, tolerance, f );

  do {
    
    cycle( 0 );                       // Fire the reset rule
    fire_event( 0, rules, nevents ); 
    events[0]->color ^= 1;            // Toggle color on the reset event
    run_simulation_cycle( rules, events, nevents, current_color, cycle );
    current_color ^= 1;               // Toggle the current_color for the next
    cycle.increment();                // run and add 1 to the cycle count

  } while( !done( cycle ) );
  
  // Pull out the computed cycle period and dump it to the i_out
  prob_vector pv = cycle.probability();
  copy( pv.begin(), pv.end(), i_out );
  
  return( true );
}

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
prob_vector cycle_period_tracker::find_prob() const {
  // If m_iterations is equal to m_stop_index, then the simulation
  // should terminate.
  if ( m_iterations < m_stop_index )
    return( prob_vector( 1, 0.0 ) );
  return( prob_vector( 1, 1.0 ) );
}

void cycle_period_tracker::update_counts( const double& weight,
					  const int& ps, 
					  const int& ns ) {
  // This is required by the interface but not used in 
  // this implementation.  All the tracking information is 
  // acknowledged and recorded by the fire function.
}

cycle_period_tracker::cycle_period_tracker( unsigned int event_index,
					    unsigned int reps,
					    unsigned int start_index )
  : m_iterations(0),
    m_time(0),
    m_stop_index( reps + start_index ),
    m_event_index( event_index ),
    m_start_index( start_index ),
    m_vals( NULL ) {

  graph_tracker::m_fire = graph_tracker::event;
  if ( reps != 0 ) {
    m_vals = new prob_vector( reps, 0.0 );
  }

}
  
cycle_period_tracker::~cycle_period_tracker() {
  if ( m_vals != NULL ) {
    delete m_vals;
  }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool cycle_period_tracker::fire( const double& weight,
				 const int& enabled,
				 const int& enabling ) {
  // Update the elapsed time between events
  m_time += weight;

  // If this enabled event is the event of interest, then
  // update the m_iteration count and record statistics.
  if ( enabled == (int)m_event_index ) {

    // If m_iterations is above the m_start_index (cycle marker), then
    // record that amount of time that has elapsed since the last firing
    // of m_event_index.
    if ( ++m_iterations > m_start_index ) {
      assert( m_iterations-1-m_start_index < m_vals->size() );
      (*m_vals)[m_iterations-1-m_start_index] = m_time;
    }

    // reset the elapsed time tracker m_time so it can recard
    // the time of the next cycle of m_event_index
    m_time = 0;  

  }
  return( true );
}
  
cycle_period_tracker::real_type cycle_period_tracker::cycle_metric() const {
  return( accumulate( m_vals->begin(), m_vals->end(), 0.0 ) / 
	  (prob_type)m_vals->size() );
}

void cycle_period_tracker::clear() {
  m_iterations = 0;
  m_time = 0;
}

void cycle_period_tracker::store_results( stateADT* s ) const {
  // I have nothing to store in this object.
}
//
/////////////////////////////////////////////////////////////////////////////
