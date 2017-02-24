///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1998 by Eric G. Mercer
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
#include <cassert>
#include <iostream>
#include <map>
#include <cstring>
#include <algorithm>
#include <numeric>

#ifndef MAXINT
#ifndef OSX
#ifndef NT
#include <values.h>
#endif
#endif
#endif

#include <cmath>

#include "bm_trans_probs.h"
#include "markov_matrix_func.h"
#include "partition.h"
#include "stateasgn.h"

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// I added these to make the code more readable and avoid hardcoding these
// values.  Some say thats a little extreme, but I think its just one of 
// programming idiosyncrasies unique to each individual's style.
// These are the character values used in the state coding on the state
// table.
/////////////////////////////////////////////////////////////////////////////
#define _LOW_  '0'
#define _HIGH_ '1'
#define _RISE_ 'R'
#define _FALL_ 'F'

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Searched the rule matrix on the column specified by event until it finds 
// the first none NORULE entry.  Once this entry(rule) is located, 
// the function return rule->dist which is the CPdf* associated with the rule.
// NOTE: This returns the distribution in the first rule it finds, therefore
// each unique event maps to a single distribution irregardless of the
// trigger signal or causal rule.
/////////////////////////////////////////////////////////////////////////////
pdf_cache::CPdf_ptr pdf_cache::find_pdf( const unsigned int& event ) {
  for ( unsigned int i = 0 ; i < m_cache.size() ; i++ )    
    if ( m_rules[i][event] )
      if ( !( m_rules[i][event]->ruletype == NORULE ) )
	return( m_rules[i][event]->dist );
#ifndef __RELEASE
  assert( false );
#endif
  return( (CPdf_ptr)NULL );
}

/////////////////////////////////////////////////////////////////////////////
// constructor that initializes the cache to empty and directs the 
// rule matrix pointer to rules.
/////////////////////////////////////////////////////////////////////////////
pdf_cache::pdf_cache( const unsigned int& nevents, ruleADT** rules )
  : m_cache( nevents, (CPdf_ptr)NULL ),
    m_rules( rules ) {
  // Nothing to do
}

/////////////////////////////////////////////////////////////////////////////
// I wonder why I keep adding these things, since they never actually
// need to contain code!
/////////////////////////////////////////////////////////////////////////////
pdf_cache::~pdf_cache() {
  // Nothing to do
}

/////////////////////////////////////////////////////////////////////////////
// lookup checks the cache at location event and if its full returns the
// cache contents.  Otherwise, it goes to the rule matrix and finds 'a' CPdf
// for the event, places the CPdf in the cache at the event location, and 
// then returns the CPdf.
/////////////////////////////////////////////////////////////////////////////
pdf_cache::CPdf_ptr pdf_cache::lookup( const unsigned int& event ) {
#ifndef __RELEASE
  assert( event < m_cache.size() );
#endif
  CPdf_ptr tmp = m_cache[event];
  if ( tmp != (CPdf_ptr)NULL ) 
    return( tmp );
  tmp = find_pdf( event );
  m_cache[event] = tmp;
  return( tmp );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// This is what will be stored on the queue during the search procedure.
// NOTE: The event list probably isn't space efficient, but most of the time
// it is empty or only contains a 'few' entries.
/////////////////////////////////////////////////////////////////////////////
typedef pair<stateADT,event_list> queue_entry;

/////////////////////////////////////////////////////////////////////////////
// find_state_zero systematically tranverses the stateADT s[] until it finds
// a state that is assigned to number zero and returns the pointer to that
// state.
/////////////////////////////////////////////////////////////////////////////
stateADT find_state_zero( stateADT s[] ) {
  for ( int k = 0 ; k < PRIME ; k++ ) { 
    for ( stateADT curstate = s[k] ; 
	  curstate != NULL && curstate->state != NULL;
	  curstate = curstate->link ) {
      if ( curstate->number == 0 )
	return( curstate );
    }
  }
#ifndef __RELEASE         // This may need to be replaced with something
  assert( false );        // more elegant, but if there is no state zer
#endif                    // something went tragically wrong.
  return( (stateADT)NULL );
}

/////////////////////////////////////////////////////////////////////////////
// subset burst returns truee iff the enabled signals in [first2,last1) are
// a subset of the enabled signals in [first1,last2).  For burst mode 
// specifications, this is sufficient to be exact.  However, for non burst
// mode applications, this may create bizarre results.
/////////////////////////////////////////////////////////////////////////////
template<class InputIterator>
bool subset_burst( InputIterator first1, InputIterator last1,
		   InputIterator first2 ) {
  while( first1 != last1 ) {
    // If the first1 is 0 or 1, but first2 if R or F first2 cannot be a 
    // subset of first1.
    if ( ( *first1 == _LOW_ || *first1 == _HIGH_ ) &&
	 ( *first2 == _RISE_ || *first2 == _FALL_ ) )
      return( false );
    // If first1 is R or F the first2 had better have changed or still
    // have the same enabling, or it cannot be a subset burst
    if ( ( *first1 == _RISE_ && ( *first2 == _LOW_ || *first2 == _FALL_ ) ) ||
	 ( *first1 == _FALL_ && ( *first2 == _HIGH_ || *first2 == _RISE_ ) ) )
      return( false );
    ++first1;
    ++first2;
  }
  return( true );
}

/////////////////////////////////////////////////////////////////////////////
// operator<< gives a nice way of dumping a queue entry to an output stream
// This should someday be moved to a location where it can be used by other
// applications.
///////////////////////////////////////////////////////////////////////////// 
ostream& operator<<( ostream& s, 
		     const pair<stateADT,list<const CPdf*> >& p ) {
  s << (p.first)->state << ": ";
  for ( list<const CPdf*>::const_iterator i = (p.second).begin() ;
	i != (p.second).end() ;
	i++ )
    s << **i << " ";
  return( s );
}

/////////////////////////////////////////////////////////////////////////////
// adjust_width was engendered from inaccuracies that blasted into the 
// numerical integrations when area estimates were generated for a slice that
// was partly in and partly out of the actual function definition.  Instead
// of the integration estimating a non-continous change from probability to
// no probability on the boundary points, it would smooth the edge as if it
// were continous.  Because of this it was necessary to adjust the step size 
// to fall on successive integer points in the integration.  It this way 
// discountinuities can be handled by partitioning the integration into 
// continuous segments; avoiding integration accross the discontinuities.
// adjust_width will return a step size that is valid for the entire integral
// range that falls directly on the boundary partitions, whatever they may be.
/////////////////////////////////////////////////////////////////////////////
CPdf::real_type adjust_width( const CPdf::real_type& w ) {
  double tmp = rint( 1.0 / w );
  if ( tmp == 0 ) return( 1.0 ); // I force the step size to be 1 or less.
  return( 1.0 / tmp );           
}

/////////////////////////////////////////////////////////////////////////////
// step_calc is a function object that is used to calculate the value of
// a single integral step for a CPdf (probability distribution function) type.
// Necessary to this calculation is a knowledge of the step location 
// relative to some reference point and the width of the step ( width of the
// area estimate ).
/////////////////////////////////////////////////////////////////////////////
struct step_calc 
  : public unary_function<const CPdf*, prob_type> {

  typedef CPdf::real_type real_type;
  real_type step;  // The number of this step to project distance from base
  real_type base;  // Reference point used to gauge location in function
  real_type width; // The width of the step sized used in the area estimate

  step_calc( const real_type& s, 
	     const real_type& b, 
	     const real_type& w )
    : step( s ), base( b ), width( w ) {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the area under the function c around (base + step*width) using 
  // simpsons approximation. See CPdf.h for more information.
  /////////////////////////////////////////////////////////////////////////////
  prob_type operator()( const CPdf* c ) {
    return( (*c).area( step, base, width ) );
  }

};

/////////////////////////////////////////////////////////////////////////////
// enabled_update is a function object that is used to update integral data
// that is necessary to the burst mode heuristic.  In calculating a 
// transition probability, two things are required for the set of enabled
// signals ( events that can transition in that state ): the cumulative sum
// of there integral from 0 up until the current point tracked by step_calc
// and the area under the curve at the current point denoted by step_calc.
// Since one value is a sum and the other is just an assignment, this function
// object pulls out the CPdf from trans_type, makes the call to step_calc and
// then assigns the value to the assign_iter and adds the value to the
// sum_iter.
/////////////////////////////////////////////////////////////////////////////
struct enabled_update
  : public unary_function<trans_type,void> {

  typedef prob_vector::iterator iterator;

  step_calc sc;         // Function gateway
  iterator sum_iter;    // Iterator that recieves itself + the new value of sc 
  iterator assign_iter; // Iterator that recieves the new value from sc 

  enabled_update( const step_calc& s,
		  iterator s_iter,
		  iterator a_iter )
    : sc( s ), sum_iter( s_iter ), assign_iter( a_iter ) {
  }

  /////////////////////////////////////////////////////////////////////////////
  // calls step_calc to get the value of the area at this current step_calc
  // point and assigns/adds the value to the corresponding output iterators.
  /////////////////////////////////////////////////////////////////////////////
  void operator()( const trans_type& t ) {
    prob_type tmp = sc( t.second );
    *sum_iter++ += tmp;
    *assign_iter++ = tmp;
  }

};

/////////////////////////////////////////////////////////////////////////////
// fired_update is a function object that calculates the cumulative probability
// of signals that have already fired in a concurrent burst from time 0 
// up until the current step in step_calc sc.  This function object takes
// two arguments, the first argurment is the CPdf (probability distribution
// function) and the second argument is the current cumulative probability.
// The function returns the value of p + the area returned from step_calc.
/////////////////////////////////////////////////////////////////////////////
struct fired_update 
  : public binary_function<const CPdf*, prob_type, prob_type> {

  step_calc sc;
  
  fired_update( const step_calc& s ) : sc( s ) {
  }

  prob_type operator()( const CPdf* c, const prob_type& p ) {
    return( p + sc( c ) );
  }

};

/////////////////////////////////////////////////////////////////////////////
// partition_handler was designed to deal with a list of partitions that
// contained intersecting probabilty distributions.  In this way an integral
// can be divided into continous segments and avoid integrating across the
// discontinuities caused by the bounded probability distributions functions
// for the events.  This class is a function object that has been designed
// to be used with the for_each standard template algorithm.  Since this 
// algorithm actually calculates the transition probability for a set of
// enabled signals, its construction is a little more complicated that the 
// ordinary run-of-the-mill function object.
//
// The probability integral is described ignoring the partition mess and
// using discrete function notation, since numerical integration discretizes
// the functions to a fixed precision.  For this calculation, three sets are
// used:  fen which is the set of probabilty density function for the signals
// that are enabled to transition in this burst. Fen which is the set of  
// probability mass functions or cumulative max functions for the signals
// that are enabled to fire in the burst, and Ff which is the cumulative
// probability mass functions for the signals that have already fired in the
// burst.  The calculation of a transition probability for f in fen can be 
// found as follows:
// 
// sum( i in [l,u] )[ f(i) * ( sum( Fj in Fen and f != Fj )[ ( 1 - Fj(i) ) ] *
//                             sum( Fk in Ff )[ Fk(i) ] ) ]
//
// partition_handler implements the above function, but caclulate the 
// cumulative mass functions while calculating the transition probability at
// each step of the integral.  It also calculates the probability of trans-
// tioning for all enabled signals, so it gets the maximal reuse out of 
// every integral step calculation.
//
// The only wrench caused by the partitions is that you have to carry 
// probability values from one partition into another.  A data accounting
// nightmare.
/////////////////////////////////////////////////////////////////////////////
class partition_handler 
  : public unary_function<partition_type,void> {
protected:
  
  typedef CPdf::real_type                    real_type;

  // The fired cache is keyed off of a CPdf* and holds the current
  // cumulative mass of a CPdf belonging to a signal or event in the
  // fired set from time 0 up until the current integration step.
  typedef map<const CPdf*,prob_type>         fired_cache_type;
  typedef fired_cache_type::const_iterator   fired_const_iter;
  typedef fired_cache_type::iterator         fired_iter;

  // The enabled cache is keyed off of a trans_type which is a next
  // state number paired with the CPdf of the event that fires to take
  // you to that state.  The cache associates the trans_type with the
  // current probability value for the trans_type.  The probablity
  // value can be a cumulative mass from time zero to the current
  // step, or it can be the cumulative value of the integral that
  // calculates the actual transition probability ( a weighted 
  // cumulative mass value ).
  typedef map<trans_type,prob_type>          enabled_cache_type;
  typedef enabled_cache_type::const_iterator enabled_const_iter;
  typedef enabled_cache_type::iterator       enabled_iter;
  typedef enabled_cache_type::value_type     value_type;

  typedef trans_list::const_iterator         trans_iter;
  typedef event_list::const_iterator         event_iter;

  fired_cache_type   m_fired_cache;    // Cumulative mass cache for fired set
  enabled_cache_type m_enabled_cache;  // Cumulative mass cache for enabled set
  enabled_cache_type m_prob_cache;     // Weighted Cumulative mass of enabled
                                       // set
  unsigned int       m_ps;             // State number for the current burst
  real_type          m_w;              // Step width to use in numerical int.

protected:

  /////////////////////////////////////////////////////////////////////////////
  // cache_acc is a function object that can correctly add a prob_type to a
  // enabled_cach_type::value_type and return the value of the addition.  This
  // is designed to be used with accumulate to sum the transition probability
  // values in m_prob_cache prerequisite to normalization.
  /////////////////////////////////////////////////////////////////////////////
  struct cache_acc 
    : public binary_function<prob_type,value_type,prob_type> {
    prob_type operator()( const prob_type& a,
			  const enabled_cache_type::value_type& b ) const {
      return( a + b.second );
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  // cache_divide is a function object that can take a enabled_cache_type::
  // value_type and correctly divide it by a prob_type and return the results
  // in the form of a matrix_type::value_type as defined in markov_types.h.  
  // This is the final step in the transition probability calculation where
  // the values in the m_prob_cache are normalized and returned in a form
  // than can be assigned into a matrix_type container.  m_ps denoted the
  // present state of the system or row entry in the matrix_type and 
  // the column entry is taken from the trans_type which is the key in the
  // enabled_cache_type::value_type.
  /////////////////////////////////////////////////////////////////////////////
  struct cache_divide
    : public binary_function<value_type,prob_type,matrix_type::value_type> {
    
    unsigned int m_ps;
    
    cache_divide( const unsigned int ps ) : m_ps( ps ) {
    }

    matrix_type::value_type operator()( const value_type& a,
					const prob_type& b )  const {
      if ( b == 0 || a.second < 0 ) { // Watch out for the divide by zero!
	return( make_value_type( m_ps, a.first.first, 0 ) );
      }
      prob_type tmp =  a.second / b;
      if ( tmp > 1 ) {
	return(  make_value_type( m_ps, a.first.first, 1 ) );
      }
      return( make_value_type( m_ps, a.first.first, tmp ) );
    }

  };
  
  /////////////////////////////////////////////////////////////////////////////
  // initialize_E_vector is used when moving from one partition to another.
  // This function will copy for each trans_type in the trans_list its current
  // cumulative probability mass as recorded in the m_enabled_cache into 
  // e_iter.
  /////////////////////////////////////////////////////////////////////////////
  void initialize_E_vector( const trans_list& enabled,
			    prob_vector::iterator e_iter ) {
    for ( trans_iter i = enabled.begin() ; i != enabled.end() ; i++ ) {
      enabled_const_iter j = m_enabled_cache.find( *i );
#ifndef __RELEASE
      assert( j != m_enabled_cache.end() );
#endif
      *e_iter++ = (*j).second;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // initialize_F_vector is used when moving from one partition to another.
  // This function will copy for each CPdf* in the event_list its current
  // cumulative probability mass as recorded in the m_fired_cache into 
  // f_iter.
  ////////////////////////////////////////////////////////////////////////////
  void initialize_F_vector( const event_list& fired,
			   prob_vector::iterator f_iter ) {
    for ( event_iter i = fired.begin() ; i != fired.end() ; i++ ) {
      fired_const_iter j = m_fired_cache.find( *i );
#ifndef __RELEASE
      assert( j != m_fired_cache.end() );
#endif
      *f_iter++ = (*j).second;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // update_E_results copies the value found in e_iter into the m_enabled_cache
  // for each trans_type found in the trans_list enabled.  This is used to
  // record into the cache the results of the numerical integration in the
  // most recent partition.
  /////////////////////////////////////////////////////////////////////////////
  void update_E_results( const trans_list& enabled, 
			 prob_vector::const_iterator e_iter ) {
    for ( trans_iter i = enabled.begin() ; i != enabled.end() ; i++ ) {
      enabled_iter j = m_enabled_cache.find( *i );
#ifndef __RELEASE
      assert( j != m_enabled_cache.end() );
#endif
      (*j).second = *e_iter++;
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // update_F_results copies the value found in f_iter into the m_fired_cache
  // for each CPdf* found in the event_list fired.  This is used to
  // record into the cache the results of the numerical integration in the
  // most recent partition.
  /////////////////////////////////////////////////////////////////////////////
  void update_F_results( const event_list& fired, 
		       prob_vector::const_iterator f_iter ) {
    for ( event_iter i = fired.begin() ; i != fired.end() ; i++ ) {
      fired_iter j = m_fired_cache.find( *i );
#ifndef __RELEASE
      assert( j != m_fired_cache.end() );
#endif
      (*j).second = *f_iter++;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // trans_prob is used to calculate the amount of probability to be added
  // to the weighted cumulative probability mass representing the transition
  // probability for the enabled signal or event self.  This function uses 
  // the cumulative mass probabilities that are recorded in the m_enabled_cache
  // and the m_fired_cache.  Access to these values is provied via the 
  // iterators that denote the ranges: [E_first,E_last) and [F_first,F_last).
  // self is necessary to prevent the current signal under scrutiny from 
  // weighting its own probabilility as a signal that should not fire.  The
  // value current is the probability of the event self firing at the current
  // itegration step.  
  // NOTE: This funcion maps directly to the one presented in the comments for
  // this class.
  /////////////////////////////////////////////////////////////////////////////
  prob_type trans_prob( enabled_const_iter E_first, 
			enabled_const_iter E_last,
			fired_const_iter F_first, 
			fired_const_iter F_last,
			prob_type current, 
			enabled_const_iter self ) const {
    // Weight current by the probability of the other events in the
    // enabled_list not firing AND the events in the fired list firing 
    while( E_first != E_last && F_first != F_last ) {
      if ( E_first != self )
	current *= ( 1.0 - (*E_first).second );
      current *= (*F_first++).second;
      ++E_first;
    }
    // Get the remaining enabled events that aren't suppose to fire
    for ( ; E_first != E_last ; E_first++ ) {
      if ( E_first != self ) {
	current *= ( 1.0 - (*E_first).second ); 
      }
    }
    // Get the remaining fired events that are suppose to fire.
    for ( ; F_first != F_last ; F_first++ ) 
      current *= (*F_first).second;
    return( current );
  }

public:

  /////////////////////////////////////////////////////////////////////////////
  // The constructor for partition_handler does the following things:
  //    1) Calculates the adjusted width to use in the numerical integrals
  //       so that the integration drops on partition boundaries.
  //    2) Initialize all entries in the m_enabled_cache to zero
  //    3) Initialize all entries in the m_fired_cache to zero
  //    4) Initialize all entries in the m_prob_cache to zero
  // NOTE: The enabled and fired lists are necessary to set initialize the
  // different caches.  The ps is stored to use when changing the transition
  // probabilities into the standard matrix_type::value_type form.  The last
  // two parameters, tolerance and bounds are used to set the step width in
  // the numerical integral with the adjust_width function.
  /////////////////////////////////////////////////////////////////////////////
  partition_handler( const trans_list& enabled,
		     const event_list& fired,
		     const bound_type& bounds, 
		     const unsigned int& ps,
		     const real_type& tol ) 
    : m_fired_cache(), 
      m_enabled_cache(), 
      m_prob_cache(), 
      m_ps( ps ), 
      m_w( 0 ) {
    m_w = adjust_width( ( bounds.upper() - bounds.lower() ) * tol );
//     cout << endl << "m_w -> " << m_w;
//     cout << endl << "bounds -> " << bounds;
//     cout << endl << "tolerance -> " << tol << flush;
    for ( trans_iter i = enabled.begin() ; i != enabled.end() ; i++ ) {
      m_enabled_cache[*i] = 0;
      m_prob_cache[*i] = 0;
    }
    for ( event_iter i = fired.begin() ; i != fired.end() ; i++ ) {
      // Only add the fired events into the cache that can actually have
      // probability within the bounds.  This is because if they can never
      // fire, it will really mess up the calculation
      if ( (*i)->lower() <= bounds.upper() )
	m_fired_cache[*i] = 0;
    }
  }
    
  /////////////////////////////////////////////////////////////////////////////
  // Copy constructor since I will be tossing these around a bit.
  /////////////////////////////////////////////////////////////////////////////
  partition_handler( const partition_handler& p )
    : m_fired_cache( p.m_fired_cache ), 
      m_enabled_cache( p.m_enabled_cache ), 
      m_prob_cache( p.m_prob_cache ), 
      m_ps( p.m_ps ), 
      m_w( p.m_w ) {
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // operator() is the core of the function object.  This is used by the 
  // for_each stl algorithm to calculate the probability masses in each 
  // partition.  Each partition contains a set of intersecting CPdf functions,
  // so simply summing over the integrals on the partitions will give the
  // exact transition probabilities in burst mode specification.  This 
  // recieves a partition_type p, computes the probabilities, and saves the
  // results into the appropriate cache container in 'this' object.
  /////////////////////////////////////////////////////////////////////////////
  void operator()( const partition_type& p ) {
    const trans_list& enabled = p.enabled();
    const event_list& fired = p.fired();
    prob_vector E( enabled.size(), (prob_type)0.0 );
    prob_vector F( fired.size(), (prob_type)0.0 );

    // Fill the E and F vectors with the cumulative probabilities mass from
    // the previous partitions.
    initialize_E_vector( enabled, E.begin() );
    initialize_F_vector( fired, F.begin() );

    // The C vector is the value the current step integral for each 
    // signal in the enabled set.  The bounds are used to calculate the
    // number of integral steps contained in this partition and to calculate
    // the correct distance to move into each function on the integral
    // step calculations.
    bound_type b( p.bounds() );
    prob_vector C( enabled.size(), (prob_type)0.0 );
    unsigned int steps = (int)(rint( 1.0 / m_w ) * ( b.upper() - b.lower() ));

    // Sum up each step of the integral
    for ( unsigned int i = 1 ; i <= steps ; i++ ) {

      // Calculate the probability mass and integral step areal for each
      // signal in the enabled and fired set.
      step_calc s( i, b.lower(), m_w );
      ::for_each( enabled.begin(), enabled.end(), 
		  enabled_update( s , E.begin(), C.begin() ) );
      ::transform( fired.begin(), fired.end(), 
		   F.begin(), F.begin(), fired_update( s ) );

      // Save the results into the cache objects in 'this' object
      update_E_results( enabled, E.begin() );
      update_F_results( fired, F.begin() );
      
      // Update the transition probabilities using the cumulative mass 
      // from the enabled set and the fired set and using the integral
      // step value in the C vector.  Care must be taken to specify in
      // trans_prob what signal is currently being evaluated, so its
      // cumulative mass in m_enabled_cache doesn't weight its trasition 
      // probability.
      prob_vector::iterator c_iter = C.begin();
      for ( trans_iter j = enabled.begin() ; j != enabled.end() ; j++ ){
	enabled_const_iter self = m_enabled_cache.find( *j );
	enabled_iter T_out = m_prob_cache.find( *j );
#ifndef __RELEASE
	assert( self != m_enabled_cache.end() );
	assert( T_out != m_prob_cache.end() );
#endif
	(*T_out).second += trans_prob( m_enabled_cache.begin(), 
				       m_enabled_cache.end(),
				       m_fired_cache.begin(), 
				       m_fired_cache.end(), 
				       *c_iter++, 
				       self );
      } 
    }
  }    

  /////////////////////////////////////////////////////////////////////////////
  // Dumps to the ostream s a textural ascii version of 'this' object.
  /////////////////////////////////////////////////////////////////////////////
  void write_string( ostream& s ) const {
    s << "ps -> " << m_ps;
    s << endl << "w -> " << m_w;
    s << endl << "E -> ";
    for ( enabled_const_iter i = m_enabled_cache.begin() ;
	  i != m_enabled_cache.end() ; i++ ) {
      s << "<(" << ((*i).first).first << ","
	<< *((*i).first).second << ")," << (*i).second << ">   ";
    }
    s << endl << "P -> ";
    for ( enabled_const_iter i = m_prob_cache.begin() ;
	  i != m_prob_cache.end() ; i++ ) {
      s << "<(" << ((*i).first).first << ","
	<< *((*i).first).second << ")," << (*i).second << ">   ";
    }
    s << endl << "F -> ";
    for ( fired_const_iter i = m_fired_cache.begin() ;
	  i != m_fired_cache.end() ; i++ ) {
      s << "<" << ((*i).first) << "," << (*i).second << ">   ";
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // normalize is used to ensure that all transition probability values from
  // a given state ( transition probabilities for the enabled signal set ) 
  // represent a valid probability distribution that sums to 1.
  // This is done by calculation the sum of the probabilities in the 
  // m_prob_cache set and then dividing each probability by the sum.  The 
  // results of the division is stored in a matrix_type::value_type object
  // and assigned to the OutputIterator i_out. 
  /////////////////////////////////////////////////////////////////////////////
  void normalize( OutputIterator i_out ) const {
    transform( m_prob_cache.begin(), m_prob_cache.end(), i_out,
		 bind2nd( cache_divide(m_ps), 
			  accumulate( m_prob_cache.begin(), 
					m_prob_cache.end(), 
					0.0,
					cache_acc() ) ) );
  }

};

/////////////////////////////////////////////////////////////////////////////
// infinity_handler is a function object that simply tacks a  trans_type and
// prob_type b and uses m_ps to create and return an object matrix_type::
// value_type.  The is created to deal with signals that are concurrently
// enabled with bounds [l,infinity] where l is some arbitrary lower bound.
// This object will take the probability, denoted with b, and simply use
// it and the trans_type to return a valid matrix_type::value_type.  This
// is designed to be used with transform.
/////////////////////////////////////////////////////////////////////////////
struct infinity_handler
  : public binary_function<trans_type,prob_type,matrix_type::value_type> {
    
  unsigned int m_ps;
  
  infinity_handler( const unsigned int ps ) : m_ps( ps ) {
  }
  
  matrix_type::value_type operator()( const trans_type& a,
				      const prob_type& b )  const {
    return( make_value_type( m_ps, a.first, b ) );
    }
};

/////////////////////////////////////////////////////////////////////////////
// assign_trans_info call the function necessary to calculate and assign to 
// i_out the transition_probabilities for the enabled set of signals 
// contained in the trans_list transition.  Current is used to track the
// current state number and to extract the fired set of signals.  Tolerance
// is necessary for the partition_handler to calculate the step size to use
// in the numerical integral and i_out is the tie to the transition probab-
// ilities final resting or storage location.
/////////////////////////////////////////////////////////////////////////////
void assign_trans_info( queue_entry current,
			const partition_list& partition,
			const trans_list& transition,
			const CPdf::real_type& tolerance,
			OutputIterator i_out ) {
  // Only one signal is enabled in this state. Don's waste time with 
  // the numerical integral!  Give it a probability of 1 and return.
  if ( transition.size() == 1 ) {
    *i_out++ = make_value_type( (current.first)->number,
				(*transition.begin()).first, 1.0 );
    return;
  }

  // This checks to see if there is a group of signal that are enabled
  // on [l,infinity].  If this is the case, give each signal an equal
  // probability of firing and return.
  // NOTE: I should add something to take into account the lower bounds
  // of each of the signals and weight their probability accordingly.
  if ( partition.bounds().upper() == INFIN ) {
    ::transform( transition.begin(), transition.end(), i_out, 
	         bind2nd( infinity_handler( (current.first)->number ),
			  1.0 / (CPdf::real_type)transition.size() ) ); 
    return;
  }

//   cout << endl << endl << "Present state -> " << (current.first)->number; 
//   cout << endl << "Partition list -> " << partition.bounds() << endl;
//   ::copy( partition.begin(), partition.bounded_end(),
// 	  ostream_iterator<partition_type>(cout,"\n") );

  // I need to perfrom the numerical integral, so construct the parition
  // handler and use the for_each algorithm to sum the integrals of each
  // of the partitions.
  partition_handler 
    result( ::for_each( partition.begin(),
			partition.bounded_end(),
			partition_handler( transition,
					   (current.second),
					   partition.bounds(),
					   (current.first)->number,
					   tolerance ) ) );      
  result.normalize( i_out ); 
}

/////////////////////////////////////////////////////////////////////////////
// This function uses a burst more approximation algorithm to assign 
// transition probabilities to the edges in the state table states.  The 
// rules matrix is necessary as an entry point to the distribution functions
// for the events.  Tolerance is used as a stepsize suggestion for all of
// the numerical integrations.  The actual transition probabilities are 
// assigned as matrix_type::value_types to the iterator out.  The function
// returns true if all went well, otherwise it returns false.  
// 
// The heuristic algorithm is based on synchronization points in the state
// graph between successive concurrent bursts.  When non-burst mode type
// designs are fed into this heuristic, accuracy of the results quickly
// degrades, since the algorithm must guess where synchronization points 
// occur.
//
// NOTE: This only works with TER structures!  Anything TEL, all bets
// are off, you get what you deserve.
/////////////////////////////////////////////////////////////////////////////
bool burst_mode_transition_probabilities( stateADT states[],
					  ruleADT* rules[],
					  const unsigned int& nevents,
					  const CPdf::real_type& tolerance,
					  OutputIterator i_out ) {
  pdf_cache cache( nevents, rules ); // Cache to find pdfs for the events
  clearColors( states, 0 );          // Initialize the state graph
  
  // Rather than use the call stack and recurrsion, I'm going to use a
  // big loop with a queue of things that need to be processed.  This will
  // more easily accomodate a breadth first search.
  front_queue<queue_entry> search_queue; 
  search_queue.push( queue_entry( find_state_zero( states ), event_list() ) );

  // I used the stl algorithms on the strings representing the states.  Since
  // the strings here are really just character arrays in contiguous memory,
  // adding the num_sigs onto the pointer to the first character gives the
  // end of the character array. The range is [char*,char* + num_sigs).
  unsigned int num_sigs = strlen( ((search_queue.front()).first)->state );
  while( !search_queue.empty() ) { // Loop until the queue is empty
    queue_entry current = search_queue.front(); // Get the first element
    search_queue.pop();                         // Remove the first element

    if ( !(current.first)->color ) {            // Check its color
      trans_list transition;                    // List of outgoing edges
      // partition_list divides pdfs into intersecting segments
      partition_list partition;
      
      // For each outgoing edged from the current state, I need to add
      // the rule to the partition_list andthe transition list.  I then
      // need to add the next states onto the search queue if they have not
      // already been searched.
      for (statelistADT i = (current.first)->succ ; i != NULL ; i = i->next) {
#ifndef __RELEASE
	assert( i->stateptr != NULL );
#endif
	partition( trans_type( i->stateptr->number, 
			       cache.lookup( i->enabled ) ) );
	transition.push_front( trans_type( i->stateptr->number, 
					   cache.lookup( i->enabled ) ) );
	
	// If a state is going to be added to the search queue, I need to
	// decide if it is a new concurrent burst, or if its just part of
	// the current concurrent burst.  If its a subset, the event on
	// the edge leading into this state is added to the fired list
	// of the state that is added into the search queue.  Otherwise
	// the fired list is left empty the state is added into the 
	// search queue.
	if ( !i->stateptr->color ) {
	  if ( subset_burst( (current.first)->state, 
			     (current.first)->state + num_sigs,
			     i->stateptr->state ) ) {
	    event_list tmp( current.second );
	    tmp.push_front( cache.lookup( i->enabled ) );
	    search_queue.push( queue_entry( i->stateptr, tmp ) );
	  }
	  else {
	    search_queue.push( queue_entry( i->stateptr, event_list() ) );
	  }
	}
      }

      // color this state as having been visited, add the fired rules into
      // the current partition list, and calculate the transition 
      // probabilities.  The results of assign_trans_info are sent directly
      // to the OutputIterator.
      (current.first)->color = 1;
      partition = ::for_each( (current.second).begin(), 
			      (current.second).end(),
			      partition );
      assign_trans_info( current, partition, transition, tolerance, i_out );
    }
  }
  return( true );
}

//
///////////////////////////////////////////////////////////////////////////////
