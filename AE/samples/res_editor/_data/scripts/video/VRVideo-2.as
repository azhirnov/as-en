// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	360 / VR360 / VR180 video player.
	Used multi-planar format (YUV).
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
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		int				mode		= 0;
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/av1_16k_360.mp4" );	mode = 1;	// 360
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/h265_16k_360.mp4" );	mode = 1;	// 360
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/vp9_cube360.webm" );	mode = 2;	// Cubemap 360
		RC<VideoImage>	vtex		= VideoImage( "res/video/vp9_vr360.webm" );		mode = 4;	// Cubemap VR360
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/vp9_vr180.webm" );		mode = 5;	// VR180
		RC<FPVCamera>	camera		= FPVCamera();
		bool			has_2eyes	= mode >= 3;

		vtex.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_709 );
		vtex.Ycbcr_Range( ESamplerYcbcrRange::ITU_Full );
		vtex.Ycbcr_XChromaOffset( ESamplerChromaLocation::CositedEven );
		vtex.Ycbcr_YChromaOffset( ESamplerChromaLocation::CositedEven );
		vtex.Ycbcr_ChromaFilter( EFilter::Nearest );

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

			if ( has_2eyes )
				pass.Slider( "iEye",	-1,  1,  -1 );		// -1 - both, 0 - left, 1 - right

			pass.Slider( "VRMode",		0,	5,	mode );
			pass.Slider( "iProj",		0,	1,	has_2eyes ? 1 : 0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "InvocationID.glsl"
	#include "CubeMap.glsl"
	#include "Ray.glsl"

	#ifndef iEye
	# define iEye	0
	#endif


	float4  SelectEye (float2 uvL, float2 uvR)
	{
		switch ( iEye )
		{
			case 0 :	return gl.texture.Sample( un_Video, uvL );
			case 1 :	return gl.texture.Sample( un_Video, uvR );
		}

		float4	col_l	= gl.texture.Sample( un_Video, uvL );
		float4	col_r	= gl.texture.Sample( un_Video, uvR );
		float	t		= TriangleWave( un_PerPass.time * 2.0 );
				t		= GreaterF( t, 0.5 );

		return	Lerp( col_l, col_r, t );
	}


	void Main ()
	{
		const float2	uv				= GetGlobalCoordUNorm().xy;
		const float		dist_to_eye		= 0.5f;		// meters
		const float2	screen_size		= un_PerPass.resolution.xy * un_PerPass.mmPerPix * 0.001f;	// meters
		const float		curve_radius	= 1.8f;		// meters
		const float		z_near			= 0.1f;
		Ray				ray				= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, z_near, uv );
		float2			uv_l, uv_r;
		
		if ( iProj == 1 )
		{
			float3	coord_face = CM_TangentialSC_Inverse( ray.dir );
			ray.dir = CM_IdentitySC_Forward( coord_face.xy, int(coord_face.z) );
		}

		switch ( VRMode )
		{
			// without projection
			case 0 :
				uv_l = uv;
				break;

			// 360
			case 1 :
				uv_l = RayInverse_PlaneTo360( ray.dir );
				break;

			// Cubemap 360 (YouTube)
			case 2 :
				uv_l = RayInverse_PlaneToCubemap360( ray.dir );
				break;

			// VR360
			case 3 :
				uv_l = RayInverse_PlaneToVR360( ray.dir, 0 );
				uv_r = RayInverse_PlaneToVR360( ray.dir, 1 );
				break;
			
			// Cubemap VR360 (YouTube)
			case 4 :
				uv_l = RayInverse_PlaneToCubemapVR360( ray.dir, 0 );
				uv_r = RayInverse_PlaneToCubemapVR360( ray.dir, 1 );
				break;
			
			// VR180
			case 5 :
			{
				ray = Ray_PlaneToSphere( ToRad(float2(80.0, 80.0)), float3(0.0), 0.1, ToSNorm(uv) );
				if ( iProj == 1 )
				{
					float3	coord_face = CM_TangentialSC_Inverse( ray.dir );
					ray.dir = CM_IdentitySC_Forward( coord_face.xy, int(coord_face.z) );
				}
				uv_l = RayInverse_PlaneToVR180( ray.dir, 0 );
				uv_r = RayInverse_PlaneToVR180( ray.dir, 1 );
				break;
			}
		}

		out_Color = SelectEye( uv_l, uv_r );
	}

#endif
//-----------------------------------------------------------------------------
