// base_type delcaration for all concrete types.
// Author: Hao Zheng
// Date:   11/9/1998

#ifndef _TELS_TABLE_H_
#define _TELS_TABLE_H_

#include <map>
#include <set>
#include <list>
#include "ly.h"
#include "auxilary.h"
#include "create_env.h"
#include <string>

typedef map<string, int> m_str_i;
typedef pair<m_str_i, TERstructADT> msi_tel;

// Each tels table carries some modules and some process.
class tels_table
{
public:

  tels_table();
  tels_table(const tels_table&);
  tels_table(const tels_table*);
  virtual ~tels_table();

  void set_id(const string&);
  void set_type(const string& ty) { type = ty; } 
  string id(void);
  string ty_id(void) { return type; }
  void update(const string &);

  TERstructADT tel(const string&, bool);

  bool insert(const string&, TERstructADT, // store the process id and its tel 
	      bool is_env = false);        // into ' processes'.
  bool insert(const string&, const tels_table*); 
                                           // store the module id and its tel 
                                           // table into ' modules'.
  bool insert(const map<string, string>&);
  const map<string,string>& portmap(void) const;
  bool insert(const map<string, int>&);
  map<string,int>& port_decl(void);
  bool insert(const list<pair<string, int> >&);
  bool add_signals(const string&, int);

  const map<string,int>& signals(void);
  const list<pair<string,int> >& signals1(void);

  bool update_port(const map<string,string>&, const string&);

  tels_table* instantiate(my_list *l, 
			  const string& scope, 
			  const string& component_name, bool csp = false);

  TERstructADT compose(); 
  pair<tels_table*,TERstructADT> compose(const string&);

  bool empty(void) const;

  tels_table find(const string&);
  void print_io(void);
  

  tels_table& operator=(const tels_table&);

  typedef map<string, pair<TERstructADT,bool> >::iterator iterator;
  typedef map<string, pair<TERstructADT,bool> >::const_iterator const_iterator;
  typedef map<string,string>::iterator s_iterator;
  typedef map<string,string>::const_iterator cs_iterator;

  friend ostream& operator<<(ostream&,const tels_table&); 

private:
  

protected:

  string label, type;
  map<string, pair<TERstructADT, bool> > processes;  
  map<string, tels_table*>  modules;
  // port map: <formal, actual>
  map<string, string> interface;
  map<string, int>  sig_list;
  list<pair<string,int> > sig_list1;
};


//ostream& operator<<(ostream&,tels_table&); 

class tels_env_tb : public tels_table
{
public: 
  
  tels_env_tb();
  tels_env_tb(const tels_table*);
  ~tels_env_tb();

  void create_env();
  void result();

private:
  
  void find_ports();
  

private:

  map<string, TERstructADT> envs;
};
#endif
