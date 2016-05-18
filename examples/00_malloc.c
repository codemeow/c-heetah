#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
