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

#ifndef _symbolic_h_
#define _symbolic_h_

#include "color.h"
#include "def.h"
#include "struct.h"
#include "cudd/cuddObj.hh"
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <string>

class modelData;
class predStruct;

typedef vector< vector< vector<predStruct*> > > predbddmap;
typedef vector< predStruct* > bddpredmap;

// Predicate of the form c1 * x_i >= c2 * x_j + c
class predStruct {
public:
  static void initialize(modelData* md);
  static BDD addPred(int i, int j, double c);
  static BDD addPred(double ci, int i, double cj, int j, double c);
  static predStruct* getTempPred(double ci, int i, double cj, int j, double c);
  static bool predExists(double ci, int i, double cj, int j, double c);
  static unsigned int getPredCount();
  static predStruct* getPred(int bddIndex);
  
  static bool timpliest(predStruct* p1, predStruct* p2);
  static bool fimpliesf(predStruct* p1, predStruct* p2);
  static bool timpliesf(predStruct* p1, predStruct* p2);
  static bool fimpliest(predStruct* p1, predStruct* p2);
  static BDD implies(predStruct*pi, predStruct* pj);
  static BDD trans(predStruct* p1, predStruct* p2, int pivot,
                   bool create = true, BDD* phi = NULL);

  static bool isTrue(double ci, int i, double cj, int j, double c);
  static bool isFalse(double ci, int i, double cj, int j, double c);

  string toString();
  string toStringInverted();
  string toStringInverted2();
  static string toString(BDD var);
  static string toStringInverted(BDD var);
  static string toStringInverted2(BDD var);
  bool contains(int var);
  BDD bdd();
  int var;  // Index of BDD variable corresponding to this predicate
  double ci;
  int i;    // Variable i
  double cj;
  int j;    // Variable j
  double c; // Constant

private:
  predStruct(int i, int j, double c);
  predStruct(double ci, int i, double cj, int j, double c);

  static modelData *md;
  static Cudd *mgr;
  static predbddmap predbdd;
  static bddpredmap bddpred;
  static void printConstant(double c);
  static void validate(double ci, int i, double cj, int j, double c);
  static void canonicalize(double &ci, int &i, double &cj, int &j, double &c);
};

enum transType {PRIMARY, SECONDARY, SECONDARY2};

struct transStruct {
  BDD guard;
  vector<BDD> boolAssign;
  set<int> clockAssign;
  map<int, double> lowerClock; // I used a map instead of a hash_map
  map<int, double> upperClock; // because I don't expect these to be large
  transType type;
};

typedef vector<transStruct*> transvector;


//////////////////////////////////////////////////////////////////////////////
// For representing and constructing timed mu calculus statements.
//
enum tmops {TM_NOT,           // NOT
            TM_OR,            // OR
            TM_AND,           // AND
            TM_NS,            // Next state operation
            TM_SC,            // Specify Clock Variable
            TM_LFP,           // Least Fixpoint operation
            TM_SL,            // Separation Logic statement
            TM_VAR            // Formula variable
}; 

extern struct tmustmt* tmuroot;

extern int X0_I;
extern int N_DLT_I;
extern int N_EPS_I;

struct tmustmt {
  enum tmops op;
  struct tmustmt *tmu1;
  struct tmustmt *tmu2;
  BDD sl;
  int z;
  string X;
  string description;

  tmustmt() {
    tmu1 = NULL;
    tmu2 = NULL;
    z = -1;
    X = "";
    description = "";
    tmuroot = this; // TODO:  This is done so that I have a pointer to
                    // the root of the tmu statement after parsing is
                    // complete.  Is there a better way?
  }
};

tmustmt* not_tmustmt(tmustmt* tmu1);
tmustmt* or_tmustmt(tmustmt* tmu1, tmustmt* tmu2);
tmustmt* and_tmustmt(tmustmt* tmu1, tmustmt* tmu2);
tmustmt* ns_tmustmt(tmustmt* tmu1, tmustmt* tmu2);
tmustmt* sc_tmustmt(int z, tmustmt* tmu1);
tmustmt* lfp_tmustmt(string X, tmustmt* tmu1);
tmustmt* sl_tmustmt(BDD phi, string desc = "");
tmustmt* var_tmustmt(string X);
tmustmt* AU_tmustmt(tmustmt* tmu1, tmustmt* tmu2, modelData* md);
tmustmt* bAU_tmustmt(tmustmt* tmu1, tmustmt* tmu2, double constant,
                     modelData* md);
tmustmt* EU_tmustmt(tmustmt* tmu1, tmustmt* tmu2);
tmustmt* EF_tmustmt(tmustmt* tmu1, modelData* md);
tmustmt* AG_tmustmt(tmustmt* tmu1, modelData* md);
tmustmt* AF_tmustmt(tmustmt* tmu1, modelData* md);
tmustmt* bAF_tmustmt(tmustmt* tmu1, double constant, modelData* md);
tmustmt* EG_tmustmt(tmustmt* tmu1, modelData* md);
tmustmt* bEG_tmustmt(tmustmt* tmu1, double constant, modelData* md);

tmustmt* copy_tmustmt(tmustmt* tmu);
string tmustmtToString(tmustmt* tmu, modelData* md);
void delete_tmustmt(tmustmt* tmu);


//////////////////////////////////////////////////////////////////////////////

// A pure virtual class that all models will inherit from.
// This will allow the interface to be kept consistent and all models
// to be treated the same by the core symbolic analysis algorithm.
class modelData {
public:
  modelData(Cudd* mgr) { this->mgr = mgr; }
  virtual ~modelData() { }
  virtual Cudd* getBDDMgr() { return mgr; }

  virtual BDD initialState() = 0;
  virtual BDD buildInvariant() = 0;
  virtual transvector buildTransRels() = 0;
  virtual BDD addPredCons(int i, int j, double c) = 0;
  virtual BDD addPredCons(double ci, int i, double cj, int j, double c) = 0;
  virtual int addClockVar(string name) = 0;
  virtual string getVarName(int varIndex) = 0;
  virtual int getVarIndex(string name) = 0;
  virtual BDD getModelVarBDD(string name) = 0;
  virtual void printBDDState(BDD r) = 0;
  virtual BDD timeElapse(BDD phi1, BDD phi2) = 0;

  // Reference to the original design.
  designADT design;

  //protected:
  Cudd * mgr;
};

class variableRate {
public:
  variableRate(int varIndex, int lrate, int urate);
  
  int varIndex;
  int lrate;
  int urate;
};

typedef map<int, variableRate*> rateprod;
typedef vector<rateprod> ratesop;

class lhpnModelData : public modelData {
public:
  lhpnModelData(Cudd* mgr, designADT design);
  virtual ~lhpnModelData() { }

  virtual BDD initialState();
  virtual BDD buildInvariant();
  virtual transvector buildTransRels();
  virtual BDD addPredCons(int i, int j, double c);
  virtual BDD addPredCons(double ci, int i, double cj, int j, double c);
  virtual int addClockVar(string name);
  virtual string getVarName(int varIndex);
  virtual int getVarIndex(string name);
  virtual int getCVarIndex(string name);
  virtual BDD getModelVarBDD(string name);
  virtual void printBDDState(BDD r);
  virtual void countBDDState(BDD r);
  virtual string BDDToString(BDD r);
  BDD abstractInconsistentIneqs(BDD phi, rateprod rates);
  BDD transTT(double ci, int xi, double cj, int xj, double c1,
              double ck, int xk, double cm, int xm, double c2,
              BDD &e1, BDD &e2, BDD &phi);
  BDD transFF(double ci, int xi, double cj, int xj, double c1,
              double ck, int xk, double cm, int xm, double c2,
              BDD &e1, BDD &e2, BDD &phi);
  BDD transFT(double ci, int xi, double cj, int xj, double c1,
              double ck, int xk, double cm, int xm, double c2,
              BDD &e1, BDD &e2, BDD &phi);
  BDD transTF(double ci, int xi, double cj, int xj, double c1,
              double ck, int xk, double cm, int xm, double c2,
              BDD &e1, BDD &e2, BDD &phi);
  BDD cleanupbasedonrate(BDD phi);
  BDD cleanup(BDD phi);
  BDD applyTransCons(BDD phi, BDD A, BDD B, double ci, int xi,
                     double cj, int xj, double c, bool tense);
  void trueForm(predStruct* p, const rateprod &rates,
                double &ci, int &xi, double &cj,
                int &xj, double &c);
  void falseForm(predStruct* p, const rateprod &rates,
                 double &ci, int &xi, double &cj,
                 int &xj, double &c);
  BDD addNewIneqsHelper(predStruct* pred1, predStruct* p2,
                        const rateprod &rates, BDD &phi);
  BDD addTranConsNew(BDD phi);
  BDD addTranConsNew2(BDD phi,BDD guard);
  BDD addNewIneqs(BDD phi, const rateprod &rates);
  bool canTimeElapse(int *cube);
  BDD timeElapseNew(BDD phi);
  BDD timeElapsePhi1(BDD phi1);
  BDD timeElapsePhi2(BDD phi2, BDD result);
  virtual BDD timeElapse(BDD phi1, BDD phi2);
  virtual BDD rateBDD(int deltaVar);
  BDD simplifyRestrict(BDD f1);
  BDD finalsimplifyRestrict(BDD f1);
  BDD checkImplication(BDD f1,BDD f2);
  
  // Reference to the original design.
  designADT design;
  //private:
  vector<string> allRealVars;  // name associated with real variable index
  map<string, int> allModelVars; // map of variable name to bdd index.
  map<string, set<string> > ratesForRealVar;
  string clockString(int tindex);
  string rateToString(string varname, int lrate, int urate);
  string rateToString(ineqADT ineq);
  variableRate* stringToRate(string rateString);
  string clockActiveString(int tindex);
  BDD ineqADTToBDD(ineqADT ineq);
  BDD ineqADTToBDDwithRates(ineqADT ineq);
  BDD ineqADTToBDDwithRatesforSGC(ineqADT ineq, bool invert);
  BDD SOPtoBDD(level_exp exp, ineqADT ineqs, bool useRates = false);
  BDD SOPtoBDDforSGC(level_exp, ineqADT ineqs, bool invert);
  BDD ProdToBDD(char* product, ineqADT ineqs, bool invert, int skipplace);
  BDD SOPtoPIBDD(level_exp exp, ineqADT ineqs, bool useRates = false);
  //transStruct* buildTransType1(eventADT event, int tindex);
  //transStruct* buildTransType2(eventADT event, int tindex);
  BDD presetBDD(int tindex);
  BDD postsetBDD(int tindex);
  void buildAssignments(ineqADT ineqs, int tindex, transStruct* trans);
  transvector mergePrimarySecondary(transvector transRels);
  bool BDDHasIneqs(BDD r);
  void mergeAssignIntoPrim(transStruct* primary, transStruct* secondary);
  
  BDD findDisStates();
  vector<BDD> disStateVector();
  void printTransRel(transStruct* trans);
  rateprod bddToRateProd(BDD r);
  vector<BDD> rateBDDVector();
  vector<BDD> rateBDDVectorForSMT();
  ratesop rateSOPVector(vector<BDD> ratevector);

  BDD rphidlt;
  BDD rphieps;
  vector<BDD> eachDisState;
  // indexing into each of eachRateProd and eachRateBDD with the same
  // value, should result in the same set of rates, just with a BDD
  // representation and vector representation, respectively.
  ratesop eachRateProd;
  vector<BDD> eachRateBDD;
  BDD allDisStates;
  bool allDisStatesBuilt;
  vector<BDD> placeDisVariables;
  vector<BDD> booleanDisVariables;
  vector<BDD> clockActiveDisVariables;
  vector<BDD> rateDisVariables;
  BDD myInvariant;
  bool invariantBuilt;
  BDD myInitialState;
  bool initialStateBuilt;
};
  

//////////////////////////////////////////////////////////////////////////////

void symbolic(designADT design);

tmustmt* loadProperty(modelData* md, designADT design);

void cleanup(modelData* md, transvector &transRels, tmustmt* mu);

BDD recCheck (modelData* md, BDD &invariant,
              transvector transRels, tmustmt* mu, bool &terminatedEarly);

BDD applyAssignments(modelData* md, BDD phi, transStruct* trans);

BDD abstractVarTense(BDD var, BDD phi);

BDD prenew(modelData* md, BDD phi, BDD &invariant, transvector transRels);

BDD pre(modelData* md, BDD phi, BDD &invariant, transvector transRels);

BDD pret(modelData* md, BDD phi, transStruct* trans);

BDD unconstrain(modelData* md, BDD f);

BDD getTranCons(modelData* md, BDD phi, int clockVar);

BDD restrictTranCons(modelData* md, BDD phi, int clockVar);

BDD addTranCons(modelData* md, BDD phi, int clockVar);

BDD specifyClock(modelData* md, int var, BDD phi);

void printTrace(modelData* md, vector<BDD> trace,
		BDD invariant, transvector transRels);

void substitute(tmustmt* mu, string X, BDD phi, string description = "");

void unsubstitute(tmustmt* mu, string X);

bool bndsNotConjoined(modelData* md, BDD phi, BDD v1, BDD v2);

BDD quantElimination(modelData* md, BDD phi);

vector<int> getSupport(Cudd *mgr, BDD b);

set<int> getSupportSet(Cudd *mgr, BDD b);

vector<int> getSupportWithVariable(Cudd *mgr, BDD b, int clockVar);

#endif /* symbolic .h */
