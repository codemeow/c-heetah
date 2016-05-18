#!/bin/sh

# Makes colored version, sending to stderr
gcc -fno-builtin        \
    -shared             \
    -D_GNU_SOURCE       \
    -DCTH_OUTPUT_STDERR \
    -DCTH_COLOURING_ON  \
    -DCTH_BUILD_LIBRARY \
    -o libcheetah.so    \
    -fPIC cheetah.c     \
    -ldl

# Use:
# LD_PRELOAD=./libcheetah.so ./a.out
