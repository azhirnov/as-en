// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define MODE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		flip		= DynamicUInt();
		const array<string>	mode_str	= {
			"LOG2", "LN", "SMOOTHSTEP", "ONE_DIV_X", "SIN", "EXP",
			"SRGB", "QUADRATIC", "CUBIC", "ONE_DIV_SQUARE_X",
			"SQRT", "CBRT", "QUARTIC", "EXP2", "INVSQRT", "ASIN"
		};

		Slider( mode,	"Mode",		0,	mode_str.size()-1, 0 );
		Slider( flip,	"Flip",		0,	1 );

		// render loop
		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.Output( "out_Color",	rt );
			pass.Constant( "iFlip",		flip );
			pass.EnableIfEqual( mode, i );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Easing.glsl"
	#include "Geometry.glsl"
	#include "GlobalIndex.glsl"
	#include "ColorSpaceUtility.glsl"

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


	float2  GetUV (int dx)
	{
		float2	uv = MapPixCoordToSNormCorrected2( float2(GetGlobalCoord().xy + int2(dx, 0)), float2(GetGlobalSize().xy) );
		uv.y = -uv.y;
		uv *= 0.5;
		uv += 0.5;
		return uv;
	}

	void Main ()
	{
		float2	uv	= GetUV( 0 );
		float2	uv2	= GetUV( 1 );

		float	d0;
		{
			float2	p0	= float2( uv.x,  Graph( uv.x  ));
			float2	p1	= float2( uv2.x, Graph( uv2.x ));
			d0 = SDF2_Line( uv, p0, p1 );
		}

		out_Color = float4(0.25);

		if ( AnyLess( uv, float2(0.0) ))
			out_Color.rgb *= 0.1;
		else
		{
			if ( AnyGreater( uv, float2(1.0) ))
				out_Color.rgb *= 0.8;

			float	w = 8.f / MaxOf(GetGlobalSize().xy);

			// diagonal
			float	d2 = Abs( uv.x - uv.y ) * 2.0;
			if ( d2 < w )
				out_Color.rgb *= SmoothStep( d2 / w, 0.0, 1.0 );

			out_Color.rgb *= AA_QuadGrid( uv * 100.0, float2(0.04), 0.4 );
			out_Color.rgb *= AA_QuadGrid( uv * 400.0, float2(0.04), 0.5 );

			// graph
			if ( d0 < w )
				out_Color = Lerp( out_Color, float4( 1.0, 0.0, 0.0, 1.0 ), SmoothStep( 1.0 - d0/w, 0.5, 1.0 ));
		}
	}

#endif
//-----------------------------------------------------------------------------
