//////////////////////////////////////////////////////////////////////////
// @name Bourne Again POSET timing analysis
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <limits.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

#include "bap.h"
#include "bag.h"
#include "error.h"
#include "common_timing.h"
#include "po_reduce.h"
#include "level_expand.h"

//#define __DUMP__
//#define __DUMP_STATE__

//  extern unsigned non_causal;
//  extern unsigned total_calls;

//------------------------------------------------------------------------
// _UNSET_ is used by combinatorial search to denote the number of events 
// that can never fire first is not yet set to a valid value.
#define _UNSET_          -1

#ifdef __PROFILE__
// These are used with combinatorial_profiler to track how
// many calls to inconsistent fails.
unsigned int inconsistent = 0;
unsigned int inconsistent_checks = 0;

//////////////////////////////////////////////////////////////////////////
// This class simply tracks the number of failed is_consistent calls
// in combinatorial_search.  It works in tandem with the 2 following
// global variables:
//
// inconsistent
// inconsistent_checks
//
// Call report to dump statistics.
//////////////////////////////////////////////////////////////////////////
class combinatorial_profiler{
protected:

  unsigned int max_failure_in_one_call;
  unsigned int accompanying_checks;
  unsigned int max_checks_in_one_call;
  unsigned int accompanying_failures;
  unsigned int total_failures;
  unsigned int total_calls_comb_search;
  unsigned int total_calls_comb_search_with_failures;
  unsigned int total_checks;

public:

  combinatorial_profiler() :
    max_failure_in_one_call(0),
    accompanying_checks(0),
    max_checks_in_one_call(0),
    accompanying_failures(0),
    total_failures(0),
    total_calls_comb_search(0),
    total_calls_comb_search_with_failures(0),
    total_checks(0) {
    // NOTHING TO DO
  }

  void update() {
    ++total_calls_comb_search;
    total_checks += inconsistent_checks;
    if ( inconsistent != 0 ) {
      ++total_calls_comb_search_with_failures;
      total_failures += inconsistent;
    }
    if ( inconsistent > max_failure_in_one_call ) {
      max_failure_in_one_call = inconsistent;
      accompanying_checks = inconsistent_checks;
    }
    if ( inconsistent_checks > max_checks_in_one_call ) {
      max_checks_in_one_call = inconsistent_checks;
      accompanying_failures = inconsistent;
    }
    inconsistent = 0;           // Reset the global variables
    inconsistent_checks = 0;
  }

  void report( char* fname ) {
    cout << "------------------------------------------------------------" 
	 << endl;
    cout << "             | Max Fails |          | Max Checks |         |" 
	 << endl;
    cout << "     Name    | One Call  |  Checks  |  One Call  |  Fails  |" 
	 << endl;
    cout << "------------------------------------------------------------" 
	 << endl;
    printf(" %11s | %9d | %8d | %10d | %7d |\n", 
	   fname,
	   max_failure_in_one_call,
	   accompanying_checks,
	   max_checks_in_one_call,
	   accompanying_failures );
    cout << "------------------------------------------------------------" 
	 << endl;
    cout << "------------------------------------------------------------"
	 << "-----------------------" << endl;
    cout << "             |   Total  |  Total   |   Total   |    %    |"
	 << " Fails per | Checks per |" << endl;
    cout << "     Name    |   Fails  |  Checks  |   Calls   |  Fails  |"
	 << "   call    |    call    |" << endl;
    cout << "------------------------------------------------------------"
	 << "-----------------------" << endl;
    printf(" %11s | %8d | %8d | %9d | %7.3g | %9.3g | %10.3g |\n",
	   fname,
	   total_failures,
	   total_checks,
	   total_calls_comb_search,
	   total_failures / (double)total_checks,
	   total_failures / (double)total_calls_comb_search,
	   total_checks / (double)total_calls_comb_search );
    cout << "------------------------------------------------------------"
	 << "-----------------------" << endl;

  }
};
//////////////////////////////////////////////////////////////////////////
#endif

//////////////////////////////////////////////////////////////////////////
//
class bap_stack_entry {
public:
  typedef _event_list* event_list_ptr;
protected:
  timed_state* ptr_timed_st;
  event_set* ptr_Ef;
  // This records if the current stack frame has unresolved TIC issues
  bool choice_clean;
  event_list_ptr ptr_Een;

public:

  bap_stack_entry( timed_state *ptr_timed_st , 
		   event_set *ptr_Ef         ,
		   const event_set& Een      ,
		   bool resolved               ) :
    ptr_timed_st( ptr_timed_st )  , 
    ptr_Ef( ptr_Ef )              , 
    choice_clean( resolved )      ,
    ptr_Een( new _event_list() )  {
    if ( ptr_timed_st == NULL || ptr_Ef == NULL ) {
      throw( new error( "ERROR: null stack entry" ) );
    }
    std::copy( Een.begin(), Een.end(),
	       back_insert_iterator<_event_list>( *ptr_Een ) );
  }

  bap_stack_entry( timed_state* ptr_timed_st    ,
		   event_set* ptr_Ef            ,
		   const _event_list& Een_prev ,
		   event_set  Een_new           ,
		   bool resolved                  ) :
    ptr_timed_st( ptr_timed_st )  , 
    ptr_Ef( ptr_Ef )              , 
    choice_clean( resolved )      ,
    ptr_Een( new _event_list() )   {
    _event_list::const_iterator i ( Een_prev.begin() );
    while ( i != Een_prev.end() ) {
      const event_type& e( *i++ );
      event_set::iterator j( Een_new.find( e ) );
      if ( j == Een_new.end() ) continue;
      ptr_Een->push_back( e );
      Een_new.erase( j );
    }
    copy( Een_new.begin(), Een_new.end()                 ,
	  back_insert_iterator<_event_list>( *ptr_Een )   );
   }

  ~bap_stack_entry() {
    if ( ptr_Ef != NULL )
      delete ptr_Ef;
    if ( ptr_timed_st != NULL )
      delete ptr_timed_st;
    if ( ptr_Een != NULL )
      delete ptr_Een;
  }

  timed_state* get_timed_st() const {
    return( ptr_timed_st );
  }
  
  event_type get_event() {
    if ( ptr_Ef->empty() ) {
      throw( 
      new error( "ERROR: empty ptr_Ef in bap_stack_entry::get_event()" ) );
    }
    event_type event = *(ptr_Ef->begin());
    ptr_Ef->erase( ptr_Ef->begin() );
    return( event );
  }

  template <class inserter>
  void build_age_order( event_set Ef , 
			inserter ins   ) const {
    _event_list::const_iterator i( ptr_Een->begin() );
    while ( i != ptr_Een->end() ) {
      const event_type& e( *i++ );
      event_set::iterator j( Ef.find( e ) );
      if ( j == Ef.end() )continue;
      *ins++ = e;
      Ef.erase( j );
    }
    copy( Ef.begin(), Ef.end(), ins );
  }

  const _event_list& get_Een() const {
    return( *ptr_Een );
  }

  bool empty() const {
    return( ptr_Ef->empty() );
  }
  
  bool is_choice_clean() const {
    return( choice_clean );
  }

};
//
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
//This defines the stack that is used in the bap_rsg timing analysis.
typedef list<bap_stack_entry*> bap_stack;
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//
timed_state* front( bap_stack& the_stack, 
		    event_type& f,
		    bool& choice_resolved ) {
  if ( the_stack.empty() ) {
    throw( new error( "ERROR: empty stack in front" ) );
  }
  bap_stack_entry* current = the_stack.front();
  f = current->get_event();
  // Set the conflict status of the frame
  choice_resolved = current->is_choice_clean();
  return( current->get_timed_st() );
}

timed_state* front( bap_stack& the_stack ) {
  if ( the_stack.empty() ) {
    throw( new error( "ERROR: empty stack in front" ) );
  }
  bap_stack_entry* current = the_stack.front();
  return( current->get_timed_st() );
}

//
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
template <class inserter>
void build_age_order( const bap_stack& the_stack ,
		      const event_set& Ef        ,
		      inserter ins                 ) {
  const bap_stack_entry* current( the_stack.front() );
  current->build_age_order( Ef, ins );
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
const _event_list& get_Een( const bap_stack& the_stack ) {
  const bap_stack_entry* current( the_stack.front() );
  return( current->get_Een() );
}
//------------------------------------------------------------------------
				    
//////////////////////////////////////////////////////////////////////////
void pop( bap_stack& the_stack ) {
  bap_stack_entry* current = the_stack.front();
  if ( current->empty() ) {
    the_stack.pop_front();
    delete current;
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void clean_stack( bap_stack& the_stack ) {
  while ( !(the_stack.empty()) ) {
    bap_stack_entry* front = the_stack.front();
    the_stack.pop_front();
    delete front;
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 
ostream& dump( ostream& s, const tel& t, const rule_set& Rb ) {
  for( rule_set::const_iterator j = Rb.begin() ; j != Rb.end() ; ){
    if ( j == Rb.begin() ) {
      s << t.get_event( t.enabled( *j ) ) << ": {";
    }
    s << t.get_event( t.enabling( *j ) );
    if ( ++j != Rb.end() ) {
      s << ",";
    }
    else {
      s << "}";
    }
  }
  return( s );
}

ostream& dump(  ostream& s, const tel& t, const enabled_set& Een ){
  for( enabled_set::const_iterator i = Een.begin() ; i != Een.end() ; ){
    dump( s, t, *i );
    if ( ++i != Een.end() )
      s << endl;
  }
  return( s );
}

//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
event_type get_enabled( const rule_set& R, const tel& t ) {
  return( t.enabled( *(R.begin()) ) );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// To be enabled, a rule must be marked and have its level expression 
// evaluate to true in the current state
//////////////////////////////////////////////////////////////////////////
bool has_satisfied_expression( const int& enabling,
			       const int& enabled,
			       const untimed_state& untimed_st,
			       const tel& t ) {
  return( level_satisfied( t.get_rules(),
			   untimed_st.get_markingADT(), 
			   t.get_nsigs(), 
			   enabling,
			   enabled ) );
}
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
template <class inserter>
bool is_enabled( int event, 
		 const untimed_state& untimed_st, 
		 const tel& t,
		 inserter Re ) {
  // If an event is fictitious, then it has NO behavior rules.  This
  // flag denotes that situation.
  bool behavior_rule_exists = false;
  // foreach behavior rule that enables the event (ignore reset: i=0 )
  for ( int i = 1 ; i < t.number_events() ; ++i ) {
    if ( t.is_behavior_rule( i, event ) ) {
      behavior_rule_exists = true;
      // Is this rule not enabled?
      int enabling_rule = t.marking_index( i, event );
      if ( !( untimed_st.is_marked( enabling_rule ) &&
	      has_satisfied_expression( i, event, untimed_st, t ) ) ) {
	bool another_rule = false;
	// Is there another rule that is enabled and does the enabling 
        // event for that rule conflict with i? 
	for ( int j = 1 ; j < t.number_events() ; ++j ) {
	  if ( t.is_behavior_rule( j, event ) && 
	       t.is_input_conflict( j, i ) ) {
	    enabling_rule = t.marking_index( j, event );
	    if ( untimed_st.is_marked( enabling_rule ) &&
		 has_satisfied_expression( j, event, untimed_st, t ) ) {
	      another_rule = true;
	      break;
	    }    
	  }
	}
	// Another rule that has an enabling conflict with <i,event> 
	// was not found, so this event does NOT have enough enabled 
	// rules to allow it to fire.
	if ( !another_rule ) { return( false ); }
      }
      else {
	// This rule is enabled (marked and level expression is true)
	// so save the rule into Re.
	*Re++ = enabling_rule;
      }
    }
  }
  // All rules enabling this event are active or fired, so this event 
  // could fire now or in the future.
  return( behavior_rule_exists );
}
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
void get_rule_set( const event_type& f             ,
		   const untimed_state& untimed_st , 
		   const tel& t                    ,
		   rule_set& Re                      ) {
  bool enabled( is_enabled( f, untimed_st, t, 
			    rule_set_inserter( Re ) ) );
  if ( enabled ) return;
  Re.clear();
  enabled = is_non_disabling( f, untimed_st, t, 
			      rule_set_inserter( Re ) );
  if( !enabled ){
    throw( new error( "ERROR: Firing an event that is not enabled" ) );
  }
}
//
//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------bool get_enabled_events
template <class inserter>
bool get_enabled_events( const untimed_state& untimed_st , 
			 const tel& t                    ,
			 inserter Een                    ,
			 bool immediate_check              ) {
  event_set Ev;
  bool enabled_event = false;
  // NOTE: I am computing the enabled set this way so I only consider
  // things that at least have a marked satisfied rule.  This is faster
  // than just checking if each event is enabled, even if is_enabled 
  // duplicates some of the checks.
  for ( marking_index i = 0 ; i < (marking_index)t.marking_size() ; ++i ) {
    event_type e = t.enabling( i );
    event_type f = t.enabled( i );
    if ( t.is_immediate( f ) != immediate_check ) continue;
    if ( untimed_st.is_marked( i )                       &&
	 t.is_behavior_rule( e, f )                      &&
	 has_satisfied_expression( e, f, untimed_st, t )    ) {
      if ( Ev.find( f ) != Ev.end() ) continue;
      Ev.insert( f );
      // NOTE: This may need to be optimized to not do the wasted copy!
      rule_set Re;
      // Add the rules_set Re to the enabled_set Een IFF there are
      // enough marked rules so that f is untimed enabled AND
      // f is allowed to fire from this untimed state.
      // This would not be the case if the firing of f has been explored
      // in an earlier firing sequence, and it conflicts with another
      // untimed enabled event.
      if ( is_enabled( f, untimed_st,
		       t, front_insert_iterator<rule_set>( Re ) ) &&
	   untimed_st.is_allowed_to_fire( f )                        ){
	*Een++ = Re;
	enabled_event = true;
      }
    }
  }
  return( enabled_event );
}
//------------------------------------------------------------------------

//-------------------------------------------------void get_enabled_events
template <class inserter>
void get_enabled_events( const untimed_state& untimed_st , 
			 const tel& t                    ,
			 inserter Een                      ) {
  if ( get_enabled_events( untimed_st, t, Een, true ) ) return;
  get_enabled_events( untimed_st, t, Een, false );
}
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
template <class container>
void get_fire_first_events( const enabled_set& Een, 
			    const tel& t,
			    const POSET& Me, 
			    container& Ef,
			    container& Enf ) {
#ifdef __DUMP__
  Me.dump( cout, t ) << endl;
#endif
  typedef enabled_set::const_iterator const_iter;
  for ( const_iter i = Een.begin() ; i != Een.end() ; ++i ) {
    event_type ei = get_enabled( *i, t );
    bool fireable = true;
    for ( const_iter j = Een.begin() ; j != Een.end() ; ++j ) {
      event_type ej = get_enabled( *j, t );
      // t(ej) - t(ei) <= 0 indicates that ei must always fire
      // after ej, and is thus, not fireable first unless the 2
      // events conflict, then choice is resolved independent of time.
      if ( ( ei != ej )                 &&
	   ( Me.get_separation( ej, ei ) < 0 ) ) {
	fireable = false;
	break;
      }
    }
    if ( fireable ) {
      Ef.insert( ei );
    }
    else {
      Enf.insert( ei );
    }
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// remove_over_bounded will remove from the rule_set Rb any rules that are
// over bounded by other rules.  A rule is overbounded if there exists
// a rule with U for an upper bound. 
// NOTE: This function changes the order of entries in Rb putting those
// entries that can be removed at the end of Rb.  It returns the location
// of the new end.  If you want to delete the over bounded entries, then
// use the following:
//
//        Rb.erase( remove_over_bounded( Rb, t ), Rb.end() );
//
// Otherwise, just set your loop to break when
//
//        i == remove_over_bounded( Rb, t )
//
// This will save the time of actually deleting the over bounded 
// entries.
//////////////////////////////////////////////////////////////////////////
rule_set::iterator remove_over_bounded( rule_set& Rb    , 
					const POSET& Me ,
					const tel& t      ) {
  typedef rule_set::iterator iterator;
  iterator first = Rb.begin();
  // Does a rule with with an unbounded upper limit exist in this set?
  // If it does, then save its marking index.
  for ( iterator i = first ; i != Rb.end() ; ++i ) {
    event_type enabling = t.enabling( *i );
    event_type enabled = t.enabled( *i );
    if ( t.is_unbounded( enabling, enabled ) && 
	 Me.is_in_POSET( enabling )             ) {
      iter_swap( i, first++ );
      return( first );
    }
  }
  return( Rb.end() );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
bool can_never_fire_first( event_type event,
			   const enabled_set& Een, 
			   const tel& t,
			   const POSET& Me ) {
  typedef enabled_set::const_iterator enabled_iter;
  typedef rule_set::const_iterator rule_iter;
  for ( enabled_iter i = Een.begin() ; i != Een.end() ; ++i ) {
    bool all_rules_expired = true;
    for ( rule_iter j = (*i).begin() ; j != (*i).end() ; ++j ) {
      event_type e = t.enabling( *j );
      if ( e == event ) continue;
      if ( !Me.is_in_POSET( e ) ) continue;
      event_type f = t.enabled( *j );
      if ( abs( Me.get_causal_separation( e, f ) ) <= t.upper_bound( e, f ) ) {
	all_rules_expired = false;
	break;
      }
    }
    // If there exists an f that has all of its rules expired, then
    // this 'event' can never fire before f.
    if ( all_rules_expired ) return( true );
  }
  return( false );
}                  
//////////////////////////////////////////////////////////////////////////
 
//////////////////////////////////////////////////////////////////////////
unsigned int count_non_fire_first_events( const enabled_set& Een,
					  const tel& t, 
					  POSET Me,
					  const event_set& Enf ) {
  typedef enabled_set::const_iterator enabled_iter;
  typedef rule_set::const_iterator rule_iter;
  typedef event_set::const_iterator event_iter;
  unsigned int kill_count = 0;
  Me.clear_separations_on_rows( Een.size() );
  // Loop through each event in Enf and check to see if it is
  // ever possible for this event to fire first.
  for ( event_iter i = Enf.begin() ; i != Enf.end() ; ++i ) {
    event_type ei = *i;
    POSET Mt( Me );
    // Set the separation in the POSET to constrain ei to fire first
    for ( enabled_iter j = Een.begin() ; j != Een.end() ; ++j ) {
      rule_iter rule_j = (*j).begin();
      event_type ej = t.enabled( *rule_j );
      Mt.set_min_separation( ei, ej, 0 );
    }
    // If this the POSET Mt is consistent and this event can never
    // fire first, then delete it from Enf.
    if ( Mt.is_consistent( Een.size() ) &&
	 can_never_fire_first( ei, Een, t, Mt ) ) {
      ++kill_count;
    }
  }
  return( kill_count );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// This searches all possible causal configurations for the events in Een
// and generates a POSET according to each configuration.  From these 
// POSETs, a set of enabled events that can fire before other enabled
// events is constructed.  This is a complete search and has a factorial
// cost.  If the size of Ef equals the size of Een, then all enabled
// events can fire concurrently.  At this point it is safe to exist the
// combinatorial search.
//////////////////////////////////////////////////////////////////////////
bool combinatorial_search( enabled_set::iterator current, 
			   enabled_set& Een,
			   const tel& t, 
			   POSET& Me,
			   event_set& Ef,
			   int& cannot_fire ) {
  typedef rule_set::iterator rule_iter;
  typedef enabled_set::iterator enabled_iter;
  if ( current != Een.end() ) {
    rule_iter end = remove_over_bounded( *current, Me, t );
    for ( rule_iter i = (*current).begin() ; i != end ; ++i ) {
      //if ( !is_causal_candidate( t, *current, Me, i ) ) continue;
      event_type e = t.enabling( *i );
      event_type f = t.enabled( *i );
      if ( !Me.is_in_POSET( e, Een.size() ) ) continue;
#ifdef __DUMP__
      cout << "Setting causal assignment: (" 
	   << t.get_event( e )  << "," << t.get_event( f ) 
	   << ") -> " << t.upper_bound( e, f ) << endl;
#endif
      Me.set_max_separation( e, f, t.upper_bound( e, f ) );
      enabled_iter next = current;
      if ( combinatorial_search( ++next, Een, t, Me, Ef, cannot_fire ) ) {
	return( true );
      }
      Me.set_max_separation( e, f, POSET::_UNBOUNDED_ );
    }
  }
  else if ( current == Een.end() ) {
    POSET Mt( Me );
    event_set Enf;
#ifdef __PROFILE__
    ++inconsistent_checks;
#endif
#ifdef __DUMP__
    cout << endl << "Checking causal configuration -> " << endl;
#endif
    // is_consistent must know how many events we have just
    // added.  If any of these events must fire before those
    // already in the POSET, then this POSET is not consistent.
    if ( Mt.is_consistent( Een.size() ) ) {
      get_fire_first_events( Een, t, Mt, Ef, Enf );
#ifdef __DUMP__
      cout << "Ef: ";
      dump( cout, t, Ef ) << endl << endl;
#endif
      if ( cannot_fire == _UNSET_ && Enf.size() != 0 ) {
	cannot_fire = count_non_fire_first_events( Een, t, Me, Enf );
      }
      if ( Enf.size() == 0 || (int)Enf.size() == cannot_fire ) {
	return( true );
      }
    }
#if defined( __DUMP__ ) || defined( __PROFILE__ )
    else {
#ifdef __DUMP__
      cout << endl << "     INCONSISTENT    " << endl << endl;
#endif
#ifdef __PROFILE__
      ++inconsistent;
#endif
    }
#endif
  }
  return( false );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void get_fireable_set( enabled_set& Een                       ,
		       const timed_state& sc                  ,
		       const enabled_set_expander& Een_expand ,
		       const tel& t                           ,
		       event_set& Ef                           ) {
  POSET Me( sc.get_POSET() );
  // I may want to combine this operation into a single constructor
  // to save the burden of the extra copy on the POSET.
  //  POSET Me( Mt );
  Me.add( Een, t );
  Een_expand.set_causal_orders( t, Me );
#ifdef __DUMP__
  cout << endl << "Computing fireable events from -> " << endl;
  Me.dump( cout, t ) << endl;
#endif
  int cannot_fire = _UNSET_;
  combinatorial_search( Een.begin(), Een, t, Me, Ef, cannot_fire );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template <class inserter>
void get_enabling_events( const tel& t, 
			  const untimed_state& untimed_st,
			  inserter E ){
  for ( index_type i = 0 ; i < (index_type)t.marking_size() ; i++ ) {
    if ( untimed_st.is_marked( i ) ) {
      *E++ =  t.enabling( i );
    }
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template <class container>
void assert_equal( const container& a,
		   const string& a_name,
		   const container& b,
		   const string& b_name,
		   const tel& t ) {
  if ( a.size() != b.size() ||
       !equal( a.begin(), a.end(), b.begin() ) ) {
    cout << endl << "ERROR: containers do not match";
    cout << endl << a_name;
    dump( cout, t, a ) << endl;
    cout << b_name;
    dump( cout, t, b ) << endl << endl;
    throw( new error( "ERROR: containers do not match" ) );
  }
}
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------is_TIC_candidate
bool is_TIC_candidate( const event_type& event, 
		       const event_type& candidate, 
		       const tel& t ) {
  // If the 2 events do not conflict, then conflict doesn't apply.
  if ( !( t.is_output_conflict( event, candidate ) ) ) {
    return( false );
  }
  // If 'event' and 'candidate' do not share a common enabling, then 
  // 'candidate' can not be an event that needs to be addressed by 
  // the new conflict semantics
  event_set E;
  t.common_enabling(event,candidate,insert_iterator<event_set>(E,E.begin()));
  if ( E.size() == 0 ){
    return( false );
  }
  // All conditions are satisfied.  This implies that the event 
  // 'candidate' must be handled elsewhere for TIC semantics.
  return( true );
}
//------------------------------------------------------------------------

//---------------------------------------------------------is_TIC_resolved
// Returns true IFF any of the following conditions hold:
//   1) Fireable set (Ef) is the same size as the enabled set (Een);
//   2) There is no conflict found in the set of enabled events that
//      that meets the is_TIC_candidate conditions and is not
//      found in the Ef set.
//------------------------------------------------------------------------
bool is_TIC_resolved( const enabled_set& Een,
		      const event_set& Ef,
		      const tel& t ) {
  typedef enabled_set::const_iterator const_iter;
  // The fireable set and untimed enabled set are the same size so
  // if any conflict exists, it will be resolved without further 
  // intervention.
  if ( Ef.size() == Een.size() ) return( true );
  for ( const_iter i = Een.begin() ; i != Een.end() ; ++i ) {
    event_type ei = get_enabled( *i, t );
    // NOTE: I can start at the next location because conflict is a 
    // reflexive relationship.
    const_iter j = i;
    while ( ++j != Een.end() ) {
      event_type ej = get_enabled( *j, t );
      // If ei and ej meet the is_TIC_candidate criteria, so 
      // so this situation will have to be resolved in the
      // firing trace.
      if ( is_TIC_candidate( ei, ej, t )                              &&
	   ( Ef.find( ei ) == Ef.end() || Ef.find( ej ) == Ef.end() )    ) {
#ifdef __DUMP__
	cout << "__NOT_TIC_RESOLVED_BY_DEFAULT__ -> ";
	cout << t.get_event( ei ) << " # " << t.get_event( ej ) << endl;
#endif
	return( false );
      }
    }
  }
  return( true );
}
//------------------------------------------------------------------------

//---------------------------------------------------------------get_event
struct get_event {

  // Required for bind2nd stl interface.
  typedef rule_set first_argument_type;
  // NOTE: I use a pointer here to prevent the copy performed by 
  // bind2nd in its usage with transform
  typedef tel* second_argument_type;
  typedef event_type result_type;

  result_type operator()( const rule_set& e, 
			  const tel* t ) const {
    return( t->enabled( *(e.begin()) ) ); 
  }

};
//------------------------------------------------------------------------

//------------------------------------------------------resolve_TIC_choice
// This function will push new frames on the stack to reflect different
// choice resolutions.  Currently, it is in a suboptimal form.  This
// is because it potentially pushes frames that are not needed.  To
// optimize, look at is_TIC_candidate.  A push only needs to happen
// if all conflicting events are not allowed to fire concurrently from
// the given timed_state due to time bounds.  If all conflicting events
// are a part of Ef in the original stack frame, then another frame is
// NOT required!
//------------------------------------------------------------------------
void resolve_TIC_choice( const event_type& event     ,
			 const timed_state& timed_st ,
			 timed_state_space* ptr_tss  ,
			 const tel& t                ,
			 timeoptsADT opts            ,		      
			 bap_stack& the_stack          ) {
  untimed_state su = timed_st.get_untimed_st();
  // foreach event that conflicts with this event:
  // Ignoring reset event 0
  for ( event_type i = 1 ; i < (event_type)t.number_events() ; ++i ) {
    // This is not used here, but is required by the interface 
    // on is_enabled.
    rule_set Re;  
    // Is this a potential conflict candidate and is it enabled?
    if ( is_TIC_candidate( event, i, t )                       &&
	 su.is_allowed_to_fire( i )                                 &&
	 is_enabled( i, su, t, front_insert_iterator<rule_set>(Re) ) ) {
      // 1) Mark this event as being explored (i.e., add it to
      //    E_not_allowed in the untimed state );
      su.mark_explored( event );
     
      // 2) Get the set of untimed enabled events from untimed_st;
      enabled_set Een_Rb;
      get_enabled_events( su, t, 
			  front_insert_iterator<enabled_set>( Een_Rb ) );
      event_set Een;
      transform( Een_Rb.begin(), Een_Rb.end() , 
		 inserter( Een, Een.begin() ) , 
		 bind2nd( get_event(), &t )     );
      enabled_set_expander Een_expand( t, su, Een_Rb );

      // 3) Get the set of firable events (i.e., concurrently time
      //    enabled events;
      event_set* ptr_Ef = new event_set();
      timed_state* ptr_sc = new timed_state( su, 
					     timed_st.get_POSET(),
					     timed_st.get_rule() );

      event_set* ptr_Ec = get_causal_events( t, *ptr_sc, Een, Een_expand );
      (*ptr_sc).remove_non_causal( *ptr_Ec );
#ifdef __DUMP__
      cout << endl << "Ec: ";
      dump( cout, t, *ptr_Ec ) << endl;
#endif
      delete ptr_Ec;

      do {
	Een_Rb = Een_expand.get_Een();
#ifdef __DUMP__
	cout << endl << "Een is " << endl;
	dump( cout, t, Een ) << endl;
#endif
	get_fireable_set( Een_Rb, *ptr_sc, Een_expand, t, *ptr_Ef );
	if ( ptr_Ef->size() == Een.size() ) continue;
      } while ( Een_expand.next() );
      
      // 4) Apply a partial order reduction on the set of
      //    fireable events if the PO_reduce is set.
      if ( opts.PO_reduce ) {
	event_set* ptr_Ea;
	_event_list Ef;
	copy( ptr_Ef->begin(), ptr_Ef->end(), 
	      back_insert_iterator<_event_list>( Ef ) );
	ptr_Ea = get_ample_set( t, *ptr_sc, Ef, Een );
#ifdef __PO_COMPARE__
	cout << "Ea: ";
	dump( cout, t, *ptr_Ea ) << endl;
#endif
	delete ptr_Ef;
	ptr_Ef = ptr_Ea;
      }

#ifdef __DUMP__
      cout << endl << "---- TIC conflict resolution ----" << endl;
#endif

      // 5) Check the TIC choice resolution and push the new frame
      bool TIC_resolved = is_TIC_resolved( Een_Rb, *ptr_Ef, t );
      the_stack.push_back( new bap_stack_entry( ptr_sc, ptr_Ef,
						get_Een( the_stack ), 
					        Een, TIC_resolved ) );
      
      // 6) Reports interesting stats after pushing onto the stack;
      ptr_tss->update( cout, the_stack.size() );
#ifdef __DUMP__
      cout << endl << "Pushing stack frame -> " << endl;
      ptr_sc->dump( cout, t ) << endl;
      cout << "Ef: ";
      dump( cout, t, *ptr_Ef ) << endl;
#endif
      // 7) Return control back to bap_rsg
      return;
    } 
  }
}
//------------------------------------------------------------------------
bool disables( const event_type& ei,
	       const event_type& ej,
	       const untimed_state& untimed_st,
	       const tel& t ) {

  /* Not quite right, must check all expresions */
  /* Also, symmetric property is no longer true */
  for (int i=1;i<t.number_events();i++) {
    if (t.is_behavior_rule(i,ei)) {
      for (level_exp curlevel=t.get_expression(i,ei);curlevel;
	   curlevel=curlevel->next) 
	if ((ej % 2==1) && (curlevel->product[t.get_signal(ej)] == '0') ||
	    (ej % 2==0) && (curlevel->product[t.get_signal(ej)] == '1')) {
	  //printf("%s disables %s - [%s] -> %s\n",t.get_event(ej),
	  //	 t.get_event(i),curlevel->product,t.get_event(ei));
	  return true;
	}
    }
  }
  return false;
}

//--------------------------------------------------------is_TDC_candidate
// A given event pair (ei,ej) is NOT a candidate for TDC choice resolution
// iff any of the following conditions hold:
//
//   1) ei does not conflict with ej;
//   2) ei and ej do not share a common enabling;
//   3) ei and ej have single behaviro rules, identical upper bounds,
//      and no level expressions.
//
// If none of these conditions are satisfied, then (ei,ej) is a TDC
// candidate that needs to be addressed.
//------------------------------------------------------------------------
bool is_TDC_candidate( const event_type& ei,
		       const event_type& ej,
		       const untimed_state& untimed_st,
		       const tel& t ) {

  if (disables(ei,ej,untimed_st,t) || disables(ej,ei,untimed_st,t))
    return true;

  // If the 2 events do not conflict, then conflict doesn't apply.
  if ( !( t.is_output_conflict( ei, ej ) ) ) {
    return( false );
  }
  // If 'event' and 'candidate' do not share a common enabling, then 
  // 'candidate' can not be an event that needs to be addressed by 
  // the new conflict semantics
  event_set E;
  t.common_enabling( ei, ej, insert_iterator<event_set>( E, E.begin() ) );
  if ( E.size() == 0 ){
    return( false );
  }
  // If the ei and ej each have a single behavior rule with no level
  // expression, then this is not a situation that needs to be handled
  // in a special way.
  rule_set Ri, Rj;
  is_enabled( ei, untimed_st, t, front_insert_iterator<rule_set>(Ri) );
  is_enabled( ej, untimed_st, t, front_insert_iterator<rule_set>(Rj) );
  for ( event_set::iterator i = E.begin() ; i != E.end() ; ++i ) {
    event_type enabling = *i;
    if ( Ri.size() == 1                                                 && 
	 Rj.size() == 1                                                 &&
	 t.upper_bound( enabling, ei ) == t.upper_bound( enabling, ej ) &&
	 !t.has_level_expression( enabling, ei )                        &&
	 !t.has_level_expression( enabling, ej )                           ) {
      return( false );
    }
  }
  // All conditions are satisfied.  This implies that the event 
  // 'candidate' must be handled elsewhere for correct TDC implementation.
  return( true );

}
//------------------------------------------------------------------------

//---------------------------------------------------------is_TDC_resolved
// A conflict scenario is_TDC_resolved if there does not exist a pair
// of conflict candidates that are both members of Ef.  See the comments
// on is_TDC_candidate for details on what even pairs qualify for TDC
// choice intervention.
//------------------------------------------------------------------------
bool is_TDC_resolved( const enabled_set& Een,
		      const event_set& Ef,
		      const untimed_state& untimed_st,
		      const tel& t ) {
  typedef enabled_set::const_iterator const_iter;
  for ( const_iter i = Een.begin() ; i != Een.end() ; ++i ) {
    event_type ei = get_enabled( *i, t );
    // NOTE: I can start at the next location because conflict is a 
    // reflexive relationship.
    const_iter j = i;
    while ( ++j != Een.end() ) {
      event_type ej = get_enabled( *j, t );
      // ei and ej meet the is_conflict_candidate criteria, so 
      // so this situation will have to be resolved
      // by correctly limiting the upper bounds when firing
      // events from Ef
      if ( Ef.find( ei ) != Ef.end()                 &&
  	   Ef.find( ej ) != Ef.end()                 &&
	   is_TDC_candidate( ei, ej, untimed_st, t )    ) {
#ifdef __DUMP__
  	cout << "__NOT_TDC_RESOLVED_BY_DEFAULT__ -> ";
	cout << t.get_event( ei ) << " # " << t.get_event( ej ) << endl;
#endif
  	return( false );
      }
    }
  }
  return( true );
}
//------------------------------------------------------------------------

//------------------------------------------------bool constrain_TDC_bound
// This function will take a set (Ec) of enabled events that confict
// with the event 'enabled' and find the maximum allowed separation 
// between their common enabling and 'enabled' according to TDC semantics.
// This is done by exploring all causal configurations of Ec adding in
// the relation t(enabled) - t(x) <= 0 where x is in Ec.  This constrains
// the event 'enabled' to fire before all other enabled events.  After the
// consistency check, the maximum value found in the column of the POSET
// for the common enabling is the allowed separation according to TDC
// semantics. This value is maximized over all possible causal 
// configurations.  NOTE: upper_bound is set to the correct value on
// return.
//------------------------------------------------------------------------
void constrain_TDC_bound( enabled_set::iterator current, 
			  enabled_set& Een,
			  const tel& t, 
			  POSET& Me,
			  const event_type& enabled,
			  POSET::bound_type& upper_bound ) {
  typedef rule_set::iterator rule_iter;
  typedef enabled_set::iterator enabled_iter;
  bool recursed = false;

  if ( current != Een.end() ) {
    // An unbounded rule is going to give the largest separation allowed
    // by the net.
    rule_iter end = remove_over_bounded( *current, Me, t );
    for ( rule_iter i = (*current).begin() ; i != end ; ++i ) {
      //if ( !is_causal_candidate( t, *current, Me, i ) ) continue;
      // 1) Generate a causal asignment for this event
      event_type e = t.enabling( *i );
      event_type f = t.enabled( *i );
      if ( !Me.is_in_POSET( e ) ) continue;
      // 2) Set the separation for the causal assignment
      Me.set_max_separation( e, f, t.upper_bound( e, f ) );
      // 3) Set the order so that t(f) >= t(enabled)
      Me.set_order( enabled, f );
      enabled_iter next = current;
      // 4) Make the recursive call to set other causal assignments or
      //    check the POSET consistency to get the bound.
      recursed = true;
      constrain_TDC_bound( ++next, Een, t, Me, enabled, upper_bound );
      // 5) Restore the orignal separations.
      /*      Me.set_max_separation( e, f, POSET::_UNBOUNDED_ );
	      Me.set_max_separation( enabled, f, POSET::_UNBOUNDED_ ); */
    }
  }
  if ( !recursed || current == Een.end() ) {
    POSET Mt( Me );
#ifdef __PROFILE__
    ++inconsistent_checks;
#endif

    // is_consistent must know how many events we have just
    // added.  If any of these events must fire before those
    // already in the POSET, then this POSET is not consistent.
    if ( Mt.is_consistent( Een.size() ) ) {

      //cout << "Mt is consistent" << endl;
      //Mt.dump(cout,t);

      // NOTE: This assumes a single common enabling in this group and
      // uses the last value of f as a reference to find it.
      event_set E;
      // 1) Find the set of common enablings.
      t.common_enabling( enabled, get_enabled( *Een.begin(), t ),
			 insert_iterator<event_set>( E, E.begin() ) );
      /*
      if ( E.size() == 0 ){
	throw( new 
	       error("ERROR: constrain_TDC_bound common conflict not found"));
      }
      */
      // 2) Get the maximum allowed bound for this causality assignment
      for ( event_set::iterator i = E.begin() ; i != E.end() ; ++i ) {
	POSET::bound_type tmp = Mt.get_max_over( *i, Een.size() );
	if ( tmp < upper_bound ) {
	  upper_bound = tmp;
	}
      }
#ifdef __DUMP__
      cout << "max separation allowed for " << t.get_event( enabled ) << " : " 
	   << upper_bound << endl;
#endif
    }
#ifdef __PROFILE__
    else {
      ++inconsistent;
    }
#endif
  }
}
//------------------------------------------------------------------------

//-------------------------------------------------------bool is_TDC_issue
// A TDC issue can only occur when a stack frame is not choice clean
// and there exists for this event, another event that it conflicts with 
// that is currently enabled in the untimed state.
// NOTE: This adds to Ec the marked rule_set's for each enabled 
// conflicting event.
//------------------------------------------------------------------------
bool is_TDC_issue( const untimed_state& untimed_st, 
		   const tel& t, 
		   const event_type& event, 
		   /*front_*/insert_iterator<enabled_set> Ec ) {
  bool issue = false;
  for ( event_type i = 1 ; i < (event_type)t.number_events() ; ++i ) {
    rule_set Re;
    if ( i != event                                                    && 
	 (t.is_output_conflict( i, event ) ||
	  disables(i,event,untimed_st,t))
	 && is_enabled(i,untimed_st,t,front_insert_iterator<rule_set>(Re))  ) {

      level_expand( t, Re.begin(), Re.end(), Re, untimed_st,
		    Ec /*inserter( Ec, Ec.begin() )*/ );

      issue = true;
      //*Ec++ = Re;
    }
  }
  return( issue );
}
//------------------------------------------------------------------------

//----------------------------------------------------void get_timed_states
// get_timed_states returns the list of possible future timed states
// according to allowed causality assignments.  In addition, this
// function addresses unresolved TDC.  NOTE: Ec is not const because
// constrain_TDC_bound can change the order of its entries.
//------------------------------------------------------------------------ 
template <class inserter>
void get_timed_states( const tel& t, 
		       rule_set& Rb, 
		       const untimed_state& untimed_st,
		       POSET Me,
		       inserter S,
		       enabled_set& Ec,
		       bool TDC = false ) {
  typedef rule_set::iterator iterator;
  Me.add( Rb, t );
  // After the lower bounds have been added into Me (see previous line), 
  // then Remove rules from Rb that are overbounded by an unbounded rule
  // (i.e., exists a rule r in Rb with an unbounded upper limit, then 
  // all the other rules no longer matter; they will never be constraining).
  iterator end = remove_over_bounded( Rb, Me, t );
  for ( iterator i = Rb.begin() ; i != end ; ++i ) {
    event_type causal = t.enabling( *i );
    event_type enabled = t.enabled( *i );
    if ( !Me.is_in_POSET( causal ) ) continue;
    POSET::bound_type upper_bound = t.upper_bound( causal, enabled );
    Me.set_max_separation( causal, enabled, upper_bound );
    timed_state* timed_st = new timed_state( untimed_st, Me, *i );
    set_causal_order( t, Rb.begin(), Rb.end(), i, *timed_st );
    if ( timed_st->is_consistent() ) {
#ifdef __DUMP__
      cout << endl << "Timed state: " << endl;
      timed_st->dump( cout, t );
#endif
      // This is a TDC scenario that is not default covered.  Find
      // the constrained upper_bound and use that as the max
      // separation to the causal event.
      if ( TDC ) {
	POSET Mt( Me );
	Mt.add( Ec, t );
	POSET::bound_type original = upper_bound;
	// 1) Find the maximum allowed separation under TDC semantics

  	cout << endl << "Ec: ";
  	dump( cout, t, Ec ) << endl;

	constrain_TDC_bound( Ec.begin(), Ec, t, Mt, enabled, upper_bound );

	/* THIS IS KINDA A MESS */
	if (Mt.is_consistent( Ec.size() )) {
	  cout << "Mt:" << endl;
	  Mt.dump(cout,t);
	  for (int i=1;i<t.number_events();i++)
	    for (int j=1;j<t.number_events();j++)
	    if ((Me.is_in_POSET(i)) && (Me.is_in_POSET(j))) {
	      timed_st->constrain( i, j,
				   Mt.get_causal_separation( j, i) );
	      cout << "Setting " << t.get_event(i) << " " << 
	      t.get_event(enabled) << " to " << 
	      Mt.get_causal_separation( enabled, i ) << endl;
	    }
	  cout << "Me:" << endl;
	  timed_st->dump(cout,t);
	}

	if ( upper_bound != original ) {
	  // 2) Add the constraint to the timed state. NOTE: this function 
	  //    checks consistency after the constraint is added.
	  timed_st->constrain( causal, enabled, upper_bound );
#ifdef __DUMP__
	  cout << "---- TDC conflict resolution ----" << endl;
	  cout << "Constraining bound for "
	       << "(" << t.get_event( causal ) << "," 
	       << t.get_event( enabled ) << ") from " << original 
	       << " to " << upper_bound << endl;
#endif
	}
      }
      *S++ = timed_st;
    }
    else {
      delete timed_st;
    }
    Me.set_max_separation( causal, enabled, POSET::_UNBOUNDED_ );
  }
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
template <class stack_inserter>
void initialize_stack( const tel& t, bool verbose, 
		       stack_inserter the_stack,
		       timeoptsADT opts ){
  // 1) Get and check the initial marking?  
  // Throws exception if the initial marking is NULL
  untimed_state untimed_st( t, verbose );
  
  // Throws exception if initial marking has a merge conflict
  // with both rules marked.
  try {
    check_merge_conflicts( t, untimed_st.get_marking() );
  }
  catch( error* ptr_e ) {
    ptr_e->print_message( lg );
    delete ptr_e;
  }
  // 2) Create initial timed state from marked rules in the untimed_st?
  // Throws out of memory error if constructor fails
  timed_state* ptr_timed_st = new timed_state( t, untimed_st );

  // 3) Get the set of untimed enabled events from the new 
  //    untimed state su.
  enabled_set Een_Rb;
  get_enabled_events( ptr_timed_st->get_untimed_st(), t,
		      front_insert_iterator<enabled_set>( Een_Rb ) );
  event_set Een;
  transform( Een_Rb.begin(), Een_Rb.end() , 
	     inserter( Een, Een.begin() ) , 
	     bind2nd( get_event(), &t )     );
  enabled_set_expander Een_expand( t, untimed_st, Een_Rb );

  // 4) Create initial firing list?
  event_set* ptr_Ef = new event_set();
  do {
    Een_Rb = Een_expand.get_Een();
#ifdef __DUMP__
    cout << endl << "Een is " << endl;
    dump( cout, t, Een_Rb ) << endl;
#endif
    // If no events are fireable, this will throw a deadlock_error
    get_fireable_set( Een_Rb, *ptr_timed_st, Een_expand, t, *ptr_Ef );
    if ( ptr_Ef->size() == Een.size() ) continue;
  } while ( Een_expand.next() );
  
  // 5) Apply a partial order reduction on the set of
  //    fireable events if the PO_reduce is set.
  if ( opts.PO_reduce ) {
    event_set* ptr_Ea;
    _event_list Ef;
    copy( ptr_Ef->begin(), ptr_Ef->end(), 
	  back_insert_iterator<_event_list>( Ef ) );
    ptr_Ea = get_ample_set( t, *ptr_timed_st, Ef, Een );
#ifdef __PO_COMPARE__
    cout << "Ea: ";
    dump( cout, t, *ptr_Ea ) << endl;
#endif
    delete ptr_Ef;
    ptr_Ef = ptr_Ea;
  }

  // 6) Push timed_state( initial_marking, firing list ) onto stack.
  //    choice_clean denotes the choice state: resolved or unresolved.
  bool TDC = opts.TDC;
  bool choice_clean = !TDC && is_TIC_resolved( Een_Rb, *ptr_Ef, t ) ||
                      TDC && is_TDC_resolved( Een_Rb, *ptr_Ef, untimed_st, t );
  *the_stack++ = new bap_stack_entry( ptr_timed_st , 
				      ptr_Ef       , 
				      Een          , 
				      choice_clean   );
}
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------void clean_state_space
void clean_state_space( timed_state_space* ptr_tss, 
			 timed_state* sc, 
			 event_type f, 
			 const tel& t ) {
  if ( ptr_tss != NULL ) {
    // Add the error state to the timed state space to generate
    // a correct error trace.
    if ( sc != NULL ) {
      ptr_tss->error( t, *sc, f );
    }
    // Clean up the timed_state_space interface if we had an exception
    // NOTE: This is just an INTERFACE.  The state space is not deleted
    // by this.
    delete( ptr_tss );
  }
}
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
bool bap_rsg( tel& t                ,
	      stateADT *state_table ,
	      timeoptsADT opts      ,		      
	      bddADT bdd_state      , 
	      bool use_bdd          ,
	      bool verbose          ,
	      bool noparg             ) {
  typedef front_insert_iterator<timed_st_set> timed_st_set_insert;

   // Check that this TEL is sensible on the surface.
  if ( !( t.good_tel() ) ) return( report_incomplete_tel( t.get_fname() ) );

  // Set the prunning option
  set_prune( opts.prune );

  // This is the call stack for the depth first search
  bap_stack the_stack;

  // This is the the pointer to the state space interface
  timed_state_space* ptr_tss = NULL;

  // Used to hole references from the front of the stack
  event_type f = 0;        // Event to fire in this iterations
  timed_state* sc = NULL;  // Associated timed state

  // Records if a stack frame has unresolved TIC issues
  bool choice_clean = true;

  bool needs_cache( is_prune_set() || opts.PO_reduce );

  // Initialize the cache if neeced.  Besure to delete this cache 
  // when done with algorithm.  This must be done before the call
  // to initialize_stack or any call to get_causal_events or
  // get_ample_set.
  if ( needs_cache ) init_cache( t.number_events() );

  try {

    // Create state space interface object.
    // Throws a file_error if it cannot open a file to dump states to.
    // NOTE: lg is the global FILE* to the atacs.log file.
    ptr_tss = new timed_state_space( lg, 1000, t, state_table, opts, bdd_state,
				     use_bdd, verbose );
    if ( ptr_tss == NULL ) {
      throw( new error( "ERROR: out of memory" ) );
    }

    // initialize_stack can throw a deadlock error if no events are fireable.
    initialize_stack( t, verbose, front_insert_iterator<bap_stack>(the_stack),
		      opts );
    
    // Add the initial state to the state_table
    sc = front( the_stack );
    ptr_tss->add( t, *sc );

#ifdef __PROFILE__
    combinatorial_profiler profile;
    profile.update();
#endif

#ifdef __DUMP__
    cout << endl << "Adding timed state -> " << endl;
    sc->dump( cout, t ) << endl;
#endif

    while( !( the_stack.empty() ) ) {
    
      // 1) Get the next event (f) to fire from the stack with its 
      //    accompanying timed state (sc).
      sc = front( the_stack, f, choice_clean );

#ifdef __DUMP__
      cout << endl << "Firing: " << t.get_event( f ) << "-> " << endl;
      sc->dump( cout, t ) << endl;
#endif
      
      // 2) Handle unresolved timed independent choice issues
      //    if this is a dirty stack frame.
      if ( !opts.TDC && !choice_clean ) {
	// NOTE: This makes call to get_ample_set and get_causal_events;
	// thus, it must reinitialize the cache accordingly.
	if ( needs_cache ) reinit_cache( t.number_events() );
	resolve_TIC_choice( f, *sc, ptr_tss, t, opts, the_stack );
      }

      // 3) Get the behavioral rules (Rb) that enabled f in sc
      rule_set Rb;
      get_rule_set( f, sc->get_untimed_st(), t, Rb );
      enabled_set Rbx;
      level_expand( t, Rb.begin(), Rb.end(), Rb, sc->get_untimed_st(),
		    inserter( Rbx, Rbx.begin() ) );

      // 4) Get the new untimed state (su) derived from firing f
      untimed_state su( t, sc->get_untimed_st(), f , opts, noparg );

      // 5) Get the set of untimed enabled events from the new 
      //    untimed state su.
      enabled_set Een_Rb;
      get_enabled_events(su, t, front_insert_iterator<enabled_set>( Een_Rb ));
#ifdef __DUMP__
      cout << endl << "Untimed state: ";
      dump( cout, t, su.get_marking() ) << endl;
      cout << endl << "Een: ";
      dump( cout, t, get_Een( the_stack )) << endl;
      cout << endl << "Een_Rb: " << endl;
      dump( cout, t, Een_Rb ) << endl << flush;
#endif
      non_disabling_semantic_support( t, su, get_Een( the_stack ), Een_Rb,
				      opts.disabling                       );
      event_set Een;
      transform( Een_Rb.begin(), Een_Rb.end() , 
		 inserter( Een, Een.begin() ) , 
		 bind2nd( get_event(), &t )      );
#ifdef __DUMP__
      cout << endl << "Een: ";
      dump( cout, t, Een ) << endl;
      cout << endl << "Een_Rb: " << endl;
      dump( cout, t, Een_Rb ) << endl << flush;
#endif
      enabled_set_expander Een_expand( t, su, Een_Rb );

      // 6) Set the TDC flag to handle unresolved TDC choice issues if
      //    required by the stack frame.  NOTE: This is used by 
      //    get_timed_states to correctly set bounds on causal assignments.
      enabled_set Ec;
      bool TDC = opts.TDC && !choice_clean &&
	         is_TDC_issue( sc->get_untimed_st(), t, f, 
			       /*front_*/
                 insert_iterator<enabled_set>(Ec,Ec.begin()) );

      // 7) Build a list of succsessor timed states derived from firing f
      //    NOTE: get_timed_states changes the order of entries in Rb!
      timed_st_set S;
      for ( enabled_set::iterator i = Rbx.begin() ; i != Rbx.end() ; ++i ) {
	POSET Mt( sc->get_POSET() );
	set_causal_order( t, Rbx.begin(), Rbx.end(), i, Mt );
	get_timed_states( t, *i, su, Mt, 
			  timed_st_set_insert(S), Ec, TDC );
      }
    
      // 7.5) Initialize the necessary_set cache.  This cache
      // is used by get_causal_events AND get_ample_set
      if ( needs_cache ) reinit_cache( t.number_events() );

      // 8) Loop through each new timed state, and if it is a new state
      //    build a fire_list and make a new stack entry.
      for ( timed_st_set::iterator i = S.begin() ; i != S.end() ; ++i ) {
	event_set* ptr_Ec = get_causal_events( t, **i, Een, Een_expand );
	(**i).remove_non_causal( *ptr_Ec );
#ifdef __DUMP__
  	cout << endl << "Ec: ";
  	dump( cout, t, *ptr_Ec ) << endl;
#endif
	delete ptr_Ec;
	if ( ptr_tss->add( t, *sc, **i ) ) {

	  // 1) Get the set of concurrently enabled events.
	  event_set* ptr_Ef = new event_set();
	  do {
	    Een_Rb = Een_expand.get_Een();
#ifdef __DUMP__
	    cout << endl << "Een is " << endl;
	    dump( cout, t, Een_Rb ) << endl;
#endif
	    get_fireable_set( Een_Rb, **i, Een_expand, t, *ptr_Ef );
	    if ( ptr_Ef->size() == Een.size() ) continue;
	  } while ( Een_expand.next() );

	  // This must be a bad future zone due to a previous causality
	  // assignment. See notebook page 102
	  if ( ptr_Ef->empty() ) {
	    delete ptr_Ef;
	    continue;
	  }

#ifdef __DUMP__
	  cout << endl << "Adding timed state -> " << endl;
	  (*i)->dump( cout, t ) << endl;
#endif
#if defined(__DUMP__) || defined(__PO_COMPARE__)
	  cout << "Ef: ";
	  dump( cout, t, *ptr_Ef ) << endl;
	  cout << "Een: ";
	  dump( cout, t, Een ) << endl;
	  cout << "get_Een( the_stack ): ";
	  dump( cout, t, get_Een( the_stack ) ) << endl;
#endif
	  
	  // 2) Apply a partial order reduction on the set of
	  //    fireable events if the PO_reduce is set.
	  if ( opts.PO_reduce ) {
	    event_set* ptr_Ea;
	    _event_list Ef;
	    build_age_order( the_stack, *ptr_Ef, 
			     back_insert_iterator<_event_list>( Ef ) );
	    ptr_Ea = get_ample_set( t, **i, Ef, Een );
#ifdef __PO_COMPARE__
	    cout << "Ea: ";
	    dump( cout, t, *ptr_Ea ) << endl << endl;
#endif
	    delete ptr_Ef;
	    ptr_Ef = ptr_Ea;
	  }
	  
	  // 3) Get the choice status of this stack frame according
	  //    to the user specified semantics.
	  choice_clean = !opts.TDC && is_TIC_resolved(Een_Rb, *ptr_Ef, t) ||
	                  opts.TDC && is_TDC_resolved(Een_Rb, *ptr_Ef, su, t);
	  
	  // 4) Push the new frame on the stack.
	  the_stack.push_back( new bap_stack_entry( *i                   , 
						    ptr_Ef               ,
						    get_Een( the_stack ) ,
						    Een, choice_clean )    );
	  
	  // 5) Report interesting stats after adding new state
	  ptr_tss->update( cout, the_stack.size() );
#ifdef __PROFILE__
	  profile.update();
#endif
	}
	else {
	  // This is NOT being added to the stack, so clean it up now!
	  delete *i;
	}
      }
      // 9) Pop the entry from the stack since it has now been processed
      //    This will delete memory if the front of the stackas an
      //    empty firing list.
      pop( the_stack );           
    }
#ifdef __PROFILE__
    profile.report( t.get_fname() );
#endif
    ptr_tss->report( t.get_fname() );
#ifdef __DUMP_STATE__
    ptr_tss->dump( cout, t );
#endif
    delete( ptr_tss );
    // Delete this cache if it was initialized
    if ( needs_cache ) delete_cache( t.number_events() );
    return( true );
  }
  catch( marking_error* ptr_e ) {
    ptr_e->print_message( lg );
    delete ptr_e;
  }
  catch( error* ptr_e ) {
    ptr_e->print_message( lg );
    delete ptr_e;
  }
  clean_state_space( ptr_tss, sc, f, t );
  clean_stack( the_stack );
  // Delete this cache if it was initialized
  if ( needs_cache ) delete_cache( t.number_events() );
  return( false );
}
//////////////////////////////////////////////////////////////////////////
