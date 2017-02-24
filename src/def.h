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
/* def.h                                                                    */
/*   Contains definitions for procedures for the interface which gets and    */
/*   processes commands.  Also, single letter commandline arguments for each */
/*   option in the program are defined here.                                 */
/*****************************************************************************/

#ifndef _def_h
#define _def_h

/* Uncomment for compilation under gcc 3.x w/out deprecated headers
   warnings */
#define __GCC3_INC__

/* Uncomment for optimized gcc296 compilation.  This basically turns
   on Eric Mercer's custom memory allocators */
//#define __GCC_OLD_OPT__

/*****************************************************************************/
/* Single letter commandline arguments for each option in the program.       */
/*****************************************************************************/

/* File Menu */
#define LOAD 'l'
#define STORE 's'
#define DISPLAY 'd'
#define PRINT 'p'
#define QUIT 'q'

/* File types */
#define CSP 'c'
#define HSE 'h'
#define VHDL 'v'
#define TIMEDER 'e'
#define TEL 't'
#define LPN 'l'
#define DATA 'd'
#define SG 'm'
#define RSG 's'
#define RSG_WITH_STATS 'z'
#define PRS 'p'
#define GRAPH 'g'
#define NET 'n'
#define REG 'r'
#define XBM 'x'
#define KISS2SG 'K'
#define TEL2VHD 'T'
#define VERILOG 'V'
#define SPICE 'S'

/* Synthesis Menu */
#define SYNTHESIS 'y'
#define EXPAND  'E'
#define REDUCE  'r'
#define FINDRED 'D'
#define FINDRSG 'S'
#define PROJECT 'P'
#define FINDREG 'R'
#define FINDVIOL 'V'
#define FINDCOVER 'C'
#define DOALL 'A'

/* Synthesis methods */
#define SINGLE 's'
#define MULTICUBE 'm'
#define USEBDD 'b'
#define DIRECT 'd'
#define SYN 'Y'
#define SIS 'I'

/* Verification Menu */
#define VERIFY 'v'
#define VERATACS 'a'
#define VERORBITS 'o'
#define VERGATE 'g'
#define SEARCH 's'
#define ANALYZE 'A'
#define TRACE 't'
#define CHECKCTL 'C'
#define VERNATHAN 'N'
#define VERVIJAY 'V'

/* Exceptions Menu */
#define EXCEPTIONS 'e'
#define CONNECT 'C'
#define ADDORD 'O'
#define ADDPERS 'P'
#define SOLVECSC 'V'
#define FINDSV 'S'
#define RESOLVE 'R'
#define BREAKUP 'B'
#define DECOMP 'D'
#define BIOMAP 'M'

/* Analysis Menu */
#define ANALYSIS 'a'
#define FINDTD 'T'
#define FINDWCTD 'W'
#define FINDSLACK 'K'
#define DTMCRP 'R'
#define MEDLEY 'D'
#define TPSIM 'M'
#define TPRGLIN 'L'
#define TPEXP 'E'
#define TPBURST 'B'
#define SIMTEL 'S'
#define SIMCTMC 'C'
#define CTMC 'A'
#define TRIGPROB 'G'
#define CYCPER 'P'
#define LTTPSIM 'I'
#define TRIGSIM 'F'
#define SIMCYC 's'
#define PROFILE 'p'
#define HPNSIM 'h'
#define LHPNSIM 'l'

/* Help menu */
#define HELP 'h'

/* Compilation options */
#define COMPOPT 'c'
#define POSTPROC 'P'
#define REDCHK 'R'
#define NEWTAB 'N'
#define XFORM2 'T' 
#define EXPANDRATE 'E'

/* Timing methods */
#define TIMING 't'
#define SI 'i'
#define COMPRESS 'c'
#define UNTIMED 'u'
#define HEURISTIC 'h'
#define ORBITS 'o'
#define GEOMETRIC 'g'
#define APPROX 'a'
#define SETS 's'
#define BAG 'b'
#define BAP 'p'
#define BAPTDC 't'
#define SL 'S'
#define HPN 'Y'
#define LHPNDBM 'L'
#define LHPNBDD 'B'
#define LHPNSMT 'M'

/* Modes */
#define OPTIONS 'o'

/* Timing options */
#define NOFAIL 'f'
#define DISABLE 'D'
#define INFOPT 'F'
#define GENRG 'G'
#define INTERLEAV 'I'
#define ORBMATCH 'O'
#define PRUNE 'P'
#define SUBSETS 'S'
#define SUPERSETS 'U'
#define PORED 'p'
#define ABSTRACT 'a'
#define EXPLPN 'L'
#define KEEPGOING 'K'
#define NOPROJ 'j'

/* Synthesis options */
#define COMBO 'C'
#define EXACT 'E'
#define MANUAL 'M'
#define SHAREGATE 'H'
#define INPONLY 'N'

/* Technology options */
#define ATOMIC 't'
#define GENC 'g'
#define GATELEVEL 's'
#define BURSTGC 'b'
#define BURST2L 'l'

/* Other options */
#define DOTPARG 'd'
#define NOCHECKS 'n'
#define NOPARG 'q'
#define VERBOSE 'v'
#define EXCEPTION 'X'
#define REDUCTION 'R'
#define INSERTION 'i'
#define MININS 'm'

/* Pruning options */
#define PRUNING 'P'
#define NOT_FIRST 'N'
#define PRESERVE 'P'
#define ORDERED 'O'
#define SUBSET 'S'
#define UNSAFE 'U'
#define EXPENSIVE 'E'
#define DISJOINT 'D'
#define CONFLICT 'C'
#define REACHABLE 'R'
#define DUMB 'D'

/* Defaults */
#define CYCLES 'C'
#define MAXSIZE 'M'
#define GATEDELAY 'G'
#define LINKSIZE 'L'
#define TOLERANCE 'T'
#define SIMTIME 'S'
#define ITERATIONS 'I'

/* Obsolete
#define TSE 'T'
*/

/*****************************************************************************/
/* Definitions used for synthesis status.                                    */
/*****************************************************************************/

#define RESET          0
#define LOADED         1
#define CONNECTED      2
#define EXPANDED       4
#define REDUCED        8
#define FOUNDRED      16
#define FOUNDSTATES   32
#define FOUNDRSG      64
#define FOUNDREG     128
#define FOUNDCONF    256
#define FOUNDCOVER   512
#define STOREDPRS   1024
#define EXACTPRS    2048

// Set high if a found state graph is complete state coded.  Flag is
// manipulated in findrsg.c:find_reduced_state_graph
#define CSC         4096

///////////////////////////////////////////////////////////////////////////////
// I'm going to define a few status bits to indicate if the state of the
// statistical information.  Specifically I'm adding flags for the existence
// of: transition and state probabilities.
///////////////////////////////////////////////////////////////////////////////
#define TRANSPROBS  8192 // Set if transition probabilties exist in state table
#define STATEPROBS 16384 // Set if state probabilities exist in state table

/*****************************************************************************/
/* Definitions for token types needed for the user interface.                */
/*****************************************************************************/

#define WORD 1          /* strings, characters */
#define PIPE 2          /* the symbol "|" */
#define OUTPUT 3        /* the symbol ">" */
#define INPUT 4         /* the symbol "<" */
#define END_OF_LINE 5   /* return character "\n" */
#define SHELL 6         /* the symbol "!" */
#define IMPLIES 7       /* the symbol "->" */
#endif  /* def.h */
