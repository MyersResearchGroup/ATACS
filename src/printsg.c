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
/* printsg.c                                                                 */
/*****************************************************************************/

#include "printsg.h"

#define PRED

/*****************************************************************************/
/* Print a reduced state graph using parseGraph by Tom Rokicki.              */
/*****************************************************************************/

void print_sg(char * filename,signalADT *signals,eventADT *events,
	      stateADT *state_table,int nsignals,bool display,bool genrg,
	      int nevents,int ninpsig,bool stats)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,j;
  int edge;
  stateADT curstate;
  statelistADT predstate;
  bool empty;
  bool dummy;

  if (genrg) {
    printf("ERROR:  Cannot generate state graph from region graph\n");
    fprintf(lg,"ERROR:  Cannot generate state graph from region graph\n");
    return;
  }
  strcpy(outname,filename);
  strcat(outname,".sg");
  printf("Storing reduced state graph to:  %s\n",outname);
  fprintf(lg,"Storing reduced state graph to:  %s\n",outname);
  fp=Fopen(outname,"w");
  fprintf(fp,".name %s\n",filename);
  fprintf(fp,".inputs");
  for (i=0;i<ninpsig;i++)
    fprintf(fp," %s",signals[i]->name);
  fprintf(fp,"\n");
  fprintf(fp,".outputs");
  for (i=ninpsig;i<nsignals;i++)
    fprintf(fp," %s",signals[i]->name);
  fprintf(fp,"\n");
  fprintf(fp,".state graph\n");
  empty=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) { 
	empty=FALSE;
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
	   predstate=predstate->next) {
#else
      for (predstate=curstate->succ;predstate;
	   predstate=predstate->next) {
#endif
	dummy=TRUE;
	for (j=0;j<nsignals;j++) 

#ifdef PRED
	  if ((predstate->stateptr->state[j]=='R') && 
	      (VAL(curstate->state[j])=='1')) {
#else
	  if ((curstate->state[j]=='R') && 
	      (VAL(predstate->stateptr->state[j])=='1')) {
#endif
#ifdef PRED
	    fprintf(fp,"S%d %s+ S%d\n",predstate->stateptr->number,
		    signals[j]->name,curstate->number);
#else
	    fprintf(fp,"S%d %s+ S%d\n",curstate->number,
		    signals[j]->name,predstate->stateptr->number);
#endif
	    edge++;
	    dummy=FALSE;
#ifdef PRED
	  } else if ((predstate->stateptr->state[j]=='F') && 
		     (VAL(curstate->state[j])=='0')) {
#else
	  } else if ((curstate->state[j]=='F') && 
		     (VAL(predstate->stateptr->state[j])=='0')) {
#endif
#ifdef PRED
	    fprintf(fp,"S%d %s- S%d\n",predstate->stateptr->number,
		    signals[j]->name,curstate->number);
#else
	    fprintf(fp,"S%d %s- S%d\n",curstate->number,
		    signals[j]->name,predstate->stateptr->number);
#endif

	    edge++;
	    dummy=FALSE;
	  }
	if (dummy) {
#ifdef PRED
	    fprintf(fp,"S%d $ S%d\n",predstate->stateptr->number,
		    curstate->number);
#else
	    fprintf(fp,"S%d $ S%d\n",curstate->number,
		    predstate->stateptr->number);
#endif

	  edge++;
	}
      }
  fprintf(fp,".marking {S0}\n");
  fprintf(fp,".end\n");
  fclose(fp);
}
