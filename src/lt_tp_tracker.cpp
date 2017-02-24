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
#include <algorithm>
#include "lt_tp_tracker.h"

/////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
// NOTE: This function is defined in findstats.c
// This function takes a vector of probabilities p and a state table s and
// saves the probabilities in the state table.  This assumes that there won't
// be a state in the state table that isn't in the probability list p when
// indexing p using the state number.  If this isn't true, there will be an
// assertion failure.
///////////////////////////////////////////////////////////////////////////////
extern void save_state_probs( stateADT* s, 
			      const prob_vector& p, 
			      bool overwrite = true );

///////////////////////////////////////////////////////////////////////////////
// NOTE: This function is defined in findstats.c
// This function will take a matrix_type m and a stateADT* s and save in
// s the values found in n.  This correctly annotate both links in the state
// table ( i.e. The pred and succ list. ).  The return value of this function
// is void, it is assumed to always work.
///////////////////////////////////////////////////////////////////////////////
extern void save_transition_probs( stateADT*s, const matrix_type& m );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
prob_vector long_term_transition_tracker::find_prob() const {
  prob_vector retVal( m_trans.size() );
  matrix_type::const_iterator i = m_trans.begin();
  prob_vector::iterator j = retVal.begin();
  while( i != m_trans.end() ) {
    *j++ = (*i++).second / m_iterations;
  }
  return( retVal );
}


void long_term_transition_tracker::update_counts( const double& weight,
						  const int& ps, 
						  const int& ns ) {
  matrix_type::iterator i = m_trans.find( (key_type)make_key_node( (index_type)ps, (index_type)ns ) );
  assert( i != m_trans.end() );
  ++m_iterations;
  ++((*i).second);
}

/////////////////////////////////////////////////////////////////////////////
// Simple constructor initializes all member vars.
/////////////////////////////////////////////////////////////////////////////
long_term_transition_tracker::
long_term_transition_tracker( stateADT* s, const fire_type f ) 
  : graph_tracker( s, f ), m_trans(), m_iterations( 0 ) {
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT ps = s[k] ; 
	  ps != NULL && ps->state != NULL;
	  ps = ps->link ) {
      for ( statelistADT ns = ps->succ ; ns != NULL ; ns = ns->next ) {
	m_trans.insert( make_value_type( ps->number, 
					 ns->stateptr->number, 
					 0 ) );
      }
    }
  }
}
  
/////////////////////////////////////////////////////////////////////////////
// Destructor required by the interface.
/////////////////////////////////////////////////////////////////////////////
long_term_transition_tracker::~long_term_transition_tracker() {
  // Nothing needs to be done here.
}
  
void long_term_transition_tracker::store_results( stateADT* s ) const {
  _matrix_interface mi;
  prob_vector lt;
  back_insert_iterator<prob_vector> i_lt( lt );
  matrix_type tp( m_trans );
  matrix_type::iterator i = tp.begin();
  while ( i != tp.end() ) {
    matrix_type::iterator j = i;
    prob_type sum = 0;
    while( j != mi.next_row( tp, i ) ) {
      sum += (*j++).second / m_iterations;
    }
    *i_lt++ = sum;
    j = i;
    i = mi.next_row( m_trans, i );
    while ( j != i ) {
      if ( sum == 0 )
	(*j).second = 0;
      else
	(*j).second = (*j).second / ( m_iterations * sum );
      ++j;
    }
  }
  save_state_probs( s, lt );
  save_transition_probs( s, tp );
}
//
/////////////////////////////////////////////////////////////////////////////
