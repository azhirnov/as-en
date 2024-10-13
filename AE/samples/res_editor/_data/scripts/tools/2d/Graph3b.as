// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	top:    1
	bottom: 0
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define BACKGROUND
#	define POW_GRAPH
#	define RCP_GRAPH
#	define RCP2_GRAPH
#	define SQRT_GRAPH
#	define CBRT_GRAPH
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>	red_graph	= DynamicUInt();
		RC<DynamicUInt>	green_graph	= DynamicUInt();
		RC<DynamicUInt>	blue_graph	= DynamicUInt();

		Slider( red_graph,		"Red",		0,	2,	1 );
		Slider( green_graph,	"Green",	0,	2,	0 );
		Slider( blue_graph,		"Blue",		0,	1,	0 );


		// render loop
		{
			RC<Postprocess>	pass = Postprocess( "", "BACKGROUND" );
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
		}
		
		// red graph
		{
			RC<Postprocess>	pass = Postprocess( "", "RCP_GRAPH" );
			pass.SetDebugLabel( "R:Rcp", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0), float4(2.0),	float4(0.5, 1.0, 1.0, 1.0) );
			pass.EnableIfEqual( red_graph, 1 );
		}{
			RC<Postprocess>	pass = Postprocess( "", "RCP2_GRAPH" );
			pass.SetDebugLabel( "R:Rcp2", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0), float4(2.0),	float4(0.5, 1.0, 1.0, 1.0) );
			pass.EnableIfEqual( red_graph, 2 );
		}
		
		// green graph
		{
			RC<Postprocess>	pass = Postprocess( "", "SQRT_GRAPH" );
			pass.SetDebugLabel( "G:Sqrt", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0), float4(2.0),	float4(1.0, 0.0, 1.0, 0.0) );
			pass.EnableIfEqual( green_graph, 1 );
		}{
			RC<Postprocess>	pass = Postprocess( "", "CBRT_GRAPH" );
			pass.SetDebugLabel( "G:Cbrt", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0), float4(2.0),	float4(1.0, 0.0, 1.0, 0.0) );
			pass.EnableIfEqual( green_graph, 2 );
		}

		// blue graph
		{
			RC<Postprocess>	pass = Postprocess( "", "POW_GRAPH" );
			pass.SetDebugLabel( "B:Pow", RGBA8u(0, 0, 255, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iPow",		1.f,	10.f,	1.f );
			pass.Slider( "iXOffset",	0.f,	1.f,	0.f );
			pass.Slider( "iYOffset",	-0.1f,	0.1f,	0.f );
			pass.EnableIfEqual( blue_graph, 1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Easing.glsl"
	#include "GlobalIndex.glsl"

	float2  GetUV (int dx)
	{
		float2	uv = MapPixCoordToSNormCorrected2( float2(GetGlobalCoord().xy + int2(dx, 0)), float2(GetGlobalSize().xy) );
		uv.y = -uv.y;
		uv *= 0.5;
		uv += 0.5;
		return uv;
	}

	float  XTransform (float x)
	{
	//	return 1.0 - Abs(ToSNorm( x ));
		return Abs(ToSNorm( x ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BACKGROUND

	void Main ()
	{
		float2	uv	= GetUV( 0 );

		out_Color = float4(0.25);

		if ( AnyLess( uv, float2(0.0) ) or AnyGreater( uv, float2(1.0) ))
			out_Color.rgb *= 0.8;

		float	w = 8.f / MaxOf(GetGlobalSize().xy);

		// diagonal
		float	d = Abs( XTransform( uv.x ) - uv.y ) * 2.0;
		if ( d < w )
			out_Color.rgb *= SmoothStep( d / w, 0.0, 1.0 );

		out_Color.rgb *= AA_QuadGrid( uv * 100.0, float2(0.04), 0.4 );
		out_Color.rgb *= AA_QuadGrid( uv * 400.0, float2(0.04), 0.5 );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(RCP_GRAPH) || defined(RCP2_GRAPH)
	const float4	c_Color = float4(1.0, 0.0, 0.0, 0.0);
#endif
#ifdef RCP_GRAPH

	float  Graph (float x)
	{
		x = XTransform( x );
		x = iParams.w / (-x * iParams.x + iParams.y) - iParams.z;
		return x;
	}

#endif
#ifdef RCP2_GRAPH

	float  Graph (float x)
	{
		x = XTransform( x );
		x = iParams.w / (-(x*x) * iParams.x + iParams.y) - iParams.z;
		return x;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef POW_GRAPH
	
	float  Graph (float x)
	{
		x = XTransform( x );
		x += iXOffset;
		x = Pow( x, iPow );
		x += iYOffset;
		return x;
	}
	
	const float4	c_Color = float4(0.0, 0.5, 1.0, 0.0);

#endif
//-----------------------------------------------------------------------------
#if defined(SQRT_GRAPH) || defined(CBRT_GRAPH)
	const float4	c_Color = float4(0.0, 1.0, 0.0, 0.0);
#endif
#ifdef SQRT_GRAPH

	float  Graph (float x)
	{
		x = XTransform( x );
		x = SquareRootEaseIn( x * iParams.x + iParams.y ) * iParams.z - iParams.w;
		return x;
	}
	
#endif
#ifdef CBRT_GRAPH
	
	float  Graph (float x)
	{
		x = XTransform( x );
		x = CubicRootEaseIn( x * iParams.x + iParams.y ) * iParams.z - iParams.w;
		return x;
	}

#endif
//-----------------------------------------------------------------------------
#if defined(RCP_GRAPH) || defined(RCP2_GRAPH) || defined(POW_GRAPH) || defined(SQRT_GRAPH) || defined(CBRT_GRAPH)

	void Main ()
	{
		float2	uv	= GetUV( 0 );
		float2	uv2	= GetUV( 1 );
		
		float	d;
		{
			float2	p0	= float2( uv.x,  Graph( uv.x  ));
			float2	p1	= float2( uv2.x, Graph( uv2.x ));
			d = SDF2_Line( uv, p0, p1 );
		}

		out_Color = c_Color;
		
		if ( IsUNorm( uv ))
		{
			float	w = 8.f / MaxOf(GetGlobalSize().xy);
			
			// graph
			if ( d < w )
				out_Color.a = SmoothStep( 1.0 - d/w, 0.5, 1.0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
