// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	360 / VR360 / VR180 video player.
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
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
	//	RC<VideoImage>	vtex	= VideoImage( "res/video/av1_16k_360.mp4" );	// 360
		RC<VideoImage>	vtex	= VideoImage( "res/video/h265_16k_360.mp4" );	// 360
	//	RC<VideoImage>	vtex	= VideoImage( "res/video/vp9_vr180.webm" );		// VR180
	//	RC<VideoImage>	vtex	= VideoImage( "res/video/vp9_vr360.webm" );		// Cubemap VR360
	//	RC<VideoImage>	vtex	= VideoImage( "res/video/vp9_cube360.webm" );	// Cubemap 360
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s, s );
			camera.UpDownScale( s, s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set(	 camera );
			pass.ArgIn(  "un_Video",	vtex,	Sampler_LinearClamp );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iEye",		0,	1 );
			pass.Slider( "VRMode",		0,	5,	1 );
			pass.Slider( "CameraMode",	0,	2 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "InvocationID.glsl"
	#include "Ray.glsl"


	void Main ()
	{
		const float2	uv				= GetGlobalCoordUNorm().xy;
		const float		dist_to_eye		= 0.5f;		// meters
		const float2	screen_size		= un_PerPass.resolution.xy * un_PerPass.mmPerPix * 0.001f;	// meters
		const float		curve_radius	= 1.8f;		// meters
		const float		z_near			= 0.1f;
		Ray				ray;
		float2			uv2;

		switch ( CameraMode )
		{
			case 0 :
				ray	= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, z_near, uv );
				break;

			case 1 :
				ray = Ray_PerspectiveFromFlatScreen( un_PerPass.camera.pos, dist_to_eye, screen_size, z_near, ToSNorm(uv) );
				Ray_Rotate( INOUT ray, MatTranspose(float3x3(un_PerPass.camera.view)) );
				break;

			case 2 :
				ray = Ray_PerspectiveFromCurvedScreen( un_PerPass.camera.pos, dist_to_eye, curve_radius, screen_size, z_near, ToSNorm(uv) );
				Ray_Rotate( INOUT ray, MatTranspose(float3x3(un_PerPass.camera.view)) );
				break;
		}

		switch ( VRMode )
		{
			// without projection
			case 0 :	uv2 = uv;																break;

			// 360
			case 1 :	uv2 = RayInverse_PlaneTo360( ray.dir );									break;

			// Cubemap 360 (YouTube)
			case 2 :	uv2 = RayInverse_PlaneToCubemap360( ray.dir );							break;

			// VR360
			case 3 :
			//	ray = Ray_PlaneToSphere( ToRad(float2(90.0, 90.0)), float3(0.0), 0.1, ToSNorm(uv) );
				uv2  = RayInverse_PlaneToVR360( ray.dir, iEye );								break;

			// Cubemap VR360 (YouTube)
			case 4 :
			//	ray = Ray_PlaneToSphere( ToRad(float2(90.0, 90.0)), float3(0.0), 0.1, ToSNorm(uv) );
				uv2  = RayInverse_PlaneToCubemapVR360( ray.dir, iEye );							break;

			// VR180
			case 5 :
				ray = Ray_PlaneToSphere( ToRad(float2(80.0, 80.0)), float3(0.0), 0.1, ToSNorm(uv) );
				uv2  = RayInverse_PlaneToVR180( ray.dir, iEye );								break;
		}

		out_Color = gl.texture.Sample( un_Video, uv2 );
	}

#endif
//-----------------------------------------------------------------------------
