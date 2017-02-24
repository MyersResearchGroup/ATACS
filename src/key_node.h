/**
 * @name key_node Class  
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

#ifndef _KEY_NODE_H
#define _KEY_NODE_H

#include <iostream>

using namespace std;

template <class T1, class T2>
struct key_node {

  T1 row;
  T2 col;
  key_node() : row( T1() ), col( T2() ) {}
  key_node( const T1& a, const T2& b ) : row(a), col(b) {}

#ifdef __STL_MEMBER_TEMPLATES
  template <class U1, class U2>
  key_node(const key_node<U1, U2>& p) : row(p.row), col(p.col) {}
#endif

};

template <class T1, class T2>
inline bool operator==(const key_node<T1, T2>& x, const key_node<T1, T2>& y) { 
  return x.row == y.row && x.col == y.col; 
}

template <class T1, class T2>
inline bool operator<(const key_node<T1, T2>& x, const key_node<T1, T2>& y) { 
  return x.row < y.row || ( y.row == x.row && x.col < y.col); 
}

template <class T1, class T2>
inline key_node<T1, T2> make_key_node(const T1& x, const T2& y) {
  return key_node<T1, T2>(x, y);
}

template <class T1, class T2>
ostream& operator<<( ostream& s, const key_node<T1, T2>& k ) {
  s << "(" << k.row << "," << k.col << ")";
  return( s );
}

#endif /* _KEY_NODE_H */
