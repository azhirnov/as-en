// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{

    //
    // Metal Tile Pipeline
    //

    class MTilePipeline final
    {
    // types
    public:
        struct CreateInfo
        {
            MPipelinePack const&                                pplnPack;
            PipelineCompiler::SerializableTilePipeline const&   templCI;
            TilePipelineDesc const&                             specCI;
            PipelineLayoutID                                    layoutId;
            MPipelinePack::ShaderModuleRef                      shader;
            PipelineCacheID                                     cacheId;
            MPipelinePack::Allocator_t *                        allocator       = null;
        };


    // variables
    private:
        MetalRenderPipelineRC       _pipeline;

        ushort2                     _localSize;
        ubyte                       _rasterOrderGroup   = UMax;

        Strong<PipelineLayoutID>    _layoutId;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        MTilePipeline ()                                            __NE___ {}
        ~MTilePipeline ()                                           __NE___;

        ND_ bool  Create (MResourceManager &, const CreateInfo &ci) __NE___;
            void  Destroy (MResourceManager &)                      __NE___;

        ND_ MetalRenderPipeline     Handle ()                       C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _pipeline; }
        ND_ PipelineLayoutID        LayoutID ()                     C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
        ND_ uint2                   LocalSize ()                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return uint2{_localSize}; }
        ND_ EPipelineDynamicState   DynamicState ()                 C_NE___ { return Default; }
        ND_ uint                    RasterOrderGroup ()             C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _rasterOrderGroup; }

        DEBUG_ONLY( ND_ StringView  GetDebugName ()                 C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };

} // AE::Graphics

#endif // AE_ENABLE_METAL
