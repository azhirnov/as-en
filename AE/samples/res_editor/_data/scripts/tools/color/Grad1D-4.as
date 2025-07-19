// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MAIN_PASS
#	define PALETTE_PASS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	fmt		= EPixelFormat::RGBA8_UNorm;
		RC<Image>			rt		= Image( fmt, SurfaceSize() );
		RC<Image>			pal		= Image( fmt, uint2(24,1) );

		const float			min		= -2.f;
		const float			max		= 2.f;
		const array<float>	params	= {
			-1.0000f,	0.6030f,	0.9530f,	0.6030f, 	0.2270f,	-0.4230f,	0.3830f, 	// Red
			-1.0000f,	-0.0270f,	0.7170f,	0.7740f, 	0.2830f,	-0.0040f,	0.2880f, 	// Green
			-0.2990f,	-0.0540f,	0.2540f,	0.9180f, 	0.8570f,	0.2230f,	-1.0000f, 	// Blue
			-0.2990f,	2.0000f,	2.0000f,	2.0000f, 	2.0000f,	2.0000f,	2.0000f, 	// Alpha
		};

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "MAIN_PASS" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Palette",	pal,	Sampler_LinearClamp );

			pass.Slider( "iRed",		float4(min),	float4(max),	float4( params[0],  params[1],  params[2], params[3] ));
			pass.Slider( "jRed",		float3(min),	float3(max),	float3( params[4],  params[5],  params[6] ));

			pass.Slider( "iGreen",		float4(min),	float4(max),	float4( params[7],  params[8],  params[9], params[10] ));
			pass.Slider( "jGreen",		float3(min),	float3(max),	float3( params[11], params[12], params[13] ));

			pass.Slider( "iBlue",		float4(min),	float4(max),	float4( params[14], params[15], params[16], params[17] ));
			pass.Slider( "jBlue",		float3(min),	float3(max),	float3( params[18], params[19], params[20] ));

			pass.Slider( "iAlpha",		float4(min),	float4(max),	float4( params[21], params[22], params[23], params[24] ));
			pass.Slider( "jAlpha",		float3(min),	float3(max),	float3( params[25], params[26], params[27] ));

			pass.Slider( "iMode",		0,	2 );
			pass.Slider( "iPreview",	0,	1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "PALETTE_PASS" );
			pass.Output( "out_Color",	pal );
			pass.ArgIn(  "un_RT",		rt,		Sampler_NearestClamp );
		}
		Present( rt );

		Export( pal, "gradient1d4-.aeimg" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MAIN_PASS
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "tools/SplineHelper.glsl"

	float  Wave (float x)
	{
		return TriangleWave( x * 100.0 ) * 1.5;
	}

	void Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
				uv.y = 1.0 - uv.y;

		float4	col = float4(0.0);
		col.r	= ApplySpline( uv.x,  iMode, iRed,   jRed   ).x;
		col.g	= ApplySpline( uv.x,  iMode, iGreen, jGreen ).x;
		col.b	= ApplySpline( uv.x,  iMode, iBlue,  jBlue  ).x;
		col.a	= ApplySpline( uv.x,  iMode, iAlpha, jAlpha ).x;

		if ( iPreview == 0 )
		{
			float	x2 = GetGlobalCoordUNorm( int3(1) ).x;
			float4	c2;
			c2.r = ApplySpline( x2,  iMode, iRed,   jRed   ).x;
			c2.g = ApplySpline( x2,  iMode, iGreen, jGreen ).x;
			c2.b = ApplySpline( x2,  iMode, iBlue,  jBlue  ).x;
			c2.a = ApplySpline( x2,  iMode, iAlpha, jAlpha ).x;

			float4	d;
			d.r = SDF2_Line( uv, float2(uv.x,col.r), float2(x2,c2.r) );
			d.g = SDF2_Line( uv, float2(uv.x,col.g), float2(x2,c2.g) );
			d.b = SDF2_Line( uv, float2(uv.x,col.b), float2(x2,c2.b) );
			d.a = SDF2_Line( uv, float2(uv.x,col.a), float2(x2,c2.a) );

			const float	iWidth = 0.001f;
			if ( d.r < iWidth )	col.rgb = float3( 1.0, 0.0, 0.0 ) * Wave( uv.x );
			if ( d.g < iWidth )	col.rgb = float3( 0.0, 1.0, 0.0 ) * Wave( uv.x + 1.0 );
			if ( d.b < iWidth )	col.rgb = float3( 0.4, 0.5, 1.0 ) * Wave( uv.x + 2.0 );
			if ( d.a < iWidth )	col.rgb = float3( 1.0, 1.0, 1.0 ) * Wave( uv.x + 3.0 );
		}
		else
		{
			if ( uv.y > 0.1 and uv.y < 0.2 )
				col = gl.texture.Sample( un_Palette, uv );
		}

		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PALETTE_PASS
	#include "InvocationID.glsl"

	void Main ()
	{
		int		size	= GetGlobalSize().x;
		int		ix		= GetGlobalCoord().x;
		float	x		= (float(ix) + 0.5) / float(size);

		if ( ix == 0 )		x = 0.f;
		if ( ix == size-1 )	x = 1.f;

		out_Color = gl.texture.Sample( un_RT, float2(x, 0.5) );
	}

#endif
//-----------------------------------------------------------------------------

