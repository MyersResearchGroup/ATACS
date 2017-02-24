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

#include "smtsym.h"
#include <iostream>
#include <sstream>
#include <limits>

using namespace std;

#define __SMT_PROGRESS__
//#define __SMT_TYPECHECK__
#define __SMT_PRINTTRACEONSAT__
//#define __SMT_MKDECLS__
//#define __SMT_MKDELTAFORITERATION__
//#define __SMT_MKEXPRFORPRED__
//#define __SMT_MKEXPRFORBDD__
//#define __SMT_MKEXPRFORINV__
//#define __SMT_MKEXPRFORTRANSREL__
//#define __SMT_MKEXPRFORTIMEELAPSE__
//#define __SMT_MKEXPRFORINITSTATE__
//#define __SMT_MKEXPRFORFAILPROP__

// NOTES:
// Throughout the code below, I construct vectors and pass them into the 
// yices API functions as arrays.  This behavior is documented and explained
// in Item 16 of the book "Effective STL".  

// TODO:
// On some of the debug outputs should it only print for the first iteration?
// (Like on invariant, transrel, time elapse, etc.  They're all the same 
// except for the iteration number.)
#ifdef YICES

statedecls* mkDeclsForIteration(yices_context ctx, lhpnModelData* md,
				int iteration) {
  statedecls* sd = new statedecls();
  sd->iteration = iteration;
  sd->md = md;
  sd->fail = NULL;
  // Iterate through the allModelVars map in lhpnModelData and construct
  // a Boolean yices_var_decl for each Boolean variable in the model.  The 
  // index in the vector corresponds to the BDD index.
  sd->booldecls.resize(md->allModelVars.size(), NULL);
  for (map<string, int>::iterator i = md->allModelVars.begin(); 
       i != md->allModelVars.end(); i++) {
    ostringstream oss;
    oss << (*i).first << ":" << iteration;
    char* name = const_cast<char*>(oss.str().c_str());
    sd->booldecls[(*i).second] = yices_mk_bool_var_decl(ctx, name);
    if ((*i).first == "fail") {
      sd->fail = sd->booldecls[(*i).second];
    }
#ifdef __SMT_MKDECLS__
    cout << "Created Boolean yices_var_decl with name " 
	 << yices_get_var_decl_name(sd->booldecls[(*i).second]) << endl;
#endif
  }
  // Iterate through the allRealVars vector in lhpnModelData and construct
  // a yices_var_decl for each real variable in the model.  The index in the 
  // vector corresponds to the real variable index in the model.
  // It is not necessary to create yices_var_decls for X0_I, N_DLT_I, and
  // N_EPS_I
  yices_type type = yices_mk_type(ctx, (char*)"real");
  sd->realdecls.resize(md->allRealVars.size()*2, NULL);
  for (vector<string>::iterator i = md->allRealVars.begin();
       i != md->allRealVars.end(); i++) {
    if (*i == md->getVarName(X0_I) || 
	*i == md->getVarName(N_DLT_I) ||
	*i == md->getVarName(N_EPS_I)) {
      // Don't declare these variables.
      continue;
    }
    ostringstream oss;
    oss << *i << ":" << iteration;
    char* name = const_cast<char*>(oss.str().c_str());
    sd->realdecls[md->getVarIndex(*i)] = yices_mk_var_decl(ctx, name, type);
#ifdef __SMT_MKDECLS__
    cout << "Created real yices_var_decl with name " 
	 << yices_get_var_decl_name(sd->realdecls[md->getVarIndex(*i)]) 
	 << endl;
#endif
  } 
  return sd;
}

yices_var_decl mkDeltaForIteration(yices_context ctx, int iteration) {
  yices_type type = yices_mk_type(ctx, (char*)"real");
  ostringstream oss;
  oss << "delta" << iteration;
  char* name = const_cast<char*>(oss.str().c_str());
  yices_var_decl result = yices_mk_var_decl(ctx, name, type);
#ifdef __SMT_MKDELTAFORITERATION__
  cout << "Created yices_var_decl for delta timestep "
       << yices_get_var_decl_name(result) << endl;
#endif
  return result;
}

yices_expr mkExprForPred(yices_context ctx, statedecls* sd, predStruct* pred) {
  // TODO: More precise way of converting the constants to integers?
  // Perhaps store the original values used when constructing the inequality
  // from the model?

  // TODO: If multiplying by 1, leave the multiplication operation out.
  yices_expr left;
  yices_expr right;
  // Special cases for when xi or xj is x0.
  if (pred->i == X0_I) {
    left = yices_mk_num(ctx, 0);
  }
  else {
    ostringstream oss;
    oss << pred->ci;
    yices_expr args[2];
    args[0] = yices_mk_var_from_decl(ctx, sd->realdecls[pred->i]);
    args[1] = yices_mk_num_from_string
      (ctx, const_cast<char*>(oss.str().c_str()));
    left = yices_mk_mul(ctx, args, 2);
  }
  if (pred->j == X0_I) {
    right = yices_mk_num(ctx, 0);
  }
  else {
    ostringstream oss;
    oss << pred->cj;
    yices_expr args[2];
    args[0] = yices_mk_var_from_decl(ctx, sd->realdecls[pred->j]);
    args[1] = yices_mk_num_from_string
      (ctx, const_cast<char*>(oss.str().c_str()));
    right = yices_mk_mul(ctx, args, 2);
  }
  ostringstream oss;
  oss << pred->c;
  yices_expr c = yices_mk_num_from_string
    (ctx, const_cast<char*>(oss.str().c_str()));  
  // Construct cj xj + c expression
  yices_expr args[2];
  args[0] = right;
  args[1] = c;
  right = yices_mk_sum(ctx, args, 2);
  
  // Construct complete ci xi >= cj xj + c expression
  yices_expr result = yices_mk_ge(ctx, left, right);

#ifdef __SMT_MKEXPRFORPRED__
  cout << pred->toString() << ": ";
  yices_pp_expr(result);
  cout << endl;
#endif  
  return result;
}

yices_expr mkExprForBDD(yices_context ctx, statedecls* sd, BDD bdd) {
  yices_expr result;
  if (bdd == sd->md->mgr->bddZero()) {
    result = yices_mk_false(ctx);
  }
  if (bdd == sd->md->mgr->bddOne()) {
    result = yices_mk_true(ctx);
  }

  int* cube = NULL;
  DdGen *g = Cudd_FirstPrime(sd->md->mgr->getManager(), 
                             bdd.getNode(), bdd.getNode() ,&cube);
  assert(g);
  ExprContainer sum;
  do {
    ExprContainer prod;
    for (unsigned int i = 0; i < sd->booldecls.size(); i++) {
      if (sd->booldecls[i] == NULL) continue;
      if (cube[i] == 0) {
	prod.push_back(yices_mk_not
		       (ctx, yices_mk_var_from_decl(ctx, sd->booldecls[i])));
      }
      else if (cube[i] == 1) {
	prod.push_back(yices_mk_var_from_decl(ctx, sd->booldecls[i]));
      }
    }
    for (unsigned int k = 0; k < predStruct::getPredCount(); k++) {
      predStruct *ps = predStruct::getPred(k);
      if (ps == NULL) continue;
      if (cube[ps->var] == 0) {
        prod.push_back(yices_mk_not(ctx, mkExprForPred(ctx, sd, ps)));
      }
      else if (cube[ps->var] == 1) {
	prod.push_back(mkExprForPred(ctx, sd, ps));
      }
    }

    if (prod.size() == 1) {
      sum.push_back(prod[0]);
    }
    else if (!prod.empty()) {
      sum.push_back(yices_mk_and(ctx, &prod[0], prod.size()));
    } 
  } while (Cudd_NextPrime(g, &cube));
  GenFree(g);
  if (sum.size() == 1) {
    result = sum[0];
  }
  else if (!sum.empty()) {
    result = yices_mk_or(ctx, &sum[0], sum.size());
  }
  else {
    result = yices_mk_true(ctx);
  }
#ifdef __SMT_MKEXPRFORBDD__
  cout << "BDD for iteration " << sd->iteration << ": " << endl;
  yices_pp_expr(result);
  cout << endl;
#endif
  return result;
}

yices_expr mkExprForInv(yices_context ctx, statedecls* sd, BDD inv) {
  //todo: clocks >= 0?
  yices_expr result = mkExprForBDD(ctx, sd, inv);
#ifdef __SMT_MKEXPRFORINV__
  cout << "Invariant: " << endl;
  yices_pp_expr(result);
  cout << endl;
#endif
  return result;
}

// TODO: variables that are not assigned anything should be assigned their
// old value.
yices_expr mkExprForTransRel(yices_context ctx, statedecls* sdpresent,
			     statedecls* sdnext, yices_var_decl delta, 
			     transStruct* ts) {
  ExprContainer args;
  // The guard in terms of the present state variables.
  args.push_back(mkExprForBDD(ctx, sdpresent, ts->guard));
  // If a transition fires, the delta for the step was 0
  args.push_back(yices_mk_eq(ctx, yices_mk_var_from_decl(ctx, delta),
			     yices_mk_num(ctx, 0)));
  // The Boolean portion of the assignments in terms next state variables.
  // Variables that are not explicitly assigned a new value, should be
  // assigned their previous value in the next state.
  for (unsigned int i = 0; i < sdpresent->booldecls.size(); i++) {
    bool assigned = false;
    for (vector<BDD>::iterator j = ts->boolAssign.begin(); 
	 j != ts->boolAssign.end(); j++) {
      // TODO:  Concerned about the following.  If it is an inverse of a 
      // of a variable does NodeReadIndex still return the proper index?
      if ((*j).NodeReadIndex() == i) {
	args.push_back(mkExprForBDD(ctx, sdnext, *j));
	assigned = true;
	break;
      }
    }
    if (!assigned) {
      args.push_back(yices_mk_eq(ctx, 
				 yices_mk_var_from_decl
				 (ctx, sdnext->booldecls[i]),
				 yices_mk_var_from_decl
				 (ctx, sdpresent->booldecls[i])));;
    }
  }
  // The real variable portion of the assignments in terms of the
  // next state variables.
  // Variables that are not explicitly assigned a new value, should be
  // assigned their previous value in the next state.
  for (unsigned int i = 0; i < sdpresent->realdecls.size(); i++) {
    if (sdpresent->realdecls[i] == NULL) {
      continue;
    }
    if (ts->clockAssign.find(i) != ts->clockAssign.end()) {
      // If clock is assigned to infinity, don't make any assignment 
      // (leave undefined)
      if (ts->lowerClock[i] != numeric_limits<double>::infinity() && 
	  ts->lowerClock[i] != -numeric_limits<double>::infinity())  {
	args.push_back(yices_mk_ge(ctx, 
				   yices_mk_var_from_decl
				   (ctx, sdnext->realdecls[i]), 
				   yices_mk_num(ctx, ts->lowerClock[i])));
      }
      if (ts->upperClock[i] != numeric_limits<double>::infinity() &&
	  ts->upperClock[i] != -numeric_limits<double>::infinity()) {
	args.push_back(yices_mk_le(ctx, 
				   yices_mk_var_from_decl
				   (ctx, sdnext->realdecls[i]), 
				   yices_mk_num(ctx, ts->upperClock[i])));
      }
    }
    else {
      args.push_back(yices_mk_eq(ctx, 
				 yices_mk_var_from_decl
				 (ctx, sdnext->realdecls[i]),
				 yices_mk_var_from_decl
				 (ctx, sdpresent->realdecls[i])));;
    }
  }
  yices_expr result;
  if (!args.empty()) {
    result = yices_mk_and(ctx, &args[0], args.size());
  } 
  else {
    result = yices_mk_true(ctx);
  }	
#ifdef __SMT_MKEXPRFORTRANSREL__
  cout << "Transition Relation: " << endl;
  yices_pp_expr(result);
  cout << endl;
#endif
  return result;
}

yices_expr mkExprForTimeElapse(yices_context ctx, statedecls* sdpresent,
			       statedecls* sdnext, yices_var_decl delta,
			       vector<BDD> eachRateBDD, ratesop eachRateProd) {
  ExprContainer args;
  // delta is greater than zero.
  // TODO: geq or gt?
  args.push_back(yices_mk_ge(ctx, yices_mk_var_from_decl(ctx, delta), 
			     yices_mk_num(ctx, 0)));
  // Present state Boolean variables equal next state Boolean variables.
  for (unsigned int i = 0; i < sdpresent->booldecls.size(); i++) {
    args.push_back(yices_mk_eq(ctx, yices_mk_var_from_decl
			       (ctx, sdnext->booldecls[i]),
			       yices_mk_var_from_decl
			       (ctx, sdpresent->booldecls[i])));
  }
  
  // Iterate through the possible rates and form inequalities for the 
  // real variables based on the Boolean rate variables.
  ExprContainer ratesum;
  for (unsigned int i = 0; i < eachRateBDD.size(); i++) {
    ExprContainer rateprod;
    rateprod.push_back(mkExprForBDD(ctx, sdpresent, eachRateBDD[i]));
    for (rateprod::iterator j = eachRateProd[i].begin();
	 j != eachRateProd[i].end(); j++) {
      // TODO:  If rate is negative, do anything differently?
      variableRate* vr = (*j).second;
      yices_expr varnext = yices_mk_var_from_decl
	(ctx, sdnext->realdecls[vr->varIndex]);
      yices_expr varpres = yices_mk_var_from_decl
	(ctx, sdpresent->realdecls[vr->varIndex]);
      yices_expr lrate = yices_mk_num(ctx, vr->lrate);
      yices_expr urate = yices_mk_num(ctx, vr->urate);
      // lowerbound inequality.
      yices_expr iargs[2];
      iargs[0] = yices_mk_var_from_decl(ctx, delta);
      iargs[1] = lrate;
      iargs[1] = yices_mk_mul(ctx, iargs, 2);
      iargs[0] = varpres;
      iargs[0] = yices_mk_sum(ctx, iargs, 2);
      rateprod.push_back(yices_mk_ge(ctx, varnext, iargs[0]));
      // upperbound inequality
      iargs[0] = yices_mk_var_from_decl(ctx, delta);
      iargs[1] = urate;
      iargs[1] = yices_mk_mul(ctx, iargs, 2);
      iargs[0] = varpres;
      iargs[0] = yices_mk_sum(ctx, iargs, 2);
      rateprod.push_back(yices_mk_le(ctx, varnext, iargs[0]));
    }
    if (!rateprod.empty()) {
      ratesum.push_back(yices_mk_and(ctx, &rateprod[0], rateprod.size()));
    }
  }
  if (!ratesum.empty()) {
    args.push_back(yices_mk_or(ctx, &ratesum[0], ratesum.size()));
  }

  // Clocks always increment at a rate of 1 * delta.
  for (unsigned int i = 0; i < sdnext->realdecls.size(); i++) {
    if (sdnext->realdecls[i] == NULL) {
      continue;
    }
    if (!isClock(sdnext->realdecls[i])) {
      continue;
    }
    yices_expr next = yices_mk_var_from_decl(ctx, sdnext->realdecls[i]);
    yices_expr iargs[2];
    iargs[0] = yices_mk_var_from_decl(ctx, sdpresent->realdecls[i]);
    iargs[1] = yices_mk_var_from_decl(ctx, delta);
    args.push_back(yices_mk_eq(ctx, next, yices_mk_sum(ctx, iargs, 2)));
  }
 
  yices_expr result;
  if (args.size() == 1) {
    result = args[0];
  }		   
  else if (!args.empty()) {
    result = yices_mk_and(ctx, &args[0], args.size());
  } 
  else {
    result = yices_mk_true(ctx);
  }
  
#ifdef __SMT_MKEXPRFORTIMEELAPSE__
  cout << "Time Elapse: " << endl;
  yices_pp_expr(result);
  cout << endl;
#endif
  return result;
}

bool isClock(yices_var_decl var) {
  string name = yices_get_var_decl_name(var);
  if (name.substr(0,1) == "#") {
    return true;
  }
  else {
    return false;
  }
}

yices_expr mkExprForInitState(yices_context ctx, statedecls* sd,
			      BDD initState) {
  yices_expr result = mkExprForBDD(ctx, sd, initState);
#ifdef __SMT_MKEXPRFORINITSTATE__
  cout << "Initial State: " << endl;
  yices_pp_expr(result);
  cout << endl;
#endif
  return result;
}

yices_expr mkExprForFailProp(yices_context ctx, StateContainer sds) {
  ExprContainer args;
  for (unsigned int i = 0; i < sds.size(); i++) {
    if (sds[i]->fail != NULL) {
      args.push_back(yices_mk_var_from_decl(ctx, sds[i]->fail));
    }
  }
  yices_expr result;
  if (args.size() == 1) {
    result = args[0];
  }
  else if (!args.empty()) { 
    result = yices_mk_or(ctx, &args[0], args.size());
  }
  else {
    result = yices_mk_true(ctx);
  }
#ifdef __SMT_MKEXPRFORFAILPROP__
  cout << "Default Failure Property: " << endl;
  yices_pp_expr(result); 
  cout << endl;
#endif
  return result;
}

yices_expr mkExprForGClock(yices_context ctx, StateContainer sds) {
  ExprContainer args;
  for (unsigned int i = 0; i < sds.size(); i++) {
    for (DCIterator j = sds[i]->realdecls.begin();
	 j != sds[i]->realdecls.end(); j++) {
      if (*j == NULL) {
	continue;
      }
      if (string(yices_get_var_decl_name(*j)).substr(0, 6) == "gclock") {
	args.push_back(yices_mk_eq(ctx, yices_mk_var_from_decl(ctx, *j),
				   yices_mk_num(ctx, 1)));
	break;
      }
    }
  }
  yices_expr result;
  if (args.size() == 1) {
    result = args[0];
  }
  else if (!args.empty()) { 
    result = yices_mk_or(ctx, &args[0], args.size());
  }
  else {
    result = yices_mk_true(ctx);
  }
#ifdef __SMT_MKEXPRFORFAILPROP__
  cout << "Default Failure Property: " << endl;
  yices_pp_expr(result); 
  cout << endl;
#endif
  return result;
}


void smtSymbolic(designADT design) {
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
  BDD invariant = md->buildInvariant();
  BDD iState = md->initialState();

  cout << "Beginning SMT model checking" << endl;
  cout << "Max iterations: " << design->iterations << endl;
  smtCheck(md, iState, invariant, transRels, design->iterations, 5);
  cleanup(md, transRels, NULL);
  return;
}

void smtCheck(lhpnModelData* md, BDD& iState, BDD& invariant, 
	      transvector& transRels, int iterations, int checkEvery) {
#ifdef __SMT_TYPECHECK__
  cout << "Enabling Yices type checking..." << endl;
  yices_enable_type_checker(1);
#endif
  // Create a special rateBDD where the clockactive variables were
  // also removed.
  vector<BDD> smtRateBDD = md->rateBDDVectorForSMT();
  ratesop smtRateProd = md->rateSOPVector(smtRateBDD);
  yices_context ctx = yices_mk_context();
  StateContainer states;
  DeclContainer deltas;
#ifdef __SMT_PROGRESS__
    cout << 0 << flush;
#endif
  states.push_back(mkDeclsForIteration(ctx, md, 0));
  yices_assert(ctx, mkExprForInitState(ctx, states[0], iState));
  yices_assert(ctx, mkExprForInv(ctx, states[0], invariant));
  lbool smtresult;
  for (int i = 1; i < iterations; i++) {
#ifdef __SMT_PROGRESS__
    cout << i % 10 << flush;
#endif
    states.push_back(mkDeclsForIteration(ctx, md, i));
    deltas.push_back(mkDeltaForIteration(ctx, i-1));
    yices_assert(ctx, mkExprForInv(ctx, states[i], invariant));
    ExprContainer args;
    for (transvector::iterator t = transRels.begin(); 
    	 t != transRels.end(); t++) {
      args.push_back(mkExprForTransRel(ctx, states[i-1], states[i], 
				       deltas.back(), *t));
    }
    args.push_back(mkExprForTimeElapse(ctx, states[i-1], states[i], 
				       deltas.back(), 
				       smtRateBDD, smtRateProd));
    yices_assert(ctx, yices_mk_or(ctx, &args[0], args.size()));
    if (i%checkEvery == 0 && i != iterations) {
      yices_push(ctx);
      yices_assert(ctx, mkExprForFailProp(ctx, states));
      smtresult = yices_check(ctx);
      if (smtresult == l_true) {
	cout << endl << "Found inconsistency on iteration " << i << endl;
	break;
      }
      yices_pop(ctx);
    }
  }
  yices_assert(ctx, mkExprForFailProp(ctx, states));
  cout << endl;
  smtresult = yices_check(ctx);
  if (smtresult == l_true) {
    cout << "Context is satisfiable ==> Property violated" << endl;
#ifdef __SMT_PRINTTRACEONSAT__
    printSimplifiedModel(states, deltas, yices_get_model(ctx));
#endif
  }
  else if (smtresult == l_false) {
    cout << "Context is not satisfiable ==> Property not violated (yet)" 
	 << endl;
  }
  else {
    cout << "Context is undefined ==> Something's wrong." << endl;
  }
  yices_del_context(ctx);
}


void printTraceUsingSMT(lhpnModelData* md, BDD iState, BDD& invariant, 
			transvector& transRels, vector<BDD> trace) {

  // Create a special rateBDD where the clockactive variables were
  // also removed.
  vector<BDD> smtRateBDD = md->rateBDDVectorForSMT();
  ratesop smtRateProd = md->rateSOPVector(smtRateBDD);
  yices_context ctx = yices_mk_context();
  StateContainer states;
  DeclContainer deltas;
  states.push_back(mkDeclsForIteration(ctx, md, 0));
  yices_assert(ctx, mkExprForInitState(ctx, states[0], iState));
  yices_assert(ctx, mkExprForInv(ctx, states[0], invariant));
  lbool smtresult;
  //for (unsigned int i = 1; i < trace.size()*2; i++) {
  cout << "Trace size: " << trace.size() << endl;
  for (unsigned int i = 1; i < 5; i++) {
#ifdef __SMT_PROGRESS__
    cout << i % 10 << flush;
#endif
    states.push_back(mkDeclsForIteration(ctx, md, i));
    deltas.push_back(mkDeltaForIteration(ctx, i-1));
    yices_assert(ctx, mkExprForInv(ctx, states[i], invariant));
    ExprContainer args;
    if (i%2 == 0) {
      for (transvector::iterator t = transRels.begin(); 
	   t != transRels.end(); t++) {
	args.push_back(mkExprForTransRel(ctx, states[i-1], states[i], 
					 deltas.back(), *t));
      }
    }
    else {	 
      args.push_back(mkExprForTimeElapse(ctx, states[i-1], states[i], 
				       deltas.back(), 
				       smtRateBDD, smtRateProd));
    }
    yices_assert(ctx, yices_mk_or(ctx, &args[0], args.size()));
    yices_assert(ctx, mkExprForBDD(ctx, states[i], trace[trace.size()-((i+2)/2)]));
  }
  smtresult = yices_check(ctx);
  if (smtresult == l_true) {
    cout << "Context is satisfiable ==> Trace Found" << endl;
#ifdef __SMT_PRINTTRACEONSAT__
    printModel(states, deltas, yices_get_model(ctx));
#endif
  }
  else if (smtresult == l_false) {
    cout << "Context is not satisfiable ==> Trace Not Found" 
	 << endl;
  }
  else {
    cout << "Context is undefined ==> Something's wrong." << endl;
  }
  cout << endl;
  cout << "iteration 4" << endl;
  md->printBDDState(trace[trace.size()-3]);
  yices_del_context(ctx);
}


void printModel(StateContainer states, DeclContainer deltas,
		yices_model m) {
  DCIterator delta = deltas.begin();
  for (SCIterator state = states.begin();
       state != states.end(); state++) {
    for (DCIterator var = (*state)->booldecls.begin();
	 var != (*state)->booldecls.end(); var++) {
      if (yices_get_value(m, *var) == l_true) {
	cout << yices_get_var_decl_name(*var) << " = true" << endl;
      }
      else if (yices_get_value(m, *var) == l_false) {
	cout << yices_get_var_decl_name(*var) << " = false" << endl;
      }
      else {
      	cout << yices_get_var_decl_name(*var) << " = undef" << endl;
      }
    }
    for (DCIterator var = (*state)->realdecls.begin();
	 var != (*state)->realdecls.end(); var++) {
      if (*var == NULL) {
	continue;
      }
      double value;
      yices_get_double_value(m, *var, &value);
      cout << yices_get_var_decl_name(*var) << " = " << value << endl;
    }
    if (delta != deltas.end()) {
      cout << endl;
      double value;
      yices_get_double_value(m, *delta, &value);
      cout << yices_get_var_decl_name(*delta) << " = " << value << endl;
      cout << endl;
      delta++;
    }
  }
}

void printSimplifiedModel(StateContainer states, DeclContainer deltas,
			  yices_model m) {
  DCIterator delta = deltas.begin();
  SCIterator prevstate = states.begin();
  for (SCIterator state = states.begin();
       state != states.end(); state++) {
    DCIterator prevstatevar = (*prevstate)->booldecls.begin();
    for (DCIterator var = (*state)->booldecls.begin();
	 var != (*state)->booldecls.end(); var++) {
      if (state == states.begin() || 
	  yices_get_value(m, *var) != yices_get_value(m, *prevstatevar)) {
	if (yices_get_value(m, *var) == l_true) {
	  cout << yices_get_var_decl_name(*var) << " = true" << endl;
	}
	else if (yices_get_value(m, *var) == l_false) {
	  cout << yices_get_var_decl_name(*var) << " = false" << endl;
	}
	else {
	  cout << yices_get_var_decl_name(*var) << " = undef" << endl;
	}
      }
      prevstatevar++;
    }
    prevstatevar = (*prevstate)->realdecls.begin();
    for (DCIterator var = (*state)->realdecls.begin();
	 var != (*state)->realdecls.end(); var++) {
      if (*var != NULL) {
	double value;
	yices_get_double_value(m, *var, &value);
	double prevstatevalue;
	yices_get_double_value(m, *prevstatevar, &prevstatevalue);
	if (state == states.begin() || value != prevstatevalue) {
	  cout << yices_get_var_decl_name(*var) << " = " << value << endl;
	}
      }
      prevstatevar++;
    }
    if (delta != deltas.end()) {
      cout << endl;
      double value;
      yices_get_double_value(m, *delta, &value);
      cout << yices_get_var_decl_name(*delta) << " = " << value << endl;
      cout << endl;
      delta++;
    }
    if (state != states.begin()) {
      prevstate++;
    }
  }
}

#endif
