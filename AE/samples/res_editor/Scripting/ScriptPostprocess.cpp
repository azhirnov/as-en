// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Passes/Postprocess.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/ScriptBasePass.cpp.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptPostprocess*  ScriptPostprocess_Ctor0 () {
		return ScriptPostprocessPtr{ new ScriptPostprocess{ "", Default, Default }}.Detach();
	}

	static ScriptPostprocess*  ScriptPostprocess_Ctor1 (const String &name) {
		return ScriptPostprocessPtr{ new ScriptPostprocess{ name, Default, Default }}.Detach();
	}

	static ScriptPostprocess*  ScriptPostprocess_Ctor2 (const String &name, ScriptPostprocess::EPostprocess ppFlags) {
		return ScriptPostprocessPtr{ new ScriptPostprocess{ name, ppFlags, Default }}.Detach();
	}

	static ScriptPostprocess*  ScriptPostprocess_Ctor3 (ScriptPostprocess::EPostprocess ppFlags) {
		return ScriptPostprocessPtr{ new ScriptPostprocess{ Default, ppFlags, Default }}.Detach();
	}

	static ScriptPostprocess*  ScriptPostprocess_Ctor4 (ScriptPostprocess::EPostprocess ppFlags, const String &defines) {
		return ScriptPostprocessPtr{ new ScriptPostprocess{ Default, ppFlags, defines }}.Detach();
	}

	static ScriptPostprocess*  ScriptPostprocess_Ctor5 (const String &name, const String &defines) {
		return ScriptPostprocessPtr{ new ScriptPostprocess{ name, Default, defines }}.Detach();
	}

} // namespace


/*
=================================================
	constructor
=================================================
*/
	ScriptPostprocess::ScriptPostprocess (const String &name, EPostprocess ppFlags, const String &defines) __Th___ :
		_pplnPath{ ScriptExe::ScriptPassApi::ToShaderPath( name )},
		_ppFlags{ ppFlags }
	{
		_dbgName = ToString( _pplnPath.filename().replace_extension("") );
		_defines = defines;

		if ( not _defines.empty() )
			_dbgName << "|" << _defines;

		StringToColor( OUT _dbgColor, StringView{_dbgName} );
		FindAndReplace( INOUT _defines, '=', ' ' );

		ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );

		CHECK_THROW_MSG( FileSystem::IsFile( _pplnPath ),
			"File '"s << name << "' is not exists" );
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptPostprocess::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
	{
		arg.state |= EResourceState::FragmentShader;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptPostprocess::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;
		{
			EnumBinder<EPostprocess>	binder{ se };
			binder.Create();

			binder.Comment( "Entry point: 'Main'");
			binder.AddValue( "None",			EPostprocess::Unknown );

			binder.Comment( "Entry point: 'void mainImage (out float4 fragColor, in float2 fragCoord)'" );
			binder.AddValue( "Shadertoy",		EPostprocess::Shadertoy );

			binder.Comment( "Entry point: 'void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir)'" );
			binder.AddValue( "ShadertoyVR",		EPostprocess::ShadertoyVR );
			binder.AddValue( "ShadertoyVR_180",	EPostprocess::ShadertoyVR_180 );
			binder.AddValue( "ShadertoyVR_360",	EPostprocess::ShadertoyVR_360 );
			binder.AddValue( "Shadertoy_360",	EPostprocess::Shadertoy_360 );
			StaticAssert( uint(EPostprocess::_Count) == 6 );
		}
		{
			ClassBinder<ScriptPostprocess>	binder{ se };
			binder.CreateRef( 0, False{"no ctor"} );

			_BindBase( binder, True{"withArgs"} );
			_BindBaseRenderPass( binder, True{"withBlending"} );

			binder.Comment( "Set path to fragment shader, empty - load current file." );
			binder.AddFactoryCtor( &ScriptPostprocess_Ctor0,	{} );
			binder.AddFactoryCtor( &ScriptPostprocess_Ctor1,	{"shaderPath"} );
			binder.AddFactoryCtor( &ScriptPostprocess_Ctor2,	{"shaderPath", "postprocessFlags"} );
			binder.AddFactoryCtor( &ScriptPostprocess_Ctor3,	{"postprocessFlags"} );
			binder.AddFactoryCtor( &ScriptPostprocess_Ctor4,	{"postprocessFlags", "defines"} );
			binder.AddFactoryCtor( &ScriptPostprocess_Ctor5,	{"shaderPath", "defines"} );
		}
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptPostprocess::_CompilePipeline (OUT Bytes &ubSize) C_Th___
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
	RC<IPass>  ScriptPostprocess::ToPass () C_Th___
	{
		auto		result		= MakeRC<Postprocess>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();
		Bytes		ub_size;

		result->_rtech = _CompilePipeline( OUT ub_size ); // throw
		result->_depthRange = this->_depthRange;

		EnumSet<IPass::EDebugMode>	dbg_modes;

		const auto	AddPpln = [this, pp = result.get(), &dbg_modes] (IPass::EDebugMode mode, EFlags flag, PipelineName::Ref name)
		{{
			if ( AllBits( _baseFlags, flag ))
			{
				auto	id = pp->_rtech.rtech->GetGraphicsPipeline( name );
				if ( id ) {
					pp->_pipelines.insert_or_assign( mode, id );
					dbg_modes.insert( mode );
				}
			}
		}};

		AddPpln( IPass::EDebugMode::Unknown,		EFlags::Unknown,				PipelineName{"postprocess"} );
		AddPpln( IPass::EDebugMode::Trace,			EFlags::Enable_ShaderTrace,		PipelineName{"postprocess.Trace"} );
		AddPpln( IPass::EDebugMode::FnProfiling,	EFlags::Enable_ShaderFnProf,	PipelineName{"postprocess.FnProf"} );
		AddPpln( IPass::EDebugMode::TimeHeatMap,	EFlags::Enable_ShaderTmProf,	PipelineName{"postprocess.TmProf"} );

		auto	ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second;

		#if PIPELINE_STATISTICS
		{
			auto&	res = res_mngr.GetResourcesOrThrow( ppln );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
		}
		#endif

		result->_ubuffer = _CreateUBuffer( ub_size, "ShadertoyUB", EResourceState::UniformRead | EResourceState::FragmentShader );  // throw

		// create descriptor set
		{
			CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_dsIndex, OUT result->_descSets.data(), max_frames,
														ppln, DescriptorSetName{"ds0"} ));
			_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw
		}

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

		result->_rpDesc.renderPassName	= RenderPassName{"rp"};
		result->_rpDesc.subpassName		= SubpassName{"main"};
		result->_rpDesc.packId			= result->_rtech.packId;
		result->_rpDesc.layerCount		= ImageLayer{min_layer_count};

		for (usize i = 0; i < _output.size(); ++i)
		{
			auto&	src	= _output[i];
			auto	rt	= src.rt->ToResource();
			CHECK_THROW( rt );

			// validate
			for (auto& [name, res, state] : result->_resources.Get())
			{
				if ( auto* tex = UnionGet< RC<Image> >( res ))
					CHECK_THROW_MSG( tex->get() != rt.get(), "Image '"s << rt->GetName() << "' used as input and output" );
			}

			ImageViewDesc	view;
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

		_Init( *result, null );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Fragment );

		return result;
	}


} // AE::ResEditor


#include "res_editor/Scripting/PipelineCompiler.inl.h"
#include "base/Algorithms/Parser.h"

namespace AE::ResEditor
{
	using namespace AE::PipelineCompiler;

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptPostprocess::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "ShadertoyUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"ShadertoyUB"}};
		st->Set( EStructLayout::Std140, R"#(
				float3		resolution;				// viewport resolution (in pixels)
				float		time;					// shader playback time (in seconds)
				float		timeDelta;				// render time (in seconds)
				uint		frame;					// shader playback frame
				uint		seed;					// unique value, updated on each shader reloading
				float4		channelTime;			// channel playback time (in seconds)
				float4		channelResolution [4];	// channel resolution (in pixels)
				float4		mouse;					// mouse unorm coords. xy: current (if MRB down), zw: click
				float4		date;					// (year, month, day, time in seconds)
				float		sampleRate;				// sound sample rate (i.e., 44100)
				float		customKeys;
				float		pixToMm;

				// controller //
				CameraData	camera;

				// sliders //
				float4		floatSliders [4];
				int4		intSliders [4];
				float4		colors [4];

				// constants //
				float4		floatConst [4];
				int4		intConst [4];
			)#");

		StaticAssert( UIInteraction::MaxSlidersPerType == 4 );
		StaticAssert( IPass::Constants::MaxCount == 4 );
		StaticAssert( IPass::CustomKeys_t{}.max_size() == 1 );
		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptPostprocess::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}

/*
=================================================
	_CompilePipeline2
=================================================
*/
	void  ScriptPostprocess::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
	{
		const String	subpass = "main";

		_args.ValidateArgs();

		CHECK_THROW( not _output.empty() );
		for (auto& out : _output)
		{
			CHECK_THROW_MSG( out.rt );
			CHECK_THROW_MSG( out.rt->ToResource() );
		}


		CompatibleRenderPassDescPtr	compat_rp{ new CompatibleRenderPassDesc{ "compat.rp" }};
		compat_rp->AddSubpass( subpass );
		{
			for (usize i = 0; i < _output.size(); ++i)
			{
				RPAttachmentPtr		att		= compat_rp->AddAttachment2( _output[i].name );
				auto				rt		= _output[i].rt;
				const auto			desc	= rt->ToResource()->GetImageDesc();

				att->format		= desc.format;
				att->samples	= desc.samples;

				att->AddUsage( subpass, (rt->IsDepthOrStencil() ? EAttachment::DepthStencil : EAttachment::Color) );
			}
		}{
			RenderPassSpecPtr	rp_spec		= compat_rp->AddSpecialization2( "rp" );
			const auto			ds_state	= EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestAfterFS;

			for (usize i = 0; i < _output.size(); ++i)
			{
				RPAttachmentSpecPtr	att = rp_spec->AddAttachment2( _output[i].name );
				att->loadOp		= EAttachmentLoadOp::Load;
				att->storeOp	= EAttachmentStoreOp::Store;

				if ( _output[i].HasClearValue() )
				{
					att->loadOp = EAttachmentLoadOp::Clear;
					att->AddLayout( "ExternalIn", EResourceState::Invalidate );
				}

				att->AddLayout( subpass, (_output[i].rt->IsDepthOrStencil() ? ds_state : EResourceState::ColorAttachment) );
			}
		}


		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTGraphicsPassPtr	pass = rtech->AddGraphicsPass2( subpass );
			pass->SetRenderPass( "rp", subpass );
		}


		const auto				stage	= EShaderStages::Fragment;
		DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
		{
			ShaderStructTypePtr	st = _CreateUBType();	// throw
			ubSize = st->StaticSize();

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ShadertoyUB", EResourceState::ShaderUniform, False{} );
		}
		_args.ArgsToDescSet( stage, ds_layout, ArraySize{1}, EAccessType::Coherent );  // throw


		uint			fs_line = 0;
		String			fs;
		const String	vs = R"#(
	void Main () {
		float2	uv = float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.0;
		gl.Position	= float4( uv * 2.0 - 1.0, 0.0, 1.0 );
	}
)#";
		{
			String	header;
			_AddDefines( _defines, INOUT header );

			// add shader header
			header << R"#(
// for "GlobalIndex.glsl"
ND_ int3  GetGlobalSize() {
	return int3(un_PerPass.resolution);
}
)#";
			if ( _ppFlags != EPostprocess::Unknown )
			{
				CHECK_THROW( _output.size() == 1 );
				header
					<< R"#(
#define iResolution			un_PerPass.resolution
#define iTime				un_PerPass.time
#define iTimeDelta			un_PerPass.timeDelta
#define iFrame				int(un_PerPass.frame)
#define iChannelTime		un_PerPass.channelTime
#define iChannelResolution	un_PerPass.channelResolution
#define iMouse				float4( un_PerPass.mouse.xy * un_PerPass.resolution.xy, un_PerPass.mouse.zw )
#define iDate				un_PerPass.date
#define iSampleRate			un_PerPass.sampleRate
)#";
				switch_enum( _ppFlags )
				{
					case EPostprocess::Shadertoy :
						header << R"#(
void mainImage (out float4 fragColor, in float2 fragCoord);

void Main ()
{
	float2 coord = gl.FragCoord.xy;		// + gl.SamplePosition;
	coord = float2(coord.x - 0.5, iResolution.y - coord.y + 0.5);

	mainImage( )#" << _output.front().name << R"#(, coord );
}
)#";
						break;

					case EPostprocess::ShadertoyVR :
					case EPostprocess::ShadertoyVR_180 :
					case EPostprocess::ShadertoyVR_360 :
					case EPostprocess::Shadertoy_360 :
					{
						header << R"#(
#include "Ray.glsl"
void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir);

void Main ()
{
	Ray	ray = )#";
						AE_END_ENUM_CHECKS();
						switch ( _ppFlags )
						{
							case EPostprocess::ShadertoyVR :
								header << "Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, 0.f, gl.FragCoord.xy / iResolution.xy );\n";
								break;
							case EPostprocess::ShadertoyVR_180 :
								header << "Ray_PlaneToVR180( un_PerPass.cameraIPD, un_PerPass.camera.pos, 0.f, gl.FragCoord.xy / iResolution.xy );\n";
								break;
							case EPostprocess::ShadertoyVR_360 :
								header << "Ray_PlaneToVR360( un_PerPass.cameraIPD, un_PerPass.camera.pos, 0.f, gl.FragCoord.xy / iResolution.xy );\n";
								break;
							case EPostprocess::Shadertoy_360 :
								header << "Ray_PlaneTo360( un_PerPass.camera.pos, 0.f, gl.FragCoord.xy / iResolution.xy );\n";
								break;
						}
						AE_BEGIN_ENUM_CHECKS();
						header << R"#(
	float2 coord = gl.FragCoord.xy;		// + gl.SamplePosition;
	coord = float2(coord.x - 0.5, iResolution.y - coord.y + 0.5);
	mainVR( )#" << _output.front().name << R"#(, coord, ray.origin, ray.dir );
})#";
						break;
					}
					case EPostprocess::Unknown :
					case EPostprocess::_Count :
						break;
				}
				switch_end
			}

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

		const EShaderOpt	sh_opt = EShaderOpt::Optimize | EShaderOpt::OptimizeSize;
	//	const EShaderOpt	sh_opt = EShaderOpt::DebugInfo;	// for shader debugging in RenderDoc

	  #if PIPELINE_STATISTICS
		const EPipelineOpt	ppln_opt = EPipelineOpt::Optimize | EPipelineOpt::CaptureStatistics | EPipelineOpt::CaptureInternalRepresentation;
	  #else
		const EPipelineOpt	ppln_opt = EPipelineOpt::Optimize;
	  #endif

		_CompilePipeline3( subpass, vs, fs, fs_line, "postprocess", uint(sh_opt), ppln_opt );

	  #ifdef AE_ENABLE_GLSL_TRACE
		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
			NOTHROW( _CompilePipeline3( subpass,	vs, fs, fs_line, "postprocess.Trace", uint(sh_opt | EShaderOpt::Trace), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
			NOTHROW( _CompilePipeline3( subpass,	vs, fs, fs_line, "postprocess.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
			NOTHROW( _CompilePipeline3( subpass,	vs, fs, fs_line, "postprocess.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default ));
	  #endif
	}

/*
=================================================
	_CompilePipeline3
=================================================
*/
	void  ScriptPostprocess::_CompilePipeline3 (const String &subpass, const String &vs, const String &fs, uint fsLine,
												const String &pplnName, uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
	{
		PipelineLayoutPtr		ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );

		if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
			ppln_layout->AddDebugDSLayout2( 1, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::Fragment) );

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

			for (usize i = 0, c = 0; i < _output.size(); ++i)
			{
				const auto&	src	= _output[i];
				auto&		dst = rs.color.buffers[c];

				if ( src.rt->IsDepthOrStencil() )
				{
					rs.depth.write = true;
					continue;
				}

				++c;
				dst.blend = src.enableBlend;

				if ( src.enableBlend )
				{
					dst.blendOp.color			= src.blendOpRGB;
					dst.blendOp.alpha			= src.blendOpA;

					dst.srcBlendFactor.color	= src.srcFactorRGB;
					dst.srcBlendFactor.alpha	= src.srcFactorA;
					dst.dstBlendFactor.color	= src.dstFactorRGB;
					dst.dstBlendFactor.alpha	= src.dstFactorA;
				}
			}

			ppln_spec->SetRenderState( rs );

			// if successfully compiled
			ppln_spec->Enable();
		}
	}


} // AE::ResEditor
