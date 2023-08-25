// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Normal calculation functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

// as macros
#if 0
    void  SmoothNormal2x1i (out float3 normal, float3 (*getPos)(int2, int2), int2 coord);
    void  SmoothNormal2x2i (out float3 normal, float3 (*getPos)(int2, int2), int2 coord);
    void  SmoothNormal3x3i (out float3 normal, float3 (*getPos)(int2, int2), int2 coord);

    void  SmoothNormal2x1f (out float3 normal, float3 (*getPos)(float2, float2), float2 coord, float scale);
    void  SmoothNormal2x2f (out float3 normal, float3 (*getPos)(float2, float2), float2 coord, float scale);
    void  SmoothNormal3x3f (out float3 normal, float3 (*getPos)(float2, float2), float2 coord, float scale);


    void  SmoothTBN2x2i (out float3x3 outTBN, float3 (*getPos)(int2,   int2),   float2 (*getUV)(int2,   int2),   int2 coord);
    void  SmoothTBN3x3i (out float3x3 outTBN, float3 (*getPos)(int2,   int2),   float2 (*getUV)(int2,   int2),   int2 coord);

    void  SmoothTBN2x2f (out float3x3 outTBN, float3 (*getPos)(float2, float2), float2 (*getUV)(float2, float2), float2 coord, float scale);
    void  SmoothTBN3x3f (out float3x3 outTBN, float3 (*getPos)(float2, float2), float2 (*getUV)(float2, float2), float2 coord, float scale);
#endif

#ifdef SH_FRAG
    ND_ float3  ScreenSpaceNormal_dxdy (const float3 pos);
    ND_ float3  ScreenSpaceNormal_QuadSubgroup (const float3 pos);
#endif

    void  CalcTBN (const float3 position0, const float2 texcoord0,
                   const float3 position1, const float2 texcoord1,
                   const float3 position2, const float2 texcoord2,
                   out float3x3 outTBN);
//-----------------------------------------------------------------------------



/*
=================================================
    SmoothNormal2x1
----
    sample points:
        0 1
        2
=================================================
*/
#define _impl_SmoothNormal2x1( _outNormal_, _getPos_, _coord_ )     \
                                                                    \
        const float3    v0  = _getPos_( _coord_, offset.xx );       \
        const float3    v1  = _getPos_( _coord_, offset.yx );       \
        const float3    v2  = _getPos_( _coord_, offset.xy );       \
                                                                    \
        _outNormal_  = Cross( v1 - v0, v2 - v0 );   /* 1-0, 2-0 */  \
        _outNormal_  = Normalize( _outNormal_ );                    \

#define SmoothNormal2x1i( _outNormal_, _getPos_, _coord_ )          \
    {                                                               \
        const int2      offset  = int2(0, 1);                       \
        _impl_SmoothNormal2x1( _outNormal_, _getPos_, _coord_ )     \
    }

#define SmoothNormal2x1f( _outNormal_, _getPos_, _coord_, _scale_ ) \
    {                                                               \
        const float2    offset  = float2(0.0, _scale_);             \
        _impl_SmoothNormal2x1( _outNormal_, _getPos_, _coord_ )     \
    }

/*
=================================================
    SmoothNormal2x2
----
    sample points:
        0 1
        2 3
=================================================
*/
#define _impl_SmoothNormal2x2( _outNormal_, _getPos_, _coord_ )     \
                                                                    \
        const float3    v0  = _getPos_( _coord_, offset.xx );       \
        const float3    v1  = _getPos_( _coord_, offset.yx );       \
        const float3    v2  = _getPos_( _coord_, offset.xy );       \
        const float3    v3  = _getPos_( _coord_, offset.yy );       \
                                                                    \
        _outNormal_  = float3(0.0);                                 \
        _outNormal_ += Cross( v1 - v0, v3 - v0 );   /* 1-0, 3-0 */  \
        _outNormal_ += Cross( v3 - v0, v2 - v0 );   /* 3-0, 2-0 */  \
        _outNormal_  = Normalize( _outNormal_ );                    \

#define SmoothNormal2x2i( _outNormal_, _getPos_, _coord_ )          \
    {                                                               \
        const int2      offset  = int2(0, 1);                       \
        _impl_SmoothNormal2x2( _outNormal_, _getPos_, _coord_ )     \
    }

#define SmoothNormal2x2f( _outNormal_, _getPos_, _coord_, _scale_ ) \
    {                                                               \
        const float2    offset  = float2(0.0, _scale_);             \
        _impl_SmoothNormal2x2( _outNormal_, _getPos_, _coord_ )     \
    }

/*
=================================================
    SmoothNormal3x3
----
    sample points:
        0 1 2
        3 4 5
        6 7 8
=================================================
*/
#define _impl_SmoothNormal3x3( _outNormal_, _getPos_, _coord_ )     \
                                                                    \
        const float3    v0  = _getPos_( _coord_, offset.xx );       \
        const float3    v1  = _getPos_( _coord_, offset.yx );       \
        const float3    v2  = _getPos_( _coord_, offset.zx );       \
        const float3    v3  = _getPos_( _coord_, offset.xy );       \
        const float3    v4  = _getPos_( _coord_, offset.yy );       \
        const float3    v5  = _getPos_( _coord_, offset.zy );       \
        const float3    v6  = _getPos_( _coord_, offset.xz );       \
        const float3    v7  = _getPos_( _coord_, offset.yz );       \
        const float3    v8  = _getPos_( _coord_, offset.zz );       \
                                                                    \
        _outNormal_  = float3(0.0);                                 \
        _outNormal_ += Cross( v1 - v4, v2 - v4 );   /* 1-4, 2-4 */  \
        _outNormal_ += Cross( v2 - v4, v5 - v4 );   /* 2-4, 5-4 */  \
        _outNormal_ += Cross( v5 - v4, v8 - v4 );   /* 5-4, 8-4 */  \
        _outNormal_ += Cross( v8 - v4, v7 - v4 );   /* 8-4, 7-4 */  \
        _outNormal_ += Cross( v7 - v4, v6 - v4 );   /* 7-4, 6-4 */  \
        _outNormal_ += Cross( v6 - v4, v3 - v4 );   /* 6-4, 3-4 */  \
        _outNormal_ += Cross( v3 - v4, v0 - v4 );   /* 3-4, 0-4 */  \
        _outNormal_ += Cross( v0 - v4, v1 - v4 );   /* 0-4, 1-4 */  \
        _outNormal_  = Normalize( _outNormal_ );                    \


#define SmoothNormal3x3i( _outNormal_, _getPos_, _coord_ )          \
    {                                                               \
        const int3      offset  = int3(-1, 0, 1);                   \
        _impl_SmoothNormal3x3( _outNormal_, _getPos_, _coord_ )     \
    }

#define SmoothNormal3x3f( _outNormal_, _getPos_, _coord_, _scale_ ) \
    {                                                               \
        const float3    offset  = float3(-_scale_, 0.0, _scale_);   \
        _impl_SmoothNormal3x3( _outNormal_, _getPos_, _coord_ )     \
    }

/*
=================================================
    ScreenSpaceNormal_*
----
    requires fragment shader
=================================================
*/
#ifdef SH_FRAG

    // Calc normal using derivatives
    float3  ScreenSpaceNormal_dxdy (const float3 pos)
    {
        float3  dx = gl.dFdxCoarse( pos );
        float3  dy = gl.dFdyCoarse( pos );
        float3  norm = Cross( dy, dx );
        return Normalize( norm );
    }

    // Calc normal using quad subgroup
    float3  ScreenSpaceNormal_QuadSubgroup (const float3 pos)
    {   
        float3  p0   = gl.subgroup.QuadBroadcast( pos, 0 );
        float3  p1   = gl.subgroup.QuadBroadcast( pos, 1 );
        float3  p2   = gl.subgroup.QuadBroadcast( pos, 2 );
        float3  norm = Cross( p2 - p0, p1 - p0 );
        return Normalize( norm );
    }
#endif

/*
=================================================
    CalcTBN
=================================================
*/
    void  CalcTBN (const float3 position0, const float2 texcoord0,
                   const float3 position1, const float2 texcoord1,
                   const float3 position2, const float2 texcoord2,
                   out float3x3 outTBN)
    {
        float3  e0 = float3( position1.x - position0.x,
                             texcoord1.x - texcoord0.x,
                             texcoord1.y - texcoord0.y );
        float3  e1 = float3( position2.x - position0.x,
                             texcoord2.x - texcoord0.x,
                             texcoord2.y - texcoord0.y );
        float3  cp = Cross( e0, e1 );

        float3  normal = Normalize( Cross( position1 - position0, position2 - position0 ));
        float3  tangent;
        float3  bitangent;

        tangent.x   = IsNotZero( cp.x ) ? -cp.y / cp.x : 0.0;
        bitangent.x = IsNotZero( cp.x ) ? -cp.z / cp.x : 0.0;

        e0.x = position1.y - position0.y;
        e1.x = position2.y - position0.y;
        cp   = Cross( e0, e1 );

        tangent.y   = IsNotZero( cp.x ) ? -cp.y / cp.x : 0.0;
        bitangent.y = IsNotZero( cp.x ) ? -cp.z / cp.x : 0.0;

        e0.x = position1.z - position0.z;
        e1.x = position2.z - position0.z;
        cp   = Cross( e0, e1 );

        tangent.z   = IsNotZero( cp.x ) ? -cp.y / cp.x : 0.0;
        bitangent.z = IsNotZero( cp.x ) ? -cp.z / cp.x : 0.0;

        tangent     = Normalize( tangent );
        bitangent   = Normalize( bitangent );

        outTBN      = float3x3( tangent, bitangent, normal );
    }

/*
=================================================
    SmoothTBN2x2
----
    sample points:
        0  1
        2  3
=================================================
*/
#define _impl_SmoothTBN2x2( _outTBN_, _getPos_, _getUV_, _coord_ )          \
                                                                            \
        const float3    pos0    = _getPos_( _coord_, offset.xx );           \
        const float3    pos1    = _getPos_( _coord_, offset.yx );           \
        const float3    pos2    = _getPos_( _coord_, offset.zx );           \
        const float3    pos3    = _getPos_( _coord_, offset.xy );           \
                                                                            \
        const float2    uv0     = _getUV_( _coord_, offset.xx );            \
        const float2    uv1     = _getUV_( _coord_, offset.yx );            \
        const float2    uv2     = _getUV_( _coord_, offset.zx );            \
        const float2    uv3     = _getUV_( _coord_, offset.xy );            \
                                                                            \
        float3x3    tbn0, tbn1;                                             \
        CalcTBN( pos0, uv0, pos1, uv1, pos3, uv3, OUT tbn0 );/* 1-0, 3-0 */ \
        CalcTBN( pos0, uv0, pos3, uv3, pos2, uv2, OUT tbn1 );/* 3-0, 2-0 */ \
                                                                            \
        _outTBN_[0] = Normalize( tbn0[0] + tbn1[0] );                       \
        _outTBN_[1] = Normalize( tbn0[1] + tbn1[1] );                       \
        _outTBN_[2] = Normalize( tbn0[2] + tbn1[2] );                       \


#define SmoothTBN2x2i( _outTBN_, _getPos_, _getUV_, _coord_ )               \
    {                                                                       \
        const int2      offset  = int2(0, 1);                               \
        _impl_SmoothTBN2x2( _outTBN_, _getPos_, _getUV_, _coord_ )          \
    }

#define SmoothTBN2x2f( _outTBN_, _getPos_, _getUV_, _coord_, _scale_ )      \
    {                                                                       \
        const float2    offset  = float2(0.0, _scale_);                     \
        _impl_SmoothTBN2x2( _outTBN_, _getPos_, _getUV_, _coord_ )          \
    }

/*
=================================================
    SmoothTBN3x3
----
    sample points:
        0 1 2
        3 4 5
        6 7 8
=================================================
*/
#define _impl_SmoothTBN3x3( _outTBN_, _getPos_, _getUV_, _coord_ )          \
                                                                            \
        const float3    pos0    = _getPos_( _coord_, offset.xx );           \
        const float3    pos1    = _getPos_( _coord_, offset.yx );           \
        const float3    pos2    = _getPos_( _coord_, offset.zx );           \
        const float3    pos3    = _getPos_( _coord_, offset.xy );           \
        const float3    pos4    = _getPos_( _coord_, offset.yy );           \
        const float3    pos5    = _getPos_( _coord_, offset.zy );           \
        const float3    pos6    = _getPos_( _coord_, offset.xz );           \
        const float3    pos7    = _getPos_( _coord_, offset.yz );           \
        const float3    pos8    = _getPos_( _coord_, offset.zz );           \
                                                                            \
        const float2    uv0     = _getUV_( _coord_, offset.xx );            \
        const float2    uv1     = _getUV_( _coord_, offset.yx );            \
        const float2    uv2     = _getUV_( _coord_, offset.zx );            \
        const float2    uv3     = _getUV_( _coord_, offset.xy );            \
        const float2    uv4     = _getUV_( _coord_, offset.yy );            \
        const float2    uv5     = _getUV_( _coord_, offset.zy );            \
        const float2    uv6     = _getUV_( _coord_, offset.xz );            \
        const float2    uv7     = _getUV_( _coord_, offset.yz );            \
        const float2    uv8     = _getUV_( _coord_, offset.zz );            \
                                                                            \
        float3x3    tbn0, tbn1, tbn2, tbn3;                                 \
        CalcTBN( pos4, uv4, pos1, uv1, pos2, uv2, OUT tbn0 );/* 1-4, 2-4 */ \
        CalcTBN( pos4, uv4, pos5, uv5, pos8, uv8, OUT tbn1 );/* 5-4, 8-4 */ \
        CalcTBN( pos4, uv4, pos7, uv7, pos6, uv6, OUT tbn2 );/* 7-4, 6-4 */ \
        CalcTBN( pos4, uv4, pos3, uv3, pos0, uv0, OUT tbn3 );/* 3-4, 0-4 */ \
                                                                            \
        _outTBN_[0] = Normalize( tbn0[0] + tbn1[0] + tbn2[0] + tbn3[0] );   \
        _outTBN_[1] = Normalize( tbn0[1] + tbn1[1] + tbn2[1] + tbn3[1] );   \
        _outTBN_[2] = Normalize( tbn0[2] + tbn1[2] + tbn2[2] + tbn3[2] );   \


#define SmoothTBN3x3i( _outTBN_, _getPos_, _getUV_, _coord_ )               \
    {                                                                       \
        const int3      offset  = int3(-1, 0, 1);                           \
        _impl_SmoothTBN3x3( _outTBN_, _getPos_, _getUV_, _coord_ )          \
    }

#define SmoothTBN3x3f( _outTBN_, _getPos_, _getUV_, _coord_, _scale_ )      \
    {                                                                       \
        const float3    offset  = float3(-_scale_, 0.0, _scale_);           \
        _impl_SmoothTBN3x3( _outTBN_, _getPos_, _getUV_, _coord_ )          \
    }
