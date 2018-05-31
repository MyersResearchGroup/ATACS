/*

 Concurrency Reduction In Timed Integrated Circuits (CRITIC)

   Copyright (C) 2000-2002, by Eric R. Peskin
   Center for Asynchronous Circuit and System Design
   University of Utah

   Permission to use, copy, modify and/or distribute, but not sell,
   this software and its documentation for any purpose is hereby
   granted without fee, subject to the following terms and conditions:

   1.  The above copyright notice and this permission notice must
   appear in all copies of the software and related documentation.

   2.  The name of University Of Utah may not be used in advertising
   or publicity pertaining to distribution of the software without the
   specific, prior written permission of University Of Utah and Eric
   R. Peskin

   3.  This software may not be called "CRITIC" if it has been modified
   in any way, without the specific prior written permission of Eric
   R. Peskin.

   Questions?  Comments?  mailto:peskin@vlsigroup.ece.utah.edu
   Bug reports?  http://www.async.ece.utah.edu/cgi-bin/atacs

*/

#include <errno.h>
#include "hse.hpp"
#include "cpu_time_tracker.h"
//#include <iomanip>
#include <algorithm>

// *********************************************************************
// Generates an nevent x nevent boolean array and fills all entries.
// *********************************************************************
void make_concur_array(designADT design) {

  bool check_vec[design->nevents+1];

  // All elements in the concurrency array should be initialized when
  // this function is entered.
  // Initialize the check vector
  for (int i=0;i<design->nevents;i++)
    check_vec[i] = false;

  // Go through all states inefficiently
  for (int i=0;i<PRIME;i++) {
    for (stateADT cur_state1=design->state_table[i];
	 cur_state1 != NULL && cur_state1->state != NULL;
	 cur_state1=cur_state1->link) {
      
      // Go through all successor states
      for (statelistADT cur_state2=cur_state1->succ;
	   (cur_state2 != NULL && cur_state2->stateptr->state != NULL);
	   (cur_state2=cur_state2->next))
	check_vec[cur_state2->enabled] = true;

      // Now update the concurrency matrix
      for (int j=0;j<design->nevents;j++) {
	if (check_vec[j] == true) {
	  
	  for (int k=0;k<design->nevents;k++) {
	    if ((check_vec[k] == true) && (j != k)) {
	      
	      // We have concurrency
	      design->rules[j][k]->concur = true;
	      design->rules[k][j]->concur = true;
	    }
	  }
	}
      }
      
      // Reninitialize the check vector
      for (int l=0;l<design->nevents;l++)
	check_vec[l] = false;
    }
  } 
}

void explore(stateADT state, vector<int> count){
  for(statelistADT s(state->succ);  s && s->stateptr->state;  s=s->next){
    if(!s->iteration){
      s->iteration = ++count[s->enabled];
      explore(s->stateptr, count);
      count[s->enabled]--;
    }
  }
}

void annotate(designADT design){
  if(!design->startstate){
    throw("NULL start state!");
  }
  string startstate(design->startstate);
  markingADT startmarking(find_initial_marking(design->events,
					       design->rules,
					       design->markkey,
					       design->nevents,
					       design->nrules,
					       design->ninpsig,
					       design->nsignals,
					       design->startstate,FALSE));
  stateADT initial(NULL);
  int bin(hashpjw(design->startstate));
  for (stateADT e(design->state_table[bin]);e->link && !initial;e = e->link){
    string state(SVAL(e->state,design->nsignals));
    if (startstate == state &&
	((!startmarking) || (strcmp(e->marking,startmarking->marking)==0))) {
      initial = e;
    }
  }
  free(startmarking);
  vector<int> count(design->nevents,0);
  explore(initial,count);
}

const string int2string(int num, int base=10){
  string retval("");
  char newChar('0');
  while(num){
    int digit(num % base);
    if(digit < 10){
      newChar = '0' + digit;
    }
    else{
      newChar = 'A' + digit - 10;
    }
    retval = newChar + retval;
    num /= base;
  }
  return retval;
}

string::size_type suffixPos(const string& haystack, const string& needle){
  if(needle.size() > haystack.size()){
    return 0;
  }
  string::size_type pos(haystack.size() - needle.size());
  string found(haystack,pos);
  if(needle != found){
    return 0;
  }
  return pos;
}

string Channel(const string& signal){
  string::size_type pos =
    suffixPos(signal, RECEIVE_SUFFIX) + suffixPos(signal, SEND_SUFFIX);
  return string(signal,0,pos);
}

int boundary(const char* action){
  int length(strcspn(action,"+-")); // length of the signal
  int channelLength(Channel(string(action,length)).length());
  if(channelLength){
    return channelLength;
  }
  return length;
}

string prefix(const char* action){
  return string(action,0,boundary(action));
}

string suffix(const char* action){
  return string(action + boundary(action));
}

bool hasChannels(designADT design){  // Does this design have channels?
  loggedStream messages(cout, lg);
  if(design->verbose){
    messages << "Searching for channels... ";
  }
  for(int s(0);  s<design->nsignals;  s++){
    signalADT signal(design->signals[s]);
    if(!signal){
      throw("Signal is NULL!");
    }
    char* name(signal->name);
    if(!name){
      throw("Signal name is NULL!");
    }
    string Signal(name);
    if(!Channel(Signal).empty()){
      if(design->verbose){
	messages << "found channels." << endl;
      }
      return true;
    }
  }
  if(design->verbose){
    messages << "none found." << endl;
  }
  return false;
}

string Not(const string& exp){  // logical negation of expression
  if(exp.empty()){  // Original expression is the constant "true".
    return "false";
  }
  if(exp.find_first_of("|&") == string::npos){  // just a literal
    if('~' == exp[0]){  // already has at least one leading tilde
      return string(exp, 1, string::npos);  // Strip off the leading tilde.
    }
    else{  // positive phase
      return '~' + exp;  // Prepend a leading '~'.
    }
  }
  else{  // complex expression
    return "~(" + exp + ")";
  }
}

string And(const string& exp1, const string& exp2){  // AND of two expressions
  if(exp1.empty()){
    return exp2;
  }
  if(exp2.empty() || exp2==exp1){
    return exp1;
  }
  return('(' + exp1 + ")&(" + exp2 + ')');
}

bool subset(const string& x, const string& y){  // x has a subset of y's ones
  if(y.find('0', x.size())!=string::npos){
    return false;
  }
  for(string::size_type i(0);  i<x.size();  i++){
    if(x[i] & ~(y[i]|'2')){
      return false;
    }
  }
  return true;
}

/* The following strips out information about which candidates were redundant,
   leaving only information about which candidates were actually added:  */
string added(const string& x){
  string result;
  for(string::size_type i(0);  i<x.size();  i++){
    result += x[i]&'2';
  }
  return result;
}

void reload(designADT design, const string& basename, const string& goodName,
	    char fromType=TEL){
  if(goodName.size() >= FILENAMESIZE || basename.size() >= FILENAMESIZE){
    throw("file name too long!");
  }
  strcpy(design->filename, goodName.c_str());
  design->fromTEL=false;
  design->fromVHDL=true;
  if(!process_command(LOAD, fromType, design, NULL, true, design->si)){
    throw("Could not load event rule system for solution.");
  }
  strcpy(design->filename, basename.c_str());
}

// Expand communication actions to handshaking signals:
bool handshaking_expansion(char command, designADT design){
  loggedStream messages(cout, lg);
  loggedStream errors(cerr, lg);
  try{
    cpu_time_tracker time;
    time.touch();
    bool mustExpand(!(design->status & EXPANDED) &&
		    design->fromVHDL &&
		    hasChannels(design));
    bool mustReduce((mustExpand || design->reduction) &&
		    !(design->status & REDUCED));
    if(mustExpand || mustReduce){
      if(!design ||
	 design->nevents && !design->events ||
	 design->nrules && !design->rules ||
	 design->nsignals && !design->signals){
	throw("Attempted to expand NULL-pointer design!");
      }
      Rule::gateLower = design->mingatedelay;
      Rule::gateUpper = design->maxgatedelay;
      string basename(design->filename);
      if(mustExpand){
	int oldStatus(design->status);
	bool old_untimed(design->untimed);
	bool old_compress(design->compress);
	bool old_heuristic(design->heuristic);
	bool old_orbits(design->orbits);
	bool old_geometric(design->geometric);
	bool old_bag(design->bag);
	bool old_bap(design->bap);
	bool old_pomaxdiff(design->pomaxdiff);
	bool old_poapprox(design->poapprox);
	bool old_posets(design->posets);
	design->untimed = true;
	design->compress =
	  design->heuristic =
	  design->orbits =
	  design->geometric =
	  design->bag =
	  design->bap =
	  design->pomaxdiff =
	  design->poapprox =
	  design->posets = false;
	if(!find_reduced_state_graph(VERIFY,VERATACS,design)){
	  throw("Channel-level verification failed!");
	}
	design->status=oldStatus;
	design->untimed = old_untimed;
	design->compress = old_compress;
	design->heuristic = old_heuristic;
	design->orbits = old_orbits;
	design->geometric = old_geometric;
	design->bag = old_bag;
	design->bap = old_bap;
	design->pomaxdiff = old_pomaxdiff;
	design->poapprox = old_poapprox;
	design->posets = old_posets;
	sparseTEL aTEL(design);
	aTEL.store(basename+"_my");  // Write the new TEL to <name>_my.tel.
	aTEL.passify();
	aTEL.interleave();
	//aTEL.fatten();
	reload(design, basename, aTEL.lastStored);
	design->status |= EXPANDED;
      }
      if(mustReduce){
	int oldStatus(design->status);
	if(!find_reduced_state_graph(VERIFY,VERATACS,design)){
	  throw("Verification on most-concurrent starting point failed!");
	}
	design->status=oldStatus;
	sparseTEL aTEL(design, true);
	list<Candidate> missing(aTEL.candidates());
	list<Candidate>::iterator boundary(missing.end());
	prob_vector max(1,DBL_MAX);
	string tableName(basename + ".table");
	ofstream table(tableName.c_str(),ios::app);
	if(!table){
	  throw("Could not open " + tableName + " for appending: " +
		strerror(errno));
	}
	table << "\\filters{";
	if(design->pruning.not_first){
	  table << NOT_FIRST;
	}
	table << "}{";
	if(design->pruning.preserve){
	  table << PRESERVE;
	}
	table << "}{";
	if(design->pruning.expensive){
	  table << EXPENSIVE;
	}
	table << "}{";
	if(design->pruning.ordered){
	  table << ORDERED;
	}
	table << "}{";
	if(design->pruning.subset){
	  table << SUBSET;
	}
	table << "}{";
	if(design->pruning.unsafe){
	  table << UNSAFE;
	}
	table << "}";
	table.flush();
	aTEL.enumerate(missing.begin(),missing.begin(),boundary,"",
		       Possibility(max,basename+"_HSE_CURRENT",""));
	time.mark();
	table << "\\results{"
	  //	      << setprecision(2) << setiosflags(ios::fixed)
	      << time.resource_time().first << "}{" // cpu time
	      << distance(missing.begin(),boundary) << "}{" // levels
	      << aTEL.simulations << "}{" // TELs
	      << aTEL.attempts << "}{" // leaves
	      << aTEL.solutions << "}{"  // PRSs
	  //	      << setprecision(0)
	  ;
	if(aTEL.good.empty()){
	  throw("No solution found.");
	}
	table << aTEL.good.begin()->Literals() << "}{" // literals
	      << aTEL.good.begin()->Transistors() << "}{" //transistors
	      << aTEL.good.rbegin()->ave << "}{"  // max period
	      << aTEL.good.begin()->ave << "}" << endl; // min period
	messages << "The solutions are:  " << endl
	  //<< setprecision(3)
		 << aTEL.good;
	reload(design, basename, aTEL.best.name, RSG);
	design->status |= REDUCED;
      }
    }
    return true;  // success!
  }
  catch(bad_alloc){
    errors << "ERROR: Insufficient memory for handshaking expansion." << endl;
  }
  catch(string message){
    errors << "ERROR: " << message << endl;
  }
  catch(const char* message){
    errors << "ERROR: " << message << endl;
  }
  return(false);
}

/// BEGIN class Rule /////////////////////////////////////////////////////////
int Rule::gateLower = 0;
int Rule::gateUpper = INFIN;
/* Each constructor creates a new rule from *Enabling to *Enabled, and
   inserts the new rule into the fan-out of *Enabling and the fan-in of
   *Enabled:  */
Rule::Rule(Event* Enabling, Event* Enabled, int Epsilon, const string& Exp,bool fromVisit):
  addedByVisit(fromVisit),
  enabling(Enabling),  // a pointer to the enabling Event
  enabled(Enabled),  // a pointer to the enabled Event
  epsilon(Epsilon),  // the epsilon value on this rule
  lower(gateLower),  // the lower timing bound on this Rule
  upper(gateUpper),  // the upper timing bound on this Rule
  type(SPECIFIED),  // the ruletype from ruleADT (see struct.h)
  expression(Exp){  // the expression on the Rule, withOUT brackets
  enabling->Fanout.insert(this);
  if(fromVisit){
    enabling->added.insert(this);
  }
  enabled->Fanin.insert(this);
  if(enabled->dumb()){
    clearTiming();
  }
}

Rule::Rule(Event*Enabling, Event*Enabled, ruleADT Rule)://use info from ruleADT
  addedByVisit(false),
  enabling(Enabling),  // a pointer to the enabling Event
  enabled(Enabled),  // a pointer to the enabled Event
  epsilon(Rule->epsilon),  // the epsilon value on this rule
  lower(Rule->lower),  // the lower timing bound on this Rule
  upper(Rule->upper),  // the upper timing bound on this Rule
  type(Rule->ruletype),  // the ruletype from ruleADT (see struct.h)
  expression(""){  // the expression on the Rule, withOUT brackets
  if(Rule->expr &&
     strcmp(Rule->expr, "[true]") &&
     strcmp(Rule->expr, "[(true)]")){
    expression = string(Rule->expr, 1, strlen(Rule->expr)-2);
  }
  if(type & ORDERING){
    enabling->outConstraints.insert(this);
    enabled->inConstraints.insert(this);
  }
  else{
    enabling->Fanout.insert(this);
    enabled->Fanin.insert(this);
  }
}

/* The destructor removes the rule from: the fan-out of its enabling
   event, the fan-in of its enabled event, and existence:  */
Rule::~Rule(){
  if(type & ORDERING){
    enabling->outConstraints.erase(this);
    enabled->inConstraints.erase(this);
  }
  else{
    enabling->Fanout.erase(this);
    enabled->Fanin.erase(this);
  }
}

/* The following functions deal with the expression of the Rule.  Each
   expression is represented as a string withOUT the enclosing brackets.*/

void Rule::copyExpression(Rule* that){  // Copy expression from that Rule
  expression = that->expression;
}

const string& Rule::Expression(void) const{  // the expression on the Rule
  return expression;
}

void Rule::clearExpression(void){  // removes the expression from the Rule
  expression = "";
}

void Rule::pushExpression(const string & exp){  // My expression &&= exp
  expression = And(expression, exp);
}

bool Rule::uses(const string & signal) const{  // Do I use this signal?
  string needle('(' + signal + ')');
  string prefix(Channel(signal));
  string myPrefix(Channel(expression));
  return expression.find(needle)!=string::npos || prefix == myPrefix;
}

// The following functions deal with the Events associated with the Rule:

Event* Rule::Enabling(void) const{  // Return a pointer to the enabling Event.
  return enabling;
}

Event* Rule::Enabled(void) const{  // Return a pointer to the enabled Event.
  return enabled;
}

void Rule::setEnabling(Event*New){//Change enabling event. Update event Fanouts
  enabling->Fanout.erase(this);
  enabling = New;
  enabling->Fanout.insert(this);
}

void Rule::setEnabled(Event* New){//Change enabled event.  Update event Fanins.
  enabled->Fanin.erase(this);
  enabled = New;
  enabled->Fanin.insert(this);
}

void Rule::clearTiming(void){
  lower = upper = 0;
}

void Rule::grabTiming(Set fanin){
  clearTiming();
  for(Rule::Set::iterator Rin(fanin.begin());Rin!=fanin.end();Rin++){
    lower = max(lower,(*Rin)->lower);
    upper = max(upper,(*Rin)->upper);
    (*Rin)->clearTiming();
  }
}

int Rule::Epsilon(void) const{  // Return the epsilon value on this rule.
  return epsilon;
}

void Rule::setEpsilon(const int value){  // Set epsilon value of this rule.
  epsilon = value;
}

bool Rule::operator<(const Rule& that) const{
  return *enabling < *(that.enabling)
    || enabling == that.enabling && *enabled < *(that.enabled);
}

ostream& operator<<(ostream& out, const Rule& rule){  // Output the Rule.
  out << rule.enabling << rule.enabled;
  if(!rule.expression.empty()){
    out << '[' << rule.expression << ']';
    if(rule.type & ORDERING){
      out << 'd';  // Use disabling semantics.
    }
  }
  out << " ";
  if(rule.epsilon){
    out << '1';
  }
  else{
    out << '0';
  }
  if(rule.type & ASSUMPTION){
    out << " a a";
  }
  else{
    out << " " << rule.lower;  // lower timing bound
    if(INFIN == rule.upper){
      out << " inf";  // text symbol for INFIN
    }
    else{
      out << " " << rule.upper;  // finite upper bound
    }
  }
  //out << " S(" << rule.lower << ")";
  if(rule.epsilon){
    out << " # INITIAL";
  }
  return out << endl;
}

ostream& operator<<(ostream& out, const Rule* rule){  // Output *rule.
  if(!rule){
    return out << "### NULL RULE! ###" << endl;
  }
  return out << *rule;
}
///// END class Rule /////////////////////////////////////////////////////////


/// BEGIN class Candidate/////////////////////////////////////////////////////
// Create a new Candidate:
Candidate::Candidate(Event* Enabling, Event* Enabled, int Epsilon):
  enabling(Enabling),
  enabled(Enabled),
  epsilon(Epsilon),
  rule(NULL),
  conflicts(enabled->conflicts),
  promising(Enabling->communication&&Enabled->communication &&
	    (!Epsilon && Enabled->reset() || Epsilon && !Enabled->reset()) &&
	    (!Enabling->reset() &&
	     Enabled->communication->reaches(Enabling->communication,Epsilon,1)
	     ||
	     Enabling->reset() &&
	     Enabling->communication->reaches(Enabled->communication,Epsilon,1)
	     )){
  for(Event::Set::iterator c(conflicts.begin());  c!=conflicts.end();  c++){
    conflictingCliques.insert((*c)->clique);
  }
}

Candidate::~Candidate(){  // Destroy this Candidate.
  remove();
}

void Candidate::add(void){ // Add this Candidate's rule unless it's been added.
  if(!rule){
    rule = new Rule(enabling, enabled, epsilon);
  }
}

void Candidate::remove(void){//Remove this Candidate's rule if it's been added.
  if(rule){
    delete rule;
    rule = NULL;
  }
}

bool Candidate::redundant(void){  // Is this candidate's rule now redundant?
  bool result(false);
  if(rule){  // This rule has already been added.
    remove();  // Even without this rule,
    result = enabling->reaches(enabled, epsilon);
    add();
  }
  return result;
}

bool Candidate::Promising(void) const{  // Is this candidate promising?
  return promising;
}

Event* Candidate::Enabling(void) const{//Return a pointer to the enabling Event
  return enabling;
}

Event* Candidate::Enabled(void) const{ //Return a pointer to the enabled Event.
  return enabled;
}

int Candidate::Epsilon(void) const{  // Return the epsilon value on this rule.
  return epsilon;
}

ostream& operator<<(ostream& out, const Candidate& candidate){//Print candidate
  out << candidate.enabling << candidate.enabled << candidate.epsilon;
  if(candidate.promising){
    out << " PROMISING";
  }
  return out << endl;
}
///// END class Candidate/////////////////////////////////////////////////////


/// BEGIN class Event ////////////////////////////////////////////////////////
int Event::nextIndex = 1;

Event::Event(): // Create a new dummy event named $HSE<nextIndex>.
  Rin(NULL),
  Rout(NULL),
  next(NULL),
  communication(NULL),
  action("$HSE"),
  occurrence(nextIndex++),
  type(DUMMY),
  data(""),
  signal(action),
  expression(""),
  status(0){
}

Event::Event(const string& name): // Create a new dummy event named <name>.
  Rout(NULL),
  communication(NULL),
  action(name),
  occurrence(1),
  type(DUMMY),
  data(""),
  signal(action),
  expression(""),
  status(0){
}

Event::Event(const string& Signal, char direction)://Create a new output event.
  communication(NULL),
  action(Signal+direction),
  occurrence(1),
  type(OUT),
  data(""),
  signal(Signal),
  expression(""),
  status(0){
}

Event::Event(eventADT event): // Build an event from the old eventADT.
  communication(NULL),
  action(event->event),  // This will be reduced to just <signal>+-.
  occurrence(1),
  type(event->type),
  data(event->data),
  signal(action, 0, action.find_first_of("+-")),  // just the <signal> name
  expression(""),  // This stores the expression computed by eatFanout().
  status(0){  // This is used for bookkeeping during Depth-First Searches.
  string::size_type pos(action.find("/"));
  if(pos != string::npos){
    string afterSlash(action, pos+1, string::npos);
    occurrence=atoi(afterSlash.c_str());
    action.erase(pos, string::npos);
  }
  if('$'==action[0]){
    type |= DUMMY;  // just in case this is a hand-hacked TEL
  }
}

Event::~Event(void){  // Delete this event and all rules that involve it.
  while(!Fanin.empty()){
    delete *Fanin.begin();  // The rule destructor removes the rule from Fanin.
  }
  while(!Fanout.empty()){
    delete *Fanout.begin(); //The rule destructor removes the rule from Fanout.
  }
  while(!inConstraints.empty()){
    delete *inConstraints.begin(); //Destructor removes rule from inContraints.
  }
  while(!outConstraints.empty()){
    delete *outConstraints.begin();//Destructor removes rule from outContraints
  }
}

bool Event::isCommunication(void) const{//Am I part of a channel communication?
  return !channel().empty();
}

bool Event::reset(void) const{//Am I part of the reset phase of communication?
  return isCommunication() && action.find('-') != string::npos;
}

bool Event::passive(void) const{//Am I part of a passive channel communication?
  if(type & PULL){
    return suffixPos(signal,SEND_SUFFIX);
  }
  else{
    return suffixPos(signal,RECEIVE_SUFFIX);
  }
}

bool Event::dumb(void) const{  // Is this event a dummy event?
  return type & DUMMY;
}

bool Event::uses(const string & Signal) const{ // Do I or my Fanout use Signal?
  if(signal == Signal){
    return true;
  }
  for(Rule::Set::iterator Rout(Fanout.begin());  Rout!=Fanout.end();  Rout++){
    if((*Rout)->uses(Signal)){
      return true;
    }
  }
  return false;
}

bool Event::isMerge(void) const{
  Event::Set enablings(predecessors());
  if(enablings.empty()){
    loggedStream errors(cerr,lg);
    errors << this << "has ";
    throw("no predecessor events!");
  }
  return(!disjoint((*(enablings.begin()))->conflicts, enablings));
}

void Event::eatFanout(void){//Pop expressions from Fanout rules. Store the AND.
  fanoutExpression();
  for(Rule::Set::iterator Rout(Fanout.begin());  Rout!=Fanout.end();  Rout++){
    (*Rout)->clearExpression();
  }
}

void Event::produceFanin(void){ // Push stored expression onto each Fanin rule.
  for(Rule::Set::iterator Rin(Fanin.begin());  Rin!=Fanin.end();  Rin++){
    (*Rin)->pushExpression(expression);
  }
}

const string& Event::fanoutExpression(void){  // product of fanout expressions
  expression = "";
  for(Rule::Set::iterator Rout(Fanout.begin());  Rout!=Fanout.end();  Rout++){
    expression = And(expression, (*Rout)->Expression());
  }
  return expression;
}

const string& Event::faninExpression(void){  // product of fanin expressions
  expression = "";
  for(Rule::Set::iterator Rin(Fanin.begin());  Rin!=Fanin.end();  Rin++){
    expression = And(expression, (*Rin)->Expression());
  }
  return expression;
}

void Event::markUnseen(void){
  status = 0;
}

/* On each path from this Event, find the first Event that uses the
   same signal as does loose. Create a new rule from loose to the
   event that was found. This rule will have the expression exp. The
   rule should be initially marked if the input epsilon is greater
   than 0 or if an initially marked rule is encountered along the
   path. If the added rule is to be initially marked, the
   corresponding reset rule is also added. The argument seen is used
   to distinguish distinct depth first searches. (When visit() calls
   itself, it keeps seen the same. But each new, external call to
   visit() should use a distinct value of seen.) */
void Event::visit(int seen, Event* loose, const string& exp, Event* reset,
		  int epsilon){
  if(status != seen){
    status = seen;
    if(uses(loose->signal)){
      if(this != loose){
	new Rule(loose, this, epsilon, exp, true);
	if(epsilon && disjoint(reset->Fanout, Fanin)){
	  new Rule(reset, this, 0, exp, true);
	}
      }
    }
    else{
      for(Rule::Set::iterator pos(Fanout.begin());  pos!=Fanout.end();  pos++){
	Rule* Rout(*pos);
	if(!Rout->addedByVisit){
	  Rout->Enabled()->
	    visit(seen, loose, exp, reset, epsilon+Rout->Epsilon());
	}
      }
    }
  }
}

// Is there a path with at most allow tokens from this to target?
bool Event::reaches(Event* target, int allow, int allowCommunications){
  static int count(0);
  count+=2;
  return reaches(target, allow, count, allowCommunications);
}

// Path from this to target?:
bool Event::reaches(Event* target, int allow, int seen, int allowCommunications){
  if(allow < 0){
    return false;
  }
  if(target == this){
    return true;
  }
  if(communication){
    allowCommunications--;
  }
  if(allowCommunications < 0){
    return false;
  }
  if(status != seen+allow){
    status = seen+allow;
    for(Rule::Set::iterator pos(Fanout.begin());  pos!=Fanout.end();  pos++){
      Rule* Rout(*pos);
      if(Rout->Enabled()->reaches(target, allow - Rout->Epsilon(), seen,
				  allowCommunications)){
	return true;
      }
    }
  }
  return false;
}

/* addDescendants appends the descendants of this Event on to the back
   of the given list found, IN BREADTH-FIRST-SEARCH ORDER.  markUnseen()
   must be called on each event prior to each external call to
   addDescendants() */
void Event::addDescendants(list<Event*> & found){  // found |= my descendants
  list<Event*> Queue;
  Queue.push_back(this);
  while(!Queue.empty()){
    Event* e(Queue.front());
    Queue.pop_front();
    e->status = 1;
    found.push_back(e);
    Event::Set Esucc(e->successors());
    for(Event::Set::iterator pos(Esucc.begin());  pos != Esucc.end();  pos++){
      Event* f(*pos);
      if(!f->status){
	f->status = 1;
	Event::Set Fsucc(f->successors());
	Queue.insert(find_first_of(Queue.begin(),Queue.end(),
				   Fsucc.begin(),Fsucc.end()),f);
      }
    }
  }
}

void Event::computeClique(void){
  if(conflicts.empty()){
    clique.clear();
  }
  else{
    clique = (*(conflicts.begin()))->conflicts;
    for(Event::Set::iterator c(conflicts.begin());  c!=conflicts.end();  c++){
      clique &= (*c)->conflicts;
    }
  }
}

Event::Set Event::predecessors(void) const{
  Event::Set result;
  for(Rule::Set::iterator Rin(Fanin.begin());  Rin!=Fanin.end();  Rin++){
    result.insert((*Rin)->Enabling());
  }
  return result;
}

Event::Set Event::successors(void) const{
  Event::Set result;
  for(Rule::Set::iterator Rout(Fanout.begin());  Rout!=Fanout.end();  Rout++){
    result.insert((*Rout)->Enabled());
  }
  return result;
}

bool Event::operator<(const Event& that) const{
  return action<that.action || action==that.action&&occurrence<that.occurrence;
}

ostream& operator<<(ostream& out, const Event & event){  // Print event.
  out << event.action;
  if(event.occurrence != 1){
    out << '/' << event.occurrence;
  }
  return out << " ";
}

ostream& operator<<(ostream& out, const Event* event){  // Print *event.
  if(!event){
    return out << "### NULL EVENT! ###" << endl;
  }
  return out << *event;
}

bool conflictFree(const Event::Set& theEvents){
  for(Event::Set::iterator e(theEvents.begin());  e!=theEvents.end();  e++){
    if(!disjoint((*e)->conflicts, theEvents)){
      return false;
    }
  }
  return true;
}

const string& Event::Signal(void) const{  // the signal of this event
  return(signal);
}

string Event::channel(void) const{
  return Channel(signal);
}
///// END class Event ////////////////////////////////////////////////////////


/// BEGIN class Communication ////////////////////////////////////////////////
// Build a communication from its events:
Communication::Communication(Event* Rise, Event* Fall):rise(Rise), fall(Fall){
  rise->communication = rise;
  fall->communication = rise;
}

Communication::~Communication(){  // Destroy this communication.
}

// Compare this communication to that communication:
bool Communication::operator<(const Communication& that) const{
  return rise < that.rise || rise == that.rise && fall < that.fall;
}

void Communication::eatFanout(void) const{  // Apply eatFanout to each event.
  rise->eatFanout();
  fall->eatFanout();
}

// Print out this communication:
void Communication::produceFanin(void) const{//Apply produceFanin to each event
  rise->produceFanin();
  fall->produceFanin();
}

ostream& operator<<(ostream& out, const Communication & communication){
  return out << communication.rise << communication.fall << endl;
}
///// END class Communication ////////////////////////////////////////////////


/// BEGIN class Possibility //////////////////////////////////////////////////
Possibility::Possibility(const prob_vector& cost, const string& fileName,
			 const string&rules):
  name(fileName),
  added(rules),
  literals(0){
  setCost(cost);
  /*  median(period.begin(),period.size(),middle);
  prob_vector Middle(period.size(),middle);
  prob_vector Differences(period.size());
  transform(period.begin(),
	    period.end(),
	    Middle.begin(),
	    Differences.begin(),
	    absDiff());
  adev =
    accumulate(Differences.begin(),Differences.end(),0.0)/Differences.size();
  */
}

Possibility::~Possibility(){
}

void Possibility::setLiterals(const int lits){
  literals = lits;
}

void Possibility::setTransistors(const int area){
  transistors = area;
}

void Possibility::setCost(const prob_vector& cost){
  period = cost;
  moment(&(*period.begin()),period.size(),ave,adev,sdev,var,skew,curt);
}

void Possibility::setName(char * newName){
  name = string(newName);
}

void Possibility::setUnique(const string& uniqueName){
  unique = uniqueName;
}

const int Possibility::Literals(void) const{
  return literals;
}

const int Possibility::Transistors(void) const{
  return transistors;
}

bool Possibility::operator<(const Possibility& that) const{
  if(fabs(ave - that.ave) < (adev + that.adev)){
    return false;
    //return literals < that.literals;
      //|| literals == that.literals && name < that.name;
  }
  else{
    return ave < that.ave;
  }
}

ostream& operator<<(ostream& out, const Possibility& p){
  return out<<p.ave<< " +/- " <<p.adev<< " " <<p.added<< " " <<p.unique<< endl;
}

///// END class Possibility //////////////////////////////////////////////////


/// BEGIN class sparseTEL ////////////////////////////////////////////////////
sparseTEL::sparseTEL(designADT design, bool annotate): // Build a sparseTEL from a designADT.
  calls(0),  // number of times enumerate has been called
  attempts(0),
  checks(0),
  simulations(0),
  solutions(0),
  good(),
  blacklist(),
  infinite(design->si || design->untimed || design->maxgatedelay==INFIN),
  best(Possibility(prob_vector(1,DBL_MAX),"","")),
  filters(""),
  startState("0"),  // the start state of the TEL
  ninputs(design->ninputs),  // the number of input events
  nsmart(0),
  messages(cout, lg),  // used for normal status messages
  verbose(design->verbose),  // Should operations on this TEL be verbose?
  name(design->filename),  // the name of this TEL
  design(design),
  oldStatus(design->status),
  pruning(design->pruning),
  addVariable(false),
  maxPeriod(8.0),
  root(ninputs+1)
{
  if(design->pruning.not_first){
    filters += NOT_FIRST;
  }
  if(design->pruning.preserve){
    filters += PRESERVE;
  }
  if(design->pruning.expensive){
    filters += EXPENSIVE;
  }
  if(design->pruning.subset){
    filters += SUBSET;
  }
  if(design->pruning.unsafe){
    filters += UNSAFE;
  }
  if(design->pruning.ordered){
    filters += ORDERED;
    if(annotate){
      int oldStatus(design->status);
      bool oldReduction(design->reduction);
      design->reduction = false;
      process_command(SYNTHESIS, FINDRSG, design, NULL, FALSE, design->si);
      if(!(design->status & FOUNDSTATES)){
	throw("Could not find RSG for most-concurrent TEL!");
      }
      make_concur_array(design);  // Set up timed concurrency information
      design->reduction = oldReduction;
      design->status = oldStatus;
    }
  }
  for(int i=0;  i<design->nevents;  i++){
    if(!(design->events[i] && design->events[i]->event)){
      throw("NULL event!");
    }
    if(!design->rules[i]){
      throw("NULL row of rules matrix!");
    }
  }
  for(int e(0);  e<design->nevents;  e++){
    if(design->events[e]->event[0]!='$'){
      nsmart++;
    }
    Events.push_back(new Event(design->events[e]));
  }
  if(design->startstate && design->startstate[0]){
    startState = string(design->startstate);
  }
  for(int e(0);  e<design->nevents;  e++){
    for(int f(0);  f<design->nevents;  f++){
      ruleADT rule(design->rules[e][f]);
      if(rule){
	if(rule->ruletype){
	  new Rule(Events[e], Events[f], rule);
	  infinite = infinite || rule->upper==INFIN;
	}
	if(rule->conflict){
	  Events[f]->conflicts.insert(Events[e]);
	  Events[e]->conflicts.insert(Events[f]);
	}
	if(rule->concur){
	  Events[f]->concurs.insert(Events[e]);
	  Events[e]->concurs.insert(Events[f]);
	}
      }
    }
    mergeADT curmerge(design->merges[e]);
    if(curmerge && curmerge->mergetype && curmerge->mevent > e){
      int Mevent(e);
      list<Event*> mergeList(1, Events[Mevent]);
      while(curmerge){
	if(curmerge->mergetype && curmerge->mevent > e){
	  Mevent++;
	  mergeList.push_back(Events[++Mevent]);
	}
	curmerge = curmerge->link;
      }
      Merges.insert(mergeList);
    }
  }
  unsigned int event(1);
  bool output(false);
  while(event < nsmart){
    output = event > ninputs;
    Event* rise(Events[event++]);
    Event* fall(Events[event++]);
    if(rise->isCommunication()){
      Communication c(rise, fall);
      Communications.insert(c);
      if(rise->passive()){
	Passives.insert(c);
      }
      if(output){
	outputCommunications.insert(c);
      }
    }
  }
}

sparseTEL::~sparseTEL(){  // Destroy this sparseTEL.
  while(!Events.empty()){
    delete Events.back();  // This also deletes the rules to/from the Event.
    Events.pop_back();
  }
}

void sparseTEL::passify(void){  // Make passive communications truly passive.
  if(verbose){
    messages << "Passifying channels..." << endl;
  }
  for(set<Communication>::iterator p(Passives.begin());p!=Passives.end();p++){
    p->eatFanout();  // Delete fanout expressions, storing their logical AND.
  }
  set<Communication> Union(outputCommunications);
  Union |= Passives; // Union has all events that are either output or passive.
  /* Rules with expressions that go into passive events will need to
     be split to make room for the expression provided by
     produceFanin() below. Furthermore, even those that go into active
     output events need to be split so that interleave() will not need
     to duplicate expressions.*/
  for(set<Communication>::iterator c(Union.begin());  c!=Union.end();  c++){
    Rule::Set fanin(c->rise->Fanin);
    for(Rule::Set::iterator inPos(fanin.begin()); inPos!=fanin.end(); inPos++){
      Rule* Rin(*inPos);
      if(Rin->Enabling()!=Events[0] && // not a reset rule
	 Rin->Enabling()!=c->fall &&
	 !Rin->Enabling()->dumb() &&
	 !Rin->Expression().empty()){  // Rin has a non-trivial expression.
	Rule::Set Sin;
	Sin.insert(Rin);
	split(Sin, Rin->Enabled(),!Rin->Enabling()->dumb());
      }
    }
  }
  for(set<Communication>::iterator p(Passives.begin());p!=Passives.end();p++){
    Rule::Set fanin(p->rise->Fanin);
    bool smart(false);
    for(Rule::Set::iterator r(fanin.begin()); r!=fanin.end()&&!smart; r++){
      Rule* Rin = (*r);
      if(!Rin->Enabling()->dumb()){
	smart = true;
      }
    }
    if(smart){
      split(fanin, p->rise, false);
    }
    p->produceFanin(); // Push expression stored by eatFanout onto Fanin rules.
  }
  store(name+"_myPassive");  // Write resulting TEL to <name>_myPassive.tel.
}

void sparseTEL::interleave(void){  // Make communications maximally concurrent.
  if(verbose){
    messages << "Interleaving channels..." << endl;
  }
  Event* reset(Events.front());
  int count(1);
  set<Communication>::iterator communication(outputCommunications.begin());
  while(communication != outputCommunications.end()){
    Event* fall(communication->fall);
    if(fall->Fanin.size() != 1 || fall->Fanout.size() != 1){
      throw("unexpected size!");
    }
    fall->Rout = *(fall->Fanout.begin());
    fall->next = fall->Rout->Enabled();
    if(fall->next != communication->rise){
      fall->Rin = *(fall->Fanin.begin());
      if(!fall->passive()){
	fall->exp = Not(fall->Rin->Expression());
	if(fall->next->dumb() && fall->next->Fanin.size() > 1){
	  split(fall->Fanin,fall);
	  fall->Rin = *(fall->Fanin.begin());
	}
      }
      fall->next->visit(count++,fall, fall->exp, reset, fall->Rout->Epsilon());
    }
    communication++;
  }
  communication = outputCommunications.begin();
  while(communication != outputCommunications.end()){
    Event* fall(communication->fall);
    if(fall->next != communication->rise){
      if(fall->added.size()>1){
	Event::Set targets;
	Rule::Set::iterator r(fall->added.begin());
	while(r!=fall->added.end()){
	  targets.insert((*r)->Enabled());
	  r++;
	}// Now targets={f|(fall, f, lower, upper, expression) in fall->added}.
	bool conflictFree(true);
	Event::Set::iterator f(targets.begin());
	while(conflictFree && f!=targets.end()){
	  conflictFree = disjoint((*f)->conflicts,targets);
	  f++;
	}
	if(!conflictFree){
	  Event* source(NULL);
	  Event* sink(NULL);
	  string sourceName('$'+fall->channel()+"_Source");
	  for(unsigned int d(nsmart);  d<Events.size();  d++){
	    if(Events[d]->Signal()==sourceName){//We already handled this
	      source=Events[d];//so use the source dummy event we already added
	      if(source->Fanin.size()!=1){
		throw("unexpected size");
	      }
	      sink = (*(source->Fanin.begin()))->Enabling();
	      //  In this case, we don't need the rules that visit() added:
	      Rule::Set::iterator r(fall->added.begin());
	      while(r!=fall->added.end()){
		delete(*r);
		r++;
	      }
	    }
	  }
	  if(!source){//must add source and sink dummy events for this channel
	    source = new Event(sourceName);  // This event has no conflicts.
	    Events.push_back(source);
	    sink = new Event('$'+fall->channel()+"_Sink");//has no conflicts
	    Events.push_back(sink);
	    // All communications on this channel will start from source:
	    Rule::Set::iterator r(fall->added.begin());
	    source->conflicts = (*r)->Enabled()->conflicts;
	    while(r!=fall->added.end()){
	      Rule* Radd(*r);
	      Radd->setEnabling(source);
	      source->conflicts &= Radd->Enabled()->conflicts;
	      r++;
	    }
	    sink->conflicts = source->conflicts;
	    Event::Set::iterator c(source->conflicts.begin());
	    while(c != source->conflicts.end()){
	      (*c)->conflicts.insert(source);
	      (*c)->conflicts.insert(sink);
	      c++;
	    }
	    source->eatFanout();//fanout expressions should now appear on:
	    //Channel must reset before next use:
	    new Rule(sink, source, 0, fall->exp);
	  }
	  new Rule(fall, sink, 0);//All communications on channel end in sink.
	  sink->conflicts &= fall->conflicts;
	}
      }
      if(fall->next != fall->Rin->Enabling()){
	fall->Rout->setEnabling(fall->Rin->Enabling());
	if(!fall->passive()){
	  fall->Rout->copyExpression(fall->Rin);
	  if(fall->next->dumb() && 1==fall->next->Fanin.size()){
	    fall->Rin->setEnabling(fall->next);
	    fall->Rin->clearExpression();
	  }
	}
      }
    }
    communication++;
  }
  store(name+"_myWoven");  // Write resulting TEL to <name>_myWoven.tel.
}

void sparseTEL::enumerate(list<Candidate>::iterator start,
			  list<Candidate>::iterator level,
			  list<Candidate>::iterator stop,
			  const string& decided,
			  Possibility current){
  //messages << "ENUMERATE(" << decided << ")" << endl;
  calls++;  // Increment the number of times this function has been called.
  if(pruning.not_first && !good.empty()){
    //messages << "PRUNED -- NOTFIRST" << endl;
    return;
  }
  if(pruning.subset){
    for(set<string>::iterator b(blacklist.begin());  b!=blacklist.end();  b++){
      if(subset(decided, *b)){
	//messages << "PRUNED -- SUBSET" << endl;
	//This sub-tree has a subset of the constraints of one that failed CSC.
	return;  // Therefore, any leaf in this sub-tree would also fail CSC.
      }
    }
  }
  prob_vector results(17, 0.0);
  const int startingCycle(3);
  const int numCycles(17);
  string newFileName(name + "_HSE_CURRENT");
  if(newFileName.size() >= FILENAMESIZE){
    throw("file name too long!");
  }
  if(decided.empty() || '3'==decided[decided.size()-1]){//We just added a rule
    store(newFileName);
    strcpy(design->filename, newFileName.c_str());
    design->fromVHDL=false;
    design->fromTEL=true;
    design->status = oldStatus|REDUCED;
    if(!load_event_rule_system(LOAD, TEL, design, design->si, true)){
      throw("Could not load event rule system for solution.");
    }
    design->status = oldStatus|REDUCED;
    simulations++;
    if(pruning.not_first || !pruning.expensive ||
       run_simulation(design, root, numCycles, startingCycle, results)){
      current = Possibility(results,newFileName,decided);
    }
  }
  if(pruning.expensive && best < current){
    //messages << "PRUNED -- EXPENSIVE" << endl;
    return;
  }
  if(level==stop){  // We have reached a leaf of the search tree!
    attempts++;
    store(newFileName);
    strcpy(design->filename, newFileName.c_str());
    design->fromVHDL=false;
    design->fromTEL=true;
    design->status = oldStatus|REDUCED;
    if(!load_event_rule_system(LOAD, TEL, design, design->si, true)){
      throw("Could not load event rule system for solution.");
    }
    design->status = oldStatus|REDUCED;
    design->status =
      process_command(SYNTHESIS, SINGLE, design, NULL, false, design->si);
    current.setName(design->filename);
    if(design->status & FOUNDSTATES){
      checks++;
      if(design->status & STOREDPRS){
	current.setLiterals(design->total_lits);
	current.setTransistors(design->area);
	if(pruning.not_first || !pruning.expensive){
	  //We didn't simulate before, so let's simulate now
	  if(run_simulation(design, root, numCycles, startingCycle, results)){
	    current.setCost(results);
	  }
	}
	solutions++;
	string unique(name + "_HSE_" + filters + "_" + int2string(solutions));
	current.setUnique(unique);
	if(good.insert(current).second){ // Cost was significantly different
	  store(unique);
	  string command("mv " + current.name + ".prs " + unique +".prs");
	  if(system(command.c_str())){
	    throw("Could not execute " + command);
	  }
	}
	if(current < best){
	  current.name = name + "_HSE_BEST";
	  strcpy(design->filename, current.name.c_str());
	  store(current.name);
	  design->status =
	    process_command(STORE, RSG, design, NULL, FALSE, design->si);
	  best = current;
	}
      }
      else{  // Failed...
	if(!(design->status & CSC)){  // ...because of a CSC violation
	  blacklist.insert(decided);  // Record that fact.
	}
      }
    }
    //messages << "LEAF" << endl;
    return;
  }

  list<Candidate>::iterator candidate(level++);
  Event* enabling(candidate->Enabling());
  Event* enabled(candidate->Enabled());
  int epsilon(candidate->Epsilon());
  bool must(false);
  bool can(true);
  if(pruning.unsafe && !enabled->conflicts.empty()){
    Event::Set coverage(enabling->conflicts);
    for(list<Candidate>::iterator c(level);  c!=stop;  c++){
      if(c->Enabling() == enabling){
	coverage.insert(c->Enabled());
      }
    }
    coverage.erase(enabled);
    Event::Set successors(enabling->successors());
    coverage |= successors;
    set<Event::Set>::iterator c(candidate->conflictingCliques.begin());
    while(can && c!=candidate->conflictingCliques.end()){
      if(disjoint(*c, coverage)){
	//messages << "CANNOT" << endl;
	can = false;
      }
      c++;
    }
    if(!disjoint(enabled->conflicts, successors)){
      if(disjoint(enabled->clique, coverage)){
	//messages << "MUST" << endl;
	must = true;
      }
    }
    if(must&&!can){
      if(candidate->Promising()){
	can = true;
      }
      else{
	must = false;
      }
    }
  }
  //assert(can ||!must);
  char noAdd('0');
  if(can && pruning.reachable && enabling->reaches(enabled, epsilon)){
    //messages << "REDUNDANT" << endl;
    // There is already an epsilon-token path from enabling to enabled, so
    noAdd = '1';  // this arc is redundant.
  }
  Possibility oldBest(best);
  if(!candidate->Promising() && !must){
    enumerate(start, level, stop, decided+noAdd,current); //What if we don't add this?
  }
  if(can && noAdd != '1'){
    /* If there is a zero-token path back from enabled to enabling,
       adding a non-initial rule from enabled to enabling would
       create a zero-token cycle. This would result in deadlock.
       Therefore, we must make sure we are NOT in this situation,
       with the following if statement: */
    if(epsilon||!(pruning.reachable &&enabled->reaches(enabling,0))){
      candidate->add();
      bool ok(true);
      if(pruning.reachable){
	for(list<Candidate>::iterator c(start); ok && c!=candidate; c++){
	  ok = ! c->redundant();
	}
      }
      if(ok){
	enumerate(start,level,stop,decided+'3',current);//What if we add this rule?
      }
      candidate->remove();
    }
  }
  if(candidate->Promising() && !must){
    enumerate(start, level, stop, decided+noAdd,current); //What if we don't add this?
  }
  if(best < current && current < oldBest){
    loggedStream errors(cerr,lg);
    errors << "The old version was:  " << current
	   << "The new version is:   " << best;
    throw("NOT A BOUNDING FUNCTION");
  }
}

void sparseTEL::fatten(void){  // Insert dummy events as needed.
  /* In most cases, if a rule with a non-trivial expression goes
     directly into a non-dummy event, we will need to split that rule.
     This will give us the ability to force the wait for the
     expression to happen before some other event, without forcing the
     enabled event to also occur before that other event. However,
     there are some exceptions... */
  for(unsigned int e(ninputs+1); e<nsmart; e++){
    Rule::Set fanin(Events[e]->Fanin);
    for(Rule::Set::iterator r(fanin.begin());  r!=fanin.end();  r++){
      Rule* Rin(*r);
      if(!(Rin->Enabling()==Events.front() ||  // Don't split reset rules.
	   Rin->Enabling()->dumb() || // Don't split rules from dummy events.
	   Rin->Expression().empty() || //Don't split rules with no expression.
	   Rin->Epsilon() &&  // Don't split an initial rule, IF...
	   Rin->Enabled()->passive())){  // ...it enables a passive event.
	Rule::Set Sin;
	Sin.insert(Rin);
	split(Sin, Rin->Enabled());
      }
    }
  }
  if(addVariable){
    Event* fall(new Event("HSECSC", '-'));
    Events.push_back(fall);
    Event* rise(new Event("HSECSC", '+'));
    Events.push_back(rise);
    new Rule(rise, fall, 0);
    new Rule(fall, rise, 1);
    new Rule(Events.front(), rise, 0);
    startState+='0';
  }
  store(name + ".fat");
}

list<Candidate> sparseTEL::candidates(void){
  list<Event*> outProcess;
  for(unsigned int f(0);  f<Events.size(); f++){
    Events[f]->markUnseen();
  }
  Event::Set roots;
  for(unsigned int e(ninputs+1);  e<Events.size();  e++){
    set<Rule *> fanin(Events[e]->Fanin);
    unsigned int needed(fanin.size());
    if(!disjoint(Events.front()->Fanout,fanin)){//A reset rule points to *e
      needed--;
    }
    unsigned int sum(0);
    for(set<Rule *>::iterator Rin(fanin.begin());  Rin!=fanin.end();  Rin++){
      sum += (*Rin)->Epsilon();
    }
    if(sum >= needed){// Events[e] is a root of some process...
      if(Events[e]->reaches(Events[root],1)){
	roots.insert(Events[e]);
      }
      if(Events[e]->reaches(Events[root],0)){
	root=e;
      }
    }
  }
  for(unsigned int e(ninputs+1);  e<Events.size();  e++){
    if(Events[e]->successors() == roots){
      root = e;
    }
  }
  for(unsigned int f(0);  f<Events.size();  f++){
    Events[f]->markUnseen();
  }
  Events[root]->addDescendants(outProcess);//And descendants of e IN BFS ORDER
  /* outProcess is now a list of the Events in the output process,
     ORDERED SUCH THAT IF THERE IS AN UNMARKED RULE FROM E TO F, E
     OCCURS BEFORE F IN outProcess. THIS ORDER GREATLY REDUCES THE
     NUMBER OF REDUNDANT POSSIBILITIES THAT enumerate() WILL TRY.  */

  if(verbose){
    messages << "Output process: " << outProcess << endl;
  }
  for(unsigned int e(0);  e < Events.size();  e++){
    Events[e]->markUnseen();
    Events[e]->computeClique();
  }
  list<Candidate> candidates;
  list<Candidate> marked;
  list<Event*>::reverse_iterator e(outProcess.rbegin());
  while(e != outProcess.rend()){  // e walks BACKward through outProcess
    /* Thus, given a rule A->B, we will always consider adding B->C
       before we consider adding A->C.  */
    Event* enabling(*e++);
    if(!(pruning.dumb &&
	 enabling->dumb() &&  // Don't add rules from a dummy event, IF...
	 enabling->faninExpression().empty() &&//...it has no fanin expressions
	 !enabling->isMerge()
	 )){  //... and does not have conflicting enablings.
      list<Event*>::iterator f(outProcess.begin());
      while(f != outProcess.end()){  // f walks FORward through outProcess
	/* Thus, given a rule X->Y, we will always consider adding
	   Z->X before we consider adding Z->Y.  */
	Event* enabled(*f++);
	if(!(pruning.dumb && enabled->dumb() ||
	     pruning.reachable &&
	     (!disjoint(enabling->Fanout, enabled->Fanin) ||
	      enabling->reaches(enabled, 0)) ||
	     pruning.ordered &&
	     enabling->concurs.find(enabled) == enabling->concurs.end() ||
	     pruning.preserve &&
	     enabling->communication && enabled->communication &&
	     !enabling->communication->reaches(enabled->communication,0) &&
	     !enabled->communication->reaches(enabling->communication,0) ||
	     pruning.conflict &&
	     enabling->conflicts.find(enabled) != enabling->conflicts.end())){
	  bool forward1(enabling->reaches(enabled, 1));
	  /*	 bool reverse1(enabled->reaches(enabling, 1));
		 if(forward1 || reverse1){
	  */
	  if(!(pruning.reachable && forward1)){
	    marked.push_back(Candidate(enabling, enabled, 1));
	  }
	
	  /* If there is a zero-token path back from enabled to
	     enabling, adding a non-initial rule from enabled to
	     enabling would create a zero-token cycle. This would
	     result in deadlock. Therefore, we must make sure we are
	     NOT in this situation, with the following if statement:  */
	  if(!(pruning.reachable && enabled->reaches(enabling, 0))){
	    candidates.push_back(Candidate(enabling, enabled, 0));
	  }
	  //	 }
	}
      }
    }
  }
  candidates.insert(candidates.end(), marked.begin(), marked.end());
  if(verbose){
    messages << "CANDIDATES= " << candidates << endl;
  }
  return candidates;
}

// Redirect all rules in fanin to a new dummy event, with one rule to sink:
Event* sparseTEL::split(Rule::Set fanin, Event* sink, bool slideToken){
  if(fanin.empty()){
    throw("No fanin rules!");
  }
  int epsilon(0);
  Event* dummy(new Event);
  Events.push_back(dummy);
  dummy->conflicts = (*(fanin.begin()))->Enabling()->conflicts;
  Event::Set predecessors;
  for(Rule::Set::iterator r(fanin.begin());  r!=fanin.end();  r++){
    Rule* Rin = (*r);
    if(1==fanin.size() || Rin->Enabling()!=Events.front()){
      Rin->setEnabled(dummy);
      if(slideToken){
	if(Rin->Epsilon()){
	  epsilon=1;
	}
	Rin->setEpsilon(0);
      }
      dummy->conflicts &= Rin->Enabling()->conflicts;
    }
    predecessors.insert(Rin->Enabling());
  }
  Rule* added(new Rule(dummy, sink, epsilon));
  if(predecessors.empty()){
    throw("No predecessors!");
  }
  if(disjoint((*(predecessors.begin()))->conflicts,predecessors)){
    added->grabTiming(fanin);
  }
  else{
    added->clearTiming();
  }
  dummy->conflicts |= sink->conflicts;
  Event::Set::iterator c(dummy->conflicts.begin());
  while(c != dummy->conflicts.end()){
    (*c)->conflicts.insert(dummy);
    c++;
  }
  return dummy;
}

void sparseTEL::store(const string& baseName){  // Store TEL to file.
  string outname(baseName);
  if(design->level){
    outname += ".tel";
  }
  else{
    outname += ".er";
  }
  if(verbose){
    messages<< "Storing to file " << outname << endl;
  }
  ofstream TELfile(outname.c_str());
  if(!TELfile){
    throw("Could not open " + outname + " for writing: " + strerror(errno));
  }
  TELfile << *this;
  lastStored = baseName;
}

ostream& operator<<(ostream& out, sparseTEL& tel){  // Print a sparseTEL.
  list<Rule*> Causal;
  list<Rule*> Constraints;
  int numConflicts(0);
  for(unsigned int e(0);  e < tel.Events.size();  e++){
    Event* event(tel.Events[e]);
    Causal.insert(Causal.end(), event->Fanout.begin(), event->Fanout.end());
    Constraints.insert(Constraints.end(),
		       event->outConstraints.begin(),
		       event->outConstraints.end());
    numConflicts += event->conflicts.size();
  }
  numConflicts /= 2;
  out
    << "# This Timed Event/Level structure contains:" << endl
    << ".e " << tel.Events.size() << " # events" << endl
    << ".i " << tel.ninputs << " # input events" << endl
    << ".r " << Causal.size() << " # causal rules" << endl
    << ".n " << Constraints.size() << " # ordering rules"<< endl
    << ".d " << tel.Merges.size() << " # merges" << endl
    << ".c " << numConflicts << " # conflicts" << endl
    << ".s " << tel.startState << " # start state" << endl
    << "#" << endl
    << "# EVENTS:" << endl
    << "#" << endl
    << "reset" << endl
    << "# Each of the following events has the format <action>[/<occurrence>]"
    << endl << "# (if the occurrence is omitted, it is assumed to be 1)"
    << endl << "# Input Events:" << endl;
  unsigned int e(1);
  while(e <= tel.ninputs){
    out << tel.Events[e++];
    out << tel.Events[e++] << endl;
  }
  out << "# Output Events:" << endl;
  while(e < tel.nsmart){
    out << tel.Events[e++];
    out << tel.Events[e++] << endl;
  }
  out << "# Dummy (Sequencing) Events:" << endl;
  while(e < tel.Events.size()){
    out << tel.Events[e++] << endl;
  }
  out << "#" << endl
      << "# RULES, which have the following format:" << endl
      << "# <enabling> <enabled> [<expression>] <epsilon> <lower> <upper>"
      << endl
      << "# Causal (Behavioral) Rules:" << Causal
      << "# Ordering (Constraint) Rules:" << Constraints
      << "#" << endl << "# MERGERS:" << endl << "#" << tel.Merges
      << "#" << endl << "# CONFLICTS:" << endl << "#" << endl;
  for(unsigned int e(0);  e < tel.Events.size();  e++){
    Event::Set conflicts(tel.Events[e]->conflicts);
    Event::Set::iterator f(conflicts.begin());
    while(f!=conflicts.end()){
      if(**f < *tel.Events[e]){
	out << tel.Events[e] << *f << endl;
      }
      f++;
    }
  }
  return out;
}
///// END class sparseTEL ////////////////////////////////////////////////////


/// BEGIN class loggedStream /////////////////////////////////////////////////
loggedStream::loggedStream(ostream& Stream, FILE* log):
  interactive(Stream), logFile(log){
}

loggedStream::~loggedStream(){
}

loggedStream& loggedStream::operator<<(ostream& (*manip)(ostream&)){
  interactive << manip;
// gcc296 method
//   logFile << manip;
  ostringstream oss;
  oss << manip;
  fprintf(logFile,oss.str().c_str());
  return *this;
}
///// END class loggedStream /////////////////////////////////////////////////
