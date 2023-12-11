// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/RenderGraph/RGCommandBatch.h"

namespace AE::RG::_hidden_
{

#define RG_INHERIT_BARRIERS \
    private: \
        void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)                                  __Th_OV { return _ctx.BufferBarrier( buffer, srcState, dstState ); } \
        \
        void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)                            __Th_OV { return _ctx.BufferViewBarrier( view, srcState, dstState ); } \
        \
        void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)                                     __Th_OV { return _ctx.ImageBarrier( image, srcState, dstState ); } \
        void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)__Th_OV { return _ctx.ImageBarrier( image, srcState, dstState, subRes ); } \
        \
        void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)                              __Th_OV { return _ctx.ImageViewBarrier( view, srcState, dstState ); } \
        \
        void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)    __Th_OV { return _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
        void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)    __Th_OV { return _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
        \
        void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)       __Th_OV { return _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); } \
        void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)       __Th_OV { return _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); } \
        \
        ND_ CommandBatch &                  _Batch ()                                                                            __NE___ { return _ctx._GetBarrierMngr().GetBatch(); } \
        ND_ RGCommandBatchPtr::RGBatchData& _RGBatch ()                                                                          __NE___ { return *Cast<RGCommandBatchPtr::RGBatchData>( _Batch().GetUserData() ); } \
        ND_ uint                            _ExeIdx ()                                                                           __NE___ { return _ctx._GetBarrierMngr().GetRenderTask().GetExecutionIndex(); } \
        ND_ auto&                           _ResMngr ()                                                                          C_NE___ { return _ctx._GetBarrierMngr().GetResourceManager(); } \
        \
        ND_ auto  _SetResourceState (ImageID      id, EResourceState state)                                                      __NE___ { return _RGBatch().ResetResourceState( _ExeIdx(), id, state ); } \
        ND_ auto  _SetResourceState (ImageViewID  id, EResourceState state)                                                      __NE___ { auto& v = _ResMngr().GetResourcesOrThrow( id );  return _RGBatch().ResetResourceState( _ExeIdx(), v.ImageId(),  state ); } \
        \
    public: \
        void  CommitBarriers ()                                                                                                  __Th_OV { return _ctx.CommitBarriers(); } \
        \
        void  MemoryBarrier (EResourceState srcState, EResourceState dstState)                                                   __NE_OV { return _ctx.MemoryBarrier( srcState, dstState ); } \
        void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)                                                   __NE_OV { return _ctx.MemoryBarrier( srcScope, dstScope ); } \
        void  MemoryBarrier ()                                                                                                   __NE_OV { return _ctx.MemoryBarrier(); } \
        \
        void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)                                                __NE_OV { return _ctx.ExecutionBarrier( srcScope, dstScope ); } \
        void  ExecutionBarrier ()                                                                                                __NE_OV { return _ctx.ExecutionBarrier(); } \
        \
        void  DebugMarker (DebugLabel dbg)                                                                                       __Th_OV { return _ctx.DebugMarker( dbg ); } \
        void  PushDebugGroup (DebugLabel dbg)                                                                                    __Th_OV { return _ctx.PushDebugGroup( dbg ); } \
        void  PopDebugGroup ()                                                                                                   __Th_OV { return _ctx.PopDebugGroup(); } \
        \
        ND_ FrameUID                                GetFrameId ()                                                                C_NE_OF { return _ctx.GetFrameId(); } \
        \
        ND_ CommandBatch const&                     GetCommandBatch ()                                                           C_NE___ { return _ctx.GetCommandBatch(); } \
        ND_ CommandBatchPtr                         GetCommandBatchRC ()                                                         C_NE___ { return _ctx.GetCommandBatchRC(); } \
        ND_ RGCommandBatchPtr::RGBatchData const&   GetRGBatchData ()                                                            C_NE___ { return *Cast<RGCommandBatchPtr::RGBatchData>( _ctx._GetBarrierMngr().GetBatch().GetUserData() ); } \
        ND_ auto&                                   GetResourceManager ()                                                        C_NE___ { return _ResMngr(); } \
        ND_ RenderTask const&                       GetRenderTask ()                                                             C_NE___ { return _ctx.GetRenderTask(); } \
        \
        void  ResourceState (BufferID     id, EResourceState state)                                                              __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
        void  ResourceState (ImageID      id, EResourceState state)                                                              __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
        void  ResourceState (ImageViewID  id, EResourceState state)                                                              __Th___ { auto& v = _ResMngr().GetResourcesOrThrow( id );  _RGBatch().ResourceState( _ExeIdx(), _ctx, v.ImageId(),  state ); } \
        void  ResourceState (BufferViewID id, EResourceState state)                                                              __Th___ { auto& v = _ResMngr().GetResourcesOrThrow( id );  _RGBatch().ResourceState( _ExeIdx(), _ctx, v.BufferId(), state ); } \
        void  ResourceState (RTSceneID    id, EResourceState state)                                                              __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
        void  ResourceState (RTGeometryID id, EResourceState state)                                                              __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
        \
        ND_ auto  ReleaseCommandBuffer ()                                                                                        __Th___ { return _ctx.ReleaseCommandBuffer(); } \
        ND_ auto  EndCommandBuffer ()                                                                                            __Th___ { _RGBatch().FinalBarriers( _ExeIdx(), _ctx );  return _ctx.EndCommandBuffer(); } \
        \
        void  AddSurfaceTargets (const App::IOutputSurface::RenderTargets_t &targets)                                            __NE___ { _RGBatch().AddSurfaceTargets( _ExeIdx(), targets ); } \
        \
        void  UploadMemoryBarrier (EResourceState dstState)                                                                      __NE___ { _RGBatch().UploadMemoryBarrier( _ExeIdx(), _ctx, dstState ); } \
        void  ReadbackMemoryBarrier (EResourceState srcState)                                                                    __NE___ { _RGBatch().ReadbackMemoryBarrier( _ExeIdx(), _ctx, srcState ); } \
        \
        ND_ bool  HasUploadMemoryBarrier (EResourceState dstState)                                                               __NE___ { return _RGBatch().HasUploadMemoryBarrier( _ExeIdx(), dstState ); } \
        ND_ bool  HasReadbackMemoryBarrier (EResourceState srcState)                                                             __NE___ { return _RGBatch().HasReadbackMemoryBarrier( _ExeIdx(), srcState ); } \



    //
    // Transfer Context
    //

    template <typename BaseCtx>
    class TransferContext final : public ITransferContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< ITransferContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit TransferContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)  __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        TransferContext ()                                                                                      = delete;
        TransferContext (const TransferContext &)                                                               = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                         __NE___ { return _ctx; }

        void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)                                 __Th_OV;
        void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)                        __Th_OV;

        void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)                 __Th_OV;
        void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)                       __Th_OV;
        void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)       __Th_OV;
        void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)      __Th_OV;
        void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)       __Th_OV;
        void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)      __Th_OV;

        void  UploadBuffer (BufferID buffer, const UploadBufferDesc &desc, OUT BufferMemView &memView)          __Th_OV;
        void  UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)               __Th_OV;

        Promise<BufferMemView>  ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &desc)                __Th_OV;
        Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)                    __Th_OV;

        Promise<BufferMemView>  ReadbackBuffer (INOUT BufferStream &stream)                                     __Th_OV;
        Promise<ImageMemView>   ReadbackImage (INOUT ImageStream &stream)                                       __Th_OV;

        void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView)                             __Th_OV;
        void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView)                                __Th_OV;

        bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)                    __Th_OV;

        Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)                   __Th_OV;

        uint3  MinImageTransferGranularity ()                                                                   C_NE_OV { return _ctx.MinImageTransferGranularity(); }

        void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)  __Th_OV;

        void  GenerateMipmaps (ImageID image, EResourceState state = Default)                                                   __Th_OV;
        void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state = Default)          __Th_OV;

        using ITransferContext::UpdateHostBuffer;
        using ITransferContext::UploadBuffer;
        using ITransferContext::UploadImage;

      #ifdef AE_ENABLE_VULKAN
        void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)                    __Th___;
        void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)                    __Th___;
        void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)                    __Th___;

        void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th___;
        void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)                                __Th___;
      #endif

        RG_INHERIT_BARRIERS
    };



    //
    // Compute Context
    //

    template <typename BaseCtx>
    class ComputeContext final : public IComputeContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IComputeContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit ComputeContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)               __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        ComputeContext ()                                                                                                   = delete;
        ComputeContext (const ComputeContext &)                                                                             = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                                     __NE___ { return _ctx; }

        void  BindPipeline (ComputePipelineID ppln)                                                                         __Th_OV { return _ctx.BindPipeline( ppln ); }
        void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)        __Th_OV { return _ctx.BindDescriptorSet( index, ds, dynamicOffsets ); }

        void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, const ShaderStructName &typeName) __Th_OV { return _ctx.PushConstant( idx, size, values, typeName ); }
        using IComputeContext::PushConstant;

        using IComputeContext::Dispatch;

        void  Dispatch (const uint3 &groupCount)                                                                            __Th_OV { return _ctx.Dispatch( groupCount ); }
        void  DispatchIndirect (BufferID buffer, Bytes offset)                                                              __Th_OV;

        RG_INHERIT_BARRIERS
    };



    //
    // Draw Context
    //

    template <typename BaseCtx>
    class DrawContext final : public IDrawContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IDrawContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit DrawContext (const DrawTask &task)                                                                         __Th___ : _ctx{ task } {}
        explicit DrawContext (BaseCtx &&ctx)                                                                                __Th___ : _ctx{ RVRef(ctx) } {}

        DrawContext ()                                                                                                      = delete;
        DrawContext (const DrawContext &)                                                                                   = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                                     __NE___ { return _ctx; }

    // pipeline and shader resources //
        void  BindPipeline (GraphicsPipelineID ppln)                                                                        __Th_OV { return _ctx.BindPipeline( ppln ); }
        void  BindPipeline (MeshPipelineID ppln)                                                                            __Th_OV { return _ctx.BindPipeline( ppln ); }
        void  BindPipeline (TilePipelineID ppln)                                                                            __Th_OV { return _ctx.BindPipeline( ppln ); }
        void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)        __Th_OV { return _ctx.BindDescriptorSet( index, ds, dynamicOffsets ); }

        void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, const ShaderStructName &typeName) __Th_OV { return _ctx.PushConstant( idx, size, values, typeName ); }
        using IDrawContext::PushConstant;

    // dynamic states //
        void  SetViewport (const Viewport_t &viewport)                                                                      __Th_OV { return _ctx.SetViewport( viewport ); }
        void  SetViewports (ArrayView<Viewport_t> viewports)                                                                __Th_OV { return _ctx.SetViewports( viewports ); }
        void  SetScissor (const RectI &scissor)                                                                             __Th_OV { return _ctx.SetScissor( scissor ); }
        void  SetScissors (ArrayView<RectI> scissors)                                                                       __Th_OV { return _ctx.SetScissors( scissors ); }
        void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)                __Th_OV { return _ctx.SetDepthBias( depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor ); }
        void  SetStencilReference (uint reference)                                                                          __Th_OV { return _ctx.SetStencilReference( reference ); }
        void  SetStencilReference (uint frontReference, uint backReference)                                                 __Th_OV { return _ctx.SetStencilReference( frontReference, backReference ); }
        void  SetBlendConstants (const RGBA32f &color)                                                                      __Th_OV { return _ctx.SetBlendConstants( color ); }

      #ifdef AE_ENABLE_VULKAN
        void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)                                                   __Th___ { return _ctx.SetDepthBounds( minDepthBounds, maxDepthBounds ); }
        void  SetStencilCompareMask (uint compareMask)                                                                      __Th___ { return _ctx.SetStencilCompareMask( compareMask ); }
        void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)                                           __Th___ { return _ctx.SetStencilCompareMask( frontCompareMask, backCompareMask ); }
        void  SetStencilWriteMask (uint writeMask)                                                                          __Th___ { return _ctx.SetStencilWriteMask( writeMask ); }
        void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)                                                 __Th___ { return _ctx.SetStencilWriteMask( frontWriteMask, backWriteMask ); }
      #endif

    // draw commands //

        void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)                                             __Th_OV;
        void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)                                                  __Th_OV;
        void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)                  __Th_OV;
        bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)     __Th_OV;

        using IDrawContext::Draw;
        using IDrawContext::DrawIndexed;
        using IDrawContext::DrawIndirect;
        using IDrawContext::DrawIndexedIndirect;
        using IDrawContext::DrawMeshTasksIndirect;

        void  Draw (uint vertexCount,
                    uint instanceCount  = 1,
                    uint firstVertex    = 0,
                    uint firstInstance  = 0)                                                                                __Th_OV { return _ctx.Draw( vertexCount, instanceCount, firstVertex, firstInstance ); }

        void  DrawIndexed (uint indexCount,
                           uint instanceCount   = 1,
                           uint firstIndex      = 0,
                           int  vertexOffset    = 0,
                           uint firstInstance   = 0)                                                                        __Th_OV { return _ctx.DrawIndexed( indexCount, instanceCount, firstIndex, vertexOffset, firstInstance ); }

        void  DrawIndirect (BufferID    indirectBuffer,
                            Bytes       indirectBufferOffset,
                            uint        drawCount,
                            Bytes       stride)                                                                             __Th_OV;

        void  DrawIndexedIndirect (BufferID     indirectBuffer,
                                   Bytes        indirectBufferOffset,
                                   uint         drawCount,
                                   Bytes        stride)                                                                     __Th_OV;

        // tile shader //
        void  DispatchTile ()                                                                                               __Th_OV { return _ctx.DispatchTile(); }

        // mesh shader //
        void  DrawMeshTasks (const uint3 &taskCount)                                                                        __Th_OV { return _ctx.DrawMeshTasks( taskCount ); }

        void  DrawMeshTasksIndirect (BufferID   indirectBuffer,
                                     Bytes      indirectBufferOffset,
                                     uint       drawCount,
                                     Bytes      stride)                                                                     __Th_OV;

        // extension
      #ifdef AE_ENABLE_VULKAN
        void  DrawIndirectCount (const DrawIndirectCountCmd &cmd)                                                           __Th___ { DrawIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
        void  DrawIndirectCount (BufferID   indirectBuffer,
                                 Bytes      indirectBufferOffset,
                                 BufferID   countBuffer,
                                 Bytes      countBufferOffset,
                                 uint       maxDrawCount,
                                 Bytes      stride)                                                                         __Th___;

        void  DrawIndexedIndirectCount (const DrawIndexedIndirectCountCmd &cmd)                                             __Th___ { DrawIndexedIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
        void  DrawIndexedIndirectCount (BufferID    indirectBuffer,
                                        Bytes       indirectBufferOffset,
                                        BufferID    countBuffer,
                                        Bytes       countBufferOffset,
                                        uint        maxDrawCount,
                                        Bytes       stride)                                                                 __Th___;

        void  DrawMeshTasksIndirectCount (const DrawMeshTasksIndirectCountCmd &cmd)                                         __Th___ { DrawMeshTasksIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
        void  DrawMeshTasksIndirectCount (BufferID  indirectBuffer,
                                          Bytes     indirectBufferOffset,
                                          BufferID  countBuffer,
                                          Bytes     countBufferOffset,
                                          uint      maxDrawCount,
                                          Bytes     stride)                                                                 __Th___;
      #endif

        // for debugging //
        void  DebugMarker (DebugLabel dbg)                                                                                  __Th_OV { return _ctx.DebugMarker( dbg ); }
        void  PushDebugGroup (DebugLabel dbg)                                                                               __Th_OV { return _ctx.PushDebugGroup( dbg ); }
        void  PopDebugGroup ()                                                                                              __Th_OV { return _ctx.PopDebugGroup(); }

        // only for RW attachments //
        void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)                     __Th_OV { return _ctx.AttachmentBarrier( name, srcState, dstState ); }
        void  CommitBarriers ()                                                                                             __Th_OV { return _ctx.CommitBarriers(); }

        // vertex stream //
        bool  AllocVStream (Bytes size, OUT VertexStream &result)                                                           __Th_OV;

        // clear //
        bool  ClearAttachment (AttachmentName name, const RGBA32f &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV { return _ctx.ClearAttachment( name, color, rect, baseLayer, layerCount ); }
        bool  ClearAttachment (AttachmentName name, const RGBA32u &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV { return _ctx.ClearAttachment( name, color, rect, baseLayer, layerCount ); }
        bool  ClearAttachment (AttachmentName name, const RGBA32i &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV { return _ctx.ClearAttachment( name, color, rect, baseLayer, layerCount ); }
        bool  ClearAttachment (AttachmentName name, const RGBA8u  &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV { return _ctx.ClearAttachment( name, color, rect, baseLayer, layerCount ); }
        bool  ClearAttachment (AttachmentName name, const DepthStencil &ds, const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV { return _ctx.ClearAttachment( name, ds, rect, baseLayer, layerCount ); }

        // check state //
        ND_ bool  CheckResourceState (ImageID      id, EResourceState state)                                                __NE___ { return _RGBatch().CheckResourceState( _ExeIdx(), id, state ); }
        ND_ bool  CheckResourceState (BufferID     id, EResourceState state)                                                __NE___ { return _RGBatch().CheckResourceState( _ExeIdx(), id, state ); }
        ND_ bool  CheckResourceState (RTGeometryID id, EResourceState state)                                                __NE___ { return _RGBatch().CheckResourceState( _ExeIdx(), id, state ); }
        ND_ bool  CheckResourceState (RTSceneID    id, EResourceState state)                                                __NE___ { return _RGBatch().CheckResourceState( _ExeIdx(), id, state ); }

        ND_ FrameUID    GetFrameId ()                                                                                       C_NE_OF { return _ctx.GetFrameId(); }

        ND_ bool  HasUploadMemoryBarrier (EResourceState dstState)                                                          __NE___ { return _RGBatch().HasUploadMemoryBarrier( _ExeIdx(), dstState ); } \
        ND_ bool  HasReadbackMemoryBarrier (EResourceState srcState)                                                        __NE___ { return _RGBatch().HasReadbackMemoryBarrier( _ExeIdx(), srcState ); } \

    private:
        ND_ uint                            _ExeIdx ()                                                                      __NE___ { return _RTask().GetExecutionIndex(); }
        ND_ RGCommandBatchPtr::RGBatchData& _RGBatch ()                                                                     __NE___ { return *Cast<RGCommandBatchPtr::RGBatchData>( _RTask().GetBatchPtr()->GetUserData() ); }
        ND_ RenderTask const&               _RTask ()                                                                       __NE___ { ASSERT( _ctx.GetPrimaryCtxState().userData != null );  return *Cast<RenderTask>( _ctx.GetPrimaryCtxState().userData ); }
    };



    //
    // Graphics Context
    //

    template <typename BaseCtx>
    class GraphicsContext final : public IGraphicsContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IGraphicsContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using DrawCtx   = DrawContext< typename BaseCtx::DrawCtx >;
        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit GraphicsContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)  __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        // continue render pass
        GraphicsContext (const RenderTask &task, const DrawCommandBatch &batch, CmdBuf_t cmdbuf)                __Th___ : _ctx{ task, batch, RVRef(cmdbuf) } {}

        GraphicsContext ()                                                                                      = delete;
        GraphicsContext (const GraphicsContext &)                                                               = delete;

        ND_ BaseCtx&    GetBaseContext ()                                                                       __NE___ { return _ctx; }

        ND_ auto const& GetState ()                                                                             C_NE___ { return _ctx.GetState(); }
        ND_ bool        IsInsideRenderPass ()                                                                   C_NE___ { return _ctx.IsInsideRenderPass(); }
        ND_ bool        IsSecondaryCmdbuf ()                                                                    C_NE___ { return _ctx.IsSecondaryCmdbuf(); }

        ND_ DrawCtx BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)                      __Th___;
        ND_ DrawCtx NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg = Default)                                __Th___;
            void    EndRenderPass (DrawCtx& ctx)                                                                __Th___;

        // returns 'DrawCommandBatch'
        ND_ auto    BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)                    __Th___;
        ND_ auto    NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg = Default)             __Th___;
            void    EndMtRenderPass ()                                                                          __Th___;
            void    ExecuteSecondary (DrawCommandBatch &batch)                                                  __Th___ { return _ctx.ExecuteSecondary( batch ); }

        RG_INHERIT_BARRIERS
    };



    //
    // Ray Tracing Context
    //

    template <typename BaseCtx>
    class RayTracingContext final : public IRayTracingContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IRayTracingContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit RayTracingContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)            __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        RayTracingContext ()                                                                                                = delete;
        RayTracingContext (const RayTracingContext &)                                                                       = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                                     __NE___ { return _ctx; }

        void  BindPipeline (RayTracingPipelineID ppln)                                                                      __Th_OV { return _ctx.BindPipeline( ppln ); }
        void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)        __Th_OV { return _ctx.BindDescriptorSet( index, ds, dynamicOffsets ); }

        void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, const ShaderStructName &typeName) __Th_OV { return _ctx.PushConstant( idx, size, values, typeName ); }
        using IRayTracingContext::PushConstant;

        void  SetStackSize (Bytes size)                                                                                     __Th_OV { return _ctx.SetStackSize( size ); }

        void  TraceRays (const uint2 dim, RTShaderBindingID sbt)                                                            __Th_OV;
        void  TraceRays (const uint3 dim, RTShaderBindingID sbt)                                                            __Th_OV;

        void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)                                                  __Th_OV;
        void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)                                                  __Th_OV;

        void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)                __Th_OV;
        void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)      __Th_OV;

        void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)                                      __Th_OV;

        RG_INHERIT_BARRIERS
    };



    //
    // Acceleration Structure Build Context
    //

    template <typename BaseCtx>
    class ASBuildContext final : public IASBuildContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IASBuildContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit ASBuildContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)               __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        ASBuildContext ()                                                                                                   = delete;
        ASBuildContext (const ASBuildContext &)                                                                             = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                                     __NE___ { return _ctx; }

        void  Build (const RTGeometryBuild &cmd, RTGeometryID dst)                                                          __Th_OV;
        void  Build (const RTSceneBuild &cmd, RTSceneID dst)                                                                __Th_OV;

        void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)                                       __Th_OV;
        void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)                                                __Th_OV;

        void  Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)                          __Th_OV;
        void  Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)                                __Th_OV;

        void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)  __Th_OV;
        void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)     __Th_OV;

        Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)                                              __Th_OV;
        Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)                                                 __Th_OV;

      #ifdef AE_ENABLE_VULKAN
        void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
                             BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b,
                             Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)                                         __Th___;
        void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst,
                             BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b)                                     __Th___;

        void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)                                           __Th___;
        void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)                                              __Th___;

        void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)                                       __Th___;
        void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)                                          __Th___;
      #endif

        RG_INHERIT_BARRIERS

    private:
        void  _RTGeometryBuildBarriers (const RTGeometryBuild &cmd);
        void  _RTSceneBuildBarriers (const RTSceneBuild &cmd);
    };



    //
    // Video Decode Context
    //

    template <typename BaseCtx>
    class VideoDecodeContext final : public IVideoDecodeContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IVideoDecodeContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit VideoDecodeContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)           __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        VideoDecodeContext ()                                                                                               = delete;
        VideoDecodeContext (const VideoDecodeContext &)                                                                     = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                                     __NE___ { return _ctx; }

        RG_INHERIT_BARRIERS
    };



    //
    // Video Encode Context
    //

    template <typename BaseCtx>
    class VideoEncodeContext final : public IVideoEncodeContext
    {
    // types
    public:
        StaticAssert( IsBaseOf< IVideoEncodeContext, BaseCtx >);

        static constexpr bool   IsIndirectContext = BaseCtx::IsIndirectContext;

        using CmdBuf_t  = typename BaseCtx::CmdBuf_t;


    // variables
    private:
        BaseCtx     _ctx;


    // methods
    public:
        explicit VideoEncodeContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)           __Th___ : _ctx{ task, RVRef(cmdbuf), dbg } {}

        VideoEncodeContext ()                                                                                               = delete;
        VideoEncodeContext (const VideoEncodeContext &)                                                                     = delete;

        ND_ BaseCtx&  GetBaseContext ()                                                                                     __NE___ { return _ctx; }

        RG_INHERIT_BARRIERS
    };
//-----------------------------------------------------------------------------



    template <typename C>
    void  TransferContext<C>::FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data) __Th___
    {
        ResourceState( buffer, EResourceState::ClearDst );
        _ctx.CommitBarriers();
        _ctx.FillBuffer( buffer, offset, size, data );
    }

    template <typename C>
    void  TransferContext<C>::UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) __Th___
    {
        ResourceState( buffer, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.UpdateBuffer( buffer, offset, size, data );
    }

    template <typename C>
    void  TransferContext<C>::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) __Th___
    {
        ResourceState( srcBuffer, EResourceState::CopySrc );
        ResourceState( dstBuffer, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.CopyBuffer( srcBuffer, dstBuffer, ranges );
    }

    template <typename C>
    void  TransferContext<C>::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) __Th___
    {
        ResourceState( srcImage, EResourceState::CopySrc );
        ResourceState( dstImage, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.CopyImage( srcImage, dstImage, ranges );
    }

    template <typename C>
    void  TransferContext<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) __Th___
    {
        ResourceState( srcBuffer, EResourceState::CopySrc );
        ResourceState( dstImage,  EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.CopyBufferToImage( srcBuffer, dstImage, ranges );
    }

    template <typename C>
    void  TransferContext<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) __Th___
    {
        ResourceState( srcBuffer, EResourceState::CopySrc );
        ResourceState( dstImage,  EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.CopyBufferToImage( srcBuffer, dstImage, ranges );
    }

    template <typename C>
    void  TransferContext<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) __Th___
    {
        ResourceState( srcImage,  EResourceState::CopySrc );
        ResourceState( dstBuffer, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.CopyImageToBuffer( srcImage, dstBuffer, ranges );
    }

    template <typename C>
    void  TransferContext<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) __Th___
    {
        ResourceState( srcImage,  EResourceState::CopySrc );
        ResourceState( dstBuffer, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.CopyImageToBuffer( srcImage, dstBuffer, ranges );
    }

    template <typename C>
    void  TransferContext<C>::UploadBuffer (BufferID buffer, const UploadBufferDesc &uploadDesc, OUT BufferMemView &memView) __Th___
    {
        UploadMemoryBarrier( EResourceState::CopySrc );
        ResourceState( buffer, EResourceState::CopyDst );

        _ctx.CommitBarriers();
        _ctx.UploadBuffer( buffer, uploadDesc, OUT memView );
    }

    template <typename C>
    void  TransferContext<C>::UploadImage (ImageID image, const UploadImageDesc &uploadDesc, OUT ImageMemView &memView) __Th___
    {
        UploadMemoryBarrier( EResourceState::CopySrc );
        ResourceState( image, EResourceState::CopyDst );

        _ctx.CommitBarriers();
        _ctx.UploadImage( image, uploadDesc, OUT memView );
    }

    template <typename C>
    Promise<BufferMemView>  TransferContext<C>::ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &readDesc) __Th___
    {
        ResourceState( buffer, EResourceState::CopySrc );
        ReadbackMemoryBarrier( EResourceState::CopyDst );

        _ctx.CommitBarriers();
        return _ctx.ReadbackBuffer( buffer, readDesc );
    }

    template <typename C>
    Promise<BufferMemView>  TransferContext<C>::ReadbackBuffer (INOUT BufferStream &stream) __Th___
    {
        ResourceState( stream.BufferId(), EResourceState::CopySrc );
        ReadbackMemoryBarrier( EResourceState::CopyDst );

        _ctx.CommitBarriers();
        return _ctx.ReadbackBuffer( stream );
    }

    template <typename C>
    Promise<ImageMemView>  TransferContext<C>::ReadbackImage (ImageID image, const ReadbackImageDesc &desc) __Th___
    {
        ResourceState( image, EResourceState::CopySrc );
        ReadbackMemoryBarrier( EResourceState::CopyDst );

        _ctx.CommitBarriers();
        return _ctx.ReadbackImage( image, desc );
    }

    template <typename C>
    Promise<ImageMemView>  TransferContext<C>::ReadbackImage (INOUT ImageStream &stream) __Th___
    {
        ResourceState( stream.ImageId(), EResourceState::CopySrc );
        ReadbackMemoryBarrier( EResourceState::CopyDst );

        _ctx.CommitBarriers();
        return _ctx.ReadbackImage( stream );
    }

    template <typename C>
    void  TransferContext<C>::UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView) __Th___
    {
        UploadMemoryBarrier( EResourceState::CopySrc );
        ResourceState( stream.BufferId(), EResourceState::CopyDst );

        _ctx.CommitBarriers();
        _ctx.UploadBuffer( INOUT stream, OUT memView );
    }

    template <typename C>
    void  TransferContext<C>::UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView) __Th___
    {
        UploadMemoryBarrier( EResourceState::CopySrc );
        ResourceState( stream.ImageId(), EResourceState::CopyDst );

        _ctx.CommitBarriers();
        _ctx.UploadImage( INOUT stream, OUT memView );
    }

    template <typename C>
    bool  TransferContext<C>::UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) __Th___
    {
        ResourceState( buffer, EResourceState::Host_Write );
        return _ctx.UpdateHostBuffer( buffer, offset, size, data );
    }

    template <typename C>
    Promise<ArrayView<ubyte>>  TransferContext<C>::ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size) __Th___
    {
        ResourceState( buffer, EResourceState::Host_Read );
        _ctx.CommitBarriers();
        return _ctx.ReadHostBuffer( buffer, offset, size );
    }

    template <typename C>
    void  TransferContext<C>::GenerateMipmaps (ImageID image, EResourceState state) __Th___
    {
        const EResourceState    prev_state = _SetResourceState( image, EResourceState::BlitSrc );
        ASSERT( prev_state == state or state == Default );
        Unused( state );

        _ctx.CommitBarriers();
        _ctx.GenerateMipmaps( image, prev_state );
    }

    template <typename C>
    void  TransferContext<C>::GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state) __Th___
    {
        const EResourceState    prev_state = _SetResourceState( image, EResourceState::BlitSrc );
        ASSERT( prev_state == state or state == Default );
        Unused( state );

        _ctx.CommitBarriers();
        _ctx.GenerateMipmaps( image, ranges, prev_state );
    }

    template <typename C>
    void  TransferContext<C>::BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions) __Th___
    {
        ResourceState( srcImage, EResourceState::BlitSrc );
        ResourceState( dstImage, EResourceState::BlitDst );
        _ctx.CommitBarriers();
        _ctx.BlitImage( srcImage, dstImage, filter, regions );
    }

#ifdef AE_ENABLE_VULKAN
    template <typename C>
    void  TransferContext<C>::ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges) __Th___
    {
        ResourceState( image, EResourceState::ClearDst );
        _ctx.CommitBarriers();
        _ctx.ClearColorImage( image, color, ranges );
    }

    template <typename C>
    void  TransferContext<C>::ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges) __Th___
    {
        ResourceState( image, EResourceState::ClearDst );
        _ctx.CommitBarriers();
        _ctx.ClearColorImage( image, color, ranges );
    }

    template <typename C>
    void  TransferContext<C>::ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges) __Th___
    {
        ResourceState( image, EResourceState::ClearDst );
        _ctx.CommitBarriers();
        _ctx.ClearColorImage( image, color, ranges );
    }

    template <typename C>
    void  TransferContext<C>::ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th___
    {
        ResourceState( image, EResourceState::ClearDst );
        _ctx.CommitBarriers();
        _ctx.ClearDepthStencilImage( image, depthStencil, ranges );
    }

    template <typename C>
    void  TransferContext<C>::ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions) __Th___
    {
        ResourceState( srcImage, EResourceState::BlitSrc );
        ResourceState( dstImage, EResourceState::BlitDst );
        _ctx.CommitBarriers();
        _ctx.ResolveImage( srcImage, dstImage, regions );
    }
#endif
//-----------------------------------------------------------------------------



    template <typename C>
    void  ComputeContext<C>::DispatchIndirect (BufferID buffer, Bytes offset) __Th___
    {
        ResourceState( buffer, EResourceState::IndirectBuffer );
        _ctx.CommitBarriers();
        _ctx.DispatchIndirect( buffer, offset );
    }
//-----------------------------------------------------------------------------



    template <typename C>
    void  DrawContext<C>::BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) __Th___
    {
        CHECK( CheckResourceState( buffer, EResourceState::IndexBuffer ));
        _ctx.BindIndexBuffer( buffer, offset, indexType );
    }

    template <typename C>
    void  DrawContext<C>::BindVertexBuffer (uint index, BufferID buffer, Bytes offset) __Th___
    {
        CHECK( CheckResourceState( buffer, EResourceState::VertexBuffer ));
        _ctx.BindVertexBuffer( index, buffer, offset );
    }

    template <typename C>
    void  DrawContext<C>::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) __Th___
    {
        for (auto id : buffers) {
            CHECK( CheckResourceState( id, EResourceState::VertexBuffer ));
        }
        _ctx.BindVertexBuffers( firstBinding, buffers, offsets );
    }

    template <typename C>
    bool  DrawContext<C>::BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset) __Th___
    {
        CHECK( CheckResourceState( buffer, EResourceState::VertexBuffer ));
        return _ctx.BindVertexBuffer( pplnId, name, buffer, offset );
    }

    template <typename C>
    void  DrawContext<C>::DrawIndirect (BufferID    indirectBuffer,
                                        Bytes       indirectBufferOffset,
                                        uint        drawCount,
                                        Bytes       stride) __Th___
    {
        CHECK( CheckResourceState( indirectBuffer, EResourceState::IndirectBuffer ));
        _ctx.DrawIndirect( indirectBuffer, indirectBufferOffset, drawCount, stride );
    }

    template <typename C>
    void  DrawContext<C>::DrawIndexedIndirect (BufferID     indirectBuffer,
                                               Bytes        indirectBufferOffset,
                                               uint         drawCount,
                                               Bytes        stride) __Th___
    {
        CHECK( CheckResourceState( indirectBuffer, EResourceState::IndirectBuffer ));
        _ctx.DrawIndexedIndirect( indirectBuffer, indirectBufferOffset, drawCount, stride );
    }

    template <typename C>
    void  DrawContext<C>::DrawMeshTasksIndirect (BufferID   indirectBuffer,
                                                 Bytes      indirectBufferOffset,
                                                 uint       drawCount,
                                                 Bytes      stride) __Th___
    {
        CHECK( CheckResourceState( indirectBuffer, EResourceState::IndirectBuffer ));
        _ctx.DrawMeshTasksIndirect( indirectBuffer, indirectBufferOffset, drawCount, stride );
    }

#ifdef AE_ENABLE_VULKAN
    template <typename C>
    void  DrawContext<C>::DrawIndirectCount (BufferID   indirectBuffer,
                                             Bytes      indirectBufferOffset,
                                             BufferID   countBuffer,
                                             Bytes      countBufferOffset,
                                             uint       maxDrawCount,
                                             Bytes      stride) __Th___
    {
        CHECK( CheckResourceState( indirectBuffer, EResourceState::IndirectBuffer ));
        CHECK( CheckResourceState( countBuffer, EResourceState::IndirectBuffer ));
        _ctx.DrawIndirectCount( indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride );
    }

    template <typename C>
    void  DrawContext<C>::DrawIndexedIndirectCount (BufferID    indirectBuffer,
                                                    Bytes       indirectBufferOffset,
                                                    BufferID    countBuffer,
                                                    Bytes       countBufferOffset,
                                                    uint        maxDrawCount,
                                                    Bytes       stride) __Th___
    {
        CHECK( CheckResourceState( indirectBuffer, EResourceState::IndirectBuffer ));
        CHECK( CheckResourceState( countBuffer, EResourceState::IndirectBuffer ));
        _ctx.DrawIndexedIndirectCount( indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride );
    }

    template <typename C>
    void  DrawContext<C>::DrawMeshTasksIndirectCount (BufferID  indirectBuffer,
                                                      Bytes     indirectBufferOffset,
                                                      BufferID  countBuffer,
                                                      Bytes     countBufferOffset,
                                                      uint      maxDrawCount,
                                                      Bytes     stride) __Th___
    {
        CHECK( CheckResourceState( indirectBuffer, EResourceState::IndirectBuffer ));
        CHECK( CheckResourceState( countBuffer, EResourceState::IndirectBuffer ));
        _ctx.DrawMeshTasksIndirectCount( indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride );
    }
#endif

    template <typename C>
    bool  DrawContext<C>::AllocVStream (Bytes size, OUT VertexStream &result) __Th___
    {
        CHECK( HasUploadMemoryBarrier( EResourceState::VertexBuffer ));
        return _ctx.AllocVStream( size, OUT result );
    }
//-----------------------------------------------------------------------------



    template <typename C>
    typename GraphicsContext<C>::DrawCtx  GraphicsContext<C>::BeginRenderPass (const RenderPassDesc &inDesc, DebugLabel dbg) __Th___
    {
        RenderPassDesc  desc = inDesc;
        _RGBatch().SetRenderPassInitialStates( _ExeIdx(), INOUT desc );
        return DrawCtx{ _ctx.BeginRenderPass( desc, dbg, BitCast< void *>(&GetRenderTask()) )};
    }

    template <typename C>
    typename GraphicsContext<C>::DrawCtx  GraphicsContext<C>::NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg) __Th___
    {
        return DrawCtx{ _ctx.NextSubpass( prevPassCtx, dbg, BitCast< void *>(&GetRenderTask()) )};
    }

    template <typename C>
    void  GraphicsContext<C>::EndRenderPass (DrawCtx& ctx) __Th___
    {
        _RGBatch().SetRenderPassFinalStates( _ExeIdx(), ctx.GetBaseContext().GetPrimaryCtxState() );
        return _ctx.EndRenderPass( ctx.GetBaseContext() );
    }

    template <typename C>
    auto  GraphicsContext<C>::BeginMtRenderPass (const RenderPassDesc &inDesc, DebugLabel dbg) __Th___
    {
        RenderPassDesc  desc = inDesc;
        _RGBatch().SetRenderPassInitialStates( _ExeIdx(), INOUT desc );
        return _ctx.BeginMtRenderPass( desc, dbg, BitCast< void *>(&GetRenderTask()) );
    }

    template <typename C>
    auto  GraphicsContext<C>::NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg) __Th___
    {
        return _ctx.NextMtSubpass( prevPassBatch, dbg, BitCast< void *>(&GetRenderTask()) );
    }

    template <typename C>
    void  GraphicsContext<C>::EndMtRenderPass () __Th___
    {
        _RGBatch().SetRenderPassFinalStates( _ExeIdx(), _ctx.GetPrimaryCtxState() );
        _ctx.EndMtRenderPass();
    }
//-----------------------------------------------------------------------------



    template <typename C>
    void  RayTracingContext<C>::TraceRays (const uint2 dim, RTShaderBindingID sbt) __Th___
    {
        _ctx.CommitBarriers();
        _ctx.TraceRays( dim, sbt );
    }

    template <typename C>
    void  RayTracingContext<C>::TraceRays (const uint3 dim, RTShaderBindingID sbt) __Th___
    {
        _ctx.CommitBarriers();
        _ctx.TraceRays( dim, sbt );
    }

    template <typename C>
    void  RayTracingContext<C>::TraceRays (const uint2 dim, const RTShaderBindingTable &sbt) __Th___
    {
        _ctx.CommitBarriers();
        _ctx.TraceRays( dim, sbt );
    }

    template <typename C>
    void  RayTracingContext<C>::TraceRays (const uint3 dim, const RTShaderBindingTable &sbt) __Th___
    {
        _ctx.CommitBarriers();
        _ctx.TraceRays( dim, sbt );
    }

    template <typename C>
    void  RayTracingContext<C>::TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
        _ctx.CommitBarriers();
        _ctx.TraceRaysIndirect( sbt, indirectBuffer, indirectBufferOffset );
    }

    template <typename C>
    void  RayTracingContext<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
        _ctx.CommitBarriers();
        _ctx.TraceRaysIndirect( sbt, indirectBuffer, indirectBufferOffset );
    }

    template <typename C>
    void  RayTracingContext<C>::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
        _ctx.CommitBarriers();
        _ctx.TraceRaysIndirect2( indirectBuffer, indirectBufferOffset );
    }
//-----------------------------------------------------------------------------



    template <typename C>
    void  ASBuildContext<C>::_RTGeometryBuildBarriers (const RTGeometryBuild &cmd)
    {
        auto    triangles = cmd.triangles.get< RTGeometryBuild::TrianglesData >();
        for (auto& item : triangles) {
            ResourceState( item.vertexData,     EResourceState::BuildRTAS_Read );
            ResourceState( item.indexData,      EResourceState::BuildRTAS_Read );
            ResourceState( item.transformData,  EResourceState::BuildRTAS_Read );
        }

        auto    aabbs = cmd.aabbs.get< RTGeometryBuild::AABBsData >();
        for (auto& item : aabbs) {
            ResourceState( item.data, EResourceState::BuildRTAS_Read );
        }

        ResourceState( cmd.scratch.id, EResourceState::BuildRTAS_ScratchBuffer );
    }

    template <typename C>
    void  ASBuildContext<C>::_RTSceneBuildBarriers (const RTSceneBuild &cmd)
    {
        for (auto& id : cmd.uniqueGeoms) {
            ResourceState( id, EResourceState::BuildRTAS_Read );
        }
        ResourceState( cmd.instanceData.id, EResourceState::BuildRTAS_Read );
        ResourceState( cmd.scratch.id, EResourceState::BuildRTAS_ScratchBuffer );
    }

    template <typename C>
    void  ASBuildContext<C>::Build (const RTGeometryBuild &cmd, RTGeometryID dst) __Th___
    {
        _RTGeometryBuildBarriers( cmd );
        ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );
        _ctx.CommitBarriers();
        _ctx.Build( cmd, dst );
    }

    template <typename C>
    void  ASBuildContext<C>::Build (const RTSceneBuild &cmd, RTSceneID dst) __Th___
    {
        _RTSceneBuildBarriers( cmd );
        ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );

        _ctx.CommitBarriers();
        _ctx.Build( cmd, dst );
    }

    template <typename C>
    void  ASBuildContext<C>::Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) __Th___
    {
        _RTGeometryBuildBarriers( cmd );
        ResourceState( src, EResourceState::BuildRTAS_Read );
        ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );

        _ctx.CommitBarriers();
        _ctx.Update( cmd, src, dst );
    }

    template <typename C>
    void  ASBuildContext<C>::Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) __Th___
    {
        _RTSceneBuildBarriers( cmd );
        ResourceState( src, EResourceState::BuildRTAS_Read );
        ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );

        _ctx.CommitBarriers();
        _ctx.Update( cmd, src, dst );
    }

    template <typename C>
    void  ASBuildContext<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode) __Th___
    {
        ResourceState( src, EResourceState::CopyRTAS_Read );
        ResourceState( dst, EResourceState::CopyRTAS_Write );
        _ctx.CommitBarriers();
        _ctx.Copy( src, dst, mode );
    }

    template <typename C>
    void  ASBuildContext<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode) __Th___
    {
        ResourceState( src, EResourceState::CopyRTAS_Read );
        ResourceState( dst, EResourceState::CopyRTAS_Write );
        _ctx.CommitBarriers();
        _ctx.Copy( src, dst, mode );
    }

    template <typename C>
    void  ASBuildContext<C>::WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
    {
        ResourceState( as,        EResourceState::CopyRTAS_Read );
        ResourceState( dstBuffer, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.WriteProperty( property, as, dstBuffer, offset, size );
    }

    template <typename C>
    void  ASBuildContext<C>::WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
    {
        ResourceState( as,        EResourceState::CopyRTAS_Read );
        ResourceState( dstBuffer, EResourceState::CopyDst );
        _ctx.CommitBarriers();
        _ctx.WriteProperty( property, as, dstBuffer, offset, size );
    }

    template <typename C>
    Promise<Bytes>  ASBuildContext<C>::ReadProperty (ERTASProperty property, RTGeometryID as) __Th___
    {
        ResourceState( as, EResourceState::CopyRTAS_Read );
        _ctx.CommitBarriers();
        return _ctx.ReadProperty( property, as );
    }

    template <typename C>
    Promise<Bytes>  ASBuildContext<C>::ReadProperty (ERTASProperty property, RTSceneID as) __Th___
    {
        ResourceState( as, EResourceState::CopyRTAS_Read );
        _ctx.CommitBarriers();
        return _ctx.ReadProperty( property, as );
    }

#ifdef AE_ENABLE_VULKAN
    template <typename C>
    void  ASBuildContext<C>::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, BufferID indirectBuffer,
                                            Bytes indirectBufferOffset, Bytes indirectStride) __Th___
    {
        _RTGeometryBuildBarriers( cmd );
        ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );
        ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
        _ctx.CommitBarriers();
        _ctx.BuildIndirect( cmd, dst, indirectBuffer, indirectBufferOffset, indirectStride );
    }

    template <typename C>
    void  ASBuildContext<C>::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        _RTSceneBuildBarriers( cmd );
        ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );
        ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
        _ctx.CommitBarriers();
        _ctx.BuildIndirect( cmd, dst, indirectBuffer, indirectBufferOffset );
    }

    template <typename C>
    void  ASBuildContext<C>::SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset) __Th___
    {
        ResourceState( src, EResourceState::CopyRTAS_Read );
        ResourceState( dst, EResourceState::CopyRTAS_Write );
        _ctx.CommitBarriers();
        _ctx.SerializeToMemory( src, dst, dstOffset );
    }

    template <typename C>
    void  ASBuildContext<C>::SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset) __Th___
    {
        ResourceState( src, EResourceState::CopyRTAS_Read );
        ResourceState( dst, EResourceState::CopyRTAS_Write );
        _ctx.CommitBarriers();
        _ctx.SerializeToMemory( src, dst, dstOffset );
    }

    template <typename C>
    void  ASBuildContext<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst) __Th___
    {
        ResourceState( src, EResourceState::CopyRTAS_Read );
        ResourceState( dst, EResourceState::CopyRTAS_Write );
        _ctx.CommitBarriers();
        _ctx.DeserializeFromMemory( src, srcOffset, dst );
    }

    template <typename C>
    void  ASBuildContext<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst) __Th___
    {
        ResourceState( src, EResourceState::CopyRTAS_Read );
        ResourceState( dst, EResourceState::CopyRTAS_Write );
        _ctx.CommitBarriers();
        _ctx.DeserializeFromMemory( src, srcOffset, dst );
    }
#endif
//-----------------------------------------------------------------------------


} // AE::RG::_hidden_
