// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Performance test.
	After depth test GPU should run full subgroup of passed pixels to make 100% workload.

	Test without profiler:
		* set 'Mode=1' to use a single pipeline.
		* use engine internal graphics profiler to measure 'apply materials' pass duration.
		* change 'PipelineHint()' to find pipeline with lowest duration.
		* set 'Mode=0' and measure duration of multiple materials:
			- if 'iQuads==1' then it should be the same as a single slowest material.
			- if 'iQuads==0' then it should be around x2 to a single slowest material because of quad overdraw.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_DEPTH
#	define VIEW_MATERIAL_ID
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>			ds			= Image( EPixelFormat::Depth16, SurfaceSize() );		ds.Name( "RT-Depth" );
		array< RC<Scene> >	scenes;
		RC<Buffer>			cbuffer		= Buffer();
		array<float>		mtr_ids;

		for (uint i = 0, cnt = 30; i < cnt; ++i)
			mtr_ids.push_back( float(i) / cnt );

		array<RC<Image>>	images;
		for (uint i = 0; i < 4; ++i)
		{
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_1.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_3.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/BlueNoise.png" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Lichen.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_1.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_2.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_3.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_4.jpg" ));
		}

		cbuffer.FloatArray(	"ids",	mtr_ids );
		cbuffer.LayoutName( "CBuffer" );

		// create geometry with multiple draw calls
		{
			RC<UnifiedGeometry>	geometry	= UnifiedGeometry();

			for (uint i = 0; i < mtr_ids.size(); ++i)
			{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount		= 3;
				cmd.firstInstance	= i;
				cmd.PipelineHint( "Mtr-" + ToString(i%3+1) + ".ZTest" );
				geometry.Draw( cmd );
			}

			geometry.ArgIn(  "un_CBuffer",	cbuffer );
			geometry.ArgTex( "un_Textures",	images );

			RC<Scene>	s = Scene();
			s.Add( geometry );
			scenes.push_back( s );
		}

		// create geometry with single draw call
		for (uint i = 0; i < 3; ++i)
		{
			RC<UnifiedGeometry>	geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= 3;
			cmd.firstInstance	= 0;
			cmd.PipelineHint( "Mtr-" + ToString(i+1) + ".NoZTest" );
			geometry.Draw( cmd );

			geometry.ArgIn(  "un_CBuffer",	cbuffer );
			geometry.ArgTex( "un_Textures",	images );

			RC<Scene>	s = Scene();
			s.Add( geometry );
			scenes.push_back( s );
		}

		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		gen_depth	= DynamicUInt();
		RC<DynamicUInt>		show_hi		= DynamicUInt();
		Slider( mode,		"View",				0,	scenes.size() );		// MDB, Mat0, Mat1, Mat2, MatID
		Slider( gen_depth,	"GenDepth",			0,	1,				1 );
		Slider( show_hi,	"ShowHelpInvoc",	0,	1 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_DEPTH" );
			pass.SetDebugLabel( "set material depth", RGBA8u(255, 0, 0, 255) );
			pass.ArgIn(  "un_CBuffer",		cbuffer );
			pass.Output( ds, DepthStencil(0.f, 0) );
			pass.Slider( "iScale",	0.0f,	3.0f, 	1.22f );
			pass.Slider( "iBias",	-1.f,	1.f,	0.f );
			pass.Slider( "iQuads",	0,		2,		0 );	// FS always executed in quads 2x2, generate noise to match these quads to gen max performance
			pass.EnableIfEqual( gen_depth, 1 );
		}

		for (uint i = 0; i < scenes.size(); ++i)
		{
			RC<SceneGraphicsPass>	draw = scenes[i].AddGraphicsPass( "pass1" );
			draw.SetDebugLabel( "apply materials", RGBA8u(0, 255, 0, 255) );
			draw.Output( "out_Color", rt, RGBA32f(1.f) );
			draw.OutputLS( ds, EAttachmentLoadOp::Load, EAttachmentStoreOp::None );
			draw.Constant( "iShowHelpInvoc", show_hi );
			draw.EnableIfEqual( mode, i );

			// choose performance level
		//	draw.AddPipelines( "*-HiPerf" );
		//	draw.AddPipelines( "*-MedPerf" );
			draw.AddPipelines( "*-LowPerf" );
		}

		// check distribution of materials
		{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_MATERIAL_ID" );
			pass.ArgTex( "un_Depth",	ds );
			pass.ArgIn(  "un_CBuffer",	cbuffer );
			pass.Output( "out_Color",	rt );
			pass.EnableIfEqual( mode, scenes.size() );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_DEPTH
	#include "Hash.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	p;

		if ( iQuads == 0 )	p = gl.FragCoord.xy * iScale + iBias;
		if ( iQuads == 1 )	p = Floor(gl.FragCoord.xy * 0.5) * iScale + iBias;		// 2x2
		if ( iQuads == 2 )	p = Floor(gl.FragCoord.xy * 0.125) * iScale + iBias;	// 8x8

		float	h = DHash12( p );

		NearestSampleArray( OUT gl.FragDepth, un_CBuffer.ids, h );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_MATERIAL_ID
	#include "Color.glsl"

	void  Main ()
	{
		float	d	= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;
		uint	id	= 4;

		for (uint i = 0; i < un_CBuffer.ids.length(); ++i)
		{
			if ( IsZero( d - un_CBuffer.ids[i] ))
				id = i % 3;
		}

		out_Color = float4(d);
		if ( id == 0 )	out_Color = float4(1.f, 0.f, 0.f, 1.f);
		if ( id == 1 )	out_Color = float4(0.f, 1.f, 0.f, 1.f);
		if ( id == 2 )	out_Color = float4(0.f, 0.f, 1.f, 1.f);
	}

#endif
//-----------------------------------------------------------------------------
