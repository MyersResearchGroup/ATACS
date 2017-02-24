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
/* compile.c                                                                 */
/*****************************************************************************/


#include "compile.h"
#include "tersgen.h"
#include "symtab.h"
#include "auxilary.h"
#include "actions.h"
#include "tels_table.h"
#include <stdio.h>
#include <time.h>
#include <cassert>
#include "classes.h"
#include "postproc.h"
#include <string>

#ifdef DDEBUG
map<string, telADT> pproc_debug;
#endif

set<string> top_port;
map<string,string> top_port_map;

extern designADT design; // From atacs.c

// Record the which line the paser is currently on.
int linenumber;

int mingatedelay=MINGATEDELAY;  // minimum timing bound on gates
int maxgatedelay=MAXGATEDELAY;  // maximum timing bound on gates

// record which file the paser is currently compilng.
char *file_scope1;

// symbol table of the parser.
SymTab *table;

// This table stores tel structures of all basic modules and entities.
tels_table *tel_tb;
vhd_tel_tb *open_entity_lst;
//map<string,pair<tels_table*, bool> > 
csp_tel_tb *open_module_list;
string cur_dir;
string cur_file;

// Count of the number of dummy actions.
long PSC;

// Indicate the current entity name.
string cur_entity;
char *outputname;
char *outpath;

bool toTEL;

int yyparse();

// This variable is used to store the correspondence between the name converted
// into lower case and its original value.
map<string,string> *name_mapping = 0;

extern void lexinit();

extern my_list *filter;

int ERROR = 0;

extern const char *ieee_path;
bool pre_compile = true;

/* extern unsigned char yytext[]; */

FILE   *fpI, *fpO, *fpR, *fpD, *fpOR, *fpC, *fp;
int    nevents,ninputs,ndisj,nrules,nord,nconf,ninpsig,nsignals;
char   startin[100];
char   startstate[100];
bool   compiled;
int    curprocess;
char   errmsg[100];
//char   curfile[FILENAMESIZE];


/*****************************************************************************/
/* Print line number.                                                        */
/*****************************************************************************/
/*
void pline()
{
  printf("ERROR in line %d: ", linenumber);
  fprintf(lg, "ERROR in line %d: ", linenumber);
}
*/


/*****************************************************************************/
/* Print a compilation error.                                                */
/*****************************************************************************/
/* from lexer */
/*
extern char	g_lline[];	
extern char 	yytext[];
extern int 	column;
extern int	g_line;
extern char	curfile[];

int yyerror(string s)
{ 
  compiled=FALSE;
  printf(g_lline );
  printf("\n%*s\n%*s\n", column, "^", column, s );
  printf("%s:%d: %d \n", curfile, g_line, column );
  fprintf(lg, g_lline );
  fprintf(lg, "\n%*s\n%*s\n", column, "^", column, s );
  fprintf(lg, "%s:%d: %d \n", curfile, g_line, column );
  
  return 1;
}


 IS THIS STUFF STILL NEEDED???? */

/*****************************************************************************/
/* Print a comment.                                                          */
/*****************************************************************************/
void print_comment(FILE *fp, string comment)
{
  print_comment(fp, comment.c_str());
}

void print_comment(FILE *fp, char* comment)
{
  fprintf(fp,"#\n");
  fprintf(fp,"# %s\n",comment);
  fprintf(fp,"#\n");
}

/*****************************************************************************/
/* Copy a file into an opened file.                                          */
/*****************************************************************************/

void copy_file(FILE *fp1,string filename,int lines,char* comment)
{
  FILE *fp2;
  char line[1000];
  int i;

  print_comment(fp,comment);
  fp2=Fopen(filename.c_str(),"r");
  for(i=0;i<lines;i++) {
    fgets(line,1000,fp2);
    fputs(line,fp1);
  }
  fclose(fp2);
}

/*****************************************************************************/
/* Create and store an timed event-rule structure from temporary files       */
/*  (<filename>.inp, <filename>.out, <filename>.rule, <filename>.ord,        */
/*   <filename>.merg, <filename>.mat, <filename>.conf).                      */
/*****************************************************************************/

void make_er(char* filename,int nevents,int ninputs,int ndummy,int nrules,
	     int nord,int ndisj,int nconf,char* startin,char* startstate)
{
  //char inname[FILENAMESIZE];
  
  //char outname[FILENAMESIZE];
  //strcpy(outname,filename.c_str());
  //strcat(outname,".er");
  string outname = string(filename) + ".er";
  printf("storing result to %s\n",outname.c_str());
  fprintf(lg,"storing result to %s\n",outname.c_str());
  fp=Fopen(outname.c_str(), "w");
  print_comment(fp, filename);
  fprintf(fp,".e %d\n",nevents);
  fprintf(fp,".i %d\n",ninputs);
  fprintf(fp,".r %d\n",nrules);
  fprintf(fp,".n %d\n",nord);
  fprintf(fp,".d %d\n",ndisj);
  fprintf(fp,".c %d\n",nconf);
  if((nevents > 1) && ((startin != NULL) || (startstate !=NULL))) 
    { 
      fprintf(fp,".s ");
      if (startin != NULL) fprintf(fp,"%s",startin);
      if (startstate !=NULL) fprintf(fp,"%s",startstate);
      fprintf(fp,"\n");
    }
  print_comment(fp,"List of Events");
  fprintf(fp,"reset\n");
  
  //strcpy(inname,filename);
  //strcat(inname,".inp");
  string inname = filename;
  inname = string(filename) + ".inp";;
  if (ninputs > 0)
    copy_file(fp,inname.c_str(), ninputs/2,"List of Input Events");
  //strcpy(inname,filename);
  //strcat(inname,".out");
  inname = string(filename) + ".out";;
  if (nevents > ninputs+1)
    copy_file(fp,inname.c_str(),(nevents-(ninputs+ndummy)-1)/2,
	      "List of Output Events");
  //strcpy(inname,filename);
  //strcat(inname,".dum");
  inname = string(filename) + ".dum";;
  if (nevents > ninputs+1)
    copy_file(fp,inname.c_str(),ndummy,"List of Dummy Events");
  //strcpy(inname,filename);
  //strcat(inname,".rule");
  inname = string(filename) + ".rule";
  if (nrules > 0)
    copy_file(fp,inname.c_str(),nrules,"List of Causal Rules");
  //strcpy(inname,filename);
  //strcat(inname,".ord");
  inname = string(filename) + ".ord";
  if (nord > 0)
    copy_file(fp,inname.c_str(),nord,"List of Causal Ordering Rules");
  //strcpy(inname,filename);
  //strcat(inname,".merg");
  inname = string(filename) + ".merg";
  if (ndisj > 0)
    copy_file(fp,inname.c_str(),ndisj,"List of Mergers");
  //strcpy(inname,filename);
  //strcat(inname,".conf");
  inname = string(filename) + ".conf";
  if (nconf > 0)
    copy_file(fp, inname.c_str(), nconf, "List of Conflicts");
  fclose(fp);
}

/* TO HERE */

void del_symtab()
{/*
     do { SymTab *temp= table->header; 
   
          delete table;
          table= temp;
          cout<<"***************\n";
     } while(table);
     */
}              

/*****************************************************************************/
/* Strip path from filename.                                                 */
/*****************************************************************************/

char *strippath(const char *filename)
{
  char *pathname;
  int i,j;

  pathname=(char*)GetBlock((strlen(filename)+1)*sizeof(char));
  i=strlen(filename)-1;
  while (i > 0 && filename[i] != '/') i--;
  for (j=0;j<i;j++)
    pathname[j]=filename[j];
  pathname[j]='\0';
  return pathname;
}
     

/*****************************************************************************/
/* Compile CSP into a timed event-rule structure.                            */
/*****************************************************************************/

bool csp2er(string filename,string component,char command,int status,
	    bool newfile,bool fromVHDL,bool fromCSP,bool fromHSE,bool postproc,
	    bool newtab,bool redchk,bool abstract,bool verbose,bool xform2)
{
  string s,outname;
  FILE *infile = 0;
  static bool first=TRUE;
  
  compiled=TRUE;

  if ((!(status & LOADED)) || (newfile)) {
    if (fromVHDL || fromHSE)
      toTEL=TRUE;
    else
      toTEL=FALSE;
    pair<string,string> dir_file = split(filename, '/', true);
    if(dir_file.second.size() == 0)
      dir_file = make_pair(string("."), dir_file.first);
    cur_dir = dir_file.first;
    //if (first) 
    initialize_parser(dir_file.second, first);
    first=FALSE;
  /*
  if (fromVHDL){
    ieee_path = getenv("IEEELIB");
    if(ieee_path != 0) 
      s =  ieee_path + string("/standard.vhd");
    else{
      printf("warning: IEEELIB not set.\n");
      fprintf(lg,"warning: IEEELIB not set.\n");
      s = "./standard.vhd";
    }
    if((infile = fopen(s.c_str(), "r")) == 0){
      printf("ERROR: Unable to open %s!\n", s.c_str());
      fprintf(lg,"ERROR: Unable to open %s!\n", s.c_str());
      return FALSE;
    }
  
    yyin = infile;
    file_scope1 = copy_string(s.c_str());
    lexinit();
    pre_compile = true;
    yyparse();
    }*/

    if(dir_file.first.size())
      outpath = copy_str(dir_file.first.c_str());
    else
      outpath = 0;
    
    string inname = filename;
    if (fromVHDL) 
      inname += ".vhd";
    else if (fromHSE)
      inname += ".hse";
    else 
      inname += ".csp";
    printf("Compiling %s ... \n", inname.c_str());
    fflush(stdout);
    fprintf(lg,"Compiling %s ... \n", inname.c_str());
    fflush(lg);
    
    if((infile = fopen(inname.c_str(), "r")) == 0){
      printf("ERROR: Cannot open %s!\n", inname.c_str());
      fprintf(lg,"ERROR: Cannot open %s!\n", inname.c_str());
      return(FALSE);
    }

    name_mapping = new map<string, string>;
    file_scope1 = copy_string(inname.c_str());
    yyin = infile;
    lexinit();
    pre_compile = false;

    if(!yyparse() && compiled==TRUE){
      if(component.size()){
	// If using abstraction, component.size() != 0.

	// Splits <component>.<component> 
	// Should be extended to allow deeper nesting.
	pair<string,string> module_id = split(component);
	
	tels_table *rtp = 0, tmp_tel;
	if(!fromVHDL &&
	   open_module_list->find(module_id.first) != open_module_list->end())
	  rtp = (*open_module_list)[module_id.first].first;
	
	// If this's VHDL file, find the tels_table with the given name. 
	// 'module_id.first' is the entity ID, 'string()' means find the 
	// first tels_tbale for the given entity.
	//else if(!open_entity_lst->find(module_id.first,string()).empty())
	//rtp = &open_entity_lst->find(module_id.first,string());	
	
	TERstructADT l2 = 0;
	if(rtp){
	  if(module_id.second.size())
	    l2 = rtp->tel(module_id.second, abstract);
	  else
	    l2 = rtp->compose();
	}
	else{	  
	  l2 = tel_tb->tel(Tolower(component), abstract);
	  tmp_tel = tel_tb->find(Tolower(component));
	  rtp = &tmp_tel;
	}

	if(l2){
	  telADT tmp=l2;

	  if (redchk) rm_red_top(tmp, verbose, 0, true); //false);
	  if (postproc) tmp=post_proc(l2, redchk, verbose, false, 
				      xform2,fromVHDL || fromHSE);
	  if (redchk) rm_red_top(l2, verbose, 0, true);
	  //for(actionsetADT E=l2->A; E; E=E->link)
	  //cout<< E->action->label<<'/'<<E->action->type<< endl;
	  emitters(0, component.c_str(), l2);
	  //TERSdelete(tmp);  TERSdelete(l2);

	  // This code is used by storevhd.
	  map<string,string>::const_iterator begin=rtp->portmap().begin();
	  map<string,string>::const_iterator end=rtp->portmap().end();
	  for(map<string,string>::const_iterator iter=begin; iter!=end;
	      iter++) {
	    top_port.insert(iter->second);
	    top_port_map.insert(make_pair(iter->second,iter->first));
	  }
	}
	else {
      	  cout << "ERROR: Component '" << component << "' not found.\n";
	  compiled=FALSE;
	}
      }
      else{
	TERstructADT result = 0;	
	if(tel_tb) {
	  result = tel_tb->compose();
	  if (redchk) rm_red_top(result, verbose, 0, true); //false);
	  if (postproc) post_proc(result, redchk, verbose, false, 
				  xform2,fromVHDL || fromHSE);
	  if (redchk) rm_red_top(result, verbose, 0, true);
	  emitters(outpath, dir_file.second.c_str(), result);
	  TERSdelete(result);
	}
      }
    }

    delete[] outpath;

    if(newtab) {
      free_parser(infile);
      first=TRUE;
    }
  }
  if (!compiled) {
    printf("ERROR: Compilation failed due to above errors.\n");
    fprintf(lg,"ERROR: Compilation failed due to above errors.\n");
    if (fromCSP)
      outname = filename + ".er";
    else 
      outname = filename + ".tel";
    if ((fp=fopen(outname.c_str(),"r"))!=NULL) {
      fclose(fp);
      outname = "rm " + outname;
      system(outname.c_str());
    }
  }
  
#ifdef DDEBUG
  /*  cout<< "Entering post processing debuggin mode. Type q to quit."<< endl; 
  while(1){
    string input, f_id;
    cin >> input >> f_id;
    if(input=="q" || input=="Q")
      break;
    else{
      if(pproc_debug.find(input)==pproc_debug.end())
	cout<< "not a dummy event."<< endl;
      else{
	telADT tmp=TERScopy(pproc_debug[input]);
	emitters(0, f_id.c_str(), tmp);
	TERSdelete(tmp);
      }
    }
    } */

  for(map<string,telADT>::iterator iter=pproc_debug.begin(); 
      iter!=pproc_debug.end(); iter++)
    TERSdelete(iter->second);

    pproc_debug=map<string,telADT>();
#endif

    delete name_mapping;
    name_mapping = 0;
    return(compiled);
}
  



/*****************************************************************************/
/* Compile CSP into a timed event-rule structure.                            */
/*****************************************************************************/

bool compile(char command,designADT design,bool newfile)
{
  mingatedelay = design->mingatedelay;
  maxgatedelay = design->maxgatedelay;
  if (!csp2er(design->filename,design->component,command,design->status,
	      newfile,design->fromVHDL,design->fromCSP,design->fromHSE,
	      design->postproc,design->newtab,design->redchk,
	      design->abstract,design->verbose,design->xform2)) {
    new_design(design,FALSE);
    design->signals=new_signals(design->status,TRUE,design->oldnsignals,
				design->nsignals,design->signals);
    design->events=new_events(design->status,TRUE,design->oldnevents,
			      design->nevents,design->events);
    design->si=FALSE;
    design->status=RESET;
    return(FALSE);
  }
  if (design->component[0]!='\0')
    strcpy(design->filename,design->component);
  return(TRUE);
}


void initialize_parser(const string& f_id, bool first)
{
  if(first==true){
    new_action_table("global");
    new_event_table("global");
    new_rule_table("global");
    new_conflict_table("global");
    cur_entity = "global";
    table = new SymTab;
    open_entity_lst = new vhd_tel_tb;
    open_module_list = new csp_tel_tb;
  }
  PSC = 0;
  linenumber = 1;
  tel_tb = 0;
  cur_file = f_id;
  top_port=set<string>();
  top_port_map=map<string,string>();
}


void free_parser(FILE *infile)
{
  free_action_table();  
  free_event_table();
  free_rule_table();
  free_conflict_table();
  csp_tel_tb::iterator b;
  for(b = open_module_list->begin(); b != open_module_list->end(); b++)
    delete b->second.first;
  delete open_module_list;
  delete open_entity_lst;
  delete table;
}



