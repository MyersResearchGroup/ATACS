#include <cassert>
#include <string>
#include "create_env.h"
#include "hse.hpp"
#include "tels_table.h"
#include "tersgen.h"

tels_table::tels_table()
{}


tels_table::tels_table(const tels_table& TT)
{
  for(const_iterator b = TT.processes.begin(); b != TT.processes.end(); b++)
    processes[b->first]=make_pair(TERScopy(b->second.first),b->second.second);

  for(map<string, tels_table*>::const_iterator b = TT.modules.begin();
      b != TT.modules.end(); b++)
    modules[b->first] = new tels_table(*(b->second));

  label = TT.label;
  type = TT.type;
  interface = TT.interface;
  sig_list = TT.sig_list;
  sig_list1 = TT.sig_list1;
}


tels_table::tels_table(const tels_table* TT)
{
  *this = *TT;
}


tels_table::~tels_table()
{
  for(map<string, pair<TERstructADT, bool> >::iterator b = processes.begin();
      b != processes.end(); b++)
    TERSdelete(b->second.first);

  for(map<string, tels_table*>::iterator b = modules.begin();
      b != modules.end(); b++)
    delete b->second;
}


void tels_table::set_id(const string&  SS)
{
  label = SS;
}

string tels_table::id(void)
{
  return label;
}


void tels_table::update(const string& SS)
{
  label = SS + '.' + label; 
}



TERstructADT tels_table::tel(const string& SS, bool abstract)
{
#ifdef DDEBUG
  assert(SS.size());
#endif
  pair<tels_table*, TERstructADT> TT = compose(SS);
  
  if(TT.first == 0)
    return 0;

  TERstructADT behv = TT.first->compose();
  
  map<string,int> ports;
  for(s_iterator b = TT.first->interface.begin(); 
      b != TT.first->interface.end(); b++){
    if(TT.first->sig_list.find(b->first) != TT.first->sig_list.end()){
      ports[b->second] = TT.first->sig_list[b->first];
    }
    else
      cout << "error: signal '" << b->first << "' not declared\n";
  }
  
  for(map<string,int>::iterator iter=TT.first->sig_list.begin(); 
      iter!=TT.first->sig_list.end(); iter++){
    if(iter->second==OUT|ISIGNAL) {
      ports[iter->first]=iter->second;
    }
  }

  if(abstract){
    signals_2_dummies s2d(TT.second, behv, ports);
    TERstructADT tt = s2d.get_result();
    TERstructADT ret_val = TERScompose(behv, tt, "||");

    delete TT.first;
    return ret_val;
  }
  else{
    TERstructADT ret_val = TERScompose(behv, TT.second, "||");
    map<string,string> reverseMap; // Will hold the inverse of the portmap.
    for(map<string,string>::const_iterator iter=TT.first->portmap().begin();
	iter!= TT.first->portmap().end(); iter++){
      reverseMap[iter->second]=iter->first;
    }
    actionsetADT cur_action;
    new_action_table(TT.first->type.c_str());//Use table of entity to be built.
    for (cur_action = ret_val->A; cur_action; cur_action = cur_action->link) {
      if(cur_action->action->type != DUMMY){
	char * label(cur_action->action->label);
	actionADT candidate(action(reverseMap[prefix(label)]+suffix(label)));
	cur_action->action-> type &= ~(IN|OUT|DUMMY);
	if(candidate->type){            // This action existed in the entity.
	  cur_action->action->type |= candidate->type; //Use type from entity.
	}
	else{                           // This action is outside the entity.
	  //if (cur_action->action->type & ISIGNAL==0)
	  //cur_action->action->type |= OUT;//(OUT|ISIGNAL);
	    //else 
	    //cur_action->action->type |= OUT;
	                                // Call it an input.
	  string sig_name(cur_action->action->label, 0, 
			  strlen(cur_action->action->label)-1);

	  if (ports.find(sig_name)==ports.end())
	    cur_action->action->type |= IN;
	  else
	    cur_action->action->type |= OUT;
	}
      }
    }
    return ret_val;
  }
}


bool tels_table::insert(const string& proc_id, TERstructADT t, 
			bool is_env)
{
  if(processes.find(proc_id) != processes.end() ||
     modules.find(proc_id) != modules.end())
    return false;
  
  processes[proc_id] = make_pair(TERScopy(t), is_env);
  return true;
}


bool tels_table::insert(const string& module_id, const tels_table* t)
{
  if(modules.find(module_id) != modules.end() || 
     processes.find(module_id) != processes.end())
    return false;

  modules[module_id] = new tels_table(*t);
  return true;
}


// This function inserts the port map list into the tels_table for a component
// and it replaces the actual signal names in the interface of the components
// contained in this component with the actual signal names in this->interface.
bool tels_table::insert(const map<string, string>& LL)
{
  interface = LL;

  bool updating_state = true;

  for(map<string, tels_table*>::iterator iter = modules.begin();
      iter != modules.end(); iter++){
    updating_state = iter->second->update_port(LL, label);

    if(updating_state==false)
      return false;
  }
 
  return true;
}


const map<string,string>& tels_table::portmap(void) const
{
  return interface;
}

bool tels_table::insert(const map<string,int>& SS)
{
  sig_list = SS;
  return true;
}


map<string,int>& tels_table::port_decl(void)
{
  return sig_list;
}

bool tels_table::insert(const list<pair<string,int> >& SS)
{
  sig_list1 = SS;
  return true;
}  

bool tels_table::add_signals(const string& sig_id, int mode)
{
  sig_list[sig_id]=mode;
  return true;
}

const map<string,int>& tels_table::signals(void)
{
  return sig_list;
}

const list<pair<string,int> >& tels_table::signals1(void)
{
  return sig_list1;
}


// Replace the actual signals in the interface with the actual signal names
// in LL, recursively do this for all components contained in this component.
bool tels_table::update_port(const map<string, string>& LL,
			     const string& top_cmpnt_label)
{
  for(map<string,string>::iterator iter=interface.begin();
      iter !=interface.end(); iter++){

    map<string,string>::const_iterator LL_iter=LL.find(iter->second);

    if(LL_iter==LL.end()){
      if(top_cmpnt_label.size() != 0)
	iter->second= iter->second+'_'+top_cmpnt_label;
    }
    else{
      iter->second=LL_iter->second;
      }
  }

  bool updating_state= true;

  for(map<string, tels_table*>::iterator iter = modules.begin();
      iter != modules.end(); iter++){
    updating_state = iter->second->update_port(interface, top_cmpnt_label);

    if(updating_state==false)
      return false;
  }

  return true;
}


tels_table* tels_table::instantiate(my_list *l, const string& scope, 
				    const string& component_name, 
				    bool csp)
{
  map<string,int> new_list;

  for(map<string, int>::iterator iter= sig_list.begin(); iter!=sig_list.end();
      iter++)
    if(l->port_list().find(iter->first)!=l->port_list().end())
      new_list[iter->first] = iter->second;
    else
      new_list[iter->first+"_"+scope] = iter->second;

  sig_list = new_list;

  for(map<string,pair<TERstructADT, bool> >::iterator b = processes.begin();
      b != processes.end(); b++) {
    TERstructADT TT = b->second.first;
    string nS = scope + '.' + b->first;
    if(b->second.second == false){
      char *comp_id = copy_str(component_name.c_str());
      char *scp_id = copy_str(scope.c_str());
      processes[b->first] = make_pair(::instantiate(TT,l,scp_id,comp_id,csp),
				      b->second.second);
      delete [] comp_id;  delete [] scp_id; 
    }
    else
      processes[b->first] = make_pair((telADT)0, b->second.second);
    TERSdelete(TT);
  }

  for(map<string, tels_table*>::iterator b = modules.begin();
      b != modules.end(); b++)
    b->second->instantiate(l, scope.c_str(), component_name.c_str(), csp);
    
  return this;
}


TERstructADT tels_table::compose()
{
  TERstructADT ret_val = 0;
  
  for(map<string, pair<TERstructADT, bool> >::iterator b = processes.begin();
      b != processes.end(); b++)
    ret_val = TERScompose(ret_val, TERScopy(b->second.first), "||");
  
  for(map<string, tels_table*>::iterator b = modules.begin();
      b != modules.end(); b++){
#ifdef DDEBUG
    assert(b->second);
#endif
    ret_val = TERScompose(ret_val, b->second->compose(), "||");
  }
  return ret_val;
}


// The format of 'SS' could be 'm_1/m_2/.../m_k', where m_1, m_2,..., m_k could
// be either module names or module instance ID. 
// If it's a component name, then the search for the component will stop 
// at the first instance with given component. 
pair<tels_table*,TERstructADT> tels_table::compose(const string& SS)
{
  pair<tels_table*, TERstructADT> ret_val(0, 0);

  // Arguement 'SS' is splitted with '/' as the delimiter.
  // If c_id.second is an empty string, then c_id.first is the component 
  // expected. If c_id.second is not empty, search will continue to lower 
  // modules.
  pair<string,string> c_id = split(SS);
  for(map<string, pair<TERstructADT, bool> >::iterator b = processes.begin();
      b != processes.end(); b++){
   
    if(c_id.first != b->first)
      ret_val.second = TERScompose(ret_val.second, 
				   TERScopy(b->second.first), "||");
    else {
      ret_val.first = new tels_table;
      ret_val.first->insert(b->first, b->second.first);
      for(actionsetADT CA = b->second.first->A; CA; CA = CA->link) 
	if(CA->action->type == IN || CA->action->type==OUT){
	  string sid(CA->action->label, strlen(CA->action->label)-1);
	  ret_val.first->interface.insert(make_pair(sid, sid));
	  ret_val.first->sig_list.insert(make_pair(sid, CA->action->type));
	  ret_val.first->sig_list1.push_back(make_pair(sid, CA->action->type));
	}
    }
  }
  
  bool cmp_found=false;

  for(map<string, tels_table*>::iterator b = modules.begin();
      b != modules.end(); b++){
#ifdef DDEBUG
    assert(b->second);
#endif
    if(cmp_found || (b->first!=c_id.first && b->second->ty_id()!=c_id.first))
      ret_val.second = TERScompose(ret_val.second, b->second->compose(), "||");
    else{
      if(c_id.second.empty()){
	ret_val.first = new tels_table(*(b->second));
	cmp_found=true;
      }
      else{
	pair<tels_table*,TERstructADT> LTT = 
	  b->second->compose(c_id.second);
	ret_val.first = LTT.first;
	ret_val.second = TERScompose(ret_val.second, LTT.second, "||");
      }
    }
  }
  return ret_val;
}


bool tels_table::empty(void) const
{
  return (label.empty() && processes.empty() && modules.empty() && 
	  interface.empty() && sig_list.empty() && sig_list1.empty());
}


tels_table tels_table::find(const string& cmp_id)
{
  pair<string,string> tmp = split(cmp_id);
  string top, lower;
  if(tmp.second.empty())
    top = tmp.first;
  else{
    top = tmp.first;
    lower = tmp.second;
  }
 
  // If the 'lower' is empty, then 'top' is the name of a process/component;
  // otherwise, 'top' is the name of a component containing target.
  if(lower.empty()){
    for(map<string, pair<TERstructADT, bool> >::iterator b = processes.begin();
	b != processes.end(); b++){
      if(top == b->first)
	return tels_table();
    }
  }
  
  for(map<string, tels_table*>::iterator b = modules.begin();
      b != modules.end(); b++){
#ifdef DDEBUG
    assert(b->second);
#endif
    if(b->first == top || b->second->ty_id() == top)
      if(lower.empty())
	return (*(b->second));
      else
	return b->second->find(lower);
  }
  return tels_table();
}



void tels_table::print_io(void)
{
  cout << label<< ": ";

  for(list<pair<string,int> >::iterator iterr=sig_list1.begin();
      iterr != sig_list1.end(); iterr++){
    cout << iterr->first << " : ";
    if(iterr->second==1)
      cout<< "in, ";
    if(iterr->second==2)
      cout<< "out, ";
  }

  cout << endl;
  for(map<string,string>::iterator iterr=interface.begin();
      iterr != interface.end(); iterr++)
    cout << '(' << iterr->first << ", " << iterr->second << "),";

  cout << endl;

  cout << "Modules inside >>>>>>>>>>>>>>>>>"<< endl;

  for(map<string, tels_table*>::const_iterator iter = modules.begin();
      iter != modules.end(); iter++){

    cout << iter->second->label<< ": ";

    for(list<pair<string,int> >::iterator iterr=iter->second->sig_list1.begin()\
	  ;
        iterr != iter->second->sig_list1.end(); iterr++){
      cout << iterr->first << " : ";
      if(iterr->second==1)
        cout<< "in, ";
      if(iterr->second==2)
        cout<< "out, ";
    }

    cout << endl;
    for(map<string,string>::iterator iterr=iter->second->interface.begin();
        iterr != iter->second->interface.end(); iterr++)
      cout << '(' << iterr->first << ", " << iterr->second << "),";

    cout << endl;
  }
  cout << "--------------------------------------------------------\n";
}



tels_table& tels_table::operator=(const tels_table& TT)
{
  if(this != &TT)
  {
    for(const_iterator b = TT.processes.begin(); b != TT.processes.end(); b++)
    {
      TERSdelete(processes[b->first].first);
      processes[b->first]=make_pair(TERScopy(b->second.first),
				    b->second.second);
    }

    for(map<string, tels_table*>::const_iterator b = TT.modules.begin();
	b != TT.modules.end(); b++)
    {
      delete modules[b->first];
      modules[b->first] = new tels_table(*(b->second));
    }
    label = TT.label;
    interface = TT.interface;
    sig_list = TT.sig_list;
    sig_list1 = TT.sig_list1;
  }
  return *this;
}

ostream& operator<<(ostream& os, const tels_table& TT)
{
  os << TT.label<< ":\n";
  os << "  processes:\n";
  for(tels_table::const_iterator b=TT.processes.begin();
      b!=TT.processes.end(); b++)
    os << "\t" << b->first << ";\n";
  os << "  modules:\n";
  for(map<string, tels_table*>::const_iterator b = TT.modules.begin();
      b != TT.modules.end(); b++)
    {
      os << "\t" << b->first << ";\n";
      //os << *(b->second)<< endl;
    }
  os << endl;

  return os;
}

tels_env_tb::tels_env_tb()
{}


tels_env_tb::tels_env_tb(const tels_table* TT)
{
  //modules = TT->modules;
}


tels_env_tb::~tels_env_tb()
{}
