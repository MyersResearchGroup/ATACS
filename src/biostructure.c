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
/* biostructure.c                                                            */
/*****************************************************************************/

#include "biostructure.h"
#include <iostream>

using namespace std;

//Seven different ways to build gates.  Act, Rep, CompAct, CompRep, ActAct, RepRep, ActRep
const char* GATE_NAMES[] = {"ACT", "REP", "COMP_ACT", "COMP_REP", "ACT_ACT", "REP_REP", "ACT_REP"};
//max length of name of gate

/**	gate_data class 		**/
gate_data::gate_data() {
}
gate_data::gate_data(string new_name, vector<vector<int> > gate_builds) {
	name = new_name;
	gates = gate_builds;
}
/**	End gate_data class 		**/
/**	gene class 			**/
//Constructor for gene
gene::gene(string new_name) {
	name = new_name;
}

gene::gene() {
}

/**	end gene class 			**/
/**	interaction_data class 		**/
//Constructor for interaction_data
interaction_data::interaction_data(set<string, cmpstr> new_reactants, string new_product) {
	reactants = new_reactants;
	product = new_product;
}

interaction_data::interaction_data() {
}

//Checks if protein is a reactant of the interaction
bool interaction_data::is_reactant(string name) {
	return reactants.find(name) != reactants.end();
}

/**	end interaction_data class	**/
/**	promoter class 			**/
//Constructor for promoter
promoter::promoter(string new_name, set<string, cmpstr> new_activate, set<string, cmpstr> new_repress) {
	name = new_name;
	activate = new_activate;
	repress = new_repress;
	init_types();
}

promoter::promoter() {
	init_types();
}	

//Determines whether or not a protein is activator/repressor/input
bool promoter::is_input(string name) {
	return is_activator(name) || is_repressor(name);
}

bool promoter::is_activator(string name) {
	return activate.find(name) != activate.end();
}

bool promoter::is_repressor(string name) {
	return repress.find(name) != repress.end();
}

int promoter::num_types() {
	int total = 0;
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		if (type[i]) {
			total++;
		}
		
	}
	return total;
}
bool promoter::is_type(GATE_ENUM gate) {
	return type[gate];
}

void promoter::init_types() {
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		type.push_back(false);
	}
}

int promoter::num_inputs() {
	return activate.size() + repress.size() + act_product.size() + rep_product.size();
}

/**	end promoter class		**/
/**	used_parts class 		**/
used_parts::used_parts(vector<gene*> used_genes, promoter* promoter_used, hash_map<gene*> knocked_genes, hash_map<promoter*> knocked_promoters) {
	this->used_genes = used_genes;
	this->promoter_used = promoter_used;
	this->knocked_genes = knocked_genes;
	this->knocked_promoters = knocked_promoters;
}
int used_parts::sum_cost() {
	int cost = 0;
	vector<int> temp = promoter_cost();
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		cost = cost+temp[i];
	}
	return cost;
	
}
vector<int> used_parts::promoter_cost() {
	vector<int> cost = vector<int>(NUM_TYPE_GATES);			
	for (int i = 0; i < knocked_promoters.size(); i++) {
		promoter* p = knocked_promoters.get_at_index(i);
		for (int j = 0; i < NUM_TYPE_GATES; j++) {	
			if (p->type[j]) {
				cost[j]++;
			}
		}
	}
	return cost;
}
	
const bool used_parts::operator<(used_parts &p) {
	return sum_cost() < p.sum_cost();
}
/**	end used_parts class		**/
/**	parts_handler class 		**/
parts_handler::parts_handler(master_class *total_parts, hash_map<promoter> &promoters, hash_map<gene> &genes) {
	this->master = total_parts;
	for (int i = 0; i < promoters.size(); i++) {
		promoter* promoter = &promoters.get_at_index(i);
		this->promoters.put(promoter->name, promoter);
	}
	for (int i = 0; i < genes.size(); i++) {
		gene* gene = &genes.get_at_index(i);	
		this->genes.put(gene->name, gene);
	}
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		total_gates.push_back(0);
	}
	sum_parts();
}

//Finds all inputs for a promoter that should be used for a given type.
//Example: type = ACT, promoter = pLac would return Lac
//TODO:  Should add error checking in later.
//TODO:  Need permutation of set, to add later, for now assume max of 2 inputs per gate, no complex in double input promoter
vector<vector<gene*> > parts_handler::gene_inputs(GATE_ENUM type, promoter* p) {
	vector<vector<gene*> > inputs;
	if (type == COMP_ACT || type == ACT) {
		for (unsigned int i = 0; i < p->act_product.size(); i++) {
			inputs.push_back(master->get_genes(p->act_product[i]));			
		}
	}
	if (type == COMP_REP || type == REP) {
		for (unsigned int i = 0; i < p->rep_product.size(); i++) {			
			inputs.push_back(master->get_genes(p->rep_product[i]));			
		}
	}
	if (type == ACT) {	
		for (unsigned int i = 0; i < p->activate.size(); i++) {			
			vector<gene*> v;
			v.push_back(&(master->genes.get(p->act_product[i])));
			inputs.push_back(v);
		}
	}
	if (type == REP) {
		for (unsigned int i = 0; i < p->repress.size(); i++) {
			vector<gene*> v;
			v.push_back(&(master->genes.get(p->rep_product[i])));
			inputs.push_back(v);
		}	
	}
	//TODO: Need to redo these with permutations
	if (type == ACT_ACT) {
		vector<gene*> v;
		v.push_back(&(master->genes.get(p->act_product[0])));
		v.push_back(&(master->genes.get(p->act_product[1])));
		inputs.push_back(v);
	}
	if (type == REP_REP) {
		vector<gene*> v;
		v.push_back(&(master->genes.get(p->rep_product[0])));
		v.push_back(&(master->genes.get(p->rep_product[1])));
		inputs.push_back(v);
	}
	if (type == ACT_REP) {
		vector<gene*> v;
		cout << master->genes.get(p->rep_product[0]) << endl;
		v.push_back(&(master->genes.get(p->act_product[0])));
		v.push_back(&(master->genes.get(p->rep_product[0])));
		inputs.push_back(v);	
	}
	return inputs;
}

void parts_handler::sum_parts() {
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		total_gates[i] = 0;
	}
	for (int i = 0; i < promoters.size(); i++) {	
		promoter* temp = promoters.get_at_index(i);
		for (int i = 0; i < NUM_TYPE_GATES; i++) {
			if (temp->type[i]) {
				total_gates[i]++;
			}
		}
	}
}

//TODO:  Fill this out
//Determines if promoter is independent (has no affect on availability of other parts)
bool parts_handler::is_independent(promoter* promo, vector<gene*> inputs) {
	return false;	
}

/*
	Figures out which parts will be used if a single promoter with some input is used.
	Pre:  inputs is a set of gene and a single promoter.  If the set of genes are used, must all combine
	together to form complex.  NOTE*Currently only supports 2 protein complex, also does not check for multiple complexes*
	Post: fills map with all the genes knocked out and promoters knocked out by using the piece.
 */
 //TODO:  Rework this to handle more complex cases.
 //TODO:  Break into more lines, simplify
void parts_handler::parts_used(promoter* promo, vector<gene*> inputs, hash_map<promoter*> &promo_used, hash_map<gene*> &genes_used) {
	//must go through all inputs	
	for (unsigned int i = 0; i < inputs.size(); i++) {
		//go through the interference list and add all promoters that use that input
		hash_map<promoter*> *p = &master->interferences.get(inputs[i]->name).promoters;
		for (int j = 0; j < p->size(); j++) {
			promoter* promo = p->get_at_index(j);
			promo_used.put(promo->name, promo);
		}
		genes_used.put(inputs[i]->name, inputs[i]);		
	}
	
	//now check to see if inputs combine to form product.  If so, remove all promoters
	//using the product
	bool forms_complex = false;
	interaction_data *data = NULL;
	for (int i = 0; i < master->interactions.size(); i++) {	
		interaction_data *temp = &master->interactions.get_at_index(i);
		bool reactant = true;
		for (unsigned int i = 0; i < inputs.size(); i++) {
			if (!temp->is_reactant(inputs[i]->name)) {
				reactant = false;
				break;
			}
			else {
				for (string_iterator g = temp->reactants.begin(); g != temp->reactants.end(); g++) {
					string my_gene =  *g;
					genes_used.put(my_gene, &(master->genes.get(my_gene)));
				}
			}
		}
		if (reactant == true) {
			forms_complex = true;
			data = temp;
			break;
		}
	}
	//if it does form a product mark all promoters that take the product as input
	if (forms_complex == true) {
		hash_map<promoter*> *p = &(master->interferences.get(data->product).promoters);  
		for (int i = 0; i < p->size(); i++) {
			promo_used.put(p->get_at_index(i)->name, p->get_at_index(i));
		}
	}	
}

//Use promoter with inputs, removes parts that can no longer be used.
void parts_handler::use_gate(promoter* promo, vector<gene*> inputs) {
	hash_map<promoter*> promo_used;
	hash_map<gene*> genes_used;	
	parts_used(promo, inputs, promo_used, genes_used);
	for (int i = 0; i < promo_used.size(); i++) {
		promoters.erase(promo_used.get_at_index(i)->name);
	}
	for (int i = 0; i < genes_used.size(); i++) {
		genes.erase(genes_used.get_at_index(i)->name);
	}	
}

//Remove promoter from list of usables
void parts_handler::remove_promoter(promoter *p) {
	if (p != NULL && promoters.exists(p->name)) {
		promoters.erase(p->name);
		sum_parts();
	}
}

//Remove gene from list of usables
void parts_handler::remove_gene(gene *g) {
	if (g != NULL && genes.exists(g->name)) {
		genes.erase(g->name);
	}
}

bool parts_handler::is_promoter(string name) {
	return promoters.exists(name);
}

bool parts_handler::is_gene(string name) {
	return genes.exists(name);
}

void parts_handler::print_total() {
	cout << "[";
	for (int i = 0; i < NUM_TYPE_GATES; i++) {
		cout << total_gates[i] << " ";
	}
	cout << "]\n";
}


void parts_handler::promoters_used(vector<gene*> inputs, hash_map<promoter*> &promo_used) {
	//check to see if it's an interaction combination or a regular reaction
	//if inputs has size of 1, then it is a single protein, if it has more, it's a complex formation
	if (inputs.size() == 1) {		
		if (master->interferences.exists(inputs[0]->name)) {
			interference temp = master->interferences.get(inputs[0]->name);
			for (int i = 0; i < temp.promoters.size(); i++) {
				promo_used.put(temp.promoters.get_at_index(i)->name, temp.promoters.get_at_index(i));
			}
		}
	}
	else if (inputs.size() > 1) {
          for (unsigned int i = 0; i < inputs.size(); i++) {
		if (master->interferences.exists(inputs[i]->name)) {
			interference temp = master->interferences.get(inputs[i]->name);
			for (int j = 0; j < temp.promoters.size(); j++) {
				promo_used.put(temp.promoters.get_at_index(j)->name, temp.promoters.get_at_index(j));
			}
		}
          }
          interaction_data* data = master->forms_product(inputs);
          if (master->interferences.exists(data->product)) {
	    interference temp = master->interferences.get(data->product);
       	    for (int j = 0; j < temp.promoters.size(); j++) {   
              promo_used.put(temp.promoters.get_at_index(j)->name, temp.promoters.get_at_index(j));
            }
	  }
	}
	else {
		cerr << "Illegal State:  Too few inputs to parts_handler::promoters_used" << endl;
    		//exit (-1);
	}	
}

/**	end parts_handler class		**/
/**	master_class class 		**/
//Empty constructor
master_class::master_class() {
}

//TODO:  Need Error checking
vector<gene*> master_class::get_genes(string product) {
	vector<gene*> inputs;
	interaction_data* data = &interactions.get(product);
	for (string_iterator iter = data->reactants.begin(); iter != data->reactants.end(); iter++) {
		inputs.push_back(&genes.get(*iter));
	}
	return inputs;	
}

void master_class::set_data(hash_map<gate_data> gates, hash_map<promoter> p, hash_map<interaction_data> i, hash_map<gene> g) {
	gate_builds = gates;
	genes = g;
	promoters = p;
	interactions = i;
}

bool master_class::is_promoter(string name) {
	return promoters.exists(name);
}
bool master_class::is_product(string product) {
	return interactions.exists(product);
}
bool master_class::is_gene(string name) {
	return genes.exists(name);
}

bool master_class::is_reactant(string name) {
	return (reacts_with(name).size() > 0);
}

interaction_data* master_class::forms_product(vector<gene*> reactants) {
	//go through all interaction data
	for (int i = 0; i < interactions.size(); i++) {
		interaction_data* data = &interactions.get_at_index(i);
		//check to see if each gene is a member of the data
		bool correct_data = true;				
		for (unsigned int j = 0; j < reactants.size(); j++) {
			if (!data->is_reactant(reactants[j]->name)) {
				correct_data = false;
				break;
			}
		}
		if (reactants.size() != data->reactants.size()) {
			correct_data = false;
		}
		if (correct_data) {
			return data;
		}
	}	
	return NULL;
}

vector<gene*> master_class::reacts_with(string name) {
	vector<gene*> gene_list;
	for (int i = 0; i < interactions.size(); i++) {
		interaction_data temp = interactions.get_at_index(i);
		if (temp.is_reactant(name)) {
			for (string_iterator s = temp.reactants.begin(); s != temp.reactants.end(); s++) {
				//NOTE:  Currently includes gene listed
				gene_list.push_back(&genes.get(*s));
			}
		}
	}
	return gene_list;
}

void master_class::tag_promoters() {
	for (int i = 0; i < promoters.size(); i++) {
		promoter* promo = &promoters.get_at_index(i);
		//initialize type vector
		for (int i = 0; i < NUM_TYPE_GATES; i++) {
			promo->type.push_back(false);
		}
		if (promo->activate.size() > 0) {
			promo->type[ACT] = true;
		}
		if (promo->activate.size() > 1) {
			promo->type[ACT_ACT] = true;
		}
		if (promo->repress.size() > 0) {
			promo->type[REP] = true;
		}
		if (promo->repress.size() > 1) {
			promo->type[REP_REP] = true;
		}		
		if (promo->type[ACT] && promo->type[REP]) {
			promo->type[ACT_REP] = true;
		}
		//now find all complex products
		for (string_iterator temp = promo->activate.begin(); temp != promo->activate.end(); temp++) {
			if (is_product(*temp)) {
				promo->type[COMP_ACT] = true;
				promo->act_product.push_back(*temp);
			}			
		}
		for (string_iterator temp = promo->repress.begin(); temp != promo->repress.end(); temp++) {
			if (is_product(*temp)) {
				promo->type[COMP_REP] = true;
				promo->rep_product.push_back(*temp);
			}			
		}
	}
}

void master_class::init() {
	//TODO refactor out helper code

	//cycle through gene set to find all possible proteins
	//and find all promoters it interacts with and all proteins it combines with
	for (int i = 0; i < genes.size(); i++) {
		interference temp;
		temp.bio_type = PROTEIN_TYPE;
		gene curr_gene = genes.get_at_index(i);
		temp.name = curr_gene.name;                
		//now find all interactions that have the protein as a reactant
		for(int j = 0; j < interactions.size(); j++) {
			interaction_data* curr_inter = &interactions.get_at_index(j);
			if (curr_inter->is_reactant(curr_gene.name)) {
				temp.interactions.put(curr_inter->product, curr_inter);
			}
		}
                
                //now make a list of all proteins that react with this protein
                hash_map<gene*> reactants;
                for (int j = 0; j < temp.interactions.size(); j++) {
                  set<string, cmpstr> *set_reactants = &(temp.interactions.get_at_index(j)->reactants);
                  for (string_iterator iter = set_reactants->begin(); iter != set_reactants->end(); iter++) {
                    gene* to_add = &genes.get(*iter);
                    reactants.put(to_add->name, to_add);
                  }
                }
                
		//now find all promoters that have this protein as an input or any of the complex products
                //or have any of its complex reactants as input
		for(int j = 0; j < promoters.size(); j++) {
			promoter *curr_promoter = &promoters.get_at_index(j);
			if (curr_promoter->is_input(curr_gene.name)) {
				temp.promoters.put(curr_promoter->name, curr_promoter);
			}		
			//Now go through the interactions that the protein belongs to
			//if the interaction product is input to the promoter, add the promoter
			//to the set of promoters this gene interacts with.
			for (int k = 0; k < temp.interactions.size(); k++) {
				interaction_data* curr_inter = temp.interactions.get_at_index(k);
				if (curr_promoter->is_input(curr_inter->product)) {
					temp.promoters.put(curr_promoter->name, curr_promoter);
				}
			}
                        
                        //now finally, cycle through all reactant proteins
                        for (int k = 0; k < reactants.size(); k++) {
                           if (curr_promoter->is_input(reactants.get_at_index(k)->name)) {
			     temp.promoters.put(curr_promoter->name, curr_promoter);
			   }		
                        }
		}
				
		interferences.put(temp.name, temp);
	}
	
	//now cycle through all complexes
	for (int i = 0; i < interactions.size(); i++) {
		interference temp;
		interaction_data* inter = &interactions.get_at_index(i);
		temp.bio_type = COMPLEX_TYPE;
		temp.name = interactions.get_at_index(i).product;
		//now find all promoters that have this complex as an input
		for(int j = 0; j < promoters.size(); j++) {
			promoter *curr_promoter = &promoters.get_at_index(j);
			if (curr_promoter->is_input(temp.name)) {
				temp.promoters.put(curr_promoter->name, curr_promoter);
			}
			//now check to see if promoter uses any of the inputs of the reaction
			for (string_iterator iter = inter->reactants.begin(); iter != inter->reactants.end(); iter++) {
				if (curr_promoter->is_input(*iter)) {
					temp.promoters.put(curr_promoter->name, curr_promoter);
				}
			}
		}
		interferences.put(temp.name, temp);
	}
	
	tag_promoters();
	
	//next cycle through interactions and find 
	//cycle through all promoters to determine what proteins are used as inputs/outputs
	/**
		This entire chunk of code may be useless.  I believe that it may only be necessary
		to deal with interactions on a purely input basis.
	**/
	/*
	for (promoter_iterator promo_iter = promoters.begin(); promo_iter != promoters.end(); promo_iter++) {
		interference temp;
		temp.bio_type = PROMOTER_TYPE;
		promoter curr_promo = ((*promo_iter).second);
		//now cycle through all activators and repressors of promoters
		for (string_iterator iter = curr_promo.activate.begin(); iter != curr_promo.activate.end(); iter++) {
			//start by going through all promoters to find if protein interacts with it
			for (promoter_iterator promo = promoters.begin(); promo != promoters.end(); promo++) {
				promoter inner_promo = (*promo).second;
				if (inner_promo.name != curr_promo.name) {
					if (inner_promo.is_input(*iter)) {
						temp.promoters[inner_promo.name] = &inner_promo;
					}
				}
			}
			//now find all interactions that have the protein as a reactant
			for(interact_iterator act = interactions.begin(); act != interactions.end(); act++) {
				interaction_data curr_inter = (*act).second;
				if (curr_inter.is_reactant(*iter)) {
					temp.interactions[curr_inter.product] = &curr_inter;	
				}
			}
		}
		for (string_iterator iter = curr_promo.repress.begin(); iter != curr_promo.repress.end(); iter++) {
			//start by going through all promoters to find if protein interacts with it
			for (promoter_iterator promo = promoters.begin(); promo != promoters.end(); promo++) {
				promoter inner_promo = (*promo).second;
				if (inner_promo.name != curr_promo.name) {
					if (inner_promo.is_input(*iter)) {
						temp.promoters[inner_promo.name] = &inner_promo;
					}
				}
			}
			//now find all interactions that have the protein as a reactant
			for(interact_iterator act = interactions.begin(); act != interactions.end(); act++) {
				interaction_data curr_inter = (*act).second;
				if (curr_inter.is_reactant(*iter)) {
					temp.interactions[curr_inter.product] = &curr_inter;	
				}
			}
		}				
		interferences[temp.name] = temp;
	}		
	*/
}
/**	end master_class class		**/


/**	operator overloads		**/
ostream& operator<<(ostream& output, gate_data &g) {	
	output << g.name;
	for (unsigned int i = 0; i < g.gates.size(); i++) {
		output << "\n\t[";
		vector<int> temp = ((g.gates)[i]);
		for(int j = 0; j < NUM_TYPE_GATES; j++) {
			output << temp[j] << " " << GATE_NAMES[j] << "  ";
		}
		output << "]";
	}
	
	return output;
}

ostream& operator<<(ostream& output, promoter &p) {
	output << "Promoter " << p.name;
	output << "\n\tActivated: ";
	for (string_iterator i = p.activate.begin(); i != p.activate.end(); i++) {
		output << *i << " ";
	}
	output << "\n\tRepressed: ";
	for (string_iterator i = p.repress.begin(); i != p.repress.end(); i++) {
		output << *i << " ";
	}
	output << "Type: [";
	for (unsigned int i = 0; i < p.type.size(); i++) {
		if (p.type[i]) {
			output << "1 ";
		}
		else {
			output << "0 ";
		}
		
	}
	output << "]\n";
	return output;
}
ostream& operator<<(ostream& output, interaction_data &id) {
	output << "Reaction\n\tReactants: ";
	for (string_iterator i = id.reactants.begin(); i != id.reactants.end(); i++) {
		output << *i << " ";
	}
	output << "\n\tProduct: " << id.product;
	return output;
}
ostream& operator<<(ostream& output, gene &g) {
	output << "Gene " << g.name;
	return output;
}
ostream& operator<<(ostream& output, interference &i) {
	output << "Interference " << i.name << endl;
	hash_map<promoter*> *promoters = &i.promoters;
	output << "Input to promoters: " << promoters->size() << " ";	
	for (int j = 0; j < promoters->size(); j++) {
		output << "\t" << promoters->get_at_index(j)->name;	
	}
	output << "\nIn reactions: ";
	hash_map<interaction_data*>* interactions = &i.interactions;        
	for (int j = 0; j < interactions->size(); j++) {               
                cout << "\tSize: " << interactions->get_at_index(j)->reactants.size() << endl; 
                cout << "\tProduct: " << interactions->get_at_index(j)->product << "!" << endl;
		output << "\t" << interactions->get_at_index(j)->product;
	}		
	return output;
}

ostream& operator<<(ostream& output, parts_handler &p) {  		
  	output << "Promoters Available: ";
  	hash_map<promoter*> *promoters = &p.promoters;
  	for (int i = 0; i < promoters->size(); i++) {
  		output << *promoters->get_at_index(i) << endl;
  	}
  	output << "Genes Available: ";
  	hash_map<gene*> *genes = &p.genes;
  	for (int i = 0; i < genes->size(); i++) {
  		output << "GENE NAME: " << (genes->get_at_index(i))->name << endl;
  	}  	
  	output << "Sum Parts: ";
  	for (int i = 0; i < NUM_TYPE_GATES; i++) {
  		output << p.total_gates[i] << " ";
  	}
  	output << endl;
  	return output;  	
}

ostream& operator<<(ostream& output, master_class &m) {
	//This function prints out the master_class.  Funky scoping
	//is used because there's no base object class that allows me
	//to do what I want to do.
	hash_map<gate_data> *all_gates = &(m.gate_builds);
	hash_map<promoter> *promoters_set = &(m.promoters);
	hash_map<interaction_data> *interactions_set = &(m.interactions);
	hash_map<gene> *genes_set = &(m.genes);
	hash_map<interference> *interference_set = &(m.interferences);

	for (int i = 0; i < all_gates->size(); i++) {
		output << all_gates->get_at_index(i) << endl;
	}	
	
	for (int i = 0; i < promoters_set->size(); i++) {
		output << promoters_set->get_at_index(i) << endl;
	}	

	for (int i = 0; i < interactions_set->size(); i++) {
		output << interactions_set->get_at_index(i) << endl;
	}	

	for (int i = 0; i < genes_set->size(); i++) {
		output << genes_set->get_at_index(i) << endl;
	}	

	for (int i = 0; i < genes_set->size(); i++) {
		output << genes_set->get_at_index(i) << endl;
	}	
	
	for (int i = 0; i < interference_set->size(); i++) {
		output << interference_set->get_at_index(i) << endl;
	}	
	return output;
}
