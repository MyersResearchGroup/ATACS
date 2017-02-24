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
/* resolve.c                                                                 */
/*****************************************************************************/

#include "resolve.h"
#include "exact.h"
#include "findconf.h"
#include "storeprs.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   context rule and remove solved problems from the context rule table.    */
/*****************************************************************************/
  
bool resolve_conflict(FILE *fp,signalADT *signals,regionADT *regions,
		      contextADT *context_table,int i,char * cover,
		      int nsignals,bool verbose)
{
  int j,signal,best,best2,cost,cost2;
  contextADT new_entry=NULL;
  contextADT new_entry2=NULL;
  char sol;

  best=INFIN;
  best2=0;
  signal=(-1);
  sol='X';
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (j=0;j<nsignals;j++)
      if ((new_entry->solutions[j]=='0') ||
	  (new_entry->solutions[j]=='1')) {
	cost=share_cost(regions,j,i,new_entry->solutions[j],cover,nsignals);
	if (cost < best) {
	  signal=j;
	  best=cost;
	  sol=new_entry->solutions[j];
	  cost2=0;
	  new_entry2=(*context_table);
	  while (new_entry2 != NULL) {
	    if ((new_entry2->solutions[j]=='0') ||
		(new_entry2->solutions[j]=='1'))
	      cost2++;
	    new_entry2=new_entry2->next;
	  }
	  best2=cost2;
	} else if (cost==best) {
	  cost2=0;
	  new_entry2=(*context_table);
	  while (new_entry2 != NULL) {
	    if ((new_entry2->solutions[j]=='0') ||
		(new_entry2->solutions[j]=='1'))
	      cost2++;
	    new_entry2=new_entry2->next;
	  }
	  if (cost2 > best2) {
	    signal=j;
	    best2=cost2;
	    sol=new_entry->solutions[j];
	  }
	}
      }
    new_entry=new_entry->next;
  }
  cover[signal]=sol;
  if (verbose) {
    if (cover[signal]=='1')
      fprintf(fp,"Added %s to cover for %s",signals[signal]->name,
	      signals[(i-1)/2]->name);
    else
      fprintf(fp,"Added ~%s to cover for %s",signals[signal]->name,
	      signals[(i-1)/2]->name);
    if ((i % 2)==1) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
  }
  rm_solved_problems(context_table,signal,sol);
  return(TRUE);

  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (signal=0;signal<nsignals;signal++)
      if ((new_entry->solutions[signal]=='0') ||
	  (new_entry->solutions[signal]=='1')) {
	cover[signal]=new_entry->solutions[signal];
	if (verbose) {
	  if (cover[signal]=='1')
	    fprintf(fp,"Added %s to cover for %s",signals[signal]->name,
		    signals[(i-1)/2]->name);
	  else
	    fprintf(fp,"Added ~%s to cover for %s",signals[signal]->name,
		    signals[(i-1)/2]->name);
	  if ((i % 2)==1) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
	}
	rm_solved_problems(context_table,signal,new_entry->solutions[signal]);
	return(TRUE);
      } 
    new_entry=new_entry->next;
  }
 return(TRUE);
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   context rule and remove solved problems from the context rule table.    */
/*****************************************************************************/
  
int choose_signal(FILE *fp,signalADT *signals,regionADT *regions,
		  contextADT *context_table,int i,char * cover,
		  int nsignals,bool verbose)
{
  int j,signal,best,best2,cost,cost2;
  contextADT new_entry=NULL;
  contextADT new_entry2=NULL;
  char sol;

  best=INFIN;
  best2=0;
  signal=(-1);
  sol='X';
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (j=0;j<nsignals;j++)
      if ((new_entry->solutions[j]=='0') ||
	  (new_entry->solutions[j]=='1')) {
	cost=share_cost(regions,j,i,new_entry->solutions[j],cover,nsignals);
	if (cost < best) {
	  signal=j;
	  best=cost;
	  sol=new_entry->solutions[j];
	  cost2=0;
	  new_entry2=(*context_table);
	  while (new_entry2 != NULL) {
	    if ((new_entry2->solutions[j]=='0') ||
		(new_entry2->solutions[j]=='1'))
	      cost2++;
	    new_entry2=new_entry2->next;
	  }
	  best2=cost2;
	} else if (cost==best) {
	  cost2=0;
	  new_entry2=(*context_table);
	  while (new_entry2 != NULL) {
	    if ((new_entry2->solutions[j]=='0') ||
		(new_entry2->solutions[j]=='1'))
	      cost2++;
	    new_entry2=new_entry2->next;
	  }
	  if (cost2 > best2) {
	    signal=j;
	    best2=cost2;
	    sol=new_entry->solutions[j];
	  }
	}
      }
    new_entry=new_entry->next;
  }
  return signal;
}

int bound_crt(regionADT *regions,int i,int l,char * cover,int nsignals) 
{
  regionADT cur_region;
  int size;

  size=0;
  for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link)
    if (problem(cur_region->enstate,cover,nsignals))
      size++;
  return size;
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   cube and remove solved problems from the table.                         */
/*****************************************************************************/

int choose_next_crt(contextADT *context_table,char * cover,int nsignals)
{
  int i,signal,size;
  contextADT new_entry=NULL;

  size=INFIN;
  signal=nsignals;
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (i=0;i<nsignals;i++)
      if (new_entry->solutions[i]=='1') {
	cover[i]='1';
	rm_solved_problems(context_table,i,'1');
	return(i);
      } else if (new_entry->solutions[i]=='0') {
	cover[i]='0';
	rm_solved_problems(context_table,i,'0');
	return(i);
      } 
    new_entry=new_entry->next;
  }
  return(INFIN);
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   cube and remove solved problems from the table.                         */
/*****************************************************************************/

int rm_next_crt(contextADT *context_table,int nsignals)
{
  int i,signal,size;
  contextADT new_entry=NULL;
  bool dummy;

  size=INFIN;
  signal=nsignals;
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (i=0;i<nsignals;i++)
      if (new_entry->solutions[i]=='1') {
	rm_solution(context_table,i,&dummy);
	return(i);
      } else if (new_entry->solutions[i]=='0') {
	rm_solution(context_table,i,&dummy);
	return(i);
      }
    new_entry=new_entry->next;
  }
  return(INFIN);
}

int resolve_exact(signalADT *signals,regionADT *regions,regionADT cur_region,
		  char * cover,int ninpsig,int nsignals,
		  contextADT context_table,int *nprob,int *best,bool verbose,
		  bool burst,int i,int l,FILE *fp,int nsigs,
		  coverlistADT covers)
{
  int j,size1,size2,size1b,size2b,nprob1,nprob2,oldnprob,choice;
  bool reduced;
  char * cover1;
  char * cover2;
  contextADT context_table1,context_table2;
  regionADT cur_region2;
  coverlistADT curcover;

  reduced=TRUE;
  /*
  if (bound_crt(regions,i,l,cover,nsignals) < (*best)) {
    //printf("Bounded\n");
    return (*best);
  }
  */
  if ((*nprob)==0) return (*best);
  cover1=CopyString(cover);
  context_table1=CopyContextTable(context_table);
  oldnprob=(*nprob);
  nprob1=(*nprob);
  /*
  choice=choose_signal(fp,signals,regions,&(context_table),
		       2*i+l,cover,nsignals,FALSE);
  if (choice==(-1)) choice=INFIN;
  */
  //if (choice!=INFIN) printf("Choice = %s\n",signals[choice]->name);
  if (nprob1 > 0)
    if ((choice=choose_next_crt(&(context_table1),cover1,nsigs))!=INFIN) {
      //printf("Chose %s\n",signals[choice]->name);
      nprob1=numprob(context_table1,nsignals);
      while (((nprob1=numprob(context_table1,nsignals)) > 0) && 
	     ((nprob1 < oldnprob) || (reduced))) {
	oldnprob=nprob1;
	choose_essential_rows(fp,signals,&(context_table1),i,l,cover1,nsigs,
			      FALSE);
	eliminate_dotted_rows(&(context_table1),nsigs);
	rm_dotted_rows(&(context_table1),nsigs);
	rm_dominating_columns(&(context_table1),nsigs);
	reduced=rm_dominated_rows(regions,&(context_table1),
				  2*i+l,nsigs,cur_region->enstate,
				  cover1,burst,signals,nsignals);
      }
    } else {
      size1=0;
      for (cur_region2=regions[2*i+l];cur_region2;
	   cur_region2=cur_region2->link)
	if (problem(cur_region2->enstate,cover1,nsignals))
	  size1++;
      /*
      size1=0;
      for (j=0;j<nsignals;j++)
	if (cover1[j]=='1') size1++;
      */
      return size1;
    }
  if (nprob1 > 0)
    size1=resolve_exact(signals,regions,cur_region,cover1,ninpsig,
			nsignals,context_table1,&nprob1,best,FALSE,burst,
			i,l,fp,nsigs,covers);
  else {
    size1=0;
    for (cur_region2=regions[2*i+l];cur_region2;cur_region2=cur_region2->link)
      if (problem(cur_region2->enstate,cover1,nsignals))
	size1++;
    if (size1 > (*best))
      (*best)=size1;
    /*
    for (j=0;j<nsignals;j++)
      if (cover1[j]=='1') size1++;
    */
  }
  cover2=CopyString(cover);
  context_table2=CopyContextTable(context_table);
  oldnprob=(*nprob);
  nprob2=(*nprob);
  if (nprob2 > 0)
    if ((choice=rm_next_crt(&(context_table2),nsigs))!=INFIN) {
      //printf("Removed %s\n",signals[choice]->name);
      nprob2=numprob(context_table2,nsignals);
      while (((nprob2=numprob(context_table2,nsignals)) > 0) && 
	     ((nprob2 < oldnprob) || (reduced))) {
	oldnprob=nprob2;
	choose_essential_rows(fp,signals,&(context_table2),i,l,cover2,nsigs,
			      FALSE);
	eliminate_dotted_rows(&(context_table2),nsigs);
	rm_dotted_rows(&(context_table2),nsigs);
	rm_dominating_columns(&(context_table2),nsigs);
	reduced=rm_dominated_rows(regions,&(context_table2),
				  2*i+l,nsigs,cur_region->enstate,
				  cover2,burst,signals,nsignals);
      }
    } else {
      size2=0;
      for (cur_region2=regions[2*i+l];cur_region2;
	   cur_region2=cur_region2->link)
	if (problem(cur_region2->enstate,cover2,nsignals))
	  size2++;
      /*
	for (j=0;j<nsignals;j++)
	if (cover2[j]=='1') size2++;
      */
      return size2;
    }
  if (nprob2 > 0)
    size2=resolve_exact(signals,regions,cur_region,cover2,ninpsig,
			nsignals,context_table2,&nprob2,best,FALSE,burst,
			i,l,fp,nsigs,covers);
  else {
    size2=0;
    for (cur_region2=regions[2*i+l];cur_region2;cur_region2=cur_region2->link)
      if (problem(cur_region2->enstate,cover2,nsignals))
	size2++;
    if (size2 > (*best))
      (*best)=size2;
    /*
    for (j=0;j<nsignals;j++)
      if (cover2[j]=='1') size2++;
    */
  }
  /*  printf("nprob2=%d cover2=%s\n",nprob2,cover2);*/
  if (nprob1==0) {
    for (curcover=covers;curcover->link;curcover=curcover->link)
      if (strcmp(curcover->cover,cover1)==0)
	break;
    if (curcover->cover==NULL) {
      curcover->cover=CopyString(cover1);
      curcover->link=(coverlistADT)GetBlock(sizeof(*curcover));
      curcover->link->cover=NULL;
      curcover->link->link=NULL;
    }
  }
  if (nprob2==0) {
    for (curcover=covers;curcover->link;curcover=curcover->link)
      if (strcmp(curcover->cover,cover2)==0)
	break;
    if (curcover->cover==NULL) {
      curcover->cover=CopyString(cover2);
      curcover->link=(coverlistADT)GetBlock(sizeof(*curcover));
      curcover->link->cover=NULL;
      curcover->link->link=NULL;
    }
  }
  if ((nprob1 < nprob2) || ((nprob1==0) && (size1 > size2))) {
    strcpy(cover,cover1);
    (*nprob)=nprob1;
    return size1;
  } else if (size1==size2) {
    size1b=0;
    for (j=0;j<nsignals;j++)
      if ((cover1[j]=='0') || (cover1[j]=='1')) size1b++;
    size2b=0;
    for (j=0;j<nsignals;j++)
      if ((cover2[j]=='0') || (cover2[j]=='1')) size2b++;
    if (size1b < size2b) {
      strcpy(cover,cover1);
      (*nprob)=nprob1;
      return size1;
    } else {
      strcpy(cover,cover2);
      (*nprob)=nprob2;
      return size2;
    }
  } else {
    strcpy(cover,cover2);
    (*nprob)=nprob2;
    return size2;
  }
/*  return nprob; */
}

/*****************************************************************************/
/* Find an optimal cover of context signals to solve all conflicts.          */
/*****************************************************************************/

bool resolve(char * filename,signalADT *signals,regionADT *regions,
	     int ninpsig,int nsignals,bool verbose,bool burst,bool exact,
	     bool inponly)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,l,best;
  int nprob,oldnprob,nsigs;
  bool reduced;
  regionADT cur_region;
  timeval t0,t1;
  double time0, time1;

  gettimeofday(&t0, NULL);
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".cov");
    printf("Resolving remaining covers and storing to:  %s\n",
	   outname);
    fprintf(lg,"Resolving remaining covers and storing to:");
    fprintf(lg,"  %s\n",outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Resolving remaining covers ... ");
    fflush(stdout);
    fprintf(lg,"Resolving remaining covers ... ");
    fflush(lg);
  }
  nsigs=nsignals;
  if (inponly) nsigs=ninpsig;
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l]; cur_region;
	   cur_region=cur_region->link) {
	if (exact) {
	  nprob=numprob(cur_region->context_table,nsignals);
	  best=0;
	  //printf("CRT: %s %d\n",signals[i]->name,l);
	  cur_region->covers=(coverlistADT)GetBlock(sizeof
						    (*cur_region->covers));
	  cur_region->covers->cover=NULL;
	  cur_region->covers->link=NULL;
	  resolve_exact(signals,regions,cur_region,cur_region->cover,ninpsig,
			nsignals,cur_region->context_table,&nprob,&best,
			FALSE,burst,i,l,fp,nsigs,cur_region->covers);
	  //nprob=numprob(cur_region->context_table,nsignals);
	  /*for (curcover=cur_region->covers;curcover && curcover->cover;
	       curcover=curcover->link)
	       printf("%s\n",curcover->cover);*/
	} else {
	  while ((nprob=numprob(cur_region->context_table,nsignals)) > 0) {
	    if (!resolve_conflict(fp,signals,regions,
				  &(cur_region->context_table),
				  2*i+l,cur_region->cover,nsignals,FALSE)) {
	      if (FALSE) fclose(fp);
	      return(FALSE);
	    }
	    oldnprob=INFIN;
	    reduced=TRUE;
	    while (((nprob=numprob(cur_region->context_table,nsignals)) > 0) &&
		   ((nprob < oldnprob) || (reduced))) {
	      oldnprob=nprob;
	      choose_essential_rows(fp,signals,&(cur_region->context_table),
				    i,l,cur_region->cover,nsigs,
				    FALSE);
	      eliminate_dotted_rows(&(cur_region->context_table),nsigs);
	      rm_dotted_rows(&(cur_region->context_table),nsigs);
	      rm_dominating_columns(&(cur_region->context_table),nsigs);
	      reduced=rm_dominated_rows(regions,&(cur_region->context_table),
					2*i+l,nsigs,cur_region->enstate,
					cur_region->cover,burst,signals,
					nsignals);
	    }
	  }
	}
      }
  if (verbose) {
    for (i=2*ninpsig;i<(2*nsignals+1);i++) 
      print_covers(fp,filename,signals,regions,exact,NULL,FALSE,FALSE,i,
		   nsignals,INFIN,NULL,NULL,NULL,NULL,NULL,NULL);
    fclose(fp);
  } else {
    gettimeofday(&t1,NULL);	
    time0 = (t0.tv_sec+(t0.tv_usec*.000001));
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    printf("done (%f)\n",time1-time0);
    fprintf(lg,"done (%f)\n",time1-time0);
  }
  return(TRUE);
}
