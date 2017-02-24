//////////////////////////////////////////////////////////////////////////
// @name Bourne Again POSET timing analysis
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#ifndef __BAP_H__
#define __BAP_H__

//gcc296 method
//#include <stdiostream.h>

#include <list>
#include <set>

#include "telwrapper.h"
#include "cpu_time_tracker.h"

//////////////////////////////////////////////////////////////////////////
//
typedef unsigned int marking_index;
// NOTE: malloc_alloc will not speculatively allocate memory.
// It only allocates what you ask for.  This lets memory analyzers
// run without mistakenly reporting leaked memory -> see comments
// in telwrapper.h::event_set.  The type of allocation makes sense
// for any type of list based object in a memory critical application.
#ifdef __GCC_OLD_OPT__
typedef list<marking_index,malloc_alloc> rule_set;
#else
typedef list<marking_index> rule_set;
#endif

#ifdef __GCC_OLD_OPT__
typedef list<rule_set,malloc_alloc> enabled_set;
#else
typedef list<rule_set> enabled_set;
#endif

typedef front_insert_iterator<rule_set> rule_set_inserter;
//
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
event_type get_enabled( const rule_set& R, const tel& t );
//------------------------------------------------------------------------

//---------------------------------------------------------------get_event
struct compare : public binary_function<rule_set,event_type,bool> {

  const tel* t;

  compare( const tel& _t ) : t( &_t ) {}

  bool operator()( const rule_set& e   , 
		   const event_type& f   ) const {
    return( get_enabled( e, *t ) == f );
  }

};
//------------------------------------------------------------------------

//-------------------------------------------------------void get_rule_set
class untimed_state;
void get_rule_set( const event_type& f             ,
		   const untimed_state& untimed_st , 
		   const tel& t                    ,
		   rule_set& Re                      );
//------------------------------------------------------------------------

//-------------------------------------------bool has_satisfied_expression
bool has_satisfied_expression( const int& enabling             ,
			       const int& enabled              ,
			       const untimed_state& untimed_st ,
			       const tel& t                      );
//------------------------------------------------------------------------

//----------------------------------rule_set::iterator remove_over_bounded
class POSET;
rule_set::iterator remove_over_bounded( rule_set& Rb    , 
					const POSET& Me , 
					const tel& t      );
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//
class untimed_state {
protected:
  markingADT ptr_untimed_st;
  // The E_not_allowed set is used to to implement
  // the TIC choice semantics.  If an event e is in E_not_allowed, then
  // even though e is enabled in the marking, due to TIC choice 
  // it has been disabled because the conflict has already been
  // resolved earlier in the trace, even though this event is still
  // enabled in the marking.  This occures in atacs/examples/er/TIC_test.er.
  // $a # $b; $a, $b, and $c are concurrently enabled, and t($a) <
  // t($c) < t($b).  Allowed races: $a -> $c or $c -> $b.  NOTE: conflict
  // is resolved, then $c fires, Enoten will tell me that $b must
  // fire next, not $a.
  event_set* E_not_allowed;
public:
  typedef front_insert_iterator<rule_set> inserter;
public:
  untimed_state( const tel& t, bool verbose );
  untimed_state( const tel& t, 
		 const untimed_state& untimed_st,
		 const event_type& event, 
		 timeoptsADT timeopts,
		 bool noparg );
  untimed_state( const untimed_state& untimed_st );
  ~untimed_state();
  markingADT get_markingADT() const;
  bool is_marked( int marking_index ) const;
  char* get_state() const;
  char* get_marking() const;
  char* get_enablings() const;
  // returns true IFF event is not found in E_not_allowed
  // implying that this event is allowed to fire in
  // the marking.
  bool is_allowed_to_fire( const event_type& event ) const;
  // Adds event to E_not_allowed
  void mark_explored( const event_type& event );
  const event_set& get_E_not_allowed() const;
  bool is_low( unsigned i ) const;
  bool is_high( unsigned i ) const;
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NOTE: In the case that duplicate events are added into the POSET,
// then all get_value and set_value operations will return the value
// of the most recently added duplicate.  This assumes that when an event
// is added to the POSET, it is added onto the end of the ptr_map and
// the dbm.  Thus, the most recently added of duplicate events is always
// at the end of the ptr_map and dbm.
//////////////////////////////////////////////////////////////////////////
class POSET {
public:
  typedef int bound_type;
  typedef vector<int> event_map;
  typedef unsigned int index_type;
  typedef unsigned int event_type;
  typedef event_map::size_type size_type;
public:
  static const bound_type _UNBOUNDED_;
  static const bound_type _MARKED_;
protected:
  event_map* ptr_map;
  bound_type** dbm;
protected:
  void floyds();
  // NOTE: This assumes that dim-1 is the index of the newly added event.
  void incremental_floyds( size_type dim );
  void delete_dbm( bound_type** p, size_type size );
  bound_type** copy_shrink_dbm( bound_type** p, size_type size_p,
				size_type size_new );
  bound_type** copy_expand_dbm( bound_type** p, size_type size_p,
				size_type size_new );
  bound_type** copy_expand_dbm2( bound_type** p, size_type size_p,
				 size_type size_new );
  bound_type** copy_dbm( bound_type** p, size_type size ) const;
  unsigned int max_length( const tel& t ) const;

  template<class inserter>
  void build_not_in_set( const event_set& E, inserter Ekill ) const;

  template<class inserter>
  void build_duplicate_set( inserter Edup ) const;

  void update_and_mark_dbm( event_map* ptr_new_map,
			    const event_set& Enotin,
			    event_set& Edup );

  bool get_causal_indexes( index_type& index_i, index_type& index_j,
			   const event_type&  i, const event_type& j ) const;

  bool get_separation_indexes( index_type& index_i, 
			       index_type& index_j,
			       const event_type&  i, 
			       const event_type& j ) const;

  bool get_freshest_index( index_type& index_i, 
			   const event_type&  i ) const;

public:
  POSET( const untimed_state& untimed_st, const tel& t );
  POSET( const POSET& p );
  ~POSET();
  POSET& operator=( const POSET& p );
  void constrain_lower_bounds( const tel& t, const rule_set& Rb, 
			       event_type enabled );
  void constrain_lower_bounds( const tel& t, const rule_set& Rb, 
			       event_type enabled,
			       const enabled_set& Een );
  void add( const enabled_set& Een, const tel& t );
  void add( const rule_set& Rb, const tel& t );
  size_type get_size() const;
  event_type get_event( index_type i ) const;
  void set_min_separation( event_type e, event_type f, bound_type v );
  void set_max_separation( event_type e, event_type f, bound_type v );
  bound_type get_causal_separation( event_type e, event_type f ) const;
  bound_type get_separation( event_type e, event_type f ) const;
  bound_type** get_dbm() const;
  clockkeyADT make_clockkey() const;
  bool is_consistent( unsigned int added = 1 );
  // Removes duplicated entries in the POSET starting with oldest events
  // first.
  void remove_duplicates();
  void remove_non_causal( const event_set& E );
  void clear_separations_on_rows( unsigned int added );
  void set_order( event_type e, event_type f );
  bound_type get_max_over( event_type e, unsigned int added );
  bool is_in_POSET( event_type e ) const;
  // Only considers events in the POSET that are indexes before (size-added).
  // The events in (size-added) are those added before those in (added).
  bool is_in_POSET( event_type e, unsigned added ) const;
  ostream& dump( ostream& s, const tel& t ) const;
};
//
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
class timed_state {
protected:
  untimed_state* ptr_untimed_st;
  POSET* ptr_Me;
  marking_index rule;  // rule assignment creating this timed_state
public:
  timed_state( const tel& t, const untimed_state& untimed_st ); 
  timed_state( const untimed_state& untimed_st,
	       const POSET& Me,
	       const marking_index& rule );
  ~timed_state();
  const untimed_state& get_untimed_st() const;
  const POSET& get_POSET() const;
  bool is_consistent( unsigned int check = 1 );
  void remove_non_causal( const event_set& E );
  void remove_duplicates();
  marking_index get_rule() const;
  void constrain( event_type e, event_type f, POSET::bound_type v );
  void set_order( event_type e, event_type f );
  bool is_marked( int marking_index ) const;
  bool is_level_matched( const tel& t        ,
			 const event_type& e   ) const;
  ostream& dump( ostream& s, const tel& t ) const;
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef list<timed_state*> timed_st_set;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class timed_state_space {
public:

  typedef pair<const timed_state* const, const timed_state* const> 
          timed_state_pair;
  typedef pair<int,int> event_pair;

protected:

// gcc296 method
//   ostdiostream log_stream;
  FILE* log_stream;
  FILE* fp_rsg;
  stateADT *state_table;
  bddADT bdd_state;
  timeoptsADT timeopts;
  bool verbose;
  bool use_bdd;

  cpu_time_memory_tracker cpu_stats; // tracks time and memory ussage
  unsigned int zone_count;           // tracks the zone count;
  unsigned int state_count;          // tracks untimed state count
  unsigned int iter_count;           // Counts the number of calls to add
  unsigned int max_stack_depth;      // Max stack depth seen in exploration
  unsigned int frequency;            // How often it reports

protected:

  bool update_stats( int new_state );

public:

  timed_state_space( FILE* lg,
		     unsigned int frequency,
		     const tel& t,
		     stateADT *state_table,
		     timeoptsADT timeopts,		      
		     bddADT bdd_state, 
		     bool use_bdd,
		     bool verbose );

  ~timed_state_space();

  bool add( const tel& t, 
	    const timed_state& timed_st );

  bool add( const tel& t,
	    const timed_state& current_st,
	    const timed_state& next_st );

  // This function connect an error state following current_st.  The error
  // state is used to generate a correct trace from the error state to 
  // the beginning of the system.
  void error( const tel& t,
	      const timed_state& current_st,
	      const event_type& f );

  bool update( ostream& s, unsigned int stack_depth );

  void report( char* fname );

  ostream& dump( ostream& s, const tel& t );

};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
bool bap_rsg( tel& t                ,
	      stateADT *state_table ,
	      timeoptsADT opts      ,		      
	      bddADT bdd_state      , 
	      bool use_bdd          ,
	      bool verbose          ,
	      bool noparg             );
//
//////////////////////////////////////////////////////////////////////////
#endif
