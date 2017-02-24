// ***************************************************************************
// Functions for untimed hazard checking.
// ***************************************************************************

#include "decomp.h"

// ********************************************************************
// Find edges that can be stabilized speed independently and put
// them in a linked list. Does a depth first search of the state graph.
// Entered once per node.
// ********************************************************************
void high_level_si(designADT design, stateADT start_state,
		   node* input_vec[], node* head, int tot_num_inputs,
		   int region_num, sg_node*& temp_sg_node,
		   bool general, bool va[]) {

  // Create an index into the signals vector
  int out=((region_num-1)/2);
  
  // Pick a successor edge of the start state and recursively cover
  // the entire state graph.
  for (statelistADT cur_state2=start_state->succ;
       (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
       (cur_state2=cur_state2->next)) {

    // Don't try and stabilize an already stable edge
    if ((cur_state2->colorvec[head->counter] != '0') &&
	(cur_state2->colorvec[head->counter] != '1')) {
      if (general) {
	mid_level_si_gen(design, start_state, cur_state2, head,
			 temp_sg_node,
			 design->events[cur_state2->enabled]->signal);
      }
      else {
	
	// Do a tree decomposition netlist for the current output only
	if (design->events[cur_state2->enabled]->signal == out) {
	  mid_level_si_tree(design, start_state, cur_state2, head,
			    temp_sg_node, out); 
	}
      }
    }
    va[start_state->number] = true;
    if (!va[cur_state2->stateptr->number])
      
      // Recurse
      high_level_si(design, cur_state2->stateptr, input_vec, head,
		    tot_num_inputs, region_num, temp_sg_node,
		    general, va);
  }
}

// ********************************************************************
// Checks to see if a path exists between the node being checked and
// the edge under consideration. If so, checks to see that the change
// on the edge was caused by the node changing. If so, colors the 
// edge with a 0 or 1. Entered about (num_nodes x num_edges) times.
// ********************************************************************
void mid_level_si_gen(designADT design, stateADT start_state,
		      statelistADT cur_state2, node* head,
		      sg_node*& temp_sg_node, int out) {

#ifdef __VERBOSE_UNTIMED__
  cout << "Checking node " << head->label << " to output "
       << design->signals[out]->name << endl
       << "Checking edge "
       << design->signals[design->events[cur_state2->enabled]->signal]->name;
  print_plus_minus(cur_state2->enabled);
  cout << " between states " << start_state->state
       << " and " << cur_state2->stateptr->state;
#endif
  if (!(exists_path(head, out))) {
#ifdef __VERBOSE_UNTIMED__
    cout << "..no path." << endl;
#endif
  }
  else {
#ifdef __VERBOSE_UNTIMED__
    cout << "..yes path." << endl;
#endif
    if (!(must_prop_to_out_gen_hl(head, out, start_state))) {
#ifdef __VERBOSE_UNTIMED__
      cout << " ... Sorry! Edge "
	   << design->signals[design->events[cur_state2->enabled]
			      ->signal]->name
	   << " not stabilized." << endl;
#endif
    }
    else {
#ifdef __VERBOSE_UNTIMED__
      cout << " ... Congratulations! Edge "
	   << design->signals[design->events[cur_state2->enabled]
			      ->signal]->name
	   << " stabilized to ";
#endif
      if (cur_state2->colorvec[head->counter] == 'R') {
#ifdef __VERBOSE_UNTIMED__
	cout << "1" << endl;
#endif
	cur_state2->colorvec[head->counter] = '1';    // Stabilized high
	color_pred_vec(cur_state2, head->counter, '1');
      }
      else {
#ifdef __VERBOSE_UNTIMED__
	cout << "0" << endl;
#endif
	cur_state2->colorvec[head->counter] = '0';    // Stabilized low
	color_pred_vec(cur_state2, head->counter, '0');
      }
      
      // Save the edge and the state it is pointing to
      // in a linked list so we know what to
      // check when we propagate the info in the state graph
      push_sg(cur_state2->stateptr, cur_state2, temp_sg_node,
	      head->counter);
    }
  }
}

// *********************************************************************
// High level function to see if the value can be propagated from the
// node of interest to the signal for a general logic block.
// Return true means edge stabilized.
// *********************************************************************
bool must_prop_to_out_gen_hl(node* head, int signal, stateADT start_state) {

  for (nodelistADT cur_node=head->succs;cur_node;
       cur_node=cur_node->link) {
    if (cur_node->this_node->type == IO) {
      if (cur_node->this_node->name == signal)
	return true;
    }
    else {
      if (must_prop_to_out_gen(head, cur_node->this_node, start_state)) {
	if (must_prop_to_out_gen_hl(cur_node->this_node, signal,
				    start_state))
	  return true;
      }
    }
  }
  return false;
}

// *********************************************************************
// See if value can be propagated from the head node to the output for
// a general logic block.
// *********************************************************************
bool must_prop_to_out_gen(node* thenode, node* successor,
			  stateADT start_state) {

  int prodsize;
  bool prod_sat=false;
  char* cube;

  // Build a cube the length of a minterm
  // Cube represents the colors at the inputs to the node following
  // the node of interest (called successor in this routine)
  cube = build_cube(thenode, successor, start_state, prodsize);
  prodsize=strlen(cube);
#ifdef __VERBOSE_UNTIMED__
  cout << "Checking " << thenode->label << " propagating through " 
       << successor->label << " with cube "
       << cube << " evaluation value "
       << start_state->evalvec[successor->counter] << endl;
#endif
  if (start_state->evalvec[successor->counter] == '1') {

    // Check to see if any of the side inputs are blocking
    for (level_exp curprod=successor->SOP;curprod;curprod=curprod->next) {
      prod_sat = true;
      for (int i=0;i<prodsize;i++) {
	if (((cube[i] == '0') && (curprod->product[i] == '1')) ||
	    ((cube[i] == '1') && (curprod->product[i] == '0'))) {
	  prod_sat = false;
	  break;
	}
      }
      if (prod_sat) {
#ifdef __VERBOSE_UNTIMED__
	cout << "Can't propagate 1 because of " << curprod->product
	     << " and cube " << cube << endl;
#endif
	return false;
      }
    }
  }
  else {
    for (level_exp curprod=successor->SOP;curprod;curprod=curprod->next) {
      prod_sat = true;
      for (int i=0;i<prodsize;i++) {
	if (((cube[i] != '0') && (curprod->product[i] == '0')) ||
	    ((cube[i] != '1') && (curprod->product[i] == '1')))
	  prod_sat = false;
      }
      if (prod_sat)
	break;
    }
    if (!prod_sat) {
#ifdef __VERBOSE_UNTIMED__
      cout << "Can't propagate 0" << endl;
#endif
      return false;
    }
  }
#ifdef __VERBOSE_UNTIMED__
  cout << "Can propagate " << thenode->label << " to " << successor->label
       << endl;
#endif
  return true;
}

// *********************************************************************
// Build a cube that represents the colors at the inputs to the node
// following the node of interest.
// *********************************************************************
char* build_cube(node* thenode, node* successor, stateADT cur_state1,
		 int& prodsize) {

  int i=0;
  char* cube;

  prodsize=strlen(thenode->SOP->product);
  cube=(char*)GetBlock(sizeof(char)*(prodsize+1));

  for (nodelistADT cur_node=successor->preds;cur_node;
       cur_node=cur_node->link) {
    if (cur_node->this_node->type == IO) {
      if ((cur_state1->state[cur_node->this_node->name] == '0') ||
	  (cur_state1->state[cur_node->this_node->name] == 'R'))
	cube[i] = '0';
      else
	cube[i] = '1';
    }
    else if (cur_node->this_node == thenode) {
      if (cur_state1->colorvec[cur_node->this_node->counter] == 'R')
	cube[i] = '0';
      else if (cur_state1->colorvec[cur_node->this_node->counter] == 'F')
	cube[i] = '1';
      else 
	cube[i]=cur_state1->colorvec[cur_node->this_node->counter];
    }
    else
      cube[i] = cur_state1->colorvec[cur_node->this_node->counter];
    i++;
  }
  cube[i]='\0';
  return cube;
}

// ********************************************************************
// For a tree decomposition, checks to see if the output changes
// from R->1 or F->0. If so, checks to see if node value can propagate
// to the output. If so, colors the edge with 0 or 1.
// Entered once per node.
// ********************************************************************
void mid_level_si_tree(designADT design, stateADT start_state,
		       statelistADT cur_state2, node* head,
		       sg_node*& temp_sg_node, int out) {

  bool r_to_1 = false;
  bool f_to_0 = false;

#ifdef __VERBOSE_UNTIMED__
  cout << "Checking edge "
       << design->signals[design->events[cur_state2->enabled]->signal]->name;
  print_plus_minus(cur_state2->enabled);
  cout << " between states " << start_state->state
       << " and " << cur_state2->stateptr->state << endl;
#endif
  r_to_1 = ((start_state->state[out] == 'R') &&
	    (cur_state2->stateptr->state[out] == '1'));
  f_to_0 = ((start_state->state[out] == 'F') &&
	    (cur_state2->stateptr->state[out] == '0'));
  
  // If there is a R -> 1 OR a F -> 0 between these two states
  if ((r_to_1) || (f_to_0)) {
#ifdef __VERBOSE_UNTIMED__
    if (r_to_1)
      cout << "  Found R->1 between states " << start_state->state;
    else
      cout << "  Found F->0 between states " << start_state->state;
    cout << " and " << cur_state2->stateptr->state << endl;
#endif
    if (!(must_prop_to_out(start_state, head, out))) {
#ifdef __VERBOSE_UNTIMED__
      cout << " ... Sorry! Edge "
	   << design->signals[design->events[cur_state2->enabled]
			      ->signal]->name;
      print_plus_minus(cur_state2->enabled);
      cout << " not stabilized." << endl;
#endif
    }
    else {
#ifdef __VERBOSE_UNTIMED__
      cout << " ... Congratulations! Edge "
	   << design->signals[design->events[cur_state2->enabled]
			      ->signal]->name;
      print_plus_minus(cur_state2->enabled);
      cout << " stabilized to ";
#endif
      if (cur_state2->colorvec[head->counter] == 'R') {
#ifdef __VERBOSE_UNTIMED__
	cout << "1" << endl;
#endif
	cur_state2->colorvec[head->counter] = '1';    // Stabilized high
	color_pred_vec(cur_state2, head->counter, '1');
      }
      else {
#ifdef __VERBOSE_UNTIMED__
	cout << "0" << endl;
#endif
	cur_state2->colorvec[head->counter] = '0';    // Stabilized low
	color_pred_vec(cur_state2, head->counter, '0');
      }
      
      // Save the edge and the state it is pointing to
      // in a linked list so we know what to
      // check when we propagate the info in the state graph
      push_sg(cur_state2->stateptr, cur_state2, temp_sg_node,
	      head->counter);
    }
  }
}

// *********************************************************************
// See if value can be propagated from the head node to the output for
// a NAND2/INV configuration. Return true if head can propogate to out.
// *********************************************************************
bool must_prop_to_out(stateADT start_state, node* head, int out) {
  
  node* head_temp = head;
  node* my_pred;

  // Does the specific NAND2 / INV structure
  while ((head_temp->succ->type != OUTTEE) &&
	 (head_temp->succ->type != CEL)) {
    
#ifdef __VERBOSE_UNTIMED__
    cout << "    Checking " << head_temp->succ->label << endl;
#endif
    // Flag which input to the NAND2 I'm coming in on
    if (head_temp->succ->type == NAND2) {
      if (head_temp == head_temp->succ->pred1) my_pred=head_temp->succ->pred2;
      else my_pred=head_temp->succ->pred1;
      if (start_state->evalvec[head_temp->succ->counter]=='1') {
	if (my_pred->type == INNEE) {
	  if ((start_state->state[my_pred->name] == '0') ||
	      (start_state->state[my_pred->name] == 'R')) {
#ifdef __VERBOSE_UNTIMED__
	    cout << "     Blocking side 1" << endl;
#endif
	    // Blocking side input
	    return false;
	  }
	}
	else if (start_state->colorvec[my_pred->counter] != '1') {  
	  // Blocking side input
#ifdef __VERBOSE_UNTIMED__
	  cout << "     Blocking side 2" << endl;
#endif
	  return false;
	}
      } 
    }
    head_temp = head_temp->succ;
  }
  return true;
}

// ********************************************************************
// Return true if a path exists between node1 and signal for a general
// circuit.
// ********************************************************************
bool exists_path(node* node1, int signal) {

  for (nodelistADT cur_node=node1->succs;cur_node;
       cur_node=cur_node->link) {
    if (cur_node->this_node->type == IO) {
      if (cur_node->this_node->name == signal)
	return true;
    }
    else {
      if (exists_path(cur_node->this_node, signal))
	return true;
    }
  }
  return false;
}

// ********************************************************************
// High level function propagating the coloring information throughout
// the state graph.
// ********************************************************************
bool propagate_colors(designADT design, sg_node* head_sg_node,
		      int num_states) {

  int modified = 0;
  bool va[num_states];
  sg_node* temp_sg_node;
  
  // Propagate the coloring information throughout the state graph.
  // The sg_node list contains a linked list of all the edges
  // stabilized for the current node in the nodelist. The list has
  // empty first and last elements.
  temp_sg_node = head_sg_node->succ;
  while (temp_sg_node->succ != NULL) {
    if ((temp_sg_node->edge->colorvec[temp_sg_node->index]=='1') ||
	(temp_sg_node->edge->colorvec[temp_sg_node->index]=='0')) {

      // Clear the visited array
      clear_va(va, num_states);

      // Continue coloring if true returned
      if (color_next_state(temp_sg_node->edge, temp_sg_node->index,
			   modified)) {
	va[temp_sg_node->edge->stateptr->number] = true;
	prop_sg(temp_sg_node->cur_state, temp_sg_node->index, va, modified);
      }
    }
    temp_sg_node = temp_sg_node->succ;
  }
  if (modified == 0)
    return false;
  else
    return true;
}

// ********************************************************************
// Recursive function to propagate the coloring info throughout the
// state graph.
// ********************************************************************
void prop_sg(stateADT cur_state, int index, bool va[], int& modified) {
  
  // Go through all successor edges of cur_state
  for (statelistADT cur_state2=cur_state->succ;
       (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
       (cur_state2=cur_state2->next)) {
    if (!va[cur_state2->stateptr->number]) {      
      if (color_next_state(cur_state2, index, modified)) {
	va[cur_state2->stateptr->number] = true;

	// Recurse down this line of states
	prop_sg(cur_state2->stateptr, index, va, modified);
      }
    }
  }
}

// ********************************************************************
// Colors the next state in the state graph. Return false means stop
// coloring.
// ********************************************************************
bool color_next_state(statelistADT edge, int index, int& modified) {
  
  if ((edge->colorvec[index] == '1') ||
      (edge->colorvec[index] == 'U')) {
    if ((edge->colorvec[index] == '1') &&
	(all_in_edge_ok(edge, '1', index))) {
      
      // Propagate color from preceding edge to next state
      if (edge->stateptr->colorvec[index] != '1') {
	edge->stateptr->colorvec[index] = '1';
	modified = 1;
      }
      
      // Go through all successor edges of cur_state
      for (statelistADT cur_state2=edge->stateptr->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	if ((cur_state2->colorvec[index] == 'R') ||
	    (cur_state2->colorvec[index] == 'U')) {
	  cur_state2->colorvec[index] = '1';
	  color_pred_vec(cur_state2, index, '1');
	  modified = 1;
	}
      }
    }
    else if (all_in_edge_ok(edge, 'U', index)) {
      
      // Propagate color from preceding edge
      if (edge->stateptr->colorvec[index] != 'U') {
	edge->stateptr->colorvec[index] = 'U';
	modified = 1;
      }
      
      // Go through all successor edges of cur_state
      for (statelistADT cur_state2=edge->stateptr->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next)) {
	if (cur_state2->colorvec[index] == 'R') {
	  cur_state2->colorvec[index] = 'U';
	  color_pred_vec(cur_state2, index, 'U');
	  modified = 1;
	}
      }
    }
    else
      
      // All incoming edges are not ok. Stop coloring.
      return false;
  }
  else if ((edge->colorvec[index] == '0') &&
	   (all_in_edge_ok(edge, '0', index))) {
    
    // Propagate color from preceding edge
    if (edge->stateptr->colorvec[index] != '0') {
      edge->stateptr->colorvec[index] = '0';
      modified = 1;
    }
    
    // Go through all successor edges of cur_state
    for (statelistADT cur_state2=edge->stateptr->succ;
	 (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	 (cur_state2=cur_state2->next)) {
      if ((cur_state2->colorvec[index] == 'F') ||
	  (cur_state2->colorvec[index] == 'D')) {
	cur_state2->colorvec[index] = '0';
	color_pred_vec(cur_state2, index, '0');
	modified = 1;
      }
    }
  }
  else if (all_in_edge_ok(edge, 'D', index)) {
    
    // Propagate color from preceding edge
    if (edge->stateptr->colorvec[index] != 'D') {
      edge->stateptr->colorvec[index] = 'D';
      modified = 1;
    }
    
    // Go through all successor edges of cur_state
    for (statelistADT cur_state2=edge->stateptr->succ;
	 (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	 (cur_state2=cur_state2->next)) {
      if (cur_state2->colorvec[index] == 'F') {
	cur_state2->colorvec[index] = 'D';
	color_pred_vec(cur_state2, index, 'D');
	modified = 1;
      }
    }
  }
  else
    
    // All incoming edges are not ok. Stop coloring.
    return false;
  return true;
}

// ********************************************************************
// Checks a state to see if it is the value can be propagated. Then,
// checks all incoming edges to a state. Returns true if the next
// state can be colored.
// ********************************************************************
bool all_in_edge_ok(statelistADT edge, int value, int index) {

  // Check states first
  if ((value == '1') &&
      (edge->stateptr->colorvec[index] != 'R') &&
      (edge->stateptr->colorvec[index] != '1') &&
      (edge->stateptr->colorvec[index] != 'U'))
    return false;
  else if ((value == 'U') &&
	   (edge->stateptr->colorvec[index] != 'R') &&
	   (edge->stateptr->colorvec[index] != 'U'))
    return false;
  else if ((value == '0') &&
	   (edge->stateptr->colorvec[index] != 'F') &&
	   (edge->stateptr->colorvec[index] != '0') &&
	   (edge->stateptr->colorvec[index] != 'D'))
    return false;
  else if ((value == 'D') &&
	   (edge->stateptr->colorvec[index] != 'F') &&
	   (edge->stateptr->colorvec[index] != 'D'))
    return false;
  
  // Go through all predecessors of the state pointed to by edge
  for (statelistADT cur_state2 = edge->stateptr->pred;
       cur_state2 != NULL && cur_state2->stateptr->state != NULL;
       cur_state2 = cur_state2->next) {
    if (value == '1') {

      // Incoming edge colorings must be UP or SOFTUP or stop
      if ((cur_state2->colorvec[index] != '1') &&
	  (cur_state2->colorvec[index] != 'U'))
	return false;
    }
    else if (value == 'U') {

      // Incoming edge colorings must be RISING, UP, or SOFTUP
      if ((cur_state2->colorvec[index] != 'R') &&
	  (cur_state2->colorvec[index] != '1') &&
	  (cur_state2->colorvec[index] != 'U'))
	return false;
    }
    else if (value == '0') {
      
      // Incoming edge colorings must be DOWN or SOFTDOWN
      if ((cur_state2->colorvec[index] != '0') &&
	  (cur_state2->colorvec[index] != 'D'))
	return false;
    } 
    else if (value == 'D') {

      // Incoming edge coloring must be FALLING, DOWN, or SOFTDOWN
      if ((cur_state2->colorvec[index] != 'F') &&
	  (cur_state2->colorvec[index] != '0') && 
	  (cur_state2->colorvec[index] != 'D'))
	return false;
    } 
  }
  return true;
}

// ********************************************************************
// Get a token from an expression.
// ********************************************************************
int egettok(char* expr, char* tokvalue, int maxtok, int* cur_char) {

  int c;           // c is the character to be read in
  int toklen;      // toklen is the length of the toklen
  bool readword;   // True if token is a word

  readword = false;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=expr[*cur_char]) != '\0') {
    (*cur_char)++;
    switch (c) {
    case '[': 
    case '(':
    case '~':
    case '&':
    case ')':
    case '|':
    case ']':
      if (!readword)
	return c;
      else {
	(*cur_char)--;
	return WORD;
      }
      break;
    case ' ':
      if (readword)
	return WORD;
      break;
    default:
      if (toklen < maxtok) {
	readword=true;
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
      }
      break;
    }
  }
  if (!readword)
    return '\0';
  else
    return WORD;
}

// ********************************************************************
// Determine the size of a product term.
// ********************************************************************
int prod_size(node* curnode) {

  int cnt=0;

  for (nodelistADT curlist=curnode->preds;curlist;curlist=curlist->link)
    cnt++;
  return cnt;
}

// ********************************************************************
// Extend the size of a product term.
// ********************************************************************
void extend_size(level_exp level,int prodsize) {

  int i;
  level_exp curlevel;

  for (curlevel=level;curlevel;curlevel=curlevel->next) {
    for (i=strlen(curlevel->product);i<prodsize;i++)
      curlevel->product[i]='X';
    curlevel->product[i]='\0';
  }
}

// ********************************************************************
// Parse general circuit into a netlist.
// ********************************************************************
bool d_U(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level) {

  int j;
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;
  // node* new_node;
  nodelistADT curlist,curlist2,newlist;
  int pred,prodsize;

  switch (*token) {
  case WORD:
    if (strcmp(tokvalue,"false")==0) {
      (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
      return true;
    }
    newlevel=(level_exp)GetBlock(sizeof(*newlevel));
    newlevel->next=(*level);
    (*level)=newlevel;
    newlevel->product=(char *)GetBlock((MAXFANIN+1) * sizeof(char));
    prodsize=prod_size(curnode);
    for (j=0;j<prodsize;j++)
      newlevel->product[j]='X';
    newlevel->product[j]='\0';
    if (strcmp(tokvalue,"true")==0) {
      (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
      return true;
    }
    if (strncmp(tokvalue,"maybe",5)==0) {
      for (j=0;j<prodsize;j++)
	newlevel->product[j]='-';
      newlevel->product[j]='\0';
      (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
      return true;
    }
    for (curlist=nodelist;curlist;curlist=curlist->link)
      if (strcmp(curlist->this_node->label,tokvalue)==0) break;
    if (!curlist) {
      printf("ERROR:%d: %s is not found!\n",*linenum,tokvalue);
      fprintf(lg,"ERROR:%d: %s is not found!\n",*linenum,tokvalue);
      return false;
      /* THIS CODE IS USED TO ADD NODES IN INTERNAL FEEDBACK LOOPS
      new_node=new node;
      new_node->label = CopyString(tokvalue);
      new_node->type = GATEE;
      new_node->SOP = NULL;
      new_node->succs = NULL;
      new_node->preds = NULL;
      for (curlist=nodelist;curlist->link;curlist=curlist->link);
      newlist=(nodelistADT)GetBlock(sizeof (*(nodelist)));
      newlist->this_node=new_node;
      newlist->link=NULL;
      curlist->link=newlist;
      curlist=newlist;
      */
    }
    newlist=(nodelistADT)GetBlock(sizeof (*(nodelist)));
    newlist->this_node=curlist->this_node;
    newlist->link=NULL;
    pred=0;
    if (curnode->preds) {
      pred=(-1);
      j=0;
      for (curlist2=curnode->preds;curlist2->link;curlist2=curlist2->link){
	if (curlist2->this_node==curlist->this_node)
	  break;
	j++;
      }
      if (curlist2->this_node!=curlist->this_node) {
	pred=j+1;
	curlist2->link=newlist;
      }
      else 
	pred=j;
    }
    else {
      curnode->preds=newlist;
    }
    newlist=(nodelistADT)GetBlock(sizeof (*(nodelist)));
    newlist->this_node=curnode;
    newlist->link=NULL;
    if (curlist->this_node->succs) {
      for (curlist2=curlist->this_node->succs;curlist2->link;
	   curlist2=curlist2->link)
	if (curlist2->this_node==curnode)
	  break;
      if (curlist2->this_node!=curnode)
	curlist2->link=newlist;
    }
    else {
      curlist->this_node->succs=newlist;
    }
    if (pred<prodsize)
      newlevel->product[pred]='1';
	else {
	  newlevel->product[pred]='1';
	  newlevel->product[pred+1]='\0';
	  for (curlevel=newlevel->next;curlevel;curlevel=curlevel->next) {
	    curlevel->product[pred]='X';
	    curlevel->product[pred+1]='\0';
	  }
	}
    (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
    break;
  case '(':
    (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!d_R(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    if ((*token) != ')') {
      printf("ERROR:%d:  Expected a )!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expected a )!\n",*linenum);
    }
    (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
    break;
  default:
    printf("ERROR:%d:  Expected a signal or a (!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal or a (!\n",*linenum);
    return false;
  }
  return true;
}

// ********************************************************************
// Parse general circuit into a netlist.
// ********************************************************************
bool d_T(int* token, char tokvalue[MAXTOKEN],int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level) {

  level_exp newlevel=NULL;
  level_exp curlevel=NULL;
  char * product;
  int j,prodsize;

  switch (*token) {
  case WORD:
  case '(':
    if (!d_U(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    break;
  case '~':
    (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!d_U(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    prodsize=prod_size(curnode);
    product=(char *)GetBlock((MAXFANIN+1) * sizeof(char));
    for (j=0;j<prodsize;j++)
      product[j]='X';
    product[j]='\0';
    newlevel=invert_expr(product,(*level),NULL,prodsize);
    free(product);
    curlevel=(*level);
    while (curlevel) {
      curlevel=(*level)->next;
      /* PROBLEM HERE WITH FIR5_2mul_csc.atacs file
      if ((*level)->product)
	free((*level)->product);
      free(*level);
      */
      (*level)=curlevel;
    }
    (*level)=newlevel;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return false;
  }
  return true;
}

// ********************************************************************
// Parse general circuit into a netlist.
// ********************************************************************
bool d_C(int* token, char tokvalue[MAXTOKEN],int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level) {

  level_exp oldlevel=NULL;
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;
  int prodsize;

  switch (*token) {
  case '&':
    (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!d_T(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,&newlevel)) 
      return false;
    oldlevel=(*level);
    prodsize=prod_size(curnode);
    extend_size((*level),prodsize);
    (*level)=and_expr((*level),newlevel,prodsize,linenum);
    curlevel=oldlevel;
    while (curlevel) {
      curlevel=oldlevel->next;
      /* PROBLEM HERE WITH FIR5_2mul_csc.atacs file
      if (oldlevel->product) 
    	free(oldlevel->product);
      free(oldlevel);
      */
      oldlevel=curlevel;
    }
    curlevel=newlevel;
    while (curlevel) {
      curlevel=newlevel->next;
      if (newlevel->product) 
    	free(newlevel->product);
      free(newlevel);
      newlevel=curlevel;
    }
    if (!d_C(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    break;
  case '|':
  case ')':
  case '\0':
    break;
  default:
    printf("ERROR:%d:  Expected a |, &, ), or a ]!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a |, &, ), or a ]!\n",*linenum);
    return false;
  }
  return true;
}

// ********************************************************************
// Parse general circuit into a netlist.
// ********************************************************************
bool d_B(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level) {

  level_exp newlevel=NULL;
  int prodsize;

  switch (*token) {
  case '|':
    (*token)=egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!d_S(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,&newlevel)) 
      return false;
    prodsize=prod_size(curnode);
    extend_size((*level),prodsize);
    (*level)=or_expr((*level),newlevel,prodsize,linenum);
    if (!d_B(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    break;
  case ')':
  case '\0':
    break;
  default:
    printf("ERROR:%d:  Expected a |, ), or a ]!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a |, ), or a ]!\n",*linenum);
    return false;
  }
  return true;
}

// ********************************************************************
// Parse general circuit into a netlist.
// ********************************************************************
bool d_S(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level) {

  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!d_T(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    if (!d_C(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return false;
  }
  return true;
}

// ********************************************************************
// Parse general circuit into a netlist.
// ********************************************************************
bool d_R(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level) {

  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!d_S(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    if (!d_B(token,tokvalue,cur_char,linenum,expr,nodelist,curnode,level)) 
      return false;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return false;
  }
  return true;
}

// ********************************************************************
// Load gates from an .acc file.
// ********************************************************************
bool load_net(FILE* fp, char* filename, signalADT* signals, int ninpsig,
	      int nsignals, node* wire_vec[], int* linenum,
	      nodelistADT nodelist, int& num_nodes) {

  char tokvalue[MAXTOKEN];
  char signal[MAXTOKEN];
  int k,initvalue,lower,upper,lowerf,upperf;
  int token;
  char expr[MAXTOKEN];
  node* new_node;
  nodelistADT newlist=NULL;
  nodelistADT oldlist=NULL;
  nodelistADT curlist=NULL;
  int cnt = 0;
  int cur_char;

  token=WORD;
  token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
  while (token != END_OF_FILE) {

    // Get signal name
    strcpy(signal,tokvalue);
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!='=') {
      printf("ERROR:%d:  Expecting a '='!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a '='!\n",*linenum);
      return false;
    }
    // Get initial value
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    initvalue = atoi(tokvalue);
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!='=') {
      printf("ERROR:%d:  Expecting a '='!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a '='!\n",*linenum);
      return false;
    }
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!='[') {
      printf("ERROR:%d:  Expecting a '['!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a '['!\n",*linenum);
      return false;
    }
    // Get lower bound
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    lower = atoi(tokvalue);
    lowerf = lower;
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!=',') {
      printf("ERROR:%d:  Expecting a ','!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a ','!\n",*linenum);
      return false;
    }
    // Get upper bound
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (strcmp(tokvalue,"inf")==0)
      upper = INFIN;
    else
      upper = atoi(tokvalue);
    upperf = upper;
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token==';') {
      /* Get falling lower bound */
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
      lowerf = atoi(tokvalue);
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
      if (token!=',') {
	printf("ERROR:%d:  Expecting a ','!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Expecting a ','!\n",*linenum);
	return FALSE;
      }
      /* Get falling upper bound */
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
      if (strcmp(tokvalue,"inf")==0)
	upperf = INFIN;
      else
	upperf = atoi(tokvalue);
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    }
    if (token!=']') {
      printf("ERROR:%d:  Expecting a ']'!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a ']'!\n",*linenum);
      return false;
    }
    // Get expression
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    expr[0]='\0';
    while (token != END_OF_LINE && token != END_OF_FILE) {
      if (token==WORD)
	strcat(expr,tokvalue);
      else {
	int len=strlen(expr);
	expr[len]=token;
	expr[len+1]='\0';
      }
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    }

    // If not input, generate node for gate
    if (strcmp(expr,"input")!=0) {
      for (oldlist=nodelist;oldlist->link;oldlist=oldlist->link)
	if ((oldlist->this_node->type==GATEE) && 
	    (strcmp(oldlist->this_node->label,signal)==0)) break;
      if ((oldlist->this_node->type!=GATEE) ||
	  (strcmp(oldlist->this_node->label,signal)!=0))  
	new_node=new node;
      else 
	new_node=oldlist->this_node;
      new_node->label = CopyString(signal);
      new_node->type = GATEE;
      new_node->counter = cnt++;
      new_node->Bexpr = CopyString(expr);
      new_node->lower = lower;
      new_node->upper = upper;
      new_node->max = INFIN;
      new_node->SOP = NULL;
      for (curlist=nodelist;curlist;curlist=curlist->link)
	if ((strcmp(curlist->this_node->label,signal)==0) &&
	    (curlist->this_node->type==IO)) {
	  newlist=(nodelistADT)GetBlock(sizeof (*(nodelist)));
	  newlist->this_node=curlist->this_node;
	  newlist->link=NULL;
	  new_node->succs=newlist;
	  newlist=(nodelistADT)GetBlock(sizeof (*(nodelist)));
	  newlist->this_node=new_node;
	  newlist->link=NULL;
	  curlist->this_node->preds=newlist;
	  break;
	}       
      newlist=(nodelistADT)GetBlock(sizeof (*(nodelist)));
      if ((oldlist->this_node->type!=GATEE) ||
	  (strcmp(oldlist->this_node->label,signal)!=0)) {
	oldlist->link=newlist;
	newlist->this_node=new_node;
	newlist->link=NULL;
	oldlist=newlist;
      }
      num_nodes++;
      cur_char=0;
      token=egettok(expr,tokvalue,MAXTOKEN,&cur_char);
      if (!d_R(&token,tokvalue,&cur_char,linenum,expr,nodelist,new_node,
	       &new_node->SOP))
	return false;
    }
    else {

      // Find input signal
      for (k=0;k<nsignals;k++) {
	if (strcmp(signals[k]->name,signal)==0)
	  break;
      }
      if (k==nsignals) {
	printf("ERROR: Could not find input signal %s in specification.\n",
	       signal);
	fprintf(lg,"ERROR: Could not find input signal %s in specification.\n",
		signal);
	return false;
      }
    }
    if (token != END_OF_FILE)
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
  }
  return true;
}

// ********************************************************************
// Load a general netlist.
// ********************************************************************
bool load_netlist(char* filename, signalADT* signals, int ninpsig,
		  int nsignals, node* wire_vec[],
		  nodelistADT& nodelist, int& num_nodes) {

  char inname[FILENAMESIZE];
  FILE *fp;
  int i,linenum;
  nodelistADT oldlist=NULL;
  
  strcpy(inname,filename);
  strcat(inname,".acc");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return false; 
  }
  printf("Loading gates from: %s\n",inname);
  fprintf(lg,"Loading gates from: %s\n",inname);
  linenum=1;
  for (i=0;i<nsignals;i++) {
    wire_vec[i]= new node;
    wire_vec[i]->label = CopyString(signals[i]->name);
    wire_vec[i]->type = IO;
    wire_vec[i]->counter = 0;
    wire_vec[i]->name = i;
    // wire_vec[i]->sense = true;
    wire_vec[i]->level = 0;
    oldlist=nodelist;
    nodelist=(nodelistADT)GetBlock(sizeof(*(nodelist)));
    nodelist->this_node=wire_vec[i];
    nodelist->link=oldlist;
  }
  if (!load_net(fp, filename, signals, ninpsig, nsignals, wire_vec,
		&linenum, nodelist, num_nodes)) {
    fclose(fp);
    return false;
  }
  // print_nodelist(nodelist);
  levelize_gen(nodelist, wire_vec, nsignals);
  find_abs_minmax_gen(nodelist);
  fclose(fp);
  return true;
}

// ********************************************************************
// Levelize a general netlist.
// ********************************************************************
void levelize_gen(nodelistADT nodelist, node* wire_vec[],
		  int nsignals) {

  int level=0;
  bool modified=true;

  while (modified) {
    modified=false;
    level=level+1;
    for (nodelistADT curnode=nodelist;curnode;curnode=curnode->link) {
      if (curnode->this_node->level==INIT_LEVEL) {
	bool test=true;
	for (nodelistADT prednode=curnode->this_node->preds;prednode;
	     prednode=prednode->link) {
	  if (prednode->this_node->level==INIT_LEVEL ||
	      prednode->this_node->level > level-1) {
	    test=false;
	    break;
	  }
	}
	if (test) {
	  curnode->this_node->level=level;
	  modified=true;
	}
      }
    }
  }
  printf("Number of levels = %d\n",level-1);
}

// ********************************************************************
// High level function for hazard checking a general circuit.
// ********************************************************************
void verify_gate_level(char menu, char command, designADT design) {

  int num_unique_haz_nodes=0;
//   int haz_node_vec[MAXNUMNODES];
  int num_nodes=0;
  nodelistADT nodelist=NULL;
  node** wire_vec = new node*[design->nsignals];
  bool general = true;

  load_netlist(design->filename, design->signals, design->ninpsig,
	       design->nsignals, wire_vec, nodelist, num_nodes);
  int haz_node_vec[num_nodes];
  hazard_check(design, wire_vec, design->nsignals, (-1),
	       num_unique_haz_nodes, haz_node_vec, nodelist, num_nodes,
	       general);
}

// ********************************************************************
// Find the absolute minmax values from any node to any other node in a
// general circuit. Writes the values into the min and max fields for
// each node in the node list.
// ********************************************************************
void find_abs_minmax_gen(nodelistADT nodelist) {

  int level=0;
  bool modified=false;
  int pred_num;

  do {
    modified=false;
    level++;
    for (nodelistADT node=nodelist;node;node=node->link) {
      if (node->this_node->level == level) {
	modified = true;
	node->this_node->min = INFIN;
	node->this_node->max = 0;
	pred_num=0;
	for (nodelistADT pnode=node->this_node->preds;pnode;
	     pnode=pnode->link) {
	  if ((pnode->this_node->preds) && 
	      (pnode->this_node->preds->this_node==node->this_node)) { 
	    bool neg=false;
	    for (level_exp product=node->this_node->SOP;product;
		 product=product->next) {
	      if (product->product[pred_num]=='0') {
		neg=true;
		break;
	      }
	    }
	    if (!neg) continue;
	  }
	  if ((pnode->this_node->min + node->this_node->lower) <
	      node->this_node->min)
	    node->this_node->min = pnode->this_node->min +
	      node->this_node->lower;
	  if ((pnode->this_node->max + node->this_node->upper) >
	      node->this_node->max)
	    node->this_node->max = pnode->this_node->max +
	      node->this_node->upper;
	  pred_num++;
	}
      }
    }
  }
  while (modified);

  // Copy the min and max values to the IO nodes
  for (nodelistADT node=nodelist;node;node=node->link) {
    if ((node->this_node->type == IO) &&
	(node->this_node->preds != NULL)) {
      node->this_node->min = node->this_node->preds->this_node->min;
      node->this_node->max = node->this_node->preds->this_node->max;
    }
  }
}
