#include "struct.h"
#include "satconf.h"
#include "memaloc.h"
#include "def.h"
#include "loadg.h"
#include "lpntrsfm.h"
#include <vector>

using namespace std;
int clauses=0;


void equiv_print(FILE *out, int vars, int old)
{
/*    if(vars==57 | vars-1==57 | vars+1==57){ */
/*       printf("var 57 found\n"); */
/*       printf("vars=%d old=%d \n",vars, old); */
/*    } */
   
   fprintf(out,"%d %d 0\n",vars, vars-1);
   fprintf(out,"%d %d 0\n",vars, 0-old);
   fprintf(out,"%d %d %d 0\n",0-vars, 0-(vars-1),old);
   
   fprintf(out,"%d %d 0\n",vars+1, 0-(vars-1));
   fprintf(out,"%d %d 0\n",vars+1, old);
   fprintf(out,"%d %d %d 0\n",0-(vars+1), vars-1, 0-old);
}

int verify_cl(FILE *out, int vars, vector<vector<int> > part_verify)
{
/*    if(vars==57 | vars-1==57 | vars+1==57){ */
/*       printf("var 57 found in verify\n"); */
/*    } */

   vector<int> cons;
   for(unsigned int i=0;i<part_verify[0].size();i++){
      cons.push_back(part_verify[0][i]);
   }
   for(unsigned int i=1;i<part_verify.size();i++){
      if(part_verify[i].size()==1){
	 cons.push_back(0-part_verify[i][0]);
      }
      else{
	 vars++;
/* 	 if(vars==57 | vars-1==57 | vars+1==57){ */
/* 	    printf("var 57 found in verify\n"); */
/* 	 } */
	 cons.push_back(vars);
	 fprintf(out,"%d ",0-vars);
	 for(unsigned int j=0;j<part_verify[i].size();j++){
	    fprintf(out,"%d ",0-part_verify[i][j]);
	 }
	 fprintf(out,"0\n");
	 clauses++;
	 
	 for(unsigned int j=0;j<part_verify[i].size();j++){
	    fprintf(out,"%d %d 0\n",vars,part_verify[i][j]);
	    clauses++;
	 }
      }
   }

   vars++;
/*    if(vars==57 | vars-1==57 | vars+1==57){ */
/*       printf("var 57 found in verify\n"); */
/*    } */

/*    if(cons.size()<=1){ */
/*       printf("\n************\nOnly one verify condition\n******************\n"); */
/*    } */
   
   for(unsigned int i=0;i<cons.size();i++){
      fprintf(out,"%d %d 0\n", 0-vars, cons[i]);
      clauses++;
   }

   fprintf(out,"%d ",vars);
   for(unsigned int i=0;i<cons.size();i++){
      fprintf(out,"%d ",0-cons[i]);
   }
   fprintf(out,"0\n");
   clauses++;
   
   return vars;
   
}


bool satconf(char *filename)
{
  designADT designSPEC;
  designADT designIMPL;
	
  bool result=false;
  bool txfm;
  int oldsize;
    
  designSPEC=initialize_design();
  designIMPL=initialize_design();

  designSPEC->postproc=false;
  designIMPL->postproc=false;
  designSPEC->verbose=true;
  designIMPL->verbose=true;
  
  strcpy(designSPEC->filename,filename);
  strcpy(designIMPL->filename,filename);
  strcat(designIMPL->filename,"IMPL");
  char *cnffile=filename;
  strcat(cnffile,".cnf");

  FILE *out;
  out = fopen( cnffile, "w" );
  int vars=0;
  
  fprintf(out,"p cnf ");
  for(int i=0;i<25;i++){
     fprintf(out," ");
  }
  fprintf(out,"\n");
  
  
  if (load_lpn(designSPEC)) {
     if (load_lpn(designIMPL)) {

	/* Call your function here */
	oldsize= designSPEC->nevents + designSPEC->nplaces;
	txfm=expand_design(designSPEC);
	if(txfm==true){
	   make_markkey(designSPEC,oldsize);
	   printf("SPEC Design has been expanded\n");
	}
	
	oldsize= designIMPL->nevents + designIMPL->nplaces;
	txfm=expand_design(designIMPL);
	if(txfm==true){
	   make_markkey(designIMPL,oldsize);
	   printf("IMPL Design has been expanded\n");
	}

	vector<vector<int> > pre, post;
	pre.resize(designSPEC->nevents+designSPEC->nplaces);
	post.resize(designSPEC->nevents+designSPEC->nplaces);
		
	//	printf("Filenames : %s %s\n",designSPEC->filename,designIMPL->filename);
	//printf("Signals : %d \n Inp. signals : %d \n Inputs : %d \n Rules : %d \n Events : %d \n", designSPEC->nsignals, designSPEC->ninpsig, designSPEC->ninputs, designSPEC->nrules, designSPEC->nevents);
	result=true;

	vector<int> initial,initial_impl;
	vector<bool> pl_marked(designSPEC->nplaces,false);
	vector<bool> ipl_marked(designSPEC->nplaces,false);

/* 	for(int i=0;i<designSPEC->nsignals;i++){ */
/* 	   	printf("%d Signal is %s\n",i,designSPEC->signals[i]->name); */
/* 	} */
	
/* 	for(int i=0;i<designIMPL->nsignals;i++){ */
/* 	   	printf("%d Signal is %s\n",i,designIMPL->signals[i]->name); */
/* 	} */

	
/* 	for(int i=0; i<designSPEC->nevents-designSPEC->ndummy; i++){  */
/* 	   printf("Sig : %d i=%d  Name : %s \n",designSPEC->events[i]->signal, i, designSPEC->events[i]->event);  */
/* 	} */

/* 	for(int i=0; i<designIMPL->nevents-designIMPL->ndummy; i++){  */
/* 	   printf("Sig : %d i=%d  Name : %s \n",designIMPL->events[i]->signal, i, designIMPL->events[i]->event);  */
/* 	} */

	
	for(int i=0; i<designSPEC->nrules; i++){
	   //	   printf("[ %d , %d , %d, %d] ", i, designSPEC->markkey[i]->epsilon, designSPEC->markkey[i]->enabled, designSPEC->markkey[i]->enabling);
	   int t1=designSPEC->markkey[i]->enabling;
	   int t2=designSPEC->markkey[i]->enabled;
	   post[t1].push_back(t2);
	   pre[t2].push_back(t1);
	   if(designSPEC->markkey[i]->epsilon == 1 && t1>=designSPEC->nevents){
 	      if(t1<designSPEC->nevents){ 
 		 printf("Marking wrong\n"); 
 	      } 
	      pl_marked[t1-designSPEC->nevents]=true;
	      
	      initial.push_back(t1);
	   }
	   
	}

	printf("\n");

	vector<vector<int> > ipre, ipost;
	ipre.resize(designIMPL->nevents+designIMPL->nplaces);
	ipost.resize(designIMPL->nevents+designIMPL->nplaces);

	for(int i=0; i<designIMPL->nrules; i++){
	   //	   printf("[ %d , %d , %d, %d] ", i, designIMPL->markkey[i]->epsilon, designIMPL->markkey[i]->enabled, designIMPL->markkey[i]->enabling);
	   int t1=designIMPL->markkey[i]->enabling;
	   int t2=designIMPL->markkey[i]->enabled;
	   ipost[t1].push_back(t2);
	   ipre[t2].push_back(t1);
	   if(designIMPL->markkey[i]->epsilon == 1 && t1>=designIMPL->nevents){
 	      if(t1<designIMPL->nevents){ 
 		 printf("Marking wrong\n"); 
 	      }
	      ipl_marked[t1-designIMPL->nevents]=true;
	      initial_impl.push_back(t1);
	   }
	   
	}

	printf("\n");
	
/* 	for(unsigned int i=0;i<initial.size();i++){ */
/* 	   printf("INIT: %d \n",initial[i]); */
/* 	} */

/* 	for(unsigned int i=0;i<initial_impl.size();i++){ */
/* 	   printf("INIT_IMPL: %d \n",initial_impl[i]); */
/* 	}	 */

/* 	::stable_sort(initial.begin(), initial.end());//, cmp_var_stat); */

/* 	for(unsigned int i=0;i<initial.size();i++){ */
/* 	   printf("INIT: %d \n",initial[i]); */
/* 	} */

/* 	for(unsigned int i=0;i<initial_impl.size();i++){ */
/* 	   printf("INIT_IMPL: %d \n",initial_impl[i]); */
/* 	}	 */

	printf("For SPEC - Match case \n");
	int t1=0;
	char *signam=designSPEC->signals[t1]->name;
	for(int j=1;j<=designSPEC->nevents-designSPEC->ndummy;j++){
	   char *ev=designSPEC->events[j]->event;
	   //	   printf("Sig: %s Event: %s\n",signam,ev);
	   if(strstr(ev,signam)==NULL){
	      designSPEC->signals[t1]->event=j-1;
	      t1++;
	      // printf("Signal-Event Bound : %s - %d\n",signam,j-1);
	      if(t1<designSPEC->nsignals)
		 signam=designSPEC->signals[t1]->name;
	   }
	}
/* 	designSPEC->signals[t1]->event=designSPEC->nevents; */
/* 	printf("Signal-Event Bound : %s - %d\n",signam,designSPEC->nevents);	       */
	
	t1=0;
	printf("For IMPL \n");
	signam=designIMPL->signals[t1]->name;
	for(int j=1;j<=designIMPL->nevents-designIMPL->ndummy;j++){
	   char *ev=designIMPL->events[j]->event;
	   //	   printf("Sig: %s Event: %s\n",signam,ev);
	   if(strstr(ev,signam)==NULL){
	      designIMPL->signals[t1]->event= j-1;
	      t1++;
	      //   printf("Signal-Event Bound : %s - %d\n",signam,j-1);
	      if(t1<designIMPL->nsignals)
		 signam=designIMPL->signals[t1]->name;
	   }
	}
	
/* 	designIMPL->signals[t1]->event=designIMPL->nevents; */
/* 	printf("Signal-Event Bound : %s - %d\n",signam,designIMPL->nevents);	 */
	
	vector<vector<int> > spec_map,impl_map;

	
	for(int i=designSPEC->ninpsig;i<designSPEC->nsignals;i++){
	   char *signame=designSPEC->signals[i]->name;
	   vector<int> plus;
	   vector<int> minus;

	   printf("Signal name = %s\n",signame);
	   
	   for(int j=0; j<designIMPL->ninpsig; j++){
	      if(strcmp(signame,designIMPL->signals[j]->name)==0){
		  printf("SPEC %d (%s) = IMPL %d (%s)\n", i, signame, j, designIMPL->signals[j]->name);
		 //		 printf("Signal %d %d \n",designIMPL->signals[j]->riselower, designIMPL->signals[j]->fallupper);
		 int lower;
		 if(j==0) {
		    lower=1;
		 }
		 else{
		    lower=designIMPL->signals[j-1]->event+1;
		 }
		 int upper=designIMPL->signals[j]->event;
		 printf("Matching events (lower = %d , upper = %d) : ",lower,upper);
		 for(int k=lower;k<=upper;k++){
		    printf("%s ",designIMPL->events[k]->event);
		    if(strchr(designIMPL->events[k]->event,'+')!=NULL){
		       plus.push_back(k);
		       printf(" + ");
		    }
		    else{
		       minus.push_back(k);
		       printf(" - ");
		    }
		 }
		 printf("\n");
	      }
	      
	   }
	   spec_map.push_back(plus);
	   spec_map.push_back(minus);
	   
	}

/* 	for(unsigned int i=0;i<spec_map.size();i++){ */
/* 	   printf("SPEC_MAP %d : ",i); */
/* 	   for(unsigned int j=0;j<spec_map[i].size();j++){ */
/* 	      printf("%d ",spec_map[i][j]); */
/* 	   } */
/* 	   printf("\n"); */
/* 	} */
		
	
	for(int i=designIMPL->ninpsig;i<designIMPL->nsignals;i++){
	   char *signame=designIMPL->signals[i]->name;
	   vector<int> plus;
	   vector<int> minus;

	   for(int j=0; j<designSPEC->ninpsig; j++){
	      if(strcmp(signame,designSPEC->signals[j]->name)==0){
		 printf(" IMPL %d (%s) = SPEC %d (%s)\n", i, signame, j, designSPEC->signals[j]->name);
		 int lower;
		 if(j==0) {
		    lower=1;
		 }
		 else{
		    lower=designSPEC->signals[j-1]->event+1;
		 }
		 int upper=designSPEC->signals[j]->event;
		 //		 printf("Matching events : ");
		 printf("Matching events (lower = %d , upper = %d) : ",lower,upper);
		 for(int k=lower;k<=upper;k++){
		    printf("%s ",designSPEC->events[k]->event);
		    if(strchr(designSPEC->events[k]->event,'+')!=NULL){
		       plus.push_back(k);
		       printf(" + ");
		    }
		    else{
		       minus.push_back(k);
		       printf(" - ");
		    }
		 }
		 printf("\n");
	      }
	      
	   }
	   impl_map.push_back(plus);
	   impl_map.push_back(minus);
	}


	//	int cnt=1;
	vector<vector<int> >pl_map;
	vector<int> pl_same;
	
	pl_map.resize(designSPEC->nplaces);
	pl_same.resize(designSPEC->nplaces);

	
	printf("SPEC Places : \n");
	int cnt=1;
	for(int i=designSPEC->nevents; i<(designSPEC->nevents+designSPEC->nplaces);i++){
	   vector<int> cons(2);
	   // printf("i=%d  Name : %s \n", i, designSPEC->events[i]->event);
	   pl_map[i-(designSPEC->nevents)].push_back(cnt);
	   pl_map[i-(designSPEC->nevents)].push_back(cnt+designSPEC->nplaces);
	   //	   pl_same[i-(designSPEC->nevents)].push_back(cnt+designSPEC->nplaces+designSPEC->nplaces);
	   pl_same[i-(designSPEC->nevents)]=((2*cnt-1)+(2*designSPEC->nplaces));

	   if(pl_marked[i-designSPEC->nevents]){
	      fprintf(out,"%d 0\n",cnt);
	   }
	   else{
	      fprintf(out,"%d 0\n",0-cnt);
	   }
	   
	   
	   cons[0]=((2*cnt-1)+(2*designSPEC->nplaces));
	   cons[1]=(cnt+designSPEC->nplaces);
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);

	   cons[1]=(0-cnt);
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);

	   fprintf(out,"%d %d %d 0\n", 0-((2*cnt-1)+(2*designSPEC->nplaces)), 0-(cnt+designSPEC->nplaces), cnt);
	   
	   
	   cons[0]=((2*cnt)+(2*designSPEC->nplaces));
	   cons[1]=(0-(cnt+designSPEC->nplaces));
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);
	   cons[1]=(cnt);
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);

	   fprintf(out,"%d %d %d 0\n", 0-((2*cnt)+(2*designSPEC->nplaces)), (cnt+designSPEC->nplaces), 0-cnt);
	   
	   cnt++;
	   cons.clear();
	   clauses=clauses+7;
	}
	printf("\n");

	cnt--;
	printf("Highest got = %d \n",(2*cnt)+(2*designSPEC->nplaces));
	

	vector<vector<int> >ipl_map;
	vector<int> ipl_same;
	
	ipl_map.resize(designIMPL->nplaces);
	ipl_same.resize(designIMPL->nplaces);
	
	printf("IMPL Places : \n");
	int j=4*designSPEC->nplaces+1;
	int k=4*designSPEC->nplaces;
	
	for(int i=designIMPL->nevents; i<(designIMPL->nevents+designIMPL->nplaces);i++){
	   vector<int> cons(2);
	   //	   printf("i=%d  Name : %s \n", i, designIMPL->events[i]->event);
	   ipl_map[i-(designIMPL->nevents)].push_back(j);
	   ipl_map[i-(designIMPL->nevents)].push_back(j+designIMPL->nplaces);
	   ipl_same[i-(designIMPL->nevents)]=((2*(j-k)-1)+k+(2*designIMPL->nplaces));

	   if(ipl_marked[i-designIMPL->nevents]){
	      fprintf(out,"%d 0\n",j);
	   }
	   else{
	      fprintf(out,"%d 0\n",0-j);
	   }

	   cons[0]=((2*(j-k)-1)+k+(2*designIMPL->nplaces));
	   cons[1]=(j+designIMPL->nplaces);
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);
	   cons[1]=(0-j);
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);

	   fprintf(out,"%d %d %d 0\n", 0-((2*(j-k)-1)+k+(2*designIMPL->nplaces)), 0-(j+designIMPL->nplaces), j);
	   
	   
	   cons[0]=((2*(j-k))+k+(2*designIMPL->nplaces));
	   cons[1]=(0-(j+designIMPL->nplaces));
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);
	   cons[1]=(j);
	   fprintf(out,"%d %d 0\n", cons[0],cons[1]);

	   fprintf(out,"%d %d %d 0\n", 0-((2*(j-k))+k+(2*designIMPL->nplaces)), (j+designIMPL->nplaces), 0-j);
	   
	   cons.clear();
	   clauses=clauses+7;
	   j++;
	   
	}
	j--;
	printf("\n");
	printf("Highest got = %d \n",(2*(j-k))+k+(2*designIMPL->nplaces));

	printf("Please enter the number of iterations to UNROLL \n");
	
	int iter; // Needed for Unrolling

	scanf("%d",&iter);
	
	vector<int> verify;
	
	vars=(designSPEC->nplaces+designIMPL->nplaces)*4;

	while(iter>0){
	   
	   for(int i=0; i<designSPEC->nevents; i++){
	      if( (designSPEC->events[i]->type & OUT) > 0 && designSPEC->events[i]->dropped != true){
		 printf("Sig : %d i=%d  Name : %s \n",designSPEC->events[i]->signal, i, designSPEC->events[i]->event);

		 bool rise=false;
		 vector<vector<int> > part_verify;
		 part_verify.resize(1);

		 if(strchr(designSPEC->events[i]->event,'+')!=NULL){
		    printf("Rising signal \n");
		    rise=true;
		 }

		 for(unsigned int j1=0;j1<pre[i].size();j1++){
		       part_verify[0].push_back(pl_map[(pre[i][j1]-designSPEC->nevents)][0]);
		 }
		 
		 int smap=designSPEC->events[i]->signal - designSPEC->ninpsig;
		 if(!rise)smap=2*(smap)+1;
		 else smap=2*(smap);

		 vector<vector<int> > impl_cons;
	      
		 printf("val of smap : %d\n",smap);
		 for(unsigned int j1=0;j1<spec_map[smap].size();j1++){
		    vector<int> vnew;
		    int event=spec_map[smap][j1];
		    for(unsigned int k=0; k<ipre[event].size();k++){
		       vnew.push_back(ipl_map[(ipre[event][k]-designIMPL->nevents)][0]);
		    }
		    part_verify.push_back(vnew);
		 }
		 
		 for(unsigned int j1=0;j1<spec_map[smap].size();j1++){
		    vector<int> icons,icons1;
		    vector<int> cons;
		    vector<int> cons1;
		    int event=spec_map[smap][j1];
		    printf("IMPL Event : %d \n",event);
		    for(unsigned int k=0; k<ipre[event].size();k++){
		       printf(" %d ", ipre[event][k]);
		    
		       icons.push_back(ipl_map[(ipre[event][k]-designIMPL->nevents)][0]);
		       if(designIMPL->rules[event][ipre[event][k]]->ruletype!=NORULE && designIMPL->rules[ipre[event][k]][event]->ruletype!=NORULE){
			  //			  printf("Loop found\n");
		       }
		       else{
			  icons.push_back(0-ipl_map[(ipre[event][k]-designIMPL->nevents)][1]);
			  
			  
			  //pl_marked[pre[i][j]-designSPEC->nevents]=true;
			  icons1.push_back(ipl_same[(ipre[event][k]-designIMPL->nevents)]);
			  icons1.push_back(ipl_same[(ipre[event][k]-designIMPL->nevents)]+1);
			  
			  vars++;
			  ipl_map[(ipre[event][k]-designIMPL->nevents)][0]=ipl_map[(ipre[event][k]-designIMPL->nevents)][1];		 
			  ipl_map[(ipre[event][k]-designIMPL->nevents)][1]=vars;
			  vars++;
			  ipl_same[(ipre[event][k]-designIMPL->nevents)]=vars;
			  
			  equiv_print(out,vars,ipl_map[(ipre[event][k]-designIMPL->nevents)][0]);
			  vars++;
			  
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,ipl_map[(ipre[event][k]-designIMPL->nevents)][1]); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-ipl_map[(ipre[event][k]-designIMPL->nevents)][1]); */
			  
			  clauses=clauses+6;
		       }
		       
		    }
		    impl_cons.push_back(icons);
		    impl_cons.push_back(icons1);

		    for(unsigned int k=0; k<ipost[event].size();k++){
		       printf(" %d ", ipost[event][k]);

		       if(designIMPL->rules[event][ipost[event][k]]->ruletype!=NORULE && designIMPL->rules[ipost[event][k]][event]->ruletype!=NORULE){
			  //			  printf("Loop found\n");
		       }
		       else{
			  icons.push_back(0-ipl_map[(ipost[event][k]-designIMPL->nevents)][0]);
		       }
		       
		       icons.push_back(ipl_map[(ipost[event][k]-designIMPL->nevents)][1]); 
		       //pl_marked[post[i][j]-designSPEC->nevents]=true;
		       icons1.push_back(ipl_same[(ipost[event][k]-designIMPL->nevents)]);
		       icons1.push_back(ipl_same[(ipost[event][k]-designIMPL->nevents)]+1);
		    
		       vars++;
		       ipl_map[(ipost[event][k]-designIMPL->nevents)][0]=ipl_map[(ipost[event][k]-designIMPL->nevents)][1];		 
		       ipl_map[(ipost[event][k]-designIMPL->nevents)][1]=vars;
		       vars++;
		       ipl_same[(ipost[event][k]-designIMPL->nevents)]=vars;

		       equiv_print(out,vars,ipl_map[(ipost[event][k]-designIMPL->nevents)][0]);
		       vars++;
		    
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,ipl_map[(ipost[event][k]-designIMPL->nevents)][1]); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-ipl_map[(ipost[event][k]-designIMPL->nevents)][1]); */
		       clauses=clauses+6;
		    }
		    impl_cons.push_back(icons);
		    impl_cons.push_back(icons1);

		    // SPEC output pre-places firing
		    printf("Output signal : %s \nPlaces Pre : ", designSPEC->events[i]->event);
		    for(unsigned int j=0; j<pre[i].size();j++){
		       printf(" %d ", pre[i][j]);
		    
		       cons.push_back(pl_map[(pre[i][j]-designSPEC->nevents)][0]);
		       
		       if(designSPEC->rules[i][pre[i][j]]->ruletype!=NORULE && designSPEC->rules[pre[i][j]][i]->ruletype!=NORULE){
			  //			  printf("\nLoop found\n");
		       }
		       else{
			  cons.push_back(0-pl_map[(pre[i][j]-designSPEC->nevents)][1]);
		       
		       
			  pl_marked[pre[i][j]-designSPEC->nevents]=true;
			  cons1.push_back(pl_same[(pre[i][j]-designSPEC->nevents)]);
			  cons1.push_back(pl_same[(pre[i][j]-designSPEC->nevents)]+1);
			  
			  vars++;
			  pl_map[(pre[i][j]-designSPEC->nevents)][0]=pl_map[(pre[i][j]-designSPEC->nevents)][1];		 
			  pl_map[(pre[i][j]-designSPEC->nevents)][1]=vars;
			  vars++;
			  pl_same[(pre[i][j]-designSPEC->nevents)]=vars;
			  
			  equiv_print(out,vars,pl_map[(pre[i][j]-designSPEC->nevents)][0]);
			  vars++;
			  
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,pl_map[(pre[i][j]-designSPEC->nevents)][1]); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-pl_map[(pre[i][j]-designSPEC->nevents)][1]); */
			  clauses=clauses+6;
		       }
		       
		       //Conflict analysis - Not necessary
		       /* 		 if(post[pre[i][j]].size()==1){ */
		       /* 		    printf("No conflict\n"); */
		       /* 		 } */
		       /* 		 else{ */
		       /* 		    printf(" Conflict\n"); */
		       /* 		    for(int k=0;k<post[pre[i][j]].size();k++){ */
		       /* 		       if(post[pre[i][j]][k]!=i){ */
		       /* 			  printf("%d should remain same", post[pre[i][j]][k]); */
		       /* 			  for(int l=0;l<post[post[pre[i][j]][k]].size();l++){ */
		       /* 			     cons.push_back(pl_same[post[post[pre[i][j]][k]][l]-designSPEC->nevents]); */
		       /* 			  } */
		       /* 		       } */
		       /* 		    } */
		       /* 		 } */
		    
		    }
		    printf("\n");
		    //SPEC post places firing
		    printf("\nPlaces Post : ");
		    for(unsigned int j=0; j<post[i].size();j++){
		       printf("%d", post[i][j]);
		       if(designSPEC->rules[i][post[i][j]]->ruletype!=NORULE && designSPEC->rules[post[i][j]][i]->ruletype!=NORULE){
			  //			  printf("Loop found\n");
		       }
		       else{
			  cons.push_back(0-pl_map[(post[i][j]-designSPEC->nevents)][0]);
		       }
		       
		       cons.push_back(pl_map[(post[i][j]-designSPEC->nevents)][1]);
		       cons1.push_back(pl_same[(post[i][j]-designSPEC->nevents)]);
		       cons1.push_back(pl_same[(post[i][j]-designSPEC->nevents)]+1);
		    
		       vars++;
		       pl_map[(post[i][j]-designSPEC->nevents)][0]=pl_map[(post[i][j]-designSPEC->nevents)][1];		 
		       pl_map[(post[i][j]-designSPEC->nevents)][1]=vars;
		       vars++;
		       pl_same[(post[i][j]-designSPEC->nevents)]=vars;

		       equiv_print(out,vars,pl_map[(post[i][j]-designSPEC->nevents)][0]);
		       vars++;
		    
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,pl_map[(post[i][j]-designSPEC->nevents)][1]); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-pl_map[(post[i][j]-designSPEC->nevents)][1]); */
		       clauses=clauses+6;
		    }
		    printf("\n");
	      
		    // Convert SOP to POS
		    vector<int> cons2;
		    vars++;
		    cons2.push_back(vars);
		    for(unsigned int i1=0;i1<cons.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,cons[i1]);
		       clauses++;
		    }
		    for(unsigned int i1=0;i1<icons.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,icons[i1]);
		       clauses++;
		    }
		    fprintf(out,"%d ",vars);
		    for(unsigned int i1=0; i1<cons.size();i1++){
		       fprintf(out,"%d ", 0-cons[i1]);
		    }
		    for(unsigned int i1=0; i1<icons.size();i1++){
		       fprintf(out,"%d ", 0-icons[i1]);
		    }
		    fprintf(out,"0\n");
		    clauses++;
		 
		    vars++;
		    cons2.push_back(vars);
		    for(unsigned int i1=0;i1<cons1.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,cons1[i1]);
		       clauses++;
		    }
		    for(unsigned int i1=0;i1<icons1.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,icons1[i1]);
		       clauses++;
		    }

		    fprintf(out,"%d ",vars);
		    for(unsigned int i1=0; i1<cons1.size();i1++){
		       fprintf(out,"%d ", 0-cons1[i1]);
		    }
		    for(unsigned int i1=0; i1<icons1.size();i1++){
		       fprintf(out,"%d ", 0-icons1[i1]);
		    }

		    fprintf(out,"0\n");
		    clauses++;
		 
		    for(unsigned int i1=0;i1<cons2.size();i1++){
		       fprintf(out,"%d ",cons2[i1]);
		    }
		    fprintf(out,"0\n");
		    clauses++;
		    
		 }
		 printf("Verification Condition - vector: \n");
		 for(unsigned int i1=0;i1<part_verify.size();i1++){
		    for(unsigned int i2=0;i2<part_verify[i1].size();i2++){
		       printf("%d ", part_verify[i1][i2]);
		    }
		    printf("\n");
		 }
/* 		 if(part_verify.size()<=1){ */
/* 		    printf("\n**************\nVerify clause is only one\n************\n"); */
/* 		 } */
		 
		 vars=verify_cl(out,vars,part_verify);
		 verify.push_back(vars);
		 //	      printf("Current set of constrains are : \n");
		 //printf("\n");
	      }
	      
	      
	      
	   }
	

	   // CNF for the outputs of IMPL design
	   for(int i=0; i<designIMPL->nevents; i++){
	      //	   int temp=designIMPL->events[i]->signal;

	      if( (designIMPL->events[i]->type & OUT) > 0 && designIMPL->events[i]->dropped !=true){
		 printf("Sig : %d i=%d  Name : %s \n",designIMPL->events[i]->signal, i, designIMPL->events[i]->event);

		 bool rise=false;
		 vector<vector<int> > part_verify;
		 part_verify.resize(1);
		 
		 //	      vector<bool> pl_marked(designSPEC->nplaces,false);
		 //vector<bool> ipl_marked(designIMPL->nplaces,false);
	      
		 if(strchr(designIMPL->events[i]->event,'+')!=NULL){
		    printf("Rising signal \n");
		    rise=true;
		 }

		 for(unsigned int j1=0; j1<ipre[i].size();j1++){
		    part_verify[0].push_back(ipl_map[(ipre[i][j1]-designIMPL->nevents)][0]); 
		 }		   

		 int smap=designIMPL->events[i]->signal - designIMPL->ninpsig;
		 if(!rise)smap=2*(smap)+1;
		 else smap=2*(smap);

		 vector<vector<int> > spec_cons;
	      
		 printf("val of smap : %d\n",smap);

		 for(unsigned int j1=0;j1<impl_map[smap].size();j1++){
		    vector<int> vnew;
		    int event=impl_map[smap][j1];
		    for(unsigned int k=0; k<pre[event].size();k++){
		       vnew.push_back(pl_map[(pre[event][k]-designSPEC->nevents)][0]); 
		    }
		    part_verify.push_back(vnew);
		 }
				 
		 for(unsigned int j1=0;j1<impl_map[smap].size();j1++){
		    vector<int> scons,scons1;
		    vector<int> cons;
		    vector<int> cons1;
		    int event=impl_map[smap][j1];
		    printf("SPEC Event : %d \n",event);
		    for(unsigned int k=0; k<pre[event].size();k++){
		       printf(" %d ( %s )", pre[event][k],designSPEC->events[event]->event);
		    
		       scons.push_back(pl_map[(pre[event][k]-designSPEC->nevents)][0]); 
		       if(designSPEC->rules[event][pre[event][k]]->ruletype!=NORULE && designSPEC->rules[pre[event][k]][event]->ruletype!=NORULE){
			  //			  printf("\nLoop found\n");
		       }
		       else{
			  scons.push_back(0-pl_map[(pre[event][k]-designSPEC->nevents)][1]);
		       
		       
			  //pl_marked[pre[i][j]-designSPEC->nevents]=true;
			  scons1.push_back(pl_same[(pre[event][k]-designSPEC->nevents)]);
			  scons1.push_back(pl_same[(pre[event][k]-designSPEC->nevents)]+1);
			  
			  vars++;
			  pl_map[(pre[event][k]-designSPEC->nevents)][0]=pl_map[(pre[event][k]-designSPEC->nevents)][1];		 
			  pl_map[(pre[event][k]-designSPEC->nevents)][1]=vars;
			  vars++;
			  pl_same[(pre[event][k]-designSPEC->nevents)]=vars;
			  
			  equiv_print(out,vars,pl_map[(pre[event][k]-designSPEC->nevents)][0]);
			  vars++;
			  
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,pl_map[(pre[event][k]-designSPEC->nevents)][1]); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-pl_map[(pre[event][k]-designSPEC->nevents)][1]); */
			  clauses=clauses+6;
		       }
		       
		    }
		    printf("\n");
		    spec_cons.push_back(scons);
		    spec_cons.push_back(scons1);

		    for(unsigned int k=0; k<post[event].size();k++){
		       printf(" %d ", post[event][k]);
		    
		       if(designSPEC->rules[event][post[event][k]]->ruletype!=NORULE && designSPEC->rules[post[event][k]][event]->ruletype!=NORULE){
			  //			  printf("\nLoop found\n");
		       }
		       else{
			  scons.push_back(0-pl_map[(post[event][k]-designSPEC->nevents)][0]);
		       }
		       
		       scons.push_back(pl_map[(post[event][k]-designSPEC->nevents)][1]); 
		       //pl_marked[post[i][j]-designSPEC->nevents]=true;
		       scons1.push_back(pl_same[(post[event][k]-designSPEC->nevents)]);
		       scons1.push_back(pl_same[(post[event][k]-designSPEC->nevents)]+1);
		    
		       vars++;
		       pl_map[(post[event][k]-designSPEC->nevents)][0]=pl_map[(post[event][k]-designSPEC->nevents)][1];		 
		       pl_map[(post[event][k]-designSPEC->nevents)][1]=vars;
		       vars++;
		       pl_same[(post[event][k]-designSPEC->nevents)]=vars;

		       equiv_print(out,vars,pl_map[(post[event][k]-designSPEC->nevents)][0]);
		       vars++;
		    
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,pl_map[(post[event][k]-designSPEC->nevents)][1]); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-pl_map[(post[event][k]-designSPEC->nevents)][1]); */
		       clauses=clauses+6;

		    }
		    printf("\n");
		    spec_cons.push_back(scons);
		    spec_cons.push_back(scons1);

		    printf("Output signal : %s \nPlaces Pre : ", designIMPL->events[i]->event);
		 
	      
		    for(unsigned int j=0; j<ipre[i].size();j++){
		       printf(" %d ", ipre[i][j]);
		    
		       cons.push_back(ipl_map[(ipre[i][j]-designIMPL->nevents)][0]);
		       if(designIMPL->rules[i][ipre[i][j]]->ruletype!=NORULE && designIMPL->rules[ipre[i][j]][i]->ruletype!=NORULE){
			  //			  printf("\nLoop found\n");
		       }
		       else{
			  cons.push_back(0-ipl_map[(ipre[i][j]-designIMPL->nevents)][1]);
		       
		       
			  //ipl_marked[ipre[i][j]-designIMPL->nevents]=true;
			  cons1.push_back(ipl_same[(ipre[i][j]-designIMPL->nevents)]);
			  cons1.push_back(ipl_same[(ipre[i][j]-designIMPL->nevents)]+1);
			  
			  vars++;
			  ipl_map[(ipre[i][j]-designIMPL->nevents)][0]=ipl_map[(ipre[i][j]-designIMPL->nevents)][1];		 
			  ipl_map[(ipre[i][j]-designIMPL->nevents)][1]=vars;
			  vars++;
			  ipl_same[(ipre[i][j]-designIMPL->nevents)]=vars;
			  
			  equiv_print(out,vars,ipl_map[(ipre[i][j]-designIMPL->nevents)][0]);
			  vars++;
		    
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
			  /* 		    fprintf(out,"%d %d 0\n",0-vars,ipl_map[(ipre[i][j]-designIMPL->nevents)][1]); */
			  /* 		    fprintf(out,"%d %d 0\n",vars,0-ipl_map[(ipre[i][j]-designIMPL->nevents)][1]); */
			  clauses=clauses+6;
		       }
		       
		    }
		    printf("\n");
		    printf("\nPlaces Post : ");
		    for(unsigned int j=0; j<ipost[i].size();j++){
		       printf("%d", ipost[i][j]);
		       if(designIMPL->rules[i][ipost[i][j]]->ruletype!=NORULE && designIMPL->rules[ipost[i][j]][i]->ruletype!=NORULE){
			  //			  printf("\nLoop found\n");
		       }
		       else{
			  cons.push_back(0-ipl_map[(ipost[i][j]-designIMPL->nevents)][0]); 
		       }
		       
		       cons.push_back(ipl_map[(ipost[i][j]-designIMPL->nevents)][1]);
		       cons1.push_back(ipl_same[(ipost[i][j]-designIMPL->nevents)]);
		       cons1.push_back(ipl_same[(ipost[i][j]-designIMPL->nevents)]+1);
		    
		       vars++;
		       ipl_map[(ipost[i][j]-designIMPL->nevents)][0]=ipl_map[(ipost[i][j]-designIMPL->nevents)][1];		 
		       ipl_map[(ipost[i][j]-designIMPL->nevents)][1]=vars;
		       vars++;
		       ipl_same[(ipost[i][j]-designIMPL->nevents)]=vars;

		       equiv_print(out,vars,ipl_map[(ipost[i][j]-designIMPL->nevents)][0]);
		       vars++;
		    
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,vars-1); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-(vars-1)); */
		       /* 		    fprintf(out,"%d %d 0\n",0-vars,ipl_map[(ipost[i][j]-designIMPL->nevents)][1]); */
		       /* 		    fprintf(out,"%d %d 0\n",vars,0-ipl_map[(ipost[i][j]-designIMPL->nevents)][1]); */
		       clauses=clauses+6;
		    
		    }
		    printf("\n");

		    // Convert SOP to POS
		    vector<int> cons2;
		    vars++;
		    cons2.push_back(vars);
		    for(unsigned int i1=0;i1<cons.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,cons[i1]);
		       clauses++;
		    }
		    for(unsigned int i1=0;i1<scons.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,scons[i1]);
		       clauses++;
		    }
		    fprintf(out,"%d ",vars);
		    for(unsigned int i1=0; i1<cons.size();i1++){
		       fprintf(out,"%d ", 0-cons[i1]);
		    }
		    for(unsigned int i1=0; i1<scons.size();i1++){
		       fprintf(out,"%d ", 0-scons[i1]);
		    }
		    fprintf(out,"0\n");
		    clauses++;
		 
		    vars++;
		    cons2.push_back(vars);
		    for(unsigned int i1=0;i1<cons1.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,cons1[i1]);
		       clauses++;
		    }
		    for(unsigned int i1=0;i1<scons1.size();i1++){
		       fprintf(out,"%d %d 0\n",0-vars,scons1[i1]);
		       clauses++;
		    }

		    fprintf(out,"%d ",vars);
		    for(unsigned int i1=0; i1<cons1.size();i1++){
		       fprintf(out,"%d ", 0-cons1[i1]);
		    }
		    for(unsigned int i1=0; i1<scons1.size();i1++){
		       fprintf(out,"%d ", 0-scons1[i1]);
		    }

		    fprintf(out,"0\n");
		    clauses++;
		 
		    for(unsigned int i1=0;i1<cons2.size();i1++){
		       fprintf(out,"%d ",cons2[i1]);
		    }
		    fprintf(out,"0\n");
		    clauses++;
		 }
		 printf("Verification Condition - vector: \n");
		 for(unsigned int i1=0;i1<part_verify.size();i1++){
		    for(unsigned int i2=0;i2<part_verify[i1].size();i2++){
		       printf("%d ", part_verify[i1][i2]);
		    }
		    printf("\n");
		 }
/* 		 if(part_verify.size()<=1){ */
/* 		    printf("\n**************\nVerify clause is only one\n************\n"); */
/* 		 } */

		 vars=verify_cl(out,vars,part_verify);
		 verify.push_back(vars);
		 
	      }
	   
	   }

/* 	   for(int i=0; i< designSPEC->nevents; i++){ */
/* 	      for(int j=0 ; j<designSPEC->nevents; j++){ */
/* 		 if(designSPEC->rules[i][j]->epsilon != -1) */
/* 		    printf("[ %d , %d ] = %d , %d , %s \n", i, j, designSPEC->rules[i][j]->epsilon, designSPEC->rules[i][j]->marking, designSPEC->rules[i][j]->expr); */
/* 	      } */
/* 	   } */
	
/* 	   for(int i=0; i<designIMPL->nevents+designIMPL->nplaces; i++){  */
/* 	      printf("Sig : %d i=%d  Name : %s \n",designIMPL->events[i]->signal, i, designIMPL->events[i]->event);  */
/* 	   } */
	   iter--;
	   
	}

	for(unsigned int i=0;i<verify.size();i++){
	   fprintf(out,"%d ",verify[i]);
	}
	fprintf(out,"0\n");
	clauses++;
	
	
     }  
  }
  

/* 	for(int i=0; i<post.size();i++){ */
/* 	   printf("%d:",i); */
/* 	   for(int j=0;j<post[i].size();j++){ */
/* 	      printf(" %d", post[i][j]); */
/* 	   } */
/* 	   printf("\n"); */
/* 	} */

/* 	for(int i=0; i<pre.size();i++){ */
/* 	   printf("%d:",i); */
/* 	   for(int j=0;j<pre[i].size();j++){ */
/* 	      printf(" %d", pre[i][j]); */
/* 	   } */
/* 	   printf("\n"); */
/* 	} */
  
  //  out=freopen(cnffile,"a+",out);
  //  fseek(out,0,SEEK_SET);
  rewind(out);
  fprintf(out,"p cnf %d %d\n",vars,clauses);
  printf("p cnf %d %d\n",vars,clauses);

  printf("\n*************************************************\n");
  printf("CNF file : %s \n",cnffile);
  printf("\n*************************************************\n");
  
  
  fclose(out);
  //delete_design(designSPEC);
  //delete_design(designIMPL);
  return result;
}

