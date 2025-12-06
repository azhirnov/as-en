// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	2D Geometry types and functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "SafeMath.glsl"


struct Line2d
{
	float2	begin;
	float2	end;
};

struct Line2dI
{
	int2	begin;
	int2	end;
};

struct Rect
{
	float4	v;		// left, top, right, bottom
};

struct RectI
{
	int4	v;		// left, top, right, bottom
};

struct Circle
{
	float2	center;
	float	radius;
};

struct Triangle2d
{
	float2	a, b, c;
};

struct Frustum2d
{
	float3	planes [4];		// planes: 0 - left, 1 - right, 2 - top, 3 - bottom;  xy - normal, z - distance
};
//-----------------------------------------------------------------------------



ND_ int2		LeftVector  (const int2   v)													{ return int2  ( -v.y,  v.x ); }
ND_ float2		LeftVector  (const float2 v)													{ return float2( -v.y,  v.x ); }

ND_ int2		RightVector (const int2   v)													{ return int2  (  v.y, -v.x ); }
ND_ float2		RightVector (const float2 v)													{ return float2(  v.y, -v.x ); }
//-----------------------------------------------------------------------------


// same for 'Line2dI'

ND_ Line2d		Line_Create (const float2 begin, const float2 end);

ND_ Rect		Line_ToRect (const Line2d line);

ND_ float3		Line_GetEquation (const Line2d line);

ND_ bool		Line_RayIntersection (const Line2d line0, const Line2d line1, out float2 intersection);

ND_ bool		Line_Perpendicular (const Line2d line, const float2 point, out float2 pointOnLine);
ND_ bool		Line_PointInside (const Line2d line, const float2 projectedPoint);
ND_ bool		Line_PointOnLine (const Line2d line, const float2 point);
ND_ float2		Line_ProjectPoint (const Line2d line, const float2 point);

ND_ bool		Line_PointOnLeftSide (const Line2d line, const float2 point);
ND_ bool		Line_PointOnRightSide (const Line2d line, const float2 point);

ND_ bool		Quadrilateral_PointInside (float2 v0, float2 v1, float2 v2, float2 v3, float2 point);

ND_ bool		Line_IsVertical (const Line2d line);
ND_ bool		Line_IsHorizontal (const Line2d line);
//-----------------------------------------------------------------------------


ND_ float		Plane2d_Distance (const float3 planeNormalAndDist, const float2 point);
ND_ float2		Plane2d_IntersectionPoint (const float3 p0, const float3 p1);
ND_ float3		Plane2d_FromPoints (const float2 p0, const float2 p1);
//-----------------------------------------------------------------------------


// same for 'RectI'

ND_ Rect		Rect_Create (float left, float top, float right, float bottom);
ND_ Rect		Rect_Create (const float2 leftTop, const float2 rightBottom);
ND_ Rect		Rect_FromRange (const float2 x, const float2 y);

ND_ Rect		Rect_FromCenterHalfSize (const float2 center, const float2 hsize);
ND_ Rect		Rect_FromCenterSize (const float2 center, const float2 size);

ND_ float		Rect_Left (const Rect rect);
ND_ float		Rect_Right (const Rect rect);
ND_ float		Rect_Top (const Rect rect);
ND_ float		Rect_Bottom (const Rect rect);

ND_ float2		Rect_LeftTop (const Rect rect);
ND_ float2		Rect_RightBottom (const Rect rect);

ND_ bool		Rect_IsEmpty (const Rect rect);
ND_ bool		Rect_IsInvalid (const Rect rect);

ND_ float2		Rect_Center (const Rect rect);
ND_ float2		Rect_Size (const Rect rect);
ND_ float2		Rect_HalfSize (const Rect rect);

ND_ float2		Rect_Point (const Rect rect, const uint cornerId);
ND_ Line2d		Rect_Edge (const Rect rect, const uint edge);
ND_ uint2		Rect_EdgeCorners (const uint edge);

ND_ bool		Rect_IsInside (const Rect rect, const float2 point);
ND_ bool		Rect_IsInside (const float2 halfSize, const float2 point);
ND_ bool		Rect_IsOutside (const Rect rect, const float2 point);

ND_ uint		Rect_DirToEdge (const float2 dir);

ND_ Rect		Rect_Join (const Rect rect, const float2 point);
ND_ Rect		Rect_Join (const Rect lhs, const Rect rhs);

ND_ bool		Rect_Intersects (const Rect lhs, const Rect rhs);
ND_ bool		Rect_Intersects (const Rect rect, const Line2d line);

ND_ Rect		Rect_Intersection (const Rect lhs, const Rect rhs);
ND_ float2		Rect_Intersection (const float2 rayDir);
ND_ float2		Rect_Intersection (const Rect rect, const float2 rayDir, const float2 rayOrigin);
//-----------------------------------------------------------------------------


ND_ Circle		Circle_Create (const float2 center, const float radius);
ND_ bool		Circle_IsInside (const Circle c, const float2 point)							{ return DistanceSq( point, c.center ) < Square( c.radius ); }
//-----------------------------------------------------------------------------


ND_ Triangle2d	Triangle_Create (const float2 a, const float2 b, const float2 c);

ND_ float		Triangle_Area (const Triangle2d t);

ND_ bool		Triangle_IsFrontFace (const Triangle2d t)										{ return Cross2( t.b - t.a, t.c - t.a ) <= 0.0; }
ND_ bool		Triangle_IsBackFace (const Triangle2d t)										{ return ! Triangle_IsFrontFace( t ); }

ND_ float		Triangle_Perimeter (const Triangle2d t)											{ return Distance( t.a, t.b ) + Distance( t.b, t.c ) + Distance( t.c, t.a ); }

ND_ Circle		Triangle_InnerCenterAndRadius (const Triangle2d t);
//-----------------------------------------------------------------------------


ND_ Frustum2d	Frustum2d_Create (const float3 frustum[4]);

ND_ Frustum2d	Frustum2d_FromCornerPoints (const float2 points[4]);
ND_ Frustum2d	Frustum2d_FromCornerPoints (float2 p0, float2 p1, float2 p2, float2 p3);

ND_ Rect		Frustum2d_ToRect (const Frustum2d fr);

	void		Frustum2d_ToCornerPoints (const Frustum2d fr, out float2 points[4]);

// returns 'true' if visible (intersects).
// visibility test is conservative so may have false positive results.
ND_ bool		Frustum2d_IsVisible (const Frustum2d fr, const float2 point);
ND_ bool		Frustum2d_IsVisible (const Frustum2d fr, const Line2d line);
ND_ bool		Frustum2d_IsVisible (const Frustum2d fr, const Rect rect);
ND_ bool		Frustum2d_IsVisible (const Frustum2d fr, const Circle circle);
ND_ bool		Frustum2d_IsVisible (const Frustum2d fr, const Triangle2d tri);
//-----------------------------------------------------------------------------



/*
=================================================
	Line_Create
=================================================
*/
#define Gen_LINECREATE( _line_, _vec2_ )						\
	_line_  Line_Create (const _vec2_ begin, const _vec2_ end)	\
	{															\
		_line_	res;											\
		res.begin	= begin;									\
		res.end		= end;										\
		return res;												\
	}

Gen_LINECREATE( Line2d,  float2 )
Gen_LINECREATE( Line2dI, int2   )
#undef Gen_LINECREATE

/*
=================================================
	2D Line
=================================================
*/
float3  Line_GetEquation (const Line2d line)
{
	// Ax + By + C = 0

	float3	abc;
	abc.x = line.begin.y - line.end.y;
	abc.y = line.end.x - line.begin.x;
	abc.z = line.begin.x * line.end.y - line.end.x * line.begin.y;
	return abc;
}

bool  Line_RayIntersection (const Line2d line0, const Line2d line1, out float2 intersection)
{
	float2	v0	= line0.begin - line0.end;
	float2	v1	= line1.begin - line1.end;
	float	c	= v0.x * v1.y - v0.y * v1.x;

	if ( IsZero( c ))
	{
		intersection = float2(float_max);
		return false;
	}

	float	a = line0.begin.x * line0.end.y - line0.begin.y * line0.end.x;
	float	b = line1.begin.x * line1.end.y - line1.begin.y * line1.end.x;
			c = Rcp( c );

	intersection.x = (a * v1.x - b * v0.x) * c;
	intersection.y = (a * v1.y - b * v0.y) * c;
	return true;
}

float2  Line_ProjectPoint (const Line2d line, const float2 point)
{
	float2	lvec	= line.end - line.begin;
	float2	pvec	= point - line.begin;

	float	pdl		= Dot( pvec, lvec );
	float	len_sq	= LengthSq( lvec );
	float	proj	= pdl / len_sq;

	return	line.begin + lvec * proj;
}

bool  Line_Perpendicular (const Line2d line, const float2 point, out float2 pointOnLine)
{
	pointOnLine = Line_ProjectPoint( line, point );
	return Line_PointInside( line, pointOnLine );
}

bool  Line_PointInside (const Line2d line, const float2 projectedPoint)
{
	float2	min = Min( line.begin, line.end );
	float2	max = Max( line.begin, line.end );
	return	AllGreater( projectedPoint, min ) and AllLess( projectedPoint, max );
}

bool  Line_PointOnLine (const Line2d line, const float2 point)
{
	float2	proj = Line_ProjectPoint( line, point );
	return	Line_PointInside( line, proj ) and
			DistanceSq( proj, point ) < 1.0e-4;
}

bool  Line_PointOnLeftSide (const Line2d line, const float2 point)
{
	float2	vec  = LeftVector( line.end - line.begin );
	float	sign = Dot( vec, point - line.begin );
	return	sign > 0.0;
}

bool  Line_PointOnRightSide (const Line2d line, const float2 point)
{
	float2	vec  = RightVector( line.end - line.begin );
	float	sign = Dot( vec, point - line.begin );
	return	sign > 0.0;
}

/*
=================================================
	Quadrilateral_PointInside
----
	points must be in clockwise order
=================================================
*/
bool Quadrilateral_PointInside (float2 v0, float2 v1, float2 v2, float2 v3, float2 point)
{
	return	Line_PointOnRightSide( Line_Create( v0, v1 ), point ) and
			Line_PointOnRightSide( Line_Create( v1, v2 ), point ) and
			Line_PointOnRightSide( Line_Create( v2, v3 ), point ) and
			Line_PointOnRightSide( Line_Create( v3, v0 ), point );
}

/*
=================================================
	Line_ToRect
=================================================
*/
#define Gen_LINETORECT( _line_, _rect_ )		\
	_rect_  Line_ToRect (const _line_ line)		\
	{											\
		_rect_	res;							\
		res.v.xy = Min( line.begin, line.end );	\
		res.v.zw = Max( line.begin, line.end );	\
		return res;								\
	}

Gen_LINETORECT( Line2d,  Rect  )
Gen_LINETORECT( Line2dI, RectI )
#undef Gen_LINETORECT

/*
=================================================
	Line_ToRect
=================================================
*/
bool  Line_IsVertical (const Line2d line)		{ return IsZero( line.begin.x - line.end.x ); }
bool  Line_IsHorizontal (const Line2d line)		{ return IsZero( line.begin.y - line.end.y ); }

bool  Line_IsVertical (const Line2dI line)		{ return line.begin.x == line.end.x; }
bool  Line_IsHorizontal (const Line2dI line)	{ return line.begin.y == line.end.y; }
//-----------------------------------------------------------------------------



/*
=================================================
	Plane2d_Distance
=================================================
*/
float  Plane2d_Distance (const float3 planeNormalAndDist, const float2 point)
{
	return Dot( planeNormalAndDist.xy, point ) + planeNormalAndDist.z;
}

/*
=================================================
	Plane2d_IntersectionPoint
=================================================
*/
float2  Plane2d_IntersectionPoint (const float3 p0, const float3 p1)
{
	float2	n0	= p0.xy;
	float2	n1	= p1.xy;
	float	det = Cross2( n0, n1 );
	float	inv	= SafeRcp( det );	// zero on NaN

	float	d0	= -p0.z;
	float	d1	= -p1.z;

	float	px	= d0 * n1.y - n0.y * d1;
	float	py	= n0.x * d1 - d0 * n1.x;

	return float2(px,py) * inv;
}

/*
=================================================
	Plane2d_FromPoints
=================================================
*/
float3  Plane2d_FromPoints (const float2 p0, const float2 p1)
{
	float2	n	= p1 - p0;
	float	len	= LengthSq( n );
			n	*= Max( InvSqrt( len ), 0.0 );	// zero on NaN

	float2	pn	= LeftVector( n );
	float	d	= Dot( pn, p0 );
	return	float3( pn, -d );
}
//-----------------------------------------------------------------------------



/*
=================================================
	Rect_Create
=================================================
*/
#define Gen_RECTCREATE( _rect_, _vec2_, _vec4_, _scalar_ )								\
	_rect_  Rect_Create (const _vec2_ leftTop, const _vec2_ rightBottom)				\
	{																					\
		_rect_	res;																	\
		res.v = _vec4_( leftTop, rightBottom );											\
		return res;																		\
	}																					\
																						\
	_rect_  Rect_Create (_scalar_ left, _scalar_ top, _scalar_ right, _scalar_ bottom)	\
	{																					\
		_rect_	res;																	\
		res.v = _vec4_( left, top, right, bottom );										\
		return res;																		\
	}																					\
																						\
	_rect_  Rect_FromRange (const _vec2_ x, const _vec2_ y)								\
	{																					\
		_rect_	res;																	\
		res.v = _vec4_( x[0], y[0], x[1], y[1] );										\
		return res;																		\
	}																					\
																						\
	_rect_  Rect_FromCenterHalfSize (const _vec2_ center, const _vec2_ hsize)			\
	{																					\
		_rect_	res;																	\
		res.v = _vec4_( center - hsize, center + hsize );								\
		return res;																		\
	}																					\
																						\
	_rect_  Rect_FromCenterSize (const _vec2_ center, const _vec2_ size)				\
	{																					\
		return Rect_FromCenterHalfSize( center, size / _scalar_(2) );					\
	}

Gen_RECTCREATE( Rect,  float2, float4, float )
Gen_RECTCREATE( RectI, int2,   int4,   int )
#undef Gen_RECTCREATE

/*
=================================================
	Rect_Point
=================================================
*/
#define Gen_RECTPOINT( _rect_, _vec2_ )							\
	_vec2_  Rect_Point (const _rect_ rect, const uint cornerId)	\
	{															\
		switch ( cornerId ) {									\
			case 0:	return rect.v.xy;	/* left top		*/		\
			case 1:	return rect.v.xw;	/* left bottom	*/		\
			case 2:	return rect.v.zy;	/* right top	*/		\
			case 3:	return rect.v.zw;	/* right bottom	*/		\
		}														\
	}

Gen_RECTPOINT( Rect,  float2 )
Gen_RECTPOINT( RectI, int2 )
#undef Gen_RECTPOINT

/*
=================================================
	Rect_Edge
=================================================
*/
#define Gen_RECTEDGE( _rect_, _line_ )																				\
	_line_  Rect_Edge (const _rect_ rect, const uint edge)															\
	{																												\
		switch ( edge ) {																							\
			case 0:	return Line_Create( rect.v.xy, rect.v.xw );		/* left edge	{left-top,    left-bottom}	*/	\
			case 1:	return Line_Create( rect.v.xw, rect.v.zw );		/* bottom edge	{left-bottom, right-bottom}	*/	\
			case 2:	return Line_Create( rect.v.zy, rect.v.zw );		/* right edge	{right-top,   right-bottom}	*/	\
			case 3:	return Line_Create( rect.v.xy, rect.v.zy );		/* top edge		{left-top,    right-top}	*/	\
		}																											\
	}

Gen_RECTEDGE( Rect,  Line2d  )
Gen_RECTEDGE( RectI, Line2dI )
#undef Gen_RECTEDGE

/*
=================================================
	Rect_EdgeCorners
=================================================
*/
uint2  Rect_EdgeCorners (const uint edge)
{
	switch ( edge ) {
		case 0 :	return uint2( 0, 1 );	// left edge	{left-top,    left-bottom}
		case 1 :	return uint2( 1, 3 );	// bottom edge	{left-bottom, right-bottom}
		case 2 :	return uint2( 3, 2 );	// right edge	{right-top,   right-bottom}
		case 3 :	return uint2( 2, 0 );	// top edge		{left-top,    right-top}
	}
}

/*
=================================================
	Rect_DirToEdge
=================================================
*/
uint  Rect_DirToEdge (const float2 dir)
{
	float2 a = Abs( dir );
	return a.x >= a.y ?
			uint(dir.x > 0.0) * 2 :
			uint(dir.y < 0.0) * 2 + 1;
}

/*
=================================================
	Rect_Intersection
----
	simple intersection test when ray origin in center of rect
=================================================
*/
float2  Rect_Intersection (const float2 rayDir)
{
	float2	inv	= Rcp( rayDir );
	float	t	= MinAbsOf( inv );
	return rayDir * t;
}

/*
=================================================
	Rect_Intersection
----
	use 'Rect_IsInvalid()' to check for error
=================================================
*/
#define Gen_RECTINTERSECTION( _rect_ )								\
	_rect_  Rect_Intersection (const _rect_ lhs, const _rect_ rhs)	\
	{																\
		_rect_	res;												\
		res.v.xy = Max( lhs.v.xy, rhs.v.xy );						\
		res.v.zw = Min( lhs.v.zw, rhs.v.zw );						\
		return res;													\
	}

Gen_RECTINTERSECTION( Rect  )
Gen_RECTINTERSECTION( RectI )
#undef Gen_RECTINTERSECTION

/*
=================================================
	Rect_Join (Point)
=================================================
*/
#define Gen_RECTJOINPOINT( _rect_, _vec2_ )						\
	_rect_  Rect_Join (const _rect_ rect, const _vec2_ point)	\
	{															\
		_rect_	res;											\
		res.v.xy = Min( rect.v.xy, point );						\
		res.v.zw = Max( rect.v.zw, point );						\
		return res;												\
	}

Gen_RECTJOINPOINT( Rect,  float2 )
Gen_RECTJOINPOINT( RectI, int2   )
#undef Gen_RECTJOINPOINT

/*
=================================================
	Rect_Join (Rect)
=================================================
*/
#define Gen_RECTJOINRECT( _rect_ )							\
	_rect_  Rect_Join (const _rect_ lhs, const _rect_ rhs)	\
	{														\
		_rect_	res;										\
		res.v.xy = Min( lhs.v.xy, rhs.v.xy );				\
		res.v.zw = Max( lhs.v.zw, rhs.v.zw );				\
		return res;											\
	}

Gen_RECTJOINRECT( Rect  )
Gen_RECTJOINRECT( RectI )
#undef Gen_RECTJOINRECT

/*
=================================================
	Rect_Intersects (Rect)
=================================================
*/
#define Gen_RECTINTERSECTS( _rect_ )														\
	bool  Rect_Intersects (const _rect_ lhs, const _rect_ rhs)								\
	{																						\
		return	All(bool4( Less( lhs.v.xy, rhs.v.zw ), Greater( lhs.v.zw, rhs.v.xy ) ));	\
	}

Gen_RECTINTERSECTS( Rect  )
Gen_RECTINTERSECTS( RectI )
#undef Gen_RECTINTERSECTS

/*
=================================================
	Rect_Intersects (Line)
=================================================
*/
#define Gen_RECTINTERSECTSLINE( _rect_, _line_ )					\
	bool  Rect_Intersects (const _rect_ rect, const _line_ line)	\
	{																\
		return Rect_Intersects( rect, Line_ToRect( line ));			\
	}

Gen_RECTINTERSECTSLINE( Rect,  Line2d  )
Gen_RECTINTERSECTSLINE( RectI, Line2dI )
#undef Gen_RECTINTERSECTSLINE

/*
=================================================
	Rect_IsInside
	Rect_IsOutside
=================================================
*/
#define Gen_RECTISINSIDE( _rect_, _vec2_ )																													\
	bool  Rect_IsInside (const _rect_ rect, const _vec2_ point)			{ return All(bool4( GreaterEqual( point, rect.v.xy ), Less( point, rect.v.zw ))); }	\
	bool  Rect_IsInside (const _vec2_ halfSize, const _vec2_ point)		{ return AllLess( Abs(point), halfSize ); }											\
	bool  Rect_IsOutside (const _rect_ rect, const _vec2_ point)		{ return Any(bool4( Less( point, rect.v.xy ), Greater( point, rect.v.zw ))); }

Gen_RECTISINSIDE( Rect,  float2 )
Gen_RECTISINSIDE( RectI, int2   )
#undef Gen_RECTISINSIDE

/*
=================================================
	Rect_Left / Rect_Right
	Rect_Top / Rect_Bottom
	Rect_LeftTop / Rect_RightBottom
=================================================
*/
#define Gen_RECTPOINT( _rect_, _vec2_, _scalar_ )								\
	_scalar_	Rect_Left (const _rect_ rect)			{ return rect.v.x; }	\
	_scalar_	Rect_Right (const _rect_ rect)			{ return rect.v.z; }	\
	_scalar_	Rect_Top (const _rect_ rect)			{ return rect.v.y; }	\
	_scalar_	Rect_Bottom (const _rect_ rect)			{ return rect.v.w; }	\
																				\
	_vec2_		Rect_LeftTop (const _rect_ rect)		{ return rect.v.xy; }	\
	_vec2_		Rect_RightBottom (const _rect_ rect)	{ return rect.v.zw; }

Gen_RECTPOINT( Rect,  float2, float )
Gen_RECTPOINT( RectI, int2,   int   )
#undef Gen_RECTPOINT

/*
=================================================
	Rect_IsEmpty
	Rect_IsInvalid
=================================================
*/
#define Gen_RECTISEMPTY( _rect_ )																							\
	bool  Rect_IsEmpty (const _rect_ rect)		{ return IsZero( rect.v.x - rect.v.z ) and IsZero( rect.v.y - rect.v.w ); }	\
	bool  Rect_IsInvalid (const _rect_ rect)	{ return AnyLess( rect.v.zw, rect.v.xy ); }

Gen_RECTISEMPTY( Rect  )
Gen_RECTISEMPTY( RectI )
#undef Gen_RECTISEMPTY

/*
=================================================
	Rect_Center
	Rect_Size / Rect_HalfSize
=================================================
*/
float2	Rect_Center (const Rect rect)		{ return (rect.v.xy * 0.5f) + (rect.v.zw * 0.5f); }
float2	Rect_Size (const Rect rect)			{ return rect.v.zw - rect.v.xy; }
float2	Rect_HalfSize (const Rect rect)		{ return (rect.v.zw - rect.v.xy) * 0.5; }

int2	Rect_Center (const RectI rect)		{ return (rect.v.xy + rect.v.zw) / 2; }
int2	Rect_Size (const RectI rect)		{ return rect.v.zw - rect.v.xy; }
int2	Rect_HalfSize (const RectI rect)	{ return (rect.v.zw - rect.v.xy) / 2; }

//-----------------------------------------------------------------------------



/*
=================================================
	Circle_Create
=================================================
*/
Circle  Circle_Create (const float2 center, const float radius)
{
	Circle	res;
	res.center	= center;
	res.radius	= radius;
	return res;
}
//-----------------------------------------------------------------------------



/*
=================================================
	Triangle_Create
=================================================
*/
Triangle2d  Triangle_Create (const float2 a, const float2 b, const float2 c)
{
	Triangle2d	res;
	res.a = a;
	res.b = b;
	res.c = c;
	return res;
}

/*
=================================================
	Triangle_Area
=================================================
*/
float  Triangle_Area (const Triangle2d t)
{
	float2	ba	= t.b - t.a;
	float2	ca	= t.c - t.a;
	return	0.5 * Cross2( ba, ca );  // (ba.x * ca.y - ba.y * ca.x)
}

/*
=================================================
	Triangle_InnerCenterAndRadius
=================================================
*/
Circle  Triangle_InnerCenterAndRadius (const Triangle2d t)
{
	float	d0		= Distance( t.a, t.b );
	float	d1		= Distance( t.b, t.c );
	float	d2		= Distance( t.c, t.a );
	float	inv_s	= Rcp( d0 + d1 + d2 );

	float2	center	= (d0 * t.a + d1 * t.b + d2 * t.c) * inv_s;
	float	radius	= Triangle_Area( t ) * inv_s * 2.0;

	return Circle_Create( center, radius );
}
//-----------------------------------------------------------------------------



/*
=================================================
	Frustum2d_Create
=================================================
*/
Frustum2d  Frustum2d_Create (const float3 frustum[4])
{
	Frustum2d	res;
	res.planes = frustum;
	return res;
}

/*
=================================================
	Frustum2d_FromCornerPoints
=================================================
*/
Frustum2d  Frustum2d_FromCornerPoints (const float2 points[4])
{
	Frustum2d	res;
	res.planes[0] = Plane2d_FromPoints( points[0], points[2] );	// left
	res.planes[1] = Plane2d_FromPoints( points[3], points[1] );	// right
	res.planes[2] = Plane2d_FromPoints( points[2], points[3] );	// top
	res.planes[3] = Plane2d_FromPoints( points[1], points[0] );	// bottom
	return res;
}

Frustum2d  Frustum2d_FromCornerPoints (float2 p0, float2 p1, float2 p2, float2 p3)
{
	float2	points[4] = { p0, p1, p2, p3 };
	return Frustum2d_FromCornerPoints( points );
}

/*
=================================================
	Frustum2d_ToCornerPoints
=================================================
*/
void  Frustum2d_ToCornerPoints (const Frustum2d fr, out float2 outPoints[4])
{
	// 2 -- 3
	// |    |
	// 0 -- 1

	outPoints[0] = Plane2d_IntersectionPoint( fr.planes[0], fr.planes[3] );
	outPoints[1] = Plane2d_IntersectionPoint( fr.planes[1], fr.planes[3] );
	outPoints[2] = Plane2d_IntersectionPoint( fr.planes[0], fr.planes[2] );
	outPoints[3] = Plane2d_IntersectionPoint( fr.planes[1], fr.planes[2] );
}

/*
=================================================
	Frustum2d_ToRect
=================================================
*/
Rect  Frustum2d_ToRect (const Frustum2d fr)
{
	float2	points[4];
	Frustum2d_ToCornerPoints( fr, OUT points );

	Rect	res;
	res.v.xy = Min( Min( points[0], points[1] ), Min( points[2], points[3] ));
	res.v.zw = Max( Max( points[0], points[1] ), Max( points[2], points[3] ));
	return res;
}

/*
=================================================
	Frustum2d_IsVisible (Point)
=================================================
*/
bool  Frustum2d_IsVisible (const Frustum2d fr, const float2 point)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 4; ++i)
	{
		invisible += LessF( Plane2d_Distance( fr.planes[i], point ), 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum2d_IsVisible (Line)
=================================================
*/
bool  Frustum2d_IsVisible (const Frustum2d fr, const Line2d line)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 4; ++i)
	{
		invisible += LessF( Plane2d_Distance( fr.planes[i], line.begin ), 0.0 ) *
					 LessF( Plane2d_Distance( fr.planes[i], line.end   ), 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum2d_IsVisible (Rect)
=================================================
*/
bool  Frustum2d_IsVisible (const Frustum2d fr, const Rect rect)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 4; ++i)
	{
		float2	v = Max( rect.v.xy * fr.planes[i].xy, rect.v.zw * fr.planes[i].xy );
		float	d = v.x + v.y + fr.planes[i].z;
		invisible += LessF( d, 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum2d_IsVisible (Circle)
=================================================
*/
bool  Frustum2d_IsVisible (const Frustum2d fr, const Circle circle)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 4; ++i)
	{
		float	d = Plane2d_Distance( fr.planes[i], circle.center ) + circle.radius;
		invisible += LessF( d, 0.0 );
	}
	return invisible < 0.f;
}

/*
=================================================
	Frustum2d_IsVisible (Triangle)
=================================================
*/
bool  Frustum2d_IsVisible (const Frustum2d fr, const Triangle2d tri)
{
	const Line2d	ab	= Line_Create( tri.a, tri.b );
	const Line2d	ac	= Line_Create( tri.a, tri.c );

	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 4; ++i)
	{
		invisible += LessF( Plane2d_Distance( fr.planes[i], ab.begin ), 0.0 ) *
					 LessF( Plane2d_Distance( fr.planes[i], ab.end   ), 0.0 );

		invisible += LessF( Plane2d_Distance( fr.planes[i], ac.begin ), 0.0 ) *
					 LessF( Plane2d_Distance( fr.planes[i], ac.end   ), 0.0 );
	}
	return invisible < 0.f;
}
