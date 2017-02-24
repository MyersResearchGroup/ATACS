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
#include <string>
#include "struct.h"
#include "loadg.h"
#include "memaloc.h"
#include "def.h"
#include "findrsg.h"
#include "lhpnrsg.h"
#include "interface.h"
#include "merges.h"
#include "loader.h"
#include "lpntrsfm.h"
#include "displayADT.h"
#define __LEVELS__

#define LML_Next(n) ( (n)->next )

extern set<string> top_port;
extern map<string,string> top_port_map;

extern int gparse();
extern int gerror(char *s);
extern FILE *gin;
extern bool completed;

char *initial_state;

proplistADT properties;

int recur_depth;

bool hasFailTrans;

char *node_type_table[]={
  "transiton",
  "place    ",
  "input    ",
  "output   ",
  "internal "
  "dummy "
};

typedef struct lml_ {
  mark_elem * ml;
  mark_elem * eventlist;
  int i;
  struct lml_ * next;
} lml;

char *out_file, *in_file, *new_in_file;
FILE *outfile;

lml *head;

/* Name table: headers of collision chains (buckets?) */
Name_Ptr name_table [NAME_MAX_ENTRIES];

int      last_hash_entry;
Name_Ptr last_name_ptr;

marking_list initial_marking;
int num_of_ios;
int num_of_inps;
int num_of_dummys;
int num_of_node;
int num_inp_trans;
Node *io_list;
Node *node_list;
int *init_state;

//integer expression parser

int intexpr_gettok(char *expr,char *tokvalue,int maxtok,int *position)
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  int readword;   /* True if token is a word */
  char *orig;

  orig = tokvalue;

  readword = 0;
  toklen = 0;
  *tokvalue = '\0';
  while ((*position)<(signed)strlen(expr)) {
    c=expr[(*position)];
    (*position)++;
    switch (c) {
    case '(':
    case ')':
    case '[':
    case ']':
    case ',':
    case '~':
    case '|':
    case '&':
    case '+':
    case '*':
    case '^':
    case '/':
    case '%':
    case '=':
    case '<':
    case '>':
      if (!readword) return(c);
      else {
	(*position)--;
	return(WORD);
      }
      break;
    case '-':
      if (!readword) {
	if (expr[(*position)]=='>'){
	  (*position)++;
	  return(IMPLIES);
	}
	else {
	  return(c);
	}
      }else {
	(*position)--;
	return(WORD);
      }
      break;
    case ' ':
      if (readword){
	return(WORD);
      }
      break;
    default:
      if (toklen < maxtok) {
	readword=1;
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
      }
      break;
    }
  }
  if (!readword)
    return(END_OF_FILE);
  else {
    return(WORD);
  }
}


/* bool intexpr_L(int *token,char *expr,char tokvalue[MAXTOKEN],int *position, */
/* 	       exprsn **result, signalADT *signals,int nsignals,  */
/* 	       eventADT *events,int nevents, int nplaces); */


bool intexpr_S(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces);


bool intexpr_R(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces);


bool intexpr_U(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  int i;
  int temp,temp2;
  double dtemp;
  bool is_real =  false;
  exprsn *newresult =NULL;

  switch (*token) {
  case WORD:
    if (!strcmp(tokvalue,"AND")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces)) {
	return 0;
      }
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on bitwise AND operator *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = (int)(*result)->lvalue & (int)newresult->lvalue;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"&",'w');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"min")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on min operator *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = ((int)(*result)->lvalue < (int)newresult->lvalue) ?
	  (int)(*result)->lvalue : (int)newresult->lvalue;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"m",'a');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"max")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on min operator *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = ((int)(*result)->lvalue > (int)newresult->lvalue) ?
	  (int)(*result)->lvalue : (int)newresult->lvalue;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"M",'a');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"idiv")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on min operator *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = (int)(*result)->lvalue / (int)newresult->lvalue;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"i",'a');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"OR")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on bitwise OR operator *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = (int)(*result)->lvalue | (int)newresult->lvalue;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"|",'w');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"XOR")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = (int)(*result)->lvalue ^ (int)newresult->lvalue;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"X",'w');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"uniform")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on uniform function *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))){
	(*result)->isit = 'n';
	(*result)->lvalue = (*result)->lvalue;
	(*result)->uvalue = newresult->uvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"u",'w');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"normal")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	    ((*result)->isit=='l')){
	cout << "*** type mismatch on normal function *** \n";
      }
      (*result)->isit = 'n';
      (*result)->lvalue = -INFIN;
      (*result)->uvalue = INFIN;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if ((!strcmp(tokvalue,"gamma"))||(!strcmp(tokvalue,"lognormal"))){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	    ((*result)->isit=='l')){
	cout << "*** type mismatch on gamma or lognormal function *** \n";
      }
      (*result)->isit = 'n';
      (*result)->lvalue = 0;
      (*result)->uvalue = INFIN;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"binomial")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on binomial function*** \n";
      }
      //simplify if operands are static
      if (((*result)->isit=='n')||((*result)->isit=='t')) {
	(*result)->isit = 'n';
	(*result)->lvalue = 0;
	(*result)->uvalue = (*result)->uvalue;
	delete newresult;
      } else{
	(*result)->isit = 'n';
	(*result)->lvalue = 0;
	(*result)->uvalue = INFIN;
	delete newresult;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"BIT")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ',') {
	printf("ERROR: Expected a ,\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((newresult->isit=='t')||(newresult->isit=='b')||
	   (newresult->isit=='l'))||
	  (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l'))){
	cout << "*** type mismatch on 'BIT' operator *** \n";
      }
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 't';
	(*result)->lvalue =
	  (((int)(*result)->lvalue)>>((int)newresult->lvalue))&1;
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	(*result) = new exprsn((*result),newresult,"[]",'b');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//     } else if (!strcmp(tokvalue,"floor")){
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//       if ((*token) != '(') {
// 	printf("ERROR: Expected a (\n");
// 	return 0;
//       }
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//       if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
// 		     events,nevents,nplaces))
// 	return 0;
//       if ((*token) != ')') {
// 	printf("ERROR: Expected a )\n");
// 	return 0;
//       }
//       //simplify if operands are static
//       if (((*result)->isit=='n')||((*result)->isit=='t')){
// 	(*result)->isit = 'n';
// 	(*result)->lvalue = floor((*result)->lvalue);
// 	(*result)->uvalue = floor((*result)->uvalue);
//       } else{
// 	(*result) = new exprsn((*result),NULL,"floor",'a');
//       }
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//     } else if (!strcmp(tokvalue,"round")){
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//       if ((*token) != '(') {
// 	printf("ERROR: Expected a (\n");
// 	return 0;
//       }
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//       if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
// 		     events,nevents,nplaces))
// 	return 0;
//       if ((*token) != ')') {
// 	printf("ERROR: Expected a )\n");
// 	return 0;
//       }
//       //simplify if operands are static
//       if (((*result)->isit=='n')||((*result)->isit=='t')){
// 	(*result)->isit = 'n';
// 	(*result)->lvalue = round((*result)->lvalue);
// 	(*result)->uvalue = round((*result)->uvalue);
//       } else{
// 	(*result) = new exprsn((*result),NULL,"round",'a');
//       }
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//     } else if (!strcmp(tokvalue,"ceil")){
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//       if ((*token) != '(') {
// 	printf("ERROR: Expected a (\n");
// 	return 0;
//       }
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
//       if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
// 		     events,nevents,nplaces))
// 	return 0;
//       if ((*token) != ')') {
// 	printf("ERROR: Expected a )\n");
// 	return 0;
//       }
//       //simplify if operands are static
//       if (((*result)->isit=='n')||((*result)->isit=='t')){
// 	(*result)->isit = 'n';
// 	(*result)->lvalue = ceil((*result)->lvalue);
// 	(*result)->uvalue = ceil((*result)->uvalue);
//       } else{
// 	(*result) = new exprsn((*result),NULL,"ceil",'a');
//       }
//       (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"NOT")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l')){
	cout << "*** type mismatch on bitwise NOT operator *** \n";
      }
      //simplify if operands are static
      if (((*result)->isit=='n')||((*result)->isit=='t')&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)){
	(*result)->isit = 'n';
	(*result)->lvalue = ~(int)(*result)->lvalue;
	(*result)->uvalue = (*result)->lvalue;
      } else{
	(*result) = new exprsn((*result),NULL,"~",'w');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"exponential")||!strcmp(tokvalue,"chisq")||
	       !strcmp(tokvalue,"rayleigh")||!strcmp(tokvalue,"poisson")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l')){
	cout << "*** type mismatch on exponential, chisq, rayleigh, or poisson function  *** \n";
      }
      (*result)->isit = 'n';
      (*result)->lvalue = 0;
      (*result)->uvalue = INFIN;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"laplace")||!strcmp(tokvalue,"cauchy")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l')){
	cout << "*** type mismatch on laplace or cauchy function  *** \n";
      }
      (*result)->isit = 'n';
      (*result)->lvalue = -INFIN;
      (*result)->uvalue = INFIN;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"bernoulli")) {
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	   ((*result)->isit=='l')){
	cout << "*** type mismatch on berboulli function  *** \n";
      }
      (*result)->isit = 'n';
      (*result)->lvalue = 0;
      (*result)->uvalue = 1;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"rate")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      (*result)->isit='d';
      (*result)->lvalue = 0;
      (*result)->uvalue = 0;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"floor")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"ceil")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"INT")){
      //cout << "matched an int cast!\n";
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_L(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (!(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
	cout << "*** type mismatch on INT typecast  *** \n";
      }
      //simplify if operands are static
      if (((*result)->isit=='n')||((*result)->isit=='t')&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)){
	(*result)->isit = 'n';
	//(*result)->lvalue = ~(int)(*result)->lvalue;
	//(*result)->uvalue = (*result)->lvalue;
      } else{
	(*result) = new exprsn((*result),NULL,"INT",'a');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if (!strcmp(tokvalue,"BOOL")){
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if ((*token) != '(') {
	printf("ERROR: Expected a (\n");
	return 0;
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      if ((*token) != ')') {
	printf("ERROR: Expected a )\n");
	return 0;
      }
      // check for type mismatch
      if (((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l')){
	cout << "*** type mismatch on BOOL typecast  *** \n";
      }
      //simplify if operands are static
      if (((*result)->isit=='n')||((*result)->isit=='t')&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)){
	(*result)->isit = 't';
	if ((*result)->lvalue != 0){
	  (*result)->lvalue = 1;
	  (*result)->uvalue = 1;
	}
      } else{
	(*result) = new exprsn((*result),NULL,"BOOL",'l');
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    } else if ((!strcmp(tokvalue,"true"))||(!strcmp(tokvalue,"t"))){
      (*result) = new exprsn('t',true,true,-1);
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    }else if ((!strcmp(tokvalue,"false"))||(!strcmp(tokvalue,"f"))){
      (*result) = new exprsn('t',false,false,-1);
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    }else{
      //do boolean lookup here!!!
      for (i=0;i<nsignals;i++){
	if (!strcmp(signals[i]->name,tokvalue)){
	  //printf("successful lookup of boolean variable %s\n",signals[i]);
	  (*result) = new exprsn('b',0,1,i);
	  (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
	  return 1;
	}
      }
      for (i=nevents;i<nevents+nplaces;i++){
	if (!strcmp(events[i]->event,tokvalue)){
	  //printf("successful lookup of variable %s\n",events[i]->event);
	  if (events[i]->type == VAR){
	    //printf("parsed discrete variable\n");
	    (*result) = new exprsn('i',-INFIN,INFIN,i);
	  }
	  else{
	    //printf("parsed continuous variable\n");
	    (*result) = new exprsn('c',-INFIN,INFIN,i);
	    //printf("isit = %c\n",(*result)->isit);
	  }
	  (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
	  return 1;
	}
      }
      if (!strcmp(tokvalue,"inf")) {
	(*result) = new exprsn('n',INFIN,INFIN,-1);
	(*result)->real = INFIN;
	(*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
	return 1;
      }
      if ((tokvalue[0]>'9')||(tokvalue[0]<'0')){
	printf("U1:ERROR(%s): Expected an ID, Number, or a (\n",tokvalue);
	return 0;
      }
      for (i=0;tokvalue[i]!=0;i++)
	if ((tokvalue[i]=='.')||(tokvalue[i]=='e')||(tokvalue[i]=='E')){
	  is_real = true;
	  break;
	}
      if (is_real){
	dtemp = atof(tokvalue);
	temp = floor(dtemp);
	temp2 = ceil(dtemp);
	(*result) = new exprsn('n',temp,temp2,-1);
	(*result)->real = dtemp;
      }
      else {
	temp = atoi(tokvalue);
	(*result) = new exprsn('n',temp,temp,-1);
      }
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      //printf("resolved number %f\n",temp);
    }
    break;
  case '(':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_L(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if ((*token) != ')') {
      printf("ERROR: Expected a )\n");
      return 0;
    }
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    break;
  default:
    printf("U2:ERROR: Expected a ID, Number, or a (\n");
    return 0;
  }
  return 1;
}

bool intexpr_T(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
      exprsn **result, signalADT *signals,int nsignals,
      eventADT *events,int nevents, int nplaces)
{
  switch (*token) {
  case WORD:
  case '(':
    if (!intexpr_U(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '-':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_U(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((*result)->isit=='t')||((*result)->isit=='b')||
	((*result)->isit=='l')){
      cout << "*** type mismatch on unary '-' operator  *** \n";
    }
    //simplify if operands are static
    if (((*result)->isit=='n')||((*result)->isit=='t')&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = -((*result)->lvalue);
      (*result)->uvalue = (*result)->lvalue;
    } else{
      (*result) = new exprsn((*result),NULL,"U-",'a');
    }
    break;
  default:
    printf("T:ERROR: Expected a ID, Number, (, or -\n");
    return 0;
  }
  return 1;
}

bool intexpr_C(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  exprsn *newresult;

  switch (*token) {
  case '*':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_T(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '*' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = (*result)->lvalue * newresult->lvalue;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"*",'a');
    }
    if (!intexpr_C(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '^':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_T(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '^' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = pow((float)(*result)->lvalue,(float)newresult->lvalue);
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"^",'a');
    }
    if (!intexpr_C(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '/':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_T(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '/' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = (*result)->lvalue / newresult->lvalue;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"/",'a');
    }
    if (!intexpr_C(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '%':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_T(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '%' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = (int)(*result)->lvalue % (int)newresult->lvalue;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"%",'a');
    }
    if (!intexpr_C(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '+':
  case '-':
  case ')':
  case '[':
  case ']':
  case '|':
  case '&':
  case '=':
  case '<':
  case '>':
  case ',':
  case IMPLIES:
  case END_OF_FILE:
    break;
  case '(':
  case WORD:
    // IMPLICIT MULTIPLICATION
    if (!intexpr_T(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on imlicit multiplication operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = (*result)->lvalue * newresult->lvalue;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"*",'a');
    }
    if (!intexpr_C(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;

  default:
    printf("ERROR: Expected a * or /\n");
    return 0;
  }
  return 1;
}

bool intexpr_B(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  exprsn *newresult;

  switch (*token) {
  case '+':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_S(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '+' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = (*result)->lvalue + newresult->lvalue;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"+",'a');
    }
    if (!intexpr_B(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '-':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_S(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '-' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 'n';
      (*result)->lvalue = (*result)->lvalue - newresult->lvalue;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"-",'a');
    }
    if (!intexpr_B(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case ')':
  case '[':
  case ']':
  case '|':
  case '&':
  case '=':
  case '<':
  case '>':
  case ',':
  case IMPLIES:
  case END_OF_FILE:
    break;
  default:
    printf("ERROR: Expected a + or -\n");
    return 0;
  }
  return 1;
}

bool intexpr_S(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  switch (*token) {
  case WORD:
  case '(':
  case '-':
    if (!intexpr_T(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if (!intexpr_C(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  default:
    printf("S:ERROR: Expected a ID, Number, (, or -\n");
    return 0;
  }
  return 1;
}

bool intexpr_R(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  switch (*token) {
  case WORD:
  case '(':
  case '-':
    if (!intexpr_S(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if (!intexpr_B(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  default:
    printf("R:ERROR: Expected a ID, Number, (, or -\n");
    return 0;
  }
  return 1;
}

bool intexpr_M(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces);

bool intexpr_P(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  exprsn *newresult;
  int spos,i;
  char ineq[MAXTOKEN];
  string comp;
  //  printf("P\n");
  switch (*token) {
  case '=':
    spos = *position;
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;

    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 't';
      (*result)->lvalue = ((*result)->lvalue == newresult->lvalue);
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      if ((*result)->isit=='c'){
	comp = events[(*result)->index]->event;
	comp +="=";
	int paren=0;
	for (i=spos;i<*position-1;i++) {
	  if (expr[i]=='(') paren++;
	  if (expr[i]==')') paren--;
	  ineq[i-spos]=expr[i];
	}
	ineq[i-spos+paren]=0;
	comp += ineq;
	//printf("looking for %s\n",comp.c_str());
	for (i=0;i<nsignals;i++){
	  if (!strcmp(signals[i]->name,comp.c_str())){
	    //printf("successful lookup of boolean variable '%s'\n",signals[i]->name);
	    (*result)->isit = 'b';
	    (*result)->index = i;
	    (*result)->lvalue = 0;
	    (*result)->uvalue = 1;
	    delete newresult;
	    return 1;
	  }
	} 
	printf("Could not find %s\n",comp.c_str());
      }else{
	(*result) = new exprsn((*result),newresult,"==",'r');
      }
    }
    break;
  case '>':
    spos = *position;
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if ((*token)=='='){
      spos = *position;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		     events,nevents,nplaces))
	return 0;
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 't';
	(*result)->lvalue = ((*result)->lvalue >= newresult->lvalue);
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	if ((*result)->isit=='c'){
	  comp = events[(*result)->index]->event;
	  comp +=">=";
	  int paren=0;
	  for (i=spos;i<*position-1;i++) {
	    if (expr[i]=='(') paren++;
	    if (expr[i]==')') paren--;
	    ineq[i-spos]=expr[i];
	  }
	  ineq[i-spos+paren]=0;
	  comp += ineq;
	  //printf("looking for %s\n",comp.c_str());
	  for (i=0;i<nsignals;i++){
	    if (!strcmp(signals[i]->name,comp.c_str())){
	      //printf("successful lookup of boolean variable '%s'\n",signals[i]->name);
	      (*result)->isit = 'b';
	      (*result)->index = i;
	      (*result)->lvalue = 0;
	      (*result)->uvalue = 1;
	      delete newresult;
	      return 1;
	    }
	  }
	  printf("Could not find %s\n",comp.c_str());
	}else{
	  (*result) = new exprsn((*result),newresult,">=",'r');
	}
      }
    }else{
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
	return 0;
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 't';
	(*result)->lvalue = ((*result)->lvalue > newresult->lvalue);
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	if ((*result)->isit=='c'){
	  comp = events[(*result)->index]->event;
	  comp +=">";
	  int paren=0;
	  for (i=spos;i<*position-1;i++) {
	    if (expr[i]=='(') paren++;
	    if (expr[i]==')') paren--;
	    ineq[i-spos]=expr[i];
	  }
	  ineq[i-spos+paren]=0;
	  comp += ineq;
	  //printf("looking for %s\n",comp.c_str());
	  for (i=0;i<nsignals;i++){
	    if (!strcmp(signals[i]->name,comp.c_str())){
	      //  printf("successful lookup of boolean variable '%s'\n",signals[i]->name);
	      (*result)->isit = 'b';
	      (*result)->index = i;
	      (*result)->lvalue = 0;
	      (*result)->uvalue = 1;
	      delete newresult;
	      return 1;
	    }
	  }
	  printf("Could not find %s\n",comp.c_str());
	}else{
	  (*result) = new exprsn((*result),newresult,">",'r');
	}
      }
    }
    break;
  case '<':
    spos = *position;
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if ((*token)=='='){
      spos = *position;
      (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
	return 0;
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 't';
	(*result)->lvalue = ((*result)->lvalue <= newresult->lvalue);
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	if ((*result)->isit=='c'){
	  comp = events[(*result)->index]->event;
	  comp +="<=";
	  int paren=0;
	  for (i=spos;i<*position-1;i++) {
	    if (expr[i]=='(') paren++;
	    if (expr[i]==')') paren--;
	    ineq[i-spos]=expr[i];
	  }
	  ineq[i-spos+paren]=0;
	  comp += ineq;
	  //printf("looking for %s\n",comp.c_str());
	  for (i=0;i<nsignals;i++){
	    if (!strcmp(signals[i]->name,comp.c_str())){
	      //printf("successful lookup of boolean variable '%s'\n",signals[i]->name);
	      (*result)->isit = 'b';
	      (*result)->index = i;
	      (*result)->lvalue = 0;
	      (*result)->uvalue = 1;
	      delete newresult;
	      return 1;
	    }
	  }
	  printf("Could not find %s\n",comp.c_str());
	}else{
	  (*result) = new exprsn((*result),newresult,"<=",'r');
	}
      }
    }else{
      if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
	return 0;
      //simplify if operands are static
      if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	  (((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
	(*result)->isit = 't';
	(*result)->lvalue = ((*result)->lvalue < newresult->lvalue);
	(*result)->uvalue = (*result)->lvalue;
	delete newresult;
      } else{
	if ((*result)->isit=='c'){
	  comp = events[(*result)->index]->event;
	  comp +="<";
	  int paren=0;
	  for (i=spos;i<*position-1;i++) {
	    if (expr[i]=='(') paren++;
	    if (expr[i]==')') paren--;
	    ineq[i-spos]=expr[i];
	  }
	  ineq[i-spos+paren]=0;
	  comp += ineq;
	  printf("looking for %s\n",comp.c_str());
	  for (i=0;i<nsignals;i++){
	    if (!strcmp(signals[i]->name,comp.c_str())){
	      printf("successful lookup of boolean variable '%s'\n",signals[i]->name);
	      (*result)->isit = 'b';
	      (*result)->index = i;
	      (*result)->lvalue = 0;
	      (*result)->uvalue = 1;
	      delete newresult;
	      return 1;
	    }
	  }
	  printf("Could not find %s\n",comp.c_str());
	}else{
	  (*result) = new exprsn((*result),newresult,"<",'r');
	}
      }
    }
    break;
  case '[':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_R(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if ((*token) != ']') {
      printf("ERROR: Expected a ]\n");
      return 0;
    }
    // check for type mismatch
    if (((newresult->isit=='t')||(newresult->isit=='b')||
	  (newresult->isit=='l'))||
	(((*result)->isit=='t')||((*result)->isit=='b')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '[]' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 't';
      (*result)->lvalue =
	(((int)(*result)->lvalue)>>((int)newresult->lvalue))&1;
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"[]",'w');
    }
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    break;
  case '&':
  case '|':
  case ')':
  case IMPLIES:
  case END_OF_FILE:
    break;
  default:
    printf("ERROR: Expected a [, =, <, or >\n");
    return 0;
  }
  //printf("/P\n");
  return 1;
}


bool intexpr_O(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  //  printf("O\n");

  switch (*token) {
  case WORD:
  case '(':
  case '-':
    if (!intexpr_R(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if (!intexpr_P(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  default:
    printf("O:ERROR: Expected a ID, Number, or a (\n");
    return 0;
  }
  //  printf("/O\n");
  return 1;
}

bool intexpr_N(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  //  printf("N\n");

  switch (*token) {
  case WORD:
  case '-':
  case '(':
    if (!intexpr_O(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '~':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_O(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (!(((*result)->isit=='t')||((*result)->isit=='b')||((*result)->isit=='r')||
	  ((*result)->isit=='l'))){
      cout << "*** type mismatch on '~' operator *** \n";
    }
    //simplify if operands are static
    if (((*result)->isit=='n')||((*result)->isit=='t')&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)){
      (*result)->isit = 't';
      (*result)->lvalue = !((*result)->lvalue);
      (*result)->uvalue = (*result)->lvalue;
    } else{
      (*result) = new exprsn((*result),NULL,"!",'l');
    }
    break;
  default:
    printf("N:ERROR: Expected a ID, Number, (, or -\n");
    return 0;
  }
  return 1;
}

bool intexpr_E(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  exprsn *newresult;
  //printf("E\n");

  switch (*token) {
  case '&':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_N(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (!(((newresult->isit=='t')||(newresult->isit=='b')||(newresult->isit=='r')||
	  (newresult->isit=='l'))&&
	(((*result)->isit=='t')||((*result)->isit=='b')||((*result)->isit=='r')||
	  ((*result)->isit=='l')))){
    	printf("l=%c r=%c\n",newresult->isit,(*result)->isit);
      cout << "*** type mismatch on '&' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 't';
      (*result)->lvalue = ((*result)->lvalue && newresult->lvalue);
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"&&",'l');
    }
    if (!intexpr_E(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case '|':
  case ')':
  case IMPLIES:
  case END_OF_FILE:
    break;
  default:
    printf("ERROR(%c): Expected an &\n",(*token));
    return 0;
  }
  return 1;
}

bool intexpr_D(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  exprsn *newresult;
  //printf("D\n");

  switch (*token) {
  case '|':
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_M(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // cout << "parsing a logical or...\n";
    // cout << "  " << newresult->isit<< "  " <<(*result)->isit<<endl;
    // check for type mismatch
    if (!(((newresult->isit=='t')||(newresult->isit=='b')||(newresult->isit=='r')||
	  (newresult->isit=='l'))&&
	(((*result)->isit=='t')||((*result)->isit=='b')||((*result)->isit=='r')||
	  ((*result)->isit=='l')))){
      cout << "*** type mismatch on '|' operator *** \n";
    }

    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 't';
      (*result)->lvalue = ((*result)->lvalue || newresult->lvalue);
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"||",'l');
    }
    if (!intexpr_D(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  case ')':
  case END_OF_FILE:
    break;
  case IMPLIES:
    (*token)=intexpr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!intexpr_M(token,expr,tokvalue,position,&newresult,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    // check for type mismatch
    if (!(((newresult->isit=='t')||(newresult->isit=='b')||(newresult->isit=='r')||
	  (newresult->isit=='l'))&&
	(((*result)->isit=='t')||((*result)->isit=='b')||((*result)->isit=='r')||
	  ((*result)->isit=='l')))){
      cout << "*** type mismatch on '->' operator *** \n";
    }
    //simplify if operands are static
    if (((newresult->isit=='n')||(newresult->isit=='t'))&&
	(((*result)->isit=='n')||((*result)->isit=='t'))&&
	  ((*result)->lvalue == (*result)->uvalue)&&
	  (newresult->lvalue == newresult->uvalue)&&
	  ((*result)->lvalue != INFIN)&&
	  ((*result)->lvalue != -INFIN)&&
	  (newresult->lvalue != INFIN)&&
	  (newresult->lvalue != -INFIN)){
      (*result)->isit = 't';
      (*result)->lvalue = ((*result)->lvalue || !(newresult->lvalue));
      (*result)->uvalue = (*result)->lvalue;
      delete newresult;
    } else{
      (*result) = new exprsn((*result),newresult,"->",'l');
    }
    if (!intexpr_D(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  default:
    printf("ERROR: Expected an | or ->\n");
    return 0;
  }
  return 1;
}

bool intexpr_M(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  //printf("M\n");
  switch (*token) {
  case WORD:
  case '(':
  case '~':
  case '-':
    if (!intexpr_N(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if (!intexpr_E(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  default:
    printf("M: ERROR: Expected a ID, Number, (, or -\n");
    return 0;
  }
  return 1;
}

bool intexpr_L(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	       exprsn **result, signalADT *signals,int nsignals,
	       eventADT *events,int nevents, int nplaces)
{
  //printf("L\n");
  switch (*token) {
  case WORD:
  case '(':
  case '~':
  case '-':
    if (!intexpr_M(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    if (!intexpr_D(token,expr,tokvalue,position,result,signals,nsignals,
		   events,nevents,nplaces))
      return 0;
    break;
  default:
    printf("L:ERROR: Expected a ID, Number, (, or -\n");
    return 0;
  }
  return 1;
}


/*
 *  Compute the hash function of string. The result should
 *  be an integer number in the range [0, max_size).
 */
int Hash_Get_Map (char *string, int max_size)
{
  int   code = 0;
  char *ptr;

  for (ptr = string;  *ptr != 0;  ptr ++) code += *ptr;
  return (code % max_size);
}

void Print_Name (Node *node)
{
    printf ("[%d]%s ", Node_Num(node), Node_Name(node) );
}

void Print_Marking (marking_list ml)
{
    mark_elem *mk;

    printf ("Marking List:\n");
    for (mk=ml; mk != NULL; mk=Mark_Next(mk) ){
	if ( Mark_Type(mk) == EDGE_MARK ) {
	    Print_Name(Mark_Src(mk));
	    printf(" -> ");
	    Print_Name(Mark_Des(mk));
	    printf("\n");
	} else {
	    Print_Name(Mark_Place(mk));
	    printf("\n");
	}
    }
}

void Print_Node_Simple (Node *node)
{
/*    printf ("%s : ", node_type_table[Node_Type(node)]);
*/

    switch ( Node_Type(node) ) {
      case INPUT_NODE:
      case OUTPUT_NODE:
      case INTERNAL_NODE:
 	printf ("[%d]%s \n", Node_Num(node), Node_Name(node));
	break;
      case PLACE_NODE:
      case TRANSITION_NODE:
	printf ("%s ", Node_Name(node));
	/* for ( i=0; i<Node_Num_of_Succ(node); i++)
	    Print_Name (Node_Succs(node)[i]);
	printf (" <- ");
	for ( i=0; i<Node_Num_of_Pred(node); i++) {
	    Print_Name (Node_Preds(node)[i]);
	}  */
	break;
    }
}


void Print_Node (Node *node)
{
    int i;

/*    printf ("%s : ", node_type_table[Node_Type(node)]);
*/

    switch ( Node_Type(node) ) {
      case INPUT_NODE:
      case OUTPUT_NODE:
      case INTERNAL_NODE:
 	printf ("[%d]%s \n", Node_Num(node), Node_Name(node));
	break;
      case PLACE_NODE:
      case TRANSITION_NODE:
	printf ("[%d]%s -> ", Node_Num(node), Node_Name(node));
	for ( i=0; i<Node_Num_of_Succ(node); i++)
	    Print_Name (Node_Succs(node)[i]);
	printf (" <- ");
	for ( i=0; i<Node_Num_of_Pred(node); i++) {
	    Print_Name (Node_Preds(node)[i]);
	}
	printf("\n");
	break;
    }
}

int event_compare(char *e1,char *e2)
{
  if (strchr(e1,'/') && strchr(e2,'/') &&
      ((strchr(e1,'/')-e1)==(strchr(e2,'/')-e2)))
    if (strncmp(e1,e2,strchr(e1,'/')-e1-1)==0)
      if (strcmp(strchr(e1,'/')+1,strchr(e2,'/')+1)!=0)
	return strcmp(strchr(e1,'/')+1,strchr(e2,'/')+1);
  return strcmp(e1,e2);
}

void update_events(signalADT *signals,eventADT *events,int l,char *event,
		   Name_Ptr name,int type)
{
  events[l]->event=CopyString(event);
  if (name) {
    if (strcmp(event,Node_Name(Name_Node(name)))==0)
      Node_Position(Name_Node(name))=l;
    else
      events[l]->dropped=true;
    if ((Node_Signal_Ptr(Name_Node(name))) &&
	!(Node_Abstract(Node_Signal_Ptr(Name_Node(name))))) {
      events[l]->signal=Node_Position(Node_Signal_Ptr(Name_Node(name)));
      if ((l > 0) && (events[l-1]->signal != events[l]->signal))
	signals[events[l]->signal]->event=l;
    } else
      events[l]->signal=(-1);
    events[l]->type=type;
  } else
    events[l]->signal=(-1);
  if (name) {
    events[l]->failtrans=Node_FailTrans(Name_Node(name));
    events[l]->nondisabling=Node_NonDisabling(Name_Node(name));
    events[l]->rate=Node_Trate(Name_Node(name));
    events[l]->lrate=Node_Lrate(Name_Node(name));
    events[l]->urate=Node_Urate(Name_Node(name));
    events[l]->lower=Node_Ldelay(Name_Node(name));
    events[l]->upper=Node_Udelay(Name_Node(name));
    events[l]->lrange=Node_LRange(Name_Node(name));
    events[l]->urange=Node_URange(Name_Node(name));
    events[l]->hsl=CopyString(Node_Hsl(Name_Node(name)));
    events[l]->transRate=CopyString(Node_TransRate(Name_Node(name)));
    events[l]->delayExpr=CopyString(Node_DelayExpr(Name_Node(name)));
    events[l]->priorityExpr=CopyString(Node_PriorityExpr(Name_Node(name)));
    events[l]->EXP=NULL;
    events[l]->delayExprTree=NULL;
    events[l]->inequalities=Node_Ineq(Name_Node(name));
  } else
    events[l]->rate=1;
}

void Print_Events(FILE *fp,bool count,int *nevents,int *ninputs,
		  bool extract,eventADT *events,signalADT *signals)
{
  char **Events;
  int *signum;
  int *types;
  int i,j,k,l;
  Name_Ptr name_ptr;
  char *temp;
  int tempint,temptype;
  Name_Ptr tempname;
  Name_Ptr *names;

  l=1;
  Events=(char**)GetBlock(num_of_node * sizeof(char*));
  signum=(int*)GetBlock(num_of_node * sizeof(int));
  types=(int*)GetBlock(num_of_node * sizeof(int));
  names=(Name_Ptr*)GetBlock(num_of_node * sizeof(Name_Ptr));
  for (i=0;i<num_of_node;i++) {
    Events[i]=(char*)GetBlock(256 * sizeof(char));
    signum[i]=0;
    types[i]=0;
    names[i]=NULL;
  }
  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	(Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))==INPUT_NODE) &&
	(!(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr)))))) {
      strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
      if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP | IN;
      if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | ABSTRAC;
      if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | NONINP;
      signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
      names[j]=name_ptr;
      j++;
    }
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr)) {
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	  (Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))==INPUT_NODE) &&
	  (!(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr)))))) {
	strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
	if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP|IN;
	if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | ABSTRAC;
	if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | NONINP;
	signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
	names[j]=name_ptr;
	j++;
      }
    }
  }
  for (k=j;k>0;k--)
    for (i=1;i<k;i++)
      if ((signum[i-1] > signum[i]) ||
	  ((signum[i-1]==signum[i]) &&
	   (event_compare(Events[i-1],Events[i]) > 0))) {
	//      if (event_compare(Events[i-1],Events[i]) > 0) {
	temp=Events[i-1];
	tempint=signum[i-1];
	temptype=types[i-1];
	tempname=names[i-1];
	Events[i-1]=Events[i];
	types[i-1]=types[i];
	signum[i-1]=signum[i];
	names[i-1]=names[i];
	Events[i]=temp;
	types[i]=temptype;
	signum[i]=tempint;
	names[i]=tempname;
      }
  for (i=0;i<j;i++) {
    if ((strchr(Events[i],'/')) && (strchr(Events[i],'+'))) {
      if (count) {
	(*nevents)++;
	(*ninputs)++;
      } else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
      if ((i < (j-1)) &&
         (strncmp(Events[i],Events[i+1],strchr(Events[i],'/')-Events[i]-1)==0)
	  && (strchr(Events[i+1],'/')==NULL)) {
	if (extract) {
	  update_events(signals,events,l,Events[i+1],names[i+1],types[i+1]);
	  l++;
	} else if (!count)
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i+1],types[i+1]);
	  else
	    fprintf(fp,"%s\n",Events[i+1]);
      } else if (!((i < (j-1)) &&
	  (strncmp(Events[i],Events[i+1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i+1],'/')+1)==0))){
	temp=strchr(Events[i],'+');
	(*temp)='-';
	if (count) {
	  (*nevents)++;
	  (*ninputs)++;
	} else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
	(*temp)='+';
      }
    } else if ((strchr(Events[i],'/')) && (strchr(Events[i],'-'))) {
      if ((i < (j-1)) &&
         (strncmp(Events[i],Events[i-1],strchr(Events[i],'/')-Events[i]-1)==0)
	  && (strchr(Events[i-1],'/')==NULL)) {
	if (extract) {
	  update_events(signals,events,l,Events[i-1],names[i-1],types[i-1]);
	  l++;
	} else if (!count)
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i-1],types[i-1]);
	else
	  fprintf(fp,"%s\n",Events[i-1]);
      }	else if (!((i > 0) &&
	  (strncmp(Events[i],Events[i-1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i-1],'/')+1)==0))){
	temp=strchr(Events[i],'-');
	(*temp)='+';
	if (count) {
	  (*nevents)++;
	  (*ninputs)++;
	} else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
	(*temp)='-';
      }
      if (count) {
	(*nevents)++;
	(*ninputs)++;
      } else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
    } else
      if (count) {
	(*nevents)++;
	(*ninputs)++;
      } else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
  }
  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	(Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))==OUTPUT_NODE) &&
	(!(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr)))))) {
      strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
      if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP|OUT;
      if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | NONINP;
      if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | ABSTRAC;
      signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
      names[j]=name_ptr;
      j++;
    }
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr)) {
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	  (Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))==OUTPUT_NODE) &&
	  (!(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr)))))) {
	strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
	if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP|OUT;
	if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | ABSTRAC;
	if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | NONINP;
	signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
	names[j]=name_ptr;
	j++;
      }
    }
  }
  for (k=j;k>0;k--)
    for (i=1;i<k;i++)
      if ((signum[i-1] > signum[i]) ||
	  ((signum[i-1]==signum[i]) &&
	   (event_compare(Events[i-1],Events[i]) > 0))) {
	//      if (event_compare(Events[i-1],Events[i]) > 0) {
	temp=Events[i-1];
	tempint=signum[i-1];
	temptype=types[i-1];
	tempname=names[i-1];
	Events[i-1]=Events[i];
	types[i-1]=types[i];
	signum[i-1]=signum[i];
	names[i-1]=names[i];
	Events[i]=temp;
	types[i]=temptype;
	signum[i]=tempint;
	names[i]=tempname;
      }
  for (i=0;i<j;i++) {
    if ((strchr(Events[i],'/')) && (strchr(Events[i],'+'))) {
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
      if (!((i < (j-1)) &&
	  (strncmp(Events[i],Events[i+1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i+1],'/')+1)==0))){
	temp=strchr(Events[i],'+');
	(*temp)='-';
	if (count)
	  (*nevents)++;
	else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i],types[i]);
	  else
	    fprintf(fp,"%s\n",Events[i]);
	(*temp)='+';
      }
    } else if ((strchr(Events[i],'/')) && (strchr(Events[i],'-'))) {
      if (!((i > 0) &&
	  (strncmp(Events[i],Events[i-1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i-1],'/')+1)==0))){
	temp=strchr(Events[i],'-');
	(*temp)='+';
	if (count)
	  (*nevents)++;
	else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i],types[i]);
	  else
	    fprintf(fp,"%s\n",Events[i]);
	(*temp)='-';
      }
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
    } else
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
  }
  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	(Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))==INTERNAL_NODE) &&
	(!(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr)))))) {
      strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
      if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP|OUT;
      if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | ABSTRAC;
      if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | NONINP;
      signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
      names[j]=name_ptr;
      j++;
    }
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr)) {
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	  (Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))==INTERNAL_NODE) &&
	  (!(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr)))))) {
	strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
	if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP|OUT;
	if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | ABSTRAC;
	if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | NONINP;
	signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
	names[j]=name_ptr;
	j++;
      }
    }
  }
  for (k=j;k>0;k--)
    for (i=1;i<k;i++)
      if ((signum[i-1] > signum[i]) ||
	  ((signum[i-1]==signum[i]) &&
	   (event_compare(Events[i-1],Events[i]) > 0))) {
	//      if (event_compare(Events[i-1],Events[i]) > 0) {
	temp=Events[i-1];
	tempint=signum[i-1];
	temptype=types[i-1];
	tempname=names[i-1];
	Events[i-1]=Events[i];
	types[i-1]=types[i];
	signum[i-1]=signum[i];
	names[i-1]=names[i];
	Events[i]=temp;
	types[i]=temptype;
	signum[i]=tempint;
	names[i]=tempname;
      }
  for (i=0;i<j;i++) {
    if ((strchr(Events[i],'/')) && (strchr(Events[i],'+'))) {
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
      if (!((i < (j-1)) &&
	  (strncmp(Events[i],Events[i+1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i+1],'/')+1)==0))){
	temp=strchr(Events[i],'+');
	(*temp)='-';
	if (count)
	  (*nevents)++;
	else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i],types[i]);
	  else
	    fprintf(fp,"%s\n",Events[i]);
	(*temp)='+';
      }
    } else if ((strchr(Events[i],'/')) && (strchr(Events[i],'-'))) {
      if (!((i > 0) &&
	  (strncmp(Events[i],Events[i-1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i-1],'/')+1)==0))){
	temp=strchr(Events[i],'-');
	(*temp)='+';
	if (count)
	  (*nevents)++;
	else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i],types[i]);
	  else
	    fprintf(fp,"%s\n",Events[i]);
	(*temp)='-';
      }
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
    } else
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
  }
  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	(Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))!=DUMMY_NODE) &&
	(Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))) {
      strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
      if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP;
      types[j]=types[j] | DUMMY;
      if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | ABSTRAC;
      if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	types[j]=types[j] | NONINP;
      signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
      names[j]=name_ptr;
      j++;
    }
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr)) {
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) &&
	  (Node_Type(Node_Signal_Ptr(Name_Node(name_ptr)))!=DUMMY_NODE) &&
	  (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))) {
	strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
	if (Node_Keep(Node_Signal_Ptr(Name_Node(name_ptr)))) types[j]=KEEP;
	types[j]=types[j] | DUMMY;
	if (Node_Abstract(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | ABSTRAC;
	if (Node_NonInp(Node_Signal_Ptr(Name_Node(name_ptr))))
	  types[j]=types[j] | NONINP;
	signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
	names[j]=name_ptr;
	j++;
      }
    }
  }
  for (k=j;k>0;k--)
    for (i=1;i<k;i++)
      if ((signum[i-1] > signum[i]) ||
	  ((signum[i-1]==signum[i]) &&
	   (event_compare(Events[i-1],Events[i]) > 0))) {
	//      if (event_compare(Events[i-1],Events[i]) > 0) {
	temp=Events[i-1];
	tempint=signum[i-1];
	temptype=types[i-1];
	tempname=names[i-1];
	Events[i-1]=Events[i];
	types[i-1]=types[i];
	signum[i-1]=signum[i];
	names[i-1]=names[i];
	Events[i]=temp;
	types[i]=temptype;
	signum[i]=tempint;
	names[i]=tempname;
      }
  for (i=0;i<j;i++) {
    if ((strchr(Events[i],'/')) && (strchr(Events[i],'+'))) {
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
      if (!((i < (j-1)) &&
	  (strncmp(Events[i],Events[i+1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i+1],'/')+1)==0))){
	temp=strchr(Events[i],'+');
	(*temp)='-';
	if (count)
	  (*nevents)++;
	else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i],types[i]);
	  else
	    fprintf(fp,"%s\n",Events[i]);
	(*temp)='+';
      }
    } else if ((strchr(Events[i],'/')) && (strchr(Events[i],'-'))) {
      if (!((i > 0) &&
	  (strncmp(Events[i],Events[i-1],strchr(Events[i],'/')-Events[i]-1)==0)
          && (strcmp(strchr(Events[i],'/')+1,strchr(Events[i-1],'/')+1)==0))){
	temp=strchr(Events[i],'-');
	(*temp)='+';
	if (count)
	  (*nevents)++;
	else if (extract) {
	  update_events(signals,events,l,Events[i],names[i],types[i]);
	  l++;
	} else
	  if (types[i]>0)
	    fprintf(fp,"%s:%d\n",Events[i],types[i]);
	  else
	    fprintf(fp,"%s\n",Events[i]);
	(*temp)='-';
      }
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
    } else
      if (count)
	(*nevents)++;
      else if (extract) {
	update_events(signals,events,l,Events[i],names[i],types[i]);
	l++;
      } else
	if (types[i]>0)
	  fprintf(fp,"%s:%d\n",Events[i],types[i]);
	else
	  fprintf(fp,"%s\n",Events[i]);
  }
  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if (Node_Type(Name_Node(name_ptr))==DUMMY_NODE) {
      strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
      types[j]=DUMMY;
      if (Node_Keep(Name_Node(name_ptr))) types[j]=types[j]|KEEP;
      if (Node_Abstract(Name_Node(name_ptr))) types[j]=types[j] | ABSTRAC;
      if (Node_NonInp(Name_Node(name_ptr))) types[j]=types[j] | NONINP;
      if (Node_Cont(Name_Node(name_ptr))) types[j]=types[j] | CONT;
      if (Node_FailTrans(Name_Node(name_ptr))) types[j]=types[j] | FAILTRAN;
      if (Node_NonDisabling(Name_Node(name_ptr))) types[j]=types[j] | NONDISABLE;
      names[j]=name_ptr;
      j++;
    }
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      if (Node_Type(Name_Node(name_ptr))==DUMMY_NODE) {
	strcpy(Events[j],Node_Name(Name_Node(name_ptr)));
	types[j]=DUMMY;
	if (Node_Keep(Name_Node(name_ptr))) types[j]=types[j]|KEEP;
	if (Node_Abstract(Name_Node(name_ptr))) types[j]=types[j] | ABSTRAC;
	if (Node_NonInp(Name_Node(name_ptr))) types[j]=types[j] | NONINP;
	if (Node_Cont(Name_Node(name_ptr))) types[j]=types[j] | CONT;
	if (Node_FailTrans(Name_Node(name_ptr))) types[j]=types[j] | FAILTRAN;
	if (Node_NonDisabling(Name_Node(name_ptr))) types[j]=types[j] | NONDISABLE;
	names[j]=name_ptr;
	j++;
      }
  }
  for (i=0;i<j;i++) {
    if (count)
      (*nevents)++;
    else if (extract) {
      update_events(signals,events,l,Events[i],names[i],types[i]);
      l++;
    } else
      if (types[i]>0)
	fprintf(fp,"$%s:%d\n",Events[i],types[i]);
      else
	fprintf(fp,"$%s\n",Events[i]);
  }

  for (i=0;i<num_of_node;i++)
    free(Events[i]);
  free(Events);
  free(signum);
  free(names);
  free(types);
}

void Print_Merges(FILE *fp,bool count,int *nmerges)
{
  char **events;
  int *signum;
  int i,j,k,cursig,tempint;
  Name_Ptr name_ptr;
  char *temp;

  events=(char**)GetBlock(num_of_node * sizeof(char*));
  signum=(int*)GetBlock(num_of_node * sizeof(int));
  for (i=0;i<num_of_node;i++)
    events[i]=(char*)GetBlock(256 * sizeof(char));
  for (i=0;i<num_of_node;i++)
    signum[i]=0;
  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if (Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) {
      strcpy(events[j],Node_Name(Name_Node(name_ptr)));
      signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
      j++;
    }
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      if (Node_Type(Name_Node(name_ptr))==TRANSITION_NODE) {
	strcpy(events[j],Node_Name(Name_Node(name_ptr)));
	signum[j]=Node_Num(Node_Signal_Ptr(Name_Node(name_ptr)));
	j++;
      }
  }
  for (k=j;k>0;k--)
    for (i=1;i<k;i++)
      if ((signum[i-1] > signum[i]) ||
	  ((signum[i-1]==signum[i]) &&
	   (event_compare(events[i-1],events[i]) > 0))) {
	//      if (event_compare(events[i-1],events[i]) > 0) {
	temp=events[i-1];
	tempint=signum[i-1];
	events[i-1]=events[i];
	signum[i-1]=signum[i];
	events[i]=temp;
	signum[i]=tempint;
      }

  cursig=(-1);
  for (i=0;i<j;i++)
    if ((cursig==signum[i]) || (((i+2)<j) && (signum[i]==signum[i+2]))) {
      if ((strchr(events[i],'/')) && (strchr(events[i],'+'))) {
	if (signum[i]!=cursig) {
	  if ((!count) && (cursig >= 0)) fprintf(fp,"\n");
	  cursig=signum[i];
	  if (count) (*nmerges)++;
	}
	if (!count)
	  fprintf(fp,"%s ",events[i]);
      } else if ((strchr(events[i],'/')) && (strchr(events[i],'-'))) {
	if ((i < (j-1)) &&
	    (strncmp(events[i],events[i-1],strchr(events[i],'/')-events[i]-1)
	     ==0) && (strchr(events[i-1],'/')==NULL))
	  ;
	else if (!((i > 0) &&
		   (strncmp(events[i],events[i-1],strchr(events[i],'/')-
			    events[i]-1)==0)
		   && (strcmp(strchr(events[i],'/')+1,
			      strchr(events[i-1],'/')+1)==0))){
	  temp=strchr(events[i],'-');
	  (*temp)='+';

	  if (signum[i]!=cursig) {
	    if ((!count) && (cursig >= 0)) fprintf(fp,"\n");
	    cursig=signum[i];
	    if (count) (*nmerges)++;
	  }
	  if (!count)
	    fprintf(fp,"%s ",events[i]);
	  (*temp)='-';
	}
      }
    }
  /*
  cursig=(-1);
  for (i=0;i<j;i++) {
    if ((strchr(events[i],'/')) && (strchr(events[i],'+')) &&
	((cursig==signum[i]) || (((i+2)<j) && (signum[i]==signum[i+2])))) {
      if (signum[i]!=cursig) {
	if ((!count) && (cursig >= 0)) fprintf(fp,"\n");
	cursig=signum[i];
	if (count) (*nmerges)++;
      }
      if (!count)
	fprintf(fp,"%s ",events[i]);
    }
  }
  */
  if ((!count) && (cursig >= 0)) fprintf(fp,"\n");
  for (i=0;i<num_of_node;i++)
    free(events[i]);
  free(events);
  free(signum);
}

int Count_Rules(FILE *fp,Node *node,int count)
{
  int i;

  switch ( Node_Type(node) ) {
  case TRANSITION_NODE:
    for ( i=0; i<Node_Num_of_Succ(node); i++)
      count++;
    break;
  }
  return count;
}

void PrintConflict(FILE *fp,Node *node1,Node *node2)
{
  if ((Node_Type(node1)!=DUMMY_NODE) && (Node_Type(node2)!=DUMMY_NODE))
    fprintf(fp,"%s %s\n",Node_Name(node1),Node_Name(node2));
  else if ((Node_Type(node1)!=DUMMY_NODE) && (Node_Type(node2)==DUMMY_NODE))
    fprintf(fp,"%s $%s\n",Node_Name(node1),Node_Name(node2));
  else if ((Node_Type(node1)==DUMMY_NODE) && (Node_Type(node2)!=DUMMY_NODE))
    fprintf(fp,"$%s %s\n",Node_Name(node1),Node_Name(node2));
  else
    fprintf(fp,"$%s $%s\n",Node_Name(node1),Node_Name(node2));
}

void Print_Conflicts(FILE *fp,Node *node,bool count,int *nconflicts)
{
  int i,j;

  switch ( Node_Type(node) ) {
  case PLACE_NODE:
    for ( i=0; i<Node_Num_of_Succ(node); i++)
      for ( j=i+1; j<Node_Num_of_Succ(node); j++)
	if (count)
	  (*nconflicts)++;
	else
	  PrintConflict(fp,Node_Succs(node)[i],Node_Succs(node)[j]);
    for ( i=0; i<Node_Num_of_Pred(node); i++)
      for ( j=i+1; j<Node_Num_of_Pred(node); j++)
	if (count)
	  (*nconflicts)++;
	else
	  PrintConflict(fp,Node_Preds(node)[i],Node_Preds(node)[j]);
    break;
  }
}

void Print_Input_Conflicts(FILE *fp,Node *node,bool count,int *nconflicts)
{
  int i,j;

  switch ( Node_Type(node) ) {
  case PLACE_NODE:
    for ( i=0; i<Node_Num_of_Pred(node); i++)
      for ( j=i+1; j<Node_Num_of_Pred(node); j++)
	if (count)
	  (*nconflicts)++;
	else
	  PrintConflict(fp,Node_Preds(node)[i],Node_Preds(node)[j]);
    break;
  }
}

void Print_Output_Conflicts(FILE *fp,Node *node,bool count,int *nconflicts)
{
  int i,j;

  switch ( Node_Type(node) ) {
  case PLACE_NODE:
    for ( i=0; i<Node_Num_of_Succ(node); i++)
      for ( j=i+1; j<Node_Num_of_Succ(node); j++)
	if (count)
	  (*nconflicts)++;
	else
	  PrintConflict(fp,Node_Succs(node)[i],Node_Succs(node)[j]);
    break;
  }
}

void PrintRule(FILE *fp,Node *node1,Node *node2,char *expr,bool disabling,
	       int eps,int l,int u,double rate)
{
  if ((Node_Type(node1)!=DUMMY_NODE) && (Node_Type(node2)!=DUMMY_NODE))
    fprintf(fp,"%s %s ",Node_Name(node1),Node_Name(node2));
  else if ((Node_Type(node1)!=DUMMY_NODE) && (Node_Type(node2)==DUMMY_NODE))
    fprintf(fp,"%s $%s ",Node_Name(node1),Node_Name(node2));
  else if ((Node_Type(node1)==DUMMY_NODE) && (Node_Type(node2)!=DUMMY_NODE))
    fprintf(fp,"$%s %s ",Node_Name(node1),Node_Name(node2));
  else
    fprintf(fp,"$%s $%s ",Node_Name(node1),Node_Name(node2));
  if (expr)
    fprintf(fp,"[%s]",expr);
  if (disabling)
    fprintf(fp,"d");
  if (u==INFIN)
    fprintf(fp," %d %d inf",eps,l);
  else
    fprintf(fp," %d %d %d",eps,l,u);
  if (rate != 1.0)
    fprintf(fp," S(%f)",rate);
  fprintf(fp,"\n");
}

bool Print_Rule(FILE *fp,Node *node,bool count,int *nrules,bool ordering)
{
  int i,j;
  mark_elem *mk;
  bool marked;

  switch ( Node_Type(node) ) {
  case PLACE_NODE:
    break;
  case VAR_NODE:
    break;
  case TRANSITION_NODE:
  case DUMMY_NODE:
    for ( i=0; i<Node_Num_of_Succ(node); i++) {
      if ((Node_Type(Node_Succs(node)[i])==TRANSITION_NODE) ||
	  (Node_Type(Node_Succs(node)[i])==DUMMY_NODE)) {
	if ((!ordering) && (Node_RuleType(node)[i]) &&
	    (strcmp(Node_RuleType(node)[i],"C")==0)) continue;
	if ((ordering) && (!(Node_RuleType(node)[i]) ||
	    (strcmp(Node_RuleType(node)[i],"C")!=0))) continue;
	marked=FALSE;
	for (mk=initial_marking; mk != NULL && !marked; mk=Mark_Next(mk))
	  if ( Mark_Type(mk) == EDGE_MARK )
	    if ((Mark_Src(mk)==node) &&
		(Mark_Des(mk)==Node_Succs(node)[i])) marked=TRUE;
	if (marked)
	  if (count)
	    (*nrules)++;
	  else
	    PrintRule(fp,node,Node_Succs(node)[i],Node_Expr(node)[i],
		      Node_Disabling(node)[i],1,Node_Lower(node)[i],
		      Node_Upper(node)[i],Node_Rate(node)[i]);
	else
	  if (count)
	    (*nrules)++;
	  else
	    PrintRule(fp,node,Node_Succs(node)[i],Node_Expr(node)[i],
		      Node_Disabling(node)[i],0,
		      Node_Lower(node)[i],Node_Upper(node)[i],
		      Node_Rate(node)[i]);
      } else {
	for ( j=0; j<Node_Num_of_Succ(Node_Succs(node)[i]); j++) {
	  if (!ordering && Node_RuleType(Node_Succs(node)[i])[j] &&
	      strcmp(Node_RuleType(Node_Succs(node)[i])[j],"C")==0) continue;
	  if (ordering && (!Node_RuleType(Node_Succs(node)[i])[j] ||
	      strcmp(Node_RuleType(Node_Succs(node)[i])[j],"C")!=0)) continue;
	  marked=FALSE;
	  for (mk=initial_marking; mk != NULL && !marked; mk=Mark_Next(mk))
	    if ( Mark_Type(mk) != EDGE_MARK ) {
	      if ((Mark_Place(mk)==Node_Succs(node)[i])) marked=TRUE;
	    } else
	      if ((Mark_Src(mk)==node) &&
		  (Mark_Des(mk)==Node_Succs(Node_Succs(node)[i])[j]))
		marked=TRUE;
	  if (marked)
	    if (count)
	      (*nrules)++;
	    else
	      PrintRule(fp,node,Node_Succs(Node_Succs(node)[i])[j],
			Node_Expr(Node_Succs(node)[i])[j],
			Node_Disabling(node)[i],1,
			Node_Lower(Node_Succs(node)[i])[j],
			Node_Upper(Node_Succs(node)[i])[j],
			Node_Rate(Node_Succs(node)[i])[j]);
	  else
	    if (count)
	      (*nrules)++;
	    else
	      PrintRule(fp,node,Node_Succs(Node_Succs(node)[i])[j],
			Node_Expr(Node_Succs(node)[i])[j],
			Node_Disabling(node)[i],0,
			Node_Lower(Node_Succs(node)[i])[j],
			Node_Upper(Node_Succs(node)[i])[j],
			Node_Rate(Node_Succs(node)[i])[j]);
	}
      }
    }
    break;
  }
  return TRUE;
}

mark_elem *New_Mark ()
{
    mark_elem *tmp;

    tmp = (mark_elem*) GetBlock (sizeof(mark_elem));

    Mark_Src (tmp) = NULL;
    Mark_Des (tmp) = NULL;
    Mark_Next(tmp) = NULL;
    Mark_State(tmp) = NULL;

    return tmp;
}

void Delete_Mark (mark_elem *tmp)
{
  if (tmp) {
    if (Mark_State(tmp)) free(Mark_State(tmp));
    free(tmp);
  }
}

void Enter_Mark (marking_list *ml, int type, Node *src, Node *des,
		 int markl, int marku)
{
    mark_elem *tmp;

    tmp = New_Mark ();

    Mark_Type (tmp) = type;
    Mark_Markl (tmp) = markl;
    Mark_Marku (tmp) = marku;

    if ( src != NULL ) Mark_Src (tmp) = src;
    else Mark_Src (tmp) = des;
    Mark_Des (tmp) = des;

    if ( *ml == NULL )
	*ml = tmp;
    else {
	Mark_Next (tmp) = *ml;
	*ml = tmp;
    }
}

marking_list Dup_Mark_List (marking_list ml)
{
  int i;
  marking_list newmark, tmp;

  if ( ml == NULL ) return NULL;
  tmp = newmark = New_Mark ();

  while (ml != NULL ) {
    Mark_Type(tmp) = Mark_Type(ml);
    Mark_Src(tmp) = Mark_Src(ml);
    Mark_Des(tmp) = Mark_Des(ml);
    if (Mark_State(ml)) {
      Mark_State(tmp) = (int *) GetBlock (sizeof(int *) * num_of_ios);
      for (i = 0; i < num_of_ios; i++) {
	Mark_State(tmp)[i] = Mark_State(ml)[i];
      }
    } else Mark_State(tmp)=NULL;
    ml = Mark_Next(ml);
    if ( ml == NULL ) Mark_Next(tmp) = NULL;
    else {
      Mark_Next(tmp) = New_Mark();
      tmp = Mark_Next(tmp);
    }
  }
  return newmark;
}

void Free_Mark_List (marking_list ml)
{
    marking_list tmp, ptr;

    tmp = ptr = ml;

    while (ptr!=NULL){
	tmp = Mark_Next(tmp);
	Delete_Mark(ptr);
//	free (ptr);
	ptr = tmp;
    }

}

/*
void Free_lml(lml *head)
{
  lml *curlml;
  lml *nextlml;

  curlml=head;
  while (curlml) {
    nextlml=curlml->next;
    Delete_Mark(curlml->ml);
    Delete_Mark(curlml->eventlist);
    free(curlml);
    curlml=nextlml;
  }
}
*/

void Free_lml(lml *curlml)
{
  Free_Mark_List(curlml->ml);
//  Free_Mark_List(curlml->eventlist);
  free(curlml);
}

void Enter_Pred (Node *pred, Node *succ)
{
  if (!succ) return;
  Node_Preds (succ) = (Node**) Realloc (Node_Preds(succ),
				sizeof(Node*)*(Node_Num_of_Pred(succ)+1));
  Node_Preds(succ)[Node_Num_of_Pred(succ)++] = pred;

}

void Enter_Succ (Node *pred, Node *succ, int l, int u, int pt, int pl, int pu,
		 char *expr, char *hsl, char *ruletype, bool disabling,
		 double rate,int weight)
{
  if (!pred) return;


  Node_Succs (pred) = (Node**) Realloc (Node_Succs(pred),
			sizeof(Node*)*(Node_Num_of_Succ(pred)+1));
  Node_Lower (pred) = (int*) Realloc (Node_Lower (pred),
			sizeof(int)*(Node_Num_of_Succ(pred)+1));
  Node_Upper (pred) = (int*) Realloc (Node_Upper (pred),
			sizeof(int)*(Node_Num_of_Succ(pred)+1));
  Node_PredType (pred) = (int*) Realloc (Node_PredType (pred),
			sizeof(int)*(Node_Num_of_Succ(pred)+1));
  Node_Plower (pred) = (int*) Realloc (Node_Plower (pred),
			sizeof(int)*(Node_Num_of_Succ(pred)+1));
  Node_Pupper (pred) = (int*) Realloc (Node_Pupper (pred),
			sizeof(int)*(Node_Num_of_Succ(pred)+1));
  Node_Weight (pred) = (int*) Realloc (Node_Weight (pred),
			sizeof(int)*(Node_Num_of_Succ(pred)+1));
  Node_Expr (pred) = (char**) Realloc (Node_Expr (pred),
			sizeof(char*)*(Node_Num_of_Succ(pred)+1));
  //Node_TransRate (pred) = (char**) Realloc (Node_TransRate (pred),
  //			sizeof(char*)*(Node_Num_of_Succ(pred)+1));
  //Node_Hsl (pred) = (char**) Realloc (Node_Hsl (pred),
  //			sizeof(char*)*(Node_Num_of_Succ(pred)+1));
  Node_RuleType (pred) = (char**) Realloc (Node_RuleType (pred),
			sizeof(char*)*(Node_Num_of_Succ(pred)+1));
  Node_Disabling (pred) = (bool*) Realloc (Node_Disabling (pred),
			sizeof(bool)*(Node_Num_of_Succ(pred)+1));
  Node_Rate (pred) = (double*) Realloc (Node_Rate (pred),
         	        sizeof(double)*(Node_Num_of_Succ(pred)+1));
  Node_Lower(pred)[Node_Num_of_Succ(pred)] = l;
  Node_Upper(pred)[Node_Num_of_Succ(pred)] = u;
  Node_PredType(pred)[Node_Num_of_Succ(pred)] = pt;
  Node_Plower(pred)[Node_Num_of_Succ(pred)] = pl;
  Node_Pupper(pred)[Node_Num_of_Succ(pred)] = pu;
  Node_Weight(pred)[Node_Num_of_Succ(pred)] = weight;
  Node_Expr(pred)[Node_Num_of_Succ(pred)] = expr;
  //Node_TransRate(pred)[Node_Num_of_Succ(pred)] = transRate;
  //Node_Hsl(pred)[Node_Num_of_Succ(pred)] = hsl;
  Node_RuleType(pred)[Node_Num_of_Succ(pred)] = ruletype;
  Node_Disabling(pred)[Node_Num_of_Succ(pred)] = disabling;
  Node_Rate(pred)[Node_Num_of_Succ(pred)] = rate;
  Node_Succs(pred)[Node_Num_of_Succ(pred)++] = succ;
}

Node *New_Node (int type, char *name, int id)
{
    Node_Ptr ptr;

    ptr = (Node_Ptr) GetBlock (sizeof(Node));

    Node_Type (ptr) = type;
    Node_Name (ptr) = name;
    Node_Succs(ptr) = NULL;
    Node_Preds(ptr) = NULL;
    Node_Lower(ptr) = NULL;
    Node_Upper(ptr) = NULL;
    Node_PredType(ptr) = NULL;
    Node_Plower(ptr) = NULL;
    Node_Pupper(ptr) = NULL;
    Node_Weight(ptr) = NULL;
    Node_Expr(ptr) = NULL;
    Node_Hsl(ptr) = NULL;
    Node_TransRate(ptr) = NULL;
    Node_DelayExpr(ptr) = NULL;
    Node_PriorityExpr(ptr) = NULL;
    Node_RuleType(ptr) = NULL;
    Node_Disabling(ptr) = NULL;
    Node_Rate(ptr) = NULL;
    Node_Num_of_Succ (ptr) = 0;
    Node_Num_of_Pred (ptr) = 0;
    Node_Signal_Ptr  (ptr) = NULL;
    Node_Num (ptr) = id;
    Node_Cont(ptr)=false;
    Node_FailTrans(ptr)=false;
    Node_NonDisabling(ptr)=false;
    Node_LRange(ptr)=(-1)*INFIN;
    Node_URange(ptr)=INFIN;
    Node_Trate(ptr)=0;
    Node_Lrate(ptr)=0;
    Node_Urate(ptr)=0;
    Node_Ineq(ptr)=NULL;

    return ptr;
}

Node *Enter_Trans (char *name, char *idx, char dir)
{
    char *str;
    Name_Ptr nptr;
    Node *node;
    int type;

    str = (char*) GetBlock ( strlen(name) + strlen(idx) + 3 );

    strcpy (str, name);

    /*
    strcat (str, idx);
    */
    if ( dir == '+' ) strcat (str, "+");
    else if ( dir == '-' ) strcat (str, "-");

    if (strcmp(idx,"")!=0) {
      strcat(str,"/");
      strcat(str,idx);
    }

    nptr = Name_Find_Str (str);

    if ( nptr != NAME_NULL_PTR ) {
      free(str);
      return Name_Node (nptr);
    }

    nptr = Name_Enter_Str (str);

    node = Name_Node (nptr) = New_Node (TRANSITION_NODE, str, num_of_node++);

    Node_Dir (node) = dir;

    nptr = Name_Find_Str (name);

    if ( nptr == NAME_NULL_PTR ) {
	printf ("Error: the signal %s is not declared\n", name);
	gerror("ERROR");
	free(str);
	return NULL;
    }

    type = Node_Type ( Name_Node (nptr) );
    if ( type != INPUT_NODE && type != OUTPUT_NODE &&
		 type != INTERNAL_NODE && type != DUMMY_NODE) {
	printf ("Error: the signal %s is not a signal\n", name);
	gerror("ERROR");
	free(str);
	return NULL;
    }

    Node_Signal_Ptr (node) = Name_Node(nptr);

    if (Node_Type(Name_Node(nptr))==INPUT_NODE) num_inp_trans++;

    Add_Node (&node_list, node);

    return node;
}


Node *Enter_IO (int type, char *str)
{
  Name_Ptr nptr;

  Node *sig;

  nptr = Name_Find_Str (str);

  if ( nptr != NULL ) {
    printf ("Enter_Signal: duplicate signal names %s\n",str);
    gerror("ERROR");
    return NULL;
  }

  nptr = Name_Enter_Str (str);

  sig = New_Node(type, str, num_of_ios+num_of_dummys);

  if (type == DUMMY_NODE) {
    num_of_dummys++;
    Node_Keep(sig)=false;
    Node_Abstract(sig)=true;
    Node_NonInp(sig)=false;
  } else {
    num_of_ios++;
    Node_Keep(sig)=true;
    Node_Abstract(sig)=false;
    Node_NonInp(sig)=false;
    if (type==INPUT_NODE) num_of_inps++;
    Add_IO (&io_list, sig);
  }

  Name_Node (nptr) = sig;

  return sig;
}

void Enter_FailTrans (char *str)
{
  Name_Ptr nptr;

  nptr = Name_Find_Str (str);

  if ( nptr == NULL ) {
    printf ("transition %s not declared, cannot mark as a failure transition.\n",str);
    gerror("ERROR");
  } else {
    Node_FailTrans(Name_Node(nptr))=true;
    hasFailTrans=true;
  }
}

void Enter_NonDisabling (char *str)
{
  Name_Ptr nptr;

  nptr = Name_Find_Str (str);

  if ( nptr == NULL ) {
    printf ("transition %s not declared, cannot mark as a non-disabling transition.\n",str);
    gerror("ERROR");
  } else {
    Node_NonDisabling(Name_Node(nptr))=true;
    //hasFailTrans=true;
  }
}

void Enter_Keep (char *str)
{
  Name_Ptr nptr;

  nptr = Name_Find_Str (str);

  if ( nptr == NULL ) {
    printf ("signal %s not declared cannot keep.\n",str);
    gerror("ERROR");
  } else {
    Node_Keep(Name_Node(nptr))=true;
    Node_Abstract(Name_Node(nptr))=false;
  }
}

void Enter_Abstract (char *str)
{
  Name_Ptr nptr;

  nptr = Name_Find_Str (str);

  if ( nptr == NULL ) {
    printf ("signal %s not declared cannot abstract.\n",str);
    gerror("ERROR");
  } else if (Node_Abstract(Name_Node(nptr))) {
    printf ("signal %s abstracted twice.\n",str);
    gerror("ERROR");
  } else {
    Node_Abstract(Name_Node(nptr))=true;
    Node_Keep(Name_Node(nptr))=false;
    if (Node_Type(Name_Node(nptr))==INPUT_NODE)
      num_of_inps--;
    num_of_ios--;
  }
}

void Enter_Cont (char *str,int lrange,int urange)
{
  Name_Ptr nptr;

  nptr = Name_Find_Str (str);

  if ( nptr == NULL ) {
    printf ("transition %s not declared cannot make continuous.\n",str);
    gerror("ERROR");
  } else if (Node_Cont(Name_Node(nptr))) {
    printf ("transition %s marked continuous twice.\n",str);
    gerror("ERROR");
  } else {
    Node_Cont(Name_Node(nptr))=true;
    Node_LRange(Name_Node(nptr))=lrange;
    Node_URange(Name_Node(nptr))=urange;
  }
}

void Enter_Trate(Node *node, int lrate, int urate)
{
  Node_Lrate(node)=lrate;
  Node_Urate(node)=urate;
  Node_Trate(node)=lrate;
}

void Enter_Hsl (Node *node, char *str)
{
  //  char* newstring = CopyString(str);
  Node_Hsl(node)= CopyString(str);
}

void Enter_TransRate (Node *node, char *str)
{
  //  char* newstring = CopyString(str);
  Node_TransRate(node)= CopyString(str);
}

void Enter_Delay (Node *node, int lower, int upper)
{
  Node_Ldelay(node)=lower;
  Node_Udelay(node)=upper;
}

void Enter_DelayExpr (Node *node, char *str)
{
  Node_DelayExpr(node)= CopyString(str);
}

void Enter_PriorityExpr (Node *node, char *str)
{
  Node_PriorityExpr(node)= CopyString(str);
}

ineqADT Enter_Inequality(Node *node, int type, int constant, int uconstant,
			 char *expr)
{
  ineqADT curineq;
  curineq=(ineqADT)GetBlock(sizeof(*curineq));
  curineq->place = (intptr_t)node;
  curineq->type = type;
  curineq->constant = constant;
  curineq->uconstant = uconstant;
  curineq->signal = (-1);
  curineq->transition = (-1);
  curineq->next = NULL;
  curineq->expr = expr;
  curineq->tree = NULL;
  return curineq;
}

void Enter_NonInp (char *str)
{
  Name_Ptr nptr;

  nptr = Name_Find_Str (str);

  if ( nptr == NULL ) {
    printf ("signal %s not declared cannot be non-input.\n",str);
    gerror("ERROR");
  } else {
    Node_NonInp(Name_Node(nptr))=true;
  }
}

Node *Enter_Place (char *str, int add)
{
    Name_Ptr nptr;
    Node *node = NULL;

    nptr = Name_Find_Str (str);

    if ( nptr != NAME_NULL_PTR ) {
      num_of_node++;
      free(str);
      return Name_Node (nptr);
    }
    if (add){
      nptr = Name_Enter_Str (str);

      node = New_Node (PLACE_NODE, str, num_of_node++);

      Name_Node (nptr) = node;

      Add_Node (&node_list, node);
    } else {
      printf("INEQUALITY: %s is not a signal!\n",str);
    }

    return node;
}

Node *Enter_Variable (char *str)
{
    Name_Ptr nptr;
    Node *node;


    nptr = Name_Find_Str (str);

    if ( nptr != NAME_NULL_PTR ) {
      num_of_node++;
      if ((Node_Type(Name_Node(nptr))==INPUT_NODE) ||
    		  (Node_Type(Name_Node(nptr))==OUTPUT_NODE) ||
    		  (Node_Type(Name_Node(nptr))==INTERNAL_NODE)) {
    	  num_of_ios--;
      }
      if (Node_Type(Name_Node(nptr))==INPUT_NODE) {
	  num_of_inps--;
      }
      Node_Type(Name_Node(nptr)) = VAR_NODE;
      //free(str);
      return Name_Node (nptr);
    }
    nptr = Name_Enter_Str (str);

    node = New_Node (VAR_NODE, str, num_of_node++);

    Name_Node (nptr) = node;

    Add_Node (&node_list, node);

    return node;
}

void Add_IO (Node **list, Node *node)
{
    Node *ptr;
    if (*list == NULL) {
	*list = node;
	Node_NextL(node) = NULL;
    } else {
	for (ptr=*list; Node_Next(ptr) != NULL; ptr=Node_Next(ptr));
	Node_NextL(ptr) = (char*)node;
	Node_NextL(node) = NULL;
    }
}

void Add_Node (Node **list, Node *node)
{
  if (*list == NULL) {
    *list = node;
    Node_NextL(node) = NULL;
  } else {
    Node_NextL(node) = (char*)*list;
    *list = node;
/*
	for (ptr=*list; Node_Next(ptr) != NULL; ptr=Node_Next(ptr));
	Node_Next(ptr) = node;
	Node_Next (node) = NULL;
*/
  }
}


void Print_IO_List (Node *list)
{
    Node *ptr;
    int i=0;

    for (ptr=list; ptr!=NULL; ptr=Node_Next(ptr))
	printf("[%d]%s ", i++, Node_Name(ptr));
    printf ("\n");
}

void Print_Node_List (Node *list)
{
    Node *ptr;
    int i=0;

    for (ptr=list; ptr!=NULL; ptr=Node_Next(ptr)){
	printf("[%d] ", i++);
	Print_Node (ptr);
    }
    printf ("\n");
}

/*
 *  Name_Table_Init_Table: Initializes the name hash table by
 *       setting all entries to NULL_NAME, then enters all
 *       keywords (upper and lower case) and special literal:
 *       TRUE, FALSE, NIL.
 */
void Name_Init_Table ()

   {int inx;

    for (inx = 0;  inx < NAME_MAX_ENTRIES;  inx ++)
        name_table [inx] = NAME_NULL_PTR;

   }

/* This function searches for string in the name table by hashing its name
 * and then comparing it against all strings in the same chain. If the
 * string is found, the name structure is returned; otherwise it returns
 * NAME_NULL_PTR. variables last_hash_entry and last_name_ptr are used
 * to remember the hash_entry (chain) searched and the last member of
 * the chain. This variables should be used in Name_Enter_Str to avoid
 * searching twice.
 *
 * 		YOU HAVE TO WRITE THIS FUNCTION
 */
Name_Ptr Name_Find_Str (char *string)
{
  Name_Ptr name_ptr;

  last_hash_entry = Hash_Get_Map (string, NAME_MAX_ENTRIES);
  last_name_ptr   = NAME_NULL_PTR;

  for (name_ptr = name_table [last_hash_entry];  name_ptr != NAME_NULL_PTR;
       name_ptr = Name_Next (name_ptr))
    {last_name_ptr = name_ptr;
    if (! strcmp (Name_String (name_ptr), string) )
      return (name_ptr);
    }
  return (NAME_NULL_PTR);
}

/*
 * This function takes a string and a token type and enters them in
 * the name table. Variables last_hash_entry and last_name_ptr should
 * be used to avoid searching again. This function is only called when
 * we know that the string is NOT in the table. A pointer to the new
 * name structure should be returned.
 *
 * 		YOU HAVE TO WRITE THIS FUNCTION
 */
Name_Ptr Name_Enter_Str (char *string)
{
  Name_Ptr tmp;

  tmp = (Name_Ptr) GetBlock (1 * sizeof (Name_Type));
  Name_String (tmp) = string;
  Name_Node   (tmp) = NULL;

  Name_Next   (tmp) = NAME_NULL_PTR;

  if (last_name_ptr == NAME_NULL_PTR)
    {
      name_table [last_hash_entry] = tmp;
    }
  else
    Name_Next (last_name_ptr) = tmp;

  return (tmp);
}

/*
 * This function is called only when we know that a string is not in
 * the name table. It calls Name_Find_Str and Name_Enter_Str.
 */
Name_Ptr Name_Find_Enter_Str (char *string)
{
  Name_Find_Str (string);
  return (Name_Enter_Str (string));
}

void Name_Debug_Graph ()
{
    int i;
    Name_Ptr name_ptr;

    for (i=0; i < NAME_MAX_ENTRIES; i++) {
	if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;

	Print_Node ( Name_Node (name_ptr));

	for ( name_ptr = Name_Next (name_ptr);
	      name_ptr != NAME_NULL_PTR;
	      name_ptr = Name_Next (name_ptr))
	     Print_Node ( Name_Node (name_ptr) );
    }

}

bool Print_Rules(char * filename)
{
  int i,count,nevents,ninputs,nrules,nmerges,norules;
  int nconflicts,niconflicts,noconflicts;
  Name_Ptr name_ptr;
  char outname[FILENAMESIZE];
  FILE *fp=NULL;
  bool okay;

  okay=TRUE;
  strcpy(outname,filename);
  strcat(outname,".er");
  printf("Converting Petri net to ER and storing to:  %s\n",outname);
  fprintf(lg,"Converting Petri net to ER and storing to:  %s\n",outname);
  fp=fopen(outname,"w");
  if (fp==NULL) {
    printf("ERROR: Cannot open file:  %s\n",outname);
    fprintf(lg,"ERROR: Cannot open file:  %s\n",outname);
    return FALSE;
  }
  count=0;
  nrules=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    Print_Rule(fp,Name_Node (name_ptr),TRUE,&nrules,false);
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      Print_Rule(fp,Name_Node (name_ptr),TRUE,&nrules,false);
  }
  norules=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    Print_Rule(fp,Name_Node (name_ptr),TRUE,&norules,true);
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      Print_Rule(fp,Name_Node (name_ptr),TRUE,&norules,true);
  }
  niconflicts=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    Print_Input_Conflicts(fp,Name_Node (name_ptr),TRUE,&niconflicts);
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      Print_Input_Conflicts(fp,Name_Node (name_ptr),TRUE,&niconflicts);
  }
  noconflicts=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    Print_Output_Conflicts(fp,Name_Node (name_ptr),TRUE,&noconflicts);
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      Print_Output_Conflicts(fp,Name_Node (name_ptr),TRUE,&noconflicts);
  }
  nconflicts=0;
  nevents=1;
  ninputs=0;
  nmerges=0;
  Print_Events(fp,TRUE,&nevents,&ninputs,false,NULL,NULL);
  Print_Merges(fp,TRUE,&nmerges);
  fprintf(fp,".e %d\n",nevents);
  fprintf(fp,".i %d\n",ninputs);
  fprintf(fp,".r %d\n",nrules);
  fprintf(fp,".n %d\n",norules);
  fprintf(fp,".d %d\n",nmerges);
  fprintf(fp,".m %d\n",niconflicts);
  fprintf(fp,".o %d\n",noconflicts);
  fprintf(fp,".c %d\n",nconflicts);
  if (initial_state)
    fprintf(fp,".s %s\n",initial_state);
  else
    fprintf(fp,".s 0\n");
  fprintf(fp,"#\n# EVENTS\n#\n");
  fprintf(fp,"reset\n");
  Print_Events(fp,FALSE,&nevents,&ninputs,false,NULL,NULL);
  fprintf(fp,"#\n# RULES\n#\n");
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    if (!Print_Rule (fp,Name_Node (name_ptr),FALSE,&nrules,false)) okay=FALSE;
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      if (!Print_Rule(fp,Name_Node (name_ptr),FALSE,&nrules,false))
	okay=FALSE;
  }
  fprintf(fp,"#\n# ORDERING RULES\n#\n");
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    if (!Print_Rule (fp,Name_Node (name_ptr),FALSE,&norules,true)) okay=FALSE;
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      if (!Print_Rule(fp,Name_Node (name_ptr),FALSE,&norules,true))
	okay=FALSE;
  }
  fprintf(fp,"#\n# MERGES\n#\n");
  Print_Merges(fp,FALSE,&nmerges);
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    Print_Input_Conflicts(fp,Name_Node (name_ptr),FALSE,&nconflicts);
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      Print_Input_Conflicts(fp,Name_Node (name_ptr),FALSE,&nconflicts);

  }
  fprintf(fp,"#\n# CONFLICTS\n#\n");
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ( (name_ptr = name_table[i]) == NAME_NULL_PTR ) continue;
    Print_Output_Conflicts(fp,Name_Node (name_ptr),FALSE,&nconflicts);
    for ( name_ptr = Name_Next (name_ptr);
	  name_ptr != NAME_NULL_PTR;
	  name_ptr = Name_Next (name_ptr))
      Print_Output_Conflicts(fp,Name_Node (name_ptr),FALSE,&nconflicts);

  }
  fclose(fp);
  return okay;
}

/* make a new node for list of markings */
lml *New_LML()
{
  lml *tmp;

  tmp =  (lml *) GetBlock (sizeof(lml));

  tmp->ml = NULL;
  tmp->eventlist = NULL;
  tmp->i = 0;
  LML_Next(tmp) = NULL;
  return(tmp);
}


void Print_SG_Header (FILE *fout)
{
  Node *ptr;

  fprintf(fout,"SG:\nSTATEVECTOR: ");
  for (ptr = io_list; ptr!=NULL; ptr = Node_Next(ptr)) {
    if (Node_Type(ptr) == INPUT_NODE) {
      fprintf(fout,"INP ");
    }
    if ((Node_Type(ptr) == INPUT_NODE) ||
	(Node_Type(ptr) == OUTPUT_NODE)) {
      fprintf(fout,"%s ",Node_Name(ptr));
    }
  }
  fprintf(fout,"\nSTATES:\n");
}

int same_mark (mark_elem *m1, mark_elem* m2)
{
  if (Mark_Src(m1) != Mark_Src(m2))
    return(FALSE);
  if (Mark_Des(m1) != Mark_Des(m2))
    return(FALSE);
  if (Mark_Type(m1) != Mark_Type(m2))
    return(FALSE);
  return(TRUE);
}

int is_mark_in_list (mark_elem *m1, mark_elem *m2)
{
  mark_elem *list;

  for (list = m2; list!=NULL; list=Mark_Next(list)) {
    if (same_mark(m1, list))
      return(TRUE);
  }
  return(FALSE);
}

int is_same_mark_list(mark_elem *m1, mark_elem *m2) {

  mark_elem *list;

  for (list = m1; list!=NULL; list=Mark_Next(list)) {
    if (is_mark_in_list(list, m2) == FALSE)
      return(FALSE);
  }
  for (list = m2; list!=NULL; list=Mark_Next(list)) {
    if (is_mark_in_list(list, m1) == FALSE)
      return(FALSE);
  }
  return(TRUE);
}

int is_ml_in_lml(mark_elem *m1, lml *markings) {

  lml *list;

  for (list = markings ; list!=NULL; list=LML_Next(list)) {
    if (is_same_mark_list(list->ml, m1)) {
      return(TRUE);
    }
  }
  return(FALSE);
}

void Print_State(mark_elem *ml)
{
  Node *ptr;

  printf("[");
  for (ptr = io_list; ptr!=NULL; ptr=Node_Next(ptr)) {
    printf("%s ",Node_Name(ptr));
  }
  printf("\b] = [");
  for (ptr = io_list; ptr!=NULL; ptr=Node_Next(ptr)) {
    printf("%d",Mark_State(ml)[Node_Num(ptr)]);
  }
  printf("]\n");
}

/* return true if all preds of this node are marked in marking ml*/
int all_preds_marked(Node *node, mark_elem *ml)
{
  int i;
  Node *pred;
  mark_elem *tmp;

  tmp=NULL;
  for (i = 0; i < Node_Num_of_Pred(node); i++)	 {
    pred = Node_Preds(node)[i];
    if ((Node_Type(pred) == TRANSITION_NODE)||
	(Node_Type(pred) == DUMMY_NODE))
      Enter_Mark(&tmp, EDGE_MARK, pred, node, 1, 1);
    else if (Node_Type(pred) == PLACE_NODE)
      Enter_Mark(&tmp, PLACE_MARK, pred, pred, 1, 1);
    else if (Node_Type(pred) == VAR_NODE)
      Enter_Mark(&tmp, VAR_MARK, pred, pred, 1, 1);
    else {
      printf("ERROR: Unexpected pred node type\n");
      return(-1);
    }
    if (is_mark_in_list(tmp, ml) == FALSE) {
      Free_Mark_List(tmp);
      return 0;
    }
    Free_Mark_List(tmp);
    tmp = NULL;
  }
  return 1;
}

int enabled(mark_elem *mark, int i)
{
  mark_elem *ml;
  Node *new_des, *des;
  int j,result;

  for (ml = mark; ml != NULL; ml = Mark_Next(ml)) {
    des = Mark_Des(ml);
    if (Node_Type(des) == TRANSITION_NODE) {
      if (Node_Num(Node_Signal_Ptr(des)) == i)  {
	if ((result=all_preds_marked(des, ml))) {
	  return result;
	}
      }
    } else {
      /* place transition */
      for (j = 0; j < Node_Num_of_Succ(des); j++) {
	new_des = Node_Succs(des)[j];
	if (Node_Type(new_des) != TRANSITION_NODE) {
	  printf("Unexpected node type\n");
	  return -1;
	}
	if (Node_Num(Node_Signal_Ptr(new_des)) == i)  {
	  if ((result=all_preds_marked(new_des, ml))) {
	    return result;
	  }
	}
      }
    }
  }
  return 0;
}

int Find_Mark(mark_elem **tmp, Node *src, Node *des, mark_elem *newml)
{
  for (*tmp = newml; (*tmp != NULL) &&
	 ((Mark_Src(*tmp) != src) ||
	  (Mark_Des(*tmp) != des)); *tmp =  Mark_Next(*tmp));
  if (*tmp == NULL) {
    printf(" Couldn't find token\n");
    return -1;
  }
  return 0;
}

void push_ml_to_lml(mark_elem *ml)
{
  lml *newlml;

  newlml = New_LML();

  /* preappend to head of list */
  newlml->next = head;
  newlml->ml = ml;
  head = newlml;
}

mark_elem *pop_ml_from_lml()
{
  lml *newlml;
  mark_elem *ml;

  /* remove from head of list */
  newlml = head;
  ml = newlml->ml;
  free(newlml);
  head = newlml->next;

  return ml;
}

//int stack=0;

void push_lml(mark_elem *ml,mark_elem *eventlist,int i)
{
  lml *newlml;
  mark_elem *newml,*oldml;

  newml=NULL;
  oldml=NULL;
  newlml = New_LML();

  /* preappend to head of list */
  newlml->next = head;
  newlml->ml = Dup_Mark_List(ml);
  if (eventlist)
    for (newml=newlml->ml,oldml=ml;oldml!=eventlist;newml=newml->next,
	   oldml=oldml->next);
  newlml->eventlist = newml; /*Dup_Mark_List(eventlist);*/
  newlml->i = i;
  head = newlml;
//  stack++;
//  printf("stack=%d\n",stack);
}

lml *pop_lml()
{
  lml *newlml;

  /* remove from head of list */
  newlml = head;
  if (newlml)
    head = newlml->next;

//  stack--;
//  printf("stack=%d\n",stack);
  return newlml;
}

void Remove_Mark(mark_elem **newml, mark_elem *token)
{
  mark_elem *tml, *old,*next;

  old = NULL;
  tml=*newml;
  while (tml) {
    next=Mark_Next(tml);
    if (same_mark(tml,token)) {
      if (old == NULL)  {
	*newml = Mark_Next(tml);
      } else {
	Mark_Next(old) = Mark_Next(tml);
      }
      Delete_Mark(tml);
      return;
    }
    old = tml;
    tml=next;
  }
}

int fire_transition (mark_elem **newml, Node *node, mark_elem *ml)
{

  mark_elem *tmp;
  Node *new_des;
  int i;

  *newml = Dup_Mark_List(ml);
  /* remove one token from each predecessor */
  for (i = 0; i < Node_Num_of_Pred(node); i++) {
    if (Node_Type(Node_Preds(node)[i]) == PLACE_NODE) {
      if (Find_Mark(&tmp, Node_Preds(node)[i], Node_Preds(node)[i],
		    *newml)==(-1)) return -1;
    }
    else
      if (Find_Mark(&tmp, Node_Preds(node)[i], node, *newml)==(-1)) return -1;
    Remove_Mark(newml, tmp);
  }
  /* add one token to each successor */
  for (i=0; i < Node_Num_of_Succ(node); i++) {
    new_des = Node_Succs(node)[i];
    if ((Node_Type(new_des) == TRANSITION_NODE) ||
	(Node_Type(new_des) == DUMMY_NODE))
      Enter_Mark(newml, EDGE_MARK, node, new_des, 1, 1);
    else if (Node_Type(new_des) == PLACE_NODE)
      Enter_Mark(newml, PLACE_MARK, new_des, new_des, 1, 1);
    else if (Node_Type(new_des) == VAR_NODE)
      Enter_Mark(newml, VAR_MARK, new_des, new_des, 1, 1);
    else  {
      printf("ERROR: Unexpected succ node type\n");
      return -1;
    }
  }
  return 0;
}

int fprint_state(FILE *fout, mark_elem *mark)
{
  int i,result;

  for (i = 0; i < num_of_ios; i++) {
    if (Mark_State(mark)[i]) {
      if ((result=enabled(mark, i))) {
	if (result==(-1)) return -1;
	fprintf(fout,"F");
      } else
	fprintf(fout,"1");
    } else {
      if ((result=enabled(mark, i))) {
	if (result==(-1)) return -1;
	fprintf(fout,"R");
      } else
	fprintf(fout,"0");
    }
  }
  fprintf(fout,"\n");
  return 0;
}

int set_state(mark_elem *newmark, mark_elem *old, Node *changed)
{
  int *ns, i;
  Node *signal;

  signal = Node_Signal_Ptr(changed);

  ns = (int *) GetBlock (sizeof(int *) * num_of_ios);

  if (Mark_State(old) == NULL) {
    printf("Previous state undefined!\n");
    return -1;
  }

  for (i = 0; i < num_of_ios; i++) {
    ns[i] = Mark_State(old)[i];
  }
  /* printf("Node_num %s = %d\n",Node_Name(signal),Node_Num(signal)); */

  if (signal)
    if (Node_Num(signal) < num_of_ios)
      ns[Node_Num(signal)] ^= 1;

  /* printf("Old state = %d; New state = %d\n",
	 Mark_State(old)[Node_Num(signal)], ns[Node_Num(signal)]); */

  if (Mark_State(newmark) != NULL) {
    for (i = 0; i < num_of_ios; i++) {
      if (ns[i] != Mark_State(newmark)[i]) {
	printf("Inconsistent state assignment\n");
	printf("Check initial state and STG!\n");
	return -1;
      }
    }
  }

  if (signal)
    if ((Node_Num(signal) < num_of_ios) &&
	(((Node_Dir(changed) == '+')^ns[Node_Num(signal)]) == 1)) {
      printf("Inconsistent state assignment\n");
      printf("Arises when firing ");
      Print_Node_Simple(changed);
      printf(" from state ");
      if (fprint_state(stdout,old)==(-1)) return -1;
      printf("Check initial state and STG!\n");
      return -1;
    }
  Mark_State(newmark) = ns;
  return 0;
}

char * str_state(mark_elem *mark)
{
  int i,result;
  char * state;

  state=(char *)GetBlock(sizeof(char)*(num_of_ios+1));
  for (i = 0; i < num_of_ios; i++) {
    if (Mark_State(mark)[i]) {
      if ((result=enabled(mark, i))) {
	if (result==(-1)) {
	  free(state);
	  return NULL;
	}
	state[i]='F';
      } else
	state[i]='1';
    } else {
      if ((result=enabled(mark, i))) {
	if (result==(-1)) {
	  free(state);
	  return NULL;
	}
	state[i]='R';
      } else
	state[i]='0';
    }
  }
  state[i]='\0';
  return state;
}

int Find_SG_Recur(stateADT *state_table,int nsignals,bddADT bdd_state,
		  bool use_bdd,markkeyADT *marker,mark_elem *oldml,
		  timeoptsADT timeopts)
{
  mark_elem *ml;
  mark_elem *token, *newml;
  Node *src, *des;
  int i,result;
  char * oldstate;
  char *    state;
  int count;

  count=0;
  ml=pop_ml_from_lml();
  /*  recur_depth++;
  printf("recur depth=%d\n",recur_depth); */
  if (oldml) oldstate=str_state(oldml); else oldstate=NULL;
  state=str_state(ml);
  if (!state) return -1;
  if (add_state(NULL,state_table,oldstate,NULL,NULL,NULL,NULL,0,NULL, state,
		NULL,NULL,nsignals,NULL,NULL,0,count,FALSE,0,0,0,0,bdd_state,
		use_bdd,marker,timeopts, -1, -1, NULL, NULL, 0)) count++;
  else {
    Free_Mark_List(ml);
//    Delete_Mark(ml);
    if (oldstate) free(oldstate);
    free(state);
    /*    recur_depth--; */
    return 0;
  }
  if (oldstate) free(oldstate);
  free(state);
/*
  if (is_ml_in_lml(ml, head) == TRUE) {
    return 0;
  }
  add_ml_to_lml(ml);
  count++;
*/
  if (count % 1000==0)
    printf("%d\n",count);
  /* for all tokens */
  for (token = ml; token!=NULL; token=Mark_Next(token)) {
    /* check if token is on place or edge */
    if (Mark_Type(token) == PLACE_NODE) {
      src = Mark_Des(token);
      /* if on place look at all successors */
      for (i = 0; i<Node_Num_of_Succ(src);i++) {
	des = Node_Succs(src)[i];
	/* check to see if successor is enabled */
	if ((result=all_preds_marked(des, ml))) {
	  if (result==(-1)) return -1;
	  /* fire successor */
	  if (fire_transition(&newml, des, ml)) return -1;
	  if (set_state(newml,ml,des)==(-1)) return -1;
	  /* recur */
	  /* printf("Recurring on ");
	     Print_Marking(newml);
	     Print_State(newml); */
	  push_ml_to_lml(newml);
	  if (Find_SG_Recur(state_table,nsignals,bdd_state,use_bdd,marker,
			    ml,timeopts)==(-1)) return -1;
	  /* printf("Back (looking at)...");
	     Print_Marking(ml);
	     printf("\n"); */
	} /* else {
	     Print_Node_Simple(des);
	     printf("  not enabled\n");
	     } */
      }
    } else {
      /* destination must be a transition */
      des = Mark_Des(token);
      if ((result=all_preds_marked(des,ml)))  {
	if (result==(-1)) return -1;
	/* Print_Node_Simple(des);
	   printf("  enabled\n"); */
	/* fire token */
	if (fire_transition(&newml, des, ml)) return -1;
	if (set_state(newml,ml,des)==(-1)) return (-1);
	/* recur only if marking is unseen */
	/* printf("Recurring on ");
	   Print_Marking(newml);
	   Print_State(newml);
	   printf("\n"); */
	push_ml_to_lml(newml);
	if (Find_SG_Recur(state_table,nsignals,bdd_state,use_bdd,marker,
			  ml,timeopts)==(-1)) return (-1);
	/* printf("Back (looking at)...");
	   Print_Marking(ml);
	   printf("\n"); */
      } /* else {
	   Print_Node_Simple(Mark_Des(ml));
	   printf("  not enabled\n");
	   } */
    }
  }
  Free_Mark_List(ml);
//  Delete_Mark(ml);
  recur_depth--;
  return 0;
}

int Find_SG_Iter(stateADT *state_table,int nsignals,bddADT bdd_state,
		 bool use_bdd,markkeyADT *marker,mark_elem *ml,
		 timeoptsADT timeopts)
{
  mark_elem *token,*newml;
  lml *curlml;
  Node *src, *des;
  int result;
  char * oldstate;
  char *    state;
  int count;
  int i;
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo memuse;
#else
  malloc_statistics_t t;
#endif
#endif
  int iter=0;
  int stack_depth=0;

  count=1;
  token=ml;
  i=0;
  while(1) {
    iter++;
    /*    curlml=pop_lml();
    if (curlml==NULL) return 0;
    if (curlml->eventlist->next)
      push_lml(curlml->ml,curlml->eventlist->next,0);*/
    if (token->next) {
      push_lml(ml,token->next,0);
      stack_depth++;
    }
    newml=NULL;
    /* check if token is on place or edge */
    if (Mark_Type(token) == PLACE_NODE) {
      src = Mark_Des(token);
      /* if on place look at all successors */
      if (i < (Node_Num_of_Succ(src)-1)) {
	push_lml(ml,token,i+1);
	stack_depth++;
      }
      des = Node_Succs(src)[i];
      /* check to see if successor is enabled */
      if ((result=all_preds_marked(des, ml))) {
	if (result==(-1)) return -1;
	/* fire successor */
	if (fire_transition(&newml, des, ml)) return -1;
	if (set_state(newml,ml,des)==(-1)) return -1;
      }
    } else {
      /* destination must be a transition */
      des = Mark_Des(token);
      if ((result=all_preds_marked(des,ml)))  {
	if (result==(-1)) return -1;
	/* fire token */
	if (fire_transition(&newml, des, ml)) return -1;
	if (set_state(newml,ml,des)==(-1)) return (-1);
      }
    }
    if (newml) {
      oldstate=str_state(ml);
      state=str_state(newml);
      if (!state) return -1;
      if (add_state(NULL,state_table,oldstate,NULL,NULL,NULL,NULL,0,NULL,state,
		    NULL,NULL,nsignals,NULL,NULL,0,count,FALSE,0,0,0,0,
		    bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL, 0)) {
	/*	push_lml(newml,newml,0);*/
	Free_Mark_List(ml);
	ml=newml;
	token=ml;
	i=0;
	count++;
	if (count % 1000 == 0) {
	  printf("%d iterations %d states (stack_depth = %d)\n",iter,count,
		 stack_depth);
	  fprintf(lg,"%d iterations %d states (stack_depth = %d)\n",iter,
		  count,stack_depth);
#ifdef MEMSTATS
#ifndef OSX
	  memuse=mallinfo();
	  printf("memory: max=%d inuse=%d free=%d \n",
		 memuse.arena,memuse.uordblks,memuse.fordblks);
	  fprintf(lg,"memory: max=%d inuse=%d free=%d \n",
		  memuse.arena,memuse.uordblks,memuse.fordblks);
#else
	  malloc_zone_statistics(NULL, &t);
	  printf("memory: max=%d inuse=%d allocated=%d\n",
		 t.max_size_in_use,t.size_in_use,t.size_allocated);
	  fprintf(lg,"memory: max=%d inuse=%d allocated=%d\n",
		  t.max_size_in_use,t.size_in_use,t.size_allocated);
#endif
#endif
	}
      } else {
	Free_Mark_List(ml);
	Free_Mark_List(newml);
	curlml=pop_lml();
	stack_depth--;
	if (curlml==NULL) return 0;
	ml=curlml->ml;
	token=curlml->eventlist;
	i=curlml->i;
	free(curlml);
      }
      free(oldstate);
      free(state);
    } else {
      Free_Mark_List(ml);
      curlml=pop_lml();
      stack_depth--;
      if (curlml==NULL) return 0;
      ml=curlml->ml;
      token=curlml->eventlist;
      i=curlml->i;
      free(curlml);
    }
      /*    Free_lml(curlml); */
  }
  return 0;
}

int Find_SG(stateADT *state_table,int nsignals,bddADT bdd_state,
	    bool use_bdd,markkeyADT *marker,mark_elem *init,
	    timeoptsADT timeopts)
{
  char * state;

  if (init_state == NULL) {
    printf("Initial state of STG has not been properly set!\n");
    return -1;
  }
  Mark_State(init) = init_state;

  state=str_state(init);
  if (!state) return -1;
  add_state(NULL,state_table,NULL,NULL,NULL,NULL,NULL,0,state,NULL,
	    NULL,NULL,nsignals,NULL,NULL,0,0,FALSE,0,0,0,0,
	    bdd_state,use_bdd,marker,timeopts, -1, -1, NULL, NULL,0);
  free(state);
//  push_lml(init,init,0);

  if (Find_SG_Iter(state_table,nsignals,bdd_state,use_bdd,marker,init,
		   timeopts)==(-1))
    return -1;
  return 0;
}

void Set_Initial_State(char *init_state_char)
{
  int i;

  if (strlen(init_state_char) != (unsigned)num_of_ios) {
    printf("Initial state vector has %d entries, but there are %d IOs.\n",
           (int)strlen(init_state_char),num_of_ios);
    //gerror("ERROR");
    //return;
  }
  for (i = 0; i < strlen(init_state_char); i++) {
    if ((init_state_char[i] != '0')&&(init_state_char[i] != '1')&&(init_state_char[i] != 'X')) {
      printf("Illegal character %c in initial state.\n",init_state_char[i]);
      gerror("ERROR");
      return;
    }
  }
  initial_state=CopyString(init_state_char);
  /* initially set to all zero */
  /*
  init_state = (int *) GetBlock (sizeof(int) * (num_of_ios + 1));
  for (i = 0; i < num_of_ios; i++) {
    if (init_state_char[i] == '0')  {
      init_state[i] = 0;
    } else if (init_state_char[i] == '1') {
      init_state[i] = 1;
    } else {
      printf("Illegal character in initial state\n");
      gerror("ERROR");
      return;
    }
  }
  */
  /* printf("Done setting initial state\n");  */
}

void Enter_Prop(char *property)
{
  proplistADT curprop;

  if (!properties) {
    properties=(proplistADT)GetBlock(sizeof(*properties));
    properties->property=CopyString(property);
    properties->next=NULL;
  } else {
    for (curprop=properties;curprop->next;curprop=curprop->next);
    curprop->next=(proplistADT)GetBlock(sizeof(*properties));
    curprop->next->property=CopyString(property);
    curprop->next->next=NULL;
  }
}

int dummy_enabled(mark_elem *ml)
{
  mark_elem *list;
  int i,result;

  for (list = ml; list!=NULL; list=Mark_Next(list)) {
    for (i = num_of_ios; i < num_of_dummys+num_of_ios; i++)  {
      if ((result=enabled(list,i)))
	return result;
    }
  }
  return 0;
}

int Print_SG(FILE *fout)
{
  lml *lml;
  int result;

  Print_SG_Header(fout);
  for (lml = head; lml !=NULL; lml = LML_Next(lml)) {
    if ((result=dummy_enabled(lml->ml)) == 0) {
      if (fprint_state(fout, lml->ml)==(-1)) return -1;
    } else if (result==(-1)) return -1;
    else
      printf("dummy enabled, skipping state\n");
  }
  fclose(fout);
  return 0;
}

bool load_g(char * filename,int *ninpsigs,int *nsignals,bool lhpn)
{
  char inname[FILENAMESIZE];

  initial_marking=NULL;
  num_of_ios=0;
  num_of_inps=0;
  num_of_dummys=0;
  num_of_node=0;
  num_inp_trans=0;
  io_list=NULL;
  node_list=NULL;
  init_state = NULL;
  initial_state = NULL;
  properties=NULL;

  Name_Init_Table ();
  strcpy(inname,filename);
  if (lhpn) {
    strcat(inname,".lpn");
  } else {
    strcat(inname,".g");
    if (gin == NULL) {
      strcpy(inname,filename);
      strcat(inname,".lpn");
    }
  }
  gin = fopen (inname, "r");
  if ( gin == NULL ) {
    printf("ERROR: Cannot open input file: %s.\n", inname);
    return FALSE;
  }
  completed=true;
  hasFailTrans=false;
  if (gparse()) return false;
  if (!completed) return false;
  (*ninpsigs)=num_of_inps;
  (*nsignals)=num_of_ios;
  fclose(gin);
  return TRUE;
}

bool find_rsg_from_g(stateADT *state_table,int nsignals,bddADT bdd_state,
		     bool use_bdd,markkeyADT *marker,timeoptsADT timeopts)
{
  if (Find_SG(state_table,nsignals,bdd_state,use_bdd,marker,
	      initial_marking,timeopts)==(-1)) return FALSE;
  return TRUE;
}

void extract_signals(signalADT *signals,ineqADT inequalities,int nsignals)
{
  Name_Ptr name_ptr;
  Node *ptr;
  int i;
  char ineq[MAXTOKEN];

  i=0;
  for (ptr = io_list; ptr!=NULL; ptr = Node_Next(ptr))
    if ((Node_Type(ptr)==INPUT_NODE) &&
        !(Node_Abstract(ptr))) {
      signals[i]->name=CopyString(Node_Name(ptr));
      Node_Position(ptr)=i;
      i++;
    }
  for (ptr = io_list; ptr!=NULL; ptr = Node_Next(ptr))
    if (((Node_Type(ptr)==OUTPUT_NODE) || (Node_Type(ptr)==INTERNAL_NODE)) &&
        !(Node_Abstract(ptr))) {
      signals[i]->name=CopyString(Node_Name(ptr));
      Node_Position(ptr)=i;
      i++;
    }

  int start = i;
  for (ineqADT globalineq = inequalities;globalineq != NULL;
       globalineq = globalineq->next) {
    if (globalineq->type < 5) {
      sprintf(ineq,"%s%s%s",Node_Name((Node*)globalineq->place),
              (globalineq->type==0)?">":((globalineq->type==1)?">=":
                                         ((globalineq->type==2)?"<":
                                          ((globalineq->type==3)?"<=":"="))),
              globalineq->expr);
      signals[i]->name=CopyString(ineq);
      globalineq->signal=i;
      //printf("e_s:%s\n",signals[i]->name);
      i++;
    }
  }


  for (int j=0; j < NAME_MAX_ENTRIES; j++) {
    if ((name_ptr = name_table[j]) == NAME_NULL_PTR) continue;
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE)||
        (Node_Type(Name_Node(name_ptr))==PLACE_NODE)||
        (Node_Type(Name_Node(name_ptr))==DUMMY_NODE)) {
      for(ineqADT curineq=Node_Ineq(Name_Node(name_ptr));curineq;
          curineq=curineq->next) {
        if ((Node_Cont((Node*)curineq->place)) &&
            (curineq->type < 5)) {
          sprintf(ineq,"%s%s%s",Node_Name((Node*)curineq->place),
                  (curineq->type==0)?">":((curineq->type==1)?">=":
                                          ((curineq->type==2)?"<":
                                           ((curineq->type==3)?"<=":"="))),
                  curineq->expr);
          for (int k = start; k < nsignals; k++) {
            if (strcmp(ineq,signals[k]->name) == 0) {
              curineq->signal=k;
            }
          }
        }
      }
    }
    for ( name_ptr = Name_Next (name_ptr);
          name_ptr != NAME_NULL_PTR;
          name_ptr = Name_Next (name_ptr)) {
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE)||
          (Node_Type(Name_Node(name_ptr))==PLACE_NODE)||
          (Node_Type(Name_Node(name_ptr))==DUMMY_NODE)) {
        for(ineqADT curineq=Node_Ineq(Name_Node(name_ptr));curineq;
            curineq=curineq->next) {
          if ((Node_Cont((Node*)curineq->place)) &&
              (curineq->type < 5)) {
            sprintf(ineq,"%s%s%s",Node_Name((Node*)curineq->place),
                    (curineq->type==0)?">":((curineq->type==1)?">=":
                                            ((curineq->type==2)?"<":
                                             ((curineq->type==3)?"<=":"="))),
                    curineq->expr);
            for (int k = start; k < nsignals; k++) {
              if (strcmp(ineq,signals[k]->name) == 0) {
                curineq->signal=k;
              }
            }
          }
        }
      }
    }
  }
}

void free_nodes()
{
  Name_Ptr name_ptr;
  Name_Ptr next_name_ptr;
  mark_elem *mk;
  mark_elem *mk_next;

  for (int i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    while (name_ptr != NAME_NULL_PTR) {
      next_name_ptr = Name_Next (name_ptr);
      if (Node_Lower(Name_Node(name_ptr)))
	free(Node_Lower(Name_Node(name_ptr)));
      if (Node_Upper(Name_Node(name_ptr)))
	free(Node_Upper(Name_Node(name_ptr)));
      if (Node_PredType(Name_Node(name_ptr)))
	free(Node_PredType(Name_Node(name_ptr)));
      if (Node_Plower(Name_Node(name_ptr)))
	free(Node_Plower(Name_Node(name_ptr)));
      if (Node_Pupper(Name_Node(name_ptr)))
	free(Node_Pupper(Name_Node(name_ptr)));
      if (Node_Weight(Name_Node(name_ptr)))
	free(Node_Weight(Name_Node(name_ptr)));
      if (Node_Expr(Name_Node(name_ptr)))
	free(Node_Expr(Name_Node(name_ptr)));
      if (Node_RuleType(Name_Node(name_ptr)))
	free(Node_RuleType(Name_Node(name_ptr)));
      if (Node_Disabling(Name_Node(name_ptr)))
	free(Node_Disabling(Name_Node(name_ptr)));
      if (Node_Rate(Name_Node(name_ptr)))
	free(Node_Rate(Name_Node(name_ptr)));
      if (Node_Succs(Name_Node(name_ptr)))
	free(Node_Succs(Name_Node(name_ptr)));
      if (Node_Preds(Name_Node(name_ptr)))
	free(Node_Preds(Name_Node(name_ptr)));
      if (Node_Hsl(Name_Node(name_ptr)))
      	free(Node_Hsl(Name_Node(name_ptr)));
      if (Node_TransRate(Name_Node(name_ptr)))
      	free(Node_TransRate(Name_Node(name_ptr)));
      if (Node_DelayExpr(Name_Node(name_ptr)))
      	free(Node_DelayExpr(Name_Node(name_ptr)));
      if (Node_PriorityExpr(Name_Node(name_ptr)))
      	free(Node_PriorityExpr(Name_Node(name_ptr)));
      free(Name_Node(name_ptr));
      free(Name_String(name_ptr));
      free(name_ptr);
      name_ptr=next_name_ptr;
    }
  }
  mk=initial_marking;
  while (mk != NULL) {
    mk_next=Mark_Next(mk);
    free(mk);
    mk=mk_next;
  }
}

int extract_nevents()
{
  Name_Ptr name_ptr;
  int i,nevents;

  nevents=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr))
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE)||
	  (Node_Type(Name_Node(name_ptr))==DUMMY_NODE)) nevents++;
  }
  return nevents;
}

int extract_ndummy()
{
  Name_Ptr name_ptr;
  int i,ndummy;

  ndummy=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr))
      if (Node_Type(Name_Node(name_ptr))==DUMMY_NODE) ndummy++;
  }
  return ndummy;
}

int extract_nplaces()
{
  Name_Ptr name_ptr;
  int i,nplaces;

  nplaces=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr))
      if (Node_Type(Name_Node(name_ptr))==PLACE_NODE ||
	  Node_Type(Name_Node(name_ptr))==VAR_NODE) nplaces++;
  }
  return nplaces;
}

int extract_nvars()
{
  Name_Ptr name_ptr;
  int i,nvars;

  nvars=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr))
      if (Node_Type(Name_Node(name_ptr))==INT_NODE ||
	  Node_Type(Name_Node(name_ptr))==VAR_NODE) nvars++;
  }
  //  printf("nvars = %d\n",nvars);
  return nvars;
}

void extract_variables(variableADT *variables, int nvars)
{
  Name_Ptr name_ptr;
  int i,j;
  mark_elem *mk;


  j=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr))
      if (Node_Type(Name_Node(name_ptr))==VAR_NODE) {
	if (Node_Cont(Name_Node(name_ptr)))
	  variables[j]->var_type = VAR_NODE;
	else
	  variables[j]->var_type = INT_NODE;
	variables[j]->name  =
	  (char*)GetBlock(strlen(Node_Name(Name_Node(name_ptr)))+1);
	strcpy(variables[j++]->name,Node_Name(Name_Node(name_ptr)));
      }
  }
  for (mk=initial_marking; mk != NULL; mk=Mark_Next(mk)) {
    if ( Mark_Type(mk) == VAR_MARK ) {
      //printf("initializing %s\n",Node_Name(Mark_Place(mk)));
      for (i=0;i<nvars;i++){
	//printf("looking up variables[%d](%s)\n",i,variables[i]->name);
	if (!strcmp(variables[i]->name,Node_Name(Mark_Place(mk)))){
	  variables[i]->lInitVal=Mark_Markl(mk);
	  variables[i]->uInitVal=Mark_Marku(mk);
	  break;
	}
      }
    }
  }
//   for (i = 0;i<nvars;i++){
//     if (variables[i]->var_type==VAR_NODE)
//       printf("real: %s = [%d,%d]\n",variables[i]->name,
// 	     variables[i]->lVal,variables[i]->uinitVal);
//     else
//       printf("int: %s = [%d,%d]\n",variables[i]->name,
// 	     variables[i]->lVal,variables[i]->uinitVal);
//   }
}



/*****************************************************************************/
/* Get a token from an expression.                                           */
/*****************************************************************************/

int expr_gettok(char *expr,char *tokvalue,int maxtok,int *position)
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readword;   /* True if token is a word */

  readword = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((*position)<(signed)strlen(expr)) {
    c=expr[(*position)];
    (*position)++;
    switch (c) {
    case '[':
    case '(':
    case '~':
    case '&':
    case ')':
    case '|':
    case ']':
      if (!readword) return(c);
      else {
	(*position)--;
	return(WORD);
      }
      break;
    case ' ':
      if (readword)
	return (WORD);
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

bool lpn_S(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals);

bool lpn_R(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals);

bool lpn_U(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals)
{
  level_exp newlevel=NULL;
  int j;

  switch (*token) {
  case WORD:
    if (strcmp(tokvalue,"false")==0) {
      (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
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
      (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
      return TRUE;
    }
    if (strncmp(tokvalue,"maybe",5)==0) {
      for (j=0;j<nsignals;j++)
	newlevel->product[j]='-';
      newlevel->product[nsignals]='\0';
      (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
      return TRUE;
    }
    for (j=0;j<nsignals;j++)
      if (strcmp(signals[j]->name,tokvalue)==0) {
	newlevel->product[j]='1';
	break;
      }
    if (j==nsignals) {
//      printf("lpn ERROR: %s is not a signal!\n",tokvalue);
//      fprintf(lg,"lpn ERROR: %s is not a signal!\n",tokvalue);
      return FALSE;
    }
    (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
    break;
  case '(':
    (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!lpn_R(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    if ((*token) != ')') {
      printf("ERROR: Expected a )!\n");
      fprintf(lg,"ERROR: Expected a )!\n");
      return FALSE;
    }
    (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
    break;
  default:
    printf("ERROR: Expected a signal or a (!\n");
    fprintf(lg,"ERROR: Expected a signal or a (!\n");
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

bool lpn_T(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals)
{
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;
  char * product;
  int j;

  switch (*token) {
  case WORD:
  case '(':
    if (!lpn_U(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    break;
  case '~':
    (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!lpn_U(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    product=(char *)GetBlock((nsignals+1) * sizeof(char));
    for (j=0;j<nsignals;j++)
      product[j]='X';    //count global ineq list
    product[nsignals]='\0';
/*
    printf("INVERTING: ");
    for (newlevel=(*level);newlevel;newlevel=newlevel->next)
      printf("%s ",newlevel->product);    //count global ineq list
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
    printf("ERROR: Expected a signal, (, or ~!\n");
    fprintf(lg,"ERROR: Expected a signal, (, or ~!\n");
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

bool lpn_C(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals)
{
  level_exp oldlevel=NULL;
  level_exp newlevel=NULL;
  level_exp curlevel=NULL;

  switch (*token) {
  case '&':
    (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!lpn_T(token,expr,tokvalue,position,&newlevel,signals,nsignals))
      return FALSE;
    oldlevel=(*level);
    (*level)=and_expr((*level),newlevel,nsignals,position);
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
    if (!lpn_C(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    break;
  case '|':
  case ')':
  case END_OF_FILE:
    break;
  default:
    printf("ERROR: In expr: %s, Expected a |, &, ), or a ]!\n",expr);
    fprintf(lg,"ERROR: Expected a |, &, ), or a ]!\n");
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

bool lpn_B(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals)
{
  level_exp newlevel=NULL;

  switch (*token) {
  case '|':
    (*token)=expr_gettok(expr,tokvalue,MAXTOKEN,position);
    if (!lpn_S(token,expr,tokvalue,position,&newlevel,signals,nsignals))
      return FALSE;
    (*level)=or_expr((*level),newlevel,nsignals,position);
    if (!lpn_B(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    break;
  case ')':
  case END_OF_FILE:
    break;
  default:
    printf("ERROR: Expected a |, ), or a ]!\n");
    fprintf(lg,"ERROR: Expected a |, ), or a ]!\n");
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

bool lpn_S(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals)
{
  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!lpn_T(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    if (!lpn_C(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    break;
  default:
    printf("ERROR: Expected a signal, (, or ~!\n");
    fprintf(lg,"ERROR: Expected a signal, (, or ~!\n");
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

bool lpn_R(int *token,char *expr,char tokvalue[MAXTOKEN],int *position,
	   level_exp *level,signalADT *signals,int nsignals)
{
  switch (*token) {
  case WORD:
  case '(':
  case '~':
    if (!lpn_S(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    if (!lpn_B(token,expr,tokvalue,position,level,signals,nsignals))
      return FALSE;
    break;
  default:
    printf("ERROR: Expected a signal, (, or ~!\n");
    fprintf(lg,"ERROR: Expected a signal, (, or ~!\n");
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




void extract_events(signalADT *signals,int nsignals, eventADT *events,
                    ineqADT inequalities,int nevents, int nplaces, bool *level,
		    variableADT *variables,int nvars)
{
  Name_Ptr name_ptr;
  int i,k,pos,position,token;
  char tokvalue[MAXTOKEN];
  level_exp newlevel=NULL;

  update_events(signals,events,0,"reset",NULL,0);
  i=0;
  pos=nevents;
  Print_Events(NULL,false,&pos,&i,true,events,signals);
  /*
    for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr))
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE)||
	  (Node_Type(Name_Node(name_ptr))==DUMMY_NODE)) {
    events[pos]->event=CopyString(Name_String(name_ptr));
    if (Node_Signal_Ptr(Name_Node(name_ptr)))
	  events[pos]->signal=
    Node_Position(Node_Signal_Ptr(Name_Node(name_ptr)));
    Node_Position(Name_Node(name_ptr))=pos;
    pos++;
    }
    }
  */

  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    //    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for (name_ptr = name_table[i]; name_ptr != NAME_NULL_PTR;
         name_ptr = Name_Next(name_ptr)) {
      if (Node_Type(Name_Node(name_ptr))==PLACE_NODE) {
        events[pos]->event=CopyString(Name_String(name_ptr));
        if (Node_Cont(Name_Node(name_ptr))) events[pos]->type = CONT;
        else events[pos]->type=UNKNOWN;

        events[pos]->failtrans = Node_FailTrans(Name_Node(name_ptr));
        events[pos]->nondisabling = Node_NonDisabling(Name_Node(name_ptr));
        events[pos]->inequalities=Node_Ineq(Name_Node(name_ptr));
        events[pos]->lrange=Node_LRange(Name_Node(name_ptr));
        events[pos]->urange=Node_URange(Name_Node(name_ptr));
        events[pos]->hsl=CopyString(Node_Hsl(Name_Node(name_ptr)));
        events[pos]->transRate=CopyString(Node_TransRate(Name_Node(name_ptr)));
        events[pos]->delayExpr=CopyString(Node_DelayExpr(Name_Node(name_ptr)));
        events[pos]->priorityExpr=CopyString(Node_PriorityExpr(Name_Node(name_ptr)));
	events[pos]->EXP=NULL;
	events[pos]->delayExprTree=NULL;
        events[pos]->lower=Node_Ldelay(Name_Node(name_ptr));
        events[pos]->upper=Node_Udelay(Name_Node(name_ptr));
        Node_Position(Name_Node(name_ptr))=pos;
        pos++;
      }
      else if (Node_Type(Name_Node(name_ptr))==VAR_NODE) {
        events[pos]->event=CopyString(Name_String(name_ptr));
	if (Node_Cont(Name_Node(name_ptr)))
	  events[pos]->type = CONT + VAR;
	else
	  events[pos]->type = VAR;

        events[pos]->failtrans = Node_FailTrans(Name_Node(name_ptr));
        events[pos]->nondisabling = Node_NonDisabling(Name_Node(name_ptr));
        events[pos]->inequalities=Node_Ineq(Name_Node(name_ptr));
        events[pos]->lrange=Node_LRange(Name_Node(name_ptr));
        events[pos]->urange=Node_URange(Name_Node(name_ptr));
        events[pos]->hsl=CopyString(Node_Hsl(Name_Node(name_ptr)));
        events[pos]->transRate=CopyString(Node_TransRate(Name_Node(name_ptr)));
        events[pos]->delayExpr=CopyString(Node_DelayExpr(Name_Node(name_ptr)));
        events[pos]->priorityExpr=CopyString(Node_PriorityExpr(Name_Node(name_ptr)));
	events[pos]->EXP=NULL;
	events[pos]->delayExprTree=NULL;
        events[pos]->lower=Node_Ldelay(Name_Node(name_ptr));
        events[pos]->upper=Node_Udelay(Name_Node(name_ptr));
        Node_Position(Name_Node(name_ptr))=pos;
        pos++;
      }
    }
  }

  for (int i=0;i<pos;i++) {
    for (ineqADT curineq=events[i]->inequalities;
         curineq;curineq=curineq->next) {
      //The next line is a hack...
      //Just checking to see if it is already a place
      //or still a memory location
      if (curineq->place > nevents+nplaces ||
          curineq->place < -1) {
        curineq->place=Node_Position((Node*)curineq->place);
      }
      curineq->transition=i;
      // convert expression strings to trees here
      //printf("parsing %s\n",events[i]->event);
      if (curineq->expr){
	position=0;
	token=intexpr_gettok(curineq->expr,tokvalue,MAXTOKEN,&position);
	if (!intexpr_L(&token,curineq->expr,tokvalue,&position,
		       &curineq->tree,signals,nsignals,events,nevents,nplaces))
	  printf("expression %s failed to parse.\n",curineq->expr);
	else{
// 	  string out_str = curineq->tree->out_string(signals,events);
// 	  cout << "just parsed: " <<curineq->expr<<" :"<< out_str << endl;
	  if ((curineq->tree->isit == 'n')||(curineq->tree->isit == 't')){
	    curineq->constant = curineq->tree->lvalue;
	    curineq->uconstant = curineq->tree->uvalue;
	    //printf("%s:[%d,%d]\n",curineq->expr,
	    //	   curineq->constant,curineq->uconstant);
	  }
	}
	//curineq->constant = curineq->ltree->lvalue;
	//curineq->uconstant = curineq->ltree->lvalue;
	/*printf("type = %d [%d,%d]\n",curineq->type,curineq->constant,curineq->uconstant);*/
      }
      else{
	curineq->tree = NULL;
      }
    }
  }
  for(ineqADT i=inequalities;i!=NULL;i=i->next) {
    i->place=Node_Position((Node*)i->place);
    i->transition=Node_Position((Node*)i->transition);
    if (i->expr){
      position=0;
      token=intexpr_gettok(i->expr,tokvalue,MAXTOKEN,&position);
      if (!intexpr_L(&token,i->expr,tokvalue,&position,
		     &i->tree,signals,nsignals,events,nevents,nplaces))
	printf("expression %s failed to parse.\n",i->expr);
//       string out_str = i->tree->out_string(signals,events);
//       cout << "just parsed: " <<i->expr<<" :"<< out_str << endl;
    } else {
      i->tree = NULL;
    }
  }

  for (int i=0;i<pos;i++) {
    if (events[i]->hsl) {
      (*level)=true;
      position=0;
      token=intexpr_gettok(events[i]->hsl,tokvalue,MAXTOKEN,&position);
      if (intexpr_L(&token,events[i]->hsl,tokvalue,&position,
		    &events[i]->EXP,signals,nsignals,events,nevents,nplaces)){
	position=0;
	token=expr_gettok(events[i]->hsl,tokvalue,MAXTOKEN,&position);
	lpn_R(&token,events[i]->hsl,tokvalue,&position,
	      &events[i]->SOP,signals,nsignals);
      }
      else{
	events[i]->EXP = NULL;
	return;
      }
    }
    else {
      events[i]->SOP=(level_exp)GetBlock(sizeof(*newlevel));
      events[i]->SOP->next=NULL;
      events[i]->SOP->product=
        (char *)GetBlock((nsignals+1)*sizeof(char));
      for (k=0;k<nsignals;k++)
        events[i]->SOP->product[k]='X';
      events[i]->SOP->product[nsignals]='\0';
    }
    if (events[i]->delayExpr) {
      //cout << "about to parse "<< events[i]->delayExpr << endl;
      (*level)=true;
      position=0;
      token=intexpr_gettok(events[i]->delayExpr,tokvalue,MAXTOKEN,&position);
      if (!intexpr_L(&token,events[i]->delayExpr,tokvalue,&position,
		     &events[i]->delayExprTree,signals,nsignals,events,nevents,nplaces)){
	events[i]->delayExprTree = NULL;
	return;
      }
      //string out_str = events[i]->delayExprTree->out_string(signals,events);
      //cout << "just parsed: " <<events[i]->delayExpr<<" :"<< out_str << endl;
      if ((events[i]->delayExprTree->isit == 'n')||(events[i]->delayExprTree->isit == 't')){
	events[i]->lower = events[i]->delayExprTree->lvalue;
	events[i]->upper = events[i]->delayExprTree->uvalue;
      }
    }
    //The next block converts to SOP, so add that in
    /*absorb_expr(rules,e,f,false); //What does this do?
      if (events[i]->level) {
      char *sum=(char*)GetBlock((nsignals+1)*sizeof(char));
      for (int i=0;i<nsignals;i++) sum[i]='X';
      sum[nsignals]='\0';
      SOP_2_POS(rules,e,f,events[i]->level,sum,nsignals);
      free(sum);
      absorb_expr(rules,e,f,true); //And again
      }*/

    //cout << "HSL:" << *(Node_Hsl(Name_Node(name_ptr))) << endl;
  }
}

int extract_rules(signalADT *signals,eventADT *events,ruleADT **rules,
		  markkeyADT *markkey,int nsignals,int nevents,int nplaces,
		  bool *level)
{
  Name_Ptr name_ptr;
  int i,j,k,e,f,nrules,position,token;
  level_exp newlevel=NULL;
  char tokvalue[MAXTOKEN];
  mark_elem *mk;
  bool dup_warn=false;

  token=WORD;
  nrules=0;
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    for ( ; name_ptr != NAME_NULL_PTR; name_ptr = Name_Next (name_ptr)) {
      for ( j=0; j<Node_Num_of_Succ(Name_Node(name_ptr)); j++) {
	e=Node_Position(Name_Node(name_ptr));
	f=Node_Position(Node_Succs(Name_Node(name_ptr))[j]);
	if (rules[e][f]->ruletype) {
	  if (!dup_warn) {
	    printf("WARNING: Duplicate rules ignored.\n");
	    dup_warn=true;
	  }
	  continue;
	}
	if (Node_RuleType(Name_Node(name_ptr))[j]) {
	  if (strcmp(Node_RuleType(Name_Node(name_ptr))[j],"C")==0)
	    rules[e][f]->ruletype=(ORDERING | SPECIFIED);
	  else if (strcmp(Node_RuleType(Name_Node(name_ptr))[j],"T")==0)
	    rules[e][f]->ruletype=(TRIGGER | SPECIFIED);
	  else if (strcmp(Node_RuleType(Name_Node(name_ptr))[j],"A")==0)
	    rules[e][f]->ruletype=(ASSUMPTION | SPECIFIED);
	  else {
	    printf("WARNING: Unknown rule type %s for rule <%s,%s>\n",
		   Node_RuleType(Name_Node(name_ptr))[j],events[e]->event,
		   events[f]->event);
	  }
	} else
	  rules[e][f]->ruletype=(TRIGGER | SPECIFIED);
	if (Node_Disabling(Name_Node(name_ptr))[j])
	  rules[e][f]->ruletype=(rules[e][f]->ruletype | DISABLING);
	rules[e][f]->epsilon=0;
	rules[e][f]->lower=Node_Lower(Name_Node(name_ptr))[j];
	rules[e][f]->upper=Node_Upper(Name_Node(name_ptr))[j];
	rules[e][f]->predtype=Node_PredType(Name_Node(name_ptr))[j];
	rules[e][f]->plower=Node_Plower(Name_Node(name_ptr))[j];
	rules[e][f]->pupper=Node_Pupper(Name_Node(name_ptr))[j];
	rules[e][f]->weight=Node_Weight(Name_Node(name_ptr))[j];
	rules[e][f]->rate=Node_Rate(Name_Node(name_ptr))[j];
	/* Do I need to use CopyString, can I do a free here */
	rules[e][f]->expr=CopyString(Node_Expr(Name_Node(name_ptr))[j]);
	rules[e][f]->marking=nrules;
	if (rules[e][f]->expr) {
	  (*level)=true;
	  position=0;
	  token=expr_gettok(rules[e][f]->expr,tokvalue,MAXTOKEN,&position);
	  if (!lpn_R(&token,rules[e][f]->expr,tokvalue,&position,
		     &rules[e][f]->level,signals,nsignals)) return FALSE;
	} else {
	  rules[e][f]->level=(level_exp)GetBlock(sizeof(*newlevel));
	  rules[e][f]->level->next=NULL;
	  rules[e][f]->level->product=
	    (char *)GetBlock((nsignals+1)*sizeof(char));
	  for (k=0;k<nsignals;k++)
	    rules[e][f]->level->product[k]='X';
	  rules[e][f]->level->product[nsignals]='\0';
	}
	absorb_expr(rules,e,f,false);
	if (rules[e][f]->level) {
	  char *sum=(char*)GetBlock((nsignals+1)*sizeof(char));
	  for (int i=0;i<nsignals;i++) sum[i]='X';
	  sum[nsignals]='\0';
	  SOP_2_POS(rules,e,f,rules[e][f]->level,sum,nsignals);
	  free(sum);
	  absorb_expr(rules,e,f,true);
	}
	markkey[nrules]->enabling=e;
	markkey[nrules]->enabled=f;
	markkey[nrules]->epsilon=0;
	int markl=0;
	int marku=0;
	for (mk=initial_marking; mk != NULL && !markl; mk=Mark_Next(mk)) {
	  if ( Mark_Type(mk) == EDGE_MARK ) {
	    if ((Mark_Src(mk)==Name_Node(name_ptr)) &&
		(Mark_Des(mk)==Node_Succs(Name_Node(name_ptr))[j])) {
	      markl=Mark_Markl(mk);
	      marku=Mark_Markl(mk);
	    }
	  }
	  else {
	    if (Mark_Place(mk)==Name_Node(name_ptr)) {
	      markl=Mark_Markl(mk);
	      marku=Mark_Marku(mk);
	    }
	  }

	}
	if (markl) {
	  rules[e][f]->epsilon=markl;
	  markkey[nrules]->epsilon=markl;
	  /*events[e]->lower=markl;
	    events[e]->upper=marku;*/
	}
	nrules++;
      }
    }
  }
  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    for (name_ptr = name_table[i]; name_ptr != NAME_NULL_PTR;
	 name_ptr = Name_Next (name_ptr)) {
      if (Node_Type(Name_Node(name_ptr))==INT_NODE) continue;
      e=Node_Position(Name_Node(name_ptr));
      for (mk=initial_marking; mk != NULL; mk=Mark_Next(mk)) {
	if (Mark_Place(mk)==Name_Node(name_ptr)) {
	  if (Mark_Type(mk) == RATE_MARK) {
	    events[e]->linitrate=Mark_Markl(mk);
	    events[e]->uinitrate=Mark_Marku(mk);
	  }
	  else if (Mark_Type(mk) != EDGE_MARK) {
	    events[e]->lower=Mark_Markl(mk);
	    events[e]->upper=Mark_Marku(mk);
	  }
	}
      }
    }
  }
  for (i=0;i<nevents;i++)
    for (j=i+1;j<nevents;j++) {
      /* For consistent state assignment checks */
      /*
      if (events[i]->signal==events[j]->signal &&
	  events[i]->signal >= 0) {
	rules[i][j]->conflict=true;
	rules[j][i]->conflict=true;
      }
      */
      /* i conflicts with j if firing i prevents firing j by stealing token */
      for (k=nevents;k<nevents+nplaces;k++)
	if ((rules[k][i]->ruletype) && (rules[k][j]->ruletype)) {
	  if ((!rules[i][k]->ruletype) || (!rules[j][k]->ruletype)) {
	    rules[i][j]->conflict=OCONFLICT;
	    rules[j][i]->conflict=OCONFLICT;
	  }
	  break;
	}
      for (k=nevents;k<nevents+nplaces;k++)
	if ((rules[i][k]->ruletype) && (rules[j][k]->ruletype)) {
	  rules[i][j]->conflict=ICONFLICT;
	  rules[j][i]->conflict=ICONFLICT;
	  break;
	}
    }

  /* A very simple definition of conflict for level expressions
     Namely, given two rules, r1 and r2, if the enabled event in for r1
     appears in the opposite phase in the expression for r2 then the
     enabled events for both rules conflict. */
#ifdef __LEVELS__
  for (i=0;i<nrules;i++)
    for (level_exp curlevel=
	   rules[markkey[i]->enabling][markkey[i]->enabled]->level;
	 curlevel;curlevel=curlevel->next)
      for (j=0;j<nsignals;j++)
	if (curlevel->product[j]=='0') {
	  for (k=signals[j]->event;k<nevents;k+=2)
	    if ((events[k]->signal==j)&&(k!=markkey[i]->enabled)) {
	      rules[k][markkey[i]->enabled]->conflict=OCONFLICT;
	      rules[markkey[i]->enabled][k]->conflict=OCONFLICT;
	    } else
	      break;
	} else if (curlevel->product[j]=='1') {
	  for (k=signals[j]->event+1;k<nevents;k+=2)
	    if ((events[k]->signal==j)&&(k!=markkey[i]->enabled)) {
	      rules[k][markkey[i]->enabled]->conflict=OCONFLICT;
	      rules[markkey[i]->enabled][k]->conflict=OCONFLICT;
	    } else
	      break;
	}
#endif

  for (i=0;i<nevents+nplaces;i++) {
    bool dropped=true;
    for (j=0;j<nevents+nplaces;j++)
      if (rules[i][j]->ruletype || rules[j][i]->ruletype) {
	dropped=false;
	break;
      }
    if (dropped) events[i]->dropped=true;
  }
  return nrules;
}

bool transform_1(eventADT *events,ruleADT **rules,markkeyADT *markkey,
		 int nevents,int nplaces,int ndummy)
{
  bool modified=false;
  for (int i=nevents-ndummy;i<nevents;i++)
    if (!(events[i]->dropped)) {
      int place=0;
      for (int j=1;j<nevents+nplaces;j++)
	if (rules[i][j]->ruletype & TRIGGER)
	  if (place==0) place=j;
	  else {
	    place=(-1);
	    break;
	  }
      if (place==(-1)) continue;
      events[i]->dropped=true;
      printf("Dropping %s\n",events[i]->event);
      modified=true;
      rules[i][place]->ruletype=NORULE;
      printf("DropFO %s -> %s\n",events[i]->event,events[place]->event);
      for (int j=1;j<nevents;j++) {
	if (rules[i][j]->ruletype & ORDERING) rules[i][j]->ruletype=NORULE;
	if (rules[j][i]->ruletype & ORDERING) rules[j][i]->ruletype=NORULE;
      }
      for (int j=nevents;j<nevents+nplaces;j++) {
	if (rules[i][j]->ruletype & ORDERING) {
	  for (int k=0;k<nevents;k++)
	    if (rules[j][k]->ruletype)
	      rules[j][k]->ruletype=NORULE;
	  rules[i][j]->ruletype=NORULE;
	  printf("DropO %s -> %s\n",events[i]->event,events[j]->event);
	}
	if (rules[j][i]->ruletype & ORDERING) {
	  for (int k=0;k<nevents;k++)
	    if (rules[k][j]->ruletype)
	      rules[k][j]->ruletype=NORULE;
	  rules[j][i]->ruletype=NORULE;
	  printf("DropO %s -> %s\n",events[j]->event,events[i]->event);
	}
      }
      for (int j=1;j<nevents;j++)
	if (rules[j][i]->ruletype & TRIGGER) {
	  rules[j][place]->ruletype=rules[j][i]->ruletype;
	  printf("Add1 %s -> %s\n",events[j]->event,events[place]->event);
	  rules[j][place]->lower=rules[j][i]->lower+rules[i][place]->lower;
	  rules[j][place]->upper=rules[j][i]->upper+rules[i][place]->upper;
	  if (rules[i][place]->epsilon==1) rules[j][place]->epsilon=1;
	  rules[j][i]->ruletype=NORULE;
	  printf("DropFI1 %s -> %s\n",events[j]->event,events[i]->event);
	}
      if (place < nevents) {
	for (int j=nevents;j<nevents+nplaces;j++)
	  if (rules[j][i]->ruletype & TRIGGER) {
	    rules[j][place]->ruletype=rules[j][i]->ruletype;
	    printf("Add2 %s -> %s\n",events[j]->event,events[place]->event);
	    rules[j][place]->lower=rules[j][i]->lower+rules[i][place]->lower;
	    rules[j][place]->upper=rules[j][i]->upper+rules[i][place]->upper;
	    if (rules[i][place]->epsilon==1) rules[j][place]->epsilon=1;
	    rules[j][i]->ruletype=NORULE;
	    printf("DropFI2 %s -> %s\n",events[j]->event,events[i]->event);
	  }
      } else {
	events[place]->dropped=true;
	for (int j=nevents;j<nevents+nplaces;j++)
	  if (rules[j][i]->ruletype & TRIGGER) {
	    for (int k=1;k<nevents;k++)
	      if (rules[place][k]->ruletype & TRIGGER) {
		rules[j][k]->ruletype=rules[place][k]->ruletype;
		printf("Add3 %s -> %s\n",events[j]->event,events[k]->event);
		rules[j][k]->lower=rules[j][i]->lower+rules[place][k]->lower;
		rules[j][k]->upper=rules[j][i]->upper+rules[place][k]->upper;
		if (rules[place][k]->epsilon==1) rules[j][k]->epsilon=1;
		rules[place][k]->ruletype=NORULE;
		printf("DropFI3 %s -> %s\n",events[place]->event,
		       events[k]->event);
	      }
	  }
      }
    }
  return modified;
}

int count_ineq(ineqADT& inequalities)
{
  Name_Ptr name_ptr;
  int i;
  int count=0;

  for (i=0; i < NAME_MAX_ENTRIES; i++) {
    if ((name_ptr = name_table[i]) == NAME_NULL_PTR) continue;
    if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE)||
        (Node_Type(Name_Node(name_ptr))==PLACE_NODE)||
        (Node_Type(Name_Node(name_ptr))==DUMMY_NODE)) {
      for(ineqADT curineq=Node_Ineq(Name_Node(name_ptr));curineq;
          curineq=curineq->next) {
        if ((Node_Cont((Node*)curineq->place)) &&
            (curineq->type < 5)) {
          bool found = false;
          for (ineqADT globalineq = inequalities;globalineq != NULL;
               globalineq = globalineq->next) {
            if (globalineq->place == curineq->place &&
                globalineq->type == curineq->type &&
                globalineq->constant == curineq->constant &&
                globalineq->uconstant == curineq->uconstant) {
              found = true;
              break;
            }
          }
          if (!found) {
            count++;
            if (inequalities == NULL) {
              inequalities = (ineqADT) GetBlock(sizeof(ineq_tag));
              inequalities->place = curineq->place;
              inequalities->type = curineq->type;
              inequalities->uconstant = curineq->uconstant;
              inequalities->constant = curineq->constant;
              inequalities->expr = CopyString(curineq->expr);
	      inequalities->tree = curineq->tree;
              inequalities->signal = curineq->signal;
              inequalities->transition = (intptr_t)Name_Node(name_ptr);
              inequalities->next = NULL;
            }
            else {
              for (ineqADT globalineq = inequalities;
                   globalineq != NULL;
                   globalineq = globalineq->next) {
                if (globalineq->next == NULL) {
                  globalineq->next = (ineqADT) GetBlock(sizeof(ineq_tag));
                  globalineq->next->place = curineq->place;
                  globalineq->next->type = curineq->type;
                  globalineq->next->uconstant = curineq->uconstant;
                  globalineq->next->constant = curineq->constant;
		  globalineq->next->expr = CopyString(curineq->expr);
		  globalineq->next->tree = curineq->tree;
                  globalineq->next->signal = curineq->signal;
                  globalineq->next->transition = (intptr_t)Name_Node(name_ptr);
                  globalineq->next->next = NULL;
                  break;
                }
              }
            }
          }
        }
      }
    }
    for ( name_ptr = Name_Next (name_ptr);
          name_ptr != NAME_NULL_PTR;
          name_ptr = Name_Next (name_ptr)) {
      if ((Node_Type(Name_Node(name_ptr))==TRANSITION_NODE)||
          (Node_Type(Name_Node(name_ptr))==PLACE_NODE)||
          (Node_Type(Name_Node(name_ptr))==DUMMY_NODE)) {
        for(ineqADT curineq=Node_Ineq(Name_Node(name_ptr));curineq;
            curineq=curineq->next) {
          if ((Node_Cont((Node*)curineq->place)) &&
              (curineq->type < 5)) {

            bool found = false;
            for (ineqADT globalineq = inequalities;globalineq != NULL;
                 globalineq = globalineq->next) {
              if (globalineq->place == curineq->place &&
                  globalineq->type == curineq->type &&
                  globalineq->constant == curineq->constant &&
                  globalineq->uconstant == curineq->uconstant) {
                found = true;
                break;
              }
            }
            if (!found) {
              count++;
              if (inequalities == NULL) {
                inequalities = (ineqADT) GetBlock(sizeof(ineq_tag));
                inequalities->place = curineq->place;
                inequalities->type = curineq->type;
                inequalities->uconstant = curineq->uconstant;
                inequalities->constant = curineq->constant;
		inequalities->expr = CopyString(curineq->expr);
		inequalities->tree = curineq->tree;
                inequalities->signal = curineq->signal;
                inequalities->transition = (intptr_t)Name_Node(name_ptr);
                inequalities->next = NULL;
              }
              else {
                for (ineqADT globalineq = inequalities;
                     globalineq != NULL;
                     globalineq = globalineq->next) {
                  if (globalineq->next == NULL) {
                    globalineq->next = (ineqADT) GetBlock(sizeof(ineq_tag));
                    globalineq->next->place = curineq->place;
                    globalineq->next->type = curineq->type;
                    globalineq->next->uconstant = curineq->uconstant;
                    globalineq->next->constant = curineq->constant;
		    globalineq->next->expr = CopyString(curineq->expr);
		    globalineq->next->tree = curineq->next->tree;
                    globalineq->next->signal = curineq->signal;
                    globalineq->next->transition = (intptr_t)Name_Node(name_ptr);
                    globalineq->next->next = NULL;
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  //count global ineq list
  return count;
}

/*****************************************************************************/
/* Load graph from .g file and find the cooresponding state graph.           */
/*****************************************************************************/

bool load_lpn(designADT design)
{
  int size;

  printf("Loading LPN: %s.lpn\n",design->filename);
  fprintf(lg,"Loading LPN: %s.lpn\n",design->filename);
  if (load_g(design->filename,&(design->ninpsig),&(design->nsignals),true)) {

    design->inequalities=new_inequalities(design->inequalities);

    design->nineqs = count_ineq(design->inequalities);

    design->nsignals = design->nsignals + design->nineqs;

    design->signals=new_signals(design->status,FALSE,design->oldnsignals,
                                design->nsignals,design->signals);

    /* QUESTION: Should ->event be setup? */
    extract_signals(design->signals,design->inequalities,design->nsignals);

    //design->nevents=extract_nevents();
    design->nevents=1;
    Print_Events(NULL,true,&(design->nevents),&(design->ninputs),false,
                 NULL,NULL);
    design->ndummy=extract_ndummy();
    design->nplaces=extract_nplaces();

    design->nvars=extract_nvars();
    design->variables=new_variables(design->status,FALSE,design->oldnvars,
				  design->nvars,design->variables);
    extract_variables(design->variables,design->nvars);

    /* Must allocate enough memory for nevents + nplaces */
    size=design->nevents + design->nplaces;

    design->events=new_events(design->status,FALSE,design->oldnevents,
                              size,design->events);
    /* QUESTION: Are merges necessary for LPNs? */
    design->merges=new_merges(design->status,FALSE,design->oldnevents,
                              size,design->merges);
    design->rules=new_rules(design->status,FALSE,design->oldnevents,
                            size,design->rules);
    design->markkey=new_markkey(design->status,FALSE,
                                (design->oldnevents)*(design->oldnevents),
                                size*size,design->markkey);
    if (initial_state) {
      design->startstate=(char*)GetBlock((design->nsignals+1)*sizeof(char));
      for (int i=0;i<(design->nsignals-design->nineqs);i++)
        design->startstate[i]=initial_state[i];
      for (int i=design->nsignals-design->nineqs;i<design->nsignals;i++)
        design->startstate[i]='0';
      design->startstate[design->nsignals]='\0';
      free(initial_state);
    } else if (design->nsignals==0) {
      design->startstate=(char*)GetBlock(2*sizeof(char));
      strcpy(design->startstate,"0");
    } else {
      design->startstate=(char*)GetBlock((design->nsignals+1)*sizeof(char));
      for (int i=0;i<design->nsignals;i++)
        design->startstate[i]='0';
      design->startstate[design->nsignals]='\0';
    }

    if (properties) {
      design->properties=properties;
    } else if (!hasFailTrans) {
      design->properties=(proplistADT)GetBlock(sizeof(*properties));
      design->properties->property=CopyString("AG(~fail)");
      design->properties->next=NULL;
    }
    extract_events(design->signals,design->nsignals,
                   design->events,design->inequalities,design->nevents,
                   design->nplaces,&(design->level),
		   design->variables,design->nvars);

    design->nrules=extract_rules(design->signals,design->events,design->rules,
                                 design->markkey,design->nsignals,
                                 design->nevents,design->nplaces,
                                 &(design->level));

    /*
      for (ineqADT globalineq = design->inequalities;
      globalineq != NULL;
      globalineq = globalineq->next) {
      printf("place=%s,type=%d,uconstant=%d,constant=%d,signal=%s,transition=%s\n",
      design->events[globalineq->place]->event,globalineq->type,
      globalineq->uconstant,globalineq->constant,
      design->signals[globalineq->signal]->name,
      design->events[globalineq->transition]->event);
      }
    */

    // 9/8/05 - Expand the design outside of transform so that the
    // design is always expanded but transform is only applied based
    // on flag.  Transform also no longer recompresses the design.
#ifdef EXPANDNET
    printf("Replacing implicit places with explicit places.\n");
    int oldsize = (design->nevents + design->nplaces)*
      (design->nevents + design->nplaces);
    expand_design(design);
    make_markkey(design,oldsize);
#endif

    // perform transformations if post processing is enabled.
    if (design->postproc)
      transform(design);
    if(design->expandRate)
      expandRate(design);
    /*
      if (design->postproc)
      while(transform_1(design->events,design->rules,design->markkey,
			design->nevents,design->nplaces,design->ndummy));
    */
    /*
      printf("SIGNALS:\n");
      for (int i=0;i<design->nsignals;i++)
      printf("%d:%s event=%d\n",i,design->signals[i]->name,
      design->signals[i]->event);
    */
    /*
      printf("EVENTS:\n");
      for (int i=0;i<design->nevents;i++) {
      printf("%d:%s",i,design->events[i]->event);
      for (level_exp step = design->events[i]->SOP;
      step != NULL; step = step->next) {
      printf(":SOP:%s",step->product);
      }
      printf("\n");
      }
    */
    //for (ineqADT curineq=design->events[i]->inequalities;
    //   curineq;curineq=curineq->next) {
    //	printf(":%s %d %d",design->events[curineq->place]->event,
    //	       curineq->type,curineq->constant);
    //}
    /*
      if (design->events[i]->hsl)
      printf(":HSL:%s",design->events[i]->hsl);
      printf("\n");
      }
    */
    /*
      printf("PLACES:\n");
      for (int i=design->nevents;i<design->nevents+design->nplaces;i++) {
      printf("%d:%s [%d,%d] <%d,%d>",i,design->events[i]->event,
      design->events[i]->lower, design->events[i]->upper,
      design->events[i]->linitrate, design->events[i]->uinitrate);
      for (ineqADT curineq=design->events[i]->inequalities;
      curineq;curineq=curineq->next) {
      printf(":%s %d %d",design->events[curineq->place]->event,
      curineq->type,curineq->constant);
      }
      printf("\n");
      }
      printf("RULES:\n");
      for (int i=0;i<(design->nevents+design->nplaces);i++) {
      for (int j=0;j<(design->nevents+design->nplaces);j++)
      if (design->rules[i][j]->ruletype) {
      printf("%d:%d:%s -> %s eps=%d type=%d marker=%d\n",
      i,j,design->events[i]->event,
      design->events[j]->event,
      design->rules[i][j]->epsilon,
      design->rules[i][j]->ruletype,
      design->rules[i][j]->marking);
      }
      }*//*
           printf("RULES:\n");
           for (int i=0;i<design->nrules;i++) {
           printf("%d:%s -> %s eps=%d %s type=%d %g\n",i,
           design->events[design->markkey[i]->enabling]->event,
           design->events[design->markkey[i]->enabled]->event,
           design->rules[design->markkey[i]->enabling]
           [design->markkey[i]->enabled]->epsilon,
           design->rules[design->markkey[i]->enabling]
           [design->markkey[i]->enabled]->expr,
           design->rules[design->markkey[i]->enabling]
           [design->markkey[i]->enabled]->ruletype,
           design->rules[design->markkey[i]->enabling]
           [design->markkey[i]->enabled]->rate);
           for (level_exp curlevel=design->rules[design->markkey[i]->enabling]
           [design->markkey[i]->enabled]->level;
           curlevel;curlevel=curlevel->next)
           printf("%s\n",curlevel->product);
           }
           printf("\n");
           printf("CONFLICTS:\n");
           for (int i=0;i<design->nevents;i++)
           for (int j=0;j<design->nevents;j++)
           if (design->rules[i][j]->conflict)
           printf("%s # %s\n",design->events[i]->event,
           design->events[j]->event);
         */
    /* Do we need cycles? Does it even make sense? */
    /*
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
    */

    initialize_state_table(LOADED,FALSE,design->state_table);
#ifdef DLONG
    initialize_bdd_state_table(design->status,FALSE,design->bdd_state,
                               design->signals,design->nsignals,
                               design->nrules);
#else //CUDD
    design->bdd_state=new_bdd_state_table(design->status,FALSE,
                                          design->bdd_state,design->signals,
                                          design->nsignals,design->ninpsig,0,
                                          design->filename);

#endif
    design->regions=new_regions(design->status,FALSE,(2*design->oldnsignals)+1,
                                (2*design->nsignals)+1,design->regions);

    design->oldnsignals=design->nsignals;

    /* Check these */
    design->oldnevents=size; //design->nevents;
    design->oldncycles=0; //design->ncycles;
    design->si=TRUE;//sifile;
    design->status=LOADED;
    design->fromLPN=TRUE;

    /*
      if (find_rsg_from_g(design->state_table,design->nsignals,
			design->bdd_state,design->bdd,design->markkey,
			design->timeopts)) {
      design->status=(LOADED | CONNECTED | FOUNDRED);
      if (check_csc(design->filename,design->signals,design->state_table,
      design->nsignals,design->ninpsig,design->verbose)) {
      design->status=(design->status | FOUNDRSG);
      return(TRUE);
      }
      }
    */
    top_port=set<string>();
    top_port_map=map<string,string>();
    for (int i=0;i<design->nsignals;i++) {
      if (!strchr(design->signals[i]->name,'>') &&
          !strchr(design->signals[i]->name,'<') &&
          !strchr(design->signals[i]->name,'=')) {
        top_port.insert(design->signals[i]->name);
        top_port_map.insert(make_pair(design->signals[i]->name,
                                      design->signals[i]->name));
        //printf("%s\n",design->signals[i]->name);
      }
    }
    free_nodes();

    return TRUE;
  }
  free_nodes();
  printf("ERROR: %s.lpn is not a valid LHPN file!\n",design->filename);
  fprintf(lg,"ERROR: %s.lpn is not a valid LHPN file!\n",design->filename);
  return(FALSE);
}

/*****************************************************************************/
/* Load graph from .g file and find the cooresponding state graph.           */
/*****************************************************************************/

bool load_graph(designADT design)
{
  if (load_g(design->filename,&(design->ninpsig),&(design->nsignals),false)) {
/*    printf("ninputs=%d nsignals=%d\n",design->ninpsig,design->nsignals);*/
    design->signals=new_signals(design->status,FALSE,design->oldnsignals,
				design->nsignals,design->signals);
    if (design->status & LOADED)
      initialize_state_table(design->status,FALSE,design->state_table);
    design->regions=new_regions(design->status,FALSE,(2*design->oldnsignals)+1,
				(2*design->nsignals)+1,design->regions);
    design->oldnsignals=design->nsignals;
    extract_signals(design->signals,design->inequalities,design->nsignals);
#ifdef DLONG
    initialize_bdd_state_table(design->status,FALSE,design->bdd_state,
			       design->signals,design->nsignals,
			       design->nrules);
#else
    //CUDD

    design->bdd_state=new_bdd_state_table(design->status,FALSE,
					  design->bdd_state,design->signals,
					  design->nsignals,design->ninpsig,0,
					  design->filename);

#endif
    if (Print_Rules(design->filename)) {
      free_nodes();
      return true;
    }
    free_nodes();
    return false;
  }
  free_nodes();
  printf("ERROR: %s.g is not a valid g file!\n",design->filename);
  fprintf(lg,"ERROR: %s.g is not a valid g file!\n",design->filename);
  return(FALSE);
}
