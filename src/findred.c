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
/* findred.c                                                                 */
/*****************************************************************************/

#include "findred.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* For each event with multiple enabling events check if some rules are      */
/*   redundant                                                               */
/*****************************************************************************/

bool find_redundant(char * filename,eventADT *events,ruleADT **rules,
		    cycleADT ****cycles,int nevents,
		    int ncycles,int *ncausal,int *nord,
		    bool si,bool heuristic,bool verbose,bool closure,
		    bool untimed)
{
  int i,u,v,j;
  //int x,y,z,a,b;
  boundADT timediff=NULL;
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  bool okay;
  int w,eps;
  bool print;
  bool warning;
  bool noconf;
  timeval t0,t1;
  double time0, time1;

  gettimeofday(&t0, NULL);
  if (untimed) return TRUE;
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".rr");
    printf("Finding redundant rules and storing to:  %s\n",outname);
    fprintf(lg,"Finding redundant rules and storing to:  %s\n",outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Finding redundant rules ... ");
    fflush(stdout);
    fprintf(lg,"Finding redundant rules ... ");
    fflush(lg);
  }
  /*
  for (x=1;x<nevents;x++)
    for (y=1;y<nevents;y++)
      for (z=0;z<2;z++)
	if (reachable(cycles,nevents,ncycles,x,0,y,z)==INFIN) {
	  if (!verbose) {
	    printf("WARNING.");
	    fprintf(lg,"WARNING.");
	  }
	  printf("WARNING: Cycle detected in acyclic constraint graph!\n");
	  fprintf(lg,"WARNING: Cycle detected in acyclic constraint graph!\n");
	  return TRUE;
	}
  */
  warning=FALSE;
  for (u=1;u<nevents;u++)
    for (v=1;v<nevents;v++) {
      if ((!si) && (heuristic))
	for (j=0;j<2;j++) 
	  if ((rules[u][v]->ruletype != NORULE) && (rules[u][v]->epsilon==j)) {
	    if (closure) timediff=TSE_WCTimeDiff(u,v,j);
	    else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,u,v,j);
	    if (timediff->L > rules[u][v]->upper) {
	      noconf=FALSE;
	      for (i=1;i<nevents;i++)
		if ((!(rules[u][i]->conflict)) &&
		    (rules[i][v]->ruletype != NORULE)) {
		  free(timediff);
		  if (closure) 
		    timediff=TSE_WCTimeDiff(i,v,rules[i][v]->epsilon);
		  else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,i,v,
					   rules[i][v]->epsilon);
		  if (timediff->L <= rules[/*u*/i][v]->upper) noconf=TRUE;
		}

	      if (noconf) {
		if (verbose)
		  fprintf(fp,"< %s,%s,%d,[%d,%d] >\n",events[u]->event,
			  events[v]->event,rules[u][v]->epsilon,
			  rules[u][v]->lower,rules[u][v]->upper);
		rules[u][v]->ruletype=(rules[u][v]->ruletype | REDUNDANT);
	      }
	    } else if (timediff->Lbest > rules[u][v]->upper) {
	      if (verbose)
		fprintf(fp,"< %s,%s,%d,[%d,%d] > : Lbest = %d\n",
			events[u]->event,events[v]->event,rules[u][v]->epsilon,
			rules[u][v]->lower,rules[u][v]->upper,timediff->Lbest);
	      warning=TRUE;
	    }
	    free(timediff);
	  }

/*****************************************************************************/
/* Added to remove redundancies in speed-independent circuits.               */
/*****************************************************************************/
      if (/*(v > ninputs) &&*/ (!(rules[u][v]->ruletype & REDUNDANT)) &&
	  (rules[u][v]->ruletype != NORULE) && (rules[u][v]->expr == NULL) 
/*	  && (rules[u][v]->ruletype & ORDERING)*/ ) {
	print=TRUE;

	/* ????? */
	
	for (w=1;w<nevents;w++) 
	  if ((rules[w][v]->ruletype != NORULE) && 
	      (rules[u][w]->conflict) &&
      // (!(rules[w][v]->ruletype & ORDERING)) &&
	      (!(rules[w][v]->ruletype & REDUNDANT)))
	    print=FALSE;
	
	if (print) {
	  print=FALSE;
	  for (w=1;w<nevents;w++) 
	    if ((rules[w][v]->ruletype != NORULE) && 
		(!(rules[w][v]->ruletype & ORDERING)) &&
		(!(rules[w][v]->ruletype & REDUNDANT)) &&
		(rules[w][v]->lower==rules[u][v]->lower) &&
		(rules[w][v]->upper==rules[u][v]->upper)) {
	      eps=rules[u][v]->epsilon-rules[w][v]->epsilon; 
	      if (eps >= 0) {
		if (eps < ncycles)
		  if (reachable(cycles,nevents,ncycles,u,0,w,eps)==1) 
		    print=TRUE;
	      }
/* ??????
	    if ((print) && (!(rules[u][v]->ruletype & ORDERING)) &&
		(events[u]->signal != events[w]->signal)) 
	      print=FALSE;
*/
	    }
	}
	if (print)
	  for (w=1;w<nevents && print;w++) 
	    if (rules[/*u*/v][w]->conflict) /*
	      for (a=1;a<nevents && print;a++) 
		for (b=1;b<nevents && print;b++) 
		  if ((rules[b][v]->conflict) && (a!=v) && (a!=w) &&
		      (!(rules[a][u]->conflict)) &&
		      (!(rules[a][w]->conflict)) &&
		      (rules[w][v]->ruletype==NORULE) &&
		      (rules[u][b]->ruletype==NORULE) &&
		      (rules[a][b]->ruletype != NORULE) && 
		      (!(rules[a][b]->ruletype & ORDERING)) &&
		      (!(rules[a][b]->ruletype & REDUNDANT)) &&
		      (rules[w][b]->ruletype != NORULE) && 
		      (!(rules[w][b]->ruletype & ORDERING)) &&
		      (!(rules[w][b]->ruletype & REDUNDANT)) &&
		      (rules[a][v]->ruletype != NORULE) && 
		      (!(rules[a][v]->ruletype & ORDERING)) &&
		      (!(rules[a][v]->ruletype & REDUNDANT))) */ {
		    print=FALSE;
		    /*printf("u=%s v=%s a=%s b=%s w=%s\n",
			   events[u]->event,events[v]->event,events[a]->event,
			   events[b]->event,events[w]->event);*/
		  }
	/* THIS SEEMS TO BE BREAKING THINGS
	if (print) {
	  if (verbose) 
	    fprintf(fp,"< %s,%s,%d,[%d,%d] > ???\n",events[u]->event,
		    events[v]->event,rules[u][v]->epsilon,
		    rules[u][v]->lower,rules[u][v]->upper);
	  if (rules[u][v]->ruletype & ORDERING) (*nord)--;
	  else (*ncausal)--;
	  if (rules[u][v]->epsilon==1) {
	    for (w=1;w<nevents;w++) 
	      if ((rules[w][v]->ruletype != NORULE) && 
		  (!(rules[w][v]->ruletype & ORDERING)) &&
		  (!(rules[w][v]->ruletype & REDUNDANT)) &&
		  (rules[w][v]->epsilon==1)) print=FALSE;
	    if (print) {
	      if (verbose) 
		fprintf(fp,"< %s,%s,%d,[%d,%d] > ???\n",events[u]->event,
			events[v]->event,rules[u][v]->epsilon,
			rules[u][v]->lower,rules[u][v]->upper);
	      if (rules[u][v]->ruletype & ORDERING) (*nord)--;
	      else (*ncausal)--;
	      rules[0][v]->epsilon=(-1);
	      rules[0][v]->lower=(-1);
	      rules[0][v]->upper=(-1);
	      rules[0][v]->ruletype=NORULE;
	    }
	  }
	  rules[u][v]->epsilon=(-1);
	  rules[u][v]->lower=(-1);
	  rules[u][v]->upper=(-1);
	  rules[u][v]->ruletype=NORULE;
	  } */
      }
/*****************************************************************************/
    }
  okay=TRUE;
  if (heuristic)
    for (u=1;u<nevents;u++)
      for (v=1;v<nevents;v++) {
	if ((rules[u][v]->ruletype & ORDERING) &&
	    (!(rules[u][v]->ruletype & REDUNDANT))) {
	  if (okay) {
	    if (!verbose) {
	      printf("EXCEPTION!\n");
	      fprintf(lg,"EXCEPTION!\n");
	      printf("EXCEPTION:  Ordering rule violation!\n");
	      fprintf(lg,"EXCEPTION:  Ordering rule violation!\n");
	      if (warning) {
		printf("WARNING:  More cycles may remove more rules.\n");
		fprintf(lg,"WARNING:  More cycles may remove more rules.\n");
	      } 
	      return(FALSE);
	    } else {
	      fclose(fp);
	      strcpy(outname,filename);
	      strcat(outname,".err");
	      printf("EXCEPTION:  Storing ordering rule violations to:  %s\n",
		     outname);
	    fprintf(lg,"EXCEPTION:  Storing ordering rule violations to:  %s\n"
		    ,outname);
	      okay=FALSE;
	      fp=Fopen(outname,"w");
	    }
	  }
	  fprintf(fp,"Ordering rule < %s,%s,%d,%d > not redundant\n",
		  events[u]->event,events[v]->event,rules[u][v]->epsilon,
		  rules[u][v]->lower);
	}
      }
  if (verbose) fclose(fp);
  else {
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("done (%f)\n",time1-time0);
    fprintf(lg,"done (%f)\n",time1-time0);
  }
  if (warning) {
    printf("WARNING:  More cycles may remove more rules.\n");
    fprintf(lg,"WARNING:  More cycles may remove more rules.\n");
  }
  if (!okay) fclose(fp);
  return(okay);
}

/*****************************************************************************/
/* Find all redundant rules.                                                 */
/*****************************************************************************/

bool find_redundant_rules(char menu,char command,designADT design)
{
  bool verbose;

  if ((!(design->status & FOUNDRED)) /* && (!design->potonly)*/) {
    verbose=design->verbose;
    if ((menu==SYNTHESIS) && (command==FINDRED)) verbose=TRUE;
    if (!find_redundant(design->filename,design->events,design->rules,
			design->cycles,design->nevents,design->ncycles,
			&(design->ncausal),&(design->nord),
			(design->si | design->sis),design->heuristic,
			verbose,design->closure,design->untimed)) {
      if ((command==ADDORD) || (design->exception)) {
	verbose=design->verbose;
	if (command==ADDORD) verbose=TRUE;
	if (!add_ordering_rules(design->filename,design->events,design->rules,
				design->markkey,design->cycles,design->nevents,
				design->ninputs,&(design->nrules),
				design->ncycles,design->maxgatedelay,
				&(design->ncausal),&(design->nord),
				design->startstate,verbose))
	  return(FALSE);
	notreachable--;
	rulecpy(design->nevents,design->ncycles,design->cycles,design->rules,
		FALSE);
	if (command != ADDORD)
	  design->status=process_command(SYNTHESIS,command,design,NULL,FALSE,
					 design->si);
	return(FALSE);
      } else return(FALSE);
    }
    design->status=design->status | FOUNDRED;
  }
  return(TRUE);
}

