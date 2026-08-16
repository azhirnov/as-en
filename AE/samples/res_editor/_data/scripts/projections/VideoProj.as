// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Test all video/photo projections.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define DIRECT_VIEW
#	define INVERSE_MASK
#	define WRITE_INVERSE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		int					mode		= 0;

		RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/yt360.jpg" );					mode = 1;	// 360
	//	RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/fisheye.jpg" );					mode = 3;	// fisheye
	//	RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/dual_fisheye.jpg" );			mode = 4;	// dual fisheye
	//	RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/vr360.jpg" );					mode = 5;	// VR360 (top/bottom)
	//	RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/cubemap_vr360.jpg" );			mode = 6;	// Cubemap VR360
	//	RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/vr180.jpg" );					mode = 7;	// VR180 (left/right)
	//	RC<Image>			vtex		= Image( EImageType::Float_2D, "res/tex/fisheye-vr180.jpg" );			mode = 8;	// fisheye VR

		RC<Image>			vtex2		= Image( EPixelFormat::RGBA8_UNorm, vtex.Dimension() );
		RC<Image>			mask		= Image( EPixelFormat::R8_UNorm, vtex.Dimension() );
		RC<FPVCamera>		camera		= FPVCamera();
		RC<DynamicUInt>		vr_mode		= DynamicUInt();
		RC<DynamicUInt>		proj		= DynamicUInt();
		RC<DynamicUInt>		eye			= DynamicUInt();
		RC<DynamicFloat>	col_scale	= DynamicFloat();
		RC<DynamicFloat2>	fisheye_a	= DynamicFloat2();
		RC<DynamicFloat>	fisheye_uv	= DynamicFloat();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s, s );
			camera.UpDownScale( s, s );
			camera.SideMovementScale( s );
		}

		Slider( vr_mode,	"VRMode",			0,				8,				mode );
		Slider( proj,		"SphereProj",		0,				2,				0 );		// 0 - none, 1 - tangential, 2 - inv tangential
		Slider( eye,		"Eye",				0,				1,				0 );		// for mode >= 5
		Slider( col_scale,	"ColScale",			0.0,			1.0,			0.4 );
		Slider( fisheye_a,	"FishEyeAngle",		float2(160.0),	float2(360.0),	float2(180.0) );
		Slider( fisheye_uv,	"FishEyeUVScale",	0.8,			1.0,			1.0 );

		// render loop
		{
			ClearImage( mask, RGBA32f(0.0) );

			RC<ComputePass>		pass = ComputePass( "", "INVERSE_MASK" );
			pass.Set( camera );
			pass.ArgInOut( "un_Mask",			mask );
			pass.Slider(   "iScale",			0.0,	1.0,	1.0 );
			pass.Constant( "iEye",				eye );
			pass.Constant( "iProj",				proj );
			pass.Constant( "iVRMode",			vr_mode );
			pass.Constant( "iFishEyeAngle",		fisheye_a );
			pass.Constant( "iFishEyeUVScale",	fisheye_uv );
			pass.Constant( "iDim",				mask.Dimension() );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( mask.Dimension() );
		}{
			RC<Postprocess>		pass = Postprocess( "", "WRITE_INVERSE" );
			pass.Output(	"out_Color",		vtex2 );
			pass.ArgIn(		"un_Mask",			mask,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Video",			vtex,	Sampler_LinearRepeat );
			pass.Constant(	"iColScale",		col_scale );
		}{
			RC<Postprocess>		pass = Postprocess( "", "DIRECT_VIEW" );
			pass.Set( camera );
			pass.Output(   "out_Color",			rt );
			pass.ArgIn(    "un_Video",			vtex,	Sampler_LinearRepeat );
			pass.ArgIn(    "un_Video2",			vtex2,	Sampler_LinearRepeat );
			pass.ArgIn(    "un_Mask",			mask,	Sampler_NearestClamp );
			pass.Constant( "iEye",				eye );
			pass.Constant( "iProj",				proj );
			pass.Constant( "iVRMode",			vr_mode );
			pass.Constant( "iColScale",			col_scale );
			pass.Constant( "iFishEyeAngle",		fisheye_a );
			pass.Constant( "iFishEyeUVScale",	fisheye_uv );
			pass.Slider(   "iInverseView",		0,	2,	1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(INVERSE_MASK) or defined(DIRECT_VIEW)
	#include "InvocationID.glsl"
	#include "CubeMap.glsl"
	#include "Ray.glsl"

	float3  TransformDir (const float3 dir)
	{
		switch ( iProj )
		{
			case 1 :
			{
				float3	coord_face = CM_TangentialSC_Inverse( dir );
				return CM_IdentitySC_Forward( coord_face.xy, int(coord_face.z) );
			}
			case 2 :
			{
				float3	coord_face = CM_IdentitySC_Inverse( dir );
				return CM_TangentialSC_Forward( coord_face.xy, int(coord_face.z) );
			}
		}
		return dir;
	}


	float4  ProjectTo2D (float3 dir, const float2 uv)
	{
		float2	uv_l, uv_r;

		dir = TransformDir( dir );

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
				uv_l = RayInverse_PlaneToCubemap360( dir );
				uv_r = uv_l;
				break;

			// FishEye 180
			case 3 :
				uv_l = RayInverse_FishEye( ToRad(iFishEyeAngle), dir, iFishEyeUVScale );
				uv_r = uv_l;
				break;

			// Double FishEye 180 (360)
			case 4 :
				uv_l = RayInverse_DualFishEye( ToRad(iFishEyeAngle.x), dir, iFishEyeUVScale );
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
				uv_l = RayInverse_PlaneToCubemapVR360( dir, 0 );
				uv_r = RayInverse_PlaneToCubemapVR360( dir, 1 );
				break;

			// VR180
			case 7 :
				uv_l = RayInverse_PlaneToVR180( dir, 0 );
				uv_r = RayInverse_PlaneToVR180( dir, 1 );
				break;

			// FishEye VR180
			case 8 :
				uv_l = RayInverse_FishEyeVR( ToRad(iFishEyeAngle.x), dir, 0 );
				uv_r = RayInverse_FishEyeVR( ToRad(iFishEyeAngle.x), dir, 1 );
				break;
		}
		return float4( uv_l, uv_r );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INVERSE_MASK

	float2  UVperEye (float4 uvRL)
	{
		return iEye == 0 ? uvRL.xy : uvRL.zw;
	}

	void  Main ()
	{
		float2	uv0		= GetGlobalCoordUNorm().xy;
		float2	uv1		= GetGlobalCoordUNorm( int3(1) ).xy;

				uv0		= ToUNorm( ToSNorm(uv0) * iScale );
				uv1		= ToUNorm( ToSNorm(uv1) * iScale );

		float3	dir0	= Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), 0.0, uv0 ).dir;
		float3	dir1	= Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), 0.0, uv1 ).dir;

				uv0		= UVperEye( ProjectTo2D( dir0, uv0 ));
				uv1		= UVperEye( ProjectTo2D( dir1, uv1 ));

		if ( IsNotUNorm( uv0 ))
			return;

		int2	coord0	= int2( uv0 * float2(iDim) + 0.5 );
		int2	coord1	= int2( uv1 * float2(iDim) + 0.5 );

		int2	tc0		= coord0;
		int2	tc1;

		tc1.x = coord0.x < coord1.x ? Min( coord1.x, coord0.x+5 ) : coord0.x+1;
		tc1.y = coord0.y < coord1.y ? Min( coord1.y, coord0.y+5 ) : coord0.y+1;

		tc0 = Clamp( tc0, int2(0), int2(iDim)-1 );
		tc1 = Clamp( tc1, int2(0), int2(iDim)-1 );

		for (int y = tc0.y; y < tc1.y; ++y)
		for (int x = tc0.x; x < tc1.x; ++x)
			gl.image.Store( un_Mask, int2(x,y), float4(1.0) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef WRITE_INVERSE

	void  Main ()
	{
		const float2	uv	= gl.FragCoord.xy * un_PerPass.invResolution;

		float4	col		= gl.texture.SampleLod( un_Video, uv, 0.0 );
		float	mask	= gl.texture.SampleLod( un_Mask, uv, 0.0 ).r;

		if ( mask < 0.5 )
			col *= iColScale;

		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DIRECT_VIEW
	#include "Frustum.glsl"


	float4  DirectView (const float3 dir, const float2 uv)
	{
		float4	uv_rl	= ProjectTo2D( dir, uv );

		float4	col_l	= IsUNorm( uv_rl.xy ) ? gl.texture.SampleLod( un_Video, uv_rl.xy, 0.0 ) : float4(1.0, 0.0, 0.0, 1.0);
		float4	col_r	= IsUNorm( uv_rl.zw ) ? gl.texture.SampleLod( un_Video, uv_rl.zw, 0.0 ) : float4(1.0, 0.0, 0.0, 1.0);

		return	iEye == 0 ? col_l : col_r;
	}


	float4  InverseView (const float2 uv)
	{
		float4		col		= gl.texture.SampleLod( un_Video, uv, 0.0 );
		float		mask	= gl.texture.SampleLod( un_Mask, uv, 0.0 ).r;

		if ( mask < 0.5 )
			col *= iColScale;

		return col;
	}


	float4  InverseView3 (const float3 dir, float2 uv)
	{
		float4	uv_rl = ProjectTo2D( dir, uv );

		uv = iEye == 0 ? uv_rl.xy : uv_rl.zw;

		return gl.texture.SampleLod( un_Video2, uv, 0.0 );
	}


	void Main ()
	{
		const float2	uv	= GetGlobalCoordUNorm().xy;
		const Ray		ray	= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, uv );

		switch ( iInverseView )
		{
			case 0 :	out_Color = DirectView( ray.dir, uv );		break;
			case 1 :	out_Color = InverseView( uv );				break;
			case 2 :	out_Color = InverseView3( ray.dir, uv );	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
