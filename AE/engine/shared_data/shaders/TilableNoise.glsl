// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Noise functions which can be tiled
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Hash.glsl"


ND_ float  TilableVoronoiNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias);		// range [0..inf]
ND_ float  TilableVoronoiNoise (const float3 pos, const float3 tileSize);									// range [0..inf]
ND_ float  TilableWarleyNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias);		// range [0..1]
ND_ float  TilableWarleyNoise (const float3 pos, const float3 tileSize);									// range [0..1]
//-----------------------------------------------------------------------------


// range [0..inf]
float  TilableVoronoiNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias)
{
	float3	ipoint	= Floor( pos * tileSize );
	float3	fpoint	= Fract( pos * tileSize );
	float	md		= float_max;

	for (int z = -1; z <= 1; ++z)
	for (int y = -1; y <= 1; ++y)
	for (int x = -1; x <= 1; ++x)
	{
		float3	ioffset	= float3(x, y, z);
		float3	offset	= DHash33( Mod( ipoint + ioffset, tileSize ) * seedScaleBias.x + seedScaleBias.y );
		float3	vec		= offset + ioffset - fpoint;
		float	d		= Dot( vec, vec );
		md = Min( md, d );
	}
	return md;
}

// range [0..inf]
float  TilableVoronoiNoise (const float3 pos, const float3 tileSize)
{
	return TilableVoronoiNoise( pos, tileSize, float2(1.0, 0.0) );
}


// range [0..1]
float  TilableWarleyNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias)
{
	return Max( 1.0 - TilableVoronoiNoise( pos, tileSize, seedScaleBias ), 0.0 );
}

// range [0..1]
float  TilableWarleyNoise (const float3 pos, const float3 tileSize)
{
	return TilableWarleyNoise( pos, tileSize, float2(1.0, 0.0) );
}
//-----------------------------------------------------------------------------
