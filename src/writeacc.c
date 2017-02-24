// ************************************************************************
// Outputs .acc files to help verify the correctness of the decomposition.
// ************************************************************************

#include "decomp.h"

// ********************************************************************
// Writes one line of boolean expression information to the .acc file
// for a C-element. Equation is (set&reset + set&out + reset&out).
// Done for the .acc file that exposes one node only. Note that the
// reset negation has been removed because an inverter has been pulled
// out of the C-element.
// ********************************************************************

void acc_bool_cel(designADT design, node* head) {
  
  if (head->pred1->type != INNEE) {
    
    // Neither of the inputs are primary
    strcat(head->expr,"(");
    strcat(head->expr,head->pred1->expr);
    strcat(head->expr,"&");
    strcat(head->expr,head->pred2->expr);
    strcat(head->expr,") | (");
    strcat(head->expr,head->pred1->expr);
    strcat(head->expr,"&");
    strcat(head->expr,design->signals[head->succ->name]->name);
    strcat(head->expr,") | (");
    strcat(head->expr,head->pred2->expr);
    strcat(head->expr,"&");
    strcat(head->expr,design->signals[head->succ->name]->name);
    strcat(head->expr,")");
  }
  else {
    
    // If the set input is primary and reset is not
    strcat(head->expr,"(");
    strcat(head->expr,design->signals[head->pred1->name]->name);
    strcat(head->expr,"&");
    strcat(head->expr,head->pred2->expr);
    strcat(head->expr,") | (");
    strcat(head->expr,design->signals[head->pred1->name]->name);
    strcat(head->expr,"&");
    strcat(head->expr,design->signals[head->succ->name]->name);
    strcat(head->expr,") | (");
    strcat(head->expr,head->pred2->expr);
    strcat(head->expr,"&");
    strcat(head->expr,design->signals[head->succ->name]->name);
    strcat(head->expr,")");
  }
}

// ********************************************************************
// Evaluate boolean expressions for an inverter node for the .acc file
// that exposes one node only.
// ********************************************************************
void acc_bool_inv(designADT design, node* head) {

  if (head->pred1->type == INNEE) {
	    
    // The input is a primary input
    strcat(head->expr,"~");
    strcat(head->expr,design->signals[head->pred1->name]->name);
  }
  else {
    
    // Not a primary input
    strcat(head->expr, "~(");
    strcat(head->expr,head->pred1->expr);
    strcat(head->expr,")");
  }
}

// ********************************************************************
// Evaluate boolean expressions for a nand2 node for the .acc file
// that exposes one node only.
// ********************************************************************
void acc_bool_nand2(designADT design, node* head) {

  strcat(head->expr,"~(");
  if ((head->pred1->type != INNEE) && (head->pred2->type != INNEE)) {

    // Neither input to this NAND2 is a primary input
    strcat(head->expr,head->pred1->expr);
    strcat(head->expr,"&");
    strcat(head->expr,head->pred2->expr);
    strcat(head->expr,")");
  }
  else if ((head->pred1->type == INNEE)
	   && (head->pred2->type == INNEE)) {
    
    // Both inputs are primary inputs
    strcat(head->expr,design->signals[head->pred1->name]->name);
    strcat(head->expr,"&");
    strcat(head->expr,design->signals[head->pred2->name]->name);
    strcat(head->expr,")");
  }
  else {
    
    // One input is primary, the other is not
    if (head->pred1->type == INNEE) {
      strcat(head->expr,design->signals[head->pred1->name]->name);
      strcat(head->expr,"&");
      strcat(head->expr,head->pred2->expr);
    }
    else {
      strcat(head->expr,design->signals[head->pred2->name]->name);
      strcat(head->expr,"&");
      strcat(head->expr,head->pred1->expr);
    }
    strcat(head->expr,")");
  }
}

// ********************************************************************
// Build filename for this node.
// ********************************************************************
void build_acc_filename(designADT design, char outname[],
			char node_name[], node* head, int out_num) {

  strcpy(outname,design->filename);
  strcat(outname,"_xx");
  
  // Convert node number to a string
  itoa(node_name, head);
  strcat(outname,node_name);
  strcat(outname,"_");
  strcat(outname,design->signals[(out_num-1)/2]->name);
  strcat(outname,".acc");
#ifdef __VERBOSE_ACC__
  cout << "Storing netlist for node xx" << head->counter
       << ", output " << design->signals[(out_num-1)/2]->name
       << " to " << outname << endl;
#endif
}

// ********************************************************************
// Creates the boolean expressions for all outputs from the regionsADT
// and stores them in the output vector. Called once per decomposition.
// ********************************************************************
void build_acc_out_exps(designADT design, node* output_vec[],
			bool rules_delay) {

  int num_set_cov=0;
  int num_reset_cov=0;
  int num_inputs = design->ninpsig;
  bool combo = false;
  bool first_lit = false;
  node* head_node;
  char expr[MAXEXPRLEN];
  char temp[7];

  // For all outputs, do output lines of the form:
  //   Output = (set&reset) | (set&output) | (reset&output)
  // The i variable is the output_vec index
  for (int i=0;i<(design->nsignals - num_inputs);i++) {

    // Create null expression
    for (int j=0;j<MAXEXPRLEN;j++)
      expr[j] = (char)NULL;
  
    // Find number of set covers
    for (regionADT cur_region=design->regions[(num_inputs*2)+1+2*i];cur_region;
	 cur_region=cur_region->link) {
      if (cur_region->cover) {
	if (cur_region->cover[0] != 'E')
	  num_set_cov++;
      }
    }
    
    // Find number of reset covers
    for (regionADT cur_region=design->regions[(num_inputs*2)+2+2*i];cur_region;
	 cur_region=cur_region->link) {
      if (cur_region->cover) {
	if (cur_region->cover[0] != 'E')
	  num_reset_cov++;
      }
    }
	
    // Is it combinational?
    combo = false;
    if ((num_set_cov == 0) || (num_reset_cov == 0))
      combo = true;
	
    // Print starting information
    strcat(expr,design->signals[num_inputs+i]->name);
    strcat(expr," = ");
    if ((design->startstate[num_inputs+i] == '0') ||
	(design->startstate[num_inputs+i] == 'R'))
      strcat(expr,"0 = [ ");
    else
      strcat(expr,"1 = [ ");
    
    // Get the right index into the signals array
    int l = i + num_inputs;
    int lower = INFIN;
    int upper = 0;

    // Find lower and upper time limits
    if (rules_delay) {
      for (int j=design->signals[l]->event;(design->events[j]->signal==l) &&
	     (j < design->nevents - 1);j++) {
	for (int k=0;k<design->nevents;k++) {
	  if (design->rules[k][j]->ruletype) {
	    if (design->rules[k][j]->lower < lower)
	      lower = design->rules[k][j]->lower;
	    if (design->rules[k][j]->upper > upper)
	      upper = design->rules[k][j]->upper;
	  }
	}
      }
    }
    else {

      // Get delays from the decomposition
      lower = output_vec[i]->min;
      upper = output_vec[i]->max;
    }

    // Build output line
    if (lower == INFIN)
      strcat(expr,"inf");
    else {
      if (lower == 0)
	lower = 1;
      num_to_char(temp, lower);
      strcat(expr,temp);
    }
    strcat(expr," , ");
    if (upper == INFIN)
      strcat(expr,"inf");
    else {
      if (upper == 0)
	upper = 1;
      num_to_char(temp, upper);
      strcat(expr,temp);
    }
    strcat(expr," ] ((");

    // Go straight to set region
    for (regionADT cur_region=design->regions[(num_inputs*2)+1+2*i];cur_region;
	 cur_region=cur_region->link) {
      if (cur_region->cover) {
	if (cur_region->cover[0] != 'E') {
	  
	  // We have a valid cover
	  first_lit = true;
	  for (int j=0;j<design->nsignals;j++) {
	    if (cur_region->cover[j] == '0') {
	      if (first_lit) {
		strcat(expr,"~");
		strcat(expr,design->signals[j]->name);
		first_lit = false;
	      }
	      else {
		strcat(expr,"&~");
		strcat(expr,design->signals[j]->name);
	      }
	    }
	    else if (cur_region->cover[j] == '1') {
	      if (first_lit) {
		strcat(expr,design->signals[j]->name);
		first_lit = false;
	      }
	      else {
		strcat(expr,"&");
		strcat(expr,design->signals[j]->name);
	      }
	    }
	  }
	  num_set_cov--;
	  if (num_set_cov == 0) {
	    if (combo)
	      strcat(expr,"))");
	    else
	      strcat(expr,")) | (~((");
	  }
	  else
	    strcat(expr,") | (");
	}
      }
    }
    
    // Now do reset region
    for (regionADT cur_region=design->regions[(num_inputs*2)+2+2*i];cur_region;
	 cur_region=cur_region->link) {
      if (cur_region->cover) {
	if (cur_region->cover[0] != 'E') {
	  
	  // We have a valid cover
	  first_lit = true;
	  for (int j=0;j<design->nsignals;j++) {
	    if (cur_region->cover[j] == '0') {
	      if (first_lit) {
		strcat(expr,"~");
		strcat(expr,design->signals[j]->name);
		first_lit = false;
	      }
	      else {
		strcat(expr,"&~");
		strcat(expr,design->signals[j]->name);
	      }
	    }
	    else if (cur_region->cover[j] == '1') {
	      if (first_lit) {
		strcat(expr,design->signals[j]->name);
		first_lit = false;
	      }
	      else {
		strcat(expr,"&");
		strcat(expr,design->signals[j]->name);
	      }
	    }
	  }
	  num_reset_cov--;
	  if (num_reset_cov == 0) {
	    if (combo)
	      strcat(expr,"))");
	    else
	      strcat(expr,"))&");
	  }
	  else
	    strcat(expr,") | (");
	}
      }
    }
    
    // Print ending
    if (!combo) {
      strcat(expr,design->signals[num_inputs+i]->name);
      strcat(expr,")");
    }
    head_node = make_output_node(design, output_vec, i, num_inputs+i);
    strcpy(head_node->expr,expr);
  }
}

// ********************************************************************
// Clears the acc_visit field in the decomposed netlist.
// ********************************************************************
void clear_acc_visit(node* input_vec[], int tot_num_inputs) {

  for (int i=0;i<tot_num_inputs;i++) {
    node* head = input_vec[i]->succ;
    while (head->type != OUTTEE) {
      head->acc_visit = false;
      head = head->succ;
    }
    head->acc_visit = false;
  }
}

// ********************************************************************
// Evaluate boolean expressions for each node and place in expr field.
// Done for the acc files that expose one node only. Writes the node
// line and the output line to the .acc file.
// ********************************************************************
void eval_acc_exprs(FILE* fp, designADT design, node* head,
		    int tot_num_inputs, node* input_vec[]) {
  
  int lev=1;
  char temp_expr[MAXEXPRLEN];
  node* temp_head = head;

  // Allow up to 999 nodes (would spew 999 files)
  char node_name[4];

  // Create null expression
  for (int i=0;i<MAXEXPRLEN;i++)
    temp_expr[i] = (char)NULL;
  
  // Clear the visited flags in the decomposed netlist
  clear_visited_decomp(input_vec, tot_num_inputs);

  do {
    for (int i=0;i<tot_num_inputs;i++) {
      temp_head = input_vec[i]->succ;

      // Advance to the correct level
      while (temp_head->level < lev)
	temp_head = temp_head->succ;
      if ((temp_head->level == lev) && (temp_head->visited == false) &&
	  (temp_head->type != OUTTEE)) {
	
	// Null out the expr field
	strcpy(temp_head->expr, temp_expr);
	if (temp_head->type == INV)
	  acc_bool_inv(design, temp_head);
	else if (temp_head->type == NAND2)
	  acc_bool_nand2(design, temp_head);
	else if (temp_head->type == CEL)
	  acc_bool_cel(design, temp_head);
	temp_head->visited = true;
	if (temp_head == head) {
	  
	  // Write node line
	  write_acc_node(fp, design, temp_head);

	  // Rename node->expr to the node designator name
	  strcpy(temp_head->expr, temp_expr);
	  strcat(temp_head->expr,"xx");
	  itoa(node_name, temp_head);
	  strcat(temp_head->expr,node_name);
	}
      }
    }
    lev++;
  }
  while (temp_head->type != OUTTEE);

  // Write output line
  write_acc_output_internal(fp, design, temp_head);
}

// ********************************************************************
// Function evaluates initial values on all nodes.
// ********************************************************************
void eval_acc_inits(designADT design, node* input_vec[],
		    int tot_num_inputs) {

  int lev=1;
  node* head=NULL;

  
  // Evaluate all input initial values
  eval_acc_inputs(design, input_vec, tot_num_inputs);

  // Evaluate initial values on the rest of the decomposition
  do {
    for (int i=0;i<tot_num_inputs;i++) {
      head = input_vec[i]->succ;
      while (head->level < lev)
	head = head->succ;
      if (head->level == lev) {
	if (head->type == INV)
	  if (head->succ->type == OUTTEE) {
	    if ((design->startstate[head->succ->name] == '0') ||
		(design->startstate[head->succ->name] == 'R'))
	      head->value = false;
	    else
	      head->value = true;
	  }
	  else
	    head->value = !(head->pred1->value);
	else if (head->type == NAND2) {
	  if (head->succ->type == OUTTEE) {
	    if ((design->startstate[head->succ->name] == '0') ||
		(design->startstate[head->succ->name] == 'R'))
	      head->value = false;
	    else
	      head->value = true;
	  }
	  else {
	    if ((head->pred1->value == true) &&
		   (head->pred2->value == true))
	      head->value = false;
	    else
	      head->value = true;
	  }
	}
	else if (head->type == CEL) {

	  // CEL output must be the same as initial value of output
	  if ((design->startstate[head->succ->name] == '0') ||
	      (design->startstate[head->succ->name] == 'R'))
	    head->value = false;
	  else
	    head->value = true;
	}
      }
    }
    lev++;
  }
  while (head->type != OUTTEE);
}

// ********************************************************************
// Function evaluates initial values on all input nodes.
// ********************************************************************
void eval_acc_inputs(designADT design, node* input_vec[],
		     int tot_num_inputs) {

  for (int i=0;i<tot_num_inputs;i++) {
    for (int j=0;j<design->nsignals;j++) {
      if (j == input_vec[i]->name) {
	if ((design->startstate[j] == '0') ||
	    (design->startstate[j] == 'R'))
	  input_vec[i]->value = false;
	else
	  input_vec[i]->value = true;
      }
    }
  }
}

// ********************************************************************
// Convert node number to a string. Allows up to 999 nodes.
// ********************************************************************
void itoa(char node_name[], node* head) {

  if (head->counter < 10) {
    node_name[0] = (char)(head->counter + 48);
    node_name[1] = (char)NULL;
  }
  else if (head->counter < 100) {
    node_name[0] = (char)(floor((float)head->counter/10) + 48);
    node_name[1] = (char)((head->counter%10) + 48);
    node_name[2] = (char)NULL;
  }
  else {
    node_name[0] = (char)(floor((float)head->counter/100) + 48);
    node_name[1] = (char)(floor((float)(head->counter%100)/10) + 48);
    node_name[2] = (char)(((head->counter%100)%10) + 48);
    node_name[3] = (char)NULL;
  }
}

// ********************************************************************
// High level routine to write .acc files. Entered once per output.
// Each entry writes a file for the output (out_num) and a file for
// each node within that output structure. It also adds all the nodes
// for that output to the .allnodes.acc file for the entire design.
// ********************************************************************
void make_acc_files(designADT design, node* input_vec[],
		    int out_num, int tot_num_inputs,
		    node* output_vec[], FILE* fpall,
		    int num_nodes_curt) {

  // Make a file for each individual output, exposing all nodes.
  // Called once per output.
  make_all_acc_nodes(design, input_vec, out_num, tot_num_inputs,
		     output_vec, num_nodes_curt);

  // Make one file that exposes all nodes for all outputs
  make_honkin_acc(fpall, design, input_vec, out_num, tot_num_inputs,
		  output_vec, num_nodes_curt);

  // Now make a file for every internal node for this one output
  // Writes num_nodes_curt files
//   make_internal_acc_nodes(design, input_vec, out_num,
// 			  tot_num_inputs, output_vec);
}

// ********************************************************************
// Writes one .acc file with one line for each input, and one line for
// each output. Uses delays calculated from the decomposition.
// ********************************************************************
void make_cge_file_acc(designADT design, node* output_vec[]) {
  
  char outname[FILENAMESIZE];
  FILE* fp;
  
  // Build the output expressions with the correct output delays
  build_acc_out_exps(design, output_vec, false);
  strcpy(outname,design->filename);
  strcat(outname,"_cge.acc");
#ifdef __VERBOSE_ACC__
  cout << "Storing netlist " << outname << endl;
#endif
  fp=fopen(outname,"w");
  if (fp==NULL)
    cout << "ERROR: Unable to open file " << outname << endl;
  fprintf(fp, "# Filename %s\n", outname);

  // Write all input lines
  write_acc_input(fp, design);
  for (int i=0;i<(design->nsignals - design->ninpsig);i++)
    fprintf(fp,"%s\n",output_vec[i]->expr);
  fclose(fp);
}

// ********************************************************************
// Writes one .acc file for all nodes exposed for all outputs.
// ********************************************************************
void make_honkin_acc(FILE* fpall, designADT design, node* input_vec[],
		     int out_num, int tot_num_inputs,
		     node* output_vec[], int num_nodes_curt) {
  
  static int base = 0;

  // Clear the visited fields in the decomposed netlist
  clear_acc_visit(input_vec, tot_num_inputs);
  clear_visited_decomp(input_vec, tot_num_inputs);

  // Evaluate initial values on all nodes and place in value field
  eval_acc_inits(design, input_vec, tot_num_inputs);

  // Write all internal node and output lines
  write_acc_nodes(fpall, design, input_vec, tot_num_inputs,
		  num_nodes_curt, base);
  if (num_nodes_curt > 0)
    base = base + num_nodes_curt - 1;
}

// ********************************************************************
// High level function to write internal node lines to the .acc file.
// Used for writing the file that contains all nodes exposed.
// ********************************************************************
void write_acc_nodes(FILE* fp, designADT design, node* input_vec[],
		     int tot_num_inputs, int num_nodes_curt,
		     int base) {
  
  int lev=1;
  bool stop=false;

  if (num_nodes_curt == 0) {
    fprintf(fp,input_vec[0]->succ->expr);
    fprintf(fp,"\n");
  }
  else {
    while (!stop) {
      for (int i=0;i<tot_num_inputs;i++) {
	node* head = input_vec[i]->succ;
	while (head->level < lev)
	  head = head->succ;
	if (head->level == lev) {
	  if (head->type != OUTTEE) {
	    if (head->visited == false) {
	      if (head->type == INV)
		write_acc_inv(fp, design, head, base);
	      else if (head->type == NAND2)
		write_acc_nand2(fp, design, head, base);
	      else if (head->type == CEL)
		write_acc_cel(fp, design, head, base);
	    }
	    head->visited = true;
	    head = head->succ;
	  }
	  else
	    stop = true;
	}
      }
      lev++;
    }
  }
}

// ********************************************************************
// Writes one line of boolean expression information for an inverter
// to the .acc file that exposes all nodes.
// ********************************************************************
void write_acc_inv(FILE* fp, designADT design, node* head, int base) {

  if (head->succ->type != OUTTEE) {
    fprintf(fp,"xx%i = %i = [ ", head->counter+base, head->value);
    fprintf(fp,"%i , %i ] ", INVMINDEL, INVMAXDEL);
  }
  else {
    fprintf(fp,"%s = ", design->signals[head->succ->name]->name);
    fprintf(fp,"%i = [ ", head->value);
    fprintf(fp,"%i , %i ] ", INVMINDEL, INVMAXDEL);
  }
  if (head->pred1->type == INNEE) {
      
    // The input is a primary input
    fprintf(fp,"~");
    fprintf(fp,"%s\n", design->signals[head->pred1->name]->name);
  }
  else
    fprintf(fp,"~xx%i\n", head->pred1->counter+base);
}

// ********************************************************************
// Writes one line of boolean expression information for a NAND2
// to the .acc file that exposes all nodes.
// ********************************************************************
void write_acc_nand2(FILE* fp, designADT design, node* head, int base) {

  if (head->succ->type != OUTTEE) {
    fprintf(fp,"xx%i = %i = [ ", head->counter+base, head->value);
    fprintf(fp,"%i , %i ] ", NAND2MINDEL, NAND2MAXDEL);
  }
  else {
    fprintf(fp,"%s = ", design->signals[head->succ->name]->name);
    fprintf(fp,"%i = [ ", head->value);
    fprintf(fp,"%i , %i ] ", NAND2MINDEL, NAND2MAXDEL);
  }
  if ((head->pred1->type != INNEE) && (head->pred2->type != INNEE)) {

    // Neither input to this NAND2 is a primary input
    fprintf(fp, "~xx%i | ~xx", head->pred1->counter+base);
    fprintf(fp, "%i\n", head->pred2->counter+base);
  }
  else if ((head->pred1->type == INNEE)
	   && (head->pred2->type == INNEE)) {
    
    // Both inputs are primary inputs
    fprintf(fp,"~");
    fprintf(fp,"%s | ", design->signals[head->pred1->name]->name);
    fprintf(fp,"~");
    fprintf(fp,"%s\n", design->signals[head->pred2->name]->name);
  }      
  else {
    
    // One input is primary, the other is not
    if (head->pred1->type == INNEE) {
      fprintf(fp,"~");
      fprintf(fp,"%s | ~xx", design->signals[head->pred1->name]->name);
      fprintf(fp,"%i\n", head->pred2->counter+base);
    }
    else {
      fprintf(fp,"~");
      fprintf(fp,"%s | ~xx", design->signals[head->pred2->name]->name);
      fprintf(fp,"%i\n", head->pred1->counter+base);
    }
  }
}

// ********************************************************************
// Writes one line of boolean expression information to the .acc file
// for a C-element. Equation is (set&reset + set&out + reset&out).
// Used for the file that exposes all nodes. Note that the reset
// negation has been removed because an inverter has been pulled out
// of the C-element.
// ********************************************************************
void write_acc_cel(FILE* fp, designADT design, node* head, int base) {

  fprintf(fp, "%s = ", design->signals[head->succ->name]->name);
  fprintf(fp,"%i = [ ", head->value);
  fprintf(fp,"%i , %i ] ", CELMINDEL, CELMAXDEL);
  if (head->pred1->type != INNEE) {

    // The SET input is not primary
    fprintf(fp,"(xx%i&", head->pred1->counter+base);
    fprintf(fp,"xx%i) | ", head->pred2->counter+base);
    fprintf(fp,"(xx%i&", head->pred1->counter+base);
    fprintf(fp,"%s) | ", design->signals[head->succ->name]->name);
    fprintf(fp,"(xx%i&", head->pred2->counter+base);
    fprintf(fp,"%s)\n", design->signals[head->succ->name]->name);
  }
  else {
    
    // The SET input is primary
    fprintf(fp,"(%s&", design->signals[head->pred1->name]->name);
    fprintf(fp,"xx%i) | ", head->pred2->counter+base);
    fprintf(fp,"(%s&", design->signals[head->pred1->name]->name);
    fprintf(fp,"%s) | ", design->signals[head->succ->name]->name);
    fprintf(fp,"(xx%i&", head->pred2->counter+base);
    fprintf(fp,"%s)\n", design->signals[head->succ->name]->name);
  }
}

// ********************************************************************
// Writes a .acc file for each output with all nodes exposed.
// Entered once per output.
// ********************************************************************
void make_all_acc_nodes(designADT design, node* input_vec[],
			int out_num, int tot_num_inputs,
			node* output_vec[], int num_nodes_curt) {
  
  static int base=0;
  char outname[FILENAMESIZE];
  FILE* fp;

  strcpy(outname,design->filename);
  strcat(outname,"_");
  strcat(outname,design->signals[(out_num-1)/2]->name);
  strcat(outname,".acc");
#ifdef __VERBOSE_ACC__
  cout << "Storing netlist for output "
       << design->signals[(out_num-1)/2]->name << " to: "
       << outname << endl;
#endif
  fp=fopen(outname,"w");
  if (fp==NULL) {
    cout << "ERROR: Unable to open file " << outname << endl;
  }
  fprintf(fp, "# Filename %s\n", outname);

  // Clear the visited fields in the decomposed netlist
  clear_acc_visit(input_vec, tot_num_inputs);
  clear_visited_decomp(input_vec, tot_num_inputs);

  // Evaluate initial values on all nodes and place in value field
  eval_acc_inits(design, input_vec, tot_num_inputs);

  // Write all input lines
  write_acc_input(fp, design);

  // Write all output lines except the one under consideration
  write_acc_output(fp, design, output_vec, out_num);

  // Write all internal node and output lines
  write_acc_nodes(fp, design, input_vec, tot_num_inputs,
		  num_nodes_curt, base);
  fclose(fp);
  if (num_nodes_curt > 0)
    base = base + num_nodes_curt - 1;
}

// ********************************************************************
// Writes a .acc file for each internal node. Entered once per output.
// ********************************************************************
void make_internal_acc_nodes(designADT design, node* input_vec[],
			     int out_num, int tot_num_inputs,
			     node* output_vec[]) {

  int lev=1;
  char outname[FILENAMESIZE];
  node* head;
  FILE* fp;

  // Allow up to 999 nodes (would spew 999 files)
  char node_name[4];

  // Clear acc_visit field in the decomposed netlist
  clear_acc_visit(input_vec, tot_num_inputs);

  do {
    for (int i=0;i<tot_num_inputs;i++) {
      head = input_vec[i]->succ;
      while (head->level < lev)
	head = head->succ;
      if ((head->level == lev) &&
	  (head->acc_visit == false) &&
	  (head->type != OUTTEE)) {
	    
	// Build file name
	build_acc_filename(design, outname, node_name, head, out_num);
	fp=fopen(outname,"w");
	if (fp==NULL) {
	  cout << "ERROR: Unable to open file " << outname << endl;
	}
	fprintf(fp, "# Filename %s\n", outname);
	
	// Write all input lines
	write_acc_input(fp, design);
	
	// Write all output lines except the one under consideration
	write_acc_output(fp, design, output_vec, out_num);

	// Evaluate boolean expressions and write other two lines, one
	// being the node of interest, the other being the output
	eval_acc_exprs(fp, design, head, tot_num_inputs, input_vec);
	fclose(fp);
	head->acc_visit = true;
      }
    }
    lev++;
  }
  while (head->type != OUTTEE);
}

// ********************************************************************
// Convert number to a string. Allows a number up to 999,999.
// ********************************************************************
void num_to_char(char temp[], int num) {

  if (num < 10) {
    temp[0] = (char)(num + 48);
    temp[1] = (char)NULL;
  }
  else if (num < 100) {
    temp[0] = (char)(floor((float)num/10) + 48);
    temp[1] = (char)((num%10) + 48);
    temp[2] = (char)NULL;
  }
  else if (num < 1000) {
    temp[0] = (char)(floor((float)num/100) + 48);
    temp[1] = (char)(floor((float)(num%100)/10) + 48);
    temp[2] = (char)(((num%100)%10) + 48);
    temp[3] = (char)NULL;
  }
  else if (num < 10000) {
    temp[0] = (char)(floor((float)num/1000) + 48);
    temp[1] = (char)(floor((float)(num%1000)/100) + 48);
    temp[2] = (char)(floor((float)((num%1000)%100)/10) + 48);
    temp[3] = (char)((((num%1000)%100)%10) + 48);
    temp[4] = (char)NULL;
  }
  else if (num < 100000) {
    temp[0] = (char)(floor((float)num/10000) + 48);
    temp[1] = (char)(floor((float)(num%10000)/1000) + 48);
    temp[2] = (char)(floor((float)((num%10000)%1000)/100) + 48);
    temp[3] = (char)(floor((float)(((num%10000)%1000)%100)/10) + 48);
    temp[4] = (char)(((((num%10000)%1000)%100)%10) + 48);
    temp[5] = (char)NULL;
  }
  else if (num < 1000000) {
    temp[0] = (char)(floor((float)num/100000) + 48);
    temp[1] = (char)(floor((float)(num%100000)/10000) + 48);
    temp[2] = (char)(floor((float)((num%100000)%10000)/1000) + 48);
    temp[3] = (char)(floor((float)(((num%100000)%10000)%1000)/100) + 48);
    temp[4] = (char)(floor((float)((((num%100000)%10000)%1000)%100)/10) + 48);
    temp[5] = (char)((((((num%100000)%10000)%1000)%100)%10) + 48);
    temp[6] = (char)NULL;
  }
}

// ********************************************************************
// Writes the input lines to the .acc file.
// ********************************************************************
void write_acc_input(FILE* fp, designADT design) {

  for (int i=0;i<design->ninpsig;i++) {
    int lower = INFIN;
    int upper = 0;
    int lowerf = INFIN;
    int upperf = 0;

    // Find lower and upper times
    for (int j=design->signals[i]->event;design->events[j]->signal==i;j+=2) {
      for (int k=0;k<design->nevents;k++) {
	if (design->rules[k][j]->ruletype) {
	  if (design->rules[k][j]->lower < lower)
	    lower = design->rules[k][j]->lower;
	  if (design->rules[k][j]->upper > upper)
	    upper = design->rules[k][j]->upper;
	}
      }
    }

    // Find lower and upper times
    for (int j=design->signals[i]->event+1;design->events[j]->signal==i;j+=2) {
      for (int k=0;k<design->nevents;k++) {
	if (design->rules[k][j]->ruletype) {
	  if (design->rules[k][j]->lower < lowerf)
	    lowerf = design->rules[k][j]->lower;
	  if (design->rules[k][j]->upper > upperf)
	    upperf = design->rules[k][j]->upper;
	}
      }
    }

    // Output acc line
    fprintf(fp,"%s = ",design->signals[i]->name);
    if ((design->startstate[i] == '0') ||
	(design->startstate[i] == 'R'))
      fprintf(fp,"0 = [ ");
    else
      fprintf(fp,"1 = [ ");
    if (lower == INFIN)
      fprintf(fp,"inf , ");
    else
      fprintf(fp,"%i , ",lower);
    if (upper == INFIN)
      fprintf(fp,"inf");
    else
      fprintf(fp,"%i",upper);
    if (lowerf != lower || upperf != upper) {
      if (lowerf == INFIN)
	fprintf(fp,"; inf , ");
      else
	fprintf(fp,"; %i , ",lowerf);
      if (upperf == INFIN)
	fprintf(fp,"inf");
      else
	fprintf(fp,"%i",upperf);
    }
    fprintf(fp," ] input\n");
  }
}

// ********************************************************************
// Writes one internal node line for a .acc file that exposes one node.
// ********************************************************************
void write_acc_node(FILE* fp, designADT design, node* temp_head) {
  
  if (temp_head->pred1->type != OUTTEE)
    fprintf(fp,"xx%i = ", temp_head->counter);
  else
    fprintf(fp,"%s = ",design->signals[temp_head->pred1->name]->name);
  fprintf(fp,"%i = [ ", temp_head->value);
  fprintf(fp,"%i , %i ] ", temp_head->min, temp_head->max);
  fprintf(fp,"%s\n", temp_head->expr);
}

// ********************************************************************
// Writes the output lines to the .acc file.
// ********************************************************************
void write_acc_output(FILE* fp, designADT design, node* output_vec[],
		      int out_num) {

  for (int i=0;i<(design->nsignals - design->ninpsig);i++) {
    if (output_vec[i]->name != (out_num-1)/2)
      fprintf(fp,"%s\n",output_vec[i]->expr);
  }
}

// ********************************************************************
// Writes one line to a .acc file for an output. Done for the file
// that exposes only one node.
// ********************************************************************
void write_acc_output_internal(FILE* fp, designADT design, node* head) {

  fprintf(fp,"%s = ", design->signals[head->name]->name);
  fprintf(fp,"%i = [ ", head->pred1->value);
  fprintf(fp,"%i , %i ] ", head->min, head->max);
  fprintf(fp,"%s\n", head->pred1->expr);
}
