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
/* storenet.c                                                                */
/*****************************************************************************/

#include "storenet.h"

/*****************************************************************************/
/* Print an AND gate.                                                        */
/*****************************************************************************/

void print_AND_gate(FILE *fp,signalADT *signals,char * cover,int i,
		    int nsignals,bool noOR,bool comb,int output)
{
  int j;
  bool first;
  int nliterals;

  nliterals=0;
  for (j=0;j<nsignals;j++)
    if ((cover[j] == '0') || (cover[j] == '1')) 
      nliterals++;
/*  if ((nliterals > 1) || (noOR)) { */
  if (nliterals > 1) {
    fprintf(fp,"a%d and%d",i,nliterals);
/*
    else
      fprintf(fp,"a%d wire",i);
*/
    nliterals=0;
    first=TRUE;
    for (j=0;j<nsignals;j++)
      if ((cover[j] == '0') || (cover[j] == '1')) {
	nliterals++;
	if (cover[j] == '0') {
	  if (first) {
	    first=FALSE;
	    fprintf(fp,"-n-%d",nliterals);
	  } else fprintf(fp,"-%d",nliterals);
	}
      }
    fprintf(fp," ");
    for (j=0;j<nsignals;j++)
      if ((cover[j] == '0') || (cover[j] == '1'))
	fprintf(fp,"%s ",signals[j]->name);
    if (noOR && comb)
      fprintf(fp,"%s\n",signals[output]->name);
    else
      fprintf(fp,"u_%d\n",i);
  }
}

/*****************************************************************************/
/* Print an OR gate.                                                         */
/*****************************************************************************/

int print_OR_gate(FILE *fp,signalADT *signals,regionADT *regions,int nsignals,
		  int i,bool comb,int output)
{
  int nstacks,j,nliterals,lit;
  regionADT cur_region;
  bool noOR,first;

  nstacks=0;
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') nstacks++;
  if (nstacks > 1) noOR=FALSE; else noOR=TRUE;
  nstacks=0;
  for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
    if (cur_region->cover[0] != 'E') {
      nstacks++;
      if (noOR)
	print_AND_gate(fp,signals,cur_region->cover,i+nsignals,nsignals,noOR,
		       comb,output);
      else 
	print_AND_gate(fp,signals,cur_region->cover,(3+i)*nsignals+nstacks,
		       nsignals,noOR,comb,output);
    }
  if (nstacks==1) {
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
        nliterals=0;
        for (j=0;j<nsignals;j++)
          if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
            if (cur_region->cover[j]=='1') lit=j; else lit=j+nsignals;
            nliterals++;
          }
      }
    if (nliterals==1) return(lit);
  } else if (nstacks > 1) {
    fprintf(fp,"o%d or%d",i,nstacks);
    nstacks=0;
    first=TRUE;
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
        nliterals=0;
        for (j=0;j<nsignals;j++)
          if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
            lit=j;
            nliterals++;
          }
        nstacks++;
        if ((nliterals==1) && (cur_region->cover[lit]=='0'))
          if (first) {
            first=FALSE;
             fprintf(fp,"-n-%d",nstacks);
          } else fprintf(fp,"-%d",nstacks);
      }
    fprintf(fp," ");
    nstacks=0;
    for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0] != 'E') {
        nliterals=0;
        for (j=0;j<nsignals;j++)
          if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
            lit=j;
            nliterals++;
          }
	nstacks++;
	if (nliterals==1)
	  fprintf(fp,"%s ",signals[lit]->name);
	else
	  fprintf(fp,"u_%d ",(3+i)*nsignals+nstacks);
      }
    if (comb)
      fprintf(fp,"%s\n",signals[output]->name);
    else
      fprintf(fp,"u_%d\n",i+nsignals);
  }
  return(-1);
}

/*****************************************************************************/
/* Print a gate-level netlist.                                               */
/*****************************************************************************/

void print_netlist(FILE *fp,signalADT *signals,regionADT *regions,
		   int ninpsig,int nsignals)
{
  int i,set,reset;
  int sands,rands;
  regionADT cur_region;

  for (i=0;i<nsignals;i++) {
    if (i < ninpsig)
      fprintf(fp,"input\t");
    else 
      fprintf(fp,"output\t");
    fprintf(fp,"%s\n",signals[i]->name);
  }
  set=(-1);
  reset=(-1);
  for (i=ninpsig;i<nsignals;i++) {
    sands=0;
    for (cur_region=regions[2*i+1];cur_region;cur_region=cur_region->link)
      if (cur_region->cover[0]!='E') sands++;
    rands=0;
    for (cur_region=regions[2*i+2];cur_region;cur_region=cur_region->link) 
      if (cur_region->cover[0]!='E') rands++;
    if (sands > 0)
      set=print_OR_gate(fp,signals,regions,nsignals,2*i+1,(rands==0),i);
    if (rands > 0)
      reset=print_OR_gate(fp,signals,regions,nsignals,2*i+2,FALSE,i);
    if ((rands==0) && (set != (-1))) {
      if (set >= nsignals) 
	fprintf(fp,"b%d buf-n-1 %s %s\n",i,signals[set-nsignals]->name,
		signals[i]->name);
      else
	fprintf(fp,"b%d buf %s %s\n",i,signals[set]->name,signals[i]->name);
    } else if (sands==0) {
      if (reset >= nsignals)
	fprintf(fp,"b%d buf %s %s\n",i,signals[reset-nsignals]->name,
		signals[i]->name);
      else if (reset >= 0)
	fprintf(fp,"b%d buf-n-1 %s %s\n",i,signals[reset]->name,
		signals[i]->name);
      else
	fprintf(fp,"b%d buf-n-1 u_%d %s\n",i,2*i+2+nsignals,
		signals[i]->name);
    } else {
      fprintf(fp,"c%d c2",i);
      if ((reset < nsignals) || (set >= nsignals)) fprintf(fp,"-n");
      if (reset < nsignals) fprintf(fp,"-1");
      if (set >= nsignals) fprintf(fp,"-2");
      if (reset >= nsignals) fprintf(fp," %s",signals[reset-nsignals]->name);
      else if (reset >= 0) fprintf(fp," %s",signals[reset]->name);
      else fprintf(fp," u_%d",2*i+2+nsignals);
      if (set >= nsignals) fprintf(fp," %s",signals[set-nsignals]->name);
      else if (set >= 0) fprintf(fp," %s",signals[set]->name);
      else fprintf(fp," u_%d",2*i+1+nsignals);
      fprintf(fp," %s\n",signals[i]->name);
    }
  }
}

/*****************************************************************************/
/* Store netlist file to interface with SYN.                                 */
/*****************************************************************************/

void store_net(char * filename,signalADT *signals,stateADT *state_table,
	       regionADT *regions,int ninpsig,int nsignals,bool combo)
{
  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,filename);
  strcat(outname,".net");
  printf("Storing netlist to:  %s\n",outname);
  fprintf(lg,"Storing netlist to:  %s\n",outname);
  fp=Fopen(outname,"w");
  print_netlist(fp,signals,regions,ninpsig,nsignals);
  fclose(fp);
}
