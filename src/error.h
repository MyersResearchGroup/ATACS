//////////////////////////////////////////////////////////////////////////
// @name error.h
// @version 0.1 alpha
//
// (c)opyright 2000 by Eric G. Mercer
//
// @author Eric G. Mercer
//
// NOTE: This a general error class that can be used to throw and 
// catch expressions. See bap.cc for examples of its usage.
//////////////////////////////////////////////////////////////////////////
#ifndef __ERROR_H__
#define __ERROR_H__

#include <sstream>
#include <string>
#include <stdio.h>

#include "telwrapper.h"
#include "bap.h"

//-------------------------------------------------------------class errror
// NOTE: when using osstream, it is important to terminate the string
// with a NULL character.  This can be done using the 'ends' symbols. 
// Example:  message << "Message text" << ends;
class error {
protected:
  static const char default_message[];
  ostringstream message;
public:
  error();
  // ASSUMES ptr_message is NULL terminated
  error( char* ptr_message );
  error( const string& s );
  virtual void print_message( FILE* fp );
  virtual ~error();
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
class file_error : public error {
protected:
  static const char error_header[];
  static const char reading[];
  static const char writing[];
  static const char reading_writing[];
public:
  enum file_mode {read, write, read_write};
public:
  file_error( char* fname, const file_mode& mode );
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
class tel_error : public error {
protected:
  static const char header[];
  static const char footer[];
public:
  tel_error( char* fname );
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
class marking_error : public error {
public:
  marking_error( );
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
class deadlock_error : public error {
public:
  deadlock_error( const tel& t, const timed_state& timed_st );
};
//
//////////////////////////////////////////////////////////////////////////
#endif
