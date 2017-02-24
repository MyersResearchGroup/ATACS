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

#ifndef _CUDEF_H_ 
#define _CUDEF_H_

#include "CPdf.h"
#include "CSpline.h"
#include "struct.h"

class CUdef : public CPdf {
public:

  typedef CSpline::point_type point_type;
  typedef CSpline::point_list point_list;

protected:

  point_list m_curve;

protected:
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns the value at an arbitrary point x.  If x is the coordinate of a
  // point fournd in the point_list m_curve, than the second value is returned
  // from the point.  Otherwise the function returns zero.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type func_def( const real_type& x ) const;

  /////////////////////////////////////////////////////////////////////////////
  // This is not correct.  Returns 1 always.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type rand_def();

  /////////////////////////////////////////////////////////////////////////////
  // This function will returns the expected value of the distribution
  // which is defined as follows:
  //               sum for all i p(i)i
  // where i is each point that is defined for the distribution and p(i) is
  // the probability of that point.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type expected_value_def() const;

  /////////////////////////////////////////////////////////////////////////////
  // Normalizes the sum of the heights each point in m_curve to equal 1
  /////////////////////////////////////////////////////////////////////////////
  virtual void normalize();

public:
  
  CUdef();

  CUdef( const real_type& l, 
	 const real_type& u,
	 const point_list& s );
  
  CUdef( const real_type& l,
	 const real_type& u,
	 const point_list_node* ptr );

  CUdef( const CUdef& c );
  
  virtual ~CUdef();

  /////////////////////////////////////////////////////////////////////////////
  // Calculates the sum of the values of f(x) where l <= x <= u. This is the
  // discrete case version of an integral. If X is the set of all points 
  // defined and M:x->y then the integral is the sum of M(x) for x in X.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type integrate( const real_type& l, 
			       const real_type& u, 
			       const real_type& tolerance = 0 ) const;
  
  virtual void serializeOut( ostream& s ) const;
  
};

#endif
