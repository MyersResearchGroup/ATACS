//////////////////////////////////////////////////////////////////////////
// @name Level Expansion Support for BAP
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "level_expand.h"
#include "error.h"

//------------------------------------------------------------------------
// I really need to just move this to the bap.h file!
extern ostream& dump( ostream& s, const tel& t, const rule_set& Rb );
//------------------------------------------------------------------------

//---------------------------------------------------void set_causal_order
void set_causal_order( const tel& t                     ,
		       rule_set::const_iterator first   ,
		       rule_set::const_iterator last    ,
		       rule_set::const_iterator current ,
		       timed_state& sc                    ) {
  event_type causal( t.enabling( *current ) );
  while ( first != last ) {
    rule_set::const_iterator i( first++ );
    if ( i == current ) continue;
    if ( !t.is_level_rule( *i ) ) continue;
    event_type context( t.enabling( *i ) );
    // The context from the level rule must fire before the causal
    sc.set_order( context, causal );
  }
}
//------------------------------------------------------------------------

//---------------------------------------------------void set_causal_order
void set_causal_order( const tel& t                        ,
		       enabled_set::const_iterator first   ,
		       enabled_set::const_iterator last    ,
		       enabled_set::const_iterator current ,
		       POSET& Me                             ) {
  const rule_set& Rcurrent( *current );
  while ( first != last ) {
    enabled_set::const_iterator i( first++ );
    if ( i == current ) continue;

    /* Not right, conservative */
    bool cont=false;
    for ( rule_set::const_iterator j = Rcurrent.begin() ; 
	  j != Rcurrent.end()                           ;
	  ++j                                             ) {
      if ( !t.is_level_rule( *j ) ) continue;
      const rule_set& Rnext( *i );
//       event_type ej( t.enabling( *j ) );
      for ( rule_set::const_iterator k = Rnext.begin() ; 
	    k != Rnext.end()                           ;
	    ++k                                          ) {
	if (*j == *k)  {
	    cont=true;
	    break;
	  }
      }
      if (cont) break;
    }
    if (cont) continue;
    /* TO HERE */

    for ( rule_set::const_iterator j = Rcurrent.begin() ; 
	  j != Rcurrent.end()                           ;
	  ++j                                             ) {
      if ( !t.is_level_rule( *j ) ) continue;
      const rule_set& Rnext( *i );
      event_type ej( t.enabling( *j ) );
      for ( rule_set::const_iterator k = Rnext.begin() ; 
	    k != Rnext.end()                           ;
	    ++k                                          ) {
	if ( !t.is_level_rule( *k ) ) continue;
	event_type ek( t.enabling( *k ) );
	/*
    	cout << endl << "Adding order relation to "
    	     << t.get_event( ej ) << " and " << t.get_event( ek ) << endl;
	*/
	//    	Me.dump( cout, t ) << endl << endl;
	Me.set_order( ej, ek );
//    	Me.dump( cout, t ) << endl;
      }
    }
  }
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
bool is_non_disabling( int event                       , 
		       const untimed_state& untimed_st , 
		       const tel& t                    ,
		       rule_set_inserter Re              ) {

  // If an event is fictitious, then it has NO behavior rules.  This
  // flag denotes that situation.
  bool behavior_rule_exists = false;
  // foreach behavior rule that enables the event (ignore reset: i=0 )
  for ( int i = 1 ; i < t.number_events() ; ++i ) {
    if ( t.is_behavior_rule( i, event ) ) {
      behavior_rule_exists = true;
      // Is this rule not enabled?
      int enabling_rule = t.marking_index( i, event );
      if ( !( ( untimed_st.is_marked( enabling_rule ) ) &&
	      ( has_satisfied_expression( i, event, untimed_st, t ) 
		|| !t.is_disabling_rule( i, event ) ) ) ){
	bool another_rule = false;
	// Is there another rule that is enabled and does the enabling 
        // event for that rule conflict with i? 
	for ( int j = 1 ; j < t.number_events() ; ++j ) {
	  if ( t.is_behavior_rule( j, event ) && 
	       t.is_input_conflict( j, i ) ) {
	    enabling_rule = t.marking_index( j, event );
	    if ( ( untimed_st.is_marked( enabling_rule ) ) &&
		 ( has_satisfied_expression( j, event, untimed_st, t ) 
		   || !t.is_disabling_rule( j, event ) ) ) {
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
//------------------------------------------------------------------------

//------------------------------------------------------------------------
void non_disabling_semantic_support( tel& t                    , 
				     const untimed_state& su   , 
				     const _event_list& Een    , 
				     const enabled_set& Een_Rb ,
				     bool no_fail_on_disable     ) {
  no_fail_on_disable = true;
  for ( _event_list::const_iterator i( Een.begin() ) ; 
	i != Een.end()                               ; 
	++i                                            ) {
    if ( find_if( Een_Rb.begin(), Een_Rb.end(),
		  bind2nd( compare( t ), *i ) ) != Een_Rb.end() ) continue;
    rule_set Rb;
    // NOTE: The current implemenation will fail if you add this 
    // event to Een_Rb.  This is because when it tries to find causality
    // on the level expressions that are no longer true in the state.
    // See the level_expand functions for more details
    if ( is_non_disabling( *i, su, t, rule_set_inserter( Rb ) ) &&
         su.is_allowed_to_fire(*i) ) {
      // Fast path: a disabling on this signal has already been reported
      if ( no_fail_on_disable && t.is_marked_disabled( *i ) ) continue;
      ostringstream message;
      message << ( (!no_fail_on_disable) ? "ERROR: " : "WARNING: " )
	      << t.get_event( *i ) << " disabled in state "
	      << su.get_state();
      // A disabled event is a failure
      if ( !no_fail_on_disable ) {
	message << ends;
	throw( new error( message.str() ) );
      }
      // A disabled event is a warning
      else {
	message << endl << "WARNING: "
		<< "(disabling reported once "
		<< "for first state it occurs in)" << endl << ends;
	cout << message.str();
	fprintf( lg, "%s", message.str().c_str() );
	t.mark_disabled( *i );
      }
    }
  }
}
//------------------------------------------------------------------------

//--------------------------------const_iterator find_rule_with_expression
rule_set::const_iterator 
find_rule_with_expression( const tel& t                   ,
			   rule_set::const_iterator first ,
			   rule_set::const_iterator last    ) {
  while( first != last ) {
    marking_index index( *first );
    event_type enabling( t.enabling( index ) );
    event_type enabled( t.enabled( index ) );
    if ( t.has_level_expression( enabling, enabled ) ) {
      return( first );
    }
    ++first;
  }
  return( last );
}
//------------------------------------------------------------------------

//-----------------------------------------------bool is_satisfied_product
bool is_satisfied_product( const tel& t            ,
			   level_exp exp           ,
			   const untimed_state& sc   ) {
  for ( unsigned i = 0 ; i < (unsigned)t.get_nsigs() ; ++i ) {
    // If this is a dont-care term in the product, then move to the
    // next term.
    if ( exp->product[i] == 'X' ) continue;
    // If the term is positive phase and the signal is low in the current 
    // state or the term is negative phase and the signal is high in the 
    // current state, then this product is NOT satisfied.  Move to the
    // next product.
    if ( ( exp->product[i] == '1' && sc.is_low( i ) )  ||
	 ( exp->product[i] == '0' && sc.is_high( i ) )    ) return( false );
  }
  return( true );
}
//------------------------------------------------------------------------

//-------------------------------------------------------void level_expand
void level_expand( const tel& t                        ,
		   rule_set::const_iterator first      ,
		   rule_set::const_iterator last       ,
		   rule_set& Rb                        ,
		   const untimed_state& st             ,
		   insert_iterator<enabled_set> Rb_set   ) {
  rule_set::const_iterator i( find_rule_with_expression( t, first, last ) );
  if ( i == last ) {
    *Rb_set++ = Rb;
    return;
  }
  event_type enabled( t.enabled( *i ) );
  event_type enabling( t.enabling( *i++ ) );
  level_exp exp( t.get_expression( enabling, enabled ) );
  bool satisfied_product_exists( false );
  for ( ; exp != NULL ; exp = exp->next ) {
    // If the expression is _not_ satisfied, panic because I do not know
    // what do to here.
    if ( !is_satisfied_product( t, exp, st ) )  continue;
    satisfied_product_exists = true;
    unsigned added_rules = 0;
    for ( unsigned j = 0 ; j < (unsigned)t.get_nsigs() ; ++j ) {
      // If this is a dont-care term in the product, then move to the
      // next term.
      if ( exp->product[j] == 'X' ) continue;
      for ( unsigned k = 0 ; k < (unsigned)t.number_events() ; ++k ) {
	// If event k is not an event on signal j, then move on
	if ( ( (unsigned)t.get_signal( k ) != j ) ) continue;
	// If signal j is high and this is a falling event, then move on
	if ( exp->product[j] == '1'  && t.is_falling_event( k ) ) continue;
	// If signal j is low and this is a rising event, then move on
	if ( exp->product[j] == '0' && t.is_rising_event( k ) ) continue;
	// If this rule is already in the rule_set, then move on.
	marking_index level_rule( t.level_rule_index( k, enabled ) );
	if ( find( Rb.begin(), Rb.end(), level_rule ) != Rb.end() ) continue;
	Rb.push_front( level_rule );
	++added_rules;
      }
    }
    level_expand( t, i, last, Rb, st, Rb_set );
    while ( added_rules-- ) Rb.pop_front();
  }
  if ( !satisfied_product_exists ) {
    ostringstream message;
    message << "ERROR: " << t.get_event( enabling ) 
            << " no longer has an enabled product term on rule "
            << "<" << t.get_event( enabling ) << ","
	    << t.get_event( enabled ) << "> " << ends;
    // NOTE: How do you define causality for a product term that 
    // enabled the rule but has now become falsified?  What does that mean?
    throw( new error( message.str() ) );
  }
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
enabled_set_expander::enabled_set_expander( const tel& t            , 
					    const untimed_state& st ,
					    enabled_set& Een          ) 
  : _M_Een_set_list(),
    _M_state_vector( Een.size(), NULL ),
    _M_Een( Een.size() ) {
  state_vector::iterator j( _M_state_vector.begin() );
  enabled_set::iterator k( _M_Een.begin() );
  for ( enabled_set::iterator i = Een.begin() ; i != Een.end() ; ++i ) {
    _M_Een_set_list.push_front( enabled_set() );
    enabled_set& ref( _M_Een_set_list.front() );
    rule_set& Rb( *i );
    level_expand( t, Rb.begin(), Rb.end(), Rb, st,
		  inserter( ref, ref.begin() ) );
    assert( ref.size() != 0 );
    *j = new struct state_entry();
    (*j)->first = ref.begin();
    (*j)->last = ref.end();
    (*j)->current = (*j)->first;
    *k++ = *((*j++)->current);
  }
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
enabled_set_expander::~enabled_set_expander() {
  state_vector::iterator i( _M_state_vector.begin() );
  while ( i != _M_state_vector.end() ) {
    delete *i++;
  }
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
bool enabled_set_expander::next( state_vector::iterator i ) {
  if ( i == _M_state_vector.end() ) return( false );
  enabled_set::iterator& current( (*i)->current );
  if ( ++current != (*i)->last ) return( true );
  current = (*i)->first;
  return( next( ++i ) );
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
bool enabled_set_expander::next() {
  state_vector::iterator i( _M_state_vector.begin() );
  bool result( next( i ) );
  enabled_set::iterator j(_M_Een.begin() );
  while( i != _M_state_vector.end() ) {
    *j++ = *((*i++)->current);
  }
  return( result );
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
const enabled_set& enabled_set_expander::get_Een() const {
  return( _M_Een );
}
//------------------------------------------------------------------------

//--------------------------------------------------void set_causal_orders
void enabled_set_expander::set_causal_orders( const tel& t , 
					      POSET& Me      ) const {
  state_vector::const_iterator i( _M_state_vector.begin() );
  while ( i != _M_state_vector.end() ) {
    const struct state_entry& sv( *(*i++) );
    ::set_causal_order( t, sv.first, sv.last, sv.current, Me );
  }
}
//------------------------------------------------------------------------
