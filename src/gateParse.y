%{
/*  
    This file generates a grammar for the reading of
    gate building data.
    
    Author: Nam Nguyen
    School: University of Utah
    Date:   1/12/07
*/
#include <stdio.h>
#include "gateConstruction.c"
#include "biostructure.h"
#include <map>
#include <iostream>

using namespace std;    
   
int yyerror(char*);
void parseGateFile(hash_map<gate_data> *gate_builds);
void loadGateFile(char*);
void copyName(char*);
string stripQuote(char* to_strip);

char current_name[MAX_LENGTH];
int num_gates = 0;
hash_map<gate_data>* all_gates;
vector<vector<int> > builds;

//Needed for Bison 2.2 
#ifdef OSX
//extern char yytext[];
#endif 
%}

%token COMMA GATE LATCH LEFT_BRACE LEFT_CURLY NUMBER RIGHT_BRACE RIGHT_CURLY TEXT UNKNOWN_TOKEN;



/*
 * Grammar Rules
 * For the construct rules, each number represents:
 *  #act, #rep, #compAct, #compRep, #actAct, #repRep, #actRep
 
 */
%%
line:  	line one_gate | line one_latch | ;
one_gate: GATE TEXT {copyName(yytext); } LEFT_CURLY {builds.clear();} gates RIGHT_CURLY {	
	string temp = stripQuote(current_name);
	gate_data new_gate = gate_data(temp.c_str(), builds); 	
	(*all_gates).put(temp, new_gate);
	num_gates++; 
}; 
one_latch: LATCH TEXT {copyName(yytext); } LEFT_CURLY {builds.clear();} gates RIGHT_CURLY {
	string temp = stripQuote(current_name);
	gate_data new_gate = gate_data(temp.c_str(), builds); 
	(*all_gates).put(temp, new_gate);
	num_gates++; 
};  
gates: construct | construct gates;
construct: LEFT_BRACE NUMBER COMMA NUMBER COMMA NUMBER COMMA NUMBER COMMA NUMBER COMMA NUMBER COMMA NUMBER RIGHT_BRACE {
	{
		vector<int>gate_build;
		gate_build.push_back($2);  gate_build.push_back($4);  gate_build.push_back($6);  gate_build.push_back($8);  gate_build.push_back($10);
		gate_build.push_back($12);  gate_build.push_back($14);
		builds.push_back(gate_build);
	}
};
%%

extern FILE *yyin;

//Keep a copy of the name around for later use.
void copyName(char* name) {
	strcpy(current_name, name);	
}

string stripQuote(char* to_strip) {
	string temp = string(to_strip);
	unsigned int size = temp.length();
	return temp.substr(1, size-2);
}

void loadGateFile(char* filename) {
	yyin = fopen(filename, "r");	
}

void parseGateFile(hash_map<gate_data> *gate_builds) {
	all_gates = gate_builds;		
	do {
		yyparse();
	}
	while (!feof(yyin));
	printf("\n");
}

int yyerror(char* s)
{
	fprintf(stderr, "%s\n",s);
	return -1;
}
