#ifndef OSX
#include <malloc.h>
#endif

#include <stdio.h>
#include <cstdlib> 
#include <cstring>
#define MAXLINE 2000      /* Max characters of a line in input file */
#define MAXSTRING 200     /* Max characters of a signal name/state number in input file */

#define UNDEF 0           /* Definitions for signal types and values */
#define EDGE 1
#define LEVEL 3
#define RISING 1
#define FALLING 2
#define DDC 3
#define HIGH 4
#define LOW 5
#define INBURST 1
#define OUTBURST 2
#define STATEBURST 3

/*** GTRANS: forms a list of graph places where nextplace is the next place and the outtrans ***/
/*** is the signal burst of the outgoing transition (in form of a list of signal names).     ***/

typedef struct SIGNAL_LIST {
  char  *name;
  int    value;
  int    type;
  SIGNAL_LIST *next; };

// Example use:  [a+]->[b-]->[NULL]

typedef struct GTRANS {
  int  placetype;
  char *nextplace;
  char *statevector;
  SIGNAL_LIST *outtrans;
  GTRANS *next; };

// Example use:  [2]->[3]




typedef struct STRING_LIST {
  char *str;
  STRING_LIST *next; };


STRING_LIST *split_line(char *str);     /*** Splits a string into a list of its constituent words   ***/

void print_gtrans(int* value);          /*** Prints hash table with value type GTRANS               ***/

char* itoa(int n);                      /*** Allocates and returns string equivalent to integer 'n' ***/





