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
/* printlpn.c                                                                */
/*****************************************************************************/

#include "printlpn.h"

void print_rule_label(FILE *fp,int type1,int type2,int lower,int upper,
		      int plower,int pupper,int weight,char *expr,bool marked,
		      char *place,char *place2,bool ruletype) 
{
  fprintf(fp,"\n");
  return;

  if (!ruletype) {
    if ((!(type1 & CONT)) && (type2 & CONT)) {
      fprintf(fp," [label=\"");
      if (plower==pupper) 
	fprintf(fp,"%s:=%d\\n\"",place2,pupper);
      else if (pupper!=INFIN)
	fprintf(fp,"%s:=[%d,%d]\\n\"",place2,plower,pupper);
      else if (pupper==INFIN)
	fprintf(fp,"%s:=[%d,inf]\\n\"",place2,plower);
      fprintf(fp,",style=dotted]\n");
    } else
      fprintf(fp,"\n");
  } else {
    fprintf(fp," [label=\"");
    if (weight!=1)
      fprintf(fp,"%d\\n",weight);
    if (((type1 & CONT) && (!(type2 & CONT))) ||
	((type2 & CONT) && (!(type1 & CONT)))) {
      if ((type2 & CONT) && (plower==1) && (pupper=INFIN)) 
	;
      else if (plower==pupper) 
	fprintf(fp,"%s=%d\\n",place,pupper);
      else if ((plower==(-1)*INFIN)&&(pupper!=INFIN))
	fprintf(fp,"%s<=%d\\n",place,pupper);
      else if ((plower!=(-1)*INFIN)&&(pupper==INFIN))
	fprintf(fp,"%s>=%d\\n",place,plower);
      else if ((plower!=(-1)*INFIN)&&(pupper!=INFIN))
	fprintf(fp,"%d>=%s>=%d\\n",plower,place,pupper);
    }
    if (!(type2 & CONT)) {
      fprintf(fp,"[%d,",lower);
      if (upper==INFIN)
	fprintf(fp,"inf]");
      else 
	fprintf(fp,"%d]",upper);
    }
    if (expr)
      fprintf(fp,"\\n[%s]\"",expr);
    else
      fprintf(fp,"\"");
    if (((type1 & CONT) && !(type2 & CONT)) ||
	(!(type1 & CONT) && (type2 & CONT)))
      fprintf(fp,",style=dotted]\n");
    else
      fprintf(fp,"]\n");
  }
}

void print_inequalities(FILE *fp,eventADT *events,ineqADT inequalities) 
{
  bool first = true;
  fprintf(fp,"[");
  for (ineqADT curineq=inequalities;curineq;curineq=curineq->next) {
    if (!first) fprintf(fp,"&");
    else first=false;
    fprintf(fp,"%s",events[curineq->place]->event);
    switch (curineq->type) {
    case 0:
      fprintf(fp,">");
      break;
    case 1:
      fprintf(fp,">=");
      break;
    case 2:
      fprintf(fp,"<");
      break;
    case 3:
      fprintf(fp,"<=");
      break;
    case 4:
      fprintf(fp,"=");
      break;
    case 5:
      fprintf(fp,":=");
      break;
    case 6:
      fprintf(fp,"'dot:=");
      break;
    }
    if (curineq->expr) {
      fprintf(fp,"%s",curineq->expr);
    } else {
      if (curineq->constant == curineq->uconstant)
	fprintf(fp,"%d",curineq->constant);
      else
	fprintf(fp,"[%d,%d]",curineq->constant,curineq->uconstant);
    }
  }
  fprintf(fp,"]");
}

void print_assignments(FILE *fp,signalADT *signals,eventADT *events,
		       ineqADT inequalities) 
{
  bool first = true;
  for (ineqADT curineq=inequalities;curineq;curineq=curineq->next) {
    if ((curineq->type>=5) && (curineq->type<=7)) {
      if (!first) fprintf(fp,",");
      else {
	fprintf(fp,"<");
	first=false;
      }
    }
    if (curineq->type==5) {
      fprintf(fp,"%s:=",events[curineq->place]->event);
      if (curineq->expr) {
	fprintf(fp,"%s",curineq->expr);
      } else {
	if (curineq->constant == curineq->uconstant)
	  fprintf(fp,"%d",curineq->constant);
	else
	  fprintf(fp,"[%d,%d]",curineq->constant,curineq->uconstant);
      }
    } else if (curineq->type==6) {
      fprintf(fp,"%s'dot:=",events[curineq->place]->event);
      if (curineq->expr) {
	fprintf(fp,"%s",curineq->expr);
      } else {
	if (curineq->constant == curineq->uconstant)
	  fprintf(fp,"%d",curineq->constant);
	else
	  fprintf(fp,"[%d,%d]",curineq->constant,curineq->uconstant);
      }
    } else if (curineq->type==7) {
      fprintf(fp,"%s:=",signals[curineq->place]->name);
      if (curineq->expr) {
	fprintf(fp,"%s",curineq->expr);
      } else {
	if (curineq->constant==TRUE) {
	  fprintf(fp,"T");
	} else {
	  fprintf(fp,"F");
	}
      }
    }
  }
  if (!first) fprintf(fp,">");
}

/*****************************************************************************/
/* Print an LPN using parseGraph by Tom Rokicki.                            */
/*****************************************************************************/

void dot_print_lpn(char * filename,signalADT *signals,
		   eventADT *events,ruleADT **rules,
		   int nevents,int nplaces,bool display,markingADT marking,
		   bool use_color,bool noparg,double *var)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int size;

  size=nevents+nplaces;
  strcpy(outname,filename);
  strcat(outname,".dot");
  printf("Storing LPN to:  %s\n",outname);
  fprintf(lg,"Storing LPN to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  fprintf(fp,"digraph G {\n");
  fprintf(fp,"size=\"7.5,10\"\n");
  for (int i=1;i<nevents;i++)
    if (!events[i]->dropped)
      if (events[i]->type & CONT) {
	fprintf(fp,"T%d [shape=box,label=\"%s=%d",i,events[i]->event,
		events[i]->rate);
	if (events[i]->inequalities) {
	  fprintf(fp,"\\n");
	  print_inequalities(fp,events,events[i]->inequalities);
	} 
	fprintf(fp,"\"];\n");
      } else {
	fprintf(fp,"T%d [shape=plaintext,label=\"%s\\n",i,events[i]->event);
	if (events[i]->hsl)
	  fprintf(fp,"{%s}\\n",events[i]->hsl);
	if (events[i]->priorityExpr) {
	  fprintf(fp,"(%s)\\n",events[i]->priorityExpr);
	}
	if (events[i]->transRate) {
	  fprintf(fp,"(%s)\\n",events[i]->transRate);
	} else if (events[i]->delayExpr) {
	  fprintf(fp,"[%s]\\n",events[i]->delayExpr);
	} else {
	  if ((events[i]->lower != 0)||(events[i]->upper != INFIN)) {
	    if (events[i]->upper != INFIN) {
	      fprintf(fp,"[%d,%d]\\n",events[i]->lower,events[i]->upper);
	    } else {
	      fprintf(fp,"[%d,inf]\\n",events[i]->lower);
	    }
	  }
	}
	if (events[i]->inequalities) {
	  print_assignments(fp,signals,events,events[i]->inequalities);
	} 
	fprintf(fp,"\"");
	if (events[i]->failtrans) {
	  fprintf(fp,",fontcolor=\"red\"");
	}
	fprintf(fp,"];\n");
      }
  int p=0;
  for (int i=1;i<nevents;i++)
    for (int j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype) && 
	  ((!use_color) || 
	  (/*(events[i]->color==2) &&*/ (events[j]->color > 0)))) {
	if ((marking && marking->marking[rules[i][j]->marking]=='F')||
	    (!marking && rules[i][j]->epsilon==0)) {
	  fprintf(fp,"T%d -> T%d",i,j);
	  print_rule_label(fp,events[i]->type,events[j]->type,
			   rules[i][j]->lower,rules[i][j]->upper,
			   rules[i][j]->plower,rules[i][j]->pupper,
			   rules[i][j]->weight,
			   rules[i][j]->expr,false,events[i]->event,
			   events[j]->event,true);
	} else if ((marking && marking->marking[rules[i][j]->marking]!='F')||
		   (!marking && rules[i][j]->epsilon==1)) {
	  fprintf(fp,"P%d",p);
	  print_rule_label(fp,events[i]->type,events[j]->type,
			   rules[i][j]->lower,rules[i][j]->upper,
			   rules[i][j]->plower,rules[i][j]->pupper,
			   rules[i][j]->weight,
			   rules[i][j]->expr,true,events[i]->event,
			   events[j]->event,true);
	  fprintf(fp,"P%d [shape=circle,width=0.30,height=0.30]\n",p);
	  fprintf(fp,"P%d [height=.15,width=.15,peripheries=2,",p);
	  fprintf(fp,"style=filled,color=black,fontcolor=white];\n");
	  fprintf(fp,"T%d -> P%d [dir=back];\n",j,p);
	  fprintf(fp,"P%d -> T%d [dir=back];\n",p,i);
	  p++;
	}
      }
  for (int i=nevents;i<size;i++) {
    bool first=TRUE;
    bool marked=false;
    for (int j=0;j<nevents;j++)
      if ((rules[i][j]->ruletype) && 
	  ((!use_color) || 
	  (/*(events[i]->color==2) && */(events[j]->color > 0)))) {
	if (first) {
	  if (!(events[i]->dropped)) {
	    if (events[i]->type & CONT) {
	      if (var)
		fprintf(fp,"P%d [label=\"%s=%g\"];\n",i,events[i]->event,
			var[i-nevents]);
	      else 
		fprintf(fp,"P%d [label=\"%s=%d\"];\n",i,events[i]->event,
			rules[i][j]->epsilon);
	     fprintf(fp,"P%d [shape=doublecircle,width=0.50,height=0.50]\n",i);
	    } else {
	      fprintf(fp,"P%d [label=\"%s",i,events[i]->event);
	      if (events[i]->inequalities) {
		fprintf(fp,"\\n");
		print_inequalities(fp,events,events[i]->inequalities);
	      } 
	      fprintf(fp,"\"];\n");
	      fprintf(fp,"P%d [shape=circle,width=0.40,height=0.40]\n",i);
	      if ((marking && marking->marking[rules[i][j]->marking]!='F')||
		  (!marking && rules[i][j]->epsilon!=0)) { 
		marked=true;
		fprintf(fp,"P%d [height=.3,width=.3,peripheries=2,",i);
		fprintf(fp,"style=filled,color=black,fontcolor=white];\n");
	      }
	    }
	  }
	  first=FALSE;
	}
	fprintf(fp,"P%d -> T%d",i,j);
	print_rule_label(fp,events[i]->type,events[j]->type,
			 rules[i][j]->lower,rules[i][j]->upper,
			 rules[i][j]->plower,rules[i][j]->pupper,
			 rules[i][j]->weight,
			 rules[i][j]->expr,marked,events[i]->event,
			 events[j]->event,true);
      }
    if (!(events[i]->dropped)) {
      if (first) {
	fprintf(fp,"T%d [label=\"%s\"];\n",i,events[i]->event);
	if (events[i]->type & CONT) 
	  fprintf(fp,"T%d [shape=doublecircle,width=0.40,height=0.40]\n",i);
	else
	  fprintf(fp,"T%d [shape=circle,width=0.40,height=0.40]\n",i);
      }
    }
  }
  for (int i=0;i<nevents;i++)
    for (int j=nevents;j<size;j++)
      if ((rules[i][j]->ruletype) && 
	  ((!use_color) || 
	  (/*(events[i]->color==2) &&*/ (events[j]->color > 0)))) {
	fprintf(fp,"T%d -> P%d",i,j);
	print_rule_label(fp,events[i]->type,events[j]->type,
			 rules[i][j]->lower,rules[i][j]->upper,
			 rules[i][j]->plower,rules[i][j]->pupper,
			 rules[i][j]->weight,
			 rules[i][j]->expr,false,events[i]->event,
			 events[j]->event,false);
      }
  fprintf(fp,"}\n");
  fclose(fp);
  if (display) {
    if (!noparg) {
      sprintf(shellcommand,"dotty %s.dot >& /dev/null &",filename);
      printf("Executing command:  %s\n",shellcommand);
      fprintf(lg,"Executing command:  %s\n",shellcommand);
      system(shellcommand);
    }
  } else {
    sprintf(shellcommand,"printdot %s",filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
  }
}

void parg_print_lpn(char * filename,eventADT *events,ruleADT **rules,
		    int nevents,int nplaces,bool display,markingADT marking,
		    bool use_color,bool noparg)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int size;

  size=nevents+nplaces;
  strcpy(outname,filename);
  strcat(outname,".prg");
  printf("Storing LPN to:  %s\n",outname);
  fprintf(lg,"Storing LPN to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  for (int i=0;i<nevents;i++)
    fprintf(fp,"N F T%d %s\n",i,events[i]->event);
  int p=0;
  for (int i=1;i<nevents;i++)
    for (int j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype) && 
	  ((!use_color) || 
	   (/*(events[i]->color==2) &&*/ (events[j]->color > 0)))) {
	if ((marking && marking->marking[rules[i][j]->marking]=='F')||
	    (!marking && rules[i][j]->epsilon==0)) {
	  if (rules[i][j]->upper==INFIN)
	    fprintf(fp,"N E P%d [%d,inf]",p,rules[i][j]->lower);
	  else 
	    fprintf(fp,"N E P%d [%d,%d]",p,rules[i][j]->lower,
		    rules[i][j]->upper);
	  if (rules[i][j]->expr)
	    fprintf(fp,"[%s]\n",rules[i][j]->expr);
	  else
	    fprintf(fp,"\n");
	  fprintf(fp,"E P T%d 0 P%d 0\n",i,p);
	  fprintf(fp,"E P P%d 0 T%d 0\n",p,j);
	  p++;
	} else if ((marking && marking->marking[rules[i][j]->marking]!='F')||
		   (!marking && rules[i][j]->epsilon==1)) {
	  if (rules[i][j]->upper==INFIN)
	    fprintf(fp,"N M P%d [%d,inf]",p,rules[i][j]->lower);
	  else
	    fprintf(fp,"N M P%d [%d,%d]",p,rules[i][j]->lower,
		    rules[i][j]->upper);
	  if (rules[i][j]->expr)
	    fprintf(fp,"[%s]\n",rules[i][j]->expr);
	  else
	    fprintf(fp,"\n");
	  fprintf(fp,"E P T%d 0 P%d 0\n",i,p);
	  fprintf(fp,"E P P%d 0 T%d 0\n",p,j);
	  p++;
	}
      }
  for (int i=nevents;i<size;i++) {
    bool first=TRUE;
    for (int j=0;j<nevents;j++)
      if ((rules[i][j]->ruletype) && 
	  ((!use_color) || 
	   (/*(events[i]->color==2) && */(events[j]->color > 0)))) {
	if (first) {
	  if ((marking && marking->marking[rules[i][j]->marking]=='F')||
	      (!marking && rules[i][j]->epsilon==0))
	    fprintf(fp,"N P T%d %s\n",i,events[i]->event);
	  else 
	    fprintf(fp,"N M T%d %s\n",i,events[i]->event);
	  first=FALSE;
	}
	/*if (rules[i][j]->lower > 0 || rules[i][j]->upper < INFIN ||
	    rules[i][j]->expr) {
	  if (rules[i][j]->upper==INFIN)
	    fprintf(fp,"N E P%d [%d,inf]",p,rules[i][j]->lower);
	  else 
	    fprintf(fp,"N E P%d [%d,%d]",p,rules[i][j]->lower,
		    rules[i][j]->upper);
	  if (rules[i][j]->expr)
	    fprintf(fp,"[%s]\n",rules[i][j]->expr);
	  else
	    fprintf(fp,"\n");
	  fprintf(fp,"E P T%d 0 P%d 0\n",i,p);
	  fprintf(fp,"E P P%d 0 T%d 0\n",p,j);
	  p++;
	  } else*/
	fprintf(fp,"E P T%d 0 T%d 0\n",i,j);
      }
    if (first)
      fprintf(fp,"N P T%d %s\n",i,events[i]->event);
  }
  for (int i=0;i<nevents;i++)
    for (int j=nevents;j<size;j++)
      if ((rules[i][j]->ruletype) && 
	  ((!use_color) || 
	   (/*(events[i]->color==2) &&*/ (events[j]->color > 0)))) {
	fprintf(fp,"E P T%d 0 T%d 0\n",i,j);
      }
  fprintf(fp,"ZZZZZ\n");
  fclose(fp);
  if (display) {
    if (!noparg) {
      sprintf(shellcommand,"parg %s.prg &",filename);
      printf("Executing command:  %s\n",shellcommand);
      fprintf(lg,"Executing command:  %s\n",shellcommand);
      system(shellcommand);
    }
  } else {
    sprintf(shellcommand,"pG %s",filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
  }
}

void print_lpn(char * filename,signalADT *signals,eventADT *events,
	       ruleADT **rules,
	       int nevents,int nplaces,bool display,markingADT marking,
	       bool use_color,bool noparg,bool use_dot,double *var)
{
  if (use_dot)
    dot_print_lpn(filename,signals,events,rules,nevents,nplaces,display,
		  marking,use_color,noparg,var);
  else
    parg_print_lpn(filename,events,rules,nevents,nplaces,display,marking,
		   use_color,noparg);
}
