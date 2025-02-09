// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Demonstrates how to render to cubemap as in shadertoy.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PASS_1
#	define PASS_2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(1024), ImageLayer(6) );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::ShadertoyCubemap, "PASS_1" );
			pass.Output( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy, "PASS_2" );
			pass.Output( rt );
			pass.ArgIn( "iChannel0",	cubemap_view,	Sampler_LinearRepeat );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS_1
	#include "Math.glsl"

	void mainCubemap (out float4 fragColor, in float2 fragCoord, in float3 rayOri, in float3 rayDir)
	{
		fragColor.rgb = ToUNorm( rayDir );
		fragColor.rgb += Sin( rayDir * 10.0 ) * 0.1;
		fragColor.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS_2
	#include "Geometry.glsl"
	#include "Quaternion.glsl"

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		float4	norm = UVtoSphereNormal( (fragCoord - iResolution.xy * 0.5) / MaxOf( iResolution ) * 4.0 );

		norm.xyz = QMul( QRotationY( iTime * 0.5 ), norm.xyz );

		fragColor = texture( iChannel0, norm.xyz );
		fragColor *= SmoothStep( norm.w, 0.0, 0.01 );
	}

#endif
//-----------------------------------------------------------------------------
