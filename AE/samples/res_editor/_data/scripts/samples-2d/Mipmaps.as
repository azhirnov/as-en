// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Difference between mipmap generation from base level (top) and generating image for each mip level (bottom).
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_IMAGE
#	define DRAW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		const uint2		dim		= uint2(64);
		RC<Image>		img1	= Image( EPixelFormat::sRGB8_A8, dim, MipmapLevel(~0) );
		RC<Image>		img2	= Image( EPixelFormat::sRGB8_A8, dim, MipmapLevel(~0) );

		RC<DynamicFloat>	s_c_radius	= DynamicFloat();
		RC<DynamicFloat>	s_c_smooth	= DynamicFloat();
		RC<DynamicFloat>	s_l_width	= DynamicFloat();
		RC<DynamicFloat>	s_l_smooth	= DynamicFloat();
		RC<DynamicFloat>	s_r_size	= DynamicFloat();
		RC<DynamicFloat>	s_r_smooth	= DynamicFloat();
		RC<DynamicUInt>		s_cor		= DynamicUInt();
		RC<DynamicUInt>		s_details	= DynamicUInt();

		Slider( s_c_radius,	"CircleRadius",	0.01,	0.1,	0.05 );
		Slider( s_c_smooth,	"CircleSmooth",	0.0,	0.1,	0.05 );
		Slider( s_l_width,	"LineWidth",	0.01,	0.1,	0.025 );
		Slider( s_l_smooth,	"LineSmooth",	0.0,	0.1,	0.04 );
		Slider( s_r_size,	"RectSize",		0.01,	0.1,	0.05 );
		Slider( s_r_smooth,	"RectSmooth",	0.0,	0.1,	0.0 );
		Slider( s_cor,		"Correction",	0,		1,		1 );
		Slider( s_details,	"HideDetails",	0,		1,		0 );

		// render loop
		for (uint mip = 0; mip < 4; ++mip)
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_IMAGE="+mip );
			pass.Output( "out_Color",	img2,	MipmapLevel(mip) );
			pass.Constant( "iCircleRadius",	s_c_radius );
			pass.Constant( "iCircleSmooth",	s_c_smooth );
			pass.Constant( "iLineWidth",	s_l_width );
			pass.Constant( "iLineSmooth",	s_l_smooth );
			pass.Constant( "iRectSize",		s_r_size );
			pass.Constant( "iRectSmooth",	s_r_smooth );
			pass.Constant( "iCorrection",	s_cor );
			pass.Constant( "iHideDetails",	s_details );
		}
		{
			CopyImage( img2, img1 );
			GenMipmaps( img1 );
		}
		{
			RC<Postprocess>		pass = Postprocess( "", "DRAW" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn( "un_Image1",	img1,	Sampler_LinearClamp );
			pass.ArgIn( "un_Image2",	img2,	Sampler_LinearClamp );
			pass.Slider( "iScale",		1.0,	10.0,	1.0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_IMAGE
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	float  AntiAliasing (float sdf, float factor, float2 md)
	{
		if ( iCorrection == 0 )
			return SmoothStep( sdf, -factor, factor );

		// at least 1px border will be added
		factor = factor > 0.0 ? Max( factor * 40.0, 1.0 ) : 0.0;
		md *= factor;

		md = SmoothStep( float2(sdf), -md, md );
		return MinOf( md );
	}

	float  HideSmallDetails (float2 md, float size)
	{
		if ( iHideDetails == 0 )
			return 1.0;

		// details with size < 1px will be hidden
		return GreaterF( size, MinOf(md) );
	}

	void Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;
		float2	md = AA_Helper_minDist( uv );	// minimal distance for 1px

		float3	c0 = float3(1.0, 0.0, 0.0) *
					 AntiAliasing( -SDF2_Circle( uv - float2(0.25, 0.75), iCircleRadius ), iCircleSmooth, md ) *
					 HideSmallDetails( md, iCircleRadius );

		float3	c1 = float3(0.0, 1.0, 0.0) *
					 AntiAliasing( iLineWidth - SDF2_Line( uv, float2(0.0), float2(1.0) ), iLineSmooth, md ) *
					 HideSmallDetails( md, iLineWidth*4.0 );

		float3	c2 = float3(0.9, 0.8, 0.1) *
					 AntiAliasing( -SDF2_Rect( uv - float2(0.75, 0.25), float2(iRectSize) ), iRectSmooth, md ) *
					 HideSmallDetails( md, iRectSize*2.0 );

		out_Color = float4(c0 + c1 + c2, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DRAW
	#include "InvocationID.glsl"

	void Main ()
	{
		float2	pos		= GetGlobalCoordUNorm().xy;
				pos.y	= ToSNorm( pos.y ) * (un_PerPass.resolution.y*2.0 / un_PerPass.resolution.x);
				pos		*= float2(4.0, 1.0);
		float2	border	= Abs(float2( gl.dFdx( pos.x ), gl.dFdy( pos.y )));

		float2	uv		= Fract( Abs( pos ));
		float	lod		= Floor( pos.x );

		if ( pos.y < 0.0 )
		{
			// top
			uv.y = 1.0 - uv.y;
			out_Color = gl.texture.SampleLod( un_Image1, uv, lod );
		}
		else
		{
			// bottom
			out_Color = gl.texture.SampleLod( un_Image2, uv, lod );
		}

		out_Color *= iScale;
		//out_Color = float4(Fract(pos), 0.0, 0.0);

		if ( AnyLess( uv, border ))
			out_Color = float4(1.0);

		if ( Abs(pos.y) > 1.0 )
			out_Color = float4(0.25);
	}

#endif
//-----------------------------------------------------------------------------
