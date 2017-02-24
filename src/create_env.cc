#include "create_env.h"
#include <string>

extern actionADT dummyE();
//extern actionADT dummyE(string id);

actionADT dummyE(const string& ID)
{
  string s= "$" + ID;
  actionADT a = action(s.c_str(), s.size());
  a->type = DUMMY;
  return a;
}


// Test if str appears as part of level. If it does, return true; otherwise,
// return false.
bool in_level(const string& level, const string& str)
{
  string tmp_level(level);
  string tmp_str("-");
  tmp_str += str;
  tmp_str[tmp_str.size()-1] = '-';

  for(unsigned int i = 0; i < level.size(); i++)
    if(tmp_level[i]=='[' || tmp_level[i]==']' || tmp_level[i]=='(' ||
       tmp_level[i]==')' || tmp_level[i]=='&' || tmp_level[i]=='|' ||
       tmp_level[i]=='~' || tmp_level[i]==' ')
      tmp_level[i] = '-';

//for(string::iterator iter = tmp_level.begin(); iter!=tmp_level.end(); iter++)

  return (tmp_level.find(tmp_str) != string::npos);

  //return false;
}



signals_2_dummies::signals_2_dummies(const TERstructADT S, 
				     const TERstructADT T, 
				     const map<string,int>& LL)
{
  ret_val = 0;
  ret_val = (*this)(S, T, LL);
}


void signals_2_dummies::reset()
{
  TERSdelete(ret_val); 
  ret_val = 0;
  mappings = map<string, string>();
  port = map<string,int>();
}


TERstructADT signals_2_dummies::get_result() const
{
  return TERScopy(ret_val);
}


TERstructADT signals_2_dummies::operator()(const TERstructADT S,
					   const TERstructADT T,
					   const map<string,int>& LL)
{
  reset();

  tmp_S = S;
  
  for(map<string,int>::const_iterator b = LL.begin(); b != LL.end(); b++)
    {
      port[b->first + "+"] = b->second;
      port[b->first + "-"] = b->second;
    }

  TERstructADT ret_val = TERSempty();
  if(S){
    eventsetADT tmp = union_events(S->I, S->O);

    ret_val->A = action_set(tmp,T);
    ret_val->I = event_set(S->I);
    ret_val->O = event_set(S->O);
    //ret_val->first = event_set(S->first);
    //ret_val->last = event_set(S->last);
    //ret_val->loop = event_set(S->loop);
    ret_val->R = rule_set(S->R);
    ret_val->Rp = rule_set(S->Rp);
    ret_val->C = conflict_set(S->C);
    ret_val->Cp = conflict_set(S->Cp);
  }
  return TERScopy(ret_val);
}


actionsetADT signals_2_dummies::action_set(const eventsetADT E,
					   const TERstructADT T)
{
  actionsetADT result = 0;

  for(eventsetADT cur_event = E; cur_event; cur_event = cur_event->link) {
    // For each event whose signal is not on the port,
    // a new dummy action is created. Otherwise, its action is copied.
    string sig_id = cur_event->event->action->label;
    string event_id = sig_id + "/"+ itos(cur_event->event->occurrence);
    actionADT tmp = 0;

    if(port.find(sig_id) != port.end()){
      tmp = action(sig_id.c_str(), (int)sig_id.size());
      tmp->type = port[sig_id];
      mappings[event_id] = sig_id;
    }
    else{
      // Test if the sig_id appears in the level of any rules. If it does,
      // variable dummy_type = true; otherwise, dummy_type = false.
      bool dummy_type = false;
      for(rulesetADT iter = tmp_S->R; iter; iter = iter->link){
	if(in_level(iter->rule->exp, sig_id)==true){
	  dummy_type = true;
	  break;
	}	  
      }

      for(rulesetADT iter = tmp_S->Rp; iter; iter = iter->link){
	if(in_level(iter->rule->exp, sig_id)==true){
	  dummy_type = true;
	  break;
	}	  
      }

      for(rulesetADT iter = T->R; iter; iter = iter->link){
	if(in_level(iter->rule->exp, sig_id)==true){
	  dummy_type = true;
	  break;
	}	  
      }

      for(rulesetADT iter = T->Rp; iter; iter = iter->link){
	if(in_level(iter->rule->exp, sig_id)==true){
	  dummy_type = true;
	  break;
	}	  
      }
      //cout << sig_id.c_str() << " dummy " << dummy_type << endl;

      // If dmmy_type is true and the signal is an input; only the type of
      // the signal action is changed to DUMMY; otherwise, the signal action
      // is replaced by a dummy action.
      //#ifdef __DUMMY_TYPE__
      if(dummy_type){
	tmp = action(sig_id.c_str(), (int)sig_id.size());
	tmp->type = (DUMMY | IN);
	mappings[event_id] = sig_id;
      }
      //#else
      else {
	tmp = dummyE(sig_id);
	tmp->type = DUMMY;
	tmp->upper = 0;
	tmp->delay.ur = 0;
	tmp->delay.uf = 0;
	mappings[event_id] = tmp->label;
      }
	//#endif
    }

    actionsetADT AA = create_action_set(tmp);
    actionsetADT previous = result;
    result = union_actions(result, AA);

    delete_action_set(previous);
    delete AA;      
  } 
  return result;
}


eventsetADT signals_2_dummies::event_set(const eventsetADT E)
{
  eventsetADT result = 0;

  eventsADT tmp;
  
  /*for(map<string,string>::iterator iter=mappings.begin();
      iter!=mappings.end(); iter++)
    cout << iter->first << "  "<< iter->second<< endl;
  cout <<"----------------\n";
  */
  for(eventsetADT cur_event = E; cur_event; cur_event = cur_event->link) {
    // New action with new action label.
    
    string sig_id = cur_event->event->action->label;
    string event_id = sig_id + "/"+ itos(cur_event->event->occurrence);
    string new_label = mappings[event_id];
    
    actionADT aa = action(new_label.c_str(), new_label.size());

    // New event with new action.
    tmp = event(aa, cur_event->event->occurrence, cur_event->event->lower,
		cur_event->event->upper,cur_event->event->dist);
    
    if(tmp != cur_event->event){ 
      if(tmp->exp)  free(tmp->exp);
      tmp->exp = CopyString("[true]");
    }
    
    eventsetADT EE = create_event_set(tmp);
    eventsetADT previous = result;
    result = union_events(result, EE);
    
    delete_event_set(previous);
    delete EE;
  }
  /*
  cout <<"================\n";
  
  for(eventsetADT cur_event = E; cur_event; cur_event = cur_event->link) 
    cout << cur_event->event->action->label << "/" 
    << cur_event->event->occurrence<< endl;*/
  return result;
}


rulesetADT signals_2_dummies::rule_set(const rulesetADT R)
{
  rulesetADT result = 0;

  rulesADT tmp;

  for(rulesetADT cur_rule = R; cur_rule; cur_rule=cur_rule->link){
    string enabling_sig_id=cur_rule->rule->enabling->action->label;
    string enabled_sig_id = cur_rule->rule->enabled->action->label;
    string enabling_event_id=enabling_sig_id + "/" +
      itos(cur_rule->rule->enabling->occurrence);
    string enabled_event_id = enabled_sig_id + "/" +
      itos(cur_rule->rule->enabled->occurrence);

      string new_enabling_label;

      if(enabling_sig_id == string("reset"))
	new_enabling_label = "reset";
      else
	new_enabling_label = mappings[enabling_event_id];

      string new_enabled_label = mappings[enabled_event_id];

      // New enabling action of the current rule.
      actionADT new_enabling_action = action(new_enabling_label.c_str(),
					     new_enabling_label.size());
      
      // New enabled action of the current rule.
      actionADT new_enabled_action = action(new_enabled_label.c_str(),
					    new_enabled_label.size());
      
      // Using new_enabling_action to find new enabling event 
      // in the current rule. 
      eventsADT new_enabling_event = 
	event(new_enabling_action,	
	      cur_rule->rule->enabling->occurrence,
	      cur_rule->rule->enabling->lower,
	      cur_rule->rule->enabling->upper,
	      cur_rule->rule->enabling->dist,
	      cur_rule->rule->enabling->exp);
      
      // Using new_enabled_action to find new enabled event 
      // in the current rule. 
      eventsADT new_enabled_event = 
	event(new_enabled_action,	
	      cur_rule->rule->enabled->occurrence,
	      cur_rule->rule->enabled->lower,
	      cur_rule->rule->enabled->upper,
	      cur_rule->rule->enabled->dist,
	      cur_rule->rule->enabled->exp);

      // Find or create the rule with new enabling and enabled events.
      tmp = rule(new_enabling_event,
		 new_enabled_event,
		 cur_rule->rule->lower,
		 cur_rule->rule->upper, 
		 cur_rule->rule->type,
		 cur_rule->rule->regular, 
		 cur_rule->rule->dist,
		 cur_rule->rule->exp);
      /*
      if (tmp != cur_rule->rule)
	{
	  if(tmp->exp)
	    free(tmp->exp);
	  tmp->exp = CopyString("[true]");
	}
      */
      rulesetADT RR = create_rule_set(tmp);
      rulesetADT previous = result;
      result = union_rules(result, RR);

      delete_rule_set(previous);
      free(RR);
    }

  return result; 
}


conflictsetADT signals_2_dummies::conflict_set(const conflictsetADT C)
{
  conflictsetADT result = 0;
  
  for(conflictsetADT cur_conflict= C; cur_conflict; 
      cur_conflict = cur_conflict->link){

    string left_sig_id = cur_conflict->conflict->left->action->label;
    string right_sig_id = cur_conflict->conflict->right->action->label;
    string left_event_id = left_sig_id + "/" +
      itos(cur_conflict->conflict->left->occurrence);
    string right_event_id = right_sig_id + "/" +
      itos(cur_conflict->conflict->right->occurrence);
      
    string left_label = mappings[left_event_id];
    string right_label = mappings[right_event_id];
      
      actionADT new_left_action = action(left_label.c_str(),left_label.size());
      actionADT new_right_action = action(right_label.c_str(),
					  right_label.size());
      
      eventsADT new_left_event = 
	event(new_left_action,	
	      cur_conflict->conflict->left->occurrence,
	      cur_conflict->conflict->left->lower,
	      cur_conflict->conflict->left->upper,
	      cur_conflict->conflict->left->dist,
	      cur_conflict->conflict->left->exp);
      
      eventsADT new_right_event = 
	event(new_right_action,	
	      cur_conflict->conflict->right->occurrence,
	      cur_conflict->conflict->right->lower,
	      cur_conflict->conflict->right->upper,
	      cur_conflict->conflict->right->dist,
	      cur_conflict->conflict->right->exp);

      result = add_conflict(result,new_left_event, new_right_event);
    }
  return result; 
}


char *signals_2_dummies::new_expression(const char *expr)
{
  return "true";
}
