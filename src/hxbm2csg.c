#include "hxbm2csg.h"
#include "findreg.h"
#include "def.h"
#include "misclib.h"
#include "memaloc.h"
#include "findrsg.h"
#include "hhash.h"
#include "hmisc.h"

/************** GLOBAL VARIABLES: **************/

bool debug = 0;
bool burst2l = 0;
int hash_size = 63;
int vector_size = 0;
int number_of_inputs = 0;
IHASH_TABLE *signal_position, *signal_isoutput, *signal_initval, *signal_islevel, *vecpos_islevel, *place_visited, *place_rename;
SHASH_TABLE *vecpos_signal;
HASH_TABLE *xbm_graph, *csg_graph;
int last_burst_type = UNDEF;

/************* FUNCTION DECLARATIONS **********/

void parse_file(FILE* infile);
void calculate_state_vectors(char *prev_vector, char* curr_state);
bool store_csg(designADT design, FILE *outfile);


bool load_extended_burstmode_graph(designADT design)
{
  FILE *infile, *outfile;
  char *inname, *outname;
  bool status;

  burst2l = design->burst2l;
  inname = (char *) malloc((strlen((*design).filename) + 5) * sizeof(char));
  inname = strcpy(inname,(*design).filename);
  inname = strcat(inname,".unc\0");
  outname = (char *) malloc((strlen((*design).filename) + 5) * sizeof(char));
  outname = strcpy(outname,(*design).filename);
  outname = strcat(outname,".rsg\0");
  if ((infile = fopen(inname,"r")) == NULL) {
    printf("Error: cannot open burstmode file '%s'\n",inname); 
    fprintf(lg,"Error: cannot open burstmode file '%s'\n",inname); 
    return(FALSE); }
  else {
    printf("Reading burstmode file %s\n",inname);
    fprintf(lg,"Reading burstmode file %s\n",inname); }

  signal_position = ihash_create(hash_size);
  vecpos_signal = shash_create(hash_size);
  signal_initval = ihash_create(hash_size);
  signal_isoutput = ihash_create(hash_size);
  signal_islevel = ihash_create(hash_size);
  vecpos_islevel = ihash_create(hash_size);
  place_visited = ihash_create(hash_size);
  place_rename = ihash_create(hash_size);
  xbm_graph = hash_create(hash_size);
  parse_file(infile);

  if (debug) {
    //printf("XBM GRAPH: "); hash_print(xbm_graph,(int *)print_gtrans);
    //printf("SIGNAL POSITION: "); ihash_print(signal_position);
    //printf("SIGNAL INITVAL: "); ihash_print(signal_initval);
    //printf("SIGNAL ISOUTPUT: "); ihash_print(signal_isoutput);
    //printf("SIGNAL ISLEVEL: "); ihash_print(signal_islevel);
    //printf("VECPOS ISLEVEL: "); ihash_print(vecpos_islevel);
    //printf("PLACE VISITED: "); ihash_print(place_visited);
    fflush(stdout);
  }
	
  // build initial signal vector
  char *signal_vector = (char *) malloc((vector_size + 1) * sizeof(char));
  signal_vector[vector_size] = '\0';
  SI_LIST *signal_position_list = ihash_to_list(signal_position);
  if (signal_position_list == NULL) { printf("Error: No signals declared\n"); return(FALSE); }
  while (signal_position_list) {
    int initval = ihash_lookup(signal_initval,(*signal_position_list).key);
    if (ihash_lookup(signal_islevel,(*signal_position_list).key)) {
      signal_vector[(*signal_position_list).value] = 'X'; }
    else {
      char *value = itoa(initval);
      signal_vector[(*signal_position_list).value] = value[0];
      free(value); }
    signal_position_list = (*signal_position_list).next;
  }
  //if (debug) { printf("INIT VECTOR: %s\n",signal_vector); }
  design->startstate = CopyString(signal_vector);
  calculate_state_vectors(signal_vector,"0");
  if (debug) { printf("XBM GRAPH: "); hash_print(xbm_graph,(int *)print_gtrans); }

  // Open output file and write graph
  if ((*design).verbose) {
    if ((outfile = fopen(outname,"w")) == NULL) {
      printf("Warning: cannot open output file '%s'\n",outname);
      fprintf(lg,"Warning: cannot open output file '%s'\n",outname); }
    else {
      printf("Writing file %s\n",outname);
      fprintf(lg,"Writing file %s\n",outname); }}
  status = store_csg(design,outfile);

  return(status);
}  


/**************************************************************************/
/*** Store graph to ATACS designADT RSG format                          ***/
/**************************************************************************/

bool store_csg(designADT design, FILE *outfile) 
{
  SI_LIST *start_place_list, *place_list;
  GTRANS *trans, *nextplace_trans;
  int transno, placeno, nextplace_transno, statecount = 0, edgecount = 0;
  char *signal;
  // ATACS declarations
  statelistADT statelist;
  stateADT curstate, prevstateptr, nextstateptr;
  char *vstate;

  start_place_list = hash_to_list(xbm_graph);
  if((*design).verbose) fprintf(outfile,"SG:\nSTATEVECTOR:");
  (*design).nsignals = vector_size;
  (*design).ninpsig = number_of_inputs;
  /*** Some ATACS specific initialization stuff ***/
  design->signals=new_signals(design->status,FALSE,design->oldnsignals,
			      design->nsignals,design->signals);
  initialize_state_table(design->status,FALSE,design->state_table);
  design->regions=new_regions(design->status,FALSE,(2*design->oldnsignals)+1,
			      (2*design->nsignals)+1,design->regions);
  design->oldnsignals=design->nsignals;
  /***/
  for (int i = 0; i < vector_size; i++) {
    signal = shash_lookup(vecpos_signal,itoa(i));
    // Insert signals into ATACS designADT
    (*design).signals[i]->name = strdup(signal);
    if (ihash_lookup(signal_islevel,signal)) {
      (*design).signals[i]->is_level = TRUE; }
    if (ihash_lookup(signal_isoutput,signal)) {
      if((*design).verbose) fprintf(outfile,"%s ",signal); }
    else {
      if((*design).verbose) fprintf(outfile,"INP %s ",signal); }}
  if((*design).verbose) fprintf(outfile,"\n");
  /*** Some ATACS BDD initialization stuff ***/
#ifdef DLONG
  initialize_bdd_state_table((*design).status,FALSE,(*design).bdd_state,(*design).signals,(*design).nsignals,(*design).nrules);
#else
  //CUDD
  design->bdd_state=new_bdd_state_table((*design).status,FALSE,(*design).bdd_state,(*design).signals,
					(*design).nsignals,(*design).ninpsig,0,(*design).filename);
  //  bdd_state=design->bdd_state;
#endif 
  /***/
  // Rename place numbers to contiguous integers (and reserve numbers for choices)
  // Also print states
  if((*design).verbose) fprintf(outfile,"STATES:\n");
  placeno = 0;
  place_list = start_place_list;
  char *trans_vector = (char *) malloc((vector_size + 1) * sizeof(char));
  char *nextplace_trans_vector = (char *) malloc((vector_size + 1) * sizeof(char));
  while (place_list) {
    trans = (GTRANS *) (*place_list).value;
    transno = 0;
    while (trans) { 
      statecount++;
      trans_vector = strcpy(trans_vector,(*trans).statevector);
      for (int i = 0; i < vector_size; i++) {
	if (trans_vector[i] == 'R') { trans_vector[i] = 'U'; } 
	else if (trans_vector[i] == 'F') { trans_vector[i] = 'D'; }}
      if((*design).verbose) fprintf(outfile,"%i:%s\n",(placeno+transno),trans_vector);
      // Insert state into ATACS designADT
      add_state(NULL,(*design).state_table,NULL,NULL,NULL, NULL, NULL, 0, NULL, 
		trans_vector,NULL,NULL, (*design).nsignals, NULL, NULL, 0, (placeno+transno), FALSE,
		FALSE,0,0,0,(*design).bdd_state,(*design).bdd,(*design).markkey,(*design).timeopts, -1, -1, 
		NULL, NULL, 0);
      transno++;
      trans = (*trans).next; }
    ihash_insert(place_rename,(*place_list).key,placeno);
    if (debug) { printf("key: %s -> %i\n",(*place_list).key,placeno); }
    placeno += transno;
    place_list = (*place_list).next;
  }
  // Print edges
  if((*design).verbose) fprintf(outfile,"EDGES:\n");
  place_list = start_place_list;
  while (place_list) {
    transno = ihash_lookup(place_rename,(*place_list).key);
    trans = (GTRANS *) (*place_list).value;
    while (trans) { 
      trans_vector = strcpy(trans_vector,(*trans).statevector);
      nextplace_trans = (GTRANS *) hash_lookup(xbm_graph,(*trans).nextplace);
      nextplace_transno = ihash_lookup(place_rename,(*trans).nextplace);
      while (nextplace_trans) {
	edgecount++;
	nextplace_trans_vector = strcpy(nextplace_trans_vector,(*nextplace_trans).statevector);
	for (int i = 0; i < vector_size; i++) {
	  if (trans_vector[i] == 'R') { trans_vector[i] = 'U'; } 
	  else if (trans_vector[i] == 'F') { trans_vector[i] = 'D'; }
	  if (nextplace_trans_vector[i] == 'R') { nextplace_trans_vector[i] = 'U'; } 
	  else if (nextplace_trans_vector[i] == 'F') { nextplace_trans_vector[i] = 'D'; }}
	if((*design).verbose) fprintf(outfile,"%i:%s -> %i:%s\n",transno,trans_vector,nextplace_transno,nextplace_trans_vector);
	// Insert edge into ATACS designADT
	int j = transno;
	vstate = SVAL(trans_vector,(*design).nsignals);
	int i = hashpjw(vstate);
	free(vstate);
	prevstateptr = NULL;
	for (curstate = (*design).state_table[i]; 
	     curstate != NULL && curstate->state != NULL;
	     curstate = curstate->link) {
	  if (curstate->number == j) { prevstateptr = curstate; }}
	int k = nextplace_transno;
	vstate = SVAL(nextplace_trans_vector,(*design).nsignals);
	int l = hashpjw(vstate);
	free(vstate);
	nextstateptr = NULL;
	for (curstate = (*design).state_table[l];
	     curstate != NULL && curstate->state != NULL;
	     curstate = curstate->link) {
	  if (curstate->number == k) { nextstateptr = curstate; }}
	if (!prevstateptr) { printf("ERROR: Could not find %d:%s.\n",j,trans_vector); return(FALSE); }
	if (!nextstateptr) { printf("ERROR: Could not find %d:%s.\n",k,nextplace_trans_vector); return(FALSE); }
	if (prevstateptr) {
	  if (prevstateptr->succ) {
	    for (statelist = prevstateptr->succ; 
		 statelist->next; statelist = statelist->next);
	    statelist->next=(statelistADT) GetBlock(sizeof (*(prevstateptr->succ)));
	    statelist->next->enabling=0;
	    statelist->next->enabled=0;
	    statelist->next->probability=0;
	    statelist->next->stateptr=nextstateptr;
	    statelist->next->next=NULL;
	  } else {
	    prevstateptr->succ=(statelistADT) GetBlock(sizeof (*(prevstateptr->succ)));
	    prevstateptr->succ->enabling=0;
	    prevstateptr->succ->enabled=0;
	    prevstateptr->succ->probability=0;
	    prevstateptr->succ->stateptr=nextstateptr;
	    prevstateptr->succ->next=NULL; }} 
	if (nextstateptr) {
	  if (nextstateptr->pred) {
	    for (statelist=nextstateptr->pred; 
		 statelist->next; statelist=statelist->next);
	    statelist->next=(statelistADT) GetBlock(sizeof (*(nextstateptr->pred)));
	    statelist->next->enabling=0;
	    statelist->next->enabled=0;
	    statelist->next->probability=0;
	    statelist->next->stateptr=prevstateptr;
	    statelist->next->next=NULL;
	  } else {
	    nextstateptr->pred=(statelistADT) GetBlock(sizeof (*(nextstateptr->pred)));
	    nextstateptr->pred->enabling=0;
	    nextstateptr->pred->enabled=0;
	    nextstateptr->pred->probability=0;
	    nextstateptr->pred->stateptr=prevstateptr;
	    nextstateptr->pred->next=NULL; }}
	//
	nextplace_transno++;
	nextplace_trans = (*nextplace_trans).next;
      }
      transno++;
      trans = (*trans).next;
    }
    place_list = (*place_list).next;
  }
  printf("states = %i transitions = %i\n",statecount,edgecount);
  fprintf(lg,"states = %i transitions = %i\n",statecount,edgecount);
  /*** Some ATACS status check stuff ***/
  design->status=(LOADED | CONNECTED | EXPANDED | FOUNDRED);
  if (!(design->atomic && design->burstgc))
    if (check_csc(design->filename,design->signals,design->state_table,
		  design->nsignals,design->ninpsig,design->verbose)) {
      design->status=(design->status | FOUNDSTATES | FOUNDRSG);
      return(TRUE); } 
    else {
      return (FALSE); 
    }
  else {
    design->status |= (FOUNDSTATES | FOUNDRSG);
    return(TRUE); } 
  /***/
}

/**************************************************************************/
/*** Calculates the state vectors of the XBM graph                      ***/
/**************************************************************************/

void calculate_state_vectors(char *vector, char *place_name)
{

  GTRANS *curr_gtrans, *start_gtrans;
  SIGNAL_LIST *signal_list;
  char *curr_vector, *prev_vector;
  char *curr_place_name;

  prev_vector = strdup(vector);
  curr_place_name = strdup(place_name);
  ihash_insert(place_visited,curr_place_name,1);
  curr_gtrans = (GTRANS *) hash_lookup(xbm_graph,curr_place_name);
  start_gtrans = curr_gtrans;

  while (curr_gtrans) {
    curr_vector = strdup(prev_vector);
    // Reset all R and F's in the new vector
    for(int i = 0; i < vector_size; i++) {
      if (curr_vector[i] == 'R') {
	curr_vector[i] = '1'; }
      else if (curr_vector[i] == 'F') {
	curr_vector[i] = '0'; }}
    // Reset all level signals to X's if the place is a new inburst
    // or an outburst following a stateburst.
    if (((*curr_gtrans).placetype == INBURST) || 
	((last_burst_type == STATEBURST) && ((*curr_gtrans).placetype == OUTBURST))) {
      for(int i = 0; i < vector_size; i++) {
	if (ihash_lookup(vecpos_islevel,itoa(i))) {
	  curr_vector[i] = 'X'; }}}
    last_burst_type = (*curr_gtrans).placetype;
    // Set signals enabled in current transition
    signal_list = (*curr_gtrans).outtrans;
    while (signal_list) {
      int sigpos = ihash_lookup(signal_position,(*signal_list).name);
      if (((*signal_list).value == RISING) && ((prev_vector[sigpos] == '0') || (prev_vector[sigpos] == 'F') || (prev_vector[sigpos] == 'U'))) {
	curr_vector[sigpos] = 'R'; }
      else if (((*signal_list).value == FALLING) && ((prev_vector[sigpos] == '1') || (prev_vector[sigpos] == 'R') || (prev_vector[sigpos] == 'D'))) {
	curr_vector[sigpos] = 'F'; }
      else if (((*signal_list).value == DDC) && ((prev_vector[sigpos] == '0') || (prev_vector[sigpos] == 'F') || (prev_vector[sigpos] == 'U'))) {
	curr_vector[sigpos] = 'U'; }
      else if (((*signal_list).value == DDC) && ((prev_vector[sigpos] == '1') || (prev_vector[sigpos] == 'R') || (prev_vector[sigpos] == 'D'))) {
	curr_vector[sigpos] = 'D'; }
      else if ((*signal_list).value == HIGH) {
	curr_vector[sigpos] = '1'; }
      else if ((*signal_list).value == LOW) {
	curr_vector[sigpos] = '0'; }
      else {
	char chr = '\0';
	if ((*signal_list).value == RISING) { chr = '+'; }
	else if ((*signal_list).value == FALLING) { chr = '-'; }
	else if ((*signal_list).value == DDC) { chr = '*'; }
	printf("value = %i, prev = %c\n", (*signal_list).value, prev_vector[sigpos]);
	printf("Error: Unexpected transition for signal %s%c in state transition %s -> %s\n",
	       (*signal_list).name,chr,curr_place_name,(*curr_gtrans).nextplace); exit(1); }
      signal_list = (*signal_list).next; }
    if (debug) { printf("%s\t: %s -> %s\n",curr_place_name,curr_vector,(*curr_gtrans).nextplace); }
    (*curr_gtrans).statevector = strdup(curr_vector);
    if (debug) { printf("PREV VECTOR: %s\n",prev_vector); }
    if (debug) { printf("CURR VECTOR: %s\n",curr_vector); }
    fflush(stdout);
    // Recurse
    if (!ihash_lookup(place_visited,(*curr_gtrans).nextplace)) { 
      calculate_state_vectors(curr_vector,(*curr_gtrans).nextplace); }
    last_burst_type = (*curr_gtrans).placetype;
    curr_gtrans = (*curr_gtrans).next;
  }
  hash_insert(xbm_graph,curr_place_name,(intptr_t *) start_gtrans);
}

/**************************************************************************/
/*** Parses input xbm file and records data into the signal_position,   ***/
/*** signal_initval, signal_isoutput, and xbm_graph global hash tables. ***/
/*** XBM states are stored in a hash table by their state number.       ***/
/*** The multiple bursts of choice states are stored as linked list of  ***/
/*** GTRANS, the list being stored in the hash table under their common ***/
/*** state number.                                                      ***/
/**************************************************************************/

void parse_file(FILE* infile)
{
  int lineno = 0;
  int sigpos = 0;
  int pipe_tokens_seen, ddc_found;
  char line[MAXLINE];
  char str[MAXSTRING];
  char chr;
  char *lineptr;
  char *curr_place, *next_place, *stateburst_place, *outburst_place;
  GTRANS *start_place, *tmp_place;
  SIGNAL_LIST *burst, *tmpburst, *inburst, *stateburst, *stateburst_last_entry, *outburst, *outburst_last_entry, *ddcburst, *prev;
  STRING_LIST *list, *entry;

  number_of_inputs = 0;

 // Parse file
  while ((lineptr = fgets(line, MAXLINE, infile)) != NULL) {
    lineno++;
    list = split_line(lineptr);
    entry = list;
    if (debug) {
      while((*entry).str) {
	printf("%s ",(*entry).str);
	entry = (*entry).next; }
      printf("\n"); }
    // 
    if ((*list).str != NULL) {
      if ((*list).str[0] == 'i' || (*list).str[0] == 'I') {
	if (((*list).next->str == NULL) || ((*list).next->next->str == NULL)) {
	  printf("Error: Error in input declaration on line %i\n",lineno); exit(1); }
	number_of_inputs++;
	shash_insert(vecpos_signal,itoa(sigpos),(*list).next->str);
	ihash_insert(signal_position,(*list).next->str,sigpos++);
	ihash_insert(signal_initval,(*list).next->str,atoi((*list).next->next->str));
	ihash_insert(signal_isoutput,(*list).next->str,0);
	if (debug) { printf("Input declaration\n"); }}
      else if ((*list).str[0] == 'o' || (*list).str[0] == 'O') {
	if (((*list).next->str == NULL) || ((*list).next->next->str == NULL)) {
	  printf("Error: Error in output declaration on line %i\n",lineno); exit(1); }
	shash_insert(vecpos_signal,itoa(sigpos),(*list).next->str);
	ihash_insert(signal_position,(*list).next->str,sigpos++);
	ihash_insert(signal_initval,(*list).next->str,atoi((*list).next->next->str));
	ihash_insert(signal_isoutput,(*list).next->str,1);
	if (debug) { printf("Output declaration\n"); }}
      else if ((*list).str[0] >= '0' && (*list).str[0] <= '9') {
	if ((*list).next->str == NULL) {
	  printf("Error: Empty next state on line %i\n",lineno); exit(1); }
	curr_place = strdup((*list).str);
	next_place = strdup((*list).next->str);
	ihash_insert(place_visited,curr_place,0);
	// Build signal lists of input and output bursts
	burst = (SIGNAL_LIST *) malloc(sizeof(struct SIGNAL_LIST));
	(*burst).name = NULL;
	(*burst).value = UNDEF;
	(*burst).type = UNDEF;
	(*burst).next = NULL;
	inburst = burst;
	prev = burst;
	ddcburst = NULL;
	outburst = NULL;
	stateburst = NULL;
	entry = (*list).next->next;
	pipe_tokens_seen = 0;
       	while((*entry).str) {
      	  if ((*entry).str[0] == '|') { 
	    pipe_tokens_seen++;
	    if (debug) { printf("\n"); }
	    (*prev).next = NULL;    // Set last entry.next in inburst/stateburst to NULL
	    // Record place in burst list where state and output bursts start.
	    if (pipe_tokens_seen == 1) {
	      outburst = burst; }   // Use the last alloc entry (which is unused) from inburst
	    else if (pipe_tokens_seen == 2) {
	      if ((*entry).next->str) {  
		// Only create stateburst if there really is an outburst following the second pipe token.
		stateburst_last_entry = prev;
		(*stateburst_last_entry).next = NULL;
		stateburst = outburst;
		outburst = burst; }}}
	  else {
	    if (strpbrk((*entry).str,"[")) { 
	      (*burst).type = LEVEL; 
	      sscanf((*entry).str,"[%[A-Za-z0-9_.]%c]",str,&chr);
	      (*burst).name = strdup(str);
	      ihash_insert(signal_islevel,(*burst).name,1);
	      ihash_insert(vecpos_islevel,itoa(ihash_lookup(signal_position,(*burst).name)),1);
	      if (chr == '+') { (*burst).value = HIGH; }
	      else if (chr == '-') { (*burst).value = LOW; }
	      else { printf("ERROR: Illegal polarity of level signal %s\n",(*burst).name); exit(1); }}
	    else if (strpbrk((*entry).str,"*+-")) { 
	      (*burst).type = EDGE;
	      sscanf((*entry).str,"%[A-Za-z0-9_.]%c",str,&chr);
	      (*burst).name = strdup(str);
	      if (chr == '+') { (*burst).value = RISING; }
	      else if (chr == '-') { (*burst).value = FALLING; }
	      else if (chr == '*') { (*burst).value = DDC; }}
	    else { printf("ERROR: Illegal polarity of signal %s\n",(*burst).name); exit(1); }
	    (*burst).next = (SIGNAL_LIST *) malloc(sizeof(struct SIGNAL_LIST));
	    prev = burst;
	    burst = (*burst).next;
	    (*burst).name = NULL;
	    (*burst).value = UNDEF;
	    (*burst).type = UNDEF;
	    (*burst).next = NULL; }
	  if (debug) { printf("%s ",(*entry).str); }
	  entry = (*entry).next;
	}
	outburst_last_entry = prev;
	(*outburst_last_entry).next = NULL;    // Set last entry.next in outburst to NULL

	if ((*inburst).name == NULL) {
	  printf("ERROR: Empty inburst on line %i\n",lineno); exit(1); }
	if ((*outburst).name == NULL) {
	  if (debug) { printf("Empty outburst\n"); }
	  stateburst = NULL;
	  outburst = NULL;
	  ddcburst = NULL;
	  free(burst);
	  outburst_place = strdup(next_place);
	  if (debug) { printf("OUTBURST_PLACE: %s\n",outburst_place); }
	  if (debug) { if (stateburst == NULL) { printf("Empty stateburst\n"); }}}
	else {
	  char *linenostr = itoa(lineno);
	  stateburst_place = (char *) malloc((strlen(next_place) + 2 + strlen(linenostr) + 1) * sizeof(char));
	  stateburst_place[0] = '\0';
	  stateburst_place = strcat(stateburst_place,next_place);
	  stateburst_place = strcat(stateburst_place,"__");
	  stateburst_place = strcat(stateburst_place,linenostr);
	  outburst_place = (char *) malloc((strlen(next_place) + 1 + strlen(linenostr) + 1) * sizeof(char));
	  outburst_place[0] = '\0';
	  outburst_place = strcat(outburst_place,next_place);
	  outburst_place = strcat(outburst_place,"_");
	  outburst_place = strcat(outburst_place,linenostr);
	  if (debug) { printf("STATEBURST_PLACE: %s\nOUTBURST_PLACE: %s\n",stateburst_place,outburst_place); }
	  fflush(stdout);
	  // Record DDC's to later append to state and output burst
	  ddcburst = burst;  // 'burst' is the last (and empty) entry of outburst list
	  prev = burst;
	  tmpburst = inburst;
	  ddc_found = 0;
	  while (tmpburst) {
	    if ((*tmpburst).value == DDC) {
	      ddc_found = 1;
	      (*burst).name = strdup((*tmpburst).name);
	      (*burst).value = (*tmpburst).value;
	      (*burst).type = (*tmpburst).type;
	      (*burst).next = (SIGNAL_LIST *) malloc(sizeof(struct SIGNAL_LIST));
	      prev = burst;
	      burst = (*burst).next;
	      (*burst).name = NULL;
	      (*burst).value = UNDEF;
	      (*burst).type = UNDEF;
	      (*burst).next = NULL; }
	    tmpburst = (*tmpburst).next; }
	  (*prev).next = NULL;  // Set last entry.next in ddcburst to NULL
	  free(burst);
	  if (! ddc_found) { ddcburst = NULL; }
	}
	// Insert input burst in hash table
	GTRANS *place = (GTRANS *) malloc(sizeof(struct GTRANS));
	(*place).placetype = INBURST;
	(*place).outtrans = inburst;
	(*place).next = NULL;
	if (stateburst) { (*place).nextplace = strdup(stateburst_place); }
	else { (*place).nextplace = strdup(outburst_place); }
	if ((start_place = (GTRANS *) hash_lookup(xbm_graph,curr_place)) == NULL) {
	  start_place = place; }
	else {
	  tmp_place = start_place;
	  while ((*tmp_place).next) {
	    tmp_place = (*tmp_place).next; }
	  (*tmp_place).next = place; }
	hash_insert(xbm_graph,curr_place,(intptr_t *) start_place);
	// Insert output burst in hash table
	if (stateburst) {
	  (*stateburst_last_entry).next = ddcburst;
	  GTRANS *place = (GTRANS *) malloc(sizeof(struct GTRANS));
	  (*place).placetype = STATEBURST;
	  (*place).nextplace = strdup(outburst_place);
	  (*place).outtrans = stateburst;
	  (*place).next = NULL;
	  hash_insert(xbm_graph,stateburst_place,(intptr_t *) place);
	}
	if (outburst) {
	  (*outburst_last_entry).next = ddcburst;
	  GTRANS *place = (GTRANS *) malloc(sizeof(struct GTRANS));
	  (*place).placetype = OUTBURST;
	  (*place).nextplace = strdup(next_place);
	  (*place).outtrans = outburst;
	  (*place).next = NULL;
	  hash_insert(xbm_graph,outburst_place,(intptr_t *) place);
	}
	free(curr_place);
	free(next_place);	    
	if (debug) { printf("Transition\n\n"); }}
      else {
	printf("Error: Parse error on line %i\n",lineno);
	exit(1); }}
    else {
      if (debug) { printf("Empty line\n"); }}
    free(list);
  }
  vector_size = sigpos;
}
