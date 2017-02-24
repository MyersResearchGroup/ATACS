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
#ifndef __INTERNAL_LP_TP_TRACKER_H
#define __INTERNAL_LP_TP_TRACKER_H

#include "markov_types.h"
#include "graph_tracker.h"

/////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// transition_tracker when used in a monte-carlo simulation of a TEL will 
// calculate transition probabilities in a stateADT*.  The edges in the 
// stateADT* can be rules or events.  The results are stored in the
// state_listADT which is a list of edges leaving each stateADT.
///////////////////////////////////////////////////////////////////////////////
class long_term_transition_tracker : public graph_tracker {

  matrix_type  m_trans;
  unsigned int m_iterations;
  
protected:

  virtual prob_vector find_prob() const;

  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns ); 
public:

  long_term_transition_tracker( stateADT* s, const fire_type f = event );
  
  ~long_term_transition_tracker();
  
  virtual void store_results( stateADT* s ) const;

};

//
/////////////////////////////////////////////////////////////////////////////

#endif
