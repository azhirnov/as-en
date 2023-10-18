// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/MeshPipeline.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
    static MeshPipelineScriptBinding*  MeshPipelineScriptBinding_Ctor (const String &name) {
        return MeshPipelinePtr{ new MeshPipelineScriptBinding{ name }}.Detach();
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    MeshPipelineScriptBinding::MeshPipelineScriptBinding (const String &name) __Th___ : BasePipelineTmpl{name}
    {
        CHECK_THROW_MSG( ObjectStorage::Instance()->mpipelines.emplace( _name, MeshPipelinePtr{this} ).second,
            "MeshPipeline with name '"s << name << "' is already defined" );
    }

    MeshPipelineScriptBinding::MeshPipelineScriptBinding () :
        MeshPipelineScriptBinding{ "<unknown>" }
    {}

/*
=================================================
    SetShaderIO
=================================================
*/
    void  MeshPipelineScriptBinding::SetShaderIO (EShader output, EShader input, const String &typeName) __Th___
    {
        CHECK_THROW_MSG( AnyBits( EShaderStages::MeshStages, (EShaderStages{0} | output) ));
        CHECK_THROW_MSG( AnyBits( EShaderStages::MeshStages, (EShaderStages{0} | input) ));

        CHECK_THROW_MSG( not _shaderIO[output].second, String{ToString(output)} << " shader input is already defined" );
        CHECK_THROW_MSG( not _shaderIO[input].first, String{ToString(input)} << " shader input is already defined" );

        auto&   struct_types = ObjectStorage::Instance()->structTypes;
        auto    st_it = struct_types.find( typeName );
        CHECK_THROW_MSG( st_it != struct_types.end(),
            "StructureType with name '"s << typeName << "' is not found" );

        if ( st_it->second->Layout() != EStructLayout::InternalIO )
            AE_LOG_SE( "StructureType '"s << typeName << "' should use 'InternalIO' layout" );

        _shaderIO[input].first      = st_it->second;
        _shaderIO[output].second    = st_it->second;
    }

/*
=================================================
    _GetShaderInput
=================================================
*/
    ShaderStructTypePtr  MeshPipelineScriptBinding::_GetShaderInput (EShader shader) const
    {
        auto    io_it = _shaderIO.find( shader );
        return  io_it != _shaderIO.end() ? io_it->second.first : Default;
    }

/*
=================================================
    _GetShaderOutput
=================================================
*/
    ShaderStructTypePtr  MeshPipelineScriptBinding::_GetShaderOutput (EShader shader) const
    {
        auto    io_it = _shaderIO.find( shader );
        return  io_it != _shaderIO.end() ? io_it->second.second : Default;
    }

/*
=================================================
    SetTaskShader
=================================================
*/
    void  MeshPipelineScriptBinding::SetTaskShader (const ScriptShaderPtr &inShader) __Th___
    {
        CHECK_THROW_MSG( inShader );
        CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::MeshTask, Default ));

        inShader->type = EShader::MeshTask;
        CHECK_THROW_MSG( _GetShaderInput( inShader->type ) == Default );

        TEST_FEATURE( GetFeatures(), taskShader );

        _CompileShader( INOUT task, inShader, Default, _GetShaderOutput( inShader->type ));
        _stages |= inShader->type;
    }

/*
=================================================
    SetMeshShader
=================================================
*/
    void  MeshPipelineScriptBinding::SetMeshShader (const ScriptShaderPtr &inShader) __Th___
    {
        CHECK_THROW_MSG( inShader );
        CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Mesh, Default ));
        inShader->type = EShader::Mesh;

        TEST_FEATURE( GetFeatures(), meshShader );

        _CompileShader( INOUT mesh, inShader, _GetShaderInput( inShader->type ), _GetShaderOutput( inShader->type ));
        _stages |= inShader->type;
    }

/*
=================================================
    SetFragmentShader
=================================================
*/
    void  MeshPipelineScriptBinding::SetFragmentShader (const ScriptShaderPtr &inShader) __Th___
    {
        CHECK_THROW_MSG( inShader );
        CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Fragment, Default ));

        inShader->type = EShader::Fragment;
        CHECK_THROW_MSG( _GetShaderOutput( inShader->type ) == Default );

        _CompileShader( INOUT fragment, inShader, _GetShaderInput( inShader->type ), Default, _fragOutput );
        _stages |= inShader->type;
    }

/*
=================================================
    TestRenderPass
=================================================
*/
    void  MeshPipelineScriptBinding::TestRenderPass (const String &compatRP, const String &subpass) __Th___
    {
        SubpassShaderIO     frag_io;
        GetSubpassShaderIO( OUT frag_io );

        ObjectStorage::Instance()->TestRenderPass( compatRP, subpass, frag_io, false, false );  // throw
    }

/*
=================================================
    AddSpecialization
=================================================
*/
    MeshPipelineSpecPtr  MeshPipelineScriptBinding::AddSpecialization2 (const String &name) __Th___
    {
        _Prepare();

        CHECK_THROW_MSG( _isPrepared );
        _OnAddSpecialization();

        return _pplnSpec.emplace_back( MeshPipelineSpecPtr{new MeshPipelineSpecScriptBinding{ this, name }} );
    }

    MeshPipelineSpecScriptBinding*  MeshPipelineScriptBinding::AddSpecialization (const String &name) __Th___
    {
        return AddSpecialization2( name ).Detach();
    }

/*
=================================================
    Build
=================================================
*/
    bool  MeshPipelineScriptBinding::Build ()
    {
        try {
            _Prepare();
        }
        catch (...) {
            return false;
        }

        for (auto& src : _pplnSpec)
        {
            CHECK_ERR( src->Build( *_pipelineUID ));
        }

        return true;
    }

/*
=================================================
    _Prepare
=================================================
*/
    void  MeshPipelineScriptBinding::_Prepare () __Th___
    {
        if ( _pipelineUID.has_value() )
            return;

        auto&   storage = *ObjectStorage::Instance();

        CHECK_THROW_MSG( _AddLayout( task     ));
        CHECK_THROW_MSG( _AddLayout( mesh     ));
        CHECK_THROW_MSG( _AddLayout( fragment ));
        CHECK_THROW_MSG( _BuildLayout() );

        SerializableMeshPipeline    desc;

        desc.layout = _GetLayout();
        CHECK_THROW_MSG( desc.layout != Default );

        CHECK_THROW_MSG( mesh );
        CHECK_THROW_MSG( fragment );

        if ( task )
        {
            desc.taskDefaultLocalSize   = ushort3{ task->reflection.mesh.taskGroupSize };
            desc.taskLocalSizeSpec      = ushort3{ task->reflection.mesh.taskGroupSpec };
            CHECK( desc.shaders.insert_or_assign( EShader::MeshTask, task->uid ).second );
        }

        if ( mesh )
        {
            desc.outputTopology         = mesh->reflection.mesh.topology;
            desc.maxVertices            = mesh->reflection.mesh.maxVertices;
            desc.maxIndices             = mesh->reflection.mesh.maxIndices;
            desc.meshDefaultLocalSize   = ushort3{ mesh->reflection.mesh.meshGroupSize };
            desc.meshLocalSizeSpec      = ushort3{ mesh->reflection.mesh.meshGroupSpec };
            CHECK( desc.shaders.insert_or_assign( EShader::Mesh, mesh->uid ).second );
        }

        if ( fragment )
        {
            desc.earlyFragmentTests = fragment->reflection.fragment.earlyFragmentTests;
            CHECK( desc.shaders.insert_or_assign( EShader::Fragment, fragment->uid ).second );
        }

        desc.features = storage.CopyFeatures( _features );

        auto [uid, is_unique] = storage.pplnStorage->AddPipeline( _name, RVRef(desc) );

        CHECK_MSG( is_unique,
                    "Pipeline name '"s << GetName() << "' is already used and will be overriden" );

        _pipelineUID    = uid;
        _isPrepared     = true;
    }

/*
=================================================
    SetFragmentOutputFromRenderTech
=================================================
*/
    void  MeshPipelineScriptBinding::SetFragmentOutputFromRenderTech (const String &renTechName, const String &passName) __Th___
    {
        CHECK_THROW_MSG( not _fragOutput.has_value() );
        CHECK_THROW_MSG( not AllBits( _states, EStateBits::HasShaders ));

        _fragOutput = FragOutput_t{};
        CHECK_THROW_MSG( BasePipelineTmpl::_FragmentOutputFromRenderTech( renTechName, passName, OUT *_fragOutput ));
    }

/*
=================================================
    SetFragmentOutputFromRenderPass
=================================================
*/
    void  MeshPipelineScriptBinding::SetFragmentOutputFromRenderPass (const String &compatRPassName, const String &subpassName) __Th___
    {
        CHECK_THROW_MSG( not _fragOutput.has_value() );
        CHECK_THROW_MSG( not AllBits( _states, EStateBits::HasShaders ));

        _fragOutput = FragOutput_t{};
        CHECK_THROW_MSG( BasePipelineTmpl::_FragmentOutputFromCompatRenderPass( compatRPassName, subpassName, OUT *_fragOutput ));
    }

/*
=================================================
    GetSubpassShaderIO
=================================================
*/
    void  MeshPipelineScriptBinding::GetSubpassShaderIO (OUT SubpassShaderIO &fragIO) C_Th___
    {
        if ( _fragOutput.has_value() )
        {
            CHECK_THROW_MSG( _FragOutToSubpassShaderIO( *_fragOutput, OUT fragIO ));
            return;
        }

        CHECK_THROW_MSG( fragment );
        fragIO = fragment->reflection.fragment.fragmentIO;
    }

/*
=================================================
    Bind
=================================================
*/
    void  MeshPipelineScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<MeshPipelineScriptBinding>  binder{ se };
        binder.CreateRef();

        binder.Comment( "Create pipeline template.\n"
                        "Name is used in C++ code to create pipeline." );
        binder.AddFactoryCtor( &MeshPipelineScriptBinding_Ctor, {"name"} );

        binder.Comment( "Add macros which will be used in shader.\n"
                        "Format: MACROS = value; DEF" );
        binder.AddMethod( &MeshPipelineScriptBinding::Define,               "Define",           {} );

        binder.Comment( "Set task shader. This shader is optional.\n"
                        "Pipeline will inherit shader feature sets." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetTaskShader,        "SetTaskShader",    {} );

        binder.Comment( "Set mesh shader.\n"
                        "Pipeline will inherit shader feature sets." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetMeshShader,        "SetMeshShader",    {} );

        binder.Comment( "Set fragment shader.\n"
                        "Pipeline will inherit shader feature sets." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetFragmentShader,    "SetFragmentShader", {} );

        binder.Comment( "Create specialization for pipeline template.\n"
                        "Name is used in C++ code to get pipeline from render technique.\n"
                        "Pipeline specialization use the same pipeline layout, same shader binaries, compatible render pass, difference only in some parameters." );
        binder.AddMethod( &MeshPipelineScriptBinding::AddSpecialization,    "AddSpecialization", {"specName"} );

        binder.Comment( "Check is fragment shader compatible with render pass." );
        binder.AddMethod( &MeshPipelineScriptBinding::TestRenderPass,       "TestRenderPass",   {"compatRP", "subpass"} );

        binder.Comment( "Add FeatureSet to the pipeline." );
        binder.AddMethod( &MeshPipelineScriptBinding::AddFeatureSet,        "AddFeatureSet",    {"fsName"} );

        binder.Comment( "Set pipeline layout.\n"
                        "Pipeline will inherit layout feature sets." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetLayout,            "SetLayout",        {"plName"} );
        binder.AddMethod( &MeshPipelineScriptBinding::SetLayout2,           "SetLayout",        {"pl"} );

        binder.Comment( "Set shader interface between stages." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetShaderIO,          "SetShaderIO",      {"output", "input", "typeName"} );

        binder.Comment( "Add fragment shader output variables from render technique graphics pass.\n"
                        "Don't use it with explicit shader output." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetFragmentOutputFromRenderTech,  "SetFragmentOutputFromRenderTech", {"renTechName", "passName"} );

        binder.Comment( "Add fragment shader output variables from render pass.\n"
                        "Don't use it with explicit shader output." );
        binder.AddMethod( &MeshPipelineScriptBinding::SetFragmentOutputFromRenderPass,  "SetFragmentOutputFromRenderPass", {"compatRP", "subpass"} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    MeshPipelineSpecScriptBinding::MeshPipelineSpecScriptBinding (const MeshPipelineScriptBinding* base, const String &name) __Th___ :
        BasePipelineSpec{ base, name }
    {}

/*
=================================================
    SetRenderPass
=================================================
*/
    void  MeshPipelineSpecScriptBinding::SetRenderPass (const RenderPassName &rpName, const SubpassName &subpass) __Th___
    {
        CHECK_THROW_MSG( rpName.IsDefined() and subpass.IsDefined() );

        CHECK_THROW_MSG( GetBase() != null and GetBase()->fragment );
        auto&   storage = *ObjectStorage::Instance();

        auto    iter    = storage.renderPassToCompat.find( rpName );
        CHECK_THROW_MSG( iter != storage.renderPassToCompat.end(),
            "RenderPass '"s << storage.GetName( rpName ) << "' is not exists" );

        if ( desc.renderPass.IsDefined() and desc.subpass.IsDefined() )
        {
            CHECK_THROW_MSG( desc.renderPass == iter->second and desc.subpass == subpass,
                "RenderPass is already defined" );
        }

        SubpassShaderIO     frag_io;
        GetBase()->GetSubpassShaderIO( OUT frag_io );

        storage.TestRenderPass( iter->second, subpass, frag_io, false, false );  // throw

        desc.renderPass = iter->second;
        desc.subpass    = subpass;
    }

/*
=================================================
    SetDynamicState
=================================================
*/
    void  MeshPipelineSpecScriptBinding::SetDynamicState (/*EPipelineDynamicState*/uint states) __Th___
    {
        auto    ds = EPipelineDynamicState(states);
        CHECK_THROW_MSG( (ds & ~EPipelineDynamicState::MeshPipelineMask) == Default,
            "unsupported dynamic state for mesh pipeline" );

        desc.dynamicState = ds;
    }

/*
=================================================
    SetRenderState
=================================================
*/
    void  MeshPipelineSpecScriptBinding::SetRenderState (const RenderState &state) __Th___
    {
        CHECK_THROW_MSG( state.inputAssembly.topology == Default );
        _ValidateRenderState( desc.dynamicState, state, GetFeatures() );

        renderState = state;

        SubpassShaderIO     frag_io;
        GetBase()->GetSubpassShaderIO( OUT frag_io );
        _CheckDepthStencil( renderState, frag_io, desc.renderPass, desc.subpass );  // throw
    }

    void  MeshPipelineSpecScriptBinding::SetRenderState2 (const String &name) __Th___
    {
        const auto& map = ObjectStorage::Instance()->renderStatesMap;

        auto    iter = map.find( name );
        CHECK_THROW_MSG( iter != map.end(),
            "RenderState '"s << name << "' is not exists" );

        return SetRenderState( iter->second->Get() );
    }

/*
=================================================
    SetViewportCount
=================================================
*/
    void  MeshPipelineSpecScriptBinding::SetViewportCount (uint value) __Th___
    {
        CHECK_THROW_MSG( value > 0 );
        CHECK_THROW_MSG( value <= GraphicsConfig::MaxViewports );

        TestFeature_Min( GetBase()->GetFeatures(), &FeatureSet::maxViewports, value, "maxViewports", "viewportCount" );

        desc.viewportCount = CheckCast<ubyte>(value);
    }

/*
=================================================
    SetTaskGroupSize3
=================================================
*/
    void  MeshPipelineSpecScriptBinding::SetTaskGroupSize3 (uint x, uint y, uint z) __Th___
    {
        CHECK_THROW_MSG( GetBase() != null and GetBase()->task, "task shader is not compiled" );

        const uint3 spec        = uint3{ GetBase()->task->reflection.mesh.taskGroupSpec };
        uint        total_size  = Max( 1u, GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxTaskWorkGroupSize ));

        _SetLocalGroupSize( "task localSize ", spec, uint3{total_size}, total_size, uint3{x,y,z}, OUT desc.taskLocalSize );
    }

/*
=================================================
    SetMeshGroupSize3
=================================================
*/
    void  MeshPipelineSpecScriptBinding::SetMeshGroupSize3 (uint x, uint y, uint z) __Th___
    {
        CHECK_THROW_MSG( GetBase() != null and GetBase()->mesh, "mesh shader is not compiled" );

        const uint3 spec        = uint3{ GetBase()->mesh->reflection.mesh.meshGroupSpec };
        uint        total_size  = Max( 1u, GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxMeshWorkGroupSize ));

        _SetLocalGroupSize( "mesh localSize ", spec, uint3{total_size}, total_size, uint3{x,y,z}, OUT desc.meshLocalSize );
    }

/*
=================================================
    Build
=================================================
*/
    bool  MeshPipelineSpecScriptBinding::Build (PipelineTemplUID templUID)
    {
        if ( IsBuilded() )
            return true;

        auto&   ppln_storage    = *ObjectStorage::Instance()->pplnStorage;
        auto    rs_uid          = ppln_storage.AddRenderState( SerializableRenderState{ renderState });
        auto    dss_uid         = ppln_storage.AddDepthStencilState( SerializableDepthStencilState{ renderState });

        if ( GetBase()->task )
        {
            const uint3 task_spec   = uint3{ GetBase()->task->reflection.mesh.taskGroupSpec };
            const bool  req_spec    = Any( task_spec != uint3{~0u} );
            if ( req_spec )
            {
                CHECK_THROW_MSG( (task_spec.x == UMax) == (desc.taskLocalSize.x == UMax), "use 'SetTaskLocalSize()' to set local size" );
                CHECK_THROW_MSG( (task_spec.y == UMax) == (desc.taskLocalSize.y == UMax), "use 'SetTaskLocalSize()' to set local size" );
                CHECK_THROW_MSG( (task_spec.z == UMax) == (desc.taskLocalSize.z == UMax), "use 'SetTaskLocalSize()' to set local size" );
            }
        }

        if ( GetBase()->mesh )
        {
            const uint3 mesh_spec   = uint3{ GetBase()->mesh->reflection.mesh.meshGroupSpec };
            const bool  req_spec    = Any( mesh_spec != uint3{~0u} );
            if ( req_spec )
            {
                CHECK_THROW_MSG( (mesh_spec.x == UMax) == (desc.meshLocalSize.x == UMax), "use 'SetMeshLocalSize()' to set local size" );
                CHECK_THROW_MSG( (mesh_spec.y == UMax) == (desc.meshLocalSize.y == UMax), "use 'SetMeshLocalSize()' to set local size" );
                CHECK_THROW_MSG( (mesh_spec.z == UMax) == (desc.meshLocalSize.z == UMax), "use 'SetMeshLocalSize()' to set local size" );
            }
        }

        SerializableMeshPipelineSpec    spec;
        spec.templUID       = templUID;
        spec.rStateUID      = rs_uid;
        spec.dsStateUID     = dss_uid;
        spec.desc           = desc;
        spec.desc.options   = _options;

        _OnBuild( ppln_storage.AddPipeline( Name(), RVRef(spec) ));
        return true;
    }

/*
=================================================
    Bind
=================================================
*/
    void  MeshPipelineSpecScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<MeshPipelineSpecScriptBinding>  binder{ se };
        binder.CreateRef();

        binder.Comment( "Set specialization value.\n"
                        "Specialization constant must be previously defined in shader by 'Shader::AddSpec()'." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetSpecValueU,        "SetSpecValue",     {"name", "value"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetSpecValueI,        "SetSpecValue",     {"name", "value"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetSpecValueF,        "SetSpecValue",     {"name", "value"} );

        binder.Comment( "Set dynamic states (EPipelineDynamicState).\n"
                        "Supported: StencilCompareMask, StencilWriteMask, StencilReference, DepthBias, BlendConstants', FragmentShadingRate." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetDynamicState,      "SetDynamicState",  {"states"} );

        binder.Comment( "Set render state." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetRenderState,       "SetRenderState",   {"rs"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetRenderState2,      "SetRenderState2",  {"rsName"} );

        binder.Comment( "Set number of viewports. Default is 1.\n"
                        "Requires 'multiViewport' feature." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetViewportCount,     "SetViewportCount", {"count"} );

        binder.Comment( "Set task shader workgroup size. All threads in workgroup can use same (shared) memory and payload.\n"
                        "Shader must use 'SetMeshSpec1/2/3()' to define specialization constant." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetTaskGroupSize1,    "SetTaskLocalSize", {"x"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetTaskGroupSize2,    "SetTaskLocalSize", {"x", "y"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetTaskGroupSize3,    "SetTaskLocalSize", {"x", "y", "z"} );

        binder.Comment( "Set mesh shader workgroup size. All threads in workgroup can use same (shared) memory.\n"
                        "Shader must use 'SetMeshSpec1/2/3()' to define specialization constant." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetMeshGroupSize1,    "SetMeshLocalSize", {"x"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetMeshGroupSize2,    "SetMeshLocalSize", {"x", "y"} );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetMeshGroupSize3,    "SetMeshLocalSize", {"x", "y", "z"});

        binder.Comment( "Attach pipeline to the render technique.\n"
                        "When rtech is created it will create all attached pipelines." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::AddToRenderTech,      "AddToRenderTech",  {"rtech", "gpass"} );

        binder.Comment( "Set pipeline options (EPipelineOpt).\n"
                        "Supported: 'Optimize'.\n"
                        "By default used value from 'GlobalConfig::SetPipelineOptions()'." );
        binder.AddMethod( &MeshPipelineSpecScriptBinding::SetOptions,           "SetOptions",       {"opts"} );
    }


} // AE::PipelineCompiler
