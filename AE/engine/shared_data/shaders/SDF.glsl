/*
    Default signed distance fields.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Quaternion.glsl"


// 2D Shapes
ND_ float  SDF2_Line (const float2 position, const float2 point0, const float2 point1);
ND_ float  SDF2_Rect (const float2 position, const float2 hsize);
ND_ float  SDF2_Circle (const float2 position, const float radius);
ND_ float  SDF2_RoundedRect (const float2 position, const float2 hsize, float4 radius);
ND_ float  SDF2_OrientedRect (const float2 position, const float2 a, const float2 b, const float angle);
ND_ float  SDF2_EquilateralTriangle (float2 position, const float size);
ND_ float  SDF2_Triangle (const float2 position, const float2 p0, const float2 p1, const float2 p2);
ND_ float  SDF2_Pentagon (const float2 position, const float radius);
ND_ float  SDF2_Hexagon (const float2 position, const float radius);
ND_ float  SDF2_Octagon (const float2 position, const float radius);


// 3D Shapes
ND_ float  SDF_Sphere (const float3 position, const float radius);
ND_ float  SDF_Ellipsoid (const float3 position, const float3 radius);
ND_ float  SDF_Box (const float3 position, const float3 halfSize);
ND_ float  SDF_RoundedBox (const float3 position, const float3 halfSize, const float radius);
ND_ float  SDF_BoxFrame (const float3 position, const float3 halfSize, const float width);
ND_ float  SDF_Torus (const float3 position, const float2 outerAndInnerRadius);
ND_ float  SDF_Cylinder (const float3 position, const float2 radiusHeight);
ND_ float  SDF_Cone (const float3 position, const float2 direction);
ND_ float  SDF_Plane (const float3 position, const float3 norm, const float dist);
ND_ float  SDF_Plane (const float3 center, const float3 planePos, const float3 pos);
ND_ float  SDF_HexagonalPrism (const float3 position, const float2 h);
ND_ float  SDF_TriangularPrism (const float3 position, const float2 h);
ND_ float  SDF_Capsule (const float3 position, const float3 a, const float3 b, const float r);
ND_ float  SDF_CappedCone (const float3 position, const float height, const float r1, const float r2);
ND_ float  SDF_Octahedron (const float3 position, const float size);
ND_ float  SDF_Pyramid (const float3 position, const float height);


// Unions
ND_ float  SDF_OpUnite (const float d1, const float d2);
ND_ float  SDF_OpUnite (const float d1, const float d2, const float smoothFactor);
ND_ float  SDF_OpSub (const float d1, const float d2);
ND_ float  SDF_OpSub (const float d1, const float d2, float smoothFactor);
ND_ float  SDF_OpIntersect (const float d1, const float d2);
ND_ float  SDF_OpIntersect (const float d1, const float d2, float smoothFactor);
ND_ float  SDF_OpRoundedShape (const float dist, const float radius);
ND_ float  SDF_OpAnnularShape (const float dist, const float radius);
ND_ float  SDF_OpExtrusion (const float posZ, const float dist, const float height);
#if 0 // macros
ND_ float  SDF_OpRevolution (const float3 position, float (*sdf)(float2), float offset);
#endif

ND_ float  SDF_Length2 (float3 position);
ND_ float  SDF_Length6 (float3 position);
ND_ float  SDF_Length8 (float3 position);


// Transformation
ND_ float2  SDF_Move (const float2 position, const float  delta);
ND_ float2  SDF_Move (const float2 position, const float2 delta);
ND_ float3  SDF_Move (const float3 position, const float  delta);
ND_ float3  SDF_Move (const float3 position, const float3 delta);
ND_ float3  SDF_Rotate (const float3 position, const quat q);
ND_ float2  SDF_Rotate2D (const float2 position, const float angle);
ND_ float3  SDF_Transform (const float3 position, const quat q, const float3 delta);

#if 0 // macros
ND_ float  SDF_Scale (const float2 position, float scale, float (*sdf)(float2));
ND_ float  SDF_Scale (const float3 position, float scale, float (*sdf)(float3));

ND_ float  SDF_OpSymX (const float2 position, float (*sdf)(float2));
ND_ float  SDF_OpSymX (const float3 position, float (*sdf)(float3));

ND_ float  SDF_OpSymXZ (const float2 position, float (*sdf)(float2));
ND_ float  SDF_OpSymXZ (const float3 position, float (*sdf)(float3));

ND_ float  SDF_InfRepetition (const float2 position, const float2 center, float (*sdf)(float2));
ND_ float  SDF_InfRepetition (const float3 position, const float3 center, float (*sdf)(float3));

ND_ float  SDF_Repetition (const float2 position, const float step, const float2 count, float (*sdf)(float2));
ND_ float  SDF_Repetition (const float3 position, const float step, const float3 count, float (*sdf)(float3));

ND_ float  SDF_Glow (const float2 position, float (*sdf)(float2));
ND_ float  SDF_Glow (const float3 position, float (*sdf)(float3));
#endif


// Anti-aliased shapes
ND_ float  AA_QuadGrid (float2 uv, const float2 invGridSize, const float thicknessPx);

// multi-channel SDF
ND_ float  MCSDF_Median (const float3 msd);

//-----------------------------------------------------------------------------


#include "../3party_shaders/SDF-1.glsl"
//-----------------------------------------------------------------------------

/*
=================================================
    AA_QuadGrid
----
    anti-aliased SDF-based grid.
    'invGridSize' - 1.0 / grid_size_in_px
    'thicknessPx' - line thickness, must be >= 1.5
=================================================
*/
float  AA_QuadGrid (float2 uv, const float2 invGridSize, const float thicknessPx)
{
    // triangle wave
    uv = Fract( uv * invGridSize );
    uv = Min( uv, 1.0 - uv ) * 2.0;
    // grid lines
    uv = SmoothStep( float2(0.0), invGridSize * thicknessPx, uv );
    return Min( uv.x, uv.y );
}

/*
=================================================
    MCSDF_Median
=================================================
*/
float  MCSDF_Median (const float3 msd)
{
    return Max( Min( msd.r, msd.g ), Min( Max( msd.r, msd.g ), msd.b ));
}
