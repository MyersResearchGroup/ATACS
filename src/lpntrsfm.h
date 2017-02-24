#ifndef _lpntrsfm_h_
#define _lpntrsfm_h_

#include "struct.h"

bool transform(designADT design);
bool expand_design(designADT design);
void compress_design(designADT design);
void make_markkey(designADT design,int oldsize);
void check_constraints(designADT design);
void remove_constraints(designADT design);
void replace_constraints(designADT design);
void fix_constraints(designADT design, bool remove);
void print_size(designADT design);
bool xform0(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy, bool verbose);
bool xform1(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy, bool verbose);
bool xform2(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy, bool verbose);
bool xform3a(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose);
bool xform3b(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose);
bool xform3c(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose);
bool xform4a(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose);
bool xform4b(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose);
bool xform4c(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy, int tIndex,int nsignals,
             bool verbose);
bool xform5a(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy,
             int t1Index, int t2Index,int nsignals, bool verbose);
bool xform5b(eventADT *events,ruleADT **rules,markkeyADT *markkey,
             int nevents,int nplaces,int ndummy,
             int t1Index, int t2Index,int nsignals, bool verbose);
bool xform6(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy,
            int t1Index, int t2Index,int nsignals, bool verbose);
bool xform7(eventADT *events,ruleADT **rules,markkeyADT *markkey,
            int nevents,int nplaces,int ndummy,
            int t1Index, int t2Index,int nsignals, bool verbose);
bool expandRate(designADT design);
void createExpandEvent(eventADT *events,int nevents,int i,int curNewT,
                       ineqADT ineqEx,signalADT *signals,int nsignals,
                       int nineqs,bool newSig,int curNewSig,int numNewSig,
                       map<const string,int> &nameCntM,
                       map<const int,string> &sigNameM);
void createExpandMarkkey(markkeyADT *markkey,ruleADT **rules,int nrules,
                         int nevents,int t,int curNewT);
void adjustRate(eventADT *events,int nevents,ineqADT ineq,int curNewT,
                int rate,char *expr,designADT design,signalADT *signals,int curNewSig,
                int newSig,bool secondPass,map<const int,string> &sigNameM);
bool eqIneq(ineqADT i,ineqADT j);

#endif /* lpntrsfm.h */
