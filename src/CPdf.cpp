/**
 * @name CPdf Class
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
#include <sys/time.h>
#include <unistd.h>
#include "CPdf.h"

#define MAX_RAND 2147483647

/////////////////////////////////////////////////////////////////////////////
// 

unsigned int random_seed() {
  struct timeval tv;
  gettimeofday( &tv, NULL );
  return( tv.tv_usec );
}

//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
uniform_generator::uniform_generator() : m_seed( ::rand() ) {
  // Do Nothing
}

uniform_generator::uniform_generator( unsigned int seed ) : m_seed( seed ) {
  // Do Nothing
}

uniform_generator::uniform_generator( const uniform_generator& g ) 
  : m_seed( g.m_seed ) {
  // Do Nothing
}

uniform_generator& uniform_generator::operator=( const uniform_generator& g ) {
  if ( this == &g ) return( *this );
  m_seed = g.m_seed;
  return( *this );
}

double uniform_generator::rand() {
  //::srand( m_seed );
  m_seed = ::rand();
  return( (double)m_seed / ((double)MAX_RAND + 1.0 ) );
}
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

const int CPdf::_UNDER_ = 5;
const int CPdf::_OVER_ = 10;

CPdf::real_type CPdf::adapt( const real_type& a, 
			     const real_type& b, 
			     const real_type& tolerance ) const {
  real_type x = simpson( a, b, CPdf::_OVER_ );
  if ( fabs( x = simpson( a, b, CPdf::_UNDER_ ) ) > tolerance )
    return( adapt( a, (a+b)/2, tolerance ) + adapt( (a+b)/2, b, tolerance ) );
  return( x );
}

///////////////////////////////////////////////////////////////////////////////
// simpson_step returns the area under the function at the point i*w from a 
// using the simpson method.  Simpson's method combines trapazoidal and 
// rectangular approximations of estimating area to obtain an area value that
// is more accurate to the actual function. 
// NOTE: Accuracy is dependent on w.  The smaller the w, the more accurate
// the area estimation is between [a+(i-1)*w,a+i*w].
///////////////////////////////////////////////////////////////////////////////
CPdf::real_type CPdf::simpson_step( const int& i,
				    const real_type& a,
				    const real_type& w ) const {
  return( w * ( func_def( a + (i-1) * w ) +
		4 * func_def( a - w/2 + i*w ) +
		func_def( a + i*w ) ) / 6 );
}

CPdf::real_type CPdf::simpson( const real_type& a, 
			       const real_type& b, 
			       const int& steps ) const {
  real_type s = 0;
  real_type w = ( b - a ) / (real_type)steps;
  for ( int i = 1 ; i <= steps ; i++ ) {
    s += simpson_step( i, a, w );
    //w * ( func_def( a + (i-1) * w ) +
    //       4 * func_def( a - w/2 + i*w ) +
    //       func_def( a + i*w ) ) / 6;
  }
  return( s );
}

void CPdf::normalize() {
  assert( m_lower < m_upper );
  m_norm = 1.0 / adapt( m_lower, m_upper, _PRECISION_ );
}

CPdf::CPdf()
  : m_lower( 0 ),
    m_upper( 0 ),
    m_norm( 1 ),
    m_upsilon( 1 ),
    m_last_value( 0 ) {
}

CPdf::CPdf( const real_type& lower,
	    const real_type& upper,
	    const real_type& upsilon ) 
  : m_lower( lower ),
    m_upper( upper ),
    m_norm( 1 ),
    m_upsilon( upsilon ),
    m_last_value( 0 ) {
  // Do Nothing
}

CPdf::CPdf( const CPdf& c ) 
  : m_lower( c.m_lower ),
    m_upper( c.m_upper ),
    m_norm( c.m_norm ),
    m_upsilon( c.m_upsilon ),
    m_last_value( c.m_last_value ) {
  // Do Nothing
}

CPdf::~CPdf() {
  // Nothing to do here
}

CPdf::real_type CPdf::f( const real_type& x ) const {
  return( func_def( x ) * m_norm );
}  

CPdf::real_type CPdf::integrate( const real_type& a, 
				 const real_type& b, 
				 const real_type& tolerance ) const {  
  if ( b < m_lower || a > m_upper ) 
    return( 0 );
  real_type i = ( a < m_lower ) ? m_lower : a;
  real_type j = ( b > m_upper ) ? m_upper : b;
  return( adapt( i, j, tolerance ) * m_norm );
}

CPdf::real_type CPdf::rand() {
  m_last_value = rand_def();
  return( m_last_value );
}

/////////////////////////////////////////////////////////////////////////////
// This returns the approximate area under the function on the interval of
// [base+(index-1)*width,base+width*index], by employing simsons rule.  See
// simson_step for further explanation.  
// NOTE: This is virtual for child classes which do not use simpson methods.
/////////////////////////////////////////////////////////////////////////////
CPdf::real_type CPdf::area( const real_type& index,
			    const real_type& base,
			    const real_type& width ) const {
  return( simpson_step( (int)index, base, width ) );
}

/////////////////////////////////////////////////////////////////////////////
// Return the expected value of the distribution.
/////////////////////////////////////////////////////////////////////////////
CPdf::real_type CPdf::expected_value() const {
  return( expected_value_def() );
}

//
///////////////////////////////////////////////////////////////////////////////
