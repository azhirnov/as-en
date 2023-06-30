// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{

    //
    // Metal Compute Pipeline
    //

    class MComputePipeline final
    {
    // types
    public:
        struct CreateInfo
        {
            PipelineCompiler::SerializableComputePipeline const&    templCI;
            ComputePipelineDesc const&                              specCI;
            PipelineLayoutID                                        layoutId;
            MPipelinePack::ShaderModuleRef                          shader;
            PipelineCacheID                                         cacheId;
            MPipelinePack::Allocator_t *                            allocator       = null;
        };


    // variables
    private:
        MetalComputePipelineRC      _pipeline;

        ushort3                     _localSize;

        Strong<PipelineLayoutID>    _layoutId;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        MComputePipeline ()                                             __NE___ {}
        ~MComputePipeline ()                                            __NE___;

        ND_ bool  Create (MResourceManager &, const CreateInfo &ci)     __NE___;
            void  Destroy (MResourceManager &)                          __NE___;

        ND_ usize   MaxTotalThreadsPerThreadgroup ()                    C_NE___;
        ND_ usize   ThreadExecutionWidth ()                             C_NE___;
        ND_ Bytes   StaticThreadgroupMemoryLength ()                    C_NE___;
        ND_ Bytes   ImageblockMemoryLength (const uint2 &imageBlockDim) C_NE___;

        ND_ MetalComputePipeline    Handle ()                           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _pipeline; }
        ND_ PipelineLayoutID        LayoutID ()                         C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
        ND_ uint3                   LocalSize ()                        C_NE___ { DRC_SHAREDLOCK( _drCheck );  return uint3{_localSize}; }
        ND_ EPipelineDynamicState   DynamicState ()                     C_NE___ { return Default; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };

} // AE::Graphics

#endif // AE_ENABLE_METAL
