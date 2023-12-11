// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

    //
    // GlobalLinearAllocatorRef
    //

    class GlobalLinearAllocatorRef final : public AllocatorRef< MemoryManagerImpl::GlobalLinearAllocator_t >
    {
    public:
        GlobalLinearAllocatorRef () __NE___ :
            AllocatorRef{ MemoryManager().GetGlobalLinearAllocator() }
        {}
    };



    //
    // GlobalLinearStdAllocatorRef
    //

    template <typename T>
    class GlobalLinearStdAllocatorRef final : public StdAllocatorRef< T, MemoryManagerImpl::GlobalLinearAllocator_t* >
    {
        using Base_t = StdAllocatorRef< T, MemoryManagerImpl::GlobalLinearAllocator_t* >;

    public:
        GlobalLinearStdAllocatorRef () __NE___ :
            Base_t{ &MemoryManager().GetGlobalLinearAllocator() }
        {}

        explicit GlobalLinearStdAllocatorRef (MemoryManagerImpl::GlobalLinearAllocator_t* alloc) __NE___ :
            Base_t{ alloc }
        {}
    };


#   define AE_GLOBALLY_ALLOC                                                                                                    \
        void*  operator new (std::size_t size) __NE___                                                                          \
        {                                                                                                                       \
            return AE::MemoryManager().GetGlobalLinearAllocator().Allocate( SizeAndAlign{ Bytes{size}, DefaultAllocatorAlign });\
        }                                                                                                                       \
        void*  operator new (std::size_t size, std::align_val_t align) __NE___                                                  \
        {                                                                                                                       \
            return AE::MemoryManager().GetGlobalLinearAllocator().Allocate( SizeAndAlign{ size, align });                       \
        }                                                                                                                       \
                                                                                                                                \
        void operator delete (void *)                                   __NE___ {}                                              \
        void operator delete (void *, std::align_val_t)                 __NE___ {}                                              \
        void operator delete (void *, std::size_t)                      __NE___ {}                                              \
        void operator delete (void *, std::size_t, std::align_val_t)    __NE___ {}                                              \


    //
    // GlobalLinearAllocatorBase
    //
    struct GlobalLinearAllocatorBase
    {
        AE_GLOBALLY_ALLOC
    };


} // AE::Threading
