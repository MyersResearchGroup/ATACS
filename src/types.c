#include "types.h"
#include "auxilary.h"
#include <iostream>
#include <cstdlib>
#include <string>

extern char *file_scope1;
extern int linenumber;
    
TYPES::TYPES(const string& obj, const string& data):
  grp(Integer), obj_ty(obj), data_ty(data), base_ty(), 
  int_val(0), flt_val(0.0), str_val(), ty_lst(0)
{
  /*grp = Integer;

  obj_ty = copy_string(obj);

  data_ty = copy_string(data);

  base_ty = 0;
  str_val = string();
  int_val = 0;
  flt_val = 0.0;
  list = 0;*/
}


TYPES::TYPES(const TYPES& TT):
  grp(TT.grp), obj_ty(TT.obj_ty), data_ty(TT.data_ty), base_ty(TT.base_ty), 
  int_val(TT.int_val), flt_val(TT.flt_val), str_val(TT.str_val), 
  ty_lst(new TYPELIST(TT.ty_lst))
{}

TYPES::TYPES(const TYPES *t)
{
  if(t){
    grp = t->grp;
    obj_ty = t->obj_ty;
    data_ty = t->data_ty;
    base_ty = t->base_ty;
    int_val = t->int_val;
    flt_val = t->flt_val;
    if(t->ty_lst)
      ty_lst = new TYPELIST(t->ty_lst); 
    else
      ty_lst = 0;
  }
  /*  else{
    obj_ty = 0;
    data_ty = 0;
    base_ty = 0;
    int_val = 0;
    flt_val = 0.0;
    list = 0;
    }*/
}



TYPES::~TYPES()
{
  //  delete [] obj_ty;
  //delete [] data_ty;
  //delete [] base_ty;
  //if(ty_lst) delete ty_lst;
}


void TYPES::set_obj(const string& SS)
{ obj_ty = SS; }
  
const string& TYPES::obj_type() const
{  return obj_ty;}

const string& TYPES::data_type() const 
{  return data_ty;}

void TYPES::set_data(const string& SS)
{ data_ty = SS; }

// return the base type of the object.
const string& TYPES::base_type() const 
{  return base_ty;}


void TYPES::set_base(const string& s)
{ base_ty = s; }

void TYPES::set_int(long v)
{
  grp = Integer;
  int_val = v;
}


long TYPES::get_int() const
{ return int_val; }


void TYPES::set_flt(double v)
{
  grp = Real;
  flt_val = v;
}


double TYPES::get_flt() const 
{  return flt_val;}


void TYPES::set_string(const string& SS) 
{  str_val = Tolower(SS); }

void TYPES::set_str(const string& SS)
{ str_val = Tolower(SS); }

const string& TYPES::get_string() const
{ return str_val;}

const string& TYPES::str(void) const
{ return str_val; }


void TYPES::set_list(const TYPELIST& l)
{
  if(ty_lst != &l)
    delete ty_lst;
  ty_lst = new TYPELIST(l);
}

void TYPES::set_list(const TYPELIST* l)
{
  if(ty_lst != l)
    delete ty_lst;
  ty_lst = new TYPELIST(l);
}


//void TYPES::set_list(const TYPELIST& LL)
//{ set_list(&LL); }

const TYPELIST *TYPES::get_list() const 
{ return ty_lst; }


void TYPES::set_grp(groups g)
{  grp = g; }


const TYPES::groups TYPES::grp_id() const
{ return grp; }

TYPES& TYPES::operator=(const TYPES& TT)
{
  if(this != &TT){
    grp = TT.grp;
    obj_ty= TT.obj_ty;
    data_ty = TT.data_ty;
    base_ty = TT.base_ty;
    int_val = TT.int_val;
    flt_val = TT.flt_val;
    str_val = TT.str_val;
    delete ty_lst;
    ty_lst = new TYPELIST(TT.ty_lst);
  }
  return *this;
}

bool TYPES::find(const char *s)
{
  if(ty_lst == 0)
    return false;	
  
  //if(ty_lst->find(s).first)
  //return true;

  return false;
}


char *TYPES::link(const char *s1, const char *s2)
{
  return NULL;

  /*int l_1, l_2;
  char *s_1, *s_2;

  if(s1 == 0)
    {
      l_1 = 0;
      s_1 = "";
    }
  else
    {

      l_1 = strlen(s1);
      s_1 = s1;
    }
  
  if(s2 == 0)
    {
      l_2 = 0;
      s_2 = "";
    }
  else
    {
      l_2 = strlen(s2);
      s_2 = s2;
    }

  char *ret_val = new char[l_1 + l_2 + 3];
  sprintf(ret_val, "%s#%s", s_1, s_2);

  return ret_val;*/
}



int TYPES::operator==(TYPES *t)
{    
  return 0;
}

ostream& operator<<(ostream&out, const TYPES& t){ // Output the TYPES
  if("integer" == t.data_ty){
    out << t.int_val;
  }
  else{
    out << t.str_val;
    if(t.ty_lst){
      out << '(' << *(t.ty_lst) << ')';
    }
  }
  return out;
}

/****************************************************************************/
// The definitions of the member functions of TYPELIST.
//**************************************************************************
  

// Create a hard copy of TYPELIST * 'l'.
TYPELIST::TYPELIST(const TYPELIST *l)
{  if(l)   ll = l->ll; }

TYPELIST::TYPELIST(const TYPELIST& LL): ll(LL.ll)
{}

TYPELIST::~TYPELIST()
{}

TYPELIST& TYPELIST::operator=(const TYPELIST& TT)
{
  if(this != &TT){
    ll = TT.ll;
  }
  return *this;
}

void TYPELIST::combine(const TYPELIST& tl)
{
  
  list<pair<string,TYPES> > tmp(tl.ll);
  //tmp.splice(tmp.begin(), ll);
  //ll = tmp;
  
  ll.splice(ll.end(),tmp);
}


void TYPELIST::insert(const string& s, const TYPES& t)
{ ll.push_back(make_pair(s, t)); }


// Go through the list 'll' to find tye type with given name 's'.
TYPES TYPELIST::find(const string& s)
{
  list<pair<string,TYPES> >::iterator BB;
  for(BB = ll.begin(); BB != ll.end(); BB++)
    if(s == BB->first)
      return BB->second;
  return TYPES();
}


bool TYPELIST::is_pos_assoc()
{
  if(ll.front().first.empty())
    return true;

  return false;
}


bool TYPELIST::is_name_assoc()
{
  return !is_pos_assoc();
}

ostream& operator<<(ostream&out, const TYPELIST& t){ // Output the TYPES
  list<pair<string,TYPES> >::const_iterator i(t.ll.begin());
  while(i!=t.ll.end()){
    out << i->first << "=>" << i->second << ' ';
    i++;
  }
  return out;
}

//**********************************************************************

stmt_t::stmt_t()
{
  list = 0; 
}


stmt_t::stmt_t( char *name, TERstructADT ts )
{
  val_list *n = new val_list( name, ts );

  if( list == 0 )
    list = n;
  else 
    {
      list->next = n;
      n->prev = list;
      list = list->next;
    }
}


stmt_t::stmt_t( const stmt_t& )
{}


stmt_t::~stmt_t() 
{ 
  delete list; 
}


stmt_t stmt_t::operator = ( const stmt_t&c )
{
  return c;
}


void stmt_t::insert( char *name, TERstructADT ts )
{
  val_list *n = new val_list( name, ts );

  if( list == 0 )
    list = n;
  else 
    {
      if((!strcmp(list->st_ty, "wait") || !strcmp(list->st_ty, "signal"))
	 && !strcmp(list->st_ty, name))
	list->node = TERScompose(list->node, ts, "||");
	
      list->next = n;
      n->prev = list;
      list = list->next;
    }
}


exp_class::exp_class()
{
  str_val = string();
  int_val = 0;
  float_val = 0.0;
  val_list = list<exp_class*>();
  ty = 0;
}

exp_class::~exp_class()
{
  for(list<exp_class*>::iterator b = val_list.begin(); b!=val_list.end(); b++)
    delete (*b);
  delete ty;
}

void exp_class::set_str(const string& SS)
{ str_val = SS; }

string exp_class::str(void) const
{ return str_val; }

void exp_class::set_int(const int I)
{ int_val = I; }

int exp_class::Int(void) const
{ return int_val; }
 
void exp_class::set_real(const float F)
{ float_val = F; }

double exp_class::real(void) const
{ return float_val; }

void exp_class::set_lst(const list<exp_class*>& LL)
{ val_list = LL; }

const list<exp_class*>& exp_class::lst(void) const
{ return val_list; }

void exp_class::set_ty(const TYPES& T)
{ ty = new TYPES(&T); }

const TYPES* exp_class::type(void) const
{ return ty; }
