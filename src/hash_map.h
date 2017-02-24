/**
 *	My own hash map class based off of STL Maps.
 *      Has ability to traverse all elements in map by
 *      index in O(n) time.  
 *
 **/
 

#ifndef _hash_map_h
#define _hash_map_h
 
#include <string>
#include <map>
#include <iostream>
#include <string.h>

using namespace std;

//comparator struct used in hashmaps and sets
struct cmpstr{
  bool operator()(const string s1, const string s2) const {
  	return strcmp(s1.c_str(), s2.c_str()) < 0;
  }
};

template <class T>
class hash_map {
public:
   //typedef typename map<string,T,cmpstr>::iterator my_iterator;

  /**
   *  Internal class.  Does not allow modifications to hash_map   
   */
  class hash_iterator {
  public:
    hash_iterator();
    string get_key();
    T& get_value();
    void next();
    bool has_next();
    hash_iterator& operator=(const hash_iterator& to_copy);  
//  protected:
    void reset();
    void set(typename map<string,T,cmpstr>::iterator iter);  
    //void set(hash_map<T> *my_map);
    //Member Variables
    hash_map<T> *my_map;  
    typename map<string,T,cmpstr>::iterator my_iter;    
  };

  hash_map();
  T& get(string key);
  void put(string key, T value);
  bool exists(string key);
  bool erase(string key);
  int size();
  void clear();
  hash_iterator begin();
  T& get_at_index(int i);
  hash_map<T>& operator=(const hash_map<T>& to_copy);  
private:
  map<string, T, cmpstr> my_map;
  hash_iterator my_iter;
  int index;

};

template <class T>
typename hash_map<T>::hash_iterator& hash_map<T>::hash_iterator::operator=(const hash_iterator& to_copy) {
    my_iter = to_copy.my_iter;
    my_map = to_copy.my_map;
    return *this;
}

//Returns an iterator to the hash_map
template <class T>
typename hash_map<T>::hash_iterator hash_map<T>::begin() {
  hash_iterator iter;
  iter.set(my_map.begin());
  return iter;
}

//Iterator class.
template <class T>
void hash_map<T>::hash_iterator::set(typename map<string,T,cmpstr>::iterator new_iter) {
  my_iter = new_iter;
}

template <class T>
hash_map<T>::hash_iterator::hash_iterator() {
}

//Returns the current key of the iterator
template <class T>
string hash_map<T>::hash_iterator::get_key() {
  return (*my_iter).first;
}

//Returns the current value of the iterator
template <class T>
T& hash_map<T>::hash_iterator::get_value() {
  return (*my_iter).second;  
}

//Moves to next iterator object
template <class T>
void hash_map<T>::hash_iterator::next() {
  if (has_next()) {
    my_iter++;
  }
  else {
    cerr << "Iterator Index out of bounds" << endl;
    //exit(-1); 
  }
}

//Checks to see if there are more elements
template <class T>
bool hash_map<T>::hash_iterator::has_next() {
  return my_iter != my_map->my_map.end();
}

template <class T>
hash_map<T>::hash_map() {
  index = -1;
}

//Makes a deep copy
template <class T>
hash_map<T>& hash_map<T>::operator=(const hash_map<T>& to_copy) {
    my_map = to_copy.my_map;
    my_iter = to_copy.my_iter;
    index = to_copy.index;
    return *this;
}

//Returns the size of the hash_map
template <class T>
int hash_map<T>::size() {
  return my_map.size();
}

//Puts an object into the hash_map
template <class T>
void hash_map<T>::put(string key, T value) {
  my_map[key] = value;
  index = -1;
}

//Checks to see if an object exists
template <class T>
bool hash_map<T>::exists(string key) {
  return my_map.find(key) != my_map.end();
}

//Removes an object from the map
template <class T>
bool hash_map<T>::erase(string key) {
  index = -1;
  if (exists(key)) {
    my_map.erase(key);    
    return true;
  }
  return false;
}

//Returns an object with the key.
template <class T>
T& hash_map<T>::get(string key) {
  if (exists(key)) {
    return my_map[key];
  }
  cerr << "Object with " << key << " does not exist:\n";
  //exit(-1);
}

//Clears the map
template <class T>
void hash_map<T>::clear() {
  index = -1;
  my_map.clear();
}

//Iterates through the map by indices
template <class T>
T& hash_map<T>::get_at_index(int i) {
  if (i >= size() || i < 0) {
    cerr << "Access out of bounds at " << i << endl;
    //exit(-1);
  }
  if (index == -1 || index > i) {
    my_iter.set(my_map.begin());
    index = 0;
  }
  while (index < i) {
    my_iter.next();
    index++;
  }
  return my_iter.get_value();
}

#endif
