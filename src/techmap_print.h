// *********************************************************************
// Functions to print various information in Curt Nelson's technology
// mapping software, mostly for debugging purposes.
// *********************************************************************

#ifndef _techmap_print_h
#define _techmap_print_h

void print_color(char color);

void print_coloring_error(designADT design, stateADT state,
			  statelistADT edge, int index);

void print_cover_list(int num_cov, cover_node* head_node);

void print_decomp(designADT design, node* input_vec[], 
		  int Tot_num_inputs, int combo);

void print_decomp_delays(designADT design, node* input_vec[],
			 int tot_num_inputs, int region_num);

void print_events_data_struct(designADT design);

void print_general_node_delays(nodelistADT nodelist, node* input_vec[],
			       int tot_num_inputs);

void print_hazard_info(int ack_haz_node_vec[], int mono_haz_node_vec[],
		       int total_ack_hazards, int total_mono_hazards,
		       int haz_node_vec[], int& num_unique_haz_nodes,
		       int num_nodes, char* output_name,
		       int mono_haz_node_vec1[]);
  
void print_input_vec(designADT design, node* input_vec[],
		     int tot_num_inputs, int combo);

void print_lib_cell_recurse(node* head, bool pred1_flag);

void print_lib_cells(lib_node* lib_vec[], int num_lib_cells);

void print_lib_gate(int num_lib_cells, char* name, char* outlab,
		    char* expr, int area, int min_rise, int max_rise,
		    int min_fall, int max_fall, int num_inputs,
		    char** inp_labels, level_exp set_sop);

void print_lib_gc(int num_lib_cells, char* name, char* outlab,
		  char* nextlab, char* expr, int area, int min_rise,
		  int max_rise, int min_fall, int max_fall,
		  int num_inputs, char** inp_labels, level_exp set_sop,
		  level_exp reset_sop);

void print_match_info(node* head);

void print_netlist_list(netlist_node* head_netlist_node,
			lib_node* lib_vec[]);

void print_new_inp_vec(designADT design, node* input_vec[],
		       int tot_num_inputs, int combo);
  
void print_no_match_info(int cell_area_cost, int cell_min_del,
			 int cell_max_del, int cell_haz_cost);

void print_node_delays_recurse(nodelistADT node, int min, int max,
			       char* label);

void print_nodelist(nodelistADT head_nodelist);

void print_output_vec(designADT design, node* output_vec[]);
  
void print_plus_minus(int number);
  
void print_region_info(designADT design, regionADT cur_region,
		       char *cover,int region_number, int cover_number);

void print_rules(designADT design);

void print_sg_edges(designADT design, sg_node* head_sg_node);

void print_signals_list(designADT design);

int print_state_zones(designADT design, clocklistADT clocklist,
		      bool print_flag);

void print_states(designADT design);

void print_states_edges(designADT design);

void print_states_col(designADT design, int color);

void print_timed_color_rf(designADT design, stateADT state,
			  statelistADT edge);

void print_zone(designADT design, zone_curt* zone);

#endif    // Ending _techmap_print_h
