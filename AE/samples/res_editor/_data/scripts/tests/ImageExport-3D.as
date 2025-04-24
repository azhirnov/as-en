// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Press 'F3' to export 3D image to a file.
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
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<Image>		volume	= Image( EPixelFormat::R8_UNorm, uint3(256) );			volume.Name( "Volume" );
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
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage", volume );
			pass.LocalSize( 4, 4, 4 );
			pass.DispatchThreads( volume.Dimension3() );

			Export( volume, "exported-image-3d-.aeimg" );
		}{
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
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "SDF.glsl"

	ND_ float  SDFSphere (const float3 pos) {
		return SDF_Sphere( pos, 0.2 );
	}

	ND_ float  SDFScenes (const float3 pos) {
		return SDF_Repetition( pos, 0.5f, float3(10.f), SDFSphere );
	}

	ND_ float  Density (const float3 pos)
	{
		float	x = Saturate( 1.0 - SDFScenes( pos ));
		x = Pow( x, 8.0 );
		return x;
	}

	void Main ()
	{
		float4	color = float4(Density( GetGlobalCoordSNorm() * 2.f ));

		gl.image.Store( un_OutImage, int3(GetGlobalCoord()), color );
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

