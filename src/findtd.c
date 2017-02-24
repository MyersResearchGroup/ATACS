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
/* findtd.c                                                                  */
/*****************************************************************************/

#include "findtd.h"
#include "connect.h"

/*****************************************************************************/
/* Find the maximum time difference between two events for some general      */
/*   occurrence.                                                             */
/*****************************************************************************/

int maxdiffi(ruleADT **rules,cycleADT ****cycles,int nevents,int ncycles,
	     int e,int i,int f,int j)
{
  int d,k,m,newmax,max;

  if (cycles[e][f][i][j]->Ui==(NaN-1)) {
    printf("ERROR:  Cycle detected in acyclic constraint graph.\n");
    fprintf(lg,"ERROR:  Cycle detected in acyclic constraint graph.\n");
    printf("e = %d  f = %d\n",e,f);
    return NaN;
  }
  if ((e==f) && (i==j)) return(0);
  if (cycles[e][f][i][j]->Ui != NaN) return(cycles[e][f][i][j]->Ui);
  cycles[e][f][i][j]->Ui--;
  max=(-INFIN);
  if ((m=i-1)<0) m=i;
  for (d=0;d<nevents;d++)
    for (k=m;k<=i;k++)
    if (cycles[d][e][k][i]->rule) {
      cycles[d][f][k][j]->Ui=maxdiffi(rules,cycles,nevents,ncycles,d,k,f,j);
      if (cycles[d][f][k][j]->Ui==NaN) return NaN;
      if ((cycles[d][f][k][j]->Ui == INFIN) ||
	  (rules[d][e]->upper == INFIN))
	newmax = INFIN;
      else if (d!=0) newmax=cycles[d][f][k][j]->Ui + rules[d][e]->upper;
      else newmax=INFIN;
      if (max < newmax) max=newmax; 
    }
  if (max==(-INFIN)) max=INFIN;
  if (reachable(cycles,nevents,ncycles,e,i,f,j)==1) {
    if ((m=j-1)<0) m=j;
    for (d=0;d<nevents;d++)
      for (k=m;k<=j;k++)
	if (cycles[d][f][k][j]->rule) {
	  cycles[e][d][i][k]->Ui=maxdiffi(rules,cycles,nevents,ncycles,e,i,d,
					  k);
	  if (cycles[e][d][i][k]->Ui==NaN) return NaN;
	  if (cycles[e][d][i][k]->Ui == INFIN)
	    newmax = INFIN;
	  else if (d!=0) newmax=cycles[e][d][i][k]->Ui - rules[d][f]->lower;
	  else newmax=cycles[e][d][i][k]->Ui - 0;
	  if (newmax < max) max=newmax; 
	}
  }
  if (reachable(cycles,nevents,ncycles,e,i,f,j)==INFIN) return NaN;
  return(max);
}

/*****************************************************************************/
/* Find the maximum time difference between two events for some given        */
/*   occurrence.                                                             */
/*****************************************************************************/

int maxdiff(ruleADT **rules,cycleADT ****cycles,int nevents,int ncycles,
	    int e,int i,int f,int j)
{
  int d,k,m,newmax,max;

  if (cycles[e][f][i][j]->Uj==(NaN-1)) {
    printf("ERROR:  Cycle detected in acyclic constraint graph.\n");
    fprintf(lg,"ERROR:  Cycle detected in acyclic constraint graph.\n");
    printf("e = %d  f = %d\n",e,f);
    return NaN;
  }
  if ((e==f) && (i==j)) return(0);
  if (cycles[e][f][i][j]->Uj != NaN) return(cycles[e][f][i][j]->Uj);
  cycles[e][f][i][j]->Uj--;
  max=(-INFIN);
  if ((m=i-1)<0) m=i;
  for (d=0;d<nevents;d++)
    for (k=m;k<=i;k++)
      if (cycles[d][e][k][i]->rule) {
	cycles[d][f][k][j]->Uj=maxdiff(rules,cycles,nevents,ncycles,d,k,f,j);
      if (cycles[d][f][k][j]->Uj==NaN) return NaN;
	if ((cycles[d][f][k][j]->Uj == INFIN) ||
	    (rules[d][e]->upper == INFIN))
	  newmax = INFIN;
	else newmax=cycles[d][f][k][j]->Uj + rules[d][e]->upper;
	if (max < newmax) max=newmax;
      }
  if (max==(-INFIN)) max=INFIN;
  if (reachable(cycles,nevents,ncycles,e,i,f,j)==1) {
    if ((m=j-1)<0) m=j;
    for (d=0;d<nevents;d++)
      for (k=m;k<=j;k++)
	if (cycles[d][f][k][j]->rule) {
	  cycles[e][d][i][k]->Uj=maxdiff(rules,cycles,nevents,ncycles,e,i,d,k);
	  if (cycles[e][d][i][k]->Uj==NaN) return NaN;
	  if (cycles[e][d][i][k]->Uj == INFIN)
	    newmax = INFIN;
	  else newmax=cycles[e][d][i][k]->Uj - rules[d][f]->lower;
	  if (newmax < max) max=newmax; 
	}
  }
  if (reachable(cycles,nevents,ncycles,e,i,f,j)==INFIN) return NaN;
  return(max);
}

/*****************************************************************************/
/* Print a delay value.  If the delay is equal to INFIN, print "inf".     */
/*****************************************************************************/
  
void printdelay(FILE *fp,int delay)
{
  if (delay == (-1)*INFIN)
    fprintf(fp,"-inf");
  else if (delay == INFIN)
    fprintf(fp,"inf");
  else if (delay == NaN)
    fprintf(fp,"NaN");
  else
    fprintf(fp,"%d",delay);
}

/*****************************************************************************/
/* Print a time difference for a general occurrence.                         */
/*****************************************************************************/

void printtdi(FILE *fp,char * eventv,char * eventu,int j,int lower,int upper)
{
  if (j==0)
    fprintf(fp,"t(< %s,i >) - t(< %s,i >) = [ ",eventv,eventu);
  else
    fprintf(fp,"t(< %s,i >) - t(< %s,i-%d >) = [ ",eventv,eventu,j);
  printdelay(fp,lower);
  fprintf(fp,",");
  printdelay(fp,upper);
  fprintf(fp," ]\n");
}

/*****************************************************************************/
/* Print a time difference for a particular occurrence.                      */
/*****************************************************************************/

void printtd(FILE *fp,char * eventv,char * eventu,int i,int j,int lower,
	     int upper)
{
  fprintf(fp,"t(< %s,%d >) - t(< %s,%d >) = [ ",eventv,i,eventu,j);
  printdelay(fp,lower);
  fprintf(fp,",");
  printdelay(fp,upper);
  fprintf(fp," ]\n");
}

/*****************************************************************************/
/* Find all time differences.                                                */
/*****************************************************************************/

void find_td(char * filename,eventADT *events,ruleADT **rules,
	     cycleADT ****cycles,int nevents,int ncycles)
{
int e,f,i,j;
char outname[FILENAMESIZE];
FILE *fp;

strcpy(outname,filename);
strcat(outname,".td");
printf("Finding time differences and storing to:  %s\n",outname);
fprintf(lg,"Finding time differences and storing to:  %s\n",outname);
fp=Fopen(outname,"w");
for (e=0;e<nevents;e++)
  for (f=0;f<nevents;f++) 
    for (i=0;i<ncycles;i++) 
      for (j=0;j<ncycles;j++) {
	cycles[e][f][i][j]->Ui=maxdiffi(rules,cycles,nevents,ncycles,e,i,f,j);
	cycles[e][f][i][j]->Uj=maxdiff(rules,cycles,nevents,ncycles,e,i,f,j);
      }
if ((i=ncycles-1)<0) i=0;
if ((j=ncycles-1)<0) j=0;;
for (e=1;e<nevents;e++)
  for (f=1;f<nevents;f++) 
    printtdi(fp,events[e]->event,events[f]->event,0,
	    (-1)*cycles[f][e][j][i]->Ui,cycles[e][f][i][j]->Ui);
if ((j=j-1)<0) j=0;
for (e=1;e<nevents;e++)
  for (f=1;f<nevents;f++) 
    printtdi(fp,events[e]->event,events[f]->event,1,
	    (-1)*cycles[f][e][j][i]->Ui,cycles[e][f][i][j]->Ui);
if ((j=j-1)<0) j=0;
for (e=1;e<nevents;e++)
  for (f=1;f<nevents;f++) 
    printtdi(fp,events[e]->event,events[f]->event,2,
	    (-1)*cycles[f][e][j][i]->Ui,cycles[e][f][i][j]->Ui);
for (e=0;e<nevents;e++)
  for (f=0;f<nevents;f++) 
    for (i=0;i<ncycles;i++) 
      for (j=0;j<ncycles;j++) 
	if ((((i-j)==0) || ((i-j)==1) || ((i-j)==2))
	    && ((e!=0) || (i==0)) && ((f!=0) || (j==0)))
	  printtd(fp,events[e]->event,events[f]->event,i,j,
		  (-1)*cycles[f][e][j][i]->Uj,cycles[e][f][i][j]->Uj);
fclose(fp);
}
