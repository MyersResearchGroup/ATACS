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

/*****************************************************************************/
/* atacs.c -                                                                 */
/*   Includes procedures to get and process commands from either the         */
/*   command line or entered by the user.                                    */
/*****************************************************************************/

/*
#include <fcntl.h>
*/

#include "atacs.h"
#ifdef TIMER
#include <sys/time.h> 
#endif 

#define STDMODE 0600

#include <string>

FILE *lg;
int status;
designADT design;
int problems(0);  // Problems encountered during this atacs run.

/*****************************************************************************/
/* Get a token from the keyboard.                                            */
/*****************************************************************************/

int gettok(char *tokvalue, int maxtok) 

{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while (TRUE) {
    switch (c=getchar()) {
    case '!':
      if (!readword) {
	return(SHELL);
      } else {
	ungetc('!', stdin);
	return (WORD);
      }
    case '\n':
      if (!readword) {
	return(END_OF_LINE);
      } else {
	ungetc('\n', stdin);
	return (WORD);
      }
      break;
    case ' ':
      if (!readword) {
	;
      } else {
	return (WORD);      
      }
      break;
    default:
      if (toklen < maxtok) {
	readword=TRUE;
	*tokvalue++=c;
	*tokvalue = '\0';
	toklen++;
      }
      break;
    }
  }
  return 0;
}

/*****************************************************************************/
/* Switch between exact and heuristic cyclic table resolution.               */
/*****************************************************************************/

void exact_resolution(designADT design)
{
  if (design->exact) {
    printf("Exact cyclic table resolution mode now turned OFF.\n");
    fprintf(lg,"Exact cyclic table resolution mode now turned OFF.\n");
    design->exact=FALSE;
  } else {
    printf("Exact cyclic table resolution mode now turned ON.\n");
    fprintf(lg,"Exact cyclic table resolution mode now turned ON.\n");
    design->exact=TRUE;
  }
  if (design->status & FOUNDCONF) 
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch between using gate sharing constraints and not.                    */
/*****************************************************************************/

void share_gates(designADT design)
{
  if (design->sharegates) {
    printf("No longer using gate sharing constraints.\n");
    fprintf(lg,"No longer using gate sharing constraints.\n");
    design->sharegates=FALSE;
  } else {
    printf("Using gate sharing constraints.\n");
    fprintf(lg,"Using gate sharing constraints.\n");
    design->sharegates=TRUE;
  }
  if (design->status & FOUNDCONF) 
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch to using burst-mode gC constraints for synthesis.                  */
/*****************************************************************************/

void burstgc(designADT design)
{
  printf("Now generating burst-mode gC implementations.\n");
  fprintf(lg,"Now generating burst-mode gC implementations.\n");
  design->gatelevel=FALSE;
  design->gC=FALSE;
  design->atomic=FALSE;
  design->burstgc=TRUE;
  design->burst2l=FALSE;
  if (design->status & FOUNDREG) 
    new_impl(design,(2*design->nsignals)+1,TRUE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG);
}

/*****************************************************************************/
/* Switch to using burst-mode 2-level constraints for synthesis.             */
/*****************************************************************************/

void burst2l(designADT design)
{
  printf("Now generating burst-mode two-level implementations.\n");
  fprintf(lg,"Now generating burst-mode two-level implementations.\n");
  design->gatelevel=FALSE;
  design->gC=FALSE;
  design->atomic=FALSE;
  design->burst2l=TRUE;
  design->burstgc=FALSE;
  if (design->status & FOUNDREG) 
    new_impl(design,(2*design->nsignals)+1,TRUE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG);
}

/*****************************************************************************/
/* Switch to turn postprocessing during compilation on and off.              */
/*****************************************************************************/

void use_postproc(designADT design)
{
  if (design->postproc) {
    printf("No longer using postprocessing during compilation.\n");
    fprintf(lg,"No longer using postprocessing during compilation.\n");
    design->postproc=FALSE;
  } else {
    printf("Now using postprocessing during compilation.\n");
    fprintf(lg,"Now using postprocessing during compilation.\n");
    design->postproc=TRUE;
  }
  /* Reset design */
}

/*****************************************************************************/
/* Switch to turn abstraction during compilation on and off.                 */
/*****************************************************************************/

void use_abstraction(designADT design)
{
  if (design->abstract) {
    printf("No longer using abstraction during compilation.\n");
    fprintf(lg,"No longer using abstraction during compilation.\n");
    design->abstract=FALSE;
  } else {
    printf("Now using abstraction during compilation.\n");
    fprintf(lg,"Now using abstraction compilation.\n");
    design->abstract=TRUE;
  }
  /* Reset design */
}

/*****************************************************************************/
/* Switch to turn redundant check during compilation on and off.             */
/*****************************************************************************/

void use_redchk(designADT design)
{
  if (design->redchk) {
    printf("No longer using redundant check during compilation.\n");
    fprintf(lg,"No longer using redundant check during compilation.\n");
    design->redchk=FALSE;
  } else {
    printf("Now using redundant check during compilation.\n");
    fprintf(lg,"Now using redundant check during compilation.\n");
    design->redchk=TRUE;
  }
  /* Reset design */
}

/*****************************************************************************/
/* Switch to turn xform2 on and off.                                         */
/*****************************************************************************/

void use_xform2(designADT design)
{
  if (design->xform2) {
    printf("No longer using transform 2 during postprocessing.\n");
    fprintf(lg,"No longer using transform 2 during postprocessing.\n");
    design->xform2=FALSE;
  } else {
    printf("Now using transform 2 during postprocessing.\n");
    fprintf(lg,"Now using transform 2 during postprocessing.\n");
    design->xform2=TRUE;
  }
  /* Reset design */
}

/*****************************************************************************/
/* Switch to turn expandrate on and off.                                     */
/*****************************************************************************/

void use_expandrate(designADT design)
{
  if (design->expandRate) {
    printf("No longer using rate expanded LHPNs.\n");
    fprintf(lg,"No longer using rate expanded LHPNs.\n");
    design->expandRate=FALSE;
  } else {
    printf("Now using rate expanded LHPNs.\n");
    fprintf(lg,"Now using rate expanded LHPNs.\n");
    design->expandRate=TRUE;
  }
  /* Reset design */
}

/*****************************************************************************/
/* Switch to turn TELs table clearing after compilation on and off.          */
/*****************************************************************************/

void use_newtab(designADT design)
{
  if (design->newtab) {
    printf("No longer clearing TELs table after compilation.\n");
    fprintf(lg,"No longer clearing TELs table after compilation.\n");
    design->newtab=FALSE;
  } else {
    printf("Now clearing TELs table after compilation.\n");
    fprintf(lg,"Now clearing TELs table after compilation.\n");
    design->newtab=TRUE;
  }
}

/*****************************************************************************/
/* Switch to allow only inputs to be used as context signals.                */
/*****************************************************************************/

void use_inponly(designADT design)
{
  if (design->inponly) {
    printf("Now allowing both inputs and outputs as context signals.\n");
    fprintf(lg,"Now allowing both inputs and outputs as context signals.\n");
    design->inponly=FALSE;
  } else {
    printf("Now allowing only input signals as context signals.\n");
    fprintf(lg,"Now allowing only input signals as context signals.\n");
    design->inponly=TRUE;
  }
  if (design->status & FOUNDCONF) 
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch to using atomic gates.                                             */
/*****************************************************************************/

void atomicGate(designADT design)
{
  printf("Now generating atomic gate implementations.\n");
  fprintf(lg,"Now generating atomic gate implementations.\n");
  design->atomic=TRUE;
  design->gC=FALSE;
  design->gatelevel=FALSE;
  design->burstgc=FALSE;
  design->burst2l=FALSE;
  if (design->status & FOUNDREG) 
    new_impl(design,(2*design->nsignals)+1,TRUE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG);
}

/*****************************************************************************/
/* Switch to using generalized C-implementations.                            */
/*****************************************************************************/

void generalizedC(designADT design)
{
  printf("Now generating generalized C-implementations.\n");
  fprintf(lg,"Now generating generalized C-implementations.\n");
  design->gC=TRUE;
  design->gatelevel=FALSE;
  design->atomic=FALSE;
  design->burstgc=FALSE;
  design->burst2l=FALSE;
  if (design->status & FOUNDREG) 
    new_impl(design,(2*design->nsignals)+1,TRUE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG);
}

/*****************************************************************************/
/* Switch gate level implementation mode on and off.                         */
/*****************************************************************************/

void gate_level(designADT design)
{
  printf("Now generating standard C-implementations.\n");
  fprintf(lg,"Now generating standard C-implementations.\n");
  design->gatelevel=TRUE;
  design->gC=FALSE;
  design->atomic=FALSE;
  design->burstgc=FALSE;
  design->burst2l=FALSE;
  if (design->status & FOUNDREG) 
    new_impl(design,(2*design->nsignals)+1,TRUE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG);
}

/*****************************************************************************/
/* Switch between using combinational optimization and not.                  */
/*****************************************************************************/

void use_combo(designADT design)
{
  if (design->combo) {
    printf("No longer using combinational optimization.\n");
    fprintf(lg,"No longer using combinational optimization.\n");
    design->combo=FALSE;
  } else {
    printf("Now using combinational optimization.\n");
    fprintf(lg,"Now using combinational optimization.\n");
    design->combo=TRUE;
  }
  design->status &= (LOADED      | CONNECTED | EXPANDED | REDUCED | FOUNDRED |
		     FOUNDSTATES | FOUNDRSG  | FOUNDREG | FOUNDCONF);
}

/*****************************************************************************/
/* Switch between using PARG and DOT for displaying routines.                */
/*****************************************************************************/

void use_dot(designADT design)
{
  if (design->dot) {
    printf("PARG now being used for display and print routines.\n");
    fprintf(lg,"PARG now being used for display and print routines.\n");
    design->dot=FALSE;
  } else {
    printf("DOT now being used for display and print routines.\n");
    fprintf(lg,"DOT now being used for display and print routines.\n");
    design->dot=TRUE;
  }
}

/*****************************************************************************/
/* Switch verbose mode on and off.                                           */
/*****************************************************************************/

void verbose(designADT design)
{
  if (design->verbose) {
    printf("Verbose mode now turned OFF.\n");
    fprintf(lg,"Verbose mode now turned OFF.\n");
    design->verbose=FALSE;
  } else {
    printf("Verbose mode now turned ON.\n");
    fprintf(lg,"Verbose mode now turned ON.\n");
    design->verbose=TRUE;
  }
}

/*****************************************************************************/
/* Toggle automation                                                         */
/*****************************************************************************/
void toggle(bool & value, const string & mode){
  loggedStream messages(cout,lg);
  if (value) {
    value = false;
    messages << mode << " now DISABLED." << endl;
  }
  else{
    value = true;
    messages << mode << " now ENABLED."  << endl;
  }
}

/*****************************************************************************/
/* Switch between using TSE and my algorithm for timing analysis             */
/*****************************************************************************/
#ifdef TSE_ALGM
void tse(designADT design)
{
  if (design->closure) {
    printf("Unfolding algorithm being used for timing analysis.\n");
    fprintf(lg,"Unfolding algorithm being used for timing analysis.\n");
    design->closure=FALSE;
    if (design->status & LOADED) free_tse();
  } else {
    printf("Closure algorithm being used for timing analysis.\n");
    fprintf(lg,"Closure algorithm being used for timing analysis.\n");
    design->closure=TRUE;
    if (design->status & LOADED) 
      init_tse(design->filename,design->events,design->rules,design->nevents);
  }
}
#endif

/*****************************************************************************/
/* Switch between using or not using untimed state exploration.              */
/*****************************************************************************/

void use_untimed(char command,designADT design)
{
  printf("Using no timing analysis.\n");
  fprintf(lg,"Using no timing analysis.\n");
  design->untimed=TRUE;
  design->compress=FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->geometric=FALSE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using or not using compressed state graphs. .              */
/*****************************************************************************/

void use_compress(char command,designADT design)
{
  printf("Using compressed state graphs.\n");
  fprintf(lg,"Using compressed state graphs.\n");
  design->untimed=FALSE;
  design->compress=TRUE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->geometric=FALSE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using or not using heuristic timing analysis.              */
/*****************************************************************************/

void use_heuristic(char command,designADT design)
{
  printf("Using heuristic timing analysis.\n");
  fprintf(lg,"Using heuristic timing analysis.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->heuristic=TRUE;
  design->orbits=FALSE;
  design->geometric=FALSE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) { 
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using or not using Orbits to obtain state graphs.          */
/*****************************************************************************/

void use_orbits(char command,designADT design)
{
  printf("Using Orbits to obtain state graphs.\n");
  fprintf(lg,"Using Orbits to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->orbits=TRUE;
  design->heuristic=FALSE;
  design->geometric=FALSE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) { 
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between projecting and not projecting states (default project)     */
/*****************************************************************************/

void use_noproj(char command,designADT design)
{
  if (design->timeopts.noproj) {
    printf("Now projecting states in state graph.\n");
    fprintf(lg,"Now projecting states in state graph.\n");
    design->timeopts.noproj=FALSE;
  } else {
    printf("No longer projecting states in state graph.\n");
    fprintf(lg,"No longer projecting states in state graph.\n");
    design->timeopts.noproj=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between failures as being errors or warnings (default error)       */
/*****************************************************************************/

void use_nofail(char command,designADT design)
{
  if (design->timeopts.nofail) {
    printf("Failures are now considered errors.\n");
    fprintf(lg,"Failures are now considered errors.\n");
    design->timeopts.nofail=FALSE;
  } else {
    printf("Failures are now considered warnings.\n");
    fprintf(lg,"Failures are now considered warnings.\n");
    design->timeopts.nofail=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between keeping going after failures or not                        */
/*****************************************************************************/

void use_keepgoing(char command,designADT design)
{
  if (design->timeopts.keepgoing) {
    printf("Failures now terminate analysis.\n");
    fprintf(lg,"Failures now terminate analysis.\n");
    design->timeopts.keepgoing=FALSE;
  } else {
    printf("Analysis now keeps going after failures.\n");
    fprintf(lg,"Analysis now keeps going after failures.\n");
    design->timeopts.keepgoing=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between disabling being a failure or not (default failure)         */
/*****************************************************************************/

void use_disabling(char command,designADT design)
{
  if (design->timeopts.disabling) {
    printf("Rule disablings are now considered failures.\n");
    fprintf(lg,"Rule disablings are now considered failures.\n");
    design->timeopts.disabling=FALSE;
  } else {
    printf("Allow rules to be disabled with only a warning.\n");
    fprintf(lg,"Allow rules to be disabled with only a warning.\n");
    design->timeopts.disabling=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using subsets or exact region matches.                     */
/*****************************************************************************/

void use_subsets(char command,designADT design)
{
  if (design->timeopts.subsets) {
    printf("Using exact region match during state space exploration.\n");
    fprintf(lg,"Using exact region match during state space exploration.\n");
    design->timeopts.subsets=FALSE;
  } else {
    printf("Using subset region match during state space exploration.\n");
    fprintf(lg,"Using subset region match during state space exploration.\n");
    design->timeopts.subsets=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using supersets or exact region matches.                   */
/*****************************************************************************/

void use_supersets(char command,designADT design)
{
  if (design->timeopts.supersets) {
    printf("Using exact region match during state space exploration.\n");
    fprintf(lg,"Using exact region match during state space exploration.\n");
    design->timeopts.supersets=FALSE;
  } else {
    printf("Using superset region match during state space exploration.\n");
   fprintf(lg,"Using superset region match during state space exploration.\n");
    design->timeopts.supersets=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using interleaving optimization and not.                   */
/*****************************************************************************/

void use_interleav(char command,designADT design)
{
  if (design->timeopts.interleav) {
    printf("No longer using interleaving optimization.\n");
    fprintf(lg,"No longer using interleaving optimization.\n");
    design->timeopts.interleav=FALSE;
  } else {
    printf("Now using interleaving optimization.\n");
    fprintf(lg,"Now using interleaving optimization.\n");
    design->timeopts.interleav=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) { 
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using stack pruning on superset optimization and not.      */
/*****************************************************************************/

void use_prune(char command,designADT design)
{
  if (design->timeopts.prune) {
    printf("No longer using stack pruning on superset optimization.\n");
    fprintf(lg,"No longer using stack pruning on superset optimization.\n");
    design->timeopts.prune=FALSE;
  } else {
    printf("Now using stack pruning on superset optimization.\n");
    fprintf(lg,"Now using stack pruning on superset optimization.\n");
    design->timeopts.prune=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using infinity optimization or not.                        */
/*****************************************************************************/

void use_infopt(char command,designADT design)
{
  if (design->timeopts.infopt) {
    printf("No longer using infinity optimization.\n");
    fprintf(lg,"No longer using infinity optimization.\n");
    design->timeopts.infopt=FALSE;
  } else {
    printf("Now using infinity optimization.\n");
    fprintf(lg,"Now using infinity optimization.\n");
    design->timeopts.infopt=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using orbits match or not.                                 */
/*****************************************************************************/

void use_orbmatch(char command,designADT design)
{
  if (design->timeopts.orbmatch) {
    printf("No longer using orbits match.\n");
    fprintf(lg,"No longer using orbits match.\n");
    design->timeopts.orbmatch=FALSE;
  } else {
    printf("Now using orbits match.\n");
    fprintf(lg,"Now using orbits match.\n");
    design->timeopts.orbmatch=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

//-----------------------------------------------------------use_supersets
// Toggles on and off the partial order optimization in posets, bap, and
// baptdc.
//------------------------------------------------------------------------
void use_pored(char command,designADT design)
{
  if (design->timeopts.PO_reduce) {
    printf("Using total order exploration in bap, baptdc, and posets.\n");
    fprintf(lg,"Using total order exploration in bap, baptdc, and posets.\n");
    design->timeopts.PO_reduce=FALSE;
  } else {
    printf("Using partial order reduction in bap, baptdc, and posets.\n");
    fprintf(lg,"Using partial order reduction in bap, baptdc,and posets.\n");
    design->timeopts.PO_reduce=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}
//------------------------------------------------------------------------

/*****************************************************************************
* Toggles on and off the use of LPN code in the future variables method
*****************************************************************************/
void use_lpn(char command,designADT design) 
{
  if(design->timeopts.lpn) {
    printf("Turning off code to correctly handle LPNs.\n");
    fprintf(lg,"Turning off code to correctly handle LPNs.\n");
    design->timeopts.lpn=FALSE;
  }
  else {
    printf("Turning on code to correctly handle LPNs.\n");
    fprintf(lg,"Turning on code to correctly handle LPNs.\n");
    design->timeopts.lpn=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between generating a region graph and a state graph.               */
/*****************************************************************************/

void gen_reg_graph(char command,designADT design)
{
  if (design->timeopts.genrg) {
    printf("Generating state graph during state space exploration.\n");
    fprintf(lg,"Generating state graph during state space exploration.\n");
    design->timeopts.genrg=FALSE;
  } else {
    printf("Generating region graph during state space exploration.\n");
    fprintf(lg,"Generating region graph during state space exploration.\n");
    design->timeopts.genrg=TRUE;
  }    
  if (!(design->fromRSG) && !(design->fromUNC)) { 
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using or not using geometric timing to obtain state graphs.*/
/*****************************************************************************/

void use_geometric(char command,designADT design)
{
  printf("Using geometric timing to obtain state graphs.\n");
  fprintf(lg,"Using geometric timing to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=TRUE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the BAG flag for state space exploration                     
/////////////////////////////////////////////////////////////////////////////
void use_bag(char command,designADT design)
{
  printf("Using bourne again geometric timing to obtain state graphs.\n");
  fprintf(lg,"Using bourne again geometric timing to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  
  // egm 08/30/00 -- Set the flag on bag!
  design->bag = TRUE;

  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the BAP flag for state space exploration                     
/////////////////////////////////////////////////////////////////////////////
void use_bap(char command,designADT design)
{
  printf("Using bourne again POSET timing to obtain state graphs.\n");
  fprintf(lg,"Using bourne again POSET timing to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  
  // egm 08/30/00 -- Set the flag on bag!
  design->bag = FALSE;
  
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = TRUE;

  // egm 04/12/01 -- Set the TDC flag to false!
  design->timeopts.TDC = false;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the SL code                     
/////////////////////////////////////////////////////////////////////////////
void use_srl(char command,designADT design)
{
  printf("Using SRL timing to obtain state graphs.\n");
  fprintf(lg,"Using SRL to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = TRUE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  design->bag = FALSE;
  design->bap = FALSE;
  design->timeopts.TDC = false;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the HPN code                     
/////////////////////////////////////////////////////////////////////////////
void use_hpn(char command,designADT design)
{
  printf("Using HPN timing to obtain state graphs.\n");
  fprintf(lg,"Using HPN to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = TRUE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  design->bag = FALSE;
  design->bap = FALSE;
  design->timeopts.TDC = false;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the LHPN DBM code                     
/////////////////////////////////////////////////////////////////////////////
void use_lhpndbm(char command,designADT design)
{
  printf("Using DBMs to obtain state graphs from LHPNs.\n");
  fprintf(lg,"Using DBMs to obtain state graphs from LHPNs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = TRUE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  design->bag = FALSE;
  design->bap = FALSE;
  design->timeopts.TDC = false;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the LHPN BDD code                     
/////////////////////////////////////////////////////////////////////////////
void use_lhpnbdd(char command,designADT design)
{
  printf("Using BDDs to obtain state graphs from LHPNs.\n");
  fprintf(lg,"Using BDDs to obtain state graphs from LHPNs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = TRUE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  design->bag = FALSE;
  design->bap = FALSE;
  design->timeopts.TDC = false;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the LHPN SMT code                     
/////////////////////////////////////////////////////////////////////////////
void use_lhpnsmt(char command,designADT design)
{
  printf("Using SMT solver to obtain state graphs from LHPNs.\n");
  fprintf(lg,"Using SMT solver to obtain state graphs from LHPNs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = TRUE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  design->bag = FALSE;
  design->bap = FALSE;
  design->timeopts.TDC = false;

  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Turns on the BAP flag with TDC for state space exploration
/////////////////////////////////////////////////////////////////////////////
void use_bap_tdc(char command,designADT design)
{
  printf("Using bourne again POSET timing with TDC to obtain state graphs.\n");
  fprintf(lg,
	 "Using bourne again POSET timing with TDC to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->geometric=FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  design->posets=FALSE;
  
  // egm 08/30/00 -- Set the flag on bag!
  design->bag = FALSE;
  
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = TRUE;

  // egm 04/12/01 -- Set the TDC flag to true!
  design->timeopts.TDC = true;
  
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

//////////////////////////////////////////////////////////////////////////
// Suppress or unsuppress parg output windows on errors
//////////////////////////////////////////////////////////////////////////
void toggle_PARG_output(designADT design)
{
  if (design->noparg) {
    printf("PARG output on errors enabled.\n");
    fprintf(lg,"PARG output on errors enabled.\n");
    design->noparg=FALSE;
  } else {
    printf("PARG output on errors disabled.\n");
    fprintf(lg,"PARG output on errors disabled.\n");
    design->noparg=TRUE;
  }    
}

//////////////////////////////////////////////////////////////////////////
// Toggles cyclic and connectivity checks on TELs and TERs
//////////////////////////////////////////////////////////////////////////
void toggle_checks(designADT design)
{
  if (design->nochecks) {
    printf("Cyclic and connectivity checks enabled.\n");
    fprintf(lg,"Cyclic and connectivity checks enabled.\n");
    design->nochecks=FALSE;
  } else {
    printf("Cyclic and connectivity checks disabled.\n");
    fprintf(lg,"Cyclic and connectivity checks disabled.\n");
    design->nochecks=TRUE;
  }    
}


/*****************************************************************************/
/* Switch between using or not using POApprox timing to obtain state graphs. */
/*****************************************************************************/

void use_poapprox(char command,designADT design)
{
  printf("Using PO approx timing to obtain state graphs.\n");
  fprintf(lg,"Using PO approx timing to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->poapprox=TRUE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->geometric=FALSE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->pomaxdiff=FALSE;
  design->posets=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) { 
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using or not using POSets timing to obtain state graphs.   */
/*****************************************************************************/

void use_posets(char command,designADT design)
{
  printf("Using PO sets timing to obtain state graphs.\n");
  fprintf(lg,"Using PO sets timing to obtain state graphs.\n");
  design->untimed=FALSE;
  design->compress=FALSE;
  design->posets=TRUE;
  design->heuristic=FALSE;
  design->orbits=FALSE;
  design->geometric=FALSE;
  // egm 09/11/00 -- Set the flag on bag!
  design->bag = FALSE;
  // egm 11/15/00 -- Set the flag on bap!
  design->bap = FALSE;
  design->srl = FALSE;
  design->hpn = FALSE;
  design->lhpndbm = FALSE;
  design->lhpnbdd = FALSE;
  design->lhpnsmt = FALSE;
  design->pomaxdiff=FALSE;
  design->poapprox=FALSE;
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Switch between using or not using single-cube algorithm for synthesis.    */
/*****************************************************************************/

void use_single(designADT design)
{
  printf("Using single-cube algorithm for synthesis.\n");
  fprintf(lg,"Using single-cube algorithm for synthesis.\n");
  design->single=TRUE;
  design->multicube=FALSE;
  design->bdd=FALSE;
  design->syn=FALSE;
  design->sis=FALSE;
  design->direct=FALSE;
  if (design->status & FOUNDREG)
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch exact cover mode on and off.                                       */
/*****************************************************************************/

void multicube_cover(designADT design)
{
  printf("Using multi-cube covering procedure.\n");
  fprintf(lg,"Using multi-cube covering procedure.\n");
  design->multicube=TRUE;
  design->single=FALSE;
  design->bdd=FALSE;
  design->syn=FALSE;
  design->sis=FALSE;
  design->direct=FALSE;
  if (design->status & FOUNDREG)
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch between using or not using bdd methods for synthesis.              */
/*****************************************************************************/

void use_bdd(designADT design)
{
  printf("Using BDD methods for synthesis.\n");
  fprintf(lg,"Using BDD methods for synthesis.\n");
  design->bdd=TRUE;
  design->single=FALSE;
  design->multicube=FALSE;
  design->syn=FALSE;
  design->sis=FALSE;
  design->direct=FALSE;
  if (!(design->fromUNC)) { 
    if (design->status & FOUNDREG)
      new_impl(design,(2*design->nsignals)+1,FALSE);
    design->status &=  (LOADED | CONNECTED | EXPANDED | REDUCED | FOUNDRED);
  }
}

/*****************************************************************************/
/* Switch between using or not using SYN for synthesis.                      */
/*****************************************************************************/

void use_syn(designADT design)
{
  printf("Using SYN for synthesis.\n");
  fprintf(lg,"Using SYN for synthesis.\n");
  design->syn=TRUE;
  design->single=FALSE;
  design->multicube=FALSE;
  design->bdd=FALSE;
  design->sis=FALSE;
  design->direct=FALSE;
  if (design->status & FOUNDREG)
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch between using or not using SIS for synthesis.                      */
/*****************************************************************************/

void use_sis(char command,designADT design)
{
  printf("Using SIS for synthesis.\n");
  fprintf(lg,"Using SIS for synthesis.\n");
  design->sis=TRUE;
  design->single=FALSE;
  design->multicube=FALSE;
  design->bdd=FALSE;
  design->syn=FALSE;
  design->direct=FALSE;
  if (design->status & LOADED) make_env(command,design);
  if (design->status & FOUNDREG)
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch between using or not using SIS for synthesis.                      */
/*****************************************************************************/

void use_direct(designADT design)
{
  printf("Using direct synthesis method.\n");
  fprintf(lg,"Using direct synthesis method.\n");
  design->sis=FALSE;
  design->single=FALSE;
  design->multicube=FALSE;
  design->bdd=FALSE;
  design->syn=FALSE;
  design->direct=TRUE;
  if (design->status & FOUNDREG)
    new_impl(design,(2*design->nsignals)+1,FALSE);
  design->status &= 
    (LOADED|CONNECTED|EXPANDED|REDUCED|FOUNDRED|FOUNDSTATES|FOUNDRSG|FOUNDREG);
}

/*****************************************************************************/
/* Switch between handling and not handling exceptions.                      */
/*****************************************************************************/

void handle_exceptions(designADT design)
{
  if (design->exception) {
    printf("Exception handling now turned OFF.\n");
    fprintf(lg,"Exception handling now turned OFF.\n");
    design->exception=FALSE;
  } else {
    printf("Exception handling now turned ON.\n");
    fprintf(lg,"Exception handling now turned ON.\n");
    design->exception=TRUE;
  }
  design->brk_exception=design->exception;
}

/*****************************************************************************/
/* Switch between handling and not handling exceptions.                      */
/*****************************************************************************/

void manual_breakup(designADT design)
{
  if (design->manual) {
    printf("Decompositions done automatically.\n");
    fprintf(lg,"Decompositions done automatically.\n");
    design->manual=FALSE;
  } else {
    printf("Decompositions done manually.\n");
    fprintf(lg,"Decompositions done manually.\n");
    design->manual=TRUE;
  }
}

/*****************************************************************************/
/* Change maximum allowable stack size.                                      */
/*****************************************************************************/

void change_maxsize(designADT design)
{
  printf("Changing maximum transistor stack size from %d to %d.\n",
	 design->oldmaxsize,design->maxsize);
  fprintf(lg,"Changing maximum transistor stack size from %d to %d.\n",
	  design->oldmaxsize,design->maxsize);
  design->oldmaxsize=design->maxsize;
  design->status &= (~STOREDPRS);
}

/*****************************************************************************/
/* Change number of iterations for SMT solver                                */
/*****************************************************************************/

void change_iterations(designADT design)
{
  printf("Changing number of iterations for SMT solver from %d to %d.\n",
	 design->olditerations,design->iterations);
  fprintf(lg,"Changing number of iterations for SMT solver from %d to %d.\n",
	  design->olditerations,design->iterations);
  design->olditerations=design->iterations;
}

/*****************************************************************************/
/* Change the linksize used in BDDs.                                         */
/*****************************************************************************/

void change_linksize(char command,designADT design)
{
#ifdef DLONG
  printf("Changing linksize for BDDs from %d to %d.\n",
	 design->bdd_state->oldlinksize,design->bdd_state->linksize);
  fprintf(lg,"Changing linksize for BDDs from %d to %d.\n",
	  design->bdd_state->oldlinksize,design->bdd_state->linksize);
  design->bdd_state->oldlinksize=design->bdd_state->linksize;
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
#endif
}

/*****************************************************************************/
/* Change the amount of simulation time.                                     */
/*****************************************************************************/

void change_simtime(char command,designADT design)
{
  printf("Changing simulation time to %d.\n",design->simtime);
  fprintf(lg,"Changing simulation time to %d.\n",design->simtime);
}

/*****************************************************************************/
/* Change number of cycles used in constraint graph.                         */
/*****************************************************************************/

void change_cycles(char command,designADT design)
{
  printf("Changing number of cycles from %d to %d.\n",design->oldncycles,
	 design->ncycles);
  fprintf(lg,"Changing number of cycles from %d to %d.\n",
	  design->oldncycles,design->ncycles);
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
  design->oldncycles=design->ncycles;
}

/*****************************************************************************/
/* Change default maximum gate delay used in added rules.                    */
/*****************************************************************************/

void change_gatedelay(char command,designADT design)
{
  printf("Changing minimum gate delay from %d to %d.\n",
	 design->oldmingatedelay,design->mingatedelay);
  fprintf(lg,"Changing minimum gate delay from %d to %d.\n",
	  design->oldmingatedelay,design->mingatedelay);
  printf("Changing maximum gate delay from ");
  fprintf(lg,"Changing maximum gate delay from ");
  if (design->oldmaxgatedelay==INFIN) {
    printf("infinity to ");
    fprintf(lg,"infinity to ");
  } else {
    printf("%d to ",design->oldmaxgatedelay);
    fprintf(lg,"%d to ",design->oldmaxgatedelay);
  }
  if (design->maxgatedelay==INFIN) {
    printf("infinity\n");
    fprintf(lg,"infinity\n");
  } else {
    printf("%d\n",design->maxgatedelay);
    fprintf(lg,"%d\n",design->maxgatedelay);
  }
  design->oldmingatedelay=design->mingatedelay;
  design->oldmaxgatedelay=design->maxgatedelay;
  if (!(design->fromRSG) && !(design->fromUNC)) {
    reinit_design(command,design,FALSE);
    design->status &= LOADED;
  }
}

/*****************************************************************************/
/* Make speed-independent by assigning lower-bound of delay to 0 and         */
/*   upper-bound to infinity.                                                */
/*****************************************************************************/

void declare_si(char command,designADT design)
{
  int i,j;

  if (!(design->si)) {
    strcat(design->filename,"SI");
    printf("Declaring speed-independent, filename changed to:  %s\n",
	   design->filename);
    fprintf(lg,"Declaring speed-independent, filename changed to:  %s\n",
	    design->filename);
    design->si=TRUE;
    if (!(design->fromRSG) && !(design->fromUNC)) {
      reinit_design(command,design,FALSE);
      for (i=0;i<design->nevents;i++)
	for (j=0;j<design->nevents;j++) 
	  if ((design->rules[i][j]->ruletype != NORULE) &&
	      (!(design->rules[i][j]->ruletype & ORDERING))) {
	    design->rules[i][j]->lower=0;
	    design->rules[i][j]->upper=INFIN;
	  }
      design->status=LOADED;
    }
  }
}

/*****************************************************************************/
/* Control of synthesis routines.                                            */
/*****************************************************************************/

void synthesis(char menu,char command,designADT design,bool sifile,
	       bool newfile)
{
#ifdef TIMER
  timeval t1,t2;
  double time1,time2;
#endif

  if (design->fromCSP || design->fromVHDL || design->fromHSE) 
    if (!compile(command,design,newfile)) return;
  if ((design->fromG || (design->fromLPN && menu==SYNTHESIS)) && newfile) {
    if (!load_graph(design)) return;
    design->fromG = TRUE;
    design->fromLPN = FALSE;
  }
  if ((!design->fromRSG) && (!design->fromUNC) && (!design->fromLPN)) {
    if (!load_event_rule_system(menu,command,design,sifile,newfile)) return;
    if (!handshaking_expansion(command,design)){
      problems++;
      return;
    }
    if (!design->nochecks) 
      if (!SConnect(command,design)) return;
    if ((menu==EXCEPTIONS) && (command==CONNECT)) return;
    if (design->direct) {
      heuristic_synthesis(menu,command,design);
      return;
    }
    if (!design->nochecks) 
      if (!find_redundant_rules(menu,command,design)) return;
    if (((menu==SYNTHESIS) && (command==FINDRED)) ||
        ((menu==EXCEPTIONS) && (command==ADDORD))) return;
    if ((command==GRAPH) || (design->sis)) 
      store_g(design->filename,design->signals,
              design->events,design->merges,design->rules,
              design->nevents,design->nplaces,design->ninputs,
              design->startstate,design->initval,design->initrate,
              design->level,design->fromG,design->fromLPN);
    if (design->sis) {
      load_sis(design); 
      return;
    }
  } else 
    if (newfile) {
      if (design->fromUNC) {
        if (!load_extended_burstmode_graph(design)) return;
      } else if (design->fromLPN) {
	if (!load_lpn(design)) return;
      } else {
        if (!load_reduced_state_graph(design)) return;
      }
    }
  if ((!find_reduced_state_graph(menu,command,design))){
    problems++;
    return;
  }
  else if ((command==FINDRSG)||(command==PROJECT)||(menu==VERIFY)){
    return;
  }
  if (design->syn)
    store_state_graph(design->filename,design->signals,design->events,
		      design->state_table,
                      design->ninpsig,design->nsignals);
#ifdef DLONG
  if (design->bdd) {
    bddize_reduced_state_graph(command,design);  
    return;
  }
#else 
  if (design->bdd) {
#ifdef CUDD
    cudd_synthesis(command,design);  
#else
    printf("BDD library is unavailable.\n");
    fprintf(lg,"BDD library is unavailable.\n");
#endif
    return;
  }
#endif
#ifdef TIMER
  gettimeofday(&t1,NULL);
#endif
  if (((!find_regions(command,design)) && (!design->multicube)) || 
      (command==FINDREG)||((menu==EXCEPTIONS)&&
                           ((command==ADDPERS)||(command==SOLVECSC)||
                            (command==FINDSV)))) return;
  if (design->multicube) {
    find_exact_cover(design);
#ifdef TIMER
    gettimeofday(&t2,NULL);
    time1 = (t1.tv_sec+(t1.tv_usec*.000001));
    time2 = (t2.tv_sec+(t2.tv_usec*.000001));
    fprintf(lg,"synthesis took %g seconds.\n",time2-time1);
#endif
    return;
  }
  if (design->syn)
    if ((!load_production_rules(command,design)) || (command==FINDCOVER)) 
      return;
  if ((!find_conflicts(command,design)) || (command==FINDVIOL)) return;
  if ((!find_cover(command,design)) || (command==FINDCOVER) || 
      ((menu==EXCEPTIONS) && (command==RESOLVE))) return;
#ifdef TIMER
  gettimeofday(&t2,NULL);
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  time2 = (t2.tv_sec+(t2.tv_usec*.000001));
  fprintf(lg,"synthesis took %g seconds.\n",time2-time1);
#endif

  if(store_production_rules(command,design)){
    problems=0;
  }
}

//------------------------------------------------------------------------
// Hazard Free SI decomposition
//------------------------------------------------------------------------
void do_decomp( char menu        ,
		char command     ,
		designADT design ,
		bool sifile      ,
		bool newfile       ) {
  if ( !( design->status & FOUNDRSG   &&
	  design->status & FOUNDREG   && 
	  design->status & FOUNDCONF  &&
	  design->status & FOUNDCOVER    ) ) {
    synthesis(menu,command,design,sifile,newfile);
  }
  decomp( design );
}

//------------------------------------------------------------------------
// Hazard Free SI decomposition
//------------------------------------------------------------------------
void do_biomap( char menu        ,
		char command     ,
		designADT design ,
		bool sifile      ,
		bool newfile       ) {
  if ( !( design->status & FOUNDRSG   &&
	  design->status & FOUNDREG   && 
	  design->status & FOUNDCONF  &&
	  design->status & FOUNDCOVER    ) ) {
    printf("menu %c command %c\n", menu, command); 
    synthesis(menu,command,design,sifile,newfile);
  }
  bio_map( design );
}

/*****************************************************************************/
/* Process a command from either the user interface or the command line.     */
/*   Returns FALSE if no ER System is currently loaded, and                  */
/*   returns TRUE if an ER System is currently loaded.                       */
/*****************************************************************************/

int process_command(char menu,char command,designADT design,char * selection,
		    bool newfile,bool sifile)
{
  //timeval t;
  //double curtime;
  //time_t tep;

  //gettimeofday(&t,NULL);
  //curtime = (t.tv_sec+(t.tv_usec*.000001));
  //tep = time(NULL);
  //if (tep > 959388805)
  //  exit(1);

  switch (menu) {
  case LOAD:
    switch (command) {
    case KISS2SG:
      kiss2sg(design->filename,design->state_table,design->timeopts,
              design->bdd_state,design->bdd,design->markkey); 
      design->status=design->status | FOUNDRSG;
      break;
    case GRAPH:
      if (load_graph(design)) {
        design->fromG=true;
        load_event_rule_system(menu,command,design,sifile,newfile);
        /*
          if (design->postproc || design->redchk) {
          postprocess(design->filename,design->postproc,design->redchk,
		      design->fromG,design->verbose);
          load_event_rule_system(menu,command,design,sifile,newfile);
          }*/
      }
      break;
    case LPN:
      load_lpn(design);
      break;
    case XBM:
      load_extended_burstmode_graph(design);
      break;
    case RSG:
      load_reduced_state_graph(design);
      break;
    case CSP:
    case HSE:
    case VHDL:
      if(!(compile(command,design,newfile) &&
           load_event_rule_system(menu,command,design,sifile,newfile))){
        problems++;
      }
      break;
    case TIMEDER:
    case TEL:
      load_event_rule_system(menu,command,design,sifile,newfile);
      break;
    case PRS:
      if (load_graph(design)) {
        if (load_event_rule_system(menu,command,design,sifile,newfile))
          load_ac_gate_file(menu,command,design);
      }
      /*
        synthesis(SYNTHESIS,FINDREG,design,sifile,newfile);
        if (design->status & FOUNDREG)
        load_production_rules(command,design);
      */
      break;
    case DATA:
      load_data(design->filename,design);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case STORE:
    switch (command) {
    case GRAPH:
      //      synthesis(SYNTHESIS,FINDRED,design,sifile,newfile);
      //if (design->nochecks || design->status & FOUNDRED)
      store_g(design->filename,design->signals,
              design->events,design->merges,design->rules,
              design->nevents,design->nplaces,design->ninputs,
              design->startstate,design->initval,design->initrate,
              design->level,design->fromG,design->fromLPN);
      break;
    case LPN:
      //      synthesis(SYNTHESIS,FINDRED,design,sifile,newfile);
      //if (design->nochecks || design->status & FOUNDRED)
      // ZHEN(Done): push design->assignments down to where it is printed
      store_lhpn(design->filename,design->signals,
                 design->events,design->merges,design->rules,
                 design->nevents,design->nplaces,design->ninputs,
                 design->startstate,design->initval,design->initrate,
                 design->level,design->fromG,design->fromLPN,
                 design->nsignals,design->nineqs,design->assignments);
      //load_lpn(design);
      design->fromLPN = FALSE;
      /*store_lhpn(design->filename,design->signals,
                 design->events,design->merges,design->rules,
                 design->nevents,design->nplaces,design->ninputs,
                 design->startstate,design->initval,design->initrate,
                 design->level,design->fromG,design->fromLPN,
                 design->nsignals,design->nineqs,design->assignments);*/
      break;
    case TIMEDER:
    case TEL:
      store_er(design->filename,design->events,design->rules,design->merges,
               design->nevents,design->ninputs,design->ncausal,design->nord,
               design->ndisj,design->nconf,design->startstate,design->level);
      break;
    case RSG:
      synthesis(SYNTHESIS,FINDRSG,design,sifile,newfile);
      if (design->status & FOUNDSTATES)
        store_state_graph(design->filename,design->signals,design->events,
			  design->state_table,
                          design->ninpsig,design->nsignals);
      break;
    case SG:
      print_sg(design->filename,design->signals,design->events,
               design->state_table,design->nsignals,TRUE,
               design->timeopts.genrg,design->nevents,design->ninpsig);
      break;
    case PRS:
      synthesis(SYNTHESIS,DOALL,design,sifile,newfile);
      break;
    case VHDL:
      synthesis(SYNTHESIS,DOALL,design,sifile,newfile);
      if (design->status & FOUNDCOVER) 
        store_struct_vhd(command,design);
      break;
    case TEL2VHD:
      tel_two_vhd(command,design);
      break;
    case VERILOG:
      synthesis(SYNTHESIS,DOALL,design,sifile,newfile);
      simulate(design->filename,design->signals,design->events,design->merges,
               design->rules,design->regions,design->ninpsig,design->ninputs,
               design->nsignals,design->nevents,design->startstate,design->gatelevel);
      break;
    case NET:
      synthesis(SYNTHESIS,DOALL,design,sifile,newfile);
      if (design->status & FOUNDCOVER)
        store_net(design->filename,design->signals,design->state_table,
                  design->regions,design->ninpsig,design->nsignals,
                  design->combo);
      break;
    case SPICE:
      synthesis(SYNTHESIS,DOALL,design,sifile,newfile);
      delaycalc(design->filename,design->signals,design->state_table,
                design->regions,design->ninpsig,design->nsignals,
                design->verbose);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case DISPLAY:
    switch (command) {
    case TIMEDER:
      if (design->status & LOADED) {
	if (design->dot)
	  dot_print_grf(design->filename,design->events,design->rules,
			design->nevents,TRUE);
	else 
	  print_grf(design->filename,design->events,design->rules,
		    design->nevents,TRUE,NULL);
      }
      break;
    case LPN:
      if (design->status & LOADED) {
	print_lpn(design->filename,design->signals,design->events,design->rules,
		  design->nevents,design->nplaces,TRUE,NULL,false,false,
		  design->dot,NULL);
      }
      break;
    case RSG:
      if (design->dot)
        dot_print_rsg(design->filename,design->signals,design->events,
                      design->state_table,design->nsignals,design->nevents,
                      TRUE);
      else {
        print_rsg(design->filename,design->signals,design->events,
                  design->state_table,design->nsignals,TRUE,
                  design->timeopts.genrg,design->nevents,design->nplaces); 
      }
      break;
    case RSG_WITH_STATS:
      if (design->dot)
        dot_print_rsg(design->filename,design->signals,design->events,
                      design->state_table,design->nsignals,design->nevents,
                      TRUE,TRUE);
      /*trans_dot_print_rsg(design->filename,design->signals,design->events,
        design->state_table,design->nsignals,
        design->nevents,TRUE,TRUE);*/
      else 
        print_rsg( design->filename,
                   design->signals,
                   design->events,
                   design->state_table,
                   design->nsignals,
                   TRUE,
                   design->timeopts.genrg,
                   design->nevents,design->nplaces,
                   TRUE); 
      break;
    case NET:
      draw_dot_net(command,design,FALSE);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case PRINT:
    switch (command) {
    case TIMEDER:
      if (design->status & LOADED) {
	if (design->dot)
	  dot_print_grf(design->filename,design->events,design->rules,
			design->nevents,FALSE); 
	else
	  print_grf(design->filename,design->events,design->rules,
		    design->nevents,FALSE,NULL);
      }
      break;
    case LPN:
      if (design->status & LOADED) {
	print_lpn(design->filename,design->signals,design->events,design->rules,
		  design->nevents,design->nplaces,FALSE,NULL,false,false,
		  design->dot,NULL);
      }
      break;
    case RSG:
      if (design->dot)
        dot_print_rsg(design->filename,design->signals,design->events,
                      design->state_table,design->nsignals,design->nevents,
                      FALSE);
      else 
        print_rsg(design->filename,design->signals,design->events,
                  design->state_table,design->nsignals,FALSE,
                  design->timeopts.genrg,design->nevents,design->nplaces); 
      break;
    case RSG_WITH_STATS:
      if ( design->dot )
        dot_print_rsg(design->filename,design->signals,design->events,
                      design->state_table,design->nsignals,design->nevents,
                      FALSE,TRUE);
      else
        print_rsg( design->filename,
                   design->signals,
                   design->events,
                   design->state_table,
                   design->nsignals,
                   FALSE,
                   design->timeopts.genrg,
                   design->nevents,design->nplaces,
                   TRUE ); 
      break;
    case REG:
      if (design->dot)
        dot_print_reg(design->filename,design->signals,design->state_table,
                      design->nsignals,FALSE);
      else
        print_reg(design->filename,design->signals,design->state_table,
                  design->nsignals,FALSE); 
      /* struct2net(design->filename,design->events,design->rules,design->nevents);*/
      break;
    case NET:
      draw_dot_net(command,design,TRUE);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case SYNTHESIS:
    switch (command) {
    case EXPAND:
    case REDUCE:
      handshaking_expansion(command,design);
      break;
    case FINDRED:
    case FINDRSG:
    case PROJECT:
    case FINDREG:
    case FINDVIOL:
    case FINDCOVER:
    case SINGLE:
      use_single(design);
      synthesis(menu,command,design,sifile,newfile);
      break;
    case MULTICUBE:
      multicube_cover(design);
      synthesis(menu,command,design,sifile,newfile);
      break;
    case USEBDD:
      use_bdd(design);
      synthesis(menu,command,design,sifile,newfile);
      break;
    case DIRECT:
      use_direct(design);
      synthesis(menu,command,design,sifile,newfile);
      break;
    case SYN:
      use_syn(design);
      synthesis(menu,command,design,sifile,newfile);
      break;
    case SIS:
      use_sis(command,design);
      synthesis(menu,command,design,sifile,newfile);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case EXCEPTIONS:
    switch (command) {
    case CONNECT:
    case ADDORD:
    case ADDPERS:
    case SOLVECSC:
    case FINDSV:
    case RESOLVE:
    case BREAKUP:
      synthesis(menu,command,design,sifile,newfile);
      break;
    case DECOMP:
      do_decomp(menu,command,design,sifile,newfile);
      break;
    case BIOMAP:
      do_biomap(menu,command,design,sifile,newfile);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case VERIFY:
    switch (command) {
    case VERORBITS:
      synthesis(SYNTHESIS,DOALL,design,sifile,newfile);
      if (design->status & STOREDPRS) {
        verify(design->filename,design->signals,design->events,design->merges,
               design->rules,design->cycles,design->regions,
               design->ninputs,design->nevents,design->ncycles,design->ninpsig,
               design->nsignals,design->startstate,design->si,design->verbose,
               design->gatelevel,design->combo,design->markkey,
               design->nrules,design->level,design->ndummy);
      }
      break;
    case VERGATE:
      synthesis(VERIFY,FINDRSG,design,sifile,newfile);
      if (design->status & FOUNDSTATES) 
        verify_gate_level(menu,command,design);
      break;
    case VERATACS:
      if (design->lhpnbdd) {
#ifdef CUDD
        symbolic(design);
#else
	printf("BDD library not available.\n");
	fprintf(lg,"BDD library not available.\n");
#endif
      } else if (design->lhpnsmt) {
#ifdef YICES
	smtSymbolic(design);
#else
	printf("SMT solver not available.\n");
	fprintf(lg,"SMT solver not available.\n");
#endif
      } else {
        synthesis(VERIFY,VERATACS,design,sifile,newfile);
      }
      break;
    case SEARCH:
      synthesis(VERIFY,SEARCH,design,sifile,newfile);
      break;
    case TRACE:
      synthesis(VERIFY,TRACE,design,sifile,newfile);
      break;
    case ANALYZE:
      synthesis(VERIFY,ANALYZE,design,sifile,newfile);
      break;
    case CHECKCTL:
      checkCTL(design->state_table,design->signals,design->nsignals,
               design->nevents,design->nplaces,design->events,design->rules,
               NULL);
      break;
    case VERVIJAY:
      satconf(design->filename); 
      break;
    case VERNATHAN:
      partial_order(design->filename,design->verbose,
                    design->timeopts.PO_reduce);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case ANALYSIS:
    switch (command) {
    case HPNSIM:
      hpn_simulate( design );
      break;
    case LHPNSIM:
      lhpn_simulate( design );
      break;
    case SIMTEL:
      monte_carlo_steady_state( design );
      break;
    case FINDTD:
      synthesis(SYNTHESIS,FINDRED,design,sifile,newfile);
      find_td(design->filename,design->events,design->rules,design->cycles,
              design->nevents,design->ncycles);
      break;
    case FINDWCTD:
      synthesis(SYNTHESIS,FINDRED,design,sifile,newfile);
      find_wctd(design->filename,design->events,design->rules,design->cycles,
                design->nevents,design->ncycles,design->closure);
      break;
    case TPRGLIN:
      rg_trans_prob( design );
      break;
    case TPSIM:
      monte_carlo_trans_prob( design );
      break;
    case TPEXP:
      exp_rate_trans_prob( design );
      break;
    case TRIGPROB:
      trigger_signal_probability( design );
      break;
    case DTMCRP:
      markov_reduced_power( design );
      break;
    case CTMC:
      continuous_time_markov_analysis( design );
      break;
    case MEDLEY:
      medley( design );
      break;
    case SIMCTMC:
      time_tracking_stochastic_simulation( design );
      break;
    case TPBURST:
      burst_mode_trans_prob( design );
      break;
    case CYCPER:
      cycle_period_analysis( design );
      break;
    case LTTPSIM:
      long_term_trans_prob_simulation( design );
      break;
    case SIMCYC:
      cycle_period_simulation( design );
      break;
    case TRIGSIM:
      simulate_trigger_probabilities( design );
      break;
    case PROFILE:
      cycle_period_unroll( design );
      break;
    case FINDSLACK:
#ifdef INTEL
      synthesis(SYNTHESIS,FINDRSG,design,sifile,newfile);
      if ((design->geometric || 
           design->pomaxdiff || 
           design->poapprox ||
           design->posets || 
           design->bag ||
           design->bap ) && (design->status & FOUNDSTATES)) {
        slack_chk(design->filename,design->signals,design->events,
                  design->merges,design->rules,design->cycles,
                  design->markkey,design->state_table,design->nevents,
                  design->ncycles,design->ninputs,design->nrules,
                  design->maxsize,
                  design->maxgatedelay,design->startstate,TRUE,NULL,
                  design->status,design->regions,design->manual,
                  design->ninpsig,design->nsignals,design->verbose);
      }
#else
      printf("Not available in this version.\n");
      fprintf(lg,"Not available in this version.\n");
#endif
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case HELP:
    help(selection);
    break;
  case TOLERANCE:
    printf("New tolerance is: %g\n",design->tolerance);
    fprintf(lg,"New tolerance is: %g\n",design->tolerance);
    break;
  case CYCLES:
    change_cycles(command,design);
    break;
  case LINKSIZE:
    change_linksize(command,design);
    break;
  case SIMTIME:
    change_simtime(command,design);
    break;
  case MAXSIZE:
    change_maxsize(design);
    break;
  case ITERATIONS:
    change_iterations(design);
    break;
  case GATEDELAY:
    change_gatedelay(command,design);
    break;
  case COMPOPT:
    switch (command) {
    case POSTPROC:
      use_postproc(design);
      break;
    case REDCHK:
      use_redchk(design);
      break;
    case NEWTAB:
      use_newtab(design);
      break;
    case XFORM2:
      use_xform2(design);
      break;
    case EXPANDRATE:
      use_expandrate(design);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case PRUNING:
    switch (command) {
    case NOT_FIRST:
      toggle(design->pruning.not_first,
             "Stopping concurrency reduction after the first solution");
      break;
    case PRESERVE:
      toggle(design->pruning.preserve,"Preserving user-specified concurrency");
      break;
    case ORDERED:
      toggle(design->pruning.ordered,
             "Pruning rules between events that are timed-ordered");
      break;
    case SUBSET:
      toggle(design->pruning.subset,
             "Assuming that a subset of rules implies a superset of states");
      break;
    case UNSAFE:
      toggle(design->pruning.unsafe,
             "Pruning TELs with rules into some but not all branches of a choice");
      break;
    case EXPENSIVE:
      toggle(design->pruning.expensive,
             "Using branch and bound to prune expensive solutions");
      break;
    case CONFLICT:
      toggle(design->pruning.conflict,
             "Pruning rules whose enabling and enabled events conflict");
      break;
    case REACHABLE:
      toggle(design->pruning.reachable,
             "Pruning redundant rules according to reachability");
      break;
    case DUMB:
      toggle
        (design->pruning.dumb,
         "Pruning rules to dummy events or from non-level-enabled dummy events"
         );
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case TIMING:
    switch (command) {
    case SI:
      declare_si(command,design);
      break;
    case UNTIMED:
      use_untimed(command,design);
      break;
    case COMPRESS:
      use_compress(command,design);
      break;
    case HEURISTIC:
      use_heuristic(command,design);
      break;
    case ORBITS:
      use_orbits(command,design);
      break;
    case GEOMETRIC:
      use_geometric(command,design);
      break;
    case SL:
      use_srl(command,design);
      break;
    case HPN:
      use_hpn(command,design);
      break;
    case LHPNBDD:
      use_lhpnbdd(command,design);
      break;
    case LHPNDBM:
      use_lhpndbm(command,design);
      break;
    case LHPNSMT:
      use_lhpnsmt(command,design);
      break;
    case BAG:
      use_bag(command,design);
      break;
    case BAP:
      use_bap(command,design);
      break;
    case BAPTDC:
      use_bap_tdc(command,design);
      break;
    case APPROX:
      use_poapprox(command,design);
      break;
    case SETS:
      use_posets(command,design);
      break;
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
    break;
  case OPTIONS:
    switch (command) {
    case NOPARG:
      toggle_PARG_output(design);
      break;
    case NOCHECKS:
      toggle_checks(design);
      break;
    case EXACT:
      exact_resolution(design);
      break;
    case DOTPARG:
      use_dot(design);
      break;
    case SHAREGATE:
      share_gates(design);
      break;
    case DISABLE:
      use_disabling(command,design);
      break;
    case NOFAIL:
      use_nofail(command,design);
      break;
    case NOPROJ:
      use_noproj(command,design);
      break;
    case KEEPGOING:
      use_keepgoing(command,design);
      break;
    case SUBSETS:
      use_subsets(command,design);
      break;
    case SUPERSETS:
      use_supersets(command,design);
      break;
    case INTERLEAV:
      use_interleav(command,design);
      break;
    case PRUNE:
      use_prune(command,design);
      break;
    case INFOPT:
      use_infopt(command,design);
      break;
    case ORBMATCH:
      use_orbmatch(command,design);
      break;
    case ABSTRACT:
      use_abstraction(design);
      break;
    case PORED:
      use_pored(command,design);
      break;
    case EXPLPN:
      use_lpn(command,design);
      break;
    case GENRG:
      gen_reg_graph(command,design);
      break;
    case INPONLY:
      use_inponly(design);
      break;
    case COMBO:
      use_combo(design);
      break;
    case ATOMIC:
      atomicGate(design);
      break;
    case GENC:
      generalizedC(design);
      break;
    case GATELEVEL:
      gate_level(design);
      break;
    case BURSTGC:
      burstgc(design);
      break;
    case BURST2L:
      burst2l(design);
      break;
    case MANUAL:
      manual_breakup(design);
      break;
    case EXCEPTION:
      handle_exceptions(design);
      break;
    case VERBOSE:
      verbose(design);
      break;
    case REDUCTION:
      toggle(design->reduction,"Automatic concurrency reduction");
      break;
    case INSERTION:
      toggle(design->insertion,"Automatic state-variable insertion");
      break;
    case MININS:
      toggle(design->minins,"Limited transition point size");
      break;
#ifdef TSE_ALGM
    case TSE:
      tse(design);
      break;
#endif
    default:
      printf("INVALID COMMAND\n"); 
      break;
    }
  case QUIT:
    break;
  default:
    printf("INVALID COMMAND\n"); 
    break;
  }
  return(design->status);
}

/*****************************************************************************/
/* If a filename ends with ".vhd" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_vhdl(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='v') &&
      (filename[length-2]=='h') && (filename[length-1]=='d')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".csp" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_csp(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='c') &&
      (filename[length-2]=='s') && (filename[length-1]=='p')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".hse" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_hse(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='h') &&
      (filename[length-2]=='s') && (filename[length-1]=='e')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".tel" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_tel(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='t') &&
      (filename[length-2]=='e') && (filename[length-1]=='l')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".er" then strip it off.                          */
/*****************************************************************************/

bool trim_filename_er(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 3) && (filename[length-3]=='.') && (filename[length-2]=='e') &&
      (filename[length-1]=='r')) {
    filename[length-3]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".g" then strip it off.                           */
/*****************************************************************************/

bool trim_filename_g(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 2) && (filename[length-2]=='.') && (filename[length-1]=='g')){
    filename[length-2]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".lpn" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_lpn(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='l') &&
      (filename[length-2]=='p') && (filename[length-1]=='n')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".unc" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_unc(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='u') &&
      (filename[length-2]=='n') && (filename[length-1]=='c')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".rsg" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_rsg(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='r') &&
      (filename[length-2]=='s') && (filename[length-1]=='g')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* If a filename ends with ".rsg" then strip it off.                         */
/*****************************************************************************/

bool trim_filename_data(char * filename)
{
  int length;

  length=strlen(filename);
  if ((length > 4) && (filename[length-4]=='.') && (filename[length-3]=='d') &&
      (filename[length-2]=='a') && (filename[length-1]=='t')) {
    filename[length-4]='\0';
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************/
/* Determine input file type.                                                */
/*****************************************************************************/

void find_file_type(designADT design,char menu,char command)
{
  FILE *fp;
  char infile[MAXTOKEN];

  design->fromCSP=FALSE;
  design->fromVHDL=FALSE;
  design->fromHSE=FALSE;
  design->fromER=FALSE;
  design->fromTEL=FALSE;
  design->fromG=FALSE;
  design->fromRSG=FALSE;
  design->fromUNC=FALSE;
  design->fromDATA=FALSE;
  if ((trim_filename_vhdl(design->filename)) || 
      ((menu==LOAD) && (command==VHDL)))
    design->fromVHDL=TRUE;
  else if ((trim_filename_csp(design->filename)) || 
	   ((menu==LOAD) && (command==CSP)))
    design->fromCSP=TRUE;
  else if ((trim_filename_hse(design->filename)) || 
	   ((menu==LOAD) && (command==HSE)))
    design->fromHSE=TRUE;
  else if ((trim_filename_g(design->filename)) ||
	   (menu==LOAD) && (command==GRAPH))
    design->fromG=TRUE;
  else if ((trim_filename_lpn(design->filename)) ||
	   (menu==LOAD) && (command==LPN))
    design->fromLPN=TRUE;
  else if ((trim_filename_unc(design->filename)) ||
	   (menu==LOAD) && (command==XBM))
    design->fromUNC=TRUE;
  else if ((trim_filename_er(design->filename)) || 
	   ((menu==LOAD) && (command==TIMEDER))) 
    design->fromER=TRUE;
  else if ((trim_filename_tel(design->filename)) || 
	   ((menu==LOAD) && (command==TEL))) 
    design->fromTEL=TRUE;
  else if ((trim_filename_rsg(design->filename)) ||
      (menu==LOAD) && (command==RSG))
    design->fromRSG=TRUE;
  else if ((trim_filename_data(design->filename)) ||
           (menu==LOAD) && (command==DATA))
    design->fromDATA=TRUE;
  else {
    sprintf(infile,"%s.vhd",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromVHDL=TRUE;
      return;
    }
    sprintf(infile,"%s.csp",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromCSP=TRUE;
      return;
    }
    sprintf(infile,"%s.hse",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromHSE=TRUE;
      return;
    }
    sprintf(infile,"%s.g",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromG=TRUE;
      return;
    }
    sprintf(infile,"%s.lpn",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromLPN=TRUE;
      return;
    }
    sprintf(infile,"%s.unc",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromUNC=TRUE;
      return;
    }
    sprintf(infile,"%s.er",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromER=TRUE;
      return;
    }
    sprintf(infile,"%s.tel",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromTEL=TRUE;
      return;
    }
    sprintf(infile,"%s.rsg",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromRSG=TRUE;
      return;
    }
    sprintf(infile,"%s.dat",design->filename);
    if ((fp=fopen(infile,"r"))!=NULL) {
      fclose(fp);
      design->fromDATA=TRUE;
      return;
    }
  }
}

/*****************************************************************************/
/* Get commands from the user.                                               */
/*****************************************************************************/

char * get_command(designADT design,char * selection,bool *newfile)
{
  static int oldmingatedelay=MINGATEDELAY;
  static int oldmaxgatedelay=MAXGATEDELAY;
  int i,token,index;
  char tokvalue[MAXARG][MAXTOKEN];
  bool shell;
  char shellcommand[100];
  char * menu_command;

  printf("atacs> ");
  index=0;
  shell=FALSE;
  menu_command=(char *)GetBlock(4 * sizeof(char));
  menu_command[0]='\0';
  menu_command[1]='\0';
  menu_command[2]='\0';
  shellcommand[0]='\0';
  (*newfile)=FALSE;
  while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE) 
    if ((token==WORD) && (index < MAXARG)) index++;
    else if (token==SHELL) shell=TRUE;
  if (index > 0) {
    if (shell) {
      for (i=0;i<index;i++) {
	strcat(shellcommand,tokvalue[i]);
	strcat(shellcommand," ");
      }
      system(shellcommand);
      return(menu_command);
    }
    else if (strcmp(tokvalue[0],"cd")==0) {
      if (index==1) {
	printf("New pathname: ");
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
    if (index > 1)   
      if (chdir(tokvalue[1]) != 0) 
	printf("%s: No such file or directory.\n",tokvalue[1]); 
      return(menu_command);
    } else if (strcmp(tokvalue[0],"help")==0) {
      if (index > 1) strcpy(selection,tokvalue[1]);
      else selection[0]='\0';
      menu_command[0]=HELP;
      menu_command[1]=HELP;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"cycles")==0) {
      if (index==1) {
	printf("Number of Cycles (%d): ",design->ncycles);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) design->ncycles=atoi(tokvalue[1]);
      else return(menu_command);
      menu_command[0]=CYCLES;
      menu_command[1]=CYCLES;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"simtime")==0) {
      if (index==1) {
	printf("Number of Simulation Cycles (%d): ",design->simtime);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) design->simtime=atoi(tokvalue[1]);
      else return(menu_command);
      menu_command[0]=SIMTIME;
      menu_command[1]=SIMTIME;
      return(menu_command);
#ifdef DLONG
    } else if (strcmp(tokvalue[0],"linksize")==0) {
      if (index==1) {
	printf("Number of BDD link bits (%d): ",design->bdd_state->linksize);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) {
	design->bdd_state->linksize=atoi(tokvalue[1]);
	design->bdd_state->maxlinks=1<<design->bdd_state->linksize;
      }
      else return(menu_command);
      menu_command[0]=LINKSIZE;
      menu_command[1]=LINKSIZE;
      return(menu_command);
#endif
    } else if (strcmp(tokvalue[0],"tolerance")==0) {
      if (index==1) {
	printf("Tolerance for convergence checks (%g): ",design->tolerance);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) design->tolerance=atof(tokvalue[1]);
      else return(menu_command);
      menu_command[0]=TOLERANCE;
      menu_command[1]=TOLERANCE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"maxsize")==0) {
      if (index==1) {
	printf("Maximum number of transistors in a stack (%d): ",
	       design->maxsize);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) design->maxsize=atoi(tokvalue[1]);
      else return(menu_command);
      menu_command[0]=MAXSIZE;
      menu_command[1]=MAXSIZE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"iterations")==0) {
      if (index==1) {
	printf("Number iterations for the SMT solver (%d): ",
	       design->iterations);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) design->iterations=atoi(tokvalue[1]);
      else return(menu_command);
      menu_command[0]=ITERATIONS;
      menu_command[1]=ITERATIONS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"gatedelay")==0) {
      if (index==1) {
	printf("Minimum gate delay (%d): ",design->mingatedelay);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index==2) {
	if (design->maxgatedelay==INFIN) 
	  printf("Maximum gate delay (infinity): ");
	else printf("Maximum gate delay (%d): ",design->maxgatedelay);
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE)
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 1) {
	design->mingatedelay=atoi(tokvalue[1]);
	oldmingatedelay=design->mingatedelay;
	if (index > 2) {
	  if ((strcmp(tokvalue[2],"inf")==0) || 
	      (strcmp(tokvalue[2],"infinity")==0))
	    design->maxgatedelay=INFIN;
	  else design->maxgatedelay=atoi(tokvalue[2]);
	  oldmaxgatedelay=design->maxgatedelay;
	}
      } else return(menu_command);
      menu_command[0]=GATEDELAY;
      menu_command[1]=GATEDELAY;
      return(menu_command);
    } 
    else if (strcmp(tokvalue[0],"si")==0) {
      oldmingatedelay=design->mingatedelay;
      design->mingatedelay=0;
      oldmaxgatedelay=design->maxgatedelay;
      design->maxgatedelay=INFIN;
      menu_command[0]=TIMING;
      menu_command[1]=SI;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"untimed")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=UNTIMED;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"compress")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=COMPRESS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"heuristic")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=HEURISTIC;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"orbits")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=ORBITS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"geometric")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=GEOMETRIC;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"srl")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=SL;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"hpn")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=HPN;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"lhpndbm")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=LHPNDBM;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"lhpnbdd")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=LHPNBDD;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"lhpnsmt")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=LHPNSMT;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"poapprox")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=APPROX;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"posets")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=SETS;
      return(menu_command);
      // egm 08/30/00 Bourne Again Geometric
    } else if (strcmp(tokvalue[0],"bag")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=BAG;
      return( menu_command );
      // egm 11/15/00 Bourne Again POSET Timing
    } else if (strcmp(tokvalue[0],"bap")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=BAP;
      return( menu_command );
      // egm 04/12/00 Bourne Again POSET Timing with TDC
    } else if (strcmp(tokvalue[0],"baptdc")==0) {
      menu_command[0]=TIMING;
      menu_command[1]=BAPTDC;
      return( menu_command );
    } else if (strcmp(tokvalue[0],"single")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=SINGLE;
    } else if (strcmp(tokvalue[0],"expand")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=EXPAND;
    } else if (strcmp(tokvalue[0],"multicube")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=MULTICUBE;
    } else if (strcmp(tokvalue[0],"direct")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=DIRECT;
    } else if (strcmp(tokvalue[0],"bdd")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=USEBDD;
    } else if (strcmp(tokvalue[0],"syn")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=SYN;
    } else if (strcmp(tokvalue[0],"sis")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=SIS;
    } else if (strcmp(tokvalue[0],"noparg")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=NOPARG;
      return( menu_command );
    } else if (strcmp(tokvalue[0],"nochecks")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=NOCHECKS;
      return( menu_command );
    } else if (strcmp(tokvalue[0],"combo")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=COMBO;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"abstract")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=ABSTRACT;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"postproc")==0) {
      menu_command[0]=COMPOPT;
      menu_command[1]=POSTPROC;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"redchk")==0) {
      menu_command[0]=COMPOPT;
      menu_command[1]=REDCHK;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"xform2")==0) {
      menu_command[0]=COMPOPT;
      menu_command[1]=XFORM2;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"newtab")==0) {
      menu_command[0]=COMPOPT;
      menu_command[1]=NEWTAB;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"not_first")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=NOT_FIRST;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"preserve")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=PRESERVE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"ordered")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=ORDERED;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"subset")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=SUBSET;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"unsafe")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=UNSAFE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"expensive")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=EXPENSIVE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"conflict")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=CONFLICT;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"reachable")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=REACHABLE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"dumb")==0) {
      menu_command[0]=PRUNING;
      menu_command[1]=DUMB;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"inponly")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=INPONLY;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"atomic")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=ATOMIC;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"gc")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=GENC;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"gatelevel")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=GATELEVEL;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"burstgc")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=BURSTGC;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"burst2l")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=BURST2L;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"exact")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=EXACT;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"dot")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=DOTPARG;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"sharegate")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=SHAREGATE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"disabling")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=DISABLE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"nofail")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=NOFAIL;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"noproj")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=NOPROJ;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"keepgoing")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=KEEPGOING;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"subsets")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=SUBSETS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"supersets")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=SUPERSETS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"infopt")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=INFOPT;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"orbmatch")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=ORBMATCH;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"pored")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=PORED;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"interleav")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=INTERLEAV;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"prune")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=PRUNE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"genrg")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=GENRG;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"manual")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=MANUAL;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"exception")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=EXCEPTION;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"verbose")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=VERBOSE;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"reduction")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=REDUCTION;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"insertion")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=INSERTION;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"minins")==0) {
      menu_command[0]=OPTIONS;
      menu_command[1]=MININS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"storeer")==0) {
      if (index > 1) strcpy(design->filename,tokvalue[1]);
      menu_command[0]=STORE;
      menu_command[1]=TIMEDER;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"storeg")==0) {
      menu_command[0]=STORE;
      menu_command[1]=GRAPH;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"storelpn")==0) {
      menu_command[0]=STORE;
      menu_command[1]=LPN;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"shower")==0) {
      menu_command[0]=DISPLAY;
      menu_command[1]=TIMEDER;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"showlpn")==0) {
      menu_command[0]=DISPLAY;
      menu_command[1]=LPN;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"showrsg")==0) {
      menu_command[0]=DISPLAY;
      menu_command[1]=RSG;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"showstat")==0) {
      menu_command[0]=DISPLAY;
      menu_command[1]=RSG_WITH_STATS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"shownet")==0) {
      menu_command[0]=DISPLAY;
      menu_command[1]=NET;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"printer")==0) {
      menu_command[0]=PRINT;
      menu_command[1]=TIMEDER;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"printlpn")==0) {
      menu_command[0]=PRINT;
      menu_command[1]=LPN;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"printrsg")==0) {
      menu_command[0]=PRINT;
      menu_command[1]=RSG;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"printstat")==0) {
      menu_command[0]=PRINT;
      menu_command[1]=RSG_WITH_STATS;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"printreg")==0) {
      menu_command[0]=PRINT;
      menu_command[1]=REG;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"printnet")==0) {
      menu_command[0]=PRINT;
      menu_command[1]=NET;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"quit")==0 || strcmp(tokvalue[0],"q")==0) {
      menu_command[0]=QUIT;
      return(menu_command);
    } else if (strcmp(tokvalue[0],"loadcsp")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=CSP;
    } else if (strcmp(tokvalue[0],"loadhse")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=HSE;
    } else if (strcmp(tokvalue[0],"loadvhdl")==0 ||
	       strcmp(tokvalue[0],"loadvhd")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=VHDL;
    } else if (strcmp(tokvalue[0],"loader")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=TIMEDER;
    } else if (strcmp(tokvalue[0],"loadtel")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=TEL;
    } else if (strcmp(tokvalue[0],"loadrsg")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=RSG;
    } else if (strcmp(tokvalue[0],"loadxbm")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=XBM;
    } else if (strcmp(tokvalue[0],"loadg")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=GRAPH;
    } else if (strcmp(tokvalue[0],"loadlpn")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=LPN;
    } else if (strcmp(tokvalue[0],"loadprs")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=PRS;
    } else if (strcmp(tokvalue[0],"storersg")==0) {
      menu_command[0]=STORE;
      menu_command[1]=RSG;
    } else if (strcmp(tokvalue[0],"storesg")==0) {
      menu_command[0]=STORE;
      menu_command[1]=SG;
    } else if (strcmp(tokvalue[0],"storeprs")==0) {
      menu_command[0]=STORE;
      menu_command[1]=PRS;
    } else if (strcmp(tokvalue[0],"storevhdl")==0 ||
	       strcmp(tokvalue[0],"storevhd")==0) {
      menu_command[0]=STORE;
      menu_command[1]=VHDL;
    } else if (strcmp(tokvalue[0],"storevlog")==0) {
      menu_command[0]=STORE;
      menu_command[1]=VERILOG;
    } else if (strcmp(tokvalue[0],"tel2vhdl")==0 ||
	       strcmp(tokvalue[0],"tel2vhd")==0) {
      menu_command[0]=STORE;
      menu_command[1]=TEL2VHD;
    } else if (strcmp(tokvalue[0],"storenet")==0) {
      menu_command[0]=STORE;
      menu_command[1]=NET;
    } else if (strcmp(tokvalue[0],"storehsp")==0) {
      menu_command[0]=STORE;
      menu_command[1]=SPICE;
    } else if (strcmp(tokvalue[0],"findred")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=FINDRED;
    } else if (strcmp(tokvalue[0],"hse")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=REDUCE;
    } else if (strcmp(tokvalue[0],"kiss2sg")==0) {
      menu_command[0]=LOAD;
      menu_command[1]=KISS2SG;
    } else if (strcmp(tokvalue[0],"findrsg")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=FINDRSG;
    } else if (strcmp(tokvalue[0],"project")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=PROJECT;
    } else if (strcmp(tokvalue[0],"findreg")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=FINDREG;
    } else if (strcmp(tokvalue[0],"findviol")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=FINDVIOL;
    } else if (strcmp(tokvalue[0],"findcover")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=FINDCOVER;
    } else if (strcmp(tokvalue[0],"doall")==0) {
      menu_command[0]=SYNTHESIS;
      menu_command[1]=DOALL;
    } else if (strcmp(tokvalue[0],"connect")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=CONNECT;
    } else if (strcmp(tokvalue[0],"addord")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=ADDORD;
    } else if (strcmp(tokvalue[0],"addpers")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=ADDPERS;
    } else if (strcmp(tokvalue[0],"solvecsc")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=SOLVECSC;
    } else if (strcmp(tokvalue[0],"findsv")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=FINDSV;
    } else if (strcmp(tokvalue[0],"resolve")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=RESOLVE;
    } else if (strcmp(tokvalue[0],"breakup")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=BREAKUP;
    } else if (strcmp(tokvalue[0],"decomp")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=DECOMP;
    } else if (strcmp(tokvalue[0],"biomap")==0) {
      menu_command[0]=EXCEPTIONS;
      menu_command[1]=BIOMAP;
    } else if (strcmp(tokvalue[0],"search")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=SEARCH;
    } else if (strcmp(tokvalue[0],"analyze")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=ANALYZE;
    } else if (strcmp(tokvalue[0],"verify")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=VERATACS;
    } else if (strcmp(tokvalue[0],"trace")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=TRACE;
    } else if (strcmp(tokvalue[0],"vergate")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=VERGATE;
    } else if (strcmp(tokvalue[0],"verorbits")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=VERORBITS;
    } else if (strcmp(tokvalue[0],"checkctl")==0) {
      menu_command[0]=VERIFY;
      menu_command[1]=CHECKCTL;
    } else if (strcmp(tokvalue[0],"hpnsim")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=HPNSIM;
    } else if (strcmp(tokvalue[0],"simtel")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=SIMTEL;
    } else if (strcmp(tokvalue[0],"findtd")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=FINDTD;
    } else if (strcmp(tokvalue[0],"findwctd")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=FINDWCTD;
    } else if (strcmp(tokvalue[0],"findslack")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=FINDSLACK;
    } else if (strcmp(tokvalue[0],"tpsim")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=TPSIM;
    } else if (strcmp(tokvalue[0],"tprglin")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=TPRGLIN;
    } else if (strcmp(tokvalue[0],"tpexp")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=TPEXP;
    } else if (strcmp(tokvalue[0],"trigprob")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=TRIGPROB;
    } else if (strcmp(tokvalue[0],"dtmcrp")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=DTMCRP;
    } else if (strcmp(tokvalue[0],"ctmc")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=CTMC;
    } else if (strcmp(tokvalue[0],"medley")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=MEDLEY;
    } else if (strcmp(tokvalue[0],"simctmc")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=SIMCTMC;
    } else if (strcmp(tokvalue[0],"tpburst")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=TPBURST;
    } else if (strcmp(tokvalue[0],"cycper")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=CYCPER;
    } else if (strcmp(tokvalue[0],"lttpsim")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=LTTPSIM;
    } else if (strcmp(tokvalue[0],"simcyc")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=SIMCYC;
    } else if (strcmp(tokvalue[0],"trigsim")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=TRIGSIM;
    } else if (strcmp(tokvalue[0],"profile")==0) {
      menu_command[0]=ANALYSIS;
      menu_command[1]=PROFILE;
    } else {
      for (i=0;i<index;i++) {
	strcat(shellcommand,tokvalue[i]);
	strcat(shellcommand," ");
      }
      system(shellcommand);
      return(menu_command);
    }
    if ((menu_command[0]==LOAD) || (!(design->status & LOADED)) ||
	(index > 1)) {
      while (index==1) {
	printf("Filename: ");
	while ((token=gettok(tokvalue[index],MAXTOKEN)) != END_OF_LINE) 
	  if ((token==WORD) && (index < MAXARG)) index++;
      }
      if (index > 2)
	strcpy(design->component,tokvalue[2]);
      else
	design->component[0]='\0';
      (*newfile)=TRUE;
      design->mingatedelay=oldmingatedelay;
      design->maxgatedelay=oldmaxgatedelay;
      strcpy(design->filename,tokvalue[1]);
      find_file_type(design,menu_command[0],menu_command[1]);
    }
  }
  return(menu_command);
}

/*****************************************************************************/
/* Run user interface.                                                       */
/*****************************************************************************/

void user_interface(void)
{
  char * menu_command;
  char nextmenu,nextcommand;
  char selection[MAXTOKEN];
  bool newfile;

//  status=RESET;
//  design=initialize_design();
  menu_command=get_command(design,selection,&newfile); 
  nextmenu=menu_command[0];
  nextcommand=menu_command[1];
  free(menu_command);
  while (nextmenu != QUIT) {
    if ((nextmenu != '\0') && (newfile==FALSE) && (status==RESET) &&
	(nextmenu != HELP) && (nextmenu != MAXSIZE) &&
	(nextmenu != ITERATIONS) &&
	(nextmenu != GATEDELAY) && (nextmenu != CYCLES) && 
	(nextmenu != SIMTIME) && 
	(nextmenu != TOLERANCE) && (nextmenu != LINKSIZE) && 
#ifdef TSE_ALGM
	(nextmenu != TSE) &&
#endif
	(nextmenu != OPTIONS) && (nextmenu != COMPOPT) &&
	(nextmenu != TIMING) && (nextmenu != PRUNING))
      printf("ERROR:  Need to load file first!\n"); 
    else if (nextmenu != '\0') {
      status=process_command(nextmenu,nextcommand,design,selection,newfile,
			     FALSE);
    }
    menu_command=get_command(design,selection,&newfile);
    nextmenu=menu_command[0];
    nextcommand=menu_command[1];
    free(menu_command);
  } 
}

/*****************************************************************************/
/* Run commands given in the command line.                                   */
/*****************************************************************************/

void process_arguments(int argc,char *argv[])
{
  char *options;
  char **newargv;
  char **head;
  unsigned i;
  int j,newargc;
  bool newfile,success;
  int value;
  char nextmenu,nextcommand;
  char tol[MAXTOKEN];

//  design=initialize_design();
  newargv = (char **)GetBlock((argc+1) * sizeof(char *));
  head=newargv;
  options = processoptions(argv,argc,newargv,&newargc);
  success=TRUE;
  if ((strlen(options) > 1) && (options[0]=='h') && (options[1]=='l')) 
    printman();
  else if ((strlen(options) > 0) && (options[0]=='h')) printman_short();
  else {
    while (newargc-- > 2) {
      strcpy(design->filename,*newargv++);
      find_file_type(design,QUIT,QUIT);
      if (!(success=compile(LOAD,design,TRUE)))
	break;
    }
    if (success) {
      if (newargc >= 0)
	strcpy(design->filename,*newargv++);
      if (newargc-- > 0)
	strcpy(design->component,*newargv++);
      else
	design->component[0]='\0';
      find_file_type(design,QUIT,QUIT);
      newfile=TRUE;
      //	design->status=RESET;
      design->initval=NULL;
      design->initrate=NULL;
      design->ncycles=NUMCYCLES;
      design->simtime=MAXSIMTIME;
      design->maxsize=MAXSTACKSIZE;
      design->iterations=ITERATIONDEF;
      design->mingatedelay=MINGATEDELAY;
      design->maxgatedelay=MAXGATEDELAY;
#ifdef DLONG
      design->bdd_state->linksize=LOG_MAX_REGIONS;
      design->bdd_state->maxlinks=MAX_REGIONS;
#endif
      design->tolerance=INIT_TOL;
      for (i=0;i<strlen(options);i++) 
	if (isalpha(options[i])) {
	  nextmenu=options[i];
	  if ((nextmenu==CYCLES) || (nextmenu==MAXSIZE) ||
	      (nextmenu==ITERATIONS) ||
	      (nextmenu==SIMTIME) ||
	      (nextmenu==LINKSIZE) || (nextmenu==GATEDELAY)) {
	    nextcommand=nextmenu;
	    value=0;
	    for (j=i+1;isdigit(options[j]);j++) 
	      value=value*10+(options[j]-'0');
	    switch(nextmenu) {
	    case CYCLES: 
	      design->ncycles=value;
	      break;
	    case SIMTIME: 
	      design->simtime=value;
	      break;
	    case MAXSIZE:
	      design->maxsize=value;
	      break;
	    case ITERATIONS:
	      design->iterations=value;
	      break;
	    case GATEDELAY:
	      design->mingatedelay=value;
	      value=0;
	      for (j=j+1;isdigit(options[j]);j++) 
		value=value*10+(options[j]-'0');
	      design->maxgatedelay=value;
	      break;
	    case LINKSIZE:
#ifdef DLONG
	      design->bdd_state->linksize=value;
	      design->bdd_state->maxlinks=1<<value;
#endif
		break;
	      }
	    } else if (nextmenu==TOLERANCE) {
	      nextcommand=nextmenu;
	      for (j=0;isdigit(options[j+i+1]) || options[j+i+1]=='.' ||
		     options[j+i+1]=='-' || options[j+i+1]=='e' ||
		     options[j+i+1]=='E';j++)
		tol[j]=options[j+i+1];
	      tol[j]='\0';
	      design->tolerance=atof(tol);
	      i=j+i;
	      printf("New tolerance is %g\n",design->tolerance);
	      fprintf(lg,"New tolerance is %g\n",design->tolerance);
	    } else 
	      if (((nextmenu==LOAD)||(nextmenu==STORE)||(nextmenu==DISPLAY)||
		   (nextmenu==PRINT)||(nextmenu==SYNTHESIS)||
		   (nextmenu==EXCEPTIONS)||(nextmenu==VERIFY)||
		   (nextmenu==ANALYSIS)||(nextmenu==OPTIONS)||
		   (nextmenu==COMPOPT)||(nextmenu==TIMING)||
		   (nextmenu==PRUNING)) &&
		  ((i+1)<strlen(options))) {
		i++;
		nextcommand=options[i];
	      } else {
		printf("INVALID OPTION %c\n",nextmenu);
		return;
	      }
	    if ((design->status==RESET) && (nextmenu != HELP) && 
		(nextmenu != MAXSIZE) && (nextmenu != ITERATIONS) && 
		(nextmenu != GATEDELAY) && 
		(nextmenu != TOLERANCE) && (nextmenu != LINKSIZE) && 
		(nextmenu != CYCLES) && (nextmenu != SIMTIME) && 
#ifdef TSE_ALGM
	      (nextmenu != TSE) &&
#endif
		(nextmenu != OPTIONS) && (nextmenu != COMPOPT) && 
		(nextmenu != TIMING) && (nextmenu != PRUNING) &&
		(newfile==FALSE))
	      printf("ERROR:  Need to load file first!\n"); 
	    else  {
	      status=process_command(nextmenu,nextcommand,design,NULL,
				     newfile,FALSE);
	      if ((design->status != RESET) && (nextmenu != HELP) &&
		  (nextmenu != MAXSIZE) && (nextmenu != ITERATIONS) && 
		  (nextmenu != GATEDELAY) &&
		  (nextmenu != LINKSIZE) && (nextmenu != TOLERANCE) &&
		  (nextmenu != CYCLES) && (nextmenu != OPTIONS) && 
		  (nextmenu != COMPOPT) && (nextmenu != TIMING) &&
		  (nextmenu != PRUNING) && (nextmenu != SIMTIME))
		newfile=FALSE;
	    }
	  }
      }
  }
  free(options);
  free(head);
}
  
#ifdef XIF

/****************************************************************************/
/* main function which tries to solve state assignment problems.            */
/****************************************************************************/
static int X_findsv(ClientData clientData, Tcl_Interp *interp,
		    int argc, const char *argv[])
{
  //  bool csc_present;
  bool state_graph_empty=true;
  int  i;
  stateADT curstate;
  bool success=false;
  int initialCausedViolations, initialNaturalViolations;
  int causedViolations, naturalViolations;
  int variableNumber = 0;
  bool punt(false); // not making progress, give up
  int initialCost;
  int userSelection;
  char *ip;
	
  freopen("atacs.log","w",lg);
  // check that CSC isn't already present
  initialCost = countCSCViolations(design->state_table,
				   design->ninpsig,
				   initialNaturalViolations,
				   initialCausedViolations);

  // create a textTel for the current tel
  static CTextTel textTel(design->nevents,design->events, 
			  design->ninputs, design->ncausal,design->rules,
			  design->merges, design->startstate, 
			  design->ndisj, design->nconf, design->nord);
      
  // create a container for the reversion and the best solutions
  static CBestSolutions bestSolutions(initialCost, 30, textTel, 
				      design->events); 
      
  if (argc <= 1) {
    // first make sure the rsg has been found
    for (i=0;i<PRIME;i++)
      for (curstate=design->state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link)
	{ 
	  state_graph_empty=FALSE;
	}
    if (state_graph_empty)
      {
	printf("ERROR: No state graph present.\n");
	fprintf(lg,"ERROR: No state graph present.\n");
	fflush(lg);
	return TCL_OK;
      }
    if (initialCost == 0)
      {
	printf("Graph has no CSC violations.\n");
	fprintf(lg,"Graph has no CSC violations.\n");
	fflush(lg);
	return TCL_OK;
      }

    // the game is afoot
    // keep adding variables until no CSCV's remain or we give up

    if ((countCSCViolations(design->state_table, // still violations
			    design->ninpsig,
			    naturalViolations,
			    causedViolations) != 0)
	&& !punt) // still hope */
      {
	success=find_sv(design->filename,design->signals,design->events,
			design->merges,design->rules,design->markkey,
			design->cycles, design->ncycles,
			design->state_table,design->nevents,design->ninputs,
			design->ncausal, // used to be nrules
			design->maxgatedelay,design->startstate,
			design->status,design->regions,design->ninpsig,
			design->nsignals,
			design->ndisj, design->nconf, design->nord,
			design->verbose, naturalViolations,
			variableNumber, !(design->untimed),
			bestSolutions,design->minins);
	
	// if a strange error occurred, exit findsv (CURRENTLY BOGUS)
	if (! success)
	  {
	    printf("ERROR: Unable to solve CSC -- Possible error.\n");
	    fprintf(lg,"ERROR: Unable to solve CSC -- Possible error.\n");
	    fflush(lg);
	    return TCL_OK;
	  }
	
	// SELECT THE BEST SOLUTION
	if (design->manual) {
	  int solNumber(0);
	  for (solution_list::iterator i_sol = 
		 bestSolutions.allSolutions().begin();
	       i_sol != bestSolutions.allSolutions().end(); i_sol++,
		 solNumber++)
	    {
	      ip = (char *)GetBlock(255*sizeof(char));
	      sprintf(ip,"%d %s",solNumber,
		      insertionString(design->events, *i_sol).c_str());
/* + " : " + (*i_sol).naturalViolations + "+" + 
	      (*i_sol).causedViolations + " [" + (*i_sol).initialValue + "]";*/
	      Tcl_AppendElement(interp,ip);
	    }
	  fflush(lg);
	  return TCL_OK;
	} else 
	  userSelection=0;
      }
  }
  if (argc > 1) userSelection=atoi(argv[1]);
  else userSelection=0;

  solution_list::iterator i_sol = bestSolutions.allSolutions().begin();
  advance(i_sol, userSelection);
      
  CInsertionPoint insertionPoint = *i_sol;
      
  // PACKAGE the insertion point into a new CStateVariableInfo
  // create a unique name that doesn't already exist in the system
  string variableName(nextCSCVariableName(variableNumber, design->signals, 
					  design->nsignals));
      
  // create a CStateVariableInfo object
  CStateVariableInfo svInfo(variableName,
			    "1", // instance, for now always 1
			    insertionPoint, // the graph location of the new sv
			    design->events, design->nevents, 
// for reachable() calls
			    0, // lower bound
			    design->maxgatedelay, // upper bound
			    design->cycles, design->ncycles);// for reachable,

  printf("Solution costing %d + %d used.\n",insertionPoint.naturalViolations,
	 insertionPoint.causedViolations);
  fprintf(lg,"Solution costing %d + %d used.\n",
	  insertionPoint.naturalViolations,insertionPoint.causedViolations);
  printf("Signal named %s being added.\n",svInfo.signalName.c_str());
  fprintf(lg,"Signal named %s being added.\n",svInfo.signalName.c_str());
  printf("IP: %s\n",insertionString(design->events,insertionPoint).c_str());
  fprintf(lg,"IP: %s\n",
	  insertionString(design->events,insertionPoint).c_str());
      
  // INSERT the state variable
  // add the state variable to the CTextTel
  success = textTel.addStateVariable(svInfo);
  textTel.writeToFile(design->filename,design->level); // dump to disk
  
  // reload the modified TEL file into memory
  design->status=
    process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);
  
  // check for liveness
  design->status=
    process_command(SYNTHESIS, FINDRED, design, NULL, FALSE, design->si);
  
  // if graph deadlocks
  int cscIndex = -1;
  if((design->status & FOUNDRED) == 0)
    {
      printf("Fixing marking of first CSC transition.\n");
      fprintf(lg,"Fixing marking of first CSC transition.\n");
      // determine the event index for the first CSC transition
      if (design->startstate[design->nsignals-1] == '0')
	cscIndex = design->nevents - 2;
      else
	cscIndex = design->nevents - 1;
      
      // check each rule entering the first CSC transition
      for (int i=0; i < design->nevents; i++)
	{
	  if (design->rules[i][cscIndex]->ruletype != NORULE)
	    if (design->rules[i][cscIndex]->reachable == FALSE)
	      {
		design->rules[i][cscIndex]->epsilon = 1;
	      }
	}
    }
  
  // now that we've tried to fix it, did we?
  // check for liveness
  design->status=
    process_command(STORE, TIMEDER, design, NULL, FALSE, design->si);
  design->status=
    process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);
  design->status=
    process_command(SYNTHESIS, FINDRED, design, NULL, FALSE, design->si);
  
  // if still not live, mess with the other transition
  if((design->status & FOUNDRED) == 0)
    {
      printf("Fixing marking of second CSC transition.\n");
      fprintf(lg,"Fixing marking of second CSC transition.\n");
      // determine the event index for the second CSC transition
      if (design->startstate[design->nsignals] == '0')
	cscIndex = design->nevents - 1; // these are swapped from above
      else
	cscIndex = design->nevents - 2;
      
      // check each rule entering the second CSC transition
      for (int i=0; i < design->nevents; i++)
	{
	  if (design->rules[i][cscIndex]->ruletype != NORULE)
	    if (design->rules[i][cscIndex]->reachable == FALSE)
	      {
		design->rules[i][cscIndex]->epsilon = 1;
	      }
	}
    }
  
  // check one final time for liveness
  design->status=
    process_command(STORE, TIMEDER, design, NULL, FALSE, design->si);
  design->status=
    process_command(LOAD, TIMEDER, design, NULL, TRUE, design->si);
  design->status=
    process_command(SYNTHESIS, FINDRED, design, NULL, FALSE, design->si);
  if((design->status & FOUNDRED) != 0 && cscIndex != -1)
    {
      printf("Graph brought back to life.\n");
      fprintf(lg,"Graph brought back to life.\n");
    }
  
  // find the new state graph
  design->status =
    process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, design->si);
  
  // rate the new state graph
  int newNaturalViolations;
  int newCausedViolations;
  countCSCViolations(design->state_table,
		     design->ninpsig,
		     newNaturalViolations,
		     newCausedViolations);
  printf("Initial: %d + %d\n",initialNaturalViolations,
	 initialCausedViolations);
  fprintf(lg,"Initial: %d + %d\n",initialNaturalViolations,
	  initialCausedViolations);
  printf("Predicted: %d + %d\n",insertionPoint.naturalViolations,
	 insertionPoint.causedViolations);
  fprintf(lg,"Predicted: %d + %d\n",insertionPoint.naturalViolations,
	  insertionPoint.causedViolations);
  printf("Actual: %d + %d\n",newNaturalViolations,newCausedViolations); 
  fprintf(lg,"Actual: %d + %d\n",newNaturalViolations,newCausedViolations); 

  /*
  if (design->manual) {
    cout << "E(x)it, (C)ontinue adding variables " << endl;
    char userInput;
    cin >> userInput;
    if (userInput == 'x') punt = true;
  }
  
  } // end while

  // all done, so ...
  // pass control to the next correct function in the function chain


  if (command != FINDSV) // if part of a larger command
    {
      design->status=process_command(SYNTHESIS, command, design,
				     NULL, FALSE, design->si);
    }
  */
  fflush(lg);
  return TCL_OK;
}

/*****************************************************************************/
/* Process command passed in from X interface.                               */
/*****************************************************************************/

static int X_interface(ClientData clientData, Tcl_Interp *interp,
		       int argc, const char *argv[])
{
  char nextmenu,nextcommand;
  char selection[MAXTOKEN];
  bool newfile;

  /*  freopen("tcl.log","w",stdout); */
  nextmenu=HELP;
  nextcommand=HELP;
  freopen("atacs.log","w",lg);
  newfile=FALSE;
  if (argc > 1) {
    nextmenu=argv[1][0];
    nextcommand=argv[1][1];
    if (nextcommand=='\0') nextcommand=nextmenu;
  }
  if (argc > 2) {
    if (nextmenu==CYCLES)
      design->ncycles=atoi(argv[2]);
    else if (nextmenu==MAXSIZE)
      design->maxsize=atoi(argv[2]);
    else if (nextmenu==ITERATIONS)
      design->iterations=atoi(argv[2]);
    else if (nextmenu==GATEDELAY) {
      design->mingatedelay=atoi(argv[2]);
      if (argc > 3)
	if (strcmp(argv[3],"inf")==0)
	  design->maxgatedelay=INFIN;
	else
	  design->maxgatedelay=atoi(argv[3]);
    }
    else if (nextmenu==TOLERANCE)
      design->tolerance=atof(argv[2]);
    else if (nextmenu==SIMTIME)
      design->simtime=atoi(argv[2]);
    else if (nextmenu==LINKSIZE) {
#ifdef DLONG
      design->bdd_state->linksize=atoi(argv[2]);
      design->bdd_state->maxlinks=1<<design->bdd_state->linksize;
#endif
    }
    else if (nextmenu==HELP)
      strcpy(selection,argv[2]);
    else {
      strcpy(design->filename,argv[2]);
      find_file_type(design,nextmenu,nextcommand);
      newfile=TRUE;
      if (argc > 3)
	strcpy(design->component,argv[3]);
      else
	design->component[0]='\0';
    }
  }

  if ((nextmenu != '\0') && (newfile==FALSE) && (status==RESET) &&
      (nextmenu != HELP) && (nextmenu != MAXSIZE) &&
      (nextmenu != ITERATIONS) &&
      (nextmenu != TOLERANCE) && (nextmenu != LINKSIZE) && 
      (nextmenu != SIMTIME) && 
      (nextmenu != GATEDELAY) && (nextmenu != CYCLES) && 
#ifdef TSE_ALGM
      (nextmenu != TSE) &&
#endif
      (nextmenu != OPTIONS) && (nextmenu != COMPOPT) && 
      (nextmenu != TIMING) && (nextmenu != PRUNING))
    printf("ERROR:  Need to load file first!\n"); 
  else if (nextmenu != '\0') {
    status=process_command(nextmenu,nextcommand,design,selection,newfile,
			   FALSE);
  }
  /*  fflush(stdout); */
  fflush(lg);
  return(0);
}


int Tcl_AppInit( Tcl_Interp *interp)
{
/*    int outdes; */

    Tk_Window main;

    main = Tk_MainWindow(interp);

    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

    if (Tcl_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }
    if (Tk_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

//    design=initialize_design();
//    status=RESET;

    /*
    if ((outdes=open("tcl.log",O_WRONLY | O_CREAT | O_TRUNC, STDMODE)) == -1) 
      printf("ERROR: Could not open tcl.log!\n"); 
    else {
      if (dup2(outdes, STDOUT_FILENO) == -1)
	printf("ERROR: Could not redirect standard output!\n"); 
      close(outdes);
    }
    */

    Tcl_CreateCommand(interp, "X_interface", X_interface,
		      (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

    Tcl_CreateCommand(interp, "X_findsv", X_findsv,
		      (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

    /*
     * Specify a user-specific start-up file to invoke if the application
     * is run interactively.  Typically the start-up file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific start-up file will be run under any conditions.
     */

//    tcl_RcFileName = "~/.wishrc";

    return TCL_OK;
}
#endif

/*****************************************************************************/
/* MAIN PROGRAM                                                              */
/*   Contains main program which opens a file named "atacs.log" to store     */
/*   displayed messages, and if there are arguments on the command line      */
/*   processes them, otherwise, initiates the user interface to get and      */
/*   process commands from the user.                                         */
/*****************************************************************************/

int main(int argc,char *argv[])
{
  srand( 1 );
#ifdef MEMDEBUG
  set_MemdebugOptions(c_Yes,c_Yes,c_Yes,c_No,c_Yes,c_Yes,c_Yes,0,0,
		      "stats","");
#endif
  design=initialize_design();
  status=RESET;
  lg=fopen("atacs.log","w");
  if(lg==NULL) {
    printf("ERROR: Cannot open atacs.log\n");
    return 1;
  }
#ifdef XIF
  if (strstr(argv[0],"xatacs")!=NULL || strstr(argv[0],"xxatacs")!=NULL ) 
    Tk_Main( argc, argv, Tcl_AppInit);
  else 
#endif
    {
      printf("ATACS VERSION 6.0\n");
      printf("Logging session in:  atacs.log\n");
      if (argc > 1) process_arguments(argc,argv); 
      else user_interface();
    }
  fclose(lg);

  //  delete_design(design);

  return problems;
}
