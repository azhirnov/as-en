// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Core/EditorUI.h"

#include "Scripting/ScriptBasePass.cpp.h"
#include "_data/cpp/types.h"

#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
	using DebugModeBits = EnumSet<IPass::EDebugMode>;

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
	void  ScriptSceneRayTracingPass::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) __Th___
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
	RC<IPass>  ScriptSceneRayTracingPass::ToPass () __Th___
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

		#ifdef AE_ENABLE_VULKAN
		{
			auto&	res = res_mngr.GetResourcesOrThrow( result->_pipeline );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
		}
		#endif

		result->_sbt = result->_rtech.rtech->GetRTShaderBinding( sbt_name );
		CHECK_THROW( result->_sbt );

		result->_ubuffer = _CreateUBuffer( SizeOf<ShaderTypes::ComputePassUB>, "ComputePassUB",
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
		AS_METHOD( binder, ScriptSceneRayTracingPass::SetPipeline,				"SetPipeline",		{"pplnFile"} );

		binder.Comment( "Run RayGen shader with specified number of threads." );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreads1,			"Dispatch",			{"threadsX"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreads2,			"Dispatch",			{"threadsX", "threadsY"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreads3,			"Dispatch",			{"threadsX", "threadsY", "threadsZ"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreads2v,		"Dispatch",			{"threads"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreads3v,		"Dispatch",			{"threads"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreadsDS,		"Dispatch",			{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreads1D,		"Dispatch",			{"dynamicThreadCount"} );

		binder.Comment( "Run RayGen shader with number of threads from indirect command." );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreadsIndirect1,	"DispatchIndirect",	{"indirectBuffer"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreadsIndirect2,	"DispatchIndirect",	{"indirectBuffer", "indirectBufferOffset"} );
		AS_METHOD( binder, ScriptSceneRayTracingPass::DispatchThreadsIndirect3,	"DispatchIndirect",	{"indirectBuffer", "indirectBufferFieldName"} );
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptSceneRayTracingPass::GetShaderTypes (INOUT CppStructsFromShaders &) __Th___
	{
		// reuse ScriptComputePass
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


		RenderTechniquePtr	rtech = RenderTechnique::Create( "rtech" );
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "main" );
		}

		{
			Unused( ScriptComputePass::_CreateUBType() );	// throw

			DescriptorSetLayoutPtr	ds_layout	= DescriptorSetLayout::Create( "pass.ds" );
			const auto				stage		= EShaderStages::AllRayTracing;

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ComputePassUB", EResourceState::ShaderUniform, False{} );
			ds_layout->AddRayTracingScene( stage, "un_RtScene", ArraySize{1} );
			_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw

			String	str;
			_AddSlidersAsMacros( INOUT str );
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
					},
					_baseFlags );
	}

/*
=================================================
	_CreateRTScene
=================================================
*/
	void  ScriptSceneRayTracingPass::_CreateRTScene (RTShaderBindingName::Ref sbtName, OUT ScriptRTScenePtr &rtScene) C_Th___
	{
		using namespace AE::PipelineCompiler;

		auto&	storage = *ObjectStorage::Instance();
		CHECK_THROW( storage.rtShaderBindings.size() == 1 );
		CHECK_THROW( sbtName == RTShaderBindingName{storage.rtShaderBindings.begin()->second->Name()} );

		rtScene = ScriptRTScenePtr{new ScriptRTScene{}};
		rtScene->MaxRayTypes( storage.rtShaderBindings.begin()->second->GetMaxRayTypes() );

		RTInstanceSBTOffset		opaque_sbt		{0};
		RTInstanceSBTOffset		translucent_sbt	{1};
		RTInstanceSBTOffset		volumetric_sbt	{2};	// use callable shader for different implementations

		// TODO: remove RTGeometry if opaque/dual-sided/translucent is not present in model

		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::Opaque );
			if ( not rt_geom ) continue;
			rtScene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
								  opaque_sbt, ERTInstanceOpt::ForceOpaque | ERTInstanceOpt::TriangleCullBack | ERTInstanceOpt::TriangleFrontCCW );
		}
		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::OpaqueDualSided );
			if ( not rt_geom ) continue;
			rtScene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
								  opaque_sbt, ERTInstanceOpt::ForceOpaque | ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW );
		}
		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::Translucent );
			if ( not rt_geom ) continue;
			rtScene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
								  translucent_sbt, ERTInstanceOpt::ForceNonOpaque | ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW );
		}
		for (auto& inst : _scene->_geomInstances)
		{
			auto	rt_geom = inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType::Volumetric );
			if ( not rt_geom ) continue;
			rtScene->AddInstance( rt_geom, float4x3{inst.transform}, RTInstanceCustomIndex{}, RTInstanceMask{},
								  volumetric_sbt, ERTInstanceOpt::ForceNonOpaque | ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW );
		}

		CHECK_THROW( rtScene->ToResource() );
	}


} // AE::ResEditor
