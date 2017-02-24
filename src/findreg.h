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
/* findreg.h                                                                 */
/*****************************************************************************/

#ifndef _findreg_h
#define _findreg_h

#include "struct.h"

/*****************************************************************************/
/* Print state vectoreader.                                                */
/*****************************************************************************/

void print_state_vector(FILE *fp,signalADT *signals,int nsignals,int ninpsig);

/*****************************************************************************/
/* Split region to solve non-persistency of trigger signals.                 */
/*****************************************************************************/

bool split_region(stateADT *state_table,regionADT *regions,int nsignals,
		  int k,int m,char * new_region,char * new_enablings,
		  char * new_tsignals,regionADT cur_region,
		  char * prime_region);

/*****************************************************************************/
/* Delete a region from a list of regions.                                   */
/*****************************************************************************/

regionADT delete_region(regionADT first_region,regionADT prev_region,
			regionADT cur_region,regionADT next_region);

/*****************************************************************************/
/* Print enabled cubes and trigger cubes.                                    */
/*****************************************************************************/

void print_regions(FILE *fp,signalADT *signals,regionADT *regions,
		   int ninpsig,int nsignals);

/*****************************************************************************/
/* Find value of a state bit (0 and R return 0 and 1 and F return 1).        */
/*****************************************************************************/

char VAL(char bit);

bool enstate_includes(char * enstate,char * state,int nsignals);

void union_enstate(char * new_enstate,char * old_enstate,int nsignals);

/*****************************************************************************/
/* Find enabled states.                                                      */
/*****************************************************************************/

bool find_regions(char command,designADT design);

#endif  /* findreg.h */





