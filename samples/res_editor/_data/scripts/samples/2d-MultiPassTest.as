// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	define PASS1
#	define PASS2
# 	include <res_editor>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt1		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt1.Name( "RT-1" );
		RC<Image>			rt2		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt2.Name( "RT-2" );
		RC<Image>			tex1	= Image( EImageType::FImage2D, "shadertoy/Abstract_1.jpg" );
		RC<Controller2D>	cam		= Controller2D();

		// render loop
		{
			RC<Postprocess>		pass1 = Postprocess( EPostprocess::Shadertoy, "PASS1" );
			pass1.Input( "iTexture1",	tex1,	Sampler_LinearClamp );
			pass1.Output( rt1 );

			DbgView( rt1, DbgViewFlags::NoCopy );
			
			RC<Postprocess>		pass2 = Postprocess( EPostprocess::Shadertoy, "PASS2" );
			pass2.Input( "iTexture2",	rt1,	Sampler_LinearClamp );
			pass2.Input( cam );
			pass2.Output( rt2 );
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
		uv = Transform2D( ub.camera.viewProj, uv );

		fragColor = 1.0 - gl.texture.Sample( iTexture2, 1.0 - uv );
	}

#endif
//-----------------------------------------------------------------------------
