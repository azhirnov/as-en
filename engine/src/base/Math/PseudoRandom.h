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
			0x3504f333u,	// W0 = 3*2309*128413 
			0xf1bbcdcbu,	// W1 = 7*349*1660097 
			741103597u		// M  = 13*83*686843
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
	
		uint	x = p.x;
		uint	y = p.y;

		x *= _hidden_::IWeylConst().x;	// x' = Fx(x)
		y *= _hidden_::IWeylConst().y;	// y' = Fy(y)
		x ^= y;							// combine
		x *= _hidden_::IWeylConst().z;	// MLCG constant
		return x;
	}

	ND_ inline uint  IWeylHash2 (const uint2 &p) __NE___
	{
		// from https://www.shadertoy.com/view/4dlcR4
		// LICENSE: http://unlicense.org/
	
		uint	x = p.x;
		uint	y = p.y;
	
		x *= _hidden_::IWeylConst().x;	// x' = Fx(x)
		y *= _hidden_::IWeylConst().y;	// y' = Fy(y)
		x += _hidden_::IWeylConst().y;	// some odd constant
		y += _hidden_::IWeylConst().x;	// some odd constant
		x ^= y;							// combine
		x *= _hidden_::IWeylConst().z;	// MLCG constant
		return x;
	}
	
/*
=================================================
	StringToColor
=================================================
*/
	template <typename T>
	inline void  StringToColor (OUT HSVColor &col, BasicStringView<T> str) __NE___
	{
		HashVal	u = HashOf( str );
		//float	h = (float(usize(u) & 0xFF) / float(0xFF));
		float	h = float(u.Cast<ushort>() & 0xFF) / float(0xFF);
		//float	h = HEHash( u.Cast<uint>() );
		col = HSVColor{ h * 0.74f };
	}
	
	template <typename T>
	inline void  StringToColor (OUT RGBA32f &col, BasicStringView<T> str) __NE___
	{
		HSVColor	hsv;
		StringToColor( OUT hsv, str );
		col = RGBA32f{ hsv };
	}
	
	template <typename T>
	inline void  StringToColor (OUT RGBA8u &col, BasicStringView<T> str) __NE___
	{
		RGBA32f	rgba;
		StringToColor( OUT rgba, str );
		col = RGBA8u{ rgba };
	}


} // AE::Math::PseudoRandom
