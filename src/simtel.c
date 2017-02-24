/*****************************************************************************/
/* simtel.c                                                                  */
/*****************************************************************************/
#include "simtel.h"
#include "signal_handler.h"
#include "CUniform.h"

#define MAXDBL       1.7976931348623157e+308
#define _EVENT_      -1

extern bool *disabled;

///////////////////////////////////////////////////////////////////////////////
// ostream operator<< that correctly formats and output a ren_list::value_type
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const ren_list::value_type& a ) {
  s << "(" << a.first << "," << a.second << ")";
  return( s );
}

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Construct a time_advace object and set the class member m_time to time.
///////////////////////////////////////////////////////////////////////////////
time_advance::time_advance( const double& time ) : m_time( time ) {
  // Do Nothing
}

///////////////////////////////////////////////////////////////////////////////
// Return a new pair<double,int> were first = a.first - m_timer and 
// second = a.second
///////////////////////////////////////////////////////////////////////////////
pair<double,int> time_advance::operator()( const pair<double,int>& a ) {
  if ( m_time == 0 ) return( a );
  return( make_pair( a.first - m_time, a.second ) );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Simple rule_comp constructor
///////////////////////////////////////////////////////////////////////////////
rule_comp::rule_comp( const int & i ) 
  : m_index( i ) {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// If a.second == b.second return True otherwise return False
///////////////////////////////////////////////////////////////////////////////
bool rule_comp::operator()( const pair<double,int>& a ) {
  return( a.second == m_index );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Baisc constructor
///////////////////////////////////////////////////////////////////////////////
mark_update::mark_update( markingADT mark, 
			  markkeyADT* mark_key,
			  ruleADT** rules,
			  const int& nsignals,
			  ren_list& Ren,
			  char_bv_tf::const_iterator Rf ) 
  : m_mark( mark ),
    m_mark_key( mark_key ),
    m_rules( rules ),
    m_nsignals( nsignals ),
    m_Ren( Ren ),
    m_Rf( Rf ),
    m_index( 0 ) {
  // Nothing To Do
}

///////////////////////////////////////////////////////////////////////////////
// Each time this function is called, it checks to see if c is 'T'.  If it is
// 'T' and the rule has not fired ( check m_Rf[m_index] ) and rule is level
// enabled, than go ahead and add it to the m_Ren list. Otherwise, remove the
// rule from the m_Ren list.  Update m_index. NOTE: m_mark_key is used with 
// to find the actual rule that is added to m_Ren. 
///////////////////////////////////////////////////////////////////////////////
void mark_update::operator()( const char& c ) {
  // Is the rule marked, not fired, and level enabled
  if ( c != 'F' &&                   
       m_Rf[m_index] == 'F' && 
       level_satisfied( m_rules, 
			m_mark, 
			m_nsignals, 
			m_mark_key[m_index]->enabling,
			m_mark_key[m_index]->enabled ) ) {
    // If the rule isn't in m_Ren, add it to the list
    if ( ::find_if( m_Ren.begin(), 
		    m_Ren.end(), 
		    rule_comp( m_index ) ) == m_Ren.end() ) {
      ruleADT r = m_rules[m_mark_key[m_index]->enabling]
	                 [m_mark_key[m_index]->enabled];
      m_Ren.push_back( ren_list::value_type( r->dist->rand(), m_index ) );
    }
  }
  // Must be a conflict rule, so delete it from the m_Ren list
  else 
    m_Ren.remove_if( rule_comp( m_index ) );
  ++m_index; // Increment to index for the next call.
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Basic constructor that iniitializes all class members
///////////////////////////////////////////////////////////////////////////////
rf_update::rf_update( ruleADT** rules, markkeyADT* mark_key, int event ) 
  : m_rules(rules),
    m_mark_key( mark_key ),
    m_event( event ),
    m_index( 0 ) {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// Returns true iff the rule at m_index in the mark_key has the same enabled
// event as m_event. NOTE: Used with stl algorithms to iterate on a list and
// identify rules that have the an enabled event that matches m_event.
///////////////////////////////////////////////////////////////////////////////
bool rf_update::operator()( const char& c  ) {
  bool retVal = (m_mark_key[m_index]->enabled == m_event || 
		 m_rules[m_mark_key[m_index]->enabled][m_event]->conflict);
  ++m_index;
  return( retVal );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Sets m_rule.first to MAXDOUBLE and m_rule.second to -1
///////////////////////////////////////////////////////////////////////////////
min_find::min_find() 
  : m_rule( rule_vec( 1, rule_type( MAXDBL, -1 ) ) ) {
  // Nothing to do
}

///////////////////////////////////////////////////////////////////////////////
// Set m_rule equal to a iff a.first is < m_ruls.first
///////////////////////////////////////////////////////////////////////////////
void min_find::operator()( const pair<double,int>& a ) {
  if ( a.first < (*m_rule.begin()).first ) {
    if ( m_rule.size() > 1 ) {
      m_rule.erase( m_rule.begin() + 1, m_rule.end() );
    }
    (*m_rule.begin()) = a;
    return;
  }
  if ( a.first == (*m_rule.begin()).first ) {
    m_rule.push_back( a );
  }
}

/////////////////////////////////////////////////////////////////////////////
// Resolves conflict by considering all events that are scheduled to fire
// at the same time.  This is done by consulting the upsilon values for
// each of the rules stored in the ruleADT.  They are normalized and then
// a Uniform random number between 0 and 1 is obtained.  The function then
// Picks a rule based upon the normalized distribution and the uniform 
// distribution.
/////////////////////////////////////////////////////////////////////////////
void min_find::resolve_conflict( markkeyADT* markkey, ruleADT** rules ) {
  if ( m_rule.size() == 1 ) { 
    return;
  }
  prob_vector pc( m_rule.size(), 0.0 );
  prob_type mark = 0.0;
  for ( unsigned int i = 0 ; i < m_rule.size() ; i++ ) {
    int enabling = markkey[(m_rule[i]).second]->enabling;
    int enabled = markkey[(m_rule[i]).second]->enabled;
    pc[i] = (prob_type)((rules[enabling][enabled])->dist->upsilon());
    mark += pc[i];
  }
  transform( pc.begin(), pc.end(), pc.begin(), 
	     bind2nd( divides<prob_type>(), mark ) );
  CUniform uni( 0, 1 );
  mark = uni.rand();
  prob_type base = 0;
  for ( unsigned int i = 0 ; i < pc.size() ; i++ ) {
    if ( mark >= base && mark < base + pc[i] ) {
      m_rule[0] = m_rule[i];
      m_rule.erase( m_rule.begin() + 1, m_rule.end() );
      return;
    }
    base += pc[i];
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns m_rule
///////////////////////////////////////////////////////////////////////////////
pair<double,int> min_find::rule() const {
  return( *m_rule.begin() );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// Performs a monte-carlo simulation of a TEL structure. Template parameter
// specifies the object to be used to test for convergence.  Current objects
// are a generic_analysis_control, which is templatized to support a variety
// of graph_trackers.  The graph_tracker must support a probability function
// that returns a prob_vector.  The graph_tracker class is used to determine
// what probabilities are being calculated.  Current options include trackers
// that react to event or rule firings and calculate state or transition 
// probabilities.
// NOTE: This cannot be run without an existing state graph.
///////////////////////////////////////////////////////////////////////////////
template<class anal_control>
bool sim_tel( signalADT *signals,
	      eventADT *events,
	      ruleADT **rules,
	      markkeyADT *markkey,
	      int nevents, 
	      int ninpsig,
	      int nsignals,
	      int nrules,
	      char * startstate,
	      bool verbose,
	      int ndummy,
	      graph_tracker* g,
	      anal_control done,
	      bool noparg,
	      timeoptsADT timeopts) 
{
  disabled=(bool*)GetBlock(sizeof(bool)*nevents);
  for (int i=0;i<nevents;i++)
    disabled[i]=false;

  double time = 0;
  char_bv_tf Rf( nrules + 1, 'F' );    // Tracks fired rules 
  Rf[nrules] = '\0';                   // Necessary for 'c' code compatability
  // Get the initial marking
  markingADT mks = find_initial_marking( events, rules, markkey, nevents, 
					 nrules, ninpsig, nsignals, startstate,
					 verbose );
  // Initialize the Ren list using the mark_update class
  ren_list Ren;
  for_each( mks->marking, mks->marking + nrules, 
	      mark_update( mks, markkey, rules, nsignals, Ren, Rf.begin() ) );
  //if ( sigsetjmp( gCtrl_C_mngr.new_entry(), 1 ) != 0 ) {
    cout << "interupted ... ";
    gCtrl_C_mngr.pop();
    free(disabled);
    return( TRUE );
    //}
  do {
    // A deadlock has occured.  Simply return false.
    // Assume user is not going to look at statistics.
    if ( Ren.empty() ) {
      delete_marking( mks );        // Wipe memory for old marking
      free(mks);
      gCtrl_C_mngr.pop();
      free(disabled);
      return( FALSE );
    }
    // Find the next rule to fire
    min_find f = for_each( Ren.begin(), Ren.end(), min_find() );
    f.resolve_conflict( markkey, rules );
    // Mark the rule as fired in the Rf list
    Rf[f.rule().second] = 'T';
    if ( g->mode() == graph_tracker::rule ||
	 g->mode() == graph_tracker::both ) // Fire rule if in rule mode
      assert( g->fire( f.rule().first,
		       markkey[f.rule().second]->enabled, 
    		       markkey[f.rule().second]->enabling ) );
    // Remove the fired rule from the Ren list
    Ren.remove_if( rule_comp( f.rule().second ) );
    // Advance time on all of the rules in Ren
    transform( Ren.begin(), Ren.end(), Ren.begin(), 
		 time_advance( f.rule().first ) );
    time += f.rule().first;
    // If a event can now be fired, fire the event
    if ( bv_enabled_event( markkey[f.rule().second]->enabled,
			   markkey[f.rule().second]->enabling, 
			   rules, nevents, &(*Rf.begin()) ) ) {
      if ( g->mode() == graph_tracker::event ) {// Fire event if in event mode
	assert( g->fire( time,
			 markkey[f.rule().second]->enabled,
			 markkey[f.rule().second]->enabling ) );
	time = 0;
      }
      else if ( g->mode() == graph_tracker::both ) {
	assert( g->fire( _EVENT_,
			 markkey[f.rule().second]->enabled,
			 markkey[f.rule().second]->enabling ) );
      }
      // Update the Rf list to remove any rules associated with the fired event
      replace_if( Rf.begin(), Rf.end()-1, 
		    rf_update( rules, markkey, 
			       markkey[f.rule().second]->enabled ), 
		    'F' );
      markingADT tmp = mks;
      // Get the new marking, watch if its a dummy or normal event
      if ( markkey[f.rule().second]->enabled < (nevents - ndummy) )
	mks = find_new_marking( events, rules, markkey, tmp, 
				markkey[f.rule().second]->enabled, nevents,
				nrules, verbose, nsignals,ninpsig,
				timeopts.disabling||timeopts.nofail,
				noparg); 
      else 
	mks = dummy_find_new_marking( events, rules, markkey, tmp, 
				      markkey[f.rule().second]->enabled, 
				      nevents, nrules, verbose, nsignals, 
				      ninpsig,
				      timeopts.disabling||timeopts.nofail,
				      noparg);
      delete_marking( tmp );        // Wipe memory for old marking
      free(tmp);
      // A safety violation has occured.  Simply return false.
      // Assume user is not going to look at statistics.
      if ( !mks ) { 
	gCtrl_C_mngr.pop();
	free(disabled);
	return( FALSE );
      }
      // Update Ren to reflect the new marking
      for_each( mks->marking, mks->marking + nrules, 
		  mark_update(mks,markkey,rules,nsignals,Ren,Rf.begin()));
    }
  } while( !done( *g ) ); // Loop until graph_tracker has converged
  if ( mks ) {
    delete_marking( mks );
    free(mks);
  }
  gCtrl_C_mngr.pop();
  free(disabled);
  return( TRUE );
}

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// creates a convergence function that is more geared towards simulation.  
// This function has the following characteristics:
//             cycles <= 10000 distance = 1000
//       10000 < cycles <= 50000 distance = 5000
//      50000 < cycles <= 100000 distance = 10000
//             cycles > 100000 distance = 20000
// Where cycles is the total number of iterations in the system and distance
// is the amount of distance thats lags between each successive convergence
// check.
///////////////////////////////////////////////////////////////////////////////
convergence_func simulation_convergence_func() {
  vector<int> levels( (long)4, 1000 );
  vector<unsigned long> borders( (long)3, 10000 );
  levels[1] = 5000;
  levels[2] = 10000;
  levels[3] = 20000;
  borders[1] = 50000;
  borders[2] = 100000;
  return( convergence_func( levels, borders ) );
}
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////
// This function sets up and calls a monte-carlo simulation of the current
// design.  The function uses the genrg flag to create a specific type of
// graph_tracker to be used in the simlation of the TEL.  graph_trackers can
// be created to react to rule or event firings.  They can find transition 
// or state probabilities. The system will store the results of the simulation
// in the state_table.
// NOTE: There needs to be a flag added to designADT that specifies what type
// the user wants from the simulation, transition or state probabilities.
// This will determine what type of graph_tracker is sent to simtel and where
// the final results are stored.
///////////////////////////////////////////////////////////////////////////////
bool simulate_tel( designADT design, graph_tracker* g )
{
  assert( (intptr_t)g != 0 );
  convergence_func f = simulation_convergence_func();
  generic_analysis_control<graph_tracker> a( INT_MAX, design->tolerance, f );
  cout << "Simulating TEL -> firing ";
  if ( design->timeopts.genrg )
    cout << "rules ... " << flush;
  else 
    cout << "events ... " << flush;
  cpu_time_tracker time;
  assert( sim_tel( design->signals,
		   design->events,
		   design->rules,
		   design->markkey,
		   design->nevents,
		   design->ninpsig,
		   design->nsignals, 
		   design->nrules,
		   design->startstate,
		   design->verbose,	
		   design->ndummy,
		   g,
		   a,
		   design->noparg,
		   design->timeopts) );
  time.mark();
  cout << "done: " << time << endl << flush;
  return( true );
}

bool simulate_tel( designADT design, graph_tracker* g, anal_cont a ) {
  assert( (intptr_t)g != 0 );
  return( sim_tel( design->signals,
		   design->events,
		   design->rules,
		   design->markkey,
		   design->nevents,
		   design->ninpsig,
		   design->nsignals, 
		   design->nrules,
		   design->startstate,
		   design->verbose,	
		   design->ndummy,
		   g,
		   a, 
		   design->noparg,
		   design->timeopts) );
}

//
///////////////////////////////////////////////////////////////////////////////
