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
#ifndef __INTERNAL_CPU_TIME_TRACKER_H
#define __INTERNAL_CPU_TIME_TRACKER_H

#include <utility>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cassert>
#include <stdio.h>

#ifndef OSX
#include <malloc.h>
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// This is list an operator= for the rusage struct defined in the file
// /usr/include/resourcebits.h (see man getrusage).  This simply does a 
// member by member assignment of b to a ( i.e. a = b ).
///////////////////////////////////////////////////////////////////////////////
void assignment( rusage& a, const rusage& b );

/////////////////////////////////////////////////////////////////////////////
// User interface for 'C' programmers
// This is a struct with two doubles in it.  You can get at the doubles
// by using the timestruct.first and time_struct.second.  first is the
// user time and second is the system time both reported in seconds.
///////////////////////////////////////////////////////////////////////////////
typedef pair<double,double> time_struct;

///////////////////////////////////////////////////////////////////////////////
// Tracks the amount of time that is spent running user code ( i.e. working on
// 'this' process ) and running system code.  This is done by using the
// getrusage() function.  NOTE: This only reports user and system time.  
// Although this doesn't say anything about memory usage, it can be modified
// to report a whole slew of memory and other system information.
// This can be done in two ways: 1) Inherit and redefine the serializeOut 
// function to report and caculate the new statistics.  2) Add more observer
// functions that return the values of other statistics.
///////////////////////////////////////////////////////////////////////////////
class cpu_time_tracker {
protected:
  
  rusage* m_begin;  // Start time
  
  rusage* m_mark;   // End time or marker

public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor which initializes all class members.
  /////////////////////////////////////////////////////////////////////////////
  cpu_time_tracker();

  /////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////
  cpu_time_tracker( const cpu_time_tracker& r );

  /////////////////////////////////////////////////////////////////////////////
  // Destructor, deletes memory held by m_begin and m_mark.
  /////////////////////////////////////////////////////////////////////////////
  virtual ~cpu_time_tracker();

  /////////////////////////////////////////////////////////////////////////////
  // Assignment operator
  /////////////////////////////////////////////////////////////////////////////
  cpu_time_tracker& operator=( const cpu_time_tracker& r );

  /////////////////////////////////////////////////////////////////////////////
  // touch() will reset the m_begin values with a call to getrusage. The return
  // value is the return value from getrusage() ( Means nothing to me. If I had
  // to0 guess, -1 would mean it failed and anything else is OK.
  /////////////////////////////////////////////////////////////////////////////
  virtual int touch();

  /////////////////////////////////////////////////////////////////////////////
  // mark() makes a call to getrusage() to set the values in m_mark.  This is
  // a 'distance' marker if you are measuring time.  If you are measuring other
  // system resources, its depends on how you use m_mark.  Either way, it makes
  // a call to getrusage() with m_mark in the parameter and returns the value
  // of the call.
  /////////////////////////////////////////////////////////////////////////////
  virtual int mark();

  /////////////////////////////////////////////////////////////////////////////
  // 'C' Style print function
  /////////////////////////////////////////////////////////////////////////////
  virtual void print( FILE* fp ) const;

  /////////////////////////////////////////////////////////////////////////////
  // Calculates the elapsed system and user time that has passed between 
  // m_begin and m_mark.  ( If you don't call mark() before this function, the
  // time isn't going to mean anything.  Either it will be zero or the value 
  // from the last time you called mark.  time_struct.first is the
  // user time and time_struct.second is the system time.
  // NOTE: All time is reported in seconds.
  /////////////////////////////////////////////////////////////////////////////
  time_struct resource_time() const;
  
  /////////////////////////////////////////////////////////////////////////////
  // serializeOut() puts a textual representation of 'this' object into the 
  // ostream s.  I made this function virtual so it could be redefined in 
  // subclasses to report other statistics such as memory usage, page swaps,
  // etc.  Currently it just outputs the user time and system time that has
  // elapsed between m_begin and m_mark.  ( Both can be set with touch() and
  // mark() ).  Output is as follows: d.ddu d.dds, where the d's represent
  // digits, the u denotes user time, and the s denotes system time. User time
  // is time spent executing 'this' process.  NOTE: All time is reported in
  // seconds.
  /////////////////////////////////////////////////////////////////////////////
  virtual ostream& serializeOut( ostream& s ) const;
  
  /////////////////////////////////////////////////////////////////////////////
  // operator<< simply makes a call to c.serializeOut( s ).  This is here
  // to support the virtual function serializeOut.  Only serializeOut needs 
  // to be rewritten in a subclass to make the subclass work with the << 
  // operator.  
  /////////////////////////////////////////////////////////////////////////////
  friend ostream& operator<<( ostream& s, const cpu_time_tracker& c );

};

//////////////////////////////////////////////////////////////////////////
// This class reports max memory usage as well as the time usage
//////////////////////////////////////////////////////////////////////////
//

// NOTE: mallinfo is used instead of rusage because memory numbers in 
// rusage are not implemented yet (i.e., always 0).
#ifdef MEMSTATS
#ifndef OSX
void copy( const struct mallinfo& a, const struct mallinfo& b );
#endif
#endif

class cpu_time_memory_tracker : public cpu_time_tracker {
protected:

  // NOTE: This should be covered in the rusage struct from getrusage.
  // However, it does not seem to record memeory statistics, thus as
  // a temporary fix, I am using mallinfo to record memory activities.
#ifdef MEMSTATS
#ifndef OSX
  struct mallinfo m_begin_mem;  
  struct mallinfo m_mark_mem;
#endif
#endif

protected:

  int allocated_memory() const;

public:

  ////////////////////////////////////////////////////////////////////////
  // Constructor which initializes all class members.
  ////////////////////////////////////////////////////////////////////////
  cpu_time_memory_tracker();

  ////////////////////////////////////////////////////////////////////////
  // Copy constructor
  ////////////////////////////////////////////////////////////////////////
  cpu_time_memory_tracker( const cpu_time_memory_tracker& r );

  ////////////////////////////////////////////////////////////////////////
  // Destructor, deletes memory held by m_begin and m_mark.
  ////////////////////////////////////////////////////////////////////////
  virtual ~cpu_time_memory_tracker();

  ////////////////////////////////////////////////////////////////////////
  // Assignment operator
  ////////////////////////////////////////////////////////////////////////
  cpu_time_memory_tracker& operator=( const cpu_time_memory_tracker& r );

  virtual int touch();

  virtual int mark();

  ////////////////////////////////////////////////////////////////////////
  // 'C' Style print function
  /////////////////////////////////////////////////////////////////////////
  virtual void print( FILE* fp ) const;
  
  ////////////////////////////////////////////////////////////////////////
  // serializeOut() puts a textual representation of 'this' object into  
  // the ostream s. 
  ////////////////////////////////////////////////////////////////////////
  virtual ostream& serializeOut( ostream& s ) const;

  ostream& report_memory( ostream& s ) const;

  int max_memory() const;

};

//
//////////////////////////////////////////////////////////////////////////

#endif //__INTERNAL_CPU_TIME_TRACKER_H
