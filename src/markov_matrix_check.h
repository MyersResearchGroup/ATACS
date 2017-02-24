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
#ifndef __INTERNAL_MARKOV_MATRIX_CHECK_H
#define __INTERNAL_MARKOV_MATRIX_CHECK_H

#include "struct.h"
#include "bm_trans_probs.h"
#include "markov_types.h"

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
bool clear_transient_edges( stateADT states[] );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// state_count returns the number of states in the system.
/////////////////////////////////////////////////////////////////////////////
unsigned int state_count( stateADT s[] );

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
// connectivty anlaysis returns a matrix_vector containing the non-transient
// non-absorbing strongly connected components of the state graph represented
// in states.  If a the return type is 0, then the state graph is non-cyclic
// and cannot be analyzed with Markovian techniques.  Otherwise, the matrix
// vector will contain all strongly connected components.
/////////////////////////////////////////////////////////////////////////////
matrix_vector connectivity_analysis( stateADT states[] );

//
/////////////////////////////////////////////////////////////////////////////

#endif
