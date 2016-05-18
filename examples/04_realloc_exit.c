#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cheetah.h"

int main(void)
{
    char * ptr = malloc(sizeof(int) * 100);

    strcat(ptr, "Hello world!\n");

    printf("%s", ptr);

    ptr = realloc(ptr, 2147483648U /* 2^31 */);

    strcat(ptr, "Have you mooed today?\n");

    free(ptr);

    return 0;
}
