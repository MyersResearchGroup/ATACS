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

#include <stack>
#include <list>

#include "markov_matrix_func.h"
#include "markov_matrix_check.h"

typedef stateADT queue_entry;

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// clear_transient_edges is a method of tracing states from the initially
// marked state and marking any transitions that come from a transient 
// initial state to have zero probability.  In this way, is the 
// get_transition_probs function is modified and zero probability transitions
// aren't saved in the matrix, then the resulting matrix SHOULD be strongly
// connected, although this cannot be verified!
/////////////////////////////////////////////////////////////////////////////
bool clear_transient_edges( stateADT states[] ) {
  bool retVal( false );
  clearColors( states, 0 );
  // Seed the search queue with the initial state
  front_queue<queue_entry> search_queue;
  search_queue.push( find_state_zero( states ) );
  (search_queue.front())->color ^= 1;
  do {
    stateADT current = search_queue.front();
    search_queue.pop();
    //cout << endl << endl << "current -> " << current->state;
    // Loop on each state in the successor list
    for ( statelistADT i = current->succ ; i != NULL ; i = i->next ) {
      //cout << endl << "Analyzing -> " << i->stateptr->state;
      bool ok = false;
      // Now check all of the predecessors to the state
      for ( statelistADT j = i->stateptr->pred ; j != NULL ; j = j->next ) {
	// This edge comes from a state that is not initialy marked, so
	// this state is not redundant.
	if ( !j->stateptr->color ) {
	  //cout << endl << "Not Marked -> " << j->stateptr->state;
	  ok = true;
	}
	// This edge comes from a state that is initially marked, so the
	// edge should be cleared.
	else {
	  j->probability = 0;
	  i->probability = 0;
	}
      }
      // The state is redundant, so color the state and add it to
      // the search queue.
      if ( !ok ) {
	if ( !retVal ) retVal = true;
	i->stateptr->color ^= 1;
	search_queue.push( i->stateptr );
      }
    }
  } while( !search_queue.empty() );
  return( retVal );
}

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// state_count returns the number of states in the system.
/////////////////////////////////////////////////////////////////////////////
unsigned int state_count( stateADT s[] ) {
  int state_count = -1;
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT curstate = s[k] ; 
	  curstate != NULL && curstate->state != NULL;
	  curstate = curstate->link ) {
      if ( curstate->number > state_count )
	state_count = curstate->number;
    }
  }
  return( ++state_count );
}

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
// transient_or_absorbing is a unary function object that returns true IFF
// the matrix passed to it represents a transient component or an absorbing
// component of the graph.
/////////////////////////////////////////////////////////////////////////////
struct transient_or_absorbing 
  : public unary_function<matrix_type,bool> {

  _matrix_interface mi;

  bool operator()( const matrix_type& m ) {
    if ( m.empty() ) return( true );
    // Look at each entry in the matrix (graph)
    for ( matrix_type::const_iterator i = m.begin() ; i != m.end() ; i++ ) {
      matrix_type::const_iterator j = m.find( make_key_node( mi.row(i),
							     mi.row(i) ) );
      // If there is an entry at location (i,i) and that entry is
      // equal to 1, then this must be an absorbing state, return true.
      if ( j != m.end() ) {
	if ( mi.prob( j ) == 1 ) {
// 	  cout << endl << "Returned true for " << endl;
// 	  copy( m.begin(), m.end(), 
// 		ostream_iterator<matrix_type::value_type>(cout,"\n") );
	  return( true );
	}
      }
      // If I have an entry (i,j), then there better be a non-zero entry
      // in row j or this must be a transient component, matrix, or graph
      j = mi.begin( m, mi.col( i ) );
      if ( j == m.end() ) {  
//  	cout << endl << "Returned true, failed row check on " << mi.col(i)
//  	     << endl;
//  	copy( m.begin(), m.end(), 
//  	      ostream_iterator<matrix_type::value_type>(cout,"\n") );
	return( true );
      }
    }
    return( false );
  }

};

/////////////////////////////////////////////////////////////////////////////
// strongly_connected is a modified depth first search to find strongly
// connected components of the graph.  Each vertex of a strongly connected 
// component with its accompaning edges that have non-zero probablitiy are
// assigned to a matrix_type.  When the component is complete, the matrix
// is assigned out to the iterator i_out.  This algorithm is proven O(V+E) 
// is covered in depth on page 482 of "Algorithms in C++" by Rober Sedgewick
// ISBN 0-201-51059-6
/////////////////////////////////////////////////////////////////////////////
template<class mv_output_iter>
unsigned int strongly_connected( stateADT k,
				 vector<unsigned int>& val,
				 stack<stateADT>& search_stack,
				 unsigned int& id,
				 mv_output_iter i_out ) {
  val[k->number] = ++id;
  unsigned int min = id;
  search_stack.push( k );
  unsigned int m = 0;
  for ( statelistADT t = k->succ ; t != NULL ; t = t->next ) {
    if ( !t->probability ) continue;
    m = ( !val[t->stateptr->number] ) ? strongly_connected( t->stateptr,
							    val,
							    search_stack,
							    id,
							    i_out ) : 
                                        val[t->stateptr->number];
    if ( m < min ) min = m;
  }
  if ( min == val[k->number] ) {
    //cout << endl << endl;
    matrix_type mat;
    do {
      m = (search_stack.top())->number;
      for ( statelistADT t = (search_stack.top())->succ ; 
	    t != NULL ; t = t->next ) {
	if ( !t->probability ) continue;
	mat.insert( make_value_type( (search_stack.top())->number,
				     t->stateptr->number,
				     t->probability ) );
      }
//       cout << endl << "State -> " << (search_stack.top())->number 
// 	   << ":" << (search_stack.top())->state;
      search_stack.pop();
    } while( m != (unsigned int)k->number );
//    cout << endl << endl;
    *i_out++ = mat;
  }
  return( min );
}

/////////////////////////////////////////////////////////////////////////////
// connectivty anlaysis returns a matrix_vector containing the non-transient
// non-absorbing strongly connected components of the state graph represented
// in states.  If a the return type is 0, then the state graph is non-cyclic
// and cannot be analyzed with Markovian techniques.  Otherwise, the matrix
// vector will contain all strongly connected components.
/////////////////////////////////////////////////////////////////////////////
matrix_vector connectivity_analysis( stateADT states[] ) {
  // The val vector tracks the order that each state is visited.  It also
  // denotes the fact that a state has been visited.
  unsigned int num_states = state_count( states );
  vector<unsigned int> val( num_states, 0 );
  // search_stack is used to keep track of strongly connected components
  // on the strongly_connected algorithm.  Its empty after the call
  stack<stateADT> search_stack;
  // Id is the counter that increments on each NEW node that is found
  unsigned int id = 0;
  // Storage for the results of strongly_connected
  matrix_vector mv;
  back_insert_iterator<matrix_vector> i_out( mv );
  strongly_connected( find_state_zero(states), val, search_stack, id, i_out );

  // Remove all components from the matrix_vector mv that are transient
  // or absorbing
  mv.erase( ::remove_if( mv.begin(), mv.end(), transient_or_absorbing() ),
            mv.end() );

//   for ( matrix_vector::const_iterator i = mv.begin() ; i != mv.end() ; i++ ) {
//     cout << endl << endl << "scc -> " << endl;
//     ::copy( (*i).begin(), (*i).end(),
//  	    ostream_iterator<matrix_type::value_type>(cout,"\n") );
//   }

  return( mv );
}

//
/////////////////////////////////////////////////////////////////////////////
