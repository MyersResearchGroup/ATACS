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
/* verify.c                                                                  */
/*****************************************************************************/

#include "verify.h"
#include "connect.h"
#include "findtd.h"
#include "storeprs.h"
#include "merges.h"
#include "loader.h"
#include "findrsg.h"
#include <map>
#include <set>
#include <string>
#include "hpnrsg.h"
#include "color.h"

using namespace std;

/*****************************************************************************/
/* Print header for verification file.                                       */
/*****************************************************************************/

void print_header(FILE *fp,char * filename)
{
  fprintf(fp,";\n");
  fprintf(fp,";  %s\n",filename);
  fprintf(fp,";\n");
#ifdef RS6000
  fprintf(fp,"(load \"%s/srcdir.scm\")\n",getenv("ORBITSDIR"));
  fprintf(fp,"(load \"%s/orbits.scm\")\n",getenv("ORBITSDIR"));
#endif
  fprintf(fp,"(define *require-all-muxes-for-violation?* #t)\n");
}

/*****************************************************************************/
/* Print a signal instance name.                                             */
/*****************************************************************************/

void print_signal_instance_name(FILE *fp,eventADT *events,int i)
{
  int j;

  for (j=0;
       (events[i]->event[j] != '\0');
       j++)
    if ((events[i]->event[j] != '+') && (events[i]->event[j] != '-'))
      fprintf(fp,"%c",events[i]->event[j]);
}

/*****************************************************************************/
/* Print an event instance name.                                             */
/*****************************************************************************/

void print_event_instance_name(FILE *fp,eventADT *events,int i)
{
  int j;
  char phase='+';

  for (j=0;
       (events[i]->event[j] != '\0');
       j++)
    if ((events[i]->event[j] != '+') && (events[i]->event[j] != '-'))
      fprintf(fp,"%c",events[i]->event[j]);
    else phase=events[i]->event[j];
  fprintf(fp,"%c",phase);
}

/*****************************************************************************/
/* Print an event name.                                                      */
/*****************************************************************************/

void print_event_name(FILE *fp,eventADT *events,int i)
{
  int j;

  for (j=0;
       (events[i]->event[j] != '+') && (events[i]->event[j] != '-');
       j++)
    fprintf(fp,"%c",events[i]->event[j]);
  fprintf(fp,"%c",events[i]->event[j]);
}

/*****************************************************************************/
/* Print initial state information for a merged signal.                      */
/*****************************************************************************/

void print_initial_merge(FILE *fp,eventADT *events,int i,char value)
{
  fprintf(fp," (");
  print_signal_name(fp,events,i);
  fprintf(fp," %c)",value);
}

/*****************************************************************************/
/* Determine if extra signal needed for type 2 to 1 conversion.              */
/*****************************************************************************/

bool need_expansion(ruleADT **rules,int nevents,int j)
{
  bool error;
  int i,e,l,u,count;

  e=0; l=0; u=0;
  error=FALSE;
  count=0;
  for (i=1;i<nevents;i++)
    if (rules[i][j]->ruletype & (TRIGGER | PERSISTENCE )) {
      count++;
      if (count==1) {
	e=rules[i][j]->epsilon;
	l=rules[i][j]->lower;
	u=rules[i][j]->upper;
      } 
      if ((rules[i][j]->lower != l) || (rules[i][j]->upper != u)) error=TRUE;
      if (rules[i][j]->epsilon != e) error=TRUE;
    }
/* TEMP */
  error=TRUE;
  return(error);
}

/*****************************************************************************/
/* Print initial state information.                                          */
/*****************************************************************************/

void print_initial_state(FILE *fp,eventADT *events,ruleADT **rules,
			 cycleADT ****cycles,int nevents,int ndummy,
			 int ncycles,char * startstate,bool internal)
{
  int i,j;
  char value='0';
  char * slash;

  for (i=1;i<nevents;i++) 
    if ((i % 2==1)||(i >=(nevents-ndummy))) {
      if (((slash=strchr(events[i]->event,'/'))==NULL) || (internal)) { 
	if (i != 1) fprintf(fp," ");
	fprintf(fp,"(");
	print_signal_instance_name(fp,events,i);
      }
      /*
      if (reachable(cycles,nevents,ncycles,i,0,i+1,0)==1)
	value='0';
      else if (reachable(cycles,nevents,ncycles,i+1,0,i,0)==1)
	value='1';
      else
      */
      if (startstate==NULL) {
	printf("ERROR:  Need initial state information.\n");
	fprintf(lg,"ERROR:  Need initial state information.\n");
      } else {
	if (events[i]->signal >= 0)
	  value=startstate[events[i]->signal];
	else
	  value='0';
      }
      if (value=='R') value='0';
      if (value=='F') value='1';
      if ((value!='0') && (value!='1')) {
	printf("ERROR: Could not determine initial value setting to 0.\n");
	fprintf(lg,"ERROR: Could not determine initial value setting to 0.\n");
	value='0';
      }
      if (((slash=strchr(events[i]->event,'/'))==NULL) || (internal)) {
	fprintf(fp," %c)",value);
	
	fprintf(fp,"(_");
	print_signal_instance_name(fp,events,i);
	fprintf(fp," %c)",value);
	
	fprintf(fp,"(");
	print_signal_instance_name(fp,events,i);
	fprintf(fp,"_");
	print_signal_instance_name(fp,events,i);
	fprintf(fp," %c)",value);

	if (i < (nevents-ndummy)) {
	  for (j=1;j<(nevents-ndummy);j+=2) {
	    if (((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE )) &&
		 (need_expansion(rules,nevents,j))) ||
		((rules[i][j+1]->ruletype & (TRIGGER | PERSISTENCE )) &&
		 (need_expansion(rules,nevents,j+1))) ||
		((rules[i+1][j]->ruletype & (TRIGGER | PERSISTENCE )) &&
		 (need_expansion(rules,nevents,j))) ||
		((rules[i+1][j+1]->ruletype & (TRIGGER | PERSISTENCE )) &&
		 (need_expansion(rules,nevents,j+1)))) {
	      fprintf(fp," (");
	      print_signal_instance_name(fp,events,i);
	      print_signal_instance_name(fp,events,j);
	      fprintf(fp," %c)",('1'-value)+'0');
	      if (i != j) {
		fprintf(fp," (");
		print_signal_instance_name(fp,events,i);
		fprintf(fp,"_");
		print_signal_instance_name(fp,events,j);
		fprintf(fp," %c)",('1'-value)+'0');
	      }
	    }
	  }    
	}
      }
      if (((slash=strchr(events[i]->event,'/')) != NULL) && 
	  (strcmp(slash,"/1")==0))
	print_initial_merge(fp,events,i,value);
    }
}

int eps_disjunct_rule(ruleADT **rules,int nevents,int i,int j)
{
  int k,eps;

  eps=rules[i][j]->epsilon;
  if (eps==0) return 0;
  for (k=1;k<nevents;k++)
    if ((rules[k][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	(rules[k][i]->conflict) && 
	(rules[k][j]->epsilon==0)) return 0;
  return eps;
}

bool disjunct_rule(ruleADT **rules,int nevents,int i,int j)
{
  int k;

  if (!(rules[i][j]->ruletype & (TRIGGER | PERSISTENCE))) return FALSE;
  for (k=1;k<nevents;k++)
    if ((rules[k][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	(rules[k][i]->conflict) /* && 
	(rules[i][j]->epsilon==rules[k][j]->epsilon) */) return TRUE;
  return FALSE;
}

bool disjunct_rule2(ruleADT **rules,int i,int j,int l)
{
  int k;

  if (!(rules[i][j]->ruletype & (TRIGGER | PERSISTENCE))) return FALSE;
  for (k=1;k<l;k++)
    if ((rules[k][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	(rules[k][i]->conflict) /* && 
	(rules[i][j]->epsilon==rules[k][j]->epsilon) */) return TRUE;
  return FALSE;
}

bool disjunct_rule3(ruleADT **rules,int i,int j,int l)
{
  int k;

  if (!(rules[i][j]->ruletype & (TRIGGER | PERSISTENCE))) return FALSE;
  for (k=1;k<l;k++)
    if ((rules[i][k]->ruletype & (TRIGGER | PERSISTENCE)) &&
	(rules[k][j]->conflict) && 
	(rules[i][j]->epsilon==rules[i][k]->epsilon)) return TRUE;
  return FALSE;
}

void print_enabling_condition(FILE *fp,eventADT *events,ruleADT **rules,
			      int nevents,int j,int f,int count,int disjcount)
{
  int i;

  if (count==1) 
    print_event_instance_name(fp,events,f);
  else if (disjcount==0) {
    fprintf(fp,"(burst");
    for (i=1;i<nevents;i++)
      if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	  (!disjunct_rule(rules,nevents,i,j))) {
	fprintf(fp," ");
	print_event_instance_name(fp,events,i);
      } 
    fprintf(fp,")");
  } else if (count==disjcount) {
    fprintf(fp,"(choice");
    for (i=1;i<nevents;i++) {
/*      if (disjunct_rule(rules,nevents,i,j)) { */
      if (disjunct_rule(rules,nevents,i,j) &&
	  ((i==f) || (rules[i][f]->conflict))) {
	fprintf(fp," (");
	print_event_instance_name(fp,events,i);
	fprintf(fp,")");
      } 
    }
    fprintf(fp,")");
  } else {
    printf("count = %d disjcount = %d j = %s f = %s\n",count,disjcount,
	   events[j]->event,events[f]->event);
    printf("DO NOT KNOW WHAT TO DO???\n");
  }
}

void print_enabling_condition2(FILE *fp,eventADT *events,ruleADT **rules,
			       int nevents,int i,int j,int disjcount)
{
  int e;

  if ((disjcount > 1) && (disjunct_rule(rules,nevents,i,j))) {
    fprintf(fp,"(choice");
    for (e=1;e<nevents;e++)
/*      if (disjunct_rule(rules,nevents,e,j)) { */
      if (disjunct_rule(rules,nevents,e,j) &&
	  ((i==e) || (rules[i][e]->conflict))) {
	fprintf(fp," (");
	print_event_instance_name(fp,events,e);
	fprintf(fp,")");
      } 
    fprintf(fp,")");
  } else print_event_instance_name(fp,events,i);
}

void print_orbits_bound(FILE *fp,int lower,int upper,bool si)
{
  if (!si) {
    fprintf(fp,":%d,",lower);
    printdelay(fp,upper);
    fprintf(fp,": ");
  } 
}

void print_enabled_event(FILE *fp,eventADT *events,mergeADT *merges,int j,
			 bool noconf,bool *conf,int nevents)
{
  int k;
  mergeADT curcon;
  bool amerge,first;
  
  first=TRUE;
  if (!noconf) {
    fprintf(fp,"(choice");
    for (k=1;k<nevents;k++)
      if (conf[k]) {
	amerge=FALSE;
	for (curcon=merges[k]; curcon != NULL; curcon=curcon->link)
	  if (((curcon->mergetype==CONJMERGE) || 
	       (curcon->mergetype==DISJMERGE)) && (curcon->mevent==j))
	    amerge=TRUE;
	if (first) {
	  fprintf(fp," (");
	  print_event_instance_name(fp,events,j);
	  fprintf(fp,")");
	  first=FALSE;
	}
	fprintf(fp," (");
	print_event_instance_name(fp,events,k);
	fprintf(fp,")");
      }
    fprintf(fp,")");
  } else print_event_instance_name(fp,events,j);
}

void print_fast_event(FILE *fp,eventADT *events,int i,int j,bool printX)
{
  print_signal_instance_name(fp,events,i);
  if (printX) fprintf(fp,"_");
  print_signal_instance_name(fp,events,j);
  if (strchr(events[i]->event,'+') != NULL)
    fprintf(fp,"+");
  else fprintf(fp,"-");
}

/*****************************************************************************/
/* Count fanin.                                                              */
/*****************************************************************************/

int countfanin(ruleADT **rules,int j,int nevents,bool problem,bool indep,
	       bool *disj,bool *ind)
{
  int i,count;

  count=0;
  for (i=1;i<nevents;i++)
    if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	(!disjunct_rule2(rules,i,j,i)))
      if ((!problem) || (indep && ind[i]) || 
	  (!indep && !ind[i])) count++;
  return(count);
/*
  count=0;
  disjprinted=FALSE;
  for (i=1;i<nevents;i++)
    if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	((!disjprinted) || (!disjunct_rule(rules,nevents,i,j)))) 
      if ((!problem) || (indep && ind[i]) || 
	  (!indep && !ind[i] && (disj[i] || !disjprinted))) {
	count++;
	if (disjunct_rule(rules,nevents,i,j)) disjprinted=TRUE;
      }
  return(count);
*/
}

/*****************************************************************************/
/* Print all possible orderings for type 2 to 1 conversion.                  */
/*****************************************************************************/

void print_trace(FILE *fp,eventADT *events,ruleADT **rules,
		 int nevents,bool *fanin,int oldj,int j,int remain,int max,
		 bool noconf,bool *conf,bool fixed,bool si,
		 bool *disj,bool *ind,bool indep,bool problem,int nconf)
{
  int i,k,count,oldmax,oldrem;
  bool nextfixed;
  bool useoldj;
  bool printX;
  bool choice;
  bool endit;
  bool changed;
  bool freechoice;

  oldmax=0;
  oldrem=0;
  if (!indep && problem)  
    printX=TRUE;
  else printX=FALSE;
  count=0;
  choice=TRUE;
  endit=FALSE;
  changed=FALSE;
  for (i=1;i<nevents;i++)
    if ((fanin[i]) && (!problem || (indep && ind[i]) || (!indep && !ind[i])
		       || (i==j))) 
      if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) ||
	  (!noconf && !fixed) || (i==j)) {
	if ((!disj[i]) ||
	    (rules[i][j]->ruletype & (TRIGGER | PERSISTENCE))) 
	  count++;
	else endit=TRUE;
	nextfixed=fixed;
	if (disj[i]) {
	  if (!(rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)))
	    for (k=1;k<nevents;k++) 
	      if ((conf[k]) && (rules[i][k]->ruletype & 
				(TRIGGER | PERSISTENCE))) {
		changed=TRUE;
		j=k;
		oldmax=max;
		oldrem=remain;
		max=countfanin(rules,k,nevents,problem,indep,disj,ind)+nconf;
		remain=remain+(max-oldmax);
/*		printf("oldj=%d j=%d oldmax=%d max=%d oldrem=%d remain=%d\n",
		       oldj,j,oldmax,max,oldrem,remain); */
		break;
	      }
	  nextfixed=TRUE;
	}
	fanin[i]=FALSE;
	if (((remain > 1) && (!fixed || noconf)) || (remain > (nconf+1))) {
	  fprintf(fp,"\n\t\t");
	  for (k=remain;k<max;k++) fprintf(fp,"\t");
	  if (choice) {
	    fprintf(fp,"(choice (");
	    choice=FALSE;
	  } else fprintf(fp,"\t(");
	}
	useoldj=FALSE;
	for (k=1;k<nevents;k++)
	  if ((conf[k]) && (rules[i][k]->ruletype & 
			    (TRIGGER | PERSISTENCE))
	      && (rules[i][oldj]->ruletype & 
		  (TRIGGER | PERSISTENCE)))
	    useoldj=TRUE;
	if (i==j) print_fast_event(fp,events,i,j,TRUE);
	else if (useoldj) print_fast_event(fp,events,i,oldj,printX);
	else print_fast_event(fp,events,i,j,printX);
	fprintf(fp," ");
	print_trace(fp,events,rules,nevents,fanin,oldj,j,remain-1,
		    max,noconf,conf,nextfixed,si,disj,ind,indep,problem,nconf);
	if (changed) {
	  j=oldj;
	  remain=oldrem;
	  max=oldmax;
	}
	fanin[i]=TRUE;
      }
/*
      else { 
	remain--; 
      }
*/
  if ((((remain-nconf) > 1) || (endit))
      && (count==(remain-nconf))) fprintf(fp,"))");
  if (count==0) { 

    
/* Hack to get environment choice with multiple behavior places to work */
    freechoice=FALSE;
    for (k=0;k<nevents;k++) 
      if (conf[k]) {
	freechoice=TRUE;
	for (i=0;i<nevents;i++)
	  if (disj[i] || ind[i]) freechoice=FALSE;
	if (!freechoice) break;
      } 
    if (freechoice) {
      if (si) fprintf(fp," "); else fprintf(fp,":0,0: (choice (");
      if (printX) fprintf(fp,"_");
      print_event_instance_name(fp,events,j);
      fprintf(fp,")");
      for (k=0;k<nevents;k++) 
	if (conf[k]) {
	  fprintf(fp,"(");
	  if (printX) fprintf(fp,"_");
	  print_event_instance_name(fp,events,k);
	  fprintf(fp,")");
	}
      fprintf(fp,"))))");
    } else {
      if (si) fprintf(fp," "); else fprintf(fp,":: ");
      if (printX) fprintf(fp,"_");
      print_event_instance_name(fp,events,j);
      fprintf(fp,")");
    }
  }
}

/*****************************************************************************/
/* Do type 1 to 2 conversion.                                                */
/*****************************************************************************/

int dotype2conv(FILE *fp,eventADT *events,ruleADT **rules,int j,int nevents,
		bool si,bool* fanin,bool printdisj,int count,bool *disj,
		bool *ind,bool indep,bool problem,int disjcount)
{
  int i;
  bool printX;

  if (!indep && problem)  
    printX=TRUE;
  else printX=FALSE;
  for (i=1;i<nevents;i++) 
    if (rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) {
/*      printf("%s -> %s\n",events[i]->event,events[j]->event); */
      if (!disjunct_rule2(rules,i,j,i)) {
/*	printf("not disj rule2\n"); */
	if (!disjunct_rule3(rules,i,j,j)) {
/*	  printf("not disj rule3\n");
	  printf("%s -> %s\n",events[i]->event,events[j]->event);
	  printf("problem=%d indep=%d ind[i]=%d disj[i]=%d printdisj=%d\n",
		 problem,indep,ind[i],disj[i],printdisj); */
	  if ((!problem) || (indep && ind[i]) || 
	      (!indep && !ind[i] && (disj[i] || !printdisj))) {
	    fanin[i]=TRUE;
	    count++;
	    fprintf(fp,"\t(process ");
	    if (eps_disjunct_rule(rules,nevents,i,j)==1) {
	      print_orbits_bound(fp,rules[i][j]->lower,rules[i][j]->upper,si);
	      print_fast_event(fp,events,i,j,printX);
	      if (si) fprintf(fp," :: "); else fprintf(fp," ");
	      print_enabling_condition2(fp,events,rules,nevents,i,j,disjcount);
	    } else {
	      print_enabling_condition2(fp,events,rules,nevents,i,j,disjcount);
	      if (si) fprintf(fp," :: "); else fprintf(fp," ");
	      print_orbits_bound(fp,rules[i][j]->lower,rules[i][j]->upper,si);
	      print_fast_event(fp,events,i,j,printX);
	    }
	    fprintf(fp,")\n");
	  } 
	}
      }
    }
  return(count);
/*
  bool disjprinted;

  if (!indep && problem)  
    printX=TRUE;
  else printX=FALSE;
  disjprinted=printdisj;
  for (i=1;i<nevents;i++)
    if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	((!disjprinted) || (!disjunct_rule(rules,nevents,i,j)))) 
      if ((!problem) || (indep && ind[i]) || 
	  (!indep && !ind[i] && (disj[i] || !printdisj))) {
	fanin[i]=TRUE;
	count++;
	fprintf(fp,"\t(process ");
	if (eps_disjunct_rule(rules,nevents,i,j)==1) {
	  print_orbits_bound(fp,rules[i][j]->lower,rules[i][j]->upper,si);
	  print_fast_event(fp,events,i,j,printX);
	  if (si) fprintf(fp," :: "); else fprintf(fp," ");
	  print_enabling_condition2(fp,events,rules,nevents,i,j,disjcount);
	} else {
	  print_enabling_condition2(fp,events,rules,nevents,i,j,disjcount);
	  if (si) fprintf(fp," :: "); else fprintf(fp," ");
	  print_orbits_bound(fp,rules[i][j]->lower,rules[i][j]->upper,si);
	  print_fast_event(fp,events,i,j,printX);
	}
	if (disjunct_rule(rules,nevents,i,j)) disjprinted=TRUE;
	fprintf(fp,")\n");
      }
  return(count);
*/
}

bool analyze_graph(ruleADT **rules,int nevents,bool *fanin,bool *disj,
		   bool *ind,int *nconf,bool noconf,bool *conf,int j,
		   eventADT *events)
{
  int i,k,l;
  bool problem;
  bool indep;
  bool disjoint;

  for (i=0;i<nevents;i++) {
    fanin[i]=FALSE;
    disj[i]=FALSE;
    ind[i]=FALSE;
  }
  problem=FALSE;
  (*nconf)=0;
  if (!noconf)
    for (k=1;k<nevents;k++) 
      if (conf[k]) {
/*	printf("checking %s and %s\n",events[j]->event,events[k]->event); */
	(*nconf)++;
	indep=FALSE;
	disjoint=FALSE;
	for (i=1;i<nevents;i++) 
	  if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	      (!(rules[i][k]->ruletype & (TRIGGER | PERSISTENCE)))) {

	    for (l=1;l<nevents;l++)
	      if ((rules[l][k]->ruletype & (TRIGGER | PERSISTENCE)) &&
		  (rules[l][i]->conflict)) {
/*		  printf("disjoint %s and %s\n",events[i]->event,
			 events[l]->event); */
		  disj[i]=TRUE;
		}

	    if (disj[i]) disjoint=TRUE;
	    if (!disj[i]) {
/*	      printf("independent %s\n",events[i]->event); */
	      ind[i]=TRUE;
	      indep=TRUE;
	    }
	  } else if ((!(rules[i][j]->ruletype &	(TRIGGER | PERSISTENCE))) &&
		      ((rules[i][k]->ruletype & (TRIGGER | PERSISTENCE)))) {

	    for (l=1;l<nevents;l++)
	      if ((rules[l][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
		  (rules[l][i]->conflict)) {
/*		  printf("disjoint %s and %s\n",events[i]->event,
			 events[l]->event); */
		  disj[i]=TRUE;
		}

	    if (disj[i]) disjoint=TRUE;
	    if (!disj[i]) {
/*	      printf("independent %s\n",events[i]->event); */
	      ind[i]=TRUE;
	      indep=TRUE;
	    }
	  }
	if ((indep) && (disjoint)) {
/*	  printf("problem %s and %s\n",events[j]->event,events[k]->event); */
	  problem=TRUE;
	}
      }
  return problem;
}


/*****************************************************************************/
/* Do type 1 to 2 conversion.                                                */
/*****************************************************************************/

void type2conv(FILE *fp,eventADT *events,ruleADT **rules,int j,int nevents,
	       bool si,bool noconf,bool *conf,int disjcount)
{
  bool *fanin;
  bool *disj;
  bool *ind;
  int k,count,nconf;
  bool problem;

/*  printf("doing type2conv for %s\n",events[j]->event); */
  fanin=(bool *)GetBlock((nevents+2)*sizeof(int));
  disj=(bool *)GetBlock((nevents+2)*sizeof(int));
  ind=(bool *)GetBlock((nevents+2)*sizeof(int));
  problem=analyze_graph(rules,nevents,fanin,disj,ind,&nconf,noconf,conf,j,
			events);
  count=0;
  count=dotype2conv(fp,events,rules,j,nevents,si,fanin,FALSE,0,disj,ind,FALSE,
		    problem,disjcount);

/*  printf("count=%d\n",count); */

  for (k=0;k<nevents;k++)
    if (conf[k])
      count=dotype2conv(fp,events,rules,k,nevents,si,fanin,TRUE,count,disj,ind,
			FALSE,problem,disjcount);
  fprintf(fp,"\t(process ");
  count=countfanin(rules,j,nevents,problem,FALSE,disj,ind)+nconf;
  print_trace(fp,events,rules,nevents,fanin,j,j,count,count,noconf,conf,
	      FALSE,si,disj,ind,FALSE,problem,nconf);
  fprintf(fp,"\n");
  if (problem) {
    count=0;
    count=dotype2conv(fp,events,rules,j,nevents,si,fanin,TRUE,0,disj,ind,TRUE,
		      problem,disjcount);
    
    fprintf(fp,"\t(process _");
    print_event_instance_name(fp,events,j);
    if (si) fprintf(fp," :: "); else fprintf(fp," ");
    print_orbits_bound(fp,0,0,si);
    print_fast_event(fp,events,j,j,TRUE);
    fprintf(fp,")\n");

    fanin[j]=TRUE;
    fprintf(fp,"\t(process ");
    count=countfanin(rules,j,nevents,problem,TRUE,disj,ind);
    count++;
    print_trace(fp,events,rules,nevents,fanin,j,j,count,count,TRUE,conf,
		FALSE,si,disj,ind,TRUE,problem,0);
    fanin[j]=FALSE;
    fprintf(fp,"\n");
    for (k=0;k<nevents;k++)
      if (conf[k]) {
	count=0;
	count=dotype2conv(fp,events,rules,k,nevents,si,fanin,TRUE,0,disj,ind,
			  TRUE,problem,disjcount);

	fprintf(fp,"\t(process _");
	print_event_instance_name(fp,events,k);
	if (si) fprintf(fp," :: "); else fprintf(fp," ");
	print_orbits_bound(fp,0,0,si);
	print_fast_event(fp,events,k,k,TRUE);
	fprintf(fp,")\n");

	fanin[k]=TRUE;
	fprintf(fp,"\t(process ");
	count=countfanin(rules,k,nevents,problem,TRUE,disj,ind);
	count++;
	print_trace(fp,events,rules,nevents,fanin,k,k,count,count,TRUE,conf,
		    FALSE,si,disj,ind,TRUE,problem,0);
	fprintf(fp,"\n");
	fanin[k]=FALSE;
      }
  }
  free(fanin);
  free(disj);
  free(ind);
}

/*****************************************************************************/
/* Print an input process.                                                   */
/*****************************************************************************/

void print_process(FILE *fp,eventADT *events,ruleADT **rules,mergeADT *merges,
		   int j,int nevents,bool si)
{
  int i,k,m;
  int de,f,e,l,u,count,disjcount,eps;
  bool error;
  bool noconf;
  bool *conf;
  bool countit;
  bool *fanin;
  bool *disj;
  bool *ind;
  int nconf;

  m=0;
  count=0;  disjcount=0; l=(-1);  u=(-1); e=(-1); f=(-1);
  error=FALSE;
  conf=(bool *)GetBlock(nevents*sizeof(bool));
  
  noconf=TRUE;
  for (i=0;i<nevents;i++) conf[i]=FALSE;

  for (k=1;k<nevents;k++)
    if (rules[j][k]->conflict) 
      for (i=1;i<nevents;i++)
	if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE )) &&
	    (rules[i][k]->ruletype & (TRIGGER | PERSISTENCE)) &&
	    (rules[i][j]->epsilon==rules[i][k]->epsilon)) 
	  if (k < j) { 
	    free(conf); 
	    return; 
	  } else { 
	    conf[k]=TRUE;
	    noconf=FALSE;
	  }
  
  for (i=1;i<nevents;i++) {
    countit=FALSE;
    if (rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) {
      m=j;
      countit=TRUE;
    } if (!countit)
      for (k=1;k<nevents;k++)
	if ((conf[k]) && 
	    (rules[i][k]->ruletype & (TRIGGER | PERSISTENCE))) 
	  if (!countit) {
	    error=TRUE;
	    m=k;
	    countit=TRUE;
	  }
    if (countit) {
      count++;
      if (disjunct_rule(rules,nevents,i,j)) {
	if ((disjcount > 0) && (!disjunct_rule2(rules,i,j,i))) {
	  error=TRUE;
	}
	disjcount++;
	if (disjcount==1) f=i;
	de=eps_disjunct_rule(rules,nevents,i,j);
      }
      if (count==1) {
	f=i; 
	e=rules[i][m]->epsilon;
	eps=e;
	l=rules[i][m]->lower;
	u=rules[i][m]->upper;
      } 
      error=TRUE; /* ADDED TO REMOVE BURSTS */
      if ((rules[i][m]->lower != l) || (rules[i][m]->upper != u)) {
	error=TRUE;
      }
      if ((rules[i][m]->epsilon != e) && (!rules[i][f]->conflict)) {
	error=TRUE;
      }
    }
  }
  if ((!si) && (((count > 1) && (error)) || 
	       ((disjcount > 1) && (disjcount != count)))) {
    type2conv(fp,events,rules,j,nevents,si,noconf,conf,disjcount); 
    free(conf);
    return;
  }
  if (f==(-1)) return;
  if (!si) {
    fprintf(fp,"\t(process ");
    if (eps_disjunct_rule(rules,nevents,f,j)==1) {
      print_orbits_bound(fp,rules[f][j]->lower,rules[f][j]->upper,si);
      print_enabled_event(fp,events,merges,j,noconf,conf,nevents);
      fprintf(fp," ");
      print_enabling_condition(fp,events,rules,nevents,j,f,count,disjcount);
    } else {
      print_enabling_condition(fp,events,rules,nevents,j,f,count,disjcount);
      fprintf(fp," ");
      print_orbits_bound(fp,rules[f][j]->lower,rules[f][j]->upper,si);
      print_enabled_event(fp,events,merges,j,noconf,conf,nevents);
    }
    fprintf(fp,")\n");
  } else {
    if (disjcount > 1) {
      fprintf(fp,"\t(process ");
      if (eps_disjunct_rule(rules,nevents,f,j)==1) {
	print_orbits_bound(fp,rules[f][j]->lower,rules[f][j]->upper,si);
	print_enabled_event(fp,events,merges,j,noconf,conf,nevents);
	fprintf(fp," ");
	print_enabling_condition(fp,events,rules,nevents,j,f,disjcount,
				 disjcount);
      } else {
	print_enabling_condition(fp,events,rules,nevents,j,f,disjcount,
				 disjcount);
	fprintf(fp," ");
	print_orbits_bound(fp,rules[f][j]->lower,rules[f][j]->upper,si);
	print_enabled_event(fp,events,merges,j,noconf,conf,nevents);
      }
      fprintf(fp,")\n");
    }
    fanin=(bool *)GetBlock((nevents+2)*sizeof(int));
    disj=(bool *)GetBlock((nevents+2)*sizeof(int));
    ind=(bool *)GetBlock((nevents+2)*sizeof(int));
    analyze_graph(rules,nevents,fanin,disj,ind,&nconf,noconf,conf,j,events);
    for (i=1;i<nevents;i++)
      if ((rules[i][j]->ruletype & (TRIGGER | PERSISTENCE)) &&
	  (!disjunct_rule(rules,nevents,i,j))) {
	fprintf(fp,"\t(process ");
	if (eps_disjunct_rule(rules,nevents,i,j)==1) {
	  print_orbits_bound(fp,rules[i][j]->lower,rules[i][j]->upper,si);
	  if (!disj[i] && !ind[i])
	    print_enabled_event(fp,events,merges,j,noconf,conf,nevents);
	  else
	    print_enabled_event(fp,events,merges,j,TRUE,conf,nevents);
	  fprintf(fp," ");
	  print_enabling_condition(fp,events,rules,nevents,j,i,1,0);
	} else {
	  print_enabling_condition(fp,events,rules,nevents,j,i,1,0);
	  fprintf(fp," ");
	  print_orbits_bound(fp,rules[i][j]->lower,rules[i][j]->upper,si);
	  if (!disj[i] && !ind[i])
	    print_enabled_event(fp,events,merges,j,noconf,conf,nevents);
	  else
	    print_enabled_event(fp,events,merges,j,TRUE,conf,nevents);
	}
	fprintf(fp,")\n");
      }
    if (!noconf)
      for (k=1;k<nevents;k++)
	if (conf[k]) {
	  for (i=1;i<nevents;i++)
	    if ((rules[i][k]->ruletype & (TRIGGER | PERSISTENCE)) &&
		(!disjunct_rule(rules,nevents,i,k)) && (disj[i] || ind[i])) {
	      fprintf(fp,"\t(process ");
	      if (eps_disjunct_rule(rules,nevents,i,k)==1) {
		print_orbits_bound(fp,rules[i][k]->lower,
				   rules[i][k]->upper,si);
		print_enabled_event(fp,events,merges,k,TRUE,conf,nevents);
		fprintf(fp," ");
		print_enabling_condition(fp,events,rules,nevents,k,i,1,0);
	      } else {
		print_enabling_condition(fp,events,rules,nevents,k,i,1,0);
		fprintf(fp," ");
		print_orbits_bound(fp,rules[i][k]->lower,
				   rules[i][k]->upper,si);
		print_enabled_event(fp,events,merges,k,TRUE,conf,nevents);
	      }
	      fprintf(fp,")\n");
	    } 
	}
    free(fanin);
    free(disj);
    free(ind);
  }
  free(conf);
}

/*****************************************************************************/
/* Print list of signals.                                                    */
/*****************************************************************************/

void print_signal_list(FILE *fp,signalADT *signals,eventADT *events,
		       int ninpsig,int nsignals,int ninputs,int nevents,
		       char apos)
{
  int i;
  //char * slash;
  bool first;

  fprintf(fp,"(");
  first=TRUE;
  for (i=0;i<ninpsig;i++) {
    if (!first) fprintf(fp," "); else first=FALSE;
    fprintf(fp,"%s",signals[i]->name);
  }
  fprintf(fp,") %c(",apos);
  first=TRUE;
  for (i=ninpsig;i<nsignals;i++) {
    if (!first) fprintf(fp," "); else first=FALSE;
    fprintf(fp,"%s",signals[i]->name);
  }
  fprintf(fp,"))\n");
  /*
  fprintf(fp,"(");
  first=TRUE;
  for (i=1;i<ninputs;i+=2)
    if (((slash=strchr(events[i]->event,'/')) == NULL) ||
	(strcmp(slash,"/1")==0)) {
      if (!first) fprintf(fp," "); else first=FALSE;
      print_signal_name(fp,events,i);
    }
  fprintf(fp,") %c(",apos);
  first=TRUE;
  for (i=ninputs+1;i<nevents;i+=2)
    if (((slash=strchr(events[i]->event,'/')) == NULL) ||
	(strcmp(slash,"/1")==0)) {
      if (!first) fprintf(fp," "); else first=FALSE;
      print_signal_name(fp,events,i);
    }
  fprintf(fp,"))\n");
  */
}

void print_signal_instance_list(FILE *fp,eventADT *events,int ninputs,
				int nevents,char apos)
{
  int i;

  fprintf(fp,"(");
  for (i=1;i<ninputs;i+=2) {
    print_signal_instance_name(fp,events,i);
    if (i+2 < ninputs) fprintf(fp," ");
  }
  fprintf(fp,") %c(",apos);
  for (i=ninputs+1;i<nevents;i+=2) {
    print_signal_instance_name(fp,events,i);
    if (i+2 < nevents) fprintf(fp," ");
  }
  fprintf(fp,"))\n");
}

/*****************************************************************************/
/* Print process to merge signals.                                           */
/*****************************************************************************/

void print_merge_process(FILE *fp,eventADT *events,mergeADT *merges,
			 int nevents,int i)
{
  int j;
  bool merge;
  mergeADT curmerge;

  merge=FALSE;
  for (curmerge=merges[i]; curmerge != NULL; curmerge=curmerge->link)
    if ((curmerge->mergetype==CONJMERGE) || (curmerge->mergetype==DISJMERGE))
      merge=TRUE;
  
  fprintf(fp,"\t(process\n");
  fprintf(fp,"\t\t(choice\n");
  fprintf(fp,"\t\t\t(");
  print_event_instance_name(fp,events,i);
  fprintf(fp," :: ");
  print_event_name(fp,events,i);
  fprintf(fp,")\n");
  for (j=i+2;j<nevents;j+=2)
    if ((are_merged(merges,i,j)) && (!events[j]->dropped)) {
      fprintf(fp,"\t\t\t(");
      print_event_instance_name(fp,events,j);
      fprintf(fp," :: ");
      print_event_name(fp,events,j);
      fprintf(fp,")\n");
    }
  fprintf(fp,"\t\t))\n");
}

/*****************************************************************************/
/* Print the specification.                                                  */
/*****************************************************************************/

void print_specification(FILE *fp,signalADT *signals,eventADT *events,
			 mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
			 char * filename,int ninpsig,int nsignals,int ninputs,
			 int nevents,int ndummy,int ncycles,
			 char * startstate,bool si,bool instances)
{
  int i,j;
  char * slash;

  fprintf(fp,";\n");
  fprintf(fp,";  The specification for %s\n",filename);
  fprintf(fp,";\n");
  fprintf(fp,"(def (%s-spec ",filename);
  if (instances) print_signal_instance_list(fp,events,ninputs,nevents,' ');
  else print_signal_list(fp,signals,events,ninpsig,nsignals,ninputs,
			 nevents,' ');
  fprintf(fp,"(wire-init-set! ");
  print_initial_state(fp,events,rules,cycles,nevents,ndummy,ncycles,
		      startstate,TRUE);
  fprintf(fp,")\n");
  fprintf(fp,"(par\n");
  for (j=1;j<=ninputs;j++)
    print_process(fp,events,rules,merges,j,nevents,si);
  for (j=ninputs+1;j<nevents;j++)
    print_process(fp,events,rules,merges,j,nevents,TRUE);
  for (i=1;i<nevents;i++)
    if (((slash=strchr(events[i]->event,'/')) != NULL) && 
	(strcmp(slash,"/1")==0))
      print_merge_process(fp,events,merges,nevents,i);
  fprintf(fp,"))\n");
}

/*****************************************************************************/
/* Print a guard.                                                            */
/*****************************************************************************/

void print_vguard(FILE *fp,signalADT *signals,char * cover,int nsignals)
{
  int j;
  bool first;
  int nliterals;

  nliterals=0;
  for (j=0;j<nsignals;j++)
    if ((cover[j]=='0') || (cover[j]=='1')) 
      nliterals++;
  if (nliterals > 1)
    for (j=1;j<nliterals;j++)
      fprintf(fp,"(and ");
  first=TRUE;
  for (j=0;j<nsignals;j++)
    if (cover[j]=='1') {
      if (!first) fprintf(fp," ");
      fprintf(fp,"%s",signals[j]->name);
      if (!first) fprintf(fp,")");
      first=FALSE;
    } else if (cover[j]=='0') { 
      if (!first) fprintf(fp," ");
      fprintf(fp,"(not %s)",signals[j]->name);
      if (!first) fprintf(fp,")");
      first=FALSE;
    }
}

/*****************************************************************************/
/* Print all stacks.                                                         */
/*****************************************************************************/

void print_stacks(FILE *fp,signalADT *signals,regionADT region,int nsignals)
{
  regionADT cur_region;
  bool first;
  int k,nstacks;

  nstacks=0;
  for (cur_region=region;cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') nstacks++;
  first=TRUE;
  if (nstacks > 1) {
    fprintf(fp,"\t");
    for (k=1;k<nstacks;k++)
      fprintf(fp,"(or ");
    fprintf(fp,"\n");
  }
  for (cur_region=region;cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') {
      fprintf(fp,"\t");
      if (nstacks > 1) fprintf(fp,"\t");
      print_vguard(fp,signals,cur_region->cover,nsignals);
      if (!first) fprintf(fp,")");
      fprintf(fp,"\n");
      first=FALSE;
    }
}

/*****************************************************************************/
/* Print a generalized C-element.                                            */
/*****************************************************************************/

void print_gCs(FILE *fp,signalADT *signals,regionADT *regions,
	       int ninpsig,int nsignals)
{
  int i,j;
  regionADT cur_region;
  int sands,rands;

  for (i=ninpsig;i<nsignals;i++) {
    sands=0;
    for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0]!='E') sands++;
    rands=0;
    for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link) 
      if (cur_region->cover[0]!='E') rands++;
    if ((sands > 0) && (rands > 0))
      fprintf(fp,"(genc-ckt (fast-%s-c (",signals[i]->name);
    else 
      fprintf(fp,"(sm-ckt (fast-%s-c (",signals[i]->name);
    for (j=0;j<nsignals;j++) {
      cur_region=NULL;
      if (sands > 0)
	for (cur_region=regions[2*i+1]; cur_region; 
	     cur_region=cur_region->link)
	  if ((cur_region->cover[j]=='0') || (cur_region->cover[j]=='1')) {
	    fprintf(fp," %s",signals[j]->name);
	    break;
	  }
      if ((!cur_region) && (rands > 0))
	for (cur_region=regions[2*i+2]; cur_region; 
	     cur_region=cur_region->link)
	  if ((cur_region->cover[j]=='0') || (cur_region->cover[j]=='1')) {
	    fprintf(fp," %s",signals[j]->name);
	    break;
	  }
    }
    fprintf(fp,") (%s))\n",signals[i]->name);
    if (sands > 0)
      print_stacks(fp,signals,regions[2*i+1],nsignals);
    if (rands > 0) {
      if (sands==0) fprintf(fp,"(not \n");
      print_stacks(fp,signals,regions[2*i+2],nsignals);
      if (sands==0) fprintf(fp,")");
    }
    fprintf(fp,")\n");
  }
}

/*****************************************************************************/
/* Print delay elements.                                                     */
/*****************************************************************************/

void print_delay_elements(FILE *fp,eventADT *events,ruleADT **rules,
                          int ninputs,int nevents,markkeyADT *markkey,
			  int nrules,int ninpsig,int nsignals,int ndummy,
			  char * startstate)
{
  int i,j;
  char * slash;
  markingADT marking;

  marking=find_initial_marking(events,rules,markkey,nevents,nrules,ninpsig,
			       nsignals,startstate,FALSE);

  for (i=ninputs+1;i<(nevents-ndummy);i+=2)
    if (((slash=strchr(events[i]->event,'/'))==NULL) ||
        (strcmp(slash,"/1")==0)) {
      fprintf(fp,";\n");
      fprintf(fp,"(def (");
      print_signal_name(fp,events,i);
      j=1;
      while ((j<nevents) && ((rules[j][i]->ruletype==NORULE) ||
			     (rules[j][i]->ruletype & 
			      (CONTEXT | ORDERING | REDUNDANT))))
	j++;
      if (j<nevents) {
	fprintf(fp,"-delay) :%d,",rules[j][i]->lower);
	printdelay(fp,rules[j][i]->upper);
	fprintf(fp,":)\n");
      } else
	fprintf(fp,"-delay) :0,0:)\n");
      fprintf(fp,"(def (");
      print_signal_name(fp,events,i);
      fprintf(fp,"-one-delay (a) (b)) (delay-buffer a b ");
      print_signal_name(fp,events,i);
      fprintf(fp,"-delay))\n");
      fprintf(fp,"(def (");
      print_signal_name(fp,events,i);
      fprintf(fp,"-del ckt) (connect ");
      print_signal_name(fp,events,i);
/*      if (events[i]->event[0] != 'x') */
/* ??? */
/*	fprintf(fp,"-one-delay ckt #f #t))\n");
      else */
      if ((event_enabled(events,rules,markkey,marking->marking,i,nrules,
			 FALSE, marking, nsignals,FALSE,FALSE,FALSE)) ||
	  (event_enabled(events,rules,markkey,marking->marking,i+1,nrules,
			 FALSE, marking, nsignals,FALSE,FALSE,FALSE)))
	fprintf(fp,"-one-delay ckt #f #f))\n");
      else
	fprintf(fp,"-one-delay ckt #f #t))\n");
      fprintf(fp,"(def /");
      print_signal_name(fp,events,i);
      fprintf(fp,"-c/ (");
      print_signal_name(fp,events,i);
      fprintf(fp,"-del fast-");
      print_signal_name(fp,events,i);
      fprintf(fp,"-c))\n");
    }
  delete_marking(marking);
}

/*****************************************************************************/
/* Print signal list.                                                        */
/*****************************************************************************/

void print_signals(FILE *fp,signalADT *signals,int ninpsig,int nsignals)
{
  int i;

  fprintf(fp,"(");
  for (i=0;i<ninpsig;i++) {
    if (i != 0) fprintf(fp," ");
    fprintf(fp,"%s",signals[i]->name);
  }
  fprintf(fp,")(");
  for (i=ninpsig;i<nsignals;i++) {
    if (i != ninpsig) fprintf(fp," ");
    fprintf(fp,"%s",signals[i]->name);
  }
  fprintf(fp,")");
}

/*****************************************************************************/
/* Print startstate.                                                         */
/*****************************************************************************/

void print_startstate(FILE *fp,signalADT *signals,char * startstate,
		      int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    fprintf(fp,"(%s %c)",signals[i]->name,startstate[i]);
}

/*****************************************************************************/
/* Print overall implementation.                                             */
/*****************************************************************************/

void print_overall_implementation(char * filename,FILE *fp,signalADT *signals,
				  regionADT *regions,int ninpsig,int nsignals,
				  char * startstate)
{
  int i,j;
  regionADT cur_region;
  int sands,rands;

  fprintf(fp,";\n");
  fprintf(fp,"(def (%s-imp ",filename);
  print_signals(fp,signals,ninpsig,nsignals);
  fprintf(fp,")\n");
  fprintf(fp,"(wire-init-set! ");
  print_startstate(fp,signals,startstate,nsignals);
  fprintf(fp,")\n");
  fprintf(fp,"(par\n");
  for (i=ninpsig;i<nsignals;i++) {
    sands=0;
    for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0]!='E') sands++;
    rands=0;
    for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link) 
      if (cur_region->cover[0]!='E') rands++;
    fprintf(fp,"\t(/%s-c/",signals[i]->name);
    for (j=0;j<nsignals;j++) {
      cur_region=NULL;
      if (sands > 0)
	for (cur_region=regions[2*i+1]; cur_region;cur_region=cur_region->link)
	  if ((cur_region->cover[j]=='0') || (cur_region->cover[j]=='1')) {
	    fprintf(fp," %s",signals[j]->name);
	    break;
	  }
      if ((!cur_region) && (rands > 0))
	for (cur_region=regions[2*i+2]; cur_region; 
	     cur_region=cur_region->link)
	  if ((cur_region->cover[j]=='0') || (cur_region->cover[j]=='1')) {
	    fprintf(fp," %s",signals[j]->name);
	    break;
	  }
    }
    fprintf(fp," %s)\n",signals[i]->name);
  }
  fprintf(fp,"))\n");
}

/*****************************************************************************/
/* Print implementation.                                                     */
/*****************************************************************************/

void print_implementation(char * filename,FILE *fp,signalADT *signals,
			  eventADT *events,ruleADT **rules,
			  regionADT *regions,int ninputs,int nevents,
			  int ninpsig,int nsignals,char * startstate,
			  markkeyADT *markkey,int nrules,int ndummy)
{
  fprintf(fp,";\n");
  fprintf(fp,"; The implementation for %s\n",filename);
  fprintf(fp,";\n");
  print_gCs(fp,signals,regions,ninpsig,nsignals);
  print_delay_elements(fp,events,rules,ninputs,nevents,markkey,nrules,
		       ninpsig,nsignals,ndummy,startstate);
  print_overall_implementation(filename,fp,signals,regions,ninpsig,nsignals,
			       startstate);
}

/*****************************************************************************/
/* Print an AND gate.                                                        */
/*****************************************************************************/

void print_AND(FILE *fp,signalADT *signals,char * cover,int i,int nsignals,
	       bool noOR,bool comb,int output,bool noint)
{
  int j;
  bool first;
  int nliterals;

  nliterals=0;
  for (j=0;j<nsignals;j++)
    if ((cover[j] == '0') || (cover[j] == '1')) 
      nliterals++;
  if (nliterals > 1) {
    if (!noint) fprintf(fp,"\t");
    fprintf(fp,"\t(/and%d",nliterals); 
    nliterals=0;
    first=TRUE;
    for (j=0;j<nsignals;j++)
      if ((cover[j] == '0') || (cover[j] == '1')) {
	nliterals++;
	if (cover[j] == '0') {
	  if (first) {
	    first=FALSE;
	    fprintf(fp,"-n-%d",nliterals);
	  } else fprintf(fp,"-%d",nliterals);
	}
      }
    fprintf(fp,"/ ");
    for (j=0;j<nsignals;j++)
      if ((cover[j] == '0') || (cover[j] == '1'))
	fprintf(fp,"%s ",signals[j]->name);
    if (noOR && comb)
      fprintf(fp,"%s)\n",signals[output]->name);
    else
      fprintf(fp,"u_%d)\n",i);
  }
}

/*****************************************************************************/
/* Print an OR gate.                                                         */
/*****************************************************************************/

int print_OR(FILE *fp,signalADT *signals,regionADT *regions,int nsignals,
	      int i,bool comb,int output,bool noint)
{
  int nstacks,j,nliterals,lit;
  regionADT cur_region;
  bool noOR,first;

  nliterals=0;
  lit=0;
  nstacks=0; 
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') nstacks++;
  if (nstacks > 1) noOR=FALSE; else noOR=TRUE;
  nstacks=0;
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') {
      nstacks++;
      if (noOR)
	print_AND(fp,signals,cur_region->cover,i+nsignals,nsignals,noOR,
		  comb,output,noint);
      else
	print_AND(fp,signals,cur_region->cover,(3+i)*nsignals+nstacks,
		  nsignals,noOR,comb,output,noint);
    }
  if (nstacks==1) {
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
        nliterals=0;
        for (j=0;j<nsignals;j++)
          if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
            if (cur_region->cover[j]=='1') lit=j; else lit=j+nsignals;
            nliterals++;
          }
      }
    if (nliterals==1) return(lit);
  } else if (nstacks > 1) {
    if (!noint) fprintf(fp,"\t");
    fprintf(fp,"\t(/or%d",nstacks);
    nstacks=0;
    first=TRUE;
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
	nliterals=0;
	for (j=0;j<nsignals;j++)
	  if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
	    lit=j;
	    nliterals++;
	  }
	nstacks++;
	if ((nliterals==1) && (cur_region->cover[lit]=='0'))
	  if (first) {
	    first=FALSE;
	    fprintf(fp,"-n-%d",nstacks);
	  } else fprintf(fp,"-%d",nstacks);
      }
    fprintf(fp,"/ ");
    nstacks=0;
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
	nliterals=0;
	for (j=0;j<nsignals;j++)
	  if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
	    lit=j;
	    nliterals++;
	  }
	nstacks++;
	if (nliterals==1)
	  fprintf(fp,"%s ",signals[lit]->name);
	else 
	  fprintf(fp,"u_%d ",(3+i)*nsignals+nstacks);
      }
    if (comb)
      fprintf(fp,"%s)\n",signals[output]->name);
    else
      fprintf(fp,"u_%d)\n",i+nsignals);
  }
  return(-1);
}

/*****************************************************************************/
/* Print a C-element (if necessary).                                         */
/*****************************************************************************/
void print_C(FILE *fp,signalADT *signals,eventADT *events,ruleADT **rules,
	     int nsignals,bool Scomb,bool Rcomb,markkeyADT *markkey,
	     int nrules,int i,int set,int reset,bool noint,markingADT marking)
{
  if ((Scomb) && (set != (-1))) {
    if (!noint) fprintf(fp,"\t");
    if (set >= nsignals)
      fprintf(fp,"\t(/buf-n-1/ %s %s)\n",signals[set-nsignals]->name,
	      signals[i]->name);
    else
      fprintf(fp,"\t(/buf/ %s %s)\n",signals[set]->name,signals[i]->name);
  } else if (Rcomb) {
    if (!noint) fprintf(fp,"\t");
    if (reset >= nsignals)
      fprintf(fp,"\t(/buf/ %s %s)\n",signals[reset-nsignals]->name,
	      signals[i]->name);
    else if (reset >= 0)
      fprintf(fp,"\t(/buf-n-1/ %s %s)\n",signals[reset]->name,
	      signals[i]->name);
    else
      fprintf(fp,"\t(/buf-n-1/ u_%d %s)\n",2*i+2+nsignals,
	      signals[i]->name);
  } else if (!Scomb) {
    if (!noint) fprintf(fp,"\t");

    if ((reset >= 0) && (set >= 0) && (reset-nsignals==set))
      fprintf(fp,"\t(/buf/ %s",signals[set]->name);
    else if ((reset >= 0) && (set >= 0) && (set-nsignals==reset))
      fprintf(fp,"\t(/buf-n-1/ %s",signals[reset]->name);
    else {
      if ((event_enabled(events,rules,markkey,marking->marking,
			 signals[i]->event,nrules,FALSE,marking,nsignals,FALSE,
			 FALSE,FALSE)) ||
	  (event_enabled(events,rules,markkey,marking->marking,
			 signals[i]->event+1,nrules,FALSE,marking,nsignals,
			 FALSE,FALSE,FALSE)))
	fprintf(fp,"\t(/ic2");
      else
	fprintf(fp,"\t(/c2");
      if ((reset < nsignals) || (set >= nsignals)) fprintf(fp,"-n");
      if (reset < nsignals) fprintf(fp,"-1");
      if (set >= nsignals) fprintf(fp,"-2");
      if (reset >= nsignals) fprintf(fp,"/ %s",signals[reset-nsignals]->name);
      else if (reset >= 0) fprintf(fp,"/ %s",signals[reset]->name);
      else fprintf(fp,"/ u_%d",2*i+2+nsignals);
      if (set >= nsignals) fprintf(fp," %s",signals[set-nsignals]->name);
      else if (set >= 0) fprintf(fp," %s",signals[set]->name);
      else fprintf(fp," u_%d",2*i+1+nsignals);
    }
    fprintf(fp," %s)\n",signals[i]->name);
  }
}


/*****************************************************************************/
/* Print internal signal wires.                                              */
/*****************************************************************************/

bool print_internal_wires(FILE *fp,regionADT *regions,int nsignals,int i,
			  bool comb,bool first)
{
  int nstacks,j,nliterals;
  regionADT cur_region;
  bool noOR;

  nstacks=0;
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') nstacks++;
  if (nstacks > 1) noOR=FALSE; else noOR=TRUE;
  nstacks=0;
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') {
      nstacks++;
      nliterals=0;
      for (j=0;j<nsignals;j++)
	if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1'))
	  nliterals++;
      if ((nliterals > 1) && (!(noOR && comb)))
	if (first) {
	  fprintf(fp,"\t(with-wires (");
	  first=FALSE;
	} else fprintf(fp," ");
      if (nliterals > 1) 
	if ((noOR) && (!comb)) fprintf(fp,"u_%d",i+nsignals);
	else if ((!noOR) || (!comb))
	  fprintf(fp,"u_%d",(3+i)*nsignals+nstacks);
    }
  if ((!noOR) && (!comb))
    if (first) {
      fprintf(fp,"\t(with-wires (");
      first=FALSE;
    } else fprintf(fp," ");
  if ((!noOR) && (!comb)) fprintf(fp,"u_%d",i+nsignals);
  return(first);
}

/*****************************************************************************/
/* Print a gate-level implementation.                                        */
/*****************************************************************************/

void print_gl_implementation(char * filename,FILE *fp,signalADT *signals,
			     eventADT *events,ruleADT **rules,
			     regionADT *regions,int ninputs,int nevents,
			     int ninpsig,int nsignals,char * startstate,
			     markkeyADT *markkey,int nrules)
{
  int i,set,reset;
  bool noint;
  markingADT marking;
  int sands,rands;
  regionADT cur_region;

  marking=find_initial_marking(events,rules,markkey,nevents,nrules,ninpsig,
			       nsignals,startstate,FALSE);
  fprintf(fp,";\n");
  fprintf(fp,"; The gate-level implementation for %s\n",filename);
  fprintf(fp,";\n");
  fprintf(fp,"(def (%s-imp ",filename);
  print_signals(fp,signals,ninpsig,nsignals);
  fprintf(fp,")\n");
  fprintf(fp,"(wire-init-set! ");
  print_startstate(fp,signals,startstate,nsignals);
  fprintf(fp,")\n");
  fprintf(fp,"(par\n");
  set=(-1);
  reset=(-1);
  for (i=ninpsig;i<nsignals;i++) {
    noint=TRUE;
    sands=0;
    for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0]!='E') sands++;
    rands=0;
    for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link) 
      if (cur_region->cover[0]!='E') rands++;
    if (sands > 0)
      noint=print_internal_wires(fp,regions,nsignals,2*i+1,(rands==0),noint);
    if (rands > 0)
      noint=print_internal_wires(fp,regions,nsignals,2*i+2,FALSE,noint);
    if (!noint) {
      fprintf(fp,")\n");
      fprintf(fp,"\t(par\n");
    }
    if (sands > 0)
      set=print_OR(fp,signals,regions,nsignals,2*i+1,(rands==0),i,noint);
    if (rands > 0)
      reset=print_OR(fp,signals,regions,nsignals,2*i+2,FALSE,i,noint);
    print_C(fp,signals,events,rules,nsignals,(rands==0),(sands==0),markkey,
	    nrules,i,set,reset,noint,marking);    
    if (!noint) fprintf(fp,"\t))\n");
  }
  fprintf(fp,"))\n");
  delete_marking(marking);
}

/*****************************************************************************/
/* Print verification test.                                                  */
/*****************************************************************************/

void print_test(FILE *fp,signalADT *signals,eventADT *events,ruleADT **rules,
		cycleADT ****cycles,char * filename,int ninpsig,int nsignals,
		int ninputs,int nevents,int ndummy,int ncycles,
		char * startstate)
{
  int i;
  //char * slash;

  fprintf(fp,";\n");
  fprintf(fp,"; Mirror specification and compose with the implementation.\n");
  fprintf(fp,";\n");
  fprintf(fp,"(def /mirrored-%s-spec/ (mirror %s-spec))\n",filename,filename);
  fprintf(fp,";\n");
  fprintf(fp,"(def /%s-spec/ (receptive /mirrored-%s-spec/))\n",filename,
	  filename);
  fprintf(fp,";\n");
/*  fprintf(fp,"(define (doit)\n"); */
  fprintf(fp,"(verify\n");
  fprintf(fp,"\t (with-wires global (");
  print_initial_state(fp,events,rules,cycles,nevents,ndummy,ncycles,
		      startstate,TRUE);
  fprintf(fp,")\n");
  fprintf(fp,"\t(par\n");

  fprintf(fp,"\t\t(/%s-spec/",filename);
  for (i=ninpsig;i<nsignals;i++) {
      fprintf(fp," ");
      fprintf(fp,"%s",signals[i]->name);
    }
  for (i=0;i<ninpsig;i++) {
      fprintf(fp," ");
      fprintf(fp,"%s",signals[i]->name);
  }
  fprintf(fp,")\n");
  /*
  for (i=ninputs+1;i<nevents;i+=2)
    if (((slash=strchr(events[i]->event,'/')) == NULL) ||
	(strcmp(slash,"/1")==0)) {
      fprintf(fp," ");
      print_signal_name(fp,events,i);
    }
  for (i=1;i<ninputs;i+=2)
    if (((slash=strchr(events[i]->event,'/')) == NULL) ||
	(strcmp(slash,"/1")==0)) {
      fprintf(fp," ");
      print_signal_name(fp,events,i);
    }
  fprintf(fp,")\n");
  */

  fprintf(fp,"\t\t(%s-imp",filename);
  for (i=0;i<nsignals;i++) {
      fprintf(fp," ");
      fprintf(fp,"%s",signals[i]->name);
  }
  fprintf(fp,"))))\n");
  /*
  for (i=1;i<ninputs;i+=2)
    if (((slash=strchr(events[i]->event,'/')) == NULL) ||
	(strcmp(slash,"/1")==0)) {
      fprintf(fp," ");
      print_signal_name(fp,events,i);
    }
  for (i=ninputs+1;i<nevents;i+=2)
    if (((slash=strchr(events[i]->event,'/')) == NULL) ||
	(strcmp(slash,"/1")==0)) {
      fprintf(fp," ");
      print_signal_name(fp,events,i);
    }
  fprintf(fp,"))))\n");
  */

/*
  fprintf(fp,")\n");
  fprintf(fp,"(display-stuff \"Partial order timing:\" 'newline)\n");
  fprintf(fp,"(doit)\n");
  fprintf(fp,"(set! *use-po-pruning?* #f)\n");
  fprintf(fp,"(display-stuff \"With subset, no partial orders:\" 'newline)\n");
  fprintf(fp,"(doit)\n");
  fprintf(fp,"(set! *require-exact-region-match?* #t)\n");
  fprintf(fp,"(display-stuff \"Exact region match:\" 'newline)\n");
  fprintf(fp,"(doit)\n");
*/
}

/*****************************************************************************/
/* Store a file to be used by Rokicki's Timed Circuit Verifier Orbit.        */
/*****************************************************************************/

bool print_library(FILE *fp1,char * filename)
{
  FILE *fp2;
  char line[1000];

  fprintf(fp1,";\n");
  fprintf(fp1,"; Gate library: %s\n",filename);
  fprintf(fp1,";\n");
  fp2=fopen(filename,"r");
  if (fp2==NULL) {
    printf("ERROR: Cannot find file library.ver needed to verify ");
    printf("gatelevel circuits.\n");
    fprintf(lg,"ERROR: Cannot find file library.ver needed to verify ");
    fprintf(lg,"gatelevel circuits\n");
    return FALSE;
  }
  while (fgets(line,1000,fp2) != NULL)
    fputs(line,fp1);
  fclose(fp2);
  return TRUE;
}

/*****************************************************************************/
/* Get a token from a file.  Used for parsing dead traces.                   */
/*****************************************************************************/

int fgetdead(FILE *fp,char * tokvalue, int maxtok) 

{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;  
  tokvalue[toklen]='\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '\n':
    case ' ':
    case ',':
    case '(':
    case ')':
      if (readword)
	return (WORD);      
      break;
    default:
      if (toklen < maxtok) {
        readword=TRUE;
        tokvalue[toklen]=c;
        toklen++;
        tokvalue[toklen]='\0';
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
/* Parse a dead trace.                                                       */
/*****************************************************************************/

void parse_dead_trace()
{   
  FILE *fp;
  char tokvalue[MAXTOKEN];

  printf("ERROR TRACE: ");
  fprintf(lg,"ERROR TRACE: ");
  fp=Fopen("output","r");
  while ((fgetdead(fp,tokvalue,MAXTOKEN)!=END_OF_FILE) &&
	 (strcmp(tokvalue,"Dead")!=0));
  if (strcmp(tokvalue,"Dead")==0) {
    while ((fgetdead(fp,tokvalue,MAXTOKEN)!=END_OF_FILE) &&
	   (strcmp(tokvalue,"Dead")!=0));
    if (strcmp(tokvalue,"Dead")==0) {
      fgetdead(fp,tokvalue,MAXTOKEN);
      fgetdead(fp,tokvalue,MAXTOKEN);
      while ((fgetdead(fp,tokvalue,MAXTOKEN)!=END_OF_FILE) &&
	     (strcmp(tokvalue,"--")!=0))
 	if (!strchr(tokvalue,'/')) {
	  printf("%s => ",tokvalue);
	  fprintf(lg,"%s => ",tokvalue);
	}
    }
  } 
  printf("FAILURE\n");
  fprintf(lg,"FAILURE\n");
  fclose(fp);
}

/*****************************************************************************/
/* Store a file to be used by Rokicki's Timed Circuit Verifier Orbit.        */
/*****************************************************************************/

void verify(char * filename,signalADT *signals,eventADT *events,
	    mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	    regionADT *regions,int ninputs,int nevents,
	    int ncycles,int ninpsig,int nsignals,char * startstate,bool si,
	    bool verbose,bool gatelevel,bool combo,markkeyADT *markkey,
	    int nrules,bool level,int ndummy)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int c;

  if (level) {
    printf("ERROR: Cannot use Orbits to verify level-based specifications.\n");
    fprintf(lg,"ERROR: Cannot use Orbits to verify level-based specifications.\n");
    return;
  }
  strcpy(outname,filename);
  strcat(outname,".ver");
  printf("Storing verification file to:  %s\n",outname);
  fprintf(lg,"Storing verification file to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  print_header(fp,filename);
  print_specification(fp,signals,events,merges,rules,cycles,filename,ninpsig,
		      nsignals,ninputs,nevents,ndummy,ncycles,startstate,si,
		      FALSE);
  if (gatelevel) {
    if (!print_library(fp,"library.ver")) {
      fclose(fp);
      return;
    }
    print_gl_implementation(filename,fp,signals,events,rules,regions,
			    ninputs,nevents,ninpsig,nsignals,startstate,
			    markkey,nrules);
  } else
    print_implementation(filename,fp,signals,events,rules,regions,
			 ninputs,nevents,ninpsig,nsignals,startstate,
			 markkey,nrules,ndummy);
  print_test(fp,signals,events,rules,cycles,filename,ninpsig,nsignals,ninputs,
	     nevents,ndummy,ncycles,startstate);
  fclose(fp);

  fp=Fopen("verify","w");
  fprintf(fp,"(load \"%s\")\n",outname);
  fprintf(fp,"(exit)\ny\n");
  fclose(fp);
  printf("Executing command:  orbits < verify > output\n");
  fprintf(lg,"Executing command:  orbits < verify > output\n");
  sprintf(shellcommand,"%s < verify > output",getenv("ORBITS"));
  system(shellcommand);
  fp=Fopen("output","r");
  while ((c=getc(fp)) != EOF) 
    if (c==' ')
      if (((c=getc(fp)) != EOF) && (c=='s')) { 
	if (((c=getc(fp)) != EOF) && (c=='u'))
	  if (((c=getc(fp)) != EOF) && (c=='c'))
	    if (((c=getc(fp)) != EOF) && (c=='c'))
	      if (((c=getc(fp)) != EOF) && (c=='e'))
		if (((c=getc(fp)) != EOF) && (c=='e'))
		  if (((c=getc(fp)) != EOF) && (c=='d'))
		    if (((c=getc(fp)) != EOF) && (c=='e'))
		      if (((c=getc(fp)) != EOF) && (c=='d')) {
			printf("Circuit verification succeeded\n");
			fprintf(lg,"Circuit verification succeeded\n");
			break;
		      }
      } else if (c=='f') 
	if (((c=getc(fp)) != EOF) && (c=='a'))
	  if (((c=getc(fp)) != EOF) && (c=='i'))
	    if (((c=getc(fp)) != EOF) && (c=='l'))
	      if (((c=getc(fp)) != EOF) && (c=='e'))
		if (((c=getc(fp)) != EOF) && (c=='d')) {
		  printf("ERROR:  Circuit verification failed!\n");
		  fprintf(lg,"ERROR:  Circuit verification failed!\n");
		  parse_dead_trace();
		  break;
		}
  if (c==EOF) {
    printf("ERROR:  Circuit verification did not complete!\n");
    fprintf(lg,"ERROR:  Circuit verification did not complete!\n");
  }
  fclose(fp);
}


