// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base
{
    static constexpr inline Bytes   DefaultAllocatorAlign   {__STDCPP_DEFAULT_NEW_ALIGNMENT__};
    static constexpr inline Bytes   SmallAllocationSize     {4 << 10};      // 4 Kb
    static constexpr inline Bytes   LargeAllocationSize     {4 << 20};      // 4 Mb - large page size in WIndows
    static constexpr inline Bytes   DefaultAllocationSize   = SmallAllocationSize;

/*
=================================================
    AddressOf
=================================================
*/
    template <typename T>
    ND_ constexpr decltype(auto)  AddressOf (T &value) __NE___
    {
        return std::addressof( value );
    }

    template <typename T>
    ND_ constexpr decltype(auto)  VAddressOf (T &value) __NE___
    {
        return Cast<void>( std::addressof( value ));
    }

/*
=================================================
    AddressDistance
=================================================
*/
    template <typename LT, typename RT>
    ND_ Bytes  AddressDistance (LT &lhs, RT &rhs) __NE___
    {
        return Bytes{ usize(AddressOf(lhs)) - usize(AddressOf(rhs)) };
    }

/*
=================================================
    DbgInitMem
=================================================
*/
#ifdef AE_DEBUG
    inline void  DbgInitMem (OUT void* ptr, Bytes size) __NE___
    {
        ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
        std::memset( OUT ptr, 0xCD, usize{size} );
    }

    template <typename T>
    void  DbgInitMem (OUT T& value) __NE___
    {
        DbgInitMem( OUT &value, SizeOf<T> );
    }
#endif

/*
=================================================
    DbgFreeMem
=================================================
*/
#ifdef AE_DEBUG
    inline void  DbgFreeMem (OUT void* ptr, Bytes size) __NE___
    {
        ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
        std::memset( OUT ptr, 0xFE, usize{size} );
    }

    template <typename T>
    void  DbgFreeMem (OUT T& value) __NE___
    {
        DbgFreeMem( OUT &value, SizeOf<T> );
    }
#endif

/*
=================================================
    PlacementNew
----
    copy-ctor may throw
    default-ctor & move-ctor should not throw
=================================================
*/
    template <typename T, typename ...Args>
    T*  PlacementNew (OUT void* ptr, Args&&... args)  NoExcept(IsNothrowCtor< T, Args... >)
    {
        ASSERT( ptr != null );
        CheckPointerCast<T>( ptr );

        return ( new(ptr) T{ FwdArg<Args>(args)... });
    }

/*
=================================================
    PlacementDelete
=================================================
*/
    template <typename T>
    void  PlacementDelete (INOUT T &val) __NE___
    {
        StaticAssert( std::is_nothrow_destructible_v<T> );
        val.~T();

        DEBUG_ONLY( DbgFreeMem( val ));
    }

/*
=================================================
    Reconstruct
=================================================
*/
    template <typename T, typename ...Args>
    void  Reconstruct (INOUT T &value, Args&& ...args) __NE___
    {
        CheckPointerCast<T>( &value );

        StaticAssert( std::is_nothrow_destructible_v<T> );

        if constexpr( IsNothrowCtor< T, Args... >)
        {
            value.~T();
            DEBUG_ONLY( DbgFreeMem( value ));

            new(&value) T{ FwdArg<Args>(args)... };
        }
        else
        {
            TRY{
                value.~T();
                DEBUG_ONLY( DbgFreeMem( value ));

                new(&value) T{ FwdArg<Args>(args)... };
            }
            CATCH_ALL(
                DBG_WARNING( "exception in ctor!" );
                DEBUG_ONLY( DbgFreeMem( value ));
            );
        }
    }

/*
=================================================
    MemCopy
----
    memory must not intersects
=================================================
*/
    template <typename T1, typename T2>
    void  MemCopy (OUT T1 &dst, const T2 &src) __NE___
    {
        StaticAssert( sizeof(dst) >= sizeof(src) );
        StaticAssert( IsMemCopyAvailable<T1> );
        StaticAssert( IsMemCopyAvailable<T2> );
        StaticAssert( not IsConst<T1> );
        ASSERT( static_cast<const void *>(&dst) != static_cast<const void *>(&src) );

        std::memcpy( OUT &dst, &src, sizeof(src) );
    }

    inline void  MemCopy (OUT void* dst, const void* src, const Bytes size) __NE___
    {
        if_likely( size > 0 )
        {
            ASSERT( (dst != null) and (src != null) );
            ASSERT( not IsIntersects<const void *>( dst, dst + size, src, src + size ));

            std::memcpy( OUT dst, src, usize(size) );
        }
    }

    inline void  MemCopy (OUT void* dst, Bytes dstSize, const void* src, const Bytes srcSize) __NE___
    {
        if_likely( srcSize > 0 )
        {
            ASSERT( srcSize <= dstSize );
            ASSERT( (dst != null) and (src != null) );
            ASSERT( not IsIntersects<const void *>( dst, dst + dstSize, src, src + srcSize ));

            std::memcpy( OUT dst, src, usize(std::min(srcSize, dstSize)) );
        }
    }

    template <typename T>
    inline void  MemCopy (OUT T* dst, const T* src, const usize count) __NE___
    {
        StaticAssert( IsMemCopyAvailable<T> );
        if_likely( count > 0 )
        {
            ASSERT( (dst != null) and (src != null) );
            ASSERT( not IsIntersects<const void *>( dst, dst + count, src, src + count ));

            std::memcpy( OUT dst, src, sizeof(T)*count );
        }
    }

/*
=================================================
    MemMove
----
    memory may intersects
=================================================
*/
    inline void  MemMove (OUT void* dst, const void* src, Bytes size) __NE___
    {
        if_likely( size > 0 )
        {
            ASSERT( (dst != null) and (src != null) );

            std::memmove( OUT dst, src, usize(size) );
        }
    }

    inline void  MemMove (OUT void* dst, Bytes dstSize, const void* src, Bytes srcSize) __NE___
    {
        if_likely( srcSize > 0 )
        {
            ASSERT( srcSize <= dstSize );
            ASSERT( (dst != null) and (src != null) );

            std::memmove( OUT dst, src, usize(std::min(srcSize, dstSize)) );
        }
    }

    template <typename T>
    inline void  MemMove (OUT T* dst, const T* src, const usize count) __NE___
    {
        StaticAssert( IsMemCopyAvailable<T> );
        if_likely( count > 0 )
        {
            ASSERT( (dst != null) and (src != null) );

            std::memmove( OUT dst, src, sizeof(T)*count );
        }
    }

/*
=================================================
    ZeroMem
=================================================
*/
    template <typename T>
    void  ZeroMem (OUT T& value) __NE___
    {
        StaticAssert( IsZeroMemAvailable<T> );
        StaticAssert( not IsPointer<T> );

        std::memset( OUT &value, 0, sizeof(value) );
    }

    inline void  ZeroMem (OUT void* ptr, Bytes size) __NE___
    {
        ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
        std::memset( OUT ptr, 0, usize{size} );
    }

    template <typename T>
    void  ZeroMem (OUT T* ptr, usize count) __NE___
    {
        StaticAssert( IsZeroMemAvailable<T> );
        ASSERT( (count == 0) or ((ptr != null) == (count != 0)) );

        std::memset( OUT ptr, 0, sizeof(T) * count );
    }

/*
=================================================
    SecureZeroMem
=================================================
*/
    template <typename T>
    void  SecureZeroMem (OUT T& value) __NE___
    {
        StaticAssert( IsZeroMemAvailable<T> );
        StaticAssert( not IsPointer<T> );

        SecureZeroMem( OUT &value, SizeOf<T> );
    }

    void  SecureZeroMem (OUT void* ptr, Bytes size) __NE___;    // native

    namespace _hidden_ {
        void  SecureZeroMemFallback (OUT void* ptr, Bytes size) __NE___;    // software
    }

/*
=================================================
    CopyCString
=================================================
*/
    template <usize S1, usize S2>
    void  CopyCString (OUT char (&dst)[S1], const char (&src)[S2])
    {
        StaticAssert( S1 >= S2 );
        #ifdef AE_COMPILER_MSVC
            Unused( ::strcpy_s( OUT dst, src ));
        #else
            ::strcpy( OUT dst, src );
        #endif
    }

/*
=================================================
    MemEqual
=================================================
*/
    ND_ inline bool  MemEqual (const void* lhs, const void* rhs, Bytes size) __NE___
    {
        return std::memcmp( lhs, rhs, usize(size) ) == 0;
    }

    template <typename T>
    ND_ bool  MemEqual (const T &lhs, const T &rhs) __NE___
    {
        return std::memcmp( &lhs, &rhs, sizeof(T) ) == 0;
    }

/*
=================================================
    MemLess
=================================================
*/
    ND_ inline bool  MemLess (const void* lhs, const void* rhs, Bytes size) __NE___
    {
        return std::memcmp( lhs, rhs, usize(size) ) < 0;
    }

    template <typename T>
    ND_ bool  MemLess (const T &lhs, const T &rhs) __NE___
    {
        return std::memcmp( &lhs, &rhs, sizeof(T) ) < 0;
    }


} // AE::Base
