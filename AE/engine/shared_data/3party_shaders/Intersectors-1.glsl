/*
	from https://iquilezles.org/articles/intersectors/
*/

#ifdef AE_ENABLE_UNKNOWN_LICENSE

/*
=================================================
	Sphere_Ray_Intersect
=================================================
*/
bool  Sphere_Ray_Intersect_v1 (const Sphere sphere, const Ray ray, out float2 tBeginEnd)
{
	float3  oc	= ray.pos - sphere.center;
	float	b	= Dot( oc, ray.dir );
	float	c	= Dot( oc, oc ) - Square( sphere.radius );
	float	h	= Square( b ) - c;

	if ( h < 0.0 )
	{
		tBeginEnd = float2(0.0);
		return false;
	}

	h = Sqrt( h );
	tBeginEnd = float2( -b-h, -b+h );
	return true;
}

bool  Sphere_Ray_Intersect_v2 (const Sphere sphere, const Ray ray, out float2 tBeginEnd)
{
	float3	oc	= ray.pos - sphere.center;
	float	b	= Dot( oc, ray.dir );
	float3	qc	= oc - b * ray.dir;
	float	h	= Square( sphere.radius ) - Dot( qc, qc );

	if ( h < 0.0 )
	{
		tBeginEnd = float2(0.0);
		return false;
	}

	h = Sqrt( h );
	tBeginEnd = float2( -b-h, -b+h );
	return true;
}

/*
=================================================
	AABB_Ray_Intersect
=================================================
*/
bool  AABB_Ray_Intersect (const float3 boxSize, const Ray ray, out float2 tBeginEnd, out float3 outNormal)
{
	float3	m	= 1.0 / ray.dir;
	float3	n	= m * ray.pos;
	float3	k	= Abs(m) * boxSize;
	float3	t1	= -n - k;
	float3	t2	= -n + k;
	float	tN	= Max( Max( t1.x, t1.y ), t1.z );
	float	tF	= Min( Min( t2.x, t2.y ), t2.z );

	outNormal = (tN > 0.0) ? Step( float3(tN), t1 ) :	// ray.pos outside the box
							 Step( t2, float3(tF) );	// ray.pos inside the box
	outNormal *= -SignOrZero( ray.dir );
	tBeginEnd = float2( tN, tF );

	return !(tN > tF or tF < 0.0);
}

/*
=================================================
	Rect_Ray_Intersect
----
	modified AABB_Ray_Intersect
=================================================
*/
bool  Rect_Ray_Intersect (const float2 rectSize, const float2 rayDir, const float2 rayPos, out float2 tBeginEnd)
{
	float2	m	= 1.0 / rayDir;
	float2	n	= m * rayPos;
	float2	k	= Abs(m) * rectSize;
	float2	t1	= -n - k;
	float2	t2	= -n + k;
	float	tN	= Max( t1.x, t1.y );
	float	tF	= Min( t2.x, t2.y );

	tBeginEnd = float2( tN, tF );

	return !(tN > tF or tF < 0.0);
}

/*
=================================================
	Cone_Sphere_Intersects
----
	from https://bartwronski.com/2017/04/13/cull-that-cone/
=================================================
*/
bool  Cone_Sphere_Intersects (const Cone c, const Sphere s)
{
	const float3	v		= s.center - c.origin;
	const float		vv		= Dot( v, v );
	const float		vd		= Dot( v, c.dir );
	const float		dist	= Cos( c.halfAngle ) * Sqrt( vv - vd * vd ) - vd * Sin( c.halfAngle );

	const bool		angle_cull	= dist >  s.radius;
	const bool		front_cull	= vd   >  s.radius + c.height;
	const bool		back_cull	= vd   < -s.radius;

	return !(angle_cull or front_cull or back_cull);
}

#endif // AE_ENABLE_UNKNOWN_LICENSE

/*
=================================================
	AABB_Ray_Intersect
----
	from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
=================================================
*/
#ifdef AE_ENABLE_UNKNOWN_LICENSE

	bool  AABB_Ray_Intersect (const AABB aabb, const Ray ray, out float2 tBeginEnd)
	{

		float3	dirfrac	= 1.0 / ray.dir;
		float3	t135	= (aabb.min - ray.pos) * dirfrac;
		float3	t246	= (aabb.max - ray.pos) * dirfrac;
		float	tmin	= Max( Max( Min( t135[0], t246[0] ), Min( t135[1], t246[1] )), Min( t135[2], t246[2] ));
		float	tmax	= Min( Min( Max( t135[0], t246[0] ), Max( t135[1], t246[1] )), Max( t135[2], t246[2] ));

		tBeginEnd = float2( tmin, tmax );
		return (tmax >= 0) and (tmin <= tmax);
	}

#endif // AE_ENABLE_UNKNOWN_LICENSE

/*
=================================================
	Line_Line_Intersect
----
	from https://gist.github.com/TimSC/47203a0f5f15293d2099507ba5da44e6
=================================================
*/
#ifdef AE_LICENSE_CC0

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

#endif // AE_LICENSE_CC0
