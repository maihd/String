#include <stdio.h>
#include "String.hpp"

int main()
{
    StringView emptyString;
    printf("emptyString.buffer = %p\n", emptyString.buffer);
    printf("emptyString.length = %d, emptyString.buffer = \"%s\"\n", emptyString.GetLength(), emptyString.buffer);
    
    StringView string = "must be have static lifetime";
    printf("string.length = %d, string.buffer = \"%s\"\n", string.GetLength(), string.buffer);
    printf("string.buffer = %p\n", string.buffer);

    String stringBuffer = string.buffer;
    stringBuffer.SaveBuffer();

    printf("stringBuffer.buffer = %p, string.buffer = %p\n", stringBuffer.buffer, string.buffer);
    printf("stringBuffer.buffer == string.buffer: %d\n", stringBuffer.buffer == string.buffer);

    printf("See, there is no nullptr string!\n");

    stringBuffer.FreeBuffer();
    return 0;
}
