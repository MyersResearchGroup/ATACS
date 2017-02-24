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
/* loadprs.c                                                                 */
/*****************************************************************************/

#include "loadprs.h"
#include "loader.h"
#include "findcover.h"
#include "storeprs.h"
#include "def.h"
#include "merges.h"
#include "connect.h"

/*****************************************************************************/
/* Get a token from a file.  Used for loading PR sets.                       */
/*****************************************************************************/

int fgettokprs(FILE *fp,char *tokvalue, int maxtok) 

{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readsignal;   /* True if token is a signal */

  readsignal = FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case NOT:
    case LBRK:
    case RBRK:
    case LPAR:
    case RPAR:
    case AND:
    case COLON:
    case RISE:
    case FALL:
      if (!readsignal) return(c);
      else {
        ungetc(c,fp);
        return (SIGNAL);
      }
      break;
    case '\n':
      if (!readsignal) return(EOL);
      else {
        ungetc('\n',fp);
        return (SIGNAL);
      }
      break;
    case '#':
      if (!readsignal) while (((c=getc(fp)) != EOF) && (c != '\n'));
      else {
	ungetc('#',fp);
	return (SIGNAL);
      }
      break;
    case ' ':
      if (readsignal) return (SIGNAL);      
      break;
    default:
      if (toklen < maxtok) {
	readsignal=TRUE;
	*tokvalue++=c;
	*tokvalue='\0';
	toklen++;
      }
      break;
    }
  }
  if (!readsignal) 
    return(EOF);
  else
    return(SIGNAL);
}

/*****************************************************************************/
/* Check if an unexpected character is read, and if so handle the error.     */
/*****************************************************************************/

bool read_error(FILE *fp,FILE *fpOut,char token,char expected1,char expected2,
		char * message)
{
  if ((token != expected1) && (token != expected2)) {
    printf("ERROR:  %s expected\n",message);
    fprintf(lg,"ERROR:  %s expected!\n",message);
    fclose(fp);
    fclose(fpOut);
    return(TRUE);
  } 
  return(FALSE);
}

/*****************************************************************************/
/* Find an event on a particular signal.                                     */
/*****************************************************************************/

int find_signal_event(FILE *fp,FILE *fpOut,eventADT *events,char * tokvalue,
		      char phase,int nevents)
{
  char event[MAXTOKEN];
  int e;

  if (phase=='1') sprintf(event,"%s+",tokvalue);
  else sprintf(event,"%s-",tokvalue);
  if ((e=which_event(nevents,events,event))==(-1)) {
    sprintf(event,"%s/1",event);
    if ((e=which_event(nevents,events,event))==(-1)) {
      printf("ERROR:  Signal %s is undefined\n",tokvalue);
      fprintf(lg,"ERROR:  Signal %s is undefined\n",tokvalue);
      fclose(fp);
      fclose(fpOut);
    }
  }
  return(e);
}

/*****************************************************************************/
/* Load a guard for a production rule.                                       */
/*****************************************************************************/

bool load_guard(FILE *fp,FILE *fpOut,eventADT *events,mergeADT *merges,
		ruleADT **rules,markkeyADT *markkey,cycleADT ****cycles,
		int f,int *nrules,int nevents,int ncycles,regionADT *regions,
		int nsignals)
{
  int e,token;
  char tokvalue[MAXTOKEN];
  char phase;
  int i,signal;
  char * guard;
  int j;
  bool match;
  
  guard=(char *)GetBlock(nsignals+2);
  for (i=0;i<nsignals;i++)
    guard[i]='Z';
  guard[nsignals]='\0';
  token=fgettokprs(fp,tokvalue,MAXTOKEN);
  if (read_error(fp,fpOut,token,LPAR,LPAR,"'('")) {
    free(guard);
    return(FALSE);
  }
  token=fgettokprs(fp,tokvalue,MAXTOKEN);
  while (token != RPAR) {
    phase='1';
    if (token==NOT) {
      phase='0';
      token=fgettokprs(fp,tokvalue,MAXTOKEN);
    }
    if (read_error(fp,fpOut,token,SIGNAL,SIGNAL,"Signal name")) {
      free(guard);
     return(FALSE);
     }
    if ((e=find_signal_event(fp,fpOut,events,tokvalue,phase,nevents))==(-1)) {
      free(guard);
      return(FALSE);
    }
    signal=(e-1)/2;
    guard[signal]=phase;
    token=fgettokprs(fp,tokvalue,MAXTOKEN);
    if (read_error(fp,fpOut,token,RPAR,AND,"'&' or ')'")) {
      free(guard);
      return(FALSE);
    }
    if (token==AND) token=fgettokprs(fp,tokvalue,MAXTOKEN);
  }
  for (j=f;j<nevents;j+=2) {
    if ((j==f) || (are_merged(merges,f,j))) {
      match=TRUE;
      for (i=0;i<nsignals;i++)
	if ((guard[i] != 'Z') && (regions[j]->enstate[i] != guard[i])) 
	  match=FALSE;
      if (match)
	for (i=0;i<nsignals;i++)
	  if (guard[i] != 'Z')
	    add_context_rule(fpOut,events,rules,markkey,cycles,
			     guard[i],i,j,nrules,nevents,ncycles,TRUE);
    }
  }
  free(guard);
  return(TRUE);
}

/*****************************************************************************/
/* Load production rules from a file.                                        */
/*****************************************************************************/

bool load_prs(char * filename,eventADT *events,mergeADT *merges,
	      ruleADT **rules,markkeyADT *markkey,cycleADT ****cycles,
	      int *nrules,int nevents,int ncycles,regionADT *regions,
	      int nsignals)
{
  char inname[FILENAMESIZE];
  char outname[FILENAMESIZE];
  FILE *fp;
  FILE *fpOut;
  char phase;
  int f,token;
  char tokvalue[MAXTOKEN];

  strcpy(inname,filename);
  strcat(inname,".prs");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return(FALSE); 
  }
  printf("Loading production rules from:  %s\n",inname);
  fprintf(lg,"Loading production rules from:  %s\n",inname);
  strcpy(outname,filename);
  strcat(outname,".cr");
  fpOut=Fopen(outname,"w");
  printf("Storing context rules to:  %s\n",outname);
  fprintf(lg,"Storing context rules to:  %s\n",outname);
  token=fgettokprs(fp,tokvalue,MAXTOKEN);
  while (token != EOF) {
    if (read_error(fp,fpOut,token,LBRK,LBRK,"'['")) return(FALSE);
    token=fgettokprs(fp,tokvalue,MAXTOKEN);
    if (read_error(fp,fpOut,token,RISE,FALL,"'+' or '-'")) return(FALSE);
    else if (token==RISE) phase='1'; else phase='0';
    token=fgettokprs(fp,tokvalue,MAXTOKEN);
    if (read_error(fp,fpOut,token,SIGNAL,SIGNAL,"Signal name")) return(FALSE);
    if ((f=find_signal_event(fp,fpOut,events,tokvalue,phase,nevents))==(-1)) 
      return(FALSE);
    token=fgettokprs(fp,tokvalue,MAXTOKEN);
    if (read_error(fp,fpOut,token,COLON,COLON,"':'")) return(FALSE);
    if (!load_guard(fp,fpOut,events,merges,rules,markkey,cycles,f,nrules,
		    nevents,ncycles,regions,nsignals))
      return(FALSE);
    token=fgettokprs(fp,tokvalue,MAXTOKEN);
    if (read_error(fp,fpOut,token,RBRK,RBRK,"']'")) return(FALSE);
    while ((token=fgettokprs(fp,tokvalue,MAXTOKEN))==EOL);
  }
  fclose(fp);
  fclose(fpOut);
  return(TRUE);
}

/*****************************************************************************/
/* Load production rules.                                                    */
/*****************************************************************************/

bool load_production_rules(char command,designADT design)
{  
  char shellcommand[100];

  if ((!(design->status & FOUNDCOVER)) && (design->syn)) {
    sprintf(shellcommand,"syn3.0 -P -o0 %s",design->filename);
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
    system(shellcommand);
  }
  if ((!(design->status & FOUNDCOVER)) && 
      ((design->syn) || (command==PRS))) {
    if (!load_prs(design->filename,design->events,design->merges,design->rules,
		  design->markkey,design->cycles,&(design->nrules),
		  design->nevents,design->ncycles,design->regions,
		  design->nsignals)) 
      return(FALSE);
    design->status=design->status | FOUNDCONF | FOUNDCOVER;
  }
  return(TRUE);
}
