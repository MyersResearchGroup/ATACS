/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
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
/*****************************************************************************/
/* loadac.c                                                                  */
/*****************************************************************************/
#include <unistd.h>
#include <cstdlib>

#include "loadac.h"
#include "compile.h"
#include "def.h"
#include "misclib.h"
#include "memaloc.h"

//#define KRONOS_CMP
//#define CHK_BEHV

/*****************************************************************************/
/* Get a token from a file.  Used for loading timed event-rule structures.   */
/*****************************************************************************/

int fgettok_ac(FILE *fp,char *tokvalue,int maxtok,int *linenum) 
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '[': 
    case ']':
    case '~':
    case '\\':
    case '/':
    case '=':
    case ',':
    case ';':
      if (!readword) return(c);
      else {
	ungetc(c,fp);
	return(WORD);
      }
      break;
    case '\n':
      if (!readword) {
	(*linenum)++;
	return(END_OF_LINE);
      } else {
	ungetc('\n',fp);
	return (WORD);
      }
      break;
    case '#':
      if (!readword) {
	while (((c=getc(fp)) != EOF) && (c != '\n'));
	(*linenum)++;
	toklen=0;
	*tokvalue = '\0';
	//return(COMMENT);
      } else {
	ungetc('#',fp);
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

void print_instance(FILE *fp,char * event)
{
  char phase;

  if (strchr(event,'/')) {
    fprintf(fp,"$");
    while (*event != '+' && *event != '-') {
      fprintf(fp,"%c",*event);
      event++;
    }
    phase = *event;
    fprintf(fp,"%s%c",strchr(event,'/')+1,phase);
  } else {
    fprintf(fp,"%s",event);
  }
}

void print_expected_instance(FILE *fp,char * event)
{
  char phase;

  if (strchr(event,'/')) {
    fprintf(fp,"$X");
    while (*event != '+' && *event != '-') {
      fprintf(fp,"%c",*event);
      event++;
    }
    phase = *event;
    fprintf(fp,"%s%c",strchr(event,'/')+1,phase);
  } else {
    fprintf(fp,"%s",event);
  }
}

void print_signal(FILE *fp,char * event)
{
  if (strchr(event,'/')) {
    while (*event != '/') {
      fprintf(fp,"%c",*event);
      event++;
    }
  } 
}

/*****************************************************************************/
/* Load gates from ac file.                                                  */
/*****************************************************************************/

bool load_gate(FILE *fp,char * filename,int nevents,int ninputs,int nrules,
	       eventADT *events,ruleADT **rules,int nsignals,
	       signalADT *signals,int *linenum,int ndummy)
{
  char tokvalue[MAXTOKEN];
  char signal[MAXTOKEN];
  int i,j,k,l,m,initvalue,lower,upper,lowerf,upperf;
  int token;
  int ne,nr,ni,no,nc,nd;

  char tmp[MAXTOKEN];
  char expr[MAXTOKEN];
  char exprb[MAXTOKEN];
  char ssi[MAXTOKEN];
  char ss[MAXTOKEN];
  token=WORD;

  FILE *fpI,*fpO,*fpR,*fpD,*fpC,*fpS;

  ne=1;
  ni=0;
  nr=0;
  no=0;
  nc=0;
  nd=0;
  ss[0]='\0';
  ssi[0]='\0';

  string outname = filename;
  outname = string(filename) + ".inp";;
  if ((fpI=Fopen(outname.c_str(),"w"))==NULL)
    return false;
  outname = string(filename) + ".out";;
  if ((fpO=Fopen(outname.c_str(),"w"))==NULL)
    return false;
  outname = string(filename) + ".dum";;
  if ((fpS=Fopen(outname.c_str(),"w"))==NULL)
    return false;
  outname = string(filename) + ".rule";;
  if ((fpR=Fopen(outname.c_str(),"w"))==NULL)
    return false;
  outname = string(filename) + ".ord";;
  if ((fpD=Fopen(outname.c_str(),"w"))==NULL)
    return false;
  outname = string(filename) + ".conf";;
  if ((fpC=Fopen(outname.c_str(),"w"))==NULL)
    return false;

  token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
  while (token != END_OF_FILE) {

    /* Get signal name */
    strcpy(signal,tokvalue);
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!='=') {
      printf("ERROR:%d:  Expecting a '='!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a '='!\n",*linenum);
      return FALSE;
    }
    /* Get initial value */
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    initvalue = atoi(tokvalue);
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!='=') {
      printf("ERROR:%d:  Expecting a '='!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a '='!\n",*linenum);
      return FALSE;
    }
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!='[') {
      printf("ERROR:%d:  Expecting a '['!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a '['!\n",*linenum);
      return FALSE;
    }
    /* Get lower bound */
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    lower = atoi(tokvalue);
    lowerf = lower;
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token!=',') {
      printf("ERROR:%d:  Expecting a ','!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a ','!\n",*linenum);
      return FALSE;
    }
    /* Get upper bound */
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (strcmp(tokvalue,"inf")==0)
      upper = INFIN;
    else
      upper = atoi(tokvalue);
    upperf = upper;
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    if (token==';') {
      /* Get falling lower bound */
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
      lowerf = atoi(tokvalue);
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
      if (token!=',') {
	printf("ERROR:%d:  Expecting a ','!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Expecting a ','!\n",*linenum);
	return FALSE;
      }
      /* Get falling upper bound */
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
      if (strcmp(tokvalue,"inf")==0)
	upperf = INFIN;
      else
	upperf = atoi(tokvalue);
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    }
    if (token!=']') {
      printf("ERROR:%d:  Expecting a ']'!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expecting a ']'!\n",*linenum);
      return FALSE;
    }
    /* Get expression */
    token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    tmp[0]='\0';
    while (token != END_OF_LINE && token != END_OF_FILE) {
      if (token==WORD)
	strcat(tmp,tokvalue);
      else {
	int len=strlen(tmp);
	tmp[len]=token;
	tmp[len+1]='\0';
      }
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
    }
    /* If not input, generate rules for gate */
    if (strcmp(tmp,"input")!=0) {
      expr[0]='\0';
      strcat(expr,"[");
      strcat(expr,tmp);
      strcat(expr,"]d");
      exprb[0]='\0';
      strcat(exprb,"[~(");
      strcat(exprb,tmp);
      strcat(exprb,")]d");
      fprintf(fpO,"%s+ %s-\n",signal,signal);
      ne+=2;
      if (initvalue==0) {
	strcat(ss,"0");
	fprintf(fpR,"%s- %s+ %s 1 %d ",signal,signal,expr,lower);
	if (upper==INFIN) fprintf(fpR,"inf\n"); 
	else fprintf(fpR,"%d\n",upper);
	fprintf(fpR,"%s+ %s- %s 0 %d ",signal,signal,exprb,lowerf);
	if (upperf==INFIN) fprintf(fpR,"inf\n"); 
	else fprintf(fpR,"%d\n",upperf);
      } else {
	strcat(ss,"1");
	fprintf(fpR,"%s+ %s- %s 1 %d ",signal,signal,exprb,lowerf);
	if (upperf==INFIN) fprintf(fpR,"inf\n"); 
	else fprintf(fpR,"%d\n",upperf);
	fprintf(fpR,"%s- %s+ %s 0 %d ",signal,signal,expr,lower);
	if (upper==INFIN) fprintf(fpR,"inf\n"); 
	else fprintf(fpR,"%d\n",upper);
      }
      nr+=2;
    } else {
      /* Find input signal */
      for (k=0;k<nsignals;k++)
	if (strcmp(signals[k]->name,signal)==0) break;
      if (k < nsignals) {
	/* Record initial value in startstate */
	if (initvalue==0) 
	  strcat(ssi,"0");
	else 
	  strcat(ssi,"1");
	for (j=signals[k]->event;(j<nevents && (events[j]->signal == k));j++) {
	  if (strchr(events[j]->event,'/')) {
	    if ((atoi(strchr(events[j]->event,'/')+1)==1) && (j % 2==1)) {
	      /* Emit input signal */
	      ni+=2;
	      ne+=2;
	      fprintf(fpI,"%s+ %s-\n",signal,signal);

	      /* Emit rules between input signal instances to input signal */
	      for (l=signals[k]->event;
		   (l<nevents && (events[l]->signal == k));l++) {

		for (m=1;m<nevents;m++)
		  if (rules[m][l]->ruletype) break;
		if (m != nevents) {
		  nr++;
		  print_instance(fpR,events[l]->event);
		  if (l % 2==1) 
		    fprintf(fpR," %s+ 0 0 0\n",signal);
		  else
		    fprintf(fpR," %s- 0 0 0\n",signal);
#ifdef KRONOS_CMP
		  nr++;
		  if (l % 2==1) 
		    fprintf(fpR,"%s- ",signal);
		  else
		    fprintf(fpR,"%s+ ",signal);
		  print_instance(fpR,events[l]->event);
		  if (((initvalue==0) && (l % 2==1)) || 
		      ((initvalue==1) && (l % 2==0)))
		    fprintf(fpR," 1 %d ",lower);
		  else 
		    fprintf(fpR," 0 %d ",lower);
		  if (upper==INFIN) fprintf(fpR,"inf\n"); 
		  else fprintf(fpR,"%d\n",upper);
#endif
		}
	      }
#ifdef KRONOS_CMP
	      lower=1;
	      upper=INFIN;
#endif
	      /* Emit conflicts between input signal instances */
	      for (l=signals[k]->event;
		   (l<nevents && (events[l]->signal == k));l+=2)
		for (m=l+2;(m<nevents && (events[m]->signal == k));m+=2) {
		  nc++;
		  print_instance(fpC,events[l]->event);
		  fprintf(fpC," ");
		  print_instance(fpC,events[m]->event);
		  fprintf(fpC,"\n");
		  nc++;
		  print_instance(fpC,events[l+1]->event);
		  fprintf(fpC," ");
		  print_instance(fpC,events[m+1]->event);
		  fprintf(fpC,"\n");
		}
	    }
	    /* Emit input signal instances */
	    if (j % 2==1) {
	      nd+=2;
	      ne+=2;
	      print_instance(fpS,events[j]->event);
	      fprintf(fpS,"\n");
	      print_instance(fpS,events[j+1]->event);
	      fprintf(fpS,"\n");
	    }
	  } else { 
	    /* Emit input signal */
	    if (j % 2==1) {
	      ni+=2;
	      ne+=2;
	      fprintf(fpI,"%s %s\n",events[j]->event,events[j+1]->event);

#ifdef KRONOS_CMP
	      if (initvalue==0) {
		fprintf(fpR,"%s- %s+ 1 %d ",signal,signal,lower);
		if (upper==INFIN) fprintf(fpR,"inf\n"); 
		else fprintf(fpR,"%d\n",upper);
		fprintf(fpR,"%s+ %s- 0 %d ",signal,signal,lower);
		if (upper==INFIN) fprintf(fpR,"inf\n"); 
		else fprintf(fpR,"%d\n",upper);
	      } else {
		fprintf(fpR,"%s+ %s- 1 %d ",signal,signal,lower);
		if (upper==INFIN) fprintf(fpR,"inf\n"); 
		else fprintf(fpR,"%d\n",upper);
		fprintf(fpR,"%s- %s+ 0 %d ",signal,signal,lower);
		if (upper==INFIN) fprintf(fpR,"inf\n"); 
		else fprintf(fpR,"%d\n",upper);
	      }
	      nr+=2;
	      lower=1;
	      upper=INFIN;
#endif

	    }
	  }
	  /* Emit rules with enabled input events */
	  for (i=0;i<nevents;i++)
	    if (rules[i][j]->ruletype) {
	      if (rules[i][j]->expr) {
		print_instance(fpR,events[i]->event);
		fprintf(fpR," ");
		print_instance(fpR,events[j]->event);
		if (strchr(events[j]->event,'-')) {
		  fprintf(fpR," %s %d %d ",rules[i][j]->expr,
			  rules[i][j]->epsilon,lower);
		  if (upper==INFIN) fprintf(fpR,"inf\n");
		  else fprintf(fpR,"%d\n",upper);
		} else {
		  fprintf(fpR," %s %d %d ",rules[i][j]->expr,
			  rules[i][j]->epsilon,lowerf);
		  if (upperf==INFIN) fprintf(fpR,"inf\n");
		  else fprintf(fpR,"%d\n",upperf);
		} 
	      } else { 
		print_instance(fpR,events[i]->event);
		fprintf(fpR," ");
		print_instance(fpR,events[j]->event);
		if (strchr(events[j]->event,'-')) {
		  fprintf(fpR," %d %d ",rules[i][j]->epsilon,lowerf);
		  if (upperf==INFIN) fprintf(fpR,"inf\n"); 
		  else fprintf(fpR,"%d\n",upperf);
		} else {
		  fprintf(fpR," %d %d ",rules[i][j]->epsilon,lower);
		  if (upper==INFIN) fprintf(fpR,"inf\n"); 
		  else fprintf(fpR,"%d\n",upper);
		}
	      }
	      nr++;
	    }
	}
      }
    }
    if (token != END_OF_FILE)
      token=fgettok_ac(fp,tokvalue,MAXTOKEN,linenum);
  }

  for (i=ninputs+1;i<nevents;i++)
    if ((strchr(events[i]->event,'/')) &&
	((atoi(strchr(events[i]->event,'/')+1)==1))) {
      for (int j=i;(j<nevents && 
		   (events[j]->signal == events[i]->signal));j+=2) {
	/* Check if event instance is used */
	for (m=1;m<nevents;m++)
	  if (rules[j][m]->ruletype) break;
	/* Emit rule between output signal and its event instance */
	if (m != nevents) {
	  nr++;
	  print_signal(fpR,events[i]->event);
	  fprintf(fpR," ");
	  print_instance(fpR,events[j]->event);
	  fprintf(fpR," 0 0 0\n");
	}
      }
      /* Emit conflicts between output event instances */
      for (int j=i;(j<nevents && 
		   (events[j]->signal == events[i]->signal));j+=2) {
	for (int k=j+2;(k<nevents && 
		      (events[k]->signal == events[i]->signal));k+=2) {
	  nc++;
	  print_instance(fpC,events[j]->event);
	  fprintf(fpC," ");
	  print_instance(fpC,events[k]->event);
	  fprintf(fpC,"\n");
	  nc++;
	  print_expected_instance(fpC,events[j]->event);
	  fprintf(fpC," ");
	  print_expected_instance(fpC,events[k]->event);
	  fprintf(fpC,"\n");
	}
      }
    }

  for (i=1;i<nevents;i++)
    for (j=ninputs+1;j<(nevents-ndummy);j++) {
      if (strchr(events[j]->event,'/')) { 
	/* Emit output event instances */
	if ((i==1) && (j % 2==1)) {
	  ne+=2;
	  nd+=2;
	  print_expected_instance(fpS,events[j]->event);
	  fprintf(fpS,"\n");
	  print_expected_instance(fpS,events[j+1]->event);
	  fprintf(fpS,"\n");
	  ne+=2;
	  nd+=2;
	  print_instance(fpS,events[j]->event);
	  fprintf(fpS,"\n");
	  print_instance(fpS,events[j+1]->event);
	  fprintf(fpS,"\n");
	}
	/* Emit rules with enabled output event instances */
	if (rules[i][j]->ruletype) {
	  if (rules[i][j]->expr) {
	    print_instance(fpR,events[i]->event);
	    fprintf(fpR," ");
	    print_instance(fpR,events[j]->event);
	    fprintf(fpR," %s %d 0 0\n",rules[i][j]->expr,
		    rules[i][j]->epsilon);
	    print_instance(fpR,events[i]->event);
	    fprintf(fpR," ");
	    print_expected_instance(fpR,events[j]->event);
	    fprintf(fpR," %s %d 0 0\n",rules[i][j]->expr,
		    rules[i][j]->epsilon);
#ifdef CHK_BEHV
	    print_expected_instance(fpD,events[j]->event);
	    fprintf(fpD," ");
	    print_signal(fpD,events[j]->event);
	    fprintf(fpD," %s %d 0 inf\n",rules[i][j]->expr,
		    rules[i][j]->epsilon);
#endif
	  } else { 
	    print_instance(fpR,events[i]->event);
	    fprintf(fpR," ");
	    print_instance(fpR,events[j]->event);
	    fprintf(fpR," %d 0 0\n",rules[i][j]->epsilon);
	    print_instance(fpR,events[i]->event);
	    fprintf(fpR," ");
	    print_expected_instance(fpR,events[j]->event);
	    fprintf(fpR," %d 0 0\n",rules[i][j]->epsilon);
#ifdef CHK_BEHV
	    print_expected_instance(fpD,events[j]->event);
	    fprintf(fpD," ");
	    print_signal(fpD,events[j]->event);
	    fprintf(fpD," %d 0 inf\n",rules[i][j]->epsilon);
#endif
	  }
	  nr+=2;
#ifdef CHK_BEHV
	  no++;
#endif
	}
      } else {
	/* Emit constraints on enabled output events */
#ifdef CHK_BEHV
	if (rules[i][j]->ruletype) {
	  if (rules[i][j]->expr) {
	    print_instance(fpD,events[i]->event);
	    fprintf(fpD," ");
	    print_instance(fpD,events[j]->event);
	    fprintf(fpD," %s %d 0 inf\n",rules[i][j]->expr,
		    rules[i][j]->epsilon);
	  } else { 
	    print_instance(fpD,events[i]->event);
	    fprintf(fpD," ");
	    print_instance(fpD,events[j]->event);
	    fprintf(fpD," %d 0 inf\n",rules[i][j]->epsilon);
	  }
	  no++;
	}
#endif
      }
    }
  /* Emit conflicts in specification file */
  for (i=1;i<nevents;i++)
    for (j=i+1;j<nevents;j++)
      if (rules[i][j]->conflict) {
	print_instance(fpC,events[i]->event);
	fprintf(fpC," ");
	print_instance(fpC,events[j]->event);
	fprintf(fpC,"\n");
	nc++;
      }
  /* Emit dummys and dummy rules in specification file */
  for (j=nevents-ndummy;j<nevents;j++) {
    fprintf(fpS,"%s\n",events[j]->event);
    nd++;
    ne++;
    for (i=1;i<nevents;i++) 
      if (rules[i][j]->ruletype) {
	if (rules[i][j]->expr) {
	  print_instance(fpR,events[i]->event);
	  fprintf(fpR," ");
	  print_instance(fpR,events[j]->event);
	  fprintf(fpR," %s %d 0 0\n",rules[i][j]->expr,
		  rules[i][j]->epsilon);
	} else { 
	  print_instance(fpR,events[i]->event);
	  fprintf(fpR," ");
	  print_instance(fpR,events[j]->event);
	  fprintf(fpR," %d 0 0\n",rules[i][j]->epsilon);
	}
	nr++;
      }
  }

  fclose(fpI);
  fclose(fpO);
  fclose(fpS);
  fclose(fpR);
  fclose(fpD);
  fclose(fpC);
  make_er(filename,ne,ni,nd,nr,no,0/*ndisj*/,nc,ssi,ss);
  return TRUE;
}

/*****************************************************************************/
/* Load a timed event-rule structure from a file.                            */
/*****************************************************************************/

bool load_gates(char * filename,eventADT *events,mergeADT *merges,
		ruleADT **rules,int nevents,int ninputs,int nrules,
		bool verbose,int nord,
		int nconf,int niconf,int noconf,int ncausal,int nsignals,
		int ninpsig,signalADT *signals,int ndummy)
{
  char inname[FILENAMESIZE];
  FILE *fp;
  int linenum;
  
  strcpy(inname,filename);
  strcat(inname,".acc");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE); 
  }
  printf("Loading gates from:  %s\n",inname);
  fprintf(lg,"Loading gates from:  %s\n",inname);
  linenum=1;
  if (!load_gate(fp,filename,nevents,ninputs,nrules,events,rules,nsignals,
		 signals,&linenum,ndummy)) {
    fclose(fp);
    return(FALSE);
  }
  fclose(fp);
  return(TRUE);
}

/*****************************************************************************/
/* Load ac gate file.                                                        */
/*****************************************************************************/

bool load_ac_gate_file(char menu,char command,designADT design)
{
  if (menu==LOAD && command==PRS) {
    if (load_gates(design->filename,design->events,design->merges,
		   design->rules,design->nevents,design->ninputs,
		   design->nrules,
		   design->verbose,design->nord,design->nconf,
		   design->niconf,design->noconf,design->ncausal,
		   design->nsignals,design->ninpsig,design->signals,
		   design->ndummy)) {
	return(TRUE);
    } else {
      return(FALSE);
    }
  }
  return(TRUE);
}



