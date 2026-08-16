// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Load sphere map and use it as cubemap.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt			= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Image>		spheremap	= Image( EImageType::Float_2D, "res/PolyHaven/abandoned_slipway_16k.hdr", ImageLoadOpFlags::GenMipmaps );
		RC<FPVCamera>	camera		= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 70.f );

			const float	s = 0.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",		rt,			RGBA32f(0.0) );
			pass.ArgIn(  "un_SphereMap",	spheremap,	Sampler_LinearMipmapClamp );
			pass.Slider( "iProj",			0,		1,		1 );
			pass.Slider( "iTonemap",		0,		1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "ToneMapping.glsl"
	#include "TexSampling.glsl"

	float4  SphereMapProj (float2 uv)
	{
		Ray		ray = Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), 0.1, uv );

		uv = RayInverse_PlaneToSphereMap360( ray.dir );

		// fix UV discontinue
		float2	dx = Abs( gl.dFdxCoarse( uv ));
		float2	dy = Abs( gl.dFdyCoarse( uv ));

		dx = Min( dx, Abs(dx - 1.0) );
		dy = Min( dy, Abs(dy - 1.0) );

		return gl.texture.SampleGrad( un_SphereMap, uv, dx, dy );
	}


	void  Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		switch ( iProj )
		{
			case 0 :	out_Color = gl.texture.Sample( un_SphereMap, uv );	break;
			case 1 :	out_Color = SphereMapProj( uv );					break;
		}

		switch ( iTonemap )
		{
			case 1 :	out_Color.rgb = ToneMap_LinearHDR( out_Color.rgb );  break;
		}
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
