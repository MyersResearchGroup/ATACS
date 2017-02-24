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
#include <cassert>
#include <iostream>
#include <numeric>

#include "markov_matrix_func.h"
#include "markov_exp.h"

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// rate is used to return the expected value of a pdf in a rule for the given
// event enabled.  NOTE: This function searches each row of the rule matrix
// on column enabled and returns the expected value of the first entry that
// contains a rule with a valid pdf.  This means that delays associated with
// specific trigger signals do not exist!!!!!!
///////////////////////////////////////////////////////////////////////////////
prob_type rate( const unsigned int& enabled, 
		ruleADT* rules[],
		const unsigned int& nevents ) {
  for ( unsigned int i = 0 ; i < nevents ; i++ )    
    if ( rules[i][enabled] )
      if ( !( rules[i][enabled]->ruletype == NORULE ) )
	return( 1.0/rules[i][enabled]->dist->expected_value() );
  assert( false );
  return( -1 );
}

///////////////////////////////////////////////////////////////////////////////
// build_transition_rate_matrix generates an infinitesimal generator for the
// CTMC which is the stochastic process modeling the timed specification.  In
// order for this process to assert the Markov memoryless property, all firing
// distributions are modeled with negative exponential distributions whose 
// mean values are the inverse of the average waiting time of the transition.
// NOTE: This process is modeled with a SPN ( stochastic petri net ) which 
// means that each event has a single distribution associated with it.  This
// model does not allow for different delay distributions that are determined
// by a trigger signal.
///////////////////////////////////////////////////////////////////////////////
void build_transition_rate_matrix( designADT design, 
				   insert_iterator<matrix_type> i_out ) {
  for ( int k = 0 ; k < PRIME ; k++ ) {           // Loop all states
    for ( stateADT ps = design->state_table[k] ;  // in each hash entry
	  ps != NULL && ps->state != NULL ; 
	  ps = ps->link ) {
      prob_type sum =  0;
      for ( statelistADT ns = ps->succ ; ns ; ns=ns->next ) {
#ifndef __RELEASE
	assert( ( ns->enabled >= 0 ) &&           // Make sure its valid
		( ns->stateptr ) );
#endif
        // Pull out the rate which is the expected value
	prob_type trans_rate =  rate( ns->enabled, 
				      design->rules,
				      design->nevents );
	// Make the new entry in the matrix
	*i_out++ = make_value_type( (index_type)ps->number, 
				    (index_type)ns->stateptr->number,
				    trans_rate );
	// The sum is necesary for the diagonal
	sum += trans_rate;
      }
      // Save the negative rate which is the delta t relation
      *i_out++ = make_value_type( ps->number, ps->number, -1.0*sum );
    }
  }
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// build_embeded_markov_chain extracts the EMC from the CTMC represented by
// the infinitesimal generator trm.  This function will work correclty on
// any valid transition rate matrix, but does not check the matrix for
// for this assertion.
// NOTE: This will assert failure if the CTMC does not have values in
// the diagonal.
///////////////////////////////////////////////////////////////////////////////
void build_embeded_markov_chain( const matrix_type& trm,
				 insert_iterator<matrix_type> i_out ) {
  typedef matrix_type::const_iterator const_iterator;
  _matrix_interface mi;
  const_iterator i = trm.begin();
  while( i != trm.end() ) {
    // Find the diagonal element which is the normalizer
    const_iterator j = trm.find( make_key_node( mi.row( i ), mi.row( i ) ) );
#ifndef __RELEASE
    assert( j != trm.end() );
#endif
    prob_type q = -1.0 * mi.prob( j );
    j = mi.next_row( trm, i );             // Find the beginning of the next 
    while( i != j ) {                      // row.
      if ( mi.row( i ) != mi.col( i ) ) {
	*i_out++ = make_value_type( mi.row( i ), mi.col( i ),
				    mi.prob( i ) / q );
      }
      ++i;
    }
  }
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// convert_to_continuous_steady_state will take a solution to an EMC and 
// the original CTMC that the EMC was derived from and find the solution to
// the CTMC.  This new solution is assigned into i_out and reflects the 
// probability of being in any state at any time.  This is different than
// being in any state at any state transition that is represented by p, 
// the EMC solution.
///////////////////////////////////////////////////////////////////////////////
void convert_to_continuous_steady_state( prob_vector p,
					 const matrix_type& trm,
					 back_insert_iterator<prob_vector> 
					 i_out ) {
  typedef prob_vector::iterator pv_iterator;
  typedef matrix_type::const_iterator mt_const_iterator;
  _matrix_interface mi;
  pv_iterator p_out = p.begin();
  prob_type sum = 0;
  while( p_out != p.end() ) {
    unsigned int index = p_out - p.begin();     // Pull out the diagnol value
    mt_const_iterator i = trm.find( make_key_node( index, index ) );
#ifndef __RELEASE
    assert( i != trm.end() );
#endif           
    prob_type prob = ( -1.0 * (*p_out) / mi.prob( i ) ); // Calculate the prob
    *p_out++ = prob;
    sum += prob;
  }
  // Normalize the values and assign them into i_out.
  ::transform( p.begin(), p.end(), i_out, 
	       bind2nd( divides<prob_type>(), sum ) );
}

//
///////////////////////////////////////////////////////////////////////////////
