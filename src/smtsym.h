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

#ifndef _smtsym_h_
#define _smtsym_h_

#include "struct.h"
#include "def.h"
#include "color.h"
#include "symbolic.h"
#include "yices/yices_c.h"
#include <map>

using namespace std;

typedef vector<yices_var_decl> DeclContainer;
typedef DeclContainer::iterator DCIterator;

typedef vector<yices_expr> ExprContainer;
typedef ExprContainer::iterator ECIterator;

// The string corresponds to the string model name.  The name in the
// yices_var_decl corresponds to the string model name + the iteration value.
struct statedecls {
  int iteration; // The iteration that these statedecls are for.
  lhpnModelData* md; // just in case I need it.
  yices_var_decl fail; // the Boolean fail variable.
  // BDD index correlates with decl.  Some of the elements of this vector
  // could potentially be NULL.
  DeclContainer booldecls;
  // Real variable index correlates with vardecl.  
  // Use lhpnModelData::getVarName to get string name.  Note that some of the 
  // elements of this vector will be NULL.
  DeclContainer realdecls;
};

typedef vector<statedecls*> StateContainer;
typedef StateContainer::iterator SCIterator;


// Given an initialized yices_context, an initialized lhpnModelData, 
// and an iteration number, constructs a statedecls struct which contains
// yices_var_decls for each of the Boolean and real variables in the lhpn.
// The user is responsible for deallocating the statedecls struct using 
// delete.
statedecls* mkDeclsForIteration(yices_context ctx, lhpnModelData* md, 
				int interation);

// Given an initialized yices_context and an iteration number,
// constructs a real yices_var_decl for for a delta time step.
yices_var_decl mkDeltaForIteration(yices_context ctx, int iteration);

// Given an initialized yices_context, a valid predStruct, and a statedecls
// that has been constructed using mkDeclsForIteration, generates a yices_expr
// representing the inequality in predStruct. 
yices_expr mkExprForPred(yices_context ctx, statedecls* sd, predStruct* pred);

// Given an initialized yices_context, a statedecls that has been
// constructed using mkDeclsForIteration, and a BDD representing a Boolean
// relatipnship, constructs an equivalent yices expression 
// representing that model.
yices_expr mkExprForBDD(yices_context ctx, statedecls* sd, BDD bdd);

// Given an initialized yices_context, a statedecls that has been
// constructed using mkDeclsForIteration, and a BDD representing the
// invariant for a model, constructs an equivalent yices expression 
// representing that model.
yices_expr mkExprForInv(yices_context ctx, statedecls* sd, BDD inv);

// Given an initialized yices_context, two statedecls (represent present and 
// next state) which have been constructed using mkDeclsForIteration, and a
// transStruct for a particular transition, constructs a yices expression
// representing that transStruct.
yices_expr mkExprForTransRel(yices_context ctx, statedecls* sdpresent,
			     statedecls* sdnext, yices_var_decl delta, 
			     transStruct* ts);

// Given an initialized yices_context, two statedecls(representing present
// and next state) which have been constructed using mkDeclsForIteration,
// eachRateBDD, and eachRateProd, constructs a yices expression representing
// the time elapse operation.  Delta is also passed in so that it is assigned
// to zero when a transition fires.
yices_expr mkExprForTimeElapse(yices_context ctx, statedecls* sdpresent,
			       statedecls* sdnext, yices_var_decl delta,
			       vector<BDD> eachRateBDD, ratesop eachRateProd);

// Given a yices_var_decl returns true of the variable corresponds to a 
// clock variable.  False otherwise.
bool isClock(yices_var_decl var);

// Given an initialized yices_context,  a statedecls and a BDD representing
// the initial state, returns a yices_expr representing the initial state.
yices_expr mkExprForInitState(yices_context ctx, statedecls* sd,
			      BDD initState);

// Given an initializes yices_context, and a vector of statedecls which 
// have been constructed using mkDeclsForIteration, returns a property which
// is the disjunction of the fail Boolean variables in each state.  
// If there are no fail boolean variables in any of the states, returns true.
yices_expr mkExprForFailProp(yices_context ctx, StateContainer sds);

// Useful for debugging purposes.  If there is a clock variable named
// "gclock", this property will assert that the value of the clock will
// eventually be some value.  This way, a trace can be generated at some
// point in the future to see what is going on.
yices_expr mkExprForGClockProp(yices_context ctx, StateContainer sds);

// Outer function for call to SMT based symbolic model checker.
// Ensures that a valid property has been read in, creates the
// lhpnModelData structure, and then calls the smtCheck functio which
// begins the actual model checking.
void smtSymbolic(designADT design);

// Performs the main model checking.  Asserts the invariant, and then
// the transition relations and time elapse for each iteration.  Finally,
// asserts the property.  iterations specifies the bound on the number of
// iterations that should be performed.  checkEvery is the number of
// iterations that must pass before a check should be performed
void smtCheck(lhpnModelData* md, BDD& iState, BDD& invariant, 
	      transvector& transRels, int iterations, int checkEvery);

// Prints a satisfying assignment.  This is equivalent to a trace
// corresponding to a failure of the property.
void printModel(StateContainer states, DeclContainer deltas,
		yices_model m);

// Prints a satisfying assignment where only the variables that change
// at each iteration are printed out, in addition to the first state's
// variables.  This is equivalent to a trace corresponding to a 
// failure of the property.
void printSimplifiedModel(StateContainer states, DeclContainer deltas,
			  yices_model m);

// This function is called by the BDD based model checker to generate an 
// error trace for a given fixpoint iteration.
void printTraceUsingSMT(lhpnModelData* md, BDD iState, BDD& invariant, 
			transvector& transRels, vector<BDD> trace);

#endif /* smtsym.h */
