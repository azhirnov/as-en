// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define SET_DENSITY
#	define SUBSAMPLE_BLIT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	rt2		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt2.Name( "Subsampled-Color" );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "Subsampled-Depth" );
		RC<Image>	fdm		= Image( EPixelFormat::RG8_UNorm, SurfaceSize().DivCeil(32) );	fdm.Name( "Fragment density map" );
		RC<Scene>	scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 1.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3( 0.f, 0.f, -2.f ));

			scene.Set( camera );
		}

		// setup sphere
		{
			RC<SphericalCube>	sphere = SphericalCube();
			sphere.DetailLevel( 4 );
			scene.Add( sphere );
		}

		// render loop
		{
			RC<Postprocess>			pass = Postprocess( "", "SET_DENSITY" );
			pass.Output( "out_Color",	fdm );
			pass.Slider( "iRate",	float2(0.0),	float2(1.0) );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/FragmentDensity.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/FragmentDensity.as)
			draw.Output( "out_Color", rt2, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.FragmentDensityMap( fdm );
		}{
			RC<Postprocess>		pass = Postprocess( "", "SUBSAMPLE_BLIT" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_Subsampled",	rt2,	Sampler_NearestClampSubsampled );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SET_DENSITY

	void  Main ()
	{
		out_Color.rg = iRate;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SUBSAMPLE_BLIT

	void  Main ()
	{
		out_Color = gl.texture.SampleLod( un_Subsampled, gl.FragCoord.xy * un_PerPass.invResolution, 0.0 );
	}

#endif
//-----------------------------------------------------------------------------
