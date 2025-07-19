// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Put NaN to different indices of triangle and different component of vertex.
	Only 2 triangle on right side doesn't have NaNs and must be visible.
	2 triangles on left side must not be visible.
	Other triangles may be visible depends on implementation.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>	scene	= Scene();

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= 3;
			cmd.instanceCount	= 4*2;
			geometry.Draw( cmd );
			scene.Add( geometry, float3( -1.1f, 0.f, 0.f ));
		}

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "grid" );
			pass.AddPipeline( "tests/NanVertex.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/NanVertex.as)
			pass.Output( "out_Color",		rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
