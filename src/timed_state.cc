//////////////////////////////////////////////////////////////////////////
// @name timed_state.cc => Code to implement class defined in bap.h
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#include "bap.h"
#include "findrsg.h"
#include "error.h"

//////////////////////////////////////////////////////////////////////////
// NOTE: setting rule=0 indicates that there is no associated rule
// for causality in this state.  If the state is created in this way,
// then rule should not be used!
//////////////////////////////////////////////////////////////////////////
timed_state::timed_state( const tel& t, const untimed_state& untimed_st ) :
  ptr_untimed_st( NULL ), ptr_Me( NULL ), rule( 0 ) {
  ptr_Me = new POSET( untimed_st, t );
  if ( ptr_Me == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr.c_str() ) );
  }
  ptr_untimed_st = new untimed_state( untimed_st );
  if ( ptr_untimed_st == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr.c_str() ) );
  }
}

timed_state::timed_state( const untimed_state& untimed_st,
			  const POSET& Me,
			  const marking_index& rule ) :
  ptr_untimed_st( NULL ), ptr_Me( NULL ), rule( rule ) {
  ptr_Me = new POSET( Me );
  if ( ptr_Me == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr.c_str() ) );
  }
  ptr_untimed_st = new untimed_state( untimed_st );
  if ( ptr_untimed_st == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr.c_str() ) );
  }
}

timed_state::~timed_state() {
  if ( ptr_Me != NULL ) {
    delete ptr_Me;
  }
  if ( ptr_untimed_st != NULL ) {
    delete ptr_untimed_st;
  }
}

const untimed_state& timed_state::get_untimed_st() const {
  if ( ptr_untimed_st == NULL ) {
    string errorStr = "ERROR: attempting to dereference an NULL pointer";
    throw( new error( errorStr.c_str() ) );
  }
  return( *ptr_untimed_st );
}

const POSET& timed_state::get_POSET() const {
  if ( ptr_Me == NULL ) {
    string errorStr = "ERROR: attempting to dereference an NULL pointer";
    throw( new error( errorStr.c_str() ) );
  }
  return( *ptr_Me );
}

bool timed_state::is_consistent( unsigned int check ) {
  if ( ptr_Me == NULL ) return( false );
  return( ptr_Me->is_consistent( check ) );
}

void timed_state::remove_non_causal( const event_set& E ) {
  if ( ptr_Me == NULL ) return;
  ptr_Me->remove_non_causal( E );
}

void timed_state::remove_duplicates() {
  if ( ptr_Me == NULL ) return;
  ptr_Me->remove_duplicates();
}

marking_index timed_state::get_rule() const {
  return( rule );
}

void timed_state::constrain( event_type e, event_type f, 
			     POSET::bound_type v ) {
  ptr_Me->set_max_separation( e, f, v );
  if ( !is_consistent() ) {
    string errorStr = "ERROR: matrix is not consistent in constrain";
    throw( new error( errorStr.c_str() ) );
  }
}

void timed_state::set_order( event_type e, event_type f ) {
  ptr_Me->set_order( e, f );
}

bool timed_state::is_marked( int marking_index ) const {
  if ( ptr_untimed_st == NULL ) {
    string errorStr = "ERROR: null pointer in timed_state::is_marked";
    throw( new error( errorStr.c_str() ) );
  }
  return( ptr_untimed_st->is_marked( marking_index ) );
}

//---------------------------------------------------bool is_level_matched
bool timed_state::is_level_matched( const tel& t        ,
				    const event_type& e   ) const {
  // A pure _dummy_ event is always level matched.
  if ( t.is_dummy_event( e ) ) return( true );
  int s( t.get_signal( e ) );
  return( ( t.is_rising_event( e ) && ptr_untimed_st->is_high( s ) ) ||
	  ( t.is_falling_event( e ) && ptr_untimed_st->is_low( s ) )    );

}
//------------------------------------------------------------------------

ostream& timed_state::dump( ostream& s, const tel& t ) const {
  if ( ptr_untimed_st == NULL || ptr_Me == NULL ) {
    string errorStr = "ERROR: null pointer in timed_state::dump";
    throw( new error( errorStr.c_str() ) );
  }
  s << "s: " << ptr_untimed_st->get_state() << endl;
  s << "Rm: ";
  ::dump( s, t, ptr_untimed_st->get_marking() ) << endl;
  s << "E_not_allowed: ";
  ::dump( s, t, ptr_untimed_st->get_E_not_allowed() ) << endl;
  s << "Me: " << endl;
  ptr_Me->dump( s, t );
  return( s );
}

//
//////////////////////////////////////////////////////////////////////////
