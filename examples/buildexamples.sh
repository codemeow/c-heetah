#!/bin/sh

gcc -Wall 00*.c              -o ./bin/00.elf
gcc -Wall 01*.c ../cheetah.c -o ./bin/01.elf
gcc -Wall 02*.c ../cheetah.c -o ./bin/02.elf
gcc -Wall 03*.c ../cheetah.c -o ./bin/03.elf
gcc -Wall 04*.c ../cheetah.c -o ./bin/04.elf -DCTH_ACTION_EXIT
gcc -Wall 05*.c ../cheetah.c -o ./bin/05.elf -DCTH_OUTPUT_FILE
gcc -Wall 06*.c ../cheetah.c -o ./bin/06.elf -DCTH_OUTPUT_STDERR
gcc -Wall 07*.c ../cheetah.c -o ./bin/07.elf -DCTH_ACTION_EXIT -DCTH_COLOURING_ON
