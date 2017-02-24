//////////////////////////////////////////////////////////////////////////
// @name wrapper for tel object that is a collection of various structs.
// @version 0.1 alpha
//
// (c)opyright 2000 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "telwrapper.h"
#include "error.h"

//#define __DUMP__

//////////////////////////////////////////////////////////////////////////
//

//------------------------------------------------------------------------
void tel::insert_and_absorb( const event_set& Ec  ,
			     preset_list& P   ) {
  if ( Ec.empty() ) return;
  preset_list::iterator i( P.begin() );
  while ( i != P.end() ) {
    const event_set& Ep( **i );
    event_set Etmp;
    set_intersection( Ep.begin(), Ep.end()           ,
		      Ec.begin(), Ec.end()           ,
		      inserter( Etmp, Etmp.begin() )   );
    if ( !Etmp.empty() ) { 
      if ( Ec.size() == Ep.size() && Ec.size() == Etmp.size() ) return;
      if ( Ep.size() > Ec.size() && Ec.size() == Etmp.size() ) return;
      if ( Ep.size() < Ec.size() && Ep.size() == Etmp.size() ) {
	P.erase( i );
	break;
      }
    }
    ++i;
  }
  P.insert( new event_set( Ec ) );
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Returns true iff e conflicts with every event in Ec
bool tel::all_input_output_conflict( const event_set& Ec ,    
				     const event_type& e   ) {
  event_set::iterator i( Ec.begin() );
  while ( i != Ec.end() ) {
    const event_type& tmp( *i++ );
    if ( !is_input_conflict( tmp, e )          && 
	 !is_output_conflict( tmp, e)  ) {
	return( false );
    }
  }
  return( true );
}
//------------------------------------------------------------------------

/* NOT QUITE RIGHT FOR MULTIPLE BEHAVIORAL RULES WITH LEVELS */

bool tel::check_complete( const event_set& Ec       , 
			  const event_type& enabled )
{
  for (int i=1;i < nevents; i++) {
    if (is_behavior_rule((const event_type&)i,enabled))
      for (level_exp curlevel=get_expression(i,enabled);curlevel;
	   curlevel=curlevel->next) { 

#ifdef __DUMP__
	printf("Checking %s\n",curlevel->product);
#endif
	int k;
	for (k=0;k<nsigs;k++)
	  if (curlevel->product[k]!='X') break;
	if (k==nsigs) continue;

	event_set::iterator j( Ec.begin() );
	bool checks=false;
	while ( j != Ec.end() ) {
	  const event_type& tmp( *j++ );
	  if (((tmp % 2==1) && (curlevel->product[get_signal(tmp)] == '1') ||
	       (tmp % 2==0) && (curlevel->product[get_signal(tmp)] == '0'))) {
#ifdef __DUMP__
	    cout << "checks " << get_event(tmp) << endl;
#endif
	    checks=true;
	  }
	}
	if (!checks) return false;
      }
  }
  return true;
}

/* NOT QUITE RIGHT FOR MULTIPLE BEHAVIORAL RULES WITH LEVELS */

bool tel::is_disjunctive( const event_type& tmp, 
		     const event_type& e, 
		     const event_type& enabled )
{
  if ((get_signal(e) == get_signal(tmp)) &&
      (e % 2 != tmp % 2))
    return false;
  for (int i=1;i < nevents; i++) {
    if (is_behavior_rule((const event_type&)i,enabled))
      for (level_exp curlevel=get_expression(i,enabled);curlevel;
	   curlevel=curlevel->next) 
	if (((e % 2==1) && (curlevel->product[get_signal(e)] == '1') ||
	     (e % 2==0) && (curlevel->product[get_signal(e)] == '0')) &&
	    ((tmp % 2==1) && (curlevel->product[get_signal(tmp)] != '1') ||
	     (tmp % 2==0) && (curlevel->product[get_signal(tmp)] != '0')))
	  return true;
  }
  return false;
}

//------------------------------------------------------------------------
// Returns true iff e conflicts with every event in Ec
bool tel::all_disjunctive_or_same_action( const event_set& Ec       , 
					  const event_type& e       ,
					  const event_type& enabled   ) {
  event_set::iterator i( Ec.begin() );
  while ( i != Ec.end() ) {
    const event_type& tmp( *i++ );
    if ( !is_disjunctive( tmp, e, enabled ) && 
      //( !is_disjunctive_conflict( tmp, e, enabled ) && 
         !is_same_action( tmp, e )                      ) {
	return( false );
    }
  }
  return( true );
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
void tel::get_preset_list( event_set& E              , 
			   event_set& Ec             ,
			   preset_list& P      ,
			   const event_type& enabled ,
                           bool level                   ) {
  /*
#ifdef __DUMP__
  cout << "BEFORE:" << get_event(enabled) << endl;
  cout << "E: ";
  dump(cout,*this,E);
  cout << endl;
  cout << "Ec: ";
  dump(cout,*this,Ec);
  cout << endl;
  cout << "P: ";
  dump(cout,*this,P);
  cout << endl;
#endif
  */
  if ( E.empty() ) {
    if (level && !check_complete( Ec, enabled)) return;
    insert_and_absorb( Ec, P );
    return;
  }
  event_set::iterator i( E.begin() );
  event_type e( *i );
  E.erase( i );
  if ( Ec.empty()                                                    || 
       (!level && all_input_output_conflict( Ec, e ) )               ||
       ( level && 
	 all_disjunctive_or_same_action( Ec, e, enabled ) )    ) {
    Ec.insert( e );
    get_preset_list( E, Ec, P, enabled, level );
    Ec.erase( e );
  }
  get_preset_list( E, Ec, P, enabled, level );
  E.insert( e );

#ifdef __DUMP__
  cout << "AFTER:" << get_event(enabled) << endl;
  cout << "E: ";
  dump(cout,*this,E);
  cout << endl;
  cout << "Ec: ";
  dump(cout,*this,Ec);
  cout << endl;
  cout << "P: ";
  dump(cout,*this,P);
  cout << endl;
#endif
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
void tel::update_marking_key( unsigned rule_count ) {
  _M_markkey_size = rule_count + marking_size();
  markkeyADT* tmp( new markkeyADT[_M_markkey_size] );
  for ( unsigned i = 0 ; i < (unsigned)marking_size() ; ++i ) {
    tmp[i] = new struct markkey_struct;
    tmp[i]->enabling = enabling( i );
    tmp[i]->enabled = enabled( i );
    tmp[i]->epsilon = markkey[i]->epsilon;
  }
  unsigned current = marking_size();
  for ( unsigned i = 0 ; i < (unsigned)number_events() ; ++i ) {
    for ( unsigned j = 0 ; j < (unsigned)number_events() ; ++j ) {
      if ( _M_level_rules[i][j]->ruletype == NORULE ) continue;
      _M_level_rules[i][j]->marking = current;
      tmp[current] = new struct markkey_struct;
      tmp[current]->enabling = i;
      tmp[current]->enabled = j;
      tmp[current++]->epsilon = 0;
    }
  }
  markkey = tmp;
}
//------------------------------------------------------------------------

//---------------------------------------------void set_conflict_relations
void tel::set_conflict_relations( const preset_list& P      ,
				  const event_type& enabled   ) {
  list<pair<event_type,event_type> >& cl( _M_disjunctive_conflict[enabled] );
  for ( preset_list::const_iterator i( P.begin() ) ; i != P.end() ; ++i ) {
    preset_list::const_iterator j( i );
    while( ++j != P.end() ) {
      for ( event_set::const_iterator x( (**i).begin() ) ;
	    x != (**i).end()                             ;
	    ++x                                            ){
	for ( event_set::const_iterator y( (**j).begin() ) ; 
	      y != (**j).end()                             ;
	      ++y                                            ) {
	  cl.push_front( make_pair( *x, *y ) );
	  cl.push_front( make_pair( *y, *x ) );
	  _M_level_rules[*x][*y]->conflict = true;
	  _M_level_rules[*y][*x]->conflict = true;
#ifdef __DUMP__
	  cout << get_event( *x ) << "#"
	       << get_event( *y ) << endl;
#endif
	}
      }
    }
  }
}
//------------------------------------------------------------------------

//------------------------------------------------------create_level_rules
unsigned tel::create_level_rules( int lower          ,
				  int upper          ,
				  level_exp exp      ,
				  event_type enabled   ) {
  unsigned rule_count = 0;
  preset_list P;
  do {
    event_set* ptr_E( new event_set() );
    for ( int i = 0 ; i < get_nsigs() ; ++i ) {
      if ( (exp->product)[i] == 'X' ) continue;
      // I am skipping reset rules at 0 and first rising event begins
      // at location 1 followed by its falling event.
      event_type type( ( (exp->product)[i] == '1' ) ? 1 : 2 );
      for ( int j = type ; j < number_events() ; j += 2 ) {
	// No sense is considering dummy events and they always appear after
	// the real events!
	//if ( is_sequencing_event( j ) ) break;
	if ( events[j]->signal == i ) {
	  // Record this rule being inserted on this product term.
	  ptr_E->insert( j );
	  // Only set SPECIFIED and increment rule_count if this is
	  // a new rule.  See below for case where rule already exists
	  if ( _M_level_rules[j][enabled]->ruletype == NORULE ) {
	    ++rule_count;
	    _M_level_rules[j][enabled]->ruletype |= SPECIFIED;
	  }
	  // Satisfiability insists that lower bounds must be met before
	  // an event can fire.  Thus, if rules (a,f) and (b,f) have a
	  // common level expression, then use the maximum of their lower
	  // and upper bounds for the new rules created for the signals
	  // in the expression.
	  if ( lower > _M_level_rules[j][enabled]->lower )
	    _M_level_rules[j][enabled]->lower = lower;
	  if ( upper > _M_level_rules[j][enabled]->upper )
	    _M_level_rules[j][enabled]->upper = upper;
	}
      }
    }    
    P.insert( ptr_E );
  } while ( ( exp = exp->next ) != NULL );
  set_conflict_relations( P, enabled );
  for ( preset_list::iterator i( P.begin() ) ; i != P.end() ; ++i )
    delete *i;
  return( rule_count );
}
//------------------------------------------------------------------------

//------------------------------------------------------create_level_rules
unsigned tel::create_level_rules() {
  _M_level_rules = new struct rule_struct**[number_events()];
  for ( int i = 0 ; i < number_events() ; ++i ) {
    _M_level_rules[i] = new struct rule_struct*[number_events()];
    for ( int j = 0 ; j < number_events() ; ++j ) {
      _M_level_rules[i][j] = new struct rule_struct;
      _M_level_rules[i][j]->ruletype = NORULE;
      _M_level_rules[i][j]->lower = -1;
      _M_level_rules[i][j]->upper = -1;
      _M_level_rules[i][j]->marking = -1;
      bool conflict( ( events[i]->signal == events[j]->signal ) &&
		     ( ( is_rising_event( i ) && is_rising_event( j ) ) ||
		       ( is_falling_event( i ) && is_falling_event( j ) ) ) );
      _M_level_rules[i][j]->conflict = conflict;
    }
  }
  unsigned rule_count = 0;
  // I am skipping reset rules at location 0
  for ( int i = 1 ; i < number_events() ; ++i ) {
    for ( int j = 1 ; j < number_events() ; ++j ) {
      if ( !is_behavior_rule( i, j ) || !has_level_expression( i, j ) ) 
	continue;
      rule_count += create_level_rules( lower_bound( i, j )    , 
					upper_bound( i, j )    , 
					get_expression( i, j ) , 
					j                        );
    }
  }
  return( rule_count );
}
//------------------------------------------------------------------------

tel::tel() :
  filename( NULL ),
  signals( NULL ),
  events( NULL ),
  rules( NULL ),
  markkey( NULL ),
  nevents( 0 ),
  ninpsig( 0 ),
  nsigs( 0 ),
  nrules( 0 ),
  ncausal( NULL ),
  nord( NULL ),
  startstate( NULL ),
  ndummy( 0 ),
  status( bad ),
  levels( false ),
  conflict_cache( 0 ),
  preset_cache( 0 ),
  _M_level_rules( NULL ),
  _M_markkey_size( 0 ) {
  // NOTHING TO DO
}

tel::tel( char* filename,
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
	  bool verify ) :
  filename( filename ),
  signals( signals ),
  events( events ),
  rules( rules ),
  markkey( markkey ),
  nevents( nevents ),
  ninpsig( ninpsig ),
  nsigs( nsigs ),
  nrules( nrules ),
  ncausal( ncausal ),
  nord( nord ),
  startstate( startstate ),
  ndummy( ndummy ),
  status( good ),
  levels( level ),
  verify( verify ),
  conflict_cache( nevents, event_set() ), 
  preset_cache( nevents,preset_list() ),
  _M_level_rules( NULL ),
  _M_markkey_size( nrules ),
  _M_disjunctive_conflict( nevents ) {
  if ( filename == NULL || events == NULL  ||
       rules == NULL    || markkey == NULL || nevents == 0   ||
       nrules == 0      || startstate == NULL ) {
    status = bad;
  }
  // Clear the color field for tracking disabled events
  // See: level_expand.cc:non_disabling_semantic_support
  for ( int i = 0 ; i < number_events() ; i++ ) {
    events[i]->color = 0;
  }
  // Create the implicit rules defined by the level expressions if this 
  // is a level specification.
  if ( has_levels() ) {
    unsigned rule_count( create_level_rules() );
    update_marking_key( rule_count );
#ifdef __DUMP__
    for ( int i = 0 ; i < number_events() ; ++i ) {
      for ( int j = 0 ; j < number_events() ; ++j ) {
      	if ( _M_level_rules[i][j]->ruletype ) {
      	  cout << endl << "Adding rule (" 
      	       << get_event( i ) << ","
	       << get_event( j ) << ","
	       << _M_level_rules[i][j]->lower << ","
	       << _M_level_rules[i][j]->upper << ")";
	}
      }
    }
    cout << endl << "Created " << rule_count << " new level rules" << endl;
#endif
  }
  // Build the conflict cache.
  // This builds the preset cache too.
  for ( int i = 1 ; i < nevents ; ++i ) {
    insert_iterator<event_set> Ec( conflict_cache[i], 
				   conflict_cache[i].begin() );
    event_set E;
    insert_iterator<event_set> Ep( E, E.begin() );

    event_set E2;
    insert_iterator<event_set> Ep2( E2, E2.begin() );

    for ( int j = 1 ; j < nevents ; ++j ) {
      // NOTE: This defines the conflict cache according to PO 
      // synthesis semantics (i.e., all visible events are in conflict )
      if ( i != j ) {
	if ( ( is_output_conflict( i, j )             && 
	       common_enabling( i, j ) != _NOT_FOUND_    ) ||
	     ((!(events[i]->type & DUMMY)) &&
	      ( is_level_rule((i%2)?i+1:i-1,j) )) || 
	     ((!(events[j]->type & DUMMY)) &&
	      ( is_level_rule((j%2)?j+1:j-1,i) )) || 
	     ( !verify && is_visible( i ) && is_visible( j ) ) ) {
	  *Ec++ = j;
	}
      }
      // PRESET CACHE STUFF
      if ( is_behavior_rule( j, i ) ) {
	*Ep++ = j;
      }
      if ( is_level_rule( j, i ) ) {
	*Ep2++ = j;
      }

    }
    event_set Etmp;
    get_preset_list( E, Etmp, preset_cache[i], i, false );
    get_preset_list( E2, Etmp, preset_cache[i], i, true );
#ifdef __DUMP__
    cout << "preset_cache[" << get_event( i ) << "] = ";
    dump( cout, *this, preset_cache[i] ) << endl;
    cout << "conflict_cache[" << get_event( i ) << "] = ";
    dump( cout, *this, conflict_cache[i] ) << endl;
#endif
  }
}

tel::~tel() {
  for ( preset_list_cache::iterator i( preset_cache.begin() ) ; 
	i != preset_cache.end()                               ;
	++i                                                     ) {
    preset_list& P( *i );
    for ( preset_list::iterator j( P.begin() ) ; j != P.end() ; ++j ) {
      if ( *j == NULL ) continue;
      delete *j;
    }
  }
  if ( _M_level_rules == NULL ) return;
  for ( unsigned i = 0 ; i < _M_markkey_size ; ++i ) {
    delete markkey[i];
  }
  delete [] markkey;
  for ( int i = 0 ; i < number_events() ; ++i ) {
    for ( int j = 0 ; j < number_events() ; ++j ) {
      delete _M_level_rules[i][j];
    }
    delete [] _M_level_rules[i];
  }
  delete [] _M_level_rules;
}

char* tel::get_fname() const {
  return( filename );
}

signalADT* tel::get_signals() const {
  return( signals );
}

ruleADT** tel::get_rules() const {
  return( rules );
}

markkeyADT* tel::get_markkey() const {
  return( markkey );
}

eventADT* tel::get_events() const {
  return( events );
}

char* tel::get_event( int i ) const {
  return( events[i]->event );
}

bool tel::is_immediate( int i ) const {
  return( events[i]->immediate );
}

char* tel::get_startstate() const {
  return( startstate );
}

int tel::number_events() const {
  return( nevents );
}

int tel::marking_size() const {
  return( nrules );
}

unsigned tel::level_expanded_marking_size() const {
  return( _M_markkey_size );
}

bool tel::is_input_conflict( const int& i, const int& j ) const {
  return( rules[i][j]->conflict & ICONFLICT );
}

bool tel::is_output_conflict( const int& i, const int& j ) const {
  return( rules[i][j]->conflict & OCONFLICT );
}

int tel::get_ninpsig() const {
  return( ninpsig );
}

int tel::get_nsigs() const {
  return( nsigs );
}

bool tel::good_tel() const {
  return( status == good );
}

int tel::enabling( const int& marking_index ) const {
  return( markkey[marking_index]->enabling );
}

int tel::enabled( const int& marking_index ) const {
  return( markkey[marking_index]->enabled );
}

bool tel::is_behavior_rule( const int& i, const int& j ) const {
  return( rules[i][j]->ruletype && !(rules[i][j]->ruletype & ORDERING) );
}

bool tel::is_constraint_rule( const int& i, const int& j ) const {
  return( rules[i][j]->ruletype && rules[i][j]->ruletype & ORDERING );
}

//--------------------------------------------------bool is_disabling_rule
bool tel::is_disabling_rule( const int& i, const int& j ) const {
  return( rules[i][j]->ruletype && rules[i][j]->ruletype & DISABLING );
}
//------------------------------------------------------------------------

int tel::marking_index( const int& i, const int& j ) const {
  return( rules[i][j]->marking );
}

void tel::common_enabling( const int& a, const int& b,
			   insert_iterator<event_set> Ecom ) const {
  for ( int i = 1 ; i < nevents ; ++i ) {
    if ( is_behavior_rule( i, a ) && is_behavior_rule( i, b ) ) {
      *Ecom++ = i;
    }
  }
}

int tel::common_enabling( const int& a, const int& b ) const {
  for ( int i = 1 ; i < nevents ; ++i ) {
    if ( is_behavior_rule( i, a ) && is_behavior_rule( i, b ) ) {
      return( i );
    }
  }
  return( _NOT_FOUND_ );
}

int tel::upper_bound( const int& e, const int& f ) const {
  // Add skunky rule check here using a try and catch statment.
  if (( rules[e][f]->ruletype != NORULE ) && 
      ( rules[e][f]->ruletype < ORDERING )) return( rules[e][f]->upper );
  return( _M_level_rules[e][f]->upper );
}

int tel::lower_bound ( const int& e, const int& f ) const {
  if (( rules[e][f]->ruletype != NORULE ) &&
      ( rules[e][f]->ruletype < ORDERING )) return( rules[e][f]->lower );
  return( _M_level_rules[e][f]->lower );
}

int tel::get_signal( int i ) const {
  return( events[i]->signal );
}

bool tel::is_dummy_event( const int& event ) const {
  return( events[event]->type & DUMMY );
}

bool tel::is_sequencing_event( const int& event ) const {
  return( events[event]->type & DUMMY );
}

bool tel::has_levels() const {
  return( levels );
}

//------------------------------------------------------------------------
// NOTE: I am assuming that this is a valid rule!  If it is not, then
// the behavior is undefined.
// NOTE: A expression only exists if the expression is not TRUE or FALSE
// (i.e., an expression that is always true is equivalent to no expression
// and an expression that is always false is treated as no expression too).
//------------------------------------------------------------------------
bool tel::has_level_expression( const int& e, const int& f ) const {
  level_exp exp = rules[e][f]->level;
  // A NULL expression represents an expression that is always FALSE.  
  // This is equivalent to not having an expression at all, only it 
  // prevents the rule from ever being enabled.
  // This function is only called by:
  //       a) tell_wrapper to create level rules
  //       b) bap.cc to check for TDC resolution
  //       c) level_expand.cc to see if any level rules need
  //          to be added to the enabled set.
  // These functions are not confused by returning false on a NULL 
  // expression.  Further more, they will never be called for a rule
  // with a NULL expression because that rule will never be enabled.
  if ( exp == NULL ) return( false );
  for ( int i = 0 ; i < get_nsigs() ; ++i ) {
    if ( (exp->product)[i] != 'X' && (exp->product)[i] != '-' )
      return( true );
  }
  return( false );
}
//------------------------------------------------------------------------

//------------------------------------------------level_exp get_expression
level_exp tel::get_expression( int i, int j ) const {
  assert( is_behavior_rule( i, j ) );
  return( rules[i][j]->level );
}
//------------------------------------------------------------------------

//---------------------------------------------------------tel::is_visible
// visibility is currently defined in terms of sequencing events. 
// Anything that is NOT a sequencing event is visible to the 
// specification.
//------------------------------------------------------------------------
bool tel::is_visible( const int& e ) const {
  return( !is_sequencing_event( e ) );
}


//------------------------------------------------------------------------

//---------------------------------------------------tel::get_conflict_set
// returns the set of events that conflict with e
//------------------------------------------------------------------------
const event_set& tel::get_conflict_set( const int& e ) const {
  return( conflict_cache[e] );
}
//------------------------------------------------------------------------

//---------------------------------------------------tel::get_preset_set
// returns the set of events that are in preset of e
//------------------------------------------------------------------------
const preset_list& tel::get_preset_list( const int& e ) const {
  return( preset_cache[e] );
}
//------------------------------------------------------------------------

//-----------------------------------------------unsigned level_rule_index
unsigned tel::level_rule_index( const event_type& e, 
				const event_type& f ) const {
  assert( is_level_rule( e, f ) );
  return( _M_level_rules[e][f]->marking );
}
//------------------------------------------------------------------------

//------------------------------------------------------bool is_level_rule
bool tel::is_level_rule( const event_type& e,
			 const event_type& f ) const {
  return( _M_level_rules != NULL                   &&
	  _M_level_rules[e][f]->ruletype != NORULE    );
}
//------------------------------------------------------------------------

//------------------------------------------------------bool is_level_rule
bool tel::is_level_rule( const int& i ) const {
  return( is_level_rule( enabling( i ), enabled( i ) ) );
}
//------------------------------------------------------------------------


//--------------------------------------------bool is_disjunctive_conflict
bool tel::is_disjunctive_conflict( const event_type& e       ,
			     const event_type& f       ,
			     const event_type& enabled   ) {
  // NOTE: If there is not any level rules that exists connecting
  // e and f to a common ancestor enabled, then there can be no
  // level conflict between e and f.
  if ( !is_level_rule( e, enabled ) || !is_level_rule( f, enabled ) )
    return( false );
  typedef list<pair<event_type,event_type> > list_type;
  const list_type& cl( _M_disjunctive_conflict[enabled] );
  return ( find(cl.begin(), cl.end(), make_pair( e, f )) != cl.end() ||
           find(cl.begin(), cl.end(), make_pair( f, e )) != cl.end()    );
}
//------------------------------------------------------------------------

//----------------------------------------------------bool is_rising_event
bool tel::is_rising_event( const event_type& e ) const {
  return( e % 2 == 1 );
}
//------------------------------------------------------------------------

//---------------------------------------------------bool is_falling_event
bool tel::is_falling_event( const event_type& e ) const {
  return( e % 2 == 0 );
}
//------------------------------------------------------------------------

//-----------------------------------------------------bool is_same_action
bool tel::is_same_action( const event_type& a , 
			  const event_type& b   ) const {
  return( get_signal( a ) == get_signal( b ) && 
	  (a % 2) == (b % 2)                    );
}
//------------------------------------------------------------------------

//-------------------------------------------------------bool is_unbounded
bool tel::is_unbounded( const event_type& e, const event_type& f ) const {
  return ( upper_bound( e, f ) == INFIN );
}
//------------------------------------------------------------------------

//-------------------------------------------------bool is_marked_disabled
bool tel::is_marked_disabled( const event_type& e ) const {
  return( events[e]->color );
}
//------------------------------------------------------------------------

//------------------------------------------------------bool mark_disabled
void tel::mark_disabled( const event_type& e ) {
  events[e]->color = true;
}
//------------------------------------------------------------------------

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
bool report_incomplete_tel( char* filename ) {
  tel_error t( filename );
  t.print_message( lg );
  return( false );
}
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
ostream& dump( ostream& s, const tel& t, const char* Rf  ) {
  s << "{";
  bool comma = false;
  for ( int i = 0 ; i < t.marking_size() ; ++i ) {
    if ( Rf[i] != 'F' ) {
      if ( comma ) {
	s << ",";
      }
      s << "(" << t.get_event(t.enabling( i ))
	<< "," << t.get_event(t.enabled( i ))
	<< ")";
      comma = true;
    }
  }
  s << "}";
  return( s );
}
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------ostream& dump
// dumps a human readable form of an event_set E
//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const event_set& E ) {
  typedef event_set::const_iterator const_iter;
  s << "{";
  for ( const_iter i = E.begin() ; i != E.end() ; ) {
    s << t.get_event( *i );
    if ( ++i != E.end() ) {
      s << ",";
    }
  }
  s << "}";
  return( s );
}
//------------------------------------------------------------------------

//-----------------------------------------------------------ostream& dump
// dumps a human readable form of an _event_list E
//------------------------------------------------------------------------
ostream& dump( ostream& s, const tel& t, const _event_list& E ) {
  typedef _event_list::const_iterator const_iter;
  s << "{";
  for ( const_iter i = E.begin() ; i != E.end() ; ) {
    s << t.get_event( *i );
    if ( ++i != E.end() ) {
      s << ",";
    }
  }
  s << "}";
  return( s );
}
//------------------------------------------------------------------------

//-----------------------------------------------------------ostream& dump
ostream& dump( ostream& s, const tel& t, const preset_list& p ) {
  s << "{";
  preset_list::const_iterator i( p.begin() ); 
  while ( i != p.end() ) {
    dump( s, t, **i );
    if ( ++i == p.end() ) break;
    s << ",";
  }
  return( s << "}" );
}
//------------------------------------------------------------------------
