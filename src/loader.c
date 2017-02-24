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
/*****************************************************************************/
/* loader.c                                                                  */
/*   Procedures to load a timed event-rule structure from a file in this     */
/*   format:                                                                 */
/*   .e number of events                                                     */
/*   .i number of inputs                                                     */
/*   .r number of conjunctive causal rules                                   */
/*   .o number of disjuctive causal rules                                    */
/*   .n number of ordering rules                                             */
/*   .d number of merges                                                     */
/*   .c number of conflicts                                                  */
/*   reset                                                                   */
/*   input events                                                            */
/*   ...                                                                     */
/*   other events                                                            */
/*   ...                                                                     */
/*   conjunctive causal rules in the following form:                         */
/*   <enabling event> <enabled event> <epsilon> <lower bound> <upper bound>  */
/*   ...                                                                     */
/*   disjuctive causal rules in the following form:                          */
/*   <enabling event> <enabled event> <epsilon> <lower bound> <upper bound>  */
/*   ...                                                                     */
/*   ordering rules in the following form:                                   */
/*   <enabling event> <enabled event> <epsilon> <lower bound> <upper bound>  */
/*   ...                                                                     */
/*   merges in the following form:                                           */
/*   if a disjunctive merge:  <signal+/1> <signal+/2> etc ...                */
/*   if a conjunctive merge:  <signal-/1> <signal-/2> etc ...                */
/*   conflicts in the following form:                                        */
/*   <event> <event> etc ...                                                 */
/*****************************************************************************/
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>

#include "loader.h"
#include "connect.h"
#include "def.h"
#include "misclib.h"
#include "memaloc.h"
#include "loadsis.h"
#include "findcl.h"
#include "merges.h"
#include "CPdf.h"
#include "CNormal.h"
#include "CUniform.h"
#include "CUdef.h"
#include "CSingular.h"
#include "ly.h"

extern set<string> top_port;
extern map<string,string> top_port_map;

/*****************************************************************************/
/* Get a token from a file.  Used for loading timed event-rule structures.   */
/*****************************************************************************/

int fgettok(FILE *fp,char *tokvalue,int maxtok,int *linenum)
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '[':
    case '(':
    case '~':
    case '&':
    case ')':
    case '|':
    case ']':
    case ',':
      if (!readword) return(c);
      else {
	ungetc(c,fp);
	return(WORD);
      }
      break;
    case '\n':
      if (!readword) {
	(*linenum)++;
	return(END_OF_LINE);
      } else {
	ungetc('\n',fp);
	return (WORD);
      }
      break;
    case '#':
      if (!readword) {
	while (((c=getc(fp)) != EOF) && (c != '\n'));
	(*linenum)++;
	return(COMMENT);
      } else {
	ungetc('#',fp);
	return (WORD);
      }
      break;
    case '.':
      if (!readword) {
	if ((c=getc(fp)) != EOF) {
	  *tokvalue++=c;
	  *tokvalue='\0';
	  return(DOT);
	} else return(END_OF_FILE);
      } else {
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
	/*	ungetc('.',fp);
	return (WORD);*/
      }
      break;
    case ' ':
      if (readword)
	return (WORD);
      break;
    case '@':
      if (toklen < maxtok) {
	readword=TRUE;
	*tokvalue++='_';
	*tokvalue='\0';
	toklen++;
      }
      break;
    default:
      if (toklen < maxtok) {
	readword=TRUE;
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
      }
      break;
    }
  }
  if (!readword)
    return(END_OF_FILE);
  else
    return(WORD);
}

/*****************************************************************************/
/* Find event in event list and return its position.                         */
/*****************************************************************************/

int find_event(int nevents,eventADT *events,char * event,int linenum)
{
  char *data("");
  char *endit(strchr(event,':'));//pointer to the first ':' in event occurence
  if (endit) {
    data = endit+1;
    (*endit) = '\0';
  }
  for (int a(0);  a<nevents;  a++){
    if (strcmp(events[a]->event,event)==0){
      events[a]->data = CopyString(data);
      return(a);
    }
  }
  cout << endl;
  printf("ERROR:%d:  %s is an undeclared event!\n",linenum,event);
  fprintf(lg,"ERROR:%d:  %s is an undeclared event!\n",linenum,event);
  return(-1);
}

/*****************************************************************************/
/* Load header info.                                                         */
/*****************************************************************************/

char * load_header(FILE *fp,int *nevents,int *ninputs,int *nrules,int *ndisj,
		   int *nconf,int *niconf,int *noconf,int *nord,
		   char * *startstate,char * *initval,char * *initrate,
		   int *linenum)
{
  int value=0;
  int token;
  char * dot;
  char dotvalue[MAXTOKEN];

  *nevents=0; *ninputs=0; *nrules=0; *ndisj=0; *nconf=0; *niconf=0; *noconf=0;
  *nord=0; (*startstate)=NULL;
  dot=(char *)GetBlock(MAXTOKEN*sizeof(char));
  while ((token=fgettok(fp,dot,MAXTOKEN,linenum)) != WORD) {
    if (token==END_OF_FILE) {
      printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
      return(NULL);
    }
    if (token==DOT) {
      token=fgettok(fp,dotvalue,MAXTOKEN,linenum);
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(NULL);
      }
      if (dot[0] != 's') value=atoi(dotvalue);
      if (token==WORD)
	switch (dot[0]) {
	case 'e':
	  *nevents=value;
	  break;
	case 'i':
	  *ninputs=value;
	  break;
	case 'r':
	  *nrules=value;
	  break;
	case 'd':
	  *ndisj=value;
	  break;
	case 'c':
	  *nconf=value;
	  break;
	case 'm':
	  *niconf=value;
	  break;
	case 'o':
	  *noconf=value;
	  break;
	case 'n':
	  *nord=value;
	  break;
	case 's':
	  *startstate=CopyString(dotvalue);
	  break;
	case 'q':
	  *initval=CopyString(dotvalue);
	  break;
	case 'v':
	  *initrate=CopyString(dotvalue);
	  break;
	default:
	  printf("ERROR:%d:  Bad header information!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Bad header information!\n",*linenum);
	  return(NULL);
	}
      else {
	printf("ERROR:%d:  Bad header information!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Bad header information!\n",*linenum);
	return(NULL);
      }
    }
  }
  if (*nevents==0) {
    printf("ERROR:%d:  No events in system!\n",*linenum);
    fprintf(lg,"ERROR:%d:  No events in system!\n",*linenum);
    return(NULL);
  }
  if (*nrules==0) {
    printf("ERROR:%d:  No rules in system!\n",*linenum);
    fprintf(lg,"ERROR:%d:  No rules in system!\n",*linenum);
    return(NULL);
  }
  return(dot);
}

/*****************************************************************************/
/* Load event list.                                                          */
/*****************************************************************************/

bool load_events(FILE *fp,int nevents,eventADT *events,char * first,
		 int *nsignals,int *ninpsig,signalADT *signals,int ninputs,
		 int *ndummy,int *linenum)
{
  int i,j,len,token;
  char event[MAXTOKEN];
  char signal[MAXTOKEN];
  char * endit;

  (*nsignals)=0;
  (*ninpsig)=0;
  (*ndummy)=0;
  strcpy(event,first);
  for (i=0;i<nevents;i++) {
    while (((i !=0) && (token=fgettok(fp,event,MAXTOKEN,linenum)) != WORD))
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE);
      }
    len=strlen(event)+1;
    events[i]->event=(char *)GetBlock(len * sizeof(char));
    strcpy(events[i]->event,event);
    endit=strchr(events[i]->event,':');
    events[i]->type = 0;
    if (endit) {
      events[i]->type = atoi(endit+1);
      (*endit) = '\0';
    }
    if((strlen(event) < 1) || ((event[0]!='$')&&(!(events[i]->type & DUMMY)))){
      events[i]->type = (events[i]->type | KEEP);
      if (i>0) {
	strcpy(signal,event);
	endit=strchr(signal,'+');
	if (endit) (*endit)='\0';
	else {
	  endit=strchr(signal,'-');
	  if (endit) (*endit)='\0';
	}
	for (j=0;j<(*nsignals);j++)
	  if (strcmp(signals[j]->name,signal)==0) {
	    if (i & 2==1) signals[j]->maxoccurrence++;
	    break;
	  }
	events[i]->signal=j;
	if (j==(*nsignals)) {
	  signals[*nsignals]->name=CopyString(signal);
	  signals[*nsignals]->maxoccurrence=1;
	  signals[*nsignals]->riselower=0;
	  signals[*nsignals]->riseupper=INFIN;
	  signals[*nsignals]->falllower=0;
	  signals[*nsignals]->fallupper=INFIN;
	  signals[*nsignals]->event=i;
	  (*nsignals)++;
	  if (i < ninputs) (*ninpsig)++;
	}
      }
    }
    else{
      if (strncmp(events[i]->event,"$XL_",4)==0)
	events[i]->immediate=TRUE;
      events[i]->type = events[i]->type | DUMMY;
      events[i]->signal=(-1);
      (*ndummy)++;
    }
  }
  return(TRUE);
}

/*****************************************************************************/
/* Initialize rule array.                                                    */
/*****************************************************************************/

void init_rules(int nevents,ruleADT **rules)
{
int a,b;

for (a=0;a<nevents;a++)
  for (b=0;b<nevents;b++) {
    rules[a][b]->epsilon=(-1);
    rules[a][b]->lower=0;
    rules[a][b]->upper=INFIN;
    rules[a][b]->plower=(-1);
    rules[a][b]->pupper=(-1);
    rules[a][b]->weight=(-1);
    rules[a][b]->ruletype=NORULE;
    rules[a][b]->conflict=FALSE;
    rules[a][b]->marking=(-1);
    rules[a][b]->level=NULL;
    rules[a][b]->expr=NULL;
    rules[a][b]->dist=NULL;
    rules[a][b]->maximum_sep=-(INFIN);
    rules[a][b]->concur=FALSE;
  }
}

/*****************************************************************************/
/* Get a field of a rule.                                                    */
/*****************************************************************************/

bool get_field(FILE *fp,char * tokvalue,int *token,int *linenum)
{
  while (((*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != WORD) {
    if ((*token)=='[') return(TRUE);
    if ((*token)=='(') return(TRUE);
    if ((*token)==',') return(TRUE);
    if ((*token)==')') return(TRUE);
    if ((*token)==END_OF_FILE) {
      printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
      return(FALSE);
    }
    if ((*token)==END_OF_LINE) {
      printf("ERROR:%d:  Unexpected end of line!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Unexpected end of line!\n",*linenum);
      return(FALSE);
    }
  }
  return(TRUE);
}

/*****************************************************************************/
/* List Connectivity Warnings.                                               */
/*****************************************************************************/

void print_warnings(int nevents,eventADT *events,char * connected,
		    bool verbose,bool display)
{
  int a,count;

  if (display)
    for (a=1;a<nevents;a++) {
      if ((!verbose) && ((connected[a]=='F') || (connected[a]=='O') ||
			 (connected[a]=='I'))) {
	printf("WARNING:  Graph not connected!\n");
	fprintf(lg,"WARNING:  Graph not connected!\n");
	break;
      }
      if (connected[a]=='F') {
	printf("WARNING:  No arcs into or out of %s.\n",events[a]->event);
	fprintf(lg,"WARNING:  No arcs into or out of %s.\n",events[a]->event);
      }
      if (connected[a]=='O') {
	printf("WARNING:  No arcs into %s.\n",events[a]->event);
	fprintf(lg,"WARNING:  No arcs into %s.\n",events[a]->event);
      }
      if (connected[a]=='I') {
	printf("WARNING:  No arcs out of %s.\n",events[a]->event);
	fprintf(lg,"WARNING:  No arcs out of %s.\n",events[a]->event);
      }
    }
  count=0;
  for (a=1;a<nevents;a++)
    if (connected[a]=='F') {
      count++;
      events[a]->dropped=TRUE;
    }
  if (count>0) {
    /*
    printf("WARNING:  Dropped %d unconnected nodes.\n",count);
    fprintf(lg,"WARNING:  Dropped %d unconnected nodes.\n",count);
    */
  }
}

bool S(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,
       int nsignals,int nevents,eventADT *events,int enabled,bool second);

bool R(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,
       int nsignals,int nevents,eventADT *events,int enabled,bool second);


int parseExpr(string token,int *linenum,int nevents,eventADT *events,
	      bool negate, int enabled, bool isSignal,signalADT *signals,
	      int nsignals) {

  ineqADT expr = new ineq_tag;
  expr->expr = NULL;
  string relop;
  if (!isSignal) {
    if (token.find("'dot:=") != string::npos) {
      relop = "'dot:=";
      expr->type = 6;
    }
    else if (token.find(":=") != string::npos) {
      relop = ":=";
      expr->type = 5;
    }
    else if (token.find(">=") != string::npos) {
      if (negate) expr->type = 2;
      else expr->type = 1;
      relop = ">=";
    }
    else if (token.find("<=") != string::npos) {
      if (negate) expr->type = 0;
      else expr->type = 3;
      relop = "<=";
    }
    else if (token.find("<") != string::npos) {
      if (negate) expr->type = 1;
      else expr->type = 2;
      relop = "<";
    }
    else if (token.find(">") != string::npos) {
      if (negate) expr->type = 3;
      else expr->type = 0;
      relop = ">";
    }
    else if (token.find("=") != string::npos) {
      relop = "=";
      if (negate) expr->type = 0;
      else expr->type = 4;
    }
    else {
      expr->type = -1;
      relop = "";
      cerr << token << ":WARNING: Improper symbol\n";
    }
  }
  else {
    expr->type = 4;
  }

  string tokenString;
  if (isSignal) {
    if (negate)
      tokenString = token + '-' + "/1";
    else
      tokenString = token + '+' + "/1";
  }
  else {
    if (relop != ":=") {
      tokenString = token.substr(0,token.find(relop)) + "/1";
    } else if (token.find("TRUE") != string::npos) {
      tokenString = token.substr(0,token.find(relop));
      expr->type = 7;
    } else if (token.find("FALSE") != string::npos) {
      tokenString = token.substr(0,token.find(relop));
      expr->type = 7;
    } else {
      tokenString = token.substr(0,token.find(relop)) + "/1";
    }
  }
  char tok[MAXTOKEN];
  strcpy(tok,tokenString.c_str());

  expr->transition = enabled;
  if (expr->type==7) {
    int i;
    for (i=0;i<nsignals;i++)
      if (strcmp(signals[i]->name,tok)==0) break;
    if (i < nsignals)
      expr->place = i;
    else {
      printf("ERROR:%d:  %s is not defined!\n",*linenum,tok);
      fprintf(lg,"ERROR:%d:  %s is not defined!\n",*linenum,tok);
    }
  } else {
    expr->place = find_event(nevents,events,tok,*linenum);
  }

  if (expr->type==7) {
    if (token.find("TRUE") != string::npos) {
      expr->constant = 1;
      expr->uconstant = 0;
    } else {
      expr->constant = 0;
      expr->uconstant = 1;
    }
  } else if (!isSignal) {
    if (token.find("{") != string::npos) {
      expr->constant =
	atoi(token.substr(token.find("{")+1,token.find(";")).c_str());
      expr->uconstant =
	atoi(token.substr(token.find(";")+1,token.find("}")).c_str());
    }
    else {
      string integer = token.substr(token.find(relop)+relop.size(),
				    string::npos);
      expr->constant = atoi(integer.c_str());
      expr->uconstant = expr->constant;
    }
  }
  else {
    if (negate) {
      expr->constant = 0;
      expr->uconstant = 1;
    }
    else {
      expr->constant = 1;
      expr->uconstant = 0;
    }
  }

  //check to see if it's already in the list
  bool add = true;
  for (ineqADT test = events[expr->transition]->inequalities; test;
       test = test->next) {
    if (expr->place == test->place &&
        expr->type == test->type &&
        expr->constant == test->constant)
      add = false;
  }

  //make the negate of = both > and <
  if (relop == "=" && negate && add & !isSignal) {
    expr->next = events[expr->transition]->inequalities;
    events[expr->transition]->inequalities = expr;
    ineqADT expr1 = new ineq_tag;
    expr->expr = NULL;
    expr->type = 2;
    expr1->transition = enabled;
    expr1->place = find_event(nevents,events,tok,*linenum);
    expr1->next = events[expr1->transition]->inequalities;
    events[expr1->transition]->inequalities = expr1;
  }
  else if (add) {
    expr->next = events[expr->transition]->inequalities;
    events[expr->transition]->inequalities = expr;
  }
  return expr->type;
}

bool U(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,int nsignals,
       int nevents,eventADT *events,int enabled,bool second)
{
  level_exp newlevel=NULL;
  int j;
  bool negate = false;
  string toParse;
  string expression;
  bool issignal = true;
  if (expr[strlen(expr)-2] == '~') negate = true;
  switch (*token) {
  case WORD:

    strcat(expr,tokvalue);

    if (strcmp(tokvalue,"false")==0) {
      (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      return TRUE;
    }
    newlevel=(level_exp)GetBlock(sizeof(*newlevel));
    newlevel->next=(*level);
    (*level)=newlevel;
    newlevel->product=(char *)GetBlock((nsignals+1) * sizeof(char));
    for (j=0;j<nsignals;j++)
      newlevel->product[j]='X';
    newlevel->product[nsignals]='\0';
    if (strcmp(tokvalue,"true")==0) {
      (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      return TRUE;
    }
    if (strncmp(tokvalue,"maybe",5)==0) {
      for (j=0;j<nsignals;j++)
	newlevel->product[j]='-';
      newlevel->product[nsignals]='\0';
      (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      return TRUE;
    }
    for (j=0;j<nsignals;j++)
      if (strcmp(signals[j]->name,tokvalue)==0) {
	newlevel->product[j]='1';
	break;
      }

    if (j==nsignals) {
      issignal = false;
      toParse = tokvalue;
      expression = expr;
      expression = expression.substr(0,expression.find(toParse));
      int type =
	parseExpr(toParse,linenum,nevents,events,negate,enabled,issignal,
		  signals,nsignals);
      //uncomment the next part when there is a separate hsl piece
      if (type > 4) {
	strcpy(expr,expression.c_str());
	//}
      }
    }
    //printf("WARNING:%d: %s is not a signal!\n",*linenum,tokvalue);
    //fprintf(lg,"WARNING:%d: %s is not a signal!\n",*linenum,
    //     tokvalue);
    //return FALSE;
    //}
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    break;
  case '(':
    strcat(expr,"(");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!R(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    if ((*token) != ')') {
      printf("ERROR:%d:  Expected a )!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Expected a )!\n",*linenum);
      return FALSE;
    }
    strcat(expr,")");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    break;
  default:
    printf("ERROR:%d:  Expected a signal or a (!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal or a (!\n",*linenum);
    return FALSE;
  }
  /*
  printf("U:");
  for (level_exp curlevel=(*level);curlevel;curlevel=curlevel->next)
    printf("%s ",curlevel->product);
  printf("\n");
  */
  return TRUE;
}

/* Return if string1 absorbs string2 */
bool absorbed(char *string1,char *string2)
{
  for (unsigned int i=0;i<strlen(string1);i++)
    if ((string1[i] != string2[i]) && (string1[i] != 'X')) return false;
  return true;
}

level_exp invert_expr(char * product,level_exp oldlist,level_exp newlist,
		      int nsignals)
{
  level_exp newlevel=NULL;
  char oldval;
  int j;
  bool addit;

  if (oldlist) {
    for (j=0;j<nsignals;j++)
      if (oldlist->product[j]!='-') break;
    if (j==nsignals) {
      for (j=0;j<nsignals;j++)
	product[j]='-';
      oldlist=NULL;
    }
  }
  if (oldlist) {
    newlevel=newlist;

    bool done=false;
    for (j=0;j<nsignals;j++)
      if ((product[j]!='X') && (oldlist->product[j]!='X') &&
	  (oldlist->product[j]!=product[j])) {
	done=true;
	break;
      }
    if (done) {
      newlevel=invert_expr(product,oldlist->next,newlevel,nsignals);
    } else {
      for (j=0;j<nsignals;j++)
	if ((oldlist->product[j]=='1') && (product[j]!='1')) {
	  oldval=product[j];
	  product[j]='0';
	  newlevel=invert_expr(product,oldlist->next,newlevel,nsignals);
	  product[j]=oldval;
	} else if ((oldlist->product[j]=='0') && (product[j]!='0')) {
	  oldval=product[j];
	  product[j]='1';
	  newlevel=invert_expr(product,oldlist->next,newlevel,nsignals);
	  product[j]=oldval;
	}
    }
  } else {
    addit=true;
    for (level_exp curlevel=newlist;curlevel;curlevel=curlevel->next)
      if (absorbed(curlevel->product,product)) {
	addit=false;
	break;
      }
    if (addit) {
      newlevel=(level_exp)GetBlock(sizeof(*newlevel));
      newlevel->next=newlist;
      newlevel->product=(char *)GetBlock((nsignals+1) * sizeof(char));
      strcpy(newlevel->product,product);
    } else {
      newlevel=newlist;
    }
  }
  return newlevel;
}

bool T(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,int nsignals,
       int nevents,eventADT *events,int enabled,bool second)
{
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;
  char * product;
  int j;

  switch (*token) {
  case WORD:
  case '(':
    if (!U(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    break;
  case '~':

    strcat(expr,"~");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!U(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;

    product=(char *)GetBlock((nsignals+1) * sizeof(char));
    for (j=0;j<nsignals;j++)
      product[j]='X';
    product[nsignals]='\0';

    /*
    printf("INVERTING: ");
    for (newlevel=(*level);newlevel;newlevel=newlevel->next)
      printf("%s ",newlevel->product);
    printf("\n");
    */

    newlevel=invert_expr(product,(*level),NULL,nsignals);

    free(product);

    curlevel=(*level);
    while (curlevel) {
      curlevel=(*level)->next;
      if ((*level)->product) free((*level)->product);
      free(*level);
      (*level)=curlevel;
    }

    (*level)=newlevel;
    /*
    printf("RESULT:    ");
    for (newlevel=(*level);newlevel;newlevel=newlevel->next)
      printf("%s ",newlevel->product);
    printf("\n");
    */
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return FALSE;
  }
  /*
  printf("T:");
  for (level_exp curlevel=(*level);curlevel;curlevel=curlevel->next)
    printf("%s ",curlevel->product);
  printf("\n");
  */
  return TRUE;
}

level_exp and_expr(level_exp expr1,level_exp expr2,int nsignals,int *linenum)
{
  level_exp newlevel=NULL;
  level_exp level1=NULL;
  level_exp level2=NULL;
  level_exp retlevel=NULL;
  int i;
  bool drop;

  for (level1=expr1;level1;level1=level1->next)
    for (level2=expr2;level2;level2=level2->next) {
      newlevel=(level_exp)GetBlock(sizeof(*newlevel));
      newlevel->next=retlevel;
      newlevel->product=(char *)GetBlock((nsignals+1) * sizeof(char));
      strcpy(newlevel->product,level1->product);
      drop=FALSE;
      for (i=0;i<nsignals;i++)
	if ((newlevel->product[i]=='X')||(newlevel->product[i]=='-'))
	  newlevel->product[i]=level2->product[i];
	else if ((newlevel->product[i]=='1') &&
		 (level2->product[i]=='0'))
	  drop=TRUE;
//	  printf("WARNING:%d: Expression not satisfiable.\n",*linenum);
	else if ((newlevel->product[i]=='0') &&
		 (level2->product[i]=='1'))
	  drop=TRUE;
//	  printf("WARNING:%d: Expression not satisfiable.\n",*linenum);
      if (!drop) retlevel=newlevel;
    }
  return retlevel;
}

level_exp or_expr(level_exp expr1,level_exp expr2,int nsignals,int *linenum)
{
  level_exp level1=NULL;

  if (!expr1) return expr2;
  for (level1=expr1;level1->next;level1=level1->next);
  level1->next=expr2;
  return expr1;
}

bool C(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,int nsignals,
       int nevents,eventADT *events,int enabled, bool second)
{
  level_exp oldlevel=NULL;
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;

  switch (*token) {
  case '&':
    strcat(expr,"&");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!T(token,fp,tokvalue,linenum,expr,&newlevel,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;

    oldlevel=(*level);
    (*level)=and_expr((*level),newlevel,nsignals,linenum);

    curlevel=oldlevel;
    while (curlevel) {
      curlevel=oldlevel->next;
      if (oldlevel->product)
	free(oldlevel->product);
      free(oldlevel);
      oldlevel=curlevel;
    }
    curlevel=newlevel;
    while (curlevel) {
      curlevel=newlevel->next;
      if (newlevel->product)
	free(newlevel->product);
      free(newlevel);
      newlevel=curlevel;
    }

    if (!C(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    break;
  case '|':
  case ')':
  case ']':
    break;
  default:
    printf("ERROR:%d:  Expected a |, &, ), or a ]!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a |, &, ), or a ]!\n",*linenum);
    return FALSE;
  }
  /*
  printf("C:");
  for (level_exp curlevel=(*level);curlevel;curlevel=curlevel->next)
    printf("%s ",curlevel->product);
  printf("\n");
  */
  return TRUE;
}

bool B(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,int nsignals,
       int nevents,eventADT *events,int enabled,bool second)
{
  level_exp newlevel=NULL;


  switch (*token) {
  case '|':
    strcat(expr,"|");
    (*token)=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (!S(token,fp,tokvalue,linenum,expr,&newlevel,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    (*level)=or_expr((*level),newlevel,nsignals,linenum);
    if (!B(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    break;
  case ')':
  case ']':
    break;
  default:
    printf("ERROR:%d:  Expected a |, ), or a ]!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a |, ), or a ]!\n",*linenum);
    return FALSE;
  }
  /*
  printf("B:");
  for (level_exp curlevel=(*level);curlevel;curlevel=curlevel->next)
    printf("%s ",curlevel->product);
  printf("\n");
  */
  return TRUE;
}

bool S(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,int nsignals,
       int nevents,eventADT *events, int enabled, bool second)
{

  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!T(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    if (!C(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return FALSE;
  }
  /*
  printf("S:");
  for (level_exp curlevel=(*level);curlevel;curlevel=curlevel->next)
    printf("%s ",curlevel->product);
  printf("\n");
  */
  return TRUE;
}

bool R(int *token,FILE *fp,char tokvalue[MAXTOKEN],int *linenum,
       char expr[MAXTOKEN],level_exp *level,signalADT *signals,int nsignals,
       int nevents,eventADT *events, int enabled, bool second = false)
{
  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!S(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    if (!B(token,fp,tokvalue,linenum,expr,level,signals,nsignals,
	   nevents,events,enabled,second))
      return FALSE;
    break;
  default:
    printf("ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    fprintf(lg,"ERROR:%d:  Expected a signal, (, or ~!\n",*linenum);
    return FALSE;
  }
  /*
  printf("R:");
  for (level_exp curlevel=(*level);curlevel;curlevel=curlevel->next)
    printf("%s ",curlevel->product);
  printf("\n");
  */
  return TRUE;
}

void absorb_expr(ruleADT **rules,int e,int f,bool POS)
{
  level_exp curlevel;
  if (POS)
    curlevel=rules[e][f]->POS;
  else
    curlevel=rules[e][f]->level;
  level_exp last=NULL;
  while (curlevel) {
    //printf("CHECK: %s\n",curlevel->product);
    level_exp last2=curlevel;
    level_exp curlevel2=curlevel->next;
    bool subset=false;
    while (curlevel2) {
      if ((strcmp(curlevel->product,curlevel2->product)!=0)&&
	  (absorbed(curlevel2->product,curlevel->product))) subset=true;
      if (absorbed(curlevel->product,curlevel2->product)) {
	last2->next=curlevel2->next;
	//printf("DELETEING: %s\n",curlevel2->product);
	free(curlevel2);
      } else
	last2=curlevel2;
      curlevel2=last2->next;
    }
    if (subset) {
      if (last) {
	last->next=curlevel->next;
	//printf("SUBSETING: %s\n",curlevel->product);
	free(curlevel);
      } else {
	if (POS)
	  rules[e][f]->POS=curlevel->next;
	else
	  rules[e][f]->level=curlevel->next;
	//printf("SUBSETING: %s\n",curlevel->product);
	free(curlevel);
      }
    } else
      last=curlevel;
    if (last)
      curlevel=last->next;
    else {
      if (POS)
	curlevel=rules[e][f]->POS;
      else
	curlevel=rules[e][f]->level;
    }
  }
}

void SOP_2_POS(ruleADT **rules,int e,int f,level_exp product,char *sum,
	       int nsignals)
{
  bool recursed=false;
  bool addit;
  if (product) {
    for (int i=0;i<nsignals;i++)
      if (product->product[i]!='X') {
	char old=sum[i];
	sum[i]=product->product[i];
	recursed=true;
	SOP_2_POS(rules,e,f,product->next,sum,nsignals);
	sum[i]=old;
      }
  }
  if (!recursed) {
    addit=true;
    for (level_exp curlevel=rules[e][f]->POS;curlevel;curlevel=curlevel->next)
      if (absorbed(curlevel->product,sum)) {
	addit=false;
	break;
      }
    if (addit) {
      level_exp result=(level_exp)GetBlock(sizeof(*result));
      result->product=CopyString(sum);
      result->next=rules[e][f]->POS;
      rules[e][f]->POS=result;
    }
  }
}

/*****************************************************************************/
/* Load rules.                                                               */
/*****************************************************************************/

bool load_rules(FILE *fp,int nevents,int *nrules,int *number,eventADT *events,
		ruleADT **rules,markkeyADT *markkey,int ruletype,
		char * connected,bool *warning,int nsignals,signalADT *signals,
		int *linenum,bool *level)
{
  int e,f,i,j,count,eps,lower,upper,token;
  char tokvalue[MAXTOKEN];
   level_exp newlevel=NULL;
  //char siglevel;
  //bool firstOR,firstAND;
  CPdf *dist = NULL;
  double rate;
  double x,y;
  char expr[MAXTOKEN];
  bool disabling,assumption;
  count=(*number);
  token=WORD;
  for (i=0;i<count;i++) {

    /* Get enabling event */

    while ((token != END_OF_LINE) && (token != COMMENT)) {
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE);
      }
      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    }
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    if ((e=find_event(nevents,events,tokvalue,*linenum))==(-1)) return(FALSE);
    /* Get enabled event */
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    if ((f=find_event(nevents,events,tokvalue,*linenum))==(-1)) return(FALSE);
    //printf("Loading %d -> %d %d\n",e,f,i);

    /* Get boolean expression */
    expr[0]='\0';
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    disabling=FALSE;
    if (token=='[') {
      (*level)=TRUE;
      // printf("%s -> %s\n",events[e]->event,events[f]->event);
      strcat(expr,"[");
      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);

      if (!R(&token,fp,tokvalue,linenum,expr,&rules[e][f]->level,
	     signals,nsignals,nevents,events,f)) {
	while (token!=']') {
	  if (token==WORD) strcat(expr,tokvalue);
	  else {
	    int position=strlen(expr);
	    expr[position]=token;
	    expr[position+1]='\0';
	  }
	  token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
	}
      }

      if (token!=']') {
	printf("ERROR:%d:  Expecting a ']'!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Expecting a ']'!\n",*linenum);
	return(FALSE);
      }
      strcat(expr,"]");
      //printf("Expr=%s\n",expr);
      /*
      printf("%s -> %s ",events[e]->event,events[f]->event);
      for (newlevel=rules[e][f]->level;newlevel;newlevel=newlevel->next)
      	printf("%s ",newlevel->product);
      printf("\n");
      */
      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      if (strcmp(tokvalue,"d")==0) {
	strcat(expr,"d");
	disabling=TRUE;
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
	events[f]->nondisabling=FALSE;
      }
      else {
    	  events[f]->nondisabling=TRUE;
      }
    } else {
      rules[e][f]->level=(level_exp)GetBlock(sizeof(*newlevel));
      rules[e][f]->level->next=NULL;
      rules[e][f]->level->product=(char *)GetBlock((nsignals+1)*sizeof(char));
      for (j=0;j<nsignals;j++)
	rules[e][f]->level->product[j]='X';
      rules[e][f]->level->product[nsignals]='\0';
    }
    //printf("%s -> %s\n",events[e]->event,events[f]->event);
    /*printf("BEFORE:\n");
    for (level_exp curlevel=rules[e][f]->level;curlevel;
	 curlevel=curlevel->next)
      printf("%s\n",curlevel->product);
    */
    absorb_expr(rules,e,f,false);
    /*
    printf("AFTER:\n");
    for (level_exp curlevel=rules[e][f]->level;curlevel;
	 curlevel=curlevel->next)
      printf("%s\n",curlevel->product);
    */
    /* TODO: PROBLEM HERE Feb 3, 2011
    if (rules[e][f]->level) {
      char *sum=(char*)GetBlock((nsignals+1)*sizeof(char));
      for (int i=0;i<nsignals;i++) sum[i]='X';
      sum[nsignals]='\0';
      SOP_2_POS(rules,e,f,rules[e][f]->level,sum,nsignals);
      free(sum);
      absorb_expr(rules,e,f,true);
    }
    */
    /*
    printf("%s -> %s %s\n",events[e]->event,events[f]->event,expr);
    printf("SOP\n");
    for (level_exp curlevel=rules[e][f]->level;curlevel;
	 curlevel=curlevel->next)
      printf("%s\n",curlevel->product);
    printf("POS\n");
    for (level_exp curlevel=rules[e][f]->POS;curlevel;
	 curlevel=curlevel->next)
      printf("%s\n",curlevel->product);
    */
    /*
    if (0) {
      strcat(expr,"[");
      firstOR=TRUE;
      while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != ']') {
	if (!firstOR) {
	  if (token!='|') {
	    printf("ERROR:%d:  Expecting a '|'!\n",*linenum);
	    fprintf(lg,"ERROR:%d:  Expecting a '|'!\n",*linenum);
	    return(FALSE);
	  }
	  strcat(expr,"|");
	  token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
	}
	firstOR=FALSE;
        if (token=='(') {
	  strcat(expr,"(");
	  newlevel=(level_exp)GetBlock(sizeof(*newlevel));
	  newlevel->next=rules[e][f]->level;
	  newlevel->product=(char *)GetBlock((nsignals+1) * sizeof(char));
	  for (j=0;j<nsignals;j++)
	    newlevel->product[j]='X';
	  newlevel->product[nsignals]='\0';
	  firstAND=TRUE;
	  while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != ')') {
	    if (!firstAND) {
	      if (token!='&') {
		printf("ERROR:%d:  Expecting a '&'!\n",*linenum);
		fprintf(lg,"ERROR:%d:  Expecting a '&'!\n",*linenum);
		return(FALSE);
	      }
	      strcat(expr,"&");
	      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
	    }
	    firstAND=FALSE;
	    siglevel='1';
	    if (token=='~') {
	      strcat(expr,"~");
	      siglevel='0';
	      token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
	    }
	    if (token != WORD) {
	      printf("ERROR:%d:  Expecting a signal name!\n",*linenum);
	      fprintf(lg,"ERROR:%d:  Expecting a signal name!\n",*linenum);
	      return(FALSE);
	    }
	    strcat(expr,tokvalue);
	    for (j=0;j<nsignals;j++)
	      if (strcmp(signals[j]->name,tokvalue)==0) {
		newlevel->product[j]=siglevel;
		break;
	      }
	    if (j==nsignals) {
	      printf("WARNING:%d:  %s is not a signal!\n",*linenum,tokvalue);
	      fprintf(lg,"WARNING:%d:  %s is not a signal!\n",*linenum,
		      tokvalue);
	    }
	  }
	  strcat(expr,")");
	  rules[e][f]->level=newlevel;
	    //printf("%s - %s -> %s\n",events[e]->event,newlevel->product,
		// events[f]->event);
 	} else {
	  printf("ERROR:%d:  Expecting a '('!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expecting a '('!\n",*linenum);
	  return(FALSE);
	}
      }
      if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
      strcat(expr,"]");
    }
    */
    /* Get epsilon value */

    eps=atoi(tokvalue);

    /* Get lower bound */
    assumption=FALSE;
    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    if ((strcmp(tokvalue,"inf")==0) || (strcmp(tokvalue,"infinity")==0)) {
      printf("ERROR:%d:  A lower bound of infinity is not allowed!\n",
	     *linenum);
      fprintf(lg,"ERROR:%d:  A lower bound of infinity is not allowed!\n",
	      *linenum);
      return(FALSE);
    } else if (strcmp(tokvalue,"a")==0) {
      assumption=TRUE;
      lower=0;
    } else lower=atoi(tokvalue);
    if (lower < 0) {
      printf("ERROR:%d:  Negative timing constraints not allowed!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Negative timing constraints not allowed!\n",
	      *linenum);
      return(FALSE);
    }
    if (lower > INFIN) {
      printf("ERROR:%d:  All timing constraints must be less than %d!\n",
	     *linenum,INFIN);
      fprintf(lg,"ERROR:%d:  All timing constraints must be less than %d!\n",
	      *linenum,INFIN);
      return(FALSE);
    }

    /* Get upper bound */

    if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
    if ((strcmp(tokvalue,"inf")==0) || (strcmp(tokvalue,"infinity")==0)) {
      upper=INFIN;
    } else if (strcmp(tokvalue,"a")==0) {
      assumption=TRUE;
      upper=0;
    } else upper=atoi(tokvalue);
    if (upper < 0) {
      printf("ERROR:%d:  Negative timing constraints are not allowed!\n",
	     *linenum);
      fprintf(lg,"ERROR:%d:  Negative timing constraints are not allowed!\n",
	      *linenum);
      return(FALSE);
    }
    if (upper > INFIN) {
      printf("ERROR:%d:  All timing constraints must be less than %d!\n",
	     *linenum,INFIN);
      fprintf(lg,"ERROR:%d:  All timing constraints must be less than %d!\n",
	      *linenum,INFIN);
      return(FALSE);
    }
    if (upper < lower) {
/*      printf("e=%s f=%s ruletype = %d count = %d lower = %d upper = %d\n",
	     events[e]->event,events[f]->event,ruletype,count,lower,upper);*/
      printf("ERROR:%d:  Lower bound larger than upper bound!\n",*linenum);
      fprintf(lg,"ERROR:%d:  Lower bound larger than upper bound!\n",*linenum);
      return(FALSE);
    }

    /* Get probability distribution */
    rate=0.0;
    token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
    if (token==WORD) {
      if ((strcmp(tokvalue,"N")==0) || (strcmp(tokvalue,"n")==0)) {
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!='(') {
	  printf("ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  return(FALSE);
	}
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	x=atof(tokvalue);
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!=',') {
	  printf("ERROR:%d:  Expected a \',\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \',\'!\n",*linenum);
	  return(FALSE);
	}
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	y=atof(tokvalue);
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!=')') {
	  printf("ERROR:%d:  Expected a \')\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \')\'!\n",*linenum);
	  return(FALSE);
	}
	dist=new CNormal(lower,upper,x,y,random_seed());
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      } else if ((strcmp(tokvalue,"U")==0) || (strcmp(tokvalue,"u")==0)) {
	dist=new CUniform(lower,upper,random_seed());
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      } else if ((strcmp(tokvalue,"S")==0) || (strcmp(tokvalue,"s")==0)) {
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!='(') {
	  printf("ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \'(\'!\n",*linenum);
	  return(FALSE);
	}
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	x=atof(tokvalue);
	if (!get_field(fp,tokvalue,&token,linenum)) return(FALSE);
	if (token!=')') {
	  printf("ERROR:%d:  Expected a \')\'!\n",*linenum);
	  fprintf(lg,"ERROR:%d:  Expected a \')\'!\n",*linenum);
	  return(FALSE);
	}
	dist = new CSingular( lower, x );
	rate = x;
	token=fgettok(fp,tokvalue,MAXTOKEN,linenum);
      }
    } else {
      if ( lower == upper )
	dist = new CSingular( lower );
      else
	dist=new CUniform(lower,upper,random_seed());
    }

    /* Store rule in rules matrix */

    if (rules[e][f]->ruletype==NORULE) {
      rules[e][f]->ruletype=(ruletype | SPECIFIED);
      if (disabling)
	rules[e][f]->ruletype=rules[e][f]->ruletype | DISABLING;
      if (assumption)
	rules[e][f]->ruletype=rules[e][f]->ruletype | ASSUMPTION;
      rules[e][f]->epsilon=eps;
      rules[e][f]->lower=lower;
      rules[e][f]->upper=upper;
      rules[e][f]->dist=dist;
      rules[e][f]->rate=rate;
      if (expr[0]!='\0') {
	//Cleans all the erroneous remnants from the expression
	string expression = expr;
	while (expression.find("~()") != string::npos)
	  expression.replace(expression.find("~()"),3,"");
	while (expression.find("()") != string::npos)
	  expression.replace(expression.find("()"),2,"");
	while (expression.find("&]") != string::npos)
	  expression.replace(expression.find("&]"),2,"]");
	while (expression.find("&)") != string::npos)
	  expression.replace(expression.find("&)"),2,")");
	while (expression.find("[]") != string::npos)
	  expression.replace(expression.find("[]"),2,"");
	rules[e][f]->expr=CopyString(expression.c_str());
      }
      if (connected[e]=='I') connected[e]='T';
      else if (connected[e]=='F') connected[e]='O';
      if (connected[f]=='O') connected[f]='T';
      else if (connected[f]=='F') connected[f]='I';
      markkey[(*nrules)]->enabling=e;
      markkey[(*nrules)]->enabled=f;
      markkey[(*nrules)]->epsilon=rules[e][f]->epsilon;
      rules[e][f]->marking=(*nrules);
      (*nrules)++;
      if (events[f]->signal >= 0) {
	if ((f % 2)==1) {
	  signals[events[f]->signal]->riselower=lower;
	  signals[events[f]->signal]->riseupper=upper;
	} else {
	  signals[events[f]->signal]->falllower=lower;
	  signals[events[f]->signal]->fallupper=upper;
	}
      }
      events[f]->lower=lower;
      events[f]->upper=upper;
      events[f]->rate=1;
    } else {
      //printf("Ignored: %s -> %s %d %d\n",events[e]->event,events[f]->event,
      //	     lower,upper);
      if (!(*warning)) {
	printf("WARNING:  Duplicate rules ignored.\n");
	fprintf(lg,"WARNING:  Duplicate rules ignored.\n");
	(*warning)=TRUE;;
      }
      (*number)--;
    }
  }
  return(TRUE);
}

/*****************************************************************************/
/* Load conflict list.                                                       */
/*****************************************************************************/

bool load_conflicts(FILE *fp,int nevents,int *nconf,eventADT *events,
		    ruleADT **rules,int *linenum,int conftype)
{
  int e,f,i,token,count;
  char tokvalue[MAXTOKEN];
  bool warning;

  count=(*nconf);
  warning=FALSE;
  for (i=0;i<count;i++) {
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != WORD)
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE);
      }
    if ((e=find_event(nevents,events,tokvalue,*linenum))==(-1)) return(FALSE);
    while ((token=fgettok(fp,tokvalue,MAXTOKEN,linenum)) != END_OF_LINE) {
      if (token==END_OF_FILE) {
	printf("ERROR:%d:  Unexpected end of file!\n",*linenum);
	fprintf(lg,"ERROR:%d:  Unexpected end of file!\n",*linenum);
	return(FALSE);
      }
      if (token==WORD) {
	if ((f=find_event(nevents,events,tokvalue,*linenum))==(-1))
	  return(FALSE);
	if (rules[e][f]->conflict)
	  (*nconf)--;
	rules[e][f]->conflict=(rules[e][f]->conflict | conftype);
	rules[f][e]->conflict=(rules[e][f]->conflict | conftype);
      }
    }
  }
  return(TRUE);
}

/*****************************************************************************/
/* Load assignments.                                                       */
/*****************************************************************************/

bool load_assignments(FILE *fp, char* *lpnassigns)
{
  int n=0;
  int k=0;
  fpos_t position;
  while (!feof(fp))
  {
    if ((fgetc(fp) == 64) && k==0)  // search for the first occurrance of @ (start of assignments)
    {
      fgetpos(fp,&position);
      k++;
      continue;
    }
    if (k>0)
      n++;
  }
  fsetpos(fp,&position);
  *lpnassigns = (char*)GetBlock(n);
  if(!feof(fp))
  {
    fgets(*lpnassigns,n,fp);
  }
  //cout << "@1 " << *lpnassigns << endl;
  return(TRUE);
}



/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

int number_signals(signalADT *signals,eventADT *events,mergeADT *merges,
		   ruleADT **rules,int nevents,int ninputs,int *ninpsig)
{
  int i,k,position,maxoccur;
  bool countit;
  mergeADT curmerge;
  char * endit;

  position=(-1);
  maxoccur=0;
  for (k=0;k<(nevents-1)/2;k++) {
    countit=TRUE;
    if (merges) {
      for (curmerge=merges[2*k+1]; curmerge != NULL; curmerge=curmerge->link)
        if ((curmerge->mergetype==DISJMERGE) && (curmerge->mevent < (2*k+1)))
	  countit=FALSE;
      for (curmerge=merges[2*k+2]; curmerge != NULL; curmerge=curmerge->link)
        if ((curmerge->mergetype==CONJMERGE) && (curmerge->mevent < (2*k+2)))
	  countit=FALSE;
    }
    maxoccur++;
    if (countit) {
      position++;
      maxoccur=1;
      signals[position]->name=CopyString(events[2*k+1]->event);
      endit=strchr(signals[position]->name,'+');
      if (endit) (*endit)='\0';
      else {
	endit=strchr(signals[position]->name,'-');
	if (endit) (*endit)='\0';
      }
      signals[position]->event=2*k+1;
      for (i=0;i<nevents;i++) {
	if (rules[i][2*k+1]->ruletype & TRIGGER) {
	  signals[position]->riselower=rules[i][2*k+1]->lower;
	  signals[position]->riseupper=rules[i][2*k+1]->upper;
	}
	if (rules[i][2*k+2]->ruletype & TRIGGER) {
	  signals[position]->falllower=rules[i][2*k+2]->lower;
	  signals[position]->fallupper=rules[i][2*k+2]->upper;
	}
      }
    }
    if (position >= 0) signals[position]->maxoccurrence=maxoccur;
    events[2*k+1]->signal=position;
    events[2*k+2]->signal=position;
    if (k==(ninputs/2-1)) (*ninpsig)=position+1;
  }
  printf("ninputs = %d ; nsignals = %d\n",(*ninpsig),position+1);
  return(position+1);
}

/*****************************************************************************/
/* Load a timed event-rule structure from a file.                            */
/*****************************************************************************/

bool load_er(char * filename,eventADT *events,mergeADT *merges,ruleADT **rules,
	     markkeyADT *markkey,int *nrules,bool verbose,int *nord,
	     int *nconf,int *niconf,int *noconf,int *ncausal,int *nsignals,
	     int *ninpsig,
	     signalADT *signals,int *ndummy,bool *level,bool fromCSP,
	     bool fromVHDL,bool fromHSE,bool fromER,bool fromTEL,
	     bool fromG, char* *lpnassigns)
{
  char inname[FILENAMESIZE];
  char * connected;
  char * event;
  char * startstate;
  char * initval;
  char * initrate;
  int i,nevents,ninputs,ndisj;
  FILE *fp;
  bool warning;
  int linenum;

  strcpy(inname,filename);
  if (fromCSP || fromER || fromG)
    strcat(inname,".er");
  else
    strcat(inname,".tel");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE);
  }
  if (fromCSP || fromER || fromG) {
    printf("Loading timed event-rule structure from:  %s\n",inname);
    fprintf(lg,"Loading timed event-rule structure from:  %s\n",inname);
  } else {
    printf("Loading timed event/level structure from:  %s\n",inname);
    fprintf(lg,"Loading timed event/level structure from:  %s\n",inname);
  }
  linenum=1;
  event=load_header(fp,&nevents,&ninputs,ncausal,&ndisj,nconf,niconf,noconf,
		    nord,&startstate,&initval,&initrate,&linenum);
  if (startstate) free(startstate);
  //if (initial) free(initial);
  if (event==NULL) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  if (!load_events(fp,nevents,events,event,nsignals,ninpsig,signals,ninputs,
		   ndummy,&linenum)) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  init_rules(nevents,rules);
  connected=(char *)GetBlock((nevents+1) * sizeof(char));
  for (i=0;i<nevents;i++)
    connected[i]='F';
  warning=FALSE;
  if (!load_rules(fp,nevents,nrules,ncausal,events,rules,markkey,TRIGGER,
		  connected,&warning,*nsignals,signals,&linenum,level)) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  if (!load_rules(fp,nevents,nrules,nord,events,rules,markkey,ORDERING,
		  connected,&warning,*nsignals,signals,&linenum,level)) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  if (!load_mergers(fp,nevents,ndisj,events,merges,&linenum)) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  if (!load_conflicts(fp,nevents,niconf,events,rules,&linenum,ICONFLICT)) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  if (!load_conflicts(fp,nevents,noconf,events,rules,&linenum,OCONFLICT)) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  if (!load_conflicts(fp,nevents,nconf,events,rules,&linenum,
		      (ICONFLICT | OCONFLICT))) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  // ZHEN(Done): Call a function load_assignments
  // Read the next line of the file pointed to by "fp"
  if (!load_assignments(fp, lpnassigns)) {
      fclose(fp);
      return(FALSE);
  } 
  /*
  cout << "**************" << endl;
  cout << "@2_1 " << lpnassigns << endl;
  cout << "@2_2 " << *lpnassigns << endl;
  cout << "@2_3 " << **lpnassigns << endl;
  cout << "**************" << endl;
  */
  print_warnings(nevents,events,connected,verbose,!(*level));
  free(connected);
  free(event);
  fclose(fp);
  return(TRUE);
}

/*****************************************************************************/
/* Load just the header information.                                         */
/*****************************************************************************/

bool load_header_only(char * filename,int *nevents,int *ninputs,int *ndisj,
		      int *nconf,int *niconf,int *noconf,int *nord,
		      char * *startstate,char * *initval,char * *initrate,
		      bool fromCSP,bool fromVHDL,bool fromHSE,bool fromER,
		      bool fromTEL,bool fromG)
{
  char * event;
  char inname[FILENAMESIZE];
  int ncausal;
  FILE *fp;
  int linenum;

  strcpy(inname,filename);
  if (fromCSP || fromER || fromG)
    strcat(inname,".er");
  else
    strcat(inname,".tel");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE);
  }
  if (*startstate) free(*startstate);
  //if (*initval) free(*initval);
  //if (*initrate) free(*initrate);
  *initval=NULL;
  *initrate=NULL;
  *startstate=NULL;
  linenum=1;
  event=load_header(fp,nevents,ninputs,&ncausal,ndisj,nconf,niconf,noconf,
		    nord,startstate,initval,initrate,&linenum);
  if (event==NULL) {
    free(event);
    fclose(fp);
    return(FALSE);
  }
  free(event);
  fclose(fp);
  return(TRUE);
}

void assign_processes(eventADT *events,cycleADT ****cycles,int nevents,
		      int ncycles)
{
  int i,j,proc;

  proc=0;
  for (i=1;i<nevents;i++)
    if (!events[i]->dropped)
      if (events[i]->process==proc) {
	for (j=i+1;j<nevents;j++) {
	  if (!events[j]->dropped)
	    if (events[j]->process==proc)
	      if (reachable(cycles,nevents,ncycles,i,0,j,ncycles-1)!=1)
		events[j]->process++;
	}
	proc++;
      }
}

/*****************************************************************************/
/* Load event rule system and setup appropriate data structures.             */
/*****************************************************************************/

bool load_event_rule_system(char menu,char command,designADT design,
			    bool sifile,bool newfile)
{
  int i,tempsigs;

  if ((menu==LOAD && command==TIMEDER) ||
      (!(design->status & LOADED)) || (newfile)) {
    if (load_header_only(design->filename,&(design->nevents),
			 &(design->ninputs),&(design->ndisj),
			 &(design->nconf),&(design->niconf),&(design->noconf),
			 &(design->nord),
			 &(design->startstate),&(design->initval),
			 &(design->initrate),design->fromCSP,
			 design->fromVHDL,design->fromHSE,design->fromER,
			 design->fromTEL,design->fromG)) {

      design->nsignals=design->nevents;
      design->ninpsig=0;

      design->signals=new_signals(design->status,FALSE,design->oldnsignals,
				  design->nsignals,design->signals);

      tempsigs=design->nsignals;

      design->events=new_events(design->status,FALSE,design->oldnevents,
				design->nevents,design->events);
      design->merges=new_merges(design->status,FALSE,design->oldnevents,
				design->nevents,design->merges);
      design->rules=new_rules(design->status,FALSE,design->oldnevents,
			      design->nevents,design->rules);
      design->markkey=new_markkey(design->status,FALSE,
				  (design->oldnevents)*(design->oldnevents),
				  (design->nevents)*(design->nevents),
				  design->markkey);
      design->nrules=0;
      design->level=FALSE;
      if (load_er(design->filename,design->events,design->merges,design->rules,
		  design->markkey,&(design->nrules),
		  design->verbose,&(design->nord),&(design->nconf),
		  &(design->niconf),&(design->noconf),
		  &(design->ncausal),&(design->nsignals),&(design->ninpsig),
		  design->signals,&(design->ndummy),&(design->level),
		  design->fromCSP,design->fromVHDL,design->fromHSE,
		  design->fromER,design->fromTEL,design->fromG,&(design->assignments))) {
	//cout << "#################" << endl;
	//cout << "@3 " << design->assignments << endl;
	//cout << "#################" << endl;
/*
	design->nsignals=number_signals(design->signals,design->events,
					design->merges,design->rules,
					design->nevents,design->ninputs,
					&(design->ninpsig));
*/
	printf("Initializing %d cycles ... ",design->ncycles); fflush(stdout);
	fprintf(lg,"Initializing %d cycles ... ",design->ncycles);
	fflush(lg);
	design->cycles=new_cycles(design->status,FALSE,design->oldncycles,
				  design->ncycles,design->oldnevents,
				  design->nevents,design->rules,
				  design->cycles);
	printf("done\n");
	fprintf(lg,"done\n");
	assign_processes(design->events,design->cycles,design->nevents,
			 design->ncycles);
#ifdef TSE_ALGM
	if ((design->status & LOADED) && (design->closure)) free_tse();
#endif
	initialize_state_table(LOADED,FALSE,design->state_table);

#ifdef DLONG
	initialize_bdd_state_table(design->status,FALSE,design->bdd_state,
				   design->signals,design->nsignals,
				   design->nrules);
#else
	//CUDD
	design->bdd_state=new_bdd_state_table(design->status,FALSE,
					      design->bdd_state,
					      design->signals,
					      design->nsignals,
					      design->ninpsig,
					      design->nrules,
					      design->filename);
#endif

	design->regions=new_regions(design->status,FALSE,
				    2*(design->oldnsignals)+1,
				    2*(design->nsignals)+1,design->regions);

	for (i=design->nsignals;i<tempsigs;i++)
	  free(design->signals[i]);
	design->signals=(signalADT*)realloc(design->signals,
					    design->nsignals * sizeof(design->signals[0]));

	design->oldnsignals=design->nsignals;
	design->oldnevents=design->nevents;
	design->oldncycles=design->ncycles;
	design->si=sifile;
	design->status=LOADED;
	if (design->sis) make_env(command,design);
#ifdef TSE_ALGM
	if (design->closure) init_tse(design->filename,design->events,
				      design->rules,design->nevents);
#endif
	return(TRUE);
      } else {
	design->signals=new_signals(design->status,TRUE,design->oldnsignals,
				    design->nsignals,design->signals);
	design->events=new_events(design->status,TRUE,design->oldnevents,
				  design->nevents,design->events);
	design->merges=new_merges(design->status,TRUE,design->oldnevents,
				  design->nevents,design->merges);
	design->rules=new_rules(design->status,TRUE,design->oldnevents,
				design->nevents,design->rules);
	design->markkey=new_markkey(design->status,TRUE,
				    (design->oldnevents)*(design->oldnevents),
				    ((design->nevents)*(design->nevents)),
				    design->markkey);
	design->status=RESET;
	design->nevents=0;
	design->nsignals=0;
	design->si=FALSE;
	return(FALSE);
      }
    } else {
      design->status=RESET;
      design->signals=new_signals(design->status,TRUE,design->oldnsignals,
				  design->nsignals,design->signals);
      design->events=new_events(design->status,TRUE,design->oldnevents,
				design->nevents,design->events);
      new_design(design,FALSE);
      design->nevents=0;
      design->nsignals=0;
      design->si=FALSE;
      return(FALSE);
    }
  }
  if (!(design->fromVHDL)) {
    top_port=set<string>();
    top_port_map=map<string,string>();
    for (int i=0;i<design->nsignals;i++) {
      top_port.insert(design->signals[i]->name);
      top_port_map.insert(make_pair(design->signals[i]->name,
				    design->signals[i]->name));
    }
  }
  return(TRUE);
}



