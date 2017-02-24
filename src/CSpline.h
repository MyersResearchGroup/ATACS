/**
 * @name Spline Representation 
 * @version 0.1 beta
 *
 * (c)opyright 1997 by Eric G. Mercer
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

#ifndef _CSPLINE_H_ 
#define _CSPLINE_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
ostream& operator<<( ostream& s, const pair<double,double>& p );
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//

/**
 * Algortihms from pages 548-550 of C++ Algorithms by Sedgewick.
 */
class CSpline {
public:

  typedef pair<double,double>        point_type;
  typedef vector<point_type>         point_list;
  typedef point_list::const_iterator const_point_list_iter;
  typedef vector<double>             array_type;
  typedef array_type::size_type      size_type;

protected:

  point_list m_Pdf;
  array_type m_P;

protected:

  double u( const size_type& i ) const;
  
  void makeSpline();
  
  double fx( const double& x ) const;
  
public:

  CSpline();

  CSpline( const point_list& pdf );
  
  CSpline( const CSpline& s );

  ~CSpline();

  double f( const double& v ) const;

  CSpline& operator=( const CSpline& s );

  void push_back( const point_type& p );

  void fit();
  
  void plot( const char* name, const int& steps ) const;

  const_point_list_iter begin() const;

  const_point_list_iter end() const;

  friend ostream& operator<<( ostream& s, const CSpline& c );
  
  friend istream& operator>>( istream& s, CSpline& c );

};

//
////////////////////////////////////////////////////////////////////////////////

#endif
