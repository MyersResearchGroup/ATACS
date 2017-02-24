#include "hmisc.h"

/*************************************************************/
/*** Function for splitting a line into constituent words. ***/
/*************************************************************/

STRING_LIST *split_line(char *str) 
{
  STRING_LIST *list_start;
  char wrd[MAXSTRING];
  int i;

  STRING_LIST *entry = (STRING_LIST *) malloc(sizeof(struct STRING_LIST));
  list_start = entry;
  (*entry).str = NULL;
  (*entry).next = NULL;
  while (*str != '\n' && *str != '\0' && *str != ';') {
    i = 0;
    while (*str == ' ' || *str == '\t') {
      str++; }
    while (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\0' && *str != ';') {
      wrd[i++] = *str++; }
    if (i > 0) {
      wrd[i] = '\0';
      (*entry).str = strdup(wrd);
      (*entry).next = (STRING_LIST *) malloc(sizeof(struct STRING_LIST));
      entry = (*entry).next;
      (*entry).str = NULL;
      (*entry).next = NULL; 
    }
  }
  return(list_start);
}


/***********************************************************/
/*** Function for printing content of a hash table with  ***/
/*** GTRANS as value type.                               ***/
/***********************************************************/

void print_gtrans(int* value) 
{
  GTRANS *trans;
  SIGNAL_LIST *signal;

  printf("\t");
  trans = (GTRANS *) value;
  while (trans) {
    printf("%s,",(*trans).nextplace);
    signal = (*trans).outtrans;
    while (signal) {
      switch ((*signal).type) {
      case LEVEL : 
	switch ((*signal).value) {
	case HIGH : printf("[%s+] ",(*signal).name); fflush(stdout); break;
	case LOW : printf("[%s-] ",(*signal).name); fflush(stdout); break;
	default : printf("Error: invalid polarity for level signal %s\n",(*signal).name); fflush(stdout); exit(1); }
	  break;
      case EDGE : 
	switch ((*signal).value) {
	case RISING  : printf("%s+ ",(*signal).name); fflush(stdout); break;
	case FALLING : printf("%s- ",(*signal).name); fflush(stdout); break;
	case DDC     : printf("%s* ",(*signal).name); fflush(stdout); break;
	default : printf("Error: invalid polarity for edge signal %s\n",(*signal).name); fflush(stdout); exit(1); }
	  break;
      default : 
	printf("Error: invalid type for signal %s\n",(*signal).name); fflush(stdout); exit(1); }
      signal = (*signal).next; }
    if ((*trans).statevector) { printf(" %s ",(*trans).statevector); fflush(stdout); }
    trans = (*trans).next;
    if (trans) { printf("\t; "); fflush(stdout); } }
  printf("\n");
}

/***********************************************************/
/*** Function translating an integer to a string         ***/
/***********************************************************/

char* itoa(int n) 
{
  int i, j, sign;

  if ((sign = n) < 0) {
    n = -n; }
  i = 1;
  do {
    n = n / 10;
    i++; } while (n > 0);
  if (sign < 0) { i++; }
  char *rev = (char *) malloc(i * sizeof(char));
  char *str = (char *) malloc(i * sizeof(char));
  i = 0;
  n = sign;
  do {
    rev[i++] = n % 10 + '0';
    n = n / 10; } while (n > 0);
  if (sign < 0) {
    rev[i++] = '-'; }
  for (j = 0; j < i; j++) {
    str[j] = rev[i-j-1]; }
  str[i] = '\0';
  free(rev);
  return(str);
}

