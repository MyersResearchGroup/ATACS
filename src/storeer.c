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
/* storeer.c                                                                 */
/*****************************************************************************/

#include "storeer.h"

/*****************************************************************************/
/* Store current design as a timed event-rule structure.                     */
/*****************************************************************************/

void store_er(char * filename,eventADT *events,ruleADT **rules,
	      mergeADT *merges,int nevents,int ninputs,int ncausal,
	      int nord,int ndisj,int nconf,char * startstate,bool level)
{
  FILE *fp;
  int i,j,nred;
  mergeADT curmerge;
  char outname[FILENAMESIZE];

  strcpy(outname,filename);
  if (level)
    strcat(outname,".tel");
  else
    strcat(outname,".er");
  printf("Storing timed event-rule structure to:  %s\n",outname);
  fprintf(lg,"Storing timed event-rule structure to:  %s\n",outname);
  fp=fopen(outname,"w");
  if (fp==NULL) {
    printf("ERROR: Unable to open file:  %s\n",outname);
    fprintf(lg,"ERROR: Unable to open file:  %s\n",outname);
    return;
  }
  print_comment(fp,filename);
  nred=0;
/*
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  (rules[i][j]->ruletype & REDUNDANT) &&
	  (!(rules[i][j]->ruletype & ORDERING))) nred++;
*/
  fprintf(fp,".e %d\n",nevents);
  fprintf(fp,".i %d\n",ninputs);
  fprintf(fp,".r %d\n",ncausal-nred);
  fprintf(fp,".n %d\n",nord+nred);
  fprintf(fp,".d %d\n",ndisj);
  fprintf(fp,".c %d\n",nconf);
  if (startstate != NULL)
    fprintf(fp,".s %s\n",startstate);
  print_comment(fp,"List of Events");
  fprintf(fp,"reset\n");
  print_comment(fp,"List of Input Events");
  for (i=1;i<ninputs;i+=2)
    if (events[i]->type && events[i+1]->type)
      fprintf(fp,"%s:%d %s:%d\n",events[i]->event,events[i]->type,
	      events[i+1]->event,events[i+1]->type);
    else
      fprintf(fp,"%s %s\n",events[i]->event,events[i+1]->event);
  print_comment(fp,"List of Output Events");
  for (i=ninputs+1;i<nevents;i+=2)
    if (events[i]->event[0]=='$') {
      if (events[i]->type)
	fprintf(fp,"%s:%d\n",events[i]->event,events[i]->type);
      else
	fprintf(fp,"%s\n",events[i]->event);
      if (i+1 < nevents)
	if (events[i+1]->type)
	  fprintf(fp,"%s:%d\n",events[i+1]->event,events[i+1]->type);
	else
	  fprintf(fp,"%s\n",events[i+1]->event);
    } else
      if (events[i]->type && events[i+1]->type)
	fprintf(fp,"%s:%d %s:%d\n",events[i]->event,events[i]->type,
		events[i+1]->event,events[i+1]->type);
      else
	fprintf(fp,"%s %s\n",events[i]->event,events[i+1]->event);
  print_comment(fp,"List of Causal Rules");
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  ((rules[i][j]->ruletype & TRIGGER) ||
	   (rules[i][j]->ruletype & PERSISTENCE))) {
	if (rules[i][j]->expr)
	  fprintf(fp,"%s %s %s %d %d ",events[i]->event,events[j]->event,
		  rules[i][j]->expr,rules[i][j]->epsilon,rules[i][j]->lower);
	else
	  fprintf(fp,"%s %s %d %d ",events[i]->event,events[j]->event,
		  rules[i][j]->epsilon,rules[i][j]->lower);
	printdelay(fp,rules[i][j]->upper);
	fprintf(fp,"\n");
      }
  print_comment(fp,"List of Ordering Rules");
  for (i=0;i<nevents;i++)
    for (j=0;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  (rules[i][j]->ruletype & ORDERING)) {
	if (rules[i][j]->expr)
	  fprintf(fp,"%s %s %s %d %d ",events[i]->event,events[j]->event,
		  rules[i][j]->expr,rules[i][j]->epsilon,rules[i][j]->lower);
	else
	  fprintf(fp,"%s %s %d %d ",events[i]->event,events[j]->event,
		  rules[i][j]->epsilon,rules[i][j]->lower);
	printdelay(fp,rules[i][j]->upper);
	fprintf(fp,"\n");
      }
  print_comment(fp,"List of Mergers");
  for (i=1;i<nevents;i++)
    for (curmerge=merges[i]; curmerge != NULL; curmerge=curmerge->link)
      if (((curmerge->mergetype==CONJMERGE) || 
	   (curmerge->mergetype==DISJMERGE)) && (curmerge->mevent > i))
	fprintf(fp,"%s %s\n",events[i]->event,events[curmerge->mevent]->event);
  print_comment(fp,"List of Conflicts");
  for (i=0;i<nevents;i++)
    for (j=i+1;j<nevents;j++)
      if (rules[i][j]->conflict)
	fprintf(fp,"%s %s\n",events[i]->event,events[j]->event);
  fclose(fp);
}
