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
/* misclib.c                                                                 */
/*****************************************************************************/

#include <stdio.h>
/* #include <cstdlib> */
#include <cstring>
#include <stdarg.h>
#include "struct.h"

#define OPTIONSIZE 100

#include "errno.h"
#include "misclib.h"
#include "hse.hpp"
/*
void operator delete(void *rawMemory){
}
*/

//void nop(void *junk){
  //  if (junk!=NULL) free(junk);
//}

char undefined_object[] = "UNDEFINED";

/*****************************************************************************/
/* Used for memory allocation in place of malloc to check for memory         */
/*   exhaustion.                                                             */
/*****************************************************************************/
#ifndef MEMDEBUG
void *GetBlock(size_t nbytes)
{
  void *result;

  result = malloc(nbytes);
  
  if (result == NULL)
    Error("Free storage exhausted");
  return (result);
}
#endif
/*****************************************************************************/
/* Does a strcpy in which memory for the new pointer is first obtained.      */
/*****************************************************************************/

char * CopyString(const char s[])
{
  char * t;
  
  if (s) {
    t = (char *)GetBlock(strlen(s) + 1);
    strcpy(t, s);
    return(t);
  } else return(NULL);
}

/*****************************************************************************/
/* Fatal Error Routine.                                                      */
/*****************************************************************************/

void Error(char * msg, ...)
{
    va_list args;

    va_start(args, msg);
    printf("Error: ");
    vprintf(msg, args);
    fprintf(lg, "Error: ");
    vfprintf(lg, msg, args);
    fclose(lg);
    va_end(args);
    printf("\n");
    fprintf(lg, "\n");
    exit(ErrorExitStatus);
}

/*****************************************************************************/
/* Process dash options given on the command line and return as string.      */
/*****************************************************************************/

char * processoptions(char * argv[], int argc, char * newargv[],
	       int *newargcptr)
{
  char * options;            /* character array containing option letters */ 
  int j;                     /* counter for options */
  unsigned k;                /* counter for string allocated by pointer argv */

  options=(char *)GetBlock(OPTIONSIZE * sizeof(char *));
  *newargcptr=0;
  j=0;
  while (--argc > 0) {
    if ((*++argv)[0] != '-') {
      newargv[*newargcptr]=(*argv);
      *newargcptr=(*newargcptr)+1;
    } else {
        for(k=1; k < strlen(*argv); k++) {
	  options[j]=(*argv)[k];
	  j++;
	}
    }
  }
  options[j]='\0';
  newargv[*newargcptr]=NULL;
  return(options);
}

/*****************************************************************************/
/* Determines if an integer is a power of 2 minus 1.                         */
/*****************************************************************************/

int is_power_of_2_minus_1(int x) 
{
  int shifts;

  shifts=0;
  while (x != 0) {
    if (x % 2==0) return (-1);
    x=x>>1;
    shifts++;
  }
  return shifts;
}

/* The following front end to fopen(path,mode) checks the return code,
   and prints a message if it is NULL.  However, it then returns the
   NULL pointer on up!  */
FILE *Fopen (const char *path, const char *mode){
  //loggedStream errors(cerr,lg);
  FILE * result(fopen(path,mode));  // Attempt to open file path in mode mode.
  /*
  if(!result){  //  Something went wrong!
    errors << "Could not open " << path << " in mode " << mode
	   << ": " << strerror(errno) << endl;
	   }*/
  return result;  // WARNING:   This could be NULL!
}
