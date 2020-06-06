/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1996 by, Chris J. Myers                                   */
/*   Electrical Engineering Department                                       */
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
/*   specific, prior written permission of Univeristy of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL U. OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE LIABLE  */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* heursyn.c                                                                 */
/*****************************************************************************/

#include "findrsg.h"
#include "heursyn.h"
#include "def.h"
#include "connect.h"
#include "findwctd.h"
#include "findred.h"
#include "verify.h"
#include "storeprs.h"
#include "bcp.h"
#include "dotgrf.h"
#include "printgrf.h"
#include <sys/time.h>

#define ALLOC(type, num)	(((type *) 0) + (num))
#define REALLOC(type, obj, num)	((obj) + (num))

/*****************************************************************************/
/* Heuristic timed circuit synthesis based on Sung Tae Jung's Direct         */
/*   synthesis method                                                        */
/*****************************************************************************/

static int print_graph=0;
static int heuristic1 = FALSE;
static int heuristic2 = FALSE;
static bool ddebug=TRUE;
static bool verbose=FALSE;
static designADT Design;
static ruleADT **rules;
static signalADT *signals;
static eventADT *events;
static cycleADT ****cycles;
static int ncycles;
static hevent **h_events;
static hevent **unfolded_events;
static stjung_signal **h_signals;
static int nsignals;
static int nevents;
static int ndummy;
static int marked_graph = TRUE;
static int si_mode=FALSE;
static bool gatelevel;
static int allocated_mem[10];
static int freed_mem[10];
static FILE *dbg=NULL;
static char *alloc_type[10] = {"literal", "term", "logic", "logic_list", 
                               "queue", "xnode", "comp"};
static int pgraphs = 0;
static int pgraph_nodes = 0;
static int pgraph_edges = 0;
static char **EC_approximated;
static cube_list **EC_exact;
static int array_i;
static array_t *mg_components=NULL;
static int *selected_comp=NULL;
static int *initial_cube=NULL;
static int *org_dropped;
static int **org_ruletype;
static int **conflict;
static int **ruletype;
static int **concurrency_table;
static int **ordered_table;
static int *dropped;
static int **cause_table;
static int **org_cause_table;
static int **comp_nr;
static int **comp_pr;
static mg_comp_struct **mg_comps;
static bool noparg;

#define LITERAL		0
#define TERM		1
#define LOGIC		2
#define LOGIC_LIST	3
#define QUEUE		4
#define XNODE		5
#define COMP            6

#define SUBSET_INCLUDED		1
#define OTHER_EVENTS_INCLUDED   2
#define NO_EVENTS_INCLUDED      3


void print_memory()
{
  int i;
  int total_allocated_mem=0, total_freed_mem=0;

  if (ddebug) {
    for (i=0; i< 4; i++) {
      fprintf(dbg, "%s : allocated = %d freed = %d lost = %d\n", alloc_type[i], 
              allocated_mem[i], freed_mem[i], allocated_mem[i] - freed_mem[i]);
      total_allocated_mem += allocated_mem[i];
      total_freed_mem += freed_mem[i];
    }
    fprintf(dbg, "Allocated Memory = %d Freed Memory = %d Lost Memory = %d\n", 
            total_allocated_mem, total_freed_mem, 
            total_allocated_mem - total_freed_mem);
  }
}


array_t *
array_dup(array_t *old)
{
    array_t *new_array;

    new_array = ALLOC(array_t, 1);
    new_array->num = old->num;
    new_array->n_size = old->num;
    new_array->obj_size = old->obj_size;
    new_array->space = ALLOC(char, new_array->n_size * new_array->obj_size);
    (void) memcpy(new_array->space, old->space, old->num * old->obj_size);
    return new_array;
}

int                     /* would like to be void, except for macro's */
array_resize(array_t *array, int new_size)
{
    int old_size;
    char *pos;

    old_size = array->n_size;
    array->n_size = MAX(array->n_size * 2, new_size);
    array->space = REALLOC(char, array->space, array->n_size * array->obj_size); 
    pos = array->space + old_size * array->obj_size;
    (void) memset(pos, 0, (array->n_size - old_size)*array->obj_size);
    return TRUE;
}

void
array_sort(array_t *array, int (*compare)(const void *, const void *))
{
    qsort(array->space, array->num, array->obj_size, compare);
}

void
array_free(array_t *array)
{
    FREE(array->space);
    FREE(array);
}

array_t *
array_do_alloc(int size, int number)
{
    array_t *array;

    array = ALLOC(array_t, 1);
    array->num = 0;
    array->n_size = MAX(number, 3);
    array->obj_size = size;
    array->space = ALLOC(char, array->n_size * array->obj_size);
    (void) memset(array->space, 0, array->n_size * array->obj_size);
    return array;
}

int                     /* would like to be void, except for macro's */
array_abort(int i)
{
    switch (i) {
        case 0:
          //            fail("array: array access out of bounds\n");
            /* NOTREACHED */
        case 1:
          //fail("array: object size mismatch\n");
            /* NOTREACHED */
        case 2:
          //fail("array: append not defined for arrays of different sizes\n");
            /* NOTREACHED */
        case 3:
          //fail("array: join not defined for arrays of different sizes\n");
            /* NOTREACHED */
        default:
          ;
          //fail("array: unknown error\n");
            /* NOTREACHED */
    }
}

void print80(FILE *fp, char *str, bool reset)
{
    static int count=0;

    if (reset) {
        fprintf(fp,"%s", str);
        count = strlen(str);
    }
    else {
        if (count + strlen(str) > 78) {
            count = 0;
            fprintf(fp,"\n");
        }
        fprintf(fp,"%s", str);
        count = count + strlen(str);
    }
}

void print_concurrency_table(FILE *fp)
{
  char buf[100];
  int i, j, count;

  for (i = 1; i < (nevents - ndummy); i++) 
  {
    if (events[i]->dropped) continue;
    count = 0;
    for (j = 1; j < (nevents - ndummy); j++) 
      if (concurrency_table[i][j]) count++;
    if (count == 0) continue;
    sprintf(buf, "%s is con with ", h_events[i]->name);
    print80(fp, buf, TRUE);
    for (j = 1; j < (nevents - ndummy); j++) 
      if (concurrency_table[i][j]) {
        sprintf(buf, " %s", h_events[j]->name);
        print80(fp, buf, FALSE);
      }
    fprintf(fp,"\n");
  }
}

int events_num(hevent *e)
{
  int i, n = 0;

  for (i = e->signal->first; i <= e->signal->last; i++) 
    if (!events[i]->dropped) n++;

  return n / 2;
}

int events_num(stjung_signal *s)
{
  int i, n = 0;

  for (i = s->first; i <= s->last; i++) 
    if (!events[i]->dropped) n++;

  return n / 2;
}

int get_max_nevents()
{ 
  int i, max_events_occur=-1;

  for (i = 0; i < nsignals; i++) {
    if (events_num(h_signals[i]) > max_events_occur) 
       max_events_occur = events_num(h_signals[i]);
  }
  return max_events_occur;
}

void new_comp(comp_set *set, int comp)
{
  hcomp *l, *newcomp;

  for (l = set->comps; l; l = l->next)
    if (l->comp == comp) return;
  newcomp = ALLOC(hcomp, 1);
  allocated_mem[COMP] += sizeof(hcomp);
  newcomp->comp = comp;
  newcomp->next = set->comps;
  set->comps = newcomp;
}

void new_literal_unique(term *term, hevent *events)
{
    literal *p;
    literal *newliteral;

    for (p = term->llist; p; p = p->next)
        if (IS_SAME_SIGNAL(p->event, events)) return;
    newliteral = ALLOC(literal, 1);
    allocated_mem[LITERAL] += sizeof(literal);
    newliteral->next = term->llist;
    newliteral->event = events;
    term->llist = newliteral;
}

comp_set * new_comp_set(set_of_comp_set *setofset)
{
    comp_set *set;

    set = ALLOC(comp_set, 1);
    allocated_mem[COMP] += sizeof(comp_set);
    set->comps = NULL;
    if (setofset) {
      set->next = setofset->sets;
      setofset->sets = set;
    }
    else set->next = NULL;
    return set;
}

term *alloc_term(hlogic *net)
{
    term *newterm;

    newterm = ALLOC(term, 1);
    allocated_mem[TERM] += sizeof(term);
    newterm->llist = NULL;
    newterm->src_list = NULL;
    newterm->comp = NULL;
    newterm->nsignals = -1;

    if (net) {
        newterm->next = net->terms;
        net->terms = newterm;
    }
    else newterm->next = NULL;
    return newterm;
}

void free_cube_list(struct cube_list *list)
{
  struct cube_list *cl, *fp;

  if (list == NULL) return;

  for (cl = list; cl; cl = cl->next) free(cl->cube);
  cl = list;
  fp = cl;
  while (cl->next) {
    cl = cl->next;
    free(fp);
    fp = cl;
  }
  free(fp);
}

void free_list(literal *list)
{
    literal *fp;
    literal *l;

    if (!list) return;

    l = list;
    fp = l;
     while (l->next) {
        l = l->next;
        free(fp);
        freed_mem[LITERAL] += sizeof(literal);
        fp = l;
    }
    free(fp);
    freed_mem[LITERAL] += sizeof(literal);
}

void delete_term(struct term *t)
{
  if (t->llist) free_list(t->llist);
  if (t->src_list) free_list(t->src_list);
  if (t->comp) free(t->comp);
  free(t);
  freed_mem[TERM] += sizeof(struct term);
}

void delete_term(hlogic *net, struct term *term)
{
    struct term *t, *pre_t=NULL;

    for (t = net->terms; t ; t = t->next) {
        if (t == term) {
            if (pre_t == NULL) {
                net->terms = t->next;
                delete_term(t);
            }
            else {
                pre_t->next = t->next;
                delete_term(t);
            }
            return;
        }
        pre_t = t;
    }
}

void free_comp(hcomp *c)
{
  hcomp *l, *free_c;

  if (!c) return;
  l = c;
  free_c = l;
  while (l->next) {
    l = l->next;
    free(free_c);
    freed_mem[COMP] += sizeof(hcomp);
    free_c = l;
  }
  free(free_c);
  freed_mem[COMP] += sizeof(hcomp);
}

void free_comp_set(comp_set *s)
{
  if (s->comps) free_comp(s->comps);
  free(s);
  freed_mem[COMP] += sizeof(comp_set);
}

void free_set_of_comp_set(set_of_comp_set *sos)
{
  comp_set *s, *free_s;

  for (s = sos->sets; s; ) {
    free_s = s;
    s = s->next;
    free_comp_set(free_s);
  }
  free(sos);
  freed_mem[COMP] += sizeof(set_of_comp_set);
}

void delete_net(hlogic *net)
{
    term *term, *free_term;

    for (term=net->terms; term; ) {
        free_term = term;
        term = term->next; 
        delete_term(free_term);
    }
    free(net);
    freed_mem[LOGIC] += sizeof(hlogic);
}

set_of_comp_set *new_set_of_comp_set()
{
    set_of_comp_set *new_set;

    new_set = ALLOC(set_of_comp_set, 1);
    allocated_mem[COMP] += sizeof(set_of_comp_set);
    new_set->sets = NULL;
    return new_set;
}

hlogic *alloc_net(hlogic *net, hevent *events)
{
    hlogic *new_net;

    if (net) delete_net(net);
    new_net = ALLOC(hlogic, 1);
    allocated_mem[LOGIC] += sizeof(hlogic);
    new_net->terms = NULL;
    new_net->event = events;
    return new_net;
}

void decomp_get_initial_logic(ruleADT **rules, hevent **h_events, 
                            int nevents,  hlogic *net, 
                            event_list *events_list)
{   
  int i;
  term *curr_term;
  event_list *el;
  
  if (net->terms == NULL) curr_term = alloc_term(net);
  else curr_term = net->terms;

  for (i = 1; i < (nevents - ndummy); i++) 
  {
    if (events[i]->dropped) continue;
    for (el = events_list; el; el = el->next) 
      if (ruletype[i][el->event->index] == (TRIGGER | SPECIFIED) ||
          si_mode && ruletype[i][el->event->index] == 
             (TRIGGER | SPECIFIED | REDUNDANT))
        new_literal_unique(curr_term, h_events[i]);
  }
}   

void get_initial_logic(ruleADT **rules, hevent **h_events, 
                            int nevents,  hlogic *net, 
                            event_list *events_list)
{   
  int i;
  term *curr_term;
  event_list *el;
  
  curr_term = alloc_term(net);
  for (i = 1; i < (nevents - ndummy); i++) 
  {
    if (events[i]->dropped) continue;
    for (el = events_list; el; el = el->next) 
      if (ruletype[i][el->event->index] == (TRIGGER | SPECIFIED) ||
          si_mode && ruletype[i][el->event->index] == 
             (TRIGGER | SPECIFIED | REDUNDANT))
        new_literal_unique(curr_term, h_events[i]);
  }
}   

void add_list(literal **l, hevent *t)
{
    literal *newliteral;

    newliteral = ALLOC(literal, 1);
    allocated_mem[LITERAL] += sizeof(literal);
    newliteral->next = *l;
    newliteral->event = t;
    *l = newliteral;
}

int add_list_unique(literal **l, hevent *t)
{
    literal *newliteral, *p;

    for (p = *l; p && p->next; p = p->next) if (p->event == t) return FALSE;
    if (p && p->event == t) return FALSE;

    newliteral = ALLOC(literal, 1);
    allocated_mem[LITERAL] += sizeof(literal);

    if (p) {
        newliteral->next = NULL;
        newliteral->event = t;
        p->next = newliteral;
    }
    else {
        newliteral->next = *l;
        newliteral->event = t;
        *l = newliteral;
    }
    return TRUE;
}

event_list *
get_trig_events(hevent *v)
{
  event_list *tl=NULL;

  int i;
  
  for (i = 1; i < (nevents - ndummy); i++) 
  {
    if (events[i]->dropped) continue;
    if (ruletype[i][v->index] == (TRIGGER | SPECIFIED) ||
        si_mode && ruletype[i][v->index] == 
           (TRIGGER | SPECIFIED | REDUNDANT)) 
            add_list(&tl, h_events[i]);
  }
  return tl;
}

int trig_events_num(hevent *e)
{   
  int i, trig_events=0;
  
  for (i = 1; i < (nevents - ndummy); i++) 
  {
    if (events[i]->dropped) continue;
    if (ruletype[i][e->index] == (TRIGGER | SPECIFIED) ||
        si_mode && ruletype[i][e->index] == (TRIGGER | SPECIFIED | REDUNDANT)) 
       trig_events++;
  }
  return trig_events;
}   

int is_trig_event(hevent *e1, hevent *e2)
{   
  if (ruletype[e1->index][e2->index] == (TRIGGER | SPECIFIED) ||
         si_mode && ruletype[e1->index][e2->index] == (TRIGGER | SPECIFIED | REDUNDANT)) 
    return TRUE;
  else
    return FALSE;
}   

void add_to_logic_list(stjung_signal *s, hlogic *set, hlogic *reset)
{
    logic_list *new_logic;

    if (set) {
        new_logic = ALLOC(logic_list, 1);
        allocated_mem[LOGIC_LIST] += sizeof(logic_list);
        new_logic->next = s->set;
        new_logic->net = set;
        s->set = new_logic;
    }
    if (reset) {
        new_logic = ALLOC(logic_list, 1);
        allocated_mem[LOGIC_LIST] += sizeof(logic_list);
        new_logic->next = s->reset;
        new_logic->net = reset;
        s->reset = new_logic;
    }
}

/*
 *---------------------------------------------------------------------
 * lname:
 *        remove '+', '-' and occurrence number from a event name
 *---------------------------------------------------------------------
 */

char *lname(char *tname)
{
    char *c;
    static char buf[256];
    int i=0;
    

    for (c=tname; c && *c != '+' && *c != '-'; c++)
        buf[i++] = *c;
    if (*c == '-') buf[i++] = '\'';
    buf[i] = '\0';
    return buf;
}   
    
char *prs_lname(hevent *e)
{
    static char buf[256];
   
    if ((e->index % 2) == 1) return e->signal->name;
    else {
      sprintf(buf, "~%s", e->signal->name);
      return buf;
    }
}   
    
/*
 *---------------------------------------------------------------------
 * rev_lname:
 *        remove '+', '-' and occurrence number from a event name
 *        and return reverse type name
 *---------------------------------------------------------------------
 */

char *rev_lname(char *tname)    
{
    char *c;
    static char buf[256];
    int i=0;


    for (c=tname; c && *c != '+' && *c != '-'; c++) 
        buf[i++] = *c;
    if (*c == '+') buf[i++] = '\''; 
    buf[i] = '\0';
    return buf;
}


void print_logic(FILE *fp, char *str, hlogic *net, int andor, net_type reset) 
{   
  term *t;
  literal *l;
  char buf[256];
  int ncubes=0; 

  print80(fp, str, TRUE);
  if (!net) {
    fprintf(fp," logic net is null \n");
    return;
  }
  if (andor) {
    if (net->terms == NULL) fprintf(fp, " term is NULL\n");
    for (t=net->terms; t; t = t->next) {
      ncubes++;
      if (!t->llist) fprintf(fp," literal is NULL");
      for(l=t->llist; l; l = l->next) {
        sprintf(buf, "%s ", reset == SET_NET ? lname(l->event->name) : 
                                rev_lname(l->event->name));
                print80(fp, buf, FALSE);
      }
      if (t->next) print80(fp, "+ ", FALSE);
    }
  }
  else
    for (t=net->terms; t; t = t->next) {
      ncubes++;
      if (!t->llist) print80(fp, "NULL", FALSE);
      if (t->llist->next) print80(fp, " ( ", FALSE);
      for(l=t->llist; l; l = l->next) {
        sprintf(buf, "%s ", reset == SET_NET ? lname(l->event->name) :
                                rev_lname(l->event->name));
        print80(fp, buf, FALSE);
        if (l->next) print80(fp, "+ ", FALSE);
      }
      if (t->llist->next) print80(fp, ")", FALSE);
    }
  fprintf(fp,"\n");

  fprintf(fp, "Number of cubes in the logic = %d\n", ncubes);
}

void print_logic_and_src(FILE *fp, char *str, hlogic *net, int andor, net_type reset) 
{   
  term *t;
  literal *l;
  char buf[256];
  int ncubes=0; 

  print80(fp, str, TRUE);
  if (!net) {
    fprintf(fp," logic net is null \n");
    return;
  }
  if (andor) {
    if (net->terms == NULL) fprintf(fp, " term is NULL\n");
    for (t=net->terms; t; t = t->next) {
      ncubes++;
      if (!t->llist) fprintf(fp," literal is NULL");
      for(l=t->llist; l; l = l->next) {
        sprintf(buf, "%s ", reset == SET_NET ? lname(l->event->name) : 
                                rev_lname(l->event->name));
                print80(fp, buf, FALSE);
      }
      if (t->src_list) {
        fprintf(fp, "(");
        for(l=t->src_list; l; l = l->next) {
          sprintf(buf, "%s ", l->event->name);
          print80(fp, buf, FALSE);
        }
        fprintf(fp, ")");
      }
      if (t->next) print80(fp, "+ ", FALSE);
    }
  }
  else
    for (t=net->terms; t; t = t->next) {
      ncubes++;
      if (!t->llist) print80(fp, "NULL", FALSE);
      if (t->llist->next) print80(fp, " ( ", FALSE);
      for(l=t->llist; l; l = l->next) {
        sprintf(buf, "%s ", reset == SET_NET ? lname(l->event->name) :
                                rev_lname(l->event->name));
        print80(fp, buf, FALSE);
        if (l->next) print80(fp, "+ ", FALSE);
      }
      if (t->llist->next) print80(fp, ")", FALSE);
    }
  fprintf(fp,"\n");

  fprintf(fp, "Number of cubes in the logic = %d\n", ncubes);
}

void print_logic_to_prs(FILE *fp, hlogic *net, net_type nettype, int logic_type) 
{   
  term *t;
  literal *l;

    for (t=net->terms; t; t = t->next) {
      fprintf(fp, "[%s%s: (", logic_type == COMBI ? 
                               (nettype == SET_NET ? " " : "~") : 
                               (nettype == SET_NET ? "+" : "-"), 
                           signals[net->event->signal->index]->name);
      for(l=t->llist; l; l = l->next) {
        fprintf(fp, "%s", 
                   nettype == SET_NET ? prs_lname(l->event) : 
                   prs_lname(l->event));
        if (l->next) fprintf(fp, " & ");
      }
      fprintf(fp, ")]");
      if (logic_type == COMBI) fprintf(fp, " Combinational"); 
      fprintf(fp, "\n");
    }
}

void add_queue(hevent *s, hqueue **front, hqueue **rear)
{
    hqueue *new_node;

    new_node = ALLOC(hqueue, 1);
    allocated_mem[QUEUE] += sizeof(hqueue);
    new_node->next = NULL;
    new_node->event = s;
    if (*rear)  (*rear)->next = new_node;
    if (*front == NULL) *front = new_node;
    *rear = new_node;
}

hevent *delete_queue(hqueue **front, hqueue **rear)
{
    hevent *t;
    hqueue *fq;

    if (*front) {
        t = (*front)->event;
        fq = *front;
        if (*front == *rear) *front = *rear = NULL;
        else (*front) = (*front)->next;
        free(fq);
        freed_mem[QUEUE] += sizeof(hqueue);
        return t;
    }
    else
        return NULL;
}

void add_to_xlist(hevent *t, hevent *u)
{
  xnode *l;

  for (l = t->xlist; l; l = l->next)
    if (l->event == u) return;

  l = ALLOC(xnode, 1);
  allocated_mem[XNODE] += sizeof(xnode);
  l->next = t->xlist;
  l->event = u;
  l->extra = NULL;
  t->xlist = l;
}

int some_event_cause(hevent *s, hevent *zi)
{
    int i;

    for (i = h_signals[s->signal->index]->first; 
         i <= h_signals[s->signal->index]->last; i++) 
      if (IS_SAME_TYPE(s, h_events[i]) && cause(h_events[i], zi)) return TRUE;
    return FALSE;
}

int some_rev_event_cause(hevent *s, hevent *zi)
{
    int i;

    for (i = h_signals[s->signal->index]->first; 
         i <= h_signals[s->signal->index]->last; i++) 
      if (IS_REV_TYPE(s, h_events[i]) && cause(h_events[i], zi)) return TRUE;
    return FALSE;
}

int get_cause_event_type(hevent *s, hevent *zi)
{
    int i;

    for (i = h_signals[s->signal->index]->first; 
         i <= h_signals[s->signal->index]->last; i++) 
      if (cause(h_events[i], zi)) return h_events[i]->event_type;
    return -1;
}

int some_event_concur(hevent *s, hevent *zi)
{
    int j;

    for (j = 1; j < nevents - ndummy; j++)
      if (concurrency_table[j][zi->index] && IS_SAME_SIGNAL(h_events[j], s)) 
        return TRUE;
    return FALSE;
}

void unfolded_print_precedence_graph(event_list *Sn, event_list *Dn, FILE *fp)
{
    xnode *xl; 
    literal *l;    hevent *t;
    char buf[100];
    int i;

    if (Sn != NULL) pgraphs++;

    for (i = 1; i < nevents * 2; i++) unfolded_events[i]->selected = FALSE;

    print80(fp, "Source nodes: ", TRUE);
    for (l = Sn; l; l = l->next) {
        sprintf(buf, " %s ", l->event->name);
        print80(fp, buf, FALSE);
        l->event->selected = TRUE;
    }
    fprintf(fp,"\n");
    print80(fp, "Destination nodes: ", TRUE);
    for (l = Dn; l; l = l->next) {
        sprintf(buf, " %s ", l->event->name);
        print80(fp, buf, FALSE);
        l->event->selected = TRUE;
    }
    fprintf(fp,"\n");
    for (i = 1; i < nevents * 2; i++) 
    {
        t = unfolded_events[i];
        if (!t->xlist) continue;
        sprintf(buf, "%s : ", t->name); 
        print80(fp, buf, TRUE);
        for (xl = t->xlist; xl ; xl = xl->next) {
            sprintf(buf, " %s ", xl->event->name);
            print80(fp, buf, FALSE);
            xl->event->selected = TRUE;
            if (Sn) pgraph_edges++;
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"\n");
    for (i = 1; i < nevents * 2; i++) 
      if (unfolded_events[i]->selected == TRUE && Sn) pgraph_nodes++; 
}

void print_precedence_graph(event_list *Sn, event_list *Dn, FILE *fp)
{
    xnode *xl; 
    literal *l;    hevent *t;
    char buf[100];
    int i;

    if (Sn != NULL) pgraphs++;

    for (i = 1; i < nevents; i++) h_events[i]->selected = FALSE;

    print80(fp, "Source nodes: ", TRUE);
    for (l = Sn; l; l = l->next) {
        sprintf(buf, " %s ", l->event->name);
        print80(fp, buf, FALSE);
        l->event->selected = TRUE;
    }
    fprintf(fp,"\n");
    print80(fp, "Destination nodes: ", TRUE);
    for (l = Dn; l; l = l->next) {
        sprintf(buf, " %s ", l->event->name);
        print80(fp, buf, FALSE);
        l->event->selected = TRUE;
    }
    fprintf(fp,"\n");
    for (i = 1; i < nevents; i++) 
    {
        t = h_events[i];
        if (!t->xlist) continue;
        sprintf(buf, "%s : ", t->name); 
        print80(fp, buf, TRUE);
        for (xl = t->xlist; xl ; xl = xl->next) {
            sprintf(buf, " %s ", xl->event->name);
            print80(fp, buf, FALSE);
            xl->event->selected = TRUE;
            if (Sn) pgraph_edges++;
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"\n");
    for (i = 1; i < nevents; i++) 
      if (h_events[i]->selected == TRUE && Sn) pgraph_nodes++; 
}

void literal_number(event_list *ll, int *lcount, int *bcount)
{
    *bcount = 0; 
    *lcount = 0;
    for (; ll; ll = ll->next) {
        if (event_type(ll->event) == FALLING) (*bcount)++;
        (*lcount)++; 
    }
}

void new_literal(term *term, hevent *events)
{
    literal *newliteral;
    
    newliteral = ALLOC(literal, 1);
    allocated_mem[LITERAL] += sizeof(literal);
    newliteral->next = term->llist;
    newliteral->event = events;
    term->llist = newliteral;
}


int num_of_context_signals(hlogic *net, term *term)
{
    int trigger;
    int count = 0;
    event_list *tl, *ul;
    
    for (tl = term->llist; tl; tl = tl->next) {
      trigger = FALSE;
      for (ul = net->terms->llist; !trigger && ul; ul = ul->next)
        if (IS_SAME_SIGNAL(tl->event, ul->event)) trigger = TRUE;
      if (!trigger) count++;
    }

    return count;
}

int count_num_of_signals(term *term)
{
    int count = 0;
    event_list *tl;
    
    for (tl = term->llist; tl; tl = tl->next) {
      count++;
    }
    term->nsignals = count;

    return count;
}

void add_to_net(hlogic *net, struct term *term, int unique)
{
    struct term *free_t, *t, *new_term;
    int sig_num;
    bool covered, found;
    event_list *tl, *sl, *literal;

    if (unique) {
        sig_num = num_of_signals(term);

        /* if it is covered by an element of the set then don't add */
        for (t = net->terms; t; t = t->next) {
            if (sig_num >= num_of_signals(t)) {
                covered = TRUE;
                for (tl = t->llist; tl && covered ; tl = tl->next) {
                    found = FALSE;
                    for (sl = term->llist; sl && !found; sl = sl->next)
                        if (IS_SAME_SIGNAL(tl->event, sl->event))
                            found = TRUE;
                    if (!found) covered = FALSE;
                }
                if (covered) return;
            }
        }
    
        /* if it covers some elements of the set then remove them
            and add the new term */
        for (t = net->terms; t; ) {
            if (sig_num < num_of_signals(t)) {
                covered = TRUE;
                for (tl = term->llist; tl && covered ; tl = tl->next) {
                    found = FALSE;
                    for (sl = t->llist; sl && !found; sl = sl->next)
                        if (IS_SAME_SIGNAL(tl->event, sl->event))
                            found = TRUE;
                    if (!found) covered = FALSE;
                }
                if (covered) {
                  free_t = t;
                  t = t->next;
                  delete_term(net, free_t);
                }
                else t = t->next;
            }
            else t = t->next;
        }
    }
    
    /* add the new term */
    new_term = alloc_term(net);
    for (literal = term->llist; literal; literal = literal->next) 
        new_literal(new_term, literal->event);
}

void add_to_net_and_src(hlogic *net, struct term *term, int unique)
{
    struct term *free_t, *t, *new_term;
    int sig_num;
    bool covered, found;
    event_list *tl, *sl, *literal;

    if (unique) {
        sig_num = num_of_signals(term);

        /* if it is covered by an element of the set then don't add */
        for (t = net->terms; t; t = t->next) {
            if (sig_num >= num_of_signals(t)) {
                covered = TRUE;
                for (tl = t->llist; tl && covered ; tl = tl->next) {
                    found = FALSE;
                    for (sl = term->llist; sl && !found; sl = sl->next)
                        if (IS_SAME_SIGNAL(tl->event, sl->event))
                            found = TRUE;
                    if (!found) covered = FALSE;
                }
                if (covered) return;
            }
        }
    
        /* if it covers some elements of the set then remove them
            and add the new term */
        for (t = net->terms; t; ) {
            if (sig_num < num_of_signals(t)) {
                covered = TRUE;
                for (tl = term->llist; tl && covered ; tl = tl->next) {
                    found = FALSE;
                    for (sl = t->llist; sl && !found; sl = sl->next)
                        if (IS_SAME_SIGNAL(tl->event, sl->event))
                            found = TRUE;
                    if (!found) covered = FALSE;
                }
                if (covered) {
                  free_t = t;
                  t = t->next;
                  delete_term(net, free_t);
                }
                else t = t->next;
            }
            else t = t->next;
        }
    }
    
    /* add the new term */
    new_term = alloc_term(net);
    for (literal = term->llist; literal; literal = literal->next) 
        new_literal(new_term, literal->event);
    new_term->src_list = term->src_list;
    term->src_list = NULL;
}

hlogic *copy_net(hlogic *net)
{
    hlogic *new_net;
    term *term, *new_term;
    event_list *tl;

    if (!net) return NULL;
    new_net = alloc_net(NULL, net->event);
    for (term = net->terms; term; term = term->next) {
        new_term = alloc_term(new_net);
        for (tl = term->llist; tl; tl = tl->next)
            new_literal(new_term, tl->event);
        for (tl = term->src_list; tl; tl = tl->next)
            add_list(&new_term->src_list, tl->event);
    }
    return new_net;
}

void free_xlist(xnode *l)
{
    xnode *fp;

    if (!l) return;

    fp = l;
    while (l->next) {
        l = l->next;
        if (fp->extra) delete_net(fp->extra);
        free(fp);
        freed_mem[XNODE] += sizeof(xnode);
        fp = l;
    }
    free(l);
    freed_mem[XNODE] += sizeof(xnode);
}

void free_xgraph()
{
    int i;

    for (i = 0; i < nevents; i++)
        if (h_events[i]->xlist)  free_xlist(h_events[i]->xlist); 
}

int is_in_list(hevent *t, struct event_list *l)
{
    literal *p;

    for (p = l; p ; p = p->next)
        if (p->event == t) return TRUE;
    return FALSE;
}

void print_list(FILE *fp, char *str, literal *l)
{
    char buf[256];

    if (!l) {
        fprintf(fp,"%s : NULL\n", str);
        return;
    }

    print80(fp, str, TRUE);
    for (; l ; l = l->next) {
        sprintf(buf, " %s ", l->event->name);
        print80(fp, buf, FALSE);
    }
    fprintf(fp,"\n");
}

int con_with_some_event(hevent *s, hevent *t)
{
  int i;

  for (i = s->signal->first; i <= s->signal->last; i++) 
    if (is_concur(h_events[i], t)) return TRUE;
  return FALSE;
}

int con_with_some_rev_event(hevent *s, hevent *t)
{
  int i;

  for (i = s->signal->first; i <= s->signal->last; i++) 
    if (IS_REV_TYPE(s, h_events[i]) && is_concur(h_events[i], t)) return TRUE;
  return FALSE;
}

void find_destination_nodes(hevent *zi, hevent *zj, 
                                 hevent *zk, event_list **E, 
                                 event_list **Dn)
{
    event_list *enabling_events, *tl;

    (*E) = (*Dn) = NULL;
    
    enabling_events = get_trig_events(zk);
    for (tl = enabling_events; tl ; tl = tl->next) add_list(E, tl->event);

    for (tl = enabling_events; tl ; tl = tl->next) add_list(Dn, tl->event->pr);
    free_list(enabling_events);
}

int is_stable_events(hevent *t, hevent *zi)
{
    int i,j;

    i = t->signal->index;
    for (j = h_signals[i]->first; j <= h_signals[i]->last; j++) {
      if (is_concur(h_events[j], zi)) return FALSE;
    }
    return TRUE;
}

event_list * copy_list(event_list *L)
{
    event_list *newliteral=NULL, *tl;

    for (tl = L; tl; tl = tl->next)
        add_list(&newliteral, tl->event);
    return newliteral;
}

set_of_comp_set *
multiply_two_set(set_of_comp_set *setofset1, set_of_comp_set *setofset2)
{
    set_of_comp_set *new_set_of_set=NULL;
    comp_set *s, *new_set, *set1, *set2;
    hcomp *l, *c1, *c2;
    int m, n, all_comp_are_same, same_comp_set_exist, same_comp_exist;

    for (m = 0, set1 = setofset1->sets; set1; set1 = set1->next, m++) ;
    for (m = 0, set1 = setofset2->sets; set1; set1 = set1->next, m++) ;
    new_set_of_set = new_set_of_comp_set();

    for (set1 = setofset1->sets; set1; set1 = set1->next) 
        for (set2 = setofset2->sets; set2; set2 = set2->next) {
            new_set = new_comp_set(NULL);
            for (l = set1->comps; l; l = l->next)
                new_comp(new_set, l->comp);
            for (l = set2->comps; l; l = l->next)
                new_comp(new_set, l->comp);
            same_comp_set_exist = FALSE;
            n = 0;
            for (c1 = new_set->comps; c1; c1 = c1->next) n++; 
            for (s = new_set_of_set->sets; s; s = s->next) {
              m = 0;
              for (c2 = s->comps; c2; c2 = c2->next) m++;
              if (m != n) continue;
              all_comp_are_same = TRUE;
              for (c1 = new_set->comps; c1; c1 = c1->next) {
                same_comp_exist = FALSE;
                for (c2 = s->comps; c2; c2 = c2->next) 
                  if (c1->comp == c2->comp) same_comp_exist = TRUE;
                if (same_comp_exist == FALSE) all_comp_are_same = FALSE;
              }
              if (all_comp_are_same) same_comp_set_exist = TRUE;
            }
            if (new_set_of_set->sets == NULL || same_comp_set_exist == FALSE) {
              new_set->next = new_set_of_set->sets;
              new_set_of_set->sets = new_set;
            }
            else {
               free_comp_set(new_set);
            }
        }
    return new_set_of_set;
}

void copy_drop_and_ruletype()
{
  int i, j;

  org_dropped = (int *) malloc(nevents * sizeof(int));
  org_ruletype = (int **) malloc(nevents * sizeof(int *));
  for (i = 1; i < nevents; i++) 
    org_ruletype[i] = (int *) malloc(nevents * sizeof(int));
  for (i = 1; i < nevents; i++) org_dropped[i] = events[i]->dropped;
  
  for (i = 1; i < nevents; i++) 
    for (j = 1; j < nevents; j++)
      org_ruletype[i][j] = rules[i][j]->ruletype; 

  ruletype = org_ruletype;
  dropped = org_dropped;
}

void find_MG_comp_data(char menu,char command,designADT design)
{
  int i, j, k;
  array_t *component;

  mg_comps = (mg_comp_struct **) malloc(sizeof(mg_comp_struct *) * 
                                         array_n(mg_components));
  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    mg_comps[i] = (mg_comp_struct *) malloc(sizeof(mg_comp_struct)); 
    mg_comps[i]->dropped = (int *) malloc(sizeof(int) * nevents);
    mg_comps[i]->ruletype = (int **) malloc(sizeof(int *) * nevents);
    mg_comps[i]->cause_table = NULL;
    for (j = 1; j < nevents; j++) 
      mg_comps[i]->ruletype[j] = (int *) malloc(sizeof(int) * nevents); 

    for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
    for (j = 0; j < array_n(component); j++) {
      k = array_fetch(int, component, j);
      h_events[k]->selected = TRUE;
    }
    for (j = 1; j < nevents; j++) 
      if (h_events[j]->selected == FALSE) mg_comps[i]->dropped[j] = TRUE;
 
    design->status  = design->status & ~FOUNDRED;
    design->verbose = FALSE;
    
    if (!find_redundant_rules(menu,command,design)) {
      printf("ERROR: during find redundant rules\n");
      fprintf(lg, "ERROR: during find redundant rules\n");
    }

    for (j = 1; j < nevents; j++) 
      for (k = 1; k < nevents; k++) {
        mg_comps[i]->ruletype[j][k] = rules[j][k]->ruletype;
        if (rules[j][k]->ruletype != 0 && (h_events[j]->selected == FALSE || 
               h_events[k]->selected == FALSE))
          mg_comps[i]->ruletype[j][k] = rules[j][k]->ruletype | REDUNDANT;
       }
  }
}


void set_to_a_component(array_t *component, int i)
{
  int j, k;

  for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
  for (j = 0; j < array_n(component); j++) {
    k = array_fetch(int, component, j);
    h_events[k]->selected = TRUE;
  }
  for (j = 1; j < nevents; j++) 
    if (h_events[j]->selected == FALSE) events[j]->dropped = TRUE;

  ruletype = mg_comps[i]->ruletype;
  if (mg_comps[i]->cause_table) cause_table = mg_comps[i]->cause_table;
}

void set_nr_pr(int i)
{
  int j;

  for (j = 1; j < (nevents - ndummy); j++) {
    if (events[j]->dropped) continue;
    h_events[j]->nr = h_events[comp_nr[i][j]];
    h_events[j]->pr = h_events[comp_pr[i][j]];
  }
}

int 
check_events_included(event_list *events_set, array_t *component, 
                          event_list **subset)
{
  int i, include;
  stjung_signal *s;
  event_list *el;
 
  s = events_set->event->signal;


  if (ddebug) fprintf(dbg, "s = %s\n", s->name);


  *subset = NULL;

  include = FALSE;
  for (i = s->first; i <= s->last; i++) 
    if (events[i]->dropped == FALSE) include = TRUE;
    
  if (include == FALSE) return NO_EVENTS_INCLUDED;

  for (el = events_set; el; el = el->next) {
    if (events[el->event->index]->dropped == FALSE) {
      add_list(subset, el->event);
    }
  }

  if (*subset == NULL) return OTHER_EVENTS_INCLUDED;
  else return SUBSET_INCLUDED;
}

void restore_to_original_spec()
{
  int j;

  for (j = 1; j < nevents; j++) events[j]->dropped = org_dropped[j];
  ruletype = org_ruletype;
  cause_table = org_cause_table;
}

hlogic *
multiply_two_net(hlogic *net1, hlogic *net2, bool unique)
{
  hlogic *new_net=NULL;
  term *new_term, *term1, *term2;
  event_list *tl;

  new_net = alloc_net(NULL, NULL);
  for (term1 = net1->terms; term1; term1 = term1->next) 
    for (term2 = net2->terms; term2; term2 = term2->next) {
      new_term = alloc_term(NULL);
      for (tl = term1->llist; tl; tl = tl->next)
        if (unique) new_literal_unique(new_term, tl->event);
        else new_literal(new_term, tl->event);
      for (tl = term2->llist; tl ; tl = tl->next)
        if (unique) new_literal_unique(new_term, tl->event);
        else new_literal(new_term, tl->event);
/*
      new_term->next = new_net->terms;
      new_net->terms = new_term;
*/
      add_to_net(new_net, new_term, TRUE);
      delete_term(new_term);
  }
  return new_net;
}

int check_hazard(term_struct *candidate, event_list *events_set)
{
  hevent *cause_event;
  event_list *tl, *zl, *ul, *el, *subset=NULL, *trig_events;
  int j, k, result,  covered;
  array_t *component;


  if (ddebug) {
    print_list(dbg, "\ncheck_hazard: events_set = ", events_set);
    print_list(dbg, "candidate = ", candidate->llist);
    print_list(dbg, "src_list = ", candidate->src_list);
  }


  for (j = 0; j < array_n(mg_components); j++) {
    restore_to_original_spec();
    component = array_fetch(array_t *, mg_components, j);
    set_to_a_component(component, j);
    set_nr_pr(j);

    result = check_events_included(events_set, component, &subset);

    if (result != SUBSET_INCLUDED) continue;

    for (zl = subset; zl; zl = zl->next) {
      for (tl = candidate->llist; tl; tl = tl->next) {
        cause_event = NULL;
        for (k=h_signals[tl->event->signal->index]->first; 
             !cause_event && k<=h_signals[tl->event->signal->index]->last;k++) {
          if (events[k]->dropped) continue;
          if (cause(h_events[k], zl->event)) cause_event = h_events[k];
        }
        if (!cause_event) continue;


        if (ddebug) fprintf(dbg, "cause_event = %s\n", cause_event->name);


        if (!(cause(zl->event, cause_event->nr) && 
              cause(zl->event, cause_event->nr->nr))) continue;

        covered = FALSE;
        for (ul = candidate->src_list; !covered && ul; ul = ul->next){
          if (events[ul->event->index]->dropped) continue;
          if ((cause(zl->event, ul->event) || zl->event == ul->event) &&
              (cause(ul->event, cause_event->nr) || 
               cause(ul->event, cause_event->nr->nr))) {
            covered = TRUE;

            if (ddebug && cause(ul->event, cause_event->nr)) 
              fprintf(dbg, "%s cause %s->nr\n", ul->event->name, 
                     cause_event->nr->name);
           if (ddebug && cause(ul->event, cause_event->nr->nr)) 
             fprintf(dbg,"%s cause %s->nr->nr\n",ul->event->name, 
                     cause_event->nr->nr->name);

          }
          else {

              if (ddebug) fprintf(dbg, "%s does not cause %s or %s\n",
                   ul->event->name, cause_event->nr->name,  
                   cause_event->nr->nr->name);

          }
        }

        if (covered == FALSE) {
          trig_events = get_trig_events(zl->event);
          for (el = trig_events; el; el = el->next)
            if (cause(el->event->nr, cause_event->nr) ||
                cause(el->event->nr, cause_event->nr->nr)) covered = TRUE;
          free_list(trig_events);
        }

        if (covered == FALSE) {
          for (el = candidate->llist; el; el = el->next)
            if (zl->event->signal == el->event->signal) covered = TRUE;
        }

        if (covered == FALSE) {
          restore_to_original_spec();

          if (ddebug) fprintf(dbg, "The term causes hazard\n");

          return FALSE;
        }
      }
    }
  }
  restore_to_original_spec();

  if (ddebug) fprintf(dbg, "The term does not cause any hazard\n");

  return TRUE;
}

void print_component(int i, array_t *varray, FILE *stream)
{
    int j, n;

    fprintf (stream,"   %d)",i);
    for (n=0; n < array_n(varray); n++) {
        j = array_fetch (int, varray, n);
        fprintf(stream," %s",h_events[j]->name);
    }
    fputs("\n",stream);
}

hlogic *
multiply_two_net_and_src(hlogic *net1, hlogic *net2, bool unique)
{
  hlogic *new_net=NULL;
  term *new_term, *term1, *term2;
  event_list *tl;

  new_net = alloc_net(NULL, NULL);
  for (term1 = net1->terms; term1; term1 = term1->next) 
    for (term2 = net2->terms; term2; term2 = term2->next) {
      new_term = alloc_term(NULL);
      for (tl = term1->llist; tl; tl = tl->next)
        if (unique) new_literal_unique(new_term, tl->event);
        else new_literal(new_term, tl->event);
      for (tl = term2->llist; tl ; tl = tl->next)
        if (unique) new_literal_unique(new_term, tl->event);
        else new_literal(new_term, tl->event);
      for (tl = term1->src_list; tl; tl = tl->next)
        add_list_unique(&new_term->src_list, tl->event);
      for (tl = term2->src_list; tl; tl = tl->next)
        add_list_unique(&new_term->src_list, tl->event);
/*
      new_term->next = new_net->terms;
      new_net->terms = new_term;
*/
      add_to_net_and_src(new_net, new_term, unique);
      delete_term(new_term);
  }
  return new_net;
}

void add_term_unique(hlogic *old, hlogic *net, term *cube, hlogic *sol_cubes)
{
  int sig_num, same_term, same_literal;
  int covered, found;
  term *t;
  event_list *tl, *sl;

  sig_num = num_of_signals(cube);

  for (t = sol_cubes->terms; t; t = t->next) {
    if (sig_num >= num_of_signals(t)) {
      covered = TRUE;
      for (tl = t->llist; tl && covered ; tl = tl->next) {
        found = FALSE;
        for (sl = cube->llist; sl && !found; sl = sl->next)
          if (IS_SAME_SIGNAL(tl->event, sl->event)) found = TRUE;
        if (!found) covered = FALSE;
      }
      if (covered) {
        delete_term(cube);
        return;
      }
    }
  }
    
  for (t = old->terms; t; t = t->next) {
    if (sig_num != num_of_signals(t)) continue;
    same_term = TRUE;
    for ( tl = t->llist; same_term && tl; tl = tl->next) {
      same_literal = FALSE;
      for (sl = cube->llist; !same_literal && sl; sl = sl->next)
        if (IS_SAME_SIGNAL(tl->event, sl->event)) same_literal = TRUE;
      if (!same_literal) same_term = FALSE;
    }
    if (same_term) {
      delete_term(cube);
      return;
    }
  }
  for (t = net->terms; t; t = t->next) {
    if (sig_num != num_of_signals(t)) continue;
    same_term = TRUE;
    for ( tl = t->llist; same_term && tl; tl = tl->next) {
      same_literal = FALSE;
      for (sl = cube->llist; !same_literal && sl; sl = sl->next)
        if (IS_SAME_SIGNAL(tl->event, sl->event)) same_literal = TRUE;
      if (!same_literal) same_term = FALSE;
    }
    if (same_term) {
      delete_term(cube);
      return;
    }
  }
  cube->next = net->terms;
  net->terms = cube;
}
  
hlogic * set_union_minimal(hlogic *s, hlogic *sum, hlogic *net, int unique)
{
  int min_nsigs=0x7FFFFFFF, nsigs;
  term *term;
  hlogic *newnet;

  for (term = s->terms; term; term = term->next) {
    nsigs = num_of_context_signals(net, term);
    if (nsigs < min_nsigs) min_nsigs = nsigs;
  }
  for (term = sum->terms; term; term = term->next) {
    nsigs = num_of_context_signals(net, term);
    if (nsigs < min_nsigs) min_nsigs = nsigs;
  }
 
  newnet = alloc_net(NULL, NULL); 
  newnet->event = sum->event;

  for (term = s->terms; term; term = term->next) {
    if (num_of_context_signals(net, term) == min_nsigs) 
      add_to_net(newnet, term, FALSE);
  }
  for (term = sum->terms; term; term = term->next) {
    if (num_of_context_signals(net, term) == min_nsigs) 
      add_to_net(newnet, term, FALSE);
  }
  delete_net(sum);
  return newnet;
}

void set_union(hlogic *s, hlogic *sum, int unique)
{
    term *term;

    if (!s) return;    
    for (term = s->terms; term; term = term->next)
        add_to_net(sum, term, unique);
}

void set_union_and_src(hlogic *s, hlogic *sum, int unique)
{
    term *term;

    if (!s) return;    
    for (term = s->terms; term; term = term->next)
        add_to_net_and_src(sum, term, unique);
}

void add_logic_list(hlogic *extra, hevent *t, hevent *dest)
{
    logic_list *newlogic;
    
    newlogic = ALLOC(logic_list, 1);
    allocated_mem[LOGIC_LIST] += sizeof(logic_list);
    newlogic->net = copy_net(extra);
    newlogic->net->event = dest;
    newlogic->next = t->extra;
    t->extra = newlogic;
}

int MG_distinct_extra_signals(hevent *curr, hevent *dest, hlogic **extra, 
                              event_list *E, int ndests, int pass)
{
  hlogic *S;
  term *term;
  xnode *xl;
  bool is_extra, reachable;
  event_list *tl;
  int min_nsigs=0x7FFFFFFF, nsigs;


  if (ddebug) fprintf(dbg, "find_extra_signals: curr = %s dest = %s\n", 
                     curr->name, dest->name);


  S = alloc_net(NULL, NULL);
  S->event = dest;
  reachable = FALSE;
     
  if (ndests == 1 && pass == 1) {
    for (xl = curr->xlist; xl; xl = xl->next) {
      if (xl->extra) {
        for (term = xl->extra->terms; term; term = term->next) {
          nsigs = num_of_signals(term);
          if (nsigs < min_nsigs) min_nsigs = nsigs;
        }
      }
    }
    for (xl = curr->xlist; xl; xl = xl->next) {
      if (xl->extra) {
        for (term = xl->extra->terms; term; term = term->next) {
          if (num_of_signals(term) == min_nsigs) {
            reachable = TRUE;
            add_to_net(S, term, FALSE);
          }
        }
      }
    }
  }
  else {
    for (xl = curr->xlist; xl; xl = xl->next) {
      if (xl->extra) {
        set_union(xl->extra, S, TRUE);
        reachable = TRUE;
      }
    }
  }

  if (reachable == FALSE) {
    *extra = NULL;
    return FALSE;
  }

  /* add the current event if it is not in E set */
  is_extra = TRUE;
  for (tl = E; tl; tl = tl->next)
    if (IS_SAME_SIGNAL(curr, tl->event)) is_extra = FALSE;

  if (is_extra)
    for (term = S->terms; term; term = term->next)
        new_literal_unique(term, curr);

  *extra = S;

  if (ddebug) {
    print_logic(dbg, "find_extra_signals: the final extra signals = ",
                       S, TRUE, SET_NET);
  }

  return TRUE;
}

int is_trigger(hevent *t, hevent *zi)
{
  event_list *trig_events, *cl;

  trig_events = get_trig_events(zi);
  for (cl = trig_events; cl; cl = cl->next)
    if (IS_SAME_SIGNAL(cl->event, t)) return TRUE;
  return FALSE;
}
 
int MG_find_all_paths_dfs(hevent *zi, hevent *source, hevent *dest, 
                          hevent *curr, hlogic **extra, event_list *E, 
                          event_list *Sn, event_list *Dn, int extra_sigs_needed, 
                          int ndests, int pass)
{
  int result, reachable;
  xnode *xl;
  term *new_term;
  char buf[256];
  logic_list *ll;


  if (ddebug) fprintf(dbg,"dfs: src = %s dest = %s curr = %s\n", 
                       source->name, dest->name, curr->name);


  curr->visited = TRUE;

  if (curr == dest) {

    if (ddebug) fprintf(dbg,"dfs: dest %s is visited\n", dest->name);

    *extra = alloc_net(NULL, NULL);
    new_term = alloc_term(*extra);
    curr->visited = FALSE;
    return TRUE;
  }
   
  for (xl = curr->xlist; xl; xl = xl->next) {
    if (xl->event->visited) {

      if (ddebug) fprintf(dbg,"dfs: %s is visited adready\n", xl->event->name);

      xl->extra = NULL;
    }
    else if (xl->event->selected) {
      reachable = FALSE;
      for (ll = xl->event->extra; ll; ll = ll->next) {
        if (!reachable && ll->net->event == dest) {
          reachable = TRUE;
          xl->extra = copy_net(ll->net);

          if (ddebug) {
            fprintf(dbg, "dfs: %s is evaluated already\n", curr->name);
            sprintf(buf, "~dfs: Extra signals from %s to %s: ", 
                           curr->name, dest->name);
            print_logic(dbg, buf, *extra, TRUE, SET_NET);
            fprintf(dbg,"\n");
          }

        }
      }
      if (reachable == FALSE) xl->extra = NULL;
    }
    else if (extra_sigs_needed && is_in_list(xl->event, Sn)) {

      if (ddebug) fprintf(dbg, "dfs: %s is a source node\n", xl->event->name);

      xl->extra = NULL;
    } 
    else {
      if (extra_sigs_needed) 
        MG_find_all_paths_dfs(zi, source, dest, xl->event, &(xl->extra), 
                            E, Sn, Dn, extra_sigs_needed, ndests, pass);
      else 
        MG_find_all_paths_dfs(zi, source, dest, xl->event, &(xl->extra), 
                            E, Sn, Dn, !is_trigger(curr, zi), ndests, pass);
    }
  }
        
  result = MG_distinct_extra_signals(curr, dest, extra, E, ndests, pass);


  if (ddebug) {
    if (*extra) {
      sprintf(buf, "~dfs: Extra signals from %s to %s: ", curr->name, 
                      dest->name);
      print_logic(dbg, buf, *extra, TRUE, SET_NET);
      fprintf(dbg,"\n");
    }
    else fprintf(dbg,"~dfs: %s is not reachable to %s\n",
                        curr->name, dest->name);
  }


  /* mark current node as not visited */
  curr->visited = FALSE;
  curr->selected = TRUE;
  if (result == TRUE) if (*extra) add_logic_list(*extra, curr, dest); 
  return result;
}

int sig_num_not_in_net_term(term *term, hlogic *net)
{
    int count = 0, exist;
    event_list *tl, *sl;
    
    for (tl = term->llist; tl; tl = tl->next) {
        exist = FALSE;
        for (sl = net->terms->llist; sl; sl = sl->next) 
            if (IS_REV_TYPE(tl->event, sl->event)) exist = TRUE;
        if (!exist) count++;
    }
    return count;
}

int is_stable_term(term *term, hevent *t)
{
    event_list *tl;

    for (tl = term->llist; tl; tl = tl->next) {
        if (!is_stable_events(tl->event, t)) {

          if (ddebug) print_list(dbg,"The term is not stable : ", term->llist);

          return FALSE;
        }
    }


    if (ddebug) print_list(dbg, "The term is stable : ", term->llist);

    return TRUE;
}

int is_shared_stable_term(term *term, event_list *events_set)
{
    event_list *tl, *el;

    for (tl = term->llist; tl; tl = tl->next) {
      for (el = events_set; el; el = el->next) 
        if (!is_stable_events(tl->event, el->event)) return FALSE;
    }
    return TRUE;
}

void get_all_cause_events(hevent *zi, hlogic *net,  
                              term *term, event_list **cause_events) 
{
    event_list *tl;
    int j;

    /* get the events causing zi */
    *cause_events = NULL;
    for (tl = term->llist; tl; tl = tl->next) {
        for (j = tl->event->signal->first; j <= tl->event->signal->last; j++)
            if (events[j]->dropped == FALSE && cause(h_events[j], zi)) 
                add_list(cause_events, h_events[j]);
    }
    for (tl = net->terms->llist; tl; tl = tl->next) {
        for (j = tl->event->signal->first; j <= tl->event->signal->last; j++)
            if (events[j]->dropped == FALSE && cause(h_events[j], zi)) 
                add_list(cause_events, h_events[j]);
    }
}

void get_cause_events(hevent *zi, term *term,                                                         event_list **cause_events)
{
    event_list *tl;
    int j;

    /* get the events causing zi */
    *cause_events = NULL;
    for (tl = term->llist; tl; tl = tl->next) {
        for (j = tl->event->signal->first; j <= tl->event->signal->last; j++)
            if (cause(h_events[j], zi)) 
                add_list(cause_events, h_events[j]);
    }
}

void 
find_down_trig_events(event_list *eventslist, event_list *trig_tl,
                          event_list *context_tl, hevent *zi, 
                          hevent *zj, event_list **down)
{
    event_list *tl, *sl;
    bool caused;


    if (ddebug) {
      fprintf(dbg, "zi = %s zj = %s\n", zi->name, zj->name);
      print_list(dbg, "find_down_trig_events ( eventslist ) : ", eventslist);
      print_list(dbg, "find_down_trig_events ( trig_tl ) : ", trig_tl);
      print_list(dbg, "find_down_trig_events ( context_tl ) : ", context_tl);
    }


    for (tl = eventslist; tl; tl = tl->next) {
      caused = FALSE;
      for (sl = trig_tl; sl && !caused; sl = sl->next) {
          if (sl->event->nr != tl->event->nr &&
              cause(sl->event->nr, tl->event->nr))
            caused = TRUE;
      }
      for (sl = context_tl; sl; sl = sl->next) {
          if (sl->event->nr != tl->event->nr && 
              cause(sl->event->nr, tl->event->nr))
             caused = TRUE;
      }
      if (!caused) add_list_unique(down, tl->event->nr);
    }
}

int is_later(event_list *down, event_list *latest_down)
{
    bool late, late_all;
    event_list *tl, *sl;

    late_all = TRUE;
    for (tl = down; tl; tl = tl->next) {
        late = FALSE;
        for (sl = latest_down; sl; sl = sl->next) 
            if (cause(sl->event, tl->event))
                late = TRUE;
        if (!late) late_all = FALSE;
    }
    if (late_all) return TRUE;
    else return FALSE;
}

void free_logic_list(struct logic_list *llist)
{
  struct logic_list *l, *free_l;

  for (l = llist; l; ) {
    free_l = l;
    l = l->next;
    if (free_l->net) {
      delete_net(free_l->net);
    }
    free(free_l);
    freed_mem[LOGIC_LIST] += sizeof(struct logic_list);
  }
}
 
hlogic * MG_convert_to_source_node_sets(event_list *Sn)
{
  hlogic *set=NULL;
  event_list *il;
  term_struct *new_term;

  if (Sn == NULL) return NULL;

  set = alloc_net(NULL, NULL);
  for (il = Sn; il; il = il->next) {
    new_term = alloc_term(set);
    new_literal(new_term, il->event);
  }
  return set;
}

hlogic *
MG_find_all_covering_cubes(hlogic *net, hevent *zi, hevent *zj, event_list *Sn, 
                           event_list **Sn1, event_list **Sn2, event_list **Sn3,
                           int ndests, event_list *E, event_list *Dest, int pass)
{
  event_list *Dn, *tl, *fl, **Src, *sl;
  event_list *il ;
  hlogic *extra=NULL, *s1=NULL, *s2=NULL, *s=NULL, *set=NULL, *W=NULL;
  hlogic *free_net=NULL;
  xnode *xl;
  bool reachable;
  bool reachable_all;
  term_struct *i_term, *terms, *t;
  int i, j, result, index, source;
  char buf[256];
  logic_list *ll;
  hlogic *S[100], *U[100];

  for (i = 1; i < nevents; i++) {
    h_events[i]->extra = NULL;
    for (xl = h_events[i]->xlist; xl; xl = xl->next) { 
      if (xl->extra) {
        delete_net(xl->extra); 
        xl->extra = NULL;
      }
    }
  }

  Src = (event_list **) malloc(sizeof(event_list *) * ndests);

  for (index = 0; index < ndests; index++) {
    S[index] = NULL;
    U[index] = NULL;
  }

  Dn = NULL;
  for (i = 0, j = 0, fl = Dest; fl; j++, fl = fl->next) {
    if (events_num(fl->event->signal) == 1) {
      source = FALSE;
      for (tl = Sn; tl; tl = tl->next) 
        if (fl->event == tl->event) source = TRUE;
      if (source) {
        ndests--;
        continue;
      }
    }
    add_list(&Dn, fl->event);
    Src[i] = NULL;
    for (sl = Sn3[j]; sl; sl = sl->next) add_list(&Src[i], sl->event);  
    for (sl = Sn1[j]; sl; sl = sl->next) add_list(&Src[i], sl->event);  
    for (sl = Sn2[j]; sl; sl = sl->next) add_list(&Src[i], sl->event);  
    S[i] = MG_convert_to_source_node_sets(Sn3[j]);
    if (!(Sn2[j] == NULL || Sn1[j] == NULL)) {
      s1 = MG_convert_to_source_node_sets(Sn1[j]);
      s2 = MG_convert_to_source_node_sets(Sn2[j]);
      s = multiply_two_net(s1, s2, FALSE);
      delete_net(s1);
      delete_net(s2);
      set_union(s, S[i], FALSE);
    }


    if (ddebug) {
      fprintf(dbg, "Source Nodes for the Dest %s : ", fl->event->name);
      print_logic(dbg, " ", S[i], TRUE, SET_NET);
    }

    i++;
  }

  if (ndests == 0) {
    W = alloc_net(NULL, NULL);
    t = alloc_term(W);
    for (fl = Dest; fl; fl = fl->next) add_list(&t->src_list, fl->event);
    return W;
  }

  for (j = 0, fl = Dn; fl; j++, fl = fl->next) {
    for (i = 1; i < nevents; i++) h_events[i]->selected = FALSE;
    for (tl = Src[j]; tl; tl = tl->next) {
      extra = NULL;
      for (i = 1; i < nevents; i++) h_events[i]->visited = FALSE;
    
      result = MG_find_all_paths_dfs(zi, tl->event, fl->event, tl->event, 
                                     &extra, E, Src[j], Dn, !is_trigger(tl->event, zi), ndests, pass);    
      if (result == TRUE && extra) 
        add_logic_list(extra, tl->event, fl->event); 


      if (ddebug)  
        if (result == TRUE && extra) {
           sprintf(buf, "p-graph: Extra signals from %s to %s: ", 
                        tl->event->name, fl->event->name);
           print_logic(dbg, buf, extra, TRUE, SET_NET);
           fprintf(dbg,"\n");
        }
        else fprintf(dbg,"\np-graph : %s is not reachable from %s\n\n", 
             fl->event->name, tl->event->name);

      if (extra) delete_net(extra);
      for (i = 1; i < nevents; i++) {
        for (xl = h_events[i]->xlist; xl; xl = xl->next) { 
          if (xl->extra) {
            delete_net(xl->extra); 
            xl->extra = NULL;
          }
        }
      }
    }
  }

  /* after deapth-first-search, all the source nodes in the  precedence graph 
     must contain all the sets of extra signals to arrive to each node in Dn */

  /* the source node 'Sn' are divided into three sets : Sn1, Sn2 and Sn3.
  FOREACH event 'e+/1' in E
    FOREACH event 's' in Sn
      if (s => zj AND !(s => e+/2)) s is in Sn1(e+/1)
      if (s || zj AND s => e+/2) add s is in Sn2(e+/1) */


  if (ddebug)  {
    for (il = Sn; il; il = il->next) {
      fprintf(dbg,"Initial node[%s] : \n", il->event->name);
      for (ll = il->event->extra; ll; ll = ll->next) {
            sprintf(buf, "Destination node[%s] : ", ll->net->event->name);
            print_logic(dbg, buf, ll->net, TRUE, SET_NET);
      }
    }
  }


  /* FOREACH event 'e-/1' in Dn
       S(e-/1) = Sn31 X ... X Sn3m  U  (Sn11 X ... X Sn1n) X (Sn21 X ... X Sn2k)
       T = S(e-/1) X S(e2-/1) X ... X S(en-/1)  */

  i = 0;
  free_net = NULL;
  for (fl = Dn; fl; fl = fl->next) {
    if (!S[i]) {
      i++; 
      continue;
    }
    U[i] = alloc_net(NULL, NULL);
    for (i_term = S[i]->terms; i_term; i_term = i_term->next) {
      reachable_all = TRUE;
      set = NULL;
      for (tl = i_term->llist; tl; tl = tl->next) {
        reachable = FALSE;
        for (ll = tl->event->extra; ll; ll = ll->next) 
          if (ll->net->event == fl->event) reachable = TRUE;
        if (!reachable) reachable_all = FALSE;
      }
      if (!reachable_all) continue;
      for (tl = i_term->llist; tl; tl = tl->next) {
        for (ll = tl->event->extra; ll; ll = ll->next) {
          if (fl->event != ll->net->event) continue;
          if (set == NULL) set = copy_net(ll->net);
          else {
             free_net = set;
             set = multiply_two_net_and_src(set, ll->net, TRUE);
             delete_net(free_net);
          }
          for (terms = set->terms; terms; terms = terms->next)
            add_list(&(terms->src_list), tl->event);

          if (ddebug) print_logic_and_src(dbg, "set: ", set, TRUE, SET_NET);

        }
      }
      if (set) {
        set_union_and_src(set, U[i], TRUE);  
      }
    }

    if (ddebug) sprintf(buf, "U[%d]: ", i);
    if (ddebug) print_logic_and_src(dbg, buf, U[i], TRUE, SET_NET);

    i++;
  }

  if (!U[0]) {

    if (ddebug) fprintf(dbg,"Can't find a cover: U[0] is NULL\n");

    return NULL;
  }

  W = copy_net(U[0]);
  for (i = 1; i < ndests; i++) {
    if (!U[i]) {

      if (ddebug) fprintf(dbg,"Can't find a cover: U[%d] is NULL\n", i);

      return NULL;
    }
    free_net = W;
    W = multiply_two_net_and_src(W, U[i], TRUE);
    delete_net(free_net);
  }

  /* free temporary list */
  for (index = 0; index < ndests; index++) {
    if (U[index]) delete_net(U[index]);
    if (S[index]) delete_net(S[index]);
  }

  for (i = 1; i < nevents; i++)
    if (h_events[i]->extra) free_logic_list(h_events[i]->extra); 

  if (Dn) free_list(Dn);
  for (i = 0; i < ndests; i++) if (Src[i]) free_list(Src[i]);
  if (Src) free(Src);

  return W;

}

int find_a_minimal_shared_covering_cube(hlogic *net, 
                               hlogic *candidate_set,
                               char * sm_shared_enabling_cube, 
                               event_list *events_set,
                               hevent *zi, hevent *zj,
                               int combo)
{
  int i, can_be_shared, nsharedevents=0, cost, minimal_cost, st_minimal_cost;
  int can_be_combinational, is_combinational;
  term *prev_term, *free_term, *result_term, *term;
  event_list *tl, *el, *cl, *cause_events;
  int is_all_combinational;

  for (el = events_set; el; el = el->next) nsharedevents++;

  /* get the minimal cost solutions */
  /* minimal_cost = infinite */
  minimal_cost = 0x7FFFFFFFL;
  st_minimal_cost = 0x7FFFFFFFL;
  /* FOREACH s in U
        s->cost = num_of_extra_sig(s)
        if (minimal_cost > s->cost) minimal_cost = s->cost */
  prev_term = NULL;
  for (term = candidate_set->terms; term; ) {
    can_be_shared = TRUE;
    for (tl = term->llist; tl; tl = tl->next) {
      if (sm_shared_enabling_cube[tl->event->signal->index] == 'X' ||
          event_type(tl->event) == RISING &&
          sm_shared_enabling_cube[tl->event->signal->index] == '1' ||
          event_type(tl->event) == FALLING &&
          sm_shared_enabling_cube[tl->event->signal->index] == '0')
         can_be_shared = FALSE;
         
    }
    if (can_be_shared) {
      cost = sig_num_not_in_net_term(term, net);
      if (cost < minimal_cost) minimal_cost = cost;
      if (is_shared_stable_term(term, events_set) && 
              cost < st_minimal_cost) 
        st_minimal_cost = cost;
      prev_term = term;
      term = term->next;
    }
    else {
      free_term = term;
      if (prev_term == NULL) candidate_set->terms = term;
      else prev_term->next = term;
      term = term->next;
      delete_term(free_term);
    }
  }


  if (ddebug) fprintf(dbg,"minimal cost = %d\n", minimal_cost);
  if (ddebug) fprintf(dbg,"st_minimal cost = %d\n", st_minimal_cost);
  if (ddebug) {
    i = 0;
    for (term = candidate_set->terms; term; term = term->next) 
      if (st_minimal_cost < 0x7FFFFFFFL &&
              sig_num_not_in_net_term(term, net) == st_minimal_cost &&
              is_shared_stable_term(term, events_set)) {
          fprintf(dbg,"Solution[%d] : ", i++);
          for (tl = term->llist; tl; tl = tl->next) 
            fprintf(dbg,"%s", tl->event->name);    
          fprintf(dbg,"\n");
        }
  }

  
  /* If there are many solutions with the same cost then 
     select a combinational one  */

  result_term = NULL;
  if (combo) {
    can_be_combinational = TRUE;
    for (el = events_set; el; el = el->next)
      if (trig_events_num(el->event->nr) > 1) 
         can_be_combinational = FALSE;
  
    /* find a combinational cube if one exists */
    if (can_be_combinational) {
      for (term = candidate_set->terms; term && !result_term; 
           term = term->next) {
        if (st_minimal_cost < 0x7FFFFFFFL &&
                sig_num_not_in_net_term(term, net) == st_minimal_cost &&
                is_shared_stable_term(term, events_set)) {
          is_all_combinational = TRUE;
          for (el = events_set; el; el = el->next) {
            zi = el->event;
            zj = zi->nr;
            get_all_cause_events(zi, net, term, &cause_events);

            if (ddebug) print_list(dbg, "cause_events: ", cause_events);

            is_combinational = TRUE;
            for (cl = cause_events; cl; cl = cl->next) {
              if (!(cause(cl->event, zj) || 
                    is_trig_event(cl->event->nr, zj))) 
                is_combinational = FALSE;
            }
            if (!is_combinational) {
              is_all_combinational = FALSE;
            }
          }
          if (is_all_combinational) {
              result_term = term;
              net->type = COMBI;
          }
        }
      }
    }
  }

  if (result_term == NULL) 
    for (term = candidate_set->terms; term && !result_term; term = term->next) 
      if (st_minimal_cost < 0x7FFFFFFFL &&
              sig_num_not_in_net_term(term, net) == st_minimal_cost &&
              is_shared_stable_term(term, events_set)) {
         result_term = term;
  }
          
  /* return latest_solution */
  if (result_term && st_minimal_cost < 0x7FFFFFFFL) {
    for (tl = result_term->llist; tl; tl = tl->next)
      new_literal_unique(net->terms, tl->event->nr);
    return TRUE;
  }
  else if (result_term) {
   printf("\nEXCEPTION : Non-Persistent Context Signals\n");
   fprintf(lg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   if (ddebug) fprintf(dbg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   return FALSE;
  }
  else {
    if (!find_reduced_state_graph(SYNTHESIS,SINGLE,Design)) exit(0);
    printf("Internal Error: Can't find a cover\n");
    fprintf(lg, "Internal Error: Can't find a cover\n");

    if (ddebug) fprintf(dbg, "Internal Error: Can't find a cover\n");

    return FALSE;
  }
}

int caused_by_trig_event(hevent *e, hevent *zj)
{
  event_list *el, *trig_events;

  trig_events = get_trig_events(zj);

  for (el = trig_events; el; el = el->next) 
    if (cause(el->event, e)) return TRUE;
  return FALSE;
}

int decomp_find_a_minimal_covering_cube(hlogic *net, hlogic *candidates, 
                                 hevent *zi, hevent *zj, int combo, 
                                 event_list *events_set, int pass)
{

  event_list *cl, *tl;
  term *term, *result_term;
  event_list *cause_events=NULL;
  int can_be_combinational, is_combinational, is_all_combinational;
  array_t *component;


  int minimal_cost, cost, st_minimal_cost;
  int i;


  if (ddebug) print_logic(dbg, "Initial Logic = ", net, TRUE, SET_NET);


  /* get the minimal cost solutions */
  minimal_cost = 0x7FFFFFFFL;
  st_minimal_cost = 0x7FFFFFFFL;
  for (term = candidates->terms; term; term = term->next) {
    cost = sig_num_not_in_net_term(term, net);
    if (cost < minimal_cost) minimal_cost = cost;
    if (is_stable_term(term, zi) && cost < st_minimal_cost) 
      st_minimal_cost = cost;
  }


  if (ddebug) {
    fprintf(dbg,"minimal cost = %d\n", minimal_cost);
    fprintf(dbg,"st_minimal cost = %d\n", st_minimal_cost);

    i = 0;
    for (term = candidates->terms; term; term = term->next) {
      if (st_minimal_cost < 0x7FFFFFFFL &&
             sig_num_not_in_net_term(term, net) == st_minimal_cost &&
             is_stable_term(term, zi) && check_hazard(term, events_set)) {
          fprintf(dbg,"Solution[%d] : ", i++);
          for (tl = term->llist; tl; tl = tl->next) 
            fprintf(dbg,"%s", tl->event->name);    
          fprintf(dbg,"\n");
      }
    }
  }


  result_term = NULL; 
  if (combo) {
    can_be_combinational = TRUE;
    for (i = 0; can_be_combinational && i < array_n(mg_components); i++) {
      component = array_fetch(array_t *, mg_components, i);
      set_to_a_component(component, i);
      set_nr_pr(i);

      if (!events[zi->index]->dropped && !events[zj->index]->dropped) 
          if (trig_events_num(zj) > 1) can_be_combinational = FALSE;

      restore_to_original_spec();
    }

    /* find a combinational cube if one exists */
    if (can_be_combinational) {
      for (term = candidates->terms; term && !result_term; term=term->next) {
        if (st_minimal_cost < 0x7FFFFFFFL &&
                sig_num_not_in_net_term(term, net) == st_minimal_cost &&
                is_stable_term(term, zi) &&
                check_hazard(term, events_set)) {
          is_all_combinational = TRUE;
          for (i = 0; is_all_combinational && i < array_n(mg_components); i++) {
            component = array_fetch(array_t *, mg_components, i);
            set_to_a_component(component, i);
            set_nr_pr(i);

            if (!events[zi->index]->dropped && !events[zj->index]->dropped) {
                get_all_cause_events(zi, net, term, &cause_events);

                if (ddebug) print_list(dbg, "cause_events: ", cause_events);

                is_combinational = TRUE;
                for (cl = cause_events; cl; cl = cl->next) {
                  if (!(cause(cl->event, zj) ||
                        is_trig_event(cl->event->nr, zj)) ||
                      (is_concur(cl->event->nr, zj) &&
                       !caused_by_trig_event(cl->event->nr, zj))) {

                     if (ddebug) {
                       if (!cause(cl->event, zj))
                         fprintf(dbg, "cause_event %s does not cause zj %s\n",
                                   cl->event->name, zj->name);
                       if (!is_trig_event(cl->event->nr, zj))
                         fprintf(dbg,"cause_event->nr %s does not trigger zj\n",
                                   cl->event->nr->name);
                       if (con_with_some_event(cl->event, zj))
                         fprintf(dbg, "cause event con with zj\n");
                    }

                    is_combinational = FALSE;
                  }
                }
                if (!is_combinational) {
                  is_all_combinational = FALSE;
                }
            }
            restore_to_original_spec();
          }
          if (is_all_combinational) {
              result_term = term;
              net->type = COMBI;
          }
        }
      }
    }
  }

  if (result_term == NULL)
    for (term = candidates->terms; term && !result_term; term = term->next)
      if (st_minimal_cost < 0x7FFFFFFFL &&
            sig_num_not_in_net_term(term, net) == st_minimal_cost &&
            is_stable_term(term, zi) &&
            check_hazard(term, events_set)) {
         result_term = term;
       }

  if (result_term == NULL)
    for (term = candidates->terms; term && !result_term; term = term->next)
      if (st_minimal_cost < 0x7FFFFFFFL &&
            sig_num_not_in_net_term(term, net) == st_minimal_cost &&
            is_stable_term(term, zi))  {
         result_term = term;
         new_literal_unique(result_term, zi);
       }

  /* return latest_solution */
  if (result_term && st_minimal_cost < 0x7FFFFFFFL) {
    for (tl = result_term->llist; tl; tl = tl->next)
      new_literal_unique(net->terms, tl->event->nr);

    if (ddebug) print_logic(dbg, "Final Logic = ", net, TRUE, SET_NET);

    return TRUE;
  }
  else if (pass == 2 && result_term) {
   printf("\nEXCEPTION : Non-Persistent Context Signals\n");
   fprintf(lg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   if (ddebug) fprintf(dbg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   return FALSE;
  }
  else if (pass == 2) {
    if (!find_reduced_state_graph(SYNTHESIS,SINGLE,Design)) exit(0);
    printf("Internal Error: Can't find a cover\n");
    fprintf(lg, "Internal Error: Can't find a cover\n");

    if (ddebug) fprintf(dbg, "Internal Error: Can't find a cover\n");

    return FALSE;
  }
  else return FALSE;
}

int find_a_minimal_covering_cube(hlogic *net, hlogic *W, 
                                 hevent *zi, hevent *zj, int combo)
{

  event_list *tl;
  term *term, *result_term;
  event_list *down=NULL, *latest_down=NULL;
  event_list *cause_events=NULL, *latest_cause_events=NULL;
  event_list *cl, *up, *free_down, *free_cause_events;
  int i=0;
  int is_combinational;
  int minimal_cost, cost, st_minimal_cost;


  if (ddebug) print_logic(dbg, "Initial Logic = ", net, TRUE, SET_NET);

  /* get the minimal cost solutions */
  /* minimal_cost = infinite */
  minimal_cost = 0x7FFFFFFFL;
  st_minimal_cost = 0x7FFFFFFFL;
  /* FOREACH s in U
        s->cost = num_of_extra_sig(s)
        if (minimal_cost > s->cost) minimal_cost = s->cost */
  for (term = W->terms; term; term = term->next) {
    cost = sig_num_not_in_net_term(term, net);
    if (cost < minimal_cost) minimal_cost = cost;
    if (is_stable_term(term, zi) && cost < st_minimal_cost) 
      st_minimal_cost = cost;
  }

  if (ddebug) fprintf(dbg,"minimal cost = %d\n", minimal_cost);
  if (ddebug) fprintf(dbg,"st_minimal cost = %d\n", st_minimal_cost);


  /* select a solution among the minimal covering solutions */
  result_term = NULL;
  if (combo && trig_events_num(zj) == 1) {
    for (term = W->terms; term; term = term->next) {
      if (st_minimal_cost < 0x7FFFFFFFL &&
                  sig_num_not_in_net_term(term, net) == st_minimal_cost &&
                  is_stable_term(term, zi)) {

        if (ddebug) {
          fprintf(dbg,"Solution[%d] : ", i);
          for (tl = term->llist; tl; tl = tl->next) 
            fprintf(dbg,"%s", tl->event->name);    
          fprintf(dbg,"\n");
        }

        get_all_cause_events(zi, net, term, &cause_events);

        if (ddebug) print_list(dbg, "cause_events: ", cause_events);

        is_combinational = TRUE;
        for (cl = cause_events; cl; cl = cl->next) {
          if (!(cause(cl->event, zj) || is_trig_event(cl->event->nr, zj)) ||
                 (is_concur(cl->event->nr, zj) && 
                  !caused_by_trig_event(cl->event->nr, zj)))
            is_combinational = FALSE;
        }
        if (is_combinational) {
          result_term = term;
          net->type = COMBI;
        }
      }
    }
  }

  if (result_term == NULL) {
    i = 0;
    up = get_trig_events(zi);
    cause_events = NULL;
    down = NULL;
    result_term = NULL;
    latest_down = NULL;
    latest_cause_events = NULL;
  
    for (term = W->terms; term; term = term->next) {
      if (st_minimal_cost < 0x7FFFFFFFL &&
                  sig_num_not_in_net_term(term, net) == st_minimal_cost &&
                  is_stable_term(term, zi)) {

        if (ddebug) {
          fprintf(dbg,"Solution[%d] : ", i);
          for (tl = term->llist; tl; tl = tl->next) 
            fprintf(dbg,"%s", tl->event->name);    
          fprintf(dbg,"\n");
        }

  
        /* find the latest one */
        cause_events = NULL;
        down = NULL;
        get_cause_events(zi, term, &cause_events);

        if (ddebug) print_list(dbg, "cause_events: ", cause_events);

        find_down_trig_events(cause_events, up, cause_events, zi, zj, &down);

        if (ddebug) print_list(dbg, "down: ", down);

  
        if (!result_term) {
          latest_down = down;
          result_term = term;
          latest_cause_events = cause_events;
        }
        else {
          if (is_later(down, latest_down)) {
            free_down = latest_down;
            free_cause_events = latest_cause_events;
            latest_down = down;
            latest_cause_events = cause_events;
            result_term = term;
          }
          else {
            free_down = down;
            free_cause_events = cause_events;
          }
          free_list(free_cause_events);
          free_list(free_down);
        }
        i++;
      }
      if (st_minimal_cost == 0x7FFFFFFFL && 
                  sig_num_not_in_net_term(term, net) == minimal_cost) {

        if (ddebug) fprintf(dbg,"NON ST Solution[%d] : ", i);
        for (tl = term->llist; tl; tl = tl->next) 
          if (ddebug) fprintf(dbg,"%s", tl->event->name);    
        if (ddebug) fprintf(dbg,"\n");

        if (!result_term) result_term = term;
        if (minimal_cost == 1 && is_trig_event(term->llist->event, zj))
          result_term = term;
        i++;
      }
    }
  
    /* check if the selected interval network is a combinational one */
    if (combo) {
      find_down_trig_events(up, up, latest_cause_events, zi, zj, 
              &latest_down);
      if (latest_down == NULL) {

      if (ddebug) fprintf(dbg, "Internal Error : latest_down == NULL\n");

      }

      if (ddebug) fprintf(dbg,"zi = %s zj = %s\n", zi->name, zj->name);
      if (ddebug) print_list(dbg, "latest_down", latest_down);

  
      if (latest_down && trig_events_num(zj) == 1 &&
                latest_down->next == NULL &&
                is_trig_event(latest_down->event, zj)) 
        net->type = COMBI;
      else 
        net->type = STORAGE;
    }
             
    if (latest_down) free_list(latest_down);
    if (latest_cause_events) free_list(latest_cause_events);
    if (up) free_list(up);
  }
  
  /* return latest_solution */
  if (result_term && st_minimal_cost < 0x7FFFFFFFL) {
    for (tl = result_term->llist; tl; tl = tl->next)
      new_literal_unique(net->terms, tl->event->nr);
    net->terms->src_list = result_term->src_list;
    result_term->src_list = NULL;

    if (ddebug) print_logic(dbg, "Final Logic = ", net, TRUE, SET_NET);

    return TRUE;
  }
  else if (result_term) {
   printf("\nEXCEPTION : Non-Persistent Context Signals\n");
   fprintf(lg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   if (ddebug) fprintf(dbg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   return FALSE;
  }
  else {
    if (!find_reduced_state_graph(SYNTHESIS,SINGLE,Design)) exit(0);
    printf("Internal Error: Can't find a cover\n");
    fprintf(lg, "Internal Error: Can't find a cover\n");

    if (ddebug) fprintf(dbg, "Internal Error: Can't find a cover\n");

    return FALSE;
  }
}

int check_persistency(event_list *events_set)
{
  event_list *el, *fl, *tl; 
  int persistent = TRUE;
  int j;
 
  for (el = events_set; el; el = el->next) {
    for (fl = events_set; fl; fl = fl->next) {
      if (el == fl) continue;
      for (tl = get_trig_events(el->event); tl; tl = tl->next) 
        for (j = 1; j < nevents - ndummy; j++)
          if (concurrency_table[fl->event->index][j] &&
                 events[j]->dropped == FALSE && 
                 h_events[j]->signal == tl->event->signal)
            persistent = FALSE;
    }
  }
  return persistent;
}

hlogic *
MG_find_a_single_cube(hlogic *result_net, event_list *events_set, 
                           int shared, int combo, FILE *pg)
{
  event_list **S1=NULL, **S2=NULL, **S3=NULL, *Sn, *E, *Dn, *l;
  event_list *el, *tl, *trigger_events;
  hevent *snr, *zi=NULL, *zj=NULL, *zk, *s, *t, *e, *event;
  hqueue *front, *rear;
  int i, index, ndests, found, result=FALSE;
  char buf[256];
  hlogic *interval_net[100];
  int k=0;
  char *sm_shared_enabling_cube=NULL;
  hlogic *W=NULL, *candidate_set=NULL, *free_net;
  term *c;
  int count = 0;
//  xnode *xl;


  print_memory();


  if (shared) {
    if (check_persistency(events_set) == FALSE) {
      printf("\nEXCEPTION: Persistency Violation!\n");
      return NULL;
    }
    sm_shared_enabling_cube = (char *) malloc(nsignals+1);
    strcpy(sm_shared_enabling_cube, EC_approximated[events_set->event->index]);
    for (el = events_set->next; el; el = el->next)
      for (i = 0; i < nsignals; i++) 
        if (EC_approximated[el->event->index][i] == 'X') 
          sm_shared_enabling_cube[i] = 'X';

    if (ddebug) fprintf(dbg, "Smallest shared enabling cube = %s\n", 
                            sm_shared_enabling_cube);

  }

  get_initial_logic(rules, h_events, nevents, result_net, events_set);

  for (el = events_set, k = 0; el; el = el->next, k++) {
    zi = el->event;
    zj = zi->nr;
    if (shared == FALSE) zk = zi;
    else {
      found = FALSE;
      event = zi->nr->nr;
      while (!found && event != zi) {
        for (tl = events_set; tl; tl = tl->next) {
          if (event == tl->event) found = TRUE;
        }
        if (!found) event = event->nr->nr;
      }
      assert (found);
      zk = event;
    }


    if (ddebug) fprintf(dbg,"MG_find_a_single_cube: zi = %s zj = %s zk = %s\n",
                          zi->name, zj->name, zk->name);
    if (ddebug) 
    {
      if (event_type(zi) == RISING) 
        print_logic(dbg, "Initial logic : ", result_net, TRUE, SET_NET);
      else 
        print_logic(dbg, "Initial logic : ", result_net, TRUE, RESET_NET);
    }


    front = rear = NULL;
    for (i = 0; i < nevents; i++) {
      h_events[i]->selected = FALSE;
      h_events[i]->xlist = NULL;
    }
  
    find_destination_nodes(zi, zj, zk, &E, &Dn);
    ndests = 0;
    for (l = Dn; l; l = l->next) ndests++;
    S1 = (event_list **) malloc(sizeof(event_list *) * ndests);
    S2 = (event_list **) malloc(sizeof(event_list *) * ndests);
    S3 = (event_list **) malloc(sizeof(event_list *) * ndests);
    for (i = 0; i < ndests; i++) {
      S1[i] = NULL;
      S2[i] = NULL;
      S3[i] = NULL;
      add_list(&(S3[i]), zi);
    }
  
    Sn = NULL;
    add_list(&Sn, zi);
    add_queue(zi, &front, &rear);
    zi->selected = TRUE;
  
    if (!gatelevel || get_max_nevents() == 1) {

      if (ddebug) fprintf(dbg, "!gatelevl && max_nevents = 1\n");

      for (i = 1; i < (nevents - ndummy); i++) {   
        if (events[i]->dropped) continue;
        s = h_events[i];
        if (cause(zi, s) && some_rev_event_cause(s, zi) &&
          cause(s, zj)) {
          add_list(&Sn, s);
          add_queue(s, &front, &rear);
          s->selected = TRUE;
        }
      }

      if (ddebug) print_list(dbg, "Sn = ", Sn);

      for (i = 0; i < ndests; i++) {
         S3[i] = copy_list(Sn);
         S1[i] = NULL;
         S2[i] = NULL;
      }

      if (ddebug) 
        for (i = 0; i < ndests; i++) print_list(dbg, "S3[%d] = ", S3[i]);

    }
    else {
      index = -1;
      trigger_events = get_trig_events(zi);
      for (l = trigger_events; l; l = l->next) {
        e = l->event;
        index++;
        for (i = 1; i < (nevents - ndummy); i++) {
          if (events[i]->dropped) continue;
          s = h_events[i];
          if (s->signal->nevents > 1 && cause(zi, s->pr) && 
              cause(zi, s->pr->pr) && cause(zi, s)) continue;
          if (is_concur(zj, e->nr->nr)) {

            if (ddebug) fprintf(dbg, "max_nevents > 1 && zj || e->nr->nr\n");

            if (cause(zi, s) && !is_concur(s, zj) && 
                   some_rev_event_cause(s, zi) &&
                   (cause(e, s) || cause(e->nr, s)) && 
                   !is_concur(s, e->nr->nr)) {
              add_list_unique(&(S3[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
            else if (cause(zi, s) && !is_concur(s, zj) && 
                         some_rev_event_cause(s, zi)) {
              add_list_unique(&(S1[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
            else if (cause(zi, s) && some_rev_event_cause(s, zi) &&
                    (cause(e, s) || cause(e->nr, s)) &&
                   !is_concur(s, e->nr->nr)) {
              add_list_unique(&(S2[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
          }
          else {

            if (ddebug) fprintf(dbg,"s = %s max_nevents > 1 && NOT (zj || e->nr->nr)\n", s->name);
            if (ddebug) {
              fprintf(dbg, "e = %s\n", e->name);
              fprintf(dbg, "s = %s\n", s->name);
              if (!cause(zi,s)) fprintf(dbg, "zi does not cause s\n");
              if (!cause(s, zj)) fprintf(dbg, "s does not cause zj\n");
              if (!some_rev_event_cause(s, zi)) fprintf(dbg, "some_rev_event_cause does not cause zi\n");
              if (!cause(e, s)) fprintf(dbg, "e does not cause s\n");
              if (!cause(e->nr, s)) fprintf(dbg, "e->nr does not cause s\n");
              if (is_concur(s, e->nr->nr)) fprintf(dbg, "is concur\n");
            }

            if (cause(zi, s) && !is_concur(s, zj) && 
                   some_rev_event_cause(s, zi) &&
                   (cause(e, s) || cause(e->nr, s)) &&
                   !is_concur(s, e->nr->nr)) {
              add_list_unique(&(S3[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
          }
        }
      }
      free_list(trigger_events);
    }
  

    if (ddebug) {
      print_list(dbg, "Sn = ", Sn);
      for (i = 0; i < ndests; i++) {
        sprintf(buf, "S1[%d] = ", i);
        print_list(dbg, buf, S1[i]);
      }
      for (i = 0; i < ndests; i++) {
        sprintf(buf, "S2[%d] = ", i);
        print_list(dbg, buf, S2[i]);
      }
      for (i = 0; i < ndests; i++) {
        sprintf(buf, "S3[%d] = ", i);
        print_list(dbg, buf, S3[i]);
      }
    }

    /* expand the  precedence graph */
    /* FOREACH unprocessed node s* of X */
    while ((s = delete_queue(&front, &rear))) {
      snr = s->nr;
      for (i = 1; i < (nevents - ndummy); i++) { 
        if (events[i]->dropped) continue;
        t = h_events[i];
        if ((cause(s, t) || is_concur(t, s)) &&
            cause(t, snr) && some_rev_event_cause(t, zi)) {
          add_to_xlist(s, t);
          if (!t->selected) {
             add_queue(t, &front, &rear);
             t->selected = TRUE;
          }
        }
      }
    }
    if (verbose) {
      fprintf(pg, "\nThe Precedence Graph for the interval %s => %s\n", 
                zi->name, zj->name);
      print_precedence_graph(Sn, Dn, pg);
    }

    W = MG_find_all_covering_cubes(result_net,zi,zj,Sn,S1,S2,S3,ndests,E,Dn,2);

    if (!W) {

      if (ddebug) fprintf(dbg,"Can't find any cover\n");

      return NULL;
    }
    else 

      if (ddebug) print_logic(dbg, "All covering cubes : ", W, TRUE, SET_NET);


    for (c = W->terms; c; c = c->next) count++;

    if (ddebug) fprintf(dbg, "Number of all covering cubes = %d\n", count);


    for (i = 0; i < ndests; i++) {
      if (S1[i]) free_list(S1[i]);
      if (S2[i]) free_list(S2[i]);
      if (S3[i]) free_list(S3[i]);
    }
    free_list(Sn);
    free_list(Dn);
    free_list(E);
    free_xgraph();
    free(S1);
    free(S2);
    free(S3);

    if (shared) interval_net[k] = copy_net(W);
  }

  if (shared) {
    candidate_set = copy_net(interval_net[0]);
    for (i = 1; i < k; i++) {
      free_net = candidate_set;
      candidate_set = multiply_two_net(candidate_set, interval_net[i], TRUE);
      delete_net(free_net);
    }
    for (i = 0; i < k; i++) delete_net(interval_net[i]);
  }

  if (!shared) {
    result = find_a_minimal_covering_cube(result_net, W, zi, zj, combo); 

    if (ddebug) {
      sprintf(buf, "\nSingle Cube for %s -> %s = ", zi->name, zj->name);
      print_logic(dbg, buf, result_net, TRUE, SET_NET);
      fprintf(dbg, "\n");
    }

    delete_net(W);
  }
  else {
    result = find_a_minimal_shared_covering_cube(result_net, candidate_set, 
                  sm_shared_enabling_cube, events_set, zi, zj, combo); 
    delete_net(candidate_set);
  }

  if (result == TRUE) return result_net;
  else return NULL;
}

int is_comb_candidate(hevent *zi)
{
  event_list *tl, *trigger_events;
  int candidate = TRUE;

  if (!marked_graph) return FALSE;

  trigger_events = get_trig_events(zi->nr);
  for (tl = trigger_events; tl; tl = tl->next) {
    if (events_num(tl->event->signal) > 1) candidate = FALSE;
      if (!(cause(zi,tl->event->pr) ||
            is_trig_event(tl->event->pr,zi)))
        candidate = FALSE;
  }
  free_list(trigger_events);
  return candidate;
}

event_list *get_rising_events(hlogic *net, hevent *zi)
{
    event_list *tl, *start_events=NULL;
    int i;

    for (tl = net->terms->llist; tl; tl = tl->next)
      for (i = h_signals[tl->event->signal->index]->first;
           i <= h_signals[tl->event->signal->index]->last; i++)
        if (cause(h_events[i], zi)) add_list_unique(&start_events, h_events[i]);
    return start_events;
}

int is_before_falling_events(hevent *s, event_list *start_events)
{
  event_list *tl;

  for (tl = start_events; tl; tl = tl->next)
    if (!(cause(tl->event, s) && !is_concur(tl->event->nr, s))) 
      return FALSE;
  return TRUE;
}
    
void comb_get_E_and_F(hlogic *net, hevent *zi, hevent *zj, 
        event_list *I, event_list **E, event_list **F)
{
    event_list *tl;

    (*E) = (*F) = NULL;
    
    *E = get_trig_events(zj);
    for (tl = *E; tl ; tl = tl->next) {
      if (!(events_num(tl->event->pr) == 1 && 
           is_trig_event(tl->event->pr, zi) &&
           trig_events_num(zi) == 1 && 
           num_of_signals(net->terms) == 1))
         add_list(F, tl->event->pr);
    }


    if (ddebug)     print_list(dbg, "Destination Nodes : ", *F);

}

int comb_distinct_extra_signals(hevent *curr, hevent *dest, 
                  hlogic **extra, event_list *E)
{
  hlogic *S;
  term *term;
  xnode *xl;
  bool is_extra, reachable;
  event_list *tl;

  S = alloc_net(NULL, NULL);
  S->event = dest;
  reachable = FALSE;

  for (xl = curr->xlist; xl; xl = xl->next) {
    if (xl->extra) {
      set_union(xl->extra, S, TRUE);  
      reachable = TRUE;
    }
  }

  if (reachable == FALSE) {
    *extra = NULL;
    return FALSE;
  }

  /* add the current event if it is not in E set */
  is_extra = TRUE;
  for (tl = E; tl; tl = tl->next)
    if (IS_SAME_SIGNAL(curr, tl->event)) is_extra = FALSE;

  if (is_extra)
    for (term = S->terms; term; term = term->next)
      new_literal_unique(term, curr);

  *extra = S;


  if (ddebug) print_logic(dbg, "comd_distinct_extra_signals: ", 
                            S, TRUE, SET_NET);


  return TRUE;
}


int comb_find_all_paths_dfs(hevent *dest, hevent *curr, hlogic **extra, 
                             event_list *E)
{
  int reachable, result;
  xnode *xl;
  term *new_term;
  logic_list *ll;
  char buf[256];


  if (ddebug) 
    fprintf(dbg,"comb_dfs: dest= %s curr= %s\n", dest->name, curr->name);


  curr->visited = TRUE;

  if (curr == dest) {

    if (ddebug) fprintf(dbg,"comb_dfs: dest %s is visited\n", curr->name);

    *extra = alloc_net(NULL, NULL);
    new_term = alloc_term(*extra);    
    return TRUE;
  }

  for (xl = curr->xlist; xl; xl = xl->next) {
    if (xl->event->visited) {
      xl->extra = NULL;
    }
    else if (xl->event->selected) {
      reachable = FALSE;
      for (ll = xl->event->extra; ll; ll = ll->next) {
        if (!reachable && ll->net->event == dest) {
          reachable = TRUE;
          xl->extra = copy_net(ll->net);

          if (ddebug) {
            fprintf(dbg, "dfs: %s is evaluated already\n", curr->name);
            sprintf(buf, "~dfs: Extra signals from %s to %s: ",
                           curr->name, dest->name);
            print_logic(dbg, buf, *extra, TRUE, SET_NET);
            fprintf(dbg,"\n");
          }

        } 
      }
    }
    else {
      comb_find_all_paths_dfs(dest, xl->event, &(xl->extra), E);
    }
  }

  result = comb_distinct_extra_signals(curr, dest, extra, E);

  curr->visited = FALSE;
  curr->selected = TRUE;
  if (result == TRUE) if (*extra) add_logic_list(*extra, curr, dest);
  return TRUE;
}


event_list *
zgraph_minimal_covering(hlogic *net, hevent *zi, 
                                hevent *zj, event_list *I, 
                                event_list *E, event_list *F, 
                                event_list *start_events)
{
  event_list *enabling_events, *tl, *fl;
  event_list *il;
  hlogic *extra, *set, *W, *free_net;
  xnode *xl;
  bool reachable;
  bool reachable_all;
  term *term, *i_term, *result_term;
  int i, index;
  char buf[256];
  logic_list *ll;
  hlogic *S[100], *U[100];
  int minimal_cost, cost, st_minimal_cost;
  event_list *extra_sig=NULL;

  for (i = 1; i < nevents; i++) {
    h_events[i]->selected = FALSE;
    h_events[i]->extra = NULL;
  }

  for (fl = F; fl; fl = fl->next) {
    for (i = 1; i < nevents; i++) h_events[i]->selected = FALSE;
    for (tl = I; tl; tl = tl->next) {
      tl->event->extra = NULL;
      extra = NULL;
      for (i = 1; i < nevents; i++) {
        h_events[i]->visited = FALSE;
        for (xl = h_events[i]->xlist; xl; xl = xl->next) {
          if (xl->extra) {
            delete_net(xl->extra);
            xl->extra = NULL;
          }
        }
      }

      comb_find_all_paths_dfs(fl->event, tl->event, &extra, E);    
      if (extra) add_logic_list(extra, tl->event, fl->event); 


      if (ddebug) {
        if (extra) {
          sprintf(buf, "Extra signals from %s to %s: ", tl->event->name, 
                        fl->event->name);
          print_logic(dbg, buf, extra, TRUE, SET_NET);
          fprintf(dbg,"\n");
        }
        else fprintf(dbg,"%s is not reachable from %s\n", fl->event->name, 
                        tl->event->name);
      }


      if (extra) delete_net(extra); 
    }
  }

  index = -1;
  for (fl = F; fl; fl = fl->next) index++;
  
  for (i = 0; i <= index; i++) {
    S[i] = NULL;
    U[i] = NULL;
  }

  for (i = 0; i <= index; i++) {
    S[i] = MG_convert_to_source_node_sets(I);

    if (ddebug) sprintf(buf, "S[%d]: ", i);
    if (ddebug) print_logic(dbg, buf, S[i], TRUE, SET_NET);

  }


  for (il = I; il; il = il->next) {
    if (ddebug) fprintf(dbg,"Initial node[%s] : \n", il->event->name);
    for (ll = il->event->extra; ll; ll = ll->next) {
      if (ddebug) sprintf(buf, "Destination node[%s] : ", ll->net->event->name);
      if (ddebug) print_logic(dbg, buf, ll->net, TRUE, SET_NET);
    }
  }


  i = 0;
  for (fl = F; fl; fl = fl->next) {
    if (!S[i]) {
      i++; 
      continue;
    }
    U[i] = alloc_net(NULL, NULL);
    for (i_term = S[i]->terms; i_term; i_term = i_term->next) {
      reachable_all = TRUE;
      set = NULL;
      for (tl = i_term->llist; tl; tl = tl->next) {
        reachable = FALSE;
        for (ll = tl->event->extra; ll; ll = ll->next) 
            if (ll->net->event == fl->event) 
                reachable = TRUE;
        if (!reachable) reachable_all = FALSE;
      }
      if (!reachable_all) continue;
      for (tl = i_term->llist; tl; tl = tl->next) 
        for (ll = tl->event->extra; ll; ll = ll->next) {
            if (fl->event != ll->net->event) continue;
            if (set == NULL) set = copy_net(ll->net);
            else {
              free_net = set;
              set = multiply_two_net(set, ll->net, TRUE);
              delete_net(free_net);
            }

            if (ddebug) print_logic(dbg, "set: ", set,TRUE, SET_NET);

        }
      if (set) {
        set_union(set, U[i], TRUE);  
        delete_net(set);
      }
    }

    if (ddebug) sprintf(buf, "U[%d]: ", i);
    if (ddebug) print_logic(dbg, buf, U[i], TRUE, SET_NET);

    i++;
  }

  if (!U[0]) {

    if (ddebug) 
      fprintf(dbg,"comb: Can't find a minimal covering: U[0] is NULL\n");

    return NULL;
  }

  W = copy_net(U[0]);
  for (i = 1; i <= index; i++) {
    if (!U[i]) {

      if (ddebug) 
        fprintf(dbg,"comb: Can't find a minimal covering: U[%d] is NULL\n",
                   i);

      return NULL;
    }
    free_net = W;
    W = multiply_two_net(W, U[i], TRUE);
    delete_net(free_net);
  }
  if (!W) {

    if (ddebug) fprintf(dbg,"comb: Can't find a minimal covering: W is NULL\n");

    return NULL;
  }


  if (ddebug) print_logic(dbg, "W: ", W, TRUE, SET_NET);


  /* get the minimal cost solutions */
  /* minimal_cost = infinite */
  minimal_cost = 0x7FFFFFFFL;
  st_minimal_cost = 0x7FFFFFFFL;
  /* FOREACH s in U
    s->cost = num_of_extra_sig(s)
    if (minimal_cost > s->cost) minimal_cost = s->cost */
  for (term = W->terms; term; term = term->next) {
    cost = num_of_signals(term);
    if (cost < minimal_cost) minimal_cost = cost;
    if (is_stable_term(term, zi) && cost < st_minimal_cost) 
      st_minimal_cost = cost;
  }

  if (ddebug) fprintf(dbg,"minimal cost = %d\n", minimal_cost);
  if (ddebug) fprintf(dbg,"st_minimal cost = %d\n", st_minimal_cost);


  /* select a solution among the minimal covering solutions */
  /* If this interval is a candidate for a combinational one and
    there are many solutions with the same cost then 
    select the latest event among the set I for the construction of
    combinational network */
  /* FOREACH s in S
    if (s->cost == minimal_cost)
      if (s is later than latest_solution) latest_solutuion = s */
  result_term = NULL;
  i = 0;

  for (term = W->terms; term; term = term->next) {
    if (st_minimal_cost < 0x7FFFFFFFL &&
        num_of_signals(term) == st_minimal_cost &&
        is_stable_term(term, zi)) {

      if (ddebug) fprintf(dbg,"Solution[%d] : ", i);
      for (tl = term->llist; tl; tl = tl->next) 
        if (ddebug) fprintf(dbg,"%s", tl->event->name);    
      if (ddebug) fprintf(dbg,"\n");

      if (!result_term) result_term = term;
      i++;
    }
    if (st_minimal_cost == 0x7FFFFFFFL && 
        num_of_signals(term) == minimal_cost) {

      if (ddebug) fprintf(dbg,"NON ST Solution[%d] : ", i);
      for (tl = term->llist; tl; tl = tl->next) 
        if (ddebug) fprintf(dbg,"%s", tl->event->name);    
      if (ddebug) fprintf(dbg,"\n");

      if (!result_term) result_term = term;
      if (minimal_cost == 1 && 
        is_trig_event(term->llist->event, zj))
        result_term = term;
      i++;
    }
  }

  /* return latest_solution */
  if (result_term && st_minimal_cost < 0x7FFFFFFFL) {
    enabling_events = get_trig_events(zj);
    for (tl = enabling_events; tl; tl = tl->next) {
      if (!(events_num(tl->event) == 1 &&
            is_trig_event(tl->event->pr, zi)))
        add_list_unique(&extra_sig, tl->event->pr);
    }
    free_list(enabling_events);
    for (tl = result_term->llist; tl; tl = tl->next)
      add_list_unique(&extra_sig, tl->event);

    if (ddebug) print_list(dbg, "comb: resultant extra sig : ", extra_sig);

  }
  else {

    if (ddebug) fprintf(dbg,"comb: Can't find a combinational network\n");

    extra_sig = NULL;
  }

  for (i = 0; i <= index; i++) {
    if (U[i]) delete_net(U[i]);
    if (S[i]) delete_net(S[i]);
  }
  for (i = 1; i < nevents; i++) {
    if (h_events[i]->extra) {
      free_logic_list(h_events[i]->extra);
    }
    for (xl = h_events[i]->xlist; xl; xl = xl->next) {
      if (xl->extra) {
        delete_net(xl->extra);
        xl->extra = NULL;
      }
    }
  }
  if (W) delete_net(W);
  return extra_sig;
}

int comb_find_min_extra_sig(hlogic *net, event_list **extra_sig, FILE *pg)
{
  int i;
  event_list *tl, *I, *E, *F;
  hevent *zi, *zj, *s, *t;
  hqueue *front, *rear;
  event_list *new_extra, *rising_events;

    *extra_sig = NULL;
    zi = net->event;
    zj = zi->nr;


  if (ddebug) fprintf(dbg,"comb_find_min_extra_sig: zi = %s zj = %s\n", zi->name, zj->name);


    front = rear = NULL;
    for (i = 1; i < nevents; i++) {
        h_events[i]->selected = FALSE;
        h_events[i]->xlist = NULL;
    }

    I = NULL;
    rising_events = get_rising_events(net, zi);


    if (ddebug) print_list(dbg, "comb_find_min_extra_sig: rising_events: ",
                                  rising_events);


    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped) continue;
      s = h_events[i];
      if (events_num(s) == 1 && cause(zi, s) && cause(s->nr, zj) &&
           is_before_falling_events(s, rising_events))
      { 
        add_list(&I, s);
        add_queue(s, &front, &rear);
        s->selected = TRUE;
      }
    }

    /* expand the precedence graph */
    while ((s = delete_queue(&front, &rear))) {
      for (i = 1; i < (nevents - ndummy); i++) {
        if (events[i]->dropped) continue;
        t = h_events[i];
        if (events_num(t) == 1 && cause(zi, t) &&
            (cause(s, t) || is_concur(t, s)) && 
            cause(t, s->nr) && cause(t->nr, zj)) 
        {
          add_to_xlist(s, t);
          if (!t->selected) {
            add_queue(t, &front, &rear);
            t->selected = TRUE;
          }
        }
      }
    }

    comb_get_E_and_F(net, zi, zj, I, &E, &F);
    if (ddebug) {
      fprintf(pg, "The Precedence Graph for combinational network\n");
      print_precedence_graph(I, F, pg);
    }

    new_extra = zgraph_minimal_covering(net,zi,zj,I,E, F, rising_events);
    if (new_extra) {
      for (tl = new_extra; tl; tl = tl->next)
        add_list_unique(extra_sig, tl->event);
      free_list(I);
      free_list(E);
      free_list(F);
      free_xgraph();
      return TRUE;
    }
    else {
      free_list(*extra_sig);
      *extra_sig = NULL;
      free_list(I);
      free_list(E);
      free_list(F);
      free_xgraph();
      return FALSE;
    }
}

void  find_a_multi_cube_comb_net(hlogic *net, event_list *events_set, FILE *pg) 
{
    term *term;
    event_list *extra_sig=NULL, *tl;

    if (events_set->next) return;
    if (!is_comb_candidate(net->event))  return; 
    if (comb_find_min_extra_sig(net, &extra_sig, pg)) {
        for (tl = extra_sig; tl; tl = tl->next) {
            term = alloc_term(net);
            new_literal_unique(term, tl->event);
        }
        net->type = COMBI;
        free_list(extra_sig);
    }
}

void print_cause_events(FILE *fp)
{
    char buf[100];
    int i, j;

    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped) continue;
      sprintf(buf, "%s timed causes : ", h_events[i]->name);
      print80(fp, buf, TRUE);
      for (j = 1; j < (nevents - ndummy); j++) {
        if (events[j]->dropped) continue;
        if (cause(h_events[i], h_events[j])) {
            sprintf(buf, "%s ", h_events[j]->name);
            print80(fp, buf, FALSE);
        }
      }
      fprintf(fp,"\n");
    }
    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped) continue;
      sprintf(buf, "%s timed caused by : ", h_events[i]->name);
      print80(fp, buf, TRUE);
      for (j = 1; j < (nevents - ndummy); j++) {
        if (events[j]->dropped) continue;
        if (cause(h_events[j], h_events[i])) {
            sprintf(buf, "%s ", h_events[j]->name);
            print80(fp, buf, FALSE);
        }
      }
      fprintf(fp,"\n");
    }
}

int has_no_concurrent_event(int i)
{
  int j;

  for (j = 1; j < nevents - ndummy; j++)
    if (concurrency_table[i][j]) return FALSE;
  return TRUE;
}

void find_smallest_enabling_cubes()
{
  int i, j;
//  event_list *l;

  EC_approximated = (char **) malloc((nevents - ndummy) * sizeof(char *));
  for (i = 0; i < (nevents - ndummy); i++) 
    EC_approximated[i] = (char *) malloc(nsignals+1);
  for (i = 0; i < (nevents - ndummy); i++) {
    for (j = 0; j < nsignals; j++) EC_approximated[i][j] = 'X';
    EC_approximated[i][nsignals] = '\0';
  }

  for (i = 1; i < nevents - ndummy; i++) { 
    if (events[i]->dropped) continue;
    for (j = 1; j < nevents - ndummy; j++) {
      if (cause(h_events[j], h_events[i])) {
        if (h_events[j]->event_type == RISING)
           EC_approximated[i][h_events[j]->signal->index] = '1';
        else
           EC_approximated[i][h_events[j]->signal->index] = '0';
      }
    }
    if (h_events[i]->event_type == RISING)
       EC_approximated[i][h_events[i]->signal->index] = '0';
    else
       EC_approximated[i][h_events[i]->signal->index] = '1';
    for (j = 1; j < (nevents - ndummy); j++) 
      if (is_concur(h_events[i], h_events[j]))
        EC_approximated[i][h_events[j]->signal->index] = 'X';
    EC_approximated[i][nsignals] = '\0';

    if (ddebug) fprintf(dbg, "smallest enabling cube of %s = %s\n", 
                          h_events[i]->name, EC_approximated[i]);
    if (ddebug) fprintf(dbg, "smallest enabling cube of %s = %s\n", 
            h_events[i]->name, EC_approximated[i]);

  }
}

void find_enstate(hlogic *net, char * state)
{
  int j;

  for (j = 0; j < nsignals; j++) state[j] = 'X';
  state[nsignals] = '\0';
}

void make_a_region(int i, regionADT *regions, hlogic *net, int net_type, 
              int logic_type, int ninpsig, bool *comb)
{
  struct region_tag *region;
  term *t;
  literal *l;
  int region_index;
  char *cover;
  int j;

  if (net_type == SET_NET) region_index = 2*i+1;
  else region_index = 2*i+2;

  if (logic_type == COMBI) {
    if (net_type == SET_NET) comb[i] = TRUE;
    else comb[i+nsignals] = TRUE;
  }

  for (t=net->terms; t; t = t->next) {
    region = (struct region_tag *) malloc(sizeof(struct region_tag));
    region->link = regions[region_index];
    regions[region_index] = region;
    region->enstate = (char *) malloc(nsignals+1);
    region->switching = NULL;
    region->enablings = NULL;
    region->tsignals = NULL;
    region->primes = NULL;
    region->implied = NULL;
    region->nprimes = 0;
    region->nvprimes = 0;
    region->context_table = NULL;
    region->decomp = NULL;
    region->covers = NULL;

    find_enstate(net, region->enstate);
 
    cover = (char *) malloc(nsignals+1);
    cover[nsignals] = '\0';
    for (j = 0; j < nsignals; j++) {
      cover[j] = 'X';
    }
    for(l=t->llist; l; l = l->next) {
      if (l->event->event_type == RISING) 
         cover[l->event->signal->index] = '1';
      else  
         cover[l->event->signal->index] = '0';
    }
    region->cover = cover;


    if (ddebug) fprintf(dbg, "net->event = %s Cover = %s\n", 
            net->event->name, cover);

  }
}

void gen_prs_file(char command, regionADT *regions, FILE *fp, 
                       int combo, int ninpsig, bool *comb)
{
    stjung_signal *s;
    int i, set_literal, reset_literal;
    int total_literal_number = 0, set_terms=0, reset_terms=0;
    logic_list *nl;
    event_list *ll;
    term *term;

    /* FOREACH output signal
        if (generate combinational circuits) 
           check the net type
           count the literal number of set and reset network
           output the network which has less literals 
        else
           output the set and reset network
    */


    for (i = ninpsig; i < nsignals; i++) {
        s = h_signals[i];
        set_literal = 0;
        reset_literal = 0;
        set_terms = 0;
        reset_terms = 0;
        s->setnet_type = COMBI;
        for (nl = s->set; nl; nl = nl->next) 
            if (nl->net->type == STORAGE) s->setnet_type = STORAGE;
        s->resetnet_type = COMBI;
        for (nl = s->reset; nl; nl = nl->next)
              if(nl->net->type == STORAGE) s->resetnet_type = STORAGE;

        set_literal=0;
        for (nl = s->set; nl; nl = nl->next)
            for (term = nl->net->terms; term; term = term->next) {
                set_terms++;
                for (ll = term->llist; ll; ll = ll->next) set_literal++;
            }
        
        reset_literal=0;
        for (nl = s->reset; nl; nl = nl->next) 
            for (term = nl->net->terms; term; term = term->next) {
                reset_terms++;
                for (ll = term->llist; ll; ll = ll->next) reset_literal++;
            }


        if (ddebug) fprintf(dbg, "set_literal = %d reset_literal = %d\n", 
                set_literal, reset_literal);
        if (ddebug) fprintf(dbg, "set_term = %d reset_term = %d\n", 
                set_terms, reset_terms);

        if (combo && s->setnet_type == COMBI && 
                     s->resetnet_type==STORAGE) {
          for (nl = s->set; nl; nl = nl->next) {
            print_logic_to_prs(fp, nl->net, SET_NET, COMBI);
            make_a_region(i, regions, nl->net, SET_NET, COMBI, 
                            ninpsig,comb);   
          }
          total_literal_number += set_literal;
        }
        else if (combo && s->setnet_type == STORAGE && 
                          s->resetnet_type == COMBI) {
          for (nl = s->reset; nl; nl = nl->next) {
            print_logic_to_prs(fp, nl->net, RESET_NET, COMBI);
            make_a_region(i, regions, nl->net, RESET_NET, COMBI, 
                           ninpsig,comb);   
          }
          total_literal_number += reset_literal;
        }
        else if (combo && s->setnet_type == COMBI && 
                          s->resetnet_type==COMBI){
          if ((set_literal == reset_literal && set_terms <= reset_terms) ||
               set_literal < reset_literal) {
            for (nl = s->set; nl; nl = nl->next) {
              print_logic_to_prs(fp, nl->net, SET_NET, COMBI);
              make_a_region(i, regions, nl->net, SET_NET, COMBI, 
                               ninpsig,comb);   
            }
            total_literal_number += set_literal;
          }
          else {
            for (nl = s->reset; nl; nl = nl->next) {
              print_logic_to_prs(fp, nl->net, RESET_NET, COMBI);
              make_a_region(i, regions, nl->net, RESET_NET, COMBI, 
                             ninpsig,comb);   
            }
            total_literal_number += reset_literal;
          }
        }
        else {
          for (nl = s->set; nl; nl = nl->next) {
            print_logic_to_prs(fp, nl->net, SET_NET, STORAGE);
            make_a_region(i, regions, nl->net, SET_NET, STORAGE, 
                             ninpsig,comb);   
          }
          for (nl = s->reset; nl; nl = nl->next) {
            print_logic_to_prs(fp, nl->net, RESET_NET, STORAGE);
            make_a_region(i, regions, nl->net, RESET_NET, STORAGE, 
                             ninpsig,comb);   
          }
          total_literal_number += set_literal;
          total_literal_number += reset_literal;
        }
    }
    fclose(fp);
    printf("Total area = %d literals\n", total_literal_number);
    fprintf(lg, "Total area = %d literals\n", total_literal_number);
}

int inbetween(cycleADT ****cycles,int ncycles,int i,int j,int l) 
{
  int c,d;

  for (c=0;c<ncycles;c++) 
    if (reachable(cycles,nevents,ncycles,i,0,j,c)!=notreachable) {
      for (d=0;d<=c;d++)
        if ((reachable(cycles,nevents,ncycles,i,0,l,d)!=notreachable) &&
            (reachable(cycles,nevents,ncycles,l,d,j,c)!=notreachable)) {

          if (ddebug) 
            fprintf(dbg, "%s,%d is in between  %s,0 and %s,%d)\n",
                    events[l]->event,d,events[i]->event,events[j]->event,c);

          return TRUE;
        }
      return FALSE;
    }
  if (!verbose) {
    printf("WARNING.\n");
    fprintf(lg,"WARNING.\n");
  }
  printf("WARNING: More cycles needed to determine reverse events.\n");
  fprintf(lg,"WARNING: More cycles needed to determine reverse events.\n");
  return FALSE;
}

int timed_inbetween(cycleADT ****cycles,int ncycles,int i,int j,int l)
{
  boundADT timediff1=NULL;
  boundADT timediff2;
  boundADT timediff3;
  int c,d;

  for (c=0;c<ncycles;c++) {
    timediff1 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,c);
    if (((reachable(cycles,nevents,ncycles,i,0,j,c)!=notreachable) &&
         (timediff1->L==0)) || (timediff1->L > 0)) {
      for (d=0;d<=c;d++) {
        timediff2 = WCTimeDiff(rules,cycles,nevents,ncycles,i,l,d);
        timediff3 = WCTimeDiff(rules,cycles,nevents,ncycles,l,j,c-d);
        if ((((reachable(cycles,nevents,ncycles,i,0,l,d)!=notreachable) &&
              (timediff2->L==0))||(timediff2->L > 0)) &&
            (((reachable(cycles,nevents,ncycles,l,d,j,c)!=notreachable) &&
              (timediff3->L==0))||(timediff3->L > 0))) {

          if (ddebug) 
            fprintf(dbg, "%s,%d is timed in between  %s,0 and %s,%d)\n",
                    events[l]->event,d,events[i]->event,events[j]->event,c);

          free(timediff1);
          free(timediff2);
          free(timediff3);
          return TRUE;
        }
        free(timediff2);
        free(timediff3);
      }
      free(timediff1);
      return FALSE;
    }
  }
  free(timediff1);
  if (!verbose) {
    printf("WARNING.\n");
    fprintf(lg,"WARNING.\n");
  }
  printf("WARNING: More cycles needed to determine reverse events.\n");
  fprintf(lg,"WARNING: More cycles needed to determine reverse events.\n");
  return FALSE;
}

void find_MG_timed_cause_events(cycleADT ****cycles, int ncycles, FILE *rel)
{
  int i,j,l;
  boundADT timediff1;
  boundADT timediff2;
  boundADT timediff3;
  boundADT timediff4;
  boundADT timediff5;
  boundADT timediff6;

  for (j = 1; j < (nevents - ndummy); j++) {
    if (events[j]->dropped) continue;
    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped) continue;
      l = h_events[i]->nr->index;
      if (i == j) continue;
      if (j == l) continue;
      timediff1 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,0);
      timediff2 = WCTimeDiff(rules,cycles,nevents,ncycles,i,l,0);
      timediff3 = WCTimeDiff(rules,cycles,nevents,ncycles,l,j,0);
      timediff4 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,1);
      timediff5 = WCTimeDiff(rules,cycles,nevents,ncycles,i,l,1);
      timediff6 = WCTimeDiff(rules,cycles,nevents,ncycles,l,j,1);
      if (((reachable(cycles,nevents,ncycles,i,0,j,0) != notreachable) &&
           (timediff1->L==0)) || (timediff1->L > 0)) {
        if (((reachable(cycles,nevents,ncycles,i,0,l,0) != notreachable) &&
             (timediff2->L==0)) || (timediff2->L > 0)) {
          if (((reachable(cycles,nevents,ncycles,l,0,j,0) == notreachable) &&
               (timediff3->L==0)) || (timediff3->L < 0)) {
            cause_table[i][j] = TRUE;

            if (ddebug) fprintf(dbg, "%s,0 timed causes %s,0 (reverse=%s)\n",
                   events[i]->event,events[j]->event,events[l]->event);

          }
        } else if (((reachable(cycles,nevents,ncycles,i,0,l,1) != notreachable)
                   && (timediff5->L==0)) || (timediff5->L > 0)) {
            cause_table[i][j] = TRUE;

            if (ddebug) fprintf(dbg, "%s,0 timed causes %s,0 (reverse=%s)\n",
                   events[i]->event,events[j]->event,events[l]->event);

         }
      } else if (((reachable(cycles,nevents,ncycles,i,0,j,1) != notreachable)&&
                  (timediff4->L==0)) || (timediff4->L > 0)) {
        if (((reachable(cycles,nevents,ncycles,i,0,l,0) != notreachable)&&
             (timediff2->L==0)) || (timediff2->L > 0)) {
          if (((reachable(cycles,nevents,ncycles,l,0,j,1) == notreachable)&&
               (timediff6->L==0)) || (timediff6->L < 0)) {
            cause_table[i][j] = TRUE;

            if (ddebug) fprintf(dbg, "%s,0 timed causes %s,0 (reverse=%s)\n",
                   events[i]->event,events[j]->event,events[l]->event);

          }
        } else 
          if ((((reachable(cycles,nevents,ncycles,i,0,l,1) != notreachable)&&
                (timediff5->L==0)) || (timediff5->L > 0)) && 
              (((reachable(cycles,nevents,ncycles,l,0,j,0) == notreachable)&&
                (timediff3->L==0)) || (timediff3->L < 0))) {
            cause_table[i][j] = TRUE;

            if (ddebug) fprintf(dbg, "%s,0 timed causes %s,0 (reverse=%s)\n",
                   events[i]->event,events[j]->event,events[l]->event);

          }
      }
      free(timediff1); 
      free(timediff2); 
      free(timediff3); 
      free(timediff4); 
      free(timediff5); 
      free(timediff6); 
    }
  }
  for (i = 1; i < (nevents - ndummy); i++)
    cause_table[h_events[i]->pr->index][i] = TRUE;

  if (verbose) print_cause_events(rel);
}

void find_MG_comp_timed_cause_events_removing(cycleADT ****cycles, int ncycles)
{
  int i,j,l;

  for (j = 1; j < nevents - ndummy; j++) { 
    if (events[j]->dropped) continue;
    for (i = 1; i < nevents - ndummy; i++) {
      if (events[i]->dropped || !cause(h_events[i], h_events[j])) continue;
      l = h_events[i]->nr->index;
      if (timed_inbetween(cycles, ncycles, i, j, l)) {
        cause_table[i][j] = FALSE;

        if (ddebug) fprintf(dbg, "Removing %s from %s cause_events\n", 
        	             h_events[i]->name, h_events[j]->name);

      }
    }
  }
}

void find_MG_comp_untimed_cause_events_adding(cycleADT ****cycles, int ncycles)
{
  int i,j,l;

  for (j = 1; j < nevents - ndummy; j++) { 
    if (events[j]->dropped) continue;
    for (i = 1; i < nevents - ndummy ; i++) {
      if (i == j || events[i]->dropped || 
          cause(h_events[i], h_events[j]) ||
          some_rev_event_cause(h_events[i], h_events[j])) continue;
      l = h_events[i]->nr->index;
      if (j == l) continue;
      if (inbetween(cycles, ncycles, i, j, l) == FALSE) {
        cause_table[i][j] = TRUE;


        if (ddebug) fprintf(dbg, "Adding %s to %s cause_events\n", 
                             h_events[i]->name, h_events[j]->name);

      }
    }
  }
}

void find_MG_comp_timed_cause_events_adding(cycleADT ****cycles, int ncycles)
{
  int i,j,l;
//  struct event_list *el, *rl;

  for (j = 1; j < (nevents - ndummy); j++) { 
    if (events[j]->dropped) continue;
    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped || cause(h_events[i], h_events[j])) continue;
      l = h_events[i]->nr->index;
      if (j == l) continue;
      if (timed_inbetween(cycles, ncycles, i, j, l) == FALSE) {
        cause_table[i][j] = TRUE;

        if (ddebug) fprintf(dbg, "Adding %s to %s cause_events\n", 
                             h_events[i]->name, h_events[j]->name);

      }
    }
  }
}

void find_FC_untimed_cause_events_dfs(hevent *src, hevent *curr, 
                                      int concurrent)
{
  int i;
  int con;

  curr->selected = TRUE;

  if (IS_REV_TYPE(src, curr)) {
    cause_table[src->index][curr->index] = TRUE;
    return;
  }
  else if (IS_SAME_TYPE(src, curr)) {
    printf("Internal Error(find_FC_untimed_cause_events) : same type event\n");

    if (ddebug) fprintf(dbg, "Internal Error(find_FC_untimed_cause_events) : same type event\n");
    if (ddebug) fprintf(dbg, "Internal Error: src = %s curr = %s\n", src->name, curr->name);

    return;
  }
  else {
    if (curr->index < nevents - ndummy) 
      cause_table[src->index][curr->index] = TRUE;
    if (!concurrent) con = con_with_some_rev_event(src, curr);
    else con = TRUE;

    for (i = 1; i < nevents; i++) {
      if (!events[i]->dropped &&
           non_redundant_rule(curr->index, i) &&
          !h_events[i]->selected &&
          (curr->index >= nevents - ndummy || 
           !con || con && con_with_some_rev_event(src, h_events[i])))
        find_FC_untimed_cause_events_dfs(src, h_events[i], con);
    }
  }
}

void find_FC_untimed_cause_events(FILE *rel)
{
  int i, j, k, l, m, n, count, multiple_causes;
  array_t *component;

  printf("Finding untimed cause events... ");
  fflush(stdout);
  for (i = 1; i < nevents - ndummy; i++) {
    if (events[i]->dropped) continue;
    for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
    for (j = 1; j < nevents; j++) 
      if (non_redundant_rule(i, j)) 
        find_FC_untimed_cause_events_dfs(h_events[i], h_events[j], FALSE);
  }

  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    set_nr_pr(i);

    multiple_causes = FALSE;
    for (j = 1; !multiple_causes && j < nevents; j++) {
      if (events[j]->dropped) continue;
      for (k = 0; !multiple_causes && k < nsignals; k++) {
        count = 0;
        for (l = h_signals[k]->first; l <= h_signals[k]->last; l++)
        if (!events[l]->dropped && cause_table[l][j]) count++;
        if (count > 1) multiple_causes = TRUE;
      }
    }
    if (!multiple_causes) mg_comps[i]->cause_table = cause_table;
    else {
      mg_comps[i]->cause_table = (int **) malloc((nevents-ndummy) * 
                                                     sizeof (int *));
      for (j = 0; j < nevents - ndummy; j++) 
        mg_comps[i]->cause_table[j] = (int *) malloc((nevents - ndummy) * 
                                                       sizeof (int));
      for (j = 0; j < nevents - ndummy; j++)
        for (k = 0; k < nevents - ndummy; k++) 
          mg_comps[i]->cause_table[j][k] = cause_table[j][k];

      cause_table = mg_comps[i]->cause_table;

      for (j = 1; j < nevents-ndummy; j++) {
        if (events[j]->dropped) continue;
        for (k = 0; k < nsignals; k++) {
          count = 0;
          for (l = h_signals[k]->first; l <= h_signals[k]->last; l++)
          if (!events[l]->dropped && cause_table[l][j]) count++;
          if (count > 1) {
            for (l = h_signals[k]->first; l <= h_signals[k]->last; l++)
              mg_comps[i]->cause_table[l][j] = FALSE;
            for (l = h_signals[k]->first; l <= h_signals[k]->last; l++) {
              if (events[l]->dropped) continue;
              for (m = 1; m < nevents; m++) h_events[m]->selected = FALSE;
              for (n = 1; n < nevents; n++) 
                if (!events[n]->dropped && non_redundant_rule(l, n)) 
                  find_FC_untimed_cause_events_dfs(h_events[l], h_events[n], FALSE);
            }
          }
        }
      }
    }
    restore_to_original_spec();
  }
  if (verbose) print_cause_events(rel);
  printf("done\n");
  fflush(stdout);
}
 
void find_next_reverse_event_dfs(hevent *src, hevent *curr, hevent **nre)
{
  int i;

  curr->selected = TRUE;

  if (IS_REV_TYPE(src, curr)) {
    *nre = curr;
    return;
  }
  else if (IS_SAME_TYPE(src, curr)) {
    printf("Internal Error(find_next_reverse_event) : same type event\n");

    if (ddebug) fprintf(dbg, "Internal Error(find_next_reverse_event) : same type event\n");
    if (ddebug) fprintf(dbg, "Internal Error: src = %s curr = %s\n", src->name, curr->name);

    return;
  }
  else if (con_with_some_event(src, curr)) {
    return;
  }
  else {
    for (i = 1; i < nevents; i++) {
      if (!events[i]->dropped &&
           non_redundant_rule(curr->index, i) &&
          !h_events[i]->selected)
        find_next_reverse_event_dfs(src, h_events[i], nre);
        if (*nre) return;
    }
  }
}

hevent * find_next_reverse_event(hevent *t)
{
  int j;
  hevent *nre=NULL;

  for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
  for (j = 1; j < nevents; j++) 
    if (!events[j]->dropped && non_redundant_rule(t->index, j)) 
      find_next_reverse_event_dfs(t, h_events[j], &nre);
  return nre;
}
 
void find_FC_untimed_reverse_events_dfs(hevent *src, hevent *curr)
{
  int i;

  curr->selected = TRUE;

  if (IS_REV_TYPE(src, curr)) {
    add_list_unique(&src->nr_list, curr);
    add_list_unique(&curr->pr_list, src);
    return;
  }
  else if (IS_SAME_TYPE(src, curr)) {
    printf("Internal Error(find_FC_untimed_reverse_events) : same type event\n");

    if (ddebug) fprintf(dbg, "Internal Error(find_FC_untimed_reverse_events) : same type event\n");
    if (ddebug) fprintf(dbg, "Internal Error: src = %s curr = %s\n", src->name, curr->name);

    return;
  }
  else if (con_with_some_event(src, curr)) {
    return;
  }
  else {
    for (i = 1; i < nevents; i++) {
      if (!events[i]->dropped &&
           non_redundant_rule(curr->index, i) &&
          !h_events[i]->selected)
        find_FC_untimed_reverse_events_dfs(src, h_events[i]);
    }
  }
}

void print_reverse_events(FILE *re)
{
  int i;
  event_list *el;

    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped) continue;
      fprintf(re,"%s's pr = ", h_events[i]->name);
      if (!marked_graph) {
        for (el = h_events[i]->pr_list; el; el = el->next) 
          fprintf(re, " %s ", el->event->name);
      }
      else fprintf(re, " %s ", h_events[i]->pr->name);
      fprintf(re, "\n");
      fprintf(re,"%s's nr = ", h_events[i]->name);
      if (!marked_graph) {
        for (el = h_events[i]->nr_list; el; el = el->next) 
          fprintf(re, " %s ", el->event->name);
      }
      else fprintf(re, " %s ", h_events[i]->nr->name);
      fprintf(re, "\n");
    }
}

void find_FC_untimed_reverse_events(FILE *rel)
{
  int count,c,i,j;
  array_t *component;
  event_list *nl;
  hevent *nre;

  printf("Finding reverse events... ");
  fflush(stdout);

  comp_nr = (int **) malloc(sizeof(int *) * array_n(mg_components));
  comp_pr = (int **) malloc(sizeof(int *) * array_n(mg_components));

  for (i = 1; i < nevents - ndummy; i++) {
    if (events[i]->dropped) continue;
    for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
    for (j = 1; j < nevents; j++) 
      if (non_redundant_rule(i, j)) 
        find_FC_untimed_reverse_events_dfs(h_events[i], h_events[j]);
  }
 
  for (c = 0; c < array_n(mg_components); c++) {
    component = array_fetch(array_t *, mg_components, c);
    set_to_a_component(component, c);
    comp_nr[c] = (int *) malloc(nevents * sizeof (int));
    comp_pr[c] = (int *) malloc(nevents * sizeof (int));
    for (i = 1; i < (nevents - ndummy); i++) {
      if (events[i]->dropped) continue;
      count = 0;
      for (nl = h_events[i]->nr_list; nl; nl = nl->next) {
        if (events[nl->event->index]->dropped) continue;
        count++;
      }
      if (count == 1) {
        for (nl = h_events[i]->nr_list; nl; nl = nl->next) {
          if (events[nl->event->index]->dropped) continue;
          comp_pr[c][nl->event->index] = i;
          comp_nr[c][i] = nl->event->index;
        }
      }
      else {

        if (ddebug) fprintf(dbg, "WARNING: find reverse events: count = %d\n", 
                              count);

        nre = find_next_reverse_event(h_events[i]);

        if (ddebug) fprintf(dbg, "WARNING : %s , %s->nr = %s\n", h_events[i]->name,  h_events[i]->name, nre->name);

        comp_nr[c][i] = nre->index;
        comp_pr[c][nre->index] = i;
      } 
    }
    restore_to_original_spec();
  }
  if (verbose) print_reverse_events(rel);
  printf("done\n");
  fflush(stdout);
}

void find_FC_timed_cause_events(cycleADT ****cycles, int ncycles, FILE *rel)
{
  int untimed_cause;
  int i,j,l;
  struct event_list *el;
  array_t *component;

  printf("Finding timed cause events... ");
  fflush(stdout);
  for (j = 1; j < (nevents - ndummy); j++) {
    if (events[j]->dropped) continue;
    for (i = 1; i < (nevents - ndummy); i++) {
      if (i == j || events[i]->dropped) continue;
      untimed_cause = TRUE;
      for (el = h_events[i]->nr_list; untimed_cause && el; el = el->next) {
        l = el->event->index;
        if (j == l) continue;
        if (inbetween(cycles,ncycles,i,j,l)) untimed_cause = FALSE;
      }
      if (untimed_cause) cause_table[i][j] = TRUE;
    }
  }

  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    set_nr_pr(i);
    find_MG_comp_timed_cause_events_removing(cycles, ncycles);
    restore_to_original_spec();
  }
  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    set_nr_pr(i);
    find_MG_comp_timed_cause_events_adding(cycles, ncycles);
    restore_to_original_spec();
  }

  for (i = 1; i < (nevents - ndummy); i++)
    for (el =  h_events[i]->pr_list; el; el = el->next)
      cause_table[el->event->index][i] = TRUE;
  if (verbose) print_cause_events(rel);
  printf("done\n");
  fflush(stdout);
}

void find_MG_reverse_events(cycleADT ****cycles, int ncycles, FILE *rel)
{

  int i,j,l;
  int prev_reverse;
  int in_between;
  stjung_signal *s;

  for (j = 1; j < nevents - ndummy; j++) {
    if (events[j]->dropped) continue;
    s = h_signals[events[j]->signal];
    for (i = s->first; i <= s->last; i++) {
      if (i == j) continue;
      if (events[i]->dropped) continue;
      if (h_events[i]->event_type == h_events[j]->event_type) continue;
      prev_reverse = TRUE;
      for (l = s->first; l <= s->last; l++) {
        if (events[l]->dropped) continue;
        if (i == j || j == l || i == l) continue;

        if (ddebug) fprintf(dbg, " i = %d j = %d l = %d\n", i, j, l);

        in_between=inbetween(cycles,ncycles,i,j,l);
        if (in_between) prev_reverse = FALSE;
      }
      if (prev_reverse) {
        h_events[j]->pr = h_events[i];
        h_events[i]->nr = h_events[j];
      }
    }
  }
  if (verbose) print_reverse_events(rel);
}

void find_FC_reverse_events(cycleADT ****cycles, int ncycles, FILE *rel)
{
  int c,i,j,l;
  int prev_reverse;
  int in_between;
  stjung_signal *s;
  array_t *component;

  printf("Finding reverse events... ");
  fflush(stdout);
  comp_nr = (int **) malloc(sizeof(int *) * array_n(mg_components));
  comp_pr = (int **) malloc(sizeof(int *) * array_n(mg_components));

  for (c = 0; c < array_n(mg_components); c++) {
    component = array_fetch(array_t *, mg_components, c);
    set_to_a_component(component, c);
    comp_nr[c] = (int *) malloc(nevents * sizeof (int));
    comp_pr[c] = (int *) malloc(nevents * sizeof (int));
    for (j = 1; j < (nevents - ndummy); j++) {
      if (events[j]->dropped) continue;
      s = h_signals[events[j]->signal];
      for (i = s->first; i <= s->last; i++) {
        if (i == j || events[i]->dropped) continue;
        if (h_events[i]->event_type == h_events[j]->event_type) continue;
        prev_reverse = TRUE;
        for (l = s->first; prev_reverse && l <= s->last; l++) {
          if (events[l]->dropped) continue;
          if (i == j || j == l || i == l) continue;

          if (ddebug) fprintf(dbg, " i = %d j = %d l = %d\n", i, j, l);

          in_between=inbetween(cycles,ncycles,i,j,l);
          if (in_between) prev_reverse = FALSE;
        }
        if (prev_reverse) {
          add_list_unique(&(h_events[j]->pr_list), h_events[i]);
          add_list_unique(&(h_events[i]->nr_list), h_events[j]);
          comp_pr[c][j] = i;
          comp_nr[c][i] = j;
        }
      }
    }
    restore_to_original_spec();
  }


  if (ddebug) {
    for (c = 0; c < array_n(mg_components); c++) {
      component = array_fetch(array_t *, mg_components, c);
      set_to_a_component(component, c);
      for (j = 1; j < (nevents - ndummy); j++) {
        if (events[j]->dropped) continue;
        fprintf(dbg, "%s : nr = %s pr = %s\n", h_events[j]->name, 
                     h_events[comp_nr[c][j]]->name, 
                     h_events[comp_pr[c][j]]->name);
      }
      restore_to_original_spec();
    }
  }


  for (j = 1; j < nevents - ndummy; j++) {
    if (events[j]->dropped) continue;
    s = h_signals[events[j]->signal];
    for (i = s->first; i <= s->last; i++) {
      if (i == j || events[i]->dropped ||
          h_events[i]->event_type == h_events[j]->event_type ||
          is_in_list(h_events[j], h_events[i]->nr_list)) continue;
      prev_reverse = TRUE;
      for (l = s->first; prev_reverse && l <= s->last; l++) {
        if (events[l]->dropped || i == j || j == l || i == l) continue;

        if (ddebug) fprintf(dbg, " i = %d j = %d l = %d\n", i, j, l);

        in_between=inbetween(cycles,ncycles,i,j,l);
        if (in_between) prev_reverse = FALSE;
      }
      if (prev_reverse) {
        add_list_unique(&(h_events[j]->pr_list), h_events[i]);
        add_list_unique(&(h_events[i]->nr_list), h_events[j]);

        if (ddebug) fprintf(dbg, "### %s is added as a next reverse events of %s\n", 
               h_events[j]->name, h_events[i]->name);

      }
    }
  }
  if (verbose) print_concurrency_table(rel);
  printf("done\n");
  fflush(stdout);
}

int is_same_trig_sigs(int j, int k)
{
  event_list *t1, *t2, *el;
  int result = TRUE;
 
  t1 = get_trig_events(h_events[j]);
  t2 = get_trig_events(h_events[k]);

  for (el = t1; el; el = el->next) {
    if (EC_approximated[j][el->event->signal->index] == 'X' ||
          EC_approximated[k][el->event->signal->index] == 'X') continue;
    if (EC_approximated[j][el->event->signal->index] != 
          EC_approximated[k][el->event->signal->index] )
      result = FALSE;
  }

  for (el = t2; el; el = el->next) {
    if (EC_approximated[j][el->event->signal->index] == 'X' ||
          EC_approximated[k][el->event->signal->index] == 'X') continue;
    if (EC_approximated[j][el->event->signal->index] != 
          EC_approximated[k][el->event->signal->index] )
      result = FALSE;
  }

  free_list(t1);
  free_list(t2);
  return result;
}

int may_enabling_cube_intersect(int j, int k)
{
  int i;

  for (i = 0; i < nsignals; i++)
    if (EC_approximated[j][i] != 'X' && EC_approximated[k][i] != 'X' &&
        EC_approximated[j][i] != EC_approximated[k][i]) return FALSE;
  return TRUE;
}

int num_of_x(char *cube)
{
  int j, count = 0;

  for (j = 0; j < nsignals; j++) if (cube[j] == 'X') count++;
  return count; 
}

void print_cube_list(char *msg, cube_list *list)
{
  cube_list *cl;
 
  fprintf(dbg, "%s", msg); 
  for (cl = list; cl; cl = cl->next)
    fprintf(dbg, "%s ", cl->cube);
  fprintf(dbg, "\n");
}

void find_EC_exact(int i)
{
  char *cube1, *cube2;
  cube_list *cl, *dl,*concurrent_cl=NULL, *newcl, *free_cl, *to_refine_cl=NULL ;
  cube_list *refined_cl=NULL;
  char *cube;
  int j, k, l,refined, already_refined, no_more_refinement, concurrent_with_all;

  cube = (char *) malloc(nsignals+1);
  cube[nsignals] = '\0';
  newcl = ALLOC(struct cube_list, 1);
  newcl->next = NULL;
  newcl->cube = (char *) malloc(nsignals+1);
  strcpy(newcl->cube, EC_approximated[i]);
  if (num_of_x(newcl->cube) < 2) {
    EC_exact[i] = newcl;
    return;
  }
  to_refine_cl = newcl;
   
  for (l = 1; l < nevents - ndummy; l++) {
    if (!concurrency_table[i][l]) continue;
    concurrent_with_all = TRUE;
    for (k = 1; k < nevents - ndummy; k++) {
      if (l == k || !concurrency_table[i][k]) continue;
      if (!concurrency_table[l][k]) concurrent_with_all = FALSE;
    }
  
    if (concurrent_with_all) continue;

    newcl = ALLOC(struct cube_list, 1);
    newcl->cube = (char *) malloc(nsignals+1);
    strcpy(newcl->cube, EC_approximated[l]);
    newcl->next = concurrent_cl;
    concurrent_cl = newcl;

    for (j = 1; j < (nevents - ndummy); j++) {
      if (ruletype[l][j] == (TRIGGER | SPECIFIED) ||
                 si_mode && ruletype[l][j] == 
                 (TRIGGER | SPECIFIED | REDUNDANT)) {
        if (is_concur(h_events[i], h_events[j]) == FALSE) {
          newcl = ALLOC(struct cube_list, 1);
          newcl->cube = (char *) malloc(nsignals+1);
          strcpy(newcl->cube, EC_approximated[l]);
          if (event_type(h_events[l]) == RISING) 
            newcl->cube[h_events[l]->signal->index] = '1';
          else 
            newcl->cube[h_events[l]->signal->index] = '0';
          newcl->next = concurrent_cl;
          concurrent_cl = newcl;
        }
      }
    }
  }


  if (ddebug) print_cube_list("Cube to be refined : ", to_refine_cl);
  if (ddebug) print_cube_list("Cube intersected   : ", concurrent_cl);


  if (concurrent_cl == NULL) {
    EC_exact[i] = to_refine_cl;
    return;
  }

  no_more_refinement = FALSE;
  while (no_more_refinement == FALSE) {
    refined_cl = NULL;
    for (cl = to_refine_cl; cl; cl = cl->next) {
      cube1 = cl->cube;
      refined = FALSE;
      if (num_of_x(cube1) < 2) printf("Internal Error : find_EC_exact\n");
      for (dl = concurrent_cl; dl; dl = dl->next) {
        cube2 = dl->cube;

        if (ddebug) fprintf(dbg, "cube1 = %s cube2 = %s\n", cube1, cube2);

        already_refined = FALSE;
        for (j = 0; j < nsignals; j++) {
          if (cube1[j] == cube2[j]) cube[j] = cube1[j]; 
          else if (cube1[j] == 'X' && cube2[j] != 'X') 
            cube[j] = cube2[j];
          else if (cube1[j] != 'X' && cube2[j] == 'X') 
            cube[j] = cube1[j];
          else {
            already_refined = TRUE;
          }
        }
        if (!already_refined && num_of_x(cube) < num_of_x(cube1)) {
          refined = TRUE;
          newcl = ALLOC(struct cube_list, 1);
          newcl->cube = (char *) malloc(nsignals+1);
          strcpy(newcl->cube, cube);
          if (num_of_x(cube) < 2) {
            newcl->next = EC_exact[i];
            EC_exact[i] = newcl;
          }
          else {
            newcl->next = refined_cl;
            refined_cl = newcl;
          }
        }
      }
      if (refined == FALSE) {
        newcl = ALLOC(struct cube_list, 1);
        newcl->cube = (char *) malloc(nsignals+1);
        strcpy(newcl->cube, cube1);
        newcl->next = EC_exact[i];
        EC_exact[i] = newcl;
      }
    }

    if (refined_cl == NULL) no_more_refinement = TRUE;
    else {
      free_cl = to_refine_cl;
      to_refine_cl = refined_cl;
      free_cube_list(free_cl);
    }
  }
  free(cube);
}

int is_enabling_cube_intersect(int j, int k)
{
  cube_list *cl, *dl;
  int i, intersect;


  if (ddebug) fprintf(dbg,"exact EC for %s and %s\n", h_events[j]->name, h_events[k]->name);


  if (EC_exact[j] == NULL) find_EC_exact(j);
  if (EC_exact[k] == NULL) find_EC_exact(k);


  if (ddebug) print_cube_list("Cube list : ", EC_exact[j]);
  if (ddebug) print_cube_list("Cube list : ", EC_exact[k]);

 
  for (cl = EC_exact[j]; cl; cl = cl->next)
    for (dl = EC_exact[k]; dl; dl = dl->next) {
      intersect = TRUE;
      for (i = 0; i < nsignals; i++)
        if (cl->cube[i] != 'X' && dl->cube[i] != 'X' &&
          cl->cube[i] != dl->cube[i]) intersect = FALSE;
      if (intersect) return TRUE;
    }
  return FALSE;
}

int  cmp_vertex (const void *obj1, const void *obj2)
{
    int e1 = *((int *) obj1);
    int e2 = *((int *) obj2);

    return (h_events[e2]->weight - h_events[e1]->weight);
}

int cmp_component (array_t *comp1, array_t *comp2)
{
    /* These are already in canonical form. */
    /* Only need equal/not equal test, no ordering. */
    int n = array_n(comp1);

    if (n != array_n(comp2)) return -1;

    while (n--)
        if (array_fetch(int,comp1,n) != array_fetch(int,comp2,n)) return 1;

    /* These are equal. */
    return 0;
}

int add_component(array_t *cc, int n, void * fdata)
{
  array_t *component_array = (array_t *) fdata;
  array_t *component;
  int unique = TRUE;
  int i;

  component = array_dup(cc);
  array_sort(component, cmp_vertex);
  for (i = array_n(component_array); i--; ) {
    if (!cmp_component(component, array_fetch(array_t *, component_array, i))) {
      array_free(component);
      unique = FALSE;
      break;
    }
  }
  if (unique) array_insert_last(array_t *, component_array, component);
  return TRUE;
}

void connected_components_dfs(int i, array_t *vcomp, int component_n)
{
  int j;

  if (h_events[i]->unprocessed) {
    h_events[i]->unprocessed = FALSE;
    h_events[i]->comp_num  = component_n;
    array_insert_last (int, vcomp, i);
    for (j = 1; j < nevents; j++) 
      if (non_redundant_rule(i,j)) 
        connected_components_dfs(j, vcomp,component_n);
  }
}

int find_connected_comp(int (*f)(array_t *cc, int n, void * fdata), 
                        void *f_data)
{
  int i, component_n=0;
  array_t *vcomp;

  for (i = 1; i < nevents; i++) {
    if (h_events[i]->unprocessed && !events[i]->dropped) {
      vcomp = array_alloc(int, 0);
      connected_components_dfs(i, vcomp, component_n);

      if (f != NULL) (*f)(vcomp,component_n,f_data);
      component_n++;
      array_free (vcomp);
    }
  }
  return component_n;
}


void del_mg_event(int i, int depth)
{
  int unremoved_fanin, j, k;

  if (h_events[i]->removed) return;
  depth++;
  h_events[i]->removed = TRUE;


  if (ddebug) fprintf(dbg, "%s is removed\n", h_events[i]->name);

  
  for (j = 1; j < nevents; j++)
    if (non_redundant_rule(i, j)) {
      if (h_events[j]->removed) continue;
      unremoved_fanin = 0;
      for (k = 1; k < nevents; k++)
        if (non_redundant_rule(k, j) && rules[i][k]->conflict && 
            h_events[k]->removed == FALSE)
          unremoved_fanin++;

        if (ddebug) fprintf(dbg, "The unremoved fanin for %s = %d\n", h_events[j]->name, unremoved_fanin);

      if (unremoved_fanin == 0) del_mg_event(j, depth);
    }
}
 
int is_in_choice_events_sets(int j, array_t *choice_events_sets)
{
  int i, k, n;
  array_t * cevents1;

  for (n = 0; n < array_n(choice_events_sets); n++) {
    cevents1 = array_fetch(array_t *, choice_events_sets, n);
    for (i = 0; i < array_n(cevents1); i++) {
      k = array_fetch(int, cevents1, i);
      if (k == j) return TRUE;
    }
  }
  return FALSE;
}
    
void do_mg_reduce(array_t * components, array_t *choice_events_sets, 
                  int choices_are_ordered)
{
  int i, j, n; 
  int static count = -1;

  count++;

  if ((count % 1000) == 0) if(ddebug) fprintf(dbg, "Processing %d component\n", count);


  if (choices_are_ordered) 
    for (i = 1; i < nevents; i++) 
      if (h_events[i]->not_in_allocation == FALSE && 
             h_events[i]->processed == TRUE) {
//        printf("This component need not to be processed\n"); 
//        printf("%s is processed already\n", h_events[i]->name);
        return;
      }
  
  // printf("Processing %d component\n", count);
  for (i = 1; i < nevents; i++) 
    if (h_events[i]->not_in_allocation == FALSE && 
          is_in_choice_events_sets(i, choice_events_sets)) {
      //printf("%s->processed = TRUE\n", h_events[i]->name);
      h_events[i]->processed = TRUE;
    }

  for (i = 1; i < nevents; i++) h_events[i]->removed = FALSE;


  for (i = 1; i < nevents; i++) 
    if (h_events[i]->not_in_allocation) {
      if (ddebug) fprintf(dbg, "%s is not allocated (removed)\n", h_events[i]->name);
    }


  for (i = 1; i < nevents; i++) {
    if (h_events[i]->removed) continue;
    for (j = 1; j < nevents; j++) {
      if (!non_redundant_rule(i,j)) continue;
      if (h_events[j]->removed) continue;
      if (h_events[j]->not_in_allocation) del_mg_event(j, 0);
    }
  }

  for (i = 1; i < nevents; i++) 
    h_events[i]->unprocessed = !h_events[i]->removed;


  if (ddebug) fprintf(dbg, "\n");
  for (i = 1; i < nevents; i++) {
    if (h_events[i]->removed) if (ddebug) fprintf(dbg,"%s ", h_events[i]->name);
  }
  if (ddebug) fprintf(dbg,"are removed\n");


  i = array_n(components);
  find_connected_comp(add_component, (void *) components);
  j = array_n(components);


  if (ddebug) fprintf(dbg,"number of new components = %d\n", j-i);
  for (n = i; n < j; n++)
    if (ddebug) print_component (n-i,array_fetch (array_t *,components,n),dbg);

}

void gen_mg_allocation(array_t *components, array_t *choice_events_sets, int n,
			int choices_are_ordered)
{
  int i, j;
  array_t * choice_events;

  if (n-- == 0) {
    do_mg_reduce(components, choice_events_sets, choices_are_ordered);
  }
  else {
    choice_events = array_fetch(array_t *, choice_events_sets, n);
    for (j = 0; j < array_n(choice_events); j++) {
      i = array_fetch(int, choice_events, j);
      h_events[i]->not_in_allocation = FALSE;

      if (ddebug) fprintf(dbg, "%s->not_in_alloc = FALSE\n", h_events[j]->name);

      gen_mg_allocation(components, choice_events_sets, n, choices_are_ordered);
      h_events[i]->not_in_allocation = TRUE;
    }
  }
}

array_t *get_merge_events_sets()
{
  array_t *merge_events_sets;
  array_t *merge_events;
  int i, j, k, n, degree;

  merge_events_sets = array_alloc(array_t *, 0);

  for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
  for (i = 1; i < nevents; i++) {
    for (j = 1; j < nevents; j++) {
      if (h_events[j]->selected || non_redundant_rule(j,i) == FALSE) continue;
      degree = 0;
      for (k = 1; k < nevents; k++) {
        if (j == k || non_redundant_rule(k,i) == FALSE) continue;
        if (rules[j][k]->conflict) degree++;
      } 
      if (degree > 0) {
        merge_events = array_alloc(int, 0);
        array_insert_last(int, merge_events, j);
        h_events[j]->selected = TRUE;
        
        for (k = 1; k < nevents; k++) {
          if (j == k || non_redundant_rule(k,i) == FALSE) continue;
          if (rules[j][k]->conflict) {
            h_events[k]->selected = TRUE;
            array_insert_last (int, merge_events, k);
          }
        } 
        array_insert_last(array_t *, merge_events_sets, merge_events); 

        if (ddebug) {
          fprintf(dbg, "\n\nA merge events set : ");
          for (n=0; n < array_n(merge_events); n++) {
              j = array_fetch (int, merge_events, n);
              fprintf(dbg," %s",h_events[j]->name);
          }
          fputs("\n",dbg);
        }

      }
    }
  }

  if (ddebug) fprintf(dbg, "\nNumber of merge events set = %d\n", 
                       array_n(merge_events_sets));

  return merge_events_sets;
}

array_t *get_choice_events_sets(int *ubound)
{
  array_t *choice_events_sets;
  array_t *choice_events;
  int i, j, k, n, degree;

  choice_events_sets = array_alloc(array_t *, 0);
  *ubound = 1;

  for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
  for (i = 1; i < nevents; i++) {
    for (j = 1; j < nevents; j++) {
      if (h_events[j]->selected || non_redundant_rule(i,j) == FALSE) continue;
      degree = 0;
      for (k = 1; k < nevents; k++) {
        if (j == k || non_redundant_rule(i,k) == FALSE) continue;
        if (rules[j][k]->conflict) degree++;
      } 
      if (degree > 0) {
        choice_events = array_alloc(int, 0);
        array_insert_last (int, choice_events, j);
        h_events[j]->selected = TRUE;
        h_events[j]->not_in_allocation = TRUE;
        
        for (k = 1; k < nevents; k++) {
          if (j == k || non_redundant_rule(i,k) == FALSE) continue;
          if (rules[j][k]->conflict) {
            h_events[k]->selected = TRUE;
            h_events[k]->not_in_allocation = TRUE;
            array_insert_last (int, choice_events, k);
          }
        } 
        *ubound = (*ubound) * (degree+1);
        array_insert_last(array_t *, choice_events_sets, choice_events); 

        if (ddebug) {
          fprintf(dbg, "\n\nA choice events set : ");
          for (n=0; n < array_n(choice_events); n++) {
              j = array_fetch (int, choice_events, n);
              fprintf(dbg," %s",h_events[j]->name);
          }
          fputs("\n",dbg);
        }

      }
    }
  }

    if (ddebug) fprintf(dbg, "\nNumber of key events set = %d\n", array_n(choice_events_sets));

  return choice_events_sets;
}

int all_free_choice_has_no_concurrent_events()
{
  int i, j;

  for (i = 1; i < nevents; i++) {
    if (has_no_concurrent_event(i)) continue;
    for (j = 1; j < nevents; j++)
      if (rules[i][j]->conflict) return FALSE;
  }
  return TRUE;
}

void find_a_minimal_cover(array_t *component_array)
{
  int i, j, k, e, ncomps, unique;
  array_t *component;
  tableADT row, newtab,old=NULL;
  tableSolADT Sol,NewSol;
  int *cost;
  int U;

  selected_comp = (int *) malloc(sizeof(int) * array_n(component_array));
  for (i = 0; i < array_n(component_array); i++) selected_comp[i] = FALSE;
  
  /* Determine table size */ 
  ncomps = array_n(component_array); 
  /* nsolutions = ncomps;*/


if (ddebug) fprintf(dbg,"\nNumber of components = %d\n", ncomps);

  Sol=(tableSolADT)GetBlock(sizeof(*(Sol)));
  Sol->currentSol=(char *)GetBlock((ncomps+1) * sizeof(char));
  Sol->size=ncomps;
  for (i=0;i<Sol->size;i++)
    Sol->currentSol[i]='0';
  Sol->currentSol[Sol->size]='\0';

  /* Determine cost function (number of literals) */
  cost = (int *)GetBlock(ncomps*sizeof(int));
  for (i = 0; i < ncomps; i++) cost[i] = 1;

  /* Construct table */
  for (e = 1; e < nevents; e++) {
    if (events[e]->dropped) continue;

if (ddebug) fprintf(dbg,"\n%s is unprocessed\n", h_events[e]->name);

 newtab=(tableADT)GetBlock(sizeof(*(newtab)));
 newtab->row=(char *)GetBlock((ncomps+1)*sizeof(char));
 newtab->size=ncomps;
   for (i = 0; i < ncomps; i++) newtab->row[i] = '0';
    for (i = 0; i < array_n(component_array); i++) {
      component = array_fetch(array_t *, component_array, i);
      for (j = 0; j < array_n(component); j++) {
        k = array_fetch(int, component, j);
        if (e == k) newtab->row[i] = '1';
      }
    }
    newtab->row[ncomps]='\0';
    unique = TRUE;
    for (row = old; row; row = row->link)
      if (strcmp(newtab->row, row->row) == 0) unique = FALSE;
    if (unique) {   
      newtab->link=old;
      old=newtab;
    }
    else free(newtab);
  }

  Sol->F=old;

  if (ddebug) print_current_solution(dbg,Sol);


  U=0;
  for (j=0;j<ncomps;j++) U = U + cost[j];
  U++;

  NewSol=BCP(Sol,&U,cost,0);
  if (!NewSol) {
    printf("ERROR: Could not cover all events with MG components.\n");
    fprintf(lg,"ERROR: Could not cover all events with MG components.\n");
  }
  else {

    if (ddebug) print_current_solution(dbg,NewSol);

  }

  // delete_tableSol(Sol);

  for (i = 0; i < ncomps; i++) 
    if (NewSol->currentSol[i] == '1') selected_comp[i] = TRUE; 
}

cn_graph * alloc_cn_graph()
{
  cn_graph *g;

  g = ALLOC(cn_graph, 1);

  g->vertexes = NULL;
  g->edges = NULL;
  g->cycles = NULL;
  
  return g;
}
void free_cn_cycle(cn_cycle *c)
{
  cn_vertex_list *vl, *free_vl;
  cn_edge_list *el, *free_el;
 
  for (vl = c->vertexes; vl; ) {
    free_vl = vl;
    vl = vl->next;
    free(free_vl);
  } 
  for (el = c->edges; el; ) {
    free_el = el;
    el = el->next;
    free(free_el);
  }
}

void free_cn_graph(cn_graph *g)
{
  cn_edge *e, *free_e;
  cn_vertex *v, *free_v;
  cn_cycle *c, *free_c;

  for (v = g->vertexes; v; ) {
    free_v = v;
    v = v->next;
    free(free_v);
  }
  for (e = g->edges; e; ) {
    free_e = e;
    e = e->next;
    free(free_e);
  }
  for (c = g->cycles; c; ) {
    free_c = c;
    c = c->next;
    free_cn_cycle(free_c);
  }
}

cn_vertex *alloc_cn_vertex(cn_graph *g)
{
  cn_vertex *v;
  static int id = 0;

  v = ALLOC(cn_vertex, 1);
  
  v->in_edges = NULL;
  v->out_edges = NULL;
  v->next = g->vertexes;
  v->id = id++;
  g->vertexes = v;


  if (ddebug) fprintf(dbg, "A new cn_vertex is alloacted. id = %d\n", v->id);


  return v;
}

cn_edge *alloc_cn_edge(cn_graph *g)
{
  static int id = 0;
  cn_edge *e;

  e = ALLOC(cn_edge, 1);
  e->id = id++;
  e->type = 0;
  e->head = NULL;
  e->tail = NULL;
  e->next_in = NULL;
  e->next_out = NULL;
  e->next = g->edges;
  g->edges = e;
 
  return e;
}

void add_cn_edge(cn_graph *g, cn_vertex *v1, cn_vertex *v2, hevent *ev,int type)
{
  cn_edge *e;

  e = alloc_cn_edge(g);
  e->type = type;
  e->tail = v1;
  e->head = v2;
  e->next_in = v2->in_edges;
  v2->in_edges = e;
  e->next_out = v1->out_edges;
  v1->out_edges = e;
  e->event = ev;


  if (ddebug) fprintf(dbg, "An edge is added: v1 = %d v2 = %d event = %s\n", v1->id, v2->id, ev->name);

}

void create_cn_vertexes(cn_graph *g, array_t *choice_events_sets, 
                        array_t *merge_events_sets)
{
  int i, j, n, npredecessors;
  cn_vertex *v;
  array_t *choice_events, *merge_events;

  for (n = 0; n < array_n(choice_events_sets); n++) {
    choice_events = array_fetch(array_t *, choice_events_sets, n);
    v = alloc_cn_vertex(g);
    for (j = 0; j < array_n(choice_events); j++) {
      i = array_fetch(int, choice_events, j);
      h_events[i]->cn_choice_vertex = v;
    }
    i = array_fetch(int, choice_events, 0);
    npredecessors = 0;
    for (j = 1; j < nevents; j++) 
      if (non_redundant_rule(j, i)) npredecessors++;
    if (npredecessors > 1) {
      for (j = 1; j < nevents; j++) 
        if (non_redundant_rule(j, i)) h_events[j]->cn_merge_vertex = v;
      v->type = CHOICE_AND_MERGE;
    }
    else v->type = CHOICE;
  }

  for (n = 0; n < array_n(merge_events_sets); n++) {
    merge_events = array_fetch(array_t *, merge_events_sets, n);
    i = array_fetch(int, merge_events, 0);
    if (h_events[i]->cn_merge_vertex) continue;
    v = alloc_cn_vertex(g);
    for (j = 0; j < array_n(merge_events); j++) {
      i = array_fetch(int, merge_events, j);
      h_events[i]->cn_merge_vertex = v;
    }
    v->type = MERGE;
  }
}

int find_destination_dfs(hevent *curr, cn_vertex **result_dest)
{
  int i;
  cn_vertex *prev_dest, *dest;


  if (ddebug) fprintf(dbg, "find_dest_dfs: curr = %s\n", curr->name);

  if (curr->processed) {
    *result_dest = curr->dest;
    return TRUE;
  }
  if (curr->selected) {

    if (ddebug) fprintf(dbg, "find_dest_dfs : %s is visited already\n", curr->name);

    *result_dest = NULL;
    return TRUE;
  }
  if (curr->cn_choice_vertex) {

    if (ddebug) fprintf(dbg, "find_dest_dfs: dest = %d\n", curr->cn_choice_vertex->id);

    *result_dest = curr->cn_choice_vertex;
    return TRUE;
  }
  else if (curr->cn_merge_vertex) {

    if (ddebug) fprintf(dbg, "find_dest_dfs: dest = %d\n", curr->cn_merge_vertex->id);

    *result_dest = curr->cn_merge_vertex;
    return TRUE;
  }
  else {
    curr->selected = TRUE;
    prev_dest = NULL;
    for (i = 1; i < nevents; i++) {
      if (!events[i]->dropped && non_redundant_rule(curr->index, i)) {
        if (!find_destination_dfs(h_events[i], &dest)) return FALSE;
        if (dest) {
          if (prev_dest && prev_dest != dest) {

            if (ddebug) {
              if (!dest) fprintf(dbg, "dest is NULL\n");
              if (prev_dest && prev_dest != dest) fprintf(dbg, "dest is not same : %d %d\n", prev_dest->id, dest->id);
            }

            return FALSE; 
          }
          prev_dest = dest; 
        }
      }
    }
  }

  if (prev_dest) *result_dest = prev_dest;
  else return FALSE;

  curr->dest = *result_dest;
  curr->processed = TRUE;
  curr->selected = FALSE;
  

  if (ddebug) fprintf(dbg, "dest from %s is %d\n", curr->name, (*result_dest)->id);

  return TRUE;
}

cn_vertex *find_destination(hevent *src)
{
  int i;
  cn_vertex *dest, *prev_dest;

  if (src->cn_choice_vertex && src->cn_merge_vertex)
    return src->cn_merge_vertex;

  for (i = 1; i < nevents; i++) {
    h_events[i]->selected = FALSE;
    h_events[i]->processed = FALSE;
    h_events[i]->dest = NULL;
  }
    
  prev_dest = NULL; 
  for (i = 1; i < nevents; i++) {
    if (!events[i]->dropped && non_redundant_rule(src->index, i)) {
      if (!find_destination_dfs(h_events[i], &dest)) return FALSE;
      if (dest) {
        if (prev_dest && prev_dest != dest) {

          if (ddebug) {
            if (!dest) fprintf(dbg, "dest is NULL\n");
            if (prev_dest && prev_dest != dest) fprintf(dbg, "dest is not same : %d %d\n", prev_dest->id, dest->id);
          }

          return NULL;
        }
        prev_dest = dest;
      }
    }
  }

  if (ddebug && prev_dest) fprintf(dbg, "dest from %s is %d\n", src->name, prev_dest->id);

  if (prev_dest) return prev_dest;
  else return NULL;
}

void find_cn_minimal_covering_cycles(cn_graph *g)
{
  cn_edge *e;
  cn_cycle *c;
  int ncycles = 0;
  int i, j, unique;
  cn_edge_list *el;
  tableADT row, newtab,old=NULL;
  tableSolADT Sol,NewSol;
  int *cost;
  int U;

  /* Determine table size */ 
  for (c = g->cycles; c; c = c->next) ncycles++;
  /*nsolutions = ncycles;*/

  Sol=(tableSolADT)GetBlock(sizeof(*(Sol)));
  Sol->currentSol=(char *)GetBlock((ncycles+1) * sizeof(char));
  Sol->size=ncycles;
  for (i=0;i<Sol->size;i++)
    Sol->currentSol[i]='0';
  Sol->currentSol[Sol->size]='\0';

  /* Determine cost function (number of literals) */
  cost = (int *)malloc(ncycles*sizeof(int));
  for (i = 0; i < ncycles; i++) cost[i] = 1;

  /* Construct table */
  for (e = g->edges; e; e = e->next) {

    if (ddebug) fprintf(dbg, "edge:  id = %d in = %d out = %d\n", e->id,e->tail->id, e->head->id);

 newtab=(tableADT)GetBlock(sizeof(*(newtab)));
 newtab->row=(char *)GetBlock((ncycles+1)*sizeof(char));
 newtab->size=ncycles;
    for (i = 0; i < ncycles; i++) newtab->row[i] = '0';
    for (c = g->cycles, j = 0; c; c = c->next, j++) {
      for (el = c->edges; el; el = el->next) 
        if (el->edge == e) newtab->row[j] = '1';
    }
    newtab->row[ncycles]='\0';
    unique = TRUE;
    for (row = old; row; row = row->link)
      if (strcmp(newtab->row, row->row) == 0) unique = FALSE;
    if (unique) {   
      newtab->link=old;
      old=newtab;
    }
    else free(newtab);
  }

  Sol->F=old;

  if (ddebug) print_current_solution(dbg,Sol);


  U=0;
  for (j=0;j<ncycles;j++) U = U + cost[j];
  U++;

  NewSol=BCP(Sol,&U,cost,0);
  if (!NewSol) {
    printf("ERROR: Could not cover all edges with simple cycles.\n");
  }
  else {

    if (ddebug) print_current_solution(dbg,NewSol);

  }

  for (c = g->cycles, j = 0; c; c = c->next, j++)
    if (NewSol->currentSol[j] == '1') c->essential = TRUE;
    else c->essential = FALSE;
}

int cn_add_cycle(cn_graph *g)
{
  cn_vertex *v, *head, *tail;
  cn_cycle *cycle;
  cn_vertex_list *vl;
  cn_vertex *last=NULL;
  cn_edge *e;
  cn_edge_list *new_el;
  int first = TRUE;

  // printf("cn_add_cycle is called\n");

  if (ddebug) fprintf(dbg, "cn_add_cycle is called\n");

  cycle = ALLOC(cn_cycle, 1);
  if (g->cycles == NULL) cycle->id = 0;
  else cycle->id = g->cycles->id + 1;
  cycle->next = g->cycles;
  g->cycles = cycle;
  cycle->vertexes = NULL;
  cycle->edges = NULL;
  
  tail = g->path_start; 
  head = NULL;
  for (v = g->path_start; first || v != g->path_start; v = v->trail) {
    vl = ALLOC(cn_vertex_list, 1);
    vl->vertex = v;
    vl->next = cycle->vertexes;
//printf("vertex : %d is added\n", v->id);
    cycle->vertexes = vl;
    if (first) first = FALSE;
    else {
      if (head) tail = head;
      head = v;
      for (e = g->edges; e; e = e->next) {
        if (e->active && e->head == head && e->tail == tail) {
          new_el = ALLOC(cn_edge_list, 1);
          new_el->edge = e;
          new_el->next = cycle->edges;
          cycle->edges = new_el;
//printf("edge: %d is added\n", e->id);
          break;
        }
      }
    }
    last = v;
  }

    for (e = g->edges; e; e = e->next) {
      if (e->active && e->head == g->path_start && e->tail == last) {
        new_el = ALLOC(cn_edge_list, 1);
        new_el->edge = e;
        new_el->next = cycle->edges;
        cycle->edges = new_el;
//printf("edge: %d is added\n", e->id);
        break;
      }
    }

  return 1;
}  

int cn_cycles(cn_graph  *g, cn_vertex *v)
{
    int rc = 0;
    cn_edge *e;

   // printf("cn_cycles: vertex ->id = %d\n", v->id);

    if (ddebug) fprintf(dbg, "cn_cycles: vertex ->id = %d\n", v->id);


    if (v->active) {
        if (v->unprocessed) {
            g->path_start = v;
            rc = cn_add_cycle(g);
        }
    }
    else {
        v->active =  TRUE;
        for (e = v->out_edges;  e; e = e->next_out) {
            v->trail = e->head; 
            e->active = TRUE;
            rc += cn_cycles(g, e->head);
            e->active = FALSE;
        }
        v->active = v->unprocessed = FALSE;
    }
    return rc;
}

int find_cn_simple_cycles(cn_graph  *g)
{
    int rc = 0;
    cn_vertex *v;
    cn_edge *e;

    for (v = g->vertexes; v; v = v->next) { 
      v->active = FALSE;
      v->unprocessed = TRUE;
      v->trail = NULL;
    }
    for (e = g->edges; e; e = e->next) {
      e->active = FALSE;
    }

    for (v = g->vertexes; v; v = v->next) 
      if (v->unprocessed) rc += cn_cycles(g, v);
    return rc;
}

void gen_mg_allocation_for_simple_cycles(cn_graph *g, array_t *components, 
                                         array_t *choice_events_sets)
{
  cn_cycle *c;
  cn_edge_list *el;

  for (c = g->cycles; c; c = c->next) {
    if (c->essential) {
      for (el = c->edges; el; el = el->next) {
        if (el->edge->type == CHOICE) {
          el->edge->event->not_in_allocation = FALSE;

          if (ddebug) fprintf(dbg, "%s->not_in_alloc = FALSE\n", 
                               el->edge->event->name);

        }
      }
      do_mg_reduce(components, choice_events_sets, FALSE);
      for (el = c->edges; el; el = el->next) {
        if (el->edge->type == CHOICE) {
          el->edge->event->not_in_allocation = TRUE;

          if (ddebug) fprintf(dbg, "%s->not_in_alloc = FALSE\n", 
                               el->edge->event->name);

        }
      }
    }
  }
}

int find_MG_components_using_simple_cycles(FILE **cm, char *filename)
{ 
  char buf[256];
  int i, j, n, ubound, nsuccessors;
  array_t *merge_events_sets, *choice_events_sets;
  array_t *choice_events, *merge_events;
  cn_graph *g;
  cn_vertex *v;
  cn_cycle *c;
  cn_edge_list *el;
  int successor=0;

  sprintf(buf, "%s.cm", filename);
  if ((*cm=fopen(buf, "w")) == NULL) {
    printf("\nERROR : Can't open %s\n", buf);
    fprintf(lg, "\nERROR : Can't open %s\n", buf);
    return FALSE;
  }

  g = alloc_cn_graph();

  for (i = 1; i < nevents; i++) {
    h_events[i]->weight = i;
    h_events[i]->removed = FALSE;
    h_events[i]->not_in_allocation = FALSE;
  }

  mg_components = array_alloc(array_t *, 0);

  choice_events_sets = get_choice_events_sets(&ubound);
  merge_events_sets = get_merge_events_sets();

  create_cn_vertexes(g, choice_events_sets, merge_events_sets);

  for (n = 0; n < array_n(choice_events_sets); n++) {
    choice_events = array_fetch(array_t *, choice_events_sets, n);
    for (j = 0; j < array_n(choice_events); j++) {
      i = array_fetch(int, choice_events, j);
      if ((v = find_destination(h_events[i]))) 
        add_cn_edge(g, h_events[i]->cn_choice_vertex, v, h_events[i], CHOICE);
      else return FALSE;
    }
  }
  for (n = 0; n < array_n(merge_events_sets); n++) {
    merge_events = array_fetch(array_t *, merge_events_sets, n);
    i = array_fetch(int, merge_events, 0);
    nsuccessors = 0;
    for (j = 1; j < nevents; j++) 
      if (!events[j]->dropped && non_redundant_rule(i,j)) {
        nsuccessors++;
        successor = j;
      }
    if (nsuccessors > 1) continue;
    i = array_fetch(int, merge_events, 0);
    if ((v = find_destination(h_events[successor]))) 
      add_cn_edge(g, h_events[i]->cn_merge_vertex, v, h_events[successor], MERGE);
    else return FALSE;
  }

  find_cn_simple_cycles(g);


  if (ddebug) 
    for (c = g->cycles; c; c = c->next) {
        fprintf(dbg, "cycle (id = %d) : ", c->id);
        for (el = c->edges; el; el = el->next) fprintf(dbg, "%d ",el->edge->id);
        fprintf(dbg, "\n");
    }

  
  find_cn_minimal_covering_cycles(g);

  for (i = 1; i < nevents; i++) h_events[i]->processed = FALSE;


  if (ddebug) 
    for (c = g->cycles; c; c = c->next) {
      if (c->essential) {
        fprintf(dbg, "cycle (id = %d) : ", c->id);
        for (el = c->edges; el; el = el->next) 
          if (el->edge->type == CHOICE) fprintf(dbg, "%d ", el->edge->id);
        fprintf(dbg, "\n");
      }
    }

  
  gen_mg_allocation_for_simple_cycles(g, mg_components, choice_events_sets);

  /* free */
  array_free(choice_events_sets); 
  array_free(merge_events_sets); 
  free_cn_graph(g);

  return TRUE;
}

int find_MG_components(FILE **cm, char *filename)
{ 
  char buf[256];
  int i, ubound;
  array_t *choice_events_sets, *cevents;
  int choices_are_ordered = FALSE;

  sprintf(buf, "%s.cm", filename);
  if ((*cm=fopen(buf, "w")) == NULL) {
    printf("\nERROR : Can't open %s\n", buf);
    fprintf(lg, "\nERROR : Can't open %s\n", buf);
    return FALSE;
  }
  for (i = 1; i < nevents; i++) {
    h_events[i]->weight = i;
    h_events[i]->removed = FALSE;
    h_events[i]->not_in_allocation = FALSE;
  }
  mg_components = array_alloc(array_t *, 0);
  choice_events_sets = get_choice_events_sets(&ubound);


  if (ddebug) fprintf(dbg, "Upper bound of the number of MG components = %d \n", ubound);
  if (ddebug) {
    for (i = 1; i < nevents; i++) 
      if (h_events[i]->not_in_allocation) 
        fprintf(dbg, "%s is not_in_allocation\n", h_events[i]->name);
  }


  for (i = 1; i < nevents; i++) h_events[i]->processed = FALSE;
  gen_mg_allocation(mg_components, choice_events_sets, array_n(choice_events_sets), choices_are_ordered);
  if (all_free_choice_has_no_concurrent_events())
    find_a_minimal_cover(mg_components);
  for (i = 0; i < array_n(choice_events_sets); i++) {
    cevents = array_fetch(array_t *, choice_events_sets, i);
    array_free(cevents);
  }
  array_free(choice_events_sets); 
  return TRUE;
}

void ts_dfs (int i, int *num_p)
{
  int j;

  if (h_events[i]->unprocessed) {
    h_events[i]->unprocessed = FALSE;
    for (j = 1; j < nevents; j++) ts_dfs (j, num_p);
    h_events[i]->reachable = (*num_p)++;
  }
}

array_t *top_sort ()
{
  int vertex_n;
  array_t *varray;
  int i;

  for (i = 1; i < nevents; i++) {
    h_events[i]->reachable = 0;
    h_events[i]->unprocessed = h_events[i]->subset;
  }

  vertex_n = 0;
  for (i = 1; i < nevents; i++) {
    if (h_events[i]->unprocessed) ts_dfs(i,&vertex_n);
  }

  varray = array_alloc (int, nevents);
  for (i = 1; i < nevents; i++) {
    h_events[i]->unprocessed =  h_events[i]->subset;
    if (!h_events[i]->subset) continue;
    array_insert (int, varray, h_events[i]->reachable, i);
  }

  return varray;
}

void scc_dfs (int i, array_t *vcomp, int component_n)
{   
  int j;

  if (h_events[i]->unprocessed) {
    h_events[i]->unprocessed = FALSE;
    /* Add to component array, and number it. */
    h_events[i]->comp_num = component_n;
    array_insert_last (int, vcomp, i);
    for (j = 1; j < nevents; j++)
      if (non_redundant_rule(j, i)) scc_dfs (j,vcomp,component_n);
  }
}

int strong_comp ()
{
  array_t *varray;
  array_t *vcomp;
  int i, n, component_n;

  varray = top_sort ();
  /* unprocessed is set appropriately. */
  component_n = 0;

  for (n=array_n(varray); n--; /*empty*/) {
    i = array_fetch (int,varray,n);
    if (h_events[i]->unprocessed) {
      vcomp = array_alloc (int, 0);
      scc_dfs (i,vcomp,component_n);
      component_n++;
      array_free (vcomp);
    }
  }

  array_free (varray);
  return component_n;
}

void remove_unselected_comp()
{
  int i;
  array_t *component, *newcomp;
  array_t *selected_mg_components;

  selected_mg_components = array_alloc(array_t *, 0);

  for (i=0; i < array_n(mg_components); i++) {
    component = array_fetch (array_t *,mg_components,i);
    if (selected_comp[i] == TRUE) {
      newcomp = array_dup(component);
      array_insert_last(array_t *, selected_mg_components, newcomp);
    }
  }
  array_free(mg_components);
  mg_components = selected_mg_components;
}

int check_reductions (FILE *cm)
{
  int status = TRUE;
  int i, j, k, n, count = 0, n_bad = 0;
  array_t *component;
  array_t *component_array;

  component_array = mg_components;
  for (i = 1; i < nevents; i++) h_events[i]->covered = FALSE;

  fprintf(cm, "There are %d marked graph components\n",
            array_n(component_array));
  if (selected_comp) {
    for (i = 0; i < array_n(component_array); i++)
      if (selected_comp[i]) count++;
    fprintf(cm, "%d components are selected\n", count);
  }

  for (i=0, n=0; i < array_n(component_array); i++) {
    component = array_fetch (array_t *,component_array,i);
    /* Make sure the component is strongly connected. */
    for (j = 1; j < nevents; j++) h_events[j]->subset = FALSE;
  
    /* Mark all vertices covered by the components. */
    for (j=0; j < array_n(component); j++) {
      k = array_fetch (int,component,j);
      h_events[k]->subset = TRUE;
    }
    if (!selected_comp || (selected_comp && selected_comp[i] == TRUE))
      for (j=0; j < array_n(component); j++) {
        k = array_fetch (int,component,j);
        h_events[k]->covered = TRUE;
      }
    if (selected_comp) {
      if (selected_comp[i]) 
      /* Print all components for debugging. */
      print_component (n++,array_fetch (array_t *,component_array,i),cm);
    }
    else {
      print_component (i,array_fetch (array_t *,component_array,i),cm);
    }
    if (strong_comp() != 1) {

      if (ddebug && status == TRUE) {
        printf("warning: %s component(s)", "marked graph");
      }
      if (ddebug) printf(" %d",i+1);

      status = FALSE;
      n_bad++;
    }
  }
  if (ddebug && n_bad != 0) {
        printf(" %s not strongly connected.\n",(n_bad==1)?"is":"are");
  }

  for (i = 1; i < nevents; i++) {
    if (!h_events[i]->covered && !events[i]->dropped) {
      status = FALSE;
      printf("warning: %s not covered by reductions\n",h_events[i]->name);
    }
  }

  return status;
}

int check_free_choice()
{
  int i, j, k, m, l, degree;
  int result = TRUE;
  char *rule1, *rule2;

  rule1 = (char *) malloc(nevents+1);
  rule2 = (char *) malloc(nevents+1);
  rule1[0] = '0';
  rule2[0] = '0';
  rule1[nevents] = '\0';
  rule2[nevents] = '\0';

  for (i = 1; i < nevents; i++) {
    for (j = 1; j < nevents; j++) h_events[j]->unprocessed = TRUE;
    for (j = 1; j < nevents; j++) {
      if (!h_events[j]->unprocessed || non_redundant_rule(i,j) == FALSE) 
        continue;
      degree = 0;
      for (k = 1; k < nevents; k++) {
        if (j == k || non_redundant_rule(i,k) == FALSE) continue;
        if (rules[j][k]->conflict) degree++;
      } 
      if (degree > 0) {
        for (k = 1; k < nevents; k++) h_events[k]->selected = FALSE;
        h_events[j]->selected = TRUE;
        h_events[j]->unprocessed = FALSE;
        
        for (k = 1; k < nevents; k++) {
          if (j == k || non_redundant_rule(i,k) == FALSE) continue;
          if (rules[j][k]->conflict) {
            h_events[k]->selected = TRUE;
            h_events[k]->unprocessed = FALSE;
          }
        } 
        for (k = 1; k < nevents; k++) {
          if (h_events[k]->selected == FALSE) continue;
          for (m = 1; m < nevents; m++) {
            if (non_redundant_rule(m,k)) rule1[m] = '1';
            else rule1[m] = '0';
          }
          
          for  (l = 1; l < nevents; l++) {
            if (k == l || h_events[l]->selected == FALSE) continue;
            for (m = 1; m < nevents; m++) {
              if (non_redundant_rule(m,l)) rule2[m] = '1';
              else rule2[m] = '0';
            }
            if (strcmp(rule1, rule2) != 0) {
              result = FALSE;

              if (ddebug) {
                fprintf(dbg, "\nEXCEPTION : choice event %s is not a free-choice\n", 
                       h_events[k]->name);
                fprintf(dbg, "Because choices have different predecessor events\n");
                fprintf(dbg, "Predecessor = %s\n", h_events[i]->name);
                fprintf(dbg, "rule1[%s] = %s\nrule2[%s] = %s\n", 
                       h_events[k]->name, rule1, h_events[l]->name, rule2);
              }

            }
          }
        }
      }
    }
  }
  free(rule1);
  free(rule2);
  return result;
}

int open_verbose_files(char *filename, FILE **dbg, FILE **rel, FILE **pg)
{
  char buf[256];

  if (ddebug) {
    sprintf(buf, "%s.dbg", filename);
    if ((*dbg=fopen(buf, "w")) == NULL) {
      printf("\nERROR : Can't open %s\n", buf);
      fprintf(lg, "\nERROR : Can't open %s\n", buf);
      return FALSE;
    }
  }
  sprintf(buf, "%s.rel", filename);
  if ((*rel=fopen(buf, "w")) == NULL) {
    printf("\nERROR : Can't open %s\n", buf);
    fprintf(lg, "\nERROR : Can't open %s\n", buf);
    return FALSE;
  }
  sprintf(buf, "%s.pg", filename);
  if ((*pg=fopen(buf, "w")) == NULL) {
    printf("\nERROR : Can't open %s\n", buf);
    fprintf(lg, "\nERROR : Can't open %s\n", buf);
    return FALSE;
  }
  return TRUE;
}

void print_basic_info(cycleADT ****cycles, int ncycles)
{
  // boundADT timediff1, timediff2;
  int i, j;

    for (i = 0; i < nsignals; i++) 
      fprintf(dbg,"Signal's name = %s\n", signals[i]->name);

    for (i = 0; i < nevents; i++) {
      fprintf(dbg,"Event's name = %s signal = %d (%s) \n", events[i]->event, 
              events[i]->signal, events[i]->dropped ? "dropped" : "");
    }
    for (i = 1; i < nevents; i++)
      for (j = 1; j < nevents; j++)
        if (rules[i][j]->ruletype != 0) {
          fprintf(dbg, "Rule's type %s -> %s = %d\n",
                 events[i]->event, events[j]->event, rules[i][j]->ruletype); 
        }
    for (i = 1; i < nevents; i++)
      for (j = 1; j < nevents; j++)
        if (rules[i][j]->conflict != 0) 
           fprintf(dbg, "Rule is conflict : %s -> %s (rule type = %d)\n",
                   events[i]->event, events[j]->event, rules[i][j]->ruletype); 
/*
    for (i = 1; i < (nevents - ndummy); i++)
      for (j = 1; j < (nevents - ndummy); j++) {
          timediff1 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,0);
          timediff2 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,1);
          fprintf(dbg,"%s and %s : L = %d U = %d L = %d U = %d\n",
                 events[i]->event, events[j]->event, timediff1->L, 
                 timediff1->U, timediff2->L, timediff2->U);
          free(timediff1);
          free(timediff2);
        }
    for (i = 1; i < (nevents - ndummy); i++)
      for (j = 1; j < (nevents - ndummy); j++) 
        if (reachable(cycles,nevents,ncycles,i,0,j,0) != notreachable)
          fprintf(dbg, "reachable(%s, %s) is TRUE\n", 
                       events[i]->event, events[j]->event);
        else 
          fprintf(dbg, "reachable(%s, %s) is FALSE\n", 
                       events[i]->event, events[j]->event);
*/
}

void find_MG_timed_concurrent_events(cycleADT ****cycles, int ncycles,FILE *rel)
{
  boundADT timediff1, timediff2;
  int i,j;

  for (i = 1; i < (nevents - ndummy); i++) {
    if (events[i]->dropped) continue;
    for (j = 1; j < (nevents - ndummy); j++) {
      if (i == j || events[j]->dropped) continue;


      if (ddebug) {
        fprintf(dbg, "i = %s j = %s ordered_table[i][j] = %d R(i,0,j,0) = %d R(j,0,i,0) = %d notreachable = %d\n", h_events[i]->name, h_events[j]->name, ordered_table[i][j],
reachable(cycles,nevents,ncycles,i,0,j,0), reachable(cycles,nevents,ncycles,j,0,i,0), notreachable);
      }


      if (concurrency_table[i][j] || ordered_table[i][j]) continue;
      if (((reachable(cycles,nevents,ncycles,i,0,j,0) == notreachable) &&
           (reachable(cycles,nevents,ncycles,j,0,i,0) == notreachable)) ||
           (reachable(cycles,nevents,ncycles,i,0,j,1) == notreachable) ||
           (reachable(cycles,nevents,ncycles,j,0,i,1) == notreachable))
      {
        if (si_mode) {
          concurrency_table[i][j] = TRUE;
          concurrency_table[j][i] = TRUE;
        }
        else {
          timediff1 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,0);
          timediff2 = WCTimeDiff(rules,cycles,nevents,ncycles,i,j,1);
          if (timediff1->L <= 0 && timediff1->U >= 0 ||
              timediff2->L <= 0 && timediff2->U >= 0) {
            concurrency_table[i][j] = TRUE;
            concurrency_table[j][i] = TRUE;
          }
          else {
            ordered_table[i][j] = TRUE;
            ordered_table[j][i] = TRUE;
          }
          free(timediff1);
          free(timediff2);
        }
      } 
      else {
        ordered_table[i][j] = TRUE;
        ordered_table[j][i] = TRUE;
      }
    }
  }
  if (verbose && rel) print_concurrency_table(rel);
}

void find_FC_timed_concurrent_events(cycleADT ****cycles, int ncycles,FILE *rel)
{
  int i;
  array_t *component;

  printf("\nFinding timed concurrency relations...");
  fflush(stdout);
  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    find_MG_timed_concurrent_events(cycles, ncycles, NULL);
    restore_to_original_spec();
  }
  if (verbose) print_concurrency_table(rel);
  printf("done\n");
  fflush(stdout);
}

void find_FC_conflict_events(FILE *fp)
{
  int i, j, k;
  array_t *component;

  printf("Finding conflict events... ");
  fflush(stdout);

  for (j = 1; j < (nevents - ndummy); j++) 
    for (k = 1; k < (nevents - ndummy); k++) 
      conflict[j][k] = 1;

  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    for (j = 1; j < (nevents - ndummy); j++) {
      if (events[j]->dropped) continue;
      for (k = 1; k < (nevents - ndummy); k++) {
        if (events[k]->dropped) continue;
        conflict[j][k] = 0;
      }
    }
    restore_to_original_spec();
  }


  if (ddebug) {
    for (j = 1; j < (nevents - ndummy); j++) {
      if (events[j]->dropped) continue;
      fprintf(fp, "%s is conflict with : ", h_events[j]->name);
      for (k = 1; k < (nevents - ndummy); k++) {
        if (events[k]->dropped) continue;
        if (conflict[j][k]) fprintf(fp, "%s ", h_events[k]->name);
      }
      fprintf(fp, "\n");
    }
  }

  printf("done\n");
  fflush(stdout);
}
  
void print_components(FILE *fp) 
{
  int i;

  for (i = 0; i < array_n(mg_components); i++) {
    fprintf(fp, "Printing component %d\n", i);
    print_component (i,array_fetch (array_t *,mg_components,i),fp);
  }
}

void print_component_graph(char menu, char command, designADT design, 
			   char *filename, FILE *fp) 
{
  int i, j, k;
  array_t *component;
  char buf[256];

    if (print_graph) 
      print_grf(filename,events,rules, nevents,TRUE,NULL);
    for (i = 0; i < array_n(mg_components); i++) {
      fprintf(fp, "Printing component %d\n", i);
      print_component (i,array_fetch (array_t *,mg_components,i),fp);
      component = array_fetch(array_t *, mg_components, i);
      for (j = 1; j < nevents; j++) h_events[j]->selected = FALSE;
      for (j = 0; j < array_n(component); j++) {
        k = array_fetch(int, component, j);
        h_events[k]->selected = TRUE;
      }
      for (j = 1; j < nevents; j++) 
        if (h_events[j]->selected == FALSE) events[j]->dropped = TRUE;
      for (j = 1; j < nevents; j++) 
        for (k = 1; k < nevents; k++)
          if (rules[j][k]->ruletype != 0 && (h_events[j]->selected == FALSE || 
                 h_events[k]->selected == FALSE))
            rules[j][k]->ruletype =  rules[j][k]->ruletype | REDUNDANT;

      design->status  = design->status & ~FOUNDRED;
      design->verbose = FALSE;
      if (!find_redundant_rules(menu,command,design)) {
        printf("ERROR: during find redundant rules\n");
        fprintf(lg, "ERROR: during find redundant rules\n");
      }
     design->verbose = verbose;
      for (j = 1; j < nevents; j++) 
        for (k = 1; k < nevents; k++)
          if (!events[j]->dropped && !events[k]->dropped && 
              rules[j][k]->ruletype != 0)
            fprintf(fp, "%s -> %s ruletype = %d\n", h_events[j]->name, 
                     h_events[k]->name, rules[j][k]->ruletype);
 
      if (print_graph) dot_print_grf(filename,events,rules, nevents,TRUE);
      sprintf(buf, "%s%d", filename, i);
      if (print_graph) 
	print_grf(buf,events,rules, nevents,TRUE,NULL);
      for (j = 1; j < nevents; j++) events[j]->dropped = org_dropped[j];
      for (j = 1; j < nevents; j++)
        for (k = 1; k < nevents; k++)
          rules[j][k]->ruletype = org_ruletype[j][k];
    }
}

stjung_signal *alloc_hsignal_struct()
{
  stjung_signal *newhsignal;

  newhsignal = (stjung_signal *) malloc(sizeof(struct signal));
  newhsignal->set = NULL;
  newhsignal->reset = NULL;
  return newhsignal;
}

hevent *alloc_hevent_struct()
{
  hevent *newhevent;

  newhevent = (hevent *) malloc(sizeof(hevent));
  newhevent->pr_list = NULL;
  newhevent->nr_list = NULL;
  newhevent->extra = NULL;
  newhevent->cn_choice_vertex = NULL;
  newhevent->cn_merge_vertex = NULL;
  newhevent->xlist = NULL;
  return newhevent;
}

event_list *
find_one_stable_events(hevent *output_event)
{
  int i, j, cause_all;
  event_list *stable_events = NULL;

  for (i = 1; i < (nevents - ndummy); i++) {
    if (some_event_cause(h_events[i], output_event) == FALSE) continue;
    cause_all = TRUE;
    for (j = 1; j < (nevents - ndummy); j++) 
      if (!events[j]->dropped && !cause(h_events[i], h_events[j])) 
        cause_all = FALSE;
    if (cause_all) {

      if (ddebug) fprintf(dbg, "%s is one stable\n", h_events[i]->name);

      add_list(&stable_events, h_events[i]);
    }
  }
  return stable_events;
} 

event_list *
find_zero_stable_events(event_list *events_set)
{
  int i, j, k, first, direction, cause_one, cause_all, candidate;
  int stable;
  event_list *stable_events = NULL, *el;

  for (i = 0; i < nsignals; i++) {
    stable = TRUE;
    for (j = h_signals[i]->first; j <= h_signals[i]->last; j++) 
      if (events[j]->dropped == FALSE) stable = FALSE;
    if (!stable) continue; 

    candidate = TRUE;
    first = h_signals[i]->first;
    direction = get_cause_event_type(h_events[first], events_set->event);
    if (some_event_concur(h_events[first],events_set->event)) candidate = FALSE;
    for (el = events_set->next; el; el = el->next) {
      if (get_cause_event_type(h_events[first], el->event) != direction) 
        candidate = FALSE;
      if (some_event_concur(h_events[first], el->event)) candidate = FALSE;
    }
    if (!candidate) continue; 

    cause_all = TRUE;
    for (j = 1; j < (nevents - ndummy); j++) {
      if (!events[j]->dropped) {
        cause_one = FALSE;
        for (k= h_signals[i]->first; !cause_one && k <= h_signals[i]->last; k++)
          if (!org_dropped[k] && direction != h_events[k]->event_type && 
              cause(h_events[k], h_events[j])) cause_one = TRUE;
        if (!cause_one) cause_all = FALSE;
      }
    }
    if (cause_all) {

      if (ddebug) fprintf(dbg, "%s is zero stable\n", h_signals[i]->name);

      for (j = h_signals[i]->first; j <= h_signals[i]->last; j++) {
        if (org_dropped[j] == FALSE && direction != h_events[j]->event_type) {
          add_list(&stable_events, h_events[j]);
          break;
        }
      }
    }
  }
  return stable_events;
}

int is_in_same_cycle(hevent *s, hevent *t)
{
  int i, j, l;

  i = s->index % nevents;
  j = t->index % nevents;
  l = s->nr->index % nevents;

//  if (ddebug) fprintf(dbg, "nevents = %d i = %d j = %d l = %d\n", nevents, i, j, l);

  if ((reachable(cycles,nevents,ncycles,i,0,l,0) != notreachable &&
       reachable(cycles,nevents,ncycles,j,0,l,0) != notreachable) ||
      (reachable(cycles,nevents,ncycles,l,0,i,0) != notreachable &&
       reachable(cycles,nevents,ncycles,j,0,l,1) != notreachable)
)
  {
//    if (ddebug) fprintf(dbg, "%s and %s is in the same cycle\n", s->name, t->name);
    return TRUE;
  }
  else
    return FALSE;
}

int is_in_diff_cycle(hevent *s, hevent *t)
{
  boundADT timediff3;
  int i, j, l;

  i = s->index % nevents;
  j = t->index % nevents;
  l = s->nr->index % nevents;

  if (si_mode) {
    if ((reachable(cycles,nevents,ncycles,i,0,j,0) == notreachable &&
         reachable(cycles,nevents,ncycles,i,0,l,0) == notreachable &&
         reachable(cycles,nevents,ncycles,j,0,l,0) != notreachable))
    {

      if (ddebug) fprintf(dbg, "%s and %s is in the diff cycle\n", s->name, t->name);

      return TRUE;
    }
    else 
      return FALSE;
  }
  else {
    timediff3 = WCTimeDiff(rules,cycles,nevents,ncycles,j,l,0);
    if (reachable(cycles,nevents,ncycles,i,0,j,0) == notreachable &&
        reachable(cycles,nevents,ncycles,i,0,l,0) == notreachable &&
        (reachable(cycles,nevents,ncycles,j,0,l,0) != notreachable || 
         timediff3->L > 0))
    {

      if (ddebug) fprintf(dbg, "%s and %s is in the diff cycle\n", s->name, t->name);

      return TRUE;
    }
    else return FALSE;
  }
}

hlogic *
unfolded_distinct_off_cubes(hevent *curr, hevent *dst)
{
  xnode *xl;
  term *c;
  hlogic *off_cubes;
  char buf[256];

  off_cubes = NULL;

  for (xl = curr->xlist; xl; xl = xl->next) {
    if (xl->extra) {
      if (off_cubes == NULL) off_cubes = copy_net(xl->extra);
      else set_union(xl->extra, off_cubes, TRUE); 
    }

    if (ddebug) {
      sprintf(buf, "xl->extra of %s = ", xl->event->name); 
      print_logic(dbg, buf, xl->extra, TRUE, SET_NET);
    }

  }

  if (off_cubes && !initial_cube[curr->signal->index]) {
    for (c = off_cubes->terms; c; c = c->next)
      new_literal_unique(c, h_events[curr->index % nevents]);
  }


  if (ddebug) {
    sprintf(buf, "Extra signals from %s to %s\n", curr->name, dst->name);
    print_logic(dbg, buf, off_cubes, TRUE, SET_NET);
  }

  return off_cubes;
}

  

hlogic *
unfolded_find_all_paths_dfs(hevent *src, hevent *dst, hevent *curr)
{
  term *new_term;
  xnode *xl;
  hlogic *off_cubes;


  if (ddebug) 
    fprintf(dbg, "src = %s dst = %s curr = %s\n", 
            src->name, dst->name, curr->name);


  if (curr->selected) {
    if (curr->extra) {
      off_cubes = copy_net(curr->extra->net);
      return off_cubes;
    }
    else return NULL;
  }

  if (curr->visited) {

    if (ddebug) fprintf(dbg,"%s is visited already\n", curr->name);

    return NULL;
  }
 
  curr->visited = TRUE;

  if (curr == dst) {

    if (ddebug) {
      fprintf(dbg, "dest is visited\n");
    }

    off_cubes = alloc_net(NULL, NULL);
    new_term = alloc_term(off_cubes);
    curr->visited = FALSE;
    return off_cubes;
  }

  for (xl = curr->xlist; xl; xl = xl->next)
    xl->extra = unfolded_find_all_paths_dfs(src, dst, xl->event);

  off_cubes = unfolded_distinct_off_cubes(curr, dst);

  curr->selected = TRUE;
  if (off_cubes) add_logic_list(off_cubes, curr, dst);
  return off_cubes;
}


hlogic *
unfolded_find_all_paths(event_list *Sn, event_list *Dn, hlogic *net, int pass)
{
  int i;
  event_list *sl, *dl;
  hlogic *all_off_cubes=NULL, *off_cubes=NULL;
  hevent *t;
  xnode *xl;
  char buf[256];

  for (sl = Sn; sl; sl = sl->next) {
    for (dl = Dn; dl; dl = dl->next) {
      if (sl->event->index % nevents != dl->event->index % nevents) continue;
      for (i = 1; i < nevents * 2; i++) {
        t = unfolded_events[i];
        t->visited = FALSE;
      }
  
      for (i = 1; i < nevents * 2; i++) {
        unfolded_events[i]->selected = FALSE;
        unfolded_events[i]->extra = NULL;
      }

      off_cubes = unfolded_find_all_paths_dfs(sl->event, dl->event, sl->event);


      if (ddebug) {
        if (off_cubes) {
          sprintf(buf, "unfolded p-graph : off cubes from %s to %s = ",
                   sl->event->name, dl->event->name);
          print_logic(dbg, buf, off_cubes, TRUE, SET_NET);
          fprintf(dbg, "\n");

        }
        else fprintf(dbg,"\nunfolded p-graph: %s is not reachable from %s\n\n",
                     dl->event->name, sl->event->name);
      }


      if (off_cubes) {
        if (all_off_cubes == NULL) all_off_cubes = copy_net(off_cubes);
        else {
          if (pass == 1 && heuristic2) 
            all_off_cubes = set_union_minimal(off_cubes, all_off_cubes, net, TRUE);
          else 
            set_union(off_cubes, all_off_cubes, TRUE);
        }
        delete_net(off_cubes);
      }
      for (i = 1; i < nevents * 2; i++)
        if (unfolded_events[i]->extra) 
           free_logic_list(unfolded_events[i]->extra); 

      for (i = 1; i < nevents * 2; i++) {
        for (xl = unfolded_events[i]->xlist; xl; xl = xl->next) {
          if (xl->extra) {
            delete_net(xl->extra);
            xl->extra = NULL;
          }
        }
      }
    }
  }
  return all_off_cubes;
}

hlogic *
find_off_cubes(int comp, hlogic *net, int pass, FILE *pg)
{
  int i;
  hqueue *front, *rear;
  event_list *Sn=NULL, *Dn=NULL;
  hevent *s, *t, *u, *v;
  hlogic *off_cubes = NULL;
  char buf[256];
  xnode *xl;
  hevent *e;

  e = net->event;

  unfolded_events = (hevent **) malloc(2 * nevents * sizeof (hevent *));
  for (i = 0; i < nevents * 2; i++) {
    unfolded_events[i] = (hevent *) malloc(sizeof(hevent));
    unfolded_events[i]->pr_list = h_events[i % nevents]->pr_list; 
    unfolded_events[i]->nr_list = h_events[i % nevents]->nr_list; 
    unfolded_events[i]->pr = h_events[i % nevents]->pr; 
    unfolded_events[i]->nr = h_events[i % nevents]->nr; 
    unfolded_events[i]->extra = NULL; 
    unfolded_events[i]->signal = h_events[i % nevents]->signal;
    unfolded_events[i]->name=(char*)malloc(strlen(h_events[i%nevents]->name)+3);
    sprintf(buf, "%s,%s", h_events[i % nevents]->name, i > nevents ? "1" : "0");
    strcpy(unfolded_events[i]->name, buf);
    unfolded_events[i]->index = i;
    unfolded_events[i]->event_type = h_events[i % nevents]->event_type;
  }

  front = rear = NULL;
  for (i = 0; i < nevents * 2; i++) {
    unfolded_events[i]->selected = FALSE;
    unfolded_events[i]->xlist = NULL;
  }

  for (i = 1; i < nevents - ndummy; i++) {
    if (!events[i]->dropped && some_rev_event_cause(h_events[i], e)) 
    {
      add_list(&Sn, unfolded_events[i]);
      add_queue(unfolded_events[i], &front, &rear);
      unfolded_events[i]->selected = TRUE; 
      add_list(&Dn, unfolded_events[i+nevents]);
    }
  }

  while ((s = delete_queue(&front, &rear))) {
    u = h_events[s->index % nevents];
    if (s->index < nevents) {
      for (i = 1; i < nevents - ndummy; i++) {
        t = unfolded_events[i];
        v = h_events[i];


        if (ddebug) {
           fprintf(dbg, "\nu = %s v = %s, u->nr = %s e = %s\n", 
              u->name, v->name, u->nr->name, e->name);
           fprintf(dbg, "v->dropped = %d cause(u,v) = %d cause(v, u->nr) = %d some_rev_evt_cause(v, e) = %d is_in_diff_cycle(u, v) = %d\n", 
              events[i]->dropped, cause(u, v), cause(v, u->nr),
              some_rev_event_cause(v, e),is_in_diff_cycle(u, v));
        }


        if (!events[i]->dropped && (cause(u, v) || is_concur(u, v)) &&
            cause(v, u->nr) && some_rev_event_cause(v, e) &&
            !is_in_diff_cycle(u, v) && !is_concur(v, u->nr)) {
          add_to_xlist(s, t);
          if (!t->selected) {
            add_queue(t, &front, &rear);
            t->selected = TRUE;
          }
        }
      }
      for (i = nevents+1; i < (nevents * 2) - ndummy; i++) {
        t = unfolded_events[i];
        v = h_events[i % nevents];
        if (!events[i % nevents]->dropped && (cause(u, v) || is_concur(u, v)) &&
            cause(v, u->nr) && some_rev_event_cause(v, e) &&
            is_in_diff_cycle(u, v)) {
          add_to_xlist(s, t);
          if (!t->selected) {
            add_queue(t, &front, &rear);
            t->selected = TRUE;
          }
        }
      }
    }
    else {
      for (i = nevents+1; i < (nevents * 2) - ndummy; i++) {
        t = unfolded_events[i];
        v = h_events[i % nevents];
        if (!events[i % nevents]->dropped && (cause(u, v) || is_concur(u, v)) &&
            cause(v, u->nr) && some_rev_event_cause(v, e) &&
            !is_in_diff_cycle(u, v) && !is_concur(v, u->nr)) {
          add_to_xlist(s, t);
          if (!t->selected) {
            add_queue(t, &front, &rear);
            t->selected = TRUE;
          }
        }
      }
    }
  }

  if (verbose) {
    fprintf(pg, "The unfolded precedence graph for MG-component %d\n", comp);
    unfolded_print_precedence_graph(Sn, Dn, pg);
  }

  off_cubes = unfolded_find_all_paths(Sn, Dn, net, pass);

  for (i = 0; i < nevents * 2; i++)
    if ((xl = get_xlist(unfolded_events[i])))  free_xlist(xl); 
  for (i = 0; i < nevents * 2; i++) {
    if (unfolded_events[i]->name) free(unfolded_events[i]->name);
    if (unfolded_events[i]) free(unfolded_events[i]); 
  }
  free(unfolded_events);

  return off_cubes;

}

int is_always_on(term *cube, event_list *one_stable_events)
{
  int exist;
  event_list *l, *e;

  for (e = cube->llist; e; e = e->next) {
    exist = FALSE;
    for (l = one_stable_events; l; l = l->next)
      if (IS_REV_TYPE(e->event, l->event)) exist = TRUE;
    if (exist == FALSE) {
//      print_list(stdout, "Always not ON cube : ", cube->llist);
      return FALSE;
    }
  }
//  print_list(stdout, "Always ON cube : ", cube->llist);
  return TRUE; 
}

int is_always_off(term *c, hlogic *off_cubes)
{
  int all_exist, exist;
  event_list *l, *m;
  term *cube;
  
  for (cube = off_cubes->terms; cube; cube = cube->next) {
    all_exist = TRUE;
    for (l = cube->llist; l; l = l->next) {
      exist = FALSE;
      for (m = c->llist; m; m = m->next) {
        if (IS_SAME_TYPE(m->event, l->event)) exist = TRUE;
      }
      if (exist == FALSE) all_exist = FALSE;
    }
    if (all_exist) {
//      print_list(stdout, "Always off cube ", c->llist);
      return TRUE;
    }
  }
//  print_list(stdout, "Always not off cube ", c->llist);
  return FALSE;
}

hlogic *
decomp_MG_find_single_cubes(hlogic *net, event_list *events_set, 
                           int shared, int combo, FILE *pg, int pass)
{
  event_list **S1=NULL, **S2=NULL, **S3=NULL, *Sn, *E, *Dn, *l;
//  event_list *cl, *trigger_events, *el, *tl;
//  event_list *cl, *el, *tl;
  event_list *el, *tl;
  hevent *snr, *zi, *zj, *zk, *s, *t, *e, *event;
  hqueue *front, *rear;
  int i, j, index, ndests, found;
  char buf[256];
  hlogic *interval_net[100];
  int k=0;
  hlogic *W, *candidate_set=NULL, *free_net;
  term *c;
  int count = 0;

  if (shared) {
    if (check_persistency(events_set) == FALSE) {
      printf("\nEXCEPTION: Persistency Violation!\n");
      return NULL;
    }
  }

  decomp_get_initial_logic(rules, h_events, nevents, net, events_set);

  for (el = events_set, k = 0; el; el = el->next, k++) {
    zi = el->event;
    zj = zi->nr;
    if (shared == FALSE) zk = zi;
    else {
      found = FALSE;
      event = zi->nr->nr;
      while (!found && event != zi) {
        for (tl = events_set; tl; tl = tl->next) {
          if (event == tl->event) found = TRUE;
        }
        if (!found) event = event->nr->nr;
      }
      assert (found);
      zk = event;
    }


    if (ddebug) fprintf(dbg,"MG_find_a_single_cube: zi = %s zj = %s zk = %s\n",
                          zi->name, zj->name, zk->name);
    if (ddebug) 
    {
      if (event_type(zi) == RISING) 
        print_logic(dbg, "Initial logic : ", net, TRUE, SET_NET);
      else 
        print_logic(dbg, "Initial logic : ", net, TRUE, RESET_NET);
    }


    front = rear = NULL;
    for (i = 0; i < nevents; i++) {
      h_events[i]->selected = FALSE;
      h_events[i]->xlist = NULL;
    }
  
    find_destination_nodes(zi, zj, zk, &E, &Dn);


    if (ddebug) {
      print_list(dbg, "Trigger Events: ", E);
      print_list(dbg, "Destination Events: ", Dn);
    }

    ndests = 0;
    for (l = Dn; l; l = l->next) ndests++;
    S1 = (event_list **) malloc(sizeof(event_list *) * ndests);
    S2 = (event_list **) malloc(sizeof(event_list *) * ndests);
    S3 = (event_list **) malloc(sizeof(event_list *) * ndests);
    for (i = 0; i < ndests; i++) {
      S1[i] = NULL;
      S2[i] = NULL;
      S3[i] = NULL;
      add_list(&(S3[i]), zi);
    }
  
    Sn = NULL;
    add_list(&Sn, zi);
    add_queue(zi, &front, &rear);
    zi->selected = TRUE;
  
    if (!gatelevel || get_max_nevents() == 1) {

      if (ddebug) fprintf(dbg, "!gatelevl && max_nevents = 1\n");

      for (i = 1; i < (nevents - ndummy); i++) {   
        if (events[i]->dropped) continue;
        s = h_events[i];
        if (cause(zi, s) && some_rev_event_cause(s, zi) &&
          cause(s, zj)) {
          add_list(&Sn, s);
          add_queue(s, &front, &rear);
          s->selected = TRUE;
        }
      }

      if (ddebug) print_list(dbg, "Sn = ", Sn);

      for (i = 0; i < ndests; i++) {
         S3[i] = copy_list(Sn);
         S1[i] = NULL;
         S2[i] = NULL;
      }

      if (ddebug) 
        for (i = 0; i < ndests; i++) print_list(dbg, "S3[%d] = ", S3[i]);

    }
    else {
      for (l = Dn, index = 0; l; l = l->next, index++) 
      {
        e = NULL;
        for (j=h_signals[l->event->signal->index]->first; 
             !e && j<=h_signals[l->event->signal->index]->last;j++) {
          if (events[j]->dropped) continue;
          if (cause(h_events[j], zi)) e = h_events[j];
        }

        for (i = 1; i < (nevents - ndummy); i++) {
          if (events[i]->dropped) continue;
          s = h_events[i];

          if (ddebug) fprintf(dbg, "\nChecking: s = %s\n", s->name);
          if (ddebug) fprintf(dbg, "s->pr = %s s->pr->pr = %s\n", s->pr->name, 
                                s->pr->pr->name);

          if (events_num(s->signal) > 1 && cause(zi, s->pr) && 
              cause(zi, s->pr->pr) && cause(zi, s)) continue;
          if (is_concur(zj, e->nr->nr)) {

            if (ddebug) fprintf(dbg, "max_nevents > 1 && zj || e->nr->nr\n");

            if (cause(zi, s) && !is_concur(s, zj) && 
                   some_rev_event_cause(s, zi) &&
                   (cause(e, s) || cause(e->nr, s)) && 
                   !is_concur(s, e->nr->nr)) {
              add_list_unique(&(S3[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
            else if (cause(zi, s) && !is_concur(s, zj) && 
                         some_rev_event_cause(s, zi)) {
              add_list_unique(&(S1[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
            else if (cause(zi, s) && some_rev_event_cause(s, zi) &&
                    (cause(e, s) || cause(e->nr, s)) &&
                   !is_concur(s, e->nr->nr)) {
              add_list_unique(&(S2[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;
            }
          }
          else {

            if (ddebug) fprintf(dbg,"max_nevents > 1 && NOT (zj || e->nr->nr)\n");
            if (ddebug) {
              fprintf(dbg, "s = %s e = %s\n", s->name, e->name);
              if (!cause(zi,s)) fprintf(dbg, "zi does not cause s\n");
              if (!cause(s, zi)) fprintf(dbg, "s does not cause zi\n");
              if (!some_rev_event_cause(s, zi)) fprintf(dbg, "some_rev_event_cause does not cause zi\n");
              if (!cause(e, s)) fprintf(dbg, "e does not cause s\n");
              if (!cause(e->nr, s)) fprintf(dbg, "e->nr does not cause s\n");
              if (is_concur(s, e->nr->nr)) fprintf(dbg, "is concur\n");
            }

            if (cause(zi, s) && !is_concur(s, zj) && 
                   some_rev_event_cause(s, zi) &&
                   (cause(e, s) || cause(e->nr, s)) &&
                   !is_concur(s, e->nr->nr)) {
              add_list_unique(&(S3[index]), s);
              add_list_unique(&Sn, s);
              add_queue(s, &front, &rear);
              s->selected = TRUE;

              if (ddebug) fprintf(dbg,"%s is added as a src node\n",s->name);

            }
            else {

              if (ddebug) fprintf(dbg,"%s is not a src node\n",s->name);

            }
          }
        }
      }
    }
  

    if (ddebug) {
      print_list(dbg, "Sn = ", Sn);
      for (i = 0; i < ndests; i++) {
        sprintf(buf, "S1[%d] = ", i);
        print_list(dbg, buf, S1[i]);
      }
      for (i = 0; i < ndests; i++) {
        sprintf(buf, "S2[%d] = ", i);
        print_list(dbg, buf, S2[i]);
      }
      for (i = 0; i < ndests; i++) {
        sprintf(buf, "S3[%d] = ", i);
        print_list(dbg, buf, S3[i]);
      }
    }

    /* expand the  precedence graph */
    /* FOREACH unprocessed node s* of X */
    while ((s = delete_queue(&front, &rear))) {
      snr = s->nr;
      for (i = 1; i < (nevents - ndummy); i++) { 
        if (events[i]->dropped) continue;
        t = h_events[i];
        if ((cause(s, t) || is_concur(t, s)) &&
            cause(t, snr) && some_rev_event_cause(t, zi)) {
          add_to_xlist(s, t);
          if (!t->selected) {
             add_queue(t, &front, &rear);
             t->selected = TRUE;
          }
        }
      }
    }
    if (verbose) {
      fprintf(pg, "\nThe Precedence Graph for the interval %s => %s\n", 
                zi->name, zj->name);
      print_precedence_graph(Sn, Dn, pg);
    }
  
    W = MG_find_all_covering_cubes(net,zi,zj,Sn,S1,S2,S3,ndests,E,Dn,pass);

    if (!W) {

      if (ddebug) fprintf(dbg,"Can't find any cover\n");

      return NULL;
    }
    else {

      if (ddebug) print_logic_and_src(dbg, "All covering cubes : ", W, TRUE, SET_NET);

    }

    for (c = W->terms; c; c = c->next) count++;

    if (ddebug) fprintf(dbg, "Number of all covering cubes = %d\n", count);


    for (i = 0; i < ndests; i++) {
      if (S1[i]) free_list(S1[i]);
      if (S2[i]) free_list(S2[i]);
      if (S3[i]) free_list(S3[i]);
    }
    free_list(Sn);
    if (Dn) free_list(Dn);
    free_list(E);
    free_xgraph();
    free(S1);
    free(S2);
    free(S3);

    interval_net[k] = copy_net(W);
    delete_net(W);
    W = NULL;
  }

  candidate_set = copy_net(interval_net[0]);
  delete_net(interval_net[0]);

  if (shared) {
    for (i = 1; i < k; i++) {
      free_net = candidate_set;
      candidate_set = multiply_two_net_and_src(candidate_set, interval_net[i], 
                                               TRUE);
      delete_net(free_net);
    }
    for (i = 1; i < k; i++) delete_net(interval_net[i]);
  }

  for (c = candidate_set->terms; c; c = c->next)
    for (l = net->terms->llist; l; l = l->next) 
      new_literal_unique(c, l->event->nr);

  return candidate_set;
}

int decomp_find_a_minimal_shared_covering_cube(hlogic *net, 
                               hlogic *candidate_set,
                               char * sm_shared_enabling_cube, 
                               event_list *events_set,
                               hevent *zi, hevent *zj,
                               int combo, int pass)
{
  int i, can_be_shared, nsharedevents=0, cost, minimal_cost, st_minimal_cost;
  int can_be_combinational, is_combinational;
  term *prev_term, *free_term, *result_term, *term;
  event_list *tl, *el, *cl, *cause_events;
  int is_all_combinational;
  array_t *component;
  event_list *subset;
  int result;

  for (el = events_set; el; el = el->next) nsharedevents++;

  /* get the minimal cost solutions */
  /* minimal_cost = infinite */
  minimal_cost = 0x7FFFFFFFL;
  st_minimal_cost = 0x7FFFFFFFL;
  /* FOREACH s in U
        s->cost = num_of_extra_sig(s)
        if (minimal_cost > s->cost) minimal_cost = s->cost */
  prev_term = NULL;
  for (term = candidate_set->terms; term; ) {
    can_be_shared = TRUE;
    for (tl = term->llist; tl; tl = tl->next) {
      if (sm_shared_enabling_cube[tl->event->signal->index] == 'X' ||
          event_type(tl->event) == RISING &&
          sm_shared_enabling_cube[tl->event->signal->index] == '1' ||
          event_type(tl->event) == FALLING &&
          sm_shared_enabling_cube[tl->event->signal->index] == '0')
         can_be_shared = FALSE;
    }
    if (can_be_shared) {
      cost = sig_num_not_in_net_term(term, net);
      if (cost < minimal_cost) minimal_cost = cost;
      if (is_shared_stable_term(term, events_set) && cost < st_minimal_cost) 
        st_minimal_cost = cost;
      prev_term = term;
      term = term->next;
    }
    else {
      free_term = term;
      if (prev_term == NULL) candidate_set->terms = term->next;
      else prev_term->next = term->next;
      term = term->next;
      delete_term(free_term);
    }
  }

  if (ddebug) fprintf(dbg,"minimal cost = %d\n", minimal_cost);
  if (ddebug) fprintf(dbg,"st_minimal cost = %d\n", st_minimal_cost);
  if (ddebug) {
    i = 0;
    for (term = candidate_set->terms; term; term = term->next) 
      if (st_minimal_cost < 0x7FFFFFFFL &&
              sig_num_not_in_net_term(term, net) == st_minimal_cost &&
              is_shared_stable_term(term, events_set)) {
          fprintf(dbg,"Solution[%d] : ", i++);
          for (tl = term->llist; tl; tl = tl->next) 
            fprintf(dbg,"%s", tl->event->name);    
          fprintf(dbg,"\n");
        }
  }

  
  /* If there are many solutions with the same cost then 
     select a combinational one  */

  result_term = NULL;
  if (combo) {
    can_be_combinational = TRUE;
    for (i = 0; can_be_combinational && i < array_n(mg_components); i++) {
      component = array_fetch(array_t *, mg_components, i);
      set_to_a_component(component, i);
      set_nr_pr(i);

      result = check_events_included(events_set, component, &subset);
  
      if (result == SUBSET_INCLUDED) {
        for (el = subset; el; el = el->next)
          if (trig_events_num(el->event->nr) > 1) 
             can_be_combinational = FALSE;
      }
      if (subset) free_list(subset);
      restore_to_original_spec();
    }

    /* find a combinational cube if one exists */
    if (can_be_combinational) {
      for (term = candidate_set->terms; term && !result_term; term=term->next) {
        if (st_minimal_cost < 0x7FFFFFFFL &&
                sig_num_not_in_net_term(term, net) == st_minimal_cost &&
                is_shared_stable_term(term, events_set) && 
                check_hazard(term, events_set)) {
          is_all_combinational = TRUE;
          for (i = 0; is_all_combinational && i < array_n(mg_components); i++) {
            component = array_fetch(array_t *, mg_components, i);
            set_to_a_component(component, i);
            set_nr_pr(i);
     
            result = check_events_included(events_set, component, &subset);
  
            if (result == SUBSET_INCLUDED) {
              for (el = subset; el; el = el->next) {
                zi = el->event;
                zj = zi->nr;
                get_all_cause_events(zi, net, term, &cause_events);

                if (ddebug) print_list(dbg, "cause_events: ", cause_events);

                is_combinational = TRUE;
                for (cl = cause_events; cl; cl = cl->next) {
                  if (!(cause(cl->event, zj) || 
                        is_trig_event(cl->event->nr, zj)) ||
                      (is_concur(cl->event->nr, zj) &&
                       !caused_by_trig_event(cl->event->nr, zj))) {

                     if (ddebug) {
                       if (!cause(cl->event, zj)) 
                         fprintf(dbg, "cause_event %s does not cause zj %s\n", 
                                   cl->event->name, zj->name);
                       if (!is_trig_event(cl->event->nr, zj))
                         fprintf(dbg,"cause_event->nr %s does not trigger zj\n",
                                   cl->event->nr->name);
                       if (con_with_some_event(cl->event, zj)) 
                         fprintf(dbg, "cause event con with zj\n");
                    }

                    is_combinational = FALSE;
                  }
                }
                if (!is_combinational) {
                  is_all_combinational = FALSE;
                }
              }
            }
            restore_to_original_spec();
            if (subset) free_list(subset);
          }
          if (is_all_combinational) {
              result_term = term;
              net->type = COMBI;
          }
        }
      }
    }
  }
  
  if (result_term == NULL) 
    for (term = candidate_set->terms; term && !result_term; term = term->next) 
      if (st_minimal_cost < 0x7FFFFFFFL &&
            sig_num_not_in_net_term(term, net) == st_minimal_cost &&
            is_shared_stable_term(term, events_set) && 
            check_hazard(term, events_set)) {
         result_term = term;
       }

  if (result_term == NULL) 
    for (term = candidate_set->terms; term && !result_term; term = term->next) 
      if (st_minimal_cost < 0x7FFFFFFFL &&
            sig_num_not_in_net_term(term, net) == st_minimal_cost &&
            is_shared_stable_term(term, events_set)) {
         result_term = term;
         new_literal_unique(result_term, events_set->event->nr); 
       }

  /* return latest_solution */
  if (result_term && st_minimal_cost < 0x7FFFFFFFL) {
    for (tl = result_term->llist; tl; tl = tl->next)
      new_literal_unique(net->terms, tl->event->nr);
    net->terms->src_list = result_term->src_list;
    result_term->src_list = NULL;
    return TRUE;
  }
  else if (pass == 2 && result_term) {
   printf("\nEXCEPTION : Non-Persistent Context Signals\n");
   fprintf(lg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   if (ddebug) fprintf(dbg, "\nEXCEPTION : Non-Persistent Context Signals\n");

   return FALSE;
  }
  else if (pass == 2) {
    if (!find_reduced_state_graph(SYNTHESIS,SINGLE,Design)) exit(0);
    printf("Internal Error: Can't find a cover\n");
    fprintf(lg, "Internal Error: Can't find a cover\n");

    if (ddebug) fprintf(dbg, "Internal Error: Can't find a cover\n");

    return FALSE;
  }
  else return FALSE;
}

int is_trigger_signal_included(hlogic *net, event_list *events_set)
{
  event_list *el, *tl;

  if (events_set == NULL) return FALSE;
  for (el = events_set; el; el = el->next)
    for (tl = net->terms->llist; tl; tl = tl->next)
      if (el->event->signal == tl->event->signal) return TRUE;

//  printf("trigger signal is not included in zero-stable signal\n");
  return FALSE;
}

int all_one_stable_trigger_signals(hlogic *net, event_list *one_stable_events)
{
  event_list *tl, *el;
  int one_stable;

  for (tl = net->terms->llist; tl; tl = tl->next) {
    one_stable = FALSE;
    for (el = one_stable_events; el; el = el->next)
      if (tl->event->signal == el->event->signal) one_stable = TRUE;
    if (one_stable == FALSE) return FALSE;
  }
  return TRUE;
}

int some_one_stable_cubes(hlogic *all_cubes, event_list *one_stable_events)
{
  event_list *tl, *el;
  int one_stable, one_stable_cube;
  term_struct *t;

  for (t = all_cubes->terms; t; t = t->next) {
    one_stable_cube = TRUE;
    for (tl = t->llist; one_stable_cube && tl; tl = tl->next) {
      one_stable = FALSE;
      for (el = one_stable_events; !one_stable && el; el = el->next)
        if (tl->event->signal == el->event->signal) one_stable = TRUE;
      if (one_stable == FALSE) one_stable_cube = FALSE;
    }
    if (one_stable_cube) return TRUE;
  }
  return FALSE;
}

hlogic *
decomp_find_a_single_cube(hlogic *net, event_list *events_set, 
                           int shared, int combo, FILE *pg, int pass)
{
  int m, n = 0, i, j, always_on_or_off, result, subset_shared, satisfied;
  array_t *component;
  hlogic *sol_cubes, *free_cubes, *off_cubes[256], *zero_cubes[256];
  hlogic *all_cubes=NULL, *cubes, *new_cubes;
  event_list *subset=NULL, *stable_events, *el, *l, *tl;
  event_list *one_stable_events[256];
  event_list *zero_stable_events;
  term *new_cube;
  term *off_c, *c, *new_term;
  char buf[256];
  char *sm_shared_enabling_cube=NULL;
//  timeval t0,t1;
//  double time0, time1;
   
//  gettimeofday(&t0, NULL);
  if (shared) {
    sm_shared_enabling_cube = (char *) malloc(nsignals+1);
    strcpy(sm_shared_enabling_cube, EC_approximated[events_set->event->index]);
    for (el = events_set->next; el; el = el->next)
      for (i = 0; i < nsignals; i++) 
        if (EC_approximated[el->event->index][i] == 'X') 
          sm_shared_enabling_cube[i] = 'X';

    if (ddebug) fprintf(dbg, "Smallest shared enabling cube = %s\n", 
                            sm_shared_enabling_cube);

  }


  if (ddebug) fprintf(dbg, "# of components = %d\n", array_n(mg_components));


  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    set_nr_pr(i);

    result = check_events_included(events_set, component, &subset);

    cubes = NULL;

    if (result == SUBSET_INCLUDED) {

      if (ddebug) print_component(i, component, dbg);
      if (ddebug) fprintf(dbg, "comp %d : type = %d\n", i, result);

      if (subset->next != NULL) subset_shared = TRUE;
      else subset_shared = FALSE;
      cubes = decomp_MG_find_single_cubes(net, subset, subset_shared, 
                                       combo, pg, pass); 
      if (cubes == NULL) return NULL;


      if (ddebug) 
        print_logic_and_src(dbg, "Covering cubes : ", cubes, TRUE, SET_NET);


/*
    print_logic_and_src(stdout,"Covering cubes", cubes, TRUE, SET_NET);
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("Elapsed time = %f\n",time1-time0);
    gettimeofday(&t0,NULL);	
*/

      if (all_cubes == NULL) all_cubes = cubes;
      else {
        free_cubes = all_cubes;
        all_cubes = multiply_two_net_and_src(all_cubes, cubes, TRUE);
        delete_net(free_cubes);
        delete_net(cubes);
      }
/*
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("Elapsed time for multiplication = %f\n",time1-time0);
    gettimeofday(&t0,NULL);	
*/

    }
    restore_to_original_spec();
    if (subset) free_list(subset);
  }

  initial_cube = (int *) malloc(nsignals * sizeof(int));
  for (i = 0; i < nsignals; i++) initial_cube[i] = FALSE;
  for (tl = net->terms->llist; tl; tl = tl->next) 
    initial_cube[tl->event->signal->index] = TRUE;

/*
    print_logic_and_src(stdout,"All cubes for type = 1:", all_cubes, TRUE, 
                        SET_NET);
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("Elapsed time = %f\n",time1-time0);
    gettimeofday(&t0,NULL);	
*/


  if (ddebug) 
    print_logic_and_src(dbg,"All cubes (0) : ", all_cubes, TRUE, SET_NET);


  for (i = 0; i < array_n(mg_components); i++) {
    component = array_fetch(array_t *, mg_components, i);
    set_to_a_component(component, i);
    set_nr_pr(i);

    result = check_events_included(events_set, component, &subset);


    if (result != SUBSET_INCLUDED && ddebug) {
      print_component(i, component, dbg);
      fprintf(dbg, "comp %d : type = %d\n", i, result);
    }


    cubes = NULL;

    if (result == OTHER_EVENTS_INCLUDED) {
      stable_events = find_zero_stable_events(events_set);

      if (ddebug) print_list(dbg, "zero stable events : ", stable_events);


      if (is_trigger_signal_included(net, stable_events)) {
        if (subset) free_list(subset);
        restore_to_original_spec();
        continue;
      }

      if (stable_events == NULL || heuristic1 == FALSE) {
        cubes = find_off_cubes(i, net, pass, pg);
        if (!cubes) cubes = alloc_net(NULL, NULL);
        for (el = stable_events; el; el = el->next) {
          new_term = alloc_term(cubes);
          new_literal(new_term, el->event);
        }

        if (ddebug) print_logic(dbg, "Off cubes : ", cubes, TRUE, SET_NET);

      }
      else {
        cubes = alloc_net(NULL, NULL);
        for (el = stable_events; el; el = el->next) {
          new_term = alloc_term(cubes);
          new_literal(new_term, el->event);
        }
        free_list(stable_events);
      }

      if (!cubes) {
        // printf("ERROR: find_off_cubes is failed\n");

        if (ddebug) fprintf(dbg, "ERROR: find_off_cubes is failed\n");

      } else {
        if (all_cubes == NULL) all_cubes = cubes;
        else {
          free_cubes = all_cubes;
          all_cubes = multiply_two_net_and_src(all_cubes, cubes, TRUE);
          delete_net(free_cubes);
          delete_net(cubes);
        }
      }
    }
    else if (result == NO_EVENTS_INCLUDED) {
      one_stable_events[n] = find_one_stable_events(net->event);

      if (ddebug) print_list(dbg,"one stable events : ", one_stable_events[n]);

      zero_stable_events = find_zero_stable_events(events_set);

      if (ddebug) print_list(dbg, "zero stable events : ", zero_stable_events);


      if (is_trigger_signal_included(net, zero_stable_events)) {
        if (one_stable_events[n]) free_list(one_stable_events[n]);
        if (subset) free_list(subset);
        restore_to_original_spec();
        continue;
      }

      if (zero_stable_events == NULL || heuristic1 == FALSE) {
        off_cubes[n] = find_off_cubes(i, net, pass, pg);
        if (!off_cubes[n]) {
          //printf("ERROR: find_off_cubes is failed\n");

          if (ddebug) fprintf(dbg, "ERROR: find_off_cubes is failed\n");

        }
        if (!off_cubes[n]) off_cubes[n] = alloc_net(NULL, NULL);
        for (el = zero_stable_events; el; el = el->next) {
          new_term = alloc_term(off_cubes[n]);
          new_literal(new_term, el->event);
        }

        if (ddebug) print_logic(dbg,"Off cubes:",off_cubes[n], TRUE, SET_NET);

      }
      else {
        off_cubes[n] = alloc_net(NULL, NULL);
        for (el = zero_stable_events; el; el = el->next) {
          new_term = alloc_term(off_cubes[n]);
          new_literal(new_term, el->event);
        }
        free_list(zero_stable_events);
      }
      n++;
    }

    if (subset) free_list(subset);
    restore_to_original_spec();
  }

/*

    print_logic_and_src(stdout,"All cubes for type 2 and 3: ", 
                        all_cubes, TRUE, SET_NET);
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("Elasped time = %f\n",time1-time0);
*/


  if (ddebug) print_logic_and_src(dbg,"All cubes (1) : ", all_cubes, TRUE, SET_NET);


  sol_cubes = NULL;
  if (n > 0) {
    m = 0;
    for (i = 0; i < n; i++) {
      if (!all_one_stable_trigger_signals(net, one_stable_events[i]) ||
          !some_one_stable_cubes(all_cubes, one_stable_events[i])) {
        free_cubes = all_cubes;
        all_cubes = multiply_two_net_and_src(all_cubes, off_cubes[i], TRUE);
        delete_net(free_cubes);
        delete_net(off_cubes[i]);
      }
      else {
        zero_cubes[m] = off_cubes[i];
        m++;
      }
    }
    n = m;
  }
      
  if (n > 0) {
// printf("n = %d\n", n);
    for (i = 0; i < n; i++) if (zero_cubes[i] == NULL) return NULL;
    sol_cubes = alloc_net(NULL, NULL);
    satisfied = FALSE;
    for (c = all_cubes->terms; c; c = c->next) {
      c->comp = (char *) malloc(n+1);
      for (j = 0; j < n; j++) c->comp[j] = '0';
      c->comp[n] = '\0';
    }
    while (satisfied == FALSE) {
      satisfied = TRUE;
      new_cubes = alloc_net(NULL, NULL);
      for (c = all_cubes->terms; c; c = c->next) {
        always_on_or_off = TRUE;
        for (i = 0; i < n; i++) 
          if (!is_always_on(c, one_stable_events[i]) &&
              !is_always_off(c, zero_cubes[i])) 
            always_on_or_off = FALSE;
        if (always_on_or_off) add_to_net_and_src(sol_cubes, c, TRUE);
        else {
          for (i = 0; i < n; i++) {
            if (c->comp[i] == '1') continue;
            if (!is_always_on(c, one_stable_events[i])) {
              satisfied = FALSE;
              for (off_c = zero_cubes[i]->terms; off_c; off_c = off_c->next) {
                new_cube = alloc_term(NULL);
                new_cube->comp = (char *) malloc(n+1);
                strcpy(new_cube->comp, c->comp);
                new_cube->comp[i] = '1';
                for (l = c->llist; l; l = l->next) 
                  new_literal_unique(new_cube, l->event);
                for (l = off_c->llist; l; l = l->next) 
                  new_literal_unique(new_cube, l->event);
                for (l = c->src_list; l; l = l->next)
                  add_list(&new_cube->src_list, l->event);
                add_term_unique(all_cubes, new_cubes, new_cube, sol_cubes);
              }
            }
          }
        }
      }
      free_cubes = all_cubes;
      all_cubes = new_cubes;
      delete_net(free_cubes);

      if (ddebug) print_logic_and_src(dbg,"refined cubes : ",all_cubes, TRUE, SET_NET);

    }

/*
    print_logic_and_src(stdout, "All cubes after set multiplication: ", 
                         sol_cubes, TRUE, SET_NET);
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("Elasped time = %f\n",time1-time0);
*/


    if (ddebug) print_logic_and_src(dbg, "All cubes (2) : ", sol_cubes, 
                                      TRUE, SET_NET);


    for (i = 0; i < n; i++) if (zero_cubes[i]) delete_net(zero_cubes[i]);
  }
  else sol_cubes = all_cubes;

  if (!shared) {
    result = decomp_find_a_minimal_covering_cube(net, sol_cubes, net->event,
                                         net->event->nr, combo, events_set, pass);
    sprintf(buf, "\nSingle Cube for %s -> %s = ", 
              net->event->name, net->event->nr->name);

    if (ddebug) {
      print_logic(dbg, buf, net, TRUE, SET_NET);
      fprintf(dbg, "\n");
    }

    delete_net(sol_cubes);
  }
  else {
    result = decomp_find_a_minimal_shared_covering_cube(net, sol_cubes, 
                sm_shared_enabling_cube, events_set, net->event, 
                net->event->nr, combo, pass);

    if (ddebug) {
      print_list(dbg, "Single Cube for the events set : ", events_set);
      print_logic_and_src(dbg, " ", net, TRUE, SET_NET);
      fprintf(dbg, "\n");
    }

    delete_net(sol_cubes);
    free(sm_shared_enabling_cube);
  }

  free(initial_cube);

  if (result == TRUE) return net;
  else return NULL;
}

bool heur_syn(char menu,char command, designADT design, char * filename, 
	      cycleADT ****cycles,
	      regionADT *regions, int ninpsig,int ndummy,int ncycles,
	      bool si,int maxsize,bool gatelevel,bool combo, bool *comb)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  FILE *rel=NULL;
  FILE *pg=NULL;
  FILE *cm=NULL;
  int i, j, k, events_num;
  int total_allocated_mem=0, total_freed_mem=0;
  hevent *v;
  stjung_signal *s;
  event_list *triger_events, *tl, *rl;
  event_list *events_set=NULL;
  hlogic *cube=NULL, *set_net=NULL, *reset_net=NULL;
  int nrules=0, shared;
  int *processed;
  int successful;
  int nodes;
  

  printf("Initializing ... ");
  fprintf(lg,"Initializing ... ");
  fflush(stdout);
  fflush(lg);

  if (verbose) {
    if (open_verbose_files(filename, &dbg, &rel, &pg) == FALSE) 
      return FALSE;
    if (ddebug) print_basic_info(cycles, ncycles);
  }

  /* construct data structure */
  for (i=0; i< 10; i++) {
    allocated_mem[i] = 0;
    freed_mem[i] = 0;
  }

  conflict = (int **) malloc(nevents * sizeof (int *));
  for (i = 1; i < nevents; i++) 
    conflict[i] = (int *) malloc(nevents * sizeof (int));
  for (i = 1; i < nevents; i++)
    for (j = 1; j < nevents; j++) conflict[i][j] = FALSE;

  concurrency_table = (int **) malloc(nevents * sizeof (int *));
  for (i = 0; i < nevents; i++) 
    concurrency_table[i] = (int *) malloc(nevents * sizeof (int));
  for (i = 0; i < nevents; i++)
    for (j = 0; j < nevents; j++) 
      concurrency_table[i][j] = FALSE;
  ordered_table = (int **) malloc(nevents * sizeof (int *));
  for (i = 0; i < nevents; i++) 
    ordered_table[i] = (int *) malloc(nevents * sizeof (int));
  for (i = 0; i < nevents; i++)
    for (j = 0; j < nevents; j++) 
      ordered_table[i][j] = FALSE;
  cause_table = (int **) malloc((nevents-ndummy) * sizeof (int *));
  for (i = 0; i < nevents - ndummy; i++) 
    cause_table[i] = (int *) malloc((nevents - ndummy) * sizeof (int));
  for (i = 0; i < nevents - ndummy; i++)
    for (j = 0; j < nevents - ndummy; j++) 
      cause_table[i][j] = FALSE;
  org_cause_table = cause_table;

  processed = (int *) malloc(nevents * sizeof(int));
  EC_exact = (struct cube_list **) malloc(nevents * sizeof(struct cube_list *));
  for (i = 0; i < nevents; i++) EC_exact[i] = NULL;
  h_signals = (stjung_signal **) 
                   malloc(sizeof(stjung_signal *) * nsignals);
  h_events = (hevent **) 
                   malloc(sizeof(hevent *) * nevents);
  for (i = 0; i < nsignals; i++) {
    h_signals[i] = alloc_hsignal_struct();
    h_signals[i]->index = i;
    h_signals[i]->first = signals[i]->event;
    for (j = signals[i]->event; j < nevents; j++)
      if (events[j]->signal == i) h_signals[i]->last = j;
    h_signals[i]->name = (char *) malloc(strlen(signals[i]->name)+1);
    strcpy(h_signals[i]->name, signals[i]->name);
  }
  for (i = 0; i < nevents; i++) {
    h_events[i] = alloc_hevent_struct();
    h_events[i]->signal = h_signals[events[i]->signal];
    h_events[i]->index = i;
    h_events[i]->name = (char *) malloc(strlen(events[i]->event)+1);
    strcpy(h_events[i]->name, events[i]->event);
    if ((i % 2) == 1) h_events[i]->event_type = RISING;
    else h_events[i]->event_type = FALLING;
  }

  /* get the total occurrence of each signal event */
  for (i = 0; i < nsignals; i++) {

    if (ddebug) fprintf(dbg,"Signal's event index = %d\n", signals[i]->event);

    if (i < nsignals -1 )
      events_num = signals[i+1]->event - signals[i]->event;
    else 
      events_num = nevents - signals[i]->event ;
    h_signals[i]->nevents = events_num / 2;
  }


  if (ddebug) 
    for (i = 0; i < nsignals; i++) 
     fprintf(dbg,"%s's events = %d\n",h_signals[i]->name,h_signals[i]->nevents);


  printf("done\n");
  fprintf(lg,"done\n");
  fflush(stdout);
  fflush(lg);
    
  copy_drop_and_ruletype();
 
  /* check input specification */ 
  printf("Checking non free-choice events...");
  fflush(lg);
  if (!marked_graph && check_free_choice() == FALSE) {
    printf("EXCEPTION: The specification has non-free choice behavior\n");
    return FALSE;
  }  
  printf("done\n");
  fflush(lg);

  /* find marked graph components */
  if (!marked_graph) {
    printf("Finding MG components ... ");
    fflush(stdout);
   
    if (find_MG_components_using_simple_cycles(&cm, filename)) {
      check_reductions(cm);

      if (ddebug) print_component_graph(menu, command, design, filename, dbg);
      if (ddebug) print_components(dbg);

    }
    else {
      if (find_MG_components(&cm, filename) == FALSE) return FALSE;
      check_reductions(cm);

      if (ddebug) print_component_graph(menu, command, design, filename, dbg);
      if (ddebug) print_components(dbg);

      if (selected_comp) {
        remove_unselected_comp();

        if (ddebug) {
          fprintf(dbg, "Selected comp : ");
          print_components(dbg);
        }

      }
    }
    fclose(cm);
    printf("done\n");
    fflush(stdout);

    find_MG_comp_data(menu,command,design);

  }

  /* examine the relations between signal events */
  if (verbose) {
    printf("Finding relations and storing to %s.rel ... ", filename);
    fprintf(lg,"Finding relations and storing to %s.rel ... ", filename);
  }
  else {
    printf("Finding relations ... ");
    fprintf(lg,"Finding relations ... ");
  }
  fflush(stdout);
  fflush(lg);

  if (marked_graph) {
    find_MG_timed_concurrent_events(cycles, ncycles, rel);
    find_MG_reverse_events(cycles, ncycles, rel);
    find_MG_timed_cause_events(cycles, ncycles, rel);
  }
  else {
    find_FC_timed_concurrent_events(cycles, ncycles, rel);
    find_FC_conflict_events(rel);
    if (si_mode) find_FC_untimed_reverse_events(rel);
    else find_FC_reverse_events(cycles, ncycles, rel);
    if (si_mode) find_FC_untimed_cause_events(rel);
    else find_FC_timed_cause_events(cycles, ncycles, rel);
  }
  printf("done\n");
  fflush(stdout);
  fprintf(lg,"done\n");
  fflush(lg);
  if (verbose) fclose(rel);
  
  /* check Persistence Violation! */
  if (marked_graph) {
    for (i = 1; i < nevents - ndummy; i++) {
      if (events[i]->dropped || h_events[i]->signal->index < ninpsig) continue;
      triger_events = get_trig_events(h_events[i]);
      for (tl = triger_events; tl; tl = tl->next) 
        if (is_concur(tl->event->nr, h_events[i])) {
          printf("\nEXCEPTION : Persistency Violation!\n");
          fprintf(lg, "\nEXCEPTION : Persistency Violation!\n");
          if (verbose) fprintf(lg, "%s is not persistent\n", tl->event->name);
          return FALSE;
        }
    }
  }
  else {
    for (i = 1; i < nevents - ndummy; i++) {
      if (events[i]->dropped || h_events[i]->signal->index < ninpsig) continue;
      triger_events = get_trig_events(h_events[i]);
      for (tl = triger_events; tl; tl = tl->next) 
        for (rl = tl->event->nr_list; rl; rl = rl->next)
          if (is_concur(rl->event, h_events[i])) {
            printf("\nEXCEPTION : Persistency Violation!\n");
            fprintf(lg, "\nEXCEPTION : Persistency Violation!\n");
            if (verbose) fprintf(lg, "%s is not persistent\n", rl->event->name);
            return FALSE;
          }
    }
  }

  /* synthesize the set and reset network for each output signal */
  if (verbose) {
    printf("Finding precedence graphs and storing graphs to %s.pg ... ", 
               filename);
    fprintf(lg, "Finding precedence graphs and storing graphs to %s.pg ... ",
               filename);
  }
  else {
    printf("Finding precedence graphs ... ");
    fprintf(lg,"Finding precedence graphs ... ");
  }
  fflush(stdout);
  fflush(lg);

  find_smallest_enabling_cubes();

  successful = TRUE;
  for (i = ninpsig; i < nsignals; i++) {
    s = h_signals[i];
    s->reset = s->set = NULL;
    for (j = s->first; j <= s->last; j++) {
      processed[j] = FALSE;
    }

    for (j = s->first; j <= s->last; j++) {
      v = h_events[j];
      if (processed[j] || events[j]->dropped) continue;
      processed[j] = TRUE; 
     
      events_set = NULL;

      shared = FALSE;
      add_list(&events_set, h_events[j]);
      for (k = j+1; k <= s->last; k++) {
        if (events[k]->dropped == FALSE &&
            event_type(h_events[j]) == event_type(h_events[k]) &&
            may_enabling_cube_intersect(j,k) && is_same_trig_sigs(j,k) &&
            is_enabling_cube_intersect(j,k))
        {
          processed[k] = TRUE;
          add_list(&events_set, h_events[k]);
          shared = TRUE;
        }
      }

//      print_list(stdout, "Synthesize the events : ", events_set);

      if (ddebug) print_list(dbg, "Synthesize the events : ", events_set);

      fflush(lg);
      fflush(stdout);

      set_net = reset_net = NULL;
      if (event_type(v) == RISING) {
        set_net = alloc_net(NULL, v);
        set_net->type = STORAGE;
        if (marked_graph) 
          cube = MG_find_a_single_cube(set_net,events_set,shared,combo,pg);
        else {
          cube=decomp_find_a_single_cube(set_net,events_set,shared,combo,pg,1);
          if (!cube) {
            delete_term(set_net->terms); 
            set_net->terms = NULL;
            cube=decomp_find_a_single_cube(set_net,events_set,shared, 
                                                    combo, pg, 2);
          }
        }
        if (set_net->type == STORAGE && combo) 
          find_a_multi_cube_comb_net(set_net, events_set, pg);
      }
      else {
        reset_net = alloc_net(NULL, v);
        reset_net->type = STORAGE;
	if (marked_graph) 
          cube=MG_find_a_single_cube(reset_net,events_set,shared,combo,pg);
        else {
          cube=decomp_find_a_single_cube(reset_net, events_set,shared, 
                                         combo,pg,1);
          if (!cube) {
             delete_term(reset_net->terms); 
             reset_net->terms = NULL;
             cube=decomp_find_a_single_cube(reset_net,events_set,
                                               shared,combo,pg,2);
          }
        }
	if (reset_net->type == STORAGE && combo) 
          find_a_multi_cube_comb_net(reset_net, events_set, pg);
      }
      add_to_logic_list(s, set_net, reset_net);
      if (cube == NULL) successful = FALSE;
      if (events_set) free_list(events_set);
    }
  }
  printf("done\n");
  fprintf(lg,"done\n");
  if (verbose) fclose(pg);
  fflush(stdout);
  fflush(lg);
  if (successful == FALSE) return FALSE;

  strcpy(outname,filename);
  strcat(outname,".prs");
  printf("Finding production rules and storing to:  %s\n",outname);
  fflush(stdout);
  fprintf(lg,"Finding production rules and storing to:  %s\n",outname);
  fflush(lg);
  fp=Fopen(outname,"w"); 
  gen_prs_file(command, regions, fp, combo, ninpsig, comb);

  for (i = 1; i < nevents; i++) {
    free(h_events[i]->name);
    free_list(h_events[i]->nr_list);
    free_list(h_events[i]->pr_list);
    free_cube_list(EC_exact[i]);
    free(conflict[i]);
    free(concurrency_table[i]);
    free(ordered_table[i]);
    free(org_ruletype[i]);
  }

  for (i = 0; i < (nevents - ndummy); i++) {
    free(EC_approximated[i]);
    free(cause_table[i]);
  }

  for (i = 0; i < nsignals; i++) {
    free(h_signals[i]->name);
    if (h_signals[i]->set) free_logic_list(h_signals[i]->set);
    if (h_signals[i]->reset) free_logic_list(h_signals[i]->reset);
  }

  free(conflict);
  free(cause_table);
  free(ordered_table);
  free(concurrency_table);
  free(EC_exact);
  free(EC_approximated);
  free(h_events);
  free(h_signals);
  free(processed);
  free(org_dropped);
  free(org_ruletype);

  if (selected_comp) free(selected_comp);

  if (mg_components) {
    for (i = 0; i < array_n(mg_components); i++) {
      free(comp_nr[i]);
      free(comp_pr[i]);
      array_free(array_fetch(array_t *, mg_components, i));
      free(mg_comps[i]->dropped);
      for (j = 1; j < nevents; j++) free(mg_comps[i]->ruletype[j]); 
      free(mg_comps[i]->ruletype);
/*
      if (mg_comps[i]->cause_table != org_cause_table) {
        for (j = 0; j < nevents - ndummy; j++) 
          if (mg_comps[i]->cause_table[j]) free(mg_comps[i]->cause_table[j]);
        free(mg_comps[i]->cause_table);
      }
*/
      free(mg_comps[i]);
    }
    array_free(mg_components);
    free(comp_nr);
    free(comp_pr);
  }


  if (ddebug) {
    fprintf(dbg, "size of literal = %d term = %d logic = %d\n", 
             sizeof(literal), sizeof(term), sizeof(hlogic));
    for (i=0; i< 10; i++) {
      fprintf(dbg, "%s : allocated = %d freed = %d lost = %d\n", alloc_type[i], 
              allocated_mem[i], freed_mem[i], allocated_mem[i] - freed_mem[i]);
      total_allocated_mem += allocated_mem[i];
      total_freed_mem += freed_mem[i];
    }
    fprintf(dbg, "Allocated Memory = %d Freed Memory = %d Lost Memory = %d\n", 
            total_allocated_mem, total_freed_mem, 
            total_allocated_mem - total_freed_mem);

    nodes = 0;
    nrules = 0;
    for (i = 1; i < nevents; i++) if (!events[i]->dropped) nodes++;
    for (i = 1; i < nevents; i++)
      for (j = 1; j < nevents; j++) 
        if (rules[i][j]->ruletype != 0) nrules++;
    if (ddebug) fprintf(dbg, "signals = %d nodes = %d arcs = %d\n", 
                          nsignals, nodes, nrules);
    if (ddebug) fprintf(dbg, "pgraphs= %d avg_nds= %d avg_edgs = %d\n", 
            pgraphs, (int) floor(((float) pgraph_nodes / (float) pgraphs)+0.5), 
            (int) floor(((float) pgraph_edges / (float) pgraphs)+0.5)); 
  }

  if (ddebug) fclose(dbg);

  return TRUE;
}

/*****************************************************************************/
/* Heuristic timed circuit synthesis based on Sung Tae Jung's Direct         */
/*   synthesis method                                                        */
/*****************************************************************************/

bool heuristic_synthesis(char menu,char command,designADT design)
{
  bool *comb;
  int i, j;

  rules = design->rules;
  signals = design->signals;
  events = design->events;
  nsignals = design->nsignals;
  nevents = design->nevents;
  ndummy = design->ndummy;
  verbose = design->verbose;
  gatelevel = design->gatelevel;
  si_mode = design->si | design->untimed; 
  cycles = design->cycles;
  ncycles = design->ncycles;
  noparg = design->noparg;
  Design = design;
  if (!verbose) ddebug = FALSE;

  marked_graph = TRUE;
  for (i = 1; i < nevents; i++) 
    for (j = 1; j < nevents; j++) 
      if (rules[i][j]->conflict == TRUE) 
        marked_graph = FALSE;
   
  if (marked_graph) 
    printf("%s is a marked graph\n", design->filename);
  else 
    printf("%s is not a marked graph\n", design->filename);

  if (!design->untimed && marked_graph) 
     if (!find_redundant_rules(menu,command,design)) return FALSE;

  comb=(bool*)GetBlock(2*nsignals*sizeof(bool));
  for (i=0;i<2*nsignals;i++) comb[i]=FALSE;

  if (!(design->status & STOREDPRS)) {
    if (!heur_syn(menu, command, design, design->filename,design->cycles,
                  design->regions, design->ninpsig, design->ndummy,
                  design->ncycles, (design->si|design->untimed), 
                  design->maxsize, design->gatelevel, design->combo, comb)) 
    {
      return FALSE;
    }
    design->status |= FOUNDSTATES;
    design->status |= FOUNDRSG;
    design->status |= FOUNDREG;
    design->status |= FOUNDCONF;
    design->status |= FOUNDCOVER;
    design->status |= STOREDPRS;
  }
  design->comb=comb;
  return(TRUE);
}
