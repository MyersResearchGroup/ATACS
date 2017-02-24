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
/* findwctd.c                                                                */
/*****************************************************************************/

#include "findwctd.h"
#include "findcl.h"
#include "findtd.h"

/*****************************************************************************/
/* Find the worst-case time difference between two events for any occurrence.*/
/*****************************************************************************/

boundADT WCTimeDiff(ruleADT **rules,cycleADT ****cycles,int nevents,int 
		    ncycles,int u,int v,int j)
{
  boundADT timediff=NULL;
  int Lk,Uk,k;

  timediff=(boundADT)GetBlock(sizeof *timediff);
  timediff->L=(-INFIN);
  timediff->U=INFIN;
  timediff->Lbest=INFIN;
  timediff->Ubest=(-INFIN);
  if (j>=ncycles) return(timediff);
  else {
    cycles[u][v][ncycles-j-1][ncycles-1]->Ui=
      maxdiffi(rules,cycles,nevents,ncycles,u,ncycles-j-1,v,ncycles-1);
    timediff->L=(-1)*cycles[u][v][ncycles-j-1][ncycles-1]->Ui;
    cycles[v][u][ncycles-1][ncycles-j-1]->Ui=
      maxdiffi(rules,cycles,nevents,ncycles,v,ncycles-1,u,ncycles-j-1);
    timediff->U=cycles[v][u][ncycles-1][ncycles-j-1]->Ui;
    for (k=j;k<ncycles-1;k++) {
      cycles[u][v][k-j][k]->Uj=maxdiff(rules,cycles,nevents,ncycles,u,k-j,v,k);
      Lk=(-1)*cycles[u][v][k-j][k]->Uj;
      cycles[v][u][k][k-j]->Uj=maxdiff(rules,cycles,nevents,ncycles,v,k,u,k-j);
      Uk=cycles[v][u][k][k-j]->Uj;
      if (Lk < timediff->L) timediff->L=Lk;
      if (Uk > timediff->U) timediff->U=Uk;
      if (Lk < timediff->Lbest) timediff->Lbest=Lk;
      if (Uk > timediff->Ubest) timediff->Ubest=Uk;
    }
    return(timediff);
  }
}

/*****************************************************************************/
/* Print a worst-case time difference.                                       */
/*****************************************************************************/

void printwctd(FILE *fp,char * eventv,char * eventu,int j,boundADT timediff)
{
  if (j==0)
    fprintf(fp,"t(< %s,i >) - t(< %s,i >) = [ ",eventv,eventu);
  else
    fprintf(fp,"t(< %s,i >) - t(< %s,i-%d >) = [ ",eventv,eventu,j);
  printdelay(fp,timediff->L);
  fprintf(fp,",");
  printdelay(fp,timediff->U);
  if ((timediff->Lbest > timediff->L) || (timediff->Ubest < timediff->U)) {
    fprintf(fp," ] : Best case = [ ");
    printdelay(fp,timediff->Lbest);
    fprintf(fp,",");
    printdelay(fp,timediff->Ubest);
  }
  fprintf(fp," ]\n");
}

/*****************************************************************************/
/* Find all worst-case time differences.                                     */
/*****************************************************************************/

void find_wctd(char * filename,eventADT *events,ruleADT **rules,
	       cycleADT ****cycles,int nevents,int ncycles,bool closure)
{
int u,v;
boundADT timediff; 
char outname[FILENAMESIZE];
FILE *fp;

strcpy(outname,filename);
strcat(outname,".wctd");
printf("Finding worst-case time differences and storing to:  %s\n",outname);
fprintf(lg,"Finding worst-case time differences and storing to:  %s\n",
	outname);
fp=Fopen(outname,"w");
for (v=1;v<nevents;v++) 
  for (u=1;u<nevents;u++) {
    if (closure)
      timediff=TSE_WCTimeDiff(u,v,0);
    else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,u,v,0);
    printwctd(fp,events[v]->event,events[u]->event,0,timediff);
    free(timediff);
  }
for (v=1;v<nevents;v++)
  for (u=1;u<nevents;u++) {
    if (closure) 
      timediff=TSE_WCTimeDiff(u,v,1);
    else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,u,v,1);
    printwctd(fp,events[v]->event,events[u]->event,1,timediff);
    free(timediff);
  }
for (v=1;v<nevents;v++) 
  for (u=1;u<nevents;u++) {
    if (closure) 
      timediff=TSE_WCTimeDiff(u,v,2);
    else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,u,v,2);
    printwctd(fp,events[v]->event,events[u]->event,2,timediff);
    free(timediff);
  }
fclose(fp);
}
