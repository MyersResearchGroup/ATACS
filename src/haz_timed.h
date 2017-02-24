// ***************************************************************************
// Functions for using timing to help detect hazards.
// ***************************************************************************

#ifndef _hazard_timed_h
#define _hazard_timed_h

void canon_zone(zone_curt* work_zone);

void clear_va(bool va[], int num_states);

void copy_first_zone(zone_curt* work_zone, clocklistADT clocklist,
		     int transition, int t_index);

void copy_zone_info(zone_curt* new_zone, zone_curt* work_zone,
		    int transition, int t_index);

void do_time(designADT design, zone_curt* work_zone,
	     int decomp_delay, stateADT cur_state,
	     sg_node*& temp_sg_node, node* head, int index,
	     bool va_path[], bool va_nodeR[],
	     bool va_nodeF[], int num_states,
	     node* input_vec[], bool general,
	     int& depth_tot, int depth_curr, int& num_paths,
	     int& num_sucs, int& num_states_vis);

void fill_zone_time_data(designADT design, zone_curt* work_zone,
			 node* input_vec[], bool general,
			 int transition);

void find_abs_minmax(node* input_vec[], int tot_num_inputs);

void find_fr_rf(designADT design, node* head, timed_node*& temp_timed_node);

void find_minmax(designADT design, int& mindelay, int& maxdelay,
		 node* input_vec[], int tot_num_inputs, node* head,
		 timed_node* head_timed_node);

zone_curt* make_new_zone(int dim);

void free_new_zone(zone_curt* temp_zone,int dim);

timed_node* make_timed_node(void);

void push_timed(stateADT cur_state1, statelistADT cur_state2,
		timed_node*& temp_timed_node);
  
void stabilize_edge_timed(designADT design, sg_node*& temp_sg_node,
			  node* head, int num_states, node* input_vec[],
			  bool general, int& depth_tot, int& num_paths,
			  int& num_sucs, int& num_states_vis);

#endif    // Ending _hazard_timed_h
