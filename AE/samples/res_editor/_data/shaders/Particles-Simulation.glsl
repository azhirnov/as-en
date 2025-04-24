// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------

#include "AABB.glsl"
#include "InvocationID.glsl"
#include "Hash.glsl"
#include "Color.glsl"

//-----------------------------------------------------------------------------
// Types

struct GravityObject
{
	float3		position;
	float		gravity;
	float		radius;
};

struct MagneticObject
{
	float3		north;
	float3		south;
	float		induction;
};

struct Emitter
{
	float3		position;
	float		gravity;
	float		radius;
	float		induction;
};


//-----------------------------------------------------------------------------
// Utils

ND_ float3  GravityAccel (const float3 position, const float3 center, const float gravity)
{
	const float3 v = center - position;
	return Normalize( v ) * gravity / LengthSq( v );
}


ND_ float3  LinearMagneticFieldAccel (const float3 velocity, const float3 magneticInduction)
{
	// Lorentz force
	return Cross( velocity, magneticInduction );
}


ND_ float3  SphericalMagneticFieldAccel (const float3 velocity, const float3 postion,
									     const float3 northPos, const float3 southPos,
									     const float induction)
{
	const float3	nv = postion - northPos;
	const float3	n  = Normalize( nv ) * induction / LengthSq( nv );
	const float3	sv = southPos - postion;
	const float3	s  = Normalize( sv ) * induction / LengthSq( sv );
	return LinearMagneticFieldAccel( velocity, n + s );
}


void  UniformlyAcceleratedMotion (inout float3 pos, inout float3 vel, const float3 accel, const float dt)
{
	pos += vel * dt * 0.5;
	vel += accel * dt;
	pos += vel * dt * 0.5;
}
//-----------------------------------------------------------------------------



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
//-----------------------------------------------------------------------------



ND_ float3  ParticleEmitter_Plane (const float index, const float count)
{
	const float side = Sqrt( count );
	return float3( ToSNorm( float2(Mod( index, side ), Floor( index / side )) / side ), 0.0 );
}


ND_ float3  ParticleEmitter_Plane (const float index, const float count, const float ratio)
{
	const float	side_x	 = Sqrt( count * ratio );
	const float	side_y	 = count / side_x;
	const float	max_side = Max( side_x, side_y );

	return float3( (float2(Mod( index, side_x ), Floor( index / side_x )) * 2.0 - float2(side_x, side_y)) / max_side, 0.0 );
}


ND_ float3  ParticleEmitter_Circle (const float index, const float count)
{
	return float3( SinCos( float_Pi2 * index / (count - 1.0) ), 0.0 );
}


ND_ float3  ParticleEmitter_FillCircle (const float index, const float count)
{
	const float2	p = ParticleEmitter_Plane( index, count ).xy;
	return float3( SinCos( float_Pi2 * p.x ) * p.y, 0.0 );
}


ND_ float3  ParticleEmitter_Sphere (const float index, const float count)
{
	// TODO: spherical cube
	const float2	angle	= ParticleEmitter_Plane( index, count, 0.5 ).yx * float_Pi;
	const float2	theta	= SinCos( angle.x );
	const float2	phi		= SinCos( angle.y );

	return float3( theta.x * phi.y, theta.x * phi.x, theta.y );
}


ND_ float3  ParticleEmitter_ConeVector (const float index, const float count, const float zLength)
{
	const float2	p = ParticleEmitter_Plane( index, count ).xy;
	const float2	c = SinCos( float_Pi2 * p.x ) * p.y;
	return Normalize( float3( c, zLength ));
}
//-----------------------------------------------------------------------------
