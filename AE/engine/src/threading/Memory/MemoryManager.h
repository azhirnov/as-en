// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Memory/LfLinearAllocator.h"
#include "threading/Memory/TsStackAllocator.h"
#include "threading/Memory/MemoryProfiler.h"

namespace AE::Threading { class MemoryManagerImpl; }
namespace AE { Threading::MemoryManagerImpl&  MemoryManager () __NE___; }

namespace AE::Threading
{

    //
    // Memory Manager
    //

    class MemoryManagerImpl final : public Noncopyable
    {
        friend struct InPlace<MemoryManagerImpl>;

    // types
    public:
        using GlobalLinearAllocator_t   = LfLinearAllocator< 16u<<20, AE_CACHE_LINE, 32 >;
        using FrameAllocator_t          = LfLinearAllocator< 4u<<20, AE_CACHE_LINE, 8 >;

        class TaskSchedulerApi {
            friend class TaskScheduler;
            static void  CreateInstance ()  __NE___;
            static void  DestroyInstance () __NE___;
        };


    private:
        using DefaultAlloc_t    = AllocatorImpl< UntypedAllocator >;

        static constexpr uint   _MaxFrames = FrameUID::MaxFramesLimit();


        //
        // Frame Alloc
        //
        struct FrameAlloc
        {
        // variables
        private:
            Atomic<uint>        _idx    {0};                // TODO: remove
            FrameAllocator_t    _alloc  [_MaxFrames];

            DEBUG_ONLY(
                AtomicFrameUID  _dbgFrameId;
            )   

        // methods
        public:
            FrameAlloc ()                                   __NE___ {}

                void  BeginFrame (FrameUID frameId)         __NE___;
                void  EndFrame (FrameUID frameId)           __NE___;

            ND_ FrameAllocator_t&  Get ()                   __NE___ { return _alloc[ _idx.load() ]; }
            ND_ FrameAllocator_t&  Get (FrameUID frameId)   __NE___ { ASSERT( _dbgFrameId.load() == frameId );  return _alloc[ frameId.Index() ]; }
        };


    // variables
    private:
        GlobalLinearAllocator_t         _globalLinear;

        FrameAlloc                      _graphicsFrameAlloc;
        FrameAlloc                      _simulationFrameAlloc;

        InPlace<DefaultAlloc_t>         _defaultAlloc;

        PROFILE_ONLY(
            AtomicRC<IMemoryProfiler>   _profiler;
        )


    // methods
    public:
        void  SetProfiler (RC<IMemoryProfiler> profiler)                __NE___;

        ND_ GlobalLinearAllocator_t&    GetGlobalLinearAllocator ()     __NE___ { return _globalLinear; }

        ND_ FrameAlloc&                 GetGraphicsFrameAllocator ()    __NE___ { return _graphicsFrameAlloc; }
        ND_ FrameAlloc&                 GetSimulationFrameAllocator ()  __NE___ { return _simulationFrameAlloc; }

        ND_ RC<IAllocator>              GetDefaultAllocator ()          __NE___ { return _defaultAlloc->GetRC(); }


    private:
        MemoryManagerImpl ()                                            __NE___;
        ~MemoryManagerImpl ()                                           __NE___;

        friend MemoryManagerImpl&       AE::MemoryManager ()            __NE___;
        ND_ static MemoryManagerImpl&   _Instance ()                    __NE___;
    };

} // AE::Threading


namespace AE
{
/*
=================================================
    MemoryManager
=================================================
*/
    ND_ inline Threading::MemoryManagerImpl&  MemoryManager () __NE___
    {
        return Threading::MemoryManagerImpl::_Instance();
    }

} // AE
