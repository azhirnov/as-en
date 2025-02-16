// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use tileable noise to generate circle with distortion.
	Can not be used for 2D space distortion!
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
		RC<Image>		rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		const array<float>	params = {
			0, 								// iNoise
			3, 								// iOctaves
			20.0f,							// iPScale
			0.0000f, 1.2500f, 2.3210f, 		// iPBias
			0.5540f, 2.3380f, 				// iLacunarity
			1.0000f, 23.7450f, 				// iPersistence
			2.3260f, 0.0000f, 				// iVScaleBias
		};

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );

			pass.Slider( "iIsolines",	0,		1 );
			pass.Slider( "iScale",		1.0,	10.0,	5.0 );
			pass.Slider( "iSmooth",		0.0,	0.2,	0.05 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Hash.glsl"
	#include "InvocationID.glsl"


	ND_ float  CircleNoise (const float2 uv)
	{
		float	d = Length( uv );	// center
		float	k = iSmooth;

		d = SDF_OpUnite( d, Length( uv - float2( 0.02,  0.10 )), k );
		d = SDF_OpUnite( d, Length( uv - float2(-0.08, -0.03 )), k );

		return d;
	}

	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected();

		float	d	= Length( uv );
		float	d2	= d + CircleNoise( uv ) * iScale;

		out_Color = float4(0.25);

		if ( iIsolines == 0 )
		{
			out_Color.rgb *= AA_Lines( d * un_PerPass.resolution.x, 1.0/300.0, 3.0 );

			if ( Abs( d2 - 0.5 ) < 0.003 )
				out_Color = float4(1.0);
		}

		if ( iIsolines == 1 )
		{
			out_Color.rgb = SDF_Isolines( (d2 - 0.5) * 8.0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
