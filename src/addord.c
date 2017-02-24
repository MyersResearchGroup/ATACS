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
/* addord.c                                                                 */
/*****************************************************************************/

#include "addord.h"

/*****************************************************************************/
/* Add a new causal rule to solve an ordering violation.                     */
/*****************************************************************************/

bool add_causal_rule(FILE *fp,eventADT *events,ruleADT **rules,
		     markkeyADT *markkey,cycleADT ****cycles,
		     int nevents,int *nrules,int ncycles,int ninputs,
		     int u,int v,char * startstate,int maxgatedelay,
		     int *ncausal,bool verbose)
{
  int i,x,eps;

  for (i=1;i<nevents;i++) 
    if (rules[i][v]->ruletype & TRIGGER) {
      eps=rules[u][v]->epsilon-rules[i][v]->epsilon;
      if (eps < 0) {
	eps=0;
      }
      if (reachable(cycles,nevents,ncycles,i,0,u,eps)==1) return(FALSE);
      rules[u][i]->epsilon=eps;
      if (i <= ninputs)
	return(add_causal_rule(fp,events,rules,markkey,cycles,nevents,nrules,
			       ncycles,ninputs,u,i,startstate,maxgatedelay,
			       ncausal,verbose));
      rules[u][i]->lower=0;      
      rules[u][i]->upper=maxgatedelay;
      rules[u][i]->ruletype=TRIGGER;
      if (verbose)
	fprintf(fp,"< %s,%s,%d,[%d,%d] >\n",events[u]->event,events[i]->event,
		rules[u][i]->epsilon,rules[u][i]->lower,rules[u][i]->upper);
      markkey[*nrules]->enabling=u;
      markkey[*nrules]->enabled=i;
      markkey[*nrules]->epsilon=eps;
      (*ncausal)++;
      (*nrules)++;
      for (x=0;x<ncycles-eps;x++)
	cycles[u][i][x][x+eps]->rule=TRUE;
      if ((eps==1) && (rules[0][i]->ruletype==NORULE)) {
	rules[0][i]->epsilon=0;
	rules[0][i]->lower=0;
	rules[0][i]->upper=maxgatedelay;
	rules[0][i]->ruletype=TRIGGER;
	if (verbose)
	  fprintf(fp,"< %s,%s,0,[%d,%d] >\n",events[0]->event,events[i]->event,
		  rules[0][i]->lower,rules[0][i]->upper);
	markkey[*nrules]->enabling=0;
	markkey[*nrules]->enabled=i;
	markkey[*nrules]->epsilon=0;
	(*ncausal)++;
	(*nrules)++;
	cycles[0][i][0][0]->rule=TRUE;
      }
    }
  return(TRUE);
}

/*****************************************************************************/
/* Solve ordering rule violations.                                           */
/*****************************************************************************/

bool add_ordering_rules(char * filename,eventADT *events,ruleADT **rules,
			markkeyADT *markkey,cycleADT ****cycles,
			int nevents,int ninputs,int *nrules,
			int ncycles,int maxgatedelay,int *ncausal,
			int *nord,char * startstate,bool verbose)
{
  int i,j;
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  bool okay;

 okay=TRUE;
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".ors");
    printf("Solving ordering rule violations and storing to: %s\n",outname);
    fprintf(lg,"Solving ordering rule violations and storing to: %s\n",
	    outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Solving ordering rule violations ... ");
    fflush(stdout);
    fprintf(lg,"Solving ordering rule violations ... ");
    fflush(lg);
  }
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype & ORDERING) && 
	  (!(rules[i][j]->ruletype & REDUNDANT))) {
	if (j > ninputs) {
	  rules[i][j]->ruletype=rules[i][j]->ruletype-ORDERING+TRIGGER;
	  rules[i][j]->upper=maxgatedelay;
	  (*ncausal)++;
	  (*nord)--;
	  if (verbose)
	    fprintf(fp,"< %s,%s,%d,[%d,%d] >\n",events[i]->event,
		    events[j]->event,rules[i][j]->epsilon,rules[i][j]->lower,
		    rules[i][j]->upper);
	} else {
	  if (!add_causal_rule(fp,events,rules,markkey,cycles,nevents,nrules,
			       ncycles,ninputs,i,j,startstate,maxgatedelay,
			       ncausal,verbose)) {
	    if (verbose) fclose(fp);
	    else {
	      printf("EXCEPTION!\n");
	      fprintf(lg,"EXCEPTION!\n");
	    }
	    printf("EXCEPTION:  Unable to solve ordering rule violations!\n");
	    fprintf(lg,"EXCEPTION:  Unable to solve ordering rule ");
	    fprintf(lg,"violations!\n");
	    return(FALSE);
	  }
	  rules[i][j]->ruletype=rules[i][j]->ruletype-ORDERING;
	}
      }
  if (verbose) fclose(fp);
  else {
    printf("done\n");
    fprintf(lg,"done\n");
  }
  return(TRUE);
}


