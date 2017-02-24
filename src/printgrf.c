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
/* printgrf.c                                                                */
/*****************************************************************************/

#include "printgrf.h"

/*****************************************************************************/
/* Print a graph using parseGraph by Tom Rokicki.                            */
/*****************************************************************************/

void print_grf(char * filename,eventADT *events,ruleADT **rules,int nevents,
	       bool display,markingADT marking)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int i,j,k,l;
  int **place;
  int nextp,p;
  int eps;
  bool shared;
  bool print;

  strcpy(outname,filename);
  strcat(outname,".prg");
  printf("Storing constraint graph to:  %s\n",outname);
  fprintf(lg,"Storing constraint graph to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  place=(int **)GetBlock(nevents * sizeof(int*));
  for (i=1;i<nevents;i++) {
    place[i]=(int *)GetBlock(nevents * sizeof(int));
    for (j=1;j<nevents;j++) 
      place[i][j]=0;
    fprintf(fp,"N F T%d %s\n",i,events[i]->event);
  }
  nextp=0;
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  (rules[i][j]->ruletype < ORDERING)) {
	p=0;
	if (marking) { 
	  if (marking->marking[rules[i][j]->marking]!='F')
	    eps=1;
	  else 
	    eps=0;
	} else
	  eps=rules[i][j]->epsilon;
	shared=FALSE;
	for (k=1;k<nevents;k++)
	  if ((rules[i][k]->conflict & ICONFLICT) &&
	      (rules[k][j]->ruletype != NORULE) &&
	      (rules[k][j]->ruletype < ORDERING)) { 
	    if ((p != 0) && (p != place[k][j])) {
	      print=TRUE;
	      for (l=1;l<nevents;l++)
		if (place[i][l]==p) print=FALSE;
	      if (print) fprintf(fp,"E P T%d 0 P%d 0\n",i,p);
	      print=TRUE;
	      for (l=1;l<nevents;l++)
		if (place[l][j]==p) print=FALSE;
	      if (print) fprintf(fp,"E P P%d 0 T%d 0\n",p,j);
	    }
	    if (place[k][j] != 0) p=place[k][j];
	    shared=TRUE;
	    if ((!marking) && (rules[k][j]->epsilon==0)) eps=0; 
	    if ((marking) && (marking->marking[rules[k][j]->marking]!='F'))
	      eps=1;
	  }
	if (p==0) 
	  for (k=1;k<nevents;k++)
	    if ((rules[k][j]->conflict & OCONFLICT) &&
		(rules[i][k]->ruletype != NORULE) &&
		(rules[i][k]->ruletype < ORDERING)) {
	      if ((p != 0) && (p != place[i][k])) {
		print=TRUE;
		for (l=1;l<nevents;l++)
		  if (place[i][l]==p) print=FALSE;
		if (print) fprintf(fp,"E P T%d 0 P%d 0\n",i,p);
		print=TRUE;
		for (l=1;l<nevents;l++)
		  if (place[l][j]==p) print=FALSE;
		if (print) fprintf(fp,"E P P%d 0 T%d 0\n",p,j);
	      }
	      if (place[i][k] != 0) p=place[i][k];
	      shared=TRUE;
	      if ((!marking) && (rules[i][k]->epsilon==0)) eps=0; 
	      if ((marking) && (marking->marking[rules[i][k]->marking]!='F'))
		eps=1;
	    }
	if (p==0) {
	  p=++nextp;
	  if (eps==1) 
	    fprintf(fp,"N M P%d [%d,",p,rules[i][j]->lower);
	  else if (shared)
	    fprintf(fp,"N P P%d [%d,",p,rules[i][j]->lower);
	  else fprintf(fp,"N E P%d [%d,",p,rules[i][j]->lower);
	  printdelay(fp,rules[i][j]->upper);
	  fprintf(fp,"]");
	  if (rules[i][j]->expr)
	    fprintf(fp,"%s",rules[i][j]->expr);
	  fprintf(fp,"\n");
	}
	print=TRUE;
	for (k=1;k<nevents;k++)
	  if (place[i][k]==p) print=FALSE;
	if (print) fprintf(fp,"E P T%d 0 P%d 0\n",i,p);
	print=TRUE;
	for (k=1;k<nevents;k++)
	  if (place[k][j]==p) print=FALSE;
	if (print) fprintf(fp,"E P P%d 0 T%d 0\n",p,j);
	place[i][j]=p;
      }
  for (i=1;i<nevents;i++) 
    free(place[i]);
  free(place);
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
