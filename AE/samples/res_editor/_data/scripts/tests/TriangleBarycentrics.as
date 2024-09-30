// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<Scene>	scene		= Scene();
		uint		shape_count	= 0;
		uint		proj_count	= 0;

		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				vbuf		= Buffer();
			const array<float3>		vertices	= {
				float3(0.f, -1.f, 0.5f), float3(-1.f,  1.f, 0.5f), float3(1.f,  1.f, 0.5f),
				float3(0.f, -1.f, 0.1f), float3(-1.f,  1.f, 0.7f), float3(1.f,  1.f, 0.3f)
			};
			const array<float4x4>	proj		= {
				float4x4(),
				float4x4().Ortho( RectF(-1.f, -1.f, 1.f, 1.f), float2(-100.f, 100.f) ),
				float4x4().InfinitePerspective( ToRad(45.f), 1.f, 0.1f ),
				float4x4().InfinitePerspective( ToRad(90.f), 1.5f, 0.1f )
			};

			shape_count = vertices.size()/3;
			proj_count	= proj.size();

			vbuf.FloatArray( "vertices",	vertices );
			vbuf.FloatArray( "projection",	proj );
			vbuf.LayoutName( "VBuffer" );

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 3;
			geometry.Draw( cmd );

			geometry.ArgIn( "un_VBuffer",	vbuf );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "tests/TriangleBarycentrics.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/TriangleBarycentrics.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Slider( "iMode",		0,		3 );
			pass.Slider( "iCmp",		0,		2 );
			pass.Slider( "iScale",		0,		9,					2 );
			pass.Slider( "iShape",		0,		shape_count-1 );
			pass.Slider( "iProj",		0,		proj_count-1 );
			pass.Slider( "iCameraPos",	float3(-10.f),	float3(10.f, 10.f, 100.f),	float3(0.f, 0.f, 0.f) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
