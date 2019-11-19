#include <stdio.h>

#define STRING_IMPL
#include "String.h"

int main()
{
    string greeting = String(64, "Hello world, my name is: %s", "DancingConcat");
    printf("Greeting: %s\n", greeting);    

    StringFree(greeting);
    return 0;
}
