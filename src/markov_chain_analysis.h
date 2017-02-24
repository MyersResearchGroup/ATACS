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

#ifndef __MARKOV_INTERNAL_CHAIN_ANALYSIS_FUNC_H
#define __MARKOV_INTERNAL_CHAIN_ANALYSIS_FUNC_H

#include <algorithm>
#include <cmath>

#include "markov_matrix_func.h"
#include "markov_periodic_analysis.h"

///////////////////////////////////////////////////////////////////////////////
// convergence_func is used to determine when a convergence test should be
// performed.  The object contains a list of levels and borders.  The levels
// designate the distance between successive convergence tests.  The borders
// are where the levels change.  The border vector is 1 unit smaller in size 
// than the levels vector.  In this way, the levels[0] is set while cycles is
// less than and equal to borders[0].  Simularly, levels[borders.size()] is 
// sets when cycles is greater than borders[borders.size()-1].  Inside the 
// boundries, levels is set according to cycles, where cycles is below the 
// current border.
// Note: The object always starts at 0, and assumes that the cycles will 
// always monotonically increase.
// The default constructor for this class constructs the following function:
//
//             cycles <= 100 distance = 5
//       100 < cycles <= 500 distance = 10
//      500 < cycles <= 1000 distance = 20
//             cycles > 1000 distance = 50
//
///////////////////////////////////////////////////////////////////////////////
class convergence_func {
protected:

  vector<int> m_levels;
  vector<unsigned long> m_borders;
  unsigned int m_index;

public:

  /////////////////////////////////////////////////////////////////////////////
  // Initializes the object to the default function
  /////////////////////////////////////////////////////////////////////////////
  convergence_func();

  /////////////////////////////////////////////////////////////////////////////
  // Constructor to set the m_levels = levels and m_borders = borders.  This
  // also sets the initial distance to be levels[0] and borders simularly.
  /////////////////////////////////////////////////////////////////////////////
  convergence_func( const vector<int>& levels, 
		    const vector<unsigned long>& borders );

  /////////////////////////////////////////////////////////////////////////////
  // Copy Constructor
  /////////////////////////////////////////////////////////////////////////////
  convergence_func( const convergence_func& c );

  /////////////////////////////////////////////////////////////////////////////
  // Constructor: Does not need to do anything.
  /////////////////////////////////////////////////////////////////////////////
  ~convergence_func();

  /////////////////////////////////////////////////////////////////////////////
  // Assigment operator
  /////////////////////////////////////////////////////////////////////////////
  convergence_func& operator=( const convergence_func& f );

  /////////////////////////////////////////////////////////////////////////////
  // bool operator() ( const unsigned long& cycles, const int& distance ) is a 
  // function that looks at the cycles and last time that the convergence test
  // was run (distance) and returns true if it is time to run an actual 
  // convergence test again.  This is done by looking at the current value of 
  // m_levels[m_index].  If the cycles has reached the border in 
  // m_borders[m_index], the m_index is incremented setting the new level and 
  // border.  If the m_index is at the end of m_borders, it is not incremented
  // and only the distance is checked.
  /////////////////////////////////////////////////////////////////////////////
  bool operator()( const unsigned long& cycles, const int& distance );

  /////////////////////////////////////////////////////////////////////////////
  // void clear() resets the m_index back to 0 to start the function over 
  // again.
  /////////////////////////////////////////////////////////////////////////////
  void clear();

};

///////////////////////////////////////////////////////////////////////////////
// analysis_control is an object used in markov chain analysis and returns true
// once a chain has converged to a specified tolerance.  The object is 
// constructed with a convergence_func object, a maximum number of iterations 
// value, and a tolerance metric.  Object is used with repeated calls to its
// operator().
///////////////////////////////////////////////////////////////////////////////
class analysis_control {
protected:
  
  double _MIN_VALUE_;        // Static value set to 10e-25
  
  unsigned long _MAX_ITERATION_;
  double m_tolerance;

  convergence_func m_conv;
  unsigned long m_iteration;
  unsigned long m_distance;
  prob_vector m_probs;                    // Value of vector from previous run

protected:

  /////////////////////////////////////////////////////////////////////////////
  // bool convergence(const prob_vector& p ) is called to check if p has 
  // converged to a steady state. The convergence test used is as follows:
  //
  //          | v[i]^k - v[i]^(k-m)|
  //      max | ------------------ | < Epsilon
  //          |       v[i]^k       |
  //
  // Returns true if the series has converged.
  /////////////////////////////////////////////////////////////////////////////
  bool convergence( const prob_vector& p );

public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor that sets the maximum iteration count max_iter, the tolerance,
  // tol, and the convergence func f.  NOTE: This doesn't set the initial prob_
  // vector because it may not be known or available at this time.
  /////////////////////////////////////////////////////////////////////////////
  analysis_control( const unsigned long max_iter, 
		    const double& tol,
		    const convergence_func& f );

  /////////////////////////////////////////////////////////////////////////////
  // Copy Constructor
  /////////////////////////////////////////////////////////////////////////////
  analysis_control( const analysis_control& c );
  
  /////////////////////////////////////////////////////////////////////////////
  // Destructor: Doens't need to do anything
  /////////////////////////////////////////////////////////////////////////////
  virtual ~analysis_control();

  /////////////////////////////////////////////////////////////////////////////
  // Assignment operator
  /////////////////////////////////////////////////////////////////////////////
  analysis_control& operator=( const analysis_control& a );

  /////////////////////////////////////////////////////////////////////////////
  // Sets the value of the prob_vector to p.  This is how the analysis 
  // control is seeded for the first time.  Note: This could be replaced by a 
  // function that checks to see if the prob_vectoras any size and if it 
  // doesn't, initialize it to be zero.
  // NOTE: This is legacy code that needs to be phased out!!  This does not
  // need to be here, but I don't have time to remove it.  
  /////////////////////////////////////////////////////////////////////////////
  void prob( const prob_vector& p );

  /////////////////////////////////////////////////////////////////////////////
  // bool operator()(const prob_vector& p ) is called to see if its time to
  // run a convergence check.  If it is time, it resets the distance counter
  // and returns the value of convergence( p );
  /////////////////////////////////////////////////////////////////////////////
  bool operator()( const prob_vector& p );

  /////////////////////////////////////////////////////////////////////////////
  // Resets the analysis control for another run.
  /////////////////////////////////////////////////////////////////////////////
  void clear( const prob_vector& p );

};

///////////////////////////////////////////////////////////////////////////////
// mutex is a structure which has a single operator() defined that takes two
// objects of type value_type.  If value_type a is zero and b is non-zero then
// b is returned, otherwise return a.  
// NOTE: used by the reduced power method to fill in the steady state vector.
///////////////////////////////////////////////////////////////////////////////
template<class value_type>
struct _mutex {
  _mutex() {}     // Void contructor to make passing it as an param easy
  value_type operator()( const value_type& a, const value_type& b ) const {
    if ( !a && b ) return( b );
    return( a );
  } 
};

///////////////////////////////////////////////////////////////////////////////
// Uses the reduced power method to calclulate the steady state of an 
// ireducible markov chain.   The algorithm does a periodic analysis of m to
// divide it into its periodic classes.  It then initializes a vector v to 
// have the value of 1 / (period(m) * preclass[1].size()), or 1 divided by the
// number of states in the first preclass multiplied by the number of periodic
// classes. The function returns the steady state vector for m.
// NOTE: See page 364 of "Introduction to the Numerical Solutions of Markov 
// Chains" By William J. Stewart ISBN: 0-691-03699-3 for a complete explanation
// of the algorithm.
///////////////////////////////////////////////////////////////////////////////
prob_vector reduced_power( const matrix_type& m, 
			   analysis_control done );

#endif // __MARKOV_INTERNAL_CHAIN_ANALYSIS_FUNC_H
