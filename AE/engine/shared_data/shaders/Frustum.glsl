// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Frustum functions.

	TODO:
	 - add optimizations for view space culling
	 - OOBB
	 - perf tests
*/

#ifdef __cplusplus
# pragma once
#endif

#include "AABB.glsl"
#include "Cone.glsl"
#include "Sphere.glsl"
#include "Geometry.glsl"


struct Frustum
{
	// Each plane is stored as a float4 (a, b, c, d) where the plane
	// equation is 'a*x + b*y + c*z + d = 0' and the normal (a,b,c) points
	// INTO the frustum.

	// Plane indices:
	// 0 – near, 1 – far, 2 – left, 3 – right, 4 – top, 5 – bottom
	// see 'EPlane' in [Frustum.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/base/Math/Frustum.h)

	float4	planes[6];
};


// Uses spherical projection, when Frustum use rectilinear projection.
struct FrustumRays
{
	float3	leftTop;
	float3	leftBottom;
	float3	rightTop;
	float3	rightBottom;
};
//-----------------------------------------------------------------------------


ND_ Frustum		Frustum_Create (const float4 frustum[6]);
ND_ Frustum		Frustum_FromMatrix (const float4x4 mat);
ND_ Frustum		Frustum_FromMatrix (const float4x4 mat, const float2 clipPlanes);
ND_ Frustum		Frustum_FromCornerPoints (const float3 points[8]);

ND_ Frustum		Frustum_FromRays (const FrustumRays rays, const float2 clipPlanes, const float3 origin);
ND_ FrustumRays	Frustum_ToRays (const Frustum fr);

ND_ Frustum		Frustum_WithXYOffset (const Frustum fr, const float2 offset);

	void		Frustum_ToCornerPoints (const Frustum fr, out float3 points[8]);

ND_ AABB		Frustum_ToAABB (const Frustum fr);
ND_ Sphere		Frustum_ToSphere (const Frustum fr);
ND_ float2		Frustum_ExtractClipPlanes (const Frustum fr);

ND_ Frustum		Frustum_ToTile (const Frustum mainFrustum, const int2 tileIdx, const int2 tileCount);

// returns 'true' if visible (intersects).
// visibility test is conservative so may have false positive results.
ND_ bool		Frustum_IsVisible (const Frustum fr, const Sphere sp);
ND_ bool		Frustum_IsVisible (const Frustum fr, const AABB box);
ND_ bool		Frustum_IsVisible (const Frustum fr, const float3 begin, const float3 end);
ND_ bool		Frustum_IsVisible (const Frustum fr, const float3 point);
ND_ bool		Frustum_IsVisible (const Frustum fr, const Cone c);
//-----------------------------------------------------------------------------


ND_ float3		FrustumRays_Lerp (const FrustumRays origin, const float2 uv);
ND_ FrustumRays	FrustumRays_GetTile (const FrustumRays origin, const int2 tileIdx, const int2 tileCount);
//-----------------------------------------------------------------------------



/*
=================================================
	Frustum_Create
=================================================
*/
Frustum  Frustum_Create (const float4 frustum[6])
{
	Frustum	res;
	res.planes = frustum;
	return res;
}

/*
=================================================
	Frustum_FromMatrix
=================================================
*/
float4  _Frustum_FromMatrix_SetPlane (float a, float b, float c, float d)
{
	float	len		= Length( float3( a, b, c ));
	float	inv_len	= Max( Rcp( len ), 0.0 );
	return float4( a, b, c, Abs(d) ) * inv_len;
}

Frustum  Frustum_FromMatrix (const float4x4 mat)
{
	Frustum	res;
	res.planes[0] = _Frustum_FromMatrix_SetPlane( mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], -mat[3][3] - mat[3][2] );
	res.planes[1] = _Frustum_FromMatrix_SetPlane( mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], -mat[3][3] + mat[3][2] );
	res.planes[2] = _Frustum_FromMatrix_SetPlane( mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], -mat[3][3] - mat[3][0] );
	res.planes[3] = _Frustum_FromMatrix_SetPlane( mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], -mat[3][3] + mat[3][0] );
	res.planes[4] = _Frustum_FromMatrix_SetPlane( mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], -mat[3][3] + mat[3][1] );
	res.planes[5] = _Frustum_FromMatrix_SetPlane( mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], -mat[3][3] - mat[3][1] );
	// TODO: near distance may be incorrect for Vulkan matrices
	return res;
}

Frustum  Frustum_FromMatrix (const float4x4 mat, const float2 clipPlanes)
{
	Frustum	fr = Frustum_FromMatrix( mat );
	fr.planes[0].w = -clipPlanes.x;
	fr.planes[1].w = clipPlanes.y;
	return fr;
}

/*
=================================================
	Frustum_IsVisible (Sphere)
=================================================
*/
bool  Frustum_IsVisible (const Frustum fr, const Sphere sp)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float	d = Plane_Distance( fr.planes[i], sp.center ) + sp.radius;
		invisible += LessF( d, 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum_IsVisible (AABB)
=================================================
*/
bool  Frustum_TestAABB_v1 (const Frustum fr, const AABB box)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float3	v = Max( box.min * fr.planes[i].xyz, box.max * fr.planes[i].xyz );
		float	d = v.x + v.y + v.z + fr.planes[i].w;
		invisible += LessF( d, 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_TestAABB_v2 (const Frustum fr, const AABB box)
{
	float3	center	= AABB_Center( box );
	float	radius	= AABB_OuterRadius( box );
	return Frustum_IsVisible( fr, Sphere_Create( center, radius ));
}

bool  Frustum_IsVisible (const Frustum fr, const AABB box)
{
	return Frustum_TestAABB_v1( fr, box );
}

/*
=================================================
	Frustum_IsVisible (Line)
=================================================
*/
bool  Frustum_TestLine_v1 (const Frustum fr, const float3 begin, const float3 end)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		invisible += LessF( Plane_Distance( fr.planes[i], begin ), 0.0 ) *
					 LessF( Plane_Distance( fr.planes[i], end   ), 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_IsVisible (const Frustum fr, const float3 begin, const float3 end)
{
	return Frustum_TestLine_v1( fr, begin, end );
}

/*
=================================================
	Frustum_IsVisible (Point)
=================================================
*/
bool  Frustum_IsVisible (const Frustum fr, const float3 point)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		invisible += LessF( Plane_Distance( fr.planes[i], point ), 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum_IsVisible (Cone)
=================================================
*/
bool  Frustum_TestCone_v1 (const Frustum fr, const Cone c)
{
	return Frustum_IsVisible( fr, Cone_ToBoundingSphere( c ));
}

bool  Frustum_TestCone_v2 (const Frustum fr, const Cone c)
{
	const float3	base_center	= Cone_BaseCenter( c );
	const float		radius		= Cone_BaseRadius( c );

	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float	dist_apex	= Plane_Distance( fr.planes[i], c.origin );
		float	dist_base	= Plane_Distance( fr.planes[i], base_center );

		if ( dist_apex < -radius and dist_base < -radius )
			return false;
	}
	return true;
}

bool  Frustum_TestCone_v3 (const Frustum fr, const Cone c)
{
	const float3	base_center	= Cone_BaseCenter( c );
	const float		radius		= Cone_BaseRadius( c );
	const float		sin_ha		= Sin( c.halfAngle );

	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float	dist_apex	= Plane_Distance( fr.planes[i], c.origin );
		float	dist_base	= Plane_Distance( fr.planes[i], base_center );

		if ( dist_apex < -radius and dist_base < -radius )
			return false;

		float	dir_on_plane = Dot( fr.planes[i].xyz, c.dir );

		if ( dir_on_plane > sin_ha )
		{
			// Plane normal is within the cone's opening angle
			// Worst case is at the apex
			//if ( dist_apex < 0.0 )
			//    return false;		// TODO: wrong test
		}
		else
		{
			// Calculate the point on cone boundary that's closest to the plane

			// For a conservative approach, check if base sphere is completely outside
			if ( dist_apex < 0.0 )
				return false;
		}
	}
	return true;
}

bool  Frustum_IsVisible (const Frustum fr, const Cone c)
{
	return Frustum_TestCone_v3( fr, c );
}

/*
=================================================
	Frustum_ToCornerPoints
=================================================
*/
void  Frustum_ToCornerPoints (const Frustum fr, out float3 points[8])
{
	//  2 - 3        6 - 7
	//  | / |-near   | \ |-far
	//  0 - 1        4 - 5

	points[0] = Plane_IntersectionPoint( fr.planes[2], fr.planes[4], fr.planes[0] );
	points[1] = Plane_IntersectionPoint( fr.planes[3], fr.planes[4], fr.planes[0] );
	points[2] = Plane_IntersectionPoint( fr.planes[2], fr.planes[5], fr.planes[0] );
	points[3] = Plane_IntersectionPoint( fr.planes[3], fr.planes[5], fr.planes[0] );

	points[4] = Plane_IntersectionPoint( fr.planes[2], fr.planes[4], fr.planes[1] );
	points[5] = Plane_IntersectionPoint( fr.planes[3], fr.planes[4], fr.planes[1] );
	points[6] = Plane_IntersectionPoint( fr.planes[2], fr.planes[5], fr.planes[1] );
	points[7] = Plane_IntersectionPoint( fr.planes[3], fr.planes[5], fr.planes[1] );
}

/*
=================================================
	Frustum_FromCornerPoints
=================================================
*/
Frustum  Frustum_FromCornerPoints (const float3 points[8])
{
	Frustum	res;
	res.planes[0] = Plane_FromPoints( points[0], points[2], points[1] );	// near
	res.planes[1] = Plane_FromPoints( points[4], points[5], points[6] );	// far
	res.planes[2] = Plane_FromPoints( points[0], points[4], points[2] );	// left
	res.planes[3] = Plane_FromPoints( points[1], points[3], points[5] );	// right
	res.planes[4] = Plane_FromPoints( points[0], points[5], points[4] );	// top
	res.planes[5] = Plane_FromPoints( points[2], points[6], points[3] );	// bottom

	// fix distance when frustum center located at the origin of coordinate system
	res.planes[2].w = 0.0;
	res.planes[3].w = 0.0;
	res.planes[4].w = 0.0;
	res.planes[5].w = 0.0;

	return res;
}

/*
=================================================
	Frustum_ToAABB
=================================================
*/
AABB  Frustum_ToAABB (const Frustum fr)
{
	float3	corners[8];
	Frustum_ToCornerPoints( fr, OUT corners );

	AABB	res;
	res.min	=	Min( Min( Min( corners[0], corners[1] ), Min( corners[2], corners[3] )),
					 Min( Min( corners[4], corners[5] ), Min( corners[6], corners[7] )));
	res.max	=	Max( Max( Max( corners[0], corners[1] ), Max( corners[2], corners[3] )),
					 Max( Max( corners[4], corners[5] ), Max( corners[6], corners[7] )));
	return res;
}

/*
=================================================
	Frustum_ToSphere
=================================================
*/
Sphere  Frustum_ToSphere (const Frustum fr)
{
	AABB	box = Frustum_ToAABB( fr );
	return Sphere_Create( AABB_Center( box ), AABB_OuterRadius( box ));
}

/*
=================================================
	Frustum_FromRays
=================================================
*/
Frustum  Frustum_FromRays (const FrustumRays rays, const float2 clipPlanes, const float3 origin)
{
	Frustum	res;
	res.planes[2] = Plane_From2Normals( rays.leftTop,		rays.leftBottom,	origin );
	res.planes[3] = Plane_From2Normals( rays.rightBottom,	rays.rightTop,		origin );
	res.planes[4] = Plane_From2Normals( rays.rightTop,		rays.leftTop,		origin );
	res.planes[5] = Plane_From2Normals( rays.leftBottom,	rays.rightBottom,	origin );

	const float3	avr_dir = Normalize( rays.leftBottom + rays.leftTop + rays.rightBottom + rays.rightTop );
	res.planes[0] = float4(  avr_dir, -clipPlanes.x );
	res.planes[1] = float4( -avr_dir,  clipPlanes.y );
	return res;
}

/*
=================================================
	Frustum_ToRays
=================================================
*/
FrustumRays  Frustum_ToRays (const Frustum fr)
{
	FrustumRays	res;
	res.leftBottom	= Plane_IntersectionRay( fr.planes[2], fr.planes[5] );
	res.leftTop		= Plane_IntersectionRay( fr.planes[4], fr.planes[2] );
	res.rightBottom	= Plane_IntersectionRay( fr.planes[5], fr.planes[3] );
	res.rightTop	= Plane_IntersectionRay( fr.planes[3], fr.planes[4] );
	return res;
}

/*
=================================================
	Frustum_ExtractClipPlanes
=================================================
*/
float2  Frustum_ExtractClipPlanes (const Frustum fr)
{
	return float2( -fr.planes[0].w, fr.planes[1].w );
}

/*
=================================================
	Frustum_WithXYOffset
=================================================
*/
Frustum  Frustum_WithXYOffset (const Frustum fr, const float2 offset)
{
	Frustum		res;
	res.planes[0]	= fr.planes[0];
	res.planes[1]	= fr.planes[1];
	res.planes[2]	= float4( fr.planes[2].xyz, offset.x );
	res.planes[3]	= float4( fr.planes[3].xyz, offset.x );
	res.planes[4]	= float4( fr.planes[4].xyz, offset.y );
	res.planes[5]	= float4( fr.planes[5].xyz, offset.y );
	return res;
}
//-----------------------------------------------------------------------------



/*
=================================================
	FrustumRays_Lerp
=================================================
*/
float3  FrustumRays_Lerp (const FrustumRays origin, const float2 uv)
{
	float3	left	= Lerp( origin.leftBottom,  origin.leftTop,  uv.y );
	float3	right	= Lerp( origin.rightBottom, origin.rightTop, uv.y );
	float3	dir		= Lerp( left, right, uv.x );
	return	Normalize( dir );
}

/*
=================================================
	FrustumRays_GetTile
----
	only for ray tracing
=================================================
*/
FrustumRays  FrustumRays_GetTile (const FrustumRays origin, const int2 tileIdx, const int2 tileCount)
{
	float2	uv_min = float2(tileIdx) / float2(tileCount);
	float2	uv_max = float2(tileIdx+1) / float2(tileCount);

	FrustumRays	res;
	res.leftBottom	= FrustumRays_Lerp( origin, float2( uv_min.x, uv_min.y ));
	res.leftTop		= FrustumRays_Lerp( origin, float2( uv_min.x, uv_max.y ));
	res.rightBottom	= FrustumRays_Lerp( origin, float2( uv_max.x, uv_min.y ));
	res.rightTop	= FrustumRays_Lerp( origin, float2( uv_max.x, uv_max.y ));
	return res;
}
