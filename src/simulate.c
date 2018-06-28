/*****************************************************************************/
/*                                                                           */
/* Automated Timed Asynchronous Circuit Synthesis (ATACS)                    */
/*                                                                           */
/*   Copyright (C) 1993 by, Chris J. Myers                                   */
/*   Center for Integrated Systems,                                          */
/*   Stanford University                                                     */
/*                                                                           */
/*   Permission to use, copy, modify and/or distribute, but not sell, this   */
/*   software and its documentation for any purpose is hereby granted        */
/*   without fee, subject to the following terms and conditions:             */
/*                                                                           */
/*   1.  The above copyright notice and this permission notice must          */
/*   appear in all copies of the software and related documentation.         */
/*                                                                           */
/*   2.  The name of Stanford University may not be used in advertising or   */
/*   publicity pertaining to distribution of the software without the        */
/*   specific, prior written permission of Stanford.                         */
/*                                                                           */
/*   3.  This software may not be called "ATACS" if it has been modified     */
/*   in any way, without the specific prior written permission of            */
/*   Chris J. Myers.                                                         */
/*                                                                           */
/*   4.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,  */
/*   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY        */
/*   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.        */
/*                                                                           */
/*   IN NO EVENT SHALL STANFORD OR THE AUTHORS OF THIS SOFTWARE BE LIABLE    */
/*   FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY   */
/*   KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR     */
/*   PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON    */
/*   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE   */
/*   OR PERFORMANCE OF THIS SOFTWARE.                                        */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* simulate.c                                                                */
/*****************************************************************************/

#include "simulate.h"
#include "merges.h"
#include "string.h"

/*****************************************************************************/
/* Print an assignment.                                                      */

/*****************************************************************************/

void
print_assignment(FILE *fp, char * event, int delay, int value) {
    int j;

    fprintf(fp, "#%d assign ", delay);
    for (j = 0; (event[j] != '+') && (event[j] != '-'); j++)
        fprintf(fp, "%c", event[j]);
    fprintf(fp, "=%d;\n", value);
}

/*****************************************************************************/
/* Print a condition.                                                        */

/*****************************************************************************/

void
print_condition(FILE *fp, char * event, int value, bool first) {
    int j;

    if (!first) fprintf(fp, " && ");
    fprintf(fp, "(");
    for (j = 0; (event[j] != '+') && (event[j] != '-'); j++)
        fprintf(fp, "%c", event[j]);
    fprintf(fp, "==%d)", value);
}

/*****************************************************************************/
/* Print the initialization.                                                 */

/*****************************************************************************/

void
print_initialization(FILE *fp, eventADT *events, char * startstate, int i) {
    int value;

    if (startstate == NULL) {
        printf("ERROR:  Need initial state information.\n");
        fprintf(lg, "ERROR:  Need initial state information.\n");
	return;
    } else value = startstate[events[i]->signal];
    fprintf(fp, "initial\n");
    fprintf(fp, "  begin\n");
    fprintf(fp, "    ");
    if (value == '0') print_assignment(fp, events[i]->event, 0, 0);
    else print_assignment(fp, events[i]->event, 0, 1);
    fprintf(fp, "  end\n\n");
}

/*****************************************************************************/
/* Print the initialization.                                                 */

/*****************************************************************************/

void
print_gate_initialization(FILE *fp, signalADT *signals,
        char * startstate, int i) {
    int value;

    if (startstate == NULL) {
        printf("ERROR:  Need initial state information.\n");
        fprintf(lg, "ERROR:  Need initial state information.\n");
	return;
    } else value = startstate[i];
    fprintf(fp, "initial\n");
    fprintf(fp, "  begin\n");
    fprintf(fp, "    ");
    fprintf(fp, "#0 assign %s=%c;\n", signals[i]->name, value);
    fprintf(fp, "  end\n\n");
}

/*****************************************************************************/
/* Print a production.                                                       */

/*****************************************************************************/

void
print_production(FILE *fp, eventADT *events, mergeADT *merges,
        ruleADT **rules, int nevents, int ninputs, int j) {
    int i;
    bool firstAND;
    int delay;
    int k;

    fprintf(fp, "always\n");
    fprintf(fp, "  begin\n");
    for (k = j; k < ninputs + 1; k += 2)
        if ((j == k) || (are_merged(merges, j, k))) {
            firstAND = TRUE;
            for (i = 1; i < nevents; i += 2) {
                if ((rules[i][k]->ruletype != NORULE) &&
                        (!(rules[i][k]->ruletype & REDUNDANT))) {
                    if (!(rules[i][k]->ruletype & (ORDERING | REDUNDANT)))
                        delay = rules[i][k]->lower;
                    if (firstAND) {
                        fprintf(fp, "    wait(");
                        print_condition(fp, events[j]->event, 1 - (j % 2), firstAND);
                        firstAND = FALSE;
                    }
                    if (!are_merged(merges, i, k))
                        print_condition(fp, events[i]->event, 1, firstAND);
                }
                if ((rules[i + 1][k]->ruletype != NORULE) &&
                        (!(rules[i + 1][k]->ruletype & REDUNDANT))) {
		  if (!(rules[i + 1][k]->ruletype & (ORDERING | REDUNDANT))) {
		      if (k > ninputs) {
			delay = rules[i + 1][k]->upper; 
		      } else {
			delay = rules[i + 1][k]->lower;
		      }
		  }
		  if (firstAND) {
		    fprintf(fp, "    wait(");
		    print_condition(fp, events[j]->event, 1 - (j % 2), firstAND);
		    firstAND = FALSE;
		  }
		  if (!are_merged(merges, i, k))
		    print_condition(fp, events[i]->event, 0, firstAND);
                }
            }
            if (!firstAND) {
                fprintf(fp, ");\n");
                fprintf(fp, "    ");
                print_assignment(fp, events[j]->event, delay, j % 2);
            }
        }
    fprintf(fp, "  end\n\n");
}

/*****************************************************************************/
/* Print a production.                                                       */

/*****************************************************************************/
void
print_WC_gate_production(FILE *fp, signalADT *signals, regionADT *regions,
        int nsignals, int ninpsig) {

    int i = 0;
    int sig = 0;
    coverlistADT cur_cover;
    
    for (int input = ninpsig; input < nsignals; input++) {
        sig = input;
        i = 2 * input + 1;
        bool firstOR, firstAND;
        int j;

        regionADT cur_region;
	bool OPEN;
	
        firstOR = TRUE;
        fprintf(fp, "assign %s = ", signals[sig]->name);
        for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
	  if (cur_region->covers && cur_region->covers->cover) {
	    for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
		 cur_cover=cur_cover->link) {
	      if (cur_cover->cover[0] != 'E') { //if don't care?
                if (!firstOR) fprintf(fp, " | ");
                firstOR = FALSE;
                firstAND = TRUE;
		OPEN = FALSE;
                for (j = 0; j < nsignals; j++) {
                    if (!OPEN)
                        fprintf(fp, "(");
                    OPEN = TRUE;
                    if (cur_cover->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                    }
                    if (cur_cover->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                    }
                }
                if (OPEN)
                    fprintf(fp, ")");
		OPEN = FALSE;
	      }
	    }
	  } else {
            if (cur_region->cover[0] != 'E') { //if don't care?
                if (!firstOR) fprintf(fp, " | ");
                firstOR = FALSE;
                firstAND = TRUE;
		OPEN = FALSE;
                for (j = 0; j < nsignals; j++) {
                    if (!OPEN)
                        fprintf(fp, "(");
                    OPEN = TRUE;
                    if (cur_region->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                    }
                    if (cur_region->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                    }
                }
                if (OPEN)
                    fprintf(fp, ")");
		OPEN = FALSE;
	    }
	  }
        }
        firstOR=TRUE;
        bool RESET = FALSE;
        for (cur_region = regions[i + 1]; cur_region; cur_region = cur_region->link) { //once with i for set
	  if (cur_region->covers && cur_region->covers->cover) {
	    for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
		 cur_cover=cur_cover->link) {
	      if (cur_cover->cover[0] != 'E') { //if don't care?
                if (!firstOR) fprintf(fp, " & ");
		else fprintf(fp, " | ");
                firstOR = FALSE;
                firstAND = TRUE;
		OPEN = FALSE;
                for (j = 0; j < nsignals; j++) {
                    if (!OPEN)
                        fprintf(fp, "(");
                    OPEN = TRUE;
                    if (cur_cover->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " | ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                    if (cur_cover->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " | ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                }
                if (RESET) {
                    fprintf(fp, ")");
                }
	      }
	    }
	  } else {
            if (cur_region->cover[0] != 'E') { //if don't care?
                if (!firstOR) fprintf(fp, " & ");
		else fprintf(fp, " | ");
                firstOR = FALSE;
                firstAND = TRUE;
		OPEN = FALSE;
                for (j = 0; j < nsignals; j++) {
                    if (!OPEN)
                        fprintf(fp, "(");
                    OPEN = TRUE;
                    if (cur_region->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " | ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                    if (cur_region->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " | ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                }
                if (RESET) {
                    fprintf(fp, ")");
                }
            }
	  }
        }
	if (RESET) {
	  fprintf(fp, " & %s", signals[sig]->name); //& SIG
	}
        fprintf(fp, ";\n");
    }
}

/*****************************************************************************/
/* Print initialization                                                      */
/*****************************************************************************/
void
print_WC_gate_initialization(FILE *fp, signalADT *signals, regionADT *regions,
			     int nsignals, int ninpsig, char *startstate, int gateLevel) {
  int i = 0;
  int sig = 0;

  fprintf(fp, "// Initial state: \n// ");

  for (int i = 0; i < nsignals; i++) {

    int value;

    if (startstate == NULL) {
      printf("ERROR:  Need initial state information.\n");
      fprintf(lg, "ERROR:  Need initial state information.\n");
      return;
    } else value = startstate[i];
    if (value == '0') {
      fprintf(fp, "!%s ", signals[i]->name);
    } else {
      fprintf(fp, "%s ", signals[i]->name);
    }
  }

  if (gateLevel) {
    for (int input = ninpsig; input < nsignals; input++) {
      sig = input;
      i = 2 * input + 1;
      int j;
      regionADT cur_region;
      coverlistADT cur_cover;
      bool initialValue;
      bool SETValue = FALSE;
      bool RESETValue = TRUE;
      bool foundIt;
    
      int set_counter = 1;
      int reset_counter = 1;
      for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
	if (cur_region->covers && cur_region->covers->cover) {
	  foundIt = FALSE;
	  for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
	       cur_cover=cur_cover->link) {
	    if (cur_cover->cover[0] != 'E') { //if don't care?
	      foundIt = TRUE;
	      initialValue = TRUE;
	      for (j = 0; j < nsignals; j++) {
		if (cur_cover->cover[j] == '0') {
		  if (startstate[j] == '1') {
		    initialValue = FALSE;
		  }
		}
		if (cur_cover->cover[j] == '1') { 
		  if (startstate[j] == '0') {
		    initialValue = FALSE;
		  }
		}
	      }
	      if (initialValue) {
		SETValue = TRUE;
		break;
	      }
	    }
	  }
	  if (foundIt) {
	    fprintf(fp, "%c%s_SET%i ",initialValue?' ':'!', signals[input]->name, set_counter);
	    set_counter++;
	  }
	} else {
	  if (cur_region->cover[0] != 'E') { //if don't care?
	    initialValue = TRUE;
	    for (j = 0; j < nsignals; j++) {
	      if (cur_region->cover[j] == '0') { //only include signal into cover if 1 or 0
		if (startstate[j] == '1') {
		  initialValue = FALSE;
		}
	      }
	      if (cur_region->cover[j] == '1') { //only include signal into cover if 1 or 0
		if (startstate[j] == '0') {
		  initialValue = FALSE;
		}
	      }
	    }
	    if (initialValue) {
	      SETValue = TRUE;
	    }
	    fprintf(fp, "%c%s_SET%i ",initialValue?' ':'!', signals[input]->name, set_counter);
	    set_counter++;
	  }
	}
      }
      for (cur_region = regions[i+1]; cur_region; cur_region = cur_region->link) { //once with i for set
	if (cur_region->covers && cur_region->covers->cover) {
	  foundIt = FALSE;
	  for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
	       cur_cover=cur_cover->link) {
	    if (cur_cover->cover[0] != 'E') { //if don't care?
	      foundIt=TRUE;
	      initialValue = TRUE;
	      for (j = 0; j < nsignals; j++) {
		if (cur_cover->cover[j] == '0') {
		  if (startstate[j] == '1') {
		    initialValue = FALSE;
		  }
		}
		if (cur_cover->cover[j] == '1') { 
		  if (startstate[j] == '0') {
		    initialValue = FALSE;
		  }
		}
	      }
	      if (initialValue) {
		RESETValue = FALSE;
		break;
	      }
	    }
	  }
	  if (foundIt) {
	    fprintf(fp, "%c%s_RESET%i ",initialValue?' ':'!', signals[input]->name, reset_counter);
	    reset_counter++;
	  }
	} else {
	  if (cur_region->cover[0] != 'E') { //if don't care?
	    initialValue = TRUE;
	    for (j = 0; j < nsignals; j++) {
	      if (cur_region->cover[j] == '0') { //only include signal into cover if 1 or 0
		if (startstate[j] == '1') {
		  initialValue = FALSE;
		}
	      }
	      if (cur_region->cover[j] == '1') { //only include signal into cover if 1 or 0
		if (startstate[j] == '0') {
		  initialValue = FALSE;
		}
	      }
	    }
	    if (initialValue) {
	      RESETValue = FALSE;
	    }
	    fprintf(fp, "%c%s_RESET%i ",initialValue?' ':'!', signals[input]->name, reset_counter);
	    reset_counter++;
	  }
	}
      }
      if (set_counter == 2 && reset_counter == 2) { //single set and reset function
	// do nothing
      } else if (set_counter > 2 && reset_counter == 2){ //multiset, single reset function
	fprintf(fp, "%c%s_SET ", SETValue?' ':'!', signals[input]->name);
      } else if (set_counter == 2 && reset_counter > 2) { //single set, multireset functions
	fprintf(fp, "%c%s_nRESET ", RESETValue?' ':'!', signals[input]->name);
      } else { //multiset and multireset functions
	fprintf(fp, "%c%s_SET ", SETValue?' ':'!', signals[input]->name);
	fprintf(fp, "%c%s_nRESET ", RESETValue?' ':'!', signals[input]->name);
      }
    }
  }
  
  fprintf(fp, "\n\n");
}

/*****************************************************************************/
/* Print a production.                                                       */
/*****************************************************************************/
void
print_WC_standard_gate_production(FILE *fp, signalADT *signals, regionADT *regions,
        int nsignals, int ninpsig) {

    int i = 0;
    int sig = 0;
    coverlistADT cur_cover;

    for (int input = ninpsig; input < nsignals; input++) {

        sig = input;
        i = 2 * input + 1;
        bool firstOR, firstAND;
        int j;
        regionADT cur_region;
        bool SET = FALSE;
        bool RESET = FALSE;

        int set_counter = 1;
        int reset_counter = 1;
        for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
	  if (cur_region->covers && cur_region->covers->cover) {
	    firstOR = TRUE;
	    for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
		 cur_cover=cur_cover->link) {
	      if (cur_cover->cover[0] != 'E') { //if don't care?
		if (firstOR) {
		  fprintf(fp, "assign %s_SET%i = ", signals[input]->name, set_counter);
		  set_counter++;
		  firstOR = FALSE;
		} else {
		  fprintf(fp, " | ");
		}
		fprintf(fp,"(");
                firstAND = TRUE;
                for (j = 0; j < nsignals; j++) {
                    if (cur_cover->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                        SET = TRUE;
                    }
                    if (cur_cover->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                        SET = TRUE;
                    }
                }
		fprintf(fp,")");
	      }
	    }
	    if (!firstOR) fprintf(fp, ";\n");
	  } else {
            if (cur_region->cover[0] != 'E') { //if don't care?

                fprintf(fp, "assign %s_SET%i = ", signals[input]->name, set_counter);
                set_counter++;

                firstAND = TRUE;
                for (j = 0; j < nsignals; j++) {

                    if (cur_region->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                        SET = TRUE;
                    }
                    if (cur_region->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                        SET = TRUE;
                    }
                }
                fprintf(fp, ";\n");
	    }
	  }
        }

        for (cur_region = regions[i + 1]; cur_region; cur_region = cur_region->link) { //once with i for reset
	  if (cur_region->covers && cur_region->covers->cover) {
	    firstOR = TRUE;
	    for (cur_cover=cur_region->covers;cur_cover && cur_cover->cover;
		 cur_cover=cur_cover->link) {
	      if (cur_cover->cover[0] != 'E') { //if don't care?
		if (firstOR) {
		  fprintf(fp, "assign %s_RESET%i = ", signals[input]->name, reset_counter);
		  reset_counter++;
		  firstOR = FALSE;
		} else {
		  fprintf(fp, " | ");
		}
		fprintf(fp,"(");
                firstAND = TRUE;
                for (j = 0; j < nsignals; j++) {
                    if (cur_cover->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                    if (cur_cover->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                }
		fprintf(fp,")");
	      }
	    }
	    if (!firstOR) fprintf(fp, ";\n");
	  } else {
            if (cur_region->cover[0] != 'E') { //if don't care?

                fprintf(fp, "assign %s_RESET%i = ", signals[input]->name, reset_counter);
                reset_counter++;

                firstAND = TRUE;
                for (j = 0; j < nsignals; j++) {
                    if (cur_region->cover[j] == '0') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "~%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                    if (cur_region->cover[j] == '1') { //only include signal into cover if 1 or 0
                        if (!firstAND) fprintf(fp, " & ");
                        fprintf(fp, "%s", signals[j]->name);
                        firstAND = FALSE;
                        RESET = TRUE;
                    }
                }
                fprintf(fp, ";\n");
	    }
	  }
        }
        fprintf(fp, "\n");

        if (set_counter == 2 && reset_counter == 2) { //single set and reset function
            int which_set = 1;
            int which_reset = 1;
            set_counter = 1;
            for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
                if (cur_region->cover[0] != 'E') { //if don't care?

                    which_set = set_counter;
                    set_counter++;
                }
            }

            reset_counter = 1;
            for (cur_region = regions[i + 1]; cur_region; cur_region = cur_region->link) { //once with i for set
                if (cur_region->cover[0] != 'E') { //if don't care?

                    which_reset = reset_counter;
                    reset_counter++;
                }
            }
            
            if (SET && RESET) {
                fprintf(fp, "assign %s = %s_SET%i & ~(%s_RESET%i) | %s & (%s_SET%i | ~(%s_RESET%i));\n", signals[sig]->name, signals[sig]->name, which_set, signals[sig]->name, which_reset, signals[sig]->name, signals[sig]->name, which_set, signals[sig]->name, which_reset);
            } else if (SET) {
                fprintf(fp, "assign %s = %s_SET%i;\n", signals[sig]->name, signals[sig]->name, which_set);
            } else if (RESET) {

                fprintf(fp, "assign %s = ~(%s_RESET%i);\n", signals[sig]->name, signals[sig]->name, which_reset);
            }
            fprintf(fp, "\n");
        } 
        else if (set_counter > 2 && reset_counter == 2){ //multiset, single reset function
            set_counter = 1;
            if (SET) {
                fprintf(fp, "assign %s_SET = ", signals[sig]->name);
                firstOR = TRUE;
                for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
                    if (cur_region->cover[0] != 'E') { //if don't care?

                        if (!firstOR) fprintf(fp, " | ");
                        firstOR = FALSE;
                        fprintf(fp, "%s_SET%i", signals[input]->name, set_counter);
                        set_counter++;
                    }
                }
                fprintf(fp, ";\n");
            }
             
            int which_reset = 1;
            reset_counter = 1;
            for (cur_region = regions[i + 1]; cur_region; cur_region = cur_region->link) { //once with i for set
                if (cur_region->cover[0] != 'E') { //if don't care?

                    which_reset = reset_counter;
                    reset_counter++;
                }
            }
            
	    fprintf(fp, "\n");


            if (SET && RESET) {
                fprintf(fp, "assign %s = %s_SET & ~(%s_RESET%i) | %s & (%s_SET | ~(%s_RESET%i));\n", signals[sig]->name, signals[sig]->name, signals[sig]->name, which_reset, signals[sig]->name, signals[sig]->name, signals[sig]->name, which_reset);
            } else if (SET) {
                fprintf(fp, "assign %s = %s_SET;\n", signals[sig]->name, signals[sig]->name);
            } else if (RESET) {
                fprintf(fp, "assign %s = ~(%s_RESET%i);\n", signals[sig]->name, signals[sig]->name, which_reset);
            }
            fprintf(fp, "\n");
            
            
        } else if (set_counter == 2 && reset_counter > 2) { //single set, multireset functions

            int which_set = 1;
            set_counter = 1;
            for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
                if (cur_region->cover[0] != 'E') { //if don't care?

                    which_set = set_counter;
                    set_counter++;
                }
            }

            reset_counter = 1;
            if (RESET) {
                fprintf(fp, "assign %s_nRESET = ~(", signals[sig]->name);
                firstOR = TRUE;
                for (cur_region = regions[i + 1]; cur_region; cur_region = cur_region->link) { //once with i for set
                    if (cur_region->cover[0] != 'E') { //if don't care?

                        if (!firstOR) fprintf(fp, " | ");
                        firstOR = FALSE;
                        fprintf(fp, "%s_RESET%i", signals[input]->name, reset_counter);
                        reset_counter++;
                    }
                }
                fprintf(fp, ");\n");
            }
            fprintf(fp, "\n");


            if (SET && RESET) {
                fprintf(fp, "assign %s = %s_SET%i & %s_nRESET | %s & (%s_SET%i | %s_nRESET);\n", signals[sig]->name, signals[sig]->name, which_set, signals[sig]->name, signals[sig]->name, signals[sig]->name, which_set, signals[sig]->name);
            } else if (SET) {
                fprintf(fp, "assign %s = %s_SET%i;\n", signals[sig]->name, signals[sig]->name, which_set);
            } else if (RESET) {
                fprintf(fp, "assign %s = %s_nRESET;\n", signals[sig]->name, signals[sig]->name);
            }
            fprintf(fp, "\n");


        } else { //multiset and multireset functions
            set_counter = 1;
            if (SET) {
                fprintf(fp, "assign %s_SET = ", signals[sig]->name);
                firstOR = TRUE;
                for (cur_region = regions[i]; cur_region; cur_region = cur_region->link) { //once with i for set
                    if (cur_region->cover[0] != 'E') { //if don't care?

                        if (!firstOR) fprintf(fp, " | ");
                        firstOR = FALSE;
                        fprintf(fp, "%s_SET%i", signals[input]->name, set_counter);
                        set_counter++;
                    }
                }
                fprintf(fp, ";\n");
            }

            reset_counter = 1;
            if (RESET) {
                fprintf(fp, "assign %s_nRESET = ~(", signals[sig]->name);
                firstOR = TRUE;
                for (cur_region = regions[i + 1]; cur_region; cur_region = cur_region->link) { //once with i for set
                    if (cur_region->cover[0] != 'E') { //if don't care?

                        if (!firstOR) fprintf(fp, " | ");
                        firstOR = FALSE;
                        fprintf(fp, "%s_RESET%i", signals[input]->name, reset_counter);
                        reset_counter++;
                    }
                }
                fprintf(fp, ");\n");
            }
            fprintf(fp, "\n");


            if (SET && RESET) {
                fprintf(fp, "assign %s = %s_SET & %s_nRESET | %s & (%s_SET | %s_nRESET);\n", signals[sig]->name, signals[sig]->name, signals[sig]->name, signals[sig]->name, signals[sig]->name, signals[sig]->name);
            } else if (SET) {
                fprintf(fp, "assign %s = %s_SET;\n", signals[sig]->name, signals[sig]->name);
            } else if (RESET) {
                fprintf(fp, "assign %s = %s_nRESET;\n", signals[sig]->name, signals[sig]->name);
            }
            fprintf(fp, "\n");

        }
    }
}

void
print_gate_production(FILE *fp, signalADT *signals, regionADT *regions,
        int nsignals, int sig, int i) {
    bool firstOR, firstAND;
    int j;

    regionADT cur_region;

    fprintf(fp, "always\n");
    fprintf(fp, "  begin\n");
    fprintf(fp, "    wait((%s==%d) && (", signals[sig]->name, (1 - (i % 2)));
    firstOR = TRUE;
    for (cur_region = regions[i]; cur_region; cur_region = cur_region->link)
        if (cur_region->cover[0] != 'E') {
            if (!firstOR) fprintf(fp, " || "); //might not need
            fprintf(fp, "(");
            firstOR = FALSE;
            firstAND = TRUE;
            for (j = 0; j < nsignals; j++)
                if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) { //signal is used in the cover if 0 or 1
                    if (!firstAND) fprintf(fp, " && ");
                    fprintf(fp, "(%s==%c)", signals[j]->name, cur_region->cover[j]);
                    firstAND = FALSE;
                }
            fprintf(fp, ")");
        }
    fprintf(fp, "));\n");
    if ((i % 2) == 1)
        fprintf(fp, "    #%d assign %s=1;\n", signals[sig]->riseupper,
            signals[sig]->name);

    else
        fprintf(fp, "    #%d assign %s=0;\n", signals[sig]->fallupper,
            signals[sig]->name);
    fprintf(fp, "  end\n\n");
}

/*****************************************************************************/
/* Print an interface for a module.                                          */

/*****************************************************************************/

void
print_interface(FILE *fp, eventADT *events, mergeADT *merges,
        ruleADT **rules, int nevents, int ninputs, int i) {
    bool first;
    int j, k, l;
    char * slash;

    first = TRUE;
    fprintf(fp, "module ");
    print_signal_name(fp, events, i);
    if (i > ninputs) fprintf(fp, "_gC(");
    else fprintf(fp, "_env(");
    for (k = i; k < nevents; k += 2)
        for (j = 1; j < nevents; j += 2)
            if (((slash = strchr(events[j]->event, '/')) == NULL) ||
                    (strcmp(slash, "/1") == 0))
                for (k = i; k < nevents; k += 2)
                    for (l = j; l < nevents; l += 2)
                        if (((i == k) || (are_merged(merges, i, k))) &&
                                ((j == l) || (are_merged(merges, j, l))))
                            if (((rules[l][k]->ruletype != NORULE) &&
                                    (!(rules[l][k]->ruletype & REDUNDANT))) ||
                                    ((rules[l + 1][k]->ruletype != NORULE) &&
                                    (!(rules[l + 1][k]->ruletype & REDUNDANT))) ||
                                    ((rules[l][k + 1]->ruletype != NORULE) &&
                                    (!(rules[l][k + 1]->ruletype & REDUNDANT))) ||
                                    ((rules[l + 1][k + 1]->ruletype != NORULE) &&
                                    (!(rules[l + 1][k + 1]->ruletype & REDUNDANT)))) {
                                if ((j != i) && (!are_merged(merges, j, i))) {
                                    if (!first) fprintf(fp, ", ");
                                    print_signal_name(fp, events, j);
                                    first = FALSE;
                                }
                                k = nevents;
                                l = nevents;
                            }
    if (!first) fprintf(fp, ", ");
    print_signal_name(fp, events, i);
    fprintf(fp, ");\n");
    for (k = i; k < nevents; k += 2)
        for (j = 1; j < nevents; j += 2)
            if (((slash = strchr(events[j]->event, '/')) == NULL) ||
                    (strcmp(slash, "/1") == 0))
                for (k = i; k < nevents; k += 2)
                    for (l = j; l < nevents; l += 2)
                        if (((i == k) || (are_merged(merges, i, k))) &&
                                ((j == l) || (are_merged(merges, j, l))))
                            if (((rules[l][k]->ruletype != NORULE) &&
                                    (!(rules[l][k]->ruletype & REDUNDANT))) ||
                                    ((rules[l + 1][k]->ruletype != NORULE) &&
                                    (!(rules[l + 1][k]->ruletype & REDUNDANT))) ||
                                    ((rules[l][k + 1]->ruletype != NORULE) &&
                                    (!(rules[l][k + 1]->ruletype & REDUNDANT))) ||
                                    ((rules[l + 1][k + 1]->ruletype != NORULE) &&
                                    (!(rules[l + 1][k + 1]->ruletype & REDUNDANT)))) {
                                if ((j != i) && (!are_merged(merges, j, i))) {

                                    fprintf(fp, "  input ");
                                    print_signal_name(fp, events, j);
                                    fprintf(fp, ";\n");
                                }
                                k = nevents;
                                l = nevents;
                            }
    fprintf(fp, "  output ");
    print_signal_name(fp, events, i);
    fprintf(fp, ";\n");
    fprintf(fp, "  reg ");
    print_signal_name(fp, events, i);
    fprintf(fp, ";\n\n");
}

/*****************************************************************************/
/* Print an interface for a module.                                          */

/*****************************************************************************/

void
print_WC_gate_interface(FILE *fp, signalADT *signals, regionADT *regions,
        int nsignals, int ninpsigs) {

    int i;
    fprintf(fp, "module ");
    for (i = 0; i < nsignals; i++) {
        fprintf(fp, "%s_", signals[i]->name);

    }
    fprintf(fp, "net(");
    for (i = 0; i < nsignals; i++) {
        if (i < nsignals - 1)
            fprintf(fp, "%s, ", signals[i]->name);
        else
            fprintf(fp, "%s", signals[i]->name);

    }
    fprintf(fp, ");\n\n");

    //for (i = 0; i < nsignals; i++) {
    for (int j = 0; j < nsignals; j++) {
        if (j < ninpsigs)
            fprintf(fp, "  input %s;\n", signals[j]->name);

        else
            fprintf(fp, "  output %s;\n", signals[j]->name);

    }
    //}
    fprintf(fp, "\n");

}

/*****************************************************************************/
/* Print an interface for a module.                                          */

/*****************************************************************************/

void
print_gate_interface(FILE *fp, signalADT *signals, regionADT *regions,
        int nsignals, int i) {
    bool first;
    int j;
    regionADT cur_region;
    bool print;

    fprintf(fp, "module %s_gC(", signals[i]->name);
    first = TRUE;
    for (j = 0; j < nsignals; j++)
        if (j != i) {
            print = FALSE;
            for (cur_region = regions[2 * i + 1]; cur_region; cur_region = cur_region->link)
                if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
                    print = TRUE;
                    break;
                }
            if (!print)
                for (cur_region = regions[2 * i + 2]; cur_region;
                        cur_region = cur_region->link)
                    if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
                        print = TRUE;
                        break;
                    }
            if (print) {
                if (!first) fprintf(fp, ",");
                fprintf(fp, "%s", signals[j]->name);
                first = FALSE;
            }
        }
    if (!first) fprintf(fp, ",");
    fprintf(fp, "%s);\n", signals[i]->name);
    for (j = 0; j < nsignals; j++)
        if (j != i) {
            print = FALSE;
            for (cur_region = regions[2 * i + 1]; cur_region; cur_region = cur_region->link)
                if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
                    print = TRUE;
                    break;
                }
            if (!print)
                for (cur_region = regions[2 * i + 2]; cur_region;
                        cur_region = cur_region->link)
                    if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
                        print = TRUE;
                        break;
                    }

            if (print) fprintf(fp, "  input %s;\n", signals[j]->name);
        }
    fprintf(fp, "  output %s;\n", signals[i]->name);
    fprintf(fp, "  reg %s;\n\n", signals[i]->name);
}

/*****************************************************************************/
/* Print all environment modules.                                            */

/*****************************************************************************/

void
print_env_modules(FILE *fp, eventADT *events, mergeADT *merges,
        ruleADT **rules, int nevents, int ninputs,
        char * startstate) {
    int i;
    char * slash;

    for (i = 1; i < ninputs + 1; i += 2)
        if (((slash = strchr(events[i]->event, '/')) == NULL) ||
                (strcmp(slash, "/1") == 0)) {

            print_interface(fp, events, merges, rules, nevents, ninputs, i);
            print_initialization(fp, events, startstate, i);
            print_production(fp, events, merges, rules, nevents, ninputs, i);
            print_production(fp, events, merges, rules, nevents, ninputs, i + 1);
            fprintf(fp, "endmodule\n\n\n");
        }
}

/*****************************************************************************/
/* Print all gate modules.                                                   */

/*****************************************************************************/

void
print_gate_modules(FILE *fp, signalADT *signals, regionADT *regions,
        int ninpsig, int nsignals, char * startstate, int gateLevel) {

    print_WC_gate_interface(fp, signals, regions, nsignals, ninpsig);

    if (gateLevel) {
      print_WC_standard_gate_production(fp, signals, regions, nsignals, ninpsig);

    } else {

        print_WC_gate_production(fp, signals, regions, nsignals, ninpsig);
        fprintf(fp, "\n");

    }

    print_WC_gate_initialization(fp, signals, regions, nsignals, ninpsig, startstate, gateLevel);

    fprintf(fp, "endmodule\n\n\n");

}

/*****************************************************************************/
/* Print an instantiation of a module.                                       */

/*****************************************************************************/

void
print_gate_instantiation(FILE *fp, signalADT *signals, regionADT *regions,
        int ninpsig, int nsignals) {
    bool first;
    int i, j;

    regionADT cur_region;
    bool print;

    for (i = ninpsig; i < nsignals; i++) {
        fprintf(fp, "%s_gC ", signals[i]->name);
        fprintf(fp, "%sgC(", signals[i]->name);
        first = TRUE;
        for (j = 0; j < nsignals; j++)
            if (j != i) {
                print = FALSE;
                for (cur_region = regions[2 * i + 1]; cur_region; cur_region = cur_region->link)
                    if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
                        print = TRUE;
                        break;
                    }
                if (!print)
                    for (cur_region = regions[2 * i + 2]; cur_region;
                            cur_region = cur_region->link)
                        if ((cur_region->cover[j] == '0') || (cur_region->cover[j] == '1')) {
                            print = TRUE;
                            break;
                        }
                if (print) {
                    if (!first) fprintf(fp, ",");
                    fprintf(fp, "%s", signals[j]->name);
                    first = FALSE;
                }
            }

        if (!first) fprintf(fp, ",");
        fprintf(fp, "%s);\n", signals[i]->name);
    }
    fprintf(fp, "\n");
}

/*****************************************************************************/
/* Print an instantiation of a module.                                       */

/*****************************************************************************/

void
print_env_instantiation(FILE *fp, eventADT *events, mergeADT *merges,
        ruleADT **rules, int nevents, int ninputs) {
    bool first;
    int i, j, k, l;
    char * slash;

    for (i = 1; i < ninputs + 1; i += 2)
        if (((slash = strchr(events[i]->event, '/')) == NULL) ||
                (strcmp(slash, "/1") == 0)) {
            first = TRUE;
            print_signal_name(fp, events, i);
            fprintf(fp, "_env ");
            print_signal_name(fp, events, i);
            fprintf(fp, "env(");
            for (k = i; k < nevents; k += 2)
                for (j = 1; j < nevents; j += 2)
                    if (((slash = strchr(events[j]->event, '/')) == NULL) ||
                            (strcmp(slash, "/1") == 0))
                        for (k = i; k < nevents; k += 2)
                            for (l = j; l < nevents; l += 2)
                                if (((i == k) || (are_merged(merges, i, k))) &&
                                        ((j == l) || (are_merged(merges, j, l))))
                                    if (((rules[l][k]->ruletype != NORULE) &&
                                            (!(rules[l][k]->ruletype & REDUNDANT))) ||
                                            ((rules[l + 1][k]->ruletype != NORULE) &&
                                            (!(rules[l + 1][k]->ruletype & REDUNDANT))) ||
                                            ((rules[l][k + 1]->ruletype != NORULE) &&
                                            (!(rules[l][k + 1]->ruletype & REDUNDANT))) ||
                                            ((rules[l + 1][k + 1]->ruletype != NORULE) &&
                                            (!(rules[l + 1][k + 1]->ruletype & REDUNDANT)))) {
                                        if ((j != i) && (!are_merged(merges, j, i))) {
                                            if (!first) fprintf(fp, ", ");
                                            print_signal_name(fp, events, j);
                                            first = FALSE;
                                        }
                                        k = nevents;
                                        l = nevents;
                                    }

            if (!first) fprintf(fp, ", ");
            print_signal_name(fp, events, i);
            fprintf(fp, ");\n");
        }
    fprintf(fp, "\n");
}

/*****************************************************************************/
/* Print the waves output command.                                           */

/*****************************************************************************/

void
print_waves(FILE *fp, signalADT *signals, int nsignals) {
    int i;

    fprintf(fp, "initial begin\n");
    fprintf(fp, "  $gr_waves(");
    for (i = 0; i < nsignals; i++) {

        if (i != 0) fprintf(fp, ",");
        fprintf(fp, "\"%s\", %s", signals[i]->name, signals[i]->name);
    }
    fprintf(fp, ");\n");
    fprintf(fp, "end\n\n");
}

/*****************************************************************************/
/* Store a file to be used by VERILOG for simulation.                        */

/*****************************************************************************/

void
simulate(char * filename, signalADT *signals, eventADT *events,
        mergeADT *merges, ruleADT **rules, regionADT *regions,
        int ninpsig, int ninputs, int nsignals, int nevents,
        char * startstate, int atomic) {
    char outname[FILENAMESIZE];
    //char shellcommand[100];
    FILE *fp;

    strcpy(outname, filename);
    strcat(outname, ".v");
    printf("Storing simulation to:  %s\n", outname);
    fprintf(lg, "Storing simulation file to:  %s\n", outname);
    fp = Fopen(outname, "w");
    fprintf(fp, "/*\n");
    fprintf(fp, " * %s\n", filename);
    fprintf(fp, "*/\n");
    //fprintf(fp,"module %s;\n\n",filename);
    //print_gate_instantiation(fp,signals,regions,ninpsig,nsignals);
    //print_env_instantiation(fp,events,merges,rules,nevents,ninputs);
    //print_waves(fp,signals,nsignals);
    //fprintf(fp,"endmodule\n\n");

    print_gate_modules(fp, signals, regions, ninpsig, nsignals, startstate, atomic);
    //print_env_modules(fp, events, merges, rules, nevents, ninputs, startstate);

    fclose(fp);
    //    printf("Executing command:  verilog %s\n", outname);
    //fprintf(lg, "Executing command:  verilog %s\n", outname);
    //sprintf(shellcommand, "verilog %s", outname);
    //system(shellcommand);
}




