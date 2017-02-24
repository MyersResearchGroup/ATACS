//////////////////////////////////////////////////////////////////////////
// @name timed_state_space.cc => Code that implements timed_state_space 
//       class defined in bap.h
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////

#include "bap.h"
#include "bag.h"
#include "memaloc.h"
#include "def.h"
#include "findrsg.h"
#include "common_timing.h"
#include <sstream>

using namespace std;

//////////////////////////////////////////////////////////////////////////
// new_state determines how the state_count and zone_count are updated.
// Here is a table describing the meaning and allowed values of new_state:
//
//     new_state < 0 : timeopts.supersets is set to true and in adding
//                     the last state, several zones were removed from
//                     the state table because of a superset relationship.
//                     Update the zone_count to reflect that zones have
//                     been removed from the state space.
//     new_state == 1: A new untimed state has been added to the 
//                     state space with the accompanying zone.  Update the
//                     zone_count and state_count to reflect this.
//     new_state > 0 : A new zone has been added to an existing untimed 
//                     state.  Update zone_count accordingly.
//     new_state == 0: The timed state has already been explored.
//
//////////////////////////////////////////////////////////////////////////
bool timed_state_space::update_stats( int new_state ) {
  if ( new_state == 0 ) 
    return( false );
  if( new_state > 0 ){
    ++zone_count;
  } else if ( timeopts.supersets ) {
    zone_count = zone_count + new_state + 1;
  }
  if( new_state == 1 ){
    ++state_count;
  }
  return( true );
}

timed_state_space::timed_state_space( FILE* lg,
				      unsigned int frequency,
				      const tel& t,
				      stateADT *state_table,
				      timeoptsADT timeopts,		      
				      bddADT bdd_state, 
				      bool use_bdd,
				      bool verbose ) :   
  log_stream( lg ),
  fp_rsg( NULL ),
  state_table( state_table ),
  bdd_state( bdd_state ),
  timeopts( timeopts ),
  verbose( verbose ),
  use_bdd( use_bdd ),
  cpu_stats(),
  zone_count( 0 ),
  state_count( 0 ),
  iter_count( 0 ),
  max_stack_depth( 0 ),
  frequency( frequency ) {
  // Open file for untimed states according to verbose flag.
  fp_rsg = open_rsg_file( t, verbose );

  // Initialize memory for the state table.
  initialize_state_table(LOADED,FALSE,state_table);
}

timed_state_space::~timed_state_space() {
  if ( fp_rsg != NULL )
    fclose( fp_rsg );
}

bool timed_state_space::add( const tel& t, 
			     const timed_state& timed_st ) {
  // Update the number of calls;
  ++iter_count;
  const POSET* const zone = &(timed_st.get_POSET());
  clockkeyADT clockkey = zone->make_clockkey();
  untimed_state untimed_st( timed_st.get_untimed_st() );

  int status = add_state(fp_rsg, 
			 state_table, 
			 NULL,
			 NULL,
			 NULL,
			 NULL,
			 NULL,
			 0,
			 NULL,
			 untimed_st.get_state(),
			 untimed_st.get_marking(), 
			 untimed_st.get_enablings(),
			 t.get_nsigs(), 
			 (clocksADT)(zone->get_dbm()), 
			 clockkey, 
			 zone->get_size(), 
			 state_count,
			 verbose, 
			 1, 
			 t.marking_size(), 
			 zone->get_size(), 
			 zone->get_size(),
			 bdd_state, 
			 use_bdd,
			 t.get_markkey(),
			 timeopts,
			 -1, 
			 -1, 
			 untimed_st.get_marking(),
			 t.get_rules(), 
			 t.number_events(),
			 true ); // Designates the call as comming from bap!
  
  delete [] clockkey;
  return( update_stats( status ) );

}

bool timed_state_space::add( const tel& t,
			     const timed_state& current_st,
			     const timed_state& next_st ) {
  // Update then number of calls;
  ++iter_count;

  const POSET* current_POSET = &(current_st.get_POSET());
  const POSET* next_POSET = &(next_st.get_POSET());

  clockkeyADT current_clockkey = current_POSET->make_clockkey();
  clockkeyADT next_clockkey = next_POSET->make_clockkey();
 
  untimed_state current_untimed( current_st.get_untimed_st() );
  untimed_state next_untimed( next_st.get_untimed_st() );

  marking_index rule = next_st.get_rule();

  int status =  add_state(fp_rsg, 
  			  state_table, 
  			  current_untimed.get_state(),
			  current_untimed.get_marking(), 
			  current_untimed.get_enablings(),
			  (clocksADT)(current_POSET->get_dbm()), 
			  current_clockkey, 
			  current_POSET->get_size(), 
			  NULL,
			  next_untimed.get_state(),
			  next_untimed.get_marking(), 
			  next_untimed.get_enablings(),
			  t.get_nsigs(), 
			  (clocksADT)(next_POSET->get_dbm()), 
			  next_clockkey, 
			  next_POSET->get_size(), 
			  state_count,
			  verbose, 
			  1, 
			  t.marking_size(), 
			  current_POSET->get_size(), 
			  next_POSET->get_size(),
			  bdd_state, 
			  use_bdd,
			  t.get_markkey(),
			  timeopts,
			  t.enabling( rule ),
			  t.enabled( rule ), 
			  NULL,
			  t.get_rules(), 
			  t.number_events(),
			  true ); // Designates the call as comming from bap!

#ifdef __RSG__
  cout << "s: " << next_untimed.get_state() << endl;
  cout << "e: " << current_untimed.get_state() 
       << " -> " << next_untimed.get_state() << endl;
#endif

  delete [] current_clockkey;
  delete [] next_clockkey;

  return( update_stats( status ) );
    
}

// This function connect an error state following current_st.  The error
// state is used to generate a correct trace from the error state to 
// the beginning of the system.
// NOTE: This is simply add state with all of the next_st information
// set to 0 or NULL as apropriate.
void timed_state_space::error( const tel& t,
			       const timed_state& current_st,
			       const event_type& f ) {
  // Update then number of calls;
  ++iter_count;

  const POSET* current_POSET = &(current_st.get_POSET());
  clockkeyADT current_clockkey = current_POSET->make_clockkey();
  untimed_state current_untimed( current_st.get_untimed_st() );

  add_state(fp_rsg, 
	    state_table, 
	    current_untimed.get_state(),
	    current_untimed.get_marking(), 
	    current_untimed.get_enablings(),
	    (clocksADT)(current_POSET->get_dbm()), 
	    current_clockkey, 
	    current_POSET->get_size(), 
	    NULL,
	    NULL,
	    NULL, 
	    NULL,
	    t.get_nsigs(), 
	    NULL, 
	    NULL, 
	    0, 
	    state_count,
	    verbose, 
	    1, 
	    t.marking_size(), 
	    current_POSET->get_size(), 
	    0,
	    bdd_state, 
	    use_bdd,
	    t.get_markkey(),
	    timeopts,
	    0,
	    f, 
	    NULL,
	    t.get_rules(), 
	    t.number_events(),
	    true ); // Designates the call as comming from bap!
  
  delete [] current_clockkey;
}
  
void timed_state_space::report( char* fname ) {
// gcc296 method
//   FILE* lg = (log_stream.rdbuf())->stdiofile();
  FILE* lg = log_stream;
  cpu_stats.mark();
  double process_time = (cpu_stats.resource_time()).first;
  ::report( lg, 
	    fname,
	    zone_count, 
	    state_count, 
	    max_stack_depth,
	    cpu_stats.max_memory(),
	    process_time );
}

bool timed_state_space::update( ostream& s, unsigned int stack_depth ) {
  if ( stack_depth > max_stack_depth ) {
    max_stack_depth = stack_depth;
  }
  if ( ( zone_count % frequency ) == 0 ) {
    s << iter_count << " iterations, "
      << zone_count << " zones, "
      << state_count << " states "
      << "(stack_depth = " << stack_depth 
      << ")" << endl;
//gcc296 method
//     log_stream << iter_count << " iterations, "
// 	       << zone_count << " zones, "
// 	       << state_count << " states "
// 	       << "(stack_depth = " << stack_depth 
// 	       << ")" << endl;
    ostringstream oss;
    oss << iter_count << " iterations, "
        << zone_count << " zones, "
        << state_count << " states "
        << "(stack_depth = " << stack_depth 
        << ")" << endl;
    fprintf(log_stream,oss.str().c_str());
    cpu_stats.mark();
    cpu_stats.report_memory( s ) << endl;
//gcc296 method
//     cpu_stats.report_memory( log_stream ) << endl;
    ostringstream oss1;
    cpu_stats.report_memory(oss1) << endl;
    fprintf(log_stream,oss1.str().c_str());
    return( true );
  }
  return( false );
}

ostream& timed_state_space::dump( ostream& s, const tel& t ) {
  
  stateADT e;
  clocklistADT region;

  for( int i=0 ; i < PRIME ; i++ ){
    if( state_table[i]->state != NULL ){
      e = state_table[i];
      while( e->state != NULL ){
	s << "s(" << e->number << "): " << e->state << endl;
	s << "Rm: ";
	::dump( s, t, e->marking ) << endl;
	region = e->clocklist;
	while( region != NULL ){
	  int number_events = region->clocknum;
	  processkeyADT pkey = new processkey[number_events];
	  for ( int j = 0 ; j < number_events ; ++j ) {
	    pkey[j].eventnum = (region->clockkey[j]).enabled;
	  }
	  ::dump( s, number_events, pkey, region->clocks, t.get_events(), true );
	  region=region->next;
	  delete pkey;
	}	
	e = e->link;
	s << endl;
      }
    }
  }
  return( s );
}
//////////////////////////////////////////////////////////////////////////
