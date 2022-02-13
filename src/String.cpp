#include "String.hpp"

#include <stdio.h>
#include <stdlib.h>

uint32_t String::SaveBuffer()
{
    if (IsHeapAlloc())
    {
        return 0;    
    }

    printf("Attempting malloc new buffer!\n");

    const int length = GetLength();
    void* newBuffer = malloc(length + 1);
    memcpy(newBuffer, buffer, length + 1);
    buffer = (const char*)newBuffer;

    lengthAndFlags &= ~FLAGS_WEAK_REF;
    lengthAndFlags |= FLAGS_HEAP_ALLOC;
    return 0;
}

void String::FreeBuffer()
{
    if (IsHeapAlloc())
    {
        free((void*)buffer);

        buffer = "";
        lengthAndFlags = FLAGS_WEAK_REF | FLAGS_CALC_LENGTH;
    }
}
