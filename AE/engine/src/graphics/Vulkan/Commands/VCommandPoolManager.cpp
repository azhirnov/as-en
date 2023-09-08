// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    ref:
        https://github.com/KhronosGroup/Vulkan-Samples/tree/main/samples/performance/command_buffer_usage
*/

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{

/*
=================================================
    VPrimaryCmdBufState::operator ==
=================================================
*/
    bool  VPrimaryCmdBufState::operator == (const VPrimaryCmdBufState &rhs) C_NE___
    {
        // ignore 'userData'
        return  (renderPass         == rhs.renderPass)          &
                (framebuffer        == rhs.framebuffer)         &
                (frameId            == rhs.frameId)             &
                (subpassIndex       == rhs.subpassIndex)        &
                (hasViewLocalDeps   == rhs.hasViewLocalDeps)    &
                (useSecondaryCmdbuf == rhs.useSecondaryCmdbuf);
    }

/*
=================================================
    VPrimaryCmdBufState::LayerCount
=================================================
*/
    uint  VPrimaryCmdBufState::LayerCount () C_NE___
    {
        return framebuffer->LayerCount();
    }
//-----------------------------------------------------------------------------


/*
=================================================
    CmdPoolGuard::try_lock
=================================================
*/
    inline bool  _hidden_::CmdPoolUtils::CmdPoolGuard::try_lock () __NE___
    {
        CHECK_ERR( IsValid() );

        auto*   pool    = _Ptr();
        auto    bit     = (1ull << _BitIndex());
        auto    old     = pool->assignedBits.fetch_or( bit );   // 0 -> 1

        return (old & bit) == 0;
    }

/*
=================================================
    CmdPoolGuard::unlock
=================================================
*/
    inline bool  _hidden_::CmdPoolUtils::CmdPoolGuard::unlock () __NE___
    {
        if_likely( IsValid() )
        {
            auto*   pool    = _Ptr();
            auto    bit     = (1ull << _BitIndex());
            auto    old     = pool->assignedBits.fetch_and( ~bit ); // 1 -> 0

            ASSERT( (old & bit) != 0 );
            Unused( old );

            _ptr = UMax;
            return true;
        }
        return false;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    VCommandBuffer::VCommandBuffer (VkCommandBuffer cmdbuf, EQueueType queueType, ECommandBufferType cmdType, CmdPoolGuard guard) __NE___ :
        _cmdbuf{ cmdbuf },
        _queueType{ queueType },
        _cmdType{ cmdType },
        _recording{ true },
        _guard{ RVRef(guard) }
    {
        ASSERT( _cmdbuf != Default );
        ASSERT( _guard.IsLocked() );
    }

    VCommandBuffer::VCommandBuffer (VCommandBuffer && other) __NE___ :
        _cmdbuf{ other._cmdbuf },
        _queueType{ other._queueType },
        _cmdType{ other._cmdType },
        _recording{ other._recording },
        _guard{ RVRef(other._guard) }
    {
        other._cmdbuf       = Default;
        other._recording    = false;
        ASSERT( not other._guard.IsValid() );
    }

/*
=================================================
    destructor
=================================================
*/
    VCommandBuffer::~VCommandBuffer () __NE___
    {
        Unused( EndAndRelease() );
        ASSERT( not _guard.IsValid() );
    }

/*
=================================================
    EndAndRelease
=================================================
*/
    bool  VCommandBuffer::EndAndRelease () __NE___
    {
        if ( _guard )
        {
            ASSERT( _IsInCurrentThread() );

            ASSERT( _cmdbuf != Default );

            if_unlikely( _recording )
            {
                auto&   dev = RenderTaskScheduler().GetDevice();

                _recording = false;
                VK_CHECK_ERR( dev.vkEndCommandBuffer( _cmdbuf ));
            }

            _cmdbuf = Default;
            CHECK( _guard.unlock() );
        }

        ASSERT( not _recording );
        ASSERT( _cmdbuf == Default );

        return true;
    }

/*
=================================================
    operator =
=================================================
*/
    VCommandBuffer&  VCommandBuffer::operator = (VCommandBuffer && rhs) __NE___
    {
        Unused( EndAndRelease() );

        _cmdbuf     = rhs._cmdbuf;
        _queueType  = rhs._queueType;
        _cmdType    = rhs._cmdType;
        _recording  = rhs._recording;
        _guard      = RVRef(rhs._guard);

        rhs._cmdbuf     = Default;
        rhs._recording  = false;
        ASSERT( not rhs._guard.IsValid() );

        DRC_ONLY( _stCheck.Reset() );

        return *this;
    }

/*
=================================================
    GetQueue
=================================================
*/
    VQueuePtr  VCommandBuffer::GetQueue () C_NE___
    {
        return RenderTaskScheduler().GetDevice().GetQueue( _queueType );
    }

/*
=================================================
    DebugMarker
=================================================
*/
    void  VCommandBuffer::DebugMarker (VulkanDeviceFn fn, NtStringView label, RGBA8u color) __NE___
    {
        ASSERT( not label.empty() );
        ASSERT( IsValid() );
        ASSERT( _IsInCurrentThread() );

        VkDebugUtilsLabelEXT    info = {};
        info.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        info.pLabelName = label.c_str();
        MemCopy( OUT info.color, RGBA32f{color} );

        fn.vkCmdInsertDebugUtilsLabelEXT( _cmdbuf, &info );
    }

/*
=================================================
    PushDebugGroup
=================================================
*/
    void  VCommandBuffer::PushDebugGroup (VulkanDeviceFn fn, NtStringView label, RGBA8u color) __NE___
    {
        ASSERT( not label.empty() );
        ASSERT( IsValid() );
        ASSERT( _IsInCurrentThread() );

        VkDebugUtilsLabelEXT    info = {};
        info.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        info.pLabelName = label.c_str();
        MemCopy( OUT info.color, RGBA32f{color} );

        fn.vkCmdBeginDebugUtilsLabelEXT( _cmdbuf, &info );
    }

/*
=================================================
    PopDebugGroup
=================================================
*/
    void  VCommandBuffer::PopDebugGroup (VulkanDeviceFn fn) __NE___
    {
        ASSERT( IsValid() );
        ASSERT( _IsInCurrentThread() );

        fn.vkCmdEndDebugUtilsLabelEXT( _cmdbuf );
    }

/*
=================================================
    SetDebugName
=================================================
*/
    void  VCommandBuffer::SetDebugName (NtStringView label) __NE___
    {
    #if AE_DBG_GRAPHICS
        RenderTaskScheduler().GetDevice().SetObjectName( _cmdbuf, label, VK_OBJECT_TYPE_COMMAND_BUFFER );
    #else
        Unused( label );
    #endif
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    VCommandPoolManager::VCommandPoolManager (const VDevice &dev) __NE___ :
        _device{ dev }
    {
        VulkanDeviceFn_Init( _device );

        for (auto& frame : _perFrame)
        {
            for (uint q = 0; q < uint(EQueueType::_Count); ++q)
            {
                frame[q].queue = _device.GetQueue( EQueueType(q) );
            }
        }
    }

/*
=================================================
    destructor
=================================================
*/
    VCommandPoolManager::~VCommandPoolManager () __NE___
    {
        for (uint i = 0; i < GraphicsConfig::MaxFrames; ++i)
        {
            CHECK( ReleaseResources( FrameUID::FromIndex( i, GraphicsConfig::MaxFrames )));
        }
    }

/*
=================================================
    NextFrame
=================================================
*/
    bool  VCommandPoolManager::NextFrame (FrameUID frameId) __NE___
    {
        _frameId = frameId;

        auto&   per_frame_queues = _perFrame[ _frameId.Index() ];

        for (auto& queue : per_frame_queues)
        {
            const uint  pool_count = Min( _PoolCount, queue.poolCount.load() );

            for (uint i = 0; i < pool_count; ++i)
            {
                // reset command pool and free command buffers
                CmdPoolGuard    guard   { queue, i };
                DeferExLock     lock    { guard };
                CHECK_ERR( lock.try_lock() );

                auto&       pool        = guard.GetPool();
                const uint  cmd_count   = Min( _CmdCount, pool.count.exchange( 0 ));

                if_unlikely( cmd_count == 0 )
                    continue;

                vkFreeCommandBuffers( _device.GetVkDevice(), pool.handle, cmd_count, pool.buffers.data() ); // TODO: free or reset ?

                // VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT specifies that resetting a command pool recycles all of the resources from the command pool back to the system.
                VK_CHECK_ERR( vkResetCommandPool( _device.GetVkDevice(), pool.handle, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT ));
            }

            ASSERT( queue.poolCount.load() == pool_count );
        }
        return true;
    }

/*
=================================================
    ReleaseResources
=================================================
*/
    bool  VCommandPoolManager::ReleaseResources (FrameUID frameId) __NE___
    {
        auto&   per_frame_queues = _perFrame[ frameId.Index() ];

        for (auto& queue : per_frame_queues)
        {
            const uint  pool_count = Min( _PoolCount, queue.poolCount.exchange( 0 ));

            for (uint i = 0; i < pool_count; ++i)
            {
                // free command buffers and destroy pool
                CmdPoolGuard    guard   { queue, i };
                DeferExLock     lock    { guard };

                CHECK_ERR( lock.try_lock() );

                auto&       pool        = guard.GetPool();
                const uint  cmd_count   = Min( _CmdCount, pool.count.exchange( 0 ));

                if ( cmd_count > 0 )
                    vkFreeCommandBuffers( _device.GetVkDevice(), pool.handle, cmd_count, pool.buffers.data() );

                vkDestroyCommandPool( _device.GetVkDevice(), pool.handle, null );
                pool.handle = Default;
            }

            ASSERT( queue.poolCount.load() == 0 );
        }
        return true;
    }

/*
=================================================
    GetCommandBuffer
=================================================
*/
    VCommandBuffer  VCommandPoolManager::GetCommandBuffer (EQueueType queueType, ECommandBufferType cmdType, const VPrimaryCmdBufState *primaryState) __NE___
    {
        auto&   per_frame_queues    = _perFrame[ _frameId.Index() ];
        auto&   queue               = per_frame_queues[ uint(queueType) ];
        uint    pool_idx            = 0;

        CHECK_ERR( queue.queue );

        const auto  CreateCmdBuf = [this, queueType, cmdType, primaryState] (auto& pool, const uint cmdIndex, DeferExLock<CmdPoolGuard> &lock) -> VCommandBuffer
        {{
            VkCommandBufferAllocateInfo info = {};
            info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandPool        = pool.handle;
            info.commandBufferCount = 1;

            VkCommandBufferBeginInfo    begin = {};
            begin.sType             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            VkCommandBufferInheritanceInfo  inheritance_info = {};
            inheritance_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

            BEGIN_ENUM_CHECKS();
            switch ( cmdType )
            {
                case ECommandBufferType::Primary_OneTimeSubmit :
                    info.level  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                    CHECK_ERR( primaryState == null );
                    break;

                case ECommandBufferType::Secondary_RenderCommands :
                    info.level  = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
                    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
                    begin.pInheritanceInfo = &inheritance_info;

                    CHECK_ERR( primaryState != null );
                    CHECK_ERR( primaryState->IsValid() );

                    inheritance_info.renderPass     = primaryState->renderPass->Handle();
                    inheritance_info.subpass        = primaryState->subpassIndex;
                    inheritance_info.framebuffer    = primaryState->framebuffer->Handle();  // can be null
                    //inheritance_info.occlusionQueryEnable = 
                    //inheritance_info.queryFlags           = 
                    //inheritance_info.pipelineStatistics   = 
                    break;

                case ECommandBufferType::_Count :
                case ECommandBufferType::Unknown :
                    RETURN_ERR( "unknown command buffer type" );
                    break;
            }
            END_ENUM_CHECKS();

            VkCommandBuffer cmd = Default;
            VK_CHECK_ERR( vkAllocateCommandBuffers( _device.GetVkDevice(), &info, OUT &cmd ));

            VK_CHECK_ERR( vkBeginCommandBuffer( cmd, &begin ));

            pool.buffers[cmdIndex] = cmd;
            return VCommandBuffer{ cmd, queueType, cmdType, RVRef(*lock.release()) };
        }};

        // find existing command pool
        for (uint i = 0; i < 4; ++i)
        {
            const uint  pool_count = Min( queue.poolCount.load(), _PoolCount );

            for (; pool_idx < pool_count; ++pool_idx)
            {
                CmdPoolGuard    guard   { queue, pool_idx };
                DeferExLock     lock    { guard };

                if ( not lock.try_lock() )  // TODO: optimize
                    continue;

                auto&   pool = guard.GetPool();

                if ( pool.handle == Default )
                    continue; // not created yet

                const uint  index = pool.count.fetch_add( 1 );

                if_likely( index < _CmdCount )
                    return CreateCmdBuf( pool, index, lock );
            }
        }

        // create new command pool
        {
            pool_idx = queue.poolCount.fetch_add( 1 );
            CHECK_ERR( pool_idx < _PoolCount );

            CmdPoolGuard    guard   { queue, pool_idx };
            DeferExLock     lock    { guard };
            CHECK_ERR( lock.try_lock() );

            auto&   pool = guard.GetPool();
            ASSERT( pool.handle == Default );

            VkCommandPoolCreateInfo info = {};
            info.sType              = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.queueFamilyIndex   = uint(queue.queue->familyIndex);
            info.flags              = 0;

            VK_CHECK_ERR( vkCreateCommandPool( _device.GetVkDevice(), &info, null, OUT &pool.handle ));

            const uint  index = pool.count.fetch_add( 1 );

            if_likely( index < _CmdCount )
                return CreateCmdBuf( pool, index, lock );
        }

        RETURN_ERR( "failed to allocate command buffer" );
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
