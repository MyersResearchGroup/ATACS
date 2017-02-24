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
/* drawnet.c                                                                 */
/*****************************************************************************/

#include "storeprs.h"
#include "drawnet.h"

void print_signal_name(FILE *fp,char *signame)
{
  for (unsigned int i=0;i<strlen(signame);i++)
    if (signame[i]=='?')
      fprintf(fp,"R");
    else if (signame[i]=='!')
      fprintf(fp,"S");
    else 
      fprintf(fp,"%c",signame[i]);
}

/*****************************************************************************/
/* Print the gC's                                                            */
/*****************************************************************************/

int output_gc(FILE *fp,signalADT *signals,char * set,char * reset,int nsignals,
	      int r,int m,int maxsize)
{
  int j,countset,countreset;

  if ((!set) || (!reset)) return 0;
  countset=0;
  for (j=0;j<nsignals;j++)
    if ((set[j]=='1') || (set[j]=='0')) countset++;
  
  countreset=0;
  for (j=0;j<nsignals;j++)
    if ((reset[j]=='1') || (reset[j]=='0')) countreset++;

  for (j=0;j<nsignals;j++){
    if (set[j]=='0') {
      if ((countset > maxsize) || (countreset > maxsize)) {
	fprintf(fp," NOT%d [shape=diamond,color=red,label=NOT];\n",r);
	if (reset[j]=='1')
	  fprintf(fp," NOT%d -> C%d [color=red];\n",r,m);
	else 
	  fprintf(fp," NOT%d -> C%d [label=\"+\",color=red,fontsize=20];\n",
		  r,m);
      } else {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",r);
	if (reset[j]=='1')
	  fprintf(fp," NOT%d -> C%d;\n",r,m);
	else 
	  fprintf(fp," NOT%d -> C%d [label=\"+\",fontsize=20];\n",r,m);
      }
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"_%d [shape=plaintext,label=",r);
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"];\n");
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      if ((countset > maxsize) || (countreset > maxsize)) 
	fprintf(fp,"_%d -> NOT%d [color=red];\n",r,r);
      else
	fprintf(fp,"_%d -> NOT%d;\n",r,r);
      r++;
    } else if (set[j]=='1') {
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"_%d [shape=plaintext,label=",r);
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"];\n");
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      if ((countset > maxsize) || (countreset > maxsize)) {
	if (reset[j]=='0')
	  fprintf(fp,"_%d -> C%d [color=red];\n",r,m);
	else
	  fprintf(fp,"_%d -> C%d [label=\"+\",color=red,fontsize=20];\n",r,m);
      } else {
	if (reset[j]=='0')
	  fprintf(fp,"_%d -> C%d;\n",r,m);
	else
	  fprintf(fp,"_%d -> C%d [label=\"+\",fontsize=20];\n",r,m);
      }
      r++;
    } else if (reset[j]=='1') {
      if ((countset > maxsize) || (countreset > maxsize)) {
	fprintf(fp," NOT%d [shape=diamond,color=red,label=NOT];\n",r);
	fprintf(fp," NOT%d -> C%d [label=\"-\",color=red,fontsize=20];\n",r,m);
      } else {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",r);
	fprintf(fp," NOT%d -> C%d [label=\"-\",fontsize=20];\n",r,m);
      }
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"_%d [shape=plaintext,label=",r);
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"];\n");
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      if ((countset > maxsize) || (countreset > maxsize)) 
	fprintf(fp,"_%d -> NOT%d [color=red];\n",r,r);
      else
	fprintf(fp,"_%d -> NOT%d;\n",r,r);
      r++;
    } else if (reset[j]=='0') {
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"_%d [shape=plaintext,label=",r);
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"];\n");
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      if ((countset > maxsize) || (countreset > maxsize)) 
	fprintf(fp,"_%d -> C%d [label=\"-\",color=red,fontsize=20];\n",r,m);
      else
	fprintf(fp,"_%d -> C%d [label=\"-\",fontsize=20];\n",r,m);
      r++;
    }
  } 
  return r;
}

/*****************************************************************************/
/* Print a guard.                                                            */
/*****************************************************************************/

int output_dot_guard(FILE *fp,signalADT *signals,char * cover,int nsignals,
		     int i,bool comb,int o,int n,int p,
		     int sands,int rands,int m,int maxsize)
{
  int j,literals;

 
  /* OUTPUT A NEW AND GATE */

  literals=0;
  for (j=0;j<nsignals;j++)
    if ((cover[j]=='1') || (cover[j]=='0')) literals++; 

  if (literals > 1) {
    if(literals > maxsize) 
      fprintf(fp," AND%d [shape=box,color=red,label=AND];\n",o);
    else fprintf(fp," AND%d [shape=box,label=AND];\n",o);

/*  fprintf(fp," sands = %d;\n",sands);
  fprintf(fp," rands = %d;\n",rands); */

  /* WIRE AND GATE TO THE RIGHT OR GATE */

  /*   If (i % 2==1) then hook to set OR gate */
  /*   If (i % 2==0) then hook to reset OR gate */
  /*   where these gates are labeled with signals[(i-1)/2]->name */

    if (i % 2==1) {
      if (sands > 1)
	fprintf(fp," AND%d -> OR%d;\n",o,(n-1));
      else if (!comb)
	fprintf(fp," AND%d -> C%d;\n",o,m);
      else {
	fprintf(fp," AND%d -> ",o);
	print_signal_name(fp,signals[(i-1)/2]->name);
	fprintf(fp,";\n");
      }
    }
    if (i % 2==0) {
      if (rands > 1)
	fprintf(fp," AND%d -> OR%d;\n",o,n);
      else {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",m);
	fprintf(fp," AND%d -> NOT%d;\n",o,m);
	fprintf(fp," NOT%d -> C%d;\n",m,m);
      }
    }
  }
  for (j=0;j<nsignals;j++){ 
    if (cover[j]=='1') {
      /* Wire signals[j]->name to input of AND gate */

      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"_%d [shape=plaintext,label=",o);
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"];\n");
      if (literals > 1) {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	if(literals > maxsize) 
	  fprintf(fp,"_%d -> AND%d [color=red];\n",o,o);
        else fprintf(fp,"_%d -> AND%d;\n",o,o); 	
      } else if (((i % 2==1) && (sands > 1)) || ((i % 2==0) && (rands > 1))) {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> OR%d;\n",o,(n-1));
      } else if ((i % 2==1) && (!comb)) {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> C%d;\n",o,m);
      } else if (i % 2==1) {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> ",o);
	print_signal_name(fp,signals[(i-1)/2]->name);
	fprintf(fp,";\n");
      } else {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> NOT%d;\n",o,m);
	p++;
      }
    } else if (cover[j]=='0') {
      /* Wire signals[j]->name to input of a NOT gate feeding the AND gate */
       
      fprintf(fp," ");
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"_%d [shape=plaintext,label=",o);
      print_signal_name(fp,signals[j]->name);
      fprintf(fp,"];\n");
      if (literals > 1) {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",p);
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> NOT%d;\n",o,p);
	if(literals > maxsize) 
	  fprintf(fp," NOT%d -> AND%d [color=red];\n",p,o);
        else 
	  fprintf(fp," NOT%d -> AND%d;\n",p,o); 	
	p++;
      } else if (((i % 2==1) && (sands > 1)) || ((i % 2==0) && (rands > 1))) {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",p);
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> NOT%d;\n",o,p);
	fprintf(fp," NOT%d -> OR%d;\n",p,n);
	p++;
      } else if ((i % 2==1) && (!comb)) {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",p);
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> NOT%d;\n",o,p);
	fprintf(fp," NOT%d -> C%d;\n",p,m);      
	p++;
      } else if (i % 2==1) {
	fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",p);
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> NOT%d;\n",o,p);
	fprintf(fp," NOT%d -> %s;\n",p,signals[(i-1)/2]->name);
	p++;
      } else if (!comb) {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> C%d;\n",o,m);
      } else {
	fprintf(fp," ");
	print_signal_name(fp,signals[j]->name);
	fprintf(fp,"_%d -> ",o);
	print_signal_name(fp,signals[(i-1)/2]->name);
	fprintf(fp,";\n");
      }
    } 
  }
  return p;
}
    
/*****************************************************************************/
/* Store production rules.                                                   */
/*****************************************************************************/

void draw_net(char * filename,signalADT *signals,stateADT *state_table,
	      regionADT *regions,int ninpsig,int nsignals,int maxsize,
	      bool gatelevel,bool combo)
{
  char outname[FILENAMESIZE];
  FILE *fp;
  int i,j,tempi,k,m,n,o,p,r,countset,countreset;
  int *fanout;
  int sands,rands,area;
  regionADT cur_region;
  int oldp;
  char *  set;
  char *reset;

  strcpy(outname,filename);
  strcat(outname,".dot");
  printf("Generating dot file for net:  %s\n",outname);
  fprintf(lg,"Generating dot file for net:  %s\n",outname);
  fp=Fopen(outname,"w");
  area=0;

  /* ADD PREAMBLE FOR DOT FILE */

  fprintf(fp,"digraph G {\n");
  fprintf(fp,"size=\"7.5,10\"\n");
  fprintf(fp," rankdir=LR;\n");
  fprintf(fp," edge [dir=none];\n");

  k=0;
  m=0;
  n=0;
  o=0;
  p=(nsignals-ninpsig+1);
  r=50;

  fanout=(int*)GetBlock((2*nsignals+1)*sizeof(int));
  for (i=0;i<(2*nsignals+1);i++) fanout[i]=0;
  for (i=2*ninpsig;i<(2*nsignals+1);i++) {

    if (i % 2==1) {

    /* ADD C-ELEMENT signals[(i-1)/2]->name AND OR OUTPUT HERE */

      sands=0;
      set=NULL;
      for (cur_region=regions[i];cur_region;cur_region=cur_region->link) {
	if (cur_region->cover[0]!='E') {
	  set=cur_region->cover;
	  sands++;
	}
      }

      rands=0;
      reset=NULL;
      for (cur_region=regions[i+1];cur_region;cur_region=cur_region->link) {
	if (cur_region->cover[0]!='E') {
	  reset=cur_region->cover;
	  rands++;
	}
      }
   
      m++;
      n++;
      fprintf(fp," ");
      print_signal_name(fp,signals[(i-1)/2]->name);
      fprintf(fp," [shape=plaintext];\n");

      /* UNCOMMENT TO ADD GCs    wire gC  ->  signals[(i-1)/2]->name     
                           wire signals[j]->name  ->  gC */
      countset=0;
      if (sands > 0)
	for (j=0;j<nsignals;j++)
	  if (set)
	    if ((set[j]=='1') || (set[j]=='0')) countset++;
  
      countreset=0;
      if (rands > 0)
	for (j=0;j<nsignals;j++)
	  if (reset)
	    if ((reset[j]=='1') || (reset[j]=='0')) countreset++;

      if ((!gatelevel) && (sands==1) && (rands==1)) {
        if((countset > maxsize) || (countreset > maxsize)){        
	 fprintf(fp," C%d [shape=circle,color=red,label=gC];\n",m);
	 fprintf(fp," C%d -> ",m);
	 print_signal_name(fp,signals[(i-1)/2]->name);
	 fprintf(fp," [color=red];\n");
	 r++;
	 r=output_gc(fp,signals,set,reset,nsignals,r,m,maxsize);
	}else{
	 fprintf(fp," C%d [shape=circle,label=gC];\n",m);
	 fprintf(fp," C%d -> ",m);
	 print_signal_name(fp,signals[(i-1)/2]->name);
	 fprintf(fp,";\n");
	 r++;
	 r=output_gc(fp,signals,set,reset,nsignals,r,m,maxsize);
	}
      } else if (sands > 0 && rands > 0) {
	fprintf(fp," C%d [shape=circle,label=C];\n",m);
	fprintf(fp," C%d -> ",m);
	print_signal_name(fp,signals[(i-1)/2]->name);
	fprintf(fp,";\n");
	if (sands > 1) {
	  fprintf(fp," OR%d [shape=box,label=OR];\n",n);
	  fprintf(fp," OR%d -> C%d;\n",n,m);
	}

	n++;
	for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
	  if ((cur_region->cover[0] != 'E')) {
	    k++;
	    o++;
	    p=output_dot_guard(fp,signals,cur_region->cover,nsignals,i,
			       false,o,n,p,sands,rands,m,maxsize);
	  }

	oldp=p;
	tempi=i;
	i++;
	for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
	  if ((cur_region->cover[0] != 'E')) {
	    k++;
	    o++;
	    p=output_dot_guard(fp,signals,cur_region->cover,nsignals,i,
			       false,o,n,p,sands,rands,m,maxsize);
	  }
	i=tempi;

	if (p!=oldp) fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",m);
	if (rands > 1) {
	  fprintf(fp," OR%d [shape=box,label=OR];\n",n);
	  if (p!=oldp)
	    fprintf(fp," OR%d -> NOT%d -> C%d;\n",n,m,m);
	  else
	    fprintf(fp," OR%d -> C%d;\n",n,m);
	} else if (p!=oldp) 
	  fprintf(fp," NOT%d -> C%d;\n",m,m);

      } else if (rands==0) {
	if (sands > 1) {
	  fprintf(fp," OR%d [shape=box,label=OR];\n",n);
	  fprintf(fp," OR%d -> ",n);
	  print_signal_name(fp,signals[(i-1)/2]->name);
	  fprintf(fp,";\n");
	}

	n++;
	for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
	  if ((cur_region->cover[0] != 'E')) {
	    k++;
	    o++;
	    p=output_dot_guard(fp,signals,cur_region->cover,nsignals,i,
			       true,o,n,p,sands,rands,m,maxsize);
	  }

      } else {
	oldp=p;
	n++;
	for (cur_region=regions[i];cur_region;cur_region=cur_region->link)
	  if ((cur_region->cover[0] != 'E')) {
	    k++;
	    o++;
	    p=output_dot_guard(fp,signals,cur_region->cover,nsignals,i,
			       true,o,n,p,sands,rands,m,maxsize);
	  }
	if (p!=oldp) fprintf(fp," NOT%d [shape=diamond,label=NOT];\n",m);
	if (rands > 1) {
	  fprintf(fp," OR%d [shape=box,label=OR];\n",(n+1));
	  if (p!=oldp) {
	    fprintf(fp," OR%d -> NOT%d -> ",(n+1),m);
	    print_signal_name(fp,signals[(i-1)/2]->name);
	    fprintf(fp,";\n");
	  } else {
	    fprintf(fp," OR%d -> ",(n+1));
	    print_signal_name(fp,signals[(i-1)/2]->name);
	    fprintf(fp,";\n");
	  }

	} else if (p != oldp) {
	  fprintf(fp," NOT%d -> ",m);
	  print_signal_name(fp,signals[(i-1)/2]->name);
	  fprintf(fp,";\n");
	}
      }
 
    }
    
  }

  /* ADD CLOSING FOR DOT FILE */

  fprintf(fp,"}\n");

  fclose(fp);
  free(fanout);
}

/*****************************************************************************/
/* Draw net for dot.                                                         */
/*****************************************************************************/

void draw_dot_net(char command,designADT design,bool print)
{
  char shellcommand[100];

  if (!(design->single)) {
    printf("Display net only works when using SingleCube algorithm\n");
    fprintf(lg,"Display net only works when using SingleCube algorithm\n");
  } else if (design->status & FOUNDCOVER) {  
    draw_net(design->filename,design->signals,design->state_table,
	     design->regions,design->ninpsig,design->nsignals,
	     design->maxsize,design->gatelevel,design->combo);
    if (print)
      sprintf(shellcommand,"printnet %s &",design->filename);
    else 
      sprintf(shellcommand,"shownet %s &",design->filename); 
    printf("Executing command:  %s\n",shellcommand);
    fprintf(lg,"Executing command:  %s\n",shellcommand);
    system(shellcommand);
  } else {
    printf("Display net only works after finding a cover.\n");
    fprintf(lg,"Display net only works after finding a cover.\n");
  }
}
