// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Ray-shape intersection functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Ray.glsl"
#include "AABB.glsl"
#include "Sphere.glsl"
#include "Matrix.glsl"


// 3D //
ND_ bool  Sphere_Ray_Intersect_v1 (const Sphere sphere, const Ray ray, out float2 tBeginEnd);
ND_ bool  Sphere_Ray_Intersect_v2 (const Sphere sphere, const Ray ray, out float2 tBeginEnd);

ND_ bool  AABB_Ray_Intersect (const float3 boxSize, const Ray ray, out float2 tBeginEnd, out float3 outNormal);
ND_ bool  AABB_Ray_Intersect (const AABB aabb, const Ray ray, out float2 tBeginEnd);

ND_ bool  Plane_Ray_Intersect (const Ray ray, const float3 planePos, const float3 planeNorm, inout float tHit);
//-----------------------------------------------------------------------------


// 2D //
ND_ bool  Rect_Ray_Intersect (const float2 rectSize, const float2 rayDir, const float2 rayPos, out float2 tBeginEnd);

ND_ bool  Line_Line_Intersect (const float2 line0begin, const float2 line0end,
							   const float2 line1begin, const float2 line1end,
							   out float2 outPoint);
ND_ bool  Line_Line_Intersects (const float2 line0begin, const float2 line0end,
								const float2 line1begin, const float2 line1end);
//-----------------------------------------------------------------------------

#include "../3party_shaders/Intersectors-1.glsl"
#include "../3party_shaders/Intersectors-2.glsl"


/*
=================================================
	Line_Line_Intersect
----
	from https://gist.github.com/TimSC/47203a0f5f15293d2099507ba5da44e6
=================================================
*/
bool  Line_Line_Intersect (const float2 line0begin, const float2 line0end,
						   const float2 line1begin, const float2 line1end,
						   out float2 outPoint)
{
	float	detl1	= MatDeterminant( float2x2( line0begin.x, line0begin.y, line0end.x, line0end.y ));
	float	detl2	= MatDeterminant( float2x2( line1begin.x, line1begin.y, line1end.x, line1end.y ));
	float	x1mx2	= line0begin.x - line0end.x;
	float	x3mx4	= line1begin.x - line1end.x;
	float	y1my2	= line0begin.y - line0end.y;
	float	y3my4	= line1begin.y - line1end.y;

	float	denom	= MatDeterminant( float2x2( x1mx2, y1my2, x3mx4, y3my4 ));
	outPoint.x		= MatDeterminant( float2x2( detl1, x1mx2, detl2, x3mx4 ));
	outPoint.y		= MatDeterminant( float2x2( detl1, y1my2, detl2, y3my4 ));
	outPoint		/= denom;

	float2	min = Min( Min( line0begin, line0end ), Min( line1begin, line1end ));
	float2	max = Max( Max( line0begin, line0end ), Max( line1begin, line1end ));

	return	IsNotZero( denom )					and
			AllGreaterEqual( outPoint, min )	and
			AllLessEqual( outPoint, max );
}

bool  Line_Line_Intersects (const float2 line0begin, const float2 line0end,
							const float2 line1begin, const float2 line1end)
{
	float2	point;
	return Line_Line_Intersect( line0begin, line0end, line1begin, line1end, point );
}


/*
=================================================
	Plane_Ray_Intersect
=================================================
*/
bool  Plane_Ray_Intersect (const Ray ray, const float3 planePos, const float3 planeNorm, inout float tHit)
{
	float	d = -Dot( planePos, planeNorm );
	float	v = Dot( ray.dir, planeNorm );
	float	t = -(Dot( ray.pos, planeNorm ) + d) / v;

	tHit = t;
	return t > 0.0;
}
