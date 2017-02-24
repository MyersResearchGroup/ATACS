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

#ifndef _ctlchecker_h
#define _ctlchecker_h

#include "struct.h"

enum ops {OP_NOT, OP_OR, OP_AND, OP_EX, OP_EU, OP_EG, OP_VAR, OP_TRUE};

typedef struct ctlstmt {
  enum ops op;
  struct ctlstmt *s1;
  struct ctlstmt *s2;
  char* var;
} ctlstmt;

ctlstmt* newctlstmt(enum ops op, ctlstmt* s1, ctlstmt* s2, char* var);

bool checkCTL(stateADT *state_table,signalADT *signals,int nsignals,
              int nevents,int nplaces,eventADT *events,ruleADT **rules,
              char *property);
set<stateADT> checkCTLRec(ctlstmt* c, stateADT *state_table,
                          map<string,int>& stateVec,set<stateADT>& U,
                          int nevents,int nplaces,eventADT *events,
                          ruleADT **rules,signalADT *signals,int nsignals);
set<stateADT> witnessCTLRec(ctlstmt* c, stateADT *state_table,
                          map<string,int>& stateVec, set<stateADT>& U);
set<stateADT> findUniverse(stateADT *state_table);
set<stateADT> atomicGetStates(stateADT *state_table,int signal);
set<stateADT> checkNot(set<stateADT> f1, set<stateADT> U);
set<set<stateADT> > witnessNot(set<set<stateADT> > f1, set<stateADT> U);
set<stateADT> checkOr(set<stateADT> f1, set<stateADT> f2);
set<set<stateADT> > witnessOr(set<set<stateADT> > f1, set<set<stateADT> > f2);
set<stateADT> checkAnd(set<stateADT> f1, set<stateADT> f2);
set<set<stateADT> > witnessAnd(set<set<stateADT> > f1, set<set<stateADT> > f2);
set<stateADT> checkEX(set<stateADT> f1);
set<stateADT> witnessEX(set<stateADT> f1);
set<stateADT> checkEU(set<stateADT> f1,set<stateADT> f2);
set<stateADT> witnessEU(set<stateADT> f1,set<stateADT> f2);
set<stateADT> checkEG(set<stateADT> f1,stateADT *state_table,int nevents,
                      int nplaces,eventADT *events,ruleADT **rules,
		      signalADT *signals,int nsignals);
set<stateADT> witnessEG(set<stateADT> f1,stateADT *state_table);
void printSet(set<stateADT> s);
void printSetOfSets(set<set<stateADT> > s);

void stateDFS(stateADT *state_table);
void stateDFSVisit(stateADT u, int &time);
set<set<stateADT> > stateSCC(stateADT *state_table);
void stateSCCHelp(stateADT u, set<stateADT> &T);
void createSubGraph(set<stateADT> S,stateADT *state_table);
void prepColorSubGraph(set<stateADT> S,stateADT *state_table);

#endif  /* ctlchecker.h */

