#ifndef STRING_SAFE_H
#define STRING_SAFE_H

#include <stdarg.h>

#ifndef STRING_API
#define STRING_API
#endif

#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#   if __STDC_VERSION__ >= 199409L
#       include <stdbool.h>
#   else
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

typedef struct String
{
    int         length;
    const char* buffer;
} String;

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

STRING_API String               string_weak(const char* source);
STRING_API String               string_from(const char* source);
STRING_API void                 string_free(String* target);

STRING_API String               string_format(int bufferSize, const char* format, ...);
STRING_API String               string_format_argv(int bufferSize, const char* format, va_list argv);

STRING_API String               string_from_buffer(void* buffer, const char* source);
STRING_API String               string_format_buffer(void* buffer, const char* format, ...);
STRING_API String               string_format_buffer_argv(void* buffer, const char* format, va_list argv);

STRING_API int                  string_length(const char* target);
STRING_API const StringBuffer*  string_buffer(const char* target);

STRING_API bool                 string_is_heap(const char* target);
STRING_API bool                 string_is_weak(const char* target);
STRING_API bool                 string_is_smart(const char* target);

#ifdef __cplusplus
}
#endif

#endif // STRING_SAFE_H

#ifdef STRING_SAFE_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef STRING_CONST_HASH_U64
#define STRING_CONST_HASH_U64(string, defaultValue) (uint64_t)(defaultValue)
#endif

#define EMPTY_STRING ""
#define HEAP_MEMTAG  STRING_CONST_HASH_U64("__string_heap_memory_tag__", 0xa020b127788efe8fULL) // ISO CRC64
#define WEAK_MEMTAG  STRING_CONST_HASH_U64("__string_weak_memory_tag__", 0xb64c61277893498fULL) // ISO CRC64

bool string_is_heap(const char* target)
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

bool string_is_weak(const char* target)
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

bool string_is_smart(const char* target)
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

StringBuffer* string_new_buffer(int length)
{
    StringBuffer* buffer = (StringBuffer*)malloc(length + 1 + sizeof(StringBuffer));
    buffer->length = length;
    buffer->memref = 1;
    buffer->memtag = HEAP_MEMTAG;
    return buffer;
}

String string_weak(const char* source)
{
    String result = { string_length(source), source ? source : EMPTY_STRING };
    return result;
}

String string_from(const char* source)
{
    if (string_is_heap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        buffer->memref++;
        
        String result = { buffer->length, buffer->data };
        return result;
    }

    int length = string_length(source);
    if (length == 0)
    {
        String result = { 0, EMPTY_STRING };
        return result;
    }
    else
    {
        StringBuffer* buffer = string_new_buffer(length);
        strncpy(buffer->data, source, length);
        
        String result = { buffer->length, buffer->data };
        return result;
    }
}

void string_free(String* target)
{
    if (string_is_heap(target->buffer))
    {
        StringBuffer* buffer = (StringBuffer*)(target->buffer - sizeof(StringBuffer));
        if (--buffer->memref <= 0)
        {
            free(buffer);
        }
    }

    target->length = 0;
    target->buffer = EMPTY_STRING;
}

String string_from_buffer(void* buffer, const char* source)
{
    int length = string_length(source);
    if (length == 0)
    {
        String result = { 0, EMPTY_STRING };
        return result;
    }
    else
    {
        StringBuffer* stringBuffer = (StringBuffer*)buffer;
        stringBuffer->length = length;
        stringBuffer->memtag = WEAK_MEMTAG;

        strncpy(stringBuffer->data, source, length);
        
        String result = { stringBuffer->length, stringBuffer->data };
        return result;
    }
}

String string_format(int bufferSize, const char* format, ...)
{
    StringBuffer* buffer = string_new_buffer(bufferSize);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsnprintf(buffer->data, bufferSize, format, argv);
    va_end(argv);

    buffer->length = length;
    
    String result = { buffer->length, buffer->data };
    return result;
}

String string_format_argv(int bufferSize, const char* format, va_list argv)
{
    StringBuffer* buffer = string_new_buffer(bufferSize);
    buffer->length = (int)vsnprintf(buffer->data, bufferSize, format, argv);

    String result = { buffer->length, buffer->data };
    return result;
}

String string_format_buffer(void* buffer, const char* format, ...)
{
    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsprintf(stringBuffer->data, format, argv);
    va_end(argv);

    stringBuffer->length = length;
    stringBuffer->memtag = WEAK_MEMTAG;
    
    String result = { stringBuffer->length, stringBuffer->data };
    return result;
}

String string_format_buffer_argv(void* buffer, const char* format, va_list argv)
{
    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    stringBuffer->length = (int)vsprintf(stringBuffer->data, format, argv);
    stringBuffer->memtag = WEAK_MEMTAG;
    
    String result = { stringBuffer->length, stringBuffer->data };
    return result;
}

int string_length(const char* target)
{
    if (!target || target == EMPTY_STRING)
    {
        return 0;
    }

    if (string_is_smart(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->length;
    }
    else
    {
        return (int)strlen(target);
    }
}

const StringBuffer* string_buffer(const char* target)
{
    if (!target || target == EMPTY_STRING)
    {
        return NULL;
    }

    if (string_is_smart(target))
    {
        return (StringBuffer*)(target - sizeof(StringBuffer));
    }
    else
    {
        return NULL;
    }
}

#endif // STRING_IMPL
