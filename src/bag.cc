////////////////////////////////////////////////////////////////////////////
// @name Bourne Again timing analysis
// @version 0.1 alpha
//
// (c)opyright 2000 by Eric G. Mercer
//
// @author Eric G. Mercer
///////////////////////////////////////////////////////////////////////////
#include <utility>
#include <algorithm>
#include <limits.h>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "bag.h"
#include "geom.h"
#include "findreg.h"
#include "findrsg.h"
#include "memaloc.h"
#include "def.h"
#include "common_timing.h"
#include "approx.h"
#include "error.h"

#define MIN_CLOCK_SIZE    5

/////////////////////////////////////////////////////////////////////////////
//
FILE* open_rsg_file( const tel& t, bool verbose ) {
  FILE* fp_rsg = NULL;
  char rsg_name[FILENAMESIZE];
  if ( verbose ) {
    strcpy( rsg_name, t.get_fname() );
    strcat( rsg_name, ".rsg" );
    fp_rsg = fopen( rsg_name, "w" );
    if ( fp_rsg == NULL ) {
      throw( new file_error( rsg_name, file_error::write ) );
    }
    printf( "Finding reduced state graph and storing to:  %s\n", rsg_name );
    fprintf(lg, "Finding reduced state graph and storing to:  %s\n", rsg_name);
    fprintf( fp_rsg, "SG:\n" );
    print_state_vector(fp_rsg, t.get_signals(), t.get_nsigs(), 
		       t.get_ninpsig() );
    fprintf( fp_rsg, "STATES:\n" );
    return( fp_rsg );
  }
//    printf( "Finding reduced state graph ... " );
//    fflush( stdout );
//    fprintf( lg,"Finding reduced state graph ... " );
//    fflush( lg );
  return( fp_rsg );
}
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// check_merge_conflicts throws an exception if the passed in marking has
// both multiple branches on a conflicting merge point marked!
//////////////////////////////////////////////////////////////////////////
void check_merge_conflicts( const tel& t,
			    const char* const marking ) {
  for ( int i = 0 ; i < t.marking_size() ; ++i ) {
    if ( marking[i] != 'F' ) {
      int enabling = t.enabling( i );
      int enabled = t.enabled( i );
      for ( int j = 1 ; j < t.number_events() ; ++j ) {
	if ( j != enabling                    &&
	     t.is_behavior_rule( j, enabled ) &&
	     t.is_input_conflict( j, enabling )        ) {
	  for ( int k = 0 ; k < t.marking_size() ; ++k ) {
	    if ( marking[k] != 'F'    &&
		 t.enabling( k ) == j    ) {
	      //throw( new error("WARNING: marking includes 2 events that conflict at a merge place" ) ); /* CAUSING WIERD CHARACTERS TO SCREEN. */
	      
	    }
	  }
	}
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
clock_zone::clock_zone() : clocks( NULL ),
	       clockkey( NULL ),
	       clocknums_iv( NULL ),
               clock_size( 0 ),
               num_clocks( 0 ),
               nreference( NULL ) {
  // NOTHING TO DO FOR NOW!
}

clock_zone::clock_zone( const clock_zone& z ) : clocks( z.clocks ),
                              clockkey( z.clockkey ),
                              clocknums_iv( z.clocknums_iv ),
			      clock_size( z.clock_size ),
			      num_clocks( z.num_clocks ),
			      nreference( z.nreference ) {
  // NOTHING TO DO
}

clock_zone::clock_zone( const clock_zone& z, const int& marking_size ) 
  : clocks( NULL ),
    clockkey( NULL ),
    clocknums_iv( NULL ),
    clock_size( z.clock_size ),
    num_clocks( z.num_clocks ),
    nreference( z.nreference ){
  clocks = copy_clocks( clock_size, z.clocks );
  clockkey = (clockkeyADT)copy_clockkey( clock_size , z.clockkey );
  clocknums_iv = (clocknumADT)GetBlock((marking_size+1) * (sizeof(int)));
  memcpy( clocknums_iv, z.clocknums_iv, (marking_size * (sizeof(int))));
}

bool clock_zone::is_active( const int& marking_index ) const {
  return( clocknums_iv[marking_index] >= 0 );
}

int clock_zone::max_clock_value( const int& marking_index ) const {
  if ( is_active( marking_index ) ){
    return( clocks[0][clocknums_iv[marking_index]] );
  }
  return( INFIN );
}

int clock_zone::num_active_clocks() const {
  return( num_clocks );
}

void clock_zone::free_memory() {
  for( int i = 0 ; i < clock_size ; i++ ){
    free( clocks[i] );
  }
  free( clocks );
  free( clockkey );
  free( clocknums_iv );
}
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
fired_data::fired_data() : Rf( NULL ),
                           confl_removed( NULL ),
                           fired( NULL ) {
  // NOTHING TO DO
}

fired_data::fired_data( bit_vector exp_enabled_bv, 
			bit_vector confl_removed_bv,
			bit_vector fired_bv ) 
  : Rf( exp_enabled_bv ),
    confl_removed( confl_removed_bv),
    fired( fired_bv ) {
  // NOTHING TO DO
}

fired_data::fired_data( const fired_data& f ) 
   : Rf( f.Rf ),
     confl_removed( f.confl_removed ),
     fired( f.fired ) {
  // NOTHING TO DO
}

bool fired_data::is_fired( const int& marking_index ) const {
  return( Rf[marking_index] != 'F' );
}
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

//////////////////////////////////////////////////////////////////////////
// is_candidate returns true iff the event 'candidate' should be 
// addressed to correctly implement the new conflict semantics.  The event
// 'candidate' must be considered when all of the following are true:
//
//     1) 'candidate' # 'event';
//     2) There exists rules <enabling,candidate> and <enabling,event>;
//     3) The rule r=<enabling,candidate,l,u> has an active clock in z;
//     4) u != INFIN for <enabling,candidate> and <enabling,event>;
//     5) max_clock_value for r in z is not equal to u
//
//////////////////////////////////////////////////////////////////////////
bool is_conflict_candidate( const tel& t,
			    const clock_zone& z,
			    const int& event,
			    const int& candidate ) {
  // If the 2 events do not conflict, then conflict doesn't apply.
  if ( !( t.is_output_conflict( event, candidate ) ) ) {
    return( false );
  }
  // If 'event' and 'candidate' do not share a common enabling, then 
  // 'candidate' can not be an event that needs to be addressed by 
  // the new conflict semantics
  int enabling = t.common_enabling( event, candidate );
  if ( enabling == _NOT_FOUND_ ){
    return( false );
  }
  // Does the rule r=<enabling,candidate> have an active or unbounded 
  // clock?  If yes, then 'candidate' is not an event the needs to be 
  // addressed. If the 1 has a [x,inf] bound, then the other rule must 
  // have a [x,inf] bound too.
  int index = t.marking_index( enabling, candidate );
  int max_clock_value = z.max_clock_value( index );
  int candidate_upper_bound = t.upper_bound( enabling, candidate );
  int event_upper_bound = t.upper_bound( enabling, event );
  if ((event_upper_bound == INFIN && candidate_upper_bound == INFIN)|| 
       (max_clock_value == INFIN) ){
    return( false );
  }
  // Given rule r=<enabling,candidate,l,u>: If the maximum clock value 
  // for r in the given clock_zone z is equal to u, then the event 'candidate' 
  // does not need to dealt with.
  if ( max_clock_value == candidate_upper_bound ) {
    return( false );
  }
  // All conditions are satisfied.  This implies that the event 
  // 'candidate' must be handled in the new conflict semantics.
  return( true );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// is_active_or_fired returns true IFF a rule is either in the Rf list, in
// the clock_zone as an active clock, or in given fire_list.  If it is 
// not found in any of these locations, false is returned.
//////////////////////////////////////////////////////////////////////////
bool is_active_or_fired( const tel& t, 
			 const clock_zone& z,
			 const fired_data& Rf,
			 firinglistADT fire_list,
			 const int& enabling_rule ) {
  // Is the rule fired in the Rf information?
  if ( Rf.is_fired( enabling_rule ) ) return( true );

  // Does the rule have an active clock in the clock_zone?
  if ( z.is_active( enabling_rule ) ) return( true );

  // Does the rule exist in the firing_list?  (This covers rules with
  // [0,inf] bounds that would not have an active clock in the clock_zone).
  firinglistADT ptr_fire = fire_list;
  while ( ptr_fire != NULL ) {
    if ( ptr_fire->enabling == t.enabling( enabling_rule ) &&
	 ptr_fire->enabled == t.enabled( enabling_rule ) ) {
      return( true );
    }
    ptr_fire = ptr_fire->next;
  }
  
  // This rule is not satisfied or fired.
  return( false );

}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// is_untimed_enabled returns true IFF a given event has enough rules that
// are either enabled, satisifed, or fired to allow it to fire now or 
// in the future (i.e., it guarantees that an event will be able to fire 
// because the event has a sufficient set of enabled rules).  Enabled means
// that the rule is marked, and its boolean expression evaluates to true.
//////////////////////////////////////////////////////////////////////////
bool is_untimed_enabled( const tel& t,
			 const clock_zone& z,
			 const fired_data& Rf,
			 firinglistADT fire_list,
			 const int& event ) {
  // foreach behavior rule that enables the event:
  // Ignoring reset event 0
  for ( int i = 1 ; i < t.number_events() ; ++i ) {
    if ( t.is_behavior_rule( i, event ) ) {
      // Is this rule fired or active? Active is enabled or satisified.
      int enabling_rule = t.marking_index( i, event );
      if ( !( is_active_or_fired( t, z, Rf, fire_list, enabling_rule ) ) ) {
	bool another_rule = false;
	// Is there another rule that is enabled, satisfied, or fired
	// that enables event has an enabling event that conflicts with i.
	// Ignoring reset event 0
	for ( int j = 1 ; j < t.number_events() ; ++j ) {
	  if ( t.is_behavior_rule( j, event ) && 
	       t.is_input_conflict( j, i ) ) {
	    enabling_rule = t.marking_index( j, event );
	    if ( is_active_or_fired( t, z, Rf, fire_list, enabling_rule ) ) {
	      another_rule = true;
	      break;
	    }    
	  }
	}
	// Another rule that has an enabling conflict with <i,event> 
        // was not found, so this event does NOT have enough tokens to 
        // allow it to fire.
	if ( !another_rule ) { return( false ); }
      }
    }
  }
  // All rules enabling this event are active or fired, so this event 
  // could fire now or in the future.
  return( true );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// has_untimed_conflict returns true IFF there is another 
// conflicting event that conflicts with the passed in event that has a 
// sufficient set of  enabled rules at this instant to allow it to fire at 
// some time.  If this case does not exist, then false is returned.
//////////////////////////////////////////////////////////////////////////
bool has_untimed_conflict( const tel& t, 
			   const clock_zone& z, 
			   const fired_data& Rf,
			   firinglistADT fire_list,
			   const int& event ) {
  // foreach event that conflicts with this event:
  // Ignoring reset event 0
  for ( int i = 1 ; i < t.number_events() ; ++i ) {
    // Is this potential candidate for the new conflict semantics address?
    if ( is_conflict_candidate( t, z, event, i ) ) {
      // If the conflict has enough enabled rules to allow it to fire
      // at some time, then return true.
      if ( is_untimed_enabled( t, z, Rf, fire_list, i ) ) {
	return( true );
      } 
    } 
  }
  // There is not a conflicting event that has a sufficient set of tokens
  // at this instant to allow it to fire at sometime.
  return( false );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
bool bag_rsg( tel& t,
	      stateADT *state_table,
	      timeoptsADT timeopts,		      
	      bddADT bdd_state, 
	      bool use_bdd,
	      bool verbose,
	      bool noparg ) {

  clock_zone cur;
  clock_zone old;

  markingADT marking = NULL;
  markingADT old_marking = NULL;
  firinglistADT firinglist = NULL;
  firinglistADT temp = NULL;
  geom_workstackADT workstack=NULL;
  geom_workADT current_work=NULL;
  int fire_enabling;
  int fire_enabled;
  bool event_fired=0;
  int sn=0;
  int new_state;
  int regions=0;
  exp_enabledADT exp_enabled_bv = NULL;
  exp_enabledADT old_exp_enabled_bv = NULL;
  confl_removedADT confl_removed_bv = NULL;
  rule_contextADT context_pv = NULL;
  firedADT fired_bv = NULL;
  int old_clock_size = 0;
  int stack_depth=0;
  int stack_removed = 0;
  bool ordering=0;
  int pushed=0;
  int iter=0;
  int failing_rule = 0;
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
#else
  malloc_statistics_t stats;
#endif
#endif

  FILE* fp_rsg = NULL;
  // Check that this TEL is sensible on the surface.
  if ( !( t.good_tel() ) ) return( report_incomplete_tel( t.get_fname() ) );

  try {
    
    // Open file for untimed states according to verbose flag.
    fp_rsg = open_rsg_file( t, verbose );
    
    initialize_state_table(LOADED,FALSE,state_table);
    
    // Mark the time
    cpu_time_tracker time;

    init_rule_info(t.rules, t.nevents, t.nrules, 
		   &exp_enabled_bv, &confl_removed_bv,
		   &fired_bv, &(cur.clocknums_iv), &context_pv,
		   t.ncausal, t.nord);
    old_exp_enabled_bv = (exp_enabledADT)GetBlock((t.nrules+1) * sizeof(char));
    strcpy(old_exp_enabled_bv, exp_enabled_bv);
    marking = (markingADT)find_initial_marking(t.events, t.rules, t.markkey, 
					       t.nevents, 
					       t.nrules, t.ninpsig, t.nsigs, 
					       t.startstate, 
					       verbose);
    if(marking==NULL){
      add_state(fp_rsg, state_table, NULL, NULL, NULL, NULL,
		NULL, 0, NULL, NULL, NULL, NULL, t.nsigs, NULL, NULL,0, 0, 
		verbose, TRUE, t.nrules, old_clock_size, 0,
		bdd_state, use_bdd,t.markkey,timeopts, -1, -1, exp_enabled_bv,
		t.rules, t.nevents);
      if(fp_rsg){
	fclose(fp_rsg);
      }
      return FALSE;
    }
    
    // Throws exception if any 2 branches on a conflict merge are marked.
    try {
      check_merge_conflicts( t, marking->marking );
    }
    catch( error* ptr_e ) {
      ptr_e->print_message( lg );
      delete ptr_e;
    }

    cur.clock_size=mark_rules(t.rules, t.markkey, marking, t.nrules)+2;
    if(cur.clock_size<MIN_CLOCK_SIZE) cur.clock_size=MIN_CLOCK_SIZE;
    cur.clocks = create_clocks(cur.clock_size);
    cur.clockkey = create_clockkey(cur.clock_size);
    old_clock_size=cur.clock_size;

    cur.num_clocks=geom_mark_initial_region(t.rules,cur.clocks,marking,
					    cur.clockkey,
					    t.nevents, t.nrules, 
					    cur.clocknums_iv, 
					    t.nsigs,
					    timeopts.infopt);
    normalize(cur.clocks, cur.num_clocks, cur.clockkey, 
	      t.rules, FALSE, NULL, 0);
    add_state(fp_rsg, state_table, NULL, NULL, NULL, NULL,
	      NULL, 0, NULL, marking->state,marking->marking,
	      marking->enablings,
	      t.nsigs, cur.clocks, cur.clockkey, cur.num_clocks, 
	      sn, verbose, 1, t.nrules,
	      old_clock_size, cur.clock_size,bdd_state, use_bdd,
	      t.markkey,timeopts,
	      -1, -1, exp_enabled_bv, t.rules, t.nevents);
    sn++;
    regions++;

    /*Mark this rule as exp_enabled */
    old.clocks=copy_clocks(cur.num_clocks+2, cur.clocks);
    old.clockkey=copy_clockkey(cur.num_clocks+2, cur.clockkey);
    old.num_clocks=cur.num_clocks;
    old.clock_size=cur.num_clocks+2;
    strcpy(old_exp_enabled_bv, exp_enabled_bv);

    firinglist = (firinglistADT)get_t_enabled_list(t.rules, cur.clocks, 
						   cur.clockkey, 
						   marking, 
						   t.markkey, exp_enabled_bv, 
						   cur.clocknums_iv,
						   confl_removed_bv,
						   &(cur.num_clocks), 
						   t.nevents, 
						   t.nrules, 
						   timeopts.interleav);
#ifdef __DUMP__
  cout << "Rs: ";
  dump( cout, firinglist, t.get_events() ) << endl << endl;
#endif
    
    /* Main orbits loop, exits when there are no unexplored states */
    while(1){ 

      iter++;
      
      // Make sure the system is not in a deadlock state
      if(firinglist==NULL){
	printf("System deadlocked in state %s\n", marking->state);
	if(fp_rsg){
	  fclose(fp_rsg);
	}
	return FALSE;
      }
      
      // Check all constraint rules that they have not exceeded their
      // upper bounds.  If they have, flag an error and return.
      failing_rule = check_maximums(t.rules, cur.clocks, cur.clockkey, 
				    cur.num_clocks);
      if(failing_rule!=0){
	printf("Verification failure: \n");
	printf("Constraint rule [%s,%s] exceeded its upper bound. \n", 
	       t.events[cur.clockkey[failing_rule].enabling]->event, 
	       t.events[cur.clockkey[failing_rule].enabled]->event);
	if(fp_rsg){
	  fclose(fp_rsg);
	}
	return FALSE;
      }
      
      // Pop a rule off the front of the firing list.
      fire_enabling=firinglist->enabling;
      fire_enabled=firinglist->enabled;     

      // If the rule has a [0,inf] bound and a NULL level,
      // then ordering is set high.
      if((t.rules[fire_enabling][fire_enabled]->lower==0) && 
	 (t.rules[fire_enabling][fire_enabled]->upper==INFIN) &&
	 (t.rules[fire_enabling][fire_enabled]->level==NULL) &&
	 (timeopts.infopt)) ordering=1;
      else ordering=0;
      
      // Save the head of the firing list to delete later and move to the
      // next entry.  merge_conflicting_rules is an optimization that 
      // removes unnecessary rule interleavings on free_choice places where
      // the shared branches of <fire_enabling,fire_enabled> are not causal
      // to their enabled events.
      temp=firinglist;
      firinglist=firinglist->next;
      firinglist=merge_conflicting_rules(cur.clockkey, cur.clocks, firinglist,
					 cur.num_clocks, t.nevents, 
					 exp_enabled_bv, 
					 t.nrules, t.rules, fire_enabling, 
					 fire_enabled, cur.clocknums_iv);
      cur.num_clocks =  cur.num_clocks - 
	geom_conflict_fired_rules(t.rules, cur.clocks, 
				  cur.clockkey, 
				  t.nevents,  cur.clocknums_iv,
				  fired_bv,
				  fire_enabling, 
				  fire_enabled, cur.num_clocks);
      free(temp);
      pushed=0;
      /*    printf("firing rule {%d,%d}\n", fire_enabling, fire_enabled);  */
      /* If there are unfired events, put this state and firinglist */
      /* on the stack to explore later.                            */
      
      // Push the rest of the firing_list on the stack to be handled later.
      if(firinglist!=NULL){
	geom_push_work(&workstack, marking, firinglist, cur.clocks, 
		       cur.clockkey, cur.num_clocks, cur.clock_size, t.nrules, 
		       exp_enabled_bv,
		       confl_removed_bv, fired_bv, cur.clocknums_iv);   
	stack_depth++;
	pushed=1;
      } 
      
      // Mark the rule <fire_enabled,fire_enabled> as fired in Rf!
      exp_enabled_bv[t.rules[fire_enabling][fire_enabled]->marking]='T';   
      
      // Project out the clock for the fired rule iff the rule was not
      // an ordering rule or a [0,inf] rule.
      if(!ordering){
	restrict_region(t.rules, cur.clocks, cur.clockkey, 
			fire_enabling,fire_enabled,
			cur.num_clocks, 0, 0, cur.clocknums_iv);
	cur.num_clocks=geom_project_clocks(t.rules, cur.clocks, cur.clockkey, 
					   t.nevents, 
					   cur.clocknums_iv, 
					   fired_bv,fire_enabling, 
					   fire_enabled, cur.num_clocks,-1);
      }

      if(timeopts.genrg){
	old_marking=marking;
      }

#ifdef __DUMP__
      cout << "Fired rule: (" << t.events[fire_enabling]->event << ","
	   << t.events[fire_enabled]->event << ") -> " << endl;
      cout << "zone: " << cur.num_clocks << endl;
      dump( cout, cur.num_clocks, cur.clockkey, t.events ) << endl;
      dump( cout, cur.clocks, cur.num_clocks) << endl;      
#endif

      // Does firing <fire_enabling,fire_enabled> enable an event to fire?
      if(bv_enabled_event(fire_enabled, fire_enabling, t.rules, t.nevents,
			  exp_enabled_bv)){
#ifdef __DUMP__
	// Output the timed state at this point where the event is firing.
	cout << endl << "Firing: " << t.events[fire_enabled]->event 
	     << "-> " << endl;
	cout << "Rc: (" << t.events[fire_enabling]->event << ","
	     << t.events[fire_enabled]->event << ")" << endl;
	cout << "s: " << marking->state << endl << "Rm: ";
	dump( cout, t.nrules, marking->marking, t.markkey, t.events ) << endl;
	cout << "Rf: ";
	dump( cout, t.nrules, exp_enabled_bv, t.markkey, t.events ) << endl;
#endif
	// New Conflict Semantics: If there is an event that conflicts
	// with fire_enabled, and it has a sufficient set of enabled rules
        // at this instant that it could fire at sometime, then push onto 
	// the stack this frame showing fire_enabled unfired.  When this 
        // frame is popped from the stack, the path where the conflicting 
        // event fires will be explored.
	// NOTE: I moved restrict and project to be above this function so I
	// have the fired rule already projected from the current clock_zone.
	fired_data Rf( exp_enabled_bv, confl_removed_bv, fired_bv );
	if ( has_untimed_conflict( t, cur, Rf, firinglist, fire_enabled ) ) {
#ifdef __DUMP__
	  cout << endl << "******* " << t.get_event(fire_enabled) 
	       << " HAS UNTIMED CONFLICT *******" << endl;
#endif

	  /*Mark this rule as exp_enabled */
	  old.clocks=copy_clocks(cur.num_clocks+2, cur.clocks);
	  old.clockkey=copy_clockkey(cur.num_clocks+2, cur.clockkey);
	  old.num_clocks=cur.num_clocks;
	  old.clock_size=cur.num_clocks+2;
	  strcpy(old_exp_enabled_bv, exp_enabled_bv);

	  // Things that must be done:
	  //   1) Remove (fire_enabling,fire_enabled) from Rf
	  exp_enabled_bv[t.rules[fire_enabling][fire_enabled]->marking]='F';
	  //   2) Add (fire_enabling,fire_enabled) to removed conflict Rc
	  //      I need to do this to prevent geom_make_new_clocks from 
	  //      incorrectly adding clocks back into the zone for 
	  //      for fire_enabled.
	  confl_removed_bv[t.rules[fire_enabling][fire_enabled]->marking]='T';
	  //   3) Copy Clock_Zone
	  clock_zone tmp( cur, t.marking_size() );
	  //   4) Advance time
	  bv_advance_time(t.rules, tmp.clocks, tmp.clockkey, tmp.num_clocks);
	  //   5) Normalize
	  normalize(tmp.clocks, tmp.num_clocks, tmp.clockkey, 
		    t.rules, FALSE, NULL, 0);
	  //   6) Get newly enabled rules
	  firinglistADT new_list = NULL;
      
	  new_list = (firinglistADT)get_t_enabled_list(t.rules, tmp.clocks, 
						       tmp.clockkey,
						       marking, t.markkey, 
						       exp_enabled_bv, 
						       tmp.clocknums_iv,
						       confl_removed_bv, 
						       &(tmp.num_clocks), 
						       t.nevents, 
						       t.nrules, 
						       timeopts.interleav); 
#ifdef __DUMP__
	  cout << "Rs: ";
	  dump( cout, new_list, t.get_events() ) << endl
						   << "*******************"
						   << endl << endl;
#endif
	  //   7) Push copied clock_zone onto the stack ( may need to write
          //      specialized push_work function so it doesn't make yet
          //      another copy of the clock_zone).
	  if ( new_list != NULL ) {
	    geom_push_work(&workstack, marking, new_list, tmp.clocks, 
			   tmp.clockkey, tmp.num_clocks, tmp.clock_size, 
			   t.nrules, 
			   exp_enabled_bv,
			   confl_removed_bv, fired_bv, tmp.clocknums_iv);   
	    stack_depth++;
	    pushed=1;
	  }
	  //   8) Restore (fire_enabling,fire_enabled) back into Rf
	  exp_enabled_bv[t.rules[fire_enabling][fire_enabled]->marking]='T';
	  //   9) Remove (fire_enabling,fire_enabled) from Rc
	  confl_removed_bv[t.rules[fire_enabling][fire_enabled]->marking]='F';
	  //   10) Clean up memory from temporary zone.
	  tmp.free_memory();
	}

	// Remove tokens from rules in Ren whose enabled event conflicts
	// with fire_enabled and shares an enabling event with fire_enabled.
	cur.num_clocks=geom_remove_conflicts(t.rules, cur.clocks, 
					     cur.clockkey, t.nevents, 
					     fire_enabling,fire_enabled,
					     cur.num_clocks,
					     exp_enabled_bv, confl_removed_bv,
					     cur.clocknums_iv, fired_bv);
	//       printf("Firing event: %s\n", events[fire_enabled]->event);
	
	// Get the new marking that results from firing this event.
	// This will check ORDERING rule violations that are an event fires 
	// and one of its ordering rules is not enabled; it checks
	// safety violations; and it checks output disablings.
	old_marking=marking;
	if(fire_enabled < (t.nevents-t.ndummy)){
	  marking = (markingADT) find_new_marking(t.events,t.rules,t.markkey,
						  marking, 
						  fire_enabled, 
						  t.nevents,t.nrules, 
						  0, t.nsigs,t.ninpsig,
						  timeopts.disabling||
						  timeopts.nofail,
						  noparg);
	}
	else{
	  marking = (markingADT) dummy_find_new_marking(t.events,t.rules,
							t.markkey,
							marking,fire_enabled, 
							t.nevents, t.nrules, 
							0,t.nsigs,t.ninpsig,
							timeopts.disabling||
							timeopts.nofail,
							noparg);
	}						      	
	
	// No new marking was found due to either an ORDERING rule violation
	// (an event fired with an ordering rule that was not enabled),
	// the net has a safety violation, or it has an output disabling.
	if(marking==NULL){
	  add_state(fp_rsg,state_table,old_marking->state,old_marking->marking,
		    old_marking->enablings,old.clocks,old.clockkey,
		    old.num_clocks,old_exp_enabled_bv,NULL,NULL,NULL,
		    t.nsigs,NULL,NULL,0,sn,verbose,TRUE,t.nrules,
		    old.clock_size,
		    0,bdd_state,use_bdd,t.markkey,timeopts,fire_enabling,
		    fire_enabled,exp_enabled_bv,t.rules, t.nevents);
	  if(fp_rsg){
	    fclose(fp_rsg);
	  }
	  return FALSE;
	}
	
	// Remove the rules in Rf that are in the preset of fire_enabled.
	// Mark all rules in fired_bv in the preset of fire_enabled in 
	// fired_bv as T.
	/*       print_marking(marking);  */
	bv_clear_event(fire_enabled, t.rules, exp_enabled_bv, 
		       confl_removed_bv, 
		       t.nevents, cur.clocknums_iv, t.nrules);
	bv_mark_firing(fire_enabled, t.rules, t.nevents, fired_bv);
	event_fired=1;

      }
      else{
	event_fired = 0;
      }
      
      // If any rules in Rf or in Ren have become disabled due to this
      // event firing, then if it is a disabling rule, remove its clock
      // and mark it unfired in Ren or mark it unfired in the Rf.  If 
      // ATACS is configured to fail on disablings, then it will fail in
      // the find_new_marking or add_state function calls.  This code is
      // to clean up disabled rules.   Responsibility division: Chris
      // checks and flags errors; and Wendy cleans up timing stuff, but
      // leaves error report to Chris.
      cur.num_clocks=check_enablings(t.rules, marking, cur.clocks,
				     cur.clockkey, t.nevents, t.nrules, 
				     exp_enabled_bv,
				     fired_bv, cur.clocknums_iv, 
				     cur.num_clocks,
				     t.nsigs,
				     t.markkey);
      
      // If we did fire an event, then check for verification failures (i.e.,
      // No ordering rules related to fire_enabled should be expired.  If any
      // ordering rule is expired, then report an error and return false.
      if(event_fired){
	failing_rule = check_minimums(t.rules, cur.clocks, cur.clockkey, 
				      cur.num_clocks,
				      fire_enabled);
	if(failing_rule!=0){
	  printf("Verification failure: \n");
	  printf("Constraint rule [%s,%s] does not meet lower bound. \n", 
		 t.events[cur.clockkey[failing_rule].enabling]->event, 
		 t.events[cur.clockkey[failing_rule].enabled]->event);
	  if(fp_rsg){
	    fclose(fp_rsg);
	  }
	  return FALSE;
	}
	
	// As the event has fired, it is OK to now delete the ordering rules
	// and remove their clocks from M.
	cur.num_clocks = geom_project_ordering(t.rules,  marking, cur.clocks, 
					       cur.clockkey, 
					       t.nevents, t.nrules, fired_bv, 
					       cur.clocknums_iv,
					       cur.num_clocks, t.nsigs, 
					       fire_enabled);
	
	cur.num_clocks=geom_make_new_clocks(t.rules, t.markkey, marking, 
					    &(cur.clocks),
					    &(cur.clockkey), t.nevents, 
					    t.nrules,
					    fire_enabled, 
					    cur.num_clocks,&(cur.clock_size), 
					    t.nsigs, exp_enabled_bv, 
					    confl_removed_bv,
					    cur.clocknums_iv, timeopts.infopt);
      }
      
      // Advance time and normalize to keep regions from growning unboundly.
      bv_advance_time(t.rules, cur.clocks, cur.clockkey, cur.num_clocks);
      normalize(cur.clocks, cur.num_clocks, cur.clockkey, 
		t.rules, FALSE, NULL, 0);
      
      // If we fired an events or we are just makring a region graph, then
      // add the state into the state table with the old data.
      // Otherwise, just add the new state without the old data.
      if(event_fired || timeopts.genrg){
	new_state=add_state(fp_rsg, state_table, old_marking->state,
			    old_marking->marking, old_marking->enablings,
			    old.clocks, old.clockkey, old.num_clocks, 
			    old_exp_enabled_bv,
			    marking->state,marking->marking, 
			    marking->enablings,
			    t.nsigs, cur.clocks, cur.clockkey, 
			    cur.num_clocks, sn,
			    verbose, 1, t.nrules, old.clock_size, 
			    cur.clock_size,
			    bdd_state, use_bdd,t.markkey,timeopts,
			    fire_enabling, fire_enabled, exp_enabled_bv,
			    t.rules, t.nevents);
	if(old_marking!=NULL && event_fired){
	  delete_marking(old_marking);
	  free(old_marking);
	}
      }
      else{
	new_state=add_state(fp_rsg, state_table, NULL, NULL, NULL, NULL,
			    NULL,0, NULL, marking->state, marking->marking,
			    marking->enablings, t.nsigs, cur.clocks, 
			    cur.clockkey,
			    cur.num_clocks, sn, verbose, 1, t.nrules, 
			    old.clock_size, cur.clock_size,
			    bdd_state, use_bdd,t.markkey,timeopts,
			    -1, -1, exp_enabled_bv, t.rules, t.nevents);
      }
      free_region(old.clockkey, old.clocks, old.clock_size);    
      
      // If the added state was a new state, then increment the region count
      if( new_state != 0 ){
	
	// If this is a new timed state, then increment the region count.
	if( new_state > 0 ){
	  regions++;
	}
	// If supersets is on and we did remove regions 
	// (new_state is equal to -1 * removed_regions from find_state), 
	// so update the region count.
	else if ( timeopts.supersets ) {
	  regions=regions+new_state+1;
	  if ( timeopts.prune ) {
	    stack_removed=geom_clean_stack(&workstack, cur.clocks, 
					   cur.clockkey, cur.num_clocks,  
					   pushed, stack_depth,
					   marking);
	    stack_depth=stack_depth-stack_removed;
	  }
	}     
	
#ifdef LIMIT
	if ( regions > LIMIT ) {
	  printf("Too many regions (%d > max=%d). Bailing out!\n",
		 regions, LIMIT );
	  fprintf( lg,"Too many regions (%d > max=%d). Bailing out!\n",
		   regions, LIMIT );
	  fclose( lg );
	  exit( regions );
	}
#endif
	if( ( regions % 1000 ) == 0 ) {
	  printf("%d regions, stack_depth=%d\n", regions, stack_depth); 
	  fprintf(lg,"%d regions, stack_depth=%d\n", regions, stack_depth); 
#ifdef DLONG
	  printf("%ld nodes used \n",bdd_total_size(bdd_state->bddm));
#endif
#ifdef MEMSTATS
#ifndef OSX
	  memuse=mallinfo();
	  printf("memory: max=%d inuse=%d free=%d \n",
		 memuse.arena,memuse.uordblks,memuse.fordblks);
	  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
		  memuse.arena,memuse.uordblks,memuse.fordblks);
#else
	  malloc_zone_statistics(NULL, &stats);
	  printf("memory: max=%d inuse=%d allocated=%d\n",
		 stats.max_size_in_use,stats.size_in_use,stats.size_allocated);
	  fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		  stats.max_size_in_use,stats.size_in_use,stats.size_allocated);
#endif
#endif
	}
	// We added a new untimed state, increment the untimed state count.
	if(new_state==1){
	  sn++;
	}
#ifdef __DUMP__
	bv_advance_time(t.rules, cur.clocks, cur.clockkey, cur.num_clocks);
	normalize(cur.clocks, cur.num_clocks, cur.clockkey, t.rules, 
		  FALSE, NULL, 0);
	cout << "zone: " << cur.num_clocks << endl;
	dump( cout, cur.num_clocks, cur.clockkey, t.events ) << endl;
	dump( cout, cur.clocks, cur.num_clocks) << endl;
#endif
	/*Mark this rule as exp_enabled */
	old.clocks=copy_clocks(cur.num_clocks+2, cur.clocks);
	old.clockkey=copy_clockkey(cur.num_clocks+2, cur.clockkey);
	old.num_clocks=cur.num_clocks;
	old.clock_size=cur.num_clocks+2;
	strcpy(old_exp_enabled_bv, exp_enabled_bv);

	firinglist = (firinglistADT)get_t_enabled_list(t.rules, cur.clocks, 
						       cur.clockkey,
						       marking, t.markkey, 
						       exp_enabled_bv, 
						       cur.clocknums_iv,
						       confl_removed_bv, 
						       &(cur.num_clocks), 
						       t.nevents, 
						       t.nrules, 
						       timeopts.interleav); 
#ifdef __DUMP__
	  cout << "Lower Rs: ";
	  dump( cout, firinglist, t.get_events() ) << endl << endl;
#endif
      } // Matchs if(newstate)
      else {

	// We did not add a new state to the state table, so this state has
	// has already been found and we can pop a new item off of the stack.
	current_work=geom_pop_work(&workstack);
	
	// The stack is empty, so we have found all reachable states.
	// Mark the times and report the final region and state count.
	if(current_work==NULL){
	  time.mark();
	  printf( "Found %d regions in %d states (", regions, sn );
	  time.print(stdout);
	  printf(")\n");
	  if (fp_rsg) fclose(fp_rsg);
	  return(TRUE);
	}
	else{
	  
	  // We have not found all of the state, so clean up memory and
	  // set pointers from current work to the local scope.
	  
	  /* don't free the first rules matrix since the rest of the */
	  /* program needs it */
	  geom_free_structures(marking, cur.clocks, cur.clockkey, 
			       cur.clock_size, 
			       cur.clocknums_iv,
			       exp_enabled_bv, confl_removed_bv, fired_bv); 
	  
	  // update the new stack depth.
	  stack_depth--;
	  marking=current_work->marking;
	  firinglist=current_work->firinglist;
	  cur.clocks=current_work->clocks;
	  cur.clockkey=current_work->clockkey;
	  cur.num_clocks=current_work->num_clocks;
	  cur.clock_size=current_work->clock_size;
	  cur.clocknums_iv=current_work->clocknums;
	  exp_enabled_bv=current_work->exp_enabled;
	  confl_removed_bv= current_work->confl_removed;
	  fired_bv = current_work->fired;

	  /*Mark this rule as exp_enabled */
	  old.clocks=copy_clocks(cur.num_clocks+2, cur.clocks);
	  old.clockkey=copy_clockkey(cur.num_clocks+2, cur.clockkey);
	  old.num_clocks=cur.num_clocks;
	  old.clock_size=cur.num_clocks+2;

	  strcpy(old_exp_enabled_bv, exp_enabled_bv);

	  free(current_work);

#ifdef __DUMP__
	  cout << endl << "Popping timed state -> " << endl;
	  cout << "s: " << marking->state << endl << "Rm: ";
	  dump( cout, t.nrules, marking->marking, t.markkey, t.events ) 
	    << endl;
	  cout << "Rf: ";
	  dump( cout, t.nrules, exp_enabled_bv, t.markkey, t.events ) << endl;
	  cout << "zone: " << cur.num_clocks << endl;
	  dump( cout, cur.num_clocks, cur.clockkey, t.events ) << endl;
	  dump( cout, cur.clocks, cur.num_clocks) << endl;
	  cout << "Rs: ";
	  dump( cout, firinglist, t.events ) << endl << endl;
#endif	  
	}
      }
    }
  }
  catch( error* ptr_e ) {
    ptr_e->print_message( lg );
    delete ptr_e;
  }
  if(fp_rsg){
    fclose(fp_rsg);
  }
  return( false );
}
//////////////////////////////////////////////////////////////////////////
