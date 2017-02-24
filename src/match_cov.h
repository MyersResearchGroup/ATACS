// *****************************************************************
// Functions for matching and covering the decomposition.
// *****************************************************************


#ifndef _match_cov_h
#define _match_cov_h

void add_to_netlist(designADT design, node* head,
		    input_node*& temp_input_node, char name_node[],
		    char* name_pri, char temp[], char* lib_expr,
		    lib_node* lib_vec[], bool& covered,
		    netlist_node*& new_netlist_node_ptr, bool& ignore,
		    int base, char* lib_letter);

bool com_input_chk(designADT design, node* lib_head, node* decomp_head,
		   lib_node* lib_element, int* ci_vec, bool pred1,
		   nodelistADT head_nodelist);

void cover_and_write_netlist(designADT design, node* input_vec[],
			     int tot_num_inputs, lib_node* lib_vec[],
			     int num_lib_cells, node* output_vec[],
			     int out_vec_index, int region_num,
			     int num_haz_nodes, int haz_node_vec[],
			     int pri_cost, int sec_cost, FILE* fpallcov,
			     int num_nodes_curt);

bool cover_lib_cell(designADT design, node* decomp_head, node*& lib_head,
		    lib_node* lib_vec[], netlist_node*& temp_netlist_node,
		    int lib_num_inputs, netlist_node* head_netlist_node,
		    netlist_node*& new_netlist_node_ptr, int base,
		    input_node*& temp_input_node);

bool cover_lib_cell_recurse(designADT design, node* decomp_head,
			    bool pred1_flag, lib_node* lib_vec[],
			    input_node*& temp_input_node,
			    netlist_node*& temp_netlist_node,
			    int& num_inputs_cov, int lib_num_inputs,
			    node* lib_head, netlist_node* head_netlist_node,
			    netlist_node*& new_netlist_node_ptr, int base);

bool cover_netlist_cells(FILE* fp, designADT design, node* output_vec[],
			 int out_vec_index, lib_node* lib_vec[],
			 netlist_node* head_netlist_node, int base,
			 FILE* fpallcov);

bool cover_write_one_output(designADT design, lib_node* lib_vec[],
			    node* output_vec[], int out_vec_index,
			    int region_num, netlist_node* head_netlist_node,
			    int num_haz_nodes, int haz_node_vec[],
			    int pri_cost, int sec_cost, int base,
			    FILE* fpallcov);
  
bool equal_chk(designADT design, int node1num, int node2num,
	       nodelistADT head_nodelist);

node* findnode(nodelistADT head_nodelist, int num);

void match_all_lib_cells(designADT design, node* head,
			 lib_node* lib_vec[], int num_lib_cells,
			 int lev, int pri_cost, int sec_cost,
			 nodelistADT head_nodelist);

void match_decomp(designADT design, node* input_vec[],
		  int tot_num_inputs, lib_node* lib_vec[],
		  int num_lib_cells, int pri_cost, int sec_cost,
		  int combo, nodelistADT head_nodelist);

void match_input(designADT design, node* lib_head, node* decomp_head,
		 lib_node* lib_element, int* sc_vec, bool pred1);

bool match_one_lib_cell(designADT design, node* decomp_head,
			lib_node* lib_element, int& cell_min_del,
			int& cell_max_del, int& cell_area_cost,
			int& cell_haz_cost, int& cell_int_mono_haz,
			int& cell_int_ack_haz, int& cell_ext_mono_haz, 
			int& cell_ext_ack_haz, int* sc_vec, int* ci_vec,
			nodelistADT head_nodelist, lib_node* lib_vec[]);

bool match_one_lib_cell_recurse(designADT design, node* decomp_head,
				node* lib_head, int& cell_min_del,
				int& cell_max_del, int& cell_area_cost,
				int& cell_haz_cost, int& cell_int_mono_haz,
				int& cell_int_ack_haz, int& cell_ext_mono_haz, 
				int& cell_ext_ack_haz, int* sc_vec,
				bool pred1, lib_node* lib_element,
				int* ci_vec, nodelistADT head_nodelist,
				lib_node* lib_vec[]);

void munge_exprs(designADT design, netlist_node* head_netlist_node);

bool node_compare(designADT design, node* node1, node* node2, bool pred1);

bool sc_problem(char* color_temp, char* sc_temp, int num_inputs);

bool short_circuit(designADT design, int* cur_prod,
		   lib_node* lib_element);

void update_area(node* head, int lib_index, int cell_max_del,
		 int cell_min_del, int cell_area_cost,
		 int cell_haz_cost, int sec_cost);
  
void update_costs(node* temp_head, int& cell_max_del, int& cell_min_del,
		  int& cell_area_cost, int& cell_haz_cost);

void update_delay(node* head, int lib_index, int cell_max_del,
		  int cell_min_del, int cell_area_cost,
		  int cell_haz_cost, int sec_cost);
  
void update_hazard(node* head, int lib_index, int cell_max_del,
		   int cell_min_del, int cell_area_cost,
		   int cell_haz_cost, int sec_cost);

void update_node_costs(node* head, int lib_index, int cell_max_del,
		       int cell_min_del, int cell_area_cost,
		       int cell_haz_cost);

int write_netlist_cells(FILE* fp, netlist_node* net_node,
			lib_node* lib_vec[]);

#endif    // Ending _match_cov_h
