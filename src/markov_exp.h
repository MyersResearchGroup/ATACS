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
#ifndef __INTERNAL_MARKOV_EXP_H
#define __INTERNAL_MARKOV_EXP_H

#include "struct.h"
#include "markov_types.h"

///////////////////////////////////////////////////////////////////////////////
//

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
				   insert_iterator<matrix_type> i_out );

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
				 insert_iterator<matrix_type> i_out );

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
					 i_out );

//
///////////////////////////////////////////////////////////////////////////////

#endif // __INTERNAL_MARKOV_EXP_H
