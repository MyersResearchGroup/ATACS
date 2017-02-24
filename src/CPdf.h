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

#ifndef _CPDF_H_ 
#define _CPDF_H_

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <cassert>

#define _PRECISION_ 0.001

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// 

unsigned int random_seed();

//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

class uniform_generator {
protected:
  
  unsigned int m_seed;
  
public:
  
  uniform_generator();
  
  uniform_generator( unsigned int seed );

  uniform_generator( const uniform_generator& g );
  
  uniform_generator& operator=( const uniform_generator& g );

  double rand();  
  
};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

class CPdf {

public:
  
  typedef double real_type;

  static const int _UNDER_;
  static const int _OVER_;

protected:

  real_type m_lower;
  real_type m_upper;
  real_type m_norm;

  // Used for conflict resolution.  Assigns a weight to this distribution
  real_type m_upsilon;
  //uniform_generator m_uni;

  real_type m_last_value;

protected:

  real_type adapt( const real_type& a, 
		   const real_type& b, 
		   const real_type& tolerance ) const;

  /////////////////////////////////////////////////////////////////////////////
  // simpson_step returns the area under the function at the point i*w from a 
  // using the simpson method.  Simpson's method combines trapazoidal and 
  // rectangular approximations of estimating area to obtain an area value that
  // is more accurate to the actual function. 
  // NOTE: Accuracy is dependent on w.  The smaller the w, the more accurate
  // the area estimation is between [a+(i-1)*w,a+i*w].
  /////////////////////////////////////////////////////////////////////////////
  real_type simpson_step( const int& i,
			  const real_type& a,
			  const real_type& w ) const;

  real_type simpson( const real_type& a, 
		     const real_type& b, 
		     const int& steps ) const;

  virtual real_type func_def( const real_type& x ) const = 0;

  virtual real_type rand_def() = 0;

  /////////////////////////////////////////////////////////////////////////////
  // This function will returns the expected value of the distribution and
  // must be defined by implementing classes.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type expected_value_def() const = 0;

  virtual void  normalize();

public:

  CPdf();

  CPdf( const real_type& lower,
	const real_type& upper,
	const real_type& upsilon = 1 );

  CPdf( const CPdf& c );

  virtual ~CPdf();

  real_type f( const real_type& x ) const;
  
  virtual real_type integrate( const real_type& l, 
			       const real_type& u, 
			       const real_type& tolerance = 0 ) const;

  virtual void serializeOut( ostream& s ) const = 0;

  real_type lower() const { return( m_lower ); }
  
  real_type upper() const { return( m_upper ); }

  real_type rand();

  real_type last_value() const { return( m_last_value ); }

  real_type upsilon() const { return( m_upsilon ); }

  /////////////////////////////////////////////////////////////////////////////
  // This returns the approximate area under the function on the interval of
  // [base+(index-1)*width,base+width*index], by employing simsons rule.  See
  // simson_step for further explanation.  
  // NOTE: This is virtual for child classes which do not use simpson methods.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type area( const real_type& index, 
			  const real_type& base,
			  const real_type& width ) const;
			  
  /////////////////////////////////////////////////////////////////////////////
  // Return the expected value of the distribution.
  /////////////////////////////////////////////////////////////////////////////
  real_type expected_value() const;

  inline friend ostream& operator<<( ostream& s, const CPdf& c ){
    c.serializeOut( s );
    return( s );
  }

};

//
///////////////////////////////////////////////////////////////////////////////

#endif
