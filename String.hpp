#pragma once

#include <stdarg.h>

#ifndef UINT64_MAX
typedef unsigned long long uint64_t;
#endif

struct String
{
    int         length;
    char*       buffer;
    uint64_t    hash;

    String();
    ~String();
    
    String(String&& string);
    String(const String& string);

    char*       GetBuffer();
    const char* GetBuffer() const;
    int         GetLength() const;

    bool        IsHeap();
    bool        IsWeak();
    bool        IsSmart();

    static String From(const char* source);
    static String From(void* buffer, const char* source);

    static String Format(int bufferSize, const char* format, ...);
    static String FormatArgv(int bufferSize, const char* format, va_list argv);

    static String Format(void* buffer, const char* format, ...);
    static String FormatArgv(void* buffer, const char* format, va_list argv);
};

struct StringView
{
    const char* buffer;

    int         length : 30;
    bool        isOwned : 1;
    bool        isStatic : 1;

    constexpr StringView()
        : buffer("")
        , length(0)
        , isOwned(false)
        , isStatic(true)
    {
    }

    template <int LENGTH>
    constexpr StringView(const char (&buffer)[LENGTH])
        : buffer(buffer)
        , length(LENGTH)
        , isOwned(false)
        , isStatic(true)
    {
    }
};