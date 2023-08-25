// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    VideoEncodeCtx  --> DirectVideoEncodeCtx   --> BarrierMngr --> Vulkan device 
                    \-> IndirectVideoEncodeCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"

namespace AE::Graphics::_hidden_
{

    //
    // Vulkan Direct Video Encode Context implementation
    //

    class _VDirectVideoEncodeCtx : public VBaseDirectContext
    {
    // methods
    public:
        void  Encode (const VkVideoEncodeInfoKHR &info)     __Th___;

        ND_ VkCommandBuffer EndCommandBuffer ()             __Th___;
        ND_ VCommandBuffer  ReleaseCommandBuffer ()         __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VDirectVideoEncodeCtx (const VkVideoBeginCodingInfoKHR &, const RenderTask &task,
                                VCommandBuffer cmdbuf, DebugLabel dbg)  __Th___;
    };



    //
    // Vulkan Indirect Video Encode Context implementation
    //

    class _VIndirectVideoEncodeCtx : public VBaseIndirectContext
    {
    // methods
    public:
        void  Encode (const VkVideoEncodeInfoKHR &info)     __Th___;

        ND_ VBakedCommands      EndCommandBuffer ()         __Th___;
        ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()     __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VIndirectVideoEncodeCtx (const VkVideoBeginCodingInfoKHR &, const RenderTask &task,
                                  VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)    __Th___;
    };



    //
    // Vulkan Video Encode Context implementation
    //

    template <typename CtxImpl>
    class _VVideoEncodeContextImpl : public CtxImpl, public IASBuildContext
    {
    // types
    public:
        static constexpr bool   IsVideoEncodeContext        = true;
        static constexpr bool   IsVulkanVideoEncodeContext  = true;

        using CmdBuf_t      = typename CtxImpl::CmdBuf_t;
    private:
        using RawCtx        = CtxImpl;
        using AccumBar      = VAccumBarriers< _VVideoEncodeContextImpl< CtxImpl >>;
        using DeferredBar   = VAccumDeferredBarriersForCtx< _VVideoEncodeContextImpl< CtxImpl >>;
        using Validator_t   = VideoEncodeContextValidation;


    // methods
    public:
        _VVideoEncodeContextImpl (VideoSessionID, const RenderTask &task,
                                  CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)  __Th___;

        _VVideoEncodeContextImpl ()                                                     = delete;
        _VVideoEncodeContextImpl (const _VVideoEncodeContextImpl &)                     = delete;

        void  Encode (const VideoEncodeCmd &)                                           __Th___;

        VBARRIERMNGR_INHERIT_BARRIERS
    };

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
    using VDirectVideoEncodeContext     = Graphics::_hidden_::_VVideoEncodeContextImpl< Graphics::_hidden_::_VDirectVideoEncodeCtx >;
    using VIndirectVideoEncodeContext   = Graphics::_hidden_::_VVideoEncodeContextImpl< Graphics::_hidden_::_VIndirectVideoEncodeCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{

/*
=================================================
    constructor
=================================================
*/
    template <typename C>
    _VVideoEncodeContextImpl<C>::_VVideoEncodeContextImpl (VideoSessionID sessionId, const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
        RawCtx{ task, RVRef(cmdbuf), dbg }
    {
        Validator_t::CtxInit( task.GetQueueMask() );

        //auto& session = _GetResourcesOrThrow( sessionId );
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
