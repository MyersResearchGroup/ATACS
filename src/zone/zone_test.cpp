#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zone.h"

//////////////////////////////////////////////////////////////////////////
// Various commands for the command interpreter.
//////////////////////////////////////////////////////////////////////////
static const char _LOAD_[2] = "l";
static const char _SAVE_[2] = "s";
static const char _VIEW_[2] = "v";
static const char _DELETE_[2] = "d";
static const char _ADD_[2] = "a";
static const char _RECANON_[2] = "c";
static const char _CHANGE_[2] = "m";
static const char _WARP_[2] = "w";
static const char _REVERSE_[2] = "r";
static const char _PRINT_[2] = "p";
static const char _UNDO_[2] = "u";
static const char _HELP_[2] = "h";
static const char _QUIT_[2] = "q";
static const char _SWAP_[2] = "t";

void print_help() {
  fprintf( stdout, "Zone Mangler v1.0 compiled %s %s\n", __DATE__, __TIME__ );
  fprintf( stdout, "Written by Eric G Mercer (c)opyright 2000\n");
  fprintf( stdout, "  load -> %s <filename>\n", _LOAD_ );
  fprintf( stdout, "  save -> %s <filename>\n", _SAVE_ );
  fprintf( stdout, "  view -> %s i,j\n", _VIEW_ );
  fprintf( stdout, "  delete clock -> %s i\n", _DELETE_ );
  fprintf( stdout, "  add clock -> %s i (l,u)\n",  _ADD_ );
  fprintf( stdout, "  cannonacalize -> %s\n", _RECANON_ );
  fprintf( stdout, "  modify value -> %s (i,j) value\n", _CHANGE_ );
  fprintf( stdout, "  warp zone -> %s (i,n) (j,m)\n", _WARP_ );
  fprintf( stdout, "  reverse clock -> %s i\n", _REVERSE_ );
  fprintf( stdout, "  swap rows -> %s i j\n", _SWAP_ );
  fprintf( stdout, "  print zone -> %s\n", _PRINT_ );
  fprintf( stdout, "  undo -> %s\n", _UNDO_ );
  fprintf( stdout, "  help -> %s\n", _HELP_ );
  fprintf( stdout, "  quit -> %s\n", _QUIT_ );
}

void command_interpreter() {
  zone* ptr_z = new zone();
  char input[_MAX_LINE_];
  zone::index_type warp_list[_MAX_LINE_];
  int warp_vec[_MAX_LINE_];
  int warp_vecf[_MAX_LINE_];
  char* tmp = NULL;

  fprintf( stdout, "Zone Mangler v1.0 compiled %s %s\n",
           __DATE__, __TIME__ );
  fprintf( stdout, "Written by Eric G Mercer (c)opyright 2000\n" );
  while ( true ) {
    fprintf( stdout, "z> " );
    if ( fgets( input, _MAX_LINE_, stdin ) != NULL ) {
      if ( ( tmp = strtok( input, " \n\t" ) ) == NULL ) {
        continue;
      }
      if ( strcmp( tmp, _LOAD_ ) == 0 ) {
        if( ( tmp = strtok( NULL, " \n\t" ) ) == NULL ) {
          fprintf( stdout, "error: missing file name\n" );
          continue;
        }
        ptr_z->load( tmp );
      }
      else if ( strcmp( tmp, _UNDO_ ) == 0 ) {
        ptr_z->load( "tmp.txt" );
      }
      else if ( strcmp( tmp, _SAVE_ ) == 0 ) {
        if( ( tmp = strtok( NULL, " \n\t" ) ) == NULL ) {
          fprintf( stdout, "error: missing file name\n" );
          continue;
        }
        ptr_z->save( tmp );
      }
      else if ( strcmp( tmp, _VIEW_ ) == 0 ) {
        zone::index_type i,j;
        if( ( tmp = strtok( NULL, " \n\t(,)" ) ) != NULL ) {
          i = zone::make_index( tmp );
          if( ( tmp = strtok( NULL, " \n\t" ) ) == NULL ) {
            fprintf( stdout, "error: missing second dimension\n" );
            continue;
          }
          j = zone::make_index( tmp );
          ptr_z->view(i,j);
        } else {
          ptr_z->view();
        }
      }
      else if ( strcmp( tmp, _PRINT_ ) == 0 ) {
        ptr_z->fprintf( stdout );
      }
      else if ( strcmp( tmp, _RECANON_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        ptr_z->floyds();
      }
      else if ( strcmp( tmp, _CHANGE_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        if( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing indexes and value\n" );
          continue;
        }
        zone::index_type i = zone::make_index( tmp );
        if( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing index and value\n" );
          continue;
        }
        zone::index_type j = zone::make_index( tmp );
        if ( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing value\n" );
          continue;
        }
        zone::constraint_type v = zone::_MAX_VALUE_;
        if ( strcmp( tmp, zone::_UNBOUNDED_ ) != 0 ) {
          v = (zone::constraint_type)atoi( tmp ); 
        }
        ptr_z->set_value( i, j, v );
      }
      else if ( strcmp( tmp, _WARP_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        int size=0;
        while (( tmp = strtok( NULL, " \n\t(,)" ) ) != NULL ) {
          warp_list[size] = zone::make_index( tmp );
          if( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
            fprintf( stdout, "error: missing value\n" );
            continue;
          }
          warp_vec[size] = atoi( tmp ); 
          if (strchr(tmp,'/'))
            warp_vecf[size] = atoi( strchr(tmp,'/') + 1);
          else 
            warp_vecf[size] = 1;
          size++;
        }
        ptr_z->warp( warp_list, warp_vec, warp_vecf, size );
      }
      else if ( strcmp( tmp, _REVERSE_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        if ( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing index\n" );
          continue;
        }
        zone::index_type ident = zone::make_index( tmp );
        ptr_z->reverse( ident );
      }
      else if ( strcmp( tmp, _SWAP_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        if ( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing index\n" );
          continue;
        }
        zone::index_type identI = zone::make_index( tmp );
        if ( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing index\n" );
          continue;
        }
        zone::index_type identJ = zone::make_index( tmp );
        ptr_z->swap_col( identI, identJ );
      }
      else if ( strcmp( tmp, _ADD_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        if ( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing new index\n" );
          continue;
        }
        zone::index_type ident = zone::make_index( tmp );
        zone::constraint_type l;
        zone::constraint_type u = zone::_MAX_VALUE_;
        if( ( tmp = strtok( NULL, " \n\t(,)" ) ) != NULL ) {
          l = (zone::constraint_type)atoi( tmp ); 
          if( ( tmp = strtok( NULL, " \n\t" ) ) == NULL ) {
            fprintf( stdout, "error: missing value\n" );
            continue;
          }
          if ( strcmp( tmp, zone::_UNBOUNDED_ ) != 0 ) {
            u = (zone::constraint_type)atoi( tmp ); 
          }
          ptr_z->add( ident );
          ptr_z->set_min( ident, l );
          ptr_z->set_max( ident, u );
          ptr_z->floyds();
        } else {
          ptr_z->add( ident );
        }
      }
      else if ( strcmp( tmp, _DELETE_ ) == 0 ) {
        ptr_z->save( "tmp.txt" );
        if ( ( tmp = strtok( NULL, " \n\t(,)" ) ) == NULL ) {
          fprintf( stdout, "error: missing index\n" );
          continue;
        }
        zone::index_type ident = zone::make_index( tmp );
        ptr_z->remove( ident );
      }
      else if ( strcmp( tmp, _HELP_ ) == 0 ) {
        print_help();
      }
      else if ( strcmp( tmp, _QUIT_ ) == 0 ) {
        delete( ptr_z );
        return;
      }
      else {
        fprintf(stdout,"error: unknown command '%s', try 'h' for help\n",tmp);
      }
    }
  }
}

int main(int argc, char **argv) {
  if ( argc == 2 ) {
    zone d( argv[1] );
    fprintf( stdout, "Input DBM: \n" );
    d.fprintf( stdout );
    fprintf( stdout, "Canonical DBM: \n" );
    d.floyds();
    d.fprintf( stdout );
    exit( 0 );
  }
  command_interpreter();
  exit(0);
}

