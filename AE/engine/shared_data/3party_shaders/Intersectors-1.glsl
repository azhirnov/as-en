/*
    from https://iquilezles.org/articles/intersectors/
*/

/*
=================================================
    Sphere_Ray_Intersect
=================================================
*/
bool  Sphere_Ray_Intersect_v1 (const Sphere sphere, const Ray ray, out float2 tBeginEnd)
{
    float3  oc  = ray.pos - sphere.center;
    float   b   = Dot( oc, ray.dir );
    float   c   = Dot( oc, oc ) - Square( sphere.radius );
    float   h   = Square( b ) - c;
            h   = Sqrt( h );

    tBeginEnd = float2( -b-h, -b+h );
    return !(h < 0.0);
}

bool  Sphere_Ray_Intersect_v2 (const Sphere sphere, const Ray ray, out float2 tBeginEnd)
{
    float3  oc  = ray.pos - sphere.center;
    float   b   = Dot( oc, ray.dir );
    float3  qc  = oc - b * ray.dir;
    float   h   = Square( sphere.radius ) - Dot( qc, qc );
            h   = Sqrt( h );

    tBeginEnd = float2( -b-h, -b+h );
    return !(h < 0.0);
}

/*
=================================================
    AABB_Ray_Intersect
=================================================
*/
bool  AABB_Ray_Intersect (const float3 boxSize, const Ray ray, out float2 tBeginEnd, out float3 outNormal)
{
    float3  m   = 1.0 / ray.dir;    // can precompute if traversing a set of aligned boxes
    float3  n   = m * ray.pos;      // can precompute if traversing a set of aligned boxes
    float3  k   = Abs(m) * boxSize;
    float3  t1  = -n - k;
    float3  t2  = -n + k;
    float   tN  = Max( Max( t1.x, t1.y ), t1.z );
    float   tF  = Min( Min( t2.x, t2.y ), t2.z );

    outNormal = (tN > 0.0) ? Step( float3(tN), t1 ) :   // ray.pos outside the box
                             Step( t2, float3(tF) );    // ray.pos inside the box
    outNormal *= -SignOrZero( ray.dir );
    tBeginEnd = float2( tN, tF );

    return !(tN > tF || tF < 0.0);
}

