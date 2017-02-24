// ********************************************************************
// Functions for handling the library used for matching and covering.
// ********************************************************************

#include "decomp.h"
#include "cmath"
#include "loader.h"

// ********************************************************************
// Fill a library cell with data.
// ********************************************************************
void fill_lib_vec(lib_node* lib_vec[], int index, bool gate, char* name,
		  int num_inputs, int min_rise, int max_rise,
		  int min_fall, int max_fall, int area,
		  level_exp set_sop, level_exp reset_sop,
		  char **inp_labels, char *outlab, char *nextlab,
		  char *expr, int& num_lib_cells) {

  int num_levels = 1;
  int max_num_levels = 1;
  bool plain_gc = false;
  static bool celem_done = false;
  bool celem = false;
  bool dup;
  node* head;
  
  if (gate) {

    // This is a combinational gate
    detect_inv(set_sop, num_inputs);
    detect_2nand(set_sop, num_inputs);
#ifdef __VERBOSE_LIB__
    print_lib_gate(num_lib_cells, name, outlab, expr, area, min_rise,
		   max_rise, min_fall, max_fall, num_inputs, inp_labels,
		   set_sop);
#endif
  }
  else {
    
    // These are gC elements
    plain_gc = detect_plain_gc(set_sop, reset_sop, num_inputs);
    if (!celem_done)
      celem = detect_celem(set_sop, reset_sop, num_inputs);
#ifdef __VERBOSE_LIB__
    print_lib_gc(num_lib_cells, name, outlab, nextlab, expr, area,
		 min_rise, max_rise, min_fall, max_fall, num_inputs,
		 inp_labels, set_sop, reset_sop);
#endif
  }
  if (celem && !celem_done) {
    
    // Put a simple C-element in the library
    lib_vec[num_lib_cells] = make_lib_node();
    lib_vec[num_lib_cells]->pred = make_node();

    head = fill_lib_lib_cell(lib_vec, num_lib_cells, name,
			     num_inputs, min_rise, max_rise,
			     area, expr, !gate);
    
    // Make a C-element circuit
    head->type = CEL;
    head->input1 = inp_labels[0];
    head->input1_index = 0;
    head->input2 = inp_labels[1];
    head->input2_index = 1;
    head->input3 = inp_labels[2];
    lib_vec[num_lib_cells]->inp_labls = inp_labels;
    lib_vec[num_lib_cells]->out_labl = outlab;
    lib_vec[num_lib_cells]->set = set_sop;
    lib_vec[num_lib_cells]->reset = reset_sop;
    lib_vec[num_lib_cells]->num_levels = 1;
#ifdef __VERBOSE_LIB__
    cout << "Number of levels = 1" << endl;
    print_lib_cells(lib_vec, num_lib_cells+1);
    cout << endl;
#endif
    num_lib_cells++;
    name = "cel_long:asynch";
#ifdef __VERBOSE_LIB__
    print_lib_gc(num_lib_cells, name, outlab, nextlab, expr, area,
		 min_rise, max_rise, min_fall, max_fall, num_inputs,
		 inp_labels, set_sop, reset_sop);
#endif
    celem_done = true;
  }
  
#ifndef __SIMPLE_LIB__
  // Create the library element and do the decomposition
  lib_vec[num_lib_cells] = make_lib_node();
  lib_vec[num_lib_cells]->pred = make_node();
  head = fill_lib_lib_cell(lib_vec, num_lib_cells, name, num_inputs, 
			   min_rise, max_rise, area, expr, !gate);
  
  if (!gate) {
    
    // Need to put additional information into the library cell if this
    // is a gC element
    head->input3 = inp_labels[num_inputs-1];
    lib_vec[num_lib_cells]->set = set_sop;
    lib_vec[num_lib_cells]->reset = reset_sop;
  }
  lib_vec[num_lib_cells]->inp_labls = inp_labels;
  lib_vec[num_lib_cells]->out_labl = outlab;
  lib_decomp(head, num_inputs, set_sop, reset_sop, inp_labels, gate,
	     plain_gc);
  strip_dou_invs(lib_vec, lib_vec[num_lib_cells]->pred);
  find_num_levels(lib_vec, num_lib_cells, num_levels, max_num_levels);
  lib_vec[num_lib_cells]->num_levels = max_num_levels;
  
#ifdef __VERBOSE_LIB__
  print_lib_cells(lib_vec, num_lib_cells+1);
#endif
  
  // Determine if there are duplicate care terms (common inputs)
  dup = dup_care(num_inputs, set_sop, reset_sop, gate, lib_vec[num_lib_cells]);
#ifdef __VERBOSE_LIB__
  if (dup)
    cout << "This library cell has common inputs" << endl;
  cout << endl;
#endif
  num_lib_cells++;
#endif
}

// ********************************************************************
// Detect if there are multiple care terms in the same column.  If so,
// indicate this by setting the common input flag in the library cell.
// ********************************************************************
bool dup_care(int num_inputs, level_exp set_sop, level_exp reset_sop,
	      bool gate, lib_node* lib_element) {

  level_exp setlev;
  level_exp nextlev;
  level_exp resetlev;

  // Check set terms against set terms for either gates or gc's
  for (setlev=set_sop;setlev;setlev=setlev->next) {
    if (setlev->next) {
      for (nextlev=setlev->next;nextlev;nextlev=nextlev->next) {
	for (int i=0;i<num_inputs;i++) {
	  if (((setlev->product[i] == '0') ||
	       (setlev->product[i] == '1')) &&
	      ((nextlev->product[i] == '0') ||
	       (nextlev->product[i] == '1'))) {
	    lib_element->com_inp = true;
	    return true;
	  }
	}
      }
    }
  }
  if (!gate) {
    
    // We have a gC element
    // Check set terms against reset terms
    for (setlev=set_sop;setlev;setlev=setlev->next) {
      for (resetlev=reset_sop;resetlev;resetlev=resetlev->next) {
	for (int j=0;j<num_inputs;j++) {
	  if (((setlev->product[j] == '0') ||
	       (setlev->product[j] == '1')) &&
	      ((resetlev->product[j] == '0') ||
	       (resetlev->product[j] == '1'))) {
	    lib_element->com_inp = true;
	    return true;
	  }
	}
      }
    }

    // Check reset terms against reset terms
    for (resetlev=reset_sop;resetlev;resetlev=resetlev->next) {
      if (resetlev->next) {
	for (nextlev=resetlev->next;nextlev;nextlev=nextlev->next) {
	  for (int i=0;i<num_inputs;i++) {
	    if (((resetlev->product[i] == '0') ||
		 (resetlev->product[i] == '1')) &&
		((nextlev->product[i] == '0') ||
		 (nextlev->product[i] == '1'))) {
	      lib_element->com_inp = true;
	      return true;
	    }
	  }
	}
      }
    }
  }
  return false;
}

// ********************************************************************
// Detect if this is an inverter.
// ********************************************************************
bool detect_inv(level_exp set_sop, int num_inputs) {
  
  if (num_inputs == 1) {
    if (set_sop->product[0] == '0') {
#ifdef __VERBOSE_LIB__
      cout << "Found an inverter...." << endl;
#endif
      return true;
    }
  }
  return false;
}

// ********************************************************************
// Detect if this is a 2-input nand.
// ********************************************************************
bool detect_2nand(level_exp set_sop, int num_inputs) {
  
  if (num_inputs == 2) {
    if ((set_sop->product[0] == 'X') &&
	(set_sop->product[1] == '0') &&
	(set_sop->next->product[0] == '0') &&
	(set_sop->next->product[1] == 'X')) {
#ifdef __VERBOSE_LIB__
      cout << "Found a 2-input nand ...." << endl;
#endif
      return true;
    }
  }
  return false;
}

// ********************************************************************
// Detect if this is a C-element.
// ********************************************************************
bool detect_celem(level_exp set_sop, level_exp reset_sop,
		  int num_inputs) {
  
  if (num_inputs == 3) {
    if ((set_sop->product[0] == '1') &&
	(set_sop->product[1] == '1') &&
	(set_sop->product[2] == 'X')) {
      if ((reset_sop->product[0] == '0') &&
	  (reset_sop->product[1] == '0') &&
	  (reset_sop->product[2] == 'X')) {
#ifdef __VERBOSE_LIB__
	cout << "Found a C-element...." << endl;
#endif
	return true;
      }
    }
  }
  return false;
}

// ********************************************************************
// Detect whether or not this is a plain gC.
// ********************************************************************
bool detect_plain_gc(level_exp set_sop, level_exp reset_sop,
		     int num_inputs) {

  if (num_inputs == 3) {
    if ((set_sop->product[0] == '1') &&
	(set_sop->product[1] == 'X') &&
	(set_sop->product[2] == 'X')) {
      if ((reset_sop->product[0] == 'X') &&
	  (reset_sop->product[1] == '0') &&
	  (reset_sop->product[2] == 'X'))
#ifdef __VERBOSE_LIB__
	cout << "Found a plain gc .... " << endl;
#endif
	return true;
    }
  }
  return false;
}

// ********************************************************************
// High level library decomposition routine.
// ********************************************************************
void lib_decomp(node*& head, int num_inputs, level_exp set_sop,
		level_exp reset_sop, char** inp_labels, bool gate,
		bool plain_gc) {
  
  int num_set_terms = 0;
  int num_reset_terms = 0;
  int num_lits;
  bool or_to_and = false;
  node* head_CEL_node;

  // Find the number of product terms
  for (level_exp curlevel=set_sop;curlevel;curlevel=curlevel->next)
    num_set_terms++;
  for (level_exp curlevel=reset_sop;curlevel;curlevel=curlevel->next)
    num_reset_terms++;

  if (gate) {
    if (num_set_terms == 1) {
      num_lits = find_num_lib_lits(num_inputs, set_sop->product);
      do_lib_and(head, num_inputs, set_sop->product, num_lits,
		 inp_labels, or_to_and);
    }
    else
      do_lib_or(head, num_set_terms, num_inputs, set_sop, inp_labels);
  }
  else {

    // Make a C-element circuit
    head->type = CEL;

    // Store a pointer at the C-element for doing the reset leg later
    head_CEL_node = head;

    // Do the set part of the chain
    // Add 3 inverters if not a plain_gc
    if (!plain_gc) {
      head = make_fill_node_pred1(head, INV);
      head = make_fill_node_pred1(head, INV);
    }
    head = make_fill_node_pred1(head, INV);
    if (num_set_terms == 1) {
      num_lits = find_num_lib_lits(num_inputs, set_sop->product);
      do_lib_and(head, num_inputs, set_sop->product, num_lits,
		 inp_labels, or_to_and);
    }
    else
      do_lib_or(head, num_set_terms, num_inputs, set_sop, inp_labels);

    // Do the reset part of the chain
    head = head_CEL_node;
    
    // Add an inverter since the reset path has a bubble at the C-element
    head = make_fill_node_pred2(head, INV);
    head = make_fill_node_pred1(head, INV);
    if (num_reset_terms == 1) {
      num_lits = find_num_lib_lits(num_inputs, reset_sop->product);
      do_lib_and(head, num_inputs, reset_sop->product, num_lits,
		 inp_labels, or_to_and);
    }
    else
      do_lib_or(head, num_reset_terms, num_inputs, set_sop, inp_labels);
  }
}

// ********************************************************************
// High level function for implementing the OR portion of the SOP tree.
// Calls the AND function part.
// ********************************************************************
void do_lib_or(node*& head, int num_terms, int num_inputs,
	       level_exp sop, char** inp_labels) {
  
  node* temp_head;
  int i=0;
  int num_lits;
  level_exp curlevel;
  bool first_time = true;
  bool or_to_and = true;
  
  for (curlevel=sop;curlevel;curlevel=curlevel->next) {
    num_lits = find_num_lib_lits(num_inputs, curlevel->product);

    // Do as long as this is not the last cover
    if (i != num_terms-1) {
      if (first_time) {

	// Rename first cell
	head->type = NAND2;
	first_time = false;

	// Store pointer to NAND2 gate so we can return and do the other path
	temp_head = head;
	head = make_fill_node_pred2(head, INV);
      }
      else {
	head = make_fill_node_pred1(head, INV);
	head = make_fill_node_pred1(head, INV);
	head = make_fill_node_pred1(head, NAND2);
	temp_head = head;
	head = make_fill_node_pred2(head, INV);
      }
    }

    // Decompose an AND function
    do_lib_and(head, num_inputs, curlevel->product,  num_lits,
	       inp_labels, or_to_and);
    
    // Go down other successor of the NAND2 gate to finish the OR gate
    // but only if this is not the last cover term
    if (i != num_terms-1) {
      head = temp_head;
      head = make_fill_node_pred1(head, INV);
    }
    i++;
  }
}

// ********************************************************************
// Decompose one library AND gate.
// ********************************************************************
void do_lib_and(node*& head, int num_inputs, char* product,
		int num_lits, char** inp_labels, bool or_to_and) {
  
  int lits_covered=0;
  bool first_time = true;

  if (or_to_and)
    head = make_fill_node_pred1(head, INV);
  if (num_lits == 1) {

    // Just add the input node
    for (int j=0;j<num_inputs;j++) {
      if (product[j] != 'X') {
	if (product[j] == '1')
	  head = make_fill_node_pred1(head, INV);
	head->input1 = inp_labels[j];
	head->input1_index = j;
	break;
      }
    }
  }
  else {
    for (int i=0;i<num_inputs;i++) {
      if (product[i] != 'X') {
	if ((num_lits - lits_covered) == 1) {
	  
	  // We're done so add input label
	  if (product[i] == '0')
	    head = make_fill_node_pred1(head, INV);
	  head->input1 = inp_labels[i];
	  head->input1_index = i;
	  break;
	}
	else {
	  if (!first_time) {
	    head = make_fill_node_pred1(head, INV);
	    head = make_fill_node_pred1(head, INV);
	    head = make_fill_node_pred1(head, INV);
	  }
	  first_time = false;
	  head = make_fill_node_pred1(head, NAND2);
	  if (product[i] == '0') {
	    head = make_fill_node_pred2(head, INV);
	    head->input1 = inp_labels[i];
	    head->input1_index = i;
	    head = head->succ;
	  }
	  else {
	    head->input2 = inp_labels[i];
	    head->input2_index = i;
	  }
	  lits_covered++;
	}
      }
    }
  }
}

// ********************************************************************
// Find the number of levels in one library cell.
// ********************************************************************
void find_num_levels(lib_node* lib_vec[], int num_lib_cells,
		     int num_levels, int& max_num_levels) {

  bool pred1_flag=true;
  node* head;

#ifdef __VERBOSE_LIB__
  cout << "Number of levels = ";
#endif
  head = lib_vec[num_lib_cells]->pred;
  if (head->pred1 != NULL) {
    pred1_flag = true;
    find_num_levels_recurse(head, pred1_flag, num_levels, max_num_levels);
    num_levels--;
  }
  if (head->pred2 != NULL) {
    pred1_flag = false;
    find_num_levels_recurse(head, pred1_flag, num_levels, max_num_levels);
    num_levels--;
  }
#ifdef __VERBOSE_LIB__
  cout << max_num_levels << endl;
#endif
}

// ********************************************************************
// Recursive part of finding number of levels.
// ********************************************************************
void find_num_levels_recurse(node* head, bool pred1_flag,
			     int num_levels, int& max_num_levels) {

  num_levels++;
  if (num_levels > max_num_levels)
    max_num_levels++;
  if (pred1_flag)
    head = head->pred1;
  else
    head = head->pred2;
  if (head->pred1 != NULL) {
    pred1_flag = true;
    find_num_levels_recurse(head, pred1_flag, num_levels, max_num_levels);
    num_levels--;
  }
  if (head->pred2 != NULL) {
    pred1_flag = false;
    find_num_levels_recurse(head, pred1_flag, num_levels, max_num_levels);
    num_levels--;
  }
}

// ********************************************************************
// Remove any pair of double inverters at an input.
// ********************************************************************
void strip_dou_invs(lib_node* lib_vec[], node* head) {

  bool pred1_flag=true;

#ifdef __VERBOSE_LIB__
  cout << "Stripping double inverters..." << endl;
#endif
  if (head->pred1 != NULL) {
    pred1_flag = true;
    strip_dou_invs_recurse(head, pred1_flag);
  }
  if (head->pred2 != NULL) {
    pred1_flag = false;
    strip_dou_invs_recurse(head, pred1_flag);
  }
}

// ********************************************************************
// Recursive part of stripping double inverters.
// ********************************************************************
void strip_dou_invs_recurse(node* head, bool pred1_flag) {

  if (pred1_flag)
    head = head->pred1;
  else
    head = head->pred2;
  if (head->pred1 != NULL) {
    pred1_flag = true;
    strip_dou_invs_recurse(head, pred1_flag);
  }
  else {
    if (head->type == INV) {
      if ((head->succ != NULL) && (head->succ->type == INV)) {
	if (head->succ->succ != NULL) { 
	  if (head->succ->succ->type == INV) {
	    
	    // 3 inverter case - Hose the front two inverters
	    head->succ->succ->input1 = head->input1;
	    head->succ->succ->input1_index = head->input1_index;
	    head = head->succ->succ;
	    free(head->pred1->pred1);
	    free(head->pred1);
	    head->pred1 = NULL;
	    if (head->succ->pred1 == head)
	      head = head->succ;
	  }
	  else if (head->succ->succ->pred1 == head->succ) {
	    
	    // hose front 2 inverters on pred1 leg
	    head->succ->succ->input1 = head->input1;
	    head->succ->succ->input1_index = head->input1_index;
	    head = head->succ->succ;
	    free(head->pred1->pred1);
	    free(head->pred1);
	    head->pred1 = NULL;
	  }
	  else {
	    
	    // hose front 2 inverters on pred2 leg
	    head->succ->succ->input2 = head->input1;
	    head->succ->succ->input2_index = head->input1_index;
	    head = head->succ->succ;
	    free(head->pred2->pred1);
	    free(head->pred2);
	    head->pred2 = NULL;
	  }
	}
      }
    }
  }
  if (head->pred2 != NULL) {
    pred1_flag = false;
    strip_dou_invs_recurse(head, pred1_flag);
  }
}

// ********************************************************************
// Fill a library cell with data.
// ********************************************************************
node* fill_lib_lib_cell(lib_node* lib_vec[], int num_lib_cells,
			char* name, int num_inputs, int min_del,
			int max_del, int area, char* expr, bool gc) {

  node* head;

  lib_vec[num_lib_cells]->min_del = min_del;
  lib_vec[num_lib_cells]->max_del = max_del;
  lib_vec[num_lib_cells]->name = name;
  lib_vec[num_lib_cells]->num_inputs = num_inputs;
  lib_vec[num_lib_cells]->area = area;
  lib_vec[num_lib_cells]->expr = expr;
  lib_vec[num_lib_cells]->gc = gc;
  head = lib_vec[num_lib_cells]->pred;
  head->type = INV;
  return head;
}

// ********************************************************************
// Find the number of literals in a level expression product term.
// ********************************************************************
int find_num_lib_lits(int num_inputs, char* product) {

  int num_lits=0;

  for (int i=0;i<num_inputs;i++) {
    if (product[i] != 'X')
      num_lits++;
  }
  return num_lits;
}

/*****************************************************************************/
/* Get a token from a file.  Used for loading timed event-rule structures.   */
/*****************************************************************************/
int fgettok_lib(FILE *fp,char *tokvalue,int maxtok,int *linenum) 
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '=':
    case ';':
      if (!readword) return(c);
      else {
	ungetc(c,fp);
	return(WORD);
      }
      break;
    case '\n':
      if (!readword) {
	(*linenum)++;
      } else {
	ungetc('\n',fp);
	return (WORD);
      }
      break;
    case '#':
      if (!readword) {
	while (((c=getc(fp)) != EOF) && (c != '\n'));
	(*linenum)++;
	toklen=0;
	*tokvalue = '\0';
	//return(COMMENT);
      } else {
	ungetc('#',fp);
	return (WORD);
      }
      break;
    case '\t':
    case ' ':
      if (readword)
	return (WORD);      
      break;
    default:
      if (toklen < maxtok) {
	readword=TRUE;
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
      }
      break;
    }
  }
  if (!readword) 
    return(END_OF_FILE);
  else
    return(WORD);
}

// ********************************************************************
// Get a token from an expression.
// ********************************************************************
int lib_egettok(char* expr, char* tokvalue, int maxtok, int* cur_char) {

  int c;           // c is the character to be read in
  int toklen;      // toklen is the length of the toklen
  bool readword;   // True if token is a word

  readword = false;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=expr[*cur_char]) != '\0') {
    (*cur_char)++;
    switch (c) {
    case '(':
    case '!':
    case '*':
    case ')':
    case '+':
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

bool lib_R(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256], 
	   int ninp,level_exp* level); 

bool lib_S(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256], 
	   int ninp, level_exp* level);

bool lib_U(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256],
	   int ninp, level_exp* level) {

  int i,j;
  level_exp newlevel=NULL;

  switch (*token) {
  case WORD:
    if (strcmp(tokvalue,"false")==0) {
      (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
      return true;
    }
    newlevel=(level_exp)GetBlock(sizeof(*newlevel));
    newlevel->next=(*level);
    (*level)=newlevel;
    newlevel->product=(char *)GetBlock((MAXFANIN+1) * sizeof(char));
    for (j=0;j<ninp;j++)
      newlevel->product[j]='X';
    newlevel->product[j]='\0';
    if (strcmp(tokvalue,"true")==0) {
      (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
      return true;
    }
    if (strncmp(tokvalue,"maybe",5)==0) {
      for (j=0;j<ninp;j++)
	newlevel->product[j]='-';
      newlevel->product[j]='\0';
      (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
      return true;
    }
    for (i=0;i<ninp;i++)
      if (strcmp(inpsig[i],tokvalue)==0) break;
    if (i==ninp) {
      printf("ERROR:%d: %s is not found!\n",*linenum,tokvalue);
      fprintf(lg,"ERROR:%d: %s is not found!\n",*linenum,tokvalue);
      return false;
    }
    newlevel->product[i]='1';
    (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
    break;
  case '(':
    (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!lib_R(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    if ((*token) != ')') {
      printf("ERROR:%d:  Expected a ')'!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expected a ')'!\n",*linenum);
    }
    (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
    break;
  default:
    printf("ERROR:%d:  Expected a signal or a '('!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal or a '('!\n",*linenum);
    return false;
  }
  return true;
}

bool lib_T(int* token, char tokvalue[MAXTOKEN],int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256],
	   int ninp,level_exp* level) {

  level_exp newlevel=NULL;
  level_exp curlevel=NULL;
  char * product;
  int j;

  switch (*token) {
  case WORD:
  case '(':
    if (!lib_U(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    break;
  case '!':
    (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!lib_U(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    product=(char *)GetBlock((ninp+1) * sizeof(char));
    for (j=0;j<ninp;j++)
      product[j]='X';
    product[j]='\0';
    newlevel=invert_expr(product,(*level),NULL,ninp);
    free(product);
    curlevel=(*level);
    while (curlevel) {
      curlevel=(*level)->next;
      if ((*level)->product)
	free((*level)->product);
      free(*level);
      (*level)=curlevel;
    }
    (*level)=newlevel;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, '(', or '!'!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, '(', or '!'!\n",*linenum);
    return false;
  }
  return true;
}

bool lib_C(int* token, char tokvalue[MAXTOKEN],int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256], 
	   int ninp, level_exp* level) {

  level_exp oldlevel=NULL;
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;

  switch (*token) {
  case '*':
    (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!lib_T(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,&newlevel)) 
      return false;
    oldlevel=(*level);
    (*level)=and_expr((*level),newlevel,ninp,linenum);
    curlevel=oldlevel;
    while (curlevel) {
      curlevel=oldlevel->next;
      if (oldlevel->product) 
    	free(oldlevel->product);
      free(oldlevel);
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
    if (!lib_C(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    break;
  case '+':
  case ')':
  case '\0':
    break;
  default:
    printf("ERROR:%d:  Expected a '+', '*', or a ')'!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a '+', '*', or a ')'!\n",*linenum);
    return false;
  }
  return true;
}

bool lib_B(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256], 
	   int ninp, level_exp* level) {

  level_exp newlevel=NULL;

  switch (*token) {
  case '+':
    (*token)=lib_egettok(expr,tokvalue,MAXTOKEN,cur_char);
    if (!lib_S(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,&newlevel)) 
      return false;
    (*level)=or_expr((*level),newlevel,ninp,linenum);
    if (!lib_B(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    break;
  case ')':
  case '\0':
    break;
  default:
    printf("ERROR:%d:  Expected a '+' or a ')'!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a '+' or a ')'!\n",*linenum);
    return false;
  }
  return true;
}

bool lib_S(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256], 
	   int ninp, level_exp* level) {

  switch (*token) {
  case WORD:
  case '(':
  case '!':
    if (!lib_T(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    if (!lib_C(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, '(', or '!'!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, '(', or '!'!\n",*linenum);
    return false;
  }
  return true;
}

bool lib_R(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	   int* linenum, char expr[MAXTOKEN], char inpsig[256][256], 
	   int ninp, level_exp* level) {

  switch (*token) {
  case WORD:
  case '(':
  case '!':
    if (!lib_S(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    if (!lib_B(token,tokvalue,cur_char,linenum,expr,inpsig,ninp,level)) 
      return false;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, '(', or '!'!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, '(', or '!'!\n",*linenum);
    return false;
  }
  return true;
}

void lib_absorb_expr(level_exp *first)
{
  level_exp curlevel=(*first);
  level_exp last=NULL;
  while (curlevel) {
    //printf("CHECK: %s\n",curlevel->product);
    level_exp last2=curlevel;
    level_exp curlevel2=curlevel->next;
    bool subset=false;
    while (curlevel2) {
      if ((strcmp(curlevel->product,curlevel2->product)!=0)&&
	  (absorbed(curlevel2->product,curlevel->product))) subset=true;
      if (absorbed(curlevel->product,curlevel2->product)) {
	last2->next=curlevel2->next;
	//printf("DELETEING: %s\n",curlevel2->product);
	free(curlevel2);
      } else 
	last2=curlevel2;
      curlevel2=last2->next;
    }
    if (subset) {
      if (last) {
	last->next=curlevel->next;
	//printf("SUBSETING: %s\n",curlevel->product);
	free(curlevel);
      } else {
	(*first)=curlevel->next;
	//printf("SUBSETING: %s\n",curlevel->product);
	free(curlevel);
      }
    } else 
      last=curlevel;
    if (last)
      curlevel=last->next;
    else 
      curlevel=(*first);
  }
}

bool swap_order(char **inplab,level_exp setfunc,level_exp resetfunc,
		int i,int j) {
  char *tempstr;
  char tempchar;
  bool interesting=false;
  char phase_i,phase_j;
  char r_phase_i,r_phase_j;

  tempstr=inplab[i];
  inplab[i]=inplab[j];
  inplab[j]=tempstr;
  phase_i='X';
  phase_j='X';
  for (level_exp curlevel=setfunc;curlevel;curlevel=curlevel->next) {
    if (curlevel->product[i]!='X') 
      if (phase_i=='X') 
	phase_i=curlevel->product[i];
      else
	interesting=true;
    if (curlevel->product[j]!='X') 
      if (phase_j=='X') 
	phase_j=curlevel->product[j];
      else
	interesting=true;
    tempchar=curlevel->product[i];
    curlevel->product[i]=curlevel->product[j];
    curlevel->product[j]=tempchar;
  }
  if (phase_i!='X' && phase_j!='X' && phase_i!=phase_j) {
    interesting=true;
  }
  r_phase_i='X';
  r_phase_j='X';
  for (level_exp curlevel=resetfunc;curlevel;curlevel=curlevel->next) {
    if (curlevel->product[i]!='X') 
      if (r_phase_i=='X')
	r_phase_i=curlevel->product[i];
      else
	interesting=true;
    if (curlevel->product[j]!='X') 
      if (r_phase_j=='X')
	r_phase_j=curlevel->product[j];
      else
	interesting=true;
    tempchar=curlevel->product[i];
    curlevel->product[i]=curlevel->product[j];
    curlevel->product[j]=tempchar;
  }
  if (r_phase_i!='X' && r_phase_j!='X' && r_phase_i!=r_phase_j) {
    interesting=true;
  }
  if ((phase_i=='X' && phase_j!='X' && r_phase_i!='X' && r_phase_j!='X')||
      (phase_i!='X' && phase_j=='X' && r_phase_i!='X' && r_phase_j!='X')||
      (phase_i!='X' && phase_j!='X' && r_phase_i=='X' && r_phase_j!='X')||
      (phase_i!='X' && phase_j!='X' && r_phase_i!='X' && r_phase_j=='X')) {
    interesting=true;
  }
  return interesting;
}

int load_library(char *library, lib_node* lib_vec[], double ratio,
		 int& num_lib_cells) {

  FILE *fp;
  char *cell;
  char *outp;
  char *next;
  int area;
  double rise,fall;
  char *expr;
  char tokvalue[MAXTOKEN];
  int token;
  char tokvalue2[MAXTOKEN];
  int token2;
  int linenum=0;
  bool gate;
  int ninp;
  char inpsigs[256][256];
  char curinp[MAXTOKEN];
  level_exp setfunc;
  level_exp resetfunc;
  level_exp setfunc2;
  level_exp resetfunc2;
  level_exp tempfunc;
  level_exp prevlevel;
  bool readsig;
  int i,j,k;
  bool found;
  int cur_char;
  int index=0;
  char **inplab;
  char **inplab2;
  int minrise,maxrise,minfall,maxfall;
  bool sync_latch = false;

  if ((fp=fopen(library,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",library);
    fprintf(lg,"ERROR:  Cannot access %s!\n",library);
    return 0; 
  }
  printf("Loading library from:  %s\n",library);
  fprintf(lg,"Loading library from:  %s\n",library);
  token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
  while (token!=END_OF_FILE) {
    if (token!=WORD) {
      printf("ERROR:%d:  Expecting GATE or LATCH!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting GATE or LATCH!\n",linenum);
      return 0;
    }
    if (strcmp(tokvalue,"GATE")==0) gate=true;
    else if (strcmp(tokvalue,"LATCH")==0) gate=false;
    else {
      printf("ERROR:%d:  Expecting GATE or LATCH!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting GATE or LATCH!\n",linenum);
      return 0;
    }
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    if (token!=WORD) {
      printf("ERROR:%d:  Expecting cell name!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting cell name!\n",linenum);
      return 0;
    }
    cell=CopyString(tokvalue);
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    if (token!=WORD) {
      printf("ERROR:%d:  Expecting cell area!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting cell area!\n",linenum);
      return 0;
    }
    area=atoi(tokvalue);
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    if (token!=WORD) {
      printf("ERROR:%d:  Expecting output name!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting output name!\n",linenum);
      return 0;
    }
    outp=CopyString(tokvalue);
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    if (token!='=') {
      printf("ERROR:%d:  Expecting '='!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting '='!\n",linenum);
      return 0;
    }
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    if (token!=WORD) {
      printf("ERROR:%d:  Expecting cell logic function!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting cell logic function!\n",linenum);
      return 0;
    }
    expr=CopyString(tokvalue);
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    if (token!=';') {
      printf("ERROR:%d:  Expecting ';'!\n",linenum);
      fprintf(lg,"ERROR:%d:  Expecting ';'!\n",linenum);
      return 0;
    }
    rise=0.0;
    fall=0.0;
    token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    while ((token==WORD) && (strcmp(tokvalue,"PIN")==0)) {
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if ((token!=WORD) && (strcmp(tokvalue,"*")==0)) {
	printf("ERROR:%d:  Expecting '*'!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting '*'!\n",linenum);
	return 0;
      }
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("%d %s\n",token,tokvalue);
	printf("ERROR:%d:  Expecting phase!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting phase!\n",linenum);
	return 0;
      }
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("ERROR:%d:  Expecting input load!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting input load!\n",linenum);
	return 0;
      }
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("ERROR:%d:  Expecting maximum load!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting maximum load!\n",linenum);
	return 0;
      }
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("ERROR:%d:  Expecting rise time!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting rise time!\n",linenum);
	return 0;
      }
      rise=atof(tokvalue);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("ERROR:%d:  Expecting rise fanout delay!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting rise fanout delay!\n",linenum);
	return 0;
      }
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("ERROR:%d:  Expecting fall time!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting fall time!\n",linenum);
	return 0;
      }
      fall=atof(tokvalue);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      if (token!=WORD) {
	printf("ERROR:%d:  Expecting fall fanout delay!\n",linenum);
	fprintf(lg,"ERROR:%d:  Expecting fall fanout delay!\n",linenum);
	return 0;
      }
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    }
    if ((token==WORD) && (strcmp(tokvalue,"SEQ")==0)) {
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      next=CopyString(tokvalue);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    } else 
      next = NULL;
    if ((token==WORD) && (strcmp(tokvalue,"CONTROL")==0)) {
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      sync_latch = true;
    }
    if ((token==WORD) && (strcmp(tokvalue,"CONSTRAINT")==0)) {
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
      token=fgettok_lib(fp,tokvalue,MAXTOKEN,&linenum);
    }
    ninp=0;
    readsig=false;
    j=0;
    for (i=0;i<(int)strlen(expr);i++) {
      if ((expr[i]=='!') || (expr[i]=='+') || (expr[i]=='*') || 
	  (expr[i]=='(') || (expr[i]==')')) {  
	if (readsig) {
	  readsig=false;
	  curinp[j]='\0';
	  found=false;
	  for (k=0;k<ninp;k++)
	    if (strcmp(inpsigs[k],curinp)==0) { 
	      found=true;
	      break;
	    }
	  if (!found) {
	    strcpy(inpsigs[ninp],curinp);
	    ninp++;
	  }
	  j=0;
	}
      }	else {
	readsig=true;
	curinp[j]=expr[i];
	j++;
      }
    }
    if (readsig) {
      curinp[j]='\0';
      found=false;
      for (k=0;k<ninp;k++)
	if (strcmp(inpsigs[k],curinp)==0) { 
	  found=true;
	  break;
	}
      if (!found) {
	strcpy(inpsigs[ninp],curinp);
	ninp++;
      }
    }
    cur_char=0;
    setfunc=NULL;
    token2=lib_egettok(expr,tokvalue2,MAXTOKEN,&cur_char);
    if (!lib_R(&token2,tokvalue2,&cur_char,&linenum,expr,inpsigs,ninp,
	       &setfunc))
      return 0;
    resetfunc=NULL;
    if (!gate) {
      for (i=0;i<ninp;i++)
	if (strcmp(inpsigs[i],next)==0) break;
      if (i<ninp) {
	prevlevel=NULL;
	for (level_exp curlevel=setfunc;curlevel;curlevel=curlevel->next) 
	  if (curlevel->product[i]=='1') {
	    tempfunc=(level_exp)GetBlock(sizeof(*tempfunc));
	    tempfunc->next=prevlevel;
	    tempfunc->product=(char *)GetBlock((ninp+1) * sizeof(char));
	    strcpy(tempfunc->product,curlevel->product);
	    tempfunc->product[i]='X';
	    prevlevel=tempfunc;
	  }
	char *product=(char *)GetBlock((ninp+1) * sizeof(char));
	for (j=0;j<ninp;j++)
	  product[j]='X';
	product[j]='\0';
	resetfunc=invert_expr(product,tempfunc,NULL,ninp);
	lib_absorb_expr(&resetfunc);
	free(product);
	prevlevel=tempfunc;
	while (prevlevel) {
	  prevlevel=tempfunc->next;
	  if (tempfunc->product)
	    free(tempfunc->product);
	  free(tempfunc);
	  tempfunc=prevlevel;
	}
	tempfunc=(level_exp)GetBlock(sizeof(*tempfunc));
	tempfunc->next=NULL;
	tempfunc->product=(char *)GetBlock((ninp+1) * sizeof(char));
	for (j=0;j<ninp;j++)
	  tempfunc->product[j]='X';
	tempfunc->product[i]='0';
	tempfunc->product[j]='\0';
	setfunc=and_expr(setfunc,tempfunc,ninp,&linenum);
	for (level_exp curlevel=setfunc;curlevel;curlevel=curlevel->next) 
	  if (curlevel->product[i]=='0') curlevel->product[i]='X';
	free(tempfunc->product);
	free(tempfunc);
      }
    }
    minrise=(int)rint(10 * (1-ratio) * rise);
    maxrise=(int)rint(10 * (1+ratio) * rise);
    minfall=(int)rint(10 * (1-ratio) * fall);
    maxfall=(int)rint(10 * (1+ratio) * fall);
    inplab=(char **)GetBlock(ninp * sizeof(char*));
    for (i=0;i<ninp;i++) 
      inplab[i]=CopyString(inpsigs[i]);
    if (!sync_latch) {
      fill_lib_vec(lib_vec, index, gate, cell, ninp, minrise, maxrise,
		   minfall,maxfall, area, setfunc, resetfunc, inplab,
		   outp,next,expr,num_lib_cells);
      index++;
//       printf("%s\n",cell);
//       printf("Initial:");
//       for (i=0;i<ninp;i++)
// 	printf("%s ",inplab[i]);
//       printf("\n");
//       printf("Set=");
//       for (level_exp curlevel=setfunc;curlevel;curlevel=curlevel->next) 
// 	printf("%s ",curlevel->product);
//       printf("\n");
//       printf("Reset=");
//       for (level_exp curlevel=resetfunc;curlevel;curlevel=curlevel->next) 
// 	printf("%s ",curlevel->product);
//       printf("\n");

      if (ninp > 1) {

	int skipkey=(-1);
	while (1) {
	  bool interesting=true;
	  int len;
	  if (gate) len=ninp;
	  else len=ninp-1;
	  int key=len-1;
	  int newkey=len-1;
	  /*The key value is the first value from the 
	    end which is smaller than the value to 
	    its immediate right*/
	  while((key>0)&&(strcmp(inplab[key],inplab[key-1])<=0)) key--;
	  key--;
	
	  /*If key<0 the data is in reverse sorted order, 
	    which is the last permutation.  */
	  if(key <0) break;
	  
	  /*str[key+1] is greater than str[key] because 
	    of how key was found. If no other is greater, 
	    str[key+1] is used*/
	  newkey=len-1;
	  while((newkey > key) && (strcmp(inplab[newkey],inplab[key])<=0))
	    newkey--;
	  
	  int x=0,y=0;
	  if ((skipkey!=(-1))&&(key!=skipkey)) {
	    interesting=false;
	  } else {
	    skipkey=(-1);
	  }
	  if (!swap_order(inplab,setfunc,resetfunc,key,newkey)) {
	    if (skipkey==(-1)) skipkey=key;
	    x=key;
	    y=newkey;
	    interesting=false;
	  }
	  /*variables len and key are used to walk through 
	    the tail, exchanging pairs from both ends of 
	    the tail.  len and key are reused to save 
	    memory*/
	  len--;
	  key++;
	  /*The tail must end in sorted order to produce the 
	    next permutation.*/
	  while(len>key) {
	    if (swap_order(inplab,setfunc,resetfunc,len,key)) {
	      interesting=true;
	    }
	    key++;
	    len--;
	  }
// 	  for (i=0;i<ninp;i++)
// 	    printf("%s ",inplab[i]);
// 	  if (interesting) printf("-- interesting\n");
// 	  else printf("-- NOT interesting key=%d newkey=%d\n",x,y);
	  if (interesting) {
	    inplab2=(char **)GetBlock(ninp * sizeof(char*));
	    for (i=0;i<ninp;i++) 
	      inplab2[i]=CopyString(inplab[i]);
	    cell=CopyString(cell);
	    outp=CopyString(outp);
	    next=CopyString(next);
	    expr=CopyString(expr);
	    setfunc2=NULL;
	    for (level_exp curlevel=setfunc;curlevel;curlevel=curlevel->next) {
	      tempfunc=(level_exp)GetBlock(sizeof(*tempfunc));
	      tempfunc->product=CopyString(curlevel->product);
	      tempfunc->next=setfunc2;
	      setfunc2=tempfunc;
	    }
	    resetfunc2=NULL;
	    for (level_exp curlevel=resetfunc;curlevel;
		 curlevel=curlevel->next) {
	      tempfunc=(level_exp)GetBlock(sizeof(*tempfunc));
	      tempfunc->product=CopyString(curlevel->product);
	      tempfunc->next=resetfunc2;
	      resetfunc2=tempfunc;
	    }
	    fill_lib_vec(lib_vec, index, gate, cell, ninp, minrise, maxrise,
			 minfall,maxfall, area, setfunc2, resetfunc2, inplab2,
			 outp,next,expr,num_lib_cells);
	    index++;
	  }
	}
      }
    }
    sync_latch = false;
  }
  printf("Loaded library with %d library cells\n",index);
  return index;
}
