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
/* connect.c                                                                 */
/*****************************************************************************/

#include "connect.h"
#include "def.h"

int notreachable=(-1);

/*****************************************************************************/
/* Determine if one event is reachable from another.                         */
/*****************************************************************************/

int reachable(cycleADT ****cycles,int nevents,int ncycles,
	      int e,int i,int f,int j)
{
  int k,l,m;

  if ((cycles[e][f][i][j]->reachable == 1) ||
      (cycles[e][f][i][j]->reachable == notreachable))
    return(cycles[e][f][i][j]->reachable);
  if (cycles[e][f][i][j]->rule) {
    cycles[e][f][i][j]->reachable=1;
    return(cycles[e][f][i][j]->reachable);
  }
  if ((e==f) && (i==j)) {
    cycles[e][f][i][j]->reachable=notreachable;
    return(cycles[e][f][i][j]->reachable);
  }
  if (cycles[e][f][i][j]->reachable < notreachable) {
    /*printf("ERROR:  Cycle detected in acyclic constraint graph.\n");
    fprintf(lg,"ERROR:  Cycle detected in acyclic constraint graph.\n");
    printf("e = %d  f = %d\n",e,f); */
    return INFIN;
/*
    cycles[e][f][i][j]->reachable=1;
    return(cycles[e][f][i][j]->reachable);
*/
  }
  cycles[e][f][i][j]->reachable=notreachable-1;
  m=j-1;
  if (m<0) m=0;
  for (k=0;k<nevents;k++)
    for (l=m;l<=j;l++)
      if (cycles[k][f][l][j]->rule) {
	if (reachable(cycles,nevents,ncycles,e,i,k,l)==1) {
	  cycles[e][f][i][j]->reachable=1;
	  return(cycles[e][f][i][j]->reachable);
	} else if (reachable(cycles,nevents,ncycles,e,i,k,l)==INFIN)
	  return INFIN;
      }
  cycles[e][f][i][j]->reachable=notreachable;
  return(cycles[e][f][i][j]->reachable);
}

/*****************************************************************************/
/* Find position of event using event's name.                                */
/*****************************************************************************/

int which_event(int nevents,eventADT *events,char * event)
{
int a;

for (a=0;a<nevents;a++)
  if ((strcmp(events[a]->event,event)==0) &&
      (!events[a]->dropped)) return(a);
return(-1);
}

/*****************************************************************************/
/* Given an event, find the inverse of the event.                            */
/*****************************************************************************/

int find_inverse_event(eventADT *events,cycleADT ****cycles,int nevents,
		       int ncycles,int u,int *j,char * startstate)
{
int index,inv;
char * c;
char * s;

if (strchr(events[u]->event,'+') != NULL) {
  s=CopyString(events[u]->event);
  c=strchr(s,'+'); 
  *c='-';
  *j=0;
} else if (strchr(events[u]->event,'-') != NULL) {
  s=CopyString(events[u]->event);
  c=strchr(s,'-'); 
  *c='+';
  *j=1;
} else {
  printf("ERROR: No inverse found.\n");
  fprintf(lg,"ERROR: No inverse found.\n");
  return(-1);
}
inv=which_event(nevents,events,s);
if (inv==(-1)) {
  c=strchr(s,'/'); 
  c++;
  sprintf(c,"1");
  inv=which_event(nevents,events,s);
}
if (reachable(cycles,nevents,ncycles,u,0,inv,0)==1) *j=0;
else if (reachable(cycles,nevents,ncycles,inv,0,u,0)==1) {
  *j=1;
  if (strchr(events[u]->event,'/') != NULL) {
    c=strchr(s,'/'); 
    c++;
    index=atoi(c);
    index++;
    sprintf(c,"%d",index);
    inv=which_event(nevents,events,s);
    if (inv==(-1)) {
      sprintf(c,"1");
      inv=which_event(nevents,events,s);
    } else *j=0;
  }
}
else { 
  if (startstate==NULL) {
    printf("ERROR:  Need initial state information.\n");
    fprintf(lg,"ERROR:  Need initial state information.\n");
    *j=(-1); 
  } else {
    if (u % 2==1)
      if (startstate[events[u]->signal]=='0') *j=0;
      else {
	*j=1;
	if (strchr(events[u]->event,'/') != NULL) {
	  c=strchr(s,'/'); 
	  c++;
	  index=atoi(c);
	  index++;
	  sprintf(c,"%d",index);
	  inv=which_event(nevents,events,s);
	  if (inv==(-1)) {
	    sprintf(c,"1");
	    inv=which_event(nevents,events,s);
	  } else *j=0;
	}}
    else 
      if (startstate[events[u]->signal]=='1') *j=0;
      else {
	*j=1;
	if (strchr(events[u]->event,'/') != NULL) {
	  c=strchr(s,'/'); 
	  c++;
	  index=atoi(c);
	  index++;
	  sprintf(c,"%d",index);
	  inv=which_event(nevents,events,s);
	  if (inv==(-1)) {
	    sprintf(c,"1");
	    inv=which_event(nevents,events,s);
	  } else *j=0;
	}}
  }
}
free(s);
return(inv);
}

/*****************************************************************************/
/* Make graph strongly connected by adding persistence rules.                */
/*****************************************************************************/

void connectivity_check(FILE *fp,eventADT *events,ruleADT **rules,
			markkeyADT *markkey,cycleADT ****cycles,
			int nevents,int *nrules,int *nord,int ncycles,
			int u,int v,char * startstate,bool *rule_added,
			bool verbose)
{
int x,j,inv,eps;

inv=find_inverse_event(events,cycles,nevents,ncycles,u,&j,startstate);
if (j==(-1)) j=rules[u][v]->epsilon;
if ((rules[u][v]->epsilon < ncycles) && (j < ncycles) && (v != inv)) {
  if (reachable(cycles,nevents,ncycles,v,rules[u][v]->epsilon,inv,j) != 1) {
    eps=j; /*-rules[u][v]->epsilon;*/
    if (eps < 0) {
      printf("ERROR eps\n");
      fprintf(lg,"ERROR eps\n");
    }
    (*rule_added)=TRUE;
    if (verbose)
      fprintf(fp,"< %s,%s,%d,0 >\n",events[v]->event,
	      events[inv]->event,eps);
    rules[v][inv]->epsilon=eps;      
    rules[v][inv]->lower=0;      
    rules[v][inv]->upper=0;
    rules[v][inv]->ruletype=ORDERING;
    markkey[*nrules]->enabling=v;
    markkey[*nrules]->enabled=inv;
    markkey[*nrules]->epsilon=eps;
    (*nrules)++;
    (*nord)++;
    for (x=0;x<ncycles-eps;x++)
      cycles[v][inv][x][x+eps]->rule=TRUE;
    if ((eps==1) && (rules[0][inv]->ruletype==NORULE)) {
      if (verbose)
	fprintf(fp,"< %s,%s,0,0 >\n",events[0]->event,
		events[inv]->event);
      rules[0][inv]->epsilon=0;      
      rules[0][inv]->lower=0;      
      rules[0][inv]->upper=0;
      rules[0][inv]->ruletype=ORDERING;
      markkey[*nrules]->enabling=0;
      markkey[*nrules]->enabled=inv;
      markkey[*nrules]->epsilon=0;
      (*nrules)++;
      (*nord)++;
      cycles[0][inv][0][0]->rule=TRUE;
    }
    if (verbose) fflush(fp);
    notreachable--;
  }
}
}

/*****************************************************************************/
/* Check if graph is safe.                                                   */
/*****************************************************************************/

bool safe(char * filename,eventADT *events,ruleADT **rules,
	  cycleADT ****cycles,int nevents,int ncycles,bool verbose)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int i,j;
  bool okay;

  okay=TRUE;
  printf("Checking safety ... ");
  fprintf(lg,"Checking safety ... ");
  fflush(stdout);
  fflush(lg);
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if (rules[i][j]->epsilon==1) {
	if (reachable(cycles,nevents,ncycles,j,0,j,1) != 1) {
	  if (okay) {
	    printf("NOT safe\n");
	    fprintf(lg,"NOT safe\n");
	    if (verbose) {
	      strcpy(outname,filename);
	      strcat(outname,".err");
	      fp=Fopen(outname,"w");
	      printf("ERROR:  Storing safety violations to:  %s\n",
		     outname);
	      fprintf(lg,"ERROR:  Storing safety violations to:");
	      fprintf(lg,"  %s\n",outname);
	    } else {
	      printf("ERROR:  Safety violation!\n");
	      fprintf(lg,"ERROR:  Safety violation!\n");
	    }
	    okay=FALSE;
	  }
	  if (verbose)
	    fprintf(fp,"Marking for rule between %s and %s is not safe\n",
		    events[i]->event,events[j]->event);
	}
      }
  if (okay) {
    printf("safe\n");
    fprintf(lg,"safe\n");
  } else if (verbose) fclose(fp);
  return(okay);
}

/*****************************************************************************/
/* Check if graph is live.                                                   */
/*****************************************************************************/

bool live(char * filename,eventADT *events,ruleADT **rules,int nevents,
	  bool verbose)
{

  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int i,j,k;
  bool okay;
  bool fire,progress;

  okay=TRUE;
  printf("Checking liveness ... ");
  fprintf(lg,"Checking liveness ... ");
  fflush(stdout);
  fflush(lg);
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      rules[i][j]->reachable=FALSE;
  do {
    progress=FALSE;
    for (j=1;j<nevents;j++) {
      fire=FALSE;
      for (i=1;i<nevents;i++)
	if (rules[i][j]->ruletype)
	  fire=TRUE;
      for (i=1;i<nevents && fire;i++)
	if (rules[i][j]->ruletype)
	  if ((!rules[i][j]->reachable) && (rules[i][j]->epsilon==0)) {
	    fire=FALSE;
	    for (k=1;k<nevents;k++)
	      if ((rules[i][k]->conflict) && (rules[k][j]->ruletype) && 
		  ((rules[k][j]->reachable) || (rules[k][j]->epsilon==1)))
		fire=TRUE;
	  }
      if (fire) {
	for (i=1;i<nevents;i++)
	  if ((!rules[j][i]->reachable) && (rules[j][i]->ruletype)) {
	    progress=TRUE;
	    rules[j][i]->reachable=TRUE;
/*	    printf("%s -> %s\n",events[j]->event,events[i]->event); */
	  }
      }
    }
  } while (progress);
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((!rules[i][j]->reachable) && (rules[i][j]->ruletype)) {
	if (okay) {
	  printf("NOT live\n");
	  fprintf(lg,"NOT live\n");
	  if (verbose) {
	    strcpy(outname,filename);
	    strcat(outname,".err");
	    fp=Fopen(outname,"w");
	    printf("ERROR:  Storing liveness violations to:  %s\n",
		   outname);
	    fprintf(lg,"ERROR:  Storing liveness violations to:");
	    fprintf(lg,"  %s\n",outname);
	  } else {
	    printf("ERROR:  Liveness violation!\n");
	    fprintf(lg,"ERROR:  Liveness violation!\n");
	  }
	  okay=FALSE;
	}
	if (verbose)
	  if (rules[i][j]->ruletype < ORDERING) 
	    fprintf(fp,"Cannot reach rule between %s and %s\n",
		    events[i]->event,events[j]->event);
      }
  if (okay) {
    printf("live\n");
    fprintf(lg,"live\n");
  } else if (verbose) fclose(fp);
  return(okay);
}

/*****************************************************************************/
/* Check if graph is strongly connected.                                     */
/*****************************************************************************/

bool strongly_connected(char * filename,eventADT *events,ruleADT **rules,
			int nevents,bool verbose)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int i,j,k;
  bool okay;
  bool *drop;

  okay=TRUE;
  printf("Checking connectivity ... ");
  fprintf(lg,"Checking connectivity ... ");
  fflush(stdout);
  drop=(bool*)GetBlock(nevents * sizeof(bool));
  for (i=1;i<nevents;i++) drop[i]=TRUE;
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if (rules[i][j]->ruletype) {
	rules[i][j]->reachable=TRUE;
	drop[i]=FALSE;
	drop[j]=FALSE;
      } else rules[i][j]->reachable=FALSE;
  for (k=1;k<nevents;k++)
    for (i=1;i<nevents;i++)
      for (j=1;j<nevents;j++)
        if (!rules[i][j]->reachable) {
          if (rules[i][k]->reachable & rules[k][j]->reachable)
	    rules[i][j]->reachable=TRUE;
        if (k==(nevents-1))
          if (((!drop[i]) && (!drop[j])) && (!rules[i][j]->reachable)
	      && (!rules[j][i]->reachable)) {
	    if (okay) {
	      printf("NOT strongly connected\n");
	      fprintf(lg,"NOT strongly connected\n");
	      if (verbose) {
		strcpy(outname,filename);
		strcat(outname,".err");
		fp=Fopen(outname,"w");
		printf("EXCEPTION:  Storing connectivity violations to:  %s\n",
		       outname);
		fprintf(lg,"EXCEPTION:  Storing connectivity violations to:");
		fprintf(lg,"  %s\n",outname);
	      } else {
		printf("EXCEPTION:  Connectivity violation!\n");
		fprintf(lg,"EXCEPTION:  Connectivity violation!\n");
	      }
	      okay=FALSE;
	    }
	    if (verbose)
	      fprintf(fp,"No path from the event %s to the event %s\n",
		      events[i]->event,events[j]->event);
	  }
	}
  free(drop);
  if (okay) {
    printf("strongly connected\n");
    fprintf(lg,"strongly connected\n");
  } else if (verbose) fclose(fp);
  return(okay);
}

/*****************************************************************************/
/* Make graph strongly connected by adding persistence rules                 */
/*****************************************************************************/

bool connect_graph(char * filename,eventADT *events,ruleADT **rules,
		   markkeyADT *markkey,cycleADT ****cycles,
		   int nevents,int *nrules,int *nord,int ncycles,
		   char * startstate,bool verbose)
{
  int u,v;
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  bool rule_added;

  if (ncycles==0) {
    printf("ERROR:  Cannot connect graph with 0 cycles!\n");
    fprintf(lg,"ERROR:  Cannot connect graph with 0 cycles!\n");
    return(FALSE);
  }
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".or");
    fp=Fopen(outname,"w");
    printf("Finding ordering rules and storing to:  %s\n",outname);
    fprintf(lg,"Finding ordering rules and storing to:  %s\n",outname);
  } else {
    printf("Finding ordering rules ... ");
    fflush(stdout);
    fprintf(lg,"Finding ordering rules ... ");
    fflush(lg);
  }
  rule_added=FALSE;
  for (u=1;u<nevents;u++)
    for (v=1;v<nevents;v++)
      if (rules[u][v]->ruletype & TRIGGER)
	connectivity_check(fp,events,rules,markkey,cycles,nevents,
			   nrules,nord,ncycles,u,v,startstate,&rule_added,
			   verbose);
  if (!verbose) {
    printf("done\n");
    fprintf(lg,"done\n");
  }
  while ((rule_added) && (!strongly_connected(filename,events,rules,nevents,
					      verbose))){
    if (!verbose) {
      printf("Finding ordering rules ... ");
      fflush(stdout);
      fprintf(lg,"Finding ordering rules ... ");
      fflush(lg);
    }
    rule_added=FALSE;
    for (u=1;u<nevents;u++)
      for (v=1;v<nevents;v++)
	if (rules[u][v]->ruletype != NORULE) 
	  connectivity_check(fp,events,rules,markkey,cycles,nevents,
			     nrules,nord,ncycles,u,v,startstate,&rule_added,
			     verbose);
    if (!verbose) {
      printf("done\n");
      fprintf(lg,"done\n");
    }
  }
  if (!rule_added) {
    printf("ERROR:  Unable to make graph strongly connected.\n");
    fprintf(lg,"ERROR:  Unable to make graph strongly connected.\n");
    if (verbose) fclose(fp);
    return(FALSE);
  }
  if (verbose) fclose(fp);
  return(TRUE);
}

bool check_cycles(cycleADT ****cycles,int nevents,int ncycles)
{
  int i,j,k;

  printf("Checking for cycles in acyclic constraint graph ... ");
  fprintf(lg,"Checking for cycles in acyclic constraint graph ... ");
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      for (k=0;k<2;k++)
	if (reachable(cycles,nevents,ncycles,i,0,j,k)==INFIN) {
	  printf("ERROR!\n");
	  fprintf(lg,"ERROR!\n");
	  printf("ERROR:  Cycle detected in acyclic constraint graph.\n");
	 fprintf(lg,"ERROR:  Cycle detected in acyclic constraint graph.\n");
	  return FALSE;
	}
  printf("done.\n");
  fprintf(lg,"done.\n");
  return TRUE;
}

/*****************************************************************************/
/* Check if graph is strongly connected, and if not connect it.              */
/*****************************************************************************/

bool SConnect(char command,designADT design)
{
  bool verbose;

  verbose=design->verbose;
  if (command==CONNECT) verbose=TRUE;
  if (!(design->status & CONNECTED)) {
    if (!check_cycles(design->cycles,design->nevents,design->ncycles))
      return TRUE; /* TEMP */
    if (!live(design->filename,design->events,design->rules,
	      design->nevents,verbose))
      return(FALSE);
    if ((!(design->level)) && 
	!strongly_connected(design->filename,design->events,design->rules,
			    design->nevents,verbose)) {
      if (/*design->heuristic || design->untimed*/0)
	if ((command==CONNECT) || (design->exception) ) {
	  if (!connect_graph(design->filename,design->events,design->rules,
			     design->markkey,design->cycles,design->nevents,
			     &(design->nrules),&(design->nord),
			     design->ncycles,design->startstate,verbose)) {
	    return(FALSE);
	  }    
	} else
	  return(FALSE);
    } else
      design->status=design->status | CONNECTED;
    if (!design->fromVHDL)
      if (!safe(design->filename,design->events,design->rules,design->cycles,
		design->nevents,design->ncycles,verbose))
	return TRUE; /* TEMP */
  }
  return(TRUE);
}
