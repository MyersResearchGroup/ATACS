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
/* atacs.h                                                                   */
/*   Contains definitions for procedures for the interface which gets and    */
/*   processes commands.  Also, single letter commandline arguments for each */
/*   option in the program are defined here.                                 */
/*****************************************************************************/

#ifndef _atacs_h
#define _atacs_h
/*
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#include <ctype.h>
#include <unistd.h> 
}
#else
#include <ctype.h>
#include <unistd.h>
#endif
*/

#include "struct.h"
#include "misclib.h"
#include "memaloc.h"
#include "loader.h"
#include "loadac.h"
#include "loadrsg.h"
#include "loadprs.h"
#include "loadsis.h"
#include "connect.h"
#include "findtd.h"
#include "findwctd.h"
#include "findcl.h"
#include "findrsg.h"
#include "storesg.h"
#include "findconf.h"
#include "findcover.h"
#include "storenet.h"
#include "verify.h"
#include "ctlchecker.h"
#include "printgrf.h"
#include "printrsg.h"
#include "printsg.h"
#include "printreg.h"
#include "delaycalc.h"
#include "simulate.h"
#include "loadg.h"
#include "storeg.h"
#include "storeer.h"
#include "compile.h"
#include "help.h"
#include "struct2net.h"
#include "exact.h"
#include "findred.h"
#include "findreg.h"
#include "storeprs.h"
#ifdef DLONG
#include "bddrsg.h"
#else
#include "cudd_synth.h"
#endif
#include "drawnet.h"
#include "storevhd.h"
#include "dotrsg.h"
#include "dotreg.h"
#include "dotgrf.h"
#include "findstats.h"
#include "interface.h"
#include "stateasgn.h"
#include "heursyn.h"
#include "kiss2sg.h"
#include "hxbm2csg.h"
#include "decomp.h"
#include "hse.hpp"
#include "tel2vhd.h"
#include "tersgen.h"
#include "printlpn.h"
#include "svcommon.h"
#include "bag.h"
#include "hpnsim.h"
#include "lhpnsim.h"
#include "symbolic.h"
#include "smtsym.h"
#include "satconf.h"
#include "partial_order.h"
#include "biomap.h"
#include "loadData.h"

#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>

#ifdef INTEL
#include "slack.h"
#endif

#undef EXTERN

#ifdef XIF
#include <tk.h>
#endif

#ifdef MEMDEBUG
#include <memdebug.h>
#endif

#endif  /* interface.h */










