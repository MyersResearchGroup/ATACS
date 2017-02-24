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

#include "lpntrsfm.h"
#include "symbolic.h"
#include "yices/yices_c.h"
#include <fstream>
#include <limits>
#include <algorithm>

#ifdef CUDD
//#define __timeElapseNew__
//#define __timeElapseNew2__
//#define __timeElapseNew3__

/* Debug Defines */
//#define __SYM_CLEANUP__
//#define __SYM_PROMPT_TO_PRINT__
//#define __SYM_PRINT_VARS__ 
//#define __SYM_REPRINT_VARS__  // Reprint nodes if they already exist
//#define __SYM_INVARIANT__
//#define __SYM_INITSTATE__
//#define __SYM_PRINTRATE__
//#define __SYM_TRANSRELS__
//#define __SYM_TRANSRELSMERGE__ // transition relation merge debug
//#define __SYM_PRINTFINALTRANSRELS__
//#define __SYM_PRINTDISSTATEVECTOR__
//#define __SYM_PRINTRATEVECTOR__
//#define __SYM_PRINTDISSTATEEXP__ // discrete state exploration debug
//#define __SYM_PRINTSTEPS__
//#define __SYM_PRINTCONS_FULL__  // Print transitivity constraints when added
//#define __SYM_PRINTCONS_NUMS__
//#define __SYM_PRINTABST__  // Print before and after abstraction statement
//#define __SYM_USEOPT1__
//#define __SYM_USEOPT2__
#define __SYM_FIXPOINTITERS__
#define __SYM_GUARDCONS__

//#define __SYM_UNOPTIMIZETRANSCONS__
//#define __SYM_FORCEUNOPTIMIZEDTIMEELAPSE__

// Use the following define if rate assignments are on discrete transitions.
//#define __SYM_RATES_ON_DISTRANS__ 

//#define MEMSTATS

extern void printTraceUsingSMT(lhpnModelData* md, BDD iState, BDD& invariant, 
			       transvector& transRels, vector<BDD> trace);

#ifdef MEMSTATS
#ifndef OSX
struct mallinfo memuse;
#else 
malloc_statistics_t t;
#endif
#endif

double threshold = .00001;
extern int tctlparse(void*);
tmustmt* tmuroot;
// Store the textual representation of the property to be verified.
char tctlprop[MAXTOKEN];
char *tctlpropptr; /* current position in tctlprop */
int  tctlproplim;  /* end of data */


using namespace std;

// indices into special positions in predbddmap for negated delta and
// negated epsilon predicates.
// Declared static so they're only available for use within this
// source file. 2/16/07 - Removed the static so that smtsym.c could access
// variables
int N_EPS_I = -1; 
int N_DLT_I = -1;
int X0_I = -1;

//////////////////////////////////////////////////////////////////////////////
// Various function to construct timed mu calculus statements.

tmustmt* not_tmustmt(tmustmt* tmu1) {
  if (tmu1->op == TM_SL && tmu1->sl == tmu1->sl.manager()->bddOne()) {
    tmu1->sl = tmu1->sl.manager()->bddZero();
    return tmu1;
  }
  if (tmu1->op == TM_SL && tmu1->sl == tmu1->sl.manager()->bddZero()) {
    tmu1->sl = tmu1->sl.manager()->bddOne();
    return tmu1;
  }
  if (tmu1->op == TM_NOT) {
    tmustmt* ts = tmu1->tmu1;
    tmu1->tmu1 = NULL;
    delete_tmustmt(tmu1);
    return ts;
  }
  tmustmt* ts = new tmustmt();
  ts->op = TM_NOT;
  ts->tmu1 = tmu1;
  ts->tmu2 = NULL;
  return ts;
}

tmustmt* or_tmustmt(tmustmt* tmu1, tmustmt* tmu2) {
  if (tmu1->op == TM_SL && tmu1->sl == tmu1->sl.manager()->bddOne()) {
    delete_tmustmt(tmu2);
    return tmu1;
  }
  if (tmu1->op == TM_SL && tmu1->sl == tmu1->sl.manager()->bddZero()) {
    delete_tmustmt(tmu1);
    return tmu2;
  }
  if (tmu2->op == TM_SL && tmu2->sl == tmu2->sl.manager()->bddOne()) {
    delete_tmustmt(tmu1);
    return tmu2;
  }
  if (tmu2->op == TM_SL && tmu2->sl == tmu2->sl.manager()->bddZero()) {
    delete_tmustmt(tmu2);
    return tmu1;
  }
  tmustmt* ts = new tmustmt();
  ts->op = TM_OR;
  ts->tmu1 = tmu1;
  ts->tmu2 = tmu2;
  return ts;
}

tmustmt* and_tmustmt(tmustmt* tmu1, tmustmt* tmu2) {
  if (tmu1->op == TM_SL && tmu1->sl == tmu1->sl.manager()->bddOne()) {
    delete_tmustmt(tmu1);
    return tmu2;
  }
  if (tmu1->op == TM_SL && tmu1->sl == tmu1->sl.manager()->bddZero()) {
    delete_tmustmt(tmu2);
    return tmu1;
  }
  if (tmu2->op == TM_SL && tmu2->sl == tmu2->sl.manager()->bddOne()) {
    delete_tmustmt(tmu2);
    return tmu1;
  }
  if (tmu2->op == TM_SL && tmu2->sl == tmu2->sl.manager()->bddZero()) {
    delete_tmustmt(tmu1);
    return tmu2;
  }
  tmustmt* ts = new tmustmt();
  ts->op = TM_AND;
  ts->tmu1 = tmu1;
  ts->tmu2 = tmu2;
  return ts;
}

tmustmt* ns_tmustmt(tmustmt* tmu1, tmustmt* tmu2) {
  tmustmt* ts = new tmustmt();
  ts->op = TM_NS;
  ts->tmu1 = tmu1;
  ts->tmu2 = tmu2;
  return ts;
}

tmustmt* sc_tmustmt(int z, tmustmt* tmu1) {
  tmustmt* ts = new tmustmt();
  ts->op = TM_SC;
  ts->z = z;
  ts->tmu1 = tmu1; 
  ts->tmu2 = NULL;
  return ts;
}

tmustmt* lfp_tmustmt(string X, tmustmt* tmu1) {
  tmustmt* ts = new tmustmt();
  ts->op = TM_LFP;
  ts->X = X;
  ts->tmu1 = tmu1;
  ts->tmu2 = NULL;
  return ts;
}

tmustmt* sl_tmustmt(BDD phi, string desc) {  
  tmustmt* ts = new tmustmt();
  ts->op = TM_SL;
  ts->sl = phi;
  ts->description = desc;
  ts->tmu1 = NULL;
  ts->tmu2 = NULL;
  return ts;
}

tmustmt* var_tmustmt(string X) {
  tmustmt* ts = new tmustmt();
  ts->op = TM_VAR;
  ts->X = X; 
  ts->tmu1 = NULL;
  ts->tmu2 = NULL;
  return ts;
}

// Constructs the equivalent timed mu calculus statement for the CTL
// statement AU.
tmustmt* AU_tmustmt(tmustmt* tmu1, tmustmt* tmu2, modelData* md) {
  // create a new unique fixpoint variable name.
  static int i;
  char var[10];
  sprintf(var, "_X%i", i);
  string svar = string(var);
  i++;
  // create a new unique clock variable name.
  static int j;
  char cvar[10];
  sprintf(cvar, "_c1%i", j);
  int cIndex = md->addClockVar(cvar);
  j++;


  // create the predicate z > c where c > 0 (in this case 1)
  int c = -100;
  BDD phi = !predStruct::addPred(X0_I, cIndex, c);
  ostringstream desc;
  desc << "x0 >= " << md->getVarName(cIndex) << " + " << c;
    // build up the proper timed mu statement.
  tmustmt* result = or_tmustmt(tmu1, var_tmustmt(svar));
  result = not_tmustmt(result);
  result = or_tmustmt(result, not_tmustmt(sl_tmustmt(phi, desc.str())));
  result = EU_tmustmt(not_tmustmt(var_tmustmt(svar)), result);
  result = not_tmustmt(sc_tmustmt(cIndex, result));
  result = lfp_tmustmt(svar, or_tmustmt(tmu2, result));
  return result;
}

// Constructs the equivalent timed mu calculus statement for the
// bounded CTL statement AU where the bound is of the form
// (new clock) <= constant
tmustmt* bAU_tmustmt(tmustmt* tmu1, tmustmt* tmu2, double constant,
                     modelData* md) {

  // create a new unique clock variable name.
  static int j;
  char cvar[10];
  sprintf(cvar, "_c2%i", j);
  int cIndex = md->addClockVar(cvar);
  j++;

  ostringstream desc;
  desc <<  cvar << " > " << constant;
  BDD phi = !predStruct::addPred(X0_I, cIndex, -constant);
  tmustmt* mu = sl_tmustmt(phi, desc.str());
  mu = or_tmustmt(not_tmustmt(or_tmustmt(tmu1, copy_tmustmt(tmu2))), mu);
  mu = EU_tmustmt(not_tmustmt(tmu2), mu);
  mu = not_tmustmt(sc_tmustmt(cIndex, mu));
  return mu;
}

// Constructs the equivalent timed mu calculus statement for the CTL
// statement EU.
tmustmt* EU_tmustmt(tmustmt* tmu1, tmustmt* tmu2) {
  // create a new unique fixpoint variable name.
  static int i;
  char var[10];
  sprintf(var, "_Y%i", i);
  string svar = string(var);
  i++;
  tmustmt* result;
  result = ns_tmustmt(tmu1, var_tmustmt(svar));
  result = lfp_tmustmt(svar, or_tmustmt(tmu2, result));
  return result;
}

// Constructs the equivalent timed mu calculus statement for the CTL
// statement AU.
tmustmt* EF_tmustmt(tmustmt* tmu1, modelData* md) {
  return EU_tmustmt(sl_tmustmt(md->getBDDMgr()->bddOne(), "true"), tmu1);
}

// Constructs the equivalent timed mu calculus statement for the CTL
// statement AU.
tmustmt* AG_tmustmt(tmustmt* tmu1, modelData* md) {
  return not_tmustmt(EF_tmustmt(not_tmustmt(tmu1), md));
}

// Constructs the equivalent timed mu calculus statement for the CTL
// statement AF.
tmustmt* AF_tmustmt(tmustmt* tmu1, modelData* md) {
  return AU_tmustmt(sl_tmustmt(md->getBDDMgr()->bddOne(), "true"), tmu1,  md);
}

// Constructs the equivalent timed mu calculus statement for the
// bounded CTL statement AF where the bound is of the form
// (new clock) <= constant
tmustmt* bAF_tmustmt(tmustmt* tmu1, double constant, modelData* md) {
  return bAU_tmustmt(sl_tmustmt(md->getBDDMgr()->bddOne(), "true"),
                     tmu1, constant, md);
}


// Constructs the equivalent timed mu calculus statement for the CTL
// statement EG.
tmustmt* EG_tmustmt(tmustmt* tmu1, modelData* md) {
  return not_tmustmt(AF_tmustmt(not_tmustmt(tmu1), md));
}

// Constructs the equivalent timed mu calculus statement for the
// bounded CTL statement EG where the bound is of the form
// (new clock) <= constant
tmustmt* bEG_tmustmt(tmustmt* tmu1, double constant, modelData* md) {
  return not_tmustmt(bAF_tmustmt(not_tmustmt(tmu1), constant, md));
}


tmustmt* copy_tmustmt(tmustmt* tmu) {
  if (tmu == NULL)
    return NULL;
  
  tmustmt* copy = new tmustmt();
  copy->op = tmu->op;
  copy->sl = tmu->sl;
  copy->z = tmu->z;
  copy->X = tmu->X;
  copy->description = tmu->description;
  copy->tmu1 = copy_tmustmt(tmu->tmu1);
  copy->tmu2 = copy_tmustmt(tmu->tmu2);
  return copy;
}


string tmustmtToString(tmustmt* tmu, modelData* md) {
  ostringstream result;
  switch(tmu->op) {
  case TM_NOT:
    result << "!(" << tmustmtToString(tmu->tmu1, md) << ")";
    break;
  case TM_OR:
    result << "(" << tmustmtToString(tmu->tmu1, md) << " + "
	   << tmustmtToString(tmu->tmu2, md) << ")";
    break;
  case TM_AND:
    result << "(" << tmustmtToString(tmu->tmu1, md) << " * "
	   << tmustmtToString(tmu->tmu2, md) << ")";
    break;
  case TM_NS:
    result << "(" << tmustmtToString(tmu->tmu1, md) << " |> "
	   <<  tmustmtToString(tmu->tmu2, md) << ")";
    break;
  case TM_SC:
    result << md->getVarName(tmu->z) << ".(" 
	   << tmustmtToString(tmu->tmu1, md) << ")";
    break;
  case TM_LFP:
    result << "u" << tmu->X << ".(" 
	   << tmustmtToString(tmu->tmu1, md) << ")";
    break;
  case TM_SL:
    if (tmu->description == "")
      result << "BDD";
    else
      result << tmu->description;
    break;
  case TM_VAR:
    result << tmu->X;
    break;
  }
  return result.str();
}

void delete_tmustmt(tmustmt* tmu) {
  if (tmu == NULL)
    return;  
  delete_tmustmt(tmu->tmu1);
  delete_tmustmt(tmu->tmu2);
  tmu->tmu1 = NULL;
  tmu->tmu2 = NULL;  
  delete tmu;
}


//////////////////////////////////////////////////////////////////////////////

modelData* predStruct::md = NULL;
Cudd* predStruct::mgr = NULL;
predbddmap predStruct::predbdd;
bddpredmap predStruct::bddpred;

predStruct::predStruct(int i, int j, double c) {
  predStruct(1, i, 1, j, c);
}

predStruct::predStruct(double ci, int i, double cj, int j, double c) {
  assert(ci == 1 || ci == -1);
  this->md = md;  
  this->i = i;
  this->j = j;
  this->ci = ci;
  this->cj = cj;
  this->c = c;
}

void predStruct::initialize(modelData* md) {
  predStruct::md = md;
  predStruct::mgr = predStruct::md->getBDDMgr();
}

BDD predStruct::addPred(int i, int j, double c) {
  return addPred(1, i, 1, j, c);
}

// Creates a new BDD variable representing a predicate of the form
// rulei >= rulej + c
BDD predStruct::addPred(double ci, int i, double cj, int j, double c) {
  if (c == -INFIN) {
    c = -numeric_limits<double>::infinity();
  }
  else if (c==INFIN) {
    c = numeric_limits<double>::infinity();
  }
  validate(ci, i, cj, j, c);
  
  if (predStruct::isTrue(ci, i, cj, j, c)) {
// #ifdef __SYM_REPRINT_VARS__
//       cSetFg(CYAN);
//       cout << "DPredicate: true" << endl;
//       cSetAttr(NONE);
// #endif
    return mgr->bddOne();
  }
  if (predStruct::isFalse(ci, i, cj, j, c)) {
// #ifdef __SYM_REPRINT_VARS__
//     cSetFg(CYAN);
//     cout << "DPredicate: false" << endl;
//     cSetAttr(NONE);
// #endif
    return mgr->bddZero();
  }

  predStruct::canonicalize(ci, i, cj, j, c);
  
  // resize the predbdd if necessary
  if ((int) predbdd.size() < i+1) {
    predbdd.resize(i+1);
  }
  if ((int) predbdd[i].size() < j+1) {
    predbdd[i].resize(j+1);
  }
  
  // Check if the same predicate already exists.
  // If it does, return the same BDD variable.
  for (unsigned int k = 0; k < predbdd[i][j].size(); k++) {
    if (fabs(predbdd[i][j][k]->c - c) < threshold &&
        fabs(predbdd[i][j][k]->ci - ci) < threshold &&
        fabs(predbdd[i][j][k]->cj - cj) < threshold) {
#ifdef __SYM_REPRINT_VARS__
      cSetFg(CYAN);
      cout << "DPredicate: " << predbdd[i][j][k]->toString();
      cout << "\t Index: " << predbdd[i][j][k]->var << endl;
      cSetAttr(NONE);
#endif      
      return mgr->bddVar(predbdd[i][j][k]->var);
    }
  }
  // create corresponding c variables
//  if ((i == X0_I)&&(j < md->design->nplaces)) {
//    BDD newBddVar;
//    predStruct *newPred = new predStruct(ci, j+md->design->nplaces, cj, j, c);
//    newBddVar = mgr->bddVar();
//    newPred->var = newBddVar.NodeReadIndex();
//    predbdd[j+md->design->nplaces][j].push_back(newPred);
//    bddpred.resize(newPred->var+1, NULL);
//    bddpred[newPred->var] = newPred;
//#ifdef __SYM_PRINT_VARS__
//    cSetFg(CYAN);
//    cout << "Predicate: " << newPred->toString();
//    cout << "\t Index: " << newPred->var << endl;
//    cSetAttr(NONE);
//#endif
//  }
//  if ((j == X0_I)&&(i < md->design->nplaces)) {
//    BDD newBddVar;
//    predStruct *newPred = new predStruct(ci, i, cj, i+md->design->nplaces, c);
//    newBddVar = mgr->bddVar();
//    newPred->var = newBddVar.NodeReadIndex();
//    predbdd[i][i+md->design->nplaces].push_back(newPred);
//    bddpred.resize(newPred->var+1, NULL);
//    bddpred[newPred->var] = newPred;
//#ifdef __SYM_PRINT_VARS__
//    cSetFg(CYAN);
//    cout << "Predicate: " << newPred->toString();
//    cout << "\t Index: " << newPred->var << endl;
//    cSetAttr(NONE);
//#endif
//  }
//  // end of create corresponding c variables
  BDD newBddVar;
  predStruct *newPred = new predStruct(ci, i, cj, j, c);
  newBddVar = mgr->bddVar();
  newPred->var = newBddVar.NodeReadIndex();
  predbdd[i][j].push_back(newPred);
  bddpred.resize(newPred->var+1, NULL);
  bddpred[newPred->var] = newPred;
#ifdef __SYM_PRINT_VARS__
  cSetFg(CYAN);
  cout << "Predicate: " << newPred->toString();
  cout << "\t Index: " << newPred->var << endl;
  cSetAttr(NONE);
#endif
  return newBddVar;
}

// This function creates a predStruct for temporary use.  It does not
// allocat a BDD to the predicate and it does not add it to the
// predbddmap or bddpredmap.  The caller is responsible for deleting
// the predStruct that is allocated by getTempPred.
predStruct* predStruct::getTempPred(double ci, int i,
                                    double cj, int j, double c) {
  validate(ci, i, cj, j, c);
  
  if (predStruct::isTrue(ci, i, cj, j, c)) {
    assert(false);
  }
  if (predStruct::isFalse(ci, i, cj, j, c)) {
    assert(false);
  }

  predStruct::canonicalize(ci, i, cj, j, c);
  return new predStruct(ci, i, cj, j, c);
}


bool predStruct::predExists(double ci, int i, double cj, int j, double c) {
  validate(ci, i, cj, j, c);
  if (predStruct::isTrue(ci, i, cj, j, c)) {
    return true;
  }
  else if (predStruct::isFalse(ci, i, cj, j, c)) {
    return true;
  }
  canonicalize(ci, i, cj, j, c);
  // resize the predbdd if necessary
  if ((int) predbdd.size() < i+1) {
    predbdd.resize(i+1);
  }
  if ((int) predbdd[i].size() < j+1) {
    predbdd[i].resize(j+1);
  }
  for (unsigned int k = 0; k < predbdd[i][j].size(); k++) {
    if (fabs(predbdd[i][j][k]->c - c) < threshold &&
        fabs(predbdd[i][j][k]->ci - ci) < threshold &&
        fabs(predbdd[i][j][k]->cj - cj) < threshold) {
      return true;
    }
  }
  return false;
}


unsigned int predStruct::getPredCount() {
  return bddpred.size();
}

predStruct* predStruct::getPred(int bddIndex) {
  if (bddIndex >= (int) bddpred.size()) return NULL;
  return bddpred[bddIndex];
}


bool predStruct::timpliest(predStruct* p1, predStruct* p2) {
  if (p1->i == p2->i && p1->j == p2->j &&
      fabs(p1->ci - p2->ci) < threshold && fabs(p1->cj - p2->cj) < threshold &&
      p1->c > p2->c) {
    return true;
  }
  return false;
}

bool predStruct::fimpliesf(predStruct* p1, predStruct* p2) {
  if (p1->i == p2->i && p1->j == p2->j &&
      fabs(p1->ci - p2->ci) < threshold && fabs(p1->cj - p2->cj) < threshold &&
      -p1->c > -p2->c) {
    return true;
  }
  return false;
}

bool predStruct::timpliesf(predStruct* p1, predStruct* p2) {
  if (((p1->ci > 0) && (p1->cj > 0) && (p2->ci > 0) && (p2->cj > 0)))
  {
    if (p1->i == p2->j && p1->j == p2->i &&
        fabs(p1->cj/p1->ci - p2->ci/p2->cj) < threshold &&
        p1->c/p1->ci - -1*(p2->c/p2->cj) > threshold) {
      /*
      cout << "p1->i=" << p1->i << " p1->ci=" << p1->ci << " p1->j=" 
	   << p1->j << " p1->cj=" << p1->cj << " p1->c=" << p1->c << endl;
      cout << "p2->i=" << p2->i << " p2->ci=" << p2->ci  << " p2->j=" 
	   << p2->j << " p2->cj=" << p2->cj << " p2->c=" << p2->c << endl;
      cout << "-p2->c/p2->cj=" << -p2->c/p2->cj << endl;
      */
      return true;
    } 
  } else if (((p1->ci < 0) && (p1->cj > 0) && (p2->ci > 0) && (p2->cj < 0))||
             ((p1->ci > 0) && (p1->cj < 0) && (p2->ci < 0) && (p2->cj > 0))) {
    if (p1->i == p2->i && p1->j == p2->j &&
        fabs(p1->ci/p1->cj - p2->ci/p2->cj) < threshold &&
        (p1->c - -p2->c) > threshold) {
      return true;
    } 
  }
  return false;
  /*
  if (p1->i == p2->j && p1->j == p2->i &&
      fabs(p1->ci - p2->cj/fabs(p2->cj)) < threshold &&
      fabs(p1->cj - p2->ci/fabs(p2->cj)) < threshold &&
      p1->c > -p2->c/fabs(p2->cj)) {
    return true;
  }
  return false;
  */
}

bool predStruct::fimpliest(predStruct* p1, predStruct* p2) {
  if (((p1->ci > 0) && (p1->cj > 0) && (p2->ci > 0) && (p2->cj > 0)))
  {
    if (p1->i == p2->j && p1->j == p2->i &&
        fabs(p1->ci/p1->cj - p2->cj/p2->ci) < threshold &&
        -p1->c/p1->cj - p2->c/p2->ci > threshold) {
      return true;
    } 
  } else if (((p1->ci < 0) && (p1->cj > 0) && (p2->ci > 0) && (p2->cj < 0))||
             ((p1->ci > 0) && (p1->cj < 0) && (p2->ci < 0) && (p2->cj > 0))) {
    if (p1->i == p2->i && p1->j == p2->j &&
        fabs(p1->ci/p1->cj - p2->ci/p2->cj) < threshold &&
        -p1->c - p2->c > threshold) {
      return true;
    } 
  }
  return false;
  
  /*
  if (p1->j == p2->i && p1->i == p2->j &&
      fabs(p1->cj/fabs(p1->cj) - p2->ci) < threshold &&
      fabs(p1->ci/fabs(p1->cj) - p2->cj) < threshold &&
      -p1->c/fabs(p1->cj) >= p2->c) {
    return true;
  }
  return false;
  */
}

BDD predStruct::implies(predStruct* pi, predStruct* pj) {
  BDD result = mgr->bddOne();  
  if (pi == pj) {
    return result;
  }
  if (predStruct::timpliest(pi, pj)) {
    result *= !pi->bdd() + pj->bdd();
#ifdef __SYM_PRINTCONS_FULL__
    cSetFg(BLUE);
    cout << pi->toString() << " --> " << pj->toString() << endl;
    cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
    cSetFg(BLUE);
    cout << pi->var << " --> " << pj->var << endl;
    cSetAttr(NONE);
#endif
  }
  if (predStruct::fimpliesf(pi, pj)) {
    result *= pi->bdd() + !pj->bdd();
#ifdef __SYM_PRINTCONS_FULL__
    cSetFg(BLUE);
    cout << pi->toStringInverted() << " --> "
         << pj->toStringInverted() << endl;
    cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
    cSetFg(BLUE);
    cout <<"!" << pi->var << " --> !" << pj->var << endl;
    cSetAttr(NONE);
#endif
  }
  if (predStruct::timpliesf(pi, pj)) {
    result *= !pi->bdd() + !pj->bdd();
#ifdef __SYM_PRINTCONS_FULL__
    cSetFg(BLUE);
    cout << pi->toString() << " --> " << pj->toStringInverted() << endl;
    cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
    cSetFg(BLUE);
    cout << pi->var << " --> !" << pj->var << endl;
    cSetAttr(NONE);
#endif
  }
  if (predStruct::fimpliest(pi, pj)) {
    result *= pi->bdd() + pj->bdd();
#ifdef __SYM_PRINTCONS_FULL__
    cSetFg(BLUE);
    cout << pi->toStringInverted() << " --> " << pj->toString() << endl;
    cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
    cSetFg(BLUE);
    cout <<"!" << pi->var << " --> " << pj->var << endl;
    cSetAttr(NONE);
#endif
  }
  return result;
}



BDD predStruct::trans(predStruct* p1, predStruct* p2, int pivot,
                      bool create, BDD* phi) {
  if (p1 == p2) return mgr->bddOne();
  // Transitivity Constraints of Type A as specified in Sanjit
  // Seshia's paper.
  int i, j;
  double ci, cj, c;
  bool p1inverted;
  bool p2inverted;
  if (p1->i == pivot && p1->ci > 0) {
    i = p1->j;
    ci = p1->cj/p1->ci;
    c = -p1->c/p1->ci;
    p1inverted = true;
  }
  else if (p1->i == pivot && p1->ci < 0) {
    i = p1->j;
    ci = p1->cj/p1->ci;
    c = -p1->c/p1->ci;
    p1inverted = false;    
  }
  else if (p1->j == pivot && p1->cj > 0) {
    i = p1->i;
    ci = p1->ci/p1->cj;
    c = p1->c/p1->cj;    
    p1inverted = false;
  }
  else if (p1->j == pivot && p1->cj < 0) {
    i = p1->i;
    ci = p1->ci/p1->cj;
    c = p1->c/p1->cj;    
    p1inverted = true;
  }
  else {
    return mgr->bddOne();
  }
  
  if (p2->i == pivot && p2->ci > 0) {
    j = p2->j;
    cj = p2->cj/p2->ci;
    c = c + p2->c/p2->ci;
    p2inverted = false;
  }
  else if (p2->i == pivot && p2->ci < 0) {
    j = p2->j;
    cj = p2->cj/p2->ci;
    c = c + p2->c/p2->ci;
    p2inverted = true;
  }
  else if (p2->j == pivot && p2->cj > 0) {
    j = p2->i;
    cj = p2->ci/p2->cj;
    c = c + -p2->c/p2->cj;
    p2inverted = true;
  }
  else if (p2->j == pivot && p2->cj < 0) {
    j = p2->i;
    cj = p2->ci/p2->cj;
    c = c + -p2->c/p2->cj;
    p2inverted = false;
  }
  else {
    return mgr->bddOne();
  }
  BDD cons = md->getBDDMgr()->bddOne();  
  BDD p1bdd = md->getBDDMgr()->bddVar(p1->var);
  BDD p2bdd = md->getBDDMgr()->bddVar(p2->var);
  BDD pred;
  if (!p1inverted && !p2inverted) {
    if (create || predStruct::predExists(ci, i, cj, j, c)) {
      pred = predStruct::addPred(ci, i, cj, j, c);
      bool add = true;
      if ((pred != md->getBDDMgr()->bddOne()) &&
	  (pred != md->getBDDMgr()->bddZero()) &&
	  (phi != NULL)) {
        set<int> support = getSupportSet(mgr, *phi);
        if (support.find(pred.NodeReadIndex()) == support.end()) {
          add = false;
        }
      }
      if (add) {
        cons *= (!(p1bdd * p2bdd) + pred);
#ifdef __SYM_PRINTCONS_FULL__        
        cSetFg(BLUE);
        cout << p1->toString() << " * " << p2->toString() << " --> " 
             << predStruct::toString(pred) << endl;
        cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
        cSetFg(BLUE);
        cout << p1->var << " * " << p2->var << " --> "
             << pred.NodeReadIndex() << endl;
        cSetAttr(NONE);
#endif
      }
    }
  }
  else if (p1inverted && !p2inverted) {
    if (create || predStruct::predExists(ci, i, cj, j, c)) {
      pred = predStruct::addPred(ci, i, cj, j, c);
      bool add = true;
      if ((pred != md->getBDDMgr()->bddOne()) &&
	  (pred != md->getBDDMgr()->bddZero()) &&
	  (phi != NULL)) {
        set<int> support = getSupportSet(mgr, *phi);
        if (support.find(pred.NodeReadIndex()) == support.end()) {
          add = false;
        }
      }
      if (add) {
        cons *= (!(!p1bdd * p2bdd) + pred);
#ifdef __SYM_PRINTCONS_FULL__
        cSetFg(BLUE);        
        cout << p1->toStringInverted() << " * " << p2->toString() << " --> "
             << predStruct::toString(pred) << endl;
        cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
        cSetFg(BLUE);
        cout << "!" << p1->var << " * " << p2->var << " --> " 
             << pred.NodeReadIndex() << endl;
        cSetAttr(NONE);
#endif
      }
    }
    if (create || predStruct::predExists(cj, j, ci, i, -c)) {
      pred = predStruct::addPred(cj, j, ci, i, -c);
      bool add = true;
      if ((pred != md->getBDDMgr()->bddOne()) &&
	  (pred != md->getBDDMgr()->bddZero()) &&
	  (phi != NULL)) {
        set<int> support = getSupportSet(mgr, *phi);
        if (support.find(pred.NodeReadIndex()) == support.end()) {
          add = false;
        }
      }
      if (add) {
        cons *= (!(!p1bdd * p2bdd) + !pred);
#ifdef __SYM_PRINTCONS_FULL__
        cSetFg(BLUE);
        cout << p1->toStringInverted() << " * " << p2->toString() << " --> "
             << predStruct::toStringInverted(pred) << endl;
        cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
        cSetFg(BLUE);
        cout << "!" << p1->var << " * " << p2->var << " --> !"
             << pred.NodeReadIndex() << endl;
        cSetAttr(NONE);
#endif
      }
    }
  }
  else if (!p1inverted && p2inverted) {
    if (create || predStruct::predExists(ci, i, cj, j, c)) {
      pred = predStruct::addPred(ci, i, cj, j, c);
      bool add = true;
      if ((pred != md->getBDDMgr()->bddOne()) &&
	  (pred != md->getBDDMgr()->bddZero()) &&
	  (phi != NULL)) {
        set<int> support = getSupportSet(mgr, *phi);
        if (support.find(pred.NodeReadIndex()) == support.end()) {
          add = false;
        }
      }
      if (add) {
        cons *= (!(p1bdd * !p2bdd) + pred);
#ifdef __SYM_PRINTCONS_FULL__
        cSetFg(BLUE);
        cout << p1->toString() << " * " << p2->toStringInverted() << " --> "
             <<predStruct::toString(pred) << endl;
        cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
        cSetFg(BLUE);
        cout << p1->var << " * !" << p2->var << " --> "
             << pred.NodeReadIndex() << endl;
        cSetAttr(NONE);
#endif
      }
    }
    if (create || predStruct::predExists(cj, j, ci, i, -c)) {
      pred = predStruct::addPred(cj, j, ci, i, -c);
      bool add = true;
      if ((pred != md->getBDDMgr()->bddOne()) &&
	  (pred != md->getBDDMgr()->bddZero()) &&
	  (phi != NULL)) {
        set<int> support = getSupportSet(mgr, *phi);
        if (support.find(pred.NodeReadIndex()) == support.end()) {
          add = false;
        }
      }
      if (add) {
        cons *= (!(p1bdd * !p2bdd) + !pred);
#ifdef __SYM_PRINTCONS_FULL__
        cSetFg(BLUE);
        cout << p1->toString() << " * " << p2->toStringInverted() << " --> "
             << predStruct::toStringInverted(pred) << endl;
        cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
        cSetFg(BLUE);
        cout << p1->var << " * !" << p2->var << " --> !" 
             << pred.NodeReadIndex() << endl;
        cSetAttr(NONE);
#endif
      }
    }
  }
  else if (p1inverted && p2inverted) {
    if (create || predStruct::predExists(cj, j, ci, i, -c)) {
      pred = predStruct::addPred(cj, j, ci, i, -c);
      bool add = true;
      if ((pred != md->getBDDMgr()->bddOne()) &&
	  (pred != md->getBDDMgr()->bddZero()) &&
	  (phi != NULL)) {
        set<int> support = getSupportSet(mgr, *phi);
        if (support.find(pred.NodeReadIndex()) == support.end()) {
          add = false;
        }
      }
      if (add) {
        cons *= (!(!p1bdd * !p2bdd) + !pred);
#ifdef __SYM_PRINTCONS_FULL__
        cSetFg(BLUE);
        cout << p1->toStringInverted() << " * "
             << p2->toStringInverted() << " --> " 
             << predStruct::toStringInverted(pred) << endl;
        cSetAttr(NONE);
#endif
#ifdef __SYM_PRINTCONS_NUMS__
        cSetFg(BLUE);
        cout << "!" << p1->var << " * !" << p2->var << " --> !"
             << pred.NodeReadIndex() << endl;        
        cSetAttr(NONE);
#endif
      }
    }
  }
  return cons;
}

string predStruct::toString() {
  ostringstream out;
  if (md != NULL) {
    if (ci != 1) out << ci;
    out << md->getVarName(i) << " >= ";
    if (cj != 1) out << cj;
    out << md->getVarName(j);
    out << " + " << c;
  }
  else {
    if (ci != 1) out << ci;
    out << "c" << i << " >= ";
    if (cj != 1) out << cj;
    out << "c" << j;
    out << " + " << c;
  }
  return out.str();  
}

string predStruct::toStringInverted() {
  ostringstream out;
  if (md != NULL) {
    if (cj != 1) out << cj;
    out << md->getVarName(j) << " > ";
    if (ci != 1) out << ci;
    out << md->getVarName(i);
    out << " + " << -c;
  }
  else {
    if (cj != 1) out << cj;
    out << "c" << j << " > ";
    if (ci != 1) out << ci;
    out << "c" << i;
    out << " + " << -c;
  }
  return out.str();
}

string predStruct::toStringInverted2() {
  ostringstream out;
  if (md != NULL) {
    if (cj/fabs(cj) != 1) out << cj/fabs(cj);
    out << md->getVarName(j) << " > ";
    if (ci/fabs(cj) != 1) out << ci/fabs(cj);
    out << md->getVarName(i);
    out << " + " << -c/fabs(cj);
  }
  else {
    if (cj/fabs(cj) != 1) out << cj/fabs(cj);
    out << "c" << j << " > ";
    if (ci/fabs(cj) != 1) out << ci/fabs(cj);
    out << "c" << i;
    out << " + " << -c/fabs(cj);
  }
  return out.str();
}

string predStruct::toString(BDD var) {
  if (var == mgr->bddOne())
    return "true";
  else if (var == mgr->bddZero()) {
    return "false";
  }
  else if (getPred(var.NodeReadIndex()) != NULL) {
    return getPred(var.NodeReadIndex())->toString();
  }
  else {
    return "UNKNOWN VAR";
  }
}

string predStruct::toStringInverted(BDD var) {
  if (var == mgr->bddOne())
    return "false";
  else if (var == mgr->bddZero()) {
    return "true";
  }
  else if (getPred(var.NodeReadIndex()) != NULL) {
    return getPred(var.NodeReadIndex())->toStringInverted();
  }
  else {
    return "UNKNOWN VAR";
  }
}

string predStruct::toStringInverted2(BDD var) {
  if (var == mgr->bddOne())
    return "false";
  else if (var == mgr->bddZero()) {
    return "true";
  }
  else if (getPred(var.NodeReadIndex()) != NULL) {
    return getPred(var.NodeReadIndex())->toStringInverted2();
  }
  else {
    return "UNKNOWN VAR";
  }
}

bool predStruct::contains(int var) {
  return (i == var || j == var);
}

BDD predStruct::bdd() {
  return mgr->bddVar(var);
}

void predStruct::printConstant(double c) {
  cout << c;
}

void predStruct::validate(double ci, int i, double cj, int j, double c) {
  if (md == NULL) {
    cout << "FATAL ERROR: Attempting to add pred without first calling "
         << "predStruct::initialize()" << endl;
    cout << "ci: " << ci << " i: " << md->getVarName(i)
         << " cj: " << cj << " j: " << md->getVarName(j)
         << " c: " << c << endl;
    assert(false);
  }
  if (ci == INFIN || ci == -INFIN || cj == INFIN || cj == -INFIN ||
      c == INFIN || c == -INFIN) {
    cout << "FATAL ERROR: Attempting to create predicate with"
         << "  constants of INFIN or -INFIN.  Should be proper double"
         << "  representation for ininity" << endl; 
    cout << "ci: " << ci << " i: " << md->getVarName(i)
         << " cj: " << cj << " j: " << md->getVarName(j)
         << " c: " << c << endl;
    assert(false);
  }
  if (fabs(ci) < threshold || fabs(cj) < threshold) {
    cout << "FATAL ERROR: Attempting to create predicate"
         << " with zero in ci or cj position." << endl;
    cout << "ci: " << ci << " i: " << md->getVarName(i)
         << " cj: " << cj << " j: " << md->getVarName(j)
         << " c: " << c << endl;
    assert(false);
  }
  if (i == X0_I && fabs(ci-1.0) > threshold) {
    cout << "FATAL ERROR: i is x0 with a non-unit constant." << endl;
    cout << "ci: " << ci << " i: " << md->getVarName(i)
         << " cj: " << cj << " j: " << md->getVarName(j)
         << " c: " << c << endl;
    assert(false);
  }
  if (j == X0_I && fabs(cj-1.0) > threshold) {
    cout << "FATAL ERROR: j is x0 with a non-unit constant." << endl;
    cout << "ci: " << ci << " i: " << md->getVarName(i)
         << " cj: " << cj << " j: " << md->getVarName(j)
         << " c: " << c << endl;
    assert(false);
  }
}
bool predStruct::isTrue(double ci, int i, double cj, int j, double c) {
  // Test if the pred is not really just true
  if (i == j && i == X0_I && c <= 0.0) {
    return true;
  }
  else if (i == j && fabs(ci - cj) < threshold && c <= 0.0) {
    return true;
  }
  else if (c == -numeric_limits<double>::infinity()) {
    return true;
  }
  else {
    return false;
  }
}

  
bool predStruct::isFalse(double ci, int i, double cj, int j, double c) {
  // Test if the pred is not really just false.
  if (i == j && i == X0_I && c > 0.0) {
    return true;
  }
  else if (i == j && fabs(ci - cj) < threshold && c > 0.0) {
    return true;
  }
  else if (c == numeric_limits<double>::infinity()) {
    return true;
  }
  else {
    return false;
  }
}

void predStruct::canonicalize(double &ci, int &i, double &cj, int &j,
                               double &c) {
  
  // Manipulate the predicate to ensure a canonical form.

  // If relating the same variable (they cannot both be zero at this
  // point), put relation in terms of X0_I
  if (i == j) {
    if (ci-cj > 0.0) {      
      j = X0_I;
      ci = ci-cj;
      cj = 1;
    }
    else if (cj-ci > 0.0) {
      i = X0_I;
      cj = cj-ci;
      ci = 1;
    }
    else {
      cout << "FATAL ERROR: Unexpected predicate." << endl;
      assert(false);
    }
  }

  // if i is X0_I, then normalize the right hand size of the inequality.
  if (i == X0_I) {
    c = c/fabs(cj);
    if (cj < 0) {
      i = j;
      j = X0_I;
    }
    cj = 1.0;
  }
  // if j is X0_I, then normalize the right hand size of the inequality.
  else if (j == X0_I) {
    c = c/fabs(ci);
    if (ci < 0) {
      j = i;
      i = X0_I;
    }
    ci = 1.0;
  }
  // Don't want to divide by a negative number because that would
  // require flipping the direction of the inequality so make sure
  // that left side is not negative.
  else if (ci < 0 && cj < 0) {
    int tmpi = i;
    i = j;
    j = tmpi;
    double tmpci = ci;
    ci = -cj;
    cj = -tmpci;
    c = c / ci;
    cj = cj / ci;
    ci = 1.0;
  }
  else if (ci > 0 && cj > 0) {
    c = c /ci;
    cj = cj/ci;
    ci = 1.0;    
  }  
  else if (ci > 0 && cj < 0) {
    if (i < j) {
      c = c /ci;
      cj = cj/ci;
      ci = 1.0;
    }
    else if (i > j) {
      int tmpi = i;
      i = j;
      j = tmpi;
      double tmpci = ci;
      ci = cj;
      cj = tmpci;

      cj = cj/ci;
      c = c/fabs(ci);
      ci = 1.0;
    }
  }
  else if (ci < 0 && cj > 0) {
    if (i < j) {
      cj = cj/fabs(ci);
      c = c/fabs(ci);
      ci = -1.0;
    }
    else if (i > j) {
      int tmpi = i;
      i = j;
      j = tmpi;
      double tmpci = ci;
      ci = cj;
      cj = tmpci;

      cj = fabs(cj)/ci;
      c = c/ci;
      ci = -1.0;
    }
  }
}
                              


//////////////////////////////////////////////////////////////////////////////

variableRate::variableRate(int varIndex, int lrate, int urate) {
  this->varIndex = varIndex;
  this->lrate = lrate;
  this->urate = urate;
}

//////////////////////////////////////////////////////////////////////////////

lhpnModelData::lhpnModelData(Cudd* mgr, designADT design) : modelData(mgr) {
  this->invariantBuilt = false;
  this->allDisStatesBuilt = false;
  this->initialStateBuilt = false;
  predStruct::initialize(this);
  this->design = design;

  // Create indices for some real variables.
  X0_I = addClockVar("x0");
  N_DLT_I = addClockVar("ndlt");
  N_EPS_I = addClockVar("neps");
  
  // Assign real variable index to special clocks for timers & 
  // create rate boolean variables corresponding to those clocks.
  for (int t = 1; t < design->nevents; t++) {
    // If transition is dropped do not create a clock variable for it.
    if (design->events[t]->dropped) {
      continue;
    }
    addClockVar(clockString(t));
    BDD newvar = mgr->bddVar();
    allModelVars[clockActiveString(t)] = newvar.NodeReadIndex();
    clockActiveDisVariables.push_back(newvar);
#ifdef __SYM_PRINT_VARS__
    cSetFg(CYAN);
    cout << "Clock Active Boolean Variable: " << clockActiveString(t)
         << "\t Index: " << newvar.NodeReadIndex()
         << "\t(Transition " << t << ")" << endl;
    cSetAttr(NONE);
#endif
  }

  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->events[p]->type & CONT) {
      assert (design->events[p]->type & VAR);
      // Continuous variables are marked as dropped even though we really need
      // them so we do not ignore dropped continuous variables.
      // Assign a real variable index to the real variables for use in
      // inequality construction.
      addClockVar(design->events[p]->event);
    }
    // Create a BDD variable for each discrete place.
    else {
      // If place/variable is dropped, do not create a clock variable for it.
      if (design->events[p]->dropped) {
	cout << design->events[p]->event << " is dropped" << endl;
	continue;
      }
      BDD newvar = mgr->bddVar();
      allModelVars[design->events[p]->event] = newvar.NodeReadIndex();
      placeDisVariables.push_back(newvar);

      // TODO:  It may be necessary for these to be in the allRealVar
      // vector also so that getVarName returns something sensible.
#ifdef __SYM_PRINT_VARS__
      cSetFg(CYAN);
      cout << "Discrete Place: " << design->events[p]->event
           << "\tIndex: " << newvar.NodeReadIndex()
           << "\t(Event " << p << ")" << endl;
      cSetAttr(NONE);
#endif 
    }
  }
  // Create a BDD variable for each Boolean variable found in signals.
  for (int s = 0; s < design->nsignals; s++) {
    if (strchr(design->signals[s]->name, '>') || 
        strchr(design->signals[s]->name, '<')) {
      // This signal is an inequality predicate - no BDD to create.
      continue;
    }
    BDD newvar = mgr->bddVar();
    allModelVars[design->signals[s]->name] = newvar.NodeReadIndex();
    booleanDisVariables.push_back(newvar);

#ifdef __SYM_PRINT_VARS__
    cSetFg(CYAN);
    cout << "Boolean Variable: " << design->signals[s]->name
         << "\t Index: " << newvar.NodeReadIndex()
         << "\t(Signal " << s << ")" << endl;
    cSetAttr(NONE);
#endif
  }
  // Create BDD variables for each rate assignment.
  // First check the initial states for possible rates.
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->events[p]->type & CONT) {
      string ratevar = rateToString(design->events[p]->event,
                                    design->events[p]->linitrate,
                                    design->events[p]->uinitrate);
      if (allModelVars.find(ratevar) == allModelVars.end()) {
        BDD newvar = mgr->bddVar();
        allModelVars[ratevar] = newvar.NodeReadIndex();
        rateDisVariables.push_back(newvar);
#ifdef __SYM_PRINT_VARS__
        cSetFg(CYAN);
        cout << "Boolean Rate Variable: " << ratevar
             << "\t Index: " << newvar.NodeReadIndex()
             << "\t(Initial rate on " << design->events[p]->event << ")" 
	     << endl;
        cSetAttr(NONE);
#endif  
      }
    }
  }
  // Now check the inequalities on each transition.
  for (int t = 1; t < design->nevents; t++) {
    assert (!(design->events[t]->type & CONT));
    ineqADT i = design->events[t]->inequalities;
    while (i != NULL) {
      if (i->type == 6 &&
          allModelVars.find(rateToString(i)) == allModelVars.end()) {
        BDD newvar = mgr->bddVar();
        allModelVars[rateToString(i)] = newvar.NodeReadIndex();
        rateDisVariables.push_back(newvar);
#ifdef __SYM_PRINT_VARS__
        cSetFg(CYAN);
        cout << "Boolean Rate Variable: " << rateToString(i)
             << "\t Index: " << newvar.NodeReadIndex()
             << "\t(Rate assignment on " << design->events[t]->event << ")" << endl;
        cSetAttr(NONE);
#endif  
      }
      i = i->next;
    }
  }
  // Construct the rate BDDs
  // DEBUG: at the line below this:
  // These are necessary for old time elapse calculation.
  this->rphidlt = rateBDD(N_DLT_I);
  this->rphieps = rateBDD(N_EPS_I);
  // Construct the discrete state stuff
  this->allDisStates = findDisStates();
  this->eachDisState = disStateVector();
  // Construct the rate set
  this->eachRateBDD = rateBDDVector();
  this->eachRateProd = rateSOPVector(this->eachRateBDD);
}


string lhpnModelData::rateToString(string varname, int lrate, int urate) {
  ostringstream oss;
  oss << varname << "'[" << lrate << "," << urate << "]";
  ratesForRealVar[varname].insert(oss.str());
  return oss.str();
}

// Given an inequality representing a rate, constructs a string which
// refers to the Boolean variable enabling that rate.
string lhpnModelData::rateToString(ineqADT ineq) {
  assert(ineq->type == 6);
  return rateToString(design->events[ineq->place]->event,
                      ineq->constant, ineq->uconstant);
}

variableRate* lhpnModelData::stringToRate(string rateString) {
  string::size_type pos;
  if (rateString.substr(0,3) == "ca_") {
    //cout << rateString.substr(3, rateString.length()-3) << endl;
    int varIndex = getVarIndex("#"+rateString.substr(3, rateString.length()-3));
    return new variableRate(varIndex, 1, 1);
  }
  else if ((pos = rateString.find("\'")) != string::npos) {
    string::size_type lb = rateString.find("[");
    string::size_type cm = rateString.find(",");
    string::size_type rb = rateString.find("]");
    //cout << rateString.substr(0, pos) << endl;
    //cout << rateString.substr(lb+1, cm-lb-1) << endl;
    //cout << rateString.substr(cm+1, rb-cm-1) << endl;
    int varIndex = getVarIndex(rateString.substr(0, pos));
    int lrate = atoi(rateString.substr(lb+1, cm-lb-1).c_str());
    int urate = atoi(rateString.substr(cm+1, rb-cm-1).c_str());
    return new variableRate(varIndex, lrate, urate);
  }
  else {
    cout << "Expecting a rate string in stringToRate: " << rateString << endl;
    assert(false);
  }
}
  
// Given an index for a transition, 
// constructs a string representing the clock on that transition.
string lhpnModelData::clockString(int tindex) {
  assert (!(design->events[tindex]->type & CONT));
  ostringstream cname;
  cname << "#" << design->events[tindex]->event;
  return cname.str();
}


string lhpnModelData::clockActiveString(int tindex) {
  assert (!(design->events[tindex]->type & CONT));
  ostringstream oss;
  oss << "ca_" << design->events[tindex]->event;
  return oss.str();
}

// given a particular ineqADT, a new BDD corresponding to that
// inequality is returned.
BDD lhpnModelData::ineqADTToBDD(ineqADT ineq) {
  assert (ineq != NULL);
  BDD ineqbdd = mgr->bddOne();
  int place = getVarIndex(design->events[ineq->place]->event);
  int constant = ineq->constant;
  // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
  // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS*
  if (ineq->type == 0) {
    ineqbdd *= !predStruct::addPred(X0_I, place, -constant);
  }
  else if (ineq->type == 1) {
    ineqbdd *= predStruct::addPred(place, X0_I, constant);
  }
  else if (ineq->type == 2) {
    ineqbdd *= !predStruct::addPred(place, X0_I, constant);
  }
  else if (ineq->type == 3) {
    ineqbdd *= predStruct::addPred(X0_I, place, -constant);
  }
  else if (ineq->type == 4) {
    ineqbdd *= predStruct::addPred(place, X0_I, constant);
    ineqbdd *= predStruct::addPred(X0_I, place, -constant);
  }
  return ineqbdd;
}

BDD lhpnModelData::ineqADTToBDDwithRates(ineqADT ineq) {
  assert (ineq != NULL);
  BDD ineqbdd = mgr->bddOne();
  int place = getVarIndex(design->events[ineq->place]->event);
  string varname = design->events[ineq->place]->event;
  int constant = ineq->constant;
  // Find rates associated with the real variable in the inequality.
  BDD incr = mgr->bddZero();
  BDD decr = mgr->bddZero();
  set<string> rates = ratesForRealVar[varname];
  for (set<string>::iterator i = rates.begin(); i!= rates.end(); i++) {
    variableRate* vr = stringToRate(*i);
    BDD boolratevar = getModelVarBDD(*i);
    if (vr->varIndex == place) {
      if (vr->lrate >= 0 && vr->urate >= 0) {
	incr += (boolratevar);
      }
      else if (vr->lrate < 0 && vr->urate < 0) {
	decr += (boolratevar);
      }
      else {
	incr += (boolratevar);
	decr += (boolratevar);
      }
    }
  }
  // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
  // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS*
  if (ineq->type == 1) {
    // x >= c gets converted to ((x > c) | (x=c & incr(x)))
    ineqbdd *= predStruct::addPred(place, X0_I, constant);
    ineqbdd *= predStruct::addPred(X0_I, place, -constant);
    ineqbdd *= incr;
    ineqbdd += !predStruct::addPred(X0_I, place, -constant);
  }
  else if (ineq->type == 3) {
    // x <= c gets converted to ((x < c) | (x=c & decr(x)))
    ineqbdd *= predStruct::addPred(place, X0_I, constant);
    ineqbdd *= predStruct::addPred(X0_I, place, -constant);
    ineqbdd *= decr;
    ineqbdd += !predStruct::addPred(place, X0_I, constant);
  }
  else {
    assert("Don't know how to handle this type of inequality.");
  }
  //  cout << "ineqADTToBDDwithRates for type " << ineq->type 
  //     << ", " << getVarName(place) << ", " << constant << ": " << endl;
  //printBDDState(ineqbdd);
  return ineqbdd;
}

BDD lhpnModelData::ineqADTToBDDwithRatesforSGC(ineqADT ineq, bool invert) {
  assert (ineq != NULL);
  BDD ineqbdd = mgr->bddOne();
  int place = getVarIndex(design->events[ineq->place]->event);
  string varname = design->events[ineq->place]->event;
  int constant = ineq->constant;
  // Find rates associated with the real variable in the inequality.
  BDD incr = mgr->bddZero();
  BDD decr = mgr->bddZero();
  set<string> rates = ratesForRealVar[varname];
  for (set<string>::iterator i = rates.begin(); i!= rates.end(); i++) {
    variableRate* vr = stringToRate(*i);
    BDD boolratevar = getModelVarBDD(*i);
    if (vr->varIndex == place) {
      if (vr->lrate > 0 && vr->urate > 0) {
	incr += (boolratevar);
      }
      else if (vr->lrate < 0 && vr->urate < 0) {
	decr += (boolratevar);
      }
      /*
      else {
	incr += (boolratevar);
	decr += (boolratevar);
	}*/
    }
  }
  // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
  // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS*
  ineqbdd *= predStruct::addPred(place, X0_I, constant);
  ineqbdd *= predStruct::addPred(X0_I, place, -constant);
  if ((ineq->type == 1 && !invert) ||
      (ineq->type == 3 && invert)) {
    // x >= c gets converted to x=c & incr(x)
    ineqbdd *= incr;
  }
  else if ((ineq->type == 3 && !invert) ||
	   (ineq->type == 1 && invert)) {
    // x <= c gets converted to x=c & decr(x)
    ineqbdd *= decr;
  }
  else {
    assert("Don't know how to handle this type of inequality.");
  }
  //  cout << "ineqADTToBDDwithRates for type " << ineq->type 
  //     << ", " << getVarName(place) << ", " << constant << ": " << endl;
  //printBDDState(ineqbdd);
  return ineqbdd;
}

// Given a linked list of inequalities over continuous places
// or clocks, constructs a BDD of those inequalities.
BDD lhpnModelData::SOPtoBDD(level_exp exp, ineqADT ineqs, bool useRates) {
  BDD result = mgr->bddZero();
  while (exp != NULL) {
    BDD resultpart = mgr->bddOne();
    for (int s = 0; s < design->nsignals; s++) {
      BDD b;
      if (exp->product[s] == 'X') continue;
      assert (exp->product[s] == '0' || exp->product[s] == '1');
      if (strchr(design->signals[s]->name, '>') || 
          strchr(design->signals[s]->name, '<')) {
        //cout << "SOPtoBDD: " << design->signals[s]->name << endl;
        ineqADT ineqit = ineqs;
        while (ineqit != NULL) {
          if (ineqit->signal == s) {
            int otype = ineqit->type;
            // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
            // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS*
            assert (ineqit->type == 0 || ineqit->type == 1 ||
                    ineqit->type == 2 || ineqit->type == 3);
            // Do not force everything to contain an =.
            //if (exp->product[s] == '0') {
            //  if (ineqit->type == 0) ineqit->type = 3;
            //  else if (ineqit->type == 1) ineqit->type = 2;
            //  else if (ineqit->type == 2) ineqit->type = 1;
            //  else if (ineqit->type == 3) ineqit->type = 0;
            //}
            // Force everthing to contain an =.
            if (exp->product[s] == '0') {
              if (ineqit->type == 0) ineqit->type = 3;
              else if (ineqit->type == 1) ineqit->type = 3;
              else if (ineqit->type == 2) ineqit->type = 1;
              else if (ineqit->type == 3) ineqit->type = 1;
            }
            else if (exp->product[s] == '1') {
              if (ineqit->type == 0) ineqit->type = 1;
              else if (ineqit->type == 1) ineqit->type = 1;
              else if (ineqit->type == 2) ineqit->type = 3;
              else if (ineqit->type == 3) ineqit->type = 3;
            }
	    if (useRates) {
	      b = ineqADTToBDDwithRates(ineqit);
	    }
	    else {
	      b = ineqADTToBDD(ineqit);
	    }
	    ineqit->type = otype;
	    break;
          }
          ineqit = ineqit->next;
        }
        resultpart *= b;
      }
      else {
        b = getModelVarBDD(design->signals[s]->name);
        if (exp->product[s] == '0') {
          resultpart *= !b;
        }
        else if (exp->product[s] == '1') {
          resultpart *= b;
        }
      }
    }
    result += resultpart;
    exp = exp->next;
  }
  return result;
}

BDD lhpnModelData::ProdToBDD(char* product, ineqADT ineqs, bool invert, 
			     int skipplace) {
  BDD resultpart = mgr->bddOne();
  for (int s = 0; s < design->nsignals; s++) {
    if (product[s] == 'X') continue;
    assert (product[s] == '0' || product[s] == '1');
    if (strchr(design->signals[s]->name, '>') || 
	strchr(design->signals[s]->name, '<')) {
      ineqADT ineqit = ineqs;
      while (ineqit != NULL) {
	if (ineqit->signal == s && 
	    skipplace != getVarIndex(design->events[ineqit->place]->event)) {
	  int otype = ineqit->type;
	  // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
	  // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS*
	  assert (ineqit->type == 0 || ineqit->type == 1 ||
		  ineqit->type == 2 || ineqit->type == 3);
	  // Force everthing to contain an =.
	  if (ineqit->type == 0 || ineqit->type == 2) {
	    cout << "Warning: Inequality does not contain an = "
		 << "sign for variable "
		 << design->events[ineqit->place] << endl;
	  }
	  if ((product[s] == '0' && !invert) ||
	      (product[s] == '1' && invert)) {
	    if (ineqit->type == 0) ineqit->type = 3;
	    else if (ineqit->type == 1) ineqit->type = 3;
	    else if (ineqit->type == 2) ineqit->type = 1;
	    else if (ineqit->type == 3) ineqit->type = 1;
	  }
	  else if ((product[s] == '1' && !invert) ||
		   (product[s] == '0' && invert)) {
	    if (ineqit->type == 0) ineqit->type = 1;
	    else if (ineqit->type == 1) ineqit->type = 1;
	    else if (ineqit->type == 2) ineqit->type = 3;
	    else if (ineqit->type == 3) ineqit->type = 3;
	  }
	  resultpart *= ineqADTToBDD(ineqit);
	  ineqit->type = otype;
	  break;
	}
	ineqit = ineqit->next;
      }
    }
    else {
      BDD b = getModelVarBDD(design->signals[s]->name);
      if (product[s] == '0') {
	resultpart *= !b;
      }
      else if (product[s] == '1') {
	resultpart *= b;
      }
    }
  }
  return resultpart;
}

// Given a linked list of inequalities over continuous places
// or clocks, constructs a BDD of those inequalities.
// For use in the secondary guarded command construction
BDD lhpnModelData::SOPtoBDDforSGC(level_exp exp, ineqADT ineqs, bool invert) {
  BDD result = mgr->bddZero();
  while (exp != NULL) {
    bool didone = false;
    BDD resultpart = mgr->bddOne();
    for (int s = 0; s < design->nsignals; s++) {
      if (exp->product[s] == 'X') continue;
      assert(exp->product[s] == '0' || exp->product[s] == '1');
      if (strchr(design->signals[s]->name, '>') ||
	  strchr(design->signals[s]->name, '<')) {
	didone = true;
	ineqADT ineqit = ineqs;
	int skipplace=-1;
	while (ineqit != NULL) {
	  if (ineqit->signal == s) {
            int otype = ineqit->type;
	    assert (ineqit->type == 0 || ineqit->type == 1 ||
                    ineqit->type == 2 || ineqit->type == 3);
            if (ineqit->type == 0 || ineqit->type == 2) {
              cout << "Warning: Inequality does not contain an = "
                   << "sign for variable "
                   << design->events[ineqit->place] << endl;
            }
            if (exp->product[s] == '0') {
              if (ineqit->type == 0) ineqit->type = 1;
              else if (ineqit->type == 1) ineqit->type = 1;
              else if (ineqit->type == 2) ineqit->type = 3;
              else if (ineqit->type == 3) ineqit->type = 3;
            }
            else if (exp->product[s] == '1') {
              if (ineqit->type == 0) ineqit->type = 3;
              else if (ineqit->type == 1) ineqit->type = 3;
              else if (ineqit->type == 2) ineqit->type = 1;
              else if (ineqit->type == 3) ineqit->type = 1;
            }	    
	    resultpart = ineqADTToBDDwithRatesforSGC(ineqit, invert);
	    skipplace = getVarIndex(design->events[ineqit->place]->event);
	    ineqit->type = otype;
	    break;
	  }
	  ineqit = ineqit->next;
	}
	assert(skipplace != -1);
	resultpart *= ProdToBDD(exp->product, ineqs, false, skipplace);
	result += resultpart;
      }
    }
    if (!didone) {
      resultpart = ProdToBDD(exp->product, ineqs, false,-1);
    }
    result += resultpart;
    exp = exp->next;
  }
  return result;
}

// Given a linked list of inequalities over continuous places
// or clocks, constructs a pseudo inverse BDD of those inequalities.
BDD lhpnModelData::SOPtoPIBDD(level_exp exp, ineqADT ineqs, bool useRates) {
  BDD result = mgr->bddOne();
  while (exp != NULL) {
    BDD resultpart = mgr->bddZero();
    for (int s = 0; s < design->nsignals; s++) {
      BDD b;
      if (exp->product[s] == 'X') continue;
      assert (exp->product[s] == '0' || exp->product[s] == '1');
      if (strchr(design->signals[s]->name, '>') || 
          strchr(design->signals[s]->name, '<')) {
        //cout << "SOPtoBDD: " << design->signals[s]->name << endl;
        ineqADT ineqit = ineqs;
        while (ineqit != NULL) {
          if (ineqit->signal == s) {
            int otype = ineqit->type;
            // 0 =  ">", 1 = ">=", 2 = "<", 3 = "<=", 4 = "="
            // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"  *AMS*
            assert (ineqit->type == 0 || ineqit->type == 1 ||
                    ineqit->type == 2 || ineqit->type == 3);
            if (ineqit->type == 0 || ineqit->type == 2) {
              cout << "Warning: Inequality does not contain an = "
                   << "sign for variable "
                   << design->events[ineqit->place] << endl;
            }
            if (exp->product[s] == '0') {
              if (ineqit->type == 0) ineqit->type = 1;
              else if (ineqit->type == 1) ineqit->type = 1;
              else if (ineqit->type == 2) ineqit->type = 3;
              else if (ineqit->type == 3) ineqit->type = 3;
            }
            else if (exp->product[s] == '1') {
              if (ineqit->type == 0) ineqit->type = 3;
              else if (ineqit->type == 1) ineqit->type = 3;
              else if (ineqit->type == 2) ineqit->type = 1;
              else if (ineqit->type == 3) ineqit->type = 1;
            }
	    if (useRates) {
	      b = ineqADTToBDDwithRates(ineqit);
	    }
	    else {
	      b = ineqADTToBDD(ineqit);
	    }
            ineqit->type = otype;
            break;
          }
          ineqit = ineqit->next;
        }
        resultpart += b;
      }
      else {
        b = getModelVarBDD(design->signals[s]->name);
        if (exp->product[s] == '0') {
          resultpart += b;
        }
        else if (exp->product[s] == '1') {
          resultpart += !b;
        }
      }
    }
    result *= resultpart;
    exp = exp->next;
  }
  return result;
}

BDD lhpnModelData::initialState() {
  if (this->initialStateBuilt) {
    return this->myInitialState;
  }
  BDD result = mgr->bddOne();
/*   // initialize active clocks to zero. */
/*   for (int t = 1; t < design->nevents; t++) { */
/*     bool a//but in order for the clocks to be active, the En(t) must */
/*           //be true. */
/*     for (int p = design->nevets; p < design->nevents+design->nplaces; p++) { */
/*     } */
/*   } */
/*        int cindex = getVarIndex(clockString(t));*/
  
  for (int p = design->nevents; p < design->nevents + design->nplaces; p++) {
    // discrete places
    if (!(design->events[p]->type & CONT) && !design->events[p]->dropped) {
      bool marked = false;
      for (int t = 1; t < design->nevents; t++) {
        if (design->rules[p][t]->ruletype != NORULE && 
            design->rules[p][t]->epsilon == 1) {
          marked = true;
          break;
        }
      }
      if (marked) {
        result *= getModelVarBDD(design->events[p]->event);
      }
      else {
        result *= !getModelVarBDD(design->events[p]->event);
      }
    }
    // Initial values and rates for continuous places/variables
    if (design->events[p]->type & CONT) {
      int vindex = getVarIndex(design->events[p]->event);
      int l = design->events[p]->lower;
      int u = design->events[p]->upper;
      ostringstream ratename;
      ratename << design->events[p]->event << "'["
               << design->events[p]->linitrate << ","
               << design->events[p]->uinitrate << "]";
      result *= predStruct::addPred(vindex, X0_I, l);
      result *= predStruct::addPred(X0_I, vindex, -1*u);
      result *= getModelVarBDD(ratename.str());
      
    }
  }
  //cout << design->startstate << endl;
  // Boolean Signals
  for (int s = 0; s < design->nsignals; s++) {
    if (strchr(design->signals[s]->name, '>') || 
	strchr(design->signals[s]->name, '<') || 
	strchr(design->signals[s]->name, '=')) continue;
    if (design->startstate[s] == '1') {
      result *= getModelVarBDD(design->signals[s]->name);
    }
    else if (design->startstate[s] == '0') {
      result *= !getModelVarBDD(design->signals[s]->name);
    }
  }
  // Add on active clocks and assign them to zero.
  for (int t = 1; t < design->nevents; t++) {
    if (design->events[t]->dropped) {
      continue;
    }
    eventADT event = design->events[t];
    BDD en = SOPtoBDD(event->SOP, event->inequalities, true);
    BDD preset = presetBDD(t);
    BDD ca = getModelVarBDD(clockActiveString(t));
    if (addTranConsNew(result * en * preset) != getBDDMgr()->bddZero()) {
      int cIndex = getVarIndex(clockString(t));
      result *= ca;
      result *= predStruct::addPred(cIndex, X0_I, 0);
      result *= predStruct::addPred(X0_I, cIndex, 0);
    }
    else {
      result *= !ca;
    }
  }
  this->initialStateBuilt = true;
  //cout << "Initial State Before simplifyRestrict: " << endl;
  //printBDDState(result);
  this->myInitialState = simplifyRestrict(result * buildInvariant());
#ifdef __SYM_INITSTATE__  
  cout << "Initial State: " << endl;
  printBDDState(this->myInitialState);
#endif
  return this->myInitialState;
}

BDD lhpnModelData::buildInvariant() {
  if (invariantBuilt) {
    return myInvariant;
  }
#ifdef __SYM_INVARIANT__
  cSetFg(BLUE);
  cSetAttr(UNDERLINE);
  cout << "Building Invariant" << endl;
  cSetAttr(NONE);
#endif 
  BDD invariant = findDisStates();
#ifdef __SYM_INVARIANT__
  cout << "Discrete States: " << endl;
  printBDDState(invariant);
#endif
  for (int t = 1; t < design->nevents; t++) {
    if (design->events[t]->dropped) {
      continue;
    }
    eventADT event = design->events[t];
    assert (!(design->events[t]->type & CONT));
    assert(event->SOP != NULL && event->SOP->product != NULL);
    int cindex = getVarIndex(clockString(t));
    // c on t <= upperbound
    BDD pred = predStruct::addPred(X0_I, cindex, -design->events[t]->upper);
    // c on t >= 0
    BDD lpred = predStruct::addPred(cindex, X0_I, 0);
    
    BDD en = SOPtoBDD(event->SOP, event->inequalities);
    // pseudo inverse of enabling.
    BDD pien = SOPtoPIBDD(event->SOP, event->inequalities); 
    BDD preset = presetBDD(t);
    BDD ca = getModelVarBDD(clockActiveString(t));
    // clockactive --> preset ^ en
    invariant *= (!ca + (preset * en));
    // clockactive --> (c >= 0 & c <= upperbound)
    invariant *= (!ca + (lpred * pred));
    // clock active ^ preset ^ en --> c <= upperbound    
    //invariant *= (!(ca * preset * en) + pred);
    // preset ^ en(t) ^ !clock active --> ~en(t) (~ = pseudo inverse)
    invariant *=(!(preset /* * en */ * !ca) + pien);
    // preset ^ ~en(t) ^ clock active --> en(t)  (~ = pseudo inverse)
    invariant *= (!(preset /* * pien */ * ca) + en);
  }
#ifdef __SYM_INVARIANT__
  cSetFg(BLUE);
  printBDDState(invariant);
  cSetAttr(NONE);
#endif
  myInvariant = invariant;
  invariantBuilt = true;
  return invariant;
}

// Fill up the assignment portion of trans using the given set of inequalities
// and the index of the transition (tindex) into  design->rules
// event is the event to transition to generate the transition relation for
// tindex is the index of that event in design->rules
void lhpnModelData::buildAssignments(ineqADT ineqs, int tindex,
                                     transStruct* trans) {
  // Assignments to update the marking.
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    // Preset
    if (design->rules[p][tindex]->ruletype != NORULE &&
        design->rules[tindex][p]->ruletype == NORULE) {
      assert(!(design->events[p]->type & CONT));
      BDD bassign = !getModelVarBDD(design->events[p]->event);
      trans->boolAssign.push_back(bassign);
    }
    // Postset
    if (design->rules[tindex][p]->ruletype != NORULE &&
        design->rules[p][tindex]->ruletype == NORULE) {
      assert(!(design->events[p]->type & CONT));
      BDD bassign = getModelVarBDD(design->events[p]->event);
      trans->boolAssign.push_back(bassign);
    }
  }

  // Assignments to rate boolean, real variables, and regular booleans.
  for (ineqADT ineq=ineqs; ineq; ineq=ineq->next) {
    //cout << ratesForRealVar[design->events[ineq->place]->event] << endl;
    //int place = getVarIndex(design->events[ineq->place]->event);
    // 5 = ":=", 6 = "'dot:=", 7 = ":=T/F"
    if (ineq->type == 5) {
      int index = getVarIndex(design->events[ineq->place]->event);
      trans->clockAssign.insert(index);
      trans->lowerClock[index] = ineq->constant;
      trans->upperClock[index] = ineq->uconstant;
    }
    else if (ineq->type == 6) {
      string newrate = rateToString(ineq);
      trans->boolAssign.push_back(getModelVarBDD(newrate));
      //  Need to assign other bools affecting this rate to false?
      set<string> rates = ratesForRealVar[design->events[ineq->place]->event];
      for (set<string>::iterator i = rates.begin(); i != rates.end(); i++) {
        if (*i != newrate) {
          trans->boolAssign.push_back(!getModelVarBDD(*i));
        }
      }
    }
    else if (ineq->type == 7) {
      if (ineq->constant == TRUE) {
        BDD bassign = getModelVarBDD(design->signals[ineq->place]->name);
        trans->boolAssign.push_back(bassign);
      }
      else if (ineq->constant == FALSE) {
        BDD bassign = !getModelVarBDD(design->signals[ineq->place]->name);
        trans->boolAssign.push_back(bassign);
      }
    }
  }
}

BDD lhpnModelData::presetBDD(int tindex) {
  BDD preset= mgr->bddOne();
  for (int p = design->nevents; p < design->nevents + design->nplaces; p++) {
    if (design->rules[p][tindex]->ruletype != NORULE) {
      assert(!(design->events[p]->type & CONT));
     preset *= getModelVarBDD(design->events[p]->event);
    }
  }
  return preset;
}

// Returns the postset of the transition with places that are also in the 
// preset of the transition removed.  The variables in the postset are
// inverted also so the resulting BDD can be used to ensure that the places
// in the postset are not marked.
BDD lhpnModelData::postsetBDD(int tindex) {
  BDD postset = mgr->bddOne();
  for (int p = design->nevents; p < design->nevents + design->nplaces; p++) {
    if (design->rules[tindex][p]->ruletype != NORULE && 
	design->rules[p][tindex]->ruletype == NORULE) {
      assert(!(design->events[p]->type & CONT));
     postset *= !getModelVarBDD(design->events[p]->event);
    }
  }
  return postset;
}

void lhpnModelData::printTransRel(transStruct* trans) {
  cout << "\tGuard: " << BDDToString(trans->guard) << endl;
  cout << "\tBool Assignments: ";
  for (unsigned int j = 0; j < trans->boolAssign.size(); j++) {
    cout << BDDToString(trans->boolAssign[j]) << ", ";
  }
  cout << endl;
  cout << "\tReal Assignments: ";
  for (set<int>::iterator j = trans->clockAssign.begin();
       j != trans->clockAssign.end(); j++) {
    cout << getVarName(*j) << ":=[" << trans->lowerClock[*j] 
         << "," << trans->upperClock[*j] << "], ";
  }
  cout << endl;
}


transvector lhpnModelData::buildTransRels() {
#ifdef __SYM_TRANSRELS__
  cSetFg(MAGENTA);
  cSetAttr(UNDERLINE);
  cout << "Building Transition Relations" << endl;
  cSetAttr(NONE);
#endif 
  transvector transRels;
  for (int t = 1; t < design->nevents; t++) {
    if (design->events[t]->dropped) {
      continue;
    }
    eventADT event = design->events[t];
    int cIndex = getVarIndex(clockString(t));
    assert (!(event->type & CONT));
    assert(event->SOP != NULL && event->SOP->product != NULL);

    // Stuff used when constructing the guards
    BDD en = SOPtoBDD(event->SOP, event->inequalities,false);
    BDD pien = SOPtoPIBDD(event->SOP, event->inequalities,false);
    BDD ensgc = SOPtoBDDforSGC(event->SOP, event->inequalities, true);
    BDD piensgc = SOPtoBDDforSGC(event->SOP, event->inequalities, false);
    BDD preset = presetBDD(t);
    BDD postset = postsetBDD(t);
    BDD ca = getModelVarBDD(clockActiveString(t));
    BDD lbnd = predStruct::addPred(cIndex, X0_I, event->lower);
    //BDD ubund = predStruct::addPred(X0_I, cIndex, -event->upper);

    transStruct* trans = new transStruct();
    trans->type = PRIMARY;
    transStruct* transenable = new transStruct();
    transenable->type = SECONDARY;
    transStruct* transdisable = new transStruct();
    transdisable->type = SECONDARY;
    
    transStruct* transenable2 = new transStruct();
    transenable2->type = SECONDARY2;
    transStruct* transdisable2 = new transStruct();
    transdisable2->type = SECONDARY2;

    // Construct the guards
    trans->guard = preset * postset * en * ca * lbnd;
    transenable->guard = preset * en * !ca;
    transdisable->guard = (!preset + pien) * ca;
    
    transenable2->guard = preset * ensgc * !ca;
    transdisable2->guard = (!preset + piensgc) * ca;

    // Construct the assignments
    // buildassignments constructs for marking update, the var_assign,
    // and the rate_assign.
    buildAssignments(event->inequalities, t, trans);
    trans->clockAssign.insert(cIndex);
    trans->boolAssign.push_back(!ca);
    trans->lowerClock[cIndex] = -numeric_limits<double>::infinity();
    trans->upperClock[cIndex] = numeric_limits<double>::infinity();
    //trans->boolAssign.push_back(!ca);

    transenable->boolAssign.push_back(ca);
    transenable->clockAssign.insert(cIndex);
    transenable->lowerClock[cIndex] = 0;
    transenable->upperClock[cIndex] = 0;

    transdisable->boolAssign.push_back(!ca);
    transdisable->clockAssign.insert(cIndex);
    transdisable->lowerClock[cIndex] = -numeric_limits<double>::infinity();
    transdisable->upperClock[cIndex] = numeric_limits<double>::infinity();

    transenable2->boolAssign.push_back(ca);
    transenable2->clockAssign.insert(cIndex);
    transenable2->lowerClock[cIndex] = 0;
    transenable2->upperClock[cIndex] = 0;

    transdisable2->boolAssign.push_back(!ca);
    transdisable2->clockAssign.insert(cIndex);
    transdisable2->lowerClock[cIndex] = -numeric_limits<double>::infinity();
    transdisable2->upperClock[cIndex] = numeric_limits<double>::infinity();
    
    transRels.push_back(transenable2);
    transRels.push_back(transdisable2);

    transRels.push_back(transenable);
    transRels.push_back(transdisable);
    transRels.push_back(trans);
#ifdef __SYM_TRANSRELS__
    cSetFg(MAGENTA);
    cout << "Primary transition for " << event->event << endl;
    printTransRel(trans);
    cout << "Secondary enabling transition for " << event->event << endl;
    printTransRel(transenable);
    cout << "Secondary disabling transition for " << event->event << endl;
    printTransRel(transdisable);
    cSetAttr(NONE);
#endif
  }
  transRels = mergePrimarySecondary(transRels);
#ifdef __SYM_TRANSRELS__
  cout << "Transition relations after performing merge:" << endl;
  for (transvector::iterator i = transRels.begin();
       i != transRels.end(); i++) {
    printTransRel(*i);
    cout << endl;
  }
#endif
  return transRels;
}

transvector lhpnModelData::mergePrimarySecondary(transvector transRels) {
#ifdef  __SYM_TRANSRELSMERGE__  
  cout << "Merging primary and secondary transitions" << endl;
#endif
  // List is used for the primary transitions so that iterators remain
  // valid after insertions.
  list<transStruct*> primary;
  transvector secondary;
  transvector secondary2;

  BDD invariant = buildInvariant();
  for (transvector::iterator i = transRels.begin();
       i != transRels.end(); i++) {
    if ((*i)->type == PRIMARY) {
      primary.push_back(*i);
    }
    else if ((*i)->type == SECONDARY) {
      secondary.push_back(*i);
    }
    else {
      secondary2.push_back(*i);
    }
  }
  for (list<transStruct*>::iterator p = primary.begin();
       p != primary.end(); p++) {
    for (transvector::iterator s = secondary.begin();
         s != secondary.end(); s++) {
      // Determine if the primary and secondary assignments sets conflict.
      // If they do, continue on without doing anything.
      set<int> cint;
      set_intersection((*p)->clockAssign.begin(), (*p)->clockAssign.end(),
		       (*s)->clockAssign.begin(), (*s)->clockAssign.end(),
		       inserter(cint, cint.begin()));
      if (cint.begin() != cint.end()) {
/* 	cout << "WARNING: Clock assignment sets being merged contain conflicts:"; */
/* 	for (set<int>::iterator i = cint.begin(); i != cint.end(); i++) { */
/* 	  cout << "\t" << getVarName(*i) << endl; */
/* 	} */
/* 	cout << "Skipping this pairing." << endl; */
	continue;
      }

#ifdef  __SYM_TRANSRELSMERGE__
      cout << "Current primary:" << endl;
      printTransRel(*p);
      cout << "Current secondary:" << endl;
      printTransRel(*s);
#endif
      BDD guardpart = applyAssignments(this, (*s)->guard, *p);
      if (guardpart == (*s)->guard) {
#ifdef  __SYM_TRANSRELSMERGE__
        cout << "mps: 1" << endl;
#endif
        continue;
      }
      else if (guardpart == getBDDMgr()->bddZero()) {
#ifdef  __SYM_TRANSRELSMERGE__
        cout << "mps: 2" << endl;
#endif
        continue;
      }
      else if (guardpart == getBDDMgr()->bddOne()) {
#ifdef  __SYM_TRANSRELSMERGE__
        cout << "mps: 3" << endl;
#endif
        mergeAssignIntoPrim(*p, *s);
#ifdef  __SYM_TRANSRELSMERGE__
        cout << "Modified primary:" << endl;
        printTransRel(*p);
#endif
      }
      else {
        if (simplifyRestrict((*p)->guard * guardpart * invariant)
            == getBDDMgr()->bddZero()) {
#ifdef  __SYM_TRANSRELSMERGE__
          cout << "mps: 4" << endl;
#endif
          continue;
        }
        else if (simplifyRestrict((*p)->guard * !guardpart * invariant)
                 == getBDDMgr()->bddZero()) {
#ifdef  __SYM_TRANSRELSMERGE__
          cout << "mps: 5" << endl;
#endif
          (*p)->guard *= guardpart;
          (*p)->guard = (*p)->guard.Restrict(invariant);
          mergeAssignIntoPrim(*p, *s);
        }
        else {
#ifdef  __SYM_TRANSRELSMERGE__
          cout << "mps: 6" << endl;
#endif
          transStruct* newt = new transStruct();
          newt->guard = (*p)->guard * !guardpart;
          (*p)->guard = (*p)->guard.Restrict(invariant);
          newt->boolAssign = (*p)->boolAssign;
          newt->clockAssign = (*p)->clockAssign;
          newt->lowerClock = (*p)->lowerClock;
          newt->upperClock = (*p)->upperClock;
          newt->type = PRIMARY;
          primary.push_back(newt);
#ifdef  __SYM_TRANSRELSMERGE__
          cout << "Adding new primary transition: " << endl;
          printTransRel(newt);
#endif

          (*p)->guard *= guardpart;
          (*p)->guard = (*p)->guard.Restrict(invariant);
          mergeAssignIntoPrim(*p, *s);
        }
#ifdef  __SYM_TRANSRELSMERGE__
        cout << "Modified primary:" << endl;
        printTransRel(*p);
#endif
      }
    }
  }
  transvector result;
  result.insert(result.end(), primary.begin(), primary.end());
  // insert all secondary transitions into primary transition set.
  // I believe that this really only needs to be the secondary
  // transitions with guards that refer to teh real variables but that
  // inserting them all shouldn't hurt anything.
  for (transvector::iterator i = secondary2.begin(); 
       i != secondary2.end(); i++) {
    if (BDDHasIneqs((*i)->guard)) {
      result.insert(result.end(), *i);
      //cout << "Keeping secondary transition because its guard " 
      //   << "does have inequalities:" << endl;
      //printTransRel(*i);
    }
    else {
      //cout << "WARNING: Tossing secondary transition because its guard " 
      //   << "doesn't have any inequalities:" << endl;
      //printTransRel(*i);
      delete *i;
    }
  }
  //result.insert(result.end(), secondary.begin(), secondary.end());
  return result;
}

// Returns true if the passed in BDD contains any variables that map 
// to inequalities.  Returns fals otherwise.
bool lhpnModelData::BDDHasIneqs(BDD r) {
  if (r == mgr->bddZero() || r == mgr->bddOne()) {
    return false;
  }

  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(mgr->getManager(), 
                             r.getNode(), r.getNode() ,&cube);
  assert(g);
  do {
    for (unsigned int k = 0; k < predStruct::getPredCount(); k++) {
      // Print uncomplemented literals.
      predStruct *ps = predStruct::getPred(k);
      if (ps == NULL) continue;
      if (cube[ps->var] == 1 || cube[ps->var] == 0) {
	GenFree(g);
	return true;
      }
    }
  } while (Cudd_NextPrime(g, &cube));
  GenFree(g);
  return false;
}

// Takes the assignment set from secondary and inserts
// them into primary.  
void lhpnModelData::mergeAssignIntoPrim(transStruct* primary,
                                        transStruct* secondary) {

  // Do some checking to ensure that the assignment
  // sets do not conflict with each other.
  set<int> cint;
  set_intersection(primary->clockAssign.begin(), primary->clockAssign.end(),
                   secondary->clockAssign.begin(), secondary->clockAssign.end(),
                   inserter(cint, cint.begin()));
  if (cint.begin() != cint.end()) {
    cout << "WARNING: Clock assignment sets being merged contain conflicts:";
    for (set<int>::iterator i = cint.begin(); i != cint.end(); i++) {
      cout << "\t" << getVarName(*i) << endl;
    }
    cout << "Secondary assignments will take precedence. "
         << "Is this a self loop?" << endl;
  }
  for (set<int>::iterator i = secondary->clockAssign.begin(); 
       i != secondary->clockAssign.end(); i++) {
    primary->clockAssign.insert(*i);
    primary->lowerClock[*i] = secondary->lowerClock[*i];
    primary->upperClock[*i] = secondary->upperClock[*i];
  }

  // Now do the Boolean variables.
  for (vector<BDD>::iterator i = secondary->boolAssign.begin(); 
       i != secondary->boolAssign.end(); i++) {
    vector<BDD>::iterator j;
    for (j = primary->boolAssign.begin(); 
	 j != primary->boolAssign.end(); j++) {
      if (*i == !(*j)) {
	cout << "WARNING: Bool assignment set being merged contains conflict for ";
	for (map<string, int>::iterator k = allModelVars.begin();
	     k != allModelVars.end(); k++) {
	  if (((int) (*i).NodeReadIndex()) == (int) (*k).second) {
	    cout << (*k).first << endl;
	  }
	}
	cout << "No assignments will be performed on this variable. "
	     << "Is this a self loop situation?" << endl;
	break;
      }
    }
    if (j == primary->boolAssign.end()) {
      primary->boolAssign.insert(primary->boolAssign.end(), *i);
    }
    else {
      primary->boolAssign.erase(j);
    }
  }
}


BDD lhpnModelData::addPredCons(int i, int j, double c) {
  return addPredCons(1, i, 1, j, c);
}

BDD lhpnModelData::addPredCons(double ci, int i, double cj, int j, double c) {
  return predStruct::addPred(ci, i, cj, j, c);
}

int lhpnModelData::addClockVar(string name) {
  for (unsigned int i = 0; i < allRealVars.size(); i++) {
    assert(name != allRealVars[i]);
  }
  //cout << "Adding clock var: " << name << endl;
  allRealVars.push_back(name);
  return (allRealVars.size()-1)*2;
}

string lhpnModelData::getVarName(int varIndex) {
  assert(varIndex >= 0 && varIndex < (int) allRealVars.size()*2);
  if (varIndex%2 == 0)
    return allRealVars[varIndex/2];
  else 
    return "_c_" + allRealVars[varIndex/2];
}

int lhpnModelData::getVarIndex(string name) {
  for (unsigned int i = 0; i < allRealVars.size(); i++) {
    if (allRealVars[i] == name) {
      return i*2;
    }
  }
  return -1;
}

int lhpnModelData::getCVarIndex(string name) {
  return getVarIndex(name)+1;
}

BDD lhpnModelData::getModelVarBDD(string name) {
  if (allModelVars.find(name) == allModelVars.end()) {
    // this should never happen.
    cout << name << endl;
    assert(false);
  }
  return mgr->bddVar(allModelVars[name]);
}

string lhpnModelData::BDDToString(BDD r) {
  ostringstream oss;
  if (r == mgr->bddZero()) {
    oss << "false";
    return oss.str();
  }
  if (r == mgr->bddOne()) {
    oss << "true";
    return oss.str();
  }

  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(mgr->getManager(), 
                             r.getNode(), r.getNode() ,&cube);
  assert(g);
  do {
    for (map<string, int>::iterator i = allModelVars.begin();
         i != allModelVars.end(); i++) {
      if (cube[(*i).second] == 0) {
        oss << "!" << (*i).first << " ";
      }
      else if (cube[(*i).second] == 1) {
        oss << (*i).first << " ";
      }
    }
    for (unsigned int k = 0; k < predStruct::getPredCount(); k++) {
      // Print uncomplemented literals.
      predStruct *ps = predStruct::getPred(k);
      if (ps == NULL) continue;
      if (cube[ps->var] == 1) {
        oss << "(" << ps->toString() << ") ";
      }
      // Print complemented literals.
      else if (cube[ps->var] == 0) {
        oss << "(" << ps->toStringInverted() << ") ";
      }
    }
    oss << "+ ";
  } while (Cudd_NextPrime(g, &cube));
  GenFree(g);
  string result = oss.str();
  return result.erase(result.length()-3);
}

void lhpnModelData::printBDDState(BDD r) {
  if (r == mgr->bddZero()) {
    cout << "false" << endl;
    return;
  }
  if (r == mgr->bddOne()) {
    cout << "true" << endl;
    return;
  }
  //r = simplifyRestrict(r);

  if (r == mgr->bddZero()) {
    cout << "false" << endl;
    return;
  }
  if (r == mgr->bddOne()) {
    cout << "true" << endl;
    return;
  }

  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(mgr->getManager(), 
                             r.getNode(), r.getNode() ,&cube);
  assert(g);
  int count = 1;
  do {
    //for (int i = 0; i < mgr->ReadSize(); i++) {
    //  if (cube[i] == 2) 
    //    cout << "x";
    //  else 
    //    cout << cube[i];
    //}
    cout << endl;
    cout << count++ << ":  Marking: { ";
    for (map<string, int>::iterator i = allModelVars.begin();
         i != allModelVars.end(); i++) {
      if (cube[(*i).second] == 0) {
        cout << "!" << (*i).first << ", ";
      }
      else if (cube[(*i).second] == 1) {
        cout << (*i).first << ", ";
      }
    }
    cout << "\b\b }" << endl;
    if (predStruct::getPredCount() > 0) {
      cout << "    Predicates:" << endl;
    }
    for (unsigned int k = 0; k < predStruct::getPredCount(); k++) {
      // Print uncomplemented literals.
      predStruct *ps = predStruct::getPred(k);
      if (ps == NULL) continue;
      if (cube[ps->var] == 1) {
        cout << "      (" << ps->var << ") " << ps->toString() << endl;
      }
      // Print complemented literals.
      else if (cube[ps->var] == 0) {
        cout << "      (!" << ps->var << ") " << ps->toStringInverted() <<endl;
      }
    }
    cout << endl;
  } while (Cudd_NextPrime(g, &cube));
  GenFree(g);
}


void lhpnModelData::countBDDState(BDD r) {
  if (r == mgr->bddZero()) {
    cout << "false" << endl;
    return;
  }
  if (r == mgr->bddOne()) {
    cout << "true" << endl;
    return;
  }
  //r = simplifyRestrict(r);

  if (r == mgr->bddZero()) {
    cout << "false" << endl;
    return;
  }
  if (r == mgr->bddOne()) {
    cout << "true" << endl;
    return;
  }

  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(mgr->getManager(), 
                             r.getNode(), r.getNode() ,&cube);
  assert(g);
  int count = 1;
  do {
    count++;
  } while (Cudd_NextPrime(g, &cube));
  GenFree(g);
  cout << "Number of primes = " << count << endl;
}

BDD lhpnModelData::abstractInconsistentIneqs(BDD phi, rateprod rates) {
  vector<int> support = getSupport(getBDDMgr(), phi);
  for (vector<int>::iterator p = support.begin(); p != support.end(); p++) {
    predStruct* pred = predStruct::getPred(*p);
    if (pred == NULL) {
      continue;    
    }
    bool abstractTrue = false;
    bool abstractFalse = false;
    // All inequalities containing X0_I
    // lrate < 0 - GEQ case

#ifdef __SYM_PRINTABST__
    cout << pred->toString() << endl;
#endif

    if (pred->i == X0_I && rates[pred->j]->lrate > 0) {
      abstractFalse = true;
    }
    // lrate < 0 - GT case
    else if (pred->j == X0_I && rates[pred->i]->lrate > 0) {
      abstractTrue = true;
    }
    // urate > 0 - GEQ
    if (pred->j == X0_I && rates[pred->i]->urate < 0 ) {
      abstractFalse = true;
    }
    // urate > 0 - GT case
    else if (pred->i == X0_I && rates[pred->j]->urate < 0) {
      abstractTrue = true;
    }
    // All two variables inequalities:
    if (pred->i != X0_I && pred->j != X0_I) {
      if (pred->ci > 0 && pred->cj > 0) {
        if ((pred->ci * rates[pred->i]->lrate-
	     pred->cj * rates[pred->j]->urate) > threshold) {
          abstractTrue = true;
        }
        if ((pred->cj * rates[pred->j]->lrate-
	     pred->ci * rates[pred->i]->urate) > threshold) {
          abstractFalse = true;
        }
      }
      else if (pred->ci < 0 && pred->cj > 0) {
        if ((pred->ci * rates[pred->i]->urate-
	     pred->cj * rates[pred->j]->urate) > threshold) {
          abstractTrue = true;
        }
        if ((pred->cj * rates[pred->j]->lrate-
	     pred->ci * rates[pred->i]->lrate) > threshold) {
          abstractFalse = true;
        }
      }
      else if (pred->ci > 0 && pred->cj < 0) {
        if ((pred->ci * rates[pred->i]->lrate-
	     pred->cj * rates[pred->j]->lrate) > threshold) {
          abstractTrue = true;
        }
        if ((pred->cj * rates[pred->j]->urate-
	     pred->ci * rates[pred->i]->urate) > threshold) {
          abstractFalse = true;
        }
      }
    }

    if (abstractTrue) {
#ifdef __SYM_PRINTABST__
      cout << "Abstracting true instances of " << pred->toString() << endl;
#endif
      phi = abstractVarTense(pred->bdd(), phi);
    }
    if (abstractFalse) {
#ifdef __SYM_PRINTABST__
      cout << "Abstracting false instances of " << pred->toString() << endl;
#endif
      phi = abstractVarTense(!pred->bdd(), phi);
    }
  }
  return phi;
}

BDD lhpnModelData::transTT(double ci, int xi, double cj, int xj, double c1,
                           double ck, int xk, double cm, int xm, double c2,
                           BDD &e1, BDD &e2, BDD &phi) {
  if (xj == xk && xi != N_DLT_I) {
    if (cj > 0 && ck > 0) {
#ifdef __timeElapseNew2__
      cout << "TT1" << endl;
#endif
      return applyTransCons(phi, e1, e2,
                            ci/cj, xi, cm/ck, xm, c1/cj+c2/ck, true);
      
      //BDD e3 = predStruct::addPred(ci/cj, xi, cm/ck, xm, c1/cj+c2/ck);
      //return !(e1 * e2) + e3;
    }
    else if (cj < 0 && ck < 0) {
#ifdef __timeElapseNew2__
      cout << "TT2" << endl;
#endif
      return applyTransCons(phi, e1, e2,
                            cm/ck, xm, ci/cj, xi, (-1)*(c1/cj+c2/ck), true);
      //BDD e3 = predStruct::addPred(cm/ck, xm, ci/cj, xi, (-1)*(c1/cj+c2/ck));
      //return !(e1 * e2) + e3;
    }
  } else if (xi == xk && xj != N_DLT_I) {
    if (ci > 0 && ck < 0) {
#ifdef __timeElapseNew2__
      cout << "TT3" << endl;
#endif
      return applyTransCons(phi, e1, e2,
                            cm/ck, xm, cj/ci, xj, (-1)*c2/ck+c1/ci, true);
      //BDD e3 = predStruct::addPred(cm/ck, xm, cj/ci, xj, (-1)*c2/ck+c1/ci);
      //return !(e1 * e2) + e3;
    }
  } else if (xj == xm && xi != N_DLT_I) {
    if (cj > 0 && cm < 0) {
#ifdef __timeElapseNew2__
      cout << "TT4" << endl;
#endif
      return applyTransCons(phi, e1, e2,
                            ci/cj, xi, ck/cm, xk, (-1)*c2/cm+c1/cj, true);
      //BDD e3 = predStruct::addPred(ci/cj, xi, ck/cm, xk, (-1)*c2/cm+c1/cj);
      //return !(e1 * e2) + e3;
    }
  }
#ifdef __timeElapseNew2__
  cout << "TT5" << endl;
#endif
  return phi;
  //return getBDDMgr()->bddOne();
}

BDD lhpnModelData::transFF(double ci, int xi, double cj, int xj, double c1,
                           double ck, int xk, double cm, int xm, double c2,
                           BDD &e1, BDD &e2, BDD &phi) {
  if (xj == xk && xi != N_DLT_I) {
    if (cj > 0 && ck > 0) {
#ifdef __timeElapseNew2__
      cout << "FF1" << endl;
#endif
      return applyTransCons(phi, !e1, !e2,
                            ci/cj, xi, cm/ck, xm, c1/cj+c2/ck, false);
      //BDD e3 = predStruct::addPred(ci/cj, xi, cm/ck, xm, c1/cj+c2/ck);
      //return !(!e1 * !e2) + !e3;
    }
    else if (cj < 0 && ck < 0) {
#ifdef __timeElapseNew2__
      cout << "FF2" << endl;
#endif
      return applyTransCons(phi, !e1, !e2,
                            cm/ck, xm, ci/cj, xi, (-1)*(c1/cj+c2/ck), false);
      //BDD e3 = predStruct::addPred(cm/ck, xm, ci/cj, xi, (-1)*(c1/cj+c2/ck));
      //return !(!e1 * !e2) + !e3;
    }
  } else if (xi == xk && xj != N_DLT_I) {
    if (ci > 0 && ck < 0) {
#ifdef __timeElapseNew2__
      cout << "FF3" << endl;
#endif
      return applyTransCons(phi, !e1, !e2,
                            cm/ck, xm, cj/ci, xj, (-1)*c2/ck+c1/ci, false);
      //BDD e3 = predStruct::addPred(cm/ck, xm, cj/ci, xj, (-1)*c2/ck+c1/ci);
      //return !(!e1 * !e2) + !e3;
    }
  } else if (xj == xm && xi != N_DLT_I) {
    if (cj > 0 && cm < 0) {
#ifdef __timeElapseNew2__
      cout << "FF4" << endl;
#endif
      return applyTransCons(phi, !e1, !e2,
                            ci/cj, xi, ck/cm, xk, (-1)*c2/cm+c1/cj, false);
      //BDD e3 = predStruct::addPred(ci/cj, xi, ck/cm, xk, (-1)*c2/cm+c1/cj);
      //return !(!e1 * !e2) + !e3;
    }
  }
#ifdef __timeElapseNew2__
  cout << "TT5" << endl;
#endif
  return phi;
  //return getBDDMgr()->bddOne();
}

BDD lhpnModelData::transFT(double ci, int xi, double cj, int xj, double c1,
                           double ck, int xk, double cm, int xm, double c2,
                           BDD &e1, BDD &e2, BDD &phi) {
  if (xi == xk && xj != N_DLT_I) {
    if (ci > 0 && ck > 0) {
#ifdef __timeElapseNew2__
      cout << "FT1" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           cj/ci, xj, cm/ck, xm, -c1/ci+c2/ck, true);
      return applyTransCons(phi, !e1, e2,
                            cm/ck, xm, cj/ci, xj, c1/ci-c2/ck, false);
      //BDD e3 = predStruct::addPred(cj/ci, xj, cm/ck, xm, -c1/ci+c2/ck);
      //BDD e4 = predStruct::addPred(cm/ck, xm, cj/ci, xj, c1/ci-c2/ck);
      //return (!(!e1 * e2) + e3) * (!(!e1 * e2) + !e4);
    }
    else if (ci < 0 && ck < 0) {
#ifdef __timeElapseNew2__
      cout << "FT2" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           cj/ci, xj, cm/ck, xm, -c1/ci+c2/ck, false);
      return applyTransCons(phi, !e1, e2,
                            cm/ck, xm, cj/ci, xj, c1/ci-c2/ck, true);
      //BDD e3 = predStruct::addPred(cj/ci, xj, cm/ck, xm, -c1/ci+c2/ck);
      //BDD e4 = predStruct::addPred(cm/ck, xm, cj/ci, xj, c1/ci-c2/ck);
      //return (!(!e1 * e2) + !e3) * (!(!e1 * e2) + e4);
    }
  } else if (xj == xm && xi != N_DLT_I) {
    if (cj > 0 && cm > 0) {
#ifdef __timeElapseNew2__
      cout << "FT3" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           ck/cm, xk, ci/cj, xi, -c1/cj+c2/cm, true);
      return applyTransCons(phi, !e1, e2,
                            ci/cj, xi, ck/cm, xk, c1/cj-c2/cm, false);
      //BDD e3 = predStruct::addPred(ck/cm, xk, ci/cj, xi, -c1/cj+c2/cm);
      //BDD e4 = predStruct::addPred(ci/cj, xi, ck/cm, xk, c1/cj-c2/cm);
      //return (!(!e1 * e2) + e3) * (!(!e1 * e2) + !e4);
    }
    else if (cj < 0 && cm < 0) {
#ifdef __timeElapseNew2__
      cout << "FT4" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           ck/cm, xk, ci/cj, xi, -c1/cj+c2/cm, false);
      return applyTransCons(phi, !e1, e2,
                            ci/cj, xi, ck/cm, xk, c1/cj-c2/cm, true);
      //BDD e3 = predStruct::addPred(ck/cm, xk, ci/cj, xi, -c1/cj+c2/cm);
      //BDD e4 = predStruct::addPred(ci/cj, xi, ck/cm, xk, c1/cj-c2/cm);
      //return (!(!e1 * e2) + !e3) * (!(!e1 * e2) + e4);
    }
  } else if (xj == xk && xi != N_DLT_I) {
    if (cj > 0 && ck < 0) {
#ifdef __timeElapseNew2__
      cout << "FT5" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           cm/ck, xm, ci/cj, xi, -c1/cj-c2/ck, true);
      return applyTransCons(phi, !e1, e2,
                            ci/cj, xi, cm/ck, xm, c1/cj+c2/ck, false);
      //BDD e3 = predStruct::addPred(cm/ck, xm, ci/cj, xi, -c1/cj-c2/ck);
      //BDD e4 = predStruct::addPred(ci/cj, xi, cm/ck, xm, c1/cj+c2/ck);
      //return (!(!e1 * e2) + e3) * (!(!e1 * e2) + !e4);
    }
    else if (cj < 0 && ck > 0) {
#ifdef __timeElapseNew2__
      cout << "FT6" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           cm/ck, xm, ci/cj, xi, -c1/cj-c2/ck, false);
      return applyTransCons(phi, !e1, e2,
                            ci/cj, xi, cm/ck, xm, c1/cj+c2/ck, true);
      //BDD e3 = predStruct::addPred(cm/ck, xm, ci/cj, xi, -c1/cj-c2/ck);
      //BDD e4 = predStruct::addPred(ci/cj, xi, cm/ck, xm, c1/cj+c2/ck);
      //return (!(!e1 * e2) + !e3) * (!(!e1 * e2) + e4);
    }
  } else if (xi == xm && xj != N_DLT_I) {
    if (ci > 0 && cm < 0) {
#ifdef __timeElapseNew2__
      cout << "FT7" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           cj/ci, xj, ck/cm, xk, -c1/ci-c2/cm, true);
      return applyTransCons(phi, !e1, e2,
                            ck/cm, xk, cj/ci, xj, c1/ci+c2/cm, false);
      //BDD e3 = predStruct::addPred(cj/ci, xj, ck/cm, xk, -c1/ci-c2/cm);
      //BDD e4 = predStruct::addPred(ck/cm, xk, cj/ci, xj, c1/ci+c2/cm);
      //return (!(!e1 * e2) + e3) * (!(!e1 * e2) + !e4);
    }
    else if (ci < 0 && cm > 0) {
#ifdef __timeElapseNew2__
      cout << "FT8" << endl;
#endif
      phi = applyTransCons(phi, !e1, e2,
                           cj/ci, xj, ck/cm, xk, -c1/ci-c2/cm, false);
      return applyTransCons(phi, !e1, e2,
                            ck/cm, xk, cj/ci, xj, c1/ci+c2/cm, true);
      //BDD e3 = predStruct::addPred(cj/ci, xj, ck/cm, xk, -c1/ci-c2/cm);
      //BDD e4 = predStruct::addPred(ck/cm, xk, cj/ci, xj, c1/ci+c2/cm);
      //return (!(!e1 * e2) + !e3) * (!(!e1 * e2) + e4);
    }
  }
#ifdef __timeElapseNew2__
  cout << "FT9" << endl;
#endif
  return phi;
  //return getBDDMgr()->bddOne();
}

BDD lhpnModelData::transTF(double ci, int xi, double cj, int xj, double c1,
                           double ck, int xk, double cm, int xm, double c2,
                           BDD &e1, BDD &e2, BDD &phi) {
  // This function is old and should not be used... instead use
  // transFT with the parameters swapped (i for k, j for m, e1 for
  // e2);
  assert(false);
  assert (ci > 0 || cj > 0);
  assert (ck > 0 || cm > 0);
  if (xj == xm && (cj > 0 && cm > 0) || (cj < 0 && cm < 0)) {
    BDD e3 = predStruct::addPred(ci/cj, xi, ck/cm, xk, c1/cj-c2/cm);
    BDD e4 = predStruct::addPred(ck/cm, xk, ci/cj, xi, -c1/cj+c2/cm);
    if (cj > 0 && cm > 0) {
      cout << "TF1" << endl;
      return (!(e1 * !e2) + e3) * (!(e1 * !e2) + !e4);
    }
    else if (cj < 0 && cm < 0) {
#ifdef __timeElapseNew2__
      cout << "TF2" << endl;
#endif
      return (!(e1 * !e2) + !e3) * (!(e1 * !e2) + e4);
    }
    else {
      assert(false);
#ifdef __timeElapseNew2__
      cout << "TF3" << endl;
#endif
      return getBDDMgr()->bddOne();
    }
  }
  else {
#ifdef __timeElapseNew2__
    cout << "TF4" << endl;
#endif
    return getBDDMgr()->bddOne();
  }
}

BDD lhpnModelData::applyTransCons(BDD phi, BDD A, BDD B,
                                  double ci, int xi,
                                  double cj, int xj, double c, bool tense) {
  //BDD p1 = predStruct::addPred(ci, xi, cj, xj, c);
  //if (tense == true) {
  //  return phi * (!(A*B) + p1);
  // }
  //else {
  //  return phi * (!(A*B) + !p1);
  // }

#ifdef __timeElapseNew3__
  cout << "Testing of applying of predicate: " << endl;
  cout << "pred: " << ci << getVarName(xi) << " >= "
       << cj << getVarName(xj) << " + " << c << " tense: " << tense << endl;
  cout << "A:" << endl;
  printBDDState(A);
  cout << "B:" << endl;
  printBDDState(B);
#endif

  if (predStruct::isTrue(ci, xi, cj, xj, c)) {
    BDD result;
    if (tense == true) {
      //cout << "returning phi 1" << endl;
      result =  phi;
    }
    else {
      //cout << "returning phi * !(A*B) 1" << endl;
      result =  phi * !(A*B);
    }
    //cout << "Result of applyTransCons" << endl;
    //printBDDState(result);
    return result;
  }
  if (predStruct::isFalse(ci, xi, cj, xj, c)) {
    BDD result;
    if (tense == true) {
      //cout << "returning phi * !(A*B) 2" << endl;
      result = phi * !(A*B);
    }
    else {
      //cout << "returning phi 2" << endl;
      result =  phi;
    }
    //cout << "Result of applyTransCons" << endl;
    //printBDDState(result);
    return result;
  }
      
  //cout << "Checking Predicate: " << p->toString() << endl;
  
  BDD abpart1 = A*B*phi; // All parts where A*B could be true.
  if (abpart1 == getBDDMgr()->bddZero()) {
    return phi;
  }
  BDD abpart2 = abpart1;

  vector<BDD> implies_p;
  vector<BDD> p_implies;
  // p is the predicate which may or may not be create.
  // Find all the predicate in the support of phi that either imply
  // p or that p implies.
  vector<int> support = getSupport(getBDDMgr(), abpart1);
  predStruct* p = predStruct::getTempPred(ci, xi, cj, xj, c);
  for (vector<int>::iterator i = support.begin();
       i != support.end(); i++) {
    predStruct* pred = predStruct::getPred(*i);
    if (pred == NULL) continue;
    if (tense == true) {
      // p_implies
      if (predStruct::timpliest(p, pred)) {
        p_implies.push_back(pred->bdd());
      }
      else if (predStruct::timpliesf(p, pred)) {
        p_implies.push_back(!pred->bdd());
      }
      // implies_p
      if (predStruct::timpliest(pred, p)) {
        implies_p.push_back(pred->bdd());
      }
      else if (predStruct::fimpliest(pred, p)) {
        implies_p.push_back(!pred->bdd());
      }
    }
    else {
      // p_implies
      if (predStruct::fimpliest(p, pred)) {
        p_implies.push_back(pred->bdd());
      }
      else if (predStruct::fimpliesf(p, pred)) {
        p_implies.push_back(!pred->bdd());
      }
      // implies_p
      if (predStruct::timpliesf(pred, p)) {
        implies_p.push_back(pred->bdd());
      }
      else if (predStruct::fimpliesf(pred, p)) {
        implies_p.push_back(!pred->bdd());
      }
    }
  }
  delete p;

#ifdef __timeElapseNew3__
  cout << "applyTransCons: Before applying constraint:" << endl;
  printBDDState(phi);
#endif
  
  // Find everything where p will be the tightest bound.
#ifdef __timeElapseNew3__
  cout << "Cofactoring implies_p" << endl;
#endif
  for (vector<BDD>::iterator i = implies_p.begin();
       i != implies_p.end(); i++) {
#ifdef __timeElapseNew3__
    printBDDState(*i);
#endif
    abpart1 = abpart1.Cofactor(!*i);
  }
  // If p is never the tightest bound, do nothing.
  // Otherwise, remove all constraints from abpart that are less
  // tight than p.  This also removes inconsistent constraints.
  if (abpart1 != getBDDMgr()->bddZero()) {
    phi = phi - abpart2;
#ifdef __timeElapseNew3__
    cout << "Cofactoring p_implies" << endl;
#endif
    for (vector<BDD>::iterator i = p_implies.begin();
         i != p_implies.end(); i++) {
#ifdef __timeElapseNew3__
      printBDDState(*i);
#endif
      abpart2 = abpart2.Cofactor(*i);
    }
    // Put the new predicated into abpart.
    BDD p = predStruct::addPred(ci, xi, cj, xj, c);
    if (tense == true) {
      abpart2 *= p;
    }
    else {
      abpart2 *= !p;
    }
    // Put abpart back onto phi.
    phi = phi + abpart2;
  }
#ifdef __timeElapseNew3__
  cout << "applyTransCons: after applying constraint:" << endl;
  printBDDState(phi);
#endif
  return phi;
}


void lhpnModelData::trueForm(predStruct* p, const rateprod &rates,
                             double &ci, int &xi, double &cj,
                             int &xj, double &c) {
  assert (p->i == X0_I || p->j == X0_I);
  assert (p->i != X0_I || p->j != X0_I);
  if (p->j == X0_I) {
    assert (p->ci == 1.0);
    ci = 1.0;
    xi = p->i;
    cj = rates.find(p->i)->second->urate;
    xj = N_DLT_I;
    c = p->c;
  }
  else if (p->i == X0_I) {
    assert (p->cj == 1.0);
    ci = rates.find(p->j)->second->lrate;
    xi = N_DLT_I;
    cj = 1.0;
    xj = p->j;
    c = p->c;
  }
#ifdef __timeElapseNew2__
  cout << "trueForm: ci=" << ci << " xi=" << xi << " cj=" << cj << " xj="
       << xj << " c=" << c << endl;
#endif
}

void lhpnModelData::falseForm(predStruct* p, const rateprod &rates,
                              double &ci, int &xi, double &cj,
                              int &xj, double &c) {
#ifdef __timeElapseNew2__
  cout << "falseForm initially: ci=" << p->ci << " xi=" << p->i << " p->cj="
       << p->cj << " xj=" << p->j << " c=" << p->c << endl;
  cout << "p->i lrate: " << rates.find(p->i)->second->lrate << endl;
  cout << "p->j urate: " << rates.find(p->j)->second->urate << endl;
#endif
  assert (p->i == X0_I || p->j == X0_I);
  assert (p->i != X0_I || p->j != X0_I);
  if (p->j == X0_I) {
    assert (p->ci == 1.0);
    ci = 1.0;
    xi = p->i;
    cj = rates.find(p->i)->second->lrate;
    xj = N_DLT_I;
    c = p->c;
  }
  else if (p->i == X0_I) {
    assert (p->cj == 1.0);
    ci = rates.find(p->j)->second->urate;
    xi = N_DLT_I;
    cj = 1.0;
    xj = p->j;
    c = p->c;
  }
#ifdef __timeElapseNew2__
  cout << "falseForm: ci=" << ci << " xi=" << xi << " cj=" << cj << " xj="
       << xj << " c=" << c << endl;
#endif
}


typedef pair<bool, predStruct*> tensePred;
struct xi_less {
  bool operator()(tensePred p1, tensePred p2) const {
    int p1i;
    int p2i;
    if (p1.first)
      p1i = p1.second->i;
    else
      p1i = p1.second->j;
    if (p2.first)
      p2i = p2.second->i;
    else
      p2i = p2.second->j;
    return(p1i < p2i);
  }
};

struct xj_less {
  bool operator()(tensePred p1, tensePred p2) const {
    int p1j;
    int p2j;
    if (p1.first)
      p1j = p1.second->j;
    else
      p1j = p1.second->i;
    if (p2.first)
      p2j = p2.second->j;
    else
      p2j = p2.second->i;
    return(p1j < p2j);
  }
};

struct ci_less {
  bool operator()(tensePred p1, tensePred p2) const {
    double p1ci;
    double p2ci;
    if (p1.first)
      p1ci = p1.second->ci;
    else
      p1ci = p1.second->cj/fabs(p1.second->cj);
    if (p2.first)
      p2ci = p2.second->ci;
    else
      p2ci = p2.second->cj/fabs(p2.second->cj);
    return(p1ci < p2ci);
  }
};

struct cj_less {
  bool operator()(tensePred p1, tensePred p2) const {
    double p1cj;
    double p2cj;
    if (p1.first)
      p1cj = p1.second->cj;
    else
      p1cj = p1.second->ci/fabs(p1.second->cj);
    if (p2.first)
      p2cj = p2.second->cj;
    else
      p2cj = p2.second->ci/fabs(p2.second->cj);
    return(p1cj < p2cj);
  }
};

struct c_less {
  bool operator()(tensePred p1, tensePred p2) const {
    double p1c;
    double p2c;
    if (p1.first)
      p1c = p1.second->c;
    else
      p1c = -p1.second->c/fabs(p1.second->cj);
    if (p2.first)
      p2c = p2.second->c;
    else
      p2c = -p2.second->c/fabs(p2.second->cj);
    if (!p1.first)
      return(p1c >= p2c);
    else
      return (p1c > p2c);
  }
};


struct nonparallel {
  bool operator()(tensePred p1, tensePred p2) const {
    int p1i, p1j;
    double p1ci, p1cj;
    if (p1.first) {
      p1i = p1.second->i;
      p1j = p1.second->j;
      p1ci = p1.second->ci;
      p1cj = p1.second->cj;
    }
    else {
      p1i = p1.second->j;
      p1j = p1.second->i;
      p1ci = p1.second->cj/fabs(p1.second->cj);
      p1cj = p1.second->ci/fabs(p1.second->cj);
    }
    int p2i, p2j;
    double p2ci, p2cj;
    if (p2.first) {
      p2i = p2.second->i;
      p2j = p2.second->j;
      p2ci = p2.second->ci;
      p2cj = p2.second->cj;
    }
    else {
      p2i = p2.second->j;
      p2j = p2.second->i;
      p2ci = p2.second->cj/fabs(p2.second->cj);
      p2cj = p2.second->ci/fabs(p2.second->cj);
    }
    return ((p1i != p2i) || (p1j != p2j) ||
            (fabs(p1ci - p2ci) > threshold) ||
            (fabs(p1cj - p2cj) > threshold));
  }
};

BDD lhpnModelData::cleanupbasedonrate(BDD phi) {
  BDD result = getBDDMgr()->bddZero();
  for (vector<BDD>::iterator rbdd = eachRateBDD.begin();
       rbdd != eachRateBDD.end(); rbdd++) {
    BDD part = phi * (*rbdd);
    while (part!=getBDDMgr()->bddZero()) {
      int* cube = NULL;
      CUDD_VALUE_TYPE c;
      DdGen *g = part.FirstCube(&cube, &c);
      BDD partpart = mgr->bddOne();
      for (int i = 0; i < getBDDMgr()->ReadSize(); i++) {
        if (cube[i] == 1) {
          partpart *= mgr->bddVar(i);
        }
        else if (cube[i] == 0) {
          partpart *= !mgr->bddVar(i);
        }
      }
      GenFree(g);
      part -= partpart;
      result += cleanup(partpart);
    }
    //    result += cleanup(part);
  }
  return result;
}



BDD lhpnModelData::cleanup(BDD phi) {
  //cout << "Applying BDD cleanup to " << endl;
  //printBDDState(phi);
  if (phi == getBDDMgr()->bddZero()) {
    return phi;
  }
  vector< tensePred > supportPreds;
  set<int> support = getSupportSet(getBDDMgr(), phi);
  for (set<int>::iterator pi = support.begin(); pi != support.end(); pi++) {
    predStruct* pred = predStruct::getPred(*pi);
    if (pred != NULL) {
      supportPreds.push_back(tensePred(true, pred));
      supportPreds.push_back(tensePred(false, pred));
    }
  }
  // Stable sort preds by xi, then xj, then ci, then cj, then c
  stable_sort(supportPreds.begin(), supportPreds.end(), c_less());
  stable_sort(supportPreds.begin(), supportPreds.end(), cj_less());
  stable_sort(supportPreds.begin(), supportPreds.end(), ci_less());
  stable_sort(supportPreds.begin(), supportPreds.end(), xj_less());
  stable_sort(supportPreds.begin(), supportPreds.end(), xi_less());

  //cout << "Sorted supportPreds" << endl;
  //for (vector<tensePred>::iterator i = supportPreds.begin();
  //     i != supportPreds.end(); i++) {
  //  if ((*i).first)
  //    cout << (*i).second->toString() << endl;
  //  else
  //    cout << (*i).second->toStringInverted() << endl;
  //}
  
  // split the supportPreds along where adjacent preds do not
  // represent parallel lines.
  vector< vector<tensePred> > partitioned;
  while (!supportPreds.empty()) {
    vector<tensePred>::iterator split = adjacent_find(supportPreds.begin(),
                                                      supportPreds.end(),
                                                      nonparallel());
    if (split != supportPreds.end()) split++;
    partitioned.push_back(vector<tensePred>(supportPreds.begin(), split));
    supportPreds.erase(supportPreds.begin(), split);
  }

#ifdef __SYM_CLEANUP__
  cout << "Cleanup has found the following implication groups: " << endl;
  for (vector< vector<tensePred> >::iterator i = partitioned.begin();
       i != partitioned.end(); i++) {
    for (vector<tensePred>::iterator j = (*i).begin();
         j != (*i).end(); j++) {
      if ((*j).first)
        cout << (*j).second->toString() << endl;
      else
        cout << (*j).second->toStringInverted2() << endl;      
    }
    cout << endl;
  }
#endif
  
  // Now do the cofactoring to remove unnecessary inequalities.
  for (vector< vector<tensePred> >::iterator i = partitioned.begin();
       i != partitioned.end(); i++) {
    if (phi == mgr->bddZero()) {
      break;
    }
    BDD test = phi;
    for (vector<tensePred>::iterator j = (*i).begin();
         j != (*i).end(); j++) {
      if (phi == mgr->bddZero()) {
        break;
      }
      if (test == getBDDMgr()->bddZero()) {
        if ((*j).first) {
#ifdef __SYM_CLEANUP__
          cout << "Cleanup is removing "
               << (*j).second->toString() << endl;
#endif
          phi = phi.Cofactor((*j).second->bdd());
        }
        else {
#ifdef __SYM_CLEANUP__
          cout << "Cleanup is removing "
               << (*j).second->toStringInverted2() << endl;
#endif
          phi = phi.Cofactor(!(*j).second->bdd());
        }
      }
      else {
        if ((*j).first) {
          test = test.Cofactor(!(*j).second->bdd());
        }
        else {
          test = test.Cofactor((*j).second->bdd());
        }
      }
    }
  } 
  
  return phi;
}


BDD lhpnModelData::addTranConsNew2(BDD phi,BDD guard) {
  vector<int> gsupport = getSupport(getBDDMgr(), guard);
  if (gsupport.begin()==gsupport.end()) return phi;

  // iterate through each possible rate grouping.
  BDD result = getBDDMgr()->bddZero();
  for (vector<BDD>::iterator rbdd = eachRateBDD.begin();
       rbdd != eachRateBDD.end(); rbdd++) {
    BDD part = phi * (*rbdd);

    vector<int> support = getSupport(getBDDMgr(), part);
    for (vector<int>::iterator p1 = gsupport.begin(); 
         p1 != gsupport.end(); p1++) {
      predStruct* pred1 = predStruct::getPred(*p1);
      if (pred1 == NULL) {
        continue;    
      }
      for (vector<int>::iterator p2 = support.begin(); 
           p2 != support.end(); p2++) {
        if (p1==p2) continue;
        predStruct* pred2 = predStruct::getPred(*p2);
        if (pred2 == NULL) {
          continue; 
        }
        double ci = pred1->ci, cj = pred1->cj, c1 = pred1->c;
        int xi = pred1->i, xj = pred1->j;
        double ck = pred2->ci, cm = pred2->cj, c2 = pred2->c;
        int xk = pred2->i, xm = pred2->j;

	if ((xi == X0_I) && (xk == X0_I)) continue;
	if ((xi == X0_I) && (xm == X0_I)) continue;
	if ((xj == X0_I) && (xk == X0_I)) continue;
	if ((xj == X0_I) && (xm == X0_I)) continue;

//         cout << "pred1=" << pred1->toString()
//              << " pred2=" << pred2->toString()
//              << endl;
        BDD e1 = pred1->bdd();
        BDD e2 = pred2->bdd();
        part = transTT(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, part);
        part = transFT(ck, xk, cm, xm, c2, ci, xi, cj, xj, c1, e2, e1, part);
        part = transFT(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, part);
        part = transFF(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, part);
      }
    }
    result += part;
  }
  return result;
}

BDD lhpnModelData::addTranConsNew(BDD phi) {
  // iterate through each possible rate grouping.
  BDD result = getBDDMgr()->bddZero();
  for (vector<BDD>::iterator rbdd = eachRateBDD.begin();
       rbdd != eachRateBDD.end(); rbdd++) {
    BDD part = phi * (*rbdd);
    
    vector<int> support = getSupport(getBDDMgr(), part);
    for (vector<int>::iterator p1 = support.begin(); 
         p1 != support.end(); p1++) {
      predStruct* pred1 = predStruct::getPred(*p1);
      if (pred1 == NULL) {
        continue;    
      }
      for (vector<int>::iterator p2 = support.begin(); 
           p2 != support.end(); p2++) {
        if (p1==p2) continue;
        predStruct* pred2 = predStruct::getPred(*p2);
        if (pred2 == NULL) {
          continue; 
        }
        //cout << "pred1=" << pred1->toString()
        //     << " pred2=" << pred2->toString()
        //     << endl;
        double ci = pred1->ci, cj = pred1->cj, c1 = pred1->c;
        int xi = pred1->i, xj = pred1->j;
        double ck = pred2->ci, cm = pred2->cj, c2 = pred2->c;
        int xk = pred2->i, xm = pred2->j;
        BDD e1 = pred1->bdd();
        BDD e2 = pred2->bdd();
        part = transTT(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, part);
        part = transFT(ck, xk, cm, xm, c2, ci, xi, cj, xj, c1, e2, e1, part);
        part = transFT(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, part);
        part = transFF(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, part);
      }
    }
    result += part;
  }
  return result;
}


BDD lhpnModelData::addNewIneqsHelper(predStruct* p1, predStruct* p2,
                                     const rateprod &rates, BDD &phi) {
  double ci, cj, c1;
  int xi, xj;
  double ck, cm, c2;
  int xk, xm;
  BDD e1 = p1->bdd();
  BDD e2 = p2->bdd();
  
  // true, true
  trueForm(p1, rates, ci, xi, cj, xj, c1);
  trueForm(p2, rates, ck, xk, cm, xm, c2);
  phi = transTT(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, phi);
  // true, false
  trueForm(p1, rates, ci, xi, cj, xj, c1);
  falseForm(p2, rates, ck, xk, cm, xm, c2);
  //result *= transTF(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2);
  phi = transFT(ck, xk, cm, xm, c2, ci, xi, cj, xj, c1, e2, e1, phi);
  // false, true
  falseForm(p1, rates, ci, xi, cj, xj, c1);
  trueForm(p2, rates, ck, xk, cm, xm, c2);
  phi = transFT(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, phi);
  // false, false
  falseForm(p1, rates, ci, xi, cj, xj, c1);
  falseForm(p2, rates, ck, xk, cm, xm, c2);
  phi = transFF(ci, xi, cj, xj, c1, ck, xk, cm, xm, c2, e1, e2, phi);
  return phi;
}

BDD lhpnModelData::addNewIneqs(BDD phi, const rateprod &rates) {
  vector<int> support = getSupport(getBDDMgr(), phi);
  for (vector<int>::iterator p1 = support.begin(); 
       p1 != support.end(); p1++) {
    predStruct* pred1 = predStruct::getPred(*p1);
    if (pred1 == NULL || (pred1->j != X0_I && pred1->i != X0_I)) {
      continue;    
    }
    assert (pred1->ci > 0 && pred1->cj > 0);
    for (vector<int>::iterator p2 = support.begin(); 
         p2 != support.end(); p2++) {
      if (p1==p2) continue;
      predStruct* pred2 = predStruct::getPred(*p2);
      if (pred2 == NULL || (pred2->j != X0_I && pred2->i != X0_I)) {
        continue; 
      }
#ifdef __timeElapseNew2__
      cout << "pred1=" << pred1->toString() << " pred2=" << pred2->toString()
           << endl;
#endif
      phi = addNewIneqsHelper(pred1, pred2, rates, phi);
    }
  }
  return phi;
}

BDD lhpnModelData::timeElapseNew(BDD phi) {
  BDD result = getBDDMgr()->bddZero();
  vector<BDD>::iterator rbdd = eachRateBDD.begin();
  ratesop::iterator rprod = eachRateProd.begin();
  // TODO:  Insert assertion that there are no inequalities containing
  // variables without a current rate.

  int count=0;

#ifdef __timeElapseNew__
  cout << "PHI " << endl;
  printBDDState(phi);
#endif

  while (rbdd != eachRateBDD.end() && rprod != eachRateProd.end()) {
    count++;

    BDD part = phi * (*rbdd);
#ifdef __timeElapseNew__
    cout << "RBDD " << count << endl;
    printBDDState(*rbdd);

    cout << "PART " << count << endl;
    printBDDState(part);
#endif

//     if (part!=getBDDMgr()->bddZero()) {
//       rateprod rates = *rprod;
//       part = addNewIneqs(part, rates);
// #ifdef __timeElapseNew__
//       cout << "ADDINEQS " << count << endl;
//       printBDDState(part);
// #endif
//       if (part!=getBDDMgr()->bddZero()) {
// 	part = abstractInconsistentIneqs(part, rates);
// #ifdef __timeElapseNew__
// 	cout << "ABSINEQS " << count << endl;
// 	printBDDState(part);
// #endif
// 	result += part;
//       }
//     }
    while (part!=getBDDMgr()->bddZero()) {
      int* cube = NULL;
      CUDD_VALUE_TYPE c;
      DdGen *g = part.FirstCube(&cube, &c);
      BDD partpart = mgr->bddOne();
      for (int i = 0; i < getBDDMgr()->ReadSize(); i++) {
        if (cube[i] == 1) {
          partpart *= mgr->bddVar(i);
        }
        else if (cube[i] == 0) {
          partpart *= !mgr->bddVar(i);
        }
      }
      part -= partpart;
      if (canTimeElapse(cube)) {
#ifdef __timeElapseNew__
	cout << "PARTPART " << count << endl;
	printBDDState(partpart);
#endif
        
	rateprod rates = *rprod;
	partpart = addNewIneqs(partpart, rates);
#ifdef __timeElapseNew__
	cout << "ADDINEQS " << count << endl;
	printBDDState(partpart);
#endif
	if (partpart!=getBDDMgr()->bddZero()) {
	  partpart = abstractInconsistentIneqs(partpart, rates);
#ifdef __timeElapseNew__
	  cout << "ABSINEQS " << count << endl;
	  printBDDState(partpart);
#endif
	}
      }
/*       else { */
/*       	cout << "Determined that time cannot elapse for: " << endl; */
/*       	printBDDState(partpart); */
/*       } */
      result += partpart;
      GenFree(g);
    }
    rbdd++;
    rprod++;
  }
#ifdef __timeElapseNew__
  cout << "RESULT " << endl;
  printBDDState(result);
#endif

  return result;
}

// Given a cube, determines if the inequalities in the corresponding
// BDD would allow time to elapse.
// TODO:  perhaps use this to find obviously conflicting inequalities and 
// throwing out the terms if found.

// NOTE (11/16/2006): We're not totally convinced that this is correct. 
// However, it does seem to work for the example in the ASP-DAC paper.
// Specifically, we're concerned that time may be able to elapse backwards
// if the clock, which is constraining time becomes disabled.  But maybe
// transitions which disable clocks and thereby pull them out, will allow time
// to proceed once again.
bool lhpnModelData::canTimeElapse(int *cube) {
  for (int i = 0; i < getBDDMgr()->ReadSize(); i++) {
    if (cube[i] != 1) {
      continue;
    }
    predStruct *pi = predStruct::getPred(i);
    if (pi == NULL || (pi->i != X0_I && pi->j != X0_I) || 
	(getVarName(pi->i)[0] != '#' && getVarName(pi->j)[0] != '#')) {
      continue;
    }
    for (int j = i+1; j < getBDDMgr()->ReadSize(); j++) {
      if (cube[j] != 1) {
	continue;
      }
      predStruct *pj = predStruct::getPred(j);
      if (pj == NULL || (pj->i != X0_I && pj->j != X0_I) ||
	  (getVarName(pj->i)[0] != '#' && getVarName(pj->j)[0] != '#')) {
	continue;
      }
      // Need a (X0 >= #var + 0 and #var >= X0 + 0) relationship
      // # preceeding a variable name indicates it is a clock variable
      // on a transition.
      // for time to be prevented from elapsing
      if (pi->i == pj->j && pi->j == pj->i &&
	  fabs(pi->c) < threshold && fabs(pj->c) < threshold) {
	return false;
      }
    }
  }
  return true;
}

BDD lhpnModelData::timeElapsePhi1(BDD phi1) {
  vector<int> support;
  support = getSupport(getBDDMgr(), phi1);
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    // Ignore marked and fired nodes
    if (ps == NULL) continue;
    // If the BDD variable is dependended on in phi1, check if the predicate
    // contains a zero.  If it does, subsitute for a new predicate BDD
    // where the zero has been replaced with the corresponding c variable.
    if (ps->i == X0_I && ps->j != X0_I) {
#ifdef __SYM_PRINTSUBS__
      cout << "te1: Substituting predicate " << ps->toString() << endl;
#endif // __SYM_PRINTSUBS__
      BDD subBDD = predStruct::addPred(ps->ci, getCVarIndex(getVarName(ps->j)),
                                       ps->cj, ps->j, ps->c);
      phi1 = phi1.Compose(subBDD, ps->var);
    }
    if (ps->j == X0_I && ps->i != X0_I) {
#ifdef __SYM_PRINTSUBS__
      cout << "te2: Substituting predicate " << ps->toString() << endl;
#endif // __SYM_PRINTSUBS__  
      BDD subBDD = predStruct::addPred(ps->ci, ps->i, ps->cj,
                                       getCVarIndex(getVarName(ps->i)), ps->c);
      phi1 = phi1.Compose(subBDD, ps->var);
    }
  }
  // Couple of extra predicates.
  BDD result = rphieps * phi1;
  // result = unconstrain(this, result);
  // Add transitivity constraints for all c1 variables
  for (int i = 0; i < (int) allRealVars.size()*2; i+=2) {
    if (i == X0_I || i == N_DLT_I || i == N_EPS_I) {
      continue;
    }
    result = addTranCons(this, result, i+1);
  }
  support = getSupport(getBDDMgr(), result);
  BDD abstractVars = mgr->bddOne();
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    if (ps == NULL) continue;
    if (ps->i%2 == 1 || ps->j%2 ==1) {
      abstractVars *= getBDDMgr()->bddVar(ps->var);
    }
  }
#ifdef __SYM_PRINTABST__
  cout << "Before abstracting c1: " << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  result = result.ExistAbstract(abstractVars);
#ifdef __SYM_PRINTABST__
  cout << "After abstracting c1: " << endl;
  printBDDState(result);
  cout << "Inverse of after abstracting c1: " << endl;
  //printBDDState(!result);
#endif // __SYM_PRINTABST__
  
  // N_EPS_I <= x0
  BDD b1 = predStruct::addPred(X0_I, N_EPS_I, 0);
  // N_DLT_I <= N_EPS_I
  BDD b2 = predStruct::addPred(N_EPS_I, N_DLT_I, 0);
  // N_DLT_I <= x0
  BDD b3 = predStruct::addPred(X0_I, N_DLT_I, 0);
  result = b1 * b2 * !result;
  //printf("addTranCons2\n");
  //printBDDState(result);
  result = addTranCons(this, result, N_EPS_I);
  support = getSupport(getBDDMgr(), result);
  abstractVars = mgr->bddOne();
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    if (ps == NULL) continue;
    if (ps->i == N_EPS_I || ps->j == N_EPS_I) {
      abstractVars *= getBDDMgr()->bddVar(ps->var);
    }
  }
#ifdef __SYM_PRINTABST__
  cout << "Before abstracting eps: " << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  result = result.ExistAbstract(abstractVars);
#ifdef __SYM_PRINTABST__
  //cout << "After abstracting eps: " << endl;
  //printBDDState(result);
  cout << "invariant*!After abstracting eps: " << endl;
  printBDDState(buildInvariant()*!result * b3);
#endif // __SYM_PRINTABST__
  return result;
}

BDD lhpnModelData::timeElapsePhi2(BDD phi2, BDD result) {
#ifdef __SYM_PRINTABST__
  cout << "phi2 BEFORE: " << endl;
  printBDDState(phi2);
#endif // __SYM_PRINTABST__
  vector<int> support = getSupport(getBDDMgr(), phi2);
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    // Ignore marked and fired nodes
    if (ps == NULL) continue;
    // If the BDD variable is depended on in phi2, check if the predicate
    // contains a zero.  If it does, subsitute for a new predicate BDD
    // where the zero has been replaced with the corresponding c variable.   
    if (ps->i == X0_I && ps->j != X0_I) {
#ifdef __SYM_PRINTSUBS__
      cout << "te3: Substituting predicate " << ps->toString() << endl;
#endif // __SYM_PRINTSUBS__  
      BDD subBDD = predStruct::addPred(ps->ci, getCVarIndex(getVarName(ps->j)),
                                         ps->cj, ps->j, ps->c);
      phi2 = phi2.Compose(subBDD, ps->var);
    }
    if (ps->j == X0_I && ps->i != X0_I) {
#ifdef __SYM_PRINTSUBS__
      cout << "te4: Substituting predicate " << ps->toString() << endl;
#endif // __SYM_PRINTSUBS__
      BDD subBDD = predStruct::addPred(ps->ci, ps->i, ps->cj,
                                       getCVarIndex(getVarName(ps->i)), ps->c);
      phi2 = phi2.Compose(subBDD, ps->var);
    }
  }
#ifdef __SYM_PRINTABST__
  cout << "phi2 AFTER: " << endl;
  printBDDState(phi2);
#endif // __SYM_PRINTABST__

  BDD b3 = predStruct::addPred(X0_I, N_DLT_I, 0);
  result = rphidlt * phi2 * result * b3;
  //result = rphidlt * phi2 /** buildInvariant()*/ * b3;
#ifdef __SYM_PRINTABST__
  cout << "rphidlt*phi2*!result:" << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  // result = unconstrain(this, result);
  // Add transitivity constraints for all c2 variables
  for (int i = 0; i < (int) allRealVars.size()*2; i+=2) {
    if (i == X0_I || i == N_DLT_I || i == N_EPS_I) {
      continue;
    }
    result = addTranCons(this, result, i+1);
  }
  //cout << "done adding trancons for c2" << endl;
  
  support = getSupport(getBDDMgr(), result);
  BDD abstractVars = mgr->bddOne();
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    if (ps == NULL) continue;
    if (ps->i%2 == 1 || ps->j%2 ==1) {
      abstractVars *= getBDDMgr()->bddVar(ps->var);
    }
  }
#ifdef __SYM_PRINTABST__  
  cout << "Before abstracting c2: " << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  result = result.ExistAbstract(abstractVars);
#ifdef __SYM_PRINTABST__
  cout << "After abstracting c2: " << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  

  result = b3 * result;
#ifdef __SYM_PRINTABST__
  cout << "After x0 >= dlt and'ed on" << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  
  result = addTranCons(this, result, N_DLT_I);
  // quantify over N_DLT_I in phi2
  support = getSupport(getBDDMgr(), result);
  abstractVars = mgr->bddOne();
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    if (ps == NULL) continue;
    if (ps->i == N_DLT_I || ps->j == N_DLT_I) {
      abstractVars *= getBDDMgr()->bddVar(ps->var);
    }
  }
  
#ifdef __SYM_PRINTABST__
  cout << "Before abstracting dlt: " << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  result = result.ExistAbstract(abstractVars);
#ifdef __SYM_PRINTABST__
  cout << "After abstracting dlt: " << endl;
  printBDDState(result);
#endif // __SYM_PRINTABST__
  //cout << "Returning from timeElapse" << endl;
  return result;
}


BDD lhpnModelData::timeElapse(BDD phi1, BDD phi2) {
  //cout << "Beginning timeElapse calculation" << endl;
  /*
    if (phi1.nodeCount() > 100) {
    phi1 = simplifyRestrict(phi1);
    }
    if (phi2.nodeCount() > 100) {
    phi2 = simplifyRestrict(phi2);
    }
  */
  
  // TODO: VectorCompose may be more efficient here.  Although
  // allocating a BDDVector the same length as the number of variables
  // may not be efficient either.
  
  //cout << "timeElapse received: " << endl;
  //cout << "phi1: " << endl;
  //printBDDState(phi1);
  //cout << "phi2: " << endl;
  //printBDDState(phi2);
  
  // Add transitivity constraints for all regular variables.
  //for (int i = 0; i < design->nplaces; i++) {
  //  if (con_place(design->nevents+i)) {
  //    phi1 = addTranCons(this, phi1, i);
  //  }
  //}
  BDD result;
#ifndef __SYM_FORCEUNOPTIMIZEDTIMEELAPSE__
  if (checkImplication(phi1, buildInvariant()) == getBDDMgr()->bddZero()) {
    //result = timeElapsePhi2(phi2, buildInvariant());
        result = timeElapseNew(phi2);
  }
  else {
#endif
    result = !timeElapsePhi1(phi1);
    result = timeElapsePhi2(phi2, result);
#ifndef __SYM_FORCEUNOPTIMIZEDTIMEELAPSE__
  }
#endif
  return result;
}

// r must be a product term.
rateprod lhpnModelData::bddToRateProd(BDD r) {
  if (r == mgr->bddZero()) {
    assert(false);
  }
  if (r == mgr->bddOne()) {
    assert(false);
  }
  int count = 0;
  rateprod result;
  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(mgr->getManager(), 
                             r.getNode(), r.getNode() ,&cube);
  assert(g);
  do {
    count++;
    for (map<string, int>::iterator i = allModelVars.begin();
         i != allModelVars.end(); i++) {
      if (cube[(*i).second] == 1) {
        variableRate *vr = stringToRate((*i).first);
        result[vr->varIndex] = vr;
      }
    }
  } while (Cudd_NextPrime(g, &cube));
  GenFree(g);
  assert(count == 1);
  return result;
}


ratesop lhpnModelData::rateSOPVector(vector<BDD> ratevector) {
  ratesop result;
  for (vector<BDD>::iterator i = ratevector.begin();
       i != ratevector.end(); i++) {
    result.push_back(bddToRateProd(*i));
  }
  
#ifdef __SYM_PRINTRATEVECTOR__
  cout << "Each rate in rateSOPVector" << endl;
  int count = 0;
  for (ratesop::iterator i = result.begin(); i != result.end(); i++) {
    cout << count << ": ";
    for (rateprod::iterator j = (*i).begin(); j != (*i).end(); j++) {
      cout << (*j).first << ": "
           << getVarName((*j).second->varIndex) << "[" << (*j).second->lrate
           << "," << (*j).second->urate << "] ";
    }
    cout << endl;
    count++;
  }
  cout << "End of each rate." << endl;
#endif
  return result;
}

vector<BDD> lhpnModelData::rateBDDVector() {
  BDD pterm = getBDDMgr()->bddOne();
  for (vector<BDD>::iterator i = placeDisVariables.begin();
       i != placeDisVariables.end(); i++) {
    pterm *= *i;
  }
  for (vector<BDD>::iterator i = booleanDisVariables.begin();
       i != booleanDisVariables.end(); i++) {
    pterm *= *i;
  }
  //for (vector<BDD>::iterator i = clockActiveDisVariables.begin();
  //     i != clockActiveDisVariables.end(); i++) {
  //  pterm *= *i;
  //}
  //cout << "Non rate discrete variables: " <<  BDDToString(pterm) << endl;
  vector<BDD> result;
  for (vector<BDD>::iterator i = eachDisState.begin();
       i != eachDisState.end(); i++) {
    BDD rate = (*i).ExistAbstract(pterm);
    if (find(result.begin(), result.end(), rate) == result.end()) {
      result.push_back(rate);
    }
  }

#ifdef __SYM_PRINTRATEVECTOR__
  cout << "Each rate in rateBDDVector" << endl;
  int count = 0;
  for (vector<BDD>::iterator i = result.begin(); i != result.end(); i++) {
    cout << count << ": " << BDDToString(*i) << endl;
    count++;
  }
  cout << "End of each rate." << endl;
#endif
  return result;
}

vector<BDD> lhpnModelData::rateBDDVectorForSMT() {
  BDD pterm = getBDDMgr()->bddOne();
  for (vector<BDD>::iterator i = placeDisVariables.begin();
       i != placeDisVariables.end(); i++) {
    pterm *= *i;
  }
  for (vector<BDD>::iterator i = booleanDisVariables.begin();
       i != booleanDisVariables.end(); i++) {
    pterm *= *i;
  }
  for (vector<BDD>::iterator i = clockActiveDisVariables.begin();
       i != clockActiveDisVariables.end(); i++) {
    pterm *= *i;
  }
  //cout << "Non rate discrete variables: " <<  BDDToString(pterm) << endl;
  vector<BDD> result;
  for (vector<BDD>::iterator i = eachDisState.begin();
       i != eachDisState.end(); i++) {
    BDD rate = (*i).ExistAbstract(pterm);
    if (find(result.begin(), result.end(), rate) == result.end()) {
      result.push_back(rate);
    }
  }

#ifdef __SYM_PRINTRATEVECTOR__
  cout << "Each rate in rateBDDVectorForSMT" << endl;
  int count = 0;
  for (vector<BDD>::iterator i = result.begin(); i != result.end(); i++) {
    cout << count << ": " << BDDToString(*i) << endl;
    count++;
  }
  cout << "End of each rate." << endl;
#endif
  return result;
}

BDD lhpnModelData::rateBDD(int deltaVar) {
#ifdef __SYM_PRINTRATE__
  cSetFg(RED);
  cSetAttr(UNDERLINE);
  cout << "Building Rate Predicate for " << getVarName(deltaVar) << endl;
  cSetAttr(NONE);
#endif 
  BDD result = mgr->bddOne();
  set<string> doneRates;
  for (int t = 1; t < design->nevents; t++) {
    if (design->events[t]->dropped) {
      continue;
    }
    assert (!(design->events[t]->type & CONT));
    ineqADT i = design->events[t]->inequalities;
    // Construct rate implications for each rate assignment in the
    // inequalities list.
    while (i != NULL) {
      if (i->type == 6 && doneRates.find(rateToString(i)) == doneRates.end()) {
        doneRates.insert(rateToString(i));
        int varIndex = getCVarIndex(design->events[i->place]->event);
        // lower bound predicate
        BDD lbndpred;
        if (i->constant == 0) {
          lbndpred = predStruct::addPred(1.0, X0_I, 1.0, varIndex, 0);
        }
        else {
          lbndpred =  predStruct::addPred(i->constant, deltaVar, 1.0,
                                          varIndex, 0);
        }
        // Upper bound predicate
        BDD ubndpred;
        if (i->uconstant == 0) {
          ubndpred = predStruct::addPred(1.0, varIndex, 1.0, X0_I, 0);
        }
        else {
          ubndpred = predStruct::addPred(1.0, varIndex, i->uconstant,
                                          deltaVar, 0);
        }
        result *= (!getModelVarBDD(rateToString(i)) + (lbndpred * ubndpred));
#ifdef __SYM_PRINTRATE__
        cSetFg(RED);
        cout << rateToString(i) << " --> (" << predStruct::toString(lbndpred)
             << " * " << predStruct::toString(ubndpred) << ")" << endl;
        cSetAttr(NONE);
#endif
      }
      i = i->next;
    }
    // Assign clock variable for each transition a rate of one at all times.
    int cIndex = getCVarIndex(clockString(t));
    BDD lbndpred = predStruct::addPred(1, deltaVar, 1, cIndex, 0);
    BDD ubndpred = predStruct::addPred(1, cIndex, 1, deltaVar, 0);
    //result *= (!getModelVarBDD(clockActiveString(t)) + (lbndpred * ubndpred));
    result *= (lbndpred * ubndpred);
#ifdef __SYM_PRINTRATE__
    cSetFg(RED);
    cout << predStruct::toString(lbndpred)
         << " * " << predStruct::toString(ubndpred) << endl;
    //cout << clockActiveString(t) << " --> (" << predStruct::toString(lbndpred)
    //     << " * " << predStruct::toString(ubndpred) << ")" << endl;
    cSetAttr(NONE);
#endif
  }
  return result;
}

// This function performs an untimed discrete state space
// exploration.  It assumes that the discrete net is safe.  Algorithm
// is from "Petri Net Analysis Using Boolean Manipulation" by Pastor,
// Roig, Cortadella, and Badia.
BDD lhpnModelData::findDisStates() {
  if (this->allDisStatesBuilt) {
    return this->allDisStates;
  }
  vector<BDD> APM; // All predecessors marked (enabled)
  vector<BDD> NPM; // No predecessors marked
  vector<BDD> ASM; // All successors marked
  vector<BDD> NSM; // No successors marked
  BDD initState = mgr->bddOne();
  // Construct initial state
  // Initial discrete Marking.
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->events[p]->type & CONT || design->events[p]->dropped) {
      continue;
    }
    bool marked = false;
    for (int t = 1; t < design->nevents; t++) {
      if (design->rules[p][t]->epsilon == 1) {
        marked = true;
        break;
      }
    }
    if (marked) {
      initState *= getModelVarBDD(design->events[p]->event);
    }
    else {
      initState *= !getModelVarBDD(design->events[p]->event);
    }
  }
  // Initial Boolean Signals
  for (int s = 0; s < design->nsignals; s++) {
    if (strchr(design->signals[s]->name, '>') || 
	strchr(design->signals[s]->name, '<') || 
	strchr(design->signals[s]->name, '=')) continue;
    if (design->startstate[s] == '1') {
      initState *= getModelVarBDD(design->signals[s]->name);
    }
    else if (design->startstate[s] == '0') {
      initState *= !getModelVarBDD(design->signals[s]->name);
    }
  }
  // Initial Boolean Rates
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->events[p]->type & CONT) {
      // Hopefully the initial rate is one of the assigned rates also.
      // If not, will need to create the corresponding boolean
      // variable for the initial rate in the constructor.
      string ratevar = rateToString(design->events[p]->event,
                                    design->events[p]->linitrate,
                                    design->events[p]->uinitrate);
      initState *= getModelVarBDD(ratevar);
      //  Need to assign other bools affecting this rate to false
      set<string> rates = ratesForRealVar[design->events[p]->event];
      for (set<string>::iterator i = rates.begin(); i != rates.end(); i++) {
        if (*i != ratevar) {
          initState *= !getModelVarBDD(*i);
        }
      }
    }
  }
#ifdef  __SYM_PRINTDISSTATEEXP__
  cout << "In findDisStates: InitState:" << endl;  
  cout << BDDToString(initState) << endl;
#endif
  
  // Construct transition relation
  for (int t = 1; t < design->nevents; t++) {
    if (design->events[t]->type & CONT || design->events[t]->dropped) {
      continue;
    }
    BDD APMt = mgr->bddOne(); // all predecessors of t are marked
    BDD NPMt = mgr->bddOne(); // no predecessors of t is marked
    BDD ASMt = mgr->bddOne(); // all successors of t are marked
    BDD NSMt = mgr->bddOne(); // no sucessor of t is marked
    // Discrete Marking update
    for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
      if (design->events[p]->type & CONT || design->events[p]->dropped) { 
        continue;
      }
      if (design->rules[p][t]->ruletype != NORULE) {
        APMt *= getModelVarBDD(design->events[p]->event);
        NPMt *= !getModelVarBDD(design->events[p]->event);
      }
      if (design->rules[t][p]->ruletype != NORULE) {
        ASMt *= getModelVarBDD(design->events[p]->event);
        NSMt *= getModelVarBDD(design->events[p]->event);
      }
    }
    // Rate update
    for (ineqADT ineq=design->events[t]->inequalities; ineq != NULL;
         ineq = ineq->next) {
      if (ineq->type == 6) {
        string newrate = rateToString(ineq);
        ASMt *= getModelVarBDD(newrate);
        NSMt *= getModelVarBDD(newrate);
        //  Need to assign other bools affecting this rate to false
        set<string> rates=ratesForRealVar[design->events[ineq->place]->event];
	BDD temp = mgr->bddZero();
        for (set<string>::iterator i = rates.begin(); i != rates.end(); i++) {
          if (*i != newrate) {
            ASMt *= !getModelVarBDD(*i);
            NSMt *= getModelVarBDD(*i);
          }
        }
      }
      // Boolean assignments
      else if (ineq->type == 7) {
        BDD bassign = getModelVarBDD(design->signals[ineq->place]->name);
        if (ineq->constant == TRUE) {
          ASMt *= bassign;
          NSMt *= bassign;
        }
        else if (ineq->constant == FALSE) {
          ASMt *= !bassign;
          NSMt *= bassign;
        }
      }
    }
    
    BDD originalAPMt = APMt;
    BDD originalNPMt = NPMt;
    for (level_exp exp = design->events[t]->SOP; exp != NULL; exp=exp->next) {
      APMt = originalAPMt;
      NPMt = originalNPMt;      
      for (int s = 0; s < design->nsignals; s++) {
        if (strchr(design->signals[s]->name, '>') || 
            strchr(design->signals[s]->name, '<')) {
          // term is an inqeuality so ignore.
          continue;
        }
        BDD b = getModelVarBDD(design->signals[s]->name);
        if (exp->product[s] == '0') {
          APMt *= !b;
          NPMt *= !b;
        }
        else if (exp->product[s] == '1') {
          APMt *= b;
          NPMt *= b;
        }
      }
      APM.push_back(APMt);
      NPM.push_back(NPMt);
      ASM.push_back(ASMt);
      NSM.push_back(NSMt);
#ifdef  __SYM_PRINTDISSTATEEXP__
      cout << "APM: " << BDDToString(APMt) << endl;
      cout << "NPM: " << BDDToString(NPMt) << endl;
      cout << "ASM: " << BDDToString(ASMt) << endl;
      cout << "NSM: " << BDDToString(NSMt) << endl << endl;
#endif

    }
  }

  BDD reached = initState;
  BDD from = initState;
  BDD newStates = initState;
  do {
    BDD to = mgr->bddZero();
    for (unsigned int i = 0; i < APM.size(); i++) {
      to += (from.Cofactor(APM[i]) * NPM[i]).ExistAbstract(NSM[i]) * ASM[i];
    }
    newStates = to - reached;
    from = newStates;
    reached = reached + newStates;
  } while (newStates != mgr->bddZero());

  // Add the clock active Boolean variables into the dis states based on
  // the presets and the boolean portion of the enabling conditions.
  for (int t = 1; t < design->nevents; t++) {
    if (design->events[t]->dropped)  {
      continue;
    }
    BDD preset = presetBDD(t);
    BDD ca = getModelVarBDD(clockActiveString(t));
    BDD disjunct = getBDDMgr()->bddZero();
    bool hasCont = false;
    for (level_exp exp = design->events[t]->SOP; exp != NULL; exp=exp->next) {
      BDD term = getBDDMgr()->bddOne();
      for (int s = 0; s < design->nsignals; s++) {
	if (exp->product[s] != '0' && exp->product[s] != '1') {
	  continue;
	}
        if (strchr(design->signals[s]->name, '>') || 
            strchr(design->signals[s]->name, '<') ||
            strchr(design->signals[s]->name, '=')) {
          // term is an inequality so ignore.
	  hasCont = true;
          continue;
        }
        BDD b = getModelVarBDD(design->signals[s]->name);
        if (exp->product[s] == '0') {
          term *= !b;
        }
        else if (exp->product[s] == '1') {
          term *= b;
        }
      }
      disjunct += term;
    }
    reached *= (!ca + (preset*disjunct));
    if (!hasCont) {
      reached *= (!preset + !disjunct + ca);
    }
  }

  this->allDisStates = reached;
  this->allDisStatesBuilt = true;

#ifdef  __SYM_PRINTDISSTATEEXP__
  cout << "Found Discrete States: " << endl;
  cout << BDDToString(this->allDisStates) << endl;
#endif
  return this->allDisStates;
  // TODO: Test for safety.
}

// Generates a vector of BDDs where each element is a discrete state of the 
// LHPN.
vector<BDD> lhpnModelData::disStateVector() {
  BDD disStates = findDisStates();
  vector<BDD> allDisVariables;
  allDisVariables.insert(allDisVariables.end(), placeDisVariables.begin(),
                         placeDisVariables.end());
  allDisVariables.insert(allDisVariables.end(), booleanDisVariables.begin(),
                         booleanDisVariables.end());
  allDisVariables.insert(allDisVariables.end(),
                         clockActiveDisVariables.begin(),
                         clockActiveDisVariables.end());
  allDisVariables.insert(allDisVariables.end(), rateDisVariables.begin(),
                         rateDisVariables.end());
  BDDvector bddv(allDisVariables.size(), getBDDMgr());
  int pos = 0;
  for (vector<BDD>::iterator i = allDisVariables.begin();
       i != allDisVariables.end(); i++) {
    bddv[pos] = *i;
    pos += 1;
  }
  vector<BDD> result;
  while (disStates != getBDDMgr()->bddZero()) {
    BDD term = disStates.PickOneMinterm(bddv);
    disStates *= !term;
    result.push_back(term);
  }
#ifdef __SYM_PRINTDISSTATEVECTOR__
  cout << "Each discrete state in disStateVector" << endl;
  for (unsigned int i = 0; i < result.size(); i++) {
    cout << i << ": " <<  BDDToString(result[i]) << endl;
  }
  cout << "End of each discrete state." << endl;
#endif  
  return result;
}


BDD lhpnModelData::simplifyRestrict(BDD f1) {
  /*
  if (f1.nodeCount() < 100) {
    cout << "Not large enough... not simplifying" << endl;
    return f1;
  }
  */
  //cout << "Before simplifying. Size: " << f1.nodeCount() <<endl;
  BDD cons = mgr->bddOne();
  // X0
//   for (unsigned int i = 0;
//        i < ((lhpnModelData*) this)->eachDisState.size(); i++) {
//     BDD f1part = f1 * ((lhpnModelData*) this)->eachDisState[i];
//     cons = getTranCons(this, f1part, X0_I);
//     //    cout << "Before " << f1.nodeCount() << endl;
//     f1 = (f1).Restrict(cons);
//     //cout << "After " << f1.nodeCount() << endl;
//   }
  //f1 = restrictTranCons(this, f1, X0_I);
  cons = getTranCons(this, f1, X0_I);
  f1 = (f1).Restrict(cons);

  for (int i = 0; i < (int) allRealVars.size()*2; i+=2) {
    if (i == X0_I || i == N_DLT_I || i == N_EPS_I) {
      continue;
    }
    // Real Vars
//     for (unsigned int i = 0;
// 	 i < ((lhpnModelData*) this)->eachDisState.size(); i++) {
//       BDD f1part = f1 * ((lhpnModelData*) this)->eachDisState[i];
//       cons = getTranCons(this, f1part, i);
//       //      cout << "Before " << f1.nodeCount() << endl;
//       f1 = (f1).Restrict(cons);
//       //cout << "After " << f1.nodeCount() << endl;
//     }
    //f1 = restrictTranCons(this, f1, i);
    cons = getTranCons(this, f1, i);
    f1 = (f1).Restrict(cons);

    // C Variables
    /*
    cons = getTranCons(this, f1, i + design->nplaces);
    f1 = f1.Restrict(cons);
    */
  }
  //f1 = f1.Restrict(cons);
  return f1;
}

BDD lhpnModelData::finalsimplifyRestrict(BDD f1) {
  BDD cons = mgr->bddOne();
  cons *= getTranCons(this, f1, X0_I);
  //cout << "X0:cons" << endl;
  //printBDDState(cons);
  for (int i = 0; i < (int) allRealVars.size()*2; i+=2) {
    if (i == X0_I || i == N_DLT_I || i == N_EPS_I) {
      continue;
    }
    //cout << i << ":cons" << endl;
    //printBDDState(cons);
    cons *= getTranCons(this, f1, i);
  }
  f1 = f1.Restrict(cons);

  if (f1 == mgr->bddZero()) { 
    //cout << "EXIT" << endl; 
    return f1;
  }
  //cout << "Orig" << endl;
  //printBDDState(f1);
/*   f1 = addTranCons(this, f1, X0_I); */
/*   //cout << "X0:add" << endl; */
/*   //printBDDState(f1); */
/*   if (f1 == mgr->bddZero()) {  */
/*     //cout << "EXIT" << endl;  */
/*     return f1; */
/*   } */
/*   for (int i = 0; i < (int) allRealVars.size()*2; i+=2) { */
/*     if (i == X0_I || i == N_DLT_I || i == N_EPS_I) { */
/*       continue; */
/*     } */
/*     f1 = addTranCons(this, f1, i); */
/*     //cout << i << ":add" << endl; */
/*     //printBDDState(f1); */
/*     if (f1 == mgr->bddZero()) { */
/*       //cout << "EXIT" << endl;  */
/*       return f1; */
/*     } */
/*   } */
  f1 = addTranConsNew(f1);
  return f1;
}

BDD lhpnModelData::checkImplication(BDD f1,BDD f2) {
/*   BDD cons = mgr->bddOne(); */
/*   for (unsigned int i = 0; */
/*        i < ((lhpnModelData*) this)->eachDisState.size(); i++) { */
/*     BDD part = (f1+f2) * ((lhpnModelData*) this)->eachDisState[i]; */
/*     cons = getTranCons(this, part, X0_I); */
/*     //cons *= getTranCons(this, part, X0_I); */
/*     f1 *= cons; */
/*     f2 *= cons; */
/*   } */
/*   for (int i = 0; i < (int) allRealVars.size()*2; i+=2) { */
/*     if (i == X0_I || i == N_DLT_I || i == N_EPS_I) { */
/*       continue; */
/*     } */
/*     // Real Vars */
/*     for (unsigned int i = 0; */
/* 	 i < ((lhpnModelData*) this)->eachDisState.size(); i++) { */
/*       BDD part = (f1+f2) * ((lhpnModelData*) this)->eachDisState[i]; */
/*       cons = getTranCons(this, part, i); */
/*       //cons *= getTranCons(this, part, i); */
/*       f1 *= cons; */
/*       f2 *= cons; */
/*     } */
/*   } */
  return (f1 * !f2);
}

// BDD lhpnModelData::checkImplication(BDD f1,BDD f2) {
//   BDD cons = mgr->bddOne();
// //   for (unsigned int i = 0;
// //        i < ((lhpnModelData*) this)->eachDisState.size(); i++) {
// //     BDD part = (f1+f2) * ((lhpnModelData*) this)->eachDisState[i];
// //     cons = getTranCons(this, part, X0_I);
// //     //cons *= getTranCons(this, part, X0_I);
// //     f1 *= cons;
// //     f2 *= cons;
// //   }
//   BDD invariant = buildInvariant();
//   f2 = !f2*invariant;
//   cons = getTranCons(this, f1+f2, X0_I);
//   f1 *= cons;
//   f2 *= cons;
// //   for (int i = 0; i < (int) allRealVars.size()*2; i+=2) {
// //     if (i == X0_I || i == N_DLT_I || i == N_EPS_I) {
// //       continue;
// //     }
// //     // Real Vars
// //     for (unsigned int i = 0;
// // 	 i < ((lhpnModelData*) this)->eachDisState.size(); i++) {
// //       BDD part = (f1+f2) * ((lhpnModelData*) this)->eachDisState[i];
// //       cons = getTranCons(this, part, i);
// //       //cons *= getTranCons(this, part, i);
// //       f1 *= cons;
// //       f2 *= cons;  
// //     }  
// //   }
//   for (int i = 0; i < (int) allRealVars.size()*2; i+=2) {
//     if (i == X0_I || i == N_DLT_I || i == N_EPS_I) {
//       continue;
//     }
//     // Real Vars
//     cons = getTranCons(this, f1+f2, i);
//     f1 *= cons;
//     f2 *= cons;  
//   }
// //   cout << "f1" << endl;
// //   printBDDState(f1);
// //   cout << "f2" << endl;
// //   printBDDState(f2);
//   return (f1 * f2);
// }
//////////////////////////////////////////////////////////////////////////////


void symbolic(designADT design) {
#ifdef MEMSTATS
#ifndef OSX
  memuse=mallinfo();
  printf("memory: max=%d inuse=%d free=%d \n",
         memuse.arena,memuse.uordblks,memuse.fordblks);
  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
          memuse.arena,memuse.uordblks,memuse.fordblks);
#else
	malloc_zone_statistics(NULL, &t);
	printf("memory: max=%d inuse=%d allocated=%d\n",
	       t.max_size_in_use,t.size_in_use,t.size_allocated);
	fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
  
  if (!(design->status & LOADED)) {
    cout << "Cannot perform symbolic analysis.  Design has not been loaded."
         << endl;
    fprintf(lg, "Cannot perform symbolic analysis.  Design has not been loaded.\n");
    return;
  }
  if (design->fromLPN != true) {
    cout << "Cannot perform symbolic analysis.  Design must be an LPN."
         << endl;
    fprintf(lg, "Cannot perform symbolic analysis.  Design must be an LPN.\n");
    return;
  }
  
  Cudd mgr(0,0);
  //mgr.makeVerbose();

  lhpnModelData* md = new lhpnModelData(&mgr, design);
  transvector transRels = md->buildTransRels();
#ifdef __SYM_PRINTFINALTRANSRELS__
  cout << "Final set of transition relations: " << endl;
  int count = 0;
  for (transvector::iterator i = transRels.begin(); i != transRels.end(); i++) {
    cout << "Transition relation " << count << endl;
    count++;
    ((lhpnModelData*) md)->printTransRel(*i);
    cout << "----------------------------------------------------" << endl;
  }
#endif 

  BDD invariant = md->buildInvariant();
  //#ifdef __SYM_INVARIANT__
  //cout << "Invariant: " << endl;
  //md->printBDDState(invariant);
  //#endif

  BDD iState = md->initialState();
  //#ifdef __SYM_INITSTATE__  
  //cout << "Initial State: " << endl;
  //md->printBDDState(iState);
  //#endif

  // 5/15/06 - Testing
  //BDD phi = iState * invariant;
  //cout << "istate*invariant: " << endl;
  //md->printBDDState(phi);
  //phi = pre(md, phi, invariant, transRels);
  //cout << "After call to pre:" << endl;
  //md->printBDDState(phi);
  //phi = prenew(md, phi, invariant, transRels);
  //cout << "After second call to prenew:" << endl;
  //md->printBDDState(phi);
  //exit(1);

  // If no test cases, just proceed as normal:
  tmustmt* mu = loadProperty(md, design);
  if (mu == NULL) {
    cleanup(md, transRels, mu);
    cout << "The TCTL property could not be parsed. Terminating analysis."
         << endl;
    fprintf(lg, "The TCTL property could not be parsed. Terminating analysis.\n");
    return;
  }
  else {
    cout << "The TCTL property is equivalent to this Tu property: " << endl;
    cout << tmustmtToString(mu, md) << endl;
    fprintf(lg, "The TCTL property is equivalent to this Tu property: \n");
    fprintf(lg, "%s\n", tmustmtToString(mu, md).c_str());
    //cout << "Press <enter> to continue";
    //char junk;
    //cin.get(junk);
  }
  cout << "Performing LHPN BDD verification." << endl;
  fprintf(lg, "Performing LHPN BDD verification.\n");

  bool terminatedEarly = false;
  BDD result = recCheck(md, invariant, transRels, mu, terminatedEarly);
                
  if (terminatedEarly) {
    cout << "Terminated early because a found state implied initial state. " << endl;
    fprintf(lg, "Terminated early because a found state implied initial state.\n");
    cout << "The property is not satisfied." << endl;
    fprintf(lg, "The property is not satisfied.\n");
    if (design->verbose) {
      cout << "Result of LHPN BDD verification:" << endl;
      fprintf(lg, "Result of LHPN BDD verification:\n");
      cout << md->BDDToString(result) << endl;
      fprintf(lg, "%s", md->BDDToString(result).c_str());
    }
  }
  else {
    //cout << "Initial Result:" << endl;
    //md->printBDDState(result);

    //result=(invariant * !(invariant * iState)) + result;
    result=md->checkImplication(iState,result);
    result=md->finalsimplifyRestrict(result);
  
    if (result == mgr.bddZero()) {
      cout << "The property is satisfied." << endl;
      fprintf(lg, "The property is satisfied.\n");
    }
    else {
      cout << "The property is not satisfied." << endl;
      fprintf(lg, "The property is not satisfied.\n");
      if (design->verbose) {
	cout << "Result of LHPN BDD verification:" << endl;
	fprintf(lg, "Result of LHPN BDD verification:\n");
	cout << md->BDDToString(result) << endl;
	fprintf(lg, "%s", md->BDDToString(result).c_str());
      }
    }
  }
  cleanup(md, transRels, mu);
#ifdef MEMSTATS
#ifndef OSX
  memuse=mallinfo();
  printf("memory: max=%d inuse=%d free=%d \n",
         memuse.arena,memuse.uordblks,memuse.fordblks);
  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
          memuse.arena,memuse.uordblks,memuse.fordblks);
#else
  malloc_zone_statistics(NULL, &t);
  printf("memory: max=%d inuse=%d allocated=%d\n",
	 t.max_size_in_use,t.size_in_use,t.size_allocated);
  fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
	  t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif

}

tmustmt* loadProperty(modelData* md, designADT design) {
  if (design->properties == NULL) {
    cout << "A property was not specified in the g file.\n";
    cout << "Enter a TCTL property to verify: ";
    cin.getline(tctlprop, MAXTOKEN);    
  }
  else {
    // TODO: design->properties is actually a linked list of
    // properties.  I currently only handle the first one.
    proplistADT prop = design->properties;
    if (prop->next != NULL) {
      cout << "Select a property to verify: " << endl;
      int count = 1;
      while (prop != NULL) {
        cout << "  " << count << ": " << prop->property << endl;
        prop = prop->next;
        count++;
      }
      int choice = 0;
      while (choice <= 0 || choice >= count) {
        cout << "Which property would you like to verify? ";
        cin >> choice;
        cin.ignore(100, '\n');
      }
      count = 1;
      prop = design->properties;
      while (count < choice) {
        prop = prop->next;
        count++;
      }
    }
    strcpy(tctlprop, prop->property);
  }
  cout << "Verifying TCTL property: " << tctlprop << endl;
  fprintf(lg, "Verifying TCTL property: %s\n", tctlprop);
  tctlpropptr = tctlprop;
  tctlproplim = (intptr_t) tctlpropptr+strlen(tctlprop);

  if (tctlparse((void*) md)) {
    return NULL;
  }
  return tmuroot;
}

void cleanup(modelData* md, transvector &transRels, tmustmt* mu) {
  delete_tmustmt(mu);
  for (unsigned int i = 0; i < transRels.size(); i++) {
    delete transRels[i];
    transRels[i] = NULL;
  }
  delete md;
}

BDD recCheck (modelData* md, BDD &invariant,
              transvector transRels, tmustmt* mu, bool &terminatedEarly) {
  int iteration = 0; // counter of # of iterations in lfp calculation
  BDD tmu1, tmu2, pt, te, phinew, phiold;
  BDD result, imp;
  vector<BDD> trace;
  switch (mu->op) {
  case TM_SL:
#ifdef __SYM_PRINTSTEPS__
    cout << "INV AND " << tmustmtToString(mu, md) << endl;
#endif
    result =  invariant * mu->sl;
    break;
  case TM_NOT:
#ifdef __SYM_PRINTSTEPS__
    cout << "NOT " << tmustmtToString(mu->tmu1, md) << endl;
#endif
    result = recCheck(md, invariant, transRels, mu->tmu1, terminatedEarly);
    if (terminatedEarly) {
      return result;
    }
    result = invariant & !result;
    break;
  case TM_OR:
#ifdef __SYM_PRINTSTEPS__
    cout << tmustmtToString(mu->tmu1, md) << " OR "
	 << tmustmtToString(mu->tmu2, md) << endl;
#endif
    tmu1 = recCheck(md, invariant, transRels, mu->tmu1, terminatedEarly);
    if (terminatedEarly) {
      return tmu1;
    }
    tmu2 = recCheck(md, invariant, transRels, mu->tmu2, terminatedEarly);
    if (terminatedEarly) {
      return tmu2;
    }
    result = tmu1 + tmu2;
    break;
  case TM_AND:
#ifdef __SYM_PRINTSTEPS__
    cout << tmustmtToString(mu->tmu1, md) << " AND "
	 << tmustmtToString(mu->tmu2, md) << endl;
#endif
    tmu1 = recCheck(md, invariant, transRels, mu->tmu1, terminatedEarly);
    if (terminatedEarly) {
      return tmu1;
    }
    tmu2 = recCheck(md, invariant, transRels, mu->tmu2, terminatedEarly);
    if (terminatedEarly) {
      return tmu2;
    }
    result = tmu1 * tmu2;
    break;
  case TM_NS:
#ifdef __SYM_PRINTSTEPS__
    cout << tmustmtToString(mu->tmu1, md) << " |> "
	 << tmustmtToString(mu->tmu2, md) << endl;
#endif
    tmu1 = recCheck(md, invariant, transRels, mu->tmu1, terminatedEarly);
    if (terminatedEarly) {
      return tmu1;
    }
    tmu2 = recCheck(md, invariant, transRels, mu->tmu2, terminatedEarly);
    if (terminatedEarly) {
      return tmu2;
    }

    //cout << "Before pre " << endl;
    //md->printBDDState(tmu2);

    pt = pre(md, tmu2, invariant, transRels);

    //cout << "After pre " << endl;
    //md->printBDDState(pt);
    //pt=((hpnModelData*)md)->simplifyRestrict(pt);

    te = md->timeElapse(tmu1+tmu2, pt);
    result = invariant * te;
    //cout << "After time elapse " << endl;
    //md->printBDDState(result);
    break;
  case TM_SC:
#ifdef __SYM_PRINTSTEPS__
    cout << md->getVarName(mu->z) << "." 
	 << tmustmtToString(mu->tmu1, md) << endl;
#endif
    tmu1 = recCheck(md, invariant, transRels, mu->tmu1, terminatedEarly);
    if (terminatedEarly) {
      return tmu1;
    }
    result = specifyClock(md, mu->z, tmu1);
    break;
  case TM_LFP:
#ifdef __SYM_PRINTSTEPS__
    cout << mu->X << "." << tmustmtToString(mu->tmu1, md) << endl;
#endif
    phinew = md->getBDDMgr()->bddZero();
    do {
      ostringstream desc;
      desc << "[" << mu->X << " iter " << iteration << "]";
      phiold = phinew;
      substitute(mu->tmu1, mu->X, phiold, desc.str());
      phinew = recCheck(md, invariant, transRels, mu->tmu1, terminatedEarly);
      if (terminatedEarly) {
	if (((lhpnModelData*) md)->design->verbose) {
	  printTrace(md, trace, invariant, transRels);
	}
	return phinew;
      }
      unsubstitute(mu->tmu1, mu->X);
      //phinew = ((lhpnModelData*)md)->addTranConsNew(phinew);
      //cout << "Before calling cleanup:" << endl;
      //((lhpnModelData*) md)->countBDDState(phinew);
      //cout << "Node Count: " << phinew.nodeCount() << endl;
      phinew = ((lhpnModelData*)md)->simplifyRestrict(phinew);
      if (((lhpnModelData*) md)->design->verbose) {
	trace.push_back(phinew-phiold);
      }
      //phinew = ((lhpnModelData*)md)->cleanupbasedonrate(phinew);
      //cout << "  After calling cleanup then simplifyrestrict:" << endl;
      //((lhpnModelData*) md)->countBDDState(phinew);
      //cout << "  Node Count: " << phinew.nodeCount() << endl;
#ifdef __SYM_FIXPOINTITERS__
      cout<< "New additions after fixpoint iteration " << iteration << ":"
          <<endl;
      cout << "Number of preds: " << predStruct::getPredCount() << endl;
      ((lhpnModelData*) md)->printBDDState(phinew-phiold);
#endif
      //cout << "Number of primes" << endl;
      //((lhpnModelData*) md)->countBDDState(phinew);
      //cout << "Number of new primes" << endl;
      //((lhpnModelData*) md)->countBDDState(phinew-phiold);
      cout << "Iteration " << iteration << endl;
      if (((lhpnModelData*) md)->design->verbose) {
	fprintf(lg, "Iteration %i\n", iteration);
      }
      iteration++;
      /* TODO: IS THIS OKAY? */
      //imp = phinew-phiold;
      /* OR SHOULD USE THE NEXT TWO LINES? */
      imp = ((lhpnModelData*)md)->checkImplication(phinew,phiold);
      imp = ((lhpnModelData*)md)->simplifyRestrict(imp);
      //cout << "Implication is" << endl;
      //md->printBDDState(imp);

      //cout << "Implication size: " << imp.nodeCount() << endl;

      // Check if the initial state implies phinew.  If it does,
      // terminate.
      BDD result=((lhpnModelData*)md)->checkImplication(((lhpnModelData*)md)->myInitialState,!phinew);
      result=((lhpnModelData*)md)->finalsimplifyRestrict(result);

      if (result != md->getBDDMgr()->bddZero()) {
	terminatedEarly = true;
	if (((lhpnModelData*) md)->design->verbose) {
	  printTrace(md, trace, invariant, transRels);
	}
	return result;
      }
    } while(imp != md->getBDDMgr()->bddZero());
    result = phiold;
    //cout << "Returning ... " << endl;
    //md->printBDDState(phiold);
    break;
 default:
   cout << "FATAL ERROR: Unexpected timed mu calculus operation encountered."
        << endl;
   cout << mu->op << " " << mu->X << " " << mu->description << endl;
   result = md->getBDDMgr()->bddZero();
   assert(false);
   break;
  }
  return result;
}


void printTrace(modelData* md, vector<BDD> trace,
		BDD invariant, transvector transRels) {
  cout << "Attempting to generate trace. " << endl;
  lhpnModelData* lmd = (lhpnModelData*) md;
  Cudd* mgr = lmd->mgr;
  BDD initState = lmd->initialState();

  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(mgr->getManager(), 
                             trace.back().getNode(), 
			     trace.back().getNode() ,&cube);
  assert(g);
  BDD p;
  bool found = false;
  do {
    p = mgr->bddOne();
    for (int i = 0; i < mgr->ReadSize(); i++) {
      if (cube[i] == 0) {
	p *= !mgr->bddVar(i);
      }
      else if (cube[i] == 1) {
	p *= mgr->bddVar(i);
      }
    }
    BDD result = lmd->checkImplication(initState, !p);
    result = lmd->finalsimplifyRestrict(result);
    if (result != mgr->bddZero()) {
      cout << "Beginning from: " << endl;
      lmd->printBDDState(p);
      cout << endl;
      found = true;
      break;
    }
  } while (Cudd_NextPrime(g, &cube));
  //  GenFree(g);
  if (!found) {
    cout << "PrintTrace: No product was found to be a subset of the initial state." << endl;
    return;
  }

  for (int t = trace.size()-2; t >= 0; t--) {
    g = Cudd_FirstPrime(mgr->getManager(),
			trace[t].getNode(), trace[t].getNode(), &cube);
    bool done = false;
    do {
      BDD pp = mgr->bddOne();
      for (int i = 0; i < mgr->ReadSize(); i++) {
	if (cube[i] == 0) {
	  pp *= !mgr->bddVar(i);
	}
	else if (cube[i] == 1) {
	  pp *= mgr->bddVar(i);
	}
      }
      //cout << "transRels size: " << transRels.size() << endl;
      //for (unsigned int j = 0; j < transRels.size(); j++) {
      //cout << j%10 << flush;
	BDD ppp =  lmd->timeElapse(invariant, 
				   pre(lmd, pp, invariant, transRels));
	//lmd->printBDDState(ppp);
	BDD result = lmd->checkImplication(p, !ppp);
	result = lmd->finalsimplifyRestrict(result);
	if (result != mgr->bddZero()) {
	//if ((p * !ppp) == mgr->bddZero())
	  // && 
	  //  lmd->checkImplication(p, ppp) == mgr->bddZero()) {
	  //cout << "Fired Transition: " << endl;
	  //	  lmd->printTransRel(transRels[j]);
	  //cout << endl;

	  cout << "Resulting in: " << endl;
	  lmd->printBDDState(pp);
	  p = pp;
	  done = true;
	  break;
	}
	//}
    //cout << endl;
    //if (done) { 
    //	break; 
    //}
    } while(Cudd_NextPrime(g, &cube));
    //    GenFree(g);
    if (!done) {
      cout << "printTrace: could not find product term" << endl;
      return;
    }
  }
}

void substitute(tmustmt* mu, string X, BDD phi, string description) {
  switch(mu->op) {
  case TM_VAR:
    if (mu->X == X) {
      mu->op = TM_SL;
      mu->sl = phi;
      mu->description = description;
    }
    break;
  case TM_OR:
  case TM_AND:
  case TM_NS:
    substitute(mu->tmu1, X, phi, description);
    substitute(mu->tmu2, X, phi, description);
    break;
  case TM_NOT:
  case TM_SC:
  case TM_LFP:
    substitute(mu->tmu1, X, phi, description);
    break;
  case TM_SL:
    break;
  }
}

void unsubstitute(tmustmt* mu, string X) {
  switch(mu->op) {
  case TM_VAR:
    break;
  case TM_OR:
  case TM_AND:
  case TM_NS:
    unsubstitute(mu->tmu1, X);
    unsubstitute(mu->tmu2, X);
    break;
  case TM_NOT:
  case TM_SC:
  case TM_LFP:
    unsubstitute(mu->tmu1, X);
    break;
  case TM_SL:
    if (mu->X == X) {
      mu->op = TM_VAR;
      mu->description = "";
    }
    break;
  }    
}

BDD abstractVarTense(BDD var, BDD phi) {
  BDD posCofactor = phi.Cofactor(var);
  BDD negCofactor = phi.Cofactor(!var);
  return posCofactor + (!var * !posCofactor * negCofactor);
}


// prenew and pretnew implement the two level transition relation 
// method.
BDD prenew(modelData* md, BDD phi, BDD &invariant, transvector transRels) {
  // Special vector of only secondary transition relations.
  transvector secondary;
  for (transvector::iterator i = transRels.begin();
       i != transRels.end(); i++) {
    if ((*i)->type == SECONDARY) {
      secondary.push_back(*i);
    }
  }
  BDD initresult = md->getBDDMgr()->bddZero();
  for (transvector::iterator i = secondary.begin();
       i != secondary.end(); i++) {
    cout << "Applying secondary transition relation: " << endl;
    ((lhpnModelData*) md)->printTransRel(*i);
    BDD part = (*i)->guard * applyAssignments(md, phi, *i);
    md->printBDDState(part);
    initresult += (part);
  }
  cout << "secondary:" << endl;
  md->printBDDState(initresult);
  BDD result = phi + initresult;
  for (transvector::iterator i = transRels.begin();
       i != transRels.end(); i++) {
    cout << "Applying primary transition relation: " << endl;
    ((lhpnModelData*) md)->printTransRel(*i);
    BDD part = (*i)->guard*applyAssignments(md, initresult, *i);
    md->printBDDState(part);
    result += (part);
  }
  cout << "Result before anding invariant:" << endl;
  md->printBDDState(result);
  result *= invariant;
  return result;
}

BDD applyAssignments(modelData* md, BDD phi, transStruct* trans) {
  // Apply Boolean assignments
  for (vector<BDD>::iterator i = trans->boolAssign.begin(); 
       i != trans->boolAssign.end(); i++) {
    phi = phi.Cofactor(*i);
  }
  
  // Apply Real assignments by iterating through all the BDD variables
  // in the support that are predicates and performing substitutions
  // as necessary according to the assignment list.
  vector<int> support = getSupport(md->getBDDMgr(), phi);
  for (vector<int>::iterator i = support.begin(); i != support.end(); i++) {
    predStruct *ps = predStruct::getPred(*i);
    // Beginning of new substitution method.
    if (ps == NULL) continue;
    if (trans->clockAssign.find(ps->j) != trans->clockAssign.end() &&
        trans->clockAssign.find(ps->i) != trans->clockAssign.end()) {
      double lower = trans->lowerClock[ps->j];
      double upper = trans->upperClock[ps->i];
      // TODO: Need to properly implement handling where the lower
      // and upper bounds do not match.
      assert(lower == upper);
      BDD subBDD = predStruct::addPred(X0_I, X0_I,
                                       ps->c - ps->ci*upper + ps->cj*lower);
      phi = phi.Compose(subBDD, ps->var);
  #ifdef __SYM_PRINTSUBS__
       cout << "pret5: Substituting predicate " << ps->toString() << endl;
  #endif
    }
    else if (trans->clockAssign.find(ps->i) != trans->clockAssign.end()) {
      double upper = trans->upperClock[ps->i];
      BDD subBDD = predStruct::addPred(1, X0_I,
                                       ps->cj, ps->j, ps->c - ps->ci*upper);
      phi = phi.Compose(subBDD, ps->var);
  #ifdef __SYM_PRINTSUBS__
      cout << "pret3: Substituting predicate " << ps->toString() << endl;
  #endif
    }
    else if (trans->clockAssign.find(ps->j) != trans->clockAssign.end()) {
      double lower = trans->lowerClock[ps->j];
      BDD subBDD = predStruct::addPred(ps->ci, ps->i, 1,
                                       X0_I, ps->c + ps->cj*lower);
      phi = phi.Compose(subBDD, ps->var);
  #ifdef __SYM_PRINTSUBS__
      cout << "pret4: Substituting predicate " << ps->toString() << endl; 
  #endif
    }
  }
  return phi;
}
 
BDD pre(modelData* md, BDD phi, BDD &invariant, transvector transRels) {
  BDD result = phi;
  //BDD oldresult = md->getBDDMgr()->bddZero();
  //cout << "BEFORE:" << endl;
  //md->printBDDState(result);
  for (unsigned int i = 0; i < transRels.size(); i++) {
    //cout << "i=" << i << endl;
    transStruct* trans = transRels[i];
    BDD pretR = pret(md, invariant*phi, trans);
    //cout << "pretR:" << endl;
    //md->printBDDState(pretR);
    result += pretR;
    //cout << "result:" << endl;
    //md->printBDDState((result-oldresult)*invariant);
    //oldresult=result;
  }
  result *= invariant;
  //cout << "AFTER:" << endl;
  //md->printBDDState(result);
  return result;
}

BDD pret(modelData* md, BDD phi, transStruct* trans) {
  for (unsigned int i = 0; i < trans->boolAssign.size(); i++) {
    phi = phi.Cofactor(trans->boolAssign[i]);
  }
  // Add transitivity constraints around each real variable that is
  // going to be assigned a value.
#ifdef __SYM_UNOPTIMIZETRANSCONS__
  for (set<int>::iterator i = trans->clockAssign.begin();
       i != trans->clockAssign.end(); i++) {
    //#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
    //    cout << "Pret: Adding transitivity constraints for "
    //         << md->getVarName(*i) << endl;
    //#endif
     phi = addTranCons(md, phi, *i);
  }
#endif
  
  set<int> clockAssign;
  set<int> infinAssign;
  // special case for [-infin, infin] assignments - Existentially
  // abstract all inequalities that contain a real variable being
  // assigned [-infin, infin].  
  for (set<int>::iterator i = trans->clockAssign.begin();
       i != trans->clockAssign.end(); i++) {
    if (trans->lowerClock[*i] ==  -numeric_limits<double>::infinity() &&
        trans->upperClock[*i] ==  numeric_limits<double>::infinity()) {
      infinAssign.insert(*i);
    }
    else {
      // currently don't handle assignment of ranges of rates unless
      // [-infin, infin]
      assert(trans->lowerClock[*i] == trans->upperClock[*i]);
      clockAssign.insert(*i);
    }
  }
  // Iterate through all current bdd variables in support of phi and
  // replace the clocks such that 
  vector<int> support = getSupport(md->getBDDMgr(), phi);  
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    if (ps == NULL) continue;
    if (infinAssign.find(ps->i) != infinAssign.end() ||
        infinAssign.find(ps->j) != infinAssign.end()) {
      phi = phi.ExistAbstract(ps->bdd());
    }
    else {
      if (clockAssign.find(ps->j) != clockAssign.end() &&
          clockAssign.find(ps->i) != clockAssign.end()) {
        double lower = trans->lowerClock[ps->j];
        double upper = trans->upperClock[ps->i];
        BDD subBDD = predStruct::addPred(X0_I, X0_I,
                                         ps->c - ps->ci*upper + ps->cj*lower);
        phi = phi.Compose(subBDD, ps->var);
#ifdef __SYM_PRINTSUBS__
        cout << "pret5: Substituting predicate " << ps->toString() << endl;
#endif
      }
      else if (clockAssign.find(ps->i) != clockAssign.end()) {
        double upper = trans->upperClock[ps->i];
        BDD subBDD = predStruct::addPred(1, X0_I,
                                         ps->cj, ps->j, ps->c - ps->ci*upper);
        phi = phi.Compose(subBDD, ps->var);
#ifdef __SYM_PRINTSUBS__
        cout << "pret3: Substituting predicate " << ps->toString() << endl;
#endif
      }
      else if (clockAssign.find(ps->j) != clockAssign.end()) {
        double lower = trans->lowerClock[ps->j];
        BDD subBDD = predStruct::addPred(ps->ci, ps->i, 1,
                                         X0_I, ps->c + ps->cj*lower);
        phi = phi.Compose(subBDD, ps->var);
#ifdef __SYM_PRINTSUBS__
        cout << "pret4: Substituting predicate " << ps->toString() << endl; 
#endif
      }
    } 
  }
  
  phi = phi * trans->guard;
  // Add all transitivity constraints after performing conjunction.
  // X0
#ifdef __SYM_GUARDCONS__
  //  phi = ((lhpnModelData*) md)->finalsimplifyRestrict(phi);
  phi = ((lhpnModelData*) md)->simplifyRestrict(phi);
  phi = ((lhpnModelData*) md)->addTranConsNew2(phi,trans->guard);
  phi = ((lhpnModelData*) md)->simplifyRestrict(phi);
#endif
#ifdef __SYM_UNOPTIMIZETRANSCONS__
  phi = addTranCons(md, phi, X0_I);
  for (int i = 6;
       i < (int) ((lhpnModelData*) md)->allRealVars.size() * 2; i+=2) {
    phi = addTranCons(md, phi, i);
  }
#endif

  return phi;
}

// Used for optimization discussed in section 5.1 of Sanjit's paper.
// Returns true of the bounds are not conjoined and false otherwise.
bool bndsNotConjoined(modelData* md, BDD phi, BDD v1, BDD v2) {
#ifdef __SYM_USEOPT1__
  return ((v1 * v2 *
           phi.Cofactor(v1) * !phi.Cofactor(!v1) *
           phi.Cofactor(v2) * !phi.Cofactor(!v2)) !=
          md->getBDDMgr()->bddZero());
#else
  return true;
#endif
}

// Used for optimization discussed in section 5.2 of Sanjit's paper.
// An iterative method for quantifier elimination by elimination upper
// bounds on x_0 to avoid introduction of N_EPS_I and abstraction over
// variables containing N_EPS_I.
BDD quantElimination(modelData* md, BDD phi) {
  vector<int> fullSupport = getSupport(md->getBDDMgr(), phi);
  vector<int> support;
  for (vector<int>::iterator i = fullSupport.begin();
       i != fullSupport.end(); i++) {
    if (predStruct::getPred(*i) != NULL && predStruct::getPred(*i)->j == X0_I) {
      support.push_back(*i);
    }
  }
  for (vector<int>::iterator j = support.begin(); j != support.end(); j++) {
    BDD phibool = phi;
    BDD phicons = getTranCons(md, phibool, X0_I);
    phicons *= getTranCons(md, phibool, predStruct::getPred(*j)->i);
    phi = ((phibool * phicons).
           Cofactor(md->getBDDMgr()->bddVar(predStruct::getPred(*j)->var))) +
      (!md->getBDDMgr()->bddVar(predStruct::getPred(*j)->var) *
       phibool.Cofactor(md->getBDDMgr()->bddVar(predStruct::getPred(*j)->var)));
  }
  return phi;
}

// This function will place further restrictions on inequalities of
// the form c_i x_i >= c_j x_j + c_i where x_i and x_j are real
// variables from the model.  This prevents those relationships from
// overconstraining the state space when time elapse is performed.
BDD unconstrain(modelData* md, BDD f) {
  vector<int> support = getSupport(md->getBDDMgr(), f);
  if (support.size() == 0)
    return f;
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct* pi = predStruct::getPred(support[i]);
    if (pi == NULL) continue;
    if (pi->i < md->design->nplaces && pi->j < md->design->nplaces) {
      BDD b = predStruct::addPred(pi->ci, pi->i+md->design->nplaces,
                                  pi->cj, pi->j+md->design->nplaces, 0);
      f = f + !b * f.Cofactor(pi->bdd()) + b * f.Cofactor(!(pi->bdd()));
      //cout << "Adding " << predStruct::getPred(b.NodeReadIndex())->toString()
      //     << " for " << pi->toString() << endl;
    }
  }
  return f;
}


BDD getTranCons(modelData* md, BDD phi, int clockVar) {
  // Iterate through all pairs of predicates and add transitivity
  // constraints as necessary.
  vector<int> support = getSupportWithVariable(md->getBDDMgr(), phi, clockVar);
  if (support.size() == 0)
    return md->getBDDMgr()->bddOne();
#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
  cout << "Getting transitivity constraints for "
       << md->getVarName(clockVar) << endl;
  cout << "Support size: " << support.size() << endl;
  //md->printBDDState(phi);
#endif
  BDD cons = md->getBDDMgr()->bddOne();
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct* pi = predStruct::getPred(support[i]);
    for (unsigned int j = 0; j < support.size(); j++) {
      //printf("i=%d j=%d size=%d\n",i,j,support.size());
      predStruct *pj = predStruct::getPred(support[j]);
      if (pi == pj) continue;
      cons *= predStruct::implies(pi, pj);
      //cout << "imp cons size " << cons.nodeCount() << endl;
      cons *= predStruct::trans(pi, pj, clockVar, false, &phi);
      //cout << "trans cons size " << cons.nodeCount() << endl;
    }
  }
#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
  cout << "cons size " << cons.nodeCount() << endl;
  cout << "Done getting transitivity constraints for "
       << md->getVarName(clockVar) << endl;
#endif
  return cons;
}

BDD restrictTranCons(modelData* md, BDD phi, int clockVar) {
  // Iterate through all pairs of predicates and add transitivity
  // constraints as necessary.
  BDD result = phi;
  vector<int> support = getSupportWithVariable(md->getBDDMgr(), phi, clockVar);
  if (support.size() == 0)
    return result;
#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
  cout << "Getting transitivity constraints for "
       << md->getVarName(clockVar) << endl;
  cout << "Support size: " << support.size() << endl;
  //md->printBDDState(phi);
#endif
  BDD cons = md->getBDDMgr()->bddOne();
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct* pi = predStruct::getPred(support[i]);
    for (unsigned int j = 0; j < support.size(); j++) {
      //printf("i=%d j=%d size=%d\n",i,j,support.size());
      predStruct *pj = predStruct::getPred(support[j]);
      if (pi == pj) continue;
      cons = predStruct::implies(pi, pj);
      result = (result).Restrict(cons);
      cons = predStruct::trans(pi, pj, clockVar, false, &phi);
      result = (result).Restrict(cons);
    }
  }
#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
  cout << "cons size " << cons.nodeCount() << endl;
  cout << "Done getting transitivity constraints for "
       << md->getVarName(clockVar) << endl;
#endif
  return result;
}

BDD addTranCons(modelData* md, BDD phi, int clockVar) {
  // Iterate through each discrete marking and generate transitivity
  // constraints for that portion of phi.
  BDD result = md->getBDDMgr()->bddZero();
  for (unsigned int i = 0;
       i < ((lhpnModelData*) md)->eachDisState.size(); i++) {
    BDD phipart = phi * ((lhpnModelData*) md)->eachDisState[i];
    vector<int> support = getSupportWithVariable(md->getBDDMgr(),
                                                 phipart, clockVar);
    if (support.size() == 0) {
      result += phipart;
      continue;
    }
#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
    cout << "Adding transitivity constraints for "
         << md->getVarName(clockVar) << endl;
    cout << "Support size: " << support.size() << endl;
    //md->printBDDState(phi);
#endif
    BDD partresult = phipart;
    // Iterate through all pairs of predicates and add transitivity
    // constraints as necessary.
    for (unsigned int i = 0; i < support.size(); i++) {
      predStruct* pi = predStruct::getPred(support[i]);
      for (unsigned int j = 0; j < support.size(); j++) {
        predStruct *pj = predStruct::getPred(support[j]);
        if (pi == pj) continue;
        partresult *= predStruct::implies(pi, pj);
        partresult *= predStruct::trans(pi, pj, clockVar);
      }
    }
#if (defined __SYM_PRINTCONS_FULL__) || (defined __SYM_PRINTCONS_NUMS__)
    cout << "result size " << result.nodeCount() << endl;
    cout << "Done adding transitivity constraints for "
         << md->getVarName(clockVar) << endl;
#endif
    result += partresult;
  }
  return result;
}

/**
 * specifyClock
 * Performs the operation z.phi or phi[z:=0].
 *
 * mgr - BDD package amanger
 * design - ATACS data structures
 * predbdd - 2D vector of all predicates. predbdd[i][j] contains a vector
 *   of all predicates of the form xi >= xj +c
 * bddpred - Vector of all created predicated BDD nodes
 * var - clock variable in phi to assign to zero.
 * phi - Booleanized separation logic expression represented as a BDD.
 */
BDD specifyClock(modelData* md, int var, BDD phi) {
  // TODO: Using ComposeVector may be much more efficient here.
  // TODO: The efficiency of this nees to be improved.  Instead of
  // iterating through all BDD nodes, it should go directly to the
  // proper position in the predbddmap.
  // But i'm not sure how to use it.
  // Iterate through all current bdd variables and replace as
  // necessary.

  // Remove variables that won't be involved.
  vector<int> fullSupport = getSupport(md->getBDDMgr(), phi);
  vector<int> support;
  for (vector<int>::iterator i = fullSupport.begin();
       i != fullSupport.end(); i++) {
    if (predStruct::getPred(*i) != NULL &&
        (predStruct::getPred(*i)->i == var ||
         predStruct::getPred(*i)->j == var)) {
      support.push_back(*i);
    }
  }
  for (unsigned int i = 0; i < support.size(); i++) {
    predStruct *ps = predStruct::getPred(support[i]);
    if (ps->i == var) {
      BDD subBDD = predStruct::addPred(X0_I, ps->j, ps->c);
      phi = phi.Compose(subBDD, ps->var);
#ifdef __SYM_PRINTSUBS__
      cout << "sc3: Substituting predicate ";
      ps->toString();
      //cout << " with " ;
      //printPred(design, clockVars, cout, mgr, preds, subBDD);
      cout << endl;
#endif
    }
    else if (ps->j == var) {
      BDD subBDD = predStruct::addPred(ps->i, X0_I, ps->c);
      phi = phi.Compose(subBDD, ps->var);
#ifdef __SYM_PRINTSUBS__
      cout << "sc4: Substituting predicate ";
      ps->toString();
      //cout << " with " ;
      //printPred(design, clockVars, cout, mgr, preds, subBDD);
      cout << endl;
#endif
    }
  }
  return phi;
}


vector<int> getSupport(Cudd *mgr, BDD b) {
  set<int> result = getSupportSet(mgr, b);
  vector<int> vresult;
  for (set<int>::iterator i = result.begin(); i != result.end(); i++) {
    vresult.push_back(*i);
  }
  return vresult;
}

set<int> getSupportSet(Cudd *mgr, BDD b) {
  set<int> result;
  int* support = Cudd_SupportIndex(mgr->getManager(), b.getNode());
  for (int i = 0; i < mgr->ReadSize(); i++) {
    if (support[i] == 1)
      result.insert(i);
  }
  return result;
}

vector<int> getSupportWithVariable(Cudd *mgr, BDD b, int clockVar) {
  vector<int> fullSupport = getSupport(mgr, b);
  vector<int> support;
  for (vector<int>::iterator i = fullSupport.begin();
       i != fullSupport.end(); i++) {
    if (predStruct::getPred(*i) != NULL &&
        predStruct::getPred(*i)->contains(clockVar)) {
      support.push_back(*i);
    }
  }
  return support;
}
#endif
