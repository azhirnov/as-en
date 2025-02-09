// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Mode:

	0. Quads.
	1. Threads in subgroup.

	2. Unique subgroup.
		Each subgroup has random color, use 'Hash' slider to change color.
		Use it to check if GPU can:
			- fill multiple triangles by a single subgroup
			- fill triangles with different gl_InstanceIndex

	3. Full subgroup.
		Red color indicates that all threads in subgroup are executed.
		Green/blue/violet colors indicates that some threads in subgroup are not executed,
		this happens when:
			- helper invocation allocates thread but not executed (optimized)
			- triangle in tile is too small
			- multiple triangles can not be filled with a single subgroup

	4. Helper invocations per quad.
		Red color indicates that all threads in quad are not a helper invocations.
		Blue color indicates that 3 threads in quad are helper invocations.
		The absence of blue colors indicates that driver optimizes quads.

	5. Full quad.
		Red color indicates that all threads in quad are executed.
		Green/blue/violet colors indicates that some threads in quad are not executed.
----

	Draw mode:

	0. Without texturing, with instancing
	1. With texturing, with instancing
	2. Without texturing, without instancing
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/4 );		rt.Name( "RT" );
		RC<Image>			tex			= Image( EImageType::Float_2D, "shadertoy/BlueNoise.png" );
		RC<Scene>			scene		= Scene();
		RC<Scene>			scene2		= Scene();
		RC<DynamicUInt>		draw_mode	= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		with_offset	= DynamicUInt();
		RC<DynamicUInt>		scale		= DynamicUInt();
		RC<DynamicFloat>	hash		= DynamicFloat();
		const float			size		= 0.085;

		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				vbuf		= Buffer();
			array<float2>			vertices;

			vertices.reserve( 10 * 10 * 3 );
			for (uint y = 0; y < 10; ++y)
			for (uint x = 0; x < 10; ++x)
			{
				float2	pos = (float2(x,y) / 5.0) - 1.0 + 0.1;
				vertices.push_back( float2(-1.0, -1.0) * size + pos );
				vertices.push_back( float2(-1.0,  1.0) * size + pos );
				vertices.push_back( float2( 1.0, -1.0) * size + pos );
			}
			vbuf.FloatArray( "vertices", vertices );
			vbuf.LayoutName( "VBuffer" );

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= vertices.size();
			cmd.instanceCount	= 2;
			cmd.firstInstance	= 0;
			geometry.Draw( cmd );

			geometry.ArgIn( "un_VBuffer",	vbuf );

			scene.Add( geometry );
		}
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				vbuf		= Buffer();
			array<float2>			vertices;
			array<uint>				indices;

			vertices.reserve( 10 * 10 * 6 );
			for (uint y = 0; y < 10; ++y)
			for (uint x = 0; x < 10; ++x)
			{
				uint	idx = vertices.size();
				float2	pos = (float2(x,y) / 5.0) - 1.0 + 0.1;
				vertices.push_back( float2(-1.0, -1.0) * size + pos );
				vertices.push_back( float2(-1.0,  1.0) * size + pos );
				vertices.push_back( float2( 1.0, -1.0) * size + pos );
				vertices.push_back( float2( 1.0,  1.0) * size + pos );

				indices.push_back( idx+0 );
				indices.push_back( idx+1 );
				indices.push_back( idx+2 );

				indices.push_back( idx+1 );
				indices.push_back( idx+2 );
				indices.push_back( idx+3 );
			}
			vbuf.FloatArray( "vertices",	vertices );
			vbuf.UIntArray(  "indices",		indices );
			vbuf.LayoutName( "VBuffer" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( vbuf, "indices" );
			geometry.Draw( cmd );

			geometry.ArgIn( "un_VBuffer",	vbuf );

			scene2.Add( geometry );
		}

		Slider( draw_mode,		"DrawMode",		0,		2 );
		Slider( mode,			"Mode",			0,		8,		2 );
		Slider( with_offset,	"WithOffset",	0,		1,		0 );
		Slider( scale,			"Scale",		0,		2,		2 );
		Slider( hash,			"Hash",			1.f,	16.f );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/Subgroups-2a.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Subgroups-2a.as)
			pass.Output(   "out_Color",		rt,		RGBA32f(0.0) );
			pass.Constant( "iMode",			mode );
			pass.Constant( "iWithOffset",	with_offset );
			pass.Constant( "iScale",		scale );
			pass.Constant( "iHash",			hash );
			pass.EnableIfEqual( draw_mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/Subgroups-2b.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Subgroups-2b.as)
			pass.Output(   "out_Color",		rt,		RGBA32f(0.0) );
			pass.Constant( "iMode",			mode );
			pass.Constant( "iWithOffset",	with_offset );
			pass.Constant( "iScale",		scale );
			pass.Constant( "iHash",			hash );
			pass.ArgIn(    "un_Texture",	tex,	Sampler_NearestClamp );
			pass.EnableIfEqual( draw_mode, 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/Subgroups-2c.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Subgroups-2c.as)
			pass.Output(   "out_Color",		rt,		RGBA32f(0.0) );
			pass.Constant( "iMode",			mode );
			pass.Constant( "iScale",		scale );
			pass.Constant( "iHash",			hash );
			pass.ArgIn(    "un_Texture",	tex,	Sampler_NearestClamp );
			pass.EnableIfEqual( draw_mode, 2 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
