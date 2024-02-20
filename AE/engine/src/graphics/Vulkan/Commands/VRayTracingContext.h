// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    RayTracingCtx --> DirectRayTracingCtx   --> BarrierMngr --> Vulkan device
                  \-> IndirectRayTracingCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

    //
    // Vulkan Direct Ray Tracing Context implementation
    //

    class _VDirectRayTracingCtx : public VBaseDirectContext
    {
    // types
    private:
        using Validator_t   = RayTracingContextValidation;


    // variables
    protected:
        // cached states
        struct {
            VkPipelineLayout    pplnLayout  = Default;
        }                   _states;


    // methods
    public:
        void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

        void  TraceRays (const uint3 &dim,
                         const VkStridedDeviceAddressRegionKHR &raygen,
                         const VkStridedDeviceAddressRegionKHR &miss,
                         const VkStridedDeviceAddressRegionKHR &hit,
                         const VkStridedDeviceAddressRegionKHR &callable)                                           __Th___;

        ND_ VkCommandBuffer EndCommandBuffer ()                                                                     __Th___;
        ND_ VCommandBuffer  ReleaseCommandBuffer ()                                                                 __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VDirectRayTracingCtx (const RenderTask &task, VCommandBuffer cmdbuf, DebugLabel dbg)                       __Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::RayTracing } {}

        void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout);
        void  _PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages);
        void  _SetStackSize (Bytes size);

        void  _TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress);
        void  _TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR &raygen,
                                  const VkStridedDeviceAddressRegionKHR &miss,
                                  const VkStridedDeviceAddressRegionKHR &hit,
                                  const VkStridedDeviceAddressRegionKHR &callable,
                                  VkDeviceAddress                       indirectDeviceAddress);
    };



    //
    // Vulkan Indirect Ray Tracing Context implementation
    //

    class _VIndirectRayTracingCtx : public VBaseIndirectContext
    {
    // types
    private:
        using Validator_t   = RayTracingContextValidation;


    // variables
    protected:
        // cached states
        struct {
            VkPipelineLayout    pplnLayout  = Default;
        }                   _states;


    // methods
    public:
        void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

        void  TraceRays (const uint3 &dim,
                         const VkStridedDeviceAddressRegionKHR &raygen,
                         const VkStridedDeviceAddressRegionKHR &miss,
                         const VkStridedDeviceAddressRegionKHR &hit,
                         const VkStridedDeviceAddressRegionKHR &callable)                                           __Th___;

        ND_ VBakedCommands      EndCommandBuffer ()                                                                 __Th___;
        ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()                                                             __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VIndirectRayTracingCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)                 __Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::RayTracing } {}

        void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout);
        void  _PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages);
        void  _SetStackSize (Bytes size);

        void  _TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress);
        void  _TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR &raygen,
                                  const VkStridedDeviceAddressRegionKHR &miss,
                                  const VkStridedDeviceAddressRegionKHR &hit,
                                  const VkStridedDeviceAddressRegionKHR &callable,
                                  VkDeviceAddress                       indirectDeviceAddress);
    };



    //
    // Vulkan Ray Tracing Context implementation
    //

    template <typename CtxImpl>
    class _VRayTracingContextImpl : public CtxImpl, public IRayTracingContext
    {
    // types
    public:
        using CmdBuf_t      = typename CtxImpl::CmdBuf_t;
    private:
        using RawCtx        = CtxImpl;
        using AccumBar      = VAccumBarriers< _VRayTracingContextImpl< CtxImpl >>;
        using DeferredBar   = VAccumDeferredBarriersForCtx< _VRayTracingContextImpl< CtxImpl >>;
        using Validator_t   = RayTracingContextValidation;


    // methods
    public:
        explicit _VRayTracingContextImpl (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)      __Th___;

        _VRayTracingContextImpl ()                                                                                          = delete;
        _VRayTracingContextImpl (const _VRayTracingContextImpl &)                                                           = delete;

        using RawCtx::BindDescriptorSet;

        void  BindPipeline (RayTracingPipelineID ppln)                                                                      __Th_OV;
        void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)        __Th_OV;

        void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)   __Th_OV;
        using IRayTracingContext::PushConstant;

        void  SetStackSize (Bytes size)                                                                                     __Th_OV { RawCtx::_SetStackSize( size ); }

        void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)                                                  __Th_OV;
        void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)                                                  __Th_OV;

        void  TraceRays (const uint2 dim, RTShaderBindingID sbt)                                                            __Th_OV;
        void  TraceRays (const uint3 dim, RTShaderBindingID sbt)                                                            __Th_OV;

        void  TraceRaysIndirect (const RTShaderBindingTable &sbt, DeviceAddress address)                                    __Th___;
        void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes)                           __Th_OV;
        void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes)                                     __Th_OV;

        void  TraceRaysIndirect2 (DeviceAddress address)                                                                    __Th___;
        void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)                                      __Th_OV;

        VBARRIERMNGR_INHERIT_BARRIERS
    };

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
    using VDirectRayTracingContext      = Graphics::_hidden_::_VRayTracingContextImpl< Graphics::_hidden_::_VDirectRayTracingCtx >;
    using VIndirectRayTracingContext    = Graphics::_hidden_::_VRayTracingContextImpl< Graphics::_hidden_::_VIndirectRayTracingCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{
/*
=================================================
    constructor
=================================================
*/
    template <typename C>
    _VRayTracingContextImpl<C>::_VRayTracingContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
        RawCtx{ task, RVRef(cmdbuf), dbg }
    {
        Validator_t::CtxInit( task.GetQueueMask() );
    }

/*
=================================================
    BindPipeline
=================================================
*/
    template <typename C>
    void  _VRayTracingContextImpl<C>::BindPipeline (RayTracingPipelineID ppln) __Th___
    {
        auto&   rt_ppln = _GetResourcesOrThrow( ppln );

        RawCtx::_BindPipeline( rt_ppln.Handle(), rt_ppln.Layout() );
    }

/*
=================================================
    BindDescriptorSet
=================================================
*/
    template <typename C>
    void  _VRayTracingContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
    {
        auto&   desc_set = _GetResourcesOrThrow( ds );

        RawCtx::BindDescriptorSet( index, desc_set.Handle(), dynamicOffsets );
    }

/*
=================================================
    PushConstant
=================================================
*/
    template <typename C>
    void  _VRayTracingContextImpl<C>::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
    {
        VALIDATE_GCTX( PushConstant( idx, size, typeName ));
        Unused( typeName );
        RawCtx::_PushConstant( idx.offset, size, values, EShaderStages(0) | idx.stage );
    }

/*
=================================================
    TraceRays
=================================================
*/
    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, const RTShaderBindingTable &sbt) __Th___
    {
        return TraceRays( uint3{ dim.x, dim.y, 1u }, sbt );
    }

    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, const RTShaderBindingTable &sbt) __Th___
    {
        RawCtx::TraceRays( dim, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
    }

    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, RTShaderBindingID sbtId) __Th___
    {
        auto&   sbt = _GetResourcesOrThrow( sbtId ).GetSBT();
        RawCtx::TraceRays( uint3{ dim.x, dim.y, 1u }, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
    }

    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, RTShaderBindingID sbtId) __Th___
    {
        auto&   sbt = _GetResourcesOrThrow( sbtId ).GetSBT();
        RawCtx::TraceRays( dim, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
    }

/*
=================================================
    TraceRaysIndirect
=================================================
*/
    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, DeviceAddress address) __Th___
    {
        RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable, address );
    }

    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        auto&   buf = _GetResourcesOrThrow( indirectBuffer );
        VALIDATE_GCTX( TraceRaysIndirect( buf.Description(), indirectBufferOffset ));

        RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable,
                                    BitCast<VkDeviceAddress>( buf.GetDeviceAddress() + indirectBufferOffset ));
    }

    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRaysIndirect (RTShaderBindingID sbtId, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        auto    [buf, sbt_obj]  = _GetResourcesOrThrow( indirectBuffer, sbtId );
        auto&   sbt             = sbt_obj.GetSBT();
        VALIDATE_GCTX( TraceRaysIndirect( buf.Description(), indirectBufferOffset ));

        RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable,
                                    BitCast<VkDeviceAddress>( buf.GetDeviceAddress() + indirectBufferOffset ));
    }

/*
=================================================
    TraceRaysIndirect2
=================================================
*/
    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRaysIndirect2 (DeviceAddress address) __Th___
    {
        RawCtx::_TraceRaysIndirect2( address );
    }

    template <typename C>
    void  _VRayTracingContextImpl<C>::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        auto&   buf = _GetResourcesOrThrow( indirectBuffer );
        VALIDATE_GCTX( TraceRaysIndirect2( buf.Description(), indirectBufferOffset ));

        RawCtx::_TraceRaysIndirect2( BitCast<VkDeviceAddress>( buf.GetDeviceAddress() + indirectBufferOffset ));
    }
//-----------------------------------------------------------------------------



/*
=================================================
    TraceRays
=================================================
*/
    inline void  _VDirectRayTracingCtx::TraceRays (const uint3 &dim,
                                                    const VkStridedDeviceAddressRegionKHR &raygen,
                                                    const VkStridedDeviceAddressRegionKHR &miss,
                                                    const VkStridedDeviceAddressRegionKHR &hit,
                                                    const VkStridedDeviceAddressRegionKHR &callable) __Th___
    {
        VALIDATE_GCTX( TraceRays( _states.pplnLayout, dim ));

        vkCmdTraceRaysKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, dim.x, dim.y, dim.z );
    }

/*
=================================================
    _TraceRaysIndirect
=================================================
*/
    inline void  _VDirectRayTracingCtx::_TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR&  raygen,
                                                            const VkStridedDeviceAddressRegionKHR&  miss,
                                                            const VkStridedDeviceAddressRegionKHR&  hit,
                                                            const VkStridedDeviceAddressRegionKHR&  callable,
                                                            VkDeviceAddress                         indirectDeviceAddress) __Th___
    {
        VALIDATE_GCTX( TraceRaysIndirect( _states.pplnLayout, indirectDeviceAddress ));

        vkCmdTraceRaysIndirectKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, VkDeviceAddress(indirectDeviceAddress) );
    }

/*
=================================================
    _TraceRaysIndirect2
=================================================
*/
    inline void  _VDirectRayTracingCtx::_TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress) __Th___
    {
        VALIDATE_GCTX( TraceRaysIndirect2( _states.pplnLayout, indirectDeviceAddress ));

        vkCmdTraceRaysIndirect2KHR( _cmdbuf.Get(), indirectDeviceAddress );
    }

/*
=================================================
    _BindPipeline
=================================================
*/
    inline void  _VDirectRayTracingCtx::_BindPipeline (VkPipeline ppln, VkPipelineLayout layout) __Th___
    {
        _states.pplnLayout = layout;
        vkCmdBindPipeline( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, ppln );
    }

/*
=================================================
    BindDescriptorSet
=================================================
*/
    inline void  _VDirectRayTracingCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
    {
        VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

        vkCmdBindDescriptorSets( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
    }

/*
=================================================
    _PushConstant
=================================================
*/
    inline void  _VDirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
    {
        VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

        vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
    }

/*
=================================================
    _SetStackSize
=================================================
*/
    inline void  _VDirectRayTracingCtx::_SetStackSize (Bytes size) __Th___
    {
        GCTX_CHECK( _states.pplnLayout != Default );

        vkCmdSetRayTracingPipelineStackSizeKHR( _cmdbuf.Get(), uint(size) );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _BindPipeline
=================================================
*/
    inline void  _VIndirectRayTracingCtx::_BindPipeline (VkPipeline ppln, VkPipelineLayout layout) __Th___
    {
        _states.pplnLayout = layout;
        _cmdbuf->BindPipeline( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, ppln, layout );
    }

/*
=================================================
    BindDescriptorSet
=================================================
*/
    inline void  _VIndirectRayTracingCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
    {
        VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

        _cmdbuf->BindDescriptorSet( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );
    }

/*
=================================================
    _PushConstant
=================================================
*/
    inline void  _VIndirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
    {
        VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

        _cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
