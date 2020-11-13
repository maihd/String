#include <stdio.h>
#include "String.hpp"

int main()
{
    StringView emptyString;
    printf("emptyString.length = %d, emptyString.buffer = \"%s\"\n", emptyString.length, emptyString.buffer);
    
    StringView string = "must be have static lifetime";
    printf("string.length = %d, string.buffer = \"%s\"\n", string.length, string.buffer);

    printf("See, there is no nullptr string!\n");
    return 0;
}