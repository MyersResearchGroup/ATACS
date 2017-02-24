// Contains facility to create environment code for a block
// Author: Hao Zheng
// Date:   4-19-99
#ifndef CREATE_ENV_H
#define CREATE_ENV_H

#include "auxilary.h"
#include <string>


class signals_2_dummies
{
public:
  
  signals_2_dummies() { ret_val = 0; }
  signals_2_dummies(const TERstructADT, const TERstructADT, 
		    const map<string,int>&);
  ~signals_2_dummies() { TERSdelete(ret_val); }

  void reset(void);
  TERstructADT get_result() const;
  TERstructADT operator()(const TERstructADT, const TERstructADT, 
			  const map<string,int>&);

  //Private member function section
private:
  
  actionsetADT action_set(const eventsetADT E,const TERstructADT T);
  eventsetADT event_set(const eventsetADT E);
  rulesetADT rule_set(const rulesetADT R);
  conflictsetADT conflict_set(const conflictsetADT C);
  char *new_expression(const char *s);

  // Data member section
private:
  
  TERstructADT ret_val;
  map<string, string> mappings;
  map<string,int> port;
  telADT tmp_S;
};
#endif
