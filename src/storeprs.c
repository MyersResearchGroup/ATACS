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
/* storeprs.c                                                                */
/*****************************************************************************/

#include "storeprs.h"
#include "interface.h"
#include "def.h"
#include "memaloc.h"
#include "breakup.h"
#include "findconf.h"
#include "bcp.h"
#include <sys/time.h>
#include <unistd.h>

/*****************************************************************************/
/* Print a guard.                                                            */
/*****************************************************************************/

int print_guard(FILE *fp,signalADT *signals,char * cover,int nsignals,int i,
		int *fanout,bool comb)
{
  int j,literals;
  bool first;

  first=TRUE;
  literals=0;
  fprintf(fp,"[");
  if ((i % 2==1) && (comb)) fprintf(fp," ");
  else if ((i % 2==0) && (comb)) fprintf(fp,"~");
  else if (i % 2==1) fprintf(fp,"+");
  else fprintf(fp,"-");
  fprintf(fp,"%s: (",signals[(i-1)/2]->name);
  for (j=0;j<nsignals;j++) 
    if (cover[j]=='1') {
      if (!first) fprintf(fp," & ");
      else first=FALSE;
      fprintf(fp,"%s",signals[j]->name);
      literals++;
      if (fanout != NULL) 
	if (i % 2==1) fanout[2*j+1]++;
	else fanout[2*j+2]++;
    } else if (cover[j]=='0') {
      if (!first) fprintf(fp," & ");
      else first=FALSE;
      fprintf(fp,"~%s",signals[j]->name);
      literals++;
      if (fanout != NULL) 
	if (i % 2==1) fanout[2*j+2]++;
	else fanout[2*j+1]++;
    } 
  if (comb) fprintf(fp,")] Combinational\n");
  else fprintf(fp,")]\n");
  return(literals);
}

bool cover_includes(char * enstate,char * cover,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++) {
    if (cover[i]=='E') return FALSE;
    if ((cover[i]!='X') && (cover[i]!='Z') &&
	(cover[i]!=enstate[i]) && 
	!((cover[i]=='0') && (enstate[i]=='R')) && 
	!((cover[i]=='1') && (enstate[i]=='F'))) return(FALSE);
  }
  return TRUE;
}

tableSolADT find_multicube_prs(signalADT *signals,regionADT *regions,
			       int ninpsig,int nsignals,bool exact)
{
  regionADT cur_region1,cur_region2;
  tableADT newtab,old=NULL;
  tableSolADT Sol,NewSol;
  int i,j,k,l,m,nregions,U;
  int *cost;
  coverlistADT cur_cover,cur_cover2;
  bool first,breakout;

  /* Determine table size */
  nregions=0;
  for (l=2*ninpsig+1;l<(2*nsignals);l++)
    for (cur_region1=regions[l];cur_region1;cur_region1=cur_region1->link) 
      if (exact && cur_region1->covers && cur_region1->covers->cover) {
	for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	     cur_cover=cur_cover->link) {
	  nregions++;
	  printf("%d:%s\n",nregions,cur_cover->cover);
	} 
      } else {
	printf("%d:%s\n",nregions,cur_region1->cover);
	nregions++;
      }
  Sol=(tableSolADT)GetBlock(sizeof(*(Sol)));
  Sol->currentSol=(char *)GetBlock((nregions+1) * sizeof(char));
  Sol->size=nregions;
  Sol->cost=0;
  for (i=0;i<Sol->size;i++)
    Sol->currentSol[i]='-';
  Sol->currentSol[Sol->size]='\0';

  /* Determine cost function (number of literals) */
  j=0;
  cost=(int *)GetBlock(nregions*sizeof(int));
  for (l=2*ninpsig+1;l<(2*nsignals);l++)
    for (cur_region1=regions[l];cur_region1;cur_region1=cur_region1->link) 
      if (exact && cur_region1->covers && cur_region1->covers->cover) {
	for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	     cur_cover=cur_cover->link) {
	  cost[j]=0;
	  for (k=0;k<nsignals;k++)
	    if ((cur_cover->cover[k]=='1') || (cur_cover->cover[k]=='0')) 
	      cost[j]++;
	  j++;
	} 
      } else {
	cost[j]=0;
	for (k=0;k<nsignals;k++)
	  if ((cur_region1->cover[k]=='1') || (cur_region1->cover[k]=='0')) 
	    cost[j]++;
	j++;
      }
  /* Construct table */
  for (l=2*ninpsig+1;l<(2*nsignals);l++) {
    k=0;
    for (cur_region1=regions[l];cur_region1;cur_region1=cur_region1->link) {
      newtab=(tableADT)GetBlock(sizeof(*(newtab)));
      newtab->row=(char *)GetBlock((nregions+1)*sizeof(char));
      newtab->size=nregions;
      i=0;
      for (m=2*ninpsig+1;m<(2*nsignals);m++)
	for (cur_region2=regions[m];cur_region2;
	     cur_region2=cur_region2->link) {
	  if (exact && cur_region2->covers && cur_region2->covers->cover) {
	    for (cur_cover=cur_region2->covers;cur_cover && cur_cover->cover;
		 cur_cover=cur_cover->link) {
	      newtab->row[i]='0';
	      if ((l==m) && (cover_includes(cur_region1->enstate,
					    cur_cover->cover,nsignals))) {
		newtab->row[i]='1';
	      } else {
		if (exact && cur_region1->covers && 
		    cur_region1->covers->cover) {
		  for (cur_cover2=cur_region1->covers;cur_cover2 && 
		       cur_cover2->cover;
		       cur_cover2=cur_cover2->link) {
		    if (strcmp(cur_cover2->cover,cur_cover->cover)==0) {
		      newtab->row[i]='1';
		      break;
		    } 
		  }
		} else if (strcmp(cur_region1->cover,cur_cover->cover)==0) {
		  newtab->row[i]='1';
		} 
	      }
	      i++;
	    }	   
	  } else {
	    if (((l==m) && (cover_includes(cur_region1->enstate,
					  cur_region2->cover,nsignals))) ||
		(strcmp(cur_region1->cover,cur_region2->cover)==0)) {
	      newtab->row[i]='1';
	    } else
	      newtab->row[i]='0';
	    i++;
	  }
	}
      newtab->row[i]='\0';
      if ((l==(2*ninpsig+1)) && (k==6)) 
	for (j=0;j<nregions;j++)
	  if (newtab->row[j]=='1') printf("%d\n",j);
      newtab->link=old;
      old=newtab;
      k++;
    }
  }
  Sol->F=old;

  U=0;
  for (j=0;j<nregions;j++)
    U = U + cost[j];
  U++;
  print_current_solution(stdout,Sol);
  NewSol=BCP(Sol,&U,cost,0);
  print_current_solution(stdout,NewSol);

  m=0;
  for (l=2*ninpsig+1;l<(2*nsignals);l++)
    for (cur_region1=regions[l];cur_region1;cur_region1=cur_region1->link) 
      if (exact && cur_region1->covers && cur_region1->covers->cover) {
	for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	     cur_cover=cur_cover->link) {
	  if (NewSol->currentSol[m]=='1') {
	    first=TRUE;
	    for (i=0;i<nsignals;i++)
	      if (cur_cover->cover[i]=='1') {
		if (!first) printf(" & "); else first=FALSE;
		printf("%s",signals[i]->name);
	      } else if (cur_cover->cover[i]=='0') {
		if (!first) printf(" & "); else first=FALSE;
		printf("~%s",signals[i]->name);
	      }
	    printf(" -> ");
	    for (j=2*ninpsig+1;j<(2*nsignals);j++)
	      for (cur_region2=regions[j];cur_region2;
		   cur_region2=cur_region2->link) {
		if ((l==j) && (cover_includes(cur_region2->enstate,
					      cur_cover->cover,nsignals))) { 
		  printf("%s ",signals[(j-1)/2]->name);
		  break;
		}
		if (exact && cur_region2->covers && 
		    cur_region2->covers->cover) {
		  breakout=FALSE;
		  for (cur_cover2=cur_region2->covers;cur_cover2 && 
			 cur_cover2->cover;
		       cur_cover2=cur_cover2->link) {
		    if (strcmp(cur_cover2->cover,cur_cover->cover)==0) { 
		      printf("%s ",signals[(j-1)/2]->name);
		      breakout=TRUE;
		      break;
		    }
		  }
		  if (breakout) break;
		} else if (strcmp(cur_region2->cover,cur_cover->cover)==0) {
		  printf("%s ",signals[(j-1)/2]->name);
		  break;
		}
	      }
	    printf("\n");
	  }
	  m++;
	} 
      } else {
	if (NewSol->currentSol[m]=='1') {
	  first=TRUE;
	  for (i=0;i<nsignals;i++)
	    if (cur_region1->cover[i]=='1') {
	      if (!first) printf(" & "); else first=FALSE;
	      printf("%s",signals[i]->name);
	    } else if (cur_region1->cover[i]=='0') {
	      if (!first) printf(" & "); else first=FALSE;
	      printf("~%s",signals[i]->name);
	    }
	    printf(" -> ");
	    for (j=2*ninpsig+1;j<(2*nsignals);j++)
	      for (cur_region2=regions[j];cur_region2;
		   cur_region2=cur_region2->link) {
		if ((l==j) && (cover_includes(cur_region2->enstate,
					      cur_region1->cover,nsignals))) {
		  printf("%s ",signals[(j-1)/2]->name);
		  break;
		}
		if (exact && cur_region2->covers && 
		    cur_region2->covers->cover) {
		  breakout=FALSE;
		  for (cur_cover2=cur_region2->covers;cur_cover2 && 
			 cur_cover2->cover;
		       cur_cover2=cur_cover2->link) {
		    if (strcmp(cur_cover2->cover,cur_region1->cover)==0) {
		      printf("%s ",signals[(j-1)/2]->name);
		      breakout=TRUE;
		      break;
		    }
		  }
		  if (breakout) break;
		} else if (strcmp(cur_region2->cover,cur_region1->cover)==0) {
		  printf("%s ",signals[(j-1)/2]->name);
		  break;
		}
	      }
	  printf("\n");
	}
	m++;
      }


  if (!NewSol) {
    printf("ERROR: Could not cover excitation regions with covers.\n");
    fprintf(lg,"ERROR: Could not cover excitation regions with covers.\n");
    return NULL;
  }
  free(cost);

  return NewSol;
}

tableSolADT find_minimal_prs(regionADT region,int nsignals,bool exact)
{
  regionADT cur_region1,cur_region2;
  tableADT newtab,old=NULL;
  tableSolADT Sol,NewSol;
  int i,j,k,nregions,U;
  int *cost;
  coverlistADT cur_cover;

  /* Determine table size */
  nregions=0;
  for (cur_region1=region;cur_region1;cur_region1=cur_region1->link) 
    if (exact && cur_region1->covers && cur_region1->covers->cover)
      for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	   cur_cover=cur_cover->link)
	nregions++;
    else
      nregions++;
  Sol=(tableSolADT)GetBlock(sizeof(*(Sol)));
  Sol->currentSol=(char *)GetBlock((nregions+1) * sizeof(char));
  Sol->size=nregions;
  Sol->cost=0;
  for (i=0;i<Sol->size;i++)
    Sol->currentSol[i]='-';
  Sol->currentSol[Sol->size]='\0';

  /* Determine cost function (number of literals) */
  j=0;
  cost=(int *)GetBlock(nregions*sizeof(int));
  for (cur_region1=region;cur_region1;cur_region1=cur_region1->link) 
    if (exact && cur_region1->covers && cur_region1->covers->cover) {
      for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	   cur_cover=cur_cover->link) {
	cost[j]=0;
	for (k=0;k<nsignals;k++)
	  if ((cur_cover->cover[k]=='1') || (cur_cover->cover[k]=='0')) 
	    cost[j]++;
	j++;
      } 
    } else {
      cost[j]=0;
      for (k=0;k<nsignals;k++)
	if ((cur_region1->cover[k]=='1') || (cur_region1->cover[k]=='0')) 
	  cost[j]++;
      j++;
    }
  /* Construct table */
  for (cur_region1=region;cur_region1;cur_region1=cur_region1->link) {
    newtab=(tableADT)GetBlock(sizeof(*(newtab)));
    newtab->row=(char *)GetBlock((nregions+1)*sizeof(char));
    newtab->size=nregions;
    i=0;
    for (cur_region2=region;cur_region2;cur_region2=cur_region2->link) {
      if (exact && cur_region2->covers && cur_region2->covers->cover) {
	for (cur_cover=cur_region2->covers;cur_cover && cur_cover->cover;
	     cur_cover=cur_cover->link) {
	  if (cover_includes(cur_region1->enstate,cur_cover->cover,nsignals)) {
	    newtab->row[i]='1';
	  } else
	    newtab->row[i]='0';
	  i++;
	}
      } else {
	if (cover_includes(cur_region1->enstate,cur_region2->cover,nsignals)) {
	  newtab->row[i]='1';
	} else
	  newtab->row[i]='0';
	i++;
      }
    }
    newtab->row[i]='\0';
    newtab->link=old;
    old=newtab;
  }

  Sol->F=old;

  //print_current_solution(stdout,Sol);

  U=0;
  for (j=0;j<nregions;j++)
    U = U + cost[j];
  U++;
  /*nsolutions=nregions;*/
  NewSol=BCP(Sol,&U,cost,0);
  if (!NewSol) {
    printf("ERROR: Could not cover excitation regions with covers.\n");
    fprintf(lg,"ERROR: Could not cover excitation regions with covers.\n");
    /*
    for (cur_region1=region;cur_region1;cur_region1=cur_region1->link) {
      printf("ec=%s\n",cur_region1->enstate);
      if (exact && cur_region1->covers && cur_region1->covers->cover) {
	for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	     cur_cover=cur_cover->link) {
	  printf(" c=%s %d\n",cur_cover->cover,cost[i]);
	}
      } else {
	printf(" c=%s\n",cur_region1->cover);
      }
    }
    */
    return NULL;
  }
  //delete_tableSol(Sol);
  /*
  print_current_solution(stdout,NewSol);
  i=0;
  for (cur_region1=region;cur_region1;cur_region1=cur_region1->link)  
    if (exact && cur_region1->covers && cur_region1->covers->cover) {
      for (cur_cover=cur_region1->covers;cur_cover && cur_cover->cover;
	   cur_cover=cur_cover->link) {
	if (NewSol->currentSol[i]=='1')
	  printf("%s %d\n",cur_cover->cover,cost[i]);
	i++;
      }
    } else {
      if (NewSol->currentSol[i]=='1')
	printf("%s %d\n",cur_region1->cover,cost[i]);
      i++;
    }
  */
  free(cost);

  return NewSol;
}

/*****************************************************************************/
/* Check if current region is covered by an upcoming guard.                  */
/*****************************************************************************/

bool symmetric_guard(regionADT start,regionADT region,int nsignals)
{
  regionADT cur_region;
  bool symmetric;
  bool last;

  /* delete regions with unresolved conflicts */
  if (region->context_table != NULL) return TRUE; 

  symmetric=FALSE;
  last=FALSE;
  for (cur_region=start;cur_region;cur_region=cur_region->link) 
    if (cur_region->context_table==NULL) {
      if (cur_region==region) last=TRUE;
      if (cur_region != region) {
	if (problem(region->cover/*enstate*/,cur_region->cover,nsignals))
	  /* ??? */
	  if ((!problem(cur_region->cover/*enstate*/,
			region->cover,nsignals))||(last))
	  {
	    symmetric=TRUE;
	  } 
      }
    }
  return(symmetric);
}

bool covers_include(regionADT region,char * curstate,bool exact,int nsignals,
		    tableSolADT NewSol)
{
  int j;
  regionADT cur_region;
  coverlistADT cur_cover;

  j=(-1);
  for (cur_region=region;cur_region;cur_region=cur_region->link) {
    if (exact && cur_region->covers && cur_region->covers->cover) {
      for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
	   cur_cover=cur_cover->link) {
	j++;
	if (((!NewSol) || NewSol->currentSol[j]=='1') &&
	    (cover_includes(curstate,cur_cover->cover,nsignals)))
	  return TRUE;
      } 
    } else {
      j++;
      if (((!NewSol) || NewSol->currentSol[j]=='1') &&
	  (cover_includes(curstate,cur_region->cover,nsignals)))
	return TRUE;
    } 
  }
  return FALSE;
}

bool check_combo_compressed(regionADT region,stateADT curstate,bool exact,
			    int nsignals,tableSolADT NewSol)
{
  statelistADT nextstate;
  char * tmpstate;
  int l;

  tmpstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (nextstate=curstate->succ;nextstate;
       nextstate=nextstate->next) {
    strcpy(tmpstate,curstate->state);
    for (l=0;l<nsignals;l++) 
      if ((curstate->state[l]=='U') && 
	  (nextstate->stateptr->state[l]=='1')) {
	tmpstate[l]='0';
	if (!covers_include(region,tmpstate,exact,nsignals,NewSol)) {
	  free(tmpstate);
	  return FALSE;
	}
	tmpstate[l]='U';
      } else if ((curstate->state[l]=='D') && 
		 (nextstate->stateptr->state[l]=='0')) {
	tmpstate[l]='1';
	if (!covers_include(region,tmpstate,exact,nsignals,NewSol)) {
	  free(tmpstate);
	  return FALSE;
	}
	tmpstate[l]='D';
      }
  }
  free(tmpstate);
  return TRUE;
}

/*****************************************************************************/
/* Remove unnecessary C-elements.                                            */
/*****************************************************************************/

bool check_combo(stateADT *state_table,regionADT *regions,int nsignals,
		bool combo,bool exact,tableSolADT NewSol,int k,int l)
{
  int i,j;
  stateADT curstate;
  bool compressed;

  if (!combo) return FALSE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      compressed=FALSE;
      for (j=0;j<nsignals;j++) 
	if ((curstate->state[j]=='U')||(curstate->state[j]=='D')) {
	  compressed=TRUE;
	  break;
	}
      if (l==0) { 
	if ((curstate->state[k]=='1') || (curstate->state[k]=='R') ||
	    (curstate->state[k]=='U')) {
	  if (compressed) {
	    if (!check_combo_compressed(regions[2*k+1],curstate,exact,nsignals,
					NewSol)) 
	      return FALSE;
	  } else {
	    if (!covers_include(regions[2*k+1],curstate->state,exact,nsignals,
				NewSol))
	      return FALSE;
	  }
	}
      }	else {
	if ((curstate->state[k]=='0') || (curstate->state[k]=='F') || 
	    (curstate->state[k]=='D')) {
	  if (compressed) {
	    if (!check_combo_compressed(regions[2*k+2],curstate,exact,nsignals,
					NewSol)) 
	      return FALSE;
	  } else {
	    if (!covers_include(regions[2*k+2],curstate->state,exact,nsignals,
				NewSol))
	      return FALSE;
	  }
	}
      }
    }
  return TRUE;
}

/*****************************************************************************/
/* Remove unnecessary C-elements.                                            */
/*****************************************************************************/

bool *check_combinational(stateADT *state_table,regionADT *regions,
			  int ninpsig,int nsignals,bool combo)
{
  int i,j,k,l;
  stateADT curstate;
  bool *comb;
  regionADT cur_region;
  char * tmpstate;
  bool compressed;
  statelistADT nextstate;
  timeval t0,t1;
  double time0, time1;

  gettimeofday(&t0, NULL);
  printf("Finding combinational covers ... ");
  fflush(stdout);
  fprintf(lg,"Finding combinational covers ... ");
  fflush(lg);
  comb=(bool*)GetBlock(2*nsignals*sizeof(bool));
  if (!combo) {
    for (i=0;i<2*nsignals;i++)
      comb[i]=FALSE;
    return comb;
  }
  tmpstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (i=0;i<2*nsignals;i++)
    comb[i]=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      compressed=FALSE;
      for (j=0;j<nsignals;j++) 
	if ((curstate->state[j]=='U')||(curstate->state[j]=='D')) {
	  compressed=TRUE;
	  break;
	}
      for (k=ninpsig;k<nsignals;k++) {
	if (comb[k]==TRUE)
	  if ((curstate->state[k]=='1') || (curstate->state[k]=='R') ||
	      (curstate->state[k]=='U')) {
	    if (compressed) {
	      for (nextstate=curstate->succ;nextstate;
		   nextstate=nextstate->next) {
		strcpy(tmpstate,curstate->state);
		for (l=0;l<nsignals;l++) 
		  if ((curstate->state[l]=='U') && 
		      (nextstate->stateptr->state[l]=='1')) {
		    tmpstate[l]='0';
		    comb[k]=FALSE;
		    for (cur_region=regions[2*k+1];cur_region;
			 cur_region=cur_region->link)
		      if ((!symmetric_guard(regions[2*k+1],cur_region,
					    nsignals)) &&
			  (cover_includes(tmpstate,cur_region->cover,
					  nsignals))) 
			comb[k]=TRUE;
		    tmpstate[l]='U';
		  } else if ((curstate->state[l]=='D') && 
			     (nextstate->stateptr->state[l]=='0')) {
		    tmpstate[l]='1';
		    comb[k]=FALSE;
		    for (cur_region=regions[2*k+1];cur_region;
			 cur_region=cur_region->link)
		      if ((!symmetric_guard(regions[2*k+1],cur_region,
					    nsignals)) &&
			  (cover_includes(tmpstate,cur_region->cover,
					  nsignals))) 
			comb[k]=TRUE;
		    tmpstate[l]='D';
		  }
	      }
	    } else {
	      comb[k]=FALSE;
	      for (cur_region=regions[2*k+1];cur_region;
		   cur_region=cur_region->link)
		if ((!symmetric_guard(regions[2*k+1],cur_region,nsignals)) &&
		    (problem(curstate->state,cur_region->cover,
			     nsignals))) 
		  comb[k]=TRUE;
	    }
	  }
	if (comb[k+nsignals]==TRUE)
	  if ((curstate->state[k]=='0') || (curstate->state[k]=='F') || 
	      (curstate->state[k]=='D')) {
	    if (compressed) {
	      for (nextstate=curstate->succ;nextstate;
		   nextstate=nextstate->next) {
		strcpy(tmpstate,curstate->state);
		for (l=0;l<nsignals;l++) 
		  if ((curstate->state[l]=='U') && 
		      (nextstate->stateptr->state[l]=='1')) {
		    tmpstate[l]='0';
		    comb[k+nsignals]=FALSE;
		    for (cur_region=regions[2*k+2];cur_region;
			 cur_region=cur_region->link)
		      if ((!symmetric_guard(regions[2*k+2],cur_region,
					    nsignals)) &&
			  (cover_includes(tmpstate,cur_region->cover,
					  nsignals))) 
			comb[k+nsignals]=TRUE;
		    tmpstate[l]='U';
		  } else if ((curstate->state[l]=='D') && 
		      (nextstate->stateptr->state[l]=='0')) {
		    tmpstate[l]='1';
		    comb[k+nsignals]=FALSE;
		    for (cur_region=regions[2*k+2];cur_region;
			 cur_region=cur_region->link)
		      if ((!symmetric_guard(regions[2*k+2],cur_region,
					    nsignals)) &&
			  (cover_includes(tmpstate,cur_region->cover,
					  nsignals))) 
			comb[k+nsignals]=TRUE;
		    tmpstate[l]='D';
		  }
	      }
	    } else {
	      comb[k+nsignals]=FALSE;
	      for (cur_region=regions[2*k+2];cur_region;
		   cur_region=cur_region->link)
		if ((!symmetric_guard(regions[2*k+2],cur_region,nsignals)) &&
		    (problem(curstate->state,cur_region->cover,
			     nsignals))) 
		  comb[k+nsignals]=TRUE;
	    }
	  }
      }
    }
  free(tmpstate);
  gettimeofday(&t1,NULL);	
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  printf("done (%f)\n",time1-time0);
  fprintf(lg,"done (%f)\n",time1-time0);
  return(comb);
}

/*****************************************************************************/
/* Check if guards are disjoint.                                             */
/*****************************************************************************/

bool disjoint_check(char * filename,signalADT *signals,stateADT *state_table,
		    regionADT *regions,bool *comb,int ninpsig,int nsignals)
{
  int i,k;
  stateADT curstate;
  regionADT first_region=NULL;
  regionADT cur_region;
  bool covered;
  bool okay;
  FILE *errfp=NULL;
  char outname[FILENAMESIZE];

  okay=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
      for (k=ninpsig;k<nsignals;k++) {
	if ((curstate->state[k]=='1') || (curstate->state[k]=='R')) {
	  covered=FALSE;
	  for (cur_region=regions[2*k+1];cur_region;
	       cur_region=cur_region->link)
	    if ((!symmetric_guard(regions[2*k+1],cur_region,nsignals)) &&
		(problem(curstate->state,cur_region->cover,nsignals))) 
	      if (!covered) { 
		first_region=cur_region;
		covered=TRUE;
	      } else {
		if (okay) {
		  strcpy(outname,filename);
		  strcat(outname,".err");
		  printf("ERROR: Storing non-disjoint guards to:  %s\n",
			 outname);
		  fprintf(lg,"ERROR: Storing non-disjoint guards to:  %s\n",
			  outname);
		  errfp=fopen(outname,"w");
		  if (errfp==NULL) {
		    printf("ERROR: Unable to open file %s!\n",outname);
		    fprintf(lg,"ERROR: Unable to open file %s!\n",outname);
		    return FALSE;
		  }
		  okay=FALSE;
		}
		print_guard(errfp,signals,first_region->cover,nsignals,2*k+1,
			    NULL,comb[k]);
		print_guard(errfp,signals,cur_region->cover,nsignals,2*k+1,
			    NULL,comb[k]);
		fprintf(errfp,"---\n");
	      }
	}
	if ((curstate->state[k]=='0') || (curstate->state[k]=='F')) {
	  covered=FALSE;
	  for (cur_region=regions[2*k+2];cur_region;
	       cur_region=cur_region->link)
	    if ((!symmetric_guard(regions[2*k+2],cur_region,nsignals)) &&
		(problem(curstate->state,cur_region->cover,nsignals))) 
	      if (!covered) {
		first_region=cur_region;
		covered=TRUE;
	      } else {
		if (okay) {
		  strcpy(outname,filename);
		  strcat(outname,".err");
		  printf("ERROR: Storing non-disjoint guards to:  %s\n",
			 outname);
		  fprintf(lg,"ERROR: Storing non-disjoint guards to:  %s\n",
			  outname);
		  errfp=fopen(outname,"w");
		  if (errfp==NULL) {
		    printf("ERROR: Unable to open file %s!\n",outname);
		    fprintf(lg,"ERROR: Unable to open file %s!\n",outname);
		    return FALSE;
		  }
		  okay=FALSE;
		}
		print_guard(errfp,signals,first_region->cover,nsignals,2*k+2,
			    NULL,comb[k+nsignals]);
		print_guard(errfp,signals,cur_region->cover,nsignals,2*k+2,
			    NULL,comb[k+nsignals]);
		fprintf(errfp,"---\n");
	      }
	}
      }
  if (!okay) fclose(errfp);
  return(okay);
}

bool print_cover(FILE *fp,char * filename,signalADT *signals,char * cover,
		 int *fanout,bool comb1,bool comb2,int i,int nsignals,
		 int maxsize,bool *first,bool *okay,int *area,int *total_lits,
		 FILE **errfp)
{
  char outname[FILENAMESIZE];
  int literals=0;
  bool comb;

  if (i % 2==1) comb=comb1;
  else comb=comb2;
  if (((i % 2==0) && (!comb1)) || 
      ((i % 2==1) && (!comb2)) ||
      ((i % 2==1) && (comb1))) {
    if (area)
      if ((*first) && (!comb)) (*area)+=1;
    if (first)
      (*first)=FALSE;
    if (!fanout)
      print_guard(fp,signals,cover,nsignals,i,fanout,FALSE);
    else 
      if ((literals=print_guard(fp,signals,cover,nsignals,i,fanout,comb)) 
	  > maxsize) {
	if (okay)
	  if (*okay) {
	    strcpy(outname,filename);
	    strcat(outname,".err");
	    printf("WARNING: Storing guards with fanin > %d to:  %s\n",
		   maxsize,outname);
	    fprintf(lg,"WARNING: Storing guards with fanin > %d to:  %s",
		    maxsize,outname);
	    fprintf(lg,"\n");
	    (*errfp)=fopen(outname,"w");
	    if ((*errfp)==NULL) {
	      fclose(fp);
	      free(fanout);
	      printf("ERROR: Unable to open file %s!\n",outname);
	      fprintf(lg,"ERROR: Unable to open file %s!\n",outname);
	      return FALSE;
	    }
	    (*okay)=FALSE;
	  }
	if (errfp)
	  print_guard(*errfp,signals,cover,nsignals,i,NULL,comb);
      } 
    if (area) {
      (*area)+=literals;
      if (comb) (*area)+=literals;
    }
    if (total_lits)
      (*total_lits)+=literals;
  }
  return TRUE;
} 

bool print_covers(FILE *fp,char * filename,signalADT *signals,
		  regionADT *regions,bool exact,int *fanout,bool comb1,
		  bool comb2,int i,int nsignals,int maxsize,bool *first,
		  bool *okay,int *area,int *total_lits,FILE **errfp,
		  tableSolADT NewSol)
{
  int j,k,l;
  regionADT cur_region;
  coverlistADT cur_cover;

  j=(-1);
  k=0;
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link) {
    if (!NewSol) {
      fprintf(fp,"Covers for %s",signals[(i-1)/2]->name);
      if (i % 2==1) fprintf(fp,"+,%d\n",k);
      else fprintf(fp,"-,%d\n",k);
      k++;
    }
    if (exact && cur_region->covers && cur_region->covers->cover) {
      if (cur_region->cover) {
	for (l=0;l<nsignals;l++)
	  cur_region->cover[l]='E';
      }
      for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
	   cur_cover=cur_cover->link) {
	j++;
	if ((!NewSol) || NewSol->currentSol[j]=='1') {
	  if (!print_cover(fp,filename,signals,cur_cover->cover,fanout,comb1,
			   comb2,i,nsignals,maxsize,first,okay,area,
			   total_lits,errfp)) 
	    return FALSE;
	  else 
	    if (((i % 2==0) && (!comb1)) || 
		((i % 2==1) && (!comb2)) ||
		((i % 2==1) && (comb1)))
	      cur_region->cover=CopyString(cur_cover->cover);
	} else {
	  for (l=0;l<nsignals;l++)
	    cur_cover->cover[l]='E';
	}
      }
    } else {
      j++;
      if ((!NewSol) || NewSol->currentSol[j]=='1') {
	if (!print_cover(fp,filename,signals,cur_region->cover,fanout,comb1,
			 comb2,i,nsignals,maxsize,first,okay,area,
			 total_lits,errfp))
	  return FALSE;
	else
	  if (!(((i % 2==0) && (!comb1)) || 
		((i % 2==1) && (!comb2)) ||
		((i % 2==1) && (comb1))))
	    if (cur_region->cover) {
	      for (l=0;l<nsignals;l++)
		cur_region->cover[l]='E';
	    }
      } else {
	if (cur_region->cover) {
	  for (l=0;l<nsignals;l++)
	    cur_region->cover[l]='E';
	}
      }
    } 
  }
  return TRUE;
}
    
/*****************************************************************************/
/* Store production rules.                                                   */
/*****************************************************************************/

bool store_prs(char * filename,signalADT *signals,stateADT *state_table,
	       regionADT *regions,int ninpsig,int nsignals,int maxsize,
	       bool gatelevel,bool combo,bool exact,int& total_lits, 
	       int & area)
{
  char outname[FILENAMESIZE];
  FILE *fp,*errfp;
  int i;
  bool okay,first,comb1,comb2;
  int *fanout;
  tableSolADT NewSol1,NewSol2;

  strcpy(outname,filename);
  strcat(outname,".prs");
  printf("Storing production rules to:  %s\n",outname);
  fprintf(lg,"Storing production rules to:  %s\n",outname);
  fp=fopen(outname,"w");
  if (fp==NULL) {
    printf("ERROR: Unable to open file %s!\n",outname);
    fprintf(lg,"ERROR: Unable to open file %s!\n",outname);
    return FALSE;
  }
  okay=TRUE; 
  area=0;
  total_lits=0;
  errfp=NULL;
  fanout=(int*)GetBlock((2*nsignals+1)*sizeof(int));
  for (i=0;i<(2*nsignals+1);i++) fanout[i]=0;  
  for (i=2*ninpsig+1;i<(2*nsignals+1);i+=2) {
    first=TRUE;
    //printf("%s+\n",signals[(i-1)/2]->name);
    NewSol1=find_minimal_prs(regions[i],nsignals,exact);
    //printf("%s-\n",signals[(i-1)/2]->name);
    NewSol2=find_minimal_prs(regions[i+1],nsignals,exact);
    if (!NewSol1) return TRUE;
    if (!NewSol2) return TRUE;
    comb1=check_combo(state_table,regions,nsignals,combo,exact,NewSol1,
		      (i-1)/2,(i-1) % 2);
    comb2=check_combo(state_table,regions,nsignals,combo,exact,NewSol2,
		      i/2,i % 2);
    if (!print_covers(fp,filename,signals,regions,exact,fanout,comb1,comb2,i,
		      nsignals,maxsize,&first,&okay,&area,&total_lits,&errfp,
		      NewSol1))
      return FALSE;
    if (!print_covers(fp,filename,signals,regions,exact,fanout,comb1,comb2,i+1,
		      nsignals,maxsize,&first,&okay,&area,&total_lits,&errfp,
		      NewSol2))
      return FALSE;
    /* SHOULDN'T I DELETE NewSol1 and NewSol2 */
    delete_tableSol(NewSol1);
    delete_tableSol(NewSol2);
  }
  //find_multicube_prs(signals,regions,ninpsig,nsignals,exact);
  for (i=2;i<((2*nsignals)+1);i+=2)
    if (((regions[i]) && (i > ninpsig)) || (fanout[i] > 0)) area+=2;
  if (!okay) fclose(errfp);
  fclose(fp);
  free(fanout);
  printf("Synthesis succeeded with area = %d literals %d transistors\n",
	 total_lits,area);
  fprintf(lg,"Synthesis succeeded with area = %d literals %d transistors\n",
	  total_lits,area);
  return TRUE;//(okay); /* Need to handle this better for decomp */
}

/*****************************************************************************/
/* If a filename ends with "BRK" then strip it off.                          */
/*****************************************************************************/

void trim_filename_BRK(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 3) && (filename[length-3]=='B') &&
      (filename[length-2]=='R') && (filename[length-1]=='K'))
    filename[length-3]='\0';
}

/*****************************************************************************/
/* Store production rules.                                                   */
/*****************************************************************************/

bool store_production_rules(char command,designADT design)
{
  designADT cs_design=NULL;
  int cs_notreachable,status;
  char cs_command;
  int olddnevents;
  char shellcommand[100];
  regionADT *lastregions; 
  costADT cost;
  bool error;

  if (!(design->status & STOREDPRS)) {
    int total_lits(0);
    int area(0);
    if (!store_prs(design->filename,design->signals,design->state_table,
		   design->regions,design->ninpsig,design->nsignals,
		   design->maxsize,design->gatelevel,design->combo,
		   design->exact,total_lits,area)) {
      if ((command==BREAKUP) /*|| (design->brk_exception)*/) {
	trim_filename_BRK(design->filename);
	olddnevents=0;
	if (!breakup_gates(design->filename,design->signals,design->events,
			   design->merges,
			   design->rules,design->cycles,design->nevents,
			   design->ncycles,design->ninputs,design->maxsize,
			   design->maxgatedelay,design->startstate,TRUE,NULL,
			   design->status,design->regions,design->manual,
			   design->ninpsig,design->nsignals,&error)) {
	  trim_filename_BRK(design->filename);
/*	  design->nsignals=((design->nevents)-1)/2;*/
	  design->status |= (LOADED   | CONNECTED | FOUNDRED  | EXPANDED |
			     FOUNDRSG | FOUNDREG  | FOUNDCONF | FOUNDCOVER);
	  return(FALSE);
	}
	cs_design=initialize_design();
	context_swap(cs_design,design,&cs_command,command,&cs_notreachable,
		     notreachable,TRUE);
	design->brk_exception=FALSE; 
	design->fromCSP=FALSE;
	design->fromVHDL=FALSE;
	design->fromHSE=FALSE;
	design->fromER=TRUE;
	design->fromTEL=FALSE;
	design->fromG=FALSE;
	design->fromRSG=FALSE;
	design->status=process_command(SYNTHESIS,DOALL,design,NULL,TRUE,
				       design->si);
	design->brk_exception=design->exception;
	while (!(design->status & STOREDPRS)) {
	  if (design->status & FOUNDCOVER) {
	    cost=init_decomp1(design->ninpsig,design->nsignals,design->regions,
			      design->maxsize,cs_design->nsignals,
			      cs_design->regions);
/*	    printf("COST: %d high-fanin gates with a maximum fanin of %d\n",
		   cost->gates,cost->maxfanin); */
	    free(cost);
	    olddnevents=design->nevents;
	  }
	  if (design->status & FOUNDREG) lastregions=design->regions;
	  else lastregions=NULL;
	  new_design(design,FALSE);

	  status=design->status;

	  context_swap(design,cs_design,&command,cs_command,&notreachable,
		       cs_notreachable,FALSE);
	  trim_filename_BRK(design->filename);
	  design->status |=  LOADED;
	  if (!breakup_gates(design->filename,design->signals,design->events,
			     design->merges,design->rules,design->cycles,
			     design->nevents,design->ncycles,design->ninputs,
			     design->maxsize,design->maxgatedelay,
			     design->startstate,FALSE,lastregions,
			     status,
/*			     design->status, */
			     design->regions,design->manual,design->ninpsig,
			     design->nsignals,&error)){
	    if (!error) {
	      design->events=new_events(design->status,TRUE,design->nevents,
					design->nevents,design->events);
	      context_swap(cs_design,design,&cs_command,command,
			   &cs_notreachable,notreachable,FALSE);
	      cs_design->events=new_events(cs_design->status,TRUE,
					   cs_design->nevents,
					   cs_design->nevents,
					   cs_design->events);
	      new_design(cs_design,TRUE);
	      design->fromCSP=FALSE;
	      design->fromVHDL=FALSE;
	      design->fromHSE=FALSE;
	      design->fromER=TRUE;
	      design->fromTEL=FALSE;
	      design->fromG=FALSE;
	      design->fromRSG=FALSE;
	      design->status=process_command(EXCEPTIONS,BREAKUP,design,NULL,
					     TRUE,design->si);
	      if (design->status & STOREDPRS) return(TRUE);
	      else return(FALSE);
	    } else {
	      design->events=new_events(design->status,TRUE,design->nevents,
					design->nevents,design->events);
	      
	      design->events=cs_design->events;
	      trim_filename_BRK(design->filename);
	      design->status|= (LOADED   | CONNECTED | FOUNDRED  | EXPANDED |
				FOUNDRSG | FOUNDREG  | FOUNDCONF | FOUNDCOVER);
	      free(cs_design);
	      return(FALSE);
	    }
	  }
	  design->events=new_events(design->status,TRUE,design->nevents,
				    design->nevents,design->events);
	  context_swap(cs_design,design,&cs_command,command,&cs_notreachable,
		       notreachable,FALSE);
	  design->brk_exception=FALSE;
	  design->fromCSP=FALSE;
	  design->fromVHDL=FALSE;
	  design->fromHSE=FALSE;
	  design->fromER=TRUE;
	  design->fromTEL=FALSE;
	  design->fromG=FALSE;
	  design->fromRSG=FALSE;
	  design->status=process_command(SYNTHESIS,DOALL,design,NULL,TRUE,
					 design->si);
	  design->brk_exception=design->exception;
	}
	if (design->status & STOREDPRS) {
	  cs_design->events=new_events(cs_design->status,TRUE,
				       cs_design->nevents,
				       cs_design->nevents,cs_design->events);
	  new_design(cs_design,TRUE);
	} else {
	  free(cs_design);
	  return(FALSE);
	}
      } else {
	free(cs_design);
	return(FALSE);
      }
      if (!(design->verbose)) {
	sprintf(shellcommand,"rm %s.inp",design->filename);
	system(shellcommand);
	sprintf(shellcommand,"rm %s.out",design->filename);
	system(shellcommand);
	sprintf(shellcommand,"rm %s.rule",design->filename);
	system(shellcommand);
	sprintf(shellcommand,"rm %s.ord",design->filename);
	system(shellcommand);
	sprintf(shellcommand,"rm %s.merg",design->filename);
	system(shellcommand);
      }
    }
    design->status |= STOREDPRS;
    design->total_lits = total_lits;
    design->area = area;
  }
  return(TRUE);
}
