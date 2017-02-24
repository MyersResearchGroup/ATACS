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
#include <sys/types.h>
#include <signal.h>
#include <cassert>
#include <unistd.h>
#include "signal_handler.h"

///////////////////////////////////////////////////////////////////////////////
// This is really a simple interface function that is only required to call
// the gCtrl_C_mngr.raise() function, which will take care of returning 
// control to the location of the last call of sigsetjmp(...).  If there is
// no call to sigsetjmp(...), then the old signal handler will be raised by
// gCtrl_C_mngr.raise().
// NOTE: When other signal_manager are created, follow this function for a
// pattern.
///////////////////////////////////////////////////////////////////////////////
static void control_C_signal_handler( int ) {
  gCtrl_C_mngr.raise();
}

///////////////////////////////////////////////////////////////////////////////
// gCtrl_C_mngr is the interrupt stack for the ^C SIGINT signal.  This should
// be used by any program wishing to use SIGINT.  The usage of this object 
// is explained in the class definition.
///////////////////////////////////////////////////////////////////////////////
signal_manager gCtrl_C_mngr( SIGINT, 
			     control_C_signal_handler );

///////////////////////////////////////////////////////////////////////////////
// base construction where signum is the value of the interupt to manage,
// and signal_hander is the function used as the handler.  Both arguments
// should be very simply.  See void control_C_signal_handler( int ) in 
// and gControl_C_mngr in signal_handler.cpp for an example of how the
// constructor is used.
///////////////////////////////////////////////////////////////////////////////
signal_manager::signal_manager( int signum, signal_handler s ) 
  : m_context(),
    m_signum( signum ) {
  assert( signum );
  m_old_handler = signal( signum, s );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor, reinstalls the old signal handler.
///////////////////////////////////////////////////////////////////////////////
signal_manager::~signal_manager() {
  signal( m_signum, m_old_handler );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a reference to a new entry on the stack.  This is used with any
// call to the sigsetjmp( .. ) function.  sigsetjmp() must be used so future
// interupts are saved in the process block.
///////////////////////////////////////////////////////////////////////////////
jmp_buf& signal_manager::new_entry() {
  m_context.push( jmp_buf_shell() );
  return( (m_context.top()).m_jmp );
}

///////////////////////////////////////////////////////////////////////////////
// raise() is called by the actual interupt handler to deal with an signal.  
// This function does the following things:
//   1) If the stack is empty
//        a) Install the old signal handler
//        b) ::raise( signum ) ( Through the signal ).
//   2) Stack isn't empty so pop of the top of the stack.
//   3) Return program control to the jmp_buf from the stack with
//      signum as the return value.
// siglongjmp(..) is used to preserve other interupt calls.
///////////////////////////////////////////////////////////////////////////////
void signal_manager::raise() {
  if ( m_context.empty() ) {
    signal( m_signum, m_old_handler );
    kill( getpid(), m_signum );
    return;
  }
  jmp_buf_shell j = m_context.top();
  //siglongjmp( j.m_jmp, m_signum );
}

///////////////////////////////////////////////////////////////////////////////
// Pop the top entry off the stack.
// NOTE: Must always be called before leaving a function that has used
// sigsetjmp( signal_handler::new_entry() );
///////////////////////////////////////////////////////////////////////////////
void signal_manager::pop() {
  if ( m_context.empty() )
    return;
  m_context.pop();
}
