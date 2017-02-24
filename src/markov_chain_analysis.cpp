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

#include "markov_chain_analysis.h"
#include "signal_handler.h"

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Initializes the object to the default function
///////////////////////////////////////////////////////////////////////////////
convergence_func::convergence_func() : m_levels(4), m_borders(3), m_index(0) {
  m_levels[0] = 5;
  m_levels[1] = 10;
  m_levels[2] = 20;
  m_levels[3] = 50;
  m_borders[0] = 100;
  m_borders[1] = 500;
  m_borders[2] = 1000;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor to set the m_levels = levels and m_borders = borders.  This
// also sets the initial distance to be levels[0] and borders simularly.
///////////////////////////////////////////////////////////////////////////////
convergence_func::convergence_func( const vector<int>& levels, 
				    const vector<unsigned long>& borders ) 
  : m_levels( levels ), 
    m_borders( borders ),
    m_index(0) {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// Copy Constructor
///////////////////////////////////////////////////////////////////////////////
convergence_func::convergence_func( const convergence_func& c )
  : m_levels( c.m_levels ), 
    m_borders( c.m_borders ),
    m_index( c.m_index ) {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// Constructor: Does not need to do anything.
///////////////////////////////////////////////////////////////////////////////
convergence_func::~convergence_func() {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// Assigment operator
///////////////////////////////////////////////////////////////////////////////
convergence_func& convergence_func::operator=( const convergence_func& f ) {
  if ( &f == this ) return( *this );
  m_levels = f.m_levels;
  m_borders = f.m_borders;
  m_index = f.m_index;
  return( *this );
}

///////////////////////////////////////////////////////////////////////////////
// bool operator() ( const unsigned long& cycles, const int& distance ) is a 
// function that looks at the cycles and last time that the convergence test
// was run (distance) and returns true if it is time to run an actual 
// convergence test again.  This is done by looking at the current value of 
// m_levels[m_index].  If the cycles has reached the border in 
// m_borders[m_index], the m_index is incremented setting the new level and 
// border.  If the m_index is at the end of m_borders, it is not incremented 
// and only the distance is checked.
///////////////////////////////////////////////////////////////////////////////
bool convergence_func::operator()( const unsigned long& cycles, 
				   const int& distance ) {
  if ( m_index == m_borders.size() ) return( distance == m_levels[m_index] );
  bool tmp = ( distance == m_levels[m_index] ) ? true : false;
  if ( cycles >= m_borders[m_index] )
    ++m_index;
  return( tmp );
}

///////////////////////////////////////////////////////////////////////////////
// void clear() resets the m_index back to 0 to start the function over again.
///////////////////////////////////////////////////////////////////////////////
void convergence_func::clear() {
  m_index = 0;
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

// Moved this to be a non-static class member!
// Initial the static memory for the min value.
//const double analysis_control::_MIN_VALUE_ = 10e-25;

///////////////////////////////////////////////////////////////////////////////
// bool convergence(const prob_vector& p ) is called to check if p has 
// converged to a steady state. The convergence test used is as follows:
//
//          | v[i]^k - v[i]^(k-m)|
//      max | ------------------ | < Epsilon
//          |       v[i]^k       |
//
// Returns true if the series has converged.
///////////////////////////////////////////////////////////////////////////////
bool analysis_control::convergence( const prob_vector& p ) {
  if ( !m_probs.size() ) {    // Make sure that the m_probs exists
    m_probs = p;              // If it doesn't exist, set it and return
    // Although gludgy, I need to be able to adapt the sensitivity of the
    // _MIN_VALUE_.  If something isn't at least above _MIN_VALUE_ its 
    // as good as zero and should be treated as thus.  Other ideas to try
    // are 1.0 / (p.size()*p.size()),...
    _MIN_VALUE_ = ( 1.0 / (double)p.size() ) * m_tolerance;
    return( false );
  }
  double max = 0;
  for( unsigned int i = 0 ; i < p.size() ; i++ )
    if ( p[i] != 0 && p[i] > _MIN_VALUE_ ) {
      double tmp = fabs( ( p[i] - m_probs[i] ) / p[i] );
      if ( tmp > max )
	max = tmp;
    }
  m_probs = p;
  return( max < m_tolerance );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor that sets the maximum iteration count max_iter, the tolerance,
// tol, and the convergence func f.  NOTE: This doesn't set the initial prob_
// vector because it may not be known or available at this time.
///////////////////////////////////////////////////////////////////////////////
analysis_control::analysis_control( const unsigned long max_iter, 
				    const double& tol,
				    const convergence_func& f ) 
  : _MIN_VALUE_( 10e-25 ), 
    _MAX_ITERATION_( max_iter ),
    m_tolerance( tol ),
    m_conv( f ),
    m_iteration( 0 ),
    m_distance( 0 ),
    m_probs() {
  // Nothing to do 
}
  
///////////////////////////////////////////////////////////////////////////////
// Copy Constructor
///////////////////////////////////////////////////////////////////////////////
analysis_control::analysis_control( const analysis_control& c )
  : _MIN_VALUE_( 10e-25 ), 
    _MAX_ITERATION_( c._MAX_ITERATION_ ), 
    m_tolerance( c.m_tolerance ),
    m_conv( c.m_conv ),
    m_iteration( c.m_iteration ),
    m_distance( c.m_distance ),
    m_probs( c.m_probs ) {
  // Nothing to do 
} 

///////////////////////////////////////////////////////////////////////////////
// Destructor: Doens't need to do anything
///////////////////////////////////////////////////////////////////////////////
analysis_control::~analysis_control() {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// Assignment operator
///////////////////////////////////////////////////////////////////////////////
analysis_control& analysis_control::operator=( const analysis_control& a ) {
  if ( &a == this ) return( *this );
  _MAX_ITERATION_ = a._MAX_ITERATION_;
  m_tolerance = a.m_tolerance;
  m_conv = a.m_conv;
  m_iteration = a.m_iteration;
  m_distance = a.m_distance;
  m_probs = a.m_probs;
  return( *this );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the value of the prob_vector to p.  This is how the analysis control is
// seeded for the first time.  Note: This could be replaced by a function that
// checks to see if the prob_vectoras any size and if it doesn't, initialize
// it to be zero.
// NOTE: This is legacy code that needs to be phased out!!  This does not
// need to be here, but I don't have time to remove it.
///////////////////////////////////////////////////////////////////////////////
void analysis_control::prob( const prob_vector& p ) {
  m_probs = p;
  // Although gludgy, I need to be able to adapt the sensitivity of the
  // _MIN_VALUE_.  If something isn't at least above _MIN_VALUE_ its 
  // as good as zero and should be treated as thus.  Other ideas to try
  // are 1.0 / (p.size()*p.size()),...
  _MIN_VALUE_ = ( 1.0 / (double)p.size() ) * m_tolerance;
}

///////////////////////////////////////////////////////////////////////////////
// bool operator()(const prob_vector& p ) is called to see if its time to
// run a convergence check.  If it is time, it resets the distance counter
// and returns the value of convergence( p );
///////////////////////////////////////////////////////////////////////////////
bool analysis_control::operator()( const prob_vector& p ) {
  if ( !(m_conv( ++m_iteration, ++m_distance )) ) return( false );

  if ( m_iteration >= _MAX_ITERATION_ ) {
    cout << " -- reached maximum iteration count of " 
	 << _MAX_ITERATION_  << " -- ";
    return( true );
  }

  m_distance = 0;
  return( convergence( p ) );
}

///////////////////////////////////////////////////////////////////////////////
// Resets the analysis control for another run.
///////////////////////////////////////////////////////////////////////////////
void analysis_control::clear( const prob_vector& p ) {
  m_iteration = 0;
  m_distance = 0;
  m_probs = p;
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Uses the reduced power method to calclulate the steady state of an 
// ireducible markov chain.   The algorithm does a periodic analysis of m to
// divide it into its periodic classes.  It then initializes a vector v to 
// have the value of 1 / (period(m) * preclass[1].size()), or 1 divided by 
// the number of states in the first preclass multiplied by the number of 
// periodic classes.  The function returns the steady state vector for m.
// NOTE: See page 364 of "Introduction to the Numerical Solutions of Markov 
// Chains" By William J. Stewart ISBN: 0-691-03699-3 for a complete explanation
// of the algorithm.
///////////////////////////////////////////////////////////////////////////////
prob_vector reduced_power( const matrix_type& m, 
			   analysis_control done ) {
#ifndef __RELEASE
  assert( m.size() );
#endif
  // The transpose is required to simplify the matrix vector
  // multiplication operator!
  matrix_vector pm = periodic_analysis( transpose( m ) ); // Find the periodic
  // This outputs the period of the design along with its periodic
  // classes.
  //  cout << endl << endl << "Period -> " << pm.size() << endl;
//    for ( matrix_vector::const_iterator i = pm.begin();
//   	i != pm.end() ;
//   	i++ ) {
//      ::copy( (*i).begin(), (*i).end(), 
//   	    ostream_iterator<matrix_type::value_type>(cout,"\n") );
//       cout << endl << flush;
//    }
  matrix_vector::const_iterator start( pm.end() - 1 );    // matrix
  matrix_vector::const_iterator head = pm.begin();        // Set the begin 
  matrix_vector::const_iterator i = head;                 // and end points
  _matrix_interface mi;                                   // Create interface
  double init = 1.0 / ( (double)pm.size() * (double)(*head).size());
  prob_vector v( mi.row( --m.end() ) + 1 );            // Initialize the vector
  for ( matrix_type::const_iterator j = (*head).begin() ; 
	j != (*head).end() ; j++ )
    v[mi.col( j )] += init;                // Each state needs equal prob
  done.prob( v );                          // Set the analysis ctrl
  //if ( sigsetjmp( gCtrl_C_mngr.new_entry(), 1 ) == 0 ) {
    do{
      v = (*i) * v;                        // Complete one interation of mult
      for ( i = start ; i != head ; i-- )
	v = (*i) * v;
    }while( !done( v ) );                  // Loop until convergence
    //}
    //else 
    //  cout << endl << "interupted ... ";
  v = (*i) * v;                            // Start filling in the values
  prob_vector x( v );                      // Make a copy of v.
  for( i = start ; i != head ; i-- ) {
    v = (*i) * v;                          // Find new values and copy into x
    ::transform( x.begin(), x.end(), v.begin(), x.begin(), 
		 _mutex<prob_type>() );
  }
  gCtrl_C_mngr.pop();
  return( x );
}

//
///////////////////////////////////////////////////////////////////////////////
