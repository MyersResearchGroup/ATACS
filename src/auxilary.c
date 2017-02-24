#include <algorithm>
#include <cassert>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include "auxilary.h"
#include "hse.hpp"

//From compile.c
extern int linenumber;
extern char *file_scope1;
extern bool compiled;

//From postproc.cc
extern int cnt;

string new_label_string(const char *label, my_list *l, const char *scope, bool csp);

string itos(char c)
{
  stringstream tmp_str;
  tmp_str<<c;//<< ends;
  return tmp_str.str();
}

string itos(int i)
{
  stringstream tmp_str;
  tmp_str<<i;//<< ends;
  return tmp_str.str();
}

void warn_msg(const string& s)
{
  cout<< file_scope1 << ": line " << linenumber
      << ": " << s << endl;
}

void err_msg(const string& s)
{
  cout<< file_scope1 << ": line " << linenumber
      << ": " << s << endl;
  compiled=FALSE;
}

void err_msg(const string& s1, const string& s2)
{
  cout<< file_scope1 << ": line " << linenumber
      << ": " << s1 << s2 << endl;
  compiled=FALSE;
}


void err_msg(const string& s1, const string& s2, const string& s3)
{
  cout<< file_scope1 << ": line " << linenumber << ": " 
      << s1 << s2 << s3 << endl; 
  compiled=FALSE;
}


void err_msg(const string& s1, const string& s2, 
	     const string& s3, const string& s4)
{
  cout<< file_scope1 << ": line " << linenumber << ": " 
      << s1 << s2 << s3 << s4 << endl; 
  compiled=FALSE;
}

void err_msg(const string& s1, const string& s2, const string& s3, 
	     const string& s4, const string& s5)
{
  cout<< file_scope1 << ": line " << linenumber << ": " 
      << s1 << s2 << s3 << s4 << s5 << endl; 
  compiled=FALSE;
}


char *strip_char(char *s)
{
  if(s)
    {
      char *temp = strtok(s, "[(");
      char *a_copy = copy_string(temp); 
      char *result = strtok(a_copy, ")]");

      if(result)
	return result;
      else
	return "\0";
    }

  return "\0";
}


bool switch_buffer(const string& SS)
{       
  if(SS != "error")
    return switch_buffer(SS.c_str());
  return false;
}
 
        
string my_not(const string& SS)
{
  if(SS == "'0'" || SS == "false") return "true";
  else if(SS == "'1'" || SS == "true") return "false";
  else if(SS[0] == '~')
    return string(SS.begin()+1, SS.end());
  else
    return "~(" + SS + ')';
}

string logic(const list<string>& LL, const string& op)
{
  if(LL.size() == 1)
    return LL.front();

  string ret_s;
  for(list<string>::const_iterator b = LL.begin(); b != LL.end(); b++){
    if(ret_s.size() == 0)
      ret_s = *b;
    else
      ret_s = logic(ret_s, *b, op, false);
  }
  return ret_s;    
}

string logic(const string& s1, const string& s2, const string& op,
	     bool para)
{ 
  if(s1 == s2) return s1;
 
  if(op!= "&" && op != "|"){
    cout << "error: undefined operator '" << op << "'\n";
    return "'1'";
  }  
  
  if(op == "&"){
    if ((s1 == "maybe")||(s1 == "~maybe")||(s1 == ('(' + "maybe" + ')'))||
	(s1 == ('(' + "~maybe" + ')'))||(s2 == "maybe")||(s2 == "~maybe")||
	(s2 == ('(' + "maybe" + ')'))||((s2 == ('(' + "~maybe" + ')'))))
      return "maybe";
    else if(s1 ==  "'0'" || s1 == "false" || s2 ==  "'0'" || s2 == "false" ||
       s1 == my_not(s2) || s1 == ('('+my_not(s2)+')'))
      return "false";
    else if(s1 ==  "'1'" || s1 == "true" || s1 == "[true]")
      return s2;
    else if(s2 ==  "'1'" || s2 == "true" || s2 == "[true]")
      return s1;
  }
  
  if(op == "|"){
    if(s1 ==  "'1'" || s1 == "true" || s2 ==  "'1'" || s2 == "true" ||
       s1 == my_not(s2) || s1 == ('('+my_not(s2)+')'))
      return "true";
    else if(s1 ==  "'0'" || s1 == "false")
      return s2;
    else if(s2 ==  "'0'" || s2 == "false")
      return s1;
  }

  if(para == true)
    return '('+s1+')' + op + '('+s2+')';
  return s1 + op + s2;
}

string bool_relation(const string& S1, const string& S2, const string& op)
{
  string S1_c = my_not(S1);  
  string S2_c = my_not(S2);
    
  if(op == "=")
    if(S1 == S2) return "'1'"; 
    else return logic(logic(S1,S2), logic(S1_c,S2_c), "|");
  if(op == "/=")
    if(S1 == S2) return "'0'";
    else return logic(logic(S1,S2_c), logic(S1_c,S2), "|");
  if(op == ">")
    if(S1 == S2) return "'0'";
    else return logic(S1, S2_c, "&");
  if(op == "<")
    if(S1 == S2) return "'0'";
    else return logic(S1_c, S2, "&");
  return string();
}

TYPES *make_relation(SymTab *table, TYPES *t1, TYPES *t3, bool &error)
{
  /*  TYPES *t = 0;
  TYPES *ret_val = 0;

  if(strcmp(t1->data_type(), "err") &&
     strcmp(t3->data_type(), "err"))
    { 
      if(t1->get_grp() == TYPES::Array || t3->get_grp() == TYPES::Array)
	{
	  ret_val = new TYPES("exp", "bool");
	  ret_val->set_string(array_relation(table, t1, t3, error));
	  return ret_val;
	}
      else if(strcmp(t1->data_type(), "uk"))
	{
	  if(strcmp(t1->data_type(), t3->data_type()) == 0)
	    {
	      ret_val = new TYPES("exp", "bool");
	      ret_val->set_string(logic_cond(t1->get_string(),
					     t3->get_string()));
	      return ret_val;
	    }	

	  t = table->lookup(t1->data_type());
	  if(t == 0)
	    {
	      t = new TYPES("err", "err");
	      t->set_string("false");
	      error = true;
	      return t;
	    }
	  if(strcmp(t3->data_type(), "uk") == 0 &&
	     t->find(t3->get_string()))
	    {
	      ret_val = new TYPES("exp", "bool");	
	      ret_val->set_string(logic_cond(t1->get_string(),
					     t3->get_string()));
	    }	
	  else if((strcmp(t->data_type(), "int") == 0 ||
		   strcmp(t->data_type(), "integer") == 0) &&
		  (strcmp(t3->data_type(), "int") == 0 ||
		   strcmp(t3->data_type(), "integer") == 0))
	    {
	      ret_val = new TYPES("exp", "bool");	
	      ret_val->set_string("true");
	    }
	  else
	    { 
	      t = new TYPES("err", "err");
	      t->set_string("false");
	      error = true;
	      return t;
	    }
	}
      else if(strcmp(t3->data_type(), "uk"))
	{
	  t = table->lookup(t3->data_type());
	  if(t == 0)
	    {
	      t = new TYPES("err", "err");
	      t->set_string("false");
	      error = true;
	      return t;
	    }
	  if(strcmp(t1->data_type(), "uk") == 0 &&
	     t->find(t1->get_string()))
	    {
	      ret_val = new TYPES("exp", "bool");	
	      ret_val->set_string(logic_cond(t1->get_string(),
					t3->get_string()));
	    }	
	}
      else if(strcmp(t1->data_type(), t3->data_type()))
	{
	  ret_val = new TYPES("err", "err");
	  ret_val->set_string("false");
	  error = true;
	}
    }
  else
    {
      ret_val = new TYPES("err", "err");
      ret_val->set_string("false");
    }  

  if(ret_val == 0) 
    {
      ret_val = new TYPES("err", "err");
      ret_val->set_string("false");
    }
  */  
  return 0;
}
  

int check_type( TYPES *t1, TYPES *t2)
{
  /*if(strcmp(t1->data_type(), t2->data_type()) == 0 &&
     strcmp(t1->data_type(), "err"))
    return 0; 
  */
  return 1;

}



TYPES *check_relation( TYPES *t1, TYPES *t2, char *str)
{   /*
  if( strcmp(t1->datatype(), t2->datatype())
      && !(t1->isError() || t2->isError()) 
      && !(t1->isUnknown() || t2->isUnknown())  ) {
    err_msg(" different types on left and right side of",str,"operator"); 
    return new TYPES( "ERROR", "ERROR", "ERROR" ); 
  }
  else 
    return new TYPES( "unknown", "BOOLEAN", "BOOLEAN" ); */
  return NULL;
}
         



TYPES *check_logic(TYPES *t1, TYPES *t2, char *op)
{ /*
  if( strcmp(t1->datatype(), t2->datatype())
      && strcmp(t1->subtype(), t2->subtype()) 
      && !(t1->isError() || t2->isError()) ) {
    err_msg("different types on left and right side of",op,"operator"); 
    return new TYPES("ERROR", "ERROR", "ERROR");
  }
  else 
    return t1; */
  return NULL;
}



char *addsuffix(char *name, char *suffix)
{
  char *str;
  if(suffix)
    {
      str = new char[strlen(name) + strlen(suffix)+ 2];
      sprintf(str, "%s@%s", name, suffix);
    }
  else
    {
      str = copy_string(name);
    }

  return str;
}


char *rmsuffix(char *s)
{
  char *temp = copy_string(s);
  char *ret_val = strtok(temp, "@");

  return ret_val;
}


// Compare two strings without case difference.
bool strcmp_nocase(const string& s1, const string& s2)
{
  string::const_iterator p1 = s1.begin();
  string::const_iterator p2 = s2.begin();
  
  while(p1 != s1.end() && p2 != s2.end()){
    if(toupper(*p1) != toupper(*p2))
      return false;
    p1++; p2++;
  }
  return !(s1.size() - s2.size());
}
	  
telADT telcompose(const ty_tel_lst& LL, const string& op)
{
  telADT ret_val = 0;
  if(&LL)
    for(ty_tel_lst::const_iterator BB = LL.begin(); 
	BB != LL.end(); BB++){
      if(ret_val){
	char *Cstr=copy_str(op.c_str());
	ret_val=TERScompose(ret_val,TERSrename(ret_val,BB->second),Cstr);
	delete[] Cstr;
      }
      else
	ret_val = BB->second;
    }
  return ret_val;
}


char *strhcpy (char *s1, const char *s2)
{
  assert(s2);
  s1 = new char[strlen(s2)+1];
  assert(s1);
  strcpy(s1, s2);

  return s1;
}


char *strhcat (char *s1, const char *s2)
{
  assert(s1 || s2);
  char *t = new char[strlen(s1)+strlen(s2)+1];
  assert(t);
  sprintf(t, "%s%s", s1, s2);

  return t;
}



char *Tolower(const char *s)
{
#ifdef DDEBUG
  assert(s);
#endif
  char *t = new char[strlen(s)+1];
  strcpy(t,s);
  if(!t) 
    cout<<"WARNING: running out of memory\n";
  for (int i=0;i<(signed)strlen(s);i++)
    t[i]=tolower(s[i]);
  return t;
}

string Tolower(const string& SS)
{
  string ret_s = SS;
  for(string::iterator p1 = ret_s.begin(); p1 != ret_s.end(); p1++)
    *p1 = tolower(*p1);
  if(ret_s.size() != SS.size()) cout << "error:  " << SS << "  " << ret_s <<endl;
  return ret_s;
}


/*****************************************************/
// Definition of class list
/*****************************************************/
my_list::my_list(const map<string, string>& l):netlist(l)
{
  //for(map<string,string>::const_iterator b = l.begin();
  //  b != l.end(); b++)
  //cout << b->first << "   " << b->second << endl;
}

void my_list::add(char *s1, char *s2)
{ 
  //actual[count] = copy_string(s1);
  //formal[count] = copy_string(s2);

  // s2 -> formal name, s1 ->actual name.
  netlist[s2] = s1;
}


string my_list::match(const string& s, const string& scope)
{
  //string LL = Tolower(s); 
  if(netlist.find(s) != netlist.end()){
    return netlist[s];
  }
  if(s != "true" && s != "false" && s != "reset" && !isdigit(s[0])){
    if(scope.empty() || s == "bit"){
      if (s == "bit"){
      	return "BIT";
      }
      else {
      	return s;
      }
    }
    else{
      return (s + '_' + scope);
    }
  }
  return s;
}



ostream &operator<<(ostream &os, const my_list &l)
{
  for(map<string,string>::const_iterator b = l.netlist.begin(); 
      b != l.netlist.end(); b++)
    os<<b->first<<"   " << b->second << endl;
  return os;
}

//*************************************************************************

char *copy_str(const char *s)
{ return copy_string(s); }

char *copy_string(const char *s)
{
  if(s) {
    char *t = new char[strlen(s)+1];
    strcpy(t, s);
    return t;
  }
  else
    return 0;
}


//****************************************************************************
//Remove the redundant rules from a TEL structure. A redundant rule is defined
//as follows:
//  1. there is a rule (u, e, (l,u));
//  2. there is another rule (w, e, (l, u));
//  3. if u can reach w through a sequence of rules, and u is not in conflict
//     with any other events.
//
//In this implementation, Rvec is the transitive closure of all the events in 
//the TEL structure. A map of map is used as a two-D array for Rvec, in which
//each entry [e][f] stores two bool veriables, the first one indicating if 
//e is reachable to f, and the second one indicating between e and f if there
//is a initial rule.
//
//Rlst stores a list of rules which have the same enabled event. The bool 
//variable indicates wther the rule is from R or Rp.
//
//Part 2: find the redundant rule for each event as the enebled event.
//Part 3: remove the redundant rules from R and Rp.
//Problem: this implement has a more than necessary restrictive conflict check.
//****************************************************************************

bool check_redundant(telADT S, set<eventsADT> ED_SET, eventsADT W,
		     rulesADT RULE, int MODE)
{
  for(set<eventsADT>::iterator iter = ED_SET.begin(); iter != ED_SET.end();
      iter++)
    if(MODE == 1){
      if(!event_cmp(RULE->enabled, *iter) && 
	 check_C(S->C, RULE->enabled, *iter) &&
	 !check_C(S->C, *iter, W)){
	return  false;
	//S->C = add_conflict(S->C, *iter, W);
	//return true;
      }
    }
    else if(MODE==2){
      if(!event_cmp(RULE->enabled, *iter) && 
	 check_C(S->C, RULE->enabled, *iter))
	return  false;
    }
      
  
  return true;
}


bool check_redundant(telADT S, set<pair<eventsADT,eventsADT> > C_SET,
		     eventsADT E1, eventsADT E2)
{
  bool c1=false, c2=false;
  for(eventsetADT cur_event=S->O; cur_event; cur_event=cur_event->link){
    if(C_SET.find(make_pair(cur_event->event, E1)) != C_SET.end() ||
       C_SET.find(make_pair(E1, cur_event->event)) != C_SET.end())
      c1=true;
    if(C_SET.find(make_pair(cur_event->event, E2)) != C_SET.end() ||
       C_SET.find(make_pair(E2, cur_event->event)) != C_SET.end())
      c2=true;
  }	
  if(c1 && c2)
    return false;

  return true;
}



bool rm_red_top(telADT S, bool verbose, int state, bool TC)
{
  bool modified = false;

  modified = rm_red(S, state, TC, verbose);

  while(modified)
    modified = rm_red(S, state, TC, verbose);
  
  return modified;
}


// Check if 'R' is redudant in the initial marking.
bool initial_redundant(telADT S, list<pair<rulesADT,bool> > R_Set,
		       rulesADT R)
{
  // true_set stores rules which make 'R' redundant, while false_set stores
  // those rules which make 'R' not redundant.
  set<eventsADT> true_set, false_set;

  for(list<pair<rulesADT,bool> >::iterator iter=R_Set.begin();
      iter!=R_Set.end(); iter++){
    if(iter->first==R)
      continue;

    if((iter->first->lower >= R->lower) && (iter->first->upper >= R->upper)){
      //cout<< "2 "<<iter->first->enabling->action->label<< endl;
      true_set.insert(iter->first->enabling);
    }
    else{
      false_set.insert(iter->first->enabling);
      //cout<< "1 "<<iter->first->enabling->action->label<< endl;
    }
  }

  // if no rule makes 'R' redudant.
  if(!true_set.size())
    return false;

  // if some rules make 'R' redudant and 'S' contains no choices.
  if(!S->C)
    return true;

  // If the enabling events of two rules are in conflict, they both must be in
  // the true_set to make 'R' redudant.
  for(set<eventsADT>::iterator iter=true_set.begin(); iter!=true_set.end();
      iter++){
    bool redudant = true;
    for(set<eventsADT>::iterator iter_1=false_set.begin();
	iter_1!=false_set.end(); iter_1++)
      if(check_C(S->C, *iter, *iter_1)){
	redudant = false;
	break;
      }

    if(redudant)
      return true;
  }

  return false;
}

//TC: indicate weather calculate the transitve closure among all events in 'S'.
bool rm_red(telADT S, bool verbose, int state, bool TC)
{
  bool modified = false;
  
  typedef map<eventsADT,map<eventsADT,pair<bool,bool> > > str2_b2_m;
  typedef map<eventsADT,list<pair<rulesADT,bool> > > erb2l_m;
  typedef map<eventsADT,set<eventsADT> > eventset;
  str2_b2_m Rvec;
  erb2l_m Rlst, r_enabling;
  eventset Envec, ed_set;
  
  for(rulesetADT cur_rule = S->R; cur_rule; cur_rule = cur_rule->link){
    if ((cur_rule->rule->enabling->action->label != string("reset")) &&
	(!(cur_rule->rule->type & ORDERING))) {
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;
      Rvec[EN][ED] = make_pair(true, false);
      Rlst[ED].push_back(make_pair(cur_rule->rule, false));
      r_enabling[EN].push_back(make_pair(cur_rule->rule, false));
      Envec[ED].insert(EN);
      ed_set[EN].insert(ED);
    }
  }
  /*
  for(rulesetADT cur_rule = S->Rp; cur_rule; cur_rule = cur_rule->link){
    if (!(cur_rule->rule->type & ORDERING)) {
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;
      Rvec[EN][ED] = make_pair(true, true);
      Rlst[ED].push_back(make_pair(cur_rule->rule, true));
      r_enabling[EN].push_back(make_pair(cur_rule->rule, true));
      Envec[ED].insert(EN);
      ed_set[EN].insert(ED);
    }
  }
  */
  set<pair<eventsADT,eventsADT> > C_set;

  for(conflictsetADT iter = S->C; iter; iter = iter->link){
    C_set.insert(make_pair(iter->conflict->left, iter->conflict->right));
    C_set.insert(make_pair(iter->conflict->right, iter->conflict->left));
  }
  
  // Compute the transitive closure among events upon 'TC'.
  if(TC){
    eventsetADT E = link_event(S->I, S->O);
    for(eventsetADT C = E; C; C = C->link){
      eventsADT CE = C->event;
      for(eventsetADT I = E; I; I = I->link){
	eventsADT IE = I->event;
	for(eventsetADT II = E; II; II = II->link){
	  eventsADT IIE = II->event;
	  if(Rvec[IE][CE].first && Rvec[CE][IIE].first)
	    if((!(Rvec[IE][IIE].second)) && 
	       (Rvec[IE][CE].second == true || Rvec[CE][IIE].second == true)){
	      if (Rvec[IE][CE].second==false || Rvec[CE][IIE].second==false)
		Rvec[IE][IIE] = make_pair(true, true);
	    } else
	      Rvec[IE][IIE] = make_pair(true, false);
	}
      }
    }
  }

  // Part 2.
  
  //  conflictsetADT Cset = link_conflict(S->C, S->Cp);
  set<pair<rulesADT,bool> > junk_R;

  //remove the redundancies depending on the following conditions  
  // I->first: enabled event; 
  // I->second: a list of rules enabling I->first;
  // For each event, U and W are two enabling events. If W is reachable from 
  // U, and the rule containing U has the larger delay than that W, then
  // RII->first is redundant relative to RI->first. 

  for(erb2l_m::iterator I = Rlst.begin(); I != Rlst.end(); I++){
    // if there is only one rule enabling 'I->first', stop checking.
    if(I->second.size() <= 1) 
      continue;
    
    eventsADT V = I->first;

    list<pair<rulesADT,bool> >::iterator RI, RII;
    map<eventsADT,list<pair<rulesADT,bool> > > pr_rules;
    //    bool modified = false;

    for(RII = I->second.begin(); RII != I->second.end(); RII++) {
      eventsADT U = RII->first->enabling;

      for(RI = I->second.begin(); RI != I->second.end(); RI++) {
	eventsADT W = RI->first->enabling;
	
	if(C_set.find(make_pair(W, V)) != C_set.end() ||
	   C_set.find(make_pair(V, W)) != C_set.end()||
	   C_set.find(make_pair(U, W)) != C_set.end() ||
	   C_set.find(make_pair(W, U)) != C_set.end())
	  continue;						    
	
       	
	if((U!=W)){// && !check_C(S->C, U, W)){
	  
	  rulesADT U_2_W = check_rule(S->R, U, W);
	  bool U_2_W_initial= false;
	  if(!U_2_W){
	    U_2_W = check_rule(S->Rp, U, W);
	    U_2_W_initial = true;
	  }

	  long RI_U_W_lower=0;
	  long RI_U_W_upper=0;
	  if(U_2_W){
	    RI_U_W_lower = RI->first->lower + U_2_W->lower;;
	    if(RI->first->upper==INFIN || U_2_W->upper==INFIN)
	      RI_U_W_upper = INFIN;
	    else
	      RI_U_W_upper = RI->first->upper + U_2_W->upper;
	  }
	  
	  bool C_in_en_b = false;

	  for(set<eventsADT>::iterator iter = Envec[W].begin();
	      iter != Envec[W].end(); iter++)
	    if(U_2_W && !event_cmp(U, *iter) && //check_C(S->C, U, *iter))
	       C_set.find(make_pair(U, *iter)) != C_set.end())
	      C_in_en_b = true;
	  
	  if(!C_in_en_b && U_2_W && !RI->second &&
	     ((!U_2_W_initial && !RII->second) ||
	      (U_2_W_initial && RII->second))){
	    
	    if(RI_U_W_lower >= RII->first->lower &&
	       RI_U_W_upper >= RII->first->upper &&
	       check_redundant(S, ed_set[RII->first->enabling], W,
			       RII->first, 1) &&
	       check_redundant(S, C_set, RII->first->enabling, 
			       RII->first->enabled)){
	      junk_R.insert(make_pair(RII->first, RII->second));
	    }
	    else if(RI->first->lower >= RII->first->lower &&
		    RI->first->upper >= RII->first->upper &&
		    check_redundant(S, ed_set[RII->first->enabling], W,
				    RII->first, 1) &&
		    check_redundant(S, C_set, RII->first->enabling, 
				    RII->first->enabled)){
	      junk_R.insert(make_pair(RII->first, RII->second));
	    }
	    else if(Envec[W].size()==1 && 
		    (RII->first->lower >= U_2_W->upper + RI->first->lower) &&
		    (RII->first->upper >= U_2_W->upper + RI->first->upper) &&
		    check_redundant(S, ed_set[RI->first->enabling], 0,
				    RI->first, 2) &&
		    check_redundant(S, C_set, RI->first->enabling, 
				    RI->first->enabled)){
	      junk_R.insert(make_pair(RI->first, RI->second));
	    }
	  }
	  else if(!C_in_en_b && U_2_W &&
		  !U_2_W_initial && RI->second && RII->second){
	    
		  //cout << (RI_U_W_lower >= RII->first->lower)<< "  "
		 //<< (RI_U_W_upper >= RII->first->upper) << "  "
		 //<< initial_redundant(S, I->second, RII->first) << "  "
		 //<< check_redundant(S, ed_set[RII->first->enabling], W,
		//		    RII->first, 1)<< endl;
	    
	    if((RI_U_W_lower >= RII->first->lower) &&
	       (RI_U_W_upper >= RII->first->upper) &&
	       initial_redundant(S, I->second, RII->first) && 
	       check_redundant(S, ed_set[RII->first->enabling], W,
			       RII->first, 1) &&
	       check_redundant(S, C_set, RII->first->enabling, 
			       RII->first->enabled)){
	      junk_R.insert(make_pair(RII->first, RII->second));
	    }	  
	    else if((RII->first->lower == RI->first->lower) &&
		    (RII->first->upper == RI->first->upper) &&
		    check_redundant(S, ed_set[RI->first->enabling], 0,
				    RI->first, 2)){
	      cout <<"not implemented\n";
	    }
	  }	  
	}
      }
    }
    
    // This is for the following case:
    // a-->b, a-->c, b-->d, c-->d. The following code check if one of 
    //  (b-->d, c-->d) can be redundant to the other.
    /*if(modified==false){
      bool possible = true;
      set<eventsADT> source;
      map<eventsADT, pair<int,int> > path_delay;
      map<eventsADT, pair<rulesADT,bool> > enabling_r;
      long max_lower = 0, max_upper = 0;
      eventsADT max_e = 0;

      for(list<pair<rulesADT,bool> >::iterator iter = I->second.begin();
	  iter != I->second.end(); iter++){
	
	if(Rlst[iter->first->enabling].size() > 1)
	  possible = false;
	else if(Rlst[iter->first->enabling].front().second==false)
	  possible = false;
	else{
	  source.insert(Rlst[iter->first->enabling].front().first->enabling);
	  
	  long lower = iter->first->lower + 
	    Rlst[iter->first->enabling].front().first->lower;
	  long upper;
	  if(iter->first->upper==INFIN ||
	     Rlst[iter->first->enabling].front().first->upper==INFIN)
	    upper = INFIN;
	  else
	    upper = iter->first->upper + 
	      Rlst[iter->first->enabling].front().first->upper;

	  path_delay[iter->first->enabling] = make_pair(lower, upper);
	  enabling_r[iter->first->enabling] = *iter;

	  if(lower > max_lower){ 
	    max_lower = lower; max_upper = upper; 
	    max_e = iter->first->enabling;
	  }	  
	}
	
	if(possible==false || source.size() != 1)
	  break;
      }
      
      for(map<eventsADT, pair<int,int> >::iterator iter= path_delay.begin();
	  iter!=path_delay.end(); iter++){
	
	if(iter->first != max_e && iter->second.second <= max_lower)
	  if(iter->first->action->type != DUMMY){
	    pr_rules.insert(enabling_r[iter->first]);
	    
	  }
	  else{
	    pr_rules.insert(enabling_r[max_e]);
	    max_e = iter->first;
	    rulesADT tmp = enabling_r[iter->first].first;
	    tmp->lower += (max_lower-Rlst[iter->first].front().first->lower);
	    if(max_upper==INFIN ||
	       Rlst[iter->first].front().first->upper==INFIN)
	      tmp->upper = INFIN;
	    else
	      tmp->upper +=(max_upper-Rlst[iter->first].front().first->upper);
	  }
      }
    }*/
  }
    
  if(junk_R.size())
    modified = true;

  for(set<pair<rulesADT,bool> >::iterator I = junk_R.begin(); 
      I != junk_R.end(); I++){

    if (verbose)
      cout << "Removing redundant rule: " 
	   << I->first->enabling->action->label<<'/'
	   <<I->first->enabling->occurrence <<", "
	   <<I->first->enabled->action->label<<'/'
	   <<I->first->enabled->occurrence<<", " << I->second << ' '
	   << '['<<I->first->lower << ", "<< I->first->upper<<']'<< endl;

    if(I->second == false)
      S->R = rm_rules(S->R, I->first);
    else
      S->Rp = rm_rules(S->Rp, I->first);
  }

  if (verbose && modified)
    emitters(0, itos(cnt++).c_str(), S);

  return modified;
}


//*********************************************************
bool check_red(telADT S, 
	       rulesADT A_2_B, bool A_2_B_init,
	       rulesADT A_left, bool A_left_init,
	       rulesADT A_right, bool A_right_init,
	       rulesADT B_left, bool B_left_init,
	       rulesADT B_right, bool B_right_init,
	       map<eventsADT,list<pair<rulesADT,bool> > > enabling_Rset,
	       list<pair<rulesADT,bool> >& enabling_RsetB)
{
  long BL_A_B_lower, BL_A_B_upper, BR_A_B_lower, BR_A_B_upper;

  BL_A_B_lower = B_left->lower + A_2_B->lower;
  if(B_left->upper==INFIN || A_2_B->upper==INFIN)
    BL_A_B_upper = INFIN;
  else
    BL_A_B_upper = B_left->upper + A_2_B->upper;

  BR_A_B_lower = B_right->lower + A_2_B->lower;
  if(B_right->upper==INFIN || A_2_B->upper==INFIN)
    BR_A_B_upper = INFIN;
  else
    BR_A_B_upper = B_right->upper + A_2_B->upper;
  
  bool C_in_en_b = false;
  for(list<pair<rulesADT,bool> >::iterator iter = enabling_RsetB.begin();
      iter != enabling_RsetB.end(); iter++)
    if(!event_cmp(A_2_B->enabling, iter->first->enabling) && 
       check_C(S->C, A_2_B->enabling, iter->first->enabling))
      C_in_en_b = true;  
      
  if((A_2_B_init&&A_left_init&&A_right_init&&!B_left_init&&!B_right_init) ||
     (!A_2_B_init&&!A_left_init&&!A_right_init&&!B_left_init&&!B_right_init)){
    if(!C_in_en_b &&
       (BL_A_B_lower >= A_left->lower) &&
       (BL_A_B_upper >= A_left->upper) &&
       (BR_A_B_lower >= A_right->lower) &&
       (BR_A_B_upper >= A_right->upper))
      return true;
    else if((B_left->lower >= A_left->lower) &&
	    (B_left->upper >= A_left->upper) &&
	    (B_right->lower >= A_right->lower) &&
	    (B_right->upper >= A_right->upper))
      return true;
  }
  else if(!A_2_B_init&&A_left_init&&A_right_init&&B_left_init&&B_right_init){
    if(!C_in_en_b &&
       (BL_A_B_lower >= A_left->lower) &&
       (BL_A_B_upper >= A_left->upper) &&
       (BR_A_B_lower >= A_right->lower) &&
       (BR_A_B_upper >= A_right->upper) &&
       initial_redundant(S, enabling_Rset[A_left->enabled], A_left) &&
       initial_redundant(S, enabling_Rset[A_right->enabled], A_right))
      return true;
    else if((B_left->lower >= A_left->lower) &&
	    (B_left->upper >= A_left->upper) && 
	    (B_right->lower >= A_right->lower) &&
	    (B_right->upper >= A_right->upper))
      return true;
  }

  return false;
}


//*******************************************************
bool rm_red2(telADT S)
{
  bool modified = false;

  map<eventsADT,set<pair<eventsADT,bool> > > enabling_Eset, enabled_Eset;
  map<eventsADT,set<pair<eventsADT,bool> > > enabling_init, enabled_init;
  map<eventsADT,list<pair<rulesADT,bool> > > enabling_Rset, enabled_Rset;
  map<pair<eventsADT,eventsADT>, pair<rulesADT,bool> > Rset;  
  map<eventsADT,set<pair<eventsADT,bool> > > C_set;

  for(conflictsetADT cur_c = S->C; cur_c; cur_c = cur_c->link){
    eventsADT left = cur_c->conflict->left;
    eventsADT right = cur_c->conflict->right;
    C_set[left].insert(make_pair(right, false));
    C_set[right].insert(make_pair(left, false));
  }

  for(rulesetADT cur_rule = S->R; cur_rule; cur_rule = cur_rule->link){
    if ((cur_rule->rule->enabling->action->label != string("reset")) &&
	(!(cur_rule->rule->type & ORDERING))) {
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;

      enabling_Eset[ED].insert(make_pair(EN, false));
      enabled_Eset[EN].insert(make_pair(ED, false));

      enabling_init[ED].insert(make_pair((eventsADT)0, false));
      enabled_init[EN].insert(make_pair((eventsADT)0, false));

      enabling_Rset[ED].push_back(make_pair(cur_rule->rule, false));
      enabled_Rset[EN].push_back(make_pair(cur_rule->rule, false));

      Rset[make_pair(EN,ED)] = make_pair(cur_rule->rule,false);
    }
  }

  for(rulesetADT cur_rule = S->Rp; cur_rule; cur_rule = cur_rule->link){
    if (!(cur_rule->rule->type & ORDERING)) {
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;
      
      enabling_Eset[ED].insert(make_pair(EN, true));
      enabled_Eset[EN].insert(make_pair(ED, true));
      
      enabling_init[ED].insert(make_pair((eventsADT)0, true));
      enabled_init[EN].insert(make_pair((eventsADT)0, true));

      enabling_Rset[ED].push_back(make_pair(cur_rule->rule, true));
      enabled_Rset[EN].push_back(make_pair(cur_rule->rule, true));

      Rset[make_pair(EN,ED)] = make_pair(cur_rule->rule,true);
    }
  }

  map<rulesADT,set<rulesADT> > C_Rset_en, C_Rset_ed;

  for(conflictsetADT cur_c = S->C; cur_c; cur_c = cur_c->link){
    eventsADT left = cur_c->conflict->left;
    eventsADT right = cur_c->conflict->right;
    
    for(eventsetADT cur_e = S->O; cur_e; cur_e = cur_e->link){
      map<pair<eventsADT,eventsADT>, pair<rulesADT,bool> >::iterator 
	RL_en, RR_en, RL_ed, RR_ed;
      RL_en = Rset.find(make_pair(left, cur_e->event));
      RR_en = Rset.find(make_pair(right, cur_e->event));
      RL_ed = Rset.find(make_pair(cur_e->event, left));
      RR_ed = Rset.find(make_pair(cur_e->event, right));
      
      if(RL_en != Rset.end() && RR_en != Rset.end()){
	C_Rset_en[RL_en->second.first].insert(RR_en->second.first);
	C_Rset_en[RR_en->second.first].insert(RL_en->second.first);
      }
      
      if(RL_ed != Rset.end() && RR_ed != Rset.end()){
	C_Rset_ed[RL_ed->second.first].insert(RR_ed->second.first);
	C_Rset_ed[RR_ed->second.first].insert(RL_ed->second.first);
      }
    }
  }

  for(conflictsetADT cur_c = S->C; cur_c; cur_c = cur_c->link){
    eventsADT left = cur_c->conflict->left;
    eventsADT right = cur_c->conflict->right;

    map<eventsADT,list<pair<rulesADT,bool> > > junk_r;
    map<rulesADT,bool> junk_r_set;
    set<pair<rulesADT,bool> > true_red_set1, true_red_set2;

    for(map<pair<eventsADT,eventsADT>,pair<rulesADT,bool> >::iterator
	  cur_r = Rset.begin(); cur_r!=Rset.end(); cur_r++){
      
      map<pair<eventsADT,eventsADT>, pair<rulesADT,bool> >::iterator 
	A_left, A_right, B_left, B_right;
	
      A_left = Rset.find(make_pair(cur_r->first.first, left));
      A_right = Rset.find(make_pair(cur_r->first.first, right));
      B_left = Rset.find(make_pair(cur_r->first.second, left));
      B_right = Rset.find(make_pair(cur_r->first.second, right));
      if(!(A_left!=Rset.end() && A_right!=Rset.end() && 
	   B_left!=Rset.end() && B_right != Rset.end()))
	continue;
      
      /*cout << "A -> B "<<cur_r->second.first->enabling->action->label<<'/'
	   <<cur_r->second.first->enabling->occurrence <<", "
	   <<cur_r->second.first->enabled->action->label<<'/'
	   <<cur_r->second.first->enabled->occurrence<<", " 
	   << cur_r->second.second << ' '
	   << '['<<cur_r->second.first->lower << ", "
	   << cur_r->second.first->upper<<']'
	   << endl;
      
      cout << left->action->label<< " ### "<< right->action->label<< endl;
      
      
      cout << cur_r->second.second<< " "
	   << A_left->second.second<< " "
	   << A_right->second.second<< " "
	   << B_left->second.second<< " "
	   <<B_right->second.second<< endl;
      */
      if(check_red(S, 
		   cur_r->second.first, cur_r->second.second,
		   A_left->second.first, A_left->second.second,
		   A_right->second.first, A_right->second.second,
		   B_left->second.first, B_left->second.second,
		   B_right->second.first, B_right->second.second,
		   enabling_Rset,
		   enabling_Rset[cur_r->first.second])){
	junk_r[left].push_back(A_right->second);
	junk_r[right].push_back(A_left->second);
	junk_r_set[A_left->second.first] = A_left->second.second;
	junk_r_set[A_right->second.first] = A_right->second.second;
      }
    }
    
    /*    for(map<rulesADT,bool>::iterator iter = junk_r_set.begin();
        iter != junk_r_set.end(); iter++)
      cout<<"<<< "<< iter->first->enabling->action->label<<'/'
          <<iter->first->enabling->occurrence <<", "
          <<iter->first->enabled->action->label<<'/'
          <<iter->first->enabled->occurrence<<", " << iter->second << ' '
          << '['<<iter->first->lower << ", "<< iter->first->upper<<']'
          << endl;
    cout << endl<< endl;
    */

    for(map<rulesADT,set<rulesADT> >::iterator I=C_Rset_en.begin();
	I!= C_Rset_en.end(); I++){
      map<rulesADT,bool>::iterator ppr = junk_r_set.find(I->first);
      if(ppr==junk_r_set.end())  continue;

      bool true_red = true;

      for(set<rulesADT>::iterator II=I->second.begin(); II!= I->second.end();
	  II++)
	if(junk_r_set.find(*II)==junk_r_set.end()){
	  true_red = false;
	  break;
	}
      //cout << true_red<< "  1"<< endl;
      if(true_red)
	if(true_red_set1.find(make_pair(ppr->first, ppr->second)) != true_red_set1.end())
	  cout << "Error line 1103\n";
	else
	  true_red_set1.insert(make_pair(ppr->first, ppr->second));
    }

    for(map<rulesADT,set<rulesADT> >::iterator I=C_Rset_ed.begin();
	I!= C_Rset_ed.end(); I++){      
      map<rulesADT,bool>::iterator ppr = junk_r_set.find(I->first);
      if(ppr==junk_r_set.end())  continue;
      cout<< I->first->enabling->action->label<< " "<< I->second.size()<<endl;
      bool true_red = true;

      for(set<rulesADT>::iterator II=I->second.begin(); II!= I->second.end();
	  II++)
	if(junk_r_set.find(*II)==junk_r_set.end()){
	  true_red = false;
	  break;
	}
      //cout << true_red<< "  2"<< endl;
      if(true_red)
	if(true_red_set2.find(make_pair(ppr->first, ppr->second)) != true_red_set2.end())
	  cout << "Error line 1126\n";
	else
	  true_red_set2.insert(make_pair(ppr->first, ppr->second));
    }
    /*
    for(set<pair<rulesADT,bool> >::iterator iter = true_red_set1.begin();
        iter != true_red_set1.end(); iter++)
      cout<<iter->first->enabling->action->label<<'/'
          <<iter->first->enabling->occurrence <<", "
          <<iter->first->enabled->action->label<<'/'
          <<iter->first->enabled->occurrence<<", " << iter->second << ' '
          << '['<<iter->first->lower << ", "<< iter->first->upper<<']'
          << endl;

    cout <<"---------------------------"<< endl;
    for(set<pair<rulesADT,bool> >::iterator iter = true_red_set2.begin();
        iter != true_red_set2.end(); iter++)
      cout<<iter->first->enabling->action->label<<'/'
          <<iter->first->enabling->occurrence <<", "
          <<iter->first->enabled->action->label<<'/'
          <<iter->first->enabled->occurrence<<", " << iter->second << ' '
          << '['<<iter->first->lower << ", "<< iter->first->upper<<']'
          << endl;
    cout<< endl<< endl;;

    */
    set<pair<rulesADT,bool> > true_red_set;

    set_intersection(true_red_set1.begin(), true_red_set1.end(),
		     true_red_set2.begin(), true_red_set2.end(),
		     inserter(true_red_set, true_red_set.begin()));

    for(set<pair<rulesADT,bool> >::iterator iter = true_red_set.begin();
	iter != true_red_set.end(); iter++){
      
      if(iter->second)
	S->Rp = rm_rules(S->Rp, iter->first);
      else
	S->R = rm_rules(S->R, iter->first);

      /*cout<<"*** "<<iter->first->enabling->action->label<<'/'
	  <<iter->first->enabling->occurrence <<", "
	  <<iter->first->enabled->action->label<<'/'
	  <<iter->first->enabled->occurrence<<", " << iter->second << ' '
	  << '['<<iter->first->lower << ", "<< iter->first->upper<<']'
	  << endl;
      */
    }
    if(true_red_set.size())
      modified = true;
  }

  if(modified)
    emitters(0, itos(cnt++).c_str(), S);
  return modified;
}    

   
//*********************************************************************
void rm_red1(telADT S)
{
  cout << "Checking redundant rules ... ";
  typedef map<eventsADT,map<eventsADT,pair<bool,bool> > > str2_b2_m;
  typedef map<eventsADT,list<pair<rulesADT,bool> > > erb2l_m;
  typedef map<eventsADT,set<eventsADT> > eventset;
  str2_b2_m Rvec;
  erb2l_m Rlst;
  eventset Envec;

  for(rulesetADT cur_rule = S->R; cur_rule; cur_rule = cur_rule->link){
    if ((cur_rule->rule->enabling->action->label != string("reset")) &&
	(!(cur_rule->rule->type & ORDERING))) {
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;
      Rvec[EN][ED] = make_pair(true, true);
      Rlst[ED].push_back(make_pair(cur_rule->rule, true));
      Envec[ED].insert(EN);
    }
  }
  for(rulesetADT cur_rule = S->Rp; cur_rule; cur_rule = cur_rule->link){
    if (!(cur_rule->rule->type & ORDERING)) {
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;
      Rvec[EN][ED] = make_pair(true, false);
      Rlst[ED].push_back(make_pair(cur_rule->rule, false));
      Envec[ED].insert(EN);
    }
  }

  /*for(map<eventsADT,set<eventsADT> >::iterator I=Envec.begin();I!=Envec.end();
      I++){
    //cout <<I->first->action->label<<'/'<<I->first->occurrence<< " ->  ";
    for(set<eventsADT>::iterator II=I->second.begin(); II!=I->second.end(); II++)
      //cout <<(*II)->action->label<<'/'<<(*II)->occurrence<< " ";
      //cout << endl;
  }
  cout << endl;
  */

  eventsetADT E = link_event(S->I, S->O);
  for(eventsetADT C = E; C; C = C->link){
    eventsADT CE = C->event;
    
    for(eventsetADT I = E; I; I = I->link){
      eventsADT IE = I->event;
      for(eventsetADT II = E; II; II = II->link){
	eventsADT IIE = II->event;
	if(Rvec[IE][CE].first && Rvec[CE][IIE].first)
	  if((!(Rvec[IE][IIE].second)) && 
	     (Rvec[IE][CE].second == false || Rvec[CE][IIE].second == false)){
	    if (Rvec[IE][CE].second == true || Rvec[CE][IIE].second == true)
	      Rvec[IE][IIE] = make_pair(true, false);
	  } else
	    Rvec[IE][IIE] = make_pair(true, true);
      }
    }
  }
  
  /*
  map<eventsADT,map<eventsADT,pair<bool,bool> > >::iterator I;
  map<eventsADT,pair<bool,bool> >::iterator II;
  for(I = Rvec.begin(); I!=Rvec.end(); I++){
    cout<<I->first->action->label<<": ";
    for(II = (I->second).begin(); II!=(I->second).end(); II++){
      if(II->second.first)
	cout<<"("<<II->first->action->label<<" "<<II->second.first<<" "
	    << II->second.second<<") ";
    }
    cout<<endl;
  }
  */
  
  // Part 2.
  
  conflictsetADT Cset = link_conflict(S->C, S->Cp);
  set<pair<rulesADT,bool> > junk_R;

  remove("rrules");
  ofstream os("rrules");

  //remove the redundancies depending on the following conditions  
  // I->first: enabled event; 
  // I->second: a list of rules enabling I->first;
  
  for(erb2l_m::iterator I = Rlst.begin(); I != Rlst.end(); I++){
    // if there is only one rule enabling 'I->first', drop it.
    if(I->second.size() <= 1) 
      continue;
    
    list<pair<rulesADT,bool> >::iterator RI, RII;
    set<pair<rulesADT,bool> > pr_rules;

    for(RII = I->second.begin(); RII != I->second.end(); RII++) {
      eventsADT U = RII->first->enabling;
      // This conflict check may be conservative
      //bool conflictfree=true;
      /*map<eventsADT,pair<bool,bool> >* EI = &(Envec[I->first]);
      for(eventsetADT F = E; F; F = F->link)
	if(EI->find(F->event) == EI->end() &&
	   ((check(U,F->event) && in_conflict_set(check(U,F->event),C))||
	    (check(F->event,U) && in_conflict_set(check(F->event,U),C)))) {
	  conflictfree=false;
	  break;
	  }*/

      //if (conflictfree == true) {
      for(RI = I->second.begin(); RI != I->second.end(); RI++) {
	eventsADT W = RI->first->enabling;

	if((U!=W) && (!check(U,W) || !in_conflict_set(check(U,W), Cset)) &&
	   (!check(W,U) || !in_conflict_set(check(W,U), Cset)) &&
	   (RI->first->lower == RII->first->lower) &&
	   (RI->first->upper == RII->first->upper))
	  if(Rvec[U][W].first==true &&
	     ((RI->second==true&&RII->second==true&&Rvec[U][W].second==true)||
	      (RI->second==true&&RII->second==false&&Rvec[U][W].second==false)||

	      (RI->second==false&&RII->second==false&&Rvec[U][W].second==true)
	      )){
	    pr_rules.insert(make_pair(RII->first, RII->second));
	    /*os << RII->first->enabling->action->label<<" " 
	      << RII->first->enabled->action->label << " " 
	      << RII->second << endl;*/
	    break;
	  }
      }
    }
    //}

    // part3: Find the truly redundant rules from potentially redundant rules.
    set<eventsADT>* rr_enabling_set = &(Envec[I->first]);
    //cout << endl<< endl;
    //cout << I->first->action->label<<'/'<<I->first->occurrence 
    //<<" ->\n----------------------------\n";

    for(set<pair<rulesADT,bool> >::iterator ipr = pr_rules.begin();
	ipr != pr_rules.end(); ipr++){
      bool redundant = true;

      /*   cout << '('<<ipr->first->enabling->action->label<<'/'
	   <<ipr->first->enabling->occurrence<< ' '
	   <<ipr->first->enabled->action->label<<'/'
	   <<ipr->first->enabled->occurrence<< ')'<< endl;
      */
      // find enabling event set for event 'ip->first->enabled'.
      // ip->first is a potentially redundant rule.
      //set<eventsADT> rr_enabling_set;
      for(set<eventsADT>::iterator ipe = rr_enabling_set->begin();
	  ipe != rr_enabling_set->end(); ipe++)
	;//cout << (*ipe)->action->label<<'/'<<(*ipe)->occurrence<< ' ';
      //cout << endl<<"----------------------------\n";
      
      // find enabling event set for the event which is in conflict with
      // event 'ip->first->enabled'.
      for(eventsetADT C = E; C; C = C->link) {
	eventsADT CE = C->event;
	
	if(check(CE, I->first) && in_conflict_set(check(CE,I->first), Cset)){
	  
	  set<eventsADT> *CE_enabling_set = &(Envec[CE]);
	  
	  //cout << ">  "<<CE->action->label<<'/'<<CE->occurrence
	  //   <<" ->\n   ----------------------------\n";
	  for(set<eventsADT>::iterator ipe = CE_enabling_set->begin();
	      ipe != CE_enabling_set->end(); ipe++)
	    ;//cout << "   "<<(*ipe)->action->label<<'/'<<(*ipe)->occurrence;
	  //cout << endl<<"   ----------------------------\n";
	  
	  
	  if(CE_enabling_set->empty()) break;
	  
	  //cout << endl
	  //   << rr->action->label<<": ";
	  
	  //cout << endl;
	  
	  if(!includes(CE_enabling_set->begin(), CE_enabling_set->end(),
		       rr_enabling_set->begin(), rr_enabling_set->end()) &&
	     !includes(rr_enabling_set->begin(), rr_enabling_set->end(),
		       CE_enabling_set->begin(), CE_enabling_set->end())){
	    
	    rr_enabling_set->erase(ipr->first->enabling);

	    if(includes(CE_enabling_set->begin(), CE_enabling_set->end(),
			rr_enabling_set->begin(), rr_enabling_set->end()) ||
	       includes(rr_enabling_set->begin(), rr_enabling_set->end(),
			CE_enabling_set->begin(), CE_enabling_set->end()))
	      redundant = false;
	      
	    rr_enabling_set->insert(ipr->first->enabling);
	    
	    if(redundant==false)
	      break;
	  }
	}	
      }
      
      if(redundant==true){
	junk_R.insert(*ipr); 
	//rr_enabling_set->erase(ipr->first->enabling);
	//cout << "*** "<<ipr->first->enabling->action->label<< " ***\n";
      }
    }
  }

  os << endl << endl;
  //remove("rrules");
  //ofstream os("rrules");
  for(set<pair<rulesADT,bool> >::iterator I = junk_R.begin(); 
      I != junk_R.end(); I++){
    os <<I->first->enabling->action->label<<'/'
       <<I->first->enabling->occurrence <<", "
       <<I->first->enabled->action->label<<'/'
       <<I->first->enabled->occurrence<<", " << I->second << ' '
       << '['<<I->first->lower << ", "<< I->first->upper<<']'<< endl;
    
    if(I->second == true)
      S->R = rm_rules(S->R, I->first);
    else
      S->Rp = rm_rules(S->Rp, I->first);
  }	
  os.close();

  cout << "done\n";
}


void update_io(TERstructADT target, const map<string,int>& new_io)
{
  actionsetADT cur_action;
 
  for (cur_action = target->A; cur_action; cur_action = cur_action->link) {
    if(cur_action->action->type != DUMMY){
      map<string,int>::const_iterator tmp_i=new_io.find(cur_action->action->label);
      if(tmp_i != new_io.end())
	cur_action->action->type = tmp_i->second; 
    }
  }
}


string new_dummy(const string &id, const string& Iid, const string& Cid)
{
  string result(id);
  if(!Cid.empty()){
    result += '_' + Cid;
  }
  if(!Iid.empty()){
    result += '_' + Iid;
  }
  return result;
}

/***************************************************************************/
/* Return a new TEL structure which is a copy of S with all action names   */
/* replaced by the names in the list l.                                    */
/***************************************************************************/
// scope: component instance label;
// component_name: component name.
TERstructADT instantiate(TERstructADT S, my_list *l, 
			 char *scope,
			 char *component_name, bool csp)
{
  TERstructADT ret_val = TERSempty();
  if (S){
    ret_val->A = instantiate_action_set(S->A, l, scope, component_name, csp);
    ret_val->I = instantiate_event_set(S->I, l, scope, component_name, csp);
    ret_val->O = instantiate_event_set(S->O, l, scope, component_name, csp);
    ret_val->first = instantiate_event_set(S->first, l, 
					   scope, component_name, csp);
    ret_val->last = instantiate_event_set(S->last, l, scope, 
					  component_name, csp);
    ret_val->loop = instantiate_event_set(S->loop, l, scope, 
					  component_name, csp);
    
    
    ret_val->R = instantiate_rule_set(S->R, l, scope, component_name, csp);
    ret_val->Rp = instantiate_rule_set(S->Rp, l, scope, component_name, csp);
    ret_val->C = instantiate_conflict_set(S->C, l, scope, component_name, csp);
    ret_val->Cp = instantiate_conflict_set(S->Cp, l, scope, 
					   component_name, csp);
  }
  return ret_val;
}


/***************************************************************************/
/* Return a new action set  which is a copy of A with all labels           */
/* replaced by the names in the list l.                                    */
/***************************************************************************/

actionsetADT instantiate_action_set(actionsetADT A, my_list *l, 
				    char *scope,
				    char *C_id, bool csp)
{
  actionsetADT ret_val = 0;
  actionsetADT cur_action;
  actionADT tmp, inverse;

  for (cur_action = A; cur_action; cur_action = cur_action->link) {
    if(cur_action->action->type & DUMMY){
      tmp = action(new_dummy(cur_action->action->label,scope,C_id));
      tmp->type = DUMMY;
      // ZHEN(Done): copy assignments from old action to new action
      // assignment variables (lhs) should be renamed (+ "_" + scope)
      // possibly also on rhs need to be renamed
      if (cur_action->action->list_assigns != NULL)
      {
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	tmp->list_assigns = CopyRenameList(cur_action->action->list_assigns, scope);
	cout << "label = " << tmp->label << endl;
	cout << "assignment: " << tmp->list_assigns->var << ":= " << tmp->list_assigns->assign << endl;
      }
    }		     
    else{
      char *label = new_label(cur_action->action->label, l, scope, csp);
      char *inverse_label = CopyString(label);
      int len = strlen(inverse_label);
      char state = inverse_label[len - 1];
      
      if(state == '+')
	inverse_label[len - 1] = '-';
      else if(state == '-')
	inverse_label[len - 1] = '+';	
      
      tmp = action(label, strlen(label));
      inverse = action(inverse_label, len);
      //cout << "#####################################" << endl;
      //cout << "label = " << tmp->label << endl;
      //cout << "cur_action->action->type = " << tmp->type << endl;
      
      //tmp->type = cur_action->action->type;

      if(tmp != cur_action->action && cur_action->action->type & OUT){
	if(tmp->type == UNKNOWN || tmp->type & ISIGNAL) {
	  tmp->type = cur_action->action->type;
	  tmp->initial = cur_action->action->initial;
	  tmp->lower = cur_action->action->lower;
	  tmp->upper = cur_action->action->upper;
	  tmp->delay = cur_action->action->delay;
	  tmp->initial_state.initial_value_known = true;
	  tmp->initial_state.initial_delay_known = true;
	}
	else{
	  if(tmp->initial_state.initial_value_known == false) {
	    tmp->initial = cur_action->action->initial;
	    tmp->initial_state.initial_value_known = true;
	  }
	  if(tmp->initial_state.initial_delay_known == false){
	    tmp->lower = cur_action->action->lower;
	    tmp->upper = cur_action->action->upper;
	    tmp->delay = cur_action->action->delay;
	    tmp->initial_state.initial_delay_known = true;
	  }
	}
	tmp->vacuous = cur_action->action->vacuous;
	tmp->odata = cur_action->action->odata;
	tmp->idata = cur_action->action->idata;
	//tmp->delay = cur_action->action->delay;
	// Where is the "*dist"
	tmp->maxoccur = cur_action->action->maxoccur;
      }
      free(label);
      free(inverse_label);
    }
    if(!(cur_action->action->type & DUMMY)){
      tmp->inverse = inverse;
    }
    actionsetADT AA = create_action_set(tmp);
    actionsetADT previous = ret_val;
    ret_val = union_actions(ret_val, AA);
    
    delete_action_set(previous);
    delete AA;            
  }  
  return ret_val;
}

assignList CopyRenameList(assignList head, char* scope) 
{ 
  assignList current = head;	// used to iterate over the original list assignList newList = NULL;	
  assignList newList = NULL;    // head of the new list assignList tail = NULL; 
  assignList tail = NULL;       // kept pointing to the last node in the new list
  char var_append[255];
  while (current != NULL) { 
    if (newList == NULL) { // special case for the first new node
      newList = (assignList)GetBlock(sizeof(*newList)); 
      var_append[0]='\0';
      strcat(var_append,current->var);
      strcat(var_append,"_");
      strcat(var_append,scope);
      newList->var = CopyString(var_append); 
      newList->assign = CopyString(current->assign);
      newList->next = NULL; 
      tail = newList;
    } 
    else {
      tail->next = (assignList)GetBlock(sizeof(*(tail->next))); 
      tail = tail->next; 
      var_append[0]='\0';
      strcat(var_append,current->var);
      strcat(var_append,"_");
      strcat(var_append,scope);
      tail->var = CopyString(var_append); 
      tail->assign = CopyString(current->assign);
      tail->next = NULL;
    } 
    current = current->next;
  }
  return(newList);
}

/****************************************************************************/
/* Return a new event set which is a copy of E with all labels              */
/* replaced by the names in the list l.                                     */
/****************************************************************************/

eventsetADT instantiate_event_set(eventsetADT E, my_list *l, 
				  char *scope,
				  char *component_name, bool csp)
{
  eventsetADT ret_val = 0;

  eventsetADT cur_event;
  eventsADT tmp;
  string newData;

  for(cur_event = E; cur_event; cur_event = cur_event->link) {
    actionADT a = 0; 
    if(cur_event->event->action->type & DUMMY) {
      a = action(new_dummy(cur_event->event->action->label,
			   scope, component_name));
      newData = cur_event->event->data;
    }else{
      char *label = new_label(cur_event->event->action->label, l, scope, csp);
      newData = new_label_string(cur_event->event->data.c_str(), l, scope, csp);
      // New action with new action label.
      a = action(label, strlen(label));
      free(label);
    }
    // New event with new action.
    tmp = event(a, cur_event->event->occurrence, cur_event->event->lower,
		cur_event->event->upper,cur_event->event->dist,"[true]",
		FALSE,newData);
      
    if (tmp != cur_event->event){
      if(tmp->exp)
	free(tmp->exp);
      //cout << "New expr = " << cur_event->event->exp << endl;
      tmp->exp = new_expression(cur_event->event->exp, l, scope, csp);
    }
    eventsetADT EE = create_event_set(tmp);
    eventsetADT previous = ret_val;
    ret_val = union_events(ret_val, EE);

    delete_event_set(previous);
    delete EE;      
  }

  return ret_val;
}


/***************************************************************************/
/* Return a new rule set which is a copy of E with all labels              */
/* replaced by the names in the list l.                                    */
/***************************************************************************/

rulesetADT instantiate_rule_set(rulesetADT R, my_list *l, 
				char *scope,
				char *component_name, bool csp)
{
  rulesetADT ret_val = 0;

  rulesetADT cur_rule;
  rulesADT tmp;
  string newEnablingData;
  string newEnabledData;

  for (cur_rule = R; cur_rule; cur_rule=cur_rule->link){ 
    //    if(cur_rule->rule->regular == false){
    //  cout << "error: output signal '" 
    //	   << cur_rule->rule->enabled->action->label 
    //	   << "' used as an input" << endl;
    //  continue;
    //}
      
    actionADT new_enabling_action, new_enabled_action;
    char *Ecur_label = cur_rule->rule->enabling->action->label;
    char *Dcur_label = cur_rule->rule->enabled->action->label;
    
    // Enabling events:
    if(cur_rule->rule->enabling->action->type & DUMMY){
      string Aid = new_dummy(Ecur_label, scope, component_name);
      new_enabling_action = action(Aid);
      newEnablingData = cur_rule->rule->enabling->data;
    }
    else{
      char *new_enabling_label = new_label(cur_rule->rule->	
					   enabling->action->label,
					   l, scope, csp);
      newEnablingData = new_label_string(cur_rule->rule->enabling->data.c_str(),l,scope,csp);
      new_enabling_action = action(new_enabling_label, 
				   strlen(new_enabling_label));
      free(new_enabling_label);
    }
    // Enabled events:
    if(cur_rule->rule->enabled->action->type & DUMMY){
      string Aid = new_dummy(Dcur_label, scope, component_name);
      new_enabled_action = action(Aid);
      newEnabledData = cur_rule->rule->enabled->data;
    }
    else {
      char *new_enabled_label = new_label(cur_rule->rule->	
					  enabled->action->label,
					  l, scope, csp);
      newEnabledData = new_label_string(cur_rule->rule->enabled->data.c_str(),l,scope,csp);
      // New enabled action of the current rule.
      new_enabled_action = action(new_enabled_label, 
				  strlen(new_enabled_label));
      free(new_enabled_label);
    }
    
    // Using new_enabling_action to find new enabling event 
    // in the current rule. 
    eventsADT new_enabling_event = 
      event(new_enabling_action,	
	    cur_rule->rule->enabling->occurrence,
	    cur_rule->rule->enabling->lower,
	    cur_rule->rule->enabling->upper,
	    cur_rule->rule->enabling->dist,
	    cur_rule->rule->enabling->exp,
	    FALSE,
	    newEnablingData
	    );
    
    // Using new_enabled_action to find new enabled event 
    // in the current rule. 
    eventsADT new_enabled_event = 
      event(new_enabled_action,	
	    cur_rule->rule->enabled->occurrence,
	    cur_rule->rule->enabled->lower,
	    cur_rule->rule->enabled->upper,
	    cur_rule->rule->enabled->dist,
	    cur_rule->rule->enabled->exp,
	    FALSE,
	    newEnabledData
	    );

    // Find or create the rule with new enabling and enabled events.
    tmp = rule(new_enabling_event, new_enabled_event, cur_rule->rule->lower,
	       cur_rule->rule->upper, cur_rule->rule->type,
	       cur_rule->rule->regular,cur_rule->rule->dist);

    if (tmp != cur_rule->rule){
      if(tmp->exp)
	free(tmp->exp);
      tmp->exp = new_expression(cur_rule->rule->exp, l, scope, csp);
    }
      
    rulesetADT RR;                           // create_rule_set
    RR = (rulesetADT) GetBlock(sizeof(*RR));  // cannot be 
    RR->rule = tmp;                           // compiled.
    RR->link = 0;
    
    rulesetADT previous = ret_val;
    ret_val = union_rules(ret_val, RR);//create_rule_set(tmp));
    
    delete_rule_set(previous);
    free(RR);
  }
  return ret_val; 
}



/***************************************************************************/
/* Return a new conflict set which is a copy of E with all labels          */
/* replaced by the names in the list l.                                    */
/***************************************************************************/

conflictsetADT instantiate_conflict_set(conflictsetADT C, 
					my_list *l,
					char *scope,
					char *component_name, bool csp)
{
  conflictsetADT ret_val = 0;
  conflictsetADT cur_conflict;
  string newLeftData;
  string newRightData;

  for (cur_conflict= C; cur_conflict; cur_conflict = cur_conflict->link) {
    actionADT new_left_action, new_right_action;

    char *l_id = cur_conflict->conflict->left->action->label;
    char *r_id = cur_conflict->conflict->right->action->label;

    if(cur_conflict->conflict->left->action->type & DUMMY){
      string Aid= new_dummy(l_id, scope, component_name);
      new_left_action = action(Aid);
      newLeftData = cur_conflict->conflict->left->data;
    }
    else{
      char *left_label = new_label(cur_conflict->conflict->
				   left->action->label,
				   l, scope, csp);
      new_left_action = action(left_label, strlen(left_label));
      newLeftData = new_label_string(cur_conflict->conflict->left->data.c_str(),l,scope,csp);
      free(left_label);
    }

    if(cur_conflict->conflict->right->action->type & DUMMY){
      string Aid= new_dummy(r_id, scope, component_name);
      new_right_action = action(Aid);
      newRightData = cur_conflict->conflict->right->data;
    }
    else{
      char *right_label = new_label(cur_conflict->conflict->
				    right->action->label,
				    l, scope, csp);
      new_right_action = action(right_label, strlen(right_label));
      newRightData = new_label_string(cur_conflict->conflict->right->data.c_str(),l,scope,csp);
      free(right_label);
    }
    
    eventsADT new_left_event = 
      event(new_left_action,	
	      cur_conflict->conflict->left->occurrence,
	    cur_conflict->conflict->left->lower,
	    cur_conflict->conflict->left->upper,
	    cur_conflict->conflict->left->dist,
	    cur_conflict->conflict->left->exp,
	    FALSE,
	    newLeftData
	    );
    
    eventsADT new_right_event = 
      event(new_right_action,	
	    cur_conflict->conflict->right->occurrence,
	    cur_conflict->conflict->right->lower,
	    cur_conflict->conflict->right->upper,
	    cur_conflict->conflict->right->dist,
	    cur_conflict->conflict->right->exp,
	    FALSE,
	    newRightData
	    );

    ret_val = add_conflict(ret_val,new_left_event, new_right_event);
  }
  
  return ret_val; 
}


/***************************************************************************/
/* Find the new label in the list 'l' corresponding to the 'label'         */
/***************************************************************************/

char *new_label(const char *label, my_list *l, const char *scope, bool csp)
{
  string new_name;
  if(csp){
    new_name = l->match(prefix(label), scope);
  }
  else{
    new_name = l->match(Tolower(prefix(label)), scope);
  }
  new_name += suffix(label);
  return strdup(new_name.c_str());
}


/***************************************************************************/
/* Find the new label in the list 'l' corresponding to the 'label'         */
/***************************************************************************/

string new_label_string(const char *label, my_list *l, const char *scope, bool csp)
{
  string new_name;
  if(csp){
    new_name = l->match(prefix(label), scope);
  }
  else{
    new_name = l->match(Tolower(prefix(label)), scope);
  }
  new_name += suffix(label);
  
  return new_name;
}


/***************************************************************************/
/* Replace identifiers in the 'expr' by the corresponding ones in the list'l'*/
/***************************************************************************/

char *new_expression(const char *expr, my_list *l, const char *scope, bool csp)
{
  if(expr == string("[true]") || expr == string("[false]") || 
     expr == string("[maybe]") || expr == string("[~(maybe)]"))
    return CopyString(expr);
  if(expr == string("[]"))
    return CopyString("[true]");
  char *ss = copy_string(expr);
  char *sep = "[]()&|,~ ";
  string *id_vec;
  char *operators;// = { '\0' };
  int num_of_op = 0;
  int index = 0;
  char *p ;

  id_vec = new string[strlen(expr)+1];
  operators = new char[strlen(expr)+1];
  operators[0] = '\0';

  id_vec[index++] = strtok(ss, sep);
  while((p = strtok(0, sep))) {
    id_vec[index++] = p;
    //cout << "p: " << p << endl;
  }
  for (int i = 0; i < index; i++){
    if (id_vec[i]!="bit" && id_vec[i]!="0" && id_vec[i]!="1" && id_vec[i]!="2" && id_vec[i]!="3" && id_vec[i]!="4" && id_vec[i]!="5") {
      //if (id_vec[i]!="bit" && !isdigit(id_vec[i])) {
      // ZHEN: Modify the above "if" condition to handle all digits.
      id_vec[i] = new_label(id_vec[i].c_str(),l,scope,csp);
      //cout << "****************" << endl;
      //cout << "id_vec[" << i << "].c_str(): " << id_vec[i].c_str() << endl;
      //cout << "l: " << *l << endl;
      //cout << "scope: " << scope << endl;
    }
  }
 
  int k = 0;
  bool seperator = true;
  while (expr[k]){
    if (expr[k] == '[' || expr[k] == ']' || 
	(expr[k] == 'd' && k>0 && expr[k-1] == ']') || 
	expr[k] == '(' || expr[k] == ')'  ||
	expr[k] == '&' || expr[k] == '|'  || expr[k] == ','  ||
	expr[k] == ' ' || expr[k] == '~') {
      operators[num_of_op++] = expr[k];
      seperator = true;
    }
    else{
      operators[num_of_op] = '-';
      if(seperator)
	num_of_op++;
      seperator = false;
    }
    k++;
  }
  
  //ostringstream outs;
  string r_str;
  int ptr = 0;
  for(int i = 0; i < num_of_op; i++)
    if (operators[i] == '[' || operators[i] == ']' || 
	(operators[i] == 'd' && i>0 && operators[i-1] == ']')  || 
	operators[i] == '(' || operators[i] == ')' ||
	operators[i] == '&' || operators[i] == '|' || operators[i] == ',' ||
	operators[i] == ' ' || operators[i] == '~')
      r_str += operators[i];	
    else if (operators[i] == '-')
      r_str += id_vec[ptr++].c_str();
	
  //  outs<< ends; 

  delete [] ss;
  return CopyString(r_str.c_str());
}


char *array_relation(SymTab *table, TYPES *t1, TYPES *t3, bool &error)
{
  /* const char *base_ty;
  if(t1->get_grp() == TYPES::Array)
    base_ty = t1->base_type();
  else
    base_ty = t3->base_type();

  TYPES *type = table->lookup(base_ty);
  if(type == 0 || strcmp(type->obj_type(), "err") == 0)
    {
      error = false;
      return "false";
    }
  
  ostringstream ret_val;
  char *sig_name;
  char *sig_lit;
  if(t1->get_grp() == TYPES::Array)
     {
       sig_name = t1->get_string();
       sig_lit = t3->get_string();
     }
  else
    {
      sig_name = t3->get_string();
      sig_lit = t1->get_string();
    }

  int loop_n = strlen(sig_lit);
  for(int i = 0; i < loop_n; i++)
    {
      if(i)
	ret_val<< "& ";

      char temp[10];
      sprintf(temp, "'%c'", sig_lit[i]);
      if(type->find(temp) && (sig_lit[i] == '0' || sig_lit[i] == '1'))
	{
	  if(sig_lit[i] == '0')
	    ret_val<< "~"<< sig_name<< "__"<< i<< "__ ";
	  else
	    ret_val<< sig_name<< "__"<< i<< "__ ";
	}
      else
	{
	  error = false;
	  return "false";
	}
    }
   ret_val<< ends;
   return ret_val.str();*/
  return 0;
}


pair<string,string> split(const string& SS, char C, bool B)
{
  string::size_type pos = 0; 
  if(B)
    pos = SS.rfind(C);
  else
    pos = SS.find(C);

  if((pos+1) == 0)
    return make_pair(SS, string());
  else{
    string S1(SS, 0, pos);
    string S2(SS, pos+1, SS.size()-pos-1);
    return make_pair(S1, S2);
  }
} 

//void drop_env_rules()
