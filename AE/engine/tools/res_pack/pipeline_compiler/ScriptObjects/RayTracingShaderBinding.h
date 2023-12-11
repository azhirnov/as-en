// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/RayTracingPipeline.h"

namespace AE::PipelineCompiler
{
    struct InstanceIndex
    {
        uint        value   = 0;

        InstanceIndex ()                __NE___ {}
        explicit InstanceIndex (uint v) __NE___ : value{v} {}
    };

    struct RayIndex
    {
        uint        value   = 0;

        RayIndex ()                     __NE___ {}
        explicit RayIndex (uint v)      __NE___ : value{v} {}
    };

    struct CallableIndex
    {
        uint        value   = 0;

        CallableIndex ()                __NE___ {}
        explicit CallableIndex (uint v) __NE___ : value{v} {}
    };



    //
    // Ray Tracing Shader Binding
    //
    struct RayTracingShaderBinding final : EnableScriptRC
    {
    // types
    private:
        enum class GroupIndex : uint { Unknown = ~0u };

    // variables
    private:
        uint                            _maxRayTypes    = 0;

        GroupIndex                      _rayGen         = Default;
        Array<GroupIndex>               _missShaders;
        Array< Array<GroupIndex> >      _hitGroups;     // [instances] [ray types]
        Array<GroupIndex>               _callable;

        RayTracingPipelineSpecPtr       _spec;

        Optional<RTShaderBindingUID>    _uid;
        const String                    _name;


    // methods
    public:
        RayTracingShaderBinding ();
        explicit RayTracingShaderBinding (const RayTracingPipelineSpecPtr &ptr, const String &name)         __Th___;

        void  MaxRayTypes (uint value)                                                                      __Th___;
        void  BindRayGen (const String &groupName)                                                          __Th___;
        void  BindMiss (const String &groupName, const RayIndex &missIndex)                                 __Th___;
        void  BindHitGroup (const String &groupName, const InstanceIndex &instIndex, const RayIndex &rayIdx)__Th___;
        void  BindCallable (const String &groupName, const CallableIndex &callableIdx)                      __Th___;

        // TODO: set recursion depth for each shader and then use it for stack size calculation

        void  Print ()                                                                                      C_Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                       __Th___;

        ND_ bool  Build ();

        ND_ StringView          Name ()                                                                     const   { return _name; }
        ND_ RTShaderBindingUID  UID ()                                                                      const   { return _uid.value_or( Default ); }
        ND_ uint                GetMaxRayTypes ()                                                           const   { return _maxRayTypes; }
    };


} // AE::PipelineCompiler
