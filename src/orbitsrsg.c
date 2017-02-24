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
/* orbitsrsg.c                                                               */
/*****************************************************************************/

#include "orbitsrsg.h"
#include "verify.h"
#include "merges.h"
#include "findrsg.h"
#include "def.h"
#include "findreg.h"
#include "memaloc.h"

#define WORD 1          /* strings, characters */
#define END_OF_FILE 4   /* eof character */

typedef struct worklist_tag {
  int t;
  int o;
  int s1;
  int s2;
  char dir;
  struct worklist_tag *link;
} *worklistADT;

typedef struct ostate_tag {
  char * state;
  char * enablings;
  int *next;
} *ostateADT;

/*****************************************************************************/
/* Find event instance in event list and return its position.                */
/*****************************************************************************/

int find_event_instance(eventADT *events,int nevents,char * event)
{
  int i,j,k;

  for (i=1;i<nevents;i+=2) {
    for (j=0; (events[i]->event[j]==event[j]) ||
	 (events[i]->event[j]==(event[j] - 32)); j++)
      if ((events[i]->event[j+1]=='+') || (events[i]->event[j+1]=='-'))
	if ((event[j+1]=='+') || (event[j+1]=='-') &&
	    (events[i]->event[j+2]=='\0')) return((i-1)/2);
	else for (k=j+1; (events[i]->event[k+1]==event[k]); k++)
	  if ((event[k+1]=='+') || (event[k+1]=='-') &&
	      (events[i]->event[k+2]=='\0')) return((i-1)/2);
  }
  printf("ERROR:  %s is an undeclared event!\n",event);
  fprintf(lg,"ERROR:  %s is an undeclared event!\n",event);
  return(-1);
}

/*****************************************************************************/
/* Find event instance in event list and return its position.                */
/*****************************************************************************/

int find_flat_event_instance(eventADT *events,mergeADT *merges,int nevents,
			     char * event)
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
            ((event[j+1]=='/') || (event[j+1]=='+') || (event[j+1]=='-')))
	  return(position);
      position++;
    }
  }
  printf("ERROR:  %s is an undeclared event!\n",event);
  fprintf(lg,"ERROR:  %s is an undeclared event!\n",event);
  return(-1);
}

/*****************************************************************************/
/* Get a token from a file.  Used for loading grf files.                     */
/*****************************************************************************/

int fgettokgrf(FILE *fp, char * tokvalue, int maxtok)
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

bool valid_state(char * state,int nsignals)
{
  int i;

  for (i=0; i<nsignals; i++)
    if (state[i]=='Z') return(FALSE);
  return(TRUE);
}

void state_transition(ostateADT *ostates,int nsignals,int t,int o,int s1,
		      int s2,char dir)
{
  int i;

  for (i=0; i<nsignals; i++) 
    if (ostates[s2]->state[i]=='Z') {
      if (ostates[s1]->state[i]=='R')
	ostates[s2]->state[i]='0';
      else if (ostates[s1]->state[i]=='F')
	ostates[s2]->state[i]='1';
      else 
	ostates[s2]->state[i]=ostates[s1]->state[i];
    } else {
      if ((i != t) && (VAL(ostates[s1]->state[i])!=VAL(ostates[s2]->state[i])))
	ostates[s2]->state[i]='X';
/*	printf("%s -> %s %d should be equal value\n",
	       ostates[s1]->state,ostates[s2]->state,i); */
    }
  if (dir=='+') {
    if (ostates[s1]->state[t]=='0') {
      ostates[s1]->state[t]='R';
      ostates[s1]->enablings[o]='T';
    }
    if (ostates[s2]->state[t]!='F') ostates[s2]->state[t]='1';
  } else {
    if (ostates[s1]->state[t]=='1') {
      ostates[s1]->state[t]='F';
      ostates[s1]->enablings[o]='T';
    }
    if (ostates[s2]->state[t]!='R') ostates[s2]->state[t]='0'; 
  } 
}

worklistADT new_worklist(int t,int o,int s1,int s2,char dir)
{
  worklistADT worklist=NULL;

  worklist=(worklistADT)GetBlock(sizeof(*worklist));
  worklist->t=t;
  worklist->o=o;
  worklist->s1=s1;
  worklist->s2=s2;
  worklist->dir=dir;
  worklist->link=NULL;
  return(worklist);
}

void add_to_worklist(worklistADT worklist,int t,int o,int s1,int s2,char dir)
{
  for (; worklist->link; worklist=worklist->link);
  worklist->link=(worklistADT)GetBlock(sizeof(*worklist));
  worklist=worklist->link;
  worklist->t=t;
  worklist->o=o;
  worklist->s1=s1;
  worklist->s2=s2;
  worklist->dir=dir;
  worklist->link=NULL;
}

worklistADT rm_from_worklist(worklistADT worklist,
			      int *t,int *o,int *s1,int *s2,char *dir)
{
  worklistADT newlist=NULL;

  *t=worklist->t;
  *o=worklist->o;
  *s1=worklist->s1;
  *s2=worklist->s2;
  *dir=worklist->dir;
  newlist=worklist->link;
  free(worklist);
  return(newlist);
}

worklistADT apply_transition(ostateADT *ostates,worklistADT worklist,
			     int nsignals,int t,int o,int s1,int s2,char dir)
{
  if (valid_state(ostates[s1]->state,nsignals))
    state_transition(ostates,nsignals,t,o,s1,s2,dir);
  else {
    if (!worklist)
      worklist=new_worklist(t,o,s1,s2,dir);
    else
      add_to_worklist(worklist,t,o,s1,s2,dir);
  }
  return(worklist);
}

void skip_n_tokens(FILE *fp,char * tokvalue,int n)
{
  int i;

  for (i=0;i<n;i++)
    fgettokgrf(fp,tokvalue,MAXTOKEN);
}

/*****************************************************************************/
/* Load a grf file output from Orbits.                                       */
/*****************************************************************************/

ostateADT *loadgrf(char * filename,eventADT *events,mergeADT *merges,
		   int nevents,int nsignals,char * startstate,int *nstates)
{
  char inname[FILENAMESIZE];
  char tokvalue[MAXTOKEN];
  int i,j,t,o,s1,s2;
  char dir;
  ostateADT *ostates=NULL;
  bool first;
  FILE *fp;
  worklistADT worklist=NULL;
 
  strcpy(inname,filename);
  strcat(inname,".osg");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(NULL);
  }
  printf("Loading state graph in grf format from:  %s\n",inname);
  fprintf(lg,"Loading state graph in grf format from:  %s\n",inname);
  first=TRUE;
  *nstates=0;
  while (fgettokgrf(fp,tokvalue,MAXTOKEN) != END_OF_FILE) {
    if (strcmp(tokvalue,"N")==0) {
      fgettokgrf(fp,tokvalue,MAXTOKEN);
      if (strcmp(tokvalue,"F")==0) {
	skip_n_tokens(fp,tokvalue,2);
	if (atoi(tokvalue) > *nstates) *nstates=atoi(tokvalue);
      } else {
	if (first) {
	  ostates=(ostateADT *)GetBlock((*nstates) * sizeof(ostates[0]));
	  for (i=0;i<*nstates;i++) {
	    ostates[i]=(ostateADT)GetBlock(sizeof(*ostates[0]));
	    ostates[i]->state=(char *)GetBlock((nsignals+2) * sizeof(char));
	    ostates[i]->enablings=(char *)GetBlock((((nevents-1)/2)+2) 
						 * sizeof(char));
	    ostates[i]->next=(int *)GetBlock((nsignals+2) * sizeof(int));
	    for (j=0;(startstate[j] != '\0');j++) {
	      ostates[i]->state[j]='Z';
	      ostates[i]->next[j]=(-1);
	    }
	    ostates[i]->state[j]='\0';
	    for (j=0;j<(nevents-1)/2;j++)
	      ostates[i]->enablings[j]='F';
	    ostates[i]->enablings[j]='\0';
	  }
	  strcpy(ostates[0]->state,startstate);
	}
	first=FALSE;
	skip_n_tokens(fp,tokvalue,2);
	t=/* find_flat_event(events,merges,nevents,tokvalue); */
	  find_flat_event_instance(events,merges,nevents,tokvalue);
	if (t==(-1)) return NULL;
	o=find_event_instance(events,nevents,tokvalue);
	if (o==(-1)) return NULL;
	dir=tokvalue[strlen(tokvalue)-1];
      } 
    } else {
      skip_n_tokens(fp,tokvalue,2);
      strcpy(tokvalue,&tokvalue[1]);
      s1=atoi(tokvalue)-1;
      skip_n_tokens(fp,tokvalue,8);
      strcpy(tokvalue,&tokvalue[1]);
      s2=atoi(tokvalue)-1;
      i=0;
      while (ostates[s1]->next[i]!=(-1)) i++;
      ostates[s1]->next[i]=s2;
      worklist=apply_transition(ostates,worklist,nsignals,t,o,s1,s2,dir);
      skip_n_tokens(fp,tokvalue,1);
    } 
  }
  fclose(fp);
  while (worklist) {
    worklist=rm_from_worklist(worklist,&t,&o,&s1,&s2,&dir);
    worklist=apply_transition(ostates,worklist,nsignals,t,o,s1,s2,dir);
  }
  return(ostates);
}

/*
bool vacuous_state(char * state,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if ((state[i]=='R') || (state[i]=='F')) return(FALSE);
  printf("vacuous state %s\n",state);
  return(TRUE);
}
*/

int add_states(FILE *fp,stateADT *state_table,ostateADT *ostates,int nsignals,
	       bool verbose,int i,int sn,bddADT bdd_state,
	       bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
	       ruleADT **rules)
{
  int j;

  j=0;
  while (ostates[i]->next[j] != (-1)) {
    if (add_state(fp,state_table,ostates[i]->state,NULL,
		  ostates[i]->enablings, NULL, NULL, 0, NULL, 
		  ostates[ostates[i]->next[j]]->state,NULL,
		  ostates[ostates[i]->next[j]]->enablings,
		  nsignals, NULL, NULL, 0, sn,verbose, 0, 0, 0,0,
		  bdd_state, use_bdd,marker,timeopts, -1, -1, NULL,
		  rules, 0)) {
      sn++;
      sn=add_states(fp,state_table,ostates,nsignals,verbose,
		    ostates[i]->next[j],sn, bdd_state, use_bdd,marker,
		    timeopts,rules);
    }
    j++;
  }
  return(sn);
}

/*****************************************************************************/
/* Load Orbits state graph in grf format and convert to rsg format.          */
/*****************************************************************************/

bool grf2rsg(char * filename,signalADT *signals,eventADT *events,
	     mergeADT *merges,stateADT *state_table,int nevents,
	     int nsignals,int ninpsig,int nsigs,char * startstate,
	     bool verbose,bddADT bdd_state, bool use_bdd,markkeyADT *marker,
	     timeoptsADT timeopts,ruleADT **rules)
{
  char outname[FILENAMESIZE];
  int i,nstates;
  ostateADT *ostates;
  FILE *fp;

  ostates=loadgrf(filename,events,merges,nevents,nsignals,startstate,&nstates);
  if (ostates==NULL) {
    printf("ERROR:  Orbits unable to produce state graph!\n");
    fprintf(lg,"ERROR:  Orbits unable to produce state graph!\n");
    return(FALSE);
  }
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".rsg");
    printf("Finding reduced state graph and storing to:  %s\n",outname);
    fprintf(lg,"Finding reduced state graph and storing to:  %s\n",outname);
    fp=Fopen(outname,"w");
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,nsigs,ninpsig);
    fprintf(fp,"STATES:\n");
  } else {
    printf("Finding reduced state graph ... ");
    fflush(stdout);
    fprintf(lg,"Finding reduced state graph ... ");
    fflush(lg);
  }
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
 	    ostates[0]->state,NULL,
 	    ostates[0]->enablings,nsignals, NULL, NULL, 0, 0,verbose, 0, 0,0,0,
	    bdd_state, use_bdd,marker,timeopts, -1, -1, NULL,
	    rules,0);
  add_states(fp,state_table,ostates,nsignals,verbose,0,1,
	     bdd_state, use_bdd,marker,timeopts,rules);
  if (verbose) fclose(fp);
  else {
    printf("done\n");
    fprintf(lg,"done\n");
  }
  for (i=0;i<nstates;i++) {
    free(ostates[i]->state);
    free(ostates[i]->enablings);
    free(ostates[i]->next);
  }
  free(ostates);
  return(TRUE);
}

/*****************************************************************************/
/* Print state graph generation command.                                     */
/*****************************************************************************/

void print_sg_gen(FILE *fp,eventADT *events,ruleADT **rules,
		  cycleADT ****cycles,char * filename,int nevents,int ndummy,
		  int ncycles,char * startstate,bool verify)
{
  fprintf(fp,";\n");
  fprintf(fp,"; Mirror specification and compose with the implementation.\n");
  fprintf(fp,";\n");
  fprintf(fp,"(def /mirrored-%s-spec/ (mirror %s-spec))\n",filename,filename);
  fprintf(fp,";\n");
  fprintf(fp,"(def /%s-spec/ (receptive /mirrored-%s-spec/))\n",filename,
	  filename);
  fprintf(fp,";\n");
  if (verify) fprintf(fp,"(verify\n");
  else 
    fprintf(fp,"(save-state-graph \"%s.osg\" normalize ignore-receptiveness\n",
	    filename);
  fprintf(fp,"\t (with-wires global (");
  print_initial_state(fp,events,rules,cycles,nevents,ndummy,ncycles,
		      startstate,TRUE);
  fprintf(fp,")\n");
  fprintf(fp,"\t/%s-spec/))",filename);
}

/*****************************************************************************/
/* Store a file to be used by Rokicki's Orbits to find state graph.          */
/*****************************************************************************/

bool orbits_rsg(char * filename,signalADT *signals,eventADT *events,
		mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
		stateADT *state_table,int nevents,int ncycles,
		int nsignals,int ninpsig,int nsigs,char * startstate,bool si,
		bool verbose,bddADT bdd_state,bool use_bdd,markkeyADT *marker,
		timeoptsADT timeopts,bool level,int ndummy)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;

  if (level) {
    printf("ERROR: Cannot use Orbits to find state space for level-based specification\n");
    fprintf(lg,"ERROR: Cannot use Orbits to find state space for level-based specification\n");
    return FALSE;
  }
  strcpy(outname,filename);
  strcat(outname,".spec");
  printf("Storing orbits specification file to:  %s\n",outname);
  fprintf(lg,"Storing orbits specification file to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  initialize_state_table(LOADED,FALSE,state_table);
  print_header(fp,filename);
  print_specification(fp,signals,events,merges,rules,cycles,filename,
		      nsignals,nsignals,nevents-1,nevents,ndummy,ncycles,
		      startstate,si,/* FALSE */ TRUE);
  print_sg_gen(fp,events,rules,cycles,filename,nevents,ndummy,ncycles,
	       startstate,FALSE);
  fclose(fp);
  fp=Fopen("findrsg","w");
  fprintf(fp,"(load \"%s\")\n",outname);
  fprintf(fp,"(exit)\ny\n");
  fclose(fp);

/*  
  sprintf(shellcommand,"rm %s.osg",filename);
  system(shellcommand);
*/

  printf("Executing command:  orbits < findrsg > output\n");
  fprintf(lg,"Executing command:  orbits < findrsg > output\n");
  sprintf(shellcommand,"%s < findrsg > output",getenv("ORBITS"));
  system(shellcommand);
  if (!verbose) {
    system("rm findrsg");
/*    system("rm output"); */
  }
  if ((!grf2rsg(filename,signals,events,merges,state_table,nevents,
		nsignals,ninpsig,nsigs,startstate,verbose,
		bdd_state,use_bdd,marker,timeopts,rules))) {
    strcpy(outname,filename);
    strcat(outname,".ver");
    printf("Storing orbits verification file to:  %s\n",outname);
    fprintf(lg,"Storing orbits verification file to:  %s\n",outname);
    fp=Fopen(outname,"w"); 
    print_header(fp,filename);
    print_specification(fp,signals,events,merges,rules,cycles,filename,
			nsignals,nsignals,nevents-1,nevents,ndummy,ncycles,
			startstate,si,/* FALSE */ TRUE);
    print_sg_gen(fp,events,rules,cycles,filename,nevents,ndummy,ncycles,
		 startstate,TRUE);
    fclose(fp);
    fp=Fopen("verify","w");
    fprintf(fp,"(load \"%s\")\n",outname);
    fprintf(fp,"(exit)\ny\n");
    fclose(fp);
    printf("Executing command:  orbits < verify > output\n");
    fprintf(lg,"Executing command:  orbits < verify > output\n");
    sprintf(shellcommand,"%s < verify > output",getenv("ORBITS"));
    system(shellcommand);
    parse_dead_trace();
    return(FALSE);
  }
  return(TRUE);
}


