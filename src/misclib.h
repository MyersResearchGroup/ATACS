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
/* misclib.h                                                                 */
/*****************************************************************************/

#ifndef _libmisc_h
#define _libmisc_h

/*****************************************************************************/
/* Section 1: Imports                                                        */
/*****************************************************************************/

#include <stdio.h>

#ifndef OSX
#include <malloc.h>
#else
#include <malloc/malloc.h>
#endif

#include <cstdlib> 

//void operator delete(void *rawMemory);

//void nop(void *junk);

/*****************************************************************************/
/* Section 2: New defined types                                              */
/*****************************************************************************/

/******************************************************************************
   Type: bool

   This type has two values, FALSE and TRUE, which are equal
   to 0 and 1 respectively.  Most of the advantage of defining
   this type comes from readability, because it allows the
   programmer to provide documentation that a value will
   take on only one of these two values.
******************************************************************************/

#ifdef THINK_C
   typedef Boolean bool;
#else
#define FALSE 0
#define TRUE 1 
#ifndef __cplusplus
typedef int bool; 
#else
/*
#ifdef hpux
typedef int bool; 
#endif
*/
#endif

/*   typedef enum {FALSE, TRUE} bool; */

#endif

/******************************************************************************
   The type (void *) is the most general pointer type.
   Besides NULL, the only other constant of type (void *) is
   UNDEFINED, which is used in certain packages as a special
   sentinel value to indicate an undefined pointer value in
   those contexts in which NULL would be a legitimate value.
******************************************************************************/

#define UNDEFINED ((void *) undefined_object)

extern char undefined_object[];

/******************************************************************************
   Type: opaque

   Every time the symbol opaque is used in a type definition,
   it creates a new unique type.  The purpose of this
   mechanism is to allow the interface to hide the details
   of a type implementation from the client.  An interface
   declares a type as

        typedef opaque *mytype;

   and then uses mytype as the name of the type exported
   by the interface.  In the implementation, pointers to
   the opaque type must be converted back and forth to
   the underlying type using type casts.
******************************************************************************/

#define opaque struct { int dummy; }

/*****************************************************************************/
/* Section 3: Functions                                                      */
/*****************************************************************************/

/******************************************************************************
   Function: GetBlock
   Usage: ptr = GetBlock(nbytes);

   GetBlock allocates a block of memory of the given size.
   If there no memory available, GetBlock generates an error.
******************************************************************************/

#ifdef MEMDEBUG
#include <memdebug.h>
#define GetBlock malloc
#else
void *GetBlock(size_t nbytes);
#endif

/******************************************************************************
   Function: CopyString
   Usage: s = CopyString(t);

   Copies the string t into dynamically-allocated storage.
******************************************************************************/

char * CopyString(const char s[]);

/******************************************************************************
   Function: GetLine
   Usage: length = GetLine(s, n);

   s[] is an array of size n.
   GetLine reads the next line from standard input.
   The line is stored in s without the terminating '\n',
   and the result is terminated with a '\0'.  At most
   n-1 characters are read, to allow room for the '\0'.
   
   Returns: length of the string, or -1 on end of file.
******************************************************************************/

int GetLine(char s[], int n);

/******************************************************************************
   Function: Error
   Usage: Error(msg, ...)

   Prints an error message, expanding % constructions in
   the error message string in the manner of printf.
   The program exits then exits with status code as given by
   the constant ErrorExitStatus.
******************************************************************************/

#define ErrorExitStatus 1

void Error(char * msg, ...);

/*****************************************************************************/
/* Process dash options given on the command line and return as string.      */
/*****************************************************************************/

char * processoptions(char * argv[], int argc, char * newargv[],
               int *newargcptr);

/*****************************************************************************/
/* Determines if an integer is a power of 2 minus 1.                         */
/*****************************************************************************/

int is_power_of_2_minus_1(int x);

/* The following front end to fopen(path,mode) checks the return code,
   and prints a message if it is NULL.  However, it then returns the
   NULL pointer on up!  */
FILE *Fopen (const char *path, const char *mode);

#endif /* _libmisc_h */
