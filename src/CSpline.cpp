/**
 * @name Spline Representation   
 * @version 0.1 alpha
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

#include "CSpline.h"

////////////////////////////////////////////////////////////////////////////////
//
ostream& operator<<( ostream& s, const pair<double,double>& p ) {
  s << "(" << p.first << ", " << p.second << ")";
  return( s );
}
//
////////////////////////////////////////////////////////////////////////////////

double CSpline::u( const size_type& i ) const {
  assert( i < m_Pdf.size() - 1 );
  return( m_Pdf[i+1].first - m_Pdf[i].first );
}

void CSpline::makeSpline() {
  array_type d( m_Pdf.size() );
  array_type w( m_Pdf.size() );
  for ( size_type i = 1 ; i < m_Pdf.size()-1 ; i++ )
    d[i] = 2.0 * ( m_Pdf[i+1].first - m_Pdf[i-1].first );
  for ( size_type i = 1 ; i < m_Pdf.size()-1 ; i++ )
    w[i] = 6.0 * ( ( m_Pdf[i+1].second - m_Pdf[i].second ) / u(i) -
		   ( m_Pdf[i].second - m_Pdf[i-1].second ) / u(i-1) );
  m_P = d;
  ::fill( m_P.begin(), m_P.end(), 0 );
  for ( size_type i = 1 ; i < m_Pdf.size()-2 ; i++ ) {
    w[i+1] = w[i+1] - w[i] * u(i) / d[i];
    d[i+1] = d[i+1] - u(i) * u(i) / d[i];
  }
  for ( size_type i = m_Pdf.size()-2 ; i > 0 ; i-- )
    m_P[i] = ( w[i] - u(i) * m_P[i+1] ) / d[i];
}

double CSpline::fx( const double& x ) const {
  return( pow( x, 3 ) - x );
}

double CSpline::f( const double& v ) const {
  if ( v < (*m_Pdf.begin()).first || v > (*(m_Pdf.end()-1)).first ) return( 0 );
  int i = 0;
  while( v > m_Pdf[i+1].first ) i++;
  double t = ( v - m_Pdf[i].first ) / u(i);
  return( t*m_Pdf[i+1].second + 
	  (1-t)*m_Pdf[i].second +
	  u(i)*u(i)*( fx(t)*m_P[i+1] + fx(1-t)*m_P[i] ) / 6.0 );
}

CSpline::CSpline() : m_Pdf(), m_P() {
}

CSpline::CSpline( const point_list& pdf ) : m_Pdf(pdf), m_P() {
  fit();
}

CSpline::CSpline( const CSpline& s ) : m_Pdf(s.m_Pdf), m_P(s.m_P) {
  // No Dynamically Allocated memory
} 

CSpline::~CSpline() {
  // No Dynamically Allocated memory
}

CSpline& CSpline::operator=( const CSpline& s ) {
  if ( this == &s ) return( *this );
  m_Pdf = s.m_Pdf;
  m_P = s.m_P;
  return( *this );
}

void CSpline::push_back( const point_type& p ) {
  m_Pdf.push_back( p );
}

void CSpline::fit() {
  ::sort( m_Pdf.begin(), m_Pdf.end() );
  m_Pdf.erase( ::unique( m_Pdf.begin(), m_Pdf.end() ), m_Pdf.end() );
  makeSpline();
}

void CSpline::plot( const char* name, const int& steps ) const {
  assert( m_P.size() != 0 );
  double w = ( (*(m_Pdf.end()-1)).first - (*m_Pdf.begin()).first ) / (double)steps;
  cout << endl << "hold on;";
  cout << endl << name << "_x = zeros( [1," << steps << "] );";
  cout << endl << name << "_y = zeros( [1," << steps << "] );";
  cout << endl << name << "_i = zeros( [1," << m_Pdf.size() << "] );";
  cout << endl << name << "_j = zeros( [1," << m_Pdf.size() << "] );";
  for ( size_type i = 0 ; i < m_Pdf.size() ; i++ ) {
    cout << endl << endl << name << "_i(" << (i+1) << ") = " << (m_Pdf[i]).first << ";";
    cout << endl << name << "_j(" << (i+1) << ") = " << (m_Pdf[i]).second << ";";
  }
  for ( int i = 1 ; i <= steps+1 ; i++ ) {
    cout << endl << endl << name << "_x(" << (i) << ") = " << ( (*m_Pdf.begin()).first + (i-1)*w ) << ";";
    cout << endl << name << "_y(" << (i) << ") = " << f( (*m_Pdf.begin()).first + (i-1) * w ) << ";";
  }
  cout << endl << "plot( " << name << "_x, " << name << "_y, 'b.-' );";
  cout << endl << "plot( " << name << "_i, " << name << "_j, 'rx' );";
  cout << endl << "grid;";
  cout << endl << "title( 'Spline curve fit' );";
  cout << endl << "ylabel( 'y values' );";
  cout << endl << "xlabel( 'x values' );" << endl;
  cout << endl << "hold off;";
}

CSpline::const_point_list_iter CSpline::begin() const {
  return( m_Pdf.begin() );
}

CSpline::const_point_list_iter CSpline::end() const {
  return( m_Pdf.end() );
}

ostream& operator<< ( ostream& s, const CSpline& c ) {
  for ( CSpline::point_list::const_iterator i = c.m_Pdf.begin() ; i != c.m_Pdf.end() ; i++ ) 
    s << *i << endl;
  return( s );
}

/**
 * Input operator input format:
 *   Number of points
 *   x value y value
 *   .
 *   .
 *   .
 */
istream& operator>>( istream& s, CSpline& c ) {
  int i;
  s >> i;
  while( i-- ) {
    double x;
    double y;
    s >> x;
    s >> y;
    c.push_back( CSpline::point_type( x, y ) );
  }
  return s;
}
