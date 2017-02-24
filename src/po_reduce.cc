//////////////////////////////////////////////////////////////////////////
// @name Partial Order Reduction
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#include "po_reduce.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <list>

#include "error.h"

//#define __DUMP__
//#define __PO_DUMP__

long hit = 0;
long calls = 0;

//-----------------------------------------------------------------------
// This global flag is used to toggle prunning using the necessary set
// calculation on and off.  Its default value is to do the most aggressive
// prunning that it can.
// NOTE: I should move this into a parameter, but I do not want to change
// any interfaces to keep a large range of compatability in the CVS 
// repository (i.e., easier to role back on versions )
//------------------------------------------------------------------------
bool USE_NECESSARY_SET_TO_PRUNE( true );

void set_prune( bool value ) {
  USE_NECESSARY_SET_TO_PRUNE = value;
}

bool is_prune_set() {
  return( USE_NECESSARY_SET_TO_PRUNE );
}
//------------------------------------------------------------------------

//#ifdef __PO_DUMP__
unsigned int recursive_depth = 0;
//#endif
unsigned int max_depth = 0;

#define EVENT( j ) j.first
#define DELAY( j ) j.second

// typedef map<event_type,int,less<event_type>,malloc_alloc> event_map;
typedef map<event_type,int,less<event_type> > event_map;

typedef struct map_dirty_struct {
  event_map* res;
  event_set* dirty;
} map_dirtyADT;

event_map **cache;

//-----------------------------------------------------------ostream& dump
ostream& dump( ostream& s, const tel& t,
	       const event_map::value_type& i ) {
  s << "(" << t.get_event( EVENT( i ) ) << ","
    << DELAY( i ) << ")";
  return( s );
}
//------------------------------------------------------------------------

//-----------------------------------------------------------ostream& dump
// dumps a human readable form of an event_map E
//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const event_map& E ) {
  typedef event_map::const_iterator const_iter;
  s << "{";
  for ( const_iter i = E.begin() ; i != E.end() ; ) {
    dump( s, t, *i );
    if ( ++i != E.end() ) {
      s << ",";
    }
  }
  s << "}";
  return( s );
}
//------------------------------------------------------------------------

//----------------------------------------------------bool is_enough_slack
// NOTE: I do not set an ordering on f and e because delay is on _places_
// not events in our semantics.  Thus, e firing before f is not requisite
// for f to be causal!
bool is_enough_slack( const tel& t                  , 
		      const timed_state& sc         , 
		      const event_type& f           , 
		      const event_map::value_type e   ) {
  typedef rule_set::iterator iterator;
  typedef front_insert_iterator<rule_set> rule_set_insert;
  enabled_set E( 2 );
  enabled_set::iterator Rb = E.begin();
  get_rule_set( f, sc.get_untimed_st(), t, *Rb++ );
  get_rule_set( EVENT( e ), sc.get_untimed_st(), t, *Rb );
  POSET Me( sc.get_POSET() );
  Me.add( E, t );
  // After the lower bounds have been added into Me (see previous line), 
  // then Remove rules from Rb that are overbounded by an unbounded rule
  // (i.e., exists a rule r in Rb with an unbounded upper limit, then 
  // all the other rules no longer matter; they will never be constraining).
  iterator end = remove_over_bounded( (*(Rb = E.begin())), Me, t );
  for ( iterator i = (*Rb).begin() ; i != end ; ++i ) {
    //if ( !is_causal_candidate( t, *Rb, sc.get_POSET(), i ) ) continue;
    event_type causal = t.enabling( *i );
    event_type enabled = t.enabled( *i );
    if ( !Me.is_in_POSET( causal ) ) continue;
    POSET::bound_type upper_bound = t.upper_bound( causal, enabled );
    Me.set_max_separation( causal, enabled, upper_bound );
    POSET Mt( Me );
    if ( Mt.is_consistent()                               && 
	 DELAY( e ) <= Mt.get_separation( f, EVENT( e ) )    ) {
#ifdef __PO_DUMP__
      dump( cout, t, e ) << " satisfies t("
			 << t.get_event( f ) << ") - t(" 
			 << t.get_event( EVENT( e ) ) << ") <= "
			 << Mt.get_separation( f, EVENT( e ) ) << endl;
#endif
      return( true );
    }
    Me.set_max_separation( causal, enabled, POSET::_UNBOUNDED_ );
  }
  return( false );
}

//------------------------------------------------------------------------

//----------------------------------------------------void get_preset_list
// This assigns to preset_list event_sets of mutually conflicting events
// that enable the event e.  In other words, it looks at all the events
// in the preset of e, and groups them according to their conflict 
// relations in separate event_sets.  Thus if e has e1, e2, and e3 in its
// preset and e1 # e2, then get_preset_list would add 2 event_sets to
// preset_list: {e1,e2} and {e3}.
//------------------------------------------------------------------------
template <class inserter>
void get_preset_list( const tel& t              , 
		      const timed_state& sc     ,
		      const event_type& e       , 
		      inserter marked_insert    ,
		      inserter nonmarked_insert   ) {
  // Ep is the set of all events in the preset of e.  These events
  // must be connected to e with a behavior rule or a level_rule.
  const preset_list& P( t.get_preset_list( e ) );
  bool throttle = false; //( ( P.size() > 10 ) ? true : false );
  bool inserted( false );
  for ( preset_list::const_iterator i( P.begin() ) ; i != P.end() ; ++i ) {
    const event_set& Etmp( **i );
    event_set::const_iterator j( Etmp.begin() );
    bool marked( false );
    while ( j != Etmp.end() ) {
      const event_type& tmp( *j++ );
      if ( t.is_behavior_rule( tmp, e ) ) {
	int marking_index( t.marking_index( tmp, e ) );
	marked = sc.is_marked( marking_index );
      }
      else {
	marked = sc.is_level_matched( t, tmp );
      }
      if ( marked ) break;
    }
    if ( !marked ) {
      if ( !throttle              ||
	    throttle && !inserted    ) {
	*nonmarked_insert++ = (event_set*)&Etmp;
	inserted = true;
      }
    }
    else {
      *marked_insert++ = (event_set*)&Etmp;
    }
  }
}
//------------------------------------------------------------------------

//------------------------------------------------------------add_to_delay
struct add_to_delay {

  // Required for bind2nd stl interface.
  typedef event_map::value_type first_argument_type;
  typedef int second_argument_type;
  typedef event_map::value_type result_type;

  result_type operator()( const event_map::value_type& e, int d ) const {
    return( make_pair( EVENT( e ), DELAY( e ) + d ) ); 
  }

};
//------------------------------------------------------------------------

//------------------------------------------------------int get_max_delay
int get_max_delay( const event_map& E ) {
  int max_delay = -1;
  for ( event_map::const_iterator i = E.begin() ; i != E.end() ; ++i ) {
    int tmp_delay = DELAY( (*i) );
    if ( tmp_delay > max_delay ) {
      max_delay = tmp_delay;
    }
  }
  return( max_delay );
}
//------------------------------------------------------------------------

//-------------------------------------------------------void combine_maps
namespace
{

  // I HATE PAIRS!  They are evil, but since I have to deal with them,
  // I am going to use these interface functions to make the code at
  // a minimum, readable!
  typedef pair<event_map::iterator,bool> insert_type;

  inline int get_delay( insert_type i ) {
    return( DELAY( (*(i.first)) ) );
  }

  inline bool is_inserted( insert_type i ) {
    return( i.second );
  }

  inline void set_delay( insert_type i, int delay ) {
    DELAY( (*(i.first)) ) = delay;
  }

}

template<class delay_calc>
void combine_maps( event_map* ptr_source, 
		   event_map* ptr_dest,
		   const delay_calc& compute_delay ) {
  typedef event_map::iterator iterator;
  for (iterator i = ptr_source->begin() ; i != ptr_source->end() ; ++i) {
    // 1) compute the new delay for the map entry.
    event_map::value_type tmp( compute_delay( *i ) );
    // 2) Try and insert the new map entry into the destination map (dest)
    insert_type result = ptr_dest->insert( tmp );
    // 3) if the insert is successfull, then move to the next entry.
    if ( is_inserted( result ) ) continue;
    // 4) The insert failed, so get the delay from the existing map entry.
    int delay( get_delay( result ) );
    // If the new delay is smaller than the old delay, store the smaller
    // delay from the tmp entry into the map.  This is required to 
    // correctly handle merge places.
    if ( tmp.second < delay ) {
      set_delay( result, tmp.second );
    }
  }
}

void combine_sets( event_set* ptr_source, 
		   event_set* ptr_dest ) {
  typedef event_set::iterator iterator;
  for (iterator i = ptr_source->begin() ; i != ptr_source->end() ; ++i) {
    ptr_dest->insert( *i );
  }
}

//------------------------------------------------------------------------

//---------------------------------------------event_set* recursive_search
// This function recursively searches back from the preset of e looking 
// for a minimal set of fireable events that will enable e to fire 
// (i.e., the necessary set for e). To find the necessary set, the function 
// will find all possible necessary sets; and it return the set with the
// smallest cardinality and the largest delay.  NOTE: at each return
// the visited list needs to be reset to false for event e.  This list
// is used to detect dependent cycles in the graph, and to record where
// the algorithm has already at a specific call level.
//------------------------------------------------------------------------
map_dirtyADT recursive_search( const tel& t,
			       const timed_state& sc,
			       const event_type& e,
			       const event_set& Ef,
			       vector<bool>& visited) {

#ifdef __PO_DUMP__
  cout << " visiting " << t.get_event( e ) << endl;
#endif

  map_dirtyADT result;
  result.res = new event_map();
  result.dirty = new event_set();

  // This event has already been visited on _this_ search path! This
  // denotes a loop creating a circular dependency (i.e., the firing of
  // e leads the enabling of e).  Return an empty necessary set.
  if ( visited[e] ) { 
#ifdef __PO_DUMP__
    cout << "Found cycle (visited) returning ";
    dump( cout, t, *(result.res) );
    cout << endl;
    cout << endl;
#endif
    result.dirty->insert( e );
    return( result ); 
  }

  // If this event is fireable, then insert it into En and return.
  // NOTE: The delay is zero because the delay for e will be considered
  // in is_enough_slack when causality is assigned.
  if ( Ef.find( e ) != Ef.end() ) {
    result.res->insert( make_pair( e, 0 ) );
#ifdef __PO_DUMP__
    cout << "Is fireable added to necessary returning ";
    dump( cout, t, *(result.res) );
    cout << endl;
    cout << endl;
#endif
    return( result );
  }
  
  // If there is a cached result, then copy it into result and return
  calls++;
  if ( cache[e] ) {
    hit++;
    const event_map& tmp( *cache[e] );
    copy( tmp.begin(), tmp.end()                       , 
	  inserter( *result.res, result.res->begin() )   );
#ifdef __PO_DUMP__
    if ( recursive_depth > 0 )
      --recursive_depth;
    cout << "Found in cache returning ";
    dump( cout, t, *(result.res) );
    cout << endl;
    cout << endl;
#endif
    return( result );
  }
  
  // Get the list of events in the preset of e.  If any of those events
  // conflict, then group them according to their conflict relations
  // (i.e., if e' # e'', the put e' and e'' in the same event set.
  // According to Yoneda's Async 99 paper, because of the conflict 
  // relation, e' or e'' may cause e, thus both must be included in
  // the necessary set for e.
  preset_list marked;
  preset_list nonmarked;
  get_preset_list( t, sc, e, inserter( marked ), inserter( nonmarked ) );
#ifdef  __PO_DUMP__
  cout << endl;
  //sc.dump( cout, t ) << endl;
  cout << "preset[" << t.get_event( e ) << "]";
  dump( cout, t, nonmarked ) << endl;
  cout << "marked[" << t.get_event( e ) << "]";
  dump( cout, t, marked ) << endl;
#endif

  int necessary_size = -1;
  int max_delay = -1;
  for ( preset_list::const_iterator i = nonmarked.begin() ; 
	i != nonmarked.end()                              ; 
        ++i                                                 ) {
    // Create a temporary map and dirty set (temp) to store the necessary set 
    // that results from choosing this event_set (*i) to enable e.
    map_dirtyADT temp;
    temp.res = new event_map();
    temp.dirty = new event_set();
    visited[e] = true;
    for ( event_set::const_iterator j = (*i)->begin() ; 
          j != (*i)->end()                            ; 
          ++j                                           ) {
      event_type ej = (*j);
      POSET::bound_type lower_bound = t.lower_bound( ej, e );
      // Get the necessary set for ej and union it into Et.  During the
      // union, update the delays with the lower bound on the rule
      // (ej, e, l, u).
#ifdef __PO_DUMP__
      cout << "Recursive Working on " << t.get_event( e );
      ++recursive_depth;
#endif	

      /* Mark all occurence of an event as visited when you visit one */
      if (!t.is_dummy_event(ej)/*!t.is_behavior_rule(ej,e)*/) {
	int sig=t.get_signal(ej);
	for (int cnt=(t.get_signals())[sig]->event + (ej % 2 ? 0 : 1);
	     cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
	  if ((unsigned)cnt != ej) visited[cnt]=true;
      }
	
      map_dirtyADT rd = recursive_search( t, sc, ej, Ef, visited );

      /* Unmark all occurence of an event as visited */
      if (!visited[ej])
	if (!t.is_dummy_event(ej)/*!t.is_behavior_rule(ej,e)*/) {
	  int sig=t.get_signal(ej);
	  for (int cnt=(t.get_signals())[sig]->event + (ej % 2 ? 0 : 1);
	       cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
	    if ((unsigned)cnt != ej) visited[cnt]=false;
	}
      
      combine_maps( rd.res, temp.res, bind2nd( add_to_delay(), lower_bound ) );
      combine_sets( rd.dirty, temp.dirty );
      delete rd.res;
      delete rd.dirty;
    }
    visited[e] = false;
    int tmp_size = temp.res->size();
    int tmp_delay = get_max_delay( *(temp.res) );

    // If the tmp_size is zero, then a loop forming a circular 
    // dependency was discovered in the search (i.e., event e
    // cannot fire until event e fires).
    if ( tmp_size == 0 ) {
      delete result.res;
      delete result.dirty;
#ifdef __PO_DUMP__
      cout << "Found cycle returning ";
      dump( cout, t, *(temp.res) );
      cout << endl;
      cout << endl;
#endif
      /* Newly added */
      /* This code should be in a function call */
      if (!cache[e]) {
	bool cacheit=true;
	/* Is this right? */
	for (int i=0;i<t.number_events();i++)
	  if ((visited[i]) && 
	      (temp.dirty->find( i ) != temp.dirty->end())) {
	    cacheit=false;
	    break;
	  }
	if (cacheit) {
#ifdef __PO_DUMP__
	  cout << "Storing result on " << t.get_event( e ) << " in cache ";
	  dump( cout, t, *(temp.res) );
	  cout << endl;
	  cout << endl;
#endif
	  cache[e] = new event_map();
	  *cache[e] = *(temp.res);
	} else {
#ifdef __PO_DUMP__
	  cout << "Dirty, not storing result on " << t.get_event( e ) 
	       << " in cache ";
	  dump( cout, t, *(temp.res) );
	  cout << endl;
	  cout << endl;
#endif
	}
      }
      /* To here */
      return( temp );
    }
    // The PO reduction gets the best performance by using the smallest
    // En that it can.  Thus, this code searches over all possible events
    // that can be used as a necessary set.  It selects a set that 
    // has the smallest cardinality (0 is optimal) and the largest minimum
    // delay for any given member.  The delay is included in the selection
    // process because it is possible to exclude members from the ready
    // set if they can not fire soon enough to enable a dependent event.
    if ( ( tmp_size < necessary_size )                           ||
	 ( tmp_size == necessary_size && tmp_delay > max_delay ) || 
	 ( necessary_size == -1 )                                   ) {
      delete result.res;
      delete result.dirty;
      result.res = temp.res;
      result.dirty = temp.dirty;
      necessary_size = tmp_size;
      max_delay = tmp_delay;
    }
    // Et must be size zero has more members than the current En.
    // Free its memory and move on.
    else {
      delete temp.res;
      delete temp.dirty;
    }
  }
#ifdef __PO_DUMP__
  cout << "Working on " << t.get_event( e ) << " returning ";
  dump( cout, t, *(result.res) );
  cout << endl;
  cout << endl;
#endif
  if (!cache[e]) {
    bool cacheit=true;
    /* Is this right? */
    for (int i=0;i<t.number_events();i++)
      if ((visited[i]) && (result.dirty->find( i ) != result.dirty->end())) {
	cacheit=false;
	break;
      }
    if (cacheit) {
#ifdef __PO_DUMP__
      cout << "Storing result on " << t.get_event( e ) << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
      cache[e] = new event_map();
      *cache[e] = *(result.res);
    } else {
#ifdef __PO_DUMP__
      cout << "Dirty, not storing result on " << t.get_event( e ) 
	   << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
    }
  }
  return( result );
}
//------------------------------------------------------------------------

//--------------------------------------------event_set* get_necessary_set
// Interface function that creates a visited vector used to control
// termination in the recursive_search routine.  This function will return
// a valid pointer to the minimal necessary set for e.  A necessary set is
// a set of events whose firing leads the tel structure t to a timed
// state that where e can fire.  NOTE: it may be possible to get
// an empty necessary set if e is never allowed to fire due to TDC
// semantics.  This needs to be tested!  See exception hanlder in 
// get_dependent_set
//------------------------------------------------------------------------ 
event_map* get_necessary_set( const tel& t,
			      const timed_state& sc,
			      const event_type& e,
			      const event_type& e_conflict,
			      const event_set& Ef) { 

  map_dirtyADT result;

  vector<bool> visited( t.number_events(), false );
  visited[e] = true;
#ifdef __PO_DUMP__
  cout << "--------------------------------------------------------------"
       << endl;
  cout << "Start working on " << t.get_event( e_conflict );
  recursive_depth = 0;
#endif
  
  /* Mark all occurence of an event as visited when you visit one */
  if (!t.is_dummy_event(e_conflict)/*!t.is_behavior_rule(ej,e)*/) {
    int sig=t.get_signal(e_conflict);
    for (int cnt=(t.get_signals())[sig]->event + (e_conflict % 2 ? 0 : 1);
	 cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
      if ((unsigned)cnt != e_conflict) visited[cnt]=true;
  }
  result = recursive_search( t, sc, e_conflict, Ef, visited );
  /* Cache results obtained from recursive search */
  if (!cache[e_conflict]) {
    bool cacheit=true;
    /* Is this right? */
    for (int i=0;i<t.number_events();i++)
      if ((visited[i]) && (result.dirty->find( i ) != result.dirty->end())) {
	cacheit=false;
	break;
      }
    if (cacheit) {
#ifdef __PO_DUMP__
      cout << "Storing result on " << t.get_event( e_conflict ) 
	   << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
      cache[e_conflict] = new event_map();
      *cache[e_conflict] = *(result.res);
    } else {
#ifdef __PO_DUMP__
      cout << "Dirty, not storing result on " << t.get_event( e_conflict ) 
	   << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
    }
  }
  // Opps, need to free memory in the dirty map.  The memory
  // in the res map will be cleaned by the caller.
  delete result.dirty;
#ifdef __PO_DUMP__
  cout << "End working on " << t.get_event( e_conflict )
       << " with recursion depth " << recursive_depth << endl;
  cout << "--------------------------------------------------------------"
       << endl;
#endif
  return( result.res );
}

//------------------------------------------------------------------------

//--------------------------------------------------void get_dependent_set
// This function returns the set of enabled events that e depends
// on (Ed).  e depends on e' iff any of the following conditions hold:
//
//          1) e # e'
//          2) the firing of e' leads to eventually firing an event
//             e'' where e'' # e
//
// NOTE: This function requires a recursive call because: if e' is 
// added to Ed, then Ed must include any events that e' depends on too.
// This satisfies the correctness preserving condition for PO reduction
// in that all possible choice interleavings are explored.
//------------------------------------------------------------------------
bool get_dependent_set( const tel& t,
			const timed_state& sc,
			const event_type& e,
			const event_set& Ef,
			event_set& Ed,
			const unsigned int maxsize ) {

  // This terminates the recursive calls.  If an event
  // is already in Ed, then Ed already includes its dependent set.
  // If it is not in Ed, then this will add it in the set.
  bool added = ( Ed.insert( e ) ).second;
  if ( !added ) return true;

  /* ERIC: I added this. */
  if (maxsize > 0 && Ed.size() >= maxsize) return false;
  // Get the set of events that conflict with Ed.  This definition of
  // conflicts implies that each e' in Ec shares a common enabling 
  // with the event e (i.e., preset( e ) intersect preset( e' ) 
  // is not equal to the empty set).
  const event_set& Ec( t.get_conflict_set( e ) );
#ifdef __PO_DUMP__
  cout << "Ec(" << t.get_event( e ) << "): ";
  dump( cout, t, Ec ) << endl;
#endif
  // There are not conflicts with this event, the dependent set
  // contains only e.
  if ( Ec.size() == 0 ) {
    return true;
  }
  typedef event_set::const_iterator set_iter;
  typedef event_map::iterator map_iter;
  for ( set_iter i = Ec.begin() ; i != Ec.end() ; ++i ) {

#ifdef __PO_DUMP__
    cout << endl;
    cout << "CHECKING " << t.get_event( *i );
#endif
    // If event i is enabled (i.e., i is in Ef), then find
    // the dependent set for i.
    if ( Ef.find( *i ) != Ef.end() ) {
#ifdef __PO_DUMP__
      cout << " firable getting dependent set." << endl;
#endif
      /* ERIC: I changed this.*/
      if (!get_dependent_set( t, sc, *i, Ef, Ed, maxsize )) {
#ifdef __PO_DUMP__
	cout << "Found cycle returning false." << endl;
#endif
	return false;
      }
    } 
    // The event cannot fire (i.e., i is not in Ef), so find the
    // minimal necessary set for i.  By implementation, the 
    // necessary set is like the dependent set, only it contains
    // events that are not in direct conflict with e.  Rather the
    // events lead to the enabling of e.
    // NOTE: the necessary set contains only enabled events, but I 
    // believe it may be possible to get an empty En.  Either way,
    // I am checking to see if it can happen.
    else {
#ifdef __PO_DUMP__
      cout << " not firable getting necessary set." << endl;
#endif
      event_map* ptr_Et = get_necessary_set( t, sc, e, *i, Ef );
      if ( ptr_Et->size() == 0 ) {
#ifdef __PO_DUMP__
	cout << " Found empty necessary set." << endl;
#endif
	// Remember to clean memory on these failed calls
	delete ptr_Et;
	continue; 
      }

#ifdef __PO_DUMP__
      cout << "En(" << t.get_event( *i ) << "): ";
      dump( cout, t, *ptr_Et ) << endl;
#endif
      // For each member of the necessary set, find its dependent
      // set.  NOTE: j is added in the recursive call.
      for ( map_iter j = ptr_Et->begin() ; j != ptr_Et->end() ; ++j ) {
	if ( is_enough_slack( t, sc, e, *j ) ) {
	  /* ERIC: I changed this. */
	  if (!get_dependent_set( t, sc, EVENT((*j)), Ef, Ed, maxsize )) {
	    // Must Clean memory before return Chris
	    delete ptr_Et;
	    return false;
	  }
	} else {
#ifdef __PO_DUMP__
	  cout << "Not enough slack." << endl;
#endif
	}
      }
      delete ptr_Et;
    }
  }
  return true;
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
void init_cache( unsigned cache_size ) {
  typedef event_map* event_map_ptr;
  cache = new event_map_ptr[cache_size];
  for ( unsigned i = 0 ; i < cache_size ; ++i ) 
    cache[i] = NULL;
}

void reinit_cache( unsigned cache_size ) {
  for ( unsigned i = 0 ; i < cache_size ; ++i ) {
    if ( cache[i] ) {
      delete cache[i];
    }
    cache[i] = NULL;
  }
}

void delete_cache( unsigned cache_size ) {
  for ( unsigned i = 0 ; i < cache_size ; ++i ) {
    if ( cache[i] ) {
      delete cache[i];
    }
  }
  delete [] cache;
}

//------------------------------------------------------------------------

//----------------------------------------event_set* choose_best_ample_set
event_set* choose_best_ample_set( event_set* ptr_Ea     ,
				  event_set* ptr_Ea_new ,
				  const event_set& Ef   ) {
  // Are all of the events in Ea_new firable?
  // If they are not, then do not save this as a valid ready set.
  event_set Ediff;
  set_difference( ptr_Ea_new->begin(), ptr_Ea_new->end()   ,
		  Ef.begin(), Ef.end()                     ,
		  inserter( Ediff, Ediff.begin() )           );
  unsigned int size_of_Ediff( Ediff.size() );
  if ( size_of_Ediff != 0 ) {
    delete ptr_Ea_new;
    return( ptr_Ea );
  }
  // If the dependent set is size 1, then we are done.
  unsigned int size_of_Ea_new( ptr_Ea_new->size() );
  if ( size_of_Ea_new == 1 ) {
    delete ptr_Ea;
    return( ptr_Ea_new );
  }
  // Examine the sizes of Ea_new and Ea.  Return the set with
  // the smaller carndinality as the new ample set.
  unsigned int size_of_Ea( ptr_Ea->size() );
  if ( size_of_Ea_new < size_of_Ea || size_of_Ea == 0 ) {
    delete ptr_Ea;
    return( ptr_Ea_new );
  }
  delete ptr_Ea_new;
  return( ptr_Ea );
}
//------------------------------------------------------------------------

//------------------------------------------------event_set* get_ample_set
// find_ample_set returns Ea subset-equal Ef by applying a partial order 
// reduction on invisible events.  In this function, invisible is 
// defined as any sequencing event in the tel structure t.
// NOTE: This definition may need to evolve and change to accommodate
// verification applications.  However, this initial implementation is
// for synthesis purposes.  For synthesis, we are making any events not
// in the module being synthesized sequencing events, thus making them
// invisible to the specification and candidates for the partial order
// reduction.
//------------------------------------------------------------------------
event_set* find_ample_set( const tel& t          , 
			   const timed_state& sc ,
			   const _event_list& Ef ,
			   const event_set& Een    ) {
#if defined( __PO_DUMP__ ) || defined( __READY__ )
  cout << endl << "PO Reduction" << endl << "------------" << endl;
  cout << "Ef";
  dump( cout, t, Ef ) << endl;
#ifdef __PO_DUMP__
  sc.dump(cout, t);
  cout << endl;
#endif
#endif
  event_set Etmp;
  copy( Ef.begin(), Ef.end(), inserter( Etmp, Etmp.begin() ) );
  event_set* ptr_Ea( new event_set() );
  for ( _event_list::const_iterator i( Ef.begin() ) ; i != Ef.end() ; ++i ) {
    // 1) Get the dependent( i )
    event_set* ptr_Et( new event_set() );
    if ( !get_dependent_set( t, sc, *i, Een, *ptr_Et, ptr_Ea->size() ) ) {
      // Free memory for when the call fails.
      delete ptr_Et;
      continue;
    }
#if defined( __PO_DUMP__ ) || defined( __READY__ )
    cout << "Ed(" << t.get_event( *i ) << "): ";
    dump( cout, t, *ptr_Et ) << endl;
#endif
    // 2) If dependent( i ) is better than the current ample_set,
    //    then set Ea = dependent( i ).
    ptr_Ea = choose_best_ample_set( ptr_Ea, ptr_Et, Etmp );
    if ( ptr_Ea->size() == 1 ) return( ptr_Ea );
  }
  // 3) There were no invisible events in Ef, or there did not exist a
  //    ready set that contained only fireable events.  Make Ef = Ea 
  //    because all of these events need to be interleaved in the exploration 
  //    process.
  if ( ptr_Ea->size() == 0 ) {
    copy( Ef.begin(), Ef.end(), inserter( *ptr_Ea, (*ptr_Ea).begin() ) );
  }
  return( ptr_Ea );
}
//------------------------------------------------------------------------

//------------------------------------------------event_set* get_ample_set
event_set* get_ample_set( const tel& t            , 
			  const timed_state& sc   ,
			  const _event_list& Ef   ,
			  const event_set& Een      ) {
  event_set* ptr_Ea = find_ample_set( t, sc, Ef, Een );
  return( ptr_Ea );
}

//------------------------------------------------------------------------

//---------------------------------------------event_set* recursive_search
// This function recursively searches back from the preset of e looking 
// for a minimal set of fireable events that will enable e to fire 
// (i.e., the necessary set for e). To find the necessary set, the function 
// will find all possible necessary sets; and it return the set with the
// smallest cardinality and the largest delay.  NOTE: at each return
// the visited list needs to be reset to false for event e.  This list
// is used to detect dependent cycles in the graph, and to record where
// the algorithm has already at a specific call level.
//------------------------------------------------------------------------
map_dirtyADT prune_recursive_search( const tel& t,
				     const timed_state& sc,
				     const event_type& e,
				     const event_set& Ef,
				     vector<bool>& visited,
				     event_type ep = 0) {

#ifdef __PO_DUMP__
  cout << " visiting " << t.get_event( e ) << endl;
#endif

  map_dirtyADT result;
  result.res = new event_map();
  result.dirty = new event_set();
  // This event has already been visited on _this_ search path! This
  // denotes a loop creating a circular dependency (i.e., the firing of
  // e leads the enabling of e).  Return an empty necessary set.
  if ( visited[e] ) { 
#ifdef __PO_DUMP__
    cout << "Found cycle (visited) returning ";
    dump( cout, t, *(result.res) );
    cout << endl;
    cout << endl;
#endif
    /* THIS IS LIKELY NEEDED, BUT MAKES IT GO SLOW */
    //result.dirty->insert( e );
    return( result ); 
  }

  /* Cannot be causal if found another event on signal ep */

  /* CHRIS: Move this to setup visited before calling it */
  if (ep > 0)
    
    if ( (t.get_signal( e ) >= 0) &&
	 (t.get_signal( e ) == t.get_signal( ep )) ) {
#ifdef __PO_DUMP__
      cout << "Found another event on the signal returning " << endl;
#endif
      //delete result.res;
      //result.res = NULL;
      visited[e] = true;
      result.dirty->insert( e );
      return( result ); 
    }

  // If this event is fireable, then insert it into En and return.
  // NOTE: The delay is zero because the delay for e will be considered
  // in is_enough_slack when causality is assigned.
  if ( Ef.find( e ) != Ef.end() ) {
    result.res->insert( make_pair( e, 0 ) );
#ifdef __PO_DUMP__
    cout << "Is fireable added to necessary returning ";
    dump( cout, t, *(result.res) );
    cout << endl;
    cout << endl;
#endif
    return( result );
  }
  
  // If there is a cached result, then copy it into result and return
  calls++;
  if ( cache[e] ) {
    hit++;
    const event_map& tmp( *cache[e] );
    copy( tmp.begin(), tmp.end()                       , 
	  inserter( *result.res, result.res->begin() )   );
    if ( recursive_depth > 0 )
      --recursive_depth;
#ifdef __PO_DUMP__
    cout << "Found in cache returning ";
    dump( cout, t, *(result.res) );
    cout << endl;
    cout << endl;
#endif
    return( result );
  }
  
  // Get the list of events in the preset of e.  If any of those events
  // conflict, then group them according to their conflict relations
  // (i.e., if e' # e'', the put e' and e'' in the same event set.
  // According to Yoneda's Async 99 paper, because of the conflict 
  // relation, e' or e'' may cause e, thus both must be included in
  // the necessary set for e.
  preset_list marked;
  preset_list nonmarked;
  get_preset_list( t, sc, e, inserter( marked ), inserter( nonmarked ) );
#ifdef  __PO_DUMP__
  cout << endl;
  //sc.dump( cout, t ) << endl;
  cout << "preset[" << t.get_event( e ) << "]";
  dump( cout, t, nonmarked ) << endl;
  cout << "marked[" << t.get_event( e ) << "]";
  dump( cout, t, marked ) << endl;
#endif

  /* Used fact that ep event is marked to fire so can't be causal */
  /* CHRIS: this can be moved onto visited */
  if (ep > 0)
    for ( preset_list::const_iterator i = marked.begin() ; 
	  i != marked.end()                              ; 
	  ++i                                             ) {
      for ( event_set::const_iterator j = (*i)->begin() ; 
	    j != (*i)->end()                            ; 
	    ++j                                           ) {
	event_type ej = (*j);
	if (ep == ej) {
#ifdef __PO_DUMP__
	  cout << "Used fact that potential causal event is marked" << endl;
#endif
	  //delete result.res;
	  //result.res = NULL;
	  visited[e] = true;
	  result.dirty->insert( e );
	  return result;
	}
      }
    }

  int necessary_size = -1;
  int max_delay = -1;
  for ( preset_list::const_iterator i = nonmarked.begin() ; 
	i != nonmarked.end()                              ; 
        ++i                                                 ) {
    // Create a temporary map and dirty set (temp) to store the necessary set 
    // that results from choosing this event_set (*i) to enable e.
    map_dirtyADT temp;
    temp.res = new event_map();
    temp.dirty = new event_set();
    visited[e] = true;
    for ( event_set::const_iterator j = (*i)->begin() ; 
          j != (*i)->end()                            ; 
          ++j                                           ) {
      event_type ej = (*j);
      POSET::bound_type lower_bound = t.lower_bound( ej, e );
      // Get the necessary set for ej and union it into Et.  During the
      // union, update the delays with the lower bound on the rule
      // (ej, e, l, u).
#ifdef __PO_DUMP__
      cout << "Recursive Working on " << t.get_event( e );
#endif	
      ++recursive_depth;

      /* Mark all occurence of an event as visited when you visit one */
      if (!t.is_dummy_event(ej)/*!t.is_behavior_rule(ej,e)*/) {
	int sig=t.get_signal(ej);
	for (int cnt=(t.get_signals())[sig]->event + (ej % 2 ? 0 : 1);
	     cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
	  if ((unsigned)cnt != ej) visited[cnt]=true;
      }
	
      map_dirtyADT rd = prune_recursive_search( t, sc, ej, Ef, visited , ep);

      /* Unmark and cache */ /* ADD TO PO? */
      if (!visited[ej])
	if (!t.is_dummy_event(ej)/*!t.is_behavior_rule(ej,e)*/) {
	  int sig=t.get_signal(ej);
	  /* THIS IS NOT COMPLETELY SAFE! 
	  for (int cnt=(t.get_signals())[sig]->event;
	       cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
	    if ((!cache[cnt]) && (((unsigned)cnt != ej) ||
		(rd.dirty->find( ej ) == rd.dirty->end()))) {
	      bool cacheit=true;
	      for (int i=0;i<t.number_events();i++)
		if ((visited[i]) && 
		    (rd.dirty->find( i ) != rd.dirty->end())) {
		  cacheit=false;
		  break;
		}
	      if (cacheit) {
#ifdef __PO_DUMP__
		cout << "Storing result on2 " << t.get_event( cnt ) 
		     << " in cache ";
		dump( cout, t, *(rd.res) );
		cout << endl;
		cout << endl;
#endif
		cache[cnt] = new event_map();
		*cache[cnt] = *(rd.res);
	      } else {
#ifdef __PO_DUMP__
		cout << "Dirty, not storing result on " << t.get_event( cnt ) 
		     << " in cache ";
		dump( cout, t, *(rd.res) );
		cout << endl;
		cout << endl;
#endif
	      }
	    }
*/
	  for (int cnt=(t.get_signals())[sig]->event + (ej % 2 ? 0 : 1);
	       cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
	    if ((unsigned)cnt != ej) visited[cnt]=false;
	}
      
      combine_maps( rd.res, temp.res, bind2nd( add_to_delay(), lower_bound ) );
      combine_sets( rd.dirty, temp.dirty );
      delete rd.res;
      delete rd.dirty;
    }
    visited[e] = false;
    int tmp_size = temp.res->size();
    int tmp_delay = get_max_delay( *(temp.res) );

    // If the tmp_size is zero, then a loop forming a circular 
    // dependency was discovered in the search (i.e., event e
    // cannot fire until event e fires).
    if ( tmp_size == 0 ) {
      delete result.res;
      delete result.dirty;
#ifdef __PO_DUMP__
      cout << "Found cycle returning ";
      dump( cout, t, *(temp.res) );
      cout << endl;
      cout << endl;
#endif
      /* Newly added, does it do anything? */
      if (!cache[e]) {
	bool cacheit=true;
	/* Is this right? */
	for (int i=0;i<t.number_events();i++)
	  if ((visited[i]) && 
	      (temp.dirty->find( i ) != temp.dirty->end())) {
	    cacheit=false;
	    break;
	  }
	if (cacheit) {
#ifdef __PO_DUMP__
	  cout << "Storing result on3 " << t.get_event( e ) << " in cache ";
	  dump( cout, t, *(temp.res) );
	  cout << endl;
	  cout << endl;
#endif
	  cache[e] = new event_map();
	  *cache[e] = *(temp.res);
	} else {
#ifdef __PO_DUMP__
	  cout << "Dirty, not storing result on " << t.get_event( e ) 
	       << " in cache ";
	  dump( cout, t, *(temp.res) );
	  cout << endl;
	  cout << endl;
#endif
	}
      }
      return( temp );
    }
    // The PO reduction gets the best performance by using the smallest
    // En that it can.  Thus, this code searches over all possible events
    // that can be used as a necessary set.  It selects a set that 
    // has the smallest cardinality (0 is optimal) and the largest minimum
    // delay for any given member.  The delay is included in the selection
    // process because it is possible to exclude members from the ready
    // set if they can not fire soon enough to enable a dependent event.
    if ( ( tmp_size < necessary_size )                           ||
	 ( tmp_size == necessary_size && tmp_delay > max_delay ) || 
	 ( necessary_size == -1 )                                   ) {
      delete result.res;
      delete result.dirty;
      result.res = temp.res;
      result.dirty = temp.dirty;
      necessary_size = tmp_size;
      max_delay = tmp_delay;
    }
    // Et must be size zero has more members than the current En.
    // Free its memory and move on.
    else {
      delete temp.res;
      delete temp.dirty;
    }
  }
#ifdef __PO_DUMP__
  cout << "Working on " << t.get_event( e ) << " returning ";
  dump( cout, t, *(result.res) );
  cout << endl;
  cout << endl;
#endif
  if (!cache[e]) {
    bool cacheit=true;
    /* Is this right? */
    for (int i=0;i<t.number_events();i++)
      if ((visited[i]) && (result.dirty->find( i ) != result.dirty->end())) {
	cacheit=false;
	break;
      }
    if (cacheit) {
#ifdef __PO_DUMP__
      cout << "Storing result on4 " << t.get_event( e ) << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
      cache[e] = new event_map();
      *cache[e] = *(result.res);
    } else {
#ifdef __PO_DUMP__
      cout << "Dirty, not storing result on " << t.get_event( e ) 
	   << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
    }
  }
  return( result );
}
//------------------------------------------------------------------------

//--------------------------------------------event_set* get_necessary_set
// Interface function that creates a visited vector used to control
// termination in the recursive_search routine.  This function will return
// a valid pointer to the minimal necessary set for e.  A necessary set is
// a set of events whose firing leads the tel structure t to a timed
// state that where e can fire.  NOTE: it may be possible to get
// an empty necessary set if e is never allowed to fire due to TDC
// semantics.  This needs to be tested!  See exception hanlder in 
// get_dependent_set
//------------------------------------------------------------------------ 
event_map* prune_get_necessary_set( const tel& t,
				    const timed_state& sc,
				    const event_type& e,
				    const event_type& e_conflict,
				    const event_set& Ef) {

  map_dirtyADT result;

  vector<bool> visited( t.number_events(), false );
  visited[e] = true;
#ifdef __PO_DUMP__
  cout << "--------------------------------------------------------------"
       << endl;
  cout << "Start working on " << t.get_event( e_conflict );
  recursive_depth = 0;
#endif

  /* Mark all occurence of an event as visited when you visit one */
  if (!t.is_dummy_event(e_conflict)/*!t.is_behavior_rule(ej,e)*/) {
    int sig=t.get_signal(e_conflict);
    for (int cnt=(t.get_signals())[sig]->event + (e_conflict % 2 ? 0 : 1);
	 cnt<t.number_events() && t.get_signal(cnt)==sig; cnt+=2)
      if ((unsigned)cnt != e_conflict) visited[cnt]=true;
  }
  //if (!(t.is_behavior_rule(e_conflict,f)))
    result = prune_recursive_search( t, sc, e_conflict, Ef, visited, e );
    //else
    //result = prune_recursive_search( t, sc, e_conflict, Ef, visited );
  if (!cache[e_conflict]) {
    bool cacheit=true;
    /* Is this right? */
    for (int i=0;i<t.number_events();i++)
      if ((visited[i]) && (result.dirty->find( i ) != result.dirty->end())) {
	cacheit=false;
	break;
      }
    if (cacheit) {
#ifdef __PO_DUMP__
      cout << "Storing result on " << t.get_event( e_conflict ) 
	   << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
      cache[e_conflict] = new event_map();
      *cache[e_conflict] = *(result.res);
    } else {
#ifdef __PO_DUMP__
      cout << "Dirty, not storing result on " << t.get_event( e_conflict ) 
	   << " in cache ";
      dump( cout, t, *(result.res) );
      cout << endl;
      cout << endl;
#endif
    }
  }
  // Opps, need to free memory in the dirty map.  The memory
  // in the res map will be cleaned by the caller.
  delete result.dirty;
#ifdef __PO_DUMP__
  cout << "End working on " << t.get_event( e_conflict )
       << " with recursion depth " << recursive_depth << endl;
  cout << "--------------------------------------------------------------"
       << endl;
#endif
//  if (recursive_depth > max_depth) {
  //    max_depth=recursive_depth;
  //    cout << "DEPTH = " << recursive_depth << endl;
  //  } 
  return( result.res );
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Returns false if the enabling event for rule cannot be causal in 
// the current zone Me with the given ruleset Rb or if it can be causal
// but there is another causal candidate that will create a zone that 
// is larger than that from the causalility with enabling, and this 
// candidate will still make sure that the rule for enabling and enabled
// is satisfied in the other causality.
//------------------------------------------------------------------------
bool is_causal_candidate( const tel& t,
			  const rule_set& Rb,
			  const POSET& Me,
			  rule_set::const_iterator rule ) {
  event_type enabling( t.enabling( *rule ) );
  event_type enabled( t.enabled( *rule ) );
  // Unbounded rules can always be causal
  /* KEEP THIS OR NOT? */
  //if ( t.is_unbounded( enabling, enabled ) ) return ( true );
  POSET::bound_type upper( t.upper_bound( enabling, enabled ) );
  POSET::bound_type lower( t.lower_bound( enabling, enabled ) );
#ifdef __DUMP__
  cout << endl << "Causal candidate: (" << t.get_event( enabling ) << ","
       << t.get_event( enabled ) << ") -> "
       << "[" << lower << "," << upper << "]" << endl;
#endif
  for ( rule_set::const_iterator i = Rb.begin() ; i != Rb.end() ; ++i ) {
    if ( rule == i ) continue;
    event_type ei( t.enabling( *i ) );
    // 1) If the enabling event is _not_ in the POSET, then move on
    //    to the next event
    if ( !Me.is_in_POSET( ei ) ) continue;
    // 2) Get the minimum separation between enabling and the enabling
    //    event for the current rule being considered ei
    POSET::bound_type min_separation( -1 * Me.get_separation(enabling,ei) );
#ifdef __DUMP__
    cout << "considering " << t.get_event( ei ) 
	 << " with separation " << min_separation 
	 << " on [" << t.lower_bound( ei, enabled ) 
	 << "," << t.upper_bound( ei, enabled ) << "]" << endl;
#endif
    // 3) If the minimum separation between enabling and ei plus the
    //    upper bound for the rule between ei and enabled is less than
    //    the upper bound for rule for enabling and enabled, then the rule
    //    on ei and enabled does not exclude enabling from defining the
    //    largest separations in the POSET.
    POSET::bound_type bound( t.upper_bound( ei, enabled ) );
    /* I ADDED THIS */
    if (( min_separation == 0 ) && 
	((t.is_behavior_rule( ei, enabled )) ||
	 (!t.is_behavior_rule( enabling, enabled ))))
      continue;

    /* SHOULD THE BOUNDS BE USED OR NOT??? */
    if ( min_separation + bound < upper ) continue;
    // 4) If the minimum separation between enabling and ei plus the
    //    lower bound for the rule between ei and enabled is less than
    //    the lower bound for the rule for enabling and enabled, then the rule
    //    on ei and enabled does allow the rule for enabling and enabled to
    //    satisfied before enabled fires, so the rule on ei and enabling
    //    does not exclude enabling from the POSET.
    bound = t.lower_bound( ei, enabled );
    /* CHRIS CHANGED < to <= */
    /* DON'T CHECKED */
    if ( min_separation + bound < lower ) continue;
    // 5) The rule for ei and enabled excludes enabling from defining the 
    //    maximum separation in the zone; and thus, it is does not need 
    //    to be considered as a causal candidate in this grouping.
#ifdef __DUMP__
    cout << "Returning false" << endl;
#endif
    return( false );
  }
#ifdef __DUMP__
  cout << "Returning true" << endl;
#endif
  return( true );
}

			  
//------------------------------------------------bool is_causal_candidate
bool is_causal_candidate_2( const tel& t                          , 
			    const timed_state& sc                 , 
			    const event_type& candidate           , 
			    const event_map::value_type necessary ,
			    const event_type& enabling            ,
			    const event_type& enabled               ) {
#ifdef __DUMP__
  cout << endl << "Checking " << t.get_event( candidate ) << " for "
       << t.get_event( enabled ) << " considering (" 
       << t.get_event( EVENT( necessary ) ) << "," 
       << DELAY( necessary ) << ")" << endl;
  cout << "Enabling event is " << t.get_event( enabling ) << endl << endl;
#endif
  typedef rule_set::iterator iterator;
  typedef front_insert_iterator<rule_set> rule_set_insert;
  event_type e( EVENT( necessary ) );
  rule_set Rb;
  // 1) Get the set of rules necessary to fire e
  get_rule_set( e, sc.get_untimed_st(), t, Rb );

  /* CHRIS ADDED THIS */
  enabled_set Rbx;
  level_expand( t, Rb.begin(), Rb.end(), Rb, sc.get_untimed_st(),
		inserter( Rbx, Rbx.begin() ) );
  /* TO HERE */

  // 2) Make a copy of the POSET in sc
  POSET Me( sc.get_POSET() );
  // 3) Add into the POSET the rules from Rb with their lower bounds set
  Me.add( Rbx, t );
  // 6) If this is a valid POSET and the latest time for candidate is
  //    greater than the earliest time for the enabling, then the
  //    candidate could be causal to enabled.
  if ( Me.is_consistent() ) {
#ifdef __DUMP__
    Me.dump( cout, t ) << endl;
#endif
    // 4) Compute the latest time that the candidate event can arrive
    //    as a causal event.

    /* CHRIS ADDED THIS */
    bool use_bounds;
    if (((t.is_behavior_rule(candidate,enabled)) &&
	 (t.is_behavior_rule(enabling,enabled))) &&
	(t.upper_bound(candidate,enabled) != t.upper_bound(enabling,enabled) ||
	 t.lower_bound(candidate,enabled) != t.lower_bound(enabling,enabled)))
      use_bounds=true;
    else
      use_bounds=false;

    POSET::bound_type bound;
    if (use_bounds)
      bound = t.upper_bound( candidate, enabled );
    else
      bound = 0;

    POSET::bound_type sep = Me.get_separation( candidate, e );
    POSET::bound_type latest_candidate = ( sep == POSET::_UNBOUNDED_ ||
                                           bound == INFIN            ) ?
      POSET::_UNBOUNDED_ : sep + bound;
    // 5) Compute the earliest time that enabling can arrive as a causal
    //    event.
    POSET::bound_type earliest_enabling;
    if (use_bounds)
      earliest_enabling = DELAY(necessary) + t.lower_bound(enabling, enabled);
    else
      earliest_enabling = DELAY( necessary );

#ifdef __DUMP__
    cout << endl << "Earliest enabling is " << earliest_enabling;
    cout << endl << "Latest candidate is " << latest_candidate << endl;
#endif
    /* CHRIS: <= or < */
    if ( earliest_enabling < latest_candidate ) {
#ifdef __DUMP__
      cout << "Returning true" << endl;
#endif 
      return( true );
    }
  }
  return( false );
}

//------------------------------------------------------------------------

//------------------------------------------------bool is_causal_candidate
// This function returns true if enabling can be a causal candidate for 
// the event enabled that does not yet have enough tokens or level-rules to 
// fire (i.e., it is possible for other tokens or rules to become
// satisfied in time for clock on enabling to still be causal).
bool is_causal_candidate_3( const tel& t                 ,
			    const timed_state& sc        ,
			    event_type enabling          ,
			    event_type enabled           ,
			    const preset_list& nonmarked ,
			    const event_set& Een           ) {
  typedef event_map::iterator map_iter;
  /* KEEP? */
  //if ( t.upper_bound( enabling, enabled ) == INFIN ) {
  //  return( true );
  //}
  for ( preset_list::const_iterator i = nonmarked.begin() ; 
        i != nonmarked.end()                              ; 
        ++i                                                 ) {
    // NOTE: If any member of this conflict group allows enabling to be
    // causal, then enabling is still valid.  Enabling can only be removed
    // if all members of the conflict group prevent it from being causal.
    bool keep( false );
    for ( event_set::const_iterator j = (*i)->begin() ; 
          j != (*i)->end()                            ; 
          ++j                                           ) {
      // 1) Get the enabling event from the rule j.
      event_type ej( *j );
      // 2) Calculate the necessary set for the enabling event
      event_map* ptr_Et = prune_get_necessary_set( t, sc, enabling, ej, Een );
      if (!ptr_Et) continue;
#ifdef __DUMP__
      cout << endl << "En(" << t.get_event( ej ) << "):";
      dump( cout, t, *ptr_Et ) << endl;
#endif
      // 3) Check each event in En to see if it can be a causal candidate.
      //    If there exists an event the prevents enabling from being
      //    causal, then return false
      for ( map_iter k = ptr_Et->begin() ; k != ptr_Et->end() ; ++k ) {
	keep = keep || 
	  is_causal_candidate_2( t, sc, enabling, *k, ej, enabled );
      }
      // 4) Free memory held in the En set
      delete ptr_Et;
      // 5) If the keep is unset, then enabling cannot ever be causal so
      //    break out of for loop
      if ( keep ) break;
    }
    // 6) enabling is not a keeper, so clean memory and return
    if ( !keep ) {
      return( false );
    }
  }
  // 8) enabling is a keeper, so return true
  return( true );
}
//------------------------------------------------------------------------

extern ostream& dump( ostream& s, const tel& t, const rule_set& Rb );

//--------------------------------------------------void get_causal_events
// Adds to ptr_Ek any enabling events from the set of untimed enabled 
// events that are currently in the POSET Me.  It will only add this
// event if it is claimed by some valid causality assignment.  If an
// event is not part of a causality assignment for an untimed enabled 
// event, then it is not added to the keep list.
//------------------------------------------------------------------------
void get_causal_events_2( const tel& t              ,
			  const timed_state& sc     ,
			  const enabled_set& Een_Rb ,
			  event_set* ptr_Ek           ) {
  typedef enabled_set::const_iterator enabled_set_iter;
  typedef rule_set::const_iterator rule_set_iter;

  const POSET& Me( sc.get_POSET() );
  for ( enabled_set_iter i = Een_Rb.begin() ; i != Een_Rb.end() ; ++i ) {
    // 1) Get the set of marked rules enabling the event *i
    const rule_set& Rb( *i );
    bool added( false );
    int the_chosen( -1 );
    for ( rule_set_iter j = Rb.begin() ; j != Rb.end() ; ++j ) {
      // 2) Get the enabling event
      event_type causal = t.enabling( *j );
      //cout << "causal = " << t.get_event( causal ) << endl;
      // 3) If the events is already in the keep list, move on.
      if ( ptr_Ek->find( causal ) != ptr_Ek->end() ) continue;
      //cout << "not in keep" << endl;
      // 4) If the event is not found in the POSET, move on.
      if ( !Me.is_in_POSET( causal ) ) continue;
      //cout << "in POSET" << endl;
      // 5) Store the chosen one ( hopefully an event from a nonlevel rule
      if ( the_chosen == -1 ) the_chosen = (int)causal;
      //cout << "chosen = " << t.get_event(the_chosen) << endl;
      // 6) Check that this is a possible causality assignment.
      if ( is_prune_set() ) 
	if ( !is_causal_candidate( t, Rb, Me, j ) ) continue;
      //cout << "is causal" << endl;
      added = true;
      // 7) Add the event to the keep list.
      ptr_Ek->insert( causal );
    }
    // NOTE: If we have not added a causal assignment for an event because
    // all casualities create the same size zone, then add the last causal
    // candidate that was considered as all essential create the same 
    // zone only is a different dimension
    // IMPORTANT: This can be done better, this is going to include for 
    // causal grouping that has not assignments an actual assignment.
    // This may include more things than wanted.
    /* CHRIS: WHAT DOES THIS DO?
    if ( !added && the_chosen != -1 ) {
      ptr_Ek->insert( (event_type)the_chosen );
      }*/
  }
}
//------------------------------------------------------------------------

//--------------------------------------------------void get_causal_events
// Add events to the keep list that might be causal to an event that is 
// not yet untimed enabled.
void get_causal_events_3( const tel& t          ,
			  const timed_state& sc ,
			  const event_set& Een  ,
			  event_set* ptr_Ek       ) {
  const POSET& Me( sc.get_POSET() );

  //cout << "START" << endl;
  for ( event_type f = 1 ; f < (event_type)t.number_events() ; ++f ) {
    // 1) If this event is enabled, then move on to the next 
    //    event because causality on an enabled f is computed elsewhere.

    // Cache to keep track of events checked 
    event_set* ptr_cache( new event_set() );

    if ( Een.find( f ) != Een.end() ) continue;

    //cout << "  Checking " << t.get_event(f) << endl;
    
    // 2) Get the preset of f; get_preset_list divides the preset 
    //    into rules that are marked and rules that are nonmarked.
    preset_list marked;
    preset_list nonmarked;

    get_preset_list( t, sc, f, inserter( marked ), inserter( nonmarked ) );
    for ( preset_list::const_iterator i = marked.begin() ; 
        i != marked.end()                                ; 
        ++i                                                ) {
      for ( event_set::const_iterator j = (*i)->begin() ; 
	    j != (*i)->end()                            ; 
	    ++j                                           ) {
	// 3) Get the current enabling event from the marked rule
    
	event_type enabling( *j );

	//cout << "    Enabling " << t.get_event(enabling) << endl;

	// 4) If this event does not level match in the current state,
	//    then it is not the event that fired to mark the rule.
        //    NOTE: is_level_matched returns true if enabling is a
        //    pure dummy event (i.e., events[enabling]->type == DUMMY )
	if ( !sc.is_level_matched( t, enabling ) ) continue;

	//cout << "      Level matched " << endl;

	// 5) If the enabling event is already in the Ek set, then move to 
	//    the next event.  It is already required by some untimed enabled 
	//    event.
	if ( ptr_Ek->find( enabling ) != ptr_Ek->end() ) continue;

	// If event already checked then skip
	if ( ptr_cache->find( enabling ) != ptr_cache->end() ) continue;

	//cout << "      Not cached " << endl;

	// 6) If the enabling event is not in the POSET, then move to the 
	//    next event.  It has already been pruned in an earlier state.
	if ( !Me.is_in_POSET( enabling ) ) continue;

	//cout << "      In POSET " << endl;

#ifdef __DUMP__
	cout << "Considering " << t.get_event( enabling ) 
	     << " as causal candidate for " << t.get_event( f ) << endl;
	cout << "Found " << t.get_event( enabling ) 
	     << " in a " << (t.is_behavior_rule(enabling,f) ? "behavior " 
			     : "level " ) << "rule" << endl;
#endif
	// If prunning is enabled, then this will use the necessary set
	// calculation to prune as aggressively as possible.
	if ( is_prune_set() ) {
	  // 7) If the enabling event can be a causal event for the enabled
	  //    event, then add it to the Ek set.
	  if ( is_causal_candidate_3( t, sc, enabling, f, nonmarked, Een ) ) {
#ifdef __DUMP__
	    cout << "  " << t.get_event(enabling) 
	    	 << " is causal candidate for " 
	    	 << t.get_event(f) << endl;
#endif
	    ptr_Ek->insert( enabling );
	  } else {
	    //	    cout << "  " << t.get_event(enabling) 
	    //	 << " is NOT causal candidate for " 
	    //	 << t.get_event(f) << endl;
	  }
	}
	// No prunning is set, so things required by the marking must remain
	else {
	  ptr_Ek->insert( enabling );
	}
	ptr_cache->insert( enabling );
      }
    }
    delete ptr_cache;
  }
}
//------------------------------------------------------------------------

//--------------------------------------------event_set* get_causal_events
event_set* get_causal_events( const tel& t                     , 
			      const timed_state& sc            ,
			      const event_set& Een             ,
			      enabled_set_expander& Een_expand   ) {
#ifdef __DUMP__
  cout << endl << "Computing Potential Causal Assignments";
  cout << endl << "--------------------------------------" << endl;
  sc.dump( cout, t ) << endl;
#endif
  // 2) Ek is the set of events that must stay in the POSET (i.e.,
  //    the casual events )
  event_set* ptr_Ek( new event_set() );
  // 3) Get the required casual events for the events that are untimed
  //    enabled in sc
  do {
    enabled_set Een_Rb( Een_expand.get_Een() );
    get_causal_events_2( t, sc, Een_Rb, ptr_Ek );
  } while ( Een_expand.next() );
#ifdef __DUMP__
  	cout << endl << "Potential causality for enabled events: ";
  	dump( cout, t, *ptr_Ek ) << endl;
#endif
  // 4) Get the required causal events for the events that are not
  //    untimed enabled in sc, but have some of their behavior
  //    rules marked.

#ifdef __DUMP__
  cout << endl << "Checking: ";
  dump( cout, t, Een ) << endl;
  cout << endl << "--------------------------------------" << endl;
#endif

  get_causal_events_3( t, sc, Een, ptr_Ek );
  // 6) Return the set of events that must be kept in the POSET.
#ifdef __DUMP__
  cout << endl << "Potential causals: ";
  dump( cout, t, *ptr_Ek ) << endl;
  cout << endl << "--------------------------------------" << endl;
#endif
  return( ptr_Ek );

}
//------------------------------------------------------------------------
