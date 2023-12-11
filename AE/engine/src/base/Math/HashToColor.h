// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Color.h"

namespace AE::Math
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
        using namespace AE::Math::_hidden_;

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
        using namespace AE::Math::_hidden_;

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
    StringToColor
=================================================
*/
    inline void  HashToColor (OUT RGBA32f &col, HashVal u) __NE___
    {
        float   h = float(u.Cast<ushort>() & 0xFF) / float(0xFF);
        col = Rainbow( h );
    }

    inline void  HashToColor (OUT RGBA8u &col, HashVal u) __NE___
    {
        RGBA32f c;
        HashToColor( OUT c, u );
        col = RGBA8u{ c };
    }

/*
=================================================
    StringToColor
=================================================
*/
    template <typename C, typename T>
    void  StringToColor (OUT C &col, BasicStringView<T> str) __NE___
    {
        HashVal u = HashOf( str );
        HashToColor( OUT col, u );
    }

/*
=================================================
    IntToColor
=================================================
*/
    template <typename C, typename T>
    void  IntToColor (OUT C &col, const T value) __NE___
    {
        StaticAssert( IsInteger<T> );

        HashVal u = HashOf( value );
        HashToColor( OUT col, u );
    }


} // AE::Math
