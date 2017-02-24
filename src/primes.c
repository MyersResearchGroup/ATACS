#include "primes.h"

int NInputs=0;
int NOutputs=0;
int NImps=0;

implicantADT read_implicants(FILE *fp) {
  char temp[255];
  implicantADT newimp,old=NULL;
  int i;

  fgets(temp,255,fp);
  for (i=3;temp[i]!='\n';i++)
    temp[i-3]=temp[i];
  temp[i]='\0';
  NInputs=atoi(temp);
  fgets(temp,255,fp);
  for (i=3;temp[i]!='\n';i++)
    temp[i-3]=temp[i];
  temp[i]='\0';
  NOutputs=atoi(temp);
  fgets(temp,255,fp);
  while (strcmp(temp,".e\n")!=0) {
    if (temp[0]!='.') {
      NImps++;
      newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
      for (i=0;i<NInputs;i++) 
	newimp->implicant[i]=temp[i];
      newimp->implicant[i]='\0';
      for (i=0;i<NOutputs;i++) 
	newimp->outputs[i]=temp[i+NInputs+1];
      newimp->outputs[i]='\0';
      newimp->subset=0;
      newimp->link=old;
      old=newimp;
    }
    fgets(temp,255,fp);
  }
  return(old);
}

void write_implicants(FILE *fp,implicantADT imps) {
  implicantADT curimps;

  NImps=0;
  curimps=imps;
  while (curimps) {
    NImps++;
    curimps=curimps->link;
  }
  fprintf(fp,".i %d\n",NInputs);
  fprintf(fp,".o %d\n",NOutputs);
  fprintf(fp,".p %d\n",NImps);
  while (imps) {
    fprintf(fp,"%s %s\n",imps->implicant,imps->outputs);
    imps=imps->link;
  }
  fprintf(fp,".e\n");
}

implicantADT pos_cofactor(implicantADT imps,int x) {
  implicantADT newimp,old=NULL;
  int i;

  while (imps) {
    if (x < NInputs) {
      if (imps->implicant[x]!='0') {
	newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
	strcpy(newimp->implicant,imps->implicant);
	strcpy(newimp->outputs,imps->outputs);
	newimp->implicant[x]='-';
	newimp->subset=0;
	newimp->link=old;
	old=newimp;
      }
    } else {
      if (imps->outputs[x-NInputs]!='0') {
	newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
	strcpy(newimp->implicant,imps->implicant);
	for (i=0;i<NOutputs;i++)
	  newimp->outputs[i]='1';
	newimp->outputs[NOutputs]='\0';
	newimp->subset=0;
	newimp->link=old;
	old=newimp;
      }
    }
    imps=imps->link;
  }
  return old;
}

implicantADT neg_cofactor(implicantADT imps,int x) {
  implicantADT newimp,old=NULL;
  int i,empty;

  while (imps) {
    if (x < NInputs) {
      if (imps->implicant[x]!='1') {
	newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
	strcpy(newimp->implicant,imps->implicant);
	strcpy(newimp->outputs,imps->outputs);
	newimp->implicant[x]='-';
	newimp->subset=0;
	newimp->link=old;
	old=newimp;
      }
    } else {
      empty=1;
      for (i=0;i<NOutputs;i++) 
	if ((i!=(x-NInputs)) && (imps->outputs[i]=='1')) empty=0;
      if (!empty) {
	newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
	strcpy(newimp->implicant,imps->implicant);
	strcpy(newimp->outputs,imps->outputs);
	newimp->outputs[x-NInputs]='1';
	newimp->outputs[NOutputs]='\0';
	newimp->subset=0;
	newimp->link=old;
	old=newimp;
      }
    }
    imps=imps->link;
  }
  return old;
}

int binate(implicantADT F,int x) {
  int phase;

  phase=(-1);
  if (x<NInputs) { 
    for (F=F;F;F=F->link)
      if ((F->implicant[x]=='0') && (phase==1)) return 1;
      else if ((F->implicant[x]=='1') && (phase==0)) return 1;
      else if (F->implicant[x]=='1') phase=1;
      else if (F->implicant[x]=='0') phase=0;
  } else
    for (F=F;F;F=F->link)
      if ((F->outputs[x-NInputs]=='0') && (phase==1)) return 1;
      else if ((F->outputs[x-NInputs]=='1') && (phase==0)) return 1;
      else if (F->outputs[x-NInputs]=='1') phase=1;
      else if (F->outputs[x-NInputs]=='0') phase=0;
  return 0;
}

int select(implicantADT F) {
  int x;
  
  //  write_implicants(stdout,F);
  for (x=0;x<(NInputs+NOutputs);x++)
    if (binate(F,x)) return x;
  return -1;
}

implicantADT pos(int x) {
  implicantADT newimp;
  int i;

  newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
  for (i=0;i<NInputs;i++)
    newimp->implicant[i]='-';
  if (x<NInputs)
    newimp->implicant[x]='1';
  newimp->implicant[NInputs]='\0';
  for (i=0;i<NOutputs;i++)
    if (x<NInputs)
      newimp->outputs[i]='1';
    else 
      newimp->outputs[i]='0';
  if (x>=NInputs)
    newimp->outputs[x-NInputs]='1';
  newimp->outputs[NOutputs]='\0';
  newimp->subset=0;
  newimp->link=NULL;
  return newimp;
}

implicantADT neg(int x) {
  implicantADT newimp;
  int i;

  newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
  for (i=0;i<NInputs;i++)
    newimp->implicant[i]='-';
  if (x<NInputs)
    newimp->implicant[x]='0';
  newimp->implicant[NInputs]='\0';
  for (i=0;i<NOutputs;i++)
    newimp->outputs[i]='1';
  if (x>=NInputs)
    newimp->outputs[x-NInputs]='0';
  newimp->outputs[NOutputs]='\0';
  newimp->subset=0;
  newimp->link=NULL;
  return newimp;
}

implicantADT or(implicantADT X,implicantADT Y) {
  implicantADT newimp,old=NULL;

  while (X) {
    newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
    strcpy(newimp->implicant,X->implicant);
    strcpy(newimp->outputs,X->outputs);
    newimp->subset=0;
    newimp->link=old;
    old=newimp;
    X=X->link;
  }
  while (Y) {
    newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
    strcpy(newimp->implicant,Y->implicant);
    strcpy(newimp->outputs,Y->outputs);
    newimp->subset=0;
    newimp->link=old;
    old=newimp;
    Y=Y->link;
  }
  return old;
}

implicantADT and(implicantADT X,implicantADT Y) {
  implicantADT newimp,old=NULL,curY;
  int add;
  int i;

  for (X=X;X;X=X->link) 
    for (curY=Y;curY;curY=curY->link) {
      add=1;
      for (i=0;i<NInputs;i++)
	if ((X->implicant[i]=='0') && (curY->implicant[i]=='1')) add=0;
	else if ((X->implicant[i]=='1') && (curY->implicant[i]=='0')) add=0;
      if (add) {
	add=0;
	for (i=0;i<NOutputs;i++)
	  if ((X->outputs[i]!='0') && (curY->outputs[i]=='1')) add=1;
	  else if ((X->outputs[i]=='1') && (curY->outputs[i]!='0')) add=1;
      }
      if (add) {
	newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
	strcpy(newimp->implicant,X->implicant);
	for (i=0;i<NInputs;i++)
	  if ((curY->implicant[i]=='0')||(curY->implicant[i]=='1'))
	    newimp->implicant[i]=curY->implicant[i];
	for (i=0;i<NOutputs;i++)
	  if (((X->outputs[i]!='0') && (curY->outputs[i]=='1'))|| 
	      ((X->outputs[i]=='1') && (curY->outputs[i]!='0'))) 
	    newimp->outputs[i]='1';
	  else
	    newimp->outputs[i]='0';
	/*
	for (i=0;i<NOutputs;i++)
	  if (curY->outputs[i]=='0')
	    newimp->outputs[i]='0';
	  else if ((curY->outputs[i]=='1') && (newimp->outputs[i]=='-'))
	    newimp->outputs[i]='1';
	*/
	/*
	  if (curY->outputs[i]=='1')
	    newimp->outputs[i]='1';
	*/
	newimp->subset=0;
	newimp->link=old;
	old=newimp;
      }
    }
  return old;
}

char andchar(char a,char b) {
  if ((a=='0') && (b=='0')) return '0';
  if ((a=='0') && (b=='-')) return '0';
  if ((a=='-') && (b=='0')) return '0';
  if ((a=='0') && (b=='1')) return 'X';
  if ((a=='1') && (b=='0')) return 'X';
  if ((a=='-') && (b=='-')) return '-';
  return '1';
}

char orchar(char a,char b) {
  if ((a=='0') && (b=='0')) return '0';
  if ((a=='1') && (b=='1')) return '1';
  return '-';
}

implicantADT consensus(implicantADT A,implicantADT B) {
  implicantADT newImp,old=NULL,curA,curB;
  char newimp[255];
  char newout[255];
  int x,i,add;

  for (curA=A;curA;curA=curA->link) 
    for (curB=B;curB;curB=curB->link) {
      for (x=0;x<NInputs;x++) {
	add=1;
	for (i=0;i<NInputs;i++) {
	  if (i==x) 
	    newimp[i]=orchar(curA->implicant[i],curB->implicant[i]);
	  else
	    newimp[i]=andchar(curA->implicant[i],curB->implicant[i]);
	  if (newimp[i]=='X') {
	    add=0;
	    break;
	  }
	}
	if (add) {
	  add=0;
	  for (i=0;i<NOutputs;i++) 
	    if ((curA->outputs[i]=='1') && (curB->outputs[i]=='1')) {
	      add=1;
	      newout[i]='1';
	    } else newout[i]='0';
	}
	if (add) {
	  newimp[NInputs]='\0';
	  newout[NOutputs]='\0';
	  newImp=(implicantADT)GetBlock(sizeof(*(newImp)));
	  strcpy(newImp->implicant,newimp);
	  strcpy(newImp->outputs,newout);
	  newImp->subset=0;
	  newImp->link=old;
	  old=newImp;
	}
      }
      add=1;
      for (i=0;i<NInputs;i++) {
	newimp[i]=andchar(curA->implicant[i],curB->implicant[i]);
	if (newimp[i]=='X') {
	  add=0;
	  break;
	}
      }
      if (add) {
	for (i=0;i<NOutputs;i++) 
	  if ((curA->outputs[i]=='1') || (curB->outputs[i]=='1')) 
	    newout[i]='1';
	  else newout[i]='0';
      }
      if (add) {
	newimp[NInputs]='\0';
	newout[NOutputs]='\0';
	newImp=(implicantADT)GetBlock(sizeof(*(newImp)));
	strcpy(newImp->implicant,newimp);
	strcpy(newImp->outputs,newout);
	newImp->subset=0;
	newImp->link=old;
	old=newImp;
      }
    }
  return old;
}

int subset(char imp1[255],char imp2[255],char outs1[255],char outs2[255]) {
  int i;

  for (i=0;i<NInputs;i++)
    if ((imp2[i]!='-') && (imp1[i]!=imp2[i])) return 0;
  /* PROBABLY WRONG */
  for (i=0;i<NOutputs;i++) {
    if ((outs1[i]=='1') && (outs2[i]!='1')) return 0;
    if ((outs1[i]=='-') && (outs2[i]!='-')) return 0;
  }
  return 1;
}

implicantADT abs(implicantADT F) {
  implicantADT newimp,curF,curF2;
  implicantADT old=NULL;
  int add,first,seenit;

  for (curF=F;curF;curF=curF->link) {
    add=1;
    first=1;
    seenit=0;
    for (curF2=F;curF2 && add && first;curF2=curF2->link) 
      if (!curF2->subset) {
	if (curF==curF2) seenit=1;
	else if ((strcmp(curF->implicant,curF2->implicant)!=0) ||
	    (strcmp(curF->outputs,curF2->outputs)!=0)) {
	  if ((curF != curF2) && 
	      (subset(curF->implicant,curF2->implicant,
		      curF->outputs,curF2->outputs))) add=0;
	} else {
	  if (!seenit) first=0;
	}
      }
    if (!add || !first)
      curF->subset=1;
    if (add && first) {
      newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
      strcpy(newimp->implicant,curF->implicant);
      strcpy(newimp->outputs,curF->outputs);
      newimp->subset=0;
      newimp->link=old;
      old=newimp;
    }
  }
  return old;
}

int unate(implicantADT F) {
  int i,phase;
  implicantADT imps;
  char dc[255];

  for (i=0;i<NInputs;i++) {
    phase=(-1);
    for (imps=F;imps;imps=imps->link)
      if ((imps->implicant[i]=='0') && (phase==1)) return 0;
      else if ((imps->implicant[i]=='1') && (phase==0)) return 0;
      else if (imps->implicant[i]=='0') phase=0;
      else if (imps->implicant[i]=='1') phase=1;
  }
  for (i=0;i<NOutputs;i++) 
    dc[i]='1';
  dc[NOutputs]='\0';
  for (imps=F;imps;imps=imps->link)
    if ((strcmp(imps->outputs,F->outputs)!=0) &&
	(strcmp(imps->outputs,dc)!=0)) return 0;

  return 1;
}

void delete_implicants(implicantADT imps) {
  implicantADT cur,next;

  cur=imps;
  while (cur) {
    next=cur->link;
    //free(cur);
    cur=next;
  }
}

implicantADT CS(implicantADT F,int depth) {
  implicantADT CS1,CS0,t,result,n,p;
  int x;

  if (unate(F)) return abs(F);
  x=select(F);
  if (x==(-1)) return abs(F);

  t=pos_cofactor(F,x);
  CS1=CS(t,depth+1);
  delete_implicants(t);

  t=neg_cofactor(F,x);
  CS0=CS(t,depth+1);
  delete_implicants(t);

  t=neg(x);
  n=or(t,CS1);
  delete_implicants(t);
  delete_implicants(CS1);

  t=pos(x);
  p=or(t,CS0);
  delete_implicants(t);
  delete_implicants(CS0);

  t=and(n,p);
  delete_implicants(n);
  delete_implicants(p);

  result=abs(t);
  delete_implicants(t);

  return result; 
}

implicantADT OutputComp(implicantADT F) {
  implicantADT newimp,old=NULL;
  char dc[MAXVAR];
  int i;

  for (i=0;i<NOutputs;i++)
    dc[i]='1';
  dc[NOutputs]='\0';
  for (F=F;F;F=F->link) 
    if (strcmp(dc,F->outputs)!=0) {
      newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
      strcpy(newimp->implicant,F->implicant);
      for (i=0;i<NOutputs;i++)
	if (F->outputs[i]=='0') 
	  newimp->outputs[i]='1';
	else 
	  newimp->outputs[i]='0';
      newimp->outputs[NOutputs]='\0';
      newimp->subset=0;
      newimp->link=old;
      old=newimp;
    }
  return old;
}

implicantADT DeMorgan(implicantADT F) {
  implicantADT newimp,old=NULL;
  char dc[MAXVAR];
  int i;
  
  for (i=0;i<NInputs;i++)
    dc[i]='-';
  dc[NInputs]='\0';
  for (i=0;i<NInputs;i++) 
    if (F->implicant[i]=='0') {
      newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
      strcpy(newimp->implicant,dc);
      strcpy(newimp->outputs,F->outputs);
      newimp->implicant[i]='1';
      newimp->subset=0;
      newimp->link=old;
      old=newimp;
    } else if (F->implicant[i]=='1') {
      newimp=(implicantADT)GetBlock(sizeof(*(newimp)));
      strcpy(newimp->implicant,dc);
      strcpy(newimp->outputs,F->outputs);
      newimp->implicant[i]='0';
      newimp->subset=0;
      newimp->link=old;
      old=newimp;
    }   
  newimp=OutputComp(F);
  if (newimp) {
    strcpy(newimp->implicant,dc);
    newimp->link=old;
    old=newimp;
  }

  return old;
}

int comp_select(implicantADT F) {
  int x;
  implicantADT cur;

  if (!F) return -1;
  //  write_implicants(stdout,F);

  for (cur=F;cur;cur=cur->link)
    for (x=0;x<NInputs;x++)
    //    if (binate(F,x)) return x;
      if ((cur->implicant[x]=='0')||(cur->implicant[x]=='1')) return x;
  for (x=0;x<NOutputs;x++)
    if (binate(F,x+NInputs)) return x+NInputs;
  return -1;
}

implicantADT complement(implicantADT F,int depth) {
  implicantADT F1,F0,t,result,n,p;
  int x;

  if (!F) return pos(-1); /* return true */
  if (F->link==NULL) return DeMorgan(F);
  x=comp_select(F);
  if (x==(-1)) return OutputComp(F);

  t=pos_cofactor(F,x);
  F1=complement(t,depth+1);
  delete_implicants(t);

  t=neg_cofactor(F,x);
  F0=complement(t,depth+1);
  delete_implicants(t);

  t=neg(x);
  n=and(t,F0);
  delete_implicants(t);
  delete_implicants(F0);

  t=pos(x);
  p=and(t,F1);
  delete_implicants(t);
  delete_implicants(F1);

  t=or(n,p);
  result=abs(t);
  delete_implicants(t);
  delete_implicants(n);
  delete_implicants(p);
  return result; 
}

bool find_primes() {
  implicantADT imps,imps2;
  FILE *infile,*outfile;

  infile=Fopen("input","r");
  imps=read_implicants(infile);
  fclose(infile);

  imps2=complement(imps,0);
  write_implicants(stdout,imps2);

  imps2=CS(imps,0);
  delete_implicants(imps);
  outfile=Fopen("output","w");
  write_implicants(outfile,imps2);
  fclose(outfile);
  delete_implicants(imps2);
  return TRUE;
}
