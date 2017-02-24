#ifndef _hpnsim_h_
#define _hpnsim_h_


/***************************************************************************/
/* The overall state of the net <m, c, x>                                  */
/***************************************************************************/
typedef struct hpn_state_struct {
  int* m;      // marking of each discrete place.  Indexed by markkey index.
  double* c;   // Clock on each arc.  Indexed by markkey index.
  double* x;   // Marking of continuous places.  Indexed by place #.

  // Extra stuff which isn't essential but makes things convenient
  // and prevents a lot of extra memory allocation/deallocation.
  double tau;  // The current clock value.
  double* v;   // Current velocity of continuous transitions.
  bool* v_cur; // Indicates if the value in v is current
  
} *hpnStateADT;

void hpn_simulate(designADT design);
void start_sim(designADT design, hpnStateADT &state, FILE* &simlog);
void stop_sim(designADT design, hpnStateADT state, FILE* simlog);
void print_help();
void interactive(designADT design, hpnStateADT state, FILE* sl);
void run_hpn_sim(designADT design, hpnStateADT state, FILE* sl,
                 double maxtime, int steps, int esteps);
bool con_place(designADT design, int p);
bool dis_place(designADT design, int p);
bool con_trans(designADT design, int t);
bool dis_trans(designADT design, int t);
hpnStateADT initialize_state(designADT design);
void print_headers(designADT design, hpnStateADT state, FILE* sl);
void print_state(designADT design, hpnStateADT state, FILE* sl);
void print_all_state(designADT design, hpnStateADT state);
void view_net(designADT design, hpnStateADT state);
void destroy(hpnStateADT);
bool dis_enabled(designADT design, hpnStateADT state, int t);
bool enabled(designADT design, hpnStateADT state, int t);
bool can_fire(designADT design, hpnStateADT state, int t);
bool must_fire(designADT design, hpnStateADT state, int t);
void fire_T(designADT design, hpnStateADT state, int t);
void fire_tau(designADT design, hpnStateADT state, double tau);
double xdot(designADT design, hpnStateADT state, int p);
double velocity(designADT design, hpnStateADT state, int t);
double velocity(designADT design, hpnStateADT state, int t, bool* visited);
void reset_velocity(designADT design, hpnStateADT state);
double tau(designADT design, hpnStateADT state);
double tau_min(designADT design, hpnStateADT state);
double tau_dis_max(designADT design, hpnStateADT state);
double tau_con_max(designADT design, hpnStateADT state);
double tau_max(designADT design, hpnStateADT state);

#endif 
