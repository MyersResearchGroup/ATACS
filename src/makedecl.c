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
/* makedecl.c - miscellaneous routines used in timed event-rule structure    */
/*           generation.                                                     */
/*****************************************************************************/

//#include "misclib.h"
#include "ly.h"
#include "actions.h"
#include "types.h"
#include "auxilary.h"
#include "parser.tab.h"

/*****************************************************************************/
/* Assign delays to events on a signal.                                      */
/*****************************************************************************/

void assigndelays(delayADT *del,int lr,int ur,char *distr,
		  int lf,int uf,char *distf)
{
  del->lr=lr;
  del->ur=ur;
  del->distr=CopyString(distr);
  del->lf=lf;
  del->uf=uf;
  del->distf=CopyString(distf);
}

/*****************************************************************************/
/* Declare a constant.                                                       */
/*****************************************************************************/
void makeconstantdecl(actionADT A,int intval)
{
  char errmsg[80];

  if (A->type != UNKNOWN) { 
    sprintf(errmsg,"%s is multiply defined.",A->label);
    //yyerror(errmsg);
  }
  A->type=CONSTANT;
  A->intval=intval;
}

/*****************************************************************************/
/* Declare a delay.                                                          */
/*****************************************************************************/
void makedelaydecl(actionADT A,delayADT del)
{
  char errmsg[80];

  if (A->type != UNKNOWN) { 
    sprintf(errmsg,"%s is multiply defined.",A->label);
    //yyerror(errmsg);
  }
  A->type=ATACS_DELAY;
  assigndelays(&(A->delay),del.lr,del.ur,del.distr,del.lf,del.uf,del.distf);
}

/*****************************************************************************/
/* Declare a real macro.                                                     */
/*****************************************************************************/
void makedefinedecl(actionADT A,double real_def)
{
  char errmsg[80];

  if (A->type != UNKNOWN) { 
    sprintf(errmsg,"%s is multiply defined.",A->label);
    //yyerror(errmsg);
  }
  A->type=ATACS_DEFINE;
  A->real_def=real_def;
}

/*****************************************************************************/
/* Declare an action.                                                        */
/*****************************************************************************/
void makeactiondecl(int type,actionADT A,bool initial,int lower,int upper,
		    char *dist)
{
  char errmsg[80];

  if (A->type != UNKNOWN) { 
    sprintf(errmsg,"%s is multiply defined.",A->label);
    //yyerror(errmsg);
  }
  A->type=type;
  A->initial=initial;
  //printf("MAD: %s %d\n",A->label,A->initial);
  A->lower=lower;
  A->upper=upper;
  A->dist=CopyString(dist);
}

/*****************************************************************************/
/* Declare a signal.                                                         */
/*****************************************************************************/
void makesignaldecl(int type, actionADT baselabel,bool initial,delayADT *del)
{
  actionADT rise,fall;
  char label[80];

  sprintf(label,"%s+",baselabel->label);
  rise=(actionADT)action(label,strlen(label));
  rise->initial_state = baselabel->initial_state;
  if (del)
    makeactiondecl(type,rise,initial,del->lr,del->ur,del->distr);
  else
    makeactiondecl(type,rise,initial,0,INFIN,NULL);
  if(!(rise->initial)) 
    rise->vacuous=FALSE;
  
  sprintf(label, "%s-", baselabel->label);
  fall=(actionADT)action(label,strlen(label));
  fall->initial_state = baselabel->initial_state;
  if (del)
    makeactiondecl(type,fall,initial,del->lf,del->uf,del->distf);
  else
    makeactiondecl(type,fall,initial,0,INFIN,NULL);
  if(fall->initial) 
    fall->vacuous = FALSE;

/*TEMP*/
  rise->inverse=fall;
  fall->inverse=rise;
}

/*****************************************************************************/
/* Declare a boolean.                                                        */
/*****************************************************************************/
void makebooldecl(int type,actionADT A,bool initial,delayADT *del)	
{
  char errmsg[80];

  if (A->type != UNKNOWN) { 
    sprintf(errmsg,"%s is multiply defined.",A->label);
    //yyerror(errmsg);
  }
  A->type=type;
  A->initial=initial;
  makesignaldecl(type,A,initial,del);
}

/*****************************************************************************/
/* Declare a port for communication.                                         */
/*****************************************************************************/
void makeportdecl(int type,actionADT A,bool initial,int olr,int our,int olf,
		  int ouf,int ilr,int iur,int ilf,int iuf,int odata,int idata)
{
  /*
  char errmsg[80];
  char label[80];
  int i;

  if (A->type != UNKNOWN) { 
    sprintf(errmsg,"%s is multiply defined.",A->label);
    //yyerror(errmsg);
  }
  A->type=type;
  A->odata=odata;
  A->idata=idata;

  for (i=0;i<odata;i++) { 
    if (odata > 1) sprintf(label,"%s%do",A->label,i);
    else sprintf(label,"%so",A->label);
    makesignaldecl(OUT,label,initial,olr,our,olf,ouf);
  }
  for (i=0;i<idata;i++) { 
    if (idata > 1) sprintf(label,"%s%di",A->label,i);
    else sprintf(label,"%si",A->label);
    makesignaldecl(IN,label,initial,ilr,iur,ilf,iuf);
  }
  */
}

/*****************************************************************************/
/* Check if type is "delay".                                                 */
/*****************************************************************************/

void checkdelay(char * label,int type)
{
  char errmsg[255];

  if (type == UNKNOWN) {
    sprintf(errmsg,"Undeclared delay definition %s.",label);
    //yyerror(errmsg);
  } else if (type != ATACS_DELAY) {
    sprintf(errmsg,"Expected %s to be delay definition.",label);
    //yyerror(errmsg);
  }
}

/*****************************************************************************/
/* Check if type is "define".                                                */
/*****************************************************************************/

void checkdefine(char * label,int type)
{
  char errmsg[255];

  if (type == UNKNOWN) {
    sprintf(errmsg,"Undeclared real definition %s.",label);
    //yyerror(errmsg);
  } else if (type != ATACS_DEFINE) {
    sprintf(errmsg,"Expected %s to be real definition.",label);
    //yyerror(errmsg);
  }
}
