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
#ifndef __INTERNAL_BM_TRANS_PROB_H
#define __INTERNAL_BM_TRANS_PROB_H

#include "def.h"

#include <vector>
#include <list>

#include "struct.h"
#include "markov_types.h"
#include "CPdf.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// find_state_zero systematically tranverses the stateADT s[] until it finds
// a state that is assigned to number zero and returns the pointer to that
// state.
/////////////////////////////////////////////////////////////////////////////
stateADT find_state_zero( stateADT s[] );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// clearColors traverses the state_table[] setting all color entries to the
// value color.  The actual location of this code is in stateasgn.c and is
// not exported through the stateasgn.h interface, hence the extern statement
// bringing it into this scope.
/////////////////////////////////////////////////////////////////////////////
extern void clearColors( stateADT state_table[], int color );

//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// find_state_zero systematically tranverses the stateADT s[] until it finds
// a state that is assigned to number zero and returns the pointer to that
// state.
/////////////////////////////////////////////////////////////////////////////
stateADT find_state_zero( stateADT s[] );

//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// The pdf_cache is used to associate event numbers with rule distributions.
// Under this implementation, when a lookup is invoked on an event, the 
// cache searches the rule matrix until it finds the first rule associated 
// with the specified event number.  Once the rule is located, its pdf
// (ruleADT->dist) is placed in a cache, to speed up future lookups and
// dist is returned to the caller.
// NOTE: This enforces a single distribution for each event, irregardless of
// the actual trigger signal.  More advanced distributions that are based 
// on the trigger cannot be modeled with this structure.
/////////////////////////////////////////////////////////////////////////////
class pdf_cache {
protected:
  
  typedef const CPdf*         CPdf_ptr;
  typedef vector<CPdf_ptr>    CPdf_ptr_vector;
  
  CPdf_ptr_vector  m_cache;   // CPdf cache
  ruleADT**        m_rules;   // Rule matrix where the CPdfs are found

protected:

  /////////////////////////////////////////////////////////////////////////////
  // Searched the rule matrix on the column specified by event until it finds 
  // the first none NORULE entry.  Once this entry(rule) is located, 
  // the function return rule->dist which is the CPdf associated with the rule.
  // NOTE: This returns the distribution in the first rule it finds, therefore
  // each unique event maps to a single distribution irregardless of the
  // trigger signal or causal rule.
  /////////////////////////////////////////////////////////////////////////////
  CPdf_ptr find_pdf( const unsigned int& event );

public:

  /////////////////////////////////////////////////////////////////////////////
  // constructor that initializes the cache to empty and directs the 
  // rule matrix pointer to rules.
  /////////////////////////////////////////////////////////////////////////////
  pdf_cache( const unsigned int& nevents, ruleADT** rules );

  /////////////////////////////////////////////////////////////////////////////
  // I wonder why I keep adding these things, since they never actually
  // need to contain code!
  /////////////////////////////////////////////////////////////////////////////
  ~pdf_cache();

  /////////////////////////////////////////////////////////////////////////////
  // lookup checks the cache at location event and if its full returns the
  // cache contents.  Otherwise, it goes to the rule matrix and finds 'a' CPdf
  // for the event, places the CPdf in the cache at the event location, and 
  // then returns the CPdf.
  /////////////////////////////////////////////////////////////////////////////
  CPdf_ptr lookup( const unsigned int& event );

};
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// front_queue is a container adapter for sequenced containers that models
// more religiously an actual queue.  This implies that this adapter provides
// absolutely no method for accessing the back of the queue.  The only visible
// part is the front element of the queue.  To use this adapter, a sequence
// must support the following functions:
//
//                  size() : Returns the number of elements in the sequence
//                  empty(): Returns true is no elements are in the sequence
//                  front(): Returns a reference or value_type& to the first
//                           element in the sequence ( const and non-const
//                           versions of this )
//  insert( iterator i, T ): This is necessary to add new elements to the 
//                           end of the sequence.
//              pop_front(): Physically removes the first element from the
//                           sequence.
//          operator==(...): Returns true iff two sequences are equal
//           operator<(...): Returns true iff the left sequence is less
//                           than the right sequence
//
/////////////////////////////////////////////////////////////////////////////
template <class T, class Sequence = list<T> >
class front_queue {
public:
  
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;
  
protected:

  Sequence c; // Storage class used to manage elements in the queue
  
public:
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns true iff the are no elements currently in the queue
  /////////////////////////////////////////////////////////////////////////////
  bool empty() const { 
    return c.empty(); 
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns the number of elements currently in the queue
  /////////////////////////////////////////////////////////////////////////////
  size_type size() const { 
    return c.size(); 
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns a non-constant reference to the first element in the queue
  /////////////////////////////////////////////////////////////////////////////
  value_type& front() { 
    return c.front(); 
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // returns a constant reference to the first element in the queue
  /////////////////////////////////////////////////////////////////////////////
  const value_type& front() const { 
    return c.front(); 
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // pushes the object x onto the back of the queue
  /////////////////////////////////////////////////////////////////////////////
  void push( const value_type& x ) { 
    c.insert( c.end(), x ); 
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Removes the first element from the queue.  I have no idea what this will
  // do if its run on an empty queue.  It really depends on the behavior of
  // the class sequence and how its defined to handle a pop on an empty
  // sequence.
  /////////////////////////////////////////////////////////////////////////////
  void pop() { 
    c.pop_front(); 
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns true iff the 'this' queus the y are equal. Equality is defined
  // by the Sequence class.
  /////////////////////////////////////////////////////////////////////////////
  inline
  bool operator==( const front_queue<T, Sequence>& y ) {
    return c == y.c;
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns true iff 'this' queue is less than y. Less than is defined by the
  // Sequence class.
  /////////////////////////////////////////////////////////////////////////////
  inline
  bool operator<( const front_queue<T, Sequence>& y) {
    return c < y.c;
  }

}; // end front_queue

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// I would like to replace this with a template parameter at some time, but
// I only want to include the declaration of burst_mode_transition_
// probabilities in this file.  Since currently the export keyword is not 
// supported, I cannot seperate the definition and declaration.  Therefore,
// I'm not going to use the template parameters, but substitute in the 
// typedef until the export keyword is supported.
/////////////////////////////////////////////////////////////////////////////
typedef insert_iterator<matrix_type> OutputIterator;

/////////////////////////////////////////////////////////////////////////////
// This function uses a burst more approximation algorithm to assign 
// transition probabilities to the edges in the state table states.  The 
// rules matrix is necessary as an entry point to the distribution functions
// for the events.  Tolerance is used as a stepsize suggestion for all of
// the numerical integrations.  The actual transition probabilities are 
// assigned as matrix_type::value_types to the iterator out.  The function
// returns true if all went well, otherwise it returns false.  
// 
// The heuristic algorithm is based on synchronization points in the state
// graph between successive concurrent bursts.  When non-burst mode type
// designs are fed into this heuristic, accuracy of the results quickly
// degrades, since the algorithm must guess where synchronization points 
// occur.
//
// NOTE: This only works with TER structures!  Anything TEL, all bets
// are off, you get what you deserve.
//
// NOTE: Change the structure of the breadth first search to mark entries
// as being queued, rather than as being visited.  This will prevent duplicate
// state entries from being pushed on the stack!
/////////////////////////////////////////////////////////////////////////////
bool burst_mode_transition_probabilities( stateADT states[],
					  ruleADT* rules[],
					  const unsigned int& nevents,
					  const CPdf::real_type& tolerance,
					  OutputIterator i_out );

//
///////////////////////////////////////////////////////////////////////////////
#endif // __INTERNAL_BM_TRANS_PROB_H
