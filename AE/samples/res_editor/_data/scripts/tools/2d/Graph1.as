// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Another easing functions.

	top:    1
	bottom: 0
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define BACKGROUND
#	define MODE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode1		= DynamicUInt();
		RC<DynamicUInt>		mode2		= DynamicUInt();
		RC<DynamicUInt>		flip		= DynamicUInt();
		RC<DynamicFloat4>	color1		= DynamicFloat4( float4( 1.0, 0.0, 0.0, 1.0 ));
		RC<DynamicFloat4>	color2		= DynamicFloat4( float4( 0.0, 1.0, 0.0, 1.0 ));
		const array<string>	mode_str	= {
			"LOG2", "LN", "SMOOTHSTEP", "ONE_DIV_X", "SIN", "EXP",
			"SRGB", "QUADRATIC", "CUBIC", "ONE_DIV_SQUARE_X",
			"ASIN", "SQRT", "CBRT", "QUARTIC", "EXP2", "INVSQRT"
		};

		Slider( mode1,	"Red",		0,	mode_str.size()-1,	0 );
		Slider( mode2,	"Green",	0,	mode_str.size(),	0 );
		Slider( flip,	"Flip",		0,	1 );

		// render loop
		{
			RC<Postprocess>	pass = Postprocess( "", "BACKGROUND" );
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
		}

		// red graph
		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.SetDebugLabel( "R:"+mode_str[i], RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Constant( "iFlip",		flip );
			pass.Constant( "iColor",	color1 );
			pass.EnableIfEqual( mode1, i );
		}

		// green graph
		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.SetDebugLabel( "G:"+mode_str[i], RGBA8u(0, 255, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Constant( "iFlip",		flip );
			pass.Constant( "iColor",	color2 );
			pass.EnableIfEqual( mode2, i+1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	float2  GetUV (int dx)
	{
		float2	uv = MapPixCoordToSNormCorrected2( float2(GetGlobalCoord().xy + int2(dx, 0)), float2(GetGlobalSize().xy) );
		uv.y = -uv.y;
		uv *= 0.5;
		uv += 0.5;
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

			float	w = 8.f / MaxOf(GetGlobalSize().xy);

			// diagonal
			float	d = Abs( uv.x - uv.y ) * 2.0;
			if ( d < w )
				out_Color.rgb *= SmoothStep( d / w, 0.0, 1.0 );

			out_Color.rgb *= AA_QuadGrid( uv * 100.0, float2(0.04), 0.4 );
			out_Color.rgb *= AA_QuadGrid( uv * 400.0, float2(0.04), 0.5 );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	#include "Easing.glsl"
	#include "Geometry.glsl"
	#include "ColorSpace.glsl"

	#define SMOOTHSTEP			0
	#define ONE_DIV_X			1
	#define ONE_DIV_SQUARE_X	2
	#define SQRT				3
	#define CBRT				4
	#define QUADRATIC			5
	#define LN					6
	#define CUBIC				7
	#define QUARTIC				8
	#define LOG2				9
	#define EXP					10
	#define SIN					11
	#define EXP2				12
	#define SRGB				13
	#define INVSQRT				14
	#define ASIN				15


	float  Graph2 (float x)
	{
		#if MODE == SMOOTHSTEP
			return HermiteEaseIn( x );

		#elif MODE == ONE_DIV_X
			return ReciprocalEaseIn( x );

		#elif MODE == ONE_DIV_SQUARE_X
			return ReciprocalSquaredEaseIn( x );

		#elif MODE == SQRT
			return SquareRootEaseIn( x );

		#elif MODE == CBRT
			return CubicRootEaseIn( x );

		#elif MODE == QUADRATIC
			return QuadraticEaseIn( x );

		#elif MODE == CUBIC
			return CubicEaseIn( x );

		#elif MODE == QUARTIC
			return QuarticEaseIn( x );

		#elif MODE == LN
			return LogarithmicEaseIn( x );

		#elif MODE == LOG2
			return Logarithmic2EaseIn( x );

		#elif MODE == EXP
			return ExponentialE_EaseIn( x );

		#elif MODE == EXP2
			return ExponentialEaseIn( x );

		#elif MODE == SIN
			return SineEaseIn( x );

		#elif MODE == SRGB
			return RemoveSRGBCurve( x );

		#elif MODE == INVSQRT
			return (InvSqrt( 1.008 - x ) - 1.0) * 0.1;

		#elif MODE == ASIN
			return ASin( x ) / (float_Pi * 0.5);

		#else
		#	error unsupported MODE
		#endif
	}


	float  Graph (float x)
	{
		if ( iFlip == 1 )
			x = 1.0 - x;

		x = Graph2( Saturate( x ));

		if ( iFlip == 1 )
			x = 1.0 - x;

		return x;
	}

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

		out_Color = float4(iColor.rgb, 0.0);

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
