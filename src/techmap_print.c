// *********************************************************************
// Functions to print various information in Curt Nelson's technology
// mapping software, mostly for debugging purposes.
// *********************************************************************

#include "decomp.h"

// *********************************************************************
// Prints the coloring of the value 'color' passed.
// *********************************************************************
void print_color(char color) {

  if (color == 'R')
    cout << "RISING";
  else if (color == 'F')
    cout << "FALLING";
  else if (color == '0')
    cout << "LOW";
  else if (color == '1')
    cout << "HIGH";
  else if (color == 'D')
    cout << "SOFTDOWN";
  else if (color == 'U')
    cout << "SOFTUP";
  else if (color == 'I')
    cout << "INIT";
  else
    cout << color;
}

// ********************************************************************
// Outputs a state graph coloring error from state to state.
// ********************************************************************
void print_coloring_error(designADT design, stateADT state,
			  statelistADT edge, int index) {
  
  cout << "  State = " << state->state << "  Edge = "
       << design->signals[design->events[edge->enabled]->signal]->name;
  print_plus_minus(edge->enabled);
  cout << "  Next state = " << edge->stateptr->state << endl;
}

// ********************************************************************
// Print out a linked list of covers.
// ********************************************************************
void print_cover_list(int num_cov, cover_node* head_node) {

  cout << "covers = " << num_cov << endl;
  if (num_cov != 0) {
    cover_node* current_node = head_node;
    while (current_node->cover != NULL) {
      cout << "Cover = " << current_node->cover
	   << "  Num lits = " << current_node->num_lits << endl;
      current_node = current_node->succ;
    }
  }
}

// ********************************************************************
// Print out the decomposed netlist after the matching step is done.
// ********************************************************************
void print_decomp(designADT design, node* input_vec[], 
		  int tot_num_inputs, int combo) {

  node* head;

  cout << endl << "Printing the decomposition structure ...";
  for (int i=0;i<tot_num_inputs;i++) {
    head = input_vec[i];
    cout << endl;
    do {
      if (head->type == CEL) {
	cout << "CEL\t"
	     << design->signals[head->succ->name]->name
//	     << " xx" << head->counter
// 	     << " val=" << head->value
	     << " lev=" << head->level
	     << " cnt=" << head->counter
 	     << " min=" << head->min
 	     << " max=" << head->max
// 	     << " dmin=" << head->min_time_cost
// 	     << " dmax=" << head->max_time_cost
// 	     << " area=" << head->area_cost
 	     << " lib#=" << head->lib_num
	     << " haz_cost=" << head->hazard_cost
	     << " ahaz=" << head->ahazard
	     << " mhaz=" << head->mhazard << endl;
      }
      else if (head->type == NAND2) {
	cout << "NAND2\t";
	if (head->succ->type == OUTTEE)
	  cout << design->signals[head->succ->name]->name;
	else
	  cout << "xx" << head->counter;
// 	cout  << " val=" << head->value
	cout << " lev=" << head->level
	     << " cnt=" << head->counter
	     << " min=" << head->min
	     << " max=" << head->max
// 	     << " dmin=" << head->min_time_cost
// 	     << " dmax=" << head->max_time_cost
// 	     << " area=" << head->area_cost
	     << " lib#=" << head->lib_num
	     << " haz_cost=" << head->hazard_cost
	     << " ahaz=" << head->ahazard
	     << " mhaz=" << head->mhazard << endl;
      }
      else if (head->type == INV) {
	cout << "INV\t";
	if (head->succ->type == OUTTEE)
	  cout << design->signals[head->succ->name]->name;
	else
	  cout << "xx" << head->counter;
// 	cout  << " val=" << head->value
	cout << " lev=" << head->level
	     << " cnt=" << head->counter
	     << " min=" << head->min
	     << " max=" << head->max;
	if (head->succ->pred1 == head)
	  cout << " p1 ";
	else
	  cout << " p2 ";
// 	     << " dmin=" << head->min_time_cost
// 	     << " dmax=" << head->max_time_cost
// 	     << " area=" << head->area_cost
 	cout << " lib#=" << head->lib_num
	     << " haz_cost=" << head->hazard_cost
	     << " ahaz=" << head->ahazard
	     << " mhaz=" << head->mhazard << endl;
      }
      else if (head->type == OUTTEE) {
	cout << "OUTPUT\t" << design->signals[head->name]->name
// 	     << " val=" << head->value
	     << " lev=" << head->level
	     << " cnt=" << head->counter
 	     << " min=" << head->min
 	     << " max=" << head->max
// 	     << " dmin=" << head->min_time_cost
// 	     << " dmax=" << head->max_time_cost
// 	     << " area=" << head->area_cost
	     << " haz_cost=" << head->hazard_cost
 	     << " lib#=" << head->lib_num
	     << " ahaz=" << head->ahazard
	     << " mhaz=" << head->mhazard << endl;
      }
      else if (head->type == INNEE) {
	cout << "INPUT\t" << design->signals[head->name]->name;
	if (combo)
	  cout << " combo";
	else if (head->set_res)
	  cout << " set";
	else
	  cout << " reset";
// 	cout << " val=" << head->value
	cout << " lev=" << head->level
	     << " cnt=" << head->counter
	     << " min=" << head->min
	     << " max=" << head->max
// 	     << " mmin " << head->min_time_cost
// 	     << " mmax " << head->max_time_cost
//  	     << " lib#=" << head->lib_num
	     << " ahaz=" << head->ahazard
	     << " mhaz=" << head->mhazard << endl;
      }
      head = head->succ;
    }
    while (head != NULL);
  }
  cout << endl;
}

// ********************************************************************
// Find maximum and minimum delays in my decomposition from all inputs
// in the input vector to the output. Entered once per output.
// ********************************************************************
void print_decomp_delays(designADT design, node* input_vec[],
			 int tot_num_inputs, int region_num) {

  node* head;

  int out=((region_num-1)/2);
  cout << "Printing min/max numbers from all inputs to output "
       << design->signals[out]->name << endl;
  for (int i=0;i<tot_num_inputs;i++) {
    head = input_vec[i];
    int mindelay = 0;
    int maxdelay = 0;
    while (head->type != OUTTEE) {
      head = head->succ;
      if (head->type == INV) {
	mindelay = mindelay + INVMINDEL;      
	maxdelay = maxdelay + INVMAXDEL;
      }
      else if (head->type == NAND2) {
	mindelay = mindelay + NAND2MINDEL;      
	maxdelay = maxdelay + NAND2MAXDEL;
      }
      else if (head->type == CEL) {
	mindelay = mindelay + CELMINDEL;      
	maxdelay = maxdelay + CELMAXDEL;
      }
    }
    cout << design->signals[input_vec[i]->name]->name;
    cout << "  " << mindelay << ", " << maxdelay << endl;
  }
}

// *********************************************************************
// Prints the number of events, signals, inputs and the entire events
// list.
// *********************************************************************
void print_events_data_struct(designADT design) {

  cout << "Printing events data structure ...."  << endl;
  cout << "number of events = " << design->nevents << endl;
  cout << "number of signals = " << design->nsignals << endl;
  cout << "number of input signals = " << design->ninpsig << endl;
  for (int i=0;i<design->nevents;i++)
    cout << "index = " << i << "\tevent - " << design->events[i]->event
         << "\tsignal - " << design->events[i]->signal << endl;
  cout << endl;
}

// ********************************************************************
// Prints out all delays from all possible IO's to all nodes for a
// general nodelist.
// ********************************************************************
void print_general_node_delays(nodelistADT nodelist, node* input_vec[],
			       int tot_num_inputs) {

  int min = 0;
  int max = 0;

  cout << "Printing min/max numbers from IO's to Outputs ..." << endl;
  for (nodelistADT curnode=nodelist;curnode;curnode=curnode->link)
    curnode->this_node->visited=false;
  for (int i=0;i<tot_num_inputs;i++) {
    for (nodelistADT curnode=input_vec[i]->succs;curnode;
	 curnode=curnode->link) {
      min = curnode->this_node->lower;
      max = curnode->this_node->upper;
      if (curnode->this_node->type != IO)
	print_node_delays_recurse(curnode, min, max, input_vec[i]->label);
      else
	cout << input_vec[i]->label << "  " << curnode->this_node->label
	     << "  "  << min << ", " << max << endl;
    }
  }
}

// *********************************************************************
// Prints statistics concerning hazards.
// *********************************************************************
void print_hazard_info(int ack_haz_node_vec[], int mono_haz_node_vec[],
		       int total_ack_hazards, int total_mono_hazards,
		       int haz_node_vec[], int& num_unique_haz_nodes,
		       int num_nodes, char* output_name,
		       int mono_haz_node_vec1[]) {
  
  // Number of unique nodes that are ack or mono hazardous
  int num_ack_haz_nodes = 0;
  int num_mono_haz_nodes = 0;
  bool first_pass = true;

  cout << "*******************************************************" << endl;
  cout << "*** Hazard summary for output " << output_name << endl;
  cout << "*** Total number of nodes = " << num_nodes << endl;
  for (int i=0;i<num_nodes;i++) {
    if ((ack_haz_node_vec[i] != 0) || (mono_haz_node_vec1[i] != 0)) {
      num_unique_haz_nodes++;
      haz_node_vec[i] = HAZARDOUS;
      if (ack_haz_node_vec[i] != 0)
	num_ack_haz_nodes++;
      if (mono_haz_node_vec[i] != 0)
	num_mono_haz_nodes++;
    }
  }
  cout << "*** Number of unique hazardous nodes = " << num_unique_haz_nodes;
  if (num_unique_haz_nodes > 0) {
    cout << ": ";
    for (int l=0;l<num_nodes;l++) {
      if (haz_node_vec[l] != 0) {
	if (first_pass) {
	  cout << l;
	  first_pass = false;
	}
	else
	  cout << "," << l;
      }
    }
  }
  cout << endl << "*** Number of ack hazardous nodes = " << num_ack_haz_nodes;
  first_pass = true;
  if (num_ack_haz_nodes > 0) {
    cout << ": ";
    for (int j=0;j<num_nodes;j++) {
      if (ack_haz_node_vec[j] != 0) {
	if (first_pass) {
	  cout << j;
	  first_pass = false;
	}
	else
	  cout << "," << j;
      }
    }
    cout << endl << "*** Total number of ack hazards = "
	 << total_ack_hazards << endl;
  }
  else
    cout << endl;
  cout << "*** Number of mono hazardous nodes = " << num_mono_haz_nodes;
  first_pass = true;
  if (num_mono_haz_nodes > 0) {
    cout << ": ";
    for (int k=0;k<num_nodes;k++) {
      if (mono_haz_node_vec[k] != 0) {
	if (first_pass) {
	  cout << k;
	  first_pass = false;
	}
	else
	  cout << "," << k;
      }
    }
    cout << endl << "*** Total number of mono hazards = "
	 << total_mono_hazards << endl;
  }
  else
    cout << endl;
  cout << "*******************************************************"
       << endl << endl;
}

// ********************************************************************
// Prints out the input vector.
// ********************************************************************
void print_input_vec(designADT design, node* input_vec[],
		     int tot_num_inputs, int combo) {
  
  cout << "Printing input vector ..." << endl;
  for (int i=0;i<tot_num_inputs;i++) {
    cout << "index = " << i
	 << "  input = " << design->signals[input_vec[i]->name]->name;
    if (combo)
      cout << "  combo";
    else if (input_vec[i]->set_res)
      cout << "  set  ";
    else
      cout << "  reset";
    cout << "  value = " << input_vec[i]->value;
    if (input_vec[i] == input_vec[i]->succ->pred1)
      cout << "  pred1" << endl;
    else
      cout << "  pred2" << endl;
  }
  cout << endl;
}

// ********************************************************************
// Recursively print a library cell.
// ********************************************************************
void print_lib_cell_recurse(node* head, bool pred1_flag) {

  if (pred1_flag) {
    cout << "  Pred1..";
    head = head->pred1;
  }
  else {
    cout << "  Pred2..";
    head = head->pred2;
  }
  if (head->type == 0)
    cout << "  INV";
  else if (head->type == 1)
    cout << "  NAND2";
  else if (head->type == CEL)
    cout << "  CEL";
  if (head->pred1 != NULL) {
    pred1_flag = true;
    print_lib_cell_recurse(head, pred1_flag);
    cout << "  UP";
  }
  else {
    cout << "  Input1 = " << head->input1;
//     cout << "  input1_index = " << head->input1_index;
  }
  if (head->pred2 != NULL) {
    pred1_flag = false;
    print_lib_cell_recurse(head, pred1_flag);
    cout << "  UP";
  }
  else if (head->type != INV) {
    cout << "  Input2 = " << head->input2;
//     cout << "  input2_index = " << head->input2_index;
  }
}

// ********************************************************************
// High level routine for printing out a library cell.
// ********************************************************************
void print_lib_cells(lib_node* lib_vec[], int num_lib_cells) {

  bool pred1_flag=true;
  node* head;

//   for (int i=0;i<num_lib_cells;i++) {
//     cout << "Name=" << lib_vec[i]->name
// 	 << "  Inps=" << lib_vec[i]->num_inputs
// 	 << "  Levs=" << lib_vec[i]->num_levels
// 	 << "  Mindel=" << lib_vec[i]->min_del
// 	 << "  Maxdel=" << lib_vec[i]->max_del
// 	 << "  Area=" << lib_vec[i]->area
// 	 << "  Expr=" << lib_vec[i]->expr << endl;
//     head = lib_vec[i]->pred;
    head = lib_vec[num_lib_cells-1]->pred;
    cout << "Output";
    if (head->type == 0)
      cout << "  INV";
    else if (head->type == 1)
      cout << "  NAND2";
    else if (head->type == CEL)
      cout << "  CEL";
    else
      cout << "  Unknown";
    if (head->pred1 != NULL) {
      pred1_flag = true;
      print_lib_cell_recurse(head, pred1_flag);
      cout << "  UP";
    }
    else {
      cout << "  Input1 = " << head->input1;
//       cout << "  input1_index = " << head->input1_index;
    }
    if (head->pred2 != NULL) {
      pred1_flag = false;
      print_lib_cell_recurse(head, pred1_flag);
      cout << "  UP";
    }
    else if (head->type != INV) {
      cout << "  Input2 = " << head->input2;
//       cout << "  input2_index = " << head->input2_index;
    }
    cout << endl;
//   }
}

// ********************************************************************
// Print info about a combinational library gate.
// ********************************************************************
void print_lib_gate(int num_lib_cells, char* name, char* outlab,
		    char* expr, int area, int min_rise, int max_rise,
		    int min_fall, int max_fall, int num_inputs,
		    char** inp_labels, level_exp set_sop) {

  cout << num_lib_cells << ": GATE=" << name
       << " Out=" << outlab << endl;
  cout << "Expr=" << expr << " Area=" << area << " Minrise="
       << min_rise << " Maxrise=" << max_rise << " Minfall="
       << min_fall << " Maxfall=" << max_fall << " NumInps="
       << num_inputs << endl << "Inputs are:";
  for (int i=0;i<num_inputs;i++)
    cout << " " << inp_labels[i];
  cout << endl;

  // Print out functions
  cout << "Library Function is:";
  for (level_exp curlevel=set_sop;curlevel;curlevel=curlevel->next) {
    cout << " " << curlevel->product;
  }
  cout << endl;
}

// ********************************************************************
// Print info about a gc library gate.
// ********************************************************************
void print_lib_gc(int num_lib_cells, char* name, char* outlab,
		  char* nextlab, char* expr, int area, int min_rise,
		  int max_rise, int min_fall, int max_fall,
		  int num_inputs, char** inp_labels, level_exp set_sop,
		  level_exp reset_sop) {

  cout << num_lib_cells << ": LATCH=" << name << " Out=" << outlab
       << " Next=" << nextlab << endl;
  cout << "Set function is:";
  for (level_exp curlevel=set_sop;curlevel;curlevel=curlevel->next) 
    cout << " " << curlevel->product;
  cout << endl << "Reset function is:";
  for (level_exp curlevel=reset_sop;curlevel;curlevel=curlevel->next) 
    cout << " " << curlevel->product;
  cout << endl << "Expr=" << expr << " Area=" << area << " Minrise="
       << min_rise << " Maxrise=" << max_rise << " Minfall="
       << min_fall << " Maxfall=" << max_fall << " NumInps="
       << num_inputs << endl << "Inputs are:";
  for (int j=0;j<num_inputs;j++)
    cout << " " << inp_labels[j];
  cout << endl;
}

// ********************************************************************
// Print the node parameters from the node pointed to by head.
// ********************************************************************
void print_match_info(node* head) {

  cout << "  Area=" << head->area_cost
       << "  Min=" << head->min_time_cost
       << "  Max=" << head->max_time_cost
       << "  Haz=" << head->hazard_cost
       << "  Lib# =" << head->lib_num << endl;
}

// ********************************************************************
// Print out a linked list of lines to be written to the covered
// netlist.
// ********************************************************************
void print_netlist_list(netlist_node* head_netlist_node,
			lib_node* lib_vec[]) {

  cout << endl << "Printing netlist of covering cells ..." << endl;
  netlist_node* current_node = head_netlist_node;
  input_node* cur_input_node = current_node->first;
  while (current_node->succ != NULL) {
    cout << "Cnt = " << current_node->counter
	 << "  Lib = " << lib_vec[current_node->lib_num]->name
	 << "  Init = " << current_node->init_value
	 << "  Min = " << current_node->min
	 << "  Max = " << current_node->max
	 << "  Output = " << current_node->output << endl;
    cout << "  Expr = " << current_node->expr
	 << "  Expr_acc = " << current_node->expr_acc << endl;
    while(cur_input_node->name != NULL) {
      cout << "    Count = " << cur_input_node->counter
	   << "    Name = " << cur_input_node->name
	   << "    Lib_expr = " << cur_input_node->lib_char << endl;
      cur_input_node = cur_input_node->succ;
    }
    current_node = current_node->succ;
    cur_input_node = current_node->first;
  }
}  

// ********************************************************************
// Prints out the input vector.
// ********************************************************************
void print_new_inp_vec(designADT design, node* input_vec[],
		       int tot_num_inputs, int combo) {
  
  cout << "Printing new input vector ..." << endl;
  for (int i=0;i<tot_num_inputs;i++) {
    cout << "index = " << i;
    cout << "  counter = " << input_vec[i]->counter;
    if (input_vec[i]->succ != NULL)
      cout << "  succ = " << input_vec[i]->succ->counter;
    else
      cout << "  succ = NULL";
    if (input_vec[i]->pred1 != NULL)
      cout << "  pred1 = " << input_vec[i]->pred1->counter;
    else
      cout << "  pred1 = NULL";
    if (input_vec[i]->pred2 != NULL)
      cout << "  pred2 = " << input_vec[i]->pred2->counter << endl;
    else
      cout << "  pred2 = NULL" << endl;
  }
  cout << endl;
}

// ********************************************************************
// Print the node parameters from the calculations that were to high to
// update the node costs.
// ********************************************************************
void print_no_match_info(int cell_area_cost, int cell_min_del,
			 int cell_max_del, int cell_haz_cost) {

#ifdef __VERBOSE_MAT__
  cout << "  ... not updating costs because:" << endl;
  cout << "Current info:"
       << "  Area=" << cell_area_cost
       << "  Min=" << cell_min_del
       << "  Max=" << cell_max_del
       << "  Haz=" << cell_haz_cost << endl;
#endif
}

// ********************************************************************
// Recursive part of printing all delays.
// ********************************************************************
void print_node_delays_recurse(nodelistADT node, int min, int max,
			       char* label) {

  int temp_min = min;
  int temp_max = max;

  for (nodelistADT curnode=node->this_node->succs;curnode;
       curnode=curnode->link) {
    min = temp_min;
    max = temp_max;
    if (curnode->this_node->type != IO) {
      min = min + curnode->this_node->lower;
      max = max + curnode->this_node->upper;
      print_node_delays_recurse(curnode, min, max, label);
    }
    else
      cout << label << "  " << curnode->this_node->label
	   << "  "  << min << ", " << max << endl;
  }
}

// ********************************************************************
// Prints a list of all nodes in the nodelist.
// ********************************************************************
void print_nodelist(nodelistADT head) {

  nodelistADT temp_head = head;
  nodelistADT curnode;
  
  cout << "Printing nodelist ..." << endl;
  if (temp_head == NULL)
    cout << "Nodelist is null ..." << endl;
  else {
    cout << "Count=" << temp_head->this_node->counter
	 << " Label=" << temp_head->this_node->label
	 << " Level=" << temp_head->this_node->level
// 	 << " Name=" << temp_head->this_node->name
	 << " Type=" << temp_head->this_node->type
// 	 << " Min=" << temp_head->this_node->min
// 	 << " Max=" << temp_head->this_node->max
// 	 << " Low=" << temp_head->this_node->lower
// 	 << " Up=" << temp_head->this_node->upper;
	 << " haz_cost=" << temp_head->this_node->hazard_cost
	 << " ahaz=" << temp_head->this_node->ahazard
	 << " mhaz=" << temp_head->this_node->mhazard;
    cout << endl;
    for (curnode=temp_head->this_node->succs;curnode;
	 curnode=curnode->link) {
    cout << "  Count=" << curnode->this_node->counter
	 << " Label=" << curnode->this_node->label
	 << " Level=" << curnode->this_node->level
// 	 << " Name=" << curnode->this_node->name
	 << " Type=" << curnode->this_node->type
// 	 << " Min=" << curnode->this_node->min
// 	 << " Max=" << curnode->this_node->max
// 	 << " Low=" << curnode->this_node->lower
// 	 << " Up=" << curnode->this_node->upper;
	 << " haz_cost=" << curnode->this_node->hazard_cost
	 << " ahaz=" << curnode->this_node->ahazard
	 << " mhaz=" << curnode->this_node->mhazard;
      cout << endl;
    }
    for (curnode=temp_head->this_node->preds;curnode;
	 curnode=curnode->link) {
      cout << "    Count=" << curnode->this_node->counter
	 << " Label=" << curnode->this_node->label
	 << " Level=" << curnode->this_node->level
// 	 << " Name=" << curnode->this_node->name
	 << " Type=" << curnode->this_node->type
// 	 << " Min=" << curnode->this_node->min
// 	 << " Max=" << curnode->this_node->max
// 	 << " Low=" << curnode->this_node->lower
// 	 << " Up=" << curnode->this_node->upper;
	 << " haz_cost=" << curnode->this_node->hazard_cost
	 << " ahaz=" << curnode->this_node->ahazard
	 << " mhaz=" << curnode->this_node->mhazard;
      cout << endl;
    }
    while (temp_head->link != NULL) {
      temp_head = temp_head->link;
      cout << "Count=" << temp_head->this_node->counter
	 << " Label=" << temp_head->this_node->label
	 << " Level=" << temp_head->this_node->level
// 	 << " Name=" << temp_head->this_node->name
	 << " Type=" << temp_head->this_node->type
// 	 << " Min=" << temp_head->this_node->min
// 	 << " Max=" << temp_head->this_node->max
// 	 << " Low=" << temp_head->this_node->lower
// 	 << " Up=" << temp_head->this_node->upper;
	 << " haz_cost=" << temp_head->this_node->hazard_cost
	 << " ahaz=" << temp_head->this_node->ahazard
	 << " mhaz=" << temp_head->this_node->mhazard;
      cout << endl;
      for (curnode=temp_head->this_node->succs;curnode;
	   curnode=curnode->link) {
	cout << "  Count=" << curnode->this_node->counter
	 << " Label=" << curnode->this_node->label
	 << " Level=" << curnode->this_node->level
// 	 << " Name=" << curnode->this_node->name
	 << " Type=" << curnode->this_node->type
// 	 << " Min=" << curnode->this_node->min
// 	 << " Max=" << curnode->this_node->max
// 	 << " Low=" << curnode->this_node->lower
// 	 << " Up=" << curnode->this_node->upper;
	 << " haz_cost=" << curnode->this_node->hazard_cost
	 << " ahaz=" << curnode->this_node->ahazard
	 << " mhaz=" << curnode->this_node->mhazard;
	cout << endl;
      }
      for (curnode=temp_head->this_node->preds;curnode;
	   curnode=curnode->link) {
	cout << "    Count=" << curnode->this_node->counter
	 << " Label=" << curnode->this_node->label
	 << " Level=" << curnode->this_node->level
// 	 << " Name=" << curnode->this_node->name
	 << " Type=" << curnode->this_node->type
// 	 << " Min=" << curnode->this_node->min
// 	 << " Max=" << curnode->this_node->max
// 	 << " Low=" << curnode->this_node->lower
// 	 << " Up=" << curnode->this_node->upper;
	 << " haz_cost=" << curnode->this_node->hazard_cost
	 << " ahaz=" << curnode->this_node->ahazard
	 << " mhaz=" << curnode->this_node->mhazard;
	cout << endl;
      }
    }
  }
  cout << endl;
}

// ********************************************************************
// Prints out the output vector.
// ********************************************************************
void print_output_vec(designADT design, node* output_vec[]) {
  
  cout << "Printing output vector ..." << endl;
  for (int i=0;i<(design->nsignals - design->ninpsig);i++)
    cout << "Output = " << output_vec[i]->label
	 << "  name = " << output_vec[i]->name
	 << "  lev = " << output_vec[i]->level
	 << "  cnt = " << output_vec[i]->counter
	 << "  min = " << output_vec[i]->min
	 << "  max = " << output_vec[i]->max
	 << "  type = " << output_vec[i]->type
	 << endl;
  cout << endl;
}

// *********************************************************************
// Prints a plus sign or a minus sign.
// *********************************************************************
void print_plus_minus(int number) {
  
  if ((number % 2) == 1)
    cout << "+";
  else
    cout << "-";
}

// *********************************************************************
// Prints the cover, excitation, and trigger cubes for the current
// region.
// *********************************************************************
void print_region_info(designADT design, regionADT cur_region,
		       char *cover,
		       int region_number, int cover_number) {

  cout << "Cover for " << design->signals[(region_number-1)/2]->name;
  
  // If region_number is odd, must be a set region so print a + sign
  print_plus_minus(region_number);
  cout << cover_number << endl
       << "cover = " << cover
       << "\texcitation = " << cur_region->enstate
       << "\ttrigger = " <<  cur_region->tsignals << endl;
}

// ********************************************************************
// Print rules information.
// ********************************************************************
void print_rules(designADT design) {
  
  cout << "Printing existing rules ..." << endl;
  for (int i=0;i<design->nevents;i++) {
    for (int j=0;j<design->nevents;j++) {
      if (design->rules[i][j]->ruletype != NORULE) {
	cout << "i = " << i << "  j = " << j
	     << "  Lower = " << design->rules[i][j]->lower
	     << "  \tUpper = " << design->rules[i][j]->upper
	     << "  \tRuletype = " << design->rules[i][j]->ruletype << endl;
      }
    }
  }
  cout << endl;
}

// ********************************************************************
// Print a linked list of state graph edges colored 1 or 0.
// ********************************************************************
void print_sg_edges(designADT design, sg_node* top_sg_node) {

  sg_node* node = top_sg_node;

  cout << "Edges stabilized ..." << endl;
  while (node->succ != NULL) {
    cout << "  Edge = " << design->signals[design->events[
			   node->edge->enabled]->signal]->name;
    print_plus_minus(node->edge->enabled);
    cout << "  Color = ";
    print_color(node->edge->colorvec[node->index]);
    cout << "  Next state = " << node->cur_state->state
	 << "  Node = " << node->index << endl;
    node = node->succ;
  }
}

// *********************************************************************
// Prints the list of signals.
// *********************************************************************
void print_signals_list(designADT design) {

  cout << endl << "printing signals list ... " << endl;
  for (int j=0;j<design->nsignals;j++) {
    cout << "Signal = " << design->signals[j]->name << "\t"
	 << "First event index = " << design->signals[j]->event << endl;
  }
  cout << endl;
}

// ********************************************************************
// Prints out zone information for a given state.
// ********************************************************************
int print_state_zones(designADT design, clocklistADT clocklist,
		      bool print_flag) {
  
  int num_events = 0;
  int num_zones = 0;

  // clocklist is a linked list of zones
  while (clocklist != NULL) {
    num_zones++;
    if (print_flag) {
      num_events = clocklist->clocknum;
      for (int i=0;i<num_events;i++) {
	cout << "\t  " << design->events[clocklist->clockkey[i].enabled]->event
	     << " (" << clocklist->clockkey[i].enabled << ")";
      }
      cout << endl;
      for (int i=0;i<num_events;i++)
	cout << "\t ---------";
      cout << endl;
      for (int i=0;i<num_events;i++) {
	cout << design->events[clocklist->clockkey[i].enabled]->event << "\t|";
	for (int j=0;j<num_events;j++) {
	  cout << "    " << clocklist->clocks[i][j] << "\t\t";
	}
	cout << endl;
      }
      cout << endl;
    }
    clocklist=clocklist->next;
  }
  return num_zones;
}

// *********************************************************************
// Prints all states, predecessors, and successors.
// *********************************************************************
void print_states(designADT design) {

  for (int n=0;n<PRIME;n++) {
    for (stateADT cur_state1=design->state_table[n];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      cout << "State = " << cur_state1->state
	   << "  Colorvec = " << cur_state1->colorvec
	   << "  Evalvec = " << cur_state1->evalvec
	   << "  Number = " << cur_state1->number << endl;

      // Go through all successor states
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {

	cout << "  Edge = " << design->signals[design->events[
				    cur_state2->enabled]->signal]->name;
	print_plus_minus(cur_state2->enabled);
	cout << "  Col = " << cur_state2->colorvec
	     << "  NS = " << cur_state2->stateptr->state
	     << "  NScol = " << cur_state2->stateptr->colorvec
	     << endl;
      }

      // Go through all predecessor states
      for (statelistADT cur_state2=cur_state1->pred;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {

	cout << "    Edge = " << design->signals[design->events[
				    cur_state2->enabled]->signal]->name;
	print_plus_minus(cur_state2->enabled);
	cout << "  Col = " << cur_state2->colorvec
	     << "  NS = " << cur_state2->stateptr->state
	     << "  NScol = " << cur_state2->stateptr->colorvec
	     << endl;
      }
    }
  }
  cout << endl;
}

// *********************************************************************
// Prints number of states and edges.
// *********************************************************************
void print_states_edges(designADT design) {

  int num_states=0;
  int num_edges=0;

  for (int n=0;n<PRIME;n++) {
    for (stateADT cur_state1=design->state_table[n];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      num_states++;

      // Go through all successor states
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {

	num_edges++;
      }
    }
  }
  cout << endl;
  cout << "Number of states = " << num_states
       << "    Number of edges = " << num_edges << endl;
}

// *********************************************************************
// Prints all states, predecessors, and successors.
// *********************************************************************
void print_states_col(designADT design, int index) {

  for (int n=0;n<PRIME;n++) {
    for (stateADT cur_state1=design->state_table[n];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      cout << "State = " << cur_state1->state << "  "
	   << cur_state1->colorvec[index] << endl;

      // Go through all successor states
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	cout << "  Edge = " << design->signals[design->events[
				    cur_state2->enabled]->signal]->name;
	print_plus_minus(cur_state2->enabled);
	cout << "  " << cur_state2->colorvec[index] << endl;
      }
    }
  }
  cout << endl;
}

// ********************************************************************
// Outputs where an R->F or F->R transition occured for the node of
// interest. Done in the timed edge stabilization section.
// ********************************************************************
void print_timed_color_rf(designADT design, stateADT state,
			  statelistADT edge) {
  
  cout << " State = " << state->state << "  Edge = "
   << design->signals[design->events[edge->enabled]->signal]->name;
  print_plus_minus(edge->enabled);
  cout << "  Next State = " << edge->stateptr->state << endl;
}

// ********************************************************************
// Prints out zone information.
// ********************************************************************
void print_zone(designADT design, zone_curt* zone) {
  
  int num_events = zone->dim;
  for (int i=0;i<num_events;i++) {
    cout << "\t  " << design->events[zone->key[i]]->event
	 << " (" << zone->key[i] << ")";
  }
  cout << endl;
  for (int i=0;i<num_events;i++)
    cout << "\t ---------";
  cout << endl;
  for (int i=0;i<num_events;i++) {
    cout << design->events[zone->key[i]]->event << "\t|";
    for (int j=0;j<num_events;j++) {
      if (zone->clocks[i][j] == INFIN)
	cout << "    I " << "\t\t";
      else
	cout << "    " << zone->clocks[i][j] << "\t\t";
    }
    cout << endl;
  }
  cout << endl;
}
