// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Wrapper for ray tracing pipeline (VK_KHR_ray_tracing_pipeline).
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

    //
    // Vulkan Ray Tracing Pipeline
    //

    class VRayTracingPipeline final
    {
    // types
    public:
        struct CreateInfo
        {
            PipelineCompiler::SerializableRayTracingPipeline const& templCI;
            RayTracingPipelineDesc const&                           specCI;
            PipelineLayoutID                                        layoutId;
            ArrayView< VPipelinePack::ShaderModuleRef >             shaders;
            PipelineCacheID                                         cacheId;
            VPipelinePack::Allocator_t *                            allocator       = null;
            VTempLinearAllocator *                                  tempAllocator   = null;
        };

    private:
        template <typename K, typename V>
        using THashMap = FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAllocatorRef< Pair<const K, V>, VPipelinePack::Allocator_t* >>;

        using NameToHandleAlloc_t   = StdAllocatorRef< Pair<const RayTracingGroupName::Optimized_t, uint>, VPipelinePack::Allocator_t* >;
        using NameToHandle_t        = THashMap< RayTracingGroupName::Optimized_t, uint >;   // name to index in '_groupHandles'


    // variables
    private:
        VkPipeline                  _handle             = Default;
        VkPipelineLayout            _layout             = Default;

        EPipelineDynamicState       _dynamicState       = Default;
        EPipelineOpt                _options            = Default;

        Strong<PipelineLayoutID>    _layoutId;
        InPlace<NameToHandle_t>     _nameToHandle;      // allocated by pipeline pack linear allocator
        ArrayView<ulong>            _groupHandles;      // allocated by pipeline pack linear allocator
        ArrayView<ShaderTracePtr>   _dbgTrace;          // allocated by pipeline pack linear allocator

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        VRayTracingPipeline ()                                          __NE___ {}
        ~VRayTracingPipeline ()                                         __NE___;

        ND_ bool  Create (VResourceManager &, const CreateInfo &ci)     __NE___;
            void  Destroy (VResourceManager &)                          __NE___;

        ND_ bool  ParseShaderTrace (const void *ptr, Bytes maxSize, ShaderDebugger::ELogFormat, OUT Array<String> &result)  C_NE___;

        ND_ Bytes  GetShaderGroupStackSize (const VDevice &, const RayTracingGroupName &group)  C_NE___;

            bool  CopyHandle (const VDevice &, const RayTracingGroupName &name, OUT void* dst, Bytes dstSize)   C_NE___;
            bool  CopyHandle (const VDevice &, uint index, OUT void* dst, Bytes dstSize)                        C_NE___;

        ND_ VkPipeline              Handle ()               C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _handle; }
        ND_ VkPipelineLayout        Layout ()               C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layout; }
        ND_ VkPipelineBindPoint     BindPoint ()            C_NE___ { return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR; }
        ND_ PipelineLayoutID        LayoutID ()             C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
        ND_ EPipelineDynamicState   DynamicState ()         C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()        C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
