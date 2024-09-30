// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define MODE		Quadratic
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		submode		= DynamicUInt();
		const array<string>	mode_str	= {
			"Sine", "Hermite", "Quadratic", "Cubic", "Quartic", "Quintic", "Exponential", "Circular", "Elastic"
		};

		Slider( mode,		"Mode",		0, mode_str.size()-1,	0 );
		Slider( submode,	"InOut",	0, 2,					0 );

		// render loop
		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.Output(	"out_Color",	rt );
			pass.Constant(	"iInOut",		submode );
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

	#define GRAPH_FN( _name_ )									\
		float  Graph (float x)									\
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

		float2	p0	= float2( uv.x,  Graph( uv.x  ));
		float2	p1	= float2( uv2.x, Graph( uv2.x ));
		float	d	= SDF2_Line( uv, p0, p1 );

		out_Color = float4(0.25);

		if ( AnyLess( uv, float2(0.0) ))
			out_Color.rgb *= 0.1;
		else
		{
			if ( AnyGreater( uv, float2(1.0) ))
				out_Color.rgb *= 0.8;

			float	w = 8.f / MaxOf(GetGlobalSize().xy);
			if ( d < w )	out_Color = Lerp( out_Color, float4( 1.0, 0.0, 0.0, 1.0 ), SmoothStep( 1.0 - d/w, 0.5, 1.0 ));

			out_Color.rgb *= AA_QuadGrid( uv * 100.0, float2(0.04), 0.25 );
		}
	}

#endif
//-----------------------------------------------------------------------------
