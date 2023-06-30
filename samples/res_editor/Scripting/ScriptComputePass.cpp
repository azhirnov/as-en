// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/EditorUI.h"

#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"
#include "res_editor/Scripting/ScriptBasePass.cpp.h"

namespace AE::ResEditor
{
namespace
{
    static ScriptComputePass*  ScriptComputePass_Ctor1 () {
        return ScriptComputePassPtr{ new ScriptComputePass{ Default, Default, Default }}.Detach();
    }

    static ScriptComputePass*  ScriptComputePass_Ctor2 (const String &name) {
        return ScriptComputePassPtr{ new ScriptComputePass{ name, Default, Default }}.Detach();
    }

    static ScriptComputePass*  ScriptComputePass_Ctor3 (const String &name, const String &defines) {
        return ScriptComputePassPtr{ new ScriptComputePass{ name, defines, Default }}.Detach();
    }

    static ScriptComputePass*  ScriptComputePass_Ctor4 (const String &name, ScriptBasePass::EFlags baseFlags) {
        return ScriptComputePassPtr{ new ScriptComputePass{ name, Default, baseFlags }}.Detach();
    }

    static ScriptComputePass*  ScriptComputePass_Ctor5 (const String &name, const String &defines, ScriptBasePass::EFlags baseFlags) {
        return ScriptComputePassPtr{ new ScriptComputePass{ name, defines, baseFlags }}.Detach();
    }

} // namespace


/*
=================================================
    constructor
=================================================
*/
    ScriptComputePass::ScriptComputePass (const String &name, const String &defines, EFlags baseFlags) __Th___ :
        ScriptBasePass{ baseFlags },
        _pplnPath{ ScriptExe::ScriptPassApi::ToShaderPath( name )},
        _defines{ defines }
    {
        _dbgName = ToString( _pplnPath.filename().replace_extension("") );

        if ( not _defines.empty() )
            _dbgName << "|" << _defines;

        FindAndReplace( INOUT _defines, '=', ' ' );

        ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );

        CHECK_THROW_MSG( FileSystem::IsFile( _pplnPath ),
            "File '"s << name << "' is not exists" );
    }

/*
=================================================
    LocalSize*
=================================================
*/
    void  ScriptComputePass::LocalSize3v (const packed_uint3 &v) __Th___
    {
        _localSize = v;

        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( _iterations.empty(), "LocalSize() must be used before Dispatch() call" );
    }

/*
=================================================
    DispatchGroups*
=================================================
*/
    void  ScriptComputePass::DispatchGroups3v (const packed_uint3 &groupCount) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( All( groupCount > 0u ), "'groupCount' must be > 0" );

        auto&   it = _iterations.emplace_back();
        it.count    = groupCount;
        it.isGroups = true;
    }

    void  ScriptComputePass::DispatchGroupsDS  (const ScriptDynamicDimPtr &ds) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( ds );

        auto&   it = _iterations.emplace_back();
        it.count    = ds->Get();
        it.isGroups = true;
    }

    void  ScriptComputePass::DispatchGroups1D (const ScriptDynamicUIntPtr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = true;
    }

/*
=================================================
    DispatchThreads*
=================================================
*/
    void  ScriptComputePass::DispatchThreads3v (const packed_uint3 &threads) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( All( uint3{threads} >= _localSize ), "'threads' must be >= LocalSize()" );

        auto&   it = _iterations.emplace_back();
        it.count    = threads;
        it.isGroups = false;
    }

    void  ScriptComputePass::DispatchThreadsDS (const ScriptDynamicDimPtr &ds) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( ds );

        auto&   it = _iterations.emplace_back();
        it.count    = ds->Get();
        it.isGroups = false;
    }

    void  ScriptComputePass::DispatchThreads1D (const ScriptDynamicUIntPtr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = false;
    }

/*
=================================================
    ArgSceneIn
=================================================
*/
    void  ScriptComputePass::ArgSceneIn (const String &name, const ScriptRTScenePtr &scene) __Th___
    {
        CHECK_THROW_MSG( scene );

        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );
        CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

        Argument&   arg = _args.emplace_back();
        arg.name    = name;
        arg.res     = scene;
        arg.state   = EResourceState::ComputeShader | EResourceState::ShaderRTAS_Read;
    }

/*
=================================================
    ArgBufferIn***
=================================================
*/
    void  ScriptComputePass::ArgBufferIn (const String &name, const ScriptBufferPtr &buf)   __Th___ { _AddArg( name, buf, EResourceUsage::ComputeRead ); }
    void  ScriptComputePass::ArgBufferOut (const String &name, const ScriptBufferPtr &buf)  __Th___ { _AddArg( name, buf, EResourceUsage::ComputeWrite ); }
    void  ScriptComputePass::ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)__Th___ { _AddArg( name, buf, EResourceUsage::ComputeRW ); }

    void  ScriptComputePass::_AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage) __Th___
    {
        CHECK_THROW_MSG( buf );
        buf->AddUsage( usage );

        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );
        CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

        Argument&   arg = _args.emplace_back();
        arg.name    = name;
        arg.res     = buf;
        arg.state   = EResourceState::ComputeShader;

        switch ( usage ) {
            case EResourceUsage::ComputeRead :  arg.state |= EResourceState::ShaderStorage_Read;    break;
            case EResourceUsage::ComputeWrite : arg.state |= EResourceState::ShaderAddress_Write;   break;
            case EResourceUsage::ComputeRW :    arg.state |= EResourceState::ShaderStorage_RW;      break;
            default :                           CHECK_THROW_MSG( false, "unsupported usage" );
        }
    }

/*
=================================================
    ArgImageIn***
=================================================
*/
    void  ScriptComputePass::ArgImageIn (const String &name, const ScriptImagePtr &img)     __Th___ { _AddArg( name, img, EResourceUsage::ComputeRead ); }
    void  ScriptComputePass::ArgImageOut (const String &name, const ScriptImagePtr &img)    __Th___ { _AddArg( name, img, EResourceUsage::ComputeWrite ); }
    void  ScriptComputePass::ArgImageInOut (const String &name, const ScriptImagePtr &img)  __Th___ { _AddArg( name, img, EResourceUsage::ComputeRW ); }

    void  ScriptComputePass::_AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage) __Th___
    {
        CHECK_THROW_MSG( img );
        img->AddUsage( usage );

        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );
        CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

        Argument&   arg = _args.emplace_back();
        arg.name    = name;
        arg.res     = img;
        arg.state   = EResourceState::ComputeShader;

        switch ( usage ) {
            case EResourceUsage::ComputeRead :  arg.state |= EResourceState::ShaderStorage_Read;    break;
            case EResourceUsage::ComputeWrite : arg.state |= EResourceState::ShaderAddress_Write;   break;
            case EResourceUsage::ComputeRW :    arg.state |= EResourceState::ShaderStorage_RW;      break;
            default :                           CHECK_THROW_MSG( false, "unsupported usage" );
        }
    }

/*
=================================================
    ArgTextureIn
=================================================
*/
    void  ScriptComputePass::ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName) __Th___
    {
        CHECK_THROW_MSG( tex );
        CHECK_THROW_MSG( not samplerName.empty() );
        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );
        CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

        tex->AddUsage( EResourceUsage::Sampled );

        Argument&   arg = _args.emplace_back();
        arg.name        = name;
        arg.res         = tex;
        arg.state       = EResourceState::ComputeShader | EResourceState::ShaderSample;
        arg.samplerName = samplerName;
    }

/*
=================================================
    ArgVideoIn
=================================================
*/
    void  ScriptComputePass::ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName) __Th___
    {
        CHECK_THROW_MSG( tex );
        CHECK_THROW_MSG( not samplerName.empty() );
        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );
        CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

        tex->AddUsage( EResourceUsage::Sampled );

        Argument&   arg = _args.emplace_back();
        arg.name        = name;
        arg.res         = tex;
        arg.state       = EResourceState::ComputeShader | EResourceState::ShaderSample;
        arg.samplerName = samplerName;
    }

/*
=================================================
    ArgController
=================================================
*/
    void  ScriptComputePass::ArgController (const ScriptBaseControllerPtr &controller) __Th___
    {
        CHECK_THROW_MSG( controller );
        CHECK_THROW_MSG( not _controller, "controller is already exists" );

        _controller = controller;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptComputePass::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptComputePass>   binder{ se };
        binder.CreateRef( 0, False{"ctor"} );
        ScriptBasePass::_BindBase( binder );

        binder.AddFactoryCtor( &ScriptComputePass_Ctor1 );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor2 );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor3 );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor4 );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor5 );

        binder.AddMethod( &ScriptComputePass::LocalSize1,           "LocalSize"         );
        binder.AddMethod( &ScriptComputePass::LocalSize2,           "LocalSize"         );
        binder.AddMethod( &ScriptComputePass::LocalSize3,           "LocalSize"         );
        binder.AddMethod( &ScriptComputePass::LocalSize2v,          "LocalSize"         );
        binder.AddMethod( &ScriptComputePass::LocalSize3v,          "LocalSize"         );

        binder.AddMethod( &ScriptComputePass::DispatchGroups1,      "DispatchGroups"    );
        binder.AddMethod( &ScriptComputePass::DispatchGroups2,      "DispatchGroups"    );
        binder.AddMethod( &ScriptComputePass::DispatchGroups3,      "DispatchGroups"    );
        binder.AddMethod( &ScriptComputePass::DispatchGroups2v,     "DispatchGroups"    );
        binder.AddMethod( &ScriptComputePass::DispatchGroups3v,     "DispatchGroups"    );
        binder.AddMethod( &ScriptComputePass::DispatchGroupsDS,     "DispatchGroups"    );
        binder.AddMethod( &ScriptComputePass::DispatchGroups1D,     "DispatchGroups"    );

        binder.AddMethod( &ScriptComputePass::DispatchThreads1,     "DispatchThreads"   );
        binder.AddMethod( &ScriptComputePass::DispatchThreads2,     "DispatchThreads"   );
        binder.AddMethod( &ScriptComputePass::DispatchThreads3,     "DispatchThreads"   );
        binder.AddMethod( &ScriptComputePass::DispatchThreads2v,    "DispatchThreads"   );
        binder.AddMethod( &ScriptComputePass::DispatchThreads3v,    "DispatchThreads"   );
        binder.AddMethod( &ScriptComputePass::DispatchThreadsDS,    "DispatchThreads"   );
        binder.AddMethod( &ScriptComputePass::DispatchThreads1D,    "DispatchThreads"   );

        binder.AddMethod( &ScriptComputePass::ArgSceneIn,           "ArgIn"             );

        binder.AddMethod( &ScriptComputePass::ArgBufferIn,          "ArgIn"             );
        binder.AddMethod( &ScriptComputePass::ArgBufferOut,         "ArgOut"            );
        binder.AddMethod( &ScriptComputePass::ArgBufferInOut,       "ArgInOut"          );

        binder.AddMethod( &ScriptComputePass::ArgImageIn,           "ArgIn"             );
        binder.AddMethod( &ScriptComputePass::ArgImageOut,          "ArgOut"            );
        binder.AddMethod( &ScriptComputePass::ArgImageInOut,        "ArgInOut"          );

        binder.AddMethod( &ScriptComputePass::ArgTextureIn,         "ArgIn"             );
        binder.AddMethod( &ScriptComputePass::ArgVideoIn,           "ArgIn"             );
        binder.AddMethod( &ScriptComputePass::ArgController,        "ArgIn"             );
    }

/*
=================================================
    _CompilePipeline
=================================================
*/
    auto  ScriptComputePass::_CompilePipeline (OUT Bytes &ubSize) C_Th___
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
    RC<IPass>  ScriptComputePass::ToPass () C_Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() is not used" );
        CHECK_THROW_MSG( not _iterations.empty(), "add at least one Dispatch() call" );
        CHECK_THROW_MSG( not _args.empty(), "empty argument list" );

        auto        result      = MakeRC<ComputePass>();
        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        Renderer&   renderer    = ScriptExe::ScriptPassApi::GetRenderer();  // throw
        const auto  max_frames  = RenderTaskScheduler().GetMaxFrames();
        Bytes       ub_size;

        result->_rtech = _CompilePipeline( OUT ub_size );   // throw

        EnumBitSet<IPass::EDebugMode>   dbg_modes;

        const auto  AddPpln = [cp = result.get(), &dbg_modes] (IPass::EDebugMode mode, ComputePipelineID id)
        {{
            if ( id ) {
                cp->_pipelines.insert_or_assign( mode, id );
                dbg_modes.insert( mode );
            }
        }};

        auto    ppln = result->_rtech.rtech->GetComputePipeline( PipelineName{"compute"} );
        CHECK_THROW( ppln );
        AddPpln( IPass::EDebugMode::Unknown,        ppln );
        AddPpln( IPass::EDebugMode::Trace,          result->_rtech.rtech->GetComputePipeline( PipelineName{"compute.Trace"} ));
        AddPpln( IPass::EDebugMode::FnProfiling,    result->_rtech.rtech->GetComputePipeline( PipelineName{"compute.FnProf"} ));
        AddPpln( IPass::EDebugMode::TimeHeatMap,    result->_rtech.rtech->GetComputePipeline( PipelineName{"compute.TmProf"} ));

        result->_dbgName    = this->_dbgName;
        result->_dbgColor   = this->_dbgColor;
        result->_localSize  = this->_localSize;
        result->_iterations = this->_iterations;

        if ( this->_controller )
        {
            RC<DynamicDim>  dyn_dim = Iteration::FindDynamicThreadCount( this->_iterations );
            if ( not dyn_dim )
                dyn_dim = MakeRC<DynamicDim>( Iteration::FindMaxConstThreadCount( this->_iterations, this->_localSize ));

            result->_controller = this->_controller->ToController( dyn_dim );  // throw
            CHECK_THROW( result->_controller );
        }

        result->_ubuffer = res_mngr.CreateBuffer( BufferDesc{ ub_size, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                  "ComputePassUB", renderer.GetAllocator() );
        CHECK_THROW( result->_ubuffer );

        // create descriptor set
        {
            CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_dsIndex, OUT result->_descSets.data(), max_frames,
                                                        ppln, DescriptorSetName{"ds0"} ));

            for (auto& arg : _args)
            {
                Visit( arg.res,
                    [&] (ScriptBufferPtr buf) {
                        auto    res = buf->ToResource();
                        CHECK_THROW( res );
                        result->_resources.emplace_back( UniformName{arg.name}, res, arg.state );
                    },
                    [&] (ScriptImagePtr tex) {
                        auto    res = tex->ToResource();
                        CHECK_THROW( res );
                        result->_resources.emplace_back( UniformName{arg.name}, res, arg.state );
                    },
                    [&] (ScriptVideoImagePtr video) {
                        auto    res = video->ToResource();
                        CHECK_THROW( res );
                        result->_resources.emplace_back( UniformName{arg.name}, res, arg.state );
                    },
                    [&] (ScriptRTScenePtr scene) {
                        auto    res = scene->ToResource();
                        CHECK_THROW( res );
                        result->_resources.emplace_back( UniformName{arg.name}, res, arg.state );
                    },
                    [] (NullUnion) {
                        CHECK_THROW_MSG( false, "unsupported argument type" );
                    }
                );
            }
        }

        // add debug modes
        UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Compute );

        _AddSlidersToUIInteraction( result.get() );
        _CopyConstants( OUT result->_shConst );

        AE_LOGI( "Compiled: "s << _dbgName );
        return result;
    }


} // AE::ResEditor


#include "res_editor/Scripting/PassCommon.inl.h"

#include "base/DataSource/FileStream.h"
#include "base/Algorithms/StringParser.h"

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
    auto  ScriptComputePass::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "ComputePassUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"ComputePassUB"}};
        st->Set( EStructLayout::Std140, R"#(
                float       time;                   // shader playback time (in seconds)
                float       timeDelta;              // render time (in seconds)
                int         frame;                  // shader playback frame
                float4      mouse;                  // mouse pixel coords. xy: current (if MRB down), zw: click

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
        return st;
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptComputePass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
    {
        auto    st = _CreateUBType();   // throw

        CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
    }

/*
=================================================
    _CompilePipeline3
=================================================
*/
    void  ScriptComputePass::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
    {
        // validate
        for (auto& arg : _args)
        {
            Visit( arg.res,
                [] (ScriptBufferPtr buf)        { buf->AddLayoutReflection();  CHECK_THROW_MSG( buf->ToResource() ); },
                [] (ScriptImagePtr tex)         { CHECK_THROW_MSG( tex->ToResource() ); },
                [] (ScriptVideoImagePtr video)  { CHECK_THROW_MSG( video->ToResource() ); },
                [] (ScriptRTScenePtr scene)     { CHECK_THROW_MSG( scene->ToResource() ); },
                [] (NullUnion)                  { CHECK_THROW_MSG( false, "unsupported argument type" ); }
            );
        }

        RenderTechniquePtr  rtech{ new RenderTechnique{ "rtech" }};
        {
            RTComputePassPtr    pass = rtech->AddComputePass2( "Compute" );
            Unused( pass );
        }

        const uint              stage   = uint(EShaderStages::Compute);
        DescriptorSetLayoutPtr  ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
        ds_layout->AddFeatureSet( "Default" );
        {
            ShaderStructTypePtr st = _CreateUBType();   // throw
            ubSize = st->StaticSize();

            ds_layout->AddUniformBuffer( stage, "ub", ArraySize{1}, "ComputePassUB", EResourceState::ShaderUniform );
        }

        for (auto& arg : _args)
        {
            Visit( arg.res,
                [&] (ScriptBufferPtr buf) {
                    if ( buf->HasLayout() )
                        ds_layout->AddStorageBuffer( stage, arg.name, ArraySize{1}, buf->GetTypeName(), EAccessType::Coherent, arg.state );
                    else
                        ds_layout->AddStorageTexelBuffer( stage, arg.name, ArraySize{1}, PipelineCompiler::EImageType(buf->TexelBufferType()), arg.state );
                },
                [&] (ScriptImagePtr tex) {
                    const auto  type = PipelineCompiler::EImageType(tex->ImageType());
                    if ( arg.samplerName.empty() )
                        ds_layout->AddStorageImage( stage, arg.name, ArraySize{1}, type, tex->Description().format, EAccessType::Coherent, arg.state );
                    else
                        ds_layout->AddCombinedImage_ImmutableSampler( stage, arg.name, type, arg.state, arg.samplerName );
                },
                [&] (ScriptVideoImagePtr video) {
                    ds_layout->AddCombinedImage_ImmutableSampler( stage, arg.name, PipelineCompiler::EImageType(video->ImageType()), arg.state, arg.samplerName );
                },
                [&] (ScriptRTScenePtr scene) {
                    ds_layout->AddRayTracingScene( stage, arg.name, ArraySize{1} );
                },
                [] (NullUnion) {
                    CHECK_THROW_MSG( false, "unsupported argument type" );
                }
            );
        }

        uint    cs_line = 0;
        String  cs;
        {
            String  header;

            // add defines
            if ( not _defines.empty() )
            {
                Array<StringView>   def_tokens;
                StringParser::Tokenize( _defines, ';', OUT def_tokens );

                for (auto def : def_tokens) {
                    header << "#define " << def << '\n';
                }
            }

            // add sliders
            {
                const uint  max_sliders = UIInteraction::MaxSlidersPerType;
                for (usize i = 0; i < _sliderCounter.size(); ++i) {
                    CHECK_THROW_MSG( _sliderCounter[i] <= max_sliders );
                }

                for (auto& slider : _sliders)
                {
                    header << "#define " << slider.name << " ub.";
                    BEGIN_ENUM_CHECKS();
                    switch ( slider.type )
                    {
                        case ESlider::Int :     header << "intSliders[";    break;
                        case ESlider::Float :   header << "floatSliders[";  break;
                        case ESlider::Color :   header << "colors[";        break;
                        case ESlider::_Count :
                        default :               CHECK_THROW_MSG( false, "unknown slider type" );
                    }
                    END_ENUM_CHECKS();

                    header << ToString( slider.index ) << "]";
                    switch ( slider.count )
                    {
                        case 1 :    header << ".x";     break;
                        case 2 :    header << ".xy";    break;
                        case 3 :    header << ".xyz";   break;
                        case 4 :    header << ".xyzw";  break;
                        default :   CHECK_THROW_MSG( false, "unknown slider value size" );
                    }
                    header << "\n";
                }
            }

            // add constants
            {
                for (auto& c : _constants)
                {
                    header << "#define " << c.name << " ub.";
                    BEGIN_ENUM_CHECKS();
                    switch ( c.type )
                    {
                        case ESlider::Int :     header << "intConst[";      break;
                        case ESlider::Float :   header << "floatConst[";    break;
                        case ESlider::Color :
                        case ESlider::_Count :
                        default :               CHECK_THROW_MSG( false, "unknown constant type" );
                    }
                    END_ENUM_CHECKS();

                    header << ToString( c.index ) << "]\n";
                }
            }

            // load shader source from file
            {
                FileRStream     file {_pplnPath};
                CHECK_THROW_MSG( file.IsOpen(),
                    "Failed to open shader file: '"s << ToString(_pplnPath) << "'" );

                CHECK_THROW_MSG( file.Read( file.RemainingSize(), OUT cs ),
                    "Failed to read shader file '"s << ToString(_pplnPath) << "'" );

                header >> cs;
                cs_line = uint(StringParser::CalculateNumberOfLines( header )) - 1;
            }
        }

        const EShaderOpt    sh_opt = EShaderOpt::Optimize;
    //  const EShaderOpt    sh_opt = EShaderOpt::DebugInfo; // for shader debugging in RenderDoc

        _CompilePipeline3( cs, cs_line, "compute", uint(sh_opt), EPipelineOpt::Optimize );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
            _CompilePipeline3( cs, cs_line, "compute.Trace", uint(sh_opt | EShaderOpt::Trace), Default );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
            _CompilePipeline3( cs, cs_line, "compute.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
            _CompilePipeline3( cs, cs_line, "compute.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default );
    }

/*
=================================================
    _CompilePipeline3
=================================================
*/
    void  ScriptComputePass::_CompilePipeline3 (const String &cs, uint line, const String &pplnName,
                                                uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
    {
        PipelineLayoutPtr       ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
        ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );

        if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
            ppln_layout->AddDebugDSLayout2( 1, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::Compute) );

        ComputePipelinePtr      ppln_templ{ new ComputePipelineScriptBinding{ pplnName }};
        ppln_templ->SetLayout2( ppln_layout );

        {
            ScriptShaderPtr sh{ new ScriptShader{}};
            sh->SetSource2( EShader::Compute, cs, PathAndLine{_pplnPath, line} );
            sh->options = EShaderOpt(shaderOpts);
            sh->SetComputeLocalSize3( _localSize.x, _localSize.y, _localSize.z );

            ppln_templ->SetShader( sh );
        }
        {
            ComputePipelineSpecPtr  ppln_spec = ppln_templ->AddSpecialization2( pplnName );
            ppln_spec->AddToRenderTech( "rtech", "Compute" );
            ppln_spec->SetOptions( pplnOpt );
        }
    }


} // AE::ResEditor
