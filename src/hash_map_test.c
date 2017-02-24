#include "test.h"
#include "hash_map.h"


//Test all functions hash_map
void run_hash_test(bool flag) {  
  map_test(flag);
  iterator_test(flag);
}

//Test hash_iterator
void iterator_test(bool flag) {
  hash_map<int> my_map;
  for (int i = 0; i < 10; i++) {
    char a = '0' + i;
    my_map.put(string(1,a), i);
  }
}

//Tests hash_map
void map_test(bool flag) {  
  hash_map<int> my_map;
  
  my_print("Testing size", flag);
  if (my_map.size() != 0) {
    error("Size is not 0");
  }
  
  my_print("Testing insert", flag);
  my_map.put("0", 0);
  if (my_map.exists("0") != true) {
    error("Failed put/detect 0");
  }
  if (my_map.get("0") != 0) {
    error("Failed to retrieve 0");
  }
  if (my_map.size() != 1) {
    error("Size is not 1");
  }
  
  my_print("Testing erase", flag);
  my_map.erase("0");
  if (my_map.exists("0") != false) {
    error("errored delete 0");
  }
  if (my_map.size() != 0) {
    error("Size is not 0");
  }
  
  my_print("Stress test", flag);
  for (int i = 0; i < 10; i++) {
    char a = '0' + i;
    my_map.put(string(1,a), i);
  }
  
  if (my_map.size() != 10) {
    error("Size is not 10");
  }
  for (int i = 0; i < 10; i++) {
    char a = '0' + i;
    if (!my_map.exists(string(1,a))) {
      error("errored to find element");
    }
  }
  
  my_print("Test assign deep copy", flag);
  hash_map<int> new_map = my_map;
  
  my_print("Test iterator vs. get_at_index", flag);
  hash_map<int>::hash_iterator temp = new_map.begin();  
  for (int i = 0; i < 10; i++) {    
    if (my_map.get_at_index(i) != temp.get_value()) {
      error("errored to get object");
    }
    temp.next();
  }
  
  my_print("Passed All Tests!!!!", flag);
}
