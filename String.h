#ifndef STRING_H
#define STRING_H

#include <stdarg.h>

#ifndef STRING_API
#define STRING_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

STRING_API char* String(int size, const char* format, ...);
STRING_API void  StringFree(char* string);

STRING_API char* StringFormat(char* string, const char* format, ...);
STRING_API char* StringFormatArgv(char* string, const char* format, va_list argv);

STRING_API int   StringSize(const char* string);
STRING_API int   StringLength(const char* string);

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

char* String(int size, const char* format, ...)
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

void StringFree(char* string)
{
    if (string)
    {
        StringBuffer* buffer = (StringBuffer*)(string - sizeof(StringBuffer));
        if (buffer != &sEmptyStringBuffer)
        {
            free(buffer);
        }
    }   
}

char* StringFormat(char* string, const char* format, ...)
{
    if (string)
    {
        va_list argv;
        va_start(argv, format);
        StringFormat(string, format, argv);
        va_end(argv);
    }
    
    return string;
}

char* StringFormatArgv(char* string, const char* format, va_list argv)
{
    vsprintf(string, format, argv);
    return string;
}

int StringSize(const char* string)
{
    if (string)
    {
        StringBuffer* buffer = (StringBuffer*)(string - sizeof(StringBuffer));
        return buffer->size;
    }

    return 0;
}

int StringLength(const char* string)
{
    return (int)strlen(string);
}

#endif // STRIGN_IMPL
