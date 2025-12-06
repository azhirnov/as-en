// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt2		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>	scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera	= FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 90.f );

			const float	s = 1.0f;
			camera.ForwardBackwardScale( s*2.0f, s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup model
		{
			RC<Model>	model	= Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( float3(0.f, -1.f, 0.f), float3(0.f, ToRad(90.f), ToRad(180.f)), 100.f );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene.Add( model );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw_pass = scene.AddGraphicsPass( "opaque" );
			draw_pass.AddPipeline( "samples/Model.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model.as)
			draw_pass.Output( "out_Color", rt2, RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			draw_pass.Output( ds, DepthStencil(1.f, 0) );
			draw_pass.Layer( ERenderLayer::Opaque );
		}{
			RC<SceneGraphicsPass>	draw_pass = scene.AddGraphicsPass( "translucent" );
			draw_pass.AddPipeline( "samples/Model.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model.as)
			draw_pass.Output( "out_Color", rt2 );
			draw_pass.Output( ds );
			draw_pass.Layer( ERenderLayer::Translucent );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Texture",	rt2,	Sampler_LinearRepeat );
			pass.Slider( "iMode",		0,		1 );
			pass.Slider( "iParams1",	0.0,	2.0,	1.0 );
			pass.Slider( "iParams2",	0.0,	2.0,	1.0 );
			pass.Slider( "iParams3",	0.0,	2.0,	1.0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"
	#include "InvocationID.glsl"

	float2  Panini (float2 viewPos)
	{
		const float	d				= 1.0;
		const float	view_dist		= 2.0;
		const float	view_dist_sq	= 4.0;

		float	view_hyp2		= (viewPos.x * viewPos.x + view_dist_sq);

		float	cyl_hyp_frac	= 1.0 - (viewPos.x * viewPos.x) / view_hyp2;
		float	cyl_dist		= view_dist * cyl_hyp_frac;

		float2	cyl_pos			= viewPos * cyl_hyp_frac;

		return cyl_pos / (cyl_dist - d);
	}


	void  Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;

		float2	uv2 = ToSNorm( uv ) * iParams1 * iParams2;

		if ( iMode == 1 )
			uv2 = Panini( uv2 );

		uv2 = ToUNorm( uv2 / iParams1 );

		if ( iMode != 0 )
			uv = uv2;

		out_Color = gl.texture.Sample( un_Texture, uv );
	}

#endif
//-----------------------------------------------------------------------------
