/*******************SYMBOL TABLE************************/
#ifndef SYMTAB_H
#define SYMTAB_H

#include <iostream>
#include <map>
#include <list>
#include <stack>
#include "types.h"
#include <string>

const int TABSIZE = 1000;
const int LIBSIZE = 8;
const int STACK_DEPTH = 8;

class SymTab {
   
public:

  SymTab(); // Creates a new symbol table 
  ~SymTab();  // destroys symbol table 

  //(maps name -> (mode,TYPES field):
  typedef map<string,pair<string,TYPES> > basic_table;
  typedef list<basic_table*> working_table;

  bool new_design_unit(const string&);//Creates new basic_table for a unit.
  bool design_unit(const string&);    //Is there already a table for this unit?

  //  The following two commands must be used as a pair.
  //  Do not call any other SymTab methods between new_tb and delete_tb.
  void new_tb(void);//Creates a temporary table not associated with any unit.
  void delete_tb(void);  // Destroys temporary basic table created with new_tb

  // The following two functions work on the current basic table 
  // -- that most recently referenced by new_design_unit, new_tb, etc.

  //Associate a name with a type in the current basic table:
  bool insert(const string&, const pair<string,TYPES*>&);

  pair<string,TYPES*> lookup(const string&);//Look up a name in current table
  
  //void set_lib(const char *, const char *);
  //const char* get_lib_path(const char*);

  void push(void);  //Push into a new name scope.
  void pop(void);   //Return to previous name scope.

  //void print();

  //void set_scope(string s) { }//cur_scope = s; }
  basic_table* find(const string&);//Find entity, return pointer to its table
  
private:

  /*list< pair<pair<string, string>, map<string, TYPES*>* > > *scope;
  map<string, TYPES*> *preDefined, *cur_table;
  stack< list< pair<pair<string, string>, map<string, TYPES*>* > > * > stk;
  string cur_scope;  
  list< pair<string, string> > lib;*/

  // entity_id::entity_sym_table
  map<string,basic_table*> open_entity_list;//Maps entity name onto its table.
  working_table* cur_design_unit; 
  basic_table *cur_table;
  stack<working_table*> sym_stack;  // Stack of scopes.
};
#endif
