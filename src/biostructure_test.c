/**
 *    This file tests the biostructure classes.  This file must be run with test.genlib, test.build, test.interact
 *
 */

#include "test.h"
#include "biostructure.h"
#include "biomap.h"

master_class bio;

void init_test(master_class &bio);
void test_gate_data(bool flag);
void test_genes(bool flag);
void test_promoter(bool flag);
void test_interaction(bool flag);
void test_interference(bool flag);
void test_master_class(bool flag);
void test_parts_handler(bool flag);


void run_biostructure_test(bool flag) {  
  my_print("Testing biostructure classes", flag);
  init_bio_map(bio);
  test_gate_data(flag);
  test_genes(flag);
  test_promoter(flag);
  test_interaction(flag);
  test_interference(flag);
  test_master_class(flag);
  test_parts_handler(flag);
  //TODO:  add used_parts class
  my_print("Finished biostructure classes", flag);
}

void test_gate_data(bool flag) {
  my_print("Testing gate data", flag);
  gate_data gate = bio.gate_builds.get("buf:combinational");
  
  //now check to make sure it can be built using activators
  bool found = false;
  for (unsigned int i = 0; i < gate.gates.size(); i++) {
    if (gate.gates[i][ACT] == 1) {
      found = true;
    }    
  }
  if (!found) {
    error("Failed to load gates");
  }
}

void test_genes(bool flag) {
  gene temp = bio.genes.get("A1");
  if (temp.name != "A1") {
    error("Failed to load genes");
  }
}

void test_promoter(bool flag) {
  my_print("Testing promoter", flag);
  promoter act = bio.promoters.get("pA1");
  if (act.name != "pA1") {
    error("Failed to load genes");
  }
  if (!act.is_input("A1")) {
    error("Incorrect input");
  }
  if (act.is_input("foo")) {
    error("Incorrect input");
  }
  if (!act.is_activator("A1")) {
    error("Incorrect input");
  }
  if (act.is_repressor("A1")) {
    error("Incorrect input");
  }
  if (!act.is_type(ACT)) {
    error("Incorrect gate type");
  }
  if (act.is_type(ACT_ACT)) {
    error("Incorrect gate type");
  }
  if (act.num_types() != 1) {
    error("Incorrect numer of gate types");
  }
  if (act.num_inputs() != 1) {
    error("Incorrect number of inputs");
  }    
}

void test_interaction(bool flag) {
  my_print("Testing interaction", flag);
  interaction_data data = bio.interactions.get("AB3");
  if (data.product != "AB3") {
    error("Incorrectly load test file");
  }
  if (!data.is_reactant("AB1") || !data.is_reactant("AB2") || data.is_reactant("AB3")) {
    error("Incorrectly tracking reactants");
  }
}

void test_interference(bool flag) {
  my_print("Testing interference", flag);
  interference temp = bio.interferences.get("AB3");
  if (temp.name != "AB3") {
    error("Incorrectly loaded file");
  }
  if (!temp.promoters.exists("pAB1") || !temp.promoters.exists("pAB2") || !temp.promoters.exists("pAB3")) {
    error("Incorrectly configured intereference:  Couldn't find promoters");
  }
  if (temp.promoters.size() != 3) {
    error("Incorrectly configured intereference:  Too many/few promoters");
  }  
  
  temp = bio.interferences.get("A1");
  if (temp.name != "A1") {
    error("Incorrectly loaded file");
  }
  if (!temp.promoters.exists("pA1")) {
    error("Incorrectly configured intereference:  Couldn't find promoters");
  }
  if (temp.promoters.size() != 1) {
    error("Incorrectly configured intereference:  Too many/few promoters");
  }
}

void test_master_class(bool flag) {
  my_print("Testing master_class", flag);
  if (!bio.is_promoter("pA1") || bio.is_promoter("foo")) {
    error("is_promoter not working correctly");
  }
  if (!bio.is_product("AB3") || bio.is_product("A1")) {
    error("is_product not working correctly");
  }
  if (!bio.is_reactant("AB1") || !bio.is_reactant("AB2") || bio.is_reactant("AB3")) {
    error("is_reactant not working correctly");
  }
  if (!bio.is_gene("AB1") || !bio.is_gene("AB2")|| bio.is_gene("AB3")) {
    error("is_gene not working correctly");
  }
  vector<gene*> genes = bio.reacts_with("AB1");
  if (!(genes[0]->name != "AB1" || genes[0]->name != "AB2") || !(genes[1]->name != "AB1" || genes[1]->name != "AB2") || genes.size() != 2) {
    error("reacts_with not working correctly");
  }
  genes = bio.reacts_with("AB3");
  if (genes.size() != 0) {
    error("incorrectly calculates reacts_with");
  }
  genes = bio.get_genes("AB3");  
  if (!(genes[0]->name != "AB1" || genes[0]->name != "AB2") || !(genes[1]->name != "AB1" || genes[1]->name != "AB2") || genes.size() != 2) {
    error("get_genes not working correctly");
  }    
  interaction_data* data = bio.forms_product(genes);
  if (data == NULL || data->product != "AB3") {
    error("forms_product is not working correctly");
  }
}

void test_parts_handler(bool flag) {
  my_print("Testing parts_handler", flag);
  parts_handler parts(&bio, bio.promoters, bio.genes);
  if (!parts.is_promoter("pA1") || parts.is_promoter("foo")) {
    error("is_promoter not working correctly");
  }
  if (!parts.is_gene("AB1") || !parts.is_gene("AB2")|| parts.is_gene("AB3")) {
    error("is_gene not working correctly");
  }
  vector<gene*> inputs;
  inputs.push_back(parts.genes.get("AB1"));
  inputs.push_back(parts.genes.get("AB2"));
  hash_map<promoter*> promo_used;
  parts.promoters_used(inputs, promo_used);
  if (!promo_used.exists("pAB1") || !promo_used.exists("pAB2") || !promo_used.exists("pAB3")) {
    error("promoters_used incorrectly find interference due to reaction");
  }
  promo_used.clear();
  inputs.clear();
  inputs.push_back(parts.genes.get("AB1"));
  parts.promoters_used(inputs, promo_used);
  if (!promo_used.exists("pAB1") || !promo_used.exists("pAB2") || !promo_used.exists("pAB3")) {
    error("promoters_used incorrectly find interference due to reaction");
  }
  //TODO: is_independent doesn't work right now  
  //bool is_independent(promoter* promo, vector<gene*> inputs);  
  
  hash_map<gene*> genes_used;
  promo_used.clear();
  parts.parts_used(parts.promoters.get("pAB1"), inputs, promo_used, genes_used);
  if (!promo_used.exists("pAB1") || !promo_used.exists("pAB2") || !promo_used.exists("pAB3")) {
    error("parts_used incorrectly find interference due to reaction");
  }
  if (!genes_used.exists("AB1") || !genes_used.exists("AB2")) {
    error("parts_used not working correctly");
  }
  
  parts.use_gate(parts.promoters.get("pAB1"), inputs);
  if (parts.is_promoter("pAB1")) {
    error("use_gate not working correctly");
  }
  if (parts.is_gene("AB1") || parts.is_gene("AB2")) {
    error("use_gate not working correctly");
  }
  cout << "3!!!!!!!!!!!!!!!!!!!!" << endl;
  cout << parts << endl;
  cout << "3!!!!!!!!!!!!!!!!!!!!" << endl;
  
  vector<vector<gene*> > genes = parts.gene_inputs(ACT, &bio.promoters.get("pCA1"));
  if (genes.size() != 1 || genes[0].size() != 2 || !(genes[0][0]->name == "IA1" || genes[0][1]->name == "IX1")) {
    error("gene_inputs not working correctly");
  }
  cout << "2!!!!!!!!!!!!!!!!!!!!" << endl;
  cout << parts << endl;
  cout << "2!!!!!!!!!!!!!!!!!!!!" << endl;
  parts.remove_promoter(parts.promoters.get("pCA1"));
  cout << "1!!!!!!!!!!!!!!!!!!!!" << endl;
  cout << parts << endl;
  cout << "1!!!!!!!!!!!!!!!!!!!!" << endl;
  if (parts.promoters.exists("pCA1")) {
    error("remove_promoter not working correctly");
  }
  
  parts.remove_gene(parts.genes.get("IA1"));
  if (parts.genes.exists("IA1")) {
    error("remove_gene not working correctly");
  }
}
