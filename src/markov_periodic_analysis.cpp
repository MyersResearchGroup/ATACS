///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1997 by Eric G. Mercer
//
// @author Eric G. Mercer
//                    
// Permission to use, copy, modify and/or distribute, but not sell, this  
// software and its documentation for any purpose is hereby granted       
// without fee, subject to the following terms and conditions:            
//                                                                          
// 1. The above copyright notice and this permission notice must         
// appear in all copies of the software and related documentation.        
//                                                                          
// 2. The name of University of Utah may not be used in advertising or  
// publicity pertaining to distribution of the software without the       
// specific, prior written permission of Univsersity of Utah.
//                                                                          
// 3. This software may not be called "Taco" if it has been modified    
// in any way, without the specific prior written permission of           
// Eric G. Mercer                                
//                                                                          
// 4. THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY       
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.       
//                                                                          
// IN NO EVENT SHALL THE UNIVERSITY OF UTAH OR THE AUTHORS OF THIS 
// SOFTWARE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL 
// DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
// DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE  
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////

#include "markov_periodic_analysis.h"

///////////////////////////////////////////////////////////////////////////////
// Function to find the index of the preclass for which i belongs to. (i.e.
// the index of the set starting from begin() that contains i )
///////////////////////////////////////////////////////////////////////////////
index_type preclass_index( const preclass_set_list& psl, const index_type& i ){
  int j = 0;
  for( preclass_set_list::const_iterator k = psl.begin() ; k != psl.end() ; k++, j++ )
    if ( (*k).find( i ) != (*k).end() ) return( j );
  return( --j );
}

///////////////////////////////////////////////////////////////////////////////
// Function that uses a preclass_set_list and a possibe set of periods to 
// divide a graph represented by a matrix m into its periodic classes.  The 
// result is to return a matrix_vector where each entry in the vector is the
// graph or matrix representing a periodic class of the matrix m.
///////////////////////////////////////////////////////////////////////////////
matrix_vector matrix_segregate( const preclass_set_list& psl, 
				const period_list& pl, 
				const matrix_type& m ) {

  // This is a hack to prevent the system from core dumping.  If a seriously
  // non-periodic reducible chain is found!  This is a hack, not a fix.
  if ( !pl.size() ) {
    cout << "Warning - There is a serious problem with the state graph ";
    cout << "causing markov_periodic_analysis::matrix_segregate ";
    cout << "to recieve an empty cycle period vector from periodic ";
    cout << "analysis." << endl;
    matrix_vector mv( 1 );
    mv[0] = m;
    return( mv );
  }

  int period = (*pl.begin());             // Seed the period
  if ( pl.size() > 1 )                    // If multiple periods are found
    for ( period_list::const_iterator i = (++pl.begin()) ; 
	  i != pl.end() ; i++ )
      period = gcd( period, *i );    // actual period is the gcd of all periods
  matrix_vector mv( period );             // One matrix for each periodic class
  for ( int i = 0 ; i < period ; i++ ) {  // Pull out each class
    matrix_type tmp;                      // psl is a list of preclasses
    preclass_set_list::const_iterator ci = psl.begin();
    advance( ci, i );          // Move ci to the first preclass for this class
    for ( unsigned int j = i ; j < psl.size() ; 
	  j += period, advance(ci,period) ) {
      _matrix_interface finder;// Copy elements from preclass into matrix
      for ( preclass_set::const_iterator k = (*ci).begin() ; 
	    k != (*ci).end() ; k++ )
 	for ( matrix_type::const_iterator l = finder.begin( m, *k ) ; 
	      l != finder.end( m, *k ) ; l++ )
 	  tmp.insert( *l );
    }
    mv[i] = tmp;              // Stick the periodic class in the vector
  }
  return( mv );
}

///////////////////////////////////////////////////////////////////////////////
// Function to divide a graph represented by a matrix into periodic classes.  
// Return each periodic class as an entry in a matrix_vector with the size of 
// the vector denoting the periodicity of the matrix.
///////////////////////////////////////////////////////////////////////////////
matrix_vector periodic_analysis( const matrix_type& m ) {
  preclass_set_list psl;                
  period_list pl;
  int preClass = 0;                      
  psl.push_back( preclass_set() );         // Initialize the first preClass
  _matrix_interface mi;
  (*psl.begin()).insert( mi.row( m.begin() ) );
  preclass_set_list::const_iterator psl_iter = psl.begin();
  while( true ) {
    preclass_set tmp;                      // Allocate a new preclass set
    for( preclass_set::const_iterator i = (*psl_iter).begin() ; 
	 i != (*psl_iter).end() ; i++ ) {
      for( matrix_type::const_iterator j = mi.begin( m, *i ) ; 
	   j != mi.end( m, *i ) ; j++ ) {
	// If state in earlier preclass
	int k = preclass_index( psl, mi.col( j ) ); 
	if ( k < preClass ) {
	  // Add a new period into the period list
	  pl.push_back( preClass - k + 1 );
	}
	else {
	  // Otherwise add the state to the preclass set
	  tmp.insert( mi.col( j ) ); 
	}
      }
    }
    // If the preclass set is empty, visited all states
    if ( tmp.empty() ) {   
      // Divide matrixes into periodic groups
      return( matrix_segregate( psl, pl, m ) ); 
    }
    psl.push_back( tmp ); // Add the new preclass to the preclass set
    ++preClass;           // Increment the preclass and preclass set iterator
    ++psl_iter;
  }
}
