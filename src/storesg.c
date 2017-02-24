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
/* storesg.c                                                                 */
/*****************************************************************************/

#include "storesg.h"
#include "findreg.h"

/*****************************************************************************/
/* Store reduced state graph.                                                */
/*****************************************************************************/

void store_state_graph(char * filename,signalADT *signals,eventADT *events,
		       stateADT *state_table,int ninpsig,int nsignals)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i;
  stateADT curstate;
  statelistADT predstate;

  strcpy(outname,filename);
  strcat(outname,".rsg");
  printf("Storing reduced state graph to:  %s\n",outname);
  fprintf(lg,"Storing reduced state graph to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  fprintf(fp,"SG:\n");
  print_state_vector(fp,signals,nsignals,ninpsig);
  fprintf(fp,"STATES:\n");
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      fprintf(fp,"%d:%s\n",curstate->number,curstate->state);
    }
  fprintf(fp,"EDGES:\n");
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      for (predstate=curstate->pred;predstate;
	   predstate=predstate->next) {
	fprintf(fp,"%d:%s - %s -> %d:%s\n",predstate->stateptr->number,
		predstate->stateptr->state,
		events[predstate->enabled]->event,
		curstate->number,curstate->state);
    }
  fclose(fp);
}
