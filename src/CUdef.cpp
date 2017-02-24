/**
 * @name CUdef Class
 * @version 0.1 beta
 *
 * (c)opyright 1998 by Eric G. Mercer
 *
 * @author Eric G. Mercer
 *
 * Permission to use, copy, modify and/or distribute, but not sell, this
 * software and its documentation for any purpose is hereby granted
 * without fee, subject to the following terms and conditions:
 *
 * 1. The above copyright notice and this permission notice must
 * appear in all copies of the software and related documentation.
 *
 * 2. The name of University of Utah may not be used in advertising or
 * publicity pertaining to distribution of the software without the
 * specific, prior written permission of Univsersity of Utah.
 *
 * 3. This software may not be called "Taco" if it has been modified
 * in any way, without the specific prior written permission of
 * Eric G. Mercer
 *
 * 4. THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF UTAH OR THE AUTHORS OF THIS
 * SOFTWARE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <functional>
#include <numeric>

#include "CUdef.h"

///////////////////////////////////////////////////////////////////////////////
// Compares the first value in a pair point_type and returns true iff it
// is equal to the coord_type x.
///////////////////////////////////////////////////////////////////////////////
struct find_predicate 
  : binary_function<CUdef::point_type,CPdf::real_type,bool> {
  bool operator()( const CUdef::point_type& pt, CPdf::real_type x ) const {
    return( pt.first == x );
  } 
};

///////////////////////////////////////////////////////////////////////////////
// Returns the value at an arbitrary point x.  If x is the coordinate of a
// point fournd in the point_list m_curve, than the second value is returned
// from the point.  Otherwise the function returns zero.
///////////////////////////////////////////////////////////////////////////////
CPdf::real_type CUdef::func_def( const real_type& x ) const {
  if ( x >= m_lower && x <= m_upper ) {
    typedef point_list::const_iterator const_iterator;
    const_iterator i = ::find_if( m_curve.begin(), m_curve.end(),
				  bind2nd( find_predicate(), x ) );
    if ( i == m_curve.end() )
      return( 0 );
    return( (*i).second );
  }
  return( 0 );
}

///////////////////////////////////////////////////////////////////////////////
// This is not correct.  This just returns 1 always.
///////////////////////////////////////////////////////////////////////////////
CPdf::real_type CUdef::rand_def() {
  return( 1 );
}

/////////////////////////////////////////////////////////////////////////////
// This function will returns the expected value of the distribution
// which is defined as follows:
//               sum for all i p(i)i
// where i is each point that is defined for the distribution and p(i) is
// the probability of that point.
/////////////////////////////////////////////////////////////////////////////
CPdf::real_type CUdef::expected_value_def() const {
  typedef CSpline::const_point_list_iter const_iterator;
  CPdf::real_type expected = 0;
  for ( const_iterator i = m_curve.begin() ; i != m_curve.end() ; i++ ) {
    expected += (*i).first * (*i).second * m_norm;
  }
  return( expected );
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
struct accumulate_helper 
  : binary_function<CPdf::real_type,CUdef::point_type,CPdf::real_type> {
  CPdf::real_type operator()( const CPdf::real_type& a, 
			      const CUdef::point_type& b ) {
    return( a + b.second );
  }
};

///////////////////////////////////////////////////////////////////////////////
// Calculates the normalization constant for the userdefined function
///////////////////////////////////////////////////////////////////////////////
void CUdef::normalize() {
  m_norm = 1 / ::accumulate( m_curve.begin(), m_curve.end(), (double)0.0,
			     accumulate_helper() );
}

CUdef::CUdef()
  : CPdf(),
    m_curve() {
}

CUdef::CUdef( const real_type& l, 
	      const real_type& u,
	      const point_list& s ) 
  : CPdf( l, u ),
    m_curve( s ) {
  ::sort( m_curve.begin(), m_curve.end() );
  normalize();
}
  
CUdef::CUdef( const real_type& l,
	      const real_type& u,
	      const point_list_node* ptr ) 
  : CPdf( l, u ),
    m_curve() {
  while( ptr != NULL ) {
    point_type pt( ptr->x, ptr->y );
    m_curve.push_back( pt );
    ptr = ptr->next;
  }
  ::sort( m_curve.begin(), m_curve.end() );
  normalize();
}
  
CUdef::CUdef( const CUdef& c )
  : CPdf( c ),
    m_curve( c.m_curve ) {
  // DO NOTHING
}
  
CUdef::~CUdef() {
  // NOTHING TO DO
}
  
///////////////////////////////////////////////////////////////////////////////
// Returns true IFF pt.first < x 
///////////////////////////////////////////////////////////////////////////////
struct lower_bound_helper 
  : binary_function<CUdef::point_type,CPdf::real_type,bool> {
  bool operator()( const CUdef::point_type& pt, 
		   const CPdf::real_type& x ) const {
    return( pt.first < x );
  }
};

///////////////////////////////////////////////////////////////////////////////
// returns true IFF x < pt.first 
///////////////////////////////////////////////////////////////////////////////
struct upper_bound_helper 
  : binary_function<CPdf::real_type,CUdef::point_type,bool> {
  bool operator()( const CPdf::real_type& x,
		   const CUdef::point_type& pt ) const {
    return( x < pt.first );
  }
};

/////////////////////////////////////////////////////////////////////////////
// Calculates the sum of the values of f(x) where l <= x <= u. This is the
// discrete case version of an integral. If X is the set of all points defined
// and M:x->y then the integral is the sum of M(x) for x in X.
///////////////////////////////////////////////////////////////////////////////
CPdf::real_type CUdef::integrate( const real_type& l, 
				  const real_type& u, 
				  const real_type& tolerance ) const {
  if ( l < m_lower || u > m_upper ) 
    return( 0 );
  typedef point_list::const_iterator const_iterator;
  const_iterator first = ::lower_bound( m_curve.begin(),
					m_curve.end(),
					l,
					lower_bound_helper() );
  const_iterator last = ::upper_bound( m_curve.begin(),
				       m_curve.end(),
				       u,
				       upper_bound_helper() );
  return( ::accumulate( first, last, double(0.0), accumulate_helper() ) );
}

void CUdef::serializeOut( ostream& s ) const {
  s << "<" << m_lower << "," << m_upper << "> => ";
  s << "D(" << m_lower << "," << m_upper << ") : ";
  s << m_norm;
}
