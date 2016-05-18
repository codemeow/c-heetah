# C-heetah

Memory management watcher.
Logs any calls of allocation functions:

Example:
```
[C-TH][        07_all_coloured.c:0009] Malloc :              0x00000190 B (0x09A9C008) : Success
[C-TH][        07_all_coloured.c:0013] Realloc: (0x09A9C008) 0x00000320 B (0x09A9C008) : Success
[C-TH][        07_all_coloured.c:0017] Free   : (0x09A9C008)                           : Success
[C-TH][        07_all_coloured.c:0019] Calloc :              0x00000080 B (0x09A9C008) : Success
[C-TH][        07_all_coloured.c:0022] Free   : (0x09A9C008)                           : Success
```

# Call from source

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Add C-heetah header to 
   automatically watch memory
   management in this file */
#include "cheetah.h"

int main(void)
{
    char * ptr = malloc(sizeof(int) * 100);
    if (!ptr)
    {
        printf("Bad alloc\n");
        return 1;
    }

    strcat(ptr, "Hello world!\n");

    printf("%s", ptr);

    free(ptr);

    return 0;
}
```

# Call as library

You also may use C-heetah with any built application via LD_PRELOAD trick:

```
./buildlibrary.sh
LD_PRELOAD=./libcheetah.so ./a.out
```

For example you can use it with an example built without included header (#00):
```
cd examples
./buildexamples.sh
cd ..

./buildlibrary.sh
LD_PRELOAD=./libcheetah.so ./examples/bin/00.elf

[C-TH][         C-heetah library:0000] Malloc :              0x00000190 B (0x083FE008) : Success
Hello world!
[C-TH][         C-heetah library:0000] Free   : (0x083FE008)                           : Success
```

Note, that by default `buildlibrary.sh`  builds with options `CTH_COLOURING_ON` and `CTH_OUTPUT_STDERR`. You may change it as you wish, no limits applied.

# Compile-time options

To activate additional features you may use the following cc-compiler options:

```
Memory management philosophy:

-DCTH_PHILOSOPHY_PESSIMISTIC
    Disables linux-provided optimistic memory management, when
    the malloc function returns pointer to partly-occupied block.
    In fact this option replaces malloc calls to calloc, so
    if such case happened, the program will exit (see -D_ACTION_* ) at once.

-DCTH_PHILOSOPHY_OPTIMISTIC
    Allows linux optimistic memory management.

-DCTH_PHILOSOPHY_DEFAULT
    Equals to -DCTH_PHILOSOPHY_OPTIMISTIC

Colouring:

-DCTH_COLOURING_ON
    Enables coloured output. Make your day brighter. Uses ANSI-escape codes.

-DCTH_COLOURING_OFF
    Diables coloured output.

-DTCH_COLOURING_DEFAULT
    Equals to -DCTH_COLOURING_OFF. In fact _OFF and _DEFAULT do nothing.

Output:

-DCTH_OUTPUT_STDOUT
    Places any output to stdout.

-DCTH_OUTPUT_STDERR
    Places any output to stderr.

-DCTH_OUTPUT_FILE
    Places any output to file "/tmp/cheetah_<PID>.log"

-DCTH_OUTPUT_DEFAULT
    Equals to -DCTH_OUTPUT_STDOUT

Actions for failed allocation:

-DCTH_ACTION_NOTHING
    Do nothing if allocation functions have failed. Full compability.

-DCTH_ACTION_EXIT
    Exits if any allocation function has failed. With this option you
    may skip any allocated pointer checking. Is there any way why you need
    to continue when OOM-Reaper is taking its bloody harvest?

-DCTH_ACTION_DEFAULT
    Equals to -DCTH_ACTION_NOTHING
```

See practical examples in /examples directory.


