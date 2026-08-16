// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Core/EditorUI.h"
#include "Scripting/ScriptBasePass.cpp.h"

#define COMPUTE_PASS	ScriptComputePass
#include "Scripting/ComputePassShared.cpp.h"
#undef COMPUTE_PASS

#include "_data/cpp/types.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptComputePass*  ScriptComputePass_Ctor1 () {
		return ScriptComputePassPtr{ new ScriptComputePass{ Default, Default }}.Detach();
	}

	static ScriptComputePass*  ScriptComputePass_Ctor2 (const String &name) {
		return ScriptComputePassPtr{ new ScriptComputePass{ name, Default }}.Detach();
	}

	static ScriptComputePass*  ScriptComputePass_Ctor3 (const String &name, const String &defines) {
		return ScriptComputePassPtr{ new ScriptComputePass{ name, defines }}.Detach();
	}

} // namespace


/*
=================================================
	operator Iteration
=================================================
*/
	ScriptComputePass::Iteration::operator ComputePass::Iteration () C_Th___
	{
		ComputePass::Iteration	result;
		result.count	= count;
		result.isGroups	= isGroups;

		if ( indirect )
		{
			result.indirect			= indirect->ToResource();	// throw
			result.indirectOffset	= indirectOffset;

			if ( not indirectCmdField.empty() )
			{
				ASSERT( indirectOffset == 0 );
				result.indirectOffset = indirect->GetFieldOffset( indirectCmdField );	// throw
			}
		}
		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	ScriptComputePass::ScriptComputePass (const String &name, const String &defines) __Th___ :
		_pplnPath{ ScriptExe::ScriptPassApi::ToShaderPath( name )}
	{
		_dbgName = ToString( _pplnPath.stem() );
		_defines = defines;

		if ( not _defines.empty() )
			_dbgName << "|" << _defines;

		StringToColor( OUT _dbgColor, StringView{_dbgName} );
		FindAndReplace( INOUT _defines, '=', ' ' );

		CHECK_THROW_MSG( FileSystem::IsFile( _pplnPath ),
			"File '"s << name << "' is not exists" );

		ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptComputePass::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptComputePass>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		ScriptBasePass::_BindBase( binder, True{"withArgs"} );

		binder.AddFactoryCtor( &ScriptComputePass_Ctor1,	{} );
		binder.AddFactoryCtor( &ScriptComputePass_Ctor2,	{"shaderPath"} );
		binder.AddFactoryCtor( &ScriptComputePass_Ctor3,	{"shaderPath", "defines"} );

		binder.Comment( "Set workgroup size - number of threads which can access shared memory." );
		AS_METHOD( binder, ScriptComputePass::LocalSize1,				"LocalSize",			{"x"} );
		AS_METHOD( binder, ScriptComputePass::LocalSize2,				"LocalSize",			{"x", "y"} );
		AS_METHOD( binder, ScriptComputePass::LocalSize3,				"LocalSize",			{"x", "y", "z"} );
		AS_METHOD( binder, ScriptComputePass::LocalSize2v,				"LocalSize",			{} );
		AS_METHOD( binder, ScriptComputePass::LocalSize3v,				"LocalSize",			{} );

		binder.Comment( "Set subgroup size. Must be power of 2." );
		AS_METHOD( binder, ScriptComputePass::SubgroupSize,				"SubgroupSize",			{} );

		binder.Comment( "Set minimal subgroup size. May not be power of 2.\n"
						"If device supports subgroup size control then subgroup size set to minimal supported, but not less than required.\n"
						"Will fail if maximal supported subgroup size < than required size." );
		AS_METHOD( binder, ScriptComputePass::MinSubgroupSize,			"MinSubgroupSize",		{} );

		binder.Comment( "Execute compute shader with number of the workgroups.\n"
						"Total number of threads is 'groupCount * localSize'." );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups1,			"DispatchGroups",		{"groupCountX"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups2,			"DispatchGroups",		{"groupCountX", "groupCountY"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups3,			"DispatchGroups",		{"groupCountX", "groupCountY", "groupCountZ"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups2v,			"DispatchGroups",		{"groupCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups3v,			"DispatchGroups",		{"groupCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroupsDS,			"DispatchGroups",		{"dynamicGroupCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups1D,			"DispatchGroups",		{"dynamicGroupCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups2D,			"DispatchGroups",		{"dynamicGroupCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroups3D,			"DispatchGroups",		{"dynamicGroupCount"} );

		binder.Comment( "Execute compute shader with total number of the threads." );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads1,			"DispatchThreads",		{"threadsX"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads2,			"DispatchThreads",		{"threadsX", "threadsY"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads3,			"DispatchThreads",		{"threadsX", "threadsY", "threadsZ"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads2v,		"DispatchThreads",		{"threads"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads3v,		"DispatchThreads",		{"threads"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreadsDS,		"DispatchThreads",		{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads1D,		"DispatchThreads",		{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads2D,		"DispatchThreads",		{"dynamicThreadCount"} );
		AS_METHOD( binder, ScriptComputePass::DispatchThreads3D,		"DispatchThreads",		{"dynamicThreadCount"} );

		binder.Comment( "Execute compute shader with indirect command.\n"
						"Indirect buffer must contains 'DispatchIndirectCommand' data." );
		AS_METHOD( binder, ScriptComputePass::DispatchGroupsIndirect1,	"DispatchGroupsIndirect",	{"indirectBuffer"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroupsIndirect2,	"DispatchGroupsIndirect",	{"indirectBuffer", "indirectBufferOffset"} );
		AS_METHOD( binder, ScriptComputePass::DispatchGroupsIndirect3,	"DispatchGroupsIndirect",	{"indirectBuffer", "indirectBufferField"} );
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptComputePass::_CompilePipeline () C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this] (ScriptEnginePtr) {
						_CompilePipeline2();	// throw
					},
					_baseFlags );
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptComputePass::ToPass () __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() is not used" );
		CHECK_THROW_MSG( not _iterations.empty(), "add at least one Dispatch() call" );
		CHECK_THROW_MSG( not _args.Empty(), "empty argument list" );

		auto		result		= MakeRC<ComputePass>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		result->_rtech = _CompilePipeline();	// throw

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

		// create descriptor set
		{
			CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_dsIndex, OUT result->_descSets.data(), max_frames,
														ppln, DescriptorSetName{"ds0"}, null, _dbgName ));
			_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw
		}

		result->_pcIndex = res_mngr.GetPushConstantIndex<ShaderTypes::ComputePassPC>( ppln, PushConstantName{"pc"} );
		CHECK_THROW( result->_pcIndex );

		_Init( *result, null );
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
	_CreateUBType
=================================================
*/
	ScriptRCBase  ScriptComputePass::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ComputePassUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st = ShaderStructType::Create( "ComputePassUB" );
		st->Set( EStructLayout::Compatible_Std140, R"#(
				float		time;			// shader playback time (in seconds)
				float		timeDelta;		// frame render time (in seconds), max value: 1/30s
				uint		frame;			// shader playback frame, global frame counter
				uint		passFrameId;	// current pass frame index
				uint		seed;			// unique value, updated on each shader reloading
				float4		mouse;			// mouse unorm coords. xy: current (if MRB down), zw: click
				float2		customKeys;
				float		pixPerMm;		// pix / mm
				float		mmPerPix;		// mm / pix

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
		StaticAssert( IPass::CustomKeys_t{}.max_size() == 2 );
		return st;
	}

/*
=================================================
	_CreatePCType
=================================================
*/
	ScriptRCBase  ScriptComputePass::_CreatePCType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ComputePassPC" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st = ShaderStructType::Create( "ComputePassPC" );
		st->Set( EStructLayout::Compatible_Std140, R"#(
				uint4	wgCount_dispatchIndex;
			)#");

		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptComputePass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		{
			ShaderStructTypePtr		st = _CreateUBType();	// throw
			CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
		}{
			ShaderStructTypePtr		st = _CreatePCType();	// throw
			CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
		}
	}

/*
=================================================
	_CompilePipeline2
=================================================
*/
	void  ScriptComputePass::_CompilePipeline2 () C_Th___
	{
		_args.ValidateArgs();

		RenderTechniquePtr	rtech = RenderTechnique::Create( "rtech" );
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}

		const auto				stage		= EShaderStages::Compute;
		DescriptorSetLayoutPtr	ds_layout	= DescriptorSetLayout::Create( "dsl.0" );
		{
			Unused( _CreateUBType() );	// throw

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ComputePassUB", EResourceState::ShaderUniform, False{} );
		}
		_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw


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
	void  ScriptComputePass::_CompilePipeline3 (const String &cs, uint line, const String &pplnName,
												uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
	{
		PipelineLayoutPtr		ppln_layout = PipelineLayout::Create( pplnName + ".pl" );
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );
		ppln_layout->AddPushConst2( "pc", _CreatePCType(), EShader::Compute );

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


} // AE::ResEditor
