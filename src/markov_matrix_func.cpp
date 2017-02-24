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

#include "markov_matrix_func.h"

_matrix_interface::_matrix_interface() {
  // Nothing to do
}

const index_type& 
_matrix_interface::row( const matrix_type::iterator& i ) const {
  return( (*i).first.row );
}

const index_type& 
_matrix_interface::row( const matrix_type::const_iterator& i ) const {
  return( (*i).first.row );
}  

const index_type& 
_matrix_interface::col( const matrix_type::iterator& i ) const{
  return( (*i).first.col );
}

const index_type& 
_matrix_interface::col( const matrix_type::const_iterator& i ) const {
  return( (*i).first.col );
}  

prob_type& 
_matrix_interface::prob( const matrix_type::iterator& i ) const{
  return( (*i).second );
}

const prob_type& 
_matrix_interface::prob( const matrix_type::const_iterator& i ) const{
  return( (*i).second );
}

matrix_type::iterator 
_matrix_interface::begin( matrix_type& m, const index_type& i ) {
  matrix_type::iterator j = 
    m.lower_bound( make_key_node( i, (index_type)0 ) );
  if ( j != m.end() ) {
    if ( row( j ) != i )
      return( m.end() );
  }
  return( j );
}

matrix_type::const_iterator 
_matrix_interface::begin( const matrix_type& m, const index_type& i ) const {
  matrix_type::const_iterator j = 
    m.lower_bound( make_key_node( i, (index_type)0 ) );
  if ( j != m.end() ) {
    if ( row( j ) != i )
      return( m.end() );
  }
  return( j );
}

matrix_type::iterator 
_matrix_interface::end( matrix_type& m, const index_type& i ) {
  return( m.lower_bound( make_key_node( i+1, (index_type)0 ) ) );
}

matrix_type::const_iterator 
_matrix_interface::end( const matrix_type& m, const index_type& i ) const {
  return( m.lower_bound( make_key_node( i+1, (index_type)0 ) ) );
}

matrix_type::const_iterator 
_matrix_interface::next_row( const matrix_type& m, 
			     matrix_type::const_iterator i ) const {
  index_type cur = row( i );
  while( ++i != m.end() ) { 
    // If && evaluates left to right and stops at the first failure
    if ( row( i ) != cur ) {  
      // change while loop to while( ++i != end() && row( i ) == cur );
      return( i );
    }
  }
  return( i );
}

matrix_type::iterator 
_matrix_interface::next_row( const matrix_type& m, 
			     matrix_type::iterator i ) {
  index_type cur = row( i );
  while( ++i != m.end() ) { 
    // If && evaluates left to right and stops at the first failure
    if ( row( i ) != cur ) {  
      // change while loop to while( ++i != end() && row( i ) == cur );
      return( i );
    }
  }
  return( i );
}

matrix_type::value_type make_value_type( const index_type& r, 
					 const index_type& c,
					 const prob_type& p ) {
  return( matrix_type::value_type( make_key_node( r, c ), p ) );
}

matrix_type transpose( const matrix_type& m ) {
  matrix_type tmp;
  _matrix_interface mi;
  for ( matrix_type::const_iterator i = m.begin() ; i != m.end() ; i++ )
    if ( mi.prob( i ) ) 
      tmp.insert( make_value_type( mi.col( i ), mi.row( i ), mi.prob( i ) ) );
  return( tmp );
}

bool project( const index_type& src, 
	      const index_type& dest,
	      matrix_type& m ) {
  _matrix_interface mi;
  typedef list <matrix_type::value_type> tlist;
  tlist change_list;
  for ( matrix_type::const_iterator i = m.begin() ; i != m.end() ; i++ )
    if ( (mi.row( i ) == dest && mi.col( i ) == src) || 
	 mi.row( i ) == src || mi.col( i ) == src )
      change_list.push_back( *i );
  assert( m.find( make_key_node( dest, src ) ) != m.end() );
  tlist::const_iterator j = ::find( change_list.begin(), change_list.end(), 
				    (*m.find( make_key_node( dest, src ) ) ) );
  assert( j != change_list.end() );
  for ( tlist::const_iterator i = change_list.begin() ; 
	i != change_list.end() ; i++ ) {
    m.erase( m.find( (*i).first ) );
    if ( (*i).first.col == src && (*i).first.row != dest )
      m.insert( make_value_type( (*i).first.row, dest, (*i).second ) );
    else if ( (*i).first.row == src ) {
      matrix_type::iterator k = m.find( make_key_node( dest, 
						       (*i).first.col ) );
      if ( k == m.end() )
	m.insert( make_value_type( dest, (*i).first.col, 
				   (*j).second * (*i).second ) );
      else
	mi.prob( k ) += (*j).second * (*i).second;
    }
  }
  return( true );
}

prob_vector operator*( const matrix_type& m, const prob_vector& p ) {
  _matrix_interface mi;
  assert( mi.row( --m.end() ) <= p.size() - 1 && m.begin() != m.end() );
  prob_vector tmp( p.size() );
  matrix_type::const_iterator i = m.begin();
  while( i != m.end() ) {
    prob_type sum = 0;
    index_type cur_row = mi.row( i );
    matrix_type::const_iterator j = mi.end( m, cur_row );
    for ( ; i != j ; i++ )
      sum += p[mi.col(i)] * mi.prob(i);
    tmp[cur_row] = sum;
  }
  return( tmp );
}

ostream& operator<<( ostream& s, const matrix_type::value_type& v ) {
  s << v.first << " -> " << v.second;
  return( s );
}
