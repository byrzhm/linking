Refer to [https://gist.github.com/xianbaoqian/10dae2219db91122bac981263ea0c27c](https://gist.github.com/xianbaoqian/10dae2219db91122bac981263ea0c27c).


```sh
# Compile each source file to object files with `-c` option (without linking)
gcc -c main.c 
gcc -c lib.c

# Check for symbols in the object file. 
# 0000000000000000 D GLOBAL_COUNTER        Symbol type D means an initialized value (main.c sets this value to 100).
# 000000000000005c T inc_global_counter    T means that this symbol is in the text section and refers to the compiled machine code.
# 0000000000000000 T main                  
#                  U my_lib_func           U means that this symbol is not defined in this file and must be loaded from another file.
#                  U printf
#                  U puts
nm main.o
#                  U GLOBAL_COUNTER
#                  U inc_global_counter
# 0000000000000000 B LOCAL_COUNTER         B means that this value is zero-initialized (lib.c sets its value to 0).
# 0000000000000000 T my_lib_func
#                  U printf
#                  U puts
nm lib.o

# As we can see from `nm` outputs, none of these are complete that contain all the symbols, so we need to link them together.
gcc main.o lib.o

# Now let's run the program and everything seems to be working (no unresolved symbols)
# Main: 
#          global counter: 100
#          Lib: 
#                  local counter: 0
#                  global counter: 100
#                  processing...
#                  local counter: 1
#                  global counter: 102
#          global counter: 102
./a.out

# Bonus 1: hidden shared library glibc
# You might notice some undefined symbols above like `printf` and `puts`. What happened to them? GCC is smart enough to find them.
# Let's verify it.

# We'll find the following two lines which mean that these symbols are going to be loaded from glibc.
#                 U printf@GLIBC_VERSION
#                 U puts@GLIBC_VERSION
nm a.out

# ldd can reveal the actual location of the libc library
ldd a.out

# You can also check the actual linker command passed to `ld` with `-v` flag
gcc -v main.o lib.o

# Bonus 2: build glibc statically into the executable
# What if I don't want to depend on the shared library and want to instead compile everything in a single binary that can be deployed on any machine e.g. the one without glibc?
# This is possible with `--static` linker flag.

gcc -v --static main.o lib.o -o static.out

# First let's make sure it's running correctly
./static.out

# And let's verify that no shared library is used.
ldd static.out

# Static compiled binary is often much larger as it embeds code that is otherwise shared.
ls -lh a.out static.out

# We can verify that `printf` is already embedded by looking at the binary's symbol list.
nm static.out | grep printf
```


```sh
# Now let's play with the shared library.

# Compile the main as before.
gcc -c main.c

# Now we instruct the compiler to build Position Independent Code (PIC) so that the library code can be loaded to any memory position.
# Check this great StackOverflow question to understand more details: https://stackoverflow.com/questions/18026333/what-does-compiling-with-pic-dwith-pic-with-pic-actually-do
gcc -c -fPIC lib.c

# Use `--shared` to generate the shared library.
# Note that the name of the shared library file follows a convention libLIBRARY_NAME.so (libmy.so in this case)
gcc --shared lib.o -o libmy.so 

# Now link the final binary. -l gives the library name and -L gives the library search path to the linker. 
gcc main.o -lmy -L.

# Let's run the program and see if the output makes sense.
./a.out

# Unfortunately the binary doesn't run due to the following error.
# ./a.out: error while loading shared libraries: libmy.so: cannot open shared object file: No such file or directory

# This just means that the binary needs libmy.so which is not found from the runtime linker search path.
# Note that the runtime linker search path is not affected by the above -L flag passed to the linker. They're used for different stages.
# Check `rpath` from WIKI to understand more about the default runtime search path: https://en.wikipedia.org/wiki/Rpath

# `ldd` command also gives the same information
#         libmy.so => not found
ldd a.out | grep libmy.so

# We can temporarily mutate the runtime linker's search path by setting the `LD_LIBRARY_PATH` environment variable.
LD_LIBRARY_PATH=. ldd ./a.out
LD_LIBRARY_PATH=. ./a.out

# Bonus: initializer and finalizer for the shared library

# A shared library contains a few additional symbols.
# Here are some detailed explanations: https://stackoverflow.com/questions/34966097/what-functions-does-gcc-add-to-the-linux-elf
# Some of them will be invoked by the runtime linker upon initialization and finalization. Linker flag -Wl,-init, and -Wl,-fini, allows us to custom these functions.
gcc --shared lib.o -o libmy.so -Wl,-init,my_lib_func -Wl,-fini,my_lib_func 

# Now let's build the binary and see what's going on
gcc main.o -lmy -L.

# We can see that my_lib_func has been run before and after the main function. 
#          Lib: 
#                  local counter: 0
#                  global counter: 100
#                  processing...
#                  local counter: 1
#                  global counter: 102
# Main: 
#          global counter: 102
#          Lib: 
#                  local counter: 1
#                  global counter: 102
#                  processing...
#                  local counter: 2
#                  global counter: 104
#          global counter: 104
#          Lib: 
#                  local counter: 2
#                  global counter: 104
#                  processing...
#                  local counter: 3
#                  global counter: 106
LD_LIBRARY_PATH=. ./a.out

# And it becomes more obvious with `LD_DEBUG=files`
LD_LIBRARY_PATH=. LD_DEBUG=files ./a.out

# These initializer and finalizer functions are very useful for module-level initialization.
```

```sh
# Let's build the shared library in the same way
gcc -c -fPIC lib.c
gcc --shared lib.o -o libmy.so

# Now we build the main binary.
gcc -c main_dynamic.c
# -ldl to inject dl shared library as a dependency to the executable.
# -rdynamic to allow the shared library loaded by `dlopen` to refer back to symbols in the binary file itself.
# Check '-E' option in this doc to read more information related to -rdynamic. https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_2.html
gcc -rdynamic main_dynamic.o -ldl

# And now we can execute the final binary
LD_LIBRARY_PATH=. ./a.out

# Bonus: module initializer

# Attach the initializer and finalizer to the shared library
gcc --shared lib.o -o libmy.so -Wl,-init,my_lib_func -Wl,-fini,my_lib_func

# And now let's see the magic. Enjoy!
LD_LIBRARY_PATH=. LD_DEBUG=files ./a.out
```
