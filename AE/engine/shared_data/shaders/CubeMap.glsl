// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Cube map utilities
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float3  CM_RotateVec (const float3 c, const int face);
ND_ float4  CM_InverseRotation (const float3 c);        // returns {xyz, face}

ND_ float3  CM_IdentitySC_Forward (const float2 snormCoord, const int face);
ND_ float3  CM_IdentitySC_Inverse (const float3 coord);

ND_ float3  CM_TangentialSC_Forward (const float2 snormCoord, const int face);
ND_ float3  CM_TangentialSC_Inverse (const float3 coord);
//-----------------------------------------------------------------------------



/*
=================================================
    CM_RotateVec / CM_InverseRotation
=================================================
*/
float3  CM_RotateVec (const float3 c, const int face)
{
  #if 0
    switch ( face )
    {
        case 0 : return float3( c.z,  c.y, -c.x);   // X+
        case 1 : return float3(-c.z,  c.y,  c.x);   // X-
        case 2 : return float3( c.x, -c.z,  c.y);   // Y+
        case 3 : return float3( c.x,  c.z, -c.y);   // Y-
        case 4 : return float3( c.x,  c.y,  c.z);   // Z+
        case 5 : return float3(-c.x,  c.y, -c.z);   // Z-
    }
    return float3(0.f);
  #else
    return  float3( c.z,  c.y, -c.x) * float(face == 0) +
            float3(-c.z,  c.y,  c.x) * float(face == 1) +
            float3( c.x, -c.z,  c.y) * float(face == 2) +
            float3( c.x,  c.z, -c.y) * float(face == 3) +
            float3( c.x,  c.y,  c.z) * float(face == 4) +
            float3(-c.x,  c.y, -c.z) * float(face == 5);
  #endif
}

float4  CM_InverseRotation (const float3 c)
{
    // front (xy space)
    if ( All(bool3( Abs(c.x) <= c.z,  c.z > 0.f,  Abs(c.y) <= c.z )))
        return float4( c.x, c.y, c.z, 4.f );

    // right (zy space)
    if ( All(bool3( Abs(c.z) <= c.x,  c.x > 0.f,  Abs(c.y) <= c.x )))
        return float4( -c.z, c.y, c.x, 0.f );

    // back (xy space)
    if ( All(bool3( Abs(c.x) <= -c.z,  c.z < 0.f,  Abs(c.y) <= -c.z )))
        return float4( -c.x, c.y, -c.z, 5.f );

    // left (zy space)
    if ( All(bool3( Abs(c.z) <= -c.x,  c.x < 0.f,  Abs(c.y) <= -c.x )))
        return float4( c.z, c.y, -c.x, 1.f );

    // up (xz space)
    if ( c.y > 0.f )
        return float4( c.x, c.z, c.y, 2.f );

    // down (xz space)
    return float4( c.x, -c.z, -c.y, 3.f );

    // TODO:
    //  float2( 0.5f + 0.5f * ATan(dir.z, dir.x) / Pi(),
    //          1.f - ACos(dir.y) / Pi());
}

/*
=================================================
    CM_IdentitySC_Forward / CM_IdentitySC_Inverse
----
    identity spherical cube projection
=================================================
*/
float3  CM_IdentitySC_Forward (const float2 snormCoord, const int face)
{
    return Normalize( CM_RotateVec( float3(snormCoord, 1.f), face ));
}

float3  CM_IdentitySC_Inverse (const float3 coord)
{
    float4  coord_face = CM_InverseRotation( coord );
    coord_face.xy /= coord_face.z;
    return coord_face.xyw;
}

/*
=================================================
    CM_TangentialSC_Forward / CM_TangentialSC_Inverse
----
    tangential spherical cube projection
=================================================
*/
float3  CM_TangentialSC_Forward (const float2 snormCoord, const int face)
{
    const float warp_theta      = 0.868734829276f;
    const float tan_warp_theta  = 1.182286685546f; //tan( warp_theta );
    float2      coord           = Tan( warp_theta * snormCoord ) / tan_warp_theta;

    return Normalize( CM_RotateVec( float3(coord.x, coord.y, 1.f), face ));
}

float3  CM_TangentialSC_Inverse (const float3 coord)
{
    const float warp_theta      = 0.868734829276f;
    const float tan_warp_theta  = 1.182286685546f; //tan( warp_theta );

    float4  coord_face = CM_InverseRotation( coord );
    coord_face.xy /= coord_face.z;

    float2  c = ATan( coord_face.xy * tan_warp_theta ) / warp_theta;
    return float3( c.xy, coord_face.w );
}
