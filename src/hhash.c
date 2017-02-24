#include "hhash.h"


int hash_value(char* str, int hash_size) 
{
  char chr;
  int val = 0, i = 0, last_chrs = 0;
  int strln = strlen(str);
  if (strln > 2) {
    last_chrs = str[strln - 1] + str[strln - 2]; }
  while ((((chr = *str++) != '\0')) && (i++ < HASH_KEYSAMPLES)) {
    val = (val * HASH_PRIME * strln + (int) chr); }
  val = abs(val + last_chrs) % hash_size; 
  return(val);
}


/**********************************************************************/
/*** Functions for hash table that stores integers                  ***/
/**********************************************************************/

IHASH_TABLE* ihash_create(int hash_size) 
{
  IHASH_TABLE *hash_table = (IHASH_TABLE *) malloc(sizeof(struct IHASH_TABLE));
  IHASH_ENTRY *table_array = (IHASH_ENTRY *) malloc(hash_size * sizeof(struct IHASH_ENTRY));
  (*hash_table).size = hash_size;
  (*hash_table).entries = 0;
  (*hash_table).table = table_array;
  for (int i = 0; i < (*hash_table).size; i++) {
    (*hash_table).table[i].empty = 1;
    (*hash_table).table[i].key = NULL;
    (*hash_table).table[i].value = 0;
    (*hash_table).table[i].next = NULL; 
  }
  return(hash_table);
}

void ihash_insert(IHASH_TABLE* hash_table, char* key, int value)
{
  IHASH_ENTRY *entry;

  entry = &((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if ((*entry).empty) {
    (*hash_table).entries++;
    (*entry).empty = 0;
    (*entry).key = strdup(key);
    (*entry).value = value; }
  else if (strcmp((*entry).key,key) == 0) {
    (*entry).value = value; }
  else {
    while ((*entry).next) {
      entry = (*entry).next;
      if (strcmp((*entry).key,key) == 0) {
	(*entry).value = value;
	return; }}
    (*entry).next = (IHASH_ENTRY *) malloc(sizeof(struct IHASH_ENTRY));
    (*hash_table).entries++;
    (*entry).next->empty = 0;
    (*entry).next->key = strdup(key);
    (*entry).next->value = value;
    (*entry).next->next = NULL;
  }
}

int ihash_lookup(IHASH_TABLE* hash_table, char* key)
{
  int position;
  IHASH_ENTRY entry;
  
  position = hash_value(key,(*hash_table).size);
  entry = ((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if (entry.key) { 
    if(strcmp(entry.key,key) == 0) {
      return(entry.value); }
    else {
      while (entry.next) {
	entry = *entry.next;
	if(strcmp(entry.key,key) == 0) {
	  return(entry.value); }}}
  }
  //printf("Hash table lookup: Could not find entry with key %s, returning 0.\n",key);
  return(0);
}

int ihash_exists(IHASH_TABLE* hash_table, char* key)
{
  int position;
  IHASH_ENTRY entry;
  
  position = hash_value(key,(*hash_table).size);
  entry = ((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if (entry.key) { 
    if(strcmp(entry.key,key) == 0) {
      return(1); }
    else {
      while (entry.next) {
	entry = *entry.next;
	if(strcmp(entry.key,key) == 0) {
	  return(1); }}}
  }
  //printf("Hash table lookup: Could not find entry with key %s, returning 0.\n",key);
  return(0);
}

SI_LIST *ihash_to_list(IHASH_TABLE* hash_table)
{
  IHASH_ENTRY entry;
  SI_LIST *list_start = NULL, *prev = NULL;

  SI_LIST *node = (SI_LIST *) malloc(sizeof(struct SI_LIST));
  for(int i = 0; i < (*hash_table).size; i++) {
    entry = (*hash_table).table[i];
    if (entry.key != (char*)'\0') {
      if (list_start == NULL) { list_start = node; }
      (*node).key = strdup(entry.key);
      (*node).value = entry.value;
      (*node).next = (SI_LIST *) malloc(sizeof(struct SI_LIST));
      prev = node;
      node = (*node).next;
      (*node).key = NULL;
      (*node).value = 0;
      (*node).next = NULL;
      while (entry.next) {
	entry = *entry.next;
	(*node).key = strdup(entry.key);
	(*node).value = entry.value;
	(*node).next = (SI_LIST *) malloc(sizeof(struct SI_LIST));
	prev = node;
	node = (*node).next;
	(*node).key = NULL;
	(*node).value = 0;
	(*node).next = NULL;
      }
    }
  }
  free(node);
  if (prev != NULL) {
    (*prev).next = NULL; }
  return(list_start);
}

void ihash_print(IHASH_TABLE* hash_table)
{
  IHASH_ENTRY entry;

  printf("Hash table size: %i, entries: %i\n",(*hash_table).size,(*hash_table).entries);
  for(int i = 0; i < (*hash_table).size; i++) {
    if (!(*hash_table).table[i].empty) {
      printf("hash_table[%i][0].value = %i,  key = %s\n",i,(*hash_table).table[i].value,(*hash_table).table[i].key);
      fflush(stdout);
      entry = (*hash_table).table[i];
      int j = 1;
      while (entry.next) {
	entry = *entry.next;
	printf("hash_table[%i][%i].value = %i,  key = %s\n",i,j++,entry.value,entry.key);
	fflush(stdout); }
    }
  }
}

void ihash_remove() {}




/**********************************************************************/
/*** Functions for hash table that stores strings                   ***/
/**********************************************************************/

void shash_remove() {}

void shash_append() {}

SHASH_TABLE* shash_create(int hash_size) 
{
  SHASH_TABLE *hash_table = (SHASH_TABLE *) malloc(sizeof(struct SHASH_TABLE));
  SHASH_ENTRY *table_array = (SHASH_ENTRY *) malloc(hash_size * sizeof(struct SHASH_ENTRY));
  (*hash_table).size = hash_size;
  (*hash_table).entries = 0;
  (*hash_table).table = table_array;
  for (int i = 0; i < (*hash_table).size; i++) {
    (*hash_table).table[i].empty = 1;
    (*hash_table).table[i].key = NULL;
    (*hash_table).table[i].value = NULL;
    (*hash_table).table[i].next = NULL; 
  }
  return(hash_table);
}

void shash_insert(SHASH_TABLE* hash_table, char* key, char* value)
{
  SHASH_ENTRY *entry;

  entry = &((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if ((*entry).empty) {
    (*hash_table).entries++;
    (*entry).empty = 0;
    (*entry).key = strdup(key);
    (*entry).value = strdup(value); }
  else if (strcmp((*entry).key,key) == 0) {
    free((*entry).value);
    (*entry).value = strdup(value); }
  else {
    while ((*entry).next) {
      entry = (*entry).next;
      if (strcmp((*entry).key,key) == 0) {
	free((*entry).value);
	(*entry).value = strdup(value);
	return; }}
    (*entry).next = (SHASH_ENTRY *) malloc(sizeof(struct SHASH_ENTRY));
    (*hash_table).entries++;
    (*entry).next->empty = 0;
    (*entry).next->key = strdup(key);
    (*entry).next->value = strdup(value);
    (*entry).next->next = NULL;
  }
}

char* shash_lookup(SHASH_TABLE* hash_table, char* key)
{
  int position;
  SHASH_ENTRY entry;
  
  position = hash_value(key,(*hash_table).size);
  entry = ((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if (entry.key) { 
    if(strcmp(entry.key,key) == 0) {
      return(entry.value); }
    else {
      while (entry.next) {
	entry = *entry.next;
	if(strcmp(entry.key,key) == 0) {
	  return(entry.value); }}}
  }
  printf("Hash table lookup: Could not find entry with key %s, returning NULL.\n",key);
  return(NULL);
}

SS_LIST *shash_to_list(SHASH_TABLE* hash_table)
{
  SHASH_ENTRY entry;
  SS_LIST *list_start = NULL;

  SS_LIST *node = (SS_LIST *) malloc(sizeof(struct SS_LIST));
  for(int i = 0; i < (*hash_table).size; i++) {
    entry = (*hash_table).table[i];
    if (entry.key != NULL) {
      if (list_start == NULL) { list_start = node; }
      (*node).key = strdup(entry.key);
      (*node).value = strdup(entry.value);
      (*node).next = (SS_LIST *) malloc(sizeof(struct SS_LIST));
      node = (*node).next;
      (*node).key = NULL;
      (*node).value = NULL;
      (*node).next = NULL;
      while (entry.next) {
	entry = *entry.next;
	(*node).key = strdup(entry.key);
	(*node).value = strdup(entry.value);
	(*node).next = (SS_LIST *) malloc(sizeof(struct SS_LIST));
	node = (*node).next;
	(*node).key = NULL;
	(*node).value = NULL;
	(*node).next = NULL;
      }
    }
  }
  if (list_start == NULL) { 
    free(node); }
  else {
    node = list_start;
    while ((*node).next) {
      printf("value = %s,  key = %s\n",(*node).value,(*node).key); 
      node = (*node).next; }}
  return(list_start);
}

void shash_print(SHASH_TABLE* hash_table)
{
  SHASH_ENTRY entry;

  printf("Hash table size: %i, entries: %i\n",(*hash_table).size,(*hash_table).entries);
  for(int i = 0; i < (*hash_table).size; i++) {
    if (!(*hash_table).table[i].empty) {
      printf("hash_table[%i][0].value = %s,  key = %s\n",i,(*hash_table).table[i].value,(*hash_table).table[i].key);
      fflush(stdout);
      entry = (*hash_table).table[i];
      int j = 1;
      while (entry.next) {
	entry = *entry.next;
	printf("hash_table[%i][%i].value = %s,  key = %s\n",i,j++,entry.value,entry.key);
	fflush(stdout); }
    }
  }
}

/**********************************************************************/
/*** Functions for hash table that stores generic value structures  ***/
/**********************************************************************/

void hash_remove() {}

HASH_TABLE* hash_create(int hash_size) 
{
  HASH_TABLE *hash_table = (HASH_TABLE *) malloc(sizeof(struct HASH_TABLE));
  HASH_ENTRY *table_array = (HASH_ENTRY *) malloc(hash_size * sizeof(struct HASH_ENTRY));
  (*hash_table).size = hash_size;
  (*hash_table).entries = 0;
  (*hash_table).table = table_array;
  for (int i = 0; i < (*hash_table).size; i++) {
    (*hash_table).table[i].empty = 1;
    (*hash_table).table[i].key = NULL;
    (*hash_table).table[i].value = NULL;
    (*hash_table).table[i].next = NULL; 
  }
  return(hash_table);
}

void hash_insert(HASH_TABLE* hash_table, char* key, intptr_t* value)
{
  HASH_ENTRY *entry;

  entry = &((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if ((*entry).empty) {
    (*hash_table).entries++;
    (*entry).empty = 0;
    (*entry).key = strdup(key);
    (*entry).value = value; }
  else if (strcmp((*entry).key,key) == 0) {
    (*entry).value = value; }
  else {
    while ((*entry).next) {
      entry = (*entry).next;
      if (strcmp((*entry).key,key) == 0) {
	(*entry).value = value;
	return; }}
    (*entry).next = (HASH_ENTRY *) malloc(sizeof(struct HASH_ENTRY));
    (*hash_table).entries++;
    (*entry).next->empty = 0;
    (*entry).next->key = strdup(key);
    (*entry).next->value = value;
    (*entry).next->next = NULL;
  }
}

intptr_t* hash_lookup(HASH_TABLE* hash_table, char* key)
{
  int position;
  HASH_ENTRY entry;
  
  position = hash_value(key,(*hash_table).size);
  entry = ((*hash_table).table)[hash_value(key,(*hash_table).size)];
  if (!entry.empty) {
    if (strcmp(entry.key,key) == 0) {
      return(entry.value); }
    else {
      while (entry.next) {
	entry = *entry.next;
	if(strcmp(entry.key,key) == 0) {
	  return(entry.value); }}
    }
  }
  //  printf("Hash table lookup warning: Could not find entry with key %s.\n",key);
  //  fflush(stdout);
  return(NULL);
}

SI_LIST *hash_to_list(HASH_TABLE* hash_table)
{
  HASH_ENTRY entry;
  SI_LIST *list_start = NULL, *prev = NULL;

  SI_LIST *node = (SI_LIST *) malloc(sizeof(struct SI_LIST));
  for(int i = 0; i < (*hash_table).size; i++) {
    entry = (*hash_table).table[i];
    if (entry.key != (char*)'\0') {
      if (list_start == NULL) { list_start = node; }
      (*node).key = strdup(entry.key);
      (*node).value = (intptr_t) entry.value;
      (*node).next = (SI_LIST *) malloc(sizeof(struct SI_LIST));
      prev = node;
      node = (*node).next;
      (*node).key = NULL;
      (*node).value = 0;
      (*node).next = NULL;
      while (entry.next) {
	entry = *entry.next;
	(*node).key = strdup(entry.key);
	(*node).value = (intptr_t) entry.value;
	(*node).next = (SI_LIST *) malloc(sizeof(struct SI_LIST));
	prev = node;
	node = (*node).next;
	(*node).key = NULL;
	(*node).value = 0;
	(*node).next = NULL;
      }
    }
  }
  free(node);
  if (prev != NULL) {
    (*prev).next = NULL; }
  return(list_start);
}

void hash_print(HASH_TABLE* hash_table, int* funct)
{
  // funct is a function pointer, being passed as an int pointer
  // why, I don't know -- it's the magic of atacs

  HASH_ENTRY entry;
  void (*func)(int *);

  func = (void (*)(int *))(funct);
  printf("Hash table size: %i, entries: %i\n",(*hash_table).size,(*hash_table).entries);
  for(int i = 0; i < (*hash_table).size; i++) {
    if (!(*hash_table).table[i].empty) {
      printf("[%i][0]: %s",i,(*hash_table).table[i].key);
      fflush(stdout);
      func((int *)((*hash_table).table[i].value));
      entry = (*hash_table).table[i];
      int j = 1;
      while (entry.next) {
	entry = *entry.next;
	printf("[%i][%i]: %s",i,j++,entry.key);
	fflush(stdout);
	func((int *)(entry.value)); }
    }
  }
}







