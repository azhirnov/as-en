// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple volumetric rendering.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<Image>		volume	= Image( EImageType::Float_3D, "res/tex/Cloud_PerlinWorley.dds" );
		RC<FPVCamera>	camera	= FPVCamera();

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

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy );
			pass.Set(	camera );
			pass.ArgIn( "un_Volume",	volume,		Sampler_LinearClamp );
			pass.ColorSelector( "iVolumeColor", RGBA32f(0.0, 1.0, 0.0, 1.0) );
			pass.Output( rt );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Intersectors.glsl"

	const float3	c_VolumePos		= float3(0.0, 0.0, 4.0);
	const AABB		c_VolumeAABB	= {float3(-1.0) + c_VolumePos, float3(1.0) + c_VolumePos};
	const uint		c_MaxSteps		= 256;

	ND_ float  Density (const float3 snorm)
	{
		return gl.texture.Sample( un_Volume, ToUNorm(snorm) ).r * 0.01;
	}

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

		// camera may be inside cube
		Ray_SetLength( INOUT ray, Max( t_min_max.x, ray.t ));

		for (uint i = 0; (i < c_MaxSteps) and (ray.t < t_min_max.y) and (accum_density < 0.999); ++i)
		{
			float3	local_pos	= ray.pos - center;
			float	density		= Density( local_pos / (box_size*0.5) );

			accum_density += density;

			Ray_Move( INOUT ray, step );
		}

		accum_density = Min( 1.0, accum_density );

		return iVolumeColor.rgb * accum_density;
	}
	//-----------------------------------------------------


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
