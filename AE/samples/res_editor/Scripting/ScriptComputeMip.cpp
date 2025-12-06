// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptExe.h"
#include "Core/EditorUI.h"
#include "Scripting/ScriptBasePass.cpp.h"
#include "_data/cpp/types.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptComputeMip*  ScriptComputeMip_Ctor1 () {
		return ScriptComputeMipPtr{ new ScriptComputeMip{ Default, Default }}.Detach();
	}

	static ScriptComputeMip*  ScriptComputeMip_Ctor2 (const String &name) {
		return ScriptComputeMipPtr{ new ScriptComputeMip{ name, Default }}.Detach();
	}

	static ScriptComputeMip*  ScriptComputeMip_Ctor3 (const String &name, const String &defines) {
		return ScriptComputeMipPtr{ new ScriptComputeMip{ name, defines }}.Detach();
	}

} // namespace


/*
=================================================
	constructor
=================================================
*/
	ScriptComputeMip::ScriptComputeMip (const String &name, const String &defines) __Th___ :
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
	_OnAddArg
=================================================
*/
	void  ScriptComputeMip::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
	{
		arg.state |= EResourceState::ComputeShader;
	}

/*
=================================================
	_Variable
=================================================
*/
	void  ScriptComputeMip::_Variable (Scripting::ScriptArgList args) __Th___
	{
		auto*	obj = args.GetObject< ScriptComputeMip >();
		auto&	dst	= obj->_variables.emplace_back();
		uint	idx	= 0;

		CHECK_THROW_MSG( args.IsArg< String const& >(idx),
			"First argument must be string with uniform name of input resource" );
		dst.inName = args.Arg< String const& >(idx++);

		CHECK_THROW_MSG( args.IsArg< String const& >(idx),
			"Second argument must be string with uniform name of output resource" );
		dst.outName = args.Arg< String const& >(idx++);

		CHECK_THROW_MSG( args.IsArg< ScriptImagePtr const& >(idx),
			"Third argument must be image" );
		dst.image = args.Arg< ScriptImagePtr const& >(idx++);

		if ( args.IsArg< MipmapLevel const& >(idx) )
			dst.baseMipmap = args.Arg< MipmapLevel const& >(idx++);

		if ( args.IsArg< String const& >(idx) )
		{
			dst.sampler = args.Arg< String const& >(idx++);
			dst.inState = EResourceState::ShaderSample | EResourceState::ComputeShader;
			dst.image->AddUsage( EResourceUsage::Sampled );
		}
		else
		{
			dst.inState = EResourceState::ShaderStorage_Read | EResourceState::ComputeShader;
			dst.image->AddUsage( EResourceUsage::ComputeRead );
		}

		dst.outState = EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;
		dst.image->AddUsage( EResourceUsage::ComputeWrite );

		CHECK_THROW_MSG( idx == args.ArgCount() );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptComputeMip::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptComputeMip>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		ScriptBasePass::_BindBase( binder, True{"withArgs"} );

		binder.AddFactoryCtor( &ScriptComputeMip_Ctor1,	{} );
		binder.AddFactoryCtor( &ScriptComputeMip_Ctor2,	{"shaderPath"} );
		binder.AddFactoryCtor( &ScriptComputeMip_Ctor3,	{"shaderPath", "defines"} );

		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &)					>( &ScriptComputeMip::_Variable, "Variable", {"inName", "outName", "image"} );
		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &, const String &)	>( &ScriptComputeMip::_Variable, "Variable", {"inName", "outName", "image", "sampler"} );

		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &, const MipmapLevel &)					>( &ScriptComputeMip::_Variable, "Variable", {"inName", "outName", "image", "baseMipmap"} );
		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &, const MipmapLevel &, const String &)	>( &ScriptComputeMip::_Variable, "Variable", {"inName", "outName", "image", "baseMipmap", "sampler"} );
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptComputeMip::_CompilePipeline (OUT Bytes &ubSize) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this, &ubSize] (ScriptEnginePtr) {
						_CompilePipeline2( OUT ubSize );	// throw
					},
					_baseFlags );
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptComputeMip::ToPass () __Th___
	{
		CHECK_THROW_MSG( not _variables.empty(), "empty variable args list" );
		CHECK_THROW_MSG( not _controller, "controller must be null" );

		auto		result		= MakeRC<ComputeMip>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();
		Bytes		ub_size;

		result->_rtech = _CompilePipeline( OUT ub_size );	// throw

		EnumSet<IPass::EDebugMode>	dbg_modes;

		const auto	AddPpln = [this, cp = result.get(), &dbg_modes] (IPass::EDebugMode mode, EFlags flag, PipelineName::Ref name)
		{{
			if ( AllBits( _baseFlags, flag ))
			{
				auto	id = cp->_rtech.rtech->GetComputePipeline( name );
				if ( id ) {
					cp->_pipelines.insert_or_assign( mode, id );
					dbg_modes.insert( mode );
				}
			}
		}};

		AddPpln( IPass::EDebugMode::Unknown,		EFlags::Unknown,				PipelineName{"compute"} );
		AddPpln( IPass::EDebugMode::Trace,			EFlags::Enable_ShaderTrace,		PipelineName{"compute.Trace"} );
		AddPpln( IPass::EDebugMode::FnProfiling,	EFlags::Enable_ShaderFnProf,	PipelineName{"compute.FnProf"} );
		AddPpln( IPass::EDebugMode::TimeHeatMap,	EFlags::Enable_ShaderTmProf,	PipelineName{"compute.TmProf"} );

		auto	ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second;

		result->_ubuffer = _CreateUBuffer( ub_size, "ComputeMipUB", EResourceState::UniformRead | EResourceState::ComputeShader );  // throw

		// create descriptor set
		{
			CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_ds0Index, OUT result->_descSets.data(), max_frames,
														ppln, DescriptorSetName{"ds0"}, null, _dbgName ));
			_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw

			result->_pcIndex = res_mngr.GetPushConstantIndex< ShaderTypes::ComputeMipPC >( ppln, PushConstantName{"pc"} );
			CHECK_THROW( result->_pcIndex );
		}

		result->_variables.resize( _variables.size() );
		for (usize i = 0; i < _variables.size(); ++i)
		{
			const auto&	src	= _variables[i];
			auto&		dst = result->_variables[i];

			dst.image		= src.image->ToResource();
			dst.baseMipmap	= src.baseMipmap;
			dst.inState		= src.inState;
			dst.outState	= src.outState;
			dst.inName		= UniformName{src.inName};
			dst.outName		= UniformName{src.outName};
		}

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
	auto  ScriptComputeMip::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ComputeMipUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"ComputeMipUB"}};
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
	auto  ScriptComputeMip::_CreatePCType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ComputeMipPC" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"ComputeMipPC"}};
		st->Set( EStructLayout::Compatible_Std140, R"#(
				float2	invResolution;
				uint2	resolution;
			)#");

		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptComputeMip::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		{
			auto	st = _CreateUBType();	// throw
			CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
		}{
			auto	st = _CreatePCType();	// throw
			CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
		}
	}

/*
=================================================
	_CompilePipeline2
=================================================
*/
	void  ScriptComputeMip::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
	{
		_args.ValidateArgs();

		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}

		const auto	stage = EShaderStages::Compute;
		{
			DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "dsl.0" }};

			ShaderStructTypePtr	st = _CreateUBType();	// throw
			ubSize = st->StaticSize();

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ComputeMipUB", EResourceState::ShaderUniform, False{} );
			_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw
		}{
			DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "dsl.1" }};

			for (auto& var : _variables)
			{
				const auto	type = EImageType(var.image->ImageType());
				if ( not var.sampler.empty() )
					ds_layout->AddCombinedImage_ImmutableSampler( stage, var.inName, type, var.inState, {var.sampler} );
				else
					ds_layout->AddStorageImage( stage, var.inName, ArraySize{1}, type, var.image->PixelFormat(), Default, var.inState );

				ds_layout->AddStorageImage( stage, var.outName, ArraySize{1}, type, var.image->PixelFormat(), Default, var.outState );
			}
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
				cs_line = uint(Parser::CalculateNumberOfLines( header )) - 1;
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

		StaticAssert( uint(UIInteraction::EShaderFlags::_Count) == 5 );

		_CompilePipeline3( cs, cs_line, "compute", uint(sh_opt), ppln_opt );

	  #ifdef AE_ENABLE_GLSL_TRACE
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
	void  ScriptComputeMip::_CompilePipeline3 (const String &cs, uint line, const String &pplnName,
												uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
	{
		PipelineLayoutPtr		ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );
		ppln_layout->AddDSLayout2( "ds1", 1, "dsl.1" );

		{
			ShaderStructTypePtr	st = _CreatePCType();	// throw
			ppln_layout->AddPushConst2( "pc", st, EShader::Compute );

			ppln_layout->Define( "iInvResolution=pc.invResolution" );
			ppln_layout->Define( "iResolution=pc.resolution" );
		}

		if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
			ppln_layout->AddDebugDSLayout2( 2, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::Compute) );

		ComputePipelinePtr		ppln_templ{ new ComputePipelineScriptBinding{ pplnName }};
		ppln_templ->Disable();
		ppln_templ->SetLayout2( ppln_layout );

		{
			ScriptShaderPtr	sh{ new ScriptShader{}};
			sh->SetSource2( EShader::Compute, cs, PathAndLine{_pplnPath, line} );
			sh->options = EShaderOpt(shaderOpts);
			sh->SetComputeLocalSize2( ComputeMip::_localSize, ComputeMip::_localSize );

			ppln_templ->SetShader( sh );
		}
		{
			ComputePipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( pplnName );
			ppln_spec->Disable();
			ppln_spec->AddToRenderTech( "rtech", "Compute" );
			ppln_spec->SetOptions( pplnOpt );

			// if successfully compiled
			ppln_spec->Enable();
		}
	}


} // AE::ResEditor
