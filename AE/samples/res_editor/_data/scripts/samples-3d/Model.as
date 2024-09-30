// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>	scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera	= FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

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
			draw_pass.Output( "out_Color", rt, RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			draw_pass.Output( ds, DepthStencil(1.f, 0) );
			draw_pass.Layer( ERenderLayer::Opaque );
		}{
			RC<SceneGraphicsPass>	draw_pass = scene.AddGraphicsPass( "translucent" );
			draw_pass.AddPipeline( "samples/Model.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model.as)
			draw_pass.Output( "out_Color", rt );
			draw_pass.Output( ds );
			draw_pass.Layer( ERenderLayer::Translucent );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
