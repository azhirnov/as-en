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
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	smoke	= Image( EImageType::Float_2D, "res/tex/smoke-2.jpg" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ArgIn( "un_Smoke",		smoke,	Sampler_LinearMipmapClamp );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"


	float  Blend (float a, float b)
	{
		return (a * (1.0 - b)) + (b * (1.0 - a));
	}


	float  Sample (float2 uv, float angle, float2 off, float scale)
	{
		uv = SDF_Move( uv, off );
		uv = SDF_Rotate2D( uv, angle );
		uv = ToUNorm( uv / scale );

		float	x = gl.texture.Sample( un_Smoke, uv ).r;
		x = RemoveSRGBCurve( x );

		return IsUNorm( uv ) ? x : 0.0;
	}

	float  Sample2 (float2 uv, float2 off, float scale)
	{
		float	angle1	= ToRad( un_PerPass.time * 10.f );
		float	angle2	= ToRad( un_PerPass.time * 15.f + 77.f );

		float	a	= Sample( uv, angle1, off, scale );
		float	b	= Sample( uv, angle2, off+0.1, scale+0.1 );

		return (a * 0.5) + (b * 0.5);
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected2().xy * 2.0;

	#if 1
		float	a	= Sample2( uv, float2(0.0), 2.0 );
	#else
		float	a	= Sample( uv, ToRad( un_PerPass.time * 10.f ),			float2(-1.5, 1.5), 0.7 );
		float	b	= Sample( uv, ToRad( un_PerPass.time * 10.f + 75.f ),	float2(-1.0, 1.0), 1.0 );
				a	= Blend( a, b );

				b	= Sample( uv, ToRad( un_PerPass.time * 10.f + 165.f ),	float2(-0.2, 0.2), 1.5 );
				a	= Blend( a, b );
	#endif

		out_Color = float4(ApplySRGBCurve( a ));
	}

#endif
//-----------------------------------------------------------------------------
