#ifndef AUX_H
#define AUX_H

#include <iostream>
#include <cstring>
#include "types.h"
#include "symtab.h"
#include "tersgen.h"
#include "actions.h"
#include "events.h"
#include "rules.h"
#include "conflicts.h"
#include <map>
#include <string>


const int MAX_FILE_NEST_DEPTH= 10;

const long UNDEF_I= 2147483647;

const double UNDEF_F= 1.0e38; 

string itos(int);
string itos(char);

void err_msg(const string&);
void err_msg(const string&, const string&);
void err_msg(const string&, const string&, const string&);
void err_msg(const string&, const string&, const string&, const string&);
void err_msg(const string&, const string&, const string&, 
	     const string&, const string&);

void warn_msg(const string&);

string my_not(const string&);
string logic(const list<string>&, const string& op = "&");
string logic(const string&, const string&, const string& op = "&",
	     bool para = true);
string bool_relation(const string& S1, const string& S2, const string& op = "=");

TYPES *make_relation(SymTab*, TYPES*, TYPES*, bool&);

void extend(char *);

char *link(char *,char *);

int check_type( TYPES*, TYPES *);

TYPES *check_relation( TYPES*, TYPES*, char *);

TYPES *check_logic( TYPES*, TYPES*, char *);

char *addsuffix( char*, char* );

int havesuffix(char*s);

char *rmsuffix(char*);

char *strhcpy (char*, const char*);

char *strhcat (char*, const char*);

char *Tolower(const char*);
string Tolower(const string&);

char *copy_string(const char*);
char *copy_str(const char*);

char *strip_char(char *s);

bool switch_buffer(const string&);
bool switch_buffer(const char*);

// Compare two strings without case difference.
bool strcmp_nocase(const string& s1, const string& s2); 

telADT telcompose(const ty_tel_lst&, const string& op = ";");

class my_list{

public:

  my_list() {}
  my_list(const map<string, string>&);
  virtual ~my_list() {}

  void add(char*s1, char *s2);
  string match(const string&, const string&);

  friend ostream &operator<<(ostream&, const my_list&);
  operator const map<string, string>&() { return netlist; }
  const map<string,string> & port_list() { return netlist; }


protected:

  map<string, string> netlist; 
};

bool rm_red_top(telADT S, bool verbose, int state=0, bool TC=false);
bool rm_red(telADT, bool verbose, int state=0, bool TC=false);
void rm_red1(telADT);
bool rm_red2(telADT S);
void update_io(TERstructADT target, const list<pair<string,int> >& new_io);

//***************************************************************************
// Return a new TEL structure which is a copy of S with all action names 
// replaced by the names in the list l.
//***************************************************************************

TERstructADT instantiate(TERstructADT S, my_list *l, 
			 char *scope,
			 char *component_name, bool csp);
assignList CopyRenameList(assignList head, char* scope);

//***************************************************************************
// Return a new action set  which is a copy of A with all labels 
// replaced by the names in the list l.
//***************************************************************************

actionsetADT instantiate_action_set(actionsetADT A, 
				    my_list *l, 
				    char *scope,
				    char *component_name, bool csp);

//***************************************************************************
// Return a new event set which is a copy of E with all labels 
// replaced by the names in the list l.
//***************************************************************************

eventsetADT instantiate_event_set(eventsetADT E, my_list *l, 
				  char *scope,
				  char *component_name, bool csp);

//***************************************************************************
// Return a new rule set which is a copy of E with all labels 
// replaced by the names in the list l.
//***************************************************************************

rulesetADT instantiate_rule_set(rulesetADT R, my_list *l, 
				char *scope,
				char *component_name, bool csp);

//***************************************************************************
// Return a new conflict set which is a copy of E with all labels 
// replaced by the names in the list l.
//***************************************************************************

conflictsetADT instantiate_conflict_set(conflictsetADT C, 
					my_list *l,
					char *scope,
					char *component_name, bool csp);
					

//***************************************************************************
// Find the new label in the list 'l' corresponding to the 'label' 
//***************************************************************************

char *new_label(const char *label, my_list *l, 
		const char *scope, bool csp);

//***************************************************************************
// Replace identifiers in the 'expr' by the corresponding ones in the list 'l'
//***************************************************************************

char *new_expression(const char *expr, my_list *l, const char *scope,bool csp);


char *array_relation(SymTab *table, TYPES *t1, TYPES *t3, bool &error);

pair<string,string> split(const string&, char C = '.', bool B = false);

char *Tolower(const char *s);

#endif

