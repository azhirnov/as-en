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
    void  SmoothNormal3x3i (out float3 normal, float3 (*getPos)(int2), int2 coord);
    void  SmoothNormal3x3i_2 (out float3 normal, float3 (*getPos)(int2), int2 coord);

    void  SmoothNormal3x3f (out float3 normal, float3 (*getPos)(float3), float2 coord, float scale);
    void  SmoothNormal3x3f_2 (out float3 normal, float3 (*getPos)(float3), float2 coord, float scale);
#endif
//-----------------------------------------------------------------------------


#define _impl_SmoothNormal3x3( _outNormal_, _getPos_, _coord_ )     \
                                                                    \
        const float3    v0  = _getPos_( _coord_ + offset.xx );      \
        const float3    v1  = _getPos_( _coord_ + offset.yx );      \
        const float3    v2  = _getPos_( _coord_ + offset.zx );      \
        const float3    v3  = _getPos_( _coord_ + offset.xy );      \
        const float3    v4  = _getPos_( _coord_ );                  \
        const float3    v5  = _getPos_( _coord_ + offset.zy );      \
        const float3    v6  = _getPos_( _coord_ + offset.xz );      \
        const float3    v7  = _getPos_( _coord_ + offset.yz );      \
        const float3    v8  = _getPos_( _coord_ + offset.zz );      \
                                                                    \
        _outNormal_  = float3(0.0);                                 \
        _outNormal_ += Cross( v1 - v4, v2 - v4 );   /* 1-4, 2-4 */  \
        _outNormal_ += Cross( v5 - v4, v8 - v4 );   /* 5-4, 8-4 */  \
        _outNormal_ += Cross( v7 - v4, v6 - v4 );   /* 7-4, 6-4 */  \
        _outNormal_ += Cross( v3 - v4, v0 - v4 );   /* 3-4, 0-4 */  \
        _outNormal_  = Normalize( _outNormal_ );                    \


#define _impl_SmoothNormal3x3_2( _outNormal_, _getPos_, _coord_ )   \
                                                                    \
        const float3    v0  = _getPos_( _coord_ + offset.xx );      \
        const float3    v1  = _getPos_( _coord_ + offset.yx );      \
        const float3    v2  = _getPos_( _coord_ + offset.zx );      \
        const float3    v3  = _getPos_( _coord_ + offset.xy );      \
        const float3    v4  = _getPos_( _coord_ );                  \
        const float3    v5  = _getPos_( _coord_ + offset.zy );      \
        const float3    v6  = _getPos_( _coord_ + offset.xz );      \
        const float3    v7  = _getPos_( _coord_ + offset.yz );      \
        const float3    v8  = _getPos_( _coord_ + offset.zz );      \
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

#define SmoothNormal3x3i_2( _outNormal_, _getPos_, _coord_ )        \
    {                                                               \
        const int3      offset  = int3(-1, 0, 1);                   \
        _impl_SmoothNormal3x3_2( _outNormal_, _getPos_, _coord_ )   \
    }

#define SmoothNormal3x3f( _outNormal_, _getPos_, _coord_, _scale_ ) \
    {                                                               \
        const float3    offset  = float3(-_scale_, 0.0, _scale_);   \
        _impl_SmoothNormal3x3( _outNormal_, _getPos_, _coord_ )     \
    }

#define SmoothNormal3x3f_2( _outNormal_, _getPos_, _coord_, _scale_ )\
    {                                                               \
        const float3    offset  = float3(-_scale_, 0.0, _scale_);   \
        _impl_SmoothNormal3x3_2( _outNormal_, _getPos_, _coord_ )   \
    }
