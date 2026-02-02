// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Sample cubemap from fullscreen quad.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );

	//	const string	cm_addr		= "res/humus/LancellottiChapel/";
		const string	cm_addr		= "res/humus/Yokohama3/";
		const string	cm_ext		= ".jpg";
		const uint2		cm_dim		(2048);

		RC<Image>		cubemap		= Image( EPixelFormat::RGBA8_UNorm, cm_dim, ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>		cubemap_view= cubemap.CreateView( EImage::Cube );

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

		// load cubemap
		{
			cubemap.LoadLayer( cm_addr+ "posx" +cm_ext, 0, ImageLoadOpFlags::GenMipmaps );	// -Z
			cubemap.LoadLayer( cm_addr+ "negx" +cm_ext, 1, ImageLoadOpFlags::GenMipmaps );	// +Z
			cubemap.LoadLayer( cm_addr+ "posy" +cm_ext, 2, ImageLoadOpFlags::GenMipmaps );	// +Y	- up
			cubemap.LoadLayer( cm_addr+ "negy" +cm_ext, 3, ImageLoadOpFlags::GenMipmaps );	// -Y	- down
			cubemap.LoadLayer( cm_addr+ "posz" +cm_ext, 4, ImageLoadOpFlags::GenMipmaps );	// -X
			cubemap.LoadLayer( cm_addr+ "negz" +cm_ext, 5, ImageLoadOpFlags::GenMipmaps );	// +X
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",	cubemap_view,	Sampler_LinearMipmapRepeat );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"

	void Main ()
	{
		Ray		ray = Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), 0.1, gl.FragCoord.xy * un_PerPass.invResolution );

		ray.dir.y *= -1.0;
		out_Color.rgb = gl.texture.Sample( un_CubeMap, ray.dir ).rgb;
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
