// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt1		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt1.Name( "RT-Color1" );
		RC<Image>	rt2		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt2.Name( "RT-Color2" );
		RC<Image>	ds1		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds1.Name( "RT-Depth1" );
		RC<Image>	ds2		= Image( EPixelFormat::Depth16, SurfaceSize() );		ds2.Name( "RT-Depth2" );
		RC<Scene>	scene	= Scene();

		// create full screen quad
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 3;
			geometry.Draw( cmd );
			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "subpass-1" );
			{
				draw.AddPipeline( "tests/Subpass1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/Subpass1.as)
				draw.Output( "out_Color1",	rt1,	RGBA32f(0.f) );
				draw.Output( "Depth1",		ds1,	DepthStencil(1.f, 0) );
			}
			draw.NextSubpass( "subpass-2" );
			{
				draw.AddPipeline( "tests/Subpass1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/Subpass1.as)
				draw.Output( "Depth2",		ds2,	DepthStencil(1.f, 0) );
				draw.Output( "out_Color2",	rt2,	RGBA32f(0.f) );
				draw.Constant( "iSubpass1",	1 );
			}
			draw.NextSubpass( "subpass-3" );
			{
				draw.AddPipeline( "tests/Subpass2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/Subpass2.as)
				draw.Input( "in_Color2",	rt2,			"out_Color2" );
				draw.Input( "in_Depth1",	ds1,			"Depth1" );
				draw.Input( "in_Depth2",	ds2,			"Depth2" );
				draw.InOut( "in_Color1",	"out_Color1",	rt1 );
				draw.Slider( "iMode",		0,	3 );
			}
		}

		Present( rt1 );
	}

#endif
//-----------------------------------------------------------------------------
