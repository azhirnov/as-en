// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Wrapper for tile pipeline (VK_HUAWEI_subpass_shading).
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

    //
    // Vulkan Tile Pipeline
    //

    class VTilePipeline final
    {
    // types
    public:
        struct CreateInfo
        {
            VPipelinePack const&                                pplnPack;
            PipelineCompiler::SerializableTilePipeline const&   templCI;
            TilePipelineDesc const&                             specCI;
            PipelineLayoutID                                    layoutId;
            VPipelinePack::ShaderModuleRef                      shader;
            PipelineCacheID                                     cacheId;
        };


    // variables
    private:
        VkPipeline                  _handle             = Default;
        VkPipelineLayout            _layout             = Default;

        ushort2                     _localSize;
        EPipelineOpt                _options            = Default;
        ubyte                       _subpassIndex       = UMax;

        Strong<PipelineLayoutID>    _layoutId;

        ShaderTracePtr              _dbgTrace;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        VTilePipeline ()                                            __NE___ {}
        ~VTilePipeline ()                                           __NE___;

        ND_ bool  Create (VResourceManager &, const CreateInfo &ci) __NE___;
            void  Destroy (VResourceManager &)                      __NE___;

        ND_ bool  ParseShaderTrace (const void *ptr, Bytes maxSize, ShaderDebugger::ELogFormat, OUT Array<String> &result) C_NE___;

        ND_ VkPipeline              Handle ()                       C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _handle; }
        ND_ VkPipelineLayout        Layout ()                       C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layout; }
        ND_ VkPipelineBindPoint     BindPoint ()                    C_NE___ { return VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI; }
        ND_ PipelineLayoutID        LayoutID ()                     C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
        ND_ uint2                   LocalSize ()                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return uint2{_localSize}; }
        ND_ EPipelineDynamicState   DynamicState ()                 C_NE___ { return Default; }
        ND_ uint                    RenderPassSubpassIndex ()       C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
