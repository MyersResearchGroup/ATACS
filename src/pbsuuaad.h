///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 beta
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
#ifndef _INTERNAL_PBSUUAAD_H
#define _INTERNAL_PBSUUAAD_H

#include <vector>

#include "graph_tracker.h"
#include "markov_types.h"
#include "CPdf.h"
#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
//

class cycle_tracker : public graph_tracker {
protected:

  typedef CPdf::real_type real_type;
  typedef real_type**** delay_matrix;
  typedef vector<int> int_array;

  ruleADT** rules;
  cycleADT**** cycles;
  int nevents;
  int ncycles;
  int event_e;
  int index_e;
  int event_f;
  int index_f;

  int_array* cycle_index;
  delay_matrix delay_values;

  // The index on seperation_values implies a specific sequence of 
  // events in time.  The index is [A event][B event][A index][B index],
  // as such, if index A is less than index B, and Event A preceded event
  // B, then your return value is negative.  It is positive in the dual.
  delay_matrix separation_values;

  static const int _NOT_USED_;
  static const real_type _NAN_;
  static const real_type _INF_;

  double precision;
  prob_vector* m_vals;
  bool dirty;

protected:

  real_type maxdiffi( ruleADT **rules,
		      cycleADT ****cycles,
		      int nevents,
		      int ncycles,
		      int e, int i,
		      int f, int j );

  void mem_aloc_values();

  int index( const int& i ) const;
  void index( const int& enabling, const int& enabled );

  virtual prob_vector find_prob() const;

  virtual void update_counts( const double& weight,
			      const int& ps, 
			      const int& ns ); 
public:

  cycle_tracker( ruleADT** r, 
		 cycleADT**** c,
		 int num_events,
		 int num_cycles,
		 int e, int i,
		 int f, int j );
  
  ~cycle_tracker();

  /////////////////////////////////////////////////////////////////////////////
  //
  /////////////////////////////////////////////////////////////////////////////
  bool fire( const double& weight,
	     const int& enabled,
	     const int& enabling = 0 );
  
  /////////////////////////////////////////////////////////////////////////////
  //
  /////////////////////////////////////////////////////////////////////////////
  void store_results( stateADT* s ) const;

  void clear();

  bool done() const;

  real_type cycle_period();

  bool backtrace( prob_vector* cpv );

  /////////////////////////////////////////////////////////////////////////////
  // This function returns the current vector of observations seen thus far.
  // If start_index is set to 5, then the value in 0 location is the amount of
  // time that elapsed between the 5th and 6th firing of the event as specified
  // by the event_index in the constructor.  The vector is the same size
  // as ncycles (i.e., 1 entry for each observation used in the calculation of
  // the average cycle-time (cycle_metric).
  /////////////////////////////////////////////////////////////////////////////
  const prob_vector* vals();
  
};

//
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
class cycle_analysis_control {
public:

  cycle_analysis_control();
  cycle_analysis_control( const cycle_analysis_control& c );
  ~cycle_analysis_control();
  bool operator()( const graph_tracker& g );
  
};
//
///////////////////////////////////////////////////////////////////////////////

#endif
