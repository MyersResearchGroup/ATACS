#include "test.h"

//Print out an error message and exit the program
void error(string message) {
  cerr << message << endl;
  //exit(-1);
}

//Prints out a message if flag is true.
void my_print(string message, bool flag) {
  if (flag) {
    cout << message << endl;
  }
}
