// *********************************************************************
// Miscellaneous functions to support Curt Nelson's tech mapping code.
// *********************************************************************

#ifndef _techmap_misc_h
#define _techmap_misc_h

void alloc_color_eval_mem(designADT design, int num_nodes);

void back_lib_head(node*& head);

int build_cover_list(designADT design, int region_num,
		     cover_node* tail_node);

void build_library(lib_node* lib_vec[], int& num_lib_cells);

void build_linked_lists(designADT design, int region_num,
			int& num_set_covers, int& num_reset_covers,
			cover_node* head_set_node,
			cover_node* head_reset_node);

int check_combo_output(int num_set_covers, int num_reset_covers);

void clear_matching_decomp(node* input_vec[], int tot_num_inputs);

void clear_visited_decomp(node* input_vec[], int tot_num_inputs);

void color_pred_vec(statelistADT edge, int index, char color);

void copy_pred_vec(stateADT cur_state1, statelistADT edge);

void create_color_eval_vec(designADT design, nodelistADT nodelist,
			   node* input_vec[], int tot_num_inputs,
			   bool general);

void do_and_func(designADT design, node*& head, cover_node* head_node,
		 bool which, node* input_vec[], int& inp_vec_index,
		 int& num_inputs);

void do_input_nodes(cover_node* head_node, int i, node*& head,
		    bool which_chain, int& num_inputs, node* input_vec[],
		    int& inp_vec_index);

void do_level(node* input_vec[], int num_inputs, int index, int& count);

void do_nodelist_labels(designADT design, nodelistADT head_nodelist);

void do_nodelist_level_exps(designADT design, nodelistADT head);

void do_nodelist_preds(designADT design, nodelistADT head,
		       node* decomp_head);

void do_nodelist_succs(designADT design, nodelistADT head,
		       node* decomp_head);
  
void do_oneside_cel(designADT design, node*& head, cover_node* head_node,
		    int num_covers, bool which_chain, node* input_vec[],
		    int& inp_vec_index, int& num_inputs,
		    bool reset_path);
  
void do_or_func(designADT design, node*& head, cover_node* head_node,
		int num_covers, bool which_chain, node* input_vec[],
		int& inp_vec_index, int& num_inputs);

void dothe_decomp(designADT design, int num_set_covers, int num_reset_covers,
		  cover_node*& head_set_node, cover_node*& head_reset_node,
		  node* input_vec[], int& inp_vec_index, int combo,
		  int& num_set_inputs, int& num_reset_inputs,
		  int& num_combo_inputs, node* output_vec[],
		  int out_vec_index);

void eval_input_nodes(designADT design, node* input_vec[],
		      int tot_num_inputs, stateADT cur_state1);

void fill_color_eval_vecs(designADT design, nodelistADT nodelist,
			  stateADT cur_state1, bool general);

node* fill_lib_cell(lib_node* lib_vec[], int index, char* name,
		    int num_inputs, int num_levels, int min_del,
		    int max_del, int area, int first_cell);

void fill_null(char expression[], int length);

void find_num_colors(designADT design, int num_nodes);

int find_num_lits(designADT design, char* cover);

void find_num_state_edge(designADT design, int& num_states,
			 int& num_edges, int num_nodes);
  
stateADT find_start_state(designADT design);

void free_cover_nodes(cover_node* head_set_node,
		      cover_node* head_reset_node);

void free_decomp(node* input_vec[], int tot_num_inputs);

void free_netlist(netlist_node* head_netlist_node);

void free_sg_node(sg_node* head_sg_node);

void free_timed_node(timed_node* head_timed_node);

void gen_bool_eval(nodelistADT nodelist, stateADT cur_state1);

int levelize(node* input_vec[], int num_set_inputs,
	     int num_reset_inputs, int num_combo_inputs);

void lib_pred1(node*& head, int type);

void lib_pred2(node*& head, int type);

cover_node* make_cover_node(void);

node* make_fill_node_pred1(node*& head_node, int gate);

node* make_fill_node_pred2(node*& head_node, int type_node);

input_node* make_input_node(void);

lib_node* make_lib_node(void);

netlist_node* make_netlist_node(void);

node* make_node(void);

nodelistADT make_node_list(designADT design, node* input_vec[],
			   int tot_num_inputs, int& num_nodes_curt);

node* make_output_node(designADT design, node* output_vec[],
		       int out_vec_index, int output_num);

sg_node* make_sg_node(void);

void make_vec(char*& vec, int num_nodes);

char* mem_name(char* tmp);

void push_netlist(node* head, netlist_node* netlist_node,
		  char* lib_expr, bool covered, char* output);

void push_netlist_input(input_node*& temp_input_node, char* name,
			char* lib_letter);

void push_sg(stateADT cur_state1, statelistADT cur_state2,
	     sg_node*& temp_sg_node, int index);

#endif    // Ending _techmap_misc_h
