//////////////////////////////////////////////////////////////////////////
// @name Bourne Again Geometric timing analysis
// @version 0.1 alpha
//
// (c)opyright 2000 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#ifndef __BAG_H__
#define __BAG_H__

#include "struct.h"
#include "cpu_time_tracker.h"
#include "telwrapper.h"

//------------------------------------------------------------------------
// These functions are exported to be used by bap.

FILE* open_rsg_file( const tel& t, bool verbose );

void check_merge_conflicts( const tel& t,
			    const char* const marking );
// ------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//
class clock_zone {
public:

  clocksADT clocks;         // Difference Bound Matrix (DBM)
  clockkeyADT clockkey;     // Maps clock indexes to <i,j> rule indexes
  clocknumADT clocknums_iv; // Maps marking indexes to clock indexes
  int clock_size;
  int num_clocks;           // Size of the DBM: nclocks x nclocks
  int* nreference;          // Number of references to this clock_zone
                            // (i.e., how many places are referencing this 
                            //  memory).

public:

  clock_zone();
  clock_zone( const clock_zone& z );
  clock_zone( const clock_zone& z, const int& marking_size );
 
  bool is_active( const int& marking_index ) const;
  int max_clock_value( const int& marking_index ) const;
  int num_active_clocks() const;
  void free_memory();

};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
class fired_data {
public:

  typedef char* bit_vector;

protected:

  static unsigned int size; // Denotes the length of the bit_vectors
  bit_vector Rf;            // Rule fired set
  bit_vector confl_removed; // Tracks squashed tokens on choice places
  bit_vector fired;         // Helps determine if an event has fired
                            // (i.e., when event fires, mark all of its 
                            // rules T).
public:

  fired_data();
  fired_data( bit_vector exp_enabled_bv, 
	      bit_vector confl_removed_bv,
	      bit_vector fired_bv );
  fired_data( const fired_data& f );
  bool is_fired( const int& marking_index ) const;

};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
bool bag_rsg( tel& t,
	      stateADT *state_table,
	      timeoptsADT timeopts,		      
	      bddADT bdd_state, 
	      bool use_bdd,
	      bool verbose,bool noparg);
//////////////////////////////////////////////////////////////////////////

#endif
