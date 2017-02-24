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

#ifndef __INTERNAL_TRIGGER_PROBS_H
#define __INTERNAL_TRIGGER_PROBS_H

#include <vector>
#include <list>
#include <utility>

#include "struct.h"
#include "markov_types.h"

typedef pair<unsigned int,double> trigger_type;
typedef list<trigger_type> trigger_list;
typedef vector<trigger_list> trigger_profile;

///////////////////////////////////////////////////////////////////////////////
// calculate_trigger_profile will return a profile containing the trigger
// event for all events in the system with their occurence probability.  This
// relies on findER to get the excitation region of an event and the function
// calculate_trigger_probs to find the probability of each trigger event for 
// a given event and ER.
///////////////////////////////////////////////////////////////////////////////
trigger_profile calculate_trigger_profile( const designADT design );

///////////////////////////////////////////////////////////////////////////////
// print_trigger_profile outputs a trigger_profile to the ostream s in a 
// lexical representation.  The return value is the ostream s after the 
// trigger_profile has been streamed into it.
///////////////////////////////////////////////////////////////////////////////
ostream& print_trigger_profile( ostream& s, 
				const trigger_profile& t,
				eventADT events[] );


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool dump_trigger_probs( designADT design );

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool get_trigger_probs( designADT design,
			back_insert_iterator<trigger_profile> i_out);

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
bool save_trigger_probs( designADT design,
			 const trigger_profile& tp );

#endif // __INTERNAL_TRIGGER_PROBS_H
