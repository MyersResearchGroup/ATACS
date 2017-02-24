// ***************************************************************************
// High level functions for tech mapping and decomposition.
// General definitions for the whole techmap process.
// ***************************************************************************

#ifndef _decomp_h
#define _decomp_h

// *********************************************************************
// Global debugging flags used throughout all tech mapping software.
// *********************************************************************

// #define __VERBOSE_DEPTH__
// #define __VERBOSE_TIMED__
// #define __VERBOSE_STARTUP__
// #define __VERBOSE_HAZINFO__
// #define __VERBOSE_MAT__
// #define __VERBOSE_GENGCINFO__
// #define __VERBOSE_TEST__
// #define __VERBOSE_LIB__ 
// #define __VERBOSE_CI__
// #define __VERBOSE_SC__
// #define __VERBOSE_COV__
// #define __VERBOSE_MONO__
// #define __VERBOSE_UNTIMED__
// #define __VERBOSE_ZONE__
   #define __VERBOSE_ACC__
// #define __VERBOSE_PERMUTE__

// This will turn on Nam's printing information so he can find how this works
// #define __NAM__
// #define __NAM_INFO__ 

// *********************************************************************
// Flags used that should become atacs command line options.
// *********************************************************************

// This will turn off matching of both sides of a nand gate
// #define __NAND_MATCH__

// This needs to be defined if a simple library is used to keep the long
// version of a CEL from being made into a library gate
// #define __SIMPLE_LIB__

// Use external rather than built in library
#define __READ_EXTERNAL_LIB__

// Add equivalent boolean representations to the library
// #define __ADD_LIB_REDUND__

// Turn on/off placing double inverters on the decomposition inputs that
// are not inverted and also placing 2 or 3 inverters on every internal
// node in the decomposition.
#define __ADD_DOUBLE_INV__

// Turn on/off .acc file creation
#define __MAKE_ACC_FILES__

// Turn on or off .acc file creation for all combo's of cost parameters
// #define __MAKE_ALL_ACC_COST_FILES__

// Turn on or off input permutations
// #define __DO_ALL_INPUT_PERMUTES__

// For monitoring memory usage
#ifndef OSX
#define __DOMEM__
#include "malloc.h"
#endif

#include "struct.h"
#include "loadac.h"
#include "loader.h"
#include <iostream>
#include <stdio.h>
#include <cstdlib>

// Sets the upperlimit on maximum number of inputs for the solution to
// one output.
#define BUFFER_AREA 0    // Area number of double inverter buffer
#define MAXNUMLIBCELLS 10000
#define INPUT_VEC_SIZE 70
#define MAXFANIN 100
#define MAXNUMNODES 200
#define MAXEXPRLEN 300          // Maximum length of boolean expression
#define MAXINPLITS 64           // Maximum number of literals in input name
#define INVMINDEL 1
#define INVMAXDEL 2
#define NAND2MINDEL 2
#define NAND2MAXDEL 4
#define CELMINDEL 4
#define CELMAXDEL 6

// Definition whether or not a care term occurs in more then one level expr
#define NODUP 0
#define DUP 1

// Definition for indicating a node is hazardous in the haz_node_vec
#define HAZARDOUS 1

// Definitions for cost parameters
#define DELAY 0
#define AREA 1
#define HAZARD 2

// Definitions for node types
#define INV 0
#define NAND2 1
#define CEL 2
#define INNEE 3
#define OUTTEE 4
#define IO 5
#define GATEE 6

// Definitions for combinational outputs
#define NOT_COMBO 0
#define POS_COMBO 1
#define NEG_COMBO 2

// Definitions for set or reset part of the decomposition
#define SET_CHAIN true
#define RES_CHAIN false

// Definitions for levels in the subject graph
#define INIT_LEVEL -1
#define INPUT_LEVEL 0

// Definitions for colors in the state graph
#define INITEE -1
#define DOWN 3
#define UP 1
#define RISING 0
#define FALLING 2
#define SOFTUP 4
#define SOFTDOWN 5

// Definition for the initial value of the lib_init variable
#define LIBINIT -10

// Used for growing a new zone during timed hazard checks.
// Cleared after checking for edges stabilized due to timing.
struct zonestuff {
  char color;       // Color of the original enabling transition
  int dim;          // Dimension of enabled_vec and zone
  int* key;         // Vector of enabled terms for zone
  int** clocks;     // Clocks is a square zone
};
typedef struct zonestuff zone_curt;

// Used for creating a linked list of edges that are stabilized (marked
// 1 or 0) in the untimed check and also in the timed check.
// Cleared after every node analysis.
class sg_node {
public:
  int counter;      // Counter for tracking the number of nodes in the sg list
  int index;        // The node number from the node list-indexes the colorvec
  sg_node* succ;
  sg_node* pred;
  stateADT cur_state;
  statelistADT edge;
  sg_node() {
    index = 0;
    counter = 0;
    succ = NULL;
    pred = NULL;
    cur_state = NULL;
    edge = NULL;
  }
};

// Used for creating a linked list of edges and next states that
// indicate a R->F or F->R transition between two states in the
// state graph. Cleared after every node analysis.
class timed_node {
public:
  int counter;    // For tracking the number of nodes in the timed list
  timed_node* pred;
  timed_node* succ;
  stateADT cur_state;
  statelistADT edge;
  timed_node() {
    counter = 0;
    pred = NULL;
    succ = NULL;
    cur_state = NULL;
    edge = NULL;
  }
};

// Used for creating a linked list of covers
class cover_node {
public:
  cover_node* succ;
  char* cover;
  int num_lits;
  cover_node() {
    succ = NULL;
    cover = NULL;
    num_lits = 0;
  }
};

// Used for creating a linked list of nodes for the decomposition
class node {
public:
  int type;              // Type of node
  node* pred1;           // Main predecessor
  node* pred2;           // Secondary predecessor
  node* succ;            // Successor
  int name;              // Signal name
  bool set_res;          // Is this input in the set or reset chain?
  int level;             // Level in the subject graph (inputs = 0)
  bool value;            // Logic value used in the coloring routine
  bool visited;          // Denotes if node has been visited
  bool acc_visit;        // Secondary visited field for writing .acc files
  int counter;           // Delineates nodes
  char expr[MAXEXPRLEN]; // Boolean expression at this point
  int min;               // Absolute minimum value of delay
  int max;               // Absolute maximum value of delay
  bool mhazard;          // Does this node have a mono hazard or not?
  bool ahazard;          // Does this node have a mono hazard or not?
  int min_time_cost;     // The minimum time up to this node
  int max_time_cost;     // The maximum time up to this node
  int area_cost;         // The area cost up to this node
  int hazard_cost;       // The hazard cost up to this node
  int lib_num;           // Index number of library function for this node
  char* input1;          // First input name used in library matching
  char* input2;          // Second input name used in library matching
  char* input3;          // Third input name used for C elements
  int input1_index;      // Index number in the label array for input1
  int input2_index;      // Index number in the label array for input2
    
  // These are added by Chris to support GATES
  char* label;           // Name of node
  char* Bexpr;           // Boolean expression of this gate
  int lower;             // Lower bound of delay for this gate
  int upper;             // Upper bound of delay for this gate
  struct nodes_tag* preds; // List of predecessor nodes
  struct nodes_tag* succs; // List of successor nodes
  level_exp SOP;         // SOP representation
  // Constructor
  node() {
    label = NULL;
    Bexpr = NULL;
    lower = 0;
    upper = INFIN;
    preds = NULL;
    succs = NULL;
    SOP = NULL;
    type = OUTTEE;
    pred1 = NULL;
    pred2 = NULL;
    succ = NULL;
    name = 0;
    set_res = SET_CHAIN;
    level = INIT_LEVEL;
    value = false;
    visited = false;
    acc_visit = false;
    counter = 0;
    min = 0;
    max = 0;
    mhazard = false;
    ahazard = false;
    min_time_cost = 0;
    max_time_cost = 0;
    area_cost = 0;
    hazard_cost = 0;
    lib_num = LIBINIT;
    input1 = NULL;
    input2 = NULL;
    input3 = NULL;
    input1_index = -1;
    input2_index = -1;
  }
};

// Used for creating a linked list of all internal nodes
typedef struct nodes_tag {
  node* this_node;
  struct nodes_tag* link;
} *nodelistADT;

typedef struct gene_tag {
  char* interfer;
  struct gene_tag* link;
};

// Used for creating a library node
class lib_node {
public:
  node* pred;        // Link to output of decomposed library gate
  char* name;        // Name of library function
  char* expr;        // Boolean expression of cell
  int num_inputs;    // Number of inputs
  int num_levels;    // Number of levels
  int min_del;       // Minimum cost of gate
  int max_del;       // Maximum cost of gate
  int area;          // Area cost of the gate
  bool gc;           // Flag set if this is a gC type of gate
  char** inp_labls;  // Ordered list of input labels
  char*  out_labl;   // Output label
  level_exp set;     // SOP functions that set the output (for gC only)
  level_exp reset;   // SOP functions that reset the output (for gC only)
  bool com_inp;      // true if there are common inputs in this cell
  // Constructor
  lib_node() {
    pred = NULL;
    num_inputs = 1;
    num_levels = 1;
    min_del = 0;
    max_del = 1;
    area = 0;
    gc = false;
    com_inp = false;
  }
};

// Used for storing input names for the netlist
class input_node {
public:
  input_node* succ;      // Link to next input
  input_node* pred;      // Link to previous node
  char* name;            // Node or input name
  char* lib_char;        // The library cell input in the boolean equation
  int counter;           // Node designator
  // Constructor
  input_node() : succ(NULL),
                 pred(NULL),
		 name(NULL),
		 lib_char(NULL),
                 counter(0) {
  }
  // Destructor  
  ~input_node() {
    if (name == NULL) return;
    delete name;
  }
  // Allocate memory for the name variable
  void set_name( char* tmp ) {
//     cout << "Set name name = " << tmp << endl;
//     cout << "Wonder what NAME is = " << name << endl;
    if (name != NULL) {
//       cout << "Did I ever delete name?" << endl;
      delete name;
    }
    unsigned length(strlen(tmp)+1);
//     cout << "Length = " << length << endl;
    // This is the line where cygwin seg faults for ram-read-sbuf
    name = new char[length];
//     cout << "name = " << name << " tmp = " << tmp << endl;
    strcpy(name,tmp);
//     cout << "name = " << name << " tmp = " << tmp << endl;
  }

};

// Used for storing node information to be used in the netlist
class netlist_node {
public:
  netlist_node* succ;         // Link to next node in the netlist
  netlist_node* pred;         // Link to previous node in the netlist
  node* head;                 // Head of this netlist_node
  int lib_num;                // Number of library function
  bool init_value;            // Initial value at this node
  int min;                    // Minimum time up to this node
  int max;                    // Maximum time up to this node
  input_node* first;          // Linked list of input names
  char* output;               // Output name
  char* expr;                 // Boolean expression of cell
  char expr_acc[MAXEXPRLEN];  // expr with the node names
  int counter;                // Node designator
  bool covered;               // Is this node done being covered?
  int area;                   // Area cost at this node
  int hazard_cost;            // Hazard cost at this node
  // Constructor
  netlist_node() {
    succ = NULL;
    pred = NULL;
    head = NULL;
    lib_num = 999;
    init_value = false;
    min = 0;
    max = 1;
    first = NULL;
    output = NULL;
    expr = NULL;
    expr_acc[0] = (char)NULL;
    counter = 0;
    covered = false;
    area = 0;
    hazard_cost = 0;
  }

  // Allocate memory for the output variable
  void set_output( char* tmp ) {
    if (output != NULL) delete output;
    unsigned length(strlen(tmp)+1);
    output = new char[length];
    strcpy(output,tmp);
  }
};

// ***************************************************************************
// These includes are down here because the classes above need to be
// defined before these files are included by atacs.h
// ***************************************************************************
#include "writeacc.h"
#include "haz_timed.h"
#include "haz_untim.h"
#include "techmap_print.h"
#include "techmap_misc.h"
#include "match_cov.h"
#include "loadlib.h"
#include "biomap.h"

char* build_forcing_cube(stateADT curstate, nodelistADT curnode,
			 int num_preds);

char* build_potential_cube(stateADT curstate, nodelistADT curnode,
			   nodelistADT prednode, int num_preds);

void copy_to_inp_vec(node* input_vec[], node* new_inp_vec[],
		     int new_input_index, int num_lits, int p[]);

void create_input_copy(node* input_vec[], node* new_inp_vec[],
		       int new_input_index, int num_lits);

void decomp(designADT design);

void decomp(designADT design, node* output_vec[], node* input_vec[], lib_node* lib_vec[], int &num_lib_cells);


void decomp_main_loop(designADT design, int region_num, int& num_set_covers,
		      int& num_reset_covers, cover_node*& head_set_node,
		      cover_node*& head_reset_node, node* input_vec[],
		      int& inp_vec_index, int& num_set_inputs,
		      int& num_reset_inputs, int& num_combo_inputs,
		      int& tot_num_inputs, node* output_vec[],
		      int out_vec_index, int num_lib_cells, int& combo,
		      lib_node* lib_vec[], FILE* fpall, FILE* fpallcov);

void do_all_cost_files(designADT design, node* input_vec[],
		       int tot_num_inputs, int inp_vec_index,
		       int combo, lib_node* lib_vec[],
		       int num_lib_cells, node* output_vec[],
		       int out_vec_index, int region_num,
		       int num_unique_haz_nodes, int haz_node_vec[],
		       nodelistADT head_nodelist, int pri_cost,
		       int sec_cost, FILE* fpallcov, int num_nodes_curt);

void do_all_input_permutes(designADT design, node* input_vec[],
			   int tot_num_inputs, int inp_vec_index,
			   int combo, lib_node* lib_vec[],
			   int num_lib_cells, node* output_vec[],
			   int out_vec_index, int region_num,
			   int num_set_covers, int num_reset_covers,
			   cover_node* head_set_node,
			   cover_node* head_reset_node,
			   nodelistADT head_nodelist, int num_nodes_curt,
			   FILE* fpall, FILE* fpallcov);

void do_input_shuffle(designADT design, node* input_vec[],
		      int tot_num_inputs, int inp_vec_index,
		      int combo, lib_node* lib_vec[],
		      int num_lib_cells, node* output_vec[],
		      int out_vec_index, int region_num,
		      cover_node* head, int new_input_index,
		      nodelistADT head_nodelist,
		      int num_nodes_curt, FILE* fpall, FILE* fpallcov);

void do_one_cost_file(designADT design, node* input_vec[],
		      int tot_num_inputs, int inp_vec_index,
		      int combo, lib_node* lib_vec[],
		      int num_lib_cells, node* output_vec[],
		      int out_vec_index, int region_num,
		      int num_unique_haz_nodes, int haz_node_vec[],
		      nodelistADT head_nodelist,
		      int pri_cost, int sec_cost, FILE* fpallcov,
		      int num_nodes_curt);

void do_one_input_combo(designADT design, node* input_vec[],
			int tot_num_inputs, int inp_vec_index,
			int combo, lib_node* lib_vec[],
			int num_lib_cells, node* output_vec[],
			int out_vec_index, int region_num,
			nodelistADT head_nodelist,
			int num_nodes_curt, FILE* fpall, FILE* fpallcov);

void do_io_time_check_gen(designADT design, nodelistADT nodelist);

void final_check(designADT design, int& num_unique_haz_nodes,
		 int haz_node_vec[], int num_nodes,
		 nodelistADT nodelist, bool general, char* output_name);

void hazard_check(designADT design, node* input_vec[],
		  int tot_num_inputs, int region_num,
		  int& num_unique_haz_nodes, int haz_node_vec[],
		  nodelistADT nodelist, int num_nodes_curt,
		  bool general);

int mono_check(designADT design, nodelistADT curnode, bool mono_check,
	       int mono_haz_node_vec[], int mono_haz_node_vec1[]);

char mono_eval(nodelistADT curnode, char* cube, int num_preds);

bool mono_forcing(stateADT curstate, nodelistADT curnode,
		  int num_preds);

bool mono_stable(nodelistADT node, stateADT curstate);
  
bool potential_hazard(stateADT curstate, nodelistADT curnode,
		      nodelistADT prednode, int num_preds, 
		      int which_pred, bool general);

int sg_check(designADT design, node* curnode);

void stab_and_prop_hl(designADT design, nodelistADT nodelist,
		      sg_node*& temp_sg_node, node* input_vec[],
		      int tot_num_inputs, sg_node* head_sg_node,
		      stateADT start_state, int region_num,
		      bool general, int num_nodes, int num_states);

void stab_prop_timed(designADT design, nodelistADT nodelist,
		     sg_node* head_sg_node, sg_node*& temp_sg_node,
		     int num_nodes, int num_states, node* input_vec[],
		     bool general, int& depth, int& paths, int& sucs,
		     int& states_vis);
  
void stab_prop_untimed(designADT design, nodelistADT nodelist,
		       stateADT start_state, node* input_vec[],
		       int tot_num_inputs, int region_num,
		       sg_node* head_sg_node, sg_node*& temp_sg_node,
		       bool general, int num_nodes, int num_states);

void swap(int& m, int&n);
#endif    // Ending _decomp_h
