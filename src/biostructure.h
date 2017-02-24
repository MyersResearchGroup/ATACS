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
/* biostructure.h                                                                 */
/*****************************************************************************/

#ifndef _biostructure_h
#define _biostructure_h

#include "hash_map.h"
#include "test.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>

using namespace std;

//Seven different ways to build gates.  Act, Rep, CompAct, CompRep, ActAct, RepRep, ActRep
const int NUM_TYPE_GATES = 7;
enum GATE_ENUM {ACT=0, REP, COMP_ACT, COMP_REP, ACT_ACT, REP_REP, ACT_REP};
//max length of name of gate
const int MAX_LENGTH = 50;

/**
 *
 * This class contains the data on how a gate
 * may be constructed.
 * 
 **/ 
class gate_data {
public:
	string name;			//name of gate
	vector<vector<int> > gates;	//ways to build gate

gate_data();
gate_data(string new_name, vector<vector<int> > gate_builds);
};

/**
 *
 * This class contains a promoters
 * and what activates and repressed the promoter.
 * Contains private iters to allow easy iteration
 * without all the messiness of creation/declaration
 * 
 **/ 
class promoter {
public:
	string name;			//name promoter
	set<string, cmpstr> activate;	//what activates the promoter
	set<string, cmpstr> repress;	//what represses the promoter
	vector<bool> type;		//whether this promoter can act as a certain type
	vector<string> act_product;	//vector of complex activators
	vector<string> rep_product;	//vector of complex repressors	

promoter();
promoter(string new_name, set<string, cmpstr> activate, set<string, cmpstr> repress);
bool is_input(string name);
bool is_activator(string name);
bool is_repressor(string name);
bool is_type(GATE_ENUM gate);
int num_types();
int num_inputs();

private:
        void init_types();
};

/**
 *
 * This class contains interaction data
 * 
 **/ 
class interaction_data {
public:
	set<string, cmpstr> reactants;	//what reacts
	string product;			//the product of the reaction

interaction_data();
interaction_data(set<string, cmpstr> reactants, string product);
bool is_reactant(string name);
};

/**
 *  This class contains information about available genes
 */
class gene {
public:
	string name;		//name of protein

gene();
gene(string name);
};

//Enumeration of type of 
enum BIO_TYPE {PROTEIN_TYPE, PROMOTER_TYPE, COMPLEX_TYPE};

//struct that holds all interference/relationship information
//all are pointers because there is no need to duplicate data
//because all data is stored in the master_class
//currently all interferences are labeled purely by genes/protein
struct interference {
	string name;
	BIO_TYPE bio_type;
	hash_map<promoter*> promoters;
	hash_map<interaction_data*> interactions;
};

//type def for iterators
typedef set<string, cmpstr>::iterator string_iterator;

//TODO:  use a function pointer to initalize the cost function
/**
 *  This class keeps track of parts that are used with a promoter
 *  and keeps track of how many genes/promoters are knocked out by using
 *  the promoter 
 */
class used_parts {
public:	
	vector<gene*> used_genes;
	promoter* promoter_used;
	hash_map<gene*> knocked_genes;
	hash_map<promoter*> knocked_promoters;	
	
	used_parts(vector<gene*> used_genes, promoter* promoter_used, hash_map<gene*> knocked_genes, hash_map<promoter*> knocked_promoters);
	
	//TODO:  Fix all the const's
	int sum_cost();
	vector<int> promoter_cost() ;
	const bool operator<(used_parts &p);
private:
};

/**
 *  This class keeps track of all current parts, available parts
 *  and the gate information.  
 */
class master_class {
public:
	hash_map<gate_data> gate_builds;
  	hash_map<promoter> promoters; 
  	hash_map<interaction_data> interactions;
  	hash_map<gene> genes;
  	  	
  	//these variables must be initialized after all information
  	//has been parsed
  	vector<int> total_gates;
  	hash_map<interference> interferences;
  	
master_class();
void set_data(hash_map<gate_data> gate_builds, hash_map<promoter> promoters, hash_map<interaction_data> interactions, hash_map<gene> genes);

//initializes all the information gained from parsing
void init();
//Finds if biological structure exists
bool is_promoter(string name);
bool is_product(string product);
bool is_reactant(string name);
bool is_gene(string name);
vector<gene*> reacts_with(string name);
interaction_data* forms_product(vector<gene*> genes);
vector<gene*> get_genes(string product);

private:
//helper function
void tag_promoters();
};


class parts_handler {
public:
  	hash_map<promoter*> promoters; 
  	hash_map<gene*> genes;	
  	master_class* master;
  	vector<int> total_gates;
	
//constructor
parts_handler(master_class *total_parts, hash_map<promoter> &promoters, hash_map<gene> &genes);

//Finds if biological structure exists
bool is_promoter(string name);
bool is_gene(string name);

//Fills in the list of promoters used for a specified input
void promoters_used(vector<gene*> inputs, hash_map<promoter*> &promo_used);

//Determines if promoter is independent (has no affect on availability of other parts)
bool is_independent(promoter* promo, vector<gene*> inputs);

//Print total sum vector (lim sup of vectors)
void print_total();

//Uses the promoter-input pair as well as removes
//all parts that are knocked out by that pair
void use_gate(promoter* promo, vector<gene*> inputs);

//Figures out what parts are used if we use promoter-input pair
void parts_used(promoter* promo, vector<gene*> inputs, hash_map<promoter*> &promo_used, hash_map<gene*> &genes_used);

vector<vector<gene*> > gene_inputs(GATE_ENUM type, promoter* p);

//Removes a promoter
void remove_promoter(promoter *p);

//Removes a gene
void remove_gene(gene *g);

private:
	//sums all the parts
	void sum_parts();	
//void set_parts(master_class &total_parts, map<string, promoter, cmpstr> &promoters, map<string, gene, cmpstr> &genes);
};

ostream& operator<<(ostream& output, gate_data &g);
ostream& operator<<(ostream& output, promoter &p);
ostream& operator<<(ostream& output, interaction_data &i);
ostream& operator<<(ostream& output, gene &g);
ostream& operator<<(ostream& output, interference &i);
ostream& operator<<(ostream& output, master_class &m);
ostream& operator<<(ostream& output, parts_handler &p);
#endif

