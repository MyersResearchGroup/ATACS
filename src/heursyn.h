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
/* heursyn.h                                                                 */
/*****************************************************************************/

#ifndef _heursyn_h
#define _heursyn_h

#include "struct.h"

/*****************************************************************************/
/* Heuristic timed circuit synthesis based on Sung Tae Jung's Direct         */
/*   synthesis method                                                        */
/*****************************************************************************/

typedef struct event_list event_list;
typedef struct cycle_list cycle_list;
typedef struct after_list after_list;
typedef struct event_list literal;
typedef struct squeue hqueue;
typedef struct slogic hlogic;
typedef struct sevent hevent;
typedef struct scomp hcomp;
typedef struct edge edge;
typedef struct set_of_comp_set set_of_comp_set;
typedef struct comp_set comp_set;
typedef struct term term;
typedef struct term term_struct;
typedef struct logic_list logic_list;

typedef struct array_struct array_t;
typedef struct comb_info comb_info;
typedef struct xnode xnode;
typedef struct xnode_list xnode_list;
typedef struct pnode pnode;


#define IS_SAME_TYPE(v,t) \
  (((v)->signal == (t)->signal) && \
  ((v)->event_type == (t)->event_type))

#define IS_REV_TYPE(v,t) \
  (((v)->signal == (t)->signal) && \
  ((v)->event_type != (t)->event_type))

#define IS_SAME_SIGNAL(a,b) \
  ((a)->signal == (b)->signal) 

#define num_of_signals(term)  \
    (((term)->nsignals > -1) ? (term)->nsignals : count_num_of_signals((term)))

#define non_redundant_rule(i, j) ((ruletype[i][j] == (TRIGGER | SPECIFIED)))

#define cause(a, b) ((cause_table[(a)->index][(b)->index]))
#define is_concur(a, b) ((concurrency_table[(a)->index][(b)->index]))
#define event_type(t) ((t)->event_type)

#define get_xlist(t) ((t)->xlist)

#define CYCLE      5
#define FALSE      0
#define TRUE       1

#define CHOICE			1
#define MERGE			2
#define CHOICE_AND_MERGE	4

struct cn_vertex {
  struct cn_edge *in_edges;
  struct cn_edge *out_edges;
  struct cn_vertex *next;
  int active;
  int unprocessed;
  int selected;
  int id;
  int type;
  struct cn_vertex *trail;
};

struct cn_edge {
  int id;
  int type;
  int active;
  int selected;
  int unprocessed;
  struct sevent *event;
  struct cn_vertex *head;
  struct cn_vertex *tail;
  struct cn_edge *next_in;
  struct cn_edge *next_out;
  struct cn_edge *next;
};

struct cn_edge_list {
  struct cn_edge *edge;
  struct cn_edge_list *next;
};

struct cn_vertex_list {
  struct cn_vertex *vertex;
  struct cn_vertex_list *next;
};

struct cn_cycle {
  int id;
  int essential;
  struct cn_vertex_list *vertexes;
  struct cn_edge_list *edges;
  struct cn_cycle *next;
};

struct cn_graph {
  struct cn_vertex *vertexes;
  struct cn_edge *edges;
  struct cn_vertex *path_start;
  struct cn_cycle *cycles;
};

typedef enum logic_type_enum { 
    STORAGE, 
    COMBI
} logic_type_enum;

typedef enum net_type {
    SET_NET , 
    RESET_NET 
} net_type;

typedef enum event_enum {
    RISING,
    FALLING,
} event_enum;

typedef struct mg_comp {
    int *nr;
    int *pr;
    int *dropped;
    int **ruletype;
    int **conflict;
    int **cause_table;
} mg_comp_struct;

typedef struct signal {
    int index;
    int nevents;    /* total number of events of this signal */
    int first;
    int last;
    struct logic_list *set;
    struct logic_list *reset;
    int setnet_type;
    int resetnet_type;
    char *name;          /* name of signal: a, b, etc.           */
} stjung_signal;

typedef struct sevent {
    struct sevent *nr;   /* next reverse events */
    struct sevent *pr;   /* previous reverse events */
    struct event_list *nr_list;   /* next reverse events */
    struct event_list *pr_list;   /* previous reverse events */
    
    event_enum event_type;
    struct signal *signal;
    int selected;
    int visited;
    int reachable;
    int index;
    int weight;
    int removed;
    int not_in_allocation;
    int unprocessed;
    int comp_num;
    int covered;
    int subset;
    int processed;
    struct cn_vertex *dest;
    struct cn_vertex *cn_choice_vertex;
    struct cn_vertex *cn_merge_vertex;
    struct xnode *xlist;
    struct logic_list *extra;
    struct event_list *c_events;
    struct event_list *seqlist;
    struct event_list *conlist;
    struct comb_info *comb_infos;
    char *name;                      /* name of the vertex */
};

struct term {
  int nsignals;
  char *comp;
  struct event_list *llist;
  struct event_list *src_list;
  struct term *next;
};

struct squeue {
  struct sevent *event;
  struct squeue *next;
};

struct scomp {
  int comp;
  struct scomp *next;
};

struct comp_set {
  struct scomp *comps;
  struct comp_set *next;
};

struct set_of_comp_set {
  struct comp_set *sets;
};

struct slogic {
  struct sevent *event;
  event_list *events_set;
  struct term *terms;
  int type;
  int shared;
};

struct logic_list {
  struct sevent *events;
  struct slogic *net;
  struct logic_list *next;
};

struct cube_list {
  char *cube;
  struct cube_list *next;
};

struct event_list {
  struct sevent *event;
  struct event_list *next;
};

struct xnode_list {
  struct xnode *xnode;
  struct xnode_list *next;
};

struct xnode {
  struct sevent *event;
  struct sevent *rev_events;
  struct slogic *extra;
  int xclass;
  int cycle;
  struct xnode *next;
};

struct array_struct {
    char *space;
    int num;		/* number of array elements */
    int n_size;		/* size of 'data' array (in objects) */
    int obj_size;	/* size of each array object */
};

#define array_alloc(type, number)		\
    array_do_alloc(sizeof(type), number)

#define array_insert(type, a, i, datum)		\
    (array_i = (i),				\
      array_i < 0 ? array_abort(0) : 0,		\
      sizeof(type) != (a)->obj_size ? array_abort(1) : 0,\
      array_i >= (a)->n_size ? array_resize(a, array_i + 1) : 0,\
      array_i >= (a)->num ? (a)->num = array_i + 1 : 0,\
      *((type *) ((a)->space + array_i * (a)->obj_size)) = datum)

#define array_fetch(type, a, i)			\
    (array_i = (i),				\
      (array_i < 0 || array_i >= (a)->num) ? array_abort(0) : 0,\
      *((type *) ((a)->space + array_i * (a)->obj_size)))

#define array_fetch_p(type, a, i)                       \
    (array_i = (i),                             \
      (array_i < 0 || array_i >= (a)->num) ? array_abort(0) : 0,\
      ((type *) ((a)->space + array_i * (a)->obj_size)))

#define array_insert_last(type, array, datum)	\
    array_insert(type, array, (array)->num, datum)

#define array_fetch_last(type, array)		\
    array_fetch(type, array, ((array)->num)-1)

#define array_n(array)				\
    (array)->num

#define array_data(type, array)			\
    (type *) array_do_data(array)

bool heuristic_synthesis(char menu,char command,designADT design);

#endif  /* heursyn.h */
