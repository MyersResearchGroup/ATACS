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
/* storeg.c                                                                  */
/*****************************************************************************/

#include "storeg.h"
#include "merges.h"


void print_dummy_node(FILE *fp,char *node)
{
  if (node[0]=='$') fprintf(fp,"d_");
  for (unsigned int j=0;j<strlen(node); j++)
    if (node[j]=='+')
      fprintf(fp,"P");
    else if (node[j]=='-')
      fprintf(fp,"M");
    else if ((node[j]!='/') && (node[j]!='$'))
      fprintf(fp,"%c",node[j]);
}

void print_cont_var(FILE *fp, string node)
{
  fprintf(fp,"%s",node.substr(0,node.find('/')).c_str());
}

/*****************************************************************************/
/* Print signal names in format used by SIS.                                 */
/*****************************************************************************/

void print_SIS_signal_names(FILE *fp,eventADT *events,mergeADT *merges,
			    int nevents,int ninputs)
{
  int i,j;
  bool print,dummy;
  int cursig=(-1);

  dummy=FALSE;
  for (i=1;i<nevents;i++) {
    if ((i==1) && (ninputs > 0))
      fprintf(fp,".inputs ");
    if (i==ninputs+1)
      fprintf(fp,"\n.outputs ");
    if ((events[i]->event[0]=='$' || events[i]->type & DUMMY) &&
	!dummy) {
      fprintf(fp,"\n.dummy ");
      dummy=TRUE;
    }
    print=TRUE;
    if (!dummy) {
      if (strchr(events[i]->event,'-') || events[i]->type & CONT)
	print=FALSE;
      if ((print) && (events[i]->signal==cursig))
	print=FALSE;
      else
	cursig=events[i]->signal;
    }
    if (print) {
      if ((dummy) && (events[i]->event[0]=='$')) fprintf(fp,"d_");
      for (j=0;(unsigned)j<strlen(events[i]->event); j++) {
	if ((!dummy) && ((events[i]->event[j]=='+') ||
			 (events[i]->event[j]=='-'))) break;
	if (events[i]->event[j]=='+')
	  fprintf(fp,"P");
	else if (events[i]->event[j]=='-')
	  fprintf(fp,"M");
	else if ((events[i]->event[j]!='/') && (events[i]->event[j]!='$'))
	  fprintf(fp,"%c",events[i]->event[j]);
      }
      fprintf(fp," ");
    }
  }
  fprintf(fp,"\n");
}

/*****************************************************************************/
/* Print signal names in format used by LHPN.                                */
/*****************************************************************************/

void print_LHPN_signal_names(FILE *fp,signalADT *signals,
			     eventADT *events,mergeADT *merges,
			     int nsignals,int nineqs,int nevents,int ninputs)
{
  int i,j;
  bool dummy = false;

  fprintf(fp,".outputs");
  for (i=0;i<nsignals-nineqs;i++)
    fprintf(fp," %s",signals[i]->name);
  fprintf(fp,"\n");
  fprintf(fp,".dummy ");
  for (i=1;i<nevents;i++) {
    if ((events[i]->event[0]=='$' || events[i]->type & DUMMY) &&
        !dummy) {
      dummy=TRUE;
    }
    if (dummy) {
      if (events[i]->event[0]=='$') fprintf(fp,"d_");
      for (j=0;(unsigned)j<strlen(events[i]->event); j++) {
        if ((!dummy) && ((events[i]->event[j]=='+') ||
                         (events[i]->event[j]=='-'))) break;
        if (events[i]->event[j]=='+')
          fprintf(fp,"P");
        else if (events[i]->event[j]=='-')
          fprintf(fp,"M");
        else if ((events[i]->event[j]!='/') && (events[i]->event[j]!='$'))
          fprintf(fp,"%c",events[i]->event[j]);
      }
      fprintf(fp," ");
    }
  }
  dummy = false;
  for (i=1;i<nevents;i++) {
    if ((events[i]->event[0]=='$' || events[i]->type & DUMMY) &&
        !dummy) {
      dummy=TRUE;
    }
    if (dummy || events[i]->type & CONT) {
      break;
    }
    for (j=0;(unsigned)j<strlen(events[i]->event); j++) {
      if (events[i]->event[j]=='+')
        fprintf(fp,"P");
      else if (events[i]->event[j]=='-')
        fprintf(fp,"M");
      else if ((events[i]->event[j]!='/') && (events[i]->event[j]!='$'))
        fprintf(fp,"%c",events[i]->event[j]);
    }
    fprintf(fp," ");
  }
  fprintf(fp,"\n");
}

void print_LHPN_variables(FILE *fp,eventADT *events,int nevents)
{

  fprintf(fp,"#@.variables");

  for (int i = 1; i < nevents; i++) {
    if (events[i]->type & VAR + CONT) {
      fprintf(fp," ");
      print_cont_var(fp,events[i]->event);
    }
    if (events[i]->data!=NULL) {
      char *chanName = CopyString(events[i]->event);
      if (strstr(events[i]->event,"rcv")!=NULL) {
	if (strchr(events[i]->event,'-')!=NULL) {
	  char *chanName = CopyString(events[i]->event);
	  *(strstr(chanName,"rcv")) = '\0';
	  fprintf(fp," %sdata",chanName);
	  if (!isdigit(events[i]->data[0]))
	    fprintf(fp," %s",events[i]->data);
	  free(chanName);
	}
      } else if (strstr(events[i]->event,"send")!=NULL) {
	if (strchr(events[i]->event,'+')!=NULL) {
	  char *chanName = CopyString(events[i]->event);
	  *(strstr(chanName,"send")) = '\0';
	  fprintf(fp," %sdata",chanName);
	  if (!isdigit(events[i]->data[0]))
	    fprintf(fp," %s",events[i]->data);
	  free(chanName);
	}
      }
    }
  }
  fprintf(fp,"\n");
}

/* Helper function used below */
int findrelop(string expr) {
  int pos;
  if (expr.find('=') != string::npos)
    pos = expr.find('=');
  else if (expr.find('>') != string::npos)
    pos = expr.find('>');
  else if (expr.find('<') != string::npos)
    pos = expr.find('<');
  else pos = -1;

  return pos;
}

void print_rule(FILE *fp,ruleADT **rules,eventADT *events,int i,int j)
{

  /* This hole chunk pulls the ineqs out of the rule expression */
  /* This piece is only called if a 'sg' is used, not an 'sl'   */
  if (rules[i][j]->expr) {
    string expr = rules[i][j]->expr;
    while (findrelop(expr) != -1) {
      int pos = findrelop(expr);
      unsigned x,y;
      for (x = pos; x >= 0; x--) {
	if (expr[x] == ']' ||
	    expr[x] == '[' ||
	    expr[x] == '|' ||
	    expr[x] == '&' ||
	    expr[x] == '(' ||
	    expr[x] == ')') {
	  break;
	}
      }
      for (y = pos+1; y < expr.size(); y++) {
	if (expr[y] == ']' ||
	    expr[y] == '[' ||
	    expr[y] == '|' ||
	    expr[y] == '&' ||
	    expr[y] == '(' ||
	    expr[y] == ')') {
	  break;
	}
      }
      expr.replace(x+1,y-x-1,"");
    }
    while (expr.find("~()") != string::npos) {
      expr.replace(expr.find("~()"),3,"");
    }
    while (expr.find("()") != string::npos) {
      expr.replace(expr.find("()"),2,"");
    }
    while (expr.find("&]") != string::npos) {
      expr.replace(expr.find("&]"),2,"]");
    }
    while (expr.find("[]") != string::npos) {
      expr.replace(expr.find("[]"),2,"");
    }
    while (expr.find("&&~(&)") != string::npos) {
      expr.replace(expr.find("&&~(&)"),6,"");
    }
    while (expr.find("&&") != string::npos) {
      expr.replace(expr.find("&&"),2,"&");
    }
    rules[i][j]->expr = CopyString(expr.c_str());
  }


  if (rules[i][j]->expr || rules[i][j]->lower!=0 ||
      rules[i][j]->upper!=INFIN ||
      (rules[i][j]->ruletype & ORDERING) ||
      rules[i][j]->rate > 0.0) {
    fprintf(fp,"#@ ");
    if (rules[i][j]->ruletype & ORDERING)
      fprintf(fp,"{C} ");
    if (rules[i][j]->expr) {
      for (unsigned int k=0;k<strlen(rules[i][j]->expr);k++)
	if (rules[i][j]->expr[k]!='[' && rules[i][j]->expr[k]!=']') {
	  if (k==0) fprintf(fp,"(");
	  fprintf(fp,"%c",rules[i][j]->expr[k]);
	  if (k==(strlen(rules[i][j]->expr)-1)) fprintf(fp,")");
	}
	else if (rules[i][j]->expr[k]=='[')
	  fprintf(fp,"(");
	else if (rules[i][j]->expr[k+1]=='d') {
	  fprintf(fp,")d");
	  break;
	}
	else
	  fprintf(fp,")");
    }
    if (rules[i][j]->lower!=0 || rules[i][j]->upper!=INFIN) {
      fprintf(fp,"[%d,",rules[i][j]->lower);
      if (rules[i][j]->upper==INFIN)
	fprintf(fp,"inf]");
      else
	fprintf(fp,"%d]",rules[i][j]->upper);
    }
    if (rules[i][j]->rate > 0.0)
      if (rules[i][j]->rate < 0.00001)
	fprintf(fp," %e ",rules[i][j]->rate);
      else
	fprintf(fp," %f ",rules[i][j]->rate);
  }
  fprintf(fp,"\n");
}

/*****************************************************************************/
/* Print initial marking in format used by SIS.                              */
/*****************************************************************************/

void print_SIS_marking(FILE *fp,eventADT *events,ruleADT **rules,int nevents,
		       int **place,bool lpn)
{
  int i,j,k,l;

  fprintf(fp,".marking {");
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if ((rules[i][j]->ruletype != NORULE) &&
	  (rules[i][j]->epsilon==1)) {
	for (k=1;k<nevents;k++) {
	  for (l=1;l<nevents;l++)
	    if ((place[i][j]==place[k][l]) &&
		(rules[k][l]->epsilon==0))
	      break;
	  if (l!=nevents) break;
	}
	if (k!=nevents) continue;
	bool print_place=false;
	for (k=1;k<nevents;k++)
	  for (l=1;l<nevents;l++)
	    if (((k!=i)||(j!=l))&&(place[i][j]==place[k][l]))
	      print_place=true;
	if (!print_place) {
	  if (lpn || events[i]->event[0]=='$') {
	    fprintf(fp,"<");
	    print_dummy_node(fp,events[i]->event);
	    fprintf(fp,",");
	  }
	  else
	    fprintf(fp,"<%s,",events[i]->event);
	  if (lpn || events[j]->event[0]=='$') {
	    print_dummy_node(fp,events[j]->event);
	    fprintf(fp,">");
	  }
	  else
	    fprintf(fp,"%s>",events[j]->event);
	}
	else {
	  bool print_place=true;
	  for (l=j+1;l<nevents;l++)
	    if (place[i][j]==place[i][l])
	      print_place=false;
	  for (k=i+1;k<nevents;k++)
	    for (l=0;l<nevents;l++)
	      if (place[i][j]==place[k][l])
		print_place=false;
	  if (print_place) fprintf(fp," p_%d ",place[i][j]);
	}
      }
  fprintf(fp,"}\n");
}

/*****************************************************************************/
/* Print new model things in format used by SIS.                             */
/* enabling conditions, assignments, rate assignments                        */
/*****************************************************************************/

void print_LHPN_model(FILE *fp,signalADT *signals,
		      eventADT *events,int nevents, int nplaces,
		      ruleADT **rules, char* initval, char* initrate, char* lpnassigns) {

  //Make sure that the things exist so you don't output an empty set
  bool enablings = false;
  bool assignments = false;
  bool rate_assignments = false;
  bool priorities = false;
  for (int i = 0; i < nevents+nplaces; i++) {
    if (events[i]->hsl) enablings = true;
    if (events[i]->inequalities) {
      for (ineqADT ineq = events[i]->inequalities; ineq != NULL;
	   ineq = ineq->next) {
	if (ineq->type <= 4) enablings = true;
	if (ineq->type == 5) assignments = true;
	if (ineq->type == 6) rate_assignments = true;
      }
    }
    if (events[i]->data != NULL) assignments = true;
  }
  if (lpnassigns != NULL) assignments = true;
  for (int i=0; i< nevents; i++)
    for (int j=0; j<nevents; j++)
      if (rules[i][j]->expr) {
	enablings = true;
	break;
      }
  for (int i = 1; i < nevents; i++) {
    if (events[i]->priorityExpr) {
      priorities = true;
      break;
    }
  }

  if (initval) {
    string startval = initval;
    for (unsigned int i = 0; i < startval.size(); i++) {
      if (initval[i] == '{') initval[i] = '[';
      if (initval[i] == '}') initval[i] = ']';
      if (initval[i] == ';') initval[i] = ',';
    }
    fprintf(fp,"#@.init_vals {%s}\n",initval);
  } else {
    int i;
    for (i=nevents;i<nevents+nplaces;i++)
      if (events[i]->type & CONT) break;
    if (i != nevents+nplaces) {
      fprintf(fp,"#@.init_vals {");
      for (int i=nevents;i<nevents+nplaces;i++)
	if (events[i]->type & CONT) {
	  if (events[i]->lower == events[i]->upper) {
	    fprintf(fp,"<%s=%d>",events[i]->event,events[i]->lower);
	  } else {
	    fprintf(fp,"<%s=[%d,%d]>",events[i]->event,events[i]->lower,
		    events[i]->upper);
	  }
	}
      fprintf(fp,"}\n");
    }
  }
  if (initrate) {
    string startrate = initrate;
    for (unsigned int i = 0; i < startrate.size(); i++) {
      if (initrate[i] == '{') initrate[i] = '[';
      if (initrate[i] == '}') initrate[i] = ']';
      if (initrate[i] == ';') initrate[i] = ',';
    }
    fprintf(fp,"#@.init_rates {%s}\n", initrate);
  } else {
    int i;
    for (i=nevents;i<nevents+nplaces;i++)
      if (events[i]->type & CONT) break;
    if (i != nevents+nplaces) {
      fprintf(fp,"#@.init_rates {");
      for (int i=nevents;i<nevents+nplaces;i++)
	if (events[i]->type & CONT)
	  if (events[i]->linitrate == events[i]->uinitrate) {
	    fprintf(fp,"<%s=%d>",events[i]->event,events[i]->linitrate);
	  } else {
	    fprintf(fp,"<%s=[%d,%d]>",events[i]->event,events[i]->linitrate,
		    events[i]->uinitrate);
	  }
      fprintf(fp,"}\n");
    }
  }

  if (enablings) {
    fprintf(fp,"#@.enablings {");
    for (int i = 0; i < nevents; i++) {
      for (int j = 0; j < nevents; j++) {
	if (rules[i][j]->expr && i != 0) {
	  string expr = rules[i][j]->expr;
	  while (expr.find("~()") != string::npos) {
	    expr.replace(expr.find("~()"),3,"");
	  }
	  while (expr.find("()") != string::npos) {
	    expr.replace(expr.find("()"),2,"");
	  }
	  while (expr.find("&]") != string::npos) {
	    expr.replace(expr.find("&]"),2,"]");
	  }
	  while (expr.find("[]") != string::npos) {
	    expr.replace(expr.find("[]"),2,"");
	  }
	  while (expr.find("&&~(&)") != string::npos) {
	    expr.replace(expr.find("&&~(&)"),6,"");
	  }
	  while (expr.find("&&") != string::npos) {
	    expr.replace(expr.find("&&"),2,"&");
	  }
	  while (expr.find("]d") != string::npos) {
	    expr.replace(expr.find("]d"),2,"]");
	  }
	  if (expr.size() > 0) {
	    fprintf(fp,"<");
	    print_dummy_node(fp,events[j]->event);

	    fprintf(fp,"=%s>",expr.c_str());
	  }
	}
      }
    }
    for (int i = 0; i < nevents; i++) {
      if (events[i]->hsl) {
	fprintf(fp,"<");
	print_dummy_node(fp,events[i]->event);
	fprintf(fp,"=[%s]>",events[i]->hsl);
      }
    }
    fprintf(fp,"}\n");
  }

  if (assignments) {
    fprintf(fp,"#@.assignments {");
    // ZHEN(Done): print the assignment line here, if it is not NULL
    if (lpnassigns) {
      fprintf(fp,"%s",lpnassigns);
    }
    for (int i = 0; i < nevents+nplaces; i++) {
      if (events[i]->inequalities && events[i]->type != KEEP) {
        for (ineqADT ineq = events[i]->inequalities; ineq != NULL;
             ineq = ineq->next) {
          if (ineq->type == 5 && events[i]->type != KEEP) {
            fprintf(fp,"<");
            print_dummy_node(fp,events[i]->event);
            fprintf(fp,"=[");
            print_cont_var(fp,events[ineq->place]->event);
            fprintf(fp,":=");
	    if (ineq->expr) {
	      fprintf(fp,"%s]>",ineq->expr);
	    } else {
	      fprintf(fp,"%i]>",ineq->constant);
	    }
          }
        }
      }
    }
    for (int i = 0; i < nevents+nplaces; i++) {
      if (events[i]->data != NULL && strcmp(events[i]->data,"")!=0) {
	if (strstr(events[i]->event,"rcv")!=NULL) {
	  if (strchr(events[i]->event,'-')!=NULL) {
	    char *chanName = CopyString(events[i]->event);
	    *(strstr(chanName,"rcv")) = '\0';
	    fprintf(fp,"<");
	    print_dummy_node(fp,events[i]->event);
	    fprintf(fp,"=[%s:=",events[i]->data);
	    fprintf(fp,"%sdata]>",chanName);
	    free(chanName);
	  }
	} else if (strstr(events[i]->event,"send")!=NULL) {
	  if (strchr(events[i]->event,'+')!=NULL) {
	    char *chanName = CopyString(events[i]->event);
	    *(strstr(chanName,"send")) = '\0';
	    fprintf(fp,"<");
	    print_dummy_node(fp,events[i]->event);
	    fprintf(fp,"=[%sdata:=",chanName);
	    fprintf(fp,"%s]>",events[i]->data);
	    free(chanName);
	  }
	}
      }
    }
    fprintf(fp,"}\n");
  }

  if (rate_assignments) {
    fprintf(fp,"#@.rate_assignments {");
    for (int i = 0; i < nevents+nplaces; i++) {
      if (events[i]->inequalities) {
        for (ineqADT ineq = events[i]->inequalities; ineq != NULL;
             ineq = ineq->next) {
          if (ineq->type == 6 && events[i]->type != KEEP) {
            fprintf(fp,"<");
            print_dummy_node(fp,events[i]->event);
            fprintf(fp,"=[");
            print_cont_var(fp,events[ineq->place]->event);
            fprintf(fp,":=");
	    if (ineq->expr) {
	      fprintf(fp,"%s]>",ineq->expr);
	    } else {
	      if (ineq->constant == ineq->uconstant)
		fprintf(fp,"%i]>",ineq->constant);
	      else
		fprintf(fp,"[%i,%i]]>",ineq->constant,ineq->uconstant);
	    }
          }
        }
      }
    }
    fprintf(fp,"}\n");
  }

  fprintf(fp,"#@.delay_assignments {");
  for (int i = 1; i < nevents; i++) {
    fprintf(fp,"<");
    print_dummy_node(fp,events[i]->event);
    if (events[i]->delayExpr) {
      fprintf(fp,"=[%s]",events[i]->delayExpr);
    } else {
      fprintf(fp,"=[uniform(%d,",events[i]->lower);
      if (events[i]->upper==INFIN)
	fprintf(fp,"inf)]");
      else
	fprintf(fp,"%d)]",events[i]->upper);
    }
    fprintf(fp,">");
  }
  fprintf(fp,"}\n");

  if (priorities) {
    fprintf(fp,"#@.priority_assignments {");
    for (int i = 1; i < nevents; i++) {
      if (events[i]->priorityExpr) {
	fprintf(fp,"<");
	print_dummy_node(fp,events[i]->event);
	fprintf(fp,"=[%s]>",events[i]->priorityExpr);
      }
    }
    fprintf(fp,"}\n");
  }

  fprintf(fp,"#@.boolean_assignments {");
  for (int i = 0; i < nevents+nplaces; i++) {
    char *node = events[i]->event;
    if (node[0] != '$') {
      string boolName = node;
      int type = 0; //0 normal, 1 set high, 2 set low
      for (unsigned int j=0; j < strlen(node); j++) {
	if (node[j]=='+') {
	  type = 1;
	  boolName = boolName.substr(0,j);
	  break;
	}
	else if (node[j]=='-') {
	  type = 2;
	  boolName = boolName.substr(0,j);
	  break;
	}
      }
      if (type != 0) {
	fprintf(fp,"<");
	print_dummy_node(fp,events[i]->event);
	fprintf(fp,"=[%s:=",boolName.c_str());
	if (type == 1)
	  fprintf(fp,"TRUE]");
	else
	  fprintf(fp,"FALSE]");
	fprintf(fp,">");
      }
    }
    if (events[i]->inequalities) {
      for (ineqADT ineq = events[i]->inequalities; ineq != NULL;
	   ineq = ineq->next) {
	if (ineq->type == 7 && events[i]->type != KEEP) {
	  fprintf(fp,"<");
	  print_dummy_node(fp,events[i]->event);
	  fprintf(fp,"=[%s:=",signals[ineq->place]->name);
	  if (ineq->expr) {
	    fprintf(fp,"%s]>",ineq->expr);
	  } else {
	    if (ineq->constant) {
	      fprintf(fp,"TRUE]>");
	    } else {
	      fprintf(fp,"FALSE]>");
	    }
	  }
	}
      }
    }
  }
  fprintf(fp,"}\n");

  fprintf(fp,"#@.continuous");
  for (int i = 0; i < nevents+nplaces; i++) {
    if (events[i]->type & CONT) {
      fprintf(fp," ");
      print_cont_var(fp,events[i]->event);
    }
  }
  fprintf(fp,"\n");

  int numNonDis=0;
  for (int i = 1; i < nevents; i++)
    if (events[i]->nondisabling) numNonDis++;
  if (numNonDis > 0) {
    fprintf(fp,"#@.non_disabling");
    for (int i = 1; i < nevents; i++) {
      if (events[i]->nondisabling) {
	fprintf(fp," ");
	print_dummy_node(fp,events[i]->event);
      }
    }
    fprintf(fp,"\n");
  }
}


/*****************************************************************************/
/* Print signal transition graph in format used by SIS.                      */
/*****************************************************************************/

void print_SIS_graph(FILE *fp,signalADT *signals,
		     eventADT *events,ruleADT **rules,int nevents,
		     int nplaces, char* initval, char* initrate, bool lpn)
{
  int i,j,k,l;
  int **place;
  int nextp,p;
  int eps;
  bool shared;
  bool print;


  fprintf(fp,".graph\n");
  place=(int **)GetBlock(nevents * sizeof(int*));
  for (i=0;i<nevents;i++) {
    place[i]=(int *)GetBlock(nevents * sizeof(int));
    for (j=0;j<nevents;j++)
      place[i][j]=0;
  }
  nextp=0;
  for (i=1;i<nevents;i++)
    for (j=1;j<nevents;j++)
      if (rules[i][j]->ruletype != NORULE) {
	p=0;
	eps=rules[i][j]->epsilon;
	shared=FALSE;
	for (k=1;k<nevents;k++)
	  if ((rules[i][k]->conflict) &&
	      (rules[k][j]->ruletype != NORULE)) {
	    if ((p != 0) && (p != place[k][j])) {
	      /*
	      print=TRUE;
	      for (l=1;l<nevents;l++)
		if (place[i][l]==p) print=FALSE;
	      if (print)
		if (events[i]->event[0]=='$') {
		  print_dummy_node(fp,events[i]->event);
		  fprintf(fp," p_%d\n",p);
		} else
		  fprintf(fp,"%s p_%d\n",events[i]->event,p);
	      */
	      print=TRUE;
	      for (l=1;l<nevents;l++)
		if (place[l][j]==p) print=FALSE;
	      if (print) {
		if (events[j]->event[0]=='$') {
		  fprintf(fp,"p_%d ",p);
		  print_dummy_node(fp,events[j]->event);
		} else
		  fprintf(fp,"p_%d %s",p,events[j]->event);
		if (!lpn) print_rule(fp,rules,events,i,j);
		else fprintf(fp,"\n");
	      }
	    }
	    if (place[k][j]!=0) p=place[k][j];
	    shared=TRUE;
	    if (rules[k][j]->epsilon==0) eps=0;
	  }
	if (p==0)
	  for (k=1;k<nevents;k++)
	    if ((rules[k][j]->conflict) &&
		(rules[i][k]->ruletype != NORULE) /*&&
		    (rules[i][k]->epsilon==eps)*/) {
	      if ((p != 0) && (p != place[i][k])) {
		print=TRUE;
		for (l=1;l<nevents;l++)
		  if (place[i][l]==p) print=FALSE;
		if (print)
		  if (events[i]->event[0]=='$') {
		    print_dummy_node(fp,events[i]->event);
		    fprintf(fp," p_%d\n",p);
		  } else
		    fprintf(fp,"%s p_%d\n",events[i]->event,p);
		/*
		print=TRUE;
		for (l=1;l<nevents;l++)
		  if (place[l][j]==p) print=FALSE;
		if (print) {
		  if (events[j]->event[0]=='$') {
		    fprintf(fp,"p_%d ",p);
		    print_dummy_node(fp,events[j]->event);
		  } else
		    fprintf(fp,"p_%d %s",p,events[j]->event);
		  print_rule(fp,rules,i,j);
		  }*/
	      }
	      if (place[i][k]!=0) p=place[i][k];
	      shared=TRUE;
	      if (rules[i][k]->epsilon==0) eps=0;
	    }
	if (p==0) p=++nextp;
	if (!shared) {
	  if (!lpn) {
	    if (events[i]->event[0]=='$') {
	      print_dummy_node(fp,events[i]->event);
	      fprintf(fp," ");
	    }
	    else
	      fprintf(fp,"%s ",events[i]->event);

	    if (events[j]->event[0]=='$')
	      print_dummy_node(fp,events[j]->event);
	    else
	      fprintf(fp,"%s",events[j]->event);
	    if (!lpn) print_rule(fp,rules,events,i,j);
	    else fprintf(fp,"\n");
	  }
	  else {
	    print_dummy_node(fp,events[i]->event);
	    fprintf(fp," ");
	    print_dummy_node(fp,events[j]->event);
	    fprintf(fp,"\n");
	  }

	} else {
	  print=TRUE;
	  for (k=1;k<nevents;k++)
	    if (place[i][k]==p) print=FALSE;
	  if (print) {
	    if (!lpn) {
	      if (events[i]->event[0]=='$') {
		print_dummy_node(fp,events[i]->event);
		fprintf(fp," p_%d\n",p);
	      } else {
		fprintf(fp,"%s p_%d\n",events[i]->event,p);
	      }
	    }
	    else {
	      print_dummy_node(fp,events[i]->event);
	      fprintf(fp," p_%d\n",p);
	    }
	  }
	  print=TRUE;
	  for (k=1;k<nevents;k++)
	    if (place[k][j]==p) print=FALSE;
	  if (print) {
	    if (!lpn ) {
	      if (events[j]->event[0]=='$') {
		fprintf(fp,"p_%d ",p);
		print_dummy_node(fp,events[j]->event);
	      } else
		fprintf(fp,"p_%d %s",p,events[j]->event);
	      print_rule(fp,rules,events,i,j);

	    }
	    else {
	      fprintf(fp,"p_%d ",p);
	      print_dummy_node(fp,events[j]->event);
	      fprintf(fp,"\n");
	    }
	  }
	}
	place[i][j]=p;
      }
  print_SIS_marking(fp,events,rules,nevents,place,lpn);
  for (i=1;i<nevents;i++)
    free(place[i]);
  free(place);
}

void print_LHPN_graph(FILE *fp,signalADT *signals,
		     eventADT *events,ruleADT **rules,int nevents,
		      int nplaces, char* initval, char* initrate, char* lpnassigns)
{
  if (nplaces==0) {
    print_SIS_graph(fp,signals,events,rules,nevents,nplaces,initval,
		    initrate,true);
  } else {
    fprintf(fp,".graph\n");
    for (int i=1;i<nevents+nplaces;i++)
      for (int j=1;j<nevents+nplaces;j++)
	if (rules[i][j]->ruletype != NORULE) {
	  fprintf(fp,"%s %s\n",events[i]->event,events[j]->event);
	}
    fprintf(fp,".marking {");
    for (int i=1;i<nevents+nplaces;i++)
      for (int j=1;j<nevents;j++)
	if ((rules[i][j]->ruletype != NORULE) &&
	    (rules[i][j]->epsilon==1)) {
	  if (i < nevents) {
	    fprintf(fp,"<%s,%s>",events[i]->event,events[j]->event);
	  } else {
	    fprintf(fp,"%s ",events[i]->event);
	    break;
	  }
	}
    fprintf(fp,"}\n");
  }
  print_LHPN_model(fp,signals,events,nevents,nplaces,rules,initval,
		   initrate, lpnassigns);
}

/*****************************************************************************/
/* Store a graph to be used by LHPNs.                                        */
/*****************************************************************************/

void store_lhpn(char * filename,signalADT *signals,
		eventADT *events,mergeADT *merges,ruleADT **rules,
		int nevents,int nplaces,int ninputs,char * startstate,
		char* initval, char* initrate, bool level,
		bool fromG,bool fromLPN,int nsignals,int nineqs,char *lpnassigns)
{
  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,filename);
  if (fromLPN)
    strcat(outname,"_NEW.lpn");
  else
    strcat(outname,".lpn");
  printf("Storing LHPN file to:  %s\n",outname);
  fprintf(lg,"Storing LHPN file to:  %s\n",outname);
  fp=fopen(outname,"w");
  if (fp==NULL) {
    printf("ERROR: Could not open file:  %s\n",outname);
    fprintf(lg,"ERROR: Could not open file:  %s\n",outname);
    return;
  }
  print_LHPN_signal_names(fp,signals,events,merges,nsignals,nineqs,nevents,
			  ninputs);
  print_LHPN_variables(fp,events,nevents+nplaces);
  if (startstate && strlen(startstate) > 0) {
    fprintf(fp,"#@.init_state [",startstate);
    for (int i=0;i<(nsignals-nineqs);i++) {
      fprintf(fp,"%c",startstate[i]);
    }
    fprintf(fp,"]\n");
  } else
    fprintf(fp,"#@.init_state [0]\n");
  print_LHPN_graph(fp,signals,events,rules,nevents,nplaces,initval,initrate,lpnassigns);
  fprintf(fp,".end\n");
  fclose(fp);
}

/*****************************************************************************/
/* Store a graph to be used by SIS.                                          */
/*****************************************************************************/

void store_g(char * filename,signalADT *signals,
	     eventADT *events,mergeADT *merges,ruleADT **rules,
	     int nevents,int nplaces,int ninputs,char * startstate,
	     char* initval, char* initrate, bool level,
	     bool fromG,bool fromLPN)
{
  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,filename);
  if (fromG || fromLPN)
    strcat(outname,"_NEW.g");
  else
    strcat(outname,".g");
  printf("Storing graph file to:  %s\n",outname);
  fprintf(lg,"Storing graph file to:  %s\n",outname);
  fp=fopen(outname,"w");
  if (fp==NULL) {
    printf("ERROR: Could not open file:  %s\n",outname);
    fprintf(lg,"ERROR: Could not open file:  %s\n",outname);
    return;
  }
  //fprintf(fp,".name %s\n",filename);
  print_SIS_signal_names(fp,events,merges,nevents,ninputs);
  if (startstate && strlen(startstate) > 0)
    fprintf(fp,"#@.init_state [%s]\n",startstate);
  else
    fprintf(fp,"#@.init_state [0]\n");
  print_SIS_graph(fp,signals,events,rules,nevents,nplaces,initval,initrate,
		  false);
  fprintf(fp,".end\n");
  fclose(fp);
}
