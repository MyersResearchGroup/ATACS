#include "bcp.h"
#include "misclib.h"
#include "cstring"

/*int nsolutions;*/

void delete_table(tableADT F) {
  tableADT cur,next;

  cur=F;
  while (cur) {
    next=cur->link;
    free(cur->row);
    free(cur);
    cur=next;
  }
}

void delete_tableSol(tableSolADT Sol) {
  if (Sol) {
    delete_table(Sol->F);
    free(Sol->currentSol);
    free(Sol);
  }
}

int solved(char row[],char sol[],int size) {
  int i;

  for (i=0;i<size;i++)
    if (row[i]=='1' && sol[i]=='1') return 1;
  return 0;
}

tableSolADT find_essential(tableSolADT Sol,int *changed) {
  tableADT cur,New,old=NULL;
  int essential,i;
  tableSolADT NewSol;

  NewSol=(tableSolADT)GetBlock(sizeof(*(NewSol)));
  NewSol->currentSol=CopyString(Sol->currentSol);
  NewSol->size=Sol->size;
  NewSol->cost=Sol->cost;
  NewSol->F=NULL;

  for (cur=Sol->F;cur;cur=cur->link) {
    essential=(-1);
    for (i=0;i<Sol->size;i++)
      if (cur->row[i]=='1')
	if (essential==(-1)) 
	  essential=i;
	else {
	  essential=(-1);
	  break;
	}
    if (essential!=(-1)) {
      (*changed)=1;
      NewSol->currentSol[essential]='1';
    }
  }
  for (cur=Sol->F;cur;cur=cur->link) 
    if (!solved(cur->row,NewSol->currentSol,NewSol->size)) {
      New=(tableADT)GetBlock(sizeof(*(New)));
      New->row=CopyString(cur->row);
      New->size=cur->size;
      New->link=old;
      old=New;
    }
  NewSol->F=old;
  return NewSol;
}

tableSolADT rm_dominating_rows(tableSolADT Sol,int *changed) {
  tableADT cur,cur2,New,old=NULL;
  int dominating,i,seenit;
  tableSolADT NewSol;

  NewSol=(tableSolADT)GetBlock(sizeof(*(NewSol)));
  NewSol->currentSol=CopyString(Sol->currentSol);
  NewSol->size=Sol->size;
  NewSol->cost=Sol->cost;
  NewSol->F=NULL;

  for (cur=Sol->F;cur;cur=cur->link) {
    dominating=0;
    seenit=0;
    for (cur2=Sol->F;cur2;cur2=cur2->link) 
      if (cur!=cur2) {
	for (i=0;i<Sol->size;i++) 
	  if ((cur->row[i]=='0') && (cur2->row[i]=='1'))
	    break;
	if ((i==Sol->size) && (seenit || (strcmp(cur->row,cur2->row)!=0))) {
	  dominating=1;
	  break;
	}
      } else seenit=1;
    if (!dominating) {
      New=(tableADT)GetBlock(sizeof(*(New)));
      New->row=CopyString(cur->row);
      New->size=cur->size;
      New->link=old;
      old=New;
    } else
      (*changed)=1;
  }
  NewSol->F=old;
  return NewSol;
}

int dom(tableADT F,int i,int j) {
  int nonzero;

  nonzero=0;
  for (F=F;F;F=F->link) {
    if ((F->row[i]=='0') && (F->row[j]=='1')) return 0;
    if (F->row[j]!='0') nonzero=1;
  }
  if (!nonzero) return 0;
  return 1;
}

void rm_col(tableADT F,int i) {
  for (F=F;F;F=F->link)
    F->row[i]='0';
}

void rm_dominated_cols(tableSolADT Sol,int *changed,int *cost) {
  int i,j;
  bool *dom_cache;

  dom_cache=(bool *)GetBlock(Sol->size * sizeof(bool));
  for (i=0;i<Sol->size;i++)
    if (Sol->currentSol[i]=='1') dom_cache[i]=TRUE;
    else dom_cache[i]=FALSE;

  for (i=0;i<Sol->size;i++)
    if (!(dom_cache[i]))
      for (j=i+1;j<Sol->size;j++)
	if ((!(dom_cache[i])) && (!(dom_cache[j]))) {
	  if ((dom(Sol->F,i,j)) && (cost[j]>=cost[i])) {
	    (*changed)=1;
	    Sol->currentSol[j]='0';
	    rm_col(Sol->F,j);
	    dom_cache[j]=TRUE;
	  } else if ((dom(Sol->F,j,i)) && (cost[i]>=cost[j])) {
	    (*changed)=1;
	    Sol->currentSol[i]='0';
	    rm_col(Sol->F,i);
	    dom_cache[i]=TRUE;
	    break;
	  }
	}
  free(dom_cache);
}

tableSolADT reduce(tableSolADT Sol,int *cost) {
  int changed=1;
  tableSolADT NewSol;
  bool first=FALSE;

  while ((Sol->F) && (changed)) {
    changed=0;
    NewSol=find_essential(Sol,&changed);
    //printf("essential\n");
    //print_current_solution(stdout,NewSol);
    delete_tableSol(Sol);
    Sol=NewSol;
    NewSol=rm_dominating_rows(Sol,&changed);
    //printf("dom rows\n");
    //print_current_solution(stdout,NewSol);
    delete_tableSol(Sol);
    Sol=NewSol;
    if (!first)
      rm_dominated_cols(Sol,&changed,cost);
    //printf("dom col\n");
    //print_current_solution(stdout,NewSol);
    first=TRUE;
  }    
  return Sol;
}

void print_current_solution(FILE *fp,tableSolADT Sol) {
  tableADT M;

  fprintf(fp, "F:\n");
  for (M=Sol->F;M;M=M->link) 
    fprintf(fp,"%s\n",M->row);
  fprintf(fp, "CurrentSol=%s\n",Sol->currentSol);
}

int Cost(char sol[],int *cost,int size,int startcost) {
  int i,total;

  total=startcost;
  for (i=0;i<size;i++)
    if (sol[i]=='1') total=total+cost[i];
  return total;
}

int chooseVar(tableADT F) {
  int *counts;
  int i,max,size;
 
  size=F->size;
  counts=(int *)GetBlock(F->size * sizeof(int));
  max=(-1);
  for (i=0;i<F->size;i++)
    counts[i]=0;
  for (F=F;F;F=F->link)
    for (i=0;i<F->size;i++)
      if (F->row[i]=='1') counts[i]++;
  for (i=1;i<size;i++)
    if (((max==(-1)) && (counts[i]>0)) || (counts[i]>counts[max])) max=i;
  free(counts);
  return max;
}

tableSolADT selectCol(tableSolADT Sol,int x,int *cost) {
  tableADT cur,New,old=NULL;
  tableSolADT NewSol;
  int i,j,size,oldcost;

  Sol->currentSol[x]='1';
  oldcost=Sol->cost;
  for (i=0;i<Sol->size;i++)
    if (Sol->currentSol[i]=='1') oldcost=oldcost+cost[i];
  size=0;
  for (i=0;i<Sol->size;i++)
    if (Sol->currentSol[i]=='-') size++;
  NewSol=(tableSolADT)GetBlock(sizeof(*(NewSol)));
  NewSol->currentSol=(char *)GetBlock((size+1)*sizeof(char));
  for (i=0;i<size;i++)
    NewSol->currentSol[i]='-';
  NewSol->currentSol[size]='\0';
  NewSol->size=size;
  NewSol->cost=oldcost;
  NewSol->F=NULL;
  for (cur=Sol->F;cur;cur=cur->link) 
    if (!solved(cur->row,Sol->currentSol,Sol->size)) {
      New=(tableADT)GetBlock(sizeof(*(New)));
      New->row=(char *)GetBlock((size+1)*sizeof(char));
      j=0;
      for (i=0;i<Sol->size;i++)
	if (Sol->currentSol[i]=='-') {
	  New->row[j]=cur->row[i];
	  j++;
	}
      New->row[size]='\0';
      New->size=size;
      New->link=old;
      old=New;
    }
  NewSol->F=old;
  return NewSol;
}

tableSolADT removeCol(tableSolADT Sol,int x,int *cost) {
  tableADT cur,New,old=NULL;
  tableSolADT NewSol;
  int i,j,size,oldcost;

  Sol->currentSol[x]='0';
  oldcost=Sol->cost;
  for (i=0;i<Sol->size;i++)
    if (Sol->currentSol[i]=='1') oldcost=oldcost+cost[i];
  size=0;
  for (i=0;i<Sol->size;i++)
    if (Sol->currentSol[i]=='-') size++;
  NewSol=(tableSolADT)GetBlock(sizeof(*(NewSol)));
  NewSol->currentSol=(char *)GetBlock((size+1)*sizeof(char));
  for (i=0;i<size;i++)
    NewSol->currentSol[i]='-';
  NewSol->currentSol[size]='\0';
  NewSol->size=size;
  NewSol->cost=oldcost;
  NewSol->F=NULL;
  for (cur=Sol->F;cur;cur=cur->link) {
    New=(tableADT)GetBlock(sizeof(*(New)));
    New->row=(char *)GetBlock((size+1)*sizeof(char));
    j=0;
    for (i=0;i<Sol->size;i++)
      if (Sol->currentSol[i]=='-') {
	New->row[j]=cur->row[i];
	j++;
      }
    New->row[size]='\0';
    New->size=size;
    New->link=old;
    old=New;
  }
  NewSol->F=old;
  return NewSol;
}

int mis_quick(tableADT M,int *cost) {
  int *mis;
  int *rm;
  tableADT cur;
  int shortest,size,i,j,nrows;
  char *row;

  mis=(int *)GetBlock(M->size * sizeof(int));
  rm=(int *)GetBlock(M->size * sizeof(int));
  row=(char *)GetBlock((M->size+1) * sizeof(char));
  for (cur=M,nrows=0;cur;cur=cur->link,nrows++);
  for (i=0;i<nrows;i++) {
    mis[i]=0;
    rm[i]=0;
  }
  do {
    shortest=(-1);
    for (cur=M,i=0;cur;cur=cur->link,i++)
      if (mis[i]==0 && rm[i]==0)
	if ((shortest==(-1)) || (Cost(cur->row,cost,cur->size,0) < size)) {
	  shortest=i;
	  size=Cost(cur->row,cost,cur->size,0);
	  strcpy(row,cur->row);
	}
    mis[shortest]=1;
    for (cur=M,i=0;cur;cur=cur->link,i++)
      if (mis[i]==0 && rm[i]==0)
	for (j=0;j<cur->size;j++)
	  if ((row[j]=='1') && (cur->row[j]=='1')) {
	    rm[i]=1;
	    break;
	  }
    for (i=0;i<nrows;i++)
      if (mis[i]==0 && rm[i]==0) break;
  } while (i!=nrows);
  size=0;
  for (i=0;i<nrows;i++)
    if (mis[i]) size++;
  free(mis);
  free(rm);
  free(row);
  return size;
}

int lower_bound(tableSolADT Sol,int *cost) {
  return Cost(Sol->currentSol,cost,Sol->size,Sol->cost) + 
    mis_quick(Sol->F,cost);
}

void expand_solution(tableSolADT newtab,tableSolADT oldtab,int *cost) {
  int i,j,oldcost;
  char *solution;

  solution=(char *)GetBlock((oldtab->size+1)*sizeof(char));
  oldcost=0;
  for (i=0;i<oldtab->size;i++)
    if (oldtab->currentSol[i]=='1') oldcost=oldcost+cost[i];
  j=0;
  for (i=0;i<oldtab->size;i++) {
    if (oldtab->currentSol[i]=='-') {
      if (newtab->currentSol[j]=='-')
	solution[i]='0';
      else
	solution[i]=newtab->currentSol[j];
      j++;
    } else
      solution[i]=oldtab->currentSol[i];
  }
  solution[oldtab->size]='\0';
  free(newtab->currentSol);
  newtab->currentSol=solution;
  newtab->size=oldtab->size;
  newtab->cost=(newtab->cost)-oldcost;
}

tableSolADT BCP(tableSolADT Sol,int *U,int *cost,int depth) {
  tableSolADT NewSol,Sol1,Sol0;
  int i,j,x,L;
  int *newcost;

  //printf("depth=%d size=%d\n",depth,Sol->size);
  NewSol=reduce(Sol,cost);
  if ((NewSol) && (NewSol->F==NULL)) {
    if (Cost(NewSol->currentSol,cost,NewSol->size,NewSol->cost)<(*U)) {
      (*U)=Cost(NewSol->currentSol,cost,NewSol->size,NewSol->cost);
      return NewSol;
    } else {
      delete_tableSol(NewSol);
      return NULL;
    }
  }
  L=lower_bound(NewSol,cost);
  if (L >= (*U)) {
    delete_tableSol(NewSol);
    return NULL;
  }
  x=chooseVar(NewSol->F);
  if (x==(-1)) {
    //print_current_solution(stdout,NewSol);
    return NULL;
  }

  Sol1=selectCol(NewSol,x,cost);
  newcost=(int*)GetBlock(Sol1->size * sizeof(int));
  j=0;
  for (i=0;i<NewSol->size;i++)
    if (NewSol->currentSol[i]=='-') {
      newcost[j]=cost[i];
      j++;
    }
  Sol1=BCP(Sol1,U,newcost,depth+1);
  free(newcost);

  if (Sol1)
    expand_solution(Sol1,NewSol,cost);
  if ((Sol1) && (Cost(Sol1->currentSol,cost,Sol1->size,Sol1->cost)==L)) {
    delete_tableSol(NewSol);
    return Sol1;
  }

  Sol0=removeCol(NewSol,x,cost);
  newcost=(int*)GetBlock(Sol0->size * sizeof(int));
  j=0;
  for (i=0;i<NewSol->size;i++)
    if (NewSol->currentSol[i]=='-') {
      newcost[j]=cost[i];
      j++;
    }
  Sol0=BCP(Sol0,U,newcost,depth+1);
  free(newcost);
  if (Sol0)
    expand_solution(Sol0,NewSol,cost);

  delete_tableSol(NewSol);

  if (Sol0) {
    if (Sol1) {
      if (Cost(Sol1->currentSol,cost,Sol1->size,Sol1->cost) < 
	  Cost(Sol0->currentSol,cost,Sol0->size,Sol0->cost)) {
	delete_tableSol(Sol0);
	return Sol1;
      } else {
	delete_tableSol(Sol1);
	return Sol0;
      }
    } else {
      return Sol0;
    }
  }
  return Sol1;
}


