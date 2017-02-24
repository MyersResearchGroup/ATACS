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
/****************************************************************************/

#include <stdlib.h>
#include "struct.h"
#include "lpntrsfm.h"
#include "memaloc.h"
#include "printlpn.h"
#include "def.h"
#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <sys/time.h>
#include <string>
#include "displayADT.h"
#include "loadg.h"
#include "lhpnrsg.h"

#define __XFRM_DATA__

// #define __EXPAND_DEBUG__

level_exp CopyLevel(level_exp level)
{
  level_exp copy=NULL;
  level_exp curlevel;
  level_exp newlevel=NULL;

  for (curlevel=level;curlevel;curlevel=curlevel->next) {
    newlevel=(level_exp)GetBlock(sizeof(*newlevel));
    newlevel->product=CopyString(curlevel->product);
    newlevel->next=copy;
    copy=newlevel;
  }
  return copy;
}

bool is_hybrid(designADT design) {
  for (int i = 0; i < design->nevents + design->nplaces ; i++) {
    if ((design->events[i]->type & CONT) &&
	!(design->events[i]->type & VAR)) {
      return true;
    }
  }
  return false;
}

//////////////////////
// Applies all possible transformations to the Petri net until no
// further transforms can be applied.  The larger transforms are
// applied first so that they have more to work with.  Before
// transforms are applied, the net is expanded to make implicit places
// no implicit.  After transformations, the net is compressed by
// removing places that are no longer visited and by making places
// implicit if possible.  
bool transform(designADT design)
{
  if (is_hybrid(design)) {
    printf("WARNING: Petri net contains continuous components, ");
    printf("so transforms not applied.\n");
    fprintf(lg, "WARNING: Petri net contains continuous components, ");
    fprintf(lg, "so transforms not applied.\n");
    return false;
  }
  if (design->verbose) {
    printf("Performing Petri net transformations.\n");
    fprintf(lg,"Performing Petri net transformations.\n");
    printf("Initial net size: ");
    fprintf(lg, "Initial net size: ");
    print_size(design);
  }

  // Check constraints first so that any generated warning messages
  // are more meaningful to the user.
  timeval t1;
  gettimeofday(&t1, NULL);
  check_constraints(design);

  // 9/8/05 - Commented out below because the expansion was moved to
  // the outsize of transform such that all designs are expanded by default.
  //#ifndef EXPANDNET
  int oldsize = (design->nevents + design->nplaces)*
    (design->nevents + design->nplaces);
  expand_design(design);
  //#endif

  // Remove the constraints while transforms are applied.  They
  // will be added back in when we are done.  This MUST
  // be done after call to expand_design so that indexes will
  // be correct when constraints are added back i n.
  remove_constraints(design);
#ifdef __XFRM_DATA__
  printf("Initial net size: ");
  print_size(design);
  bool modified = false;
  int removeableTrans = 0;
  for (int i = 1; i < design->nevents; i++) {
    if ((!(design->events[i]->dropped)) &&
	(design->events[i]->type & ABSTRAC) &&
	(!(design->events[i]->hsl)||
	 (strcmp(design->events[i]->hsl,"~fail")==0))&&
	(!(design->events[i]->inequalities))) {
      removeableTrans++;
    }
  }
  printf("removableTrans: %i\n", removeableTrans);
#endif
  do {
    modified = false;
    // Transforms which merge two transitions into one
    /*
    for (int i = 1; i < design->nevents; i++) {
      if ((design->events[i]->dropped) || 
          !(design->events[i]->type & ABSTRAC)) {
        continue;
      }
      for (int j = 1; j < design->nevents; j++) {
        if ((design->events[j]->dropped) || 
            !(design->events[j]->type & ABSTRAC)) {
          continue;
        }
        if (xform5a(design->events, design->rules, design->markkey,
                    design->nevents, design->nplaces, design->ndummy,
                    i,j,design->nsignals, design->verbose)) {
          modified = true;
        }
        else if (xform5b(design->events, design->rules, design->markkey,
                         design->nevents, design->nplaces, design->ndummy,
                         i,j,design->nsignals, design->verbose)) {
          modified = true;
        }
        else if (!design->level) {
          if (xform6(design->events, design->rules, design->markkey,
                     design->nevents, design->nplaces, design->ndummy,
                     i,j,design->nsignals, design->verbose)) {
            modified = true;
          }
        }
        else if (xform7(design->events, design->rules, design->markkey,
                        design->nevents, design->nplaces, design->ndummy,
                        i,j,design->nsignals, design->verbose)) {
          modified = true;
        }
      }
    }
    */
    // Transforms which remove a transition
    for (int i = 1; i < design->nevents; i++) {
      if ((design->events[i]->dropped) || 
          !(design->events[i]->type & ABSTRAC) ||
	  ((design->events[i]->hsl)&&
	   (strcmp(design->events[i]->hsl,"~fail")!=0))||
	  (design->events[i]->inequalities)) {
        continue;
      }
      if (xform3a(design->events, design->rules, design->markkey,
                  design->nevents, design->nplaces, design->ndummy, i,
                  design->nsignals, design->verbose)) {
        modified = true;
      }
      else if (design->xform2 &&
	       xform4a(design->events, design->rules, design->markkey,
                       design->nevents, design->nplaces, design->ndummy, i,
                       design->nsignals, design->verbose)) {
        modified = true;
      }
      else if (xform3b(design->events, design->rules, design->markkey,
                       design->nevents, design->nplaces, design->ndummy, i,
                       design->nsignals, design->verbose)) {
        modified = true;
      }
      else if (design->xform2 &&
	       xform4b(design->events, design->rules, design->markkey,
                       design->nevents, design->nplaces, design->ndummy, i,
                       design->nsignals, design->verbose)) {
        modified = true;
      }
      else if (xform3c(design->events, design->rules, design->markkey,
                       design->nevents, design->nplaces, design->ndummy, i,
                       design->nsignals, design->verbose)) {
        modified = true;
      }
      else if (design->xform2 &&
	       xform4c(design->events, design->rules, design->markkey,
                       design->nevents, design->nplaces, design->ndummy, i,
                       design->nsignals, design->verbose)) {
        modified = true;
      }
    }
    
    // Transforms which remove no transitions
    if (xform0(design->events, design->rules, design->markkey,
               design->nevents, design->nplaces, design->ndummy,
               design->verbose)) {
      modified = true;
    }
    else if (design->xform2 &&
	     xform1(design->events, design->rules, design->markkey,
                    design->nevents, design->nplaces, design->ndummy,
                    design->verbose)) {
      modified = true;
    }
    /*
    else if (xform2(design->events, design->rules, design->markkey,
      design->nevents, design->nplaces, design->ndummy, design->verbose)) {
      modified = true;
    }
    */
    
  } while (modified);

  // Add constraints back in.  This MUST be done before compressing
  // design to ensure that indexes are correct.
  replace_constraints(design);

  // 9/8/05 - Removed compress
  //#ifndef EXPANDNET
  //compress_design(design);
  make_markkey(design,oldsize);
  //#endif
  timeval t2;
  gettimeofday(&t2, NULL);

  double time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  double time2 = (t2.tv_sec+(t2.tv_usec*.000001));
    
  if (design->verbose) {
    printf("Done performing Petri net transformations.\n");
    fprintf(lg,"Done performing Petri net transformations.\n");
    printf("Final net size: ");
    fprintf(lg, "Final net size: ");
    print_size(design);
    printf("Total transformation time: %f\n", time2-time1);
    fprintf(lg, "Total transformation time: %f\n", time2-time1);
  }
#ifdef __XFRM_DATA__
  printf("Final net size: ");
  print_size(design);
  printf("Total transformation time: %f\n", time2-time1);
  return true;
#endif
}

////////////////////
// Ensures that abstractable events are not connected by constraints.
// If they are, it makes the event non-abstractable.
void check_constraints(designADT design) {
  for (int t1 = 0; t1 < design->nevents; t1++) {
    for (int t2 = t1+1; t2 < design->nevents; t2++) {
      if ((design->rules[t1][t2]->ruletype & ORDERING) ||
          (design->rules[t2][t1]->ruletype & ORDERING)) {
        if (design->verbose) {
          if (design->events[t1]->type & ABSTRAC) {
            printf("Making %s non-abstractable", design->events[t1]->event);
            printf(" because it is part of a constraint rule.\n");
            fprintf(lg,"Making %s non-abstractable",design->events[t1]->event);
            fprintf(lg, " because it is part of a constraint rule.\n");
          }
          if (design->events[t2]->type & ABSTRAC) {
            printf("Making %s non-abstractable", design->events[t2]->event);
            printf(" because it is part of a constraint rule.\n");
            fprintf(lg,"Making %s non-abstractable",design->events[t2]->event);
            fprintf(lg, " because it is part of a constraint rule.\n");
          }
        }
        design->events[t1]->type &= ~ABSTRAC;
        design->events[t2]->type &= ~ABSTRAC;
      }
    }
  }
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    for (int t1 = 0; t1 < design->nevents; t1++) {
      if (design->rules[p][t1]->ruletype & ORDERING) {
        if (design->verbose) {
          if (design->events[t1]->type & ABSTRAC) {
            printf("Making %s non-abstractable", design->events[t1]->event);
            printf(" because it is part of a constraint rule.\n");
            fprintf(lg,"Making %s non-abstractable",design->events[t1]->event);
            fprintf(lg, " because it is part of a constraint rule.\n");
          }
	}
	design->events[t1]->type &= ~ABSTRAC;
	for (int t2 = 0; t2 < design->nevents; t2++) {
	  if (design->rules[t2][p]->ruletype != NORULE) {
	    if (design->verbose) {
	      if (design->events[t1]->type & ABSTRAC) {
		printf("Making %s non-abstractable",design->events[t2]->event);
		printf(" because it is part of a constraint rule.\n");
		fprintf(lg,"Making %s non-abstractable",
			design->events[t2]->event);
		fprintf(lg, " because it is part of a constraint rule.\n");
	      }
	    }
	    design->events[t2]->type &= ~ABSTRAC;
	  }
	}
      }
    }
  }
}


void remove_constraints(designADT design) {
  fix_constraints(design, true);
}

void replace_constraints(designADT design) {
  fix_constraints(design, false);
}

//////////////////
// THIS FUNCTION SHOULD NOT BE CALLED BY ANYTHING OTHER THAN
// remove_constraints AND replace_constraints.
//
// The first time this function is called, the constraint rules
// are removed from the design and stored.
// The second time that this function is called, the stored
// constraint rules are restored back into the design.
void fix_constraints(designADT design, bool remove) {
  static vector<int> fromIndex;
  static vector<int> toIndex;
  static vector<int> ruletype;

  if (remove) {
    if (design->verbose) {
      printf("Temporarily removing ordering rules.\n");
      fprintf(lg,"Temporarily removing ordering rules.\n");
    }
    for (int i = 0; i < design->nevents+design->nplaces; i++) {
      for (int j = i+1; j < design->nevents+design->nplaces; j++) {
        if (design->rules[i][j]->ruletype & ORDERING) {
          fromIndex.push_back(i);
          toIndex.push_back(j);
          ruletype.push_back(design->rules[i][j]->ruletype);
        }
      }
    }
  }
  if (!remove) {
    if (design->verbose) {
      printf("Adding ordering rules back in.\n");
      fprintf(lg,"Adding ordering rules back in.\n");
    }
    vector<int>::iterator from = fromIndex.begin();
    vector<int>::iterator to = toIndex.begin();
    vector<int>::iterator rtype = ruletype.begin();
    while (from != fromIndex.end()) {
      design->rules[*from][*to]->ruletype = *rtype;
      from++;
      to++;
      rtype++;
    }
  }
}


////////////////////
// Prints out basic information about the petri-net.  
void print_size(designADT design) {

  // Number of transitions
  int ntrans = 0;
  for (int i = 1; i < design->nevents; i++) {
    if (!(design->events[i]->dropped)) {
      ntrans++;
    }
  }

  // Number of places
  int implicitPlaces = 0;
  int rules = 0;
  for (int t1 = 0; t1 < design->nevents; t1++) {
    for (int t2 = 0; t2 < design->nevents; t2++) {
      if (design->rules[t1][t2]->ruletype != NORULE) {
        implicitPlaces++;
        rules+=2;
      }
    }
  }
    
  // Number of rules
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    for (int t = 0; t < design->nevents; t++) {
      if (design->rules[t][p]->ruletype != NORULE) {
        rules++;
      }
      if (design->rules[p][t]->ruletype != NORULE) {
        rules++;
      }
    }
  }
  printf("places=%i trans=%i rules=%i\n", design->nplaces+implicitPlaces,
         ntrans, rules);
  fprintf(lg, "places=%i trans=%i rules=%i\n", design->nplaces+implicitPlaces,
         ntrans, rules);
}

//////////
// Takes a ruleADT datastructure and expands it such that each implicit
// places becomes explicit.  In other words, after this function is 
// executed, the resulting ruleADT data structure will have no
// entries in the nevents by nevents portion of the matrix.
bool expand_design(designADT design)
{
  // Iterate through the first quadrant of the matrix and count
  // the number of implicit places.
  int implicitPlaces = 0;
  for (int i = 0; i < design->nevents; i++) {
    for (int j = 0; j < design->nevents; j++) {
      if (design->rules[i][j]->ruletype != NORULE) {
        implicitPlaces++;
      }
    }
  }

  if (design->verbose) {
    printf("Expanding design: Making %i implicit places explicit.\n", implicitPlaces);
    fprintf(lg, "Expanding design: Making %i implicit places explicit.\n", implicitPlaces);
  }
  
  // Create a new ruleADT structure with 'implicitPlaces' additional
  // places.
  if (implicitPlaces != 0) {
    // Use new_rules to allocate new memory without deleting the
    // existing structure
    ruleADT **newRules = new_rules(RESET, FALSE, 0,design->nevents+
                                   design->nplaces+implicitPlaces, NULL);
    // Copy everything from old rules to new one.
    for (int i = 0; i < design->nevents+design->nplaces; i++) {
      for (int j = 0; j < design->nevents+design->nplaces; j++) {
        memcpy(newRules[i][j], design->rules[i][j],
               sizeof(*(design->rules[i][j])));
      }
    }

    // Create new eventADT structure & copy over.
    eventADT *newEvents = new_events(RESET, FALSE, 0, design->nevents+
                                     design->nplaces+implicitPlaces, NULL);
    for (int i = 0; i < design->nevents+design->nplaces; i++) {
      memcpy(newEvents[i], design->events[i], sizeof(*newEvents[i]));
    }
    // Iterate through the first nevent x nevents of the matrix and
    // create the new place
    int next = design->nevents+design->nplaces;
    int label = 0;
    for (int i = 0; i < design->nevents; i++) {
      for (int j = 0; j < design->nevents; j++) {
        if (newRules[i][j]->ruletype != NORULE) {
          memcpy(newRules[i][next], newRules[i][j], sizeof(*newRules[i][j]));
          memcpy(newRules[next][j], newRules[i][j], sizeof(*newRules[i][j]));
          //printf("%s -%d-> %s\n",newEvents[i]->event,
          //       newRules[i][j]->epsilon,newEvents[j]->event);
          newRules[i][next]->level = NULL;
          newRules[i][next]->POS = NULL;
          newRules[i][next]->expr = NULL;
          newRules[i][next]->dist = NULL;
          newRules[i][next]->epsilon = 0;
          newRules[i][j]->ruletype = NORULE;
          newRules[i][j]->level = NULL;
          newRules[i][j]->POS = NULL;
          newRules[i][j]->expr = NULL;
          newRules[i][j]->dist = NULL;
          newEvents[next]->event = (char*) GetBlock(8 * sizeof(char));
          sprintf(newEvents[next]->event, "ip%i", label);
          //printf("CHANGED TO\n");
          //printf("%s -%d-> %s\n",newEvents[i]->event,
          //       newRules[i][next]->epsilon,newEvents[next]->event);
          //printf("%s -%d-> %s\n",newEvents[next]->event,
          //       newRules[next][j]->epsilon,newEvents[j]->event);
          next++;
          label++;
        }
      }
    }
    // NULL out the pointers in the old rulesADT and eventsADT
    // structures.  Don't want new_rules and new_events to de-allocate
    // that memory because the new rulesADT and eventsADT structures
    // still uses it.
    for (int i = 0; i < design->nevents + design->nplaces; i++) {
      for (int j = 0; j < design->nevents + design->nplaces; j++) {
        design->rules[i][j]->level = NULL;
        design->rules[i][j]->POS = NULL;
        design->rules[i][j]->expr = NULL;
        design->rules[i][j]->dist = NULL;
      }
    }
    new_rules(LOADED, TRUE, design->nevents+design->nplaces, 0, design->rules);
    for (int i = 0; i < design->nevents+design->nplaces; i++) {
      design->events[i]->event = NULL;
      design->events[i]->data = NULL;
    }
    new_events(LOADED, TRUE, design->nevents+design->nplaces,0,design->events);
    // Update design structure to point to new memory.
    design->rules = newRules;
    design->events = newEvents;
    design->nplaces += implicitPlaces;

    return true;
  }
  else {
    return false;
  }
}

void compress_design(designADT design)
{
  // Iterate throught the places.  Find all places with one ingoing
  // edge and one outgoing edge and convert it to an implicit place.
  // Keep track of which places can be removed in the remove array.

  bool remove[design->nevents+design->nplaces];
  for (int i = 0; i < design->nevents+design->nplaces; i++) {
    remove[i] = false;
  }
  int extraEvents = 0;
  int extraDummy = 0;

  //(element 0 should be left alone)
  for (int t = 1; t < design->nevents; t++) {
    int toCount = 0;
    int fromCount = 0;
    for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
      if (design->rules[t][p]->ruletype != NORULE) {
        fromCount++;
      }
      if (design->rules[p][t]->ruletype != NORULE) {
        toCount++;
      }
    }
    if (toCount == 0 && fromCount == 0) {
      extraEvents++;
      if (design->events[t]->type & DUMMY) extraDummy++;
      remove[t] = true;
    }
  }
  
  int extraPlaces = 0;
  for (int i = design->nevents; i < design->nevents+design->nplaces; i++) {
    int toCount = 0;
    int fromCount = 0;
    int toIndex = 0;
    int fromIndex = 0;
    for (int j = 0; j < design->nevents; j++) {
      if (design->rules[i][j]->ruletype != NORULE) {
        fromCount++;
        fromIndex = j;
      }
      if (design->rules[j][i]->ruletype != NORULE) {
        toCount++;
        toIndex = j;
      }
    }
    if (toCount == 0 && fromCount == 0) {
      extraPlaces++;
      remove[i] = true;
    }
    else if (toCount == 1 && fromCount == 1) {
      extraPlaces++;
      remove[i] = true;
      memcpy(design->rules[toIndex][fromIndex], design->rules[i][fromIndex],
             sizeof(*(design->rules[i][fromIndex])));
      design->rules[i][fromIndex]->level = NULL;
      design->rules[i][fromIndex]->POS = NULL;
      design->rules[i][fromIndex]->expr = NULL;
      design->rules[i][fromIndex]->dist = NULL;
      design->rules[toIndex][i]->level = NULL;
      design->rules[toIndex][i]->POS = NULL;
      design->rules[toIndex][i]->expr = NULL;
      design->rules[toIndex][i]->dist = NULL;
    }
  }
  if (design->verbose) {
    printf("Compressing design: Removing %i extra places.\n", extraPlaces);
    fprintf(lg, "Compressing design: Removing %i extra places.\n", extraPlaces);
  }

  // If there aren't any extra places, we can stop here.
  if (extraEvents == 0 && extraPlaces == 0) {
    return;
  }

  // Allocate new memory to store the new rulesADT and eventsADT
  ruleADT **newRules = new_rules(RESET, FALSE, 0,design->nevents-extraEvents+
                                 design->nplaces-extraPlaces, NULL);
  eventADT *newEvents =new_events(RESET, FALSE, 0, design->nevents-extraEvents+
                                   design->nplaces-extraPlaces, NULL);
  
  // Copy everything except for the removed place's rows and columns
  // from old rules to new one and NULL out necessary pointers so that
  // when deallocating old rulesADT and eventsADT we don't lose
  // necessary data.
  int newi = 0;
  for (int i = 0; i < design->nevents+design->nplaces; i++) {
    if (remove[i] == true)
      continue;
    int newj = 0;
    for (int j = 0; j < design->nevents+design->nplaces; j++) {
      if (remove[j] == true) {
        continue;
      }
      memcpy(newRules[newi][newj], design->rules[i][j],
             sizeof(*(design->rules[i][j])));
      design->rules[i][j]->level = NULL;
      design->rules[i][j]->POS = NULL;
      design->rules[i][j]->expr = NULL;
      design->rules[i][j]->dist = NULL;
      newj++;
    }
    newi++;
  }
  
  newi = 0;
  for (int i = 0; i < design->nevents+design->nplaces; i++) {
    if (remove[i] == true) {
      continue;
    }
    memcpy(newEvents[newi], design->events[i], sizeof(*design->events[i]));
    design->events[i]->event = NULL;
    design->events[i]->data = NULL;
    if (design->events[i]->signal >= 0)
      design->signals[design->events[i]->signal]->event=newi;
    newi++;
  }
  
  // De-allocate old memory.
  new_rules(LOADED, TRUE, design->nevents+design->nplaces, 0, design->rules);
  new_events(LOADED, TRUE, design->nevents+design->nplaces, 0, design->events);

  // Update design structure to point to new memory.
  design->rules = newRules;
  design->events = newEvents;
  design->nevents -= extraEvents;
  design->ndummy -= extraDummy;
  design->nplaces -= extraPlaces;
}

void make_markkey(designADT design,int oldsize) {
  // Count the number of rules.
  int nrules = 0;
  for (int i = 0; i < design->nevents+design->nplaces; i++) {
    for (int j = 0; j < design->nevents+design->nplaces; j++) {
      if (design->rules[i][j]->ruletype != NORULE)
        nrules++;
    }
  }

  // Re-allocate memory.
  design->markkey = new_markkey(design->status, FALSE,
                                oldsize, (design->nevents+design->nplaces)
                                *(design->nevents+design->nplaces), 
                                design->markkey);
  design->nrules = nrules;

  // Populate new markkey
  int r = 0;
  for (int i = 0; i < design->nevents+design->nplaces; i++) {
    for (int j = 0; j < design->nevents+design->nplaces; j++) {
      if (design->rules[i][j]->ruletype != NORULE) {
        design->markkey[r]->enabling = i;
        design->markkey[r]->enabled = j;
        design->markkey[r]->epsilon = design->rules[i][j]->epsilon;
        design->rules[i][j]->marking = r;
        r++;
      }
    }
  }
    
}

level_exp true_level(int nsignals)
{
  level_exp trueLevel=(level_exp)GetBlock(sizeof(*trueLevel));
  trueLevel->next=NULL;
  trueLevel->product=(char *)GetBlock((nsignals+1)*sizeof(char));
  for (int k=0;k<nsignals;k++)
    trueLevel->product[k]='X';
  trueLevel->product[nsignals]='\0';
  return trueLevel;
}

// Merge parallel places.  The places must have the same preset and
// postset.  
bool xform0(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy, bool verbose) {
  bool modified = false;
  for (int p1 = nevents; p1 < nevents+nplaces; p1++) {
    if (events[p1]->dropped) {
      continue;
    }
    for (int p2 = p1 + 1; p2 < nevents+nplaces; p2++) {
      if (events[p2]->dropped) {
        continue;
      }
      bool okay = true;
      for (int t = 0; t < nevents; t++) {
        if (events[t]->dropped) {
          continue;
        }
        if (rules[p1][t]->epsilon != rules[p2][t]->epsilon ||
            rules[t][p1]->ruletype != rules[t][p2]->ruletype ||
            rules[p1][t]->ruletype != rules[p2][t]->ruletype) {
          okay = false;
          break;
        }
      }
      if (okay) {
        if (verbose) {
          printf("Transformation 0: Merging places %s and %s.\n",
                 events[p1]->event, events[p2]->event);
          fprintf(lg, "Transformation 0: Merging places %s and %s.\n",
                  events[p1]->event, events[p2]->event);
        }
        modified = true;
        events[p2]->dropped = true;
        for (int t = 0; t < nevents; t++) {
          rules[t][p2]->ruletype = NORULE;
          rules[p2][t]->ruletype = NORULE;
          if (rules[p1][t]->ruletype != NORULE) {
            if (rules[p2][t]->lower > rules[p1][t]->lower) {
              rules[p1][t]->lower = rules[p2][t]->lower;
            }
            if (rules[p2][t]->upper > rules[p1][t]->upper) {
              rules[p1][t]->upper = rules[p2][t]->upper;
            }
          }
        }
      }
    }
  }
  return modified;
}

// Remove self loop.  A place p has an edge to a transition t and from
// transition t.  p has no other transitions in its preset or
// postset.  t has other place(s) int is preset besides p.
bool xform1(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy, bool verbose) {

  bool modified=false;
  
  // Find a place whose postset and preset are the same single
  // transition.
  for (int pIndex = nevents; pIndex < nevents+nplaces; pIndex++) {
    for (int tIndex = 0; tIndex < nevents; tIndex++) {
      if (!events[pIndex]->dropped &&
          rules[pIndex][tIndex]->epsilon == 1 &&  // p must be marked
          rules[pIndex][tIndex]->ruletype != NORULE &&
          rules[tIndex][pIndex]->ruletype != NORULE) {
        // A loop was found. Make sure additional requirements are satisfied.
        bool removable = false;
        // Make sure t has at least one other place in its preset
        // besides p.
        for (int i = nevents; i < nevents+nplaces; i++) {
          if (rules[i][tIndex]->ruletype != NORULE && i != pIndex) {
            removable = true;
            break;
          }
        }
        // Make sure t has at least one other place in its postset
        // besides p.
        if (removable) {
          removable = false;
          for (int i = nevents; i < nevents+nplaces; i++) {
            if (rules[tIndex][i]->ruletype != NORULE && i != pIndex) {
              removable = true;
              break;
            }
          }
        }
        // Make sure p has no other transitions in its preset or
        // postset
        for (int t = 0; t < nevents; t++) {
          if (tIndex != t && (rules[pIndex][t]->ruletype != NORULE ||
                              rules[t][pIndex]->ruletype != NORULE)) {
            removable = false;
            break;
          }
        }
        // If the place can be removed, do it.
        if (removable) {
          if (verbose) {
            printf("Transformation #1: Removing place %s.\n",
                   events[pIndex]->event);
            fprintf(lg, "Transformation #1: Removing place %s.\n",
                    events[pIndex]->event);
          }
          // Remove place.
          rules[pIndex][tIndex]->ruletype = NORULE;
          rules[tIndex][pIndex]->ruletype = NORULE;
          events[pIndex]->dropped = true;
          // Update transitions so the upperbounds of the places in
          // the preset of t is the max of the current upperbound and
          // the upperbound of the removed place. 
          for (int p = nevents; p < nevents+nplaces; p++) {
            if (rules[p][tIndex]->ruletype != NORULE &&
                rules[p][tIndex]->upper < rules[pIndex][tIndex]->upper) {
              rules[p][tIndex]->upper = rules[pIndex][tIndex]->upper;
            }
          }
          modified=true;
        }
      }
    }
  }
  return modified;
}

/*****************************
This transformation is bogus.
Don't use!
*******************************/
// Remove a loop with a transition.
bool xform2(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy, bool verbose) {
  for (int tIndex = 0; tIndex < nevents; tIndex++) {
    int toCount = 0;
    int fromCount = 0;
    int p1Index = -1;
    int p2Index = -1;
    for (int p = nevents; p < nevents+nplaces; p++) {
      if (rules[p][tIndex]->ruletype != NORULE) {
        toCount++;
        p2Index = p;
      }
      if (rules[tIndex][p]->ruletype != NORULE) {
        fromCount++;
        p1Index = p;
      }
    }
    if (!events[tIndex]->dropped && toCount == 1 && fromCount == 1) {
      bool removable = false;
      int t2;
      for (t2 = 0; t2 < nevents; t2++) {
        if (rules[p1Index][t2]->ruletype != NORULE &&
            rules[t2][p2Index]->ruletype != NORULE) {
          removable = true;
          break;
        }
      }
      if (removable) {
        if (verbose) {
          printf("Transformation #2: Removing transition %s.\n",
                 events[tIndex]->event);
          fprintf(lg, "Transformation #2: Removing transition %s.\n",
                  events[tIndex]->event);
        }
        for (int t4 = 0; t4 < nevents; t4++) {
          // slight error here... if not all sum of uppers are greater
          // than 0, probably shouldn't perform transformation at all.
          if (rules[p2Index][t4]->ruletype != NORULE &&
              rules[p1Index][t2]->upper + rules[p2Index][t4]->upper > 0)
            rules[p2Index][t4]->upper = INFIN;
        }
        events[tIndex]->dropped = true;
        rules[tIndex][p1Index]->ruletype = NORULE;
        rules[p2Index][tIndex]->ruletype = NORULE;
        return true;
      }
    }  }
  return false;
}


// Remove a transition t with a single place p in the postset.  For each
// place p in preset(t), the size of postset(p) must equal 1.  For the place p
// in postset(t), the size of preset(p) must equal 1.
// Given a transition, if necessary properties are met, perform transformation.
bool xform3a(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose) {

  if (events[tIndex]->dropped) {
    return false;
  }
  // There must be a single place p in the postset of t.
  int pCount = 0;
  int pIndex = -1;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      pCount++;
      pIndex = p;
    }
  }
  if (/*pIndex != -1 && */pCount != 1) {
    return false;
  }
  // For each place p in preset(t), verify that the size of
  // postset(p) is 1.  There must also be at least one place
  // in preset(t).
  int count = 0;
  bool preMarked = false;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      count++;
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE && t != tIndex) {
          return false;
        }
      }
      if (rules[p][tIndex]->epsilon == 1) {
        preMarked = true;
      }
    }
  }

  if (count == 0) {
    return false;
  }
  // For the place p in postset(t), verify that the size of
  // preset(p) is 1.  There must also be at least one transition
  // in postset(p).
  count = 0;
  bool postMarked = false;
  if (pIndex != -1) {
    for (int t = 0; t < nevents; t++) {
      if (rules[t][pIndex]->ruletype != NORULE && t != tIndex) {
	return false;
      }
      if (rules[pIndex][t]->ruletype != NORULE) {
	count++;
	if (rules[pIndex][t]->epsilon == 1) {
	  postMarked = true;
	}
      }
    }
  }
  if (postMarked && preMarked) {
    return false;
  }
  if (pIndex != -1 && count == 0) {
    return false;
  }
  if (verbose) {
    printf("Transformation #3a: Removing transition %s.\n",
           events[tIndex]->event);
    fprintf(lg, "Transformation #3a: Removing transition %s.\n",
           events[tIndex]->event);
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      // Remove edge from this place to t.
      rules[p][tIndex]->ruletype = NORULE;
      if (pIndex != -1) {
	for (int t = 0; t < nevents; t++) {
	  if (rules[pIndex][t]->ruletype != NORULE) {
	    rules[p][t]->ruletype = TRIGGER;
	    rules[p][t]->ruletype |= SPECIFIED;
	    if (postMarked) rules[p][t]->epsilon = 1;
	    else 
	      rules[p][t]->epsilon=rules[p][tIndex]->epsilon;
	    if (pIndex != -1) {
	      if ((rules[p][tIndex]->upper==INFIN)||
		  (rules[pIndex][t]->upper==INFIN))
		rules[p][t]->upper=INFIN;
	      else
		rules[p][t]->upper = rules[p][tIndex]->upper +
		  rules[pIndex][t]->upper;
	      if (postMarked) {
		rules[p][t]->lower = rules[pIndex][t]->lower;
	      } else {
		rules[p][t]->lower = rules[p][tIndex]->lower +
		  rules[pIndex][t]->lower;
	      }
	      rules[p][t]->rate = rules[p][tIndex]->rate *
		rules[pIndex][t]->rate;
	    }
	    /* Assumes level expression is on fanin rule */
	    if (rules[p][tIndex]->expr) {
	      rules[p][t]->expr = CopyString(rules[p][tIndex]->expr);
	    } else {
	      rules[p][t]->expr = NULL;
	    }
	    rules[p][t]->level = CopyLevel(rules[p][tIndex]->level);
	    rules[p][t]->POS = CopyLevel(rules[p][tIndex]->POS);
	  }
	}
      } else {
	for (int t = 0; t < nevents; t++) 
	  if (rules[t][p]->ruletype != NORULE) 
	    rules[t][p]->ruletype = NORULE;
      }
    }
  }
  for (int t = 0; t < nevents; t++) {
    if (rules[pIndex][t]->ruletype != NORULE) {
      if (!postMarked) {
	events[t]->lower = events[t]->lower + events[tIndex]->lower;
      }
      if ((events[t]->upper == INFIN)||(events[tIndex]->upper == INFIN)) {
	events[t]->upper = INFIN;
      } else {
	events[t]->upper = events[t]->upper + events[tIndex]->upper;
      }
    }
  }
  // Remove edges between p (postset(t)) and postset(p).
  if (pIndex != -1) {
    for (int t = 0; t < nevents; t++) {
      rules[pIndex][t]->ruletype = NORULE;
      rules[t][pIndex]->ruletype = NORULE;
    }
    if (verbose) {
      printf("\tRemoving place %s.\n", events[pIndex]->event);
      fprintf(lg, "\tRemoving place %s.\n",events[pIndex]->event);
    }
    events[pIndex]->dropped = true;
  } 
  events[tIndex]->dropped = true;
  return true;
}



// Variation of Transformation 3 where each place p in preset(t) does
// not have to equal 1.
// Remove a transition t with a single place p in the postset.  For the place p
// in postset(t), the size of preset(p) must equal 1.
// Given a transition, if necessary properties are met, perform transformation.
// Either all the places before t can be marked
// or the place after t can be marked, but not both.
bool xform3b(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose) {

  if (events[tIndex]->dropped) {
    return false;
  }
  // There must be a single place p in the postset of t.
  int pCount = 0;
  int pIndex = -1;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      pCount++;
      pIndex = p;
    }
  }
  if (pCount != 1) {
    return false;
  }
  // For each place p in preset(t), verify that the size of
  // postset(p) is 1.  There must also be at least one place
  // in preset(t).
  int count = 0;
  bool preMarked = false;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      count++;
      if (rules[p][tIndex]->epsilon == 1) {
        preMarked = true;;
      }
    }
  }
  if (count == 0) {
    return false;
  }
  // For the place p in postset(t), verify that the size of
  // preset(p) is 1.  There must also be at least one transition
  // in postset(p).
  count = 0;
  bool postMarked = false;
  for (int t = 0; t < nevents; t++) {
    if (rules[t][pIndex]->ruletype != NORULE && t != tIndex) {
      return false;
    }
    if (rules[pIndex][t]->ruletype != NORULE) {
      count++;
      if (rules[pIndex][t]->epsilon == 1) {
        postMarked = true;
      }
    }
  }
  if (postMarked && preMarked) {
    return false;
  }
  if (count == 0) {
    return false;
  }
  if (verbose) {
    printf("Transformation #3b: Removing transition %s.\n",
           events[tIndex]->event);
    fprintf(lg, "Transformation #3b: Removing transition %s.\n",
            events[tIndex]->event);
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype & TRIGGER) {
      // Remove edge from this place to t.
      rules[p][tIndex]->ruletype = NORULE;
      for (int t = 0; t < nevents; t++) {
        if (rules[pIndex][t]->ruletype != NORULE) {
          rules[p][t]->ruletype = TRIGGER;
          rules[p][t]->ruletype |= SPECIFIED;
          if (postMarked) rules[p][t]->epsilon = 1;
          else 
            rules[p][t]->epsilon=rules[p][tIndex]->epsilon;
          if ((rules[p][tIndex]->upper==INFIN)||
              (rules[pIndex][t]->upper==INFIN))
            rules[p][t]->upper=INFIN;
          else
            rules[p][t]->upper = rules[p][tIndex]->upper +
              rules[pIndex][t]->upper;
	  if (postMarked) {
	    rules[p][t]->lower = rules[pIndex][t]->lower;
	  } else {
	    rules[p][t]->lower = rules[p][tIndex]->lower +
	      rules[pIndex][t]->lower;
	  }
          rules[p][t]->rate = rules[p][tIndex]->rate *
            rules[pIndex][t]->rate;
	  /* Assumes level expression is on fanin rule */
	  rules[p][t]->expr = CopyString(rules[p][tIndex]->expr);
          rules[p][t]->level = CopyLevel(rules[p][tIndex]->level);
          rules[p][t]->POS   = CopyLevel(rules[p][tIndex]->POS);
        }
        if ((postMarked) && 
            (rules[p][t]->ruletype != NORULE)) {
          rules[p][t]->epsilon=1;
        }
      }
    }
  }
  // Remove edges between p (postset(t)) and postset(p).
  for (int t = 0; t < nevents; t++) {
    if (rules[pIndex][t]->ruletype != NORULE) {
      rules[pIndex][t]->ruletype = NORULE;
    }
  }
  rules[tIndex][pIndex]->ruletype = NORULE;
  events[tIndex]->dropped = true;
  events[pIndex]->dropped = true;
  return true;
}

// Variation of transformation 3 where the place in the postset of t
// can have a preset larger than 1.
// Remove a transition t with a single place p in the postset.  For each
// place p in preset(t), the size of postset(p) must equal 1.
// Given a transition, if necessary properties are met, perform transformation.
// Either all the places before t can be marked
// or the place after t can be marked, but not both.
bool xform3c(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose) {

  if (events[tIndex]->dropped) {
    return false;
  }
  // There must be a single place p in the postset of t.
  int pCount = 0;
  int pIndex = -1;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      pCount++;
      pIndex = p;
    }
  }
  if (pCount != 1) {
    return false;
  }
  // For each place p in preset(t), verify that the size of
  // postset(p) is 1.  There must also be at least one place
  // in preset(t).
  int count = 0;
  bool preMarked = false;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      count++;
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE && t != tIndex) {
          return false;
        }
      }
      if (rules[p][tIndex]->epsilon == 1) {
        preMarked = true;
      }
    }
  }
  if (count == 0) {
    return false;
  }
  // For the place p in postset(t), verify that the size of
  // preset(p) is 1.  There must also be at least one transition
  // in postset(p).
  count = 0;
  bool postMarked = false;
  for (int t = 0; t < nevents; t++) {
    if (rules[pIndex][t]->ruletype != NORULE) {
      count++;
      if (rules[pIndex][t]->epsilon == 1) {
        postMarked = true;
      }
    }
  }
  if (preMarked && postMarked) {
    return false;
  }
  if (count == 0) {
    return false;
  }
  if (verbose) {
    printf("Transformation #3c: Removing transition %s.\n",
           events[tIndex]->event);
    fprintf(lg, "Transformation #3c: Removing transition %s.\n",
            events[tIndex]->event);
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      for (int t = 0; t < nevents; t++) {
        if (rules[pIndex][t]->ruletype != NORULE) {
          rules[p][t]->ruletype = TRIGGER;
          rules[p][t]->ruletype |= SPECIFIED;
          if (postMarked) rules[p][t]->epsilon = 1;
          else 
            rules[p][t]->epsilon=rules[p][tIndex]->epsilon;
          if ((rules[p][tIndex]->upper==INFIN)||
              (rules[pIndex][t]->upper==INFIN))
            rules[p][t]->upper=INFIN;
          else
            rules[p][t]->upper = rules[p][tIndex]->upper +
              rules[pIndex][t]->upper;

          rules[p][t]->rate = rules[p][tIndex]->rate *
            rules[pIndex][t]->rate;
	  /* Assumes level expression is on fanin rule */
	  rules[p][t]->expr = CopyString(rules[p][tIndex]->expr);
          rules[p][t]->level = CopyLevel(rules[p][tIndex]->level);
          rules[p][t]->POS = CopyLevel(rules[p][tIndex]->POS);
          //rules[p][t]->lower = rules[p][tIndex]->lower +
          //  rules[pIndex][t]->lower;
        }
      }
    }
  }
  // Remove edges between p (postset(t)) and postset(p).  Add edges
  // between preset(p) and preset(t).
  for (int t = 0; t < nevents; t++) {
    if (rules[t][pIndex]->ruletype != NORULE && t != tIndex) {
      for (int p = nevents; p < nevents+nplaces; p++) {
        if (rules[p][tIndex]->ruletype != NORULE) {
          rules[t][p]->ruletype = TRIGGER;
          rules[t][p]->ruletype |= SPECIFIED;
          rules[t][p]->epsilon=0;
        }
      }
    }
    rules[t][pIndex]->ruletype = NORULE;	 
    rules[pIndex][t]->ruletype = NORULE;
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    rules[p][tIndex]->ruletype = NORULE;
  }
  rules[tIndex][pIndex]->ruletype = NORULE;
  events[tIndex]->dropped = true;
  events[pIndex]->dropped = true;
  return true;
}


// Remove a transition with a single place in the preset.
bool xform4a(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose) {
  if (events[tIndex]->dropped) {
    return false;
  }

  // There must be a single place p in preset(t)
  int pCount = 0;
  int pIndex = -1;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      pCount++;
      pIndex = p;
    }
  }

  if (pCount != 1) {
    return false;
  }
  
  // If the place in preset(t) is marked, the places in the
  // postset(t) cannot be marked.  
  bool preMarked = false;
  bool postMarked = false;
  // Verify that the size of postset(p) is equal to 1.  the size
  // of the preset(p) must also be at least 1.
  int count = 0;
  for (int t = 0; t < nevents; t++) {
    if (t != tIndex && rules[pIndex][t]->ruletype != NORULE) {
      return false;
    }
    if (rules[t][pIndex]->ruletype != NORULE) {
      count++;
    }
  }
  if (count == 0) {
    return false;
  }
  if (rules[pIndex][tIndex]->epsilon==1) {
    preMarked = true;
  }
  // Verify that the sizes of the presets for each place in the
  // postset of t are 1. The size of the postset(t) must be
  // at least 1. Also check the markings of postset(t).
  count = 0;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      count++;
      for (int t = 0; t < nevents; t++) {
        if (rules[t][p]->ruletype != NORULE && t != tIndex) {
          return false;
        }
        if (rules[p][t]->ruletype != NORULE && rules[p][t]->epsilon==1) {
          postMarked = true;
        }
      }
    }
  }
  if (count == 0) {
    return false;
  }
  if (postMarked /*&& preMarked*/) {
    return false;
  }

  // None of the transitions in the preset(t) should be in the 
  // postset(postset(t)).
  for (int i=0;i<nevents;i++)
    if (rules[i][pIndex]->ruletype != NORULE) 
      for (int j=nevents;j<nevents+nplaces;j++) 
        if (rules[tIndex][j]->ruletype != NORULE)
          for (int k=0;k<nevents;k++)
            if ((rules[j][k]->ruletype != NORULE) && (i==k)) return false;
	      
  // If transition at tIndex is removable, remove it.
  if (verbose) {
    printf("Transformation #4a: Removing transition %s.\n",
           events[tIndex]->event);
    fprintf(lg, "Transformation #4a: Removing transition %s.\n",
            events[tIndex]->event);
  }
  // Adding new edges & removing old edges.
  rules[pIndex][tIndex]->ruletype = NORULE;
  events[pIndex]->dropped = true;
  events[tIndex]->dropped = true;
  for (int t = 0; t < nevents; t++) {
    if (rules[t][pIndex]->ruletype != NORULE) {
      rules[t][pIndex]->ruletype = NORULE;
      for (int p = nevents; p < nevents+nplaces; p++) {
        if (rules[tIndex][p]->ruletype != NORULE) {
          rules[t][p]->ruletype = TRIGGER;
          rules[t][p]->ruletype |= SPECIFIED;
          for (int t1 = 0; t1 < nevents; t1++) {
            if (rules[p][t1]->ruletype != NORULE) {
              if ((rules[p][t1]->upper==INFIN)||
                  (rules[pIndex][tIndex]->upper==INFIN))
                rules[p][t1]->upper=INFIN;
              else
                rules[p][t1]->upper += rules[pIndex][tIndex]->upper;
	      if (!postMarked)
		rules[p][t1]->lower += rules[pIndex][tIndex]->lower;
              if (preMarked) rules[p][t1]->epsilon = 1;
            }
          }
        }
      }
    }
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      rules[tIndex][p]->ruletype = NORULE;
    }
  }
  return true;
}

// Remove a transition with a single place in the preset
// Variation of Transformation 4 where more than one place can be in
// the ppstset of the preset(t)..
bool xform4b(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose) {
  if (events[tIndex]->dropped) {
    return false;
  }

  // There must be a single place p in preset(t)
  int pCount = 0;
  int pIndex = -1;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      pCount++;
      pIndex = p;
    }
  }

  if (pCount != 1) {
    return false;
  }
  
  // If the place in preset(t) is marked, the places in the
  // postset(t) cannot be marked.  
  bool preMarked = false;
  bool postMarked = false;
  // The size of the preset(p) must also be at least 1.
  int count = 0;
  for (int t = 0; t < nevents; t++) {
    if (rules[t][pIndex]->ruletype != NORULE) {
      count++;
    }
  }
  if (count == 0) {
    return false;
  }
  if (rules[pIndex][tIndex]->epsilon > 0) {
    preMarked = true;
  }
  // Verify that the sizes of the presets for each place in the
  // postset of t are 1. The size of the postset(t) must be
  // at least 1. Also check the markings of postset(t).
  count = 0;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      count++;
      for (int t = 0; t < nevents; t++) {
        if (rules[t][p]->ruletype != NORULE && t != tIndex) {
          return false;
        }
        if ( rules[p][t]->ruletype != NORULE && rules[p][t]->epsilon > 0) {
          postMarked = true;
        }
      }
    }
  }
  if (count == 0) {
    return false;
  }
  if (postMarked && preMarked) {
    return false;
  }

  // None of the transitions in the preset(t) should be in the 
  // postset(postset(t)).
  for (int i=0;i<nevents;i++)
    if (rules[i][pIndex]->ruletype != NORULE) 
      for (int j=nevents;j<nevents+nplaces;j++) 
        if (rules[tIndex][j]->ruletype != NORULE)
          for (int k=0;k<nevents;k++)
            if ((rules[j][k]->ruletype != NORULE) && (i==k)) return false;
  
  // If transition at tIndex is removable, remove it.
  if (verbose) {
    printf("Transformation #4b: Removing transition %s.\n",
           events[tIndex]->event);
    fprintf(lg, "Transformation #4b: Removing transition %s.\n",
            events[tIndex]->event);
  }
  // Adding new edges & removing old edges.
  for (int t = 0; t < nevents; t++) {
    if (t != tIndex && rules[pIndex][t]->ruletype != NORULE) {
      for (int p = nevents; p < nevents+nplaces; p++) {
        if (rules[tIndex][p]->ruletype != NORULE) {
          rules[p][t]->ruletype = TRIGGER;
          rules[p][t]->ruletype |= SPECIFIED;
          rules[p][t]->level = true_level(nsignals);
        }
      }
      rules[pIndex][t]->ruletype = NORULE;
    }
  }
  for (int t = 0; t < nevents; t++) {
    if (rules[t][pIndex]->ruletype != NORULE) {
      rules[t][pIndex]->ruletype = NORULE;
      for (int p = nevents; p < nevents+nplaces; p++) {
        if (rules[tIndex][p]->ruletype != NORULE) {
          rules[t][p]->ruletype = TRIGGER;
          rules[t][p]->ruletype |= SPECIFIED;
          for (int t1 = 0; t1 < nevents; t1++) {
            if (rules[p][t1]->ruletype != NORULE) {
              if ((rules[p][t1]->upper==INFIN)||
                  (rules[pIndex][tIndex]->upper==INFIN))
                rules[p][t1]->upper=INFIN;
              else
                rules[p][t1]->upper += rules[pIndex][tIndex]->upper;
	      if (!postMarked) 
		rules[p][t1]->lower += rules[pIndex][tIndex]->lower;
              if (preMarked) rules[p][t1]->epsilon = 1;
            }
          }
        }
      }
    }
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      rules[tIndex][p]->ruletype = NORULE;
    }
  }
  rules[pIndex][tIndex]->ruletype = NORULE;
  events[pIndex]->dropped = true;
  events[tIndex]->dropped = true;
  return true;
}

// Remove a transition with a single place in the preset.  Same as
// transform 4 except that the restriction where the size of the
// preset of each place in the postset of t does not have to be one.
bool xform4c(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose) {
  if (events[tIndex]->dropped) {
    return false;
  }

  // There must be a single place p in preset(t)
  int pCount = 0;
  int pIndex = -1;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][tIndex]->ruletype != NORULE) {
      pCount++;
      pIndex = p;
    }
  }

  if (pCount != 1) {
    return false;
  }
  
  // If the place in preset(t) is marked, the places in the
  // postset(t) cannot be marked.  
  bool preMarked = false;
  bool postMarked = false;
  // Verify that the size of postset(p) is equal to 1.  the size
  // of the preset(p) must also be at least 1.
  int count = 0;
  for (int t = 0; t < nevents; t++) {
    if (t != tIndex && rules[pIndex][t]->ruletype != NORULE) {
      return false;
    }
    if (rules[t][pIndex]->ruletype != NORULE) {
      count++;
    }
  }
  if (count == 0) {
    return false;
  }
  if (rules[pIndex][tIndex]->epsilon > 0) {
    preMarked = true;
  }
  // The size of the postset(t) must be
  // at least 1. Also check the markings of postset(t).
  count = 0;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      count++;
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE && rules[p][t]->epsilon > 0) {
          postMarked = true;
        }
      }
    }
  }
  if (count == 0) {
    return false;
  }
  if (postMarked && preMarked) {
    return false;
  }

  // None of the transitions in the preset(t) should be in the 
  // postset(postset(t)).
  for (int i=0;i<nevents;i++)
    if (rules[i][pIndex]->ruletype != NORULE) 
      for (int j=nevents;j<nevents+nplaces;j++) 
        if (rules[tIndex][j]->ruletype != NORULE)
          for (int k=0;k<nevents;k++)
            if ((rules[j][k]->ruletype != NORULE) && (i==k)) return false;
  
  // If transition at tIndex is removable, remove it.
  if (verbose) {
    printf("Transformation #4c: Removing transition %s.\n",
           events[tIndex]->event);
    fprintf(lg, "Transformation #4c: Removing transition %s.\n",
            events[tIndex]->event);
  }
  // Adding new edges & removing old edges.
  rules[pIndex][tIndex]->ruletype = NORULE;
  events[pIndex]->dropped = true;
  events[tIndex]->dropped = true;
  for (int t = 0; t < nevents; t++) {
    if (rules[t][pIndex]->ruletype != NORULE) {
      rules[t][pIndex]->ruletype = NORULE;
      for (int p = nevents; p < nevents+nplaces; p++) {
        if (rules[tIndex][p]->ruletype != NORULE) {
          rules[t][p]->ruletype = TRIGGER;
          rules[t][p]->ruletype |= SPECIFIED;
          for (int t1 = 0; t1 < nevents; t1++) {
            if (rules[p][t1]->ruletype != NORULE) {
              if ((rules[p][t1]->upper==INFIN)||
                  (rules[pIndex][tIndex]->upper==INFIN))
                rules[p][t1]->upper=INFIN;
              else
                rules[p][t1]->upper += rules[pIndex][tIndex]->upper;
              //rules[p][t1]->lower += rules[pIndex][tIndex]->lower;
              if (preMarked) rules[p][t1]->epsilon = 1;
            }
          }
        }
      }
    }
  }
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[tIndex][p]->ruletype != NORULE) {
      rules[tIndex][p]->ruletype = NORULE;
    }
  }
  return true;
}

// Merge transitions with the same preset and postset.  The sizes of
// the preset and postset must be at least 1.
bool xform5a(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy,
             int t1Index, int t2Index,int nsignals, bool verbose) {
  if (t1Index == t2Index ||
      events[t1Index]->dropped || events[t2Index]->dropped) {
    return false;
  }
  int preCount = 0;       // size of preset at least one?
  int postCount = 0;      // size of postset at least on e?
  // Verify that presets and postsets  of p are the same.
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[t1Index][p]->ruletype != rules[t2Index][p]->ruletype ||
        rules[p][t1Index]->ruletype != rules[p][t2Index]->ruletype) {
      return false;
    }
    if (rules[p][t1Index]->ruletype != NORULE) {
      preCount++;
    }
    if (rules[t1Index][p]->ruletype != NORULE) {
      postCount++;
    }
  }
  if (preCount == 0 || postCount == 0) {
    return false;
  }
  if (verbose) {
    printf("Transformation #5a: Merging transitions %s and %s.\n",
           events[t1Index]->event, events[t2Index]->event);
    fprintf(lg, "Transformation #5a: Merging transitions %s and %s.\n",
            events[t1Index]->event, events[t2Index]->event);
  }
  // The new merged transition will be the transition at
  // t1Index.  The transition at t2Index will be removed.
  for (int p = nevents; p < nevents+nplaces; p++) {
    rules[t2Index][p]->ruletype = NORULE;
    rules[p][t2Index]->ruletype = NORULE;
    if (rules[t1Index][p]->lower > rules[t2Index][p]->lower) {
      rules[t1Index][p]->lower = rules[t2Index][p]->lower;
    }
    if (rules[t1Index][p]->upper < rules[t2Index][p]->upper) {
      rules[t1Index][p]->upper = rules[t2Index][p]->upper;
    }
  }
  events[t2Index]->dropped = true;
  return true;
}

bool xform5b(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy,
             int t1Index, int t2Index,int nsignals, bool verbose) {
//   cout << "xform5b " << events[t1Index]->event << " "
//        << events[t2Index]->event << endl;

  if (t1Index == t2Index || events[t1Index]->dropped ||
      events[t2Index]->dropped) {
    return false;
  }
  set<int> pre1;
  set<int> pre2;
  set<int> prepre1;
  set<int> prepre2;
  set<int> post1;
  set<int> post2;
  set<int> postpost1;
  set<int> postpost2;
  for (int p = nevents; p < nevents+nplaces; p++) {
    // preset
    if (rules[p][t1Index]->ruletype != NORULE) {
      pre1.insert(p);
      for (int t = 0; t < nevents; t++) {
        if (rules[t][p]->ruletype != NORULE) {
          prepre1.insert(t);
        }
      }
    }
    if (rules[p][t2Index]->ruletype != NORULE) {
      pre2.insert(p);
      for (int t = 0; t < nevents; t++) {
        if (rules[t][p]->ruletype != NORULE) {
          prepre2.insert(t);
        }
      }
    }
    // postset
    if (rules[t1Index][p]->ruletype != NORULE) {
      post1.insert(p);
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE) {
          postpost1.insert(t);
        }
      }
    }
    if (rules[t2Index][p]->ruletype != NORULE) {
      post2.insert(p);
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE) {
          postpost2.insert(t);
        }
      }
    }
  }

  if (prepre1 != prepre2 || postpost1 != postpost2) {
    return false;
  }
  // Do all places in preset of t1Index/t2Index have same timing
  // contrainsts entering each transition. 
  for (set<int>::iterator ppi = prepre1.begin(); ppi != prepre1.end(); ppi++) {
    for (set<int>::iterator pi1 = pre1.begin(); pi1 != pre1.end(); pi1++) {
      for (set<int>::iterator pi2 = pre2.begin(); pi2 != pre2.end(); pi2++) {
        if (rules[*ppi][*pi1]->ruletype != NORULE &&
            rules[*ppi][*pi2]->ruletype != NORULE) {
          if (rules[*pi1][t1Index]->lower != rules[*pi2][t2Index]->lower ||
              rules[*pi1][t1Index]->upper != rules[*pi2][t2Index]->upper) {
            return false;
          }     
        }
      }
    }
  }

  set<int> pre;
  set_union(pre1.begin(), pre1.end(), pre2.begin(), pre2.end(),
            insert_iterator<set<int> > (pre, pre.begin()));
  set<int> post;
  set_union(post1.begin(), post1.end(), post2.begin(), post2.end(),
            insert_iterator<set<int> > (post, post.begin()));
  
  // Timing constraints on each place in the postset of
  // t1Index/t2Index should be the same.
  for (set<int>::iterator ppi = postpost1.begin();
       ppi != postpost1.end(); ppi++) {
    int l = -1;
    int u = -1;
    for (int p = nevents; p < nevents+nplaces; p++) {
      if (post.end() != post.find(p) && rules[p][*ppi]->ruletype != NORULE) {
        if (l == -1 || u == -1) {
          l = rules[p][*ppi]->lower;
          u = rules[p][*ppi]->upper;
        }
        else {
          if (l != rules[p][*ppi]->lower || u != rules[p][*ppi]->upper) {
            return false;
          }
        }
      }
    }
  }
  
  if (verbose) {
    printf("Transformation #5b: Merging transitions %s and %s.\n",
           events[t1Index]->event, events[t2Index]->event);
    fprintf(lg, "Transformation #5b: Merging transitions %s and %s.\n",
            events[t1Index]->event, events[t2Index]->event);
  }

  // Perform the merge.  t1Index becomes the new merged transition.            
  events[t2Index]->dropped = true;
  for (set<int>::iterator i = pre.begin(); i != pre.end(); i++) {
    int p = *i; 
    if (rules[p][t2Index]->ruletype != NORULE) {
      rules[p][t1Index]->ruletype = rules[p][t2Index]->ruletype;
      rules[p][t1Index]->lower = rules[p][t2Index]->lower;
      rules[p][t1Index]->upper = rules[p][t2Index]->upper;
      rules[p][t1Index]->epsilon = rules[p][t2Index]->epsilon;
      rules[p][t1Index]->level = true_level(nsignals);
      rules[p][t2Index]->ruletype = NORULE;
    }
  }
  for (set<int>::iterator i = post.begin(); i != post.end(); i++) {
    int p = *i;
    if (rules[t2Index][p]->ruletype != NORULE) {
      rules[t1Index][p]->ruletype = rules[t2Index][p]->ruletype;
      rules[t1Index][p]->lower = rules[t2Index][p]->lower;
      rules[t1Index][p]->upper = rules[t2Index][p]->upper;
      rules[t1Index][p]->epsilon = rules[t2Index][p]->epsilon;
      rules[t1Index][p]->level = NULL;
      rules[t2Index][p]->ruletype = NORULE;
    }
  }
  return true;
}


// Merge transitions t1 and t2 with the same preset.  The places in
// the postset of t1 and t2 must have the same presets.
// there must be at most one place in the postsets of t1 and
// t2.
bool xform6(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy,
            int t1Index, int t2Index,int nsignals, bool verbose) {
  if (t1Index == t2Index ||
      events[t1Index]->dropped || events[t2Index]->dropped) {
    return false;
  }
  int preCount = 0;  // size of preset at least one?
  int post1Count = 0; // size of postset(t1) at least one?
  int p1Index = -1;
  int post2Count = 0; // size of postset(t2) at least one?
  int p2Index = -2;
  int post1Marked = 0;
  int post2Marked = 0;
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][t1Index]->ruletype != rules[p][t2Index]->ruletype) {
      return false;
    }
    if (rules[p][t1Index]->ruletype != NORULE) {
      preCount++;
    }
    if (rules[t1Index][p]->ruletype != NORULE) {
      post1Count++;
      p1Index = p;
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE) {
          if (rules[p][t]->epsilon > 0) {
            post1Marked = 1;
          }
        }
      }
    }
    if (rules[t2Index][p]->ruletype != NORULE) {
      post2Count++;
      p2Index = p;
      // Verify timing constraints.
      for (int t = 0; t < nevents; t++) {
        if (rules[p][t]->ruletype != NORULE) {
          if (rules[p][t]->epsilon > 0) {
            post2Marked = 1;
          }
        }
      }
    }
  }
  if (preCount==0 || post1Count != 1 || post2Count != 1 ) {
    return false;
  }
  if (post1Marked != post2Marked) {
    return false;
  }
  // check if the places in the postsets of t1 and t2 have the
  // same presets.
  for (int t = 0; t < nevents; t++) {
    if (t != t1Index && t != t2Index &&
        rules[t][p1Index]->ruletype != rules[t][p2Index]->ruletype) {
      return false;
    }
  }

  if (verbose) {
    printf("Transformation #6: Merging transitions %s and %s.\n",
           events[t1Index]->event, events[t2Index]->event);
    fprintf(lg, "Transformation #6: Merging transitions %s and %s.\n",
            events[t1Index]->event, events[t2Index]->event);    
  }
  // The new merged transition will be the transition at
  // t1Index.  The transition at t2Index will be removed.
  for (int p = nevents; p < nevents+nplaces; p++) {
    if (rules[p][t1Index]->lower > rules[p][t2Index]->lower) {
      rules[p][t1Index]->lower = rules[p][t2Index]->lower;
    }
    if (rules[p][t1Index]->upper < rules[p][t2Index]->upper) {
      rules[p][t1Index]->upper = rules[p][t2Index]->upper;
    }
    rules[p][t2Index]->ruletype = NORULE;
    rules[t2Index][p]->ruletype = NORULE;
  }
  events[t2Index]->dropped = true;
  // if p1 and p2 aren't the same, merge them into p1.  p2 will be removed.
  if (p1Index != p2Index) {
    for (int t = 0; t < nevents; t++) {
      if (rules[p2Index][t]->ruletype != NORULE) {
        rules[p1Index][t]->ruletype = TRIGGER;
        rules[p1Index][t]->ruletype |= SPECIFIED;
        rules[p1Index][t]->lower = rules[p2Index][t]->lower;
        rules[p1Index][t]->upper = rules[p2Index][t]->upper;
        rules[p1Index][t]->level = true_level(nsignals);
        rules[p2Index][t]->ruletype = NORULE;
      }
      rules[t][p2Index]->ruletype = NORULE;
    }
    events[p2Index]->dropped = true;
  }
  return true;
}

// Merge transitions (t1 and t2) with the same postset.  The presets of
// t1 and t2  must be of size 1 and have the same postsets (besides
// t1 and t2).
bool xform7(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy,
            int t1Index, int t2Index,int nsignals, bool verbose) {
  if (t1Index == t2Index) {
    return false;
  }
  int postCount = 0;  // size of preset at least one?
  int pre1Count = 0; // size of preset(t1) at least one?
  int p1Index = -1;
  int pre2Count = 0; // size of preset(t2) at least one?
  int p2Index = -2;
  int pre1Marked = 0;
  int pre2Marked = 0;
  for (int p = nevents; p < nevents+nplaces; p++) {
    // t1 & t2 must have smae postsets.
    if (rules[t1Index][p]->ruletype != rules[t2Index][p]->ruletype) {
      return false;
    }
    // Keep track of bounds, marking, and number of places in preset
    // of t1.
    if (rules[p][t1Index]->ruletype != NORULE) {
      pre1Count++;
      p1Index = p;
      // Marking
      if (rules[p][t1Index]->epsilon > 0) {
        pre1Marked = 1;
      }
    }
    // Keep track of bounds, marking, and number of places in preset
    // of t2.
    if (rules[p][t2Index]->ruletype != NORULE) {
      pre2Count++;
      p2Index = p;
      // Marking
      if (rules[p][t2Index]->epsilon > 0) {
        pre2Marked = 1;
      }
    }
  }
  if (postCount==0 || pre1Count != 1 || pre2Count != 1 ) {
    return false;
  }
  if (pre1Marked != pre2Marked) {
    return false;
  }
  // check if the places in the presets of t1 and t2 have the
  // same postsets.
  for (int t = 0; t < nevents; t++) {
    if (t != t1Index && t != t2Index &&
        rules[p1Index][t]->ruletype != rules[p2Index][t]->ruletype) {
      return false;
    }
  }

  if (verbose) {
    printf("Transformation #7: Merging transitions %s and %s.\n",
           events[t1Index]->event, events[t2Index]->event);
    fprintf(lg, "Transformation #7: Merging transitions %s and %s.\n",
            events[t1Index]->event, events[t2Index]->event);
  }
  // The new merged transition will be the transition at
  // t1Index.  The transition at t2Index will be removed.
  for (int p = nevents; p < nevents+nplaces; p++) {
    rules[p][t2Index]->ruletype = NORULE;
    rules[t2Index][p]->ruletype = NORULE;
  }
  events[t2Index]->dropped = true;
  // if p1 and p2 aren't the same, merge them into p1.  p2 will be removed.
  if (p1Index != p2Index) {
    for (int t = 0; t < nevents; t++) {
      if (rules[t][p2Index]->ruletype != NORULE) {
        rules[t][p1Index]->ruletype = TRIGGER;
        rules[t][p1Index]->ruletype |= SPECIFIED;
        rules[t][p2Index]->ruletype = NORULE;
      }
      if (rules[p2Index][t]->ruletype != NORULE) {
        if (rules[p2Index][t]->lower < rules[p1Index][t]->lower) {
          rules[p1Index][t]->lower = rules[p2Index][t]->lower;
        }
        if (rules[p2Index][t]->upper > rules[p1Index][t]->upper) {
          rules[p1Index][t]->upper = rules[p2Index][t]->upper;
        }
      }
      rules[p2Index][t]->ruletype = NORULE;
    }
    events[p2Index]->dropped = true;
  }
  return true;  
}

/****************************************************************************
 * Takes and lhpn with ranges of rates and produces and lhpn with
 * piecewise linear rates. 
 ****************************************************************************/
bool expandRate(designADT design)
{
  int position,token;
  char tokvalue[MAXTOKEN];

#ifdef __EXPAND_DEBUG__
  printRuleADT(design->rules,design->nevents,design->nplaces,design->events);
  printVerboseEventADT(design->events,design->nevents,design->nplaces,
                       design->signals,design->nsignals);
  printMarkkeyADT(design->markkey,design->nrules,design->events);
  printSignalADT(design->signals,design->nsignals);
  printIneqADT(design->inequalities,design->events,design->signals);
  for(int i=0;i<design->nevents+design->nplaces;i++) {
    for(int j=0;j<design->nevents+design->nplaces;j++) {
      if(design->rules[i][j]->level != NULL) {
        printf("%d:%d\n",i,j);
      }
    }
  }
#endif
  /* Determine how many new transitions are needed */
  int newT = 0;
  int newSig = 0;
  map<const string,int> nameCntM;
  map<const int,string> sigNameM;
  for(int i=1;i<design->nevents;i++) {
    bool validEvent = false;
    for(int j=0;j<design->nrules;j++) {
      if(design->markkey[j]->enabling == i) {
        validEvent = true;
        break;
      }
    }
    for(ineqADT j=design->events[i]->inequalities;j!=NULL&&validEvent;
        j=j->next) {
      if(j->type == 6) {
	if (j->tree && j->tree->isit != 'n') continue;
        if(j->constant != j->uconstant) {
          newT++;
          sigNameM[newSig] = design->events[j->place]->event;
          //nameCntM[design->events[j->place]->event] += 1;
          newSig++;
        }
        if(j->constant < 0 && j->uconstant > 0) {
          newT++;
        }
      }
    }
  }
#ifdef __EXPAND_DEBUG__
  printf("%d new transitions needed for rate expansion.\n",newT);
  printf("%d new signals needed for rate expansion.\n",newSig);
#endif
  if(newT > 0) {
    /* Create a resized markkeyADT and fill it with the correct
       information. */
    markkeyADT *newMarkkey = new_markkey(RESET,FALSE,0,(design->nevents+
                                                        design->nplaces+newT)*
                                         (design->nevents+design->nplaces+newT),
                                         NULL);
    for(int i=0;i<design->nrules;i++) {
      memcpy(newMarkkey[i],design->markkey[i],sizeof(*design->markkey[i]));
      /* Update the markkey for the places being shifted. */
      if(newMarkkey[i]->enabling >= design->nevents) {
        newMarkkey[i]->enabling += newT;
      }
      if(newMarkkey[i]->enabled >= design->nevents) {
        newMarkkey[i]->enabled += newT;
      }
    }
    /* Create a resized rulesADT and fill it with the correct
       information. */
    ruleADT **newRules = new_rules(RESET,FALSE,0,design->nevents+
                                   design->nplaces+newT,NULL);
    for(int i=0;i<design->nevents+newT+design->nplaces;i++) {
      for(int j=0;j<design->nevents+newT+design->nplaces;j++) {
        if((i >= design->nevents && i < design->nevents+newT) ||
           (j >= design->nevents && j < design->nevents+newT)) {
          newRules[i][j]->ruletype = NORULE;
          newRules[i][j]->level = NULL;
          newRules[i][j]->POS = NULL;
          newRules[i][j]->expr = NULL;
          newRules[i][j]->dist = NULL;
        }
        else if(i >= design->nevents+newT && j >= design->nevents+newT) {
          newRules[i][j] = copy_rule(design->rules[i-newT][j-newT]);
/*           memcpy(newRules[i][j],design->rules[i-newT][j-newT], */
/*                  sizeof(*(design->rules[i-newT][j-newT]))); */
        }
        else if(i >= design->nevents+newT) {
          newRules[i][j] = copy_rule(design->rules[i-newT][j]);
/*           memcpy(newRules[i][j],design->rules[i-newT][j], */
/*                  sizeof(*(design->rules[i-newT][j]))); */
        }
        else if(j >= design->nevents+newT) {
          newRules[i][j] = copy_rule(design->rules[i][j-newT]);
/*           memcpy(newRules[i][j],design->rules[i][j-newT], */
/*                  sizeof(*(design->rules[i][j-newT]))); */
        }
        else {
          newRules[i][j] = copy_rule(design->rules[i][j]);
/*           memcpy(newRules[i][j],design->rules[i][j], */
/*                  sizeof(*(design->rules[i][j]))); */
        }
      }
    }
    /* Create a resized eventADT and fill it with the correct
       information. */
    eventADT *newEvents = new_events(RESET,FALSE,0,design->nevents+
                                     design->nplaces+newT,NULL);
    for(int i=0;i<design->nevents;i++) {
      newEvents[i] = copy_event(design->events[i]);
/*       memcpy(newEvents[i],design->events[i],sizeof(*design->events[i])); */
      if(newEvents[i]->inequalities != NULL) {
        /* Update the place value in the inequalities to account for
           the shifting of places. */
        for(ineqADT j=newEvents[i]->inequalities;j!=NULL;j=j->next) {
          if(j->type != 7) {
            j->place += newT;
            if(j->signal >= design->nsignals-design->nineqs) {
              j->signal += newSig;
            }
          }
          else {
            if(j->place >= design->nsignals-design->nineqs) {
              j->place += newSig;
            }
          }
        }
      }
      /* Update the SOP for the addition of new signals */
      for(level_exp j=newEvents[i]->SOP;j!=NULL;j=j->next) {
        char* newProd =
          (char*) GetBlock(design->nsignals+newSig+1*sizeof(char));
        memset(newProd,'X',design->nsignals+newSig);
        newProd[design->nsignals+newSig] = '\0';
        for(int k=0;k<design->nsignals;k++) {
          if(k >= design->nsignals-design->nineqs) {
            newProd[k+newSig] = j->product[k];
          }
          else {
            newProd[k] = j->product[k];
          }
        }
        free(j->product);
        j->product = newProd;
      }
    }
    for(int i=design->nevents+newT;i<design->nevents+newT+design->nplaces;
        i++) {
      newEvents[i] = copy_event(design->events[i-newT]);
/*       memcpy(newEvents[i],design->events[i-newT], */
/*              sizeof(*design->events[i-newT])); */
      if(newEvents[i]->inequalities != NULL) {
        for(ineqADT j=newEvents[i]->inequalities;j!=NULL;j=j->next) {
          if(j->type != 7) {
            j->place += newT;
            if(j->signal >= design->nsignals-design->nineqs) {
              j->signal += newSig;
            }
          }
          else {
            if(j->place >= design->nsignals-design->nineqs) {
              j->place += newSig;
            }
          }
        }
      }
      /* Update the SOP for the addition of new signals */
      for(level_exp j=newEvents[i]->SOP;j!=NULL;j=j->next) {
        char* newProd =
          (char*) GetBlock(design->nsignals+newSig+1*sizeof(char));
        memset(newProd,'X',design->nsignals+newSig);
        newProd[design->nsignals+newSig] = '\0';
        for(int k=0;k<design->nsignals;k++) {
          if(k >= design->nsignals-design->nineqs) {
            newProd[k+newSig] = j->product[k];
          }
          else {
            newProd[k] = j->product[k];
          }
        }
        free(j->product);
        j->product = newProd;
      }
    }
    signalADT *newSignals = new_signals(RESET,FALSE,0,design->nsignals+
                                        newSig,NULL);
    for(int i=0;i<design->nsignals-design->nineqs;i++) {
      memcpy(newSignals[i],design->signals[i],sizeof(*design->signals[i]));
    }
    for(int i=design->nsignals-design->nineqs+newSig;
        i<design->nsignals+newSig;i++) {
      memcpy(newSignals[i],design->signals[i-newSig],
             sizeof(*design->signals[i-newSig]));
    }
    for(ineqADT i=design->inequalities;i!=NULL;i=i->next) {
      if(i->type != 7) {
        i->place += newT;
        if(i->signal >= design->nsignals-design->nineqs) {
          i->signal += newSig;
        }
      }
      else {
        if(i->place >= design->nsignals-design->nineqs) {
          i->place += newSig;
        }
      }
    }
    /* Do the rate expansion by modifying old places and adding
       information to the new places */
    int curNewT = 0;
    int curNewSig = 0;
    int prevEvent = -1;
    set<int> prevVars;
    bool secondPass = false;

    for(int i=1;i<design->nevents;i++) {
      bool validEvent = false;
      for(int j=0;j<design->nrules;j++) {
        if(design->markkey[j]->enabling == i) {
          validEvent = true;
          break;
        }
      }
      for(ineqADT j=newEvents[i]->inequalities;j!=NULL&&validEvent;j=j->next) {
        bool adjust = false;
        bool straddle = false;
        if(j->type == 6) {
	  if (j->tree && j->tree->isit != 'n') continue;
          if(j->constant != j->uconstant) {
            adjust = true;
          }
          if(j->constant < 0 && j->uconstant > 0) {
            straddle = true;
          }
          if(adjust && straddle) {
            if(prevEvent == i &&
               prevVars.count(j->place) != 0) {
              secondPass = true;
            }
#ifdef __EXPAND_DEBUG__
            printf("ADJUST && STRADDLE\n");
#endif
            createExpandEvent(newEvents,design->nevents,i,curNewT,j,newSignals,
                              design->nsignals,design->nineqs,true,curNewSig,
                              newSig,nameCntM,sigNameM);
            createExpandMarkkey(newMarkkey,newRules,design->nrules,
                                design->nevents,i,curNewT);
            adjustRate(newEvents,design->nevents,j,curNewT,j->constant,j->expr,
                       design,newSignals,curNewSig,newSig,secondPass,sigNameM);
            curNewT++;
            createExpandEvent(newEvents,design->nevents,i,curNewT,j,newSignals,
                              design->nsignals,design->nineqs,false,curNewSig,
                              newSig,nameCntM,sigNameM);
            createExpandMarkkey(newMarkkey,newRules,design->nrules,
                                design->nevents,i,curNewT);
	    /* CHECK THIS */
            adjustRate(newEvents,design->nevents,j,curNewT,j->uconstant,j->expr,
                       design,newSignals,curNewSig,newSig,secondPass,sigNameM);
            adjustRate(newEvents,i,j,0,0,NULL,design,newSignals,curNewSig,newSig,
                       secondPass,sigNameM);
            curNewT++;
            curNewSig++;
            prevEvent = i;
            secondPass = false;
          }
          else if(adjust) {
            if(prevEvent == i &&
               prevVars.find(j->place) != prevVars.end()) {
              secondPass = true;
            }
#ifdef __EXPAND_DEBUG__
            printf("ADJUST.\n");
#endif
            createExpandEvent(newEvents,design->nevents,i,curNewT,j,newSignals,
                              design->nsignals,design->nineqs,true,curNewSig,
                              newSig,nameCntM,sigNameM);
#ifdef __EXPAND_DEBUG__
            printVerboseEvent(newEvents,design->nevents+curNewT,
                              newSignals,design->nsignals+newSig);
            printf("Creating event for: %s\n",newEvents[i]->event);
#endif
            createExpandMarkkey(newMarkkey,newRules,design->nrules,
                                design->nevents,i,curNewT);
//Rates from [0,-#] should initially set the rate to 0 and then change to -#
	    if(j->constant < 0 && j->uconstant == 0) {
	      adjustRate(newEvents,design->nevents,j,curNewT,j->constant,j->expr,
			 design,newSignals,curNewSig,newSig,secondPass,sigNameM);
	      /* CHECK THIS */
	      adjustRate(newEvents,i,j,0,j->uconstant,j->expr,design,newSignals,curNewSig,
			 newSig,secondPass,sigNameM);
	    }
	    else {
	      /* CHECK THIS */
	      adjustRate(newEvents,design->nevents,j,curNewT,j->uconstant,j->expr,
			 design,newSignals,curNewSig,newSig,secondPass,sigNameM);
	      adjustRate(newEvents,i,j,0,j->constant,j->expr,design,newSignals,curNewSig,
			 newSig,secondPass,sigNameM);
	    }
            curNewT++;
            curNewSig++;
            prevEvent = i;
            prevVars.insert(j->place);
            secondPass = false;
          }
        }
      }
      prevVars.clear();
    }
#ifdef __EXPAND_DEBUG__
    printf("Checking for duplicate levels...\n");
    for(int i=0;i<design->nevents+newT+design->nplaces;i++) {
      for(int j=0;j<design->nevents+newT+design->nplaces;j++) {
        for(int k=0;k<design->nevents+newT+design->nplaces;k++) {
          for(int l=0;l<design->nevents+newT+design->nplaces;l++) {
            if(newRules[i][j]->level == newRules[k][l]->level &&
               newRules[i][j]->level != NULL &&
               (i != k && j != l)) {
              printf("Duplicate pointer:[%d][%d] and [%d][%d]\n",i,j,k,l);
            }
          }
        }
      }
    }
#endif 
    /* NULL out the pointers in the old rulesADT and eventsADT
       structures so that it doesn't get deallocated while still in
       use by the new structures. */
    for (int i=0;i<design->nevents+design->nplaces;i++) {
      for (int j=0;j<design->nevents+design->nplaces;j++) {
        design->rules[i][j]->level = NULL;
        design->rules[i][j]->POS = NULL;
        design->rules[i][j]->expr = NULL;
        design->rules[i][j]->dist = NULL;
      }
    }
    new_rules(LOADED,TRUE,design->nevents+design->nplaces,0,design->rules);
    for (int i=0;i<design->nevents+design->nplaces;i++) {
      design->events[i]->event = NULL;
      design->events[i]->hsl = NULL;
      design->events[i]->data = NULL;
      design->events[i]->SOP = NULL;
      design->events[i]->inequalities = NULL;
    }
    new_events(LOADED,TRUE,design->nevents+design->nplaces,0,design->events);
    new_markkey(LOADED,TRUE,(design->nevents+design->nplaces)*
                (design->nevents+design->nplaces),0,design->markkey);
    for(int i=0;i<design->nsignals;i++) {
      design->signals[i]->name = NULL;
    }
    new_signals(LOADED,TRUE,design->nsignals,0,design->signals);
    /* Make the newly created data structures take effect */
    design->rules = newRules;
    design->events = newEvents;
    design->markkey = newMarkkey;
    design->signals = newSignals;
    design->nevents += newT;
    design->nrules += (2*newT);
    design->nsignals += newSig;
    char* newStartstate = (char*) GetBlock((strlen(design->startstate)+1+newSig)
                                           * sizeof(char));
    newStartstate[strlen(design->startstate)+newSig] = '\0';
    for(int i=0;i<design->nsignals-newSig;i++) {
      newStartstate[i]=design->startstate[i];
    }
    for(int i=design->nsignals-newSig;i<design->nsignals;i++) {
      newStartstate[i]='0';
    }
    newStartstate[design->nsignals]='\0';
    free(design->startstate);
    design->startstate = newStartstate;

    /* Update initial rates */
    for (int i=design->nevents;i<design->nevents+design->nplaces;i++) {
      design->events[i]->uinitrate=design->events[i]->linitrate;
    }
    if (design->initrate) free(design->initrate);
    design->initrate=NULL;

    for (int i=0;i<design->nevents;i++) {
      if ((design->events[i]->hsl) && (!design->events[i]->EXP)) {
	position=0;
	token=intexpr_gettok(design->events[i]->hsl,tokvalue,MAXTOKEN,&position);
	intexpr_L(&token,design->events[i]->hsl,tokvalue,&position,
		  &design->events[i]->EXP,design->signals,design->nsignals,design->events,
		  design->nevents,design->nplaces);
      }
      for (ineqADT curineq=design->events[i]->inequalities;
	   curineq;curineq=curineq->next) {
	if ((curineq->expr) && (!curineq->tree)) {
	  position=0;
	  token=intexpr_gettok(curineq->expr,tokvalue,MAXTOKEN,&position);
	  intexpr_L(&token,curineq->expr,tokvalue,&position,
		    &curineq->tree,design->signals,design->nsignals,design->events,
		    design->nevents,design->nplaces);
	}
      }
    }
#ifdef __EXPAND_DEBUG__
    printVerboseEventADT(design->events,design->nevents,design->nplaces,
                         design->signals,design->nsignals);
    printMarkkeyADT(design->markkey,design->nrules,design->events);
    printRuleADT(design->rules,design->nevents,design->nplaces,design->events);
    printSignalADT(design->signals,design->nsignals);
    printIneqADT(design->inequalities,design->events,design->signals);
    printf("Checking for duplicate levels...\n");
    for(int i=0;i<design->nevents+design->nplaces;i++) {
      for(int j=0;j<design->nevents+design->nplaces;j++) {
        for(int k=0;k<design->nevents+design->nplaces;k++) {
          for(int l=0;l<design->nevents+design->nplaces;l++) {
            if(design->rules[i][j]->level == design->rules[k][l]->level &&
               design->rules[i][j]->level != NULL &&
               (i != k && j != l)) {
              printf("Duplicate pointer:[%d][%d] and [%d][%d]\n",i,j,k,l);
            }
          }
        }
      }
    }
#endif
    return true;
  }
  else {
    return true;
  }
}

/****************************************************************************
 * 
 ****************************************************************************/
bool doCopyIneq(ineqADT src,ineqADT tst) 
{
  if(src->type == 6 && src->place == tst->place) {
    return true;
  }
  else {
    return false;
  }
}

/****************************************************************************
 * 
 ****************************************************************************/
void createExpandEvent(eventADT *events,int nevents,int i,int curNewT,
                       ineqADT ineqEx,signalADT *signals,int nsignals,
                       int nineqs,bool newSig,int curNewSig,int numNewSig,
                       map<const string,int> &nameCntM,
                       map<const int,string> &sigNameM) 
{
  if(newSig) {
    int numVar = nameCntM[events[ineqEx->place]->event];
    nameCntM[events[ineqEx->place]->event] = numVar+1;
#ifdef __EXPAND_DEBUG__
    printf("Creating %sudot%d\n",events[ineqEx->place]->event,numVar);
#endif
    signals[nsignals+curNewSig-nineqs]->name =
      (char*) GetBlock((strlen(events[ineqEx->place]->event)+8)*sizeof(char));
    sprintf(signals[nsignals+curNewSig-nineqs]->name,"%sudot%d",
            events[ineqEx->place]->event,numVar);
    signals[nsignals+curNewSig-nineqs]->is_level = 0;
    signals[nsignals+curNewSig-nineqs]->event = 0;
    signals[nsignals+curNewSig-nineqs]->riselower = 0;
    signals[nsignals+curNewSig-nineqs]->riseupper = 0;
    signals[nsignals+curNewSig-nineqs]->falllower = 0;
    signals[nsignals+curNewSig-nineqs]->fallupper = 0;
    signals[nsignals+curNewSig-nineqs]->maxoccurrence = 0;
  }

#ifdef __EXPAND_DEBUG__
  printf("Creating ex%d\n",curNewT);
#endif
  events[nevents+curNewT]->color = events[i]->color;
  events[nevents+curNewT]->signal = events[i]->signal;
  events[nevents+curNewT]->lower = 0;
  events[nevents+curNewT]->upper = INFIN;
  events[nevents+curNewT]->process = events[i]->process;
  events[nevents+curNewT]->type = events[i]->type;
  events[nevents+curNewT]->rate = events[i]->rate;
  events[nevents+curNewT]->lrate = events[i]->lrate;
  events[nevents+curNewT]->urate = events[i]->urate;
  events[nevents+curNewT]->lrange = events[i]->lrange;
  events[nevents+curNewT]->urange = events[i]->urange;
  events[nevents+curNewT]->linitrate = events[i]->linitrate;
  events[nevents+curNewT]->uinitrate = events[i]->uinitrate;
  events[nevents+curNewT]->failtrans = events[i]->failtrans;
  /* Deal w/ the char *s */
  events[nevents+curNewT]->event = (char*) GetBlock(8 * sizeof(char));
  events[nevents+curNewT]->event[7] = '\0';
  sprintf(events[nevents+curNewT]->event,"ex%d",curNewT);

  if(events[i]->data != NULL) {  
    events[nevents+curNewT]->data =
      (char*) GetBlock((strlen(events[i]->data)+1)* sizeof(char));
    strncpy(events[nevents+curNewT]->data,events[i]->data,
            strlen(events[i]->data)+1);
  }
  else {
    events[nevents+curNewT]->data = NULL;
  }

  /* Add the proper enabling condition */
  events[nevents+curNewT]->hsl = (char*) GetBlock((strlen(signals[nsignals+curNewSig-nineqs]->name)+1)*sizeof(char));
  strncpy(events[nevents+curNewT]->hsl,signals[nsignals+curNewSig-nineqs]->name,
          strlen(signals[nsignals+curNewSig-nineqs]->name)+1);

  
  /* Deal w/ the ADTs */
  if(nsignals+newSig > 0) {
    events[nevents+curNewT]->SOP = (bool_exp*) GetBlock(sizeof(bool_exp));
    events[nevents+curNewT]->SOP->product =
      (char*) GetBlock(nsignals+numNewSig+1*sizeof(char));
    memset(events[nevents+curNewT]->SOP->product,'X',nsignals+numNewSig);
    events[nevents+curNewT]->SOP->product[nsignals+curNewSig-nineqs] = '1';
    events[nevents+curNewT]->SOP->product[nsignals+numNewSig] = '\0';
    events[nevents+curNewT]->SOP->next = NULL;
  }
  else {
    events[nevents+curNewT]->SOP = NULL;
  }

  if(events[i]->inequalities != NULL) {
    ineqADT ineq = events[i]->inequalities;
    ineqADT prevIneq;
    while(!doCopyIneq(ineq,ineqEx)) {
      ineq = ineq->next;
      if(ineq == NULL) {
        events[nevents+curNewT]->inequalities = NULL;
        return;
      }
    }
#ifdef __EXPAND_DEBUG__
    printf("Initial ineq type: %d\n",ineq->type);
#endif
    events[nevents+curNewT]->inequalities =
      (ineqADT) GetBlock(sizeof(ineq_tag));
    events[nevents+curNewT]->inequalities->place = ineq->place;
    events[nevents+curNewT]->inequalities->type = ineq->type;
    events[nevents+curNewT]->inequalities->uconstant = ineq->uconstant;
    events[nevents+curNewT]->inequalities->constant = ineq->constant;
    events[nevents+curNewT]->inequalities->expr = ineq->expr;
    events[nevents+curNewT]->inequalities->tree = ineq->tree;
    events[nevents+curNewT]->inequalities->signal = ineq->signal;
    events[nevents+curNewT]->inequalities->transition = nevents+curNewT;
    events[nevents+curNewT]->inequalities->next = NULL;
    prevIneq = events[nevents+curNewT]->inequalities;
    for(ineqADT j=ineq->next;j!=NULL;j=j->next) {
      if(doCopyIneq(j,ineqEx)) {
        ineqADT newIneq = (ineqADT) GetBlock(sizeof(ineq_tag));
        newIneq->place = j->place;
        newIneq->type = j->type;
        newIneq->uconstant = j->uconstant;
        newIneq->constant = j->constant;
        newIneq->expr = j->expr;
        newIneq->tree = j->tree;
        newIneq->signal = j->signal;
        newIneq->transition = nevents+curNewT;
        newIneq->next = NULL;
        prevIneq->next = newIneq;
        prevIneq = newIneq;
      }
    }
    /* Add the ineq for the new signal assignment */
    ineqADT sigIneq = (ineqADT) GetBlock(sizeof(ineq_tag));
    sigIneq->place = nsignals + curNewSig - nineqs;
    sigIneq->type = 7;
    sigIneq->uconstant = 1;
    sigIneq->constant = 0;
    sigIneq->expr = CopyString("false");
    sigIneq->tree = new exprsn('t',false,false,-1);
    sigIneq->signal = -1;
    sigIneq->transition = nevents+curNewT;
    sigIneq->next = NULL;
    prevIneq->next = sigIneq;
  }
  else {
    events[nevents+curNewT]->inequalities = NULL;
  }
}

/****************************************************************************
 * 
 ****************************************************************************/
void createExpandMarkkey(markkeyADT *markkey,ruleADT **rules,int nrules,
                         int nevents,int t,int curNewT)
{
  int p = -1;
  for(int i=0;i<nrules;i++) {
    if(markkey[i]->enabling == t) {
      p = markkey[i]->enabled;
      break;
    }
  }
  markkey[nrules+(curNewT*2)]->enabling = p;
  markkey[nrules+(curNewT*2)]->enabled = nevents+curNewT;
  markkey[nrules+(curNewT*2)]->epsilon = 0;
  rules[p][nevents+curNewT]->ruletype = TRIGGER;
  rules[p][nevents+curNewT]->marking = nrules+(curNewT*2);
  
  markkey[nrules+(curNewT*2)+1]->enabling = nevents+curNewT;
  markkey[nrules+(curNewT*2)+1]->enabled = p;
  markkey[nrules+(curNewT*2)+1]->epsilon = 0;
  rules[nevents+curNewT][p]->ruletype = TRIGGER;
  rules[nevents+curNewT][p]->marking = nrules+(curNewT*2)+1;
  
}

/****************************************************************************
 * 
 ****************************************************************************/
void adjustRate(eventADT *events,int nevents,ineqADT ineq,int curNewT,
                int rate,char *expr,designADT design,signalADT *signals,int curNewSig,
                int newSig,bool secondPass,map<const int, string> &sigNameM)
{
  char tmpStr[255];

  bool found = false;
#ifdef __EXPAND_DEBUG__
  printf("Ineq to adjust...");
  printIneq(ineq,events,design->signals);
#endif
  for(ineqADT j = events[nevents+curNewT]->inequalities;j!=NULL;j=j->next) {
#ifdef __EXPAND_DEBUG__
    printf("Comparing against...");
    printIneq(j,events,design->signals);
#endif
    if(eqIneq(ineq,j)) {
      j->constant = rate;
      j->uconstant = rate;
      sprintf(tmpStr,"%d",rate);
      j->expr = CopyString(tmpStr);
      found = true;
      break;
    }
  }
  if(!found) {
    printf("WARNING: Rate was not adjusted for: ");
    printIneq(ineq,events,design->signals);
  }

#ifdef __EXPAND_DEBUG__
  printf("Adjusting Boolean signal assignments for %s\n",
         events[nevents+curNewT]->event);
#endif
  /* Add inequalities to reset rate Booleans, but only to
     pre-existing transitions */
  if(nevents != design->nevents) {      
    if(secondPass){
      bool found = false;
      for(ineqADT i=events[nevents+curNewT]->inequalities;i!=NULL;i=i->next) {
        if(i->type == 7 &&
           i->place == design->nsignals+curNewSig-design->nineqs) {
          found = true;
          i->uconstant = 0;
          i->constant = 1;
	  i->expr = CopyString("true");
	  i->tree = new exprsn('t',true,true,-1);
          break;
        }
      }
      if(!found) {
        printf("ERROR: A second pass didn't find the Boolean signal present!\n");
      }
    }
    else {
      ineqADT last;
      ineqADT sigIneq;
      for(last=events[nevents+curNewT]->inequalities;last->next!=NULL;
          last=last->next) {
      }
      for(int i=0;i<newSig;i++) {
#ifdef __EXPAND_DEBUG__
        printf("Working on:%s -- %s\n",
               signals[curNewSig+design->nsignals-design->nineqs]->name,
               signals[i+design->nsignals-design->nineqs]->name);
#endif
        if(sigNameM[curNewSig] == sigNameM[i]) {
          sigIneq = (ineqADT) GetBlock(sizeof(ineq_tag));
          sigIneq->place = design->nsignals + i - design->nineqs;
          sigIneq->type = 7;
          if(i != curNewSig) {  
            sigIneq->uconstant = 1;
            sigIneq->constant = 0;
	    sigIneq->expr = CopyString("false");
	    sigIneq->tree = new exprsn('t',false,false,-1);
          }
          else {
            sigIneq->uconstant = 0;
            sigIneq->constant = 1;
	    sigIneq->expr = CopyString("true");
	    sigIneq->tree = new exprsn('t',true,true,-1);
          }
          sigIneq->signal = -1;
          sigIneq->transition = nevents+curNewT;
          sigIneq->next = NULL;
          last->next = sigIneq;
          last = sigIneq;
        }
      }
    }
  }
}

/****************************************************************************
 * 
 ****************************************************************************/
bool eqIneq(ineqADT i,ineqADT j)
{
  if(i->type == j->type &&
     i->uconstant == j->uconstant &&
     i->constant == j->constant &&
     i->signal == j->signal) {
    return true;
  }
  else {
    return false;
  }
}
