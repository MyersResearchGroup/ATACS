// *********************************************************************
// Miscellaneous functions to support Curt Nelson's tech mapping code.
// *********************************************************************

#include "decomp.h"

// ********************************************************************
// Create the color vector for each state and edge and the boolean
// evaluation vector for each state.
// ********************************************************************
void create_color_eval_vec(designADT design, nodelistADT nodelist,
			   node* input_vec[], int tot_num_inputs,
			   bool general) {

  // Go through all states
  for (int i=0;i<PRIME;i++) {
    for (stateADT cur_state1=design->state_table[i];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      
      // Evaluate the value of all input nodes
      eval_input_nodes(design, input_vec, tot_num_inputs, cur_state1);
      
      // Do a Boolean evaluation of the entire tree for current state
      // and fill the colorvec and evalvec.
      fill_color_eval_vecs(design, nodelist, cur_state1, general);

      // Copy the color vector to each successor edge of state
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	strcpy(cur_state2->colorvec,cur_state1->colorvec);
	
	// Must also color pred edge from following state
	copy_pred_vec(cur_state1, cur_state2);
      }
    }
  }
}

// ********************************************************************
// Fill in the colorvec and evalvec for cur_state1.
// ********************************************************************
void fill_color_eval_vecs(designADT design, nodelistADT nodelist,
			  stateADT cur_state1, bool general) {

  int level=0;
  bool modified=false;
  node* head=NULL;

  if (general)

    // Do a netlist with general logic elements
    gen_bool_eval(nodelist, cur_state1);
  else {

    // Do a netlist with INV, NAND2, and C-ELEMENTS
    do {
      level++;
      modified=false;
      for (nodelistADT curnode=nodelist;(curnode && curnode->this_node);
	   curnode=curnode->link) {
	head = curnode->this_node;
	if (head->level == level) {
	  modified=true;
	  if (head->type == INV) {
	    if (!(head->pred1->value)) {
	      cur_state1->colorvec[head->counter] = 'R';
	      cur_state1->evalvec[head->counter] = '1';
	      head->value = true;
	    } else {
	      cur_state1->colorvec[head->counter] = 'F';
	      cur_state1->evalvec[head->counter] = '0';
	      head->value = false;
	    }
	  }
	  else if (head->type == NAND2) {
	    if (!(head->pred1->value && head->pred2->value)) {
	      cur_state1->colorvec[head->counter] = 'R';
	      cur_state1->evalvec[head->counter] = '1';
	      head->value = true;
	    } else {
	      cur_state1->colorvec[head->counter] = 'F';
	      cur_state1->evalvec[head->counter] = '0';
	      head->value = false;
	    }
	  }
	  else if (head->type == CEL) {
	    if ((head->pred1->value && head->pred2->value) ||
		((cur_state1->state[head->succ->name]=='1' ||
		  cur_state1->state[head->succ->name]=='F') &&
		 (head->pred1->value || head->pred2->value))) {
	      cur_state1->colorvec[head->counter] = 'R';
	      cur_state1->evalvec[head->counter] = '1';
	      head->value = true;
	    } else {
	      cur_state1->colorvec[head->counter] = 'F';
	      cur_state1->evalvec[head->counter] = '0';
	      head->value = false;
	    }
	  }
	}
      }
    } while (modified);
  }
}

// ********************************************************************
// Copies the color vector from cur_state1 to the pred edge on the
// successor edge.
// ********************************************************************
void copy_pred_vec(stateADT cur_state1, statelistADT edge) {

  // Go through all predecessors
  for (statelistADT cur_state2 = edge->stateptr->pred;
       (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
       (cur_state2=cur_state2->next)) {

    // Go through all successors
    for (statelistADT cur_state3 = cur_state2->stateptr->succ;
	 (cur_state3 != NULL && cur_state3->stateptr->state != NULL);
	 (cur_state3=cur_state3->next)) {

      if (cur_state3 == edge) {
	strcpy(cur_state2->colorvec,cur_state1->colorvec);
	break;
      }
    }
  }
}

// ********************************************************************
// Determine a boolean value on all the nodes in the input vector for
// this one state.
// ********************************************************************
void eval_input_nodes(designADT design, node* input_vec[],
		      int tot_num_inputs, stateADT cur_state1) {

  for (int inp_vec_index=0;inp_vec_index<tot_num_inputs;inp_vec_index++) {
    for (int i=0;i<design->nsignals;i++) {
      if (i == input_vec[inp_vec_index]->name) {
	if ((cur_state1->state[i] == '0') ||
	    (cur_state1->state[i] == 'R'))
	  input_vec[inp_vec_index]->value = false;
	else
	  input_vec[inp_vec_index]->value = true;
      }
    }
  }
}

// ********************************************************************
// Does a boolean evaluation of a netlist with general logic elements.
// ********************************************************************
void gen_bool_eval(nodelistADT nodelist, stateADT cur_state1) {

  int level=0;
  int prodsize;
  int i=0;
  bool modified=false;
  char* cube;
  node* head=NULL;

  do {
    level++;
    modified=false;
    for (nodelistADT curnode=nodelist;curnode;curnode=curnode->link) {
      head = curnode->this_node;
      if (head->level==level) {
	modified=true;
	if (head->SOP) {
	  prodsize=strlen(head->SOP->product);
	  cube=(char*)GetBlock(sizeof(char)*(prodsize+1));
	  i=0;

	  // Build cube using values from all predecessor nodes
	  for (nodelistADT prednode=head->preds;
	       prednode;prednode=prednode->link) {
	    if (prednode->this_node->value)
	      cube[i]='1';
	    else 
	      cube[i]='0';
	    i++;
	  } 
	  cube[i]='\0';

	  // Speculate that the gate output will be false
	  head->value=false;
	  cur_state1->colorvec[head->counter] = 'F';
	  cur_state1->evalvec[head->counter] = '0';
	  for (level_exp curprod=head->SOP;curprod;
	       curprod=curprod->next) {
	    for (i=0;i<prodsize;i++) {
	      if ((curprod->product[i] != 'X') &&
		  (curprod->product[i] != cube[i]))

		// Only need one product term that doesn't match
		// the cube for the output to be false
		break;
	    }

	    // i = prodsize if the output of the function is true
	    if (i == prodsize) {
	      head->value=true;
	      cur_state1->colorvec[head->counter] = 'R';
	      cur_state1->evalvec[head->counter] = '1';
	      break;
	    }
	  }
	}
      }
    }
  } while (modified);
}

// ********************************************************************
// Allocate and init memory for vec.
// ********************************************************************
void make_vec(char*& vec, int num_nodes) {

  vec=(char*)GetBlock((num_nodes+1)*sizeof(char));

  // Null out all entries
  for (int i=0;i<num_nodes+1;i++)
    vec[i] = '\0';
}

// ********************************************************************
// Allocate and init memory for color and eval vectors in all states
// and edges.
// ********************************************************************
void alloc_color_eval_mem(designADT design, int num_nodes) {

  // Go through all states
  for (int i=0;i<PRIME;i++) {
    for (stateADT cur_state1=design->state_table[i];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      make_vec(cur_state1->colorvec, num_nodes);
      make_vec(cur_state1->evalvec, num_nodes);

      // Go through all successors of cur_state1
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	make_vec(cur_state2->colorvec, num_nodes);
	
	// Go through all predecessors
	for (statelistADT cur_state4 = cur_state2->stateptr->pred;
	     (cur_state4 != NULL && cur_state4->stateptr->state != NULL);
	     (cur_state4=cur_state4->next)) {
	  
	  // Go through all successors of the state this edge is pointing to
	  for (statelistADT cur_state3 = cur_state4->stateptr->succ;
	       (cur_state3 != NULL && cur_state3->stateptr->state != NULL);
	       (cur_state3 = cur_state3->next)) {
	    if (cur_state3 == cur_state2) {
	      make_vec(cur_state4->colorvec, num_nodes);
	      break;
	    }
	  }
	}
      }
    }
  }
}

// ********************************************************************
// Finds and colors the right predecessor link from the next state.
// ********************************************************************
void color_pred_vec(statelistADT edge, int index, char color) {

  // Go through all predecessors
  for (statelistADT cur_state2 = edge->stateptr->pred;
       (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
       (cur_state2=cur_state2->next)) {

    // Go through all successors
    for (statelistADT cur_state3 = cur_state2->stateptr->succ;
	 (cur_state3 != NULL && cur_state3->stateptr->state != NULL);
	 (cur_state3=cur_state3->next)) {

      if (cur_state3 == edge) {
	cur_state2->colorvec[index] = color;
	break;
      }
    }
  }
}

// ********************************************************************
// Finds the number of states and edges in the state graph.
// *********************************************************************
void find_num_state_edge(designADT design, int& num_states,
			 int& num_edges, int num_nodes) {
  
  // Go through all states
  num_states=0;
  for (int n=0;n<PRIME;n++) {
    for (stateADT cur_state1=design->state_table[n];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      if (cur_state1->number+1 > num_states)
	num_states=cur_state1->number+1;

      // Go through all succeeding edges from this state
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	num_edges++;
      }
    }
  }
#ifdef __VERBOSE_STARTUP__
    cout << "Num nodes = " << num_nodes
	 << "  Num states = " << num_states
	 << "  Num edges = " << num_edges << endl << endl;
#endif
}

// ********************************************************************
// Do one side of the CEL chain.
// ********************************************************************
void do_oneside_cel(designADT design, node*& head, cover_node* head_node,
		    int num_covers, bool which_chain, node* input_vec[],
		    int& inp_vec_index, int& num_inputs,
		    bool reset_path) {
  
#ifdef __ADD_DOUBLE_INV__
  if (((head_node->num_lits != 1) || (num_covers != 1))
      && (!reset_path)) {
    head = make_fill_node_pred1(head, INV);
    head = make_fill_node_pred1(head, INV);
  }
#endif
  if (num_covers == 1) {
    do_and_func(design, head, head_node, which_chain, input_vec,
		inp_vec_index, num_inputs);
  }
  else {
    
    // Decompose the entire OR chain including the AND gates
#ifdef __ADD_DOUBLE_INV__
    if (reset_path) {
      head = make_fill_node_pred1(head, INV);
      head = make_fill_node_pred1(head, INV);
    }
#endif
    do_or_func(design, head, head_node, num_covers, which_chain,
	       input_vec, inp_vec_index, num_inputs);
  }
}

// ********************************************************************
// High level decomposition routine.
// ********************************************************************
void dothe_decomp(designADT design, int num_set_covers, int num_reset_covers,
		  cover_node*& head_set_node, cover_node*& head_reset_node,
		  node* input_vec[], int& inp_vec_index, int combo,
		  int& num_set_inputs, int& num_reset_inputs,
		  int& num_combo_inputs, node* output_vec[],
		  int out_vec_index) {

  node* head_node = output_vec[out_vec_index];
  node* head_CEL_node;
  bool which_chain = SET_CHAIN;
  bool reset_path = false;

  if (combo == NOT_COMBO) {

    // Make a C-element
    head_node = make_fill_node_pred1(head_node, CEL);

    // Store a pointer at the C-element for doing the reset leg later
    head_CEL_node = head_node;

    // Do the set part of the chain
    do_oneside_cel(design, head_node, head_set_node, num_set_covers,
		   which_chain, input_vec, inp_vec_index,
		   num_set_inputs, reset_path);

    // Do the reset part of the chain
    head_node = head_CEL_node;
    which_chain = RES_CHAIN;
    reset_path = true;

    // Add an inverter since the reset path has a bubble at the C-element
    head_node = make_fill_node_pred2(head_node, INV);
    do_oneside_cel(design, head_node, head_reset_node, num_reset_covers,
		   which_chain, input_vec, inp_vec_index,
		   num_reset_inputs, reset_path);
  }
  else if (combo == POS_COMBO) {
    if (num_set_covers == 1) {
      do_and_func(design, head_node, head_set_node, which_chain, input_vec,
		  inp_vec_index, num_set_inputs);
    }
    else {
#ifdef __ADD_DOUBLE_INV__
      head_node = make_fill_node_pred1(head_node, INV);
      head_node = make_fill_node_pred1(head_node, INV);
#endif
      do_or_func(design, head_node, head_set_node, num_set_covers,
		 which_chain, input_vec, inp_vec_index,
		 num_combo_inputs);
    }
  }
  else if (combo == NEG_COMBO) {
    
    // Negative logic, add inverter
    head_node = make_fill_node_pred1(head_node, INV);
    if (num_reset_covers == 1)
      do_and_func(design, head_node, head_reset_node, which_chain, input_vec,
		  inp_vec_index, num_reset_inputs);
    else
      do_or_func(design, head_node, head_reset_node, num_reset_covers,
		 which_chain, input_vec, inp_vec_index, num_combo_inputs);
  }
}

// ********************************************************************
// High level function for implementing the OR portion of the SOP tree.
// Calls the AND function part. When done, decomposition is complete
// for this side of the output.
// ********************************************************************
void do_or_func(designADT design, node*& head, cover_node* head_node,
		int num_covers, bool which_chain, node* input_vec[],
		int& inp_vec_index, int& num_inputs) {
  
  node* temp_head;
  
  for (int i=0;i<num_covers;i++) {
    
    // Do as long as this is not the last cover
    if (i != num_covers-1) {
      head = make_fill_node_pred1(head, NAND2);
      
      // Store pointer to NAND2 gate so we can return and do the other path
      temp_head = head;
      head = make_fill_node_pred1(head, INV);
    }
    
    if (head_node->num_lits == 1) {
      
      // Just add the input node
      for (int j=0;j<design->nsignals;j++) {
	if (head_node->cover[j] != 'Z') {
	  do_input_nodes(head_node, j, head, which_chain, num_inputs,
			 input_vec, inp_vec_index);
	}
      }
    }
    else
      
      // Decompose a multiple input AND function
      do_and_func(design, head, head_node, which_chain, input_vec,
		  inp_vec_index, num_inputs);
    
    // Go down other successor of the NAND2 gate to finish the OR gate
    // but only if this is not the last cover term
    if (i != num_covers - 1) {
      head = temp_head;
      head = make_fill_node_pred2(head, INV);
#ifdef __ADD_DOUBLE_INV__
      if (i != num_covers - 2) {
	head = make_fill_node_pred1(head, INV);
	head = make_fill_node_pred1(head, INV);
      }
#endif
      head_node = head_node->succ;
    }
  }
}

// ********************************************************************
// Add one or two inverters and an input node to this leg of the
// decomposition.
// ********************************************************************
void do_input_nodes(cover_node* head_node, int i, node*& head,
		    bool which_chain, int& num_inputs, node* input_vec[],
		    int& inp_vec_index) {

  if (head_node->cover[i] == '0')
    head = make_fill_node_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  else {
    head = make_fill_node_pred1(head, INV);
    head = make_fill_node_pred1(head, INV);
  }
#endif
  head = make_fill_node_pred1(head, INNEE);
  head->level = INPUT_LEVEL;
  head->set_res = which_chain;
  head->name = i;
  num_inputs++;
  
  // Stick input node in input_vec
  input_vec[inp_vec_index] = head;
  inp_vec_index++;
}

// ********************************************************************
// Decompose one AND gate.
// ********************************************************************
void do_and_func(designADT design, node*& head, cover_node* head_node,
		 bool which_chain, node* input_vec[],
		 int& inp_vec_index, int& num_inputs) {
  
  int lits_covered=0;
  bool add_inv = false;
#ifdef __ADD_DOUBLE_INV__
  bool add_double_inv = false;
  bool first_time = true;
#endif

  if (head_node->num_lits == 1) {

    // Just add the input node
    for (int j=0;j<design->nsignals;j++) {
      if (head_node->cover[j] != 'Z') {
	do_input_nodes(head_node, j, head, which_chain, num_inputs,
		       input_vec, inp_vec_index);
	break;
      }
    }
  }
  else {
    for (int i=0;i<design->nsignals;i++) {
      if (head_node->cover[i] != 'Z') {
	if ((head_node->num_lits - lits_covered) == 1) {
	  
	  // We're done so add input node and inverters
	  do_input_nodes(head_node, i, head, which_chain, num_inputs,
			 input_vec, inp_vec_index);
	  break;
	}
	else {
#ifdef __ADD_DOUBLE_INV__
	  if (!first_time) {
	    head = make_fill_node_pred1(head, INV);
	    head = make_fill_node_pred1(head, INV);
	    first_time = false;
	  }
#endif
	  head = make_fill_node_pred1(head, INV);
	  head = make_fill_node_pred1(head, NAND2);
#ifdef __ADD_DOUBLE_INV__
	  first_time = false;
#endif
	  if (head_node->cover[i] == '0') {
	    head = make_fill_node_pred2(head, INV);
	    head = make_fill_node_pred1(head, INNEE);
	    add_inv = true;
	  }
	  else {
#ifndef __ADD_DOUBLE_INV__
	    head = make_fill_node_pred2(head, INNEE);
#else
	    head = make_fill_node_pred2(head, INV);
	    head = make_fill_node_pred1(head, INV);
	    head = make_fill_node_pred1(head, INNEE);
	    add_double_inv = true;
#endif
	  }
	  head->level = INPUT_LEVEL;
	  head->set_res = which_chain;
	  head->name = i;
	  num_inputs++;
	  
	  // Stick input node in input_vec
	  input_vec[inp_vec_index] = head;
	  inp_vec_index++;
	  head = head->succ;
	  if (add_inv == true) {
	    head = head->succ;
	    add_inv = false;
	  }
#ifdef __ADD_DOUBLE_INV__
	  if (add_double_inv == true) {
	    head = head->succ;
	    head = head->succ;
	    add_double_inv = false;
	  }
#endif
	  lits_covered++;
	}
      }
    }
  }
}

// ********************************************************************
// High level routine to build a linked list of covers for 1 output.
// ********************************************************************
void build_linked_lists(designADT design, int region_num,
			int& num_set_covers, int& num_reset_covers,
			cover_node* head_set_node,
			cover_node* head_reset_node) {

  // Do set region
  num_set_covers = build_cover_list(design, region_num, head_set_node);
#ifdef __VERBOSE_STARTUP__
  cout << "Number of set ";
  print_cover_list(num_set_covers, head_set_node);
#endif

  // Do reset region
  num_reset_covers = build_cover_list(design, region_num+1,
				      head_reset_node);
#ifdef __VERBOSE_STARTUP__
  cout << "Number of reset ";
  print_cover_list(num_reset_covers, head_reset_node);
  cout << endl;
#endif
}

// ********************************************************************
// Build a linked list of covers.
// ********************************************************************
int build_cover_list(designADT design, int region_num,
		     cover_node* current_node) {

  int num_covers=0;
  int total_covers=0;
  
  for (regionADT cur_region=design->regions[region_num];cur_region;
       cur_region=cur_region->link) 
    if (design->exact && cur_region->covers && cur_region->covers->cover) {
      for (coverlistADT cur_cover=cur_region->covers;
	   cur_cover && cur_cover->cover;
	   cur_cover=cur_cover->link) {
	if (cur_cover->cover) {
#ifdef __VERBOSE_STARTUP__
	  print_region_info(design, cur_region, cur_cover->cover,
			    region_num, total_covers);
#endif
	  if (cur_cover->cover[0] != 'E') {
	    // We have a valid cover
	    current_node->num_lits = find_num_lits(design, cur_cover->cover);
	    current_node->cover = cur_cover->cover;
	    current_node->succ = make_cover_node();
	    current_node = current_node->succ;
	    num_covers++;
	  }
	  total_covers++;
	}
      }
    } else {
      if (cur_region->cover) {
#ifdef __VERBOSE_STARTUP__
	print_region_info(design, cur_region, cur_region->cover,
			  region_num, total_covers);
#endif
	if (cur_region->cover[0] != 'E') {
	  // We have a valid cover
	  current_node->num_lits = find_num_lits(design, cur_region->cover);
	  current_node->cover = cur_region->cover;
	  current_node->succ = make_cover_node();
	  current_node = current_node->succ;
	  num_covers++;
	}
	total_covers++;
      }
    }
  return num_covers;
}

// ********************************************************************
// Check for combinational output.
// ********************************************************************
int check_combo_output(int num_set_covers, int num_reset_covers) {
  
  if ((num_set_covers != 0) && (num_reset_covers != 0))
    
    // State holding output
    return NOT_COMBO;
  else if ((num_set_covers == 0) && (num_reset_covers == 0))
    
    // Both cover lists empty, bad bad
    exit(1);
  else if (num_reset_covers == 0)
    
    // Positive combinational output
    return POS_COMBO;
  else
    
    // Negative combinational output
    return NEG_COMBO;
}

// *********************************************************************
// Finds the total numbers of 0, R, U, 1, F, and D colors in all the
// state and edge color vectors.
// *********************************************************************
void find_num_colors(designADT design, int num_nodes) {

  int num_state0s = 0;
  int num_stateRs = 0;
  int num_stateUs = 0;
  int num_state1s = 0;
  int num_stateFs = 0;
  int num_stateDs = 0;
  int num_edge0s = 0;
  int num_edgeRs = 0;
  int num_edgeUs = 0;
  int num_edge1s = 0;
  int num_edgeFs = 0;
  int num_edgeDs = 0;

  for (int n=0;n<PRIME;n++) {
    for (stateADT cur_state1=design->state_table[n];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      for (int i=0;i<num_nodes;i++) {
	switch (cur_state1->colorvec[i]) {
	case '0':
	  num_state0s++;
	  break;
	case 'R':
	  num_stateRs++;
	  break;
	case 'U':
	  num_stateUs++;
	  break;
	case '1':
	  num_state1s++;
	  break;
	case 'F':
	  num_stateFs++;
	  break;
	case 'D':
	  num_stateDs++;
	  break;
	default:
	  cout << "Entry " << cur_state1->colorvec[i] << " in error." << endl;
	  break;
	}
      }
      
      // Go through all successor states
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	for (int j=0;j<num_nodes;j++) {
	  switch (cur_state2->colorvec[j]) {
	  case '0':
	    num_edge0s++;
	    break;
	  case 'R':
	    num_edgeRs++;
	    break;
	  case 'U':
	    num_edgeUs++;
	    break;
	  case '1':
	    num_edge1s++;
	    break;
	  case 'F':
	    num_edgeFs++;
	    break;
	  case 'D':
	    num_edgeDs++;
	    break;
	  default:
	    cout << "Entry " << cur_state1->colorvec[j] << " in error."
		 << endl;
	    break;
	  }
	}
      }
    }
  }
  cout << "States:  0 = " << num_state0s
       << "  R = " << num_stateRs
       << "  U = " << num_stateUs
       << "  1 = " << num_state1s
       << "  F = " << num_stateFs
       << "  D = " << num_stateDs << endl;
//   cout << "Edges:   0 = " << num_edge0s
//        << "  R = " << num_edgeRs
//        << "  U = " << num_edgeUs
//        << "  1 = " << num_edge1s
//        << "  F = " << num_edgeFs
//        << "  D = " << num_edgeDs << endl;
}

// ********************************************************************
// Find the number of literals in a cover term.
// ********************************************************************
int find_num_lits(designADT design, char* cover) {

  int num_lits=0;

  for(int i=0;i<design->nsignals;i++) {
    if (cover[i] != 'Z')
      num_lits++;
  }
  return num_lits;
}

// ********************************************************************
// Finds the start (initial) state.
// ********************************************************************
stateADT find_start_state(designADT design) {
   
  stateADT cur_state1;

  for (int i=0;i<PRIME;i++) {
    for (cur_state1=design->state_table[i];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      if (cur_state1->number == 0)
	return cur_state1;
    }
  }
  return cur_state1;
}

// ********************************************************************
// Creates a list of all internal nodes from my decomposition.
// ********************************************************************
nodelistADT make_node_list(designADT design, node* input_vec[],
			   int tot_num_inputs, int& num_nodes_curt) {

  nodelistADT head_nodelist = NULL;
  nodelistADT head;
  node* decomp_head;
  bool first = true;

  clear_visited_decomp(input_vec, tot_num_inputs);
  for (int i=0;i<tot_num_inputs;i++) {
    decomp_head = input_vec[i]->succ;
    while ((decomp_head->visited == false) &&
	   (decomp_head->type != OUTTEE)) {
      if (!first) {
	head->link = (nodelistADT)GetBlock(sizeof(*(head)));
	head = head->link;
	head->link = NULL;
      }	
      else {
	head_nodelist = (nodelistADT)GetBlock(sizeof(*(head)));
	head = head_nodelist;
	head->link = NULL;
	first = false;
      }
      head->this_node = decomp_head;

      // Do the predecessor list
      do_nodelist_preds(design, head, decomp_head);

      // Do the successor list
      do_nodelist_succs(design, head, decomp_head);

      // Do the level expressions
      do_nodelist_level_exps(design, head);

      decomp_head->visited = true;
      decomp_head = decomp_head->succ;
      num_nodes_curt++;
    }
  }
  do_nodelist_labels(design, head_nodelist);
  return head_nodelist;
}

// ********************************************************************
// Put the label field in the nodelist.
// ********************************************************************
void do_nodelist_labels(designADT design, nodelistADT head_nodelist) {

  char expr[8];
  char temp[8];
  
  for (nodelistADT head=head_nodelist;head != NULL;head=head->link) {
    if (head->this_node->succ->type == OUTTEE)
      head->this_node->label =
	design->signals[head->this_node->succ->name]->name;
    else {
      strcpy(expr,"xx");
      num_to_char(temp, head->this_node->counter);
      strcat(expr,temp);
      head->this_node->label = CopyString(expr);
    }
  }
}

// ********************************************************************
// Do the predecessor list to this nodelist element.
// ********************************************************************
void do_nodelist_preds(designADT design, nodelistADT head,
		       node* decomp_head) {
  
  nodelistADT pred_head = NULL;

  head->this_node->preds = (nodelistADT)GetBlock(sizeof (*(head)));
  pred_head = head->this_node->preds;
  pred_head->link = NULL;
  pred_head->this_node = decomp_head->pred1;
  if (pred_head->this_node->type == INNEE)
    pred_head->this_node->label = 
      design->signals[pred_head->this_node->name]->name;
  if ((head->this_node->type == NAND2) ||
      (head->this_node->type == CEL)) {
    
    // Add a second predecessor
    pred_head->link = (nodelistADT)GetBlock(sizeof(*(head)));
    pred_head = pred_head->link;
    pred_head->link = NULL;
    pred_head->this_node = decomp_head->pred2;
    if (pred_head->this_node->type == INNEE)
      pred_head->this_node->label =
	design->signals[pred_head->this_node->name]->name;
    if (head->this_node->type == CEL) {
      
      // Add a third predecessor for a CEL and set type to OUTTEE
      pred_head->link = (nodelistADT)GetBlock(sizeof(*(head)));
      pred_head = pred_head->link;
      pred_head->link = NULL;
      pred_head->this_node = decomp_head->succ;
    }
  }
}

// ********************************************************************
// Do the successor to this nodelist element.
// ********************************************************************
void do_nodelist_succs(designADT design, nodelistADT head,
		       node* decomp_head) {
  
  head->this_node->succs = (nodelistADT)GetBlock(sizeof (*(head)));
  head->this_node->succs->link = NULL;
  head->this_node->succs->this_node = decomp_head->succ;
}

// ********************************************************************
// Do the level expressions
// ********************************************************************
void do_nodelist_level_exps(designADT design, nodelistADT head) {

  level_exp level_head = NULL;

  head->this_node->SOP = (level_exp)GetBlock(sizeof(*(level_head)));
  level_head = head->this_node->SOP;
  level_head->next = NULL;
  if (head->this_node->type == INV) {
    level_head->product = (char*)GetBlock(2*(sizeof(char)));
    level_head->product[0] = '0';
    level_head->product[1] = '\0';
  }
  else if (head->this_node->type == NAND2) {
    level_head->product = (char*)GetBlock(3*(sizeof(char)));
    level_head->product[0] = '0';
    level_head->product[1] = 'X';
    level_head->product[2] = '\0';
    level_head->next = (level_exp)GetBlock(sizeof(*(level_head)));
    level_head = level_head->next;
    level_head->next = NULL;
    level_head->product = (char*)GetBlock(3*(sizeof(char)));
    level_head->product[0] = 'X';
    level_head->product[1] = '0';
    level_head->product[2] = '\0';
  }
  else {      // CEL
    level_head->product = (char*)GetBlock(4*(sizeof(char)));
    level_head->product[0] = '1';
    level_head->product[1] = '1';
    level_head->product[2] = 'X';
    level_head->product[3] = '\0';
    level_head->next = (level_exp)GetBlock(sizeof(*(level_head)));
    level_head = level_head->next;
    level_head->next = NULL;
    level_head->product = (char*)GetBlock(4*(sizeof(char)));
    level_head->product[0] = '1';
    level_head->product[1] = 'X';
    level_head->product[2] = '1';
    level_head->product[3] = '\0';
    level_head->next = (level_exp)GetBlock(sizeof(*(level_head)));
    level_head = level_head->next;
    level_head->next = NULL;
    level_head->product = (char*)GetBlock(4*(sizeof(char)));
    level_head->product[0] = 'X';
    level_head->product[1] = '1';
    level_head->product[2] = '1';
    level_head->product[3] = '\0';
  }
}

// ********************************************************************
// Create a new list node.
// ********************************************************************
node* make_node(void) {

  node* new_node;

  new_node = new node;
  return new_node;
}

// ********************************************************************
// Create and initialize a new cover node.
// ********************************************************************
cover_node* make_cover_node(void) {

  cover_node* new_node;

  new_node = new cover_node;
  return new_node;
}

// ********************************************************************
// Create a new netlist node.
// ********************************************************************
netlist_node* make_netlist_node(void) {

  netlist_node* new_node;

  new_node = new netlist_node;
  return new_node;
}

// ********************************************************************
// Create and initialize a new state graph node.
// ********************************************************************
sg_node* make_sg_node(void) {

  sg_node* new_node;

  new_node = new sg_node;
  return new_node;
}

// ********************************************************************
// Create a new input node for covering a library cell.
// ********************************************************************
input_node* make_input_node(void) {

  input_node* new_node;

  new_node = new input_node;
  return new_node;
}

// ********************************************************************
// Make output node and fill with data.
// ********************************************************************
node* make_output_node(designADT design, node* output_vec[],
		       int out_vec_index, int output_num) {
  
  node* head_node;
  
  output_vec[out_vec_index] = make_node();
  head_node = output_vec[out_vec_index];
  head_node->name = output_num;
  head_node->type = OUTTEE;
  head_node->label = design->signals[output_num]->name;
  return head_node;
}

// ********************************************************************
// Make node of the type sent and return pointer to the new node.
// ********************************************************************
node* make_fill_node_pred1(node*& head_node, int type_node) {

  head_node->pred1 = make_node();
  head_node->pred1->succ = head_node;
  head_node = head_node->pred1;
  head_node->type = type_node;
  return head_node;
}

// ********************************************************************
// Make node of the type sent and return pointer to the new node.
// Makes successor node from pred2 instead of pred1.
// ********************************************************************
node* make_fill_node_pred2(node*& head_node, int type_node) {

  head_node->pred2 = make_node();
  head_node->pred2->succ = head_node;
  head_node = head_node->pred2;
  head_node->type = type_node;
  return head_node;
}

// ********************************************************************
// High level function for levelizing the decomposition.
// Fills in the level values in the decomposed structure.
// ********************************************************************
int levelize(node* input_vec[], int num_set_inputs, int num_reset_inputs,
	     int num_combo_inputs) {

  int index=0;
  int count=0;
  int tot_num_inputs = num_combo_inputs + num_set_inputs + num_reset_inputs;

  if (num_combo_inputs != 0) {

    // Combinational output
    index = 0;
    do_level(input_vec, num_combo_inputs, index, count);
  }
  else {
    
    // Do set inputs
    index = 0;
    do_level(input_vec, num_set_inputs, index, count);

    // Do reset inputs
    index = num_set_inputs;
    do_level(input_vec, num_reset_inputs, index, count);
  }
  return tot_num_inputs;
}

// ********************************************************************
// Levelize the subject lists. All primary inputs are level 0.
// ********************************************************************
void do_level(node* input_vec[], int num_inputs, int index, int& count) {

  int level_num=0;
  bool temp=true;
  node* head;
  
  for (int i=index;i<(index+num_inputs);i++) {
    head = input_vec[i]->succ;
    level_num = 0;
    temp = true;
    while (temp) {
      if (head->type == INV) {
	if (head->level == INIT_LEVEL) {
	  level_num++;
	  head->counter = count;
	  count++;
	  head->level = level_num;
	  if (head->succ->type == OUTTEE) {
	    head->succ->counter = 999;
	    head->succ->level = level_num+1;
	    temp = false;
	  }
	  head = head->succ;
	}
	else
	  temp = false;
      }
      else if (head->type == NAND2) {
	if (head->level == INIT_LEVEL) {
	  if ((head->pred1->level == INIT_LEVEL) ||
	      (head->pred2->level == INIT_LEVEL))
	    
	    // Stop this chain
	    temp = false;
	  else {
	    
	    // This NAND2 is done
	    // Find out which input has a higher level
	    if (head->pred1->level > head->pred2->level)
	      level_num = head->pred1->level+1;
	    else
	      level_num = head->pred2->level+1;
	    head->counter = count;
	    count++;
	    head->level = level_num;
	    if (head->succ->type == OUTTEE) {
	      head->succ->counter = 999;
	      head->succ->level = level_num+1;
	      temp = false;
	    }
	  }
	  head = head->succ;
	}
	else
	  temp = false;
      }
      else if (head->type == CEL) {
	
	if ((head->pred1->level == INIT_LEVEL) ||
	    (head->pred2->level == INIT_LEVEL))
	  
	  // Stop this chain
	  temp = false;
	else {
	  
	  // This CEL is done
	  // Find out which input has a higher level
	  if (head->pred1->level > head->pred2->level)
	    level_num = head->pred1->level+1;
	  else
	    level_num = head->pred2->level+1;
	  head->counter = count;
	  head->level = level_num;
	  
	  // Set level on output node and stop
	  head->succ->level = level_num+1;
	  head->succ->counter = 999;
	  temp = false;
	}
      }
      else if (head->type == OUTTEE) {

	// Happens only when an INNEE is tied to an OUTTEE
	head->counter = 999;
	head->level = 1;
	temp = false;
      }
    }
  }
}

// ********************************************************************
// Clears the visited field in the decomposed netlist.
// ********************************************************************
void clear_visited_decomp(node* input_vec[], int tot_num_inputs) {

  node* head;
  
  for (int i=0;i<tot_num_inputs;i++) {
    head = input_vec[i]->succ;
    while (head->type != OUTTEE) {
      head->visited = false;
      head = head->succ;
    }
    head->visited = false;
  }
}

// ********************************************************************
// Clears the fields used in matching in the decomposed netlist.
// ********************************************************************
void clear_matching_decomp(node* input_vec[], int tot_num_inputs) {

  node* head;
  
  for (int i=0;i<tot_num_inputs;i++) {
    head = input_vec[i]->succ;
    while (head->type != OUTTEE) {
      head->area_cost = 0;
      head->min_time_cost = 0;
      head->max_time_cost = 0;
      head->hazard_cost = 0;
      head->lib_num = 999;
      head = head->succ;
    }
    head->area_cost = 0;
    head->min_time_cost = 0;
    head->max_time_cost = 0;
    head->hazard_cost = 0;
    head->lib_num = 999;
  }
}

// ********************************************************************
// Free the cover nodes linked lists.
// ********************************************************************
void free_cover_nodes(cover_node* head_set_node,
		      cover_node* head_reset_node) {

  cover_node* front_node;
  cover_node* back_node = head_set_node;

  // Delete set chain
  while (back_node->succ != NULL) {
    front_node = back_node->succ;
    delete(back_node);
    back_node = front_node;
  }
  delete(back_node);

  // Delete reset chain
  back_node = head_reset_node;
  while (back_node->succ != NULL) {
    front_node = back_node->succ;
    delete(back_node);
    back_node = front_node;
  }
  delete(back_node);
}

// ********************************************************************
// Free the decomposition linked list.
// ********************************************************************
void free_decomp(node* input_vec[], int tot_num_inputs) {

  bool flag = true;
  node* go_node;
  
  // Clear the visited field
  clear_visited_decomp(input_vec, tot_num_inputs);

  for (int i=0;i<tot_num_inputs;i++) {
    flag = true;
    go_node = input_vec[i]->succ;

    // Loop until kicked out
    while(flag) {
      if (go_node->type == INV) {
	go_node = go_node->succ;
	if (go_node->visited == false)
	  delete(go_node->pred1);
	else
	  delete(go_node->pred2);
      }
      
      else if ((go_node->type == NAND2) || (go_node->type == CEL)) {
	if (go_node->visited == true) {
	  go_node = go_node->succ;
	  delete(go_node->pred1);
	}
	else {
	  go_node->visited = true;
	  flag = false;
	}
      }
      else {

	// Must be the output node
	delete(go_node);
	flag = false;
      }
    }
  }
}

// ********************************************************************
// Free the covered cells netlist linked list.
// ********************************************************************
void free_netlist(netlist_node* head_netlist_node) {

  netlist_node* next;

  while (head_netlist_node->succ) {
    next=head_netlist_node->succ;
    delete head_netlist_node;
    head_netlist_node=next;
  }
}

// ********************************************************************
// Free the sg node linked list.
// ********************************************************************
void free_sg_node(sg_node* head_sg_node) {

  sg_node* next;

  while (head_sg_node) {
    next=head_sg_node->succ;
    delete head_sg_node;
    head_sg_node=next;
  }
}

// ********************************************************************
// Free the timed node linked list.
// ********************************************************************
void free_timed_node(timed_node* head_timed_node) {

  // Advance to end of list
  while (head_timed_node->succ != NULL)
    head_timed_node = head_timed_node->succ;

  // Delete starting from the last node
  while (head_timed_node->counter != 0) {
    head_timed_node = head_timed_node->pred;
    delete(head_timed_node->succ);
  }
}

// ********************************************************************
// Create a new sg node and put the data into it.
// ********************************************************************
void push_sg(stateADT cur_state1, statelistADT cur_state2,
	     sg_node*& temp_sg_node, int index) {

  temp_sg_node->cur_state = cur_state1;
  temp_sg_node->edge = cur_state2;
  temp_sg_node->index = index;
  temp_sg_node->succ = make_sg_node();
  temp_sg_node->succ->pred = temp_sg_node;
  temp_sg_node->succ->counter = (temp_sg_node->counter)+1;
  temp_sg_node = temp_sg_node->succ;
}

// ********************************************************************
// Allocate memory for a primary input name.
// ********************************************************************
char* mem_name(char* tmp) {

  int j;
  unsigned int x;
  char* name;

  x = strlen(tmp)+1;
  name = new char[x];
  for (j=0;j<int(strlen(tmp)+1);j++)
    strcpy(&name[j],&tmp[j]);
  name[j+1] = (char)NULL;
  return name;
}

// ********************************************************************
// Create a new input entry for a netlist node.
// ********************************************************************
void push_netlist_input(input_node*& temp_input_node, char* name,
			char* lib_letter) {

  temp_input_node->set_name(name);
  temp_input_node->lib_char = lib_letter;
  temp_input_node->succ = make_input_node();
  temp_input_node->succ->pred = temp_input_node;
  temp_input_node->succ->counter = (temp_input_node->counter)+1;
  temp_input_node = temp_input_node->succ;
  temp_input_node->succ = NULL;
}

// ********************************************************************
// Create a new netlist node and put the data into it.
// ********************************************************************
void push_netlist(node* head, netlist_node* netlist_node,
		  char* lib_expr, bool covered, char* output) {

  netlist_node->head = head;
  netlist_node->lib_num = head->lib_num;
  netlist_node->init_value = head->value;
  netlist_node->min = head->min_time_cost;
  netlist_node->max = head->max_time_cost;
  netlist_node->expr = lib_expr;
  netlist_node->covered = covered;
  netlist_node->set_output(output);
  netlist_node->succ = make_netlist_node();
  netlist_node->first = make_input_node();
  netlist_node->succ->counter = (netlist_node->counter)+1;
  netlist_node->succ->pred = netlist_node;
}

// ********************************************************************
// Fill an expression with NULL characters.
// ********************************************************************
void fill_null(char expression[], int length) {

  // Create null expression
  for (int i=0;i<length;i++)
    expression[i] = (char)NULL;
}

// ********************************************************************
// Build an internal library.
// The cells here are built from output to input.
// ********************************************************************
void build_library(lib_node* lib_vec[], int& num_lib_cells) {

#ifndef __READ_EXTERNAL_LIB__
  node* head;
#endif

#ifdef __READ_EXTERNAL_LIB__
#ifndef __NAM__
  printf("Load the library\n");
#endif
load_library("test.genlib", lib_vec, 0.3, num_lib_cells);
// load_library("rich.genlib", lib_vec, 0.1, num_lib_cells);
// load_library("com_inp.genlib", lib_vec, 0.1, num_lib_cells);
// load_library("s_stack.genlib", lib_vec, 0.1, num_lib_cells);
// load_library("combo+.genlib", lib_vec, 0.1, num_lib_cells);
// load_library("simple.genlib", lib_vec, 0.1, num_lib_cells);
// load_library("asynch.genlib", lib_vec, 0.1, num_lib_cells);
 if (num_lib_cells >= MAXNUMLIBCELLS)
   cout << "Increase the MAXNUMLIBCELLS variable in decomp.h" << endl;
#else
  for(int i=0;i<num_lib_cells;i++) {   // Index for library vector
    lib_vec[i] = make_lib_node();
    lib_vec[i]->pred = make_node();
  }

  // head = fill_lib_cell(lib_vec, index, name, num_inputs, num_levels,
  // min_del, max_del, area, first_cell)

  // Inverter
  lib_vec[0]->expr = "~(a)";
  head = fill_lib_cell(lib_vec, 0, "INV", 1, 1, 1, 3, 2, INV);
  head->input1 = "a";

#ifdef __ADD_LIB_REDUND__
  // Alternate Inverter 0 (3 in a row)
  lib_vec[17]->expr = "~(a)";
  head = fill_lib_cell(lib_vec, 17, "INV-0", 1, 3, 1, 3, 2, INV);
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "a";
#endif

  // NAND2
  lib_vec[1]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 1, "NAND2", 2, 1, 2, 4, 4, NAND2);
  head->input1 = "a";
  head->input2 = "b";

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 0 - 2 inverters on output
  lib_vec[18]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 18, "NAND2-0", 2, 3, 2, 4, 4, INV);
  lib_pred1(head, INV);
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";
#endif

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 1 - two inverters on pred1
  lib_vec[19]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 19, "NAND2-1", 2, 3, 2, 4, 4, NAND2);
  head->input2 = "b";
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "a";
#endif

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 2 - two inverters on pred2
  lib_vec[20]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 20, "NAND2-2", 2, 3, 2, 4, 4, NAND2);
  head->input1 = "a";
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  head->input1 = "b";
#endif

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 3 - 2 inverters on each input
  lib_vec[21]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 21, "NAND2-3", 2, 3, 2, 4, 4, NAND2);
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  back_lib_head(head);
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  head->input1 = "b";
#endif

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 4 - 2 inverters on pred1 input, 2 inverters on output
  lib_vec[22]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 22, "NAND2-4", 2, 5, 2, 4, 4, INV);
  lib_pred1(head, INV);
  lib_pred1(head, NAND2);
  head->input2 = "b";
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "a";
#endif

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 5 - 2 inverters on pred2 input, 2 inverters on output
  lib_vec[23]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 23, "NAND2-5", 2, 5, 2, 4, 4, INV);
  lib_pred1(head, INV);
  lib_pred1(head, NAND2);
  head->input1 = "a";
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  head->input1 = "b";
#endif

#ifdef __ADD_LIB_REDUND__
  // NAND2 alternate 6 - 2 inverters on each input, 2 inverters on output
  lib_vec[24]->expr = "~(a&b)";
  head = fill_lib_cell(lib_vec, 24, "NAND2-6", 2, 5, 2, 4, 4, INV);
  lib_pred1(head, INV);
  lib_pred1(head, NAND2);
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  back_lib_head(head);
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  head->input1 = "b";
#endif

  // Zero cost buffer
  lib_vec[2]->expr = "(a)";
  head = fill_lib_cell(lib_vec, 2, "BUF", 1, 2, 0, 0, 0, INV);
//   head = fill_lib_cell(lib_vec, 2, "BUF", 1, 2, 1, 1, 1, INV);
  lib_pred1(head, INV);
  head->input1 = "a";

  // AND2
  lib_vec[3]->expr = "(a&b)";
  head = fill_lib_cell(lib_vec, 3, "AND2", 2, 2, 3, 5, 5, INV);
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";

  // OR2
  lib_vec[4]->expr = "(a|b)";
  head = fill_lib_cell(lib_vec, 4, "OR2", 2, 2, 3, 5, 5, NAND2);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  lib_pred2(head, INV);
  head->input1 = "b";

  // NOR2
  lib_vec[5]->expr = "~(a|b)";
  head = fill_lib_cell(lib_vec, 5, "NOR2", 2, 3, 2, 4, 4, INV);
  lib_pred1(head, NAND2);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  lib_pred2(head, INV);

  head->input1 = "b";

  // OAI21
  lib_vec[6]->expr = "~((a|b)&c)";
  head = fill_lib_cell(lib_vec, 6, "OAI21", 3, 3, 4, 7, 7, NAND2);
  head->input2 = "c";
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  lib_pred2(head, INV);
  head->input1 = "b";

  // NAND3
  lib_vec[7]->expr = "~(a&b&c)";
  head = fill_lib_cell(lib_vec, 7, "NAND3", 3, 3, 3, 6, 5, NAND2);
  head->input2 = "c";
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";

  // AND3
  lib_vec[8]->expr = "(a&b&c)";
  head = fill_lib_cell(lib_vec, 8, "AND3", 3, 4, 4, 8, 7, INV);
  lib_pred1(head, NAND2);
  head->input2 = "c";
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";

  // OR3
  lib_vec[9]->expr = "(a|b|c)";
  head = fill_lib_cell(lib_vec, 9, "OR3", 3, 4, 4, 8, 7, NAND2);
  lib_pred2(head, INV);
  head->input1 = "c";
  back_lib_head(head);
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  lib_pred2(head, INV);
  head->input1 = "b";

  // NOR3
  lib_vec[10]->expr = "~(a|b|c)";
  head = fill_lib_cell(lib_vec, 10, "NOR3", 3, 5, 3, 6, 5, INV);
  lib_pred1(head, NAND2);
  lib_pred2(head, INV);
  head->input1 = "c";
  back_lib_head(head);
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  lib_pred1(head, INV);
  head->input1 = "a";
  back_lib_head(head);
  lib_pred2(head, INV);
  head->input1 = "b";

  // NAND4
  lib_vec[11]->expr = "~(a&b&c&d)";
  head = fill_lib_cell(lib_vec, 11, "NAND4", 4, 5, 4, 8, 7, NAND2);
  head->input2 = "d";
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input2 = "c";
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";

  // NAND4 (alternative)
  lib_vec[12]->expr = "~(a&b&c&d)";
  head = fill_lib_cell(lib_vec, 12, "NAND4ALT", 4, 3, 4, 8, 7, NAND2);
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";
  back_lib_head(head);
  back_lib_head(head);
  lib_pred2(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "c";
  head->input2 = "d";

  // AND4
  lib_vec[13]->expr = "(a&b&c&d)";
  head = fill_lib_cell(lib_vec, 13, "AND4", 4, 6, 5, 9, 9, INV);
  lib_pred1(head, NAND2);
  head->input2 = "d";
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input2 = "c";
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input2 = "b";
  head->input1 = "a";

  // AND4 (alternative)
  lib_vec[14]->expr = "(a&b&c&d)";
  head = fill_lib_cell(lib_vec, 14, "AND4ALT", 4, 4, 5, 9, 9, INV);
  lib_pred1(head, NAND2);
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "a";
  head->input2 = "b";
  back_lib_head(head);
  back_lib_head(head);
  lib_pred2(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "c";
  head->input2 = "d";
  
  // C-element
  lib_vec[15]->expr = "(b&c)|(b&a)|(c&a)";
  head = fill_lib_cell(lib_vec, 15, "CEL", 3, 1, 3, 6, 8, CEL);
  head->input1 = "b";
  head->input2 = "c";

#ifdef __ADD_LIB_REDUND__
  // C-element alternate 0 - 2 inverters on the set input
  lib_vec[25]->expr = "(b&c)|(b&a)|(c&a)";
  head = fill_lib_cell(lib_vec, 25, "CEL-0", 3, 3, 3, 6, 7, CEL);
  head->input2 = "c";
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "b";
#endif

#ifdef __ADD_LIB_REDUND__
  // C-element alternate 1 - 2 inverters on the reset input
  lib_vec[26]->expr = "(b&c)|(b&a)|(c&a)";
  head = fill_lib_cell(lib_vec, 26, "CEL-1", 3, 3, 3, 6, 7, CEL);
  head->input1 = "b";
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  head->input1 = "c";
#endif

#ifdef __ADD_LIB_REDUND__
  // C-element alternate 2 - 2 inverters on each input
  lib_vec[27]->expr = "(b&c)|(b&a)|(c&a)";
  head = fill_lib_cell(lib_vec, 27, "CEL-2", 3, 3, 3, 6, 7, CEL);
  lib_pred1(head, INV);
  lib_pred1(head, INV);
  head->input1 = "b";
  back_lib_head(head);
  back_lib_head(head);
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  head->input1 = "c";
#endif

  // gC-element
  // Fits output x in the ebergen file
  lib_vec[16]->expr = "((b&c)&~(d&e))|(b&c&a)|~(d&e)&a";
  head = fill_lib_cell(lib_vec, 16, "gC_0", 5, 3, 6, 12, 25, CEL);
  lib_pred1(head, INV);
#ifdef __ADD_DOUBLE_INV__
  lib_pred1(head, INV);    // Double inverter
  lib_pred1(head, INV);
#endif
  lib_pred1(head, NAND2);
  head->input1 = "b";
  head->input2 = "c";
  back_lib_head(head);
  back_lib_head(head);
#ifdef __ADD_DOUBLE_INV__
  back_lib_head(head);
  back_lib_head(head);
#endif
  lib_pred2(head, INV);
  lib_pred1(head, INV);
  lib_pred1(head, NAND2);
  head->input1 = "d";
  head->input2 = "e";
  
  // Print all cells
#ifdef __VERBOSE_LIB__
  print_lib_cells(lib_vec, num_lib_cells);
#endif
#endif
}

// ********************************************************************
// Fill a library cell with data.
// ********************************************************************
node* fill_lib_cell(lib_node* lib_vec[], int index, char* name,
		    int num_inputs, int num_levels, int min_del,
		    int max_del, int area, int first_cell) {

  node* head;

  lib_vec[index]->name = name;
  lib_vec[index]->num_inputs = num_inputs;
  lib_vec[index]->num_levels = num_levels;
  lib_vec[index]->min_del = min_del;
  lib_vec[index]->max_del = max_del;
  lib_vec[index]->area = area;
  head = lib_vec[index]->pred;
  head->type = first_cell;
  return head;
}

// ********************************************************************
// Fill a node in a library gate decomposition for a pred1 link.
// ********************************************************************
void lib_pred1(node*& head, int type) {

  head->pred1 = make_node();
  head->pred1->succ = head;
  head = head->pred1;
  head->type = type;
}

// ********************************************************************
// Fill a node in a library gate decomposition for a pred2 link.
// ********************************************************************
void lib_pred2(node*& head, int type) {

  head->pred2 = make_node();
  head->pred2->succ = head;
  head = head->pred2;
  head->type = type;
}

// ********************************************************************
// Back the head pointer up one node in the library cell.
// ********************************************************************
void back_lib_head(node*& head) {

  head = head->succ;
}

// ********************************************************************
// Create a new library node.
// ********************************************************************
lib_node* make_lib_node(void) {

  lib_node* new_node;

  new_node = new lib_node;
  return new_node;
}
