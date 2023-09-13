// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Passes/Postprocess.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/ScriptBasePass.cpp.h"

namespace AE::ResEditor
{
namespace
{
    static ScriptPostprocess*  ScriptPostprocess_Ctor1 (const String &name) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ name, Default, Default, ScriptBasePass::EFlags::Enable_AllShaderDbg }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor2 (const String &name, ScriptPostprocess::EPostprocess ppFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ name, ppFlags, Default, ScriptBasePass::EFlags::Enable_AllShaderDbg }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor3 (ScriptPostprocess::EPostprocess ppFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ Default, ppFlags, Default, ScriptBasePass::EFlags::Enable_AllShaderDbg }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor4 (ScriptPostprocess::EPostprocess ppFlags, const String &defines) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ Default, ppFlags, defines, ScriptBasePass::EFlags::Enable_AllShaderDbg }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor5 (ScriptPostprocess::EPostprocess ppFlags, ScriptBasePass::EFlags baseFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ Default, ppFlags, Default, baseFlags }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor6 (ScriptPostprocess::EPostprocess ppFlags, const String &defines, ScriptBasePass::EFlags baseFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ Default, ppFlags, defines, baseFlags }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor7 (const String &name, ScriptPostprocess::EPostprocess ppFlags, ScriptBasePass::EFlags baseFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ name, ppFlags, Default, baseFlags }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor8 (const String &name, ScriptPostprocess::EPostprocess ppFlags, const String &defines, ScriptBasePass::EFlags baseFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ name, ppFlags, defines, baseFlags }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor9 (const String &name, const String &defines) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ name, Default, defines, Default }}.Detach();
    }

    static ScriptPostprocess*  ScriptPostprocess_Ctor10 (const String &name, const String &defines, ScriptBasePass::EFlags baseFlags) {
        return ScriptPostprocessPtr{ new ScriptPostprocess{ name, Default, defines, baseFlags }}.Detach();
    }

    static constexpr float  DefaultIPD  = 64.0e-3f;

} // namespace


/*
=================================================
    constructor
=================================================
*/
    ScriptPostprocess::ScriptPostprocess () :
        ScriptBaseRenderPass{ EFlags::Unknown }, _ppFlags{ Default }
    {}

    ScriptPostprocess::ScriptPostprocess (const String &name, EPostprocess ppFlags, const String &defines, EFlags baseFlags) __Th___ :
        ScriptBaseRenderPass{ baseFlags },
        _pplnPath{ ScriptExe::ScriptPassApi::ToShaderPath( name )},
        _ppFlags{ ppFlags }
    {
        _dbgName = ToString( _pplnPath.filename().replace_extension("") );
        _defines = defines;

        if ( not _defines.empty() )
            _dbgName << "|" << _defines;

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
            EnumBinder<EPostprocess>    binder{ se };
            binder.Create();

            binder.Comment( "Entry point: 'Main'");
            binder.AddValue( "None",            EPostprocess::Unknown );

            binder.Comment( "Entry point: 'void mainImage (out float4 fragColor, in float2 fragCoord)'" );
            binder.AddValue( "Shadertoy",       EPostprocess::Shadertoy );

            binder.Comment( "Entry point: 'void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir)'" );
            binder.AddValue( "ShadertoyVR",     EPostprocess::ShadertoyVR );
            binder.AddValue( "ShadertoyVR_180", EPostprocess::ShadertoyVR_180 );
            binder.AddValue( "ShadertoyVR_360", EPostprocess::ShadertoyVR_360 );
            binder.AddValue( "Shadertoy_360",   EPostprocess::Shadertoy_360 );
            STATIC_ASSERT( uint(EPostprocess::_Last) == 17 );
        }
        {
            ClassBinder<ScriptPostprocess>  binder{ se };
            binder.CreateRef();

            _BindBase( binder, True{"withArgs"} );
            _BindBaseRenderPass( binder, True{"withBlending"} );

            binder.Comment( "Set path to fragment shader, empty - load current file." );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor1,    {"shaderPath"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor2,    {"shaderPath", "postprocessFlags"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor3,    {"postprocessFlags"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor4,    {"postprocessFlags", "defines"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor5,    {"postprocessFlags", "passFlags"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor6,    {"postprocessFlags", "defines", "passFlags"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor7,    {"shaderPath", "postprocessFlags", "passFlags"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor8,    {"shaderPath", "postprocessFlags", "defines", "passFlags"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor9,    {"shaderPath", "defines"} );
            binder.AddFactoryCtor( &ScriptPostprocess_Ctor10,   {"shaderPath", "defines", "passFlags"} );
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
                        _CompilePipeline2( OUT ubSize );    // throw
                    });
    }

/*
=================================================
    ToPass
=================================================
*/
    RC<IPass>  ScriptPostprocess::ToPass () C_Th___
    {
        auto        result      = MakeRC<Postprocess>();
        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        Renderer&   renderer    = ScriptExe::ScriptPassApi::GetRenderer();  // throw
        const auto  max_frames  = RenderTaskScheduler().GetMaxFrames();
        Bytes       ub_size;

        result->_rtech = _CompilePipeline( OUT ub_size ); // throw

        result->_rpDesc.renderPassName  = RenderPassName{"rp"};
        result->_rpDesc.subpassName     = SubpassName{"main"};
        result->_rpDesc.packId          = result->_rtech.packId;
        result->_rpDesc.layerCount      = 1_layer;
        result->_depthRange             = this->_depthRange;

        EnumBitSet<IPass::EDebugMode>   dbg_modes;

        const auto  AddPpln = [this, pp = result.get(), &dbg_modes] (IPass::EDebugMode mode, EFlags flag, const PipelineName &name)
        {{
            if ( AllBits( _baseFlags, flag ))
            {
                auto    id = pp->_rtech.rtech->GetGraphicsPipeline( name );
                if ( id ) {
                    pp->_pipelines.insert_or_assign( mode, id );
                    dbg_modes.insert( mode );
                }
            }
        }};

        AddPpln( IPass::EDebugMode::Unknown,        EFlags::Unknown,                PipelineName{"postprocess"} );
        AddPpln( IPass::EDebugMode::Trace,          EFlags::Enable_ShaderTrace,     PipelineName{"postprocess.Trace"} );
        AddPpln( IPass::EDebugMode::FnProfiling,    EFlags::Enable_ShaderFnProf,    PipelineName{"postprocess.FnProf"} );
        AddPpln( IPass::EDebugMode::TimeHeatMap,    EFlags::Enable_ShaderTmProf,    PipelineName{"postprocess.TmProf"} );

        auto    ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second;

        result->_ubuffer = res_mngr.CreateBuffer( BufferDesc{ ub_size, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                  "ShadertoyUB", renderer.GetAllocator() );
        CHECK_ERR( result->_ubuffer );


        // create descriptor set
        {
            CHECK_ERR( res_mngr.CreateDescriptorSets( OUT result->_dsIndex, OUT result->_descSets.data(), max_frames,
                                                      ppln, DescriptorSetName{"ds0"} ));
            _args.InitResources( OUT result->_resources );  // throw
        }

        for (usize i = 0; i < _output.size(); ++i)
        {
            auto&   src = _output[i];
            auto    rt  = src.rt->ToResource();
            CHECK_ERR( rt );

            // validate
            for (auto& [name, res, state] : result->_resources.Get())
            {
                if ( auto* tex = UnionGet< RC<Image> >( res ))
                    CHECK_ERR_MSG( tex->get() != rt.get(), "Image '"s << rt->GetName() << "' used as input and output" );
            }

            ImageViewDesc   view;
            view.viewType       = EImage_2D;
            view.baseLayer      = src.layer;
            view.layerCount     = 1;
            view.baseMipmap     = src.mipmap;
            view.mipmapCount    = 1;

            rt = rt->CreateView( view, rt->GetName() );
            CHECK_ERR( rt );

            auto&   dst = result->_renderTargets.emplace_back();
            dst.name    = AttachmentName{src.name};
            dst.image   = rt;
            dst.clear   = src.clear;
        }
        CHECK_ERR( not result->_renderTargets.empty() );

        result->_dbgName    = this->_dbgName;
        result->_dbgColor   = this->_dbgColor;

        if ( this->_controller )
        {
            this->_controller->SetDimensionIfNotSet( _dynamicDim );
            result->_controller = this->_controller->ToController();  // throw
            CHECK_ERR( result->_controller );
        }

        // add debug modes
        UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Fragment );

        _AddSlidersToUIInteraction( result.get() );
        _CopyConstants( OUT result->_shConst );

        AE_LOGI( "Compiled: "s << _dbgName );
        return result;
    }


} // AE::ResEditor


#include "res_editor/Scripting/PassCommon.inl.h"

#include "base/DataSource/FileStream.h"
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
    auto  ScriptPostprocess::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "ShadertoyUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"ShadertoyUB"}};
        st->Set( EStructLayout::Std140, R"#(
                float3      resolution;             // viewport resolution (in pixels)
                float       time;                   // shader playback time (in seconds)
                float       timeDelta;              // render time (in seconds)
                uint        frame;                  // shader playback frame
                uint        seed;                   // unique value, updated on each shader reloading
                float4      channelTime;            // channel playback time (in seconds)
                float4      channelResolution [4];  // channel resolution (in pixels)
                float4      mouse;                  // mouse unorm coords. xy: current (if MRB down), zw: click
                float4      date;                   // (year, month, day, time in seconds)
                float       sampleRate;             // sound sample rate (i.e., 44100)
                float       customKeys;

                // controller //
                CameraData  camera;

                // sliders //
                float4      floatSliders [4];
                int4        intSliders [4];
                float4      colors [4];

                // constants //
                float4      floatConst [4];
                int4        intConst [4];
            )#");

        STATIC_ASSERT( UIInteraction::MaxSlidersPerType == 4 );
        STATIC_ASSERT( IPass::Constants::MaxCount == 4 );
        STATIC_ASSERT( IPass::CustomKeys_t{}.max_size() == 1 );
        return st;
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptPostprocess::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
    {
        auto    st = _CreateUBType();   // throw

        CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
    }

/*
=================================================
    _CompilePipeline
=================================================
*/
    void  ScriptPostprocess::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
    {
        const String    subpass = "main";

        _args.ValidateArgs();

        CHECK_THROW( not _output.empty() );
        for (auto& out : _output)
        {
            CHECK_THROW_MSG( out.rt );
            CHECK_THROW_MSG( out.rt->ToResource() );
        }


        CompatibleRenderPassDescPtr compat_rp{ new CompatibleRenderPassDesc{ "compat.rp" }};
        compat_rp->AddSubpass( subpass );
        {
            for (usize i = 0; i < _output.size(); ++i)
            {
                RPAttachmentPtr     att     = compat_rp->AddAttachment2( _output[i].name );
                const auto          desc    = _output[i].rt->ToResource()->GetImageDesc();

                att->format     = desc.format;
                att->samples    = desc.samples;
                att->AddUsage( subpass, EAttachment::Color );
            }
        }{
            RenderPassSpecPtr   rp_spec = compat_rp->AddSpecialization2( "rp" );

            for (usize i = 0; i < _output.size(); ++i)
            {
                RPAttachmentSpecPtr att = rp_spec->AddAttachment2( _output[i].name );
                att->loadOp     = EAttachmentLoadOp::Load;
                att->storeOp    = EAttachmentStoreOp::Store;

                if ( _output[i].HasClearValue() )
                {
                    att->loadOp = EAttachmentLoadOp::Clear;
                    att->AddLayout( "ExternalIn", EResourceState::Invalidate );
                }

                att->AddLayout( subpass, EResourceState::ColorAttachment );
            }
        }


        RenderTechniquePtr  rtech{ new RenderTechnique{ "rtech" }};
        {
            RTGraphicsPassPtr   pass = rtech->AddGraphicsPass2( subpass );
            pass->SetRenderPass( "rp", subpass );
        }


        const auto              stage   = EShaderStages::Fragment;
        DescriptorSetLayoutPtr  ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
        {
            ShaderStructTypePtr st = _CreateUBType();   // throw
            ubSize = st->StaticSize();

            ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ShadertoyUB", EResourceState::ShaderUniform, False{} );
        }
        _args.ArgsToDescSet( stage, ds_layout, ArraySize{1}, EAccessType::Coherent );  // throw


        uint            fs_line = 0;
        String          fs;
        const String    vs = R"#(
    void Main () {
        float2  uv = float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.0;
        gl.Position = float4( uv * 2.0 - 1.0, 0.0, 1.0 );
    }
)#";
        {
            String  header;
            _AddDefines( _defines, INOUT header );

            // add shader header
            header << R"#(
// for "GlobalIndex.glsl"
ND_ int3  GetGlobalSize() {
    return int3(un_PerPass.resolution);
}
)#";
            if ( AnyBits( _ppFlags, EPostprocess::_ShadertoyBits ))
            {
                CHECK_THROW( _output.size() == 1 );

                header
                    << "#define _ColorOutput0 " << _output.front().name << "\n"
                    << R"#(
#define iResolution         un_PerPass.resolution
#define iTime               un_PerPass.time
#define iTimeDelta          un_PerPass.timeDelta
#define iFrame              int(un_PerPass.frame)
#define iChannelTime        un_PerPass.channelTime
#define iChannelResolution  un_PerPass.channelResolution
#define iMouse              float4( un_PerPass.mouse.xy * un_PerPass.resolution.xy, un_PerPass.mouse.zw )
#define iDate               un_PerPass.date
#define iSampleRate         un_PerPass.sampleRate

#if defined(VIEW_MODE_VR)
    void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir);

    void Main ()
    {
        float2 coord = gl.FragCoord.xy;     // + gl.SamplePosition;
        float2 uv    = coord / iResolution.xy;
        float3 dir   = mix( mix( un_PerPass.cameraFrustumLB, un_PerPass.cameraFrustumRB, uv.x ),
                            mix( un_PerPass.cameraFrustumLT, un_PerPass.cameraFrustumRT, uv.x ),
                            uv.y );
        coord = float2(coord.x - 0.5, iResolution.y - coord.y + 0.5);
        mainVR( _ColorOutput0, coord, un_PerPass.cameraPos, dir );
    }

#elif defined(VIEW_MODE_VR180) || defined(VIEW_MODE_VR360) || defined(VIEW_MODE_360)
    void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir);

    void Main ()
    {
        // from https://developers.google.com/vr/jump/rendering-ods-content.pdf
        float2  coord   = gl.FragCoord.xy;      // + gl.SamplePosition;
        float2  uv      = coord / iResolution.xy;
        float   pi      = 3.14159265358979323846f;

    #if defined(VIEW_MODE_VR360)
        float   scale   = un_PerPass.cameraIPD * 0.5 * (uv.y < 0.5 ? -1.0 : 1.0);           // vr360 top-bottom
                uv      = float2( uv.x, (uv.y < 0.5 ? uv.y : uv.y - 0.5) * 2.0 );
    #elif defined(VIEW_MODE_VR180)
        float   scale   = un_PerPass.cameraIPD * 0.5 * (uv.x < 0.5 ? -1.0 : 1.0);           // vr180 left-right
                uv      = float2( (uv.x < 0.5 ? uv.x : uv.x - 0.5) * 0.5 + 0.25, uv.y );    // map [0, 1] to [0.25, 0.75]
    #elif defined(VIEW_MODE_360)
        float   scale   = 1.0;
    #endif

        float   theta   = (uv.x) * 2.0 * pi - pi;
        float   phi     = pi * 0.5 - uv.y * pi;

        float3  origin  = float3(cos(theta), 0.0, sin(theta)) * scale;
        float3  dir     = float3(sin(theta) * cos(phi), sin(phi), -cos(theta) * cos(phi));

        coord = float2(coord.x - 0.5, iResolution.y - coord.y + 0.5);
        mainVR( _ColorOutput0, coord, un_PerPass.cameraPos + origin, dir );
    }

#else
    void mainImage (out float4 fragColor, in float2 fragCoord);

    void Main ()
    {
        float2 coord = gl.FragCoord.xy;     // + gl.SamplePosition;
        coord = float2(coord.x - 0.5, iResolution.y - coord.y + 0.5);

        mainImage( _ColorOutput0, coord );
    }
#endif
#undef _ColorOutput0
)#";
            }

            _AddSliders( INOUT header );

            // load shader source from file
            {
                FileRStream     file {_pplnPath};
                CHECK_THROW_MSG( file.IsOpen(),
                    "Failed to open shader file: '"s << ToString(_pplnPath) << "'" );

                CHECK_THROW_MSG( file.Read( file.RemainingSize(), OUT fs ),
                    "Failed to read shader file '"s << ToString(_pplnPath) << "'" );

                header >> fs;
                fs_line = uint(Parser::CalculateNumberOfLines( header )) - 1;
            }
        }

        const EShaderOpt    sh_opt = EShaderOpt::Optimize | EShaderOpt::OptimizeSize;
    //  const EShaderOpt    sh_opt = EShaderOpt::DebugInfo; // for shader debugging in RenderDoc

        _CompilePipeline3( subpass, vs, fs, fs_line, "postprocess", uint(sh_opt), EPipelineOpt::Optimize );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
            _CompilePipeline3( subpass, vs, fs, fs_line, "postprocess.Trace", uint(sh_opt | EShaderOpt::Trace), Default );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
            _CompilePipeline3( subpass, vs, fs, fs_line, "postprocess.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
            _CompilePipeline3( subpass, vs, fs, fs_line, "postprocess.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default );
    }

/*
=================================================
    _CompilePipeline3
=================================================
*/
    void  ScriptPostprocess::_CompilePipeline3 (const String &subpass, const String &vs, const String &fs, uint fsLine,
                                                const String &pplnName, uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
    {
        PipelineLayoutPtr       ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
        ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );

        if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
            ppln_layout->AddDebugDSLayout2( 1, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::Fragment) );

        GraphicsPipelinePtr     ppln_templ{ new GraphicsPipelineScriptBinding{ pplnName }};
        ppln_templ->SetFragmentOutputFromRenderPass( "compat.rp", subpass );
        ppln_templ->SetLayout2( ppln_layout );

        {
            ScriptShaderPtr sh{ new ScriptShader{}};
            sh->SetSource( EShader::Vertex, vs );
            ppln_templ->SetVertexShader( sh );
        }
        {
            ScriptShaderPtr sh{ new ScriptShader{}};
            BEGIN_ENUM_CHECKS();
            switch ( _ppFlags )
            {
                case EPostprocess::Shadertoy :                                                      break;
                case EPostprocess::ShadertoyVR :        sh->Define( "#define VIEW_MODE_VR" );       break;
                case EPostprocess::ShadertoyVR_180 :    sh->Define( "#define VIEW_MODE_VR180" );    break;
                case EPostprocess::ShadertoyVR_360 :    sh->Define( "#define VIEW_MODE_VR360" );    break;
                case EPostprocess::Shadertoy_360 :      sh->Define( "#define VIEW_MODE_360" );      break;
                case EPostprocess::Unknown :            break;
                case EPostprocess::_Last :
                case EPostprocess::_ShadertoyBits :
                default :                               CHECK_THROW_MSG( false, "unsupported flags" );
            }
            END_ENUM_CHECKS();
            sh->SetSource2( EShader::Fragment, fs, PathAndLine{_pplnPath, fsLine} );
            sh->options = EShaderOpt(shaderOpts);

            ppln_templ->SetFragmentShader( sh );
        }
        {
            GraphicsPipelineSpecPtr ppln_spec = ppln_templ->AddSpecialization2( pplnName );
            ppln_spec->AddToRenderTech( "rtech", subpass );
            ppln_spec->SetViewportCount( 1 );
            ppln_spec->SetOptions( pplnOpt );

            RenderState     rs;
            rs.inputAssembly.topology = EPrimitive::TriangleList;

            for (usize i = 0; i < _output.size(); ++i)
            {
                const auto& src = _output[i];
                auto&       dst = rs.color.buffers[i];

                dst.blend = src.enableBlend;

                if ( src.enableBlend )
                {
                    dst.blendOp.color           = src.blendOpRGB;
                    dst.blendOp.alpha           = src.blendOpA;

                    dst.srcBlendFactor.color    = src.srcFactorRGB;
                    dst.srcBlendFactor.alpha    = src.srcFactorA;
                    dst.dstBlendFactor.color    = src.dstFactorRGB;
                    dst.dstBlendFactor.alpha    = src.dstFactorA;
                }
            }

            ppln_spec->SetRenderState( rs );
        }
    }


} // AE::ResEditor
