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
/* storevhd.h                                                                */
/*****************************************************************************/

#ifndef _storevhd_h
#define _storevhd_h

#include <stdio.h>
#include <iostream>
#include "struct.h"
#include "def.h" 
#include <ctype.h> 
#include "auxilary.h"
#include <string>

/*****************************************************************************/
/* Store structural VHDL netlist of implmentation.                           */
/*****************************************************************************/

void store_struct_vhd(char command,designADT design);

class port{
public:
  port();
  port(const port&);
  ~port();
  void set_ports(const map<string,string>& a_ports) { ports = a_ports; }
  int getCount() { return names.size(); }
  char* getMode(int i) { return NULL; }
  string getName(int i);
  void add(char*, char*, const string& stk_ty, char *ty="std_logic");
  string output();
  string output_portmap(const map<string,string>& bar_sig);
  const map<string,string>& get_portmap() { return ports; }
  int isSignal(char*, char*);
  void print();

protected: 
  
  map<int,string> names;
  map<int,string> signals;
  map<int,string> modes;
  map<int,string> stack_ty;
  int index;
  map<string, string> ports;
};


class comp{


public:
  comp();
  ~comp();
  void set(const char*, port*);
  void set_sig_tb(const map<string,string>& a_ports);
  void add_pins(const string&, const string&);
  string output(char *);
  int getCount(){ return count; }
  const char* get_inst_id(int index) { return label[index]; }
  char *getName(int i) { return names[i]; }
  port *getPort(int i) { return signals[i]; }
  void decideport();
  string output();
  void isSignal();
  int isSignal(char*);
  string port_map(int i, const map<string,string>& bar_sig);
  const map<string,string>& get_portmap(int index);
  char *lookup(const char*);
  char *sigdecl();
  char *sigass(char *signals[], int num);
  friend ostream &operator<<(ostream&, comp*); 
   
protected:
  int count;
  char *label[500];
  char *names[500];
  port *signals[500];

  int inputs, total;
  map<string,string> ports, sig_tb;
  
};
  

string get_header(const string&);

typedef map<string,string> mstrstr;

class basic_comp
{
public:
  
  basic_comp();
  ~basic_comp();
  
  void set_stk(const mstrstr& a_stk, char ty);
  void set_delay(int, int,int,int);
  void set_output(const string& a_o, char which);
  string type(void);
  string output_portmap(void);
  string output_port(bool);
  string output_stmt(void);
  void set_ty(int i) { ty=i; }
  void set_init(char i) { initial = i; }
  char get_init(void) { return initial; }
  
protected:
  
  int ty;
  // stores names of each stack and its size. 
  // mstrstr = names; int = size
  list<pair<mstrstr,int> > n_stk, p_stk;
  string o, obar;
  int lr, ur, lf, uf;   
  char initial;
};


#endif  /* storevhd.h */

