/*
/   This file defines the structure that is used to pass in parameters
/   to |layout\_adjust()|.  It is optional; if such a structure is not
/   supplied, appropriate defaults will be used instead.  Each of these
/   can be defaulted just be setting them to zero (and if you do this,
/   the |layout\_adjust()| routine will change them to the defaults, so
/   the structure had better be writeable.)
/*/
struct layout_adjust_params {
   int goal_height, goal_width ;
   int def_height ;
   int def_width ;
   int min_good_slope ;
   int min_slope ;
   int min_level_separation ;
   char vias_have_height ;
   char joinpossible ;
   char pad2, pad3 ;
   int more_params[8] ;
} ;
/*
/   The |goal\_height| and |goal\_width| parameters tell |layout\_adjust()|
/   what a maximum `good' |width| and |height| are; if at all possible,
/   the system will try to layout the graph to fit.  (With this release,
/   only |goal\_height| is used.)  The |def\_height| parameter is the height
/   to assume for nodes for which no height is given.
/   The |def\_width| parameter is the amount
/   of space to add between nodes or between edges that traverse a particular
/   level; this may need to be increased if the slopes are increased and
/   vias are not allowed to have height.
/
/   The |min\_good\_slope| parameter restricts the minimum slope of edges
/   between levels; the actual slope is $2/|min\_good\_slope|$.  The
/   |min\_slope| is normally set to a higher number and indicates the
/   absolute minimum slope that will be tolerated if the graph needs to be
/   squeezed in order to fit in |goal\_height|.
/
/   The |min\_level\_separation| marks the closest together that two levels
/   can come.  Typically this is on the order of |def\_height|.
/
/   The |vias\_have\_height| parameter is a flag indicating whether or not
/   to give edges that traverse a level a `height' or not.  Turning on this
/   flag makes long edges somewhat more `jaggy' than they otherwise would
/   be, but eliminates the problem of edges of high slope running into
/   adjacent nodes.
/
/   For convenience, we also forward declare the functions made available in
/   |layout.c|.  Descriptions of each of these is at the top of the file
/   |layout.c|.
/*/
extern int lo_debug ;
#ifdef CPLUS
void *layout_graph(int n,
   void (*fillnodearray)(void **nodearr),
   void (*filledgearray)(void *node, int *incnt, int *outcnt, int *inarity,
      int *outarity, int *succarr),
   void (*filledgeptrs)(void *node, void **varr),
   void (*filledgeio)(void *node, short *io), void *(*malloc)(int n)) ;

void layout_adjust(void *g, int (*getwidth)(void *),
   int (*getheight)(void *), void (*getoffsets)(void *, short *, short *),
   struct layout_adjust_params *lap, int *finalw, int *finalh) ;

void layout_draw(void *g, void (*drawnode)(void *, int x, int y),
   void (*drawedge)(void *, int x1, int y1, int x2, int y2, char backedge)) ;
      
void layout_clean(void *g) ;
void *layout_find_node(void *g, int x, int y) ;
void *layout_graph_p(int n, void (*fillnodearray)(),
     void (*filledgearray)(), void (*filledgeptrs)(), void (*filledgeio)(),
     void *(*malloc)(), struct layout_adjust_params *lap) ;
#else
void *layout_graph(), *layout_find_node() ;
void layout_adjust(), layout_draw(), layout_clean() ;
#endif
