///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1999 by Eric G. Mercer
//
// @author Eric G. Mercer
//                    
// Permission to use, copy, modify and/or distribute, but not sell, this  
// software and its documentation for any purpose is hereby granted       
// without fee, subject to the following terms and conditions:            
//                                                                          
// 1. The above copyright notice and this permission notice must         
// appear in all copies of the software and related documentation.        
//                                                                          
// 2. The name of University of Utah may not be used in advertising or  
// publicity pertaining to distribution of the software without the       
// specific, prior written permission of Univsersity of Utah.
//                                                                          
// 3. This software may not be called "Taco" if it has been modified    
// in any way, without the specific prior written permission of           
// Eric G. Mercer                                
//                                                                          
// 4. THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY       
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.       
//                                                                          
// IN NO EVENT SHALL THE UNIVERSITY OF UTAH OR THE AUTHORS OF THIS 
// SOFTWARE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL 
// DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE  
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <algorithm>
#include <limits>

#undef MAXINT
#ifndef OSX
#ifndef NT
#include <values.h>
#endif
#endif

#include "pbsuuaad.h"
#include "connect.h"
#include "stats.h"

//#ifdef NT
//#define MAXDOUBLE 1.79769e+308
//#endif

const int cycle_tracker::_NOT_USED_ = -1;
const cycle_tracker::real_type cycle_tracker::_INF_ = numeric_limits<double>::max();
const cycle_tracker::real_type cycle_tracker::_NAN_ = numeric_limits<double>::max() - 1e300;

cycle_tracker::real_type cycle_tracker::maxdiffi( ruleADT **rules,
						  cycleADT ****cycles,
						  int nevents,
						  int ncycles,
						  int e, int i,
						  int f, int j ) {
  // Bail out condition if cycle is detected.  MAXDOUBLE-1 means it
  // is currently being explored.
  if ( separation_values[e][f][i][j] == ( _NAN_ - 1e300 ) ) {
    printf( "ERROR:  Cycle detected in acyclic constraint graph.\n" );
    fprintf( lg, "ERROR:  Cycle detected in acyclic constraint graph.\n" );
    printf( "e = %d  f = %d\n",e,f );
    return numeric_limits<double>::max();
  }

  // The maximum diff between identical events in the same cycle is 
  // always 0
  if ( ( e == f ) && ( i == j ) ) return( 0 );

  // Have already found the maximum time diff between these two events,
  // return the found value
  if ( separation_values[e][f][i][j] != _NAN_ ) 
    return( separation_values[e][f][i][j] );

  // Mark this location as currently being considered or work in progress.
  separation_values[e][f][i][j] -= 1e300;

  real_type max = (-_INF_);
  real_type newmax = 0.0;
  int m = ( ( i - 1 ) < 0 ) ? i : i - 1;
  int d = 0, k = 0;

  for ( d = 0 ; d < nevents ; d++ ) {
    for ( k = m ; k <= i ; k++ ) {
      // If an edge exists between d and e (there is an assigned delay),
      // then calculate the seperation value between d and f where f is
      // the terminating event we are actually interested in.  (Move to 
      // the next level of the graph.
      if ( delay_values[d][e][k][i] != _NOT_USED_ ) {
	separation_values[d][f][k][j] = maxdiffi( rules,
						  cycles,
						  nevents,
						  ncycles,
						  d,k,
						  f,j );
	// Set the max seperation correctly meaning INF if the
        // seperation can be unbounded or to the found seperation
        // between d and f plus the delay between d and e.
	// if d==0, then we are at a reset event and the speration
        // is UNBOUNDED!
	if ( separation_values[d][f][k][j] == _NAN_ ) return _NAN_;
	if ( ( separation_values[d][f][k][j] == _INF_ ) ||
	     ( separation_values[d][e][k][i] == _INF_ ) )
	  newmax = _INF_;
	else if ( d != 0 ) 
	  newmax = separation_values[d][f][k][j] + delay_values[d][e][k][i];
	else 
	  newmax = _INF_;
	if ( newmax > max ) 
	  max = newmax; 
      }
    }
  }

  if ( max == ( -_INF_ ) ) 
    max = _INF_;

  // This insures consistency in the seperations.  If there is a path between
  // e(i) and f(j), then it is possible to acheive two different maximum
  // time seperations with two different timing assignments.  This implies
  // that there is no one timing assignment that will give a max for all 
  // seperations. This also handles the fact that if there is no 
  // syncrhonization between e(i) and f(j) from the reset events where there
  // seperation can be unbounded.
  //cout << endl << "reachable(" << e << "," << i << "," << f << "," << j 
  //     << ") -> " << reachable( cycles, nevents, ncycles, e, i, f, j );
  if ( reachable( cycles, nevents, ncycles, e, i, f, j ) == 1 ) {
    m = ( ( j - 1 ) < 0 ) ? j : j - 1;
    for ( d = 0 ; d < nevents ; d++ ) {
      for ( k = m ; k <= j ; k++ ) {
	// If an edge exists ( i.e., a delay value exists ), then
        // move to the next level of the graph and find the speration
        // between that level and the terminating event.
	if ( delay_values[d][f][k][j] != _NOT_USED_ ) {
	  separation_values[e][d][i][k] = maxdiffi( rules,
						    cycles,
						    nevents,
						    ncycles,
						    e, i,
						    d, k );
	  // Set the max correctly, INF if speration is unbounded
          // or the new found seperation between e and d plus the 
	  // assigned delay between d and f.  NOTE: If d is zero,
          // then we are looking at a reset event, only in this direction
          // the delay is bounded, since there is an actual delay
          // from when the reset event fired and e fired.
	  if ( separation_values[e][d][i][k] == _NAN_ ) return _NAN_;
	  if ( separation_values[e][d][i][k] == _INF_ )
	    newmax = _INF_;
	  else if ( d != 0 ) 
	    newmax = separation_values[e][d][i][k] - delay_values[d][f][k][j];
	  else 
	    newmax = separation_values[e][d][i][k];
	  if (newmax < max) 
	    max = newmax; 
	}
      }
    }
  }
  
  if ( reachable( cycles, nevents, ncycles, e, i, f, j ) == INFIN ) 
    return _NAN_;
  
  return(max);

}


void cycle_tracker::mem_aloc_values() {
  delay_values = new real_type***[nevents];
  separation_values = new real_type***[nevents];
  for ( int i = 0 ; i < nevents ; i++ ) {
    delay_values[i] = new real_type**[nevents];
    separation_values[i] = new real_type**[nevents];
  }
  for ( int i = 0 ; i < nevents ; i++ ) {
    for ( int j = 0 ; j < nevents ; j++ ) {
      delay_values[i][j] = new real_type*[ncycles];
      separation_values[i][j] = new real_type*[ncycles];      
    }
  }
  for ( int i = 0 ; i < nevents ; i++ ) {
    for ( int j = 0 ; j < nevents ; j++ ) {
      for ( int k = 0 ; k < ncycles ; k++ ) {
	delay_values[i][j][k] = new real_type[ncycles];
	separation_values[i][j][k] = new real_type[ncycles];      
      }
    }
  }
}

int cycle_tracker::index( const int& i ) const {
  return( (*cycle_index)[i] );
}
					
void cycle_tracker::index( const int& enabling, const int& enabled ) {
  assert( rules[enabling][enabled]->ruletype != NORULE );
  // Update the index for the enabled event according to the index
  // of its enabling event and the epsilon value on the rule
  (*cycle_index)[enabled] = index(enabling) + 
    rules[enabling][enabled]->epsilon;
}

prob_vector cycle_tracker::find_prob() const {
  // Return 1 in the prob_vector when done() is true
  if ( done() )
    return( prob_vector( 1, 1.0 ) );
  return( prob_vector( 1, 0.0 ) );
}

void cycle_tracker::update_counts( const double& weight,
				   const int& ps, 
				   const int& ns ) {
  // This is required by the interface but not used in 
  // this implementation.  No values are actually tracked
  // and recorded, only delays are assigned into the
  // rules matrix.
}


cycle_tracker::cycle_tracker( ruleADT** r, 
			      cycleADT**** c,
			      int num_events,
			      int num_cycles,
			      int e,
			      int i,
			      int f,
			      int j )
  : graph_tracker( NULL, graph_tracker::both ), 
    rules( r ), 
    cycles( c ),
    nevents( num_events ),
    ncycles( num_cycles ),
    event_e( e ),
    index_e( i-1 ), // Cycles start at 0, so this is adjusted accordingly
    event_f( f ),   
    index_f( j-1 ), // Cycles start at 0, so this is adjusted accordingly
    precision( 0.0 ),
    m_vals( NULL ),
    dirty( true ) {
  // Make sure someone did not call this with dumb values (should never
  // happen
  assert( r != NULL && c != NULL && ncycles >= j && ncycles >= i );
  mem_aloc_values();
  // Cycles are going to start from 0, for an events first cycle is
  // its 0th cycle.  This means that the above event indexs are
  // adjusted to be 0 based indexed.
  cycle_index = new int_array( (int_array::size_type)nevents, -1 );
  double tmp;
  // This calculates the precision of the machine (i.e., the minimum
  // step between exactly represented values
  machar( precision, tmp );
  precision = ( tmp > precision ) ? tmp : precision;
  // m_vals is used as an interface function.  The array stores
  // the value of each observed cycle using the maxdiffi function
  m_vals = new prob_vector( num_cycles, 0.0 );
  clear();
}
  
cycle_tracker::~cycle_tracker() {
  for ( int i = 0 ; i < nevents ; i++ ) {
    for ( int j = 0 ; j < nevents ; j++ ) {
      for ( int k = 0 ; k < nevents ; k++ ) {
	delete [] delay_values[i][j][k];
	delete [] separation_values[i][j][k];
      }
    }
  }
  for ( int i = 0 ; i < nevents ; i++ ) {
    for ( int j = 0 ; j < nevents ; j++ ) {
      delete [] delay_values[i][j];
      delete [] separation_values[i][j];
    }
  }
  for ( int i = 0 ; i < nevents ; i++ ) {
    delete [] delay_values[i];
    delete [] separation_values[i];
  }
  delete [] delay_values;
  delete [] separation_values;
  delete cycle_index;
  delete m_vals;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool cycle_tracker::fire( const double& weight,
			  const int& enabled,
			  const int& enabling ) {
  if ( weight != _NOT_USED_ ) {
    if ( ( index( enabling ) != _NOT_USED_ && index( enabling ) < ncycles ) &&
	 ( index( enabled ) < ncycles  ) ) {
      delay_values[enabling][enabled][index(enabling)]
	[index(enabling)+rules[enabling][enabled]->epsilon] =
	rules[enabling][enabled]->dist->last_value();
    }
    return( true );
  }
  index( enabling, enabled );
  // Clear the dirty bit if we are done because it is now OK to query
  // maxdiffi
  if ( done() )
    dirty = false;
  return( true );
}
  
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void cycle_tracker::store_results( stateADT* s ) const {
  // Currently, this object does not store any results.  However,
  // the graph_tracker interface requires this function.
}

void cycle_tracker::clear() {
  // The dirty bit indicates that the values in 
  dirty = true;
  for ( int i = 0 ; i < nevents ; i++ ) {
    for ( int j = 0 ; j < nevents ; j++ ) {
      for ( int x = 0 ; x < ncycles ; x++ ) {
	for ( int y = 0 ; y < ncycles ; y++ ) {
	  cycles[i][j][x][y]->delay = _NOT_USED_;
	  delay_values[i][j][x][y] = _NOT_USED_;
	  separation_values[i][j][x][y] = _NAN_;
	}
      }
    }
  }
  for ( int i = 0 ; i < nevents ; i++ ) {
    if ( rules[0][i]->ruletype != NORULE ) {
      delay_values[0][i][0][0] = 0;
    }
  }
  ::fill( (*cycle_index).begin(), (*cycle_index).end(), -1 );
}

bool cycle_tracker::done() const {
  // If the index of events e and f have acheived their correct
  // index value ( remember that index_e and index_f have already
  // been adjusted for the 0 based indexing ), then return true.
  // Otherwise return false.
  return( index( event_e ) >= index_e && 
	  index( event_f ) >= index_f );
}

cycle_tracker::real_type cycle_tracker::cycle_period() {
  // If the dirty bit is set, the delay matrix has NOT been filled by
  // the simulation!  Querying maxdiffi will not be good so bail
  // with a zero return value!
  if ( dirty ) return( 0.0 );
  separation_values[event_f][event_e][index_f][index_e] = maxdiffi( rules, 
								    cycles, 
								    nevents, 
								    ncycles,
								    event_f, 
								    index_f,
								    event_e, 
								    index_e ); 
  return( separation_values[event_f][event_e][index_f][index_e] / 
	  (double)(index_f - index_e) );
}

bool cycle_tracker::backtrace( prob_vector* cpv ) {
  // If the dirty bit is set or cpv is NULL, the delay matrix has NOT been 
  // filled by the simulation or some knuckly head sent us a skunky pointer.
  if ( cpv == NULL || dirty ) {
    cout << "ERROR: NULL pointer in backtrace or simulation not run." 
	 << endl << flush;
    return( false );
  }
  double update_value = 1.0 / (double)(index_f - index_e);

  // Okay, this is a major point of confusion, but maxdiff is called
  // with the latest arriving event as 'e' and the earliest arriving event as
  // 'f' in this application.  This change in variables here is to 
  // accommodate the fact that we derived the algorithm under the opposite
  // notation (e->early comes before f, f->late).  In reality, both ways
  // work, only I need to first query maxdiffi to fill in the seperation
  // matrix with the correct time seperation values.  Since I have not done
  // this yet, I will stick with this assumption.
  int e = event_f;
  int i = index_f;
  int f = event_e;
  int j = index_e;
  int cpv_size = (int)(cpv->size());

  // Make sure that the separation between the two events of interest
  // has been found by checking the value against _NAN_, the cleared
  // value.  If it is not set, make the call to maxdiffi.
  if ( separation_values[e][f][i][j] == _NAN_ ) cycle_period();

  // If there is an infinite or unbounded seperation, then no backtrace
  // exists and the user should be notified.
  if ( separation_values[e][f][i][j] == _INF_ ) {
    cout << "ERROR: backtrace -> unbounded seperation between (" << e 
         << "," << i << ") and (" << f << "," << j << ")." 
	 << endl;
    return( false );
  }
  // I want to continue back tracing as long as I have not matched 
  // events and cycle indexes.  NOTE: This is going to fail if the event
  // is not on the critical path.  This needs to be changed to stop when
  // the cycle indexs pass each other or when the seperation value is ZERO!
  while( ( e != f || i != j ) & ( separation_values[e][f][i][j] > 0 ) ) {

    int m = ( ( i - 1 ) < 0 ) ? i : i - 1;
    int d = 0, k = 0;
    bool done = false;
    
    for ( d = 0 ; d < nevents ; d++ ) {
      for ( k = m ; k <= i ; k++ ) {
	// If a there is an assigned delay in this location and that
        // delay is on the critical path, then update it value using marking 
        // index in the rulesADT* structure
	if ( delay_values[d][e][k][i] != _NOT_USED_  ) {
	  if ( real_type_compare( separation_values[e][f][i][j],
				  separation_values[d][f][k][j] +
				  delay_values[d][e][k][i], precision ) ) {
	    // NOTE: This will not work directly with TELS due to the fact
            // that this rule MAY NOT exist.  The rule was created by
            // recording the enabling event, this does not imply that a 
	    // rule actually exist.  I will need to have a different data
            // structure to record the update if I am working with TELS
	    if ( rules[d][e]->ruletype != NORULE && 
		 rules[d][e]->marking < cpv_size ) {
	      (*cpv)[rules[d][e]->marking] += update_value;
	      done = true;
	      break;
	    }
	    else {
	      cout << "ERROR: backtrace -> cpv is too short or no rule." 
		   << endl;
	      return( false );
	    }
	  }
	}
      }
      if ( done )
	break;
    }

    // I have identified the delay on the critical path, so move to the next
    // stage of trace.
    e = d;
    i = k;
  
    // If we run off the trace, then something is seriousely wrong as that
    // should have been caught in the else clause where the event_f is not
    // directly on the critical path.
    if ( e == nevents ) {
      cout << "ERROR: backtrace -> ran pass terminator and off of trace." 
	   << endl;
      return( false );
    }

  }
  return( true );
}

const prob_vector* cycle_tracker::vals() {
  if ( dirty ) return m_vals;
  real_type* ptr = &(*m_vals->begin());
  for ( int i = index_e ; i < index_f ; i++ ) {
    separation_values[event_f][event_e][i+1][i] = maxdiffi( rules, 
							    cycles, 
							    nevents, 
							    ncycles,
							    event_f, 
							    i+1,
							    event_e,
							    i );
    *ptr++ = separation_values[event_f][event_e][i+1][i];
  }
  return( m_vals );
}

/////////////////////////////////////////////////////////////////////////////
//
cycle_analysis_control::cycle_analysis_control() {
}

cycle_analysis_control::cycle_analysis_control(const cycle_analysis_control& c)
{
}

cycle_analysis_control::~cycle_analysis_control() {
}

bool cycle_analysis_control::operator()( const graph_tracker& g ){
  if ( (g.probability())[0] == 1.0 )
    return( true );
  return( false );
}
//
/////////////////////////////////////////////////////////////////////////////
