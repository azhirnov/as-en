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

		const array<float>	params = {
			3, 										// iMode
			2, 										// iEase
			0.5911f, 0.0000f, 0.8039f, 1.0000f, 	// iColor0
			0.0000f, 0.0294f, 1.0000f, 1.0000f, 	// iColor1
			0.0045f, 0.8722f, 0.9265f, 1.0000f, 	// iColor2
			0.0000f, 0.9020f, 0.1061f, 1.0000f, 	// iColor3
			0.8659f, 0.8922f, 0.0000f, 1.0000f, 	// iColor4
			0.9461f, 0.5009f, 0.0000f, 1.0000f, 	// iColor5
			0.9461f, 0.0000f, 0.0000f, 1.0000f, 	// iColor6
			1.0000f, 1.0000f, 1.0000f, 1.0000f, 	// iColor7

		};

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "MAIN_PASS" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Palette",	pal,	Sampler_LinearClamp );

			pass.Slider( "iMode",		0,	3,	int(params[0]) );
			pass.Slider( "iEase",		0,	5,	int(params[1]) );
			pass.Slider( "iPreview",	0,	1 );
			pass.Slider( "iColorMask",	0,	4 );

			uint	i = 2;
			pass.ColorSelector( "iColor0",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor1",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor2",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor3",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor4",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor5",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor6",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.ColorSelector( "iColor7",	RGBA32f( params[i+0], params[i+1], params[i+2], params[i+3] ));	i += 4;
		}{
			RC<Postprocess>		pass = Postprocess( "", "PALETTE_PASS" );
			pass.Output( "out_Color",	pal );
			pass.ArgIn(  "un_RT",		rt,		Sampler_NearestClamp );
		}
		Present( rt );

		Export( pal, "gradient1d1-.aeimg" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MAIN_PASS
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Easing.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	struct Result
	{
		float4	col;
		float	x;
	};

	float  Wave (float x)
	{
		x = TriangleWave( x * 100.0 );
		return Sqrt(x);
	}

	float  ApplyEasing (float x)
	{
		switch ( iEase )
		{
			case 0 :	return x;	// linear
			case 1 :	return HermiteEaseInOut( x );
			case 2 :	return QuadraticEaseInOut( x );
			case 3 :	return CubicEaseInOut( x );
			case 4 :	return QuarticEaseInOut( x );
			case 5 :	return QuinticEaseInOut( x );
		}
	}

	float4  MyLerp (float4 a, float4 b, float factor)
	{
		return Lerp( a, b, ApplyEasing( factor ));
	}

	float4  MyRGBLerpHSV (float4 a, float4 b, float factor)
	{
		return RGBLerpHSV( a, b, ApplyEasing( factor ));
	}

	float4  MyRGBLerpOklab (float4 a, float4 b, float factor)
	{
		return RGBLerpOklab( a, b, ApplyEasing( factor ));
	}

	float4  MySRGBLerp (float4 a, float4 b, float factor)
	{
		a = RemoveSRGBCurve( a );
		b = RemoveSRGBCurve( b );
		return ApplySRGBCurve( Lerp( a, b, ApplyEasing( factor )));
	}

	Result  GetColor (int dx)
	{
		const float4	arr [] = {
			iColor0,
			iColor1,
			iColor2,
			iColor3,
			iColor4,
			iColor5,
			iColor6,
			iColor7
		};

		Result	res;
		res.x = GetGlobalCoordUNorm( int3(dx) ).x;
		switch ( iMode )
		{
			case 0 :	LinearSampleArray2( OUT res.col, arr, res.x, MyLerp );			break;
			case 1 :	LinearSampleArray2( OUT res.col, arr, res.x, MyRGBLerpHSV );	break;
			case 2 :	LinearSampleArray2( OUT res.col, arr, res.x, MyRGBLerpOklab );	break;
			case 3 :	LinearSampleArray2( OUT res.col, arr, res.x, MySRGBLerp );		break;
		}
		res.col = Saturate( res.col );
		return res;
	}

	bool  EnableColor (Result p0, Result p1, float2 uv, uint id)
	{
		if ( iColorMask == 0 or iColorMask == id+1 )
		{
			float	d = SDF2_Line( uv, float2(p0.x, p0.col[id]), float2(p1.x, p1.col[id]) );
			return d < 0.0015f;
		}
		return false;
	}

	void Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
		uv.y = 1.0 - uv.y;

		Result	p0	= GetColor( 0 );
		float4	col	= p0.col;

		if ( iPreview == 0 )
		{
			Result	p1 = GetColor( 1 );

			if ( EnableColor( p0, p1, uv, 0 ))	col.rgb = float3( 1.0, 0.0, 0.0 ) * Wave( uv.x );
			if ( EnableColor( p0, p1, uv, 1 ))	col.rgb = float3( 0.0, 1.0, 0.0 ) * Wave( uv.x + 1.0 );
			if ( EnableColor( p0, p1, uv, 2 ))	col.rgb = float3( 0.4, 0.5, 1.0 ) * Wave( uv.x + 2.0 );
			if ( EnableColor( p0, p1, uv, 3 ))	col.rgb = float3( 1.0, 1.0, 1.0 ) * Wave( uv.x + 3.0 );

			col.rgb *= AA_QuadGrid_dxdy( uv*8.0, float2(0.0, 2.0) ).x;
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

