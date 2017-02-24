/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* biomap.c                                                                 */
/*****************************************************************************/

#include <stdio.h>
#include "biomap.h"

#ifdef __BIO_VERBOSE__
  int RECURSIVE_DEPTH = 0;
#endif

//From Parse file
char* build_file = "test.build";
char* interact_file = "test.interact";
extern void parseGateFile(hash_map<gate_data> *gate_builds);
extern void loadGateFile(char*);
extern void parseInteractFile(hash_map<promoter> *promoters_set, hash_map<interaction_data> *interactions_set, hash_map<gene> *genes_set);
extern void loadInteractFile(char*);

void init_bio_map(master_class &bio) {
  //After decomp, input_vec contains decomped structure
  //in netlist form.  Now can match and cover it.
  
  hash_map<gate_data> gate_builds;
  hash_map<promoter> promoters; 
  hash_map<interaction_data> interactions;
  hash_map<gene> genes;
  
  loadGateFile(build_file);    
  parseGateFile(&gate_builds);
  
  loadInteractFile(interact_file);    
  parseInteractFile(&promoters, &interactions, &genes);
  
  bio.set_data(gate_builds, promoters, interactions, genes);
  bio.init();  
#ifdef __BIO_VERBOSE__  
  cout << bio << endl;
#endif
}

//Runs biomap
void bio_map(designADT design) {
  // Linked list of outputs for entire design
  node* output_vec[design->nsignals - design->ninpsig];
  
  // Linked list of inputs for entire design
  node* input_vec[INPUT_VEC_SIZE];  
  
  lib_node* lib_vec[MAXNUMLIBCELLS];
  int num_lib_cells=0;  
    
  //Due to a function call build_acc_out_exps
  //output_vec gets wiped out.  It would be easy
  //enough to fix, but side effects are unknown
  //ignore for now.
  decomp(design, output_vec, input_vec, lib_vec, num_lib_cells);
  //do all set
  master_class bio = master_class();
  init_bio_map(bio);
  
  parts_handler all_parts = parts_handler(&bio, bio.promoters, bio.genes);
  all_parts.print_total();
  
  run_hash_test(true);
  run_biostructure_test(true);  
  
  //test_parts_used(&bio, &all_parts);
  
  //testing to see if it correctly removes promoters and genes
  int test[] = {1, 1, 1, 1, 1, 1, 1};
  //test_independent_parts(&bio, &all_parts, test);
  //test_promoters_affected(&bio, &all_parts);
  //cout << "Before sats:\n**************************** \n" << all_parts << endl;
  bool sat = is_satisfiable(all_parts, test);
  cout << "Is sats? " << sat << endl;
  //cout << "After sats:\n**************************** \n" << all_parts << endl;
  cout << "Parts to fill: ";
  for (int i = 0; i < NUM_TYPE_GATES; i++) {
    cout << test[i] << " ";
  }
}

void test_independent_parts(master_class* bio, parts_handler* all_parts, int test[]) {
  cout << "Parts to fill: ";
  for (int i = 0; i < NUM_TYPE_GATES; i++) {
    cout << test[i] << " ";
  }
  cout << endl;
  cout << "Before use of independent:\n" << *all_parts;
  use_independent_part(*all_parts, test);
  cout << "After use of independent:\n" << *all_parts;  
  cout << "Parts to fill: ";
  for (int i = 0; i < NUM_TYPE_GATES; i++) {
    cout << test[i] << " ";
  }
  
}

void test_promoters_affected(master_class* bio, parts_handler* all_parts) {
  cout << "Test Promoters Affected" << endl;
  vector<gene*> inputs;
  inputs.push_back(&bio->genes.get("LacI"));
  hash_map<promoter*> promo_used;
  cout << "Testing LacI: ";
  all_parts->promoters_used(inputs, promo_used);
  for (int i = 0; i < promo_used.size(); i++) {
    cout << *(promo_used.get_at_index(i)) << endl;
  }
  cout << "End Promoters Affected" << endl;
}

//This is a test function, makes sure parts used behaves correctly.
void test_parts_used(master_class* bio, parts_handler* all_parts) {
  //Try using antipLux promoter (repressed by LuxRLuxI complex)
  //should use   
  hash_map<promoter*> promo_used;
  hash_map<gene*> genes_used;
  promoter* promo = &(bio->promoters.get("antipLux"));
  vector<gene*> inputs;
  inputs.push_back(&bio->genes.get("LuxR"));
  inputs.push_back(&bio->genes.get("LuxI"));
  all_parts->parts_used(promo, inputs, promo_used, genes_used);
  
  cout << "Using antipLux promoter" << endl;
  cout << "PROMOTERS USED: " << endl;
  for (int i = 0; i < promo_used.size(); i++) {
    cout << *(promo_used.get_at_index(i)) << endl;
  }
  for (int i = 0; i < genes_used.size(); i++) {
    cout << *(genes_used.get_at_index(i)) << endl;
  }
  
  promo_used.clear();
  genes_used.clear();
  inputs.clear();
  promo = (&bio->promoters.get("pNam"));
  inputs.push_back(&bio->genes.get("TetR"));
  all_parts->parts_used(promo, inputs, promo_used, genes_used);
  
  cout << "\nUsing pNam promoter" << endl;
  cout << "PROMOTERS USED: " << endl;
  for (int i = 0; i < promo_used.size(); i++) {
    cout << *(promo_used.get_at_index(i)) << endl;
  }
  for (int i = 0; i < genes_used.size(); i++) {
    cout << *(genes_used.get_at_index(i)) << endl;
  }
  
  //now test to make sure copy constructor working right
  parts_handler temp_parts = *(all_parts);
  cout << "Checking TetR exists: ";
  cout << (all_parts->genes.exists("TetR"))<< endl;
  all_parts->genes.erase("TetR");
  cout << "Erasing TetR, still exists: ";
  cout << (all_parts->genes.exists("TetR"))<< endl;  
  cout << "Checking Copy: ";
  cout << (temp_parts.genes.exists("TetR"))<< endl;
}

//This function prints out the values parsed.  Scoping levels used to hide
//variables with the same name so that cut and paste can be used.  Wouldn't
//have this problem if this was in Java.
void printParsed(hash_map<gate_data> *all_gates, hash_map<promoter> *promoters_set, hash_map<interaction_data> *interactions_set, hash_map<gene> *genes_set) {	

	for (int i = 0; i < all_gates->size(); i++) {
		cout << all_gates->get_at_index(i) << endl;
	}

	for (int i = 0; i < promoters_set->size(); i++) {
		cout << promoters_set->get_at_index(i) << endl;
	}

	for (int i = 0; i < interactions_set->size(); i++) {
		cout << interactions_set->get_at_index(i) << endl;
	}

	for (int i = 0; i < genes_set->size(); i++) {
		cout << genes_set->get_at_index(i) << endl;
	}
}

bool is_satisfiable(parts_handler &all_parts, int to_sat[]) {	
	use_independent_part(all_parts, to_sat);
	if (!enough_available(all_parts, to_sat)) {
		return false;
	}
	bool solved = false;
	reduce(all_parts, to_sat, solved);		
	return done(to_sat);
}

//Checks to see if there's enough parts available to satisfy the matrix.
//This is only a theoretical max, and just because this returns true
//does not mean that we can satisfy the matrix.
bool enough_available(parts_handler &all_parts, int to_sat[]) {
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		if (to_sat[i] > all_parts.total_gates[i]) {
			return false;
		}
	}
	return true;
	
}

void reduce(parts_handler &all_parts, int to_sat[], bool& solution_found) {
	vector<vector<promoter*> > promoters = sort_type(all_parts);
	//start by reducing the complex activators
	for (int g = ACT_REP; g >= ACT; g = g - 1) {
		if (to_sat[g] > 0) {	
			//go through all the complex act promoters
			vector<used_parts> gates;
			for (unsigned int i = 0; i < promoters[g].size(); i++) {
				//call to use on each one to find the parts that will be used.
				cout << "!!!!!!!!!!!!!!!!!!" << "\n" << all_parts.master << endl;		
				vector<vector<gene*> > genes = all_parts.gene_inputs((GATE_ENUM)g, promoters[g][i]);
				//now cycle through each set of genes to figure out the cost of each input
				for (unsigned int j = 0; j < genes.size(); j++) {
					hash_map<promoter*> promo_used;
					hash_map<gene*> genes_used;
					all_parts.parts_used(promoters[g][i], genes[j], promo_used, genes_used);	
					used_parts temp= used_parts(genes[j], promoters[g][i], genes_used, promo_used);
					gates.push_back(temp);
				}
			}
			
			//now go through and sort the parts based on the cost function
                        //TODO:  Write Sort Algorithm
			//sort(gates.begin(), gates.end());
			for (unsigned int i = 0; i < gates.size(); i++) {
				//make a copy and split and recurse
				parts_handler new_parts = all_parts;
				int new_to_sat[NUM_TYPE_GATES];
				copy_array(new_to_sat, to_sat);
				new_to_sat[g]--;
				new_parts.use_gate(gates[i].promoter_used, gates[i].used_genes);
				reduce(new_parts, new_to_sat, solution_found);
				if (done(new_to_sat)) {
					solution_found = true;
					copy_array(to_sat, new_to_sat);
					return;
				}
			}
		}
	}	
	solution_found = false;
}

bool done(int to_sat[]) {	
	int sum = 0;
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		sum = sum + to_sat[i];
	}
	return sum == 0;
}

void copy_array(int new_to_sat[], int to_sat[]) {
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		new_to_sat[i] = to_sat[i];
	}
}

hash_map<gene*> make_map(vector<gene*> genes) {
	hash_map<gene*> gene_map;
	for (unsigned int i = 0; i < genes.size(); i++) {
		gene_map.put(genes[i]->name, genes[i]);
	}
	return gene_map;
}

//Sorts the promoters into different sets depending on the type. 
vector<vector<promoter*> > sort_type(parts_handler &all_parts) {
	vector<vector<promoter*> > promoters = vector<vector<promoter*> >(NUM_TYPE_GATES);
        for (int i = 0; i < all_parts.promoters.size(); i++) {	
		promoter* p = all_parts.promoters.get_at_index(i);
		for (int j = 0; j < NUM_TYPE_GATES; j++) {
			if (p->type[j]) {
				promoters[j].push_back(p);
			}
		}
	}
	return promoters;
}

/*
//Reduces the problem by using parts who's availability will have no affect on the existence of solution
//TODO:  This may actually be use independent part
//TODO:  May not take into account for genes/product already in system
void reduce(parts_handler &all_parts, int to_sat[]) {
	//Keeps track of what parts need to be removed.
        map<string, promoter*, cmpstr> remove_promoter;
        map<string, gene*, cmpstr> remove_gene;	

	for (interference_iterator inter = all_parts.master->interferences.begin(); inter != all_parts.master->interferences.end(); inter++) {
		interference temp = (*inter).second;
		//see if item only has 1 promoter, if so, use it.
		if (temp.promoters.size() == 1) {
			promoter* p = (*temp.promoters.begin()).second;
			//Check if promoter is available
			if (all_parts.is_promoter(p->name) && remove_promoter.find(p->name) == remove_promoter.end()) {	
				//Try to fill up the promoters going backward since the later ones are harder to fill
				for (int i = NUM_TYPE_GATES - 1; i >= 0; i--) {
					//We need this type of gate
					if (p->type[i] && to_sat[i] > 0) {					
						remove_promoter[p->name] = p;						
						//Now find out what is used as input to promoter
						//TODO: THINK THIS OUT MORE
						for (string_iterator s = p->activate.begin(); s != p->activate.end(); s++) {
							if (all_parts.master->genes.find(*s) != all_parts.master->genes.end())
							remove_gene[*s] = &((*all_parts.master->genes.find(*s)).second);
						}
						for (string_iterator s = p->repress.begin(); s != p->repress.end(); s++) {
							if (all_parts.master->genes.find(*s) != all_parts.master->genes.end())
							remove_gene[*s] = &((*all_parts.master->genes.find(*s)).second);
						}
						if (p->act_product.size() >= 1) {
							interaction_data* data = &(*all_parts.master->interactions.find(p->act_product[0])).second;
							for (string_iterator s = data->reactants.begin(); s != data->reactants.end(); s++) {
								if (all_parts.master->genes.find(*s) != all_parts.master->genes.end())
								remove_gene[(*s)] = &((*all_parts.master->genes.find(*s)).second);
							}	
						}
						if (p->rep_product.size() >= 1) {
							interaction_data* data = &(*all_parts.master->interactions.find(p->rep_product[0])).second;
							for (string_iterator s = data->reactants.begin(); s != data->reactants.end(); s++) {
								if (all_parts.master->genes.find(*s) != all_parts.master->genes.end())
								remove_gene[(*s)] = &((*all_parts.master->genes.find(*s)).second);
							}	
						}
						to_sat[i]--;
						break;
					}					
				}
			}
		}
	}
	
	for(Ppromoter_iterator iter = remove_promoter.begin(); iter != remove_promoter.end(); iter++) {
        	all_parts.remove_promoter((*iter).second);	
	}
	for(Pgene_iterator iter = remove_gene.begin(); iter != remove_gene.end(); iter++) {	
		all_parts.remove_gene((*iter).second);	        	
	}	


	//Go through each promoter and use the promoters that have genes that affect no other 
	//promoters.
	for (Ppromoter_iterator iter = all_parts.promoters.begin(); iter != all_parts.promoters.end(); iter++) {
		promoter *p = (*iter).second;
		//Now check the type of the promoter and see if it's needed, start by checking to see if it's
		//it's more complex than a simple activator or repressor and see if those are needed.
		//TODO:  Currently, things can only be 1 type of the complex reactions, so this section will
		//require modification once we allow multiple different types.
				
		
		
		
	}
	
}
*/

//NOTE:  May be case that gate can only be used one way if and only if
//the genes are not supplied.  Fix this case later
//May be the case that I need to only check for promoters where using it
//will not knock out any other promoters.
void use_independent_part(parts_handler &all_parts, int to_sat[]) {         
        vector<promoter*> remove_promoter;
        vector<gene*> remove_gene;	
        //iterate through all the existing genes
	for (int i = 0; i < all_parts.genes.size(); i++) {
		//get the interference data for the current gene
                gene* g = all_parts.genes.get_at_index(i);
                interference* interfere = &(all_parts.master->interferences.get(g->name));

		//check to see if the number promoters the gene matches is 1 and that it is involved
		//in no reactions.  If so, it is an independent part, we can use it if necessary and it will
		//have no effect on whether we have a solution or not.
		if (interfere->promoters.size() == 1 && interfere->interactions.size() == 0) {
			promoter* p = interfere->promoters.get_at_index(0);			
			if (p->num_types() == 1 && p->num_inputs() == 1) {
				if (to_sat[ACT] > 0 && p->is_type(ACT)) {
					to_sat[ACT]--;
					remove_promoter.push_back(p);
					remove_gene.push_back(all_parts.genes.get(*(p->activate.begin())));
				}
				else if (to_sat[REP] > 0 && p->is_type(REP)) {
					to_sat[REP]--;
					remove_promoter.push_back(p);
					remove_gene.push_back(all_parts.genes.get(*(p->repress.begin())));
				}				
			}
		}
	}
	

	//Now go through the interaction data, and find all those that only affect 1 promoter and use those.
	for (int i = 0; i < all_parts.master->interferences.size(); i++) {
		interference* inter = &(all_parts.master->interferences.get_at_index(i));
		if (inter->promoters.size() == 1) {
			//get the promoter and see if needed
		}
	}
	
	for(unsigned int i = 0; i < remove_promoter.size(); i++) {
        	all_parts.remove_promoter(remove_promoter[i]);	
	}
	for(unsigned int i = 0; i < remove_gene.size(); i++) {
        	all_parts.remove_gene(remove_gene[i]);	
	}	
}
