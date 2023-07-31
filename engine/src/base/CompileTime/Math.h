// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

/*
=================================================
    CT_IntLog2
=================================================
*/
namespace _hidden_
{
    template <typename T, T X, usize Bit>
    struct _IntLog2 {
        static const int    value = int((X >> Bit) != 0) + _IntLog2<T, X, Bit-1 >::value;
    };

    template <typename T, T X>
    struct _IntLog2< T, X, 0 > {
        static const int    value = 0;
    };

} // _hidden_

    template <auto X>
    static constexpr int    CT_IntLog2 = (X ? Base::_hidden_::_IntLog2< decltype(X), X, CT_SizeofInBits(X)-1 >::value : -1);

/*
=================================================
    CT_IsPowerOfTwo
=================================================
*/
namespace _hidden_
{
    template <ulong X>
    struct _IsPowerOfTwo {
        static constexpr bool   value = (X != 0) & ((X & (X - 1)) == 0);
    };
} // _hidden_

    template <auto X>
    static constexpr bool   CT_IsPowerOfTwo = Base::_hidden_::_IsPowerOfTwo< ulong(X) >::value;

/*
=================================================
    CT_CeilIntLog2
=================================================
*/
namespace _hidden_
{
    template <auto X>
    struct _CeilIntLog2 {
        static constexpr int    il2     = CT_IntLog2<X>;
        static constexpr int    value   = il2 >= 0 ? il2 + int(not CT_IsPowerOfTwo<X>) : -1;
    };
} // _hidden_

    template <auto X>
    static constexpr int    CT_CeilIntLog2 = Base::_hidden_::_CeilIntLog2<X>::value;

/*
=================================================
    CT_FloorPOT
=================================================
*/
namespace _hidden_
{
    template <auto X>
    struct _FloorPOT {
        using T                         = decltype(X);
        static constexpr int    il2     = CT_IntLog2<X>;
        static constexpr T      value   = il2 >= 0 ? (T{1} << il2) : T{0};
    };
} // _hidden_

    template <auto X>
    static constexpr auto   CT_FloorPOT = Base::_hidden_::_FloorPOT<X>::value;

/*
=================================================
    CT_Pow
=================================================
*/
    template <auto Power, typename T>
    inline constexpr T  CT_Pow (const T &base)
    {
        STATIC_ASSERT( IsInteger<T> and IsInteger<decltype(Power)> and Power >= 0 );

        if constexpr( Power == 0 )
        {
            Unused( base );
            return 1;
        }
        else
            return CT_Pow<Power-1>( base ) * base;
    }

/*
=================================================
    CT_BitCount
=================================================
*/
#ifdef __cpp_lib_bitops
    template <auto X>
    static constexpr usize      CT_BitCount = std::popcount(ulong( X ));    // C++20

#else
namespace _hidden_
{
    template <typename T>
    ND_ constexpr uint  NumberOfSetBits (T i)
    {
        if constexpr (sizeof(T) <= 4)
        {
            i = i - ((i >> 1) & 0x55555555);
            i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
            i = (i + (i >> 4)) & 0x0F0F0F0F;
            return (i * 0x01010101) >> 24;
        }
        else
        {
            return NumberOfSetBits( uint( i & 0xFFFFFFFF )) + NumberOfSetBits( uint( i >> 32 ));
        }
    }
}
    template <auto X>
    static constexpr usize      CT_BitCount = Base::_hidden_::NumberOfSetBits( X );
#endif

/*
=================================================
    CT_ToBitMask
=================================================
*/
#ifdef AE_COMPILER_MSVC
# pragma warning (push)
# pragma warning (disable: 4293)
#endif

namespace _hidden_
{
    template <typename R, auto Count>
    struct _ToBitMask {
        STATIC_ASSERT( IsUnsignedInteger<R> );
        static constexpr R  mask    =   Count >= CT_SizeOfInBits<R> ? ~R{0} :
                                        Count <  0                  ?  R{0} :
                                                                      (R{1} << Count) - 1;
    };
} // _hidden_

#ifdef AE_COMPILER_MSVC
# pragma warning (pop)
#endif

    template <typename T, auto Count>
    static constexpr T  CT_ToBitMask = _hidden_::_ToBitMask< T, Count >::mask;


} // AE::Base
