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
#include <cmath>
#include "rg_trans_prob.h"
#include "markov_matrix_func.h"

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Simple constructor that inserts the next state into the object. When these
// are constructed, all that is known about the rule is next_state.  Timing 
// information and the rule itself are extracted later.
/////////////////////////////////////////////////////////////////////////////
CTimed_rule::CTimed_rule( const int& ns ) 
  : m_ltb( 0 ),
    m_utb( 0 ),
    m_rule( NULL ),
    m_next_state( ns ) {
  // Nothing to do
}

/////////////////////////////////////////////////////////////////////////////
// Copy constructor.
/////////////////////////////////////////////////////////////////////////////
CTimed_rule::CTimed_rule( const CTimed_rule& r ) 
  : m_ltb( r.m_ltb ),
    m_utb( r.m_utb ),
    m_rule( r.m_rule ),
    m_next_state( r.m_next_state ) {
  // Nothing to do
}
  
////////////////////////////////////////////////////////////////////////////
// Basic destructor
/////////////////////////////////////////////////////////////////////////////
CTimed_rule::~CTimed_rule() {
  // Nothing to do
}

/////////////////////////////////////////////////////////////////////////////
// Sets the value of the lower timer for this timed rule.
/////////////////////////////////////////////////////////////////////////////
void CTimed_rule::ltb( const int& lt ) {
  m_ltb = abs(lt);
}

/////////////////////////////////////////////////////////////////////////////
// Sets the value of the upper timer value for this rule.
/////////////////////////////////////////////////////////////////////////////
void CTimed_rule::utb( const int& ut ) {
  m_utb = ut;
}

/////////////////////////////////////////////////////////////////////////////
// Set the ruleADT which is a pointer to a rule_struct.
// NOTE: See struct.h for more details.
/////////////////////////////////////////////////////////////////////////////
void CTimed_rule::rule( ruleADT r ) {
  m_rule = r;
}

/////////////////////////////////////////////////////////////////////////////
// Returns the lower timer bound for this CTimed_rule.
/////////////////////////////////////////////////////////////////////////////
int CTimed_rule::ltb() const {
  return( m_ltb );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the upper timer bound for this CTimed_rule.
/////////////////////////////////////////////////////////////////////////////  
int CTimed_rule::utb() const {
  return( m_utb );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the next state value when this rule fires.
/////////////////////////////////////////////////////////////////////////////
int CTimed_rule::next_state() const {
  return( m_next_state );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the lower bound for the CTimed_rule found in the ruleADT.
/////////////////////////////////////////////////////////////////////////////
int CTimed_rule::lb() const {
  if ( m_rule )
    return( m_rule->lower );
  return( -1 );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the density function for the given timed rule.
/////////////////////////////////////////////////////////////////////////////
const CPdf* CTimed_rule::density() const {
  if ( m_rule )
    return( m_rule->dist );
  return( NULL );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the value of the integral of the CTimed_rules pdf function found
// in the ruleADT between the lower timer bound and the rules upper bound.
// (i.e. integrate( m_ltb, m_rule->upper, tol ) ).
// NOTE: This is the probability that the rule didn't fire before entering
// this timer region.
/////////////////////////////////////////////////////////////////////////////
prob_type CTimed_rule::conditional( const real_type& tol ) const {
  if ( m_rule )
    return( m_rule->dist->integrate( m_ltb, m_rule->upper, tol ) );
  return( 0.0 );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the integral of the CTimed_rule's pdf found in the ruleADT between
// (a,b) using tol as the accuracy designator.
/////////////////////////////////////////////////////////////////////////////
prob_type CTimed_rule::integrate( const real_type& a, 
				  const real_type& b, 
				  const real_type& tol ) const {
  if ( m_rule )
    return( m_rule->dist->integrate( a, b, tol ) );
  return( 0.0 );
}

/////////////////////////////////////////////////////////////////////////////
// Compares two CTimed_rules and returns true is all member variables are 
// equal.
/////////////////////////////////////////////////////////////////////////////
bool CTimed_rule::operator==( const CTimed_rule& r ) {
  return( m_ltb == r.m_ltb &&
          m_utb == r.m_utb &&
	  m_rule == r.m_rule &&
	  m_next_state == r.m_next_state );
}

///////////////////////////////////////////////////////////////////////////////
// Describes how to send a CTimed_rule to a stream. 
// ex. timer(0,6):rule(<1,8> => U(1,8) : 1) -> 6
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const CTimed_rule& r ) {
  s << "timer(" << r.ltb() << "," << r.utb() << ")";
  s << ":rule("; 
  if ( r.m_rule ) {
    s << *(r.m_rule->dist);
  }
  else
    s << "NULL";
  s  << ") -> " << r.next_state();
  return( s );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This operator defines how to output a rule_map::value_type to a ostream.
// Used to output the rule_map quickly and easily.
// ex. (6,3):timer(1,6):rule(<3,6> => U(3,6) : 1) -> 2
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const rule_map::value_type& v ) {
  s << "(" << v.first.first << "," << v.first.second << "):";
  s << v.second;
  return( s );
}

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
			     const real_type& tol ) {
  if ( rm.size() == 1 ) {    // Only one rule enabled set the value and return
    *ins = make_value_type( ps, (*rm.begin()).second.next_state(), 1.0 );
    return;
  }
  // Add Dummy clauses here.  If the list is full of dummys, make
  // them equally probable.  NOTE: What is there is a mix of dummys
  // and actual rules?
  calc_trans_prob( ps, rm, ins, steps, tol );
}

///////////////////////////////////////////////////////////////////////////////
// calc_trans_prob caclulates the transition probability in a region with 
// multiple rules enabled to fire.  The results of the calculation for each
// CTimed_rule in the region are directly inserted into a matrix with ins.
// NOTE: This function does not check for dummy rules and single rule regions.
// those cases are handled in transition_probability.  This should never be
// called directly.
// Params: See transition_probability function.
///////////////////////////////////////////////////////////////////////////////
void calc_trans_prob( const int& ps,
		      const rule_map& rm,
		      insert_iterator<matrix_type>& ins,
		      const real_type& steps,
		      const real_type& tol ) {
  // Iterate through each rule
  for ( rule_map::const_iterator i = rm.begin() ; i != rm.end() ; i++ ) {
    prob_type cond_const = (*i).second.conditional( tol ); 
    prob_vector F( rm.size() - 1 );       // Initialize cond_const and allocate
    prob_vector adjust( rm.size() - 1 );  // vectors for timer adjustments and
    prob_vector::iterator fi = F.begin(); // weight constants
    prob_vector::iterator ai = adjust.begin();
    for ( rule_map::const_iterator j = rm.begin() ; j != rm.end() ; j++ )
      if ( i != j ) {                     
	cond_const *= (*j).second.conditional( tol ); 
	*fi++ = (*j).second.integrate( (*j).second.lb(), 
				       (*j).second.ltb(), tol  );
	*ai++ = (*j).second.ltb() - (*i).second.ltb();;
      }
    prob_type prob = 0;
    real_type t = (*i).second.ltb();
    real_type stepsize = (real_type)((*i).second.utb() - (*i).second.ltb()) 
                         / steps;
    for ( int k = 1 ; k <= steps; k++ ) {
      real_type weight = 1;
      fi = F.begin();
      ai = adjust.begin();
      for ( rule_map::const_iterator j = rm.begin() ; j != rm.end() ; j++ )
	if ( i != j ) {
	  *fi += (*j).second.integrate( t + *ai, t + *ai + stepsize, tol );
	  ++ai;
	  weight = weight * ( 1.0 - *fi++ );
	}
      prob += (*i).second.integrate( t, t + stepsize, tol ) * weight;
      t += stepsize;
    }
    prob = (real_type)rint( ( prob / cond_const ) * (steps/10) )/ (steps/10);
    *ins++ = make_value_type( ps, (*i).second.next_state(), prob );
  }
}

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
			      stateADT *state_table ) {
  matrix_type tran;
  for ( int k = 0 ; k < PRIME ; k++ ) 
    for ( stateADT ps = state_table[k] ; 
	  ps != NULL && ps->state != NULL ; 
	  ps = ps->link ) {
      rule_map ri;
      for ( statelistADT ns = ps->succ ; ns ; ns=ns->next ) {
	assert(( ns->enabling >= 0 ) && 
	       ( ns->enabled >= 0 ) && 
	       ( ns->stateptr ) );
	ri.insert(rule_map::value_type(make_pair( ns->enabling, ns->enabled ),
				       CTimed_rule( ns->stateptr->number ) ));
      }
      if (ps->clocklist) {
	for ( int j = 1 ; j <= ps->clocklist->clocknum ; j++ ) {
	  assert( ( ps->clocklist->clockkey[j].enabling >= 0 ) && 
		  ( ps->clocklist->clockkey[j].enabled >= 0 ) );
	  int x = ps->clocklist->clockkey[j].enabling;
	  int y = ps->clocklist->clockkey[j].enabled;
	  rule_map::iterator iter = ri.find( make_pair( x, y ) );
	  if ( iter != ri.end() ) {
	    (*iter).second.rule( rules[x][y] );
	    (*iter).second.ltb( ps->clocklist->clocks[j][0] );
	    (*iter).second.utb( ps->clocklist->clocks[0][j] );
	  }
	}
// 	cout << endl << "ri -> " << endl;
// 	::copy( ri.begin(), ri.end(), 
// 		ostream_iterator<rule_map::value_type>(cout,"\n") );
	insert_iterator<matrix_type> ins(tran,tran.begin());
	transition_probability( ps->number, ri, ins, 1000, 0.01 );

	for ( statelistADT ns = ps->succ ; ns != NULL ; ns = ns->next ) {
	  matrix_type::const_iterator i = 
 	    tran.find( (key_type)make_key_node( (index_type)ps->number, (index_type)ns->stateptr->number ) );
 	  assert( i != tran.end() );
 	  _matrix_interface mi;
 	  ns->probability = mi.prob( i );
 	}

// 	cout << endl << "tran -> " << endl;
// 	::copy( tran.begin(), tran.end(), 
// 		ostream_iterator<matrix_type::value_type>( cout, "\n" ) );
      }
    }
  return( tran );
}

//
///////////////////////////////////////////////////////////////////////////////
