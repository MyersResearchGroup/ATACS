// ***************************************************************************
// Functions for untimed hazard checking.
// ***************************************************************************

#ifndef _haz_untim_h
#define _haz_untim_h

bool all_in_edge_ok(statelistADT edge, int value, int index);

char* build_cube(node* thenode, node* successor, stateADT cur_state1,
		 int& prodsize);

bool color_next_state(statelistADT edge, int index, int& modified);

bool d_B(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level);

bool d_C(int* token, char tokvalue[MAXTOKEN],int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level);

bool d_S(int *token,char tokvalue[MAXTOKEN],int *cur_char,int *linenum,
	 char expr[MAXTOKEN],nodelistADT nodelist,node *curnode,
	 level_exp *level);

bool d_R(int *token,char tokvalue[MAXTOKEN],int *cur_char,int *linenum,
	 char expr[MAXTOKEN],nodelistADT nodelist,node *curnode,
	 level_exp *level);

bool d_T(int* token, char tokvalue[MAXTOKEN],int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level);

bool d_U(int* token, char tokvalue[MAXTOKEN], int* cur_char,
	 int* linenum, char expr[MAXTOKEN], nodelistADT nodelist,
	 node* curnode, level_exp* level);

int egettok(char* expr, char* tokvalue, int maxtok, int* cur_char);

void eval_input_nodes(designADT design, node* input_vec[],
		      int tot_num_inputs, stateADT cur_state1);

bool exists_path(node* node1, int signal);

void extend_size(level_exp level,int prodsize);

void find_abs_minmax_gen(nodelistADT nodelist);

void high_level_si(designADT design, stateADT start_state,
		   node* input_vec[], node* head, int tot_num_inputs,
		   int region_num, sg_node*& temp_sg_node,
		   bool general, bool va[]);

void levelize_gen(nodelistADT nodelist, node* wire_vec[],
		      int nsignals);

bool load_net(FILE* fp, char* filename, signalADT* signals, int ninpsig,
	      int nsignals, node* wire_vec[], int* linenum,
	      nodelistADT nodelist, int& num_nodes);

bool load_netlist(char* filename, signalADT* signals, int ninpsig,
		  int nsignals, node* wire_vec[],
		  nodelistADT& nodelist, int& num_nodes);

void mid_level_si_gen(designADT design, stateADT start_state,
		      statelistADT cur_state2, node* head,
		      sg_node*& temp_sg_node, int out);

void mid_level_si_tree(designADT design, stateADT cur_state1,
		       statelistADT cur_state2, node* head,
		       sg_node*& temp_sg_node, int out);

bool must_prop_to_out(stateADT start_state, node* head, int out);
  
bool must_prop_to_out_gen(node* thenode, node* successor, stateADT start_state);

bool must_prop_to_out_gen_hl(node* head, int out, stateADT start_state);

int prod_size(node* curnode);

bool propagate_colors(designADT design, sg_node* head_sg_node,
		      int num_states);

void prop_sg(stateADT cur_state, int index, bool va[], int& modified);

void verify_gate_level(char menu, char command, designADT design);

#endif    // Ending _haz_untim_h
