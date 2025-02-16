// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt	= Image( EPixelFormat::RGBA16F, SurfaceSize() );	rt.Name( "RT" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage",	rt );
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

	float3  ColorLerpHSV1 (float3 a, float3 b, float factor)
	{
		a = RGBtoHSV( a );
		b = RGBtoHSV( b );
		return HSVtoRGB( Lerp( a, b, factor ));
	}

	float3  ColorLerpHSV2 (float3 a, float3 b, float factor)
	{
		a = RGBtoHSV( a );
		b = RGBtoHSV( b );
		return HSVtoRGB( LerpHSV( a, b, factor ));
	}

	float3  ColorLerpYUV (float3 a, float3 b, float factor)
	{
		a = RGBtoYUV( a );
		b = RGBtoYUV( b );
		return YUVtoRGB( Lerp( a, b, factor ));
	}

	float3  ColorLerpXYY (float3 a, float3 b, float factor)
	{
		a = RGBtoXYY_v2( a );
		b = RGBtoXYY_v2( b );
		return XYYtoRGB_v2( Lerp( a, b, factor ));
	}

	float3  ColorLerpXYZ (float3 a, float3 b, float factor)
	{
		a = RGBtoXYZ( a );
		b = RGBtoXYZ( b );
		return XYZtoRGB( Lerp( a, b, factor ));
	}

	float3  ColorLerpOklab (float3 a, float3 b, float factor)
	{
		a = RGBtoOklab( a );
		b = RGBtoOklab( b );
		return OklabToRGB( Lerp( a, b, factor ));
	}

	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		float3	color	= float3(0.0);
		float3	src_col	= float3(0.0);
		float3	dst_col	= float3(0.0);

		// choose color
		switch ( int(uv.y * 5) )
		{
			case 0 :	src_col = float3(1.0, 0.0, 0.0);	dst_col = float3(0.0, 1.0, 0.0);	break;
			case 1 :	src_col = float3(1.0, 0.0, 0.0);	dst_col = float3(0.0, 0.0, 1.0);	break;
			case 2 :	src_col = float3(0.0, 1.0, 0.0);	dst_col = float3(0.0, 0.0, 1.0);	break;
			case 3 :	src_col = float3(0.3, 0.0, 0.0);	dst_col = float3(0.0, 0.0, 0.3);	break;
			case 4 :	src_col = float3(0.0, 0.0, 0.1);	dst_col = float3(1.0, 1.0, 0.0);	break;
		}

		if ( uv.x > 0.05 and uv.x < 0.95 )
		{
			const float	f = RemapClamp( float2(0.05, 0.95), float2(0.0, 1.0), uv.x );

			// choose interpolator
			switch ( int(uv.y * 5*8) % 8 )
			{
				case 0 :	color = Lerp( src_col, dst_col, f );			break;
				case 1 :	color = ColorLerpYUV( src_col, dst_col, f );	break;
				case 2 :	color = ColorLerpXYY( src_col, dst_col, f );	break;
				case 3 :	color = ColorLerpXYZ( src_col, dst_col, f );	break;
				case 4 :	color = ColorLerpHSV2( src_col, dst_col, f );	break;
				case 5 :	color = ColorLerpHSV1( src_col, dst_col, f );	break;
				case 6 :	color = ColorLerpOklab( src_col, dst_col, f );	break;
			}
		}

		if ( (uv.x < 0.04 or uv.x > 0.96) and (int(uv.y * 5*8) % 8 < 7) )
		{
			color = uv.x < 0.5 ? src_col : dst_col;
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(color, 1.0) );
	}

#endif
//-----------------------------------------------------------------------------
