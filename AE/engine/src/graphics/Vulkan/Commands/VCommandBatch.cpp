// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VCommandBatch.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{
#   include "graphics/Private/CommandBatch.cpp.h"

/*
=================================================
    GetCommands
----
    not thread safe !!!
=================================================
*/
    void  VCommandBatch::CmdBufPool::GetCommands (VkCommandBufferSubmitInfoKHR* cmdbufs, OUT uint &cmdbufCount, uint maxCount) __NE___
    {
        ASSERT( cmdbufs != null );
        ASSERT( maxCount >= GraphicsConfig::MaxCmdBufPerBatch );
        Unused( maxCount );

        cmdbufCount = 0;

        DRC_EXLOCK( _drCheck );
        MemoryBarrier( EMemoryOrder::Acquire );

        ASSERT( _cmdTypes.load() == 0 );    // software command buffers is not supported here
        ASSERT( IsReady() );

        for (uint i = 0, cnt = _count.load(); i < cnt; ++i)
        {
            // command buffer can be null
            if_likely( _pool[i].native != Default )
            {
                ASSERT( cmdbufCount < maxCount );
                auto&   dst = cmdbufs[cmdbufCount++];

                dst.sType           = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
                dst.pNext           = null;
                dst.commandBuffer   = _pool[i].native;
                dst.deviceMask      = 0;
            }
        }

        DEBUG_ONLY( Reset() );
    }

/*
=================================================
    CommitIndirectBuffers
----
    not thread safe !!!
=================================================
*/
    bool  VCommandBatch::CmdBufPool::CommitIndirectBuffers (VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType,
                                                            const VPrimaryCmdBufState* primaryState) __NE___
    {
        uint    cmd_types = _cmdTypes.load();
        if ( cmd_types == 0 )
            return true;

        DRC_EXLOCK( _drCheck );
        MemoryBarrier( EMemoryOrder::Acquire );

        CHECK_ERR( _CommitIndirectBuffers_Ordered( cmd_types, cmdPoolMngr, queue, cmdbufType, primaryState ));

        // flush changes in '_pool[].native'
        _cmdTypes.store( 0, EMemoryOrder::Release );
        return true;
    }

/*
=================================================
    _CommitIndirectBuffers_Ordered
=================================================
*/
    bool  VCommandBatch::CmdBufPool::_CommitIndirectBuffers_Ordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue,
                                                                     ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState)
    {
        auto&           dev     = cmdPoolMngr.GetDevice();
        VCommandBuffer  cmdbuf;
        uint            prev_idx = UMax;

        for (uint i : BitIndexIterate( cmdTypes ))
        {
            // recreate command buffer if indirect command buffers placed with a gap
            if ( cmdbuf.IsValid() and prev_idx+1 != i )
            {
                _pool[prev_idx].native = cmdbuf.Get();
                CHECK_ERR( cmdbuf.EndAndRelease() );
            }

            if ( not cmdbuf.IsValid() )
            {
                cmdbuf = cmdPoolMngr.GetCommandBuffer( queue, cmdbufType, primaryState );
                CHECK_ERR( cmdbuf.IsValid() );
                cmdbuf.SetDebugName( "IndirectCmdBuffers" );
            }

            auto&   item = _pool[i];
            CHECK_ERR( item.baked.Execute( dev, cmdbuf.Get() ));
            item.baked.Destroy();

            prev_idx = i;
        }

        if_likely( prev_idx < _pool.size() )
        {
            _pool[prev_idx].native = cmdbuf.Get();
            CHECK_ERR( cmdbuf.EndAndRelease() );
        }
        return true;
    }

/*
=================================================
    _CommitIndirectBuffers_Unordered
=================================================
*/
    bool  VCommandBatch::CmdBufPool::_CommitIndirectBuffers_Unordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue,
                                                                     ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState)
    {
        auto&           dev     = cmdPoolMngr.GetDevice();
        VCommandBuffer  cmdbuf;
        uint            last_idx = UMax;

        for (uint i : BitIndexIterate( cmdTypes ))
        {
            if_unlikely( not cmdbuf.IsValid() )
            {
                cmdbuf = cmdPoolMngr.GetCommandBuffer( queue, cmdbufType, primaryState );
                CHECK_ERR( cmdbuf.IsValid() );
                cmdbuf.SetDebugName( "IndirectCmdBuffers" );
            }

            auto&   item = _pool[i];
            CHECK_ERR( item.baked.Execute( dev, cmdbuf.Get() ));
            item.baked.Destroy();

            last_idx = i;
        }

        if_likely( last_idx < _pool.size() )
        {
            _pool[last_idx].native = cmdbuf.Get();
            CHECK_ERR( cmdbuf.EndAndRelease() );
        }
        return true;
    }
//-----------------------------------------------------------------------------



#if not AE_VK_TIMELINE_SEMAPHORE
/*
=================================================
    destructor
=================================================
*/
    VCommandBatch::VirtualFence::~VirtualFence () __NE___
    {
        if ( _fence != Default )
        {
            auto&   dev = GraphicsScheduler().GetDevice();

            dev.vkDestroyFence( dev.GetVkDevice(), _fence, null );
        }
    }

/*
=================================================
    Create
=================================================
*/
    bool  VCommandBatch::VirtualFence::Create (const VDevice &dev) __NE___
    {
        _complete.store( false );

        if_likely( _fence != Default )
        {
            VK_CHECK_ERR( dev.vkResetFences( dev.GetVkDevice(), 1, &_fence ));
        }
        else
        {
            VkFenceCreateInfo   info = {};
            info.sType  = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

            VK_CHECK_ERR( dev.vkCreateFence( dev.GetVkDevice(), &info, null, OUT &_fence ));
        }
        return true;
    }

/*
=================================================
    _ReleaseObject
=================================================
*/
    void  VCommandBatch::VirtualFence::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );

        ASSERT( _fence == Default or _complete.load() );

        RenderTaskScheduler::VirtualFenceApi::Recycle( this );
    }

/*
=================================================
    IsCompleted
=================================================
*/
    bool  VCommandBatch::VirtualFence::IsCompleted (const VDevice &dev) __NE___
    {
        if_likely( _complete.load() )
            return true;

        VkResult    res = dev.vkGetFenceStatus( dev.GetVkDevice(), _fence );

        if_likely( res == VK_SUCCESS )
        {
            _complete.store( true );
            return true;
        }

        if ( res != VK_NOT_READY )
            VK_CHECK( res );

        return false;
    }

/*
=================================================
    Wait
=================================================
*/
    bool  VCommandBatch::VirtualFence::Wait (const VDevice &dev, nanoseconds timeout) __NE___
    {
        if_likely( _complete.load() )
            return true;

        VkResult    res = dev.vkWaitForFences( dev.GetVkDevice(), 1, &_fence, VK_TRUE, timeout.count() );

        if_likely( res == VK_SUCCESS )
        {
            _complete.store( true );
            return true;
        }

        return false;
    }
#endif
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    VCommandBatch::VCommandBatch () __NE___
    {
        #if AE_VK_TIMELINE_SEMAPHORE
        {
            auto&   dev = GraphicsScheduler().GetDevice();

            VkSemaphoreCreateInfo       sem_ci      = {};
            VkSemaphoreTypeCreateInfo   sem_type_ci = {};

            sem_type_ci.sType           = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
            sem_type_ci.semaphoreType   = VK_SEMAPHORE_TYPE_TIMELINE;
            sem_type_ci.initialValue    = _tlSemaphoreVal.load();

            sem_ci.sType    = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            sem_ci.pNext    = &sem_type_ci;

            VK_CHECK( dev.vkCreateSemaphore( dev.GetVkDevice(), &sem_ci, null, OUT &_tlSemaphore ));

            ++_tlSemaphoreVal;
        }
        #endif
    }

/*
=================================================
    destructor
=================================================
*/
    VCommandBatch::~VCommandBatch () __NE___
    {
        #if AE_VK_TIMELINE_SEMAPHORE
            auto&   dev = GraphicsScheduler().GetDevice();
            dev.vkDestroySemaphore( dev.GetVkDevice(), _tlSemaphore, null );
        #endif
    }

/*
=================================================
    _Create
=================================================
*/
    bool  VCommandBatch::_Create (EQueueType queue, FrameUID frameId, uint submitIdx, DebugLabel dbg, void* userData) __NE___
    {
        #if AE_VK_TIMELINE_SEMAPHORE
            CHECK_ERR( _tlSemaphore != Default );
        #else
            CHECK_ERR( _fence == null );
        #endif

        _submitMode = ESubmitMode::Immediately;
        _queueType  = queue;
        _frameId    = frameId;
        _submitIdx  = CheckCast<ubyte>(submitIdx);
        _userData   = userData;

        _perTaskBarriers.fill( null );

        DBG_GRAPHICS_ONLY(
            _profiler = GraphicsScheduler().GetProfiler();
            if ( _profiler )
                _profiler->BeginBatch( frameId, this, dbg.label );

            _dbgName    = dbg.label;
            _dbgColor   = dbg.color;
        )
        Unused( dbg );

        #if defined(AE_DEBUG) and AE_VK_TIMELINE_SEMAPHORE
            GraphicsScheduler().GetDevice().SetObjectName( _tlSemaphore, _dbgName, VK_OBJECT_TYPE_SEMAPHORE );
        #endif

        CHECK_ERR( _status.exchange( EStatus::Initial ) == EStatus::Destroyed );
        return true;
    }

/*
=================================================
    Wait
=================================================
*/
    bool  VCommandBatch::Wait (nanoseconds timeout) __NE___
    {
        if_likely( IsCompleted() )
            return true;

        auto&   dev = GraphicsScheduler().GetDevice();

        #if AE_VK_TIMELINE_SEMAPHORE
        {
            CHECK_ERR( _tlSemaphore != Default );

            const ulong             val  = _tlSemaphoreVal.load();
            VkSemaphoreWaitInfo     info = {};
            info.sType              = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
            info.semaphoreCount     = 1;
            info.pSemaphores        = &_tlSemaphore;
            info.pValues            = &val;

            VkResult    res = dev.vkWaitSemaphoresKHR( dev.GetVkDevice(), &info, timeout.count() );
            return (res == VK_SUCCESS);
        }
        #else
        {
            return _fence == null or _fence->Wait( dev, timeout );
        }
        #endif
    }

/*
=================================================
    AddInputSemaphore
=================================================
*/
    bool  VCommandBatch::AddInputSemaphore (VkSemaphore sem, ulong value) __NE___
    {
        #if not AE_VK_TIMELINE_SEMAPHORE
        ASSERT( value == 0 );
        #endif
        CHECK_ERR( sem != Default );
        CHECK_ERR( not IsSubmitted() );

        EXLOCK( _gpuInDepsGuard );
        CHECK_ERR( _gpuInDeps.size() < _gpuInDeps.capacity() ); // check for overflow

        auto&   val = _gpuInDeps.emplace( sem, value ).first->second;
        val = Min( val, value );

        return true;
    }

    bool  VCommandBatch::AddInputSemaphore (const VulkanCmdBatchDependency &dep) __NE___
    {
        return AddInputSemaphore( dep.semaphore, dep.value );
    }

/*
=================================================
    AddInputDependency
----
    GPU to GPU dependency.
    returns 'false' on overflow or if batch is already completed.
=================================================
*/
    bool  VCommandBatch::AddInputDependency (RC<VCommandBatch> batch) __NE___
    {
        if_unlikely( not batch )
            return true;

        return AddInputDependency( *batch );
    }

    bool  VCommandBatch::AddInputDependency (const VCommandBatch &batch) __NE___
    {
        CHECK_ERR( not IsSubmitted() );

        // different queues
        #if AE_VK_TIMELINE_SEMAPHORE
        if_likely( batch.GetQueueType() != GetQueueType() )
            return AddInputSemaphore( batch.GetSemaphore() );
        #endif

        // same queue
        CHECK_ERR( batch.GetQueueType() == GetQueueType() );
        CHECK_ERR( batch.GetSubmitIndex() < GetSubmitIndex() );

        return true;
    }

/*
=================================================
    AddOutputSemaphore
=================================================
*/
    bool  VCommandBatch::AddOutputSemaphore (VkSemaphore sem, ulong value) __NE___
    {
        #if not AE_VK_TIMELINE_SEMAPHORE
        ASSERT( value == 0 );
        #endif
        CHECK_ERR( sem != Default );
        CHECK_ERR( not IsSubmitted() );

        EXLOCK( _gpuOutDepsGuard );
        CHECK_ERR( _gpuOutDeps.size() < _gpuOutDeps.capacity() );   // check for overflow

        auto&   val = _gpuOutDeps.emplace( sem, value ).first->second;
        val = Min( val, value );

        return true;
    }

    bool  VCommandBatch::AddOutputSemaphore (const VulkanCmdBatchDependency &dep) __NE___
    {
        return AddOutputSemaphore( dep.semaphore, dep.value );
    }

/*
=================================================
    GetSemaphore
=================================================
*/
    VulkanCmdBatchDependency  VCommandBatch::GetSemaphore () C_NE___
    {
    #if AE_VK_TIMELINE_SEMAPHORE
        return { _tlSemaphore, _tlSemaphoreVal.load() };
    #else
        return {};
    #endif
    }

/*
=================================================
    _OnSubmit
=================================================
*/
#if AE_VK_TIMELINE_SEMAPHORE
    void  VCommandBatch::_OnSubmit () __NE___
    {
        _OnSubmit2();
    }
#else
    void  VCommandBatch::_OnSubmit (RC<VirtualFence> fence) __NE___
    {
        _OnSubmit2();
        _fence = RVRef(fence);
    }
#endif

/*
=================================================
    _GetWaitSemaphores
=================================================
*/
#if AE_VK_TIMELINE_SEMAPHORE
    bool  VCommandBatch::_GetWaitSemaphores (VTempStackAllocator &allocator, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count) __NE___
    {
        semInfos = null;
        count    = 0;

        EXLOCK( _gpuInDepsGuard );
        if_likely( _gpuInDeps.empty() )
            return true;

        auto*   sem_infos = allocator.Allocate<VkSemaphoreSubmitInfoKHR>( _gpuInDeps.size() );
        CHECK_ERR( sem_infos != null );
        semInfos = sem_infos;

        const auto* sems    = _gpuInDeps.GetKeyArray().data();
        const auto* vals    = _gpuInDeps.GetValueArray().data();

        for (usize i = 0; i < _gpuInDeps.size(); ++i)
        {
            auto&   dst     = sem_infos [count++];
            dst.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
            dst.pNext       = null;
            dst.semaphore   = sems[i];
            dst.value       = vals[i];
            dst.stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;     // TODO: optimize
            dst.deviceIndex = 0;
        }

        _gpuInDeps.clear();
        return true;
    }
#endif

/*
=================================================
    _GetSignalSemaphores
=================================================
*/
#if AE_VK_TIMELINE_SEMAPHORE
    bool  VCommandBatch::_GetSignalSemaphores (VTempStackAllocator &allocator, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count) __NE___
    {
        count = 1;

        EXLOCK( _gpuOutDepsGuard );

        CHECK_ERR( _tlSemaphore != Default );

        auto*   sem_infos = allocator.Allocate<VkSemaphoreSubmitInfoKHR>( _gpuOutDeps.size() + 1 );
        CHECK_ERR( sem_infos != null );
        semInfos = sem_infos;

        sem_infos[0].sType      = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
        sem_infos[0].pNext      = null;
        sem_infos[0].semaphore  = _tlSemaphore;
        sem_infos[0].value      = _tlSemaphoreVal.load();
        sem_infos[0].stageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;     // TODO: optimize
        sem_infos[0].deviceIndex= 0;

        if_likely( _gpuOutDeps.empty() )
            return true;

        const auto* sems    = _gpuOutDeps.GetKeyArray().data();
        const auto* vals    = _gpuOutDeps.GetValueArray().data();

        for (usize i = 0; i < _gpuOutDeps.size(); ++i)
        {
            auto&   dst     = sem_infos [count++];
            dst.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
            dst.pNext       = null;
            dst.semaphore   = sems[i];
            dst.value       = vals[i];
            dst.stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;     // TODO: optimize
            dst.deviceIndex = 0;
        }

        _gpuOutDeps.clear();
        return true;
    }
#endif

/*
=================================================
    _GetWaitSemaphores
=================================================
*/
#if not AE_VK_TIMELINE_SEMAPHORE
    bool  VCommandBatch::_GetWaitSemaphores (VTempStackAllocator &allocator, OUT VkSemaphore const* &outSems, OUT VkPipelineStageFlags const* &outStages, OUT uint &count) __NE___
    {
        outSems     = null;
        outStages   = null;
        count       = 0;

        EXLOCK( _gpuInDepsGuard );
        if_likely( _gpuInDeps.empty() )
            return true;

        auto*   out_sems    = allocator.Allocate<VkSemaphore>( _gpuInDeps.size() );
        auto*   out_stages  = allocator.Allocate<VkPipelineStageFlags>( _gpuInDeps.size() );
        CHECK_ERR( out_sems != null and out_stages != null );
        outSems     = out_sems;
        outStages   = out_stages;

        const auto* in_sems = _gpuInDeps.GetKeyArray().data();
        memcpy( OUT out_sems, in_sems, sizeof(VkSemaphore) * _gpuInDeps.size() );

        for (usize i = 0; i < _gpuInDeps.size(); ++i, ++count)
            out_stages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // TODO: optimize

        _gpuInDeps.clear();
        return true;
    }
#endif

/*
=================================================
    _GetSignalSemaphores
=================================================
*/
#if not AE_VK_TIMELINE_SEMAPHORE
    bool  VCommandBatch::_GetSignalSemaphores (VTempStackAllocator &allocator, OUT VkSemaphore const* &outSems, OUT uint &count) __NE___
    {
        outSems = null;
        count   = 0;

        EXLOCK( _gpuOutDepsGuard );
        if_likely( _gpuOutDeps.empty() )
            return true;

        auto*   out_sems = allocator.Allocate<VkSemaphore>( _gpuOutDeps.size() );
        CHECK_ERR( out_sems != null );
        outSems = out_sems;

        const auto* in_sems = _gpuOutDeps.GetKeyArray().data();
        memcpy( OUT out_sems, in_sems, sizeof(VkSemaphore) * _gpuOutDeps.size() );

        count = uint(_gpuOutDeps.size());

        _gpuOutDeps.clear();
        return true;
    }
#endif


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
