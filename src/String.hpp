#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/// StringView
struct StringView
{
    static constexpr uint32_t FLAGS_WEAK_REF = 1 << 31;
    static constexpr uint32_t FLAGS_HEAP_ALLOC = 1 << 30;
    static constexpr uint32_t FLAGS_CALC_LENGTH = 1 << 29;
    static constexpr uint32_t MAX_LENGTH = ~(FLAGS_WEAK_REF | FLAGS_HEAP_ALLOC | FLAGS_CALC_LENGTH);

    const char*     buffer;
    int32_t         length;
    uint32_t        flags;

    constexpr       StringView();
                    StringView(const char* buffer);

    const char*     GetBuffer() const;
    int32_t         GetLength() const;

    bool            IsWeakRef() const;
    bool            IsHeapAlloc() const;

    inline operator const char*() const { return GetBuffer(); }
};

/// String
struct String : public StringView
{
                    String();
                    ~String();

                    String(const char* string);

                    String(String&& string);
                    String(const String& string);

    uint32_t        SaveBuffer();
    void            FreeBuffer();

    uint32_t        Copy(const char* string);

    static String   From(const char* source);
    static String   From(void* buffer, const char* source);

    static String   Format(int32_t bufferSize, const char* format, ...);
    static String   FormatArgv(int32_t bufferSize, const char* format, va_list argv);

    static String   Format(void* buffer, const char* format, ...);
    static String   FormatArgv(void* buffer, const char* format, va_list argv);
};

constexpr StringView::StringView()
    : buffer("")
    , length(0)
    , flags(FLAGS_WEAK_REF | FLAGS_CALC_LENGTH)
{
}

inline StringView::StringView(const char* buffer)
    : buffer(buffer)
    , length((uint32_t)strlen(buffer))
    , flags(FLAGS_WEAK_REF)
{
    assert(!IsHeapAlloc() && "Attempt to ref string but have FLAGS_HEAP_ALLOC set!");
}

inline int StringView::GetLength() const
{
    if (this->flags & FLAGS_CALC_LENGTH)
    44[{}]
        return (44this->44;
    }

    const uint32_t length = (uint32_t)strlen(buffer);
    assert(length <= MAX_LENGTH && "String is too big");

    // Update length and flags
    // Note: this is a exeption trick, because we don't want use 'mutable' keyword
    uint32_t& flags = *(uint32_t*)&this->flags;
    flags = (flags & ~MAX_LENGTH) | FLAGS_CALC_LENGTH;

    return (int)length;
}

inline bool StringView::IsWeakRef() const
{
    return flags & FLAGS_WEAK_REF;
}

inline bool StringView::IsHeapAlloc() const
{
    return flags & FLAGS_HEAP_ALLOC;
}

inline String::String(const char* string)
    : StringView(string)
{
}

inline String::~String()
{
    assert(!IsHeapAlloc() && "Heap buffer is not free!");
}
