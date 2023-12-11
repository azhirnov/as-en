// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

    //
    // Image Color Swizzle
    //

    struct ImageSwizzle
    {
    // variables
    private:
        ushort  _value  = 0x1234;   // 0 - unknown, 1 - R, 2 - G, 3 - B, 4 - A, 5 - O, 6 - 1, example: RGB0 - 0x1235

        static constexpr uint   _Mask = 0x7;


    // methods
    public:
        constexpr ImageSwizzle ()                                                               __NE___ {}
        explicit ImageSwizzle (const uint4 &comp)                                               __NE___;

        ND_ constexpr uint  Get ()                                                              C_NE___ { return _value; }
        ND_ constexpr bool  IsDefault ()                                                        C_NE___ { return _value == ImageSwizzle().Get(); }

        ND_ uint4           ToVec ()                                                            C_NE___;

        ND_ constexpr bool  operator == (const ImageSwizzle &rhs)                               C_NE___ { return _value == rhs._value; }
        ND_ constexpr bool  operator >  (const ImageSwizzle &rhs)                               C_NE___ { return _value >  rhs._value; }

        ND_ static constexpr ImageSwizzle  FromString (const char* str, const usize len)        __NE___;
        ND_ static constexpr ImageSwizzle  FromString (StringView str)                          __NE___ { return FromString( str.data(), str.size() ); }

        // 0..3 - src0
        // 4..7 - src1
        ND_ static uint4  Mix (ImageSwizzle src0, ImageSwizzle src1)                            __NE___;

        friend constexpr ImageSwizzle  operator "" _swizzle (const char* str, const usize len)  __NE___;


    private:
        static constexpr uint  _CharToValue (char c)                                            __NE___;
    };


/*
=================================================
    operator ""
=================================================
*/
    ND_ constexpr ImageSwizzle  operator "" _swizzle (const char* str, const usize len) __NE___
    {
        return ImageSwizzle::FromString( str, len );
    }

/*
=================================================
    constructor
=================================================
*/
    inline ImageSwizzle::ImageSwizzle (const uint4 &comp) __NE___ : _value{0}
    {
        ASSERT(All( comp < 7u ));

        _value |= (comp.x & _Mask) << 12;
        _value |= (comp.y & _Mask) << 8;
        _value |= (comp.z & _Mask) << 4;
        _value |= (comp.w & _Mask);
    }

/*
=================================================
    _CharToValue
=================================================
*/
    inline constexpr uint  ImageSwizzle::_CharToValue (char c) __NE___
    {
        return  c == 'r' or c == 'R'    ? 1 :
                c == 'g' or c == 'G'    ? 2 :
                c == 'b' or c == 'B'    ? 3 :
                c == 'a' or c == 'A'    ? 4 :
                c == '0'                ? 5 :
                c == '1'                ? 6 : 0;
    }

/*
=================================================
    ToVec
=================================================
*/
    inline uint4  ImageSwizzle::ToVec () C_NE___
    {
        return uint4( (_value >> 12) & _Mask, (_value >> 8) & _Mask, (_value >> 4) & _Mask, _value & _Mask );
    }

/*
=================================================
    FromString
=================================================
*/
    inline constexpr ImageSwizzle  ImageSwizzle::FromString (const char* str, const usize len) __NE___
    {
        ASSERT( len <= 4 );

        ImageSwizzle    res;
        uint            sw = 0;

        for (usize i = 0; (i < len) and (i < 4); ++i)
        {
            const char  c = str[i];
            const uint  v = _CharToValue( c );

            ASSERT( v != 0 );   // 'c' must be R, G, B, A, 0, 1
            sw |= (v << (3 - i) * 4);
        }
        res._value = CheckCast<ushort>(sw);
        return res;
    }

} // AE::Graphics


namespace AE::Base
{
    template <> struct TMemCopyAvailable< AE::Graphics::ImageSwizzle >      { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< AE::Graphics::ImageSwizzle > { static constexpr bool  value = true; };

} // AE::Base


template <>
struct std::hash< AE::Graphics::ImageSwizzle >
{
    ND_ size_t  operator () (const AE::Graphics::ImageSwizzle &value) C_NE___
    {
        return size_t( value.Get() );
    }
};
