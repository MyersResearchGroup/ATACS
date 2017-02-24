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

#include "struct.h"
#include "hpnsim.h"
#include "memaloc.h"
#include "def.h"
#include "printlpn.h"
#include "lpntrsfm.h"
#include <float.h>
#include <signal.h>
#include <sys/time.h>

// Defined as static because I want their scope to be within this file
// only.
// MAX_STEP is the amount of time that the simulator will step
// if tau_min is 0 and tau_max is infinity.
static const double MAX_STEP = 1.00;
// MIN_STEP is the minimum amount of time that the simulator is
// allowed to step.  This is to prevent situations where we
// are converging to zero but not reaching zero until
// there is sufficient roundoff error.
static const double MIN_STEP = .1;

bool con_place(designADT design, int p) {
  return (design->nevents <= p && design->events[p]->type & CONT);
}

bool dis_place(designADT design, int p) {
  return (design->nevents <= p && !(design->events[p]->type & CONT));
}

bool con_trans(designADT design, int t) {
  return (design->nevents > t && design->events[t]->type & CONT);
}

bool dis_trans(designADT design, int t) {
  return (design->nevents > t && !(design->events[t]->type & CONT));
}


void hpn_simulate(designADT design) {
  if (!(design->status & LOADED)) {
    printf("No Design Loaded.\n");
    return;
  }

  hpnStateADT state = NULL;
  FILE* simlog = NULL;
  print_size(design);
  
  start_sim(design, state, simlog);

  if (design->manual) {
    interactive(design, state, simlog);
  }
  else {
    run_hpn_sim(design, state, simlog, design->simtime, INFIN, INFIN);

  }
  stop_sim(design, state, simlog);
}

void start_sim(designADT design, hpnStateADT &state, FILE* &simlog) {
  if (!design->manual) {
    timeval t;
    gettimeofday(&t, NULL);
    srand((unsigned) t.tv_usec);
  }
  state = initialize_state(design);
  printf("Beginning HPN Simulation.\n");
  simlog = fopen("hpnsim.log", "w");
  
  print_headers(design, state, simlog);
  print_state(design, state, simlog);
}

void stop_sim(designADT design, hpnStateADT state, FILE* simlog) {
  printf("HPN Simulation Complete.\n");
  fclose(simlog);

  if (!design->manual) {
    print_all_state(design, state);
    //view_net(design, state);
  }
  
// Reclaim any memory used by the simulator.
  destroy(state);
  if (!design->manual) {
    system("gnuplot tmpplot");
    system("gv tmp.eps");
  }
}

void print_help() {
  printf("restar(t)  - resets the simulation to the initial conditions.\n");
  printf("(h)elp     - prints this message\n");
  printf("see(d) [x] - seeds the random number generator with x or the\n");
  printf("             current clock value if x is not provided.\n");
  printf("(r)un [x]  - simulate for x time units. If no x is provided,\n");
  printf("             x is assumed to be 100.\n");
  printf("(s)tep [x] - simulate for x steps. A step is a time advancement \n");
  printf("             or transition firing. If x is not provided, \n");
  printf("             1 is used.\n");
  printf("(e)step [x]- Step x transition firings. x defaults to one if\n");
  printf("             not provided\n");
  printf("(v)iew     - view the current net\n");
  printf("(p)rint    - prints all current state information\n");
  printf("p(l)ot     - view the current plot of the simulation\n");
  printf("(q)uit     - terminate simulation and return to main atacs\n");
  printf("             prompt.\n");
}

void interactive(designADT design, hpnStateADT state, FILE* sl) {
  const int MAX_LINE = 80;
  char input[MAX_LINE] = "";
  char prev[MAX_LINE] = "\0";
  char* tmp = NULL;
  while (true) {
    strcpy(prev, input);
    printf("(hpnsim) atacs> ");
    if (fgets(input, MAX_LINE, stdin) == NULL) {
      continue;
    }
    // use previuos command if no new command is entered.
    if (strcmp(input, "\n") == 0) {
      printf("(hpnsim) atacs> %s\n", prev);
      strcpy(input, prev);
    }
    tmp = strtok(input, " \n\t");
    if (tmp == NULL) {
      continue;
    }
    // restart the simulation from zero.
    else if (strcmp(tmp, "restart") == 0 || strcmp(tmp, "t") == 0) {
      stop_sim(design, state, sl);
      start_sim(design, state, sl);
    }
    // print the help.
    else if (strcmp(tmp, "help") == 0 || strcmp(tmp, "h") == 0) {
      print_help();
    }
    // Makes x simulation steps. x is 1 if not specified.
    else if (strcmp(tmp, "step") == 0 || strcmp(tmp, "s") == 0) {
      tmp = strtok(NULL, " \n\t");
      int steps = 1;
      if (tmp != NULL) {
        steps = atoi(tmp);
      }
      run_hpn_sim(design, state, sl, DBL_MAX, steps, INFIN);
    }
    else if (strcmp(tmp, "stepe") == 0 || strcmp(tmp, "e") == 0) {
      tmp = strtok(NULL, " \n\t");
      int steps = 1;
      if (tmp != NULL) {
        steps = atoi(tmp);
      }
      run_hpn_sim(design, state, sl, DBL_MAX, INFIN, steps);
    }
    // Simulate x time units
    else if (strcmp(tmp, "run") == 0 || strcmp(tmp, "r") == 0) {
      tmp = strtok(NULL, " \n\t");
      double tau = 100;
      if (tmp != NULL) {
        tau = strtod(tmp, NULL);
      }
      run_hpn_sim(design, state, sl, tau, INFIN, INFIN);
    }
    // view the current net.
    else if (strcmp(tmp, "view") == 0 || strcmp(tmp, "v") == 0) {
      view_net(design, state);
    }
    // seeds the random number generator with the clock
    else if (strcmp(tmp, "seed") == 0 || strcmp(tmp, "d") == 0) {
      tmp = strtok(NULL, " \n\t");
      timeval t;
      gettimeofday(&t, NULL);
      unsigned int seed = (unsigned) t.tv_usec;
      if (tmp != NULL) {
        seed = atoi(tmp);
      }
      srand(seed);
    }
    // print all the current state information.
    else if (strcmp(tmp, "print") == 0 || strcmp(tmp, "p") == 0) {
      print_all_state(design, state);
    }
    // view the current plot of the simulation.
    else if (strcmp(tmp, "plot") == 0 || strcmp(tmp, "l") == 0) {
      fflush(sl);
      system("gnuplot tmpplot");
      system("gv tmp.eps &");
    }
    else if (strcmp(tmp, "quit") == 0 || strcmp(tmp, "q") == 0) {
      return;
    }
    else {
      printf("unknown command\n");
    }
  }
}


bool hpn_go = false;

void handle_break(int i) {
  hpn_go = false;
}

void run_hpn_sim(designADT design, hpnStateADT state, FILE* sl,
                 double maxtime, int steps, int esteps) {
  signal(SIGINT, handle_break);
  int count = 0;
  int ecount = 0;
  hpn_go = true;
  if (maxtime != DBL_MAX)
    maxtime = maxtime+state->tau;
  while (hpn_go && state->tau < maxtime && count < steps && ecount < esteps) {
    bool fired = false;
    // If a transition must fire, fire it.
    int offset = (int)(((double)design->nevents*rand())/RAND_MAX);
    for (int t = 0; t < design->nevents; t++) {
      // want to fire a random T.
      int tIndex = (t+offset) % design->nevents;
      if (tIndex != 0 && must_fire(design, state, tIndex)) {
        fprintf(stdout, "# %f: Must fire discrete transition %s.\n",
                state->tau, design->events[tIndex]->event);
        fire_T(design, state, tIndex);
        fired = true;
        ecount++;
	break;
      }
    }

    // Decide if I want to fire a discrete transition or tau time.
    if (!fired && rand() > RAND_MAX/2) {
      int offset = (int)(((double)design->nevents*rand())/RAND_MAX);
      for (int t = 0; t < design->nevents; t++) {
        // want to fire a random T.
        int tIndex = (t+offset) % design->nevents;
        if (tIndex != 0 && can_fire(design, state, tIndex)) {
          fprintf(stdout, "# %f: Firing discrete transition %s.\n",
                 state->tau, design->events[tIndex]->event);
          fire_T(design, state, tIndex);
          fired = true;
          ecount++;
          break;
        }
      }
      // Fire tau instead.
      if (!fired) {
        double time = tau(design, state);
        if (time > maxtime - state->tau) {
          time = maxtime - state->tau;
        }
        if (time == DBL_MAX || time == 0.0) {
          hpn_go = false;
        }
        else {
          fprintf(stdout, "# %f: Firing %f time units.\n", state->tau, time);
          fire_tau(design, state, time);
          fired = true;
        }
      }
    }
    else if (!fired) {
      // Fire tau instead.
      if (!fired) {
        double time = tau(design, state);
        if (time > maxtime - state->tau) {
          time = maxtime - state->tau;
        }
        if (time == DBL_MAX || time == 0.0) {
          int offset = (rand()/RAND_MAX) * design->nevents;
          for (int t = 0; t < design->nevents; t++) {
            // want to fire a random T.
            int tIndex = (t+offset) % design->nevents;
            if (tIndex != 0 && can_fire(design, state, tIndex)) {
              fprintf(stdout, "# %f: Firing discrete transition %s.\n",
                     state->tau, design->events[tIndex]->event);
              fire_T(design, state, tIndex);
              fired = true;
              ecount++;
              break;
            }
          }
          if (!fired) {
            hpn_go = false;
          }
        }
        else {
          fprintf(stdout, "# %f: Firing %f time units.\n", state->tau, time);
          fire_tau(design, state, time);
          fired = true;
        }
      }
    }

    if (fired) {
      count++;
      print_state(design, state, sl);
    }
  }
  signal(SIGINT, SIG_DFL);
}


void view_net(designADT design, hpnStateADT state) {
  markingADT marking=(markingADT)GetBlock(sizeof *marking);
  marking->marking=(char *)GetBlock((design->nrules+2)*sizeof(char));
  marking->enablings=NULL;
  marking->state=NULL;
  marking->up=NULL;
  marking->down=NULL;
  for (int i=0;i<design->nrules;i++)
    if (state->m[i] > 0) 
      marking->marking[i]='T';
    else 
      marking->marking[i]='F';
  marking->marking[design->nrules]='\0';
  print_lpn(design->filename,design->signals,design->events,design->rules,
	    design->nevents,design->nplaces,true,marking,false,false,
	    design->dot,state->x);
  free(marking->marking);
  free(marking);
}

hpnStateADT initialize_state(designADT design) {
  hpnStateADT state = NULL;
  state = (hpnStateADT) GetBlock(sizeof(*state));
  state->m = (int*) GetBlock(design->nrules * sizeof(int));
  state->c = (double*) GetBlock(design->nrules * sizeof(double));
  state->x = (double*) GetBlock(design->nplaces * sizeof(double));

  // Copy the initial markings into the state.
  for (int i = 0; i < design->nrules; i++) {
    state->m[i] = design->markkey[i]->epsilon;
    state->c[i] = 0.0;

    // If the enabling place is continuous, store that marking
    // in x.
    int p = design->markkey[i]->enabling;
    if (con_place(design, p)) {
      state->x[p-design->nevents] = state->m[i];
    }
  }

  state->tau = 0.0;
  state->v = (double*) GetBlock(design->nevents * sizeof(double));
  state->v_cur = (bool*) GetBlock(design->nevents * sizeof(bool));
  for (int i = 0; i < design->nevents; i++) {
    state->v_cur[i] = false;
  }
  
  return state;
}

void print_headers(designADT design, hpnStateADT state, FILE* sl) {

  FILE *fp;
  int cnt=0;
  int graphs=0;
  fp = fopen("tmpplot","w");
  fprintf(fp,"set terminal postscript eps color\n");
  fprintf(fp,"set out \"tmp.eps\"\n");
  fprintf(fp,"set xlabel \"Time\"\n");
  fprintf(fp,"set ylabel \"Values of places\"\n");
  fprintf(fp,"set key bottom right\n");
  fprintf(fp,"set multiplot\n");
  for (int p = 0; p < design->nplaces; p++) {
    if (con_place(design, design->nevents + p)) {
      graphs++;
    }
  }
  fprintf(fp,"set size 1,%g\n",1.0/(double)graphs);
  /* fprintf(fp,"set origin 0.0,0.5\n");
     fprintf(fp,"plot ");*/
  bool first=true;

  fprintf(sl, "time\t");
  bool printed[design->nrules];
  for (int i = 0; i < design->nrules; i++) {
    printed[i] = false;
  }
  for (int i = 0; i < design->nrules; i++) {
    if (dis_place(design, design->markkey[i]->enabling) &&
        !printed[design->markkey[i]->enabling]) {
      cnt++;
      fprintf(sl, "%s\t",
              design->events[design->markkey[i]->enabling]->event);
      printed[design->markkey[i]->enabling] = true;
      if (first) {
	/*
	fprintf(fp,"\"hpnsim.log\" using 1:%d title \"%s\" with lines",
		cnt+1,design->events[design->markkey[i]->enabling]->event);
	*/
	first=false;
      } /*else 
	fprintf(fp,",\"hpnsim.log\" using 1:%d title \"%s\" with lines",
	cnt+1,design->events[design->markkey[i]->enabling]->event);*/
    }
    else if (dis_trans(design, design->markkey[i]->enabling) &&
             dis_trans(design, design->markkey[i]->enabled)) {
      cnt++;
      fprintf(sl, "%s>%s\t",
              design->events[design->markkey[i]->enabling]->event,
              design->events[design->markkey[i]->enabled]->event);
    }
  }
  /* fprintf(fp,"\n"); */
  int place=(-1);
  for (int p = 0; p < design->nplaces; p++) {
    if (con_place(design, design->nevents + p)) {
      cnt++;
      place++;
      first=true;
      fprintf(fp,"set ylabel \"%s\"\n",
	      design->events[design->nevents+p]->event);
      fprintf(fp,"set origin 0.0,%g\n",(double)place/(double)graphs);
      fprintf(fp,"plot ");
      fprintf(sl, "%s\t", design->events[p+design->nevents]->event);
      if (first) {
        fprintf(fp,"\"hpnsim.log\" using 1:%d title \"%s\" with lines",
                cnt+1,design->events[p+design->nevents]->event);
        first=false;
      } else 
        fprintf(fp,",\"hpnsim.log\" using 1:%d title \"%s\" with lines",
                cnt+1,design->events[p+design->nevents]->event);
      fprintf(fp,"\n");
    }
  }
  fclose(fp);
  fprintf(sl, "\n");
}

void print_state(designADT design, hpnStateADT state, FILE* sl) {
  fprintf(sl, "%f\t", state->tau);
  bool printed[design->nrules];
  for (int i = 0; i < design->nrules; i++) {
    printed[i] = false;
  }
  for (int i = 0; i < design->nrules; i++) {
    if (dis_place(design, design->markkey[i]->enabling) &&
        !printed[design->markkey[i]->enabling]) {
      fprintf(sl, "%i\t", state->m[i]);
      printed[design->markkey[i]->enabling] = true;
    }
    else if (dis_trans(design, design->markkey[i]->enabling) &&
             dis_trans(design, design->markkey[i]->enabled)) {
      fprintf(sl, "%i\t", state->m[i]);
    }
  }
  for (int p = 0; p < design->nplaces; p++) {
    if (con_place(design, design->nevents + p)) {
      fprintf(sl, "%f\t", state->x[p]);
    }
  }
  fprintf(sl, "\n");
}

void print_all_state(designADT design, hpnStateADT state) {
  printf("System Clock: %f\n", state->tau);
  printf("Clock Values: \n");
  for (int i = 0; i < design->nrules; i++) {
    if ((dis_place(design, design->markkey[i]->enabling) ||
         con_place(design, design->markkey[i]->enabling) ||
         dis_trans(design, design->markkey[i]->enabling)) &&
        dis_trans(design, design->markkey[i]->enabled)) {
      printf("  %s -> %s: %f\n",
             design->events[design->markkey[i]->enabling]->event,
             design->events[design->markkey[i]->enabled]->event,
             state->c[i]);
    }
  }
  
  printf("Marking of Discrete Places: \n");
  bool printed[design->nrules];
  for (int i = 0; i < design->nrules; i++) {
    printed[i] = false;
  }
  for (int i = 0; i < design->nrules; i++) {
    if (dis_place(design, design->markkey[i]->enabling) &&
        !printed[design->markkey[i]->enabling]) {
      printf("  %s: %i\n",
             design->events[design->markkey[i]->enabling]->event,
             state->m[i]);
      printed[design->markkey[i]->enabling] = true;
    }
    else if (dis_trans(design, design->markkey[i]->enabling) &&
             dis_trans(design, design->markkey[i]->enabled)) {
      printf("  %s -> %s: %i\n",
             design->events[design->markkey[i]->enabling]->event,
             design->events[design->markkey[i]->enabled]->event,
             state->m[i]);
    }
  }
  printf("Marking of Continuous Places: \n");
  for (int p = 0; p < design->nplaces; p++) {
    if (con_place(design, design->nevents + p)) {
      printf("  %s: %e\n",
             design->events[p+design->nevents]->event,
             state->x[p]);
    }
  }
  printf("Instantaneous Rate of Continuous Places (xdot): \n");
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->events[p]->type & CONT) {
      printf("  %s: %e\n", design->events[p]->event, xdot(design, state, p));
    }
  }

  printf("Velocity of Continuous Transitions:\n");
  for (int t = 0; t < design->nevents; t++) {
    if (design->events[t]->type & CONT) {
      printf("  %s: %e\n",design->events[t]->event,velocity(design, state, t));
    }
  }
  
}

void destroy(hpnStateADT state) {
  free(state->m);
  free(state->c);
  free(state->x);
  free(state->v);
  free(state->v_cur);
  free(state);
}

bool dis_enabled(designADT design, hpnStateADT state, int t) {
  if (design->events[t]->dropped)
    return false;
  if (t >= design->nevents)
    return false;
  // Iterate through the rules matrix to find all enabling places
  // and transitions (because of implicit places).
  for (int i = 0; i < design->nevents + design->nplaces; i++) {
    if (design->rules[i][t]->ruletype != NORULE &&
        !(design->events[i]->type & CONT)) {
      int index = design->rules[i][t]->marking;
      // if t is a continuous trans, the marking of the discrete
      // places feeding it must be within the predicate's range.
      if (design->events[t]->type & CONT) {
        int pl = design->rules[i][t]->plower;
        int pu = design->rules[i][t]->pupper;
        if (state->m[index] < pl || state->m[index] > pu) {
          return false;
        }
      }
      // if t is a discrete trans, the marking of the discrete
      // places feeding it must be greater than the arc's weight.
      else {
        int w = design->rules[i][t]->weight;
        if (w > state->m[index]) {
          return false;
        }
      }
    }
  }
  return true;
}

bool enabled(designADT design, hpnStateADT state, int t) {
  if (design->events[t]->dropped)
    return false;
  // t must be discretely enabled to be enabled.
  if (!dis_enabled(design, state, t)) {
    return false;
  }
  // If t is continuous & velocity(t) != 0, enabled
  if (design->events[t]->type & CONT) {
    if (velocity(design, state, t) == 0.0)
        return false;
  }

  // If t is discrete, continuous input places must have marking such
  // that the predicate is satisfied.
  else {
    for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
      if (design->rules[p][t]->ruletype != NORULE &&
          design->events[p]->type & CONT) {
      double pl = design->rules[p][t]->plower;
      double pu = design->rules[p][t]->pupper;
      if (state->x[p-design->nevents] < pl ||
          state->x[p-design->nevents] > pu)
        return false;
      }
    }
  }
  return true;
}

bool can_fire(designADT design, hpnStateADT state, int t) {
  if (design->events[t]->dropped)
    return false;
  if (!dis_trans(design, t)) {
    return false;
  }
  if (!enabled(design, state, t)) {
    return false;
  }
 
  // The clock on each input arc is >= the lower bounds of the delay. 
  for (int i = design->nevents; i < design->nevents+design->nplaces; i++) {
    if (design->rules[i][t]->ruletype != NORULE) {
      int index = design->rules[i][t]->marking;
      if (state->c[index] < design->rules[i][t]->lower)
        return false;
    }
  }
  return true;
}

bool must_fire(designADT design, hpnStateADT state, int t) {
  if (design->events[t]->dropped)
    return false;
  if (!dis_trans(design, t)) {
    return false;
  }
  if (!can_fire(design, state, t)) {
    return false;
  }
  
  // The clock on each input arc is >= the upper bounds of the delay. 
  for (int i = design->nevents; i < design->nevents+design->nplaces; i++) {
    if (design->rules[i][t]->ruletype != NORULE) {
      int index = design->rules[i][t]->marking;
      if (state->c[index] < design->rules[i][t]->upper)
        return false;
    }
  }
  return true;  
}

void fire_T(designADT design, hpnStateADT state, int t) {
  if (!dis_trans(design, t)) {
    return;
  }
  // Update the marking.  Implementation here differes from the model
  // slightly in that the marking is stored on the arc instead of the
  // places.
  // Implicit place situation first...
  for (int t1 = 0; t1 < design->nevents; t1++) {
    if (!(design->events[t1]->type & CONT)) {
      if (design->rules[t1][t]->ruletype != NORULE) {
        state->m[design->rules[t1][t]->marking] -=
          design->rules[t1][t]->weight;
      }
      if (design->rules[t][t1]->ruletype != NORULE) {
        state->m[design->rules[t][t1]->marking] +=
          design->rules[t][t1]->weight;
      }
    }
  }
  // Now regular place situation
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (!(design->events[p]->type & CONT)) {
      if (design->rules[p][t]->ruletype != NORULE) {
        for (int t1 = 0; t1 < design->nevents; t1++) {
          if (design->rules[p][t1]->ruletype != NORULE) {
            state->m[design->rules[p][t1]->marking] -=
              design->rules[p][t]->weight;
          }
        }
      }
      if (design->rules[t][p]->ruletype != NORULE) {
        for (int t1 = 0; t1 < design->nevents; t1++) {
          if (design->rules[p][t1]->ruletype != NORULE) {
            state->m[design->rules[p][t1]->marking] +=
              design->rules[t][p]->weight;
          }
        }
      }
    }      
  }

  // Reset the necessary clocks:
  for (int i = 0; i < design->nevents+design->nplaces; i++) {
    if (design->rules[t][i]->ruletype != NORULE &&
        !(design->events[i]->type & CONT)) {
      if (i < design->nevents) {
        state->c[design->rules[t][i]->marking] = 0;
      }
      else {
        for (int t1=0; t1 < design->nevents; t1++) {
          if (design->rules[i][t1]->ruletype != NORULE) {
            state->c[design->rules[i][t1]->marking] = 0;
          }
        }
      }
    }
  }

  // Added 7/28/04 for allowing set/reset of a continuous variable.  A
  // discrete transition that is connected to a continuous place will
  // set the continuous variable to a range of rates.  For simulation,
  // a random value between that range is chosen.
  // Now regular place situation
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if ((design->events[p]->type & CONT) &&
        design->rules[t][p]->ruletype != NORULE) {
      double fraction = (double) rand() / (double) RAND_MAX;
      double x = design->rules[t][p]->plower +
        ((design->rules[t][p]->pupper-design->rules[t][p]->plower) * fraction);
      state->x[p-design->nevents] = x;
      cout << "Setting continuous variable " << design->events[p]->event
           << " to " << x << endl;
    }
  }
  
  reset_velocity(design, state);
}

void fire_tau(designADT design, hpnStateADT state, double tau) {
  state->tau += tau;
  // Increment all the clocks.
  for (int i = 0; i < design->nrules; i++) {
    state->c[i] = state->c[i] + tau;
  }

  // Update the markings of continuous places
  for (int p = 0; p < design->nplaces; p++) {
    int ep = p + design->nevents;
    if (design->events[ep]->type & CONT) {
      double xd = xdot(design, state, ep);
      double xprime = state->x[p] + tau * xd;
      if (xprime < design->events[ep]->lrange + 0.000000000001)
        xprime = design->events[ep]->lrange;
      else if (xprime > design->events[ep]->urange - 0.000000000001)
        xprime = design->events[ep]->urange;
      for (int t = 1; t < design->nevents; t++) {
        int index = design->rules[ep][t]->marking;
        if (design->rules[ep][t]->ruletype != NORULE &&
            design->events[t]->type & CONT &&
            state->x[p] < design->rules[ep][t]->plower &&
            xprime >= design->rules[ep][t]->plower) {
          state->c[index] = (xprime - (double) design->rules[ep][t]->plower) /
            xd;
        }
        else if (design->rules[ep][t]->ruletype != NORULE &&
                 design->events[t]->type & CONT &&
                 state->x[p] > design->rules[ep][t]->pupper &&
                 xprime <= design->rules[ep][t]->pupper) {
          state->c[index] = ((double) design->rules[ep][t]->pupper - xprime) /
            xd;
        }
      }
      state->x[p] = xprime;
    }
  }
  reset_velocity(design, state);
}

double xdot(designADT design, hpnStateADT state, int p) {
  if (!con_place(design, p)) {
    return 0.0;
  }
  double xdot = 0;
  for (int t = 0; t < design->nevents; t++) {
    if (design->events[t]->type & CONT) {
      if (design->rules[t][p]->ruletype != NORULE) {
        xdot += design->rules[t][p]->weight * velocity(design, state, t);
      }
      if (design->rules[p][t]->ruletype != NORULE) {
        xdot -= design->rules[p][t]->weight * velocity(design, state, t);
      }
    }
  }
  return xdot;
}


double velocity(designADT design, hpnStateADT state, int t) {
  if (t >= design->nevents) {
    return 0.0;
  }
  // If we've already calculated it, just use that value.
  if (state->v_cur[t] == true) {
    return state->v[t];
  }

  // Otherwise do the hard work again.
  bool* visited = (bool*) GetBlock(design->nevents * sizeof(bool));
  for (int i = 0; i < design->nevents; i++) {
    visited[i] = false;
  }
  double result = velocity(design, state, t, visited);
  free(visited);
  return result;
}

double velocity(designADT design, hpnStateADT state, int t, bool* visited) {
  if (t >= design->nevents) {
    return 0.0;
  }

  // If we've already calculated it, just use that value.
  if (state->v_cur[t] == true) {
    return state->v[t];
  }
  
  // if t has been visited or is not discretely enabled, velocity is 0.
  if (!dis_enabled(design, state, t) || visited[t]) {
    state->v[t] = 0.0;
    state->v_cur[t] = true;
    return 0.0;
  }
  double min = 1.0;
  // For each continous place p in the preset of t with marking equal
  // to lrange...
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->rules[p][t]->ruletype != NORULE &&
        design->events[p]->type & CONT && 
        state->x[p-design->nevents] <= design->events[p]->lrange) {
      // For each continuous transition t1 in the preset of p...
      double num = 0;
      for (int t1 = 0; t1 < design->nevents; t1++) {
        if (design->rules[t1][p]->ruletype != NORULE &&
            design->events[t1]->type & CONT) {
          visited[t] = true;
          num += (design->rules[t1][p]->weight *
                  velocity(design, state, t1, visited));
          visited[t] = false;
        }
      }
      // For each continuous transition t2 in the postset of p...
      double den = 0;
      for (int t2 = 0; t2 < design->nevents; t2++) {
        if (design->rules[p][t2]->ruletype != NORULE &&
            design->events[t2]->type & CONT) {
          den += (design->rules[p][t2]->weight * design->events[t2]->rate);
        }
      }
      if (den == 0) {
        printf("FATAL ERROR: DIVISION BY ZERO WHEN CALCULATING ");
        printf("VELOCITY OF %s!!!\n", design->events[t]->event);
        printf("             PLACE: %s!!!\n", design->events[p]->event);
        exit(0);
      }
      if (num/den < min) {
        min = num/den;
      }
    }
  }

  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->rules[t][p]->ruletype != NORULE &&
        design->events[p]->type & CONT && 
        state->x[p-design->nevents] >= design->events[p]->urange) {
      // For each continuous transition t1 in the preset of p...
      double num = 0;
      for (int t1 = 0; t1 < design->nevents; t1++) {
        if (design->rules[p][t1]->ruletype != NORULE &&
            design->events[t1]->type & CONT) {
          visited[t] = true;
          num += (design->rules[p][t1]->weight *
                  velocity(design, state, t1, visited));
          visited[t] = false;
        }
      }
      // For each continuous transition t2 in the postset of p...
      double den = 0;
      for (int t2 = 0; t2 < design->nevents; t2++) {
        if (design->rules[t2][p]->ruletype != NORULE &&
            design->events[t2]->type & CONT) {
          den += (design->rules[t2][p]->weight * design->events[t2]->rate);
        }
      }
      if (den == 0) {
        printf("FATAL ERROR: DIVISION BY ZERO WHEN CALCULATING ");
        printf("VELOCITY OF %s!!!\n", design->events[t]->event);
        printf("             PLACE: %s!!!\n", design->events[p]->event);
        exit(0);
      }
      if (num/den < min) {
        min = num/den;
      }
    }
  }

  state->v[t] = min * design->events[t]->rate;
  state->v_cur[t] = true;
  return state->v[t];
}

void reset_velocity(designADT design, hpnStateADT state) {
  for (int i = 0; i < design->nevents; i++) {
    state->v_cur[i] = false;
  }
}

double tau(designADT design, hpnStateADT state) {
  double min = tau_min(design, state);
  double max = tau_max(design, state);
  //if (max < 0.0)
  //  max = 0.00;
  double t = 0.00;
  if (min < 0) {
    print_all_state(design, state);
    printf("FATAL ERROR: MIN_ADVANCE IS NEGATIVE!!!\n");
    printf("*** MIN: %e; MAX: %e\n", min, max);
    exit(0);
  }
  if (min > max) {
    print_all_state(design, state);
    printf("FATAL ERROR: MIN_ADVANCE > MAX_ADVANCE!!!\n");
    printf("*** MIN: %e; MAX: %e\n", min, max);
    exit(0);
  }
  
  if (min == 0.00 && max == DBL_MAX) {
    t = MAX_STEP;
  }
  else if (min != DBL_MAX && max == DBL_MAX) {
    t = min;
  }
  else if ((max-min) < MIN_STEP) {
    t = max;
  }
  else {
    double fraction = (double) rand() / (double) RAND_MAX;
    t = min + ((max-min) * fraction);
  }
  
  if (t < 0) {
    print_all_state(design, state);
    printf("FATAL ERROR: TAU < 0!!! min: %e; max: %e\n",
           min, max);
    exit(0);
  }
  return t;
}

double tau_min(designADT design, hpnStateADT state) {
  double overall_min = DBL_MAX;
  for (int t = 1; t < design->nevents; t++) {
    double max = 0.0;
    if (can_fire(design, state, t)) {
      return 0.0;
    }
    else if (dis_enabled(design, state, t)) {
      for (int p = 1; p < design->nevents+design->nplaces; p++) {
        double v = max;
        if (design->rules[p][t]->ruletype != NORULE) {
          int index = design->rules[p][t]->marking;
          if (!(design->events[p]->type & CONT)) {
            v = design->rules[p][t]->lower - state->c[index];
          }
          else if (design->rules[p][t]->plower<=state->x[p-design->nevents] &&
                   state->x[p-design->nevents]<=design->rules[p][t]->pupper) {
            if (design->rules[p][t]->upper == INFIN)
              v = INFIN;
            else
              v = design->rules[p][t]->lower - state->c[index];
          }
          else if (state->x[p-design->nevents] < design->rules[p][t]->plower &&
                   xdot(design, state, p) > 0) {
            v = (((double) design->rules[p][t]->plower) -
                 state->x[p-design->nevents]) /
              xdot(design, state, p) + design->rules[p][t]->lower;
          }
          else if (state->x[p-design->nevents] > design->rules[p][t]->pupper &&
                   xdot(design, state, p) < 0) {
            v = (((double) design->rules[p][t]->pupper) -
                 state->x[p-design->nevents]) /
              xdot(design, state, p) + design->rules[p][t]->lower;
          }
        }
        if (v > max) {
          max = v;
        }
      }
      if (max < overall_min) {
        overall_min = max;
      }
    }
  }
  double tau_empty = tau_con_max(design, state);
  if (tau_empty < overall_min)
    return tau_empty;
  else
    return overall_min;
}

double tau_dis_max(designADT design, hpnStateADT state) {
  double min = DBL_MAX;
  for (int t = 1; t < design->nevents; t++) {
    double max = -1;
    if (dis_enabled(design, state, t)) {
      for (int p = 1; p < design->nevents+design->nplaces; p++) {
        double v = max;
        if (design->rules[p][t]->ruletype != NORULE) {
          int index = design->rules[p][t]->marking;
          if (!(design->events[p]->type & CONT)) {
            if (design->rules[p][t]->upper == INFIN)
              v = INFIN;
            else
              v = design->rules[p][t]->upper - state->c[index];
          }
          else if (design->rules[p][t]->plower<=state->x[p-design->nevents] &&
                   state->x[p-design->nevents]<=design->rules[p][t]->pupper) {
            if (design->rules[p][t]->upper == INFIN)
              v = INFIN;
            else
              v = design->rules[p][t]->upper - state->c[index];
          }
          else if (state->x[p-design->nevents] < design->rules[p][t]->plower &&
                   xdot(design, state, p) > 0) {
            if (design->rules[p][t]->upper == INFIN)
              v = DBL_MAX;
            else
              v = (((double) design->rules[p][t]->plower) -
                   state->x[p-design->nevents]) /
                xdot(design, state, p) + design->rules[p][t]->upper;
          }
          else if (state->x[p-design->nevents] > design->rules[p][t]->pupper &&
                   xdot(design, state, p) < 0) {
            if (design->rules[p][t]->upper == INFIN ||
                design->rules[p][t]->pupper == INFIN)
              v = DBL_MAX;
            else 
              v = (((double) design->rules[p][t]->pupper) -
                   state->x[p-design->nevents]) /
                xdot(design, state, p) + design->rules[p][t]->upper;
          }
          else {
            v = DBL_MAX;
          }
        }
        if (v > max) {
          max = v;
        }
      }
      if (max != -1 && max < min) {
        min = max;
      }
    }
  }
  //double tau_empty = tau_con_max(design, state);
  //if (tau_empty < min)
  //  return tau_empty;
  //else
    return min;
}

double tau_con_max(designADT design, hpnStateADT state) {
  double min = DBL_MAX;
  for (int p = design->nevents; p < design->nevents+design->nplaces; p++) {
    if (design->events[p]->type & CONT) {
      double xdotp = xdot(design, state, p);
      if ((xdotp < 0) &&
	  (state->x[p-design->nevents]-design->events[p]->lrange != -INFIN)) {
        double t = -1 * (state->x[p-design->nevents]-design->events[p]->lrange)
          /xdotp;
        if (t < min) min = t;
      }
      if ((xdotp > 0) &&
          (state->x[p-design->nevents]-design->events[p]->urange != INFIN)) {
        double t = (design->events[p]->urange-state->x[p-design->nevents])
          /xdotp;
        if (t < min) min = t;
      }
    }
  }
  return min;
}

double tau_max(designADT design, hpnStateADT state) {
  double con_max = tau_con_max(design, state);
  double dis_max = tau_dis_max(design, state);
  // printf("con_max: %f\n", con_max);
  // printf("dis_max: %f\n", dis_max);
  return (con_max < dis_max ? con_max : dis_max);
}
