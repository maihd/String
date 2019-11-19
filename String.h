#ifndef STRING_H
#define STRING_H

#include <stdarg.h>

#ifndef STRING_API
#define STRING_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// NO POINTER for safety
typedef char* string;

STRING_API string String(int size, const string format, ...);
STRING_API void   StringFree(string target);

STRING_API string StringFormat(string target, const string format, ...);
STRING_API string StringFormatArgv(string target, const string format, va_list argv);

STRING_API int    StringSize(const string target);
STRING_API int    StringLength(const string target);

STRING_API int    StringIsValid(const string target);

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
    int  size;
    char data[];
} StringBuffer;

static StringBuffer sEmptyStringBuffer;
static string       sEmptyString = sEmptyStringBuffer.data;

int StringIsValid(const string target)
{
    return target && target != sEmptyString;
}

string String(int size, const string format, ...)
{
    if (size <= 0)
    {
        return sEmptyStringBuffer.data;
    }

    StringBuffer* buffer = (StringBuffer*)malloc(size + sizeof(StringBuffer));
    buffer->size = 0;
    
    va_list argv;
    va_start(argv, format);
    StringFormatArgv(buffer->data, format, argv);
    va_end(argv);
    
    return buffer->data;
}

void StringFree(string target)
{
    if (StringIsValid(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        if (buffer != &sEmptyStringBuffer)
        {
            free(buffer);
        }
    }   
}

string StringFormat(string target, const string format, ...)
{
    if (StringIsValid(target))
    {
        va_list argv;
        va_start(argv, format);
        StringFormat(target, format, argv);
        va_end(argv);
    }
    
    return target;
}

string StringFormatArgv(string target, const string format, va_list argv)
{
    vsprintf(target, format, argv);
    return target;
}

int StringSize(const string target)
{
    if (StringIsValid(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->size;
    }

    return 0;
}

int StringLength(const string target)
{
    return (int)strlen(target);
}

#endif // STRIGN_IMPL
