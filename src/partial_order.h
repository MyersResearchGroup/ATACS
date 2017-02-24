#ifndef _partial_order_h
#define _partial_order_h

#include <string.h>
#include "struct.h"
#include "satconf.h"
#include "memaloc.h"
#include "def.h"
#include "loadg.h"
#include "ly.h" //for OUT and IN and DUMMY

#include <list>
#include <ext/hash_map>

bool partial_order(char *filename, bool verbose, bool dont_optimize);

void do_pre_t_minus_u(designADT d1, designADT d2, list<int>* ge_1, list<int>*ge_2,list<int> * s1, list<int>* s2,
		      int t,list<int>* Wd_1, list<int>* Wd_2,list<int>* d1_return,list<int>*d2_return );

void necessary(designADT d1, designADT d2, list<int>* ge_1, list<int>* ge_2,list<int> * s1, list<int>* s2,
	       int t,list<int>* Wd_1, list<int>* Wd_2,list<int>* d1_return,list<int>*d2_return );

void print_event_type(designADT d, int i);

void print_rules_matrix(designADT d);

char * get_state(designADT d);

char * get_state(designADT d1, designADT d2);

void update_fired_transition(char * new_state, designADT d, int fired_signal);

void initialize_state(char * state, designADT d1, designADT d2);

int fake_fire_transition(designADT d1, designADT d2, int signal, int put_d1_first, char * new_state);

void figure_out_all_possible_transitions(list<int> * d1_firings, list<int> * d2_firings, designADT d1, designADT d2);

int fire_all_transitions(list<char*> * R, list<char*> * R_names, designADT d1, designADT d2,int optimize);

int fill_all_next_states(list<char*> * R, list<char*>* R_names,list<int> * d1_transitions, list<int> * d2_transitions, designADT d1, designADT d2);

int GenGlobalState_unop(char * state, designADT d1, designADT d2);

int GenGlobalState(char * state, designADT d1, designADT d2);

#endif
