#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <assert.h>

#define STRING_SAFE_IMPL
#include "string_safe.h"

int main()
{
    String static_greeting = string_weak("Hello world, my name is: DancingConcat");
    printf("static_greeting.length: %d, static_greeting.content: %s\n", static_greeting.length, static_greeting.buffer);

    char buffer[1024];
    String stack_greeting = string_from_buffer(buffer, "Hello world, my name is: DancingConcat");
    printf("stack_greeting.length: %d, stack_greeting.content: %s\n", stack_greeting.length, stack_greeting.buffer);

    String heap_greeting = string_format(64, "Hello world, my name is: %s", "DancingConcat");
    printf("heap_greeting.length: %d, heap_greeting.content: %s\n", heap_greeting.length, heap_greeting.buffer);

    string_free(&static_greeting); // This variable is no need to free, but it's should safe to call
    string_free(&stack_greeting); // This variable is no need to free, but it's should safe to call
    string_free(&heap_greeting);

    assert(static_greeting.length == 0 && static_greeting.buffer == EMPTY_STRING);
    assert(stack_greeting.length == 0 && stack_greeting.buffer == EMPTY_STRING);
    assert(heap_greeting.length == 0 && heap_greeting.buffer == EMPTY_STRING);

    printf("End of test.\n");
    return 0;
}
