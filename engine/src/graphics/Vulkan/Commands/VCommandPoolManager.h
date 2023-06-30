// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"

namespace AE::Graphics
{
namespace _hidden_
{
    struct CmdPoolUtils
    {
        using CmdBuffers_t = StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBuffersPerPool >;

        struct CmdPool
        {
            VkCommandPool       handle          = Default;
            Atomic<uint>        count           {0};
            CmdBuffers_t        buffers         {};
        };

        using CmdPools_t    = StaticArray< CmdPool, GraphicsConfig::MaxCmdPoolsPerQueue >;

        struct alignas(128) CmdPoolPerQueue
        {
            friend struct CmdPoolGuard;

            Atomic<ulong>       assignedBits    {0};    // access to command pool and command buffers must be synchronized
            Atomic<uint>        poolCount   {0};
            CmdPools_t          _pools;
            VQueuePtr           queue;
        };

        struct CmdPoolGuard
        {
        private:
            static constexpr usize  _Mask = CT_ToBitMask< usize, CT_CeilIntLog2< GraphicsConfig::MaxCmdBuffersPerPool >>;
            STATIC_ASSERT( _Mask <= alignof(CmdPoolPerQueue) );

            usize   _ptr    = UMax;

        public:
            CmdPoolGuard ()                                     __NE___ {}
            CmdPoolGuard (CmdPoolPerQueue &ref, usize idx)      __NE___ : _ptr{ usize(&ref) | idx } { ASSERT( _Ptr() == &ref );  ASSERT( idx == _BitIndex() ); }
            CmdPoolGuard (const CmdPoolGuard &)                 = delete;
            CmdPoolGuard (CmdPoolGuard && other)                __NE___ : _ptr{other._ptr} { other._ptr = UMax; }

            CmdPoolGuard&  operator = (const CmdPoolGuard&)     = delete;
            CmdPoolGuard&  operator = (CmdPoolGuard && rhs)     __NE___ { ASSERT( not IsLocked() );  _ptr = rhs._ptr;  rhs._ptr = UMax;  return *this; }

            ND_ bool            IsValid ()                      C_NE___ { return _ptr != UMax; }
            ND_ bool            IsLocked ()                     C_NE___ { return IsValid() and HasBit( _Ptr()->assignedBits.load(), _BitIndex() ); }

            ND_ bool            try_lock ()                     __NE___;
                bool            unlock ()                       __NE___;

            ND_ CmdPool &       GetPool ()                      __NE___ { ASSERT( IsLocked() );  return _Ptr()->_pools[ _BitIndex() ]; }

            ND_ explicit operator bool ()                       C_NE___ { return IsValid(); }

        private:
            ND_ uint                _BitIndex ()                C_NE___ { return _ptr & _Mask; }
            ND_ CmdPoolPerQueue*    _Ptr ()                     C_NE___ { return BitCast< CmdPoolPerQueue *>( _ptr & ~_Mask ); }
        };
    };

} // _hidden_



    //
    // Vulkan Primary Command buffer State
    //

    struct VPrimaryCmdBufState
    {
        Ptr<const VRenderPass>      renderPass;
        Ptr<const VFramebuffer>     framebuffer;
        FrameUID                    frameId;
        ulong                       subpassIndex        : 8;
        ulong                       hasViewLocalDeps    : 1;    // for multiview rendering
        ulong                       useSecondaryCmdbuf  : 1;
        void*                       userData;

        DEBUG_ONLY(
            RenderPassID            _rpId;
            VFramebufferID          _fbId;
        )

        VPrimaryCmdBufState ()                                  __NE___ :
            subpassIndex{0xFF}, hasViewLocalDeps{false}, useSecondaryCmdbuf{false}, userData{null}
        {}

        ND_ bool  IsValid ()                                    C_NE___ { return (renderPass != null) & (framebuffer != null) & frameId.IsValid(); }
        ND_ uint  LayerCount ()                                 C_NE___;

        ND_ bool  operator == (const VPrimaryCmdBufState &rhs)  C_NE___;
    };



    //
    // Vulkan Command Buffer
    //

    class VCommandBuffer
    {
        friend class VCommandPoolManager;

    // types
    private:
        using CmdPoolGuard  = _hidden_::CmdPoolUtils::CmdPoolGuard;


    // variables
    private:
        VkCommandBuffer     _cmdbuf         = Default;
        EQueueType          _queueType      = Default;
        ECommandBufferType  _cmdType        = Default;
        bool                _recording      = false;
        CmdPoolGuard        _guard;

        DRC_ONLY(
            Threading::SingleThreadCheck    _stCheck;
        )


    // methods
    protected:
        VCommandBuffer (VkCommandBuffer cmdbuf, EQueueType queueType, ECommandBufferType cmdType, CmdPoolGuard lock) __NE___;

        VCommandBuffer (const VCommandBuffer &)                 = delete;
        VCommandBuffer&  operator = (const VCommandBuffer &)    = delete;

    public:
        VCommandBuffer ()                                       __NE___ {}
        VCommandBuffer (VCommandBuffer &&)                      __NE___;
        ~VCommandBuffer ()                                      __NE___;

        VCommandBuffer&  operator = (VCommandBuffer && rhs)     __NE___;

        ND_ bool  EndAndRelease ()                              __NE___;

        ND_ EQueueType          GetQueueType ()                 C_NE___ { return _queueType; }
        ND_ VkCommandBuffer     Get ()                          C_NE___ { ASSERT( IsValid() );  ASSERT( _IsInCurrentThread() );  return _cmdbuf; }
        ND_ bool                IsValid ()                      C_NE___ { return _cmdbuf != Default; }
        ND_ bool                IsRecording ()                  C_NE___ { return _recording; }
        ND_ ECommandBufferType  GetCommandBufferType ()         C_NE___ { return _cmdType; }
        ND_ VQueuePtr           GetQueue ()                     C_NE___;

        void  DebugMarker (VulkanDeviceFn fn, NtStringView label, RGBA8u color)     __NE___;
        void  PushDebugGroup (VulkanDeviceFn fn, NtStringView label, RGBA8u color)  __NE___;
        void  PopDebugGroup (VulkanDeviceFn fn)                                     __NE___;

        void  SetDebugName (NtStringView label)                                     __NE___;

    private:
        DRC_ONLY( ND_ bool      _IsInCurrentThread ()           C_NE___ { return _stCheck.Lock(); })
    };



    //
    // Vulkan Command Pool Manager
    //

    class VCommandPoolManager final : private VulkanDeviceFn
    {
    // types
    private:
        static constexpr uint   _CmdCount   = GraphicsConfig::MaxCmdBuffersPerPool;
        static constexpr uint   _PoolCount  = GraphicsConfig::MaxCmdPoolsPerQueue;

        using CmdBuffers_t      = _hidden_::CmdPoolUtils::CmdBuffers_t;
        using CmdPool           = _hidden_::CmdPoolUtils::CmdPool;
        using CmdPoolGuard      = _hidden_::CmdPoolUtils::CmdPoolGuard;
        using CmdPoolPerQueue   = _hidden_::CmdPoolUtils::CmdPoolPerQueue;

        using Queues_t  = StaticArray< CmdPoolPerQueue, uint(EQueueType::_Count) >;
        using Frames_t  = StaticArray< Queues_t,        GraphicsConfig::MaxFrames >;


    // variables
    private:
        Frames_t            _perFrame;
        FrameUID            _frameId;
        VDevice const&      _device;


    // methods
    public:
        explicit VCommandPoolManager (const VDevice &dev)   __NE___;
        ~VCommandPoolManager ()                             __NE___;

        bool  NextFrame (FrameUID frameId)                  __NE___;
        bool  ReleaseResources (FrameUID frameId)           __NE___;

        ND_ VCommandBuffer  GetCommandBuffer (EQueueType queue, ECommandBufferType type, const VPrimaryCmdBufState *primaryState) __NE___;

        ND_ VDevice const&  GetDevice ()                    C_NE___ { return _device; }

        AE_GLOBALLY_ALLOC
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
