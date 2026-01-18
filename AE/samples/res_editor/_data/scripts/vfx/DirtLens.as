// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Dirt Lens effect: bloom * dirtLensTex
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_DIRTLENS
#	define GEN_BLOOM
#	define COMBINE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicDim>	dim			= SurfaceSize() / 2;
		RC<Image>		dirt_lens	= Image( EPixelFormat::R8_UNorm, dim );
		RC<Image>		bloom		= Image( EPixelFormat::R11G11B10F, dim );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_DIRTLENS" );
			pass.Output( "out_Color",	dirt_lens );
			pass.Slider( "iScale",		0.5,	8.0,	2.53 );
			pass.Slider( "iRadius",		0.1,	1.0,	0.2 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "GEN_BLOOM" );
			pass.Output( "out_Color",	bloom );
			pass.Slider( "iBlurRadius",	0.01,	2.0,	0.6 );
			pass.ColorSelector( "iColor", RGBA32f(1.0, 0.0, 0.0, 1.0) );
		}{
			RC<Postprocess>		pass = Postprocess( "", "COMBINE" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_DirtLens",	dirt_lens,	Sampler_LinearClamp );
			pass.ArgIn(  "un_Bloom",	bloom,		Sampler_LinearClamp );
			pass.Slider( "iView",		0,		2 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_DIRTLENS
	#include "Hash.glsl"
	#include "Matrix.glsl"
	#include "InvocationID.glsl"

	float2  Transform (float2 uv, float rotate, float offset, float scale)
	{
		uv = f2x2_Rotate( rotate * float_Pi ) * uv;
		uv += offset;
		uv /= scale;
		return uv;
	}

	float  Layer (float2 uv, float uvScale, float rScale)
	{
				uv		= uv * 10.0 / uvScale;
		float2	ipos	= Floor( uv );
		float2	fpos	= ToSNorm( Fract( uv ));
		
		float	radius	= (DHash12( ipos * 22.2 ) * 0.5 + 0.5) * rScale;
		float2	offset	= ToSNorm( DHash22( ipos * 11.1 )) * (1.0 - rScale*1.5);

				uv		= fpos + offset;

		float	d		= Length( uv ) / radius;
		float	a		= 1.0 - SmoothStep( d, 0.9, 1.0 );
		float	b		= 1.0 - (SmoothStep( d, 0.0, 0.8 ) * 0.7 + 0.3);

		return	Saturate( d * a );
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected().xy;

		float	a	= Layer( Transform( uv, 0.1, 0.0,  1.0 ), iScale, iRadius );
		float	b	= Layer( Transform( uv, 0.3, 1.45, 0.8 ), iScale*1.5, iRadius*0.7 );
				a	= Max( a, b );

		float	c	= Layer( Transform( uv, 0.66, 2.43, 0.44 ), iScale*2.5, iRadius*0.35 );
				a	= Max( a, c );

		out_Color.r = a;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_BLOOM
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv		= GetGlobalCoordSNormCorrected().xy;
		float2	point	= un_PerPass.mouse.z > 0.0 ? ToSNorm( un_PerPass.mouse.xy ) : float2(0.0);

		float	d		= Saturate( 1.0 - Distance( uv, point ) / iBlurRadius );

		out_Color = iColor * d;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef COMBINE
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv			= GetGlobalCoordUNorm().xy;
		float3	bloom		= gl.texture.Sample( un_Bloom, uv ).rgb;
		float	dirt_lens	= gl.texture.Sample( un_DirtLens, uv ).r;

		out_Color.a = 1.0;

		switch ( iView )
		{
			case 0 :	out_Color.rgb = bloom * dirt_lens;  break;
			case 1 :	out_Color.rgb = float3(dirt_lens);  break;
			case 2 :	out_Color.rgb = bloom;				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
