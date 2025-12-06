// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	results in [GeometryCulling paper](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/GeometryCulling-ru.md)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
	  #if 0
		RC<DynamicDim>		dim					= SurfaceSize();
	  #else
		RC<DynamicUInt>		dim_scale			= DynamicUInt();
		RC<DynamicUInt2>	rt_dim				= DynamicUInt2( uint2(1920, 1080)/2 ).Mul( dim_scale.Exp2().XX() );
		RC<DynamicDim>		dim					= rt_dim.Dimension();

		Slider( dim_scale,	"DimensionScale",	0, 2, 1 );	// 1K, 2K, 4K
		Label(  rt_dim,		"Dimension" );
	  #endif

		RC<DynamicUInt2>	pyramid_dim			= rt_dim.Div(uint2(2)).NearPOT();
		RC<Image>			rt					= Image( EPixelFormat::RGBA8_UNorm, dim );		rt.Name( "RT" );
		RC<Image>			ds					= Image( Supported_DepthFormat(), dim );		ds.Name( "Depth" );
		RC<Image>			pyramid				= Image( EPixelFormat::R32F, pyramid_dim.Dimension(), MipmapLevel(~0) );	pyramid.Name( "Depth pyramid" );
		RC<Image>			vis					= Image( EPixelFormat::RG16U, dim );			vis.Name( "Visibility buffer" );
		RC<Image>			vis2				= Image( EPixelFormat::RGBA16F, dim );			vis2.Name( "Visibility buffer barycentrics" );

		RC<Scene>			scene_direct_draw	= Scene();
		RC<Scene>			scene_indirect_draw	= Scene();
		RC<Scene>			scene_aabb			= Scene();
		RC<FPVCamera>		camera				= FPVCamera();
		RC<Buffer>			obj_buf				= Buffer();
		RC<Buffer>			indirect_buf		= Buffer();
		RC<Buffer>			remap_idx			= Buffer();
		RC<Buffer>			vis_flags			= Buffer();

		const uint3			local_size			= uint3( 4, 4, 2 );
		RC<DynamicUInt>		obj_count			= DynamicUInt();
		RC<DynamicUInt3>	count3d				= obj_count.XXX().Mul( local_size );
		RC<DynamicUInt>		count				= count3d.Volume();
		RC<DynamicUInt>		mode				= DynamicUInt();
		RC<DynamicUInt>		tris_count;
		RC<DynamicUInt>		repeat				= DynamicUInt();
		RC<DynamicFloat3>	light_dir			= DynamicFloat3( float3( 0.4, -1.0, -1.0 ));
		uint				index_count;
		const bool			has_minmax_sampler = GetFeatureSet().hasSamplerFilterMinmax();
		bool				low_detail			= true;

		switch ( GPUVendor() )
		{
			case EGPUVendor::NVidia :
			case EGPUVendor::AMD :
				low_detail = false;		break;
		}

		array<RC<Image>>	images;
		for (uint i = 0; i < 4; ++i)
		{
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_1.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_3.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Wood.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Lichen.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_1.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_2.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_3.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_4.jpg" ));
		}

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;" +
			"	float	scale;" +
			"	uint	color;",
			count );

		remap_idx.ArrayLayout(
			"ObjectIdxRemapping",
			"	uint	newIndex;",
			count );

		vis_flags.ArrayLayout(
			"ObjectIsVisible",
			"	uint	visible;",
			count );

		indirect_buf.UseLayout(
			"DrawCmd",
			"	DrawIndexedIndirectCommand	cmd;" );

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 200.f );
			camera.FovY( 57.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );

			scene_direct_draw.Set( camera );
			scene_indirect_draw.Set( camera );
			scene_aabb.Set( camera );
		}

		// create geometry
		{
			array<float3>	positions;
			array<float2>	uvs;
			array<uint>		indices;
			GetSphere( (low_detail ? 3 : 8), OUT positions, OUT uvs, OUT indices );
			index_count = indices.size();

			@tris_count = count.Mul( index_count/3 );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.FloatArray( "uvs",		uvs );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );
				geometry.ArgTex( "un_Textures",	images );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= index_count;
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( count );
				geometry.Draw( cmd );

				scene_direct_draw.Add( geometry );
			}{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );
				geometry.ArgTex( "un_Textures",	images );

				UnifiedGeometry_DrawIndexedIndirect		cmd;
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.IndirectBuffer( indirect_buf, "cmd" );
				geometry.Draw( cmd );

				scene_indirect_draw.Add( geometry );
			}

			// create full screen quad
			{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );
				geometry.ArgTex( "un_Textures",	images );

				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 3;
				cmd.layer		= ERenderLayer::PostProcess;
				geometry.Draw( cmd );

				scene_direct_draw.Add( geometry );
				scene_indirect_draw.Add( geometry );
			}
		}

		// create AABB
		{
			array<float3>	positions, normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( count );
			geometry.Draw( cmd );

			scene_aabb.Add( geometry );
		}


		// render loop //
		uint	mode_id = 0;

		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",	obj_buf );
			pass.Slider(	"iBackToFront",	0,		1 );
			pass.Slider(	"iRadius",		0.5,	2.0,	1.0 );
			pass.Constant(	"iDimension",	dim );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}

		// without depth test
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "no depth" );
			pass.AddPipeline( "perf/Culling/2-NoDepthTest.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-NoDepthTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// late depth test
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "late ZS" );
			pass.AddPipeline( "perf/Culling/2-DepthLateTest.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthLateTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// early depth test
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "early ZS" );
			pass.AddPipeline( "perf/Culling/2-DepthTest.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// depth pre-pass
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "depth pre-pass" );
			pass.AddPipeline( "perf/Culling/2-DepthPrePass.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthPrePass.as)
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/Culling/2-DepthEqual.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthEqual.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0) );
			pass.Output(				ds );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// depth pre-pass as subpass (for TBDR)
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "DPP subpass" );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
			{
				pass.AddPipeline( "perf/Culling/2-DepthPrePass-p0.as" );// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthPrePass-p0.as)
				pass.Output(				ds,		DepthStencil(1.0, 0) );
			}
			pass.NextSubpass( "draw" );
			{
				pass.AddPipeline( "perf/Culling/2-DepthEqual-p1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthEqual-p1.as)
				pass.Output( "out_Color",	rt,		RGBA32f(1.0) );
				pass.Output(				ds );
				pass.Constant( "iLight",	light_dir );
			}
		}

		// visibility buffer
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "VisBuf1-build" );
			pass.AddPipeline( "perf/Culling/2-VisBuf1-build.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-VisBuf1-build.as)
			pass.Output( "out_VisBuf",	vis,	RGBA32u(~0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "VisBuf1-resolve" );
			pass.AddPipeline( "perf/Culling/2-VisBuf1-resolve.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-VisBuf1-resolve.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.ArgIn(  "un_VisBuf",	vis,	Sampler_NearestClamp );
			pass.Constant( "iLight",	light_dir );
			pass.Layer( ERenderLayer::PostProcess );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// visibility buffer as subpass
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "VisBuf1" );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
			{
				pass.AddPipeline( "perf/Culling/2-VisBuf1-p0.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-VisBuf1-p0.as)
				pass.Output( "out_VisBuf",	vis,	RGBA32u(~0) );
				pass.Output(				ds,		DepthStencil(1.0, 0) );
			}
			pass.NextSubpass( "resolve" );
			{
				pass.AddPipeline( "perf/Culling/2-VisBuf1-p1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-VisBuf1-p1.as)
				pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
				pass.Input(  "in_VisBuf",	vis,	"out_VisBuf" );
				pass.Constant( "iLight",	light_dir );
				pass.Layer( ERenderLayer::PostProcess );
			}
		}

		// visibility buffer v2 as subpass
		if ( GetFeatureSet().hasFragmentShaderBarycentric() )
		{
			++mode_id;
			RC<SceneGraphicsPass>	pass = scene_direct_draw.AddGraphicsPass( "VisBuf2" );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
			{
				pass.AddPipeline( "perf/Culling/2-VisBuf2-p0.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-VisBuf2-p0.as)
				pass.Output( "out_VisBuf",	vis,	RGBA32u(~0) );
				pass.Output( "out_VisBuf2",	vis2,	RGBA32f(0.0) );
				pass.Output(				ds,		DepthStencil(1.0, 0) );
			}
			pass.NextSubpass( "resolve" );
			{
				pass.AddPipeline( "perf/Culling/2-VisBuf2-p1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-VisBuf2-p1.as)
				pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
				pass.Input(  "in_VisBuf",	vis,	"out_VisBuf" );
				pass.Input(  "in_VisBuf2",	vis2,	"out_VisBuf2" );
				pass.Constant( "iLight",	light_dir );
				pass.Layer( ERenderLayer::PostProcess );
			}
		}

		// reset indirect buffer
		{
			RC<ComputePass>		pass = ComputePass( "", "RESET_INDIRECT_BUF" );
			pass.ArgOut(	"un_IndirectCmd",	indirect_buf );
			pass.Constant(	"iIndexCount",		index_count );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			//pass.EnableIfGreater( mode, mode_id );
		}

		// raster culling
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_aabb.AddGraphicsPass( "raster cull" );
			pass.AddPipeline( "perf/Culling/RasterCull.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/RasterCull.as)
			pass.OutputLS(					ds,		EAttachmentLoadOp::Load,	EAttachmentStoreOp::None );
			pass.ArgInOut( "un_VisFlags",	vis_flags );
			pass.EnableIfEqual( mode, mode_id );
			//pass.Repeat( repeat );  // incorrect time on multiple passes
		}{
			RC<ComputePass>		pass = ComputePass( "", "CHECK_VIS_FLAGS" );
			pass.ArgInOut(  "un_VisFlags",		vis_flags );		// read and set zero
			pass.ArgOut(	"un_RemapIdx",		remap_idx );		// override
			pass.ArgInOut(	"un_IndirectCmd",	indirect_buf );		// atomic
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
			pass.EnableIfEqual( mode, mode_id );
		}{
			RC<SceneGraphicsPass>	pass = scene_indirect_draw.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/Culling/2-DepthTest.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_RemapIdx",	remap_idx );
			pass.Constant( "iLight",	light_dir );
			pass.Constant( "iRemapIdx",	1 );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// HiZ Culling
		const uint	hiz_mode = mode_id;
		{
			RC<ComputePass>		pass = ComputePass( "", "CULL_OBJECTS;USE_REDUCTION="+has_minmax_sampler );
			pass.ArgIn(		"un_Objects",		obj_buf );
			pass.ArgInOut(	"un_IndirectCmd",	indirect_buf );
			pass.ArgInOut(	"un_RemapIdx",		remap_idx );
			pass.ArgIn(		"un_DepthPyramid",	pyramid,	(has_minmax_sampler ? Sampler_MaxLinearClamp : Sampler_NearestClamp) );
			pass.Constant(	"iPyramidDim",		dim.ToFloat2() );
			pass.Slider(	"iCullMode",		0,		2,		2 );		// 0 - none, 1 - frustum, 2 - HiZ
			pass.Set( camera );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
			pass.EnableIfGreater( mode, hiz_mode );
		}

		// HiZ
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_indirect_draw.AddGraphicsPass( "HiZ" );
			pass.AddPipeline( "perf/Culling/2-DepthTest.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_RemapIdx",	remap_idx );
			pass.Constant( "iLight",	light_dir );
			pass.Constant( "iRemapIdx",	1 );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// HiZ + depth pre-pass
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_indirect_draw.AddGraphicsPass( "DPP" );
			pass.AddPipeline( "perf/Culling/2-DepthPrePass.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthPrePass.as)
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_RemapIdx",	remap_idx );
			pass.Constant( "iRemapIdx",	1 );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene_indirect_draw.AddGraphicsPass( "HiZ draw" );
			pass.AddPipeline( "perf/Culling/2-DepthEqual.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthEqual.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds );
			pass.ArgIn(  "un_RemapIdx",	remap_idx );
			pass.Constant( "iLight",	light_dir );
			pass.Constant( "iRemapIdx",	1 );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}

		// HiZ + depth pre-pass as subpass
		/*++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene_indirect_draw.AddGraphicsPass( "HiZ + DPP subpass" );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
			{
				pass.AddPipeline( "perf/Culling/2-DepthPrePass-p0.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthPrePass-p0.as)
				pass.Output(				ds,		DepthStencil(1.0, 0) );
				pass.ArgIn(  "un_RemapIdx",	remap_idx );
				pass.Constant( "iRemapIdx",	1 );
			}
			pass.NextSubpass( "HiZ draw" );
			{
				pass.AddPipeline( "perf/Culling/2-DepthEqual-p1.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/2-DepthEqual-p1.as)
				pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
				pass.Output(				ds );
				pass.ArgIn(  "un_RemapIdx",	remap_idx );
				pass.Constant( "iLight",	light_dir );
				pass.Constant( "iRemapIdx",	1 );
			}
		}*/

		// calculate HiZ
		{
			// see [GenHiZ-1](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/GenHiZ-1.as) and [GenHiZ-2](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/GenHiZ-2.as)
			// for simplification reprojection from previous frame is not used

			RC<Postprocess>		pass = Postprocess( "", "MIPMAP_0" );	// non-POT to POT image
			pass.Output( "out_Color",	pyramid );
			pass.ArgIn(  "un_Depth",	ds,		Sampler_NearestClamp );
			pass.EnableIfGreater( mode, hiz_mode );
		}{
			RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP;USE_REDUCTION="+has_minmax_sampler );
			pass.Variable( "un_InImage",	"un_OutImage",	pyramid,	(has_minmax_sampler ? Sampler_MaxLinearClamp : Sampler_NearestClamp) );
			pass.EnableIfGreater( mode, hiz_mode );
		}


		Slider( mode,		"Mode",			0,	mode_id,	2 );
		Slider( obj_count,	"ObjCount",		1,	10 );
		Slider( repeat,		"Repeat",		1,	30 );

		Label( pyramid_dim,	"PyramidDim" );
		Label( count,		"Sphere count" );
		Label( tris_count,	"Triangles" );

		RC<DynamicUInt>		vis_objects = DynamicUInt();
		ReadBuffer( vis_objects, indirect_buf, "cmd.instanceCount" );
		Label( vis_objects,	"Visible spheres" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define PUT_OBJECTS
#	define CULL_OBJECTS
#	define MIPMAP_0
#	define GEN_MIPMAP
#	define USE_REDUCTION	1
#	define CHECK_VIS_FLAGS
#	define RESET_INDIRECT_BUF
#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		ObjectTransform		obj;
		const uint			idx				= GetGlobalIndex();
		const float3		inv_size		= 1.0 / float3(GetGlobalSize());
		const float			aspect_ratio	= float(iDimension.x) / float(iDimension.y);

		obj.scale = 1.0 + DHash11( idx * 11.0 );

		float3	unorm	= GetGlobalCoordUNorm();
		float3	seed	= unorm;

		if ( iBackToFront == 1 )
			unorm.xy = 1.0 - unorm.xy;

		unorm.z   = ToSNorm( unorm.z ) + ToSNorm( DHash13( 111.0 * seed )) * 2.0 * inv_size.z;
		unorm.xy += ToSNorm( DHash23( 222.0 * seed )) * 0.4 * inv_size.xy;

		unorm.xy *= aspect_ratio;
		unorm.z  *= Length( unorm.xy ) * 2.1 + 0.3;
		unorm.xy -= RemapClamp( float2(0.0, 4.0), float2(-0.1, 0.25), aspect_ratio );

		obj.position.xz = unorm.xy * 50.0;
		obj.position.y  = unorm.z * 10.0;

		// emulate LOD
		obj.scale *= Clamp( Length( unorm.xy ) * 1.1 + 0.1, 0.1, 4.0 );

		// change triangle density, required for low-end devices
		obj.scale *= iRadius;

		obj.position = QMul( QRotationY(ToRad(-45.0)), obj.position );

		obj.color = packUnorm4x8( RainbowWrap( float(idx) / 5.0 ));

	  #if 0
		un_Objects.elements[idx] = obj;
	  #else
		// fix for Metal
		un_Objects.elements[idx].position	= obj.position;
		un_Objects.elements[idx].scale		= obj.scale;
		un_Objects.elements[idx].color		= obj.color;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RESET_INDIRECT_BUF
	#include "IndirectCmd.glsl"

	void  Main ()
	{
	  #if 0
		un_IndirectCmd.cmd = DrawIndexedIndirectCommand_Create( iIndexCount );
	  #else
		// fix for Metal
		un_IndirectCmd.cmd.indexCount		= iIndexCount;
		un_IndirectCmd.cmd.instanceCount	= 1;
		un_IndirectCmd.cmd.firstIndex		= 0;
		un_IndirectCmd.cmd.vertexOffset		= 0;
		un_IndirectCmd.cmd.firstInstance	= 0;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_OBJECTS
	#include "IndirectCmd.glsl"
	#include "InvocationID.glsl"
	#include "Frustum.glsl"
	#include "Matrix.glsl"
	#include "Sphere.glsl"


	bool  IsVisible (uint idx)
	{
		if ( iCullMode == 0 )
			return true;

		ObjectTransform		obj				= un_Objects.elements[idx];
		const float3		sphere_center	= (un_PerPass.camera.view * float4(obj.position - un_PerPass.camera.pos, 0.0)).xyz;
		const float			sphere_radius	= obj.scale;
		const float			znear			= un_PerPass.camera.clipPlanes.x;

		// frustum culling
		if ( ! Frustum_IsVisible( un_PerPass.camera.frustum, obj.position - un_PerPass.camera.pos, sphere_radius ))
			return false;

		if ( iCullMode == 1 )
			return true;

		// see [ProjectSphere test](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/ProjectSphere.as)
		// and [Quad shader](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ProjectSphere.as)
		if ( sphere_center.z - sphere_radius < znear )
			return true;  // too close to camera

		float4	aabb = Sphere_FastProject( Sphere_Create( sphere_center, sphere_radius ), un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );
				aabb = ToUNorm( aabb );	// to uv space

		// see [DepthPyramidCulling test](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/DepthPyramidCulling.as)
		float2	size		= float2( aabb.z - aabb.x, aabb.w - aabb.y ) * iPyramidDim;
		float2	center		= (aabb.xy + aabb.zw) * 0.5;
		float	level		= Ceil( Log2( MaxOf( size )));

	  #if USE_REDUCTION
		float	max_depth	= gl.texture.SampleLod( un_DepthPyramid, center, level ).r;		// normalized clip space
	  #else
		float2	c			= center - 0.5 / float2(Max( int2(iPyramidDim) >> int(level), 1 ));
		float	max_depth	=				  gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(0,0) ).r;
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(1,0) ).r );
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(0,1) ).r );
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(1,1) ).r );
	  #endif

		float	sphere_depth = FastProjectZ( un_PerPass.camera.proj, sphere_center.z - sphere_radius );	// normalized clip space
		bool	visible		= sphere_depth <= max_depth;

		if ( iCullMode == 2 )
			return visible;

		return false;  // unknown mode
	}


	void  Main ()
	{
		const uint	idx		= GetGlobalIndex();
		const uint	count	= GetGlobalIndexSize();

	#ifdef AE_shader_subgroup_ballot

		bool	is_visible		= IsVisible( idx );
		uint	dst_idx			= 0;
		uint4	visible_mask	= gl.subgroup.Ballot( is_visible );
		uint	visible_count	= gl.subgroup.BallotBitCount( visible_mask );

		if ( gl.subgroup.Index == 0 )
		{
			dst_idx = gl.AtomicAdd( INOUT un_IndirectCmd.cmd.instanceCount, visible_count );
		}
		gl.subgroup.ExecutionBarrier();	// reconvergence

		dst_idx = gl.subgroup.Broadcast( dst_idx, 0 );
		dst_idx += gl.subgroup.BallotExclusiveBitCount( visible_mask );

		if ( is_visible )
			un_RemapIdx.elements[dst_idx].newIndex = idx;

	#else

		if ( IsVisible( idx ))
		{
			uint	dst_idx = gl.AtomicAdd( INOUT un_IndirectCmd.cmd.instanceCount, 1 );

			un_RemapIdx.elements[dst_idx].newIndex = idx;
		}

	#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CHECK_VIS_FLAGS
	#include "IndirectCmd.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const int	idx			= GetGlobalIndex();
		const bool	is_visible	= (un_VisFlags.elements[ idx ].visible != 0);

		un_VisFlags.elements[ idx ].visible = 0;

	#ifdef AE_shader_subgroup_ballot

		uint	dst_idx		= 0;
		uint4	visible_mask	= gl.subgroup.Ballot( is_visible );
		uint	visible_count	= gl.subgroup.BallotBitCount( visible_mask );

		if ( gl.subgroup.Index == 0 )
		{
			dst_idx = gl.AtomicAdd( INOUT un_IndirectCmd.cmd.instanceCount, visible_count );
		}
		gl.subgroup.ExecutionBarrier();	// reconvergence

		dst_idx = gl.subgroup.Broadcast( dst_idx, 0 );
		dst_idx += gl.subgroup.BallotExclusiveBitCount( visible_mask );

		if ( is_visible )
			un_RemapIdx.elements[dst_idx].newIndex = idx;

	#else

		if ( is_visible )
		{
			uint	dst_idx = gl.AtomicAdd( INOUT un_IndirectCmd.cmd.instanceCount, 1 );

			un_RemapIdx.elements[dst_idx].newIndex = idx;
		}

	#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MIPMAP_0
	#include "InvocationID.glsl"

	void  Main ()
	{
		int2	dim		= gl.texture.GetSize( un_Depth, 0 );

		float2	uv0		= (GetGlobalCoordUF().xy * un_PerPass.invResolution.xy) * dim;
		float2	uv1		= ((GetGlobalCoordUF().xy + 1.0) * un_PerPass.invResolution.xy) * dim;

		float	d		= -1.0;

		int2	c0		= Max( int2(uv0), int2(0) );
		int2	c1		= Min( int2(uv1), dim-1 );

		for (int y = c0.y; y <= c1.y; ++y)
		for (int x = c0.x; x <= c1.x; ++x)
		{
			d = Max( d, gl.texture.Fetch( un_Depth, int2(x,y), 0 ).r );
		}

		out_Color = float4(d);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MIPMAP
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4	c;
		#if USE_REDUCTION
			float2	uv = (float2(GetGlobalCoord().xy) + 0.5) / float2(gl.image.GetSize( un_OutImage ));
			c  = gl.texture.SampleLod( un_InImage, uv, 0.0 );
		#else
			int2	p = GetGlobalCoord().xy * 2;
			c = gl.texture.Fetch( un_InImage, p, 0 );
			c = Max( c, gl.texture.Fetch( un_InImage, p + int2(1,0), 0 ));
			c = Max( c, gl.texture.Fetch( un_InImage, p + int2(0,1), 0 ));
			c = Max( c, gl.texture.Fetch( un_InImage, p + int2(1,1), 0 ));
		#endif
		gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
	}

#endif
//-----------------------------------------------------------------------------
