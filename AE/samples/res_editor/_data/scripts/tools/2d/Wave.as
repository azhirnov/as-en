// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MODE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode		= DynamicUInt();
		const array<string>	mode_str	= {
			"s_SIN", "u_SIN",
			"s_TRIANGLE", "u_TRIANGLE",
			"s_SMOOTHSTEP", "u_SMOOTHSTEP"
		};

		Slider( mode,	"Mode",		0,	mode_str.size()-1, 0 );

		// render loop
		for (uint i = 0; i < mode_str.size(); ++i)
		{
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.Output( "out_Color",	rt );
			pass.EnableIfEqual( mode, i );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	// signed
	#define s_SMOOTHSTEP		1
	#define s_TRIANGLE			2
	#define s_SIN				3

	// unsigned
	#define u_SMOOTHSTEP		10
	#define u_TRIANGLE			11
	#define u_SIN				12


	float  SmoothStepWave (float x)
	{
		x= TriangleWave( x );
		return SmoothStep( x, 0.0, 1.0 );
	}

	float  SignedTriangleWave (float x)
	{
		return TriangleWave( x ) * ToSNorm( LessF( x, 1.0 ));
	}

	float  SignedSmoothStepWave (float x)
	{
		float	y = TriangleWave( x );
		return SmoothStep( y, 0.0, 1.0 ) * ToSNorm( LessF( x, 1.0 ));
	}


	float  Graph (float x)
	{
		x = Saturate( x ) * 2.0;

		#if MODE == s_SMOOTHSTEP
			return ToUNorm( SignedSmoothStepWave( x ));

		#elif MODE == s_TRIANGLE
			return ToUNorm( SignedTriangleWave( x ));

		#elif MODE == s_SIN
			return ToUNorm( Sin( x * float_Pi ));

		#elif MODE == u_SMOOTHSTEP
			return SmoothStepWave( x );

		#elif MODE == u_TRIANGLE
			return TriangleWave( x );

		#elif MODE == u_SIN
			return Abs( Sin( x * float_Pi ));

		#else
		#	error unsupported MODE
		#endif
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

		float2	p0	= float2( uv.x,  Graph( uv.x  ));
		float2	p1	= float2( uv2.x, Graph( uv2.x ));
		float	d	= SDF2_Line( uv, p0, p1 );

		out_Color = float4(0.25);

		if ( ! IsUNorm( uv ))
			out_Color.rgb *= 0.1;
		else
		{
			if ( AnyGreater( uv, float2(1.0) ))
				out_Color.rgb *= 0.8;

			float	w = 8.f / MaxOf(GetGlobalSize().xy);
			out_Color.rgb *= AA_QuadGrid( uv * 100.0, float2(0.04), 0.4 );
			out_Color.rgb *= AA_QuadGrid( uv * 400.0, float2(0.04), 0.5 );

			if ( d < w )
				out_Color = Lerp( out_Color, float4( 1.0, 0.0, 0.0, 1.0 ), SmoothStep( 1.0 - d/w, 0.5, 1.0 ));
		}
	}

#endif
//-----------------------------------------------------------------------------
