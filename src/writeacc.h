// ***********************************************************************
// Outputs .acc files to help verify the correctness of the decomposition.
// ***********************************************************************

#ifndef _writeacc_h
#define _writeacc_h

void acc_bool_cel(designADT design, node* head);

void acc_bool_inv(designADT design, node* head);

void acc_bool_nand2(designADT design, node* head);

void build_acc_filename(designADT design, char outname[],
			char node_name[], node* head, int out_num);

void build_acc_out_exps(designADT design, node* output_vec[],
			bool rules_delay);

void clear_acc_visit(node* input_vec[], int tot_num_inputs);

void eval_acc_exprs(FILE* fp, designADT design, node* head,
		    int tot_num_inputs, node* input_vec[]);

void eval_acc_inits(designADT design, node* input_vec[],
		    int tot_num_inputs);

void eval_acc_inputs(designADT design, node* input_vec[],
		     int tot_num_inputs);

void itoa(char node_name[], node* head);

void make_acc_files(designADT design, node* input_vec[],
		    int out_num, int tot_num_inputs,
		    node* output_vec[], FILE* fpall,
		    int num_nodes_curt);

void make_all_acc_nodes(designADT design, node* input_vec[],
			int out_num, int tot_num_inputs,
			node* output_vec[], int num_nodes_curt);
  
void make_honkin_acc(FILE* fpall, designADT design, node* input_vec[],
		     int out_num, int tot_num_inputs,
		     node* output_vec[], int num_nodes_curt);
  
void make_internal_acc_nodes(designADT design, node* input_vec[],
			     int out_num, int tot_num_inputs,
			     node* output_vec[]);

void make_cge_file_acc(designADT design, node* output_vec[]);
  
void num_to_char(char temp[], int num);

void write_acc_cel(FILE* fp, designADT design, node* head, int base);

void write_acc_input(FILE* fp, designADT design);

void write_acc_inv(FILE* fp, designADT design, node* head, int base);

void write_acc_nand2(FILE* fp, designADT design, node* head, int base);

void write_acc_node(FILE* fp, designADT design, node* temp_head);


void write_acc_nodes(FILE* fp, designADT design, node* input_vec[],
		     int tot_num_inputs, int num_nodes_curt, int base);

void write_acc_output(FILE* fp, designADT design, node* output_vec[],
		      int out_num);

void write_acc_output_internal(FILE* fp, designADT design, node* head);

#endif    // Ending _writeacc_h
