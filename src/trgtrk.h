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
#ifndef __INTERNAL_TRGTRK_H
#define __INTERNAL_TRGTRK_H

#include <vector>

#include "markov_types.h"
#include "graph_tracker.h"
#include "trigger_probs.h"

/////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Graph tracker to record trigger probabilities in the ER.
///////////////////////////////////////////////////////////////////////////////
class trigger_tracker : public graph_tracker {

protected:

  trigger_profile m_tp;
  vector<unsigned int> m_ecount;

protected:
  
  virtual prob_vector find_prob() const;

  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns ); 
public:

  trigger_tracker( unsigned int nevents );
  
  ~trigger_tracker();

  /////////////////////////////////////////////////////////////////////////////
  //
  /////////////////////////////////////////////////////////////////////////////
  bool fire( const double& weight,
	     const int& enabled,
	     const int& enabling = 0 );
  
  /////////////////////////////////////////////////////////////////////////////
  //
  /////////////////////////////////////////////////////////////////////////////
  void store_results( stateADT* s ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  /////////////////////////////////////////////////////////////////////////////
  trigger_profile trig_prof() const;
  
};

//
/////////////////////////////////////////////////////////////////////////////

#endif
