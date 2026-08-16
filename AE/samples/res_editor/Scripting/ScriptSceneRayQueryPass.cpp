// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Core/EditorUI.h"
#include "Scripting/ScriptBasePass.cpp.h"

#define COMPUTE_PASS	ScriptSceneRayQueryPass
#include "Scripting/ComputePassShared.cpp.h"
#undef COMPUTE_PASS

#include "_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	ScriptSceneRayQueryPass::ScriptSceneRayQueryPass (ScriptScenePtr scene, const String &passName) __Th___ :
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
	void  ScriptSceneRayQueryPass::SetPipeline (const String &pplnFile) __Th___
	{
		_pplnPath = ScriptExe::ScriptPassApi::ToShaderPath( Path{pplnFile} );  // throw
	}

	void  ScriptSceneRayQueryPass::SetPipelineWithDefine (const String &defines) __Th___
	{
		_pplnPath = ScriptExe::ScriptPassApi::ToShaderPath( Path{} );  // throw
		_defines = defines;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptSceneRayQueryPass::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptSceneRayQueryPass>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		ScriptBasePass::_BindBase( binder, True{"withArgs"} );

		AS_METHOD( binder, ScriptSceneRayQueryPass::SetPipeline,				"SetPipeline",			{} );

		binder.Comment(	"Use current file as pipeline source.\n"
						"Format: MACROS = value; DEF; ..." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::SetPipelineWithDefine,		"SetPipelineWithDefine",	{"defines"} );

		binder.Comment( "Set workgroup size - number of threads which can access shared memory." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::LocalSize1,					"LocalSize",			{"x"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::LocalSize2,					"LocalSize",			{"x", "y"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::LocalSize3,					"LocalSize",			{"x", "y", "z"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::LocalSize2v,				"LocalSize",			{} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::LocalSize3v,				"LocalSize",			{} );

		binder.Comment( "Set subgroup size. Must be power of 2." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::SubgroupSize,				"SubgroupSize",			{} );

		binder.Comment( "Set minimal subgroup size. May not be power of 2.\n"
						"If device supports subgroup size control then subgroup size set to minimal supported, but not less than required.\n"
						"Will fail if maximal supported subgroup size < than required size." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::MinSubgroupSize,			"MinSubgroupSize",		{} );

		binder.Comment( "Execute compute shader with number of the workgroups.\n"
						"Total number of threads is 'groupCount * localSize'." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups1,			"DispatchGroups",		{"groupCountX"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups2,			"DispatchGroups",		{"groupCountX", "groupCountY"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups3,			"DispatchGroups",		{"groupCountX", "groupCountY", "groupCountZ"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups2v,			"DispatchGroups",		{"groupCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups3v,			"DispatchGroups",		{"groupCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroupsDS,			"DispatchGroups",		{"dynamicGroupCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups1D,			"DispatchGroups",		{"dynamicGroupCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups2D,			"DispatchGroups",		{"dynamicGroupCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroups3D,			"DispatchGroups",		{"dynamicGroupCount"} );

		binder.Comment( "Execute compute shader with total number of the threads." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads1,			"DispatchThreads",		{"threadsX"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads2,			"DispatchThreads",		{"threadsX", "threadsY"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads3,			"DispatchThreads",		{"threadsX", "threadsY", "threadsZ"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads2v,			"DispatchThreads",		{"threads"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads3v,			"DispatchThreads",		{"threads"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreadsDS,			"DispatchThreads",		{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads1D,			"DispatchThreads",		{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads2D,			"DispatchThreads",		{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchThreads3D,			"DispatchThreads",		{"dynamicThreadCount"} );

		binder.Comment( "Execute compute shader with indirect command.\n"
						"Indirect buffer must contains 'DispatchIndirectCommand' data." );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroupsIndirect1,	"DispatchGroupsIndirect",	{"indirectBuffer"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroupsIndirect2,	"DispatchGroupsIndirect",	{"indirectBuffer", "indirectBufferOffset"} );
		AS_METHOD( binder, ScriptSceneRayQueryPass::DispatchGroupsIndirect3,	"DispatchGroupsIndirect",	{"indirectBuffer", "indirectBufferField"} );
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptSceneRayQueryPass::_CompilePipeline (OUT RC<SceneData> &outScene, OUT ScriptRTScenePtr &rtScene) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[&] (ScriptEnginePtr se)
					{
						outScene = _scene->ToScene();		// throw
						_CompilePipeline2( se );			// throw
						_CreateRTScene( OUT rtScene );
					},
					_baseFlags );
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptSceneRayQueryPass::ToPass () __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() is not used" );
		CHECK_THROW_MSG( not _iterations.empty(), "add at least one Dispatch() call" );
		CHECK_THROW_MSG( not _args.Empty(), "empty argument list" );

		auto				result		= MakeRC<SceneRayQueryPass>();
		auto&				res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto			max_frames	= GraphicsScheduler().GetMaxFrames();
		ScriptRTScenePtr	rt_scene;

		result->_rtech = _CompilePipeline( OUT result->_scene, OUT rt_scene );	// throw

		EnumSet<IPass::EDebugMode>	dbg_modes;

		const auto	AddPpln = [this, cp = result.get(), &dbg_modes] (IPass::EDebugMode mode, EFlags flag, PipelineName::Ref name)
		{{
			if ( AllBits( _baseFlags, flag ))
			{
				auto	id = cp->_rtech.rtech->GetComputePipeline( name, True{"silent"} );
				if ( id ) {
					cp->_pipelines.insert_or_assign( mode, id );
					dbg_modes.insert( mode );
				}
			}
		}};

		AddPpln( IPass::EDebugMode::Unknown,		EFlags::Unknown,				PipelineName{"compute"} );
		AddPpln( IPass::EDebugMode::Asserts,		EFlags::Unknown,				PipelineName{"compute.Asserts"} );
		AddPpln( IPass::EDebugMode::Trace,			EFlags::Enable_ShaderTrace,		PipelineName{"compute.Trace"} );
		AddPpln( IPass::EDebugMode::FnProfiling,	EFlags::Enable_ShaderFnProf,	PipelineName{"compute.FnProf"} );
		AddPpln( IPass::EDebugMode::TimeHeatMap,	EFlags::Enable_ShaderTmProf,	PipelineName{"compute.TmProf"} );

		auto	ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second;

		#ifdef AE_ENABLE_VULKAN
		{
			auto&	res = res_mngr.GetResourcesOrThrow( ppln );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
		}
		#endif

		result->_localSize	= this->_localSize;
		result->_iterations.assign( this->_iterations.begin(), this->_iterations.end() );

		result->_ubuffer = _CreateUBuffer( SizeOf<ShaderTypes::ComputePassUB>, "ComputePassUB", EResourceState::UniformRead | EResourceState::ComputeShader );  // throw

		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_passDSIndex, OUT result->_passDescSets.data(), max_frames, ppln,
													DescriptorSetName{"pass"}, null, _dbgName + "-PassDS" ));
		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_objDSIndex,  OUT result->_objDescSets.data(),  max_frames, ppln,
													DescriptorSetName{"material"}, null, _dbgName + "-MtrDS" ));

		_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw

		{
			auto	rt_scene_res = rt_scene->ToResource();
			CHECK_THROW( rt_scene_res );
			result->_resources.Add( UniformName{"un_RtScene"}, rt_scene_res, EResourceState::ShaderRTAS | EResourceState::ComputeShader );
		}

		result->_pcIndex = res_mngr.GetPushConstantIndex<ShaderTypes::ComputePassPC>( ppln, PushConstantName{"pc"} );
		CHECK_THROW( result->_pcIndex );

		_Init( *result, _scene->GetController() );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Compute );

		return result;
	}


} // AE::ResEditor


#include "Scripting/PipelineCompiler.inl.h"

#include "base/DataSource/File.h"
#include "base/Algorithms/Parser.h"

#include "Scripting/ScriptImage.h"
#include "Scripting/ScriptVideoImage.h"

namespace AE::ResEditor
{
	using namespace AE::PipelineCompiler;

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptSceneRayQueryPass::GetShaderTypes (INOUT CppStructsFromShaders &) __Th___
	{
		// reuse ScriptComputePass
	}

/*
=================================================
	_CompilePipeline2
=================================================
*/
	void  ScriptSceneRayQueryPass::_CompilePipeline2 (ScriptEnginePtr se) C_Th___
	{
		_args.ValidateArgs();

		RenderTechniquePtr	rtech = RenderTechnique::Create( "rtech" );
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}

		// per-pass descriptor set layout
		{
			const auto				stage		= EShaderStages::Compute;
			DescriptorSetLayoutPtr	ds_layout	= DescriptorSetLayout::Create( "pass.ds" );

			Unused( ScriptComputePass::_CreateUBType() );	// throw

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ComputePassUB", EResourceState::ShaderUniform, False{} );
			ds_layout->AddRayTracingScene( stage, "un_RtScene", ArraySize{1} );
			_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw
		}

		uint	cs_line = 0;
		String	cs;
		{
			String	header;
			_AddDefines( _defines, INOUT header );
			_AddSliders( INOUT header );

			// load shader source from file
			{
				FileRStream		file {_pplnPath};
				CHECK_THROW_MSG( file.IsOpen(),
					"Failed to open shader file: '"s << ToString(_pplnPath) << "'" );

				CHECK_THROW_MSG( file.Read( file.RemainingSize(), OUT cs ),
					"Failed to read shader file '"s << ToString(_pplnPath) << "'" );

				header >> cs;
				cs_line = SubSat( uint(Parser::CalculateNumberOfLines( header )), 1u );
			}
		}

		for (auto& inst : _scene->_geomInstances) {
			inst.geom->AddLayoutReflection();  // throw
		}

		auto	include_dirs	= ScriptExe::ScriptPassApi::GetPipelineIncludeDirs();
		auto&	storage			= *ObjectStorage::Instance();
		CHECK_THROW( storage.CompilePipeline( se, ScriptExe::ScriptPassApi::ToPipelinePath( "ModelShared.as" ), include_dirs ));

		const auto		flags	 = UIInteraction::Instance().graphics->shaderFlags;
		EShaderOpt		sh_opt	 = Default;
		EPipelineOpt	ppln_opt = Default;

		if ( flags.contains( UIInteraction::EShaderFlags::DebugInfo ))
		{
			sh_opt = EShaderOpt::DebugInfo;
		}
		else
		if ( flags.contains( UIInteraction::EShaderFlags::Optimize ))
		{
			sh_opt   = EShaderOpt::Optimize;
			ppln_opt |= EPipelineOpt::Optimize;
		}

		if ( flags.contains( UIInteraction::EShaderFlags::CaptureStatistics ))
			ppln_opt |= EPipelineOpt::CaptureStatistics;

		if ( flags.contains( UIInteraction::EShaderFlags::CaptureInternalRepresentation ))
			ppln_opt |= EPipelineOpt::CaptureInternalRepresentation;

		StaticAssert( uint(UIInteraction::EShaderFlags::_Count) == 6 );

		_CompilePipeline3( cs, cs_line, "compute", uint(sh_opt), ppln_opt );

	  #ifdef AE_ENABLE_GLSL_TRACE
		if ( AllBits( _baseFlags, EFlags::Enable_ShaderAsserts )		or
			 flags.contains( UIInteraction::EShaderFlags::EnableAsserts ))
		{
			NOTHROW( _CompilePipeline3( cs, cs_line, "compute.Asserts", uint(sh_opt | EShaderOpt::Asserts), Default ));
		}
		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
			NOTHROW( _CompilePipeline3( cs, cs_line, "compute.Trace", uint(sh_opt | EShaderOpt::Trace), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
			NOTHROW( _CompilePipeline3( cs, cs_line, "compute.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
			NOTHROW( _CompilePipeline3( cs, cs_line, "compute.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default ));
	  #endif
	}

/*
=================================================
	_CompilePipeline3
=================================================
*/
	void  ScriptSceneRayQueryPass::_CompilePipeline3 (const String &cs, uint line, const String &pplnName,
														uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
	{
		PipelineLayoutPtr	ppln_layout;
		{
			auto&	storage	= *ObjectStorage::Instance();
			auto	it = storage.pplnLayouts.find( PipelineLayoutName{"rq-model.pl"} );
			CHECK_THROW( it != storage.pplnLayouts.end() );

			ppln_layout = PipelineLayout::Create( pplnName + ".pl", *it->second );
		}

		ppln_layout->AddPushConst2( "pc", ScriptComputePass::_CreatePCType(), EShader::Compute );

		if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
			ppln_layout->AddDebugDSLayout2( 1, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::Compute) );

		ComputePipelinePtr		ppln_templ = ComputePipelineScriptBinding::Create( pplnName );
		ppln_templ->Disable();
		ppln_templ->SetLayout2( ppln_layout );

		{
			ScriptShaderPtr	sh{ new ScriptShader{}};
			sh->SetSource2( EShader::Compute, cs, PathAndLine{_pplnPath, line} );
			sh->options = EShaderOpt(shaderOpts);
			sh->SetComputeLocalSize3( _localSize.x, _localSize.y, _localSize.z );

			ppln_templ->SetShader( sh );
		}
		{
			ComputePipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( pplnName );
			ppln_spec->Disable();
			ppln_spec->AddToRenderTech( "rtech", "Compute" );
			ppln_spec->SetOptions( pplnOpt );

			if ( _subgroupSize != 0 )
				ppln_spec->SetSubgroupSize( _subgroupSize );

			// if successfully compiled
			ppln_spec->Enable();
		}
	}

/*
=================================================
	_CreateRTScene
=================================================
*/
	void  ScriptSceneRayQueryPass::_CreateRTScene (OUT ScriptRTScenePtr &rtScene) C_Th___
	{
		using namespace AE::PipelineCompiler;

		rtScene = ScriptRTScenePtr{new ScriptRTScene{}};
		rtScene->MaxRayTypes( 1 );

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
