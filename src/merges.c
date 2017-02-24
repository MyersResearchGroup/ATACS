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
/* merges.c                                                                  */
/*****************************************************************************/

#include "merges.h"
#include "def.h"
#include "connect.h"
#include "loader.h"

/*****************************************************************************/
/* Print a signal name.                                                      */
/*****************************************************************************/

void print_signal_name(FILE *fp,eventADT *events,int i)
{
  int j;

  for (j=0;
       (events[i]->event[j] != '+') && (events[i]->event[j] != '-');
       j++)
    fprintf(fp,"%c",events[i]->event[j]);
}

/*****************************************************************************/
/* Check if two signals are merged.                                          */
/*****************************************************************************/

bool are_merged(mergeADT *merges,int i,int j)
{
  mergeADT curmerge;
  int iup,idown;
  int jup,jdown;

  iup=i; idown=i;
  if (i % 2==1) idown=i+1;
  else iup=i-1;
  jup=j; jdown=j;
  if (j % 2==1) jdown=j+1;
  else jup=j-1;
  for (curmerge=merges[iup]; curmerge != NULL; curmerge=curmerge->link)
    if ((curmerge->mergetype==DISJMERGE) && (curmerge->mevent==jup)) 
      return(TRUE);
  for (curmerge=merges[idown]; curmerge != NULL; curmerge=curmerge->link)
    if ((curmerge->mergetype==CONJMERGE) && (curmerge->mevent==jdown)) 
      return(TRUE);
  return(FALSE);
}

/*****************************************************************************/
/* Add a context rule to the timed event-rule structure.                     */
/*****************************************************************************/

bool add_context_rule(FILE *fp, eventADT *events,ruleADT **rules,
		      markkeyADT *markkey,cycleADT ****cycles,
		      char phase,int enabling_signal,int enabled,int *nrules,
		      int nevents,int ncycles,bool verbose)
{
  int enabling,nextoccur,index;
  char * c;
  char * s;

  if (ncycles==0) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR:  Cannot add context rules with 0 cycles!\n");
    fprintf(lg,"ERROR:  Cannot add context rules with 0 cycles!\n");
    return(FALSE);
  }
  if (phase=='1') 
    enabling=2*enabling_signal+1;
  else
    enabling=2*enabling_signal+2;
  s=CopyString(events[enabling]->event);
  if (reachable(cycles,nevents,ncycles,enabling,0,enabled,0)==1) {
    if (strchr(events[enabling]->event,'/') != NULL) {
      nextoccur=enabling;
      while ((nextoccur != (-1)) &&
	     (reachable(cycles,nevents,ncycles,nextoccur,0,enabled,0)==1)) {
	c=strchr(s,'/');
	c++;
	index=atoi(c);
	index++;
	sprintf(c,"%d",index);
	enabling=nextoccur;
	nextoccur=which_event(nevents,events,s);
	if (nextoccur != (-1)) 
	  reachable(cycles,nevents,ncycles,nextoccur,0,enabled,0);
      }
    }
    rules[enabling][enabled]->epsilon=0;
  } else {
    if (strchr(events[enabling]->event,'/') != NULL) {
      nextoccur=enabling;
      while (nextoccur != (-1)) {
	c=strchr(s,'/');
	c++;
	index=atoi(c);
	index++;
	sprintf(c,"%d",index);
	enabling=nextoccur;
	nextoccur=which_event(nevents,events,s);
      }
    }
    rules[enabling][enabled]->epsilon=1;
  }
  free(s);
  if ((rules[enabling][enabled]->ruletype == NORULE) ||
      (rules[enabling][enabled]->ruletype >= REDUNDANT)) {
    if (verbose)
      fprintf(fp,"< %s,%s,%d,[0,0] >\n",events[enabling]->event,
	      events[enabled]->event,rules[enabling][enabled]->epsilon);
    rules[enabling][enabled]->lower=0;
    rules[enabling][enabled]->upper=0;
    rules[enabling][enabled]->ruletype=
      (rules[enabling][enabled]->ruletype | CONTEXT) & (~REDUNDANT);
    markkey[*nrules]->enabling=enabling;
    markkey[*nrules]->enabled=enabled;
    markkey[*nrules]->epsilon=rules[enabling][enabled]->epsilon;
    (*nrules)++;
  }
  return(TRUE);
}

/*****************************************************************************/
/* Free space used by old set of merges and                                  */
/*   create space for new set of merges.                                     */
/*****************************************************************************/

mergeADT *new_merges(int status,bool error,int oldnevents,int nevents,
		     mergeADT *oldmerges)
{
  mergeADT *merges=NULL;
  mergeADT curmerge,nextmerge;
  int a;

  if (((status & LOADED) || (error==TRUE)) && (oldmerges != NULL)) {
    for (a=0;a<oldnevents;a++) {
      nextmerge=oldmerges[a];
      while (nextmerge) {
	curmerge=nextmerge;
	nextmerge=curmerge->link;
	free(curmerge);
      }
    }
    free(oldmerges);
  }
  if (error==FALSE) {
    merges=(mergeADT*)GetBlock(nevents * sizeof(merges[0]));
    for (a=0;a<nevents;a++) {
      merges[a]=(mergeADT)GetBlock(sizeof(*merges[0]));
      merges[a]->mevent=(-1);
      merges[a]->mergetype=NOMERGE;
      merges[a]->link=NULL;
    }
    return(merges);
  }
  return(NULL);
}

/*****************************************************************************/
/* Add merger to a list.                                                     */
/*****************************************************************************/

void add_merger(mergeADT *merges,int e,int f,int mergetype)
{
  mergeADT curmerge;

  if (merges[e]->mergetype==NOMERGE) {
    merges[e]->mevent=f;
    merges[e]->mergetype=mergetype;
  } else {
    for (curmerge=merges[e];
	 (curmerge->link != NULL) && (curmerge->mevent != f);
	 curmerge=curmerge->link);
    if (curmerge->mevent != f) {
      curmerge->link=(mergeADT)GetBlock(sizeof(*merges[0]));
      curmerge=curmerge->link;
      curmerge->mevent=f;
      curmerge->mergetype=mergetype;
      curmerge->link=NULL;
    }
  }
}

/*****************************************************************************/
/* Add mergers.                                                              */
/*****************************************************************************/

void add_mergers(mergeADT *merges,int e,int f,int mergetype)
{
  
  add_merger(merges,e,f,mergetype);
  add_merger(merges,f,e,mergetype);
/*
  for (curmerge=merges[e];
       curmerge != NULL;
       curmerge=curmerge->link) 
    if (curmerge->mevent != f) {
      add_merger(merges,curmerge->mevent,f,mergetype);
      add_merger(merges,f,curmerge->mevent,mergetype);
    }
*/
}

/*****************************************************************************/
/* Print mergers.  FOR TESTING PURPOSES!!!                                   */
/*****************************************************************************/

void print_mergers(eventADT *events,mergeADT *merges,int nevents)
{
  mergeADT curmerge;
  int i;

  for (i=0;i<nevents;i++)
    for (curmerge=merges[i];
	 curmerge != NULL;
	 curmerge=curmerge->link) 
      if (curmerge->mergetype==CONJMERGE)
	printf("%s conjunctively merged with %s\n",
	       events[i]->event,events[curmerge->mevent]->event);
      else if (curmerge->mergetype==DISJMERGE)
	printf("%s disjunctively merged with %s\n",
	       events[i]->event,events[curmerge->mevent]->event);
}

/*****************************************************************************/
/* Load merger lists.                                                        */
/*****************************************************************************/

bool load_mergers(FILE *fp,int nevents,int nmergers,eventADT *events,
		 mergeADT *merges,int *linenum)
{
  int e,f,i,token;
  char tokvalue[MAXTOKEN];

  for (i=0;i<nmergers;i++) {
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != WORD)
      if (token==END_OF_FILE) {
	printf("ERROR:  Unexpected end of file!\n");
	fprintf(lg,"ERROR:  Unexpected end of file!\n");
	return(FALSE); 
      }
    if ((e=find_event(nevents,events,tokvalue,*linenum))==(-1)) 
      return(FALSE);
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != END_OF_LINE) {
      if (token==END_OF_FILE) {
	printf("ERROR:  Unexpected end of file!\n");
	fprintf(lg,"ERROR:  Unexpected end of file!\n");
	return(FALSE);
      }
      if (token==WORD) {
	if ((f=find_event(nevents,events,tokvalue,*linenum))==(-1)) 
	  return(FALSE);
	if (e % 2==1) add_mergers(merges,e,f,DISJMERGE);
	else add_mergers(merges,e,f,CONJMERGE);
      }
    }
  }
/*  print_mergers(events,merges,nevents); */
  return(TRUE);
}

/*****************************************************************************/
/* Find flat event in event list and return its position.                    */
/*****************************************************************************/

int find_flat_event(eventADT *events,mergeADT *merges,int nevents,char * event)
{
  int i,j,position;
  mergeADT curmerge;
  bool merged;

  position=0;
  for (i=1;i<nevents;i+=2) {
    merged=TRUE;
    for (curmerge=merges[i]; curmerge != NULL; curmerge=curmerge->link)
      if ((curmerge->mergetype==DISJMERGE) && (curmerge->mevent < i))
        merged=FALSE;
    for (curmerge=merges[i+1]; curmerge != NULL; curmerge=curmerge->link)
      if ((curmerge->mergetype==CONJMERGE) && (curmerge->mevent < (i+1)))
        merged=FALSE;
    if (merged) {
      for (j=0; (events[i]->event[j]==event[j]) ||
           (events[i]->event[j]==(event[j] - 32)); j++)
        if (((events[i]->event[j+1]=='+') || (events[i]->event[j+1]=='-')) &&
            ((event[j+1]=='+') || (event[j+1]=='-')))
          return(position);
      position++;
    }
  }
  printf("ERROR:  %s is an undeclared event!\n",event);
  fprintf(lg,"ERROR:  %s is an undeclared event!\n",event);
  return(-1);
}
