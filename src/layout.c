/*
/   This is |layout|, a subroutine library for laying out arbitrary directed
/   graphs for display or print.  This code may be freely redistributed,
/   as long as this banner is not omitted or changed, as long as the code
/   below is not changed, and as long as the banner is printed out whenever
/   this program is first called (which happens automatically.)
/*/
#define BANNER "layout 1.04 (C) Copyright 1990, 1992, 1994 Tomas Rokicki"
/*
/   {\def\mbreak{\vskip\baselineskip}
/   This code will compile under C, C++, or ANSI C; if you are using either
/   ANSI or C++, define the preprocessor symbol |CPLUS| when compiling
/   this code.
/
/   If your compiler doesn't understand |void *|, compile with |-Dvoid=char|.
/   (All ANSI and C++ compilers know |void *|, so only use this if you are
/   not using the |CPLUS| flag.)
/
/   The intent of this library is to provide a link from some internal
/   graph representation to a computer display or printer.  The graphs
/   handled by this program can be cyclic and they need not be connected.
/   The nodes can have multiple inputs or outputs, as well as multiple
/   `ports' to which these inputs and outputs connect.  The nodes can have
/   different heights and widths.  Where such complexity is not desired,
/   it is easy to ignore.
/
/   To use this subroutine library, you must first write code to
/   sequentially number the nodes in the graph you wish to display.
/   The order can be arbitrary, but each of the numbers from 0 to |n-1|,
/   where n is the number of nodes, must be assigned to a node.
/
/   In general, when you are passing the identity of a node to the
/   |layout| library, you will pass this identifying number.  When the
/   |layout| library passes you the identity of a particular node, it
/   will do so with a |void *| pointer, usually a pointer to whatever
/   node structure you are using internally.
/
/   A number of accessor functions must also be written.  These functions
/   are called to get the graph topology and various other data from the
/   user; many of these are optional.  For all of these functions, the
/   names used below are strictly arbitrary, since all access is through
/   function pointers; you may even want to declare these functions as
/   static.

void fillnodearray(void **nodearr) ;

/   The first such function is |fillnodearray|.  This function gets
/   called with a pointer to an uninitialized array of |void *|, and
/   it should be filled in with whatever pointers are desired to be
/   used when the |layout| functions ask for node-specific information.
/   Typically this will be pointers to the internal node structure
/   used by the program calling |layout|.  Thus, |nodearr[i]| might
/   be a pointer to the node structure that was assigned the index
/   i before the |layout| routines were called.  This array is used
/   henceforth by |layout| to translate node indexes to node pointers
/   whenever information from the user program is required.

void filledgearray(void *node, int *incnt, int *outcnt, int *inarity,
     int *outarity, int *succarr) ;

/   The next function required is |filledgearray|; this is called once
/   for each node and returns the topology of the graph.  The |node|
/   pointer is one of those you initialized when |fillnodearray| was
/   called.  You must set the value |*incnt| to the number of edges that
/   come into this node, |*outcnt| to the number of edges that leave it,
/   |*inarity| to the number of input ports the node has, and |*outarity|
/   to the number of output ports the node has.  (Note that if you
/   don't have separate ports for the edges, you should set |*inarity|
/   and |*outarity| to 1, not 0.)  Then, fill the |succarr| array with
/   the index of each node that that edge goes to.  (The index is the
/   number from 0 to |n-1| you assigned earlier.)  The order you fill
/   this edge array is important; make sure that each function that
/   fills an array like this fills it in the same order so that
/   information about one edge stays together.

void filledgeptrs(void *node, void **edges) ;

/   This is an optional routine analogous to |fillnodearray|, only
/   for edges.  For each output edge from a node, this routine must
/   fill in a pointer to the internal edge structure; this pointer
/   will be passed back to the |drawedge| routine when it is called so
/   the user can easily identify the edge.  If you don't have any
/   specific edge structure, or if all edges are pretty much the same,
/   this routine need not be supplied.

void filledgeio(void *node, short *io) ;

/   This optional routine fills in the `port' number to which each edge
/   connects for a node.  For each output edge from |node|, two sequential
/   shorts from the |io| array must be filled; the first short is the port
/   number that that edge connects to |node| with, and the second short is
/   the port number of the node that that edge connects to its successor node
/   with.  These port numbers must be between 0 and |m-1|, where |m| is
/   the outarity of |node| or the inarity of the successor node of the edge,
/   respectively.  This routine and the two before it must all go through
/   the output edges of |node| in the same order.

void *malloc(int n) ;

/   This optional routine must allocate and return |n| bytes of memory,
/   suitably aligned so that a longword can be stored in the first four
/   bytes of this memory.  If not supplied, the routines will fall back
/   on a built-in routine that calls the C library |malloc| routine and
/   |exit()|s with an error if memory could not be allocated.  This
/   routine is usually only supplied if the user program has a special
/   memory allocation routine it uses a lot.

int getheight(void *node) ;

/   This optional function returns the height of a drawn node.  The
/   height must be positive and should always be less than or equal to
/   some maximum height, which is passed in separately.  If this routine
/   is not supplied, the maximum height is assumed for each node.

int getwidth(void *node) ;

/   This function returns the width of a node.  Such width may be
/   dependent on the length of the label that will be written in a
/   node, plus a bit for the borders.  Most windowing systems provide
/   a function such as |int stringwidth(char *s)| which returns the
/   length of a particular string in the default font.  Typically
/   this routine will call that function and add a bit for borders.
/   Or, if the nodes are to be unlabeled, this function may return a
/   constant, or a constant that depends on the type of node.

void getoffsets(void *node, short *inoffs, short *outoffs) ;

/   This optional routine fills two arrays with the horizontal offsets
/   of the ports on |node|; the array |inoffs| is of |inarity| length
/   and |outoffs| is of |outarity| length.  The offsets are from the
/   center of the node.  This routine should be supplied if |inarity|
/   or |outarity| were set to a number greater than one; otherwise all
/   ports will appear to connect at the horizontal center of the node.
/   Typically this routine will just evenly distribute ports along the
/   width of the node.
/
/   If there are vertical offsets, they should be appended to the ends
/   of the |inoffs| and |outoffs| array; otherwise, the default values
/   of zero are used.  These should be offsets from the top and bottom
/   edge of the node itself.  The vertical offsets are only used during
/   drawing; so long as they are always within the node bounding box,
/   they cannot increase the slope so this simplification is
/   conservative.

void drawnode(void *node, int x, int y) ;

/   This required routine must draw |node| such that its center lies
/   at |(x,y)|.  It might draw different shapes depending on the type
/   of the node; in an object-oriented environment it might just call
/   a |drawself| member.

void drawedge(void *edge, int x1, int y1, int x2, int y2, char back) ;

/   This required routine must draw an edge from |(x1,y1)| to |(x2,y2)|.
/   It can use the |edge| parameter to vary the color of the edge by type.
/   If |back| is set, then the edge is a back edge (goes against the
/   prevailing top to bottom order.)
/
/   Once these routines are written, the rest is a piece of cake.  The
/   definitions of the following functions omits the type declarations
/   of the callback functions for brevity.

void *layout_graph(int n, void (*fillnodearray)(),
     void (*filledgearray)(), void (*filledgeptrs)(), void (*filledgeio)(),
     void *(*malloc)()) ;

/   We also include a version that works with an optional parameter set
/   so that some of the parameters can define static layout-time
/   quantities.

void *layout_graph_p(int n, void (*fillnodearray)(),
     void (*filledgearray)(), void (*filledgeptrs)(), void (*filledgeio)(),
     void *(*malloc)(), struct layout_adjust_params *lap) ;

/   This is the main routine you call, passing in the appropriate
/   functions or 0 if you want them to default.  The |n| parameter
/   must be the total number of nodes in the graph.  This routine goes
/   ahead and does most of the layout of the graph, and it allocates
/   and returns a special structure.  This structure must be saved and
/   passed to the subsequent routines.

void layout_adjust(void *g, int (*getwidth)(), int (*getheight)(),
     void (*getoffsets)(), struct layout_adjust_params *lap,
     int *finalw, int *finalh) ;

/   This routine does the actual geometrical layout based on the width,
/   height, and offsets.  The first parameter is the pointer returned by
/   |layout\_graph()|.
/   The |lap| parameter is used to provide additional information to the
/   |layout\_adjust()| routine; see |layout.h| for more information.
/   The |*finalw| and |*finalh| are return parameters that get the resultant
/   width and height of the final graph.  The |layout\_adjust()| routine
/   may be called several times for the same graph.  For example if the
/   user selects a different font when running under X, there is no need to
/   call |layout\_graph()| again; simply call |layout\_adjust()| again with
/   different parameters.  Or, if printing is desired, just call
/   |layout\_adjust()| with accessors appropriate for printing.

void layout_draw(void *g, void (*drawnode)(), void (*drawedge)()) ;

/   This routine actually draws the node, calling the |drawnode()| function
/   once for each node and the |drawedge()| function at least once for each
/   edge.  This too can be called multiple times before the others, but it
/   may not be called before |layout\_adjust()| has been called at least
/   once.  Typically this routine is called whenever the display needs
/   refreshing.
     
void layout_clean(void *g) ;

/   This routine deallocates all of the memory allocated by the various
/   layout routines for the particular graph passed in; once this is done,
/   that graph is effectively forgotten and none of the other routines.
/   should be called.
/
/   Some more comments on the code below are in order.  All functions are
/   declared as static except for the ones that provide access to the
/   library itself; these four functions start with |layout\_|.  Note that
/   the library is not reentrant, but it does support multiple concurrent
/   graphs, since all of the information specific to a particular graph is
/   stored in the allocated structure returned by |layout\_graph()|.
/
/   A global variable |lo\_debug| is also accessible if this program
/   is compiled with the |DEBUG| preprocessor symbol (by default); if this
/   variable is set to nonzero, some amount of debugging output will be
/   written to |stdout|.
/
/   You can adjust some parameters by initializing a |layout_adjust_params|
/   structure, defined in |layout.h|.  See that file for more comments; we
/   create a default structure here to use if the user doesn't pass one in.
/*/
#include "layout.h"
static struct layout_adjust_params lo_lap = {0, 0, 14, 5, 3, 7, 10, 1} ;
/*
/   The code itself needs many more comments.
/
/   Please send comments and suggestions to Tomas Rokicki,
/   |rokicki@cs.stanford.edu|, so that improvements can be made.
/   }
/*/
#include <stdio.h>
#define DEBUG
int lo_debug = 0 ;
#ifdef CPLUS
#define VOID void
#include <std.h>
#else
#define VOID
#endif
/*
/   Define this to be |static| for normal compilation, or nothing if you
/   want, for instance, profiling.
/*/
#ifndef STATIC
#define STATIC static
#endif
/*
/   These routines lay out an arbitrary directed multigraph.  The graph
/   need not be connected.  A node can even connect to itself.
/
/   We have a global structure to hold all of the variables relevant
/   to a particular graph.  All such variables are private.
/*/
STATIC struct lo_graph {
#ifdef CPLUS
   void *(*mymalloc)(int n) ;
#else
   void *(*mymalloc)() ;
#endif
   int num_nodes, num_edges ;
   int back_edge_count, graph_height ;
   int num_extras ;
   int node_height ;
   struct lo_node **lo_nodes ;
   struct lo_edge **lo_edges ;
   struct lo_level **lo_levels ;
   struct lo_node *extra_nodes ;
   void *extra_stuff ;
   long *mwars ;
   short *mpaths ;
} *G ;
/*
/   The following define sets the maximum indegree or outdegree that a
/   node can have; it helps us to allocate a static buffer for the
/   edges.
/*/
#define MAXEDGES (2000)
/*
/   These parameters tune the graph selection process.  The |HEIGHT\_PENALTY|
/   is a penalty assessed for each increment in the graph height.  The
/   |BACKEDGE\_PENALTY| is assessed for each backwards edge in a graph.
/   The rest of the penalties is simply the sum of the lengths of the
/   edges.
/*/
#define HEIGHT_PENALTY (3)
#define BACKEDGE_PENALTY (10)
/*
/   Each time we advance a node to the bottom, we charge it this penalty
/   to try to keep it from advancing again.  This helps us to wrap all
/   loops eventually.
/*/
#define LOOP_PENALTY (10)
/*
/   This is our main structure for holding data we need.
/*/
struct lo_node {
   int index ;
   short *inoffs, *outoffs ;
   struct lo_node *next ;
   struct lo_edge **inlist, **outlist ;
   void *user_data ;
   char intree, simple ;
   short penalty ;
   short indegree, outdegree ;
   short inarity, outarity ;
   short indist, outdist ;
   short inback ;
   short dummy ;
} ;
/*
/   We also need to store data regarding the edges.
/*/
struct lo_edge {
   int index ;
   struct lo_node *head, *tail ;
   struct lo_node *top, *bottom ;
   void *user_data ;
   char back_edge, best_edge ;
   short inpos, outpos ;
   short dummy ;
} ;
/*
/   The following variable holds the maximum total indegree and outdegree
/   for any node in the graph.  It is useful for our radix searches.
/*/
STATIC int max_edge_count ;
STATIC struct lo_node **radix_array ;
/*
/   When we want to report an error to the user, we call the following
/   routine.  It may be customizeable in the future.
/*/
#ifdef CPLUS
STATIC void lo_error(char *s)
#else
STATIC void lo_error(s)
char *s ;
#endif
{
   fprintf(stderr, "layout: %s\n", s) ;
   exit(1) ;
}
/*
/   The default memory allocator if none passed.
/*/
#ifdef CPLUS
STATIC void *lo_alloc(int n)
#else
STATIC void *lo_alloc(n)
int n ;
#endif
{
   static int sofar = 0 ;

   void *p = (void *)malloc(n) ;
   sofar += n ;
#ifdef DEBUG
   if (lo_debug)
      printf("Mallocing %d (%d)\n", n, sofar) ;
#endif
   if (!p)
      lo_error("! out of memory in layout") ;
   return p ;
}
/*
/   Now we have the routines that collect the data from the user.
/   It takes pointers to functions, which for now we just declare
/   as void *, and stores them away.  It allocates a main array,
/   and fills the array with the user function.  It also allocates
/   smaller buffers for the in and out edges, and fills them as
/   well.  It allocates buffers for the input and output offsets as
/   well, but leaves them alone for now.  (They will be filled in
/   later at the device-dependent draw stage.)
/*/
#ifdef CPLUS
STATIC void *lo_init(int n,
   void (*fillnodearray)(void **nodearr),
   void (*filledgearray)(void *node, int *incnt, int *outcnt, int *inarity,
      int *outarity, int *succarr),
   void (*filledgeptrs)(void *node, void **varr),
   void (*filledgeio)(void *node, short *io), void *(*mmalloc)(int n))
#else
STATIC void lo_init(n, fillnodearray, filledgearray, filledgeptrs,
    filledgeio, mmalloc)
int n ;
void (*fillnodearray)(), (*filledgearray)(), (*filledgeptrs)(),
     (*filledgeio)(), *(*mmalloc)() ;
#endif
{
   struct lo_node *p ;
   struct lo_edge **qq, *q ;
   int inarity, outarity ;
   int inedgen, outedgen ;
   int i ;
   union {
      int vint ;
      void *vptr ;
      short inout[2] ;
   } v[MAXEDGES] ;

   if (mmalloc == 0)
      mmalloc = lo_alloc ;
   G = (struct lo_graph *)mmalloc(sizeof(struct lo_graph)) ;
   G->mymalloc = mmalloc ;
   G->num_nodes = n ;
   G->lo_nodes = (struct lo_node **)
         (G->mymalloc)(n * sizeof(struct lo_node *)) ;
   G->lo_edges = 0 ;
   G->lo_levels = 0 ;
   G->extra_nodes = 0 ;
   G->extra_stuff = 0 ;
   G->mwars = 0 ;
   fillnodearray((void *)(G->lo_nodes)) ;
   max_edge_count = 0 ;
   G->num_edges = 0 ;
   for (n=0; n<G->num_nodes; n++) {
      inarity = 1 ;
      outarity = 1 ;
      filledgearray((void *)(G->lo_nodes[n]), &inedgen, &outedgen,
                                      &inarity, &outarity, (int *)v) ;
      if (inarity <= 0)
         inarity = 1 ;
      if (outarity <= 0)
         outarity = 1 ;
      if (inedgen + outedgen >= max_edge_count)
         max_edge_count = inedgen + outedgen + 1 ;
      p = (struct lo_node *)(G->mymalloc)(sizeof(struct lo_node) + 
                   (inedgen + outedgen) * sizeof(struct lo_edge *) +
                   outedgen * sizeof(struct lo_edge) +
                   2 * (inarity + outarity) * sizeof(short)) ;
      p->index = n ;
      p->user_data = (void *)(G->lo_nodes[p->index]) ;
      G->lo_nodes[p->index] = p ;
      p->indegree = inedgen ;
      p->outdegree = outedgen ;
      p->inarity = inarity ;
      p->outarity = outarity ;
      qq = (struct lo_edge **)(p + 1) ;
      q = (struct lo_edge *)(qq + inedgen + outedgen) ;
      p->inlist = qq ;
      for (i=0; i<inedgen; i++)
         *qq++ = 0 ;
      p->outlist = qq ;
      for (i=0; i<outedgen; i++) {
         q->tail = p ;
         q->head = (struct lo_node *)(v[i].vint) ;
         q->index = G->num_edges++ ;
         q->inpos = 0 ;
         q->outpos = 0 ;
         q->user_data = 0 ;
         *qq++ = q++ ;
      }
      p->inoffs = (short *)q ;
      for (i=0; i<2*inarity; i++)
         p->inoffs[i] = 0 ;
      p->outoffs = p->inoffs + 2*inarity ;
      for (i=0; i<2*outarity; i++)
         p->outoffs[i] = 0 ;
      p->indist = 0 ; /* initialize this for later connection of tails */
      p->inback = 0 ;
      p->intree = 0 ;
      p->penalty = 0 ;
      if (filledgeptrs) {
         filledgeptrs(p->user_data, (void **)v) ;
         qq = p->outlist ;
         for (i=0; i<outedgen; i++) {
            q = *qq++ ;
            q->user_data = v[i].vptr ;
         }
      }
      if (filledgeio) {
         filledgeio(p->user_data, (short *)v) ;
         qq = p->outlist ;
         for (i=0; i<outedgen; i++) {
            q = *qq++ ;
            q->inpos = v[i].inout[0] ;
            q->outpos = v[i].inout[1] ;
         }
      }
   }
   G->lo_edges = (struct lo_edge **)
         (G->mymalloc)(G->num_edges * sizeof(struct lo_edge *)) ;
   for (n=0; n<G->num_nodes; n++) {
      struct lo_node *pp ;
      int i ;

      p = G->lo_nodes[n] ;
      for (i=0; i<p->outdegree; i++) {
         q = p->outlist[i] ;
         pp = G->lo_nodes[(int)(q->head)] ;
         q->head = pp ;
         q->back_edge = 0 ;
         q->best_edge = 0 ;
         pp->inlist[pp->indist++] = q ;
         G->lo_edges[q->index] = q ;
      }
   }
   for (n=0; n<G->num_nodes; n++) {
      p = G->lo_nodes[n] ;
      if (p->indist != p->indegree) {
         fprintf(stderr, "%d in edges expected; %d seen\n",
                          p->indist, p->indegree) ;
         lo_error("! consistency violation in input edge data") ;
      }
   }
#ifdef DEBUG
   if (lo_debug)
      printf("Initialized graph:  %d nodes, %d edges\n",
                                  G->num_nodes, G->num_edges) ;
#endif
}
/*
/   This routine marks an edge as a backwards edge and does all
/   necessary bookkeeping.
/*/
#ifdef CPLUS
STATIC void backedge(struct lo_edge *e)
#else
STATIC void backedge(e)
struct lo_edge *e ;
#endif
{
   e->back_edge = 1 ;
   G->back_edge_count++ ;
   e->head->inback++ ;
}
/*
/   This routine marks an edge as not a backwards edge and does all
/   necessary bookkeeping.
/*/
#ifdef CPLUS
STATIC void unbackedge(struct lo_edge *e)
#else
STATIC void unbackedge(e)
struct lo_edge *e ;
#endif
{
   e->back_edge = 0 ;
   G->back_edge_count-- ;
   e->head->inback-- ;
}
/*
/   This recursive routine is used to propogate a new value down
/   to the successors.  It never jumps a backwards edge, and marks an
/   edge as backwards if a cycle is encountered.
/*/
#ifdef CPLUS
STATIC void succ_chase(struct lo_node *p, int n)
#else
STATIC void succ_chase(p, n)
struct lo_node *p ;
int n ;
#endif
{
   int i ;
   struct lo_edge *e ;
   if (n > p->indist) {
      p->indist = n ;
      p->intree = 1 ;
      for (i=p->outdegree-1; i>=0; i--) {
         e = p->outlist[i] ;
         if (e->back_edge == 0)
            if (e->head->intree)
               backedge(e) ;
            else
               succ_chase(e->head, n+1) ;
      }
      p->intree = 0 ;
   }
}
/*
/   This routine is the analog of the one above, only it works
/   from the bottom up.
/*/
#ifdef CPLUS
STATIC void pred_chase(struct lo_node *p, int n)
#else
STATIC void pred_chase(p, n)
struct lo_node *p ;
int n ;
#endif
{
   int i ;
   struct lo_edge *e ;
   if (n > p->outdist) {
      p->outdist = n ;
      p->intree = 1 ;
      for (i=p->indegree-1; i>=0; i--) {
         e = p->inlist[i] ;
         if (e->back_edge == 0)
            if (e->tail->intree)
               backedge(e) ;
            else
               pred_chase(e->tail, n+1) ;
      }
      p->intree = 0 ;
   }
}
/*
/   This routine takes a particular level assignment and `optimizes'
/   it.  It does this by scanning the node list twice.
/
/   The first scan figures out the height of the graph by finding
/   the maximum distance in the graph.  It also adds each node to
/   the appropriate portion of a radix sort array.
/
/   The second scan is done through the radix sort array, and it
/   assigns the positions of any graph elements that have freedom.
/   It does it in a way that tends to move things to the top of the
/   graph if they could go either way.
/
/   Change; tgr; now we move things to the level with the fewest
/   nodes where there is freedom.
/
/   Note that this radix sort algorithm does not necessarily yield
/   optimal placement, but it is fast.
/*/
STATIC int fix_levels(VOID) {
   int i, j ;
   int height = 0 ;
   struct lo_node *p ;
   struct lo_edge *e ;

   if (radix_array == 0)
      radix_array = (struct lo_node **)
            (G->mymalloc)(MAXEDGES * sizeof(struct lo_node *)) ;
   for (i=0; i<max_edge_count; i++)
      radix_array[i] = 0 ;
   for (i=0; i<G->num_nodes; i++) {
      p = G->lo_nodes[i] ;
      if (p->indist + p->outdist > height)
         height = p->indist + p->outdist ;
      j = p->indegree - p->outdegree ;
      if (j < 0)
         j = -j ;
      p->next = radix_array[j] ;
      radix_array[j] = p ;
   }
   for (i=max_edge_count - 1; i >= 0; i--)
      for (p=radix_array[i]; p; p = p->next) {
         if (p->indist + p->outdist < height) {
            j = p->indegree - p->outdegree ;
            if (j > 0) /* the = case helps move things to the top */
               pred_chase(p, height - p->indist) ;
            else
               succ_chase(p, height - p->outdist) ;
         }
      }
   for (i=0; i<G->num_edges; i++) {
      e = G->lo_edges[i] ;
      if (e->back_edge) {
         if (e->head->indist > e->tail->indist)
            unbackedge(e) ;
      } else {
         if (e->head->indist <= e->tail->indist)
            backedge(e) ;
      }
   }
   return height + 1 ;
}
/*
/   With that under control, we proceed.  This routine assigns an
/   initial level to all the graph nodes.  If it can do so without
/   encountering any loops, well and great, it has a DAG, and it
/   simply calls the routine that optimizes the levels.
/*/
#ifdef CPLUS
STATIC int optimize_levels(struct lo_node *first)
#else
STATIC int optimize_levels(first)
struct lo_node *first ;
#endif
{
   int i ;
   struct lo_node *p ;

   for (i=0; i<G->num_nodes; i++) {
      p = G->lo_nodes[i] ;
      p->indist = -1 ;
      p->outdist = -1 ;
   }
   if (first)
      pred_chase(first, 0) ;
   for (i=0; i<G->num_nodes; i++) {
      p = G->lo_nodes[i] ;
      succ_chase(p, 0) ;
      pred_chase(p, 0) ;
   }
   return fix_levels() ;
}
/*
/   This routine assigns a value to the graph and compares it against
/   the best found so far.  
/
/   If the value is the best so far, it remembers it.  (Note that a
/   particular graph state in the context of layout is uniquely identified
/   by the set of edges that are backwards edges.)
/*/
STATIC int best_so_far ;
STATIC void check_frontier(VOID) {
   int i, val ;
   struct lo_edge *e ;
   val = HEIGHT_PENALTY * G->graph_height +
         BACKEDGE_PENALTY * G->back_edge_count ;
   for (i=0; i<G->num_edges; i++) {
      e = G->lo_edges[i] ;
      if (e->back_edge)
         val += e->tail->indist - e->head->indist ;
      else
         val += e->head->indist - e->tail->indist ;
   }
#ifdef DEBUG
   if (lo_debug)
      printf("Iteration; value=%d height=%d back=%d\n", val,
                     G->graph_height, G->back_edge_count) ;
#endif
   if (val < best_so_far) {
      best_so_far = val ;
      for (i=0; i<G->num_edges; i++) {
         e = G->lo_edges[i] ;
         e->best_edge = e->back_edge ;
      }
   }
   return ;
}
/*
/   This routine searches the nodes for a good node to advance to the
/   bottom.  A node can only be advanced to the bottom if
/*/
#ifdef CPLUS
STATIC void advance_one(void)
#else
STATIC void advance_one()
#endif
{
   struct lo_node *p, *best ;
   int val, bestval = -100000000 ;
   struct lo_edge *e ;
   int i ;

   for (i=0; i<G->num_nodes; i++) {
      p = G->lo_nodes[i] ;
      if (p->inback) {
         val = p->penalty + (p->inback - p->outdegree) * BACKEDGE_PENALTY
                          + (p->inback + p->outdegree)
                          - G->graph_height * (p->indegree - p->inback) ;
         if (val > bestval) {
            best = p ;
            bestval = val ;
         }
      }
   }
   if (best) {
#ifdef DEBUG
   if (lo_debug)
         printf("Advancing %d; value=%d inback=%d indegree=%d outdegree=%d\n",
          best->index, bestval, best->inback, best->indegree, best->outdegree) ;
#endif
      for (i=0; i<best->indegree; i++) {
         e = best->inlist[i] ;
         if (e->back_edge)
            unbackedge(e) ;
      }
      best->penalty -= LOOP_PENALTY ;
   }
   G->graph_height = optimize_levels(best) ;
}
/*
/   This routine restores our best graph by taking apart the best
/   set and marking only those as back edges.  It then restores the
/   level by calling the optimization routine.
/*/
STATIC void restore_best(VOID) {
   int i ;
   struct lo_edge *e ;

   for (i=0; i<G->num_edges; i++) {
      e = G->lo_edges[i] ;
      if (e->best_edge) {
         if (! e->back_edge)
            backedge(e) ;
      } else if (e->back_edge)
         unbackedge(e) ;
   }
   G->graph_height = optimize_levels(0) ;
#ifdef DEBUG
   if (lo_debug)
      printf("Final value=%d height=%d back=%d\n", best_so_far,
                                   G->graph_height, G->back_edge_count) ;
#endif
}
/*
/   This is our main layout routine (as opposed to data I/O.)  We
/   finally put all of the above pieces together.  This half of the
/   package assigns levels to each node and determines back edges.
/*/
STATIC void assign_levels(VOID) {
   int i = 0 ;

   G->back_edge_count = 0 ; /* no back edges in initialized graph */
   best_so_far = 1000000000 ;
   G->graph_height = optimize_levels(0) ;
   if (G->back_edge_count) {
      while (i++ < 2 * G->num_nodes) {
         check_frontier() ;
         advance_one() ;
      }
      restore_best() ;
   }
}
/*
/   You thought that was easy?  Now we have the hard part---finding the
/   ordering of the nodes in each level.  This is the most complicated
/   part of the code.
/
/   The data for each level is stored in a |level| structure.
/
/   We also set a |max_width| variable so we can allocate arrays for
/   dynamic exploration of the graph.
/
/   Our |graph\_height| variable indicates how many levels are allocated.
/*/
struct lo_level {
   short width ; /* how many nodes are on this level, including fakes? */
   short posed ; /* how many nodes have been positioned so far? */
   short indist ; /* where are we, anyway? */
   short maxdiff ; /* maximum distance an edge to prev traverses */
   short levheight ; /* max height of a node in this level */
   short indegree, outdegree ; /* how many edges in and out */
   struct lo_node **nodes ; /* here's where the nodes actually live */
} ;
STATIC int max_width ;
/*
/   When we add our virtual nodes and edges, we will add just as many of
/   one as we do the other.  The variable |extra_stuff| holds a pointer to
/   all of the memory we allocated for these.  The pointers
/   |extra_nodes| and |extra_edges| point to the actual edges and nodes
/   themselves, and the variable |num_extras| holds the number we needed.
/   The |mwars| variable is a pointer to a few arrays we can use for dynamic
/   algorithms, and |mpaths| helps us save a path array.
/
/   The first chunk of code calculates the width of each level, taking
/   into account the back edges.  If there is a back edge to a node on
/   level zero (a common occurence), we walk the entire node list,
/   incrementing the level counters, so we can easily refer to the levels
/   as 0..height, which is better than remembering some arbitrary offset.
/
/   It then constructs a new graph that will actually be `sorted' by
/   widths, marking important nodes and unimportant nodes.
/
/   We need a short `zero' array for the offsets of the simple nodes.
/*/
STATIC short lo_zero[4] ;
#ifdef CPLUS
STATIC void figure_widths(struct layout_adjust_params *lap)
#else
STATIC void figure_widths(lap)
struct layout_adjust_params *lap ;
#endif
{
   int i, j, k, m, n, o, olim ;
   struct lo_node *p, *pp ;
   struct lo_edge *e ;
   struct lo_level *v ;
   int joinpossible = (lap ? lap->joinpossible : 1) ;
   {
      int back_at_top = 0, back_at_bottom = 0 ;
      for (i=0; i<G->num_edges; i++) {
         e = G->lo_edges[i] ;
         if (e->back_edge) {
            if (e->head->indist == 0)
               back_at_top = 1 ;
            if (e->tail->indist + 1 == G->graph_height)
               back_at_bottom = 1 ;
         }
      }
      if (back_at_top) {
         for (i=0; i<G->num_nodes; i++)
            (G->lo_nodes[i]->indist)++ ;
         G->graph_height++ ;
      }
/*
/   If there is a back edge from the bottom, the |outdist| value in the
/   node list is no longer valid.  That's okay; we don't need it anymore.
/   We may even use the slot for something else in this algorithm (along
/   with inback, which may not be needed either.)
/*/
      if (back_at_bottom)
         G->graph_height++ ;
   }
   G->lo_levels = (struct lo_level **)
            (G->mymalloc)((sizeof(struct lo_level) +
                             sizeof(struct lo_level *)) * G->graph_height) ;
   for (i=0, v=(struct lo_level *)(G->lo_levels + G->graph_height);
                                              i<G->graph_height; i++, v++) {
      v->width = 0 ;
      v->posed = 0 ;
      v->indist = i ;
      G->lo_levels[i] = v ;
   }
/*
/   Now we calculate the width of each level, including any virtual nodes
/   that might be needed.  The width is incremented for every node and for
/   every edge that crosses a node.  Plus the back edges, which add four
/   more virtual nodes (one above, one below, one at the head level, and one
/   at the tail level.)
/*/
   {
      int s = 0, ss = 0 ;
      struct lo_edge *ne, **nep, *one, **onep, **epp ;
      struct lo_node *nn, *onn ;
      struct lo_node **npp ;
      int nncount = G->num_nodes ;
      int reach, mm ;

      for (i=0; i<G->num_nodes; i++, s++)
         (G->lo_levels[G->lo_nodes[i]->indist]->width)++ ;
      for (i=0; i<G->num_nodes; i++) {
         pp = G->lo_nodes[i] ;
         for (n=0; n<pp->outarity; n++) {
            reach = pp->indist + 1 ;
            for (m=0; m<pp->outdegree; m++) {
               e = pp->outlist[m] ;
               if (e && e->inpos == n) {
                  e->top = e->tail ;
                  e->bottom = e->head ;
                  if (e->back_edge) {
                     for (j=e->head->indist-1; j<=e->tail->indist+1; j++) {
                        (G->lo_levels[j]->width)++ ;
                        s++ ;
                     }
                  } else {
                     if (joinpossible && e->head->indist > pp->indist + 1)
                        ss++ ;
                     for (j=reach; j<e->head->indist; j++) {
                        (G->lo_levels[j]->width)++ ;
                        s++ ;
                        if (joinpossible)
                           reach++ ;
                     }
                  }
               }
            }
         }
      }

/*
/   Now we allocate our virtual nodes, virtual edges, and pointers to
/   the virtual nodes.  This is probably a fairly hefty allocation.
/*/
      G->num_extras = s - G->num_nodes ;
      G->extra_stuff = (void *)(G->mymalloc)(s * sizeof(struct lo_node *) +
           ss * sizeof(struct lo_edge *) +
           G->num_extras * (sizeof(struct lo_edge) + sizeof(struct lo_node) +
                                             2 * sizeof(struct lo_edge *))) ;
#ifdef DEBUG
      if (lo_debug)
         printf("Allocated %d extra nodes and edges\n", G->num_extras) ;
#endif
      G->extra_nodes = (struct lo_node *)G->extra_stuff ;
      ne = (struct lo_edge *)(G->extra_nodes + G->num_extras) ;
      nep = (struct lo_edge **)(ne + G->num_extras) ;
      onep = nep ;
      nn = G->extra_nodes ;
      npp = (struct lo_node **)(nep + 2 * G->num_extras + ss) ;
/*
/   Now we go through the process we just did, adding all the necessary
/   connections between/to our virtual nodes and edges.
/*/
      for (i=0; i<G->graph_height; i++) {
         v = G->lo_levels[i] ;
         v->nodes = npp ;
         npp += v->width ;
         v->posed = 0 ;
      }
      for (i=0; i<G->num_nodes; i++, s++) {
         p = G->lo_nodes[i] ;
         v = G->lo_levels[p->indist] ;
         p->outdist = v->posed ;
         v->nodes[v->posed++] = p ;
      }
/*
/   As we add the extra nodes, we create |top| and |bottom| edges
/   to point to the virtual nodes, as necessary.  The |head| and |tail|
/   nodes retain their original values at all times; when we create a
/   virtual edge, we copy these values for easy reference.
/   As we allocate virtual edges, we copy the |index|, so we can get
/   the `real' edge by doing
/
/      |G->lo_edges[e->index]|
/
/   (which is a no-op on a real edge.)
/
/   This gives us a `mapping' of two graphs onto each other.  One, real
/   one, uses |head|, |tail| as pointers to real nodes.  One, virtual,
/   uses |top|, |bottom| as pointers to the important virtual nodes.
/
/   For backwards edges, we `reverse' most of the edge.  First, we add
/   two virtual nodes, one at the top and one at the bottom.  Each of
/   these is not simple.  The top node has indegree of 0 and outdegree of
/   2; the bottom has indegree of 2 and outdegree of 0.  Then we simply
/   `concatenate' more in as we did for the more normal edges.
/
/   This section is long because it has got lots of pointers to update.
/   Updating these pointers needs to be done carefully but it is
/   fairly straightforward.
/*/
#ifdef DEBUG
      if (lo_debug)
         printf("Tacking back edge nodes onto graph . . .\n") ;
#endif
      for (i=0; i<G->num_edges; i++) {
         e = G->lo_edges[i] ;
         if (e->back_edge) {
            j = e->head->indist - 1 ;
            v = G->lo_levels[j] ;
            nn->index = ++nncount ;
            nn->outdist = v->posed ;
            v->nodes[v->posed++] = nn ;
            *ne = *e ;
            ne->inpos = 0 ;
     /*     ne->outpos = e->outpos ; redundant */
            e->outpos = 0 ;
            nn->simple = 0 ;
            nn->indegree = 0 ;
            nn->outdegree = 2 ;
            nn->indist = j ;
            nn->inback = 0 ;
            nn->inlist = nep ;
            nn->outlist = nep ;
            nn->intree = 0 ;
            nn->penalty = 0 ;
            nn->inoffs = lo_zero ;
            nn->outoffs = lo_zero ;
            nn->inarity = 1 ;
            nn->outarity = 1 ;
            *nep++ = ne ;
            *nep++ = e ;
            ne->top = nn ;
            p = e->head ;
            ne->bottom = p ;
            for (k=0; k<p->indegree; k++)
               if (p->inlist[k] == e) {
                  p->inlist[k] = ne ;
                  break ;
               }
            e->top = nn ;
            ne++ ;
            nn++ ;
            j = e->tail->indist + 1 ;
            v = G->lo_levels[j] ;
            nn->index = ++nncount ;
            nn->outdist = v->posed ;
            v->nodes[v->posed++] = nn ;
            *ne = *e ;
     /*     ne->inpos = e->inpos ; redundant */
            e->inpos = 0 ;
            ne->outpos = 0 ;
            nn->simple = 0 ;
            nn->indegree = 2 ;
            nn->outdegree = 0 ;
            nn->indist = j ;
            nn->inback = 0 ;
            nn->inlist = nep ;
            *nep++ = ne ;
            *nep++ = e ;
            nn->outlist = nep ;
            nn->intree = 0 ;
            nn->penalty = 0 ;
            nn->inoffs = lo_zero ;
            nn->outoffs = lo_zero ;
            nn->inarity = 1 ;
            nn->outarity = 1 ;
            ne->bottom = nn ;
            p = e->tail ;
            ne->top = p ;
            e->bottom = nn ;
            for (k=0; k<p->outdegree; k++)
               if (p->outlist[k] == e) {
                  p->outlist[k] = ne ;
                  break ;
               }
            ne++ ;
            nn++ ;
         } else {
            e->top = e->tail ;
            e->bottom = e->head ;
         }
      }
#ifdef DEBUG
      if (lo_debug)
         printf("Node count is %d\n", nncount) ;
#endif
      for (i=0; i<G->num_nodes + 2 * G->back_edge_count; i++) {
         if (i < G->num_nodes)
            pp = G->lo_nodes[i] ;
         else
            pp = G->extra_nodes + (i-G->num_nodes) ;
         for (n=0; n<pp->outarity; n++) {
            olim = (joinpossible ? 1 : pp->outdegree) ;
            for (o = 0; o < olim; o++) {
               reach = pp->indist + 1 ;
               onn = nn ;
               one = ne ;
               for (m=0; m<pp->outdegree; m++) {
                  e = pp->outlist[m] ;
                  e->best_edge = 0 ;
                  if (e && e->inpos == n &&
                               e->bottom->indist > pp->indist + 1 &&
                                                    (joinpossible || o==m)) {
                     e->best_edge = 1 ;
                     if (e->bottom->indist <= reach) {
                        v = G->lo_levels[e->bottom->indist-1] ;
                        p = v->nodes[v->posed-1] ;
                        p->outdegree++ ;
                        e->top = p ;
                        e->inpos = 0 ;
                     } else if (reach > pp->indist + 1) {
                        v = G->lo_levels[reach - 1] ;
                        p = v->nodes[v->posed-1] ;
                        p->outdegree++ ;
                        e->top = p ;
                        e->inpos = 0 ;
                     }
                     for (; reach < e->bottom->indist; reach++) {
                        v = G->lo_levels[reach] ;
                        *ne = *e ; /* copy default fields */
                        nn->simple = 1 ;
                        nn->indegree = 1 ;
                        nn->outdegree = 1 ;
                        nn->indist = reach ;
                        nn->inback = 0 ;
                        nn->inlist = nep ;
                        nn->inoffs = lo_zero ;
                        nn->outoffs = lo_zero ;
                        nn->inarity = 1 ;
                        nn->outarity = 1 ;
                        *nep++ = ne ;
                        ne->inpos = e->inpos ;
                        ne->best_edge = 0 ;
                        e->inpos = 0 ;
                        ne->outpos = 0 ;
          /*            ne->top = e->top ; redundant */
                        ne->bottom = nn ;
                        nn->outdist = v->posed ;
                        nn->intree = 0 ;
                        nn->penalty = 0 ;
                        nn->index = ++nncount ;
                        v->nodes[v->posed++] = nn ;
                        e->top = nn ;
                        ne++ ;
                        nn++ ;
                     }
                  }                  
               }
               while (onn < nn) {
                  onn->outlist = nep ;
                  nep += onn->outdegree ;
                  onn->outdegree = 0 ;
                  onn++ ;
               }
               for (m=0; m<pp->outdegree; m++) {
                  e = pp->outlist[m] ;
                  if (e->best_edge) {
                     p = e->top ;
                     p->outlist[p->outdegree++] = e ;
                     pp->outlist[m] = 0 ;
                     if (p->outdegree > 1)
                        p->simple = 0 ;
                  }
               }
               while (one < ne) {
                  p = one->top ;
                  if (p != pp) {
                     p->outlist[p->outdegree++] = one ;
                     if (p->outdegree > 1)
                        p->simple = 0 ;
                  } else {
                     for (m=pp->outdegree-1; m >= 0; m--)
                        if (pp->outlist[m] == 0) {
                           pp->outlist[m] = one ;
                           break ;
                        }
                     if (m < 0)
                        lo_error("! consistency check; no void edge pointer") ;
                  }
                  one++ ;
               }
            }
         }
         for (n=0, epp = pp->outlist; n<pp->outdegree; n++)
            if (pp->outlist[n])
               *epp++ = pp->outlist[n] ;
         pp->outdegree = epp - pp->outlist ;
      }
      if (nep > onep + 2 * G->num_extras + ss) {
         lo_error("! oops, sure we overwrote something.") ;
      }
#ifdef DEBUG
      if (lo_debug)
         printf("Total nodes = %d, should be %d\n",
                         nncount, G->num_nodes + G->num_extras) ;
#endif
   }
/*
/   We scan the list for all nodes that have indegree of one and outdegree
/   of one; we mark these as simple.
/*/
   for (i=0; i<G->num_nodes; i++) {
      p = G->lo_nodes[i] ;
      if (p->indegree == 1 && p->outdegree == 1)
         p->simple = 1 ;
      else
         p->simple = 0 ;
      p->penalty = 0 ;
      p->intree = 0 ;
   }
/*
/   Finally, a paranoia check; we make sure each level got as many nodes
/   as it was supposed to have.  And we mark the graph as having had no
/   nodes positioned.
/*/
   max_width = 0 ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      if (v->posed != v->width) {
         printf("Level %d: saw %d, expected %d\n",
                                       v->indist, v->posed, v->width) ;
         lo_error("! consistency error when building extended graph") ;
      }
      v->posed = 0 ;
      if (v->width > max_width)
         max_width = v->width ;
   }
   i = 2 * (max_width + G->graph_height + 2) + G->num_nodes + G->num_extras ;
   G->mwars = (long *)(G->mymalloc)(i * (sizeof(long) + sizeof(short))) ;
   G->mpaths = (short *)(G->mwars + i) ;
}
/*
/   This recursive routine attempts to go from a particular node to a
/   given level and find the cost at each level.  It adds the result to
/   the passed array.  It works down from the top.
/*/
#define MFUNC(s) ((s+100)*s)
#ifdef CPLUS
STATIC void find_path_costs_up(struct lo_node *p, int pos,
   struct lo_level *v, long *cost_array, short *path_array)
#else
STATIC void find_path_costs_up(p, pos, v, cost_array, path_array)
struct lo_node *p ;
int pos ;
struct lo_level *v ;
long *cost_array ;
short *path_array ;
#endif
{
   int i, j, k, s, ss ;
   struct lo_node *q ;
   struct lo_edge *e ;
   long *next_costs = cost_array + v->posed + 1 ;
   short *next_path = path_array + v->posed + 1 ;
   struct lo_level *vv = G->lo_levels[v->indist-1] ;

   if (vv->indist <= p->indist) {
      ss = 0 ;
      for (i=0; i<p->outdist; i++) {
         q = vv->nodes[i] ;
         for (k=0; k<q->outdegree; k++)
            if (q->outlist[k]->bottom->intree)
               ss++ ;
      }
      for (i=0; i<p->outdegree; i++) {
         e = p->outlist[i] ;
         if (e->bottom->intree && e->inpos < pos)
            ss++ ;
      }
      if (v->posed && p->outarity > 1) {
         k = 5 * (v->posed + 1) * pos / (p->outarity - 1) ;
         for (j=0; j<=v->posed; j++) {
            s = 5 * j - k ;
            if (s >= 0)
               cost_array[j] = s ;
            else
               cost_array[j] = -s ;
         }
      } else {
         for (j=0; j<=v->posed; j++)
            cost_array[j] = 0 ;
      }
      s = ss ;
      for (j=0; j<v->posed; j++) {
         k = MFUNC(s) ;
         cost_array[j] += k ;
#ifdef DEBUG
         if (lo_debug)
            printf("[%d]", cost_array[j]) ;
#endif
         path_array[j] = p->outdist ;
         q = v->nodes[j] ;
         for (k=0; k<q->indegree; k++) {
            e = q->inlist[k] ;
            if (e->top->intree)
               if (e->top->outdist > p->outdist)
                  s += 1 ;
               else if (e->top->outdist < p->outdist)
                  s -= 1 ;
               else if (e->inpos > pos)
                  s += 1 ;
               else if (e->inpos < pos)
                  s -= 1 ;
         }
      }
      k = MFUNC(s) ;
      cost_array[j] += k ;
#ifdef DEBUG
      if (lo_debug)
         printf("[%d]\n", cost_array[j]) ;
#endif
      path_array[j] = p->outdist ;
      return ;
   }
   find_path_costs_up(p, pos, vv, next_costs, next_path) ;
#ifdef DEBUG
   if (lo_debug)
      printf("Finding up costs from %d to %d\n", p->indist, v->indist) ;
#endif
   for (i=0; i<=v->posed; i++)
      cost_array[i] = 1000000000 ;
   ss = 0 ;
   for (i=0; i<=vv->posed; i++) {
      s = ss ;
      for (j=0; j<v->posed; j++) {
         k = MFUNC(s) ;
#ifdef DEBUG
         if (lo_debug)
            printf("[%d]", k) ;
#endif
         if (next_costs[i] + k < cost_array[j]) {
            cost_array[j] = next_costs[i] + k ;
            path_array[j] = i ;
         }
         q = v->nodes[j] ;
         for (k=0; k<q->indegree; k++) {
            e = q->inlist[k] ;
            if (e->top->intree)
               if (e->top->outdist >= i)
                  s += 1 ;
               else
                  s -= 1 ;
         }
      }
      k = MFUNC(s) ;
#ifdef DEBUG
      if (lo_debug)
         printf("[%d]\n", k) ;
#endif
      if (next_costs[i] + k < cost_array[j]) {
         cost_array[j] = next_costs[i] + k ;
         path_array[j] = i ;
      }
      if (i < vv->posed) {
         q = vv->nodes[i] ;
         for (k=0; k<q->outdegree; k++)
            if (q->outlist[k]->bottom->intree)
               ss++ ;
      }
   }
}
/*
/   This routine is the converse of the one immediately above it.
/*/
#ifdef CPLUS
STATIC void find_path_costs_down(struct lo_node *p, int pos,
   struct lo_level *v, long *cost_array, short *path_array)
#else
STATIC void find_path_costs_down(p, pos, v, cost_array, path_array)
struct lo_node *p ;
int pos ;
struct lo_level *v ;
long *cost_array ;
short *path_array ;
#endif
{
   int i, j, k, s, ss ;
   struct lo_node *q ;
   struct lo_edge *e ;
   long *next_costs = cost_array + v->posed + 1 ;
   short *next_path = path_array + v->posed + 1 ;
   struct lo_level *vv = G->lo_levels[v->indist+1] ;

   if (vv->indist >= p->indist) {
      ss = 0 ;
      for (i=0; i<p->outdist; i++) {
         q = vv->nodes[i] ;
         for (k=0; k<q->indegree; k++)
            if (q->inlist[k]->top->intree)
               ss++ ;
      }
      for (i=0; i<p->indegree; i++) {
         e = p->inlist[i] ;
         if (e->top->intree && e->outpos < pos)
            ss++ ;
      }
      if (v->posed && p->inarity > 1) {
         k = 5 * (v->posed + 1) * pos / (p->inarity - 1) ;
         for (j=0; j<=v->posed; j++) {
            s = 5 * j - k ;
            if (s >= 0)
               cost_array[j] = s ;
            else
               cost_array[j] = -s ;
         }
      } else {
         for (j=0; j<=v->posed; j++)
            cost_array[j] = 0 ;
      }
      s = ss ;
      for (j=0; j<v->posed; j++) {
         k = MFUNC(s) ;
         cost_array[j] += k ;
#ifdef DEBUG
         if (lo_debug)
            printf("[%d]", cost_array[j]) ;
#endif
         path_array[j] = p->outdist ;
         q = v->nodes[j] ;
         for (k=0; k<q->outdegree; k++) {
            e = q->outlist[k] ;
            if (e->bottom->intree)
               if (e->bottom->outdist > p->outdist)
                  s += 1 ;
               else if (e->bottom->outdist < p->outdist)
                  s -= 1 ;
               else if (e->outpos > pos)
                  s += 1 ;
               else if (e->outpos < pos)
                  s -= 1 ;
         }
      }
      k = MFUNC(s) ;
      cost_array[j] += k ;
#ifdef DEBUG
      if (lo_debug)
         printf("[%d]\n", cost_array[j]) ;
#endif
      path_array[j] = p->outdist ;
      return ;
   }
   find_path_costs_down(p, pos, vv, next_costs, next_path) ;
#ifdef DEBUG
   if (lo_debug)
      printf("Finding down costs from %d to %d\n", p->indist, v->indist) ;
#endif
   for (i=0; i<=v->posed; i++)
      cost_array[i] = 1000000000 ;
   ss = 0 ;
   for (i=0; i<=vv->posed; i++) {
      s = ss ;
      for (j=0; j<v->posed; j++) {
         k = MFUNC(s) ;
#ifdef DEBUG
         if (lo_debug)
            printf("[%d]", k) ;
#endif
         if (next_costs[i] + k < cost_array[j]) {
            cost_array[j] = next_costs[i] + k ;
            path_array[j] = i ;
         }
         q = v->nodes[j] ;
         for (k=0; k<q->outdegree; k++) {
            e = q->outlist[k] ;
            if (e->bottom->intree)
               if (e->bottom->outdist >= i)
                  s += 1 ;
               else
                  s -= 1 ;
         }
      }
      k = MFUNC(s) ;
#ifdef DEBUG
      if (lo_debug)
         printf("[%d]\n", k) ;
#endif
      if (next_costs[i] + k < cost_array[j]) {
         cost_array[j] = next_costs[i] + k ;
         path_array[j] = i ;
      }
      if (i < vv->posed) {
         q = vv->nodes[i] ;
         for (k=0; k<q->indegree; k++)
            if (q->inlist[k]->top->intree)
               ss++ ;
      }
   }
}
/*
/   This routine finds the best way to route an extended edge, and
/   does so.  Note that |p| is always higher than |q|, and that |p|
/   is never simple while |q| always is.  If |q| is 0 we don't do
/   the routing.
/*/
#ifdef CPLUS
STATIC void y_place(struct lo_node *, int) ;
STATIC void add_best_edge(struct lo_node *p, int pos, struct lo_node *q)
#else
STATIC void y_place() ;
STATIC void add_best_edge(p, pos, q)
struct lo_node *p ;
int pos ;
struct lo_node *q ;
#endif
{
   int i, j, bv, b ;
   short *paths ;
   struct lo_level *v ;
   long *next_costs ;
   struct lo_edge *e ;

   if (q) {
#ifdef DEBUG
      if (lo_debug)
         printf("Searching for best edge from %d to %d\n", p->index, q->index) ;
#endif
      j = q->indist ;
      v = G->lo_levels[j] ;
      next_costs = G->mwars + v->posed + 1 ;
      e = q->outlist[0] ;
      find_path_costs_down(e->bottom, e->outpos, v, G->mwars, G->mpaths) ;
      find_path_costs_up(p, pos, v, next_costs, G->mpaths) ;
      bv = 1000000000 ;
      for (i=0; i<=v->posed; i++) {
         next_costs[i] += G->mwars[i] ;
         if (next_costs[i] < bv) {
            bv = next_costs[i] ;
            b = i ;
         }
      }
#ifdef DEBUG
      if (lo_debug)
         printf("Best edge path has cost of %d\n", bv) ;
#endif
      paths = G->mpaths ;
      while (q->simple) {
         y_place(q, b) ;
         q = q->inlist[0]->top ;
         b = paths[b] ;
         paths += v->posed ;
                  /* the +1 is already there because of the added node */
         j-- ;
         v = G->lo_levels[j] ;
      }
   }
}
/*
/   This piece of code does the minor housekeeping associated with
/   placing a node at a particular position.  We shift all the
/   previously positioned nodes to the right to make room, updating
/   their position information.  We increment the `penalty' of all
/   connected nodes and we mark the node as placed.  We also route
/   each extended edge that is now fully connected.
/*/
#ifdef CPLUS
STATIC void y_place(struct lo_node *p, int n)
#else
STATIC void y_place(p, n)
struct lo_node *p ;
int n ;
#endif
{
   struct lo_level *v = G->lo_levels[p->indist] ;
   struct lo_node *q, *pp = p ;
   int ppos = p->outdist ;
   struct lo_edge *e ;

#ifdef DEBUG
   if (lo_debug)
      printf("Placing node %d level %d at %d\n", p->index, p->indist, n) ;
#endif
   if (p->intree)
      lo_error("! consistency error; node placed again") ;
   if (n > v->posed)
      lo_error("! consistency error; node placed too far right") ;
   if (v->posed >= v->width)
      lo_error("! consistency error; too many nodes placed") ;
   if (ppos < v->posed)
      lo_error("! consistency error; node already placed") ;
   if (ppos != n) {
      if (n < v->posed) {
         while (n <= v->posed) {
            q = v->nodes[n] ;
            v->nodes[n++] = pp ;
            pp = q ;
         }
         if (ppos >= n)
            v->nodes[ppos] = pp ;
         for (n=0; n<v->width; n++)
            v->nodes[n]->outdist = n ;
      } else {
         q = v->nodes[n] ;
         v->nodes[n] = pp ;
         v->nodes[ppos] = q ;
         q->outdist = ppos ;
         p->outdist = n ;
      }
   }
   v->posed++ ;
   p->intree = 1 ;
   if (p->simple == 0) {
      int i ;

      for (i=0; i<p->indegree; i++) {
         e = p->inlist[i] ;
         q = e->top ;
         pp = 0 ;
         while (1) {
            if (q->simple == 0)
               break ;
            if (pp == 0)
               pp = q ;
            e = q->inlist[0] ;
            q = e->top ;
         }
         if (q->intree)
            add_best_edge(q, e->inpos, pp) ;
         else if (q->indegree + q->outdegree == 1)
            q->penalty += 100 ;
         else
            q->penalty += 20 ;
      }
      for (i=0; i<p->outdegree; i++) {
         e = p->outlist[i] ;
         q = e->bottom ;
         pp = 0 ;
         while (1) {
            if (q->simple == 0)
               break ;
            pp = q ;
            q = q->outlist[0]->bottom ;
         }
         if (q->intree)
            add_best_edge(p, e->inpos, pp) ;
         else if (q->indegree + q->outdegree == 1)
            q->penalty += 100 ;
         else
            q->penalty += 20 ;
      }
   }
}
/*
/   This routine attempts to place a single node.  It does this by
/   finding all edges that lead (directly or indirectly through
/   simple nodes) to nodes that have already been placed.  It then
/   tries to find the best `route' for each such edge.  The final
/   position of the node depends on finding the least total `cost'
/   of these edges.
/*/
#ifdef CPLUS
STATIC void find_optimal(struct lo_node *p)
#else
STATIC void find_optimal(p)
struct lo_node *p ;
#endif
{
   struct lo_node *q ;
   int i, j, b, bv, fc ;
   long *cost_array = G->mwars ;
   struct lo_level *v = G->lo_levels[p->indist] ;
   long *next_cost = G->mwars + v->posed + 1 ;
   struct lo_edge *e ;

#ifdef DEBUG
   if (lo_debug)
      printf("Looking for optimum placement for node %d\n", p->index) ;
#endif
   for (i=0; i<=v->posed; i++)
      cost_array[i] = 0 ;
   fc = 0 ;
   bv = 0 ;
   for (i=0; i<p->indegree; i++) {
      e = p->inlist[i] ;
      q = e->top ;
      while (1) {
         if (q->simple == 0)
            break ;
         e = q->inlist[0] ;
         q = e->top ;
      }
      if (q->intree) {
         bv += p->inlist[i]->outpos ;
         fc += 1 ;
         find_path_costs_up(q, e->inpos, v, next_cost, G->mpaths) ;
         for (j=0; j<=v->posed; j++)
            cost_array[j] += next_cost[j] ;
      }
   }
   if (p->inarity > 1 && fc > 0 && v->posed > 0) {
      bv = v->posed + 1 - 3 * bv * (v->posed + 1) / (fc * (p->inarity - 1)) ;
      for (j=0; j<=v->posed; j++) {
         fc = bv - j * 3 ;
         if (fc >= 0)
            cost_array[j] += fc ;
         else
            cost_array[j] -= fc ;
      }
   }
   bv = 0 ;
   fc = 0 ;
   for (i=0; i<p->outdegree; i++) {
      e = p->outlist[i] ;
      q = e->bottom ;
      while (1) {
         if (q->simple == 0)
            break ;
         e = q->outlist[0] ;
         q = e->bottom ;
      }
      if (q->intree) {
         bv += p->outlist[i]->inpos ;
         fc += 1 ;
         find_path_costs_down(q, e->outpos, v, next_cost, G->mpaths) ;
         for (j=0; j<=v->posed; j++)
            cost_array[j] += next_cost[j] ;
      }
   }
   if (p->outarity > 1 && fc > 0 && v->posed > 0) {
      bv = v->posed + 1 - 3 * bv *(v->posed + 1) / (fc * (p->outarity - 1)) ;
      for (j=0; j<=v->posed; j++) {
         fc = bv - j * 3 ;
         if (fc >= 0)
            cost_array[j] += fc ;
         else
            cost_array[j] -= fc ;
      }
   }
   for (i=0, bv=1000000000; i<=v->posed; i++) {
      if (cost_array[i] < bv) {
         b = i ;
         bv = cost_array[i] ;
      }
   }
#ifdef DEBUG
   if (lo_debug)
      printf("Best placement value is %d\n", bv) ;
#endif
   y_place(p, b) ;
}
/*
/   This routine orders each level to minimize the number of crossing
/   edges and.
/
/   We use a dynamic algorithm that progresses from the top level down to
/   the bottom, finding the path through the graph that yields the maximum
/   value.  As we compute, we store our path in the `next' pointers.
/*/
STATIC void order_levels(VOID) {
   long *m = G->mwars ;
   long *n = G->mwars + G->graph_height + 1 ;
   int i, j, k, b, val ;
   struct lo_level *v ;
   struct lo_node *p, *g ;

   if (G->graph_height)
      for (i=0; i<G->lo_levels[0]->width; i++)
         m[i] = 0 ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         p->next = 0 ;
         if (p->simple)
            val = 1 ;
         else
            val = 10 * (p->indegree + p->outdegree) ;
         b = 0 ;
         for (k=0; k<p->indegree; k++) {
            g = p->inlist[k]->top ;
            if (g->indist != i-1) {
               printf("Node %d(%d) at level %d points to node %d at level %d\n",
                             p->index, p, p->indist, g->index, g->indist) ;
               lo_error("! consistency error while finding long path") ;
            }
            if (m[g->outdist] > b) {
               b = m[g->outdist] ;
               p->next = g ;
            }
         }
         n[j] = b + val ;
      }
      { long *t = m ; m = n ; n = t ; }
   }
   b = -1 ;
   p = 0 ;
   for (i=G->graph_height-2; i<G->graph_height; i++) {
      if (i > 0) {
         v = G->lo_levels[i] ;
         for (j=0; j<v->width; j++)
            if (n[j] >= b) {
               p = v->nodes[j] ;
               b = n[j] ;
            }
      }
      { long *t = m ; m = n ; n = t ; }
   }
#ifdef DEBUG
   if (lo_debug) {
      printf("Chain:") ;
      for (g=p; g; g = g->next)
         printf(" %d", g->index) ;
      printf("\n") ;
   }
#endif
   for (; p; p = p->next)
      if (p->simple == 0)
         p->penalty += 50 ;
/*
/   Next we attempt to find the best position for the rest of the
/   nodes in the graph.  We scan the entire node list looking for a
/   node that isn't simple and isn't already in the tree, trying to
/   maximize both the connectivity to the already built graph and the
/   total number of edges coming out of it.
/*/
   while (1) {
      b = -10 ;
      g = 0 ;
      for (i=0; i<G->num_nodes; i++) {
         p = G->lo_nodes[i] ;
         if (p->simple == 0 && p->intree == 0) {
            val = p->penalty + p->indegree + p->outdegree ;
            if (val > b) {
               b = val ;
               g = p ;
            }
         }
      }
      for (p=G->extra_nodes, i=G->num_extras; i>0; i--, p++) {
         if (p->simple == 0 && p->intree == 0) {
            val = p->penalty + p->indegree + p->outdegree ;
            if (val > b) {
               b = val ;
               g = p ;
            }
         }
      }
      if (g == 0)
         break ;
      find_optimal(g) ;
   }
}
/*
/   Here we have a simple algorithm to position the nodes correctly
/   horizontally, once their order is decided.  But first we need to
/   get from the user information about the widths of each node.
/   This routine can be called again and again, passing different
/   widths, for the different cases of displaying and printing (or
/   to adjust the font size or whatnot.)
/*/
#ifdef CPLUS
void layout_adjust(void *g, int (*getwidth)(void *),
   int (*getheight)(void *), void (*getoffsets)(void *, short *, short *),
   struct layout_adjust_params *lap, int *finalw, int *finalh)
#else
void layout_adjust(g, getwidth, getheight, getoffsets, lap, finalw, finalh)
struct lo_graph *g ;
int (*getwidth)(), (*getheight)() ;
void (*getoffsets)() ;
struct layout_adjust_params *lap ;
int *finalw, *finalh ;
#endif
{
   int i, j, k, ii, jj, x ;
   int min, max, h, hh ;
   struct lo_node *p ;
   struct lo_level *v ;
   struct lo_edge *e ;
   int gchanged ;

   G = g ;
   if (lap == 0)
      lap = &lo_lap ;
   else {
      if (lap->def_width == 0)
         lap->def_width = lo_lap.def_width ;
      if (lap->def_height == 0)
         lap->def_height = lo_lap.def_height ;
      if (lap->min_good_slope == 0)
         lap->min_good_slope = lo_lap.min_good_slope ;
      if (lap->min_slope == 0)
         lap->min_slope = lo_lap.min_slope ;
   }
   G->node_height = lap->def_height ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      v->indegree = 0 ;
      v->outdegree = 0 ;
      v->maxdiff = 0 ;
   }
   for (i=0; i<G->num_nodes; i++) {
      p = G->lo_nodes[i] ;
      p->penalty = getwidth(p->user_data) + lap->def_width ;
      if (getheight)
         p->inback = getheight(p->user_data) ;
      else
         p->inback = lap->def_height ;
      if (getoffsets) /* don't abbreviate any further */
         getoffsets(p->user_data, p->inoffs, p->outoffs) ;
      v = G->lo_levels[p->indist] ;
      v->indegree += p->indegree ;
      v->outdegree += p->outdegree ;
   }
   for (i=0, p=G->extra_nodes; i<G->num_extras; i++, p++) {
      p->penalty = lap->def_width ;
      v = G->lo_levels[p->indist] ;
      v->indegree += p->indegree ;
      v->outdegree += p->outdegree ;
   }
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      x = 0 ;
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         p->outdist = x + p->penalty ;
         x += 2 * p->penalty ;
      }
   }
   for (jj=0; jj<4; jj++) {
      for (i=0; i<G->graph_height; i++) {
         min = 1000000000 ;
         max = -1000000000 ;
         v = G->lo_levels[i] ;
         for (j=0; j<v->width; j++) {
            p = v->nodes[j] ;
            for (k=0; k<p->indegree; k++) {
               e = p->inlist[k] ;
               x = (p->outdist + p->inoffs[e->outpos]) -
                   (e->top->outdist + e->top->outoffs[e->inpos]) ;
               if (x < min)
                  min = x ;
               if (x > max)
                  max = x ;
            }
         }
         if (v->indegree) {
            x = (min + max) / 2 ;
            for (j=0; j<v->width; j++)
               v->nodes[j]->outdist -= x ;
            v->maxdiff = max - min ;
         } else
            v->maxdiff = 0 ;
         if (i == G->graph_height-1)
            j = i ;
         else
            j = i-1 ;
         for (; j>=0; j--) {
            gchanged = 0 ;
            v = G->lo_levels[j] ;
            for (k=0; k<v->width; k++) {
               p = v->nodes[k] ;
               if (k != 0 && k != v->width - 1 &&
                  v->nodes[k+1]->outdist - v->nodes[k-1]->outdist
                     - v->nodes[k+1]->penalty - v->nodes[k-1]->penalty <=
                      2 * p->penalty)
                  continue ;
               min = 1000000000 ;
               max = -1000000000 ;
               for (ii=0; ii<p->indegree; ii++) {
                  e = p->inlist[ii] ;
                  x = - (p->outdist + p->inoffs[e->outpos]) +
                      (e->top->outdist + e->top->outoffs[e->inpos]) ;
                  if (x < min)
                     min = x ;
                  if (x > max)
                     max = x ;
               }
               h = min + max ;
               min = 1000000000 ;
               max = -1000000000 ;
               for (ii=0; ii<p->outdegree; ii++) {
                  e = p->outlist[ii] ;
                  x = (e->bottom->outdist + e->bottom->inoffs[e->outpos]) -
                      (p->outdist + p->outoffs[e->inpos]) ;
                  if (x < min)
                     min = x ;
                  if (x > max)
                     max = x ;
               }
               if (j < G->graph_height-1) {
                  x = (h * (long)G->lo_levels[j+1]->maxdiff +
                       (min + max) * (long)v->maxdiff) /
                         (2 * (G->lo_levels[j+1]->maxdiff + v->maxdiff) + 1) ;
                  x = (jj * x + (min + max + h + 2) * (3 - jj) / 4 + 2) / 3 ;
               } else
                  x = (min + max + h + 2) / 4 ;
               if (k > 0) {
                  h = v->nodes[k-1]->outdist + v->nodes[k-1]->penalty +
                             p->penalty ;
                  if (p->outdist + x < h)
                     x = h - p->outdist ;
               }
               if (k < v->width-1) {
                  h = v->nodes[k+1]->outdist - v->nodes[k+1]->penalty -
                             p->penalty ;
                  if (p->outdist + x > h)
                     x = h - p->outdist ;
               }
               if (x != 0) {
                  p->outdist += x ;
                  gchanged++ ;
               }
            }
            for (k=v->width-1; k >= 0; k--) {
               if (k != 0 && k != v->width - 1 &&
                  v->nodes[k+1]->outdist - v->nodes[k-1]->outdist
                     - v->nodes[k+1]->penalty - v->nodes[k-1]->penalty <=
                      2 * p->penalty)
                  continue ;
               p = v->nodes[k] ;
               min = 1000000000 ;
               max = -1000000000 ;
               for (ii=0; ii<p->indegree; ii++) {
                  e = p->inlist[ii] ;
                  x = - (p->outdist + p->inoffs[e->outpos]) +
                      (e->top->outdist + e->top->outoffs[e->inpos]) ;
                  if (x < min)
                     min = x ;
                  if (x > max)
                     max = x ;
               }
               h = min + max ;
               min = 1000000000 ;
               max = -1000000000 ;
               for (ii=0; ii<p->outdegree; ii++) {
                  e = p->outlist[ii] ;
                  x = (e->bottom->outdist + e->bottom->inoffs[e->outpos]) -
                      (p->outdist + p->outoffs[e->inpos]) ;
                  if (x < min)
                     min = x ;
                  if (x > max)
                     max = x ;
               }

               if (j < G->graph_height-1) {
                  x = (h * (long)G->lo_levels[j+1]->maxdiff +
                       (min + max) * (long)v->maxdiff) /
                         (2 * (G->lo_levels[j+1]->maxdiff + v->maxdiff) + 1) ;
                  x = (jj * x + (min + max + h + 2) * (3 - jj) / 4 + 2) / 3 ;
               } else
                  x = (min + max + h + 2) / 4 ;
               if (k > 0) {
                  h = v->nodes[k-1]->outdist + v->nodes[k-1]->penalty +
                             p->penalty ;
                  if (p->outdist + x < h)
                     x = h - p->outdist ;
               }
               if (k < v->width-1) {
                  h = v->nodes[k+1]->outdist - v->nodes[k+1]->penalty -
                             p->penalty ;
                  if (p->outdist + x > h)
                     x = h - p->outdist ;
               }
               if (x != 0) {
                  p->outdist += x ;
                  gchanged++ ;
               }
            }
            if (gchanged == 0)
               break ;
         }
      }
   }
   for (i=0; i<G->graph_height; i++) {
      min = 1000000000 ;
      max = -1000000000 ;
      v = G->lo_levels[i] ;
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         for (k=0; k<p->indegree; k++) {
            e = p->inlist[k] ;
            x = (p->outdist + p->inoffs[e->outpos]) -
                (e->top->outdist + e->top->outoffs[e->inpos]) ;
            if (x < min)
               min = x ;
            if (x > max)
               max = x ;
         }
      }
      if (v->indegree) {
         x = (min + max) / 2 ;
         for (j=0; j<v->width; j++)
            v->nodes[j]->outdist -= x ;
      }
   }
   min = 1000000000 ;
   max = -1000000000 ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      p = v->nodes[0] ;
      x = p->outdist - p->penalty ;
      if (x < min)
         min = x ;
      p = v->nodes[v->width-1] ;
      x = p->outdist + p->penalty ;
      if (x > max)
         max = x ;
   }
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      for (j=0; j<v->width; j++)
         v->nodes[j]->outdist -= min ;
   }
   max -= min ;
   *finalw = (max + 1) / 2 ;
   h = 0 ;
   hh = 0 ;
   /* first we calculate things with the given slope. */
   for (i=0; i<G->graph_height; i++) {
      max = 0 ;
      v = G->lo_levels[i] ;
      min = 0 ;
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         if (p->inback > max)
            max = p->inback ;
         for (k=0; k<p->indegree; k++) {
            e = p->inlist[k] ;
            x = (p->outdist + p->inoffs[e->outpos]) -
                (e->top->outdist + e->top->outoffs[e->inpos]) ;
            if (x < 0)
               x = -x ;
            if (x > min)
               min = x ;
         }
      }
      v->maxdiff = min ;
      v->levheight = max ;
      if (i == 0 || min / lap->min_good_slope > lap->min_level_separation)
         h += min / lap->min_good_slope + max ;
      else
         h += lap->min_level_separation + max ;
      if (i > 0)
         hh += lap->min_level_separation + max ;
      else
         hh += max ;
   }
   /*  Now we try to optimize things. */
   if (lap->goal_height && h > lap->goal_height &&
       lap->min_slope > lap->min_good_slope) {
      if (hh > lap->goal_height)
         j = lap->min_slope ;
      else
         for (j=lap->min_good_slope + 1; j < lap->min_slope; j++) {
#ifdef DEBUG
            if (lo_debug)
               printf("Squeezing; trying slope of 2/%d\n", j) ;
#endif
            h = 0 ;
            for (i=0; i<G->graph_height; i++) {
               v = G->lo_levels[i] ;
               if (i == 0 || v->maxdiff / j > lap->min_level_separation)
                  h += v->maxdiff / j + v->levheight ;
               else
                  h += lap->min_level_separation + v->levheight ;
            }
            if (h <= lap->goal_height)
               break ;
      }
   } else
      j = lap->min_good_slope ;
   h = 0 ;
   hh = j ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      if (i == 0 || v->maxdiff / hh > lap->min_level_separation)
         v->maxdiff /= hh ;
      else
         v->maxdiff = lap->min_level_separation ;
      h += v->maxdiff + v->levheight ;
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         if (p->index > G->num_nodes)
            if (lap->vias_have_height)
               p->inback = v->levheight ;
            else
               p->inback = 0 ;
      }
   }
   *finalh = h ;
}
/*
/   Draws a graph, from routines passed in by the user.
/*/
#ifdef CPLUS
void layout_draw(void *g, void (*drawnode)(void *, int x, int y),
   void (*drawedge)(void *, int x1, int y1, int x2, int y2, char backedge))
#else
void layout_draw(g, drawnode, drawedge)
struct lo_graph *g ;
void (*drawnode)(), (*drawedge)() ;
#endif
{
   int i, j, k, y, ny ;
   int x1, y1, x2, y2 ;
   struct lo_node *p, *q ;
   struct lo_level *v ;
   struct lo_edge *e ;

   G = g ;
   y = G->lo_levels[0]->levheight / 2 ;
   ny = 0 ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      if (i > 0)
         y += v->maxdiff ;
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         x1 = (p->outdist + 1) / 2 ;
         if (p->index < G->num_nodes)
            drawnode(p->user_data, x1, y) ;
      }
      for (j=0; j<v->width; j++) {
         p = v->nodes[j] ;
         y1 = y - p->inback / 2 ;
         if (p->index >= G->num_nodes && p->inback &&
                  p->outdegree > 0 && p->indegree > 0) {
            x1 = (p->outdist + 1) / 2 ;
            e = p->outlist[0] ;
            drawedge(e->user_data, x1, y1, x1, y1 + p->inback, e->back_edge) ;
         }
         for (k=0; k<p->indegree; k++) {
            e = p->inlist[k] ;
            q = e->top ;
            x1 = (p->outdist + 1) / 2 + p->inoffs[e->outpos] ;
            x2 = (q->outdist + 1) / 2 + q->outoffs[e->inpos] ;
            y2 = ny + (q->inback + 1) / 2 + 
                                        q->outoffs[e->inpos + q->outarity] ;
            drawedge(e->user_data, x1, y1 + p->inoffs[e->outpos + p->inarity],
                                   x2, y2, e->back_edge) ;
         }
      }
      ny = y ;
      if (i < G->graph_height-1)
         y += (v->levheight + v[1].levheight + 1) / 2 ;
   }
}
/*
/   The whole kit and kaboodle.
/*/
#ifdef CPLUS
void *layout_graph_p(int n,
   void (*fillnodearray)(void **nodearr),
   void (*filledgearray)(void *node, int *incnt, int *outcnt, int *inarity,
      int *outarity, int *succarr),
   void (*filledgeptrs)(void *node, void **varr),
   void (*filledgeio)(void *node, short *io), void *(*malloc)(int n),
   struct layout_adjust_params *lap)
#else
void *layout_graph_p(n, fillnodearray, filledgearray, filledgeptrs, filledgeio,
   malloc, lap)
int n ;
void (*fillnodearray)(), (*filledgearray)(), (*filledgeptrs)(),
     (*filledgeio)(), *(*malloc)() ;
struct layout_adjust_params *lap ;
#endif
{
   static char *foo = BANNER ;
   if (foo) {
      printf("%s\n", foo) ;
#ifdef DEBUG
      if (lo_debug) {
         printf("Size of node = %d Size of edge = %d\n",
                 4 + sizeof(struct lo_node), 12 + sizeof(struct lo_edge)) ;
      }
#endif
      foo = 0 ;
   }
   lo_init(n, fillnodearray, filledgearray, filledgeptrs, filledgeio,
         malloc) ;
#ifdef DEBUG
   if (lo_debug)
      printf("Assigning levels . . .\n") ;
#endif
   assign_levels() ;
#ifdef DEBUG
   if (lo_debug)
      printf("Figuring widths and remaking graph . . .\n") ;
#endif
   figure_widths(lap) ;
#ifdef DEBUG
   if (lo_debug)
      printf("Ordering levels . . .\n") ;
#endif
   order_levels() ;
   return (void *)G ;
}
#ifdef CPLUS
void *layout_graph(int n,
   void (*fillnodearray)(void **nodearr),
   void (*filledgearray)(void *node, int *incnt, int *outcnt, int *inarity,
      int *outarity, int *succarr),
   void (*filledgeptrs)(void *node, void **varr),
   void (*filledgeio)(void *node, short *io), void *(*malloc)(int n))
#else
void *layout_graph(n, fillnodearray, filledgearray, filledgeptrs, filledgeio,
   malloc)
int n ;
void (*fillnodearray)(), (*filledgearray)(), (*filledgeptrs)(),
     (*filledgeio)(), *(*malloc)() ;
#endif
{
   return layout_graph_p(n, fillnodearray, filledgearray, filledgeptrs,
                         filledgeio, malloc, 0) ;
}
/*
/   The cleanup routine.  Note that edges are allocated as parts of the
/   nodes to try to minimize the memory allocations required.
/*/
#ifdef CPLUS
void layout_clean(void *g)
#else
void layout_clean(g)
void *g ;
#endif
{
   if (g==0)
      return ;
   G = (struct lo_graph *)g ;
   while (G->num_nodes-- > 0)
      free(G->lo_nodes[G->num_nodes]) ;
   if (G->lo_nodes) free(G->lo_nodes) ;
   if (G->lo_edges) free(G->lo_edges) ;
   if (G->lo_levels) free(G->lo_levels) ;
   if (G->extra_stuff) free(G->extra_stuff) ;
   if (G->mwars) free(G->mwars) ;
   free(G) ;
}
/*
/   This function locates a node given an (x,y) position, if the node is
/   within 3.
/*/
#ifdef CPLUS
void *layout_find_node(struct lo_graph *g, int x, int y)
#else
void *layout_find_node(g, tx, ty)
struct lo_graph *g ;
int tx, ty ;
#endif
{
   int i, j ;
   int x, y ;
   struct lo_level *v, *vv = 0 ;
   int off = 1000, err ;
   struct lo_node *p, *pp ;

   G = g ;
   y = G->lo_levels[0]->levheight / 2 ;
   for (i=0; i<G->graph_height; i++) {
      v = G->lo_levels[i] ;
      if (i > 0)
         y += v->maxdiff ;
      if (y < ty)
         err = ty - y - (v->levheight / 2) ;
      else
         err = y - ty - (v->levheight / 2) ;
      if (err < 0)
         err = 0 ;
      if (err < off) {
         off = err ;
         vv = v ;
      }
      if (i < G->graph_height-1)
         y += (v->levheight + v[1].levheight + 1) / 2 ;
   }
   if (vv == 0 || off > 3)
      return 0 ;
   off = 1000 ;
   pp = 0 ;
   v = vv ;
   for (j=0; j<v->width; j++) {
      p = v->nodes[j] ;
      if (p->index < G->num_nodes) {
         x = (p->outdist + 1) / 2 ;
         if (x < tx)
            err = tx - x - (p->penalty / 2) ;
         else
            err = x - tx - (p->penalty / 2) ;
         if (err < 0)
            err = 0 ;
         if (err < off) {
            off = err ; 
            pp = p ;
	 }
      }
   }
   if (pp == 0 || off > 3)
      return 0 ;
   else
      return pp->user_data ;
}
