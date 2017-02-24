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
/* delaycalc.c                                                               */
/*****************************************************************************/

#include "delaycalc.h"

/*****************************************************************************/
/* Print stacks of a gC subckt in HSPICE format.                             */
/*****************************************************************************/

void print_stack(FILE *fp,char * cover,int nsignals,bool rise,int stack)
{
  int j,literals;

  literals=0;
  for (j=0;j<nsignals;j++)
    if ((cover[j]=='1') || (cover[j]=='0'))
      literals++;
  if (rise)
    for (j=1;j<=literals;j++) {
      fprintf(fp,"m%dn%d",j,stack);
      if (j==literals) fprintf(fp,"\tOut_bar");
      else fprintf(fp,"\tn%di%d",j,stack);
      if (j==1) fprintf(fp,"\tIn%dn",stack);
      else fprintf(fp,"\tVdd");
      if (j==1) fprintf(fp,"\tGnd");
      else fprintf(fp,"\tn%di%d",j-1,stack);
      fprintf(fp,"\tGnd\tnmos\tl='minlen'\tw='%d*nwidth'\n",literals);
    }
  else
    for (j=1;j<=literals;j++) {
      fprintf(fp,"m%dp%d",j,stack);
      if (j==literals) fprintf(fp,"\tOut_bar");
      else fprintf(fp,"\tp%di%d",j,stack);
      if (j==1) fprintf(fp,"\tIn%dp",stack);
      else fprintf(fp,"\tGnd");
      if (j==1) fprintf(fp,"\tVdd");
      else fprintf(fp,"\tp%di%d",j-1,stack);
      fprintf(fp,"\tVdd\tpmos\tl='minlen'\tw='%d*pwidth'\n",literals);
    }
}

/*****************************************************************************/
/* Print vsupply voltages used in an HSPICE file.                             */
/*****************************************************************************/

void print_vsupply(FILE *fp,int maxnumstack)
{
  int i;

  fprintf(fp,"Vreset\tVReset\tGnd\tpulse ");
  fprintf(fp,"0 'vsupply' 4ns .1ns .1ns 500ns 1000ns\n");
  for (i=1;i<=maxnumstack;i++) {
    fprintf(fp,"Vin%dn\tVIn%dn\tGnd\tpulse ",i,i);
    fprintf(fp,"'vsupply' 0 %dns .1ns .1ns 14ns 500ns\n",30*(i-1)+6);
    fprintf(fp,"Vin%dp\tVIn%dp\tGnd\tpulse ",i,i);
    fprintf(fp,"0 'vsupply' %dns .1ns .1ns 14ns 500ns\n",30*(i-1)+21);
  }
}

/*****************************************************************************/
/* Print header for an HSPICE file.                                          */
/*****************************************************************************/

void print_HSPICE_header(FILE *fp,char * filename)
{
  fprintf(fp,"*** %s ***\n",filename);
  fprintf(fp,".temp 25\n");
  fprintf(fp,".param vSup = 5.0\n\n");
  fprintf(fp,".protect\n");
  fprintf(fp,".lib ");
  fprintf(fp,"'%s' TT\n",getenv("HSPICElib"));
  fprintf(fp,".unprotect\n\n");
  fprintf(fp,".width 132\n");
  fprintf(fp,".param nwidth = 4u\n");
  fprintf(fp,".param pwidth = 8u\n");
  fprintf(fp,".param minlen = 1u\n");
  fprintf(fp,".param vsupply = 5\n\n");
  fprintf(fp,".global\tVdd\tGnd\n\n");
  fprintf(fp,"Vdd\tVdd\tGnd\t'vsupply'\n\n");
  fprintf(fp,"x0pi\tVIn1n\tIn1nInv\tinv\n");
  fprintf(fp,"x0\tIn1nInv\tInv\tinv\n");
  fprintf(fp,"x0l\tInv\tInvLoad\tinv\tM=4\n");
  fprintf(fp,"c0\tInvLoad\tGnd\t800fF\n\n");
  fprintf(fp,".measure InvR\n");
  fprintf(fp,"+\tTRIG v(In1nInv) VAL='vsupply/2' FALL=1\n");
  fprintf(fp,"+\tTARG v(Inv) VAL='vsupply/2' RISE=1\n");
  fprintf(fp,".measure InvF\n");
  fprintf(fp,"+\tTRIG v(In1nInv) VAL='vsupply/2' RISE=1\n");
  fprintf(fp,"+\tTARG v(Inv) VAL='vsupply/2' FALL=1\n");
  fprintf(fp,".measure Delay\n");
  fprintf(fp,"+\tparam = '(InvR + InvF)/2'\n\n");
  fprintf(fp,".subckt\tinv\tIn\tOut\n");
  fprintf(fp,"m1\tOut\tIn\tGnd\tGnd\tnmos\tl='minlen'\tw='nwidth'\n");
  fprintf(fp,"m2\tOut\tIn\tVdd\tVdd\tpmos\tl='minlen'\tw='pwidth'\n");
  fprintf(fp,".ends\n\n");
  fprintf(fp,".subckt\tweakinv\tIn\tOut\n");
  fprintf(fp,"m1\tOut\tIn\tGnd\tGnd\tnmos\tl='2*minlen'\tw=1.6u\n");
  fprintf(fp,"m2\tOut\tIn\tVdd\tVdd\tpmos\tl='2*minlen'\tw=3.2u\n");
  fprintf(fp,".ends\n\n");
}

/*****************************************************************************/
/* Print a gC subckt in HSPICE format.                                       */
/*****************************************************************************/

void print_subckt(FILE *fp,signalADT *signals,int i,int upstacks,
		  int downstacks)
{
  int j;

  fprintf(fp,"xreset%d\tVReset\tReset%d\tinv\n",i,i);
  fprintf(fp,"xreset%dbar\tReset%d\tReset%d_bar\tinv\n",i,i,i);
  for (j=1;j<=downstacks;j++)
    fprintf(fp,"x%dp%di\tVIn%dp\tIn%dp%d\tinv\n",j,i,j,j,i);
  for (j=1;j<=upstacks;j++)
    fprintf(fp,"x%dn%di\tVIn%dn\tIn%dn%d\tinv\n",j,i,j,j,i);
  fprintf(fp,"x%d\tReset%d_bar\t",i,i);

  for (j=1;j<=downstacks;j++) 
    fprintf(fp,"In%dp%d\t",j,i);
  fprintf(fp,"\n+\t");
  for (j=1;j<=upstacks;j++) 
    fprintf(fp,"In%dn%d\t",j,i);
  
  fprintf(fp,"%s\t%s_bar\tgC_%s\n",signals[i]->name,signals[i]->name,
	  signals[i]->name);
  if (signals[i]->name[0] != 'x') {
    fprintf(fp,"x%dl\t%s\t%sLoad\tinv\tM=4\n",i,signals[i]->name,
	    signals[i]->name);
    fprintf(fp,"c%d\t%sLoad\tGnd\t800fF\n\n",i,signals[i]->name);
  } else {                                  /* TEMPORARY */
    fprintf(fp,"x%dl\t%s\t%sLoad\tinv\n",i,signals[i]->name,signals[i]->name);
    fprintf(fp,"c%d\t%sLoad\tGnd\t800fF\n\n",i,signals[i]->name);
  }
  for (j=1;(j<=upstacks || j<=downstacks);j++) {
    if (j<=upstacks) {
      fprintf(fp,".measure %sR%d\n",signals[i]->name,j);
      fprintf(fp,"+\tTRIG v(In%dn%d) VAL='vsupply/2' RISE=1\n",j,i);
      fprintf(fp,"+\tTARG v(%s) VAL = 'vsupply/2' RISE=%d\n",
	      signals[i]->name,j);
      fprintf(fp,".measure %sR%di\n",signals[i]->name,j);
      fprintf(fp,"+\tparam = '%sR%d/Delay'\n",signals[i]->name,j);
    }
    if (j<=downstacks) {
      fprintf(fp,".measure %sF%d\n",signals[i]->name,j);
      fprintf(fp,"+\tTRIG v(In%dp%d) VAL='vsupply/2' FALL=1\n",j,i);
      fprintf(fp,"+\tTARG v(%s) VAL = 'vsupply/2' FALL=%d\n",
	      signals[i]->name,j);
      fprintf(fp,".measure %sF%di\n",signals[i]->name,j);
      fprintf(fp,"+\tparam = '%sF%d/Delay'\n",signals[i]->name,j);
    }
  }
  fprintf(fp,"\n.subckt\tgC_%s\tReset_bar",signals[i]->name);

  for (j=1;j<=downstacks;j++) 
    fprintf(fp,"\tIn%dp",j);
  fprintf(fp,"\n+\t");
  for (j=1;j<=upstacks;j++) 
    fprintf(fp,"\tIn%dn",j);

  fprintf(fp,"\tOut\tOut_bar\n");
  fprintf(fp,"mreset\tOut_bar\tReset_bar\tVdd\tVdd");
  fprintf(fp,"\tpmos\tl='minlen'\tw='pwidth'\n");
}

/*****************************************************************************/
/* Print a staticizer for an HSPICE file.                                    */
/*****************************************************************************/

void print_staticizer(FILE *fp)
{
  fprintf(fp,"x1\tOut_bar\tOut\tinv\n");
  fprintf(fp,"x2\tOut\tOut_bar\tweakinv\n");
  fprintf(fp,".ends\n\n");
}

/*****************************************************************************/
/* Print tail to HSPICE file.                                                */
/*****************************************************************************/

void print_HSPICE_tail(FILE *fp,int maxnumstack)
{
  fprintf(fp,".opt post\n");
  fprintf(fp,".tran .1ns %dns\n",maxnumstack*30);
  fprintf(fp,".end\n");
}

#define TOKEN 1

/*****************************************************************************/
/* Get a token from a file.  Used for loading PR sets.                       */
/*****************************************************************************/

int fgettokmt0(FILE *fp,char * tokvalue,int maxtok)
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readtok;

  readtok=FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '\n':
    case ' ':
      if (readtok) return(TOKEN);
      break;
    default:
      if (toklen < maxtok) {
        readtok=TRUE;
        *tokvalue++=c;
        *tokvalue='\0';
        toklen++;
      }
      break;
    }
  }
  if (!readtok)
    return(EOF);
  else
    return(TOKEN);
}

void load_delays(char * filename,signalADT *signals,int nsignals,int ninpsig)
{
  char inname[FILENAMESIZE];
  FILE *fp;
  char tokvalue[MAXTOKEN];
  int token;
  int i;

  strcpy(inname,filename);
  strcat(inname,".mt0");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return;
  }
  token=fgettokmt0(fp,tokvalue,MAXTOKEN);
  while ((token != EOF) && (strcmp(tokvalue,"alter#")))
    token=fgettokmt0(fp,tokvalue,MAXTOKEN);
  token=fgettokmt0(fp,tokvalue,MAXTOKEN);
  token=fgettokmt0(fp,tokvalue,MAXTOKEN);
  token=fgettokmt0(fp,tokvalue,MAXTOKEN);
  for (i=ninpsig;i<nsignals;i++) {
    printf("%s\t: ",signals[i]->name);
    fprintf(lg,"%s\t: ",signals[i]->name);
    token=fgettokmt0(fp,tokvalue,MAXTOKEN);
    token=fgettokmt0(fp,tokvalue,MAXTOKEN);
    printf("Rise Delay = %s ",tokvalue);
    fprintf(lg,"Rise Delay = %s ",tokvalue);
    token=fgettokmt0(fp,tokvalue,MAXTOKEN);
    token=fgettokmt0(fp,tokvalue,MAXTOKEN);
    printf("Fall Delay = %s\n",tokvalue);
    fprintf(lg,"Fall Delay = %s\n",tokvalue);
  }
  fclose(fp);
}

/*****************************************************************************/
/* Calculate delays using HSPICE.                                            */
/*****************************************************************************/

void delaycalc(char * filename,signalADT *signals,stateADT *state_table,
               regionADT *regions,int ninpsig,int nsignals,bool verbose)
{
  char shellcommand[100];
  char outname[FILENAMESIZE];
  FILE *fp;
  regionADT cur_region;

  int i;
  int stack,maxnumstack,upstacks,downstacks,stacks;

  strcpy(outname,filename);
  strcat(outname,".hsp");
  fp=Fopen(outname,"w");
  printf("Storing HSPICE simulation file to:  %s\n",outname);
  fprintf(lg,"Storing HSPICE simulation file to:  %s\n",outname);
  print_HSPICE_header(fp,filename);

  maxnumstack=0;
  for (i=ninpsig;i<nsignals;i++) {
    upstacks=0;
    downstacks=0;
    for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') upstacks++;
    for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') downstacks++;
    stacks=upstacks;
    if (stacks < downstacks) stacks=downstacks;
    print_subckt(fp,signals,i,upstacks,downstacks);
    stack=0;
    for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
	stack++;
	print_stack(fp,cur_region->cover,nsignals,TRUE,stack);
      }
    stack=0;
    for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
	stack++;
	print_stack(fp,cur_region->cover,nsignals,FALSE,stack);
      }
    if (stack > maxnumstack) maxnumstack=stack;
    print_staticizer(fp);
  }
  print_vsupply(fp,maxnumstack);
  print_HSPICE_tail(fp,maxnumstack);
  fclose(fp);

  sprintf(shellcommand,"hspice %s.hsp > %s.lis",filename,filename);
  printf("Executing command:  %s\n",shellcommand);
  fprintf(lg,"Executing command:  %s\n",shellcommand);
  system(shellcommand);
  load_delays(filename,signals,nsignals,ninpsig);
}


