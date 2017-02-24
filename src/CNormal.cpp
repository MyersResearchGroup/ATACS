/**
 * @name CNormal Class
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

#include "CNormal.h"

CPdf::real_type CNormal::func_def( const real_type& x ) const {
  if ( x >= m_lower && x <= m_upper )
    return( pow( ( 2 * M_PI * m_var ), -0.5 ) * 
	    exp ( -1 * ( pow( ( x - m_mean ), 2 ) / ( 2 * m_var ) ) ) );
  return( 0 );
}

CPdf::real_type CNormal::rand_def() {
  real_type w = m_uni.rand();
  if ( w == 0.0 ) 
    w =+ 0.0000000000001;
  real_type z = m_uni.rand();
  real_type s = sqrt( -2 * log( w ) );
  real_type t = 4 * z * acos( 0.0 );
  w = s * cos( t ) * m_std;
  return( m_mean + w );
}

/////////////////////////////////////////////////////////////////////////////
// This function will returns the expected value of the distribution which
// is th mean of the distribution.
/////////////////////////////////////////////////////////////////////////////
CPdf::real_type CNormal::expected_value_def() const {
  return( m_mean );
}

CNormal::CNormal()
  : CPdf(),
    m_mean( 0 ),
    m_std( 0 ),
    m_var( 0 ),
    m_uni() {
  // Do Nothing
}

CNormal::CNormal( const real_type& l, 
		  const real_type& u, 
		  const real_type& m, 
		  const real_type& std,
		  uniform_generator uni ) 
  : CPdf( l, u ),
    m_mean( m ),
    m_std( std ),
    m_var( std * std ),
    m_uni( uni ) {
  //normalize();
}
  
CNormal::CNormal( const CNormal& c ) 
  : CPdf( c ),
    m_mean( c.m_mean ),
    m_std( c.m_std ),
    m_var( c.m_var ),
    m_uni( c.m_uni ) {
  // DO NOTHING
}
  
CNormal::~CNormal() {
  // NOTHING TO DO
}

void CNormal::serializeOut( ostream& s ) const {
  s << "<" << m_lower << "," << m_upper << "> => ";
  s << "N(" << m_mean << "," << sqrt( m_var ) << ") : ";
  s << m_norm;
}
