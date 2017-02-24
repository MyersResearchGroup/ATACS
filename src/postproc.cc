#include "postproc.h"
#include "auxilary.h"
#include "events.h"
#include "conflicts.h"
#include <string>
#include <fstream>
#include "struct.h"

int cnt= 0;
#ifdef DDEBUG
extern map<string, telADT> pproc_debug;
#endif

#define XFORM_3
#define XFORM_1_6
#define XFORM_1
#define XFORM_2
#define XFORM_4_5
//#define RM_RED
//#define PP_INTERACTIVE


bool interactive = false;

// This function removes conflicts from S-C that both events do not 
// have a common event in their presets of postsets.
void clear_C(telADT S)
{
  map<eventsADT, set<eventsADT> > preset, postset;

  for(rulesetADT iter=S->R; iter; iter=iter->link){
    preset[iter->rule->enabled].insert(iter->rule->enabling);
    postset[iter->rule->enabling].insert(iter->rule->enabled);
  }

  for(rulesetADT iter=S->Rp; iter; iter=iter->link){
    preset[iter->rule->enabled].insert(iter->rule->enabling);
    postset[iter->rule->enabling].insert(iter->rule->enabled);
  }

  set<conflictsetADT> false_c;
  for(conflictsetADT iter=S->C; iter; iter=iter->link){
    set<eventsADT> tmp, tmp2;

    map<eventsADT, set<eventsADT> >::iterator preset_left, preset_right;

    //    cout << iter->conflict->left<< endl;
    preset_left = preset.find(iter->conflict->left);
    preset_right = preset.find(iter->conflict->right);

    if(preset_left!=preset.end() && preset_right!=preset.end()) {
      set_intersection(preset_left->second.begin(), preset_left->second.end(),
		       preset_right->second.begin(),preset_right->second.end(),
		       inserter(tmp, tmp.begin()));
      if (!tmp.empty()) continue;
    }

    map<eventsADT, set<eventsADT> >::iterator postset_left, postset_right;

    postset_left = postset.find(iter->conflict->left);
    postset_right = postset.find(iter->conflict->right);

    if(postset_left!=postset.end() && postset_right!=postset.end()) {
      set_intersection(postset_left->second.begin(), 
		       postset_left->second.end(),
		       postset_right->second.begin(),
		       postset_right->second.end(),
		       inserter(tmp2, tmp2.begin()));
      if (!tmp2.empty()) continue;
    }
    false_c.insert(iter);
  }

  conflictsetADT prev=0;
  for(conflictsetADT iter=S->C; iter; iter=iter->link){
    if(false_c.find(iter)!=false_c.end()){
      if(!prev)
	S->C = iter->link;
      else
	prev->link = iter->link;
    }
    else
      prev = iter;
  }
}

void test(telADT S)
{
  eventsetADT cur_event;

  for(cur_event = S->O; cur_event; cur_event = cur_event->link){
    
    rule_lst fanin, fanout;

    find_rules(cur_event->event, S, fanin, fanout);
	
    bool fin_dummy=true;
	
    for(rule_lst::iterator i=fanin.begin(); i!=fanin.end();i++)
      if((!(i->first->enabling->action->type == DUMMY)) &&
	 i->first->enabling->action->label!=string("reset")){
	fin_dummy=false;
	break;
      }
    if(fin_dummy)
      cout << cur_event->event->action->label<<'/'
	   <<cur_event->event->occurrence<<" has pure dummy enabling\n";
  }
}


// Check if each pair of enabling events of 'fanin' are in conflict. 
bool pure_conflict(telADT S, rule_lst rules, bool preset)
{
  if(!S->C) 
    return false;

  for(rule_lst::iterator iter=rules.begin(); iter!=rules.end();iter++)
    for(rule_lst::iterator iter_i=rules.begin(); iter_i!=rules.end();iter_i++)
      if(preset == true){
	if(iter->first->enabling->action->label != string("reset") &&
	   iter_i->first->enabling->action->label != string("reset") &&
	   iter->first->enabling != iter_i->first->enabling &&
	   !check_C(S->C, iter->first->enabling, iter_i->first->enabling))
	  return false;
      }
      else{
	if(iter->first->enabled != iter_i->first->enabled &&
           !check_C(S->C, iter->first->enabled, iter_i->first->enabled))
          return false;
      }

  return true;
}

// Returns a set of events containing all events in conflict with dummy.
set<eventsADT> find_conflicts(telADT S, eventsADT dummy)
{
  set<eventsADT> ret_val;

  for(conflictsetADT iter=S->C; iter; iter=iter->link)
    if(event_cmp(iter->conflict->left, dummy)==true)
      ret_val.insert(iter->conflict->left);
    else if(event_cmp(iter->conflict->right, dummy)==true)
      ret_val.insert(iter->conflict->right);

    return ret_val;
}

/*
set<eventsADT> find_place(telADT S, conflictsADT C, )
{
  
  rule_lst fanin, fanout;
  int fin_size, fout_size;

  find_rules(C->left, S, fanin, fanout, fin_size, fout_size);
  
}
*/

//check if there is any conflict in fanin rules and fanout rules of the 
//dummy event, and if the dummy event is in conflict with any other events.
//If true, this function returns true; otherwise, this function returns false.
bool check_conflict(telADT S, eventsADT dummy, 
		    rule_lst& fanin, rule_lst& fanout)
{
  // Checking fanin rules.
  for(rule_lst::iterator iter_i=fanin.begin(); iter_i!=fanin.end();iter_i++)
    for(rule_lst::iterator iter_ii=fanin.begin(); iter_ii!=fanin.end();
	iter_ii++)
      if(iter_i->first->enabling != iter_ii->first->enabling &&
	 check(iter_i->first->enabling, iter_ii->first->enabling))
	return true;
  
  // Checking fanout rules.
  for(rule_lst::iterator iter_i=fanout.begin(); iter_i!=fanout.end();iter_i++)
    for(rule_lst::iterator iter_ii=fanout.begin(); iter_ii!=fanout.end();
	iter_ii++)
      if(iter_i->first->enabled != iter_ii->first->enabled &&
	 check(iter_i->first->enabled, iter_ii->first->enabled))
	return true; 

  // checking the dummy event.
  for(eventsetADT cur_event = S->I; cur_event; cur_event = cur_event->link)
    if(dummy != cur_event->event &&
       check(dummy, cur_event->event))
      return true;
  
  for(eventsetADT cur_event = S->O; cur_event; cur_event = cur_event->link)
    if(dummy != cur_event->event &&
       check(dummy, cur_event->event))
      return true;
  
  return false;
}


bool rm_self_lp(telADT S, eventsADT dummy, rule_lst& fanin, rule_lst& fanout,
		bool verbose)
{
  int upper=0;
  bool self_lp=false;

  if(fanout.size()==1 && fanout.front().first->enabled==dummy)
    return false;

  // Check that removing self-loop does not break conflict relations.
  for(rule_lst::iterator iter_fi=fanin.begin();iter_fi!=fanin.end();iter_fi++)
    if(check_C(S->C, iter_fi->first->enabling, dummy))
      return false;


  // Check that removing self-loop does not break conflict relations.
  for(rule_lst::iterator iter_fo=fanout.begin(); iter_fo!=fanout.end(); 
      iter_fo++)
    if(check_C(S->C, iter_fo->first->enabled, dummy))
      return false; 
       

  /* NEW: Do not remove self loop if it is the only fanin */
  bool ok = false;
  for(rule_lst::iterator iter_fi=fanin.begin();iter_fi!=fanin.end();iter_fi++){
    if(!event_cmp(iter_fi->first->enabling, dummy) &&
       (iter_fi->first->enabling->action->label!=string("reset"))) {
       ok = true;
       break;
    }
  }
  if (!ok) return false;

  for(rule_lst::iterator iter_fi=fanin.begin();iter_fi!=fanin.end();iter_fi++){
    if(event_cmp(iter_fi->first->enabling, dummy) && 
       (iter_fi->first->exp == string("[true]"))){
      for(rule_lst::iterator iter_fo=fanout.begin(); iter_fo!=fanout.end(); 
	  iter_fo++){
	if(event_cmp(iter_fo->first->enabled, dummy)){

	  upper=iter_fo->first->upper;
	  self_lp=true;
	  
	  if(iter_fo->second == true)
	    //S->Rp = subtract_rules(S->Rp, create_rule_set(iter_fo->first));
	    S->Rp= rm_rules(S->Rp, iter_fo->first);
	  else
	    //S->R = subtract_rules(S->R, create_rule_set(iter_fo->first));
	    S->R= rm_rules(S->R, iter_fo->first);
	  fanin.erase(iter_fi);
	  fanout.erase(iter_fo);
	  if (verbose)
	    cout << "rm self loop 2 " << 
	      iter_fo->first->enabling->action->label << endl;
	  break;
	}
      }
    }
    if(self_lp)
      break;
  }

  if(self_lp){
    for(rule_lst::iterator iter=fanin.begin(); iter!=fanin.end(); iter++)
      if(iter->first->upper<upper)
	iter->first->upper = upper;
    return true;
  }
  return false;
}

bool real_dummy(actionADT action) 
{
  if ((action->type == DUMMY) /*&&
				(!(strncmp(action->label,"$XL_",4)==0))*/)
    return true;
  return false;
}

bool rm_self_lp(telADT S,bool verbose)
{
  bool modified = false;
  telADT S_old = TERScopy(S);

  for(rulesetADT cur_r = S_old->Rp; cur_r; cur_r = cur_r->link){
    if((cur_r->rule->enabling == cur_r->rule->enabled) && 
       (cur_r->rule->exp == string("[true]")) &&
       (real_dummy(cur_r->rule->enabling->action))) {
      if (verbose)
	cout << "rm self loop 1\n";

      rule_lst fanin, fanout;
      int fin_size, fout_size;
      
      find_rules(cur_r->rule->enabling, S, fanin, fanout, fin_size, fout_size);

      //cout << cur_r->rule->enabling->action->label<< endl;
      modified = rm_self_lp(S, cur_r->rule->enabling, fanin, fanout, verbose);
    }
  }

  TERSdelete(S_old);

  return modified;
}

//Remove all conflicts in 'S' which are involved with 'E'.
void rm_conflict(telADT S, eventsADT E)
{
  for(eventsetADT iter = S->O; iter; iter = iter->link)
    if(iter->event != E){
      conflictsADT cft = check_C(S->C, iter->event, E);
      
      if(cft)
	S->C = subtract_conflicts(S->C, create_conflict_set(cft));  
    }
}

bool xform_1(telADT S,bool fromG,bool verbose)
{
  bool modified = false;

  telADT S_old = TERScopy(S);
  
  for(eventsetADT cur_event=S_old->O; cur_event; cur_event = cur_event->link){

    rule_lst fanin, fanout;

    if(real_dummy(cur_event->event->action)) {
      int fin_size, fout_size;

      find_rules(cur_event->event, S, fanin, fanout, fin_size, fout_size);

#ifdef XFORM_3
      rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
#endif

      // If the number of places in the preset greater than 1, 'preset'=true;
      // Same to 'postset'.
      bool preset = pure_conflict(S, fanin, true);
      bool postset = pure_conflict(S, fanout, false);

      /* if((fout_size==0)&&(fin_size>0)){
	for(rule_lst::iterator iter=fanin.begin(); iter!=fanin.end(); iter++)
	  if(iter->second==true)
	    S->Rp = rm_rules(S->Rp, iter->first);
	  else
	    S->R = rm_rules(S->R, iter->first);
	// TODO: Need code to subtract event/action when no longer used 
	//cout << "Subtracting " << cur_event->event->action->label << endl;
	S->O = subtract_events(S->O, create_event_set(cur_event->event));
	//S->A = rm_action(S->A, cur_event->event->action);  
	rm_conflict(S, cur_event->event);  
	modified = true;
      }
      else */ if(fin_size==1 && fout_size==1) {

	// See xformation-1 case 1.
	if(S->C && 
	   (fanin.front().first->enabling==fanout.front().first->enabled) &&
	   (fanin.front().first->lower+fanout.front().first->lower)>0){
	  //bool removable=false;
	  conflictsetADT cur_conflict;
	  for(cur_conflict = S->C; cur_conflict; 
	      cur_conflict = cur_conflict->link){
	    if(cur_conflict->conflict->left==cur_event->event){
	      rulesADT r_x_b = check_rule(S->R, fanin.front().first->enabling, 
					  cur_conflict->conflict->right);
	      if(r_x_b)
		r_x_b->upper=INFIN;
	    }
	    else if(cur_conflict->conflict->right==cur_event->event){
	      rulesADT r_x_b = check_rule(S->R, fanin.front().first->enabling, 
					  cur_conflict->conflict->left);
	      if(r_x_b)
		r_x_b->upper=INFIN;
	    }
	  }
	  S->R = rm_rules(S->R, fanin.front().first);
	  S->R = rm_rules(S->R, fanout.front().first);
	}
	       
	// Do not remove single self loop.
	if(!(fout_size==1 && fanout.front().first->enabled==cur_event->event))
	  if(update(S, cur_event->event, fanin, fanout, 1, fromG, verbose)){
	    modified = true;
	    //rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
	  }
      }
      // To remove the event with single place in its preset and postset. 
      else if(preset && postset){
	if(update(S, cur_event->event, fanin, fanout, 1, fromG, verbose))
	  modified = true;
      }
    }
  }

  return (modified);
}


bool xform_2(telADT S,bool fromG,bool verbose)
{
  bool modified = false;
  eventsetADT cur_event;

  for(cur_event = S->O; cur_event; cur_event = cur_event->link){
    rule_lst fanin, fanout;
    if(real_dummy(cur_event->event->action)){
      int fin_size, fout_size;

      find_rules(cur_event->event, S, fanin, fanout, fin_size, fout_size);

      // If the number of places in the preset greater than 1, 'preset'=true;
      // Same to 'postset'.
      bool postset = pure_conflict(S, fanout, false);

      if((fin_size != 1 && fout_size/*<*/==1) || postset){
	// Do not remove single self loop.
	if(!(fout_size==1 && fanout.front().first->enabled==cur_event->event)){
	  if(update(S, cur_event->event, fanin, fanout, 2, fromG, verbose)){
	    modified = true;
	  } 
	}
      }
    }
  }
  
  return (modified);
}


bool xform_3(telADT S, bool fromG, bool verbose)
{
  bool modified = false;
  bool okay;
  eventsetADT cur_event;
  
  for(cur_event = S->O; cur_event; cur_event = cur_event->link){
    rule_lst fanin, fanout;
    if(real_dummy(cur_event->event->action)){

      int fin_size, fout_size;
      find_rules(cur_event->event, S, fanin, fanout, fin_size, fout_size);
      
      if(fin_size==1){
	// Do not remove single self loop.
	if(!(fout_size==1 && fanout.front().first->enabled==cur_event->event)){

	  /* NEW: Do not remove if fanin and a fanout are same event */
	  rulesADT temp=NULL;
	  for(rule_lst::iterator iter_fi=fanin.begin();iter_fi!=fanin.end();
	      iter_fi++) {
	    if (iter_fi->first->enabling->action->label!=string("reset")) {
	      temp = iter_fi->first;
	      break;
	    }
	  }
	  okay=true;
	  for(rule_lst::iterator iter=fanout.begin();iter!=fanout.end();
	      iter++){
	    if(event_cmp(iter->first->enabled, temp->enabling)) {
	      okay=false;
	      break;
	    }
	  }
	  if (!okay) continue;

	  if(update(S, cur_event->event, fanin, fanout, 3, fromG, verbose)){
	    modified = true;
	    //rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
	  }
	}
      }
    }
  }
  
  return (modified);
}


// Remove dummy events whose all fanin and fanout are dummy events. 
bool xform_4(telADT S,bool fromG,bool verbose)
{
  bool modified = false;

  eventsetADT cur_event;

  for(cur_event = S->O; cur_event; cur_event = cur_event->link){
    rule_lst fanin, fanout;
    
    if(real_dummy(cur_event->event->action)){
      
      find_rules(cur_event->event, S, fanin, fanout);
	
      //rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
      
      bool fin_dummy=true, fout_dummy=true;
	
      for(rule_lst::iterator iter=fanin.begin(); iter!=fanin.end();iter++)
	if((!(iter->first->enabling->action->type == DUMMY)) &&
	   iter->first->enabling->action->label!=string("reset")){
	  fin_dummy=false;
	  break;
	}
      
      for(rule_lst::iterator iter=fanout.begin(); iter!=fanout.end(); iter++)
	if((!(iter->first->enabled->action->type == DUMMY))){
	  fout_dummy=false;
	  break;
	}
      
      if(fin_dummy==false || fout_dummy==false)
	continue;
      
      if(update(S, cur_event->event, fanin, fanout, 4, fromG, verbose)){
	modified=true;
	//rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
      }
    }
  }
    
  return modified;
}

bool xform_5(telADT S, bool fromG,bool verbose)
{
  bool modified = false;
  
  eventsetADT cur_event;

  for(cur_event = S->O; cur_event; cur_event = cur_event->link){
    rule_lst fanin, fanout;
    
    if(real_dummy(cur_event->event->action)){
      
      find_rules(cur_event->event, S, fanin, fanout);

      //rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
      if(update(S, cur_event->event, fanin, fanout, 5, fromG, verbose)){
	modified = true;
	//rm_self_lp(S, cur_event->event, fanin, fanout, verbose);
      }
    }
  }  
  return modified;
}



//This function ajust the initial marking after a dummy event is removed.
//After this function is called, if there are 2 rules (a -> c) and (b -> c),
//and a#b, and (a -> c) is an initial rule, then (b -> c) must be also an
//initial rule. This function corresponds to the function find_initial_marking,
//which needs the above semantics. If the function is changed, this function
//must be changed correspondingly. 
void adjust_initial_marking(telADT S,bool verbose)
{
  map<eventsADT, set<pair<rulesADT, bool> > > enabling_set;
  conflictsADT c;

  for(rulesetADT iter=S->R; iter; iter=iter->link)
    enabling_set[iter->rule->enabled].insert(make_pair(iter->rule, false));

  for(rulesetADT iter=S->Rp; iter; iter=iter->link)
    enabling_set[iter->rule->enabled].insert(make_pair(iter->rule, true));

  for(map<eventsADT, set<pair<rulesADT, bool> > >::iterator 
	i = enabling_set.begin(); i != enabling_set.end(); i++){
    set<pair<rulesADT,bool> > M_rules;

    for(set<pair<rulesADT,bool> >::iterator 
	  iter = i->second.begin(); iter != i->second.end(); iter++)
      for(set<pair<rulesADT,bool> >::iterator
	    iter_1 = i->second.begin(); iter_1 != i->second.end(); iter_1++) {
	if (iter->second==true && iter_1->second==true) continue;
	if (iter->second==false && iter_1->second==false) continue;
	if(!event_cmp(iter->first->enabling, iter_1->first->enabling) &&
	   iter->first->enabling->action->label != string("reset") &&
	   iter_1->first->enabling->action->label != string("reset") &&
	   M_rules.find(make_pair(iter->first,true))==M_rules.end() &&
	   M_rules.find(make_pair(iter_1->first,true))==M_rules.end()) {
	  c = check_C(S->C, iter->first->enabling, iter_1->first->enabling);
	  if(iter->second==true && iter_1->second==false &&
	     c && c->conftype & (ICONFLICT|OCONFLICT)){
	    S->R = rm_rules(S->R,  iter_1->first);
	    S->Rp = union_rules(S->Rp, create_rule_set(iter_1->first));
	    if (verbose)
	      cout << "If: "<< iter_1->first->enabling->action->label<<"  "
		   << iter_1->first->enabled->action->label << endl;
	    M_rules.insert(make_pair(iter_1->first, true));
	  }
	  else if(iter->second==false && iter_1->second==true &&
		  c && c->conftype & (ICONFLICT|OCONFLICT)){
	    S->R = rm_rules(S->R,  iter->first);
	    S->Rp = union_rules(S->Rp, create_rule_set(iter->first));
	    if (verbose)
	      cout << "Else: "<< iter->first->enabling->action->label<<"  "
		   << iter->first->enabled->action->label << endl;
	    M_rules.insert(make_pair(iter->first, true));
	  }
	}
      }
  }
}

bool check_conf(telADT S, eventsADT E1, eventsADT E2,conflictsADT E1_C_E2,
		bool verbose)
{
  set<eventsADT> Cset_11, Cset_22;
  for(conflictsetADT iter=S->C; iter; iter=iter->link){
    
    if(E1_C_E2 && iter->conflict==E1_C_E2)
      continue;

    if(iter->conflict->left==E1 && iter->conflict->right!=E2){
      Cset_11.insert(iter->conflict->right);
    }
    else if(iter->conflict->right==E1 && iter->conflict->left!=E2){
      Cset_11.insert(iter->conflict->left);
    }
    
    if(iter->conflict->left==E2 && iter->conflict->right!=E1){
      Cset_22.insert(iter->conflict->right);
    }
    else if(iter->conflict->right==E2 && iter->conflict->left!=E1){
      Cset_22.insert(iter->conflict->left);
    }
  }
  if(Cset_11 != Cset_22) {
    if (verbose)
      cout << "Events are in conflict with different sets of signals, skip"
	   << endl;
    return false;
  }    
  return true;
}


bool update_tel(telADT S, map<eventsADT, set<pair<eventsADT,bool> > >& EN_E,
		map<eventsADT, set<pair<eventsADT,bool> > >& ED_E,
		map<eventsADT, list<pair<rulesADT,bool> > >& EN_R,
		map<eventsADT, list<pair<rulesADT,bool> > >& ED_R,
		map<eventsADT,map<eventsADT, pair<int,int> > >& pre_time_bound,
		map<eventsADT,map<eventsADT, pair<int,int> > >& post_time_bound,		eventsADT E1, eventsADT E2, int MODE,bool fromG,bool verbose)
{
  bool modified = false;

  conflictsADT E1_C_E2 = check_C(S->C, E1, E2);
  /*
  if(!E1_C_E2) {
    if (verbose)
      cout << " Events not in conflict, skipping." << endl;
    return false;
  }
  */
  // For two events in conflict, they can be merged if they don't
  // have conflicts with different sets of signals other than themselves.
  if (!check_conf(S,E1,E2,E1_C_E2,verbose)) return false;

  if(MODE==1){

    if(pre_time_bound[E1]!=pre_time_bound[E2]){
      if(verbose)
	cout << "cannot merge "<<E1 << " and " << E2 
	     << " due to different time bounds in the preset [1].\n";

      return false;
    }

    if(post_time_bound[E1]!=post_time_bound[E2]){
      if(verbose)
	cout << "cannot merge "<<E1 << " and " << E2 
	     << " due to different time bounds in the postset [3].\n";

      return false;
    }


    for(list<pair<rulesADT,bool> >::iterator iter = EN_R[E2].begin();
	iter != EN_R[E2].end(); iter++){	    
      if(iter->second==false){
	S->R = rm_rules(S->R, iter->first);
      }
      else{
	S->Rp = rm_rules(S->Rp, iter->first);
      }
    }

    for(list<pair<rulesADT,bool> >::iterator iter = ED_R[E2].begin();
	iter != ED_R[E2].end(); iter++){
      if(iter->second)
	S->Rp = rm_rules(S->Rp, iter->first);
      else
	S->R = rm_rules(S->R, iter->first);
    }

    if (verbose)
      cout << "Remove duplicate rules 1 "<< E2->action->label
	   << "/"<<E2->occurrence<< endl;

    modified = true;

    rm_conflict(S, E2);

    S->O = subtract_events(S->O, create_event_set(E2));

    if (!fromG) adjust_initial_marking(S,verbose);

    if (verbose)
      emitters(0, itos(cnt++).c_str(), S);
  }

  else if (MODE==2){
    if(!E1_C_E2) {
      if (verbose)
	cout << "Events not in conflict, skipping." << endl;
      return false;
    }

    if(pre_time_bound[E1] != pre_time_bound[E2]){
      if(verbose)
	cout << "Cannot merge "<<E1 << " and " << E2 
	     << " due to different time bounds in the preset [2].\n";
      return false;
    }

    set<string> fanin_e;
  
    for(list<pair<rulesADT,bool> >::iterator iter = EN_R[E2].begin();
	iter != EN_R[E2].end(); iter++) {
      if (strcmp(iter->first->exp,"[true]")!=0) 
	fanin_e.insert(iter->first->exp);
    }

    // In Mode 2, if the enabling rules of sequencing have different levels,
    // they cannot be merged.
    if(fanin_e.size()>=1) {
      if (verbose)
	cout << " Has different levels, skipping." << endl;
      return false;
    }

    // Check that the postset of E1 is not a subset of the postset of E2,
    // and vice versa. Otherwise, return.
    set<pair<eventsADT,bool> > tmp1, tmp2;

    set_difference(EN_E[E1].begin(), EN_E[E1].end(),
		   EN_E[E2].begin(), EN_E[E2].end(),
		   inserter(tmp1, tmp1.begin()));

    set_difference(EN_E[E2].begin(), EN_E[E2].end(),
		   EN_E[E1].begin(), EN_E[E1].end(),
		   inserter(tmp2, tmp2.begin()));

    if((!tmp1.size() && tmp2.size()) || (tmp1.size() && !tmp2.size())){
       if (verbose)
	 cout << " postset(E1/E2) is a subset of postset(E2/E1), skipping." 
	      << endl;
      return false;
    }


    for(list<pair<rulesADT,bool> >::iterator iter = EN_R[E2].begin();
	iter != EN_R[E2].end(); iter++){	    
      if(iter->second==false){
	S->R = add_rule(S->R, E1, iter->first->enabled,
			iter->first->lower, iter->first->upper,
			iter->first->type, iter->first->dist);
	S->R = rm_rules(S->R, iter->first);
      }
      else{
	S->Rp = add_rule(S->Rp, E1, iter->first->enabled, 
			 iter->first->lower, iter->first->upper,
			 iter->first->type, iter->first->dist);
	S->Rp = rm_rules(S->Rp, iter->first);
      }
    }

    for(list<pair<rulesADT,bool> >::iterator iter = ED_R[E2].begin();
	iter != ED_R[E2].end(); iter++){
      for(list<pair<rulesADT,bool> >::iterator iter_1 = ED_R[E1].begin();
	  iter_1 != ED_R[E1].end(); iter_1++){
	if(iter->first->enabling == iter_1->first->enabling){
	  if(E1_C_E2){
	    if(iter_1->first->lower > iter->first->lower)
	      iter_1->first->lower = iter->first->lower;
	    else
	      if(iter_1->first->lower < iter->first->lower)
		iter_1->first->lower = iter->first->lower;
	  }
	  if(iter_1->first->upper < iter->first->upper)
	    iter_1->first->upper = iter->first->upper;
	}
      }

      if(iter->second)
	S->Rp = rm_rules(S->Rp, iter->first);
      else
	S->R = rm_rules(S->R, iter->first);
    }

    //Creates the conflicts between the postsets of E1 and E2.
    // 1. Get the intersection of the two sets;
    // 2. Minus the intersection from the two sets.
    // 3. Create conflicts for each pair of events from the two modifeid
    //    sets.
    // The first two setps avoid creating unnecessary conflicts.
    if(E1_C_E2){
      for(set<pair<eventsADT,bool> >::iterator
	    iter = tmp1.begin(); iter != tmp1.end(); iter++)
	for(set<pair<eventsADT,bool> >::iterator
	      iter1 = tmp2.begin(); iter1 != tmp2.end(); iter1++){

	  S->C = add_conflict(S->C, iter->first, iter1->first);
	  if (verbose)
	    cout << "(2) Adding conflict between " << iter->first
		 << " and " << iter1->first << endl;
	}
    }
    
    if (verbose)
      cout << "Remove duplicate rules 2 "<< E2->action->label
	   << "/"<<E2->occurrence<< endl;
    modified = true;
    rm_conflict(S, E2);
    S->O = subtract_events(S->O, create_event_set(E2));
    //S->A = rm_action(S->A, E2->action);
    if (!fromG) adjust_initial_marking(S,verbose);
    if (verbose)
      emitters(0, itos(cnt++).c_str(), S);
  }
  else if(MODE==3){

    if(post_time_bound[E1]!=post_time_bound[E2]){
      if(verbose)
	cout << "cannot merge "<<E1 << " and " << E2 
	     << " due to different time bounds in the postset [3].\n";

      return false;
    }

    // Check that the postset of E1 is not a subset of the postset of E2,
    // and vice versa. Otherwise, then check that the difference of the two
    // sets is in conflict with the common set of the two sets. Otherwise,
    // return.
    set<pair<eventsADT,bool> > tmp1, tmp2, common_set;

    set_difference(ED_E[E1].begin(), ED_E[E1].end(),
		   ED_E[E2].begin(), ED_E[E2].end(),
		   inserter(tmp1, tmp1.begin()));

    set_difference(ED_E[E2].begin(), ED_E[E2].end(),
		   ED_E[E1].begin(), ED_E[E1].end(),
		   inserter(tmp2, tmp2.begin()));

    if((!tmp1.size() && tmp2.size()) || (tmp1.size() && !tmp2.size())){

      set_intersection(ED_E[E2].begin(), ED_E[E2].end(),
		       ED_E[E1].begin(), ED_E[E1].end(),
		       inserter(common_set, common_set.begin()));      
      
      bool OK=false;
      if(tmp1.size()){
	for(set<pair<eventsADT,bool> >::iterator iter=tmp1.begin();
	    iter!=tmp1.end(); iter++){
	  for(set<pair<eventsADT,bool> >::iterator iter1=common_set.begin();
	      iter1!=common_set.end(); iter1++)
	    if(check_C(S->C, iter->first, iter1->first) ||
	       check_C(S->C, iter1->first, iter->first)){
	      OK = true;
	      break;
	    }
	  
	  if(OK==false){
	    if (verbose)
	      cout << " preset(E1/E2) is a subset of preset(E2/E1), skipping." 
		   << endl;
	    return false;
	  }
	}
      }
      else{
	for(set<pair<eventsADT,bool> >::iterator iter=tmp2.begin();
	    iter!=tmp2.end(); iter++){
	  for(set<pair<eventsADT,bool> >::iterator iter1=common_set.begin();
	      iter1!=common_set.end(); iter1++)
	    if(check_C(S->C, iter->first, iter1->first) ||
	       check_C(S->C, iter1->first, iter->first)){
	      OK = true;
	      break;
	    }
	  
	  if(OK==false){
	    if (verbose)
	      cout << " preset(E1/E2) is a subset of preset(E2/E1), skipping." 
		   << endl;
	    return false;
	  }
	}
      }
    }

    for(list<pair<rulesADT,bool> >::iterator iter = ED_R[E2].begin();
	iter != ED_R[E2].end(); iter++){	    
      if(iter->second){
	S->Rp = add_rule(S->Rp, iter->first->enabling, E1,
			 iter->first->lower, iter->first->upper,
			 iter->first->type, iter->first->dist);
	S->Rp = rm_rules(S->Rp, iter->first);
      }
      else{
	S->R = add_rule(S->R, iter->first->enabling, E1,
			iter->first->lower, iter->first->upper,
			iter->first->type, iter->first->dist);
	S->R = rm_rules(S->R, iter->first);
      }
    }
    //conflictsADT E1_C_E2 = check_C(S->C, E1, E2);
    for(list<pair<rulesADT,bool> >::iterator iter = EN_R[E2].begin();
	iter != EN_R[E2].end(); iter++){
      for(list<pair<rulesADT,bool> >::iterator iter_1 = EN_R[E1].begin();
	  iter_1 != EN_R[E1].end(); iter_1++){
	if(iter->first->enabled == iter_1->first->enabled){
	  //if(E1_C_E2)
	  if(iter_1->first->lower > iter->first->lower)
	    iter_1->first->lower = iter->first->lower;
	    //else
	    //if(iter_1->first->lower < iter->first->lower)
	    //iter_1->first->lower = iter->first->lower;
	  if(iter_1->first->upper < iter->first->upper)
	    iter_1->first->upper = iter->first->upper;
	}
      }
      if(iter->second)
	S->Rp = rm_rules(S->Rp, iter->first);
      else
	S->R = rm_rules(S->R, iter->first);
    }
    
    //Creates the conflicts between the presets of E1 and E2.
    // 1. Get the intersection of the two sets;
    // 2. Minus the intersection from the two sets.
    // 3. Create conflicts for each pair of events from the two modifeid
    //    sets.
    // The first two setps avoid creating unnecessary conflicts.
    if(E1_C_E2){
      for(set<pair<eventsADT,bool> >::iterator
	    iter = tmp1.begin(); iter != tmp1.end(); iter++)
	for(set<pair<eventsADT,bool> >::iterator
	      iter1 = tmp2.begin(); iter1 != tmp2.end(); iter1++){
	  S->C = add_conflict(S->C, iter->first, iter1->first);
	  if (verbose)
	    cout << "(3) Adding conflict between " << iter->first
		 << " and " << iter1->first << endl;
	}
    }
    
    modified = true;
	  
    if (verbose)
      cout << "Remove duplicate rules 3 "<< E2->action->label
	   << "/"<<E2->occurrence<< endl;
	  
    rm_conflict(S, E2);
    S->O = subtract_events(S->O, create_event_set(E2));
    //S->A = rm_action(S->A, E2->action);
    if (!fromG) adjust_initial_marking(S,verbose);
    if (verbose)
      emitters(0, itos(cnt++).c_str(), S);
  }
  //rm_conflict(S, E2);
  //S->O = subtract_events(S->O, create_event_set(E2));
  //S->A = rm_action(S->A, E2->action);
  //adjust_initial_marking(S,verbose);
  //emitters(0, itos(cnt++).c_str(), S);

  return modified;
}

void pre_merge(telADT S,
	       map<eventsADT, set<pair<eventsADT,bool> > >& en_events,
	       map<eventsADT, set<pair<eventsADT,bool> > >& ed_events,
	       map<eventsADT, list<pair<rulesADT,bool> > >& en_rules,
	       map<eventsADT, list<pair<rulesADT,bool> > >& ed_rules,
	       map<eventsADT,map<eventsADT, pair<int,int> > >& pre_time_bound,
	       map<eventsADT,map<eventsADT, pair<int,int> > >& post_time_bound,
	       set<pair<eventsADT,bool> >& dummy_E)
{
  for(rulesetADT cur_rule = S->R; cur_rule; cur_rule = cur_rule->link){
    eventsADT EN = cur_rule->rule->enabling;
    eventsADT ED = cur_rule->rule->enabled;
    
    if(EN->action->type == DUMMY){      
      en_events[EN].insert(make_pair(ED, false));
      en_rules[EN].push_front(make_pair(cur_rule->rule,false));
      post_time_bound[EN][ED] = make_pair(cur_rule->rule->lower,
					 cur_rule->rule->upper);
      pre_time_bound[ED][EN] = make_pair(cur_rule->rule->lower,
					  cur_rule->rule->upper);
      dummy_E.insert(make_pair(EN,true));
    }

    if(ED->action->type == DUMMY){
      if(EN->action->label != string("reset"))
	ed_events[ED].insert(make_pair(EN, false));
      ed_rules[ED].push_front(make_pair(cur_rule->rule, false)); 
      post_time_bound[EN][ED] = make_pair(cur_rule->rule->lower,
					 cur_rule->rule->upper);
      pre_time_bound[ED][EN] = make_pair(cur_rule->rule->lower,
					  cur_rule->rule->upper);
      dummy_E.insert(make_pair(ED, true));
    }
  }

  for(rulesetADT cur_rule = S->Rp; cur_rule; cur_rule = cur_rule->link){
    eventsADT EN = cur_rule->rule->enabling;
    eventsADT ED = cur_rule->rule->enabled;

    if (check_C(S->C, EN, ED)) continue;

    if(EN->action->type == DUMMY){
      en_events[EN].insert(make_pair(ED, true));
      en_rules[EN].push_front(make_pair(cur_rule->rule, true));
      post_time_bound[EN][ED] = make_pair(cur_rule->rule->lower,
					 cur_rule->rule->upper);
      pre_time_bound[ED][EN] = make_pair(cur_rule->rule->lower,
					  cur_rule->rule->upper);
    }
    if(ED->action->type == DUMMY){
      ed_events[ED].insert(make_pair(EN, true));
      ed_rules[ED].push_front(make_pair(cur_rule->rule, true)); 
      post_time_bound[EN][ED] = make_pair(cur_rule->rule->lower,
					 cur_rule->rule->upper);
      pre_time_bound[ED][EN] = make_pair(cur_rule->rule->lower,
					  cur_rule->rule->upper);
    }    
  }
}

//Removes the duplicate rules.
bool rm_dup_r(telADT S,bool fromG,bool verbose)
{
  if (verbose)
    cout << "Merging events..." << endl;
  map<eventsADT, set<pair<eventsADT,bool> > > en_events, ed_events;
  map<eventsADT, list<pair<rulesADT,bool> > > en_rules, ed_rules;
  map<eventsADT,map<eventsADT,pair<int,int> > > pre_time_bound, post_time_bound;  set<pair<eventsADT,bool> > dummy_E;

  pre_merge(S,en_events,ed_events,en_rules,ed_rules,
	    pre_time_bound,
	    post_time_bound,
	    dummy_E);

  for(set<pair<eventsADT,bool> >::iterator iter = dummy_E.begin();
      iter != dummy_E.end(); iter++)
    for(set<pair<eventsADT,bool> >::iterator iter_1 = iter;
	iter_1 != dummy_E.end(); iter_1++){
      if(!event_cmp(iter->first, iter_1->first)){	 
	
	if(en_events[iter->first].size() && en_events[iter_1->first].size() &&
	   ed_events[iter->first].size() && ed_events[iter_1->first].size() &&
	   en_events[iter->first] == en_events[iter_1->first] &&
	   ed_events[iter->first] == ed_events[iter_1->first]){
	  if (verbose)
	    cout << "(1:" << iter->first << ", "  << iter_1->first << ") "
		 << endl;
	  if (update_tel(S, en_events, ed_events, en_rules, ed_rules,
			 pre_time_bound,
			 post_time_bound,
			 iter->first, iter_1->first,1,fromG,verbose))
	    return true;
	}
	else if(ed_events[iter->first].size() && 
		ed_events[iter_1->first].size() &&
		ed_events[iter->first] == ed_events[iter_1->first]){
	  if (verbose)
	    cout << "(2:" << iter->first << ", "  << iter_1->first << ") "
		 << endl;
	  if (update_tel(S, en_events, ed_events, en_rules, ed_rules,
			 pre_time_bound,
			 post_time_bound,
			 iter->first, iter_1->first,2,fromG,verbose))
	    return true;
	}
	else if(en_events[iter->first].size() && 
		en_events[iter_1->first].size() &&
		en_events[iter->first] == en_events[iter_1->first]){
	  if (verbose)
	    cout << "(3:" << iter->first << ", "  << iter_1->first << ") "
		 << endl;
	  if (update_tel(S, en_events, ed_events, en_rules, ed_rules,
			 pre_time_bound,
			 post_time_bound,
			 iter->first, iter_1->first,3,fromG,verbose))
	    return true;
 	}
      }
    }
  return false;
}
 

//Removes the duplicate rules.
bool merge_C_events(telADT S,bool verbose)
{
  bool modified = false;

  list<conflictsADT> junk_c;

  for(conflictsetADT cur_C = S->C; cur_C; cur_C = cur_C->link){
    eventsADT left = cur_C->conflict->left;
    eventsADT right = cur_C->conflict->right;

    if((!(left->action->type == DUMMY)) || 
       (!(right->action->type == DUMMY)))
      continue;

    map<eventsADT, set<pair<eventsADT,bool> > > en_events, ed_events;
    map<eventsADT, list<pair<rulesADT,bool> > > en_rules, ed_rules;
    map<eventsADT,map<eventsADT, pair<rulesADT,bool> > > rule_set;
    set<pair<eventsADT,bool> > dummy_E;

    for(rulesetADT cur_rule = S->R; cur_rule; cur_rule = cur_rule->link){
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;

      rule_set[EN][ED] = make_pair(cur_rule->rule,false);

      if(EN->action->type == DUMMY){      
	en_events[EN].insert(make_pair(ED, false));
	en_rules[EN].push_front(make_pair(cur_rule->rule,false));
	dummy_E.insert(make_pair(EN,true));	
      }
      if(ED->action->type == DUMMY){
	if(EN->action->label != string("reset"))
	  ed_events[ED].insert(make_pair(EN, false));
	ed_rules[ED].push_front(make_pair(cur_rule->rule, false)); 
	dummy_E.insert(make_pair(ED, true));
      }
    }

    for(rulesetADT cur_rule = S->Rp; cur_rule; cur_rule = cur_rule->link){
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;

      rule_set[EN][ED] = make_pair(cur_rule->rule,true);

      if(EN->action->type == DUMMY){      
	en_events[EN].insert(make_pair(ED, true));
	en_rules[EN].push_front(make_pair(cur_rule->rule, true));
      }
      if(ED->action->type == DUMMY){
	ed_events[ED].insert(make_pair(EN, true));
	ed_rules[ED].push_front(make_pair(cur_rule->rule, true)); 
      }    
    }

    set<pair<eventsADT,bool> > en_set_l, ed_set_l, en_set_r, ed_set_r;
    en_set_l = ed_events[left];  en_set_r = ed_events[right];
    ed_set_l = en_events[left];  ed_set_r = en_events[right];
    
    list<pair<rulesADT,bool> > rules_len, rules_ren, rules_led, rules_red;
    rules_len = ed_rules[left]; rules_ren = ed_rules[right]; 
    rules_led = en_rules[left]; rules_red = en_rules[right]; 

    if(real_dummy(left->action) && real_dummy(right->action) &&
       en_events[left] != en_events[right] &&
       ed_events[left] != ed_events[right]){

      //cout << left->action->label<<" # "<< right->action->label<<endl;

      set<pair<eventsADT,bool> > ld_en, rd_en, ld_ed, rd_ed;

      set_difference(en_set_l.begin(), en_set_l.end(),
		     en_set_r.begin(), en_set_r.end(),
		     inserter(ld_en, ld_en.begin()));

      set_difference(en_set_r.begin(), en_set_r.end(),
		     en_set_l.begin(), en_set_l.end(),
		     inserter(rd_en, rd_en.begin()));
      
      if(ld_en.size() != rd_en.size()){
	cout << "ERROR: postproc.cc, line 887\n";
	continue;
      }

      set_difference(en_events[left].begin(), en_events[left].end(),
		     en_events[right].begin(), en_events[right].end(),
		     inserter(ld_ed, ld_ed.begin()));

      set_difference(en_events[right].begin(), en_events[right].end(),
		     en_events[left].begin(), en_events[left].end(),
		     inserter(rd_ed, rd_ed.begin()));

      if(ld_ed.size() != rd_ed.size()){
	cout << "ERROR: postproc.cc, line 900\n";
	break;
      }

      bool proceed1=true;
      for(set<pair<eventsADT,bool> >::iterator i1=ld_ed.begin();
          i1!=ld_ed.end(); i1++)
        for(set<pair<eventsADT,bool> >::iterator i2=rd_ed.begin();
            i2!=rd_ed.end(); i2++)
	  if(check_C(S->C, i1->first, i2->first) && i1->second != i2->second){
	     proceed1= false;
	     break;
	  }

      bool proceed2=true;
      for(set<pair<eventsADT,bool> >::iterator i1=ld_en.begin();
          i1!=ld_en.end(); i1++)
        for(set<pair<eventsADT,bool> >::iterator i2=rd_en.begin();
            i2!=rd_en.end(); i2++)
          if(check_C(S->C, i1->first, i2->first) && i1->second != i2->second){
	    proceed2 = false;
	    break;
          }

      if(!proceed1 || !proceed2)
	continue;

      for(set<pair<eventsADT,bool> >::iterator i1=ld_en.begin(); 
	  i1!=ld_en.end(); i1++)
	for(set<pair<eventsADT,bool> >::iterator i2=ld_ed.begin(); 
	    i2!=ld_ed.end(); i2++){
	  rulesADT r1 = rule_set[i1->first][left].first;
	  rulesADT r2 = rule_set[left][i2->first].first;

	  if(!r1 || !r2){
	    cout << " Fatal error line 935\n";
	    break;
	  }

	  if(i1->second || i2->second)
	    S->Rp = add_rule(S->Rp, i1->first, i2->first, r1->lower+r2->lower,
			     r1->upper + r2->upper, r1->type, r1->dist);
          else
            S->R = add_rule(S->R, i1->first, i2->first, r1->lower + r2->lower,
			    r1->upper + r2->upper, r1->type, r1->dist);
	}

      for(set<pair<eventsADT,bool> >::iterator i1=rd_en.begin(); 
	  i1!=rd_en.end(); i1++)
	for(set<pair<eventsADT,bool> >::iterator i2=rd_ed.begin(); 
	    i2!=rd_ed.end(); i2++){

	  rulesADT r1 = rule_set[i1->first][right].first;
	  rulesADT r2 = rule_set[right][i2->first].first;

	  if(!r1 || !r2){
	    cout << " Fatal error line 956\n";
	    break;
	  }

	  if(i1->second || i2->second)
	    S->Rp = add_rule(S->Rp, i1->first, i2->first, r1->lower+r2->lower,
			     r1->upper + r2->upper, r1->type, r1->dist);
          else
            S->R = add_rule(S->R, i1->first, i2->first, r1->lower + r2->lower,
			    r1->upper + r2->upper, r1->type, r1->dist); 
	}

      for(list<pair<rulesADT,bool> >::iterator I = rules_ren.begin();
	  I != rules_ren.end(); I++){
	if(I->second){
	  S->Rp = add_rule(S->Rp, I->first->enabling, left, I->first->lower,
			   I->first->upper, I->first->type, I->first->dist,
			   I->first->exp);
	  S->Rp = rm_rules(S->Rp, I->first);
	}
	else{
	  S->R = add_rule(S->R, I->first->enabling, left, I->first->lower,
			  I->first->upper, I->first->type, I->first->dist,
			  I->first->exp);
	  S->R = rm_rules(S->R, I->first);
        }
      }
      for(list<pair<rulesADT,bool> >::iterator I = rules_red.begin();
	  I != rules_red.end(); I++){
	if(I->second){
          S->Rp = add_rule(S->Rp, left, I->first->enabled, I->first->lower,
                           I->first->upper, I->first->type, I->first->dist,
                           I->first->exp);
	  S->Rp = rm_rules(S->Rp, I->first);
        }
        else{
          S->R = add_rule(S->R, left, I->first->enabled, I->first->lower,
                          I->first->upper, I->first->type, I->first->dist,
                          I->first->exp);
	  S->R = rm_rules(S->R, I->first);
        }
      }

      junk_c.push_back(cur_C->conflict);
      
      S->O = subtract_events(S->O, create_event_set(right));
      //S->A = rm_action(S->A, right->action);
      if (verbose)
	emitters(0, itos(cnt++).c_str(), S);
    }
  }

  for(list<conflictsADT>::iterator I=junk_c.begin(); I!=junk_c.end(); I++)
    S->C = subtract_conflicts(S->C, create_conflict_set(*I));

  //emitters(0, itos(cnt++).c_str(), S);

  return modified;
}
 

telADT post_proc(telADT S, bool redchk, bool verbose, bool fromG, 
		 bool xform2,bool VHDLorHSE )
{

  cout << "Post processing ..."; 

  //clear_C(S);

  set<pair<rulesADT,bool> > ordering_rules;
  
  for(rulesetADT cur_rule= S->R; cur_rule; cur_rule=cur_rule->link)
    if(cur_rule->rule->type==ORDERING)
      /*cur_rule->rule->enabling->action->type==DUMMY &&
	cur_rule->rule->enabled->action->type==DUMMY)*/
      ordering_rules.insert(make_pair(cur_rule->rule, false));
  
  for(rulesetADT cur_rule= S->Rp; cur_rule; cur_rule=cur_rule->link)
    if(cur_rule->rule->type==ORDERING)
      /*       cur_rule->rule->enabling->action->type==DUMMY &&
	       cur_rule->rule->enabled->action->type==DUMMY)*/
      ordering_rules.insert(make_pair(cur_rule->rule, true));

  for(set<pair<rulesADT,bool> >::iterator iter=ordering_rules.begin();
      iter !=ordering_rules.end(); iter++){
    
    if(iter->second)
      S->Rp = rm_rules(S->Rp, iter->first);
    else
      S->R = rm_rules(S->R, iter->first);
  }

  if (verbose)
    emitters(0, "preproc", S);
  
  if(S == 0)
    return S;

  // First, remove all dummy events which have a single fanin or single fanout.
  // After processing patter-1 is done, remove all self loops created in
  // patter-1; then remove any dummy events which match patter-1.  
  while(1){

#ifdef XFORM_4_5    
    if(S->C && !VHDLorHSE) {
      while(rm_dup_r(S,fromG,verbose));
    }
#endif    

    bool XB_1 = false;
    while(1){
      // Remove dummy's with single fanin "place" and single fanout "place".
      // This is a special case of xfrom 1 and xfrom 6 in Hao's thesis.
      bool M_1 = false;
#ifdef XFORM_1_6
      M_1 = xform_1(S,fromG,verbose);    
#endif

      bool M_2 = false;
#ifdef XFORM_3
      M_2 = rm_self_lp(S,verbose);
#endif

      bool M_RR = false;
#ifdef RM_RED
      if(redchk && S->C && !VHDLorHSE)  M_RR = rm_red(S,verbose);
#endif

      if(M_1 || M_2 || M_RR)
	XB_1 = true;
      
      if(M_1==false && M_2==false && M_RR == false)
	break;
    }
    
    bool XB_2 = false;
    while(1){
      // This is Y - V tranform also known as general xform 1 in thesis.
      bool M_3 = false;
#ifdef XFORM_1
      if(!(M_3 = xform_2(S,fromG,verbose))) break;
#else
      break;
#endif

      bool M_RR = false;
#ifdef RM_RED
      if(redchk && S->C && !VHDLorHSE) M_RR = rm_red(S,verbose);
#endif

      bool M_1 = false;
#ifdef XFORM_1_6
      //      bool M_1 = xform_1(S,verbose);    
#endif

      bool M_2 = false;
#ifdef XFORM_3
      M_2 = rm_self_lp(S,verbose);
#endif

      if(M_3 || M_RR || M_1 || M_2)
        XB_2 = true;
      
      if(M_3==false && M_RR==false && M_1 == false && M_2 == false)
	break;
    }

    bool XB_3 = false;
    while(1){
      // This is xform 2 in Hao's thesis.
      bool M_3 = false;
#ifdef XFORM_2
      if (xform2)
	if((M_3 = xform_3(S,fromG,verbose)) == false) break;
#else
      break;
#endif

      bool M_RR = false;
#ifdef RM_RED
      if(redchk && S->C && !VHDLorHSE)  M_RR = rm_red(S,verbose);
#endif

      bool M_1 = false;
#ifdef XFORM_1_6
      //      bool M_1 = xform_1(S,verbose);    
#endif

      bool M_2 = false;
#ifdef XFORM_3
      M_2 = rm_self_lp(S,verbose);
#endif

      if(M_3 || M_RR || M_1 || M_2)
        XB_3 = true;

      if(M_3== false &&	 M_RR == false && M_1 == false && M_2 == false)
	break;
    }
    bool M_RR = false;
#ifdef RM_RED
    if(redchk && !S->C && !VHDLorHSE) M_RR = rm_red(S,verbose);
#endif

    bool M_RR2 = false;
    if(S->C)
      ;//M_RR2 = rm_red2(S);

    if(XB_1 == false && XB_2 == false && XB_3 == false && 
       M_RR == false && M_RR2==false)
      break;

    // xforms 4 and 5 in Hao's thesis
#ifdef XFORM_4_5    
    if(!S->C && !VHDLorHSE) {
      while(rm_dup_r(S,fromG,verbose));
    }
#endif
    /*
    bool M_4 = false;
    if(!(M_4 = xform_4(S,fromG,verbose))) break;
    bool M_5 = false;
    if(!(M_5 = xform_5(S,fromG,verbose))) break;
    */
  }

  /*  while(1){
    while(rm_dup_r(S,fromG,verbose));
    
    bool M_1 = xform_1(S,verbose);    
    bool M_2 = xform_2(S,verbose);
    bool M_3 = xform_3(S,verbose);
    bool M_RR = rm_red(S,verbose);
    bool M_RR2 = rm_red2(S);

    if(!M_1 && !M_2 && !M_3 && !M_RR && !M_RR2)
      break;
  }
  */
  if (!fromG) adjust_initial_marking(S,verbose);

  for(set<pair<rulesADT,bool> >::iterator iter=ordering_rules.begin();
      iter !=ordering_rules.end(); iter++){

    if((iter->first->enabling->action->type==DUMMY || 
	iter->first->enabling->action->label==string("reset")) &&
       iter->first->enabled->action->type==DUMMY)    
      continue;

    if(iter->second)
      S->Rp = add_rule(S->Rp, iter->first);
    else
      S->R = add_rule(S->R, iter->first);
  }


  // Interactive mode.
#ifdef PP_INTERACTIVE
  
  while(1){

    interactive = 1;

    cout << "postproc > ";
    
    string command;
    cin >> command;
    
    if(command=="quit" || command=="q" || command=="Q")
      break;
    else if(command=="merge")
      rm_dup_r(S,fromG,verbose);
    else if(command=="reduce1" || command=="r1")
      xform_1(S,fromG,verbose); 
    else if(command=="reduce2" || command=="r2")
      xform_2(S,fromG,verbose); 
    else if(command=="reduce3" || command=="r3")
      xform_3(S,fromG,verbose); 
    else;
  }
    
#endif
  
  return 0;
}


bool is_removable(telADT S, eventsADT dummy, set<eventsADT> D_conflict_set)
{
  if(D_conflict_set.size() == 1){
    if((*D_conflict_set.begin())->action->type == DUMMY)
      return true;
    return false;
  }      
  
  map<eventsADT, set<eventsADT> > fanin, fanout;

  for(rulesetADT cur_rule = S->R; cur_rule; cur_rule = cur_rule->link){
    if(cur_rule->rule->enabling->action->label != string("reset")){
      eventsADT EN = cur_rule->rule->enabling;
      eventsADT ED = cur_rule->rule->enabled;
      fanin[ED].insert(EN);
      fanout[EN].insert(ED);
    }
  }
 
  for(rulesetADT cur_rule = S->Rp; cur_rule; cur_rule = cur_rule->link){
    eventsADT EN = cur_rule->rule->enabling;
    eventsADT ED = cur_rule->rule->enabled;
    fanin[ED].insert(EN);
    fanout[EN].insert(ED);
  }

  for(set<eventsADT>::const_iterator iter=D_conflict_set.begin();
      iter!=D_conflict_set.end(); iter++){
    for(set<eventsADT>::const_iterator iter_1=D_conflict_set.begin(); 
	iter_1!=D_conflict_set.end(); iter_1++){
      if((*iter != *iter_1) && !check_C(S->C, *iter, *iter_1)){
	set<eventsADT> tmp1, tmp2, tmp3, tmp4;

	set_intersection(fanin[dummy].begin(), fanin[dummy].end(),
			 fanin[*iter].begin(), fanin[*iter].end(),
			 inserter(tmp1, tmp1.begin()));
	set_intersection(fanin[dummy].begin(), fanin[dummy].end(),
			 fanin[*iter_1].begin(), fanin[*iter_1].end(),
			 inserter(tmp2, tmp2.begin()));
	set_intersection(fanout[dummy].begin(), fanout[dummy].end(),
			 fanout[*iter].begin(), fanout[*iter].end(),
			 inserter(tmp3, tmp3.begin()));
	set_intersection(fanout[dummy].begin(), fanout[dummy].end(),
			 fanout[*iter_1].begin(), fanout[*iter_1].end(),
			 inserter(tmp4, tmp4.begin()));

	if((tmp1.size() && tmp4.size()) || (tmp2.size() && tmp3.size()))
	  return false;
      }  
    }
  }
  return true;
}


// Remove the dummy action, event, and all rules containing the dummy event
// from the 'tel'. 
// Add news rules from 'pred' enabling -> 'succ' enabled.
// if it creats a self loop, then, remove the self loop, and update the delay
// of all other fanin rules of the 'dummy'. 
bool update(telADT S, eventsADT dummy, const rule_lst& fanin, 
	    const rule_lst& fanout, int mode, bool fromG,bool verbose)
{
  telADT S_old = TERScopy(S);

  //Sets of expressions of fanin rules and fanout rules.
  set<string> fanin_e, fanout_e;
  
  //If all fanin and fanout rules are non-initial rules, fin_ini and fout_ini
  //are false.
  bool fin_ini=false, fout_ini=false;

  for(rule_lst::const_iterator iter=fanin.begin(); iter!=fanin.end(); iter++){
    fanin_e.insert(iter->first->exp);
    if(iter->second==true)
      fin_ini=true;

    if(check_C(S_old->C, iter->first->enabling, iter->first->enabled) ||
       check_C(S_old->C, iter->first->enabled, iter->first->enabling)) {
      if (verbose)
	cout << dummy->action->label 
	     << " cannot be removed because conflicts with fanin." 
	     << endl; 
      return false;
    }
  }

  for(rule_lst::const_iterator iter=fanout.begin(); iter!=fanout.end();iter++){
    fanout_e.insert(iter->first->exp);
    if(iter->second==true)
      fout_ini=true;
    
    if(check_C(S_old->C, iter->first->enabling, iter->first->enabled) ||
       check_C(S_old->C, iter->first->enabled, iter->first->enabling)) {
      if (verbose)
	cout << dummy->action->label 
	     << " cannot be removed because conflicts with fanout." 
	     << endl; 
      return false;
    }
  }

  conflictsetADT new_cs2 = find_conflict(S->C, dummy);
  /* This is a check for not a PN tranformations */
  for(conflictsetADT cur_c=new_cs2; cur_c; cur_c = cur_c->link){
    eventsADT target;
    
    if(event_cmp(cur_c->conflict->left, dummy))
      target = cur_c->conflict->right;
    else
      target = cur_c->conflict->left;     
    
    //if(check_rule(S->R, dummy, target) ||
    //   check_rule(S->Rp, dummy, target))
    //continue;
    
    rule_lst C_fanin, C_fanout;
    int i1, i2;
    
    find_rules(target, S_old, C_fanin, C_fanout, i1, i2);
    
    bool en_common = false;
    bool ed_common = false;

    for(rule_lst::iterator iter= C_fanin.begin(); iter!=C_fanin.end();
	iter++){
      for(rule_lst::const_iterator iter_1= fanin.begin();
	  iter_1!=fanin.end();iter_1++)
	if(iter->first->enabling->action->label != string("reset") &&
	   iter_1->first->enabling->action->label != string("reset") &&
	   event_cmp(iter->first->enabling, iter_1->first->enabling)){
	  en_common = true;
	  break;
	}
      if(en_common)
	break;
    }

    for(rule_lst::iterator iter= C_fanout.begin();iter!=C_fanout.end();
	iter++){
      for(rule_lst::const_iterator iter_1=fanout.begin();
	  iter_1!=fanout.end();iter_1++)
	if(event_cmp(iter->first->enabled, iter_1->first->enabled)){
	  ed_common = true;
	  break;
	}
      if(ed_common)
	break;
    }
    
    if ((en_common) && (ed_common)) {
      if (verbose)
	cout << dummy->action->label 
	     << " cannot be removed because not a PN transformation (1)." 
	     << endl; 
      return false;
    }
    if(en_common)
      for(rule_lst::const_iterator iter=fanout.begin();iter!=fanout.end();
	  iter++){
	if(!check_C(S->C, target, iter->first->enabled) &&
	   !event_cmp(target, iter->first->enabled)){
	  if (verbose)
	    cout << dummy->action->label 
		 << " cannot be removed because not a PN transformation (2)." 
		 << endl; 
	  return false;
	}
      }
  }
  /* TO HERE */

  /* This tries to prevent new incorrect conflicts from being introduce 
     through postprocessing.  This is a limitation with TELs expressiveness.
     This would not be necessary for an PN */
  for(rule_lst::const_iterator iter=fanout.begin();iter!=fanout.end();iter++){
    conflictsetADT new_cs3 = find_conflict(S->C, iter->first->enabled);
    for(conflictsetADT cur_c=new_cs3; cur_c; cur_c = cur_c->link) {
      bool okay=false;
      eventsADT target1;
      eventsADT target2;
      if(event_cmp(cur_c->conflict->left, iter->first->enabled))
	target1 = cur_c->conflict->right;
      else
	target1 = cur_c->conflict->left;
      for(rule_lst::const_iterator iter_2=fanout.begin();
	  iter_2!=fanout.end();iter_2++)
	if (event_cmp(target1,iter_2->first->enabled)) {
	  okay=true;
	  break;
	}
      if (!okay) {
	for(conflictsetADT cur_c2=new_cs2; cur_c2; cur_c2 = cur_c2->link) {
	  if(event_cmp(cur_c2->conflict->left, dummy))
	    target2 = cur_c2->conflict->right;
	  else
	    target2 = cur_c2->conflict->left;     
	  if (event_cmp(target1,target2)) {
	    okay=true;
	    break;
	  }
	}
      }
      if (!okay) {
	rule_lst C_fanin, C_fanout;
	rule_lst E_fanin, E_fanout;
	int i1, i2, i3, i4;
	//cout << "Checking (" << dummy->action->label << ") "  
	//     << target1->action->label << " with "
	//     << iter->first->enabled->action->label << endl;
	find_rules(target1, S_old, C_fanin, C_fanout, i1, i2);
	find_rules(dummy, S_old, E_fanin, E_fanout, i3, i4);
	for(rule_lst::iterator iter_2= C_fanin.begin(); iter_2!=C_fanin.end();
	    iter_2++){
	  for(rule_lst::const_iterator iter_1= E_fanin.begin();
	      iter_1!=E_fanin.end();iter_1++) {
	    if ((iter_2->first->enabling==iter_1->first->enabling) &&
		(strcmp(iter_1->first->enabling->action->label,"reset")!=0)) {
	      if (verbose)
		cout << dummy->action->label 
		     << " cannot be removed due to TEL limitation and fanin " 
		     << iter_2->first->enabling << " and target " 
		     << target1 << endl; 
	      return false;
	    }
	  }
	}
	//cout << "OKAY" << endl;
      }
    }
  }
  /* TO HERE */

  //if(fout_ini)  return false;

  bool modified=false;
  
  if(((fanin_e.size()==1 && *(fanin_e.begin())==string("[true]")) &&
      (fanout_e.size()==1 && *(fanout_e.begin())==string("[true]"))) &&
     !(fin_ini && fout_ini)){
    modified=true;

    for(rule_lst::const_iterator iter_1=fanin.begin(); iter_1!=fanin.end(); 
	iter_1++)
      for(rule_lst::const_iterator iter_2=fanout.begin(); iter_2!=fanout.end();
	  iter_2++){

	int lower = 0; int upper = 0;

	rulesADT RR1 = iter_1->first;
	rulesADT RR2 = iter_2->first;

	if(RR1->lower == INFIN || RR2->lower == INFIN)
	  lower = INFIN;
	else
	  lower = RR1->lower + RR2->lower;
	
	if(RR1->upper == INFIN || RR2->upper == INFIN)
	  upper = INFIN;
	else
	  upper = RR1->upper + RR2->upper;
   
	string level = logic(RR1->exp, RR2->exp);

	if(((iter_1->second==true && iter_2->second==false) ||
	    (iter_1->second==false && iter_2->second==true)) &&
 	   RR1->enabling->action->label != string("reset")) {

	  rulesADT rr=check_rule(S->R, RR1->enabling, RR2->enabled);
	  if(!rr)
	    S->Rp = add_rule(S->Rp, RR1->enabling, RR2->enabled,
			     lower,upper,RR2->type, RR1->dist, 
			     level.c_str()
			     /*RR1->exp*/);
	    
	  else	    
	    if(upper > rr->upper)
	      rr->upper = upper;
	}
	else if((RR1->enabling->action->label==string("reset")) || 
		(iter_1->second==false  && iter_2->second==false)){
	  rulesADT rp=check_rule(S->Rp, RR1->enabling, RR2->enabled);
	  if(!rp) {
	    S->R = add_rule(S->R, RR1->enabling, RR2->enabled,
			    lower, upper, RR2->type, RR1->dist, 
			    level.c_str()
			    /*RR1->exp*/);
	  
	  } else{
	    int ub=(upper > rp->upper) ? upper : rp->upper;  
	    S->R = add_rule(S->R, RR1->enabling, RR2->enabled,
			    lower, ub, RR2->type, RR1->dist, 
			    level.c_str()
			    /*RR1->exp*/);
	    rm_rules(S->Rp, rp);
	  }  
	}
	else
	  cout <<"Error ???"<< endl;
      }
      
    // Remove all conflicts containing the 'dummy', and add new conflicts 
    // between the events enabled by 'dummy' and the events which are
    // in conflict with 'dummy' before.
    conflictsetADT new_cs = find_conflict(S->C, dummy);
    for(conflictsetADT cur_c=new_cs; cur_c; cur_c = cur_c->link){
      eventsADT target;
	  
      if(event_cmp(cur_c->conflict->left, dummy))
	target = cur_c->conflict->right;
      else
	target = cur_c->conflict->left;     

      if(cur_c->conflict->left==cur_c->conflict->right ||
	 check_rule(S->R, dummy, target) ||
	 check_rule(S->Rp, dummy, target))
	//check_rule(S->Rp, target, dummy))
	continue;

      rule_lst C_fanin, C_fanout;
      int i1, i2;

      find_rules(target, S_old, C_fanin, C_fanout, i1, i2);
      

      bool en_common = false;
      bool ed_common = false;

      for(rule_lst::iterator iter= C_fanin.begin(); iter!=C_fanin.end();
	  iter++){
	for(rule_lst::const_iterator iter_1= fanin.begin();
	    iter_1!=fanin.end();iter_1++)
	  if(iter->first->enabling->action->label != string("reset") &&
	     iter_1->first->enabling->action->label != string("reset") &&
	     event_cmp(iter->first->enabling, iter_1->first->enabling)){
	    en_common = true;
	    break;
	  }
	if(en_common)
	  break;
      }

      for(rule_lst::iterator iter= C_fanout.begin();iter!=C_fanout.end();
	  iter++){
        for(rule_lst::const_iterator iter_1=fanout.begin();
	    iter_1!=fanout.end();iter_1++)
          if(event_cmp(iter->first->enabled, iter_1->first->enabled)){
            ed_common = true;
            break;
          }
        if(ed_common)
          break;
      }

      /*if(!en_common && !ed_common){
	for(rule_lst::const_iterator iter=fanout.begin();iter!=fanout.end();
	    iter++)
	  if(!check_C(S->C, target, iter->first->enabled) &&
	     !event_cmp(target, iter->first->enabled))
	    S->C = add_conflict(S->C, target, iter->first->enabled);
      
	//if(ed_common)
        for(rule_lst::const_iterator iter=fanin.begin();iter!=fanin.end();
	    iter++)
          if(iter->first->enabling->action->label!=string("reset") &&
	     !check_C(S->C, target, iter->first->enabling) &&
	     !event_cmp(target, iter->first->enabling))
            S->C = add_conflict(S->C, target, iter->first->enabling);
      }
      else*/ {
	if(en_common)// || check_rule(S->Rp, target, dummy)){
	  for(rule_lst::const_iterator iter=fanout.begin();iter!=fanout.end();
	      iter++){
	    if(!check_C(S->C, target, iter->first->enabled) &&
	       !event_cmp(target, iter->first->enabled)){
	      S->C = add_conflict(S->C, target, iter->first->enabled,
				  (ICONFLICT|OCONFLICT));
	      if (verbose)
		cout << "New conflictO: "<< target->action->label<< " # "
		     << iter->first->enabled->action->label<< endl;
	    }
	  }
	if(ed_common)
	  for(rule_lst::const_iterator iter=fanin.begin();iter!=fanin.end();
	      iter++)
	    if(iter->first->enabling->action->label!=string("reset") &&
	       !check_C(S->C, target, iter->first->enabling) &&
	       !event_cmp(target, iter->first->enabling)){
	      S->C = add_conflict(S->C, target, iter->first->enabling,
				  (ICONFLICT|OCONFLICT));
	      if (verbose)
		cout << "new conflictI: "<< target->action->label<< " # "
		     << iter->first->enabling->action->label<< endl;
	    }	
      }
    }
	   
    if(new_cs){
      S->C = subtract_conflicts(S->C, new_cs);
      delete_conflict_set(new_cs);
    }
     
    for(rule_lst::const_iterator iter = fanin.begin(); iter!=fanin.end();
	iter++)
      if(iter->second == true)
	//S->Rp = subtract_rules(S->Rp, create_rule_set(iter->first));
	S->Rp=rm_rules(S->Rp, iter->first);
      
      else
	//S->R = subtract_rules(S->R, create_rule_set(iter->first));
	S->R=rm_rules(S->R, iter->first);
    
    for(rule_lst::const_iterator iter = fanout.begin(); iter!=fanout.end(); 
	iter++)
      if(iter->second == true)
	//S->Rp = subtract_rules(S->Rp, create_rule_set(iter->first));
	S->Rp= rm_rules(S->Rp, iter->first);
      
      else
	//S->R = subtract_rules(S->R, create_rule_set(iter->first));
	S->R=rm_rules(S->R, iter->first);
    
    //S->I = subtract_events(S->I, create_event_set(dummy));
    //S->O = subtract_events(S->O, create_event_set(dummy));
    //S->A = rm_action(S->A, dummy->action);    
  }
  
  if(verbose && modified){
    cout<< "Update "<<mode<< " "
	<<dummy->action->label<<'/'<<dummy->occurrence<< " "<< endl;
      //<<itos(cnt++).c_str()<<endl;

    //while(rm_dup_r(S,fromG,verbose));
    if((S->C)&&(!fromG)){
	adjust_initial_marking(S,verbose);
    }
    emitters(0, itos(cnt++).c_str(), S);
  }
  
  TERSdelete(S_old);

  return modified;
}

