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
/* tel2vhd.c - written by Nicholas R. Seegmiller                             */
/*****************************************************************************/
/*  Notes:                                                                   */
/*                                                                           */
/*  There are 3 output functions that are not needed for tel2vhd to workk,   */
/*   and exist only for debugging purposes.  They are so noted that they are */
/*   not needed.                                                             */
/*                                                                           */
/*  TODO:                                                                    */
/*                                                                           */
/*  Write function that walks graphs and figures out the line of code order  */
/*   for everything.  Each graph node has a vector of pointers to its        */
/*   children and its parents.  Visit each node and don't set its order      */
/*   until all parents have been visited.  Each node has a visited bool for  */
/*   easy checking.                                                          */
/*                                                                           */
/*  Write function that outputs VHDL code based on the order divined by      */
/*   previous function.                                                      */
/*                                                                           */
/*****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include "misclib.h"
#include "tel2vhd.h"
#include <string>
#include <queue>
#include <list>
#include <vector>
#include <iterator>

using namespace std;

//Node for the levels
struct levelNode {
  
  //for each level every rule contained therein
  list<pair<int, int> > l;
  
  //for each level, all of the conflict groups
  list<list<pair<int, int> > > conflicts;
  
  levelNode* next;
};

//Node for the graph
struct graphNode {

  //All of the parents and children
  //(predecessors and successors) of the node
  list< graphNode* > parents;
  list< graphNode* > children;

  //The prcoess that the Node is in
  int process;
  
  //Rules in that node
  list<pair<int, int> > events;

  //if node has been visited in final walkthrough
  bool visited;

  void display() {
    for (list<pair<int, int> >::iterator eiter = events.begin();
	 eiter != events.end(); eiter++) {
      cout << "(" << eiter->first << "," << eiter->second << ")";
    }
  }
};

typedef levelNode* lvlPtr;
typedef graphNode* graphPtr;


//fixes the filename when it has been accessed remotely,
//e.g. ../examples/hse/file.hse
string fixFilename(string &filename) { 
  string newfilename;
  for (int i = filename.size()-1; i >= 0; i--) {
    if (filename[i] == '/') {
      newfilename = filename.substr(i+1, filename.size());
      break;
    }
    else if (i == 0) newfilename = filename;
  }
  return newfilename;
}

//returns the total number of processes
int numProcs(const int &nevents, eventADT *events) {
  int procs = 0;

  for (int i = 0; i < nevents; i++) {
    if (events[i]->process > procs)
      procs = events[i]->process;
  }  
  return procs + 1;
}

/*****************************************************************************/
/* This function will output to the screen the all of the information from   */
/* 2D array in the from below:                                               */
/* (i,j) expr enabling enabled [lower,upper] process epsilon                 */
/* NOT NEEDED FOR PROGRAM TO WORK                                            */
/*****************************************************************************/
void output2dInfo(eventADT *events, ruleADT **rules, const int &nevents) {  
  cout << "2D Array information" << endl
       << "------------------------------------------" << endl
       << "(i,j) expr nbling nbled [up,lw] proc eps"  << endl
       << "------------------------------------------" << endl;
  for (int i = 0; i < nevents; i++) {
    for (int j = 0; j < nevents; j++) {
      if (rules[i][j]->ruletype != NORULE) {
	cout << "(" << i << "," << j << ") "
	   << rules[i][j]->expr << " "
	   << events[i] << ' ' << events[j]
	   << " [" << rules[i][j]->lower << "," << rules[i][j]->upper << "] "
	   << events[i]->process << " " << rules[i][j]->epsilon
	   << endl;
      }
    }
  }
  cout << endl;

}

/*****************************************************************************/
/* This function will output to the screen the events that are contained     */
/* in every level and every conflict set for every process                   */
/* NOT NEEDED FOR PROGRAM TO WORK                                            */
/*****************************************************************************/
void outputLevelInfo(eventADT *events, ruleADT **rules,
		     const int &nevents, lvlPtr proclvls[]) {
  //The process loop
  for (int f = 0; f < numProcs(nevents, events); f++) {
    cout << "Process " << f << endl;
    int curlvl = 0;

    //The level loop
    for (lvlPtr temp = proclvls[f]; temp != NULL; temp = temp->next) {
      cout << "Level " << curlvl << ": "; 

      //The events loop
      for (list<pair<int, int> >::iterator i = temp->l.begin();
	   i != temp->l.end();
	   i++) {
	cout << rules[i->first][i->second]->expr << "  ";
      }

      //The conflict sets loop
      for (list< list<pair<int, int> > >::iterator n = temp->conflicts.begin();
	   n != temp->conflicts.end();
	   n++) {
	cout << " {";
	for (list<pair<int, int> >::iterator m = n->begin();
	     m != n->end();
	     m++) {
	  cout << "(" << events[m->second]->event
	       << "[" << m->first << ',' << m->second << "])";
	}
	cout << "} ";
      }
      curlvl++;
      cout << endl;
    }
    cout << endl;
  }
}

/*****************************************************************************/
/* This function will output to the screen the all the parents and children  */
/* for every node so one can see if it is working right                      */
/* NOT NEEDED FOR PROGRAM TO WORK                                            */
/*****************************************************************************/
void displayGraphs(list<graphPtr> nodes) {

  //Display crap to know I did it right
  for (list<graphPtr>::iterator viter = nodes.begin();
       viter != nodes.end();
       viter++) {
    cout << "{";
    (*viter)->display();
    cout << "}";
    for (list<graphPtr>::iterator par = (*viter)->parents.begin();
	 par != (*viter)->parents.end();
	 par++) {
      cout << " P{";
      (*par)->display();
      cout << "}";
    }
    for (list<graphPtr>::iterator chil = (*viter)->children.begin();
	 chil != (*viter)->children.end();
	   chil++) {
      cout << " C{";
      (*chil)->display();
      cout << "}";
    }
    cout << endl;
    }
}

/*****************************************************************************/
/* These three functions below are used to check and manipulate the lists    */
/* e.g. check conflicts, check existance in list, & check subsets            */
/*****************************************************************************/
//Returns true if e does not conflict with any event in F
bool checkConflict(pair<int, int> e, list<pair<int, int> > F,
		   const int &nevents, ruleADT **rules) {
  
  for (list<pair<int, int> >::iterator i = F.begin(); i != F.end(); i++) {

    //if conflict, return false;
    if (rules[e.second][i->second]->conflict ||
	rules[e.first][i->first]->conflict) return false;
  }
  return true;
}


//returns true if toFind is in list l
template <class T>
bool inList(list<T> l, T toFind) {
  for (typename list<T>::iterator i = l.begin(); i != l.end(); i++) {
    if (*i == toFind) return true;
  }
  return false;
}


//is A a subset of B
template <class T>
bool isSubset(list<T> A, list<T> B) {
  for (typename list<T>::iterator i = A.begin(); i != A.end(); i++) {
    if (!inList(B, *i)) return false;
  }
  return true;
}


/*****************************************************************************/
/* The two functions below will build the conflict sets for every level of   */
/* every process.                                                            */
/*****************************************************************************/
// This is the recursive function, it will return a list of sets, where each 
// set consists of (i,j)'s that do not conflict with each other.
list<list<pair<int, int> > > findLists(list<pair<int, int> > E,
				       list<pair<int, int> > F,
				       list< list<pair<int, int> > > p,
				       const int &nevents, ruleADT **rules) {

  if (E.empty()) {
    //(p = p union F)
    if (!F.empty()) {
      p.push_front(F);

      //if what we are adding is a subset or has subsets
      //remove the approriate list
      for (list< list<pair<int, int> > >::iterator iter = p.begin();
	   iter != p.end();
	   iter++) {
	if (iter != p.begin() && F != *iter) {
	  if (isSubset(*iter, F)) {
	    p.remove(*iter);
	  }
	  else if (isSubset(F, *iter)) {
	    p.remove(F);
	    break;
	  }
	}
      }
    }  
  }
  else {
    //E = E - e
    pair<int, int> e = E.front();
    E.pop_front();
    //if F is empty
    if (F.empty() ||
	//or e does not conflict with any event in F
	checkConflict(e, F, nevents, rules)) 
    {
      //(F = F union e)
      F.push_front(e);
      
      p = findLists(E,F,p, nevents, rules);

      //F = F - e;
      F.remove(e);
    }
    p = findLists(E,F,p, nevents, rules);
  }
  return p;
}

//This is the function that drives the above recursive function and
//simply steps through every level of every process
void findConflicts(eventADT *events, ruleADT **rules,
		   const int &nevents, lvlPtr proclvls[]) {

  list<pair<int, int> > F;
  for(int i = 0; i < numProcs(nevents, events); i++) {
    for (lvlPtr temp = proclvls[i]; temp != NULL; temp = temp->next) {
      temp->conflicts = findLists(temp->l, F, temp->conflicts,
					 nevents, rules);
    }
  }  
}

bool is_rule(ruleADT **rules,int i,int j)
{
  if ((rules[i][j]->ruletype != NORULE) &&
      (rules[i][j]->ruletype < ORDERING))
    return true;
  return false;
}

/*****************************************************************************/
/* This function does two things:                                            */
/* First of all, it has an int array of size i * nevents + j which           */
/* represents the 2D events array.  It for each spot in that "levels" array  */
/* it assigns the correct level that is appears on.                          */
/*                                                                           */
/* Secondly, it makes an array of linked lists of lists (proclevels), where  */
/* each element in the array represents a different process, each element    */
/* of the linked list represent a different level of that process, and each  */
/* list in each node contains all of the events on that particular level.    */
/*****************************************************************************/
void findLevels(eventADT *events, ruleADT **rules, const int &nevents,
		int levels[], lvlPtr proclvls[]) {

  
  //Finds the epsilons and sets them to level 0
  queue<pair<int, int> > q;
  for (int i = 0; i < nevents; i++) {
    for (int j = 0; j < nevents; j++) {
      if (is_rule(rules,i,j) &&
	  rules[i][j]->epsilon == 1) {
	levels[i * nevents + j] = 0;
	q.push(make_pair(i,j));
	proclvls[events[i]->process]->l.push_front(make_pair(i,j));
	proclvls[events[i]->process]->next = NULL;
      }    
    }
  }
  
  //Does levels for everything else
  //using a breadth first search (that is where the queue comes in)
  pair<int, int> spot;
  int level = 1;
  
  while (!q.empty()) {
    spot = q.front();
    
    //Puts it at the end of the linked list
    if (levels[spot.first * nevents + spot.second] >= level) {
      level++;
    }
    
    for (int j = 0; j < nevents; j++) {
      if (is_rule(rules,spot.second,j) &&
	  (levels[spot.second * nevents + j] == -1 ||
	   (levels[spot.second * nevents + j] < level &&
	    levels[spot.second * nevents + j] != 0))) {
	
	q.push(make_pair(spot.second,j));

	//use the max level we find the event
	if (levels[spot.second * nevents + j] != -1) {
	  int previousLevel = levels[spot.second * nevents + j];
	  int lvlmark = 0;
	  for (lvlPtr remove = proclvls[events[spot.second]->process];
	       remove != NULL;
	       remove = remove->next) {
	    if (lvlmark == previousLevel) 
	      remove->l.remove(make_pair(spot.second,j));
	    lvlmark++;
	  }
	}

	//assign the level in the array 
	levels[spot.second * nevents + j] = level;

	int templvl = 0;
	for (lvlPtr step = proclvls[events[spot.second]->process];
	     step != NULL; step = step->next) {
	  
	  if (templvl == level) {
	    step->l.push_front(make_pair(spot.second,j));
	    break;
	  }
	  
	  else if (step->next == NULL) {
	    lvlPtr last = new levelNode;
	    last->l.push_front(make_pair(spot.second,j));
	    step->next = last;
	    last->next = NULL;
	    break;
	  }
	  templvl++;
	}	
      } 
    }
    q.pop();
  }
}

/*****************************************************************************/
/* Does some error checking on the levels and makes sure it can be done      */
/*****************************************************************************/
bool errorCheck(eventADT *events, ruleADT **rules,
      const int &nevents, lvlPtr proclvls[]) {
  string lvlexpr;
  for (int i = 0; i < numProcs(nevents, events); i++) {
    for (lvlPtr step = proclvls[i]; step != NULL; step = step->next) {
      for (list<list<pair<int, int> > >::iterator iter1 =
	     step->conflicts.begin();
	   iter1 != step->conflicts.end();
	   iter1++) {
	
	//This is a bit scary, but it should work
	if (rules[iter1->begin()->first][iter1->begin()->second]->expr == NULL)
	  lvlexpr = "";
	else
	  lvlexpr = rules[iter1->begin()->first][iter1->begin()->second]->expr;

	if (iter1->size() > 1) {
	  for (list<pair<int, int> >::iterator iter = ++(iter1->begin());
	       iter != iter1->end(); iter++) {
	    string toCompare;
	    if (rules[iter->first][iter->second]->expr == NULL)
 	      toCompare == "";
	    else toCompare = rules[iter->first][iter->second]->expr;
	    cout << lvlexpr << "==" << toCompare << endl;
	    
	    if (lvlexpr != toCompare) {	      
	      cout << "Error: Unequal concurrent group level expressions" << endl;
	      return true;
	    }
	    else if (iter->first != iter1->begin()->first &&
		     iter->second != iter1->begin()->second) {
	      cout << "Error: Invalid concurrent group" << endl;
	      return true;
	    }
	  }
	}
      }
    }
  }
  return false;
}

/*****************************************************************************/
/* These functions build all of the graphs.                                  */
/*****************************************************************************/

//Returns true if A enables B
bool enables(list<pair<int, int> > A, list<pair<int, int> > B) {

  for (list<pair<int, int> >::iterator i = A.begin(); i != A.end(); i++) {
    for (list<pair<int, int> >::iterator j = B.begin(); j != B.end(); j++) {
      if (i->second == j->first) return true;
    }
  }
  return false;
  
}

//Constructs the actual graphs
void constructGraphs(eventADT *events, ruleADT **rules, 
	 const int &nevents, lvlPtr proclvls[], graphPtr roots[]) {

  
  list<graphPtr> nodes;
  int numNodes = 0;

  //For each process
  for (int i = 0; i < numProcs(nevents, events); i++) {
    //root[i]
    //For each level
    for (lvlPtr lptr = proclvls[i]; lptr != NULL; lptr = lptr->next) {
      //For each conflict set
      for (list<list<pair<int, int> > >::iterator iter =
	     lptr->conflicts.begin();
	   iter != lptr->conflicts.end(); iter++) {
	graphPtr toAdd = new graphNode;
	toAdd->events = *iter;
	toAdd->process = i;
	toAdd->visited = false;
	nodes.push_back(toAdd);
	//nodes[numNodes]->events = *iter;
	numNodes++;
      }
    }
  }

  // THIS IS BUNK
  // To find the roots iterate through the nodes list and the first
  // node for each process is the root.
  cout << "ROOTS" << endl;
  for (int i = 0; i < numProcs(nevents, events); i++) {
    //All the roots are the first set on the first level
    for (list<graphPtr>::iterator grphIter = nodes.begin();
	 grphIter != nodes.end(); grphIter++) {
      if ((*grphIter)->process==i) {
	roots[i] = (*grphIter);
	cout << "i=" << i;
	roots[i]->display();
	cout << endl;
	break;
      }
    }
  //    roots[i]->events = *(proclvls[i]->conflicts.begin());
  }

  for (list<graphPtr>::iterator grphIter = nodes.begin();
       grphIter != nodes.end();
       grphIter++) {
    for (list<graphPtr>::iterator grphIter2 = nodes.begin();
	 grphIter2 != nodes.end();
	 grphIter2++) {
      if ((*grphIter)->events == (*grphIter2)->events) continue;
      if (enables((*grphIter)->events, (*grphIter2)->events))
	(*grphIter)->children.push_back(*grphIter2);
      if (enables((*grphIter2)->events, (*grphIter)->events))
	(*grphIter)->parents.push_back(*grphIter2);
    }
  }

  //Enable function below to output graph information
  displayGraphs(nodes);

}

/*****************************************************************************/
/* This function gets all of the level and conflict info                     */
/*****************************************************************************/
void getInfo(eventADT *events, ruleADT **rules, const int &nevents,
	     int levels[], lvlPtr proclvls[], graphPtr roots[]) {

  //array that tells us if it has been visited
  //initially false
  int visited[nevents * nevents];
  for (int i = 0; i < nevents * nevents; i++)
    visited[i] = 0;

  //Assign every event to a level
  findLevels(events, rules, nevents, levels, proclvls);

  //Assign every event to a conflict group
  findConflicts(events, rules, nevents, proclvls);
  
  //Construct the graphs
  constructGraphs(events, rules, nevents, proclvls, roots);

  //Enable this function to output level and conflict group info
  outputLevelInfo(events, rules, nevents, proclvls);

}


/*****************************************************************************/
/* Convert TEL to VHDL.                                                      */
/*                                                                           */
/* Tel to V H D                                                              */
/*   This function is just splendid.                                         */
/*     I spent lots of time.                                                 */
/*****************************************************************************/
void tel2vhd(string filename, signalADT *signals, eventADT *events,
             ruleADT **rules, int ninpsig, int nsignals, const int &nevents,
	     const string &startstate)
{
  string newfilename, procname = "processA";
  int numprocs = numProcs(nevents, events);
  
  //Enable the following function to output
  //debug information for every event of the 2d array.   
  output2dInfo(events, rules, nevents);

  //array that holds the level of everything
  //initally level -1
  int levels[nevents * nevents];
  for (int i = 0; i < nevents * nevents; i++)
    levels[i] = -1;

  //This has all the good information
  lvlPtr proclvls[numprocs];
  
  for (int i = 0; i < numprocs; i++) { 
    proclvls[i] = new levelNode;
    proclvls[i]->next = NULL;
  }

  //The roots of the graph
  graphPtr roots[numprocs];
  for (int i = 0; i < numprocs; i++) { 
    roots[i] = new graphNode;
    roots[i]->visited = false;
  }
  
  getInfo(events, rules, nevents, levels, proclvls, roots);

  //Error checking
  if (errorCheck(events, rules, nevents, proclvls) == true) return;
  
  //Checks for the file and outputs that it went  
  newfilename = fixFilename(filename);

  cout << "Generating structural VHDL from tel: "
       << (filename + ".vhd") << endl;
  fprintf(lg,"Generating structural VHDL from tel:  %s\n",
	  (filename + ".vhd").c_str());

  ofstream outfile((filename + ".vhd").c_str());
  if(!outfile){
    cout << "WARNING:: Cannot Open File " << filename + ".vhd" << endl;
    return;
  }

  //ADD OUTPUT CODE HERE

}

/*****************************************************************************/
/* Convert TEL to VHDL.                                                      */
/*****************************************************************************/

void tel_two_vhd(char command,designADT design)
{ 
  if (design->status & LOADED) { 
    tel2vhd(design->filename,design->signals,design->events,design->rules,
            design->ninpsig,design->nsignals,design->nevents,design->startstate);
  }
}


