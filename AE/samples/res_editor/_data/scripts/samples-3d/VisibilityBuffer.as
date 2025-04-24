// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Visibility buffer prototype.
	- Implemented using ray tracing pass which already has storage buffer for all vertices.
	- Derivative calculations copy-pasted from 'The Forge' engine.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	idbuf	= Image( EPixelFormat::R32U, SurfaceSize() );			idbuf.Name( "ID-Buffer" );
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds		= Image( EPixelFormat::R32F, SurfaceSize() );			ds.Name( "RT-Depth" );
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
			RC<SceneRayTracingPass>	pass = scene.AddRayTracingPass( "fill VisBuffer" );
			pass.SetPipeline( "samples/VisibilityBuffer-pass1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/VisibilityBuffer-pass1.as)
			pass.ArgOut( "un_IDBuffer",		idbuf );
			pass.ArgOut( "un_Depth",		ds );
			pass.Dispatch( rt.Dimension() );
		}{
			RC<SceneRayTracingPass>	pass = scene.AddRayTracingPass( "resolve VisBuffer" );
			pass.SetPipeline( "samples/VisibilityBuffer-pass2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/VisibilityBuffer-pass2.as)
			pass.ArgOut( "un_ColorBuf",		rt );
			pass.ArgIn(  "un_IDBuffer",		idbuf );
			pass.ArgIn(  "un_Depth",		ds );
			pass.Dispatch( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
