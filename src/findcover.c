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
/* findcover.c                                                               */
/*****************************************************************************/

#include "findcover.h"
#include "def.h"
#include "findreg.h"
#include "resolve.h"
#include "storeprs.h"
#include "findconf.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* Print unresolved conflicts.                                               */
/*****************************************************************************/

void print_unresolved_conflicts(FILE *fp,signalADT *signals,
				contextADT context_table,int i,int l,
				int nsignals)
{
  contextADT new_entry=NULL;
  int k;

  if (context_table != NULL) {
    fprintf(fp,"Unresolved Conflicts for %s",signals[i]->name);
    if (l==1) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
    for (new_entry=context_table;
	 new_entry != NULL;
	 new_entry=new_entry->next) {
      fprintf(fp,"%s:",new_entry->state);
      for (k=0;k<nsignals;k++)
	if (new_entry->solutions[k]=='1')
	  fprintf(fp," %s",signals[k]->name);
	else if (new_entry->solutions[k]=='0')
	  fprintf(fp," ~%s",signals[k]->name);
        else if (new_entry->solutions[k]==('1'+2))
          fprintf(fp," %s(O)",signals[k]->name);
        else if (new_entry->solutions[k]==('0'+2)) 
          fprintf(fp," ~%s(O)",signals[k]->name);
      fprintf(fp,"\n");
    }
  }
}

/*****************************************************************************/
/* Determine if a problem has only one possible solution, and if so, the     */
/*   signal associated with that solution is added to the guard and all      */
/*   problems solved by this signal are removed from the table.              */
/*****************************************************************************/

int distinguished(char * solutions,int nsignals)
{
  int k,signal;

  signal=(-1);
  for (k=0;k<nsignals;k++) {
    if ((solutions[k]=='1'+2) || (solutions[k]=='0'+2)) return(-1);
    if ((solutions[k]=='1') || (solutions[k]=='0')) 
      if (signal==(-1)) 
	signal=k;
      else return(-1);
  }
  return(signal);
}

/*****************************************************************************/
/* Determine if a problem has only a single dot and blanks elsewhere, then   */
/*   remove the row and column from the table.                               */
/*****************************************************************************/

int dotted(char * solutions,int nsignals)
{
  int k,signal;

  signal=(-1);
  for (k=0;k<nsignals;k++) {
    if ((solutions[k]=='1') || (solutions[k]=='0')) return(-1);
    if ((solutions[k]=='1'+2) || (solutions[k]=='0'+2)) 
      if (signal==(-1)) 
	signal=k;
      else return(-1);
  }
  return(signal);
}

/*****************************************************************************/
/* Remove a problem from a context rule table.                               */
/*****************************************************************************/

void rm_problem(contextADT *context_table,contextADT new_entry) 
{
  if (new_entry->prev != NULL)
     new_entry->prev->next=new_entry->next;
  else
    (*context_table)=new_entry->next;
  if (new_entry->next != NULL)
    new_entry->next->prev=new_entry->prev;
  free(new_entry->state);
  free(new_entry->solutions);
  free(new_entry);
}

/*****************************************************************************/
/* Remove solved problems from a context rule table.                         */
/*****************************************************************************/

void rm_solved_problems(contextADT *context_table,int signal,char phase)
{
  contextADT cur_problem=NULL;
  contextADT next_problem=NULL;

  cur_problem=(*context_table);
  while (cur_problem != NULL) {
    next_problem=cur_problem->next;
    if ((cur_problem->solutions[signal] == '0') ||
	(cur_problem->solutions[signal] == '1') ||
	((phase=='0') && (cur_problem->solutions[signal] == '1'+2) ||
	((phase=='1') && (cur_problem->solutions[signal] == '0'+2))))
      rm_problem(context_table,cur_problem);
    else if ((cur_problem->solutions[signal] == '0'+2) ||
	     (cur_problem->solutions[signal] == '1'+2))
      cur_problem->solutions[signal]='X';
    cur_problem=next_problem;
  }
}

/*****************************************************************************/
/* Remove dotted problems from a context rule table.                         */
/*****************************************************************************/

void rm_dotted_problems(contextADT *context_table,int signal,char phase)
{
  contextADT cur_problem=NULL;
  contextADT next_problem=NULL;

  cur_problem=(*context_table);
  while (cur_problem != NULL) {
    next_problem=cur_problem->next;
    if ((cur_problem->solutions[signal] == '0'+2) ||
	(cur_problem->solutions[signal] == '1'+2))
      rm_problem(context_table,cur_problem);
    else if (((phase=='0'+2) && (cur_problem->solutions[signal] == '0')) ||
	     ((phase=='1'+2) && (cur_problem->solutions[signal] == '1')))
      cur_problem->solutions[signal]='X';
    cur_problem=next_problem;
  }
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   context rule and remove solved problems from the context rule table.    */
/*****************************************************************************/
  
void choose_essential_rows(FILE *fp, signalADT *signals,
			   contextADT *context_table,int i,int l,char * cover,
			   int nsignals,bool verbose)
{
  int signal;
  contextADT new_entry=NULL;

  new_entry=(*context_table);
  while (new_entry != NULL) 
    if ((signal=distinguished(new_entry->solutions,nsignals)) >= 0) {
      cover[signal]=new_entry->solutions[signal];
      if (verbose) {
	if (cover[signal]=='1')
	  fprintf(fp,"Added %s to cover for %s",signals[signal]->name,
		  signals[i]->name);
	else
	  fprintf(fp,"Added ~%s to cover for %s",signals[signal]->name,
		  signals[i]->name);
	if (l==1) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
      }
      rm_solved_problems(context_table,signal,new_entry->solutions[signal]);
      new_entry=(*context_table);
    } else new_entry=new_entry->next;
}

/*****************************************************************************/
/* Determine if a row is dotted, and if so, remove dotted problems from the  */
/*   context rule table.                                                     */
/*****************************************************************************/
  
void eliminate_dotted_rows(contextADT *context_table,int nsignals)
{
  int signal;
  contextADT new_entry=NULL;

  new_entry=(*context_table);
  while (new_entry != NULL) 
    if ((signal=dotted(new_entry->solutions,nsignals)) >= 0) {
      rm_dotted_problems(context_table,signal,new_entry->solutions[signal]);
      new_entry=(*context_table);
    } else new_entry=new_entry->next;
}

/*****************************************************************************/
/* Determine if one column dominates another.                                */
/*****************************************************************************/
 
int dom_col(char * sol1,char * sol2,int nsignals)
{
  int k;
  int dominate;

  dominate=(-1);
  for (k=0;k<nsignals;k++)
    if (sol1[k] != sol2[k])
      if (sol1[k]=='X') 
	if ((dominate!=1) && (dominate!=3))
	  dominate=2;
	else dominate=3;
      else if (sol2[k]=='X') 
	if ((dominate!=2) && (dominate!=3))
	  dominate=1;
	else dominate=3;
      else dominate=3;
  if (dominate==(-1)) dominate=1;
  return(dominate);
}

/*****************************************************************************/
/* Remove dominating columns from a context rule table.                      */
/*****************************************************************************/

void rm_dominating_columns(contextADT *context_table,int nsignals)
{
  contextADT cur_problem1=NULL;
  contextADT cur_problem2=NULL;
  contextADT next_problem1=NULL;
  contextADT next_problem2=NULL;
  int dominate;

  cur_problem1=(*context_table);
  while (cur_problem1 != NULL) {
    next_problem1=cur_problem1->next;
    if (cur_problem1->next != NULL) {
      cur_problem2=cur_problem1->next;
      while (cur_problem2 != NULL) {
	next_problem2=cur_problem2->next;
	dominate=dom_col(cur_problem1->solutions,cur_problem2->solutions,
			 nsignals);
	if (dominate==1) {
	  rm_problem(context_table,cur_problem1);
	  cur_problem2=NULL;
	} else if (dominate==2) { 
	  if (cur_problem2==next_problem1) 
	    next_problem1=next_problem2;
	  rm_problem(context_table,cur_problem2);
	  cur_problem2=next_problem2;
	} else cur_problem2=next_problem2;
      }
    }
    cur_problem1=next_problem1;
  }
}

/*****************************************************************************/
/* Determine if a context signal is symmetric to another signal in network   */
/*   for signal being designed.                                              */
/*****************************************************************************/

bool symmetric(regionADT *regions,int signal,int event)
{
  regionADT cur_region;

  for (cur_region=regions[event];cur_region;cur_region=cur_region->link)
    if ((cur_region->cover[signal]=='0') ||
	(cur_region->cover[signal]=='1')) return(TRUE);
  for (cur_region=regions[(event-1)+(2*(event % 2))];cur_region;
       cur_region=cur_region->link)
    if ((cur_region->cover[signal]=='0') ||
	(cur_region->cover[signal]=='1')) return(TRUE);
  return(FALSE);
}

int check_restrict_dom(char * cps,char * cqs,int ri,int rj,int nsignals)
{
  char cpsri,cpsrj,cqsri,cqsrj;
  int retval;

  cpsri=cps[ri];
  cpsrj=cps[rj];
  cqsri=cqs[ri];
  cqsrj=cqs[rj];
  cps[ri]='X';
  cps[rj]='X';
  cqs[ri]='X';
  cqs[rj]='X';
  retval=dom_col(cps,cqs,nsignals);
  cps[ri]=cpsri;
  cps[rj]=cpsrj;
  cqs[ri]=cqsri;
  cqs[rj]=cqsrj;

  return retval;
}

bool find_dotted_col(contextADT *context_table,int ri,int rj,
		     contextADT cp,int nsignals)
{
  contextADT cq=NULL;

  for (cq=(*context_table);cq != NULL; cq=cq->next)
    if ((cq->solutions[rj]=='0'+2) || (cq->solutions[rj]=='1'+2))
      if (check_restrict_dom(cp->solutions,cq->solutions,ri,rj,nsignals)==1)
	return(TRUE);
  return(FALSE);
}

bool potentially_symmetric(regionADT *regions,int signal,int event)
{
  regionADT cur_region;

  for (cur_region=regions[event];cur_region;cur_region=cur_region->link)
    if ((cur_region->enstate[signal]=='0') ||
	(cur_region->enstate[signal]=='1')) return(TRUE);
  for (cur_region=regions[(event-1)+(2*(event % 2))];cur_region;
       cur_region=cur_region->link)
    if ((cur_region->enstate[signal]=='0') ||
	(cur_region->enstate[signal]=='1')) return(TRUE);
  return(FALSE);
}

/*****************************************************************************/
/* Determine number of sharing possibilities that are removed for a given    */
/*   context signal.                                                         */
/*****************************************************************************/

int share_cost(regionADT *regions,int signal,int event,char value,char * cover,
	       int nsignals)
{
  regionADT cur_region;
  int cost;

  cost=0;
  for (cur_region=regions[event];cur_region;cur_region=cur_region->link)
      if ((((cur_region->enstate[signal]=='0') && (value=='1'))||
	   ((cur_region->enstate[signal]=='1') && (value=='0'))) &&
	  (intersect(cur_region->enstate,cover,nsignals))) cost++;
  return cost;
}

/*****************************************************************************/
/* Determine signal1 removes any sharing possibilities that signal2 does not.*/
/*****************************************************************************/

int share_dom(regionADT *regions,int signal1,int signal2,int event,char value1,
	      char value2,char * cover,int nsignals,int dominate)
{
  regionADT cur_region;

  for (cur_region=regions[event];cur_region;cur_region=cur_region->link) {
      if ((((cur_region->enstate[signal1]!='1') && (value1=='1'))||
	   ((cur_region->enstate[signal1]!='0') && (value1=='0'))) &&
	  (((cur_region->enstate[signal2]!='0') && (value2=='1'))||
	   ((cur_region->enstate[signal2]!='1') && (value2=='0'))) /*&&
            (intersect(cur_region->enstate,cover,nsignals))*/) {
	if (dominate==1) return 3;
	else dominate=2;
      } else
	if ((((cur_region->enstate[signal1]!='0') && (value1=='1'))||
	     ((cur_region->enstate[signal1]!='1') && (value1=='0'))) &&
	    (((cur_region->enstate[signal2]!='1') && (value2=='1'))||
	     ((cur_region->enstate[signal2]!='0') && (value2=='0')))/* &&
              (intersect(cur_region->enstate,cover,nsignals))*/) {
	  if (dominate==2) return 3;
	  else dominate=1;
	}
  }
  return dominate;
}

bool potentially_antishare(regionADT *regions,int signal,int event,
			   char value)
{
  regionADT cur_region;

  for (cur_region=regions[event];cur_region;cur_region=cur_region->link)
    if (((cur_region->enstate[signal]=='0') && (value=='1'))||
	((cur_region->enstate[signal]=='1') && (value=='0'))) return(TRUE);
  return(FALSE);
}

bool potentially_share(regionADT *regions,int signal,int event,
		       char value)
{
  regionADT cur_region;

  for (cur_region=regions[event];cur_region;cur_region=cur_region->link)
    if (((cur_region->enstate[signal]!='1') && (value=='0'))||
	((cur_region->enstate[signal]!='0') && (value=='1'))) return(TRUE);
  return(FALSE);
}

/*****************************************************************************/
/* Determine if one row dominates another.                                   */
/*****************************************************************************/

int dom_row(regionADT *regions,contextADT *context_table,int i,int signal1,
	    int signal2,int nsignals,char * enstate,
	    char * cover,bool burst,signalADT *signals,int nsignal)
{
  contextADT problem=NULL;
  int dominate,cost1,cost2;

  dominate=(-1);
  cost1=0; 
  cost2=0;
  for (problem=(*context_table);
       problem != NULL;
       problem=problem->next) {
     if ((problem->solutions[signal1]=='X') &&
	 ((problem->solutions[signal2]=='0') || 
	  (problem->solutions[signal2]=='1'))) 
	if ((dominate!=1) && (dominate!=3))
	  dominate=2;
	else dominate=3;
     else if ((problem->solutions[signal2]=='X') &&
	      ((problem->solutions[signal1]=='0') || 
	      (problem->solutions[signal1]=='1'))) 
       if ((dominate!=2) && (dominate!=3))
	 dominate=1;
       else dominate=3;
     else if ((problem->solutions[signal1]=='0'+2) &&
	      (problem->solutions[signal2]!='0'+2) &&
	      (!find_dotted_col(context_table,signal1,signal2,problem,
				nsignals))) {
       if (dominate==1)
	 dominate=3;
       else if (dominate!=3)
	 dominate=2;
     } else if ((problem->solutions[signal1]!='0'+2) &&
		(problem->solutions[signal2]=='0'+2) &&
		(!find_dotted_col(context_table,signal2,signal1,problem,
				  nsignals))) {
       if (dominate==2)
	 dominate=3;
       else if (dominate!=3)
	 dominate=1;
     } else if ((problem->solutions[signal1]=='1'+2) &&
		(problem->solutions[signal2]!='1'+2) &&
		(!find_dotted_col(context_table,signal1,signal2,problem,
				  nsignals))) {
       if (dominate==1)
	 dominate=3;
       else if (dominate!=3)
	 dominate=2;
     } else if ((problem->solutions[signal1]!='1'+2) &&
		(problem->solutions[signal2]=='1'+2) &&
		(!find_dotted_col(context_table,signal2,signal1,problem,
				  nsignals))) {
       if (dominate==2)
	 dominate=3;
       else if (dominate!=3)
	 dominate=1;
     }
     if (dominate==3) break;
  }
  //printf("1:%s %c 2:%s %c %d\n",signals[signal1]->name,enstate[signal1],
  //	 signals[signal2]->name,enstate[signal2],dominate);
  if (dominate!=3)
    dominate=share_dom(regions,signal1,signal2,i,enstate[signal1],
		       enstate[signal2],cover,nsignals,dominate);
  /*
  if (dominate!=3) {
    cost1=share_cost(regions,signal1,i,enstate[signal1],cover,nsignal);
    cost2=share_cost(regions,signal2,i,enstate[signal2],cover,nsignal);
  }
  if ((cost1 > cost2) && (dominate==1)) dominate=3;
  else if ((cost2 > cost1) && (dominate==2)) dominate=3;
  */

  if (dominate==(-1)) {
    //printf("cost1=%d,cost2=%d\n",cost1,cost2);
    if (cost1 > cost2)
      dominate=2;
    else if (cost2 > cost1)
      dominate=1;
    else if (symmetric(regions,signal2,i))
      dominate=2;
    else if (symmetric(regions,signal1,i))
      dominate=1;
    else if (potentially_symmetric(regions,signal2,i))
      dominate=2;
    else if (potentially_symmetric(regions,signal1,i))
      dominate=1;
    else dominate=2;
  }
  return(dominate);
}

/*****************************************************************************/
/* Remove a possible solution from a context rule table.                     */
/*****************************************************************************/

bool rm_solution(contextADT *context_table,int signal,bool *reduced)
{
  contextADT cur_problem=NULL;
  contextADT next_problem=NULL;
  bool result;

  result=FALSE;
  cur_problem=(*context_table);
  while (cur_problem != NULL) {
    next_problem=cur_problem->next;
    if ((cur_problem->solutions[signal]=='0'+2) ||
	(cur_problem->solutions[signal]=='1'+2)) {
      rm_problem(context_table,cur_problem);
      (*reduced)=TRUE;
      result=TRUE;
    } else if (cur_problem->solutions[signal] != 'X') {
      cur_problem->solutions[signal]='X';
      (*reduced)=TRUE;
      result=TRUE;
    }
    cur_problem=next_problem;
  }
  return result;
}

/*****************************************************************************/
/* Remove dominated rows.                                                    */
/*****************************************************************************/
   
bool rm_dominated_rows(regionADT *regions,contextADT *context_table,
		       int i,int nsignals,char * enstate,
		       char * cover,bool burst,signalADT *signals,
		       int nsignal)
{
  int k,l,dominate;
  bool reduced;
  bool *dom_cache;

  dom_cache=(bool*)GetBlock((nsignals+1)*sizeof(bool));
  for (k=0;k<nsignals;k++)
    if (cover[k]=='Z') dom_cache[k]=FALSE;
    else dom_cache[k]=TRUE;
  reduced=FALSE;
  for (k=0;k<nsignals-1;k++)
    for (l=k+1;l<nsignals;l++) 
      if ((enstate[k]!='X' && enstate[l]!='X') &&
	  (!dom_cache[k] && !dom_cache[l])) {
	dominate=dom_row(regions,context_table,i,k,l,nsignals,enstate,
			 cover,burst,signals,nsignal);
	if (dominate==1) {
	  rm_solution(context_table,l,&reduced);
	  dom_cache[l]=TRUE;
	  //printf("Removing %s because of %s\n",signals[l]->name,
	  // signals[k]->name);
	} else if (dominate==2) {
	  rm_solution(context_table,k,&reduced);
	  dom_cache[k]=TRUE;
	  break;
	//	  printf("Removing %s because of %s\n",signals[k]->name,
	//	 signals[l]->name);
      }
    }
  free(dom_cache);
  return(reduced);
}

/*****************************************************************************/
/* Remove dotted problems from a context rule table.                         */
/*****************************************************************************/

void rm_dotted_rows(contextADT *context_table,int nsignals)
{
  contextADT cur_problem=NULL;
  contextADT next_problem=NULL;
  int j;
  bool dotted;

  for (j=0;j<nsignals;j++) {
    dotted=TRUE;
    for (cur_problem=(*context_table);cur_problem;
	 cur_problem=cur_problem->next)
      if ((cur_problem->solutions[j]=='0') ||
	  (cur_problem->solutions[j]=='1')) dotted=FALSE;
    if (dotted) {
      cur_problem=(*context_table);
      while (cur_problem) {
	next_problem=cur_problem->next;
	if ((cur_problem->solutions[j]=='0'+2) ||
	    (cur_problem->solutions[j]=='1'+2))
	  rm_problem(context_table,cur_problem);
	else 
	  cur_problem->solutions[j]='X';
	cur_problem=next_problem;
      }
    }
  }
}

/*****************************************************************************/
/* Determine the number of remaining problems to solve in a context rule     */
/*   table.                                                                  */
/*****************************************************************************/

int numprob(contextADT context_table,int nsignals)
{
  contextADT new_entry=NULL;
  int count,j;

  count=0;
  for (new_entry=context_table;
       new_entry != NULL;
       new_entry=new_entry->next) {
    for (j=0;j<nsignals;j++)
      if (new_entry->solutions[j]!='X')
	break;
    if (j==nsignals) return -1;
    count++;
  }
  return(count);
}

/*****************************************************************************/
/* Find an optimal cover of context signals to solve all conflicts.          */
/*****************************************************************************/

int find_cov(char * filename,signalADT *signals,regionADT *regions,
	     int ninpsig,int nsignals,bool verbose,bool burst,bool inponly)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  FILE *errfp=NULL;
  int i,l,nprob,oldnprob,nsigs;
  bool reduced,okay,soln;
  regionADT cur_region;
  timeval t0,t1;
  double time0, time1;

  gettimeofday(&t0, NULL);
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".cov");
    printf("Finding covers and storing to:  %s\n",outname);
    fprintf(lg,"Finding covers and storing to:  %s\n",outname);
    fp=Fopen(outname,"w");
  } else {
    fp=NULL;
    printf("Finding covers ... ");
    fflush(stdout);
    fprintf(lg,"Finding covers ... ");
    fflush(lg);
  }
  okay=TRUE;
  soln=TRUE;
  nsigs=nsignals;
  if (inponly) nsigs=ninpsig;
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++) 
      for (cur_region=regions[2*i+l]; cur_region; 
	   cur_region=cur_region->link) {
	oldnprob=INFIN;
	reduced=TRUE;
	while (((nprob=numprob(cur_region->context_table,nsignals)) > 0) && 
	       ((nprob < oldnprob) || (reduced))) {
	  oldnprob=nprob;
	  choose_essential_rows(fp,signals,&(cur_region->context_table),
				i,l,cur_region->cover,nsigs,FALSE);
	  eliminate_dotted_rows(&(cur_region->context_table),nsigs);
	  rm_dotted_rows(&(cur_region->context_table),nsigs);
	  rm_dominating_columns(&(cur_region->context_table),nsigs);
	  //printf("signal=%s,phase=%d\n",signals[i]->name,l);
	  reduced=rm_dominated_rows(regions,&(cur_region->context_table),
				    2*i+l,nsigs,cur_region->enstate,
				    cur_region->cover,burst,signals,nsignals); 
	}
	if ((nprob > 0) || (nprob < 0)) {

	  if ((nprob < 0) && (soln)) {
	    if (!verbose) {
	      printf("EXCEPTION!\n");
	      fprintf(lg,"EXCEPTION!\n");
	      printf("EXCEPTION:  Unresolvable conflicts!\n");
	      fprintf(lg,"EXCEPTION:  Unresolvable conflicts!\n");
	      soln=FALSE;
	    } else {
	      strcpy(outname,filename);
	      strcat(outname,".err");
	      printf("EXCEPTION:  Storing unresolvable conflicts to:  %s\n",
		     outname);
	      fprintf(lg,"EXCEPTION:  Storing unresolvable conflicts to:  %s\n",
		      outname);
	      if (okay) {
		errfp=Fopen(outname,"w");
		fprintf(errfp,"UNRESOLVED CONFLICTS:\n");
		print_state_vector(errfp,signals,nsignals,ninpsig);
		okay=FALSE;
	      }
	      soln=FALSE;
	    }
	  } 
	  if ((soln) && (okay) && (nprob > 0)) {
	    if (!verbose) {
	      printf("EXCEPTION!\n");
	      fprintf(lg,"EXCEPTION!\n");
	      printf("EXCEPTION:  Unresolved conflicts!\n");
	      fprintf(lg,"EXCEPTION:  Unresolved conflicts!\n");
	      okay=FALSE;
	    } else {
	      strcpy(outname,filename);
	      strcat(outname,".err");
	      printf("EXCEPTION:  Storing unresolved conflicts to:  %s\n",
		     outname);
	      fprintf(lg,"EXCEPTION:  Storing unresolved conflicts to:  %s\n",
		      outname);
	      errfp=Fopen(outname,"w");
	      fprintf(errfp,"UNRESOLVED CONFLICTS:\n");
	      print_state_vector(errfp,signals,nsignals,ninpsig);
	      okay=FALSE;
	    }
	  }
	  if (verbose) print_unresolved_conflicts(errfp,signals,
						  cur_region->context_table,
						  i,l,nsignals);
	}
      }
  if (verbose) {
    if ((!okay)||(!soln)) fclose(errfp);
    for (i=2*ninpsig;i<(2*nsignals+1);i++) 
      print_covers(fp,filename,signals,regions,FALSE,NULL,FALSE,FALSE,i,
		   nsignals,INFIN,NULL,NULL,NULL,NULL,NULL,NULL);
    fclose(fp);
  } else if ((okay) && (soln)) {
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("done (%f)\n",time1-time0);
    fprintf(lg,"done (%f)\n",time1-time0);
  }
  if ((soln) && (okay))
    return 1;
  else if (!soln)
    return 2;
  else return 0;
}    

/*****************************************************************************/
/* Find cover.                                                               */
/*****************************************************************************/

bool find_cover(char command,designADT design)
{
  bool verbose;
  int cov;

  if (!(design->status & FOUNDCOVER)) {
    verbose=design->verbose;
    if (command==FINDCOVER) verbose=TRUE;
    if ((cov=find_cov(design->filename,design->signals,design->regions,
		      design->ninpsig,design->nsignals,verbose,
		      design->burstgc || design->burst2l,
		      design->inponly))==0) {
      verbose=design->verbose;
      if (command==RESOLVE) verbose=TRUE;
      if ((command==RESOLVE) || (design->exception)) {
	if (!resolve(design->filename,design->signals,design->regions,
		     design->ninpsig,design->nsignals,verbose,
		     design->burstgc || design->burst2l,
		     design->exact,design->inponly))
	  return(FALSE);
      } else return(FALSE);
    }
    if (cov == 2) return FALSE;
    /* IS THIS NEEDED? 
    design->comb=check_combinational(design->state_table,design->regions,
				     design->ninpsig,design->nsignals,
				     design->combo);
    if ((design->gatelevel) &&
	(!disjoint_check(design->filename,design->signals,design->state_table,
			 design->regions,design->comb,design->ninpsig,
			 design->nsignals)))
      return(FALSE);
    */
    design->status=design->status | FOUNDCOVER;
  }
  return(TRUE);
}




