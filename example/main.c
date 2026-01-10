// Demonstrate the use of static compile and the use of shared library.
// Note that either use case requires the implementation of the library at
// compile time.
#include "main.h"
#include "lib.h"
#include <stdio.h>

// Memory allocation happens here.
int GLOBAL_COUNTER = 100;

int main() {
  printf("Main: \n");
  printf("\t global counter: %d\n", GLOBAL_COUNTER);
  my_lib_func();
  printf("\t global counter: %d\n", GLOBAL_COUNTER);
  return 1;
}

void inc_global_counter() { GLOBAL_COUNTER++; }