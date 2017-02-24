///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 beta
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
#ifndef __RG_INTERNAL_TRANS_PROB_H
#define __RG_INTERNAL_TRANS_PROB_H

#include <map>

#include "struct.h"
#include "CPdf.h"
#include "markov_types.h"

///////////////////////////////////////////////////////////////////////////////
//

class CTimed_rule;

///////////////////////////////////////////////////////////////////////////////
// A rule_map associates the state information with the indexes into the rule
// matrix.  This means that a ps->ns transition is caused by the firing of 
// rule[x][y], where (x,y) are used as keys to find the state information for
// the timer and rule bounds.
///////////////////////////////////////////////////////////////////////////////
typedef map<pair<int,int>, CTimed_rule, less<pair<int,int> > > rule_map;

///////////////////////////////////////////////////////////////////////////////
// Move real_type into this name spaces.  This won't be neccessary once gcc
// supports namespaces.
///////////////////////////////////////////////////////////////////////////////
typedef CPdf::real_type real_type;

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// CTimed_rule is a class designed to hold essential information about a timed
// rule.  A timed rule is a normal rule coupled with timing information and a 
// next state value, meaning that the rule can fire between the minimum and 
// maximum values of its timers and its lower and upper bound.  When the rule
// fires, it will cause the region graph to move to the next state.  Inside of
// the ruleADT is all of the data concerning the rule including its density
// function.
///////////////////////////////////////////////////////////////////////////////
class CTimed_rule {
protected:

  typedef CPdf::real_type real_type;

  int m_ltb;        // Lower timer value

  int m_utb;        // upper timer value

  ruleADT m_rule;   // Note: this is a const pointer

  int m_next_state; // Next state number

public:
  
  /////////////////////////////////////////////////////////////////////////////
  // Simple constructor that inserts the next state into the object. When these
  // are constructed, all that is known about the rule is next_state.  Timing 
  // information and the rule itself are extracted later.
  /////////////////////////////////////////////////////////////////////////////
  CTimed_rule( const int& ns );

  /////////////////////////////////////////////////////////////////////////////
  // Copy constructor.
  /////////////////////////////////////////////////////////////////////////////
  CTimed_rule( const CTimed_rule& r );

  /////////////////////////////////////////////////////////////////////////////
  // Basic destructor
  /////////////////////////////////////////////////////////////////////////////
  ~CTimed_rule();

  /////////////////////////////////////////////////////////////////////////////
  // Sets the value of the lower timer for this timed rule.
  /////////////////////////////////////////////////////////////////////////////
  void ltb( const int& lt );

  /////////////////////////////////////////////////////////////////////////////
  // Sets the value of the upper timer value for this rule.
  /////////////////////////////////////////////////////////////////////////////
  void utb( const int& ut );

  /////////////////////////////////////////////////////////////////////////////
  // Set the ruleADT which is a pointer to a rule_struct.
  // NOTE: See struct.h for more details.
  /////////////////////////////////////////////////////////////////////////////
  void rule( ruleADT r );

  /////////////////////////////////////////////////////////////////////////////
  // Returns the lower timer bound for this CTimed_rule.
  /////////////////////////////////////////////////////////////////////////////
  int ltb() const;
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns the upper timer bound for this CTimed_rule.
  /////////////////////////////////////////////////////////////////////////////
  int utb() const;

  /////////////////////////////////////////////////////////////////////////////
  // Returns the next state value when this rule fires.
  /////////////////////////////////////////////////////////////////////////////
  int next_state() const;

  /////////////////////////////////////////////////////////////////////////////
  // Returns the lower bound for the CTimed_rule found in the ruleADT.
  /////////////////////////////////////////////////////////////////////////////
  int lb() const;

  /////////////////////////////////////////////////////////////////////////////
  // Returns the density function for the given timed rule.
  ////////////////////////////////////////////////////////////////////////////
  const CPdf* density() const;

  /////////////////////////////////////////////////////////////////////////////
  // Returns the value of the integral of the CTimed_rules pdf function found
  // in the ruleADT between the lower timer bound and the rules upper bound.
  // (i.e. integrate( m_ltb, m_rule->upper, tol ) ).
  // NOTE: This is the probability that the rule didn't fire before entering
  // this timer region.
  /////////////////////////////////////////////////////////////////////////////
  prob_type conditional( const real_type& tol ) const;

  /////////////////////////////////////////////////////////////////////////////
  // Returns the integral of the CTimed_rule's pdf found in the ruleADT between
  // (a,b) using tol as the accuracy designator.
  /////////////////////////////////////////////////////////////////////////////
  prob_type integrate( const real_type& a, 
		       const real_type& b, 
		       const real_type& tol ) const;

  /////////////////////////////////////////////////////////////////////////////
  // Compares two CTimed_rules and returns true is all member variables are 
  // equal.
  /////////////////////////////////////////////////////////////////////////////
  bool operator==( const CTimed_rule& r );

  /////////////////////////////////////////////////////////////////////////////
  // Describes how to send a CTimed_rule to a stream. 
  // ex. timer(0,6):rule(<1,8> => U(1,8) : 1) -> 6
  /////////////////////////////////////////////////////////////////////////////
  friend ostream& operator<<( ostream& s, const CTimed_rule& r );

};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This operator defines how to output a rule_map::value_type to a ostream.
// Used to output the rule_map quickly and easily.
// ex. (6,3):timer(1,6):rule(<3,6> => U(3,6) : 1) -> 2
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const rule_map::value_type& v );

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// transition_probability is a simple function to quickly check the following
// conditions:
//    1) If only a single CTimed_rule exists in this regions, set the 
//       the probability to 1 and return.
//    2) If there are dummy rules in this region, make each rule equally 
//       probable ( ex. 1 / #rules ) and return.
//    3) If multiple rules are enables and there are not any dummy rules,
//       call calc_trans_prob to find their transition probabilities.
// Parameter list:
//    1) Present state of the region graph
//    2) rule_map containing the enabled CTimed_rules
//    3) insert_iterator for a matrix_type that hold the transition props.
//    4) The number of steps to use in the probability calculation.
//    5) The accuracy for each integral on the CTimed_rule's pdf.
///////////////////////////////////////////////////////////////////////////////
void transition_probability( const int& ps,                     
			     const rule_map& rm,                
			     insert_iterator<matrix_type>& ins, 
			     const real_type& steps,
			     const real_type& tol );

///////////////////////////////////////////////////////////////////////////////
// calc_trans_prob caclulates the transition probability in a region with 
// multiple rules enabled to fire.  The results of the calculation for each
// CTimed_rule in the region are directly inserted into a matrix with ins.
// NOTE: This function does not check for dummy rules and single rule regions.
// those cases are handled in transition_probability.  This should never be
// called directly.
// Parameter list: See transition_probability function.
///////////////////////////////////////////////////////////////////////////////
void calc_trans_prob( const int& ps,
		      const rule_map& rm,
		      insert_iterator<matrix_type>& ins,
		      const real_type& steps,
		      const real_type& tol );

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function walks through a region graph and builds up rule_maps for
// each state in the graph.  A rule_map is a list of CTimed_rules enabled
// in a state.  This is in transition_calc to find the transition probabilites.
// NOTE: This currently returns a matrix_type AND stores the transition 
// probabilities in statelistADTs in the stateADT.
///////////////////////////////////////////////////////////////////////////////
matrix_type rg_linear_approx( ruleADT **rules,
			      stateADT *state_table );

//
///////////////////////////////////////////////////////////////////////////////
#endif
