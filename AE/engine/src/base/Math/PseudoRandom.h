// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Color.h"

namespace AE::Math::PseudoRandom
{
namespace _hidden_
{

    ND_ inline uint3  IWeylConst ()
    {
        return uint3{
            0x3504f333u,    // W0 = 3*2309*128413 
            0xf1bbcdcbu,    // W1 = 7*349*1660097 
            741103597u      // M  = 13*83*686843
        };
    }

} // _hidden_


/*
=================================================
    HEHash
=================================================
*/
    ND_ inline float  HEHash (uint n) __NE___
    {
        // from https://www.shadertoy.com/view/llGSzw
        // The MIT License
        // Copyright (c) 2017 Inigo Quilez

        // integer hash copied from Hugo Elias
        n = (n << 13U) ^ n;
        n = n * (n * n * 15731U + 789221U) + 1376312589U;

        // floating point conversion from http://iquilezles.org/www/articles/sfrand/sfrand.htm
        return BitCast<float>( (n>>9U) | 0x3f800000U ) - 1.0f;
    }

/*
=================================================
    Integer Weylend hash
=================================================
*/
    ND_ inline uint  IWeylHash (const uint2 &p) __NE___
    {
        // from https://www.shadertoy.com/view/4dlcR4
        // LICENSE: http://unlicense.org/
        using namespace AE::Math::PseudoRandom::_hidden_;

        uint    x = p.x;
        uint    y = p.y;

        x *= IWeylConst().x;    // x' = Fx(x)
        y *= IWeylConst().y;    // y' = Fy(y)
        x ^= y;                 // combine
        x *= IWeylConst().z;    // MLCG constant
        return x;
    }

    ND_ inline uint  IWeylHash2 (const uint2 &p) __NE___
    {
        // from https://www.shadertoy.com/view/4dlcR4
        // LICENSE: http://unlicense.org/
        using namespace AE::Math::PseudoRandom::_hidden_;

        uint    x = p.x;
        uint    y = p.y;

        x *= IWeylConst().x;    // x' = Fx(x)
        y *= IWeylConst().y;    // y' = Fy(y)
        x += IWeylConst().y;    // some odd constant
        y += IWeylConst().x;    // some odd constant
        x ^= y;                 // combine
        x *= IWeylConst().z;    // MLCG constant
        return x;
    }

/*
=================================================
    Rainbow / RainbowWrap
=================================================
*/
    ND_ inline RGBA32f  Rainbow (const float factor) __NE___
    {
        return RGBA32f{ HSVColor{ Saturate( factor * 0.74f )}};
    }

    ND_ inline RGBA32f  RainbowWrap (const float factor) __NE___
    {
        return RGBA32f{ HSVColor{ Wrap( factor * 0.74f, 0.0f, 1.0f )}};
    }

/*
=================================================
    StringToColor
=================================================
*/
    template <typename T>
    void  StringToColor (OUT RGBA32f &col, BasicStringView<T> str) __NE___
    {
        HashVal u = HashOf( str );
        //float h = (float(usize(u) & 0xFF) / float(0xFF));
        float   h = float(u.Cast<ushort>() & 0xFF) / float(0xFF);
        //float h = HEHash( u.Cast<uint>() );
        col = Rainbow( h );
    }

    template <typename T>
    void  StringToColor (OUT RGBA8u &col, BasicStringView<T> str) __NE___
    {
        RGBA32f rgba;
        StringToColor( OUT rgba, str );
        col = RGBA8u{ rgba };
    }

/*
=================================================
    IntToColor
=================================================
*/
    template <typename T>
    void  IntToColor (OUT RGBA32f &col, const T value) __NE___
    {
        STATIC_ASSERT( IsInteger<T> );

        HashVal u = HashOf( value );
        float   h = float(u.Cast<ushort>() & 0xFF) / float(0xFF);
        col = RGBA32f{ HSVColor{ h * 0.74f }};
    }

    template <typename T>
    void  IntToColor (OUT RGBA8u &col, const T value) __NE___
    {
        RGBA32f tmp;
        IntToColor( OUT tmp, value );
        col = RGBA8u{ tmp };
    }


} // AE::Math::PseudoRandom
