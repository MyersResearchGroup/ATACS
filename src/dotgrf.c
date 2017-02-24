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
/* dotgrf.c                                                                */
/*****************************************************************************/

#include "dotgrf.h"

/*****************************************************************************/
/* Color events based on non-conflicting sets.                               */
/*****************************************************************************/

void color_events(eventADT *events,ruleADT **rules,int nevents) 
{
  int i,j,k,maxcolor;

  for (i=1;i<nevents;i++) 
    events[i]->color=0;
  maxcolor=0;
  for (i=1;i<nevents;i++) {
    for (k=0;(k<=maxcolor) && (events[i]->color==k);k++)
      for (j=1;j<i;j++)
	if ((events[j]->color==k) && (rules[i][j]->conflict)) {
	  events[i]->color=k+1;
	  break;
	}
    if (events[i]->color > maxcolor) maxcolor=events[i]->color;
  }
  for (i=1;i<nevents;i++) 
    for (j=1;j<nevents;j++)
      if (rules[i][j]->conflict) {
	events[i]->color++;
	break;
      }
}

/*****************************************************************************/
/* Print a dot node, color event based on conflicts.                         */
/*****************************************************************************/

void print_dot_node(FILE *fp,eventADT *events,int eventnum) 
{
  if(events[eventnum]->color){
    fprintf(fp,"\"%s\" [fontcolor=",events[eventnum]->event);
    switch (events[eventnum]->color) {
    case 0:
      fprintf(fp,"black");
      break;
    case 1:
      fprintf(fp,"blue");
      break;
    case 2:
      fprintf(fp,"green");
      break;
    case 3:
      fprintf(fp,"brown");
      break;
    case 4:
      fprintf(fp,"orange");
      break;
    default:
      fprintf(fp,"violet");
      break;
    }    
    fprintf(fp,"];\n");
  }
}


/*****************************************************************************/
/* Print a dot edge, color rule based on type.                               */
/*****************************************************************************/

void print_dot_edge(FILE *fp,eventADT *events, ruleADT **rules,
		    int i,int j,int p) 
{
  if (p != -1){
    fprintf(fp,"s%d -> t%d [label=\"[%d,",i,p,rules[i][j]->lower);
  }
  else{
    if (rules[i][j]->epsilon==1){
      fprintf(fp,"\"%s\" -> \"%s\" [label=\"[%d,",
	      events[j]->event,events[i]->event,rules[i][j]->lower);
    }
    else{
      fprintf(fp,"\"%s\" -> \"%s\" [label=\"[%d,",
	      events[i]->event,events[j]->event,rules[i][j]->lower);
    }
  }
  printdelay(fp,rules[i][j]->upper);
  fprintf(fp,"]");
  if (rules[i][j]->expr) 
    fprintf(fp,"\\n%s",rules[i][j]->expr);
  fprintf(fp,"\"");
  if (rules[i][j]->ruletype & REDUNDANT)
    fprintf(fp,",color=green");
  else if (rules[i][j]->ruletype & ORDERING)
    fprintf(fp,",color=orange");
  else if (rules[i][j]->ruletype & DISABLING)
    fprintf(fp,",color=blue");
  else if (rules[i][j]->ruletype & ASSUMPTION)
    fprintf(fp,",color=violet");
  else if (!(rules[i][j]->ruletype & SPECIFIED)){
    fprintf(fp,",color=brown");
  }
  if ((p==(-1)) && (rules[i][j]->epsilon==1))
    fprintf(fp,",fontcolor=red,style=dotted,dir=back");
  fprintf(fp,"];\n"); 
}

/*****************************************************************************/
/* Print a graph using parseGraph by Tom Rokicki.                            */
/*****************************************************************************/

void tel_struct(FILE *fp,eventADT *events,ruleADT **rules,int nevents)
{
  int i,j;

  for (i=1;i<nevents;i++){
    print_dot_node(fp,events,i);
    for (j=1;j<nevents;j++){
      if ((rules[i][j]->ruletype != NORULE)) {
	print_dot_edge(fp,events,rules,i,j,-1); 
      }
    }
  }
}

/*****************************************************************************/
/* Print a graph using parseGraph by Tom Rokicki.                            */
/*****************************************************************************/

void timed_pn(FILE *fp,eventADT *events,ruleADT **rules,int nevents)
{
  int i,j,k,l;
  int **place;
  int nextp,p;
  int eps;
  bool shared;
  bool print;

  place=(int **)GetBlock(nevents * sizeof(int*));
  for (i=1;i<nevents;i++) {
    place[i]=(int *)GetBlock(nevents * sizeof(int));
    for (j=1;j<nevents;j++) 
      place[i][j]=0;
  }
  nextp=0;    
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE)) {
	p=0;
	eps=rules[i][j]->epsilon;
	shared=FALSE;
	for (k=1;k<nevents;k++)
	  if ((rules[i][k]->conflict) &&
	      (rules[k][j]->ruletype != NORULE)) { 
	    if ((p != 0) && (p != place[k][j])) {
	      print=TRUE;
	      for (l=1;l<nevents;l++)
		if (place[i][l]==p) print=FALSE;
	      if (print){
		print_dot_node(fp,events,i);
		print_dot_edge(fp,events,rules,i,j,p); 
	      }
	      print=TRUE;
	      for (l=1;l<nevents;l++)
		if (place[l][j]==p) print=FALSE;
	      if (print) fprintf(fp,"t%d -> s%d;\n",p,j);
	    }
	    if (place[k][j] != 0) p=place[k][j];
	    shared=TRUE;
	    if (rules[k][j]->epsilon==0) eps=0; 
	  }
	if (p==0) 
	  for (k=1;k<nevents;k++)
	    if ((rules[k][j]->conflict) &&
		(rules[i][k]->ruletype != NORULE) /* &&
		(rules[i][k]->epsilon==eps)*/) {
	      if ((p != 0) && (p != place[i][k])) {
		print=TRUE;
		for (l=1;l<nevents;l++)
		  if (place[i][l]==p) print=FALSE;
		if (print){
		  print_dot_node(fp,events,i);
		  print_dot_edge(fp,events,rules,i,j,p); 
		}
		print=TRUE;
		for (l=1;l<nevents;l++)
		  if (place[l][j]==p) print=FALSE;
		if (print) fprintf(fp,"t%d -> s%d;\n",p,j);
	      }
	      if (place[i][k] != 0) p=place[i][k];
	      shared=TRUE;
	      if (rules[i][k]->epsilon==0) eps=0; 
	    }
	if (p==0) {
	  p=++nextp;
	  if (eps==1) {
	    fprintf(fp,"t%d [style=filled,color=black,shape=doublecircle,label=\"\"];\n",p);
	  } else if (shared) {
	    fprintf(fp,"t%d [shape=circle,label=\"\"];\n",p);
	  }
	}
	print=FALSE;
	if ((eps!=1) && (!shared)) {
	  print=TRUE;
	  for (k=1;k<nevents;k++)
	    if (place[i][k]==p) print=FALSE; 
	  if (print) {
	    for (k=1;k<nevents;k++)
	      if (place[k][j]==p) print=FALSE;
	    if (print) {
	      print_dot_node(fp,events,i);
	      print_dot_edge(fp,events,rules,i,j,-1); 
	    }
	  }
	}
	if (!print) {
	  print=TRUE;
	  for (k=1;k<nevents;k++)
	    if (place[i][k]==p) print=FALSE; 
	  if (print){
	    print_dot_node(fp,events,i);
	    print_dot_edge(fp,events,rules,i,j,p); 
	  }
	  print=TRUE;
	  for (k=1;k<nevents;k++)
	    if (place[k][j]==p) print=FALSE; 
	  if (print) fprintf(fp,"t%d -> s%d;\n",p,j);
	  place[i][j]=p;
	}
      }
  for (i=1;i<nevents;i++) 
    free(place[i]);
  free(place);
}

/*****************************************************************************/
/* Print a graph using parseGraph by Tom Rokicki.                            */
/*****************************************************************************/

void dot_print_grf(char * filename,eventADT *events,ruleADT **rules,
		   int nevents,bool display)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  
  strcpy(outname,filename);
  strcat(outname,".dot");
  printf("Storing constraint graph to:  %s\n",outname);
  fprintf(lg,"Storing constraint graph to:  %s\n",outname);
  if(!(fp=fopen(outname,"w"))){
    fprintf(stderr,"Could not open %s for writing.\n", outname);
    fprintf(lg    ,"Could not open %s for writing.\n", outname);
    problems++;
    return;
  }
  fprintf(fp,"digraph G {\n");
  fprintf(fp,"size=\"7.5,10\"\n");
  fprintf(fp,"node [shape=plaintext];\n");
  color_events(events,rules,nevents); 
  //  timed_pn(fp,events,rules,nevents);
  tel_struct(fp,events,rules,nevents);
  fprintf(fp,"}");
  fclose(fp);
  if (display) {
    sprintf(shellcommand,"dotty %s.dot &",filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
  } else {
    sprintf(shellcommand,"%s %s",getenv("PG"),filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
  }
  system(shellcommand);
}
