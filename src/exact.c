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
/* exact.c                                                                   */
/*****************************************************************************/

#include "exact.h"
#include "def.h"
#include "misclib.h"
#include "findcover.h"
#include "storeprs.h"
#include "findreg.h"
#include "findconf.h"

/*****************************************************************************/
/* Print uncovered states.                                                   */
/*****************************************************************************/

void print_covering_table(FILE *fp,regionADT region)
{
  contextADT new_entry=NULL;
  int k;

  if (region->context_table != NULL) {
    for (new_entry=region->context_table;
	 new_entry != NULL;
	 new_entry=new_entry->next) {
      fprintf(fp,"%s: ",new_entry->state);
      for (k=0;k<region->nvprimes;k++)
	if (new_entry->solutions[k]=='1')
	  fprintf(fp," %s",region->primes[k]);
        else if (new_entry->solutions[k]==('1'+2))
          fprintf(fp," %s(O)",region->primes[k]);
      fprintf(fp,"\n");
    }
  }
}

/*****************************************************************************/
/* Returns TRUE if a given prime includes a state.                           */
/*****************************************************************************/

bool prime_includes(char * prime,char * state,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++) 
    if ((prime[i] != 'X') && 
	(prime[i] != VAL(state[i])))
      return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Remove unnecessary C-elements.                                            */
/*****************************************************************************/

bool *cover_combinational(stateADT *state_table,regionADT *regions,
			  int ninpsig,int nsignals,bool combo)
{
  int i,k,l;
  stateADT curstate;
  bool *comb;
  regionADT cur_region;

  comb=(bool*)GetBlock((2*nsignals+2)*sizeof(bool));
  if (!combo) {
    for (i=0;i<(2*nsignals+1);i++)
      comb[i]=FALSE;
    return comb;
  }
  for (i=0;i<(2*nsignals+1);i++)
    comb[i]=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
      for (k=ninpsig;k<nsignals;k++) {
        if (comb[2*k+1]==TRUE)
          if ((curstate->state[k]=='1') || (curstate->state[k]=='R')
	      || (curstate->state[k]=='U')) {
            comb[2*k+1]=FALSE;
            for (cur_region=regions[2*k+1];cur_region;
                 cur_region=cur_region->link)
	      for (l=0;l<cur_region->nvprimes;l++)
		if ((cur_region->cover[l]=='1') &&
		    (prime_includes(cur_region->primes[l],curstate->state,
				    nsignals)))
		  comb[2*k+1]=TRUE;
          }
        if (comb[2*k+2]==TRUE)
          if ((curstate->state[k]=='0') || (curstate->state[k]=='F')
	      || (curstate->state[k]=='D')) {
            comb[2*k+2]=FALSE;
            for (cur_region=regions[2*k+2];cur_region;
                 cur_region=cur_region->link)
	      for (l=0;l<cur_region->nvprimes;l++)
		if ((cur_region->cover[l]=='1') &&
		    (prime_includes(cur_region->primes[l],curstate->state,
				    nsignals)))
		  comb[2*k+2]=TRUE;
          }
      }
  return(comb);
}

/*****************************************************************************/
/* Print cover for a function.                                               */
/*****************************************************************************/

void print_cover(FILE *fp,signalADT *signals,regionADT region,
		 int nsignals,int signal,int l,bool comb)
{
  int i,j;
  bool first;

  for (i=0;i<region->nvprimes;i++)
    if (region->cover[i]=='1') {
      if (comb)
	if (l==1) fprintf(fp,"[ "); else fprintf(fp,"[~");
      else if (l==1) fprintf(fp,"[+"); else fprintf(fp,"[-");
      fprintf(fp,"%s: (",signals[signal]->name);
      first=TRUE;
      for (j=0;j<nsignals;j++)
	if (region->primes[i][j]=='0') {
	  if (!first) fprintf(fp," & ");
	  else first=FALSE;
	  fprintf(fp,"~%s",signals[j]->name);
	} else if (region->primes[i][j]=='1') {
	  if (!first) fprintf(fp," & ");
	  else first=FALSE;
	  fprintf(fp,"%s",signals[j]->name);
	}
      if (comb) fprintf(fp,")] Combinational\n"); 
      else fprintf(fp,")]\n");
    }
}

/*****************************************************************************/
/* Determine the number of literals needed for a cube.                       */
/*****************************************************************************/

int cubesize(char * cube,int nsignals)
{
  int i;
  int size;
  
  size=0;
  for (i=0;i<nsignals;i++)
    if (cube[i] != 'X') size++;
  return(size);
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   cube and remove solved problems from the table.                         */
/*****************************************************************************/

void select_essential_rows(contextADT *context_table,char * cover,int nprimes)
{
  int signal;
  contextADT new_entry=NULL;

  new_entry=(*context_table);
  while (new_entry != NULL) 
    if ((signal=distinguished(new_entry->solutions,nprimes)) >= 0) {
      cover[signal]='1';
      rm_solved_problems(context_table,signal,new_entry->solutions[signal]);
      new_entry=(*context_table);
    } else new_entry=new_entry->next;
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   cube and remove solved problems from the table.                         */
/*****************************************************************************/

bool choose_smallest(contextADT *context_table,char * *primes,char * cover,
		     int nprimes,int nsignals)
{
  int i,signal,size;
  contextADT new_entry=NULL;

  size=INFIN;
  signal=nprimes;
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (i=0;i<nprimes;i++)
      if ((new_entry->solutions[i]=='1') && 
	  (cubesize(primes[i],nsignals) < size)) {
	signal=i;
	size=cubesize(primes[i],nsignals);
      }
    new_entry=new_entry->next;
  }
  if (signal==nprimes) return(FALSE);
  cover[signal]='1';
  rm_solved_problems(context_table,signal,'1');
  return(TRUE);
}

/*****************************************************************************/
/* Determine if one row dominates another.                                   */
/*****************************************************************************/

int prime_dom_row(char * *primes,int nprimes,contextADT *context_table,
		  int signal1,int signal2,int nsignals)
{
  contextADT problem=NULL;
  int dominate;

  dominate=(-1);
  for (problem=(*context_table);
       problem != NULL;
       problem=problem->next)
     if ((problem->solutions[signal1]=='X') &&
	 ((problem->solutions[signal2]=='0') || 
	  (problem->solutions[signal2]=='1'))) {
       if ((dominate!=1) && (dominate!=3))
	 dominate=2;
       else
	 dominate=3;
     } else if ((problem->solutions[signal2]=='X') &&
		((problem->solutions[signal1]=='0') || 
		 (problem->solutions[signal1]=='1'))) {
       if ((dominate!=2) && (dominate!=3))
	 dominate=1;
       else
	 dominate=3;
     } else if ((problem->solutions[signal1]=='0'+2) &&
		(problem->solutions[signal2]!='0'+2) &&
		(!find_dotted_col(context_table,signal1,signal2,problem,
				  nprimes))) {
       if (dominate==1)
	 dominate=3;
       else if (dominate!=3)
	 dominate=2;
     } else if ((problem->solutions[signal1]!='0'+2) &&
		(problem->solutions[signal2]=='0'+2) &&
		(!find_dotted_col(context_table,signal2,signal1,problem,
				  nprimes))) {
       if (dominate==2)
	 dominate=3;
       else if (dominate!=3)
	 dominate=1;
     } else if ((problem->solutions[signal1]=='1'+2) &&
		(problem->solutions[signal2]!='1'+2) &&
		(!find_dotted_col(context_table,signal1,signal2,problem,
				  nprimes))) {
       if (dominate==1)
	 dominate=3;
       else if (dominate!=3)
	 dominate=2;
     } else if ((problem->solutions[signal1]!='1'+2) &&
		(problem->solutions[signal2]=='1'+2) &&
		(!find_dotted_col(context_table,signal2,signal1,problem,
				  nprimes))) {
       if (dominate==2)
	 dominate=3;
       else if (dominate!=3)
	 dominate=1;
     } 
     
  if (dominate==(-1)) 
    if (cubesize(primes[signal1],nsignals) > 
	cubesize(primes[signal2],nsignals))
      dominate=2;
    else dominate=1;
  return(dominate);
}

/*****************************************************************************/
/* Remove dominated rows.                                                    */
/*****************************************************************************/

bool prime_rm_dominated_rows(regionADT region,contextADT *context_table,
			     int nsignals)
{
  int k,l,dominate;
  bool reduced;

  reduced=FALSE;
  for (k=0;k<region->nvprimes-1;k++)
    for (l=k+1;l<region->nvprimes;l++) {
      dominate=prime_dom_row(region->primes,region->nvprimes,context_table,
			     k,l,nsignals);
      if (dominate==1)
	rm_solution(context_table,l,&reduced);
      else if (dominate==2)
	rm_solution(context_table,k,&reduced);
    }
  return(reduced);
}

/*****************************************************************************/
/* Check if current region is covered by an upcoming guard.                  */
/*****************************************************************************/

bool noprint_region(regionADT region,int nsignals)
{
  char * cover;
  regionADT cur_region;
  bool first;
  int i,j;

  cover=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (cur_region=region->link;cur_region;cur_region=cur_region->link) {
    first=TRUE;
    for (i=0;i<cur_region->nvprimes;i++)
      if (cur_region->cover[i]=='1') {
	if (first) {
	  first=FALSE;
	  strcpy(cover,cur_region->primes[i]);
	} else 
	  for (j=0;j<nsignals;j++)
	    if ((cover[j] != cur_region->primes[i][j]) &&
		(cur_region->primes[i][j] !='X')) cover[j]='X';
      }
/*    printf("enstate = %s cover = %s\n",region->enstate,cover); */
    if (problem(region->enstate,cover,nsignals)) {
      free(cover);
      return(TRUE);
    }
  }
  free(cover);
  return(FALSE);
}

/*****************************************************************************/
/* Determine if a row is distinguished, and if so, add the corresponding     */
/*   cube and remove solved problems from the table.                         */
/*****************************************************************************/

int choose_next(contextADT *context_table,char * cover,int nprimes)
{
  int i,signal,size;
  contextADT new_entry=NULL;

  size=INFIN;
  signal=nprimes;
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (i=0;i<nprimes;i++)
      if (new_entry->solutions[i]=='1') {
	cover[i]='1';
	rm_solved_problems(context_table,i,'1');
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

int rm_next(contextADT *context_table,char * *primes,char * cover,
	    int nprimes,int nsignals)
{
  int i,signal,size;
  contextADT new_entry=NULL;
  bool dummy;

  size=INFIN;
  signal=nprimes;
  new_entry=(*context_table);
  while (new_entry != NULL) {
    for (i=0;i<nprimes;i++)
      if (new_entry->solutions[i]=='1') {
	rm_solution(context_table,i,&dummy);
	return(i);
      }
    new_entry=new_entry->next;
  }
  return(INFIN);
}

contextADT CopyContextTable(contextADT context_table) 
{
  contextADT cur_entry,new_entry,prev_entry,first_entry;

  prev_entry=NULL;
  first_entry=NULL;
  for (cur_entry=context_table;cur_entry;cur_entry=cur_entry->next) {
    new_entry=(contextADT)GetBlock(sizeof *new_entry);
    if (!first_entry) first_entry=new_entry;
    new_entry->state=CopyString(cur_entry->state);
    new_entry->solutions=CopyString(cur_entry->solutions);
    new_entry->prev=prev_entry;
    if (prev_entry) prev_entry->next=new_entry;
    prev_entry=new_entry;
  }
  if (prev_entry) prev_entry->next=NULL;
  return first_entry;
}

int bound(contextADT context_table,char * cover) 
{
  return INFIN;
}
  
int exact_cover(regionADT cur_region,char * cover,char * *primes,
		int nvprimes,contextADT context_table,int *nprob,int nsignals,
		int best)
{
  int i,size1,size2,nprob1,nprob2,oldnprob;
  bool reduced;
  char * cover1;
  char * cover2;
  contextADT context_table1,context_table2;

/*  printf("nprob=%d best=%d\n",*nprob,best); */
  if (bound(context_table,cover) > best) return best;
  if ((*nprob)==0) return best;
  cover1=CopyString(cover);
  context_table1=CopyContextTable(context_table);
  oldnprob=(*nprob);
  nprob1=(*nprob);
  if (nprob1 > 0)
    if ((choose_next(&(context_table1),cover1,nvprimes))!=INFIN) {
      nprob1=numprob(context_table1,nvprimes);
      while (((nprob1=numprob(context_table1,nvprimes)) > 0) && 
	     ((nprob1 < oldnprob) || (reduced))) {
	oldnprob=nprob1;
	select_essential_rows(&(context_table1),cover1,nvprimes);
	eliminate_dotted_rows(&(context_table1),nvprimes);
	rm_dotted_rows(&(context_table1),nvprimes);
	rm_dominating_columns(&(context_table1),nvprimes);
	reduced=prime_rm_dominated_rows(cur_region,&(context_table1),nsignals);
      }
    } else {
      size1=0;
      for (i=0;i<nsignals;i++)
	if (cover1[i]=='1') size1++;
      return size1;
    }
  if (nprob1 > 0)
    size1=exact_cover(cur_region,cover1,primes,nvprimes,context_table1,
		      &nprob1,nsignals,best);
  else {
    size1=0;
    for (i=0;i<nsignals;i++)
      if (cover1[i]=='1') size1++;
  }
  /*  printf("nprob1=%d cover1=%s\n",nprob1,cover1);*/

  cover2=CopyString(cover);
  context_table2=CopyContextTable(context_table);
  oldnprob=(*nprob);
  nprob2=(*nprob);
  if (nprob2 > 0)
    if ((rm_next(&(context_table2),primes,cover2,nvprimes,
		 nsignals))!=INFIN) {
      nprob2=numprob(context_table2,nvprimes);
      while (((nprob2=numprob(context_table2,nvprimes)) > 0) && 
	     ((nprob2 < oldnprob) || (reduced))) {
	oldnprob=nprob2;
	select_essential_rows(&(context_table2),cover2,nvprimes);
	eliminate_dotted_rows(&(context_table2),nvprimes);
	rm_dotted_rows(&(context_table2),nvprimes);
	rm_dominating_columns(&(context_table2),nvprimes);
	reduced=prime_rm_dominated_rows(cur_region,&(context_table2),nsignals);
      }
    } else {
      size2=0;
      for (i=0;i<nsignals;i++)
	if (cover2[i]=='1') size2++;
      return size2;
    }
  if (nprob2 > 0)
    size2=exact_cover(cur_region,cover2,primes,nvprimes,context_table2,
		      &nprob2,nsignals,best);
  else {
    size2=0;
    for (i=0;i<nsignals;i++)
      if (cover2[i]=='1') size2++;
  }
  /*  printf("nprob2=%d cover2=%s\n",nprob2,cover2);*/
  if ((nprob1 < nprob2) || ((nprob1==0) && (size1 < size2))) {
    strcpy(cover,cover1);
    (*nprob)=nprob1;
    return size1;
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

bool find_prime_cov(char * filename,signalADT *signals,stateADT *state_table,
		    regionADT *regions,int ninpsig,int nsignals,bool verbose,
		    bool exception,bool exact,bool combo,bool gatelevel)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  FILE *errfp;
  int i,j,l;
  int nprob,oldnprob;
  bool reduced;
  bool okay;
  bool *comb;
  regionADT cur_region;

  verbose=TRUE;

  strcpy(outname,filename);
  strcat(outname,".prs");
  printf("Finding prime cover and storing to:  %s\n",outname);
  fprintf(lg,"Finding prime cover and storing to:  %s\n",outname);
  fp=Fopen(outname,"w");
  okay=TRUE;
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link) {
	oldnprob=INFIN;
	reduced=TRUE;
	free(cur_region->cover); 
	cur_region->cover=(char *)
	  GetBlock((cur_region->nvprimes+2)*sizeof(char));
	for (j=0;j<cur_region->nvprimes;j++)
	  cur_region->cover[j]='0';
	cur_region->cover[j]='\0';
	while (((nprob=numprob(cur_region->context_table,
			       cur_region->nvprimes)) > 0) && 
	       ((nprob < oldnprob) || (reduced))) {
	  oldnprob=nprob;
	  select_essential_rows(&(cur_region->context_table),
				cur_region->cover,cur_region->nvprimes);
	  eliminate_dotted_rows(&(cur_region->context_table),
				cur_region->nvprimes);
	  rm_dotted_rows(&(cur_region->context_table),cur_region->nvprimes);
	  rm_dominating_columns(&(cur_region->context_table),
				cur_region->nvprimes);
	  reduced=prime_rm_dominated_rows(cur_region,
					  &(cur_region->context_table),
					  nsignals); 
	}
	if ((nprob > 0) && (exception) && (exact))
	  exact_cover(cur_region,cur_region->cover,cur_region->primes,
		      cur_region->nvprimes,cur_region->context_table,
		      &nprob,nsignals,INFIN);
	else 
	  if (nprob > 0) {
	    oldnprob=nprob;
	    if (exception) {
	      while ((nprob > 0) &&
		     (choose_smallest(&(cur_region->context_table),
				      cur_region->primes,cur_region->cover,
				      cur_region->nvprimes,nsignals))) {
		nprob=numprob(cur_region->context_table,cur_region->nvprimes);
		while (((nprob=numprob(cur_region->context_table,
				       cur_region->nvprimes)) > 0) && 
		       ((nprob < oldnprob) || (reduced))) {
		  oldnprob=nprob;
		  select_essential_rows(&(cur_region->context_table),
					cur_region->cover,
					cur_region->nvprimes);
		  eliminate_dotted_rows(&(cur_region->context_table),
					cur_region->nvprimes);
		  rm_dotted_rows(&(cur_region->context_table),
				 cur_region->nvprimes);
		  rm_dominating_columns(&(cur_region->context_table),
					cur_region->nvprimes);
		  reduced=prime_rm_dominated_rows(cur_region,
						  &(cur_region->context_table),
						  nsignals); 
		}
	      }
	    }
	  }
	if (nprob > 0) {
	  if (okay) {
	    if (!verbose) {
	      printf("EXCEPTION!\n");
	      fprintf(lg,"EXCEPTION!\n");
	      printf("EXCEPTION:  Uncovered states!\n");
	      fprintf(lg,"EXCEPTION:  Uncovered states!\n");
	      return(FALSE);
	    }
	    strcpy(outname,filename);
	    strcat(outname,".err");
	    printf("EXCEPTION:  Storing uncovered states to:  %s\n",outname);
	    fprintf(lg,"EXCEPTION:  Storing uncovered states to:  %s\n",
		    outname);
	    errfp=Fopen(outname,"w");
	    fprintf(errfp,"UNCOVERED STATES:\n");
	    print_state_vector(errfp,signals,nsignals,ninpsig);
	    okay=FALSE;
	  }
	  if (cur_region->context_table != NULL) {
	    fprintf(errfp,"Uncovered States for %s",signals[i]->name);
	    if (l==1) fprintf(errfp,"+\n"); else fprintf(errfp,"-\n");
	    print_covering_table(errfp,cur_region);
	  }
	}
      }
  comb=cover_combinational(state_table,regions,ninpsig,nsignals,combo);
  if (gatelevel)
    for (i=ninpsig;i<nsignals;i++) { 
      if (comb[2*i+1]) { 
	for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
	  if (!noprint_region(cur_region,nsignals))
	    print_cover(fp,signals,cur_region,nsignals,i,1,comb[2*i+1]);
      } else if (comb[2*i+2]) {
	for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link)
	  if (!noprint_region(cur_region,nsignals))
	    print_cover(fp,signals,cur_region,nsignals,i,2,comb[2*i+2]);
      } else {
	for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link) 
	  if (!noprint_region(cur_region,nsignals))
	    print_cover(fp,signals,cur_region,nsignals,i,1,FALSE);
	for (cur_region=regions[2*i+2];cur_region;
	     cur_region=cur_region->link)
	  if (!noprint_region(cur_region,nsignals))
	    print_cover(fp,signals,cur_region,nsignals,i,2,FALSE);
      } 
    } 
  else
    for (i=ninpsig;i<nsignals;i++) { 
      if (comb[2*i+1]) { 
	print_cover(fp,signals,regions[2*i+1],nsignals,i,1,comb[2*i+1]);
    } else if (comb[2*i+2]) {
      print_cover(fp,signals,regions[2*i+2],nsignals,i,2,comb[2*i+2]);
    } else {
      print_cover(fp,signals,regions[2*i+1],nsignals,i,1,FALSE);
      print_cover(fp,signals,regions[2*i+2],nsignals,i,2,FALSE);
    } 
  }
  free(comb);
  if (!okay) fclose(errfp);
  fclose(fp);
  return(okay);
}    

/*****************************************************************************/
/* Add entry to prime implicant table.                                       */
/*****************************************************************************/

contextADT add_entry_to_pit(regionADT region,char * state1,char * state2,
			    int nsignals) 
{
  int j;
  contextADT new_entry=NULL;

  new_entry=(contextADT)GetBlock(sizeof *new_entry);
  new_entry->next=region->context_table;
  new_entry->prev=NULL;
  if (region->context_table != NULL)
    region->context_table->prev=new_entry;
  region->context_table=new_entry;
  new_entry->state=CopyString(state1);
  new_entry->solutions=(char *)GetBlock((region->nvprimes+1) * sizeof(char));
  for (j=0;j<region->nvprimes;j++)
    if (prime_includes(region->primes[j],state2,nsignals))
      new_entry->solutions[j]='1';
    else 
      new_entry->solutions[j]='X';
  new_entry->solutions[j]='\0';
  return(new_entry);
}

/*****************************************************************************/
/* Build prime implicant table.                                              */
/*****************************************************************************/

void build_pi_tables(char * filename,signalADT *signals,stateADT *state_table,
		     regionADT *regions,int ninpsig,int nsignals,
		     bool gatelevel,bool verbose,bool atomic,bool sharegates)
{
  int i,j,k,l;
  FILE *fp;
  char outname[FILENAMESIZE];
  stateADT curstate;
  statelistADT predstate;
  contextADT new_entry=NULL;
  regionADT cur_region;
  regionADT cur_region2;

  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".pit");
    printf("Finding primes implicant tables and storing to:  %s\n",outname);
    fprintf(lg,"Finding primes implicant tables and storing to:  %s\n",
	    outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Finding prime implicant tables ... ");
    fflush(stdout);
    fprintf(lg,"Finding prime implicant tables ... ");
    fflush(lg);
  }
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      for (k=ninpsig;k<nsignals;k++) {
	if ((curstate->state[k]=='R') || (curstate->state[k]=='U') ||
	    ((atomic) && (curstate->state[k]=='1'))) {
	  for (cur_region=regions[2*k+1];cur_region;
	       cur_region=cur_region->link)
	    if ((problem(curstate->state,cur_region->enstate,nsignals)) ||
		(!gatelevel))
	      add_entry_to_pit(cur_region,curstate->state,curstate->state,
			       nsignals); 
	} else if ((curstate->state[k]=='F') ||
		   (curstate->state[k]=='D') ||
		   ((atomic) && (curstate->state[k]=='0'))) {
	  for (cur_region=regions[2*k+2];cur_region;
	       cur_region=cur_region->link)
	    if ((problem(curstate->state,cur_region->enstate,nsignals)) ||
		(!gatelevel))
	      add_entry_to_pit(cur_region,curstate->state,curstate->state,
			       nsignals);
	}
	if (gatelevel) {
          if (curstate->state[k]=='1') {
	    for (cur_region=regions[2*k+1];cur_region;
		 cur_region=cur_region->link)
	      for (j=0;j<cur_region->nvprimes;j++)
		if (prime_includes(cur_region->primes[j],curstate->state,
				   nsignals))
		  for (predstate=curstate->pred;predstate;
		       predstate=predstate->next)
		    if (((predstate->stateptr->state[k]!='R') ||
			 (predstate->stateptr->state[k]!='U') ||
			 (!problem(predstate->stateptr->state,
				   cur_region->enstate,nsignals))) &&
			(!prime_includes(cur_region->primes[j],
					 predstate->stateptr->state,
					 nsignals))) {
		    new_entry=add_entry_to_pit(cur_region,curstate->state,
					       predstate->stateptr->state,
					       nsignals); 
		    new_entry->solutions[j]='1'+2;
		  }
	  } else if (curstate->state[k]=='0') {
	    for (cur_region=regions[2*k+2];cur_region;
		 cur_region=cur_region->link)
	      for (j=0;j<cur_region->nvprimes;j++)
		if (prime_includes(cur_region->primes[j],
				   curstate->state,nsignals))
		  for (predstate=curstate->pred;predstate;
		       predstate=predstate->next)
		    if (((predstate->stateptr->state[k]!='F') ||
			 (predstate->stateptr->state[k]!='D') ||
			 (!problem(predstate->stateptr->state,
				   cur_region->enstate,nsignals))) &&
			(!prime_includes(cur_region->primes[j],
					 predstate->stateptr->state,
					 nsignals))) {
		      new_entry=add_entry_to_pit(cur_region,curstate->state,
						 predstate->stateptr->state,
						 nsignals); 
		      new_entry->solutions[j]='1'+2;
		    }

	  } else if ((curstate->state[k]=='R') || (curstate->state[k]=='U')) {
	    for (cur_region=regions[2*k+1];cur_region;
		 cur_region=cur_region->link)
	      if (!problem(curstate->state,cur_region->enstate,nsignals))
		if (sharegates) { 
		  for (cur_region2=regions[2*k+1];cur_region2;
		       cur_region2=cur_region2->link)
		    for (j=0;j<cur_region->nvprimes;j++)
		      if ((problem(curstate->state,cur_region2->enstate,
				   nsignals)) &&
			  (problem(cur_region2->enstate,cur_region->primes[j],
				   nsignals)) &&
			  (!problem(curstate->state,cur_region->primes[j],
				    nsignals))) {
			new_entry=add_entry_to_pit(cur_region,curstate->state,
						   curstate->state,nsignals); 
			new_entry->solutions[j]='1'+2;
		      }
		} 
	  } else if ((curstate->state[k]=='F') || (curstate->state[k]=='D')) {
	    for (cur_region=regions[2*k+2];cur_region;
		 cur_region=cur_region->link)
	      if (!problem(curstate->state,cur_region->enstate,nsignals))
		if (sharegates) { 
		  for (cur_region2=regions[2*k+2];cur_region2;
		       cur_region2=cur_region2->link)
		    for (j=0;j<cur_region->nvprimes;j++)
		      if ((problem(curstate->state,cur_region2->enstate,
				   nsignals)) &&
			  (problem(cur_region2->enstate,cur_region->primes[j],
				   nsignals)) &&
			  (!problem(curstate->state,cur_region->primes[j],
				    nsignals))) {
			new_entry=add_entry_to_pit(cur_region,curstate->state,
						   curstate->state,nsignals); 
			new_entry->solutions[j]='1'+2;
		      }
		} 
	  }
	  
	}
      }
  if (verbose) {
    for (i=ninpsig;i<nsignals;i++)
      for (l=1;l<3;l++)
	for (cur_region=regions[2*i+l];cur_region;
	     cur_region=cur_region->link) {
	  fprintf(fp,"Prime implicant table for %s",signals[i]->name);
	  if (l==1) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
	  print_covering_table(fp,cur_region);
	}
    fclose(fp);
  } else {
    printf("done\n");
    fprintf(lg,"done\n");
  }
}

/*****************************************************************************/
/* Get a token from a espresso file.                                         */
/*****************************************************************************/

int fgettokesp(FILE *fp,char *tokvalue, int maxtok) 

{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '\n':
      if (!readword) {
	return(END_OF_LINE);
      } else {
	ungetc('\n',fp);
	return (WORD);
      }
      break;
    case '#':
      if (!readword) {
	while (((c=getc(fp)) != EOF) && (c != '\n'));
	return(COMMENT);
      } else {
	ungetc('#',fp);
	return (WORD);
      }
      break;
    case '.':
      if (!readword) {
	if ((c=getc(fp)) != EOF) {
	  *tokvalue++=c;
	  *tokvalue='\0';
	  return(DOT);
	} else return(END_OF_FILE);
      } else {
	ungetc('.',fp);
	return (WORD);
      }
      break;
    case ' ':
      if (readword)
	return (WORD);      
      break;
    default:
      if (toklen < maxtok) {
	readword=TRUE;
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
      }
      break;
    }
  }
  if (!readword) 
    return(END_OF_FILE);
  else
    return(WORD);
}

/*****************************************************************************/
/* Load espresso header info.                                                */
/*****************************************************************************/

bool load_espresso_header(FILE *fp,int *ninputs,int *noutputs,int *nprods,
			  int *nstates)
{
  int value,token;
  char dot[MAXTOKEN];
  char dotvalue[MAXTOKEN];

  while ((token=fgettokesp(fp,dot,MAXTOKEN)) != WORD) {
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      return FALSE;
    }
    if (token==DOT) {
      token=fgettokesp(fp,dotvalue,MAXTOKEN);
      if (token==END_OF_FILE) {
	printf("ERROR:  Unexpected end of file!\n");
	fprintf(lg,"ERROR:  Unexpected end of file!\n");
	return FALSE;
      }
      value=atoi(dotvalue);
      if (token==WORD)
	switch (dot[0]) {
	case 'i':
	  *ninputs=value;
	  break;
	case 'o':
	  *noutputs=value;
	  break;
	case 'p':
	  *nprods=value;
	  break;
	case 's':
	  *nstates=value;
	  break;
	default:
	  printf("ERROR:  Bad header information!\n");
	  fprintf(lg,"ERROR:  Bad header information!\n");
	  return FALSE;
	}
      if ((*ninputs != 0) && (*noutputs != 0)  && (*nprods != 0) && 
	  (*nstates != 0)) return TRUE; 
    }
  }
  if (*ninputs==0) {
    printf("ERROR:  No inputs in system!\n");
    fprintf(lg,"ERROR:  No inputs in system!\n");
    return FALSE;
  }
  if (*noutputs==0) {
    printf("ERROR:  No outputs in system!\n");
    fprintf(lg,"ERROR:  No outputs in system!\n");
    return FALSE;
  }
  if (*nprods==0) {
    printf("ERROR:  No products in system!\n");
    fprintf(lg,"ERROR:  No products in system!\n");
    return FALSE;
  }
  if (*nstates==0) {
    printf("ERROR:  No states in system!\n");
    fprintf(lg,"ERROR:  No states in system!\n");
    return FALSE;
  }
  return TRUE;
}

/*****************************************************************************/
/* Get an espresso field.                                                    */
/*****************************************************************************/

bool get_esp_field(FILE *fp,char * tokvalue,int size)
{
  int token;

  while ((token=fgettokesp(fp,tokvalue,MAXTOKEN)) != WORD) {
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      return(FALSE);
    }
    if (token==END_OF_LINE) {
      printf("ERROR:  Unexpected end of line!\n");
      fprintf(lg,"ERROR:  Unexpected end of line!\n");
      return(FALSE);
    }
  }
  if (strlen(tokvalue) != (unsigned)size) {
    printf("ERROR:  Improper token size %s %d!\n",tokvalue,size);
    fprintf(lg,"ERROR:  Improper token size!\n");
    return(FALSE);
  }
  return(TRUE);
}

/*****************************************************************************/
/* Find implied states.                                                      */
/*****************************************************************************/

int implied_states(stateADT *state_table,regionADT *regions,regionADT region,
		   char * prime,int signal,int nsignals,bool rise)
{
  int i,count;
  stateADT curstate;
  statelistADT predstate;
  regionADT cur_region;
  bool nonred;

  count=0;
  nonred=FALSE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      if (prime_includes(prime,curstate->state,nsignals)) nonred=TRUE;
      if ((rise) && (curstate->state[signal]=='1')) {
	if (prime_includes(prime,curstate->state,nsignals))
	  for (predstate=curstate->pred;predstate;
	       predstate=predstate->next)
	    if (((predstate->stateptr->state[signal]!='R') ||
		 (predstate->stateptr->state[signal]!='U') ||
		 (!problem(predstate->stateptr->state,region->enstate,
			   nsignals))) &&
		(!prime_includes(prime,predstate->stateptr->state,
				 nsignals)))
	      count++;
      } 

      if ((rise) && ((curstate->state[signal]=='R') ||
		     (curstate->state[signal]=='U')) &&
	  (!problem(curstate->state,region->enstate,nsignals)))
	for (cur_region=regions[2*signal+1];cur_region;
	     cur_region=cur_region->link)
	  if ((problem(curstate->state,cur_region->enstate,nsignals)) &&
	      (problem(cur_region->enstate,prime,nsignals)) &&
	      (!problem(curstate->state,prime,nsignals)))
	      count++;

      if ((!rise) && (curstate->state[signal]=='0')) {
	if (prime_includes(prime,curstate->state,nsignals))
	  for (predstate=curstate->pred;predstate;
	       predstate=predstate->next)
	    if (((predstate->stateptr->state[signal]!='F') ||
		 (predstate->stateptr->state[signal]!='D') ||
		 (!problem(predstate->stateptr->state,region->enstate,
			   nsignals))) &&
		(!prime_includes(prime,predstate->stateptr->state,
				 nsignals)))
	      count++;
      } 

      if ((!rise) && ((curstate->state[signal]=='F') ||
		      (curstate->state[signal]=='D')) &&
	  (!problem(curstate->state,region->enstate,nsignals)))
	for (cur_region=regions[2*signal+2];cur_region;
	     cur_region=cur_region->link)
	  if ((problem(curstate->state,cur_region->enstate,nsignals)) &&
	      (problem(cur_region->enstate,prime,nsignals)) &&
	      (!problem(curstate->state,prime,nsignals)))
	      count++;
    }
  /*  printf("prime %s has %d impliedstate\n",prime,count); */
  if (nonred) 
    return(count);
  else return(NaN);
}

/*****************************************************************************/
/* Find implied states.                                                      */
/*****************************************************************************/

int implied_subseteq(stateADT *state_table,regionADT *regions,regionADT region,
		     char * prime,int signal,int nsignals,bool rise, 
		     char *newprime)
{
  int i,count;
  stateADT curstate;
  statelistADT predstate;
  regionADT cur_region;
  bool nonred;

  count=0;
  nonred=FALSE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      if (prime_includes(prime,curstate->state,nsignals)) nonred=TRUE;
      if ((rise) && (curstate->state[signal]=='1')) {
	if (prime_includes(prime,curstate->state,nsignals))
	  for (predstate=curstate->pred;predstate;
	       predstate=predstate->next)
	    if (((predstate->stateptr->state[signal]!='R') ||
		 (predstate->stateptr->state[signal]!='U') ||
		 (!problem(predstate->stateptr->state,region->enstate,
			   nsignals))) &&
		(!prime_includes(prime,predstate->stateptr->state,
				 nsignals))) {
	      if (!prime_includes(newprime,curstate->state,nsignals))
		return false;
	    }
	//	      count++;
      } 

      if ((rise) && ((curstate->state[signal]=='R') ||
		     (curstate->state[signal]=='U')) &&
	  (!problem(curstate->state,region->enstate,nsignals)))
	for (cur_region=regions[2*signal+1];cur_region;
	     cur_region=cur_region->link)
	  if ((problem(curstate->state,cur_region->enstate,nsignals)) &&
	      (problem(cur_region->enstate,prime,nsignals)) &&
	      (!problem(curstate->state,prime,nsignals))) {
	      count++;
	  }

      if ((!rise) && (curstate->state[signal]=='0')) {
	if (prime_includes(prime,curstate->state,nsignals))
	  for (predstate=curstate->pred;predstate;
	       predstate=predstate->next)
	    if (((predstate->stateptr->state[signal]!='F') ||
		 (predstate->stateptr->state[signal]!='D') ||
		 (!problem(predstate->stateptr->state,region->enstate,
			   nsignals))) &&
		(!prime_includes(prime,predstate->stateptr->state,
				 nsignals))) {
	      if (!prime_includes(newprime,curstate->state,nsignals))
		return false;
	      // count++;
	    }
      } 

      if ((!rise) && ((curstate->state[signal]=='F') ||
		      (curstate->state[signal]=='D')) &&
	  (!problem(curstate->state,region->enstate,nsignals)))
	for (cur_region=regions[2*signal+2];cur_region;
	     cur_region=cur_region->link)
	  if ((problem(curstate->state,cur_region->enstate,nsignals)) &&
	      (problem(cur_region->enstate,prime,nsignals)) &&
	      (!problem(curstate->state,prime,nsignals)))
	      count++;
    }
  /*  printf("prime %s has %d impliedstate\n",prime,count); */
  return true;
}

/*****************************************************************************/
/* Check if prime is a candidate prime.  In other words, if for all other    */
/*   primes that it is a subset, it has less implied states.                 */
/*****************************************************************************/

bool candidate_prime(stateADT *state_table,regionADT *regions,int signal,
		     int nsignals,bool rise,regionADT region,char * newprime,
		     int is)
{
  int i,j;
  bool subset;

  /*  printf("candidate prime\n"); */
  /* printf("Checking %s\n",newprime);*/
  //  if (is==NaN) return FALSE;
  for (i=0;i<region->nvprimes;i++) {
    subset=TRUE;
    for (j=0;j<nsignals;j++)
      if ((region->primes[i][j] != 'X') && 
	  (newprime[j] != region->primes[i][j]))
	subset=FALSE;
    if (region->implied[i]==(-1))
      region->implied[i]=implied_states(state_table,regions,region,
				region->primes[i],signal,nsignals,rise);
    if ((subset) && 
	(implied_subseteq(state_table,regions,region,region->primes[i],signal,
			  nsignals,rise,newprime))) {
      //(region->implied[i] <= is)) {
      /*printf("Not candidate due to %s\n",region->primes[i]);*/
      return(FALSE);
    }
  }
  return(TRUE);
}

/*****************************************************************************/
/* Add a prime to prime list.                                                */
/*****************************************************************************/

int add_prime(char * *primes,char * newprime,int nprimes,int nsignals,
	      int *implied,int is)
{
  int i;
  
  i=nprimes-1;
  while ((i >= 0) && (cubesize(newprime,nsignals) < 
		      cubesize(primes[i],nsignals))) {
    strcpy(primes[i+1],primes[i]);
    implied[i+1]=implied[i];
    i--;
  }
  strcpy(primes[i+1],newprime);
  implied[i+1]=is;
  nprimes++;
  
  return(nprimes);
}

/*****************************************************************************/
/* Find candidate primes.                                                    */
/*****************************************************************************/

void find_candidate_primes(FILE *fp,signalADT *signals,stateADT *state_table,
			   regionADT *regions,regionADT region,int nsignals,
			   int sig,int l,bool gatelevel,bool verbose,
			   int *nprimes)
{
  int i,j,k,is;
  char newprime[MAXSIGNALS];
  bool rise;

  if (l==1) rise=TRUE; else rise=FALSE;
  if (verbose) {
    fprintf(fp,"Primes for %s",signals[sig]->name);
    if (rise) fprintf(fp,"+\n"); else fprintf(fp,"-\n");
  }
  /*  printf("initially: nvprimes = %d nprimes = %d\n",region->nvprimes,
	 4*nsignals*nsignals*nsignals); */
  for (i=0;i<region->nvprimes;i++) {
    if (verbose) fprintf(fp,"%s\n",region->primes[i]);
    if (gatelevel) {
      if (region->implied[i]==(-1))
	region->implied[i]=implied_states(state_table,regions,region,
					  region->primes[i],sig,nsignals,rise);
      if (region->implied[i] > 0)
	for (j=0;j<nsignals;j++)
	  if (region->primes[i][j]=='X') {
	    strcpy(newprime,region->primes[i]);
	    newprime[j]='0';
	    is=implied_states(state_table,regions,region,newprime,sig,
			      nsignals,rise);
	    if (candidate_prime(state_table,regions,sig,nsignals,rise,region,
				newprime,is)) {
	      if (region->nvprimes==(*nprimes)) {
		(*nprimes)=2*(*nprimes);
		region->primes=(char **)realloc(region->primes,
						(*nprimes) * sizeof(char **));
		region->implied=(int*)realloc(region->implied,
					      (*nprimes) * sizeof(int));
		for (k=region->nvprimes;k<(*nprimes);k++)
		  region->implied[k]=(-1);
		for (k=region->nvprimes;k<(*nprimes);k++)
		  region->primes[k]=(char *)GetBlock((nsignals+1)*
						     sizeof(char));
	      }
	      region->nvprimes=add_prime(region->primes,newprime,
					 region->nvprimes,nsignals,
					 region->implied,is);
	    }
	    newprime[j]='1';
	    is=implied_states(state_table,regions,region,newprime,sig,
			      nsignals,rise);
	    if (candidate_prime(state_table,regions,sig,nsignals,rise,region,
				newprime,is)) {
	      if (region->nvprimes==(*nprimes)) {
		(*nprimes)=2*(*nprimes);
		region->primes=(char **)realloc(region->primes,
						(*nprimes) * sizeof(char **));
		region->implied=(int*)realloc(region->implied,
					      (*nprimes) * sizeof(int));
		for (k=region->nvprimes;k<(*nprimes);k++)
		  region->implied[k]=(-1);
		for (k=region->nvprimes;k<(*nprimes);k++)
		  region->primes[k]=(char *)GetBlock((nsignals+1)*sizeof(char));
	      }		
	    region->nvprimes=add_prime(region->primes,newprime,
				       region->nvprimes,nsignals,
				       region->implied,is);
	    }
	  }
    }
  }
  /*  printf("nvprimes = %d nprimes = %d\n",region->nvprimes,
	 4*nsignals*nsignals*nsignals); */
}

/*****************************************************************************/
/* Read in primes generated by espresso.                                     */
/*****************************************************************************/

bool read_espresso_primes(stateADT *state_table,regionADT region,int nsignals,
			  int *nprimes,int sig,int l,bool atomic)
{
  FILE *fp;
  int i,j,k,m,dummy1,dummy2,dummy3,np;
  int token;
  char tokvalue[MAXTOKEN];
  stateADT curstate;
  bool nonred;
  char newprime[MAXSIGNALS];
  bool rise;

  /* printf("Start\n"); */
  fp=Fopen("output","r");
  dummy1=0;
  dummy2=0;
  dummy3=1;
  np=0;
  region->nvprimes=0;
  if (!load_espresso_header(fp,&dummy1,&dummy2,&np,&dummy3)) 
    return(FALSE);
  region->primes=(char **)GetBlock((*nprimes) * sizeof(char **));
  region->implied=(int*)GetBlock((*nprimes) * sizeof(int));
  for (i=0;i<(*nprimes);i++)
    region->implied[i]=(-1);
  for (i=0;i<(*nprimes);i++)
    region->primes[i]=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (i=0;i<np;i++) {
    token=fgettokesp(fp,tokvalue,MAXTOKEN);
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      fclose(fp);
      for (j=0;j<(*nprimes);j++)
	free(region->primes[j]);
      free(region->primes);
      return(FALSE);
    } 
    if (!get_esp_field(fp,tokvalue,dummy1)) {
      fclose(fp);
      for (j=0;j<(*nprimes);j++)
	free(region->primes[j]);
      free(region->primes);
      return(FALSE);
    }
    for (j=0;j<nsignals;j++)
      if (tokvalue[j]=='0') newprime[j]='0';
      else if (tokvalue[j]=='1') newprime[j]='1';
      else newprime[j]='X';
    newprime[j]='\0';
    if (!get_esp_field(fp,tokvalue,dummy2)) {
      fclose(fp);
      for (j=0;j<(*nprimes);j++)
	free(region->primes[j]);
      free(region->primes);
      return(FALSE);
    }
    /* printf("Prime = %s ",newprime); */
    if (l==1) rise=TRUE; else rise=FALSE;
    nonred=FALSE;
    for (k=0;k<PRIME;k++)
      for (curstate=state_table[k];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link)
	    if ((rise) && ((curstate->state[sig]=='R') || 
			   (curstate->state[sig]=='U') || 
			   (/*(atomic) &&*/ (curstate->state[sig]=='1')))) {
	      if (prime_includes(newprime,curstate->state,nsignals))
		nonred=TRUE;
	    } else if ((!rise) && ((curstate->state[sig]=='F') || 
				   (curstate->state[sig]=='D') || 
				   (/*(atomic) &&*/ (curstate->state[sig]=='0')))){
	      if (prime_includes(newprime,curstate->state,nsignals))
		nonred=TRUE;
	    } 
    nonred=true;
    /*if (nonred) printf("KEEPER\n");
    else printf("\n"); */
    if (nonred) {
      if (region->nvprimes==(*nprimes)) {
	(*nprimes)=2*(*nprimes);
	region->primes=(char **)realloc(region->primes,
					(*nprimes) * sizeof(char **));
	region->implied=(int*)realloc(region->implied,
				      (*nprimes) * sizeof(int));
	for (m=region->nvprimes;m<(*nprimes);m++)
	  region->implied[m]=(-1);
	for (m=region->nvprimes;m<(*nprimes);m++)
	  region->primes[m]=(char *)GetBlock((nsignals+1)*sizeof(char));
      }
      region->nvprimes=add_prime(region->primes,newprime,region->nvprimes,
				 nsignals,region->implied,(-1));
    }
  }
  fclose(fp);
  return(TRUE);
}

/*****************************************************************************/
/* Find all candidate primes.                                                */
/*****************************************************************************/

bool find_primes(FILE *fp,signalADT *signals,stateADT *state_table,
		 regionADT *regions,regionADT region,int nsignals,int *nprimes,
		 int i,int l,bool gatelevel,bool verbose,bool atomic)
  {
  if (!read_espresso_primes(state_table,region,nsignals,nprimes,i,l,atomic))
    return(FALSE);
  find_candidate_primes(fp,signals,state_table,regions,region,nsignals,i,l,
			gatelevel,verbose,nprimes);
  return(TRUE);
}

char ESP_VAL(char bit)
{
  if ((bit=='0')||(bit=='R')) return '0';
  else if ((bit=='1')||(bit=='F')) return '1';
  else return '-';
}

/*****************************************************************************/
/* Setup an espresso run to find primes.                                     */
/*****************************************************************************/

void espresso_run(stateADT* state_table,regionADT region,int nsignals,
		  int j,int l,bool gatelevel,bool atomic,bool sharegates)
{
  FILE *fp;
  int i,k;
  stateADT curstate;

  fp=Fopen("input","w");
  fprintf(fp,".i %d\n",nsignals);
  fprintf(fp,".o 1\n");
  fprintf(fp,".type fr\n");
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      for (k=0;k<nsignals;k++)
	fprintf(fp,"%c",ESP_VAL(curstate->state[k]));
      fprintf(fp," ");
      if (l==1) {
	if (((curstate->state[j]=='R') || (curstate->state[j]=='U')) &&
	    ((problem(curstate->state,region->enstate,nsignals)) ||
	    (!gatelevel)))
	  fprintf(fp,"1");
	else if ((curstate->state[j]=='R') || (curstate->state[j]=='U'))
	  if (atomic) fprintf(fp,"1");  
	  else if (sharegates) fprintf(fp,"-");
	  else fprintf(fp,"0");
	else if (curstate->state[j]=='1')
	  if (atomic) fprintf(fp,"1");  
	  else fprintf(fp,"-");
	else fprintf(fp,"0"); 
      } else {
	if (((curstate->state[j]=='F') || (curstate->state[j]=='D')) &&
	    ((problem(curstate->state,region->enstate,nsignals)) ||
	     (!gatelevel)))
	  fprintf(fp,"1");
	else if ((curstate->state[j]=='F') || (curstate->state[j]=='D'))
	  if (atomic) fprintf(fp,"1"); 
	  else if (sharegates) fprintf(fp,"-");
	  else fprintf(fp,"0");
	else if (curstate->state[j]=='0') 
	  if (atomic) fprintf(fp,"1"); 
	  else fprintf(fp,"-");
	else fprintf(fp,"0"); 
      }
      fprintf(fp,"\n");
    }
  fprintf(fp,".e\n");
  fclose(fp);
  system("espresso -Dprimes < input > output");
}

/*****************************************************************************/
/* Find all primes for implementation.                                       */
/*****************************************************************************/

int find_all_primes(char * filename,signalADT *signals,stateADT *state_table,
		    regionADT *regions,int ninpsig,int nsignals,bool gatelevel,
		    bool verbose,bool atomic,bool sharegates)
{
  int i,l;
  int nprimes;
  FILE *fp=NULL;
  char outname[FILENAMESIZE];
  regionADT cur_region;

  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".prm");
    printf("Finding all primes and storing to:  %s\n",outname);
    fprintf(lg,"Finding all primes and storing to:  %s\n",outname);
    fp=Fopen(outname,"w");
  } else {
    printf("Finding all primes ... ");
    fflush(stdout);
    fprintf(lg,"Finding all primes ... ");
    fflush(lg);
  }
  for (i=ninpsig;i<nsignals;i++) 
    for (l=1;l<3;l++)
      for (cur_region=regions[2*i+l];cur_region;cur_region=cur_region->link) {
	nprimes=nsignals*nsignals;
	/*	printf("%s %d\n",signals[i]->name,l); */
	espresso_run(state_table,cur_region,nsignals,i,l,gatelevel,atomic,
		     sharegates);
	if (!find_primes(fp,signals,state_table,regions,cur_region,nsignals,
			 &nprimes,i,l,gatelevel,verbose,atomic)) {
	  if (verbose) fclose(fp);
	  else {
	    printf("ERROR!\n");
	    fprintf(lg,"ERROR!\n");
	  }
	  return(0);
	}
	cur_region->nprimes=nprimes;
	/*printf("Number of candidate primes = %d\n",cur_region->nvprimes);*/
	/*	printf("nprimes = %d\n",nprimes); */
      }
  if (verbose) fclose(fp);
  else {
    printf("done\n");
    fprintf(lg,"done\n");
  }
  return(nprimes);
}

/*****************************************************************************/
/* Find implementation using an exact covering algorithm.                    */
/*****************************************************************************/

bool exactcover(char * filename,signalADT *signals,stateADT *state_table,
		regionADT *regions,int ninpsig,int nsignals,
		bool gatelevel,bool verbose,bool atomic,bool exception,
		bool exact,bool sharegates,bool combo)
{
  int nprimes; 

  if ((nprimes=find_all_primes(filename,signals,state_table,regions,
			       ninpsig,nsignals,gatelevel,verbose,
			       atomic,sharegates))==0) {
/* free primes */
    return FALSE;
  }
  build_pi_tables(filename,signals,state_table,regions,ninpsig,nsignals,
		  gatelevel,verbose,atomic,sharegates);
  find_prime_cov(filename,signals,state_table,regions,ninpsig,nsignals,
		 verbose,exception,exact,combo,gatelevel);
/*
  for (i=ninputs+1;i<nevents;i++)
    new_regions(RESET,TRUE,nprimes,nprimes,primes[i]);
*/
  return TRUE;
}

/*****************************************************************************/
/* Find exact cover.                                                         */
/*****************************************************************************/

bool find_exact_cover(designADT design)
{
  bool verbose;

  if (!(design->status & EXACTPRS)) {
    verbose=design->verbose;
    if (!exactcover(design->filename,design->signals,design->state_table,
		    design->regions,design->ninpsig,design->nsignals,
		    design->gatelevel,design->verbose,design->atomic,
		    design->exception,design->exact,design->sharegates,
		    design->combo)) {
      verbose=design->verbose;
      return(FALSE);
    }
    design->status=design->status | FOUNDCONF | FOUNDCOVER | EXACTPRS;
  }
  return(TRUE);
}


