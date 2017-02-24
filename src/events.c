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
/* events.c - miscellaneous routines used to manipulate event sets.          */
/*****************************************************************************/

#include "struct.h"
#include "ly.h"
#include "hash.h"
#include "events.h"
#include "auxilary.h"
#include <map>
#include <string>

map<string, eventsADT> *eventtable = 0; 

struct event_Htables
{
  event_Htables() 
  {
    for(int i = 0; i < TABLESIZE; i++)
      {
	names[i] = 0;
	eventtables[i] = 0;
      }

    count = 0;
  }

  ~event_Htables()
  {
    free_event_table();
  }

  int count;
  char *names[TABLESIZE];
  map<string, eventsADT> *eventtables[TABLESIZE];
} Etables;


// Find the event table with the given name
void new_event_table(const char *name)
{	 
  for(int i = 0; i < TABLESIZE; i++)
    if (Etables.names[i] == 0 || strcmp(Etables.names[i], name) == 0)
      {
	// if there is not an event table for the given component, 
	// create a new one. Otherwise, return the event table.
	if (Etables.names[i] == 0)
	  {
	    Etables.names[i] = CopyString(name);
	    Etables.eventtables[i] = new map<string, eventsADT>;
	    Etables.count++;
	  }
	eventtable = Etables.eventtables[i];
	event((actionADT)action("reset",5), 0, 0, 0,NULL);

	break;
      }  
}


/*****************************************************************************/
/* Initialize hash table used for events.                                    */
/*****************************************************************************/

void init_event_table(void)
{
  int i;

  for (i=0;i<TABLESIZE;i++)
    ;//eventtable[i]=NULL;
  
  event((actionADT)action("reset",5), 0, 0, 0, NULL);
}

/*****************************************************************************/
/* Free hash table used for events.                                          */
/*****************************************************************************/

void free_event_table(void)
{
  for(int i = 0; i < TABLESIZE; i++)
    {	
      if(Etables.names[i])
	{
	  free(Etables.names[i]);
	  
	  map<string, eventsADT>::iterator begin;        // Free memory
	  for(begin = Etables.eventtables[i]->begin();  // used by keys
	      begin != Etables.eventtables[i]->end();   // and values.
	      begin++)
	    {
	      //delete begin->first;
	      if (begin->second->exp) free(begin->second->exp);
	      delete begin->second;
	    }

	  delete Etables.eventtables[i]; //Free the conflict table pointer
	  
	  Etables.names[i] = 0;
	  Etables.eventtables[i] = 0;
	}
    }
}

/*****************************************************************************/
/* Enter event into event table, and initialize event.                       */
/*****************************************************************************/

eventsADT event(actionADT a, int occur, int lower, int upper, char *dist,
		const char *level,bool fixed_occur,
		const string& data)
{
  char *ao = new char[strlen(a->label)+40];
  
  sprintf(ao,"%s%d%s\n", a->label, occur, data.c_str());

  string ss = ao;
  
/* TEMP */
  if (a->maxoccur < occur) a->maxoccur = occur;

  eventsADT entry = (*eventtable)[ss]; 
  
  if(entry == 0) {
    entry = new event_tag;
    entry->exp = NULL;
  }
  else if ((entry->exp) && (strcmp(entry->exp,level)!=0)) {
    //free(entry->exp);
    entry->exp=NULL;
  }

  entry->action = a;
  entry->occurrence = occur;
  entry->fixed_occur = fixed_occur;
  entry->lower = lower;
  entry->upper = upper;
  entry->dist = CopyString(dist);
  if (!(entry->exp) || (strcmp(entry->exp,level)!=0)) {
    if (level) {
      if (level[0] == '[') {
	entry->exp = CopyString(level);
	entry->assign = CopyString("[true]");
      }
      else {
	entry->exp = CopyString("[true]");
	entry->assign = CopyString(level);
      }
    }
    else {
      entry->exp = CopyString(level);
      entry->assign = CopyString(level);
    }
  }
  (*eventtable)[ss] = entry;
  entry->data = data;
  delete[] ao;

  return entry;  
}


// If e1 is the same as the e2, the function returns true; otherwise, returns
// false.
bool event_cmp(eventsADT e1, eventsADT e2)
{
  if((strcmp(e1->action->label,e2->action->label)==0) &&
     (e1->occurrence==e2->occurrence))
    return true;
 
  return false;
}

eventsADT check_event(const string& a,int occur)
{
  string ao=a + itos(occur);
  return (*eventtable)[ao]; 
}

/*****************************************************************************/
/* Override the default delay for an event.                                  */
/*****************************************************************************/

void delay_override(eventsADT e,int lower,int upper)
{
  e->lower = lower;
  e->upper = upper;
}

/*****************************************************************************/
/* Create an event set A with event "e" as the only member (i.e., E:={e}).   */
/*****************************************************************************/

eventsetADT create_event_set(eventsADT e)
{
  eventsetADT E;

  E = new eventset_tag;
  E->event = e;
  E->link = 0;

  return (E);
}

/*****************************************************************************/
/* Delete an event set E.                                                    */
/*****************************************************************************/

void delete_event_set(eventsetADT E)
{
  eventsetADT cur_event, next_event;

  cur_event = E;
  while(cur_event)
    {
      next_event = cur_event->link;
      delete cur_event;
      cur_event = next_event;
    }
}

/*****************************************************************************/
/* Copy a set of events (i.e., E1:=E0).                                      */
/*****************************************************************************/

eventsetADT copy_events(eventsetADT E0)
{
  eventsetADT E1, oldE, cur_event;
  
  E1 = 0;
  for(cur_event = E0; cur_event; cur_event = cur_event->link) 
    {
      oldE = E1;
      E1 = create_event_set(cur_event->event);
      E1->link = oldE;
    }
  
  return (E1);
}

/*****************************************************************************/
/* Check if an event a is in a set of events E.                              */
/*****************************************************************************/

bool in_event_set(eventsADT e,eventsetADT E)
{
  eventsetADT cur_event;

  if(E) 
    for(cur_event = E; cur_event; cur_event = cur_event->link)
      if (cur_event->event == e)
	return (TRUE);
  
  return (FALSE);
}

/*****************************************************************************/
/* Take the union of two sets of events.                                    */
/*****************************************************************************/

eventsetADT union_events(eventsetADT E0, eventsetADT E1)
{
  eventsetADT E, oldE, cur_event;
  
  E = copy_events(E0);
  for(cur_event = E1; cur_event; cur_event = cur_event->link)
    if(!in_event_set(cur_event->event,E)) 
      {
	oldE = E;
	E = create_event_set(cur_event->event);
	E->link = oldE;
      }
  
  return (E);
}


eventsetADT link_event(eventsetADT E0, eventsetADT E1)
{
  if(!E0) return E1;

  eventsetADT next_E = E0;
  while(next_E->link)
    next_E = next_E->link;
  next_E->link = E1;
  return E0;
}


/*****************************************************************************/
/* Subtract one set of events E1 from another E0.                            */
/*****************************************************************************/

eventsetADT subtract_events(eventsetADT E0,eventsetADT E1)
{
  eventsetADT oldE, E, cur_event;

  E = 0;
  for(cur_event = E0; cur_event; cur_event = cur_event->link)
    if(!in_event_set(cur_event->event,E1)){
      oldE = E;
      E = create_event_set(cur_event->event);
      E->link = oldE;
    }  
  return (E);
}

/*****************************************************************************/
/* Check if an event is in a set E, and if so rename it.                     */
/*****************************************************************************/

eventsADT rename_event(eventsetADT E,eventsADT e)
{
  eventsetADT cur_event;
  int maxoccur;

  if (e->fixed_occur) return e;
  maxoccur = 0;
  for(cur_event = E; cur_event; cur_event = cur_event->link)
    if((cur_event->event->action == e->action) && 
       (cur_event->event->occurrence > maxoccur))
      maxoccur = cur_event->event->occurrence;
  return(event(e->action, e->occurrence+maxoccur, 
	       e->lower, e->upper, e->dist, e->exp, FALSE, e->data));
}

/*****************************************************************************/
/* Rename a set of events E1 so as not to clash with a set E0                */
/*****************************************************************************/

eventsetADT rename_event_set(eventsetADT E0,eventsetADT E1)
{
  eventsetADT cur_event;

  if(!E0) 
    return E1;

  for(cur_event = E1; cur_event; cur_event = cur_event->link)
    cur_event->event = rename_event(E0,cur_event->event);
  return (E1);
}
