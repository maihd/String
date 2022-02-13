#pragma once

#include <stdint.h>

typedef struct String
{
    const char*     data;
    const int32_t   size;
    const int32_t   capacity;
} String;

#ifdef __cplusplus
#define ConstString(sequence) (String { sizeof(sequence) - 1, sequence })
#else
#define ConstString(sequence) ((String){ .size = sizeof(sequence) - 1, .data = sequence })
#endif

int32_t     StringSplitChar(String string, char pattern, String parts[], int32_t count);
int32_t     StringSplitPattern(String string, String pattern, String parts[], int32_t count);
