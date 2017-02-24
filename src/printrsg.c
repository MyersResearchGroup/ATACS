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
/* printrsg.c                                                                */
/*****************************************************************************/

#include "printrsg.h"

//#define PRED

void get_trans(signalADT *signals,int nsignals,char * pred,char * cur,
	       char *trans)
{
  int i;
  bool first;

  trans[0]='\0';
  first=TRUE;
  for (i=0;i<nsignals;i++)
    if (((pred[i]=='R')||(pred[i]=='U'))&&
	((cur[i]=='1')||(cur[i]=='F')||(cur[i]=='D'))) {
      if ((strlen(trans) + strlen(signals[i]->name)) > 250) return;
      if (!first) strcat(trans,",");
      first=FALSE;
      strcat(trans,signals[i]->name);
      strcat(trans,"+");
    } else if (((pred[i]=='F')||(pred[i]=='D'))&&
	       ((cur[i]=='0')||(cur[i]=='R')||(cur[i]=='U'))) {
      if ((strlen(trans) + strlen(signals[i]->name)) > 250) return;
      if (!first) strcat(trans,",");
      first=FALSE;
      strcat(trans,signals[i]->name);
      strcat(trans,"-");
    }
}

/*****************************************************************************/
/* Print a reduced state graph using parseGraph by Tom Rokicki.              */
/*****************************************************************************/

void print_rsg(char * filename,signalADT *signals,eventADT *events,
	       stateADT *state_table,int nsignals,bool display,bool genrg,
	       int nevents, int nplaces, bool stats)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int i;
  int edge;
  stateADT curstate;
  statelistADT predstate,predstate2;
  bool empty;
  char trans[255];
  bool skip_edge;

  strcpy(outname,filename);
  strcat(outname,".prg");
  printf("Storing reduced state graph to:  %s\n",outname);
  fprintf(lg,"Storing reduced state graph to:  %s\n",outname);
  if(!(fp=fopen(outname,"w"))){
    fprintf(stderr, "ERROR:  Could not open %s for writing!\n",outname);
    fprintf(lg    , "ERROR:  Could not open %s for writing!\n",outname);
    problems++;
    return;
  }
  empty=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) { 
	empty=FALSE;
	fprintf(fp,"N F S%d %d:%s:%d",curstate->number,curstate->number,
		curstate->state,curstate->color);
	if ( stats )
	  fprintf( fp, ":%g", curstate->probability );
	fprintf( fp, "\n" );
	if (strcmp(curstate->state,"FAIL")==0) {
	  fprintf(fp,"E P S%d 0 S%d 0\n",curstate->number,curstate->number);
	}
      }
  if (empty) {
    printf("ERROR:  No states in reduced state graph!\n");
    fprintf(lg,"ERROR:  No states in reduced state graph!\n");
    return;
  }
  edge=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
#ifdef PRED
      for (predstate=curstate->pred;predstate;
	   predstate=predstate->next) 
	if (curstate != predstate->stateptr) {
#else
      for (predstate=curstate->succ;predstate;
	   predstate=predstate->next) 
	if (curstate != predstate->stateptr) {
#endif
	skip_edge=false;
	if ((genrg) &&
	    (predstate->enabling >= 0) &&
	    (predstate->enabled >= 0) &&
	    (predstate->enabling < nevents+nplaces) &&
	    (predstate->enabled < nevents)) {
	  if (predstate->enabled >= 0)
	    fprintf(fp,"N E T%d <%s,%s>%c",edge,
		    events[predstate->enabling]->event,
		    events[predstate->enabled]->event,
		    predstate->iteration);
	  else {
#ifdef PRED
	    get_trans(signals,nsignals,predstate->stateptr->state,
		      curstate->state,trans);
#else
	    get_trans(signals,nsignals,curstate->state,
		      predstate->stateptr->state,trans);
#endif
	    fprintf(fp,"N E T%d %s",edge,trans);
	  }
	  if ( stats )
	    fprintf( fp, ":%g", predstate->probability );
	  fprintf( fp, "\n" );
	} else {
	  if ((predstate->enabled >= 0) &&
	      (predstate->enabled < (nevents + nplaces))) {
	    bool first=true;
#ifdef PRED
	    for (predstate2=curstate->pred;predstate2;
		 predstate2=predstate2->next) 
#else
            for (predstate2=curstate->succ;predstate2;
		 predstate2=predstate2->next) 
#endif
            {
	      if (predstate2->stateptr==predstate->stateptr) {
		if ((first) && (predstate==predstate2)) {
		  if (predstate->iteration == 'I') { 
#ifdef PRED
		    if (predstate->stateptr->state[predstate2->enabled]=='0'){
#else
		    if (predstate->stateptr->state[predstate2->enabled]=='1'){
#endif
		      fprintf(fp,"N E T%d {%s}",edge,
			      signals[predstate2->enabled]->name);
		    } else {
		      fprintf(fp,"N E T%d {~(%s)}",edge,
			      signals[predstate2->enabled]->name);
		    }
	          } else if (predstate->iteration == 'R') {
		      fprintf(fp,"N E T%d %s",edge,events[predstate->enabled]->event);
		  } else if (predstate->iteration != 'f') {
		    fprintf(fp,"N E T%d <%s>%c",edge,
			    events[predstate->enabled]->event,
			    predstate->iteration);
		  } else {
		    fprintf(fp,"N E T%d %s",edge,
			    events[predstate->enabled]->event);
		  }
		  first=false;
		} else if (first) {
		  skip_edge=true;
		  break;
		} else {
		  if (predstate->iteration == 'I') 
		    fprintf(fp,",{%s}",signals[predstate2->enabled]->name);
		  else if (predstate->iteration != 'f') 
		    fprintf(fp,",<%s>%c",events[predstate2->enabled]->event,
			    predstate->iteration);
		}
	      }
	    }
          } else if (predstate->enabled==(nevents+nplaces)) {
	    fprintf(fp,"N E T%d DEAD\n",edge);
	  } else {
#ifdef PRED
	    get_trans(signals,nsignals,predstate->stateptr->state,
		      curstate->state,trans);
#else
	    get_trans(signals,nsignals,curstate->state,
		      predstate->stateptr->state,trans);
#endif
	    fprintf(fp,"N E T%d %s",edge,trans);
	  }
          if (!skip_edge) {
	    if ( stats )
	      fprintf( fp, ":%g", predstate->probability );
	    fprintf( fp, "\n" );
	  }
	}
        if (!skip_edge) {  
#ifdef PRED
	  fprintf(fp,"E P S%d 0 T%d 0\n",predstate->stateptr->number,edge);
	  fprintf(fp,"E P T%d 0 S%d 0\n",edge,curstate->number);
#else
	  fprintf(fp,"E P S%d 0 T%d 0\n",curstate->number,edge);
	  fprintf(fp,"E P T%d 0 S%d 0\n",edge,predstate->stateptr->number);
#endif
	  edge++;
	}
      }
  fprintf(fp,"ZZZZZ");
  fclose(fp);
  if (display) {
    sprintf(shellcommand,"parg %s.prg &",filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
  } else {
    sprintf(shellcommand,"pG %s",filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
  }
  system(shellcommand);
}
