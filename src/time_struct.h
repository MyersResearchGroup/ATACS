#include "orbitsrsg.h"
#include <sys/time.h>
#include <unistd.h>

#define TIME_STRUCT
typedef struct firinglist{        /* list of rules timed enabled to fire */
  int enabling;
  int enabled; 
  struct firinglist *next;
} *firinglistADT;

/* The starting point for work, a marking and list of unfired timed   */
/* enabled events.                                                    */  

typedef struct work_struct {
/* Marking and state (BDDizable) */
  markingADT marking;
  char * state;
/* Could be a bit vector on rules */
  firinglistADT firinglist;
/* Lots of memory usage here, bitvectors?? */
  rule_info_listADT *rule_info;
/* Only used for sets */
  int num_sets;
  constraintADT *constraints;
/* Constraint matrix (BDDizable) */
  int clock_size;
  int num_clocks;
  clocksADT clocks;
  clockkeyADT clockkey;
/* Process matrix (BDDizable) */
  int process_size;
  int num_events;
  processADT process;
  processkeyADT processkey;
} *workADT;

typedef struct approx_work_struct {
  markingADT marking;
  firinglistADT firinglist;
  exp_enabledADT exp_enabled;
  confl_removedADT confl_removed;
  rule_contextADT context;
  clocknumADT clocknums;
  firedADT fired;
  int clock_size;
  int num_clocks;
  clocksADT clocks;
  clockkeyADT clockkey; 
  int process_size;
  int num_events;
  processADT process;
  processkeyADT processkey; 
} *approx_workADT;


typedef struct geom_work_struct{
  markingADT marking;
  firinglistADT firinglist;
  exp_enabledADT exp_enabled;
  confl_removedADT confl_removed;
  clocknumADT clocknums;
  firedADT fired;
  int clock_size;
  int num_clocks;
  clocksADT clocks;
  clockkeyADT clockkey; 
} *geom_workADT;


typedef struct verwork_struct {
  markingADT marking;
  markingADT imp_marking;
  firinglistADT firinglist;
  firinglistADT imp_firinglist;
  clocksADT clocks;
  clocksADT imp_clocks;
  clockkeyADT clockkey;
  clockkeyADT imp_clockkey;
  constraintADT *constraints;
  rule_info_listADT *rule_info;
  rule_info_listADT *imp_rule_info;
  int clock_size;
  int imp_clock_size;
  int process_size;
  int num_clocks;
  int imp_num_clocks;
  int num_events;
  int num_sets;
  processADT process;
  processkeyADT processkey;
} *verworkADT;

/* Stack of all the pending work. */

typedef struct workstack{
  workADT work;
  struct workstack *next;
} *workstackADT;

typedef struct approx_workstack{
  approx_workADT work;
  struct approx_workstack *next;
} *approx_workstackADT;

typedef struct geom_workstack{
  geom_workADT work;
  struct geom_workstack *next;
} *geom_workstackADT;

/* Stack of all the pending work for verification */

typedef struct verstack{
  verworkADT work;
  struct verstack *next;
} *verstackADT;
