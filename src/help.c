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
/*   IN NO EVENT SHALL U. OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE LIABLE  */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* help.c                                                                    */
/*   Includes procedures to display help messages.                           */
/*****************************************************************************/
#include <iostream>

#include "help.h"

/////////////////////////////////////////////////////////////////////////////
// printhelp will output, in a nice moderate format, the list of commands 
// available for atacs.  If you ever want to add a command to this list,
// do the following things:
//    1) Increase the command_count value by 1
//    2) Add your new command into the list in the correct alphabetical
//       location and with the correct number of spaces for padding.
// * If it is ever necessary to increase the width of each command, change 
// the command_width variable. Note, do not forget the extra character for 
// the NULL marker and you will need to change all commands in the list.
// * If it is ever necessary to change the number of columns printed in the
// help menu, change the column_count.  The output loop will automatically
// adapt.
/////////////////////////////////////////////////////////////////////////////
void printlonghelp(void) {
  
  static const unsigned int column_count(8);
  static const unsigned int command_count(152);
  static const unsigned int command_width(11);

  static const char command_list[command_count][command_width] = 
  {"abstract  ", "addord    ", "addpers   ", "analyze   ",
   "atomic    ", "bag       ",
   "bap       ", "baptdc    ", "bdd       ", "biomap    ",
   "breakup   ", "burst2l   ",
   "burstgc   ", "checkctl  ",
   "combo     ", "conflict  ", "connect   ", "ctmc      ",
   "cycles    ", "cycper    ", "decomp    ", "direct    ", "disabling ",
   "doall     ", "dot       ", "dtmcrp    ", "dumb      ",
   "exact     ", "exception ", "expandrate", "expensive ", "findcover ",
   "findred   ",
   "findreg   ", "findrsg   ", "findslack ", "findsv    ", "findtd    ",
   "findviol  ", "findwctd  ", "gatedelay ", "gatelevel ", "gc        ",
   "genrg     ", "geometric ", "help      ", "heuristic ", "hpn       ",
   "hpnsim    ", "infopt    ",
   "inponly   ", "insertion ", "interleav ", "iterations", 
   "keepgoing ", "lhpnbdd   ", "lhpndbm   ", 
   "lhpnsim   ", "lhpnsmt   ", "linksize  ", "loadcsp   ", "loaddata  ",
   "loader    ", "loadg     ", "loadhse   ", "loadlpn   ", "loadprs   ", 
   "loadrsg   ", "loadtel   ", "loadvhdl  ", "loadxbm   ", "lpn       ",
   "lttpsim   ", "manual    ",
   "maxsize   ", "medley    ", "minins    ", "multicube ", "newtab    ", 
   "nochecks  ", "nofail    ", "noparg    ", "noproj    ", 
   "not_first ", "orbits    ", "orbmatch  ", "ordered   ",
   "poapprox  ", "pored     ", "posets    ", "postproc  ", "preserve  ",
   "printer   ", "printlpn  ", "printnet  ", "printreg  ", "printrsg  ", 
   "printstat ",
   "profile   ", "project   ", "prune     ", "quit      ", "reachable ",
   "redchk    ", "reduction ", "resolve   ", "search    ", "sharegate ", 
   "shower    ", "showlpn   ",
   "shownet   ", "showrsg   ", "showstat  ", "si        ", "simctmc   ",
   "simcyc    ", "simtel    ", "simtime   ",
   "single    ", "sis       ", "solvecsc  ", "srl       ",
   "storeer   ", "storeg    ", "storehsp  ", "storelpn  ", "storenet  ", 
   "storeprs  ",
   "storersg  ", "storesg   ", "storevhdl ", "storevlog ", "subset    ",
   "subsets   ", "supersets ", "syn       ", "tel2vhd   ", "tolerance ",
   "tpburst   ", "tpexp     ", "tprglin   ", "tpsim     ", "trace     ",
   "trigprob  ",
   "trigsim   ", "unsafe    ", "untimed   ", "verbose   ", "vergate   ", 
   "verify    ", "verorbits ", "xform2    " };

  for ( unsigned int i = 0 ; i < command_count ; i++ ) {
    if ( !( i % column_count ) && i )
      cout << endl << command_list[i];
    else
      cout << command_list[i]; 
  }
  cout << endl;

}

void printhelp(void) {
  
  static const unsigned int column_count(8);
  static const unsigned int command_count(64);
  static const unsigned int command_width(11);

  static const char command_list[command_count][command_width] = 
  { "abstract  ", "atomic    ", "bag       ", "bap       ", "baptdc    ", 
    "bdd       ", "burst2l   ", "burstgc   ", "direct    ", "dot       ", 
    "gatedelay ", 
    "gatelevel ", "gc        ", "geometric ", "help      ", "hpnsim    ",
    "iterations", "lhpnsim   ", "loadcsp   ", "loaddata  ",
    "loader    ", "loadg     ", "loadhse   ", "loadlpn   ", "loadrsg   ", 
    "loadtel   ", "loadvhdl  ", "loadxbm   ", "maxsize   ", "multicube ", 
    "nofail    ", "noproj    ",
    "pored     ", "posets    ", "printer   ", "printlpn  ", "printnet  ", 
    "printreg  ", "printrsg  ", "printstat ", "quit      ", "search    ",
    "shower    ", "showlpn   ", "shownet   ", 
    "showrsg   ", "showstat  ", "simcyc    ", "simtime   ",
    "single    ", "storeer   ", 
    "storeg    ", "storelpn  ", "storenet  ", "storeprs  ", "storersg  ", 
    "storesg   ", 
    "storevhdl ", "trace     ", "untimed   ", "verbose   ", "vergate   ", 
    "verify    ", "verorbits " };

  for ( unsigned int i = 0 ; i < command_count ; i++ ) {
    if ( !( i % column_count ) && i )
      cout << endl << command_list[i];
    else
      cout << command_list[i]; 
  }
  cout << endl;

}

/*****************************************************************************/
/* Display command line help.                                                */
/*****************************************************************************/

void printman(void)
{
  printf("Usage: atacs [OPTIONS] f1 ... fn\n");
  printf("-%c? : load from (%c=CSP,%c=HSE,%c=VHDL,%c=PN,%c=LPN,%c=XBM,%c=ER,%c=TEL,%c=RSG,%c=PRS,%c=DATA)\n",
         LOAD,CSP,HSE,VHDL,GRAPH,LPN,XBM,TIMEDER,TEL,RSG,PRS,DATA);
  printf("-%c? : store to (%c=PN,%c=ER,%c=lpn,%c=RSG,%c=SG,%c=PRS,%c=NET,%c=VHDL,%c=VERILOG,\n\t\t%c=TEL2VHD,%c=HSPICE)\n",
	 STORE,GRAPH,TIMEDER,LPN,RSG,SG,PRS,NET,VHDL,VERILOG,TEL2VHD,SPICE);
  printf("-%c? : display (%c=ER,%c=LPN,%c=RSG,%c=STATS,%c=NET)\n",DISPLAY,TIMEDER,LPN,RSG,RSG_WITH_STATS,NET);
  printf("-%c? : print (%c=ER,%c=LPN,%c=RSG,%c=STATS,%c=REG,%c=NET)\n",
	 PRINT,TIMEDER,LPN,RSG,RSG_WITH_STATS,REG,NET);
  printf("-%c? : synthesis (%c=findred,%c=findrsg,%c=project,%c=findreg,%c=findviol\n",
	 SYNTHESIS,FINDRED,FINDRSG,PROJECT,FINDREG,FINDVIOL);
  printf("                 %c=findcover,%c=single,%c=multicube,%c=bdd,%c=direct,%c=syn,%c=sis)\n",FINDCOVER,SINGLE,MULTICUBE,USEBDD,DIRECT,SYN,SIS);
  printf("-%c? : exceptions (%c=connect,%c=addord,%c=addpers,%c=findsv,%c=solvecsc,\n\t\t  %c=resolve,%c=breakup,%c=decomp,%c=biomap)\n",
	 EXCEPTIONS,CONNECT,ADDORD,ADDPERS,FINDSV,SOLVECSC,RESOLVE,BREAKUP,
	 DECOMP,BIOMAP);
  printf("-%c? : analysis (%c=findtd,%c=findwctd,%c=findslack,%c=tpburst,%c=tpsim,%c=tprglin,\n\t        %c=tpexp,%c=simtel,%c=trigprob,%c=simctmc,%c=dtmcrp,%c=ctmc,%c=medley,\n\t        %c=cycper,%c=lttpsim,%c=simcyc,%c=trigsim,%c=profile,%c=hpnsim,%c=lhpnsim)\n",
	 ANALYSIS,FINDTD,FINDWCTD,FINDSLACK,TPBURST,TPSIM,TPRGLIN,
	 TPEXP,SIMTEL,TRIGPROB,SIMCTMC,DTMCRP,CTMC,MEDLEY,CYCPER,LTTPSIM,
         SIMCYC,TRIGSIM,PROFILE,HPNSIM,LHPNSIM);
  printf("-%c? : verify (%c=verify,%c=vergate,%c=verorbits,%c=search,%c=trace,%c=analyze,\n\t      %c=checkctl)\n",
	 VERIFY,VERATACS,VERGATE,VERORBITS,SEARCH,TRACE,ANALYZE,CHECKCTL);
  printf("-%c? : timing methods (%c=si,%c=untimed,%c=heuristic,%c=orbits,%c=geometric,\n\t\t      %c=poapprox,%c=posets,%c=bag,%c=bap,%c=baptdc,%c=srl,%c=hpn,\n\t\t      %c=lhpnbdd,%c=lhpndbm,%c=lhpnsmt)\n",
	 TIMING,SI,UNTIMED,HEURISTIC,ORBITS,GEOMETRIC,APPROX,SETS,BAG,BAP,
         BAPTDC,SL,HPN,LHPNBDD,LHPNDBM,LHPNSMT);
  printf("-%c? : compilation options (%c=newtab,%c=postproc,%c=redchk,%c=xform2, %c=expandrate)\n",
         COMPOPT,NEWTAB,POSTPROC,REDCHK,XFORM2,EXPANDRATE);
  printf("-%c? : timing options (%c=genrg,%c=subsets,%c=supersets,%c=infopt,%c=orbmatch,\n\t\t      %c=interleav,%c=prune,%c=disabling,%c=abstract,%c=po,%c=nofail,\n\t\t      %c=noproj,%c=keepgoing,%c=explpn)\n", 
	 OPTIONS,GENRG,SUBSETS,SUPERSETS,INFOPT,ORBMATCH,INTERLEAV,PRUNE,
	 DISABLE,ABSTRACT,PORED,NOFAIL,NOPROJ,KEEPGOING,EXPLPN);
  printf("-%c? : synthesis options (%c=sharegate,%c=combo,%c=exact,%c=manual,%c=inponly)\n", OPTIONS,SHAREGATE,COMBO,EXACT,MANUAL,INPONLY);
  printf("-%c? : technology options (%c=atomic,%c=gc,%c=gatelevel,%c=burstgc,%c=burst2l)\n",  
	 OPTIONS,ATOMIC,GENC,GATELEVEL,BURSTGC,BURST2L);
  printf("-%c? : pruning options (%c=not_first,%c=preserve,%c=ordered,%c=subset,%c=unsafe,\n\t\t       %c=expensive,%c=conflict,%c=reachable,%c=dumb)\n",  
	 PRUNING, NOT_FIRST, PRESERVE, ORDERED, SUBSET, UNSAFE, EXPENSIVE,
	 CONFLICT, REACHABLE, DUMB);
  printf("-%c? : other options (%c=dot,%c=exception,%c=verbose,%c=noparg,",
	 OPTIONS,DOTPARG,EXCEPTION,VERBOSE,NOPARG);
  printf("%c=nochecks,\n\t\t     %c=reduction,%c=insertion,%c=minins)\n",
	 NOCHECKS,REDUCTION,INSERTION,MININS);
  printf("-%c#  : change number of cycles (cycles)\n",CYCLES);
  printf("-%c#  : change maximum fan-in for a gate (maxsize)\n",MAXSIZE);
  printf("-%c#/#: change default gate delay (gatedelay)\n",GATEDELAY);
  printf("-%c#  : change size of BDD linkspace (linkspace)\n",LINKSIZE);
  printf("-%c#  : change tolerance for convergence checks (tolerance)\n",TOLERANCE);
  printf("-%c#  : change amount of simulation time (simtime)\n",SIMTIME);
  printf("-%c#  : change number of iterations for SMT solver (iterations)\n",
	 ITERATIONS);
}

/*****************************************************************************/
/* Display command line help (short version).                                */
/*****************************************************************************/

void printman_short(void)
{
  printf("Usage: atacs [OPTIONS] f1 ... fn\n");
  printf("-%c? : load from (%c=CSP,%c=HSE,%c=VHDL,%c=PN,%c=LPN,%c=XBM,%c=ER,%c=TEL,%c=RSG,%c=DATA)\n",
         LOAD,CSP,HSE,VHDL,GRAPH,LPN,XBM,TIMEDER,TEL,RSG,DATA);
  printf("-%c? : store to (%c=PN,%c=ER,%c=lpn,%c=RSG,%c=SG,%c=PRS,%c=NET,%c=VHDL)\n",
	 STORE,GRAPH,TIMEDER,LPN,RSG,SG,PRS,NET,VHDL);
  printf("-%c? : display (%c=ER,%c=LPN,%c=RSG,%c=STATS,%c=NET)\n",DISPLAY,TIMEDER,LPN,RSG,RSG_WITH_STATS,NET);
  printf("-%c? : print (%c=ER,%c=LPN,%c=RSG,%c=STATS,%c=REG,%c=NET)\n",
	 PRINT,TIMEDER,LPN,RSG,RSG_WITH_STATS,REG,NET);
  printf("-%c? : synthesis (%c=single,%c=multicube,%c=bdd,%c=direct)\n",
	 SYNTHESIS,SINGLE,MULTICUBE,USEBDD,DIRECT);
  printf("-%c? : analysis (%c=simcyc,%c=profile,%c=hpnsim,%c=lhpnsim)\n",ANALYSIS,SIMCYC,
         PROFILE,HPNSIM,LHPNSIM);
  printf("-%c? : verify (%c=verify,%c=vergate,%c=orbits,%c=search,%c=trace)\n",
	 VERIFY,VERATACS,VERGATE,VERORBITS,SEARCH,TRACE);
  printf("-%c? : timing methods (%c=untimed,%c=geometric,%c=posets,%c=bag,%c=bap,%c=baptdc)\n",
	 TIMING,UNTIMED,GEOMETRIC,SETS,BAG,BAP,BAPTDC);
  printf("-%c? : timing options (%c=abstract,%c=po,%c=nofail,%c=noproj)\n",OPTIONS,
	 ABSTRACT,PORED,NOFAIL,NOPROJ);
  printf("-%c? : technology options (%c=atomic,%c=gc,%c=gatelevel,%c=burstgc,%c=burst2l)\n",  
	 OPTIONS,ATOMIC,GENC,GATELEVEL,BURSTGC,BURST2L);
  printf("-%c? : other options (%c=dot,%c=verbose)\n",OPTIONS,DOTPARG,VERBOSE);
  printf("-%c#  : change maximum fan-in for a gate (maxsize)\n",MAXSIZE);
  printf("-%c#/#: change default gate delay (gatedelay)\n",GATEDELAY);
  printf("-%c#  : change amount of simulation time (simtime)\n",SIMTIME);
  printf("-%c#  : change number of iterations for SMT solver (iterations)\n",
	 ITERATIONS);
}

/*****************************************************************************/
/* The function help(selection) displays a help file using man.              */
/* For this to work, the environment variable ATACS must be set such     */
/* that help on a command called <selection> can be found in the file        */
/*              ${ATACS}man1/<selection>.1                               */
/*****************************************************************************/

void help(char * selection) 
{
  char line[1000];
  char * helppath;

  if (selection[0]=='\0') {
    printhelp();
    return;
  }
  if (strcmp(selection,"long")==0) {
    printlonghelp();
    return;
  }
  helppath=getenv("ATACS");
  if (helppath==NULL) {
    printf("ERROR: ATACS environment variable not set.\n");
    return;
  }
  sprintf(line,"man -M %s %s",helppath,selection);
  system(line);
}
