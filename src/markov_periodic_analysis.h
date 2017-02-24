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
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE  
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////
#ifndef __MARKOV_INTERNAL_PERIODIC_ANALYSIS_H
#define __MARKOV_INTERNAL_PERIODIC_ANALYSIS_H

#include <utility>
#include <list>
#include <set>
#include <vector>
#include <iterator>

#include "markov_types.h"
#include "markov_matrix_func.h"

typedef set<index_type,less<index_type> > preclass_set;

typedef list<preclass_set> preclass_set_list;

typedef list<int> period_list;

///////////////////////////////////////////////////////////////////////////////
// Function to find the index of the preclass for which i belongs to. (i.e.
// the index of the set starting from begin() that contains i )
///////////////////////////////////////////////////////////////////////////////
index_type preclass_index( const preclass_set_list& psl, const index_type& i );

///////////////////////////////////////////////////////////////////////////////
// Function that uses a preclass_set_list and a possibe set of periods to 
// divide a graph represented by a matrix m into its periodic classes.  The 
// result is to return a matrix_vector where each entry in the vector is the
// graph or matrix representing a periodic class of the matrix m.
///////////////////////////////////////////////////////////////////////////////
matrix_vector matrix_segregate( const preclass_set_list& psl, 
				const period_list& pl, 
				const matrix_type& m );

///////////////////////////////////////////////////////////////////////////////
// Function to divide a graph represented by a matrix into periodic classes.  
// Return each periodic class as an entry in a matrix_vector with the size of 
// the vector denoting the periodicity of the matrix.
///////////////////////////////////////////////////////////////////////////////
matrix_vector periodic_analysis( const matrix_type& m );

///////////////////////////////////////////////////////////////////////////////

/**
 * Finds the greatest common diviser between a pair of
 * value_types using Euclid's algorithm.
 */
template<class value_type>
value_type gcd( value_type u, value_type v ) {
  value_type t;
  while( u > 0 ) {
    if ( u < v ) {
      t = u;
      u = v;
      v = t;
    }
    u = u - v;
  }
  return( v );
}

///////////////////////////////////////////////////////////////////////////////

#endif // __MARKOV_INTERNAL_PERIODIC_ANALYSIS_H
