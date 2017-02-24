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
#ifndef __MARKOV_INTERNAL_MATRIX_FUNC_H
#define __MARKOV_INTERNAL_MATRIX_FUNC_H

#include <list>
#include <algorithm>
#include <iostream>
#include <assert.h>

#include "markov_types.h"

struct _matrix_interface {

  _matrix_interface();

  const index_type& row( const matrix_type::iterator& i ) const;
  const index_type& row( const matrix_type::const_iterator& i ) const;

  const index_type& col( const matrix_type::iterator& i ) const;
  const index_type& col( const matrix_type::const_iterator& i ) const;

  prob_type& prob( const matrix_type::iterator& i ) const;
  const prob_type& prob( const matrix_type::const_iterator& i ) const;

  matrix_type::iterator begin( matrix_type& m, const index_type& i );
  matrix_type::const_iterator begin( const matrix_type& m, 
				     const index_type& i ) const;

  matrix_type::iterator end( matrix_type& m, const index_type& i );
  matrix_type::const_iterator end( const matrix_type& m, 
				   const index_type& i ) const;

  matrix_type::const_iterator next_row( const matrix_type& m, 
					matrix_type::const_iterator i ) const;

  matrix_type::iterator next_row( const matrix_type& m, 
				  matrix_type::iterator i );

};

matrix_type::value_type make_value_type( const index_type& r, 
					 const index_type& c,
					 const prob_type& p );

bool project( const index_type& src, 
	      const index_type& dest,
	      matrix_type& m );

matrix_type transpose( const matrix_type& m );

prob_vector operator*( const matrix_type& m, const prob_vector& p );

ostream& operator<<( ostream& s, const matrix_type::value_type& v );

#endif // __MARKOV_INTERNAL_MATRIX_H
