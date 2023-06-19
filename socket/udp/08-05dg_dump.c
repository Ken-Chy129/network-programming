#include <stdio.h>
#include <stdlib.h>

void err_dump(const char *str)
{
    perror(str);
    exit(1);
}