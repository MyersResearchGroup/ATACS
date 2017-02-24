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
/* loadprs.h                                                                 */
/*****************************************************************************/

#ifndef _loadprs_h
#define _loadprs_h

#include "struct.h"

/*****************************************************************************/
/* Defining token types needed for ER system files.                          */
/*****************************************************************************/

#define SIGNAL 's'      /* strings, characters */
#define NOT '~'         /* the symbol "~" */
#define LBRK '['        /* the symbol "[" */
#define RBRK ']'        /* the symbol "]" */
#define LPAR '('        /* the symbol "(" */
#define RPAR ')'        /* the symbol ")" */
#define AND '&'         /* the symbol "&" */
#define COLON ':'       /* the symbol ":" */
#define RISE '+'        /* the symbol "+" */
#define FALL '-'        /* the symbol "-" */
#define EOL 'n'        /* return character "\n" */

/*****************************************************************************/
/* Load production rules.                                                    */
/*****************************************************************************/

bool load_production_rules(char command,designADT design);

#endif /* loadprs.h */





