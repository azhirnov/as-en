// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define GEN_NOISE
#	define GEN_TURB
#	define APPLY_SPLINE
#	define TRACE_OPAQUE
#	define TRACE_CLOUD
#	define TRACE_CLOUD2
#	define VIEW_2D
#	define vTILE		8	// volume tile size
#endif
//-----------------------------------------------------------------------------
#if defined(SH_COMPUTE) && defined(vTILE)
	#include "InvocationID.glsl"

	ND_ int3  GlobalThreadID ()
	{
		int3	size	= GetGlobalSize();
		int3	pos		= GetGlobalCoord();

		pos += size * IndexToVec3( int(un_PerPass.frame), vTILE );
		return pos;
	}

	ND_ float3  GlobalThreadSNorm ()
	{
		int3	size = GetGlobalSize() * vTILE;
		int3	pos  = GlobalThreadID();
		return UIndexToSNormRound( pos, size );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE

	#include "tools/SphereNoise.glsl"

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TURB

	#include "tools/SphereNoise.glsl"

#endif
//-----------------------------------------------------------------------------
#ifdef APPLY_SPLINE

	#include "tools/SphereNoise.glsl"

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_2D

	#include "tools/TerrainNoise.glsl"

#endif
//-----------------------------------------------------------------------------
#if defined(TRACE_OPAQUE) or defined(TRACE_CLOUD) or defined(TRACE_CLOUD2)
	#include "Fog.glsl"
	#include "Color.glsl"
	#include "Intersectors.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"


	const float3	c_VolumePos		= float3(0.0, 0.0, 3.0);
	const AABB		c_VolumeAABB	= {float3(-1.0) + c_VolumePos, float3(1.0) + c_VolumePos};
	const uint		c_MaxOpSteps	= 512;
	const uint		c_MaxTrSteps	= 256;


	ND_ float  Noise (float3 pos)
	{
		return ToUNorm( gl.texture.SampleLod( un_Volume, ToUNorm(pos), 0.0 ).r );
	}


  #ifdef TRACE_OPAQUE
	ND_ float4  RayTraceOpaque (in Ray ray, const float3 center, const float2 minMaxT)
	{
		const float		step	= Distance( c_VolumeAABB.min, c_VolumeAABB.max ) / float(c_MaxOpSteps) * 0.5;
		float			n		= 0.f;
		float			scale	= 1.0;

		for (uint i = 0; All3( i < c_MaxOpSteps, scale > 0.06, ray.t < minMaxT.y ); ++i)
		{
			float3	local_pos = ray.pos - center;

			n		= Noise( local_pos );
			scale	= n > iMinValue ? scale * 0.5 : scale;

			Ray_Move( INOUT ray, step * scale );
		}

		if ( n > iMinValue )
		{
			float	fog = FogFactorExp( ray.t - minMaxT.x, 1.8f );
			float3	col = Rainbow( 1.0 - n ).rgb;
			float3	fc	= float3(0.25);
			return float4( Lerp( col, fc, fog ), 1.0 );
		}
		else
			return float4(0.0);
	}
  #endif


  #ifdef TRACE_CLOUD
	ND_ float  DensityLowRes (float3 pos)
	{
		// TODO: use mipmap
		return ToSNorm( Noise( pos )) * iDensity;
	}

	ND_ float4  RayTraceTranslucent (in Ray ray, const float3 center, const float maxT)
	{
		const float		max_depth		= Distance( c_VolumeAABB.min, c_VolumeAABB.max );
		const float		step			= max_depth / float(c_MaxTrSteps);

		float			accum_density	= 0.0;
		float			optical_depth	= 0.0;
		float			transmittance	= 1.0;

		for (uint i = 0; All3( i < c_MaxTrSteps, transmittance > 0.01, ray.t < maxT ); ++i)
		{
			float3	local_pos	= ray.pos - center;
			float	density		= DensityLowRes( local_pos );

			// simple volumetric
			if ( density > 0.0 )
			{
				float	scattering	= density;
				float	powder		= 1.0 - Exp( -scattering * 122.0 );
				float	prev_t		= transmittance;

				optical_depth += scattering * step;
				transmittance  = Exp( -optical_depth * 400.0 );
				accum_density += powder * (prev_t - transmittance);
			}

			Ray_Move( INOUT ray, step );
		}

		transmittance	= Saturate( 1.0 - transmittance );
		accum_density	= Saturate( accum_density );

		float3	light_col = RemoveSRGBCurve( iLightColor.rgb ) * transmittance * accum_density;

		return float4(ApplySRGBCurve( light_col ), 1.0);
	}
  #endif


  #ifdef TRACE_CLOUD2
	const uint		c_MaxLightSteps	= 3;

	ND_ float  DensityLowRes (float3 pos)
	{
		// TODO: use mipmap
		return ToSNorm( Noise( pos )) * iDensity;
	}

	ND_ float4  RayTraceTranslucent (in Ray ray, const float3 center, const float maxT)
	{
		const float		max_depth		= Distance( c_VolumeAABB.min, c_VolumeAABB.max );
		const float		step			= max_depth / float(c_MaxTrSteps);
		const float		light_step		= max_depth / float(c_MaxLightSteps) * 0.5;
		const float3	light_dir		= Normalize( iLightDir );
		const float		cos_theta		= Dot( ray.dir, light_dir );

		float			accum_density	= 0.0;
		float			transmittance	= 1.0;

		for (uint i = 0; All3( i < c_MaxTrSteps, ray.t < maxT, accum_density < 0.999 ); ++i)
		{
			float3	local_pos	= ray.pos - center;
			float	density		= DensityLowRes( local_pos );

			if ( density > 0.0 )
			{
				float	density_along_light = 0.0;

				for (uint j = 0; j < c_MaxLightSteps; ++j)
				{
					float3	pos2 = local_pos + light_dir * float(j) * light_step;
					density_along_light += DensityLowRes( pos2 );
				}

				float	beers_law		= Exp( -density_along_light );
				float	beers_modulated = Max( beers_law, 0.7 * Exp( -0.25 * density_along_light ));
						beers_law		= Lerp( beers_law, beers_modulated, -cos_theta * 0.5 + 0.5 );

				transmittance	 = Lerp( transmittance, beers_law, (1.0 - accum_density) );
				accum_density	+= density;
			}

			Ray_Move( INOUT ray, step );
		}

		transmittance	= Max( 0.0, transmittance );
		accum_density	= Min( 1.0, accum_density );

		float3	light_col = RemoveSRGBCurve( iLightColor.rgb ) * transmittance * accum_density;

		return float4(ApplySRGBCurve( light_col ), 1.0);
	}
  #endif


	ND_ float4  RayTrace (in Ray ray)
	{
		float2	t_min_max;
		if ( ! AABB_Ray_Intersect( c_VolumeAABB, ray, OUT t_min_max ))
			return float4(0.0);

		const float3	center		= AABB_Center( c_VolumeAABB );
		const float3	box_size	= AABB_Size( c_VolumeAABB );
		const float		begin_t		= Max( t_min_max.x, ray.t );

		// camera may be inside cube
		Ray_SetLength( INOUT ray, begin_t );

	  #ifdef TRACE_OPAQUE
		return RayTraceOpaque( ray, center, float2(begin_t, t_min_max.y) );
	  #endif
	  #ifdef TRACE_CLOUD
		return RayTraceTranslucent( ray, center, t_min_max.y );
	  #endif
	  #ifdef TRACE_CLOUD2
		return RayTraceTranslucent( ray, center, t_min_max.y );
	  #endif
	}

	void  Main ()
	{
		Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		out_Color = RayTrace( ray );
	}

#endif
//-----------------------------------------------------------------------------
