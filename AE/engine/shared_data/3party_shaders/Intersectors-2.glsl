
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
