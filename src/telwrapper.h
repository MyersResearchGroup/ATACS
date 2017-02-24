//////////////////////////////////////////////////////////////////////////
// @name wrapper for tel object that is a collection of various structs.
// @version 0.1 alpha
//
// (c)opyright 2000 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#ifndef __TELWRAPPER_H__
#define __TELWRAPPER_H__

#include <set>
#include <algorithm>
#include <list>

#include "struct.h"

//------------------------------------------------------------------------
// Used to denote a missing or unfound entry in a list.
#define _NOT_FOUND_      -1
// -----------------------------------------------------------------------

//------------------------------------------------------------------------
// TODO: port all of the typedef's from bap.h to this file.  
typedef unsigned int event_type;
// The default allocator for STL used in sets (actually anything based
// on a tree structure) will initialize enough memory for 20 entries
// in the set.  This confuses memory analysis (ccmalloc) and causes to
// report leaked memory.  Using malloc_alloc solves this issue.  Anything
// based on a tree is only allocated the exact size it requests.
#ifdef __GCC_OLD_OPT__
typedef set<event_type,less<event_type>,malloc_alloc> event_set;
#else
typedef set<event_type,less<event_type> > event_set;
#endif
//------------------------------------------------------------------------

//------------------------------------------------------------------------
struct less_cardinality : 
public binary_function<event_set*,event_set*,bool> {
  bool operator()( event_set* __x , 
		   event_set* __y   ) const { 
   return( ( __x->size() < __y->size() )               ||
           ( __x->size() == __y->size() && __x < __y )    );
  }
};

template <class _Container>
inline insert_iterator<_Container> inserter(_Container& __x) {
  return insert_iterator<_Container>(__x, __x.begin() );
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
#ifdef __GCC_OLD_OPT__
typedef set<event_set*,less_cardinality,malloc_alloc> preset_list;
#else
typedef set<event_set*,less_cardinality> preset_list;
#endif
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#ifdef __GCC_OLD_OPT__
typedef list<event_type,malloc_alloc> _event_list;
#else
typedef list<event_type> _event_list;
#endif
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//
class tel {
public:

  typedef ::event_type event_type;

protected:

  enum status_type { bad, good };
  typedef char* marking_type;
  typedef vector<event_set> event_set_cache;
  typedef vector<preset_list> preset_list_cache;

public:
  
  char * filename;
  signalADT *signals;
  eventADT *events;
  ruleADT **rules;
  markkeyADT *markkey;
  int nevents;
  int ninpsig;
  int nsigs;
  int nrules; 
  int* ncausal;
  int* nord;
  char * startstate;
  int ndummy;
  status_type status;
  bool levels;  
  bool verify;

public:

  event_set_cache conflict_cache;
  preset_list_cache preset_cache;

  // This is to support levels.  The idea is to expand the level
  // expressions into rules.
  ruleADT **_M_level_rules;
  unsigned _M_markkey_size;

  vector<list<pair<event_type,event_type> > > _M_disable_conflict;
  vector<list<pair<event_type,event_type> > > _M_disjunctive_conflict;
  

protected:

  void insert_and_absorb( const event_set& Ec  ,
			  preset_list& P   );

  bool all_input_output_conflict( const event_set& Ec ,    
				  const event_type& e   );

  bool all_disjunctive_or_same_action( const event_set& Ec       , 
				       const event_type& e       ,
				       const event_type& enabled   );

  void get_preset_list( event_set& E              ,  
			event_set& Ec             ,
			preset_list& P      ,
			const event_type& enabled ,
			bool level                  );

  void update_marking_key( unsigned rule_count );

  void set_conflict_relations( const preset_list& P      ,
			       const event_type& enabled   );
  
  unsigned create_level_rules( int lower          ,
			       int upper          ,
			       level_exp exp      ,
			       event_type enabled   );
  unsigned create_level_rules();

public:

  tel();
  tel( char* filename,
       signalADT *signals,
       eventADT *events,
       ruleADT **rules,
       markkeyADT *markkey,
       int nevents,
       int ninpsig,
       int nsigs,
       int nrules,
       int* ncausal,
       int* nord,
       char* startstate,
       int ndummy,
       bool level,
       bool verify);

  ~tel();

  char* get_fname() const;
  signalADT* get_signals() const;
  ruleADT** get_rules() const;
  markkeyADT* get_markkey() const;
  eventADT* get_events() const;
  char* get_event( int i ) const;
  bool is_immediate( int i ) const;
  char* get_startstate() const;
  int number_events() const;
  int marking_size() const;
  unsigned level_expanded_marking_size() const;
  bool is_input_conflict( const int& i, const int& j ) const;
  bool is_output_conflict( const int& i, const int& j ) const;
  int get_ninpsig() const;
  int get_nsigs() const;
  bool good_tel() const;
  int enabling( const int& marking_index ) const;
  int enabled( const int& marking_index ) const;
  bool is_behavior_rule( const int& i, const int& j ) const;
  bool is_constraint_rule( const int& i, const int& j ) const;
  bool is_disabling_rule( const int& i, const int& j ) const;
  int marking_index( const int& i, const int& j ) const;
  void common_enabling( const int& a, const int& b,
			insert_iterator<event_set> Ecom ) const;
  int common_enabling( const int& a, const int& b ) const;
  int upper_bound( const int& e, const int& f ) const;
  int lower_bound ( const int& e, const int& f ) const;
  int get_signal( int i ) const;
  bool is_dummy_event( const int& event ) const;
  bool is_sequencing_event( const int& event ) const;
  bool has_levels() const;

  //----------------------------------------------------------------------
  // NOTE: An expression only exists if the expression is not TRUE or FALSE
  // (i.e., an expression that is always true is equivalent to no 
  // expression and an expression that is always false is treated as no 
  // expression too).
  // This function should not be used to test for a rule being enabled.
  // A false expression will prevent a rule from ever being enabled.
  // The level_satisfied function must be called for all rules regardless 
  // of the presence or absence of a level expression to check for 
  // enabledness.
  //----------------------------------------------------------------------
  bool has_level_expression( const int& e, const int& f ) const;

  level_exp get_expression( int i, int j ) const;
  bool is_visible( const int& e ) const;
  const event_set& get_conflict_set( const int& e ) const;
  const preset_list& get_preset_list( const int& e ) const;
  unsigned level_rule_index( const event_type& e, 
			     const event_type& f ) const;
  bool is_level_rule( const event_type& e,
		      const event_type& f ) const;
  bool is_level_rule( const int& i ) const;
  bool is_disjunctive_conflict( const event_type& e       ,
				const event_type& f       ,
				const event_type& enabled   );
  bool is_disjunctive( const event_type& tmp, 
		       const event_type& e, 
		       const event_type& enabled );
  bool check_complete( const event_set& Ec       , 
		       const event_type& enabled );
  bool is_rising_event( const event_type& e ) const;
  bool is_falling_event( const event_type& e ) const;
  bool is_same_action( const event_type& a, const event_type& b ) const;
  bool is_unbounded( const event_type& e, const event_type& f ) const;
  bool is_marked_disabled( const event_type& e ) const;
  void mark_disabled( const event_type& e );
};
//
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Reports error if a TEL is not correct or complete.
bool report_incomplete_tel( char* filename );
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Prints the Rf vector in a human readable form.
//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const char* Rf  );

//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const event_set& E );
//------------------------------------------------------------------------

//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const _event_list& E );
//------------------------------------------------------------------------

//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const preset_list& p );
//------------------------------------------------------------------------

#endif
