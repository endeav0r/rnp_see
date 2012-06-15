#include <stdio.h>

int strcpy2 (char * dst, char * src)
{
    int i;
    
    for (i = 0; src[i] != '\0'; i++)
        dst[i] = src[i];
    
    dst[i] = '\0';

    return i;
}

int main (int argc, char * argv[])
{
    char string[32];
    strcpy2(string, "Hello ");
    strcpy2(string, argv[1]);
    printf("hello %s", string);
    return 0;
}
    
