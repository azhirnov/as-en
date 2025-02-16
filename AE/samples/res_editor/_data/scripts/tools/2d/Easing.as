// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	top:    1
	bottom: 0
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define BACKGROUND
#	define MODE		Quadratic
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode1		= DynamicUInt();
		RC<DynamicUInt>		mode2		= DynamicUInt();
		RC<DynamicUInt>		submode		= DynamicUInt();
		RC<DynamicUInt>		inv_x		= DynamicUInt();
		RC<DynamicUInt>		inv_y		= DynamicUInt();
		RC<DynamicFloat4>	color1		= DynamicFloat4( float4( 1.0, 0.0, 0.0, 1.0 ));
		RC<DynamicFloat4>	color2		= DynamicFloat4( float4( 0.0, 1.0, 0.0, 1.0 ));
		const array<string>	mode_str	= {
			"Sine", "Hermite", "Quadratic", "Cubic", "Quartic", "Quintic", "Exponential", "Circular", "Elastic"
		};

		Slider( mode1,		"Red",		0,	mode_str.size()-1,	0 );
		Slider( mode2,		"Green",	0,	mode_str.size(),	0 );
		Slider( submode,	"InOut",	0,	2,					0 );
		Slider( inv_x,		"InvX",		0,	1 );
		Slider( inv_y,		"InvY",		0,	1 );

		// render loop
		{
			RC<Postprocess>	pass = Postprocess( "", "BACKGROUND" );
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
		}

		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.SetDebugLabel( "R:"+mode_str[i], RGBA8u(255, 0, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Constant( "iInOut",	submode );
			pass.Constant( "iColor",	color1 );
			pass.Constant( "iInvX",		inv_x );
			pass.Constant( "iInvY",		inv_y );
			pass.EnableIfEqual( mode1, i );
		}
		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.SetDebugLabel( "G:"+mode_str[i], RGBA8u(0, 255, 0, 255) );
			pass.OutputBlend( "out_Color",	rt,	EBlendFactor::SrcAlpha, EBlendFactor::One, EBlendOp::Add );
			pass.Constant( "iInOut",	submode );
			pass.Constant( "iColor",	color2 );
			pass.Constant( "iInvX",		inv_x );
			pass.Constant( "iInvY",		inv_y );
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

	#define GRAPH_FN( _name_ )									\
		float  Graph2 (float x)									\
		{														\
			x = Saturate( x );									\
			switch ( iInOut ) {									\
				case 0 :	return _name_ ## EaseIn( x );		\
				case 1 :	return _name_ ## EaseOut( x );		\
				case 2 :	return _name_ ## EaseInOut( x );	\
			}													\
			return -1.f;										\
		}
	#define GRAPH_FN2( _name_ )\
		GRAPH_FN( _name_ )

	GRAPH_FN2( MODE )


	float  Graph (float x)
	{
		if ( iInvX == 1 )
			x = 1.0 - x;

		x = Graph2( x );

		if ( iInvY == 1 )
			x = 1.0 - x;

		return x;
	}


	void Main ()
	{
		float2	uv	= GetUV( 0 );
		float2	uv2	= GetUV( 1 );

		float2	p0	= float2( uv.x,  Graph( uv.x  ));
		float2	p1	= float2( uv2.x, Graph( uv2.x ));
		float	d	= SDF2_Line( uv, p0, p1 );

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
