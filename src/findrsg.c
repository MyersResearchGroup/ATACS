/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1996 by, Chris J. Myers                                   */
/*   Electrical Engineering Department                                       */
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
/*   specific, prior written permission of Univeristy of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL U. OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE LIABLE  */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* findrsg.c                                                                 */
/*****************************************************************************/

#include "findrsg.h"
#include "def.h"
#include "findreg.h"
#include "connect.h"
#include "findwctd.h"
#include "findcl.h"
#include "orbitsrsg.h"
#include "merges.h"
#include "memaloc.h"
#include "geom.h"
#include "pot.h"
#include "approx.h"
#include "sets.h"
#include "project.h"
#include "printgrf.h"
#include "lpnrsg.h"
#include "srllpnrsg.h"
#include "hpnrsg.h"
#include "lhpnrsg.h"
#include "printlpn.h"
#include "dotrsg.h"
#include "verify.h"
#include "ctlchecker.h"
#include "lpntrsfm.h"
#include "displayADT.h"

#ifdef NEW_CSC
#include "statecode.h"
#else
#include "stateasgn.h"
#endif

// egm 02/15/00 -- New timing analysis stuff
#include "bag.h"
#include "bap.h"
#include "error.h"

#include <cmath>
#include <sys/time.h>
#include <unistd.h>

#ifdef INTEL
#include "solvecsc.h"
#include "slack.h"
#include "atacs.h"
#endif

#define EOS '\0'

//#define LIMIT 2000
#define HASH_STATE

//#define __TRACE_DEBUG__

/*****************************************************************************/
/* bdd junk                                                                  */
/*****************************************************************************/
#ifdef DLONG
#include <bdduser.h>
#include <Bdd.h>
#else
#include "cudd/include/cuddObj.hh"
#endif

bool fromG;

bool *disabled;

bool check_rule(ruleADT **rules,int i,int j,bool chkord,bool chkassump)
{
  return ((rules[i][j]->ruletype > NORULE) &&
	  ((rules[i][j]->ruletype < ORDERING) || 
	   (chkord && (rules[i][j]->ruletype & ORDERING)) || 
	   (chkassump && (rules[i][j]->ruletype & ASSUMPTION))));
}

/*****************************************************************************/
/* Check if a signal is enabled given a marking.                             */
/*****************************************************************************/

bool event_enabled(eventADT *events,ruleADT **rules,markkeyADT *markkey, 
		   char * marking,
		   int event,int nrules,bool verbose,markingADT marking_struct,
		   int nsigs, bool currently_enabled,bool chkord,
		   bool chkassump)
{
  int i,j;
  bool enabled;
  bool rule_marked;
  bool satisfied;
  bool is_rule;
  bool has_rules;

  has_rules=FALSE;

  if (events[event]->dropped) return FALSE;
  enabled=TRUE;

  for (i=0;i<nrules;i++){
    satisfied=TRUE;
    if((markkey[i]->enabled==event) && (markkey[i]->enabling!=0)) {
      is_rule = check_rule(rules,markkey[i]->enabling,markkey[i]->enabled,
			   chkord,chkassump);
      rule_marked = (!(is_rule) || (marking[i]!='F'));

      if (rule_marked) has_rules=TRUE;

      if ((is_rule) && (rule_marked)) {
	if ((!currently_enabled || 
	     rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype &
	     DISABLING))  {
	  satisfied = level_satisfied(rules, marking_struct, nsigs, 
				      markkey[i]->enabling, 
				      markkey[i]->enabled);
	}
      }
      if ((is_rule) && (!rule_marked || (rule_marked && !satisfied))) {
	enabled=FALSE;
	for (j=0;j<nrules && (!enabled);j++)
	  if ((marking[j]!='F')&&
	      (markkey[j]->enabling!=0) && (markkey[j]->enabled==event) &&
	      (rules[markkey[i]->enabling][markkey[j]->enabling]->conflict &
	       ICONFLICT)) {
	    is_rule = check_rule(rules,markkey[j]->enabling,
				 markkey[j]->enabled,chkord,chkassump);
	    if ((is_rule) && ((!currently_enabled || 
		 rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype &
		 DISABLING))) {
	      if(level_satisfied(rules, marking_struct, nsigs, 
				 markkey[j]->enabling,
				 markkey[j]->enabled)) 
		enabled=TRUE;
	    } else
	      enabled=TRUE;
	  }
      }
      if (!enabled) {
	if (chkord) {
	  if (!verbose) {
	    printf("ERROR!\n");
	    fprintf(lg,"ERROR!\n");
	  }
	  printf("ERROR: constraint rule (%s,%s) is not marked.\n",
		 events[markkey[i]->enabling]->event,
		 events[markkey[i]->enabled]->event);
	  fprintf(lg,"ERROR: constraint rule (%s,%s) is not marked.\n",
		  events[markkey[i]->enabling]->event,
		  events[markkey[i]->enabled]->event);
	}
	return(FALSE);
      }
    }
  }
  if (!has_rules) return FALSE;

  return(TRUE);
}

/*****************************************************************************/
/* Find all enabled signals.                                                 */
/*****************************************************************************/

bool *events_enabled(eventADT *events,ruleADT **rules,markkeyADT *markkey, 
		     char * marking,int nrules,markingADT marking_struct,
		     int nsigs, bool *currently_enabled, bool chkord,
		     bool chkassump,int nevents)
{
  int i,j,event;
  bool *enabled;
  bool rule_marked;
  bool satisfied;
  bool is_rule;
  bool immediate;

  enabled = (bool *)GetBlock(nevents * sizeof(bool));
  for (i=0;i<nevents;i++) 
    if ((events[i]->dropped)||(events[i]->signal < 0)) enabled[i]=FALSE;
    else enabled[i]=TRUE;
  for (i=0;i<nrules;i++) {
    event=markkey[i]->enabled;
    if (enabled[event]) {
      satisfied=TRUE;
      if (markkey[i]->enabling!=0) {
	is_rule = check_rule(rules,markkey[i]->enabling,event,chkord,
			     chkassump);
	rule_marked = (!(is_rule) || (marking[i]!='F'));
	if ((is_rule) && (rule_marked)) {
	  if ((!(currently_enabled[event]) || 
	       rules[markkey[i]->enabling][event]->ruletype & DISABLING))  {
	    satisfied = level_satisfied(rules, marking_struct, nsigs, 
					markkey[i]->enabling,event); 
	    if (!(rules[markkey[i]->enabling][event]->ruletype & DISABLING) &&
		(satisfied))
	      marking[i]='S';
	  }
	}

	if ((is_rule) && (!rule_marked || (rule_marked && !satisfied))) {
	  enabled[event]=FALSE;
	  for (j=0;j<nrules && (!enabled[event]);j++)
	    if ((marking[j]!='F')&&
		(markkey[j]->enabling!=0) && (markkey[j]->enabled==event) &&
		(rules[markkey[i]->enabling][markkey[j]->enabling]->conflict &
		 ICONFLICT)){
	      is_rule = check_rule(rules,markkey[j]->enabling,event,chkord,
				   chkassump);
	      if ((is_rule) && ((!currently_enabled[event] || 
				 rules[markkey[j]->enabling][event]->ruletype &
				 DISABLING))) {
		if (level_satisfied(rules, marking_struct, nsigs,
				    markkey[j]->enabling,event))
		  enabled[event]=TRUE;
	      } else
		enabled[event]=TRUE;
	    }
	}
	if (!enabled[event]) {
	  if (chkord)
	    printf("Ordering rule <%s,%s> not enabled.\n",
		   events[markkey[i]->enabling]->event,
		   events[markkey[i]->enabled]->event);
	}
      }
    } else {
      if (markkey[i]->enabling!=0) {
	is_rule = check_rule(rules,markkey[i]->enabling,event,chkord,
			     chkassump);
	rule_marked = (!(is_rule) || (marking[i]!='F'));
	if ((is_rule) && (rule_marked)) {
	  if (!(rules[markkey[i]->enabling][event]->ruletype & DISABLING) &&
	      (level_satisfied(rules, marking_struct, nsigs, 
			       markkey[i]->enabling,event))) 
	    marking[i]='S';
	}
      }
    }
  }
  immediate=FALSE;
  for (i=0;i<nevents;i++) 
    if ((enabled[i]) && (events[i]->immediate)) {
      immediate=TRUE;
      break;
    }
  if (immediate) 
    for (i=0;i<nevents;i++) 
      if ((enabled[i]) && (!(events[i]->immediate))) {
	enabled[i]=FALSE;
      }
  return enabled;
}

void delete_marking(markingADT marking)
{
    if (marking->marking) free(marking->marking);
    if (marking->enablings) free(marking->enablings);
    if (marking->state) free(marking->state);
    if (marking->up) free(marking->up);
    if (marking->down) free(marking->down);
}

/*****************************************************************************/
/* Find initial marking.                                                     */
/*****************************************************************************/

markingADT find_initial_marking(eventADT *events,ruleADT **rules,
				markkeyADT *markkey,int nevents,int nrules,
				int ninpsig,int nsignals,char * startstate,
				bool verbose)
{
  markingADT marking=NULL;
  int i,j,k;
  bool initial;

  marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=(char *)GetBlock((nrules+2)*sizeof(char));
  marking->enablings=(char *)GetBlock((nevents+1)*sizeof(char));
  marking->state=(char *)GetBlock((nsignals+2)*sizeof(char));
  marking->up=(char *)GetBlock((nsignals+2)*sizeof(char));
  marking->down=(char *)GetBlock((nsignals+2)*sizeof(char));

  // This is used for bap_findrsg to count references to this allocated
  // marking -- egm 12/06/00
  marking->ref_count = 0;

  for (j=0;j<nrules;j++) {
    if (markkey[j]->epsilon==1) {
      initial=TRUE;

      /* If a marked rule has an enabling event which conflicts with the
         enabling event of another rule with the same enabled event and it 
         is not marked, then strip the token. */
      if (!fromG) /* HACK - SHOULD PROPAGATE IN */
	for (k=1;k<nevents;k++)
	  if ((rules[k][markkey[j]->enabled]->ruletype) &&
	      (rules[k][markkey[j]->enabling]->conflict & ICONFLICT) &&
	      (rules[k][markkey[j]->enabled]->epsilon==0)) {
	    /*	    printf("%s -> %s and %s # %s\n",events[k]->event,
		   events[markkey[j]->enabled]->event,events[k]->event,
		   events[markkey[j]->enabling]->event);*/
	    initial=FALSE;
	    break;
	  }

      /* If there exists another later rule, r1, which conflicts with this 
         one, r2, and there does not exist another later rule that conflicts
         with r1, then strip the token.  This code cleans up the fact that
         a merge place is given a token on both rules by the compiler. */
      if (initial) {
	for (k=nevents-1;k>0;k--)
	  if (rules[k][markkey[j]->enabled]->ruletype) {
	    for (i=nevents-1;i>k;i--) 
	      if ((rules[i][markkey[j]->enabled]->ruletype) &&
		  (rules[i][k]->conflict & ICONFLICT))
		break;
	    if ((i==k) &&
		(rules[k][markkey[j]->enabling]->conflict & ICONFLICT)) {
	      initial=FALSE;
	      break;
	    }
	  }
      }
      
      /* This code does not seem necessary, but maybe it is??? 
      if (initial) 
	for (k=0;k<j;k++)
	  if ((marking->marking[k]!='F') && 
	      (markkey[j]->enabled==markkey[k]->enabled) &&
	      (rules[markkey[j]->enabling][markkey[k]->enabling]->conflict &
	      ICONFLICT)) {
	    initial=FALSE;
	    break;
	  }
      */

      if ((initial) &&
	  (rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype 
	   > NORULE) /*
	  && (!(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype & 
	  REDUNDANT))*/) 
	marking->marking[j]='T';
      else marking->marking[j]='F';
    } else
      marking->marking[j]='F';
  }
  marking->marking[nrules]='\0';
  //printf("%s\n",marking->marking);
  for (j=0;j<nevents;j++)
    marking->enablings[j]='F';
  marking->enablings[j]='\0';
  for (j=0;j<nsignals;j++) {
    marking->up[j]=1;
    marking->down[j]=1;
  }
  marking->state[nsignals]='\0';
  marking->up[nsignals]='\0';
  marking->down[nsignals]='\0';
  if (startstate==NULL) { 
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR:  Need initial state information.\n");
    fprintf(lg,"ERROR:  Need initial state information.\n");
    delete_marking(marking);
    free(marking);
    return(NULL);
  } else strcpy(marking->state,startstate);
  for (j=1;j<nevents;j++)
    if ((events[j]->signal!=(-1)) && 
	event_enabled(events,rules,markkey,marking->marking,j,nrules,FALSE, 
		      marking,nsignals,FALSE,FALSE,FALSE)) {
      marking->enablings[j]='T';
      if (j % 2==1) {
	if ((marking->state[events[j]->signal]!='0')&&
	    (marking->state[events[j]->signal]!='R')) {
	  if (events[j]->signal >= ninpsig) {
	    if (!verbose) {
	      printf("ERROR!\n");
	      fprintf(lg,"ERROR!\n");
	    }
	    printf("ERROR: %s enabled but signal is not 0 or R in state %s\n",
		   events[j]->event,marking->state);
	    fprintf(lg,"ERROR: %s enabled but signal is not 0 or R in state %s\n",
		    events[j]->event,marking->state);
	    return NULL;
	  }
	} else
	  marking->state[events[j]->signal]='R';
      } else {
	if ((marking->state[events[j]->signal]!='1')&&
	    (marking->state[events[j]->signal]!='F')) {
	  if (events[j]->signal >= ninpsig) {
	    if (!verbose) {
	      printf("ERROR!\n");
	      fprintf(lg,"ERROR!\n");
	    }
	    printf("ERROR: %s enabled but signal is not 1 or F in state %s\n",
		   events[j]->event,marking->state);
	    fprintf(lg,"ERROR: %s enabled but signal is not 1 or F in state %s\n",
		    events[j]->event,marking->state);
	    return NULL;
	  }
	} else
	  marking->state[events[j]->signal]='F';
      }
    }
  for (j=0;j<nrules;j++) {
    if ((marking->marking[j]=='T') &&
	(level_satisfied(rules, marking, nsignals, markkey[j]->enabling, 
			 markkey[j]->enabled)) && 
	(!(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype & 
	   DISABLING)))
      marking->marking[j]='S';
  }
  return(marking);
}

/*****************************************************************************/
/* Push marking onto stack of markings.                                      */
/*****************************************************************************/

void push_marking(markingsADT *marking_stack,markingADT marking,
		  char * firelist)
{
  markingsADT new_stack=NULL;

  new_stack=(markingsADT)GetBlock(sizeof *new_stack);
  new_stack->marking=marking;
  new_stack->firelist=firelist;
  new_stack->link=(*marking_stack);
  (*marking_stack)=new_stack;
}

/*****************************************************************************/
/* Implementation of Hash Function from Aho, Sethi, and Ullman, p.436        */
/*   which has been modified to perform well for bitvectors.                 */
/*****************************************************************************/
int hashpjw(char * s)
{
  char * p;
  unsigned h=0,g;

  for (p=s; *p!=EOS; p++) {
    h=(h << 1) + ((*p)-'0');
    if ((g = h&0xf0000000)) {
      h=h^(g >> 24);
      h=h^g;
    }
  }
  return h % PRIME;
}

/*
void print_table_distribution(stateADT *state_table)
{
  int i, max;
  int *distribution;
  int buckets[PRIME];
  stateADT e;
  
  max = 0;
  for (i = 0; i < PRIME; i++) {
    buckets[i]=0;
    for (e = state_table[i]; 
	 e->link != NULL;
	 e = e->link)
      buckets[i]++;
    if (e->state!=NULL)
      buckets[i]++;
    if (buckets[i] > max)
      max = buckets[i];
  }
  printf("max = %d\n",max);
  distribution = (int *) calloc(max+1,sizeof(int));
  for (i = 0; i < PRIME; i++) {
    distribution[buckets[i]]++;
  }
  for (i = 0; i <= max; i++) {
    printf("# buckets with %2d entries is %4d\n",i,distribution[i]);
  }
}
*/

/*****************************************************************************/
/* Convert R to 0 and F to 1 for all bits in a state.                        */
/*****************************************************************************/

char * SVAL(char * state,int nsignals)
{
  int k;
  char * vstate;

  vstate=(char *)GetBlock(nsignals+2);
  for (k=0;k<nsignals;k++) {
    switch(state[k]) {
    case '0':
    case 'R':
    case 'D':
    case 'X':
      vstate[k]='0';
      break;
    case '1':
    case 'F':
    case 'U':
      vstate[k]='1';
      break;
    default:
      vstate[k]=state[k];
      break;
    }
  }
  vstate[k]='\0';
  return(vstate);
}

void my_print_graph(stateADT *state_table, eventADT *events,
                    markkeyADT *markkey, int nrules){

  int i;
  stateADT e;
  clocklistADT region;
  
  for(i=0;i<PRIME;i++){
    if(state_table[i]->state!=NULL){
      printf("In position %d\n", i);
      e=state_table[i];
      while(e->state!=NULL){
        printf("%d:%s\n", e->number, e->state);
        printf("Marking: ");
        dump( cout, nrules, e->marking, markkey, events ) << endl;
        printf("This state has regions:\n");
        region=e->clocklist;
        while(region!=NULL){
          printf("Clockkey version1:\n");
          dump( cout, region->clocknum-1, region->clockkey, events ) << endl;
	  // dump( cout, region->clocknum, region->clockkey, events ) << endl;
          printf("Clocks:\n");
          dump( cout, region->clocks, region->clocknum-1) << endl;
          //dump( cout, region->clocks, region->clocknum) << endl;
	  if (region->exp_enabled) {
	    printf("Fired Clocks: ");
	    dump( cout, nrules, region->exp_enabled, markkey, events ) << endl;
	  }
          //print_clocks(region->clocks, region->clocknum-2);
          //print_clockkey(region->clocknum-1, region->clockkey, events);
          //printf("clocksize=%d\n", region->clocksize);
          region=region->next;
        }	
        e=e->link;
      }
    }
  }
}

void my_print_graph2(stateADT *state_table, eventADT *events,
		     markkeyADT *markkey, int nrules){

  int i;
  stateADT e;
  clocklistADT region;
  
  for(i=0;i<PRIME;i++){
    if(state_table[i]->state!=NULL){
      printf("In position %d\n", i);
      e=state_table[i];
      while(e->state!=NULL){
        printf("%d:%s\n", e->number, e->state);
        printf("Marking: ");
        dump( cout, nrules, e->marking, markkey, events ) << endl;
        printf("This state has regions:\n");
        region=e->clocklist;
        while(region!=NULL){
          printf("Clockkey version1:\n");
          dump( cout, region->clocknum, region->clockkey, events ) << endl;
          printf("Clocks:\n");
          dump( cout, region->clocks, region->clocknum) << endl;
	  if (region->exp_enabled) {
	    printf("Fired Clocks: ");
	    dump( cout, nrules, region->exp_enabled, markkey, events ) << endl;
	  }
          //printf("clocksize=%d\n", region->clocksize);
          region=region->next;
        }	
        e=e->link;
      }
    }
  }
}
    

bool simple_join(int event, ruleADT **rules, markkeyADT *markkey,
		 int nevents){
  int i,j;
  
  for(i=1;i<nevents;i++){
    if(rules[i][event]->ruletype){
      for(j=1;j<nevents;j++){
	if(rules[j][event]->ruletype && j!=i && 
	   !(rules[i][j]->conflict & ICONFLICT)){
	  return false;
	}
      }
    }
  }
  return true;
}

bool Petri_net_match(char * marking1, char * marking2, markkeyADT *markkey,
		     ruleADT **rules, int *mapping, int nevents){
  int i=0;
  int j=0;
  int nrules;
  nrules = strlen(marking1);
  //  bool impure=false;
  mapping=(int *)GetBlock((nrules+1)*sizeof(int));
  for(i=0;i<nrules;i++){
    mapping[i]=(-1);
  }
  for(i=0;i<nrules;i++){
    if(marking1[i]==marking2[i]){
      mapping[i]=i;
    }
    if(marking1[i]!=marking2[i]){
      for(j=0;j<nrules;j++){
	if((markkey[i]->enabled == markkey[j]->enabled) &&
	   (rules[markkey[i]->enabling][markkey[j]->enabling]->conflict 
	    & ICONFLICT) &&
	   (marking1[j]!=marking2[j]) &&
	   (marking1[i]==marking2[j])){
	  if(mapping[i]==(-1)){
	    mapping[i]=j;
	  }
	  else{
	    free(mapping);
	    return false;
	  }
	}
      }
      if(mapping[i]==(-1)){
	free(mapping);
	return false;
      }
    }
  }
  /* SRL: fixing weird problem */
  /* free(mapping); */
  return true;
}

/*****************************************************************************/
/* Check if state a is contained in state b.                                 */
/*****************************************************************************/

bool state_subset(char * a,char * b,int nsignals) 
{
  int i;
  bool compress;

  compress=FALSE;
  for (i=0;i<nsignals;i++) {
    if (((a[i]=='1') && (b[i]=='U')) || ((a[i]=='0') && (b[i]=='D')))
      compress=TRUE;
    if (((a[i]=='1') && ((b[i]=='0') || (b[i]=='D'))) ||
	((a[i]=='0') && ((b[i]=='1') || (b[i]=='U'))) ||  
	((a[i]=='U') && (b[i]!='U')) ||  
	((a[i]=='D') && (b[i]!='D')))
      return FALSE;
  } 
  return compress;
}

// Returns true IFF <i,j> is a behavioral rule
bool is_behavior_rule( int i, int j, ruleADT** rules ) {
  return( rules[i][j]->ruletype && !(rules[i][j]->ruletype & ORDERING) );
}

// Returns true IFF event_x and event_y both have behavioral rules 
// that enable a common event.
bool have_common_enabled_event( int event_x, int event_y, 
				ruleADT** rules, int num_events ) {
  for ( int i = 1 ; i < num_events ; ++i ) {
    if ( is_behavior_rule( event_x, i, rules ) && 
	 is_behavior_rule( event_y, i, rules ) ) {
      return( true );
    }
  }
  return( false );
}

int find_event_index( ruleADT** rules       ,
		      int event_i           ,
		      int event_j           ,
		      clocklistADT clockptr ,
		      int num_events        ,
		      bool Petri_match        ) {
  // 1) Search through the saved zone in the state space for event_i
  for( int j= 0 ; j < clockptr->clocknum ; ++j ){
    // a) Get the event from the stored zone (only interested in enabled
    //    in enabled field)
    // b) If event_i matches event_j in the 2 POSETs, then return
    //    the matching index.
    if ( (event_i == clockptr->clockkey[j].enabling) &&
	 (event_j == clockptr->clockkey[j].enabled) )
      return( j );
    // c) If event_i does not match event_j, but the markings represent a
    //    merge on 2 different branches ( i.e., the Petri_match flag is 
    //    true ); event_i and event_j enable a common event; and
    //    event_i conflicts with event_j, then set the match as an orbits 
    //    match.
  }
  if(Petri_match) {
    
  for( int j= 0 ; j < clockptr->clocknum ; ++j ){

    int event_j = clockptr->clockkey[j].enabled;

    if ( Petri_match && (event_i >= 0) && (event_j >= 0) &&
	 (rules[event_i][event_j]->conflict & ICONFLICT) &&
	 have_common_enabled_event( event_i, event_j, rules, num_events ) ) {
      return( j );
    }
  }
  }
  
  // 2) A matching event was _not_ found in clockptr
  return( _NOT_FOUND_ );
}

void bap_build_match_map( ruleADT** rules       ,
			  clockkeyADT clockkey  , 
			  clocklistADT clockptr ,
			  int* match            ,
			  int& clock_count      ,
			  int& timing_match     ,
			  int num_clocks        ,
			  int num_events        ,
			  bool Petri_match        ) {
  // NOTE: bag.cc -> POSET::make_clockkey only sets the enabled field,
  // so this code only needs to check the enabled field for a match!
  bool is_matched = true;
  for( int i = 0 ; i < num_clocks ; ++i ) {
    int j = find_event_index( rules, clockkey[i].enabling,
			      clockkey[i].enabled, clockptr , 
			      num_events, Petri_match    );
    // A matching event in clockptr could not be found
    if ( j == _NOT_FOUND_ ) {
      is_matched = false;
      match[i] = _NOT_FOUND_;
    }
    // Update the match index and increment the clock_count.
    else {
      match[i] = j;
      ++clock_count;
    }
  }
  // Why is this not a timing mismatch?  
  //
  // The candidate zone to be added may be a subset of a stored
  // zone since it matched events in the stored zone and has
  // more dimensions (i.e., the stored zone is unconstrained 
  // in the missing dimensions)
  if ( clock_count != clockptr->clocknum && !is_matched )
    timing_match = 0;
}

void bap_set_timing_match( clocksADT clocks              ,
			   clocklistADT clockptr         ,
			   int* match                    ,
			   int& timing_match             ,
			   int num_clocks                ,
			   timeoptsADT timeopts            ){
  // The stored zone in clockptr has more dimensions than the one
  // we are trying to add, thus, the clock are are adding could never
  // be a match or a subset---missing dimensions are unconstrained
  // separations
  if ( clockptr->clocknum > num_clocks ) {
    timing_match = 0;
    //cout << "wrong size" << endl;
    return;
  }
  // NOTE: Since we may be trying to add a clock of greater dimension
  // than the currently saved clock, it is necessary to only check
  // values that have been matched!  The check for _NOT_FOUND_ handles
  // this case.
  for( int i = 0 ; i < num_clocks ; ++i ) {
    if ( match[i] == _NOT_FOUND_ ) continue;
    for( int j = 0; j < num_clocks ; j++ ) {
      if ( match[j] == _NOT_FOUND_ ) continue;
      bool not_same_value = ( clocks[i][j] != 
			      clockptr->clocks[match[i]][match[j]] );
      bool this_is_greater = ( clocks[i][j] > 
			       clockptr->clocks[match[i]][match[j]] );
      // 2 cases will break the timing match at any given point:
      //
      //     1) The clocks have different values at (i,j) and subset is
      //        not turned on.
      //     2) The clocks have different values and subsets is turned
      //        on, but the value at clocks[i][j] is greater than 
      //        the value stored in the state space (i.e., this clock
      //        matrix is not a subset of the one in the state space.
      // 
      if ( ( !(timeopts.subsets) && not_same_value ) ||
	   ( timeopts.subsets && this_is_greater ) ) {
	//cout << "i,j   = " << clocks[i][j] << endl;
	//cout << "mi,mj = " << clockptr->clocks[match[i]][match[j]] << endl;
	timing_match = 0;
	return;
      }
    }
  }
}

int bap_get_superset( int num_clocks         ,
		      clocklistADT clockptr  ,
		      int num_matched_clocks ,
		      clocksADT clocks       ,
		      int* match               ) {
  // num_matched_clocks denotes the number of events in the zone in clocks
  // that were mapped to events in the zone in clockptr.  If all events
  // in the zone in clock were not matched, then the zone in clock could not
  // be a superset of the zone in clockptr.
  if( num_matched_clocks != num_clocks ) return( false );
  // A superset relation indicates that value at every clocks[i][j] location
  // is greater than the corresponding value in the zone stored in the 
  // state space.  If this relation is ever violated, then a superset
  // relation does not exist between clocks and the zone in the state space.
  // NOTE: For events not in the zone in clock but in the zone in clockptr,
  // they are unconstrained in the zone in clock, so they are automatically
  // equal to or larger than their counterparts in the zone in clockptr.
  for( int i = 0 ; i < num_clocks ; ++i ) {
    for( int j = 0 ; j < num_clocks ; ++j ) {
      if ( clocks[i][j] < clockptr->clocks[match[i]][match[j]] ) {
	return( false );
      }
    }
  }
  return( true );
}

void build_match_map( ruleADT** rules,
                      clockkeyADT clockkey, 
                      clocklistADT clockptr,
                      int* match,
                      int& matches,
                      int& timing_match,
                      int num_clocks,
                      bool Petri_match,
                      timeoptsADT timeopts ) {
  int i = 0, j = 0;
  for(i=1;i<=num_clocks;i++){
    for(j=1;j<=num_clocks;j++){
      if((clockkey[i].enabling == clockptr->clockkey[j].enabling) &&
         (clockkey[i].enabled == clockptr->clockkey[j].enabled)){
        if(timeopts.lhpn) {
          if((clockkey[i].lrate == clockptr->clockkey[j].lrate) &&
             (clockkey[i].urate == clockptr->clockkey[j].urate)) {
            match[i]=j;
            matches++;
            break;
          }
        }
        else {
          match[i]=j;
          matches++;
          break;
        }
      }
      if(Petri_match && 
         (clockkey[i].enabled == clockptr->clockkey[j].enabled) &&
         (rules[clockkey[i].enabling]
          [clockptr->clockkey[j].enabling]->conflict & ICONFLICT) ){
        if(timeopts.lhpn) {
          if((clockkey[i].lrate == clockptr->clockkey[j].lrate) &&
             (clockkey[i].urate == clockptr->clockkey[j].urate)) {
            match[i]=j;
            matches++;
            break;
          }
        }
        else {
          match[i]=j;
          matches++;
          break;
        }
      }
    }
  }
  if(matches!=num_clocks) (timing_match=0);
}

void set_timing_match( clocksADT clocks,
                       clocklistADT clockptr,
                       int* match,
                       int& timing_match,
                       int num_clocks,
                       timeoptsADT timeopts ){
  int i = 0, j = 0;
  for(i=0;i<=num_clocks;i++){
    for(j=0;j<=num_clocks;j++){
      if (((!timeopts.subsets) && 
           (clocks[i][j] != clockptr->clocks[match[i]][match[j]])) ||
          ((timeopts.subsets) && 
           (clocks[i][j] > clockptr->clocks[match[i]][match[j]]))) {
        timing_match=0;
        break;
      }
    }
    if(!(timing_match)){
      break;
    }
  }
}

int get_superset( int num_clocks,
		  clocklistADT clockptr,
		  int matches,
		  clocksADT clocks,
		  int* match ) {
  int i = 0, j = 0;
  if((num_clocks!=clockptr->clocknum) || (matches!=num_clocks))
    return( 0 );
  for(i=0;i<=num_clocks;i++){
    for(j=0;j<=num_clocks;j++){
      if (clocks[i][j] < clockptr->clocks[match[i]][match[j]]){
	return( 0 );
      }
    }
  }
  return( true );
}

/*****************************************************************************/
/* Find state in state table, return where it goes if it's not there.        */
/*****************************************************************************/

stateADT find_state(stateADT *state_table,char * state,char * marking,
		    char * enablings, clocksADT clocks, clockkeyADT clockkey,
		    int nsignals, int num_clocks, bool timed,int *timing_match,
		    int *is_new_state, int clock_size,timeoptsADT timeopts,
		    exp_enabledADT exp_enabled,
		    int *removed_regions, ruleADT **rules, 
		    markkeyADT *markkey, int nevents, bool bap )
{
  int position = 0;
  stateADT e;
  char * vstate;
  clocklistADT clockptr;
  clocklistADT prev;
  clocklistADT free_clockptr;
  int *match;
  int *Petri_mapping=NULL;
  bool Petri_match=0;
  int matches=0;
  bool superset=0;

  vstate=SVAL(state,nsignals);
  if ((strlen(vstate) < 2) && (marking))
    position=hashpjw(marking);
  else
    position=hashpjw(vstate);
  free(vstate);
  *timing_match=0;
  *is_new_state=1;
  *removed_regions=0;
  match=(int *)GetBlock((num_clocks+1)*sizeof(int));
  for (e = state_table[position];e->link != NULL;e = e->link){
    Petri_match=0;
    /*
      if (rules) {
      if (state_subset(state,e->state,nsignals)) {
      *is_new_state=0;
      free(match);
      return e;
      } 
      if (state_subset(e->state,state,nsignals)) {
      strcpy(e->state,state);
      } 
      }
    */
    if(strcmp(e->state, state)==0){
      if((timeopts.orbmatch)&&(e->marking)&&(marking)){
        Petri_match= Petri_net_match(e->marking, marking, 
                                     markkey, rules, Petri_mapping,
                                     nevents);
      }
    }
    if ((strcmp(e->state,state)==0) && (rules) &&
        ((!marking) || Petri_match || 
         (strcmp(e->marking,marking)==0))) {
      *is_new_state=0;
      if(timed || timeopts.genrg){
        /*printf("initial list, state %s\n",e->state);
          clockptr=e->clocklist;
          int list_size = 0;
          while(clockptr!=NULL){
          list_size++;
          print_clocks(clockptr->clocks,clockptr->clocknum);  
          clockptr = clockptr->next;
          }
          printf("%s:list size = %d\n",e->state,list_size);*/
        prev=e->clocklist;
        clockptr=e->clocklist;
        match[0]=0;
        while(clockptr!=NULL){
          if (timeopts.supersets){
            superset=1;
          }
          *timing_match=1;
          if((clockptr->exp_enabled) && (exp_enabled) &&
             (strcmp(clockptr->exp_enabled, exp_enabled)!=0) && 
             (timeopts.infopt)){
            *timing_match=0;
          }
          matches=0;
          /* ERIC: This causes problems with total order method */
          if ( (!bap || timeopts.hpn || timeopts.lhpn ||
                (!timeopts.subsets && !timeopts.supersets)) && 
               clockptr->clocknum != num_clocks ){
            *timing_match=0;
          }
          if (*timing_match) {
            if ( bap ) {
              // NOTE: This will match anytime the enabled rules match.
              // It will also create a match if Petri_match is true
              // and the enabled events conflict with each other.
              // The meaning of enabled in this case is not related
              // to rules, rather, it refers to the event in the POSET.
              bap_build_match_map( rules, clockkey, clockptr, match, 
                                   matches, *timing_match, num_clocks,
                                   nevents, Petri_match );
            }
            else {
              build_match_map( rules, clockkey, clockptr, match,
                               matches, *timing_match, num_clocks,
                               Petri_match, timeopts );
            }
          }	  

          if(*timing_match){
            if ( bap ) {
              //cout << "GOT HERE " << num_clocks << " " << clockptr->clocknum
              //   << endl;
              //dump( cout, clocks, num_clocks-1) << endl;
              //dump( cout, clockptr->clocks, clockptr->clocknum-1) << endl;
              bap_set_timing_match( clocks, clockptr, match, *timing_match, 
                                    num_clocks, timeopts );
              //cout << "match = " << *timing_match << endl;
            }
            else {
              set_timing_match( clocks, clockptr, match, *timing_match, 
                                num_clocks, timeopts );
            }
          }
          /* Code for supersets added here */
          if (timeopts.supersets && !(*timing_match)){
            if ( bap ) {
              superset = bap_get_superset( num_clocks, clockptr, matches,
                                           clocks, match );
            }
            else {
              superset = get_superset( num_clocks, clockptr, matches,
                                       clocks, match );

            }
          }
          // End block
          if((*timing_match)){
            free(match);
            return(e);
          }
          if(!(timeopts.genrg)){
            if(superset){
              if(clockptr==e->clocklist){
                if(clockptr->next!=NULL){
                  e->clocklist=clockptr->next;
                  prev=clockptr->next;
                }
              }
              else{
                prev->next=clockptr->next;
                superset=0; /* I ADDED THIS - CHRIS */
              }
              (*removed_regions)++;
              free_clockptr=clockptr;
              clockptr=clockptr->next;
              free_region(free_clockptr->clockkey, free_clockptr->clocks,
                          free_clockptr->clocksize);
              // Only free here if there is a valid pointer
              if ( exp_enabled != NULL ) {
                free(free_clockptr->exp_enabled);
              }
              free(free_clockptr);
            } else if(!superset){
              prev=clockptr;
              clockptr=clockptr->next;
            }	    
          }
          else{
            clockptr=NULL;
          }
        }
        //cout << "NEW ZONE Superset = " << superset << endl;
        if(!timeopts.genrg){
          if((prev!=e->clocklist) || (!superset)){
            prev->next=(clocklistADT)GetBlock(sizeof(struct clocklist));
            prev->next->clockkey=
              (clockkeyADT)copy_clockkey(clock_size,clockkey);
            prev->next->clocks=(clocksADT)copy_clocks((clock_size), clocks);
            prev->next->clocknum=num_clocks;
            prev->next->clocksize=clock_size;
            // Only copy exp_enabled if it is set to a non-NULL ponter!
            if ( exp_enabled != NULL ) {
              prev->next->exp_enabled=
                (exp_enabledADT)GetBlock((strlen(exp_enabled)+1)*(sizeof(char)));
              strcpy(prev->next->exp_enabled, exp_enabled);
            }
            else {
              prev->next->exp_enabled = NULL;
            }
            prev->next->next=NULL;
          }
          else{
            prev=(clocklistADT)GetBlock(sizeof(struct clocklist));
            prev->clockkey=(clockkeyADT)copy_clockkey(clock_size,clockkey);
            prev->clocks=(clocksADT)copy_clocks(clock_size, clocks);
            prev->clocknum=num_clocks;
            prev->clocksize=clock_size;
            // Only copy exp_enabled if it is set to a non-NULL ponter!
            if ( exp_enabled != NULL ) {
              prev->exp_enabled=
                (exp_enabledADT)GetBlock((strlen(exp_enabled)+1)*(sizeof(char)));
              strcpy(prev->exp_enabled, exp_enabled);
            }
            else {
              prev->exp_enabled = NULL;
            }
            prev->next=NULL;	    
            e->clocklist=prev;
          }
        }
      }
      /*clockptr=e->clocklist;
        list_size = 0;
        while(clockptr!=NULL){
        list_size++;
        print_clocks(clockptr->clocks,clockptr->clocknum);  
        clockptr = clockptr->next;
        }
        printf("list size = %d\n",list_size);*/
      if(!timeopts.genrg){
        free(match);
        return(e);
      }
    }
  }
  free(match);
  return(e);
}

/*****************************************************************************/
/* Check if state is in hashtable, if so, return true, else add to table and */
/*   return false.                                                           */
/*****************************************************************************/
#ifdef DLONG
int bdd_add_state(FILE* fp,stateADT *state_table,char * old_state,
		  char * old_marking,char * old_enablings,
		  clocksADT old_clocks, clockkeyADT old_clockkey,
		  int old_num_clocks, exp_enabledADT old_exp_enabled,
		  char * new_state,char * new_marking,
		  char * new_enablings,int nsignals,
		  clocksADT new_clocks, clockkeyADT new_clockkey, 
		  int new_num_clocks, int sn,bool verbose, bool timed, 
		  int nrules,int old_clock_size, int new_clock_size,
		  bddADT bdd_state,bool use_bdd,markkeyADT *marker,
		  timeoptsADT timeopts,int fire_enabling, int fire_enabled, 
		  exp_enabledADT new_exp_enabled,ruleADT **rules)
{
  int i,j;
  int nsigs;
  bdd s,t,u,v,w,m,M,U;
  int check;
  int *matrixkey;
  int bddtimer;
  int flag;

  int I;

  /* initialize */
  nsigs = strlen(new_state);

  if (timed) {
    matrixkey = (int*)GetBlock((nrules+1)*sizeof(int));

  /* Building BDD for the marking set to M */
    m = bdd_one(bdd_state->bddm);
    for (i = 0; i< nrules;i++){
      flag = 0;
      if (new_marking[i] == 'F'){
	flag = 1;
	t = bdd_not(bdd_state->bddm,bdd_state->bdd_mark[i]);
      }
      else
	t = bdd_state->bdd_mark[i];
      s = bdd_and(bdd_state->bddm,m,t);
      if (flag)
	bdd_free(bdd_state->bddm,t);
      bdd_free(bdd_state->bddm,m);
      m = s;
    }
    M = m;

    /* Building a mapping between sparse clock matrix and canonical 
       expanded matrix. */
    matrixkey[0] = -1;
    for (i=1;i<=new_num_clocks;i++){
      for (j=0;j<nrules;j++){
	if ((marker[j]->enabling == new_clockkey[i].enabling) &&
	    (marker[j]->enabled == new_clockkey[i].enabled))
	  matrixkey[i] = j;
      }
    }
    for (i=0;i<=new_num_clocks;i++){
      matrixkey[i]++;
    }

    /* Building MTBDD representation of clock matrix storing in U. */
    u = bdd_zero(bdd_state->bddm);
    for (i=0;i<=new_num_clocks;i++)
      for (j=0;j<=new_num_clocks;j++){
	s = bdd_and(bdd_state->bddm,bdd_state->R[matrixkey[i]],
		    bdd_state->C[matrixkey[j]]);
	t = mtbdd_ite(bdd_state->bddm,s, 
		      mtbdd_get_terminal(bdd_state->bddm,new_clocks[i][j],0),
		      u);
	bdd_free(bdd_state->bddm,s);
	bdd_free(bdd_state->bddm,u);
	u = t;
      }
    U = u;

    /* Find set of links associated with this marking. */
    flag = 0;bddtimer = 0;
    t = bdd_state->Reg;
    if (t == bdd_zero(bdd_state->bddm))
      flag = 1;
    if(0){
      for (i = 0; ((i< nrules) && !flag);i++){
	u = bdd_if(bdd_state->bddm,t);
	if (u == bdd_state->bdd_mark[i]){
	  if (new_marking[i] == 'F'){
	    v = bdd_else(bdd_state->bddm,t);
	  }
	  else{
	    v = bdd_then(bdd_state->bddm,t);
	  }
	}
	if (t != bdd_state->Reg)
	  bdd_free(bdd_state->bddm,t);
	bdd_free(bdd_state->bddm,u);
	t = v;
	if (t == bdd_zero(bdd_state->bddm))
	  flag = 1;
      }
    }else{
      t = mtbdd_ite(bdd_state->bddm,M,t,bdd_zero(bdd_state->bddm));
      bdd_assoc(bdd_state->bddm,bdd_state->marks);
      u = bdd_exists(bdd_state->bddm,t);
      bdd_free(bdd_state->bddm,t);
      t = u;
      if (t == bdd_zero(bdd_state->bddm))
	flag = 1;
    }

    /* Traverse the links and compare new region with existing ones. */
    check = flag;
    if (0){
      i = 0;
      v = t;
      for (i=0;((i<bdd_state->maxlinks)&&!flag);i++){
	v = t;
	v = bdd_identity(bdd_state->bddm,t); 
	for (j = 0;((j<bdd_state->linksize)&&!flag);j++){
	  u = bdd_if(bdd_state->bddm,v);
	  if (bdd_state->links[j] == u){
	    if (i & (1 << j)){
	      s = bdd_then(bdd_state->bddm,v);
	    }else{
	      s = bdd_else(bdd_state->bddm,v);
	    }
	    bdd_free(bdd_state->bddm,v); 
	  } else printf("ACK! PROBLEM TRAVERSING LINKS!!!\n");
	  bdd_free(bdd_state->bddm,u);
	  v = s;
	  if (v == bdd_zero(bdd_state->bddm)){
	    flag = 1;
	    i--;
	  }
	}
	if (!flag && (v == U)){
	  flag = 1;
	  bddtimer = 1;
	}
      }
      bdd_free(bdd_state->bddm,v); 
    }
    else{
      i = 0;
      for (i=0;((i<bdd_state->maxlinks)&&!flag);i++){
	v = mtbdd_ite(bdd_state->bddm,bdd_state->L[i],t,
		      bdd_zero(bdd_state->bddm));
	bdd_assoc(bdd_state->bddm,bdd_state->lks);
	u = bdd_exists(bdd_state->bddm,v);
	if (u == bdd_zero(bdd_state->bddm)){
	  flag = 1;
	  i--;
	}
	if (!flag && (u == U)){
	  flag = 1;
	  bddtimer = 1;
	}
	bdd_free(bdd_state->bddm,v);
	bdd_free(bdd_state->bddm,u);
      }
    }

    I=i;
    if (i == bdd_state->maxlinks) {
      printf("*****************LINK SPACE EXHAUSTED*******************\n");
      fprintf(lg,"*****************LINK SPACE EXHAUSTED*******************\n");
    }

    /* If new region add to region MTBDD. */
    if (0) {
      if (flag && !bddtimer){
	u = bdd_and(bdd_state->bddm,M,bdd_state->L[i]);
	v = mtbdd_ite(bdd_state->bddm,u,U,bdd_state->Reg);
	bdd_free(bdd_state->bddm,u);
	bdd_free(bdd_state->bddm,bdd_state->Reg);
	bdd_state->Reg = v;
      }
    } else {
      if (flag && !bddtimer){
	//printf("state=%s,i=%d,j=%d\n",new_state,i,j);
	j=is_power_of_2_minus_1(i);
	w = bdd_state->Reg;
	if (j>=0) {
	  if (j == bdd_state->linksize)
	    printf("ran off the end of the link bits.\n");
	  u = bdd_and(bdd_state->bddm,M,bdd_state->links[j]);
	  w = mtbdd_ite(bdd_state->bddm,u,
			bdd_zero(bdd_state->bddm),bdd_state->Reg);
	  bdd_free(bdd_state->bddm,u);
	  bdd_free(bdd_state->bddm,bdd_state->Reg);
	}
	if (i == bdd_state->maxlinks)
	  printf("ran off the end of the links.\n");
	u = bdd_and(bdd_state->bddm,M,bdd_state->SL[i]);
	v = mtbdd_ite(bdd_state->bddm,u,U,w);
	bdd_free(bdd_state->bddm,u);
	bdd_free(bdd_state->bddm,w);
	bdd_state->Reg = v;
	//	if ((strcmp(new_state,"R0R00")==0) && (i<=1)) {
	//	  printf("separator");
	//bdd_print_bdd(bdd_state->bddm,bdd_state->Reg,bdd_naming_fn_none,
	//			bdd_terminal_id_fn_none,NULL,stdout);
	//	}
      }
    }
    bdd_free(bdd_state->bddm,M);
    bdd_free(bdd_state->bddm,U);
    free(matrixkey);
  }
  if ((check) || (!timed)){
    /****************/
    /* ghost states */
    /****************/
    s = bdd_one(bdd_state->bddm);
    for (i = 0; i< nsigs;i++){
      flag = 0;
      if (( new_state[i]=='0')||( new_state[i]=='R')){
	flag = 1;
	t = bdd_not(bdd_state->bddm,bdd_state->lits[i]);
      }else{
	t = bdd_state->lits[i];
      }
      u = bdd_and(bdd_state->bddm,s,t);
      if (flag)
	bdd_free(bdd_state->bddm,t);
      bdd_free(bdd_state->bddm,s);
      s = u;
    }
    u = bdd_or(bdd_state->bddm,bdd_state->S,s);
    if (!timed) {
      bddtimer=1;
      if (u == bdd_state->S) check=0;
      else check=1;
    }
    bdd_free(bdd_state->bddm,bdd_state->S);
    bdd_state->S = u;
    for (i = 0; i< nsigs;i++){
      if (( new_state[i]=='F')||( new_state[i]=='R')){
	u = bdd_one(bdd_state->bddm);
	for (j = 0; j< nsigs;j++){	
	  flag = 0;
	  if (j == i)
	    if (( new_state[j]=='0')||( new_state[j]=='R')){
	      t = bdd_state->newlits[j];
	    }else{
	      flag = 1;
	      t = bdd_not(bdd_state->bddm,bdd_state->newlits[j]);
	    }
	  else
	    if (( new_state[j]=='0')||( new_state[j]=='R')){
	      flag = 1;
	      t = bdd_not(bdd_state->bddm,bdd_state->newlits[j]);
	    }else{
	      t = bdd_state->newlits[j];
	    }
	  v = bdd_and(bdd_state->bddm,u,t);
	  if (flag)
	    bdd_free(bdd_state->bddm,t);
	  bdd_free(bdd_state->bddm,u);
	  u = v;
	}
	v = bdd_and(bdd_state->bddm,u,s);
	w = bdd_or(bdd_state->bddm,bdd_state->N,v);
	bdd_free(bdd_state->bddm,u);
	bdd_free(bdd_state->bddm,v);
	bdd_free(bdd_state->bddm,bdd_state->N);
	bdd_state->N = w;
      }
    }
    bdd_free(bdd_state->bddm,s);
  }

  /*
    bdd_gc(bdd_state->bddm);
    after=bdd_total_size(bdd_state->bddm);
    Safter=bdd_size(bdd_state->bddm,bdd_state->S,1);
    Nafter=bdd_size(bdd_state->bddm,bdd_state->N,1);
    Rafter=bdd_size(bdd_state->bddm,bdd_state->Reg,1);
    if ((after-before) > 
	((Safter+Nafter+Rafter)-(Sbefore+Nbefore+Rbefore))) {
      leak=leak+(after-before)-
	((Safter+Nafter+Rafter)-(Sbefore+Nbefore+Rbefore));
      printf("LEAK %d %d Total = %d\n",after-before,
	((Safter+Nafter+Rafter)-(Sbefore+Nbefore+Rbefore)),leak);
    }
    */

    //     printf("statespace:\n");
    //     bdd_print_bdd(bddm,S,bdd_naming_fn_none,
    // 		  bdd_terminal_id_fn_none,NULL,stdout);
    //     printf("nextstate:\n");
    //     bdd_print_bdd(bddm,N,bdd_naming_fn_none,
    // 		  bdd_terminal_id_fn_none,NULL,stdout);

    //    printf("c=%d t=%d f=%d\n",check,bddtimer,flag);

  if (check){
    return 1;
  }
  else{
    if (bddtimer){
      return 0;	
    }
    else
      return 2;
  }
}
#else
// CUDD
/* create a minterm representing a state vector. */
#ifdef CUDD
BDD FindStateBDD(Cudd *mgr, char * state, int is_prime,int nsignals, 
		 bdd_set sigs){
  BDD s = mgr->bddOne();
  int i,j;
  for (i=sigs.start+is_prime,j=0;j<nsignals;j++,i+= sigs.step){
    if ((state[j] == '1') ||(state[j] == 'F'))
      s *= mgr->bddVar(i);
    else
      s *= !(mgr->bddVar(i));
  }
  return s;
}
#endif

int bdd_add_state(FILE* fp,stateADT *state_table,char * old_state,
		  char * old_marking,char * old_enablings,
		  clocksADT old_clocks, clockkeyADT old_clockkey,
		  int old_num_clocks, exp_enabledADT old_exp_enabled,
		  char * new_state,char * new_marking,
		  char * new_enablings,int nsignals,
		  clocksADT new_clocks, clockkeyADT new_clockkey, 
		  int new_num_clocks, int sn,bool verbose, bool timed, 
		  int nrules,int old_clock_size, int new_clock_size,
		  bddADT bdd_state,bool use_bdd,markkeyADT *marker,
		  timeoptsADT timeopts,int fire_enabling, int fire_enabled, 
		  exp_enabledADT new_exp_enabled,ruleADT **rules)
{
#ifdef CUDD
  int i,j,state_match = 0,val_match = 0,region_match = 0;
  int removed=0,superset = 0;
  int *matrixkey,*markingnumber;
  unsigned long ptr;
  int ninp = bdd_state->ninpsigs;
  char * prime_state = CopyString(new_state);
  Cudd *mgr;
  mgr = bdd_state->mgr;
  BDD s,s_prime,sin_prime,tmp,tmp2,n1=mgr->bddZero(),n2=mgr->bddZero();
  ADD Atmp,m,state,matrix,index,entry,SuperSet;
  add_list *list_temp;
  add_node *node_temp,*zombie;
  bdd_set sigs = bdd_state->sigs, ruleset = bdd_state->rules;
  char outname[FILENAMESIZE];
  FILE* err;

  // Build state
  s = FindStateBDD(mgr,new_state,FALSE,nsignals,sigs);
  tmp = bdd_state->S + s;
  if (tmp == bdd_state->S)
    val_match++;

  /* build marking minterm (TRUE if nrules = 0) */
  m = mgr->addOne();
  for (i = ruleset.start,j=0;i<ruleset.stop;i+= ruleset.step,j++){
    if (new_marking[j] != 'F')
      m *= mgr->addVar(i);
    else
      m *= ~(mgr->addVar(i));
  }
  state = s.Add()*m;
  Atmp = bdd_state->M & state;
  
  /* convert matrix */
  if (timed){
    /* canonical order listing */
    markingnumber = (int*)GetBlock((nrules)*sizeof(int));
    /* order listing for matrix */
    matrixkey = (int*)GetBlock((nrules+1)*sizeof(int));
    /* build mapping from sparse matrix to canonical marking matrix */
    j = 0;
    //printf("markingnumer:\n");
    for (i = 0;i < nrules;i++) {
      if (new_marking[i] != 'F')
	markingnumber[i] = ++j;
      else
	markingnumber[i] = 0;
      //printf("%d ",markingnumber[i]);
    }
    //printf("\n");
    /* clockkey->rules->markingnumber->matrixkey */
    matrixkey[0]=0;
    //printf("matrixkey:\n");
    for (i=1;i<=new_num_clocks;i++){
      j = rules[new_clockkey[i].enabling][new_clockkey[i].enabled]->marking;
      matrixkey[i] = j+1;//markingnumber[j];
      //printf("%d ",matrixkey[i]);     
    }
    //printf("\n");
   
    /* build ADD matrix */
    matrix = mgr->plusInfinity();
    for (i=0;i<=new_num_clocks;i++)
      for (j=0;j<=new_num_clocks;j++){
	index = bdd_state->row[matrixkey[i]] * bdd_state->col[matrixkey[j]];
	entry = mgr->constant(new_clocks[i][j]);
	matrix = index.Ite(entry,matrix);
      }
    free(markingnumber);
    free(matrixkey);
  }

  //mgr->SetBackground(mgr->plusInfinity());
  // check for uniqueness
  if (Atmp != mgr->addZero()){
    state_match = 1;
    //printf("old state: %s,%s\n",new_state,new_marking);
    if (timed){
      /* check for this matrix in the list */
      /* sigvec->marking->entry */
      entry = Atmp.ExistAbstract(bdd_state->mvars);
      ptr = (unsigned long)Cudd_V((DdNode *)entry.getNode());
      list_temp = (add_list *)ptr;
      /*if (!(strcmp(new_state, "RF111000"))){
	printf("initial list, state %s\n",new_state);
	node_temp = list_temp->head;
	while (node_temp){
	  print_clocks(node_temp->mat,node_temp->clocks);
	  node_temp->data.PrintMinterm();
	  node_temp = node_temp->next;
	}
	}*/
      node_temp = list_temp->head;
      while (node_temp){
	if (new_num_clocks == node_temp->clocks){
	  if (node_temp->data == matrix){
	    region_match = 1;
	    break;
	  } else if ((timeopts.subsets)||(timeopts.supersets)){
	    SuperSet = matrix.Maximum(node_temp->data);
	    if ((timeopts.subsets) && 
		(node_temp->data==SuperSet)){
	      region_match = 1;
	      break;
	    } else if ((timeopts.supersets) &&
		       (matrix==SuperSet)){
	      superset = 1;
	    }
	  }
	}
	if (superset){
	  zombie = node_temp;
	  node_temp = node_temp->next;
	  list_temp->remove(zombie);
	  removed--;
	  superset=0;
	}
	else{
	  node_temp = node_temp->next;
	}
      }
      if (!region_match){
	//printf("new region\n");
	list_temp->insert(matrix,new_num_clocks);
      }
      /*if (!(strcmp(new_state, "RF111000"))){
	printf("terminal list, state %s\n",new_state);
	node_temp = list_temp->head;
	while (node_temp){
	  print_clocks(node_temp->mat,node_temp->clocks);
	  node_temp->data.PrintMinterm();
	  node_temp = node_temp->next;
	}
	}*/
    }
    else
      region_match = 1;
  }
  else{
    bdd_state->S = tmp;
    //printf("new state: %s,%s\n",new_state,new_marking);
    if (verbose)
      fprintf(fp,"%s\n",new_state);
    if (timed){
      /* add new region list */
      list_temp = new add_list;
      list_temp->insert(matrix,new_num_clocks);
      entry = mgr->constant((unsigned long)list_temp);
      bdd_state->M = state.Ite(entry,bdd_state->M);
      if (bdd_state->front){
	bdd_state->back->next = list_temp;
	bdd_state->back = list_temp;
      }
      else{
	bdd_state->front = list_temp;
	bdd_state->back = list_temp;
      }
      /*printf("new list, state %s\n",new_state);
      node_temp = list_temp->head;
      while (node_temp){
	print_clocks(node_temp->mat,node_temp->clocks);
	node_temp = node_temp->next;
      }
      */
    }
    else
      bdd_state->M |= state;
  }
  // add input transitions
  for (i=0;i<ninp;i++){
    if (new_state[i] == 'R'){
      prime_state[i] = '1';
      s_prime = FindStateBDD(mgr,prime_state,TRUE,nsignals,sigs);
      prime_state[i] = 'R';
      n1 += s * s_prime;
    }
    else if(new_state[i] == 'F'){
      prime_state[i] = '0';
      s_prime = FindStateBDD(mgr,prime_state,TRUE,nsignals,sigs);
      prime_state[i] = 'F';
      n1 += s * s_prime;
    }
  }
  // add output transitions
  for (i=ninp;i<nsignals;i++){
    if (new_state[i] == 'R'){
      prime_state[i] = '1';
      s_prime = FindStateBDD(mgr,prime_state,TRUE,nsignals,sigs);
      prime_state[i] = 'R';
      n2 += s * s_prime;
    }
    else if(new_state[i] == 'F'){
      prime_state[i] = '0';
      s_prime = FindStateBDD(mgr,prime_state,TRUE,nsignals,sigs);
      prime_state[i] = 'F';
      n2 += s * s_prime;
    }
  }
  tmp2 = n1+n2;
  /* CSC check */
  if (val_match && !(state_match)){
    sin_prime = FindStateBDD(mgr,new_state,TRUE,ninp,sigs);
    tmp = bdd_state->N * s * sin_prime;
    if (n2 != tmp){
      /* set enablings for conflicting state set. */
      for (i = 0,j=sigs.start; i<nsignals;i++,j+= sigs.step){
	if (prime_state[i] == 'R')
	  prime_state[i] = '0';
	if (prime_state[i] == 'F')
	  prime_state[i] = '1';
	if ((bdd_state->N*s *mgr->bddVar(j) * !(mgr->bddVar(j+1))) !=
	    mgr->bddZero())
	  prime_state[i] = 'F';
	if ((bdd_state->N*s *!(mgr->bddVar(j)) * mgr->bddVar(j+1)) !=
	    mgr->bddZero())
	    prime_state[i] = 'R';
      }
      if (verbose){
	/* dump .err file */
	strcpy(outname,bdd_state->filename);
	strcat(outname,".err");
	if (bdd_state->hasCSC){
	  /* open file for write and dump header */
	  bdd_state->hasCSC = FALSE;
	  err=Fopen(outname,"w");
	  printf("EXCEPTION:  Storing CSC violations to: %s\n",outname);
	  fprintf(lg,"EXCEPTION:  Storing CSC violations to: %s\n",outname);
	  fprintf(err,"CSC VIOLATIONS:\n");
	  fprintf(err,"STATE1:STATE2\n");
	  if (bdd_state->signals) 
	    print_state_vector(err,bdd_state->signals,nsignals,ninp);
        } 
	else
	  /* open file for append */
	  err=Fopen(outname,"a");
	/* dump conflict pairs */
	fprintf(err,"%s:%s\n",prime_state,new_state);
	fclose(err);
      }
      else{
	if (bdd_state->hasCSC){
	  /* just print warning */
	  bdd_state->hasCSC = FALSE;
	  printf("CSC Violation!\n");
	  fprintf(lg,"CSC Violation!\n");
	}
      }
    }
  }
  if (!state_match)
    /* add transitions */
    bdd_state->N += tmp2;
  free(prime_state);
  //mgr->SetBackground(mgr->addZero());
  if (state_match){
    if (region_match)
      return 0;
    else{
      if (removed){
	//printf("removed %d regions\n",-removed); 
	return removed;
      }
      return 2;
    }
  }
  else
    return 1;

  /* -# = # of states removed (supersets)
     0 = old state (and region)
     1 = new state
     2 = new region
  */
#else
  return 0;
#endif
}
#endif

int exp_add_state(FILE* fp,stateADT *state_table,char * old_state,
		  char * old_marking,char * old_enablings,
		  clocksADT old_clocks, clockkeyADT old_clockkey,
		  int old_num_clocks, exp_enabledADT old_exp_enabled,
		  char * new_state,char * new_marking,
		  char * new_enablings,int nsignals,
		  clocksADT new_clocks, clockkeyADT new_clockkey, 
		  int new_num_clocks, int sn,bool verbose, bool timed, 
		  int nrules,int old_clock_size, int new_clock_size,
		  bddADT bdd_state,bool use_bdd,markkeyADT *marker,
		  timeoptsADT timeopts,int fire_enabling, int fire_enabled, 
		  exp_enabledADT new_exp_enabled,ruleADT **rules,
		  int nevents, bool bap, int action_type )
{
  stateADT oldptr=NULL;
  stateADT newptr=NULL;
  statelistADT statelist;
  int timing_match;
  int removed_regions=0;
  int is_new_state=0;
  bool insert=FALSE;
  
  if(timed){
    if (old_state){ 
      oldptr=find_state(state_table,old_state,old_marking,
                        old_enablings, old_clocks, old_clockkey,
                        nsignals, old_num_clocks, 0,
                        &timing_match,&is_new_state, 
                        old_clock_size,timeopts,
                        old_exp_enabled,&removed_regions,rules, 
                        marker, nevents, bap);
      if (oldptr->link==NULL) {
        printf("WARNING: Old state not found.\n");
        fprintf(lg,"WARNING: Old state not found.\n");
      }
    }
    if (new_state)
      newptr=find_state(state_table,new_state,new_marking,
                        new_enablings, new_clocks, new_clockkey, nsignals,
                        new_num_clocks, 1, &timing_match, &is_new_state, 
                        new_clock_size,timeopts,new_exp_enabled,
                        &removed_regions, rules, marker, nevents, bap);
  }
  else{
    if (old_state) {
      oldptr=find_state(state_table,old_state,old_marking,
                        old_enablings, NULL, NULL,
                        nsignals,0, 0, &timing_match,
                        &is_new_state, 0,timeopts, NULL,
                        &removed_regions, rules, marker, 
                        nevents, bap);
      if (oldptr->link==NULL) {
        printf("WARNING: Old state not found.\n");
        fprintf(lg,"WARNING: Old state not found.\n");
      }
    }
    if (new_state)
      newptr=find_state(state_table,new_state,new_marking,
                        new_enablings, NULL, NULL, nsignals,
                        0, 0, &timing_match, &is_new_state,
                        0,timeopts,NULL, &removed_regions,rules, marker,
                        nevents, bap);
  }
  if (!new_state) {
    is_new_state=true;
    new_state=CopyString("FAIL");
    for (newptr=state_table[0];newptr->link != NULL;newptr=newptr->link) {
      if (strcmp(newptr->state,"FAIL")==0) {
        is_new_state=false;
        break;
      }
    }
  }
  if(is_new_state || (timeopts.genrg & !timing_match)){
    newptr->state=CopyString(new_state);
    newptr->marking=CopyString(new_marking);
    newptr->enablings=CopyString(new_enablings);
    newptr->number=sn;
    newptr->color=0;
    newptr->highlight=0;
    newptr->num_clocks=0;
    newptr->probability=0;
    newptr->link=(stateADT)GetBlock(sizeof (*(newptr->link)));
    newptr->link->link=NULL;
    newptr->link->state=NULL; 
    newptr->pred=NULL;
    newptr->succ=NULL;
    newptr->clocklist=NULL;
    if(timed){
      newptr->clocklist=(clocklistADT)GetBlock(sizeof(struct clocklist));
      newptr->clocklist->clocks= (clocksADT)copy_clocks(new_clock_size,
                                                        new_clocks);
      newptr->clocklist->clockkey=(clockkeyADT)copy_clockkey(new_clock_size,
                                                             new_clockkey);
      newptr->clocklist->clocknum=new_num_clocks;
      newptr->clocklist->nrules=nrules;
      newptr->clocklist->clocksize=new_clock_size;
      // Only copy exp_enabled if it is set to a non-NULL ponter!
      if ( new_exp_enabled != NULL ) {
        newptr->clocklist->exp_enabled=
          (exp_enabledADT)GetBlock((strlen(new_exp_enabled)+1) * 
                                   (sizeof(char)));
        strcpy(newptr->clocklist->exp_enabled, new_exp_enabled);
      }
      else {
        newptr->clocklist->exp_enabled = NULL;
      }
      newptr->clocklist->next=NULL;

    }    
    if (old_state) {
      newptr->pred=(statelistADT)GetBlock(sizeof (*(newptr->pred)));
      init( newptr->pred );
      newptr->pred->stateptr=oldptr;
      newptr->pred->enabling=fire_enabling;
      newptr->pred->enabled=fire_enabled;
      newptr->pred->iteration=action_type;
      newptr->pred->next=NULL;
      if (oldptr->succ){
        insert=TRUE;
        for (statelist=oldptr->succ; statelist->next;
             statelist=statelist->next)
          if ((statelist->stateptr==newptr)&&
              //	      (statelist->enabling==fire_enabling)&&
              (statelist->enabled==fire_enabled)) {
            if (action_type==FIRE_TRANS) 
              statelist->iteration=FIRE_TRANS;
            insert=FALSE;
            break;
          }
        if ((statelist->stateptr==newptr)&&
            //	    (statelist->enabling==fire_enabling)&&
            (statelist->enabled==fire_enabled)) insert=FALSE;
        if (insert) {
          statelist->next=(statelistADT)GetBlock(sizeof (*(oldptr->succ)));
          init( statelist->next );
          statelist->next->stateptr=newptr;
          statelist->next->enabling=fire_enabling;
          statelist->next->enabled=fire_enabled;
          statelist->next->iteration=action_type;
          statelist->next->next=NULL;
        }
      } else {
        oldptr->succ=(statelistADT)GetBlock(sizeof (*(oldptr->succ)));
        init( oldptr->succ );
        oldptr->succ->stateptr=newptr;
        oldptr->succ->next=NULL;
        oldptr->succ->enabling=fire_enabling;
        oldptr->succ->enabled=fire_enabled;
        oldptr->succ->iteration=action_type;
      }
    } 
    if (verbose){
      fprintf(fp,"%d:%s\n",sn,new_state);
    }
    return(1);
  }
  if (old_state) {
    if (newptr->pred) {
      insert=TRUE;
      for (statelist=newptr->pred;statelist->next;statelist=statelist->next)
        if (statelist->stateptr==oldptr) {
          //	    (statelist->enabling==fire_enabling) &&
          if ((statelist->enabled==fire_enabled) ||
              (action_type!=FIRE_TRANS)) {
            if (action_type==FIRE_TRANS) 
              statelist->iteration=FIRE_TRANS;
            insert=FALSE;
            break;
          } else {
            /*
              printf("%s --> %s OLD: %d NEW: %d\n",
              old_state,new_state,statelist->enabled,fire_enabled);
            */
          }
        }
      if ((statelist->stateptr==oldptr) &&
          //	  (statelist->enabling==fire_enabling) &&
          ((statelist->enabled==fire_enabled) ||
           (action_type!=FIRE_TRANS))) {
        if (action_type==FIRE_TRANS) 
          statelist->iteration=FIRE_TRANS;
        insert=FALSE;
      }
      if (insert) {
        statelist->next=(statelistADT)GetBlock(sizeof (*(newptr->pred)));
        init( statelist->next );
        statelist->next->stateptr=oldptr;
        statelist->next->enabling=fire_enabling;
        statelist->next->enabled=fire_enabled;
        statelist->next->iteration=action_type;
        statelist->next->next=NULL;
      }
    } else {
      newptr->pred=(statelistADT)GetBlock(sizeof (*(newptr->pred)));
      init( newptr->pred );
      newptr->pred->stateptr=oldptr;
      newptr->pred->next=NULL;
      newptr->pred->enabling=fire_enabling;
      newptr->pred->enabled=fire_enabled;
      newptr->pred->iteration=action_type;
    }
    if (oldptr->succ){
      insert=TRUE;
      for (statelist=oldptr->succ; statelist->next;
           statelist=statelist->next)
        if ((statelist->stateptr==newptr)&&
            //	    (statelist->enabling==fire_enabling)&&
            ((statelist->enabled==fire_enabled) ||
             (action_type!=FIRE_TRANS))) {
          if (action_type==FIRE_TRANS) 
            statelist->iteration=FIRE_TRANS;
          insert=FALSE;
          break;
        }
      if ((statelist->stateptr==newptr)&&
          //	  (statelist->enabling==fire_enabling)&&
          ((statelist->enabled==fire_enabled) ||
           (action_type!=FIRE_TRANS))) 
        insert=FALSE;
      if (insert) {
        statelist->next=(statelistADT)GetBlock(sizeof (*(oldptr->succ)));
        init( statelist->next );
        statelist->next->stateptr=newptr;
        statelist->next->enabling=fire_enabling;
        statelist->next->enabled=fire_enabled;
        statelist->next->iteration=action_type;
        statelist->next->next=NULL;
      }
    } else {
      oldptr->succ=(statelistADT)GetBlock(sizeof (*(oldptr->succ)));
      init( oldptr->succ );
      oldptr->succ->stateptr=newptr;
      oldptr->succ->next=NULL;
      oldptr->succ->enabling=fire_enabling;
      oldptr->succ->enabled=fire_enabled;
      oldptr->succ->iteration=action_type;
    }
  }
  if(timed){
    if((!is_new_state) && (!timing_match) && (removed_regions==0)){
      //printf("Timing match to state: %d\n",newptr->number);
      return(2);
    }
    if((!is_new_state) && (!timing_match) && (removed_regions>0)) {
      //printf("Superset match to state: %d\n",newptr->number);
      return(-(removed_regions));
    }
  }
  return(FALSE);
}

int add_state(FILE* fp,stateADT *state_table,char * old_state,
	      char * old_marking,char * old_enablings,
	      clocksADT old_clocks, clockkeyADT old_clockkey,
	      int old_num_clocks, exp_enabledADT old_exp_enabled,
	      char * new_state,char * new_marking,
	      char * new_enablings,int nsignals,
	      clocksADT new_clocks, clockkeyADT new_clockkey, 
	      int new_num_clocks, int sn,bool verbose, bool timed, int nrules,
	      int old_clock_size, int new_clock_size,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,timeoptsADT timeopts,
	      int fire_enabling, int fire_enabled, 
	      exp_enabledADT new_exp_enabled,ruleADT **rules, int nevents,
	      bool bap, int action_type)
{
  int result;

#ifdef LIMIT
  if (sn > LIMIT) {
    printf (   "Too many states (%d > LIMIT=%d) - bailing out!\n",sn,LIMIT);
    fprintf(lg,"Too many states (%d > LIMIT=%d) - bailing out!\n",sn,LIMIT);
    fclose (lg                                                            );
    exit   (                                                      sn      );
  }
#endif

  if (use_bdd){
    result=bdd_add_state(fp,state_table,old_state,old_marking,old_enablings,
			 old_clocks,old_clockkey,old_num_clocks,
			 old_exp_enabled,new_state,new_marking,
			 new_enablings,nsignals,new_clocks,new_clockkey, 
			 new_num_clocks,sn,verbose,timed,nrules,
			 old_clock_size,new_clock_size,bdd_state,
			 use_bdd,marker,timeopts,fire_enabling,fire_enabled, 
			 new_exp_enabled,rules);
  } else {
    result=exp_add_state(fp,state_table,old_state,old_marking,old_enablings,
			 old_clocks,old_clockkey,old_num_clocks,
			 old_exp_enabled,new_state,new_marking,
			 new_enablings,nsignals,new_clocks,new_clockkey, 
			 new_num_clocks,sn,verbose,timed,nrules,
			 old_clock_size,new_clock_size,bdd_state,
			 use_bdd,marker,timeopts,fire_enabling,fire_enabled, 
			 new_exp_enabled,rules,nevents,bap,action_type);

  }
  /*printf("traversing link %s -> %s result=%d;\n",old_state,new_state,
	 result);
  print_clocks(new_clocks,new_num_clocks);
  printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  */
  if ((fire_enabled > 0) && (fire_enabled < nevents))
    for (int i=0;i<nevents;i++)
      rules[fire_enabled][i]->reachable=TRUE;
  return result;
}

ver_stateADT ver_find_state(ver_stateADT *state_table, char * state, 
			    char * marking, int nsignals,
			    clocksADT clocks, clockkeyADT clockkey,
			    int num_clocks, int clock_size, 
			    char * imp_state, char * imp_marking, 
			    clocksADT  imp_clocks, 
			    clockkeyADT imp_clockkey, 
			    int imp_num_clocks,  int imp_clock_size, 
			    int *timing_match, int *is_new_state){

  int i,j,position;
  ver_stateADT e;
  char * vstate;
  ver_clocklistADT clockptr;
  ver_clocklistADT prev;
  int *match;
  int *imp_match;
  int matches=0;
  int imp_matches=0;

  vstate=SVAL(state, nsignals);
  if ((strlen(vstate) < 2) && (marking))
    position=hashpjw(marking);
  else
    position=hashpjw(vstate);
  free(vstate);
  *timing_match=0;
  *is_new_state=1;
  match=(int *)GetBlock((num_clocks+1)*sizeof(int));
  imp_match=(int *)GetBlock((imp_num_clocks+1)*sizeof(int));
  for (e = state_table[position];e->next != NULL;e = e->next){ 
    if((strcmp(e->state,state)==0) &&
       (strcmp(e->marking,marking)==0) &&
       (strcmp(e->imp_state,imp_state)==0) &&
       (strcmp(e->imp_marking, imp_marking)==0)){
      *is_new_state=0;
      clockptr=e->clocklist;
      prev=e->clocklist;
      match[0]=0;
      imp_match[0]=0;
      while(clockptr!=NULL){
	*timing_match=1;
	matches=0;
	imp_matches=0;
	if((clockptr->spec_clocknum != num_clocks) ||
	   (clockptr->imp_clocknum != imp_num_clocks)){
	  *timing_match=0;
	}
	for(i=1;i<=num_clocks;i++){
	  for(j=1;j<=num_clocks;j++){
	    if((clockkey[i].enabling == clockptr->spec_clockkey[j].enabling) &&
	       (clockkey[i].enabled == clockptr->spec_clockkey[j].enabled)){
	      match[i]=j;
	      matches++;
	      break;
	    }
	  }
	}
	if(matches!=num_clocks) (*timing_match=0);      
	for(i=1;i<=imp_num_clocks;i++){
	  for(j=1;j<=imp_num_clocks;j++){
	    if((imp_clockkey[i].enabling == clockptr->imp_clockkey[j].enabling) &&
	       (imp_clockkey[i].enabled == clockptr->imp_clockkey[j].enabled)){
	      imp_match[i]=j;
	      imp_matches++;
	      break;
	    }
	  }
	}
	if(imp_matches!=imp_num_clocks) (*timing_match=0);       
	if(*timing_match){
	  for(i=0;i<=num_clocks;i++){
	    for(j=0;j<=num_clocks;j++){
	      if (clocks[i][j] > clockptr->spec_clocks[match[i]][match[j]]){
		*timing_match=0;
		break;
	      }
	    }
	    if(!(*timing_match)){
	      break;
	    }
	  }
	}      
	if(*timing_match){
	  for(i=0;i<=imp_num_clocks;i++){
	    for(j=0;j<=imp_num_clocks;j++){
	      if (imp_clocks[i][j] > clockptr->imp_clocks[imp_match[i]][imp_match[j]]){
		*timing_match=0;
		break;
	      }
	    }
	    if(!(*timing_match)){
	      break;
	    }
	  }
	}   	
	if((*timing_match)){
	  free(match);
	  return(e);
	}
	prev=clockptr;
	clockptr=clockptr->next;
      }
      prev->next=(ver_clocklistADT)GetBlock(sizeof(struct ver_clocklist));
      prev->next->spec_clockkey=(clockkeyADT)copy_clockkey(clock_size, clockkey);
      prev->next->imp_clockkey=(clockkeyADT)copy_clockkey(imp_clock_size, imp_clockkey);    
      prev->next->spec_clocks=(clocksADT)copy_clocks(clock_size, clocks);
      prev->next->imp_clocks=(clocksADT)copy_clocks(imp_clock_size, imp_clocks);
      prev->next->spec_clocknum=num_clocks;
      prev->next->imp_clocknum=imp_num_clocks;
      prev->next->next=NULL;    
      free(match);
      return(e);
    }
  }
  free(match);
  return(e);
} 


int ver_add_state(ver_stateADT *state_table, char * new_state,
		  char * new_marking, int nsignals, clocksADT new_clocks,
		  clockkeyADT new_clockkey,int new_num_clocks,int nrules,
		  int sn,int new_clock_size, char * imp_new_state,
		  char * imp_new_marking,clocksADT imp_new_clocks,
		  clockkeyADT imp_new_clockkey, int imp_new_num_clocks, 
		  int imp_new_clock_size, int nimp_rules){
  
  ver_stateADT newptr;
  int timing_match;
  int is_new_state=0;

  newptr=ver_find_state(state_table,new_state,new_marking,nsignals,new_clocks,
			new_clockkey,new_num_clocks,new_clock_size,
			imp_new_state,imp_new_marking,imp_new_clocks,
			imp_new_clockkey,imp_new_num_clocks,imp_new_clock_size,
			&timing_match,&is_new_state);

  if(is_new_state){
    newptr->state=CopyString(new_state);
    newptr->marking=CopyString(new_marking);
    newptr->imp_state=CopyString(imp_new_state);
    newptr->imp_marking=CopyString(imp_new_marking);    
    newptr->number=sn;   
    newptr->next=(ver_stateADT)GetBlock(sizeof (*(newptr->next)));
    newptr->next->next=NULL;
    newptr->next->state=NULL; 
    newptr->next->imp_state=NULL;
    newptr->clocklist=(ver_clocklistADT)GetBlock(sizeof(struct ver_clocklist));
    newptr->clocklist->spec_clocks= (clocksADT)copy_clocks(new_clock_size, 
							   new_clocks);
    newptr->clocklist->spec_clockkey=(clockkeyADT)copy_clockkey(new_clock_size, 
								new_clockkey);
    newptr->clocklist->spec_clocknum=new_num_clocks;
    newptr->clocklist->spec_nrules=nrules;
    newptr->clocklist->imp_clocks= (clocksADT)copy_clocks(imp_new_clock_size, 
							   imp_new_clocks);
    newptr->clocklist->imp_clockkey=(clockkeyADT)copy_clockkey(imp_new_clock_size, 
								imp_new_clockkey);
    newptr->clocklist->imp_clocknum=imp_new_num_clocks;
    newptr->clocklist->imp_nrules=nimp_rules;
    newptr->clocklist->next=NULL;    
    return(1);
  }
  if((!is_new_state) && (!timing_match)){
    return(2);
  }
  return(FALSE);
}

    

/*****************************************************************************/
/* Pop marking onto stack of markings.                                       */
/*****************************************************************************/

markingADT pop_marking(markingsADT *marking_stack,char * *firelist)
{
  markingADT marking;
  markingsADT old_markingstack;

  if (*marking_stack==NULL)
    return(NULL);
  marking=(*marking_stack)->marking;
  (*firelist)=(*marking_stack)->firelist;
  old_markingstack=(*marking_stack);
  (*marking_stack)=(*marking_stack)->link;
  free(old_markingstack);
  return(marking);
}

/*****************************************************************************/
/* Check if an enabled event always occurs after some other enabled event.   */
/*****************************************************************************/

bool slow(bool *warning,eventADT *events,ruleADT **rules,markkeyADT *markkey,
	  cycleADT ****cycles,
	  int nevents,int ncycles,int nsignals,int event,markingADT marking,
	  bool closure,int nrules)
{
  int i;
  int k,l;
  boundADT timediff=NULL;

  if (marking->state[events[event]->signal]=='R') {
    k=marking->up[(event-1)/2];
  } else {
    k=marking->down[(event-1)/2];
  }
  for (i=1/*0*/;i<nevents;i++)
    if ((events[i]->signal != events[event]->signal) && 
	(event_enabled(events,rules,markkey,marking->marking,i,nrules,
		       FALSE, marking, nsignals,TRUE,FALSE,TRUE))) {
      if (marking->state[events[i]->signal]=='R') {
	l=marking->up[(i-1)/2];
      } else {
	l=marking->down[(i-1)/2];
      }
      if (l >= k) {
	if (closure)
	  timediff=TSE_WCTimeDiff(event,i,l-k);
	else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,event,i,l-k);
	if (timediff->U < 0) {
	  free(timediff);
	  return(TRUE);
	}
	if (timediff->Ubest < 0) (*warning)=TRUE; 
	free(timediff);
      } else {
	if (closure) 
	  timediff=TSE_WCTimeDiff(i,event,k-l);
	else timediff=WCTimeDiff(rules,cycles,nevents,ncycles,i,event,k-l);
	if (timediff->L > 0) {
	  free(timediff);
	  return(TRUE);
	}
	if (timediff->Lbest > 0) (*warning)=TRUE; 
	free(timediff);
      }
    }
  return(FALSE);
}

void print_reset_markings(eventADT *events,markkeyADT *markkey,
			  markingADT marking,int nrules)
{
  int i;
  bool print;

  print=FALSE;
  for (i=0;i<nrules;i++)
    if ((markkey[i]->enabling==0) && (marking->marking[i]!='F'))
      printf("%s reset -> %s\n",marking->state,
	     events[markkey[i]->enabled]->event);
}

void print_stuck_info(eventADT *events,ruleADT **rules,
		      markkeyADT *markkey,markingADT marking,
		      int nevents,int nrules,bool verbose,
		      int nsigs, bool verification,bool noparg)
{
  if (!noparg)
    print_grf("deadlock",events,rules,nevents,TRUE,marking); 
  return;
  /*
  int j;

  if(!verification){
    if (!verbose) printf("ERROR\n");
    printf("ERROR: STUCK IN STATE=%s\n",marking->state);
    fprintf(lg,"ERROR: STUCK IN STATE=%s\n",marking->state);
    printf("ENABLED RULES: \n");
    fprintf(lg,"ENABLED RULES: \n");
    for (j=0;j<nrules;j++)
      if ((marking->marking[j]!='F') &&
	  (rules[markkey[j]->enabling][markkey[j]->enabled]->epsilon!=(-1))){
	printf("%s %s %s %d %d",events[markkey[j]->enabling]->event,
	       events[markkey[j]->enabled]->event,
	       rules[markkey[j]->enabling][markkey[j]->enabled]->expr,
	       rules[markkey[j]->enabling][markkey[j]->enabled]->epsilon,
	       rules[markkey[j]->enabling][markkey[j]->enabled]->lower);
	if (rules[markkey[j]->enabling][markkey[j]->enabled]->upper==
	    INFIN) 
	  printf(" inf\n");
	else printf(" %d\n",
		    rules[markkey[j]->enabling][markkey[j]->enabled]->upper);
	fprintf(lg,"%s %s %s %d %d",events[markkey[j]->enabling]->event,
		events[markkey[j]->enabled]->event,
		rules[markkey[j]->enabling][markkey[j]->enabled]->expr,
		rules[markkey[j]->enabling][markkey[j]->enabled]->epsilon,
		rules[markkey[j]->enabling][markkey[j]->enabled]->lower);
	if (rules[markkey[j]->enabling][markkey[j]->enabled]->upper==
	    INFIN) 
	  fprintf(lg," inf\n");
	else fprintf(lg," %d\n",
		     rules[markkey[j]->enabling][markkey[j]->enabled]->upper);
      }
  }
  */
}

/*****************************************************************************/
/* Find new marking given that signal i fired.                               */
/*****************************************************************************/

bool fire_transition(eventADT *events,ruleADT **rules,
		     markkeyADT *markkey,markingADT old_marking,
		     markingADT marking,int i,int nevents,int nrules,int nsigs,
		     bool verbose)
{
  int j;

/*   print_reset_markings(events,markkey,marking,nrules); */
  if ((marking->state[events[i]->signal]=='R') &&
      (event_enabled(events,rules,markkey,old_marking->marking,i,nrules,FALSE, 
		     marking, nsigs,TRUE,FALSE,TRUE))) {
    if (!event_enabled(events,rules,markkey,old_marking->marking,i,nrules,
		       FALSE,marking, nsigs,TRUE,TRUE,TRUE)) {
      if (!verbose) {
	printf("ERROR!\n");
	fprintf(lg,"ERROR!\n");
      }
      printf("ERROR: Ordering rule violation in state %s.\n",
	     marking->state);
      fprintf(lg,"ERROR: Ordering rule violation in state %s.\n",
	      marking->state);
      return FALSE;
    }

    for (j=0;j<nrules;j++) {
      if ((marking->marking[j]!='F') && 
	  (rules[i][markkey[j]->enabled]->conflict & OCONFLICT) &&
	  (rules[markkey[j]->enabling][i]->ruletype > NORULE) &&
	  (rules[markkey[j]->enabling][i]->epsilon==
	   rules[markkey[j]->enabling][markkey[j]->enabled]->epsilon))
	marking->marking[j]='F';

      if (markkey[j]->enabled==i)
	marking->marking[j]='F';
      if ((markkey[j]->enabling==i) && 
	  (rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype > NORULE)
	  && (!(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype & 
		REDUNDANT))) {
	if (marking->marking[j]!='F') {
	  if (!verbose) {
	    printf("ERROR!\n");
	    fprintf(lg,"ERROR!\n");
	  }
	  printf("ERROR: Safety violation for rule %s -> %s.\n",
		 events[markkey[j]->enabling]->event,
		 events[markkey[j]->enabled]->event);
	  fprintf(lg,"ERROR: Safety violation for rule %s -> %s.\n",
		  events[markkey[j]->enabling]->event,
		  events[markkey[j]->enabled]->event);
	  return FALSE;
	} else {
	    marking->marking[j]='T';
	}
      }
    }
/*     print_reset_markings(events,markkey,marking,nrules); */
/*
    for (j=0;j<nrules;j++)
      if (markkey[j]->enabling==(2*i+1))
	for (k=0;k<nrules;k++)
	  if ((rules[2*i+1][markkey[k]->enabling]->conflict) &&
	      (markkey[j]->enabled==markkey[k]->enabled))
	    marking->marking[k]='T';
*/
    marking->enablings[i]='F';
    marking->state[events[i]->signal]='1';
    marking->up[events[i]->signal]++; 
    if (marking->up[events[i]->signal]==0)
      marking->up[events[i]->signal]++; 
  } else if ((marking->state[events[i]->signal]=='F') &&
	     (event_enabled(events,rules,markkey,old_marking->marking,i,nrules,
			    FALSE, marking, nsigs, TRUE,FALSE,TRUE))) {
    if (!event_enabled(events,rules,markkey,old_marking->marking,i,nrules,
		       FALSE,marking, nsigs,TRUE,TRUE,TRUE)) {
      if (!verbose) {
	printf("ERROR!\n");
	fprintf(lg,"ERROR!\n");
      }
      printf("ERROR: Ordering rule violation in state %s.\n",
	     marking->state);
      fprintf(lg,"ERROR: Ordering rule violation in state %s.\n",
	      marking->state);
      return FALSE;
    }
    for (j=0;j<nrules;j++) {
      if ((marking->marking[j]!='F') && 
	  (rules[i][markkey[j]->enabled]->conflict & OCONFLICT) &&
	  (rules[markkey[j]->enabling][i]->ruletype > NORULE) &&
	  (rules[markkey[j]->enabling][i]->epsilon==
	   rules[markkey[j]->enabling][markkey[j]->enabled]->epsilon))
	marking->marking[j]='F';

      if (markkey[j]->enabled==i)
	marking->marking[j]='F';
      if ((markkey[j]->enabling==i) && 
	  (rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype > NORULE)
	  && (!(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype & 
	   REDUNDANT)))
	if (marking->marking[j]!='F') {
	  if (!verbose) {
	    printf("ERROR!\n");
	    fprintf(lg,"ERROR!\n");
	  }
	  printf("ERROR: Safety violation for rule %s -> %s.\n",
		 events[markkey[j]->enabling]->event,
		 events[markkey[j]->enabled]->event);
	  fprintf(lg,"ERROR: Safety violation for rule %s -> %s.\n",
		 events[markkey[j]->enabling]->event,
		 events[markkey[j]->enabled]->event);
	  return FALSE;
	} else {
	    marking->marking[j]='T';
	}
    }
/*
    for (j=0;j<nrules;j++)
      if (markkey[j]->enabling==(2*i+2))
	for (k=0;k<nrules;k++)
	  if ((rules[2*i+2][markkey[k]->enabling]->conflict) &&
	      (markkey[j]->enabled==markkey[k]->enabled))
	    marking->marking[k]='T';
*/
    marking->enablings[i]='F';
    marking->state[events[i]->signal]='0';
    marking->down[events[i]->signal]++; 
    if (marking->down[events[i]->signal]==0)
      marking->down[events[i]->signal]++; 
  } else {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Firing of %s is a vacuous transition.\n",
	   events[i]->event);
    fprintf(lg,"ERROR: Firing of %s is a vacuous transition.\n",
	   events[i]->event);
    return FALSE;
  }
  return TRUE;
}

/*****************************************************************************/
/* Find new marking given that dummy event i fired.                          */
/*****************************************************************************/

bool fire_dummy_event(eventADT* events, ruleADT **rules,markkeyADT *markkey,
		      markingADT marking,int i,int nrules,int nsigs,
		      bool verbose)
{
  int j;

  // 04/02/01 -- egm
  // This code checks that all ordering rules for a given dummy event
  // i are marked before trying to fire i.  If an ordering rule is NOT
  // marked, then a ordering rule violation is flagged.
  if (!event_enabled(events,rules,markkey,marking->marking,i,nrules,
		     FALSE,marking,nsigs,TRUE,TRUE,TRUE)) {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Ordering rule violation in state %s.\n",
	   marking->state);
    fprintf(lg,"ERROR: Ordering rule violation in state %s.\n",
	    marking->state);
    return FALSE;
  }
  for (j=0;j<nrules;j++) {
    if ((marking->marking[j]!='F') && 
	(rules[i][markkey[j]->enabled]->conflict & OCONFLICT) &&
	(rules[markkey[j]->enabling][i]->ruletype > NORULE) &&
	(rules[markkey[j]->enabling][i]->epsilon==
	 rules[markkey[j]->enabling][markkey[j]->enabled]->epsilon))
      marking->marking[j]='F';
    if (markkey[j]->enabled==i)
      marking->marking[j]='F';
    if ((markkey[j]->enabling==i) && 
	(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype > NORULE)
	&& (!(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype & 
	      REDUNDANT))) {
      // 03/29/01 -- egm
      // If a rule is ALREADY marked, then we have a safety violation
      // Output the standard error message and return a false status.
      if (marking->marking[j]!='F') {
	if (!verbose) {
	  printf("ERROR!\n");
	  fprintf(lg,"ERROR!\n");
	}
	printf("ERROR: Safety violation for rule %s -> %s.\n",
	       events[markkey[j]->enabling]->event,
	       events[markkey[j]->enabled]->event);
	fprintf(lg,"ERROR: Safety violation for rule %s -> %s.\n",
	       events[markkey[j]->enabling]->event,
	       events[markkey[j]->enabled]->event);
	return( false ); 
      }
      else {
	  marking->marking[j]='T';
      }
    }
  }
  return( true );
}

markingADT update_imp_marking(markingADT old_marking, int fire_enabled, ruleADT **rules,
			    int nimp_events, int ninputs, eventADT *events){

  markingADT marking=NULL;
  marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=CopyString(old_marking->marking);
  marking->enablings=CopyString(old_marking->enablings);
  marking->state=CopyString(old_marking->state);
  marking->up=CopyString(old_marking->up);
  marking->down=CopyString(old_marking->down);

  if((fire_enabled%2)==0)
    marking->state[events[fire_enabled]->signal]='0';      
  else
    marking->state[events[fire_enabled]->signal]='1';
  add_enablings(marking, rules, nimp_events, ninputs);
  return marking;
}

/*****************************************************************************/
/* Update state after event has fired.                                       */
/*****************************************************************************/

bool update_state(eventADT *events,ruleADT **rules,
		  markkeyADT *markkey,markingADT marking,
		  int nevents,int nrules,int nsigs,int ninpsig,bool verbose,
		  bool disabling)
{
  int j;
  bool enabled;
  int oldsig;
  bool *currently_enabled;
  bool *enevents;

  oldsig=0;
  enabled=FALSE;
  currently_enabled=(bool *)GetBlock(nevents * sizeof(bool));
  for (j=1;j<(nevents-1);j+=2) {
    if (events[j]->signal >= 0)
      currently_enabled[j]=(marking->state[events[j]->signal]=='R');
    if (events[j+1]->signal >= 0)
      currently_enabled[j+1]=(marking->state[events[j+1]->signal]=='F');
  }
  enevents=events_enabled(events,rules,markkey,marking->marking,nrules,
			  marking,nsigs,currently_enabled,FALSE,FALSE,nevents);
  free(currently_enabled);
  for (j=1;j<nevents;j+=2) 
    if (events[j]->signal >= 0) { 
      /*
      currently_enabled=(marking->state[events[j]->signal]=='R');
      if (event_enabled(events,rules,markkey,marking->marking,j,nrules,FALSE, 
			marking, nsigs,currently_enabled,FALSE,FALSE))
	enabled=TRUE;
      */
      if (enevents[j]) enabled=TRUE;
      if ((j >= (nevents-2)) || (events[j+2]->signal != oldsig)) {
	if (enabled) {
	  if ((marking->state[events[j]->signal]!='0') &&
	      (marking->state[events[j]->signal]!='R')) {
	    if (events[j]->signal >= ninpsig) {
	      if (!verbose) {
		printf("ERROR!\n");
		fprintf(lg,"ERROR!\n");
	      }
	      printf("ERROR: %s enabled but signal is not 0 or R in state %s\n",
		     events[j]->event,marking->state);
	      fprintf(lg,"ERROR: %s enabled but signal is not 0 or R in state %s\n",
		      events[j]->event,marking->state);
	      return TRUE;
	    }
	  } else 
	    marking->state[events[j]->signal]='R';
	  marking->enablings[j]='T'; 
	} else {
	  if (marking->state[events[j]->signal]=='R') {
	    if (events[j]->signal >= ninpsig) { 
	      if (disabling) {
		if (!disabled[j]) {
		  printf("WARNING: Output event %s is disabled\n",
			 events[j]->event);
		  fprintf(lg,"WARNING: Output event %s is disabled\n",
			  events[j]->event);
		  disabled[j]=true;
		}
	      } else {
		if (!verbose) {
		  printf("ERROR!\n");
		  fprintf(lg,"ERROR!\n");
		}
		printf("ERROR: Output event %s is disabled\n",
		       events[j]->event);
		fprintf(lg,"ERROR: Output event %s is disabled\n",
			events[j]->event);
		return TRUE;
	      }
	    }
	    marking->state[events[j]->signal]='0';
	  }
	  marking->enablings[j]='F';
	}
	if (j < (nevents-2)) oldsig=events[j+2]->signal;
	enabled=FALSE;
      }
    }
  oldsig=0;
  enabled=FALSE;
  for (j=2;j<nevents;j+=2) 
    if (events[j]->signal >= 0) { 
      /*
      currently_enabled=(marking->state[events[j]->signal]=='F');
      if (event_enabled(events,rules,markkey,marking->marking,j,nrules,FALSE, 
			marking, nsigs,currently_enabled,
			FALSE,FALSE)) enabled=TRUE;
      */
      if (enevents[j]) enabled=TRUE;
      if ((j >= (nevents-2)) || (events[j+2]->signal != oldsig)) {
	if (enabled) {
	  if ((marking->state[events[j]->signal]!='1') &&
	      (marking->state[events[j]->signal]!='F')) {
	    if (events[j]->signal >= ninpsig) {
	      if (!verbose) {
		printf("ERROR!\n");
		fprintf(lg,"ERROR!\n");
	      }
	      printf("ERROR: %s enabled but signal is not 1 or F in state %s\n",
		     events[j]->event,marking->state);
	      fprintf(lg,"ERROR: %s enabled but signal is not 1 or F in state %s\n",
		      events[j]->event,marking->state);
	      return TRUE;
	    }
	  } else
	    marking->state[events[j]->signal]='F';
	  marking->enablings[j]='T';
	} else {
	  if (marking->state[events[j]->signal]=='F') {
	    if (events[j]->signal >= ninpsig) { 
	      if (disabling) { 
		if (!disabled[j]) {
		  printf("WARNING: Output event %s is disabled\n",
			 events[j]->event); 
		  fprintf(lg,"WARNING: Output event %s is disabled\n",
			  events[j]->event); 
		  disabled[j]=true;
		}
	      } else {
		if (!verbose) {
		  printf("ERROR!\n");
		  fprintf(lg,"ERROR!\n");
		}
		printf("ERROR: Output event %s is disabled\n",
		       events[j]->event); 
		fprintf(lg,"ERROR: Output event %s is disabled\n",
			events[j]->event); 
		return TRUE;
	      }
	    }
	    marking->state[events[j]->signal]='1';
	  }
	  marking->enablings[j]='F';
	}
	if (j < (nevents-2)) oldsig=events[j+2]->signal;
	enabled=FALSE;
      }
    }
  free(enevents);
  return FALSE;
}

void find_satisfied(ruleADT **rules,markkeyADT *markkey,markingADT marking,
		    int nrules,int nsigs)
{
  int i;

  for (i=0;i<nrules;i++)
    if ((marking->marking[i]=='T') &&
	(level_satisfied(rules, marking, nsigs, markkey[i]->enabling, 
			 markkey[i]->enabled)) && 
	(!(rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype 
	   & DISABLING))) 
      marking->marking[i]='S';
}

/*****************************************************************************/
/* Find new marking given that signal i fired.                               */
/*****************************************************************************/

markingADT find_new_marking(eventADT *events,ruleADT **rules,
			    markkeyADT *markkey,markingADT old_marking,
			    int i,int nevents,int nrules,bool verbose,
			    int nsigs,int ninpsig,bool disabling,bool noparg)
{
  markingADT marking=NULL;
  bool verification=FALSE;

  marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=CopyString(old_marking->marking);
  marking->enablings=CopyString(old_marking->enablings);
  marking->state=CopyString(old_marking->state);
  marking->up=CopyString(old_marking->up);
  marking->down=CopyString(old_marking->down);

  // This is used for bap_findrsg to count references to this allocated
  // marking -- egm 12/06/00
  marking->ref_count = 0;

  if (!fire_transition(events,rules,markkey,old_marking,marking,i,nevents,
		       nrules,nsigs,verbose)) {
    print_stuck_info(events,rules,markkey,marking,nevents,
		     nrules,verbose,nsigs,verification,noparg);
    return NULL;
  }
  if (update_state(events,rules,markkey,marking,nevents,nrules,nsigs,
		   ninpsig,verbose,disabling)) {
    print_stuck_info(events,rules,markkey,marking,nevents,
		     nrules,verbose,nsigs,verification,noparg);
    return NULL;
  }
  find_satisfied(rules,markkey,marking,nrules,nsigs);
  return marking;
}

/*****************************************************************************/
/* Find new marking given that dummy event i fired.                          */
/*****************************************************************************/

markingADT dummy_find_new_marking(eventADT *events,ruleADT **rules,
				  markkeyADT *markkey,markingADT old_marking,
				  int i,int nevents,int nrules,bool verbose,
				  int nsigs,int ninpsig,bool disabling,
				  bool noparg)
{
  markingADT marking=NULL;
  bool verification=FALSE;

  marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=CopyString(old_marking->marking);
  marking->enablings=CopyString(old_marking->enablings);
  marking->state=CopyString(old_marking->state);
  marking->up=CopyString(old_marking->up);
  marking->down=CopyString(old_marking->down);

  // This is used for bap_findrsg to count references to this allocated
  // marking -- egm 12/06/00
  marking->ref_count = 0;

  // 03/29/01 -- egm
  // If the fire_dummy_event returns false, then we are in a error
  // situation.  Most likely a safetly failure!  Print the error
  // trace with the ER and return an empy marking.
  if (!fire_dummy_event(events,rules,markkey,marking,i,nrules,nsigs,verbose)) {
    print_stuck_info(events,rules,markkey,marking,nevents,
		     nrules,verbose,nsigs,verification,noparg);
    // Do not forget to clean up memory for the unused marking!
    free( marking );
    return NULL;
  }

  if (update_state(events,rules,markkey,marking,nevents,nrules,nsigs,
		   ninpsig,verbose,disabling)) {
    print_stuck_info(events,rules,markkey,marking,nevents,
		     nrules,verbose,nsigs,verification,noparg);
    return NULL;
  }
  find_satisfied(rules,markkey,marking,nrules,nsigs);
  return marking;
}

char EVAL(char bit) 
{
  if (bit=='D') return 'F';
  if (bit=='U') return 'R';
  return bit;
}

/*****************************************************************************/
/* Check if two states have same code but different outputs are enabled.     */
/*****************************************************************************/

bool csc_violation(char * state1,char * state2,int ninpsig,int nsignals)
{
  int i;
  bool final1,final2,comp1,comp2;

  for (i=0;i<nsignals;i++)
    if ((VAL(state1[i]) != VAL(state2[i])) &&
	(state1[i]!='U') && (state1[i]!='D') && (state2[i]!='U') &&
 	(state2[i]!='D')) 
	//	(VAL(state1[i]) != 'X') && (VAL(state2[i]) != 'X'))
      return(FALSE);

  // ARE X's handled okay?
  comp1=FALSE;
  comp2=FALSE;
  for (i=0;i<nsignals;i++) {
    if ((state1[i]=='D')||(state1[i]=='U')) comp1=TRUE;
    if ((state2[i]=='D')||(state2[i]=='U')) comp2=TRUE;
  }

  if (comp1) {
    final1=TRUE;
    for (i=0;i<nsignals;i++)
      if (((state1[i]=='D')&&(VAL(state2[i])=='1'))||
	  ((state1[i]=='U')&&(VAL(state2[i])=='0'))) {
	final1=FALSE;
	break;
      }
    if (final1) return FALSE;
  }

  if (comp2) {
    final2=TRUE;
    for (i=0;i<nsignals;i++) 
      if (((VAL(state1[i])=='1')&&(state2[i]=='D'))||
	  ((VAL(state1[i])=='0')&&(state2[i]=='U'))) { 
	final2=FALSE;
	break;
      }
    if (final2) return FALSE;
  }

  for (i=ninpsig;i<nsignals;i++)
    if (EVAL(state1[i]) != EVAL(state2[i])) return(TRUE);
  return(FALSE);
}

/*****************************************************************************/
/* Check that state graph satisfies CSC.                                     */
/*****************************************************************************/

bool check_csc(char * filename,signalADT *signals,stateADT *state_table,
	       int nsigs,int ninpsig,bool verbose)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int i,j;
  stateADT curstate1,curstate2;
  bool first;

  printf("Checking CSC ... ");
  fprintf(lg,"Checking CSC ... ");
  fflush(stdout);
  first=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) 
      for (j=i;j<PRIME;j++)
	for (curstate2=(i==j) ? curstate1->link : state_table[j];
	     curstate2 != NULL && curstate2->state != NULL;
	     curstate2=curstate2->link) {
	  //curstate2=curstate1->link;
      while (curstate2 != NULL && curstate2->state != NULL) {
	if (csc_violation(curstate1->state,curstate2->state,ninpsig,nsigs)) {
	  if (first) {
	    printf("NOT complete state coded\n");
	    fprintf(lg,"NOT complete state coded\n");
	    if (!verbose) {
	      printf("EXCEPTION:  CSC violation!\n");
	      fprintf(lg,"EXCEPTION:  CSC violation!\n");
	      return(FALSE);
	    }
	    strcpy(outname,filename);
	    strcat(outname,".err");
	    printf("EXCEPTION:  Storing CSC violations to:  %s\n",outname);
	    fprintf(lg,"EXCEPTION:  Storing CSC violations to:  %s\n",outname);
	    fp=Fopen(outname,"w");
	    first=FALSE;
	    fprintf(fp,"CSC VIOLATIONS:\n");
	    if (signals) 
	      print_state_vector(fp,signals,nsigs,ninpsig);
	    fprintf(fp,"STATE1 : STATE2\n");
	  }
	  fprintf(fp,"%d:%s <-> %d:%s\n",curstate1->number,curstate1->state,
		  curstate2->number,curstate2->state);
	  curstate1->highlight=i+1; 
	  curstate2->highlight=i+1;
	} /*else
	  printf("OK: %d:%s <-> %d:%s\n",curstate1->number,curstate1->state,
	  curstate2->number,curstate2->state);*/
	curstate2=curstate2->link;
      }
    }
  if (first) {
    printf("complete state coded\n");
    fprintf(lg,"complete state coded\n");
  }
  else fclose(fp);
  return(first);
}

/*****************************************************************************/
/* Check if two states have an output disabling.                             */
/*****************************************************************************/

bool disabling(char * state1,char * state2,int ninpsig,int nsignals)
{
  int i;

  for (i=ninpsig;i<nsignals;i++)
    if ((state1[i]=='U' && state2[i]=='0') ||
	(state1[i]=='R' && state2[i]=='0') ||
	(state1[i]=='D' && state2[i]=='1') ||
	(state1[i]=='F' && state2[i]=='1'))
      return TRUE;
  return FALSE;
}      

/*****************************************************************************/
/* Check that state graph has no output disablings.                          */
/*****************************************************************************/

bool check_disablings(char * filename,signalADT *signals,stateADT *state_table,
		      int nsigs,int ninpsig,bool verbose)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  int i;
  stateADT curstate1;
  statelistADT curstate2;
  bool first;

  printf("Checking output semi-modularity ... ");
  fprintf(lg,"Checking output semi-modularity ... ");
  fflush(stdout);
  first=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate1=state_table[i];
	 curstate1 != NULL && curstate1->state != NULL;
	 curstate1=curstate1->link) 
      for (curstate2=curstate1->succ;curstate2;
	   curstate2=curstate2->next) {
	if (disabling(curstate1->state,curstate2->stateptr->state,ninpsig,
		      nsigs)) {
	  if (first) {
	    printf("NOT output semi-modular!\n");
	    fprintf(lg,"NOT output semi-modular!\n");
	    if (!verbose) {
	      printf("ERROR:  NOT output semi-modular!\n");
	      fprintf(lg,"ERROR:  NOT output semi-modular!\n");
	      return(FALSE);
	    }
	    strcpy(outname,filename);
	    strcat(outname,".err");
	    printf("EXCEPTION:  Storing output semi-modular violations to:  %s\n",outname);
	    fprintf(lg,"EXCEPTION:  Storing output semi-modular violations to:  %s\n",outname);
	    fp=Fopen(outname,"w");
	    first=FALSE;
	    fprintf(fp,"OUTPUT SEMI-MODULAR VIOLATIONS:\n");
	    if (signals) 
	      print_state_vector(fp,signals,nsigs,ninpsig);
	    fprintf(fp,"STATE1 : STATE2\n");
	  }
	  fprintf(fp,"%d:%s <-> %d:%s\n",curstate1->number,curstate1->state,
		  curstate2->stateptr->number,curstate2->stateptr->state);
	  curstate1->highlight=i+1; 
	  curstate2->stateptr->highlight=i+1;
	} /*else
	  printf("OK: %d:%s <-> %d:%s\n",curstate1->number,curstate1->state,
	  curstate2->number,curstate2->state);*/
      }
  if (first) {
    printf("output semi-modular\n");
    fprintf(lg,"output semi-modular\n");
  }
  else fclose(fp);
  return(first);
}

/*****************************************************************************/
/* Print an error trace using parseGraph by Tom Rokicki.                     */
/*****************************************************************************/

void print_trace(char * filename,signalADT *signals,eventADT *events,
		 stateADT *state_table,int nsignals,bool display,bool genrg,
		 int nevents,ruleADT **rules,bool timed,bool noparg,
		 bool causal,int nplaces)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int i,edge;
  stateADT curstate;
  statelistADT predstate;
  bool empty;

  strcpy(outname,filename);
  strcat(outname,".prg");
  //printf("Storing error trace state graph to:  %s\n",outname);
  //fprintf(lg,"Storing error trace state graph to:  %s\n",outname);
  fp=Fopen(outname,"w");
  empty=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) 
      if (curstate->color>=0) { 
	empty=FALSE;
	fprintf(fp,"N F S%d %d:%s\n",curstate->number,curstate->number,
		curstate->state);
      }
  if (empty) {
    printf("ERROR:  No states in error trace!\n");
    fprintf(lg,"ERROR:  No states in error trace!\n");
    return;
  }
  edge=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
      if (curstate->color>=0)
        for (predstate=curstate->succ;predstate; predstate=predstate->next) 
	  if ((predstate->stateptr->color>=0) &&
	      (predstate->stateptr->number>curstate->number)) {
	    if ((genrg) &&
		(predstate->enabling >= 0) &&
		(predstate->enabled >= 0) &&
		(predstate->enabling < nevents) &&
		(predstate->enabled < nevents)) {
	      if (rules[predstate->enabling][predstate->enabled]->expr)
		if (timed)
		  fprintf(fp,"N E T%d <%s,%s,%s,%d>\n",edge,
			  events[predstate->enabling]->event,
			  events[predstate->enabled]->event,
			  rules[predstate->enabling][predstate->enabled]->expr,
			  predstate->stateptr->color);
		else
		  fprintf(fp,"N E T%d <%s,%s,%s>\n",edge,
			  events[predstate->enabling]->event,
			  events[predstate->enabled]->event,
			  rules[predstate->enabling][predstate->enabled]->expr);
	      else 
		if (timed)
		  fprintf(fp,"N E T%d <%s,%s,%d>\n",edge,
			  events[predstate->enabling]->event,
			  events[predstate->enabled]->event,
			  predstate->stateptr->color);
		else
		  fprintf(fp,"N E T%d <%s,%s>\n",edge,
			  events[predstate->enabling]->event,
			  events[predstate->enabled]->event);
	    } else {
	      if (predstate->enabled==(nevents+nplaces)) 
		fprintf(fp,"N E T%d DEAD\n",edge);
	      else if (timed) {
		if (predstate->iteration == 'I') 
		  if (predstate->stateptr->state[predstate->enabled]=='1'){
		    fprintf(fp,"N E T%d {%s}\n",edge,
			    signals[predstate->enabled]->name);
		  } else {
		    fprintf(fp,"N E T%d {~(%s)}\n",edge,
			    signals[predstate->enabled]->name);
		  }
		else if (predstate->iteration == 'R') {
		  fprintf(fp,"N E T%d %s\n",edge,events[predstate->enabled]->event);
		}
		else if (predstate->iteration != 'f') {
		  fprintf(fp,"N E T%d <%s>%c\n",edge,
			  events[predstate->enabled]->event,
			  predstate->iteration);
		} else {
		  if (causal)
		    fprintf(fp,"N E T%d %s:%d:%d\n",edge,
			    events[predstate->enabled]->event,
			    predstate->stateptr->color,
			    events[predstate->enabled]->color);
		  else 
		    fprintf(fp,"N E T%d %s:%d\n",edge,
			    events[predstate->enabled]->event,
			    predstate->stateptr->color);
		} 
	      } else
		fprintf(fp,"N E T%d %s\n",edge,
			events[predstate->enabled]->event);
	    }
	    if (!causal || events[predstate->enabled]->color >= 
		predstate->stateptr->color) {
	      fprintf(fp,"E P S%d 0 T%d 0\n",curstate->number,edge);
	      fprintf(fp,"E P T%d 0 S%d 0\n",edge,
		      predstate->stateptr->number);
	      edge++;
	    } else {
	      fprintf(fp,"E P S%d 0 S%d 0\n",curstate->number,
		      predstate->stateptr->number);
	    }
	  }
  fprintf(fp,"ZZZZZ");
  fclose(fp);
  if (!noparg) {
    if (display) {
      sprintf(shellcommand,"parg %s.prg &",filename);
      //printf("Executing command:  %s\n",shellcommand);
      //fprintf(lg,"Executing command:  %s\n",shellcommand);
    } else {
      sprintf(shellcommand,"pG %s",filename);
      //printf("Executing command:  %s\n",shellcommand);
      //fprintf(lg,"Executing command:  %s\n",shellcommand);
    }
    system(shellcommand);
  }
}

/*****************************************************************************/
/* Print an error trace using Dinotrace.                                     */
/*****************************************************************************/

void print_dinotrace(char * filename,signalADT *signals,eventADT *events,
		     stateADT *state_table,int nsignals,bool display,
		     bool genrg,int nevents,ruleADT **rules,bool timed,
		     bool noparg,bool print_dummy,int ndummy)
{
  char outname[FILENAMESIZE];
  char shellcommand[100];
  FILE *fp;
  int i,j,k;
  stateADT curstate;
  statelistADT predstate;
  bool empty,done,up;
  int time;

  strcpy(outname,filename);
  strcat(outname,".dino");
  printf("Storing error trace state graph to:  %s\n",outname);
  fprintf(lg,"Storing error trace state graph to:  %s\n",outname);
  fp=fopen(outname,"w");
  empty=TRUE;
  time = 0;
  done=FALSE;
  for (j=0;!done;j++) {
    fprintf(fp,"!     ");
    done=TRUE;
    for (i=0;i<nsignals;i++)
      if ((unsigned)j<strlen(signals[i]->name)) {
	done=FALSE;
	fprintf(fp,"%c",signals[i]->name[j]);
      } else
	fprintf(fp," ");
    if (print_dummy) {
      for (i=nevents-ndummy;i<nevents;i++)
	if ((unsigned)j<strlen(events[i]->event)) {
	done=FALSE;
	fprintf(fp,"%c",events[i]->event[j]);
      } else
	fprintf(fp," ");
    }
    fprintf(fp,"\n");
  }
  do {

    done=TRUE;
    time++;

    for (i=0;i<PRIME;i++) {
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {

	if (curstate->color==time) { 
	  empty=FALSE;
	  if (time < 10)
	    fprintf(fp,"%d     ",time);
	  else if (time < 100)
	    fprintf(fp,"%d    ",time);
	  else if (time < 1000)
	    fprintf(fp,"%d   ",time);
	  else if (time < 10000)
	    fprintf(fp,"%d  ",time);
	  else
	    fprintf(fp,"%d ",time);
	  if (strcmp(curstate->state,"FAIL")==0) {
	    if (print_dummy) k=nevents;
	    else k=nsignals;
	    for (predstate=curstate->pred;predstate;
		 predstate=predstate->next) 
	      if (predstate->stateptr->color==(time-1))
		if ((predstate->enabled >= 0) && 
		    (predstate->enabled < nevents) &&
		    ((events[predstate->enabled]->signal>=0)||(print_dummy))) {
		  if (predstate->enabled < nevents-ndummy) {
		    k=events[predstate->enabled]->signal;
		    if ((predstate->enabled % 2)==1) up=TRUE;
		    else up=FALSE;
		  } else 
		    k=predstate->enabled;
		}
	    for (j=0;j<k && j<nsignals;j++)
	      fprintf(fp,"X");
	    if (k < nsignals) {
	      if (up) 
		fprintf(fp,"1");
	      else 
		fprintf(fp,"0");
	      for (j=k+1;j<nsignals;j++)
		fprintf(fp,"X");
	    }
	    if (print_dummy) {
	      for (j=nevents-ndummy;j<k;j++)
		fprintf(fp,"X");
	      if (k<nevents && k>=nevents-ndummy) {
		fprintf(fp,"1");
		for (j=k+1;j<nevents;j++)
		  fprintf(fp,"X");
	      } else {
		for (j=nevents-ndummy;j<nevents;j++)
		  fprintf(fp,"X");
	      }
	    }
	  } else {
	    for (j=0;j<nsignals;j++) {
	      if ((curstate->state[j]=='R')||(curstate->state[j]=='0'))
		fprintf(fp,"0");
	      else
		fprintf(fp,"1");
	    }
	    if (print_dummy) {
	      for (predstate=curstate->pred;predstate;
		   predstate=predstate->next) 
		if (predstate->stateptr->color==(time-1))
		  if ((predstate->enabled >= 0) && 
		      (predstate->enabled < nevents)) 
		    k=predstate->enabled;
	      for (j=nevents-ndummy;j<nevents;j++)
		if (j==k)
		  fprintf(fp,"1");
		else 
		  fprintf(fp,"0");
	    }
	  }
	  fprintf(fp,"\n");
	  done=FALSE;
	}
	if ( !done ) break;
      }
      if ( !done ) break;
    }
  } while (!done);
  if (time < 10)
    fprintf(fp,"%d     ",time);
  else if (time < 100)
    fprintf(fp,"%d    ",time);
  else if (time < 1000)
    fprintf(fp,"%d   ",time);
  else if (time < 10000)
    fprintf(fp,"%d  ",time);
  else
    fprintf(fp,"%d ",time);
  for (j=0;j<nsignals;j++)
    fprintf(fp,"X");
  if (print_dummy)
    for (j=nevents-ndummy;j<nevents;j++)
      fprintf(fp,"X");
  fprintf(fp,"\n");
  if (empty) {
    printf("ERROR:  No states in error trace!\n");
    fprintf(lg,"ERROR:  No states in error trace!\n");
    return;
  }
  fclose(fp);
  if (!noparg) {
    sprintf(shellcommand,"dinotrace -decsim %s.dino &",filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
    system(shellcommand);
  }
}

/*****************************************************************************/
/* Print an error trace.                                                     */
/*****************************************************************************/

void print_error_trace(char * filename,signalADT *signals,eventADT *events,
		       stateADT *state_table,int nsignals,bool display,
		       bool genrg,int nevents,int nplaces,ruleADT **rules,bool timed,
		       bool noparg,bool print_dummy,bool causal)
{
  int i;
  stateADT curstate;
  statelistADT predstate;
  bool done;
  int time;
  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,filename);
  strcat(outname,".trace");
  fp=fopen(outname,"w");
  if (causal) {
    printf("Storing causal error trace state graph to:  %s\n",outname);
    fprintf(lg,"Storing causal error trace state graph to:  %s\n",outname);
  } else {
    printf("Storing full error trace state graph to:  %s\n",outname);
    fprintf(lg,"Storing full error trace state graph to:  %s\n",outname);
  }
  time = 0;
  do {
    done=true;
    time++;
    for (i=0;i<PRIME;i++) {
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	if (curstate->color==time) { 
	  for (predstate=curstate->pred;predstate;
	       predstate=predstate->next) 
	    if (predstate->stateptr->color==(time-1))
	      if ((predstate->enabled >= 0) && 
		  (predstate->enabled < nevents+nplaces) &&
		  ((events[predstate->enabled]->signal>=0)||(print_dummy))) {
		if (!causal || events[predstate->enabled]->color==2) {
		  if (predstate->iteration == 'I')  {
		    if (predstate->stateptr->state[predstate->enabled]=='0'){
		      printf("{%s} -> ",signals[predstate->enabled]->name);
		      fprintf(lg,"{%s} -> ",signals[predstate->enabled]->name);
		      fprintf(fp,"{%s} -> ",signals[predstate->enabled]->name);
		    } else {
		      printf("{~(%s)} -> ",signals[predstate->enabled]->name);
		      fprintf(lg,"{~(%s)} -> ",signals[predstate->enabled]->name);
		      fprintf(fp,"{~(%s)} -> ",signals[predstate->enabled]->name);
		    }
		  } else if (predstate->iteration == 'f' || predstate->iteration == 'R') {
		    printf("%s -> ",events[predstate->enabled]->event);
		    fprintf(lg,"%s -> ",events[predstate->enabled]->event);
		    fprintf(fp,"%s -> ",events[predstate->enabled]->event);
		  } else if (predstate->iteration != 'f') {
		    printf("<%s>%c -> ",events[predstate->enabled]->event,predstate->iteration);
		    fprintf(lg,"<%s>%c -> ",events[predstate->enabled]->event,predstate->iteration);
		    fprintf(fp,"<%s>%c -> ",events[predstate->enabled]->event,predstate->iteration);
		  }
		}
	      } 
	  done=false;
	}
	if ( !done ) break;
      }
      if ( !done ) break;
    }
  } while (!done);
  printf("FAIL!\n");
  fprintf(lg,"FAIL!\n");
  fprintf(fp,"FAIL!\n");
  fclose(fp);
}

/*****************************************************************************
 * Print an error trace for LHPNs containing a useful trace that
 * relates back to the LHPN.
 *****************************************************************************/
void print_ams_error_trace(char * filename,signalADT *signals,eventADT *events,
			   stateADT *state_table,int nsignals,int nineqs,bool display,
			   bool genrg,int nevents,int nplaces,ruleADT **rules,bool timed,
			   bool noparg,bool print_dummy)
{
	//TODO: Add a verbosity level and add more information like rates
	//and value ranges.
  char outname[FILENAMESIZE];
  strcpy(outname,filename);
  strcat(outname,".ams");
  FILE *fp=fopen(outname,"w");

  char zone_outname[FILENAMESIZE];
  strcpy(zone_outname,filename);
  strcat(zone_outname,".zone");
  FILE *zfp=fopen(zone_outname,"w");
  
  printf("Storing full AMS error trace to:  %s\n",outname);
  fprintf(lg,"Storing full AMS error trace to:  %s\n",outname);
  printf("Storing full AMS error trace with zones to:  %s\n",zone_outname);
  fprintf(lg,"Storing full AMS error trace with zones to:  %s\n",zone_outname);

  stateADT curState = NULL;
  char* prevStateVec = NULL;
  statelistADT predstate;
  bool done;	
  int time = 0;
  do {
    done=true;
    time++;
    for(int i=0;i<PRIME;i++) {
      for(curState=state_table[i];curState != NULL && curState->state != NULL;
	  curState=curState->link) {
	if(curState->color==time) { 
	  for(predstate=curState->pred;predstate;
	      predstate=predstate->next) 
	    if(predstate->stateptr->color==(time-1))
	      if((predstate->enabled >= 0) && 
		 (predstate->enabled < nevents+nplaces) &&
		 ((events[predstate->enabled]->signal>=0)||(print_dummy))) {
		if(predstate->iteration == 'I') {
		  if(predstate->enabling == -2) {
		    printf("~(%s)",signals[predstate->enabled]->name);
		    fprintf(lg,"~(%s)",signals[predstate->enabled]->name);
		    fprintf(fp,"~(%s)",signals[predstate->enabled]->name);
                    fprintf(zfp,"~(%s)\n",signals[predstate->enabled]->name);
		  }
		  else {
		    printf("%s",signals[predstate->enabled]->name);
		    fprintf(lg,"%s",signals[predstate->enabled]->name);
		    fprintf(fp,"%s",signals[predstate->enabled]->name);
                    fprintf(zfp,"%s\n",signals[predstate->enabled]->name);
		  }
		}
		else {
		  printf("%s",events[predstate->enabled]->event);
		  fprintf(lg,"%s",events[predstate->enabled]->event);
		  fprintf(fp,"%s",events[predstate->enabled]->event);
                  fprintf(zfp,"%s\n",events[predstate->enabled]->event);
		}
		if(prevStateVec != NULL) {
		  for(int i=nsignals-nineqs;i<nsignals;i++) {
		    if(curState->state[i] != prevStateVec[i]){
		      if(!(predstate->iteration == 'I' &&
			   predstate->enabled == i)) {
			if(prevStateVec[i] == '1') {
			  printf("[~(%s)]",signals[i]->name);
			  fprintf(lg,"[~(%s)]",signals[i]->name);
			  fprintf(fp,"[~(%s)]",signals[i]->name);
                          fprintf(zfp,"[~(%s)]\n",signals[i]->name);
			}
			else {
			  printf("[%s]",signals[i]->name);
			  fprintf(lg,"[%s]",signals[i]->name);
			  fprintf(fp,"[%s]",signals[i]->name);
                          fprintf(zfp,"[%s]\n",signals[i]->name);
			}
		      }
		    }
		  }
		}
                //Output the appropriate zone
                fprintClocklistADT(zfp,curState->clocklist,events);
                
		printf(" -> ");
		fprintf(lg," -> ");
		fprintf(fp," -> ");
                fprintf(zfp,"\n");
	      }
	  prevStateVec = curState->state;
	  done=false;
	}
	if(!done) {
	  break;
	}
      }
      if(!done) {
	break;
      }
    }
  } while (!done);
  printf(" -> FAIL!\n");
  fprintf(lg," -> FAIL!\n");
  fprintf(fp," -> FAIL!\n");
  fprintf(zfp," -> FAIL!\n");
  fclose(fp);
  fclose(zfp);
}

/*****************************************************************************
 * Update the gobal time based on the time calculated for each variable.
 *****************************************************************************/
int updateGlobalTime(int globalTime,int elapsed[],int numContVars)
{
  int min = -1;
  int max = -1;
#ifdef __TRACE_DEBUG__
  printf("Elapsed[");
#endif
  for(int i=0;i<numContVars;i++) {
#ifdef __TRACE_DEBUG__
    printf("%d,",elapsed[i]);
#endif
  }
#ifdef __TRACE_DEBUG__
  printf("]\n");
#endif
  for(int i=0;i<numContVars;i++) {
    if(elapsed[i] > 0) {
      if(min < 0) {
	min = elapsed[i];
	max = elapsed[i];
      }
    }
    else {
      if(elapsed[i] < min) {
	min = elapsed[i];
      }
      else if(elapsed[i] > max) {
	max = elapsed[i];
      }
    }
  }
#ifdef __TRACE_DEBUG__
  printf("Elapsed-max:%d min:%d globaltime:%d\n",max,min,globalTime);
#endif
  if(max - min > 1) {
    printf("WARNING: Elapsed time calculated from a zone differed by > 1 time unit between two variables.\n");
  }
  return globalTime + max;
}

/*****************************************************************************
 * Calculate the maximum value in the VCD file.
 *****************************************************************************/
void calcMaxVCD(int &globalTime,int &prevGlobalTime,int curContVal[],bool valChange[],int contVarInd[],int numContVars,clocklistADT curCL,clocklistADT prevCL,eventADT *events,int nevents,int nplaces,FILE *fp)
{
  int elapsed[numContVars];
  for(int j=0;j<numContVars;j++) {
    elapsed[j] = -1;
    curContVal[j] = 0;
    valChange[j] = false;
  }
  if(prevCL == NULL) {
    /* Case for the initial zone */
#ifdef __TRACE_DEBUG__
    printf("Max:Initial case.\n");
#endif
    int cntVar = 0;
    for(int j=0;j<curCL->clocknum;j++) {
      if(curCL->clockkey[j].enabling == -2) {
	int prevVal = events[curCL->clockkey[j].enabled]->upper;
	int curVal = 0;
	int rate = curCL->clockkey[j].urate;
	if(rate >= 0) {
	  curVal = curCL->clocks[j][0];
	}
	else if(rate < 0) {
	  curVal = -1 * curCL->clocks[0][j];
	}
#ifdef __TRACE_DEBUG__
	printf("Max:%s: prevVal:%d, curVal:%d, rate:%d\n",events[curCL->clockkey[j].enabled]->event, prevVal, curVal, rate);
#endif
	/* If the rate is equal to zero or the two
	   values are equal then a time elapsed value
	   can't be calculated based on that variable.
	   Even if the rate has changed between zones
	   the value would have to change for time to
	   elapse. */
	if(rate == 0 ||
	   prevVal == curVal) {
	  elapsed[contVarInd[curCL->clockkey[j].enabled-nevents]] = 0;
	  if(prevVal != curVal) {
#ifdef __TRACE_DEBUG__
	    printf("Max:Val change1 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
	    valChange[contVarInd[curCL->clockkey[j].enabled-nevents]] = true;
	    curContVal[contVarInd[curCL->clockkey[j].enabled-nevents]] = curVal;
	  }
	}
	else {
	  elapsed[contVarInd[curCL->clockkey[j].enabled-nevents]] = int(ceil(fabs(double((curVal*rate)-prevVal)/double(rate))));
#ifdef __TRACE_DEBUG__
	  printf("Max:Val change2 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
	  valChange[contVarInd[curCL->clockkey[j].enabled-nevents]] = true;
	  curContVal[contVarInd[curCL->clockkey[j].enabled-nevents]] = curVal * rate;
	}
	cntVar++;
      }
    }
    globalTime = updateGlobalTime(globalTime,elapsed,numContVars);
  }
  else {
    /* Search through the previous and current zones
       looking for a common continous variable whose rate
       is not 0 in the current zone. This continous
       variable will be used to calculate the global
       time. */
#ifdef __TRACE_DEBUG__
    printf("Max:Normal case.\n");
#endif
    int cntVar = 0;
    for(int j=0;j<prevCL->clocknum;j++) {
      if(prevCL->clockkey[j].enabling == -2) {
	for(int k=0;k<curCL->clocknum;k++) {
	  if(curCL->clockkey[k].enabling == -2 &&
	     prevCL->clockkey[j].enabled ==
	     curCL->clockkey[k].enabled) {
	    int prevInd = j;
	    int curInd = k;
	    int prevVal = 0;
	    int prevRate = prevCL->clockkey[prevInd].urate;
	    if(prevRate >= 0) {
	      prevVal = prevCL->clocks[prevInd][0];
	    }
	    else if(prevRate < 0) {
	      prevVal = -1 * prevCL->clocks[0][prevInd];
	    }
	    int curVal = 0;
	    int curRate = curCL->clockkey[curInd].urate;
	    if(curRate >= 0) {
	      curVal = curCL->clocks[curInd][0];
	    }
	    else if(curRate < 0) {
	      curVal = -1 * curCL->clocks[0][curInd];
	    }
#ifdef __TRACE_DEBUG__
	    printf("Max:%s: prevVal:%d, curVal:%d, prevRate:%d, curRate:%d\n",events[curCL->clockkey[curInd].enabled]->event,prevVal,curVal,prevRate,curRate);
#endif
	    if(curRate == 0 ||
	       prevVal == curVal) {
	      elapsed[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = 0;
	      if(prevVal != curVal) {
#ifdef __TRACE_DEBUG__
		printf("Max:Val change3 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
		valChange[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = true;
		curContVal[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = curVal;
	      }
	    }
	    else {
	      if(prevRate == 0) {
		prevRate = 1;
	      }
#ifdef __TRACE_DEBUG__
	      printf("Max:Val change4 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
	      elapsed[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = int(ceil(fabs(double((curVal*curRate)-(prevVal*prevRate))/double(curRate))));
	      valChange[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = true;
	      curContVal[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = curVal * curRate;
	    }
	    break;
	  }
	}
      }
      cntVar++;
    }
    globalTime = updateGlobalTime(globalTime,elapsed,numContVars);
  }
  bool timeOut = false;
  for(int j=0;j<nplaces;j++) {
    if(contVarInd[j] != -1) {
      if(valChange[contVarInd[j]]){
	if(!timeOut) {
	  if(globalTime > prevGlobalTime) {
	    fprintf(fp,"#%d\n",globalTime);
#ifdef __TRACE_DEBUG__
	    printf("Max:#%d\n",globalTime);
#endif
	    prevGlobalTime = globalTime;
	  }
	  timeOut = true;
	}
	fprintf(fp,"r%.16g *%d\n",double(curContVal[contVarInd[j]]),j+nevents);
#ifdef __TRACE_DEBUG__
	printf("Max:r%.16g *%d[%s]\n",double(curContVal[contVarInd[j]]),j+nevents,events[j+nevents]->event);
#endif
      }
    }
  }
}

/*****************************************************************************
 * Calculate the maximum value in the VCD file.
 *****************************************************************************/
void calcMinVCD(int &globalTime,int &prevGlobalTime,int curContVal[],bool valChange[],int contVarInd[],int numContVars,clocklistADT curCL,clocklistADT prevCL,eventADT *events,int nevents,int nplaces,FILE *fp)
{
  int elapsed[numContVars];
  for(int j=0;j<numContVars;j++) {
    elapsed[j] = -1;
    curContVal[j] = 0;
    valChange[j] = false;
  }
  if(prevCL == NULL) {
    /* Case for the initial zone */
#ifdef __TRACE_DEBUG__
    printf("Min:Initial case.\n");
#endif
    int cntVar = 0;
    for(int j=0;j<curCL->clocknum;j++) {
      if(curCL->clockkey[j].enabling == -2) {
	int prevVal = events[curCL->clockkey[j].enabled]->lower;
	int curVal = 0;
	int rate = curCL->clockkey[j].lrate;
	if(rate >= 0) {
	  curVal = -1 * curCL->clocks[0][j];
	}
	else if(rate < 0) {
	  curVal = curCL->clocks[j][0];
	}
#ifdef __TRACE_DEBUG__
	printf("Min:%s: prevVal:%d, curVal:%d, rate:%d\n",events[curCL->clockkey[j].enabled]->event, prevVal, curVal, rate);
#endif
	/* If the rate is equal to zero or the two
	   values are equal then a time elapsed value
	   can't be calculated based on that variable.
	   Even if the rate has changed between zones
	   the value would have to change for time to
	   elapse. */
	if(rate == 0 ||
	   prevVal == curVal) {
	  elapsed[contVarInd[curCL->clockkey[j].enabled-nevents]] = 0;
	  if(prevVal != curVal) {
#ifdef __TRACE_DEBUG__
	    printf("Min:Val change1 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
	    valChange[contVarInd[curCL->clockkey[j].enabled-nevents]] = true;
	    curContVal[contVarInd[curCL->clockkey[j].enabled-nevents]] = curVal;
	  }
	}
	else {
	  elapsed[contVarInd[curCL->clockkey[j].enabled-nevents]] = int(ceil(fabs(double((curVal*rate)-prevVal)/double(rate))));
#ifdef __TRACE_DEBUG__
	  printf("Min:Val change2 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
	  valChange[contVarInd[curCL->clockkey[j].enabled-nevents]] = true;
	  curContVal[contVarInd[curCL->clockkey[j].enabled-nevents]] = curVal * rate;
	}
	cntVar++;
      }
    }
    globalTime = updateGlobalTime(globalTime,elapsed,numContVars);
  }
  else {
    /* Search through the previous and current zones
       looking for a common continous variable whose rate
       is not 0 in the current zone. This continous
       variable will be used to calculate the global
       time. */
#ifdef __TRACE_DEBUG__
    printf("Min:Normal case.\n");
#endif
    int cntVar = 0;
    for(int j=0;j<prevCL->clocknum;j++) {
      if(prevCL->clockkey[j].enabling == -2) {
	for(int k=0;k<curCL->clocknum;k++) {
	  if(curCL->clockkey[k].enabling == -2 &&
	     prevCL->clockkey[j].enabled ==
	     curCL->clockkey[k].enabled) {
	    int prevInd = j;
	    int curInd = k;
	    int prevVal = 0;
	    int prevRate = prevCL->clockkey[prevInd].lrate;
	    if(prevRate >= 0) {
	      prevVal = -1 * prevCL->clocks[0][prevInd];
	    }
	    else if(prevRate < 0) {
	      prevVal = prevCL->clocks[prevInd][0];
	    }
	    int curVal = 0;
	    int curRate = curCL->clockkey[curInd].lrate;
	    if(curRate >= 0) {
	      curVal = -1 * curCL->clocks[0][curInd];
	    }
	    else if(curRate < 0) {
	      curVal = curCL->clocks[curInd][0];
	    }
#ifdef __TRACE_DEBUG__
	    printf("Min:%s: prevVal:%d, curVal:%d, prevRate:%d, curRate:%d\n",events[curCL->clockkey[curInd].enabled]->event,prevVal,curVal,prevRate,curRate);
#endif
	    if(curRate == 0 ||
	       prevVal == curVal) {
	      elapsed[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = 0;
	      if(prevVal != curVal) {
#ifdef __TRACE_DEBUG__
		printf("Min:Val change3 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
		valChange[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = true;
		curContVal[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = curVal;
	      }
	    }
	    else {
	      if(prevRate == 0) {
		prevRate = 1;
	      }
#ifdef __TRACE_DEBUG__
	      printf("Min:Val change4 on %s\n",events[curCL->clockkey[j].enabled]->event);
#endif
	      elapsed[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = int(ceil(fabs(double((curVal*curRate)-(prevVal*prevRate))/double(curRate))));
	      valChange[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = true;
	      curContVal[contVarInd[curCL->clockkey[curInd].enabled-nevents]] = curVal * curRate;
	    }
	    break;
	  }
	}
      }
      cntVar++;
    }
    globalTime = updateGlobalTime(globalTime,elapsed,numContVars);
  }
  bool timeOut = false;
  for(int j=0;j<nplaces;j++) {
    if(contVarInd[j] != -1) {
      if(valChange[contVarInd[j]]){
	if(!timeOut) {
	  if(globalTime > prevGlobalTime) {
	    fprintf(fp,"#%d\n",globalTime);
#ifdef __TRACE_DEBUG__
	    printf("Min:#%d\n",globalTime);
#endif
	    prevGlobalTime = globalTime;
	  }
	  timeOut = true;
	}
	fprintf(fp,"r%.16g *%d\n",double(curContVal[contVarInd[j]]),j+nevents);
#ifdef __TRACE_DEBUG__
	printf("Min:r%.16g *%d[%s]\n",double(curContVal[contVarInd[j]]),j+nevents,events[j+nevents]->event);
#endif
      }
    }
  }
}

/*****************************************************************************
 * Print an error trace for LHPNs in VCD (value change dump) format
 * that can be suitably viewed in a standard waveform viewer.
 *****************************************************************************/
void print_vcd_error_trace(char * filename,signalADT *signals,eventADT *events,
		       stateADT *state_table,int nsignals,int nineqs,bool display,
		       bool genrg,int nevents,int nplaces,ruleADT **rules,bool timed,
		       bool noparg,bool print_dummy)
{
  //TODO: break the rate calculation stuff out into another function
  char outnameMax[FILENAMESIZE];
  strcpy(outnameMax,filename);
  strcat(outnameMax,"_max.vcd");
  FILE *fpMax=fopen(outnameMax,"w");

  char outnameMin[FILENAMESIZE];
  strcpy(outnameMin,filename);
  strcat(outnameMin,"_min.vcd");
  FILE *fpMin=fopen(outnameMin,"w");

  printf("Storing full VCD error trace to: %s\n",outnameMax);
  printf("Storing full VCD error trace to: %s\n",outnameMin);

  //VCD header
  time_t rawtime;
  time(&rawtime);
  fprintf(fpMax,"$date %s$end\n",ctime(&rawtime));
  fprintf(fpMin,"$date %s$end\n",ctime(&rawtime));
  fprintf(fpMax,"$version ATACS VERSION 6.0 $end\n");
  fprintf(fpMin,"$version ATACS VERSION 6.0 $end\n");
  fprintf(fpMax,"$timescale 1 s $end\n");
  fprintf(fpMin,"$timescale 1 s $end\n");
  fprintf(fpMax,"$scope module top $end\n");
  fprintf(fpMin,"$scope module top $end\n");
  //Variable definitions
  for(int i=nevents;i<nevents+nplaces;i++) {
    //printf("%s:%d\n",events[i]->event,events[i]->type);
    if(events[i]->type&CONT) {
      fprintf(fpMax,"$var real 64 *%d %s $end\n",i,events[i]->event);
      fprintf(fpMin,"$var real 64 *%d %s $end\n",i,events[i]->event);
    }
  }
  fprintf(fpMax,"$upscope $end\n$enddefinitions $end\n\n");
  fprintf(fpMin,"$upscope $end\n$enddefinitions $end\n\n");
  //Output the initial time and initial values
  fprintf(fpMax,"#0\n");
  fprintf(fpMin,"#0\n");
  fprintf(fpMax,"$dumpvars\n");
  fprintf(fpMin,"$dumpvars\n");
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(events[i]->type&CONT) {
      double initVal = double(events[i]->lower+events[i]->upper)/2.0;
      fprintf(fpMax,"r%.16g *%d\n",initVal,i);
      fprintf(fpMin,"r%.16g *%d\n",initVal,i);
    }
  }
  fprintf(fpMax,"$end\n");
  fprintf(fpMin,"$end\n");

  //Calculate values of continuous variables along the trace.
  stateADT curState = NULL;
  char* prevStateVec = NULL;
  clocklistADT prevCL = NULL;
  clocklistADT curCL = NULL;
  statelistADT predstate;
  bool done;
  int traceTime = 0;
  int globalTimeMax = 0;
  int prevGlobalTimeMax = 0;
  int globalTimeMin = 0;
  int prevGlobalTimeMin = 0;
  int numContVars = 0;
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(events[i]->type&CONT) {
      numContVars++;
    }
  }
  int contVarInd[nplaces];
  int cvCnt = 0;
  for(int i=nevents;i<nevents+nplaces;i++) {
    if(events[i]->type&CONT) {
      contVarInd[i-nevents] = cvCnt;
      cvCnt++;
    }
    else {
      contVarInd[i-nevents] = -1;
    }
  }
  int prevContVal[numContVars];
  int curContVal[numContVars];
  bool valChange[numContVars];
  for(int i=0;i<numContVars;i++) {
    prevContVal[i] = 0;
    curContVal[i] = 0;
    valChange[i] = false;
  }
  do {
    done=true;
    traceTime++;
    for(int i=0;i<PRIME;i++) {
      for(curState=state_table[i];
	  curState != NULL && curState->state != NULL;
	  curState=curState->link) {
	if(curState->color==traceTime) { 
	  for(predstate=curState->pred;predstate;
	      predstate=predstate->next) 
	    if(predstate->stateptr->color==(traceTime-1))
	      if((predstate->enabled >= 0) && 
		 (predstate->enabled < nevents) &&
		 ((events[predstate->enabled]->signal>=0)||(print_dummy))) {
		curCL = curState->clocklist;
		calcMaxVCD(globalTimeMax,prevGlobalTimeMax,curContVal,valChange,contVarInd,numContVars,curCL,prevCL,events,nevents,nplaces,fpMax);
		calcMinVCD(globalTimeMin,prevGlobalTimeMin,curContVal,valChange,contVarInd,numContVars,curCL,prevCL,events,nevents,nplaces,fpMin);
#ifdef __TRACE_DEBUG__
		printf("-----------------------\n");
		printf("Previous zone:");
		printClocklistADT(prevCL,events);
		printf("Current zone:");
		printClocklistADT(curCL,events);
		printf("-----------------------\n");
		if(predstate->iteration == 'I') {
		  if(predstate->enabling == -2) {
		    printf("~(%s)",signals[predstate->enabled]->name);
		  }
		  else {
		    printf("%s",signals[predstate->enabled]->name);
		  }
		}
		else {
		  printf("%s",events[predstate->enabled]->event);
		}
		if(prevStateVec != NULL) {
		  for(int j=nsignals-nineqs;j<nsignals;j++) {
		    if(curState->state[j] != prevStateVec[j]){
		      if(!(predstate->iteration == 'I' &&
			   predstate->enabled == j)) {
			if(prevStateVec[i] == '1') {
			  printf("[~(%s)]",signals[j]->name);
			}
			else {
			  printf("[%s]",signals[j]->name);
			}
		      }
		    }
		  }
		}
		printf(" -> \n");
#endif
	      }
	  prevCL = curCL;
	  prevStateVec = curState->state;
	  done=false;
	}
	if(!done) {
	  break;
	}
      }
      if(!done) {
	break;
      }
    }
  } while (!done);
  printf("\n");
  fclose(fpMax);
  fclose(fpMin);
}

/*****************************************************************************/
/* Print an error trace in VHDL.                                             */
/*****************************************************************************/

void vhdl_trace(char * filename,signalADT *signals,eventADT *events,
		stateADT *state_table,int nsignals,bool display,bool genrg,
		int nevents)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,edge;
  stateADT curstate;
  statelistADT predstate;
  bool empty;

  strcpy(outname,"trace");
  strcat(outname,".vhd");
  printf("Storing error trace in VHDL to:  %s\n",outname);
  fprintf(lg,"Storing error trace in VHDL to:  %s\n",outname);
  fp=Fopen(outname,"w");
  fprintf(fp,"library ieee;\n");
  fprintf(fp,"use ieee.std_logic_1164.all;\n");
  fprintf(fp,"entity TRACE is\n");
  fprintf(fp,"end TRACE;\n\n");
  fprintf(fp,"architecture BEHAVIORAL of TRACE is\n");
  for (i=0;i<nsignals;i++)
    fprintf(fp,"\tsignal %s : std_logic;\n",signals[i]->name);
  fprintf(fp,"begin\n");
  fprintf(fp,"process\n");
  fprintf(fp,"begin\n");
  empty=TRUE;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link) 
      if (curstate->color>=0) empty=FALSE;
  if (empty) {
    printf("ERROR:  No states in error trace!\n");
    fprintf(lg,"ERROR:  No states in error trace!\n");
    return;
  }
  edge=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
      if (curstate->color>=0)
        for (predstate=curstate->succ;predstate;
	     predstate=predstate->next) 
	  if ((predstate->stateptr->color>=0) &&
	      (predstate->stateptr->number>curstate->number)) {
	    if ((predstate->enabled >= 0) && (predstate->enabled < nevents) &&
		(events[predstate->enabled]->signal>=0)) {
	      if ((predstate->enabled % 2)==1)
		fprintf(fp,"\t%s <= transport '1' after %d ns;\n",
			signals[events[predstate->enabled]->signal]->name,
			predstate->stateptr->color);
	      else
		fprintf(fp,"\t%s <= transport '0' after %d ns;\n",
			signals[events[predstate->enabled]->signal]->name,
			predstate->stateptr->color);
	    }
	  }
  fprintf(fp,"wait;\n");
  fprintf(fp,"end process;\n");
  fprintf(fp,"end BEHAVIORAL;\n");
  fclose(fp);
}

void time_trace(stateADT *state_table,char * startstate,int nsignals,
		int nevents,ruleADT **rules)
{
  int i,position;
  stateADT e;
  statelistADT temp,head,tail,statelist;
  char * vstate;
  stateADT curstate;
  int *eventtime;

  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      if (curstate->color!=INFIN) curstate->color=(-1);
    }
  eventtime=(int*)GetBlock(nevents*sizeof(int));
  for (i=0;i<nevents;i++)
    eventtime[i]=0;
  vstate=SVAL(startstate,nsignals);
  if (strlen(vstate) < 2) {
    for (position=0;position<PRIME;position++)
      for (e = state_table[position]; e != NULL && e->state != NULL;
	   e = e->link)
	if (e->number==0) break;
  } else {
    position=hashpjw(vstate);
    for (e = state_table[position];e->link != NULL;e = e->link)
      if (e->number==0) break;
  }
  free(vstate);
  e->color=0;
  head=NULL;
  tail=NULL;
  head=(statelistADT)GetBlock(sizeof (*(head)));
  head->stateptr=e;
  head->next=NULL;
  tail=head;
  while (head) {
    for (statelist=head->stateptr->succ;statelist;statelist=statelist->next) 
      if (statelist->stateptr->color==INFIN) {
	for (i=1;i<=head->stateptr->clocklist->clocknum;i++)
	  if ((head->stateptr->clocklist->clockkey[i].enabling==
	       statelist->enabling) &&
	      (head->stateptr->clocklist->clockkey[i].enabled==
	       statelist->enabled)) break;
	if (i <= head->stateptr->clocklist->clocknum)
	  statelist->stateptr->color=
	    eventtime[head->stateptr->clocklist->clockkey[i].causal]+
	    head->stateptr->clocklist->clocks[0][i];
	eventtime[statelist->enabled]=statelist->stateptr->color;
	temp=(statelistADT)GetBlock(sizeof (*(temp)));
	temp->stateptr=statelist->stateptr;
	temp->next=NULL;
	tail->next=temp;
	tail=temp;
	break;
      }
    temp=head->next;
    free(head);
    head=temp;
  }
}

void untime_trace(stateADT *state_table,char * startstate,int nsignals,
		int nevents,ruleADT **rules)
{
  int i;
  stateADT curstate;

  for (i=0;i<PRIME;i++) {
    for (curstate=state_table[i];
				 curstate != NULL && curstate->state != NULL;
				 curstate=curstate->link) {
      if (curstate->color > 0) curstate->color=(-1);
      else curstate->color=(-1)*curstate->color;
    }
	}
}

void find_trace(stateADT *state_table,char * startstate,int nsignals)
{
  int i,position,curcolor;
  stateADT e;
  statelistADT temp,head,tail,statelist;
  stateADT curstate,laststate;
  
  for (position=0;position<PRIME;position++) {
    for (e = state_table[position]; e != NULL && e->state != NULL;
	 e = e->link)
      if (e->number==0) break;
    if ((e->state) && (e->number==0)) break;
  }
  if (!(e->state)) return;
  e->color=1;
  head=NULL;
  tail=NULL;
  head=(statelistADT)GetBlock(sizeof (*(head)));
  head->stateptr=e;
  head->next=NULL;
  tail=head;
  while (head) {
    for (statelist=head->stateptr->succ;statelist;statelist=statelist->next) {
      if (statelist->stateptr->color==0) {
				statelist->stateptr->color=head->stateptr->color+1;
				temp=(statelistADT)GetBlock(sizeof (*(temp)));
				temp->stateptr=statelist->stateptr;
				temp->next=NULL;
				tail->next=temp;
				tail=temp;
      }
		}
    temp=head->next;
    free(head);
    head=temp;
  }
  laststate=NULL;
  for (i=0;i<PRIME;i++) {
    for (curstate=state_table[i];
				 curstate != NULL && curstate->state != NULL;
				 curstate=curstate->link) {
      if (!laststate) laststate=curstate;
      if (curstate->number > laststate->number) laststate=curstate;
    }
  }
  curcolor=laststate->color-1;
  laststate->color=(-1)*laststate->color;
  printf("Error found after exploring %d states\n",laststate->number);
  fprintf(lg,"Error found after exploring %d states\n",laststate->number);
  while (curcolor > 0) {
    for (statelist=laststate->pred;statelist;statelist=statelist->next) {
      if (statelist->stateptr->color==curcolor) {
				curcolor--;
				laststate=statelist->stateptr;
				laststate->color=(-1)*laststate->color;
				break;
      }
		}
  }
}

bool in_expression(eventADT *events,ruleADT **rules,int i,int event,
		   int nevents,int nplaces)
{
  if (events[i]->signal < 0) return false;
  for (int j=0;j<nevents+nplaces;j++) {
    for (level_exp cur_level=rules[j][event]->level;cur_level;
	 cur_level=cur_level->next) {
      if (((i % 2==1) && (cur_level->product[events[i]->signal]=='1')) ||
	  ((i % 2==0) && (cur_level->product[events[i]->signal]=='0')))
	return true;
    }
  }
  return false;
}

void causal_recurse(eventADT *events,ruleADT **rules,int nevents,int nplaces,
		    int event,int newcolor)
{
  if (events[event]->color >= 0) return;
  events[event]->color=(-1)*events[event]->color;
  for (int i=0;i<nevents;i++) 
    if (((rules[i][event]->ruletype > NORULE && 
	 !(rules[i][event]->ruletype & REDUNDANT) &&
	 rules[i][event]->ruletype < ORDERING &&
	 rules[i][event]->epsilon==0) ||
	 (in_expression(events,rules,i,event,nevents,nplaces))) &&
	((-1)*events[i]->color < events[event]->color))
      causal_recurse(events,rules,nevents,nplaces,i,events[event]->color);
  for (int i=nevents;i<nevents+nplaces;i++)
    if (rules[i][event]->ruletype > NORULE && 
	!(rules[i][event]->ruletype & REDUNDANT) &&
	rules[i][event]->ruletype < ORDERING) {
      events[i]->color=1;
      for (int j=0;j<nevents;j++)
	if (((rules[j][i]->ruletype > NORULE && 
	      !(rules[j][i]->ruletype & REDUNDANT) &&
	      rules[j][i]->ruletype < ORDERING &&
	      rules[j][i]->epsilon==0) ||
	     (in_expression(events,rules,j,event,nevents,nplaces))) &&
	    ((-1)*events[j]->color < events[event]->color))
	  causal_recurse(events,rules,nevents,nplaces,j,events[event]->color);
    }
  events[event]->color=newcolor;
}

void find_causal_sequence(eventADT *events,ruleADT **rules,
			  stateADT *state_table,int nevents,int nplaces)
{
  int i,j;
  statelistADT statelist;
  stateADT curstate;

  for (i=0;i<nevents;i++)
    events[i]->color=0;
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
         curstate != NULL && curstate->state != NULL;
         curstate=curstate->link)
      if (curstate->color>=0) {
	for (statelist=curstate->pred;statelist;statelist=statelist->next)
	  if (statelist->stateptr->color>=0) 
	    if (statelist->enabled>=0) {
	      if (events[statelist->enabled]->color==0 ||
		  curstate->color < (-1)*events[statelist->enabled]->color) 
		events[statelist->enabled]->color=(-1)*curstate->color;
	      break;
	    }
      }
  for (i=0;i<nevents;i++) {
    for (j=0;j<nevents+nplaces;j++)
      if (rules[i][j]->ruletype & ORDERING) break;
    if (j < nevents+nplaces)
      causal_recurse(events,rules,nevents,nplaces,i,(-1)*events[i]->color);
  }
}

char * find_initial_fire_list(eventADT *events,ruleADT **rules,
			      markkeyADT *markkey,char * marking,int nrules,
			      markingADT marking_struct,int nsigs,int nevents) 
{
  int i;
  char * firelist;
  bool immediate;

  firelist=(char *)GetBlock((nevents+1)*sizeof(char));
  firelist[0]='F';
  for (i=1;i<nevents;i++)
    if (event_enabled(events,rules,markkey,marking,i,nrules,FALSE,
		      marking_struct,nsigs,FALSE,FALSE,TRUE))
      firelist[i]='T';
    else
      firelist[i]='F';
  firelist[nevents]='\0';
  immediate=FALSE;
  for (i=0;i<nevents;i++) 
    if ((firelist[i]=='T') && (events[i]->immediate)) {
      immediate=TRUE;
      break;
    }
  if (immediate) 
    for (i=0;i<nevents;i++) 
      if ((firelist[i]=='T') && (!(events[i]->immediate))) {
	firelist[i]='F';
      }
  return firelist;
}

char * find_fire_list(eventADT *events,ruleADT **rules,markkeyADT *markkey,
		      char * marking,int nrules,markingADT marking_struct,
		      int nsigs,int nevents,char * oldfirelist) 
{
  int i;
  char * firelist;
  bool immediate;

  firelist=(char *)GetBlock((nevents+1)*sizeof(char));
  firelist[0]='F';
  for (i=1;i<nevents;i++)
    if (oldfirelist[i]=='T')
      if (event_enabled(events,rules,markkey,marking,i,nrules,FALSE,
			marking_struct,nsigs,TRUE,FALSE,TRUE))
	firelist[i]='T';
      else
	firelist[i]='F';
    else
      if (event_enabled(events,rules,markkey,marking,i,nrules,FALSE,
			marking_struct,nsigs,FALSE,FALSE,TRUE))
	firelist[i]='T';
      else
	firelist[i]='F';
  firelist[nevents]='\0';
  immediate=FALSE;
  for (i=0;i<nevents;i++) 
    if ((firelist[i]=='T') && (events[i]->immediate)) {
      immediate=TRUE;
      break;
    }
  if (immediate) 
    for (i=0;i<nevents;i++) 
      if ((firelist[i]=='T') && (!(events[i]->immediate))) {
	firelist[i]='F';
      }
  return firelist;
}
  
/*****************************************************************************/
/* Find reduced state graph using constrained token flow.                    */
/*****************************************************************************/

bool find_rsg(char * filename,signalADT *signals,eventADT *events,
	      ruleADT **rules,cycleADT ****cycles,stateADT *state_table,
	      markkeyADT *markkey,int ninpsig,int nsigs,
	      int nevents,int nrules,int nsignals,int ndummy,
	      int ncycles,char * startstate,bool si,
	      bool verbose,bool closure,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,bool untimed,
	      timeoptsADT timeopts,bool noparg)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  markingADT marking;
  markingADT new_marking;
  markingsADT marking_stack=NULL;
  bool warning=FALSE;
  int i,sn;
  bool stuck;
  char * firelist;
  char * new_firelist;
  timeval t0,t1;
  double time0, time1;
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
#endif
#endif
  int iter=0;
  int stack_depth=0;

  gettimeofday(&t0, NULL);
  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".rsg");
    printf("Finding reduced state graph and storing to:  %s\n",outname);
    fprintf(lg,"Finding reduced state graph and storing to:  %s\n",outname);
    fp=Fopen(outname,"w"); 
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,nsigs,ninpsig);
    fprintf(fp,"STATES:\n");
  } else {
    printf("Finding reduced state graph ... ");
    fflush(stdout);
    fprintf(lg,"Finding reduced state graph ... ");
    fflush(lg);
  }
  initialize_state_table(LOADED,FALSE,state_table);
  marking=find_initial_marking(events,rules,markkey,nevents,nrules,ninpsig,
			       nsignals,startstate,verbose);
  if (marking == NULL) {
    if (verbose) fclose(fp);
    return(FALSE);
  }
  firelist=find_initial_fire_list(events,rules,markkey,marking->marking,nrules,
				  marking,nsigs,nevents); 
  push_marking(&marking_stack,marking,firelist);
  stack_depth++;
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	    marking->state,marking->marking,
	    marking->enablings,nsigs, NULL, NULL, 0, 0,verbose, 0, nrules,
	    0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, rules,0);
  sn=1;
  while ((marking=pop_marking(&marking_stack,&firelist)) != NULL) {
    stack_depth--;
    stuck=TRUE;
    for (i=1;i<nevents;i++) 
      if (firelist[i]=='T') {
	stuck=FALSE;
	iter++;
	if ((si) || (untimed) ||
	    (!slow(&warning,events,rules,markkey,cycles,nevents,ncycles,
		   nsignals,i,marking,closure,nrules))) {
	  if (i < (nevents-ndummy))
	    new_marking=find_new_marking(events,rules,markkey,marking,i,
					 nevents,nrules,verbose, nsigs,ninpsig,
					 timeopts.disabling||timeopts.nofail,
					 noparg);
	  else 
	    new_marking=dummy_find_new_marking(events,rules,markkey,marking,i,
					       nevents,nrules,verbose, nsigs, 
					       ninpsig,timeopts.disabling||
					       timeopts.nofail,noparg);
	  if (!new_marking) {
	    add_state(fp,state_table,marking->state,marking->marking,
		      marking->enablings,NULL,NULL,0,NULL,NULL,NULL,NULL,
		      nsigs,NULL,NULL,0,sn,verbose,FALSE,nrules,0,0,
		      bdd_state,use_bdd,marker,timeopts,-1,i,NULL,rules,
		      nevents);
	    sn++;
	    if (!(timeopts.nofail) && !(timeopts.keepgoing)) return(FALSE);
	  } else {
	    if (add_state(fp,state_table,marking->state,marking->marking,
			  marking->enablings, NULL, NULL, 0, NULL, 
			  new_marking->state,
			  new_marking->marking,new_marking->enablings,
			  nsigs, NULL, NULL, 0, sn,verbose, 0, nrules,0,0,
			  bdd_state,use_bdd,marker,timeopts, -1, i, NULL, 
			  rules, nevents)) {
	      sn++;
	      if (sn % 1000 == 0) {
		printf("%d iterations %d states (stack_depth = %d)\n",iter,sn,
		       stack_depth);
		fprintf(lg,"%d iterations %d states (stack_depth = %d)\n",iter,
			sn,stack_depth);
#ifdef MEMSTATS
#ifndef OSX
		memuse=mallinfo();
		printf("memory: max=%d inuse=%d free=%d \n",
		       memuse.arena,memuse.uordblks,memuse.fordblks);
		fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
			memuse.arena,memuse.uordblks,memuse.fordblks);
#else
		malloc_statistics_t t;
		malloc_zone_statistics(NULL, &t);
		printf("memory: max=%d inuse=%d allocated=%d\n",
		       t.max_size_in_use,t.size_in_use,t.size_allocated);
		fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
			t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
	      }
	      new_firelist=find_fire_list(events,rules,markkey,
					  new_marking->marking,nrules,
					  new_marking,
					  nsigs,nevents,firelist); 
	      push_marking(&marking_stack,new_marking,new_firelist);
	      stack_depth++;
	    } else {
	      delete_marking(new_marking);
	      free(new_marking);
	    }
	  } 
	}
      }
    //if (marking)
    //  printf("%s\n",marking->marking);
    delete_marking(marking);
    free(marking);
    free(firelist);
    if (stuck) {
      if (timeopts.nofail) {
	printf("WARNING: Deadlock!\n");
	fprintf(lg,"WARNING: Deadlock!\n");
      } else {
	printf("ERROR: Deadlock!\n");
	fprintf(lg,"ERROR: Deadlock!\n");
	if (verbose) fclose(fp);
	return false;
      }
    }
  }
  /*  print_table_distribution(state_table); */
  gettimeofday(&t1,NULL);	
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  if (verbose) {
    printf("%d states explored in %g seconds\n",sn,time1-time0);
    fprintf(lg,"%d states explored in %g seconds\n",sn,time1-time0);
    fclose(fp);
  } else {
    printf("done (states=%d,time=%f)\n",sn,time1-time0);
    fprintf(lg,"done (states=%d,time=%f)\n",sn,time1-time0);
  }
  if (warning) {
    printf("WARNING:  More cycles may remove more states.\n");
    fprintf(lg,"WARNING:  More cycles may remove more states.\n");
  }
  return TRUE;
//  return(check_csc(filename,signals,state_table,nsigs,ninpsig,verbose));
}

markingADT fire_dummy_events(eventADT *events,ruleADT **rules,
			     markkeyADT *markkey,markingADT marking,
			     int nevents,int nrules,bool verbose,
			     int nsigs,int ninpsig,bool disabling,
			     int ndummy,char * firelist,bool noparg)
{
  int i;
  markingADT new_marking=NULL;

  i=(nevents-ndummy);
  while (i<nevents) {
    if (firelist[i]=='T') {
      printf("Firing dummy %s\n",events[i]->event);
      new_marking=dummy_find_new_marking(events,rules,markkey,marking,i,
					 nevents,nrules,verbose, nsigs, 
					 ninpsig,FALSE,noparg);
      if (!new_marking) return NULL;
      i=(nevents-ndummy);
      free(firelist);        /* NEED TO KILL OLD FIRELIST MEMORY */
      firelist=find_fire_list(events,rules,markkey,new_marking->marking,nrules,
			      new_marking,nsigs,nevents,firelist);
      marking=new_marking;
    } else
      i++;
  }
  return new_marking;
}
  

/*****************************************************************************/
/* Find reduced state graph using constrained token flow.                    */
/*****************************************************************************/

bool find_csg(char * filename,signalADT *signals,eventADT *events,
	      ruleADT **rules,cycleADT ****cycles,stateADT *state_table,
	      markkeyADT *markkey,int ninpsig,int nsigs,
	      int nevents,int nrules,int nsignals,int ndummy,
	      int ncycles,char * startstate,bool si,
	      bool verbose,bool closure,bddADT bdd_state,
	      bool use_bdd,markkeyADT *marker,bool untimed,
	      timeoptsADT timeopts,bool noparg)
{
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  markingADT marking;
  markingADT new_marking=NULL;
  markingsADT marking_stack=NULL;
  bool warning=FALSE;
  int i,j,sn;
  bool stuck,newEn;
  char * firelist;
  char * new_firelist;
  char * curstate;
  char * curmarking;

  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".csg");
    printf("Finding compressed state graph and storing to:  %s\n",outname);
    fprintf(lg,"Finding compressed state graph and storing to:  %s\n",outname);
    fp=Fopen(outname,"w"); 
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,nsigs,ninpsig);
    fprintf(fp,"STATES:\n");
  } else {
    printf("Finding compressed state graph ... ");
    fflush(stdout);
    fprintf(lg,"Finding compressed state graph ... ");
    fflush(lg);
  }
  curstate=(char *)GetBlock((nsignals+1)*sizeof(char));
  curmarking=(char *)GetBlock((nrules+1)*sizeof(char));

  initialize_state_table(LOADED,FALSE,state_table);
  marking=find_initial_marking(events,rules,markkey,nevents,nrules,ninpsig,
			       nsignals,startstate,verbose);
  if (marking == NULL) {
    if (verbose) fclose(fp);
    return(FALSE);
  }
  firelist=find_initial_fire_list(events,rules,markkey,marking->marking,nrules,
				  marking,nsigs,nevents); 
  for (i=(nevents-ndummy);i<nevents;i++) 
    if (firelist[i]=='T') {
      new_marking=fire_dummy_events(events,rules,markkey,marking,
				    nevents,nrules,verbose,nsigs,ninpsig,
				    FALSE,ndummy,firelist,noparg);
      free(firelist);
      firelist=find_fire_list(events,rules,markkey,
			      new_marking->marking,nrules,
			      new_marking,nsigs,nevents,
			      firelist);
      marking=new_marking;
      break;
    }
  strcpy(curstate,marking->state);
  strcpy(curmarking,marking->marking);
  for (j=0;j<nsignals;j++)
    if (curstate[j]=='R') curstate[j]='U';
    else if (curstate[j]=='F') curstate[j]='D';
  push_marking(&marking_stack,marking,firelist);
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	    curstate,curmarking,
	    marking->enablings,nsigs, NULL, NULL, 0, 0,verbose, 0, nrules,
	    0,0,bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, rules,
	    nevents);

  sn=1;
  while ((marking=pop_marking(&marking_stack,&firelist)) != NULL) {
    stuck=FALSE;
    strcpy(curstate,marking->state);
    strcpy(curmarking,marking->marking);
    for (j=0;j<nsignals;j++)
      if (curstate[j]=='R') curstate[j]='U';
      else if (curstate[j]=='F') curstate[j]='D';
    for (i=1;i<(nevents-ndummy);i++) 
      if (firelist[i]=='T') {
	printf("Firing %s\n",events[i]->event);
	stuck=FALSE;
	if ((si) || (untimed) || (1) ||
	    (!slow(&warning,events,rules,markkey,cycles,nevents,ncycles,
		   nsignals,i,marking,closure,nrules))) {
	  new_marking=find_new_marking(events,rules,markkey,marking,i,
				       nevents,nrules,verbose, nsigs,ninpsig,
				       FALSE,noparg);
	  if (!new_marking) {
	    add_state(fp,state_table,curstate,curmarking,
		      marking->enablings,NULL,NULL,0,NULL,NULL,NULL,NULL,
		      nsigs,NULL,NULL,0,sn,verbose,FALSE,nrules,0,0,
		      bdd_state,use_bdd,marker,timeopts,-1,i,NULL,rules,
		      nevents);
	    return(FALSE);
	  }

	  new_firelist=find_fire_list(events,rules,markkey,
				      new_marking->marking,nrules,new_marking,
				      nsigs,nevents,firelist);
	  for (j=(nevents-ndummy);j<nevents;j++) 
	    if (new_firelist[j]=='T') {
	      new_marking=fire_dummy_events(events,rules,markkey,new_marking,
					    nevents,nrules,verbose,nsigs,
					    ninpsig,FALSE,ndummy,
					    new_firelist,noparg);
	      break;
	    }
	  newEn=FALSE;
	  for (j=0;j<nsignals;j++)
	    if (((marking->state[j]=='0')&&(new_marking->state[j]=='R'))||
		((marking->state[j]=='1')&&(new_marking->state[j]=='F'))) {
	      newEn=TRUE;
	      break;
	    }
	  for (j=0;j<nsignals;j++)
	    if (new_marking->state[j]=='R') new_marking->state[j]='U';
	    else if (new_marking->state[j]=='F') new_marking->state[j]='D';
	  if (newEn) {
	    if (add_state(fp,state_table,curstate,curmarking,
			  marking->enablings, NULL, NULL, 0, NULL, 
			  new_marking->state,
			  new_marking->marking,new_marking->enablings,
			  nsigs, NULL, NULL, 0, sn,verbose, 0, nrules,0,0,
			  bdd_state,use_bdd,marker,timeopts, -1, i, NULL, 
			  rules,nevents)) {
	      printf("State added: %s\n",new_marking->state);
	      for (j=0;j<nsignals;j++)
		if (new_marking->state[j]=='U') new_marking->state[j]='R';
		else if (new_marking->state[j]=='D') 
		  new_marking->state[j]='F';
	      sn++;
	      free(new_firelist);
	      new_firelist=find_fire_list(events,rules,markkey,
					  new_marking->marking,nrules,
					  new_marking,nsigs,nevents,
					  firelist); 
	      push_marking(&marking_stack,new_marking,new_firelist);
	    } else {
	      for (j=0;j<nsignals;j++)
		if (new_marking->state[j]=='U') new_marking->state[j]='R';
		else if (new_marking->state[j]=='D') 
		  new_marking->state[j]='F';
	      //delete_marking(new_marking);
	      //free(new_marking);
	    }
	  } else {
            /*
	      new_firelist=find_fire_list(events,rules,markkey,
					  new_marking->marking,nrules,
					  new_marking,nsigs,nevents,
					  firelist); 
	    */
	    free(new_firelist);
	    new_firelist=CopyString(firelist);
	    new_firelist[i]='F';
	    strcpy(marking->state,curstate);
	    strcpy(marking->marking,curmarking);
	    for (j=0;j<nsignals;j++)
	      if (marking->state[j]=='U') marking->state[j]='R';
	      else if (marking->state[j]=='D') marking->state[j]='F';
	    push_marking(&marking_stack,marking,new_firelist);
	    marking=new_marking;
	    for (j=0;j<nsignals;j++)
	      if (marking->state[j]=='U') marking->state[j]='R';
	      else if (marking->state[j]=='D') marking->state[j]='F';
	  }
	}
      }
    for (j=0;j<nsignals;j++)
      if (new_marking->state[j]=='R') new_marking->state[j]='U';
      else if (new_marking->state[j]=='F') new_marking->state[j]='D';
    if (add_state(fp,state_table,curstate,curmarking,
		  marking->enablings, NULL, NULL, 0, NULL, 
		  new_marking->state,
		  new_marking->marking,new_marking->enablings,
		  nsigs, NULL, NULL, 0, sn,verbose, 0, nrules,0,0,
		  bdd_state,use_bdd,marker,timeopts, -1, i, NULL, 
		  rules,nevents)) {
      for (j=0;j<nsignals;j++)
	if (new_marking->state[j]=='U') new_marking->state[j]='R';
	else if (new_marking->state[j]=='D') 
	  new_marking->state[j]='F';
      sn++;
      free(new_firelist);
      new_firelist=find_fire_list(events,rules,markkey,
				  new_marking->marking,nrules,
				  new_marking,nsigs,nevents,
				  firelist); 
      push_marking(&marking_stack,new_marking,new_firelist);
    } else {
      for (j=0;j<nsignals;j++)
	if (new_marking->state[j]=='U') new_marking->state[j]='R';
	else if (new_marking->state[j]=='D') 
	  new_marking->state[j]='F';
      //delete_marking(new_marking);
      //free(new_marking);
    }
    //delete_marking(marking);
    //free(marking);
    free(firelist);
    if (stuck) {
      if (verbose) {
	fclose(fp);
	printf("ERROR: Deadlock!\n");
	fprintf(lg,"ERROR: Deadlock!\n");
      } else {
	printf("ERROR!\n");
	fprintf(lg,"ERROR!\n");
	printf("ERROR: Deadlock!\n");
	fprintf(lg,"ERROR: Deadlock!\n");
      }
      return FALSE;
    }
  }
  if (verbose) fclose(fp);
  else {
    printf("done\n");
    fprintf(lg,"done\n");
  }
  if (warning) {
    printf("WARNING:  More cycles may remove more states.\n");
    fprintf(lg,"WARNING:  More cycles may remove more states.\n");
  }
  return TRUE;
}

/*****************************************************************************/
/* Find reduced state graph.                                                 */
/*****************************************************************************/

bool timed_state_space_exploration_methods(designADT design,bool verbose,
					   bool search,bool verify,
					   bool analyze,bool trace) 
{
  fromG = design->fromG | design->fromER;
  for (int i=0;i<design->nevents;i++)
    for (int j=0;j<design->nevents;j++)
      design->rules[i][j]->reachable=FALSE;
  if (trace) 
    return(findTrace(design->filename,design->signals,design->events,
		     design->rules,design->state_table,design->markkey,
		     design->nevents,design->nplaces,design->nsignals,
		     design->ninpsig,design->nrules,design->startstate,
		     design->timeopts.nofail,design->verbose));
  else if (analyze) 
    return(bio_rsg(design->filename,design->signals,design->events,
		   design->rules,design->state_table,design->markkey,
		   design->nevents,design->nplaces,design->nsignals,
		   design->ninpsig,design->nrules,design->startstate,
		   design->si,verbose,design->bdd_state,design->bdd,
		   design->timeopts,design->ndummy,design->ncausal, 
		   design->nord,design->noparg,search,design->dot,
		   design->bap,design->tolerance));
  else if (design->fromLPN && design->srl) 
    return(lpnRsg(design->filename,design->signals,design->events,
       design->rules,design->state_table,design->markkey,
		   design->nevents,design->nplaces,design->nsignals,
		   design->ninpsig,design->nrules,design->startstate,
		   design->si,verbose,design->bdd_state,design->bdd,
		   design->timeopts,design->ndummy,design->ncausal, 
		   design->nord,design->noparg,search,design->dot,
       design->timeopts.PO_reduce,design->timeopts.lpn));
  else if (design->fromLPN && design->hpn) {
    design->timeopts.hpn = true;
/*     int oldSize = design->nevents + design->nplaces; */
/*     bool expanded = expand_design(design); */
/*     if(expanded) { */
/*       make_markkey(design,oldSize); */
/*     } */
    return(hpnRsg(design->filename,design->signals,design->events,
                  design->rules,design->state_table,design->markkey,
                  design->nevents,design->nplaces,design->nsignals,
                  design->ninpsig,design->nrules,design->startstate,
                  design->si,verbose,design->bdd_state,design->bdd,
                  design->timeopts,design->ndummy,design->ncausal, 
                  design->nord,design->noparg,search,design->dot,
                  design->timeopts.PO_reduce,design->manual));
  }
  else if (design->fromLPN && design->lhpndbm) {
    design->timeopts.lhpn = true;
    if (design->expandRate) {
      expandRate(design);
    }
    return(lhpnRsg(design->filename,design->signals,design->events,
                   design->rules,design->state_table,design->markkey,
                   design->inequalities,design->nevents,design->nplaces,
                   design->nsignals,design->ninpsig,design->nrules,
                   design->startstate,design->si,verbose,design->bdd_state,
                   design->bdd,design->timeopts,design->ndummy,
                   design->ncausal,design->nord,design->noparg,search,
                   design->dot,design->timeopts.PO_reduce,design->manual));
  }
  else if (design->fromLPN) 
    return(lpn_rsg(design->filename,design->signals,design->events,
		   design->rules,design->state_table,design->markkey,
		   design->nevents,design->nplaces,design->nsignals,
		   design->ninpsig,design->nrules,design->startstate,
		   design->si,verbose,design->bdd_state,design->bdd,
		   design->timeopts,design->ndummy,design->ncausal, 
		   design->nord,design->noparg,search,design->dot,
		   design->bap));
  else if (design->geometric)
    return(geom_rsg(design->filename,design->signals,design->events,
		    design->merges,design->rules,design->cycles,
		    design->state_table,design->markkey,design->nevents,
		    design->ncycles,(design->nevents-1)/2,design->ninpsig,
		    design->nsignals,design->nrules,design->startstate,
		    design->si,verbose,design->bdd_state,design->bdd,
		    design->markkey,design->timeopts,design->ndummy,
		    &(design->ncausal), &(design->nord),design->noparg));
  // 11/15/00 egm -- Woohoo, a new timing analysis methods!
  else if ( design->bag  || design->bap) {
    try{
      tel t( design->filename, 
	     design->signals, 
	     design->events, 
	     design->rules, 
	     design->markkey, 
	     design->nevents, 
	     design->ninpsig, 
	     design->nsignals, 
	     design->nrules,
	     &(design->ncausal),
	     &(design->nord),
	     design->startstate,  
	     design->ndummy,
	     design->level, 
	     verify );
      initialize_state_table( LOADED, FALSE, design->state_table );
      if ( design->bag ) {
	return( bag_rsg( t, 
			 design->state_table, 
			 design->timeopts,
			 design->bdd_state, 
			 design->bdd,
			 verbose,
			 design->noparg) );
      }
      else {
	// bap_rsg now supports orbmatch -- egm 12/12/00
	return( bap_rsg( t, 
			 design->state_table, 
			 design->timeopts,
			 design->bdd_state, 
			 design->bdd,
			 verbose,
			 design->noparg) );
      }
    }
    catch( error* ptr_e ) {
      ptr_e->print_message( lg );
      delete ptr_e;
      return( false );
    }
  }
  else if ((design->pomaxdiff) || (design->poapprox)|| (design->posets))
    return(approx_rsg(design->filename,design->signals,design->events,
		      design->merges,design->rules,design->cycles,
		      design->state_table, design->markkey,design->nevents,
		      design->ncycles,(design->nevents-1)/2,design->ninpsig,
		      design->nsignals,design->nrules,design->startstate,
		      design->si,verbose,design->bdd_state,
		      design->bdd,design->markkey,design->timeopts,
		      design->ndummy,&(design->ncausal),
		      &(design->nord),design->poapprox,design->noparg)); 
  else if (design->orbits) 
    return(orbits_rsg(design->filename,design->signals,design->events,
		      design->merges,design->rules,design->cycles,
		      design->state_table,design->nevents,design->ncycles,
		      (design->nevents-1)/2,design->ninpsig,design->nsignals, 
		      design->startstate,design->si,verbose,design->bdd_state, 
		      design->bdd,design->markkey,design->timeopts,
		      design->level,design->ndummy)); 
  else if (design->compress) 
    return(find_csg(design->filename,design->signals,design->events,
		    design->rules,design->cycles,design->state_table,
		    design->markkey,design->ninpsig,design->nsignals,
		    design->nevents,design->nrules,
		    ((design->nevents-design->ndummy)-1)/2,design->ndummy,
		    design->ncycles,design->startstate,design->si,verbose,
		    design->closure,design->bdd_state,design->bdd,
		    design->markkey,design->untimed,design->timeopts,
		    design->noparg));
  else
    return(find_rsg(design->filename,design->signals,design->events,
		    design->rules,design->cycles,design->state_table,
		    design->markkey,design->ninpsig,design->nsignals,
		    design->nevents,design->nrules,
		    ((design->nevents-design->ndummy)-1)/2,design->ndummy,
		    design->ncycles,design->startstate,design->si,verbose,
		    design->closure,design->bdd_state,design->bdd,
		    design->markkey,design->untimed,design->timeopts,
		    design->noparg));
  return false;
}

bool find_reduced_state_graph(char menu,char command,designADT design)
{
  bool verbose;
  bool verify=(menu==VERIFY);
  bool search=((menu==VERIFY)&&(command==SEARCH));
  bool analyze=((menu==VERIFY)&&(command==ANALYZE));
  bool trace=((menu==VERIFY)&&(command==TRACE));

  verbose=design->verbose;
  if (command==FINDRSG) verbose=TRUE;
  if (!(design->status & FOUNDSTATES)) {
    disabled=(bool*)GetBlock(sizeof(bool)*design->nevents);
    for (int i=0;i<design->nevents;i++) {
      disabled[i]=false;
		}
    if (!timed_state_space_exploration_methods(design,verbose,search,
                                               verify,analyze,trace)) {
      free(disabled);
      /*
        if (command==VERIFY) {
        printf("ERROR: Verification failed!\n");
        fprintf(lg,"ERROR: Verification failed!\n");
        }
      */
      if (!design->orbits) {
				printf("Finding the trace!\n");
        find_trace(design->state_table,design->startstate,design->nsignals);
      }
      /*
        if (!design->untimed && !design->heuristic) {
        time_trace(design->state_table,design->startstate,design->nsignals,
        design->nevents,design->rules);
        } else
      */
      untime_trace(design->state_table,design->startstate,design->nsignals,
                   design->nevents,design->rules);
      if (design->dot) {
        dot_print_rsg(design->filename,design->signals,design->events,
                      design->state_table,design->nsignals,design->nevents,
                      true,false,true,design->noparg);
      }
      else {
        print_trace(design->filename,design->signals,design->events,
                    design->state_table,design->nsignals,TRUE,
                    design->timeopts.genrg,design->nevents,design->rules,TRUE,
                    design->noparg,false,design->nplaces);
      }
      print_dinotrace(design->filename,design->signals,design->events,
                      design->state_table,design->nsignals,TRUE,
                      design->timeopts.genrg,design->nevents,
                      design->rules,FALSE,design->noparg,true,design->ndummy);
      if(design->timeopts.lhpn){
	print_ams_error_trace(design->filename,design->signals,design->events,
			      design->state_table,design->nsignals,
			      design->nineqs,TRUE,design->timeopts.genrg,
			      design->nevents,design->nplaces,design->rules,FALSE,
			      design->noparg,true);
	print_vcd_error_trace(design->filename,design->signals,design->events,
			      design->state_table,design->nsignals,
			      design->nineqs,TRUE,design->timeopts.genrg,
			      design->nevents,design->nplaces,design->rules,
			      FALSE,design->noparg,true);
      }
      print_error_trace(design->filename,design->signals,design->events,
			design->state_table,design->nsignals,TRUE,
			design->timeopts.genrg,design->nevents,design->nplaces,
			design->rules,FALSE,design->noparg,true,false);
      /*
      if (design->fromLPN) {
        find_causal_sequence(design->events,design->rules,design->state_table,
                             design->nevents,design->nplaces);
        print_trace("causal_rsg",design->signals,design->events,
                    design->state_table,design->nsignals,TRUE,
                    design->timeopts.genrg,design->nevents,design->rules,TRUE,
                    design->noparg,true);
        print_error_trace(design->filename,design->signals,design->events,
                          design->state_table,design->nsignals,TRUE,
                          design->timeopts.genrg,design->nevents,design->rules,
                          FALSE,design->noparg,true,true);
        print_lpn("causal",design->events,design->rules,
                  design->nevents,design->nplaces,TRUE,NULL,true,
                  design->noparg,design->dot,NULL);
      }
      */
      /*vhdl_trace(design->filename,design->signals,design->events,
        design->state_table,design->nsignals,TRUE,
        design->timeopts.genrg,design->nevents);*/
      if (command==VERATACS) {
        printf("Verification failed.\n");
        fprintf(lg,"Verification failed.\n");
      }
      return FALSE;
    } else {
      free(disabled);
      /*
        if (command==VERIFY) {
        printf("Verification succeeded.\n");
        fprintf(lg,"Verification succeeded.\n");
        }
      */
      design->status=design->status | FOUNDSTATES;
    }
  }
  /* if (command==VERIFY) return TRUE; */
  if (design->bdd) {
    design->status=design->status | FOUNDRSG;
    return TRUE;
  }

  //my_print_graph(design->state_table,design->events,design->markkey,
  //		 design->nrules);

  //my_print_graph(design->state_table,design->events,design->markkey,
  //		 design->nrules);
  //my_print_graph2(design->state_table,design->events,design->markkey,
  //		  design->nrules);

  if (command==FINDRSG) {
    return TRUE;
  }
  if (analyze || trace) return true;
  if (command==PROJECT) verbose=TRUE;
  if (!(design->status & FOUNDRSG)) {
    //design->nsignals = 2;
    if (!(design->timeopts.noproj)) {
      project(design->filename,design->signals,design->state_table,
	      design->ninpsig,&(design->nsignals),verbose,design->events,
	      design->nevents,design->startstate,design->nineqs);
    }
    if (!(design->timeopts.disabling || design->timeopts.nofail))
      if (!check_disablings(design->filename,design->signals,
			    design->state_table,
			    design->nsignals,design->ninpsig,verbose)) {
	return FALSE;
      }
    bool check=true;
    for (proplistADT curprop=design->properties;curprop;curprop=curprop->next){
      if (!checkCTL(design->state_table,design->signals,design->nsignals,
		    design->nevents,design->nplaces,design->events,
		    design->rules,curprop->property)) check=false;
    }      
    if ((command==VERATACS)&&(check)) {
      printf("Verification succeeded.\n");
      fprintf(lg,"Verification succeeded.\n");
      return true;
    } else if (command==VERATACS) {
      printf("Verification failed.\n");
      fprintf(lg,"Verification failed.\n");
      return true; /* false;*/
    }
    if (!check_csc(design->filename,design->signals,design->state_table,
                   design->nsignals,design->ninpsig,verbose)) {
      design->status &= ~CSC;
      if ((command==ADDPERS)||(command==SOLVECSC)||(command==FINDSV)||
          (design->insertion)) {
        if ((command==ADDPERS)||(command==SOLVECSC))
          find_regions(command,design);
#ifdef INTEL
        if ((command==SOLVECSC) || (design->insertion)) {
          if (!solve_csc(design->filename,design->signals,design->events,
                         design->merges,design->rules,design->cycles,
                         design->markkey,design->state_table,
                         design->nevents,design->ncycles,design->ninputs,
                         design->nrules,design->maxsize,
                         design->maxgatedelay,design->startstate,TRUE,NULL,
                         design->status,design->regions,design->manual,
                         design->ninpsig,design->nsignals,verbose,
                         design->fromER,design->fromTEL)) {
            return FALSE;
          }
          design->fromCSP=FALSE;
          design->fromVHDL=FALSE;
        }
        if (command==FINDSV)
#ifdef NEW_CSC
          if (!find_state_code(command,design)) {
#else
            if (!find_state_assignment(command,design)) {
#endif
              return FALSE;
            } else {
              design->status |= CSC;
              return TRUE;
            }
            if ((command!=ADDPERS) && (command!=SOLVECSC)) 
              design->status=process_command(SYNTHESIS,command,design,NULL,
                                             TRUE,design->si);
            else 
              design->status=process_command(LOAD,TIMEDER,design,NULL,TRUE,
                                             design->si);
#else
            if (command==SOLVECSC)
              printf("Not available in this version.\n");
            if ((command==FINDSV) || (design->insertion))
#ifdef NEW_CSC
              if (!find_state_code(command,design)) {
#else
                if (!find_state_assignment(command,design)) {
#endif
                  return FALSE;
                } else {
                  design->status |= CSC;
                  return TRUE;
                }
#endif
            return(FALSE);
          } else {
            return(FALSE);
          }
      }
      design->status=design->status | FOUNDRSG;
    }
    design->status |= CSC;
    return(TRUE);
  }
