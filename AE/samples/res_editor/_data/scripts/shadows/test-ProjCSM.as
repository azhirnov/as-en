// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
			"	float2	cascades ["+(max_cascades*4)+"];"
			"	int		cascadeCount;"
		);

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut( "un_Params",		buf );
			pass.Slider( "iFOV",			10.0,				120.0,					80.0 );
			pass.Slider( "iRotation",		0.0,				1.0,					0.0 );
			pass.Slider( "iZRange",			float2(0.1, 10.0),	float2(1.0, 1000.0),	float2(0.1, 100.0) );
			pass.Slider( "iNumCascades",	1,					max_cascades-1,			3 );
			pass.Slider( "iSliceMode",		0,					2 );
			pass.Slider( "iOrientMode",		0,					1 );
			pass.Slider( "iCascadeRot",		0.0,				1.0,					0.0 );		// for iOrientMode==1
			pass.Constant( "iResolution",	rt.Dimension() );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",		rt,		RGBA32f(0.0) );
			pass.ArgIn(  "un_Params",		buf );
			pass.Slider( "iUVScale",		0.5,	2.0,	1.5  );
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
				float	lambda		= 0.5;
				float	linear_z	= Lerp( iZRange.x, iZRange.y, float(cascade) / float(iNumCascades) );
				float	log_z		= iZRange.x * Pow( iZRange.y / iZRange.x,  float(cascade) / float(iNumCascades) );
				float	view_z		= Lerp( linear_z, log_z, lambda );
				return	FastProjectZ( proj, view_z );
			}
		}
	}


	void  Main ()
	{
		float3x3	view	= f3x3_LookAt( SphericalToCartesian( float2(iRotation * float_Pi2, float_HalfPi) ), float3(0.0, -1.0, 0.0) );
		float4x4	proj	= f4x4_Perspective( ToRad(iFOV), 1.0, iZRange );

		float4x4	vp		= proj * float4x4(view);
		float4x4	inv_vp	= MatInverse( vp );

		float2		center;
		float		scale;

		// project camera frustum to ground
		{
			float2		near0	= UnProjectNDC( inv_vp, float3(-1.0, 0.0, 0.0) ).xz;
			float2		near1	= UnProjectNDC( inv_vp, float3( 1.0, 0.0, 0.0) ).xz;

			float2		far0	= UnProjectNDC( inv_vp, float3(-1.0, 0.0, 1.0) ).xz;
			float2		far1	= UnProjectNDC( inv_vp, float3( 1.0, 0.0, 1.0) ).xz;

			float2		min		= Min( Min( near0, near1 ), Min( far0, far1 ));
			float2		max		= Max( Max( near0, near1 ), Max( far0, far1 ));

			Rect		scr_rect	= GetGlobalRectSNormCorrected();
						center		= Average( min, max );
			float2		inv_size	= 1.0 / (max - min);
						scale		= MinOf( inv_size ) * MinOf( scr_rect.v.zw );

			un_Params.corners[0]	= (near0 - center) * scale;
			un_Params.corners[1]	= (near1 - center) * scale;
			un_Params.corners[2]	= (far0  - center) * scale;
			un_Params.corners[3]	= (far1  - center) * scale;
		}

		// divide on layers (cascades)
		float	prev_z = 0.0;

		for (int c = 0; c < iNumCascades; ++c)
		{
			float	z = DepthSlice( proj, c+1 );

			float2	near0	= UnProjectNDC( inv_vp, float3(-1.0, 0.0, prev_z) ).xz;
			float2	near1	= UnProjectNDC( inv_vp, float3( 1.0, 0.0, prev_z) ).xz;

			float2	far0	= UnProjectNDC( inv_vp, float3(-1.0, 0.0, z) ).xz;
			float2	far1	= UnProjectNDC( inv_vp, float3( 1.0, 0.0, z) ).xz;

			switch ( iOrientMode )
			{
				//       ____/
				//      |  / |
				//   ___|/   |
				//  | / |    |
				//  |*  |    |
				//  |_\_|    |
				//      |\   |
				//      |__\_|
				//           \ 
				case 0 :
				{
					float	hsize	= Distance( far0, far1 ) * 0.5;
					float2	nn		= Normalize( near1 - near0 );
					float2	nc		= Average( near0, near1 );

					near0	= nc - nn * hsize;
					near1	= nc + nn * hsize;
					break;
				}

				//   ___
				//  |*--|______
				//  | \ |'-__  |
				//  |__\|    '-|-_
				//      |\     |
				//      |  \   |
				//      |____\_|
				//              \ 
				case 1 :
				{
					float2		fc		= Average( far0, far1 );
					float2		nc		= Average( near0, near1 );
					float2		cen		= Average( nc, fc );	// center of current cascade
					float2		dir_x	= Normalize( fc - nc );
					float2		dir_y	= LeftVector( dir_x );
					float2x2	rot		= f2x2_Rotate( iCascadeRot * float_Pi * 0.25 );
								dir_x	= rot * dir_x;
								dir_y	= rot * dir_y;

					float		far0x	= Ray_MinDistance( dir_x, cen, far0 );
					float		far0y	= Ray_MinDistance( dir_y, cen, far0 );

					float		far1x	= Ray_MinDistance( dir_x, cen, far1 );
					float		far1y	= Ray_MinDistance( dir_y, cen, far1 );

					float		near0x	= Ray_MinDistance( dir_x, cen, near0 );
					float		near0y	= Ray_MinDistance( dir_y, cen, near0 );

					float		near1x	= Ray_MinDistance( dir_x, cen, near1 );
					float		near1y	= Ray_MinDistance( dir_y, cen, near1 );

					float2		size	= float2( Max( Max( far0x, far1x ), Max( near0x, near1x )),
												  Max( Max( far0y, far1y ), Max( near0y, near1y )) );

					// new points
					near0	= -dir_x * size.x  - dir_y * size.y  + cen;
					near1	= -dir_x * size.x  + dir_y * size.y  + cen;
					far0	=  dir_x * size.x  - dir_y * size.y  + cen;
					far1	=  dir_x * size.x  + dir_y * size.y  + cen;
				}
			}

			un_Params.cascades[c*4+0] = (near0 - center) * scale;
			un_Params.cascades[c*4+1] = (near1 - center) * scale;
			un_Params.cascades[c*4+2] = (far1  - center) * scale;
			un_Params.cascades[c*4+3] = (far0  - center) * scale;

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


	void  FillArea (const float2 uv, float2 v0, float2 v1, float2 v2, float2 v3, float3 color)
	{
		if ( Quadrilateral_PointInside( v0, v1, v2, v3, uv ))
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
			FillArea( uv, un_Params.cascades[c*4+0], un_Params.cascades[c*4+1],
					  un_Params.cascades[c*4+2], un_Params.cascades[c*4+3],
					  Rainbow( float(c) / float(un_Params.cascadeCount) ).rgb );
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
