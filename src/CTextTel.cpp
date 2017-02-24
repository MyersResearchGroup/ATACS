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
 * DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "CTextTel.h"

// C++
#include <iostream>
#include <fstream>
#include <sstream>
  
/*****************************************************************************/
/* default constructor for the CTextTel                                      */
/*****************************************************************************/
CTextTel::CTextTel()
  :initial_nevents(0),nevents(0), initial_ninputs(0), ninputs(0),
   initial_nrules(0), nrules(0),
   initial_nord(0), nord(0), initial_ndisj(0),
   ndisj(0), initial_nconf(0), nconf(0)
{
}

/*****************************************************************************/
/* basic constructor for the CTextTel                                        */
/*****************************************************************************/
CTextTel::CTextTel(int numevents, eventADT *events,
		   int numinputs, int numrules, ruleADT**rules,
		   mergeADT *merges, char * startstate,
		   int numdisj, int numconf, int numord)
{
  build(numevents, events, numinputs, numrules, rules, merges, startstate,
	numdisj, numconf, numord);
}

/*****************************************************************************/
/* another basic constructor for the CTextTel                                */
/*****************************************************************************/
CTextTel::CTextTel(designADT d)
{
  build(d->nevents, d->events,
	d->ninputs, d->nrules, d->rules,
	d->merges,  d->startstate,
	d->ndisj,   d->nconf, d->nord);
}

/*****************************************************************************/
/* copy constructor for the CTextTel                                         */
/*****************************************************************************/
CTextTel::CTextTel(const CTextTel& o)
  :  initial_nevents(o.initial_nevents), nevents(o.nevents),
     initial_ninputs(o.initial_ninputs), ninputs(o.ninputs),
     initial_nrules(o.initial_nrules), nrules(o.nrules),
     initial_nord(o.initial_nord), nord(o.nord),
     initial_ndisj(o.initial_ndisj), ndisj(o.ndisj),
     initial_nconf(o.initial_nconf), nconf(o.nconf),
     stateString(o.stateString), headerString(o.headerString),
     inputEventsString(o.inputEventsString),
     outputEventsString(o.outputEventsString),
     dummyEventsString(o.dummyEventsString),
     rulesString(o.rulesString),
     orderingRulesString(o.orderingRulesString), conflictsString(o.conflictsString),
     mergersString(o.mergersString)
{
}

/*****************************************************************************/
/* write the CTextTel to a text file                                         */
/*****************************************************************************/
bool CTextTel::writeToFile(string filename, bool level) const
{
  // until the same filename should be used, do this cheap hack
  if (level)
    filename+=".tel";
  else
    filename+=".er";

  fstream outfile(filename.c_str(),ios::out); 
  if(!outfile)
  {
    cerr << "CTextTel: Couldn't create file: " << filename << endl;
    return false;
  }
  cout << "CTextTel: Storing " << ((level) ? string("TEL") : string("TER"))
       << " to file: " << filename << endl;

  // make the header string
  outfile << "#" << endl << "# " << filename << endl << "#" << endl;
  outfile << ".e " << nevents << endl;
  outfile << ".i " << ninputs << endl;
  outfile << ".r " << nrules << endl;
  outfile << ".n " << nord    << endl;
  outfile << ".d " << ndisj   << endl;
  outfile << ".c " << nconf   << endl;
  if (nevents > 1 && (stateString.size() > 0))
    outfile << ".s " << stateString << endl;

  outfile << inputEventsString; // << endl;
  outfile << outputEventsString; //<< endl; 
  outfile << dummyEventsString; //<< endl; 
  outfile << rulesString; // << endl; // may want to split initially
                          // marked and unmarked rules here ??
  outfile << orderingRulesString;// << endl;
  outfile << mergersString; // << endl;
  outfile << conflictsString;// << endl;

  outfile.close();

  return true;
}

/****************************************************************************/
/* insert an event (both + and -) into the CTextTel                         */
/****************************************************************************/
void CTextTel::addEvent(const string& signalName, string& eventNumber,
			bool isOutput)
{
  if (isOutput)
    {
      outputEventsString += (signalName + "+/" + eventNumber + ":258 "
			     + signalName + "-/" + eventNumber + ":258\n");
      nevents+=2;
    }
  else
    {
      /* TOMOHIRO: 
      inputEventsString  += (signalName + "+/" + eventNumber + ":32769 "
			     + signalName + "-/" + eventNumber + ":32769\n");
      */
      inputEventsString  += (signalName + "+/" + eventNumber + ":1 "
			     + signalName + "-/" + eventNumber + ":1\n");
      nevents+=2;
      ninputs+=2;
    }
}

/****************************************************************************/
/* extend the start state to include the new signal at its initial value    */
/****************************************************************************/
void CTextTel::extendState(char newSignalValue)
{
  stateString+= newSignalValue; 
  /* TOMOHIRO:
  stateString[stateString.length()-1]=stateString[stateString.length()-2];
  stateString[stateString.length()-2]=newSignalValue; */
}

/***************************************************************************/
/* add a rule which points to/from the new event                           */
/***************************************************************************/
bool CTextTel::addRule(const string& enablingEventString,
		       const string& enabledEventString,
		       char initialMarking,
		       const string& expression,
		       int lowerBound,
		       int upperBound)
{
  ostringstream tempBuffer;

  tempBuffer << enablingEventString
	     << " " << enabledEventString
	     << " " << expression
	     << " " << initialMarking
	     << " " << lowerBound
	     << " ";
  if (upperBound != INFIN) { tempBuffer << upperBound << endl; }
  else tempBuffer << "inf" << endl;	      

  rulesString += tempBuffer.str();

  nrules++;

  return true;
}

/***************************************************************************/
/* add a rule which points to/from the new event                           */
/***************************************************************************/
bool CTextTel::addRule(const string& enablingEventString,
		       const string& enabledEventString,
		       CRuleInfo& ruleInfo)
{
  ostringstream tempBuffer;

  tempBuffer << enablingEventString
	     << " " << enabledEventString
	     << " " << ruleInfo.expression
	     << " " << ruleInfo.initialMarking
	     << " " << ruleInfo.lowerBound
	     << " ";
  if (ruleInfo.upperBound != INFIN)
  {
    tempBuffer << " " << ruleInfo.upperBound << endl;	      
  }
  else
    tempBuffer << " inf" << endl;	      

  rulesString += tempBuffer.str();

  nrules++;

  return true;
}

/****************************************************************************/
/* move rules so they point to/from the new event                           */
/****************************************************************************/
bool CTextTel::moveRules(const string& targetEventString,
			 const string& newEventString,
			 bool newIsBeforeTarget)
{
  string adjustedRulesString;
  istringstream rulesStream;
  string ruleLine;
  bool ruleFound(false);
  unsigned int findPosition;

  if (newIsBeforeTarget) 
    // new event is before target event (as when inserting into ER)
    {
      // move all rules which enable target so that they now enable newEvent
      rulesStream.str(rulesString); // copy member rules string into stream
      //for each line in the rules string
      while(rulesStream >> ruleLine)
	{
	  findPosition=ruleLine.find(targetEventString);
	  if (findPosition != 0 && findPosition != string::npos  )
	    // if this rule enables target
	    {
	      adjustedRulesString+=ruleLine.replace(findPosition,
						    targetEventString.length(),
						    newEventString) + "\n";
	      ruleFound=true;
	    }
	  else // just copy this rule, don't adjust it
	    {
	      adjustedRulesString+=ruleLine + "\n";
	    }
	  rulesString=adjustedRulesString;
	}
    }

  else // new event is after target event (as when inserting into SR)
    {
      // move all rules which are enabled by target
      // so that they now are enabled by newEvent
      rulesStream.str(rulesString); // copy the member rules string

      //for each line in the rules string
      while(rulesStream >> ruleLine)
	{
	  if (ruleLine.find(targetEventString) == 0)
	    // if this rule is enabled by target
	    {
	      adjustedRulesString+=
		ruleLine.replace(0, targetEventString.length(),
				 newEventString) + "\n";
	      ruleFound=true;
	    }
	  else // just copy this rule, don't adjust it
	    {
	      adjustedRulesString+=ruleLine + "\n";
	    }
	  rulesString=adjustedRulesString;
	}
    }

  return ruleFound;
}


/****************************************************************************/
/* make all events which conflict with existingEvent also conflict with new */
/****************************************************************************/
void CTextTel::inheritConflicts(const string& existingEvent,
				const string& newEvent)
{
  string newConflictsString;
  string conflictLine;
  unsigned int existingEventPos;
  istringstream conflictsStream;

  // fill the stream with the string's info
  conflictsStream.str(conflictsString);

  while(conflictsStream >> conflictLine)
  {
    existingEventPos=conflictLine.find(existingEvent);
    if (existingEventPos != string::npos)
      {
	nconf++;
	newConflictsString+=
	  conflictLine.replace(existingEventPos, existingEvent.length(),
			       newEvent) + "\n";
      }
  }
  conflictsString += newConflictsString;
}

/****************************************************************************/
/* (re)fill the data structures with information from various atacs structs */
/****************************************************************************/
bool CTextTel::build(int numevents, eventADT *events,
		     int numinputs, int numrules, ruleADT**rules,
		     mergeADT *merges, char * startstate,
		     int numdisj, int numconf, int numord)
{
  int i,j;

  mergeADT curmerge;
  ostringstream tempBuffer;

  nevents=numevents;
  initial_nevents=numevents;

  ninputs=numinputs;
  initial_ninputs=numinputs;

  nrules=numrules; // this may be incorrect, due to reset rules
  initial_nrules=numrules;

  nord=numord;
  initial_nord=numord;

  ndisj=numdisj;
  initial_ndisj=numdisj;

  nconf=numconf;
  initial_nconf=numconf;

  stateString=startstate;

  // build the input events string
  tempBuffer << "#" << endl << "# INPUT/RESET EVENTS" << endl << "#" << endl;
  tempBuffer << "reset" << endl; // insert magic reset signal
  if (ninputs > 0)
  {
    for (i=1; i<=ninputs; i+=2)
    {
      tempBuffer << events[i]->event << ":" << events[i]->type << " " 
		 << events[i+1]->event << ":" << events[i+1]->type << endl;
    }
    
    // copy the buffer content into the string
    inputEventsString=tempBuffer.str();
    tempBuffer.str("");
  }

  // build the output events string
  if (nevents > ninputs + 1)
  {
    tempBuffer << "#" << endl << "# OUTPUT EVENTS" << endl << "#" << endl;
    for (i=ninputs+1; i<nevents; i+=2)
    {
      if (events[i]->event[0]!='$') {
	tempBuffer << events[i]->event << ":" << events[i]->type; 
	if (i+1 < nevents)
	  if (events[i+1]->event[0]!='$')
	    tempBuffer << " " << events[i+1]->event << ":" 
		       << events[i+1]->type << endl;
	  else
	    tempBuffer << endl;
	else
	  tempBuffer << endl;
      }
    }
    // copy the buffer content into the string
    outputEventsString=tempBuffer.str();
    tempBuffer.str("");
    
    tempBuffer << "#" << endl << "# DUMMY EVENTS" << endl << "#" << endl;
    for (i=ninputs+1; i<nevents; i++)
    {
      if (events[i]->event[0]=='$') 
	tempBuffer << events[i]->event << ":" << events[i]->type << endl;
    }
    // copy the buffer content into the string
    dummyEventsString=tempBuffer.str();
    tempBuffer.str("");
  }

  // build the rules string
  if (nrules > 0)
  {
    tempBuffer << "#" << endl << "# RULES" << endl << "#" << endl;
    int num_found_rules(0);
    for (i=0;  i < nevents; i++)
      for(j=0; j < nevents; j++)
      {
	if ((rules[i][j]->ruletype != NORULE) &&
	    ((rules[i][j]->ruletype & TRIGGER) ||
	     (rules[i][j]->ruletype & PERSISTENCE)))
	{
	  num_found_rules++;
	  tempBuffer << events[i]->event << " " << events[j]->event;
	  
	  if (rules[i][j]->expr != NULL)
	    tempBuffer << " " << rules[i][j]->expr; 
	  tempBuffer << " " << rules[i][j]->epsilon;
	  tempBuffer << " " << rules[i][j]->lower;
	  if (rules[i][j]->upper != INFIN)
	  {
	    tempBuffer << " " << rules[i][j]->upper << endl;	      
	  }
	  else
	    tempBuffer << " inf" << endl;	      
	}
      }
    rulesString=tempBuffer.str(); // copy the buffer content into the string
    tempBuffer.str("");
    nrules = num_found_rules; // correct nrules, in case it was wrong
  }

  // build the ordering rules string
  if (nord > 0)
  {
    tempBuffer << "#" << endl << "# ORDERING RULES" << endl << "#" << endl;
    for (i=0;  i < nevents; i++)
	for(j=0; j < nevents; j++)
	{
	  if ((rules[i][j]->ruletype != NORULE) &&
	      (rules[i][j]->ruletype & ORDERING))
	    // && rules[i][j]->epsilon == 0)
	    // can ord rules be initially marked??
	  {
	    tempBuffer << events[i]->event << " " << events[j]->event;
	    tempBuffer << " " << rules[i][j]->expr; // this is for tel only
	    tempBuffer << " " << rules[i][j]->epsilon;
	    tempBuffer << " " << rules[i][j]->lower;
	    if (rules[i][j]->upper != INFIN)
	    {
	      tempBuffer << " " << rules[i][j]->upper << endl;	      
	    }
	    else
	      tempBuffer << " inf" << endl;	      
	  }
	}
    // copy the buffer content into the string
    orderingRulesString=tempBuffer.str(); 
    tempBuffer.str("");
  }
  
  // build the mergers string
  if (ndisj > 0)
  {
    tempBuffer << "#" << endl << "# MERGERS" << endl << "#" << endl;
    for (i=1;i<nevents;i++)
    {
      for (curmerge=merges[i]; curmerge != NULL; curmerge=curmerge->link)
	if ((curmerge->mergetype==CONJMERGE) || 
	    (curmerge->mergetype==DISJMERGE)) 
	  if (curmerge->mevent > i)
	  {
	    tempBuffer << events[i]->event << " "
		       << events[curmerge->mevent]->event << endl;
	  }
    }
    // copy the buffer content into the string
    mergersString = tempBuffer.str(); 
    tempBuffer.str("");
  }

  // build the conflicts string
  if (nconf > 0)
  {
    tempBuffer << "#" << endl << "# CONFLICTS" << endl << "#" << endl;
    for (i=0;  i < nevents; i++)
      for(j=i+1; j < nevents; j++)
      {
	if (rules[i][j]->conflict)
	{
	  tempBuffer << events[i]->event << " "
		     << events[j]->event << endl;
	}
      }
    // copy the buffer content into the string
    conflictsString=tempBuffer.str(); 
    tempBuffer.str("");;
  }
  
  return true;
}

/****************************************************************************/
/* should insert a state signal, and add the transitions, etc               */
/****************************************************************************/
bool CTextTel::addStateVariable(CStateVariableInfo& svInfo)
{
  bool success=true;
  string newEventString;

  // extend the startstate to include another signal
  extendState(svInfo.initialValue);

  // add event to the output event list
  /* TOMOHIRO:
  addEvent(svInfo.signalName, svInfo.instance,false); */
  addEvent(svInfo.signalName, svInfo.instance);

  // first handle the + transition insertion
  newEventString=svInfo.signalName + "+/" + svInfo.instance;

  // add rules going TO the CSC+ transition
  for (rules_cont::iterator i_riseStarters=
	 svInfo.rulesToRise.begin();
       i_riseStarters != svInfo.rulesToRise.end();
       i_riseStarters++)
    {
      success= success && addRule(i_riseStarters->event->event,
				  newEventString,
				  *i_riseStarters);
    }
  // add rules going FROM the CSC+ transition
  for (rules_cont::iterator i_riseEnders=
	 svInfo.rulesFromRise.begin();
       i_riseEnders != svInfo.rulesFromRise.end();
       i_riseEnders++)
    {
      success = success && addRule(newEventString,
				   (*i_riseEnders).event->event,
				   *i_riseEnders);
    }


  // now handle the - transition
  newEventString=svInfo.signalName + "-/" + svInfo.instance;

  // add rules going TO the CSC- transition
  for (rules_cont::iterator i_fallStarters=
	 svInfo.rulesToFall.begin();
       i_fallStarters != svInfo.rulesToFall.end();
       i_fallStarters++)
    {
      success= success && addRule((*i_fallStarters).event->event,
				  newEventString,
				  *i_fallStarters);
    }
  // add rules going FROM the CSC- transition
  for (rules_cont::iterator i_fallEnders=
	 svInfo.rulesFromFall.begin();
       i_fallEnders != svInfo.rulesFromFall.end();
       i_fallEnders++)
    {
      success = success && addRule(newEventString,
				   (*i_fallEnders).event->event,
				   *i_fallEnders);
    }
  
  return success;
}

/****************************************************************************/
/* destructor                                                               */
/****************************************************************************/
CTextTel::~CTextTel()
{
  // no dynamic memory explicitly allocated, so no explicit destruction needed
}
