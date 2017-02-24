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
/* addpers.c                                                                 */
/*****************************************************************************/

#include "addpers.h"
#include "connect.h"

/*****************************************************************************/
/* Add a persistence rule.                                                   */
/*****************************************************************************/

bool add_persistence_rule(FILE *fp,eventADT *events,ruleADT **rules,
			  markkeyADT *markkey,cycleADT ****cycles,
			  int nevents,int *nrules,int ncycles,int ninputs,
			  int u,int v,char * startstate,int maxgatedelay,
			  int *ncausal,bool verbose)
{
  int x,j,inv,eps;

  if (ncycles==0) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR:  Cannot add persistence rules with 0 cycles!\n");
    fprintf(lg,"ERROR:  Cannot add persistence rules with 0 cycles!\n");
    return(FALSE);
  }
  inv=find_inverse_event(events,cycles,nevents,ncycles,u,&j,startstate);
  if (j==(-1)) j=rules[u][v]->epsilon;
  eps=j; /* -rules[u][v]->epsilon; */
  if (eps < 0) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Cannot determine epsilon for a persistence rule.\n");
    fprintf(lg,"ERROR: Cannot determine epsilon for a persistence rule.\n");
    return(FALSE);
  }
  rules[v][inv]->epsilon=eps;
  rules[v][inv]->lower=0;      
  if (verbose)
    fprintf(fp,"< %s,%s,%d,[%d,",events[v]->event,events[inv]->event,
	    rules[v][inv]->epsilon,rules[v][inv]->lower);
  if (inv > ninputs) {
    rules[v][inv]->ruletype=PERSISTENCE;
    rules[v][inv]->upper=maxgatedelay;
    if (verbose) fprintf(fp,"%d] >\n",rules[v][inv]->upper);
  } else {
    rules[v][inv]->ruletype=ORDERING;
    rules[v][inv]->upper=0;
    if (verbose) 
      fprintf(fp,"%d] > : Ordering Rule\n",rules[v][inv]->upper);
  }
  markkey[*nrules]->enabling=v;
  markkey[*nrules]->enabled=inv;
  markkey[*nrules]->epsilon=eps;
  (*nrules)++;
  (*ncausal)++;
  for (x=0;x<ncycles-eps;x++)
    cycles[v][inv][x][x+eps]->rule=TRUE;
  if ((eps==1) && (rules[0][inv]->ruletype==NORULE)) {
    rules[0][inv]->epsilon=0;
    rules[0][inv]->lower=0;
    rules[0][inv]->upper=0;
    if (verbose)
      fprintf(fp,"< %s,%s,0,[%d,",events[0]->event,events[inv]->event,
	      rules[0][inv]->lower);
    if (inv > ninputs) {
      rules[0][inv]->ruletype=PERSISTENCE;
      rules[0][inv]->upper=maxgatedelay;
      if (verbose) fprintf(fp,"%d] >\n",rules[0][inv]->upper);
    } else {
      rules[0][inv]->ruletype=ORDERING;
      rules[0][inv]->upper=0;
      if (verbose) fprintf(fp,"%d] > : Ordering Rule\n",rules[0][inv]->upper);
    }
    markkey[*nrules]->enabling=0;
    markkey[*nrules]->enabled=inv;
    markkey[*nrules]->epsilon=0;
    (*nrules)++;
    (*ncausal)++;
    cycles[0][inv][0][0]->rule=TRUE;
  }
  return(TRUE);
}

/*****************************************************************************/
/* Add all necessary persistence rules.                                      */
/*****************************************************************************/

bool add_persistence_rules(char * filename,eventADT *events,ruleADT **rules,
			   markkeyADT *markkey,cycleADT ****cycles,
			   int nevents,int ninputs,int *nrules,
			   int ncycles,int maxgatedelay,int *ncausal,
			   char * startstate,bool verbose)
{
  int i,j;
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  bool okay;
  bool modified;

  okay=TRUE;
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".pr");
    printf("Adding persistence rules and storing to:  %s\n",outname);
    fprintf(lg,"Adding persistence rules and storing to:  %s\n",outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Adding persistence rules ... ");
    fflush(stdout);
    fprintf(lg,"Adding persistence rules ... ");
    fflush(lg);
  }
  modified=TRUE;
  for (i=0;i<nevents;i++)
    for (j=ninputs+1;j<nevents;j++)
      if (rules[i][j]->ruletype & NONPERS) {
	if (!add_persistence_rule(fp,events,rules,markkey,cycles,nevents,
				  nrules,ncycles,ninputs,i,j,startstate,
				  maxgatedelay,ncausal,verbose)) 
	  modified=FALSE;
	else if (rules[i][j]->ruletype & NONPERS)
	  rules[i][j]->ruletype=rules[i][j]->ruletype-NONPERS;
      }
  if (verbose) fclose(fp);
  else {
    if (!modified) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    } else {
      printf("done\n");
      fprintf(lg,"done\n");
    }
  }
  if (!modified) {
    printf("ERROR: Unable to solve persistence violations!\n");
    fprintf(lg,"ERROR: Unable to solve persistence violations!\n");
  }
  return(modified);
}


