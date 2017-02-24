//////////////////////////////////////////////////////////////////////////
// @name Partial Order Reduction
// @version 0.1 alpha
//
// (c)opyright 2001 by Eric G. Mercer
//
// @author Eric G. Mercer
//////////////////////////////////////////////////////////////////////////
#ifndef __PO_REDUCE_H__
#define __PO_REDUCE_H__

#include "telwrapper.h"
#include "bap.h"
#include "level_expand.h"

// Setting this to true will enable the use of the necessary set to
// aggressively prune the POSETs
void set_prune( bool value );
bool is_prune_set();

// These will initialize and destroy the cache used in the necessary set
// calculation.  It does not need to be done if pruning is disable and
// no partial order reduction is being performed.
void init_cache( unsigned cache_size );
void reinit_cache( unsigned cache_size );
void delete_cache( unsigned cache_size );

event_set* get_ample_set( const tel& t          , 
			  const timed_state& sc ,
			  const _event_list& Ef ,
			  const event_set& Een    );

event_set* get_causal_events( const tel& t                     , 
			      const timed_state& sc            ,
			      const event_set& Een             ,
			      enabled_set_expander& Een_expand   );

bool is_causal_candidate( const tel& t                  ,
			  const rule_set& Rb            ,
			  const POSET& Me               , 
			  rule_set::const_iterator rule   );

void combine_sets( event_set* ptr_source , 
		   event_set* ptr_dest     );

#endif
