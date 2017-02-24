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
/* bddrsg.c                                                                 */
/*****************************************************************************/
#ifdef DLONG
#include "bddrsg.h"
#include "memaloc.h"
#include <bdduser.h>
#include <Bdd.h>
#include <bv.h>
#include <cmath>
#ifdef TIMER
#include <sys/time.h>
#include <unistd.h>
#endif
#define bdd_terminal_id_fn ((char *(*)())0)

typedef struct enabled_node{
  struct enabled_node *next;
  int sig;
} e_node;

typedef struct reg_node{
  struct reg_node *next;
  bdd ER;
  bdd QR;
  bdd VC;
  int size;
} reg_node;

// typedef struct cov_node{
//   struct cov_node *next;
//   bdd  C;
//   bdd NOTC;
//   char **psiids;
//   bdd *psilits;
// } cov_node;

// typedef struct coverlist{
//   struct cov_node *head;
//   int num_covers;
// } coverlist;

// typedef struct{
//   char  **ids;
//   bdd  *lits;
//   char  **psiids;
//   coverlist covers;
//   int nlits;
//   int gamma;
//   int phi;
// }env;

/*****************************************************************************/
/* provide names for variable bdds                                           */
/*****************************************************************************/

/* STRANGENESS WITH A NULL IN PRINT BDD TK */

char* naming_fn(bdd_manager bddm,bdd f,env *e){
  int i;
  cov_node *temp;
  /* debug 8*/
  for(i = 0;i<e->nlits;i++)
    if ((f == e->lits[i])||((unsigned int)f == ((unsigned int)e->lits[i]+1)))
      return e->ids[i];    
  /*  for(i = 0;i<e->nlits;i++)
    printf("i=%d f=%d e->lits=%d e->ids=%s\n",
	   i,f,e->lits[i],e->ids[i]); */
  for (temp = e->covers.head;temp;temp = temp->next) 
    for(i = 0;i<e->nlits;i++)
    if ((f == temp->psilits[i])||
	((unsigned int)f == ((unsigned int)temp->psilits[i]+1)))
	return temp->psiids[i];
/*
  for (temp = e->covers.head;temp;temp = temp->next) 
    for(i = 0;i<e->nlits;i++)
      printf("i=%d f=%d e->psilits=%d e->psiids=%s\n",
	     i,f,temp->psilits[i],e->psiids[i]); */
  return "UNKNOWN"; /*NULL;*/
  /*return "ghost";*/
}

/*****************************************************/
/* for printing covers, NOT CORRECT YET!!!!!!!!!!!!!!*/
/*****************************************************/
char* psi_naming_fn(bdd_manager bddm,bdd f,env *e){
  int i;
  cov_node *temp;
  for (temp = e->covers.head;temp;temp = temp->next) 
    for(i = 0;i<(2*e->nlits);i++)
      if (f == temp->psilits[i])
	return e->ids[i/2];    
  return NULL;
  /*return "ghost";*/
}

/*****************************************************************************/
/* trans: f:= bdd to be translateded; map:= predefined translation map       */
/*****************************************************************************/

bdd bdd_TRANS(bdd_manager bddm, bdd f,int map){
  bdd_assoc(bddm,map);
  return bdd_substitute(bddm,f);
}

/*****************************************************************************/
/* imply: f + ~g                                                             */
/*****************************************************************************/

bdd bdd_IMPLY(bdd_manager bddm, bdd f,bdd g){
  bdd temp, temp2;
  temp = bdd_not(bddm,f);
  temp2 = bdd_or(bddm,g,temp);
  bdd_free(bddm,temp);
  return temp2;
}

/*****************************************************************************/
/* EDIS: f:= bdd to be reduced; g:= NULL terminated list of variables to be  */
/*  eliminated                                                               */
/*****************************************************************************/

bdd bdd_EDIS(bdd_manager bddm, bdd f,bdd* g){
  bdd_temp_assoc(bddm,g,0);
  bdd_assoc(bddm,-1);
  return bdd_exists(bddm,f);
}

/*****************************************************************************/
/* ECON: f:= bdd to be reduced; g:= NULL terminated list of variables to be  */
/*  eliminated                                                               */
/*****************************************************************************/

bdd bdd_ECON(bdd_manager bddm, bdd f,bdd* g){
  bdd_temp_assoc(bddm,g,0);
  bdd_assoc(bddm,-1);
  return bdd_forall(bddm,f);
}

/*****************************************************************************/
/* expand NextSpace BDD                                                      */
/*****************************************************************************/
void NextSpace(bdd *n,stateADT curstate,bdd_manager bddm,env* e,
	       bdd *newlits,e_node *esigs,bdd s,int k,int *ghostflag)
{
  statelistADT stepper = NULL;
  bdd s_prime,t,temp;
  int doneit,flag,j;
  e_node *temp_node, *node2;

  /* construct nextspace */
  if (curstate->succ)
    for (stepper=curstate->succ;stepper;stepper=stepper->next){
      /* printf("linked to %s\n",stepper->stateptr->state); */
      e->gamma++;
      s_prime = bdd_one(bddm);
      for (j=0;j<k;j++){
	doneit = -1;
	flag = 0;
	if (( stepper->stateptr->state[j]=='0')||
	    ( stepper->stateptr->state[j]=='R')){
	  t = bdd_not(bddm,newlits[j]);
	  flag = 1;
	  if ((curstate->state[j]=='1')||(curstate->state[j]=='F'))
	    doneit = j;
	}
	else{
	  t = newlits[j];
	  if ((curstate->state[j]=='0')||(curstate->state[j]=='R'))
	    doneit = j;
	}
	temp = bdd_and(bddm,s_prime,t);
	if (flag)
	  bdd_free(bddm,t);
	bdd_free(bddm,s_prime);
	s_prime = temp;/*stupid!!!!*/

	/* remove this signal from list */
	if ((doneit != -1)&&(esigs)){
	  temp_node = esigs;
	  if (esigs->sig == doneit){
	    esigs = esigs->next;
	    free(temp_node);
	  }
	  else{
	    while (temp_node->next){
	      if (temp_node->next->sig == doneit){
		node2 = temp_node->next;
		temp_node->next =  temp_node->next->next;
		free(node2);
	      }
	      else
		temp_node = temp_node->next;
	    }
	  }
	}
      }
      temp = bdd_and(bddm,s_prime,s);
      bdd_free(bddm,s_prime);
      s_prime = temp;
      temp = bdd_or(bddm,*n,s_prime);
      bdd_free(bddm,*n);
      bdd_free(bddm,s_prime);
      *n = temp;
    }
  if (esigs&&!(*ghostflag)){
    /* printf("ghost states exist!!!!\n"); */
    *ghostflag = 1;
  }
  /*   next step: add ghost states!!!!       */
  while (esigs){
    s_prime = bdd_one(bddm);
    for (j=0;j<k;j++){
      flag=0;
      if (j==esigs->sig){
	if (( curstate->state[j]=='0')||( curstate->state[j]=='R')){
	  t = newlits[j];
	}
	else
	  t = bdd_not(bddm,newlits[j]);
	flag=1;
      }
      else{
 	if (( curstate->state[j]=='0')||( curstate->state[j]=='R')){
	  t = bdd_not(bddm,newlits[j]);
	  flag=1;
	}
	else
	  t = newlits[j];
      }
      temp = bdd_and(bddm,s_prime,t);
      if (flag)
	bdd_free(bddm,t);
      bdd_free(bddm,s_prime);
      s_prime = temp;
    }
    temp = bdd_and(bddm,s_prime,s);
    bdd_free(bddm,s_prime);
    s_prime = temp;
    temp = bdd_or(bddm,*n,s_prime);
    bdd_free(bddm,*n);
    bdd_free(bddm,s_prime);
    *n = temp;
    temp_node = esigs;
    esigs = esigs->next;
    free(temp_node);
  }
}

/*****************************************************************************/
/* create StateSpace BDD                                                     */
/*****************************************************************************/
void StateSpace(bdd *N,bdd *S,stateADT *state_table,bdd_manager bddm,
		bdd *lits,bdd *newlits, env* e)
{
  int i,j,k,flag,ghostflag;
  stateADT curstate;
  bdd n,s,t,u,temp;
  e_node *esigs,*temp_node;
  
  esigs=NULL;
  flag=0;
  ghostflag=0;
  u = bdd_zero(bddm);
  n = bdd_zero(bddm);
  for (i=0;i<PRIME;i++)
    for (curstate=state_table[i];
	 curstate != NULL && curstate->state != NULL;
	 curstate=curstate->link) {
      /*  debug 4 */
      /* curstate->state is a string containing a state */
      s = bdd_one(bddm);
      e->phi++;
      /* printf("%s\n",curstate->state); */
      k=strlen(curstate->state);
      for (j=0;j<k;j++){
	flag = 0;
	if (( curstate->state[j]=='0')||( curstate->state[j]=='R')){
	  t = bdd_not(bddm,lits[j]);
	  flag = 1;
	}
	else
	  t = lits[j];
	temp = bdd_and(bddm,s,t);
	if (flag)
	  bdd_free(bddm,t);
	bdd_free(bddm,s);
	s = temp;
	/* construct list of enabled sugnals */
	if (( curstate->state[j]=='F')||( curstate->state[j]=='R')){
	  temp_node = esigs;
	  esigs = (e_node*)GetBlock(sizeof(e_node));
	  esigs->sig = j;
	  esigs->next = temp_node;
	}
      }
      NextSpace(&n, curstate, bddm,e, newlits, esigs,s,k, &ghostflag);
      /*  debug 5 */
      temp = bdd_or(bddm,u,s);
      bdd_free(bddm,s);
      bdd_free(bddm,u);
      u = temp;
      esigs = NULL;
    }
  /* more coherent names */
  *N = n;
  *S = u;
}
/*****************************************************************************/
/* find ERs                                             */
/*****************************************************************************/
reg_node *FindER(int map2main,int map2prime,bdd_manager bddm,bdd t,
	    bdd *lits,bdd *newlits,bdd N,env *e)
{ 
  reg_node *temp,*dest = NULL;
  bdd s,u,v,w;



  bdd_unfree(bddm,t);
  while(t != bdd_zero(bddm)){
    s = bdd_satisfy(bddm,t);
    u = NULL;
    while(s!=u){
      if (u)
	bdd_free(bddm,u);
      u = s;
      v = bdd_TRANS(bddm,u,map2prime);
      s = bdd_and(bddm,t,v);
      bdd_free(bddm,v);
      v = bdd_and(bddm,s,N);
      bdd_free(bddm,s);
      s = bdd_EDIS(bddm,v,newlits);
      bdd_free(bddm,v);
      v = bdd_TRANS(bddm,t,map2prime);
      w = bdd_and(bddm,u,v);
      bdd_free(bddm,v);
      v = bdd_and(bddm,w,N);
      bdd_free(bddm,w);
      w = bdd_EDIS(bddm,v,lits);
      bdd_free(bddm,v);
      v = bdd_TRANS(bddm,w,map2main);
      bdd_free(bddm,w);
      w = bdd_or(bddm,s,v);
      bdd_free(bddm,v);
      bdd_free(bddm,s);
      s = bdd_or(bddm,u,w);
      bdd_free(bddm,w);
    } 
    temp = dest;
    dest = (reg_node*)GetBlock(sizeof(reg_node));
    dest->next = temp;
    dest->ER = s;
    u = bdd_not(bddm,s);
    bdd_free(bddm,s);
    s = bdd_and(bddm,t,u);
    bdd_free(bddm,t);
    t = s;
  }
  return dest;
}
/*****************************************************************************/
/* Expand potential covers.                                                  */
/*****************************************************************************/
void ExpandCover(bdd *C, bdd *NOTC, bdd* lits, env *e, 
		 bdd_manager bddm, int nsigs, cov_node *old )
{
  cov_node *step;
  int j = e->covers.num_covers++,k;
  bdd t,u,v,w;
  
  step = old->next;
  if (!step){
    step = old->next = (cov_node *)GetBlock(sizeof(cov_node));
    step->next = NULL;
    step->psilits = (bdd*)GetBlock((2*nsigs)*sizeof(bdd));
    step->psiids = (char **)GetBlock(2*nsigs*sizeof(char *));
    for(k=0;k<nsigs;k++){
      step->psilits[k+nsigs] = bdd_new_var_after(bddm,lits[k]);
      step->psilits[k] = bdd_new_var_after(bddm,lits[k]);
      step->psiids[k]=(char *)GetBlock((strlen(e->psiids[k])+3)*sizeof(char));
      sprintf(step->psiids[k],"%s%d",e->psiids[k],j);
      step->psiids[k+nsigs]=(char *)GetBlock((strlen(e->psiids[k+nsigs])+3)*
					     sizeof(char));
      sprintf(step->psiids[k+nsigs],"%s%d",e->psiids[k+nsigs],j);
    }
    w = bdd_one(bddm);
    for (k = 0;k<nsigs;k++){
      t = bdd_IMPLY(bddm,step->psilits[k],lits[k]);
      u = bdd_and(bddm,w,t);
      bdd_free(bddm,t);
      bdd_free(bddm,w);
      t = bdd_not(bddm,lits[k]);
      v = bdd_IMPLY(bddm,step->psilits[k+nsigs],t);
      w = bdd_and(bddm,u,v);
      bdd_free(bddm,t);
      bdd_free(bddm,u);
      bdd_free(bddm,v);
    }
    *C = bdd_or(bddm,w,old->C);
    bdd_free(bddm,w);
    *NOTC = bdd_not(bddm,*C);
    step->C = *C;
    step->NOTC = *NOTC;
    
  }
  else{
    *C = step->C;
    *NOTC = step->NOTC;
  }
}
/*****************************************************************************/
/* Find Valid covers                                                         */
/*****************************************************************************/
void FindCover(bdd N,bdd S,int map2main,int map2prime,bdd_manager bddm,
	       bool gatelevel,bdd C,bdd NOTC,bdd *lits,bdd *ES,bdd *QS,
	       reg_node** ER,int i,env* e,int nsigs)
{
  reg_node *temp;
  bdd t,u,v,w,NOTV;
  cov_node *step;
  /* int count = 0;*/
  /*  printf("gothere!(i = %d)\n",i);*/


  if (!gatelevel) {
    t = bdd_not(bddm,ES[i]);
    u = bdd_not(bddm,QS[i]);
    v = bdd_and(bddm,t,u);
    w = bdd_and(bddm,S,v);
    NOTV = bdd_not(bddm,w);
    bdd_free(bddm,t);
    bdd_free(bddm,u);
    bdd_free(bddm,v);
    bdd_free(bddm,w);
  }
  for(temp = ER[i];temp;temp = temp->next){
    /*printf("\tVC:%d\n",count++);*/
    temp->VC = bdd_zero(bddm);
    temp->size = 1;
    step = e->covers.head;
    while(temp->VC == bdd_zero(bddm)){
      if (gatelevel){
	t = bdd_not(bddm,temp->ER);
	u = bdd_not(bddm,QS[i]);
	v = bdd_and(bddm,t,u);
	bdd_free(bddm,t);
	bdd_free(bddm,u);
	/* w = V1 */
	w = bdd_and(bddm,S,v);
	bdd_free(bddm,v);
	t = bdd_TRANS(bddm,C,map2prime);
	u = bdd_not(bddm,C);
	/* v = ~C(main)^C(prime)*/
	v = bdd_and(bddm,t,u);
	bdd_free(bddm,t);
	bdd_free(bddm,u);
	t = bdd_and(bddm,N,v);
	bdd_free(bddm,v);
	/* t = N(main,prime)^~C(main)^C(prime)*/
	u = bdd_TRANS(bddm,temp->ER,map2prime);
	v = bdd_not(bddm,u);
	bdd_free(bddm,u);
	u = bdd_and(bddm,v,t);
	bdd_free(bddm,t);
	bdd_free(bddm,v);
	t = bdd_EDIS(bddm,u,lits);
	/* u = V2 */
	v = bdd_TRANS(bddm,t,map2main);
	bdd_free(bddm,t);
	bdd_free(bddm,u);
	t = bdd_or(bddm,w,v);
	NOTV = bdd_not(bddm,t);
	bdd_free(bddm,t);
	bdd_free(bddm,v);
	bdd_free(bddm,w);	
      }
      t = bdd_or(bddm,NOTC,NOTV);
      u = bdd_not(bddm, temp->ER);
      v = bdd_or(bddm,C,u);
      w = bdd_and(bddm,t,v);
      temp->VC = bdd_ECON(bddm,w,lits);
      bdd_free(bddm,t);
      bdd_free(bddm,u);
      bdd_free(bddm,v);
      bdd_free(bddm,w);
      if (temp->VC == bdd_zero(bddm)){
	ExpandCover(&C,&NOTC,lits,e,bddm,nsigs,step);
	/* printf("finding %d cube cover.\n",++temp->size); */
	temp->size++; 
	step = step->next;
      }
    }
    C = e->covers.head->C;
    NOTC = e->covers.head->NOTC;
  }
} 




/*****************************************************************************/
/* synthesize!!                                                              */
/*****************************************************************************/
void Synth(bdd N,bdd S,int map2main,int map2prime,int ninpsig, int nsigs,
	   bdd_manager bddm,bdd *lits,bdd *newlits,env *e,bdd *psi,
	   bool gatelevel,reg_node **ERplus,reg_node **ERminus)
{
  bdd *ESplus,*QSplus,*ESminus,*QSminus,var,varprime,s,t,u,v,w,C,NOTC;
  int noutpsig,i;
#ifdef SUBTIME
  timeval t0,t1,t2,t3;
  double time0,time1,time2,time3;
#endif

  /***************/
  /* get excited */
  /***************/
#ifdef SUBTIME
  gettimeofday(&t0,NULL);
#endif
  noutpsig = nsigs - ninpsig;
  ESplus = (bdd*)GetBlock((noutpsig)*sizeof(bdd));
  QSplus = (bdd*)GetBlock((noutpsig)*sizeof(bdd));
  ESminus = (bdd*)GetBlock((noutpsig)*sizeof(bdd));
  QSminus = (bdd*)GetBlock((noutpsig)*sizeof(bdd));
  for (i = 0;i<noutpsig;i++){
    var = bdd_not(bddm,lits[i+ninpsig]);
    varprime = newlits[i+ninpsig];
    t = bdd_and(bddm,var, varprime);
    s = bdd_and(bddm,N,t);
    ESplus[i] = bdd_EDIS(bddm, s, newlits);
    bdd_free(bddm,t);
    bdd_free(bddm,s);
    bdd_free(bddm,varprime);
    varprime  = bdd_not(bddm,newlits[i+ninpsig]);
    t = bdd_and(bddm,var, varprime);
    s = bdd_and(bddm,N,t);
    QSminus[i] = bdd_EDIS(bddm, s, newlits);
    bdd_free(bddm,t);
    bdd_free(bddm,s);
    bdd_free(bddm,var);
    var = lits[i+ninpsig];
    t = bdd_and(bddm,var, varprime);
    s = bdd_and(bddm,N,t);
    ESminus[i] = bdd_EDIS(bddm, s, newlits);
    bdd_free(bddm,t);
    bdd_free(bddm,s);
    bdd_free(bddm,varprime);
    varprime = newlits[i+ninpsig];
    t = bdd_and(bddm,var, varprime);
    s = bdd_and(bddm,N,t);
    QSplus[i] = bdd_EDIS(bddm, s, newlits);
    bdd_free(bddm,t);
    bdd_free(bddm,s);
    ERplus[i] = NULL;
    ERminus[i] = NULL;
    ERplus[i] = FindER(map2main,map2prime,bddm,ESplus[i],lits,newlits,N,e);
    ERminus[i] = FindER(map2main,map2prime,bddm,ESminus[i],lits,newlits,N,e);
  }

  /****************/
  /* get covered */
  /****************/
#ifdef SUBTIME
  gettimeofday(&t1,NULL);
#endif
  w = bdd_one(bddm);
  for (i = 0;i<nsigs;i++){
    t = bdd_IMPLY(bddm,psi[i],lits[i]);
    u = bdd_and(bddm,w,t);
    bdd_free(bddm,t);
    bdd_free(bddm,w);
    t = bdd_not(bddm,lits[i]);
    v = bdd_IMPLY(bddm,psi[i+nsigs],t);
    w = bdd_and(bddm,u,v);
    bdd_free(bddm,t);
    bdd_free(bddm,u);
    bdd_free(bddm,v);
  }
  C = w;
  NOTC = bdd_not(bddm,C);
  e->covers.num_covers = 1;
  e->covers.head = (cov_node *)GetBlock(sizeof(cov_node));
  e->covers.head->next = NULL;
  e->covers.head->C = C;
  e->covers.head->NOTC = NOTC;
  e->covers.head->psilits = psi;
  e->covers.head->psiids = (char **)GetBlock(2*nsigs*sizeof(char *));
  for (i=0;i<nsigs;i++) {
    e->covers.head->psiids[i]=CopyString(e->psiids[i]);
    e->covers.head->psiids[i+nsigs]=CopyString(e->psiids[i+nsigs]);
  }
  /* debug cover */

  /****************/
  /* get violated */
  /****************/
#ifdef SUBTIME
  gettimeofday(&t2,NULL);
#endif
  for (i = 0;i<noutpsig;i++){
    /****************/
    /* upstack      */
    /****************/
/*    printf("ES+ %s:\n",naming_fn(bddm,lits[i+ninpsig],e));*/
    FindCover(N,S,map2main,map2prime,bddm,gatelevel,C,
	      NOTC,lits,ESplus,QSplus,ERplus,i,e,nsigs);
    /****************/
    /* downstack    */
    /****************/
/*    printf("ES- %s:\n",naming_fn(bddm,lits[i+ninpsig],e));*/
    FindCover(N,S,map2main,map2prime,bddm,gatelevel,C,
	      NOTC,lits,ESminus,QSminus,ERminus,i,e,nsigs);
    
  }
#ifdef SUBTIME
  gettimeofday(&t3,NULL);
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  /*  time2 = (t2.tv_sec+(t2.tv_usec*.000001));*/
  time3 = (t3.tv_sec+(t3.tv_usec*.000001));
  fprintf(lg,"ER took %g seconds.\n",time1-time0);
  /*printf("C took %g seconds.\n",time2-time1);*/
  fprintf(lg,"VC took %g seconds.\n",time3-time1);
  fprintf(lg,"SYNTH took %g seconds.\n",time3-time0); 
#endif
  /* debug big */


  
}


/*****************************************************************************/
/* Print prs-type minterm expressions to <name>.min.bdd.                         */
/*****************************************************************************/
void print_min_cubes(reg_node *ER,env *e,bdd_manager bddm,FILE *fp,bool plus,
		 char * name,int nsigs,int max)
{
  int i,j,counts,countp,num = 0;
  struct BVMintermIterator *mi;
  bdd * vector = e->covers.head->psilits;
  if (ER->size !=1){
    vector = (bdd*)GetBlock((ER->size*2*nsigs+1)*sizeof(bdd));
    vector[ER->size*2*nsigs] = NULL;
    cov_node* temp = e->covers.head;
    j=0;
    while (j<ER->size) {
      for (i = 0;i<2*nsigs;i++)
	vector[i+(2*nsigs*j)]=temp->psilits[i];
      j++;
      temp=temp->next;
    }
  }
  mi = initBVMintermIterator(bddm,ER->VC,vector);
  while(nextBVMinterm(mi)){
    counts = 0;
    countp = 0;
    int flag = 0;
    int first_cube = 1;
    for (i=0;i<mi->n;i++){
      if (!(i % (2*nsigs)))
	if (first_cube)
	  first_cube=0;
	else 
	  {
	    if (++counts > max)
	      flag=1;
	    countp = 0;
	  }
      if (BVtestBit(mi->minterm,i)){
	if (++countp > max)
	  flag = 1;
      }
    }
    if (!flag){
      num++;
      int first_literal = 1;
      first_cube = 1;
      if (plus){
	fprintf(fp,"[+%s: (",name);
      }
      else{
	fprintf(fp,"[-%s: (",name);
      }
      for (i=0;i<mi->n;i++){
	if (!(i % (2*nsigs)))
	  if (first_cube)
	    first_cube=0;
	  else {
	    fprintf(fp,") | (");
	    first_literal = 1;
	  }
	if (BVtestBit(mi->minterm,i)){
	  if (first_literal)
	    first_literal = 0;
	  else{
	    fprintf(fp," & ");
	  }
	  if ((i%(2*nsigs)) >= nsigs)
	    fprintf(fp,"~%s",e->ids[i%(2*nsigs)-nsigs]);
	  else
	    fprintf(fp,"%s",e->ids[i%(2*nsigs)]);
	}
      }
      fprintf(fp,")]\n");
    }
  }
  fprintf(fp,"there is(are) %d cover(s) of size <= %d\n",num,max);
  freeBVMintermIterator(mi);  
}
/*****************************************************************************/
/* Print prs-type minterm expressions to <name>.bdd.                         */
/*****************************************************************************/
void print_cubes(reg_node *ER,env *e,bdd_manager bddm,FILE *fp,bool plus,
		 char * name,int nsigs,FILE *fp2,int maxsize)
{
  int i,j,min,num,count;
  struct BVMintermIterator *mi;
  bdd * vector = e->covers.head->psilits;
  if (ER->size !=1){
    vector = (bdd*)GetBlock((ER->size*2*nsigs+1)*sizeof(bdd));
    vector[ER->size*2*nsigs] = NULL;
    cov_node* temp = e->covers.head;
    j=0;
    while (j<ER->size) {
      for (i = 0;i<2*nsigs;i++)
	vector[i+(2*nsigs*j)]=temp->psilits[i];
      j++;
      temp=temp->next;
    }
  }
  mi = initBVMintermIterator(bddm,ER->VC,vector);
  min = INFIN;
  num = 0;
  while(nextBVMinterm(mi)){
    count = 0;
    int first_literal = 1;
    int first_cube = 1;
    if (plus){
      fprintf(fp,"[+%s: (",name);
    }
    else{
      fprintf(fp,"[-%s: (",name);
    }
    for (i=0;i<mi->n;i++){
      if (!(i % (2*nsigs)))
	if (first_cube)
	  first_cube=0;
	else {
	  fprintf(fp,") | (");
	  first_literal = 1;
	}
      if (BVtestBit(mi->minterm,i)){
	if (first_literal)
	  first_literal = 0;
	else{
	  fprintf(fp," & ");
	}
	count++;
	if ((i%(2*nsigs)) >= nsigs)
	  fprintf(fp,"~%s",e->ids[i%(2*nsigs)-nsigs]);
	else
	  fprintf(fp,"%s",e->ids[i%(2*nsigs)]);
      }
    }
    fprintf(fp,")]\n");
    if (count == min){
      num++;
    }
    else{
      if (count < min){
	min = count;
	num = 1;
      }
    }
  }
  freeBVMintermIterator(mi);  
  fprintf(fp,"there is(are) %d cover(s) of minimal size %d\n",num,min);
  print_min_cubes(ER,e,bddm,fp2,plus,name,nsigs,maxsize);
}
/*****************************************************************************/
/* Print prs-type minterm expressions to <name>.bdd.                         */
/*****************************************************************************/
void FileDump(int ninpsig,int nsigs,bdd_manager bddm,bdd* lits,env *e,
	      reg_node **ERplus,reg_node **ERminus,FILE *fp,FILE *fp2,
	      int maxsize)
{
  reg_node *temp;
  int j,noutpsig = nsigs - ninpsig;
  for (int i = 0;i<noutpsig;i++){
    temp = ERplus[i];
    j = 0;
    while (temp){
      fprintf(fp,"ER(%s+,%d):\n",naming_fn(bddm,lits[i+ninpsig],e),j);
      fprintf(fp2,"ER(%s+,%d):\n",naming_fn(bddm,lits[i+ninpsig],e),j++);
      print_cubes(temp,e,bddm,fp,TRUE,naming_fn(bddm,lits[i+ninpsig],e),
		  nsigs,fp2,maxsize);
      temp = temp->next;
    }
    
    temp = ERminus[i];
    
    j = 0;
    while (temp){
      fprintf(fp,"ER(%s-,%d):\n",naming_fn(bddm,lits[i+ninpsig],e),j);
      fprintf(fp2,"ER(%s-,%d):\n",naming_fn(bddm,lits[i+ninpsig],e),j++);
      print_cubes(temp,e,bddm,fp,FALSE,naming_fn(bddm,lits[i+ninpsig],e),
		  nsigs,fp2,maxsize);
      temp = temp->next;
    }
  } 
  
}

/*****************************************************************************/
/* BDDize the reduced state graph.                                           */
/*****************************************************************************/

void bdd_rsg(char * filename,signalADT *signals,bddADT bdd_state,
	     int nsigs,int ninpsig,bool verbose,bool gatelevel,int maxsize)
{
#ifdef TIMER
  timeval t0,t1,t2;
  double time0,time1,time2;
  double sol;
  double  count,space;
#endif
  char outname[FILENAMESIZE];
  char outname2[FILENAMESIZE+4];

  FILE *fpTk,*fp,*fp2;
  int i,j,map2main,map2prime,noutpsig;
  env e;
  bdd_manager bddm;
  bdd *lits,*psi,*newlits,*m2p,*p2m,N,S;
/*  bdd ghost,t,u,fred,barney,v;*/
  char *duper;
  reg_node **ERplus, **ERminus, *temp;

  strcpy(outname,filename);
  strcat(outname,".prs");
  strcpy(outname2,filename);
  strcat(outname2,".min");
  strcat(outname2,".prs");
  printf("Synthesizing using BDDs and storing to:  %s\n",outname);
  fprintf(lg,"Synthesizing using BDDs and storing to:  %s\n",outname);
  fp=Fopen(outname,"w"); 
  fp2=Fopen(outname2,"w"); 

  e.gamma = 0;
  e.phi = 0;
  noutpsig = nsigs - ninpsig;
  //bdd_free(bdd_state->bddm,bdd_state->Reg);
  bdd_nukit(bdd_state,TRUE);
  bdd_state->Reg=NULL;
  ERplus = (reg_node**)GetBlock((noutpsig)*sizeof(reg_node*));
  ERminus = (reg_node**)GetBlock((noutpsig)*sizeof(reg_node*));
  bddm = bdd_state->bddm;//bdd_init();
  lits = bdd_state->lits;//(bdd*)GetBlock((nsigs+2)*sizeof(bdd));
  newlits = bdd_state->newlits;//(bdd*)GetBlock((nsigs+2)*sizeof(bdd));
  m2p=(bdd*)GetBlock(((2*nsigs)+1)*sizeof(bdd));
  p2m=(bdd*)GetBlock(((2*nsigs)+1)*sizeof(bdd));
  psi=(bdd*)GetBlock((2*nsigs+1)*sizeof(bdd));
  psi[(2*nsigs)] =NULL;
  //lits[nsigs+1] = newlits[nsigs+1] = 
  m2p[2*nsigs] = p2m[2*nsigs] = NULL;
  for (i=0;i<nsigs;i++){
    //lits[i] = bdd_new_var_last(bddm);
    psi[i] = bdd_new_var_last(bddm);
    psi[nsigs+i] = bdd_new_var_last(bddm);
    //newlits[i] = bdd_new_var_last(bddm);
  }
  /*ghost = bdd_new_var_last(bddm);*/
  //lits[nsigs] = newlits[nsigs] = NULL; /* ghost; */
  e.lits = (bdd*)GetBlock(((2*nsigs)+1)*sizeof(bdd));
  e.ids = (char**)GetBlock(((2*nsigs)+1)*sizeof(char*));
  e.psiids = (char**)GetBlock(((2*nsigs)+1)*sizeof(char*));
  /*  e.lits[(2*nsigs)] = ghost;*/
  e.covers.head = NULL;
  e.nlits = (2*nsigs); /* +1;*/
  for (i=0;i<(2*nsigs);i++)
    if (i/nsigs){
      e.lits[i] = newlits[i-nsigs];
      /*  debug 1 */
    }
    else{
      e.lits[i] = lits[i];
      /*  debug 2 */
    }
  for (i=0;i<(2*nsigs);i++)
    if (i%2){
      m2p[i] = newlits[i/2];
      p2m[i] = lits[i/2];
    }
    else{
      m2p[i] = lits[i/2];
      p2m[i] = newlits[i/2];
    }
  //  for (i = 0;i<2*nsigs;i++){
  //  printf ("i = %d\n",i);
  //  bdd_print_bdd(bdd_state->bddm,p2m[i],bdd_naming_fn_none,
  //		  bdd_terminal_id_fn_none,NULL,stdout);
  //  bdd_print_bdd(bdd_state->bddm,m2p[i],bdd_naming_fn_none,
  //		  bdd_terminal_id_fn_none,NULL,stdout);
  //}
  //printf ("i = %d,%x,%x\n",i,p2m[i],m2p[i]);
  
  map2main = bdd_new_assoc(bddm, p2m, 1);
  map2prime = bdd_new_assoc(bddm, m2p, 1);
  //printf("gothere\n");
  for (i=0;i<nsigs;i++){
    e.ids[i] = signals[i]->name;
    duper = (char*)GetBlock((strlen(signals[i]->name)+2)*sizeof(char));
    strcpy(duper,signals[i]->name);
    duper[strlen(signals[i]->name)] = '\'';
    duper[strlen(signals[i]->name)+1] = 0;
    e.ids[nsigs+i] = duper;    
    duper = (char*)GetBlock((strlen(e.ids[i])+3)*sizeof(char));
    strcpy(duper,"p_");
    strcat(duper,e.ids[i]);
    e.psiids[i] = duper;    
    duper = (char*)GetBlock((strlen(e.ids[i])+6)*sizeof(char));
    strcpy(duper,"pnot_");
    strcat(duper,e.ids[i]);
    e.psiids[nsigs+i] = duper;    
      /*  debug 3 */
  }
  /*  e.ids[2*nsigs] = "ghost";*/

#ifdef TIMER
  gettimeofday(&t0,NULL);
#endif
  //StateSpace(&N,&S,state_table,bddm,lits,newlits,&e);
  N = bdd_state->N;
  S = bdd_state->S;
  //S = bdd_TRANS(bddm,bdd_state->S,map2main);

  if (verbose) {
    strcpy(outname,filename);
    strcat(outname,".S");
    fpTk=Fopen(outname,"w"); 
    bdd_print_tk(bddm,S,(char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		 bdd_terminal_id_fn,&e,fpTk);
    fclose(fpTk);
    strcpy(outname,filename);
    strcat(outname,".N");
    fpTk=Fopen(outname,"w"); 
    bdd_print_tk(bddm,N,(char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		 bdd_terminal_id_fn,&e,fpTk);
    fclose(fpTk);
   }

#ifdef TIMER
  gettimeofday(&t1,NULL);
#endif
  Synth(N,S,map2main,map2prime,ninpsig,nsigs,
	bddm,lits,newlits,&e,psi,gatelevel,ERplus,ERminus);
#ifdef TIMER
 gettimeofday(&t2,NULL);
#endif
  if (verbose) {
    for (i=0;i<nsigs-ninpsig;i++) {
      j=0;
      for (temp=ERplus[i];temp;temp=temp->next) {
	strcpy(outname,filename);
	sprintf(outname,"%s.%sR%d",outname,signals[i+ninpsig]->name,j);
	fpTk=Fopen(outname,"w"); 
	bdd_print_tk(bddm,temp->VC,
		     (char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		     bdd_terminal_id_fn,&e,fpTk);
	fclose(fpTk);
	j++;
      }
      j=0;
      for (temp=ERminus[i];temp;temp=temp->next) {
	strcpy(outname,filename);
	sprintf(outname,"%s.%sF%d",outname,signals[i+ninpsig]->name,j);
	fpTk=Fopen(outname,"w"); 
	bdd_print_tk(bddm,temp->VC,
		     (char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		     bdd_terminal_id_fn,&e,fpTk);
	fclose(fpTk);
	j++;
      }
    }
  }

#ifdef TIMER
  time0 = (t0.tv_sec+(t0.tv_usec*.000001));
  time1 = (t1.tv_sec+(t1.tv_usec*.000001));
  time2 = (t2.tv_sec+(t2.tv_usec*.000001));
  space = pow(4,nsigs);
  fprintf(lg,"StateSpace construction took %g seconds.\n",time1-time0);
  fprintf(lg,"BDD synthesis took %g seconds.\n",time2-time1);
  fprintf(lg,"|P|=%d,|G|=%d,|S|=%ld,|N|=%ld\n",
	  e.phi,e.gamma,bdd_size(bddm,S,1),bdd_size(bddm,N,1));
  /*fprintf(lg,"space=%g,nsigs=%d,ninpsig=%d,noutpsig=%d\n",
	 space,nsigs,ninpsig,nsigs-ninpsig);*/
  sol = 1;
  for (i = 0;i<noutpsig;i++){
    fprintf(lg,"ES+ %s:\n",naming_fn(bddm,lits[i+ninpsig],&e));
    temp = ERplus[i];
    j = 0;
    while (temp){
      space = pow(4,nsigs*temp->size);
      count = bdd_satisfying_fraction(bddm,temp->VC);
      sol *= space*count/temp->size;
            fprintf(lg,"\tER%d: %g solutions,%d cube(s)\n",
	     j++,count*space,temp->size);
      temp = temp->next;
    }
        fprintf(lg,"ES- %s:\n",naming_fn(bddm,lits[i+ninpsig],&e));
    temp = ERminus[i];
    j = 0;
    while (temp){
      space = pow(4,nsigs*temp->size);
      count = bdd_satisfying_fraction(bddm,temp->VC);
      sol *= space*count/temp->size;
      fprintf(lg,"\tER%d: %g solutions, %d cube(s)\n",
	     j++,count*space,temp->size);
      temp = temp->next;
    }
  }
  fprintf(lg,"there are %g solutions\n",sol);
  /*  fred = ERplus[0]->VC;
  while(fred != bdd_zero(bddm)){
    barney = bdd_satisfy(bddm,fred);
    printf("solution:\n");
    bdd_print_bdd(bddm,barney,(char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		  bdd_terminal_id_fn_none,&e,stdout);
    t = bdd_not(bddm,barney);
    u = bdd_and(bddm,t,fred);
    bdd_free(bddm,t);
    bdd_free(bddm,fred);
    bdd_free(bddm,barney);
    fred = u;
  }*/
#endif
    fpTk=Fopen("BDD","w");
    fprintf(fpTk,"S:\n");
    bdd_print_bdd(bddm,S,(char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		 bdd_terminal_id_fn_none,&e,fpTk);
    fprintf(fpTk,"N:\n");
    bdd_print_bdd(bddm,N,(char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		 bdd_terminal_id_fn_none,&e,fpTk);
    fclose(fpTk);

  if (verbose)
    FileDump(ninpsig,nsigs,bddm,lits,&e,ERplus,ERminus,fp,fp2,maxsize);
  fclose(fp);
  fclose(fp2);

  /************/
  /* clean up */
  /************/
//   free(m2p);
//   free(p2m);
//   free(lits);
//   free(e.lits);
//   free(e.ids);
//   free(newlits);
//   bdd_quit(bddm);
}

/*****************************************************************************/
/* Find reduced state graph.                                                 */
/*****************************************************************************/

void bddize_reduced_state_graph(char command,designADT design)
{
  if (!(design->status & STOREDPRS)) {
    bdd_rsg(design->filename,design->signals,design->bdd_state,
	    design->nsignals,design->ninpsig,design->verbose,
	    design->gatelevel,design->maxsize);
    design->status = design->status | FOUNDREG | FOUNDCONF | FOUNDCOVER |
      STOREDPRS;
  }
}



/*****************************************************************************/
/* debug library                                                             */
/*****************************************************************************/

      /*  debug 1 */
      /*printf("%d(%d):%x\n",i,i/nsigs, newlits[i-nsigs]);*/

      /*  debug 2 */
      /*printf("%d(%d):%x\n",i, i/nsigs,lits[i]);*/
 
      /*  debug 3 */
    /*printf("%s->%s\n",e.ids[i],e.ids[nsigs+i]);*/

      /*  debug 4 */
      /*printf("preds:\n");
      if (curstate->pred)
	stepper = curstate->pred;
      while (stepper){
	printf("%s->%s\n",stepper->stateptr->state,curstate->state);
	stepper = stepper->next;
      }
      printf("succs:\n");
      if (curstate->succ)
	stepper = curstate->succ;
      while (stepper){
	printf("%s->%s\n",curstate->state,stepper->stateptr->state);
	stepper = stepper->next;
      }*/

      /*  debug 5 */
      /*printf("%s:\n",curstate->state);
      temp_node = esigs;
      while(temp_node){
	printf(" %d",temp_node->sig);
	temp_node = temp_node->next;
      }
      printf("\n");*/

      /*  debug 6 */
      /*printf("state %s has %d successors; it should have %d;\n",
	     curstate->state,count2,count1);*/

      /*  debug 7 */
  /* printf("bdd contains %d nodes\n",bdd_size(bddm,u,0)); */

  /* debug 8*/
  /*printf("%x\n", f);*/ 

  /* debug cover */
  /*
  fp=Fopen("bdd_list","w");
  fprintf(fp,"the bdd contains %ld nodes\n",bdd_size(bddm,C,0));
  bdd_print_bdd(bddm,C,(char *(*)(bdd_manager, bdd, pointer))&naming_fn,
		bdd_terminal_id_fn_none,&e,fp);
  fclose(fp); 
  */

  /* debug big */
  /*

  fp=Fopen("bdd_list","w");
  for (i = 0;i<noutpsig;i++){
    fprintf(fp,"ES+ %s:\n",naming_fn(bddm,lits[i+ninpsig],e));
    bdd_print_bdd(bddm,ESplus[i],
		  (char *(*)(bdd_manager, bdd, pointer))&naming_fn,
    bdd_terminal_id_fn_none,e,fp);
    
    temp = ERplus[i];
    
    j = 0;
    while (temp){
      fprintf(fp,"ER%d:\n",j);
      bdd_print_bdd(bddm,temp->ER,(char *(*)(bdd_manager, bdd, 
					     pointer))&naming_fn,
		    bdd_terminal_id_fn_none,e,fp);
      fprintf(fp,"VC%d:\n",j++);
      bdd_print_bdd(bddm,temp->VC,(char *(*)(bdd_manager, bdd, 
					     pointer))&naming_fn,
		    bdd_terminal_id_fn_none,e,fp);
      temp = temp->next;
    }
    fprintf(fp,"ES- %s:\n",naming_fn(bddm,lits[i+ninpsig],e));
    bdd_print_bdd(bddm,ESminus[i],
		  (char *(*)(bdd_manager, bdd, pointer))&naming_fn,
    bdd_terminal_id_fn_none,e,fp);
    
    temp = ERminus[i];
    
    j = 0;
    while (temp){
      fprintf(fp,"ER%d:\n",j);
      bdd_print_bdd(bddm,temp->ER,(char *(*)(bdd_manager, bdd, 
					     pointer))&naming_fn,
		    bdd_terminal_id_fn_none,e,fp);
      fprintf(fp,"VC%d:\n",j++);
      bdd_print_bdd(bddm,temp->VC,(char *(*)(bdd_manager, bdd, 
					     pointer))&naming_fn,
		    bdd_terminal_id_fn_none,e,fp);
      temp = temp->next;
    }
  } 
  fclose(fp); 
   
  */  
#endif
