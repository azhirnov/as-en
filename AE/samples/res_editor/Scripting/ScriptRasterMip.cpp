// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/ScriptBasePass.cpp.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptRasterMip*  ScriptRasterMip_Ctor1 () {
		return ScriptRasterMipPtr{ new ScriptRasterMip{ Default, Default }}.Detach();
	}

	static ScriptRasterMip*  ScriptRasterMip_Ctor2 (const String &name) {
		return ScriptRasterMipPtr{ new ScriptRasterMip{ name, Default }}.Detach();
	}

	static ScriptRasterMip*  ScriptRasterMip_Ctor3 (const String &name, const String &defines) {
		return ScriptRasterMipPtr{ new ScriptRasterMip{ name, defines }}.Detach();
	}

} // namespace


/*
=================================================
	constructor
=================================================
*/
	ScriptRasterMip::ScriptRasterMip (const String &name, const String &defines) __Th___ :
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
	void  ScriptRasterMip::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
	{
		arg.state |= EResourceState::FragmentShader;
	}
	
/*
=================================================
	_Variable
=================================================
*/
	void  ScriptRasterMip::_Variable (Scripting::ScriptArgList args) __Th___
	{
		auto*	obj = args.GetObject< ScriptRasterMip >();
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
		
		CHECK_THROW_MSG( args.IsArg< String const& >(idx),
			"Last argument must be sampler name" );
		dst.sampler = args.Arg< String const& >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );

		dst.image->AddUsage( EResourceUsage::Sampled );
		dst.image->AddUsage( EResourceUsage::ColorAttachment );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRasterMip::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptRasterMip>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		ScriptBasePass::_BindBase( binder, True{"withArgs"} );

		binder.AddFactoryCtor( &ScriptRasterMip_Ctor1,	{} );
		binder.AddFactoryCtor( &ScriptRasterMip_Ctor2,	{"shaderPath"} );
		binder.AddFactoryCtor( &ScriptRasterMip_Ctor3,	{"shaderPath", "defines"} );

		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &)					>( &ScriptRasterMip::_Variable, "Variable", {"inName", "outName", "image"} );
		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &, const String &)	>( &ScriptRasterMip::_Variable, "Variable", {"inName", "outName", "image", "sampler"} );
			
		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &, const MipmapLevel &)					>( &ScriptRasterMip::_Variable, "Variable", {"inName", "outName", "image", "baseMipmap"} );
		binder.AddGenericMethod< void (const String &, const String &, const ScriptImagePtr &, const MipmapLevel &, const String &)	>( &ScriptRasterMip::_Variable, "Variable", {"inName", "outName", "image", "baseMipmap", "sampler"} );
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptRasterMip::_CompilePipeline (OUT Bytes &ubSize) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this, &ubSize] (ScriptEnginePtr) {
						_CompilePipeline2( OUT ubSize );	// throw
					});
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptRasterMip::ToPass () __Th___
	{
		CHECK_THROW_MSG( not _variables.empty(), "empty variable args list" );
		CHECK_THROW_MSG( not _controller, "controller must be null" );

		auto		result		= MakeRC<RasterMip>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();
		Bytes		ub_size;

		result->_rtech = _CompilePipeline( OUT ub_size );	// throw
		
		result->_rpDesc.renderPassName	= RenderPassName{"rp"};
		result->_rpDesc.subpassName		= SubpassName{"main"};
		result->_rpDesc.packId			= result->_rtech.packId;

		EnumSet<IPass::EDebugMode>	dbg_modes;

		const auto	AddPpln = [this, cp = result.get(), &dbg_modes] (IPass::EDebugMode mode, EFlags flag, PipelineName::Ref name)
		{{
			if ( AllBits( _baseFlags, flag ))
			{
				auto	id = cp->_rtech.rtech->GetGraphicsPipeline( name );
				if ( id ) {
					cp->_pipelines.insert_or_assign( mode, id );
					dbg_modes.insert( mode );
				}
			}
		}};

		AddPpln( IPass::EDebugMode::Unknown,		EFlags::Unknown,				PipelineName{"rastermip"} );
		AddPpln( IPass::EDebugMode::Trace,			EFlags::Enable_ShaderTrace,		PipelineName{"rastermip.Trace"} );
		AddPpln( IPass::EDebugMode::FnProfiling,	EFlags::Enable_ShaderFnProf,	PipelineName{"rastermip.FnProf"} );
		AddPpln( IPass::EDebugMode::TimeHeatMap,	EFlags::Enable_ShaderTmProf,	PipelineName{"rastermip.TmProf"} );

		auto	ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second;

		result->_ubuffer = _CreateUBuffer( ub_size, "ComputeMipUB", EResourceState::UniformRead | EResourceState::FragmentShader );  // throw

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
			dst.inName		= UniformName{src.inName};
			dst.outName		= AttachmentName{src.outName};
		}

		_Init( *result, null );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Fragment );

		return result;
	}


} // AE::ResEditor


#include "res_editor/Scripting/PipelineCompiler.inl.h"

#include "base/DataSource/File.h"
#include "base/Algorithms/Parser.h"

#include "res_editor/Scripting/ScriptImage.h"
#include "res_editor/Scripting/ScriptVideoImage.h"

namespace AE::ResEditor
{
	using namespace AE::PipelineCompiler;

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptRasterMip::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ComputeMipUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"ComputeMipUB"}};
		st->Set( EStructLayout::Std140, R"#(
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
	auto  ScriptRasterMip::_CreatePCType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ComputeMipPC" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"ComputeMipPC"}};
		st->Set( EStructLayout::Std140, R"#(
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
	void  ScriptRasterMip::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
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
	void  ScriptRasterMip::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
	{
		const String	subpass = "main";

		_args.ValidateArgs();
		
		CompatibleRenderPassDescPtr		compat_rp{ new CompatibleRenderPassDesc{ "compat.rp" }};
		compat_rp->AddSubpass( subpass );
		{
			for (auto [var, i] : WithIndex(_variables))
			{
				RPAttachmentPtr		att		= compat_rp->AddAttachment2( var.outName );
				const auto			desc	= var.image->ToResource()->GetImageDesc();

				att->format		= desc.format;
				att->samples	= desc.samples;
				
				att->AddUsage( subpass, EAttachment::Color );
			}
		}{
			RenderPassSpecPtr	rp_spec = compat_rp->AddSpecialization2( "rp" );
			for (auto [var, i] : WithIndex(_variables))
			{
				RPAttachmentSpecPtr	att = rp_spec->AddAttachment2( var.outName );

				att->loadOp		= EAttachmentLoadOp::Invalidate;
				att->storeOp	= EAttachmentStoreOp::Store;

				att->AddLayout( "ExternalIn",	EResourceState::Invalidate );
				att->AddLayout( subpass,		EResourceState::ColorAttachment );
				att->AddLayout( "ExternalOut",	EResourceState::ShaderSample | EResourceState::FragmentShader );
			}
		}


		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTGraphicsPassPtr	pass = rtech->AddGraphicsPass2( subpass );
			pass->SetRenderPass( "rp", subpass );
		}

		const auto	stage = EShaderStages::Fragment;
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
				ds_layout->AddCombinedImage_ImmutableSampler( stage, var.inName, type, EResourceState::ShaderSample, {var.sampler} );
			}
		}

		uint	fs_line = 0;
		String	vs, fs;

		// vertex shader
		{
			vs << R"#(
	void Main () {
		float2	uv = float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.0;
		gl.Position	= float4( uv * 2.0 - 1.0, 0.0, 1.0 );
	})#";
		}
		
		// fragment shader
		{
			String	header;
			_AddDefines( _defines, INOUT header );

			// add shader header
			_AddSliders( INOUT header );

			// load shader source from file
			{
				FileRStream		file {_pplnPath};
				CHECK_THROW_MSG( file.IsOpen(),
					"Failed to open shader file: '"s << ToString(_pplnPath) << "'" );

				CHECK_THROW_MSG( file.Read( file.RemainingSize(), OUT fs ),
					"Failed to read shader file '"s << ToString(_pplnPath) << "'" );

				header >> fs;
				fs_line = uint(Parser::CalculateNumberOfLines( header )) - 1;
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

		_CompilePipeline3( vs, fs, fs_line, "rastermip", uint(sh_opt), ppln_opt );

	  #ifdef AE_ENABLE_GLSL_TRACE
		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
			NOTHROW( _CompilePipeline3( vs, fs, fs_line, "rastermip.Trace", uint(sh_opt | EShaderOpt::Trace), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
			NOTHROW( _CompilePipeline3( vs, fs, fs_line, "rastermip.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
			NOTHROW( _CompilePipeline3( vs, fs, fs_line, "rastermip.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default ));
	  #endif
	}

/*
=================================================
	_CompilePipeline3
=================================================
*/
	void  ScriptRasterMip::_CompilePipeline3 (const String &vs, const String &fs, uint fsLine, const String &pplnName,
											  uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
	{
		const String			subpass = "main";

		PipelineLayoutPtr		ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );
		ppln_layout->AddDSLayout2( "ds1", 1, "dsl.1" );

		{
			ShaderStructTypePtr	st = _CreatePCType();	// throw
			ppln_layout->AddPushConst2( "pc", st, EShader::Fragment );
			
			ppln_layout->Define( "iInvResolution=pc.invResolution" );
			ppln_layout->Define( "iResolution=pc.resolution" );
		}

		if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
			ppln_layout->AddDebugDSLayout2( 2, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::Fragment) );

		GraphicsPipelinePtr		ppln_templ{ new GraphicsPipelineScriptBinding{ pplnName }};
		ppln_templ->Disable();
		ppln_templ->SetFragmentOutputFromRenderPass( "compat.rp", subpass );
		ppln_templ->SetLayout2( ppln_layout );

		{
			ScriptShaderPtr	sh{ new ScriptShader{}};
			sh->SetSource( EShader::Vertex, vs );
			ppln_templ->SetVertexShader( sh );
		}
		{
			ScriptShaderPtr	sh{ new ScriptShader{}};
			sh->SetSource2( EShader::Fragment, fs, PathAndLine{_pplnPath, fsLine} );
			sh->options = EShaderOpt(shaderOpts);
			ppln_templ->SetFragmentShader( sh );
		}
		{
			GraphicsPipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( pplnName );
			ppln_spec->Disable();
			ppln_spec->AddToRenderTech( "rtech", subpass );
			ppln_spec->SetViewportCount( 1 );
			ppln_spec->SetOptions( pplnOpt );

			RenderState		rs;
			rs.inputAssembly.topology = EPrimitive::TriangleList;

			ppln_spec->SetRenderState( rs );

			// if successfully compiled
			ppln_spec->Enable();
		}
	}


} // AE::ResEditor
