// svclasses.h
// class declarations for classes used in state coding version 2
#include "limits.h"
#ifndef _SVCLASSES_
#define _SVCLASSES_

#ifndef MAXINT
#define MAXINT INT_MAX
#endif

// atacs
#include "struct.h"

#ifndef OSX
#ifndef NT
#include <values.h> // for MAXINT
#endif
#endif

#include "CTextTel.h"

// STL
#include <list>
#include <string>
#include <vector>
#include <set>

typedef enum 
{
   COLOR_0,
   COLOR_1,
   COLOR_R,
   COLOR_F,
   COLOR_BLANK
} svColor;

#define BITS_PER_COLOR 3

typedef unsigned int sv_event_index;
typedef vector<sv_event_index> sv_event_index_cont;
typedef list<stateADT> state_list;
typedef vector<state_list> state_list_cont;

// forward declaration, declared for real later in this file
class svSystem;

////////////////////////////////////////////////////////////////////////////////
class svTP
{
   friend ostream& operator<<(ostream& o, const svTP& tp);

   private:
      sv_event_index_cont st;
      sv_event_index_cont et;
      sv_event_index_cont wt;
      
   public:
      svTP();
      
      svTP(const sv_event_index_cont& start, const sv_event_index_cont& end)
	    : st(start), et(end) {}
      
      ~svTP();
	
      const sv_event_index_cont& startEvents() const { return st; }
      const sv_event_index_cont& witnessEvents()   const { return wt; }
      const sv_event_index_cont& endEvents()   const { return et; }
      bool hasStartEvents() { return ! st.empty(); }
      bool hasEndEvents() { return ! et.empty(); }
      bool hasWitnessEvents() { return ! wt.empty(); }
      void addStartEvent(sv_event_index e) { st.push_back(e); }
      void addEndEvent(sv_event_index e)   { et.push_back(e); }
      void addWitnessEvent(sv_event_index e)   { wt.push_back(e); }
      void eraseNthWitnessEvent(sv_event_index it)
	{ wt.erase(wt.begin() + it); }
      sv_event_index popLastStartEvent()
	{ sv_event_index e = st.back(); st.pop_back(); return e;}
      sv_event_index popLastEndEvent()
	{ sv_event_index e = et.back(); et.pop_back(); return e;}
};
typedef vector<svTP> tps_container;

////////////////////////////////////////////////////////////////////////////////
class svIP
{
  friend ostream& operator<<(ostream& stream, const svIP& ip);

 private: // prevent compiler from using this ctor
  svIP();

   public:
      svIP(svSystem* s, const svTP& rise, const svTP& fall)
	 : sys(s), rTP(rise), fTP(fall), initialValue('?'), coloredCost(-1) { }

      svSystem* system() const { return sys; }

      int cost() const;

      bool compatible() const;

      bool consistent();

      char initialSignalValue() const { return initialValue; }
      void setInitialSignalValue(const char value) { initialValue = value; }

      const svTP& risingTP() const { return rTP; }
      const svTP& fallingTP() const { return fTP; }

      bool operator< (const svIP& o) const;

   private:
      svSystem* sys;
      svTP rTP;
      svTP fTP;
      char initialValue;
      mutable int coloredCost;
};
typedef vector<svIP> ips_container;
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// TODO put the enum into the class

typedef enum 
{
   eConcurrent,
   eConflicts,
   eSerial
} concurrency_status;

class svConcurrency
{
      typedef set<int> concurrencyRow;
      typedef vector< concurrencyRow > concurrencyMatrix;

   private:
      concurrencyMatrix concurrencyTable;
      ruleADT **rules; // ptr to 2D array of rules
      cycleADT ****cycles;
      int nevents;
      int ncycles;
      int svMaxDelay;

   public:
      svConcurrency(designADT d);
      ~svConcurrency();

      concurrency_status eventsAreConcurrent(const sv_event_index_cont& events) const;
      concurrency_status eventsAreConcurrent(const sv_event_index_cont& events,
					     sv_event_index e) const;
      bool hasConflict(const svTP& tp) const;
      bool hasConflict(const sv_event_index_cont& events, const sv_event_index e) const;
      bool isWitness(const sv_event_index_cont& events, const sv_event_index e) const;
      bool isWitness(const sv_event_index s, const sv_event_index e) const;

};

///////////////////////////////////////////////////////////////////////////
class svSystem
{
   private:
      designADT design;
      svConcurrency concurrency;
      state_list_cont erStates;// erStates[event#]=list of ptrs to states in ER
      state_list_cont srStates;// srStates[event#]=list of ptrs to states in SR
      unsigned int color_field_offset;
      int color_mask;
      CTextTel textTel;
      int variableNumber;

   private:
      svSystem(); // prevent calls to this function
      
   public:
      svSystem(designADT d);
      svSystem(const svSystem& o)
	 : design(o.design), concurrency(o.design),
	   erStates(o.erStates), srStates(o.srStates),
	   color_field_offset(o.color_field_offset),
	   color_mask(o.color_mask), textTel(o.textTel),
	   variableNumber(0) {}

      ~svSystem();
      
      bool operator< (const svSystem& o)
	{ return violations() < o.violations(); }

      const state_list_cont& excitationRegionStates() const { return erStates; }
      const state_list_cont& switchingRegionStates() const { return srStates; }
      bool install(bool refindStateSpace = true);
      int findInitialColor() const;
      char findInitialMarking(const svTP& tp, const sv_event_index e) const;
      bool insertStateVar(const svIP & insPoint, string name = string());
      bool exploreStateSpace();
      tps_container findIrredundantTPs() const;
      pair<int, int> violations(bool considerColoring = false) const;
      int coloredState(int color) const;
      string stateVal(const string& enabling) const;
      bool isOutput(sv_event_index e) const
	{ return design->events[e]->signal >= design->ninpsig; }
      bool isInput(sv_event_index e) const
	{ return design->events[e]->signal < design->ninpsig; }

      state_list intersectRegions(const sv_event_index_cont& events,
				  const state_list_cont& regionStates) const;

      state_list unionRegions(const sv_event_index_cont& events,
			      const state_list_cont& regionStates) const;

      int getColor(const stateADT state) const;

      void setColor(stateADT state, int color = COLOR_BLANK);

      bool colorStateGraph(const state_list& riseStart,
			   const state_list& riseEnd,
			   const state_list& fallStart,
			   const state_list& fallEnd);

 private:
      void rFindIrredundantTPs(sv_event_index_cont avail_events,
			       svTP tp, tps_container& TPs) const;

      template<class back_insert_iter>
	bool findAllRegions(back_insert_iter bii_er, back_insert_iter bii_sr);
      
      string nextCSCVariableName();

      bool findER(int eventIndex, back_insert_iterator<state_list> bii_ER);

      bool findSR(int eventIndex, back_insert_iterator<state_list> bii_SR);

      void generateColorMask();

      void clearColors(int color=COLOR_BLANK, bool reset=false);

      static int colorVal(int color);

      static string stateVal(string& enabling);

      bool colorLow(state_list& fallRegion);

      bool colorHigh(state_list& riseRegion);

      bool colorFall(const state_list& startStates,
		     const state_list& endStates,
		     state_list& seedLow);

      bool colorRise(const state_list& startStates,
		     const state_list& endStates,
		     state_list& highSeed);

};
typedef vector<svSystem> systems_container;

////////////////////////////////////////////////////////////////////////////////
class svSolution : public svSystem
{
   private:
      svSolution();

   public:
      svSolution(const svSystem& s);
      unsigned int cost() const;
      bool operator< (const svSolution& o) { return cost() < o.cost(); }
};
typedef vector<svSolution> solutions_container;


// non-class-specific templatized helper function

////////////////////////////////////////////////////////////////////////////////
// requires that operator< be defined on Obj and Cont be an STL container
////////////////////////////////////////////////////////////////////////////////
template< class Obj, class Cont >
void
#if !defined(OSX) && !defined(NT) 
ranked_insert(const Obj& item, Cont& container, size_t max_size = MAXINT)
#else
ranked_insert(const Obj& item, Cont& container, size_t max_size = INT_MAX)
#endif
{
   typename Cont::iterator iLB = lower_bound(container.begin(), container.end(), item);
   if (container.size() < max_size)
      container.insert(iLB, item);
   else if (iLB != container.end())
   {
      container.insert(iLB, item);
      container.erase(container.rbegin().base());
   }
}


#endif







