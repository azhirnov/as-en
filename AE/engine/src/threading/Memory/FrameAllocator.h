// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

    //
    // GraphicsFrameAllocatorRef
    //

    class GraphicsFrameAllocatorRef final : public AllocatorRef2< MemoryManagerImpl::FrameAllocator_t >
    {
    public:
        GraphicsFrameAllocatorRef () __NE___ :
            AllocatorRef2{ MemoryManager().GetGraphicsFrameAllocator().Get() }
        {}

        explicit GraphicsFrameAllocatorRef (FrameUID frameId) __NE___ :
            AllocatorRef2{ MemoryManager().GetGraphicsFrameAllocator().Get( frameId )}
        {}
    };


    //
    // GraphicsFrameStdAllocatorRef
    //

    template <typename T, typename BaseAlloc>
    class GraphicsFrameStdAllocatorRef final : public StdAllocatorRef< T, MemoryManagerImpl::FrameAllocator_t* >
    {
        using Base_t = StdAllocatorRef< T, MemoryManagerImpl::FrameAllocator_t* >;

    public:
        GraphicsFrameStdAllocatorRef () __NE___ :
            Base_t{ &BaseAlloc{}.GetAllocatorRef() }
        {}

        explicit GraphicsFrameStdAllocatorRef (MemoryManagerImpl::FrameAllocator_t* alloc) __NE___:
            Base_t{ alloc }
        {}
    };


} // AE::Threading
