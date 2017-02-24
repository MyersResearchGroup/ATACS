
/***************************TYPES OF VARIABLES************************/

#ifndef TYPES_H
#define TYPES_H

#include "ly.h"
#include "tersgen.h"
#include <iostream>
#include <list>
#include <string>

extern int linenumber;

extern char* copy_string(const char*);


//enum groups {Integer, Real, Enumeration, Physical, Array, Record};



class TYPELIST;


class TYPES{
  
public:

  // Define the type groups of the VHDL language.
  enum groups {Integer, Real, Enumeration, Physical, Array, Record, Error};


  // Constructors and destructors.
  TYPES(const string& s = "uk", const string& t = "uk");
  TYPES(const TYPES&);
  TYPES(const TYPES *t);

  virtual ~TYPES();

  // Member functions.

  void set_obj(const string&);
  const string& obj_type() const;
  void set_data(const string&);
  const string& data_type() const;
  void set_base(const string&);
  const string& base_type() const;
  void set_int(long v);
  long get_int() const;
  void set_flt(double v);
  double get_flt() const;
  void set_string(const string&);
  void set_str(const string&);
  const string& get_string() const;
  const string& str(void) const;
  void set_list(const TYPELIST*);
  void set_list(const TYPELIST&);
  //void set_list(const TYPELIST&);
  const TYPELIST *get_list() const;
  bool find(const char *);
  char *link(const char *s1, const char *s2);
  int operator==( TYPES*);
  virtual void set_ters(const TERstructADT ts) {}
  virtual const TERstructADT get_ters() const { return NULL; }
  void set_grp(groups g);
  const groups grp_id() const;

  TYPES& operator=(const TYPES&);
  friend ostream& operator<<(ostream&out, const TYPES& t);//print a TYPES

  string theExpression;
  // Private data members of the type.
protected:

  groups grp; 
  string obj_ty;
  string data_ty;
  string base_ty;
  long int_val;
  double flt_val;
  string str_val;
  TYPELIST *ty_lst;
};

typedef pair<string,list<string> >         stmt_pair;
typedef list<pair<string,list<string> > >  simul_lst;


typedef list<pair<TYPES,telADT> > ty_tel_lst;
typedef list<pair<string,TYPES> > str_ty_lst;
typedef list<pair<string,TYPES> >::value_type value_ty;

class TYPELIST{

public:

  TYPELIST() {}
  TYPELIST(const TYPELIST*);
  TYPELIST(const value_ty& TT):ll(1, TT) {}
  TYPELIST(const str_ty_lst& LL):ll(LL) {}
  TYPELIST(const TYPELIST&);
  ~TYPELIST();

  TYPELIST& operator=(const TYPELIST&); 
  
  void combine(const TYPELIST&);

  void iteration_init() { iteration_ptr = ll.begin(); }
  pair<string,TYPES>& value() const { return *iteration_ptr; }
  //void next() { iteration_ptr++; }
  //bool end() { return (iteration_ptr == ll.end()); } 

  void insert(const string&, const TYPES&);
  void insert(const value_ty& TT) { ll.push_back(TT); }

  TYPES find(const string&);

  int size() { return ll.size(); }
  bool empty() const { return ll.empty(); }

  bool is_pos_assoc();
  bool is_name_assoc();

  typedef list<pair<string,TYPES> >::reference reference;
  typedef list<pair<string,TYPES> >::const_reference const_reference;
  typedef list<pair<string,TYPES> >::iterator iterator;
  typedef list<pair<string,TYPES> >::const_iterator const_iterator;
  typedef list<pair<string,TYPES> >::value_type value_type; 

  reference front() { return ll.front(); };
  const_reference front() const { return ll.front(); };
  reference back() { return ll.back(); };
  const_reference back() const { return ll.back(); };
  
  iterator begin() { return ll.begin(); }
  iterator end() { return ll.end(); }
  const_iterator begin() const { return ll.begin(); }
  const_iterator end()  const { return ll.end(); }

  friend ostream& operator<<(ostream&out, const TYPELIST& t);//print a TYPELIST

private:

  list<pair<string,TYPES> >::iterator iteration_ptr;
  list<pair<string,TYPES> > ll;
};




void printype(TYPES *ty);


void printlist( TYPELIST* );



//The type is defined for statements.
//--------------------------------------

class stmt_t {

public:

  stmt_t(); 

  stmt_t( char *name, TERstructADT ts );

  stmt_t( const stmt_t& );

  ~stmt_t();
 
  stmt_t operator = ( const stmt_t& );
  
  void insert( char *name, TERstructADT ts );

  TERstructADT compose();
  
  
private:
  
  struct val_list {
    char *st_ty;
    TERstructADT node;
    val_list *next;
    val_list *prev;
    val_list(char *name, TERstructADT ts)  { st_ty = name; node = ts; }
    ~val_list() { TERSdelete(node); if(prev) delete prev; }
  }
  *list;
};


class exp_class
{
public:
  
  exp_class();
  ~exp_class();

  void set_str(const string&);
  string str(void) const;
  void set_int(const int);
  int Int(void) const;
  void set_real(const float);
  double real(void) const;
  void set_lst(const list<exp_class*>& );
  const list<exp_class*>& lst(void) const;
  void set_ty(const TYPES&);
  const TYPES* type(void) const;

private:
  string str_val;
  int int_val;
  double float_val;
  list<exp_class*> val_list;
  TYPES *ty;
};

#endif
