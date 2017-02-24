/**
 * @name CTextTel Class
 * @version 0.1 beta
 *
 * (c)opyright 1998 by Christopher D. Krieger
 *
 * @author Christopher D. Krieger
 *
 * Permission to use, copy, modify and/or distribute, but not sell, this
 * software and its documentation for any purpose is hereby granted
 * without fee, subject to the following terms and conditions:
 *
 * 1. The above copyright notice and this permission notice must
 * appear in all copies of the software and related documentation.
 *
 * 2. The name of University of Utah may not be used in advertising or
 * publicity pertaining to distribution of the software without the
 * specific, prior written permission of Univsersity of Utah.
 *
 * 3. This software may not be called "ATACS" if it has been modified
 * in any way, without the specific prior written permission of
 * Christopher D. Krieger
 *
 * 4. THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF UTAH OR THE AUTHORS OF THIS
 * SOFTWARE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFIT, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _CTEXTTEL_H_ 
#define _CTEXTTEL_H_

// includes
#include "struct.h"
#include "svcommon.h"

#include <fstream> // only here to make trigger_probs happy
// C++
#include <string>

// defines
#define FILE_LINE_LENGTH 500 // max number of characters in a text file line

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class CTextTel
{

protected:
  int initial_nevents;
  int nevents;
  int initial_ninputs;
  int ninputs;
  int initial_nrules;
  int nrules;
  int initial_nord;
  int nord;
  int initial_ndisj;
  int ndisj;
  int initial_nconf;
  int nconf;

  string stateString;
  string headerString;
  string inputEventsString;
  string outputEventsString;
  string dummyEventsString;
  string rulesString;
  string orderingRulesString;
  string conflictsString;
  string mergersString;

public:
  
  CTextTel();

  CTextTel(designADT design);

  CTextTel(int numevents, eventADT *events,
	   int numinputs, int numrules, ruleADT**rules,
	   mergeADT *merges, char * startstate,
	   int numdisj, int numconf, int numord);

  CTextTel(const CTextTel& other);

  ~CTextTel();

  bool writeToFile(string filename,bool level) const;

  bool build(int numevents, eventADT *events,
	int numinputs, int numrules, ruleADT**rules,
	mergeADT *merges, char * startstate,
	int numdisj, int numconf, int numord);

  void addEvent(const string& eventName, string& eventNumber,
		bool isOutput=true);

  bool addRule(const string& enablingEventString,
	       const string& enabledEventString,
	       CRuleInfo& ruleInfo);

  bool addRule(const string& enablingEventString,
	       const string& enabledEventString,
	       char initialMarking,
	       const string& expression,
	       int lowerBound,
	       int upperBound);

  bool moveRules(const string& targetEventString,
		 const string& newEventString,  bool newIsBeforeTarget);

  void extendState(char newSignalValue);

  void inheritConflicts(const string& existingEvent, const string& newEvent);

  bool addStateVariable(CStateVariableInfo& svInfo);

}; // end class CTextTel
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#endif
