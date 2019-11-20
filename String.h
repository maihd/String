#ifndef STRING_H
#define STRING_H

#include <stdarg.h>

#ifndef STRING_API
#define STRING_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// NO POINTER & IMMUTABLE for safety
typedef const char* string;

STRING_API string String(string source);
STRING_API void   StringFree(string target);

STRING_API string StringFormat(int bufferSize, string format, ...);
STRING_API string StringFormatArgv(int bufferSize, string format, va_list argv);

STRING_API string StringFrom(void* buffer, string source);
STRING_API string StringFormatBuffer(void* buffer, string format, ...);
STRING_API string StringFormatBufferArgv(void* buffer, string format, va_list argv);

STRING_API int    StringLength(string target);

STRING_API int    StringIsHeap(string target);
STRING_API int    StringIsWeak(string target);
STRING_API int    StringIsSmart(string target);

#ifdef __cplusplus
}
#endif

#endif // STRING_H

#ifdef STRING_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef STRING_CONST_HASH_U64
#define STRING_CONST_HASH_U64(string, defaultValue) (uint64_t)(defaultValue)
#endif

#ifndef UINT64_MAX
typedef unsigned long long uint64_t; 
#endif

typedef struct StringBuffer
{
    int         length;
    int         memref;
    uint64_t    memtag;
    char        data[];
} StringBuffer;

static string   sEmptyString = "";
static uint64_t sHeapMemtag  = STRING_CONST_HASH_U64("__string_heap_memory_tag__", 0xa020b127788efe8fULL);  // ISO CRC64
static uint64_t sWeakMemtag  = STRING_CONST_HASH_U64("__string_stack_memory_tag__", 0xb166f1068d721eceULL); // ISO CRC64

int StringIsHeap(string target)
{
    if (target && target != sEmptyString)
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->memtag == sHeapMemtag;
    }
    else
    {
        return 0;
    }
}

int StringIsWeak(string target)
{
    if (target && target != sEmptyString)
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->memtag == sWeakMemtag;
    }
    else
    {
        return 0;
    }
}

int StringIsSmart(string target)
{
    if (target && target != sEmptyString)
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->memtag == sWeakMemtag || buffer->memtag == sHeapMemtag;
    }
    else
    {
        return 0;
    }
}

StringBuffer* StringBufferNew(int length)
{
    StringBuffer* buffer = (StringBuffer*)malloc(length + 1 + sizeof(StringBuffer));
    buffer->length = length;
    buffer->memref = 1;
    buffer->memtag = sHeapMemtag;
    return buffer;
}

string String(string source)
{
    if (StringIsHeap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        buffer->memref++;
        return source;
    }

    int length = StringLength(source);
    if (length == 0)
    {
        return sEmptyString;
    }
    else
    {
        StringBuffer* buffer = StringBufferNew(length);
        
        strncpy(buffer->data, source, length);
        
        return buffer->data;
    }
}

void StringFree(string target)
{
    if (StringIsHeap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        if (--buffer->memref <= 0)
        {
            free(buffer);
        }
    }   
}

string StringFrom(void* buffer, string source)
{
    int length = StringLength(source);
    if (length == 0)
    {
        return sEmptyString;
    }
    else
    {
        StringBuffer* stringBuffer = (StringBuffer*)buffer;
        stringBuffer->length = length;
        stringBuffer->memtag = sWeakMemtag;

        strncpy(stringBuffer->data, source, length);
        
        return stringBuffer->data;
    }
}

string StringFormat(int bufferSize, string format, ...)
{
    StringBuffer* buffer = StringBufferNew(bufferSize);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsnprintf(buffer->data, bufferSize, format, argv);
    va_end(argv);

    buffer->length = length;
    
    return buffer->data;
}

string StringFormatArgv(int bufferSize, string format, va_list argv)
{
    StringBuffer* buffer = StringBufferNew(bufferSize);
    buffer->length = (int)vsnprintf(buffer->data, bufferSize, format, argv);
    return buffer->data;
}

string StringFormatBuffer(void* buffer, string format, ...)
{
    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsprintf(stringBuffer->data, format, argv);
    va_end(argv);

    stringBuffer->length = length;
    stringBuffer->memtag = sWeakMemtag;
    
    return stringBuffer->data;
}

string StringFormatBufferArgv(void* buffer, string format, va_list argv)
{
    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    stringBuffer->length = (int)vsprintf(stringBuffer->data, format, argv);
    stringBuffer->memtag = sWeakMemtag;
    
    return stringBuffer->data;
}

int StringLength(string target)
{
    if (target == sEmptyString)
    {
        return 0;
    }

    if (StringIsSmart(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->length;
    }
    else
    {
        return (int)strlen(target);
    }
}

#endif // STRING_IMPL
