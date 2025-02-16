// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Build curve with 1/x, sqrt, pow.

	top:    1
	bottom: 0
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define BACKGROUND
#	define POW_GRAPH
#	define RCP_GRAPH
#	define RCP_X_GRAPH
#	define RCP_X2_GRAPH
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
		RC<DynamicUInt>	inv_x		= DynamicUInt();
		RC<DynamicUInt>	inv_y		= DynamicUInt();

		Slider( red_graph,		"Red",		0,	3,	1 );
		Slider( green_graph,	"Green",	0,	2,	0 );
		Slider( blue_graph,		"Blue",		0,	1,	0 );
		Slider( inv_x,			"InvX",		0,	1 );
		Slider( inv_y,			"InvY",		0,	1 );


		// render loop
		{
			RC<Postprocess>	pass = Postprocess( "", "BACKGROUND" );
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
		}

		// red graph
		{
			const array<float>	params = {
				0.1410f, 0.0270f, 				// iA
				0.6420f, 0.1850f, 0.3110f, 		// iBB
			};
			RC<Postprocess>	pass = Postprocess( "", "RCP_GRAPH" );
			pass.SetDebugLabel( "R1:Rcp", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iA",	float2(-1.0),	float2(1.0),	float2(params[0], params[1]) );
			pass.Slider( "iBB",	float3(0.0),	float3(1.5),	float3(params[2], params[3], params[4]) );
			pass.Constant( "iInvX",	inv_x );
			pass.Constant( "iInvY",	inv_y );
			pass.EnableIfEqual( red_graph, 1 );
		}{
			const array<float>	params = {
				1.0000f, 0.4000f, 0.0000f, 1.4000f, 	// iParams
				0.0000f, 								// iParam2
			};
			RC<Postprocess>	pass = Postprocess( "", "RCP_X_GRAPH" );
			pass.SetDebugLabel( "R2:RcpX", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0),	float4(2.0),	float4(params[0], params[1], params[2], params[3]) );
			pass.Slider( "iParam2", -1.0,			2.0,			params[4] );
			pass.Constant( "iInvX",	inv_x );
			pass.Constant( "iInvY",	inv_y );
			pass.EnableIfEqual( red_graph, 2 );
		}{
			const array<float>	params = {
				1.9640f, 1.3000f, 0.6000f, 2.0360f, 	// iParams
				-0.7290f, 0.f, 0.f						// iParam2
			};
			RC<Postprocess>	pass = Postprocess( "", "RCP_X2_GRAPH" );
			pass.SetDebugLabel( "R3:RcpX2", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0),	float4(6.0),	float4(params[0], params[1], params[2], params[3]) );
			pass.Slider( "iParam2", float3(-1.0),	float3(2.0),	float3(params[4], params[5], params[6] ));
			pass.Constant( "iInvX",	inv_x );
			pass.Constant( "iInvY",	inv_y );
			pass.EnableIfEqual( red_graph, 3 );
		}


		// green graph
		{
			RC<Postprocess>	pass = Postprocess( "", "SQRT_GRAPH" );
			pass.SetDebugLabel( "G1:Sqrt", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0), float4(2.0),	float4(1.0, 0.0, 1.0, 0.0) );
			pass.Constant( "iInvX",	inv_x );
			pass.Constant( "iInvY",	inv_y );
			pass.EnableIfEqual( green_graph, 1 );
		}{
			RC<Postprocess>	pass = Postprocess( "", "CBRT_GRAPH" );
			pass.SetDebugLabel( "G2:Cbrt", RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iParams",	float4(0.0), float4(2.0),	float4(1.0, 0.0, 1.0, 0.0) );
			pass.Constant( "iInvX",	inv_x );
			pass.Constant( "iInvY",	inv_y );
			pass.EnableIfEqual( green_graph, 2 );
		}

		// blue graph
		{
			RC<Postprocess>	pass = Postprocess( "", "POW_GRAPH" );
			pass.SetDebugLabel( "B1:Pow", RGBA8u(0, 0, 255, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Slider( "iPow",		1.f,	10.f,	1.f );
			pass.Slider( "iXOffset",	0.f,	1.f,	0.f );
			pass.Slider( "iYOffset",	-0.1f,	0.1f,	0.f );
			pass.Constant( "iInvX",		inv_x );
			pass.Constant( "iInvY",		inv_y );
			pass.EnableIfEqual( blue_graph, 1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Easing.glsl"
	#include "InvocationID.glsl"

	float2  GetUV (int dx)
	{
		float2	uv = MapPixCoordToSNormCorrected2( float2(GetGlobalCoord().xy + int2(dx, 0)), float2(GetGlobalSize().xy) );
		uv.y = -uv.y;
		uv += float2( 1.4, 1.0 );
		return uv;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BACKGROUND

	void Main ()
	{
		float2	uv	= GetUV( 0 );

		out_Color = float4(0.25);

		if ( AnyLess( uv, float2(0.0) ))
			out_Color.rgb *= 0.1;
		else
		{
			if ( AnyGreater( uv, float2(1.0) ))
				out_Color.rgb *= 0.8;

			float	w = 16.f / MaxOf(GetGlobalSize().xy);

			// diagonal
			float	d = Abs( uv.x - uv.y ) * 2.0;
			if ( d < w )
				out_Color.rgb *= SmoothStep( d / w, 0.0, 1.0 );

			out_Color.rgb *= AA_QuadGrid( uv * 100.0, float2(0.04), 0.8 );
			out_Color.rgb *= AA_QuadGrid( uv * 400.0, float2(0.04), 1.0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
#if defined(RCP_GRAPH) or defined(RCP_X2_GRAPH) or defined(RCP_X_GRAPH)
	const float4	c_Color = float4(1.0, 0.0, 0.0, 0.0);
#endif
#ifdef RCP_GRAPH

	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		float2	p_a = iA * float2(1.0, 0.01);
		float3	p_b = iBB;

		float	a = x * (x + p_a.x) - p_a.y;
		float	b = x * (p_b.x * x + p_b.y) + p_b.z;
		x = a / b;

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}

#endif
#ifdef RCP_X_GRAPH

	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		x = (x * iParams.w + iParam2) / (x * iParams.x + iParams.y);
		x += iParams.z;

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}

#endif
#ifdef RCP_X2_GRAPH

	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		x += iParam2.y;
		x = (x * iParams.w + iParam2.x) / (x * iParams.x + iParams.y);
		x += iParams.z;
		x *= x;
		x += iParam2.z;

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef POW_GRAPH

	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		x += iXOffset;
		x = Pow( x, 1.0/iPow );
		x += iYOffset;

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}

	const float4	c_Color = float4(0.0, 0.5, 1.0, 0.0);

#endif
//-----------------------------------------------------------------------------
#if defined(SQRT_GRAPH) or defined(CBRT_GRAPH)
	const float4	c_Color = float4(0.0, 1.0, 0.0, 0.0);
#endif
#ifdef SQRT_GRAPH

	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		x = SquareRootEaseOut( x * iParams.x + iParams.y ) * iParams.z - iParams.w;

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}

#endif
#ifdef CBRT_GRAPH

	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		x = CubicRootEaseOut( x * iParams.x + iParams.y ) * iParams.z - iParams.w;

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}

#endif
//-----------------------------------------------------------------------------
#if defined(RCP_GRAPH) or defined(RCP_X2_GRAPH) or defined(RCP_X_GRAPH) or defined(POW_GRAPH) or defined(SQRT_GRAPH) or defined(CBRT_GRAPH)

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

		if ( AllGreater( uv, float2(0.0) ))
		{
			float	w = 16.f / MaxOf(GetGlobalSize().xy);

			// graph
			if ( d < w )
				out_Color.a = SmoothStep( 1.0 - d/w, 0.5, 1.0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
