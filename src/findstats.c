/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1996 by, Chris J. Myers                                   */
/*   Electrical Engineering Department                                       */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Univeristy of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL U. OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE LIABLE  */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* findstats.c                                                               */
/*****************************************************************************/

#include <iterator>
#include <fstream>
#include <iostream>

#include "findstats.h"
#include "simtel.h"
#include "signal_handler.h"
#include "rg_trans_prob.h"
#include "cpu_time_tracker.h"
#include "trigger_probs.h"
#include "def.h"
#include "markov_exp.h"
#include "bm_trans_probs.h"
#include "markov_matrix_check.h"
#include "cycper.h"
#include "lt_tp_tracker.h"
#include "trgtrk.h"
#include "pbsuuaad.h"
#include "stats.h"
#include <string>

#define _MAX_ITERATION_ 50000

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Simulates directly to trigger probabilities
/////////////////////////////////////////////////////////////////////////////
bool simulate_trigger_probabilities( designADT design ) {
  if ( !(design->status & LOADED) ) {
    cout << "Rats, no design is loaded" << endl << flush;
    return( false );
  }
  cout << "Calculating trigger signal probabilities ... ";
  cpu_time_tracker time;

  // New method of finding triggers by simulating directly them
  graph_tracker* g;
  if ( design->timeopts.genrg )
    g = new trigger_tracker( design->nevents );
  else
    g = new trigger_tracker( design->nevents );
  assert( simulate_tel( design, g ) );
  // End New Method
  
  save_trigger_probs( design, ((trigger_tracker*)g)->trig_prof() );
  dump_trigger_probs( design );
  time.mark();
  return( true );
}

///////////////////////////////////////////////////////////////////////////////
// Simple function that returns true if and only if a state graph has been
// found for the designADT d
///////////////////////////////////////////////////////////////////////////////
bool state_graph_exists( designADT d ) {
  return( d->status & FOUNDSTATES );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Simple function that returns true if and only if transition probabilties
// have been found for the designADT d.
///////////////////////////////////////////////////////////////////////////////
bool transition_probabilities_exists( designADT d ) {
  return( d->status & TRANSPROBS );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the TRANSPROBS bit in the status field of the designADT d
///////////////////////////////////////////////////////////////////////////////
void set_transition_probabilities( designADT d ) {
  d->status |= TRANSPROBS;
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Simple function that returns true if and only if state probabilties
// have been found for the designADT d.
///////////////////////////////////////////////////////////////////////////////
bool state_probabilities_exists( designADT d ) {
  return( d->status & STATEPROBS );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the STATEPROBS bit in the status field of the designADT d
///////////////////////////////////////////////////////////////////////////////
void set_state_probabilities( designADT d ) {
  d->status |= STATEPROBS;
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function will traverse a state graph s and extract the transition
// probabilities found on the edges.  The return value for this function is
// a transition matrix which can be used with the periodic_analysis function.
///////////////////////////////////////////////////////////////////////////////
matrix_type get_transition_probs( stateADT* s ) {
  cout << "Extracting transition probabilities ... ";
  cpu_time_tracker time;
  matrix_type trans;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
	  ps != NULL && ps->state != NULL;
	  ps = ps->link ) {
      for ( statelistADT ns = ps->succ ; ns != NULL ; ns = ns->next ) {
	  trans.insert( make_value_type( ps->number, 
					 ns->stateptr->number, 
					 ns->probability ) );
      }
    }
  }
  time.mark();
  cout << "done: " << time << endl;
  return( trans );
}

///////////////////////////////////////////////////////////////////////////////
// This function will take a matrix_type m and a stateADT* s and save in
// s the values found in n.  This correctly annotate both links in the state
// table ( i.e. The pred and succ list. ).  The return value of this function
// is void, it is assumed to always work.
///////////////////////////////////////////////////////////////////////////////
void save_transition_probs( stateADT*s, const matrix_type& m ) {
  cout << "Storing transition probabilities to state graph ... ";
  cpu_time_tracker time;
  _matrix_interface mi;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
 	  ps != NULL && ps->state != NULL;
 	  ps = ps->link ) {
      for ( statelistADT ns = ps->succ ; ns != NULL ; ns = ns->next ) {
	assert( ns->stateptr != NULL );
	matrix_type::const_iterator i = 
	  m.find( (key_type)make_key_node((index_type)ps->number,(index_type)ns->stateptr->number) );
	assert( i != m.end() );
	ns->probability = mi.prob( i );
      }
      for ( statelistADT ns = ps->pred ; ns != NULL ; ns = ns->next ) {
	assert( ns->stateptr != NULL );
	matrix_type::const_iterator i = 
	  m.find( (key_type)make_key_node( (index_type)ns->stateptr->number, (index_type)ps->number ) );
	assert( i != m.end() );
	ns->probability = mi.prob( i );
      }
    }
  }
  time.mark();
  cout << "done: " << time << endl << flush;
}

///////////////////////////////////////////////////////////////////////////////
// This function takes a vector of probabilities p and a state table s and
// saves the probabilities in the state table.  This assumes that there won't
// be a state in the state table that isn't in the probability list p when
// indexing p using the state number.  If this isn't true, there will be an
// assertion failure.
///////////////////////////////////////////////////////////////////////////////
prob_vector get_state_probs( stateADT* s ) {
  cout << "Extracting steady state probabilities in state graph ... ";
  cpu_time_tracker time;
  unsigned int num_states = 0;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
 	  ps != NULL && ps->state != NULL;
 	  ps = ps->link ) {
      ++num_states;
    }
  }
  prob_vector p( num_states ); 
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
 	  ps != NULL && ps->state != NULL;
 	  ps = ps->link ) {
      assert( (unsigned int)ps->number <= p.size() );
      p[ps->number] = ps->probability;
    }
  }
  time.mark();
  cout << "done: " << time << endl;
  return( p );
}  

///////////////////////////////////////////////////////////////////////////////
// This function takes a vector of probabilities p and a state table s and
// saves the probabilities in the state table.  This assumes that there won't
// be a state in the state table that isn't in the probability list p when
// indexing p using the state number.  If this isn't true, there will be an
// assertion failure.
///////////////////////////////////////////////////////////////////////////////
void save_state_probs( stateADT* s, 
		       const prob_vector& p, bool overwrite = true ) {
  cout << "Storing steady state probabilities to state graph ... ";
  cpu_time_tracker time;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
 	  ps != NULL && ps->state != NULL;
 	  ps = ps->link ) {
      if ( (unsigned int)ps->number < p.size() ) {
	if ( !ps->probability || overwrite ) 
	  ps->probability = p[ps->number];
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
// This function will take a matrix_type and a tolerance and perform
// a markov analysis on the matrix using the reduced power method.  The 
// reduced_power function is interuptable, so ctrl-C can be used to stop
// the caclutation and return control back to the calling program.  The return
// result of this function is a prob_vector containing the steady state values
// which are indexed with the state number.
///////////////////////////////////////////////////////////////////////////////
prob_vector reduced_power( const matrix_type& m, const double& tol ) {
  convergence_func f;
  analysis_control a( _MAX_ITERATION_, tol, f );
  cout << "Reduced power analysis ... ";
  cpu_time_tracker time;
  prob_vector long_run = reduced_power( m, a );
  time.mark();
  cout << "done: " << time << endl;
  return( long_run );
}

//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Extracts the transition probablities from the state table in design
// into a matrix, runs the reduced power method on the matrix to find the
// steady state distribution and then saves the distribution into the 
// states in the state table.
/////////////////////////////////////////////////////////////////////////////
bool markov_reduced_power( designADT design ) {
  if ( !state_graph_exists( design ) ||
       !transition_probabilities_exists( design ) ) {
    cout << "Rats, not enough information to calculate steady-"
	 << "state probabilities." << endl << flush;
    return( false );
  }
  
  cout << "Analyzing connectivity ... ";
  cpu_time_tracker time;
  matrix_vector mv = connectivity_analysis( design->state_table );
  cout << "done: " << time << endl;
  if ( mv.size() == 0 ) {
    cout << "-- Error, non-cyclic state graphs cannont be analyzed --" << endl;
    return( false );
  }
  bool overwrite = true;
  if ( mv.size() > 1 ) {
    cout << "-- Warning, found multiple strongly connected components --"
	 << endl;
    cout << "-- Warning, Each componenet will be analyzed seperately  --" 
	 << endl;
    overwrite = false;
  }
  
  for ( matrix_vector::const_iterator i = mv.begin() ; i != mv.end() ; i++ ) {
    if ((*i).size()) {
      prob_vector long_run = reduced_power( (*i), design->tolerance );
      save_state_probs( design->state_table, long_run, overwrite );
    }
  }

  set_state_probabilities( design );
  return( true );
}

//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function uses a linear approximation algorithm to calculate the
// the transition probabilties in a reduced or geometric state graph.  Once
// it has found the transition probabilities, it performs a markov chain 
// analysis using the reduced power method.  All results are stored in the
// design->state_table field.  
// NOTE: Does not create or store results to an output file
//
// THIS ROUTINE HAS MAJOR SERIOUS PROBLEMS AND COULD SEGMENT FAULT
// ATACS!!!!!! DO NOT RUN WITHOUT A REALLY GOOD REASON!
///////////////////////////////////////////////////////////////////////////////
bool rg_trans_prob( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  cout << "Calculating transition probabilities " 
       << "in the region graph using linear approximation ... ";
  cpu_time_tracker time;
  matrix_type trans = rg_linear_approx( design->rules, design->state_table );
  time.mark();
  cout << "done: " << time << endl;
  cout << endl << "trans -> " << endl;
  copy( trans.begin(), trans.end(), 
	  ostream_iterator<matrix_type::value_type>(cout,"\n") );
  cout << flush;

  save_transition_probs( design->state_table, trans );
  set_transition_probabilities( design );
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// markov_burst_mode_trans_prob uses an algorithm designed for burst mode
// specification to calculate the transition probabilities in TER
// representation.  This algorithm is exact for burst mode machines and 
// extremely heuristic for other types of design styles.  If a burst
// mode machine is not what is beign designed, other methods may be more
// appropriate.
/////////////////////////////////////////////////////////////////////////////
bool burst_mode_trans_prob( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  cout << "Calculation transition probablities using "
       << "burst-mode heuristic ... ";
  cpu_time_tracker time;
  matrix_type trans;
  typedef insert_iterator<matrix_type> inserter;
  burst_mode_transition_probabilities( design->state_table,
				       design->rules,
				       design->nevents,
				       design->tolerance,
				       inserter( trans, trans.begin() ) );
  time.mark();
  cout << "done: " << time << endl;
  save_transition_probs( design->state_table, trans );
  set_transition_probabilities( design );
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function uses a monte_carlo simulation to find the transition 
// probabilities in a region or a reduced state graph.  Once it has the 
// transition probabilities, it performs a markov chain analysis uses the
// reduced power method.  All results are stored in the design->state_table
// field.
// NOTE: Does not create or store results to an output file
// NOTE: ctrl-C will interupt the monte carlo simulation or the markov analysis
// without killing the atacs process.
///////////////////////////////////////////////////////////////////////////////
bool monte_carlo_trans_prob( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  graph_tracker* g = NULL;
  if ( design->timeopts.genrg )
    g = new transition_tracker( design->state_table, graph_tracker::rule );
  else
    g = new transition_tracker( design->state_table, graph_tracker::event );
  assert( simulate_tel( design, g ) );
  g->store_results( design->state_table );
  delete g;
  set_transition_probabilities( design );
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This method approximates all distributions in the system to be negative
// exponentials.  This approximations yield the memoryless property required
// by the Markov analysis.  With the approximation, the system can be modeled
// with a CTMC.  The EMC of the CTMC is extracted and analyzed to find the 
// longterm behavior of the system.  The longterm values as well as the 
// transition matrix from the EMC are stored into the state table.  Finally,
// the longterm probabilities are converted to the solution for the CTMC
// and the results of the this compared with those from the EMC are displayed
// along with the percent change between the two.
///////////////////////////////////////////////////////////////////////////////
bool exp_rate_trans_prob( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  cpu_time_tracker time;
  cout << "Building EMC from CTMC using exponential approximation ...";
  matrix_type trm;
  build_transition_rate_matrix( design, 
				insert_iterator<matrix_type>( trm,
							      trm.begin() ) );
  matrix_type trans;
  build_embeded_markov_chain( trm,
			      insert_iterator<matrix_type>( trans,
							    trans.begin() ) );
  time.mark();
  cout << " done: " << time << endl;
  save_transition_probs( design->state_table, trans );
  set_transition_probabilities( design );
  // cout << endl << "Percent Change -> " << endl;
//   ::transform( long_run.begin(), long_run.end(), 
// 	       clr.begin(), clr.begin(),
// 	       percent_change() );
//   ::copy( clr.begin(), clr.end(),
// 	  ostream_iterator<prob_type>( cout, "\n" ) );
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// With an exponential assumptions, the TER can be modeled as a CTMC.  The
// EMC is extracted from the CTMC, solved with the reduced power method, and
// the converted into the CTMC solution.  This process in "Numerical Solutions
// of Markov Chains" by William J. Stewart.
/////////////////////////////////////////////////////////////////////////////
bool continuous_time_markov_analysis( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  exp_rate_trans_prob( design );
  markov_reduced_power( design );
  matrix_type trm;
  build_transition_rate_matrix( design, 
				insert_iterator<matrix_type>( trm,
							      trm.begin() ) );
  prob_vector long_run = get_state_probs( design->state_table );

  prob_vector ctmc;
  cout << "Converting DTMC steady state to CTMC steady state ... ";
  cpu_time_tracker time;
  convert_to_continuous_steady_state( long_run,
				      trm,
				      back_insert_iterator<prob_vector>(ctmc));
  time.mark();
  cout << "done: " << time << endl;
  save_state_probs( design->state_table, ctmc );
  set_state_probabilities( design );
  return( true );
}

//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function will find the steady state probabilities for the state graph
// in design->state_table using a monte-carlo simulation. 
// NOTE: Does not create or store results to an output file
// NOTE: ctrl-C will interupt the monte carlo simulation or the markov analysis
// without killing the atacs process.
///////////////////////////////////////////////////////////////////////////////
bool monte_carlo_steady_state( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  graph_tracker* g = NULL;
  if ( design->timeopts.genrg )
    g = new state_tracker( design->state_table, graph_tracker::rule );
  else
    g = new state_tracker( design->state_table, graph_tracker::event );
  assert( simulate_tel( design, g ) );
  g->store_results( design->state_table );
  set_state_probabilities( design );
  delete g;
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// time_tracking_stochastic_simulation actually simulates the TER, but instead
// of counting states and edges, it tracks time spent in a state and the
// total simulation time.  The probabilities generated by this represents
// the probability of the system being in any state at any time.  This is
// very DIFFERENT from the other steady state probabilities.  This can be
// compared to the results from continuous_time_markov_analysis.
/////////////////////////////////////////////////////////////////////////////
bool time_tracking_stochastic_simulation( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  graph_tracker* g = NULL;
  if ( design->timeopts.genrg )
    g = new time_tracker( design->state_table, graph_tracker::rule );
  else
    g = new time_tracker( design->state_table, graph_tracker::event );
  assert( simulate_tel( design, g ) );
  g->store_results( design->state_table );
  set_state_probabilities( design );
  delete g;
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// trigger_signal_probability( designADT design ) is a function to cacluate
// the probability of each of a signal's trigger signals being the late 
// arriving signal and thus causing the gate to switch.  The results of the
// function are output to stdout using the print_trigger_profile function.
// The time required to complete the function is also ouput for the users 
// benefit.
///////////////////////////////////////////////////////////////////////////////
bool trigger_signal_probability( designADT design ) {

  // The old method requires steady-state and transition probabiliaties.
  // This just requirs an ER
  if ( !state_graph_exists( design ) ||
       !transition_probabilities_exists( design ) ||
       !state_probabilities_exists( design ) ) {
    cout << "Rats, not enough information to calculate trigger ";
    cout << "signal probabilities." << endl << flush;
    return( false );
  }

  cout << "Calculating trigger signal probabilities ... ";
  cpu_time_tracker time;

  // Old Method of finding trigger probabilities using steady-state and
  // transition probabilities
  trigger_profile tp = calculate_trigger_profile( design );

  save_trigger_probs( design, tp );
  dump_trigger_probs( design );
  time.mark();

  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Opens fname and points out_stream to the opened file.
/////////////////////////////////////////////////////////////////////////////
bool open_file_for_writing( const string& fname, ofstream& out_stream ) {
  out_stream.open( fname.c_str(), ios::out );
  if ( !out_stream.is_open() ) {
    cout << "Cannot open '" << fname << "', for writing" << endl;
    return( false );
  }
  return( true );
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool medley( designADT design ) {
  // Make sure there is not a skunky pointer in the designADT
  assert( design->cycle_period != NULL );

  if ( design->timeopts.genrg ) {
    cout << "Not applicable on a region graph." << endl << flush;
    return( false );
  }

  if ( !(design->status & LOADED) ) {
    cout << "Rats, no design is loaded" << endl << flush;
    return( false );
  }

  unsigned int e = 1, cycles = 1000, start = 100, replications = 5;
  graph_tracker* g = NULL;
  g = new cycle_period_tracker(e, cycles, start); 
  
  // This control object terminates as soon as the graph_tracker
  // returns 1 from the done function.  It works well for graph_trackers
  // that need to run a fixed number of iterations.
  cycle_analysis_control a;

  prob_vector ests( cycles, 0.0 );
  cpu_time_tracker time;
  for ( unsigned int i = 0 ; i < replications ; i++ ) {
    cout << "Running simulation replication " << i+1 << " ... " << flush;
    time.touch();
    ((cycle_period_tracker*)g)->clear();
    assert( simulate_tel( design, g, a ) );
    const prob_vector* tmp = ((cycle_period_tracker*)g)->vals();
    transform( tmp->begin(), tmp->end(), ests.begin(), ests.begin(), 
  	       plus<prob_type>() );
    time.mark();
    cout << "done: " << time << endl << flush;
  }
  transform( ests.begin(), ests.end(), ests.begin(),
  	     bind2nd( divides<prob_type>(), (prob_type)replications ) );

  real_type ave=0.0, adev=0.0, sdev=0.0, var=0.0, skew=0.0, curt=0.0,
            med=0, half_length = 0;
  moment( &(*ests.begin()), ests.size(), ave, adev, sdev, var, skew, curt );
  median( &(*ests.begin()), ests.size(), med );
  confidence_interval( sdev, ests.size(), 0.05, half_length );

  fprintf( stdout, "\n ----------------------------------------\n" );
  fprintf( stdout, "|           Summary Statistics           |\n" );
  fprintf( stdout, " ----------------------------------------\n" );
  fprintf( stdout, "    observations: %15d\n", ests.size() );
  fprintf( stdout, "    replications: %15d\n" , replications );
  fprintf( stdout, "confidence level: %19.3f\n", 0.95 );
  fprintf( stdout, "     half-length: %19.3f\n", half_length );
  fprintf( stdout, "             std: %19.3f\n", sdev );
  fprintf( stdout, "         avg std: %19.3f\n", adev );
  fprintf( stdout, "             var: %19.3f\n", var );
  fprintf( stdout, "            skew: %19.3f\n", skew );
  fprintf( stdout, "            curt: %19.3f\n", curt );
  fprintf( stdout, "          median: %19.3f\n", med );
  fprintf( stdout, "            mean: %19.3f\n", ave );
  
  //dump_stats( design, ests.begin(), ests.size(), replications );
  delete g;
  return( true );
}


//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

prob_type cycle_period_metric( designADT design ) {
  double metric = 0;
  for ( unsigned int i = 0 ; i < design->cycle_period->size() ; i++ ) {
    int enabling = design->markkey[i]->enabling;
    int enabled = design->markkey[i]->enabled;
#ifndef __RELEASE
    // Can't do this check because redundant rules have been set
    // to NORULE.
    // assert( design->rules[enabling][enabled]->ruletype != NORULE );
    assert( design->rules[enabling][enabled]->dist );
#endif
//    cout << endl << design->events[enabling]->event << "," 
    //         << design->events[enabled]->event
    //	 << " -> " << design->rules[enabling][enabled]->dist->expected_value()
    // << flush;
    metric += (*(design->cycle_period))[i] * 
             design->rules[enabling][enabled]->dist->expected_value();
  }
  return( metric );
}

/////////////////////////////////////////////////////////////////////////////
// dumb_cycle_period_file opens a stream pointing to the file designADT->
// filename + ".cp" and copies into that file a complete list of rules 
// with their associated weight in the designADT->cycle_period prob_vector
// pointer. The rules are grouped by enabled events and their triggers with
// a line separating each event.
// The last line of the file outputs a single cycle_period metric
// which is calculated by the function average_cycle_metric.
/////////////////////////////////////////////////////////////////////////////
bool dump_cycle_period_file( designADT design ) {
  string fname( string( design->filename ) + ".cp" );
  ofstream out_stream;
  if ( !open_file_for_writing( fname, out_stream ) ) return( false );
  cout << "Saving cycle period to `" << fname << "' ... ";
  cpu_time_tracker time;

  out_stream << "   Stochastic Cycle Period" << endl
             << "-----------------------------";

  for ( int j = 0 ; j < design->nevents ; j++ ) {
    out_stream << endl;
    for ( int i = 0 ; i < design->nevents ; i++ ) {
      if ( design->rules[i][j]->ruletype != NORULE &&
	   design->rules[i][j]->ruletype != REDUNDANT ) {
	out_stream << "<" << design->events[i]->event << ","
		   << design->events[j]->event << ">: "
		   << (*(design->cycle_period))[design->rules[i][j]->marking] 
		   << endl;
      }
    }
  }
  
  prob_type metric = cycle_period_metric( design );
  out_stream << endl << "Cycle Metric -> " << metric;
  time.mark();
  cout << "done: " << time << endl;
  out_stream.close();
  return( true );
}

/////////////////////////////////////////////////////////////////////////////
// Cycle period analysis iteratively unrolls the TER in design with a timing
// simulation to find the average cycle_period of the specification.  The
// average cycle period is defined as a weighted sum of delay values where
// the delay values are derived from the implementation and represent the
// delay of a gate when triggered by a specific input.  It is sipulated that
// this cycle_period will converge after an arbitrary number of unrollings.
// For now, convergence is controlled by designADT->tolerance.  If a max
// iteration count or convergence is acheived, unrolling is stoped and the 
// results are returned and stored into designADT->cycle_period.
/////////////////////////////////////////////////////////////////////////////
bool cycle_period_analysis( designADT design ) {
  if ( !(design->trigger_probs->size() ) ) {
    simulate_trigger_probabilities( design );
  }
  trigger_profile tp;
  get_trigger_probs( design,  back_inserter(tp) );
  design->cycle_period->erase( design->cycle_period->begin(),
			       design->cycle_period->end() );
  back_insert_iterator<prob_vector> iter( *(design->cycle_period) );
  cout << "Running cycle period simulation ... ";
  cpu_time_tracker time;
  simulate_cycle_period( tp,
			 design->events,
			 design->rules,
			 design->nevents,
			 design->nrules,
			 design->tolerance,
			 iter,
			 design->manual );
  time.mark();
  cout << "done: " << time << endl;
  return( dump_cycle_period_file( design ) );

}
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
bool long_term_trans_prob_simulation( designADT design ) {
  if ( !state_graph_exists( design ) ) {
    cout << "State graph does not exist, sorry." << endl << flush;
    return( false );
  }
  graph_tracker* g = NULL;
  if ( design->timeopts.genrg )
    g = new long_term_transition_tracker( design->state_table, 
					  graph_tracker::rule );
  else
    g = new long_term_transition_tracker( design->state_table, 
					  graph_tracker::event );
  assert( simulate_tel( design, g ) );
  g->store_results( design->state_table );
  delete g;
  set_transition_probabilities( design );
  set_state_probabilities( design );
  return( true );
}
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
extern void change_cycles(char command,designADT design);
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

void get_info( designADT design, 
	       unsigned int& e, unsigned int& i, 
	       unsigned int& start, unsigned int& iter_count ) {
  do {
    cout << "Enter event index to track(1," << design->nevents-1 << "): "
	 << flush;
    cin >> e;
    if ( e < 1 || e > (unsigned int)design->nevents-1 ) {
      cout << "Bad event index value!" << endl << flush;
    }
  } while( e < 1 || e > (unsigned int)design->nevents-1 ); 
  do {
    cout << "Enter number of cycles to consider: ";
    cin >> i;
    if ( i < 1 )
      cout << "Bad number of cycles!" << endl << flush;
  } while( i < 1 );
  do {
    cout << "Enter cycle index to start from: ";
    cin >> start;
    if ( start < 0 ) 
      cout << "Bad cycle index value!" << endl << flush;
  } while( start < 0 );
  do {
    cout << "Enter number of random experiments: ";
    cin >> iter_count;
    if ( iter_count < 1 )
      cout << "Bad number of random experiments!" << endl << flush;	  
  } while ( iter_count < 1 );
}

bool cycle_period_unroll( designADT design ) {

  // Make sure there is not a skunky pointer in the designADT
  if (  design->cycle_period == NULL ) {
    cout << "ERROR: memory is not allocated for design->cycle_period"
	 << endl << flush;
  }
  
  // Make sure that we have a loaded design
  if ( !(design->status & LOADED) ) {
    cout << "Rats, no design is loaded" << endl << flush;
    return( false );
  }

  unsigned int e = 0, cycles = 0, start = 0, replications = 0;
  get_info( design, e, cycles, start, replications );

  if ( cycles+start > (unsigned int)design->ncycles ) {
    design->ncycles = cycles+start;
    change_cycles( 'C', design );
  }

  graph_tracker* g;
  g = new cycle_tracker( design->rules, 
			 design->cycles,
			 design->nevents,
			 design->ncycles,
			 e, start,
			 e, cycles + start );

  cycle_analysis_control a;
  cpu_time_tracker time;
  
  // Initialize the cycle_period vector to be all zero entries.  The
  // first clause is to grow the vector if necessary
  if ( (int)(design->cycle_period->size()) < design->nrules ) {
    for ( int i = design->cycle_period->size() ; i < design->nrules ; i++ )
      design->cycle_period->push_back( 0 );
  }
  fill( design->cycle_period->begin(), design->cycle_period->end(), 0.0 );
    
  prob_vector ests( cycles, 0.0 );
  for ( unsigned int i = 0 ; i < replications ; i++ ) { 
    cout << "Running simulation replication " << i+1 << " ... " << flush;
    time.touch();
    assert( simulate_tel( design, g, a ) );
    const prob_vector* tmp = ((cycle_tracker*)g)->vals();
    transform( tmp->begin(), tmp->end(), ests.begin(), ests.begin(), 
	       plus<prob_type>() );
    if ( !( ((cycle_tracker*)g)->backtrace( design->cycle_period ) ) )
      return( false );
    ((cycle_tracker*)g)->clear();
    time.mark();
    cout << "done: " << time << endl << flush;
  }
  transform( design->cycle_period->begin(), design->cycle_period->end(), 
	     design->cycle_period->begin(), 
	     bind2nd( divides<prob_type>(), (prob_type)replications ) );
  transform( ests.begin(), ests.end(), ests.begin(),
  	     bind2nd( divides<prob_type>(), (prob_type)replications ) );
  dump_cycle_period_file( design );

  // Stop, do not call only 1 observation!
  if ( replications > 1 ) {
    dump_stats( design, &(*ests.begin()), ests.size(), replications );
  }

  return( true );
}

//
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Runs several random simulation experiments and puts the results from
// each simulation in the estimates vector.  A simulation result 
// is a real valued number denoting the average amount of time that 
// elapses between two consectutive instances of event.
//
// event: the event tracked in the simulation. 
// cycles_to_consider: the number of instances of the event to consider
//                     in calculating its average cycle period
// starting_cycle: a simulation does not record information until
//                 it has seen this many instances of the event
// estimates: storage for the result of each simulation; its size
//            denotes the number of random independent simulations to run.
//------------------------------------------------------------------------
bool run_simulation( designADT design                ,
		     unsigned int event              ,
		     unsigned int cycles_to_consider ,
		     unsigned int starting_cycle     ,
		     prob_vector& estimates            ) {
  graph_tracker* g = NULL;
  g = new cycle_period_tracker( event              , 
			        cycles_to_consider , 
			        starting_cycle       ); 
  cycle_analysis_control a;
  cpu_time_tracker time;
  for ( unsigned int i = 0 ; i < estimates.size() ; i++ ) {
    cout << "Running simulation replication " << i+1 << " ... " << flush;
    time.touch();
    ((cycle_period_tracker*)g)->clear();
    if(!( simulate_tel( design, g, a ) )){
      return false;
    }
    estimates[i] = ((cycle_period_tracker*)g)->cycle_metric();
    time.mark();
    cout << "done: " << time << endl << flush;
  }
  delete g;
  return true;
}
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
bool cycle_period_simulation( designADT design ) {

  // Make sure there is not a skunky pointer in the designADT
  assert( design->cycle_period != NULL );

  if ( design->timeopts.genrg ) {
    cout << "Not applicable on a region graph." << endl << flush;
    return( false );
  }

  if ( !(design->status & LOADED) ) {
    cout << "Rats, no design is loaded" << endl << flush;
    return( false );
  }

  unsigned int e = 0, cycles = 0, start = 0, replications = 0;
  get_info( design, e, cycles, start, replications );
  prob_vector estimates( replications, 0.0 );
  if(!run_simulation( design, e, cycles, start, estimates )){
    return false;
  }
  dump_stats( design, &(*estimates.begin()), estimates.size(), cycles );
  return( true );
}
