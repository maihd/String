#include <stdio.h>
#include "String.h"

int main()
{
    const char* staticGreeting = "Hello world, my name is: DancingConcat";
    printf("staticGreeting.length: %d, staticGreeting.content: %s\n", StringLength(staticGreeting), staticGreeting);

    char buffer[1024];
    const char* stackGreeting = StringFrom(buffer, "Hello world, my name is: DancingConcat");
    printf("stackGreeting.length: %d, stackGreeting.content: %s\n", StringLength(stackGreeting), stackGreeting);

    const char* heapGreeting = StringFormat(64, "Hello world, my name is: %s", "DancingConcat");
    printf("heapGreeting.length: %d, heapGreeting.content: %s\n", StringLength(heapGreeting), heapGreeting);

    StringFree(heapGreeting);
    return 0;
}
