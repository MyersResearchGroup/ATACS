#ifndef _bcp_h
#define _bcp_h

#include <stdio.h>

#ifndef OSX
#include <malloc.h>
#endif

/*
#define MAXVAR 255
#define MAXPRIMES 15000
*/

typedef struct table_tag {
  char *row;/*[MAXPRIMES];*/
  int size;
  struct table_tag *link;
} *tableADT;

typedef struct tableSol_tag {
  tableADT F;
  char *currentSol;/*[MAXPRIMES];*/
  int cost;
  int size;
} *tableSolADT;

/* extern int nsolutions; */

void delete_tableSol(tableSolADT Sol);

void print_current_solution(FILE *fp,tableSolADT Sol);

tableSolADT BCP(tableSolADT Sol,int *U,int *cost,int depth);

#endif  /* bcp.h */






