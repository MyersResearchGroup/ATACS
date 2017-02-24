// ***************************************************************************
// Functions for using timing to help detect hazards.
// ***************************************************************************

#include "decomp.h"

// ********************************************************************
// Tries to stabilize an edge based on timing analysis. Entered once
// per node.
// ********************************************************************
void stabilize_edge_timed(designADT design, sg_node*& temp_sg_node,
			  node* head, int num_states,
			  node* input_vec[], bool general, int& depth_tot,
			  int& num_paths, int& num_sucs,
			  int& num_states_vis) {

  int depth_curr=1;
  int index=0;
  int transition=0;
  stateADT cur_state;
  statelistADT edge;
  clocklistADT clocklist;
  zone_curt* work_zone;
#ifdef __VERBOSE_TIMED__
  int num_init_zones=0;
#endif

#ifdef __VERBOSE_DEPTH__
  cout << "Starting node " << head->label << endl;
  cout << "------------------------" << endl << endl;
  cout << "Depth tot=" << depth_tot
       << "  Curr depth=" << depth_curr
       << "  Num sucs=" << num_sucs
       << "  States vis=" << num_states_vis << endl;
#endif

  // visited array keeps track of whenever a state is visited for an entire
  // node for a RISING region. Cleared once per node.
  bool va_nodeR[num_states];

  // visited array keeps track of whenever a state is visited for an entire
  // node for a FALLING region. Cleared once per node.
  bool va_nodeF[num_states];

  // visited array keeps track of visitations during propagation of
  // rf/fr edges
  bool va_path[num_states];

  // Clear the visited arrays
  clear_va(va_nodeR, num_states);
  clear_va(va_nodeF, num_states);  clear_va(va_path, num_states);


  // Head of a linked list for storing R->F and F->R transitions used
  // for timed hazard checking
  timed_node* head_timed_node;
  head_timed_node = make_timed_node();

  // Create a temporary node for use as a pointer in the timed state
  // linked list which contains a list of F->R and R->F state
  // transitions that must be checked for timing stabilization.
  head_timed_node->succ = make_timed_node();
  head_timed_node->succ->pred = head_timed_node;
  head_timed_node->succ->counter = (head_timed_node->counter)+1;
  timed_node* temp_timed_node = head_timed_node->succ;
    
#ifdef __VERBOSE_TIMED__
  cout << "Starting the timed edge stabilization for " << head->label 
       << endl;
  cout << "Absolute min, max times = " << head->min
       << ", " << head->max << endl;
#endif
  
  // Create linked list of R->F and F->R transitions
  find_fr_rf(design, head, temp_timed_node);
  temp_timed_node = head_timed_node->succ;

  // Go through this whole list, one R->F or F->R at a time
  while (temp_timed_node->succ != NULL) {
    cur_state = temp_timed_node->cur_state;

    // Do all zones from this starting state
    for (clocklist = temp_timed_node->cur_state->clocklist;
	 clocklist;clocklist=clocklist->next) {
      transition = temp_timed_node->edge->enabled;
      edge = temp_timed_node->edge;

    // Print the zones from the state preceding the reference transition
#ifdef __VERBOSE_TIMED__
      cout << "Printing zones for state " << cur_state->state
	   << " - Edge color = " << cur_state->colorvec[head->counter]
	   << endl;
      num_init_zones = print_state_zones(design, clocklist, true);
      num_init_zones = print_state_zones(design, clocklist, false);
      cout << "Number of zones at start of timing analysis = "
	   << num_init_zones << " for state "
	   << cur_state->state << endl;
#endif

      // Make the first zone from the state preceding the transition
      // of interest. Increase the dimension by 1.
      work_zone = make_new_zone((clocklist->clocknum)+1);
      index = clocklist->clocknum;
      
      // Put the color of the enabling transition into the zone structure
      work_zone->color = cur_state->colorvec[head->counter];

      // Copy the zone information from the state into the newly
      // created zone and add the new transition
      copy_first_zone(work_zone, clocklist, transition, index);
#ifdef __VERBOSE_ZONE__
      cout << "Expanding the working zone ..." << endl;
      print_zone(design, work_zone);
#endif

      // Fill entries with timing data from the rules matrix
      fill_zone_time_data(design, work_zone, input_vec, general,
			  transition);
#ifdef __VERBOSE_ZONE__
      cout << "After adding the lower and upper bounds ..." << endl;
      print_zone(design, work_zone);
#endif

      // Canonicalize the new zone if the dimension is greater than 2
      if (work_zone->dim > 2) {
	canon_zone(work_zone);
#ifdef __VERBOSE_ZONE__
	cout << "After transitive closure ..." << endl;
	print_zone(design, work_zone);
#endif
      }

#ifdef __VERBOSE_TIMED__
      cout << "Beginning " << edge->stateptr->state << endl;
#endif
#ifdef __VERBOSE_DEPTH__
      cout << "Start of recursion for this state pair ...." << endl;
      cout << "Depth total = " << depth_tot << endl;
#endif
      depth_curr = 1;

      // Start the recursion
      do_time(design, work_zone, head->max, edge->stateptr, temp_sg_node,
	      head, index, va_path, va_nodeR, va_nodeF, num_states,
	      input_vec, general, depth_tot, depth_curr, num_paths,
	      num_sucs, num_states_vis);
      free_new_zone(work_zone,(clocklist->clocknum)+1);
    }
    temp_timed_node = temp_timed_node->succ;
  }
  free_timed_node(head_timed_node);
}

// ********************************************************************
// Clear the visited array.
// ********************************************************************
void clear_va(bool va[], int num_states) {

  for (int i=0;i<num_states;i++)
    va[i] = false;
}

// ********************************************************************
// Recursively go through the state graph until a termination
// condition is reached or until the accumulated time is greater
// then the delay through the decomposition.
// ********************************************************************
void do_time(designADT design, zone_curt* work_zone, int decomp_delay,
	     stateADT cur_state, sg_node*& temp_sg_node, node* head,
	     int t_index, bool va_path[], bool va_nodeR[], bool va_nodeF[],
	     int num_states, node* input_vec[], bool general, int& depth_tot,
	     int depth_curr, int& num_paths, int& num_sucs,
	     int& num_states_vis) {

  int timed_data=0;
  int index = head->counter;
  zone_curt* new_zone;

  num_states_vis++;

  // Mark next state visited
  if (cur_state->colorvec[index]=='R')
    va_nodeR[cur_state->number] = true;
  else
    va_nodeF[cur_state->number] = true;
  va_path[cur_state->number] = true;

  // Find the number of edges (successors)
  for (statelistADT cur_state2=cur_state->succ;
       (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
       (cur_state2=cur_state2->next)) {
    num_sucs++;
  }

  // Go through all edges
  for (statelistADT cur_state2=cur_state->succ;
       (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
       (cur_state2=cur_state2->next)) {
#ifdef __VERBOSE_TIMED__
    cout << "Checking edge before state " << cur_state2->stateptr->state << endl;
#endif
    // Let's grow the zone
    new_zone = make_new_zone((work_zone->dim)+1);
    copy_zone_info(new_zone, work_zone, cur_state2->enabled, t_index);
#ifdef __VERBOSE_ZONE__
    cout << "Expanding the working zone ..." << endl;
    print_zone(design, new_zone);
#endif
    
    // Fill two entries with timing data from the rules matrix
    fill_zone_time_data(design, new_zone, input_vec, general,
			cur_state2->enabled);
#ifdef __VERBOSE_ZONE__
    cout << "After adding the lower and upper bounds ..." << endl;
    print_zone(design, new_zone);
#endif
    
    // Canonicalize the new zone
    canon_zone(new_zone);
#ifdef __VERBOSE_ZONE__
    cout << "After transitive closure ..." << endl;
    print_zone(design, new_zone);
#endif
    
    // Get new timing information from the zone
    timed_data = -(new_zone->clocks[t_index][(new_zone->dim)-1]);
#ifdef __VERBOSE_TIMED__
    cout << "Accumulated timing data = " << timed_data << endl;
#endif
    if (timed_data > decomp_delay) {
      depth_tot = depth_tot + depth_curr;
      num_paths++;

#ifdef __VERBOSE_DEPTH__
      cout << "Termination point reached for edge pair:"
	   << endl;
      cout << "Depth tot=" << depth_tot 
	   << "  Curr depth=" << depth_curr
	   << "  Num paths=" << num_paths
	   << "  Num sucs=" << num_sucs
	   << "  States vis=" << num_states_vis << endl;
#endif
      if (((cur_state->colorvec[index]=='R') && 
	   (!va_nodeR[cur_state2->stateptr->number])) ||
	  ((cur_state->colorvec[index]=='F') && 
	   (!va_nodeF[cur_state2->stateptr->number]))) {
	if ((cur_state2->colorvec[index] == 'R')||
	    (cur_state2->colorvec[index] == 'F')) {
	// We have stabilized so add the current edge and the state
	// it is pointing to to the list of stabilized edges
	// to propagate later.
#ifdef __VERBOSE_TIMED__
	  cout << "Edge "
	       << design->signals[design->events[cur_state2->enabled]
				  ->signal]->name;
	  print_plus_minus(cur_state2->enabled);
	  cout << " stabilized between states " << cur_state->state
	       << " and " << cur_state2->stateptr->state << " to ";
#endif	  
	  if (cur_state2->colorvec[index] == 'R') {
#ifdef __VERBOSE_TIMED__
	    cout << "1" << endl;
#endif
	    cur_state2->colorvec[index] = '1';
	    color_pred_vec(cur_state2, index, '1');
	  }
	  else {
#ifdef __VERBOSE_TIMED__
	    cout << "0" << endl;
#endif
	    cur_state2->colorvec[index] = '0';
	    color_pred_vec(cur_state2, index, '0');
	  }

	  // Add this edge to the list
	  push_sg(cur_state2->stateptr, cur_state2, temp_sg_node,
		  head->counter);

	} else {
#ifdef __VERBOSE_TIMED__
	  cout << "Edge "
	       << design->signals[design->events[cur_state2->enabled]
				  ->signal]->name;
	  print_plus_minus(cur_state2->enabled);
	  cout << " ALREADY stabilized between states " << cur_state->state
	       << " and " << cur_state2->stateptr->state << " to "
	       << cur_state2->colorvec[index] << endl;
#endif
	}
      } else {
#ifdef __VERBOSE_TIMED__
	cout << "Edge "
	     << design->signals[design->events[cur_state2->enabled]
				->signal]->name;
	print_plus_minus(cur_state2->enabled);
	cout << " stabilized between states " << cur_state->state
	     << " and " << cur_state2->stateptr->state << " to ";
	if ((cur_state2->colorvec[index] == 'R')||
	    (cur_state2->colorvec[index] == '1'))
	  cout << "1" << " visited not stabilizing" << endl;
	else cout << "0" << " visited not stabilizing" << endl;
#endif
      }
    }
    else {

      // Termination conditions
      if (va_path[cur_state2->stateptr->number]) {
	
	// We have completed a loop so quit
#ifdef __VERBOSE_TIMED__
	cout << "Encountered previously visited path ... continuing"
	     << endl;
#endif
	free_new_zone(new_zone,(work_zone->dim)+1);
	depth_tot = depth_tot + depth_curr;
	num_paths++;
	continue;
      }

      else if (cur_state->colorvec[index] !=
	       cur_state2->stateptr->colorvec[index]) {
	
	// The edge color changed from R to F to R, or from F to R to F
#ifdef __VERBOSE_TIMED__
	cout << "Edge coloring reverted back to " << work_zone->color
	     << " before stabilizing ... continuing" << endl;
#endif
	free_new_zone(new_zone,(work_zone->dim)+1);
	depth_tot = depth_tot + depth_curr;
	num_paths++;
	continue;
      }

      else {
	
	if ((cur_state2->colorvec[index] == '1') ||
	    (cur_state2->colorvec[index] == '0')) {
	  
	  // We have encountered a previously stabilized edge so stop
#ifdef __VERBOSE_TIMED__
	  cout << "Encountered previously stabilized edge ... unstabilizing"
	       << endl;
#endif
	  if (cur_state2->colorvec[index]=='1') {
	    cur_state2->colorvec[index]='R';
	    color_pred_vec(cur_state2, index, 'R');
	  } else {
	    cur_state2->colorvec[index]='F';
	    color_pred_vec(cur_state2, index, 'F');
	  }
	  depth_tot = depth_tot + depth_curr;
	  num_paths++;
	  //continue;
	}

	depth_curr++;

	// Recurse to next edge
	do_time(design, new_zone, decomp_delay, cur_state2->stateptr,
		temp_sg_node, head, t_index, va_path, va_nodeR, va_nodeF,
		num_states, input_vec, general, depth_tot, depth_curr,
		num_paths, num_sucs, num_states_vis);
      }
    }
    
    // Free the zone created in the recursion routine
    free_new_zone(new_zone,(work_zone->dim)+1);
  }
  va_path[cur_state->number] = false;
}

// ********************************************************************
// Create linked list of R->F and F->R transitions between colors
// in two consecutive states. Entered once per node.
// ********************************************************************
void find_fr_rf(designADT design, node* head,
		timed_node*& temp_timed_node) {

  bool found = false;

#ifdef __VERBOSE_TIMED__
  cout << "States where R->F or F->R exists for node "
       << head->label << endl;
#endif
  for (int i=0;i<PRIME;i++) {
    for (stateADT cur_state1=design->state_table[i];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {

      // Look at all next states from cur_state1
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	if ((cur_state1->colorvec[head->counter] == 'R') && 
	    (cur_state2->stateptr->colorvec[head->counter] == 'F')) {
#ifdef __VERBOSE_TIMED__
	  cout << "  R->F:";
#endif
	  found = true;
	}
	else if ((cur_state1->colorvec[head->counter] == 'F') && 
	    (cur_state2->stateptr->colorvec[head->counter] == 'R')) {
#ifdef __VERBOSE_TIMED__
	  cout << "  F->R:";
#endif
	  found = true;
	}
	if (found) {
	  push_timed(cur_state1, cur_state2, temp_timed_node);
	  found = false;
#ifdef __VERBOSE_TIMED__
	  print_timed_color_rf(design, cur_state1, cur_state2);
#endif
	}
      }
    }
  }
}

// ********************************************************************
// Find maximum and minimum delays in my decomposition from the
// reference transition to the node of interest. Entered once per F->R
// or R->F transition.
// ********************************************************************
void find_minmax(designADT design, int& mindelay, int& maxdelay,
		 node* input_vec[], int tot_num_inputs, node* head,
		 timed_node* temp_timed_node) {

  int runmin=INFIN;
  int runmax=0;
  node* temp_head;

  mindelay=0;
  maxdelay=INFIN;
  if ((tot_num_inputs > 0) && (!input_vec[0]->succ))
    return;
  for (int i=0;i<tot_num_inputs;i++) {
    temp_head = input_vec[i];

    if (temp_head->name == 
	design->events[temp_timed_node->edge->enabled]->signal) {
      
      // We have a signal name match. Sense doesn't matter.
      // Note that since we are assuming this event has caused
      // the change in evaluation in the new node, the min/max
      // times can be computed straight from the event to the node.
      mindelay = 0;
      maxdelay = 0;
      while ((temp_head->type != CEL) && (temp_head->type != OUTTEE) &&
	     (temp_head != head)) {
	temp_head = temp_head->succ;
	if (temp_head->type == INV) {
	  mindelay = mindelay + INVMINDEL;      
	  maxdelay = maxdelay + INVMAXDEL;
	}
	else if (temp_head->type == NAND2) {
	  mindelay = mindelay + NAND2MINDEL;      
	  maxdelay = maxdelay + NAND2MAXDEL;
	}
      }
    }
    if (temp_head == head) {
      if (mindelay < runmin)
	runmin = mindelay;
      if (maxdelay > runmax)
	runmax = maxdelay;
    }
  }
  mindelay = runmin;
  maxdelay = runmax;
}

// ********************************************************************
// Find the absolute maximum and minimum delays in my decomposition
// from any input to all nodes. Entered once per output. Writes the
// results to the min and max fields in each node.
// ********************************************************************
void find_abs_minmax(node* input_vec[], int tot_num_inputs) {

  int lev=1;
  bool stop=false;

  while (!stop) {
    for (int i=0;i<tot_num_inputs;i++) {
      node* temp_head = input_vec[i]->succ;
      while (temp_head->level < lev)
	temp_head = temp_head->succ;
      if (temp_head->level == lev) {
	if (temp_head->type == INV) {
	  temp_head->min = temp_head->pred1->min+INVMINDEL;
	  temp_head->max = temp_head->pred1->max+INVMAXDEL;
	}
	else if (temp_head->type == NAND2) {
	  if (temp_head->pred1->min < temp_head->pred2->min)
	    temp_head->min = temp_head->pred1->min+NAND2MINDEL;
	  else
	    temp_head->min = temp_head->pred2->min+NAND2MINDEL;
	  if (temp_head->pred1->max > temp_head->pred2->max)
	    temp_head->max = temp_head->pred1->max+NAND2MAXDEL;
	  else
	    temp_head->max = temp_head->pred2->max+NAND2MAXDEL;
	}
	else if (temp_head->type == CEL) {
	  if (temp_head->pred1->min < temp_head->pred2->min)
	    temp_head->min = temp_head->pred1->min+CELMINDEL;
	  else
	    temp_head->min = temp_head->pred2->min+CELMINDEL;
	  if (temp_head->pred1->max > temp_head->pred2->max)
	    temp_head->max = temp_head->pred1->max+CELMAXDEL;
	  else
	    temp_head->max = temp_head->pred2->max+CELMAXDEL;
	}
	else {
	  
	  // Must be at an output so quit
	  if (temp_head->pred1->type == INNEE) {
	    temp_head->min = 1;
	    temp_head->max = 1;
	  }
	  else {
	    temp_head->min = temp_head->pred1->min;
	    temp_head->max = temp_head->pred1->max;
	  }
	  stop = true;
	}
      }
    }
  lev++;
  }
}

// ********************************************************************
// Canonicalizes a zone. Uses Floyd's shortest path algorithm.
// ********************************************************************
void canon_zone(zone_curt* work_zone) {

  int i,j,k;
  int dim = work_zone->dim;
  int** clocks = work_zone->clocks;

  // Fix k at dim-1
  for (i=0;i<dim;i++) {
    for (j=0;j<dim-1;j++) {
      if ((clocks[j][i] != INFIN) &&
	  (clocks[i][dim-1] != INFIN) &&
	  (i != j)) {
	if (clocks[j][dim-1] > (clocks[j][i] + clocks[i][dim-1]))
	  clocks[j][dim-1] = clocks[j][i] + clocks[i][dim-1];
      }
    }
  }

  // Fix j at dim-1
  for (i=0;i<dim;i++) {
    for (k=0;k<dim-1;k++) {
      if ((clocks[dim-1][i] != INFIN) &&
	  (clocks[i][k] != INFIN) &&
	  (i != k)) {
	if (clocks[dim-1][k] > (clocks[dim-1][i] + clocks[i][k]))
	  clocks[dim-1][k] = clocks[dim-1][i] + clocks[i][k];
      }
    }
  }
}

// ********************************************************************
// Copies the clocks and clockkey data into the new zone from the
// starting state. Sets new entries to INFIN except the diagonal
// which gets set to 0. Puts transition into last entry of clockkey
// (key).
// ********************************************************************
void copy_first_zone(zone_curt* work_zone, clocklistADT clocklist,
		     int transition,int t_index) {

  int redim = work_zone->dim-1;
 
  // Copy clocks data to new zone
  for (int i=0;i<redim;i++) {
    for (int j=0;j<redim;j++)
      work_zone->clocks[i][j] = clocklist->clocks[i][j];
  }

  // Set new entries to INFIN except the diagonal
  for (int i=0;i<redim;i++) {
    work_zone->clocks[redim][i] = INFIN;    // New row
    work_zone->clocks[i][redim] = INFIN;    // New column
  }
  
  // Zero the diagonal entry
  work_zone->clocks[redim][redim] = 0;

  // Copy only the enabled field from clocklist->clockkey to the
  // new key field
  for (int i=0;i<redim;i++)
    work_zone->key[i] = clocklist->clockkey[i].enabled;
  
  // Add the new transiton to the new entry in the key vector
  work_zone->key[redim] = transition;
}

// ********************************************************************
// Copies the clocks and clockkey data into the new zone.
// Sets new entries to INFIN except diagonal, which gets set to 0.
// Puts transition into last entry of clockkey (key).
// ********************************************************************
void copy_zone_info(zone_curt* new_zone, zone_curt* work_zone,
		    int transition,int t_index) {

  int dim = work_zone->dim;
 
  new_zone->color = work_zone->color;

  // Copy clocks data to new zone
  for (int i=0;i<dim;i++) {
    for (int j=0;j<dim;j++)
      new_zone->clocks[i][j] = work_zone->clocks[i][j];
  }

  // Set new entries to INFIN except the diagonal
  for (int i=0;i<dim;i++) {
    new_zone->clocks[dim][i] = INFIN;    // New row
    if (i > t_index)
      new_zone->clocks[i][dim] = 0;    // New column
    else
      new_zone->clocks[i][dim] = INFIN;    // New column
  }
  
  // Zero the diagonal entry
  new_zone->clocks[dim][dim] = 0;

  // Copy the clockkey information
  for (int i=0;i<dim;i++)
    new_zone->key[i] = work_zone->key[i];
  
  // Add the new transiton to the new entry in the key vector
  new_zone->key[dim] = transition;
}

// ********************************************************************
// Fills the new entries in a zone with data from the rules matrix.
// ********************************************************************
void fill_zone_time_data(designADT design, zone_curt* work_zone,
			 node* input_vec[], bool general,
			 int transition) {
  
  int lower = 0;
  int upper = 0;
  int index = (work_zone->dim)-1;

#ifdef __VERBOSE_TIMED__
  print_rules(design);
#endif
  
  // Do the lower bounds first. Update any column entry for the
  // newly added transition but only if a rule exists in the rules
  // matrix in which case add the negative of the lower bound
  int column = work_zone->key[index];
  for (int row=0;row<index;row++) {
    if (design->rules[work_zone->key[row]][column]->ruletype != NORULE) {

      // If the transition is an output, let's get the timing information
      // from the nodelist rather than the rules matrix
      if (design->events[transition]->signal >= design->ninpsig) {
	if (general) {
	  lower = input_vec[design->events[transition]->signal]->min;
#ifdef __VERBOSE_TIMED__
	  cout << "Lower time from general IO node "
	       << design->signals[design->events[transition]->signal]->name;
	  print_plus_minus(transition);
	  cout << " = " << lower << endl;
#endif
	}
	else {
	  lower = design->rules[work_zone->key[row]][column]->lower;
	}
      }
      else
	lower = design->rules[work_zone->key[row]][column]->lower;
      work_zone->clocks[row][index] = 0-lower;
    }
    else {
#ifdef __VERBOSE_TIMED__
      cout << "No lower rule associated the pair [" << work_zone->key[row]
	   << "][" << work_zone->key[index] << "]" << endl;
#endif
    }
  }

  // Now do the upper bounds. Walk backwards through the clock key
  // until one ruletype is found and place the upper value from the
  // rules matrix into the zone, then stop.
  int row = work_zone->key[index];
  for (int column=index-1;column>=0;column--) {
    if (design->rules[work_zone->key[column]][row]->ruletype != NORULE) {
      
      // If the transition is an output, let's get the timing information
      // from the nodelist rather than the rules matrix
      if (design->events[transition]->signal >= design->ninpsig) {
	if (general) {
	  upper = input_vec[design->events[transition]->signal]->max;
#ifdef __VERBOSE_TIMED__
	    cout << "Upper time from general IO node "
		 << design->signals[design->events[transition]->signal]->name;
	    print_plus_minus(transition);
	    cout << " = " << upper << endl;
#endif
	}
	else {
	  upper = design->rules[work_zone->key[column]][row]->upper;
	}
      }
      else
	upper = design->rules[work_zone->key[column]][row]->upper;
      work_zone->clocks[index][column] = upper;
      break;
    }
    else {
#ifdef __VERBOSE_TIMED__
      cout << "No upper rule associated the pair [" << row
	   << "][" << work_zone->key[column] << "]" << endl;
#endif
    }
  }
}

// ********************************************************************
// Allocates memory for new zone of size dim x dim.
// ********************************************************************
zone_curt* make_new_zone(int dim) {

  // Allocate a pointer to the structure
  // This allocates memory for basic data types in the structures
  zone_curt* temp_zone = (zone_curt*)malloc(sizeof(zone_curt));
  temp_zone->dim = dim;
  
  // Allocate memory for the backbone of the array
  temp_zone->clocks = (int**)malloc(dim * sizeof(int*)); 
  for (int i=0;i<dim;i++)
    
    // Allocate memory for the ribs on the array
    temp_zone->clocks[i] = (int*)malloc(dim * sizeof(int));

  // Allocate new vector for clockkey data
  temp_zone->key = (int*)malloc(dim * sizeof(int));

  // Set dim field in the new zone
  temp_zone->dim = dim;
  return(temp_zone);
}

// ********************************************************************
// Frees the zone and it's contents
// ********************************************************************
void free_new_zone(zone_curt* temp_zone,int dim) {

  free(temp_zone->key);
  for (int i=0;i<dim;i++)
    free(temp_zone->clocks[i]);
  free(temp_zone->clocks);
  free(temp_zone);
}

// ********************************************************************
// Create and initialize a new timed node.
// ********************************************************************
timed_node* make_timed_node(void) {

  timed_node* new_node;
  new_node = new timed_node;
  return(new_node);
}

// ********************************************************************
// Create a new timed node. Puts state preceeding edge into node.
// ********************************************************************
void push_timed(stateADT cur_state1, statelistADT cur_state2,
		timed_node*& temp_timed_node) {
  
  temp_timed_node->cur_state = cur_state1;
  temp_timed_node->edge = cur_state2;
  temp_timed_node->succ = make_timed_node();
  temp_timed_node->succ->pred = temp_timed_node;
  temp_timed_node->succ->counter = (temp_timed_node->counter)+1;
  temp_timed_node = temp_timed_node->succ;
}
