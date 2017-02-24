#ifndef OSX
#include <malloc.h>
#endif

#include <stdio.h>
#include <cstdlib> 
#include <cstring>
#include <inttypes.h>

#define HASH_KEYSAMPLES 2    /* Max number of characters examined while building hash value */
#define HASH_PRIME 13        /* Hash seed number */

typedef struct SI_LIST {
  char *key;
  intptr_t  value;
  SI_LIST *next; };

typedef struct SS_LIST {
  char *key;
  char *value;
  SS_LIST *next; };


typedef struct IHASH_TABLE {
  int size;
  int entries;
  struct IHASH_ENTRY *table; };

typedef struct IHASH_ENTRY {
  bool   empty;
  char  *key;
  int    value;
  struct IHASH_ENTRY *next; };

typedef struct SHASH_TABLE {
  int size;
  int entries;
  struct SHASH_ENTRY *table; };

typedef struct SHASH_ENTRY {
  bool   empty;
  char *key;
  char *value;
  struct SHASH_ENTRY *next; };

typedef struct HASH_TABLE {
  int size;
  int entries;
  struct HASH_ENTRY *table; };

typedef struct HASH_ENTRY {
  bool   empty;
  char *key;
  intptr_t    *value;
  struct HASH_ENTRY *next; };

/*******************************************/
/*** Hash table to store integers        ***/
/*******************************************/
IHASH_TABLE* ihash_create(int size);                          /*** Create a hash table of size 'size'                   ***/
SI_LIST *ihash_to_list(IHASH_TABLE* table);                   /*** Return all keys/values in table as list of pairs     ***/
void ihash_insert(IHASH_TABLE* table, char* key, int value);  /*** Insert a (key,value) pair in hash-table 'table'      ***/
int  ihash_lookup(IHASH_TABLE* table, char* key);             /*** Lookup the value for key 'key' in hash table 'table' ***/
void ihash_print(IHASH_TABLE* table);                         /*** Print the whole hash table 'table'                   ***/
int  ihash_exists(IHASH_TABLE* hash_table, char* key);        /*** Returns 1 if entry with key 'key' exists, else 0     ***/

/******************************************/
/*** Hash table to store strings        ***/
/******************************************/
SHASH_TABLE* shash_create(int size);                              /*** Create a hash table of size 'size'                   ***/
SS_LIST *shash_to_list(SHASH_TABLE* table);                       /*** Return all keys/values in table as list of pairs     ***/
void   shash_insert(SHASH_TABLE* table, char* key, char* value);  /*** Insert a (key,value) pair in hash-table 'table'      ***/
char* shash_lookup(SHASH_TABLE* table, char* key);                /*** Lookup the value for key 'key' in hash table 'table' ***/
void   shash_print(SHASH_TABLE* table);                           /*** Print the whole hash table 'table'                   ***/

/****************************************************/
/*** Hash table to store generic value structures ***/
/****************************************************/
HASH_TABLE* hash_create(int size);                             /*** Create a hash table of size 'size'                           ***/
void hash_insert(HASH_TABLE* table, char* key, intptr_t* value);    /*** Insert a (key,value) pair in hash-table 'table'              ***/
intptr_t* hash_lookup(HASH_TABLE* table, char* key);                /*** Lookup the value pointer for key 'key' in hash table 'table' ***/
void hash_print(HASH_TABLE* table, int* funct);                /*** Print the whole hash table 'table'                           ***/
SI_LIST *hash_to_list(HASH_TABLE* hash_table);                 /*** Return all keys/values in table as list of pairs             ***/



