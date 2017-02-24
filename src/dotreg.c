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
/* dotreg.c                                                                  */
/*****************************************************************************/

#include "dotreg.h"

/*****************************************************************************/
/* Output all regions in form readable Tom Rokicki's parseGraph.             */
/*****************************************************************************/

void dot_print_reg(char * filename,signalADT *signals,stateADT *state_table,
		   int nsignals,bool display)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,j,k,l;
  int edge;
  stateADT curstate;
  statelistADT predstate;
  bool empty;
  char regtype;

  for (k=0;k<nsignals;k++)
    for (l=0;l<4;l++) {
      if (l==0) regtype='R'; 
      else if (l==1) regtype='1'; 
      else if (l==2) regtype='F'; 
      else regtype='0'; 
      sprintf(outname,"%s%s%c.dot",filename,signals[k]->name,regtype);
      printf("Storing %s %c regions to:  %s\n",signals[k]->name,regtype,
	     outname);
      fprintf(lg,"Storing %s %c regions to:  %s\n",signals[k]->name,regtype,
	      outname);
      fp=Fopen(outname,"w");
      fprintf(fp,"digraph G {\n");
      fprintf(fp,"size=\"7.5,10\"\n");
      empty=TRUE;
      for (i=0;i<PRIME;i++)
	for (curstate=state_table[i];
	     curstate != NULL && curstate->state != NULL;
	     curstate=curstate->link) {
	  empty=FALSE;
/*	  fprintf(fp,"s%d [shape=ellipse,label=\"%s\"];\n",
		  curstate->number,curstate->state);  */
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
	  if (curstate->state[k]==regtype) 
	    for (predstate=curstate->pred;predstate;
		 predstate=predstate->next) {
	      for (j=0;j<nsignals;j++) 
		if ((predstate->stateptr->state[j]=='R') && 
		    (VAL(curstate->state[j])=='1')) {
		  fprintf(fp,"s%d [shape=ellipse,label=\"%s\"];\n",
			  curstate->number,curstate->state);
		  fprintf(fp,"s%d [shape=ellipse,label=\"%s\"];\n",
			  predstate->stateptr->number,
			  predstate->stateptr->state);
	          fprintf(fp,"s%d -> s%d [label=\"%s+\"];\n",
			  predstate->stateptr->number,curstate->number,
			  signals[j]->name);
		  edge++;
		} else if ((predstate->stateptr->state[j]=='F') && 
			   (VAL(curstate->state[j])=='0')) {
		  fprintf(fp,"s%d [shape=ellipse,label=\"%s\"];\n",
			  curstate->number,curstate->state);
		  fprintf(fp,"s%d [shape=ellipse,label=\"%s\"];\n",
			  predstate->stateptr->number,
			  predstate->stateptr->state);
                  fprintf(fp,"s%d -> s%d [label=\"%s-\"];\n",
			  predstate->stateptr->number,curstate->number,
			  signals[j]->name);
		  edge++;
		}
	    }
      fprintf(fp,"}");
      fclose(fp);
    }
}
