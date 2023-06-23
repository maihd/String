#include <stdio.h>

#define STRING_IMPL
#include "../src/String.h"

int main()
{
    //StringLength(NULL); // This should be assertion by the runtime

    printf("StringLength(\"\") = %d\n", StringLength(""));

    const char* staticGreeting = "Hello world, my name is: MaiHD";
    printf("staticGreeting.length: %d, staticGreeting.content: %s\n", StringLength(staticGreeting), staticGreeting);

    char buffer[1024];
    const char* stackGreeting = StringFrom(buffer, "Hello world, my name is: MaiHD");
    printf("stackGreeting.length: %d, stackGreeting.content: %s\n", StringLength(stackGreeting), stackGreeting);

    const char* heapGreeting = StringFormat(64, "Hello world, my name is: %s", "MaiHD");
    printf("heapGreeting.length: %d, heapGreeting.content: %s\n", StringLength(heapGreeting), heapGreeting);

    StringFree(heapGreeting);
    return 0;
}
