// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Math/Bytes.h"
#include "base/Math/POTValue.h"
#include "base/Utils/SourceLoc.h"
#include "base/Utils/RestrictPtr.h"

namespace AE::Base
{

    //
    // Multithreaded (Lock-free) Allocator
    //

    class MtAllocator final : public Noninstanceable
    {
        using Ptr_t = RstPtr<void>;

        static constexpr bool   IsThreadSafe    = true;
        static constexpr Bytes  DefaultAlign    = 8_b;


    // default align //
        ND_ static Ptr_t  Allocate (Bytes size)                                             __NE___ { return Allocate( POTBytes{size}, DefaultAlign ); }
        ND_ static Ptr_t  Allocate (Bytes size, const SourceLoc &loc)                       __NE___ { return Allocate( POTBytes{size}, DefaultAlign, loc ); }

            static void  Deallocate (void *ptr)                                             __NE___;

        // deallocation with explicit size may be faster
            static void  Deallocate (void *ptr, Bytes size)                                 __NE___ { return Deallocate( ptr, POTBytes{size} ); }


    // custom align //
        ND_ static Ptr_t  Allocate (const SizeAndAlign sizeAndAlign)                        __NE___ { return Allocate( POTBytes{sizeAndAlign.size}, sizeAndAlign.align ); }
        ND_ static Ptr_t  Allocate (const SizeAndAlign sizeAndAlign, const SourceLoc &loc)  __NE___ { return Allocate( POTBytes{sizeAndAlign.size}, sizeAndAlign.align, loc ); }

            //static void  Deallocate (void *ptr, Bytes align)                              __NE___ { Unused( align );  return Deallocate( ptr ); }

        // deallocation with explicit size may be faster
            static void  Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)            __NE___ { Unused( sizeAndAlign.align );  return Deallocate( ptr, POTBytes{sizeAndAlign.size} ); }


    // custom align and POT size //
        ND_ static Ptr_t  Allocate (POTBytes size, Bytes align)                             __NE___ { return Allocate( size, align, SourceLoc_Current() ); }
        ND_ static Ptr_t  Allocate (POTBytes size, Bytes align, const SourceLoc &loc)       __NE___;

            static void  Deallocate (void* ptr, POTBytes size)                              __NE___;


    // utils
        ND_ static Bytes    ReservedSize ()                                                 __NE___;
        ND_ static usize    MaxAllowedAllocations (Bytes size)                              __NE___ { return MaxAllowedAllocations( POTBytes{size} ); }
        ND_ static usize    MaxAllowedAllocations (POTBytes size)                           __NE___;
            static void     ReleaseAll ()                                                   __NE___;
    };


} // AE::Base
