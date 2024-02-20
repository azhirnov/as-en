// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/ScriptBasePass.cpp.h"
#include "res_editor/_data/cpp/types.h"

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
    operator Iteration
=================================================
*/
    ScriptComputePass::Iteration::operator ComputePass::Iteration () C_Th___
    {
        ComputePass::Iteration  result;
        result.count    = count;
        result.isGroups = isGroups;

        if ( indirect )
        {
            result.indirect         = indirect->ToResource();   // throw
            result.indirectOffset   = indirectOffset;

            if ( not indirectCmdField.empty() )
            {
                ASSERT( indirectOffset == 0 );
                result.indirectOffset = indirect->GetFieldOffset( indirectCmdField );   // throw
            }
        }
        return result;
    }

/*
=================================================
    constructor
=================================================
*/
    ScriptComputePass::ScriptComputePass (const String &name, const String &defines, EFlags baseFlags) __Th___ :
        ScriptBasePass{ baseFlags },
        _pplnPath{ ScriptExe::ScriptPassApi::ToShaderPath( name )}
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
        CHECK_THROW_MSG( ds and ds->Get() );

        auto&   it = _iterations.emplace_back();
        it.count    = ds->Get();
        it.isGroups = true;

        ScriptDynamicDimPtr ds2;
        ds2.Attach( ds->Mul3( packed_int3{_localSize} ));
        _SetDynamicDimension( ds2 );
    }

    void  ScriptComputePass::DispatchGroups1D (const ScriptDynamicUIntPtr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = true;
    }

    void  ScriptComputePass::DispatchGroups2D (const ScriptDynamicUInt2Ptr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = true;
    }

    void  ScriptComputePass::DispatchGroups3D (const ScriptDynamicUInt3Ptr &dyn) __Th___
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
        CHECK_THROW_MSG( ds and ds->Get() );

        auto&   it = _iterations.emplace_back();
        it.count    = ds->Get();
        it.isGroups = false;

        _SetDynamicDimension( ds );
    }

    void  ScriptComputePass::DispatchThreads1D (const ScriptDynamicUIntPtr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = false;
    }

    void  ScriptComputePass::DispatchThreads2D (const ScriptDynamicUInt2Ptr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = false;
    }

    void  ScriptComputePass::DispatchThreads3D (const ScriptDynamicUInt3Ptr &dyn) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( dyn );

        auto&   it = _iterations.emplace_back();
        it.count    = dyn->Get();
        it.isGroups = false;
    }

/*
=================================================
    DispatchGroupsIndirect*
=================================================
*/
    void  ScriptComputePass::DispatchGroupsIndirect1 (const ScriptBufferPtr &ibuf) __Th___
    {
        DispatchGroupsIndirect2( ibuf, 0 );
    }

    void  ScriptComputePass::DispatchGroupsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( ibuf );

        auto&   it          = _iterations.emplace_back();
        it.indirect         = ibuf;
        it.indirectOffset   = Bytes{offset};
        it.isGroups         = true;
    }

    void  ScriptComputePass::DispatchGroupsIndirect3 (const ScriptBufferPtr &ibuf, const String &field) __Th___
    {
        CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not field.empty() );

        auto&   it          = _iterations.emplace_back();
        it.indirect         = ibuf;
        it.indirectCmdField = field;
        it.isGroups         = true;
    }

/*
=================================================
    _OnAddArg
=================================================
*/
    void  ScriptComputePass::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
    {
        CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

        arg.state |= EResourceState::ComputeShader;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptComputePass::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptComputePass>   binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
        ScriptBasePass::_BindBase( binder, True{"withArgs"} );

        binder.AddFactoryCtor( &ScriptComputePass_Ctor1,    {} );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor2,    {"shaderPath"} );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor3,    {"shaderPath", "defines"} );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor4,    {"shaderPath", "passFlags"} );
        binder.AddFactoryCtor( &ScriptComputePass_Ctor5,    {"shaderPath", "defines", "passFlags"} );

        binder.Comment( "Set workgroup size - number of threads which can access shared memory." );
        binder.AddMethod( &ScriptComputePass::LocalSize1,               "LocalSize",            {"x"} );
        binder.AddMethod( &ScriptComputePass::LocalSize2,               "LocalSize",            {"x", "y"} );
        binder.AddMethod( &ScriptComputePass::LocalSize3,               "LocalSize",            {"x", "y", "z"} );
        binder.AddMethod( &ScriptComputePass::LocalSize2v,              "LocalSize",            {} );
        binder.AddMethod( &ScriptComputePass::LocalSize3v,              "LocalSize",            {} );

        binder.Comment( "Execute compute shader with number of the workgroups.\n"
                        "Total number of threads is 'groupCount * localSize'." );
        binder.AddMethod( &ScriptComputePass::DispatchGroups1,          "DispatchGroups",       {"groupCountX"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups2,          "DispatchGroups",       {"groupCountX", "groupCountY"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups3,          "DispatchGroups",       {"groupCountX", "groupCountY", "groupCountZ"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups2v,         "DispatchGroups",       {"groupCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups3v,         "DispatchGroups",       {"groupCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroupsDS,         "DispatchGroups",       {"dynamicGroupCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups1D,         "DispatchGroups",       {"dynamicGroupCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups2D,         "DispatchGroups",       {"dynamicGroupCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroups3D,         "DispatchGroups",       {"dynamicGroupCount"} );

        binder.Comment( "Execute compute shader with total number of the threads." );
        binder.AddMethod( &ScriptComputePass::DispatchThreads1,         "DispatchThreads",      {"threadsX"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads2,         "DispatchThreads",      {"threadsX", "threadsY"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads3,         "DispatchThreads",      {"threadsX", "threadsY", "threadsZ"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads2v,        "DispatchThreads",      {"threads"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads3v,        "DispatchThreads",      {"threads"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreadsDS,        "DispatchThreads",      {"dynamicThreadCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads1D,        "DispatchThreads",      {"dynamicThreadCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads2D,        "DispatchThreads",      {"dynamicThreadCount"} );
        binder.AddMethod( &ScriptComputePass::DispatchThreads3D,        "DispatchThreads",      {"dynamicThreadCount"} );

        binder.Comment( "Execute compute shader with indirect command.\n"
                        "Indirect buffer must contains 'DispatchIndirectCommand' data." );
        binder.AddMethod( &ScriptComputePass::DispatchGroupsIndirect1,  "DispatchGroupsIndirect",   {"indirectBuffer"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroupsIndirect2,  "DispatchGroupsIndirect",   {"indirectBuffer", "indirectBufferOffset"} );
        binder.AddMethod( &ScriptComputePass::DispatchGroupsIndirect3,  "DispatchGroupsIndirect",   {"indirectBuffer", "indirectBufferField"} );
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
        CHECK_THROW_MSG( not _args.Empty(), "empty argument list" );

        auto        result      = MakeRC<ComputePass>();
        auto&       res_mngr    = GraphicsScheduler().GetResourceManager();
        Renderer&   renderer    = ScriptExe::ScriptPassApi::GetRenderer();  // throw
        const auto  max_frames  = GraphicsScheduler().GetMaxFrames();
        Bytes       ub_size;

        result->_rtech = _CompilePipeline( OUT ub_size );   // throw

        EnumSet<IPass::EDebugMode>  dbg_modes;

        const auto  AddPpln = [this, cp = result.get(), &dbg_modes] (IPass::EDebugMode mode, EFlags flag, PipelineName::Ref name)
        {{
            if ( AllBits( _baseFlags, flag ))
            {
                auto    id = cp->_rtech.rtech->GetComputePipeline( name );
                if ( id ) {
                    cp->_pipelines.insert_or_assign( mode, id );
                    dbg_modes.insert( mode );
                }
            }
        }};

        AddPpln( IPass::EDebugMode::Unknown,        EFlags::Unknown,                PipelineName{"compute"} );
        AddPpln( IPass::EDebugMode::Trace,          EFlags::Enable_ShaderTrace,     PipelineName{"compute.Trace"} );
        AddPpln( IPass::EDebugMode::FnProfiling,    EFlags::Enable_ShaderFnProf,    PipelineName{"compute.FnProf"} );
        AddPpln( IPass::EDebugMode::TimeHeatMap,    EFlags::Enable_ShaderTmProf,    PipelineName{"compute.TmProf"} );

        auto    ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second;

        #if PIPELINE_STATISTICS
        {
            auto&   res = res_mngr.GetResourcesOrThrow( ppln );
            Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
        }
        #endif

        result->_localSize  = this->_localSize;
        result->_iterations.assign( this->_iterations.begin(), this->_iterations.end() );

        result->_ubuffer = res_mngr.CreateBuffer( BufferDesc{ ub_size, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                  "ComputePassUB", renderer.GetAllocator() );
        CHECK_THROW( result->_ubuffer );

        // create descriptor set
        {
            CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_dsIndex, OUT result->_descSets.data(), max_frames,
                                                        ppln, DescriptorSetName{"ds0"} ));
            _args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw
        }

        result->_pcIndex = res_mngr.GetPushConstantIndex<ShaderTypes::ComputePassPC>( ppln, PushConstantName{"pc"} );
        CHECK_THROW( result->_pcIndex );

        _Init( *result, null );
        UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Compute );

        return result;
    }


} // AE::ResEditor


#include "res_editor/Scripting/PipelineCompiler.inl.h"

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
    auto  ScriptComputePass::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "ComputePassUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"ComputePassUB"}};
        st->Set( EStructLayout::Std140, R"#(
                float       time;           // shader playback time (in seconds)
                float       timeDelta;      // render time (in seconds)
                uint        frame;          // shader playback frame
                uint        seed;           // unique value, updated on each shader reloading
                float4      mouse;          // mouse unorm coords. xy: current (if MRB down), zw: click
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

        StaticAssert( UIInteraction::MaxSlidersPerType == 4 );
        StaticAssert( IPass::Constants::MaxCount == 4 );
        StaticAssert( IPass::CustomKeys_t{}.max_size() == 1 );
        return st;
    }

/*
=================================================
    _CreatePCType
=================================================
*/
    auto  ScriptComputePass::_CreatePCType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "ComputePassPC" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"ComputePassPC"}};
        st->Set( EStructLayout::Std140, R"#(
                uint    dispatchIndex;
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
            auto    st = _CreateUBType();   // throw
            CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
        }{
            auto    st = _CreatePCType();   // throw
            CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
        }
    }

/*
=================================================
    _CompilePipeline3
=================================================
*/
    void  ScriptComputePass::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
    {
        _args.ValidateArgs();

        RenderTechniquePtr  rtech{ new RenderTechnique{ "rtech" }};
        {
            RTComputePassPtr    pass = rtech->AddComputePass2( "Compute" );
            Unused( pass );
        }

        const auto              stage   = EShaderStages::Compute;
        DescriptorSetLayoutPtr  ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
        {
            ShaderStructTypePtr st = _CreateUBType();   // throw
            ubSize = st->StaticSize();

            ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "ComputePassUB", EResourceState::ShaderUniform, False{} );
        }
        _args.ArgsToDescSet( stage, ds_layout, ArraySize{1}, EAccessType::Coherent );  // throw


        uint    cs_line = 0;
        String  cs;
        {
            String  header;

            _AddDefines( _defines, INOUT header );
            _AddSliders( INOUT header );

            // load shader source from file
            {
                FileRStream     file {_pplnPath};
                CHECK_THROW_MSG( file.IsOpen(),
                    "Failed to open shader file: '"s << ToString(_pplnPath) << "'" );

                CHECK_THROW_MSG( file.Read( file.RemainingSize(), OUT cs ),
                    "Failed to read shader file '"s << ToString(_pplnPath) << "'" );

                header >> cs;
                cs_line = uint(Parser::CalculateNumberOfLines( header )) - 1;
            }
        }

        const EShaderOpt    sh_opt  = EShaderOpt::Optimize;
    //  const EShaderOpt    sh_opt  = EShaderOpt::DebugInfo;    // for shader debugging in RenderDoc

      #if PIPELINE_STATISTICS
        const EPipelineOpt  ppln_opt = EPipelineOpt::Optimize | EPipelineOpt::CaptureStatistics | EPipelineOpt::CaptureInternalRepresentation;
      #else
        const EPipelineOpt  ppln_opt = EPipelineOpt::Optimize;
      #endif

        _CompilePipeline3( cs, cs_line, "compute", uint(sh_opt), ppln_opt );

      #ifdef AE_ENABLE_GLSL_TRACE
        if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
            _CompilePipeline3( cs, cs_line, "compute.Trace", uint(sh_opt | EShaderOpt::Trace), Default );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
            _CompilePipeline3( cs, cs_line, "compute.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default );

        if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
            _CompilePipeline3( cs, cs_line, "compute.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default );
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
        PipelineLayoutPtr       ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
        ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );
        ppln_layout->AddPushConst2( "pc", _CreatePCType(), EShader::Compute );

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
