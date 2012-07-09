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
    strcpy2(string, "david");
    printf("hello %s\n", string);
    return 0;
}
    
