/*
/   This code reads in a specification of a graph and drives the
/   layout software to draw it.
/
/   It accepts multiple graphs per file.  If there are multiple graphs
/   per file, it plots each one and then arranges them to fit on the page.
/
/   If you have the X11 includes and libraries on your system, compile
/   this with an X preprocessor definition.
/*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "layout.h"
#ifdef X
#include <X11/Xlib.h>
#include <X11/Xutil.h>
struct MyWindow {
   Window *window ;
   int width, height ;
   int hoff, voff ;
   struct graph *graph ;
} *win ;
#endif
void (*dn)() ;
void (*de)() ;
int (*strwid)() ;
/*
/   Each node line is of the form:
/
/   Node <type> <label> <name>
/
/   where <type> is one of
/
/   Place Transition Function Register MarkedPlace or Invisible
/
/   and <label> is some arbitrary labeling string and <name> is the
/   distinguishing label for that node.
/
/   Each edge line is of the form:
/
/   Edge <type> <label> <pos> <label> <pos>
/
/   where <type> is one of
/
/   Petri or Data
/
/   and <label> is from an already-defined node, and <pos> is some
/   non-negative integer.
/
/   We store the nodes in a hash table and the edges in a linked list.
/*/
static struct layout_adjust_params mylap = {0, 0, 14, 7, 3, 14, 10, 1} ;
int llx, lly, urx, ury ;
double scalex = 1.0, scaley = 1.0 ;
int dontdrop = 0 ;
struct mnode {
   struct mnode *next ;
   char *label ;
   char *name ;
   int seqno ;
   int inarity, outarity ;
   int incnt, outcnt ;
   int height, width, rwidth ;
   struct medge *inedges, *outedges ;
   char type, color ;
   char miscdat[2] ;
} ;
static int numnodes = 0 ;
static int numgraphs = 0 ;
struct medge {
   struct medge *next ;
   struct medge *innext, *outnext ;
   int seqno ;
   char type, color ;
   struct mnode *in, *out ;
   int inpos, outpos ;
} ;
static int numedges = 0 ;
struct graph {
   struct graph *next ;
   void *laidout ;
   int h, w ;
} *graphs ;
/*
/   Our hash routine.
/*/
#define HASHPRIME (1009)
static int hash(char *s)
{
   int h = 12 ;

   while (*s != 0)
      h = (h + h + *s++) % HASHPRIME ;
   return(h) ;
}
/*
/   The routine that looks up a font name.
/*/
static struct mnode *mhash[HASHPRIME] ;
struct mnode *lookup(char *name)
{
   struct mnode *p ;

   for (p=mhash[hash(name)]; p!=NULL; p=p->next)
      if (strcmp(p->label, name)==0)
         return(p) ;
   return(0) ;
}
/*
/   Error handler.
/*/
static char curline[1024] ;
static void error(char *s)
{
   fprintf(stderr, " :: -> %s\n :: %s\n", curline, s) ;
   if (*s == '!')
      exit(20) ;
}
/*
/   Allocator
/*/
extern void *malloc() ;
static void *mymalloc(int n)
{
   void *p = malloc(n) ;
   if (p == 0)
      error("! No memory?") ;
   return p ;
}
static int checktype(char *type, char *pos)
{
   while (*pos) {
      if (*pos == *type || *pos + 32 == *type)
         return *pos ;
      pos++ ;
   }
   error("! not a legal type?") ;
   return -1;
}
/*
/   How do we know the width of a string?
/*/
static short helvwidth[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   278, 278, 355, 556, 556, 889, 667, 222,
   333, 333, 389, 584, 278, 333, 278, 278,
   556, 556, 556, 556, 556, 556, 556, 556,
   556, 556, 278, 278, 584, 584, 584, 556,
   1015, 667, 667, 722, 722, 667, 611, 778,
   722, 278, 500, 667, 556, 833, 722, 778,
   667, 778, 722, 667, 611, 722, 667, 944,
   667, 667, 611, 278, 278, 278, 469, 556,
   222, 556, 556, 500, 556, 556, 278, 556,
   556, 222, 222, 500, 222, 833, 556, 556,
   556, 556, 333, 500, 278, 556, 500, 722,
   500, 500, 500, 334, 260, 334, 584, 0 } ;
int PSstrwid(char *s)
{
   int totlen = 0 ;
   while (*s)
      totlen += helvwidth[*s++] ;
   totlen = ((totlen + 100) / 200) * 2 ;
   return totlen ;
}
/*
/   The routine that adds a new node.
/*/
static struct mnode *newnode(char *label, char *name, char *type)
{
   struct mnode *p =
       (struct mnode*)mymalloc(sizeof(struct mnode) + strlen(name) + strlen(label)) ;
   int h = hash(label) ;

   p->color = 0 ;
   p->next = mhash[h] ;
   p->label = p->miscdat ;
   strcpy(p->label, label) ;
   p->name = p->miscdat + strlen(label) + 1 ;
   strcpy(p->name, name) ;
   p->seqno = -1 ;
   p->inarity = 0 ;
   p->outarity = 0 ;
   p->incnt = 0 ;
   p->outcnt = 0 ;
   p->inedges = 0 ;
   p->outedges = 0 ;
   p->type = checktype(type, "PETUFRMIJ") ;
   mhash[h] = p ;
   switch (p->type) {
   case 'P':
   case 'M':
      p->height = 20 ;
      p->width = 20 ;
      break ;
   case 'T':
   case 'U':
      p->height = 2 ;
      p->width = 20 ;
      break ;
   case 'R':
      p->height = 14 ;
      p->width = 30 ;
      break ;
   case 'I':
      p->height = 0 ;
      p->width = 0 ;
      break ;
   case 'F':
      p->height = 14 ;
      if (p->name && *(p->name)) {
         p->width = strwid(p->name) + 8 ;
         if (p->width < 14)
            p->width = 14 ;
      } else
         p->width = 20 ;
      break ;
   case 'E':
      p->height = 14 ;
      if (p->name && *(p->name)) {
         p->width = strwid(p->name) ;
         if (p->width < 8)
            p->width = 8 ;
      } else
         p->width = 20 ;
      break ;
   case 'J':
      p->height = 3 ;
      p->width = 3 ;
      break ;
   }
   if (p->name && *(p->name) && p->type != 'F') {
      p->rwidth = p->width + 4 + strwid(p->name) ;
   } else
      p->rwidth = p->width ;
   return p ;
}
/*
/  The routine that adds an edge.
/*/
static struct medge *edges ;
static struct medge *newedge(char *type, char *n1, int pos1, char *n2,
                             int pos2)
{
   struct medge *p = (struct medge*)mymalloc(sizeof(struct medge)) ;

   p->color = 0 ;
   p->next = edges ;
   edges = p ;
   p->type = checktype(type, "PD") ;
   p->in = lookup(n1) ;
   p->out = lookup(n2) ;
   p->inpos = pos1 ;
   p->outpos = pos2 ;
   p->seqno = numedges++ ;
   if (p->in == 0) {
      error(" no input node match") ;
      p->in = newnode("?", "???", "F") ;
   }
   if (p->out == 0) {
      error(" no output node match") ;
      p->out = newnode("?", "???", "F") ;
   }
   if (p->in->outarity <= pos1)
      p->in->outarity = pos1 + 1 ;
   if (p->out->inarity <= pos2)
      p->out->inarity = pos2 + 1 ;
   p->outnext = p->in->outedges ;
   p->in->outedges = p ;
   p->innext = p->out->inedges ;
   p->out->inedges = p ;
   p->in->outcnt++ ;
   p->out->incnt++ ;
   return p ;
}
/*
/   Now we get into parsing.  Each input line is broken into words,
/   stored in the following arrays.  Up to 100 words per line.
/*/
#define MAXWORDS (100)
static char pline[1024] ;
static char *pargs[MAXWORDS] ;
static int pargv[MAXWORDS] ;
static int pflags[MAXWORDS] ;
static int numargs ;
#define ISINTEGER (1)
static int parseline() {
   char *p = pline ;
   char *q ;
   int i ;

   strcpy(pline, curline) ;
   numargs = 0 ;
   for (i=0; i<MAXWORDS; i++)
      pargs[i] = "" ;
   if (pline[0] == '%' || pline[0] < ' ' || pline[0] == ';')
      return 0 ;
   while (1) {
      while (*p <= ' ' && *p)
         p++ ;
      if (*p == 0)
         return numargs ;
      pargs[numargs] = p ;
      pflags[numargs] = 0 ;
      while (*p > ' ')
         p++ ;
      if (*p != 0)
         *p++ = 0 ;
      q = pargs[numargs] ;
      if (*q == '-')
         q++ ;
      while ('0' <= *q && *q <= '9')
         q++ ;
      if (*q == 0) {
         pflags[numargs] |= ISINTEGER ;
         pargv[numargs] = atol(pargs[numargs]) ;
      }
      numargs++ ;
   }
}
/*
/   Now the routine which builds the graph.
/*/
static void buildgraph() {
   while (fgets(curline,1024,stdin)) {
      if (parseline()) {
         switch (checktype(pargs[0], "NEZ*%")) {
	 case 'N':
            if (numargs != 4 && numargs != 3)
               error("! node line needs two or three arguments") ;
            newnode(pargs[2], pargs[3], pargs[1]) ;
            break ;
	 case 'E':
            if (numargs != 6)
               error("! edge line needs six arguments") ;
            if ((pflags[3] & ISINTEGER) == 0 ||
                (pflags[5] & ISINTEGER) == 0)
               error("! second and fourth args must be numeric") ;
            newedge(pargs[1], pargs[2], pargv[3], pargs[4], pargv[5]) ;
            break ;
	 case 'Z':
            return ; /* if this is a ZZZ line indicating the eof, we return */
	 default:
            break ;
         }
      }
   }
}
/*
/   Number the nodes, omitting those that aren't connected.
/*/
static void numbernodes() {
   int i, ct ;
   struct mnode *p ;

   ct = 0 ;
   for (i=0; i<HASHPRIME; i++)
      for (p = mhash[i]; p; p = p->next) {
         if (dontdrop || p->incnt || p->outcnt)
            p->seqno = numnodes++ ;
         else
            ct++ ;
      }
   if (ct)
      fprintf(stderr, "Warning:  dropped %d unconnected nodes\n", ct) ;
}
/*
/   Now we write all the little accessor routines that layout wants.
/*/
static void fna(void **nodearr)
{
   int i, ct ;
   struct mnode *p ;

   ct = 0 ;
   for (i=0; i<HASHPRIME; i++)
      for (p = mhash[i]; p; p = p->next)
         if (p->seqno >= 0) {
            ct++ ;
            nodearr[p->seqno] = p ;
         }
   if (ct != numnodes)
      error("! internal consistency violation in fillnodearray") ;
}
static void fea(struct mnode *node, int *incnt, int *outcnt, int *inarity,
                int *outarity, int *succarr)
{
   struct medge *p ;

   *incnt = node->incnt ;
   *outcnt = node->outcnt ;
   *inarity = node->inarity ;
   *outarity = node->outarity ;
   for (p = node->outedges; p; p = p->outnext)
      *succarr++ = p->out->seqno ;
}
static void fep(struct mnode *node, void **edges)
{
   struct medge *p ;

   for (p = node->outedges; p; p = p->outnext)
      *edges++ = p ;
}
static void feio(struct mnode *node, short *io)
{
   struct medge *p ;

   for (p = node->outedges; p; p = p->outnext) {
      *io++ = p->inpos ;
      *io++ = p->outpos ;
   }
}
static int gh(struct mnode *node)
{
   return node->height ;
}
static int gw(struct mnode *node)
{
   return node->rwidth ;
}
static void go(struct mnode *node, short *inoffs, short *outoffs)
{
   int i ;
   int off = (node->width - node->rwidth) / 2 ;

   if (node->type == 'R' && node->inarity > 0) {
      *inoffs++ = - ((node->rwidth + 1) / 2) ;
      if (node->inarity > 1)
         *inoffs++ = off + (node->height + 2) / 4 ;
      *inoffs++ = (node->height + 1) / 2 ;
   } else {
      for (i=0; i<node->inarity; i++)
         *inoffs++ = off + ((2 * i + 1) * node->width + node->inarity) /
                               (2 * node->inarity) - node->width / 2 ;
   }
   if (node->type == 'R' && node->outarity > 0) {
      *outoffs++ = off + (node->height + 2) / 4 ;
   } else {
      for (i=0; i<node->outarity; i++)
         *outoffs++ = off + ((2 * i + 1) * node->width + node->outarity) /
                               (2 * node->outarity) - node->width / 2 ;
   }
}
static int finalh, finalw ;
int hoff = 5, voff = 5 ;
char *progname ;
#ifdef X
int whitePixel, blackPixel ;
int colors[4] ;
char *colornames[4] = { "Black", "Red", "Orange", "Blue" } ;
Display *display ;
Window window ;
GC gc ;
XFontStruct *fs ;
int screen_width, screen_height ;
void init_X() {
   XColor exact, def ;
   int i ;

   if (display == 0) {
      if ((display = XOpenDisplay(0))==0)
         error("! can't open display") ;
      whitePixel = WhitePixel(display, DefaultScreen(display)) ;
      blackPixel = BlackPixel(display, DefaultScreen(display)) ;
      colors[0] = blackPixel ;
      for (i=1; i<4; i++) {
         if (XAllocNamedColor(display, XDefaultColormap(display,
                      DefaultScreen(display)), colornames[i],
               &exact, &def))
            colors[i] = def.pixel ;
         else {
            colors[i] = blackPixel ;
            error(" a color is missing.") ;
	 }
      }
      gc = XDefaultGC(display, XDefaultScreen(display)) ;
      fs = XQueryFont(display, XGContextFromGC(gc)) ;
      screen_width = XWidthOfScreen(XDefaultScreenOfDisplay(display)) ;
      screen_height = XHeightOfScreen(XDefaultScreenOfDisplay(display)) ;
   }
}
int Xstrwid(char *s)
{
   return XTextWidth(fs, s, strlen(s)) ;
}
int rndm(int n)
{
   int t = (rand() >> 2) ;
   return (t ^ (t >> 16)) % n ;
}
int window_width, window_height ;
void open_window(int width, int height)
{
   XSizeHints xsizehints ;
   XSetWindowAttributes xsetwindowattributes ;
   XWindowAttributes xwindowattributes ;
   XEvent event ;
   int x, y ;

   width += 10 ;
   height += 10 ;
   if (width > screen_width)
      width = screen_width ;
   if (height > (screen_height-100))
      height = screen_height - 100;
   window_width = width ;
   window_height = height ;
   x = (width >= screen_width) ? 0 : rndm(screen_width-width) ;
   y = (height >= screen_height) ? 0 : rndm(screen_height-height) ;
   xsetwindowattributes.background_pixel = whitePixel ;
   xsetwindowattributes.border_pixel = blackPixel ;
   window = XCreateWindow(display, DefaultRootWindow(display), x, y,
      width, height, 2, DefaultDepth(display, XDefaultScreen(display)),
      InputOutput, DefaultVisual(display, XDefaultScreen(display)),
      CWBackPixel|CWBorderPixel, &xsetwindowattributes) ;
   if (window == 0)
      error("! couldn't open window") ;
   xsizehints.x = x ;
   xsizehints.y = y ;
   xsizehints.width = width ;
   xsizehints.height = height ;
   xsizehints.flags = USSize | USPosition ;
   XSetStandardProperties(display, window, progname, progname, None,
      0, 0, &xsizehints) ;
   XSelectInput(display, window, ExposureMask|KeyPressMask|ButtonPressMask) ;
   XSync(display, 1) ;
   XMapWindow(display, window) ;
   XNextEvent(display, &event) ;
   XGetWindowAttributes(display, window, &xwindowattributes) ;
   window_height = xwindowattributes.height ;
   window_width = xwindowattributes.width ;
   XFlush(display) ;
}
void Xdn(struct mnode *node, int x, int y)
{
   int xc = x + hoff - (node->rwidth - node->width) / 2, yc = y + voff ;
   int w=node->width, h=node->height ;
   int llx=xc-w/2, lly=yc+h/2 ;
   int urx = llx + w ;
   int ury = lly - h ;
   int sx = llx + 2 ;
   int sy = lly - 2 ;

   XSetForeground(display, gc, colors[node->color]) ;
   switch(node->type) {
   case 'F':
      XDrawRectangle(display, window, gc, llx, ury, w, h) ;
   case 'E':
      XDrawString(display, window, gc, sx, sy, node->name,
         strlen(node->name)) ;
      break ;
   case 'R':
      XDrawRectangle(display, window, gc, llx, ury, w, h) ;
      XDrawLine(display, window, gc, llx, lly, llx + h/2, yc) ;
      XDrawLine(display, window, gc, llx, ury, llx + h/2, yc) ;
   case 'I':
      break ;
   case 'J':
      XFillArc(display, window, gc, llx, ury, w, h, 0, 360*64) ;
      break ;
   case 'T':
      XFillRectangle(display, window, gc, llx, ury, w, h) ;
      break ;
   case 'M':
      XFillArc(display, window, gc, (llx+xc)/2, (ury+yc)/2,
         w/2, h/2, 0, 360*64) ;
   case 'P':
      XDrawArc(display, window, gc, llx, ury, w, h, 0, 360*64) ;
      break ;
   }
   if (node->name && *(node->name) && node->type != 'F' &&
                                      node->type != 'E') {
      XDrawString(display, window, gc, urx + 2, yc+3, node->name, 
         strlen(node->name)) ;
   }
}
void Xde(struct mnode *edge, int x1, int y1, int x2, int y2, char back)
{
   x1 += hoff ; x2 += hoff ;
   y1 += voff ; y2 += voff ;
   XSetForeground(display, gc, colors[edge->color]) ;
   XDrawLine(display, window, gc, x1, y1, x2, y2) ;
}
void layout_Xopen(struct graph *mg)
{
   open_window(mg->w, mg->h) ;
}
#endif
void PSdn(struct mnode *node, int x, int y)
{
   int nx = x + (node->width - node->rwidth) / 2 ;
   y = finalh - y ;
   if (node->type == 'F' || node->type == 'E')
      fprintf(stdout, "(%s) %d %d %d N%c\n", node->name, nx, y, node->width,
              node->type) ;
   else {
      fprintf(stdout, "%d %d N%c\n", nx, y, node->type) ;
      if (node->name && *(node->name)) {
         nx += node->width / 2 + 4 ;
         fprintf(stdout, "(%s) %d %d L\n", node->name, nx, y) ;
      }
   }
}
void PSde(struct medge *edge, int x1, int y1, int x2, int y2, char back)
{
   y1 = finalh - y1 ;
   y2 = finalh - y2 ;
   fprintf(stdout, "%d %d %d %d E%c\n", x1, y1, x2, y2, edge->type) ;
}
/*
/   Send out the PostScript prolog.
/*/
static char *headdat[] = {
"%%EndComments",
"/Helvetica findfont 10 scalefont setfont",
"/X { exch def } def",
"/box { /w X /h X gsave h -0.5 mul w -0.5 mul rmoveto",
"   0 w rlineto h 0 rlineto 0 w neg rlineto closepath stroke grestore } def",
"/NF { /w X moveto w 14 box dup stringwidth pop -.5 mul -4 rmoveto show } def",
"/NE { /w X moveto dup stringwidth pop -.5 mul -4 rmoveto show } def",
"/NR { moveto gsave 3.5 0 rmoveto 23 14 box grestore -15 0 rmoveto",
"      7 0 rlineto 0 -7 rmoveto 7 7 rlineto -7 7 rlineto stroke } def",
"/NI { pop pop } def",
"/NJ { gsave newpath 2 0 360 arc fill grestore } def",
"/NT { gsave 3 setlinewidth moveto -10 0 rmoveto 20 0 rlineto",
"      stroke grestore } def",
"/NU { gsave 0.5 setlinewidth moveto -10 0 rmoveto 20 0 rlineto",
"      stroke grestore } def",
"/NM { 2 copy NP gsave newpath 6 0 360 arc fill grestore } def",
"/NP { gsave newpath 10 0 360 arc closepath stroke grestore } def",
"/ED { gsave [8 4] 4 setdash moveto lineto stroke grestore } def",
"/L { gsave moveto 0 -4 rmoveto show grestore } def",
"/EP { gsave moveto lineto stroke grestore } def % for now",
"%%EndProlog",
"1 setlinecap", 0 } ;
void header() {
   char **p ;

   for (p = headdat; *p; p++) {
      fputs(*p, stdout) ;
      fputc('\n', stdout) ;
   }
   fprintf(stdout, "%d %d translate %g %g scale\n",
      llx, lly, scalex, scaley) ;
}
/*
 *   This routine sets up the parameters for scaling based on the
 *   width and height of the graph.
 */
void scaleit(int w, int h)
{
   if (w < 72 * 15 / 2) {
      llx = 72 * 17 / 4 - w / 2 ;
      urx = llx + w ;
   } else {
      llx = 36 ;
      urx = 36 + 72 * 15 / 2 ;
      scalex = ((double)(urx - llx)) / (double)w ;
   }
   if (h < 72 * 10) {
      lly = 72 * 11 / 2 - h / 2 ;
      ury = lly + h ;
   } else {
      lly = 36 ;
      ury = 36 + 72 * 10 ;
      scaley = ((double)(ury - lly)) / (double)h ;
   }
   printf("%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury) ;
}
/*
 *   If there was more than one graph, this draws arranges them all on
 *   the same page.
 */
void arrange(int *w, int *h, int *x, int *y, int n, int *fw, int *fh)
{
   int *ptrs = (int *)mymalloc(n * sizeof(int)) ;
   int *cornx = (int *)mymalloc((n+1) * sizeof(int)) ;
   int *corny = (int *)mymalloc((n+1) * sizeof(int)) ;
   int *dornx = (int *)mymalloc((n+1) * sizeof(int)) ;
   int *dorny = (int *)mymalloc((n+1) * sizeof(int)) ;
   int *ptemp ;
   int i, j, k, ii, jj, gap, temp ;
   int maxw, maxh, area ;
   int goalw, bx, by, tx, ty ;
/*
 *   First, we sort them by decreasing height.  We always want to do the
 *   tallest first.
 */
   for (i=0; i<n; i++)
      ptrs[i] = i ;
   gap = 1 ;
   do {
      gap = 3 * gap + 1 ;
   } while (gap <= n) ;
   for (gap /= 3; gap > 0; gap /= 3) {
      for (i=gap; i<n; i++) {
         temp = ptrs[i] ;
         for (j=i-gap; j >= 0 && h[ptrs[j]] < h[temp]; j -= gap)
            ptrs[j+gap] = ptrs[j] ;
         ptrs[j+gap] = temp ;
      }
   }
/*
 *   Now we calculate the total area and the max height and width.
 */
   area = 0 ;
   maxw = 0 ;
   maxh = 0 ;
   for (i=0; i<n; i++) {
      if (w[i] > maxw)
         maxw = w[i] ;
      if (h[i] > maxh)
         maxh = h[i] ;
      area += w[i] * h[i] ;
   }
/*
 *   a little consistency check
 */
   if (maxh != h[ptrs[0]] || area <= 0)
      error("! something wrong in sort or max height calculations") ;
/*
 *   For the goal width, we simply start with the square root of the area,
 *   and increase it slowly as necessary.  (This initially throws away
 *   about 15% of the area, but we probably aren't going to get packing that
 *   efficient anyway.)
 */
   goalw = (int)sqrt((double)area) ;
/*
 *   We do make sure that this leaves enough space for maxw or maxh, and
 *   increase it if necessary right away.
 */
   if (maxw > goalw)
      goalw = maxw ;
   if (maxh > goalw * 21 / 16)
      goalw = maxh * 16 / 21 ;
   while (1) {
      maxw = 0 ;
      maxh = 0 ;
      cornx[0] = 0 ;
      corny[0] = 0 ;
      cornx[1] = 20 * goalw ;
      for (i=0; i<n; i++) {
         j = ptrs[i] ;
         bx = 0 ;
         by = goalw * 10 ;
         for (k=0; cornx[k] <= goalw - w[j]; k++) {
            ty = corny[k] ;
            tx = cornx[k] ;
            for (ii=k + 1; cornx[ii] < tx + w[j]; ii++)
               if (corny[ii] > ty)
                  ty = corny[ii] ;
            if (ty < by) {
               bx = tx ;
               by = ty ;
            }
         }
         x[j] = bx ;
         y[j] = by ;
         if (bx + w[j] > maxw)
            maxw = bx + w[j] ;
         if (by + h[j] > maxh)
            maxh = by + h[j] ;
         for (k=0; cornx[k] < bx; k++) {
            dornx[k] = cornx[k] ;
            dorny[k] = corny[k] ;
         }
         dornx[k] = bx ;
         dorny[k++] = by + h[j] ;
         dornx[k] = bx + w[j] ;
         for (ii=k-1; cornx[ii+1] <= bx + w[j]; ii++) ;
         dorny[k++] = corny[ii++] ;
         for (; cornx[ii] <= goalw; ii++, k++) {
            dornx[k] = cornx[ii] ;
            dorny[k] = corny[ii] ;
         }
         dornx[k] = 10 * goalw ;
         ptemp = dornx ;
         dornx = cornx ;
         cornx = ptemp ;
         ptemp = dorny ;
         dorny = corny ;
         corny = ptemp ;
      }
      if (maxh <= goalw * 11 / 8) {
         *fw = maxw ;
         *fh = maxh ;
         return ;
      }
      goalw = goalw * 11 / 10 ; /* add 10% and iterate */
   }
}

#ifdef X
void event_loop() {
   int i, x, y, len, key ;
   XEvent event ;
   XExposeEvent *xev ;
   XKeyEvent *kev ;
   XButtonEvent *bev ;
   struct mnode *mn ;
   struct medge *me ;
   char buf[20] ;
   int div = 1 ;
   struct graph *graph ;
   struct MyWindow *mywin ;

   while (1) {
      XNextEvent(display, &event) ;
      kev = (XKeyEvent *)&event ;
      mywin = 0 ;
      for (i=0; i<numgraphs; i++)
         if (kev->window == win[i].window)
	    break ;
      if (i < numgraphs) {
	 mywin = &(win[i]) ;
         window = mywin->window ;
         graph = mywin->graph ;
         hoff = mywin->hoff ;
         voff = mywin->voff ;
         window_height = mywin->height ;
         window_width = mywin->width ;
      }
      switch(event.type) {
case KeyPress:
         kev = (XKeyEvent *)&event ;
         len = XLookupString(kev, buf, 10, 0, 0) ;
	 div = 1 ;
         if (len == 1) {
            key = buf[0] ;
            if ('A' <= key && key <= 'Z')
               key += 32 ;
	 } else {
            key = XKeycodeToKeysym(display, kev->keycode, 0) ;
            len = 1 ;
            switch(key) {
case 65361:       key = 'h' ; break ;
case 65362:       key = 'k' ; break ;
case 65363:       key = 'l' ; break ;
case 65364:       key = 'j' ; break ;
default:          break ;
            }
            if (kev->state & ShiftMask)
               div <<= 1 ;
            if (kev->state & ControlMask)
               div <<= 2 ;
         }
         switch(key) {
	 case 'q':
	 case 'x':
	 case 27:
	    exit(0) ;
	  case 'l': hoff -= window_width / div ;
                    if (hoff < window_width - graph->w - 5)
                       hoff = window_width - graph->w - 5 ;
                    goto redraw ;
	  case 'h': hoff += window_width / div ;
                    if (hoff > 5)
                       hoff = 5 ;
                    goto redraw ;
	  case 'j': voff -= window_height / div ;
                    if (voff < window_height - graph->h - 5)
                       voff = window_height - graph->h - 5 ;
                    goto redraw ;
	  case 'k': voff += window_height / div ;
                    if (voff > 5)
                       voff = 5 ;
	  redraw:
/*   printf("window: %dx%d  graph: %dx%d  off: %dx%d\n",
                    window_width, window_height, graph->w, graph->h,
                    hoff, voff) ; */
            if (mywin) {
               mywin->hoff = hoff ;
               mywin->voff = voff ;
	       XClearWindow(display, window) ;
               layout_draw(graph->laidout, dn, de) ;
            }
         }
         break ;
case Expose: case GraphicsExpose:
         xev = (XExposeEvent *)&event ;
	 if (mywin)
            layout_draw(graph->laidout, dn, de) ;
         break ;
case ButtonPress:
         bev = (XButtonEvent *)&event ;
         if (mywin) {
            mn = layout_find_node(graph->laidout, bev->x-hoff, bev->y-voff) ;
            if (mn == 0)
               break ;
            mn->color ^= 3 ;
            for (me = mn->inedges; me; me = me->innext)
               me->color ^= 1 ;
            for (me = mn->outedges; me; me = me->outnext)
               me->color ^= 2 ;
            layout_draw(graph->laidout, dn, de) ;
         }
         break ;
default:
         break ;
      }
   }
}
#endif

/*
/   Finally, our main routine.
/*/
void **gra ;
#ifdef X
int ps = 0 ;
#else
int ps = 1 ;
#endif
int dis ;
int main(int argc, char *argv[])
{
   void *g ;
   int i ;
   struct graph *mg ;
   int *x, *y, *h, *w ;
   int jp = 1 ;

   progname = argv[0] ;
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch(argv[0][1]) {
      case 'p': ps = 1 ; break ;
      case 'd': dontdrop = 1 ; break ;
      case 'v': dis = 1 ;
                break ;
      case 'x': ps = 0 ; break ;
      case 'n': jp = 0 ; break ;
      default: error("! usage: parg [-n] [-p] [-d] [-x] [-v] infile") ; break ;
      }
   }
   if (argc > 1) {
      argc-- ;
      argv++ ;
      if (freopen(argv[0], "r", stdin) == 0)
         error("! couldn't open input file") ;
   }
   if (argc > 1) {
      argc-- ;
      argv++ ;
      if (freopen(argv[0], "w", stdout) == 0)
         error("! couldn't open output file") ;
   } else if (dis == 1 && ps == 1) {
      if (freopen("parg.eps", "w", stdout) == 0)
         error("! couldn't open parg.eps for output") ;
   }
   mylap.joinpossible = jp ;
   if (ps == 0) {
#ifdef X
      init_X() ;
      dn = Xdn ;
      de = Xde ;
      strwid = Xstrwid ;
      mylap.goal_height = screen_height - 20 ;
      mylap.goal_width = screen_width - 20 ;
#else
      error("! not compiled for X") ;
#endif
   } else {
      dn = PSdn ;
      de = PSde ;
      strwid = PSstrwid ;
      fputs("%!PS-Adobe-2.0\n%%Creator: ", stdout) ;
   }
   while (1) {
      numnodes = 0 ;
      numedges = 0 ;
      for (i=0; i<HASHPRIME; i++)
         mhash[i] = 0 ;
      curline[0] = 0 ;
      edges = 0 ;
      numargs = 0 ;
      buildgraph() ;
      numbernodes() ;
      if (numnodes == 0)
         break ;
      numgraphs++ ;
      g = layout_graph_p(numnodes, fna, fea, fep, feio, 0, &mylap) ;
      mg = (struct graph*)mymalloc(sizeof(struct graph)) ;
      mg->next = graphs ;
      graphs = mg ;
      mg->laidout = g ;
      layout_adjust(g, gw, gh, go, &mylap, &finalw, &finalh) ;
      mg->h = finalh ;
      mg->w = finalw ;
   }
/*
 *   If there was more than one graph, we arrange them appropriately.
 */
   if (numgraphs > 1)
      fprintf(stderr, "Saw %d graphs\n", numgraphs) ;
   else if (numgraphs == 0)
      exit(0) ;
   x = (int*)mymalloc(numgraphs * sizeof(int)) ;
   y = (int*)mymalloc(numgraphs * sizeof(int)) ;
   h = (int*)mymalloc(numgraphs * sizeof(int)) ;
   w = (int*)mymalloc(numgraphs * sizeof(int)) ;
   gra = (int*)mymalloc(numgraphs * sizeof(void *)) ;
#ifdef X
   win = mymalloc(numgraphs * sizeof(struct MyWindow)) ;
#endif
   for (i=0, mg=graphs; mg; mg = mg->next, i++) {
      h[i] = mg->h + 10 ;
      w[i] = mg->w + 10 ;
      gra[i] = mg->laidout ;
   }
   if (ps) {
      if (numgraphs > 1) {
         arrange(w, h, x, y, numgraphs, &finalw, &finalh) ;
         scaleit(finalw, finalh) ;
         header() ;
         for (i=0, mg=graphs; mg; mg = mg->next, i++) {
            fprintf(stdout, "%d %d translate\n", x[i]+5, -y[i]-5) ;
            layout_draw(mg->laidout, dn, de) ;
            fprintf(stdout, "%d %d translate\n", -x[i]-5, y[i]+5) ;
         }
      } else {
         scaleit(finalw, finalh) ;
         header() ;
         layout_draw(g, dn, de) ;
      }
      fprintf(stdout, "showpage\n") ;
   } else {
#ifdef X
      for (i=0, mg=graphs; i<numgraphs; i++, mg = mg->next) {
         layout_Xopen(mg) ;
         win[i].window = window ;
         win[i].height = window_height ;
         win[i].width = window_width ;
         win[i].hoff = 5 ;
         win[i].voff = 5 ;
         win[i].graph = mg ;
         layout_draw(gra[i], dn, de) ;
      }
      event_loop() ;
#endif
   }
   if (dis == 1 && ps == 1) {
      fclose(stdout) ;
      system("open parg.eps") ; /* only works right on NeXT */
   }
   return 0;
}
