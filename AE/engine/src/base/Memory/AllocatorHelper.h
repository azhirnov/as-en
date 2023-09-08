// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/BitMath.h"
#include "base/Math/Bytes.h"

namespace AE::Base
{
    enum class EAllocatorType : uint
    {
        Global,
        OnStack,
        Linear,
        Stack,
        FixedBlock,
    };


    //
    // Allocator Helper
    //

    template <EAllocatorType Type>
    class AllocatorHelper
    {
    // methods
    public:
        static void  OnAllocate (void* ptr, Bytes size)                         __NE___
        {
            // TODO: profiler
            ASSERT( size > 0 );
            Unused( ptr, size );
        }

        static void  OnAllocate (void* ptr, const SizeAndAlign sizeAndAlign)    __NE___
        {
            // TODO: profiler
            ASSERT( sizeAndAlign.size >= sizeAndAlign.align );
            ASSERT( IsPowerOfTwo( sizeAndAlign.align ));
            Unused( ptr, sizeAndAlign );
        }

        static void  OnDeallocate (void *ptr)                                   __NE___
        {
            // TODO: profiler
            Unused( ptr );
        }

        static void  OnDeallocate (void *ptr, Bytes size)                       __NE___
        {
            // TODO: profiler
            ASSERT( size > 0 );
            Unused( ptr, size );
        }

        static void  OnDeallocate (void *ptr, const SizeAndAlign sizeAndAlign)  __NE___
        {
            // TODO: profiler
            ASSERT( sizeAndAlign.size >= sizeAndAlign.align );
            ASSERT( IsPowerOfTwo( sizeAndAlign.align ));
            Unused( ptr, sizeAndAlign );
        }
    };


} // AE::Base
