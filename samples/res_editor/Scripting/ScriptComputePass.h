// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBasePass.h"
#include "res_editor/Passes/ComputePass.h"

namespace AE::ResEditor
{

    //
    // Compute Pass
    //

    class ScriptComputePass final : public ScriptBasePass
    {
    // types
    private:
        using ResourceUnion_t = Union< NullUnion, ScriptBufferPtr, ScriptImagePtr, ScriptVideoImagePtr, ScriptRTScenePtr >;

        struct Argument
        {
            String              name;
            ResourceUnion_t     res;
            EResourceState      state           = Default;
            String              samplerName;
        };
        using Arguments_t   = Array< Argument >;

        using Iteration     = ComputePass::Iteration;
        using Iterations_t  = ComputePass::Iterations_t;


    // variables
    public:
        const Path              _pplnPath;
        String                  _defines;

        uint3                   _localSize  {0};

        Iterations_t            _iterations;
        Arguments_t             _args;
        FlatHashSet< String >   _uniqueNames;


    // methods
    public:
        ScriptComputePass () : ScriptBasePass{EFlags::Unknown} {}
        ScriptComputePass (const String &name, const String &defines, EFlags baseFlags)                 __Th___;

        void  LocalSize1  (uint x)                                                                      __Th___ { return LocalSize3v({ x, 1u, 1u }); }
        void  LocalSize2  (uint x, uint y)                                                              __Th___ { return LocalSize3v({ x, y, 1u }); }
        void  LocalSize3  (uint x, uint y, uint z)                                                      __Th___ { return LocalSize3v({ x, y, z }); }
        void  LocalSize2v (const packed_uint2 &v)                                                       __Th___ { return LocalSize3v({ v, 1u }); }
        void  LocalSize3v (const packed_uint3 &v)                                                       __Th___;

        void  DispatchGroups1  (uint groupCountX)                                                       __Th___ { return DispatchGroups3v({ groupCountX, 1u, 1u }); }
        void  DispatchGroups2  (uint groupCountX, uint groupCountY)                                     __Th___ { return DispatchGroups3v({ groupCountX, groupCountY, 1u }); }
        void  DispatchGroups3  (uint groupCountX, uint groupCountY, uint groupCountZ)                   __Th___ { return DispatchGroups3v({ groupCountX, groupCountY, groupCountZ }); }
        void  DispatchGroups2v (const packed_uint2 &groupCount)                                         __Th___ { return DispatchGroups3v({ groupCount, 1u }); }
        void  DispatchGroups3v (const packed_uint3 &groupCount)                                         __Th___;
        void  DispatchGroupsDS (const ScriptDynamicDimPtr &ds)                                          __Th___;
        void  DispatchGroups1D (const ScriptDynamicUIntPtr &dyn)                                        __Th___;

        void  DispatchThreads1  (uint threadsX)                                                         __Th___ { return DispatchThreads3v({ threadsX, 1u, 1u }); }
        void  DispatchThreads2  (uint threadsX, uint threadsY)                                          __Th___ { return DispatchThreads3v({ threadsX, threadsY, 1u }); }
        void  DispatchThreads3  (uint threadsX, uint threadsY, uint threadsZ)                           __Th___ { return DispatchThreads3v({ threadsX, threadsY, threadsZ }); }
        void  DispatchThreads2v (const packed_uint2 &threads)                                           __Th___ { return DispatchThreads3v({ threads, 1u }); }
        void  DispatchThreads3v (const packed_uint3 &threads)                                           __Th___;
        void  DispatchThreadsDS (const ScriptDynamicDimPtr &ds)                                         __Th___;
        void  DispatchThreads1D (const ScriptDynamicUIntPtr &dyn)                                       __Th___;

        void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)                            __Th___;

        void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)                              __Th___;
        void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)                             __Th___;
        void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)                           __Th___;

        void  ArgImageIn (const String &name, const ScriptImagePtr &img)                                __Th___;
        void  ArgImageOut (const String &name, const ScriptImagePtr &img)                               __Th___;
        void  ArgImageInOut (const String &name, const ScriptImagePtr &img)                             __Th___;

        void  ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName)   __Th___;
        void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)__Th___;
        void  ArgController (const ScriptBaseControllerPtr &)                                           __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;
        static void  GetShaderTypes (INOUT CppStructsFromShaders &)                                     __Th___;

    // ScriptBasePass //
        RC<IPass>  ToPass ()                                                                            C_Th_OV;

    private:
        void  _AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage)            __Th___;
        void  _AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage)             __Th___;

        ND_ auto  _CompilePipeline (OUT Bytes &ubSize)                                                  C_Th___;
            void  _CompilePipeline2 (OUT Bytes &ubSize)                                                 C_Th___;
            void  _CompilePipeline3 (const String &cs, uint line, const String &pplnName,
                                     uint shaderOpts, EPipelineOpt pplnOpt)                             C_Th___;

        ND_ static auto  _CreateUBType ()                                                               __Th___;
    };


} // AE::ResEditor
