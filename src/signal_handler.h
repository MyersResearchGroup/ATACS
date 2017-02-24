///////////////////////////////////////////////////////////////////////////////
// @name Timed Asynchronous Circuit Optimization  
// @version 0.1 alpha
//
// (c)opyright 1997 by Eric G. Mercer
//
// @author Eric G. Mercer
//                    
// Permission to use, copy, modify and/or distribute, but not sell, this  
// software and its documentation for any purpose is hereby granted       
// without fee, subject to the following terms and conditions:            
//                                                                          
// 1. The above copyright notice and this permission notice must         
// appear in all copies of the software and related documentation.        
//                                                                          
// 2. The name of University of Utah may not be used in advertising or  
// publicity pertaining to distribution of the software without the       
// specific, prior written permission of Univsersity of Utah.
//                                                                          
// 3. This software may not be called "Taco" if it has been modified    
// in any way, without the specific prior written permission of           
// Eric G. Mercer                                
//                                                                          
// 4. THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY       
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.       
//                                                                          
// IN NO EVENT SHALL THE UNIVERSITY OF UTAH OR THE AUTHORS OF THIS 
// SOFTWARE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL 
// DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE  
// OR PERFORMANCE OF THIS SOFTWARE.                                            
///////////////////////////////////////////////////////////////////////////////
#ifndef _INTERNAL_SIGNAL_HANDLER_H
#define _INTERNAL_SIGNAL_HANDLER_H

#include <setjmp.h>
#include <vector>
#include <stack>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// This typedef is to help alleviate some of the crypto-bizarness of the 
// syntax required to use pointers to functions.  Any call to signal will now
// return something of type signal_handler, which for all intents and purposes,
// is a pointer to a function.
///////////////////////////////////////////////////////////////////////////////
typedef void (*signal_handler)(int);

///////////////////////////////////////////////////////////////////////////////
// gCtrl_C_mngr is the interrupt stack for the ^C SIGINT signal.  This should
// be used by any program wishing to use SIGINT.  The usage of this object 
// is explained in the class definition.
///////////////////////////////////////////////////////////////////////////////
class signal_manager;
extern signal_manager gCtrl_C_mngr;

///////////////////////////////////////////////////////////////////////////////
// signal_manager is a class used to manage an interupt stack.  This can be
// used to install a handler and stack manager for any type of standard 'C'
// signal. The class is used in the following way:
// 
// void foo( ... ) {
//    .
//    .
//    .
//    if ( sigsetjmp( signal_mangaer::new_entry() ) == 0 ) {
//      // Normal execution path 
//    }
//    else {
//      // Code to execute on an interupt
//    }
//    .
//    .
//    .
//    signal_manager::pop();
// }
//
// NOTE: The call to pop is very important, because it states to the 
// signal_manager that you have completed your function and do not wish to
// service any future interupts.  You must always pop the signal_manager 
// before leaving your function, even if you have serviced and interupt!
// For an actual example of how the class is used, see simtel.c or
// markov_chain_analysis.cpp.
///////////////////////////////////////////////////////////////////////////////
class signal_manager {
protected:

  /////////////////////////////////////////////////////////////////////////////
  // jmp_buf is used by sigsetjmp and siglongjmp to save all information 
  // necessary to restart executing code from the point of the call.
  // NOTE: The wrapper is required to get around the distiction between an
  // object[] and object* caused by jmp_buf.
  /////////////////////////////////////////////////////////////////////////////
  struct jmp_buf_shell {
    jmp_buf m_jmp;
  };

  typedef vector<jmp_buf_shell> context_list;

  /////////////////////////////////////////////////////////////////////////////
  // The interface to the context_list is through a stack, hence a vector is
  // used for memory managment and such, while a stack interface is used to
  // access it.
  /////////////////////////////////////////////////////////////////////////////
  typedef stack<jmp_buf_shell,context_list> context_stack;

  context_stack m_context;       // stack to hold context of program exec.

  signal_handler m_old_handler;  // Pointer to the old handler function

  int m_signum;                  // Type of interupt being managed

public:

  /////////////////////////////////////////////////////////////////////////////
  // base construction where signum is the value of the interupt to manage,
  // and signal_hander is the function used as the handler.  Both arguments
  // should be very simply.  See void control_C_signal_handler( int ) in 
  // and gControl_C_mngr in signal_handler.cpp for an example of how the
  // constructor is used.
  /////////////////////////////////////////////////////////////////////////////
  signal_manager( int signum, signal_handler s );

  /////////////////////////////////////////////////////////////////////////////
  // Destructor, reinstalls the old signal handler.
  /////////////////////////////////////////////////////////////////////////////
  ~signal_manager();

  /////////////////////////////////////////////////////////////////////////////
  // Returns a reference to a new entry on the stack.  This is used with any
  // call to the sigsetjmp( .. ) function.  sigsetjmp() must be used so future
  // interupts are saved in the process block.
  /////////////////////////////////////////////////////////////////////////////
  jmp_buf& new_entry();

  /////////////////////////////////////////////////////////////////////////////
  // raise() is called by the actual interupt handler to deal with an signal.  
  // This function does the following things:
  //   1) If the stack is empty
  //        a) Install the old signal handler
  //        b) ::raise( signum ) ( Through the signal ).
  //   2) Stack isn't empty so pop of the top of the stack.
  //   3) Return program control to the jmp_buf from the stack with
  //      signum as the return value.
  // siglongjmp(..) is used to preserve other interupt calls.
  /////////////////////////////////////////////////////////////////////////////
  void raise();

  /////////////////////////////////////////////////////////////////////////////
  // Pop the top entry off the stack.
  // NOTE: Must always be called before leaving a function that has used
  // sigsetjmp( signal_handler::new_entry() );
  /////////////////////////////////////////////////////////////////////////////
  void pop();

};

#endif // _INTERNAL_SIGNAL_HANDLER_H
