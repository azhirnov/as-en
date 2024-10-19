// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Core/EditorUI.h"

#include "res_editor/Scripting/ScriptBasePass.cpp.h"
#include "res_editor/_data/cpp/types.h"

#include "res_editor/Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
	using DebugModeBits = EnumSet<IPass::EDebugMode>;

/*
=================================================
	InputController
=================================================
*/
	void  ScriptScene::InputController (const ScriptBaseControllerPtr &value) __Th___
	{
		CHECK_THROW_MSG( value );

		_controller = value;
	}

/*
=================================================
	InputGeometry*
=================================================
*/
	void  ScriptScene::InputGeometry1 (const ScriptGeomSourcePtr &geom, const packed_float3 &pos, const packed_float3 &rotation, float scale) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	=	float4x4::RotateX( Rad{rotation.x} )	*
							float4x4::RotateY( Rad{rotation.y} )	*
							float4x4::RotateZ( Rad{rotation.z} )	*
							float4x4::Translated( pos )				*
							float4x4::Scaled( scale );
	}

	void  ScriptScene::InputGeometry2 (const ScriptGeomSourcePtr &geom, const packed_float3 &pos) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	= float4x4::Translated( pos );
	}

	void  ScriptScene::InputGeometry3 (const ScriptGeomSourcePtr &geom) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	= float4x4::Identity();
	}

	void  ScriptScene::InputGeometry4 (const ScriptGeomSourcePtr &geom, const packed_float4x4 &mat) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	= float4x4{mat};
	}

/*
=================================================
	AddGraphicsPass
=================================================
*/
	ScriptSceneGraphicsPass*  ScriptScene::AddGraphicsPass (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _geomInstances.empty() );

		++_passCount;
		return ScriptSceneGraphicsPassPtr{ new ScriptSceneGraphicsPass{ ScriptScenePtr{this}, name }}.Detach();
	}

/*
=================================================
	AddRayTracingPass
=================================================
*/
	ScriptSceneRayTracingPass*  ScriptScene::AddRayTracingPass (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _geomInstances.empty() );

		++_passCount;
		_hasRayTracingPass = true;
		return ScriptSceneRayTracingPassPtr{ new ScriptSceneRayTracingPass{ ScriptScenePtr{this}, name }}.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptScene::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptScene>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Attach geometry to scene." );
		binder.AddMethod( &ScriptScene::InputGeometry1,		"Add",					{"geometry", "position", "rotationInRads", "scale"} );
		binder.AddMethod( &ScriptScene::InputGeometry2,		"Add",					{"geometry", "position"} );
		binder.AddMethod( &ScriptScene::InputGeometry3,		"Add",					{"geometry"} );
		binder.AddMethod( &ScriptScene::InputGeometry4,		"Add",					{"geometry", "transform"} );

		binder.Comment( "Set camera to scene." );
		binder.AddMethod( &ScriptScene::InputController,	"Set",					{"controller"} );

		binder.Comment( "Add graphics pass. It will link geometries with pipelines and draw it." );
		binder.AddMethod( &ScriptScene::AddGraphicsPass,	"AddGraphicsPass",		{"name"} );
		binder.AddMethod( &ScriptScene::AddRayTracingPass,	"AddRayTracingPass",	{"name"} );
	}

/*
=================================================
	ToScene
=================================================
*/
	RC<SceneData>  ScriptScene::ToScene () __Th___
	{
		if ( _scene )
			return _scene;

		CHECK_THROW_MSG( not _geomInstances.empty() );
		CHECK_THROW_MSG( _passCount > 0 );

		if ( _hasRayTracingPass )
		{
			for (auto& inst : _geomInstances) {
				for (auto i : IndicesOnly<ScriptGeomSource::EGeometryType>()) {
					Unused( inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType(i) ));
				}
			}
		}

		_scene = MakeRC<SceneData>();

		for (auto& src : _geomInstances)
		{
			auto	geom	= src.geom->ToGeomSource();  // throw
			CHECK_THROW( geom );

			auto&	dst		= _scene->_geomInstances.emplace_back();
			dst.transform	= src.transform;
			dst.geometry	= geom;
		}

		return _scene;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptSceneGraphicsPass::ScriptSceneGraphicsPass (ScriptScenePtr scene, const String &passName) __Th___ :
		_scene{scene}, _passName{passName}
	{
		_dbgName = passName;

		StringToColor( OUT _dbgColor, StringView{_dbgName} );

		ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );
	}

/*
=================================================
	AddPipeline
=================================================
*/
	void  ScriptSceneGraphicsPass::AddPipeline (const String &pplnFile) __Th___
	{
		Path	file_path = ScriptExe::ScriptPassApi::ToPipelinePath( Path{pplnFile} );  // throw

		if ( _uniquePplns.insert( file_path ).second )
			_pipelines.push_back( file_path );
	}

/*
=================================================
	AddPipelines
=================================================
*/
	void  ScriptSceneGraphicsPass::AddPipelines (const String &pplnsFolder) __Th___
	{
		Path	folder = ScriptExe::ScriptPassApi::ToPipelineFolder( Path{pplnsFolder} );  // throw

		for (auto& path : FileSystem::EnumRecursive( folder )) {
			if ( _uniquePplns.insert( path ).second )
				_pipelines.push_back( path );
		}
	}

/*
=================================================
	SetLayer
=================================================
*/
	void  ScriptSceneGraphicsPass::SetLayer (ERenderLayer layer) __Th___
	{
		_renderLayer = layer;
	}

/*
=================================================
	SetFragmentShadingRate
=================================================
*/
	void  ScriptSceneGraphicsPass::SetFragmentShadingRate (EShadingRate rate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp) __Th___
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		CHECK_THROW_MSG( fs.pipelineFragmentShadingRate		== FeatureSet::EFeature::RequireTrue	or
						 fs.primitiveFragmentShadingRate	== FeatureSet::EFeature::RequireTrue	or
						 fs.attachmentFragmentShadingRate	== FeatureSet::EFeature::RequireTrue,
			"Fragment shading rate is not supported." );

		_shadingRate.rate			= rate;
		_shadingRate.primitiveOp	= primitiveOp;
		_shadingRate.textureOp		= textureOp;
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptSceneGraphicsPass::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
	{
		arg.state |= EResourceState::FragmentShader;
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptSceneGraphicsPass::ToPass () C_Th___
	{
		CHECK_THROW_MSG( not _pipelines.empty(), "pipelines must be defined" );

		RC<SceneGraphicsPass>	result		= MakeRC<SceneGraphicsPass>();
		auto&					res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&					materials	= result->_materials;
		const auto				max_frames	= GraphicsScheduler().GetMaxFrames();
		PipelinesPerInstance_t	pplns_per_inst;
		DebugModeBits			dbg_modes;

		result->_rtech	= _CompilePipelines( OUT pplns_per_inst, OUT result->_scene );	// throw

		CHECK_THROW( pplns_per_inst.size() == _scene->_geomInstances.size() );
		materials.reserve( pplns_per_inst.size() );

		for (usize i = 0; i < pplns_per_inst.size(); ++i)
		{
			const auto&		geom	= _scene->_geomInstances[i].geom;
			const auto&		pplns	= pplns_per_inst[i];
			auto			mtr		= geom->ToMaterial( _renderLayer, result->_rtech.rtech, pplns );  // throw
			CHECK_THROW( mtr );
			materials.push_back( mtr );
			dbg_modes |= mtr->GetDebugModeBits();
		}

		result->_renderLayer	= this->_renderLayer;
		result->_shadingRate	= this->_shadingRate;

		result->_ubuffer = _CreateUBuffer( SizeOf<ShaderTypes::SceneGraphicsPassUB>, "SceneGraphicsPassUB",
											EResourceState::UniformRead | EResourceState::AllGraphicsShaders );  // throw

		// create descriptor set
		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_descSets.data(), max_frames, result->_rtech.packId, DSLayoutName{"pass.ds"}, null, _dbgName ));
		_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw

		uint	min_layer_count = UMax;
		for (auto& src : _output)
		{
			const uint	layers	= src.rt->Description().arrayLayers.Get();
			const uint	count	= (src.layerCount == UMax ? (layers - src.layer.Get()) : src.layerCount);

			CHECK_THROW( src.layer.Get() < layers );
			CHECK_THROW( src.layer.Get() + count <= layers );

			AssignMin( INOUT min_layer_count, count );
		}
		CHECK_THROW( min_layer_count > 0 );

		result->_wScaling				= _wScaling;
		result->_scissors				= _scissors;
		result->_rpDesc.renderPassName	= RenderPassName{"rp"};
		result->_rpDesc.subpassName		= SubpassName{"main"};
		result->_rpDesc.packId			= result->_rtech.packId;
		result->_rpDesc.layerCount		= ImageLayer{min_layer_count};
		result->_rpDesc.area			= RectI{0,0,1,1};
		result->_rpDesc.viewports		= _viewports;

		if ( result->_rpDesc.viewports.empty() )
			result->_rpDesc.AddViewport( RectF{0.f, 0.f, 1.f, 1.f}, _depthRange.x, _depthRange.y );

		for (usize i = 0; i < _output.size(); ++i)
		{
			auto&	src	= _output[i];
			auto	rt	= src.rt->ToResource();
			CHECK_THROW( rt );

			ImageViewDesc		view;
			view.viewType		= (min_layer_count > 1 ? EImage_2DArray : EImage_2D);
			view.baseLayer		= src.layer;
			view.layerCount		= ushort(min_layer_count);
			view.baseMipmap		= src.mipmap;
			view.mipmapCount	= 1;

			rt = rt->CreateView( view, rt->GetName() );
			CHECK_THROW( rt );

			auto&	dst = result->_renderTargets.emplace_back();
			dst.name	= AttachmentName{src.name};
			dst.image	= rt;
			dst.clear	= src.clear;
		}
		CHECK_THROW( not result->_renderTargets.empty() );

		_Init( *result, _scene->GetController() );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::AllGraphics );

		return result;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptSceneGraphicsPass::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		{
			EnumBinder<ERenderLayer>	binder{ se };
			binder.Create();
			binder.AddValue( "Opaque",		ERenderLayer::Opaque );
			binder.AddValue( "Translucent",	ERenderLayer::Translucent );
			binder.AddValue( "PostProcess",	ERenderLayer::PostProcess );
			StaticAssert( uint(ERenderLayer::_Count) == 3 );
		}
		{
			ClassBinder<ScriptSceneGraphicsPass>	binder{ se };
			binder.CreateRef( 0, False{"no ctor"} );

			_BindBase( binder, True{"with args"} );
			_BindBaseRenderPass( binder, False{"without blending"}, False{"without RWAttachment"} );

			binder.Comment( "Add path to single pipeline or folder with pipelines.\n"
							"Scene geometry will be linked with compatible pipeline or error will be generated." );
			binder.AddMethod( &ScriptSceneGraphicsPass::AddPipeline,			"AddPipeline",			{"pplnFile"} );
			binder.AddMethod( &ScriptSceneGraphicsPass::AddPipelines,			"AddPipelines",			{"pplnFolder"} );

			binder.AddMethod( &ScriptSceneGraphicsPass::SetLayer,				"Layer",				{} );
			binder.AddMethod( &ScriptSceneGraphicsPass::SetFragmentShadingRate,	"FragmentShadingRate",	{"rate", "primitiveOp", "textureOp"} );

			binder.Comment( "Can be used only if pass hasn't attachments." );
			binder.AddMethod( &ScriptSceneGraphicsPass::_SetDynamicDimension2,	"SetDimension",			{} );
		}
	}

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptSceneGraphicsPass::_CreateUBType () __Th___
	{
		using namespace AE::PipelineCompiler;

		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "SceneGraphicsPassUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"SceneGraphicsPassUB"}};
		st->Set( EStructLayout::Std140, R"#(
				// view //
				float2		resolution;				// viewport resolution (in pixels)
				float		time;					// shader playback time (in seconds)
				float		timeDelta;				// frame render time (in seconds), max value: 1/30s
				uint		frame;					// shader playback frame, global frame counter
				uint		seed;					// unique value, updated on each shader reloading

				// controller //
				CameraData	camera;

				// sliders //
				float4		floatSliders [8];
				int4		intSliders [8];
				float4		colors [8];

				// constants //
				float4		floatConst [8];
				int4		intConst [8];
			)#");

		StaticAssert( UIInteraction::MaxSlidersPerType == 8 );
		StaticAssert( IPass::Constants::MaxCount == 8 );
		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptSceneGraphicsPass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}

/*
=================================================
	_CompilePipelines2
=================================================
*/
	void  ScriptSceneGraphicsPass::_CompilePipelines2 (ScriptEnginePtr se, OUT PipelinesPerInstance_t &pplnNames) C_Th___
	{
		using namespace AE::PipelineCompiler;

		_args.ValidateArgs();

		auto&			storage = *ObjectStorage::Instance();
		const String	subpass = "main";

		CompatibleRenderPassDescPtr	compat_rp{ new CompatibleRenderPassDesc{ "compat.rp" }};
		compat_rp->AddSubpass( subpass );
		{
			for (usize i = 0; i < _output.size(); ++i)
			{
				RPAttachmentPtr		att		= compat_rp->AddAttachment2( _output[i].name );
				const auto			desc	= _output[i].rt->ToResource()->GetImageDesc();
				const bool			is_ds	= _output[i].rt->IsDepthOrStencil();

				att->format		= desc.format;
				att->samples	= desc.samples;
				att->AddUsage( subpass, is_ds ? EAttachment::DepthStencil : EAttachment::Color );
			}
		}{
			RenderPassSpecPtr	rp_spec = compat_rp->AddSpecialization2( "rp" );
			
			for (auto [out, i] : WithIndex(_output))
			{
				RPAttachmentSpecPtr	att		= rp_spec->AddAttachment2( out.name );
				const bool			is_ds	= out.rt->IsDepthOrStencil();
				const auto			state	= is_ds ?
												EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS :
												EResourceState::ColorAttachment;
				att->loadOp		= out.loadOp;
				att->storeOp	= out.storeOp;
				
				if ( out.loadOp == EAttachmentLoadOp::Clear )
					att->AddLayout( "ExternalIn", EResourceState::Invalidate | state );
				
				att->AddLayout( subpass, state );
			}
		}


		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTGraphicsPassPtr	pass = rtech->AddGraphicsPass2( subpass );
			pass->SetRenderPass( "rp", subpass );
		}

		{
			Unused( _CreateUBType() );	// throw

			DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "pass.ds" }};
			const auto				stage	= EShaderStages::AllGraphics;

			ds_layout->AddUniformBuffer( EShaderStages::AllGraphics, "un_PerPass", ArraySize{1}, "SceneGraphicsPassUB", EResourceState::ShaderUniform, False{} );
			_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw

			String	str;
			_AddSlidersAsMacros( OUT str );
			ds_layout->Define( str );
		}

		for (auto& inst : _scene->_geomInstances) {
			inst.geom->AddLayoutReflection();  // throw
		}

		auto	include_dirs = ScriptExe::ScriptPassApi::GetPipelineIncludeDirs();
		CHECK_THROW( storage.CompilePipeline( se, ScriptExe::ScriptPassApi::ToPipelinePath( "VertexInput.as" ), include_dirs ));

		if ( GraphicsScheduler().GetFeatureSet().bufferDeviceAddress == FeatureSet::EFeature::RequireTrue )
			CHECK_THROW( storage.CompilePipeline( se, ScriptExe::ScriptPassApi::ToPipelinePath( "ModelShared.as" ), include_dirs ));

		for (auto& ppln : _pipelines) {
			if ( not storage.CompilePipeline( se, ppln, include_dirs ))
				continue;
		}

		CHECK_THROW( not storage.gpipelines.empty() or
					 not storage.mpipelines.empty() );

		pplnNames.reserve( _scene->_geomInstances.size() );

		for (auto& inst : _scene->_geomInstances)
		{
			auto	names = inst.geom->FindMaterialGraphicsPipelines( _renderLayer );  // throw
			pplnNames.push_back( RVRef(names) );
		}
		ASSERT( _scene->_geomInstances.size() == pplnNames.size() );
	}

/*
=================================================
	_CompilePipelines
=================================================
*/
	RTechInfo  ScriptSceneGraphicsPass::_CompilePipelines (OUT PipelinesPerInstance_t &pplnNames, OUT RC<SceneData> &outScene) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this, &pplnNames, &outScene] (ScriptEnginePtr se)
					{
						outScene = _scene->ToScene();				// throw
						_CompilePipelines2( se, OUT pplnNames );	// throw
					});
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptSceneRayTracingPass::ScriptSceneRayTracingPass (ScriptScenePtr scene, const String &passName) __Th___ :
		_scene{scene}, _passName{passName}
	{
		_dbgName = passName;

		StringToColor( OUT _dbgColor, StringView{_dbgName} );

		ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );
	}

/*
=================================================
	SetPipeline
=================================================
*/
	void  ScriptSceneRayTracingPass::SetPipeline (const String &pplnFile) __Th___
	{
		_pipeline = ScriptExe::ScriptPassApi::ToPipelinePath( Path{pplnFile} );  // throw
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptSceneRayTracingPass::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
	{
		arg.state |= EResourceState::RayTracingShaders;
	}

/*
=================================================
	DispatchThreads*
=================================================
*/
	void  ScriptSceneRayTracingPass::DispatchThreads3v (const packed_uint3 &threads) __Th___
	{
		auto&	it	= _iterations.emplace_back();
		it.dim		= threads;
	}

	void  ScriptSceneRayTracingPass::DispatchThreadsDS (const ScriptDynamicDimPtr &ds) __Th___
	{
		CHECK_THROW_MSG( ds and ds->Get() );

		auto&	it	= _iterations.emplace_back();
		it.dim		= ds->Get();

		_SetDynamicDimension( ds );
	}

	void  ScriptSceneRayTracingPass::DispatchThreads1D (const ScriptDynamicUIntPtr &dyn) __Th___
	{
		CHECK_THROW_MSG( dyn );

		auto&	it	= _iterations.emplace_back();
		it.dim		= dyn->Get();
	}

/*
=================================================
	DispatchThreadsIndirect*
=================================================
*/
	void  ScriptSceneRayTracingPass::DispatchThreadsIndirect1 (const ScriptBufferPtr &ibuf) __Th___
	{
		DispatchThreadsIndirect2( ibuf, 0 );
	}

	void  ScriptSceneRayTracingPass::DispatchThreadsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset) __Th___
	{
		CHECK_THROW_MSG( ibuf );

		auto&	it			= _iterations.emplace_back();
		it.indirect			= ibuf;
		it.indirectOffset	= Bytes{offset};
	}

	void  ScriptSceneRayTracingPass::DispatchThreadsIndirect3 (const ScriptBufferPtr &ibuf, const String &field) __Th___
	{
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not field.empty() );

		auto&	it			= _iterations.emplace_back();
		it.indirect			= ibuf;
		it.indirectCmdField	= field;
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptSceneRayTracingPass::ToPass () C_Th___
	{
		CHECK_THROW_MSG( not _iterations.empty(), "add at least one Dispatch() call" );
		CHECK_THROW_MSG( not _args.Empty(), "empty argument list" );
		CHECK_THROW_MSG( not _pipeline.empty(), "pipeline is not defined" );

		RC<SceneRayTracingPass>	result		= MakeRC<SceneRayTracingPass>();
		auto&					res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto				max_frames	= GraphicsScheduler().GetMaxFrames();
		PipelineName			ppln_name;
		RTShaderBindingName		sbt_name;
		ScriptRTScenePtr		rt_scene;
		DebugModeBits			dbg_modes;

		result->_rtech		= _CompilePipelines( OUT ppln_name, OUT sbt_name, OUT result->_scene, OUT rt_scene );	// throw

		result->_pipeline	= result->_rtech.rtech->GetRayTracingPipeline( ppln_name );
		CHECK_THROW( result->_pipeline );

		#if PIPELINE_STATISTICS
		{
			auto&	res = res_mngr.GetResourcesOrThrow( result->_pipeline );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
		}
		#endif

		result->_sbt = result->_rtech.rtech->GetRTShaderBinding( sbt_name );
		CHECK_THROW( result->_sbt );

		result->_ubuffer = _CreateUBuffer( SizeOf<ShaderTypes::SceneRayTracingPassUB>, "SceneRayTracingPassUB",
											EResourceState::UniformRead | EResourceState::RayTracingShaders );  // throw

		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_passDSIndex, OUT result->_passDescSets.data(), max_frames, result->_pipeline,
													DescriptorSetName{"pass"}, null, _dbgName + "-PassDS" ));
		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_objDSIndex,  OUT result->_objDescSets.data(),  max_frames, result->_pipeline,
													DescriptorSetName{"material"}, null, _dbgName + "-MtrDS" ));

		_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw

		{
			auto	rt_scene_res = rt_scene->ToResource();
			CHECK_THROW( rt_scene_res );
			result->_resources.Add( UniformName{"un_RtScene"}, rt_scene_res, EResourceState::ShaderRTAS | EResourceState::RayTracingShaders );
		}

		result->_iterations.assign( this->_iterations.begin(), this->_iterations.end() );

		_Init( *result, _scene->GetController() );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::AllRayTracing );

		return result;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptSceneRayTracingPass::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptSceneRayTracingPass>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );

		_BindBase( binder, True{"with args"} );

		binder.Comment( "Set path to single pipeline.\n"
						"Scene geometry will be linked with compatible pipeline or error will be generated." );
		binder.AddMethod( &ScriptSceneRayTracingPass::SetPipeline,				"SetPipeline",		{"pplnFile"} );

		binder.Comment( "Run RayGen shader with specified number of threads." );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreads1,			"Dispatch",			{"threadsX"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreads2,			"Dispatch",			{"threadsX", "threadsY"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreads3,			"Dispatch",			{"threadsX", "threadsY", "threadsZ"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreads2v,		"Dispatch",			{"threads"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreads3v,		"Dispatch",			{"threads"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreadsDS,		"Dispatch",			{"dynamicThreadCount"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreads1D,		"Dispatch",			{"dynamicThreadCount"} );

		binder.Comment( "Run RayGen shader with number of threads from indirect command." );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreadsIndirect1,	"DispatchIndirect",	{"indirectBuffer"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreadsIndirect2,	"DispatchIndirect",	{"indirectBuffer", "indirectBufferOffset"} );
		binder.AddMethod( &ScriptSceneRayTracingPass::DispatchThreadsIndirect3,	"DispatchIndirect",	{"indirectBuffer", "indirectBufferFieldName"} );
	}

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptSceneRayTracingPass::_CreateUBType () __Th___
	{
		using namespace AE::PipelineCompiler;

		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "SceneRayTracingPassUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"SceneRayTracingPassUB"}};
		st->Set( EStructLayout::Std140, R"#(
				// view //
				float		time;					// shader playback time (in seconds)
				float		timeDelta;				// frame render time (in seconds), max value: 1/30s
				uint		frame;					// shader playback frame, global frame counter
				uint		seed;					// unique value, updated on each shader reloading

				// controller //
				CameraData	camera;

				// sliders //
				float4		floatSliders [8];
				int4		intSliders [8];
				float4		colors [8];

				// constants //
				float4		floatConst [8];
				int4		intConst [8];
			)#");

		StaticAssert( UIInteraction::MaxSlidersPerType == 8 );
		StaticAssert( IPass::Constants::MaxCount == 8 );
		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptSceneRayTracingPass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}

/*
=================================================
	_CompilePipelines2
=================================================
*/
	void  ScriptSceneRayTracingPass::_CompilePipelines2 (ScriptEnginePtr se, OUT PipelineName &pplnName, OUT RTShaderBindingName &sbtName) C_Th___
	{
		using namespace AE::PipelineCompiler;

		_args.ValidateArgs();

		auto&	storage = *ObjectStorage::Instance();


		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "main" );
		}

		{
			Unused( _CreateUBType() );	// throw

			DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "pass.ds" }};
			const auto				stage	= EShaderStages::AllRayTracing;

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "SceneRayTracingPassUB", EResourceState::ShaderUniform, False{} );
			ds_layout->AddRayTracingScene( stage, "un_RtScene", ArraySize{1} );
			_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw

			String	str;
			_AddSlidersAsMacros( OUT str );
			ds_layout->Define( str );
		}

		for (auto& inst : _scene->_geomInstances) {
			inst.geom->AddLayoutReflection();  // throw
		}

		auto	include_dirs = ScriptExe::ScriptPassApi::GetPipelineIncludeDirs();
		CHECK_THROW( storage.CompilePipeline( se, ScriptExe::ScriptPassApi::ToPipelinePath( "ModelShared.as" ), include_dirs ));
		CHECK_THROW( storage.CompilePipeline( se, _pipeline, include_dirs ));

		CHECK_THROW( storage.rtShaderBindings.size() == 1 );
		CHECK_THROW( storage.rtpipelines.size() == 1 );
		CHECK_THROW( storage.rtpipelines.begin()->second->GetSpecializations().size() == 1 );

		pplnName	= storage.rtpipelines.begin()->second->GetSpecializations()[0]->Name();
		sbtName		= RTShaderBindingName{storage.rtShaderBindings.begin()->second->Name()};
	}

/*
=================================================
	_CompilePipelines
=================================================
*/
	RTechInfo  ScriptSceneRayTracingPass::_CompilePipelines (OUT PipelineName &pplnName, OUT RTShaderBindingName &sbtName,
															 OUT RC<SceneData> &outScene, OUT ScriptRTScenePtr &rtScene) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[&] (ScriptEnginePtr se)
					{
						outScene = _scene->ToScene();							// throw
						_CompilePipelines2( se, OUT pplnName, OUT sbtName );	// throw
						_CreateRTScene( sbtName, OUT rtScene );
					});
	}

/*
=================================================
	_CreateRTScene
=================================================
*/
	void  ScriptSceneRayTracingPass::_CreateRTScene (RTShaderBindingName::Ref sbtName, OUT ScriptRTScenePtr &rt_scene) C_Th___
	{
		using namespace AE::PipelineCompiler;

		auto&	storage = *ObjectStorage::Instance();
		CHECK_THROW( storage.rtShaderBindings.size() == 1 );
		CHECK_THROW( sbtName == RTShaderBindingName{storage.rtShaderBindings.begin()->second->Name()} );

		rt_scene = ScriptRTScenePtr{new ScriptRTScene{}};
		rt_scene->MaxRayTypes( storage.rtShaderBindings.begin()->second->GetMaxRayTypes() );

		RTInstanceSBTOffset		opaque_sbt		{0};
		RTInstanceSBTOffset		translucent_sbt	{1};
		RTInstanceSBTOffset		volumetric_sbt	{2};	// use callable shader for different implementations

		// TODO: remove RTGeometry if opaque/dual-sided/translucent is not present in model

		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::Opaque );
			if ( not rt_geom ) continue;
			rt_scene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
									opaque_sbt, ERTInstanceOpt::ForceOpaque | ERTInstanceOpt::TriangleCullBack | ERTInstanceOpt::TriangleFrontCCW );
		}
		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::OpaqueDualSided );
			if ( not rt_geom ) continue;
			rt_scene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
									opaque_sbt, ERTInstanceOpt::ForceOpaque | ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW );
		}
		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::Translucent );
			if ( not rt_geom ) continue;
			rt_scene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
									translucent_sbt, ERTInstanceOpt::ForceNonOpaque | ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW );
		}
		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::Volumetric );
			if ( not rt_geom ) continue;
			rt_scene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
									volumetric_sbt, ERTInstanceOpt::ForceNonOpaque | ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW );
		}

		CHECK_THROW( rt_scene->ToResource() );
	}


} // AE::ResEditor
