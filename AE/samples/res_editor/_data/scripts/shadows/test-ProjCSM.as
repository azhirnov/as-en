// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*

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
		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>	buf				= Buffer();
		const int	max_cascades	= 8;

		buf.UseLayout(
			"ParamsBuffer",
			"	float2	corners [4];"
			"	float2	cascades ["+max_cascades+"][4];"
			"	int		cascadeCount;"
		);

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut( "un_Params",		buf );
			pass.Slider( "iFOV",			10.0,				120.0,					80.0 );
			pass.Slider( "iZRange",			float2(0.1, 10.0),	float2(1.0, 1000.0),	float2(0.1, 100.0) );
			pass.Slider( "iNumCascades",	1,					max_cascades-1,			3 );
			pass.Slider( "iSliceMode",		0,					2,						2 );
			pass.Slider( "iSliceLambda",	0.0,				1.0,					0.5 );
			pass.Constant( "iResolution",	rt.Dimension() );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",		rt,		RGBA32f(0.0) );
			pass.ArgIn(  "un_Params",		buf );
			pass.Slider( "iUVScale",		0.5,	2.0,	1.0  );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Matrix.glsl"
	#include "Sphere.glsl"
	#include "Geometry2D.glsl"


	Rect  GetGlobalRectSNormCorrected ()
	{
		float2	hsize		= float2(iResolution) * 0.5f;
		float	inv_scale	= Rcp( Max( hsize.x, hsize.y ));
		float2	pos			= hsize * inv_scale;
		return	Rect_Create( -pos, pos );
	}


	float  DepthSlice (float4x4 proj, int cascade)
	{
		switch ( iSliceMode )
		{
			case 0 :	// linear
			{
				float	view_z = Lerp( iZRange.x, iZRange.y, float(cascade) / float(iNumCascades) );
				return	FastProjectZ( proj, view_z );
			}
			case 1 :	// log
			{
				float	view_z = iZRange.x * Pow( iZRange.y / iZRange.x,  float(cascade) / float(iNumCascades) );
				return	FastProjectZ( proj, view_z );
			}
			case 2 :	// linear + logarithmic
			{
				float	linear_z	= Lerp( iZRange.x, iZRange.y, float(cascade) / float(iNumCascades) );
				float	log_z		= iZRange.x * Pow( iZRange.y / iZRange.x,  float(cascade) / float(iNumCascades) );
				float	view_z		= Lerp( linear_z, log_z, iSliceLambda );
				return	FastProjectZ( proj, view_z );
			}
		}
	}


	void  ProjFrustumToGround (float4x4 invVP, out float4 outScaleBias, out float2 outCorners[4])
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
	}


	void  Main ()
	{
		float3x3	cam_view	= f3x3_LookAt( SphericalToCartesian( float2(0.0, float_HalfPi) ), float3(0.0, -1.0, 0.0) );
		float4x4	cam_proj	= f4x4_Perspective( ToRad(iFOV), 1.0, iZRange );

		float4x4	cam_vp		= cam_proj * float4x4(cam_view);
		float4x4	cam_inv_vp	= MatInverse( cam_vp );

		float4		scale_bias;

		ProjFrustumToGround( cam_inv_vp, OUT scale_bias, OUT un_Params.corners );

		// divide on layers (cascades)
		float	prev_z = 0.0;

		for (int c = 0; c < iNumCascades; ++c)
		{
			float	z		= DepthSlice( cam_proj, c+1 );

			float2	near0	= UnProjectNDC( cam_inv_vp, float3(-1.0, 0.0, prev_z) ).xz;
			float2	near1	= UnProjectNDC( cam_inv_vp, float3( 1.0, 0.0, prev_z) ).xz;

			float2	far0	= UnProjectNDC( cam_inv_vp, float3(-1.0, 0.0, z) ).xz;
			float2	far1	= UnProjectNDC( cam_inv_vp, float3( 1.0, 0.0, z) ).xz;

			float	hsize	= Distance( far0, far1 ) * 0.5;
			float2	nn		= Normalize( near1 - near0 );
			float2	nc		= Average( near0, near1 );

			near0	= nc - nn * hsize;
			near1	= nc + nn * hsize;

			un_Params.cascades[c][0] = (near0 - scale_bias.zw) / scale_bias.xy;
			un_Params.cascades[c][1] = (near1 - scale_bias.zw) / scale_bias.xy;
			un_Params.cascades[c][2] = (far1  - scale_bias.zw) / scale_bias.xy;
			un_Params.cascades[c][3] = (far0  - scale_bias.zw) / scale_bias.xy;

			prev_z = z;
		}

		un_Params.cascadeCount = iNumCascades;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"


	void  DrawLine (float2 uv, float2 p0, float2 p1, float3 color, float width)
	{
		float	d	= SDF2_Line( uv, p0, p1 ) - width;
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		out_Color.rgb = Lerp( out_Color.rgb, color, f );
		out_Color.a   = 1.0;
	}


	void  FillArea (const float2 uv, float2 v[4], float3 color)
	{
		if ( Quadrilateral_PointInside( v[0], v[1], v[2], v[3], uv ))
		{
			out_Color.rgb = Lerp( color, out_Color.rgb, out_Color.a );
			out_Color.a   = Saturate( out_Color.a + 0.35 );
		}
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected() / iUVScale;

		out_Color = float2(0.25, 0.0).rrrg;

		// draw cascades
		for (int c = 0; c < un_Params.cascadeCount; ++c)
		{
			FillArea( uv, un_Params.cascades[c], Rainbow( float(c) / float(un_Params.cascadeCount) ).rgb * 0.7 );
		}

		// draw projected frustum
		{
			const float		w		= 0.012 * InvSqrt(iUVScale);
			const float3	col_e	= float3(0.7, 0.7, 0.7);
			const float3	col_n	= float3(0.2, 0.8, 0.2);
			const float3	col_f	= float3(0.8, 0.2, 0.2);

			float2	p0	= un_Params.corners[0];	// near
			float2	p1	= un_Params.corners[1];	// near
			float2	p2	= un_Params.corners[2];	// far
			float2	p3	= un_Params.corners[3];	// far

			DrawLine( uv, p0, p1, col_n, w );	// near plane (green)
			DrawLine( uv, p2, p3, col_f, w );	// far plane (red)
			DrawLine( uv, p0, p2, col_e, w );	// edge
			DrawLine( uv, p1, p3, col_e, w );	// edge
		}

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
