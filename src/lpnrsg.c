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
#include "lpnrsg.h"
#include "findrsg.h"
#include "findreg.h"
#include "common_timing.h"
#include "printlpn.h"
#include <cmath>

#define __LEVELS__
//#define __PO_DUMP__
//#define __DUMP__

int amp_cnt=0;
int enb_cnt=0;
int cur_per=0;

int spor=(-1);
int comp=(-1);
int spor_comp=0;
int comp_spor=0;

int my_depth=0;

/*****************************************************************************/
/* Create a new necessary cache.                                             */
/*****************************************************************************/
bool **create_necessary_cache(int size)
{
  bool **nec_cache;
  nec_cache=(bool **)GetBlock(size * sizeof(bool*));
  for (int i=0;i<size;i++)
    nec_cache[i]=NULL; /* Set entry as invalid */
  return nec_cache;
}

/*****************************************************************************/
/* Destroy a necessary cache.                                                */
/*****************************************************************************/
void destroy_necessary_cache(bool **nec_cache,int size)
{
  for (int i=0;i<size;i++) 
    if (nec_cache[i]) 
      free(nec_cache[i]);
  free(nec_cache);
}

/*****************************************************************************/
/* Display message and open output file if verbose.                          */
/*****************************************************************************/

FILE *lpn_print_msg(char *filename,signalADT *signals,int nsignals,
		    int ninpsig,bool verbose)
{
  FILE *fp=NULL;
  char outname[FILENAMESIZE];

  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".rsg");
    printf("Finding reduced state graph and storing to:  %s\n",outname);
    fprintf(lg,"Finding reduced state graph and storing to:  %s\n",outname);
    fp=fopen(outname,"w"); 
    if (!fp) {
      printf("ERROR: Could not open file %s\n",outname);
      fprintf(lg,"ERROR: Could not open file %s\n",outname);
      return NULL;
    }
    fprintf(fp,"SG:\n");
    print_state_vector(fp,signals,nsignals,ninpsig);
    fprintf(fp,"STATES:\n");
  } else {
    printf("Finding reduced state graph ... \n");
    fflush(stdout);
    fprintf(lg,"Finding reduced state graph ... \n");
    fflush(lg);
  }
  return fp;
}

/*****************************************************************************/
/* Display state count and memory statistics.                                */
/*****************************************************************************/

void lpn_display_info(int iter,int sn,int regions,int stack_depth,bool bap)
{
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
#else
  malloc_statistics_t t;
#endif
#endif

  if ((bap && regions % 1000==0) || (!bap && sn % 1000 == 0)) {
    if (bap) {
      printf("%d iterations %d zones %d states (stack_depth = %d)\n",iter,
	     regions,sn,stack_depth);
      fprintf(lg,"%d iterations %d zones %d states (stack_depth = %d)\n",iter,
	      regions,sn,stack_depth);
    } else {
      printf("%d iterations %d states (stack_depth = %d)\n",iter,
	     sn,stack_depth);
      fprintf(lg,"%d iterations %d states (stack_depth = %d)\n",iter,
	      sn,stack_depth);
    }
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
}

/*****************************************************************************/
/* Checks to see if the rule indicated by enabling and enabled has           */
/* its level satisfied.                                                      */
/*****************************************************************************/

bool lpn_level_satisfied(ruleADT **rules,char *state,int nsignals, 
			 int enabling,int enabled)
{
  bool satisfied;

  if (!rules[enabling][enabled]->level) return false;
  for (level_exp cur_level=rules[enabling][enabled]->level;
       cur_level;cur_level=cur_level->next) {
    satisfied = true;
    for(int i=0;i<nsignals;i++){
      if ((cur_level->product[i]=='1' && (state[i]=='0' || state[i]=='R')) ||
	  (cur_level->product[i]=='0' && (state[i]=='1' || state[i]=='F'))) {
	satisfied=false;
	break;
      }
    }
    if (satisfied) return true;
  }
  return false;
}

/*****************************************************************************/
/* Display deadlock error message.                                           */
/*****************************************************************************/

bool lpn_found_deadlock(eventADT *events,ruleADT **rules,markkeyADT *markkey,
			markingADT marking,int nevents,int nplaces,int nrules,
			bool verbose,bool nofail,bool search,int nsignals)
{
  //  return false;
  if (search) {
    bool found=true;
    for (int i=0;i<nrules;i++)
      if (((marking->marking[i]=='F') ||
	   (!lpn_level_satisfied(rules,marking->state,nsignals,
				 markkey[i]->enabling,
				 markkey[i]->enabled))) &&
	  (rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype 
	   & ORDERING))
	found=false;
    if (found) {
      if (!verbose) {
	printf("done.\n");
	fprintf(lg,"done.\n");
      }
      printf("Found trace with all constraint rules marked.\n");
      fprintf(lg,"Found trace with all constraint rules marked.\n");
    }
    return found;
  }
  bool first=true;
  for (int i=0;i<nrules;i++)
    if ((marking->marking[i]=='T') &&
	(lpn_level_satisfied(rules,marking->state,nsignals,
			     markkey[i]->enabling,
			     markkey[i]->enabled)) &&
      (rules[markkey[i]->enabling][markkey[i]->enabled]->ruletype 
	 & ORDERING)) {
      if (first) {
	if (!verbose) {
	  if (nofail) {
	    printf("WARNING!\n");
	    fprintf(lg,"WARNING!\n");
	  } else {
	    printf("ERROR!\n");
	    fprintf(lg,"ERROR!\n");
	  }
	}
	if (nofail) {
	  printf("WARNING: Deadlock with these constraint rules marked!\n");
	 fprintf(lg,"WARNING: Deadlock with these constraint rules marked!\n");
	} else {
	  printf("ERROR: Deadlock with these constraint rules marked!\n");
	  fprintf(lg,"ERROR: Deadlock with these constraint rules marked!\n");
	}
        first=false;
      }
      printf("\t%s -> %s\n",events[markkey[i]->enabling]->event,
	     events[markkey[i]->enabled]->event);
    }
  if (nofail) return false;
  else return(!first); 
}

/*****************************************************************************/
/* Check if valid rule.                                                      */
/*****************************************************************************/
bool lpn_valid_rule(int ruletype)
{
  return ((ruletype > NORULE) && (!(ruletype & REDUNDANT)));
} 

/*****************************************************************************/
/* Check if valid rule.                                                      */
/*****************************************************************************/
bool lpn_causal_rule(int ruletype)
{
  return ((ruletype > NORULE) && (!(ruletype & ORDERING)));
} 

/*****************************************************************************/
/* Create new marking with initially marked rules.                           */
/*****************************************************************************/

markingADT lpn_new_marking(eventADT *events,ruleADT **rules,
			   markkeyADT *markkey,int nrules,int nsignals,
			   int nevents)
{
  int j;
  markingADT marking=NULL;

  marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=(char *)GetBlock((nrules+1)*sizeof(char));
  marking->state=(char *)GetBlock((nsignals+1)*sizeof(char));
  marking->enablings=(char *)GetBlock((nevents+1)*sizeof(char));
  marking->up=NULL;
  marking->down=NULL;
  marking->ref_count = 0;

  /* Find initially marked rules */
  for (j=0;j<nrules;j++) {
    if (lpn_valid_rule(rules[markkey[j]->enabling][markkey[j]->enabled]->ruletype)
	&& (markkey[j]->epsilon==1)) marking->marking[j]='T';
    else marking->marking[j]='F';
  }
  marking->marking[nrules]='\0';

  /* Clear list of enabled events */
  for (j=0;j<nevents;j++)
    marking->enablings[j]='F';
  marking->enablings[j]='\0';

  return marking;
}

/*****************************************************************************/
/* Copy the startstate into a marking and error if start state is missing.   */
/*****************************************************************************/

bool lpn_copy_startstate(markingADT marking,char *startstate,bool verbose)
{
  if (startstate==NULL) { 
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR:  Need initial state information.\n");
    fprintf(lg,"ERROR:  Need initial state information.\n");
    delete_marking(marking);
    free(marking);
    return false;
  } else strcpy(marking->state,startstate);
  return true;
}

/*****************************************************************************/
/* Check if a signal is enabled given a marking.                             */
/*****************************************************************************/

bool lpn_event_enabled(eventADT *events,ruleADT **rules,markingADT marking,
		       int event,int nevents,int nplaces,int nsignals,
		       bool chkord,bool chkassump,bool verbose,bool nofail)
{
  int i;

  /* Event is unused so it can never be enabled */
  if (events[event]->dropped) return false;
  for (i=1;i<nevents+nplaces;i++) 
    /* Check if valid rule considering whether we wish to check ordering
       rules and assumption rules */
    if (check_rule(rules,i,event,chkord,chkassump)) 
      /* Rule is either not marking or level satisfied so break out */
      if ((marking->marking[rules[i][event]->marking]=='F') ||
	  (!lpn_level_satisfied(rules,marking->state,nsignals,i,event))) 
	break;
  if (i!=(nevents+nplaces)) {
    /* If checking ordering rules and a rule is not marking level satisfied
       it must be an ordering rule so there is a failure */
    if (chkord) {
      if (!verbose) {
	if (nofail) {
	  printf("WARNING!\n");
	  fprintf(lg,"WARNING!\n");
	} else {
	  printf("ERROR!\n");
	  fprintf(lg,"ERROR!\n");
	}
      }
      if (nofail) {
	printf("WARNING: constraint (%s,%s) not satisfied in state %s.\n",
	       events[i]->event,events[event]->event,marking->state);
	fprintf(lg,"WARNING: constraint (%s,%s) not satsified in state %s.\n",
		events[i]->event,events[event]->event,marking->state);
      } else {
	printf("ERROR: constraint (%s,%s) not satisfied in state %s.\n",
	       events[i]->event,events[event]->event,marking->state);
	fprintf(lg,"ERROR: constraint (%s,%s) not satsified in state %s.\n",
		events[i]->event,events[event]->event,marking->state);
      }
    }
    return false;
  }
  return true;
}

/*****************************************************************************/
/* Find all enabled events given a marking.                                  */
/*****************************************************************************/

void lpn_events_enabled(eventADT *events,ruleADT **rules,markkeyADT *markkey,
			markingADT marking,int nrules,int nsignals,int nevents,
			bool chkassump)
{
  int i;

  for (i=0;i<nevents;i++)
    if (events[i]->dropped) 
      marking->enablings[i]='F';
    else 
      marking->enablings[i]='T';
  for (i=0;i<nrules;i++) {
    if ((markkey[i]->enabled < nevents) &&
	(marking->enablings[markkey[i]->enabled]=='T') && 
	(check_rule(rules,markkey[i]->enabling,markkey[i]->enabled,false,
		    chkassump))) 
      if ((marking->marking[i]=='F') ||
	  (!lpn_level_satisfied(rules,marking->state,nsignals,
				markkey[i]->enabling,
				markkey[i]->enabled))) {
	marking->enablings[markkey[i]->enabled]='F';
      }
  }
}

/*****************************************************************************/
/* Check if state is consistent with an enabled event.                       */
/*****************************************************************************/

bool lpn_check_consistency(char *event,char *state,int signal,int ninpsig,
			   bool verbose)
{
  if (signal >= ninpsig) {
    if (((strchr(event,'+')) && (state[signal]!='0') && (state[signal]!='R'))||
	((strchr(event,'-')) && (state[signal]!='1') && (state[signal]!='F'))){
      if (!verbose) {
	printf("ERROR!\n");
	fprintf(lg,"ERROR!\n");
      }
      printf("ERROR: %s enabled inconsistently in state %s\n",event,state);
      fprintf(lg,"ERROR: %s enabled inconsistently in state %s\n",event,state);
      return false;
    }
  }
  return true;
}

/*****************************************************************************/
/* Find initial marking.                                                     */
/*****************************************************************************/

markingADT lpn_find_initial_marking(eventADT *events,ruleADT **rules,
				    markkeyADT *markkey,int nevents,
				    int nplaces,int nrules,int ninpsig,
				    int nsignals,char * startstate,
				    bool verbose,bool nofail)
{
  markingADT marking=NULL;
  int j;

  /* Create initial marking */
  marking=lpn_new_marking(events,rules,markkey,nrules,nsignals,nevents);

  /* Copy startstate as initial state, if missing abort */
  if (!lpn_copy_startstate(marking,startstate,verbose)) return NULL;

  lpn_events_enabled(events,rules,markkey,marking,nrules,nsignals,nevents,
		     false);

  /* Find enabled events and update statevector with enablings */
  for (j=1;j<nevents;j++) {
    if (marking->enablings[j]=='T') {
      /*    if (lpn_event_enabled(events,rules,marking,j,nevents,nplaces,nsignals,
			  FALSE,FALSE,verbose,nofail)) {
			  marking->enablings[j]='T';*/
      if (events[j]->signal >= 0) {
	if (!lpn_check_consistency(events[j]->event,marking->state,
				   events[j]->signal,ninpsig,verbose))
	  return NULL;
	if (strchr(events[j]->event,'+')) 
	  marking->state[events[j]->signal]='R';
	else
	  marking->state[events[j]->signal]='F';
      }
    }
  }
  return(marking);
}

/*****************************************************************************/
/* Push marking onto stack of markings.                                      */
/*****************************************************************************/

void lpn_push_marking(lpn_stackADT *marking_stack,markingADT marking,
		      char * firelist,zoneADT zone)
{
  lpn_stackADT new_stack=NULL;

  new_stack=(lpn_stackADT)GetBlock(sizeof *new_stack);
  new_stack->marking=marking;
  new_stack->firelist=firelist;
  new_stack->zone=zone;
  new_stack->link=(*marking_stack);
  (*marking_stack)=new_stack;
}

/*****************************************************************************/
/* Pop marking onto stack of markings.                                       */
/*****************************************************************************/

markingADT lpn_pop_marking(lpn_stackADT *marking_stack,char * *firelist,
			   zoneADT *zone)
{
  markingADT marking;
  lpn_stackADT old_markingstack;

  if (*marking_stack==NULL)
    return(NULL);
  marking=(*marking_stack)->marking;
  (*firelist)=(*marking_stack)->firelist;
  (*zone)=(*marking_stack)->zone;
  old_markingstack=(*marking_stack);
  (*marking_stack)=(*marking_stack)->link;
  free(old_markingstack);
  return(marking);
}

/*****************************************************************************/
/* Print safety failure error message.                                       */
/*****************************************************************************/

void lpn_safety_failure(char *e,char *f,bool verbose,bool nofail)
{
  if (nofail) {
    printf("WARNING: Safety violation for rule %s -> %s.\n",e,f);
    fprintf(lg,"WARNING: Safety violation for rule %s -> %s.\n",e,f);
  } else {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Safety violation for rule %s -> %s.\n",e,f);
    fprintf(lg,"ERROR: Safety violation for rule %s -> %s.\n",e,f);
  }
}

/*****************************************************************************/
/* Print vacuous firing failure error message.                               */
/*****************************************************************************/

void lpn_vacuous_failure(char *e,bool verbose)
{
  if (!verbose) {
    printf("ERROR!\n");
    fprintf(lg,"ERROR!\n");
  }
  printf("ERROR: Firing of %s is a vacuous transition.\n",e);
  fprintf(lg,"ERROR: Firing of %s is a vacuous transition.\n",e);
}

/*****************************************************************************/
/* Find new marking given that signal i fired.                               */
/*****************************************************************************/

bool lpn_fire_transition(eventADT *events,ruleADT **rules,
			 markingADT marking,int i,int nevents,int nplaces,
			 int nsignals,bool verbose,bool nofail)
{
  int j,k;

  /* Check if fired transition enabled, if not constraint failure */
  if (!lpn_event_enabled(events,rules,marking,i,nevents,nplaces,
			 nsignals,TRUE,TRUE,verbose,nofail)) {
    if (!nofail) return false;
  }
  /* Remove tokens from fanin places */
  for (j=1;j<nevents;j++) 
    if (lpn_valid_rule(rules[j][i]->ruletype)) {
      marking->marking[rules[j][i]->marking]='F';
    }
  for (j=nevents;j<nevents+nplaces;j++)
    if (lpn_valid_rule(rules[j][i]->ruletype))
      for (k=1;k<nevents;k++) 
	if (lpn_valid_rule(rules[j][k]->ruletype)) {
	  marking->marking[rules[j][k]->marking]='F';
	}
  /* Add tokens to fanout places, check safety */
  for (j=1;j<nevents;j++) 
    if (lpn_valid_rule(rules[i][j]->ruletype)) {
      if (marking->marking[rules[i][j]->marking]=='F')
	marking->marking[rules[i][j]->marking]='T';
      else {
	lpn_safety_failure(events[i]->event,events[j]->event,verbose,nofail);
	return false;
      }
    }
  for (j=nevents;j<nevents+nplaces;j++)
    if (lpn_valid_rule(rules[i][j]->ruletype)) {
      for (k=1;k<nevents;k++) {
	if (lpn_valid_rule(rules[j][k]->ruletype)) {
	  if (marking->marking[rules[j][k]->marking]=='F')
	    marking->marking[rules[j][k]->marking]='T';
	  else {
	    lpn_safety_failure(events[j]->event,events[k]->event,verbose,
			       nofail);
	    return false;
	  }
	}
      }
    }
  /* Fired event is no longer enabled */
  marking->enablings[i]='F';
  /* If transition on a signal, update the state vector, check vacuous */
  if (events[i]->signal >= 0) {
    if (marking->state[events[i]->signal]=='R') 
      marking->state[events[i]->signal]='1';
    else if (marking->state[events[i]->signal]=='F') 
      marking->state[events[i]->signal]='0';
    else {
      lpn_vacuous_failure(events[i]->event,verbose);
      return false;
    }
  }
  return true;
}

/*****************************************************************************/
/* Print warning/error message when output event is disabled.                */
/*****************************************************************************/

void lpn_disabling(bool disabling,int j,char *event,bool verbose)
{
  if (disabling) {
    if (!disabled[j]) {
      printf("WARNING: Output event %s is disabled\n",event);
      fprintf(lg,"WARNING: Output event %s is disabled\n",event);
      disabled[j]=true;
    }
  } else {
    if (!verbose) {
      printf("ERROR!\n");
      fprintf(lg,"ERROR!\n");
    }
    printf("ERROR: Output event %s is disabled\n",event);
    fprintf(lg,"ERROR: Output event %s is disabled\n",event);
  }
}

/*****************************************************************************/
/* Update state after event has fired.                                       */
/*****************************************************************************/

bool lpn_update_state(eventADT *events,ruleADT **rules,
		      markkeyADT *markkey,markingADT marking,
		      int nevents,int nplaces,int nrules,int nsignals,
		      int ninpsig,bool verbose,bool disabling,bool nofail)
{
  int j;
  char *old_enablings;

  /* Clear signal enablings */
  for (j=0;j<nsignals;j++)
    if (marking->state[j]=='R') marking->state[j]='0';
    else if (marking->state[j]=='F') marking->state[j]='1';

  /* Find enabled events */
  old_enablings=CopyString(marking->enablings);
  lpn_events_enabled(events,rules,markkey,marking,nrules,nsignals,nevents,
		     false);

  /* Update statevector with enablings */
  for (j=1;j<nevents;j++)
    if (marking->enablings[j]=='T') {
      if (events[j]->signal >= 0) {
	if (!lpn_check_consistency(events[j]->event,marking->state,
				   events[j]->signal,ninpsig,verbose))
	  return true;
	if (strchr(events[j]->event,'+')) 
	  marking->state[events[j]->signal]='R';
	else
	  marking->state[events[j]->signal]='F';
      }
    } else {
      /* If there has been a disabling, report it and exit if a !disabling */
      if ((events[j]->signal >= ninpsig) && (old_enablings[j]=='T')) {
	lpn_disabling(disabling,j,events[j]->event,verbose);
	if (!disabling) {
	  free(old_enablings);
	  return true;
	}
      }
    }
  free(old_enablings);
  return false;
}

/*****************************************************************************/
/* Find new marking given that signal i fired.                               */
/*****************************************************************************/

markingADT lpn_find_new_marking(signalADT *signals,
				eventADT *events,ruleADT **rules,
				markkeyADT *markkey,markingADT old_marking,
				int i,int nevents,int nplaces,int nrules,
				bool verbose,int nsignals,int ninpsig,
				bool disabling,bool noparg,bool nofail,
				bool use_dot)
{
  markingADT marking=NULL;

  /* Create a copy of the marking */
  marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=CopyString(old_marking->marking);
  marking->state=CopyString(old_marking->state);
  marking->enablings=CopyString(old_marking->enablings);
  marking->up=NULL;
  marking->down=NULL;
  marking->ref_count = 0;

  /* Fire transition and update the marking and state */
  if (!lpn_fire_transition(events,rules,marking,i,nevents,nplaces,nsignals,
			   verbose,nofail)) {
    if (!nofail)
      print_lpn("deadlock",signals,events,rules,nevents,nplaces,TRUE,marking,
		false,noparg,use_dot,NULL); 
    return NULL;
  }
  /* Update the enablings and state vector enablings */
  if (lpn_update_state(events,rules,markkey,marking,nevents,nplaces,nrules,
		       nsignals,ninpsig,verbose,disabling,nofail)) {
    if (!nofail)
      print_lpn("deadlock",signals,events,rules,nevents,nplaces,TRUE,marking,
		false,noparg,use_dot,NULL); 
    return NULL;
  }
  return marking;
}

/*****************************************************************************/
/* Boolean vectorelper functions.                                          */
/*****************************************************************************/

bool *new_bool_vector(int size) 
{
  bool *bool_vec=(bool *)GetBlock(size*sizeof(bool));
  for (int i=0;i<size;i++)
    bool_vec[i]=false;
  return bool_vec;
}

void empty_bool_vector(bool *bool_vec,int size) 
{
  for (int i=0;i<size;i++)
    bool_vec[i]=false;
}

int size_bool_vector(bool *bool_vec,int size) 
{
  int result=0;
  for (int i=0;i<size;i++)
    if (bool_vec[i]) result++;
  return result;
}

bool is_empty2(bool *bool_vec,int size) 
{
  for (int i=0;i<size;i++)
    if (bool_vec[i]) return false;
  return true;
}

void copy_bool_vector(bool *bool_vec1,bool *bool_vec2,int size) 
{
  for (int i=0;i<size;i++)
    bool_vec1[i]=bool_vec2[i];
}

bool *Copy_bool_vector(bool *bool_vec2,int size) 
{
  bool *bool_vec1=(bool*)GetBlock(size * sizeof(bool));
  for (int i=0;i<size;i++)
    bool_vec1[i]=bool_vec2[i];
  return bool_vec1;
}

void union_bool_vector(bool *bool_vec1,bool *bool_vec2,int size) 
{
  for (int i=0;i<size;i++)
    if (bool_vec2[i]) bool_vec1[i]=true;
}

bool intersects(bool *bool_vec1,bool *bool_vec2,int size)
{
  for (int i=0;i<size;i++)
    if (bool_vec1[i] && bool_vec2[i]) return true;
  return false;
}

/*****************************************************************************/
/* Update result of necessary calculation if smaller than previous set.      */
/*****************************************************************************/
bool update_result(res_dirtyADT result,bool *temp,bool *dir_temp,int nevents)
{
  if ((is_empty2(result.res,nevents) || 
       (size_bool_vector(temp,nevents) < 
	size_bool_vector(result.res,nevents)))) {
    copy_bool_vector(result.res,temp,nevents);
    copy_bool_vector(result.dirty,dir_temp,nevents);
    if (size_bool_vector(result.res,nevents)==0) return true;
  }
  return false;
}

/*****************************************************************************/
/* Copy results from temp2 to temp/dir_temp and free temp2.                  */
/*****************************************************************************/

void copy_result(bool *temp,bool *dir_temp,res_dirtyADT temp2,int nevents)
{
  copy_bool_vector(temp,temp2.res,nevents);
  copy_bool_vector(dir_temp,temp2.dirty,nevents);
  free(temp2.res);
  free(temp2.dirty);
}

/*****************************************************************************/
/* Union results from temp2 into temp/dir_temp and free temp2.               */
/*****************************************************************************/

void union_result(bool *temp,bool *dir_temp,res_dirtyADT temp2,int nevents)
{
  union_bool_vector(temp,temp2.res,nevents);
  union_bool_vector(dir_temp,temp2.dirty,nevents);
  free(temp2.res);
  free(temp2.dirty);
}

/*****************************************************************************/
/* Necessary set recursion through a transition                              */
/*****************************************************************************/

void transition_recurse(signalADT *signals,eventADT *events,
			ruleADT **rules,int nevents,int nplaces,
			bool **nec_cache,markingADT s,int t,
			bool *TD,int depth,int nsignals,
			int tp,bool *temp,bool *dir_temp)
{
  res_dirtyADT temp2;

  if (tp<nevents) {
    /* Place has single fanin and fanout */
    temp2=lpn_get_necessary_set(signals,events,rules,nevents,nplaces,
				nec_cache,s,tp,TD,depth+1,nsignals);
    copy_result(temp,dir_temp,temp2,nevents);
  } else {
    /* Must take union of results for all fanin's to the place */
    empty_bool_vector(temp,nevents);
    empty_bool_vector(dir_temp,nevents);
    for (int j=0;j<nevents;j++) {
      if ((lpn_valid_rule(rules[j][tp]->ruletype)) &&
	  (!lpn_valid_rule(rules[tp][j]->ruletype))) {
	temp2=lpn_get_necessary_set(signals,events,rules,nevents,nplaces,
				    nec_cache,s,j,TD,depth+1,nsignals);
	union_result(temp,dir_temp,temp2,nevents);
      }
    }
  }
}

/*****************************************************************************/
/* Recursion through a literal.                                              */
/*****************************************************************************/

void literal_recurse(signalADT *signals,eventADT *events,
		     ruleADT **rules,int nevents,int nplaces,
		     bool **nec_cache,markingADT s,int t,
		     bool *TD,int depth,int nsignals,
		     int tp,bool *temp,bool *dir_temp,int j)
{
  res_dirtyADT temp2;
  bool *TDp;

  TDp=new_bool_vector(nevents+nplaces);
  empty_bool_vector(temp,nevents);
  empty_bool_vector(dir_temp,nevents);
  int offset = (VAL(s->state[j])=='1');
  for (int k=signals[j]->event+offset;k<nevents;k+=2)
    if ((!events[k]->dropped) && (events[k]->signal==j)) {

      copy_bool_vector(TDp,TD,nevents+nplaces);
      if (events[k]->signal >= 0) {
	for (int l=signals[events[k]->signal]->event;
	     l<nevents && events[k]->signal==events[l]->signal;l++)
	  if (l != k) TDp[l]=true;
      }

      temp2=lpn_get_necessary_set(signals,events,rules,nevents,
				  nplaces,nec_cache,s,k,TDp,depth+1,
				  nsignals);
      union_result(temp,dir_temp,temp2,nevents);
    } else 
      break;
  free(TDp);
}

/*****************************************************************************/
/* Recursion through an AND.                                                 */
/*****************************************************************************/

void AND_recurse(signalADT *signals,eventADT *events,
		 ruleADT **rules,int nevents,int nplaces,
		 bool **nec_cache,markingADT s,int t,
		 bool *TD,int depth,int nsignals,
		 int tp,bool *temp,bool *dir_temp,char *product)
{
  res_dirtyADT result;
  result.res=new_bool_vector(nevents);
  result.dirty=new_bool_vector(nevents);

  for (int j=0;j<nsignals;j++) {
    if ((product[j]!='X') && (product[j]!=VAL(s->state[j]))) {
      literal_recurse(signals,events,rules,nevents,nplaces,nec_cache,s,t,TD,
		      depth,nsignals,tp,temp,dir_temp,j);
      if (update_result(result,temp,dir_temp,nevents)) break;
    }
  }
  copy_result(temp,dir_temp,result,nevents);
}

/*****************************************************************************/
/* Recursion through a simple OR.                                            */
/*****************************************************************************/

void OR_recurse(signalADT *signals,eventADT *events,
		ruleADT **rules,int nevents,int nplaces,
		bool **nec_cache,markingADT s,int t,
		bool *TD,int depth,int nsignals,
		int tp,bool *temp,bool *dir_temp)
{
  bool *temp2=new_bool_vector(nevents);
  bool *dir_temp2=new_bool_vector(nevents);

  empty_bool_vector(temp2,nevents);
  empty_bool_vector(dir_temp2,nevents);
  for (level_exp curlevel=rules[tp][t]->level;curlevel;
       curlevel=curlevel->next) {
    AND_recurse(signals,events,rules,nevents,nplaces,nec_cache,s,t,TD,
		depth,nsignals,tp,temp,dir_temp,curlevel->product);
    union_bool_vector(temp2,temp,nevents);
    union_bool_vector(dir_temp2,dir_temp,nevents);
  }
  copy_bool_vector(temp,temp2,nevents);
  copy_bool_vector(dir_temp,dir_temp2,nevents);
  free(temp2);
  free(dir_temp2);
}

/*****************************************************************************/
/* Get necessary set.                                                        */
/*****************************************************************************/

res_dirtyADT lpn_get_necessary_set(signalADT *signals,eventADT *events,
				   ruleADT **rules,int nevents,int nplaces,
				   bool **nec_cache,markingADT s,int t,
				   bool *TD,int depth,int nsignals)
{
  res_dirtyADT result;

  result.res=new_bool_vector(nevents);
  result.dirty=new_bool_vector(nevents);

  if (depth > my_depth) my_depth=depth;
#ifdef __PO_DUMP__
  printf("%d:Recursing on %s\n",depth,events[t]->event);
#endif 
  /* Found a cycle, so this cannot be enabled */
  if (TD[t]) {
#ifdef __PO_DUMP__
    printf("%d:Found cycle returning\n",depth);
#endif 
    result.dirty[t]=true;
    return result;
  }
  /* t is enabled so return necessary set including only t */
  if (s->enablings[t]=='T') { 
#ifdef __PO_DUMP__
    printf("%d:Event enabled returning %s\n",depth,events[t]->event);
#endif 
    result.res[t]=true; 
    return result; 
  }
  /* Found in cache, returning cached result */
  if (nec_cache[t]) {
#ifdef __PO_DUMP__
    printf("%d:Found in cache\n",depth);
#endif 
    copy_bool_vector(result.res,nec_cache[t],nevents);
    return result;
  }
  TD[t]=true;
  bool *temp=new_bool_vector(nevents);
  bool *dir_temp=new_bool_vector(nevents);
  /* Recurse through an unmarked or unsatisfied rule */
  for (int tp=1;tp<nevents+nplaces;tp++)
    if ((lpn_valid_rule(rules[tp][t]->ruletype)) && 
	((s->marking[rules[tp][t]->marking]=='F') ||
	 (!lpn_level_satisfied(rules,s->state,nsignals,tp,t)))) {
      if (s->marking[rules[tp][t]->marking]=='F') {
	/* Recurse through a transition */
	transition_recurse(signals,events,rules,nevents,nplaces,
			   nec_cache,s,t,TD,depth,nsignals,
			   tp,temp,dir_temp);
	if (update_result(result,temp,dir_temp,nevents)) break;
      } 
#ifdef __LEVELS__
      if (!lpn_level_satisfied(rules,s->state,nsignals,tp,t)) {
	if (rules[tp][t]->level) {
	  OR_recurse(signals,events,rules,nevents,nplaces,
		     nec_cache,s,t,TD,depth,nsignals,
		     tp,temp,dir_temp);
	  if (update_result(result,temp,dir_temp,nevents)) break;
	} else {
#ifdef __PO_DUMP__
	  printf("Level expression is false so not satisfiable\n");
#endif
	  break;
	}
      }
#endif
    }
  TD[t]=false;
  if (!intersects(TD,result.dirty,nevents)) {
    nec_cache[t]=Copy_bool_vector(result.res,nevents);
    empty_bool_vector(result.dirty,nevents);
  }
  free(temp);
  free(dir_temp);
#ifdef __PO_DUMP__
  printf("%d:Returning ",depth);
  for (int i=1;i<nevents;i++)
    if (result.res[i]) printf("%s ",events[i]->event);
  printf("\n");
#endif 
  return result;
}

void get_dependent_set(signalADT *signals,eventADT *events,ruleADT **rules,
		       markingADT marking,int nevents,int nplaces,int i,
		       char *cur_ample,bool **nec_cache,int nsignals)
{
  int j,k;
  bool *TD;
  res_dirtyADT necessary;

  //printf("Getting dependent for %s\n",events[i]->event);
  cur_ample[i]='T';
  /* Create TD variable for necessary set calculations */
  TD=new_bool_vector(nevents+nplaces);
  for (j=1;j<nevents;j++) {
    if ((i!=j) && (rules[i][j]->conflict && OCONFLICT))
      if (marking->enablings[j]=='T') {
	if (cur_ample[j]!='T') {
#ifdef __PO_DUMP__
	  printf("Adding event %s that conflicts with %s\n",events[j]->event,
		 events[i]->event);
#endif
	  get_dependent_set(signals,events,rules,marking,nevents,nplaces,j,
			    cur_ample,nec_cache,nsignals);
	}
      } else {
	/* Compute necessary set to enable this conflicting event */
#ifdef __PO_DUMP__
	printf("Checking %s with %s\n",events[i]->event,events[j]->event);
#endif
	if (events[i]->signal >= 0) {
	  for (int l=signals[events[i]->signal]->event;
	       l<nevents && events[i]->signal==events[l]->signal;l++)
	    TD[l]=true;
	} else 
	  TD[i]=true;
	necessary=lpn_get_necessary_set(signals,events,rules,nevents,nplaces,
					nec_cache,marking,j,TD,0,nsignals);
	//printf("My depth = %d\n",my_depth);
	empty_bool_vector(TD,nevents+nplaces);

	for (k=1;k<nevents;k++)
	  if (necessary.res[k]) {
	    if (cur_ample[k]!='T') {
	      //cur_ample[k]='T';
#ifdef __PO_DUMP__
	      printf("Adding event %s necessary for %s conflicting with %s\n",
		     events[k]->event,events[j]->event,events[i]->event);
#endif
	      get_dependent_set(signals,events,rules,marking,nevents,nplaces,k,
				cur_ample,nec_cache,nsignals);
	    }
	    if (marking->enablings[k]!='T') printf("Something wrong\n");
	  }
	free(necessary.res);
	free(necessary.dirty);
	//strcpy(cur_ample,marking->enablings);
      } 
  }
  free(TD);
  /*
  printf("Ample:\n");
  for (j=0;j<nevents;j++)
    if (cur_ample[j]=='T')
      printf("%s\n",events[j]->event);
  */
}

/*****************************************************************************/
/* Get an ample set for partial order method.                                */
/*****************************************************************************/

char *get_ample_set(signalADT *signals,eventADT *events,ruleADT **rules,
		    markingADT marking,int nevents,int nplaces,bool PO_reduce,
		    int nsignals)
{
  char *cur_ample;
  char *best_ample;
  int best,size,i,j;
  bool **nec_cache;

#ifdef __PO_DUMP__
  printf("Getting ample set for %s\n",marking->state);
  printf("Enabled:\n");
  for (i=1;i<nevents;i++)
    if (marking->enablings[i]=='T')
      printf("%s\n",events[i]->event);
#endif
  /* Create initial best ample set with all enabled signals */
  best_ample=CopyString(marking->enablings);
  /* Exit if not using partial order method */
  if (!PO_reduce) return best_ample;
  /* Compute size of initial ample set */
  best=0;
  for (j=1;j<nevents;j++)
    if (best_ample[j]=='T') best++;
  //enb_cnt=enb_cnt+best;
  if (best==1) {
    //amp_cnt=amp_cnt+best;
    return best_ample;
  }
  /* Create a cache for necessary set calculations */
  nec_cache=create_necessary_cache(nevents);
  /* Create cur_ample set for temporary calculations */
  cur_ample=(char *)GetBlock((nevents+1)*sizeof(char));
  cur_ample[nevents]='\0';
  /* Consider each enabled event as seed for an ample set */ 
  for (i=1;i<nevents;i++)
    if (marking->enablings[i]=='T') {
      for (j=0;j<nevents;j++)
	cur_ample[j]='F';
#ifdef __PO_DUMP__
      printf("Seed=%s\n",events[i]->event);
#endif
      get_dependent_set(signals,events,rules,marking,nevents,nplaces,i,
			cur_ample,nec_cache,nsignals);
      /* Compute size of ample set and if better, replace best */
      size=0;
      for (j=1;j<nevents;j++)
	if (cur_ample[j]=='T') size++;
#ifdef __PO_DUMP__
      printf("Cur Ample size=%d:\n",size);
      for (j=1;j<nevents;j++)
	if (cur_ample[j]=='T')
	  printf("%s\n",events[j]->event);
#endif
      if (size < best) {
	best=size;
	strcpy(best_ample,cur_ample);
      }
    }
  free(cur_ample);
  /* Destroy cache for necessary set calculations */
  destroy_necessary_cache(nec_cache,nevents);
#ifdef __PO_DUMP__
  printf("Best Ample:\n");
  for (i=1;i<nevents;i++)
    if (best_ample[i]=='T')
      printf("%s\n",events[i]->event);
#endif
  /*
  amp_cnt=amp_cnt+best;
  if ((amp_cnt*100)/enb_cnt != cur_per) {
    cur_per=(amp_cnt*100)/enb_cnt;
    printf("enabled = %d, ample = %d, percent = %d\n",enb_cnt,amp_cnt,cur_per);
  }
  */
  //if (best > 3) printf("BIG ONE\n");
  return best_ample;
}

/*****************************************************************************/
/* Print a zone to the screen for debug purposes.                            */
/*****************************************************************************/

void print_zone(eventADT *events,zoneADT zone) 
{
  int i,j;

  for (i=0;i<zone.clocksize;i++)
    printf("\t%s",events[zone.clockkey[i].enabled]->event);
  printf("\n");
  for (i=0;i<zone.clocksize;i++) { 
    printf("%s",events[zone.clockkey[i].enabled]->event);
    for (j=0;j<zone.clocksize;j++) 
      if (zone.clocks[i][j]==INFIN)
	printf("\tinf");
      else
	printf("\t%d",zone.clocks[i][j]);
    printf("\n");
  }
}

/*****************************************************************************/
/* Create a new zone of size "size".                                         */
/*****************************************************************************/

zoneADT lpn_create_zone(int size)
{
  zoneADT zone;

  zone.pastsize=size;
  zone.clocksize=size;
  zone.clockkey=(clockkeyADT)GetBlock((size)*sizeof(struct clockkey));
  for (int i=0;i<size;i++) {
    zone.clockkey[i].enabling=(-1); 
    zone.clockkey[i].enabled=(-1); 
    zone.clockkey[i].eventk_num=(-1);
    zone.clockkey[i].causal=(-1);      
    zone.clockkey[i].anti=NULL;
  } 
  zone.clocks=(clocksADT)GetBlock((size)*(sizeof(int *)));
  for (int i=0;i<size;i++) 
    zone.clocks[i]=(int *)GetBlock((size)*(sizeof(int)));

  return zone;
}

/*****************************************************************************/
/* Free memory used by a zone.                                               */
/*****************************************************************************/

void lpn_destroy_zone(zoneADT zone)
{
  free(zone.clockkey);
  for (int i=0;i<zone.clocksize;i++)
    free(zone.clocks[i]);
  free(zone.clocks);
}

/*****************************************************************************/
/* Recanonicalize a zone using Floyd's Algorithm.                            */
/*****************************************************************************/

void lpn_recanocalize(zoneADT zone) 
{
  int i,j,k;
  for (k=0;k<zone.clocksize;k++)
    for (i=0;i<zone.clocksize;i++)
      for (j=0;j<zone.clocksize;j++)
	if ((zone.clocks[i][k]!=INFIN) && (zone.clocks[k][j]!=INFIN) &&
	    (zone.clocks[i][j] > zone.clocks[i][k] + zone.clocks[k][j]))
	  zone.clocks[i][j]=zone.clocks[i][k] + zone.clocks[k][j];
}

/*****************************************************************************/
/* Find largest causal entry in the zone and set its upper bound.            */
/* This is only used for initial zone                                        */
/*****************************************************************************/
void lpn_find_largest(ruleADT **rules,int event,int nevents,int nplaces,
		      zoneADT zone,int i)
{
  int j,k;
  int maxj=(-1);
  int upper=0;
  
  for (j=0;j<zone.pastsize;j++) {
    if (lpn_causal_rule(rules[zone.clockkey[j].enabled][event]->ruletype)) {
      if (rules[zone.clockkey[j].enabled][event]->upper > upper) {
	maxj=j;
	upper=rules[zone.clockkey[j].enabled][event]->upper;
      }
    } else {
      for (k=nevents;k<nevents+nplaces;k++)
	if ((rules[zone.clockkey[j].enabled][k]->ruletype) &&
	    (lpn_causal_rule(rules[k][event]->ruletype))) {
	  if (rules[zone.clockkey[j].enabled][event]->upper > upper) {
	    maxj=j;
	    upper=rules[k][event]->upper;
	  }
	}
    }
  }
  if (maxj >= 0) {
    zone.clocks[i][maxj]=rules[zone.clockkey[maxj].enabled][event]->upper;
  }
}

/*****************************************************************************/
/* Find causal entry in the zone and set its upper bound.                    */
/* BIG ASSUMPTION: LATEST EVENT IS CAUSAL                                    */
/*****************************************************************************/
void lpn_find_causal(eventADT *events,ruleADT **rules,int event,int nevents,
		     int nplaces,zoneADT zone,int i,char *state,int nsignals)
{
  int j,k;
  char *new_state;
  
  new_state=CopyString(state);
  for (j=(zone.pastsize-1);j>=0;j--) {
    if (lpn_causal_rule(rules[zone.clockkey[j].enabled][event]->ruletype)) {
      zone.clocks[i][j]=rules[zone.clockkey[j].enabled][event]->upper;
      free(new_state);
      return;
    } else {
      for (k=nevents;k<nevents+nplaces;k++)
	if ((rules[zone.clockkey[j].enabled][k]->ruletype) &&
	    (lpn_causal_rule(rules[k][event]->ruletype))) {
	  zone.clocks[i][j]=rules[k][event]->upper;
	  zone.clocks[j][i]=(-1)*rules[k][event]->lower;
	  free(new_state);
	  return;
	}
    }
    if (events[zone.clockkey[j].enabled]->signal >= 0) {
      if ((new_state[events[zone.clockkey[j].enabled]->signal]=='1')||
	  (new_state[events[zone.clockkey[j].enabled]->signal]=='F'))
	new_state[events[zone.clockkey[j].enabled]->signal]='0';
      else 
	new_state[events[zone.clockkey[j].enabled]->signal]='1';
      for (k=1;k<nevents+nplaces;k++)
	if ((rules[k][event]->ruletype) && (rules[k][event]->expr) &&
	    !(lpn_level_satisfied(rules,new_state,nsignals,k,event))) {
	  zone.clocks[i][j]=rules[k][event]->upper;
	  zone.clocks[j][i]=(-1)*rules[k][event]->lower;
	  free(new_state);
	  return;
	} 	
    }
  }
  free(new_state);
}

/*****************************************************************************/
/* Find causal entry in the zone and set its upper bound.                    */
/* BIG ASSUMPTION: LATEST EVENT IS CAUSAL                                    */
/*****************************************************************************/
bool lpn_is_causal(eventADT *events,ruleADT **rules,int nevents,
		   int nplaces,zoneADT zone,char *state,int nsignals,
		   int event,int i,char *marking)
{
  int j,k;
  char *new_state;
  
  new_state=CopyString(state);
  for (j=(zone.pastsize-1);j>i;j--) {
    if (lpn_causal_rule(rules[zone.clockkey[j].enabled][event]->ruletype)) {
      free(new_state);
      return false;
    } else {
      for (k=nevents;k<nevents+nplaces;k++)
	if ((rules[zone.clockkey[j].enabled][k]->ruletype) &&
	    (lpn_causal_rule(rules[k][event]->ruletype))) {
	  free(new_state);
	  return false;
	}
    }
    if (events[zone.clockkey[j].enabled]->signal >= 0) {
      if ((new_state[events[zone.clockkey[j].enabled]->signal]=='1')||
	  (new_state[events[zone.clockkey[j].enabled]->signal]=='F'))
	new_state[events[zone.clockkey[j].enabled]->signal]='0';
      else 
	new_state[events[zone.clockkey[j].enabled]->signal]='1';
      for (k=1;k<nevents+nplaces;k++)
	if ((lpn_causal_rule(rules[k][event]->ruletype)) && 
	    (rules[k][event]->expr) &&
	    !(lpn_level_satisfied(rules,new_state,nsignals,k,event))) {
	  free(new_state);
	  return false;
	} 	
    }
  }

  if ((lpn_causal_rule(rules[zone.clockkey[j].enabled][event]->ruletype)) &&
      (marking[rules[zone.clockkey[j].enabled][event]->marking]=='T')) {
    free(new_state);
    return true;
  } else {
    for (k=nevents;k<nevents+nplaces;k++)
      if ((rules[zone.clockkey[j].enabled][k]->ruletype) &&
	  lpn_causal_rule(rules[k][event]->ruletype) &&
	  (marking[rules[k][event]->marking]=='T')) {
	free(new_state);
	return true;
      }
  }
  if (events[zone.clockkey[j].enabled]->signal >= 0) {
    if ((new_state[events[zone.clockkey[j].enabled]->signal]=='1')||
	(new_state[events[zone.clockkey[j].enabled]->signal]=='F'))
      new_state[events[zone.clockkey[j].enabled]->signal]='0';
    else 
      new_state[events[zone.clockkey[j].enabled]->signal]='1';
    for (k=1;k<nevents+nplaces;k++)
      if (lpn_causal_rule(rules[k][event]->ruletype) && 
	  (rules[k][event]->expr) &&
	  !(lpn_level_satisfied(rules,new_state,nsignals,k,event))) {
	free(new_state);
	return true;
      } 	
  }

  return false;
}

/*****************************************************************************/
/* Project past variables from zone that are no longer needed.               */
/*****************************************************************************/

zoneADT lpn_project_zone(eventADT *events,ruleADT **rules,int nevents,
			 int nplaces,markingADT new_marking,zoneADT zone,
			 int nsignals)
{
  zoneADT new_zone;
  int i,j,l,m;
  int size=0;

  /* Determine new zone size */
  for (i=(zone.pastsize-1);i>=0;i--) {
    for (j=1;j<nevents;j++)
      if ((new_marking->enablings[j]=='T') &&
	  (lpn_is_causal(events,rules,nevents,nplaces,zone,
			 new_marking->state,nsignals,j,i,
			 new_marking->marking))) {
	size++;
	break;
      }
    if (j==nevents)
      zone.clockkey[i].enabling=(-2);
    else 
      zone.clockkey[i].enabling=(-1);
  }
	/*      
      if ((rules[zone.clockkey[i].enabled][j]->ruletype) && 
	  (new_marking->marking[rules[zone.clockkey[i].enabled][j]->marking]
	   =='T') && (new_marking->enablings[j]=='T')) {
	bool last=true;
	for (l=i+1;l<zone.pastsize;l++)
	  if (rules[zone.clockkey[l].enabled][j]->ruletype) last=false;
	  else
	    for (m=nevents;m<nevents+nplaces;m++) 
	      if ((rules[l][m]->ruletype) && (rules[m][j]->ruletype) && 
		  (new_marking->marking[rules[m][j]->marking]=='T') && 
		  (new_marking->enablings[j]=='T')) last=false;
	if (last) {
	  size++;
	  break;
	}
      }
    if (j==nevents) {
      for (j=nevents;j<nevents+nplaces;j++)
	if (rules[zone.clockkey[i].enabled][j]->ruletype) {
	  for (k=1;k<nevents;k++)
	    if ((rules[j][k]->ruletype) && 
		(new_marking->marking[rules[j][k]->marking]=='T') && 
		(new_marking->enablings[k]=='T')) {
	      bool last=true;
	      for (l=i+1;l<zone.pastsize;l++)
		if (rules[zone.clockkey[l].enabled][j]->ruletype) last=false;
		else
		  for (m=nevents;m<nevents+nplaces;m++) 
		    if ((rules[l][m]->ruletype) && (rules[m][j]->ruletype) && 
			(new_marking->marking[rules[m][j]->marking]=='T') && 
			(new_marking->enablings[j]=='T')) last=false;
	      if (last) {
		size++;
		break;
	      }
	    }
	  if (k != nevents) break;
	}
      if (j==(nevents+nplaces)) 
	zone.clockkey[i].enabling=(-2);
      else 
	zone.clockkey[i].enabling=(-1);
    } else 
      zone.clockkey[i].enabling=(-1);
      } */
  new_zone=lpn_create_zone(size+1);
  new_zone.pastsize--;
  new_zone.clocksize--;

  l=0;
  for (i=0;i<zone.clocksize;i++) 
    if (zone.clockkey[i].enabling==(-1)) {
      new_zone.clockkey[l].enabled=zone.clockkey[i].enabled;
      l++;
    }
  l=0;
  m=0;
  for (i=0;i<zone.clocksize;i++) { 
    if (zone.clockkey[i].enabling==(-1)) {
      m=0;
      for (j=0;j<zone.clocksize;j++) { 
	if (zone.clockkey[j].enabling==(-1)) {
	  new_zone.clocks[l][m]=zone.clocks[i][j];
	  m++;
	}
      }
      l++;
    }
  }
  return new_zone;
}

/*****************************************************************************/
/* Extend zone with fired event.                                             */
/*****************************************************************************/

void lpn_extend_zone(eventADT *events,ruleADT **rules,int event,int nevents,
		     int nplaces,zoneADT zone,char *state,int nsignals,
		     bool initial)
{
  int i,j,k;
  
  for (i=0;i<zone.pastsize;i++) {
    zone.clocks[i][zone.pastsize]=0;
    zone.clocks[zone.pastsize][i]=INFIN;
  }
  zone.clocks[zone.pastsize][zone.pastsize]=0;
  for (j=0;j<zone.pastsize;j++) 
    if (lpn_causal_rule(rules[zone.clockkey[j].enabled][event]->ruletype)) {
      zone.clocks[j][zone.pastsize]=
	(-1)*rules[zone.clockkey[j].enabled][event]->lower;
    } else {
      for (k=nevents;k<nevents+nplaces;k++)
	if ((rules[zone.clockkey[j].enabled][k]->ruletype) &&
	    lpn_causal_rule(rules[k][event]->ruletype)) {
	  zone.clocks[j][zone.pastsize]=(-1)*rules[k][event]->lower;
	}
    }
  if (initial)
    lpn_find_largest(rules,event,nevents,nplaces,zone,zone.pastsize);
  else
    lpn_find_causal(events,rules,event,nevents,nplaces,zone,zone.pastsize,
		    state,nsignals);
  zone.clockkey[zone.pastsize].enabled=event;
}

/*****************************************************************************/
/* Find the initial zone.                                                    */
/*****************************************************************************/

zoneADT lpn_find_new_zone(eventADT *events,ruleADT **rules,
			  markkeyADT *markkey,markingADT marking,
			  markingADT new_marking,int event,
			  int nevents,int nplaces,int nrules,zoneADT zone,
			  int nsignals,bool initial)
{
  zoneADT new_zone;

  lpn_extend_zone(events,rules,event,nevents,nplaces,zone,new_marking->state,
		  nsignals,initial);
  zone.pastsize++;
  zone.clocksize++;
  lpn_recanocalize(zone); 
#ifdef __DUMP__OFF
  printf("Before project\n");
  printf("%s\n",new_marking->state);
  print_zone(events,zone);
#endif
  new_zone=lpn_project_zone(events,rules,nevents,nplaces,new_marking,zone,
			    nsignals);
#ifdef __DUMP__OFF
  printf("After project\n");
  printf("%s\n",new_marking->state);
  print_zone(events,new_zone);
#endif
  /* 1) Make fired event a past variable */
  /* 2) Make fired event fire before all enabled events */
  /* 3) Remove disabled events */
  /* 4) Add newly enabled events */
  /* 5) Recon? */
  /* 6) Remove fired event */
  return new_zone;
}

/*****************************************************************************/
/* Prunes all past variables.                                                */
/* BIG ASSUMPTION: None can be causal or are needed for timing reasons.      */
/*****************************************************************************/

zoneADT lpn_prune_past(zoneADT zone)
{
  zoneADT new_zone;
  int i,j;

  new_zone.pastsize=0;
  new_zone.clocksize=zone.clocksize-zone.pastsize;
  new_zone.clockkey=(clockkeyADT)
    GetBlock(new_zone.clocksize*sizeof(struct clockkey));
  for (i=0;i<new_zone.clocksize;i++) {
    new_zone.clockkey[i].enabling=(-1);   
    new_zone.clockkey[i].enabled=zone.clockkey[i+zone.pastsize].enabled;
    new_zone.clockkey[i].eventk_num=(-1);
    new_zone.clockkey[i].causal=(-1);      
    new_zone.clockkey[i].anti=NULL;
  }
  new_zone.clocks=(clocksADT)GetBlock(new_zone.clocksize*(sizeof(int *)));
  for (i=0;i<new_zone.clocksize;i++) {
    new_zone.clocks[i]=(int *)GetBlock(new_zone.clocksize*(sizeof(int)));
    for (j=0;j<new_zone.clocksize;j++) 
      new_zone.clocks[i][j]=zone.clocks[i+zone.pastsize][j+zone.pastsize];
  }
  free(zone.clockkey);
  for (i=0;i<zone.clocksize;i++)
    free(zone.clocks[i]);
  free(zone.clocks);
  return new_zone;
}

/*****************************************************************************/
/* Find the initial zone.                                                    */
/*****************************************************************************/

zoneADT lpn_find_initial_zone(eventADT *events,ruleADT **rules,
			      markkeyADT *markkey,int nevents,int nplaces,
			      int nrules,markingADT marking)
{
  zoneADT zone;
  int i,j,k,l;

  /* Determine zone size */
  int size=0;
  for (i=1;i<nevents;i++) {
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype) && 
	  (marking->marking[rules[i][j]->marking]=='T')) {
	size++;
	break;
      }
    if (j==nevents) {
      for (j=nevents;j<nevents+nplaces;j++)
	if (rules[i][j]->ruletype) {
	  for (k=1;k<nevents;k++)
	    if ((rules[j][k]->ruletype) && 
		(marking->marking[rules[j][k]->marking]=='T')) {
	      size++;
	      break;
	    }
	  if (k != nevents) break;
	}
    }
    //if (marking->enablings[i]=='T') zone.clocksize++;
  }

  /* Build initial clock key */
  zone=lpn_create_zone(size+1);
  zone.clocksize--;
  zone.pastsize--;
  l=0;
  for (i=1;i<nevents;i++) {
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype) && 
	  (marking->marking[rules[i][j]->marking]=='T')) {
	zone.clockkey[l].enabled=i;
	l++;
	break;
      }
    if (j==nevents) {
      for (j=nevents;j<nevents+nplaces;j++)
	if (rules[i][j]->ruletype) {
	  for (k=1;k<nevents;k++)
	    if ((rules[j][k]->ruletype) && 
		(marking->marking[rules[j][k]->marking]=='T')) {
	      zone.clockkey[l].enabled=i;
	      l++;
	      break;
	    }
	  if (k != nevents) break;
	}
    }
  }

  for(i=0;i<=zone.clocksize;i++) {
    /* Clear first part of clocks matrix */
    if (i < zone.pastsize) {
      for (j=0;j<zone.pastsize;j++) {
	zone.clocks[i][j]=0;
	zone.clocks[j][i]=0;
      }
      for (j=zone.pastsize;j<zone.clocksize;j++) {
	zone.clocks[i][j]=INFIN;
	zone.clocks[j][i]=INFIN;
      }
    } else {
      for (j=0;j<zone.clocksize;j++) {
	if (i==j) {
	  zone.clocks[i][j]=0;
	  zone.clocks[j][i]=0;
	} else {
	  zone.clocks[i][j]=INFIN;
	  zone.clocks[j][i]=INFIN;
	}
      }
    }
  }  
#ifdef __DUMP__OFF
  printf("Initial:\n");
  print_zone(events,zone);
#endif
  return zone;
}

/*****************************************************************************/
/* Check if a zone is consistent or not (i.e., no -ve diagonals).            */
/*****************************************************************************/

bool lpn_zone_consistent(zoneADT zone)
{
  for (int i=0;i<zone.clocksize;i++)
    if (zone.clocks[i][i] != 0) return false;
  return true;
}

/*****************************************************************************/
/* Copy first "size" clocks from zone2 into zone1.                           */
/*****************************************************************************/

void lpn_copy_clocks(zoneADT zone1,zoneADT zone2,int size) 
{
  for (int i=0;i<size;i++) {
    zone1.clockkey[i].enabled=zone2.clockkey[i].enabled;
    for (int j=0;j<size;j++) 
      zone1.clocks[i][j]=zone2.clocks[i][j];
  }
}

/*****************************************************************************/
/* Find the set of fireable events (i.e., can fire first in time).           */
/*****************************************************************************/

char *lpn_get_fireable(eventADT *events,ruleADT **rules,int nevents,
		       int nplaces,markingADT marking,zoneADT zone,
		       int nsignals,bool initial)
{
  char *fireable;
  int event_cnt=0;
  int entry;
  zoneADT new_zone;

  fireable=(char *)GetBlock((nevents+1)*sizeof(char));
  for (int i=1;i<nevents;i++) {
    fireable[i]='F';
    if (marking->enablings[i]=='T') event_cnt++;
  }
  fireable[nevents]='\0';
  new_zone=lpn_create_zone(zone.clocksize+event_cnt);
  new_zone.pastsize=zone.clocksize;
  lpn_copy_clocks(new_zone,zone,zone.clocksize);

  entry=new_zone.pastsize;
  for (int i=1;i<nevents;i++) 
    if (marking->enablings[i]=='T') {
      new_zone.clockkey[entry].enabled=i;
      entry++;
    }
  for (int i=0;i<new_zone.clocksize;i++)
    for (int j=0;j<new_zone.clocksize;j++)
      if ((i >= new_zone.pastsize) || (j >= new_zone.pastsize)) {
	if (i==j) new_zone.clocks[i][j]=0;
	else new_zone.clocks[i][j]=INFIN;
      }

  for (int i=new_zone.pastsize;i<new_zone.clocksize;i++) {
    for (int j=0;j<new_zone.pastsize;j++) 
      if (lpn_causal_rule(rules[new_zone.clockkey[j].enabled]
			  [new_zone.clockkey[i].enabled]->ruletype)) {
	new_zone.clocks[j][i]=(-1)*
	  rules[new_zone.clockkey[j].enabled]
	  [new_zone.clockkey[i].enabled]->lower;
      } else {
	for (int k=nevents;k<nevents+nplaces;k++)
	  if ((rules[new_zone.clockkey[j].enabled][k]->ruletype) &&
	      lpn_causal_rule(rules[k][new_zone.clockkey[i].enabled]
			      ->ruletype)) {
	    new_zone.clocks[j][i]=
	      (-1)*rules[k][new_zone.clockkey[i].enabled]->lower;
	  }
      }
    /* BIG ASSUMPTION: LATEST EVENT IS CAUSAL */
    if (initial)
      lpn_find_largest(rules,new_zone.clockkey[i].enabled,nevents,
		       nplaces,new_zone,i);
    else
      lpn_find_causal(events,rules,new_zone.clockkey[i].enabled,nevents,
		      nplaces,new_zone,i,marking->state,nsignals);
  }
#ifdef __DUMP__OFF
  printf("Before canocalizing:\n");
  print_zone(events,new_zone);
#endif
  lpn_recanocalize(new_zone); 
  for (int i=new_zone.pastsize;i<new_zone.clocksize;i++) {
    bool can_fire_first=true;
    for (int j=new_zone.pastsize;j<new_zone.clocksize;j++) 
      if (new_zone.clocks[j][i] < 0) {
	can_fire_first=false;
	break;
      }
    if (can_fire_first) fireable[new_zone.clockkey[i].enabled]='T';
  }
#ifdef __DUMP__OFF
  printf("Computing fireable from:\n");
  print_zone(events,new_zone);
  printf("Fireable ");
  for (int i=0;i<nevents;i++)
    if (fireable[i]=='T') printf("%s ",events[i]->event);
  printf("\n");
#endif
  return fireable;
}

/*****************************************************************************/
/* Find reachable states for a given LPN.                                    */
/*****************************************************************************/

bool lpn_rsg(char * filename,signalADT *signals,eventADT *events,
	     ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
	     int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
	     char * startstate,bool si,bool verbose,bddADT bdd_state,
	     bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
	     int nord,bool noparg,bool search,bool use_dot,bool bap)
{
  FILE *fp=NULL;
  markingADT marking;
  markingADT new_marking;
  lpn_stackADT lpn_stack=NULL;
  int i,sn;
  bool stuck;
  char * firelist;
  char * new_firelist;
  timeval t0,t1;
  double time0, time1;
  int iter=0;
  int stack_depth=0;
  int max_depth=0;
  zoneADT zone,new_zone;
  int new_state;
  int regions=0;
  bool initial;

  zone.pastsize=0;
  zone.clocksize=0;
  zone.clockkey=NULL;
  zone.clocks=NULL;
  new_zone.pastsize=0;
  new_zone.clocksize=0;
  new_zone.clockkey=NULL;
  new_zone.clocks=NULL;

  /* Start a timer */
  gettimeofday(&t0, NULL);
  /* Display message and open file if verbose */
  fp=lpn_print_msg(filename,signals,nsignals,ninpsig,verbose);
  if (verbose && !fp) return false;
  /* Clear the state table */
  initialize_state_table(LOADED,FALSE,state_table);
  /* Find the initial marking */
  marking=lpn_find_initial_marking(events,rules,markkey,nevents,nplaces,nrules,
				   ninpsig,nsignals,startstate,verbose,
				   timeopts.nofail);
  /* Find the initial zone */
  if (bap)
    zone=lpn_find_initial_zone(events,rules,markkey,nevents,nplaces,nrules,
			       marking);

  /* If no initial marking, there is problem, so bail out */
  if (marking == NULL) {
    if (verbose) fclose(fp);
    return(FALSE);
  }
  /* Push initial marking onto the stack */ 
  /* SHOULD NOT NEED TO PUSH ENABLINGS, REDUNDANT */
  if (bap)
    firelist=lpn_get_fireable(events,rules,nevents,nplaces,marking,zone,
			      nsignals,true);
  else 
    firelist=get_ample_set(signals,events,rules,marking,nevents,nplaces,
			   timeopts.PO_reduce,nsignals);

  lpn_push_marking(&lpn_stack,marking,firelist,zone);

  stack_depth++;
  if (stack_depth > max_depth) max_depth=stack_depth;
  /* Add the initial state to the state table */
#ifdef __DUMP__
  printf("Initial state = %s\n",marking->state);
  printf("Marking = ");
  for (int i=0;i<nrules;i++)
    if (marking->marking[i]=='T')
      printf("(%s,%s)",events[markkey[i]->enabling]->event,
	     events[markkey[i]->enabled]->event);
  printf("\n");
  printf("Enabled  = ");
  for (int i=0;i<nevents;i++)
    if (marking->enablings[i]=='T')
      printf("%s ",events[i]->event);
  printf("\n");
  printf("Firelist = ");
  for (int i=0;i<nevents;i++)
    if (firelist[i]=='T')
      printf("%s ",events[i]->event);
  printf("\n");
#endif
  add_state(fp,state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
	    marking->state,marking->marking,
	    marking->enablings,nsignals, zone.clocks, zone.clockkey, 
	    zone.clocksize, 0,verbose, bap, nrules,
	    zone.clocksize,zone.clocksize,bdd_state,use_bdd,markkey,timeopts,
	    -1, -1, NULL, rules,nevents+nplaces,bap);
  sn=1;
  regions=1;
  initial=true;

  /* Repeat while stack is not empty */
  while ((marking=lpn_pop_marking(&lpn_stack,&firelist,&zone)) != NULL) {
    stack_depth--;
    stuck=TRUE;

    /* Fire each fireable event in turn */
    for (i=1;i<nevents;i++) 
      if (firelist[i]=='T') {
#ifdef __DUMP__
	printf("=====================\n");
	printf("Firing %s\n",events[i]->event);
#endif
	stuck=FALSE;  /* There exists a fireable event so no deadlock */
	iter++;
	/* Find new marking after firing event i */
	new_marking=lpn_find_new_marking(signals,events,rules,markkey,marking,
					 i,nevents,nplaces,nrules,verbose, 
					 nsignals,ninpsig,
					 timeopts.disabling||timeopts.nofail,
					 noparg,timeopts.nofail,use_dot); 
#ifdef __DUMP__
	if (new_marking) {
	  printf("State = %s\n",new_marking->state);
	  /*
	  printf("Marking = ");
	  for (int l=0;l<nrules;l++)
	    if (new_marking->marking[l]=='T')
	      printf("(%s,%s)",events[markkey[l]->enabling]->event,
		     events[markkey[l]->enabled]->event);
	  printf("\n");
	  printf("Enabled  = ");
	  for (int l=0;l<nevents;l++)
	    if (new_marking->enablings[l]=='T')
	      printf("%s ",events[l]->event);
	      printf("\n");*/
	}
#endif
	/* Find new zone */
	if (bap && new_marking) {
	  new_zone=lpn_find_new_zone(events,rules,markkey,marking,new_marking,
				     i,nevents,nplaces,nrules,zone,nsignals,
				     initial);
	  if (!lpn_zone_consistent(new_zone)) {
	    printf("zone not consistent, continuing\n");
	    continue;
	  }
	}

	/* No new marking found, so something went wrong, bail out */
	if (!new_marking) {
	  new_state=add_state(fp,state_table,marking->state,marking->marking,
			      marking->enablings,NULL,NULL,0,NULL,NULL,NULL,
			      NULL,nsignals,NULL,NULL,0,sn,verbose,FALSE,
			      nrules,0,0,bdd_state,use_bdd,markkey,timeopts,-1,
			      i,NULL,rules,nevents);
	  if (new_state) {
	    sn++;
	    regions++;
	  }
	  if ((!timeopts.nofail)&&(!timeopts.keepgoing)) return false;
	}
	/* Check if new marking is a new state */

	/* This is only here for bio stuff */
	events[i]->color++;
	if ((spor >= 0) && (comp >= 0)) {
	  if ((events[i]->signal==spor) && (new_marking->state[comp]=='1'))
	    comp_spor++;
	  if ((events[i]->signal==comp) && (new_marking->state[spor]=='1'))
	    spor_comp++;
	}
	
	if (new_marking) {
	  new_state=add_state(fp,state_table,marking->state,marking->marking,
			      marking->enablings, zone.clocks, zone.clockkey, 
			      zone.clocksize, NULL, new_marking->state,
			      new_marking->marking,new_marking->enablings,
			      nsignals, new_zone.clocks, new_zone.clockkey, 
			      new_zone.clocksize, sn,verbose, bap, nrules,
			      new_zone.clocksize,new_zone.clocksize,
			      bdd_state,use_bdd,markkey,timeopts, -1, i, NULL, 
			      rules, nevents+nplaces, bap);
	} else
	  new_state=0;
	
	if (new_state != 0) {

	  if (new_state > 0)
	    regions++;
	  else 
	    regions=regions+new_state+1;
	  if (new_state==1) {
#ifdef __DUMP__
	    printf("New state\n");
#endif
	    sn++;
	  } else {
#ifdef __DUMP__
	    printf("New zone\n");
#endif
	  }
	  /* Display state counts and memory usage */
	  lpn_display_info(iter,sn,regions,stack_depth,bap);
	  /* Find new fireable event list */
	  /* Push new marking and fireable list to the stack */
	  if (bap)
	    new_firelist=lpn_get_fireable(events,rules,nevents,nplaces,
					  new_marking,new_zone,nsignals,
					  false);
	  else 
	    new_firelist=get_ample_set(signals,events,rules,new_marking,
				       nevents,nplaces,timeopts.PO_reduce,
				       nsignals);
	  
	  lpn_push_marking(&lpn_stack,new_marking,new_firelist,new_zone);

	  stack_depth++;
	  if (stack_depth > max_depth) max_depth=stack_depth;
	} else {
	  /* New marking is not a new state, so clean up and continue */
	  if (new_marking) {
	    delete_marking(new_marking);
	    free(new_marking);
	  }
	}
      } 
    initial=false;
    /* No events fireable from this marking, so deadlock has been found */
    if (stuck) {
      if (lpn_found_deadlock(events,rules,markkey,marking,nevents,nplaces,
			     nrules,verbose,timeopts.nofail,search,nsignals)) {
	add_state(fp,state_table,marking->state,marking->marking,
		  marking->enablings,NULL,NULL,0,NULL,NULL,NULL,NULL,
		  nsignals,NULL,NULL,0,sn,verbose,FALSE,nrules,0,0,
		  bdd_state,use_bdd,markkey,timeopts,-1,nevents,NULL,rules,
		  nevents);
	if (verbose) fclose(fp);
	return FALSE;
      }
    }
    /* Cleanup memory before looping back */
    delete_marking(marking);
    if (bap) lpn_destroy_zone(zone);
    free(marking);
    free(firelist);
  }
  /* Display runtime and exit */
  gettimeofday(&t1,NULL);	
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));

  if (verbose) {
    if (bap) {
      printf("%d zones and %d states explored in %g seconds\n",regions,sn,
	     time1-time0);
      fprintf(lg,"%d zones and %d states explored in %g seconds\n",regions,
	      sn,time1-time0);
    } else {
      printf("%d states explored in %g seconds\n",sn,time1-time0);
      fprintf(lg,"%d states explored in %g seconds\n",sn,time1-time0);
    }
    fclose(fp);
  } else {
    if (bap) {
      printf("done (zones=%d,states=%d,time=%f)\n",regions,sn,time1-time0);
      fprintf(lg,"done (zones=%d,states=%d,time=%f)\n",regions,sn,time1-time0);
#ifdef MEMSTATS
#ifndef OSX
      struct mallinfo memuse;
      memuse=mallinfo();
      printf("Found %d zones in %d states for %s (%dd %dm %fu)\n",
	     regions,sn,filename,max_depth,memuse.arena,time1-time0);
      fprintf(lg,"Found %d zones in %d states for %s (%dd %dm %fu)\n",
	      regions,sn,filename,max_depth,memuse.arena,time1-time0);
#else
	malloc_statistics_t t;
	malloc_zone_statistics(NULL, &t);
	printf("memory: max=%d inuse=%d allocated=%d\n",
	       t.max_size_in_use,t.size_in_use,t.size_allocated);
	fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
    } else {
      printf("done (states=%d,time=%f)\n",sn,time1-time0);
      fprintf(lg,"done (states=%d,time=%f)\n",sn,time1-time0);
    }
  }
  return TRUE;
}

int gcd(int a,int b) 
{
  if (b==0) return a;
  else return(gcd(b,a % b));
}

/*****************************************************************************/
/* Consider all possible initial input states and count event frequencies.   */
/*****************************************************************************/

bool bio_rsg(char * filename,signalADT *signals,eventADT *events,
	     ruleADT **rules,stateADT *state_table,markkeyADT *markkey, 
	     int nevents,int nplaces,int nsignals,int ninpsig,int nrules,
	     char * startstate,bool si,bool verbose,bddADT bdd_state,
	     bool use_bdd,timeoptsADT timeopts,int ndummy,int ncausal, 
	     int nord,bool noparg,bool search,bool use_dot,bool bap,
	     double tolerance)
{
  int i;

  for (i=0;i<nevents;i++)
    if (strcmp(events[i]->event,"sporulation+/1")==0)
      spor=events[i]->signal;
    else if (strcmp(events[i]->event,"competence+/1")==0)
      comp=events[i]->signal;

  for (int x=0;x<(1 << ninpsig);x++) {
    int y=x;
    int z=0;
    while (y > 0) {
      if (y % 2==0) startstate[z]='0';
      else startstate[z]='1';
      y = y >> 1;
      z++;
    }
    spor_comp=0;
    comp_spor=0;
    for (i=0;i<nevents;i++)
      events[i]->color=0;
    //printf("%d:=====================================================\n",x);
    //fprintf(lg,"%d:=====================================================\n",x);
    /*
    for (i=0;i<ninpsig;i++)
      if (startstate[i]=='1') {
	printf("%s ",signals[i]->name);
	fprintf(lg,"%s ",signals[i]->name);
      }
    printf("\n");
    fprintf(lg,"\n");
    */
    lpn_rsg(filename,signals,events,rules,state_table,markkey,nevents,nplaces,
	    nsignals,ninpsig,nrules,startstate,si,verbose,bdd_state,use_bdd,
	    timeopts,ndummy,ncausal,nord,noparg,search,use_dot,bap);

    /* NEW EVENT COUNTER */
    /*
    stateADT curstate;
    statelistADT nextstate;
    bool updated; 

    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link)
	if (curstate->number==0) curstate->color=INFIN /2;
	else curstate->color=0;
    do {
      updated=false;
      for (i=0;i<PRIME;i++)
	for (curstate=state_table[i];
	     curstate != NULL && curstate->state != NULL;
	     curstate=curstate->link)
	  if (curstate->color > 0) {
	    updated=true;
	    double cnt=0.0;
	    for (nextstate=curstate->succ;nextstate;
		 nextstate=nextstate->next) 
	      if (nextstate->stateptr!=curstate) {
		int k;
		for (k=1;k<nevents+nplaces;k++) {
		  if (rules[k][nextstate->enabled]->ruletype) {
		    if (rules[k][nextstate->enabled]->rate > 0.0) {
		      cnt=cnt+rules[k][nextstate->enabled]->rate;
		      break;
		    }
		  }
		}
		if (k==(nevents+nplaces)) cnt=cnt+1.0;
	      }
	    if (cnt > 0.0) {
	      for (nextstate=curstate->succ;nextstate;
		   nextstate=nextstate->next) {
		int k;
		for (k=1;k<nevents+nplaces;k++)
		  if (rules[k][nextstate->enabled]->ruletype) {
		    if (rules[k][nextstate->enabled]->rate > 0.0) { 
		      nextstate->stateptr->color+=int(1.0*curstate->color * 
			(rules[k][nextstate->enabled]->rate / cnt));
		      events[nextstate->enabled]->color+=int(curstate->color*
			(rules[k][nextstate->enabled]->rate / cnt));
		      printf("%d:%s:%d -%s:%d-> %d:%s:%d\n",
			     curstate->number,curstate->state,curstate->color,
			     events[nextstate->enabled]->event,
			     events[nextstate->enabled]->color,
			     nextstate->stateptr->number,
			     nextstate->stateptr->state,
			     nextstate->stateptr->color);
		      break;
		    } 
		  }
		if (k==(nevents+nplaces)) {
		  nextstate->stateptr->color+=int(curstate->color * 
						 (1.0 / cnt));
		  events[nextstate->enabled]->color+=int(curstate->color*
							 (1.0 / cnt));
		  printf("%d:%s:%d -%s:%d-> %d:%s:%d\n",
			 curstate->number,curstate->state,curstate->color,
			 events[nextstate->enabled]->event,
			 events[nextstate->enabled]->color,
			 nextstate->stateptr->number,
			 nextstate->stateptr->state,
			 nextstate->stateptr->color);
		}
	      }
	    }
	    curstate->color=0;
	  }
    } while (updated);
    */

    /* Output event counts */
//     printf("EVENT COUNTS\n");
//     fprintf(lg,"EVENT COUNTS\n");
//     for (i=0;i<nevents;i++) {
//       printf("%s = %d\n",events[i]->event,events[i]->color);
//       fprintf(lg,"%s = %d\n",events[i]->event,events[i]->color);
//     }
//     if (spor_comp > 0) {
//       printf("Spor -> Comp = %d\n",spor_comp);
//       fprintf(lg,"Spor -> Comp = %d\n",spor_comp);
//     }
//     if (comp_spor > 0) {
//       printf("Comp -> Spor = %d\n",comp_spor);
//       fprintf(lg,"Comp -> Spor = %d\n",comp_spor);
//     } 

    /* Markov Analysis */
    stateADT curstate;
    statelistADT nextstate;
    bool updated; 
    int k;
    double last;
    int state_cnt=0;

    /* Find periodicity */
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {

	if (curstate->number > state_cnt) state_cnt=curstate->number;

	if (curstate->number==0) curstate->color=1;
	else curstate->color=0;
      }
    bool empty;
    k=1;
    int period=0;
    do {
      empty=true;
      for (i=0;i<PRIME;i++)
	for (curstate=state_table[i];
	     curstate != NULL && curstate->state != NULL;
	     curstate=curstate->link) {
	  if (curstate->color==k) 
	    if (curstate->succ) {
	      for (nextstate=curstate->succ;nextstate;
		   nextstate=nextstate->next) {
		if (nextstate->stateptr->color==0) {
		  nextstate->stateptr->color=k+1;
		  empty=false;
		} else if (nextstate->stateptr->color != k+1) {
		  if (period!=0)
		    period=gcd(period,k-nextstate->stateptr->color+1);
		  else
		    period=k-nextstate->stateptr->color+1;
		}
	      }
	    } else {
	      period=1;
	    }
	}
      k++;
    } while (!empty);
    //printf("Period=%d\n",period);
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	curstate->color=(curstate->color-1) % period;
      }

    /* Determine transition probabilities */
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {

	//	curstate->probability=1.0 / (state_cnt+1);
	if (curstate->number==0) curstate->probability=1.0;
	else curstate->probability=0.0;

	double cnt=0.0;
	for (nextstate=curstate->succ;nextstate;
	     nextstate=nextstate->next) {
	  for (k=1;k<nevents+nplaces;k++) {
	    if (rules[k][nextstate->enabled]->ruletype) {
	      /*
	      printf("%s -> %s %g\n",events[k]->event,
		     events[nextstate->enabled]->event,
		     rules[k][nextstate->enabled]->rate);*/
	      cnt=cnt+rules[k][nextstate->enabled]->rate;
	      break;
	    }
	  }
	  if (k==(nevents+nplaces)) cnt=cnt+1.0;
	}
	if (cnt > 0.0) {
	  for (nextstate=curstate->succ;nextstate;
	       nextstate=nextstate->next) {
	    int k;
	    for (k=1;k<nevents+nplaces;k++)
	      if (rules[k][nextstate->enabled]->ruletype) {
		nextstate->probability=rules[k][nextstate->enabled]->rate/cnt;
		/*
		printf("%d:%s -%s:%g-> %d:%s\n",
		       curstate->number,curstate->state,
		       events[nextstate->enabled]->event,
		       nextstate->probability,
		       nextstate->stateptr->number,
		       nextstate->stateptr->state);*/
		break;
	      } 
	    if (k==(nevents+nplaces)) {
	      nextstate->probability=1.0 / cnt;
	      /*
	      printf("%d:%s -%s:%g-> %d:%s\n",
		     curstate->number,curstate->state,
		     events[nextstate->enabled]->event,
		     nextstate->probability,
		     nextstate->stateptr->number,
		     nextstate->stateptr->state);*/
	    }
	  }
	} 
      }

    /* Determine steady state probabilities */
    double *prob;
    prob=(double*)GetBlock((state_cnt+1)*sizeof(double));
    last=0.0;
    do {
      for (int l=0;l<=state_cnt;l++)
	prob[l]=0.0;
      for (int step=0;step<period;step++) {
	for (i=0;i<PRIME;i++)
	  for (curstate=state_table[i];
	       curstate != NULL && curstate->state != NULL;
	       curstate=curstate->link) {
	    if (curstate->color==step) {
	      if (curstate->probability > 0.0) {
		if (curstate->succ) {
		  for (nextstate=curstate->succ;nextstate;
		       nextstate=nextstate->next) {
		    prob[nextstate->stateptr->number]=
		      prob[nextstate->stateptr->number]+
		      curstate->probability*nextstate->probability;
		  }
		} else {
		  prob[curstate->number]+=curstate->probability;
		}
	      }
	    }
	  }
	updated=false;
	for (i=0;i<PRIME;i++)
	  for (curstate=state_table[i];
	       curstate != NULL && curstate->state != NULL;
	       curstate=curstate->link) 
	    if (curstate->color==((step+1) % period)) {
	      if ((prob[curstate->number] > tolerance) &&
		  (fabs(curstate->probability-prob[curstate->number])/
		   curstate->probability > tolerance)) {
		updated=true;
	      /*
	printf("%f %f %f\n",curstate->probability,prob[curstate->number],
		   fabs(curstate->probability-prob[curstate->number])/
		   curstate->probability);*/
	      }
	      curstate->probability=prob[curstate->number];
	    }
      }
    } while (updated);

    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) 
	curstate->probability=curstate->probability / period;

    /* Adjust rates from EMC back to CMC */
    double adjust;
    double total=0.0;
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	/*
	adjust=0.0;
	for (nextstate=curstate->succ;nextstate;
	     nextstate=nextstate->next) 
	  adjust=adjust + nextstate->probability;
	*/
	adjust=0.0;
	for (nextstate=curstate->succ;nextstate;
	     nextstate=nextstate->next) {
	  for (k=1;k<nevents+nplaces;k++) {
	    if (rules[k][nextstate->enabled]->ruletype) {
	      adjust=adjust+rules[k][nextstate->enabled]->rate;
	      break;
	    }
	  }
	  if (k==(nevents+nplaces)) adjust=adjust+1.0;
	}

	if (adjust!=0.0)
	  curstate->probability=curstate->probability / adjust;
	total=total+curstate->probability;
      }
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) 
	curstate->probability=curstate->probability / total;

    double *sigProb;
    sigProb=(double*)GetBlock(2*nsignals*sizeof(double));
    for (int l=0;l<(2*nsignals);l++)
      sigProb[l]=0.0;
    for (i=0;i<PRIME;i++)
      for (curstate=state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link) {
	for (int l=0;l<nsignals;l++)
	  if ((curstate->state[l]=='0')||(curstate->state[l]=='R'))
	    sigProb[l+nsignals]+=curstate->probability;
	  else 
	    sigProb[l]+=curstate->probability;
     }
//     for (int l=0;l<nsignals;l++) {
//       printf("%s (high) = %f\n",signals[l]->name,100*sigProb[l]);
//       fprintf(lg,"%s (high) = %f\n",signals[l]->name,100*sigProb[l]);
//       printf("%s (low) = %f\n",signals[l]->name,100*sigProb[l+nsignals]);
//       fprintf(lg,"%s (low) = %f\n",signals[l]->name,100*sigProb[l+nsignals]);
//     }
    FILE *fp;
    printf("Results of Markovian analysis stored in sim-rep.txt\n");
    fprintf(lg,"Results of Markovian analysis stored in sim-rep.txt\n");
    fp=fopen("sim-rep.txt","w");
    for (int l=0;l<nsignals;l++) {
      fprintf(fp,"%s(high) ",signals[l]->name);
      fprintf(fp,"%s(low) ",signals[l]->name);
    }
    fprintf(fp,"\n");
    for (int l=0;l<nsignals;l++) {
      fprintf(fp,"%f ",100*sigProb[l]);
      fprintf(fp,"%f ",100*sigProb[l+nsignals]);
    }
    fprintf(fp,"\n");
  }
  return true;
}
