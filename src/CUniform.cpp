/**
 * @name CUniform Class
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

#include "CUniform.h"

CPdf::real_type CUniform::func_def( const real_type& x ) const {
  if ( x >= m_lower && x <= m_upper )
    return( m_level );
  return( 0 );
}

CPdf::real_type CUniform::rand_def() {
  return( (real_type)m_lower + 
  	  (real_type)( m_upper - m_lower ) * m_uni.rand() );
}

/////////////////////////////////////////////////////////////////////////////
// This function will returns the expected value of the distribution 
// calculated as follows:
//             (m_upper - m_lower) / 2
/////////////////////////////////////////////////////////////////////////////
CPdf::real_type CUniform::expected_value_def() const {
  return( m_lower + ( m_upper - m_lower ) / 2 );
}

CUniform::CUniform() 
  : CPdf(),
    m_level( 1 ),
    m_uni() {
  // DO NOTHING
}

CUniform::CUniform( const real_type& l, 
		    const real_type& u,
		    uniform_generator uni ) 
  : CPdf( l, u ),
    m_level( 1.0 / ( u - l ) ),
    m_uni( uni ) {
  assert( u > l );
}
  
CUniform::CUniform( const CUniform& c ) 
: CPdf( c ),
  m_level( c.m_level ),
  m_uni( c.m_uni ) {
  // Do Nothing
}
  
CUniform::~CUniform() {
  // NOTHING TO DO
}
  
void CUniform::serializeOut( ostream& s ) const {
  s << "<" << m_lower << "," << m_upper << "> => ";
  s << "U(" << m_lower << "," << m_upper << ") : ";
  s << m_norm;
}
