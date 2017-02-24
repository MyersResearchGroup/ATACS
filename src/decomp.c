// *********************************************************************
// Tech mapping code called from the main ATACS program with
// the -eD flag on the command line or using the command (decomp)
// in interactive mode.
// *********************************************************************

#include "decomp.h"

void decomp(designADT design) {

  // Linked list of outputs for entire design
  node* output_vec[design->nsignals - design->ninpsig];
  
  // Linked list of inputs for entire design
  node* input_vec[INPUT_VEC_SIZE];  
  
  // Contains one entry for each library cell
  lib_node* lib_vec[MAXNUMLIBCELLS];
  int num_lib_cells=0;        // Number of cells in the cell library
  
  decomp(design, output_vec, input_vec, lib_vec, num_lib_cells);
}


//This function decomps the design.  Also allows access to the decomped structure
//after mapping
void decomp(designADT design, node* output_vec[], node* input_vec[], lib_node* lib_vec[], int &num_lib_cells) {
  cout << "Starting technology mapping ...." << endl;
  int inp_vec_index=0;        // Index to input_vec
  int out_vec_index=0;        // Index to output_vec
  int num_set_covers=0;       // Number of covers in set region
  int num_reset_covers=0;     // Number of covers in reset region
  int combo = NOT_COMBO;      // Number of covers if combinational output
  int num_set_inputs=0;       // Number of inputs in set portion of input_vec
  int num_reset_inputs=0;     // Number of inputs in reset portion of input_vec
  int num_combo_inputs=0;     // Number of inputs in combo portion of input_vec
  int tot_num_inputs=0;       // Total number of inputs in decomposition
  
  bool rules_delay = true;
#ifdef __MAKE_ACC_FILES__
  FILE* fpall;
  FILE* fpallcov;
  char outname[FILENAMESIZE];
  char outnamecov[FILENAMESIZE];
#endif

  // Pointers to the head of a linked list of covers for one output
  cover_node* head_set_node;
  cover_node* head_reset_node;


  // Initialize the decomposition structures
  head_set_node = make_cover_node();
  head_reset_node = make_cover_node();
#ifdef __VERBOSE_STARTUP__
  print_signals_list(design);
  print_events_data_struct(design);
#endif
  // Changes or additions to the decomposed library go in this function
  build_library(lib_vec, num_lib_cells);

  // Build the expressions for all outputs for use in .acc files
  build_acc_out_exps(design, output_vec, rules_delay);
  
#ifdef __MAKE_ACC_FILES__
  
  // Setup the .acc file creation for writing all nodes for all outputs
  // for both covered and uncovered netlists. First, uncovered.
  strcpy(outname,design->filename);
  strcat(outname,"_allnodes.acc");
  fpall = fopen(outname,"w");
  if (fpall == NULL)
    cout << "ERROR: Unable to open file " << outname << endl;
  fprintf(fpall, "# Filename %s\n", outname);
  
  // Write all input lines
  write_acc_input(fpall, design);

  // Now, for a covered netlist.
  strcpy(outnamecov,design->filename);
  strcat(outnamecov,"_allnodes_cov.acc");
  fpallcov = fopen(outnamecov,"w");
  if (fpallcov == NULL)
    cout << "ERROR: Unable to open file " << outnamecov << endl;
  fprintf(fpallcov, "# Filename %s\n", outnamecov);
  write_acc_input(fpallcov, design);
#endif

  // The for loop goes through all non-input signals
  // The regions array is organized as follows:
  // unused,inp1+,inp1-,inp2+,inp2-, ... out1+,out1-, ... sv0+,sv0-
  // Does two regions at a time, one for set and one for reset
  for (int region_num=2*design->ninpsig+1;
       region_num<(2*design->nsignals+1);
       region_num=region_num+2) {
    
    // Head to the main loop of the decomposition software
    decomp_main_loop(design, region_num, num_set_covers,
		     num_reset_covers, head_set_node,
		     head_reset_node, input_vec, inp_vec_index, 
		     num_set_inputs, num_reset_inputs, num_combo_inputs,
		     tot_num_inputs, output_vec, out_vec_index,
		     num_lib_cells, combo, lib_vec, fpall, fpallcov);

    // Free the decomposition list for this output
    // Keep the input vector and output vectors intact
    // free_decomp(input_vec, tot_num_inputs);
    num_set_covers = 0;
    num_reset_covers = 0;
    num_set_inputs = 0;
    num_reset_inputs = 0;
    num_combo_inputs = 0;
    inp_vec_index = 0;
    out_vec_index++;
  }
  // cout << endl;

#ifdef __MAKE_ACC_FILES__

  // Close the two files that write netlists containing all nodes for
  // all outputs.
  fclose(fpall);
  fclose(fpallcov);
  
  // Make a complex gate equivalent file that has only input lines and
  // primary output lines and contains the circuit min/max
  make_cge_file_acc(design, output_vec);
#endif
#ifdef __VERBOSE_ACC__
  cout << "Storing netlist " << outname << endl;
  cout << "Storing netlist " << outnamecov << endl;
#endif
  cout << "Technology mapped files written" << endl;
}

// ********************************************************************
// Main decomposition loop. Entered once per output.
// ********************************************************************
void decomp_main_loop(designADT design, int region_num, int& num_set_covers,
		      int& num_reset_covers, cover_node*& head_set_node,
		      cover_node*& head_reset_node, node* input_vec[],
		      int& inp_vec_index, int& num_set_inputs,
		      int& num_reset_inputs, int& num_combo_inputs,
		      int& tot_num_inputs, node* output_vec[],
		      int out_vec_index, int num_lib_cells, int& combo,
		      lib_node* lib_vec[], FILE* fpall, FILE* fpallcov) {

  int num_nodes_curt=0;
  nodelistADT head_nodelist = NULL;

  // Build two linked lists of the covers (AND terms) for this output
  build_linked_lists(design, region_num, num_set_covers,
		     num_reset_covers, head_set_node,
		     head_reset_node);

  // Check for a combinational output
  combo = check_combo_output(num_set_covers, num_reset_covers);
  
  // Do the decomposition for this output
  dothe_decomp(design, num_set_covers, num_reset_covers, head_set_node,
	       head_reset_node, input_vec, inp_vec_index, combo,
	       num_set_inputs, num_reset_inputs, num_combo_inputs,
	       output_vec, out_vec_index);

  // Free the cover node chains
  // free_cover_nodes(head_set_node, head_reset_node);

  // Levelize the lists
  tot_num_inputs = levelize(input_vec, num_set_inputs,
			    num_reset_inputs, num_combo_inputs);

  // Create a link list of all the nodes
  head_nodelist = make_node_list(design, input_vec, tot_num_inputs,
				 num_nodes_curt);

  // Find the absolute min and max delays from any input to all nodes
  // in the decomposition.
  find_abs_minmax(input_vec, tot_num_inputs);

#ifdef __VERBOSE_STARTUP__
  print_input_vec(design, input_vec, tot_num_inputs, combo);
  print_decomp(design, input_vec, tot_num_inputs, combo);
  print_output_vec(design, output_vec);
  print_nodelist(head_nodelist);
#endif
  
#ifdef  __DO_ALL_INPUT_PERMUTES__
  do_all_input_permutes(design, input_vec, tot_num_inputs,
			inp_vec_index, combo, lib_vec,
			num_lib_cells, output_vec,
			out_vec_index, region_num,
			num_set_covers, num_reset_covers,
			head_set_node, head_reset_node,
			head_nodelist, num_nodes_curt, fpall, fpallcov);
#else
  
  // Just do one combination of inputs
  do_one_input_combo(design, input_vec, tot_num_inputs,
		     inp_vec_index, combo, lib_vec,
		     num_lib_cells, output_vec,
		     out_vec_index, region_num,
		     head_nodelist, num_nodes_curt, fpall, fpallcov);
#endif
}

// ********************************************************************
// Does all input permutations for this output. Entered once per output.
// ********************************************************************
void do_all_input_permutes(designADT design, node* input_vec[],
			   int tot_num_inputs, int inp_vec_index,
			   int combo, lib_node* lib_vec[],
			   int num_lib_cells, node* output_vec[],
			   int out_vec_index, int region_num,
			   int num_set_covers, int num_reset_covers,
			   cover_node* head_set_node,
			   cover_node* head_reset_node,
			   nodelistADT head_nodelist,
			   int num_nodes_curt, FILE* fpall,
			   FILE* fpallcov) {

  int new_input_index=0;    // Used to index into the input_vec
  cover_node* head=head_set_node;

#ifdef __VERBOSE_STARTUP__
  print_input_vec(design, input_vec, tot_num_inputs, combo);
  cout << "Set ";
  print_cover_list(num_set_covers, head_set_node);
  cout << "Reset ";
  print_cover_list(num_reset_covers, head_reset_node);
  cout << endl;
#endif

  // Go through all the set covers
  while (head->cover != NULL) {

    // Start the input shuffling
    do_input_shuffle(design, input_vec, tot_num_inputs,
		     inp_vec_index, combo, lib_vec,
		     num_lib_cells, output_vec,
		     out_vec_index, region_num,
		     head, new_input_index, head_nodelist,
		     num_nodes_curt, fpall, fpallcov);
    new_input_index = new_input_index + head->num_lits;
    head = head->succ;
  }
  
  // Now go through all the reset covers
  head = head_reset_node;
  while (head->cover != NULL) {
    
    // Start the input shuffling
    do_input_shuffle(design, input_vec, tot_num_inputs,
		     inp_vec_index, combo, lib_vec,
		     num_lib_cells, output_vec,
		     out_vec_index, region_num,
		     head, new_input_index, head_nodelist,
		     num_nodes_curt, fpall, fpallcov);
    new_input_index = new_input_index+head->num_lits;
    head = head->succ;
  }
}

// ********************************************************************
// Starts the input shuffling for this one cover term.
// ********************************************************************
void do_input_shuffle(designADT design, node* input_vec[],
		      int tot_num_inputs, int inp_vec_index,
		      int combo, lib_node* lib_vec[],
		      int num_lib_cells, node* output_vec[],
		      int out_vec_index, int region_num,
		      cover_node* head, int new_input_index,
		      nodelistADT head_nodelist,
		      int num_nodes_curt, FILE* fpall, FILE* fpallcov) {
  
  int j, r, s;
  int iteration=0;
  int p[head->num_lits+2];
  node* new_inp_vec[head->num_lits];

  print_decomp(design, input_vec, tot_num_inputs, combo);
  print_input_vec(design, input_vec, tot_num_inputs, combo);

  // Create copy of input vector successors
  create_input_copy(input_vec, new_inp_vec, new_input_index, head->num_lits);
  print_new_inp_vec(design, new_inp_vec, tot_num_inputs, combo);

  for (j=0;j<=head->num_lits+1;j++) {
    p[j]=j;
  }
  int i=0;
  while (i >= 0) {

    // Copy the new names to the input vector
    copy_to_inp_vec(input_vec, new_inp_vec, new_input_index,
		    head->num_lits, p);
    cout << "iteration number = " << iteration << endl;
    iteration++;

    // Clear the visited fields
    clear_visited_decomp(input_vec, tot_num_inputs);

#ifdef __VERBOSE_PERMUTE__
    cout << "Permute vector = ";
    for (int k=0;k<head->num_lits;k++)
      cout << p[k] << " ";
    cout << endl;
    print_decomp(design, input_vec, tot_num_inputs, combo);
    print_new_inp_vec(design, new_inp_vec, tot_num_inputs, combo);
#endif

    // Go and do the hazard check
    do_one_input_combo(design, input_vec, tot_num_inputs,
		       inp_vec_index, combo, lib_vec,
		       num_lib_cells, output_vec,
		       out_vec_index, region_num,
		       head_nodelist, num_nodes_curt, fpall, fpallcov);
    
    i=head->num_lits-2;
    while (p[i]>p[i+1])
      i--;
    j=head->num_lits-1;
    while (p[i]>p[j])
      j--;
    swap(p[i], p[j]);
    r=head->num_lits-1;
    s=i+1;
    while(r>s) {
      swap(p[r], p[s]);
      r--;
      s++;
    }
  }
}

// ********************************************************************
// Copies the temporary vectors back to the input vector.
// ********************************************************************
void copy_to_inp_vec(node* input_vec[], node* new_inp_vec[],
		     int new_input_index, int num_lits, int p[]) {


  for (int i=0;i<num_lits;i++) {
    if (input_vec[new_input_index+i]->succ->type == INV)
      input_vec[new_input_index+i]->succ = new_inp_vec[p[i]];
    else
      input_vec[new_input_index+i] = new_inp_vec[p[i]];
  }
}

// ********************************************************************
// Copies where the input node is pointing to, to new_inp_vec. If there
// is an INV attached to the INNEE, copy where the INV is pointing to.
// ********************************************************************
void create_input_copy(node* input_vec[], node* new_inp_vec[],
		       int new_input_index, int num_lits) {

  for (int i=0;i<num_lits;i++) {
    if (input_vec[new_input_index+i]->succ->type == INV)
      new_inp_vec[i] = input_vec[new_input_index+i]->succ;
    else
      new_inp_vec[i] = input_vec[new_input_index+i];
  }
}

// ********************************************************************
// Swaps 2 index numbers.
// ********************************************************************
void swap(int& m, int&n) {

  int temp=m;

  m=n;
  n=temp;
}

// ********************************************************************
// Checks one combination of inputs.
// ********************************************************************
void do_one_input_combo(designADT design, node* input_vec[],
			int tot_num_inputs, int inp_vec_index,
			int combo, lib_node* lib_vec[],
			int num_lib_cells, node* output_vec[],
			int out_vec_index, int region_num,
			nodelistADT head_nodelist, int num_nodes_curt,
			FILE* fpall, FILE* fpallcov) {
  
  int num_unique_haz_nodes=0;
  int haz_node_vec[num_nodes_curt];
  bool general = false;

  // Definitions for cost parameters
  // DELAY = 0, AREA = 1, HAZARD = 2
  int pri_cost = HAZARD;
  int sec_cost = AREA;
//   int pri_cost = HAZARD;
//   int sec_cost = DELAY;
//   int pri_cost = AREA;
//   int sec_cost = HAZARD;
//   int pri_cost = DELAY;
//   int sec_cost = HAZARD;

  // Do the hazard check
  if (num_nodes_curt > 0)
    hazard_check(design, input_vec, tot_num_inputs, region_num,
		 num_unique_haz_nodes, haz_node_vec, head_nodelist,
		 num_nodes_curt, general);
    
  // Create .acc files for this output, for each node in this output,
  // and add this output's nodes to the allnodes file
#ifdef __MAKE_ACC_FILES__
  make_acc_files(design, input_vec, region_num, tot_num_inputs,
		 output_vec, fpall, num_nodes_curt);
#endif

#ifdef __MAKE_ALL_ACC_COST_FILES__
  // Write files for all 9 combinations of cost functions
  do_all_cost_files(design, input_vec, tot_num_inputs,
		    inp_vec_index, combo, lib_vec,
		    num_lib_cells, output_vec,
		    out_vec_index, region_num,
		    num_unique_haz_nodes, haz_node_vec, head_nodelist,
		    pri_cost, sec_cost, fpallcov, num_nodes_curt);
#else
  do_one_cost_file(design, input_vec, tot_num_inputs,
		   inp_vec_index, combo, lib_vec,
		   num_lib_cells, output_vec,
		   out_vec_index, region_num,
		   num_unique_haz_nodes, haz_node_vec, head_nodelist,
		   pri_cost, sec_cost, fpallcov, num_nodes_curt);
#endif
}

// ********************************************************************
// Write covered files for all 9 combinations of cost functions.
// ********************************************************************
void do_all_cost_files(designADT design, node* input_vec[],
		       int tot_num_inputs, int inp_vec_index,
		       int combo, lib_node* lib_vec[],
		       int num_lib_cells, node* output_vec[],
		       int out_vec_index, int region_num,
		       int num_unique_haz_nodes, int haz_node_vec[],
		       nodelistADT head_nodelist, int pri_cost,
		       int sec_cost, FILE* fpallcov,
		       int num_nodes_curt) {

  for (int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {
      pri_cost = i;
      sec_cost = j;
      clear_matching_decomp(input_vec, tot_num_inputs);
      do_one_cost_file(design, input_vec, tot_num_inputs,
		       inp_vec_index, combo, lib_vec,
		       num_lib_cells, output_vec,
		       out_vec_index, region_num,
		       num_unique_haz_nodes, haz_node_vec, head_nodelist,
		       pri_cost, sec_cost, fpallcov, num_nodes_curt);
    }
  }
}

// ********************************************************************
// Write covered file for one of the 9 combinations of cost functions.
// ********************************************************************
void do_one_cost_file(designADT design, node* input_vec[],
		      int tot_num_inputs, int inp_vec_index,
		      int combo, lib_node* lib_vec[],
		      int num_lib_cells, node* output_vec[],
		      int out_vec_index, int region_num,
		      int num_unique_haz_nodes, int haz_node_vec[],
		      nodelistADT head_nodelist,
		      int pri_cost, int sec_cost, FILE* fpallcov,
		      int num_nodes_curt) {

  // Do the matching of the library to the decomposition.
  // Selects the best library cell cover at each node for the
  // designated cost functions.

  match_decomp(design, input_vec, tot_num_inputs, lib_vec,
	       num_lib_cells, pri_cost, sec_cost, combo,
	       head_nodelist);

  // Clear the visited fields in the decomposed netlist
  clear_acc_visit(input_vec, tot_num_inputs);
  clear_visited_decomp(input_vec, tot_num_inputs);

  // Evaluate initial values on all nodes and place in value field
  eval_acc_inits(design, input_vec, tot_num_inputs);

  //  Do the covering and write the netlist for each output
  cover_and_write_netlist(design, input_vec, tot_num_inputs,
			  lib_vec, num_lib_cells, output_vec,
			  out_vec_index, region_num,
			  num_unique_haz_nodes, haz_node_vec, pri_cost,
			  sec_cost, fpallcov, num_nodes_curt);
}

// ********************************************************************
// See if the actual output timings are within bounds of those in the
// rules matrix.
// ********************************************************************
void do_io_time_check_gen(designADT design, nodelistADT nodelist) {

  for (nodelistADT curnode=nodelist;(curnode && curnode->this_node);
       curnode=curnode->link) {
    if ((curnode->this_node->type == IO) &&
	(curnode->this_node->preds != NULL)) {
#ifdef __VERBOSE_STARTUP__
      cout << curnode->this_node->label << " min = "
	   << curnode->this_node->min << " max = "
	   << curnode->this_node->max << endl;
#endif
      for (int i=design->signals[curnode->this_node->name]->event;
	   ((i<design->nevents) &&
	    (design->events[i]->signal == curnode->this_node->name));
	   i++) {
	for (int j=0;j<design->nevents;j++) {
	  if (design->rules[j][i]->ruletype != NORULE) {
	    if (design->rules[j][i]->lower > curnode->this_node->min)
	      cout << "Minimum time violation:  Circuit = "
		   << curnode->this_node->min
		   << "  Rules = " << design->rules[j][i]->lower
		   << "  Event = " << design->events[i]->event
		   << endl;
	    if (design->rules[j][i]->upper < curnode->this_node->max)
	      cout << "Maximum time violation:  Circuit = "
		   << curnode->this_node->max
		   << "  Rules = " << design->rules[j][i]->upper
		   << "  Event = " << design->events[i]->event
		   << endl;
	  }
	}
      }   
    }  
  }
}

// ********************************************************************
// High level routine for checking nodes for hazards. Entered once
// per output.
// ********************************************************************
void hazard_check(designADT design, node* input_vec[],
		  int tot_num_inputs, int region_num,
		  int& num_unique_haz_nodes, int haz_node_vec[],
		  nodelistADT nodelist, int num_nodes,
		  bool general) {

  int num_states=0;
  int num_edges=0;
  sg_node* head_sg_node;
  sg_node* temp_sg_node;
  char* output_name;

  // For memory usage monitering
#ifdef __DOMEM__
  struct mallinfo memuse;
  memuse=mallinfo();
  printf("memory: max=%d inuse=%d free=%d \n",
	 memuse.arena,memuse.uordblks,memuse.fordblks);
  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
	  memuse.arena,memuse.uordblks,memuse.fordblks);
#endif

  if (general) 
    do_io_time_check_gen(design, nodelist);
#ifdef __VERBOSE_STARTUP__
  if (general) {
    print_general_node_delays(nodelist, input_vec, tot_num_inputs);
  }
  else
    print_decomp_delays(design, input_vec, tot_num_inputs, region_num);
#endif
  
  // Find the number of states and edges in the state graph.
  find_num_state_edge(design, num_states, num_edges, num_nodes);

  // Allocate memory for the color and eval vectors.
  alloc_color_eval_mem(design, num_nodes);

  // Create the color vector for all states and edges.
  // Create the eval vector for all states.
  create_color_eval_vec(design, nodelist, input_vec, tot_num_inputs,
			general);

  // Find start state.
  stateADT start_state = find_start_state(design);
#ifdef __VERBOSE_STARTUP__
  cout << "Start state = " << start_state->state << endl;
#endif

  // Create a temporary node for use as a pointer in the state
  // graph linked list which contains a list of stabilized edges
  head_sg_node = make_sg_node();
  head_sg_node->succ = make_sg_node();
  head_sg_node->succ->pred = head_sg_node;
  head_sg_node->succ->counter = (head_sg_node->counter)+1;
  temp_sg_node = head_sg_node->succ;

  // Stabilize edges and propagate
  stab_and_prop_hl(design, nodelist, temp_sg_node, input_vec,
		   tot_num_inputs, head_sg_node, start_state,
		   region_num, general, num_nodes, num_states);

  // Free the sg node chain
  free_sg_node(head_sg_node);
  
  // High level routine to check for errors
  if (general)
    output_name = "(general)";
  else
    output_name = design->signals[(region_num-1)/2]->name;
  final_check(design, num_unique_haz_nodes, haz_node_vec, num_nodes,
	      nodelist, general, output_name);

  // For memory usage monitoring
#ifdef __DOMEM__
  memuse=mallinfo();
  printf("memory: max=%d inuse=%d free=%d \n",
	 memuse.arena,memuse.uordblks,memuse.fordblks);
  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
	  memuse.arena,memuse.uordblks,memuse.fordblks);
#endif

}

// ********************************************************************
// Does both untimed and timed edge stabilization, and propogates
// the coloring information throughout the state graph. Upon return,
// a list of all stabilized edges is contained in the sg linked list.
// ********************************************************************
void stab_and_prop_hl(designADT design, nodelistADT nodelist,
		      sg_node*& temp_sg_node, node* input_vec[],
		      int tot_num_inputs, sg_node* head_sg_node,
		      stateADT start_state, int region_num,
		      bool general, int num_nodes, int num_states) {
  
  int depth=0;
  int paths=0;
  int sucs=0;
  int states_vis=0;

  // Print the number of states and edges
  //#ifdef __VERBOSE_DEPTH__
  print_states_edges(design);
  //#endif
  
  // Find edges that can be stabilized timing-wise and add
  // them to the linked list used during propagation.
  stab_prop_timed(design, nodelist, head_sg_node, temp_sg_node,
		  num_nodes, num_states, input_vec, general,
		  depth, paths, sucs, states_vis);

  //#ifdef __VERBOSE_DEPTH__
  cout << "Depth tot=" << depth 
       << "  Num paths=" << paths
       << "  Numb sucs=" << sucs
       << "  States vis=" << states_vis << endl;
  //#endif

#ifdef __VERBOSE_TIMED__
  cout << "State colorings after timed propagation ..." << endl;
  print_states(design);
#endif
  
  // Find edges that can be stabilized untimed and iterate
  // until no more changes occur
  temp_sg_node = head_sg_node->succ;
  free_sg_node(temp_sg_node->succ);
  temp_sg_node->succ = NULL;
  stab_prop_untimed(design, nodelist, start_state, input_vec,
		    tot_num_inputs, region_num, head_sg_node,
		    temp_sg_node, general, num_nodes, num_states);
#ifdef __VERBOSE_UNTIMED__
  cout << "State colorings after untimed propagation ..." << endl;
  print_states(design);
#endif
}

// ********************************************************************
// Find edges that can be stabilized timing-wise and add
// them to the linked list used during propagation.
// ********************************************************************
void stab_prop_timed(designADT design, nodelistADT nodelist,
		     sg_node* head_sg_node, sg_node*& temp_sg_node,
		     int num_nodes, int num_states, node* input_vec[],
		     bool general, int& depth, int& paths,
		     int& sucs, int& states_vis) {
  
  bool modified = true;
  int depth_tot=0;
  int num_paths=0;
  int num_sucs=0;
  int num_states_vis=0;


  if (design->bap) {
    for (nodelistADT curnode=nodelist;(curnode && curnode->this_node);
	 curnode=curnode->link) {
      if (curnode->this_node->level != 0) {
#ifdef __VERBOSE_TIMED__
	cout << "Starting timed edge stabilization for node ... "
	     << curnode->this_node->label << endl;
#endif
	stabilize_edge_timed(design, temp_sg_node, curnode->this_node,
			     num_states, input_vec, general, depth_tot,
			     num_paths, num_sucs, num_states_vis);

	depth = depth + depth_tot;
	paths = paths + num_paths;
	sucs = sucs + num_sucs;
	states_vis = states_vis + num_states_vis;

#ifdef __VERBOSE_DEPTH__
	cout << endl << "After node " << curnode->this_node->label
	     << " is complete" << endl;
	cout << "Depth tot=" << depth_tot 
	     << "  Num paths=" << num_paths
	     << "  Numb sucs=" << num_sucs
	     << "  States vis=" << num_states_vis << endl;
#endif

	depth_tot = 0;
	num_paths = 0;
	num_sucs = 0;
	num_states_vis = 0;

#ifdef __VERBOSE_TIMED__
	print_sg_edges(design, head_sg_node->succ);
#endif
	int i = 1;
	modified = true;
#ifdef __VERBOSE_TIMED__
	cout << "After timed edge stabilization and before propagation "
	     << "for node " << curnode->this_node->label << endl;
	find_num_colors(design, num_nodes);
	print_states_col(design, curnode->this_node->counter);
#endif
	
	// Propagate the coloring information until it stabilizes
	while (modified) {
	  modified = false;
	  modified = propagate_colors(design, head_sg_node, num_states);
#ifdef __VERBOSE_TIMED__
	  cout << "After propagating " << i
	       << " times..." << endl;
	  find_num_colors(design, num_nodes);
	  print_states_col(design, curnode->this_node->counter);
#endif
	  i++;
	}
      }
    }
    temp_sg_node = head_sg_node->succ;
    if (temp_sg_node->succ != NULL) {
      free_sg_node(temp_sg_node->succ);
      temp_sg_node->succ = NULL;
    }
  }
}

// ********************************************************************
// Find edges that can be stabilized in an untimed (speed-independent)
// fasion and add them to the linked list used during propagation.
// ********************************************************************
void stab_prop_untimed(designADT design, nodelistADT nodelist,
		       stateADT start_state, node* input_vec[],
		       int tot_num_inputs, int region_num,
		       sg_node* head_sg_node, sg_node*& temp_sg_node,
		       bool general, int num_nodes, int num_states) {

  int i = 0;
  int k = 0;
  bool modified = true;
  bool modified_prop = true;
  bool modified_hl = true;
  bool va[num_states];

  while (modified_hl) {
    i++;
#ifdef __VERBOSE_UNTIMED__
    cout << "Total untimed circuit propagation =" << i
	 << "   Modified_hl = " << modified_hl  << endl;
#endif
    modified_hl = false;
    k++;
    for (nodelistADT curnode=nodelist;(curnode && curnode->this_node);
	 curnode=curnode->link) {
      if (curnode->this_node->level != 0) {
	modified = true;
	while (modified) {
	  modified = false;
	  clear_va(va, num_states);
#ifdef __VERBOSE_UNTIMED__
	  cout << "Starting untimed edge stabilization for node ... "
	       << curnode->this_node->label << endl;
#endif
	  high_level_si(design, start_state, input_vec,
			curnode->this_node, tot_num_inputs,
			region_num, temp_sg_node, general, va);
	  if (head_sg_node->succ->succ != NULL) {
	    modified = true;
	    modified_hl = true;
	  }
#ifdef __VERBOSE_UNTIMED__
	  print_sg_edges(design, head_sg_node->succ);
#endif
	  int j = 0;
	  
	  // Propagate the coloring information throughout the state graph
	  modified_prop = true;
	  while (modified_prop) {
// 	    modified_prop = false;
	    modified_prop=propagate_colors(design, head_sg_node, num_states);
	    j++;
	    if (modified_prop)
	      modified_hl = true;
#ifdef __VERBOSE_UNTIMED__
	    cout << "Number of times propagating = " << j << endl
		 << "Modified prop = " << modified_prop
		 << "  Modified_hl = " << modified_hl  << endl;
	    print_states_col(design, curnode->this_node->counter);
#endif
	  }
	  temp_sg_node = head_sg_node->succ;
	  free_sg_node(temp_sg_node->succ);
	  temp_sg_node->succ = NULL;
	}
      }
      temp_sg_node = head_sg_node->succ;
    }
  }
}

// ********************************************************************
// Top-level function for checking the final colorings for consistancy.
// ********************************************************************
void final_check(designADT design, int& num_unique_haz_nodes,
		 int haz_node_vec[], int num_nodes,
		 nodelistADT nodelist, bool general,
		 char* output_name) {
  
  // Total number of ack or mono hazards on current node under test
  int num_ack_hazards = 0;
  int num_mono_hazards = 0;
  // Running total of number of ack or mono hazards for the nodelist
  int total_ack_hazards = 0;
  int total_mono_hazards = 0;
  // If vector entry is non-zero after hazard checks, indicates how many
  // hazards were reported on the node at that index
  int ack_haz_node_vec[num_nodes];
  int mono_haz_node_vec[num_nodes];
  // This vector contains mono hazards on gate output nodes rather than on
  // the fanin that causes the hazard as is the case in the vector above
  int mono_haz_node_vec1[num_nodes];
  
  for (int i=0;i<num_nodes;i++) {
    ack_haz_node_vec[i] = 0;
    mono_haz_node_vec[i] = 0;
    mono_haz_node_vec1[i] = 0;
    haz_node_vec[i] = 0;
  }

  // Do all of the nodes
  for (nodelistADT curnode=nodelist;(curnode && curnode->this_node);
       curnode=curnode->link) {
    if (curnode->this_node->type != IO) {	
      if (curnode->this_node->level != 0) {
#ifdef __VERBOSE_HAZINFO__
 	cout << "Evaluating node "
 	     << curnode->this_node->label << "...." << endl;
#endif	
	
	// Check the state graph for consistancy
	num_ack_hazards = sg_check(design, curnode->this_node);
	if (num_ack_hazards > 0) {
	  ack_haz_node_vec[curnode->this_node->counter] = num_ack_hazards;
	  curnode->this_node->ahazard = true;
	  total_ack_hazards = total_ack_hazards + num_ack_hazards;
	}
	
	// Do a monotonicity check
	num_mono_hazards = mono_check(design, curnode, general,
				      mono_haz_node_vec,
				      mono_haz_node_vec1);
	total_mono_hazards = total_mono_hazards + num_mono_hazards;
      }
    }
  }
  print_hazard_info(ack_haz_node_vec, mono_haz_node_vec,
		    total_ack_hazards, total_mono_hazards,
		    haz_node_vec, num_unique_haz_nodes,
		    num_nodes, output_name, mono_haz_node_vec1);
#ifdef __VERBOSE_HAZINFO__
#endif	
}

// ********************************************************************
// High level function to check for monotonicity in the final coloring.
// Checks all states for curnode.
// ********************************************************************
int mono_check(designADT design, nodelistADT curnode, bool general,
	       int mono_haz_node_vec[], int mono_haz_node_vec1[]) {
  
  int num_preds;
  int which_pred=0;
  int num_mono_hazards = 0;

#ifdef __VERBOSE_MONO__
  cout << "Checking node " << curnode->this_node->label << endl;
#endif
  
  // foreach state si in S
  for (int i=0;i<PRIME;i++) {
    for (stateADT curstate=design->state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
#ifdef __VERBOSE_MONO__
      cout << "Checking state " << curstate->state << endl;
#endif      
      // Find number of preds for curnode
      if (!general) {
	if (curnode->this_node->type == INV)
	  num_preds = 1;
	else if (curnode->this_node->type == NAND2)
	  num_preds = 2;
	else if (curnode->this_node->type == CEL)
	  num_preds = 3;
	else
	  cout << "Error because node is not INV, NAND2, or CEL" << endl;
      }
      else
	num_preds = strlen(curnode->this_node->SOP->product);
      which_pred=0;
      
      // foreach v that is a pred of u
      for (nodelistADT prednode=curnode->this_node->preds;prednode;
	   prednode=prednode->link) {
	
	// if (potential_hazard(si,u,v))
	if (potential_hazard(curstate, curnode, prednode, num_preds,
			     which_pred, general)) {
#ifdef __VERBOSE_MONO__
	  cout << "Potential hazard on input "
	       << prednode->this_node->label << endl;
#endif      
	  
	  // if u is not forcing in state si
	  if (!mono_forcing(curstate, curnode, num_preds)) {
	    
	    // report monotinicity violation caused by v on u in si;
#ifdef __VERBOSE_HAZINFO__
	    cout << "  Monotonicity violation caused by node "
		 << prednode->this_node->label
		 << " in state " << curstate->state << endl;
#endif      
	    num_mono_hazards++;
	    prednode->this_node->mhazard = true;
	    mono_haz_node_vec[prednode->this_node->counter]++;
	    mono_haz_node_vec1[curnode->this_node->counter]++;
	  }
	  else
	    prednode->this_node->mhazard = false;
	}
	which_pred++;
      }
    }
  }
  return num_mono_hazards;
}
 
// ********************************************************************
// Returns true if a potential hazard exists.  potential_hazard(si,u,v)
// ********************************************************************
bool potential_hazard(stateADT curstate, nodelistADT curnode,
		      nodelistADT prednode, int num_preds, 
		      int which_pred, bool general) {
  
  char* cube;
  char* cube_comp;

#ifdef __VERBOSE_MONO__
  cout << "Pred check of " << prednode->this_node->label << endl;
#endif      
  if ((prednode->this_node->type != IO) &&
      (prednode->this_node->type != INNEE) &&
      (prednode->this_node->type != OUTTEE) &&
      (mono_stable(prednode, curstate))) {
#ifdef __VERBOSE_MONO__
    cout << prednode->this_node->label << " is stable" << endl;
#endif      
    return false;
  }

  // Build a cube and make a copy in cube_comp
  cube = build_potential_cube(curstate, curnode, prednode, num_preds);
  cube_comp = (char*)GetBlock(sizeof(char)*(num_preds+1));
  strcpy(cube_comp,cube);

  // Complement the cube entry at prednode
  if (cube_comp[which_pred] == '0')
    cube_comp[which_pred] = '1';
  else if (cube_comp[which_pred] == '1')
    cube_comp[which_pred] = '0';

  //  if (v is in (I union O)) &&
  //     eval[u][si] != eval of u with v complemented
  if ((prednode->this_node->type == IO) ||
      (prednode->this_node->type == INNEE) ||
      (prednode->this_node->type == OUTTEE)) {
    if (curstate->evalvec[curnode->this_node->counter] !=
	mono_eval(curnode, cube_comp, num_preds)) {
#ifdef __VERBOSE_MONO__
      cout << prednode->this_node->label << " is IO and not at evaluation" 
	   << endl;
#endif      
      return false;
    }
  }
  
  bool enabled=false;
  char eval;
  if (curstate->colorvec[curnode->this_node->counter]=='R' || 
      curstate->colorvec[curnode->this_node->counter]=='F') {
#ifdef __VERBOSE_MONO__
    cout << curnode->this_node->label << " is enabled." << endl;
#endif      
    enabled=true;
  }
  else {
#ifdef __VERBOSE_MONO__
    cout << curnode->this_node->label << " is not enabled." << endl;
#endif      
  }
  // if eval[u][si] == eval[u][cube]
  if ((eval = mono_eval(curnode, cube, num_preds))=='X') {
    if (enabled) {
#ifdef __VERBOSE_MONO__
      cout << prednode->this_node->label << " does not disable " <<
	cube << endl;
#endif      
      return false;
    }
  } else {
    if (/*(!enabled) ||*/ (eval==curstate->evalvec[curnode->this_node->counter])) {
#ifdef __VERBOSE_MONO__
      cout << prednode->this_node->label << " does not enable " <<
	cube << endl;
#endif      
      return false;
    }
  }
  return true;
}

// ********************************************************************
// Evaluates the node u (curnode) using the cube as input.
// ********************************************************************
char mono_eval(nodelistADT curnode, char* cube, int num_preds) {

  int i=0;
  bool prod_sat=false;

  // Check case where node evaluates to true
  for (level_exp curprod=curnode->this_node->SOP;curprod;
       curprod=curprod->next) {
#ifdef __VERBOSE_MONO__
    cout << "check 1 " << curprod->product << " cube = " << cube << endl;
#endif      
    prod_sat = true;
    for (i=0;i<num_preds;i++) {
      if ((curprod->product[i] != 'X') &&
	  (curprod->product[i] != cube[i])) {
	
	// Only need one care term in product that doesn't match
	// care term in cube for the output to be false
	prod_sat = false;
	break;
      }
    }
    if (prod_sat) {
#ifdef __VERBOSE_MONO__
      cout << "Return 1" << endl;
#endif      
      return '1';
    }
  }

  // Check case where node evaluates to false i.e. none of the product
  // terms are satisfied
  for (level_exp curprod=curnode->this_node->SOP;curprod;
       curprod=curprod->next) {
#ifdef __VERBOSE_MONO__
    cout << "check 0 " << curprod->product << " cube = " << cube << endl;
#endif      
    prod_sat = true;
    for (i=0;i<num_preds;i++) {
      if ((curprod->product[i] != 'X') &&
	  (cube[i] != 'X') &&
	  (curprod->product[i] != cube[i])) {
	
	// Only need one care term in product that doesn't match
	// care term in cube for the output to be false
	prod_sat = false;
	break;
      }
    }
    if (prod_sat) {
#ifdef __VERBOSE_MONO__
      cout << "Return X" << endl;
#endif      
      return 'X';
    }
  }
  if (!prod_sat) {
#ifdef __VERBOSE_MONO__
    cout << "Return 0" << endl;
#endif      
    return '0';
  }
#ifdef __VERBOSE_MONO__
  cout << "Return X" << endl;
#endif      
  return 'X';
}

// *********************************************************************
// Build a cube that represents the evaluations of the predecessors
// to the node of interest in curstate.
// Cube is a character vector (0,1,X) of length (# of preds).
// *********************************************************************
char* build_potential_cube(stateADT curstate, nodelistADT curnode,
			   nodelistADT prednode, int num_preds) {

  int index=0;
  char* cube;

  cube = (char*)GetBlock(sizeof(char)*(num_preds+1));

  // Foreach w in preds of u
  for (nodelistADT prednode1=curnode->this_node->preds;prednode1;
       prednode1=prednode1->link) {

  // If w is in (I union O)
    if ((prednode1->this_node->type == IO) ||
	(prednode1->this_node->type == INNEE) ||
	(prednode1->this_node->type == OUTTEE)) {
      if ((curstate->state[prednode1->this_node->name] == '0') ||
	  (curstate->state[prednode1->this_node->name] == 'R'))
	cube[index] = '0';
      else
	cube[index] = '1';
    }
    else if (prednode == prednode1)
      cube[index] = curstate->evalvec[prednode1->this_node->counter];
    else if (mono_stable(prednode1, curstate))
      cube[index] = curstate->colorvec[prednode1->this_node->counter];
    else
      cube[index] = 'X';
    index++;
  }
  cube[index] = '\0';
  return cube;
}

// ********************************************************************
// Returns true if the node is an IO or if the color of node is stable
// in curstate.
// ********************************************************************
bool mono_stable(nodelistADT node, stateADT curstate) {
  
  if ((node->this_node->type == IO) ||
      (node->this_node->type == INNEE) ||
      (node->this_node->type == OUTTEE) ||
      (curstate->colorvec[node->this_node->counter] == '1') ||
      (curstate->colorvec[node->this_node->counter] == '0'))
    return true;
  else 
    return false;
}

// ********************************************************************
// Return true if function is forcing.
// ********************************************************************
bool mono_forcing(stateADT curstate, nodelistADT curnode, 
		  int num_preds) {
  
  bool prod_sat=false;
  char* cube;

  cube = build_forcing_cube(curstate, curnode, num_preds);

#ifdef __VERBOSE_MONO__
  cout << "Forcing cube " << cube << endl;
#endif      

  // If evaluation of curnode is true
  if (curstate->evalvec[curnode->this_node->counter] == '1') {

    // The function is forcing if there exists one of the terms in
    // the SOP set whose care terms all match care terms in the cube
    for (level_exp curprod=curnode->this_node->SOP;curprod;
	 curprod=curprod->next) {
      prod_sat = true;
      for (int i=0;i<num_preds;i++) {
	if (((curprod->product[i] == '0') && (cube[i] != '0')) ||
	    ((curprod->product[i] == '1') && (cube[i] != '1'))) {
	  prod_sat = false;
	  break;
	}
      }
      if (prod_sat) {
#ifdef __VERBOSE_MONO__
	cout << "Eval is 1 and product " << curprod->product << 
	  " is satisfied." << endl;
#endif      
	return true;
      }
    }
  }
  else {
    
    // Evaluation of curnode is false  
    // The function is also forcing if, for every product term in the
    // SOP set, some care term of cube disagrees with the

    // corresponding care term in the SOP set.
    for (level_exp curprod=curnode->this_node->SOP;curprod;
	 curprod=curprod->next) {
      prod_sat = true;
      for (int i=0;i<num_preds;i++) {
	if (((curprod->product[i] == '0') && (cube[i] == '1')) ||
	    ((curprod->product[i] == '1') && (cube[i] == '0'))) {
	  prod_sat = false;
	  break;
	}
      }
      if (prod_sat)
	break;
    }
    if (!prod_sat) {
#ifdef __VERBOSE_MONO__
      cout << "Eval is 0 and no product can be satisfied." << endl;
#endif      
      return true;
    }
  }
#ifdef __VERBOSE_MONO__
  cout << "Not forcing." << endl;
#endif      
  return false;
}

// *********************************************************************
// Build a cube that represents the evaluations of the predecessors
// to the node of interest in curstate.
// Cube is a character vector (0,1,X) of length (# of preds).
// *********************************************************************
char* build_forcing_cube(stateADT curstate, nodelistADT curnode,
			 int num_preds) {

  int index=0;
  char* cube;

  cube=(char*)GetBlock(sizeof(char)*(num_preds+1));
  
  // foreach v in preds of u
  for (nodelistADT prednode=curnode->this_node->preds;prednode;
       prednode=prednode->link) {
    
    // If v is in (I union O)
    if ((prednode->this_node->type == IO) ||
	(prednode->this_node->type == INNEE) ||
	(prednode->this_node->type == OUTTEE)) {
      
      // cube[index] = curstate->evalvec[index];
      if ((curstate->state[prednode->this_node->name] == '0') ||
	  (curstate->state[prednode->this_node->name] == 'R'))
	cube[index] = '0';
      else
	cube[index] = '1';
    }
    else if (curstate->colorvec[prednode->this_node->counter] == '1')
      cube[index] = '1';
    else if (curstate->colorvec[prednode->this_node->counter] == '0')
      cube[index] = '0';
    else
      cube[index] = 'X';
    index++;
  }
  cube[index] = '\0';
  return cube;
}

// ********************************************************************
// Checks state graph for coloring inconsistancies. Any R->F, R->D,
// U->F, U->D, F->R, F->U, D->R, or D->U transition between two
// states is flagged and returns true, if the edge between them is not
// stable, indicating a coloring error.
// ********************************************************************
int sg_check(designADT design, node* curnode) {

  int index = curnode->counter;
  bool error = false;
  int error1 = 0;

  for (int i=0;i<PRIME;i++) {
    for (stateADT curstate=design->state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      
      // Look at all next states from curstate
      for (statelistADT edge=curstate->succ;
	   (edge != NULL && edge->stateptr->state != NULL);
	   (edge=edge->next)) {
	if (((curstate->colorvec[index] == 'R') ||
	     (curstate->colorvec[index] == 'U')) && 
	    ((edge->stateptr->colorvec[index] == 'F') ||
	     (edge->stateptr->colorvec[index] == 'D'))) {
	  if ((edge->colorvec[index] != '1'))
	    error = true;
	}
	else if (((curstate->colorvec[index] == 'F') ||
		  (curstate->colorvec[index] == 'D')) && 
		 ((edge->stateptr->colorvec[index] == 'R') ||
		  (edge->stateptr->colorvec[index] == 'U'))) {
	  if ((edge->colorvec[index] != '0'))
	    error = true;
	}
	if (error) {
#ifdef __VERBOSE_HAZINFO__
	  cout << "  " << curstate->colorvec[index] << "->"
	       << edge->colorvec[index] << "->"
	       << edge->stateptr->colorvec[index];
	  print_coloring_error(design, curstate, edge, index);
#endif
	  error = false;
	  error1++;
	}
      }
    }
  }
  return error1;
}
