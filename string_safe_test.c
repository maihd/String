#define _CRT_SECURE_NO_WARNINGS

#define STRING_SAFE_IMPL
#include "string_safe.h"

int main()
{
    String staticGreeting = string_weak("Hello world, my name is: DancingConcat");
    printf("staticGreeting.length: %d, staticGreeting.content: %s\n", staticGreeting.length, staticGreeting.buffer);

    char buffer[1024];
    String stackGreeting = string_from_buffer(buffer, "Hello world, my name is: DancingConcat");
    printf("stackGreeting.length: %d, stackGreeting.content: %s\n", stackGreeting.length, stackGreeting.buffer);

    String heapGreeting = string_format(64, "Hello world, my name is: %s", "DancingConcat");
    printf("heapGreeting.length: %d, heapGreeting.content: %s\n", heapGreeting.length, heapGreeting.buffer);

    string_free(staticGreeting); // This is no need to free, but it's should safe to call
    string_free(stackGreeting); // This is no need to free, but it's should safe to call
    string_free(heapGreeting);

    printf("End of test.\n");
    return 0;
}
