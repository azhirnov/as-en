

/*
=================================================
	AABB_Ray_Intersect
=================================================
*/
bool  AABB_Ray_Intersect (const AABB aabb, const Ray ray, out float2 tBeginEnd)
{
	// from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

	float3	dirfrac	= 1.0 / ray.dir;
	float3	t135	= (aabb.min - ray.pos) * dirfrac;
	float3	t246	= (aabb.max - ray.pos) * dirfrac;
	float	tmin	= Max( Max( Min( t135[0], t246[0] ), Min( t135[1], t246[1] )), Min( t135[2], t246[2] ));
	float	tmax	= Min( Min( Max( t135[0], t246[0] ), Max( t135[1], t246[1] )), Max( t135[2], t246[2] ));

	tBeginEnd = float2( tmin, tmax );
	return (tmax >= 0) and (tmin <= tmax);
}
