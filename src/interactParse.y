%{
/*  
    This file generates a grammar for the reading of
    gate building data.
    
    Author: Nam Nguyen
    School: University of Utah
    Date:   1/12/07
*/
#include <stdio.h>
#include "interaction.c"
#include "biostructure.h"
#include <iostream>
//#include "loadg.h"

using namespace std;

int yyerror(char*);
string gene_name;
string protein_name;
string promoter_name;
vector<string> activated;
vector<string> repressed;
vector<string> combos;


hash_map<promoter> promoters;
hash_map<interaction_data> interactions;
hash_map<gene> genes;

string stripQuote(string to_strip);

#ifdef OSX
//  extern char yytext[]; 
#endif

%}

%token COMMA GENE PROMOTER INTERACTION LEFT_BRACE LEFT_CURLY NUMBER RIGHT_BRACE RIGHT_CURLY TEXT UNKNOWN_TOKEN;



/*
 * Grammar Rules
 * For the construct rules, each number represents:
 *  #act, #rep, #compAct, #compRep, #actAct, #repRep, #actRep
 
 */
%%
line:  	line promoter | line gene | line interaction | ;
gene: 	GENE TEXT {gene_name = stripQuote(string(yytext));
		gene temp = gene(gene_name);
		genes.put(temp.name, gene_name);
	}; 
promoter: PROMOTER TEXT {promoter_name = stripQuote(string(yytext));} 
	LEFT_CURLY {activated.clear();} activate RIGHT_CURLY	
	LEFT_CURLY {repressed.clear();} repressed RIGHT_CURLY {	
		set<string, cmpstr> activate;		
		for (unsigned int i = 0; i < activated.size(); i++) {
			activate.insert(activated[i]);
		}
		set<string, cmpstr> repress;				
		for (unsigned int i = 0; i < repressed.size(); i++) {
			repress.insert(repressed[i]);
		}
		promoter temp = promoter(promoter_name, activate, repress);
		promoters.put(temp.name, temp);
	};
activate: activate TEXT {activated.push_back(stripQuote(string(yytext)));} |;	  
repressed: repressed TEXT {repressed.push_back(stripQuote(string(yytext)));} |;
interaction: INTERACTION LEFT_CURLY {activated.clear();} activate RIGHT_CURLY	
	LEFT_CURLY TEXT {protein_name = stripQuote(string(yytext));} RIGHT_CURLY{
		set<string, cmpstr> activate;		
		for (unsigned int i = 0; i < activated.size(); i++) {
			activate.insert(activated[i]);
		}
		interaction_data temp = interaction_data(activate, protein_name);
		interactions.put(temp.product, temp);
	};
%%

extern FILE *yyin;

void loadInteractFile(char* filename) {
	yyin = fopen(filename, "r");	
}

void parseInteractFile(hash_map<promoter> *promoters_set, hash_map<interaction_data> *interactions_set, hash_map<gene> *genes_set) {
	do {
		yyparse();
	}
	while (!feof(yyin));
	*interactions_set = interactions;
	*promoters_set = promoters;
	*genes_set = genes;
}

int yyerror(char* s)
{
	fprintf(stderr, "%s\n",s);
	return -1;
}

string stripQuote(string temp) {
	unsigned int size = temp.length();
	return temp.substr(1, size-2);
}
