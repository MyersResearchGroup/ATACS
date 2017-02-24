//////////////////////////////////////////////////////////////////////////
// @name Level Expansion Support for BAP
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#ifndef __LEVEL_EXPAND_H__
#define __LEVEL_EXPAND_H__

#include <iterator>

#include "bap.h"

// Current represents the causal assignment for the event being fired.
// This function adds relations to the POSET in the timed state that
// forces all events in context level rules from the expression to fire
// before the causal assignment.
void set_causal_order( const tel& t                     ,
		       rule_set::const_iterator first   ,
		       rule_set::const_iterator last    ,
		       rule_set::const_iterator current ,
		       timed_state& sc                    );

void set_causal_order( const tel& t                        ,
		       enabled_set::const_iterator first   ,
		       enabled_set::const_iterator last    ,
		       enabled_set::const_iterator current ,
		       POSET& Me                             );

bool is_non_disabling( int event                       , 
		       const untimed_state& untimed_st , 
		       const tel& t                    ,
		       rule_set_inserter Re              );

void non_disabling_semantic_support( tel& t                    , 
				     const untimed_state& su   , 
				     const _event_list& Een    , 
				     const enabled_set& Een_Rb ,      
				     bool no_fail_on_disable     );

void level_expand( const tel& t                        ,
		   rule_set::const_iterator first      ,
		   rule_set::const_iterator last       ,
		   rule_set& Rb                        ,
		   const untimed_state& st             ,
		   insert_iterator<enabled_set> Rb_set   );


class enabled_set_expander {

//   typedef list<enabled_set,malloc_alloc> enabled_set_list;

  typedef list<enabled_set> enabled_set_list;
  
  struct state_entry {
    enabled_set::iterator first;
    enabled_set::iterator last;
    enabled_set::iterator current;
  };

//   typedef vector<state_entry*,malloc_alloc> state_vector;
  typedef vector<state_entry*> state_vector;
  
protected:

  enabled_set_list _M_Een_set_list;
  state_vector _M_state_vector;
  enabled_set _M_Een;

protected:

  bool next( state_vector::iterator i );

public:

  enabled_set_expander( const tel& t            , 
			const untimed_state& st , 
			enabled_set& Een          );
  ~enabled_set_expander();
  bool next();
  const enabled_set& get_Een() const;
  void set_causal_orders( const tel& t, POSET& Me ) const;

};
#endif
