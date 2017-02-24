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

#include "cpu_time_tracker.h"

/*
struct mallinfo {
  long arena;
  long ordblks;
  long smblks;
  long hblks;
  long hblkhd;
  long usmblks;
  long fsmblks;
  long uordblks;
  long fordblks;
  long keepcost;
  };*/  

///////////////////////////////////////////////////////////////////////////////
// This is list an operator= for the rusage struct defined in the file
// /usr/include/resourcebits.h (see man getrusage).  This simply does a 
// member by member assignment of b to a ( i.e. a = b ).
///////////////////////////////////////////////////////////////////////////////
void assignment( rusage& a, const rusage& b ) {
  if ( &a == &b ) return;
  a.ru_utime.tv_sec = b.ru_utime.tv_sec;  /* user time used */
  a.ru_utime.tv_usec = b.ru_utime.tv_usec;
  a.ru_stime.tv_sec = b.ru_stime.tv_sec;  /* system time used */
  a.ru_stime.tv_usec = b.ru_stime.tv_usec;
  a.ru_maxrss = b.ru_maxrss;              /* maximum resident set size */
  a.ru_ixrss = b.ru_ixrss;                /* integral shared memory size */
  a.ru_idrss = b.ru_idrss;                /* integral unshared data size */
  a.ru_isrss = b.ru_isrss;                /* integral unshared stack size */
  a.ru_minflt = b.ru_minflt;              /* page reclaims */
  a.ru_majflt = b.ru_majflt;              /* page faults */
  a.ru_nswap = b.ru_nswap;                /* swaps */
  a.ru_inblock = b.ru_inblock;            /* block input operations */
  a.ru_oublock = b.ru_oublock;            /* block output operations */
  a.ru_msgsnd = b.ru_msgsnd;              /* messages sent */
  a.ru_msgrcv = b.ru_msgrcv;              /* messages received */
  a.ru_nsignals = b.ru_nsignals;          /* signals received */
  a.ru_nvcsw = b.ru_nvcsw;                /* voluntary context switches */
}

///////////////////////////////////////////////////////////////////////////////
// Constructor which initializes all class members.
///////////////////////////////////////////////////////////////////////////////
cpu_time_tracker::cpu_time_tracker() 
  : m_begin( 0 ), m_mark( 0 ) {
  m_begin = new rusage();
  m_mark = new rusage();
  assert( m_begin != NULL && m_mark != NULL );
  getrusage( RUSAGE_SELF, m_begin );
  assignment( *m_mark, *m_begin );
}

///////////////////////////////////////////////////////////////////////////////
// Copy constructor
///////////////////////////////////////////////////////////////////////////////
cpu_time_tracker::cpu_time_tracker( const cpu_time_tracker& r ) {
  m_begin = new rusage();
  m_mark = new rusage();
  assert( m_begin != 0 && m_mark != 0 );
  assignment( *m_begin, *r.m_begin );
  assignment( *m_mark, *r.m_mark );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor, deletes memory held by m_begin and m_mark.
///////////////////////////////////////////////////////////////////////////////
cpu_time_tracker::~cpu_time_tracker() {
  delete m_begin;
  delete m_mark;
}

///////////////////////////////////////////////////////////////////////////////
// Assignment operator
///////////////////////////////////////////////////////////////////////////////
cpu_time_tracker& cpu_time_tracker::operator=( const cpu_time_tracker& r ) {
  if ( this == &r ) return( *this );
  assignment( *m_begin, *r.m_begin );
  assignment( *m_mark, *r.m_mark );
  return( *this );
}

///////////////////////////////////////////////////////////////////////////////
// touch() will reset the m_begin values with a call to getrusage. The return
// value is the return value from getrusage() ( Means nothing to me.  If I had
// too guess, -1 would mean it failed and anything else is OK.
///////////////////////////////////////////////////////////////////////////////
int cpu_time_tracker::touch() {
  int tmp = getrusage( RUSAGE_SELF, m_begin );
  assignment( *m_mark, *m_begin );
  return( tmp );
}

///////////////////////////////////////////////////////////////////////////////
// mark() makes a call to getrusage() to set the values in m_mark.  This is
// a 'distance' marker if you are measuring time.  If you are measuring other
// system resources, its depends on how you use m_mark.  Either way, it makes
// a call to getrusage() with m_mark in the parameter and returns the value
// of the call.
///////////////////////////////////////////////////////////////////////////////
int cpu_time_tracker::mark() {
  return( getrusage( RUSAGE_SELF, m_mark ) );
}

/////////////////////////////////////////////////////////////////////////////
// 'C' Style print function
/////////////////////////////////////////////////////////////////////////////
void cpu_time_tracker::print( FILE* fp ) const {
  time_struct tmp = resource_time();
  fprintf( fp, "%gu %gs", tmp.first, tmp.second );
}

///////////////////////////////////////////////////////////////////////////////
// Calculates the elapsed system and user time that has passed between 
// m_begin and m_mark.  ( If you don't call mark() before this function, the
// time isn't going to mean anything.  Either it will be zero or the value 
// from the last time you called mark.  time_struct.first is the
// user time and time_struct.second is the system time.
// NOTE: All time is reported in seconds.
///////////////////////////////////////////////////////////////////////////////
time_struct cpu_time_tracker::resource_time() const {
  time_struct time;
  time.first =  ( m_mark->ru_utime.tv_sec + 
		  ( m_mark->ru_utime.tv_usec * 0.000001 ) ) -
                ( m_begin->ru_utime.tv_sec + 
		  ( m_begin->ru_utime.tv_usec * 0.000001 ) );
  time.second = ( m_mark->ru_stime.tv_sec + 
		  ( m_mark->ru_stime.tv_usec * 0.000001 ) ) -
                ( m_begin->ru_stime.tv_sec + 
		  ( m_begin->ru_stime.tv_usec * 0.000001 ) );
  return( time );
}

///////////////////////////////////////////////////////////////////////////////
// serializeOut() puts a textual representation of 'this' object into the 
// ostream s.  I made this function virtual so it could be redefined in 
// subclasses to report other statistics such as memory usage, page swaps,
// etc.  Currently it just outputs the user time and system time that has
// elapsed between m_begin and m_mark.  ( Both can be set with touch() and
// mark() ).  Output is as follows: d.ddu d.dds, where the d's represent
// digits, the u denotes user time, and the s denotes system time. User time
// is time spent executing 'this' process.  NOTE: All time is reported in
// seconds.
///////////////////////////////////////////////////////////////////////////////
ostream& cpu_time_tracker::serializeOut( ostream& s ) const{
  time_struct tmp = resource_time();
  s << tmp.first << "u " << tmp.second << "s"; 
  return( s );
}

///////////////////////////////////////////////////////////////////////////////
// operator<< simply makes a call to c.serializeOut( s ).  This is here
// to support the virtual function serializeOut.  Only serializeOut needs 
// to be rewritten in a subclass to make the subclass work with the << 
// operator.  
///////////////////////////////////////////////////////////////////////////////
ostream& operator<<( ostream& s, const cpu_time_tracker& c ) {
  return( c.serializeOut( s ) );
}

//----------------------------------------------------------------------
// cpu_time_memory_tracker code

/////////////////////////////////////////////////////////////////////////
//
#ifdef MEMSTATS
#ifndef OSX
void copy( struct mallinfo& a, const struct mallinfo& b ) {
  if ( &a == &b ) return;
  a.arena = b.arena;        /* total space allocated from system */
  a.ordblks = b.ordblks;     /* number of non-inuse chunks */
  a.smblks = b.smblks;      /* unused -- always zero */
  a.hblks = b.hblks;        /* number of mmapped regions */
  a.hblkhd = b.hblkhd;      /* total space in mmapped regions */
  a.usmblks = b.usmblks;    /* unused -- always zero */
  a.fsmblks = b.fsmblks;    /* unused -- always zero */
  a.uordblks = b.uordblks;  /* total allocated space */
  a.fordblks = b.fordblks;  /* total non-inuse space */
  a.keepcost = b.keepcost; 
}
#endif 
#endif 

int cpu_time_memory_tracker::allocated_memory() const {
#ifdef MEMSTATS
#ifndef OSX
  return( m_mark_mem.uordblks - m_begin_mem.uordblks );
#endif
#endif
}

////////////////////////////////////////////////////////////////////////
// Constructor which initializes all class members.
////////////////////////////////////////////////////////////////////////
cpu_time_memory_tracker::cpu_time_memory_tracker() : 
  cpu_time_tracker()
#ifdef MEMSTATS
#ifndef OSX
  ,
  m_begin_mem(mallinfo()),
  m_mark_mem(mallinfo())
#endif 
#endif 
{
  // Get the initial memory usage profile and copy it into m_mark_mem;
}

////////////////////////////////////////////////////////////////////////
// Copy constructor
////////////////////////////////////////////////////////////////////////
cpu_time_memory_tracker::
cpu_time_memory_tracker( const cpu_time_memory_tracker& r ) : 
  cpu_time_tracker( (cpu_time_tracker( r ) ) )
#ifdef MEMSTATS
#ifndef OSX
  , m_begin_mem(r.m_begin_mem),
  m_mark_mem(r.m_mark_mem)
#endif
#endif
{
} 

////////////////////////////////////////////////////////////////////////
// Destructor, deletes memory held by m_begin and m_mark.
////////////////////////////////////////////////////////////////////////
cpu_time_memory_tracker::~cpu_time_memory_tracker() {
  // NOTHING TO DO
}

////////////////////////////////////////////////////////////////////////
// Assignment operator
////////////////////////////////////////////////////////////////////////
cpu_time_memory_tracker& cpu_time_memory_tracker::
operator=( const cpu_time_memory_tracker& r ) {
  if ( this == &r ) return( *this );
  (cpu_time_tracker)*this = (cpu_time_tracker)r;
#ifdef MEMSTATS
#ifndef OSX
  copy( m_begin_mem, r.m_begin_mem );
  copy( m_mark_mem, r.m_mark_mem );
#endif
#endif
  return( *this );
}

int cpu_time_memory_tracker::touch(){
  int tmp = cpu_time_tracker::touch();
#ifdef MEMSTATS
#ifndef OSX
  m_begin_mem = mallinfo();
  copy( m_mark_mem, m_begin_mem );
#endif
#endif
  return( tmp );
}

int cpu_time_memory_tracker::mark() {
  int tmp = cpu_time_tracker::mark();
#ifdef MEMSTATS
#ifndef OSX
  m_mark_mem = mallinfo();
#endif
#endif
  return( tmp );
}

/////////////////////////////////////////////////////////////////////////////
// 'C' Style print function
/////////////////////////////////////////////////////////////////////////////
void cpu_time_memory_tracker::print( FILE* fp ) const{
  fprintf( fp, "(%dm %da ", max_memory(), allocated_memory() );
  cpu_time_tracker::print( fp );
  fprintf( fp, ")" );
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
ostream& cpu_time_memory_tracker::serializeOut( ostream& s ) const {
  s << "(" << max_memory() << "m " << allocated_memory() << "a ";
  cpu_time_tracker::serializeOut( s );
  s << ")";
  return( s );
}

ostream& cpu_time_memory_tracker::report_memory( ostream& s ) const {
#ifdef MEMSTATS
#ifndef OSX
  s << "memory: max=" << m_mark_mem.arena
    << " inuse=" << m_mark_mem.uordblks
    << " free=" << m_mark_mem.fordblks;
#endif
#endif
  return( s );
}

int cpu_time_memory_tracker::max_memory() const {
#ifdef MEMSTATS
#ifndef OSX
  return( m_mark_mem.arena );
#endif
#endif
}

//
//////////////////////////////////////////////////////////////////////////
