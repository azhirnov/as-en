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
		RC<VideoImage>	vtex		= VideoImage( "res/video/h265_16k_360.mp4" );	mode = 1;	// 360
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/vp9_cube360.webm" );	mode = 2;	// Cubemap 360
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/vp9_vr360.webm" );		mode = 6;	// Cubemap VR360
	//	RC<VideoImage>	vtex		= VideoImage( "res/video/vp9_vr180.webm" );		mode = 7;	// VR180
		RC<FPVCamera>	camera		= FPVCamera();
		bool			has_2eyes	= mode >= 5;

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
				pass.Slider( "iEye",	-1,  1,  0 );		// -1 - both, 0 - left, 1 - right

			pass.Slider( "iVRMode",		0,	8,	mode );
			pass.Slider( "iSphereProj",	0,	1,	(mode == 2 or mode == 6 ? 1 : 0) );

			pass.Slider( "iPixScale",	-4.0,	4.0,	1.0 );
			pass.Constant( "iPixSize",	vtex.Dimension().Inverse() );
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


	float4  SelectEye (float4 uvLR)
	{
		switch ( iEye )
		{
			case 0 :	return gl.texture.Sample( un_Video, uvLR.xy );
			case 1 :	return gl.texture.Sample( un_Video, uvLR.zw );
		}

		float4	col_l	= gl.texture.Sample( un_Video, uvLR.xy );
		float4	col_r	= gl.texture.Sample( un_Video, uvLR.zw );
		float	t		= TriangleWave( un_PerPass.time );
				t		= GreaterF( t, 0.5 );

		return	Lerp( col_l, col_r, t );
	}


	float4  ProjectTo2D (float3 dir, const float2 uv)
	{
		float2	uv_l, uv_r;
		float2	px_size = iPixSize * iPixScale;

		if ( iSphereProj == 1 )
		{
			float3	coord_face = CM_TangentialSC_Inverse( dir );
			dir = CM_IdentitySC_Forward( coord_face.xy, int(coord_face.z) );
		}

		switch ( iVRMode )
		{
			// without projection
			case 0 :
				uv_l = uv;
				uv_r = uv_l;
				break;

			// 360 (YouTube)
			case 1 :
				uv_l = RayInverse_PlaneToSphereMap360( dir );
				uv_r = uv_l;
				break;

			// Cubemap 360 (YouTube)
			case 2 :
				uv_l = RayInverse_PlaneToCubemap360( dir, px_size );
				uv_r = uv_l;
				break;

			// FishEye 180
			case 3 :
				uv_l = RayInverse_FishEye( float2(ToRad(180.0)), dir, 1.0 );
				uv_r = uv_l;
				break;

			// Double FishEye 180 (360)
			case 4 :
				uv_l = RayInverse_DualFishEye( ToRad(180.0), dir, 1.0 );
				uv_r = uv_l;
				break;

			//----------------------------------------

			// VR360
			case 5 :
				uv_l = RayInverse_PlaneToVR360( dir, 0 );
				uv_r = RayInverse_PlaneToVR360( dir, 1 );
				break;

			// Cubemap VR360 (YouTube)
			case 6 :
				uv_l = RayInverse_PlaneToCubemapVR360( dir, 0, px_size );
				uv_r = RayInverse_PlaneToCubemapVR360( dir, 1, px_size );
				break;

			// VR180
			case 7 :
				uv_l = RayInverse_PlaneToVR180( dir, 0 );
				uv_r = RayInverse_PlaneToVR180( dir, 1 );
				break;

			// FishEye VR180
			case 8 :
				uv_l = RayInverse_FishEyeVR( ToRad(180.0), dir, 0 );
				uv_r = RayInverse_FishEyeVR( ToRad(180.0), dir, 1 );
				break;
		}
		return float4( uv_l, uv_r );
	}


	void Main ()
	{
		const float2	uv		= GetGlobalCoordUNorm().xy;
		float3			dir		= ViewDir( un_PerPass.camera.invViewProj, uv );
		float4			uv_lr	= ProjectTo2D( dir, uv );

		out_Color = SelectEye( uv_lr );
	}

#endif
//-----------------------------------------------------------------------------
