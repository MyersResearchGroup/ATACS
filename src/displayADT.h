/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2003 by, Scott R. Little                                  */
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

/* Note to future users
 * I originally wrote this file so that I could really understand the
 * ATACS data structures.  I was having trouble figuring out how
 * things worked exactly so I decided to write a bunch of functions
 * that would print out the data structure that I use on a regular
 * basis in some useful way.  I hope it will be useful to others in
 * the future to debugging and understanding what is contained in the
 * data structures and how to access the data. It really only works
 * well for small examples.  I just picked a small example that I
 * wanted to understand and played around with that.  It worked well
 * enough to help solve my understanding problems. */

#ifndef __displayADT_h__
#define __displayADT_h__

#include "struct.h"
#include "def.h"
#include "memaloc.h"
#include "hpnrsg.h"
#include <vector>

void printRuleADT(ruleADT **rules,int nevents,int nplaces,eventADT *events);

void printMarkingADT(markingADT marking,int nevents,int nrules,
                     eventADT *events);

void printMarkingADT(dMarkingADT marking,int nevents,int nrules,
                     eventADT *events);

void printConciseMarkingADT(dMarkingADT marking,int nevents,int nrules,
			    int nsignals,signalADT *signals,eventADT *events,
			    markkeyADT *markkey);

void printMarkkeyADT(markkeyADT *markkey,int nrules,eventADT *events);

void printEventADT(eventADT *events,int nevents,int nplaces);

void printVerboseEventADT(eventADT *events,int nevents,int nplaces,
                          signalADT *signals,int nsignals);

void printVerboseEvent(eventADT *events,int t,signalADT *signals,int nsignals);

void printSignalADT(signalADT *signals,int nsignals);

void printClockkeyADT(clockkeyADT clockkey,int num);

void printClocksADT(clocksADT clocks);

void printClocklistADT(clocklistADT cl,eventADT *events);

void fprintClocklistADT(FILE *fp,clocklistADT cl,eventADT *events);

void printClocklistADT(clocklistADT cl);

void printLevelExp(level_exp level);

void printCStateADT(cStateADT cState,eventADT *events,int nevents);

void printIneqADT(ineqADT ineq,eventADT *events,signalADT *signals);

void printIneq(ineqADT ineq,eventADT *events,signalADT *signals);

void printIneqList(vector<ineqADT> ineqL,eventADT *events,signalADT *signals);

#endif /* displayADT.h */
