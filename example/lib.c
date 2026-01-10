#include "lib.h"
#include "main.h"
#include "stdio.h"

// Memory allocation happens here.
int LOCAL_COUNTER = 0;

void my_lib_func() {
  printf("\t Lib: \n");
  printf("\t \t local counter: %d\n", LOCAL_COUNTER);
  printf("\t \t global counter: %d\n", GLOBAL_COUNTER);

  printf("\t \t processing...\n");
  LOCAL_COUNTER++;
  GLOBAL_COUNTER++;
  inc_global_counter();

  printf("\t \t local counter: %d\n", LOCAL_COUNTER);
  printf("\t \t global counter: %d\n", GLOBAL_COUNTER);
}