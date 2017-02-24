#include <stdio.h>
#include <string>
#include <math.h>

#define   INFIN     2147483647

//////////////////////////////////////////////////////////////////////////
// Maximum size of an input line
/////////////////////////////////////////////////////////////////////////
static const int _MAX_LINE_ = 80;

using namespace std;

//////////////////////////////////////////////////////////////////////////
class zone {
public:

  typedef string index_type;
  typedef short constraint_type;
  typedef constraint_type* constraint_ptr;
  typedef short size_type;

protected:

  size_type m_dim;
  constraint_type** m_dbm;
  index_type* m_ident;

public:

  static const constraint_type _MAX_VALUE_;
  static const char _UNBOUNDED_[2];
  static const size_type _MAX_LENGTH_;

protected:

  constraint_type** malloc_dbm( const size_type& dim );
  void free_dbm( constraint_type** dbm, size_type dim );

  index_type* malloc_ident( const size_type& dim );
  void free_ident( index_type* ident );

  size_type find( const index_type& ident ) const;

public:

  static index_type make_index( char* t );
  
  zone();
  zone( const char* const fname );
  ~zone();
  bool load( const char* const fname );
  bool save( const char* const fname );
  bool view();
  bool view(const index_type& i, const index_type& j);
  void set_value( const index_type& i, const index_type& j, 
		  const constraint_type& v );
  void set_min( const index_type& i, const constraint_type& v );
  void set_max( const index_type& i, const constraint_type& v );
  void warp( zone::index_type warp_list[_MAX_LINE_],
	     int warp_vec[_MAX_LINE_],
	     int warp_vecf[_MAX_LINE_],
	     int size );
  int chkDiv(int a,int b,char func);
  void remove( const index_type& ident );
  void add( const index_type& ident );
  void reverse( const index_type& ident );
  void swap_col( const index_type& i, const index_type& j);
  void floyds();
  void fprintf( FILE* fp ) const;

};
//////////////////////////////////////////////////////////////////////////
