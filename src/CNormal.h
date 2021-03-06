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

#ifndef _CNORMAL_H_ 
#define _CNORMAL_H_

#include <cmath>
#include "CPdf.h"

class CNormal : public CPdf {
protected:

  real_type m_mean;
  real_type m_std;
  real_type m_var;

  uniform_generator m_uni;

protected:

  virtual real_type func_def( const real_type& x ) const;
  
  virtual real_type rand_def();

  /////////////////////////////////////////////////////////////////////////////
  // This function will returns the expected value of the distribution which
  // is the mean of the distribution.
  /////////////////////////////////////////////////////////////////////////////
  virtual real_type expected_value_def() const;

public:
  
  CNormal();

  CNormal( const real_type& l, 
	   const real_type& u, 
	   const real_type& m, 
	   const real_type& std,
	   uniform_generator uni = uniform_generator() );
  
  CNormal( const CNormal& c );
  
  virtual ~CNormal();
  
  virtual void serializeOut( ostream& s ) const;
  
};

#endif
