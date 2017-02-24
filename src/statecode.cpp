#ifdef NEW_CSC
// statecode.cpp
// code for state coding version 2

#include "statecode.h"

// STL
#include <functional>
#include <algorithm>

bool
find_state_code(char command, designADT design)
{
   // do some atacs integration checking
   
   // don't try to handle RSG specs
   if (design->fromRSG) {
      cerr << "ERROR: Cannot use state assignment on RSG specifications.\n";
      fprintf(lg,"ERROR: Cannot use state assignment on RSG specifications.\n"); 
      return false;
   }

   // make sure the rsg has been found
   stateADT curstate;
   bool state_graph_present(false);
   for (int i=0; i < PRIME && !state_graph_present; i++)
   {
      for (curstate=design->state_table[i];
	   curstate != NULL && curstate->state != NULL;
	   curstate=curstate->link)
      { 
        state_graph_present=true;
	break;
      }
  }
  if ( ! state_graph_present )
  {
     cerr << "findsv: No state graph present" << endl;
     return false;
  }

  // TODO make sure CSC hasn't already been achieved

  //
  // with the atacs preliminaries out of the way, get down to business
  //
  
  solutions_container solutions;
  systems_container systems;
  systems_container modified_systems;
  // TODO HARDCODE the max progress counter should be changeable by the user
  int non_progress_levels_remaining = SV_MAX_NON_PROGRESS_LVLS;
  
  // take the initial designADT and state info
  // and package it into an svSystem object
  svSystem original_system(design);
  
  // prime the systems list with the initial system
  systems.push_back(original_system);
  
  // while there's no solution, but still hope ...
  while (solutions.empty() && non_progress_levels_remaining > 0)
  {
     // fill container with valid insertion points
     // the majority of work is done here, as this function
     // generates TP's, combines them into IP's,
     // colors the graph, finds witness transitions, etc.
     ips_container IPs = find_IPs(systems);
     
     // print out some statistics
     cerr << "\n===================================================" << endl;
     cerr << "Number of valid IPs found: " << IPs.size() << endl;

     // debug dump of all IPs
     copy(IPs.begin(), IPs.end(), ostream_iterator<svIP>(cerr, "\n"));
     cerr << "\n===================================================" << endl;

     // actually do state space exploration of the most promising IPs
     // this function updates "systems" to the next generation
     // and updates "solutions" if any are found
     bool progress = explore_IPs(IPs, systems, solutions);

     // update the non_progress counter
     non_progress_levels_remaining =
	progress ? SV_MAX_NON_PROGRESS_LVLS : non_progress_levels_remaining - 1;
  }
  
  // if solutions were found, pick the best one
  if ( ! solutions.empty() )
  {
     // ramrod the solution into atacs
      solutions.begin()->install();

      cerr << "\n\n\n\n\n\n\nCSC Solved\n\n\n\n";
      
      return true;
  }
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////
ips_container find_IPs(systems_container& systems)
{
   ips_container IPs;
   
   // for each system given
   for (systems_container::iterator iSys = systems.begin();
	iSys != systems.end(); iSys++)
   {
      // get all the good TP's
      tps_container TPs = iSys->findIrredundantTPs();

      cerr << "Number of irredundant TPs: " << TPs.size() << endl;
       for (tps_container::iterator iTP = TPs.begin(); iTP != TPs.end(); iTP++)
       {
 	 cerr << *iTP;
       }

      // and do an all pairs combination of them
      for (tps_container::iterator iTPa = TPs.begin(); iTPa != TPs.end(); iTPa++)
      {
	 for (tps_container::iterator iTPb = iTPa+1; iTPb != TPs.end(); iTPb++)
	 {
	   svIP ip(&(*iSys), *iTPa, *iTPb);
	    // if the IP (combo of TP's) is valid, keep it
	    if ( ip.compatible() && ip.consistent() )
	    {
	      // capture the color of the initial state, so we can set the initial
	      // value of a new state signal
	      int initColor = ip.system()->findInitialColor();
	      if (initColor == COLOR_1 || initColor == COLOR_F)
		ip.setInitialSignalValue('1');
	      else if (initColor == COLOR_0 || initColor == COLOR_R)
		ip.setInitialSignalValue('0');
	      else
		ip.setInitialSignalValue('?');

	      // capture the colored cost of this IP,
	      // while the state graph and coloring are still available
	      ip.cost();

	      ranked_insert(ip, IPs, SV_MAX_KEPT_IPS);
	    }
	 }
      }

      cerr << "Checking that best IP is in best slot: " << endl;
      cerr << "Best IP is " << IPs.front() << endl;
      cerr << "Worst IP is " << IPs.back() << endl;

   }

   return IPs;
}

////////////////////////////////////////////////////////////////////////////////
// The return code of this function means "progress", so if false, none of the 
// systems has fewer CSC violations then before we tried to insert this variable.
// True means that at least one system got better, so there is some hope. Note
// that sometimes a system can get stuck for a variable or two before the progress
// becomes clear. Thus we track this with non_progress_levels, so that we don't
// give up too soon, nor wait around forever in a system that can't be solved.
// The solultions are returned in the solutions container, via side effect
////////////////////////////////////////////////////////////////////////////////
bool explore_IPs(const ips_container& IPs,
		  systems_container& systems,
		  solutions_container& solutions)
{
   bool progress(false);
   
   systems_container next_systems;
   
   // walk through the best IPs, trying each one
   // TODO since these are ranked, we could bail after a working solution
   // right now we try them all, so it doesn't matter TODO
   for (ips_container::const_iterator iIP = IPs.begin(); iIP != IPs.end(); iIP++)
   {
      // copy the initial system (because other IP's may refer back to it)
      svSystem sys(*(iIP->system()));

      pair<int, int> viols_before = sys.violations();

      cerr << "exploreIPs main loop >>> pre-violations count is "
	   << viols_before.first << " " << viols_before.second << endl;

      sys.insertStateVar(*iIP); // insert state var into CTextTel
      sys.install();            // load this svSystem into atacs land
      sys.exploreStateSpace();  // do real state space exploration
      pair<int, int> viols_now = sys.violations();

      cerr << "exploreIPs main loop >>> post-violations count is "
	   << viols_now.first << " " << viols_now.second << endl;

      ranked_insert(sys, next_systems, SV_MAX_KEPT_SYS);


      if (viols_now.first + viols_now.second == 0)
	{
	ranked_insert(svSolution(sys), solutions);

	cerr << "\n\n\nWOOWOO Found a solution with:\n" <<*iIP << endl;
	}

      // if fewer violations, progress has been made
      // TODO should this differentiate between natural and caused?
      if (viols_now.first + viols_now.second 
	  < viols_before.first + viols_before.second)
      {
	 progress = true;
	 // if no violations, a solution has been found
	 if (viols_now.first + viols_now.second == 0)
	    ranked_insert(svSolution(sys), solutions);
      }
   }

   systems = next_systems;

   return progress;
}
#endif
