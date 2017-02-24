/*****************************************************************************/
/* solvecsc.c                                                                */
/*****************************************************************************/

#include "solvecsc.h"

typedef struct sv_tag {
  int xplus;
  int xplus_type;
  int xminus;
  int xminus_type;
  int xplus1;
  int xplus1_type;
  int xminus1;
  int xminus1_type;
} *svADT;

/*****************************************************************************/
/* Determine what is the next new signal to add.                             */
/*****************************************************************************/

int find_next_new_signalI(eventADT *events,int nevents)
{
  int newsigs;
  char newsignal1[MAXTOKEN];
  char newsignal2[MAXTOKEN];

  newsigs=1;
  sprintf(newsignal1,"x%d+",newsigs);
  sprintf(newsignal2,"x%d+/1",newsigs);
  while ((which_event(nevents,events,newsignal1) != (-1)) ||
	 (which_event(nevents,events,newsignal2) != (-1))) {
    newsigs++;
    sprintf(newsignal1,"x%d+",newsigs);
    sprintf(newsignal2,"x%d+/1",newsigs);
  }
  return(newsigs);
}

/*****************************************************************************/
/* Print state variable insertion point being tried.                         */
/*****************************************************************************/

void print_sv_pt(signalADT *signals,int j, int k, int l, int m, int n, int o,
		 int p, int q, int newsigs)
{
  if (l==1) {
    printf("x%d+ before %s+; ",newsigs,signals[j]->name);
    fprintf(lg,"x%d+ before %s+; ",newsigs,signals[j]->name);
  } else if (l==2) {
    printf("x%d+ before %s-; ",newsigs,signals[j]->name);
    fprintf(lg,"x%d+ before %s-; ",newsigs,signals[j]->name);
  } else if (l==3) {
    printf("x%d+ after %s+; ",newsigs,signals[j]->name);
    fprintf(lg,"x%d+ after %s+; ",newsigs,signals[j]->name);
  } else if (l==4) {
    printf("x%d+ after %s-; ",newsigs,signals[j]->name);
    fprintf(lg,"x%d+ after %s-; ",newsigs,signals[j]->name);
  } else if ((l==5) && (o==1)) {
    printf("x%d+ before %s+ and %s+; ",newsigs,signals[j]->name,
	   signals[n]->name);
    fprintf(lg,"x%d+ before %s+ and %s+; ",newsigs,signals[j]->name,
	    signals[n]->name);
  } else if ((l==5) && (o==2)) {
    printf("x%d+ before %s+ and %s-; ",newsigs,signals[j]->name,
	   signals[n]->name);
    fprintf(lg,"x%d+ before %s+ and %s-; ",newsigs,signals[j]->name,
	    signals[n]->name);
  } else if ((l==6) && (o==1)) {
    printf("x%d+ before %s- and %s+; ",newsigs,signals[j]->name,
	   signals[n]->name);
    fprintf(lg,"x%d+ before %s- and %s+; ",newsigs,signals[j]->name,
	    signals[n]->name);
  } else if ((l==6) && (o==2)) {
    printf("x%d+ before %s- and %s-; ",newsigs,signals[j]->name,
	   signals[n]->name);
    fprintf(lg,"x%d+ before %s- and %s-; ",newsigs,signals[j]->name,
	    signals[n]->name);
  }
  if (m==1) {
    printf("x%d- before %s+ ... ",newsigs,signals[k]->name);
    fprintf(lg,"x%d- before %s+ ... ",newsigs,signals[k]->name);
  } else if (m==2) {
    printf("x%d- before %s- ... ",newsigs,signals[k]->name);
    fprintf(lg,"x%d- before %s- ... ",newsigs,signals[k]->name);
  } else if (m==3) {
    printf("x%d- after %s+ ... ",newsigs,signals[k]->name);
    fprintf(lg,"x%d- after %s+ ... ",newsigs,signals[k]->name);
  } else if (m==4) {
    printf("x%d- after %s- ... ",newsigs,signals[k]->name);
    fprintf(lg,"x%d- after %s- ... ",newsigs,signals[k]->name);
  } else if ((m==5) && (q==1)) {
    printf("x%d- before %s+ and %s+ ... ",newsigs,signals[k]->name,
	   signals[p]->name);
    fprintf(lg,"x%d- before %s+ and %s+ ... ",newsigs,signals[k]->name,
	    signals[p]->name);
  } else if ((m==5) && (q==2)) {
    printf("x%d- before %s+ and %s- ... ",newsigs,signals[k]->name,
	   signals[p]->name);
    fprintf(lg,"x%d- before %s+ and %s- ... ",newsigs,signals[k]->name,
	    signals[p]->name);
  } else if ((m==6) && (q==1)) {
    printf("x%d- before %s- and %s+ ... ",newsigs,signals[k]->name,
	   signals[p]->name);
    fprintf(lg,"x%d- before %s- and %s+ ... ",newsigs,signals[k]->name,
	    signals[p]->name);
  } else if ((m==6) && (q==2)) {
    printf("x%d- before %s- and %s- ... ",newsigs,signals[k]->name,
	   signals[p]->name);
    fprintf(lg,"x%d- before %s- and %s- ... ",newsigs,signals[k]->name,
	    signals[p]->name);
  }
}

/*****************************************************************************/
/* Find rising and falling regions for state variable.                       */
/*****************************************************************************/

bool find_sv_RF(signalADT *signals,stateADT *state_table,int nsigs,int ninpsig,
		int j,int k,int l,int m,int n,int o,int p,int q) 
{
  int i,z;
  stateADT curstate1;
  statelistADT curstate2;
  

  for (i=0;i<PRIME;i++) { 
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) { 
      if (((l==1) && (curstate1->state[j]=='R')) ||
	  ((l==2) && (curstate1->state[j]=='F')))
	curstate1->color=1;
      else if (((l==3) && (VAL(curstate1->state[j])=='1')) || 
	       ((l==4) && (VAL(curstate1->state[j])=='0'))) {
	curstate1->color=0;
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next))
	  if (((l==3) && (curstate2->stateptr->state[j]=='R')) ||
	      ((l==4) && (curstate2->stateptr->state[j]=='F')))
	    curstate1->color=1;
      } else if ((((l==5) && (curstate1->state[j]=='R')) ||
		  ((l==6) && (curstate1->state[j]=='F'))) &&
		 (((o==1) && (curstate1->state[n]=='R')) ||
		  ((o==2) && (curstate1->state[n]=='F')))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next))
	  if (((((l==5) && (curstate1->state[j]=='R')) ||
		((l==6) && (curstate1->state[j]=='F'))) &&
	       (!(((o==1) && (curstate1->state[n]=='R')) ||
		  ((o==2) && (curstate1->state[n]=='F'))))) ||
	      (!((((l==5) && (curstate1->state[j]=='R')) ||
		  ((l==6) && (curstate1->state[j]=='F')))) &&
	       (((o==1) && (curstate1->state[n]=='R')) ||
		((o==2) && (curstate1->state[n]=='F')))))
	    return FALSE;
	curstate1->color=1;
      } else if (((l==5) && (curstate1->state[j]=='R')) ||
		 ((l==6) && (curstate1->state[j]=='F'))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next)) {
	  if (((l==5) && (curstate2->stateptr->state[j]!='R')) ||
	      ((l==6) && (curstate2->stateptr->state[j]!='F')))
	    return FALSE;
	}
	curstate1->color=0;
      }	else if (((l==5) || (l==6)) && 
		 (((o==1) && (curstate1->state[n]=='R')) ||
		  ((o==2) && (curstate1->state[n]=='F')))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next)) {
	  if (((o==1) && (curstate2->stateptr->state[n]!='R')) ||
	      ((o==2) && (curstate2->stateptr->state[n]!='F'))) 
	    return FALSE;
	}
	curstate1->color=0;
      }	else curstate1->color=0;

      if ((curstate1->color==1) && ((l==3) || (l==4)))
	for (z=0;z<ninpsig;z++)
	  if ((curstate1->state[z]=='R') || (curstate1->state[z]=='F'))
	    return FALSE;

      if (((m==1) && (curstate1->state[k]=='R')) ||
	  ((m==2) && (curstate1->state[k]=='F')))
	if (curstate1->color==0) curstate1->color=3;
	else {
	  /* printf("INCONSISTENT 1.\n"); */
	  return FALSE;
	}		
      else if (((m==3) && (VAL(curstate1->state[k])=='1')) || 
	       ((m==4) && (VAL(curstate1->state[k])=='0'))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next))
	  if (((m==3) && (curstate2->stateptr->state[k]=='R')) ||
	      ((m==4) && (curstate2->stateptr->state[k]=='F')))
	    if (curstate1->color==0) curstate1->color=3;
	    else {
	      /* printf("INCONSISTENT 2.\n"); */
	      return FALSE;
	    }		
      } else if ((((m==5) && (curstate1->state[k]=='R')) ||
		  ((m==6) && (curstate1->state[k]=='F'))) &&
		 (((q==1) && (curstate1->state[p]=='R')) ||
		  ((q==2) && (curstate1->state[p]=='F')))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next))
	  if (((((m==5) && (curstate2->stateptr->state[k]=='R')) ||
		((m==6) && (curstate2->stateptr->state[k]=='F'))) &&
	       (!(((q==1) && (curstate2->stateptr->state[p]=='R')) ||
		  ((q==2) && (curstate2->stateptr->state[p]=='F'))))) ||
	      (!((((m==5) && (curstate2->stateptr->state[k]=='R')) ||
		  ((m==6) && (curstate2->stateptr->state[k]=='F')))) &&
	       (((q==1) && (curstate2->stateptr->state[p]=='R')) ||
		((q==2) && (curstate2->stateptr->state[p]=='F')))))
	    return FALSE;
	if (curstate1->color==0) curstate1->color=3;
	else {
	  return FALSE;
	}		
      } else if (((m==5) && (curstate1->state[k]=='R')) ||
		 ((m==6) && (curstate1->state[k]=='F'))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next)) {
	  if (((m==5) && (curstate2->stateptr->state[k]!='R')) ||
	      ((m==6) && (curstate2->stateptr->state[k]!='F')))
	    return FALSE;
	}
      }	else if (((m==5) || (m==6)) && 
		 (((q==1) && (curstate1->state[p]=='R')) ||
		  ((q==2) && (curstate1->state[p]=='F')))) {
	for (curstate2=curstate1->pred;
	     (curstate2 != NULL && curstate2->stateptr->state != NULL);
	     (curstate2=curstate2->next)) {
	  if (((q==1) && (curstate2->stateptr->state[p]!='R')) ||
	      ((q==2) && (curstate2->stateptr->state[p]!='F'))) 
	    return FALSE;
	}
      }		
      if ((curstate1->color==3) && ((m==3) || (m==4)))
	for (z=0;z<ninpsig;z++)
	  if ((curstate1->state[z]=='R') || (curstate1->state[z]=='F'))
	    return FALSE;
    }
  }
  return TRUE;
}

/*****************************************************************************/
/* Color successor states.                                                   */
/*****************************************************************************/

bool color_succ(stateADT *state_table,statelistADT curstate,
		int prevcolor,int nextcolor, int nsigs,int ninpsig)
{
  /*
  if (curstate) printf("1:%s:%d prev=%d next=%d\n",curstate->stateptr->state,
		       curstate->stateptr->color,prevcolor,nextcolor);
  */
  while (curstate != NULL && curstate->stateptr->state != NULL) {

    if (curstate->stateptr->color==(-1)) { 
    } else if ((curstate->stateptr->color==prevcolor) &&
	       (curstate->stateptr->color!=nextcolor)) {

      curstate->stateptr->color=(-1);

      if (!color_succ(state_table,curstate->stateptr->succ,prevcolor/*1*/,
		      nextcolor,nsigs,ninpsig)) return FALSE;

      curstate->stateptr->color=prevcolor;

    } else if (curstate->stateptr->color==0) {
      curstate->stateptr->color=nextcolor;
      if (!color_succ(state_table,curstate->stateptr->succ,nextcolor,
		      nextcolor,nsigs,ninpsig)) return FALSE;
      /*
    } else if (prevcolor==nextcolor) {
      return TRUE;
    } else if (curstate->stateptr->color==nextcolor) {
      return TRUE;
    } else if (curstate->stateptr->color==(nextcolor+1)) {
      return TRUE;
      */
    } else if ((curstate->stateptr->color==(nextcolor+2)) ||
	       (curstate->stateptr->color==(nextcolor-2)) ||
	       (curstate->stateptr->color==(nextcolor-1)))
    { 
      /*      printf("state %s color %d prevcolor %d nextcolor %d\n",
	     curstate->stateptr->state,curstate->stateptr->color,prevcolor,
	     nextcolor); */
      return FALSE; 
    }
    curstate=curstate->next;
  }
  return TRUE;
}

/*****************************************************************************/
/* Find queiscent regions for state variable.                                */
/*****************************************************************************/

bool find_sv_QS(signalADT *signals,stateADT *state_table,int nsigs,int ninpsig)
{
  int i;
  stateADT curstate1;
 
  for (i=0;i<PRIME;i++) {
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) {
      //      printf("0:%s\n",curstate1->state);
      if (curstate1->color==1) {
	if (!color_succ(state_table,curstate1->succ,1,2,nsigs,ninpsig)) {
	  /* printf("INCONSISTENT 4.\n"); */
	  return FALSE;
	}
      } else if (curstate1->color==3) {
	if (!color_succ(state_table,curstate1->succ,3,4,nsigs,ninpsig)) {
	  /* printf("INCONSISTENT 5.\n"); */
	  return FALSE;
	}
      }
    }
  }
  return TRUE;
}

/*****************************************************************************/
/* Check if two states have same code but different outputs are enabled.     */
/*****************************************************************************/

bool usc_violation(char* state1,char* state2,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if (VAL(state1[i]) != VAL(state2[i])) return(FALSE);
  return(TRUE);
}

/*****************************************************************************/
/* Find number of remaining CSC errors.                                      */
/*****************************************************************************/

int find_num_CSC_errors(signalADT *signals,stateADT *state_table,
			int nsigs,int ninpsig,int j,int k,int *uscerrors)
{
  int i,cscerrors;
  stateADT curstate1,curstate3;

  cscerrors=0;
  (*uscerrors)=0;
  for (i=0;i<PRIME;i++)
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) {
      for (curstate3=curstate1->link;
	   curstate3 != NULL && curstate3->state != NULL;
	   curstate3=curstate3->link) {
	if (!(((curstate1->color==2) && (curstate3->color==4)) ||
	      ((curstate1->color==4) && (curstate3->color==2)))) {
	  if (csc_violation(curstate1->state,curstate3->state,ninpsig,nsigs)) 
	    cscerrors++;
	  else if (((curstate1->color==1) && (curstate3->color==3)) ||
	      ((curstate1->color==3) && (curstate3->color==1)) ||
	      ((curstate1->color==4) && (curstate3->color==1)) ||
	      ((curstate1->color==1) && (curstate3->color==4)) ||
	      ((curstate1->color==3) && (curstate3->color==2)) ||
	      ((curstate1->color==2) && (curstate3->color==3)))
	    if (usc_violation(curstate1->state,curstate3->state,nsigs)) { 
	      cscerrors++;
	      (*uscerrors)++;
	    }
	}
      }
    }
  return(cscerrors);
}

/*****************************************************************************/
/* Find number of remaining CSC errors.                                      */
/*****************************************************************************/

int find_init_CSC_errors(signalADT *signals,stateADT *state_table,
			 int nsigs,int ninpsig)
{
  int i,cscerrors;
  stateADT curstate1,curstate3;

  cscerrors=0;
  for (i=0;i<PRIME;i++)
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) {
      for (curstate3=curstate1->link;
	   curstate3 != NULL && curstate3->state != NULL;
	   curstate3=curstate3->link) {
	if (csc_violation(curstate1->state,curstate3->state,ninpsig,nsigs)) 
	  cscerrors++;
      }
    }
  return(cscerrors);
}


/*****************************************************************************/
/* Print state graph coloring.                                               */
/*****************************************************************************/

void print_coloring(char* filename,signalADT *signals,stateADT *state_table,
		    int nsigs,int ninpsig,int j,int k,int l,int m)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i;
  stateADT curstate1;

  strcpy(outname,filename);
  sprintf(outname,"%s.%d_%d_%d_%d",filename,j,l,k,m);
  fp=Fopen(outname,"w");
  if (signals) 
    print_state_vector(fp,signals,nsigs,ninpsig);
  fprintf(fp,"STATE : Color\n");
  for (i=0;i<PRIME;i++)
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) 
      fprintf(fp,"%s : %d\n",curstate1->state,curstate1->color);
  fclose(fp);
}

/*****************************************************************************/
/* Partition state graph.                                                    */
/*****************************************************************************/

svADT partition_rsg(char* filename,signalADT *signals,stateADT *state_table,
		    regionADT *regions,int nsigs,int ninpsig,bool verbose,
		    int newsigs,bool useinputs)
{
  int j,k,l,m,n,o,p,q,initcsc,cscerrors;
  int startn,starto,startp,startq,initusc,uscerrors;
  svADT sv;

  printf("Finding state variable insertion points ... ");
  fprintf(lg,"Finding state variable insertion points ... ");
  fflush(stdout);
  initcsc=find_init_CSC_errors(signals,state_table,nsigs,ninpsig);
  printf("%d CSC errors initially.\n",initcsc);
  fprintf(lg,"%d CSC errors initially.\n",initcsc);
  initcsc--; 
  initusc=INFIN;
  sv=(svADT)GetBlock(sizeof(*sv));
  sv->xplus=(-1);
  for (j=0;j<nsigs;j++) 
    for (l=1;l<7;l++)
      for (k=j;k<nsigs;k++) 
	for (m=1;m<7;m++) {
	  if ((l==3 || l==4 || j >= ninpsig || useinputs) &&
	      (m==3 || m==4 || k >= ninpsig || useinputs)) {
	    if (l < 5) { startn=nsigs-1; starto=2;
	    } else { startn=j+1; starto=1; }
	    if (m < 5) { startp=nsigs-1; startq=2;
	    } else { startp=k+1; startq=1; }
	    for (n=startn;n<nsigs;n++) 
	      for (o=starto;o<3;o++)
		for (p=startp;p<nsigs;p++) 
		  for (q=startq;q<3;q++) {
		    if (find_sv_RF(signals,state_table,nsigs,ninpsig,j,k,l,m,
				   n,o,p,q)) {
		      if (find_sv_QS(signals,state_table,nsigs,ninpsig)) {
			if ((cscerrors=find_num_CSC_errors(signals,
							   state_table,nsigs,
							   ninpsig,j,k,
							   &uscerrors)) 
			    <= initcsc){
			  print_sv_pt(signals,j,k,l,m,n,o,p,q,newsigs); 
			  printf("%d CSC errors remain.\n",cscerrors);
			  fprintf(lg,"%d CSC errors remain.\n",cscerrors);
			  initcsc=cscerrors;
			  initusc=uscerrors;
			  sv->xplus=j;
			  sv->xplus_type=l;
			  sv->xminus=k;
			  sv->xminus_type=m;
			  sv->xplus1=n;
			  sv->xplus1_type=o;
			  sv->xminus1=p;
			  sv->xminus1_type=q;
			} else {
			  if (((cscerrors-uscerrors) < initcsc) ||
			      (((cscerrors-uscerrors)==initcsc) &&
			       (uscerrors <= initusc))) {
			    print_sv_pt(signals,j,k,l,m,n,o,p,q,newsigs); 
			    printf("%d CSC (%d USC) errors remain.\n",
				   cscerrors,uscerrors);
			    fprintf(lg,"%d CSC (%d USC) errors remain.\n",
				    cscerrors,uscerrors);
			    initcsc=cscerrors-uscerrors;
			    initusc=uscerrors;
			    sv->xplus=j;
			    sv->xplus_type=l;
			    sv->xminus=k;
			    sv->xminus_type=m;
			    sv->xplus1=n;
			    sv->xplus1_type=o;
			    sv->xminus1=p;
			    sv->xminus1_type=q;
			  } else {
			    printf ("Tried:");
			    print_sv_pt(signals,j,k,l,m,n,o,p,q,newsigs); 
			    printf("%d CSC (%d USC) errors remain.\n",
				   cscerrors,uscerrors);
			    fprintf(lg,"%d CSC (%d USC) errors remain.\n",
				    cscerrors,uscerrors);
			  }
			} 
		      } else {
			printf ("QS Prob");
			print_sv_pt(signals,j,k,l,m,n,o,p,q,newsigs); 
			printf("\n");
		      }
		    } else {
		      printf ("RF Prob");
		      print_sv_pt(signals,j,k,l,m,n,o,p,q,newsigs); 
		      printf("\n");
		    }
		  }
	  }
	}
  if (sv->xplus != (-1)) {
    find_sv_RF(signals,state_table,nsigs,ninpsig,sv->xplus,sv->xminus,
	       sv->xplus_type,sv->xminus_type,sv->xplus1,sv->xplus1_type,
	       sv->xminus1,sv->xminus1_type);
    find_sv_QS(signals,state_table,nsigs,ninpsig);
    /* print_coloring(filename,signals,state_table,nsigs,ninpsig,j,k,l,m); */
  }
  fflush(stdout);
  return sv;
}

/*****************************************************************************/
/* Output event list.                                                        */
/*****************************************************************************/

void output_eventsI(FILE *fpI,FILE *fpO,eventADT *events,
		   int nevents,int ninputs)
{
  int i;

  for (i=1;i<=ninputs;i+=2)
    fprintf(fpI,"%s %s\n",events[i]->event,events[i+1]->event);
  for (i=ninputs+1;i<nevents;i+=2)
    fprintf(fpO,"%s %s\n",events[i]->event,events[i+1]->event);
}

/*****************************************************************************/
/* Output merger list.                                                       */
/*****************************************************************************/

int output_mergersI(FILE *fpD,eventADT *events,mergeADT *merges,int nevents)
{
  int i;
  int ndisj;
  mergeADT curmerge;

  ndisj=0;
  for (i=1;i<nevents;i++) {
    for (curmerge=merges[i]; curmerge != NULL; curmerge=curmerge->link)
      if ((curmerge->mergetype==CONJMERGE) || 
	  (curmerge->mergetype==DISJMERGE)) 
	if (curmerge->mevent > i) {
	  fprintf(fpD,"%s %s\n",events[i]->event,
		  events[curmerge->mevent]->event);
	  ndisj++;
	}
  }
  return(ndisj);
}

/*****************************************************************************/
/* Create new signals which are to be added to breakup gates.                */
/*****************************************************************************/

int create_new_signalsI(FILE *fpO,FILE *fpD,signalADT *signals,
			eventADT *events,ruleADT **rules,markkeyADT *markkey,
			int nevents,int nrules,int *ndisj,
			char* startstate,int maxstack,regionADT *regions,
			int ninpsig,int nsignals,stateADT *state_table,
			svADT sv)
{
  int i,j,k;
  int newsigs,nnewsigs;
  int newnevents;
  char newsignal[MAXTOKEN];
  char newsignal2[MAXTOKEN];
  int initial;
  markingADT startmarking;
  stateADT e;
  char* state;
  int max;
  
  max=signals[sv->xminus]->maxoccurrence;
  if (signals[sv->xplus]->maxoccurrence > max)
    max=signals[sv->xplus]->maxoccurrence;
  if (sv->xplus_type >= 5) {
    if (signals[sv->xplus1]->maxoccurrence > max)
      max=signals[sv->xplus1]->maxoccurrence;
  }
  if (sv->xminus_type >= 5) {
    if (signals[sv->xminus1]->maxoccurrence > max)
      max=signals[sv->xminus1]->maxoccurrence;
  }

  nnewsigs=0;
  newsigs=find_next_new_signalI(events,nevents);
  newnevents=nevents;
  for (j=1;j<=max;j++) {
    sprintf(newsignal,"x%d+/%d",newsigs,j);
    fprintf(fpO,"%s ",newsignal);
    sprintf(newsignal,"x%d-/%d",newsigs,j);
    fprintf(fpO,"%s\n",newsignal);
  }
  if (startstate != NULL) {
    startmarking=find_initial_marking(events,rules,markkey,nevents,nrules,
				      ninpsig,nsignals,startstate,FALSE);
    initial=0;
    i=hashpjw(startstate);
    for (e = state_table[i];e->link != NULL;e = e->link){
      state=SVAL(e->state,nsignals); 
      if ((strcmp(startstate,state)==0) &&
	  ((!startmarking) || (strcmp(e->marking,startmarking->marking)==0))) {
	initial=e->color;
	if (initial !=0) break;
      }
      if (strcmp(startstate,state)==0)
	if (initial==0) initial=e->color;
    }
    free(startmarking);
    if ((initial==2) || (initial==3))
      startstate[nsignals+nnewsigs]='1';
    else
      startstate[nsignals+nnewsigs]='0';
    nnewsigs++;
    startstate[nsignals+nnewsigs]='\0';
  }
  newnevents=newnevents+(2*max);
  if (max > 1)
    for (j=1;j<max;j++) {
      if (startstate[i]=='1') sprintf(newsignal,"x%d-/%d",newsigs,j);
      else sprintf(newsignal,"x%d+/%d",newsigs,j);
      for (k=j+1;k<=signals[i]->maxoccurrence;k++) {
	if (startstate[i]=='1') sprintf(newsignal2,"x%d-/%d",newsigs,k);
	else sprintf(newsignal2,"x%d+/%d",newsigs,k);
	fprintf(fpD,"%s %s\n",newsignal,newsignal2);
	(*ndisj)++;
      }
    }
  newsigs++;
  return(newnevents);
}

/*****************************************************************************/
/* Output a causal rule.                                                     */
/*****************************************************************************/

int output_ruleI(FILE *fpR,char* enabling,char* enabled,int epsilon,
		 int lower,int upper,int resetlower,int resetupper,int nrules)
{
  fprintf(fpR,"%s %s %d %d ",enabling,enabled,epsilon,lower);
  printdelay(fpR,upper);
  fprintf(fpR,"\n");
  nrules++;
  if (epsilon==1) {
    fprintf(fpR,"reset %s 0 %d ",enabled,resetlower);
    printdelay(fpR,resetupper);
    fprintf(fpR,"\n");
    nrules++;
  }
  return(nrules);
}

/*****************************************************************************/
/* Output conflicts associated with a given decomposition.                   */
/*****************************************************************************/

int output_confI(FILE *fpC,int j,char* newvar,eventADT *events,
		 ruleADT **rules,int nevents,int nconf)
{
  int i;

  for (i=1;i<nevents;i++)
    if (rules[i][j]->conflict) {
      nconf++;
      fprintf(fpC,"%s %s\n",events[i]->event,newvar);
    }
  return(nconf);
}

/*****************************************************************************/
/* Check if enabling event is part of two variable insertion point.          */
/*****************************************************************************/

bool chk2var(eventADT *events,ruleADT **rules,int nevents,int i,int ph,int k)
{
  int j;

  ph=2-ph;
  for (j=1;j<nevents;j++)
    if ((rules[i][j]->ruletype) && (events[j]->signal==k) && (j%2==ph)) 
      return TRUE;
  return FALSE;
}

/*****************************************************************************/
/* Output rules associated with a given decomposition.                       */
/*****************************************************************************/

int output_rulesI(FILE *fpR,FILE *fpN,FILE *fpC,eventADT *events,
		  ruleADT **rules,int ninputs,int nevents,int *nord,
		  int *nconf,regionADT *regions,int maxgatedelay,
		  char* startstate,svADT sv)
{
  int i,j;
  int nrules;
  int newsigs;
  char newvarplus[MAXTOKEN];
  char newvarminus[MAXTOKEN];
  char newvarocc[MAXTOKEN];

  newsigs=find_next_new_signalI(events,nevents);
  /*
  for (j=1;j<=1;j++) {
    sprintf(newvarplus,"x%d+/%d",newsigs,j);
    sprintf(newvarminus,"x%d-/%d",newsigs,j);
  }
  */
  sprintf(newvarplus,"x%d+",newsigs);
  sprintf(newvarminus,"x%d-",newsigs);
  nrules=0;
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++) 
      if (rules[i][j]->ruletype) {
	if (((sv->xplus_type==1)&&(sv->xplus==events[j]->signal)&&(j%2==1))||
	    ((sv->xplus_type==2)&&(sv->xplus==events[j]->signal)&&(j%2==0))||
	    ((sv->xplus_type==3)&&(sv->xplus==events[i]->signal)&&(i%2==1))||
	    ((sv->xplus_type==4)&&(sv->xplus==events[i]->signal)&&(i%2==0))||

	    ((sv->xplus_type==5)&&
	     (((sv->xplus==events[j]->signal)&&(j%2==1)&&
	       (chk2var(events,rules,nevents,i,sv->xplus1_type,sv->xplus1)))||
             ((sv->xplus1_type==1)&&(sv->xplus1==events[j]->signal)&&
	      (j%2==1)&&(chk2var(events,rules,nevents,i,1,sv->xplus)))||
	      ((sv->xplus1_type==2)&&(sv->xplus1==events[j]->signal)&&
	       (j%2==0)&&(chk2var(events,rules,nevents,i,1,sv->xplus)))))||

	    ((sv->xplus_type==6)&&
	     (((sv->xplus==events[j]->signal)&&(j%2==0)&&
	       (chk2var(events,rules,nevents,i,sv->xplus1_type,sv->xplus1)))||
             ((sv->xplus1_type==1)&&(sv->xplus1==events[j]->signal)&&
	      (j%2==1)&&(chk2var(events,rules,nevents,i,2,sv->xplus)))||
	      ((sv->xplus1_type==2)&&(sv->xplus1==events[j]->signal)&&
	       (j%2==0)&&(chk2var(events,rules,nevents,i,2,sv->xplus)))))) {

	  if ((sv->xplus_type==3) || (sv->xplus_type==4))
	    sprintf(newvarocc,"%s%s",newvarplus,
		    strchr(events[i]->event,'/'));
	  else
	    sprintf(newvarocc,"%s%s",newvarplus,
		    strchr(events[j]->event,'/'));

	  if (j > ninputs) {
	    nrules=output_ruleI(fpR,events[i]->event,newvarocc,
				rules[i][j]->epsilon,rules[i][j]->lower,
				rules[i][j]->upper,rules[0][j]->lower,
				rules[0][j]->upper,nrules);
	    nrules=output_ruleI(fpR,newvarocc,events[j]->event,
				0,rules[i][j]->lower,
				rules[i][j]->upper,rules[0][j]->lower,
				rules[0][j]->upper,nrules);
	  } else {
	    nrules=output_ruleI(fpR,events[i]->event,newvarocc,
				rules[i][j]->epsilon,0,maxgatedelay,
				0,maxgatedelay,nrules);
	    if (rules[i][j]->ruletype & ORDERING)
	      (*nord)=output_ruleI(fpN,events[i]->event,events[j]->event,
				   rules[i][j]->epsilon,rules[i][j]->lower,
				   rules[i][j]->upper,rules[0][j]->lower,
				   rules[0][j]->upper,*nord);
	    else
	      nrules=output_ruleI(fpR,events[i]->event,events[j]->event,
				  rules[i][j]->epsilon,rules[i][j]->lower,
				  rules[i][j]->upper,rules[0][j]->lower,
				  rules[0][j]->upper,nrules);
	    (*nord)=output_ruleI(fpN,newvarocc,events[j]->event,0,0,0,0,0,
				 *nord);
	  } 
	  (*nconf)=output_confI(fpC,i,newvarocc,events,rules,nevents,*nconf);
	  (*nconf)=output_confI(fpC,j,newvarocc,events,rules,nevents,*nconf);
	} else if (((sv->xminus_type==1)&&(sv->xminus==events[j]->signal)&&
		    (j%2==1))||
	    ((sv->xminus_type==2)&&(sv->xminus==events[j]->signal)&&(j%2==0))||
	    ((sv->xminus_type==3)&&(sv->xminus==events[i]->signal)&&(i%2==1))||
	    ((sv->xminus_type==4)&&(sv->xminus==events[i]->signal)&&(i%2==0))||

	    ((sv->xminus_type==5)&&
	     (((sv->xminus==events[j]->signal)&&(j%2==1)&&
              (chk2var(events,rules,nevents,i,sv->xminus1_type,sv->xminus1)))||
             ((sv->xminus1_type==1)&&(sv->xminus1==events[j]->signal)&&
	      (j%2==1)&&(chk2var(events,rules,nevents,i,1,sv->xminus)))||
	      ((sv->xminus1_type==2)&&(sv->xminus1==events[j]->signal)&&
	       (j%2==0)&&(chk2var(events,rules,nevents,i,1,sv->xminus)))))||

	    ((sv->xminus_type==6)&&
	     (((sv->xminus==events[j]->signal)&&(j%2==0)&&
	      (chk2var(events,rules,nevents,i,sv->xminus1_type,sv->xminus1)))||
             ((sv->xminus1_type==1)&&(sv->xminus1==events[j]->signal)&&
	      (j%2==1)&&(chk2var(events,rules,nevents,i,2,sv->xminus)))||
	      ((sv->xminus1_type==2)&&(sv->xminus1==events[j]->signal)&&
	       (j%2==0)&&(chk2var(events,rules,nevents,i,2,sv->xminus)))))) {

	  if ((sv->xminus_type==3) || (sv->xminus_type==4))
	    sprintf(newvarocc,"%s%s",newvarminus,
		    strchr(events[i]->event,'/'));
	  else
	    sprintf(newvarocc,"%s%s",newvarminus,
		    strchr(events[j]->event,'/'));

	  if (j > ninputs) {
	    nrules=output_ruleI(fpR,events[i]->event,newvarocc,
				rules[i][j]->epsilon,rules[i][j]->lower,
				rules[i][j]->upper,rules[0][j]->lower,
				rules[0][j]->upper,nrules);
	    nrules=output_ruleI(fpR,newvarocc,events[j]->event,
				0,rules[i][j]->lower,
				rules[i][j]->upper,rules[0][j]->lower,
				rules[0][j]->upper,nrules);
	  } else {
	    nrules=output_ruleI(fpR,events[i]->event,newvarocc,
				rules[i][j]->epsilon,0,maxgatedelay,
				0,maxgatedelay,nrules);
	    if (rules[i][j]->ruletype & ORDERING)
	      (*nord)=output_ruleI(fpN,events[i]->event,events[j]->event,
				   rules[i][j]->epsilon,rules[i][j]->lower,
				   rules[i][j]->upper,rules[0][j]->lower,
				   rules[0][j]->upper,*nord);
	    else
	      nrules=output_ruleI(fpR,events[i]->event,events[j]->event,
				  rules[i][j]->epsilon,rules[i][j]->lower,
				  rules[i][j]->upper,rules[0][j]->lower,
				  rules[0][j]->upper,nrules);
	    (*nord)=output_ruleI(fpN,newvarocc,events[j]->event,0,0,0,0,0,
				 *nord);
	  } 
	  (*nconf)=output_confI(fpC,i,newvarocc,events,rules,nevents,*nconf);
	  (*nconf)=output_confI(fpC,j,newvarocc,events,rules,nevents,*nconf);
	} else 
	  if (rules[i][j]->ruletype & ORDERING)
	    (*nord)=output_ruleI(fpN,events[i]->event,events[j]->event,
				 rules[i][j]->epsilon,rules[i][j]->lower,
				 rules[i][j]->upper,rules[0][j]->lower,
				 rules[0][j]->upper,*nord);
	  else
	    nrules=output_ruleI(fpR,events[i]->event,events[j]->event,
				rules[i][j]->epsilon,rules[i][j]->lower,
				rules[i][j]->upper,rules[0][j]->lower,
				rules[0][j]->upper,nrules);
      }
  return nrules;
}

/*****************************************************************************/
/* Output conflicts associated with a given decomposition.                   */
/*****************************************************************************/

int output_conflictsI(FILE *fpC,eventADT *events,ruleADT **rules,
		      int nevents,int nconf)
{
  int i,j;

  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if (rules[i][j]->conflict) {
	nconf++;
	fprintf(fpC,"%s %s\n",events[i]->event,events[j]->event);
      }
  return(nconf);
}
 
/*****************************************************************************/
/* Output new timed ER structure with a state variable added.                */
/*****************************************************************************/

bool solve_csc(char* filename,signalADT *signals,eventADT *events,
	       mergeADT *merges,ruleADT **rules,cycleADT ****cycles,
	       markkeyADT *markkey,stateADT *state_table,
	       int nevents,int ncycles,int ninputs,int initnrules,int maxstack,
	       int maxgatedelay,char* startstate,bool initial,
	       regionADT *lastregions,int status,regionADT *regions,
	       bool manual,int ninpsig,int nsignals,bool verbose,
	       bool fromER,bool fromTEL)
{
  int nrules,nconf;
  static int ndisj;
  int nord;
  static int newnevents,oldnevents=0;
  char outname[FILENAMESIZE];
  FILE *fpI,*fpO,*fpR,*fpN,*fpD,*fpC;
  char* newstartstate=NULL;
  bool retval;
  svADT sv;
  int newsigs;

  newsigs=find_next_new_signalI(events,nevents);
  sv=partition_rsg(filename,signals,state_table,regions,nsignals,
		   ninpsig,verbose,newsigs,FALSE);
  if (sv->xplus==(-1)) {
    printf("None found so far.  Trying insertion points before inputs.\n");
    fprintf(lg,"None found so far.  Trying insertion points before inputs.\n");
    sv=partition_rsg(filename,signals,state_table,regions,nsignals,
		     ninpsig,verbose,newsigs,TRUE);
    if (sv->xplus==(-1)) {
      printf("EXCEPTION:  Unable to solve remaining CSC violations!\n");
      fprintf(lg,"EXCEPTION:  Unable to solve remaining CSC violations!\n");
      return FALSE;
    }
  }
  nord=0;
  nconf=0;
  retval=TRUE;
  /*
  if ((filename[strlen(filename)-2]!='S') ||
      (filename[strlen(filename)-1]!='V'))
    strcat(filename,"SV");
    */
  printf("Adding state variables ... ");
  fprintf(lg,"Adding state variables ... ");
  fflush(stdout);
  strcpy(outname,filename);
  strcat(outname,".inp");
  fpI=Fopen(outname,"w");
  strcpy(outname,filename);
  strcat(outname,".out");
  fpO=Fopen(outname,"w");
  strcpy(outname,filename);
  strcat(outname,".merg");
  fpD=Fopen(outname,"w");
  output_eventsI(fpI,fpO,events,nevents,ninputs);
  ndisj=output_mergersI(fpD,events,merges,nevents);
  oldnevents=nevents;
  if (newstartstate != NULL) {
    free(newstartstate);
    newstartstate=NULL;
  }
  if (startstate != NULL) {
    newstartstate=(char*)GetBlock(MAXSIGNALS);
    strcpy(newstartstate,startstate);
  }
  newnevents=create_new_signalsI(fpO,fpD,signals,events,rules,markkey,nevents,
				 initnrules,&ndisj,
				 newstartstate,maxstack,regions,ninpsig,
				 nsignals,state_table,sv);
  strcpy(outname,filename);
  strcat(outname,".rule");
  fpR=Fopen(outname,"w");
  strcpy(outname,filename);
  strcat(outname,".ord");
  fpN=Fopen(outname,"w");
  strcpy(outname,filename);
  strcat(outname,".conf");
  fpC=Fopen(outname,"w");
  nrules=output_rulesI(fpR,fpN,fpC,events,rules,ninputs,nevents,&nord,
		      &nconf,regions,maxgatedelay,startstate,sv); 
  nconf=output_conflictsI(fpC,events,rules,nevents,nconf);
  fclose(fpR);
  fclose(fpN);
  fclose(fpC);
  fclose(fpI);
  fclose(fpO);
  fclose(fpD);

  if ((fromER || fromTEL) && 
      ((strlen(filename) < 3) ||
       (filename[strlen(filename)-3]!='C') || 
       (filename[strlen(filename)-2]!='S') ||
       (filename[strlen(filename)-1]!='C')))
    strcat(filename,"CSC");
  make_er(filename,newnevents,ninputs,0,nrules,nord,ndisj,nconf,NULL,
	  newstartstate);
  return(retval);
}





