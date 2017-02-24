#include "classes.h"
//*************************************************************************
// class vhd_config definitions.
//***********************************

tels_table vhd_config::Nil = tels_table();

//multiple declaration of entity E_id
bool vhd_config::insert(const string& E_id, const tels_table& TT)
{
  string E_idL = Tolower(E_id);
  if(open_entity_list.find(E_idL) != open_entity_list.end())
    return false;
  
  open_entity_list[E_idL] = TT;
  open_entities[E_idL] = map<string,tels_table>();
  return true;
}

//multiple definition of architecture A_id
int vhd_config::insert(const string& E_id, const string& A_id,
			const tels_table& TT)
{
  string E_idL = Tolower(E_id);
  string A_idL = Tolower(A_id);
  
  if(open_entities.find(E_idL)==open_entities.end())
    return 1;

  if(open_entities[E_idL].find(A_idL)!=open_entities[E_idL].end())
    return 2;

  open_entities[E_idL].insert(make_pair(A_idL,TT));
  return true;
}

//entity E_id not declared.
const tels_table& vhd_config::find(const string& E_id) 
{ 
  string E_idL = Tolower(E_id);
  if(open_entity_list.find(E_idL) == open_entity_list.end())
    return Nil;

  return open_entity_list[E_idL];
}

//architecture A_id bot defined.
const tels_table& vhd_config::find(const string& E_id, 
				   const string& A_id) 
{
  string E_idL = Tolower(E_id);
  string A_idL = Tolower(A_id);

  if(open_entities.find(E_idL)==open_entities.end())
    return Nil;
  
  if(!A_id.empty() && 
     open_entities[E_idL].find(A_idL)==open_entities[E_idL].end())
    return Nil;
  
  if(!A_id.empty())
    return open_entities[E_idL][A_idL];
  return (open_entities[E_idL].begin())->second;
}
