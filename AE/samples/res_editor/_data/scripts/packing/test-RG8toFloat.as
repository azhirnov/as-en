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
		RC<Image>			reference	= Image( EPixelFormat::RG32F, SurfaceSize() );
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		cmp			= DynamicUInt();

		Slider( mode,	"Mode",		0,	3 );

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
			pass.Slider( "iCmp",		0,		4 );
			pass.Slider( "iCmpScale",	0,		9,		2 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "InvocationID.glsl"


	// based on code from https://stackoverflow.com/questions/34963366/encode-floating-point-data-in-a-rgba-texture
	float4  EncodeV2 (float2 value)
	{
		const float2	bitSh  = float2( 256.0, 1.0 );
		const float		bitMsk = 1.0 / 256.0;

		float4	comp = Fract( value.xxyy * bitSh.xyxy );
		comp.yw -= comp.xz * bitMsk;
		return comp;
	}

	// error: 1.98e-3
	float2  DecodeV2 (float4 value)
	{
		const float2	bitShifts = float2(	1.0 / 256.0, 1.0 );
		return float2( Dot( value.xy, bitShifts ), Dot( value.zw, bitShifts ));
	}
	//---------------------------------


	float4  EncodeV3 (float2 value)
	{
		return unpackUnorm4x8( packUnorm2x16( value ));
	}

	// error: 1.0e-5
	float2  DecodeV3 (float4 color)
	{
		return unpackUnorm2x16( packUnorm4x8( color ));
	}
	//---------------------------------


	// based on code from https://aras-p.info/blog/2009/07/30/encoding-floats-to-rgba-the-final/
	float4  EncodeV4 (float2 value)
	{
		const float2	bitSh  = float2( 255.0, 1.0 );
		const float		bitMsk = 1.0 / 255.0;

		float4	comp = Fract( value.xxyy * bitSh.xyxy );
		comp.yw -= comp.xz * bitMsk;
		return comp;
	}

	// error: 1.0e-5
	float2  DecodeV4 (float4 value)
	{
		const float2	bitShifts = float2( 1.0 / 255.0, 1.0 );
		return float2( Dot( value.xy, bitShifts ), Dot( value.zw, bitShifts ));
	}
	//---------------------------------


	// based on code from https://gamedev.ru/flame/forum/?id=248801&page=225&m=5967042#m3363
	float4  EncodeV5 (float2 value)
	{
		const float2	bitSh  = float2( 256.0, 1.0 );
		const float		bitMsk = 1.0 / 256.0;

		float4	comp = Fract( value.xxyy * bitSh.xyxy );
		comp.yw -= comp.xz * bitMsk;
		return comp * (256.0 / 255.0);
	}

	// error: 1.0e-5
	float2  DecodeV5 (float4 value)
	{
		const float2	bitShifts = float2( 255.0 / (256.0 * 256.0),
											255.0 / 256.0 );
		return float2( Dot( value.xy, bitShifts ), Dot( value.zw, bitShifts ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef ENCODE

	void  Main ()
	{
		float2	unpacked = GetGlobalCoordUNorm().xy;
		float4	packed;

		switch ( iMode )
		{
			case 0 :	packed = EncodeV2( unpacked );	break;
			case 1 :	packed = EncodeV3( unpacked );	break;
			case 2 :	packed = EncodeV4( unpacked );	break;
			case 3 :	packed = EncodeV5( unpacked );	break;
		}

		out_Color0		= packed;
		out_Color1.rg	= unpacked;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DECODE

	void  Main ()
	{
		float4	packed		= gl.texture.Fetch( un_Packed, GetGlobalCoord().xy, 0 );
		float2	reference	= gl.texture.Fetch( un_Ref,    GetGlobalCoord().xy, 0 ).rg;
		float2	unpacked;

		switch ( iMode )
		{
			case 0 :	unpacked = DecodeV2( packed );	break;
			case 1 :	unpacked = DecodeV3( packed );	break;
			case 2 :	unpacked = DecodeV4( packed );	break;
			case 3 :	unpacked = DecodeV5( packed );	break;
		}

		switch ( iCmp )
		{
			case 0 :	out_Color = float4(unpacked, 0.0, 1.0);		break;
			case 1 :	out_Color = float4(reference, 0.0, 1.0);	break;
			case 2 :
			{
				float	a = Distance( unpacked, reference ) * Exp10( float(iCmpScale) );
				out_Color = a > 1.0 ? float4(1.0, 0.0, 0.0, 1.0) : float4(a);
				break;
			}
			case 3 :
			{
				float	a = Abs( unpacked.x - reference.x ) * Exp10( float(iCmpScale) );
				out_Color = a > 1.0 ? float4(0.0, 1.0, 0.0, 1.0) : float4(a);
				break;
			}
			case 4 :
			{
				float	a = Abs( unpacked.y - reference.y ) * Exp10( float(iCmpScale) );
				out_Color = a > 1.0 ? float4(0.0, 1.0, 0.0, 1.0) : float4(a);
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
