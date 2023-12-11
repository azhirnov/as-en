// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_UTF8PROC

# include "base/Algorithms/Cast.h"
# include "utf8proc.h"

namespace AE::Base
{

/*
=================================================
    Utf8CharCount
=================================================
*/
    ND_ forceinline usize  Utf8CharCount (const CharUtf8 *str, const usize length) __NE___
    {
        auto*   ptr     = Cast<utf8proc_uint8_t>(str);
        usize   count   = 0;

        for (usize pos = 0; pos < length; ++count)
        {
            utf8proc_int32_t    symb;
            utf8proc_ssize_t    res = utf8proc_iterate( ptr + pos, length - pos, OUT &symb );

            pos = res > 0 ? pos + res : pos + 1;
        }
        return count;
    }

    ND_ forceinline usize  Utf8CharCount (BasicStringView<CharUtf8> str) __NE___
    {
        return Utf8CharCount( str.data(), str.length() );
    }

/*
=================================================
    Utf8Decode
=================================================
*/
    ND_ forceinline CharUtf32  Utf8Decode (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
    {
        ASSERT( pos < length );
        StaticAssert( sizeof(utf8proc_uint8_t) == sizeof(*str) );

        utf8proc_int32_t    symb;
        utf8proc_ssize_t    res = utf8proc_iterate( Cast<utf8proc_uint8_t>(str + pos), length - pos, OUT &symb );

        ASSERT( res >= 0 );

        pos = res > 0 ? pos + res : pos + 1;
        return symb;
    }

    ND_ forceinline CharUtf32  Utf8Decode (BasicStringView<CharUtf8> str, INOUT usize &pos) __NE___
    {
        return Utf8Decode( str.data(), str.length(), INOUT pos );
    }

/*
=================================================
    Utf8Encode
=================================================
*/
    forceinline bool  Utf8Encode (const CharUtf32 symb, const usize size, OUT CharUtf8 *dst, INOUT usize &pos) __NE___
    {
        Unused( size );
        ASSERT( pos+4 <= size );
        StaticAssert( sizeof(utf8proc_uint8_t) == sizeof(*dst) );

        ssize   cnt = utf8proc_encode_char( symb, Cast<utf8proc_uint8_t>(dst + pos) );
        ASSERT( cnt > 0 and cnt <= 4 );

        pos += cnt;
        return cnt > 0;
    }

/*
=================================================
    Utf32IsValid
=================================================
*/
    ND_ forceinline bool  Utf32IsValid (CharUtf32 c) __NE___
    {
        return utf8proc_codepoint_valid( c );
    }


} // AE::Base

#endif // AE_ENABLE_UTF8PROC
