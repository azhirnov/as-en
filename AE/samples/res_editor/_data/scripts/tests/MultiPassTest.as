// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Multiple passes with post process.
	Demonstrates texture loading and debug view.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PASS1
#	define PASS2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt1		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt1.Name( "RT-1" );
		RC<Image>			rt2		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt2.Name( "RT-2" );
		RC<Image>			tex1	= Image( EImageType::Float_2D, "shadertoy/Abstract_1.jpg" );
		RC<ScaleBiasCamera>	camera	= ScaleBiasCamera();

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy, "PASS1" );
			pass.ArgIn( "iTexture1",	tex1,	Sampler_LinearClamp );
			pass.Output( rt1 );
		}{
			DbgView( rt1, DbgViewFlags::NoCopy );
		}{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy, "PASS2" );
			pass.ArgIn( "iTexture2",	rt1,	Sampler_LinearClamp );
			pass.Set(	camera );
			pass.Output( rt2 );
		}
		Present( rt2 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS1

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		float2 uv = fragCoord / iResolution.xy;

		fragColor = gl.texture.Sample( iTexture1, uv );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS2
	#include "Matrix.glsl"

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		float2 uv = fragCoord / iResolution.xy;
		uv = Transform2D( un_PerPass.camera.viewProj, uv );

		fragColor.rgb = 1.0 - gl.texture.Sample( iTexture2, 1.0 - uv ).rgb;
		fragColor.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
