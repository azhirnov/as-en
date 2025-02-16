// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Some old GPU supports RT compression only for RGBA8_UNorm types.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define ENCODE
#	define DECODE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			packed		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			reference	= Image( EPixelFormat::R32F, SurfaceSize() );
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		cmp			= DynamicUInt();

		Slider( mode,	"Mode",		0,	4 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "ENCODE" );
			pass.Output( "out_Color0",	packed );
			pass.Output( "out_Color1",	reference );
			pass.Constant( "iMode",		mode );
		}{
			RC<Postprocess>		pass = Postprocess( "", "DECODE" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn( "un_Packed",	packed,		Sampler_NearestClamp );
			pass.ArgIn( "un_Ref",		reference,	Sampler_NearestClamp );
			pass.Constant( "iMode",		mode );
			pass.Slider( "iCmp",		0,		2 );
			pass.Slider( "iCmpScale",	0,		16,		2 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "InvocationID.glsl"


	// from http://www.gamedev.net/forums/topic/684158-rgba-to-float-percision/5321388/
	float4  EncodeV1 (float value)
	{
		int rgba = floatBitsToInt(value);
		float r = float(rgba >> 24) / 255.0;
		float g = float((rgba & 0x00ff0000) >> 16) / 255.0;
		float b = float((rgba & 0x0000ff00) >> 8) / 255.0;
		float a = float(rgba & 0x000000ff) / 255.0;
		return float4(r, g, b, a);
	}

	// range: whole
	// error: 0
	float  DecodeV1 (float4 color)
	{
		int rgba = (int(color.x * 255.0) << 24) + (int(color.y * 255.0) << 16) + (int(color.z * 255.0) << 8) + int(color.w * 255.0);
		return intBitsToFloat(rgba);
	}
	//---------------------------------


	// from https://stackoverflow.com/questions/34963366/encode-floating-point-data-in-a-rgba-texture
	float4  EncodeV2 (float value)
	{
		const float4	bitSh = float4(	256.0 * 256.0 * 256.0,
										256.0 * 256.0,
										256.0,
										1.0 );
		const float4	bitMsk = float4(0.0,
										1.0 / 256.0,
										1.0 / 256.0,
										1.0 / 256.0 );
		float4	comp = Fract( value * bitSh );
		comp -= comp.xxyz * bitMsk;
		return comp;
	}

	// range: 0..1
	// error: 1.98e-3
	float  DecodeV2 (float4 value)
	{
		const float4	bitShifts = float4(	1.0 / (256.0 * 256.0 * 256.0),
											1.0 / (256.0 * 256.0),
											1.0 / 256.0,
											1.0 );
		return Dot( value, bitShifts );
	}
	//---------------------------------


	float4  EncodeV3 (float value)
	{
		return unpackUnorm4x8( floatBitsToUint( value ));
	}

	// range: whole
	// error: 0
	float  DecodeV3 (float4 color)
	{
		return uintBitsToFloat( packUnorm4x8( color ));
	}
	//---------------------------------


	// from https://aras-p.info/blog/2009/07/30/encoding-floats-to-rgba-the-final/
	float4  EncodeV4 (float value)
	{
		const float4	bitSh = float4(	255.0 * 255.0 * 255.0,
										255.0 * 255.0,
										255.0,
										1.0 );
		const float4	bitMsk = float4(0.0,
										1.0 / 255.0,
										1.0 / 255.0,
										1.0 / 255.0 );
		float4	comp = Fract( value * bitSh );
		comp -= comp.xxyz * bitMsk;
		return comp;
	}

	// range: 0..1
	// error: 0.0 .. -1.0e-5
	float  DecodeV4 (float4 value)
	{
		const float4	bitShifts = float4(	1.0 / (255.0 * 255.0 * 255.0),
											1.0 / (255.0 * 255.0),
											1.0 / 255.0,
											1.0 );
		return Dot( value, bitShifts );
	}
	//---------------------------------


	// from https://gamedev.ru/flame/forum/?id=248801&page=225&m=5967042#m3363
	float4  EncodeV5 (float value)
	{
		const float4	bitSh = float4(	256.0 * 256.0 * 256.0,
										256.0 * 256.0,
										256.0,
										1.0 );
		const float4	bitMsk = float4(0.0,
										1.0 / 256.0,
										1.0 / 256.0,
										1.0 / 256.0 );
		float4	comp = Fract( value * bitSh );
		comp -= comp.xxyz * bitMsk;
		return comp * (256.0 / 255.0);
	}

	// range: 0..1
	// error: 1.0e-10 for value < 0.01
	// error: 0 for value > 0.01
	float  DecodeV5 (float4 value)
	{
		const float4	bitShifts = float4(	255.0 / (256.0 * 256.0 * 256.0 * 256.0),
											255.0 / (256.0 * 256.0 * 256.0),
											255.0 / (256.0 * 256.0),
											255.0 / 256.0 );
		return Dot( value, bitShifts );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef ENCODE

	void  Main ()
	{
		float	unpacked = GetGlobalCoordUNorm().x;
		float4	packed;

		switch ( iMode )
		{
			case 0 :	packed = EncodeV1( unpacked );	break;
			case 1 :	packed = EncodeV2( unpacked );	break;
			case 2 :	packed = EncodeV3( unpacked );	break;
			case 3 :	packed = EncodeV4( unpacked );	break;
			case 4 :	packed = EncodeV5( unpacked );	break;
		}

		out_Color0		= packed;
		out_Color1.r	= unpacked;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DECODE

	void  Main ()
	{
		float4	packed		= gl.texture.Fetch( un_Packed, GetGlobalCoord().xy, 0 );
		float	reference	= gl.texture.Fetch( un_Ref, GetGlobalCoord().xy, 0 ).r;
		float	unpacked;

		switch ( iMode )
		{
			case 0 :	unpacked = DecodeV1( packed );	break;
			case 1 :	unpacked = DecodeV2( packed );	break;
			case 2 :	unpacked = DecodeV3( packed );	break;
			case 3 :	unpacked = DecodeV4( packed );	break;
			case 4 :	unpacked = DecodeV5( packed );	break;
		}

		switch ( iCmp )
		{
			case 0 :	out_Color = float4(unpacked);	break;
			case 1 :	out_Color = float4(reference);	break;

			case 2 :
			{
				float	a = Abs( unpacked - reference ) * Exp10( float(iCmpScale) );
				out_Color = a > 1.0 ? float4(1.0, 0.0, 0.0, 1.0) : float4(a);
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
