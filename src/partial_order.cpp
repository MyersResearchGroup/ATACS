#include "partial_order.h"

using namespace std;
using namespace __gnu_cxx;

int nathan_debug = 0;

int num_states_visited = 0;


const int FAIL = 0;
const int DEADLOCK = -1;
const int OUT_WITHOUT_IN = -2;
const int SUCCESS = 1;

//Scraped.  turn the hash map into a hash of ints.  The code will be the base 10 representation of the number - but there is overflow even on this small example, so no.
//When firing output i, we have an input i/1 and i/2 that are able.  (2 can fire) -> error

//[row][column]

void emit(char*c,int i, int j, char*d){
  if (nathan_debug){
    printf (c,i,j,d);
  }
}

void emit(char*c,char*d,int i, int j){
  if (nathan_debug){
    printf(c,d,i,j);
  }
}

void emit(char*c,char*d,int i){
  if (nathan_debug){
    printf(c,d,i);
  }
}

void emit(char*c,char* d,char*e){
  if (nathan_debug){
    printf(c,d,e);
  }
}

void emit(char*c,char* d){
  if (nathan_debug){
    printf(c,d);
  }
}


void emit(char*c,int i,int j,int k, int l, int m,int n){
  if (nathan_debug){
    printf(c,i,j,k,l,m,n);
  }
}

void emit(char*c,int i,int j,int k, int l, int m){
  if (nathan_debug){
    printf(c,i,j,k,l,m);
  }
}

void emit(char*c,int i,int j,int k, int l){
  if (nathan_debug){
    printf(c,i,j,k,l);
  }
}


void emit(char*c,int i,int j, int k){
  if (nathan_debug){
    printf(c,i,j,k);
  }
}

void emit(char*c,int i,int j){
  if (nathan_debug){
    printf(c,i,j);
  }
}

void emit(char*c,int i){
  if (nathan_debug){
    printf(c,i);
  }
}


void emit(char*c){
  if (nathan_debug){
    printf(c);
  }
}

int is_in(list<int>*l,int i){
  for (list<int>::iterator iter = l->begin(); iter != l->end(); iter++){
    if (*iter == i){
      return 1;
    }
  }
  return 0;
}


void get_global_state(designADT d1, designADT d2, list<int>*l1,list<int>*l2){
  char * c1 = new char[d1->nevents+d1->nplaces];
  char * c2 = new char[d2->nevents+d2->nplaces];

  emit("\tGetting the global state of '");

  for (int i = 0; i < d1->nevents+d1->nplaces; i++){
    c1[i]='0';
  }
  for (int i = 0; i < d2->nevents+d2->nplaces; i++){
    c2[i]='0';
  }

  for (int i = 0; i < d1->nrules; i++){
    if (d1->markkey[i]->epsilon == 1){
      c1[d1->markkey[i]->enabling] = '1';
    }
  } 
  for (int i = 0; i < d2->nrules; i++){
    if (d2->markkey[i]->epsilon == 1){
      c2[d2->markkey[i]->enabling] = '1';
    }
  }
  for (int i = 0; i < d1->nevents+d1->nplaces; i++){
    if (c1[i]=='1'){
      emit(" %i",i);
      l1->push_back(i);
    }
  }
  emit(" ' and '");
  for (int i = 0; i < d2->nevents+d2->nplaces; i++){
    if (c2[i]=='1'){
      emit(" %i",i);
      l2->push_back(i);
    }
  }
  emit(" '\n");
  delete [] c1;
  delete [] c2;
  return;
}

void merge(list<int>*l1,list<int>*l2){
  l1->sort();
  l2->sort();
  list<int>::iterator iter1 = l1->begin();
  list<int>::iterator iter2 = l2->begin();
  while (iter2 != l2->end()){
    if (*iter2 < *iter1 || iter1 == l1->end()){
      l1->insert(iter1,*iter2);
      iter2++;
    }
    else{
      iter1++;
    }
  }

  l1->unique();
  return;
}

int names_match(char* c1, char* c2){
  int i = 0;
  //emit("\t\tChecking before the slash, '%s' against '%s'\n",c1,c2);

  while(c1[i] != 0 && c1[i] != '/' ){
    if (c1[i] != c2[i]){
      return 0;
    }
    i++;
  }
  //There could still be a longer name for c2
  if (c2[i] != '/' && c2[i] != 0){
    return 0;
  }
  return 1;
}

void print_event_type(designADT d, int i){
  if (i >= d->nevents){
    printf("P");
  }
  else if (d->events[i]->type & OUT){
    printf("O");
  }
  else if (d->events[i]->type & IN){
    printf("I");
  }
  else if (d->events[i]->type & DUMMY){
    printf("I");
  }
  else{
    printf("U");
  }
}

void print_rules_matrix(designADT d){
    printf ("[ ");
    for (int i = 0; i < d->nevents+d->nplaces; i++){
      print_event_type(d,i);
    }
    printf ("]\n");
    for (int i = 0; i < d->nevents+d->nplaces; i++){
      printf ("[");
      print_event_type(d,i);
      for (int j = 0; j < d->nevents+d->nplaces; j++){
	if (d->rules[i][j]->ruletype == NORULE){
	  printf("-");
	}
	else if (d->rules[i][j]->epsilon == -1){
	  printf ("n");
	}
	else{
	  printf ("%i",d->rules[i][j]->epsilon);
	}
      }
      printf (" ]\n");
    }
}
int power(int num_times){
  if (num_times == 0){
    return 1;
  }
  else{
    return 2*power(num_times - 1);
  }
}

int convert_to_int(char* c, int size){
  int i;
  int p = 0;
  int r = 0;

  for (i = 0; i < size; i++){
    if (c[i] == '1'){
      r = r + power(p);
    }
    p++;
  }
  return r;
}




struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

//#ifdef OSX
  //hash_map<string, int> my_hash;
//#else
hash_map<char*, int/*, hash<char*>, eqstr*/> my_hash;
//#endif

char * get_state(designADT d){
  char *c = NULL;

  c = new char[d->nrules+1];
  for (int i = 0; i < d->nrules; i++){
    if (d->markkey[i]->epsilon == -1){
      c[i] = 'n';
    }
    else if (d->markkey[i]->epsilon == 1){
      c[i] = '1';
    }
    else if (d->markkey[i]->epsilon == 0){
      c[i] = '0';
    }
    else{
      printf ("Invalid markkey epsilon %i\n",d->markkey[i]->epsilon);
      exit(0);
    }
  }
  
  c[d->nrules] = 0;

  return c;
}

char * get_state(designADT d1, designADT d2){
  char * c;

  int i,r1,r2;
  r1 = d1->nrules;
  r2 = d2->nrules;
  
  c = new char[r1+r2+1];
  for (i = 0; i < r1; i++){
    if (d1->markkey[i]->epsilon == -1){
      c[i] = 'n';
    }
    else if (d1->markkey[i]->epsilon == 1){
      c[i] = '1';
    }
    else if (d1->markkey[i]->epsilon == 0){
      c[i] = '0';
    }
    else{
      printf ("Invalid d1 markkey epsilon %i\n",d1->markkey[i]->epsilon);
      exit(0);
    }
  }
  for (i = 0; i < r2; i++){
    if (d2->markkey[i]->epsilon == -1){
      c[i+r1] = 'n';
    }
    else if (d2->markkey[i]->epsilon == 1){
      c[i+r1] = '1';
    }
    else if (d2->markkey[i]->epsilon == 0){
      c[i+r1] = '0';
    }
    else{
      printf ("Invalid d2 markkey epsilon %i\n",d2->markkey[i]->epsilon);
      exit(0);
    }
  }
  
  c[r1+r2] = 0;

  return c;
}


void update_fired_transition(char * new_state, designADT d, int fired_signal){
  int i = fired_signal;
  int j;
  //erase the column in d, as the transition fired
  emit ("\tErasing column %i's transitions from 0 to %i\n",i,d->nevents);
  for (j = 0; j < d->nevents; j++){
    if (d->rules[j][i]->epsilon != -1){
      emit("\t\tErasing a column number [%i][%i]\n",j,i);
      new_state[d->rules[j][i]->marking] = '0';
    }
  }

  //erase the row of a place if it is now no longer marked
  emit("\tErasing its places\n");

  for (j = d->nevents; j < d->nevents+d->nplaces; j++){
    if (d->rules[j][i]->ruletype != 0){
      int k;
      //emit("\t\tErasing the marked place of %s\n",d->events[j]->event);
      for (k = 0; k < d->nevents; k++){
	if (d->rules[j][k]->ruletype != NORULE){	
	  if (d->rules[j][k]->epsilon != -1){
	    new_state[d->rules[j][k]->marking] = '0';
	    //emit("\t\t\tErasing [%i][%i]\n",j,k);
	  }
	  else{
	    printf("There is an invalid assumption 1, ruletype != NORULE but epsilon == -1\n");
	    printf("This occured at rules->[%i][%i]\n",j,k);
	    exit(0);
	  }
	}
      }
    }
  }
  //printf ("\tthe new one after erasing the column, %s\n",new_state);
  //fill the row to the places transition
  emit ("\tFilling the row of %i\n",i);

  for (j = 0; j < d->nevents; j++){
    if (d->rules[i][j]->ruletype != NORULE){
      if (d->rules[i][j]->epsilon != -1){
	//emit ("\t\tadding a marking to [%i][%i]\n",i,j);
	new_state[d->rules[i][j]->marking] = '1';
      }
      else{
	printf("There is an invalid assumption 2, ruletype !=NORULE but epsilon == -1\n");
	printf("This occured at rules->[%i][%i]\n",i,j);
	exit(0);
      }
    }
  }
  //if there is a place, then fill the place row
  //fill the row to the places transition
  emit ("\tFilling the places\n");
  for (j = d->nevents; j < d->nevents+d->nplaces; j++){
    if (d->rules[i][j]->ruletype != NORULE){
      int k;
      //emit ("\t\tadding a row of %i\n",j);
      for (k = 0; k < d->nevents; k++){
	if (d->rules[j][k]->ruletype != NORULE){
	  if (d->rules[j][k]->epsilon != -1){
	    //emit ("\t\t\tadding a marking to [%i][%i]\n",j,k);
	    new_state[d->rules[j][k]->marking] = '1';
	  }
	  else{
	    printf("There is an invalid assumption 3, ruletype !=NORULE but epsilon == -1\n");
	    printf("This occured at rules->[%i][%i]\n",j,k);
	    exit(0);
	  }
	}
      }
    }
  }
}


void initialize_state(char * state, designADT d1, designADT d2){
  int i;

  for (i = 0; i < d1->nrules; i++){
    if (state[i] == 'n'){
      d1->rules[d1->markkey[i]->enabling][d1->markkey[i]->enabled]->epsilon = -1;
      d1->markkey[i]->epsilon = -1;
    }
    else if (state[i] == '1'){
      d1->rules[d1->markkey[i]->enabling][d1->markkey[i]->enabled]->epsilon = 1;
      d1->markkey[i]->epsilon = 1;
    }
    else if (state[i] == '0'){
      d1->rules[d1->markkey[i]->enabling][d1->markkey[i]->enabled]->epsilon = 0;
      d1->markkey[i]->epsilon = 0;
    }
    else{
      printf ("Invalid state %i in current state '%s'\n",i,state);
      exit(0);
    }
  }
  for (i = 0; i < d2->nrules; i++){
    if (state[i+d1->nrules] == 'n'){
      d2->rules[d2->markkey[i]->enabling][d2->markkey[i]->enabled]->epsilon = -1;
      d2->markkey[i]->epsilon = -1;
    }
    else if (state[i+d1->nrules] == '1'){
      d2->rules[d2->markkey[i]->enabling][d2->markkey[i]->enabled]->epsilon = 1;
      d2->markkey[i]->epsilon = 1;
    }
    else if (state[i+d1->nrules] == '0'){
      d2->rules[d2->markkey[i]->enabling][d2->markkey[i]->enabled]->epsilon = 0;
      d2->markkey[i]->epsilon = 0;
    }
    else{
      printf ("Invalid state %i in current state '%s'\n",i,state);
      exit(0);
    }
  }
  return;
}

int fake_fire_transition(designADT d1, designADT d2, int signal, int put_d1_first, char * new_state){
  //don't fire input signals, just remove their markings if an output signal fires
  //fire output signals, but check that the input signal is marked
  //fire the other events normally

  char * d1_state;
  char * d2_state;
  int i;
  d1_state = get_state(d1);
  d2_state = get_state(d2);

  emit ("\t\tUpdating the state '%s'\n",d1_state);
  update_fired_transition(d1_state, d1, signal);
  emit ("\t\tAfter Updating     '%s'\n",d1_state);

  emit ("\tChecking for a match (before the /) with %s\n",d1->events[signal]->event);
  //now look through the second array to see if there is a transition with the same name
  //don't forget that i/1 and i/2 are the same.  As long as any i can fire it is ok.
  int found_a_match = 0;
  int num_good_matches = 0;
  //Look for any match in the other design that can fire
  for (i = 0; i < d2->nevents; i++){
    if (d2->events[i]->type & IN){
      if (names_match(d1->events[signal]->event,d2->events[i]->event)){
	found_a_match++;
	int good = 1;
	int not_all_invalid = 0;
	//check the column and make sure that this event can fire
	for (int j = 0; j < d2->nevents+d2->nplaces; j++){
	  if (d2->rules[j][i]->epsilon == 0){
	    emit("\tI found an input with the same name, but it should not fire\n");
	    emit("\t\tIt is Event '%s' rule [%i][%i] that is low.\n",d2->events[i]->event,j,i);
	    good = 0;
	    j = d2->nevents+d2->nplaces;
	  }
	  else if (d2->rules[j][i]->epsilon == 1){
	    not_all_invalid = 1;
	  }
	}
	emit ("\tI found a match %s in the other design\n",d2->events[i]->event);
	if (good == 1 and not_all_invalid == 1){
	  emit("\tI found an input with the same name that can fire\n");
	  num_good_matches++;
	  update_fired_transition(d2_state, d2, i);
	}
      }
      else{
	//emit("\t\tIt doesn't match '%s'\n",d2->events[i]->event);
      }
    }
  }

  if (num_good_matches>1){
    printf("I found %i matches that could fire, I was only expecting 1\n",num_good_matches);
    return FAIL;
  }

  if (found_a_match > 0 && num_good_matches == 0){
    //we found a match, but couldn't fire the transition
    printf("I found %i matches in the other design, but none could fire\n",num_good_matches);
    printf("The Design Doesn't Verify!\n");
    delete [] d1_state;
    delete [] d2_state;
    return OUT_WITHOUT_IN;
  }


  if (put_d1_first){
    strcpy(new_state,d1_state);
    strcat(new_state,d2_state);
  }
  else{
    strcpy(new_state,d2_state);
    strcat(new_state,d1_state);
  }

  //  printf ("\t\tThis gives state, %s, '%i'\n",new_state,convert_to_int(new_state,d1->nrules+d2->nrules));

  delete [] d1_state;
  delete [] d2_state;
  return SUCCESS;
}

void figure_out_all_possible_transitions(list<int> * d1_firings, list<int> * d2_firings, designADT d1, designADT d2){
  int i = 0;
  int j = 0;
  char * current_state_d1 = NULL;
  char * current_state_d2 = NULL;

  emit ("\tIn Figuring out all possible transitions\n");

  current_state_d1 = get_state(d1);
  //printf ("\tFiguring out d2\n");
  current_state_d2 = get_state(d2);
  //printf ("\tDone Figuring out d2\n");

  //for the output signals, check that the input signal is marked, if not then fire it.
  // - done in fake fire transition

  //don't fire input signals, just remove their markings if an output signal fires
  //fire output signals, but check that the input signal is marked
  //fire the other events normally - taken care of in fake_fire_transition

  emit ("\t\tChecking what can fire in d1\n");

  for (i = 0; i < d1->nevents; i++){
    //dummy are considered outputs
    if (d1->events[i]->type & OUT || d1->events[i]->type & DUMMY){
      int good = 0;
      //emit ("\t\t\tevent %s column %i is an %i\n",d1->events[i]->event,i,d1->events[i]->type);
      //make sure that there is a valid transition, but abort if there is an unmarked one
      for (j = 0; j < d1->nevents+d1->nplaces; j++){
	//emit ("\t\t\t\tChecking event '%s'\n", d1->events[j]->event);
	if (d1->rules[j][i]->epsilon == 0){
	  //emit("\t\t\t\tRule [%i][%i] or '%s' is a zero, so this event fails\n",j,i,d1->events[j]->event);
	  good = 0;
	  j = d1->nevents+d1->nplaces;
	}
	else if (d1->rules[j][i]->epsilon == 1){
	  //emit ("\t\t\t\tRow [%i][%i] is assumed as a 1 or '%i' for this column\n",j,i,d1->rules[j][i]->epsilon);
	  good++;
	}
      }
      if (good>0){
	//char * c = new char[d1->nrules+d2->nrules];
	//fake_fire_transition(d1, d2, i, 1, c);
	//my_list->push_front(c);
	//emit("\t\t\t\tIt can fire\n");
	d1_firings->push_front(i);
      }
    }
    else{
      //emit ("\t\t\tevent %s column %i is not an out or dummy with value %i\n",d1->events[i]->event,i,d1->events[i]->type & OUT);
    }
  }    

  //D2
  emit ("\t\tChecking what can fire in d2\n");
  for (i = 0; i < d2->nevents; i++){
    //Dummy is also considered an output
    if (d2->events[i]->type & OUT || d1->events[i]->type & DUMMY){
      int good = 0;
      //emit ("\t\t\tevent %s column %i is an %i\n",d2->events[i]->event,i,d2->events[i]->type);
      //make sure that there is a valid transition, but abort if there is an unmarked one
      for (j = 0; j < d2->nevents+d2->nplaces; j++){
	if (d2->rules[j][i]->epsilon == 0){
	  //emit("\t\t\t\tRule [%i][%i] or '%s' is a zero, so this event fails\n",j,i,d2->events[j]->event);
	  good = 0;
	  j = d2->nevents+d2->nplaces;
	}
	else if (d2->rules[j][i]->epsilon == 1){
	  //emit ("\t\t\t\tRow %i is 1 for this column\n",j);
	  good++;
	}
      }
      if (good>0){
	//char * c = new char[d1->nrules+d2->nrules];
	//fake_fire_transition(d2, d1, i,0, c);
	//my_list->push_front(c);
	//emit("\t\t\t\tIt can fire\n");
	d2_firings->push_front(i);
      }
    }  
    else{
      //emit ("\t\t\tevent %s column %i is not an out or dummy with value %i\n",d2->events[i]->event,i,d2->events[i]->type & OUT);
    }
  }
  emit ("\tExiting Figure out all possible transitions\n");
}

int fire_all_transitions(list<char*> * R, list<char*> * R_names, designADT d1, designADT d2,int optimize){
  while(R->empty() == 0){
    char * c = R->front();
    emit("Firing transition %s\n",R_names->front());

    int g;
    if (optimize){
      g = GenGlobalState(c,d1,d2);
    }
    else{
      g = GenGlobalState_unop(c,d1,d2);
    }

    if (g != 1){
      printf("\tFired: %s\n",R_names->front());
      return g;
    }

    R->pop_front();
    R_names->pop_front();
    delete [] c;
  }

  emit ("Exiting the recursion\n");
  return SUCCESS;
}

int fill_all_next_states(list<char*> * R, list<char*>* R_names,list<int> * d1_transitions, list<int> * d2_transitions, designADT d1, designADT d2){

  while(d1_transitions->empty() == 0){
    emit("\tQueing and checking event %s\n",d1->events[d1_transitions->front()]->event);
    char * c = new char[d1->nrules + d2->nrules+1];
    int g = fake_fire_transition(d1, d2, d1_transitions->front(),1, c);
    if (g != 1){
      printf("\tFired: %s\n",d1->events[d1_transitions->front()]->event);
      return g;
    }

    R->push_back(c);
    R_names->push_back(d1->events[d1_transitions->front()]->event);
    d1_transitions->pop_front();
  }
  while (d2_transitions->empty() == 0){
    emit("\tQueing and checking event %s\n",d2->events[d2_transitions->front()]->event);
    char * c = new char[d1->nrules+d2->nrules+1];
    int g = fake_fire_transition(d2, d1, d2_transitions->front(),0, c);
    if (g != 1){
      printf("\tFired: %s\n",d2->events[d2_transitions->front()]->event);
      return g;
    }

    R->push_back(c);
    R_names->push_back(d2->events[d2_transitions->front()]->event);
    d2_transitions->pop_front();
  }
  return SUCCESS;
}


int GenGlobalState_unop(char * state, designADT d1, designADT d2){
  list<char*> R;
  list<char*> R_names;
  list<char*> R_d1;
  list<char*> R_d2;
  list<char*> iter;
  list<int> d1_transitions;
  list<int> d2_transitions;
  char * hash_key;

  emit ("Starting GenGlobalState\n");

  hash_key = new char[d1->nrules+d2->nrules+1];
  if (hash_key == NULL){
    printf ("Unable to get more memory\n");
    return FAIL;
  }
  strcpy(hash_key,state);

  emit ("\tstate '%s' and\n\thash_ '%s'\n",state,hash_key);
  
  //if s is already visited then return, else store s in a hash table
  if (my_hash[hash_key] == 1){
    emit ("\tI already saw this state.  Not checking it\nExiting the recursion\n");
    delete [] hash_key;
    return SUCCESS;
  }
  my_hash[hash_key] = 1;
  num_states_visited++;

  initialize_state(state,d1,d2);

  if (nathan_debug>0){
    emit ("Printing the d1 rules\n");
    print_rules_matrix(d1);
    emit ("Printing the d2 rules\n");
    print_rules_matrix(d2);
  }


  figure_out_all_possible_transitions(&d1_transitions,&d2_transitions,d1,d2);

  if (nathan_debug>0){
    emit ("\tI found %i total new states for d1\n\tThey are:",d1_transitions.size());
    for (list<int>::iterator iter = d1_transitions.begin(); iter != d1_transitions.end(); iter++){
      emit(" %s",d1->events[*iter]->event);
    }
    emit ("\n\tI found %i total new states for d2\n\tThey are:",d2_transitions.size());
    for (list<int>::iterator iter = d2_transitions.begin(); iter != d2_transitions.end(); iter++){
      emit(" %s",d2->events[*iter]->event);
    }
    emit ("\n");
  }

  int fill = fill_all_next_states(&R,&R_names,&d1_transitions,&d2_transitions,d1,d2);
  if (fill != SUCCESS){
    return fill;
  }

  if (R.empty() == 1){
    printf("I reached a deadlock\n");
    printf("The Design Doesn't Verify!\n");
    return DEADLOCK;
  }

  return fire_all_transitions(&R,&R_names,d1,d2,0);
}


void get_hide_fail(list<int> * l, designADT d1, designADT d2, int event){
  for (int i = 0; i < d1->nevents; i++){
    if (d1->events[i]->type & IN && d1->rules[event][i]->ruletype != NORULE){
      emit("\t\t\tThe event %s outputs to an input in its design of %s\n",d1->events[event]->event,d1->events[i]->event);
      emit("\t\t\tChecking for a match in the other design\n");
      for (int j = 0; j < d2->nevents; j++){
	if (names_match(d1->events[i]->event,d2->events[j]->event)){
	  emit("\t\t\tFound a match of %s in the other deisgn\n",d2->events[j]->event);
	  l->push_back(j);
	}
      }
    }
  }
  return;
}

void get_conflict(list<int> * l,designADT d,list<int>* enabled, int event){
  for (int i = 0; i < d->nevents; i++){
    if (d->rules[event][i]->conflict & OCONFLICT){
      //It only conflicts with it if it is enabled.
      if (is_in(enabled,i)){
	l->push_back(i);
      }
    }
  }
  return;
}

//This assumes that the list is already sorted
void union_it(list<int>*l, int i){
  for (list<int>::iterator iter = l->begin(); iter != l->end(); iter++){
    if (*iter == i){
      return;
    }
    else if (*iter > i){
      l->insert(iter,i);
    }
  }
  l->push_back(i);
  return;
}


void do_pre_t_minus_u(designADT d1, designADT d2, list<int>* ge_1, list<int>*ge_2,list<int> * s1, list<int>* s2,
		      int t,list<int>* Wd_1, list<int>* Wd_2,list<int>* d1_return,list<int>*d2_return ){
  //For every member of the preset that is not marked,
  //  Look at it's presest and calculate the necessary set for each member.
  //adding in t into Wd
  int found_one = 0;
  list<int> new_Wd(*Wd_1);
  
  new_Wd.push_back(t);
  //look at everything in the preset of t that is not marked
  //Note:  If it is a transition, we calculate the necessary set for it
  //       If it is a place, we look at the transitions before it
  emit("\t\t\tChecking the pre_t_minus_u\n");
  for (int i = 0; i < d1->nevents; i++){
    if (d1->rules[i][t]->ruletype != NORULE && i != t){
      emit ("\t\t\t\t%s or %i is in the preset\n",d1->events[i]->event,i);
      if (!is_in(s1,t)){
	emit("\t\t\t\tIt is not in the global state, performing necessary on it\n");
	list<int>* r1 = new list<int>;
	list<int>* r2 = new list<int>;
	necessary(d1,d2,ge_1,ge_2,s1,s2,i,&new_Wd,Wd_2,r1,r2);
	if (found_one == 0){
	  emit("\t\t\t\tFound an initial pi %i+%i\n",r1->size(),r2->size());
	  found_one++;
	  *d1_return = *r1;
	  *d2_return = *r2;
	}
	/*TODO - perhaps remove this to get the min set again*/
	i = d1->nevents;
	/*For the min set
	if(r1->size()+r2->size() < d1_return->size()+d2_return->size() || found_one == 0){
	  emit("\t\t\t\tFound a smaller pi %i+%i, from %i+%i\n",r1->size(),r2->size(),d1_return->size(),d2_return->size());
	  found_one++;
	  *d1_return = *r1;
	  *d2_return = *r2;
	}
	//short circuit it out if
	if (d1_return->size()+d2_return->size() == 0){
	  emit ("\t\t\t\tShort Circuit\n");
	  i = d1->nevents;
	}
	*/
	delete r1;
	delete r2;
      }
    }
  }

  //If a place is marked, we don't have to figure out how to mark it
  if (found_one != 0 && d1_return->size()+d2_return->size() == 0){
    //short circuit it
    emit("\t\t\t\tAlready found the best solution, not checking places\n");
  }
  else{
    for (int i = d1->nevents; i < d1->nevents+d1->nplaces; i++){
      if (d1->rules[i][t]->ruletype != NORULE && i != t){
	emit ("\t\t\t\t%s or %i is in the preset\n",d1->events[i]->event,i);
	if (!is_in(s1,i)){
	  emit("\t\t\t\tIt is not in the global state, checking its presets\n");
	  list<int>* r1 = new list<int>;
	  list<int>* r2 = new list<int>;
	  //perform the union of all necessary sets in the preset of the preset
	  for (int j = 0; j < d1->nevents; j++){
	    if (d1->rules[j][i]->ruletype != NORULE && j != t){
	      list<int>* t1 = new list<int>;
	      list<int>* t2 = new list<int>;
	      emit("\t\t\t\t\t%s or %i is in the preset's preset, calculating its necessary\n",d1->events[j]->event,j);
	      necessary(d1,d2,ge_1,ge_2,s1,s2,j,&new_Wd,Wd_2,t1,t2);
	      merge(r1,t1);
	      merge(r2,t2);
	      delete t1;
	      delete t2;
	    }
	  }
	  if (found_one == 0){
	    emit("\t\t\t\tFound an initial pi %i+%i\n",r1->size(),r2->size());
	    found_one++;
	    *d1_return = *r1;
	    *d2_return = *r2;
	  }
	  /*todo, make this back to the min set*/
	  i = d1->nevents+d1->nplaces;
	  /*
	  if(r1->size()+r2->size() < d1_return->size()+d2_return->size() || found_one == 0){
	    emit("\t\t\t\tFound a smaller pi %i+%i, from %i+%i\n",r1->size(),r2->size(),d1_return->size(),d2_return->size());
	    found_one++;
	    *d1_return = *r1;
	    *d2_return = *r2;
	  }
	  //short circuit it out if
	  if (d1_return->size()+d2_return->size() == 0){
	    emit("\t\t\t\tShort circuit\n");
	    i = d1->nevents+d1->nplaces;
	  }
	  */
	  delete r1;
	  delete r2;
	}
	else{
	  emit("\t\t\t\tIt is enabled, I don't have to figure out how to enable it\n");
	}
      }
    }
  }
  emit("\t\t\tFinished the pre_t_minus_u\n");
  return;
}


void necessary(designADT d1, designADT d2, list<int>* ge_1, list<int>* ge_2,list<int> * s1, list<int>* s2,
	       int t,list<int>* Wd_1, list<int>* Wd_2,list<int>* d1_return,list<int>*d2_return ){
  emit ("\t\t\tEntering the necessary calculations for %s, event %i\n",d1->events[t]->event,t);
  if (nathan_debug>0){
    emit("\t\tWires in Wd_1 are:");
    for (list<int>::iterator iter = Wd_1->begin(); iter != Wd_1->end(); iter++){
      emit(" %i",*iter);
    }
    emit("\n\t\tWires in Wd_2 are:");
    for (list<int>::iterator iter = Wd_2->begin(); iter != Wd_2->end(); iter++){
      emit(" %i",*iter);
    }
    emit("\n");
  }
  if (is_in(Wd_1,t)){
    emit ("\t\t\t\tT is already in our Wd\n");
    emit("\t\t\tReturning from necessary\n");
    return;
  }
  else if (d1->events[t]->type & OUT || d1->events[t]->type & DUMMY){
    emit ("\t\t\t\tT is an output\n");
    if(is_in(ge_1,t)){
      emit ("\t\t\tT is globally enabled, returning it\n");
      d1_return->push_back(t);
      return;
    }
    else{
      emit ("\t\t\t\tT is not globally enabled\n");
      //For every member of the preset that is not marked,
      //  Look at it's presest and calculate the necessary set for each member.
      //adding in t into Wd
      do_pre_t_minus_u(d1,d2,ge_1,ge_2,s1,s2,t,Wd_1,Wd_2,d1_return,d2_return);
      
      emit("\t\t\tReturning from necessary\n");
      return;
    }
  }
  else if (d1->events[t]->type & IN){
    emit("\t\t\t\tIt is an input\n");
    if(is_in(ge_1,t)){
      emit("\t\t\t\tIt is globally enabled, putting in its cooresponding outputs that are enabled\n");
      for (int j = 0; j < d2->nevents; j++){
	if (names_match(d1->events[t]->event,d2->events[j]->event)){
	  if (is_in(ge_2,j)){
	    emit("\t\t\t\tPutting in event %s or %i in the other design\n",d2->events[j]->event,j);
	    d2_return->push_back(j);
	  }
	}
      }
      emit("\t\t\tReturning from necessary\n");
      return;
    }
    else{
      //For every member of the preset that is not marked, calculate the necessary set for it, adding in t into Wd
      //Also, check the primes in the other state for min.
      //look at everything in the preset of t that is not marked
      do_pre_t_minus_u(d1,d2,ge_1,ge_2,s1,s2,t,Wd_1,Wd_2,d1_return,d2_return);

      //short circuit it out if
      if (d1_return->size()+d2_return->size() == 0){
	emit ("\t\t\t\tNot calculating theta, as I have the best solution\n");
      }
      else{
	emit("\t\t\t\tNow calculating the theta to see if it is smaller, swaping d1 and d2 in the process\n");
	for (int i = 0; i < d2->nevents; i++){
	  if (names_match(d1->events[t]->event,d2->events[i]->event) && i != t){
	    //perform the union of all necessary sets that are outputs with the same name
	    emit("\t\t\t\t%s or %i has the same name, calling necessary on it\n",d2->events[i]->event,i);
	    list<int>* r1 = new list<int>;
	    list<int>* r2 = new list<int>;
	    necessary(d2,d1,ge_2,ge_1,s2,s1,i,Wd_2,Wd_1,r2,r1);
	    if(r1->size()+r2->size() < d1_return->size()+d2_return->size()){
	      emit("\t\t\t\tFound a smaller theta %i+%i, from %i+%i\n",r1->size(),r2->size(),d1_return->size(),d2_return->size());
	      *d1_return = *r1;
	      *d2_return = *r2;
	    }
	    //short circuit it out if
	    if (d1_return->size()+d2_return->size() == 0){
	      emit("\t\t\t\tShort Circuit\n");
	      i = d2->nevents;
	    }
	    delete r1;
	    delete r2;
	  }
	}
      }
      emit("\t\t\tReturning from necessary\n");
      return;
    }
  }
  else{
    printf ("I found a bogus ruletype in necessary, %i\n",d1->events[t]->type);
    exit(0);
  }
}

void get_globally_enabled(designADT d1, designADT d2, list<int>*d1_e,list<int>*d2_e){
  for (int i = 0; i < d1->nevents; i++){
    int good = 0;
    //make sure that there is a valid transition, but abort if there is an unmarked one
    for (int j = 0; j < d1->nevents+d1->nplaces; j++){
      if (d1->rules[j][i]->epsilon == 0){
	good = 0;
	j = d1->nevents+d1->nplaces;
      }
      else if (d1->rules[j][i]->epsilon == 1){
	good++;
      }
    }
    if (good>0){
      d1_e->push_front(i);
    }
  }
  
  //D2
  for (int i = 0; i < d2->nevents; i++){
    int good = 0;
    for (int j = 0; j < d2->nevents+d2->nplaces; j++){
      if (d2->rules[j][i]->epsilon == 0){
	good = 0;
	j = d2->nevents+d2->nplaces;
      }
      else if (d2->rules[j][i]->epsilon == 1){
	good++;
      }
    }
    if (good>0){
      d2_e->push_front(i);
    }
  }  
  return;
}


void get_dep_set(designADT d1, designADT d2, list<int>*d1_prime,list<int>*d2_prime,list<int>*d1_g,list<int>*d2_g,int event){
  list<int> conflict;
  list<int> hide_fail;

  //fill up the global state
  list<int> s1;
  list<int> s2;
  get_global_state(d1,d2,&s1,&s2);

  emit ("\t\tGetting the dependent set\n");
  union_it(d1_prime,event);

  get_conflict(&conflict,d1,d1_g,event);
  emit("\t\tThere are %i things that conflict with event %i, name %s\n",conflict.size(),event,d1->events[event]->event);
  for (list<int>::iterator iter = conflict.begin(); iter != conflict.end(); iter++){
    if (is_in(d1_g,*iter)){
      if (!is_in(d1_prime,*iter)){
	emit ("\t\trecursing (get_dep_set)on a conflict\n");
	get_dep_set(d1,d2,d1_prime,d2_prime,d1_g,d2_g,*iter);
      }
    }
    else{
      list<int> empty1;
      list<int> empty2;
      list<int> r1;
      list<int> r2;
      list<int> d1_ge_out_and_in;
      list<int> d2_ge_out_and_in;
      //to fill up the ge set for necessary
      get_globally_enabled(d1,d2,&d1_ge_out_and_in,&d2_ge_out_and_in);
      emit ("\t\tCalculating the necessary set on a conflict\n");
      necessary(d1,d2,&d1_ge_out_and_in,&d2_ge_out_and_in,&s1,&s2,*iter,&empty1,&empty2,&r1,&r2);
      merge(d1_prime,&r1);
      merge(d2_prime,&r2);
    }
  }

  get_hide_fail(&hide_fail,d1,d2,event);
  emit("\t\tThere are %i things that hide_fail with event %i\n",hide_fail.size(),event);
  for (list<int>::iterator iter = hide_fail.begin(); iter != hide_fail.end(); iter++){
    if (is_in(d2_g,*iter)){
      if (!is_in(d2_prime,*iter)){
	//hide fail is in the other design
	emit ("\t\trecursing and switching d1 and d2 (get_dep_set) on a hide_fail\n");
	get_dep_set(d2,d1,d2_prime,d1_prime,d2_g,d1_g,*iter);
      }
    }
    else{
      list<int> empty1;
      list<int> empty2;
      list<int> r1;
      list<int> r2;
      list<int> d1_ge_out_and_in;
      list<int> d2_ge_out_and_in;
      //set the globally enabled events
      get_globally_enabled(d1,d2,&d1_ge_out_and_in,&d2_ge_out_and_in);
      //We cannot let necessary change the event wire, as we are trying to change it
      //todo:  is this true?
      //empty1.push_back(event);
      emit ("\t\tCalculating the necessary set on a hide_fail, switching d1 and d2\n");
      necessary(d2,d1,&d2_ge_out_and_in,&d1_ge_out_and_in,&s2,&s1,*iter,&empty2,&empty1,&r2,&r1);
      merge(d1_prime,&r1);
      merge(d2_prime,&r2);
    }
  }

  return;
}

int compute_best_ready(designADT d1, designADT d2, list<char*> *R, list<char*> *R_names,list<int>*d1_global, list<int>*d2_global){
  list<int> R_d1_transitions;
  list<int> R_d2_transitions;

  //R <- the set of globally enabled output transitions at s;
  //for every globally enabled output transition t at s do
  //    compute the ready set R' obtained from t
  //    if |R'| < |R| then R = R'
  //end do
  R_d1_transitions = *d1_global;
  R_d2_transitions = *d2_global;
  emit ("\tComputing best ready set, starting with %i+%i in the set\n",R_d1_transitions.size(),R_d2_transitions.size());
  for(list<int>::iterator iter = d1_global->begin(); iter != d1_global->end(); iter++){
    list<int> R_d1_prime;
    list<int> R_d2_prime;

    emit("\tFinding the ready set for event %s in d1\n",d1->events[d1_global->front()]->event);
    get_dep_set(d1,d2,&R_d1_prime,&R_d2_prime,d1_global,d2_global,*iter);

    if (R_d1_prime.size() + R_d2_prime.size() < R_d1_transitions.size() + R_d2_transitions.size()){
      emit ("\tThere is a new ready set winner of size %i+%i, over size %i+%i\n",R_d1_prime.size(), R_d2_prime.size(), R_d1_transitions.size(), R_d2_transitions.size());
      R_d1_transitions = R_d1_prime;
      R_d2_transitions = R_d2_prime;
    }
  }
  for(list<int>::iterator iter = d2_global->begin(); iter != d2_global->end(); iter++){
    list<int> R_d1_prime;
    list<int> R_d2_prime;

    emit("\tFinding the ready set for event %s in d2\n",d2->events[d2_global->front()]->event);
    get_dep_set(d2,d1,&R_d2_prime,&R_d1_prime,d2_global,d1_global,*iter);

    if (R_d1_prime.size() + R_d2_prime.size() < R_d1_transitions.size() + R_d2_transitions.size()){
      emit ("\tThere is a new ready set winner of size %i+%i, over size %i+%i\n",R_d1_prime.size(), R_d2_prime.size(), R_d1_transitions.size(), R_d2_transitions.size());
      R_d1_transitions = R_d1_prime;
      R_d2_transitions = R_d2_prime;
    }
  }

  if (nathan_debug>0){
    emit ("\tAfter partial ordering, %i total new states for d1\n\tThey are:",R_d1_transitions.size());
    for (list<int>::iterator iter = R_d1_transitions.begin(); iter != R_d1_transitions.end(); iter++){
      emit(" %s",d1->events[*iter]->event);
    }
    emit ("\n\tAfter partial ordering %i total new states for d2\n\tThey are:",R_d2_transitions.size());
    for (list<int>::iterator iter = R_d2_transitions.begin(); iter != R_d2_transitions.end(); iter++){
      emit(" %s",d2->events[*iter]->event);
    }
    emit ("\n");
  }



  return fill_all_next_states(R,R_names,&R_d1_transitions,&R_d2_transitions,d1,d2);
}

int GenGlobalState(char * state, designADT d1, designADT d2){
  list<char*> R;
  list<char*> R_names;
  list<char*> iter;
  list<int> d1_firing_transitions;
  list<int> d2_firing_transitions;
  //char * c;
  char * hash_key;
  //char * current_state;
  //char * current_state_d1;
  //char * current_state_d2;
  //hash_map<const char*, int, hash<const char*>, eqstr>::iterator it;

  emit ("Starting GenGlobalState\n");

  hash_key = new char[d1->nrules+d2->nrules+1];
  if (hash_key == NULL){
    printf ("Unable to get more memory\n");
    return FAIL;
  }
  strcpy(hash_key,state);

  emit ("\tstate '%s' and\n\thash_ '%s'\n",state,hash_key);
  
  //if s is already visited then return, else store s in a hash table
  if (my_hash[hash_key] == 1){
    emit ("\tI already saw this state.  Not checking it\nExiting the recursion\n");
    delete [] hash_key;
    return SUCCESS;
  }
  my_hash[hash_key] = 1;
  num_states_visited++;

  initialize_state(state,d1,d2);

  if (nathan_debug>0){
    emit ("Printing the d1 rules\n");
    print_rules_matrix(d1);
    emit ("Printing the d2 rules\n");
    print_rules_matrix(d2);
  }


  // if SafetyFailure(s) or liveness Failure(s) then ReportFailure & exit(s);
  figure_out_all_possible_transitions(&d1_firing_transitions,&d2_firing_transitions,d1,d2);

  if (nathan_debug>0){
    emit ("\tI found %i total new states for d1\n\tThey are:",d1_firing_transitions.size());
    for (list<int>::iterator iter = d1_firing_transitions.begin(); iter != d1_firing_transitions.end(); iter++){
      emit(" %s",d1->events[*iter]->event);
    }
    emit ("\n\tI found %i total new states for d2\n\tThey are:",d2_firing_transitions.size());
    for (list<int>::iterator iter = d2_firing_transitions.begin(); iter != d2_firing_transitions.end(); iter++){
      emit(" %s",d2->events[*iter]->event);
    }
    emit ("\n");
  }

  if (d1_firing_transitions.empty() == 1 && d2_firing_transitions.empty() == 1){
    printf("I reached a deadlock\n");
    printf("The Design Doesn't Verify!\n");
    return DEADLOCK;
  }

  //R <- the set of globally enabled output transitions at s;
  //for every globally enabled output transition t at s do
  //    compute the ready set R' obtained from t
  //    if |R'| < |R| then R = R'
  //end do
  int fill = compute_best_ready(d1,d2,&R,&R_names,&d1_firing_transitions,&d2_firing_transitions);
  if (fill != SUCCESS){
    return fill;
  }



  //for every t in R do
  //    s' <- the successor obtained by firing t at s;
  //    GenGlobalState(s')
  //end do
  //end function;

  //This is a second check for sanity
  if (R.empty() == 1){
    printf("I reached a deadlock\n");
    printf("The Design Doesn't Verify!\n");
    return DEADLOCK;
  }
  return fire_all_transitions(&R,&R_names,d1,d2,1);
}

bool partial_order(char *filename, bool verbose, bool dont_optimize)
{
  designADT designSPEC;
  designADT designIMPL;
  bool result=false;

  designSPEC=initialize_design();
  designIMPL=initialize_design();

  strcpy(designSPEC->filename,filename);
  strcpy(designIMPL->filename,filename);
  strcat(designIMPL->filename,"IMPL");

  designSPEC->postproc=false;
  designIMPL->postproc=false;
  if (load_lpn(designSPEC)) {
    if (load_lpn(designIMPL)) {
      int i;
      char * c;

      if (verbose){
	nathan_debug = 1;
      }

      if (nathan_debug>0){
	emit("The filenames are: %s, and %s\n",designSPEC->filename,designIMPL->filename);

	emit("\nInput Signals for SPEC:");
	for (i = 0; i < designSPEC->ninpsig; i++){
	  emit (" %s",designSPEC->signals[i]->name);
	}
	emit("\nOutput Signals for SPEC:");
	for (; i < designSPEC->nsignals; i++){
	  emit (" %s",designSPEC->signals[i]->name);
	}
	emit ("\nEvents for SPEC:");
	for (i = 0; i < designSPEC->nevents; i++){
	  emit(" %s",designSPEC->events[i]->event);
	}
	
	emit ("\nSPEC nevents = %i, ninputs = %i, ninpsig = %i, nsignals = %i, nplaces = %i, nrules = %i\n",designSPEC->nevents,designSPEC->ninputs,designSPEC->ninpsig,designSPEC->nsignals,designSPEC->nplaces,designSPEC->nrules);

	emit ("Printing the designSPEC rules\n");
	print_rules_matrix(designSPEC);

	emit("\nInput Signals for IMPL:");
	for (i = 0; i < designIMPL->ninpsig; i++){
	  emit (" %s",designIMPL->signals[i]->name);
	}
	emit("\nOutput Signals for IMPL:");
	for (; i < designIMPL->nsignals; i++){
	  emit (" %s",designIMPL->signals[i]->name);
	}
	emit ("\nEvents for IMPL:");
	for (i = 0; i < designIMPL->nevents; i++){
	  emit(" %s",designIMPL->events[i]->event);
	}
	emit ("\n");
	
	emit ("IMPL nevents = %i, ninputs = %i, ninpsig = %i, nsignals = %i, nplaces = %i, nrules = %i\n",designIMPL->nevents,designIMPL->ninputs,designIMPL->ninpsig,designIMPL->nsignals,designIMPL->nplaces,designIMPL->nrules);

	emit ("Printing the designIMPL rules\n");
	print_rules_matrix(designIMPL);
      }
      /*
      printf ("\nMy markkey is %s\n",c);
      printf ("Total bits are %i\n",designSPEC->nrules + designIMPL->nrules);

      
      printf ("The markkey is as follows\n");
      for (i = 0; i < designSPEC->nrules; i++){
	printf ("\t[%i,%i],%i\n",designSPEC->markkey[i]->enabling,designSPEC->markkey[i]->enabled,designSPEC->markkey[i]->epsilon);
      }
      */

      emit ("Getting the initial state\n");
      c = get_state(designSPEC,designIMPL);
      emit ("Initial State %s\n",c);

      int g;
      if (!dont_optimize){
	emit("Running with full optimizations\n");
	g = GenGlobalState(c,designSPEC,designIMPL);
      }
      else{
	  emit("Running without optimizations\n");
	g = GenGlobalState_unop(c,designSPEC,designIMPL);
      }
      if (g == SUCCESS){
	printf ("The design Verifies\n");
      }
      else if (g == FAIL){
	printf("The design did not verify because of a failure\n");
      }
      else if(g == OUT_WITHOUT_IN){
	printf ("The design did not verify because an output fired when the input was not ready\n");
      }
      else if(g == DEADLOCK){
	printf ("The design did not verify because of deadlock\n");
      }
      else{
	printf("Bad return code from GenGlobalState\n");
      }
      printf ("There were a total of %i unique states visited\n", num_states_visited);

      delete [] c;
      emit( "Ending the function\n\n\n");
      result=true;
    }
  }



  delete_design(designSPEC);
  delete_design(designIMPL);

  return result;
}

