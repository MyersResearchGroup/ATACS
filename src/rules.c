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
/* rules.c - miscellaneous routines used to manipulate rule sets.            */
/*****************************************************************************/

#include "struct.h"
#include "ly.h"
#include "hash.h"
#include "events.h"
#include "rules.h"
#include <map>
#include <string>

map<string, rulesADT> *ruletable; 

struct rule_Htables
{
  rule_Htables() 
  {
    for (int i = 0; i < TABLESIZE; i++)
      {
	names[i] = 0;
	ruletables[i] = 0;
      }

    count = 0;
  }

  ~rule_Htables()
  {
    free_rule_table();
  }

  int count;
  char *names[TABLESIZE];
  map<string, rulesADT> *ruletables[TABLESIZE];
} Rtables;


// Find the rule table with the given name
void new_rule_table(const char *name)
{	 
  for (int i = 0; i < TABLESIZE; i++)
    if (Rtables.names[i] == 0 || strcmp(Rtables.names[i], name) == 0)
      {
	// If there is not a rule table for the given component,
	// create a new one, otherwise, return the rule table.
	if (Rtables.names[i] == 0)
	  {
	    Rtables.names[i] = CopyString(name);
	    Rtables.ruletables[i] = new map<string, rulesADT>;
	    Rtables.count++;
	  }

	ruletable = Rtables.ruletables[i];

	break;
      }
}


/*****************************************************************************/
/* Initialize hash table used for rules.                                     */
/*****************************************************************************/

void init_rule_table(void)
{
  int i;

  for (i=0;i<TABLESIZE;i++)
    ;//ruletable[i]=NULL;
}

/*****************************************************************************/
/* Free hash table used for rules.                                           */
/*****************************************************************************/

void free_rule_table(void)
{
  for(int i=0; i < Rtables.count; i++)
    {
      if(Rtables.names[i])
	{
	  free(Rtables.names[i]);

	  map<string, rulesADT>::iterator begin;        // Free memory
	  for(begin = Rtables.ruletables[i]->begin();  // used by keys
	      begin != Rtables.ruletables[i]->end();   // and values.
	      begin++)
	    {
	      //	      delete begin->first;
	      //if (begin->second->exp) free(begin->second->exp);
	      delete begin->second;
	    }

	  delete Rtables.ruletables[i];  //Free the rule table pointer
	  
	  Rtables.names[i] = 0;
	  Rtables.ruletables[i] = 0;
	}
    }
}

/*****************************************************************************/
/* Enter rule into rule table, and initialize rule.                          */
/*****************************************************************************/

rulesADT rule(eventsADT e, eventsADT f, int l, int u,int type, bool rgulr,
	      char *dist, const char *level, const char *assign)
{
  char ef[1000]; 
  // = new char[strlen(e->action->label)+strlen(f->action->label)+10];
  
  sprintf(ef,"%s%d,%s%d,%d,%d\n", e->action->label, e->occurrence,
	  f->action->label, f->occurrence,l,u);

  string ss = ef;

  rulesADT entry = (*ruletable)[ss]; 
  if(entry == 0){                   // If the rule with the given string
                                    // is not found, add a new rule with
    entry = new rule_tag;         // enabling events 'e' and enabled 
    entry->enabling = e;          // event 'f' into the ruletable.
    entry->enabled = f;
    entry->lower = l;
    entry->upper = u;
    entry->dist = CopyString(dist);
    entry->exp = CopyString(level);
    entry->assign = CopyString(assign);
    entry->type = type;
    entry->regular = rgulr;
    (*ruletable)[ss] = entry;
  } else {
    //cout << "Found rule " << e->action->label << " -> "
    //	 << f->action->label << " updating entries." << endl;
    entry->lower = entry->lower > l ? l : entry->lower;
    entry->upper = entry->upper < u ? u : entry->upper;
    entry->dist = CopyString(dist);
    entry->exp = CopyString(level);
    entry->assign = CopyString(assign);
    entry->type = type;
    entry->regular = rgulr;
  }
  return entry;
}

/*****************************************************************************/
/* Create an rule set R with rule "r" as the only member (i.e., R:={r}).     */
/*****************************************************************************/

rulesetADT create_rule_set(rulesADT r)
{
  rulesetADT R;

  R = new ruleset_tag;
  R->rule = r;
  R->link = 0;

  return (R);
}

/*****************************************************************************/
/* Find rule in table and add to the rule set R.                             */
/*****************************************************************************/

rulesetADT add_rule(rulesetADT R, rulesADT r)
{
  rulesetADT oldR;

  oldR = R;
  R = create_rule_set(r);
  R->link = oldR;
  
  return (R);
}


rulesetADT add_rule(rulesetADT R, eventsADT e, eventsADT f, int l, int u,
		    int type, char * dist, const char *level,
		    const char* assign)
{
  rulesADT r;
  rulesetADT oldR;
  rulesADT tmp = check_rule(R, e, f);

  if(!tmp){  
    if(f->action->type == IN)  // this is a enviornment rule, set 'regular' 0.
      r = rule(e, f, l, u, type, false, dist, level, assign);
    else
      r = rule(e, f, l, u, type, true, dist, level, assign);

    r->lower = (r->lower > l) ? l : r->lower;  
    r->upper = (r->upper < u) ? u : r->upper;
  
    oldR = R;
    R = create_rule_set(r);
    R->link = oldR;
  }
  else{
    tmp->lower = (tmp->lower > l) ? l : tmp->lower;
    tmp->upper = (tmp->upper < u) ? u : tmp->upper;
  }

  return (R);
}


rulesADT check_rule(rulesetADT R, eventsADT e, eventsADT f)
{
  /*  char ef[1000]; 

  sprintf(ef,"%s%d,%s%d\n", e->action->label, e->occurrence,
	  f->action->label, f->occurrence);

  string ss = ef;

  map<string, rulesADT>::iterator iter=ruletable->find(ef);
  if(iter != ruletable->end() && in_rule_set(iter->second, R))
  return iter->second;*/

  for(rulesetADT iter=R; iter; iter=iter->link)
    if(event_cmp(iter->rule->enabling, e) && event_cmp(iter->rule->enabled, f))
      return iter->rule;

  
  return 0;
}

/*****************************************************************************/
/* Delete a rule set R.                                                      */
/*****************************************************************************/

void delete_rule_set(rulesetADT R)
{
  rulesetADT cur_rule, next_rule;

  cur_rule = R;
  while(cur_rule) 
    {
      next_rule = cur_rule->link;
      delete cur_rule;
      cur_rule = next_rule;
    }
}

/*****************************************************************************/
/* Copy a set of rules (i.e., R1:=R0).                                       */
/*****************************************************************************/

rulesetADT copy_rules(rulesetADT R0)
{
  rulesetADT R1, oldR, cur_rule;
  
  R1 = 0;
  for(cur_rule = R0; cur_rule; cur_rule = cur_rule->link) 
    {
      oldR = R1;
      R1 = create_rule_set(cur_rule->rule);
      R1->link = oldR;
    }
  
  return(R1);
}

/*****************************************************************************/
/* Check if a rule r is in a set of rules R.                                 */
/*****************************************************************************/

bool in_rule_set(rulesADT r,rulesetADT R)
{
  rulesetADT cur_rule;

  if(R) 
    for(cur_rule=R; cur_rule; cur_rule=cur_rule->link)
      if(cur_rule->rule == r) 
	return(TRUE);

  return (FALSE);
}

/*****************************************************************************/
/* Take the union of two sets of rules.                                    */
/*****************************************************************************/

rulesetADT union_rules(rulesetADT R0,rulesetADT R1)
{
  rulesetADT R, oldR, cur_rule;
  
  R = copy_rules(R0);
  for(cur_rule = R1; cur_rule; cur_rule = cur_rule->link)
    if(!in_rule_set(cur_rule->rule,R)) 
      {
	oldR = R;
	R = create_rule_set(cur_rule->rule);
	R->link = oldR;
      }
  
  return (R);
}


/*****************************************************************************/
/* Subtract one set of rules from another.                                  */
/*****************************************************************************/

rulesetADT subtract_rules(rulesetADT R0,rulesetADT R1)
{
  rulesetADT oldR, R, cur_rule;

  R = 0;
  for(cur_rule = R0; cur_rule; cur_rule = cur_rule->link)
    if(!in_rule_set(cur_rule->rule,R1))
      {
	oldR = R;
	R = create_rule_set(cur_rule->rule);
	R->link = oldR;
      }

  delete_rule_set(R0);
  delete_rule_set(R1);
  
  return (R);
}

rulesetADT rm_rules(rulesetADT R, rulesADT r)
{  
  rulesetADT parent = 0;
  
  for(rulesetADT cur_rule=R; cur_rule; cur_rule=cur_rule->link){
    if(cur_rule->rule == r){
      if(!parent){	
	rulesetADT RS = cur_rule->link;
	delete cur_rule;
	return RS;
      }
      else{	
	parent->link = cur_rule->link;
	delete cur_rule;
	return R;
      }    
    }
    parent = cur_rule;
  }
  return(R);
}

/*****************************************************************************/
/* Rename events in a set of rules R1 so as not to clash with event set E0.  */
/*****************************************************************************/

rulesetADT rename_rule_set(eventsetADT E0, rulesetADT R1)
{
  eventsADT e, f;
  rulesetADT cur_rule;

  for(cur_rule = R1; cur_rule; cur_rule = cur_rule->link) 
    {
      e = rename_event(E0, cur_rule->rule->enabling);
      f = rename_event(E0, cur_rule->rule->enabled);
      cur_rule->rule = rule(e, f, cur_rule->rule->lower, 
			    cur_rule->rule->upper,cur_rule->rule->type,
			    cur_rule->rule->regular,cur_rule->rule->dist,
			    cur_rule->rule->exp,cur_rule->rule->assign);
    }
  
  return(R1);
}


void find_rules(eventsADT E, TERstructADT S,
		rule_lst& fanin, rule_lst& fanout, 
		int& fin_size, int& fout_size)
{
  fin_size=fout_size=0;

  rulesetADT rs = S->R;
  while(rs && rs->rule/* && rs->rule->type!=ORDERING*/) {

    if(E == rs->rule->enabled){
      fanin.push_back(make_pair(rs->rule, false));
      if(rs->rule->enabling->action->label!=string("reset"))
	fin_size++;
    }
	
    if(E == rs->rule->enabling){
      fanout.push_back(make_pair(rs->rule, false));
      if(rs->rule->enabled->action->label!=string("reset"))
	fout_size++;
    }
    
    rs = rs->link;
  }
  
  rs = S->Rp;
  while(rs && rs->rule /*&& rs->rule->type!=ORDERING*/) {
    
    if(E == rs->rule->enabled){
      fanin.push_back(make_pair(rs->rule, true));
      if(rs->rule->enabling->action->label!=string("reset"))
	fin_size++;
    }
    
    if(E == rs->rule->enabling){
      fanout.push_back(make_pair(rs->rule, true));
      if(rs->rule->enabled->action->label!=string("reset"))
	fout_size++;
    }
    
    rs = rs->link;
  }
}


void find_rules(eventsADT E, TERstructADT S,
		rule_lst& fanin, rule_lst& fanout) 
{
  rulesetADT rs = S->R;
  while(rs && rs->rule && rs->rule->type!=ORDERING) {
    
    if(E == rs->rule->enabled)
      fanin.push_back(make_pair(rs->rule, false));
	
    if(E == rs->rule->enabling)
      fanout.push_back(make_pair(rs->rule, false));
    
    rs = rs->link;
  }
  
  rs = S->Rp;
  while(rs && rs->rule && rs->rule->type!=ORDERING) {
    
    if(E == rs->rule->enabled)
      fanin.push_back(make_pair(rs->rule, true));
    
    if(E == rs->rule->enabling)
      fanout.push_back(make_pair(rs->rule, true));
    
    rs = rs->link;
  }
}
