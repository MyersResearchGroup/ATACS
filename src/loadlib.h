// *****************************************************************
// Functions for handling the library for matching and covering.
// *****************************************************************


#ifndef _load_lib_h
#define _load_lib_h

void do_lib_and(node*& head, int num_inputs, char* product,
		int num_lits, char** inp_labels, bool or_to_and);

bool detect_2nand(level_exp set_sop, int num_inputs);

bool detect_celem(level_exp set_sop, level_exp reset_sop,
		  int num_inputs);

bool detect_inv(level_exp set_sop, int num_inputs);

bool detect_plain_gc(level_exp set_sop, level_exp reset_sop,
		     int num_inputs);

void do_lib_or(node*& head, int num_terms, int num_inputs,
	       level_exp set_sop, char** inp_labels);
  
bool dup_care(int num_inputs, level_exp set_sop, level_exp reset_sop,
	      bool gate, lib_node* lib_element);

node* fill_lib_lib_cell(lib_node* lib_vec[], int num_lib_cells,
			char* name, int num_inputs, int min_del,
			int max_del, int area, char* expr, bool gc);

void find_num_levels(lib_node* lib_vec[], int num_lib_cells,
		     int num_levels, int& max_num_levels);

void find_num_levels_recurse(node* head, bool pred1_flag,
			     int num_levels, int& max_num_levels);

int find_num_lib_lits(int num_inputs, char* product);

void lib_decomp(node*& head, int num_inputs, level_exp set_sop,
		level_exp reset_sop, char** inp_labels, bool gate,
		bool plain_gc);

int load_library(char *library, lib_node* lib_vec[], double ratio,
		 int& num_lib_cells);

void strip_dou_invs(lib_node* lib_vec[], node* head);

void strip_dou_invs_recurse(node* head, bool pred1_flag);

#endif    // Ending _load_lib_h
