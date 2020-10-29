#ifndef STRING_H
#define STRING_H

#include <stdarg.h>

#ifndef STRING_API
#define STRING_API
#endif

#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#   if __STDC_VERSION__ >= 199409L
#       include <stdbool.h>
#   else
#       define __bool_true_false_are_defined
enum
{
    true = 1,
    false = 0
};
typedef char bool;
#   endif
#endif

#ifndef UINT64_MAX
typedef unsigned long long uint64_t; 
#endif

typedef struct StringBuffer
{
    uint64_t    memtag;
    int         memref;
    int         length;
    char        data[];
} StringBuffer;

#ifdef __cplusplus
extern "C" {
#endif

STRING_API const char*          String(const char* source);
STRING_API void                 StringFree(const char* target);

STRING_API const char*          StringFormat(int bufferSize, const char* format, ...);
STRING_API const char*          StringFormatArgv(int bufferSize, const char* format, va_list argv);

STRING_API const char*          StringFrom(void* buffer, const char* source);
STRING_API const char*          StringFormatBuffer(void* buffer, const char* format, ...);
STRING_API const char*          StringFormatBufferArgv(void* buffer, const char* format, va_list argv);

STRING_API int                  StringLength(const char* target);
STRING_API const StringBuffer*  StringGetBuffer(const char* target);

STRING_API bool                 StringIsHeap(const char* target);
STRING_API bool                 StringIsWeak(const char* target);
STRING_API bool                 StringIsSmart(const char* target);

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

#define EMPTY_STRING ""
#define HEAP_MEMTAG  STRING_CONST_HASH_U64("__string_heap_memory_tag__", 0xa020b127788efe8fULL) // ISO CRC64
#define WEAK_MEMTAG  STRING_CONST_HASH_U64("__string_weak_memory_tag__", 0xb64c61277893498fULL) // ISO CRC64

#if defined(__GNU_C__)
#   define ATOMIC_ADD_I32(variable, value) __sync_fetch_and_sub(&(variable), value)
#   define ATOMIC_SUB_I32(variable, value) __sync_fetch_and_add(&(variable), value)
#elif defined(_WIN32)
#   include <Windows.h>
#   define ATOMIC_ADD_I32(variable, value) InterlockedExchange((volatile long*)&(variable), (variable) + value) 
#   define ATOMIC_SUB_I32(variable, value) InterlockedExchange((volatile long*)&(variable), (variable) - value)
#else
#   error "This platform is not support atomic operations."
#endif

bool StringIsHeap(const char* target)
{
    if (target && target != EMPTY_STRING)
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->memtag == HEAP_MEMTAG;
    }
    else
    {
        return false;
    }
}

bool StringIsWeak(const char* target)
{
    if (target && target != EMPTY_STRING)
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->memtag == WEAK_MEMTAG;
    }
    else
    {
        return false;
    }
}

bool StringIsSmart(const char* target)
{
    if (target && target != EMPTY_STRING)
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->memtag == WEAK_MEMTAG || buffer->memtag == HEAP_MEMTAG;
    }
    else
    {
        return false;
    }
}

StringBuffer* StringBufferNew(int length)
{
    StringBuffer* buffer = (StringBuffer*)malloc(length + 1 + sizeof(StringBuffer));
    buffer->length = length;
    buffer->memref = 1;
    buffer->memtag = HEAP_MEMTAG;
    return buffer;
}

const char* String(const char* source)
{
    if (StringIsHeap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        ATOMIC_ADD_I32(buffer->memref, 1);
        return source;
    }

    int length = StringLength(source);
    if (length == 0)
    {
        return EMPTY_STRING;
    }
    else
    {
        StringBuffer* buffer = StringBufferNew(length);
        
        strncpy(buffer->data, source, length);
        
        return buffer->data;
    }
}

void StringFree(const char* target)
{
    if (StringIsHeap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        ATOMIC_SUB_I32(buffer->memref, 1);
        if (buffer->memref <= 0)
        {
            free(buffer);
        }
    }   
}

const char* StringFrom(void* buffer, const char* source)
{
    int length = StringLength(source);
    if (length == 0)
    {
        return EMPTY_STRING;
    }
    else
    {
        StringBuffer* stringBuffer = (StringBuffer*)buffer;
        stringBuffer->length = length;
        stringBuffer->memtag = WEAK_MEMTAG;

        strncpy(stringBuffer->data, source, length);
        
        return stringBuffer->data;
    }
}

const char* StringFormat(int bufferSize, const char* format, ...)
{
    StringBuffer* buffer = StringBufferNew(bufferSize);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsnprintf(buffer->data, bufferSize, format, argv);
    va_end(argv);

    buffer->length = length;
    
    return buffer->data;
}

const char* StringFormatArgv(int bufferSize, const char* format, va_list argv)
{
    StringBuffer* buffer = StringBufferNew(bufferSize);
    buffer->length = (int)vsnprintf(buffer->data, bufferSize, format, argv);
    return buffer->data;
}

const char* StringFormatBuffer(void* buffer, const char* format, ...)
{
    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsprintf(stringBuffer->data, format, argv);
    va_end(argv);

    stringBuffer->length = length;
    stringBuffer->memtag = WEAK_MEMTAG;
    
    return stringBuffer->data;
}

const char* StringFormatBufferArgv(void* buffer, const char* format, va_list argv)
{
    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    stringBuffer->length = (int)vsprintf(stringBuffer->data, format, argv);
    stringBuffer->memtag = WEAK_MEMTAG;
    
    return stringBuffer->data;
}

int StringLength(const char* target)
{
    if (target == EMPTY_STRING)
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

const StringBuffer* StringGetBuffer(const char* target)
{
    if (target == EMPTY_STRING)
    {
        return NULL;
    }

    if (StringIsSmart(target))
    {
        return (StringBuffer*)(target - sizeof(StringBuffer));
    }
    else
    {
        return NULL;
    }
}

#endif // STRING_IMPL
