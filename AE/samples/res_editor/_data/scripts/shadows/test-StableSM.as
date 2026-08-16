// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Also see [DeferredSM](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/shadows/DeferredSM.as).
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
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>	buf		= Buffer();

		buf.UseLayout(
			"ParamsBuffer",
			"	float2		frustumCorners [4];"
			"	float2		shadowCorners [4];"
			"	float2		virtualShadowCorners [4];"
			"	float4		scaleBias;"					// uv to world space
			"	float4x4	shadowVP;"
		);

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "PREPARE" );
			pass.ArgInOut(	"un_Params",	buf );
			pass.Slider(	"iAnimation",	0,					1,						1 );
			pass.Slider(	"iFixedSMSize",	0,					1,						1 );
			// camera projection & view matrix
			pass.Slider(	"iFOV",			10.0,				120.0,					80.0 );
			pass.Slider(	"iRotation",	0.0,				1.0,					0.0 );
			pass.Slider(	"iZRange",		float2(0.1, 10.0),	float2(1.0, 1000.0),	float2(0.1, 100.0) );
			// shadow
			pass.Slider(	"iLightDir",	float3(-1.0),		float3(1.0),			float3(0.4, 0.0, -0.35) );
			pass.Constant(	"iShadowDim",	uint2(512) );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Output(	"out_Color",	rt,		RGBA32f(0.0) );
			pass.ArgIn(		"un_Params",	buf );
			pass.Slider(	"iUVScale",		0.25,	2.0,	0.5  );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "AABB.glsl"
	#include "Matrix.glsl"
	#include "Sphere.glsl"
	#include "Geometry2D.glsl"


	// input and output in light space
	float2  SnapToTexel (float2 centerLS, float2 sizeLS)
	{
		float2	units_per_texel	= sizeLS / float2(iShadowDim);

		float2	min_ls	= centerLS - sizeLS * 0.5;
				min_ls	= Floor( min_ls / units_per_texel ) * units_per_texel;

		return	min_ls + sizeLS * 0.5;
	}


	AABB  CalcShadowBBox (float3x3 lightView, float4x4 proj, float4x4 invVP, float shadowDist)
	{
		float		z		= FastProjectZ( proj, shadowDist );

		// 'UnProjectNDC' returns world space, camera at (0,0,0)
		// 'lightView * wp' returns light space

		float3		near	= float3(0.0);	// same as camera pos

		float3		far0	= lightView * UnProjectNDC( invVP, float3(-1.0,  1.0, z) );
		float3		far1	= lightView * UnProjectNDC( invVP, float3( 1.0,  1.0, z) );
		float3		far2	= lightView * UnProjectNDC( invVP, float3(-1.0, -1.0, z) );
		float3		far3	= lightView * UnProjectNDC( invVP, float3( 1.0, -1.0, z) );

		float3		min		= Min( near, Min( Min( far0, far1 ), Min( far2, far3 )) );
		float3		max		= Max( near, Max( Max( far0, far1 ), Max( far2, far3 )) );

		return	AABB_Create( min, max );
	}


	void  ProjFrustumToGround (float4x4 invVP, out float4 outScaleBias, out float2 outCorners[4], out float2 outShadowCorners[4])
	{
		// in world space coords
		float2		near0		= UnProjectNDC( invVP, float3(-1.0, 0.0, 0.0) ).xz;
		float2		near1		= UnProjectNDC( invVP, float3( 1.0, 0.0, 0.0) ).xz;

		float2		far0		= UnProjectNDC( invVP, float3(-1.0, 0.0, 1.0) ).xz;
		float2		far1		= UnProjectNDC( invVP, float3( 1.0, 0.0, 1.0) ).xz;

		float2		min			= Min( Min( near0, near1 ), Min( far0, far1 ));
		float2		max			= Max( Max( near0, near1 ), Max( far0, far1 ));

		float2		fr_center	= Average( Average( far0, far1 ), Average( near0, near1 ));

		float2		inv_size	= 1.0 / (max - min);
		float		scale		= 1.0 / (iZRange.y * 1.5);

		outScaleBias	= float4( 1.0/scale, 1.0/scale, fr_center );

		// in 2D screen coords
		outCorners[0]	= (near0 - fr_center) * scale;
		outCorners[1]	= (near1 - fr_center) * scale;
		outCorners[2]	= (far0  - fr_center) * scale;
		outCorners[3]	= (far1  - fr_center) * scale;

		outShadowCorners[0]	= (float2(min.x, min.y) - fr_center) * scale;
		outShadowCorners[1]	= (float2(max.x, min.y) - fr_center) * scale;
		outShadowCorners[2]	= (float2(min.x, max.y) - fr_center) * scale;
		outShadowCorners[3]	= (float2(max.x, max.y) - fr_center) * scale;
	}


	void Main ()
	{
		float		fov			= ToRad(iFOV);
		float		rot			= iAnimation == 1 ? un_PerPass.time * 0.1 : iRotation;

		float3x3	cam_view	= f3x3_LookAt( SphericalToCartesian( float2(rot * float_Pi2, float_HalfPi) ), float3(0.0, -1.0, 0.0) );
		float4x4	cam_proj	= f4x4_Perspective( fov, 1.0, iZRange );

		float4x4	cam_vp		= cam_proj * float4x4(cam_view);
		float4x4	cam_inv_vp	= MatInverse( cam_vp );

		ProjFrustumToGround( cam_inv_vp, OUT un_Params.scaleBias, OUT un_Params.frustumCorners, OUT un_Params.shadowCorners );

		// virtual shadow map with camera in center
		// virtual size:		shadow_dist*2
		// max visible size:	max_size
		const float	shadow_dist = iZRange.y / Cos( fov * 0.5 );
		const float	max_size	= iZRange.y * Max( Tan( fov * 0.5 ), 1.0 ) * 2.0;
		{
			// world space to 2D screen
			un_Params.virtualShadowCorners[0] = (float2(-shadow_dist, -shadow_dist) - un_Params.scaleBias.zw) / un_Params.scaleBias.xy;
			un_Params.virtualShadowCorners[1] = (float2( shadow_dist, -shadow_dist) - un_Params.scaleBias.zw) / un_Params.scaleBias.xy;
			un_Params.virtualShadowCorners[2] = (float2(-shadow_dist,  shadow_dist) - un_Params.scaleBias.zw) / un_Params.scaleBias.xy;
			un_Params.virtualShadowCorners[3] = (float2( shadow_dist,  shadow_dist) - un_Params.scaleBias.zw) / un_Params.scaleBias.xy;
		}

		float3		light_ang	= iLightDir * float_HalfPi + float3(0.5, 1.0, 0.0) * float_Pi;
		float3x3	light_view	= f3x3_RotateX( light_ang.x ) * f3x3_RotateY( light_ang.y ) * f3x3_RotateZ( light_ang.z );

		AABB		aabb		= CalcShadowBBox( light_view, cam_proj, cam_inv_vp, iZRange.y );	// light space

		float2		z_range		= float2( aabb.min.z, aabb.max.z );
		float2		center		= AABB_Center( aabb ).xy;
		float2		size		= AABB_Size( aabb ).xy;

		if ( iFixedSMSize == 0 )
			size = Min( float2(max_size), size );
		else
			size = float2(max_size);

		float3		view_pos	= light_view * float3(0.0);
					center		= SnapToTexel( view_pos.xy + center, size );

		un_Params.shadowVP = f4x4_Ortho( Rect_FromCenterSize( center, size ), z_range ) * float4x4(light_view);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Matrix.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"
	#include "TexSampling.glsl"


	void  DrawLine (float2 uv, float2 p0, float2 p1, float3 color, float width)
	{
		float	d	= SDF2_Line( uv, p0, p1 ) - width;
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		out_Color.rgb = Lerp( out_Color.rgb, color, f );
	}

	void  DrawRect (float2 uv, float2 corners[4], float3 col, float w)
	{
		DrawLine( uv, corners[0], corners[1], col, w );
		DrawLine( uv, corners[2], corners[3], col, w );
		DrawLine( uv, corners[0], corners[2], col, w );
		DrawLine( uv, corners[1], corners[3], col, w );
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected() / iUVScale;

		out_Color = float2(0.25, 0.0).rrrg;

		// draw shadow AABB
		{
			float3	world_pos	= float3( (uv * un_Params.scaleBias.xy) + un_Params.scaleBias.zw, 0.0 ).xzy;
			float4	sc			= ProjectShadow( un_Params.shadowVP, world_pos );
					sc.z		= 0.5;

			if ( SampleShadow_IsValidCoord( sc ))
				out_Color = float4(0.0, 0.6, 0.0, 1.0);
		}

		// draw virtual shadow map
		{
			const float		w		= 0.010 * InvSqrt(iUVScale);
			const float3	col		= float3(0.2, 0.2, 0.7);
			DrawRect( uv, un_Params.virtualShadowCorners, col, w );
		}

		// draw shadow map
		{
			const float		w		= 0.011 * InvSqrt(iUVScale);
			const float3	col		= float3(0.6, 0.6, 0.1);
			DrawRect( uv, un_Params.shadowCorners, col, w );
		}

		// draw projected frustum
		{
			const float		w		= 0.012 * InvSqrt(iUVScale);
			const float3	col_e	= float3(0.7, 0.7, 0.7);
			const float3	col_f	= float3(0.8, 0.2, 0.2);

			DrawRect( uv, un_Params.frustumCorners, col_e, w );
			DrawLine( uv, un_Params.frustumCorners[2], un_Params.frustumCorners[3], col_f, w );	// far plane (red)
		}

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
