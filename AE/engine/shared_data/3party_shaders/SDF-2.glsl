/*
    from https://github.com/electricsquare/raymarching-workshop
    MIT license
*/

ND_ float  SDF_MinCubic (const float a, float b, float k)
{
    // polynomial smooth min (k = 0.1);
    float   h = Max( k - Abs(a-b), 0.0 );
    return Min(a, b) - h * h * h / (6.0 * k * k);
}

float2  SDF_OpBlend (const float2 d1, const float2 d2)
{
    float   k = 2.0;
    float   d = SDF_MinCubic( d1.x, d2.x, k );
    float   m = Lerp( d1.y, d2.y, Clamp( d1.x - d, 0.f, 1.f ));
    return float2( d, m );
}
//-----------------------------------------------------------------------------


