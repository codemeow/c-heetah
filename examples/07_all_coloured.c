#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cheetah.h"

int main(void)
{
    char * ptr = malloc(sizeof(int) * 100);
    strcat(ptr, "Hello world!\n");
    printf("%s", ptr);

    ptr = realloc(ptr, sizeof(int) * 200);    
    strcat(ptr, "Have you mooed today?\n");
    printf("%s", ptr);

    free(ptr);

    char * calptr = calloc(128, 1);
    calptr[0] = 34;
    printf("Calptr is %d\n", calptr[0]);
    free(calptr);

    return 0;
}
