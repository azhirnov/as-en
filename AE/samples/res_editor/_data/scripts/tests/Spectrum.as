// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Test for white color spectrum.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA16F, SurfaceSize() );		rt.Name( "RT" );
		RC<Buffer>	cbuf	= Buffer();

		{
			array<float4>	wl_to_rgb_3;
			WhiteColorSpectrum3( OUT wl_to_rgb_3 );

			array<float4>	wl_to_rgb_7;
			WhiteColorSpectrum7( OUT wl_to_rgb_7, false );

			array<float4>	wl_to_rgb_50nm;
			WhiteColorSpectrumStep50nm( OUT wl_to_rgb_50nm, false );

			array<float4>	wl_to_rgb_100nm;
			WhiteColorSpectrumStep100nm( OUT wl_to_rgb_100nm, false );

			cbuf.FloatArray( "wavelengthToRGB_3",		wl_to_rgb_3 );
			cbuf.FloatArray( "wavelengthToRGB_7",		wl_to_rgb_7 );
			cbuf.FloatArray( "wavelengthToRGB_50nm",	wl_to_rgb_50nm );
			cbuf.FloatArray( "wavelengthToRGB_100nm",	wl_to_rgb_100nm );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage",		rt );
			pass.ArgIn(  "un_Constants",	cbuf );

			pass.Slider( "V0", 0.5, 1.2, 0.821 );
			pass.Slider( "V1", 0.5, 1.3, 1.071 );

			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "Color.glsl"
	#include "SDF.glsl"

	float4  ColorSelect (float4 a, float4 b, float factor)
	{
		return factor <= 0.5 ? a : b;
	}

	float4  ColorLerp (float4 a, float4 b, float factor)
	{
		a.yzw = RGBtoHSV( a.yzw );
		b.yzw = RGBtoHSV( b.yzw );
		return float4( 0.f, HSVtoRGB( LerpHSV( a.yzw, b.yzw, factor )));
	}

	#define SampleArray( _result_, _array_, _wl_, _lerp_ )										\
	{																							\
		if ( (_wl_) <= (_array_)[0].x )															\
		{																						\
			(_result_) = (_array_)[0];															\
		}																						\
		else																					\
		if ( (_wl_) >= (_array_)[ (_array_).length()-1 ].x )									\
		{																						\
			(_result_) = (_array_)[ (_array_).length()-1 ];										\
		}																						\
		else																					\
		for (uint i = 1; i < (_array_).length(); ++i)											\
		{																						\
			if ( (_wl_) >= (_array_)[i-1].x and (_wl_) < (_array_)[i].x )						\
			{																					\
				float	f  = ((_wl_) - (_array_)[i-1].x) / ((_array_)[i].x - (_array_)[i-1].x);	\
				(_result_) = _lerp_( (_array_)[i-1], (_array_)[i], f );							\
				break;																			\
			}																					\
		}																						\
	}

	#undef NearestSampleArray
	#undef LinearSampleArray

	#define NearestSampleArray( _result_, _array_, _wl_ )	SampleArray( (_result_), (_array_), (_wl_), ColorSelect )
	#define LinearSampleArray( _result_, _array_, _wl_ )	SampleArray( (_result_), (_array_), (_wl_), ColorLerp )

	#define SummArray( _result_, _array_ )				\
		for (uint i = 0; i < (_array_).length(); ++i)	\
			_result_ += (_array_)[i];


	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		float4	wl_rgb	= float4(0.0);

		uv.x *= 1.1;
		const float	wl = Remap( float2(0.0, 1.0), float2(400.0, 700.0), uv.x );

		switch ( int(uv.y * 10) )
		{
			case 0 :	wl_rgb = HSVtoRGB(float3( Saturate(V0 - uv.x*V1), 1.f, 1.f )).rrgb;			break;

			case 1 :	LinearSampleArray(  OUT wl_rgb, un_Constants.wavelengthToRGB_3,		wl );	break;
			case 2 :	LinearSampleArray(  OUT wl_rgb, un_Constants.wavelengthToRGB_100nm,	wl );	break;
			case 3 :	LinearSampleArray(  OUT wl_rgb, un_Constants.wavelengthToRGB_7,		wl );	break;
			case 4 :	LinearSampleArray(  OUT wl_rgb, un_Constants.wavelengthToRGB_50nm,	wl );	break;

			case 6 :	NearestSampleArray( OUT wl_rgb, un_Constants.wavelengthToRGB_3,		wl );	break;
			case 7 :	NearestSampleArray( OUT wl_rgb, un_Constants.wavelengthToRGB_100nm,	wl );	break;
			case 8 :	NearestSampleArray( OUT wl_rgb, un_Constants.wavelengthToRGB_7,		wl );	break;
			case 9 :	NearestSampleArray( OUT wl_rgb, un_Constants.wavelengthToRGB_50nm,	wl );	break;
		}

		if ( uv.x > 1.0 )
		{
			wl_rgb	= float4(0.0);
			switch ( int(uv.y * 10) )
			{
				case 6 :	SummArray( OUT wl_rgb, un_Constants.wavelengthToRGB_3 );		break;
				case 7 :	SummArray( OUT wl_rgb, un_Constants.wavelengthToRGB_100nm );	break;
				case 8 :	SummArray( OUT wl_rgb, un_Constants.wavelengthToRGB_7 );		break;
				case 9 :	SummArray( OUT wl_rgb, un_Constants.wavelengthToRGB_50nm );		break;
			}

			wl_rgb = wl_rgb / Floor(Remap( float2(1.0, 1.1), float2(1.0, 12.0), uv.x ));
		}
		else
		{
			wl_rgb = Lerp( float4(0.7), wl_rgb, AA_Lines( GetGlobalCoord().x, 6.0 / Floor(GetGlobalSize().x / 1.1), 1.5 ));		// step 50nm
			wl_rgb = Lerp( float4(1.0), wl_rgb, AA_Lines( GetGlobalCoord().x, 3.0 / Floor(GetGlobalSize().x / 1.1), 3.0 ));		// step 100nm
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(wl_rgb.yzw, 1.0) );
	}

#endif
//-----------------------------------------------------------------------------
