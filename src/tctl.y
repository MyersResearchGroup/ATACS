
%{
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "tctllex.c"
#include "symbolic.h"

#define YYPARSE_PARAM args
  
int yylex();
int yyerror(char*);

//Needed for Bison 2.2 
//extern char yytext[];
 
%}

%union {
  tmustmt *property;
  char *var;
  double number;
  int cIndex;
}

%type <property> statement

%token <var> VARIABLE 
%token <number> CONSTANT
%token GEQ GT LEQ LT
%left '|' '&' IMP EU AU 
%nonassoc '.' '~' TR FA EF AF EG AG
%left BOUNDED

%% 
statement: VARIABLE {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
	     if (md->allModelVars.find(string($1)) == md->allModelVars.end()) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       free($1);
	       YYABORT;
	     }
             BDD phi = md->getModelVarBDD($1);
             if (phi == md->getBDDMgr()->bddOne()) {
               $$ = var_tmustmt($1);
             }
             else {
               $$ = sl_tmustmt(phi, $1);
             }
             free($1);
#endif
           }
      |    TR {
#ifdef CUDD
             Cudd *mgr = ((lhpnModelData*) args)->getBDDMgr();
             $$ = sl_tmustmt(mgr->bddOne(), "true");
#endif
           }
      |    FA {
#ifdef CUDD
             Cudd *mgr = ((lhpnModelData*) args)->getBDDMgr();
             $$ = sl_tmustmt(mgr->bddZero(), "false");
#endif
           }
      |    '~' statement {
#ifdef CUDD
	$$ = not_tmustmt($2); 
#endif
	   }
      |    statement '|' statement { 
#ifdef CUDD
	$$ = or_tmustmt($1, $3); 
#endif
	   }
      |    statement '&' statement { 
#ifdef CUDD
	$$ = and_tmustmt($1, $3);
#endif
	   }
      |    statement IMP statement { 
#ifdef CUDD
	$$ = or_tmustmt(not_tmustmt($1), $3); 
#endif
 }
      |    statement EU statement { 
#ifdef CUDD
	$$ = EU_tmustmt($1, $3);
#endif
 }
      |    statement AU statement {
#ifdef CUDD
             $$ = AU_tmustmt($1, $3, (lhpnModelData*) args);
#endif
           }
      |    statement AU '_' CONSTANT  statement %prec BOUNDED {
#ifdef CUDD
             $$ = bAU_tmustmt($1, $5, $4, (lhpnModelData*) args);
#endif
           }
      |    EF statement {
#ifdef CUDD
             $$ = EF_tmustmt($2, (lhpnModelData*) args);
#endif
           }
      |    AG statement {
#ifdef CUDD
             $$ = AG_tmustmt($2, (lhpnModelData*) args);
#endif
           }
      |    AF statement {
#ifdef CUDD
             $$ = AF_tmustmt($2, (lhpnModelData*) args);
#endif
           }
      |    AF '_' CONSTANT statement %prec BOUNDED {
#ifdef CUDD
             $$ = bAF_tmustmt($4, $3, (lhpnModelData*) args);
#endif
           }
      |    EG statement {
#ifdef CUDD
             $$ = EG_tmustmt($2, (lhpnModelData*) args);
#endif
           }
      |    EG '_' CONSTANT statement %prec BOUNDED {
#ifdef CUDD
             $$ = bEG_tmustmt($4, $3, (lhpnModelData*) args);
#endif
           }
      |    '(' statement ')' { 
#ifdef CUDD
	$$ = $2; 
#endif
	   }
      |    VARIABLE '.' {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;        
             $<cIndex>$ = md->addClockVar($1);
             free ($1);
           } statement {
             $$ = sc_tmustmt($<cIndex>3, $4);
#endif
           }
      |    VARIABLE GEQ VARIABLE '+' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
	     if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     }
             int v2 = md->getVarIndex($3);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     }       
	     BDD phi = predStruct::addPred(v1, v2, $5);
             ostringstream desc;
             desc << $1 << " >= " << $3 << " + " << $5;
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
             free($3);
#endif
           }
      |    VARIABLE GEQ VARIABLE '-' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 
             int v2 = md->getVarIndex($3);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 
             BDD phi = predStruct::addPred(v1, v2, -1*$5);
             ostringstream desc;
             desc << $1 << " >= " << $3 << " + " << -1*$5;
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
             free($3);
#endif
           }
      |    VARIABLE GEQ CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     }        
	     int v2 = md->getVarIndex("x0");
             BDD phi = predStruct::addPred(v1, v2, $3);
             ostringstream desc;
             desc << $1 << " >= " << "x0" << " + " << $3;
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
#endif
           }
      |    VARIABLE GEQ VARIABLE {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 
             int v2 = md->getVarIndex($3);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 
             BDD phi = predStruct::addPred(v1, v2, 0);
             ostringstream desc;
             desc << $1 << " >= " << $3 << " + 0";
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
             free($3);
#endif
           }
      |    VARIABLE GT VARIABLE '+' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 
             int v2 = md->getVarIndex($3);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 
             BDD phi = predStruct::addPred(v2, v1, -1*$5);
             ostringstream desc;
             desc << $3 << " >= " << $1 << " + " << -1*$5;
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
             free($3);
#endif
           }
      |    VARIABLE GT VARIABLE '-' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 
             int v2 = md->getVarIndex($3);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 
             BDD phi = predStruct::addPred(v2, v1, $5);
             ostringstream desc;
             desc << $3 << " >= " << $1 << " + " << $5;
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
             free($3);
#endif
           }
      |    VARIABLE GT CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     }        
	     int v2 = md->getVarIndex("x0");
             BDD phi = predStruct::addPred(v2, v1, -1*$3);
             ostringstream desc;
             desc << "x0" << " >= " << $1 << " + " << -1*$3;
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
#endif
           }
      |    VARIABLE GT VARIABLE {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($1);
             int v2 = md->getVarIndex($3);
             BDD phi = predStruct::addPred(v2, v1, 0);
             ostringstream desc;
             desc << $3 << " >= " << $1 << " + 0";
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
             free($3);
#endif
           }
      |    VARIABLE LEQ VARIABLE '+' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($3);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     }          
	     int v2 = md->getVarIndex($1);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 
             BDD phi = predStruct::addPred(v1, v2, -1*$5);
             ostringstream desc;
             desc << $3 << " >= " << $1 << " + " << -1*$5;
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
             free($3);
#endif
           }
      |    VARIABLE LEQ VARIABLE '-' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($3);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     }    
	     int v2 = md->getVarIndex($1);
	     if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 
	     BDD phi = predStruct::addPred(v1, v2, $5);
             ostringstream desc;
             desc << $3 << " >= " << $1 << " + " << $5;
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
             free($3);
#endif
           }
      |    VARIABLE LEQ CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex("x0");
             int v2 = md->getVarIndex($1);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             BDD phi = predStruct::addPred(v1, v2, -1*$3);
             ostringstream desc;
             desc << "x0" << " >= " << $1 << " + " << -1*$3;
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
#endif
           }
      |    VARIABLE LEQ VARIABLE {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($3);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             int v2 = md->getVarIndex($1);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             BDD phi = predStruct::addPred(v1, v2, 0);
             ostringstream desc;
             desc << $3 << " >= " << $1 << " + 0";
             $$ = sl_tmustmt(phi, desc.str());
             free($1);
             free($3);
#endif
           }
      |    VARIABLE LT VARIABLE '+' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($3);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             int v2 = md->getVarIndex($1); 
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 			  
	     BDD phi = predStruct::addPred(v2, v1, $5);
             ostringstream desc;
             desc << $1 << " >= " << $3 << " + " << $5;
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
             free($3);
#endif
           }
      |    VARIABLE LT VARIABLE '-' CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($3); 
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 			  
	     int v2 = md->getVarIndex($1);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             BDD phi = predStruct::addPred(v2, v1, -1*$5);
             ostringstream desc;
             desc << $1 << " >= " << $3 << " + " << -1*$5;
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
             free($3);
#endif
           }
      |    VARIABLE LT CONSTANT {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex("x0");
             int v2 = md->getVarIndex($1);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             BDD phi = predStruct::addPred(v2, v1, $3);
             ostringstream desc;
             desc << $1 << " >= " << "x0" << " + " << $3;
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
#endif
           }
      |    VARIABLE LT VARIABLE {
#ifdef CUDD
             lhpnModelData* md = (lhpnModelData*) args;
             int v1 = md->getVarIndex($3);
             if (v1 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $3);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             int v2 = md->getVarIndex($1);
             if (v2 == -1) {
	       char msg[100];
	       sprintf(msg, "Unknown variable named \"%s\" in property.", $1);
	       yyerror(msg);
	       YYABORT;
	     } 			  
             BDD phi = predStruct::addPred(v2, v1, 0);
             ostringstream desc;
             desc << $1 << " >= " << $3 << " + 0";
             $$ = not_tmustmt(sl_tmustmt(phi, desc.str()));
             free($1);
             free($3);
#endif
           }
      ;

%%

int yyerror(char* s)
{
  printf("%s\n", s);
  fprintf(lg, "%s\n", s);
  YY_FLUSH_BUFFER;
  return 1; // return non-zero value to indicate failure.
}

