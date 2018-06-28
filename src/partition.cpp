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

#ifndef MAXINT
#ifndef OSX
#ifndef NT
#include <values.h>
#endif
#endif
#endif
#include <climits>
#include "partition.h"

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Void constructor
/////////////////////////////////////////////////////////////////////////////
bound_type::bound_type() : m_lower( 0 ), m_upper( 0 ) {
}

/////////////////////////////////////////////////////////////////////////////
// Sets m_lower, m_upper to l and b repsectively.
/////////////////////////////////////////////////////////////////////////////
bound_type::bound_type( const unsigned int l, const unsigned int u )
  : m_lower( l ), m_upper( u ) {
}

/////////////////////////////////////////////////////////////////////////////
// Copy Constructor
/////////////////////////////////////////////////////////////////////////////
bound_type::bound_type( const bound_type& b )
  : m_lower( b.m_lower ), m_upper( b.m_upper ) {
}

/////////////////////////////////////////////////////////////////////////////
// Assignment Operator
/////////////////////////////////////////////////////////////////////////////
bound_type& bound_type::operator=( const bound_type& b ) {
  if ( this == &b ) return( *this );
  m_lower = b.m_lower;
  m_upper = b.m_upper;
  return( *this );
}

/////////////////////////////////////////////////////////////////////////////
// Returns the lower bound value.
/////////////////////////////////////////////////////////////////////////////
const unsigned int bound_type::lower() const {
  return( m_lower );
}
  
/////////////////////////////////////////////////////////////////////////////
// Returns the upper bound value
/////////////////////////////////////////////////////////////////////////////
const unsigned int bound_type::upper() const {
  return( m_upper );
}

/////////////////////////////////////////////////////////////////////////////
// Sets the value of the lower bound to i
/////////////////////////////////////////////////////////////////////////////
void bound_type::lower( const unsigned int& i ) {
  m_lower = i;
}

/////////////////////////////////////////////////////////////////////////////
// Sets the value of the upper bound to i
/////////////////////////////////////////////////////////////////////////////
void bound_type::upper( const unsigned int& i ) {
  m_upper = i;
}

/////////////////////////////////////////////////////////////////////////////
// outputs a bound_type in its text form: <l,u>
/////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const bound_type& b ) {
  s << "<" << b.lower() << "," << b.upper() << ">";
  return( s );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////
// Constructs a new partition_type with bounds b and empty fired and 
// enabled lists.
/////////////////////////////////////////////////////////////////////////////
partition_type::partition_type( const bound_type& b )
  : m_bounds( b ), m_Enabled(), m_Fired() {
}

/////////////////////////////////////////////////////////////////////////////
// Constructs a new parition_type with bounds b, enabled list t, and 
// fired list f.
/////////////////////////////////////////////////////////////////////////////
partition_type::partition_type( const bound_type& b, 
				const trans_list& e,
				const event_list& f )
  : m_bounds( b ), m_Enabled( e ), m_Fired( f ) {
}

/////////////////////////////////////////////////////////////////////////////
// Copy Constructor
/////////////////////////////////////////////////////////////////////////////
partition_type::partition_type( const partition_type& p )
  : m_bounds( p.m_bounds ), m_Enabled( p.m_Enabled ), m_Fired( p.m_Fired ) {
}

/////////////////////////////////////////////////////////////////////////////
// assignment operator
/////////////////////////////////////////////////////////////////////////////
partition_type& partition_type::operator=( const partition_type& p ) {
  if ( this == &p ) return( *this );
  m_bounds = p.m_bounds;
  m_Enabled = p.m_Enabled;
  m_Fired = p.m_Fired;
  return( *this );
}

/////////////////////////////////////////////////////////////////////////////
// Adds transition_type t to the enabled list.
/////////////////////////////////////////////////////////////////////////////
void partition_type::operator()( const trans_type& t ) {
  m_Enabled.push_front( t );
}

/////////////////////////////////////////////////////////////////////////////
// Addes CPdf* c to the fired list.
/////////////////////////////////////////////////////////////////////////////
void partition_type::operator()( const CPdf* c ) {
  m_Fired.push_front( c );
}

/////////////////////////////////////////////////////////////////////////////
// Replaces the bounds for this parition_type with b.
/////////////////////////////////////////////////////////////////////////////
void partition_type::operator()( const bound_type& b ) {
  m_bounds = b;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a constant reference enabled list.
/////////////////////////////////////////////////////////////////////////////
const trans_list& partition_type::enabled() const {
  return( m_Enabled );
}

/////////////////////////////////////////////////////////////////////////////
// Returns a constant reference to the fired list.
/////////////////////////////////////////////////////////////////////////////
const event_list& partition_type::fired() const {
  return( m_Fired );
}

/////////////////////////////////////////////////////////////////////////////
// Returns a constant reference to the bound_type representing the bounds
// for this paritions.
/////////////////////////////////////////////////////////////////////////////
const bound_type& partition_type::bounds() const {
  return( m_bounds );
}

/////////////////////////////////////////////////////////////////////////////
// Outputs the ascii text representation of the parition_type p to the stream
// s.  The partition is displayed as follow:
// Partition <l,u>
//   E -> ...
//        ...
//   F -> ...
//        ...
/////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const partition_type& p ) {
  s << "Partition " << p.bounds() << endl << "  E-> ";
  for ( trans_list::const_iterator i = p.enabled().begin() ;
	i != p.enabled().end() ; 
	i++ ) {
    if ( i != p.enabled().begin() )
      s << "      ";
    s << "(" << (*i).first << "," << (*(*i).second) << ")";   
    if ( i != p.enabled().end() )
      s << endl;
  }
  s << endl << "  F-> ";
  for ( event_list::const_iterator i = p.fired().begin() ;
	i != p.fired().end() ; 
	i++ ) {
    if ( i != p.fired().begin() )
      s << "      ";
    s << (**i);
    if ( i != p.fired().end() )
      s << endl;
  }
  return( s );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
  
/////////////////////////////////////////////////////////////////////////////
// Functions adjusts the global bounds in this partition to be the <min,max>
// bounds of all of the types contained in this list.
/////////////////////////////////////////////////////////////////////////////
void partition_list::adjust_global_range( const bound_type& b,
					  bool lower_only ) {
//   cout << endl << "b -> " << b;
//   cout << endl << "m_bounds -> " << m_bounds;
  if ( b.lower() < m_bounds.lower() )
    m_bounds.lower( b.lower() );
  if ( !lower_only ) {
    if ( b.upper() < m_bounds.upper())
      m_bounds.upper( b.upper() );
  }
//   cout << endl << "After adjustment: m_bounds -> " << m_bounds << flush;
}

/////////////////////////////////////////////////////////////////////////////
// adjust_partitions divides and adds new parition_types into this partition-
// list depending on the bound_type b.  Complexity is O(n) where n is the
// number of partition_types currently in the list. 
/////////////////////////////////////////////////////////////////////////////
void partition_list::adjust_partitions( bound_type b ) {
  for ( iterator i = m_plist.begin() ; i != m_plist.end() ; i++ ) {
    bound_type tmp = (*i).bounds();
    // The new partion does not intersect partition list, 
    // so add the partion into its correct location and return
    if ( b.lower() < tmp.lower() && b.upper() <= tmp.lower() ) {
      m_plist.insert( i, partition_type( b ) );
      return;
    }
    // The new partion is identical to an existing partition; return
    if ( b.lower() == tmp.lower() && b.upper() == tmp.upper() )
      return;
    // The new partition has a lower bound infront of the current
    // partition being examined.  Insert a new partition to cover the
    // gap.
    if ( b.lower() < tmp.lower() ) { 
      m_plist.insert(i, partition_type(bound_type(b.lower(), tmp.lower())));
      b.lower( tmp.lower() );
    }
    // The new partition has a lower bound between the bounds of the
    // current partition being examined.  Modifiy the current partition
    // bound and a new one to cover the entrance of the new bound.
    if ( b.lower() > tmp.lower() && b.lower() < tmp.upper() ) {
      iterator j = m_plist.insert(i, (*i));
      (*j)( bound_type( tmp.lower(), b.lower() ) );
      (*i)( bound_type( b.lower(), (*i).bounds().upper() ) );
    }
    // The bound ends in the middle of the current parition. 
    // Insert a new partition from this partition's lower bound
    // to b's upper bound and adjust the bounds on the current 
    // partition.
    if ( b.upper() < tmp.upper() ) {
      m_plist.insert( i, partition_type( b, (*i).enabled(), (*i).fired() ) );
      (*i)( bound_type( b.upper(), ((*i).bounds()).upper() ) );
      return;
     }
    // b completely intersects this partition, move its lower bound to this
    // partitions upper bound.
    if ( b.upper() > tmp.upper() && b.lower() < tmp.upper() ) {
       b.lower( tmp.upper() );
    }
  }
  m_plist.insert( m_plist.end(), partition_type( b ) );
}

/////////////////////////////////////////////////////////////////////////////
// Void constructor
/////////////////////////////////////////////////////////////////////////////
partition_list::partition_list()
  : m_bounds(UINT_MAX,UINT_MAX), m_plist() {
}

/////////////////////////////////////////////////////////////////////////////
// Copy constructor
/////////////////////////////////////////////////////////////////////////////
partition_list::partition_list( const partition_list& p )
  : m_bounds( p.m_bounds ), m_plist( p.m_plist ) {
}

/////////////////////////////////////////////////////////////////////////////
// Adds CPdf* into 'this' partition_list
/////////////////////////////////////////////////////////////////////////////
void partition_list::operator()( const CPdf* c ) {
#ifndef __RELEASE
  assert( c != (const CPdf*)NULL );
#endif
  bound_type b( (unsigned int)c->lower(), 
		(unsigned int)c->upper() );
  adjust_global_range( b, true ); // fix the global range if necessary
  adjust_partitions( b );   // Split and add new partitions to cover c.bounds()
  iterator i = m_plist.begin(); 
  while ( (*i).bounds().lower() < b.lower() ) i++;
  while ( i != m_plist.end() ) { // Copy c into the correct partition_types
    if ( (*i).bounds().upper() <= b.upper() )
      (*i++)( c );
    else
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Adds transition_type t into this partition list.
/////////////////////////////////////////////////////////////////////////////
void partition_list::operator()( const trans_type& t ) {
#ifndef __RELEASE
  assert( t.second != (const CPdf*)NULL );
#endif
  bound_type b( (unsigned int)(t.second)->lower(), 
		(unsigned int)(t.second)->upper() );
  adjust_global_range( b ); // account for changes in global range
  adjust_partitions( b );   // Add delete partitions as necessary to cover
  iterator i = m_plist.begin(); // bounds of t.
  while ( (*i).bounds().lower() < b.lower() ) i++;
  while ( i != m_plist.end() ) { // Add t into correct partition_types
    if ( (*i).bounds().upper() <= b.upper() )
      (*i++)( t );
    else
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Returns a const_iterator to the beginning of the partition_type list
////////////////////////////////////////////////////////////////////////////
partition_list::const_iterator partition_list::begin() const {
  return( m_plist.begin() );
}

/////////////////////////////////////////////////////////////////////////////
// Returns a const_iterator to the end of the partition_type list.
/////////////////////////////////////////////////////////////////////////////
partition_list::const_iterator partition_list::end() const {
  return( m_plist.end() );
}

/////////////////////////////////////////////////////////////////////////////
// returns the bounds of the partition
/////////////////////////////////////////////////////////////////////////////
bound_type partition_list::bounds() const {
  return( m_bounds );
}

/////////////////////////////////////////////////////////////////////////////
// Returns a const_iterator to the position in the partition_list that
// is 1 location past the partition that matches the global upper bound.  
// A range from first to bounded_end() covers [first,bounded_end()), meaning
// the bounded_end() is not included in the range.
/////////////////////////////////////////////////////////////////////////////
partition_list::const_iterator partition_list::bounded_end() const {
  for ( const_iterator i = begin() ; i != end() ; i++ ) {
    if ( (*i).bounds().upper() == m_bounds.upper() )
      return( ++i );
  }
  return( end() );
}

//
///////////////////////////////////////////////////////////////////////////////
