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
/* dotrsg.c                                                                  */
/*****************************************************************************/

#include "dotrsg.h"

/*****************************************************************************/
/* Print a reduced state graph using parseGraph by Tom Rokicki.              */
/*****************************************************************************/

void dot_print_rsg(char * filename,signalADT *signals,eventADT *events,
		   stateADT *state_table,int nsignals,int nevents,
		   bool display,bool stats,bool error,bool noparg)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  char colorvar[12];
  FILE *fp;
  int i;
  //int j;
  int edge,colornum,chtmp,initialize;
  stateADT curstate;
  statelistADT predstate,predstate2;
  bool empty;
  bool dummy;
  
  strcpy(outname,filename);
  strcat(outname,".dot");
  printf("Storing reduced state graph to:  %s\n",outname);
  fprintf(lg,"Storing reduced state graph to:  %s\n",outname);
  if(!(fp=fopen(outname,"w"))){
    fprintf(stderr, "ERROR:  Could not open %s for writing!\n",outname);
    fprintf(lg    , "ERROR:  Could not open %s for writing!\n",outname);
    problems++;
    return;
  }
  fprintf(fp,"digraph G {\n");
  fprintf(fp,"size=\"7.5,10\"\n");
  empty=TRUE;
  initialize=1; 
  chtmp=0;
  colornum=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) 
      if (!error || curstate->color >= 0) {
      empty=FALSE;
      if(curstate->highlight && initialize){
      	chtmp=curstate->highlight;
      	initialize=0;
      	}      
      if(curstate->highlight!=chtmp && curstate->highlight) colornum++;
      if(curstate->highlight){
      	if(colornum>14) colornum=0;
      	switch(colornum){
          case 0:
            strcpy(colorvar,"green");
            chtmp=curstate->highlight;
            break;
          case 1:
            strcpy(colorvar,"red");
            chtmp=curstate->highlight;
            break;
          case 2:
            strcpy(colorvar,"blue");
            chtmp=curstate->highlight;
            break;
          case 3:
            strcpy(colorvar,"orange");
            chtmp=curstate->highlight;
            break;
          case 4:          
            strcpy(colorvar,"brown");
            chtmp=curstate->highlight;
            break;
          case 5:          
            strcpy(colorvar,"purple");
            chtmp=curstate->highlight;
            break;
          case 6:                             
            strcpy(colorvar,"darkgreen");
            chtmp=curstate->highlight;
            break;
          case 7:                              
            strcpy(colorvar,"cyan");
            chtmp=curstate->highlight;
            break;
          case 8:                              
            strcpy(colorvar,"lightblue");
            chtmp=curstate->highlight;
            break;
          case 9:                              
            strcpy(colorvar,"pink");
            chtmp=curstate->highlight;
            break;
          case 10:                               
            strcpy(colorvar,"yellow");
            chtmp=curstate->highlight;
            break;
          case 11:                 
            strcpy(colorvar,"magenta");
            chtmp=curstate->highlight;
            break;           
          case 12:                 
            strcpy(colorvar,"turquoise");
            chtmp=curstate->highlight;
            break;           
          case 13:                 
            strcpy(colorvar,"gray");
            chtmp=curstate->highlight;
            break;           
          case 14:                 
            strcpy(colorvar,"gold");
            chtmp=curstate->highlight;
            break;           
        }                                                                      
      }
      else strcpy(colorvar,"black");
      if(curstate->highlight) 
	if ( !stats ) 
	  fprintf(fp,
		  "s%d [shape=ellipse,style=filled,color=%s,label=\"%d:%s\"];\n",
		  curstate->number,colorvar,curstate->number,curstate->state);
	else
	  fprintf(fp,
		  "s%d [shape=ellipse,style=filled,color=%s,label=\"%d:%s:%1.3e\"];\n",
		  curstate->number,
		  colorvar,
		  curstate->number,
		  curstate->state, 
		  curstate->probability);
      else 
	if ( !stats )
	  fprintf(fp,"s%d [shape=ellipse,color=%s,label=\"%d:%s\"];\n",
		  curstate->number,colorvar,curstate->number,curstate->state);
	else {
	  if (curstate->probability > 0.001) strcpy(colorvar,"red");
	  fprintf(fp,"s%d [shape=ellipse,color=%s,label=\"%d:%s:%1.3e\"];\n",
		  curstate->number,
		  colorvar,
		  curstate->number,
		  curstate->state,
		  curstate->probability);
	}
    }
  if (empty) {
    printf("ERROR:  No states in reduced state graph!\n");
    fprintf(lg,"ERROR:  No states in reduced state graph!\n");
    return;
  }
  edge=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
      if (!error || curstate->color>=0)
	for (predstate=curstate->succ;predstate;
	     predstate=predstate->next) 
	  if ((!error || ((predstate->stateptr->color>=0) &&
			  (predstate->stateptr->number>curstate->number))) &&
	      (curstate != predstate->stateptr)) {
	    dummy=TRUE;
	    /*
	    for (j=0;j<nsignals;j++) 
	      if ((predstate->stateptr->state[j]=='R') && 
		  (VAL(curstate->state[j])=='1')) {
		dummy=FALSE;
		if ( !stats )
		  fprintf(fp,"s%d -> s%d [label=\"%s+\"];\n",curstate->number,predstate->stateptr->number,signals[j]->name);
		else
		  fprintf(fp,
			  "s%d -> s%d [label=\"%s+:%1.3f\"];\n",
			  curstate->number,
			  predstate->stateptr->number,
			  signals[j]->name,
			  predstate->probability);
		edge++;
	      } else if ((predstate->stateptr->state[j]=='F') && 
			 (VAL(curstate->state[j])=='0')) {
		dummy=FALSE;
		if ( !stats )
		  fprintf(fp,"s%d -> s%d [label=\"%s-\"];\n",curstate->number,predstate->stateptr->number,signals[j]->name);
		else
		  fprintf(fp,
			  "s%d -> s%d [label=\"%s-:%1.3f\"];\n",
			  curstate->number,
			  predstate->stateptr->number,
			  signals[j]->name,
			  predstate->probability);
		edge++;
	      }
	    */
	    if (dummy) {
	      if ( !stats ) {
		bool first=true;
		for (predstate2=curstate->succ;predstate2;
		     predstate2=predstate2->next) 
		  {
		    if (predstate2->stateptr==predstate->stateptr) {
		      if ((first) && (predstate==predstate2)) {		
			if ((predstate->enabled >= 0) && 
			    (predstate->enabled < nevents))
			  fprintf(fp,"s%d -> s%d [label=\"%s\"];\n",
				  curstate->number,predstate->stateptr->number,
				  events[predstate->enabled]->event);
			else if (predstate->enabled < 0)
			  fprintf(fp,"s%d -> s%d [label=\"$\"];\n",
				  curstate->number,
				  predstate->stateptr->number);
			else 
			  fprintf(fp,"s%d -> s%d [label=\"DEAD\"];\n",
				  curstate->number,
				  predstate->stateptr->number);
			first=false;
		      } else if (first) {
			break;
		      }
		    }
		  }
	      }
	      else {
		if (1/*predstate->probability > 0.001*/) {
		  if ((predstate->enabled >= 0) &&
		      (predstate->enabled < nevents)) {
		    if (curstate->number <= predstate->stateptr->number) {
		      fprintf(fp,"s%d -> s%d [label=\"%s:%1.3f\"];\n",
			      curstate->number,
			      predstate->stateptr->number,
			      events[predstate->enabled]->event,
			      predstate->probability);
		    } else {
		      fprintf(fp,"s%d -> s%d [dir=back,label=\"%s:%1.3f\"];\n",
			      predstate->stateptr->number,
			      curstate->number,
			      events[predstate->enabled]->event,
			      predstate->probability);
		    }
		  } else if (predstate->enabled < 0)
		    fprintf(fp,"s%d -> s%d [label=\"$:%1.3f\"];\n",
			    curstate->number,
			    predstate->stateptr->number,
			    predstate->probability);
		  else
		    fprintf(fp,"s%d -> s%d [label=\"DEAD:%1.3f\"];\n",
			    curstate->number,
			    predstate->stateptr->number,
			    predstate->probability);
		  edge++;
		}
	      }
	    }
	  }
  fprintf(fp,"}");
  fclose(fp);
  if (!noparg) {
    if (display) {
      sprintf(shellcommand,"dotty %s.dot &",filename);
      printf("Executing command:  %s\n",shellcommand);
      fprintf(lg,"Executing command:  %s\n",shellcommand);
    } else {
      sprintf(shellcommand,"printdot %s",filename);
      printf("Executing command:  %s\n",shellcommand);
      fprintf(lg,"Executing command:  %s\n",shellcommand);
    }
    system(shellcommand);
  }
}

bool inlist(int *statelist,int size,int state) {
  for (int i=0;i<size;i++)
    if (statelist[i]==state) return true;
  return false;
}

/*****************************************************************************/
/* Print a reduced state graph using parseGraph by Tom Rokicki.              */
/*****************************************************************************/

void trans_dot_print_rsg(char * filename,signalADT *signals,eventADT *events,
			 stateADT *state_table,int nsignals,int nevents,
			 bool display,bool stats,bool error,bool noparg)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  char colorvar[12];
  FILE *fp;
  int i;
  //int j;
  int edge,colornum,chtmp,initialize;
  stateADT curstate;
  statelistADT predstate,predstate2;
  bool empty;
  bool dummy;

  int statelist[20]={0,2,24,1,6,12,53,55};
  int size=8;
  scanf("%d",&size);
  
  strcpy(outname,filename);
  strcat(outname,".dot");
  printf("Storing reduced state graph to:  %s\n",outname);
  fprintf(lg,"Storing reduced state graph to:  %s\n",outname);
  if(!(fp=fopen(outname,"w"))){
    fprintf(stderr, "ERROR:  Could not open %s for writing!\n",outname);
    fprintf(lg    , "ERROR:  Could not open %s for writing!\n",outname);
    problems++;
    return;
  }
  fprintf(fp,"digraph G {\n");
  fprintf(fp,"size=\"7.5,10\"\n");
  fprintf(fp,"rankdir=LR;\n");
  empty=TRUE;
  initialize=1; 
  chtmp=0;
  colornum=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      if ((!error || curstate->color >= 0)&&
	  (inlist(statelist,size,curstate->number))) {
	empty=FALSE;
	if(curstate->highlight && initialize){
	  chtmp=curstate->highlight;
	  initialize=0;
      	}      
	if(curstate->highlight!=chtmp && curstate->highlight) colornum++;
	if(curstate->highlight){
	  if(colornum>14) colornum=0;
	  switch(colornum){
          case 0:
            strcpy(colorvar,"green");
            chtmp=curstate->highlight;
            break;
          case 1:
            strcpy(colorvar,"red");
            chtmp=curstate->highlight;
            break;
          case 2:
            strcpy(colorvar,"blue");
            chtmp=curstate->highlight;
            break;
          case 3:
            strcpy(colorvar,"orange");
            chtmp=curstate->highlight;
            break;
          case 4:          
            strcpy(colorvar,"brown");
            chtmp=curstate->highlight;
            break;
          case 5:          
            strcpy(colorvar,"purple");
            chtmp=curstate->highlight;
            break;
          case 6:                             
            strcpy(colorvar,"darkgreen");
            chtmp=curstate->highlight;
            break;
          case 7:                              
            strcpy(colorvar,"cyan");
            chtmp=curstate->highlight;
            break;
          case 8:                              
            strcpy(colorvar,"lightblue");
            chtmp=curstate->highlight;
            break;
          case 9:                              
            strcpy(colorvar,"pink");
            chtmp=curstate->highlight;
            break;
          case 10:                               
            strcpy(colorvar,"yellow");
            chtmp=curstate->highlight;
            break;
          case 11:                 
            strcpy(colorvar,"magenta");
            chtmp=curstate->highlight;
            break;           
          case 12:                 
            strcpy(colorvar,"turquoise");
            chtmp=curstate->highlight;
            break;           
          case 13:                 
            strcpy(colorvar,"gray");
            chtmp=curstate->highlight;
            break;           
          case 14:                 
            strcpy(colorvar,"gold");
            chtmp=curstate->highlight;
            break;           
	  }
	}
	else strcpy(colorvar,"black");
	fprintf(fp,"s%d [shape=circle,style=filled,height=0.25,label=\"\"];\n",
		curstate->number);
      }
    }
  if (empty) {
    printf("ERROR:  No states in reduced state graph!\n");
    fprintf(lg,"ERROR:  No states in reduced state graph!\n");
    return;
  }
  edge=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) {
      if ((!error || curstate->color>=0)&&
	  (inlist(statelist,size,curstate->number))) {
	for (predstate=curstate->succ;predstate;
	     predstate=predstate->next) 
	  if ((!error || ((predstate->stateptr->color>=0) &&
			  (predstate->stateptr->number>curstate->number)))&&
	      (inlist(statelist,size,predstate->stateptr->number))) {
	    dummy=TRUE;
	    if (dummy) {
	      if ( !stats ) {
		bool first=true;
		for (predstate2=curstate->succ;predstate2;
		     predstate2=predstate2->next) 
		  {
		    if (predstate2->stateptr==predstate->stateptr) {
		      if ((first) && (predstate==predstate2)) {		
			if ((predstate->enabled >= 0) && 
			    (predstate->enabled < nevents))
			  fprintf(fp,"s%d -> s%d [label=\"%s\"];\n",
				  curstate->number,predstate->stateptr->number,
				  events[predstate->enabled]->event);
			else if (predstate->enabled < 0)
			  fprintf(fp,"s%d -> s%d [label=\"$\"];\n",
				  curstate->number,
				  predstate->stateptr->number);
			else 
			  fprintf(fp,"s%d -> s%d [label=\"DEAD\"];\n",
				  curstate->number,
				  predstate->stateptr->number);
			first=false;
		      } else if (first) {
			break;
		      }
		    }
		  }
	      } else {
		if (1/*predstate->probability > 0.001*/) {
		  if ((predstate->enabled >= 0) &&
		      (predstate->enabled < nevents)) {
		    if (curstate->number <= predstate->stateptr->number) {
		      fprintf(fp,"s%d -> s%d [label=\"%s/%2.2f\%\"];\n",
			      curstate->number,
			      predstate->stateptr->number,
			      events[predstate->enabled]->event,
			      predstate->probability*100.0);
		    } else {
		      fprintf(fp,"s%d -> s%d [dir=back,label=\"%s/%2.2f\%\"];\n",
			      predstate->stateptr->number,
			      curstate->number,
			      events[predstate->enabled]->event,
			      predstate->probability*100.0);
		    }
		  } else if (predstate->enabled < 0)
		    fprintf(fp,"s%d -> s%d [label=\"$/%2.2f\%\"];\n",
			    curstate->number,
			    predstate->stateptr->number,
			    predstate->probability*100.0);
		  else
		    fprintf(fp,"s%d -> s%d [label=\"DEAD/%2.2f\%\"];\n",
			    curstate->number,
			    predstate->stateptr->number,
			    predstate->probability*100.0);
		  edge++;
		}
	      }
	    }
	  }
      }
    }
  fprintf(fp,"}");
  fclose(fp);
  if (!noparg) {
    if (display) {
      sprintf(shellcommand,"dotty %s.dot &",filename);
      printf("Executing command:  %s\n",shellcommand);
      fprintf(lg,"Executing command:  %s\n",shellcommand);
    } else {
      sprintf(shellcommand,"printdot %s",filename);
      printf("Executing command:  %s\n",shellcommand);
      fprintf(lg,"Executing command:  %s\n",shellcommand);
    }
    system(shellcommand);
  }
}

