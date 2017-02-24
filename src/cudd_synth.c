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
/* cudd_synth.c                                                                 */
/*****************************************************************************/
#ifdef CUDD
#include "cudd_synth.h"
#include "memaloc.h"
#include <cudd/cuddInt.h>
#include <algorithm>


/**Function********************************************************************
FREELY PLAGIARIZED from ddPrintMintermAux
  Synopsis    [Performs the recursive step of mask_PrintMinterm.]

  Description []

  SideEffects [None]

******************************************************************************/
static void
maskAux(
  DdManager * dd /* manager */,
  DdNode * node /* current node */,
  int * list /* current recursion path */,
  int *mask,
  FILE *fp)
{
    DdNode *background, *zero;
    DdNode	*N,*Nv,*Nnv;
    int		i,v,index,first=1;

    N = Cudd_Regular(node);
    background = dd->background;
    zero = Cudd_Not(dd->one);

    if (cuddIsConstant(N)) {
	/* Terminal case: Print one cube based on the current recursion
	** path, unless we have reached the background value (ADDs) or
	** the logical zero (BDDs).
	*/
	if (node != background && node != zero) {
	    for (i = 0; i < dd->size; i++) {
	      if (mask[i] == 2){
		if (first)
		  first = 0;
		else
		  fprintf(fp," | ");
	      }
	      v = list[i];
	      /* print entry only if mask bit is set */
	      if ((v == 0)&& mask[i]) (void) fprintf(fp,"0");
	      else if ((v == 1)&& mask[i]) (void) fprintf(fp,"1");
	      else if (mask[i]) (void) fprintf(fp,"-");
	    }
	    (void) fprintf(fp," % g\n", cuddV(node));
	}
    } else {
	Nv  = cuddT(N);
	Nnv = cuddE(N);
	if (Cudd_IsComplement(node)) {
	    Nv  = Cudd_Not(Nv);
	    Nnv = Cudd_Not(Nnv);
	}
	index = N->index;
	list[index] = 0;
	maskAux(dd,Nnv,list,mask,fp); 
	list[index] = 1;
	maskAux(dd,Nv,list,mask,fp);
	list[index] = 2;
    }
    return;

} /* end of maskAux */


/**Function********************************************************************
  FREELY PLAGIARIZED from Cudd_PrintMinterm
  Synopsis    [Prints a disjoint sum of products.]

  Description [Prints a disjoint sum of product cover for the function
  rooted at node. Each product corresponds to a path from node a leaf
  node different from the logical zero, and different from the
  background value. Uses the standard output.  
  mask is a bit vector with bits set for those variables which should 
  be printed.  fp is an optional file to dump the output into.

  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_PrintDebug]

******************************************************************************/
int
mask_PrintMinterm(BDD f, int *mask, FILE *fp)
{
  DdManager * manager = (f.manager())->getManager();
  DdNode * node = (DdNode*)f.getNode();
  int i, *list;
  
  list = ALLOC(int,manager->size);
  if (list == NULL) {
    manager->errorCode = CUDD_MEMORY_OUT;
    return(0);
  }
  for (i = 0; i < manager->size; i++) list[i] = 2;
  maskAux(manager,node,list,mask,fp);
  FREE(list);
  return(1);
  
} /* end of mask_PrintMinterm */

/*****************************************************************************/
/* extract cover print masks;                                                */
/*****************************************************************************/

int* find_mask(mask_list *masks, int size){
  mask_node *temp = masks->head;
  while (temp){
    if (temp->cubes == size)
      return temp->mask;
    temp = temp->next;
  }
  return NULL;
}


/*****************************************************************************/
/* initialize print masks;                                                   */
/*****************************************************************************/
void init_mask(bddADT bdd_state)
{
  int i,j,k,nlits;
  int lastvar = bdd_state->lastvar;
  bdd_set sigs = bdd_state->sigs;
  bdd_set rules = bdd_state->rules;
  bdd_set rows = bdd_state->rows;
  bdd_set cols = bdd_state->cols;
  bdd_set unused = bdd_state->unused;
  bdd_set covs = bdd_state->covs;

  int *mask = new int[lastvar], 
    *maskp = new int[lastvar],
    *maskn = new int[lastvar],
    *maskc = new int[lastvar],
    *maskno = new int[lastvar];
  /* make print masks */
  if (bdd_state->sigmask){
    delete[]  bdd_state->sigmask;
    delete[] bdd_state->primemask;
    delete[] bdd_state->nmask;
    delete[] bdd_state->nomask;
    delete[] bdd_state->covmask;
  }
  /* signal section */
  for (i = sigs.start; i< sigs.stop;i+= sigs.step){
    /* print sigs */
    mask[i] = 1;
    mask[i+1] = 0;
    /* print primes */
    maskp[i] = 0;
    maskp[i+1] = 1;
    /* print both */
    maskn[i] = 1;
    maskn[i+1] = 1;
    /* print everything */
    maskno[i] = 1;
    maskno[i+1] = 1;
    /* don't print covers */
    maskc[i] = 0;
    maskc[i+1] = 0;
  }
  for (i = rules.start;i< rules.stop; i+=rules.step){
    /* don't print signals */
    mask[i] = 0;
    maskp[i] = 0;
    maskn[i] = 0;
    /* print everything */
    maskno[i] = 1;
    /* don't print covers */
    maskc[i] = 0;
  }
  for (i= rows.start; i< rows.stop;i+= rows.step){
    /* don't print signals */
    mask[i] = 0;
    maskp[i] = 0;
    maskn[i] = 0;
    /* print everything */
    maskno[i] = 1;
    /* print covers */
    maskc[i] = 0;
  }
  for (i= cols.start; i< cols.stop;i+= cols.step){
    /* don't print signals */
    mask[i] = 0;
    maskp[i] = 0;
    maskn[i] = 0;
    /* print everything */
    maskno[i] = 1;
    /* print covers */
    maskc[i] = 0;
  }
  for (i= unused.start; i< unused.stop;i+= unused.step){
    /* don't print signals */
    mask[i] = 0;
    maskp[i] = 0;
    maskn[i] = 0;
    /* print everything */
    maskno[i] = 1;
    /* print covers */
    maskc[i] = 0;
  }
  for (j = 0; j < bdd_state->max_cubes; j++)
    for (i= covs.start + j*bdd_state->cubestep; 
	 i< covs.stop + j*bdd_state->cubestep;
	 i+= covs.step){
      /* don't print signals */
      mask[i] = mask[i+1] = 0;
      maskp[i] = maskp[i+1] = 0;
      maskn[i] = maskn[i+1] = 0;
      /* print everything */
      maskno[i] = maskno[i+1] = 1;
      /* print covers */
      maskc[i] = maskc[i+1] = 1;
    }    
  /* copy masks to bdd_state */
  bdd_state->sigmask = mask;
  bdd_state->primemask = maskp;
  bdd_state->nmask = maskn;
  bdd_state->nomask = maskno;
  bdd_state->covmask = maskc;
  
  nlits = 2*bdd_state->nsigs;
  delete bdd_state->covmasks;
  bdd_state->covmasks = new mask_list();
  for (i = 0; i< bdd_state->max_cubes;){
    maskc = new int[lastvar];
    for (j=0;j<lastvar;j++)
      maskc[j] = 0;    
    i++;
    for (j= 0;j<i;j++){
      maskc[covs.start + j*bdd_state->cubestep] = 2;
      maskc[covs.start + 1 + j*bdd_state->cubestep] = 1;
      for (k = covs.start + j*bdd_state->cubestep + covs.step; 
	   k < covs.stop + j*bdd_state->cubestep;
	   k +=covs.step){
	maskc[k] = maskc[k+1] = 1;
      }
    }
    bdd_state->covmasks->insert(maskc, i);
  }
}

/*****************************************************************************/
/* initialize cover;                                                         */
/*****************************************************************************/
void make_cover(Cudd *mgr,bddADT bdd_state)
{
  int i,j;
  BDD chi1,chi2,cover,old_cover;
  bdd_set covs = bdd_state->covs;
  bdd_set sigs = bdd_state->sigs;
  bdd_set unused = bdd_state->unused;
  int n_cubes = bdd_state->covers.count;
  int step = bdd_state->cubestep;

  /* loop setup */
  cover = mgr->bddOne();
  for (i = covs.start,j = sigs.start; i < covs.stop; 
       i += covs.step,j += sigs.step){
    chi1 = mgr->bddVar(i+n_cubes*step);
    chi2 = mgr->bddVar(i+n_cubes*step+1);
    /* ((chi1=> sig[j])*(chi2=> !sig[j])) */
    cover *= (!chi1 + mgr->bddVar(j)) * (!chi2 + !mgr->bddVar(j));
  }
  cover *= (!(mgr->bddVar(unused.start+(n_cubes)*unused.step))
               + (mgr->bddVar(sigs.start) * !mgr->bddVar(sigs.start)));
  //printf("oring\n");
  if (bdd_state->covers.count)
    cover += bdd_state->covers.tail->data;
  //printf("done oring\n");
  /* add new cover to list*/
  bdd_state->covers.insert(cover);
}


/*****************************************************************************/
/* initialize cover cube;                                                         */
/*****************************************************************************/
void make_cube(Cudd *mgr,bddADT bdd_state, int sig)
{
  int i,j;
  BDD chi1,chi2,chi3,chi4,cover,old_cover;
  bdd_set covs = bdd_state->covs;
  bdd_set sigs = bdd_state->sigs;
  bdd_set unused = bdd_state->unused;
  int n_cubes = bdd_state->cubes.count;
  int step = bdd_state->cubestep;

  int down = (n_cubes/2)%2;

  /* loop setup */
  cover = mgr->bddOne();
  for (i = covs.start,j = sigs.start; i < covs.stop; 
       i += covs.step,j += sigs.step){
    chi1 = mgr->bddVar(i+n_cubes*step);
    chi2 = mgr->bddVar(i+n_cubes*step+1);
    chi3 = mgr->bddVar(i);
    chi4 = mgr->bddVar(i+1);
    /* ((chi1=> sig[j])*(chi2=> !sig[j])) */
    if (1){
      cover *= (!chi1 + mgr->bddVar(j));
      cover *= (!chi2 + !mgr->bddVar(j));
    }else{
    /* don't use useless literals. */
    if ((!down && ((chi3 * bdd_state->CCplus[sig]) != mgr->bddZero())) ||
	(down && ((chi3 * bdd_state->CCminus[sig]) != mgr->bddZero())))
      cover *= (!chi1 + mgr->bddVar(j));
    else
      cover *= !chi1;
    if ((!down && ((chi4 * bdd_state->CCplus[sig]) != mgr->bddZero())) ||
	(down && ((chi4 * bdd_state->CCminus[sig]) != mgr->bddZero())))
      cover *= (!chi2 + !mgr->bddVar(j));
    else
      cover *= !chi2;
    }
  }
  cover *= (!(mgr->bddVar(unused.start+(n_cubes)*unused.step))
               + (mgr->bddVar(sigs.start) * !mgr->bddVar(sigs.start)));
  bdd_state->cubes.insert(cover);
}


/*****************************************************************************/
/* finder: finds excitation regions, and store them into bdd_state           */
/*                                                                           */
/*****************************************************************************/

void find_ER(Cudd *mgr, int nsigs, int ninpsig, BDD N, BDD S, 
	    bddADT bdd_state, signalADT *signals, bool verbose,int *mask)
{ 
  int j,i,l,k;
  BDD es,esp,er,erp,ert,tmp,tmp2;
  bdd_set sigs = bdd_state->sigs;

  for (l=sigs.start+sigs.step*ninpsig,k=0;l<sigs.stop;k++,l+=sigs.step)
    for (j=0;j<2;j++)
      {
	/* es = excitation space */
	if (j)
	  es = N * !(mgr->bddVar(l)) * mgr->bddVar(l+1);
	else
	  es = N * mgr->bddVar(l) * !(mgr->bddVar(l+1));
	es = es.ExistAbstract(bdd_state->pvars);
	if (j){
	  bdd_state->ESplus[k] = es;
	}
	else{
	  bdd_state->ESminus[k] = es;
	}
	/* esp = primed excitation space */
	esp = es.SwapVariables(*(bdd_state->v),*(bdd_state->pv));
	/* find ERs */
	i = 0;
	while (es != mgr->bddZero()){
	  /* choose seed */
	  er = es.PickOneMinterm(*(bdd_state->v));
	  /* temp = FALSE */
	  ert = mgr->bddZero();
	  while (ert != er){
	    er += ert;
	    /* prime ER */
	    erp = er.SwapVariables(*(bdd_state->v),*(bdd_state->pv));
	    /* from */
	    tmp = (es *erp*N).ExistAbstract(bdd_state->pvars);
	    /* to */
	    tmp2 = (er*esp*N).ExistAbstract(bdd_state->vars);
	    tmp2 = tmp2.SwapVariables(*(bdd_state->pv),*(bdd_state->v));
	    ert = er+tmp+tmp2;
	  }
	  if (j){
	    (bdd_state->ERplus)[k].insert(er);
	  }
	  else{
	    (bdd_state->ERminus)[k].insert(er);
	  }
	  es -= er;
	  i++;
	}
      }
  /* find quiescent sets. QS(i+) = i *S * !ES(i+)*/
  for (i=sigs.start+sigs.step*ninpsig,k=0;i<sigs.stop;k++,i+=sigs.step){
    bdd_state->QSminus[k] = 
      S * !mgr->bddVar(i) * !bdd_state->ESplus[k];
    bdd_state->QSplus[k] = 
      S * mgr->bddVar(i) * !bdd_state->ESminus[k];
  }
}

/*****************************************************************************/
/* findvc: finds valid covers for all excitation regions.                    */
/*                                                                           */
/*****************************************************************************/

void find_VC(Cudd *mgr, int nsigs, int ninpsig, int nrules, BDD N, BDD S, 
	     bddADT bdd_state, signalADT *signals, bool verbose,
	     int *mask, bool gatelevel)
{ 
  int j,k,cubes;
  bdd_node *nd,*cov;
  BDD VC,V,V1,V2,ES,ER,ERP,QS,C,CP;
  
  for (k=0;k<nsigs-ninpsig;k++)
    for (j=0;j<2;j++){
      /* copy data for this signal */
      if (j){
	ES = bdd_state->ESplus[k];
	QS = bdd_state->QSplus[k];
	nd = bdd_state->ERplus[k].head;
      }
      else{
	ES = bdd_state->ESminus[k];
	QS = bdd_state->QSminus[k];
	nd = bdd_state->ERminus[k].head;
      }
      /* loop through ERs */
      for (;nd;nd = nd->next){
	ER = nd->data;
	/* select single cube cover. */
	cov = bdd_state->covers.head;
	C = cov->data;
	cubes = 1;
	do {
	  /* generate SC violations*/
	  if (gatelevel){
	    V1 = S * !ER * !QS;
	    CP = C.SwapVariables(*(bdd_state->v),*(bdd_state->pv));
	    ERP = ER.SwapVariables(*(bdd_state->v),*(bdd_state->pv));
	    V2 = (N * !C * CP * !ERP).ExistAbstract(bdd_state->vars);
	    V2 = V2.SwapVariables(*(bdd_state->pv),*(bdd_state->v));
	    V = V1 + V2;
	  }
	  /* generate gC violations*/
	  else{
	    V = S * !ES * !QS;
	  }
	  /* filter covers */
	  VC = ((!C + !V) * (!ER + C));
	  VC = VC.UnivAbstract(bdd_state->vars);
	  /* if no covers of this size, increase cube count and try again. */
	  if (VC == mgr->bddZero()){
	    cubes++;
	    if (!(cov->next)){
	      make_cover(mgr,bdd_state);
	    }
	    cov = cov->next;
	    C = cov->data;
	  }
	}
	while (VC == mgr->bddZero());
	/* store covers */
	if (j)
	  (bdd_state->VCplus)[k].insert(VC,cubes);
	else
	  (bdd_state->VCminus)[k].insert(VC,cubes);
      }
    }
}

/*****************************************************************************/
/* findcc: finds basic covers                                                */
/*                                                                           */
/*****************************************************************************/

void find_BC(Cudd *mgr, int nsigs, int ninpsig, int nrules, BDD N, BDD S, 
	     bddADT bdd_state, signalADT *signals, bool verbose,
	     int *mask, bool gatelevel, int sig)
{ 
  const int overshoot=0;
  int j,extra;
  bdd_node *cov;
  BDD C;
  BDD temp,CSp,CSm,temp2;
 
  /* initialize*/
  bdd_state->CCplus[sig] = mgr->bddZero();
  bdd_state->CCminus[sig] = mgr->bddZero();
  /* generate combinational spaces */
  CSp = bdd_state->QSplus[sig]+ bdd_state->ESplus[sig];
  CSm = bdd_state->QSminus[sig]+ bdd_state->ESminus[sig];
  /* select single cube cover */
  cov = bdd_state->covers.head;
  extra = -1;
  for (j = 0;;j++){
      //printf("up %d\n",sig);
    C = cov->data;
    /* ((CSm=> !C)*(ESplus =>C)) */
    temp =  (!C + !S + CSp)*(C + !(bdd_state->ESplus[sig]));
    bdd_state->CCplus[sig] = temp.UnivAbstract(bdd_state->vars);
    bdd_state->CCpsize[sig] = j+1;
    /* if no cover, increase cube count and try again. */
    if (bdd_state->CCplus[sig] != mgr->bddZero()) 
      extra++;
    if (extra == overshoot ) {
	break;
    }
    if (!(cov->next)){
      make_cover(mgr,bdd_state);
    }
    cov = cov->next;
  }
  
  /* select single cube cover */
  cov = bdd_state->covers.head;
  extra = -1;
  for (j = 0;;j++){
    //printf("down %d\n",sig);
    C = cov->data;
    /* ((CSp=> !C)*(CSm =>C)) */
    temp =  (!C + !S + CSm)*(C + !(bdd_state->ESminus[sig]));
    bdd_state->CCminus[sig] = temp.UnivAbstract(bdd_state->vars);
    bdd_state->CCmsize[sig] = j+1;
    /* if no cover, increase cube count and try again. */
    if (bdd_state->CCminus[sig] != mgr->bddZero())
      extra++;
    if (extra == overshoot){
      break;
    }
    if (!(cov->next)){
      make_cover(mgr,bdd_state);
    }
    cov = cov->next;
  }
}


/*****************************************************************************/
/* mapit: finds and removes first minimum literal cover                      */
/*                                                                           */
/*****************************************************************************/

void PICKit(Cudd *mgr, bddADT bdd_state, BDD *src, 
	    BDD *sink, int *lits, int cubes, int nlits, bool twolevel){

  BDD temp;
  int i, j, size;
  char *term;
  int *first,*last, *array;
  int *weights;

  weights = new int[bdd_state->lastvar];
  for (i=0; i < bdd_state->lastvar ; i++) {
    weights[i]=0;
  }
  for (j = 0; j < bdd_state->max_cubes; j++)
    for (i=bdd_state->covs.start; 
	 i < bdd_state->covs.stop; 
	 i += bdd_state->covs.step) {
      weights[i+j*bdd_state->cubestep]=1;
      weights[i+1+j*bdd_state->cubestep]=1;
  }
  temp = (*src).ShortestPath(weights,NULL,lits);
  //    printf("PICKit lits = %d\n",(*lits));
  if (temp != mgr->bddZero()){
    size = bdd_state->lastvar;
    //term = (char*)malloc(sizeof(char)*size);
    term = new char[size];
    temp.PickOneCube(term);
    temp = mgr->bddOne();
    for (i = bdd_state->unused.start; 
	 i < bdd_state->unused.start + cubes*bdd_state->unused.step;
	 i += bdd_state->unused.step)
      if  (term[i] == 1)
	temp *= mgr->bddVar(i);
      else
	temp *= !(mgr->bddVar(i));
    for (j=0;j<cubes;j++)
      for (i=bdd_state->covs.start; 
	 i < bdd_state->covs.stop; 
	 i += bdd_state->covs.step) {
	if  (term[i+j*bdd_state->cubestep] == 1)
	  temp *= mgr->bddVar(i+j*bdd_state->cubestep);
	else
	  temp *= !(mgr->bddVar(i+j*bdd_state->cubestep));
	if  (term[i+1+j*bdd_state->cubestep] == 1)
	  temp *= mgr->bddVar(i+1+j*bdd_state->cubestep);
	else
	  temp *= !(mgr->bddVar(i+1+j*bdd_state->cubestep));
      }
    *sink = temp;
    *src -= temp;
    /* remove mirrors here. */
    if (!twolevel){
      if (cubes > 1){
	array = new int[cubes];
	for (i=0;i<cubes;i++)
	  array[i] = i;
	first = array;
	last = first+cubes;
	do{
	  temp = mgr->bddOne();
	  for (i=0;i<cubes;i++){
	    if (term[i*bdd_state->unused.step+bdd_state->unused.start])
	      temp *= mgr->bddVar(array[i]*bdd_state->unused.step
				  +bdd_state->unused.start);
	    else{
	      temp *= !(mgr->bddVar(array[i]*bdd_state->unused.step
				    +bdd_state->unused.start));
	      for (j=bdd_state->covs.start; 
		   j < bdd_state->covs.stop; 
		   j += bdd_state->covs.step) {
		if (term[j+i*bdd_state->cubestep] == 1)
		  temp *= mgr->bddVar(j+array[i]*bdd_state->cubestep);
		else 
		  temp *= !mgr->bddVar(j+array[i]*bdd_state->cubestep);
		if (term[j+1+i*bdd_state->cubestep] == 1)
		  temp *= mgr->bddVar(j+1+array[i]*bdd_state->cubestep);
		else 
		  temp *= !mgr->bddVar(j+1+array[i]*bdd_state->cubestep);
	      }
	    }
	  }
	  *src -= temp;
	}while(next_permutation(first,last));
	delete[] array;
      }
    }
    delete[] term;
    delete[] weights;
  }
}
/*****************************************************************************/
/* comboit:  tests a solution for combinationality                           */
/*                                                                           */
/*****************************************************************************/

void COMBOit(Cudd *mgr, bddADT bdd_state, BDD *cover, BDD space, 
	     int size, int var){
  BDD temp;
  int i;
  bdd_node *cov;

  temp = *cover;
  cov = bdd_state->covers.head;
  for (i = 1; i<size;i++){
    cov = cov->next;
  }
  temp *= cov->data;
  temp = temp.ExistAbstract(bdd_state->cvars);
  temp *= bdd_state->S;
  temp *= space;
  if (temp != space)
    *cover = mgr->bddZero();
}
/*****************************************************************************/
/* newcov: finds covers of next valid cube size.                             */
/*                                                                           */
/*****************************************************************************/

void NEWcov(Cudd *mgr, bddADT bdd_state, BDD *old, BDD dontimply, 
	    BDD doimply, int *oldsize){
  BDD C;
  bdd_node *nd;
  int i;
 

  nd = bdd_state->covers.head;
  for (i = 1;i<*oldsize;i++)
    nd = nd->next;
  if (!(nd->next)){
    make_cover(mgr,bdd_state);
  }
  nd = nd->next;
  C = nd->data;
  C = (!C + dontimply) * (C + doimply);
  C = C.UnivAbstract(bdd_state->vars);
  (*oldsize)++;
  *old = C;
}
/*****************************************************************************/
/* useless: detects covers which are of no potential use                     */
/*                                                                           */
/*****************************************************************************/
void USELESS(Cudd *mgr, bddADT bdd_state, BDD *cover, int size){
  BDD temp,*cubes,*states,*masks,mask,C;
  int i,j,k,nlits;
  bdd_node *nd;

  nlits = 2*bdd_state->nsigs;
  cubes = new BDD[size];
  masks = new BDD[size];
  states = new BDD[size];
  temp = *cover;
  /* get individual cubes */
  for (i = 0; i< size; i++){
    mask = mgr->bddOne();
    for (j=0;j<size;j++){
      if (i != j)
	for (k=bdd_state->covs.start;
	     k<bdd_state->covs.stop;
	     k+=bdd_state->covs.step){
	  mask *= mgr->bddVar(k + (j*bdd_state->cubestep));
	}
    }
    masks[i] = mask;
    cubes[i] = temp.ExistAbstract(mask);
  }

  /* print cube set and individual cubes */
//    printf("\n");
//    temp.PrintMinterm();
//    for (i = 0; i< size; i++){
//      cubes[i].PrintMinterm();
//    } 
  
  /* get state sets */
  nd = bdd_state->covers.head;
  for (i=1;i<size;i++)
    nd = nd->next; 
  C = nd->data;
  for (i = 0;i<size;i++){
    states[i] = cubes[i] * C.UnivAbstract(masks[i]);   
    states[i] = states[i].ExistAbstract(bdd_state->cvars);
    states[i] = states[i] * bdd_state->S;
    /* print state set */
    //states[i].PrintMinterm();
 }

  /* does it cover any reachable states? */
  
  for (i = 0; i< size; i++)
    if (states[i] == mgr->bddZero()){
      *cover = states[i];
      break;
    }

  /* does it include duplicate covers? */
  if (*cover != mgr->bddZero())
    for (i = 0;i<size;i++)
      for (j = i+1;j<size;j++){
	if (states[i] <= states[j]){
	  *cover = mgr->bddZero();
	}
	if (states[j] <= states[i]){
	  *cover = mgr->bddZero();
	}
      }	 

  delete[] states;
  delete[] masks;
  delete[] cubes;
}

/*****************************************************************************/
/* checksize: checks if gc meets max stackeight requirement.               */
/*                                                                           */
/*****************************************************************************/
void CHECKsize(Cudd *mgr, bddADT bdd_state, BDD *cover, int var, int maxsize,
	       int nsigs, int ncubes, bool twolevel){

  int *weights,size;
  int i,j;

  weights = new int[bdd_state->lastvar];
  for (i = 0; i < bdd_state->lastvar; i++) 
    weights[i]=0;
  for (j = 0; j < ncubes; j++) {
    for ( i = bdd_state->covs.start;
	  i < bdd_state->covs.stop;
	  i += bdd_state->covs.step){
      weights[i+j*bdd_state->cubestep] = 1;
      weights[i+1+j*bdd_state->cubestep] = 1;
    }
    size = (* cover).ShortestLength(weights);
    //    printf("Cube = %d size = %d\n",j,size);
    if ((size > maxsize) || (twolevel && (j >= (ncubes/2)) && 
	((* cover).ShortestLength(weights) > (maxsize-1)))) {
      (* cover) = mgr->bddZero();
      return;
    }
    for ( i = bdd_state->covs.start;
	  i < bdd_state->covs.stop;
	  i += bdd_state->covs.step){
      weights[i+j*bdd_state->cubestep] = 0;
      weights[i+1+j*bdd_state->cubestep] = 0;
    }
  }
  delete[] weights;
}

/*****************************************************************************/
/* checkit: checks cover for compliance to restrictions                      */
/*                                                                           */
/*****************************************************************************/
void CHECKit(Cudd *mgr, bddADT bdd_state, BDD *cover, BDD space, 
	     int size, int var, bool combo, bool atomic, int maxsize, 
	     int nsigs){
  if (combo)
    COMBOit(mgr,bdd_state,cover,space,size,var);
  CHECKsize(mgr, bdd_state, cover, var, maxsize, nsigs, size, 0);
}

/*****************************************************************************/
/* findmap: finds and removes first minimum literal cover                    */
/*                                                                           */
/*****************************************************************************/

void find_MAP(Cudd *mgr, int nsigs, int ninpsig, bddADT bdd_state,
	      bool combo, bool atomic ,int maxsize, int sig){
  const int litover=0;
  int i,j,maxlits,minlits;
  int *weights;
  BDD temp;

  /* set weights for shortest paths: only literals count */
  weights = new int[bdd_state->lastvar];
  for (i=0; i < bdd_state->lastvar; i++) {
    weights[i]=0;
  }
  for (j = 0; j < bdd_state->max_cubes; j++)
    for (i=bdd_state->covs.start; 
	 i < bdd_state->covs.stop; 
	 i += bdd_state->covs.step) {
      weights[i+j*bdd_state->cubestep]=1;
      weights[i+1+j*bdd_state->cubestep]=1;
  }

  /* find least possible literal count */
  temp = bdd_state->CCplus[sig].ShortestPath(weights,NULL,&minlits);
  maxlits = minlits + litover;

  /* Copy solution set */
  temp = bdd_state->CCplus[sig];

  /* search sea for solutions */
  do {
    PICKit(mgr,bdd_state,&temp,
	   &(bdd_state->MAPplus[sig]),&(bdd_state->MAPpsize[sig]),
	   bdd_state->CCpsize[sig],2*nsigs,0);
    if (bdd_state->MAPpsize[sig] > maxlits){
      bdd_state->MAPplus[sig] = mgr->bddZero();
      temp = mgr->bddZero();
      break;
    }
    /*if (bdd_state->CCpsize[i] > 1)
      USELESS(mgr, bdd_state, &(bdd_state->MAPplus[i]),
	bdd_state->CCpsize[i]);*/
    if (bdd_state->MAPplus[sig] != mgr->bddZero())
      CHECKit(mgr, bdd_state, &(bdd_state->MAPplus[sig]),
	      (bdd_state->ESplus[sig]+bdd_state->QSplus[sig]),
	      bdd_state->CCpsize[sig],sig,combo,atomic,maxsize,nsigs);
  }while ((bdd_state->MAPplus[sig] == mgr->bddZero())&&
	  (temp != mgr->bddZero()));
  
  /* find least possible literal count */
   temp = bdd_state->CCminus[sig].ShortestPath(weights,NULL,&minlits);
  maxlits = minlits + litover;

  /* Copy solution set */
  temp = bdd_state->CCminus[sig];
  
  /* search sea for solutions */
  do {
    PICKit(mgr,bdd_state,&temp,
	   &(bdd_state->MAPminus[sig]),&(bdd_state->MAPmsize[sig]),
	   bdd_state->CCmsize[sig],2*nsigs,0);
    //printf("minlits = %d, lits = %d\n",minlits, bdd_state->MAPmsize[i]);
    if (bdd_state->MAPmsize[sig] > maxlits){
	bdd_state->MAPminus[sig] = mgr->bddZero();
	temp = mgr->bddZero();
	break;
    }
    /*if (bdd_state->CCmsize[i] > 1)
      USELESS(mgr, bdd_state, &(bdd_state->MAPminus[i]),
      bdd_state->CCmsize[i]);*/
      if (bdd_state->MAPminus[sig] != mgr->bddZero())
	CHECKit(mgr, bdd_state, &(bdd_state->MAPminus[sig]),
		(bdd_state->ESminus[sig]+bdd_state->QSminus[sig]),
		bdd_state->CCmsize[sig],sig,combo,atomic,maxsize,nsigs);
  }while ((bdd_state->MAPminus[sig] == mgr->bddZero())&&
	  (temp != mgr->bddZero()));
  delete[] weights;
}

/*****************************************************************************/
/* findmap: finds and removes first minimum literal cover                    */
/*                                                                           */
/*****************************************************************************/

void find_MAP2(Cudd *mgr, int nsigs, int ninpsig, bddADT bdd_state,
	       bool combo, bool atomic ,int maxsize, int sig){
  BDD temp;

  //  for (i=0;i<nsigs-ninpsig;i++){
  if ((bdd_state->MAPplus[sig] == mgr->bddZero()) ||
      (bdd_state->MAPminus[sig] == mgr->bddZero())){
    do {
      PICKit(mgr,bdd_state,
	     &(bdd_state->CCplus[sig]),&(bdd_state->MAPplus[sig]),
	     &(bdd_state->MAPpsize[sig]),bdd_state->CCpsize[sig],
	     2*nsigs,1);
      //bdd_state->MAPplus[sig].PrintMinterm();
      /*
	  if (bdd_state->CCpsize[sig] > 1)
	  USELESS(mgr, bdd_state, &(bdd_state->MAPplus[sig]),
	  bdd_state->CCpsize[sig]);
      */
      if (bdd_state->MAPplus[sig] != mgr->bddZero())
	CHECKsize(mgr, bdd_state, &(bdd_state->MAPplus[sig]), sig, maxsize, 
		  nsigs, 8, 1);
    }while ((bdd_state->MAPplus[sig] == mgr->bddZero())&&
	      (bdd_state->CCplus[sig] != mgr->bddZero()));
  }
  //}
}

/*****************************************************************************/
/* print_BDDlist: prints BDD sets;                                           */
/*****************************************************************************/
void print_BDDlist(int nsigs, int ninpsigs, bddADT bdd_state, 
		   signalADT *signals, int *mask, char * isit, 
		   char dir,bdd_list *flist, FILE *fp =stdout, 
		   int use_masks = 0) 
{
  int i,j,k;
  bdd_node *nd;

  for (i = ninpsigs,k=0;i<nsigs;i++,k++)
    for (nd = flist[k].head,j=0;nd;nd = nd->next,j++){
      fprintf(fp,"%s(%s%c,%d) = \n",isit,signals[i]->name,dir,j);
      if (use_masks){
	mask_PrintMinterm(nd->data,
			  find_mask(bdd_state->covmasks,nd->size),fp);
      }
      else
	mask_PrintMinterm(nd->data,mask,fp);
    }
}

void print_BDDlist(int nsigs, int ninpsigs, bddADT bdd_state, 
		   signalADT *signals, int *mask, char * isit, 
		   char dir,BDD *flist, FILE *fp =stdout, int use_masks =0,
		   int* masks = NULL) 
{
  int i,k;

  for (i = ninpsigs,k=0;i<nsigs;i++,k++){
    fprintf(fp,"%s(%s%c) =  \n",isit,signals[i]->name,dir);
    if (use_masks){
      mask_PrintMinterm(flist[k],
			find_mask(bdd_state->covmasks,masks[k]),fp);      
    }
    else
      mask_PrintMinterm(flist[k],mask,fp);
  }
}

/*****************************************************************************/
/* print state graph;                                                        */
/*****************************************************************************/
void print_N_and_S(bddADT bdd_state,FILE* fp){
  fprintf(fp,"S = \n");
  mask_PrintMinterm(bdd_state->S,bdd_state->sigmask,fp); 
  fprintf(fp,"N = \n");
  mask_PrintMinterm(bdd_state->N,bdd_state->nmask,fp); 
}

void dotprint(BDD f,char * filename)
{
  /* open .dot file */
  char * outname = (char *)GetBlock(strlen(filename) + 5);
  FILE* fp;
  strcpy(outname,filename);
  strcat(outname,".dot");
  fp=Fopen(outname,"w");
  DdNode * pnode = (DdNode*)f.getNode();

  Cudd_DumpDot((f.manager())->getManager(),1,&pnode,0,0,fp);
  

  /* clean up */  
  fclose(fp);
  free(outname);
}

/*****************************************************************************/
/* Print state vectoreader.                                                */
/*****************************************************************************/

void print_cover_vector(FILE *fp,signalADT *signals,int nsignals,
			int ninpsig,int ncovers)
{
  int i,j;
  
  fprintf(fp,"LITERALVECTOR:");
  for (j=0;j<ncovers;j++){
    if (j)
      fprintf(fp,"+"); 
    fprintf(fp,"<");
    for (i=0;i<nsignals;i++) {
      //if (i<ninpsig)
      //  fprintf(fp,"INP ");
      fprintf(fp,"%s ",signals[i]->name);
      fprintf(fp,"~%s ",signals[i]->name);
    }
    fprintf(fp,">");
  }
  fprintf(fp,"\n");
}

/*****************************************************************************/
/* Print cover sets.                                                         */
/*****************************************************************************/
void print_cover(Cudd *mgr, bddADT bdd_state, signalADT *signals, 
		 int sig, BDD cover,int up, FILE *fp =stdout){
  int i,j,k,flag=0,cubeflag=0;
  char *term;
  BDD temp = cover;
  BDD wipe;


  term = new char[bdd_state->lastvar];
  while (temp != mgr->bddZero()){
    cubeflag=0;
    temp.PickOneCube(term);
    fprintf(fp,"[%s%s: ",up?"+":"-",signals[sig]->name);
    for (k=0;k< bdd_state->max_cubes;k++){
      if (!term[bdd_state->unused.start+k*bdd_state->unused.step]){
	for (i=bdd_state->covs.start,j=0; 
	     i < bdd_state->covs.stop; 
	     i += bdd_state->covs.step,j++){
	  if (term[i + k*bdd_state->cubestep] == 1){
	  fprintf(fp,"%s%s",cubeflag++?(flag?" & ":"|("):"(",
		  signals[j]->name);
	  flag++;
	  }
	if (term[i + k*bdd_state->cubestep] == 2){
	  fprintf(fp,"%s<%s>",cubeflag++?(flag?" & ":"|("):"(",
		  signals[j]->name);
	  flag++;
	}
	if (term[i + 1+ k*bdd_state->cubestep] == 1){
	  fprintf(fp,"%s~%s",cubeflag++?(flag?" & ":"|("):"(",
		  signals[j]->name);
	  flag++;
	}
	if (term[i + 1+ k*bdd_state->cubestep] == 2){
	  fprintf(fp,"%s<~%s>",cubeflag++?(flag?" & ":"|("):"(",
		  signals[j]->name);
	  flag++;
	}
      }
	if (flag)
	  fprintf(fp,")");
	flag = 0;
      }
    }
    fprintf(fp,"]\n");
    wipe = mgr->bddOne();
    for (i=0;i<bdd_state->lastvar;i++){
      if (term[i] == 1)
	wipe *= mgr->bddVar(i);
      if (term[i] == 0)
	wipe *= !(mgr->bddVar(i));
    }
    temp -= wipe;
  }
  delete[] term;
}

/*****************************************************************************/
/* Print .cov results.                                                       */
/*****************************************************************************/
void print_COV(Cudd *mgr, int nsigs, int ninpsigs, bddADT bdd_state,
	       bdd_list *flistp, bdd_list *flistm, signalADT *signals,
	       FILE *fp =stdout){
  int i,j,k;
  bdd_node *nd;

  for (i = ninpsigs,k=0;i<nsigs;i++,k++){
    for (nd = flistp[k].head,j=0;nd;nd = nd->next,j++){
      fprintf(fp,"Covers for %s+,%d\n",signals[i]->name,j);
      print_cover(mgr,bdd_state,signals,i,nd->data,1,fp);
    }
    for (nd = flistm[k].head,j=0;nd;nd = nd->next,j++){
      fprintf(fp,"Covers for %s-,%d\n",signals[i]->name,j);
      print_cover(mgr,bdd_state,signals,i,nd->data,0,fp);      
    }
  }
}
/*****************************************************************************/
/* Print solution cube.                                                      */
/*****************************************************************************/
void print_cube(Cudd *mgr, bddADT bdd_state, signalADT *signals, 
		int sig, BDD cover, int cube, bool up, bool prime, 
		bool printprime, FILE *fp =stdout){
  int i,j,flag = 0;
  char *term;
  
  term = new char[bdd_state->lastvar];
  cover.PickOneCube(term);
  if (!term[bdd_state->unused.start+cube*bdd_state->unused.step]){
      fprintf(fp,"[%s%s%s:",up?"+":"-",signals[sig]->name,prime?"'":"");
    for (i=bdd_state->covs.start,j=0; 
	 i < bdd_state->covs.stop; 
	 i += bdd_state->covs.step,j++){
      if (term[i + cube*bdd_state->cubestep] == 1){
	fprintf(fp," %s%s",flag++?"& ":"(",signals[j]->name);
      }
      if (term[i + cube*bdd_state->cubestep] == 2){
	fprintf(fp," %s<%s>",flag++?"& ":"(",signals[j]->name);
      }
      if (term[i + 1+ cube*bdd_state->cubestep] == 1){
	fprintf(fp," %s~%s",flag++?"& ":"(",signals[j]->name);
      }
      if (term[i + 1+ cube*bdd_state->cubestep] == 2){
	fprintf(fp," %s<~%s>",flag++?"& ":"(",signals[j]->name);
      }
    }
    if (printprime)
      fprintf(fp," %s%s%s'",flag?"& ":"(",up?"":"~",signals[sig]->name);
    fprintf(fp,")]\n");
  }
  delete[] term;
}

/*****************************************************************************/
/* Print gC decompositions                                                   */
/*****************************************************************************/
void printcubes(Cudd *mgr, bddADT bdd_state, signalADT *signals, FILE *fp,
		   BDD cover, int cubes, int sig, bool up){
  int i;
  for (i = 0; i< cubes; i++){
    print_cube(mgr,bdd_state,signals,sig, cover,i,up,0,0,fp);
  }
}
/*****************************************************************************/
/* Print gC decompositions                                                   */
/*****************************************************************************/
void twolevelprint(Cudd *mgr, bddADT bdd_state, signalADT *signals, FILE *fp,
		   BDD cover, int sig){
  
  print_cube(mgr,bdd_state,signals,sig, cover,0,1,1,0,fp);
  print_cube(mgr,bdd_state,signals,sig, cover,1,1,1,0,fp);
  print_cube(mgr,bdd_state,signals,sig, cover,2,0,1,0,fp);
  print_cube(mgr,bdd_state,signals,sig, cover,3,0,1,0,fp); 
  print_cube(mgr,bdd_state,signals,sig, cover,4,1,0,1,fp);
  print_cube(mgr,bdd_state,signals,sig, cover,5,1,0,1,fp); 
  print_cube(mgr,bdd_state,signals,sig, cover,6,0,0,1,fp);
  print_cube(mgr,bdd_state,signals,sig, cover,7,0,0,1,fp); 
}
/*****************************************************************************/
/* Print solutions to file                                                   */
/*****************************************************************************/
void print_MAP(Cudd *mgr, bddADT bdd_state, signalADT *signals,int nsigs,
	       FILE *fp){
  int i,j;
  
  for (i = 0, j= bdd_state->ninpsigs; j<nsigs; i++,j++)
    if (bdd_state->MAPminus[i] == mgr->bddZero()) {
      if (bdd_state->MAPplus[i] != mgr->bddZero())
	twolevelprint(mgr,bdd_state,signals,fp,bdd_state->MAPplus[i],j);
    }else{
      printcubes(mgr,bdd_state,signals,fp,bdd_state->MAPplus[i],
		 bdd_state->CCpsize[i],j,1);
      printcubes(mgr,bdd_state,signals,fp,bdd_state->MAPminus[i],
		 bdd_state->CCmsize[i],j,0);
    }
}
		
		
/*****************************************************************************/
/* image calculation fxn.                                                    */
/*                                                                           */
/*****************************************************************************/
BDD image(Cudd *mgr, bddADT bdd_state, BDD N, BDD S){
  BDD newS;
  newS = ((S * N).ExistAbstract(bdd_state->vars));
  newS = newS.SwapVariables(*(bdd_state->pv),*(bdd_state->v)) + S;
  return newS;
}


/*****************************************************************************/
/* gate array creation                                                       */
/*                                                                           */
/*****************************************************************************/
void make_gate(Cudd *mgr, bddADT bdd_state, int up, int upp, int dp, 
	       int dpp, BDD* U,BDD* Up, BDD* D, BDD* Dp){
  
  BDD Upp,Dpp;
  bdd_node *cov;

  /* printing is not correct if cubes are not entered in as unused. */
  printf("making the gate\n");
  cov = bdd_state->cubes.head;
  *Up = cov->data;
  cov = cov->next;
  if (up > 1)
    *Up += cov->data;
  cov = cov->next;
  *Dp = cov->data;
  cov = cov->next;
  if (dp > 1)
    *Dp += cov->data;
  cov = cov->next;
  Upp = cov->data;
  cov = cov->next;
  if (upp > 1)
    Upp += cov->data;
  cov = cov->next;
  Dpp = cov->data;
  cov = cov->next;
  if (dpp > 1)
    Dpp += cov->data;
  *U = *Up * Upp;
  *D = *Dp * Dpp;

}


/*****************************************************************************/
/* decomposition routine ala HSE                                             */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
void find_decomp(Cudd *mgr, int nsigs, int ninpsig, bddADT bdd_state, 
		 BDD S, BDD N,signalADT *signals,int sig, int entry){
  int i,j,k,shuffled = 0;
  int *top, *leaved;
  BDD C;
  BDD Up,Dp,U,D,Szp,Snzp,Nzconst,CSp,CSm;
 
  /* make local copies */
  BDD *QSplus = bdd_state->QSplus;
  BDD *ESplus = bdd_state->ESplus;
  BDD *QSminus = bdd_state->QSminus;
  BDD *ESminus = bdd_state->ESminus;

  top = new int[bdd_state->lastvar];
  leaved = new int[bdd_state->lastvar];
  for (i=0;i<bdd_state->lastvar;i++)
    leaved[i] = i;
  for (i=0,j=bdd_state->sigs.start;
       j<bdd_state->sigs.stop;
       i+=2,j+=bdd_state->sigs.step){
    top[i] = j;
    top[i+1] = j+1;
  }
  for (j=bdd_state->unused.start;
       j<bdd_state->unused.stop;
       i++,j+=bdd_state->unused.step)
    top[i] = j;
  for (j = 0; j<bdd_state->max_cubes;j++)
    for (k=bdd_state->covs.start;
	 k<bdd_state->covs.stop;
	 i+=2,k+=bdd_state->covs.step){
      top[i] = k+j*bdd_state->cubestep;
      top[i+1] = k+1+j*bdd_state->cubestep;
    }
  for (j=bdd_state->rules.start;
       j<bdd_state->rules.stop;
       i++,j+=bdd_state->rules.step)
    top[i] = j;
  for (j=bdd_state->rows.start;
       j<bdd_state->rows.stop;
       i+=2,j+=bdd_state->rows.step){
    top[i] = j;
    top[i+1] = j+1;
  }

  bdd_state->cubes.wipeout();
  while (bdd_state->cubes.count < 8)
    make_cube(mgr,bdd_state,sig);
  
  make_gate(mgr,bdd_state,2,2,2,2,&U,&Up,&D,&Dp);
  
  bdd_state->covers.wipeout();
  bdd_state->cubes.wipeout();
  
  printf("had to decompose %s\n",signals[sig+ninpsig]->name);
    /* initialize variables */
  Szp = S * (Up + mgr->bddVar(entry));
  Snzp = S * (Dp + !(mgr->bddVar(entry)));
  Nzconst = N * ((mgr->bddVar(entry) * mgr->bddVar(entry+1)) +
		 (!(mgr->bddVar(entry)) * !(mgr->bddVar(entry+1))));
  CSp = QSplus[sig] + ESplus[sig];
  CSm = QSminus[sig] + ESminus[sig];
  
  /* Check stability */
  C = !(image(mgr,bdd_state,Nzconst,Szp)) + Szp;
  C *= !(image(mgr,bdd_state,Nzconst,Snzp)) + Snzp;
    
  /* Check non-interference */
  C *= !S + (!Up + !Dp);
  
  /* Check complex gate equivalence */
  C *= !(ESplus[sig]) + U;
  C *= !(ESminus[sig]) + D;
  C *= !(CSp) + !D;
  C *= !(CSm) + !U;
  C *= !S + (!U + !D);
  
  printf("gate-equiv\n");
  //mgr->AutodynEnable(CUDD_REORDER_RANDOM_PIVOT);
  if (!shuffled){
    printf("shuffling\n");
    shuffled++;
    mgr->ShuffleHeap(top);
    printf("shuffled\n");
  }
  C = C.UnivAbstract(bdd_state->vars);
  printf("abstracted\n");      
  bdd_state->CCplus[sig]=C;
  bdd_state->CCpsize[sig]=8;
  bdd_state->CCminus[sig]=mgr->bddZero();
  bdd_state->MAPminus[sig]=mgr->bddZero();
  printf("done\n");      
  //mask_PrintMinterm(C,find_mask(bdd_state->covmasks,4),stdout);
  if (0){
    printf("shuffling\n");
    mgr->ShuffleHeap(leaved);
    printf("shuffled\n");
  }
  delete[] top;
  delete[] leaved;

}


/*****************************************************************************/
/* CUDD based synthesis engine.                                              */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
void cudd_synth_proc(char * filename,signalADT *signals,bddADT bdd_state,
		     int nsigs,int ninpsig,int nrules, bool verbose,
		     bool gatelevel,int maxsize, bool combo, bool atomic)
{
  Cudd *mgr = bdd_state->mgr;
  BDD N=bdd_state->N,S=bdd_state->S;
  BDD cleanN,Sp,ghosts;
  char outname[FILENAMESIZE];
  FILE* fp;
  int sig,entry;

  make_cover(mgr,bdd_state);
  /* update masks */
  init_mask(bdd_state);
  
  /* make unhaunted transition relation */
  /* move state space into prime vars. */
  Sp = S.SwapVariables(*(bdd_state->v),*(bdd_state->pv));
  cleanN = N * Sp;

  /* print rsg */
  if (0){
    strcpy(outname,filename);
    strcat(outname,".NS");
    fp=Fopen(outname,"w");
    print_N_and_S(bdd_state,fp);
    fclose(fp);
  }

  /* ER file prep */
  if (0){
    strcpy(outname,filename);
    strcat(outname,".es");
    printf("Finding excitation regions and storing to: %s...",outname);
    fprintf(lg,"Finding excitation regions and storing to: %s...",outname);
  }  
  else{
    printf("Finding excitation regions...");
    fprintf(lg,"Finding excitation regions...");
  }
    
  find_ER(mgr,nsigs,ninpsig,N,S,bdd_state,signals,verbose,bdd_state->sigmask);
  
  /* ER filedump */
  if (0){
    fp=Fopen(outname,"w");
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->sigmask,"ER",'+',
		  bdd_state->ERplus,fp);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->sigmask,"ER",'-',
		  bdd_state->ERminus,fp);
    fclose(fp);
  }
  printf(" done\n");
  fprintf(lg," done\n");

  /* ES & QS file dump */
  if (0){
    strcpy(outname,filename);
    strcat(outname,".qs");
    fp=Fopen(outname,"w");
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->sigmask,"ES",'+',
		  bdd_state->ESplus,fp);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->sigmask,"ES",'-',
		  bdd_state->ESminus,fp);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->sigmask,"QS",'+',
		  bdd_state->QSplus,fp);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->sigmask,"QS",'-',
		  bdd_state->QSminus,fp);
    fclose(fp);
  }

  /* cov file prep */
  if (verbose){
    strcpy(outname,filename);
    strcat(outname,".cov");
    printf("Finding all valid covers and storing to: %s...",outname);
    fprintf(lg,"Finding all valid covers and storing to: %s...",outname);
  }
  else{
    printf("Finding all valid covers.");
    fprintf(lg,"Finding all valid covers.");
  }
  find_VC(mgr,nsigs,ninpsig,nrules,N,S,bdd_state,signals,verbose,
	  bdd_state->sigmask,gatelevel);
  if (verbose){
    fp=Fopen(outname,"w");
    /* cov file dump */
    print_COV(mgr,nsigs,ninpsig,bdd_state,bdd_state->VCplus,
	      bdd_state->VCminus,signals,fp);
  }
  printf(" done\n");
  fprintf(lg," done\n");

//    /* BC file prep */
//    if (verbose){
//      strcpy(outname,filename);
//      strcat(outname,".BC");
//      printf("Finding basic covers and storing to: %s...",outname);
//      fprintf(lg,"Finding basic covers and storing to: %s...",outname);
//    }  
//    else{
  printf("Finding basic covers...");
  fprintf(lg,"Finding basic covers...");
//    }
  for (sig = 0; sig < nsigs - ninpsig;sig ++){
    find_BC(mgr,nsigs,ninpsig,nrules,N,S,bdd_state,signals,verbose,
	    bdd_state->sigmask,gatelevel,sig);
    find_MAP(mgr,nsigs,ninpsig,bdd_state,combo,atomic,maxsize,sig);
    
  }
//    /* BC filedump */
//    if (verbose){
//      fp=Fopen(outname,"w");
//      print_cover_vector(fp,signals,nsigs,ninpsig,bdd_state->covers.count);
//      print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->covmask,"BC",'+',
//  		  bdd_state->CCplus,fp,1,bdd_state->CCpsize);
//      print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->covmask,"BC",'-',
//  		  bdd_state->CCminus,fp,1,bdd_state->CCmsize);
//      fclose(fp);
//    }
  printf(" done\n");
  fprintf(lg," done\n");

  /* find support sets */
  for (sig = 0; sig < nsigs - ninpsig;sig ++){
    bdd_state->SSplus[sig] = (bdd_state->CCplus[sig]).Support();
    bdd_state->SSminus[sig] = (bdd_state->CCminus[sig]).Support();
  }  

  //printf("%x\n",&(bdd_state->MAPplus[0]));

  /* MAP file setup */
  if (0){
    strcpy(outname,filename);
    strcat(outname,".MAP");
    printf("Storing smallest covers to: %s...",outname);
    fprintf(lg,"Storing smallest covers to: %s...",outname);
    fp=Fopen(outname,"w");
    print_cover_vector(fp,signals,nsigs,ninpsig,bdd_state->covers.count);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->covmask,
		  "MAP",'+',bdd_state->MAPplus,fp,1,bdd_state->CCpsize);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->covmask,
		  "MAP",'-',bdd_state->MAPminus,fp,1,bdd_state->CCmsize);
    fclose(fp);
    printf(" done\n");
    fprintf(lg," done\n");
  }
   

//    if (verbose){
//      strcpy(outname,filename);
//      strcat(outname,".DEC");
//      printf("Finding valid decompositions and storing to: %s...",outname);
//      fprintf(lg,"Finding valid decompositions and storing to: %s...",outname);
//    }  
//    else{
    printf("Finding valid decompositions...");
    fprintf(lg,"Finding valid decompositions...");
//  }

  for (entry=bdd_state->sigs.start+bdd_state->sigs.step*ninpsig,sig=0;
       entry<bdd_state->sigs.stop; sig++,entry+=bdd_state->sigs.step) {
    if ((bdd_state->MAPplus[sig] == mgr->bddZero()) ||
	(bdd_state->MAPminus[sig] == mgr->bddZero())){
      find_decomp(mgr,nsigs,ninpsig,bdd_state,S,cleanN,signals,sig,entry);
      find_MAP2(mgr,nsigs,ninpsig,bdd_state,combo,atomic,maxsize,sig);
      if (bdd_state->MAPplus[sig] == mgr->bddZero()) 
	printf("%s could not be mapped.\n",signals[ninpsig+sig]->name);
      bdd_state->CCplus[sig] = mgr->bddZero();
    }
  }
//    printf("gothere\n");
//    /* BC filedump */
//    if (verbose){
//      fp=Fopen(outname,"w");
//      print_cover_vector(fp,signals,nsigs,ninpsig,bdd_state->covers.count);
//      print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->covmask,"DEC",' ',
//  		  bdd_state->CCplus,fp,1,bdd_state->CCpsize);
//      fclose(fp);
//    } 
  printf(" done\n");
  fprintf(lg," done\n");
  
  /* MAP file setup */
  if (0){
    strcpy(outname,filename);
    strcat(outname,".MAP2");
    printf("Finding smallest covers and storing to: %s...",outname);
    fprintf(lg,"Finding smallest covers and storing to: %s...",outname);
  }  
  else{
    printf("Finding smallest covers...");
    fprintf(lg,"Finding smallest covers...");
  } 

  /* MAP filedump */
  if (0){
    fp=Fopen(outname,"w");
    print_cover_vector(fp,signals,nsigs,ninpsig,bdd_state->covers.count);
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->covmask,
		  "MAP",' ',bdd_state->MAPplus,fp,1,bdd_state->CCpsize);
    /*
    print_BDDlist(nsigs,ninpsig,bdd_state,signals,bdd_state->nomask,
		  "MAP",' ',bdd_state->MAPplus,fp,0,bdd_state->CCpsize);
    */
    fclose(fp);
  }
   
  printf(" done\n");
  fprintf(lg," done\n");

  /* prs file setup */
  strcpy(outname,filename);
  strcat(outname,".prs");
  printf("Storing final solutions to: %s...",outname);
  fprintf(lg,"Storing final solutions to: %s...",outname);
  fp=Fopen(outname,"w");
  print_MAP(mgr,bdd_state,signals,nsigs,fp);
  fclose(fp);
  
   
  printf(" done\n");
  fprintf(lg," done\n");

}


void cudd_synthesis(char command,designADT design)
{
  if (!(design->status & STOREDPRS)) {
    cudd_synth_proc(design->filename,design->signals,design->bdd_state,
            design->nsignals,design->ninpsig,design->nrules,design->verbose,
            design->gatelevel,design->maxsize,design->combo,design->atomic);
    design->status = design->status | FOUNDREG | FOUNDCONF | FOUNDCOVER |
      STOREDPRS;
  }
}
#endif
