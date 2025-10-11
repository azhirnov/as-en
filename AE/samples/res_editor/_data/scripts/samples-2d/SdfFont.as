// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
	https://github.com/Chlumsky/msdfgen/files/3050967/thesis.pdf
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
		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<Image>	sdf_font_32		= Image( EImageType::Float_2D, "res/font/sdf-32.dds" );
		RC<Image>	mc_sdf_font_32	= Image( EImageType::Float_2D, "res/font/mc-sdf-32.dds" );
		RC<Image>	mc_sdf_font_64	= Image( EImageType::Float_2D, "res/font/mc-sdf-64.dds" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();

			pass.ArgIn( "un_SdfFont_32",		sdf_font_32,		Sampler_LinearRepeat );
			pass.ArgIn( "un_McSdfFont_32",		mc_sdf_font_32,		Sampler_LinearRepeat );
			pass.ArgIn( "un_McSdfFont_64",		mc_sdf_font_64,		Sampler_LinearRepeat );
			pass.Output( "out_Color",			rt );

			pass.Slider( "iAnimate",	0,		1,		1 );
			pass.Slider( "iSdfTex",		0,		2 );
			pass.Slider( "iMode",		0,		4 );
			pass.Slider( "iScale",		0.1,	8.0,	4.0 );
			pass.Slider( "iOffset",		0.0,	1.0 );
			pass.Slider( "iRotate",		0.0,	90.0 );

			pass.Slider( "iConstThick",	float2(-5.0,0.0),	float2(0.0,8.0),	float2(0.0,0.0) );	// constant thickness
			pass.Slider( "iAAFactor",	0.0,				5.0,				1.5 );				// anti-aliasing factor
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "InvocationID.glsl"


	float  ApplyStyle (float2 uv, float sd, float2 size)
	{
		float3	thick = float3(iConstThick, iAAFactor);
		switch ( iMode )
		{
			// regular
			case 0 :	thick = float3(-0.5, 0.0, iAAFactor);	break;

			// bold
			case 1 :	thick = float3(0.0, 2.0, iAAFactor);	break;

			// outline
			case 2 :	thick = float3(-3.0, 8.0, iAAFactor);	break;

			// custom
			//case 3 :
			//case 4 :
		}

		sd = 1.0 - AA_Font( uv, sd, thick, size ).x;

		if ( iMode == 2 or iMode == 4 )
			sd = sd > 0.5 ? TriangleWave( (sd-0.5) * 2.0 ) : 0.0;

		return sd;
	}


	float4  SdfFont (gl::CombinedTex2D<float> sdfTex, float2 uv, float sdfScale, float sdfBias)
	{
		float2	size	= float2(gl.texture.GetSize( sdfTex, 0 ));

		uv.x *= size.y / size.x;

		float	sd = gl.texture.SampleLod( sdfTex, uv, 0.0 ).r;
				sd = FusedMulAdd( sd, sdfScale, sdfBias );
				sd = ApplyStyle( uv, sd, size );

		return float4(sd, 0.0, 0.0, 1.0);
	}


	float4  McSdfFont (gl::CombinedTex2D<float> msdfTex, float2 uv, float sdfScale, float sdfBias)
	{
		float2	size	= float2(gl.texture.GetSize( msdfTex, 0 ));

		uv.x *= size.y / size.x;

		float3	msd	= gl.texture.SampleLod( msdfTex, uv, 0.0 ).rgb;
		float	sd	= MCSDF_Median( msd );
				sd	= FusedMulAdd( sd, sdfScale, sdfBias );
				sd	= ApplyStyle( uv, sd, size );

		return float4(0.0, sd, 0.0, 1.0);
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected2();
		uv.x += iOffset;
		uv = SDF_Rotate2D( uv, ToRad(iRotate) );
		uv = ToUNorm( uv * iScale );

		if ( iAnimate == 1 )
			uv.x += TriangleWave( un_PerPass.time * 0.01 ) * 4.0;

		switch ( iSdfTex )
		{
			case 0 :	out_Color = SdfFont(	un_SdfFont_32,		uv, -20.1597252, 17.5723000 );	break;
			case 1 :	out_Color = McSdfFont(	un_McSdfFont_32,	uv, -41.1041679, 21.1272316 );	break;

			case 2 :	uv.y = Fract( uv.y );	uv *= 0.555;
						out_Color = McSdfFont(	un_McSdfFont_64,	uv, -40.4665451, 20.5937500 );	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
