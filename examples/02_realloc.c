#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cheetah.h"

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

    char * oldptr = ptr;
    ptr = realloc(ptr, sizeof(int) * 200);
    if (!ptr)
    {
        printf("Bad alloc\n");
        free(oldptr);
        return 1;
    }

    strcat(ptr, "Have you mooed today?\n");
    printf("%s", ptr);

    free(ptr);

    return 0;
}
