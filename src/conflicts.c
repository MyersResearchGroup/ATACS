/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* conflicts.c - miscellaneous routines used to manipulate conflict sets.    */
/*****************************************************************************/

#include "struct.h"
#include "ly.h"
#include "hash.h"
#include "events.h"
#include "conflicts.h"
#include <map>
#include <string>

map<string, conflictsADT> *conflicttable = 0;

struct conflict_Htables
{
  conflict_Htables()
  {
    for(int i = 0; i < TABLESIZE; i++){
      names[i] = string();
      conflicttables[i] = 0;
    }
    count = 0;
  }

  ~conflict_Htables()
  {
    free_conflict_table();
  }

  int count;
  string names[TABLESIZE];
  map<string, conflictsADT> *conflicttables[TABLESIZE];
} Ctables;


// Find the conflict table with the given name
void new_conflict_table(const char *name)
{	 
  for (int i = 0; i < TABLESIZE; i++)
    if(Ctables.names[i].size() == 0 || Ctables.names[i] == name){
      // If tere is not a conflict table for the given componet,
      // create a new one, otherwise, return the conflict table.
      if(Ctables.names[i].size() == 0){
	Ctables.names[i] = name;
	Ctables.conflicttables[i] = new map<string, conflictsADT>; 
	Ctables.count++;
      }
      conflicttable = Ctables.conflicttables[i];
      break;
    }  
}

/*****************************************************************************/
/* Initialize hash table used for conflicts.                                 */
/*****************************************************************************/

void init_conflict_table(void)
{
  int i;

  for (i=0;i<TABLESIZE;i++)
    ;//conflicttable[i]=NULL;
}

/*****************************************************************************/
/* Free hash table used for conflicts.                                       */
/*****************************************************************************/

void free_conflict_table(void)
{
  for(int i = 0; i < Ctables.count; i++) {	
    if(Ctables.names[i].size()){
      map<string, conflictsADT>::iterator begin;       // Free memory
      for(begin = Ctables.conflicttables[i]->begin();  // used by keys
	  begin != Ctables.conflicttables[i]->end();   // and values.
	  begin++)
	delete begin->second;
	    
      delete Ctables.conflicttables[i]; //Free the conflict table pointer
	  
      Ctables.names[i] = string();
      Ctables.conflicttables[i] = 0;
    }
  }
}

/*****************************************************************************/
/* Enter conflict into conflict table, and initialize conflict.              */
/*****************************************************************************/

conflictsADT conflict(eventsADT l,eventsADT r,
		      int conftype)
{
  char lr[1000];
  // = new char[strlen(l->action->label)+strlen(r->action->label)+10];

  sprintf(lr,"%s%d,%s%d\n",l->action->label,l->occurrence,
	  r->action->label,r->occurrence);
  
  string ss = lr;
  
  conflictsADT entry = (*conflicttable)[ss]; 
  if(entry == 0)                    // If the conflict with the given string
    {                               // is not found, add a new conflict with
      entry = new conflict_tag;     // events 'l' and 'r' into the conflict
      entry->left = l;              // table.
      entry->right = r;
      entry->conftype = conftype;
      (*conflicttable)[ss] = entry;
    }

  return entry;    
}

void print_c(void)
{
  for(map<string, conflictsADT>::iterator i=conflicttable->begin();
      i!=conflicttable->end(); i++)
    cout << i->first << "   " 
	 << i->second->left->action->label<<'/'<< i->second->left->occurrence
	 << "  " 
	 << i->second->right->action->label<<'/'<<i->second->right->occurrence
	 << endl;
}

void print_c(conflictsetADT C)
{
  conflictsetADT cur_conflict;

  for(cur_conflict = C; cur_conflict; cur_conflict = cur_conflict->link)
    cout << cur_conflict->conflict->left->action->label<<'/'
	 << cur_conflict->conflict->left->occurrence
	 << "  " 
	 << cur_conflict->conflict->right->action->label<<'/'
	 <<cur_conflict->conflict->right->occurrence
	 << endl;
}  


// Check if there is a conflict in the set C between the events l and r.
// If not, return 0.
conflictsADT check_C(conflictsetADT C, eventsADT l, eventsADT r)
{
  for(conflictsetADT iter=C; iter; iter= iter->link){
    /*if((event_cmp(iter->conflict->left, l) && 
	event_cmp(iter->conflict->right, r)) ||
       (event_cmp(iter->conflict->right, l) && 
       event_cmp(iter->conflict->left,r)))*/
    if((iter->conflict->left==l && iter->conflict->right==r) ||
       (iter->conflict->right==l && iter->conflict->left==r))
      return iter->conflict;
  }
  return 0;       
}


conflictsADT check(eventsADT l, eventsADT r)
{
  char lr[1000], rl[1000];
  sprintf(lr,"%s%d,%s%d\n",l->action->label,l->occurrence,
	  r->action->label,r->occurrence);  
  sprintf(rl,"%s%d,%s%d\n",r->action->label,r->occurrence,
	  l->action->label,l->occurrence);

  if(conflicttable->find(lr) != conflicttable->end())
    return (*conflicttable)[lr];
  
  if(conflicttable->find(rl) != conflicttable->end())
    return (*conflicttable)[rl];
  
  return 0;
}

/*****************************************************************************/
/* Create a conflict set C with conflict "c" as the only member.             */
/*****************************************************************************/

conflictsetADT create_conflict_set(conflictsADT c)
{
  conflictsetADT C;

  C = new conflictset_tag; //(conflictsetADT)GetBlock(sizeof(*C));
  C->conflict = c;
  C->link = 0;
  
  return (C);
}

/*****************************************************************************/
/* Find a conflict in the table and add to the conflict set C.               */
/*****************************************************************************/

conflictsetADT add_conflict(conflictsetADT C,eventsADT l,eventsADT r,
			    int conftype)
{
  conflictsADT c;
  conflictsetADT oldC;

  c = conflict(l,r,conftype);
  oldC = C;
  C = create_conflict_set(c);
  C->link = oldC;
  
  return (C);
}


conflictsetADT add_conflict(conflictsetADT C, conflictsADT c)
{
  conflictsetADT oldC = C;
  C = create_conflict_set(c);
  C->link = oldC;
  
  return (C);
}


// FInd all conflict containing event 'E' from conflict set 'C'.
conflictsetADT find_conflict(conflictsetADT C, eventsADT E)
{
  conflictsetADT cur_c, new_cs;

  cur_c = C;
  new_cs = 0;

  while (cur_c){
    if((cur_c->conflict->left->action->label==string(E->action->label)&&
	cur_c->conflict->left->occurrence==E->occurrence) ||
       (cur_c->conflict->right->action->label==string(E->action->label)&&
	cur_c->conflict->right->occurrence==E->occurrence)){
      if(new_cs==0)
	new_cs = create_conflict_set(cur_c->conflict);
      else	 
	new_cs = add_conflict(new_cs, cur_c->conflict);
    }
     
    cur_c = cur_c->link;
  }
  return new_cs;
}

/*****************************************************************************/
/* Delete a conflict set C.                                                  */
/*****************************************************************************/

void delete_conflict_set(conflictsetADT C)
{
  if(!C)
    return;

  conflictsetADT cur_conflict, next_conflict;

  cur_conflict = C;
  while(cur_conflict){
    next_conflict = cur_conflict->link;
    delete cur_conflict;
    cur_conflict = next_conflict;
  }
}

/*****************************************************************************/
/* Copy a set of conflicts (i.e., C1:=C0).                                   */
/*****************************************************************************/

conflictsetADT copy_conflicts(conflictsetADT C0)
{
  conflictsetADT C1, oldC, cur_conflict;
  
  C1 = 0;
  for (cur_conflict = C0; cur_conflict; cur_conflict = cur_conflict->link)
    {
      oldC = C1;
      C1 = create_conflict_set(cur_conflict->conflict);
      C1->link = oldC;
    }
  
  return (C1);
}

/*****************************************************************************/
/* Check if a conflict c is in a set of conflicts C.                         */
/*****************************************************************************/

bool in_conflict_set(conflictsADT c,conflictsetADT C)
{
  conflictsetADT cur_conflict;

  if (C) 
    for(cur_conflict = C; cur_conflict; cur_conflict = cur_conflict->link)
      if((cur_conflict->conflict->left==c->left &&
	  cur_conflict->conflict->right==c->right) ||
	 (cur_conflict->conflict->left==c->right &&
	  cur_conflict->conflict->right==c->left))
	return (TRUE);

  return (FALSE);
}

/*****************************************************************************/
/* Take the union of two sets of conflicts.                                  */
/*****************************************************************************/

conflictsetADT union_conflicts(conflictsetADT C0,conflictsetADT C1)
{
  conflictsetADT C, oldC, cur_conflict;
  
  C = copy_conflicts(C0);
  for(cur_conflict = C1; cur_conflict; cur_conflict = cur_conflict->link)
    if(!in_conflict_set(cur_conflict->conflict,C)) 
      {
	oldC = C;
	C = create_conflict_set(cur_conflict->conflict);
	C->link = oldC;
      }
  
  return (C);
}

conflictsetADT link_conflict(conflictsetADT C0,conflictsetADT C1)
{
  if(!C0) return C1;

  conflictsetADT next_C = C0;
  while(next_C->link)
    next_C = next_C->link;
  
  next_C->link = C1;

  return C0;
}

/*****************************************************************************/
/* Subtract one (C1) set of conflicts from another (C0).                   */
/*****************************************************************************/

conflictsetADT subtract_conflicts(conflictsetADT C0,conflictsetADT C1)
{
  conflictsetADT oldC, C, CC, cur_conflict;
  //  conflictsetADT head;

  C = 0;
  for(cur_conflict = C0; cur_conflict; 
      oldC = cur_conflict, cur_conflict = cur_conflict->link)
    if(!in_conflict_set(cur_conflict->conflict,C1)){
      oldC = C; 
      C = create_conflict_set(cur_conflict->conflict);
      C->link = oldC;
    }

  // For debugging purpose. Need to be removed in the future.
  CC = 0;
  for(cur_conflict = C; cur_conflict; cur_conflict = cur_conflict->link){
      oldC = CC;
      CC = create_conflict_set(cur_conflict->conflict);
      CC->link = oldC;
  }


  /*
  head = cur_conflict = C0;
  int begin = 1;
  while(cur_conflict){
    if(in_conflict_set(cur_conflict->conflict,C1)){
      conflictsetADT next = cur_conflict->link;
      delete cur_conflict;
      cur_conflict = next;
      if(begin)
	head = cur_conflict;
      begin=0;
    }
    else
      cur_conflict = cur_conflict->link;
      }*/
  /*  cout << head << "  " << C0 << endl;
  for(cur_conflict = C1; cur_conflict; cur_conflict = cur_conflict->link)
    cout << cur_conflict->conflict->left->action->label<< endl;
  */
  return (CC);
}

/*****************************************************************************/
/* Rename events in a set of conflicts C1 so as not to clash with set E0.    */
/*****************************************************************************/

conflictsetADT rename_conflict_set(eventsetADT E0,conflictsetADT C1)
{
  eventsADT l, r;
  conflictsetADT cur_conflict;

  for(cur_conflict = C1; cur_conflict; cur_conflict = cur_conflict->link)
    {
      l = rename_event(E0,cur_conflict->conflict->left);
      r = rename_event(E0,cur_conflict->conflict->right);
      cur_conflict->conflict = conflict(l,r);
    }
  
  return(C1);
}
