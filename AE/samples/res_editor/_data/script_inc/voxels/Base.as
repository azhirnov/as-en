// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define RAYMARCH
#	define UNPACK_SELECTED
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#if defined(RAYMARCH) or defined(SDF_TO_VOXELS)
	#include "SDF.glsl"

	ND_ float  SDFScene (float3 pos)
	{
		pos.y -= 0.1;

		float	d0 = SDF_Sphere( pos, 0.5 );
		float	d1 = SDF_Sphere( SDF_Move( pos, float3(0.1, -0.5, -0.3) ), 0.5 );
		float	d2 = SDF_Box( SDF_Move( pos, float3(0.7, 0.5, -0.2) ), float3(0.5) );
		float	d02 = SDF_OpUnite( d0, d2, 0.5 );
		return	-Min( d02, d1 );
	}

	ND_ float  Density (const float3 pos)
	{
		return Clamp( SDFScene( pos ), -1.0, 1.0 );
	}

	float  Density (const int3 coord, const int3 size, const float3 invSize)
	{
		if ( AnyLess( coord, int3(0) ))
			return -1.0;

		if ( AnyGreaterEqual( coord, size ))
			return -1.0;

		return Density( ToSNorm( (float3(coord) + 0.5) * invSize ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RAYMARCH
	#include "Intersectors.glsl"

	const float3	c_VolumePos		= float3(0.0, 0.0, 4.0);
	const AABB		c_VolumeAABB	= {float3(-1.0) + c_VolumePos, float3(1.0) + c_VolumePos};
	const uint		c_MaxSteps		= 256;
	const float3	c_VolumeColor	= float3(0.0, 1.0, 0.0);


	ND_ float3  RayTrace (in Ray ray)
	{
		float2	t_min_max;
		if ( ! AABB_Ray_Intersect( c_VolumeAABB, ray, OUT t_min_max ))
			return float3(0.0);

		const float		max_depth		= Distance( c_VolumeAABB.min, c_VolumeAABB.max );
		const float		step			= max_depth / float(c_MaxSteps);
		const float3	center			= AABB_Center( c_VolumeAABB );
		const float3	box_size		= AABB_Size( c_VolumeAABB );

		float			accum_density	= 0.0;
		float			optical_depth	= 0.0;
		float			transmittance	= 1.0;

		// camera may be inside cube
		Ray_SetLength( INOUT ray, Max( t_min_max.x, ray.t ));

		for (uint i = 0; (i < c_MaxSteps) and (ray.t < t_min_max.y) and (transmittance > 0.01); ++i)
		{
			float3	local_pos	= ray.pos - center;
			float	density		= Density( local_pos );

			// simple volumetric
			if ( density > iMinDensity )
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

		return c_VolumeColor * accum_density * transmittance;
	}


	void  Main ()
	{
		Ray		ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, 0.1, gl.FragCoord.xy * un_PerPass.invResolution );
		out_Color.rgb = RayTrace( ray );
		out_Color.a   = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef UNPACK_SELECTED

	void  Main ()
	{
		uint	packed		= un_Select.packed;
		float	depth		= 1.0 - (float(packed >> 16) / float(0xFFFF));
		uint	inst_id		= packed & 0xFFFF;
		uint	packed_pos	= un_Voxels.elements[ inst_id ].packedPos;
		uint3	coord		= (uint3(packed_pos) >> uint3( 0, 8, 16 )) & 0xFF;

		un_Select.depth			= depth;
		un_Select.instanceId	= inst_id;
		un_Select.coord			= coord;
		un_Select.packed		= 0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Math.glsl"

	void  Main ()
	{
		float2	uv		= gl.FragCoord.xy * un_PerPass.invResolution;
		float4	col0	= gl.texture.Sample( un_RayMarch, uv );
		float4	col1	= gl.texture.Sample( un_Voxels, uv );

		out_Color = Lerp( col0, col1, iBlend );
	}

#endif
//-----------------------------------------------------------------------------
