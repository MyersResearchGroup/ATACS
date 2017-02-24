/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1993 by, Chris J. Myers                                   */
/*   Center for Integrated Systems,                                          */
/*   Stanford University                                                     */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of Stanford University may not be used in advertising or   */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Stanford.                         */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL STANFORD OR THE AUTHORS OF THIS SOFTWARE BE LIABLE    */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* storevhd.c                                                                */
/*****************************************************************************/


#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include "misclib.h"
#include "auxilary.h"
#include "storeprs.h"
#include "storevhd.h"
#include "bcp.h"
#include <string>

extern set<string> top_port;
extern map<string,string> top_port_map;
const unsigned int MAX_PORT_MAP_LINE = 75;

static int min_gate_delay, max_gate_delay;

int is_input(string sig_id, signalADT *signals, int ninpsig)
{
  for(int i = 0; i<ninpsig; i++)
    if(sig_id == signals[i]->name)
      return 1;
  return 0;
}

string get_header(const string& gate_id,string port_decl)
{
  stringstream header;
  header << "library ieee;"<< endl
	 << "use ieee.std_logic_1164.all;"<< endl
    //<< "use work.nondeterminism.all;"<< endl
	 << endl
	 << "entity "<< gate_id << " is"<< endl
	 << port_decl <<endl  
         << "end " <<  gate_id << ";"<<endl << endl;// << ends;

  return header.str();
}

string get_header(const string& gate_id, 
		  const map<string,string>& sig_vec,
		  const map<string,string>& sig_vec_md)
{
  stringstream header;
  header << "library ieee;"<< endl
	 << "use ieee.std_logic_1164.all;"<< endl
    //<< "use work.nondeterminism.all;"<< endl
	 << endl
	 << "entity "<< gate_id << " is"<< endl;

  map<string,int> port_decl;
  map<string,string> port_decl_md(sig_vec_md);

  unsigned int max_id_len = 0;
  for(map<string,string>::const_iterator iter=sig_vec.begin();
      iter!=sig_vec.end(); iter++){
    if(iter->second.size() > max_id_len)
      max_id_len =iter->second.size();
 
    if(port_decl.find(iter->second)!=port_decl.end())
      port_decl[iter->second]++;
    else
      port_decl[iter->second]=0;
  }

  stringstream spd;
  bool first_time=true;
  bool is_vec=false;
  
  
  
  map<string,int>::iterator iter=port_decl.begin();
  
  while(iter!=port_decl.end()){
    string tmp, tmp_sig;
    
    if(iter->second){
      tmp_sig=iter->first;
      is_vec=true;
    }
    else{
      tmp_sig=iter->first;
      is_vec=false;
    }

    int width=9+max_id_len;

    if(first_time){
      tmp=string("  port(")+string(width-9-tmp_sig.size(),' ')+tmp_sig+" :";
      first_time = false;
    }
    else
      tmp=string(width-2-tmp_sig.size(),' ')+tmp_sig+string(" :");
    
    spd<<tmp;
    spd<<" "<<port_decl_md[iter->first];
    if(is_vec)
      spd<<" std_logic_vector("+itos(iter->second)+" downto 0)";
    else
      spd<<" std_logic";
    
    iter++;
    if(iter!=port_decl.end())
      spd<< ';'<< endl;
  }
  spd<< endl<<"       "<<");";//<<ends;

  header<< spd.str()<< endl  
        << "end " <<  gate_id << ";"<<endl << endl;// << ends;

  return header.str();
}

string get_header(const string& gate_id, signalADT *signals,
		  int ninpsig, int nsignals)
{
  unsigned int max_id_len = 0;
  for(int i=0; i<nsignals; i++)
    if(string(signals[i]->name).size() > max_id_len)
      max_id_len = string(signals[i]->name).size();
  
  stringstream port_decl;
  bool first_time=true;
  for(int i=0; i<nsignals; i++){
    string tmp;
    if(first_time){
      tmp=string("  port(")+signals[i]->name+" :";
      first_time = false;
    }
    else
      tmp=signals[i]->name+string(" :");

    if(i<ninpsig)     
      port_decl<<tmp;
    else
      port_decl<<tmp;
    port_decl << " out std_logic";

    int count = i+1;
    if(count<nsignals)
      port_decl<<';'<< endl;
  }
  port_decl<< "       "<<");";//<<ends;
  
  stringstream header;
  header << "library ieee, packages;"<< endl
	 << "use ieee.rstd_logic_1164.all;"<< endl
	 << "use packages.nondeterminism.all;"<< endl<< endl
	 << "entity "<< gate_id << " is"<< endl 
	 << port_decl.str() << endl
	 << "end " <<  gate_id << ";" 
         <<endl << endl;//<< ends;
  
  return header.str();
}


void to_vec(signalADT *signals,int ninpsig,int nsignals,
	    map<string,string>& port_decl,
	    map<string,string>& port_decl_md,
	    map<string,string>& slb)
{
  for(int iter=0; iter<nsignals; iter++){
    string tmp_str(signals[iter]->name);
    /*if(isdigit(tmp_str[tmp_str.size()-1])){
      string new_str = string(tmp_str,0, tmp_str.size()-1);
      port_decl[tmp_str] = new_str;      
      slb[tmp_str]=new_str+'('+tmp_str[tmp_str.size()-1]+')';
      if(iter<ninpsig)
	port_decl_md[new_str]="in";
      else
	port_decl_md[new_str]="inout";
    }
    else*/{
      port_decl[tmp_str] = tmp_str;
      slb[tmp_str]=tmp_str;
      if(iter<ninpsig)
	port_decl_md[tmp_str]="in";
      else
	port_decl_md[tmp_str]="inout";
    }    
  }
}

/***************************************************************************/
//Declaration part//
//-------------------------------------------------------------------------//
extern ostringstream entity_decl; //From parser.l.

/***************************************************************************/


int comp::isSignal(char *s)
{
  for( ; *s != '\0'; s++)
    if(*s == '@')
      return 1;

  return 0;
}


/***************************************************************************/

port::port()
{ index =0; }


port::port(const port &t)
{
  names = t.names;
  signals = t.signals;
  modes = t.modes;
  stack_ty = t.stack_ty;
  index = t.index;
  ports = t.ports;  
}


port::~port()
{}


/* Definitions for operations of port. */
string port::getName(int i) 
{ 
  if(i>-1 && i<index)
    return names[i].c_str();
  return string();
}

void port::add(char *name, char *md, const string& stk_ty,char *ty)
{
  for(map<int,string>::iterator i = names.begin(); i!=names.end(); i++)
    if(i->second == name)
      return;
  
  names[index]=name; 

  ostringstream outs;
  outs<< rmsuffix(name)<< " : "<< md<< "   "<<ty;//<< ends;
  signals[index]=outs.str();
  
  modes[index]=Tolower(md);
  stack_ty[index]=stk_ty;
  index++;
}



string port::output()
{
  stringstream outs, ins_n, ins_p;
  bool first_time = true;
  for(int i=0; i< index; i++){
    if(modes[i]=="in"){
      if(first_time){
	if(stack_ty[i]=="n")
	  ins_n << signals[i]<< ";"<< endl;
	else if(stack_ty[i]=="p")
	  ins_p << signals[i]<< ";"<< endl; 
	first_time = false;
      }
      else{
	if(stack_ty[i]=="n")
	  ins_n << "         "<<signals[i]<< ";"<< endl;
	else if(stack_ty[i]=="p")
	  ins_p << "         "<<signals[i]<< ";"<< endl; 
      }
    }
    else if(modes[i]=="out")
      outs<< "       "<< signals[i]<< ";"<< endl;
  }
  
  //outs<< ends;
  //ins_p << ends;
  ins_n << ins_p.str() << outs.str();// << ends;
  string tmp_str1(ins_n.str());  // to get rid of the last ';'.
  string tmp_str(tmp_str1, 0, tmp_str1.size()-2);
  return tmp_str;
}

string port::output_portmap(const map<string,string>& i_sig)
{
  stringstream outs;
  
  map<string,string>::iterator i = ports.begin();
  string line_o, indent(13, ' ');
  bool first_time=true;
  while(i!=ports.end()){
    string tmp_str;
    tmp_str = i->first + "=>" + i->second;

    i++;
    if(i != ports.end())
      tmp_str += ", ";
    
    if((line_o.size()+tmp_str.size())<MAX_PORT_MAP_LINE){
      if(first_time){
	line_o += string("    port map(")+tmp_str;
	first_time=false;
      }
      else
	line_o += tmp_str;
    }
    else{
      outs << line_o << endl;
      line_o = indent+tmp_str;
    }
  }
  
  outs << line_o << ");";//<< ends;
  return outs.str();
}

int port::isSignal(char *s, char *m)
{
  for(int i=0; i<index; i++)
    if(names[i]==s && modes[i]==m)
      return 1;
  
  return 0;
}


void port::print()
{}

/****************************************************************************/

comp::comp()
{
  for(int i=0; i< 500; i++){
    //label[i]= NULL;
    names[i]= NULL;
    signals[i]= NULL;
    //pins[i]= NULL;
    //attr[i]= NULL;
  }
  count= 0;
  inputs= total= 0;
}


comp::~comp()
{
  for(int i=0; i< 500; i++){
    delete label[i];
    delete names[i];
    delete signals[i];
  }
  count= 0;
}


void comp::set(const char *comp_id, port *p)
{
  if(!names[count]){
    string inst_id="i_"+itos(count);
    label[count]=copy_str(inst_id.c_str());
    names[count]= copy_str(comp_id);
    signals[count]= new port(*p);
  }      
  else if(!strcmp(names[count], comp_id))
    signals[count]= new port(*p);

  count++;
}

void comp::set_sig_tb(const map<string,string>& a_ports)
{
  sig_tb = a_ports;
}

void comp::add_pins(const string& id, const string& md)
{
  ports[id] = md;
}


string comp::output(char *s)
{
  for(int i=0; i< 500; i++)
    if(!strcmp(names[i], s))
      return signals[i]->output();

  return string();
}


void comp::decideport()
{
  /* for(int i=0; i<total; i++)
    if(isSignal(pins[i]))
    strcpy(attr[i], "signal");*/
  return;
}


string comp::output()
{
  if(!total)
    return string();

  stringstream outs, ins;

  for(map<string,string>::iterator i=ports.begin(); i!=ports.end(); i++)
    if(i->second == "in")
      ins<< "          "<< i->first << " : In   std_logic;"<< endl; 
    else
      outs<< "          "<< i->first << " : Out   std_logic;"<< endl;

  //outs<< ends;
  ins << outs.str();//<< ends;
  return ins.str();
}


void comp::isSignal()
{/*
  for(int i=0; i< total; i++){
    if(strcmp(attr[i], "signal"))
      for(int k=0; k<count; k++)
	if(signals[k]->isSignal(pins[i], attr[i])){
	  strcat(pins[i], "_DUMMY");
	  strcpy(attr[i], "signal");
	  break;
	}
	}*/
}


string comp::port_map(int i, const map<string,string>& bar_sig)
{
  return signals[i]->output_portmap(bar_sig);
}

const map<string,string>& comp::get_portmap(int index)
{
  return signals[index]->get_portmap();
}

char *comp::lookup(const char *s)
{
  // for(int k=0; k<total; k++)
    //if(!strcmp(pins[k], s)){
    //char *t= new char[strlen(rmsuffix(s))+1];
    //return strcpy(t, rmsuffix(s));
    //}

    /*char *t= new char[strlen(s)+7];
  strcpy(t, s);
  strcat(t, "_DUMMY");
  for(int k=0; k<total; k++)
    if(!strcmp(pins[k], t)){
      if(isSignal(t)){
	char *t= new char[strlen(rmsuffix(t))+7];
        strcpy(t, rmsuffix(t)); 
	return strcat(t, "_DUMMY");
      }
      else
	return t;
    }
    */
  return NULL;
}


char *comp::sigdecl()
{
  /*ostringstream outs;
  for(int i=0; i<total; i++)
    if(!strcmp(attr[i], "signal"))
    outs<< "\tsignal     "<< rmsuffix(pins[i])<< " : std_logic;"<<endl;
  
  outs<< ends;
  return outs.str();*/
  return NULL;
}



char *comp::sigass(char *signals[], int num)
{
  /*ostringstream outs;
  if(num != total)
    cout<< "ERROR: the number of signals is not correct"<< endl;
  else{
    for(int i=0; i<total; i++){
      if(strcmp(signals[i], pins[i]))
	outs<< "\t"<< signals[i]<< " <= "<< pins[i]<< ";"<< endl;
    }    
  }
  
  outs<< ends;
  return outs.str();*/
  return NULL;
}

//******************************
// Definition for basic_comp.

basic_comp::basic_comp()
{}

basic_comp::~basic_comp()
{}
  
void basic_comp::set_stk(const mstrstr& a_stk, char ty)
{  
  if(ty=='n')
    n_stk.push_back(make_pair(a_stk, a_stk.size()));
  else if(ty=='p')
    p_stk.push_back(make_pair(a_stk, a_stk.size()));
}


void basic_comp::set_output(const string& a_o, char which)
{
  if(which=='o')
    o=a_o;
  else
    obar=a_o;      
}

// Return a string which is the name of this gate. This name is derived from
// its number of 'p' and 'n' stacks and size of each stack. 
string basic_comp::type(void)
{
  multiset<int> n_ty, p_ty;
  
  for(list<pair<mstrstr, int> >::iterator iter=n_stk.begin(); 
      iter!=n_stk.end();iter++)
    n_ty.insert(iter->second);

  for(list<pair<mstrstr, int> >::iterator iter=p_stk.begin(); 
      iter!=p_stk.end();iter++)
    p_ty.insert(iter->second);

  string ret_str;
  
  for(multiset<int>::reverse_iterator riter=n_ty.rbegin();riter!=n_ty.rend();
      riter++)
    if(ty==0)
      ret_str += "n"+itos(*riter);
    else if(ty==1)
      ret_str += "a"+itos(*riter);
    else
      ret_str += "i"+itos(*riter);
  
  for(multiset<int>::reverse_iterator riter=p_ty.rbegin();riter!=p_ty.rend();
      riter++)
    if(ty==0)
      ret_str += "p"+itos(*riter);
  
  if(initial=='1')
    return ret_str+'h';
  return ret_str;
  
}


string basic_comp::output_portmap(void)
{
  multimap<int,mstrstr*> n_stack, p_stack;

  for(list<pair<mstrstr, int> >::iterator iter=n_stk.begin(); 
      iter!=n_stk.end();iter++)
    n_stack.insert(make_pair(iter->second, &(iter->first)));

  for(list<pair<mstrstr, int> >::iterator iter=p_stk.begin(); 
      iter!=p_stk.end();iter++)
    p_stack.insert(make_pair(iter->second, &(iter->first)));

  string rets("    port map(");
                           
  int cnt=1;
  for(multimap<int,mstrstr*>::reverse_iterator riter=n_stack.rbegin();
      riter!=n_stack.rend(); riter++, cnt++){
    for(mstrstr::iterator iter=riter->second->begin();
	iter!=riter->second->end(); iter++)
      if(ty==0)
	rets += iter->first+itos(cnt)+"n=>"+top_port_map[iter->second]+", ";
      else
	rets += iter->first+itos(cnt)+"=>"+top_port_map[iter->second]+", ";
    rets += "\n             ";
  }  

  cnt = 1;
  for(multimap<int,mstrstr*>::reverse_iterator riter=p_stack.rbegin();
      riter!=p_stack.rend(); riter++, cnt++){
    for(mstrstr::iterator iter=riter->second->begin();
	iter!=riter->second->end(); iter++)
      if(ty==0)
	rets += iter->first+itos(cnt)+"p=>"+top_port_map[iter->second]+", ";
      else
	rets += iter->first+itos(cnt)+"=>"+top_port_map[iter->second]+", ";
    rets += "\n             ";
  }
  rets += "o=>"+top_port_map[o];
  if(ty==0)
    rets += ", obar=>"+top_port_map[obar]+");"; 
  else
    rets += ");";

  return rets;
}


string basic_comp::output_port(bool comp)
{
  multiset<int> n_ty, p_ty;
  
  for(list<pair<mstrstr, int> >::iterator iter=n_stk.begin(); 
      iter!=n_stk.end();iter++)
    n_ty.insert(iter->second);

  for(list<pair<mstrstr, int> >::iterator iter=p_stk.begin(); 
      iter!=p_stk.end();iter++)
    p_ty.insert(iter->second);

  int cnt=1;
  string rets("  port(");
  bool first_time=true;
  for(multiset<int>::reverse_iterator riter=n_ty.rbegin();riter!=n_ty.rend();
      riter++, ++cnt){
    char sig_id='a';
    for(int i = 0; i<(*riter); i++, sig_id++){
      if(first_time){
	if(ty==0)
	  rets += itos(sig_id)+itos(cnt)+"n : in std_logic;\n";
	else
	  rets += itos(sig_id)+itos(cnt)+" : in std_logic;\n";
	first_time=false;
      }
      else{
	if(ty==0)
	  rets += "       "+itos(sig_id)+itos(cnt)+"n : in std_logic;\n";
	else
	  rets += "       "+itos(sig_id)+itos(cnt)+" : in std_logic;\n";
      }
    }
  }
  
  cnt = 1;
  for(multiset<int>::reverse_iterator riter=p_ty.rbegin();riter!=p_ty.rend();
      riter++, ++cnt){
    char sig_id='a';
    for(int i = 0; i<(*riter); i++, sig_id++)
      if(ty==0)
	rets += "       "+itos(sig_id)+itos(cnt)+"p : in std_logic;\n";
      else
	rets += "       "+itos(sig_id)+itos(cnt)+" : in std_logic;\n";
  }

  
  if(ty==0){
    if(!comp){
      rets += "       o : inout std_logic :='"+itos(initial)+"'";
      if(initial=='1')
	rets += ";\n       obar : inout std_logic := '0');";
      else
	rets += ";\n       obar : inout std_logic := '1');";
    }
    else{
      rets += "       o : inout std_logic";
      rets += ";\n       obar : inout std_logic);";
    }
  }
  else{
    if(!comp)
      rets += "       o : inout std_logic :='"+itos(initial)+"');";
    else
      rets += "       o : inout std_logic);";
  }
    
  return rets;
}


string basic_comp::output_stmt(void)
{
  multiset<int> n_ty, p_ty;
  
  for(list<pair<mstrstr, int> >::iterator iter=n_stk.begin(); 
      iter!=n_stk.end();iter++)
    n_ty.insert(iter->second);

  for(list<pair<mstrstr, int> >::iterator iter=p_stk.begin(); 
      iter!=p_stk.end();iter++)
    p_ty.insert(iter->second);

  if(ty==0){
    int cnt=1;
    //string retp("  o_i <= '1' after delay(");
    //retp += itos(lr)+','+itos(ur)+") when ";
    string retp("  o_i <= '1' after ");
    retp += itos(lr)+" ns when ";
    
    for(multiset<int>::reverse_iterator riter=n_ty.rbegin();riter!=n_ty.rend();
	riter++, cnt++){
      char sig_id='a';
      string tmp_str;
      tmp_str += '(';
      for(int i = 0; i<(*riter); i++, sig_id++)
	tmp_str += itos(sig_id)+itos(cnt)+"n='1' and ";
      retp += string(tmp_str, 0, tmp_str.size()-5)+") or ";
    }
    
    string rets(retp,0,retp.size()-3);
    //rets += "else\n         '0' after delay(";
    //rets += itos(lf)+','+itos(uf)+") when ";
    rets += "else\n         '0' after ";
    rets += itos(lf)+" ns when ";
    
    cnt = 1;
    for(multiset<int>::reverse_iterator riter=p_ty.rbegin();riter!=p_ty.rend();
	riter++, ++cnt){
      char sig_id='a';
      string tmp_str("(");
      for(int i = 0; i<(*riter); i++, sig_id++)
	tmp_str += itos(sig_id)+itos(cnt)+"p='0' and ";
      rets += string(tmp_str, 0, tmp_str.size()-5)+") or ";
    }

    string tmp_str2(rets,0,rets.size()-3);
    tmp_str2 += "else\n          o_i;\n";
    tmp_str2 += "  o <= o_i;\n";
    //tmp_str2 += "  obar <= not o_i after delay("+itos(min_gate_delay)+','
    //  +itos(max_gate_delay)+");\n";
    tmp_str2 += "  obar <= not o_i after "+itos(min_gate_delay)+" ns;\n";
    return tmp_str2;
  }
  else{
    int cnt=1;
    string tmp_str("  o <= ");
        
    for(multiset<int>::reverse_iterator riter=n_ty.rbegin();riter!=n_ty.rend();
	riter++, cnt++){
      char sig_id='a';
      tmp_str += '(';
      for(int i = 0; i<(*riter); i++, sig_id++){
	tmp_str += itos(sig_id)+itos(cnt);
	if((i+1)<(*riter))
	  tmp_str += " and ";
      }
      multiset<int>::reverse_iterator tmp_iter=riter;
      tmp_iter++;
      if(tmp_iter!=n_ty.rend())
	tmp_str += ") or ";
      else
	tmp_str += ") ";
    }
    //tmp_str += string("after delay(")+itos(lr)+", "+itos(ur)+");\n";
    tmp_str += string("after ")+itos(lr)+" ns;\n";
    
    return tmp_str;
  }
}


void basic_comp::set_delay(int a_lr, int a_ur, int a_lf, int a_uf)
{
  lr = a_lr;
  ur = a_ur;
  lf = a_lf;
  uf = a_uf;
}

ostream &operator<<(ostream &os, comp *p)
{
  for(int i=0; i< p->count; i++)
    if(p->names[i])
      os<<p->names[i]<< endl;
  
  return os;
}

char *find_cover(regionADT region,int i,bool exact)
{
  int k;
  regionADT cur_region;

  k=(-1);
  for (cur_region=region;cur_region;cur_region=cur_region->link) {
    if (cur_region->cover[0]!='E') k++;
    if (i==k) return cur_region->cover;
  }
  return NULL;
}

/*****************************************************************************/
/* Store production rules.                                                   */
/*****************************************************************************/

void store_vhd(string filename,signalADT *signals,eventADT *events,
	       stateADT *state_table,
	       regionADT *regions,int ninpsig,int nsignals,int maxsize,
	       bool gatelevel,bool combo,bool exact,const string& startstate)
{
  string outname;
  int i,tempi,k,m,n,o,p;
  int *fanout;
  int area;
  bool firstOR,firstAND;
  
  /* New variables - CJM */
  int j;
  int sands,rands;
  regionADT cur_region;
  char *cur_cover;

  /* CJM - added to clear top so ports will be based on signals */
  //top_port=set<string>(); 
  for (i=0;i<nsignals;i++)
    if (!(events[signals[i]->event]->type & ISIGNAL)) {
      //      top_port.insert(string(signals[i]->name));
      top_port_map.insert(make_pair(string(signals[i]->name)+"_bar",
				    top_port_map[signals[i]->name]+"_bar"));
    } else {
      string new_string=string(signals[i]->name);
      if (new_string.find("_"+Tolower(filename))!=string::npos) {
	new_string.replace(new_string.find("_"+Tolower(filename)),
			   1+filename.size(),"");
      }
      top_port_map.insert(make_pair(string(signals[i]->name),
				      new_string));
      top_port_map.insert(make_pair(string(signals[i]->name)+"_bar",
				    new_string+"_bar"));
	/*} else {
	top_port.insert(string(signals[i]->name));
	top_port_map.insert(make_pair(string(signals[i]->name)+"_bar",
				      top_port_map[signals[i]->name]+"_bar"));
				      }*/
    }
  outname = filename + "S.vhd";
  cout<< "Generating structural VHDL for net: "<<outname << endl;
  fprintf(lg,"Generating structural VHDL for net:  %s\n",outname.c_str());

  ofstream fout(outname.c_str()); //fp=fopen(outname,"w");
  if(!fout){
    cout<< "WARNING:: cannot open file "<< outname<< endl;
    return;
  }
  area=0;

  /* ADD PREAMBLE FOR VHDL FILE */
  /* Circuit name is: filename  */

  k=0;
  m=0;
  n=0;
  o=0;
  p=(nsignals-ninpsig+1);

  fanout=(int*)GetBlock((2*nsignals+1)*sizeof(int));
  for (i=0;i<(2*nsignals+1);i++) 
    fanout[i]=0;

  map<string,string> sig_decl_tb, port_sig;
  map<string,string> sig_vec, sig_vec_md, slb;
  list<basic_comp*> component_inst;
  map<string,string> component_decl;
  
  to_vec(signals, ninpsig, nsignals, sig_vec, sig_vec_md, slb); 

  for(int iter=0; iter<nsignals; iter++)
    if(iter<ninpsig)
      port_sig[slb[signals[iter]->name]]="in";
    else
      port_sig[slb[signals[iter]->name]]="out";

  /* Replaced this

  for(i=2*ninpsig; i<(2*nsignals+1); i++){

     with this */

  for (i=2*ninpsig+1;i<(2*nsignals+1);i+=2) {

    sands=0;
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link) {
      if (cur_region->cover[0]!='E') {
	sands++;
      }
    }
    rands=0;
    for (cur_region=regions[i+1];cur_region;cur_region=cur_region->link) {
      if (cur_region->cover[0]!='E') {
	rands++;
      }
    }

    if(i % 2==1){
    /* GATE ENTITY DECLARATION AND TOP OF ARCHITECTURE */
    /* Signal name is signals[(i-1)/2]->name */
    /* add function output port map */

      // Indicate which type of gate which is going to be generated.
      // 0: gc_element; 
      //1: combnational gate 1; 
      //2: combnational gate 2
      int gate_ty; 

      multiset<string> gate_id_n, gate_id_p;
      stringstream entity, arch, sq_stmt;

      basic_comp *new_cmp = new basic_comp;

      if(top_port.find(signals[(i-1)/2]->name)==top_port.end())
	sig_decl_tb[signals[(i-1)/2]->name]='4';

      /* Replaced this 

      if (!comb[(i-1)/2] && !comb[(i-1)/2+nsignals]) {
         with this */

      // The gate is a C-element.
      if (sands > 0 && rands > 0) {
	gate_ty=0;

	//	sq_stmt<< "begin"<< endl
	//     << "  "<< "o_i <= '1' after delay("
	//     << signals[(i-1)/2]->riselower<<", "
	//     << signals[(i-1)/2]->riseupper
	//     <<") when ";
	sq_stmt<< "begin"<< endl
	       << "  "<< "o_i <= '1' after "
	       << signals[(i-1)/2]->riselower << " ns when ";
	new_cmp->set_ty(0);
	new_cmp->set_output(signals[(i-1)/2]->name, 'o');
	new_cmp->set_delay(signals[(i-1)/2]->riselower, 
			   signals[(i-1)/2]->riseupper,
			   signals[(i-1)/2]->falllower,
			   signals[(i-1)/2]->fallupper);
	new_cmp->set_init(startstate[(i-1)/2]);
		
	firstOR=TRUE;
	int or_n_loop = 1;
	j=0;
	while ((cur_cover=find_cover(regions[i],j,exact))) {
	  j++;
        /* To here -- CJM */
	  
	  mstrstr new_stk;

	  if (!firstOR) /*Output OR */ 
	    sq_stmt<< " or ";

	  firstOR=FALSE;
	  firstAND=TRUE;
	  int n_and=1; char sig_id = 'a';
	  string andstack;
	  for(k=0;k<nsignals;k++){
	    
	    /* Replaced this
	       if(cur_region->cover[k]=='1') {
	       with this */
	    if(cur_cover[k]=='1') {
	      /* To here -- CJM */
	      
	      if(!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      /* Output signals[k]->name = '1' */
	      andstack += sig_id + itos(or_n_loop) + "n='1'";
	      stringstream new_sid;
	      new_sid << sig_id<<or_n_loop<<'n';//<< ends;
	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		//sig_decl_tb[signals[k]->name+string("_i")]="2";
		new_stk[itos(sig_id)] = signals[k]->name;//+string("_i");
	      }
	      else{
		new_stk[itos(sig_id)] = slb[signals[k]->name];
	      }
	      n_and++; sig_id++; 
	    } 
	    
	    /* Replaced this
	       else if (cur_region->cover[k]=='0') {
	       with this */
	    else if(cur_cover[k]=='0') {
	      /* To here -- CJM */
	      
	      if (!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      
	      /* Output signals[k]->name = '0' */;
	      andstack += sig_id + itos(or_n_loop) + "n='1'";
	      new_stk[itos(sig_id)] = signals[k]->name+string("_bar");
	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		;//sig_decl_tb[signals[k]->name+string("_bar")]="2";
	      }
	      else{
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="1";
	      }
	      n_and++; sig_id++; 
	    }	      
	  }
	  //wrap the and stack with parenthesis.
	  if(andstack.size()){
	    andstack = '('+andstack+')';
	    sq_stmt << andstack.c_str();
	  }

	  if(new_stk.size()){
	    new_cmp->set_stk(new_stk, 'n');
	  }
	  
	  or_n_loop++;
	  gate_id_n.insert('n'+itos(n_and-1));
	}
	/* Output "else\n '0' when (" */
	//	sq_stmt<< "else\n"
	//     << string(8, ' ') <<"'0' after delay("
	//     << signals[(i-1)/2]->falllower << ", "
	//     << signals[(i-1)/2]->fallupper
	//     << ") when ";        
	sq_stmt<< "else\n"
	       << string(8, ' ') <<"'0' after "
	       << " ns when ";        
	tempi=i;
	i++;
	firstOR=TRUE;
	int or_p_loop=1;
	
	j=0;
	while ((cur_cover=find_cover(regions[i],j,exact))) {
	  j++;
        /* To here -- CJM */

	  mstrstr new_stk;

	  if(!firstOR) /*Output OR */ 
	    sq_stmt << " or ";
	  firstOR=FALSE;
	  firstAND=TRUE;
	  int p_and=1; char sig_id = 'a';
	  string andstack;
	  for (k=0;k<nsignals;k++){

	    if (cur_cover[k]=='1') {
	      if (!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      andstack += sig_id + itos(or_p_loop) + "p='0'";

	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="3";
		new_stk[itos(sig_id)] = signals[k]->name+string("_bar");
	      }
	      else{
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="1";
		new_stk[itos(sig_id)] = signals[k]->name+string("_bar");
	      }
	      string bar_sig=signals[k]->name+string("_bar");
	      p_and++; sig_id++; 
	    } 
	    else if (cur_cover[k]=='0') {
	      if (!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      andstack += sig_id + itos(or_p_loop) + "p='0'";
	      string bar_sig= signals[k]->name+string("_bar");
	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="3";
		new_stk[itos(sig_id)] = signals[k]->name;//+string("_i");
	      }
	      else{		  
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="1";
		new_stk[itos(sig_id)] = slb[signals[k]->name];
	      }
	      p_and++; sig_id++; 
	    }	      
	  }
	  //wrap the and stack with parenthesis.
	  if(andstack.size()){
	    andstack = '('+andstack+')';
	    sq_stmt << andstack.c_str();
	  }
	  
	  if(new_stk.size()){
	    new_cmp->set_stk(new_stk,'p');
	  }
	  
	  gate_id_p.insert('p'+itos(p_and-1));
	  or_p_loop++;
	}       
	i=tempi;
	sq_stmt<< " else"<< endl
	       << string(10, ' ') << "o_i;"<< endl<<endl;

	//	sq_stmt << "  o <= o_i;" << endl
	//<< "  obar <= not o_i after delay("
	//<<(signals[(i-1)/2]->falllower)/2 << ','
	//<<(signals[(i-1)/2]->fallupper)/2
	//<<");"<< endl;
	sq_stmt << "  o <= o_i;" << endl
		<< "  obar <= not o_i after "
		<<(signals[(i-1)/2]->falllower)/2 
		<<" ns;"<< endl;
	new_cmp->set_output(signals[(i-1)/2]->name+string("_bar"), 'b');
	sig_decl_tb[top_port_map[signals[(i-1)/2]->name]+string("_bar")]="3";

	/* If the component new_comp is not declared (not in component_decl), 
	   then add the component in it.
	   Add the component in the component instance list.
	*/
	string new_cmp_id=new_cmp->type();
	if(component_decl.find(new_cmp_id)==component_decl.end())
	  component_decl[new_cmp_id]=new_cmp->output_port(true);
	component_inst.push_back(new_cmp);
      }
      
      // Output combinational gates without bubble at the ouput.
      else if(rands==0){
	gate_ty=1;

	stringstream tmp_str;
	tmp_str<< "  "<< "o <= ";

	new_cmp->set_ty(1);
	new_cmp->set_output(signals[(i-1)/2]->name, 'o');
	new_cmp->set_delay(signals[(i-1)/2]->riselower, 
			   signals[(i-1)/2]->riseupper,
			   signals[(i-1)/2]->falllower,
			   signals[(i-1)/2]->fallupper);
	new_cmp->set_init(startstate[(i-1)/2]);
	

	firstOR=TRUE;
	int or_n_loop = 1;
	
	j=0;
	while ((cur_cover=find_cover(regions[i],j,exact))) {
	  j++;
	  mstrstr new_stk;

	  if (!firstOR) /*Output OR */ 
	    tmp_str<< " or ";

	  firstOR=FALSE;
	  firstAND=TRUE;
	  int n_and=1; char sig_id = 'a';
	  string andstack;
	  for(k=0;k<nsignals;k++){
	    if(cur_cover[k]=='1') {

	      if(!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;

	      /* Output signals[k]->name = '1' */
	      andstack += sig_id + itos(or_n_loop);
	      stringstream new_sid;
	      new_sid << sig_id<<or_n_loop<<'n';//<< ends;
	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		//sig_decl_tb[signals[k]->name+string("_i")]="2";
		new_stk[itos(sig_id)] = signals[k]->name;//+string("_i");
	      }
	      else{
		new_stk[itos(sig_id)] = slb[signals[k]->name];
	      }
	      n_and++; sig_id++; 
	    } 
	    else if(cur_cover[k]=='0') {
	      if (!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      
	      /* Output signals[k]->name = '0' */;
	      andstack += sig_id + itos(or_n_loop);
	      new_stk[itos(sig_id)] = signals[k]->name+string("_bar");
	      if (k < ninpsig)
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="1";
	      else
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="3";
		
	      n_and++; sig_id++; 
	    }	      
	  }
	  //wrap the and stack with parenthesis.
	  if(andstack.size()){
	    andstack = '('+andstack+')';
	    tmp_str << andstack.c_str();
	  }

	  if(new_stk.size())
	    new_cmp->set_stk(new_stk, 'n');
	  
	  or_n_loop++;
	  gate_id_n.insert('n'+itos(n_and-1));
	}
	//tmp_str<< ends;
	sq_stmt<< new_cmp->output_stmt()<<endl;
	
	// Add this new component in to component decl list if it's not there
	string new_cmp_id=new_cmp->type();
	if(component_decl.find(new_cmp_id)==component_decl.end())
	  component_decl[new_cmp_id]=new_cmp->output_port(true);
	component_inst.push_back(new_cmp);
      }

      // Combinational gates with bubbles at the outputs.
      else{
	gate_ty=2;
	stringstream tmp_str;
	tmp_str<< "  "<< "o_i <= not(";

	new_cmp->set_ty(2);
	new_cmp->set_output(signals[(i-1)/2]->name, 'o');
	new_cmp->set_delay(signals[(i-1)/2]->riselower, 
			   signals[(i-1)/2]->riseupper,
			   signals[(i-1)/2]->falllower,
			   signals[(i-1)/2]->fallupper);
	new_cmp->set_init(startstate[(i-1)/2]);
	
	firstOR=TRUE;
	int or_n_loop = 1;
	
	j=0;
	while ((cur_cover=find_cover(regions[i],j,exact))) {
	  j++;
	  mstrstr new_stk;

	  if (!firstOR) /*Output OR */ 
	    tmp_str<< " or ";

	  firstOR=FALSE;
	  firstAND=TRUE;
	  int n_and=1; char sig_id = 'a';
	  string andstack;
	  for(k=0;k<nsignals;k++){
	    if(cur_cover[k]=='1') {
	      if(!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      /* Output signals[k]->name = '1' */
	      andstack += sig_id + itos(or_n_loop);
	      stringstream new_sid;
	      new_sid << sig_id<<or_n_loop<<'n';//<< ends;
	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		//sig_decl_tb[signals[k]->name+string("_i")]="2";
		new_stk[itos(sig_id)] = signals[k]->name;//+string("_i");
	      }
	      else
		new_stk[itos(sig_id)] = slb[signals[k]->name];
	      
	      n_and++; sig_id++; 
	    } 
	    else if(cur_cover[k]=='0') {
	      if (!firstAND) /* Output AND */
		andstack += " and ";
	      firstAND=FALSE;
	      
	      /* Output signals[k]->name = '0' */;
	      andstack += sig_id + itos(or_n_loop);
	      new_stk[itos(sig_id)] = signals[k]->name+string("_bar");
	      if(port_sig.find(slb[signals[k]->name])!=port_sig.end() &&
		 port_sig[slb[signals[k]->name]]=="out"){
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="2";
	      }
	      else
		sig_decl_tb[top_port_map[signals[k]->name]+string("_bar")]="1";
	      
	      n_and++; sig_id++; 
	    }	      
	  }
	  //wrap the and stack with parenthesis.
	  if(andstack.size()){
	    andstack = '('+andstack+')';
	    tmp_str << andstack.c_str();
	  }

	  if(new_stk.size())
	    new_cmp->set_stk(new_stk, 'n');
	  
	  or_n_loop++;
	  gate_id_n.insert('n'+itos(n_and-1));
	}
	tmp_str<< ')';//<<ends;
	//	sq_stmt<< tmp_str.str()<<" after delay("
	//     << signals[(i-1)/2]->riselower<<", "
	//     << signals[(i-1)/2]->riseupper
	//     <<");"<<endl;
	sq_stmt<< tmp_str.str()<<" after "
	       << signals[(i-1)/2]->riselower
	       <<" ns;"<<endl;

	// Add this new component in to component decl list if it's not there
	string new_cmp_id=new_cmp->type();
	if(component_decl.find(new_cmp_id)==component_decl.end())
	  component_decl[new_cmp_id]=new_cmp->output_port(true);
	component_inst.push_back(new_cmp);
      }
      //sq_stmt<< ends;
      
      // Generate the generic entity name.
      string entity_id;
      for(multiset<string>::reverse_iterator i=gate_id_n.rbegin(); 
	  i!=gate_id_n.rend();i++)
	entity_id += *i;
      for(multiset<string>::reverse_iterator i=gate_id_p.rbegin(); 
	  i!=gate_id_p.rend();i++)
	entity_id += *i;

      entity<< get_header(new_cmp->type(),new_cmp->output_port(false))
            << endl;//<< ends;
      
      // Create the architecture for the above entity.
      // No internal signal is needed for combinational gates.
      arch<< "architecture behavior of "<< new_cmp->type()<< " is "<< endl;
      if(gate_ty==0)
	arch<< "  signal o_i : std_logic := '"<< new_cmp->get_init()<<"';" 
	    << endl <<endl;
      arch<< "begin"<<endl;

      if(gate_ty==0)
	arch<< new_cmp->output_stmt() <<endl;
      else if(gate_ty==1 || gate_ty==2)
	arch<< sq_stmt.str() <<endl;
      
      arch<< "end behavior;"<< endl<< endl;//<< ends;
      /*<< "configuration cfg_"<< entity_id <<"_behavior of "
	  << entity_id << " is"<< endl
          << "   for behavior"<< endl<< endl
          << "   end for;"<< endl
          << "end cfg_"<< entity_id <<"_behavior;"<< endl<< endl<< ends;
      */
      entity_id = new_cmp->type()+".vhd";

      // output each indivdule gate.
      
      ofstream fs(entity_id.c_str());
      fs << entity.str()<< arch.str();
      fs.close();     
      //delete new_cmp;
    }  
  }     

  // The following is the code for generating top-level entity and 
  //architecture.
  /* ADD CLOSING FOR GATE PACKAGE FILE */
  stringstream entity, arch;
  map<string,string> top_portdecl;
  map<string,string> top_portdecl_md;
  for(map<string,string>::iterator iter=sig_vec.begin(); iter!=sig_vec.end();
      iter++){
    if(!top_port.size()||top_port.find(iter->second.c_str())!=top_port.end()){
      if (top_port_map[iter->second].size() > 0) {
	top_portdecl[iter->first]=top_port_map[iter->second];
	top_portdecl_md[top_port_map[iter->second]]=sig_vec_md[iter->second];
      }
    }
  }
  entity << get_header(filename, top_portdecl, top_portdecl_md);//<< ends;
  
  /* Declare the internal signals which connect the inverted signals. */
  string sig_decl;
  if(sig_decl_tb.size()){
    map<string,string>::iterator iter;
    stringstream tmp, tmp1, tmp2, tmp3, tmp4;
    int cnt1, cnt2, cnt3, cnt4;
    cnt1=cnt2=cnt3=0;
    tmp1 << "  -- input complements" << endl;
    tmp2 << "  -- internal signals for output" << endl;
    tmp3 << "  -- output complements"<< endl;
    for(iter=sig_decl_tb.begin();iter!=sig_decl_tb.end();iter++){
      if(iter->second=="1"){
	string new_string=iter->first;
	if (new_string.find(Tolower(filename))!=string::npos) {
	  new_string.replace(new_string.find("_"+Tolower(filename)),
			     1+filename.size(),"");
	}
	tmp1 <<"  signal "<<new_string<<": std_logic;"<< endl;
	cnt1++;
      }
      else if(iter->second=="2"){
	string new_string=iter->first;
	if (new_string.find(Tolower(filename))!=string::npos) {
	  new_string.replace(new_string.find("_"+Tolower(filename)),
			     1+filename.size(),"");
	}
	tmp2 << "  signal "<<new_string<<": std_logic;"<< endl;
	cnt2++;
      }
      else if(iter->second=="3"){
	string new_string=iter->first;
	if (new_string.find(Tolower(filename))!=string::npos) {
	  new_string.replace(new_string.find("_"+Tolower(filename)),
			     1+filename.size(),"");
	}
	tmp3 << "  signal "<<new_string<<": std_logic;"<< endl;
	cnt3++;
      }
      else if(iter->second=="4"){
	string new_string=iter->first;
	if (new_string.find(Tolower(filename))!=string::npos) {
	  new_string.replace(new_string.find("_"+Tolower(filename)),
			     1+filename.size(),"");
	}
	tmp4 << "  signal "<<new_string<<": std_logic;"<< endl;
	cnt4++;
      }	
    }
    tmp1 << endl;//<<ends;
    tmp2 << endl;//<<ends; 
    tmp3 << endl;//<<ends;
    tmp4 << endl;//<<ends;

    if(cnt1 || cnt2 || cnt3 || cnt4)
      sig_decl += "  -- internal signals declarations\n";
    
    if(cnt4) sig_decl += string(tmp4.str())+"\n";
    if(cnt1) sig_decl += string(tmp1.str())+"\n";
    if(cnt2) sig_decl += string(tmp2.str())+"\n";
    if(cnt3) sig_decl += string(tmp3.str())+"\n";
    
  }

  map<string,string> comp_ty;
  stringstream comp_decl;
  for(mstrstr::iterator iter=component_decl.begin();iter!=component_decl.end();
      iter++)
    comp_decl<< "  component "<< iter->first << endl
	     <<iter->second<<endl
	     << "  end component;"<< endl<< endl;

  //comp_decl << ends;

  // Output the component portmaps.
  map<string,string> bar_sig; 
  stringstream comp_stmt, config_decl_1;
  int inst_id=0;
  for(list<basic_comp*>::iterator iter=component_inst.begin();
      iter!=component_inst.end(); iter++, inst_id++){
    comp_stmt<< "  i_"<<itos(inst_id)<< " : "<<(*iter)->type() <<endl
	     << (*iter)->output_portmap()<< endl << endl;
   
    string config="work.cfg_"+(*iter)->type()+"_behavior";
    config_decl_1<< "     for i_"<< itos(inst_id)<< " : "<<(*iter)->type()
		 <<endl
		 << "        use configuration "<< config<< ";"<< endl
		 << "     end for;"<< endl<< endl;
  }
  
  stringstream con_st_tmp1, con_st_tmp2;
  int cnt1=0, cnt2=0;
  con_st_tmp1 << "  -- complements of inputs"<< endl;
  con_st_tmp2 << "  -- mapping internal signals to outputs" << endl;
  for(map<string, string>::iterator iter=sig_decl_tb.begin(); 
      iter!=sig_decl_tb.end(); iter++){    
    if(iter->second=="1"){
      string tmp_sig(iter->first, 0, iter->first.size()-4);
      //      con_st_tmp1 <<"  "<<iter->first<<" <= not "<<slb[tmp_sig]
      //	  <<" after delay("<<min_gate_delay<<','<< max_gate_delay
      //	  <<");"<< endl;
      con_st_tmp1 <<"  "<<iter->first<<" <= not "<<tmp_sig
		  <<" after "<<min_gate_delay<<" ns;"<< endl;
      cnt1++;
    }
    else if(iter->second=="2"){
      string tmp_sig(iter->first, 0, iter->first.size()-2);
      //cout <<iter->first <<endl;
      con_st_tmp2 <<"  "<<slb[tmp_sig]<<" <= "<< iter->first << ';'<< endl;
      cnt2++;
    }
  }
  if(cnt1){
    con_st_tmp1 << endl;//<< ends; 
    comp_stmt << con_st_tmp1.str();
  }
  if(cnt2){
    con_st_tmp2 << endl;//<< ends;
    comp_stmt << con_st_tmp2.str();
  }
  //comp_stmt <<ends;

  stringstream config_decl;
  //config_decl_1<<ends;
  config_decl<< "configuration cfg_"<<filename<<"_structure of "<< filename
	     << " is"<< endl
	     << "  for structure"<< endl
	     << config_decl_1.str()  
	     << "  end for;"<< endl
             << "end cfg_"<<filename<<"_structure;"<< endl;//<< ends;

  arch << "architecture structure of "<< filename<< " is"<< endl << endl
       << sig_decl << endl
       << comp_decl.str()
       << "begin"<< endl
       << comp_stmt.str() 
       << "end structure;"<< endl << endl;// << ends;
  //       << config_decl.str() << ends;
 

  fout<< entity.str()<< endl
      << arch.str();
  fout.close(); 

  /* ADD CLOSING FOR VHDL FILE */
  free(fanout);

  for(list<basic_comp*>::iterator iter=component_inst.begin();
      iter!=component_inst.end();iter++)
    delete (*iter);
}

/*****************************************************************************/
/* Store structural VHDL netlist of implmentation.                           */
/*****************************************************************************/

void store_struct_vhd(char command,designADT design)
{ 
  if (design->status & STOREDPRS) { 
    min_gate_delay=design->mingatedelay;
    max_gate_delay=design->maxgatedelay;
    store_vhd(design->filename,design->signals,design->events,
	      design->state_table,
	      design->regions,design->ninpsig,design->nsignals,
	      design->maxsize,design->gatelevel,design->combo,
	      design->exact, design->startstate);
  }
}
