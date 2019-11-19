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

//STRING_API string StringFormatBuffer(char[] buffer, string format, ...);
//STRING_API string StringFormatBufferArgv(char[] buffer, string format, va_list argv);

STRING_API int    StringLength(string target);

STRING_API int    StringIsHeap(string target);

#ifdef __cplusplus
}
#endif

#endif // STRING_H

#ifdef STRING_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct StringBuffer
{
    int  length;
    //int  memref;
    long memtag;
    char data[];
} StringBuffer;

static string sEmptyString = "";
static long   sMemoryTag   = (long)sEmptyString;

int StringIsHeap(const string target)
{
    if (target && target != sEmptyString)
    {
        StringBuffer* buffer = (StringBuffer*)target;
        return buffer->memtag == sMemoryTag;
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
    buffer->memtag = sMemoryTag;
    return buffer;
}

string String(string source)
{
    if (size <= 0)
    {
        return sEmptyStringBuffer.data;
    }

    int length = StringLength(source);
    StringBuffer* buffer = StringBufferNew(length);
    
    strncpy(buffer->data, length, source);
    
    return buffer->data;
}

void StringFree(string target)
{
    if (StringIsHeap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        free(buffer);
    }   
}

string StringFormat(int bufferSize, string format, ...)
{
    StringBuffer* buffer = StringBufferNew(bufferSize);

    va_list argv;
    va_start(argv, format);
    int length = (int)vsprintf(target, format, argv);
    va_end(argv);

    buffer->length = length;
    
    return buffer->data;
}

string StringFormatArgv(string target, string format, va_list argv)
{
    StringBuffer* buffer = StringBufferNew(bufferSize);
    buffer->length = vsprintf(target, format, argv);
    return target;
}

int StringLength(string target)
{
    if (StringIsHeap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->length;
    }
    else
    {
        return (int)strlen(target);
    }
}

#endif // STRIGN_IMPL
