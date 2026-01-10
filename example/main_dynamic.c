// Same as main.c but loads the lib implementation from a dynamic library.
#include "main.h"
// Note that we don't have to depend on "lib.h" here but the developer needs to
// be cautious about the symbol names and their types. #include "lib.h"
#include <stdio.h>
// Required for dynamic loading options.
#include <dlfcn.h>

// Memory allocation happens here.
int GLOBAL_COUNTER = 100;

void call_my_lib_func() {
  printf("Starting to load the library.\n");
  void *dl_handle = dlopen("libmy.so", RTLD_LAZY);
  if (!dl_handle) {
    printf("Failed to load the library: %s.\n", dlerror());
    return;
  }
  void (*func)() = dlsym(dl_handle, "my_lib_func");
  if (!func) {
    printf("Failed to load the symbol: %s.\n", dlerror());
    return;
  }

  (*func)();

  dlclose(dl_handle);
  printf("Closing the dynamic library.\n");
}

int main() {
  printf("Main: \n");
  printf("\t global counter: %d\n", GLOBAL_COUNTER);
  call_my_lib_func();
  printf("\t global counter: %d\n", GLOBAL_COUNTER);
  return 1;
}

void inc_global_counter() { GLOBAL_COUNTER++; }