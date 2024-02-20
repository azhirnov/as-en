// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Byte.h"

namespace AE::Math
{

    //
    // Range
    //

    template <typename T>
    struct Range
    {
        //StaticAssert( IsScalar<T> or IsBytes<T> );

    // types
    public:
        using Value_t   = T;
        using Self      = Range<T>;


    // variables
    public:
        T   begin, end;


    // methods
    public:
        constexpr Range ()                                          __NE___ : begin{}, end{}
        {
          #ifdef AE_COMPILETIME_OFFSETOF
            // check if supported cast Rectangle to array
            StaticAssert( offsetof(Self, begin) + sizeof(T) == offsetof(Self, end) );
          #endif
        }

        template <typename BT, typename ET>
        constexpr Range (BT begin, ET end)                          __NE___ :
            begin{begin}, end{end}
        {}

        ND_ constexpr T     Offset ()                               C_NE___ { return begin; }
        ND_ constexpr T     Size ()                                 C_NE___ { return end - begin; }
        ND_ constexpr T     Middle ()                               C_NE___ { return Average( begin, end ); }

        ND_ constexpr bool  IsEmpty ()                              C_NE___ { return begin == end; }
        ND_ constexpr bool  IsInvalid ()                            C_NE___ { return end < begin; }
        ND_ constexpr bool  IsValid ()                              C_NE___ { return not IsInvalid(); }

        ND_ constexpr bool  Contains (T value)                      C_NE___ { return (value >= begin) and (value < end); }

        ND_ constexpr static Self  Max ()                           __NE___ { return Self{ MinValue<T>(), MaxValue<T>() }; }

        ND_ constexpr static Self  From (const Vec<T,2> &v)         __NE___ { return Self{ v.x, v.y }; }
        ND_ constexpr static Self  FromOffsetSize (T offset, T size)__NE___ { return Self{ offset, offset + size }; }
    };


    using RangeI    = Range<int>;
    using RangeU    = Range<uint>;
    using RangeF    = Range<float>;
    using RangeD    = Range<double>;

} // AE::Math


namespace AE::Base
{
    template <typename T>   struct TMemCopyAvailable< Range<T> >        { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T>   struct TZeroMemAvailable< Range<T> >        { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T>   struct TTriviallySerializable< Range<T> >   { static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base
