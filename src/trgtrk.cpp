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
#include "trgtrk.h"

/////////////////////////////////////////////////////////////////////////////
//
prob_vector trigger_tracker::find_prob() const {
  typedef trigger_list::const_iterator tl_iter;
  prob_vector vals;
  for ( unsigned int i = 0 ; i < m_ecount.size() ; i++ ) {
    prob_type total = (prob_type)(m_ecount[i]);
    for ( tl_iter j = (m_tp[i]).begin() ; j != (m_tp[i]).end() ; j++ ) {
      vals.push_back( ((*j).second) / total );
    }
  }
  return( vals );
}


void trigger_tracker::update_counts( const double& weight,
				     const int& ps, 
				     const int& ns ) {

  typedef trigger_list::iterator tl_iter;
  ++m_ecount[ns];
  for ( tl_iter i = (m_tp[ns]).begin() ; i != (m_tp[ns]).end() ; i++ ) {
    if ( (*i).first == (unsigned int)ps ) {
      ((*i).second) += 1;
      return;
    }
  }
  (m_tp[ns]).push_back( trigger_type( ps, 1 ) );
}

/////////////////////////////////////////////////////////////////////////////
// Simple constructor initializes all member vars.
/////////////////////////////////////////////////////////////////////////////
trigger_tracker::trigger_tracker( unsigned int nevents )
  : graph_tracker(), 
    m_tp( nevents, trigger_list() ), 
    m_ecount( nevents, 0 ) {
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool trigger_tracker::fire( const double& weight,
			    const int& enabled,
			    const int& enabling ) {
  update_counts( weight, enabling, enabled );
  return( true );
}
  
/////////////////////////////////////////////////////////////////////////////
// Destructor required by the interface.
/////////////////////////////////////////////////////////////////////////////
trigger_tracker::~trigger_tracker() {
  // Nothing needs to be done here.
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
trigger_profile trigger_tracker::trig_prof() const {
  typedef trigger_list::const_iterator tl_iter;
  trigger_profile tp( m_ecount.size(), trigger_list() );
  for ( unsigned int i = 0 ; i < m_ecount.size() ; i++ ) {
    prob_type total = (prob_type)(m_ecount[i]);
    for ( tl_iter j = (m_tp[i]).begin() ; j != (m_tp[i]).end() ; j++ ) {
      (tp[i]).push_back( trigger_type( (*j).first, ((*j).second) / total ) );
    }
  }
  return( tp );
}
  
void trigger_tracker::store_results( stateADT* s ) const {
  // Nothing to store
}

//
/////////////////////////////////////////////////////////////////////////////
