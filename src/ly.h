/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 2001 by, Chris J. Myers                                   */
/*   University of Utah                                                      */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of University of Utah may not be used in advertising or    */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of University of Utah.               */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE */
/*   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/*   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA */
/*   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
#ifndef _ly_h
#define _ly_h

#include <list>
#include <map>
#include "misclib.h"
#include <strings.h>
#include <string>

using namespace std;

#define   INFIN     2147483647
#define   FILENAMESIZE 256

#define UNKNOWN     0
//The following must be powers of 2 and distinct from eachother, so they are
//independent flags that can be manipulated with bitwise operations...
#define IN             1
#define OUT            2
#define PASSIVE        4
#define ACTIVE         8
#define DUMMY         16
#define IDLE          32
#define WORK          64
#define DONE         128
#define ISIGNAL      256
#define RELEVANT     512
#define SENT        1024
#define RECEIVED    2048
#define PUSH        4096
#define PULL        8192
#define KEEP       16384
#define NONINP     32768
#define ABSTRAC    65536
#define CONT      131072
#define VAR       262144
#define FAILTRAN  524288
#define NONDISABLE 1048576
//...end of flags.

const int entities = 50;

typedef struct delat_tag {
  int lr;
  int ur;
  char *distr;
  int lf;
  int uf;
  char *distf;
} delayADT;


struct initial_stateADT 
{
  initial_stateADT() 
    { 
      initial_type_known = true; 
      initial_value_known = true; 
      initial_delay_known = true; 
    }
  initial_stateADT(bool b1, bool b2, bool b3)
    {
      initial_type_known = b1; 
      initial_value_known = b2; 
      initial_delay_known = b3; 
    }
  bool initial_type_known;
  bool initial_value_known;
  bool initial_delay_known;
};

typedef struct assign_tag {
  char *var;
  char *assign;
  struct assign_tag* next;
} *assignList;

typedef struct action_tag {
  int type;
  char * label;
  bool initial;
  int lower;
  int upper;
  char *dist;
  bool vacuous;
  int odata;
  int idata;
  int intval;
  //initial values
  int linitval;
  int uinitval;
  int linitrate;
  int uinitrate;
  // ZHEN(Done): add linked list of assignments <lhs : string, rhs : string>
  assignList list_assigns;
  delayADT delay;
  double real_def;
  int maxoccur; /* TEMP */
  struct action_tag *inverse; /* TEMP */
  initial_stateADT initial_state;
} *actionADT;


typedef struct actionset_tag{
  actionADT action;
  struct actionset_tag *link;
} *actionsetADT;



typedef struct event_tag {
  actionADT action;
  int occurrence;
  bool fixed_occur;
  int initial; 
  int lower;
  int upper;
  char *dist;
  char *exp;
  char *assign;
  string data;  //  The variable participating in a send or receive operation
} *eventsADT;

#ifdef STL_STUFF
typedef list<eventsADT> *eventsetADT;
#else
typedef struct eventset_tag {
  eventsADT event;
  struct eventset_tag *link;
} *eventsetADT;
#endif

typedef struct rule_tag {
  eventsADT enabling;
  eventsADT enabled;
  char* exp;
  char* assign;
  int lower;
  int upper;
  char *dist;
  int type;
  bool regular;
} *rulesADT;


#ifdef STL_STUFF
typedef list<rulesADT> *rulesetADT;
#else
typedef struct  ruleset_tag {
  rulesADT rule;
  struct ruleset_tag *link;
} *rulesetADT;
#endif

typedef struct conflict_tag {
  eventsADT left;
  eventsADT right;
  int conftype;
} *conflictsADT;


#ifdef STL_STUFF
typedef list<conflictsADT> *conflictsetADT;
#else
typedef struct conflictset_tag {
  conflictsADT conflict;
  struct conflictset_tag *link;
} *conflictsetADT;
#endif


typedef struct terstruct_tag {
  actionsetADT A;
  eventsetADT I;
  eventsetADT O;
  eventsetADT first;
  eventsetADT last;
  eventsetADT loop;
  eventsetADT CT;
  eventsetADT CP;
  rulesetADT R;
  rulesetADT Rp;
  rulesetADT DPCT;
  rulesetADT CPCT;
  rulesetADT DTCP;
  rulesetADT CTCP;
  rulesetADT CPDT;
  conflictsetADT C;
  conflictsetADT Cp;
  char *exp;
} *TERstructADT;

typedef TERstructADT telADT;

actionADT enter(char * action,int len); 
void inithashtable(void);

int yyerror(char * s);

#include <string>

typedef pair<string, TERstructADT> str_tel;
#ifdef __GCC_OLD
typedef map<string, string> m_str_str;
else
typedef map<std::string, std::string> m_str_str;
#endif


actionADT operator++(const string& name);
actionADT operator--(const string& name);
#include "misclib.h"

//#define free nop

#endif /* ly.h */
