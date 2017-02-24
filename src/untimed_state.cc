//////////////////////////////////////////////////////////////////////////
// @name untimed_state.cc => code to implement class defined in bap.h
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#include "bap.h"
#include "findrsg.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////
//
markingADT get_initial_marking( const tel& t, bool verbose ) {
  markingADT marking = find_initial_marking( t.get_events(), 
					     t.get_rules(), 
					     t.get_markkey(), 
					     t.number_events(), 
					     t.marking_size(), 
					     t.get_ninpsig(),
					     t.get_nsigs(), 
					     t.get_startstate(), 
					     verbose );
  if ( marking == NULL ) {
    throw( new 
	   marking_error( ) );
  }
  return( marking );
}
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
markingADT get_new_marking( const tel& t, 
			    markingADT untimed_st,
			    event_type event,
			    timeoptsADT timeopts,
			    bool noparg ) {
  markingADT marking = NULL;
  if( t.is_dummy_event( event ) ) {
    marking = dummy_find_new_marking( t.get_events(),
				      t.get_rules(),
				      t.get_markkey(),
				      untimed_st,
				      event, 
				      t.number_events(), 
				      t.marking_size(), 
				      0,
				      t.get_nsigs(),
				      t.get_ninpsig(),
				      timeopts.disabling||timeopts.nofail,
				      noparg);
  }
  else{
    marking = find_new_marking( t.get_events(),
				t.get_rules(),
				t.get_markkey(),
				untimed_st, 
				event, 
				t.number_events(),
				t.marking_size(), 
				0, 
				t.get_nsigs(),
				t.get_ninpsig(),
				timeopts.disabling||timeopts.nofail,
				noparg);
  }						      	
  if ( marking == NULL ) {
    throw( new 
	   marking_error( ) );
  }
  return( marking );
}
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
untimed_state::untimed_state( const tel& t, bool verbose ) : 
  ptr_untimed_st( NULL ),
  E_not_allowed( NULL ) {
  // Initialize memory for E_not_allowed
  E_not_allowed = new event_set();
  if ( E_not_allowed == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr.c_str() ) );
  }
  ptr_untimed_st = get_initial_marking( t, verbose );
  ++(ptr_untimed_st->ref_count);
}

untimed_state::untimed_state( const tel& t, 
			      const untimed_state& untimed_st,
			      const event_type& event, 
			      timeoptsADT timeopts,
			      bool noparg ) : 
  ptr_untimed_st( NULL ),
  E_not_allowed( NULL ) {
  ptr_untimed_st = get_new_marking( t, 
				    untimed_st.get_markingADT(),
				    event, 
				    timeopts,
				    noparg );
  ++(ptr_untimed_st->ref_count);
  // Initialize memory for E_not_allowed
  E_not_allowed = new event_set();
  if ( E_not_allowed == NULL ) {
    string errorStr = "ERROR: out of memory";
    throw( new error( errorStr.c_str() ) );
  }
  for ( marking_index i = 0 ; i < (marking_index)t.marking_size() ; ++i ) {
    event_type event = t.enabled( i );
    if ( is_marked( i ) && !untimed_st.is_allowed_to_fire( event ) ) {
      mark_explored( event );
    }
  }

}


untimed_state::untimed_state( const untimed_state& untimed_st ) :
  ptr_untimed_st( untimed_st.ptr_untimed_st ),
  E_not_allowed( untimed_st.E_not_allowed ) {
  if ( ptr_untimed_st != NULL ) {
    ++(ptr_untimed_st->ref_count);
  }
}

untimed_state::~untimed_state() {
  if ( ptr_untimed_st != NULL  ) {
    --(ptr_untimed_st->ref_count); 
    if ( ptr_untimed_st->ref_count == 0 ) {
      delete_marking( ptr_untimed_st );
      free( ptr_untimed_st );
      // Free memory for E_not_allowed
      delete E_not_allowed;
    }
  }
}

markingADT untimed_state::get_markingADT() const {
  return( ptr_untimed_st );
}

bool untimed_state::is_marked( int marking_index ) const {
  return( ptr_untimed_st->marking[marking_index] != 'F' );
}

char* untimed_state::get_state() const {
  return( ptr_untimed_st->state );
}

char* untimed_state::get_marking() const {
  return( ptr_untimed_st->marking );
}

char* untimed_state::get_enablings() const {
  return( ptr_untimed_st->enablings );
}

//------------------------------------------------------is_allowed_to_fire
// is_allowed_to_fire returns ture IFF 'event' is NOT found in the
// E_not_allowed list.  This means that 'event' has not fired in any trace 
// of the exploration, and thus, its interleaving must be explored.
//------------------------------------------------------------------------
bool untimed_state::is_allowed_to_fire( const event_type& event ) const {
  return( E_not_allowed->find( event ) == E_not_allowed->end() );
}

//-----------------------------------------------------------mark_explored
// Adds event to E_not_allowed
//------------------------------------------------------------------------
void untimed_state::mark_explored( const event_type& event ) {
  E_not_allowed->insert( event );
}

//-------------------------------------------------------get_E_not_allowed
// Returns a constant reference to the E_not_allowed event set.
//------------------------------------------------------------------------
const event_set& untimed_state::get_E_not_allowed() const {
  return( *E_not_allowed );
}
//------------------------------------------------------------------------

//-------------------------------------------------------------bool is_low
bool untimed_state::is_low( unsigned i ) const {
  char* this_state( get_state() );
  return( this_state[i] == '0' || this_state[i] == 'R' );

}
//------------------------------------------------------------------------

//------------------------------------------------------------bool is_high
bool untimed_state::is_high( unsigned i ) const {
  char* this_state( get_state() );
  return( this_state[i] == '1' || this_state[i] == 'F' );
}
//------------------------------------------------------------------------

//
//////////////////////////////////////////////////////////////////////////
