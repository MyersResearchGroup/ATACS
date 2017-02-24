///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 beta
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
#ifndef _INTERNAL_FINDSTATS_H
#define _INTERNAL_FINDSTATS_H

#include "struct.h"
#include "markov_types.h"

/////////////////////////////////////////////////////////////////////////////
//
template <class InputIterator, class OutputIterator,
          class Predicate, class UnaryOp>
OutputIterator remove_transform_if( InputIterator first, 
				    InputIterator last,
				    OutputIterator result, 
				    Predicate pred,
				    UnaryOp transform ) {
  for ( ; first != last; ++first)
    if (!pred(*first)) {
      *result = transform( *first );
      ++result;
    }
  return result;
}
//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// vector_percent_change is to be used with the generic algorithm transform 
// and is designed to compare two distributions contained in a prob_vector 
// and show how different distribution B is when compared to A as the base.
///////////////////////////////////////////////////////////////////////////////
struct vector_percent_change 
  : binary_function<prob_type,prob_type,prob_type> {
  prob_type operator()( const prob_type& a,
			const prob_type& b ) {
    return( (b - a) / a );
  }
};

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
bool markov_reduced_power( designADT design );

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
// NOTE: ctrl-C will interupt the monte carlo simulation or the markov analysis
// without killing the atacs process.
///////////////////////////////////////////////////////////////////////////////
bool rg_trans_prob( designADT design );

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// burst_mode_trans_prob uses an algorithm designed for burst mode
// specification to calculate the transition probabilities in TER
// representation.  This algorithm is exact for burst mode machines and 
// extremely heuristic for other types of design styles.  If a burst
// mode machine is not what is beign designed, other methods may be more
// appropriate.
/////////////////////////////////////////////////////////////////////////////
bool burst_mode_trans_prob( designADT design );

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
bool monte_carlo_trans_prob( designADT design );

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
bool exp_rate_trans_prob( designADT design );

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
bool continuous_time_markov_analysis( designADT design );

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
bool monte_carlo_steady_state( designADT design );

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
bool time_tracking_stochastic_simulation( designADT design );

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
bool trigger_signal_probability( designADT design );

//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Medly runs all methods availabled for finding transition probabilities and
// steady-state distributions.  The results are written to files that are the
// name designADT->filename with one, two, or three of the following 
// extentions: tp*, the transition prability routine used ( tpsim, tpburst
// etc. ); lt, denoted the file contains long term probabilities;
// pc, which states the files show the percent change when this routine 
// is compared against the base simulation routines simtel ( steady-state )
// and tpsim ( transition ) probabilities; and cp, to denote that the
// file contains the resultant cycle period derived from the trigger probs
// derived from the steady_state and transition probabilities.
// NOTE: All of the 1 entries have been removed from the transition 
// probabilities and all of the zero entries have been removed from the
// cycle period metric.
/////////////////////////////////////////////////////////////////////////////
bool medley( designADT design );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

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
bool cycle_period_analysis( designADT design );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
bool long_term_trans_prob_simulation( designADT design );
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
bool cycle_period_simulation( designADT design );
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Simulates directly to trigger probabilities
/////////////////////////////////////////////////////////////////////////////
bool simulate_trigger_probabilities( designADT design );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

bool cycle_period_unroll( designADT design );

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
		     prob_vector& estimates            );

#endif // _INTERNAL_FINDSTATS_H
