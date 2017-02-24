/*********************************SYMTAB.C*********************/
#ifndef SYMTAB_C
#define SYMTAB_C

#include <cassert>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include "symtab.h"
#include "misclib.h"
#include "auxilary.h"
#include <string>

extern int     linenumber;
extern char    *file_scope1;

char *ieee_path;

SymTab::SymTab()
{
  cur_design_unit = new working_table;
  cur_table = 0;
}

SymTab::~SymTab()
{
  for(map<string,basic_table*>::iterator b = open_entity_list.begin(); 
      b != open_entity_list.end(); b++)
    delete b->second;
  
  delete cur_design_unit;
}

bool SymTab::new_design_unit(const string& id)
{
  string SS = Tolower(id);
  if(open_entity_list.find(SS) != open_entity_list.end())
    return false; // multiple declaration of the same entity.
  
  cur_table = new basic_table;
  open_entity_list[SS] = cur_table;
  cur_design_unit->push_back(cur_table);
  return true;
}


bool SymTab::design_unit(const string& id)
{
  string SS = Tolower(id);
  if(open_entity_list.find(SS) == open_entity_list.end())
    return false; // no declaration of the given entity.

  delete cur_design_unit;
  cur_design_unit = new working_table;
  cur_design_unit->push_back(open_entity_list[SS]);
  return true;
}
  
void SymTab::new_tb(void)
{
  cur_table = new basic_table;
  cur_design_unit->push_back(cur_table);
}


void SymTab::delete_tb(void)
{
  basic_table* junk = cur_design_unit->back();
  cur_design_unit->pop_back();
  delete junk;  
}
  

bool SymTab::insert(const string& id, const pair<string,TYPES*>& tt)
{
  string SS = Tolower(id);
  if(find(SS) == 0){
#ifdef DDEBUG
    assert(cur_table);
#endif
    if(tt.second)
      cur_table->insert(make_pair(SS, make_pair(tt.first, *(tt.second))));
    else
      cur_table->insert(make_pair(SS, make_pair(tt.first, TYPES())));
    return true;
  }
  return false;
}

pair<string,TYPES*> SymTab::lookup(const string& id)
{
  string SS = Tolower(id);
  basic_table* TT = find(SS);
  if(TT == 0)
    return make_pair((string)"err",(TYPES*)0);
  else
    return make_pair((*TT)[SS].first, &((*TT)[SS].second));
}

SymTab::basic_table* SymTab::find(const string& id)
{
  string SS = Tolower(id);
  for(working_table::reverse_iterator b = cur_design_unit->rbegin(); 
      b != cur_design_unit->rend(); b++)
    if((*b)->find(SS) != (*b)->end())
      return *b;
  
  return 0;
}

/*
void SymTab::print()
{
  linkedList *l = table;

  while(l)
    {
      //      for(int i = 0; i< TABSIZE; i++)
      //if(l->name[0])
      cout<< ">>  "<<l->name[0] <<"  <<"<< endl;
      
      
      l = l->backward;cout<< l<<" -------\n";
      }}*/


/*
void SymTab::set_lib(const char *s1, const char *s2)
{
  //  lib.push_back(make_pair(s1, s2));
}


const char* SymTab::get_lib_path(const char *s)
{
  list< pair<string, string> >::iterator begin, end;
  begin = lib.begin();
  end = lib.end();
  while(begin != end)
    {
      if((*begin).first == s)
	return (*begin).second.c_str();

      begin++;
    }

    return 0;
}
*/

void SymTab::push(void)
{
  sym_stack.push(cur_design_unit); 
  cur_design_unit = new working_table;
}



void SymTab::pop(void)
{
  /*  list< pair<pair<string, string>, map<string, TYPES*>* > >::iterator 
    begin, end;
  begin = scope->begin();
  end = scope->end();
  while(begin != end)
    {
      map<string, TYPES*>::iterator begin_1, end_1;
      begin_1 = begin->second->begin();
      end_1 = begin->second->end();
      while(begin_1 != end_1)
	{
	  delete begin_1->second;
	  begin_1++;
	}
      begin++;
    }
    delete scope;*/
  
  delete cur_design_unit;
  cur_design_unit = sym_stack.top();
  sym_stack.pop(); 
  cur_table = cur_design_unit->back();
}
#endif


