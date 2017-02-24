#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "zone.h"

#define _LINE_SIZE_ 300

// This is used to specify an unbounded, undefined, or
// unconstrained relationship
const char zone::_UNBOUNDED_[2] = "U";
const zone::constraint_type zone::_MAX_VALUE_ = SHRT_MAX;

// This is the maximum number of allowed characters for a constraint or
// identifier.
const zone::size_type zone::_MAX_LENGTH_ = 10;

//////////////////////////////////////////////////////////////////////////
zone::constraint_type** zone::malloc_dbm( const size_type& dim ) {
  constraint_type** dbm = 
    (constraint_type**)malloc( dim*sizeof( constraint_ptr ));
  if ( dbm == NULL ) {
    ::fprintf( stdout, "error: malloc failed to allocate memory for dbm\n" );
    exit( 1 );
  }
  for( size_type i = 0 ; i < dim ; ++i ) {
    dbm[i] = (constraint_type*)malloc( dim * sizeof( constraint_type ) );
    if ( dbm[i] == NULL ) {
      ::fprintf(stdout,
                "error: malloc failed to allocate  memory for column\n");
      exit( 1 );
    }
    for ( size_type j = 0 ; j < dim ; ++j ) {
      if ( i == j ) {
        dbm[i][j] = 0;
      }
      else {
        dbm[i][j] = _MAX_VALUE_;
      }
    }
  }
  return dbm;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
zone::index_type* zone::malloc_ident( const size_type& dim ) {
  index_type* ident = new index_type[ dim ]; 
  if ( ident == NULL ) {
    ::fprintf( stdout, "error: new failed to allocate memory for ident\n" );
    exit( 1 );
  }
  return( ident );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::free_ident( index_type* ident ) {
  delete [] ident;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::free_dbm( constraint_type** dbm, size_type dim ) {
  for ( size_type i = 0 ; i < dim ; ++i ) {
    if ( dbm[i] != NULL ) {
      free( dbm[i] );
    }
  }
  free( dbm );
  dbm = NULL;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
zone::size_type zone::find( const index_type& ident ) const {
  size_type i = 0;
  for( ; i < m_dim && m_ident[i] != ident ; ++i );
  return( i );
}
//////////////////////////////////////////////////////////////////////////
  
//////////////////////////////////////////////////////////////////////////
zone::index_type zone::make_index( char* t ) {
  index_type tmp( t );
  if ( tmp.length() > (size_t)_MAX_LENGTH_ )
    return( index_type( tmp, 0, _MAX_LENGTH_ ) );
  return( tmp );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
zone::zone() : 
  m_dim( 0 ), 
  m_dbm( NULL ),
  m_ident( NULL ) {
  // NOTHING TO DO
}
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
zone::zone( const char* const fname ) : 
  m_dim( 0 ), 
  m_dbm( NULL ),
  m_ident( NULL ) {
  load( fname );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
zone::~zone(){
  if ( m_dbm != NULL ) {
    free_dbm( m_dbm, m_dim );
  }
  if ( m_ident != NULL ) {
    free_ident( m_ident );
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
bool zone::load( const char* const fname ) {
  // Delete the data in the now old m_dbm.
  if ( m_dbm != NULL ) {
    free_dbm( m_dbm, m_dim );
  }
  if ( m_ident != NULL ) {
    free_ident( m_ident );
  }
  // Check the file name.  
  if ( fname == NULL ) {
    ::fprintf( stdout, "error: NULL file name in load command.\n" );
    m_dim = 1;
    m_dbm = malloc_dbm( m_dim );
    m_ident = malloc_ident( m_dim );
    return( false );
  }
  // Open the file
  FILE* fp = fopen( fname, "r" );
  if ( fp == NULL ) {
    ::fprintf( stdout, "error: failed to open %s for reading.\n", fname );
    m_dim = 1;
    m_dbm = malloc_dbm( m_dim );
    m_ident = malloc_ident( m_dim );
    return( false );
  }
  // Allocate memory to read in the 
  char* s = (char*)malloc( _LINE_SIZE_ * sizeof( char ) );
  if ( s == NULL ) {
    ::fprintf( stdout, "error: malloc failed to allocate memory for input\n" );
    exit( 1 );
  }
  // Read in the dimension of the matrix.  If it is 0 or greater than
  // _MAX_VALUE_, then flag and make safe size m_dbm.  If dim is good, then
  // allocate memory.  Otherwise, bad input file and die.
  if ( fgets( s, _LINE_SIZE_, fp ) != NULL ) {
    m_dim = (size_type)atoi( s );
    if ( (int)atoi( s ) > (int)_MAX_VALUE_ || m_dim == 0 ) {
      ::fprintf(stdout, "error: bad dimension value %d.\n", m_dim);
      m_dim = 1;
      m_dbm = malloc_dbm( m_dim );
      m_ident = malloc_ident( m_dim );
      free( s );
      fclose( fp );
      return( false );
    }
    m_dbm = malloc_dbm( m_dim );
    m_ident = malloc_ident( m_dim );
  }
  else {
    ::fprintf(stdout,"error: bad input file\n" );
    m_dim = 1;
    m_dbm = malloc_dbm( m_dim );
    m_ident = malloc_ident( m_dim );
    free( s );
    fclose( fp );
    return( false );
  }
  // Read in each entry and set its value.
  for ( size_type i = 0 ; i < m_dim ; i++ ) {
    if ( fgets( s, _LINE_SIZE_, fp ) == NULL ) {
      ::fprintf(stdout,"error: bad input file\n" );
      free( s );
      fclose( fp );
      return( false );
    }
    char* tmp = strtok( s, " \n\t:|" );
    m_ident[i] = make_index( tmp );
    tmp = strtok( NULL, " \n\t:|" );
    for ( size_type j = 0 ;  j < m_dim ; ++j ) {
      if ( strcmp( tmp, _UNBOUNDED_ ) == 0 ) {
        m_dbm[i][j] = _MAX_VALUE_;
      }
      else {
        m_dbm[i][j] = (constraint_type)atoi( tmp );
      }
      tmp = strtok( NULL, " \n\t" );
    }
  }
  free ( s );
  fclose( fp );
  return( true );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
bool zone::save( const char* const fname ) {

  if ( m_dim == 0 ) {
    return( false );
  }
  FILE* fp = fopen( fname, "w" );
  if ( fp == NULL ) {
    ::fprintf( stdout, "error: failed to open %s for writing.\n", fname );
    return( false );
  }
  ::fprintf(fp,"%d\n",m_dim); 
  for ( size_type i = 0 ; i < m_dim ; i++ ) {
    ::fprintf( fp, "%s: ", (m_ident[i]).c_str() );
    for ( size_type j = 0 ; j < m_dim ; j++ ) {
      if ( m_dbm[i][j] == _MAX_VALUE_ ) {
        ::fprintf( fp, "%c ", 'U' );
      }
      else {
        ::fprintf( fp, "%d ", m_dbm[i][j] );
      }
    }
    ::fprintf( fp, "\n" );
  }
  fclose(fp);
  return( true );
}

//////////////////////////////////////////////////////////////////////////
bool zone::view() {

  if ( m_dim == 0 ) {
    ::fprintf( stdout, "error: cannot view 0 size zone.\n");
    return( false );
  } else if ( m_dim > 3 ) {
    ::fprintf( stdout, "error: cannot view %d size zone.\n",m_dim);
    return( false );
  } 
  FILE* fp = fopen( "tmp.txt", "w" );
  if ( fp == NULL ) {
    ::fprintf( stdout, "error: failed to open tmp.txt for writing.\n");
    return( false );
  }
  if ( m_dim == 1) {
    ::fprintf(fp,"3\n");
    ::fprintf(fp,"%s: 0 0 0\n",(m_ident[0]).c_str() );
    ::fprintf(fp,"null: 0 0 0\n");
    ::fprintf(fp,"null: 0 0 0\n");
  } else if ( m_dim == 2) {
    ::fprintf(fp,"3\n",m_dim); 
    for ( size_type i = 0 ; i < m_dim ; i++ ) {
      ::fprintf( fp, "%s: ", (m_ident[i]).c_str() );
      for ( size_type j = 0 ; j < m_dim ; j++ ) {
        if ( m_dbm[i][j] == _MAX_VALUE_ ) {
          ::fprintf( fp, "%c ", 'U' );
        }
        else {
          ::fprintf( fp, "%d ", m_dbm[i][j] );
        }
      }
      ::fprintf( fp, "%d\n", m_dbm[i][0] );
    }
    ::fprintf( fp, "null: 0 %d 0\n", m_dbm[0][1]);
  } else { 
    ::fprintf(fp,"%d\n",m_dim); 
    for ( size_type i = 0 ; i < m_dim ; i++ ) {
      ::fprintf( fp, "%s: ", (m_ident[i]).c_str() );
      for ( size_type j = 0 ; j < m_dim ; j++ ) {
        if ( m_dbm[i][j] == _MAX_VALUE_ ) {
          ::fprintf( fp, "%c ", 'U' );
        }
        else {
          ::fprintf( fp, "%d ", m_dbm[i][j] );
        }
      }
      ::fprintf( fp, "\n" );
    }
  }
  fclose(fp);
  system("zone2ps.pl < tmp.txt");
  return( true );
}

//////////////////////////////////////////////////////////////////////////
bool zone::view(const index_type& i, const index_type& j) {

  size_type index_i = find( i );
  size_type index_j = find( j );
  FILE* fp = fopen( "tmp.txt", "w" );
  if ( fp == NULL ) {
    ::fprintf( stdout, "error: failed to open tmp.txt for writing.\n");
    return( false );
  }
  ::fprintf(fp,"3\n",m_dim); 
  ::fprintf(fp,"%s: 0 %d %d\n",(m_ident[0]).c_str(),
            m_dbm[0][index_i],m_dbm[0][index_j]);
  ::fprintf(fp,"%s: %d 0 %d\n",(m_ident[index_i]).c_str(),
            m_dbm[index_i][0],m_dbm[index_i][index_j]);
  ::fprintf(fp,"%s: %d %d 0\n",(m_ident[index_j]).c_str(),
            m_dbm[index_j][0],m_dbm[index_j][index_i]);
  fclose(fp);
  system("zone2ps.pl < tmp.txt");
  return( true );
}

//////////////////////////////////////////////////////////////////////////
void zone::set_value( const index_type& i, const index_type& j, 
                      const constraint_type& v ) {
  size_type index_i = find( i );
  size_type index_j = find( j );
  if ( index_i == m_dim || index_j == m_dim ) {
    return;
  }
  m_dbm[index_i][index_j] = v;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::set_min( const index_type& i, const constraint_type& v ) {
  size_type index_i = find( i );
  if ( index_i == m_dim ) {
    return;
  }
  m_dbm[0][index_i] = -1 * v;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::set_max( const index_type& i, const constraint_type& v ) {
  size_type index_i = find( i );
  if ( index_i == m_dim ) {
    return;
  }
  m_dbm[index_i][0] = v;
}

/*****************************************************************************
* Do an integer divide with some extra checks and take the floor or
* ceil of the answer based on the value given in func.  The possible
* values are: 'F' = floor -- 'C' = ceiling
******************************************************************************/
int zone::chkDiv(int a,int b,char func)
{
  int res;
  if(a == INFIN ||
     a == INFIN * -1) {
    if(b < 0) {
      return a * -1;
    }
    return a;
  }
  if(b == INFIN) {
    return 0;
  }
  if(b == 0) {
#ifdef __HPN_WARN__
    cSetFg(RED);
    printf("Warning: ");
    cSetAttr(NONE);
    printf("divide by zero");
    printf(".\n");
#endif
    return 0;
  }

  double Dres,Da,Db;
  Da = a;
  Db = b;
  Dres = Da/Db;
  if(func == 'C') {
    res = (int)ceil(Dres);
  }
  else if(func == 'F') {
    res = (int)floor(Dres);
  }
  return res;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::warp( zone::index_type warp_list[_MAX_LINE_],
                 int warp_vec[_MAX_LINE_],
                 int warp_vecf[_MAX_LINE_],
                 int size ) {
  int n,nf,m,mf;
  size_type index_i,index_j;

  for (int i=0;i<size;i++) 
    for (int j=i+1;j<size;j++) {
      if ((warp_vec[i]*warp_vecf[j]) > (warp_vec[j]*warp_vecf[i])) {
        index_i = find( warp_list[i] );
        index_j = find( warp_list[j] );
        n = warp_vec[i];
        nf = warp_vecf[i];
        m = warp_vec[j];
        mf = warp_vecf[j];
      } else {
        index_i = find( warp_list[j] );
        index_j = find( warp_list[i] );
        n = warp_vec[j];
        nf = warp_vecf[j];
        m = warp_vec[i];
        mf = warp_vecf[i];
      }
      if ( index_i == m_dim || index_j == m_dim ) {
        continue;
      }
      //if (m_dbm[index_i][index_j] != m_dbm[0][index_j]) {
      /*
	m_dbm[index_i][index_j] = n * m_dbm[index_i][0] / nf 
	  - m * m_dbm[index_j][0] / mf
	  + m * m_dbm[index_j][index_i] / mf 
	  - m * m_dbm[0][index_i] / mf
	  + n * m_dbm[0][index_i] / mf;
      */
	  //	  + m * m_dbm[0][index_j] / mf;
	//} else {
	//m_dbm[index_i][index_j] = m * m_dbm[index_i][index_j] / mf; 
	//}
      m_dbm[index_i][index_j] = chkDiv(m * m_dbm[index_i][index_j],mf,'C') 
        - chkDiv(m * m_dbm[index_i][0],mf,'C') 
        + chkDiv(n * m_dbm[index_i][0],nf,'C');
      m_dbm[index_j][index_i] = chkDiv(m * m_dbm[index_j][index_i],mf,'C') 
        - chkDiv(m * m_dbm[0][index_i],mf,'C') 
        + chkDiv(n * m_dbm[0][index_i],nf,'C');
    }
  for (int i=0;i<size;i++) {
    size_type index_i = find( warp_list[i] );
    if ( index_i == m_dim ) {
      continue;
    }
    m_dbm[0][index_i] = chkDiv(warp_vec[i]*m_dbm[0][index_i],warp_vecf[i],'C');
    m_dbm[index_i][0] = chkDiv(warp_vec[i]*m_dbm[index_i][0],warp_vecf[i],'C');
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::reverse( const index_type& ident ) {
  size_type d = find( ident );
  if ( d == m_dim ) {
    return;
  }
  int oldval = m_dbm[0][d];
  m_dbm[0][d] = m_dbm[d][0];
  m_dbm[d][0] = oldval;

  for ( size_type i = 1 ; i < m_dim ; i++ ) 
    if (i != d) {
      m_dbm[i][d] = _MAX_VALUE_;
      m_dbm[d][i] = _MAX_VALUE_;
    }
  floyds();
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Swap the given columns in the zone.  This is useful when testing
// things like partial orders to make two similar zones be ordered in
// the same way.
//////////////////////////////////////////////////////////////////////////
void zone::swap_col( const index_type& i_ind, const index_type& j_ind) 
{
  if(i_ind == j_ind) {
    return;
  }

  size_type i = find(i_ind);
  size_type j = find(j_ind);
  
  constraint_type** old_dbm = m_dbm;
  index_type* old_ident = m_ident;
  size_type old_dim = m_dim;
  m_dbm = malloc_dbm( m_dim );
  m_ident = malloc_ident( m_dim );

  for ( size_type t = 0 ; t < old_dim ; t++ ) {
    m_ident[t] = old_ident[t];
  }
  m_ident[i] = old_ident[j];
  m_ident[j] = old_ident[i];

  for(size_type t1 = 0;t1 < old_dim;t1++) {
    if(t1 == i) {
      for(size_type t2 = 0;t2 < old_dim;t2++) {
        if(t2 == i) {
          m_dbm[j][j] = old_dbm[t1][t2];
        }
        else if(t2 == j) {
          m_dbm[j][i] = old_dbm[t1][t2];
        }
        else {
          m_dbm[j][t2] = old_dbm[t1][t2];
        }
      }
    }
    else if(t1 == j) {
      for(size_type t2 = 0;t2 < old_dim;t2++) {
        if(t2 == i) {
          m_dbm[i][j] = old_dbm[t1][t2];
        }
        else if(t2 == j) {
          m_dbm[i][i] = old_dbm[t1][t2];
        }
        else {
          m_dbm[i][t2] = old_dbm[t1][t2];
        }
      }
    }
    else {
      for(size_type t2 = 0;t2 < old_dim;t2++) {
        if(t2 == i) {
          m_dbm[t1][j] = old_dbm[t1][t2];
        }
        else if(t2 == j) {
          m_dbm[t1][i] = old_dbm[t1][t2];
        }
        else {
          m_dbm[t1][t2] = old_dbm[t1][t2];
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////
void zone::add( const index_type& ident ) {
  size_type d = find( ident );
  if ( d != m_dim ) {
    ::fprintf( stdout, "error: entry already exists\n" );
    return;
  }
  constraint_type** old_dbm = m_dbm;
  index_type* old_ident = m_ident;
  size_type old_dim = m_dim++;
  m_dbm = malloc_dbm( m_dim );
  m_ident = malloc_ident( m_dim );
  m_ident[m_dim-1] = ident;
  for ( size_type i = 0 ; i < old_dim ; i++ ) {
    m_ident[i] = old_ident[i];
    for ( size_type j = 0 ; j < old_dim ; j++ ) {
      m_dbm[i][j] = old_dbm[i][j];
    }
  }
  free_dbm( old_dbm, old_dim );
  free_ident( old_ident );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::remove( const index_type& ident ) {
  if ( m_dim == 0 ) return;
  size_type d = find( ident );
  if ( d == m_dim ) { 
    return;
  }
  constraint_type** old_dbm = m_dbm;
  index_type* old_ident = m_ident;
  size_type old_dim = m_dim--;
  m_dbm = malloc_dbm( m_dim );
  m_ident = malloc_ident( m_dim );
  size_type i = 0, x = 0, j = 0, y = 0;
  for ( ; i < old_dim ; i++ ) {
    if ( i != d ) {
      m_ident[x] = old_ident[i];
      for ( j = 0, y = 0 ; j < old_dim ; j++ ) {
        if ( j != d ) {
          m_dbm[x][y++] = old_dbm[i][j];
        }
      }
      ++x;
    }
  }
  free_dbm( old_dbm, old_dim );
  free_ident( old_ident );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::floyds(){
  for ( size_type k = 0 ; k < m_dim ; ++k ) {
    for ( size_type i = 0 ; i < m_dim ; ++i ) {
      for ( size_type j = 0 ; j < m_dim ; ++j ) {
        if ( m_dbm[i][k] != _MAX_VALUE_ &&
             m_dbm[k][j] != _MAX_VALUE_ &&
             m_dbm[i][j] > m_dbm[i][k] + m_dbm[k][j] ) 
          m_dbm[i][j] = m_dbm[i][k] + m_dbm[k][j];
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void zone::fprintf( FILE* fp ) const{
  if ( m_dim == 0 ) {
    return;
  }
  // Print the Column information
  ::fprintf( fp, "             " );
  for ( size_type i = 0 ; i < m_dim ; i++ ) {
    ::fprintf( fp, "%11s", (m_ident[i]).c_str() );
  }
  ::fprintf( fp, "\n            -" );
  for ( size_type i = 0 ; i < m_dim ; i++ ) {
    ::fprintf( fp, "-----------" );
  }
  // Print the matrix with Row identifiers.
  ::fprintf( fp, "\n" );
  for ( size_type i = 0 ; i < m_dim ; ++i ) {
    if ( i ) ::fprintf( fp, "\n" );
    ::fprintf( fp, "%11s| ", (m_ident[i]).c_str() );
    for ( size_type j = 0 ; j < m_dim ; j++ ) {
      if ( m_dbm[i][j] == _MAX_VALUE_ ) {
        ::fprintf( fp, "%11c", 'U' );
      }
      else {
        ::fprintf( fp, "%11d", m_dbm[i][j] );
      }
    }
  }
  ::fprintf( fp, "\n" );
}
//////////////////////////////////////////////////////////////////////////
