// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Frustum functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ bool  Frustum_TestSphere (const float4 frustum[6], const float3 center, const float radius);
ND_ bool  Frustum_TestAABB (const float4 frustum[6], const float3 minBound, const float3 maxBound);
ND_ bool  Frustum_TestLine (const float4 frustum[6], const float3 begin, const float3 end);
ND_ bool  Frustum_TestPoint (const float4 frustum[6], const float3 point);
//-----------------------------------------------------------------------------


/*
=================================================
	Frustum_TestSphere
=================================================
*/
bool  Frustum_TestSphere (const float4 frustum[6], const float3 center, const float radius)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float	d = Dot( center, frustum[i].xyz ) + frustum[i].w + radius;
		invisible += LessFp( d, 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum_TestAABB
=================================================
*/
bool  Frustum_TestAABB_v1 (const float4 frustum[6], const float3 minBound, const float3 maxBound)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float3	v = Max( minBound * frustum[i].xyz, maxBound * frustum[i].xyz );
		float	d = v.x + v.y + v.z + frustum[i].w;
		invisible += LessFp( d, 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_TestAABB_v2 (const float4 frustum[6], const float3 minBound, const float3 maxBound)
{
	float3	center	= (minBound + maxBound) * 0.5;
	float	radius	= MaxOf( Abs( maxBound - minBound ));
	return Frustum_TestSphere( frustum, center, radius );
}

bool  Frustum_TestAABB (const float4 frustum[6], const float3 minBound, const float3 maxBound)
{
	return Frustum_TestAABB_v1( frustum, minBound, maxBound );
}

/*
=================================================
	Frustum_TestLine
=================================================
*/
bool  Frustum_TestLine_v1 (const float4 frustum[6], const float3 begin, const float3 end)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		invisible += LessFp( Dot( frustum[i].xyz, begin ) + frustum[i].w, 0.0 ) *
					 LessFp( Dot( frustum[i].xyz, end   ) + frustum[i].w, 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_TestLine (const float4 frustum[6], const float3 begin, const float3 end)
{
	return Frustum_TestLine_v1( frustum, begin, end );
}

/*
=================================================
	Frustum_TestPoint
=================================================
*/
bool  Frustum_TestPoint (const float4 frustum[6], const float3 point)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		invisible += LessFp( Dot( frustum[i].xyz, point ) + frustum[i].w, 0.0 );
	}
	return invisible < 0.f;
}
