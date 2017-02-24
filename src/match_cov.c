// ********************************************************************
// Functions for matching and covering the decomposition.
// ********************************************************************

#include "decomp.h"

// ********************************************************************
// Do the matching of the library to the decomposition. Annotate each
// node with the best cell at that point, based on the cost parameters.
// Entered once per output.
// ********************************************************************
void match_decomp(designADT design, node* input_vec[],
		  int tot_num_inputs, lib_node* lib_vec[],
		  int num_lib_cells, int pri_cost, int sec_cost,
		  int combo, nodelistADT head_nodelist) {
  static int area_tot=0;
  static int del_tot=0;

  int lev=1;
  node* head;
  
#ifdef __VERBOSE_MAT__
  print_decomp(design, input_vec, tot_num_inputs, combo);
#endif
  clear_visited_decomp(input_vec, tot_num_inputs);
  do {
    for (int i=0;i<tot_num_inputs;i++) {
      head = input_vec[i]->succ;
      while (head->level < lev) {
	head = head->succ;
      }
      if ((head->level == lev) &&
	  (head->type != OUTTEE) &&
	  (head->visited == FALSE)) {

	// Annotate this node with the best library cell cover
	// for all three cost functions
#ifdef __VERBOSE_MAT__
	cout << "Working on node " << head->label << endl;
#endif
	match_all_lib_cells(design, head, lib_vec, num_lib_cells, lev,
			    pri_cost, sec_cost, head_nodelist);
	head->visited = true;
#ifdef __VERBOSE_MAT__
	cout << endl;
#endif
      }
    }
    lev++;
  }
  while (head->type != OUTTEE);

  // Copy min/max info to the output cell
  head->min_time_cost = head->pred1->min_time_cost;
  head->max_time_cost = head->pred1->max_time_cost;
  area_tot=area_tot+head->pred1->area_cost;
  del_tot=del_tot+head->max_time_cost;
#ifdef __VERBOSE_MAT__
  cout << "Area = " << area_tot << endl;
  cout << "Delay = " << del_tot << endl;
  cout << "Printing decomp after matching..." << endl;
  print_decomp(design, input_vec, tot_num_inputs, combo);
#endif
}

// ********************************************************************
// Check all the library cells and see which one has the optimum cover
// for each cost function at this node. Entered once per node.
// ********************************************************************
void match_all_lib_cells(designADT design, node* head,
			 lib_node* lib_vec[], int num_lib_cells,
			 int lev, int pri_cost, int sec_cost,
			 nodelistADT head_nodelist) {

  // cell_min_del and cell_max_del contain the min and max delays at
  // the node of interest. They are calculated by taking the numbers
  // at the inputs and adding the cell delay info to get the final number.
  // cell_area_cost contains the area cost at the node of interest.
  // It is the sum of the area cost of the cell + the area cost of all
  // the inputs.
  // cell_haz_cost contains the number of exposed nodes that remain
  // if this cell is chosen as the best match. It is calculated as the
  // sum of exposed inputs + exposed output.
  // cell_int_mono_haz and cell_int_ack_haz contain the number of internal
  // nodes that contain these types of hazards if this library cell is used.
  int i;
  int cell_min_del;
  int cell_max_del;
  int cell_area_cost;
  int cell_haz_cost;
  int cell_int_mono_haz;
  int cell_int_ack_haz;
  int cell_ext_mono_haz;
  int cell_ext_ack_haz;
  int* sc_vec;
  int* ci_vec;

  // Go through all the cells in the library for this node
  for (int lib_index=0;lib_index<num_lib_cells;lib_index++) {
    if (lib_vec[lib_index]->num_levels <= lev) {

	// Allocate memory for each input in this library cell
      sc_vec = (int*)malloc((lib_vec[lib_index]->num_inputs+1)*sizeof(int));
      ci_vec = (int*)malloc((lib_vec[lib_index]->num_inputs+1)*sizeof(int));
      for (i=0;i<lib_vec[lib_index]->num_inputs;i++) {
	sc_vec[i] = -1;
	ci_vec[i] = -1;
      }
      sc_vec[i] = '\0';
      ci_vec[i] = '\0';
      cell_min_del=9999;
      cell_max_del=0;
      cell_area_cost=0;
      cell_haz_cost=0;
      cell_int_mono_haz=0;
      cell_int_ack_haz=0;
      cell_ext_mono_haz=0;
      cell_ext_ack_haz=0;
      
      // Add one if the current node is an ack hazard or mono hazard
//       if (head->ahazard)
// 	cell_ext_ack_haz++;
//       if (head->mhazard)
// 	cell_ext_mono_haz++;
#ifdef __VERBOSE_MAT__
      cout << "*** Checking library cell " << lib_vec[lib_index]->name << endl;
#endif
      if (match_one_lib_cell(design, head, lib_vec[lib_index], cell_min_del,
			     cell_max_del, cell_area_cost, cell_haz_cost,
			     cell_int_mono_haz, cell_int_ack_haz,
			     cell_ext_mono_haz, cell_ext_ack_haz, sc_vec,
			     ci_vec, head_nodelist, lib_vec)) {
	
	// Yes, this cell is a valid cover at this node
	// If it's a gC element, check for short circuits
#ifdef __VERBOSE_MAT__
	cout << "  ... cell is valid cover" << endl;
#endif
	if (!(lib_vec[lib_index]->gc) || 
	    !(short_circuit(design, sc_vec, lib_vec[lib_index]))) {
	  
	  // cell_haz_cost contains the number of hazardous nodes
	  // at this point
	  cell_max_del = cell_max_del + lib_vec[lib_index]->max_del;
	  cell_min_del = cell_min_del + lib_vec[lib_index]->min_del;
	  cell_area_cost = cell_area_cost + lib_vec[lib_index]->area;
	  
#ifdef __VERBOSE_TEST__
	  if ((cell_int_mono_haz != 0) ||
	      (cell_ext_mono_haz != 0) ||
	      (cell_int_ack_haz != 0) ||
	      (cell_ext_ack_haz != 0)) {
	    cout << "Node " << head->label << " Cell " <<
	      lib_vec[lib_index]->name << endl;
	    cout << "int mono=" << cell_int_mono_haz
		 << " ext mono=" << cell_ext_mono_haz
		 << " int ack=" << cell_int_ack_haz
		 << " ext ack=" << cell_ext_ack_haz
		 << " old cost=" << cell_haz_cost << endl;
	  }
#endif
#ifdef __VERBOSE_MAT__
	  cout << "int mono=" << cell_int_mono_haz
	       << " ext mono=" << cell_ext_mono_haz
	       << " int ack=" << cell_int_ack_haz
	       << " ext ack=" << cell_ext_ack_haz
	       << " old cost=" << cell_haz_cost;
#endif
	  // The heurestic algorithm for determing hazard cost
 	  cell_haz_cost = cell_haz_cost - 0* cell_ext_mono_haz + 1*cell_ext_ack_haz;
#ifdef __VERBOSE_MAT__
	  cout << " new cost=" << cell_haz_cost << endl;
#endif
	  if (pri_cost == HAZARD)
	    update_hazard(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost, sec_cost);
	  else if (pri_cost == DELAY)
	    update_delay(head, lib_index, cell_max_del, cell_min_del,
			 cell_area_cost, cell_haz_cost, sec_cost);
	  else
	    update_area(head, lib_index, cell_max_del, cell_min_del,
			cell_area_cost, cell_haz_cost, sec_cost);
	}
      }
      free(sc_vec);
      free(ci_vec);
    }
  }
}

// ********************************************************************
// Check one library cell for coverage at the node passed as head.
// ********************************************************************
bool match_one_lib_cell(designADT design, node* decomp_head,
			lib_node* lib_element, int& cell_min_del,
			int& cell_max_del, int& cell_area_cost,
			int& cell_haz_cost, int& cell_int_mono_haz,
			int& cell_int_ack_haz, int& cell_ext_mono_haz, 
			int& cell_ext_ack_haz, int* sc_vec, int* ci_vec,
			nodelistADT head_nodelist, lib_node* lib_vec[]) {
  node* lib_head = lib_element->pred;
  bool pred1 = true;

  // Bail out if the gates are not the same type
  if (lib_head->type != decomp_head->type)
    return false;

  if (lib_head->pred1 != NULL) {
    if (decomp_head->pred1 == NULL)
      return false;
    if (!match_one_lib_cell_recurse(design, decomp_head->pred1,
				    lib_head->pred1, cell_min_del,
				    cell_max_del, cell_area_cost,
				    cell_haz_cost, cell_int_mono_haz,
				    cell_int_ack_haz, cell_ext_mono_haz, 
				    cell_ext_ack_haz, sc_vec, pred1,
				    lib_element, ci_vec, head_nodelist,
				    lib_vec))
      return false;
  }
  // I am at the end of an input for pred1 in this library cell
  else {
    // Check for short circuit if this library gate is a gC
    if (lib_element->gc)
      match_input(design, lib_head, decomp_head, lib_element, sc_vec, pred1); 
    // Check for common inputs
    if (lib_element->com_inp) {
      if (!com_input_chk(design, lib_head, decomp_head,
			 lib_element, ci_vec, pred1, head_nodelist))
	return false;
    }
//     if (decomp_head->pred1->ahazard)
//       cell_ext_ack_haz++;

// Determine number of external ack hazards (a buffer on the node of
// interest is a special case)
    if ((decomp_head->pred1->ahazard) &&
	(decomp_head->pred1->lib_num >= 0) &&
	(lib_vec[decomp_head->pred1->lib_num]->area != BUFFER_AREA)) {
      cell_ext_ack_haz++;
    }
    else {
      if ((decomp_head->pred1->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred1->lib_num]->area == BUFFER_AREA) &&
	  (decomp_head->pred1->pred1->pred1->type != INNEE)) {
	if ((decomp_head->pred1->ahazard) ||
	    (decomp_head->pred1->pred1->pred1->ahazard))
	  cell_ext_ack_haz++;
      }
    }
    if ((decomp_head->pred1->mhazard) &&
	(((decomp_head->pred1->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred1->lib_num]->area != BUFFER_AREA)) ||
	 (((decomp_head->pred1->lib_num >= 0) && 
	   (lib_vec[decomp_head->pred1->lib_num]->area == BUFFER_AREA)) &&
	  (decomp_head->pred1->pred1->pred1->type != INNEE))))
      cell_ext_mono_haz++;
    update_costs(decomp_head->pred1, cell_max_del, cell_min_del,
		 cell_area_cost, cell_haz_cost);
  }
  if (lib_head->pred2 != NULL) {
    if (decomp_head->pred2 == NULL)
      return false;
    if (!match_one_lib_cell_recurse(design, decomp_head->pred2,
				    lib_head->pred2, cell_min_del,
				    cell_max_del, cell_area_cost,
				    cell_haz_cost, cell_int_mono_haz,
				    cell_int_ack_haz, cell_ext_mono_haz, 
				    cell_ext_ack_haz, sc_vec, pred1,
				    lib_element, ci_vec, head_nodelist,
				    lib_vec))
      return false;
  }
  
  // I am at the end of an input for pred2 in this library cell
  else if (decomp_head->pred2 != NULL) {
    if (lib_element->gc) {
      pred1 = false;
      match_input(design, lib_head, decomp_head, lib_element, sc_vec, pred1); 
    }
    if (lib_element->com_inp) {
      pred1 = false;
      if (!com_input_chk(design, lib_head, decomp_head,
			 lib_element, ci_vec, pred1, head_nodelist))
	return false;
    }
    if ((decomp_head->pred2->ahazard) &&
	(decomp_head->pred2->lib_num >= 0) &&
	(lib_vec[decomp_head->pred2->lib_num]->area != BUFFER_AREA))
      cell_ext_ack_haz++;
    else {
      if ((decomp_head->pred2->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred2->lib_num]->area == BUFFER_AREA) &&
	  (decomp_head->pred2->pred1->pred1->type != INNEE)) {
	if ((decomp_head->pred2->ahazard) ||
	    (decomp_head->pred2->pred1->pred1->ahazard))
	  cell_ext_ack_haz++;
      }
    }
    if ((decomp_head->pred2->mhazard) &&
	(((decomp_head->pred2->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred2->lib_num]->area != BUFFER_AREA)) ||
	 (((decomp_head->pred2->lib_num >= 0) &&
	   (lib_vec[decomp_head->pred2->lib_num]->area == BUFFER_AREA)) &&
	  (decomp_head->pred2->pred1->pred1->type != INNEE))))
      cell_ext_mono_haz++;
    update_costs(decomp_head->pred2, cell_max_del, cell_min_del,
		 cell_area_cost, cell_haz_cost);
  }
  return true;
}

// ********************************************************************
// Recursively check a library cell against the decomposition.
// ********************************************************************
bool match_one_lib_cell_recurse(designADT design, node* decomp_head,
				node* lib_head, int& cell_min_del,
				int& cell_max_del, int& cell_area_cost,
				int& cell_haz_cost, int& cell_int_mono_haz,
				int& cell_int_ack_haz, int& cell_ext_mono_haz, 
				int& cell_ext_ack_haz, int* sc_vec,
				bool pred1, lib_node* lib_element,
				int* ci_vec, nodelistADT head_nodelist,
				lib_node* lib_vec[]) {

  if (lib_head->type != decomp_head->type)
    return false;
  if (decomp_head->mhazard)
    cell_int_mono_haz++;
  if (decomp_head->ahazard)
    cell_int_ack_haz++;
  if (lib_head->pred1 != NULL) {
    if (decomp_head->pred1 == NULL)
      return false;
    if (!match_one_lib_cell_recurse(design, decomp_head->pred1,
				    lib_head->pred1, cell_min_del,
				    cell_max_del, cell_area_cost,
				    cell_haz_cost, cell_int_mono_haz,
				    cell_int_ack_haz, cell_ext_mono_haz, 
				    cell_ext_ack_haz, sc_vec, pred1,
				    lib_element, ci_vec, head_nodelist,
				    lib_vec))
      return false;
  }
  
  // I am at the end of the library cell for pred1
  else {
    
    // Check for short circuit if this library gate is a gC
    if (lib_element->gc)
      match_input(design, lib_head, decomp_head, lib_element, sc_vec, pred1); 
    // Check for common inputs
    if (lib_element->com_inp) {
      if (!com_input_chk(design, lib_head, decomp_head,
			 lib_element, ci_vec, pred1, head_nodelist))
	return false;
    }

    if ((decomp_head->pred1->ahazard) &&
	((decomp_head->pred1->lib_num >= 0) &&
	 lib_vec[decomp_head->pred1->lib_num]->area != BUFFER_AREA))
      cell_ext_ack_haz++;
    else {
      if ((decomp_head->pred1->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred1->lib_num]->area == BUFFER_AREA) &&
	  (decomp_head->pred1->pred1->pred1->type != INNEE)) {
	if ((decomp_head->pred1->ahazard) ||
	    (decomp_head->pred1->pred1->pred1->ahazard))
	  cell_ext_ack_haz++;
      }
    }
    if ((decomp_head->pred1->mhazard) &&
	(((decomp_head->pred1->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred1->lib_num]->area != BUFFER_AREA)) ||
	 (((decomp_head->pred1->lib_num >= 0) &&
	   (lib_vec[decomp_head->pred1->lib_num]->area == BUFFER_AREA)) &&
	  (decomp_head->pred1->pred1->pred1->type != INNEE))))
      cell_ext_mono_haz++;
    update_costs(decomp_head->pred1, cell_max_del, cell_min_del,
		 cell_area_cost, cell_haz_cost);
  }
  if (decomp_head->type == INNEE)
    return false;
  if (lib_head->pred2 != NULL) {
    if (decomp_head->pred2 == NULL)
      return false;
    if (!match_one_lib_cell_recurse(design, decomp_head->pred2,
				    lib_head->pred2, cell_min_del,
				    cell_max_del, cell_area_cost,
				    cell_haz_cost, cell_int_mono_haz,
				    cell_int_ack_haz, cell_ext_mono_haz, 
				    cell_ext_ack_haz, sc_vec, pred1,
				    lib_element, ci_vec, head_nodelist,
				    lib_vec))
      return false;
  }
  
  // I am at the end of the library cell for pred2
  else if (decomp_head->pred2 != NULL) {
    if (lib_element->gc) {
      pred1 = false;
      match_input(design, lib_head, decomp_head, lib_element, sc_vec, pred1); 
    }
    if (lib_element->com_inp) {
      pred1 = false;
      if (!com_input_chk(design, lib_head, decomp_head,
			 lib_element, ci_vec, pred1, head_nodelist))
	return false;
    }
    if ((decomp_head->pred2->ahazard) &&
	(decomp_head->pred2->lib_num >= 0) &&
	(lib_vec[decomp_head->pred2->lib_num]->area != BUFFER_AREA))
      cell_ext_ack_haz++;
    else {
      if ((decomp_head->pred2->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred2->lib_num]->area == BUFFER_AREA) &&
	  (decomp_head->pred2->pred1->pred1->type != INNEE)) {
	if ((decomp_head->pred2->ahazard) ||
	    (decomp_head->pred2->pred1->pred1->ahazard))
	  cell_ext_ack_haz++;
      }
    }
    if ((decomp_head->pred2->mhazard) &&
	(((decomp_head->pred2->lib_num >= 0) &&
	  (lib_vec[decomp_head->pred2->lib_num]->area != BUFFER_AREA)) ||
	 (((decomp_head->pred2->lib_num >= 0) &&
	   (lib_vec[decomp_head->pred2->lib_num]->area == BUFFER_AREA)) &&
	  (decomp_head->pred2->pred1->pred1->type != INNEE))))
      cell_ext_mono_haz++;
    update_costs(decomp_head->pred2, cell_max_del, cell_min_del,
		 cell_area_cost, cell_haz_cost);
  }
  return true;
}

// ********************************************************************
// Update all cost parameters.
// ********************************************************************
void update_costs(node* temp_head, int& cell_max_del, int& cell_min_del,
		  int& cell_area_cost, int& cell_haz_cost) {
  
  // Take the largest time of all the inputs for the max
  if (temp_head->max_time_cost > cell_max_del)
    cell_max_del = temp_head->max_time_cost;

  // Take the smallest non-zero time of all the inputs for the min
  if (temp_head->min_time_cost < cell_min_del)
    cell_min_del = temp_head->min_time_cost;
  
  // Do area update
  cell_area_cost = cell_area_cost + temp_head->area_cost;

  // Do hazard update
  cell_haz_cost = cell_haz_cost + temp_head->hazard_cost;
}

// ********************************************************************
// Check for short circuits to this gC element.  The cell is already
// a structural match when reaching this point.
// ********************************************************************
bool short_circuit(designADT design, int* sc_vec,
		   lib_node* lib_element) {

  level_exp sc_temp = NULL;
  level_exp sc_head = NULL;
  char* color_temp;
  char* set_product;
  char* reset_product;
  bool short_circ = false;

#ifdef __VERBOSE_SC__
  cout << "Checking nodes  ";
  for (int k=0;k<lib_element->num_inputs-1;k++)
    cout << sc_vec[k] << "  ";
  cout << "for short circuits..." << endl;
#endif
  sc_temp = (level_exp)GetBlock(sizeof(*sc_temp));
  sc_temp->next = NULL;
  sc_temp->product = (char*)GetBlock(lib_element->num_inputs * sizeof(char));
  sc_temp->product[lib_element->num_inputs-1] = '\0';
  sc_head = sc_temp;

//   sc_temp = (char*)malloc(lib_element->num_inputs * sizeof(char));
//   sc_head = (level_exp)malloc(sizeof(level_exp));
//   sc_temp[lib_element->num_inputs - 1] = (char)NULL;
  color_temp = (char*)malloc(lib_element->num_inputs * sizeof(char));
  color_temp[lib_element->num_inputs - 1] = (char)NULL;

  for (level_exp curr_set=lib_element->set;curr_set;curr_set=curr_set->next) {
#ifdef __VERBOSE_SC__
    cout << "set product = " << curr_set->product << endl;
#endif
    set_product = curr_set->product;
    for (level_exp curr_reset=lib_element->reset;curr_reset;
	 curr_reset=curr_reset->next) {
#ifdef __VERBOSE_SC__
      cout << "reset product = " << curr_reset->product << endl;
#endif
      reset_product = curr_reset->product;
      
      // Find the intersection of the set and reset products
      for (int j=0;j<lib_element->num_inputs-1;j++) {
	if ((set_product[j] == '0') && (reset_product[j] == '0') ||
	    (set_product[j] == '0') && (reset_product[j] == 'X') ||
	    (set_product[j] == 'X') && (reset_product[j] == '0'))
	  sc_temp->product[j] = '0';
	else if ((set_product[j] == '1') && (reset_product[j] == '1') ||
		 (set_product[j] == '1') && (reset_product[j] == 'X') ||
		 (set_product[j] == 'X') && (reset_product[j] == '1'))
	  sc_temp->product[j] = '1';
	else if ((set_product[j] == 'X') && (reset_product[j] == 'X'))
	  sc_temp->product[j] = 'X';
	else {
	  
	  // Empty set - no short circuit possible
#ifdef __VERBOSE_SC__
	  cout << "No short circuit for this library element - intersection is NULL" << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
// 	  cout << "No short circuit for " << lib_element->name;
// 	  cout << " - intersection is NULL" << endl;
// #endif
	  return short_circ;
	}
      }
#ifdef __VERBOSE_SC__
      cout << "intersection of set and reset products = "
	   << sc_temp->product << endl;
#endif
      sc_temp->next = (level_exp)GetBlock(sizeof(*sc_temp));
      sc_temp = sc_temp->next;
      sc_temp->next = NULL;
      sc_temp->product = (char*)GetBlock(lib_element->num_inputs * sizeof(char));
      sc_temp->product[lib_element->num_inputs-1] = '\0';
    }
  }

  // Check for short circuits in all states
  for (int i=0;i<PRIME;i++) {
    for (stateADT cur_state1=design->state_table[i];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
#ifdef __VERBOSE_SC__
      cout << "State = " << cur_state1->state
	   << "  Colorvec = " << cur_state1->colorvec;
#endif
      for (level_exp cur_le=sc_head;cur_le&&cur_le->next;cur_le=cur_le->next) {
#ifdef __VERBOSE_SC__
	cout << "  current product = " << cur_le->product << endl;
#endif
	for (int k=0;k<lib_element->num_inputs-1;k++) {
	  if (sc_vec[k] >= 10000) {
	    
	    // Indicates a primary input
	    int index = sc_vec[k] - 10000;
	    if ((cur_state1->state[index] == '0') ||
		(cur_state1->state[index] == 'R'))
	      color_temp[k] = '0';
	    else
	      color_temp[k] = '1';
	  }
	  else
	    color_temp[k] = cur_state1->colorvec[sc_vec[k]];
	}
#ifdef __VERBOSE_SC__
	cout << "  Short-circuit color vector = " << color_temp << endl;
#endif
	if (sc_problem(color_temp, cur_le->product, lib_element->num_inputs)) {
#ifdef __VERBOSE_SC__
	  cout << "Short-circuit problem in state "
	       << cur_state1->state << endl;
#endif
	  short_circ = true;
	}
      }
    }
  }
#ifdef __VERBOSE_SC__
  if (!short_circ)
    cout << "No short circuit for this library element" << endl;
  cout << endl;
#endif
#ifdef __VERBOSE_GENGCINFO__
  if (!short_circ)
    cout << lib_element->name << " has no short circuits" << endl;
  else
    cout << "Rejecting " << lib_element->name
	 << " because of short circuits" << endl;
#endif
  return short_circ;
}

// ********************************************************************
// Check for a short between the colorvec extracted from the state and
// the sc_vec created from the set and reset sop terms
// ********************************************************************
bool sc_problem(char* color_temp, char* cur_prod, int num_inputs) {

  for (int k=0;k<num_inputs-1;k++) {
    if ((color_temp[k] == '0') && (cur_prod[k] == '1') ||
	(color_temp[k] == '1') && (cur_prod[k] == '0'))
      return false;
  }
  return true;
}

// ********************************************************************
// Finds a match between one of the inputs in the gC library element
// and the input placed at lib_head, and writes the node number from
// decomp_head into the correct position in the sc_vec.
// ********************************************************************
void match_input(designADT design, node* lib_head, node* decomp_head,
		 lib_node* lib_element, int* sc_vec, bool pred1) {

  int length = 0;
  char* lib_label;
  char* input_label;
  bool equal = false;;

#ifdef __VERBOSE_SC__
  cout << "Building vector of short circuit node numbers ..." << endl;
  cout << "node number for current location = "
       << decomp_head->counter << endl;
  cout << "pred1 flag = " << pred1 << endl;
  if (decomp_head->pred1->type == INNEE)
    cout << "pred1 is primary input "
	 <<design->signals[decomp_head->pred1->name]->name << endl;
  else
    cout << "node number for pred1 = " << decomp_head->pred1->counter << endl;
  if (decomp_head->pred2) { 
    if (decomp_head->pred2->type == INNEE)
      cout << "pred2 is primary input "
	   << design->signals[decomp_head->pred2->name]->name << endl;
    else
      cout << "node number for pred2 = "
	   << decomp_head->pred2->counter << endl;
  }
  else
    cout << "pred2 is NULL" << endl;
//   cout << "gc input label1 from lib leaf = " << lib_head->input1 << endl;
//   if (lib_head->pred2)
//     cout << "gc input label2 from lib leaf = " << lib_head->input2 << endl;
//   cout << "first lib element input from label vector = "
//        << lib_element->inp_labls[0] << endl;
#endif
  
  lib_label = lib_head->input1;
  if (!pred1)
    lib_label = lib_head->input2;

  for (int i=0;i<lib_element->num_inputs-1;i++) {
    input_label = lib_element->inp_labls[i];
    equal = true;
    length = strlen(lib_label);
    for (int j=0;j<length;j++) {
      if (lib_label[j] != input_label[j]) {
	equal = false;
	break;
      }
    }
    if (equal) {
      if (pred1) {
	if (decomp_head->pred1->type == INNEE)
	  
	  // A number >10000 indicates this node in the sc_vec is a
	  // primary input
	  sc_vec[i] = 10000 + decomp_head->pred1->name;
	else
	  sc_vec[i] = decomp_head->pred1->counter;
      }
      else {
	if (decomp_head->pred2->type == INNEE)
	  sc_vec[i] = 10000 + decomp_head->pred2->name;
	else
	  sc_vec[i] = decomp_head->pred2->counter;
      }
      break;
    }
  }
#ifdef __VERBOSE_SC__
  cout << "indices of leaf nodes in subject graph = ";
  for (int k=0;k<lib_element->num_inputs-1;k++)
    cout << "  " << sc_vec[k];
  cout << endl << endl;
#endif
}

// ********************************************************************
// Called when a leaf cell of a library element has been reached.
// Determines whether or not the input of the lib element has already
// been assigned.  If so, checks that the circuits driving these two
// nodes are equivalent.  If not, rejects this element.  If so, makes
// an indication that one of these nodes can be omitted during
// covering.  Return true means the inputs are common.
// ********************************************************************
bool com_input_chk(designADT design, node* lib_head, node* decomp_head,
		   lib_node* lib_element, int* ci_vec, bool pred1,
		   nodelistADT head_nodelist) {

#ifdef __VERBOSE_CI__
  cout << "Checking for common input labels ... " << endl;
  cout << "pred1 flag = " << pred1 << endl;
  cout << "Node number for current location in circuit = "
       << decomp_head->counter << endl;
  if (decomp_head->pred1->type == INNEE)
    cout << "pred1 is primary input "
	 <<design->signals[decomp_head->pred1->name]->name << endl;
  else
    cout << "node number for pred1 = " << decomp_head->pred1->counter << endl;
  if (decomp_head->pred2) { 
    if (decomp_head->pred2->type == INNEE)
      cout << "pred2 is primary input "
	   << design->signals[decomp_head->pred2->name]->name << endl;
    else
      cout << "node number for pred2 = "
	   << decomp_head->pred2->counter << endl;
  }
  else
    cout << "pred2 is NULL" << endl;
  if (pred1)
    cout << "input label1 from library leaf = " << lib_head->input1 << endl;
  else
    cout << "input label2 from library leaf = " << lib_head->input2 << endl;
//   if (lib_head->pred1) {
//     if (lib_head->pred1->input1)
//       cout << "input label1 from library leaf = " << lib_head->input1 << endl;
//     else
//       cout << "input label1 from library leaf is null" << endl;
//   }
//   else if (lib_head->input1)
//     cout << "input label1 from library leaf = " << lib_head->input1 << endl;
//  }
//   if (lib_head->pred2) {
//     if (lib_head->pred2->input2)
//       cout << "input label2 from library leaf = " << lib_head->input2 << endl;
//     else
//       cout << "input label2 from library leaf is null" << endl;
//   }
//   else
//     cout << "pred2 from library leaf is null" << endl;
  for (int j=0;j<lib_element->num_inputs;j++)
     cout << "library element label = " << lib_element->inp_labls[j]
	 << "  common input vector = " << ci_vec[j] << endl;
  cout << "library element name = " << lib_element->name << endl;
  cout << "library element label1 index = " << lib_head->input1_index << endl;
  cout << "library element label2 index = " << lib_head->input2_index << endl;
#endif

  if (pred1) {
    if (ci_vec[lib_head->input1_index] == -1) {
      if (decomp_head->pred1->type == INNEE)

	// >= 10000 indicates that this leaf in the library cell is attached
	// to a primary input
	ci_vec[lib_head->input1_index] = 10000 + decomp_head->pred1->name;
      else
	ci_vec[lib_head->input1_index] = decomp_head->pred1->counter;
    }
    else
      
      // Must check these two nodes for equivalance
      if (decomp_head->pred1->type == INNEE)
	return (equal_chk(design, ci_vec[lib_head->input1_index],
			  10000 + decomp_head->pred1->name, head_nodelist));
      else
	return (equal_chk(design, ci_vec[lib_head->input1_index],
			  decomp_head->pred1->counter, head_nodelist));
  }
  else {
    if (ci_vec[lib_head->input2_index] == -1) {
      if (decomp_head->pred2->type == INNEE)
	ci_vec[lib_head->input2_index] = 10000 + decomp_head->pred2->name;
      else
	ci_vec[lib_head->input2_index] = decomp_head->pred2->counter;
    }
    else
      
      // Must check these two nodes for equivalance
      if (decomp_head->pred2->type == INNEE)
	return (equal_chk(design, ci_vec[lib_head->input2_index],
			  10000 + decomp_head->pred2->name, head_nodelist));
      else
	return (equal_chk(design, ci_vec[lib_head->input2_index],
			  decomp_head->pred2->counter, head_nodelist));
  }
#ifdef __VERBOSE_CI__
  cout << "Common input vector ...";
  for (int i=0;i<lib_element->num_inputs;i++)
    cout << "  " << ci_vec[i];
  cout << endl;
#endif
  return true;
}

// ********************************************************************
// Checks to see if the circuits driving the two nodes passed are equal.
// Return false indicates that the sub-circuits are not equivalent.
// Node numbers >= 10000 indicate primary inputs
// ********************************************************************
bool equal_chk(designADT design, int num1, int num2,
	       nodelistADT head_nodelist) {

  bool equal_nodes;
  bool pred1 = true;
  node* node1 = NULL;
  node* node2 = NULL;

#ifdef __VERBOSE_CI__
  cout << "Now doing equality check for nodes " << num1
       << " and " << num2 << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
//   cout << "Now doing equality check for nodes " << num1
//        << " and " << num2 << endl;
// #endif
  // First, take care of cases where one of the nodes is a primary
  // input
  if ((num1 >= 10000) && (num2 >= 10000)) {
    if (design->signals[num1-10000]->name ==
	design->signals[num2-10000]->name) {
#ifdef __VERBOSE_CI__
      cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
//       cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
// #endif
      return true;
    }
  }
  else if (num1 >= 10000) {
    node2 = findnode(head_nodelist, num2);
    if (node2->pred1->pred1) {
      if (node2->pred1->pred1->type == INNEE) {
	if (node2->pred1->pred1->name == num1 - 10000) {
#ifdef __VERBOSE_CI__
	  cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
//       cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
// #endif
	  return true;
	}
      }
    }
  }
  else if (num2 >= 10000) {
    node1 = findnode(head_nodelist, num1);
    if (node1->pred1->pred1) {
      if (node1->pred1->pred1->type == INNEE) {
	if (node1->pred1->pred1->name == num2-10000) {
#ifdef __VERBOSE_CI__
	  cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
//       cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
// #endif
	  return true;
	}
      }
    }
  }
  else {

    // find the nodes attached to these two numbers
    node1 = findnode(head_nodelist, num1);
    node2 = findnode(head_nodelist, num2);
    equal_nodes = node_compare(design, node1, node2, pred1);
    if (equal_nodes) {
#ifdef __VERBOSE_CI__
      cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
//       cout << "Nodes " << num1 << " and " << num2 << " are equal" << endl;
// #endif
      return true;
    }
  }
#ifdef __VERBOSE_CI__
  cout << "Nodes " << num1 << " and " << num2 << " are NOT equal" << endl;
#endif
// #ifdef __VERBOSE_GENGCINFO__
//   cout << "Nodes " << num1 << " and " << num2 << " are NOT equal" << endl;
// #endif
  return false;
}

// ********************************************************************
// Find the node of type node* for the counter number passed here.
// ********************************************************************
node* findnode(nodelistADT head_nodelist, int num) {
  
  nodelistADT curnode;

  for (curnode=head_nodelist;(curnode && curnode->this_node);
       curnode=curnode->link) {
    if (curnode->this_node->counter == num)
      return curnode->this_node;
  }
  return curnode->this_node;
}

// ********************************************************************
// Recursively check if the circuits driving the two nodes
// are equivalent.
// ********************************************************************
bool node_compare(designADT design, node* node1, node* node2,
		  bool pred1) {

  // Bail out if the gates are not the same type
  if (node1->type != node2->type)
    return false;
  if (node1->pred1->type == INNEE) {
    if (node2->pred1->type == INNEE) {
      if (node1->pred1->name == node2->pred1->name)
	return true;
      else
	return false;
    }
    else if ((node2->pred1->type == INV) &&
	     (node2->pred1->pred1) &&
	     (node2->pred1->pred1->type == INV) &&
	     (node2->pred1->pred1->pred1->type == INNEE) &&
	     (node2->pred1->pred1->pred1->name == node1->pred1->name))
      return true;
    else
      return false;
  }
  if (node2->pred1->type == INNEE) {
    if ((node1->pred1->type == INV) &&
	(node1->pred1->pred1) &&
	(node1->pred1->pred1->type == INV) &&
	(node1->pred1->pred1->pred1->type == INNEE) &&
	(node1->pred1->pred1->pred1->name == node2->pred1->name))
      return true;
    else
      return false;
  }
  pred1 = true;
  if (!node_compare(design, node1->pred1, node2->pred1, pred1))
    return false;
  if (node1->pred2 != NULL) {
    if (node2->pred2 == NULL)
      return false;
    pred1 = false;
    if (!node_compare(design, node1->pred2, node2->pred2, pred1))
      return false;
  }
  return true;
}

// ********************************************************************
// Updates the information for a given node during matching based on
// HAZARD being the primary cost function.
// ********************************************************************
void update_hazard(node* head, int lib_index, int cell_max_del,
		   int cell_min_del, int cell_area_cost,
		   int cell_haz_cost, int sec_cost) {
  
  // See if we need to reduce max hazards at this node
  if ((cell_haz_cost < head->hazard_cost) ||
      ((head->hazard_cost == 0) && (head->lib_num == LIBINIT)))
    update_node_costs(head, lib_index, cell_max_del, cell_min_del,
		      cell_area_cost, cell_haz_cost);
  else if (cell_haz_cost == head->hazard_cost) {

    // Hazard cost of the cell = Hazard cost stored at the node
    // Check secondary cost function
    if (sec_cost == DELAY) {
      if (cell_max_del < head->max_time_cost)
	update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost);
      else if (cell_max_del == head->max_time_cost) {

	// Check AREA as a tertiary cost function
	if (cell_area_cost < head->area_cost)
	  update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			    cell_area_cost, cell_haz_cost);
	else
	  print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			      cell_haz_cost);
      }
      else
	print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			    cell_haz_cost);
    }
    else {

      // Secondary cost is AREA
      if (cell_area_cost < head->area_cost)
	update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost);
      else if (cell_area_cost == head->area_cost) {

	// Check DELAY as a tertiary cost function
	if (cell_max_del < head->max_time_cost)
	  update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			    cell_area_cost, cell_haz_cost);
	else
	  print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			      cell_haz_cost);
      }
      else
	print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			    cell_haz_cost);
    }
  }
  else
    print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			cell_haz_cost);
}

// ********************************************************************
// Updates the information for a given node during matching based on
// DELAY being the primary cost function.
// ********************************************************************
void update_delay(node* head, int lib_index, int cell_max_del,
		  int cell_min_del, int cell_area_cost,
		  int cell_haz_cost, int sec_cost) {
  
  // See if we need to reduce max delay at this node
  if ((cell_max_del < head->max_time_cost) ||
      (head->lib_num == LIBINIT))
    update_node_costs(head, lib_index, cell_max_del, cell_min_del,
		      cell_area_cost, cell_haz_cost);
  else if (cell_max_del == head->max_time_cost) {

    // Max delay cost of the cell = Max delay cost stored at the node
    // Check secondary cost function
    if (sec_cost == HAZARD) {
      if (cell_haz_cost < head->hazard_cost)
	update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost);
      else if (cell_haz_cost == head->hazard_cost) {
	
	// Check AREA as a tertiary cost function
	if (cell_area_cost < head->area_cost)
	  update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			    cell_area_cost, cell_haz_cost);
	else
	  print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			      cell_haz_cost);
      }
      else
	print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			    cell_haz_cost);
    }
    else {

      // Secondary cost is AREA
      if (cell_area_cost < head->area_cost)
	update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost);
      else if (cell_area_cost == head->area_cost) {
	
	// Check HAZARD as a tertiary cost function
	if (cell_haz_cost < head->hazard_cost)
	  update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			    cell_area_cost, cell_haz_cost);
	else
	  print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			      cell_haz_cost);
      }
      else
	print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			    cell_haz_cost);
    }
  }
  else
    print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			cell_haz_cost);
}

// ********************************************************************
// Updates the information for a given node during matching based on
// AREA being the primary cost function.
// ********************************************************************
void update_area(node* head, int lib_index, int cell_max_del,
		 int cell_min_del, int cell_area_cost,
		 int cell_haz_cost, int sec_cost) {
  
  // See if we need to reduce max area at this node
  if ((cell_area_cost < head->area_cost) ||
      (head->lib_num == LIBINIT))
    update_node_costs(head, lib_index, cell_max_del, cell_min_del,
		      cell_area_cost, cell_haz_cost);
  else if (cell_area_cost == head->area_cost) {

    // Area cost of the cell = Area cost stored at the node
    // Check secondary cost function
    if (sec_cost == HAZARD) {
      if (cell_haz_cost < head->hazard_cost)
	update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost);
      else if (cell_haz_cost == head->hazard_cost) {
	
	// Check DELAY as a tertiary cost function
	if (cell_max_del < head->max_time_cost)
	  update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			    cell_area_cost, cell_haz_cost);
	else
	  print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			      cell_haz_cost);
      }
      else
	print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			    cell_haz_cost);
    }
    else {

      // Secondary cost is DELAY
      if (cell_max_del < head->max_time_cost)
	update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			  cell_area_cost, cell_haz_cost);
      else if (cell_max_del == head->max_time_cost) {
	
	// Check HAZARD as a tertiary cost function
	if (cell_haz_cost < head->hazard_cost)
	  update_node_costs(head, lib_index, cell_max_del, cell_min_del,
			    cell_area_cost, cell_haz_cost);
	else
	  print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			      cell_haz_cost);
      }
      else
	print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			    cell_haz_cost);
    }
  }
  else
    print_no_match_info(cell_area_cost, cell_min_del, cell_max_del,
			cell_haz_cost);
}

// ********************************************************************
// Updates 5 node parameters relating to the library cell chosen.
// ********************************************************************
void update_node_costs(node* head, int lib_index, int cell_max_del,
		   int cell_min_del, int cell_area_cost,
		   int cell_haz_cost) {

#ifdef __VERBOSE_MAT__
  cout << "  ..updating costs at node " << head->label << endl;
  cout << "Old node info:";
  print_match_info(head);
#endif
  head->hazard_cost = cell_haz_cost;
  head->max_time_cost = cell_max_del;
  head->min_time_cost = cell_min_del;
  head->area_cost = cell_area_cost;
  head->lib_num = lib_index;
#ifdef __VERBOSE_MAT__
  cout << "New node info:";
  print_match_info(head);
#endif
}

// ********************************************************************
// High level routine to optimally cover the matched netlist and write
// .acc files. Entered once per output. Each entry writes a file for
// the covered output (out_vec_indec). It also adds all the nodes for
// that output to the .allnodes_cov.acc file for the entire design.
// ********************************************************************
void cover_and_write_netlist(designADT design, node* input_vec[],
			     int tot_num_inputs, lib_node* lib_vec[],
			     int num_lib_cells, node* output_vec[],
			     int out_vec_index, int region_num,
			     int num_haz_nodes, int haz_node_vec[],
			     int pri_cost, int sec_cost,
			     FILE* fpallcov, int num_nodes_curt) {
  
  static int base = 0;

  // Head of a linked list of cells to be written to the netlist
  netlist_node* head_netlist_node;
  head_netlist_node = make_netlist_node();
  head_netlist_node->first = make_input_node();

  // Make a file for each individual output, exposing all nodes.
  // Called once per output.
  if (cover_write_one_output(design, lib_vec, output_vec,
			     out_vec_index, region_num,
			     head_netlist_node, num_haz_nodes,
			     haz_node_vec, pri_cost, sec_cost,
			     base, fpallcov)) {

    // Make one file that exposes all nodes for all outputs
    // If the output is a wire, the single line is written to the
    // allnodes file in the above routine
    write_netlist_cells(fpallcov, head_netlist_node, lib_vec);
//     free_netlist(head_netlist_node);
  }
  delete head_netlist_node;
  if (num_nodes_curt > 0)
    base = base + num_nodes_curt-1;
}

// ********************************************************************
// Do the covering of the matched decomposition and write the files
// for each output in .acc format. Entered once per output.
// ********************************************************************
bool cover_write_one_output(designADT design, lib_node* lib_vec[],
			    node* output_vec[], int out_vec_index,
			    int region_num, netlist_node* head_netlist_node,
			    int num_haz_nodes, int haz_node_vec[],
			    int pri_cost, int sec_cost, int base,
			    FILE* fpallcov) {
  
  int exp_haz_nodes=0;
  bool not_wire_out;
  char outname[FILENAMESIZE];
  FILE* fp;
  
  // Make output file name
  strcpy(outname,design->filename);
  strcat(outname,"_");
  strcat(outname,design->signals[(region_num-1)/2]->name);
  if (pri_cost == DELAY) {
    if (sec_cost == DELAY)
      strcat(outname,".dd.acc");
    else if (sec_cost == AREA)
      strcat(outname,".da.acc");
    else if (sec_cost == HAZARD)
      strcat(outname,".dh.acc");
  }
  else if (pri_cost == AREA) {
    if (sec_cost == DELAY)
      strcat(outname,".ad.acc");
    else if (sec_cost == AREA)
      strcat(outname,".aa.acc");
    else if (sec_cost == HAZARD)
      strcat(outname,".ah.acc");
  }
  else {
    
    // Primary cost = HAZARD
    if (sec_cost == DELAY)
      strcat(outname,".hd.acc");
    else if (sec_cost == AREA)
      strcat(outname,".ha.acc");
    else if (sec_cost == HAZARD)
      strcat(outname,".hh.acc");
  }
  fp=fopen(outname,"w");
  if (fp==NULL) {
    cout << "ERROR: Unable to open file " << outname << endl;
    fclose(fp);
  }
  fprintf(fp, "# Filename %s\n", outname);
  
  // Write all input lines
  write_acc_input(fp, design);
  
  // Write all output lines except the one under consideration
  write_acc_output(fp, design, output_vec, region_num);
  
  // Do the best covering of the matched netlist
  not_wire_out = cover_netlist_cells(fp, design, output_vec, out_vec_index,
				     lib_vec, head_netlist_node, base,
				     fpallcov);
  if (not_wire_out) {  
    
#ifdef __VERBOSE_COV__
    print_netlist_list(head_netlist_node, lib_vec);
    cout << endl;
#endif

    // Incorporate input names into boolean expressions for .acc files
    munge_exprs(design, head_netlist_node);
    
#ifdef __VERBOSE_COV__
    print_netlist_list(head_netlist_node, lib_vec);
    cout << endl;
#endif
    
    // Write the rest of the .acc file
    exp_haz_nodes = write_netlist_cells(fp, head_netlist_node, lib_vec);
#ifdef __VERBOSE_ACC__
    cout << "Number of exposed hazardous nodes after covering = "
	 << exp_haz_nodes << endl;
#endif
    
    // Write the hazardous nodes to the .acc file
    fprintf(fp,"# All hazardous nodes listed below:\n");
    fprintf(fp,"# ");
    if (num_haz_nodes > 0)
      fprintf(fp, "%i",haz_node_vec[0]+base);
    for (int i=1;i<num_haz_nodes;i++)
      fprintf(fp, ", %i", haz_node_vec[i]+base);
    fprintf(fp,"\n");
  }
#ifdef __VERBOSE_ACC__
  cout << "Storing covered netlist for output "
       << design->signals[(region_num-1)/2]->name << " to: "
       << outname << endl << endl;
#endif
  fclose(fp);
  return not_wire_out;
}

// ********************************************************************
// Write the library cell lines to the netlist file. Recurses from
// output to input through the matched, decomposed netlist.
// ********************************************************************
bool cover_netlist_cells(FILE* fp, designADT design, node* output_vec[],
			 int out_vec_index, lib_node* lib_vec[],
			 netlist_node* head_netlist_node, int base,
			 FILE* fpallcov) {

  int lib_num_inputs;
  bool flag=false;
  bool covered=true;
  bool not_wire_out=true;
  node* decomp_head;
  node* lib_head;
  char* lib_expr;
  char* output;
  char expr[MAXEXPRLEN];
  netlist_node* temp_netlist_node;
  netlist_node* work_netlist_node;
  netlist_node* new_netlist_node_ptr;
  input_node* temp_input_node;

  // Create null expression
  fill_null(expr, MAXEXPRLEN);
  decomp_head = output_vec[out_vec_index]->pred1;
  temp_netlist_node = head_netlist_node;
  new_netlist_node_ptr = head_netlist_node;
#ifdef __VERBOSE_COV__
  cout << endl << "Starting covering for output ... " 
       << decomp_head->succ->label << endl;
#endif
  if ((decomp_head->type == INNEE) ||
      ((decomp_head->type == INV) &&
       (decomp_head->pred1->type == INV) &&
       (decomp_head->pred1->pred1->type == INNEE))) {

    // The output is equal to an input or the output is driven by a pair of
    // inverters tied to an input - write the single line file
    fprintf(fp,"%s\n",decomp_head->succ->expr);
    fprintf(fpallcov,"%s\n",decomp_head->succ->expr);
    not_wire_out = false;
    return not_wire_out;
  }
  else {
    output = design->signals[decomp_head->succ->name]->name;
    if ((lib_vec[decomp_head->lib_num]->area == 0) &&
	(decomp_head->succ->type == OUTTEE)) {
#ifdef __VERBOSE_COV__
      cout << "Eliminating buffer driving the output ..." << endl;
#endif

      // Eliminate buffer by moving decomp_head to the head of the buffer
      decomp_head->pred1->pred1->value = decomp_head->value;
      decomp_head = decomp_head->pred1->pred1;
    }
    lib_num_inputs = lib_vec[decomp_head->lib_num]->num_inputs;
    lib_head = lib_vec[decomp_head->lib_num]->pred;
    lib_expr = lib_vec[decomp_head->lib_num]->expr;
    covered = true;
#ifdef __VERBOSE_COV__
    cout << "Adding netlist node "
	 << lib_vec[decomp_head->lib_num]->name
	 << " to node " << decomp_head->label << endl;
#endif
    push_netlist(decomp_head, new_netlist_node_ptr, lib_expr,
		 covered, output);
    temp_input_node = temp_netlist_node->first;

    // If this is a CEL or gCEL, add the output to the input list
    if (lib_vec[decomp_head->lib_num]->pred->type == CEL) {
#ifdef __READ_EXTERNAL_LIB__
      push_netlist_input(temp_input_node, output, lib_head->input3);
#else
      char* lib_letter = "a";
      push_netlist_input(temp_input_node, output, lib_letter);
#endif
    }
    cover_lib_cell(design, decomp_head, lib_head, lib_vec,
		   temp_netlist_node, lib_num_inputs,
		   head_netlist_node, new_netlist_node_ptr, base,
		   temp_input_node);
    flag = false;

    // Keep working the netlist until all nodes are covered
    work_netlist_node = head_netlist_node;
    while (work_netlist_node->succ != NULL) {
      if (!work_netlist_node->covered) {
	temp_netlist_node = work_netlist_node;
	temp_input_node = temp_netlist_node->first;
	decomp_head = temp_netlist_node->head;
	lib_head = lib_vec[decomp_head->lib_num]->pred;
	lib_num_inputs = lib_vec[decomp_head->lib_num]->num_inputs;
	cover_lib_cell(design, decomp_head, lib_head, lib_vec,
		       temp_netlist_node, lib_num_inputs,
		       head_netlist_node, new_netlist_node_ptr, base,
		       temp_input_node);
	work_netlist_node->covered = true;
	flag = true;	
      }
      if (flag) {
	work_netlist_node = head_netlist_node;
	flag = false;
      }
      else
	work_netlist_node = work_netlist_node->succ;
    }
  }
  return not_wire_out;
}

// ********************************************************************
// Add input to this node's input list.
// ********************************************************************
void add_to_netlist(designADT design, node* head,
		    input_node*& temp_input_node, char name_node[],
		    char* name_pri, char temp[], char* lib_expr,
		    lib_node* lib_vec[], bool& covered,
		    netlist_node*& new_netlist_node_ptr,
		    bool& ignore, int base, char* lib_letter) {
      
  if (head->type == INNEE) {
      
    // This is a primary input
    name_pri = mem_name(design->signals[head->name]->name);
#ifdef __VERBOSE_COV__
    cout << "  Pushing " << name_pri << " on the input stack ..." << endl;
#endif
    push_netlist_input(temp_input_node, name_pri, lib_letter);
  }
  else if (lib_vec[head->lib_num]->area == 0) {
    
    // There is a buffer (inverter pair) driving this node
#ifdef __VERBOSE_COV__
    cout << "Removing buffer ..." << endl;
#endif
    head = head->pred1->pred1;
    if (head->type == INNEE) {
      
      // This is a primary input
      name_pri = mem_name(design->signals[head->name]->name);
#ifdef __VERBOSE_COV__
      cout << "  Pushing " << name_pri << " on the input stack ..." << endl;
#endif
      push_netlist_input(temp_input_node, name_pri, lib_letter);
    }
    else {
      
      // Not a primary input or a buffer
      strcat(name_node,"xx");
      num_to_char(temp,head->counter+base);
      strcat(name_node,temp);
#ifdef __VERBOSE_COV__
      cout << "  Pushing " << name_node << " on the input stack ..." << endl;
#endif
      push_netlist_input(temp_input_node, name_node, lib_letter);
      
      // Add a node to the netlist to be covered later
      lib_expr = lib_vec[head->lib_num]->expr;
      covered = false;
      new_netlist_node_ptr = new_netlist_node_ptr->succ;
#ifdef __VERBOSE_COV__
      cout << "Adding netlist node " << lib_vec[head->lib_num]->name
	   << " to node " << head->label << endl;
#endif
      push_netlist(head, new_netlist_node_ptr, lib_expr, covered,
		   &(name_node[0]));
      ignore = true;
    }
  }
  else {
    
    // Not a primary input at this node or a buffer driving this node
    strcat(name_node,"xx");
    num_to_char(temp,head->counter+base);
    strcat(name_node,temp);
#ifdef __VERBOSE_COV__
    cout << "  Pushing " << name_node << " on the input stack ..." << endl;
#endif
    push_netlist_input(temp_input_node, name_node, lib_letter);
    
    // Add a node to the netlist to be covered later
    lib_expr = lib_vec[head->lib_num]->expr;
    covered = false;
    new_netlist_node_ptr = new_netlist_node_ptr->succ;
#ifdef __VERBOSE_COV__
    cout << "Adding netlist node " << lib_vec[head->lib_num]->name
	 << " to node " << head->label << endl;
#endif
    push_netlist(head, new_netlist_node_ptr, lib_expr, covered,
		 &(name_node[0]));
    ignore = true;
  }
}

// ********************************************************************
// Find the inputs to this library cell and write the cell line to the
// netlist file.
// ********************************************************************
bool cover_lib_cell(designADT design, node* decomp_head, node*& lib_head,
		    lib_node* lib_vec[], netlist_node*& temp_netlist_node,
		    int lib_num_inputs, netlist_node* head_netlist_node,
		    netlist_node*& new_netlist_node_ptr, int base,
		    input_node*& temp_input_node) {

  int num_inputs_cov=0;
  bool covered=true;
  bool pred1_flag = false;
  bool ignore=false;
  char* lib_expr;
  char* name_pri;
  char name_node[MAXINPLITS];
  char temp[7];
  
  // Create null expression
  fill_null(name_node, MAXINPLITS);
  if (lib_head->pred1 != NULL) {
    pred1_flag = true;
    cover_lib_cell_recurse(design, decomp_head, pred1_flag, lib_vec,
			   temp_input_node, temp_netlist_node,
			   num_inputs_cov, lib_num_inputs, lib_head,
			   head_netlist_node, new_netlist_node_ptr, base);
  }
  else {
    
    // We are at an input to the library cell so add it to the input list
    add_to_netlist(design, decomp_head->pred1, temp_input_node, name_node,
		   name_pri, temp, lib_expr, lib_vec, covered,
		   new_netlist_node_ptr, ignore, base, lib_head->input1);
    num_inputs_cov++;
    if (num_inputs_cov == lib_num_inputs)
      return ignore;
  }
  
  if (lib_head->pred2 != NULL) {
    pred1_flag = false;
    cover_lib_cell_recurse(design, decomp_head, pred1_flag, lib_vec,
			   temp_input_node, temp_netlist_node,
			   num_inputs_cov, lib_num_inputs, lib_head,
			   head_netlist_node, new_netlist_node_ptr, base);
    return ignore;
  }
  else {
    
    // Create null expression
    fill_null(name_node, MAXINPLITS);
    if (decomp_head->pred2 != NULL) {
      add_to_netlist(design, decomp_head->pred2, temp_input_node, name_node,
		     name_pri, temp, lib_expr, lib_vec, covered,
		     new_netlist_node_ptr, ignore, base, lib_head->input2);
    }
    num_inputs_cov++;
  }
  return ignore;
}

// ********************************************************************
// Find the lib cell closest to a primary input, recursing down the
// pred1 link.
// ********************************************************************
bool cover_lib_cell_recurse(designADT design, node* decomp_head,
			    bool pred1_flag, lib_node* lib_vec[],
			    input_node*& temp_input_node,
			    netlist_node*& temp_netlist_node,
			    int& num_inputs_cov, int lib_num_inputs,
			    node* lib_head, netlist_node* head_netlist_node,
			    netlist_node*& new_netlist_node_ptr, int base) {

  bool covered=true;
  bool more_to_do=false;
  bool nothing=false;
  char* lib_expr;
  char* name_pri=NULL;
  char name_node[MAXINPLITS];
  char temp[7];

  // Create null expression
  fill_null(name_node, MAXINPLITS);

  if (pred1_flag) {
    if (lib_head->pred1 != NULL) {
      pred1_flag = true;
      lib_head = lib_head->pred1;
      decomp_head = decomp_head->pred1;
      nothing = cover_lib_cell_recurse(design, decomp_head, pred1_flag,
				       lib_vec, temp_input_node,
				       temp_netlist_node, num_inputs_cov,
				       lib_num_inputs, lib_head,
				       head_netlist_node,
				       new_netlist_node_ptr, base);

      // Need to pop up one level in the library cell for the first gate
      if ((lib_head->pred1 == NULL) || (lib_head->type == INV))
	return more_to_do;
    }
    else {
      add_to_netlist(design, decomp_head->pred1, temp_input_node,
		     name_node, name_pri, temp, lib_expr, lib_vec,
		     covered, new_netlist_node_ptr, more_to_do,
		     base, lib_head->input1);
      num_inputs_cov++;
      if (num_inputs_cov == lib_num_inputs)
	return more_to_do;
    }
  }
  
  if (lib_head->pred2 != NULL) {
    pred1_flag = true;
    lib_head = lib_head->pred2;
    decomp_head = decomp_head->pred2;
    cover_lib_cell_recurse(design, decomp_head, pred1_flag, lib_vec,
			   temp_input_node, temp_netlist_node,
			   num_inputs_cov, lib_num_inputs, lib_head,
			   head_netlist_node, new_netlist_node_ptr, base);
    
    // Need to pop up one level in the library cell for the first gate
    if ((lib_head->pred2 == NULL) || (lib_head->type == INV))
      return more_to_do;
  }
  else {
    if (lib_head->type == INV)
      return false;

    // Create null expression
    fill_null(name_node, MAXINPLITS);

    // We are at an input to the library cell so add it to the input list
    if (decomp_head->pred2 != NULL) {
      add_to_netlist(design, decomp_head->pred2, temp_input_node, name_node,
		     name_pri, temp, lib_expr, lib_vec, covered,
		     new_netlist_node_ptr, more_to_do, base,
		     lib_head->input2);
    }
    num_inputs_cov++;
  }
  return more_to_do;
}

// ********************************************************************
// Replace the expr field in the netlist linked list with the actual
// inputs. Write the new expression to the expr_acc field.
// ********************************************************************
void munge_exprs(designADT design, netlist_node* head_netlist_node) {
  
  netlist_node* net_node = head_netlist_node;
  input_node* input;
  char* temp;
  char expr_acc[MAXEXPRLEN];
  int i=0;
  int length = 0;
  bool equal = false;
  
#ifdef __VERBOSE_COV__
  cout << "Replacing library expressions with node names..." << endl;
#endif
  while (net_node->succ != NULL) {
    
    // Create null expression
    fill_null(expr_acc, MAXEXPRLEN);
    temp = net_node->expr;
    while (net_node->expr[i] != (char)NULL) {
      
      // OR symbol transformation
      if (net_node->expr[i] == '+')
	net_node->expr[i] = '|';
      // AND symbol transformation
      if (net_node->expr[i] == '*')
	net_node->expr[i] = '&';
      // NEGATE symbol transformation
      if (net_node->expr[i] == '!')
	net_node->expr[i] = '~';
      if ((net_node->expr[i] == '(') ||
	  (net_node->expr[i] == '|') ||
	  (net_node->expr[i] == '&') ||
	  (net_node->expr[i] == '~') ||
	  (net_node->expr[i] == ')')) {
	
	// Pass all characters that are not letters to new string
	strncat(expr_acc, temp, 1);
	i++;
	length=0;
      }
      else {
	
	// Replace letters with input names
	input = net_node->first;
#ifdef __VERBOSE_COV__
	cout << "input->name = " << input->name
	     << "  input->lib_char = " << input->lib_char
	     << "  net_node->expr = " << net_node->expr
	     << "  net_node->expr[i] = " << net_node->expr[i] << endl;
#endif
	equal = false;
	while (!equal) {
	  length = strlen(input->lib_char);
	  equal = true;
	  for (int j=0;j<length;j++) {
	    if (input->lib_char[j] != net_node->expr[i+j]) {
	      equal = false;
	      input = input->succ;
// 	      while (input->counter == 9999)
// 		input = input->succ;
	      break;
	    }
	  }
	}
	
	// Mark this input as done
	input->counter = 9999;
	strcat(expr_acc, input->name);
	
#ifdef __VERBOSE_COV__
	cout << "Expression_acc = " << expr_acc << endl;
#endif
      }
      i=i+length;
      
      // Advance pointer to next place in expr
      temp = &(net_node->expr[i]);
    }
    i = 0;
    strcpy(net_node->expr_acc,expr_acc);
    net_node = net_node->succ;
  }
}

// ********************************************************************
// Write the library cell lines to the netlist.
// ********************************************************************
int write_netlist_cells(FILE* fp, netlist_node* net_node,
			lib_node* lib_vec[]) {
  
  int exp_haz_nodes = 0;
  char temp[7];
  char expr[MAXEXPRLEN];

  // Advance to end of netlist. The nodes must be written backwards since
  // the covering is done from output to input.
  while (net_node->succ != NULL)
    net_node = net_node->succ;
  do {
    net_node = net_node->pred;
    fill_null(expr, MAXEXPRLEN);
    if (net_node->head->ahazard || net_node->head->mhazard)
      exp_haz_nodes++;
    strcat(expr,net_node->output);
    strcat(expr," = ");
    if (net_node->init_value)
      strcat(expr,"1 = [ ");
    else
      strcat(expr,"0 = [ ");
    num_to_char(temp,lib_vec[net_node->lib_num]->min_del);
      strcat(expr,temp);
    strcat(expr," , ");
    num_to_char(temp,lib_vec[net_node->lib_num]->max_del);
      strcat(expr,temp);
    strcat(expr," ] ");
    strcat(expr,net_node->expr_acc);
    fprintf(fp,"%s\n",expr);
  }
  while (net_node->pred != NULL);
  return exp_haz_nodes;
}

