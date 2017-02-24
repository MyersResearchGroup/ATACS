/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2003 by, Scott R. Little                                  */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/

#include "displayADT.h"
#include "color.h"
#include <set>

void printRuleADT(ruleADT **rules,int nevents,int nplaces,eventADT *events) 
{
  int i,j;

  cSetFg(GREEN);
  printf("ruleADT...\n");
  cSetAttr(NONE);
  
  /* print out general stats and information about the format */
  printf("nevents: %d -- nplaces: %d\n",nevents,nplaces);
  printf("The format for each rule is stacked in the following manner: [lower,upper], ruletype, marking, level, [plower,pupper], weight,rate.\n");
  
  /* print out the columns */
  for(j=0;j<nevents+nplaces;j++) {
    printf("\t%d",j);
  }
  printf("\n");
  for(j=0;j<nevents+nplaces;j++) {
    printf("\t%s",events[j]->event);
  }
  printf("\n");
  
  for(i=0;i<nevents+nplaces;i++) {
    printf("%s",events[i]->event);
    cSetFg(BLUE);
    for(j=0;j<nevents+nplaces;j++) {
      if(rules[i][j]->upper == INFIN) {
        printf("\t[%d,U]",rules[i][j]->lower);
      }
      else {
        printf("\t[%d,%d]",rules[i][j]->lower,rules[i][j]->upper);
      }
    }
    cSetAttr(NONE);
    printf("\n");
    for(j=0;j<nevents+nplaces;j++) {
      printf("\t%d",rules[i][j]->ruletype);
    }
    printf("\n");
    for(j=0;j<nevents+nplaces;j++) {
      printf("\t%d",rules[i][j]->marking);
    }
    printf("\n");
    for(j=0;j<nevents+nplaces;j++) {
      printf("\t");
      printLevelExp(rules[i][j]->level);
    }
    printf("\n");
    cSetFg(YELLOW);
    for(j=0;j<nevents+nplaces;j++) {
      if(rules[i][j]->pupper == INFIN) {
        printf("\t[%d,U]",rules[i][j]->plower);
      }
      else {
        printf("\t[%d,%d]",rules[i][j]->plower,rules[i][j]->pupper);
      }
    }
    cSetAttr(NONE);
    printf("\n");
    for(j=0;j<nevents+nplaces;j++) {
      printf("\t%d,%.1f",rules[i][j]->weight,rules[i][j]->rate);
    }
    printf("\n");
  }
  printf("\n");
}

void printMarkingADT(markingADT marking,int nevents,int nrules,
                     eventADT *events)
{
  int i;

  cSetFg(GREEN);
  printf("markingADT...\n");
  cSetAttr(NONE);
  
  printf("nrules: %d\n",nrules);
  printf("marking: ");
  for(i=0;i<nrules;i++) {
    printf("%d:%c ",i,marking->marking[i]);
  }
  printf("\n");
  printf("nevents: %d\n",nevents);
  printf("enablings: ");
  for(i=0;i<nevents;i++) {
    printf("%s:%c ",events[i]->event,marking->enablings[i]);
  }
  printf("\n");
}

void printMarkingADT(dMarkingADT marking,int nevents,int nrules,
                     eventADT *events)
{
  int i;

  cSetFg(GREEN);
  printf("dMarkingADT...\n");
  cSetAttr(NONE);
  
  printf("nrules: %d\n",nrules);
  printf("marking: ");
  for(i=0;i<nrules;i++) {
    printf("%d:%c ",i,marking->marking[i]);
  }
  printf("\n");
  printf("nevents: %d\n",nevents);
  printf("enablings: ");
  for(i=0;i<nevents;i++) {
    printf("%s:%c ",events[i]->event,marking->enablings[i]);
  }
  printf("\n");
}

void printConciseMarkingADT(dMarkingADT marking,int nevents,int nrules,
                            int nsignals,signalADT *signals,eventADT *events,
			    markkeyADT *markkey)
{
  int i;

  cSetFg(GREEN);
  printf("State: ");
  cSetAttr(NONE);

  set<char *> sigs;
  for(i=0;i<nsignals;i++)
    if(marking->state[i] == '1')
      printf("%s ",signals[i]->name);
  //sigs.insert(signals[i]->name);

  /*   for(set<char *>::iterator i=sigs.begin();i != sigs.end();i++) { */
  /*     printf("%s ",*i); */
  /*   } */
  printf("\n");

  printf("%s\n",marking->state);
  
  cSetFg(GREEN);
  printf("Marking: ");
  cSetAttr(NONE);

  set<char *> places;
  for(i=0;i<nrules;i++)
    if(marking->marking[i] == 'T') 
      places.insert(events[markkey[i]->enabling]->event);

  for(set<char *>::iterator i=places.begin();i != places.end();i++) {
    printf("%s ",*i);
  }
  printf("\n");

  cSetFg(GREEN);
  printf("Enabled transitions: ");
  cSetAttr(NONE);
      
  for(i=0;i<nevents;i++) {
    if(marking->enablings[i] == 'T') {
      printf("%s ",events[i]->event);
    }
  }
  printf("\n");
}

void printMarkkeyADT(markkeyADT *markkey,int nrules,eventADT *events)
{
  int i;

  cSetFg(GREEN);
  printf("markkeyADT...\n");
  cSetAttr(NONE);
  
  printf("Note: the order of printing is enabling, enabled, and epsilon.\n");
  printf("nrules: %d\n",nrules);
  for(i=0;i<nrules;i++) {
    printf("\t%d",i);
  }
  printf("\n");
  for(i=0;i<nrules;i++) {
    printf("\t%s",events[markkey[i]->enabling]->event);
  }
  printf("\n");
  for(i=0;i<nrules;i++) {
    printf("\t%s",events[markkey[i]->enabled]->event);
  }
  printf("\n");
    for(i=0;i<nrules;i++) {
    printf("\t%d",markkey[i]->epsilon);
  }
  printf("\n");
}

void printEventADT(eventADT *events,int nevents,int nplaces)
{
  int i;

  cSetFg(GREEN);
  printf("eventADT...\n");
  cSetAttr(NONE);
  
  printf("nevents: %d -- nplaces: %d\n",nevents,nplaces);
  printf("The order of printing is event, dropped, continuous, rate, and signal\n");
  for(i=0;i<nevents+nplaces;i++) {
    printf("\t%s",events[i]->event);
  }
  printf("\n");
  for(i=0;i<nevents+nplaces;i++) {
    printf("\t%d",events[i]->dropped);
  }
  printf("\n");
  for(i=0;i<nevents+nplaces;i++) {
    if(events[i]->type & CONT) {
      printf("\t1");
    }
    else {
      printf("\t0");
    }
  }
  printf("\n");
  for(i=0;i<nevents+nplaces;i++) {
    printf("\t%d",events[i]->rate);
  }
  printf("\n");  
  for(i=0;i<nevents+nplaces;i++) {
    printf("\t%d",events[i]->signal);
  }
  printf("\n");
}

void printVerboseEventADT(eventADT *events,int nevents,int nplaces,
                          signalADT *signals,int nsignals)
{
  cSetFg(GREEN);
  printf("eventADT...\n");
  cSetAttr(NONE);
  printf("nevents: %d -- nplaces: %d\n",nevents,nplaces);

  for(int i=0;i<nevents+nplaces;i++) {
    printf("%s: ",events[i]->event);
    if(events[i]->signal >= 0 && events[i]->signal < nsignals) {
      printf("sig:%s ",signals[events[i]->signal]->name);
    }
    else {
      printf("sig:%d ",events[i]->signal);
    }
    printf("lower: %d upper:%d rate:%d lrate:%d urate:%d linitr:%d uinitr:%d\n"
           ,events[i]->lower,events[i]->upper,events[i]->rate,events[i]->lrate,
           events[i]->urate,events[i]->linitrate,events[i]->uinitrate);
    printf("dropped: ");
    if(events[i]->dropped) {
      printf("T\n");
    }
    else {
      printf("F\n");
    }
    if(events[i]->hsl != NULL) {
      printf("hsl:%s\n",events[i]->hsl);
    }
    else {
      printf("hsl:NULL\n");
    }
    if(events[i]->SOP != NULL) {
      printf("SOP:");
      printLevelExp(events[i]->SOP);
      printf("\n");
    }
    else {
      printf("SOP:NULL\n");
    }
    if(events[i]->inequalities != NULL) {
      printIneqADT(events[i]->inequalities,events,signals);
    }
    else {
      printf("ineq:NULL\n");
    }    
  } 
}

void printVerboseEvent(eventADT *events,int t,signalADT *signals,int nsignals)
{

  printf("%s: ",events[t]->event);
  if(events[t]->signal >= 0 && events[t]->signal < nsignals) {
    printf("sig:%s ",signals[events[t]->signal]->name);
  }
  else {
    printf("sig:%d ",events[t]->signal);
  }
  printf("lower: %d upper:%d rate:%d lrate:%d urate:%d linitr:%d uinitr:%d\n"
         ,events[t]->lower,events[t]->upper,events[t]->rate,events[t]->lrate,
         events[t]->urate,events[t]->linitrate,events[t]->uinitrate);
  if(events[t]->hsl != NULL) {
    printf("hsl:%s\n",events[t]->hsl);
  }
  else {
    printf("hsl:NULL\n");
  }
  if(events[t]->inequalities != NULL) {
    printIneqADT(events[t]->inequalities,events,signals);
  }
  else {
    printf("ineq:NULL\n");
  }
}

void printSignalADT(signalADT *signals,int nsignals)
{
  int i;

  cSetFg(GREEN);
  printf("signalADT...\n");
  cSetAttr(NONE);
  
  printf("nsignals: %d\n",nsignals);
  printf("The order of printing is name, is_level, event, and maxoccurrence\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%s",signals[i]->name);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->is_level);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->event);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->maxoccurrence);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->riselower);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->riseupper);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->falllower);
  }
  printf("\n");
  for(i=0;i<nsignals;i++) {
    printf("\t%d",signals[i]->fallupper);
  }
  printf("\n");
}

void printClockkeyADT(clockkeyADT clockkey, int num)
{
  cSetFg(GREEN);
  printf("clockkeyADT...\n");
  cSetAttr(NONE);
  
  for(int i;i<num;i++) {
    printf("%d->%d, ",clockkey[i].enabling,clockkey[i].enabled);
  }
  printf("\n");
}

void printClocksADT(clocksADT clocks)
{
  cSetFg(GREEN);
  printf("clocksADT...\n");
  cSetAttr(NONE);
  
  printf("Not implemented.\n");
}

void printClocklistADT(clocklistADT cl,eventADT *events)
{
	if(cl == NULL) {
		printf("NULL\n");
		return;
	}
	printf("Rates: ");
	for(int i=0;i<cl->clocknum;i++) {
		if(cl->clockkey[i].enabling == -2) {
			printf("%s:[%d,%d] ",events[cl->clockkey[i].enabled]->event,cl->clockkey[i].lrate,cl->clockkey[i].urate);
		}
	}
	printf("\n");

	printf("Size:%d\n",cl->clocknum);
	printf("Transitions: ");
  for(int i=0;i<cl->clocknum;i++) {
    printf("%s ",events[cl->clockkey[i].enabled]->event);
  }
  printf("\n");
  
  for(int i=0;i<cl->clocknum;i++) {
      printf("\t%d",cl->clockkey[i].enabled);
  }
  printf("\n");

  for(int i=0;i<cl->clocknum;i++) {
    printf("%d",cl->clockkey[i].enabled);
    for(int j=0;j<cl->clocknum;j++) {
      if(cl->clocks[i][j] == INFIN) {
        printf("\tU");
      }
      else if(cl->clocks[i][j] == -1*INFIN) {
        printf("\t-U");
      }
      else {
        printf("\t%d",cl->clocks[i][j]);
      }
    }
    printf("\n");
  }
}

void fprintClocklistADT(FILE *fp,clocklistADT cl,eventADT *events)
{
	if(cl == NULL) {
		fprintf(fp,"NULL\n");
		return;
	}
	fprintf(fp,"Rates: ");
	for(int i=0;i<cl->clocknum;i++) {
		if(cl->clockkey[i].enabling == -2) {
			fprintf(fp,"%s:[%d,%d] ",events[cl->clockkey[i].enabled]->event,cl->clockkey[i].lrate,cl->clockkey[i].urate);
		}
	}
	fprintf(fp,"\n");

	fprintf(fp,"Size:%d\n",cl->clocknum);
	fprintf(fp,"Transitions: ");
  for(int i=0;i<cl->clocknum;i++) {
    fprintf(fp,"%s ",events[cl->clockkey[i].enabled]->event);
  }
  fprintf(fp,"\n");
  
  for(int i=0;i<cl->clocknum;i++) {
    fprintf(fp,"\t%d",cl->clockkey[i].enabled);
  }
  fprintf(fp,"\n");

  for(int i=0;i<cl->clocknum;i++) {
    fprintf(fp,"%d",cl->clockkey[i].enabled);
    for(int j=0;j<cl->clocknum;j++) {
      if(cl->clocks[i][j] == INFIN) {
        fprintf(fp,"\tU");
      }
      else if(cl->clocks[i][j] == -1*INFIN) {
        fprintf(fp,"\t-U");
      }
      else {
        fprintf(fp,"\t%d",cl->clocks[i][j]);
      }
    }
    fprintf(fp,"\n");
  }
}

void printClocklistADT(clocklistADT cl)
{
	if(cl == NULL) {
		printf("NULL\n");
		return;
	}
	printf("Rates: ");
	for(int i=0;i<cl->clocknum;i++) {
		if(cl->clockkey[i].enabling == -2) {
			printf("%d:[%d,%d] ",cl->clockkey[i].enabled,cl->clockkey[i].lrate,cl->clockkey[i].urate);
		}
	}
	printf("\n");

	printf("Size:%d\n",cl->clocknum);
	printf("Transitions: ");
  for(int i=0;i<cl->clocknum;i++) {
    printf("%d ",cl->clockkey[i].enabled);
  }
  printf("\n");
  
  for(int i=0;i<cl->clocknum;i++) {
      printf("\t%d",cl->clockkey[i].enabled);
  }
  printf("\n");

  for(int i=0;i<cl->clocknum;i++) {
    printf("%d",cl->clockkey[i].enabled);
    for(int j=0;j<cl->clocknum;j++) {
      if(cl->clocks[i][j] == INFIN) {
        printf("\tU");
      }
      else if(cl->clocks[i][j] == -1*INFIN) {
        printf("\t-U");
      }
      else {
        printf("\t%d",cl->clocks[i][j]);
      }
    }
    printf("\n");
  }
}

void printLevelExp(level_exp level) 
{
  int cnt = 0;
  while(level) {
    printf("%d:%s ",cnt,level->product);
    level = level->next;
    cnt++;
  }
}

void printCStateADT(cStateADT cState,eventADT *events,int nevents) 
{
  cSetFg(GREEN);
  printf("cStateADT...\n");
  cSetAttr(NONE);
  
/*   printf("cPlaces: %d\n",cState->cPlaces); */

/*   printf("rates: "); */
/*   for(int i=0;i<cState->cPlaces;i++) { */
/*     printf("%s:%d, ",events[cState->eIndex[i]]->event,cState->warp[i]); */
/*   } */
/*   printf("\n"); */

/*   printf("Velocity cache...\n"); */
/*   for(int i=0;i<nevents;i++){ */
/*     printf("%s:%d:",events[i]->event,cState->vCache[i]); */
/*     if(cState->vValid[i]) { */
/*       printf("V, "); */
/*     } */
/*     else { */
/*       printf("D, "); */
/*     } */
/*   } */
/*   printf("\n"); */
}

void printIneqADT(ineqADT ineq,eventADT *events,signalADT *signals)
{
  cSetFg(GREEN);
  printf("ineqADT...\n");
  cSetAttr(NONE);
  for(ineqADT i=ineq;i!=NULL;i=i->next) {
    if(i->type == 7) {
      printf("%s",signals[i->place]->name);
    }
    else {
      printf("%s",events[i->place]->event);
    }
    if(i->type == 0) {
      printf(" > ");
    }
    else if(i->type == 1) {
      printf(" >= ");
    }
    else if(i->type == 2) {
      printf(" < ");
    }
    else if(i->type == 3) {
      printf(" <= ");
    }
    else if(i->type == 4) {
      printf(" = ");
    }
    else if(i->type == 5 || i->type == 7) {
      printf(" := ");
    }
    else if(i->type == 6) {
      printf(" dot:= ");
    }
    if (i->expr) {
      printf("%s",i->expr);
    } else {
      if(i->type == 7) {
	if(i->constant == 0) {
	  printf("F");
	}
	else if(i->constant == 1) {
	  printf("T");
	}
	else {
	  printf("%d",i->constant);
	}
      }
      else {
	printf("[%d,%d]",i->constant,i->uconstant);
      }
    }
    printf(" signal:%d",i->signal);
    if(i->transition >= 0) {
      printf(" transition:%s",events[i->transition]->event);
    }
    else {
      printf(" transition:%d",(int)i->transition);
    }
    printf("\n");
  }
}

void printIneq(ineqADT i,eventADT *events,signalADT *signals)
{
  cSetFg(GREEN);
  printf("ineq: ");
  cSetAttr(NONE);
  if(i->type == 7) {
    printf("%s",signals[i->place]->name);
  }
  else {
    printf("%s",events[i->place]->event);
  }
  if(i->type == 0) {
    printf(" > ");
  }
  else if(i->type == 1) {
    printf(" >= ");
  }
  else if(i->type == 2) {
    printf(" < ");
  }
  else if(i->type == 3) {
    printf(" <= ");
  }
  else if(i->type == 4) {
    printf(" = ");
  }
  else if(i->type == 5 || i->type == 7) {
    printf(" := ");
  }
  else if(i->type == 6) {
    printf(" dot:= ");
  }
  if(i->type == 7) {
    if(i->constant == 0) {
      printf("F");
    }
    else if(i->constant == 1) {
      printf("T");
    }
    else {
      printf("%d",i->constant);
    }
  }
  else {
    printf("[%d,%d]",i->constant,i->uconstant);
  }
  printf(" signal:%d",i->signal);
  if(i->transition >= 0) {
    printf(" transition:%s",events[i->transition]->event);
  }
  else {
    printf(" transition:%d",(int)i->transition);
  }
  printf("\n");
}

void printIneqList(vector<ineqADT> ineqL,eventADT *events,signalADT *signals)
{
  cSetFg(GREEN);
  printf("ineqList...\n");
  cSetAttr(NONE);
  for(unsigned i=0;i<ineqL.size();i++) {
    if(ineqL[i]->type == 7) {
      printf("%s",signals[ineqL[i]->place]->name);
    }
    else {
      printf("%s",events[ineqL[i]->place]->event);
    }
    if(ineqL[i]->type == 0) {
      printf(" > ");
    }
    else if(ineqL[i]->type == 1) {
      printf(" >= ");
    }
    else if(ineqL[i]->type == 2) {
      printf(" < ");
    }
    else if(ineqL[i]->type == 3) {
      printf(" <= ");
    }
    else if(ineqL[i]->type == 4) {
      printf(" = ");
    }
    else if(ineqL[i]->type == 5 || ineqL[i]->type == 7) {
      printf(" := ");
    }
    else if(ineqL[i]->type == 6) {
      printf(" dot:= ");
    }
    else if(ineqL[i]->type == 7) {
      printf(" := ");
    }
    else if(ineqL[i]->type == 8) {
      printf("'dot > ");
    }
    else if(ineqL[i]->type == 9) {
      printf("'dot >= ");
    }
    else if(ineqL[i]->type == 10) {
      printf("'dot < ");
    }
    else if(ineqL[i]->type == 11) {
      printf("'dot <= ");
    }
    else {
      printf(" ? ");
    }

    if(ineqL[i]->type == 7) {
      printf("%c",ineqL[i]->constant);
    }
    else {
      printf("%d",ineqL[i]->constant);
    }
    printf(" signal:%d",ineqL[i]->signal);
    if(ineqL[i]->transition >= 0) {
      printf(" transition:%s",events[ineqL[i]->transition]->event);
    }
    else {
      printf(" transition:%d",(int)ineqL[i]->transition);
    }
    printf("\n");
  }
}
