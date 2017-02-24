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
/* findcl.c                                                                  */
/*****************************************************************************/

#include "findcl.h"

#ifdef TSE_ALGM

#include <system.h>

System sys;
Event **TSEevents=NULL;

#endif

/*****************************************************************************/
/* Use TSE to find worst-case time differences.                              */
/*****************************************************************************/

boundADT TSE_WCTimeDiff(int u,int v,int j)
{
  boundADT timediff=NULL;

  timediff=(boundADT)GetBlock(sizeof *timediff);
#ifdef TSE_ALGM
  timediff->L = (-1)*sys.MaxSep(TSEevents[v],TSEevents[u],-j);
  timediff->Lbest = timediff->L;
  timediff->U = sys.MaxSep(TSEevents[u],TSEevents[v],j);
  timediff->Ubest = timediff->U;
#endif
  return(timediff);
}

#ifdef TSE_ALGM

/*****************************************************************************/
/* Initialize structures used by TSE to find worst-case time differences.    */
/*****************************************************************************/

void init_tse(char * filename,eventADT *events,ruleADT **rules,int nevents)
{
  int u,v;
  char rulename[FILENAMESIZE];
  int count=0;

  sys.set_name(filename);
  TSEevents = (Event **)GetBlock(nevents * sizeof(*TSEevents));
  for (u=1;u<nevents;u++)
    TSEevents[u] = sys.add_event(events[u]->event);
  for (u=1;u<nevents;u++)
    for (v=1;v<nevents;v++)
      if (rules[u][v]->ruletype != NORULE) {
	sprintf(rulename,"d%d",count++);
	sys.add_rule(rulename,TSEevents[u],TSEevents[v],rules[u][v]->lower,
		     rules[u][v]->upper,rules[u][v]->epsilon);
      }
  count=0;
  Event *root = sys.find_event("root");
  for (v=1;v<nevents;v++)
    if (rules[0][v]->ruletype != NORULE) {
      sprintf(rulename,"i%d",count++);
      sys.add_rule(rulename,root,TSEevents[v],rules[0][v]->lower,
		   rules[0][v]->upper,rules[0][v]->epsilon);
    }
  sys.initialize();
  sys.set_opt(OPT_QUIET);
}

/*****************************************************************************/
/* Free structures used by TSE to find worst-case time differences.          */
/*****************************************************************************/

void free_tse()
{
  sys.clear_questions();
  sys.clear();
  free(TSEevents);
}

#endif
