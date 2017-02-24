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
/* struct2net.c                                                              */
/*****************************************************************************/

#include "struct2net.h"

/*****************************************************************************/
/* Convert a timed event-rule structure to an orbital net.                   */
/*****************************************************************************/

void struct2net(char * filename,eventADT *events,ruleADT **rules,int nevents)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,j,k;
  int **place;
  int p;
  bool shared;
  char phase;

  strcpy(outname,filename);
  strcat(outname,".net");
  printf("Storing orbital net to:  %s\n",outname);
  fprintf(lg,"Storing orbital net to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
/*
  fprintf(fp,";\n");
  fprintf(fp,";  The specification for %s\n",filename);
  fprintf(fp,";\n");
  fprintf(fp,"(def (%s-spec ",filename);
  if (instances) print_signal_instance_list(fp,events,ninputs,nevents,' ');
  else print_signal_list(fp,events,ninputs,nevents,' ');
  fprintf(fp,"(wire-init-set! ");
  print_initial_state(fp,events,rules,cycles,nevents,ncycles,startstate,TRUE);
  fprintf(fp,")\n");
*/
  fprintf(fp,"(complete () (net\n");
  fprintf(fp,"\t(with-transitions (");
  place=(int **)GetBlock(nevents * sizeof(int*));
  for (i=1;i<nevents;i++) {
    place[i]=(int *)GetBlock(nevents * sizeof(int));
    for (j=1;j<nevents;j++) 
      place[i][j]=0;
    fprintf(fp,"(t%d ",i);
    for (k=0;
	 (events[i]->event[k] != '\0');
	 k++)
      if ((events[i]->event[k] != '+') && (events[i]->event[k] != '-'))
	fprintf(fp,"%c",events[i]->event[k]);
      else phase=events[i]->event[k];
    fprintf(fp,"%c)",phase);
  }
  fprintf(fp,")\n");

  fprintf(fp,"\t\t(with-places (");
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  (rules[i][j]->ruletype < ORDERING)) {
	p=0;
	shared=FALSE;
	for (k=1;k<nevents;k++)
	  if ((rules[i][k]->conflict) &&
	      (rules[k][j]->ruletype != NORULE) &&
	      (rules[k][j]->ruletype < ORDERING)) {
	    shared=TRUE;
	    p=place[k][j];
	  }
	if (p==0)
	  for (k=1;k<nevents;k++)
	    if ((rules[k][j]->conflict) &&
		(rules[i][k]->ruletype != NORULE) &&
		(rules[i][k]->ruletype < ORDERING)) {
	      p=place[i][k];
	      shared=TRUE;
	    }
	if (p==0) {
	  p=(i-1)*(nevents-1)+j;
	  if (rules[i][j]->epsilon==1)
	    fprintf(fp,"(p%d 1) ",p);
	  else
	    fprintf(fp,"p%d ",p);
	}
	place[i][j]=p;
      }
  fprintf(fp,")\n");

  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  (rules[i][j]->ruletype < ORDERING)) {
	fprintf(fp,"\t\t\t(make-edges (t%d) (p%d) (t%d))\n",i,place[i][j],j);
      }
  fprintf(fp,")))))\n");
  for (i=1;i<nevents;i++) 
    free(place[i]);
  free(place);
  fclose(fp);
}

