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
/* struct.h                                                                  */
/*****************************************************************************/

#ifndef _struct_h
#define _struct_h

#include <cstdio>
#include <iostream>
#include <cstring>
#include "misclib.h"
#include "ly.h"
#include <inttypes.h>
//#include "lhpnrsg.h"

#undef MAXINT
#include <set>

#ifdef DLONG
#include <bdduser.h>
#else
#include "cudd/cuddObj.hh"
#endif

#include "CPdf.h"
#include "markov_types.h"

//extern int gettimeofday(struct timeval *tv, struct timezone *tz);

//#define free nop

using namespace std;

/*****************************************************************************/
/* Miscellaneous definitions.                                                */
/*****************************************************************************/

#define   INFIN     2147483647
#define   MAXSIGNALS   1000
// #define   MAXTOKEN     1000
#define   MAXTOKEN     2000
#define   MAX_REGIONS  1024
//#define   MAX_REGIONS  131072
#define   LOG_MAX_REGIONS  10
//#define   LOG_MAX_REGIONS  17
#define   MAXARG       50
#define   NaN          2147483647-1
#define   PRIME        7919
#define   MAXSTACKSIZE 4
#define   MINGATEDELAY 0
#define   MAXGATEDELAY INFIN
#define   NUMCYCLES    3
#define   INIT_TOL     0.01
#define   MAXSIMTIME   100
#define   ITERATIONDEF 30

/*****************************************************************************/
/* Definitions used for ruletypes.                                           */
/*****************************************************************************/

#define   NORULE       0
#define   SPECIFIED    1
#define   DISABLING    2
#define   ASSUMPTION   4
#define   TRIGGER      8
#define   PERSISTENCE  16
#define   ORDERING     32
#define   NONPERS      64
#define   CONTEXT      128
#define   REDUNDANT    256

/*****************************************************************************/
/* Definitions used for mergetypes.                                          */
/*****************************************************************************/

#define   NOMERGE      0
#define   DISJMERGE    1
#define   CONJMERGE    2

/*****************************************************************************/
/* Definitions used for conflict types.                                      */
/*****************************************************************************/

#define   NOCONFLICT   0
#define   ICONFLICT    1
#define   OCONFLICT    2

/*****************************************************************************/
/* Signals in a Specification                                                */
/*****************************************************************************/

typedef struct signal_struct {
  char * name;
  bool is_level;
  int event;
  int riselower;
  int riseupper;
  int falllower;
  int fallupper;
  int maxoccurrence;
} *signalADT;

/*****************************************************************************/
/* Integers in a Specification                                                */
/*****************************************************************************/

typedef struct variable_struct {
  char *name;
  int lInitVal;
  int uInitVal;
  int var_type;
} *variableADT;


/*****************************************************************************/
/* integer expression parse tree class.                                      */
/*****************************************************************************/
class exprsn;

/*****************************************************************************/
/* Linked list of inequalities for enabling conditions and invariants        */
/*****************************************************************************/

typedef struct ineq_tag {
  intptr_t place;
  // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
  // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS* 
  int type;
  int uconstant; //for upper bound rate assignments
  int constant;
  char *expr;
  exprsn *tree;
  int signal;
  intptr_t transition;
  struct ineq_tag *next;
} *ineqADT;

/*****************************************************************************/
/* This piece used to be only for rules                                      */
/*   but is now used in event                                                */
/*****************************************************************************/


typedef struct bool_exp {
  char * product;
  struct bool_exp *next;
} *level_exp;

/*****************************************************************************/
/* Events in Timed Event-Rule Structure                                      */
/*     First event is always reset.                                          */
/*****************************************************************************/

typedef struct event_struct {
  char * event;
  bool dropped;
  bool immediate;
  bool failtrans;
  bool nondisabling;
  int color;
  int signal;
  int lower;
  int upper;
  int process;
  int type;
  char * data;
  char * hsl;
  char *transRate;
  char *delayExpr;
  exprsn *delayExprTree;
  char *priorityExpr;
  int rate;
  int lrate;
  int urate;
  int lrange;
  int urange;
  int linitrate;
  int uinitrate;
  level_exp SOP;
  exprsn *EXP;
  ineqADT inequalities;
} *eventADT;

ostream& operator<<(ostream&out,  eventADT event ); // Output an event


/*****************************************************************************/
/* Set of Merges                                                             */
/*     Linked list of merges.                                                */
/*     Mergetype links occurrences of events that occur multiple times in a  */
/*       cycle, and it is 1 if the events occur at different locations in    */
/*       the specification or 2 if they occur at the same location. (???)    */
/*****************************************************************************/

typedef struct merge_tag {
  int mevent;
  int mergetype;
  struct merge_tag *link;
} *mergeADT;

/*****************************************************************************/
/* Rules and Conflicts in Timed Event-Rule Structure                         */
/*     Includes reset rules.                                                 */
/*     Ruletype as defined above.                                            */
/*****************************************************************************/


typedef struct rule_struct {
  bool concur;
  bool markedConcur;
  int epsilon;
  int lower;
  int upper;
  int weight;
  int plower;
  int pupper;
  int predtype; /* 0 [pl,pu]; 1 [pl,pu); 2 (pl,pu]; 3 (pl,pu) */
  int ruletype; 
  int marking;
  int maximum_sep;
  bool reachable;
  int conflict;
  level_exp level;
  level_exp POS;
  char * expr;
  CPdf *dist;
  double rate;
  ineqADT inequalities;
} *ruleADT;

ostream& operator<<(ostream&out,  ruleADT rule  ); // Output a rule.


typedef struct rule_info{
  int enabled_event;
  int enabling_event;
  int exp_enabled;
  int confl_removed;
  int clocknum;
  bool fired;
} rule_infoADT;

typedef struct anti {
  int event;
  struct anti *next;
} *anti_causal_list;

typedef struct context{
  int event;
  struct context *next;
} *context_list;

typedef char *exp_enabledADT;
typedef char *confl_removedADT;
typedef char *firedADT;
typedef int *clocknumADT;
typedef context_list *rule_contextADT;

typedef struct path_list{
  int event;
  struct path_list *next;
} path_listADT;

typedef struct rule_info_list{
  int num_rules;
  rule_infoADT **list;
  path_listADT *path_list;
} rule_info_listADT; 

/*****************************************************************************/
/* Cycles in Event-Rule System                                               */
/*****************************************************************************/

typedef struct cycle_struct {
  int reachable;
  bool rule;
  int delay;
  int Ui;
  int Uj;
} *cycleADT;      

/*****************************************************************************/
/* Time Difference Bound                                                     */
/*****************************************************************************/

typedef struct bound_struct {
  int L;
  int U;
  int Lbest;
  int Ubest;
} *boundADT;

/*****************************************************************************/
/* Key to Markings                                                           */
/*     Link position in marking to rule of the form <enabling,enabled,...>.  */
/*****************************************************************************/

typedef struct markkey_struct {
  int enabling;
  int enabled;
  int epsilon;
} *markkeyADT;

/*****************************************************************************/
/* Marking                                                                   */
/*     Marking and state associated with the marking.                        */
/*****************************************************************************/

typedef struct marking_struct {
  char * marking;
  char * enablings;
  char * state;
  char * up;
  char * down;
  unsigned int ref_count;
} *markingADT;

/*****************************************************************************/
/* Set of Markings                                                           */
/*     Linked list of markings.                                              */
/*****************************************************************************/

typedef struct marking_tag {
  markingADT marking;
  char * firelist;
  struct marking_tag *link;
} *markingsADT;



/* Moved here from orbits.h so that the state struct can use them. */

typedef int **clocksADT;
typedef int **processADT;

typedef struct clockkey{
  int enabled;
  int enabling;
  int eventk_num;
  int causal;
  anti_causal_list anti;
  int lrate;
  int urate;
} *clockkeyADT;

typedef struct processkey{
  int eventnum;
  int root;
  int instance;
  anti_causal_list anti;
} *processkeyADT;

typedef struct constraint{
  int event1;
  int event2;
  int separation;
  struct constraint *next;
} *constraintADT;

/* t(clock1) - t(clock2) = separation */
typedef struct depend_list{
  int clock1;
  int clock2;
  int separation;
  int conflict;
}*depend_listADT;



typedef struct clocklist{
  clockkeyADT clockkey;
  clocksADT clocks;
  int clocknum;
  int clocksize;
  int nrules;
  exp_enabledADT exp_enabled;
  struct clocklist *next;
} *clocklistADT;

typedef struct ver_clocklist{
  clockkeyADT spec_clockkey;
  clocksADT spec_clocks;
  int spec_clocknum;
  int spec_nrules;
  clockkeyADT imp_clockkey;
  clocksADT imp_clocks;
  int imp_clocknum;
  int imp_nrules;  
  struct ver_clocklist *next; 
}*ver_clocklistADT;

/*****************************************************************************/
/* States                                                                    */
/*     For each signal bits[] is: 0 if signal is low, R if signal is low     */
/*       but enabled to rise, 1 if signal is high, F if signal is high but   */
/*       enabled to fall.                                                    */
/*****************************************************************************/

typedef struct state_tag {
  char * state;
  char * marking;
  char * enablings;
  char * colorvec;    // vector of colors, one for each node
  char * evalvec;     // vector of boolean evaluations, one for each node
  int number;
  int highlight;
  int color;
  bool visited;
  double probability;
  int num_clocks;
  struct state_tag *link;
  struct state_list_tag *pred;
  struct state_list_tag *succ;
  clocklistADT clocklist;
} *stateADT;

typedef struct state_list_tag {
  stateADT stateptr;
  int enabling;
  int enabled;
  int iteration;
  char * colorvec;     // vector of colors, one for each node
  int color;
  struct state_list_tag *next;
  double probability;
} *statelistADT;


typedef struct ver_state{
  char * state;
  char * marking;
  char * enablings;
  char * imp_state;
  char * imp_marking;
  char * imp_enablings;
  struct ver_state *next;
  int number;
  ver_clocklistADT clocklist;
} *ver_stateADT;

/*****************************************************************************/
/* BDD state table junk.                                                     */
/*****************************************************************************/
#ifdef DLONG
typedef struct cov_node{
  struct cov_node *next;
  bdd  C;
  bdd NOTC;
  char **psiids;
  bdd *psilits;
} cov_node;

typedef struct coverlist{
  struct cov_node *head;
  int num_covers;
} coverlist;

typedef struct env_struct {
  char  **ids;
  bdd  *lits;
  char  **psiids;
  coverlist covers;
  int nlits;
  int gamma;
  int phi;
}env;

typedef struct bdd_struct{
  bdd_manager bddm;
  bdd Reg,S,N,M,*L,*R,*C,*SL;
  bdd *lits,*newlits,*rows,*cols,*bdd_mark,*links;
  env e;
  int marks,lks,nsigs,nlog,nrules;
  int oldlinksize,linksize,oldmaxlinks,maxlinks;
} *bddADT;
#else
//CUDD
/* carrier for a single BDD */
class bdd_node{
public:
  bdd_node(BDD f){next = NULL;data = f;};
  bdd_node *next;
  int size;
  BDD data;
};

/* linked list of BDDs */
class bdd_list{
public:
  bdd_list(){head = NULL;tail = NULL;count = 0;};
  ~bdd_list(){
    while (head){
      bdd_node* temp = head->next;
      delete head;
      head = temp;
    }
  }
  /* queue style insertion */
  void insert(BDD f){
    bdd_node *temp = new bdd_node(f);
    if (tail)
      tail->next = temp;
    else
      head = temp;
    temp->size = 0;
    tail = temp;
    count++;
  }
  /* queue style insertion */
  void insert(BDD f, int cubes){
    bdd_node *temp = new bdd_node(f);
    if (tail)
      tail->next = temp;
    else
      head = temp;
    temp->size = cubes;
    tail = temp;
    count++;
  }
  void wipeout(){
    while (head){
      bdd_node* temp = head->next;
      delete head;
      head = temp;
    }
    tail = head;
    count = 0;
  }
  int count;
  bdd_node *head,*tail;
};

/* carrier for a single ADD */
class add_node{
public:
  add_node(ADD f,int num_clocks){
    next = prev = NULL; 
    data = f;
    clocks = num_clocks;
  };
  add_node *next,*prev;
  ADD data;
  int clocks;
};

/* linked list of ADDs */
class add_list{
public:
  add_list(){head = NULL;tail = NULL;next=NULL;count = 0;};
  ~add_list(){
    while (head){
      add_node* temp = head->next;
      delete head;
      head = temp;
    }
  }
  /* queue style insertion */
  void insert(ADD f,int clocks){
    add_node *temp = new add_node(f,clocks);
    if (tail)
      tail->next = temp;
    else
      head = temp;
    temp->prev = tail;
    tail = temp;
    count++;
  }
  /* stack style insertion */
  void push(ADD f,int clocks){
    add_node *temp = new add_node(f,clocks);
    if (head)
      head->prev = temp;
    else
      tail = temp;
    temp->next = head;
    head = temp;
    count++;
  }
  void remove(add_node *zombie){
    if (head == zombie)
      head = zombie->next;
    else
      zombie->prev->next = zombie->next;
    if (tail == zombie)
      tail = zombie->prev;
    else
      zombie->next->prev = zombie->prev;
    delete zombie;
  }
  int count;
  add_node *head,*tail;
  add_list *next;
};

/* loop bounds for variable groupings. */
class bdd_set{
public:
  bdd_set(){start = 0;stop = 0; step = 0;};
  int start,stop,step;
};

/* mask list */
class mask_node{
public:
  mask_node(int* newmask, int count){
    //printf("inserting mask %x\n",(unsigned int)newmask);
    mask = newmask;
    cubes = count;
    next = NULL;
    //printf("inserting mask %x\n",(unsigned int)mask);
  }
  ~mask_node(){
    //printf("deleting mask %x\n",(unsigned int)mask);
    delete[] mask;
  }
  int* mask;
  int cubes;
  mask_node *next;
};

class mask_list{
public:
  mask_list(){
    head = tail = NULL;
  }
  ~mask_list(){
    while (head){
      mask_node *temp = head->next;
      delete head;
      head = temp;
    }
  }
  void print(int n){
    mask_node *temp = head;
    while (temp){
      int first = 1;
      printf("%d: ",temp->cubes);
      for (int i = 0;i<n;i++)
	if (temp->mask[i] ==1)
	  printf("+");
	else if (temp->mask[i] ==2){
	  if (first){
	    printf("+");
	    first=0;
	  }
	  else 
	    printf("|+");
	}
	else
	  printf("-");
      printf("\n");
      temp = temp->next;
    }
  }
  void insert(int* mask, int cubes){
    mask_node *temp = new mask_node(mask,cubes);
    if (tail)
      tail->next = temp;
    else
      head = temp;
    tail = temp;
  }
  mask_node *head, *tail;
};

/* all-in-one bundle of BDD junk. */
class bdd_data{
public:
  Cudd *mgr;
  ADD M,mvars,*row,*col;
  add_list *front,*back;
  BDD S,N,vars,pvars,invars,pinvars,outvars,poutvars,cvars;
  BDD *ESplus,*ESminus,*QSplus,*QSminus,*CCplus,*CCminus,*SSplus,
    *SSminus,*MAPplus,*MAPminus;
  int *CCpsize,*CCmsize,*MAPpsize,*MAPmsize;
  BDDvector *v,*pv;
  bdd_list *ERplus,*ERminus,*VCplus,*VCminus,covers,cubes;
  int nsigs,ninpsigs,nrules;
  int *sigmask,*primemask,*nmask,*nomask,*covmask;
  int n_cubes,max_cubes;
  mask_list *covmasks;
  bool hasCSC;
  char * filename;
  signalADT *signals;
  bdd_set sigs,rules,rows,cols,covs,unused;
  int lastvar,cubestep;
  unsigned int maxdead;
  /* old stuff I ain't got rid of yet. */
  //BDD Reg,*lits,*newlits,*rows,*cols,*bdd_mark,*links,M,*L,*R,*C,*SL;
  //int marks,lks,nlog,nrules;
  int oldlinksize,linksize,oldmaxlinks,maxlinks;
};

typedef bdd_data *bddADT;
#endif  

/*****************************************************************************/
/* Context Signal Table                                                      */
/*****************************************************************************/

typedef struct context_tag {
  char * state;
  char * solutions;
  struct context_tag *next;
  struct context_tag *prev;
} *contextADT;

/*****************************************************************************/
/* Decomposition                                                             */
/*****************************************************************************/
typedef struct decomp_struct {
  int numliterals;
  int curdecomp;
  int prevdecomp;
  int bestdecomp;
  int bestliterals;
  int causal;
  char * statevar;
  int statesig;
  char * enabling;
} *decompADT;

/*****************************************************************************/
/* A list of potential covers                                                */
/*****************************************************************************/

typedef struct coverlist_tag {
  char * cover;
  struct coverlist_tag *link;
} *coverlistADT;

/*****************************************************************************/
/* Region Network                                                            */
/*****************************************************************************/

typedef struct region_tag {
  char * enstate;
  char * switching;
  char * enablings;
  char * tsignals;
  char * cover;
  coverlistADT covers;
  char * *primes;
  int *implied;
  int nprimes;
  int nvprimes;
  contextADT context_table;
  decompADT decomp;
  struct region_tag *link;
} *regionADT;

/*****************************************************************************/
/* Point list ADT for user defined set of points.                            */
/* NOTE: Chris, you can name this to whatever you want with the typedef.     */
/*****************************************************************************/
typedef struct point_list_node {
  double x;
  double y;
  struct point_list_node* next;
} *pointlistADT;

/*****************************************************************************/
/* Timing option variables.                                                  */
/*****************************************************************************/
typedef struct timeopts {
  bool genrg,subsets,supersets,orbmatch,infopt,interleav,prune,disabling;
  bool nofail,noproj,keepgoing;
  // Added 04/12/01 --- egm
  // TDC -> Time Dependent Choice -- default value is false
  bool TDC;
  // Added 05/16/00 --- egm
  // PO_red -> Partial order reduction -- default value is false
  bool PO_reduce;
  /* enables the LPN code instead of the default LTN code when using
     the future variables method */
  bool lpn;
  /* Running hpn reachability code */
  bool hpn;
  /* Running lhpn reachability code */
  bool lhpn;
} timeoptsADT;

typedef struct pruneStruct{
  bool not_first;  // Just stop after first solution found.
  bool preserve;  // Preserve user-specified concurrency.
  bool ordered;  // Prune rules between events that are timed-ordered.
  bool subset;  // Assume subset of rules implies superset of states.
  bool unsafe;  // Prune anything with rules into or out of some
               //    -- but not all -- branches of a choice

  bool expensive;  // Use branch-and-bound to prune expensive solutions.
  bool conflict;  // Prune rules between conflicting events.
  bool reachable;  // Prune redundant rules according to reachability.
  bool dumb;  // Prune rules to dummy events or 
             //    from dummy events that have no fanin expression.
} pruneType;

/*****************************************************************************/
/* This is a linked list of CTL properties to verify.                        */
/*****************************************************************************/
typedef struct property_tag {
  char *property;
  struct property_tag* next;
} *proplistADT;

/*****************************************************************************/
/* Data structures used in a design.                                         */
/*****************************************************************************/


typedef struct design {
  char filename[FILENAMESIZE];
  char component[FILENAMESIZE];
  proplistADT properties;
  signalADT *signals;
  variableADT *variables;
  eventADT *events;
  mergeADT *merges;
  ruleADT **rules;
  cycleADT ****cycles;
  markkeyADT *markkey;
  stateADT state_table[PRIME];
  bddADT bdd_state;
  regionADT *regions;

  // This vector contains the average cycle period as computed by the
  // cycper or -ap flag.  The index into the vector corresponds to the
  // ruleADT->marking value.  Each entry is the weight to be given to
  // the delay value associated with a rule (transistor) being the trigger
  // for a gate.  For those less familiar with STL, cycle_period->begin()
  // will return a prob_type* or a const prob_type* to the beginning of 
  // the vector (array), allowing you to use the normal [] index operator.
  // To find out the size of the array, use cycle_period->size(), which 
  // returns an unsigned int representing the number of elements in the
  // vector (array).  For more information, checkout markov_types.h,
  // any STL reference that discusses the vector interface, or
  // /usr/include/g++/stl_vector (RedHat Linux location).
  prob_vector* cycle_period;
 
  // This vector contains trigger probabilities arranged according to the
  // markkey in the designADT.  
  // NOTE: Memory management is similar to the cycle_period management.
  prob_vector* trigger_probs;

  //initial conditions
  char* startstate;
  char* initval;
  char* initrate;

  //Global inequalities list
  ineqADT inequalities;
  
  int nevents,ninputs,nrules,ninpsig,nsignals,ndisj,nconf,nord,niconf,noconf;
  int ncycles,ncausal,ndummy,nplaces,nineqs,nvars;
  bool si,syn,orbits,sis,exception,brk_exception,manual,verbose,closure;
  bool noparg,nochecks;
  bool fromCSP,fromVHDL,fromHSE,fromER,fromTEL,fromG,fromRSG,fromUNC,fromLPN;
  bool fromDATA;
  bool gatelevel,atomic,gC,heuristic,burstgc,burst2l;
  bool geometric,bdd,pomaxdiff,poapprox,posets,untimed,prune,compress,srl,hpn;
  bool lhpndbm,lhpnbdd,lhpnsmt;
  bool single,multicube,exact,dot,sharegates,combo,direct,inponly;
  bool postproc,newtab,redchk,abstract,expandRate;
  timeoptsADT timeopts; 
  int maxsize,iterations,olditerations,mingatedelay,maxgatedelay;
  double tolerance;
  int oldnvars,oldnsignals,oldnevents,oldncycles,oldmaxsize;
  int oldmingatedelay,oldmaxgatedelay;
  int status,simtime;
  bool *comb;
  bool level;
  bool xform2;
  // egm 08/30/00 -- New timing method that builds on the geometric
  // approach: Bourne Again Geometric
  bool bag;

  // egm 11/15/00 -- New timing method that builds on the POSET
  // approach: Bourne Again POSET
  bool bap;
  
  set<char*> top_port;
  bool reduction;  //  Attempt automatic concurrency reduction?
  pruneType pruning;  // concurrency reduction pruning option flags
  bool insertion;  //  Attempt automatic state-variable insertion?
  bool minins;  //  Limited transition point size to 1 start and/or end
  int total_lits;
  int area;
  char *assignments;
} *designADT;

//  The following outputs the TEL structure to a stream in TEL file format:
ostream& operator<<(ostream&out,  designADT design);  //Output a design.

extern FILE *lg;

extern bool printmem;


#endif /* struct.h */


