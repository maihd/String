#ifndef STRING_H
#define STRING_H

#include <stdarg.h>

#ifndef STRING_API
#define STRING_API
#endif

#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#   if (__STDC_VERSION__ >= 199409L)
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

#ifndef STRING_ASSERT
#include <assert.h>
#define STRING_ASSERT(test, message) assert((test) && (message))
#endif

#define STRING_EMPTY ""

#define STRING_MEMTAG_HEAP  STRING_CONST_HASH_U64("__string_heap_memory_tag__", 0xa020b127788efe8fULL) // ISO CRC64
#define STRING_MEMTAG_WEAK  STRING_CONST_HASH_U64("__string_weak_memory_tag__", 0xb64c61277893498fULL) // ISO CRC64

#if defined(__GNUC__) || defined(__clang__)
#   if defined(__GNUC__) && (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40800
#       define STRING_ATOMIC_ADD_I32(variable, value) __atomic_add_fetch(&(variable), value, __ATOMIC_RELAXED)
#       define STRING_ATOMIC_SUB_I32(variable, value) __atomic_sub_fetch(&(variable), value, __ATOMIC_RELAXED)
#   else
#       define STRING_ATOMIC_ADD_I32(variable, value) __sync_add_and_fetch(&(variable), value)
#       define STRING_ATOMIC_SUB_I32(variable, value) __sync_sub_and_fetch(&(variable), value)
#   endif
#elif defined(_WIN32)
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   define STRING_ATOMIC_ADD_I32(variable, value) InterlockedExchange((volatile long*)&(variable), (variable) + value) 
#   define STRING_ATOMIC_SUB_I32(variable, value) InterlockedExchange((volatile long*)&(variable), (variable) - value)
#elif defined(__STDC_VERSION_) && (__STDC_VERSION_ >= _201112L)
#   include <stdatomic.h>
#   define STRING_ATOMIC_ADD_I32(variable, value) atomic_sub_fetch((atomic_int*)&(variable), value)
#   define STRING_ATOMIC_SUB_I32(variable, value) atomic_add_fetch((atomic_int*)&(variable), value)
#else
#   define STRING_ATOMIC_ADD_I32(variable, value) ((variable) += (value))
#   define STRING_ATOMIC_SUB_I32(variable, value) ((variable) -= (value))
#endif

bool StringIsHeap(const char* target)
{
    STRING_ASSERT(target != NULL, "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_HEAP;
}

bool StringIsWeak(const char* target)
{
    STRING_ASSERT(target != NULL, "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_WEAK;
}

bool StringIsSmart(const char* target)
{
    STRING_ASSERT(target != NULL, "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_WEAK || buffer->memtag == STRING_MEMTAG_HEAP;
}

StringBuffer* StringBufferNew(int length)
{
    StringBuffer* buffer = (StringBuffer*)malloc(length + 1 + sizeof(StringBuffer));
    STRING_ASSERT(buffer != NULL, "Cannot create new buffer, maybe the system have been out of memory.");

    buffer->memtag = STRING_MEMTAG_HEAP;
    buffer->memref = 1;
    buffer->length = length;
    return buffer;
}

const char* String(const char* source)
{
    STRING_ASSERT(source != NULL, "Attempt to use null-pointer on string");

    if (StringIsHeap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        STRING_ATOMIC_ADD_I32(buffer->memref, 1);
        return source;
    }

    int length = StringLength(source);
    if (length == 0)
    {
        return STRING_EMPTY;
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
    STRING_ASSERT(target != NULL, "Attempt to use null-pointer on string");

    if (StringIsHeap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        
        int memref = STRING_ATOMIC_SUB_I32(buffer->memref, 1);
        if (memref <= 0)
        {
            free(buffer);
        }
    }   
}

const char* StringFrom(void* buffer, const char* source)
{
    STRING_ASSERT(buffer != NULL, "Attempt to use null-pointer on buffer");
    STRING_ASSERT(source != NULL, "Attempt to use null-pointer on string");

    int length = StringLength(source);
    if (length == 0)
    {
        return STRING_EMPTY;
    }
    else
    {
        StringBuffer* stringBuffer = (StringBuffer*)buffer;
        stringBuffer->memtag = STRING_MEMTAG_WEAK;
        stringBuffer->memref = 1;
        stringBuffer->length = length;

        strncpy(stringBuffer->data, source, length);
        stringBuffer->data[length] = '\0';
        
        return stringBuffer->data;
    }
}

const char* StringFormat(int bufferSize, const char* format, ...)
{
    STRING_ASSERT(format != NULL, "Attempt to use null-pointer on string");

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
    STRING_ASSERT(format != NULL, "Attempt to use null-pointer on string");

    StringBuffer* buffer = StringBufferNew(bufferSize);
    buffer->length = (int)vsnprintf(buffer->data, bufferSize, format, argv);
    return buffer->data;
}

const char* StringFormatBuffer(void* buffer, const char* format, ...)
{
    STRING_ASSERT(buffer != NULL, "Attempt to use null-pointer on buffer");
    STRING_ASSERT(format != NULL, "Attempt to use null-pointer on string");

    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsprintf(stringBuffer->data, format, argv);
    va_end(argv);

    stringBuffer->memtag = STRING_MEMTAG_WEAK;
    stringBuffer->memref = 1;
    stringBuffer->length = length;
    
    return stringBuffer->data;
}

const char* StringFormatBufferArgv(void* buffer, const char* format, va_list argv)
{
    STRING_ASSERT(buffer != NULL, "Attempt to use null-pointer on buffer");
    STRING_ASSERT(format != NULL, "Attempt to use null-pointer on string");

    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    stringBuffer->memtag = STRING_MEMTAG_WEAK;
    stringBuffer->memref = 1;
    stringBuffer->length = (int)vsprintf(stringBuffer->data, format, argv);
    
    return stringBuffer->data;
}

int StringLength(const char* target)
{
    STRING_ASSERT(target != NULL, "Attempt to use null-pointer on string");

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
    STRING_ASSERT(target != NULL, "Attempt to use null-pointer on string");

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
// EOF, required and empty newline
