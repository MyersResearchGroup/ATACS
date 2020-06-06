///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1998 by Eric G. Mercer
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

#include <algorithm>

#include "cpu_time_tracker.h"
#include "trigger_probs.h"
#include "stateasgn.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////
// calculate_trigger_probs( ... ) will take state_list ER which contains the
// excitation region for a given event, and output to i_prob the probability
// of each of the trigger signals comming into the excitation region ER.
///////////////////////////////////////////////////////////////////////////////
template <class OutputIterator>
void calculate_trigger_probs( const state_list& ER,
			      OutputIterator i_prob,
			      const unsigned int& nevents ){
  typedef state_list::const_iterator const_iterator;
  prob_vector prob( nevents, (prob_type)0 );
  prob_type total = 0;
  // Loop through each state in the ER
  for ( const_iterator i = ER.begin() ; i != ER.end() ; i++ ) {
    // Loop through each state feeding into the state in the ER
    for ( statelistADT ns = (*i)->pred ; ns != NULL ; ns = ns->next ) {
      // If a state is not in the ER, then adds its probability into the mix
      if ( find( ER.begin(), ER.end(), ns->stateptr ) == ER.end() ) {
	// A states probability might not be all in the same ER
	prob_type tmp = ns->probability * ns->stateptr->probability;
	prob[ns->enabled] += tmp;
	total += tmp;
      }
    }
  }
  // Loop through each event and if an event has probability, assign it to
  // i_prob
  for ( unsigned int i = 0 ; i < nevents ; i++ )
    if ( prob[i] ) 
      *i_prob++ = trigger_type( i, (prob_type)prob[i]/total );
}

///////////////////////////////////////////////////////////////////////////////
// calculate_trigger_profile will return a profile containing the trigger
// event for all events in the system with their occurence probability.  This
// relies on findER to get the excitation region of an event and the function
// calculate_trigger_probs to find the probability of each trigger event for 
// a given event and ER.
///////////////////////////////////////////////////////////////////////////////
trigger_profile calculate_trigger_profile( const designADT design ) {
  trigger_profile retVal( design->nevents, trigger_list() );
  trigger_profile::iterator j = retVal.begin();
  for ( unsigned int i = 0 ; i < (unsigned int)design->nevents ; i++, j++ ) {
    state_list ER;
    back_insert_iterator<state_list> i_ER( ER );
    if ( findER( design->state_table, 
		 design->events, 
		 design->nevents, 
		 i, 
		 i_ER ) ) {
      back_insert_iterator<trigger_list> trig_in( (*j) );
      calculate_trigger_probs( ER, trig_in, design->nevents );
    } 
  }
  return( retVal );
}

///////////////////////////////////////////////////////////////////////////////
// print_trigger_profile outputs a trigger_profile to the ostream s in a 
// lexical representation.  The return value is the ostream s after the 
// trigger_profile has been streamed into it.
///////////////////////////////////////////////////////////////////////////////
ostream& print_trigger_profile( ostream& s, 
				const trigger_profile& t,
				eventADT events[] ) {
  typedef trigger_profile::const_iterator trig_iter;
  typedef trigger_list::const_iterator trig_list_iter;
  unsigned int j = 0;
  for ( trig_iter i = t.begin() ; i != t.end() ; i++, j++ ) {
    s << endl << events[j]->event << ":";
    if ( (*i).empty() )
      s << endl << "\t<none>";
    else 
      for ( trig_list_iter k = (*i).begin() ; k != (*i).end() ; k++ )
	s << endl << "\t" << events[(*k).first]->event << " -> "
	  << (*k).second;
  }
  return( s );
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool dump_trigger_probs( designADT design ) {
  // string fname( string( design->filename ) + ".tp" );
  // ofstream out_stream;
  
  // out_stream.open( fname.c_str(), ios::out );
  // if ( !out_stream.is_open() ) {
  //   cout << "Cannot open '" << fname << "', for writing" << endl;
  //   return( false );
  // }

  // cout << "Saving transition probabilities to `" << fname << "' ... ";
  // cpu_time_tracker time;

  // out_stream << "   Trigger Probabilities" << endl
  //            << "----------------------------";

  // for ( int j = 0 ; j < design->nevents ; j++ ) {
  //   out_stream << endl;
  //   for ( int i = 0 ; i < design->nevents ; i++ ) {
  //     if ( design->rules[i][j]->ruletype != NORULE &&
  //          design->rules[i][j]->ruletype != REDUNDANT ) {
  //       out_stream << "<" << design->events[i]->event << ","
  //       	   << design->events[j]->event << ">: "
  //       	   << (*(design->trigger_probs))[design->rules[i][j]->marking] 
  //       	   << endl;
  //     }
  //   }
  // }
  // time.mark();
  // cout << "done: " << time << endl;
  // out_stream.close();
  return( true );
}
	

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool get_trigger_probs( designADT design,
			back_insert_iterator<trigger_profile> i_out) {
  for ( unsigned int j = 0 ; (int)j < design->nevents ; j++ ) {
    trigger_list tmp;
    for ( unsigned int i = 0 ; (int)i < design->nevents ; i++ ) {
      if ( design->rules[i][j]->ruletype != NORULE &&
	   design->rules[i][j]->ruletype != REDUNDANT ) {
	tmp.push_back( trigger_type( i, 
				     (*(design->trigger_probs))
				     [design->rules[i][j]->marking] ) );
      }
    }
    *i_out++ = tmp;
  } 
  return( true );
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool save_trigger_probs( designADT design,
			 const trigger_profile& tp ) {
  if ( !design->trigger_probs->size() ) {
    fill_n( back_inserter(*(design->trigger_probs)), design->nrules, 0 );
  }
  typedef trigger_profile::const_iterator trig_iter;
  typedef trigger_list::const_iterator trig_list_iter;
  for ( unsigned int j = 0 ; j < tp.size() ; j++ ) {
    for ( trig_list_iter i = (tp[j]).begin() ; i != (tp[j]).end() ; i++ ) {
      (*(design->trigger_probs))[(design->rules[(*i).first][j])->marking] =
	(*i).second;
    }
  }
  return(true);
}
