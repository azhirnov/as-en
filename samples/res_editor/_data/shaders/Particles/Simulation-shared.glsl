// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------

#include "AABB.glsl"
#include "GlobalIndex.glsl"
#include "Hash.glsl"
#include "Color.glsl"

//-----------------------------------------------------------------------------
// Types

struct GravityObject
{
    float3      position;
    float       gravity;
    float       radius;
};

struct MagneticObject
{
    float3      north;
    float3      south;
    float       induction;
};

struct Emitter
{
    float3      position;
    float       gravity;
    float       radius;
    float       induction;
};


//-----------------------------------------------------------------------------
// Utils

ND_ float3  GravityAccel (const float3 position, const float3 center, const float gravity)
{
    const float3 v = center - position;
    return Normalize( v ) * gravity / Dot( v, v );
}


ND_ float3  LinearMagneticFieldAccel (const float3 velocity, const float3 magneticInduction)
{
    // lorentz force
    return Cross( velocity, magneticInduction );
}


ND_ float3  SphericalMagneticFieldAccel (const float3 velocity, const float3 postion,
                                         const float3 northPos, const float3 southPos,
                                         const float induction)
{
    const float3    nv = postion - northPos;
    const float3    n  = Normalize( nv ) * induction / Dot( nv, nv );
    const float3    sv = southPos - postion;
    const float3    s  = Normalize( sv ) * induction / Dot( sv, sv );
    return LinearMagneticFieldAccel( velocity, n + s );
}


void  UniformlyAcceleratedMotion (inout float3 pos, inout float3 vel, const float3 accel, const float dt)
{
    pos += vel * dt * 0.5;
    vel += accel * dt;
    pos += vel * dt * 0.5;
}


ND_ uint  ParticleColor_FromNormalizedVelocity (const float3 velocity)
{
    return packUnorm4x8( float4( ToUNorm( Normalize( velocity )), 1.0 ));
}


ND_ uint  ParticleColor_FromVelocity (const float3 velocity)
{
    return packUnorm4x8( float4( ToUNorm( Clamp( velocity, -1.0, 1.0 )), 1.0 ));
}


ND_ uint  ParticleColor_FromVelocityLength (const float3 velocity)
{
    const float vel = 1.0 - Saturate( Length( velocity ));
    return packUnorm4x8( float4( HSVtoRGB( float3( vel, 1.0, 1.0 )), 1.0 ));
}


ND_ float3  ParticleEmitter_Plane (const float pointIndex, const float pointsCount)
{
    const float side = Sqrt( pointsCount );
    return float3( ToSNorm( float2(Mod( pointIndex, side ), Floor( pointIndex / side )) / side ), 0.0 );
}


ND_ float3  ParticleEmitter_Plane (const float pointIndex, const float pointsCount, const float ratio)
{
    const float side_x   = Sqrt( pointsCount * ratio );
    const float side_y   = pointsCount / side_x;
    const float max_side = Max( side_x, side_y );

    return float3( (float2(Mod( pointIndex, side_x ), Floor( pointIndex / side_x )) * 2.0 - float2(side_x, side_y)) / max_side, 0.0 );
}


ND_ float3  ParticleEmitter_Circle (const float pointIndex, const float pointsCount)
{
    return float3( SinCos( Pi() * 2.0 * pointIndex / (pointsCount - 1.0) ), 0.0 );
}


ND_ float3  ParticleEmitter_FillCircle (const float pointIndex, const float pointsCount)
{
    const float2    p = ParticleEmitter_Plane( pointIndex, pointsCount ).xy;
    return float3( SinCos( Pi() * 2.0 * p.x ) * p.y, 0.0 );
}


ND_ float3  ParticleEmitter_Sphere (const float pointIndex, const float pointsCount)
{
    const float2    angle   = ParticleEmitter_Plane( pointIndex, pointsCount, 0.5 ).yx * Pi();
    const float2    theta   = SinCos( angle.x );
    const float2    phi     = SinCos( angle.y );

    return float3( theta.x * phi.y, theta.x * phi.x, theta.y );
}


ND_ float3  ParticleEmitter_ConeVector (const float pointIndex, const float pointsCount, const float zLength)
{
    const float2    p = ParticleEmitter_Plane( pointIndex, pointsCount ).xy;
    const float2    c = SinCos( Pi() * 2.0 * p.x ) * p.y;
    return Normalize( float3( c, zLength ));
}
//-----------------------------------------------------------------------------
