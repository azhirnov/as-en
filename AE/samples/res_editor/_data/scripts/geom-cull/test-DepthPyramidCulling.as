// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	AABB visibility test on depth pyramid.

	Red color inside rect - test failed.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_DEPTH
#	define GEN_MIPMAP
#	define VIEW
#	define CULL
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			depth		= Image( EPixelFormat::R32F, SurfaceSize()/4, MipmapLevel(~0) );	depth.Name( "Depth pyramid" );
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );				rt.Name( "RT" );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat2>	scale_bias	= DynamicFloat2();
		const bool			has_minmax_sampler = GetFeatureSet().hasSamplerFilterMinmax();

		Slider( mode,		"Mode",			0,				1,					1 );
		Slider( scale_bias,	"ScaleBias",	float2(0.0),	float2(10.0, 1.0),	float2(1.0, 0.0) );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_DEPTH" );
			pass.Output( "out_Color",	depth );
			pass.Slider( "iHash",		0.0,	3.0,	0.0 );
		}{
			RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP" );
			pass.Variable( "un_InImage",	"un_OutImage",	depth,	Sampler_NearestClamp );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",			rt,		RGBA32f(0.0) );
			pass.ArgIn(	 "un_DepthPyramid2",	depth,	Sampler_NearestClamp );
			pass.Slider( "iMip",				0,	9,	0 );
			pass.Constant( "iScaleBias",		scale_bias );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CULL;USE_REDUCTION="+has_minmax_sampler );
			pass.Output( "out_Color",			rt,		RGBA32f(0.0) );
			pass.ArgIn(	"un_DepthPyramid",		depth,	(has_minmax_sampler ? Sampler_MinLinearClamp : Sampler_NearestClamp) );
			pass.ArgIn(	 "un_DepthPyramid2",	depth,	Sampler_NearestClamp );
		//	pass.Slider( "iRectSize",			float2(0.01),	float2(0.5),	float2(0.070) );
			pass.Slider( "iRectSize",			0.002,			0.08,			0.070 );
			pass.Slider( "iRectPos",			float2(0.0),	float2(1.0),	float2(0.5) );
			pass.Slider( "iBias",				-0.5,			0.5,			-0.11 );
			pass.Slider( "iTexQuad",			0,				1,				1 );
			pass.Constant( "iScaleBias",		scale_bias );
			pass.EnableIfEqual( mode, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_DEPTH
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = GetGlobalCoordUNormCorrected().xy + iHash;
		out_Color.r = Saturate( -0.1 +
								DHash12( uv * 141.2 ) * 0.4 +
								DHash12( Floor(uv * 40.0) * 28.33 ) * 0.4 +
								DHash12( Floor(uv * 10.0) * 55.77 ) * 0.4 +
								DHash12( Floor(uv * 1.0) * 55.77 ) * 0.4 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "InvocationID.glsl"

	void  Main ()
	{
		float	x = gl.texture.SampleLod( un_DepthPyramid2, GetGlobalCoordUNorm().xy, iMip ).r;
		x = x * iScaleBias.x + iScaleBias.y;
		out_Color = float4(x);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MIPMAP
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4	c;
		int2	p = GetGlobalCoord().xy * 2;

		c = gl.texture.Fetch( un_InImage, p, 0 );
		c = Min( c, gl.texture.Fetch( un_InImage, p + int2(1,0), 0 ));
		c = Min( c, gl.texture.Fetch( un_InImage, p + int2(0,1), 0 ));
		c = Min( c, gl.texture.Fetch( un_InImage, p + int2(1,1), 0 ));

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	#ifndef USE_REDUCTION
	#	define USE_REDUCTION	1
	#endif

	void  Main ()
	{
		float2	dim			= float2(gl.texture.GetSize( un_DepthPyramid, 0 ));
		float2	uv			= GetGlobalCoordUNorm().xy;

		float2	hsize		= float2(iRectSize);
				hsize.y		*= un_PerPass.resolution.x * un_PerPass.invResolution.y;	// aspect correction

		float2	pos			= iRectPos;
				pos			= un_PerPass.mouse.z > 0.0 ? un_PerPass.mouse.xy : pos;

		float	level		= Ceil( Log2( MaxOf( 2.0 * hsize * dim )) + iBias );
		float2	level_dim	= float2(Max( int2(dim) >> int(level), 1 ));

		float	depth		= gl.texture.SampleLod( un_DepthPyramid2, uv, level ).r;

	  #if USE_REDUCTION
		float	min_depth	= gl.texture.SampleLod( un_DepthPyramid, pos, level ).r;
	  #else
		float	min_depth;
		{
			float2	c = pos - 0.5 / level_dim;
			min_depth =					gl.texture.SampleLodOffset( un_DepthPyramid2, c, level, int2(0,0) ).r;
			min_depth = Min( min_depth, gl.texture.SampleLodOffset( un_DepthPyramid2, c, level, int2(1,0) ).r );
			min_depth = Min( min_depth, gl.texture.SampleLodOffset( un_DepthPyramid2, c, level, int2(0,1) ).r );
			min_depth = Min( min_depth, gl.texture.SampleLodOffset( un_DepthPyramid2, c, level, int2(1,1) ).r );
		}
	  #endif

		float3	col = float3(depth * iScaleBias.x + iScaleBias.y);

		// draw texel quad
		if ( iTexQuad == 1 )
		{
		  #if USE_REDUCTION
			// Warning: texture can be sampled in 2x1 block instead of 2x2
			float2	min	= Floor( pos * level_dim - 0.5 ) / level_dim;
			float2	max	= Floor( pos * level_dim + 1.5 ) / level_dim;
		  #else
			float2	min	= Floor( pos * level_dim - 0.5 ) / level_dim;
			float2	max	= Floor( pos * level_dim + 1.5 ) / level_dim;
		  #endif

			float2	md	= AA_Helper_fwidth( uv );
			float	sd	= SDF2_Rect( uv - (min + max) * 0.5, (max - min) * 0.5 );
			float	x	= MinOf( SmoothStep( float2(Abs(sd)), md*0.5, md*2.0 ));

			if ( sd < 0.0 )
			{
				if ( min_depth > depth ){
					col = float3(0.5, 0.0, 0.5) * Saturate(col.x + 0.3);	// invalid texel area
				}else
					col = float3(0.1, 0.4, 0.8) * Saturate(col.x + 0.3);
			}
			col = Lerp( float3(0.9, 0.7, 0.0), col, x );
		}

		// draw rect
		{
			float2	md	= AA_Helper_fwidth( uv );
			float	sd	= SDF2_Rect( uv - pos, hsize );
			float	x	= MinOf( SmoothStep( float2(Abs(sd)), md*0.5, md*2.0 ));

			if ( sd < 0.0 and min_depth > depth )
			{
				col = float3(1.0, 0.0, 0.0);	// incorrect level calculation
				x = 1.0;
			}
			col = Lerp( float3(0.2, 1.0, 0.2), col, x );
		}

		out_Color.rgb = col;
		out_Color.a = depth;
	}

#endif
//-----------------------------------------------------------------------------
