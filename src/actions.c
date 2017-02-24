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
/* actions.c - miscellaneous routines to manipulate sets of actions.         */
/*****************************************************************************/

#include "ly.h"
#include "hash.h"
#include "actions.h"
#include <strings.h>
#include "auxilary.h"
#include <map>

#include <string>

map<string, actionADT> *actiontable = 0; 

struct action_Htables
{
  action_Htables() 
  {
    for(int i = 0; i < TABLESIZE; i++)
      {
	names[i] = 0;
	actiontables[i] = 0;
      }
    count = 0;
  }

  ~action_Htables()
  {
    free_action_table();
  }
  int count;
  char *names[TABLESIZE];
  map<string, actionADT> *actiontables[TABLESIZE];
} Atables;


// Find a action table for the given entity.
void new_action_table(const char *name)
{
  for (int i = 0; i < TABLESIZE; i++)
    if (Atables.names[i] == 0 || strcmp(Atables.names[i],name)==0){
	// If there is not an action table for the given component, 
	// create a new one. Otherwise, return the action table.
      if (Atables.names[i] == 0) {
	Atables.names[i] = Tolower(name);
	Atables.actiontables[i] = new map<string, actionADT>;
	Atables.count++;	    
      }
      actiontable = Atables.actiontables[i];
      action("reset", 5);
      break;
    } 
}


/*****************************************************************************/
/* Initialize hash table used for actions.                                   */
/*****************************************************************************/

void init_action_table(void)
{
  //int i;

  //for (i=0;i<TABLESIZE;i++)
  //  actiontable[i]=NULL;
  
  action("reset", 5);
}

/*****************************************************************************/
/* Free hash table used for actions.                                         */
/*****************************************************************************/

void free_action_table(void)
{
  for(int i = 0; i < TABLESIZE; i++)
    {	
      if(Atables.names[i])
	{
	  delete[] Atables.names[i];

	  map<string, actionADT>::iterator begin;       // Free memory
	  for(begin = Atables.actiontables[i]->begin();  // used by keys
	      begin != Atables.actiontables[i]->end();   // and values.
	      begin++)
	    {
	      if (begin->second->label) free(begin->second->label);
	      delete begin->second;
	    }

	  delete Atables.actiontables[i]; //Free the conflict table pointer
	  
	  Atables.names[i] = 0;
	  Atables.actiontables[i] = 0;
	}
    } 
}

/*****************************************************************************/
/* Enter action into action table, and initialize action.                    */
/*****************************************************************************/
// ZHEN(Done): add an assignment to the assignment list for an action
assignList addAssign(assignList list_assigns, string var, string assign) 
{
  assignList temp;
  temp = list_assigns;
  list_assigns = (assignList)GetBlock(sizeof(*list_assigns));
  list_assigns->var = CopyString(var.c_str());
  int assignInt = 0;
  for (int i = 0; i < assign.size(); i++) {
    assignInt = 2*assignInt;
    if (assign[i]=='1') {
      assignInt += 1;
    }
  }
  std::string assignStr = std::to_string(assignInt);
  list_assigns->assign = CopyString(assignStr.c_str());
  list_assigns->next = temp;
  return list_assigns;
}

actionADT action(const string& SS)
{
  return action(SS.c_str(), SS.size());
}

actionADT action(const char* a, int len)
{
  actionADT entry = (*actiontable)[a];
  if(entry == 0) { 
    entry = new action_tag;
    entry->type = UNKNOWN;
    entry->label = CopyString(a);
    entry->initial = FALSE;
    entry->lower = 0;
    entry->upper = INFIN;
    entry->dist = NULL;
    entry->vacuous = FALSE;
    entry->odata = 0;
    entry->idata = 0;
    entry->delay.lr = 0;
    entry->delay.ur = INFIN;
    entry->delay.lf = 0;
    entry->delay.uf = INFIN;
    entry->maxoccur = 0;
    entry->inverse = 0;
    // ZHEN(Done): create empty assignment list
    entry->list_assigns = NULL;
    (*actiontable)[a] = entry;
  }
  return (entry);
}

/*****************************************************************************/
/* Create an action set A with action "a" as the only member (i.e., A:={a}). */
/*****************************************************************************/

actionsetADT create_action_set(actionADT a)
{
  actionsetADT A;

  A = new actionset_tag;
  A->action = a;
  A->link = NULL;
  
  return (A);
}

/*****************************************************************************/
/* Delete an action set A.                                                   */
/*****************************************************************************/

void delete_action_set(actionsetADT A)
{
  actionsetADT cur_action, next_action;

  cur_action = A;
  while(cur_action) {
    next_action = cur_action->link;
    delete cur_action;
    cur_action = next_action;
  }
}


actionsetADT rm_action(actionsetADT Aset, actionADT A)
{
  actionsetADT cur_action, parent=0;

  for(cur_action=Aset; cur_action; cur_action=cur_action->link){
    if(cur_action->action==A){
      if(!parent){	
	actionsetADT ret=cur_action->link;
	delete cur_action;
	return ret;
      }
      else{	
	parent->link = cur_action->link;
	delete cur_action;
	return Aset;
      }
    }
    parent=cur_action;
  }
  return Aset;
}

/*****************************************************************************/
/* Copy a set of actions (i.e., A1:=A0).                                     */
/*****************************************************************************/

actionsetADT copy_actions(actionsetADT A0)
{
  actionsetADT A1, oldA, cur_action;
  
  A1 = 0;
  for(cur_action = A0; cur_action; cur_action = cur_action->link) 
    {
      oldA = A1;
      A1 = create_action_set(cur_action->action);
      A1->link = oldA;
    }
  
  return (A1);
}

/*****************************************************************************/
/* Check if an action a is in a set of actions A.                            */
/*****************************************************************************/

bool in_action_set(actionADT a,actionsetADT A)
{
  actionsetADT cur_action;

  if(A)
    for (cur_action=A; cur_action; cur_action=cur_action->link)
      if (cur_action->action==a) 
	return (TRUE);
  
  return (FALSE);
}

/*****************************************************************************/
/* Take the union of two sets of actions.                                    */
/*****************************************************************************/

actionsetADT union_actions(actionsetADT A0,actionsetADT A1)
{
  actionsetADT A, oldA, cur_action;
  
  A = copy_actions(A0);
  for(cur_action = A1; cur_action; cur_action = cur_action->link)
    if(!in_action_set(cur_action->action,A)) 
      {
	oldA = A;
	A = create_action_set(cur_action->action);
	A->link = oldA;
      }
  
  return (A);
}
