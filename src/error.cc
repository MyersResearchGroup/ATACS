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

#include "error.h"
//////////////////////////////////////////////////////////////////////////
//
const char error::default_message[28] = "ERROR: No supplied message.";

error::error() : message() {
  // NOTHING TO DO
}

error::error( char* ptr_message ) : message() {
  message << ptr_message << ends;
}

error::error( const string& s ) : message() {
  message << s << ends;
}

void error::print_message( FILE* fp ) {
  if (!message.str().empty()) {
    printf( "%s\n", message.str().c_str() );
    if ( fp != NULL ) {
      fprintf( fp, "%s\n", message.str().c_str() );
    }
  }
}

error::~error() {
  // NOTHING TO DO
}
//
//////////////////////////////////////////////////////////////////////////  

//////////////////////////////////////////////////////////////////////////
//
const char file_error::error_header[28] = "ERROR:  Cannot open file '";
const char file_error::reading[15] = "' for reading.";
const char file_error::writing[15] = "' for writing.";
const char file_error::reading_writing[27] = "' for reading and writing!";

file_error::file_error( char* fname, const file_mode& mode ) : error() {
  // Copy in the contents of the message.
  message << error_header;
  if ( fname != NULL ) message << fname;
  switch( mode ) {
  case read: message <<  reading; break;
  case write: message <<  writing; break;
  case read_write: message <<  reading_writing; break;
  }
  message << ends;
}
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
const char tel_error::header[18] = "ERROR:  TEL for '";
const char tel_error::footer[32] = "' does not seem to be complete!";

tel_error::tel_error( char* fname ) : error() {
  message << header << fname << footer << endl << ends;
}
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//
marking_error::marking_error( ) : error( ) {
  // NOTHING TO DO
}
//
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
deadlock_error::deadlock_error( const tel& t, const timed_state& timed_st ) 
  : error() {
  message << "ERROR: System deadlocked" << endl;
  timed_st.dump( message, t ) << ends;
}
//
//////////////////////////////////////////////////////////////////////////
