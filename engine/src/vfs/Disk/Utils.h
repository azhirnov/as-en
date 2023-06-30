// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"

#ifndef AE_ENABLE_UTF8PROC
#   error Utf8Proc is required
#endif

namespace AE::VFS
{
namespace
{

/*
=================================================
    Convert
=================================================
*/
    template <typename T, typename Alloc>
    ND_ bool  Convert (OUT StringView &result, BasicStringView<T> path, Alloc &allocator)
    {
        if constexpr( IsSameTypes< T, CharAnsi >)
        {
            CHECK_ERR( path.length() <= FileName::MaxStringLength() );

            char*   str = allocator.template Allocate<char>( path.length()+1 );
            CHECK_ERR( str != null );

            std::memcpy( OUT str, path.data(), path.length() );
            str[path.length()] = 0;

            result = StringView{ str, path.length() };
            return true;
        }
        else
        if constexpr( IsSameTypes< T, CharUtf8 >)
        {
            const usize count = Utf8CharCount( path );
            CHECK_ERR( count <= FileName::MaxStringLength() );

            char*   str = allocator.template Allocate<char>( count+1 );
            CHECK_ERR( str != null );

            usize   len = path.length();
            bool    res = Utf8ToAnsi( OUT str, path.data(), INOUT len );
            ASSERT( count == len );

            result = StringView{ str, len };
            return res;
        }
        else
        if constexpr( IsSameTypes< T, wchar_t > or IsSameTypes< T, CharUtf32 >)
        {
            CHECK_ERR( path.length() <= FileName::MaxStringLength() );

            char*   str = allocator.template Allocate<char>( path.length()+1 );
            CHECK_ERR( str != null );

            bool    res = WCharToAnsi( OUT str, path.data(), path.length() );

            result = StringView{ str, path.length() };
            return res;
        }
    }

/*
=================================================
    Convert
=================================================
*/
    template <typename T>
    ND_ bool  Convert (OUT String &result, BasicStringView<T> path)
    {
        result.clear();

        if constexpr( IsSameTypes< T, CharAnsi >)
        {
            CHECK_ERR( path.length() <= FileName::MaxStringLength() );

            result = path;
            return true;
        }
        else
        if constexpr( IsSameTypes< T, CharUtf8 >)
        {
            const usize count = Utf8CharCount( path );
            CHECK_ERR( count <= FileName::MaxStringLength() );

            result.resize( count );

            usize   len = path.length();
            bool    res = Utf8ToAnsi( OUT result, path.data(), INOUT len );
            ASSERT( count == len );

            return res;
        }
        else
        if constexpr( IsSameTypes< T, wchar_t > or IsSameTypes< T, CharUtf32 >)
        {
            CHECK_ERR( path.length() <= FileName::MaxStringLength() );

            result.resize( path.length() );
            return WCharToAnsi( OUT result.data(), path.data(), path.length() );
        }
    }

} // namespace
} // AE::VFS
