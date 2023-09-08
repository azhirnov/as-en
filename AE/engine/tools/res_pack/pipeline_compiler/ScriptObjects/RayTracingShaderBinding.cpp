// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/RayTracingShaderBinding.h"
#include "ScriptObjects/Common.inl.h"

AE_DECL_SCRIPT_OBJ( AE::PipelineCompiler::MissIndex,        "MissIndex"     );
AE_DECL_SCRIPT_OBJ( AE::PipelineCompiler::InstanceIndex,    "InstanceIndex" );
AE_DECL_SCRIPT_OBJ( AE::PipelineCompiler::RayIndex,         "RayIndex"      );
AE_DECL_SCRIPT_OBJ( AE::PipelineCompiler::CallableIndex,    "CallableIndex" );


namespace AE::PipelineCompiler
{
namespace
{
    static RayTracingShaderBinding*  RayTracingShaderBinding_Ctor (const RayTracingPipelineSpecPtr &ptr, const String &name) {
        return RayTracingShaderBindingPtr{ new RayTracingShaderBinding{ ptr, name }}.Detach();
    }

    static void  MissIndex_Ctor (OUT void* mem, uint value) {
        PlacementNew<MissIndex>( OUT mem, value );
    }

    static void  InstanceIndex_Ctor (OUT void* mem, uint value) {
        PlacementNew<InstanceIndex>( OUT mem, value );
    }

    static void  RayIndex_Ctor (OUT void* mem, uint value) {
        PlacementNew<RayIndex>( OUT mem, value );
    }

    static void  CallableIndex_Ctor (OUT void* mem, uint value) {
        PlacementNew<CallableIndex>( OUT mem, value );
    }

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    RayTracingShaderBinding::RayTracingShaderBinding () :
        RayTracingShaderBinding{ Default, "<unknown>" }
    {}

    RayTracingShaderBinding::RayTracingShaderBinding (const RayTracingPipelineSpecPtr &ptr, const String &name) __Th___ :
        _spec{ptr},
        _name{name}
    {
        CHECK_THROW_MSG( _spec );
        CHECK_THROW_MSG( ObjectStorage::Instance()->rtShaderBindings.emplace( _name, RayTracingShaderBindingPtr{this} ).second,
            "RayTracingShaderBinding with name '"s << name << "' is already defined" );

        ObjectStorage::Instance()->AddName<RTShaderBindingName>( _name );

        for (auto& rtech : _spec->GetRTechs()) {
            rtech->AddSBT( RayTracingShaderBindingPtr{this} );
        }
    }

/*
=================================================
    Bind
=================================================
*/
    void  RayTracingShaderBinding::Bind (const ScriptEnginePtr &se) __Th___
    {
        {
            ClassBinder<MissIndex>  binder{ se };
            binder.CreateClassValue();
            binder.AddConstructor( &MissIndex_Ctor, {} );
        }{
            ClassBinder<InstanceIndex>  binder{ se };
            binder.CreateClassValue();
            binder.AddConstructor( &InstanceIndex_Ctor, {} );
        }{
            ClassBinder<RayIndex>   binder{ se };
            binder.CreateClassValue();
            binder.AddConstructor( &RayIndex_Ctor, {} );
        }{
            ClassBinder<CallableIndex>  binder{ se };
            binder.CreateClassValue();
            binder.AddConstructor( &CallableIndex_Ctor, {} );
        }{
            ClassBinder<RayTracingShaderBinding>    binder{ se };
            binder.CreateRef();

            binder.Comment( "Create ray tracing shader binding for ray tracing pipeline.\n"
                            "Name is used in C++ code to get ray tracing shader binding." );
            binder.AddFactoryCtor( &RayTracingShaderBinding_Ctor, {"ppln", "sbtName"} );

            binder.Comment( "Set number of ray types.\n"
                            "It will be used to calculate offsets in table:\n"
                            "\t'hitShaders [InstanceCount] [RayTypeCount]'"
                            "Where 'ray type' is primary, shadow, reflection and other. All types are user-defined." );
            binder.AddMethod( &RayTracingShaderBinding::HitGroupStride, "HitGroupStride",   {"stride"} );

            binder.Comment( "Bind shader group from ray tracing pipeline as a ray generation shader." );
            binder.AddMethod( &RayTracingShaderBinding::BindRayGen,     "BindRayGen",       {"groupName"} );

            binder.Comment( "Bind shader group from ray tracing pipeline as a miss shader.\n"
                            "'missIndex' should be < HitGroupStride." );
            binder.AddMethod( &RayTracingShaderBinding::BindMiss,       "BindMiss",         {"groupName", "missIndex"} );

            binder.Comment( "Bind shader group from ray tracing pipeline as a hit group.\n"
                            "'rayIndex' must be < HitGroupStride." );
            binder.AddMethod( &RayTracingShaderBinding::BindHitGroup,   "BindHitGroup",     {"groupName", "instanceIndex", "rayIndex"} );

            binder.Comment( "Bind shader group from ray tracing pipeline as callable shader." );
            binder.AddMethod( &RayTracingShaderBinding::BindCallable,   "BindCallable",     {"groupName", "callableIndex"} );
        }
    }

/*
=================================================
    Build
=================================================
*/
    bool  RayTracingShaderBinding::Build ()
    {
        if ( _uid.has_value() )
            return true;

        using BindingInfo       = SerializableRTShaderBindingTable::BindingInfo;
        using BindingTable_t    = SerializableRTShaderBindingTable::BindingTable_t;

        // validate
        CHECK_ERR( _rayGen != Default );

        auto&   storage = *ObjectStorage::Instance();

        SerializableRTShaderBindingTable    desc;

        desc.pplnName       = PipelineName::Optimized_t{ _spec->Name() };
        desc.hitGroupStride = _hitGroupStride;

        // ray gen
        {
            desc.raygen.index   = uint(_rayGen);
        }

        // miss shader
        if ( not _missShaders.empty() )
        {
            auto*   ptr = storage.allocator.Allocate<BindingInfo>( _missShaders.size() );
            CHECK_ERR( ptr != null );
            desc.miss = BindingTable_t{ ptr, _missShaders.size() };

            for (usize i = 0; i < _missShaders.size(); ++i) {
                ptr[i].index = uint(_missShaders[i]);
            }
        }

        // hit group
        if ( not _hitGroups.empty() )
        {
            usize   max_size = _hitGroupStride;

            for (auto& arr : _hitGroups) {
                max_size = Max( max_size, arr.size() );
            }
            CHECK_ERR( max_size == _hitGroupStride );

            for (auto& arr : _hitGroups) {
                arr.resize( _hitGroupStride, Default );
            }

            auto*   ptr = storage.allocator.Allocate<BindingInfo>( _hitGroups.size() * _hitGroupStride );
            CHECK_ERR( ptr != null );
            desc.hit = BindingTable_t{ ptr, _hitGroups.size() * _hitGroupStride };

            usize   k = 0;
            for (usize i = 0; i < _hitGroups.size(); ++i)
            {
                auto&   per_instance = _hitGroups[i];
                for (usize j = 0; j < per_instance.size(); ++j)
                {
                    ptr[k++].index = uint(per_instance[j]);
                }
            }
        }

        // callable
        if ( not _callable.empty() )
        {
            auto*   ptr = storage.allocator.Allocate<BindingInfo>( _callable.size() );
            CHECK_ERR( ptr != null );
            desc.callable = BindingTable_t{ ptr, _callable.size() };

            for (usize i = 0; i < _callable.size(); ++i) {
                ptr[i].index = uint(_callable[i]);
            }
        }

        //Print();

        _uid    = storage.pplnStorage->AddSBT( RVRef(desc) );
        _spec   = Default;

        return true;
    }

/*
=================================================
    Print
=================================================
*/
    void  RayTracingShaderBinding::Print () C_Th___
    {
        String  str;
        str << "RayGen: " << _spec->GetBase()->GetRayGenShaderName( uint(_rayGen) ) << "\n\n";

        if ( not _missShaders.empty() )
        {
            str << "Miss:\n";

            for (usize i = 0; i < _missShaders.size(); ++i)
            {
                str << "  [" << ToString(i) << "] - " << _spec->GetBase()->GetMissShaderName( uint(_missShaders[i]) )
                    << " - (" << ToString(uint(_missShaders[i])) << ")\n";
            }
            str << "\n";
        }

        if ( not _hitGroups.empty() )
        {
            str << "HitGroupStride: " << ToString(_hitGroupStride) << "\n"
                << "HitGroups [ray][instance]:\n";

            for (usize i = 0; i < _hitGroups.size(); ++i)
            {
                auto&   per_instance = _hitGroups[i];
                for (usize j = 0; j < per_instance.size(); ++j)
                {
                    str << "  [" << ToString(j) << "][" << ToString(i) << "] - " << _spec->GetBase()->GetHitGroupName( uint(per_instance[j]) )
                        << " - (" << ToString(uint(per_instance[j])) << ")\n";
                }
            }
            str << "\n";
        }

        if ( not _callable.empty() )
        {
            str << "Callable:\n";

            for (usize i = 0; i < _callable.size(); ++i)
            {
                str << "  [" << ToString(i) << "] - " << _spec->GetBase()->GetCallableShaderName( uint(_callable[i]) )
                    << " - (" << ToString(uint(_callable[i])) << ")\n";
            }
            str << "\n";
        }

        AE_LOGI( str );
    }

/*
=================================================
    HitGroupStride
=================================================
*/
    void  RayTracingShaderBinding::HitGroupStride (uint value) __Th___
    {
        CHECK_THROW_MSG( value > 0 );

        _hitGroupStride = value;
    }

/*
=================================================
    BindRayGen
=================================================
*/
    void  RayTracingShaderBinding::BindRayGen (const String &groupName) __Th___
    {
        const uint  idx = _spec->GetBase()->GetRayGenShader( groupName );
        CHECK_THROW_MSG( idx != UMax,
            "RayGenShader '"s << groupName << "' is not exists in '" << _spec->GetBase()->GetName() << "'" );

        _rayGen = GroupIndex(idx);
    }

/*
=================================================
    BindMiss
=================================================
*/
    void  RayTracingShaderBinding::BindMiss (const String &groupName, const MissIndex &missIndex) __Th___
    {
        const uint  idx = _spec->GetBase()->GetMissShader( groupName );
        CHECK_THROW_MSG( idx != UMax,
            "MissShader '"s << groupName << "' is not exists in '" << _spec->GetBase()->GetName() << "'" );

        _missShaders.resize( Max( _missShaders.size(), missIndex.value+1 ), Default );

        _missShaders[ missIndex.value ] = GroupIndex(idx);
    }

/*
=================================================
    BindHitGroup
=================================================
*/
    void  RayTracingShaderBinding::BindHitGroup (const String &groupName, const InstanceIndex &instanceIndex, const RayIndex &rayIdx) __Th___
    {
        const uint  idx = _spec->GetBase()->GetHitGroup( groupName );
        CHECK_THROW_MSG( idx != UMax,
            "HitGroup '"s << groupName << "' is not exists in '" << _spec->GetBase()->GetName() << "'" );

        CHECK_THROW_MSG( rayIdx.value < _hitGroupStride );

        _hitGroups.resize( Max( _hitGroups.size(), instanceIndex.value+1 ), Default );
        auto&   groups = _hitGroups[ instanceIndex.value ];

        groups.resize( Max( groups.size(), rayIdx.value+1 ), Default );
        groups[ rayIdx.value ] = GroupIndex(idx);
    }

/*
=================================================
    BindCallable
=================================================
*/
    void  RayTracingShaderBinding::BindCallable (const String &groupName, const CallableIndex &callableIdx) __Th___
    {
        const uint  idx = _spec->GetBase()->GetCallableShader( groupName );
        CHECK_THROW_MSG( idx != UMax,
            "CallableShader '"s << groupName << "' is not exists in '" << _spec->GetBase()->GetName() << "'" );

        _callable.resize( Max( _callable.size(), callableIdx.value+1 ), Default );

        _callable[ callableIdx.value ] = GroupIndex(idx);
    }


} // AE::PipelineCompiler
