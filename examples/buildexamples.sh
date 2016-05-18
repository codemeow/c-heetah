#!/bin/sh

gcc 00*.c              -o ./bin/00.elf
gcc 01*.c ../cheetah.c -o ./bin/01.elf
gcc 02*.c ../cheetah.c -o ./bin/02.elf
gcc 03*.c ../cheetah.c -o ./bin/03.elf
gcc 04*.c ../cheetah.c -o ./bin/04.elf -DCTH_ACTION_EXIT
gcc 05*.c ../cheetah.c -o ./bin/05.elf -DCTH_OUTPUT_FILE
gcc 06*.c ../cheetah.c -o ./bin/06.elf -DCTH_OUTPUT_STDERR
gcc 07*.c ../cheetah.c -o ./bin/07.elf -DCTH_ACTION_EXIT -DCTH_COLOURING_ON
