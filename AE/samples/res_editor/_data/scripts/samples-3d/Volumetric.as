// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple volumetric rendering.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define LIGHTING
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<FPVCamera>		camera	= FPVCamera();
		RC<DynamicUInt>		mode	= DynamicUInt();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 70.f );
			camera.RotationScale( 1.f, -1.f );

			const float	s = 0.6f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( -s );
			camera.SideMovementScale( s );
		}

		Slider( mode,	"Mode",		0,	1 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy );
			pass.Set( camera );
			pass.Output( rt );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy, "LIGHTING" );
			pass.Set( camera );
			pass.Output( rt );
			pass.Slider( "iLightDir",	float3(-1.0),	float3(1.0),	float3(0.0, 1.0, 0.0) );
			pass.Slider( "iLightStep",	0.01,			0.6,			0.4 );
			pass.EnableIfEqual( mode, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Noise.glsl"
	#include "SDF.glsl"
	#include "Intersectors.glsl"

	const float3	c_VolumePos		= float3(0.0, 0.0, 4.0);
	const AABB		c_VolumeAABB	= {float3(-1.0) + c_VolumePos, float3(1.0) + c_VolumePos};
	const uint		c_MaxSteps		= 256;
	const float3	c_VolumeColor	= float3(0.0, 1.0, 0.0);


	ND_ float  SDFSphere (const float3 pos) {
		return SDF_Sphere( pos, 0.3 );
	}

	ND_ float  SDFScene (const float3 pos) {
		return SDF_Repetition( pos, 0.6f, float3(1.f), SDFSphere );
	}

	ND_ float  Density (const float3 pos)
	{
		return Clamp( SDFScene( pos ), -1.0, 1.0 );
	}


# ifdef LIGHTING
	ND_ float3  RayTrace (in Ray ray)
	{
		float2	t_min_max;
		if ( ! AABB_Ray_Intersect( c_VolumeAABB, ray, OUT t_min_max ))
			return float3(0.0);

		const float		max_depth		= Distance( c_VolumeAABB.min, c_VolumeAABB.max );
		const float		step			= max_depth / float(c_MaxSteps);
		const float3	center			= AABB_Center( c_VolumeAABB );
		const float3	box_size		= AABB_Size( c_VolumeAABB );

		const uint		c_MaxLightSteps	= 10;

		const float		light_step		= max_depth / float(c_MaxLightSteps) * iLightStep;
		const float3	light_dir		= Normalize( iLightDir );
		const float		cos_theta		= Dot( ray.dir, light_dir );

		float			accum_density	= 0.0;
		float			optical_depth	= 0.0;
		float			transmittance	= 1.0;

		// camera may be inside cube
		Ray_SetLength( INOUT ray, Max( t_min_max.x, ray.t ));

		for (uint i = 0; (i < c_MaxSteps) and (ray.t < t_min_max.y) and (accum_density < 0.999); ++i)
		{
			float3	local_pos	= ray.pos - center;
			float	density		= -Density( local_pos );

			if ( density > 0.0 )
			{
				float	density_along_light = 0.0;

				for (uint j = 0; j < c_MaxLightSteps; ++j)
				{
					float3	pos2 = local_pos + light_dir * float(j) * light_step;
					density_along_light += Max( -Density( pos2 ), 0.0 );
				}

				float	beers_law		= Exp( -density_along_light );
				float	beers_modulated = Max( beers_law, 0.7 * Exp( -0.25 * density_along_light ));
						beers_law		= Lerp( beers_law, beers_modulated, -cos_theta * 0.5 + 0.5 );

				transmittance	 = Lerp( transmittance, beers_law, (1.0 - accum_density) );
				accum_density	+= density;
			}

			Ray_Move( INOUT ray, step );
		}

		float3	color = c_VolumeColor * Saturate(accum_density) * Saturate(transmittance);

		color += Max( transmittance - 1.0, 0.0 );	// HDR

		return color;
	}

# else
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
			float	density		= -Density( local_pos );

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

		return c_VolumeColor * accum_density * transmittance;
	}
# endif
	//-----------------------------------------------------------------------------


	void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir)
	{
		Ray	ray = Ray_Create( fragRayOri, fragRayDir, un_PerPass.camera.clipPlanes.x );
		fragColor.rgb = RayTrace( ray );
		fragColor.a   = 1.0;
	}

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, fragCoord / iResolution.xy );
		fragColor.rgb = RayTrace( ray );
		fragColor.a   = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
