#ifdef NEW_CSC
// statecode.h
// header for state coding algorithm version 2

// atacs
#include "struct.h"
#include "svclasses.h"

// C++/STL
#include <vector>
#include <list>

// consts
static const int SV_MAX_NON_PROGRESS_LVLS = 1; // should be around 3, set to 1 for testing
static const int SV_MAX_KEPT_IPS = 30;
static const int SV_MAX_KEPT_SYS = 5;

// prototypes
bool find_state_code(char command, designADT design);

ips_container find_IPs(systems_container& systems);

bool explore_IPs(const ips_container& IPs,
		 systems_container& systems,
		 solutions_container& solutions);

#endif
