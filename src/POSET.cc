//////////////////////////////////////////////////////////////////////////
// @name POSET.cc => Code to implement POSET class defined in bap.h
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iomanip>

#include "bap.h"
#include "error.h"
#include "limits.h"

// ERIC: This does not work either, see pab_c4_2.csp
//#define __TOTAL__

//////////////////////////////////////////////////////////////////////////
//
const POSET::bound_type POSET::_UNBOUNDED_ = INT_MAX - 1;
const POSET::bound_type POSET::_MARKED_ = INT_MAX;

void POSET::floyds(){
  size_type size_dbm = get_size();
  index_type i, j, k;
  for ( k = 0 ; k < size_dbm ; ++k ) {
    for ( i = 0 ; i < size_dbm ; ++i ) {
      for (  j = 0 ; j < size_dbm ; ++j ) {
	if ( dbm[i][k] != _UNBOUNDED_ &&
	     dbm[k][j] != _UNBOUNDED_ &&
	     dbm[i][j] > dbm[i][k] + dbm[k][j] ) 
          dbm[i][j] = dbm[i][k] + dbm[k][j];
      }
    }
  }
}

// NOTE: This can only be called if a single event has been added to the
// POSET.  If more than 1 event has been added since the last 
// canonicalization, then the full floyds must be used.
// NOTE: This assumes that size_dbm-1 is the index of the newly added event.
void POSET::incremental_floyds( size_type size_dbm ) {
  index_type e = size_dbm - 1;
  index_type i, j, k;
  for( k = 0 ; k < size_dbm ; k++ ){
    for( i = 0 ; i < size_dbm ; i++ ){
      if( dbm[i][k] != _UNBOUNDED_ &&
	  dbm[k][e] != _UNBOUNDED_ &&
	  dbm[i][e] > dbm[i][k] + dbm[k][e] ) {
	dbm[i][e]= dbm[i][k] + dbm[k][e];
      }
    }
    for( j = 0 ; j < size_dbm ; j++ ) {
      if( dbm[e][k] != _UNBOUNDED_ &&
	  dbm[k][j] != _UNBOUNDED_ &&
	  dbm[e][j] > dbm[e][k] + dbm[k][j] ){
	dbm[e][j] = dbm[e][k] + dbm[k][j];	
      }
    }
  }
  for( i = 0 ; i < size_dbm ; i++ ){
    for( j = 0 ; j < size_dbm ; j++ ){
      if( dbm[i][e]!=_UNBOUNDED_ &&
	  dbm[e][j]!=_UNBOUNDED_ &&
	  dbm[i][j] > dbm[i][e] + dbm[e][j] ){
	dbm[i][j] = dbm[i][e] + dbm[e][j];	  
      }
    }
  }
}

void POSET::delete_dbm( bound_type** p, size_type size ) {
  if ( p == NULL ) return;
  for ( unsigned int i = 0 ; i < size ; ++i ) {
    delete [] dbm[i];
  }
  delete [] dbm;
}

POSET::bound_type** 
POSET::copy_shrink_dbm( bound_type** p, size_type size_p,
			size_type size_new ) {
  if ( size_new > size_p ) {
    string errorStr = "ERROR: bad size_new in copy_expand_dbm";
    throw( new error( errorStr ) );
  }
  bound_type** tmp = new bound_type*[size_new];
  if ( tmp == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr ) );
  }
  index_type i_p = 0, i_tmp = 0;
  index_type j_p = 0, j_tmp = 0;
  while ( i_p < size_p ) {
    // If this event is not _MARKED_, then this row is included.
    if ( p[i_p][i_p] != _MARKED_ ) {
      tmp[i_tmp] = new bound_type[size_new];
      if ( tmp[i_tmp] == NULL ) {
	string errorStr = "ERROR: out of memory";
	throw( new error( errorStr ) );
      }
      j_p = 0, j_tmp = 0;
      while( j_p < size_p ) {
	// If this event is not _MARKED_, then copy in its column entry.
	if ( p[j_p][j_p] != _MARKED_ ) {
	  tmp[i_tmp][j_tmp++] = p[i_p][j_p];
	}
	++j_p;
      }
      ++i_tmp;
    }
    ++i_p;
  }
  return( tmp );
}

POSET::bound_type** 
POSET::copy_expand_dbm( bound_type** p, size_type size_p,
			size_type size_new ) {
  if ( size_new < size_p ) {
    string errorStr = "ERROR: bad size_new in copy_expand_dbm";
    throw( new error( errorStr ) );
  }
  bound_type** tmp = new bound_type*[size_new];
  if ( tmp == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr ) );
  }
  for ( index_type i = 0 ; i < size_new ; ++i ) {
    tmp[i] = new bound_type[size_new];
    if ( tmp[i] == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
    }
    for ( index_type j = 0 ; j < size_new ; ++j ) {
      if ( j < size_p && i < size_p )
	tmp[i][j] = p[i][j];
      else if ( i == j )
	tmp[i][j] = 0;
      else
	tmp[i][j] = _UNBOUNDED_;
    }
  }
  return( tmp );
}

POSET::bound_type** 
POSET::copy_expand_dbm2( bound_type** p, size_type size_p,
			 size_type size_new ) {
  if ( size_new < size_p ) {
    string errorStr = "ERROR: bad size_new in copy_expand_dbm";
    throw( new error( errorStr ) );
  }
  bound_type** tmp = new bound_type*[size_new];
  if ( tmp == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr ) );
  }
  for ( index_type i = 0 ; i < size_new ; ++i ) {
    tmp[i] = new bound_type[size_new];
    if ( tmp[i] == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
    }
    for ( index_type j = 0 ; j < size_new ; ++j ) {
      if ( j < size_p && i < size_p )
	tmp[i][j] = p[i][j];
      else if ( i == j )
	tmp[i][j] = 0;
      else if ( j >= size_p && i < size_p )
	tmp[i][j] = 0;
      else
	tmp[i][j] = _UNBOUNDED_;
    }
  }
  return( tmp );
}

POSET::bound_type** 
POSET::copy_dbm( bound_type** p, size_type size ) const{
  bound_type** tmp = new bound_type*[size];
  if ( tmp == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  for ( index_type i = 0 ; i < size ; ++i ) {
    tmp[i] = new bound_type[size];
    if ( tmp[i] == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
    }
    for ( index_type j = 0 ; j < size ; ++j ) {
      tmp[i][j] = p[i][j];
    }
  }
  return( tmp );
}

unsigned int POSET::max_length( const tel& t ) const {
  unsigned int max = 0;
  for ( event_map::const_iterator i = ptr_map->begin() ; 
	i != ptr_map->end() ;
	++i ) {
    unsigned int j = strlen( t.get_event( *i ) );
    if ( j > max ) {
      max = j;
    }
  }
  return( max );
}

//////////////////////////////////////////////////////////////////////////
// This function adds events to Ekill if the events are not in E
//////////////////////////////////////////////////////////////////////////
template<class inserter>
void POSET::build_not_in_set( const event_set& E,
			     inserter Ekill ) const {
  typedef event_map::const_iterator iterator;
  for ( iterator i = ptr_map->begin() ; i != ptr_map->end() ; ++i ) {
    if ( E.find( *i ) == E.end() ) {
      *Ekill++ = *i;
    }
  }
}

template<class inserter>
void POSET::build_duplicate_set( inserter Edup ) const {
  typedef event_map::iterator iterator;
  for ( iterator i = ptr_map->begin() ; i != ptr_map->end() ; ++i ) {
    iterator j = i;
    if ( find( ++j, ptr_map->end(), *i ) != ptr_map->end() ) {
      *Edup++ = *i;
    }
  }
}

// ---------------------------------------------POSETS::get_causal_indexes
// ASSUMPTION: index_i is set to the oldest instance of an event; 
// index_j is set to the  most recent instance of an event.
//
// If i==j and there is only a single instance of i in the matrix, then
// get_indexes will return false.  The fact that i==j implies that there
// are 2 instances of the event to be found in the POSET, i will be the
// oldest instance, and j will be the newest instance.
//------------------------------------------------------------------------
bool POSET::get_causal_indexes( index_type& index_i, 
				index_type& index_j,
				const event_type& i, 
				const event_type& j ) const {
  bool set_i = false, set_j = false;
  size_type size_dbm = get_size();
  for ( index_type k = 0 ; k < size_dbm ; ++k ) {
    event_type event = get_event( k );
    // NOTE: I want to match and only match the first i!  Thus,
    // if i is duplicated in the POSET, this with return the
    // the index of the first (oldest) i.
    if ( !set_i && event == i ) {
      index_i = k;
      set_i = true;
      continue;
    }
    // NOTE: If a duplicate j event exists, then I always want
    // to find the most recent j.  Thus, this will match every
    // time event==j, and the last match will be the most recent
    // j in the POSET.
    if ( event == j )  {
      index_j = k;
      set_j = true;
      continue;
    }
  }
  if ( set_i && set_j && index_i != index_j ) return( true );
  return( false );
}
//------------------------------------------------------------------------

//--------------------------------------bool POSET::get_separation_indexes
// ASSUMPTION: I want the most recent instance of i and j.  Thus, this will
// return the index of those most recent entries (i.e., the entries nearest
// to the end of the marix.
//------------------------------------------------------------------------
bool POSET::get_separation_indexes( index_type& index_i, 
				    index_type& index_j,
				    const event_type& i, 
				    const event_type& j ) const {
  bool set_i = false, set_j = false;
  size_type size_dbm = get_size();
  // NOTE: If a duplicate i or j event exists, then I always want
  // to find the most recent i and j.  Thus, this will match every
  // time event==j or event==i, and the last match will be the most recent
  // i or j in the POSET.
  for ( index_type k = 0 ; k < size_dbm ; ++k ) {
    event_type event = get_event( k );
    if ( event == i ) {
      index_i = k;
      set_i = true;
      continue;
    }
    if ( event == j )  {
      index_j = k;
      set_j = true;
      continue;
    }
  }
  if ( set_i && set_j && index_i != index_j ) return( true );
  return( false );
}
//------------------------------------------------------------------------

//------------------------------------------------------get_freshest_index
// Returns the index of the most recent version of event i if multiple
// instances exist in the POSET
//------------------------------------------------------------------------
bool POSET::get_freshest_index( index_type& index_i, 
				const event_type&  i ) const {
  bool set_i = false;
  size_type size_dbm = get_size();
  // NOTE: If a duplicate i or j event exists, then I always want
  // to find the most recent i and j.  Thus, this will match every
  // time event==j or event==i, and the last match will be the most recent
  // i or j in the POSET.
  for ( index_type k = 0 ; k < size_dbm ; ++k ) {
    event_type event = get_event( k );
    if ( event == i ) {
      index_i = k;
      set_i = true;
      continue;
    }
  }
  if ( set_i ) return( true );
  return( false );
};
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Removed events from the index_map that are found in Enotin or Edup.
// NOTE: I only want to remove the first duplicated instance of the 
// event.
//////////////////////////////////////////////////////////////////////////
void POSET::update_and_mark_dbm( event_map* ptr_new_map,
				 const event_set& Enotin,
				 event_set& Edup ) {
  index_type j = 0;
  event_set::iterator pos;
  size_type size_dbm = get_size();
  for ( index_type i = 0 ; i < size_dbm ; ++i ) {
    event_type e = (*ptr_map)[i];
    if ( Enotin.find( e ) != Enotin.end() ) {
      dbm[i][i] = _MARKED_;
    }
    // NOTE: I erase the event from Edup once it is matched
    // so that I only remove the first copy of the event.
    else if ( ( pos = Edup.find( e ) ) != Edup.end() ) {
      dbm[i][i] = _MARKED_;
      Edup.erase( pos );
    }    
    else {
      (*ptr_new_map)[j++] = e;
    }
  }
}

POSET::POSET( const untimed_state& untimed_st, const tel& t ) : 
  ptr_map( NULL ), dbm( NULL ) {
  event_set E;
  for ( int i = 0 ; i < t.marking_size() ; ++i ) {
    int e( t.enabling(i) );
    if ( untimed_st.is_marked( i ) )
      E.insert( e );
    else if ( !(t.is_dummy_event( e ) ) ) {
      int s( t.get_signal( e ) );
      if (s >= 0) {
	if ( ( t.is_rising_event( e ) && untimed_st.is_high( s ) ) ||
	     ( t.is_falling_event( e ) && untimed_st.is_low( s ) ) )
	  E.insert( e );
      }
    }
  }
  //for ( int i = 1 ; i < t.number_events() ; i++ ) {
  //  E.insert( i );
  //}

  ptr_map = new event_map( E.begin(), E.end() );
  if ( ptr_map == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  dbm = new bound_type*[ptr_map->size()];
  if ( dbm == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  for ( index_type i = 0 ; i < ptr_map->size() ; ++i ) {
    dbm[i] = new bound_type[ptr_map->size()];
    if ( dbm[i] == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
    }
    for ( index_type j = 0 ; j < ptr_map->size() ; ++j ) {
      dbm[i][j] = 0;
    }
  }
}

POSET::POSET( const POSET& p ) : ptr_map( NULL ), dbm( NULL ) {
  ptr_map = new event_map( *(p.ptr_map) );
  if ( ptr_map == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  dbm = copy_dbm( p.get_dbm(), get_size() );
}

POSET::~POSET() {
  delete_dbm( dbm, get_size() );
  if ( ptr_map == NULL ) return;
  delete ptr_map;
}

POSET& POSET::operator=( const POSET& p ) {
  if ( this == &p ) return( *this );
  ptr_map = new event_map( *(p.ptr_map) );
  if ( ptr_map == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  if ( get_size() != 0 ) {
    dbm = copy_dbm( p.get_dbm(), get_size() );
  }
  else {
    dbm = NULL;
  }
  return( *this );
}

void POSET::constrain_lower_bounds( const tel& t, const rule_set& Rb, 
				    event_type enabled ) {
  event_type causal = 0;
  bound_type lower_bound = 0;
  for ( rule_set::const_iterator r = Rb.begin(); r != Rb.end() ; ++r ) {
    // NOTE: For levels, this will need to have enabling, as well as
    // causal.  The seperation on every potentially causal event must
    // meet the lower bounds on its associated rule.
    causal = t.enabling( *r );
    lower_bound = (bound_type)t.lower_bound( causal, enabled );
    //cout << "Constrain " << t.get_event(causal) << " -> "
    // << t.get_event(enabled) << " l = " << lower_bound << endl;
    set_min_separation( causal, enabled, lower_bound );
  }
}

void POSET::constrain_lower_bounds( const tel& t, const rule_set& Rb, 
				    event_type enabled,
				    const enabled_set& Een ) {
  event_type causal = 0;
  bound_type lower_bound = 0;
  for ( rule_set::const_iterator r = Rb.begin(); r != Rb.end() ; ++r ) {
    // NOTE: For levels, this will need to have enabling, as well as
    // causal.  The seperation on every potentially causal event must
    // meet the lower bounds on its associated rule.
    causal = t.enabling( *r );
    // Skip over this causal event if it refers to a previous instance of
    // a causal event that is no longer in the POSET, but the next instance
    // is in the enabled set.
    // NOTE: What happens if the old instance is still in the POSET?
    if ( find_if( Een.begin(), Een.end(),
		  bind2nd( compare( t ), causal ) ) != Een.end() ) continue;
    lower_bound = (bound_type)t.lower_bound( causal, enabled );
    set_min_separation( causal, enabled, lower_bound );
  }
}

void POSET::add( const enabled_set& Een, const tel& t ) {
  unsigned int size_new = get_size() + Een.size();
  // ERIC: NOT SURE WHY THIS DOES NOT WORK, SEE circ4.csp for example.
#ifdef __TOTAL__
  bound_type** tmp = copy_expand_dbm2( dbm, get_size(), size_new );
#else
  bound_type** tmp = copy_expand_dbm( dbm, get_size(), size_new );
#endif
  delete_dbm( dbm, get_size() );
  dbm = tmp;
  ptr_map->reserve( size_new );
  for ( enabled_set::const_iterator i = Een.begin() ; 
	i != Een.end() ;
	++i ) {
    // NOTE: order is important here.  The enabled event must be added
    // to the ptr_map before you contrain_lower_bounds on causal events,
    // otherwise the enabled event will not be found in the ptr_map.
    event_type enabled = get_enabled( *i, t );
    ptr_map->push_back( enabled );
    constrain_lower_bounds( t, *i, enabled, Een );
  }
}

void POSET::add( const rule_set& Rb, const tel& t ) {
  size_type old_size_dbm = get_size();
  // Reserve memory for the new entry in the map
  ptr_map->reserve( old_size_dbm + 1 );
  // Get the enabled event and push it into the map
  event_type enabled = get_enabled( Rb, t );
  ptr_map->push_back( enabled );
  // Expand the dbm to accomodate the new entry;
#ifdef __TOTAL__
  bound_type** tmp = copy_expand_dbm2( dbm, old_size_dbm, get_size() );
#else
  bound_type** tmp = copy_expand_dbm( dbm, old_size_dbm, get_size() );
#endif
  delete_dbm( dbm, old_size_dbm );
  dbm = tmp;
  constrain_lower_bounds( t, Rb, enabled );
}

POSET::size_type POSET::get_size() const {
  return( ptr_map->size() );
}

// NOTE: 0 is reserved for system reset, but this assumes that
// reset event will be an enabled event!
POSET::event_type POSET::get_event( index_type i ) const {
  if ( i >= ptr_map->size() ) return( 0 );
  return( (*ptr_map)[i] );
}

void POSET::set_min_separation( event_type e, event_type f, bound_type v ) {
  index_type index_e = 0, index_f = 0;
  if ( !( get_causal_indexes( index_e, index_f, e, f ) ) ) return;
  // INFIN has the same meaning as _UNBOUNED_ in this scope.  
  // However, the value of _UNBOUNDED_ may be different than the value 
  // of INFIN.
  if ( v == INFIN ) {
    v = _UNBOUNDED_;
  }
  // t(e) - t(f) < -v  -> Think in terms of a rule <e,f,l,u>
  dbm[index_e][index_f] = -1 * v;
}

void POSET::set_max_separation( event_type e, event_type f, bound_type v ) {
  index_type index_e = 0, index_f = 0;
  if ( !( get_causal_indexes( index_e, index_f, e, f ) ) ) return;
  // INFIN has the same meaning as _UNBOUNED_ in this scope.  
  // However, the value of _UNBOUNDED_ may be different than the value 
  // of INFIN.
  if ( v == INFIN ) {
    v = _UNBOUNDED_;
  }
  // t(f) - t(e) < v  -> Think in terms of a rule <e,f,l,u>
  dbm[index_f][index_e] =  v;
}

//////////////////////////////////////////////////////////////////////////
// get_value should NOT be called with i == j.  If it is called with 
// i == j, then it will look for 2 instances of the event i to be in 
// the dbm and return that separation.  If only 1 instance of i exists.
// then it will return _UNBOUNDED_
//
// NOTE: The only place that will check a diagonal is is_consistent, and
// then it only checks that it is non-zero!
//////////////////////////////////////////////////////////////////////////
POSET::bound_type 
POSET::get_causal_separation( event_type e, event_type f ) const {
  index_type index_e = 0, index_f = 0;
  if ( !( get_causal_indexes( index_e, index_f, e, f ) ) ) 
    return( _UNBOUNDED_ );
  return( dbm[index_e][index_f] );
}

POSET::bound_type 
POSET::get_separation( event_type i, event_type j ) const {
  index_type index_i = 0, index_j = 0;
  if ( !( get_separation_indexes( index_i, index_j, i, j ) ) ) 
    return( _UNBOUNDED_ );
  return( dbm[index_i][index_j] );
}

POSET::bound_type** POSET::get_dbm() const {
  return( dbm );
}

clockkeyADT POSET::make_clockkey() const {
  if ( get_size() == 0 ) {
    return( NULL );
  }
  size_type size_dbm = get_size();
  clockkeyADT tmp = new clockkey[size_dbm];
  if ( tmp == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  for ( index_type i = 0 ; i < size_dbm ; ++i ) {
    tmp[i].enabled = (*ptr_map)[i];
    tmp[i].enabling = 0;
    tmp[i].eventk_num = 0;
    tmp[i].causal = 0;
    tmp[i].anti = NULL;
  }
  return( tmp );
}

bool POSET::is_consistent( unsigned int added ) {
  index_type size_dbm = get_size();
  index_type start_of_new_events = size_dbm - added;
  // Incrementally recanonicalize the dbm by pretending
  // to add a single event at a time into the dbm.  
  // NOTE: incremental_floyd's assumes that the added event
  // is located at size_dbm-1 and size_dbm is the size of the dbm.
  for ( index_type i = start_of_new_events + 1 ; i <= size_dbm ; ++i ) {
    incremental_floyds( i );
  }
  // If any non-zero values are found on the diagonal, then
  // this dbm is not consistent because this not an assignment
  // of values that will satisfy all of the inequalities 
  // represented by the dbm.
  for ( index_type i = 0 ; i < size_dbm ; ++i ) {
    if ( dbm[i][i] != 0 ) return( false );
  }
  // The value added represents the number of newly added events 
  // before this call to is_consistent.  If after canonicalization
  // the dbm shows that any of these newly added events must 
  // fire BEFORE events already in the dbm, then this dbm
  // is not consistent.  It is not consistent because if an event is
  // already in the dbm, than that event has already have fired!  
  // Thus, a newly added event could not have fired before an event 
  // already in the dbm.
  for ( index_type i = start_of_new_events ; i < size_dbm ; ++i ) {
    for ( index_type j = 0 ; j < start_of_new_events ; ++j ) {
      if ( dbm[i][j] < 0 ) return( false );
    }
  }
  return( true );
}

void POSET::remove_duplicates() {
  if ( ptr_map == NULL || dbm == NULL ) return;
  // Build the "duplicated in index_map" list 
  event_set Edup;
  build_duplicate_set( insert_iterator<event_set>( Edup, Edup.begin() ) );
  // Compute the new size of the POSET and allocate its memory.
  size_type new_size = get_size() - Edup.size();
  //size_type new_size = get_size() - Edup.size();
  if ( new_size == get_size() ) return;
  event_map* ptr_new_map = new event_map( new_size );
  if ( ptr_new_map == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  // Mark events that should be deleted and remove those events
  // from the index map
  event_set Enotin;
  update_and_mark_dbm( ptr_new_map, Enotin, Edup );
  // copy and shrink the dbm according the marked entries from 
  // the update_and_mark_dbm function.
  size_type old_size = get_size();
  bound_type** new_dbm = copy_shrink_dbm( dbm, old_size, new_size );
  // Delete and reassign the dbm and ptr_map
  delete_dbm( dbm, old_size );
  delete ptr_map;
  dbm = new_dbm;
  ptr_map = ptr_new_map;
}

void POSET::remove_non_causal( const event_set& E ) {
  if ( ptr_map == NULL || dbm == NULL ) return;
  // Build the "not in E" list
  event_set Enotin;
  build_not_in_set( E, insert_iterator<event_set>( Enotin, Enotin.begin() ) );
  // Build the "duplicated in index_map" list 
  event_set Edup;
  build_duplicate_set( insert_iterator<event_set>( Edup, Edup.begin() ) );
  // Compute the new size of the POSET and allocate its memory.
  size_type new_size = get_size() - Enotin.size() - Edup.size();
  //size_type new_size = get_size() - Edup.size();
  if ( new_size == get_size() ) return;
  event_map* ptr_new_map = new event_map( new_size );
  if ( ptr_new_map == NULL ) {
      string errorStr = "ERROR: out of memory";
      throw( new error( errorStr ) );
  }
  // Mark events that should be deleted and remove those events
  // from the index mar
  update_and_mark_dbm( ptr_new_map, Enotin, Edup );
  // copy and shrink the dbm according the marked entries from 
  // the update_and_mark_dbm function.
  size_type old_size = get_size();
  bound_type** new_dbm = copy_shrink_dbm( dbm, old_size, new_size );
  // Delete and reassign the dbm and ptr_map
  delete_dbm( dbm, old_size );
  delete ptr_map;
  dbm = new_dbm;
  ptr_map = ptr_new_map;
}

void POSET::clear_separations_on_rows( unsigned int added ) {
  index_type size_dbm = get_size();
  index_type start_of_new_events = size_dbm - added;
  for ( index_type i = start_of_new_events ; i < size_dbm ; ++i ) {
    for ( index_type j = 0 ; j < size_dbm ; ++j ) {
      if ( i == j ) continue;
      dbm[i][j] = _UNBOUNDED_;
    }
  }
}

//---------------------------------------------------void POSET::set_order
// Adds a relation to the POSET constraining e to always fire before f
// (i.e., dbm[e][f] = 0 or t(e) - t(f) <= 0).
//------------------------------------------------------------------------
void POSET::set_order( event_type e, event_type f ) {
  index_type index_e = 0, index_f = 0;
  if ( !( get_separation_indexes( index_e, index_f, e, f ) ) ) return;
  // t(e) - t(f) < v  -> Think in terms of a rule <e,f,l,u>
  // MEANING: event e will always happen before event f
  if (dbm[index_e][index_f] > 0)
    dbm[index_e][index_f] = 0;
}
//------------------------------------------------------------------------

//------------------------------------------bound_type POSET::get_max_over
// Returns the maximum allowed separation between e and any events
// in the POSET from the index 'added' to the end of the POSET.
//------------------------------------------------------------------------
POSET::bound_type POSET::get_max_over( event_type e, 
				       unsigned int added ) {
  index_type index_e = 0;
  if ( !( get_freshest_index( index_e, e ) ) ) {
    return( _UNBOUNDED_ );
  }
  index_type size_dbm = get_size();
  index_type start_of_new_events = size_dbm - added;
  POSET::bound_type max_bound = 0;
  for ( index_type i = start_of_new_events ; i < size_dbm ; ++i ) {
    bound_type tmp = dbm[i][index_e];
    if ( tmp > max_bound ) {
      max_bound = tmp;
    }
  }
  return( max_bound );
}
//------------------------------------------------------------------------

//--------------------------------------------------------bool is_in_POSET
bool POSET::is_in_POSET( event_type e ) const {
  size_type size_dbm = get_size();
  for ( index_type k = 0 ; k < size_dbm ; ++k )
    if ( e == get_event( k ) ) 
      return( true );
  return( false );
}
//------------------------------------------------------------------------

//--------------------------------------------------------bool is_in_POSET
bool POSET::is_in_POSET( event_type e, unsigned added ) const {
  size_type size_dbm = get_size() - added;
  for ( index_type k = 0 ; k < size_dbm ; ++k )
    if ( e == get_event( k ) ) 
      return( true );
  return( false );
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
ostream& POSET::dump( ostream& s, const tel& t ) const {
  if ( ptr_map->size() == 0 ) {
    return( s );
  }
  size_type w = max_length( t ) + 1;
  // Output the column labels
  s << setw( w+1 ) << " "; 
  for ( event_map::const_iterator i = ptr_map->begin() ; 
	i != ptr_map->end() ; 
	++i ) {
    s << setw( w ) << t.get_event( *i );
  }
  s << endl << setw( w+1 ) << " ";
  for ( index_type i = 0 ; i < ptr_map->size() ; ++i ) {
    s << setw( w ) << setfill( '-' ) << "-" << setfill( ' ' );
  }
  // Print the matrix with Row identifiers.
  s << endl;
  event_map::const_iterator name = ptr_map->begin();
  for ( index_type i = 0 ; i < ptr_map->size() ; ++i, ++name ) {
    if ( i ) s << endl;
    s << setw( w ) << t.get_event( *name ) << "|";
    for ( index_type j = 0 ; j < ptr_map->size() ; ++j ) {
      if ( dbm[i][j] == _UNBOUNDED_ ) {
	s << setw( w ) << "U";
      }
      else {
	s << setw( w ) << dbm[i][j];
      }
    }
  }
  s << endl;
  return( s );
}
//------------------------------------------------------------------------

//
//////////////////////////////////////////////////////////////////////////
