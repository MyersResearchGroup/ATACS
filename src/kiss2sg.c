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
/* kiss2sg.c                                                                 */
/*   Includes procedures to load an ER system from a file in this format:    */
/*   .i number of inputs                                                     */
/*   .o number of outputs                                                    */
/*   .p number of products                                                   */
/*   .s number of state bits                                                 */
/*   <inputs> <current state> <next state> <outputs>                         */
/*****************************************************************************/

#include "kiss2sg.h"

/*****************************************************************************/
/* Get a token from a kiss file.                                             */
/*****************************************************************************/

int fgettokkiss(FILE *fp,char *tokvalue, int maxtok) 

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
      if (!readword) {
	return(END_OF_LINE);
      } else {
	ungetc('\n',fp);
	return (WORD);
      }
      break;
    case '#':
      if (!readword) {
	while (((c=getc(fp)) != EOF) && (c != '\n'));
	return(COMMENT);
      } else {
	ungetc('#',fp);
	return (WORD);
      }
      break;
    case '.':
      if (!readword) {
	if ((c=getc(fp)) != EOF) {
	  *tokvalue++=c;
	  *tokvalue='\0';
	  return(DOT);
	} else return(END_OF_FILE);
      } else {
	ungetc('.',fp);
	return (WORD);
      }
      break;
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

/*****************************************************************************/
/* Load kiss header info.                                                    */
/*****************************************************************************/

bool load_kiss_header(FILE *fp,int *ninputs,int *noutputs,int *nprods,
		      int *nstates)
{
  int value,token;
  char dot[MAXTOKEN];
  char dotvalue[MAXTOKEN];

  while ((token=fgettokkiss(fp,dot,MAXTOKEN)) != WORD) {
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      return FALSE;
    }
    if (token==DOT) {
      token=fgettokkiss(fp,dotvalue,MAXTOKEN);
      if (token==END_OF_FILE) {
	printf("ERROR:  Unexpected end of file!\n");
	fprintf(lg,"ERROR:  Unexpected end of file!\n");
	return FALSE;
      }
      value=atoi(dotvalue);
      if (token==WORD)
	switch (dot[0]) {
	case 'i':
	  *ninputs=value;
	  break;
	case 'o':
	  *noutputs=value;
	  break;
	case 'p':
	  *nprods=value;
	  break;
	case 's':
	  *nstates=value;
	  break;
	default:
	  printf("ERROR:  Bad header information!\n");
	  fprintf(lg,"ERROR:  Bad header information!\n");
	  return FALSE;
	}
      if ((*ninputs != 0) && (*noutputs != 0)  && (*nprods != 0) && 
	  (*nstates != 0)) return TRUE; 
    }
  }
  if (*ninputs==0) {
    printf("ERROR:  No inputs in system!\n");
    fprintf(lg,"ERROR:  No inputs in system!\n");
    return FALSE;
  }
  if (*noutputs==0) {
    printf("ERROR:  No outputs in system!\n");
    fprintf(lg,"ERROR:  No outputs in system!\n");
    return FALSE;
  }
  if (*nprods==0) {
    printf("ERROR:  No products in system!\n");
    fprintf(lg,"ERROR:  No products in system!\n");
    return FALSE;
  }
  if (*nstates==0) {
    printf("ERROR:  No states in system!\n");
    fprintf(lg,"ERROR:  No states in system!\n");
    return FALSE;
  }
  return TRUE;
}

bool print_nextstate(FILE *fp,char * inputs,char * prevstate,
		     char * prevoutputs,char * curstate,char * curoutputs,
		     char * nextstate,char * nextoutputs,int ninputs,
		     int nstates,int noutputs,stateADT *state_table,
		     bool latchstate,bool latchoutputs,bddADT bdd_state,
		     bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
		     int *sn)
{
  int i;
  char temp;
  char * state;
  int nsignals;

  nsignals=ninputs+nstates+noutputs+2;
  if (latchstate) nsignals=nsignals+nstates;
  if (latchoutputs) nsignals=nsignals+noutputs;
  state=(char *)GetBlock((nsignals+1)*sizeof(char));
  if ((strcmp(curstate,nextstate)==0) && (strcmp(curoutputs,nextoutputs)==0)) {
    sprintf(state,"0F%s%s%s",inputs,curstate,curoutputs); 
    if (latchstate) strcat(state,prevstate);
    if (latchoutputs) strcat(state,prevoutputs);
    add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	      state,NULL,NULL, nsignals, NULL, NULL, 0, (*sn), FALSE,
	      FALSE,0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, 
	      NULL, NULL, 0);
    (*sn)++;
    sprintf(state,"R0%s%s%s",inputs,curstate,curoutputs); 
    if (latchstate) strcat(state,prevstate);
    if (latchoutputs) strcat(state,prevoutputs);
    add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	      state,NULL,NULL,nsignals,NULL,NULL, 0, *sn,TRUE, FALSE, 
	      0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,
	      0);
    (*sn)++;
  } else {
    sprintf(state,"01%s",inputs);
    for (i=0;i<nstates;i++)
      if (curstate[i]==nextstate[i]) 
	state[i+ninputs+2]=curstate[i];
      else
	if (nextstate[i]=='1') state[i+ninputs+2]='R';
	else if (nextstate[i]=='0') state[i+ninputs+2]='F';
	else {
	  printf("ERROR:  Bit not 0 or 1!\n");
	  fprintf(lg,"ERROR:  Bit not 0 or 1!\n");
	  free(state);
	  return(FALSE);
	}
    for (i=0;i<noutputs;i++)
      if (curoutputs[i]==nextoutputs[i]) 
	state[i+ninputs+nstates+2]=curoutputs[i];
      else
	if (nextoutputs[i]=='1') state[i+ninputs+nstates+2]='R';
	else if (nextoutputs[i]=='0') state[i+ninputs+nstates+2]='F';
	else {
	  printf("ERROR:  Bit not 0 or 1!\n");
	  fprintf(lg,"ERROR:  Bit not 0 or 1!\n");
	  free(state);
	  return(FALSE);
	}
    state[noutputs+ninputs+nstates+2]='\0';
    if (latchstate) strcat(state,prevstate);
    if (latchoutputs) strcat(state,prevoutputs);
    state[nsignals]='\0';
    if (add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	      state,NULL,NULL,nsignals,NULL,NULL, 0, (*sn),TRUE, FALSE, 
	      0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,
	      0)) {
      (*sn)++;
      for (i=0;i<nstates;i++)
	if (curstate[i] != nextstate[i]) {
	  temp=curstate[i];
	  curstate[i]=nextstate[i];
	  if (!print_nextstate(fp,inputs,prevstate,prevoutputs,curstate,
			       curoutputs,nextstate,nextoutputs,ninputs,
			       nstates,noutputs,state_table,latchstate,
			       latchoutputs,bdd_state, use_bdd,marker,
			       timeopts,sn)) 
	    return FALSE;
	  curstate[i]=temp;
	}
      for (i=0;i<noutputs;i++)
	if (curoutputs[i] != nextoutputs[i]) {
	  temp=curoutputs[i];
	  curoutputs[i]=nextoutputs[i];
	  if (!print_nextstate(fp,inputs,prevstate,prevoutputs,curstate,
			       curoutputs,nextstate,nextoutputs,ninputs,
			       nstates,noutputs,state_table,latchstate,
			       latchoutputs,bdd_state, use_bdd,marker,
			       timeopts,sn)) 
	    return FALSE;
	  curoutputs[i]=temp;
	}
    }
  }
  free(state);
  return TRUE;
}

bool print_latchstates(FILE *fp,char * inputs,char * prevstate,
		       char * prevoutputs,char * curstate,char * curoutputs,
		       int ninputs,int nstates,int noutputs,
		       stateADT *state_table,bool latchstate,bool latchoutputs,
		       bddADT bdd_state,bool use_bdd,markkeyADT *marker,
		       timeoptsADT timeopts,int *sn)
{
  int i;
  char temp;
  char * state;
  int nsignals;

  nsignals=ninputs+nstates+noutputs+2;
  if (latchstate) nsignals=nsignals+nstates;
  if (latchoutputs) nsignals=nsignals+noutputs;
  state=(char *)GetBlock((nsignals+1)*sizeof(char));
  if (((latchstate) && (strcmp(curstate,prevstate)!=0)) || 
      ((latchoutputs) && (strcmp(curoutputs,prevoutputs)!=0))) {
    state[0]='1';
    state[1]='0';
    for (i=0;i<ninputs;i++)
      if (inputs[i]=='0') state[i+2]='R';
    else if (inputs[i]=='1') state[i+2]='F';
    else {
      printf("ERROR:  Bit not 0 or 1!\n");
      fprintf(lg,"ERROR:  Bit not 0 or 1!\n");
      return(FALSE);
    }
    state[ninputs+2]='\0';
    strcat(state,curstate);
    strcat(state,curoutputs);
    if (latchstate) 
      for (i=0;i<nstates;i++)
	if (curstate[i]==prevstate[i]) 
	  state[i+nstates+noutputs+ninputs+2]=prevstate[i];
	else
	  if (curstate[i]=='1') state[i+nstates+noutputs+ninputs+2]='R';
	  else if (curstate[i]=='0') state[i+nstates+noutputs+ninputs+2]='F';
	  else {
	    printf("ERROR:  Bit not 0 or 1!\n");
	    fprintf(lg,"ERROR:  Bit not 0 or 1!\n");
	    free(state);
	    return(FALSE);
	  }
    if (latchoutputs)
      for (i=0;i<noutputs;i++)
	if (curoutputs[i]==prevoutputs[i]) 
	  state[i+nstates+noutputs+ninputs+nstates+2]=prevoutputs[i];
	else
	  if (curoutputs[i]=='1') 
	    state[i+nstates+noutputs+ninputs+nstates+2]='R';
	  else if (curoutputs[i]=='0') 
	    state[i+nstates+noutputs+ninputs+nstates+2]='F';
	  else {
	    printf("ERROR:  Bit not 0 or 1!\n");
	    fprintf(lg,"ERROR:  Bit not 0 or 1!\n");
	    free(state);
	    return(FALSE);
	  }
    state[nsignals]='\0';
    if (add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	      state,NULL,NULL,nsignals,NULL,NULL, 0, (*sn),TRUE, FALSE, 
	      0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,
	      0)) {
      (*sn)++;
      if (latchstate)
	for (i=0;i<nstates;i++)
	  if (curstate[i] != prevstate[i]) {
	    temp=prevstate[i];
	    prevstate[i]=curstate[i];
	    if (!print_latchstates(fp,inputs,prevstate,prevoutputs,curstate,
				   curoutputs,ninputs,nstates,noutputs,
				   state_table,latchstate,latchoutputs,
				   bdd_state, use_bdd,marker,timeopts,sn))
	      return FALSE;
	    prevstate[i]=temp;
	  }
      if (latchoutputs)
	for (i=0;i<noutputs;i++)
	  if (curoutputs[i] != prevoutputs[i]) {
	    temp=prevoutputs[i];
	    prevoutputs[i]=curoutputs[i];
	    if (!print_latchstates(fp,inputs,prevstate,prevoutputs,curstate,
				   curoutputs,ninputs,nstates,noutputs,
				   state_table,latchstate,latchoutputs,
				   bdd_state, use_bdd,marker,timeopts,sn))
	      return FALSE;
	    prevoutputs[i]=temp;
	  }
    }
  }
  free(state);
  return TRUE;
}

bool print_curstate(FILE *fp,char * inputs,char * curstate,char * curoutputs,
		    char * nextstate,int ninputs,int nstates,int noutputs,
		    stateADT *state_table,bool latchstate,bool latchoutputs,
		    bddADT bdd_state,bool use_bdd,markkeyADT *marker,
		    timeoptsADT timeopts,int *sn)
{
  int i,nsignals;
  char * state;
  
  nsignals=ninputs+nstates+noutputs+2;
  if (latchstate) nsignals=nsignals+nstates;
  if (latchoutputs) nsignals=nsignals+noutputs;
  state=(char *)GetBlock((nsignals+1)*sizeof(char));
  if (nextstate[0]=='-') state[0]='1';
  else state[0]='F';
  state[1]='0';
  for (i=0;i<ninputs;i++)
    if (inputs[i]=='0') state[i+2]='R';
    else if (inputs[i]=='1') state[i+2]='F';
    else {
      printf("ERROR:  Bit not 0 or 1!\n");
      fprintf(lg,"ERROR:  Bit not 0 or 1!\n");
      return(FALSE);
    }
  state[ninputs+2]='\0';
  strcat(state,curstate);
  strcat(state,curoutputs);
  if (latchstate) strcat(state,curstate);
  if (latchoutputs) strcat(state,curoutputs);
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	    state,NULL,NULL,nsignals,NULL,NULL, 0, *sn,TRUE, FALSE, 
	    0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,0);
  (*sn)++;
  if (nextstate[0]!='-') {
    sprintf(state,"0R%s%s%s",inputs,curstate,curoutputs); 
    if (latchstate) strcat(state,curstate);
    if (latchoutputs) strcat(state,curoutputs);
    add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	      state,NULL,NULL,nsignals,NULL,NULL, 0, *sn,TRUE, FALSE, 
	      0,0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,0);
    (*sn)++;
  }
  free(state);
  return TRUE;
}

bool latch_states(FILE *fp,char * inputs,char * curstate,char * curoutputs,
		  char * nextstate,char * nextoutputs,int ninputs,int nstates,
		  int noutputs,stateADT *state_table,bool latchstate,
		  bool latchoutputs,bddADT bdd_state,bool use_bdd,
		  markkeyADT *marker,timeoptsADT timeopts,int *sn)
{
  char *dc;

  dc=strchr(inputs,'-');
  if (dc) {
    *dc='0';
    if (!latch_states(fp,inputs,curstate,curoutputs,nextstate,
		      nextoutputs,ninputs,nstates,noutputs,state_table,
		      latchstate,latchoutputs,bdd_state, use_bdd,marker,
		      timeopts,sn)) 
      return FALSE;
    *dc='1';
    if (!latch_states(fp,inputs,curstate,curoutputs,nextstate,
		      nextoutputs,ninputs,nstates,noutputs,state_table,
		      latchstate,latchoutputs,bdd_state, use_bdd,marker,
		      timeopts,sn)) 
      return FALSE;
    *dc='-';
  } else 
    if (!print_latchstates(fp,inputs,curstate,curoutputs,nextstate,
			   nextoutputs,ninputs,nstates,noutputs,state_table,
			   latchstate,latchoutputs,bdd_state,use_bdd,marker,
			   timeopts,sn))
      return FALSE;
  return TRUE;
}

bool print_states(FILE *fp,char * inputs,char * curstate,char * curoutputs,
		  char * nextstate,char * nextoutputs,int ninputs,int nstates,
		  int noutputs,stateADT *state_table,bool latchstate,
		  bool latchoutputs,bddADT bdd_state,
		  bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,int *sn)
{
  int i;
  char * prevstate;
  char * prevoutputs;
  char * allinputs;

  prevstate=CopyString(curstate);
  prevoutputs=CopyString(curoutputs);
  if (!print_curstate(fp,inputs,curstate,curoutputs,nextstate,ninputs,
		      nstates,noutputs,state_table,latchstate,latchoutputs,
		      bdd_state, use_bdd,marker,timeopts,sn)) {
    free(prevstate);
    free(prevoutputs);
    return FALSE;
  }
  if (nextstate[0]!='-') {
    if (!print_nextstate(fp,inputs,prevstate,prevoutputs,curstate,curoutputs,
			 nextstate,nextoutputs,ninputs,nstates,noutputs,
			 state_table,latchstate,latchoutputs,bdd_state, 
			 use_bdd,marker,timeopts,sn)) { 
      free(prevstate);
      free(prevoutputs);
      return FALSE;
    }
    if ((latchstate) || (latchoutputs)) {
      allinputs=CopyString(inputs);
      for (i=0;i<ninputs;i++)
	allinputs[i]='-';
      allinputs[ninputs]='\0';
      if (!latch_states(fp,allinputs,curstate,curoutputs,nextstate,
			nextoutputs,ninputs,nstates,noutputs,state_table,
			latchstate,latchoutputs,bdd_state, use_bdd,marker,
			timeopts,sn)) {
	free(prevstate);
	free(prevoutputs);
	free(allinputs);
	return FALSE;
      }
      free(allinputs);
    }
  }
  free(prevstate);
  free(prevoutputs);
  return TRUE;
}

bool expand_state(FILE *fp,char * inputs,char * curstate,char * curoutputs,
		  char * nextstate,char * nextoutputs,int ninputs,int nstates,
		  int noutputs,stateADT *state_table,bool latchstate,
		  bool latchoutputs,bddADT bdd_state,
		  bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,int *sn)
{
  char *dc;

  dc=strchr(inputs,'-');
  if (dc) {
    *dc='0';
    if (!expand_state(fp,inputs,curstate,curoutputs,nextstate,
		      nextoutputs,ninputs,nstates,noutputs,state_table,
		      latchstate,latchoutputs,bdd_state, 
		      use_bdd,marker,timeopts,sn)) 
      return FALSE;
    *dc='1';
    if (!expand_state(fp,inputs,curstate,curoutputs,nextstate,
		      nextoutputs,ninputs,nstates,noutputs,state_table,
		      latchstate,latchoutputs,bdd_state, 
			 use_bdd,marker,timeopts,sn)) 
      return FALSE;
    *dc='-';
  } else 
    if (!print_states(fp,inputs,curstate,curoutputs,nextstate,
		      nextoutputs,ninputs,nstates,noutputs,state_table,
		      latchstate,latchoutputs,bdd_state,use_bdd,marker,
		      timeopts,sn)) 
      return FALSE;
  return TRUE;
}

/*****************************************************************************/
/* Get a kiss field.                                                         */
/*****************************************************************************/

bool get_kiss_field(FILE *fp,char * tokvalue,int size)
{
  int token;

  while ((token=fgettokkiss(fp,tokvalue,MAXTOKEN)) != WORD) {
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      return(FALSE);
    }
    if (token==END_OF_LINE) {
      printf("ERROR:  Unexpected end of line!\n");
      fprintf(lg,"ERROR:  Unexpected end of line!\n");
      return(FALSE);
    }
  }
  if (strlen(tokvalue) != (unsigned)size) {
    printf("ERROR:  Improper token size %s %d!\n",tokvalue,size);
    fprintf(lg,"ERROR:  Improper token size!\n");
    return(FALSE);
  }
  return(TRUE);
}

/*****************************************************************************/
/* Load products.                                                            */
/*****************************************************************************/

bool load_products(FILE *fp,FILE *fpO,int ninputs,int noutputs,int nprods,
		   int nstates,stateADT *state_table,bool latchstate,
		   bool latchoutputs,bddADT bdd_state,
		   bool use_bdd,markkeyADT *marker,timeoptsADT timeopts)
{
  int sn,i,token;
  char tokvalue[MAXTOKEN];
  char inputs[MAXTOKEN];
  char curstate[MAXTOKEN];
  char curoutputs[MAXTOKEN];
  char nextstate[MAXTOKEN];
  char nextoutputs[MAXTOKEN];
  
  sn=0;
  for (i=0;i<nprods;i++) {
    while (((token=fgettokkiss(fp,tokvalue,MAXTOKEN)) != END_OF_LINE) &&
           (token != COMMENT)) {
      if (token==END_OF_FILE) {
        printf("ERROR:  Unexpected end of file!\n");
        fprintf(lg,"ERROR:  Unexpected end of file!\n");
        return(FALSE); } }
    if (!get_kiss_field(fp,tokvalue,ninputs)) return(FALSE);
    strcpy(inputs,tokvalue);
    if (!get_kiss_field(fp,tokvalue,nstates)) return(FALSE);
    strcpy(curstate,tokvalue);
    if (!get_kiss_field(fp,tokvalue,noutputs)) return(FALSE);
    strcpy(curoutputs,tokvalue);
    if (!get_kiss_field(fp,tokvalue,nstates)) return(FALSE);
    strcpy(nextstate,tokvalue);
    if (!get_kiss_field(fp,tokvalue,noutputs)) return(FALSE);
    strcpy(nextoutputs,tokvalue);
    if (!expand_state(fpO,inputs,curstate,curoutputs,nextstate,
		      nextoutputs,ninputs,nstates,noutputs,state_table,
		      latchstate,latchoutputs,bdd_state, 
		      use_bdd,marker,timeopts,&sn)) 
      return FALSE;
  } 
  return(TRUE);
}

void print_kiss_header(FILE *fp,int ninputs,int nstates,int noutputs,
		       bool latchstate,bool latchoutputs)
{
  int i;

  fprintf(fp,"SG:\n");
  fprintf(fp,"STATEVECTOR: INP phi1 INP phi2 ");
  for (i=0;i<ninputs;i++)
    fprintf(fp,"INP in%d ",i);
  for (i=0;i<nstates;i++)
    fprintf(fp,"st%d ",i);
  for (i=0;i<noutputs;i++)
    fprintf(fp,"out%d ",i);
  if (latchstate)
    for (i=0;i<nstates;i++)
      fprintf(fp,"st%dp ",i);
  if (latchoutputs)
    for (i=0;i<noutputs;i++)
      fprintf(fp,"out%dp ",i);
  fprintf(fp,"\n");
  fprintf(fp,"STATES:\n");
}

void print_state(FILE *fp,char * state,int nsignals)
{
  int i;

  for (i=0;i<nsignals;i++)
    if ((state[i]=='R') || (state[i]=='0')) fprintf(fp,"0");
    else fprintf(fp,"1");
}

void run_espresso(stateADT* state_table,int nsignals,int signal,bool rise)
{
  FILE *fp;
  int i;
  stateADT curstate;

  fp=Fopen("input","w");
  fprintf(fp,".i %d\n",nsignals);
  fprintf(fp,".o 1\n");
  fprintf(fp,".type fr\n");
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link)
      if (curstate->state[signal]=='R') {
	print_state(fp,curstate->state,nsignals);
	if (rise) fprintf(fp," 1\n"); else fprintf(fp," 0\n"); 
      } else if (curstate->state[signal]=='F') {
	print_state(fp,curstate->state,nsignals);
	if (rise) fprintf(fp," 0\n"); else fprintf(fp," 1\n");
      } else if (curstate->state[signal]=='1') {
	if (!rise) {
	  print_state(fp,curstate->state,nsignals);
	  fprintf(fp," 0\n");
	}
      } else if (curstate->state[signal]=='0') {
	if (rise) {
	  print_state(fp,curstate->state,nsignals);
	  fprintf(fp," 0\n");
	}
      }
  fprintf(fp,".e\n");
  fclose(fp);
  system("espresso < input > output");
}

bool parse_espresso_output(FILE *fpO,int ninputs,int nstates,
			   int noutputs,int signal,bool rise,char **prodrules,
			   int *nprs,int nsignals)
{
  FILE *fp;
  int i,j,dummy1,dummy2,dummy3,nprods;
  int token;
  char tokvalue[MAXTOKEN];

  fp=Fopen("output","r");
  dummy1=0;
  dummy2=0;
  dummy3=1;
  nprods=0;

  if (!load_kiss_header(fp,&dummy1,&dummy2,&nprods,&dummy3)) 
    return(FALSE);

  for (i=0;i<nprods;i++) {
    prodrules[(*nprs)]=(char*)GetBlock(nsignals * sizeof(prodrules[0][0]));
    (*nprs)++;
  }

  while (((token=fgettokkiss(fp,tokvalue,MAXTOKEN)) != END_OF_LINE) &&
	   (token != COMMENT)) {
    if (token==END_OF_FILE) {
      printf("ERROR:  Unexpected end of file!\n");
      fprintf(lg,"ERROR:  Unexpected end of file!\n");
      return(FALSE); 
    } 
    if (!get_kiss_field(fp,tokvalue,dummy1)) return(FALSE);
    if (rise) fprintf(fpO,"[+"); else fprintf(fpO,"[-");
    if (signal < ninputs+nstates+2) fprintf(fpO,"st%d: (",signal-ninputs-2);
    else fprintf(fpO,"out%d: (",signal-ninputs-nstates-2);
    for (j=0;j<2;j++)
      if (tokvalue[j]=='0') fprintf(fpO,"~phi%d",j+1);
      else if (tokvalue[j]=='1') fprintf(fpO,"phi%d",j+1);
    for (j=0;j<ninputs;j++)
      if (tokvalue[j+2]=='0') {
	fprintf(fpO," & ~in%d",j);
	prodrules[signal][j+2]='F';
      } else if (tokvalue[j+2]=='1') {
	fprintf(fpO," & in%d",j);
	prodrules[signal][j+2]='T';
      }
    for (j=0;j<nstates;j++)
      if (tokvalue[j+ninputs+2]=='0') {
	fprintf(fpO," & ~st%d",j);
	prodrules[signal][j+ninputs+2]='F';
      } else if (tokvalue[j+ninputs+2]=='1') {
	fprintf(fpO," & st%d",j);
	prodrules[signal][j+ninputs+2]='T';
      }
    for (j=0;j<noutputs;j++)
      if (tokvalue[j+ninputs+nstates+2]=='0') {
	fprintf(fpO," & ~out%d",j);
	prodrules[signal][j+ninputs+nstates+2]='F';
      } else if (tokvalue[j+ninputs+nstates+2]=='1') {
	fprintf(fpO," & out%d",j);
	prodrules[signal][j+ninputs+nstates+2]='T';
      }
    for (j=0;j<nstates;j++)
      if (tokvalue[j+ninputs+nstates+noutputs+2]=='0') {
	fprintf(fpO," & ~st%dp",j);
	prodrules[signal][j+ninputs+nstates+noutputs+2]='F';
      } else if (tokvalue[j+ninputs+nstates+noutputs+2]=='1') {
	fprintf(fpO," & st%dp",j);
	prodrules[signal][j+ninputs+nstates+noutputs+2]='T';
      }
    fprintf(fpO,")]\n");
    if (!get_kiss_field(fp,tokvalue,dummy2)) return(FALSE);
  } 
  fclose(fp);
  return(TRUE);
}

bool espresso_cover(char * filename,int nsignals,int ninputs,int noutputs,
		    int nstates,stateADT *state_table,bool latch_state,
		    bool latch_outputs)
{
  int i;
  FILE *fpO;
  char outname[FILENAMESIZE];
  char **prodrules=NULL;
  int nprs;

  prodrules=(char**)GetBlock(nsignals * sizeof(prodrules[0]));

  strcpy(outname,filename);
  strcat(outname,".prs");
  fpO=Fopen(outname,"w");
  printf("Storing production rules to:  %s\n",outname);
  fprintf(lg,"Storing production rules to:  %s\n",outname);
  for (i=0;i<nstates;i++) {
    run_espresso(state_table,nsignals,ninputs+i+2,TRUE);
    if (!parse_espresso_output(fpO,ninputs,nstates,noutputs,ninputs+i+2,TRUE,
			       prodrules,&nprs,nsignals)) 
      return FALSE;
    run_espresso(state_table,nsignals,ninputs+i+2,FALSE);
    if (!parse_espresso_output(fpO,ninputs,nstates,noutputs,ninputs+i+2,FALSE,
			       prodrules,&nprs,nsignals))
      return FALSE;
  }
  for (i=0;i<noutputs;i++) {
    run_espresso(state_table,nsignals,ninputs+nstates+i+2,TRUE);
    if (!parse_espresso_output(fpO,ninputs,nstates,noutputs,
			       ninputs+nstates+i+2,TRUE,prodrules,&nprs,
			       nsignals)) 
      return FALSE;
    run_espresso(state_table,nsignals,ninputs+nstates+i+2,FALSE);
    if (!parse_espresso_output(fpO,ninputs,nstates,noutputs,
			       ninputs+nstates+i+2,FALSE,prodrules,&nprs,
			       nsignals)) 
      return FALSE;
  }
  if (latch_state)
    for (i=0;i<nstates;i++) {
      fprintf(fpO,"[+st%dp: (phi1 & st%d)]\n",i,i);
      fprintf(fpO,"[-st%dp: (phi1 & ~st%d)]\n",i,i);
    }
  if (latch_outputs)
    for (i=0;i<nstates;i++) {
      fprintf(fpO,"[+out%dp: (phi1 & out%d)]\n",i,i);
      fprintf(fpO,"[-out%dp: (phi1 & ~out%d)]\n",i,i);
    }

  fclose(fpO);
  return TRUE;
}

/*****************************************************************************/
/* Tranform kiss specification to a state graph.                             */
/*****************************************************************************/

bool kiss2sg(char * filename,stateADT *state_table,timeoptsADT timeopts,
	     bddADT bdd_state,bool use_bdd,markkeyADT *marker)
{
   char inname[FILENAMESIZE]; 
   char outname[FILENAMESIZE]; 
   int ninputs,noutputs,nproducts,nstates,nsignals; 
   FILE *fp; 
   FILE *fpO; 
   bool latch_state; 
   bool latch_outputs; 

   latch_state=TRUE; 
   latch_outputs=FALSE; 
   initialize_state_table(LOADED,FALSE,state_table);  
   strcpy(inname,filename); 
   strcat(inname,".kiss"); 
   if ((fp=fopen(inname,"r"))==NULL) { 
     printf("ERROR:  Cannot access %s!\n",inname); 
     fprintf(lg,"ERROR:  Cannot access %s!\n",inname); 
     return(FALSE);  
   } 
   printf("Loading kiss specification from:  %s\n",inname); 
   fprintf(lg,"Loading kiss specification from:  %s\n",inname); 
   strcpy(outname,filename); 
   strcat(outname,".sg"); 
   fpO=Fopen(outname,"w"); 
   printf("Storing state graph to:  %s\n",outname); 
   fprintf(lg,"Storing state graph to:  %s\n",outname); 
   ninputs=0; noutputs=0; nproducts=0; nstates=0;  
   if (!load_kiss_header(fp,&ninputs,&noutputs,&nproducts,&nstates))  
     return(FALSE); 
   print_kiss_header(fpO,ninputs,nstates,noutputs,latch_state,latch_outputs); 
   if (!load_products(fp,fpO,ninputs,noutputs,nproducts,nstates,state_table, 
		      latch_state,latch_outputs,bdd_state,use_bdd,marker,
		      timeopts)) 
     return(FALSE); 
   fclose(fpO); 
   fclose(fp); 
   nsignals=ninputs+nstates+noutputs+2; 
   if (latch_state) nsignals=nsignals+nstates; 
   if (latch_outputs) nsignals=nsignals+noutputs; 

   /*
   if (!espresso_cover(filename,nsignals,ninputs,noutputs,nstates,state_table, 
 		      latch_state,latch_outputs)) 
     return FALSE; 
   */
   return TRUE;
}



