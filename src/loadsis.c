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
/* usesis.c                                                                 */
/*****************************************************************************/

#include "loadsis.h"

/*****************************************************************************/
/* Store a SIS script file.                                                  */
/*****************************************************************************/

void store_scr(char * filename,int maxsize,bool si,bool gatelevel)
{
  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,filename);
  strcat(outname,".scr");
  printf("Storing script file used by SIS to:  %s\n",outname);
  fprintf(lg,"Storing graph file used by SIS to:  %s\n",outname);
  fp=Fopen(outname,"w");
  fprintf(fp,"read_astg %s.g\n",filename);
  fprintf(fp,"astg_to_f\n");
/*  fprintf(fp,"read_library ~myers/research/CAD/SIS/mylib%d.genlib\n",
	  maxsize); */
  if (gatelevel)
    fprintf(fp,"read_library %s/gate%d.genlib\n",getenv("SISlib"),maxsize);
  else
    fprintf(fp,"read_library %s/mylib%d.genlib\n",getenv("SISlib"),maxsize);
  fprintf(fp,"set_delay -L 4\n");
  fprintf(fp,"sweep\n");
  fprintf(fp,"gkx -abt 30\n");
  fprintf(fp,"resub -ad; sweep\n");
  fprintf(fp,"gcx -bt 30\n");
  fprintf(fp,"resub -ad; sweep\n");
  fprintf(fp,"gkx -abt 10\n");
  fprintf(fp,"resub -ad; sweep\n");
  fprintf(fp,"gcx -bt 10\n");
  fprintf(fp,"resub -ad; sweep\n");
  fprintf(fp,"gkx -ab\n");
  fprintf(fp,"resub -ad; sweep\n");
  fprintf(fp,"gcx -b\n");
  fprintf(fp,"resub -ad; sweep\n");
  fprintf(fp,"eliminate 0\n");
  fprintf(fp,"decomp -g *\n");
  fprintf(fp,"eliminate -1\n");
  fprintf(fp,"map\n");
  fprintf(fp,"print_gate -s\n");
  if (si) fprintf(fp,"astg_slow\n");
  else fprintf(fp,"astg_slow -f %s.env\n",filename);
  fprintf(fp,"print_gate -s\n");
  fprintf(fp,"print_delay -a\n");
  fprintf(fp,"write_bdnet %s.net\n",filename);
  fprintf(fp,"quit\n");
  fclose(fp);
}

/*****************************************************************************/
/* Store a SIS script file and execute it.                                   */
/*****************************************************************************/

void do_sis(char * filename)
{
  char shellcommand[100];

  sprintf(shellcommand,"sis < %s.scr > %s.sis",filename,filename);
  printf("Executing command:  %s\n",shellcommand);
  fprintf(lg,"Executing command:  %s\n",shellcommand);
  system(shellcommand);
}

#define TOKEN 1

/*****************************************************************************/
/* Get a token from a file.  Used for loading PR sets.                       */
/*****************************************************************************/

int fgettoksis(FILE *fp,char * tokvalue,int maxtok)
{
  int c;           /* c is the character to be read in */
  int toklen;      /* toklen is the length of the toklen */
  bool readtok;

  readtok=FALSE;
  toklen = 0;
  *tokvalue = '\0';
  while ((c=getc(fp)) != EOF) {
    switch (c) {
    case '\n':
    case ' ':
    case '(':
    case ')':
      if (readtok) return(TOKEN);
      break;
    default:
      if (toklen < maxtok) {
        readtok=TRUE;
        *tokvalue++=c;
        *tokvalue='\0';
        toklen++;
      }
      break;
    }
  }
  if (!readtok)
    return(EOF);
  else
    return(TOKEN);
}

/*****************************************************************************/
/* Load results of synthesis conducted by SIS.                               */
/*****************************************************************************/

void load_sis_results(char * filename)
{
  char inname[FILENAMESIZE];
  FILE *fp;
  char tokvalue[MAXTOKEN];
  int token;
  int xtors;

  strcpy(inname,filename);
  strcat(inname,".sis");
  if ((fp=fopen(inname,"r"))==NULL) {
    printf("ERROR:  Cannot access %s!\n",inname);
    fprintf(lg,"ERROR:  Cannot access %s!\n",inname);
    return;
  }
  token=fgettoksis(fp,tokvalue,MAXTOKEN);

  while ((token != EOF) && (strcmp(tokvalue,"Total:")))
    token=fgettoksis(fp,tokvalue,MAXTOKEN);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  printf("Before hazard elimination:  %s gates ",tokvalue);
  fprintf(lg,"Before hazard elimination:  %s gates ",tokvalue);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  xtors=atoi(tokvalue) % 10000;
  printf("%d transistors\n",xtors);
  fprintf(lg,"%d transistors\n",xtors);

  while ((token != EOF) && (strcmp(tokvalue,"Total:")))
    token=fgettoksis(fp,tokvalue,MAXTOKEN);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  printf("After hazard elimination:  %s gates ",tokvalue);
  fprintf(lg,"After hazard elimination:  %s gates ",tokvalue);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  xtors=atoi(tokvalue) % 10000;
  printf("%d transistors\n",xtors);
  fprintf(lg,"%d transistors\n",xtors);

  while ((token != EOF) && (strcmp(tokvalue,"arrival=")))
    token=fgettoksis(fp,tokvalue,MAXTOKEN);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  printf("Rise Delay = %s\n",tokvalue);
  fprintf(lg,"Rise Delay = %s\n",tokvalue);
  token=fgettoksis(fp,tokvalue,MAXTOKEN);
  printf("Fall Delay = %s\n",tokvalue);
  fprintf(lg,"Fall Delay = %s\n",tokvalue);

  fclose(fp);
}

/*****************************************************************************/
/* Create an environment file to be used by sis and set lower-bound of delay */
/*   to 0 and upper-bound to infinity.                                       */
/*****************************************************************************/

void make_env(char command,designADT design)
{
  int i,j;

  char outname[FILENAMESIZE];
  FILE *fp;

  strcpy(outname,design->filename);
  strcat(outname,".env");
  printf("Creating SIS environment file:  %s\n",outname);
  fprintf(lg,"Creating SIS environment file:  %s\n",outname);
  fp=Fopen(outname,"w");
  for (j=1;j<design->ninputs;j+=2)
    for (i=1;i<design->nevents;i+=2) {
      if ((design->rules[i][j]->ruletype != NORULE) &&
          (design->rules[i+1][j+1]->ruletype != NORULE)) {
        print_signal_name(fp,design->events,i);
        fprintf(fp," ");
        print_signal_name(fp,design->events,j);
        fprintf(fp," %d %d\n",design->rules[i][j]->lower,
                design->rules[i+1][j+1]->lower);
      }
      if ((design->rules[i][j+1]->ruletype != NORULE) &&
          (design->rules[i+1][j]->ruletype != NORULE)) {
        print_signal_name(fp,design->events,i);
        fprintf(fp," ");
        print_signal_name(fp,design->events,j);
        fprintf(fp," %d %d\n",design->rules[i][j+1]->lower,
                design->rules[i+1][j]->lower);
      }
    }
  fclose(fp);
  reinit_design(command,design,FALSE);
  for (i=0;i<design->nevents;i++)
    for (j=0;j<design->nevents;j++)
      if (design->rules[i][j]->ruletype != NORULE) {
        design->rules[i][j]->upper=INFIN;
        design->rules[i][j]->lower=0;
      }
  design->status=design->status & LOADED;
}

/*****************************************************************************/
/* Store a graph to be converted to postscript using parseGraph by Rokicki.  */
/*****************************************************************************/

void load_sis(designADT design)
{
  char shellcommand[100];

  store_scr(design->filename,design->maxsize,design->si,design->gatelevel);
  do_sis(design->filename);
  load_sis_results(design->filename);
  if (!(design->verbose)) {
    sprintf(shellcommand,"rm %s.g",design->filename);
    system(shellcommand);
    sprintf(shellcommand,"rm %s.scr",design->filename);
    system(shellcommand);
    sprintf(shellcommand,"rm %s.sis",design->filename);
    system(shellcommand);
    sprintf(shellcommand,"rm %s.net",design->filename);
    system(shellcommand);
    sprintf(shellcommand,"rm %s.env",design->filename);
    system(shellcommand);
  }
}
