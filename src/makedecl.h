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
/* makedecl.h - miscellaneous routines to make declarations.                 */
/*****************************************************************************/
#ifndef _makedecl_h
#define _makedecl_h

#include "misclib.h"

/*****************************************************************************/
/* Assign delays to events on a signal.                                      */
/*****************************************************************************/

void assigndelays(delayADT *del,int riselower,int riseupper,char *distr,
                  int falllower,int fallupper,char *distf);

/*****************************************************************************/
/* Declare a constant.                                                       */
/*****************************************************************************/
void makeconstantdecl(actionADT A,int intval);

/*****************************************************************************/
/* Declare a delay.                                                          */
/*****************************************************************************/
void makedelaydecl(actionADT A,delayADT del);

/*****************************************************************************/
/* Declare a real macro.                                                     */
/*****************************************************************************/
void makedefinedecl(actionADT A,double real_def);

/*****************************************************************************/
/* Declare a boolean.                                                        */
/*****************************************************************************/
void makebooldecl(int type,actionADT A,bool initial,delayADT *del);

/*****************************************************************************/
/* Declare a port for communication.                                         */
/*****************************************************************************/
void makeportdecl(int type,actionADT A,bool initial,int olr,int our,int olf,
		  int ouf,int ilr,int iur,int ilf,int iuf,int odata,int idata);

/*****************************************************************************/
/* Check if type is "delay".                                                 */
/*****************************************************************************/

void checkdelay(char * label,int type);

/*****************************************************************************/
/* Check if type is "define".                                                */
/*****************************************************************************/

void checkdefine(char * label,int type);

#endif /* makedecl.h */


