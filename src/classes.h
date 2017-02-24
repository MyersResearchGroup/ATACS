// base_type delcaration for all concrete types.
// Author: Hao Zheng
// Date:   11/9/1998

#ifndef _CLASSES_H_
#define _CLASSES_H_
#include <map>
#include <list>
#include "tels_table.h"
#include <string>

typedef list<pair<string,tels_table*> > str_telt_lst; 
//typedef map<string,list<pair<string,tels_table*> > > vhdl_tel_tb;
typedef map<string,pair<tels_table*, bool> > csp_tel_tb; 

class vhd_config
{
public:
  
  vhd_config() {}
  ~vhd_config() {}

  bool insert(const string& entity_id, const tels_table&);
  int insert(const string& entity_id, 
	      const string& architecture_id, 
	      const tels_table&);

  const tels_table& find(const string& entity_id);
  const tels_table& find(const string& entity_id, 
			 const string& architecture_id);

  static tels_table Nil;
  
  // Use entity and architecture name as a key to search a TEL table.
  // <entity_id, <archi_id, tels_table> >
  typedef map<string,map<string,tels_table> > str_telt_m;
  typedef map<string,tels_table> str_telt_l;
  
private:

  str_telt_m open_entities;
  map<string,tels_table> open_entity_list;
};  
  
typedef vhd_config vhd_tel_tb;    
#endif
