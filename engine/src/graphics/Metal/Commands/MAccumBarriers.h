// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics::_hidden_
{

    //
    // Accumulate Context Barriers
    //

    template <typename Ctx>
    class MAccumBarriers final
    {
        friend Ctx;

    // types
    private:
        using Self  = MAccumBarriers< Ctx >;

    // variables
    private:
        Ctx &   _ctx;

    // methods
    private:
        MAccumBarriers (Ctx &ctx)       __NE___ : _ctx{ctx} {}

    public:
        MAccumBarriers ()                       = delete;
        MAccumBarriers (const Self &)           = delete;
        MAccumBarriers (Self &&)        __NE___ = default;
        ~MAccumBarriers ()              __Th___ { _ctx.CommitBarriers(); }

        Self&&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)                                   rvTh___ { _ctx.BufferBarrier( buffer, srcState, dstState );         return RVRef(*this); }

        Self&&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)                                      rvTh___ { _ctx.ImageBarrier( image, srcState, dstState );           return RVRef(*this); }
        Self&&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes) rvTh___ { _ctx.ImageBarrier( image, srcState, dstState, subRes );   return RVRef(*this); }

        Self&&  MemoryBarrier (EResourceState srcState, EResourceState dstState)                                                    rvTh___ { _ctx.MemoryBarrier( srcState, dstState );                 return RVRef(*this); }
        Self&&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)                                                    rvTh___ { _ctx.MemoryBarrier( srcScope, dstScope );                 return RVRef(*this); }
        Self&&  MemoryBarrier ()                                                                                                    rvTh___ { _ctx.MemoryBarrier();                                     return RVRef(*this); }

        Self&&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)                                                 rvTh___ { _ctx.ExecutionBarrier( srcScope, dstScope );              return RVRef(*this); }
        Self&&  ExecutionBarrier ()                                                                                                 rvTh___ { _ctx.ExecutionBarrier();                                  return RVRef(*this); }

        Self&&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)     rvTh___ { _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return RVRef(*this); }
        Self&&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)     rvTh___ { _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return RVRef(*this); }

        Self&&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)        rvTh___ { _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); return RVRef(*this); }
        Self&&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)        rvTh___ { _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); return RVRef(*this); }
    };



    //
    // Accumulate Draw Context Barriers
    //

    template <typename Ctx>
    class MAccumDrawBarriers final
    {
        friend Ctx;

    // types
    private:
        using Self  = MAccumDrawBarriers< Ctx >;

    // variables
    private:
        Ctx &   _ctx;

    // methods
    private:
        MAccumDrawBarriers (Ctx &ctx)       __NE___: _ctx{ctx} {}

    public:
        MAccumDrawBarriers ()                       = delete;
        MAccumDrawBarriers (const Self &)           = delete;
        MAccumDrawBarriers (Self &&)        __NE___ = default;
        ~MAccumDrawBarriers ()              __Th___ { _ctx.CommitBarriers(); }

        Self&&  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState) rvTh___ { _ctx.AttachmentBarrier( name, srcState, dstState ); return RVRef(*this); }
    };


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
