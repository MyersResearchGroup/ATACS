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
/* loadrsg.c                                                                 */
/*****************************************************************************/

#include "loadrsg.h"
#include "findreg.h"
#include "def.h"
#include "misclib.h"
#include "memaloc.h"
#include "findrsg.h"

/*****************************************************************************/
/* Get a token from a file.  Used for loading reduced state graphs.          */
/*****************************************************************************/

int fgettokrsg(FILE *fp,char *tokvalue,int maxtok)

{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '%':
      if (!readword) {
        while (((c=getc(fp)) != EOF) && (c != '\n'));
        return(COMMENT);
      } else {
        ungetc('%',fp);
        return (WORD);
      }
      break;
    case '-':
      if (!readword)
	return '-';
      else {
	ungetc('-',fp);
	return (WORD);
      }
      break;
    case '+':
      if (!readword)
	return '+';
      else {
	ungetc('+',fp);
	return (WORD);
      }
      break;
    case '/':
      if (!readword)
	return '/';
      else {
	ungetc('/',fp);
	return (WORD);
      }
      break;
    case '>':
      if (!readword)
	return '>';
      else {
	ungetc('>',fp);
	return (WORD);
      }
      break;
    case ' ':
    case ':':
    case '\n':
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

/*****************************************************************************/
/* Get a word from a reduced state graph file.                               */
/*****************************************************************************/

bool get_word_rsg(FILE *fp,char * tokvalue)
{
  int token;

  while ((token=fgettokrsg(fp,tokvalue,MAXTOKEN)) != WORD) 
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      return(FALSE);
    }
  return(TRUE);
}

/*****************************************************************************/
/* Load header of reduced state graph file.                                  */
/*****************************************************************************/

bool load_rsg_header(char * filename,int *ninpsig,int *nsignals)
{ 
  char inname[FILENAMESIZE];
  char tokvalue[MAXTOKEN];
  FILE *fp;

  (*ninpsig)=0;
  (*nsignals)=0;
  strcpy(inname,filename);
  strcat(inname,".rsg");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE);
  }
  if (!get_word_rsg(fp,tokvalue)) return(FALSE); /* Read "SG" */
  if (!get_word_rsg(fp,tokvalue)) return(FALSE); /* Read "STATEVECTOR" */

  /* Read statevector */
  if (!get_word_rsg(fp,tokvalue)) return(FALSE);
  while (strcmp(tokvalue,"STATES")!=0) {
    if (strcmp(tokvalue,"INP")==0) {
      (*ninpsig)++;
      if (!get_word_rsg(fp,tokvalue)) return(FALSE);
    }
    (*nsignals)++;
    if (!get_word_rsg(fp,tokvalue)) return(FALSE);
  }
  fclose(fp);
  return(TRUE);
}

/*****************************************************************************/
/* Find state in state table, return NULL if not in table.                   */
/*****************************************************************************/

stateADT next_state_match(stateADT last_match,char * nextstate,int nsignals)
{
  int i;
  stateADT e;
  char * flatstate;

  flatstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  flatstate[nsignals]='\0';
  for (e = last_match; e; e = e->link) {
    if (e->state) {
      for (i=0;i<nsignals;i++)
	flatstate[i]=VAL(e->state[i]);
      if (strcmp(flatstate,nextstate)==0) {
	free(flatstate);
	return(e);
      }
    }
  }
  free(flatstate);
  return(NULL);
}

/*****************************************************************************/
/* Find state in state table, return NULL if not in table.                   */
/*****************************************************************************/

stateADT find_next_state(stateADT *state_table,char * nextstate,int nsignals)
{
  int position;

  position=hashpjw(nextstate);
  return(next_state_match(state_table[position],nextstate,nsignals));
}

/*****************************************************************************/
/* Load reduced state graph file.                                            */
/*****************************************************************************/

bool load_rsg(int status,char * filename,signalADT *signals,
	      stateADT *state_table,
	      int nsignals,char * *startstate, int nrules,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
	      designADT design, int ninpsig) 
{ 
  char inname[FILENAMESIZE];
  char tokvalue[MAXTOKEN];
  FILE *fp;
  int i,j,k,l,sn,states,trans;
  char * nextstate;
  stateADT curstate;
  stateADT prevstateptr,nextstateptr;
  statelistADT statelist;
  char * vstate;
  int token;

  strcpy(inname,filename);
  strcat(inname,".rsg");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE);
  }
  printf("Loading reduced state graph from:  %s\n",inname);
  fprintf(lg,"Loading reduced state graph from:  %s\n",inname);
  initialize_state_table(LOADED,FALSE,state_table);

  if (!get_word_rsg(fp,tokvalue)) return(FALSE); /* Read "SG" */
  if (!get_word_rsg(fp,tokvalue)) return(FALSE); /* Read "STATEVECTOR" */

  /* Read statevector */
  i=0;
  if (!get_word_rsg(fp,tokvalue)) return(FALSE);
  while (strcmp(tokvalue,"STATES")!=0) {
    if (strcmp(tokvalue,"INP")==0) 
      if (!get_word_rsg(fp,tokvalue)) return(FALSE);
    signals[i]->name=CopyString(tokvalue);
    i++;
    if (!get_word_rsg(fp,tokvalue)) return(FALSE);
  }
#ifdef DLONG
  initialize_bdd_state_table(status,FALSE,bdd_state,signals,nsignals,nrules);
#else
  //CUDD
  design->bdd_state=new_bdd_state_table(status,FALSE,bdd_state,signals,
					nsignals,ninpsig,0,filename);
  bdd_state=design->bdd_state;
#endif 

  /* Read states */
  if (0) {
    if (fgettokrsg(fp,tokvalue,MAXTOKEN) != END_OF_FILE) {
      free(*startstate);
      (*startstate)=CopyString(tokvalue); 
      add_state(NULL,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
		tokvalue,NULL,NULL, nsignals, NULL, NULL, 0, 0,FALSE, FALSE, 
		0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,
		0);
      sn=1;
      while (fgettokrsg(fp,tokvalue,MAXTOKEN) != END_OF_FILE)
	if (add_state(NULL,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
		      tokvalue,NULL,NULL, nsignals, NULL, NULL, 0, sn, FALSE,
		      FALSE,0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, 
		      NULL, NULL, 0)) sn++;
    }
    fclose(fp);
  } else {
    states=0;
    while ((fgettokrsg(fp,tokvalue,MAXTOKEN) != END_OF_FILE) &&
	   (strcmp(tokvalue,"EDGES")!=0)) {
      sn=atoi(tokvalue);
      fgettokrsg(fp,tokvalue,MAXTOKEN);
      if (sn==0) {
	free(*startstate);
	(*startstate)=CopyString(tokvalue); 
      } 
      add_state(NULL,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
		tokvalue,NULL,NULL, nsignals, NULL, NULL, 0, sn, FALSE,
		FALSE,0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, 
		NULL, NULL, 0);
      states++;
    }
  }

  trans=0;
  if (0) {
    /* Add predecessor links */
    nextstate=(char *)GetBlock((nsignals+1)*sizeof(char));
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	for (j=0;j<nsignals;j++)
	  if ((curstate->state[j]=='R') || (curstate->state[j]=='F')) {
	    for (k=0;k<nsignals;k++)
	      nextstate[k]=VAL(curstate->state[k]);
	    nextstate[k]='\0';
	    if (curstate->state[j]=='R') nextstate[j]='1';
	    if (curstate->state[j]=='F') nextstate[j]='0';
	    nextstateptr=find_next_state(state_table,nextstate,nsignals);
	    while (nextstateptr) {
	      if (nextstateptr->pred) {
		for (statelist=nextstateptr->pred; 
		     statelist->next; statelist=statelist->next);
		statelist->next=(statelistADT)
		  GetBlock(sizeof (*(nextstateptr->pred)));
		statelist->next->enabling=(-1);
		statelist->next->enabled=(-1);
		statelist->next->probability=0;
		statelist->next->stateptr=curstate;
		statelist->next->next=NULL;
	      } else {
		nextstateptr->pred=(statelistADT)
		  GetBlock(sizeof (*(nextstateptr->pred)));
		nextstateptr->pred->enabling=(-1);
		nextstateptr->pred->enabled=(-1);
		nextstateptr->pred->probability=0;
		nextstateptr->pred->stateptr=curstate;
		nextstateptr->pred->next=NULL;
	      }
	      nextstateptr=next_state_match(nextstateptr->link,nextstate,
					    nsignals);
	    }
	  }
      }
    /* Add succecessor links */
    sn=0; trans=0;
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	sn++;
	for (j=0;j<nsignals;j++)
	  if ((curstate->state[j]=='R') || (curstate->state[j]=='F')) {
	    for (k=0;k<nsignals;k++)
	      nextstate[k]=VAL(curstate->state[k]);
	    nextstate[k]='\0';
	    if (curstate->state[j]=='R') nextstate[j]='1';
	    if (curstate->state[j]=='F') nextstate[j]='0';
	    nextstateptr=find_next_state(state_table,nextstate,nsignals);
	    while (nextstateptr) {
	      trans++;
	      if (curstate->succ) {
		for (statelist=curstate->succ; 
		     statelist->next; statelist=statelist->next);
		statelist->next=(statelistADT)
		  GetBlock(sizeof (*(nextstateptr->pred)));
		statelist->next->enabling=(-1);
		statelist->next->enabled=(-1);
		statelist->next->probability=0;
		statelist->next->stateptr=nextstateptr;
		statelist->next->next=NULL;
	      } else {
		curstate->succ=(statelistADT)
		  GetBlock(sizeof (*(curstate->succ)));
		curstate->succ->enabling=(-1);
		curstate->succ->enabled=(-1);
		curstate->succ->probability=0;
		curstate->succ->stateptr=nextstateptr;
		curstate->succ->next=NULL;
	      }
	      nextstateptr=next_state_match(nextstateptr->link,nextstate,
					    nsignals);
	    }
	  }
      }
    free(nextstate);
  } else if (!use_bdd) {
    while (fgettokrsg(fp,tokvalue,MAXTOKEN) != END_OF_FILE) {
      j=atoi(tokvalue);
      if (fgettokrsg(fp,tokvalue,MAXTOKEN)!=WORD) {
	printf("ERROR: expected a state.\n");
	return FALSE;
      }
      vstate=SVAL(tokvalue,nsignals);
      i=hashpjw(vstate);
      free(vstate);
      prevstateptr=NULL;
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	if (curstate->number==j) prevstateptr=curstate;
      }
      if (!prevstateptr) {
	printf("ERROR: Could not find %d:%s.\n",j,tokvalue);
	fprintf(lg,"ERROR: Could not find %d:%s.\n",j,tokvalue);
	return FALSE;
      }
      if (fgettokrsg(fp,tokvalue,MAXTOKEN)!='-') {
	printf("ERROR: expected '-'.\n");
	return FALSE;
      }
      token=fgettokrsg(fp,tokvalue,MAXTOKEN);
      if ((token!='>') && (token!=WORD)) {
	printf("ERROR: expected '>' or event.\n");
	return FALSE;
      } else if (token==WORD) {
	token=fgettokrsg(fp,tokvalue,MAXTOKEN);
	if ((token!='-') && (token!='+') && (token!='/')) {
	  printf("ERROR: expected '-', '+', or '/'.\n");
	  return FALSE;
	}
	if ((token=='-')||(token=='+')) {
	  token=fgettokrsg(fp,tokvalue,MAXTOKEN);
	  if ((token!='-') && (token!='>') && (token!='/')) {
	    printf("ERROR: expected '-', '>', or '/'.\n");
	    return FALSE;
	  }
	}
	if (token=='/') {
	  if (fgettokrsg(fp,tokvalue,MAXTOKEN)!=WORD) {
	    printf("ERROR: expected a state number.\n");
	    return FALSE;
	  }
	  if (fgettokrsg(fp,tokvalue,MAXTOKEN)!='-') {
	    printf("ERROR: expected '-'.\n");
	    return FALSE;
	  }
	  if (fgettokrsg(fp,tokvalue,MAXTOKEN)!='>') {
	    printf("ERROR: expected '>'.\n");
	    return FALSE;
	  }
	} else if (token=='-') {
	  if (fgettokrsg(fp,tokvalue,MAXTOKEN)!='>') {
	    printf("ERROR: expected '>'.\n");
	    return FALSE;
	  }
	}
      }
      if (fgettokrsg(fp,tokvalue,MAXTOKEN)!=WORD) {
	printf("ERROR: expected a state number.\n");
	return FALSE;
      }
      k=atoi(tokvalue);
      if (fgettokrsg(fp,tokvalue,MAXTOKEN)!=WORD) {
	printf("ERROR: expected a state.\n");
	return FALSE;
      }
      vstate=SVAL(tokvalue,nsignals);
      l=hashpjw(vstate);
      free(vstate);
      nextstateptr=NULL;
      for (curstate=state_table[l];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	if (curstate->number==k) nextstateptr=curstate;
      }
      if (!nextstateptr) {
	printf("ERROR: Could not find %d:%s.\n",k,tokvalue);
	fprintf(lg,"ERROR: Could not find %d:%s.\n",k,tokvalue);
	return FALSE;
      }
      trans++;
      if (prevstateptr)
	if (prevstateptr->succ) {
	  for (statelist=prevstateptr->succ; 
	       statelist->next; statelist=statelist->next);
	  statelist->next=(statelistADT)
	    GetBlock(sizeof (*(prevstateptr->succ)));
	  statelist->next->enabling=(-1);
	  statelist->next->enabled=(-1);
	  statelist->next->probability=0;
	  statelist->next->stateptr=nextstateptr;
	  statelist->next->next=NULL;
	} else {
	  prevstateptr->succ=(statelistADT)
	    GetBlock(sizeof (*(prevstateptr->succ)));
	  prevstateptr->succ->enabling=(-1);
	  prevstateptr->succ->enabled=(-1);
	  prevstateptr->succ->probability=0;
	  prevstateptr->succ->stateptr=nextstateptr;
	  prevstateptr->succ->next=NULL;
	}
      if (nextstateptr)
	if (nextstateptr->pred) {
	  for (statelist=nextstateptr->pred; 
	       statelist->next; statelist=statelist->next);
	  statelist->next=(statelistADT)
	    GetBlock(sizeof (*(nextstateptr->pred)));
	  statelist->next->enabling=(-1);
	  statelist->next->enabled=(-1);
	  statelist->next->probability=0;
	  statelist->next->stateptr=prevstateptr;
	  statelist->next->next=NULL;
	} else {
	  nextstateptr->pred=(statelistADT)
	    GetBlock(sizeof (*(nextstateptr->pred)));
	  nextstateptr->pred->enabling=(-1);
	  nextstateptr->pred->enabled=(-1);
	  nextstateptr->pred->probability=0;
	  nextstateptr->pred->stateptr=prevstateptr;
	  nextstateptr->pred->next=NULL;
	}
    }
  }
  fclose(fp);
  printf("states = %d transitions = %d\n",sn,trans);
  fprintf(lg,"states = %d transitions = %d\n",sn,trans);
  return(TRUE);
}

/*****************************************************************************/
/* Find reduced state graph using constrained token flow.                    */
/*****************************************************************************/

bool load_reduced_state_graph(designADT design)
{
  if (load_rsg_header(design->filename,&(design->ninpsig),
		      &(design->nsignals))) {
    design->signals=new_signals(design->status,FALSE,design->oldnsignals,
				design->nsignals,design->signals);
    initialize_state_table(design->status,FALSE,design->state_table);
    design->regions=new_regions(design->status,FALSE,(2*design->oldnsignals)+1,
				(2*design->nsignals)+1,design->regions);
    design->oldnsignals=design->nsignals;
    if (load_rsg(design->status,design->filename,design->signals,
		 design->state_table,
		 design->nsignals,(&design->startstate), design->nrules,
		 design->bdd_state,design->bdd,design->markkey,
		 design->timeopts,design, design->ninpsig)) {
      design->status=(LOADED | CONNECTED | FOUNDRED);
      if (check_csc(design->filename,design->signals,design->state_table,
		    design->nsignals,design->ninpsig,design->verbose)) {
	design->status=(design->status | FOUNDSTATES | FOUNDRSG);
	return(TRUE);
      } else
	return (FALSE);
    }
  } 
  printf("ERROR: %s.rsg is not a valid rsg file!\n",design->filename);
  fprintf(lg,"ERROR: %s.rsg is not a valid rsg file!\n",design->filename);
  return(FALSE);
}
