// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Purpose:
     * lock-free multiple producers
     * non-lock-free for extracting multiple elements (single consumer)
     * lock-free for extracting single element
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{

    //
    // Lock-free Static Queue
    //

    template <typename Value,
              typename AllocatorType = UntypedAllocator>
    class LfStaticQueue final  : public Noncopyable
    {
    // types
    public:
        using Self          = LfStaticQueue< AllocatorType >;
        using Allocator_t   = AllocatorType;
        using Value_t       = Value;

    private:
        static constexpr bool   _NonTrivialDtor = not (IsZeroMemAvailable<Value> or IsTriviallyDestructible<Value>);
        static constexpr uint   _MaxSize        = 1 << 12;
        static constexpr uint   _MaxSpin        = 2'000;

        struct Bits
        {
            uint    first   : 15;
            uint    last    : 15;
            uint    locked  : 1;

            Bits ()                 __NE___ : first{0}, last{0}, locked{0} {}

            ND_ bool  Empty ()      C_NE___ { return first >= last; }
            ND_ uint  Count ()      C_NE___ { return first < last ? last - first : 0; }
        };


    // variables
    private:
        StructAtomic< Bits >    _packed     { Bits{} };
        uint                    _count      = 0;
        void *                  _arr        = null;

        NO_UNIQUE_ADDRESS
         Allocator_t            _allocator;

        DRC_ONLY( RWDataRaceCheck   _drCheck;)


    // methods
    public:
        explicit LfStaticQueue (const Allocator_t &alloc = Allocator_t{}) __NE___ : _allocator{alloc} {}
        ~LfStaticQueue ()                           __NE___ { Release(); }

            bool  Init (usize size)                 __NE___;
            void  Release ()                        __NE___;
            void  Clear ()                          __NE___;

            bool  Push (const Value_t &value)       __NE___;
            bool  Push (Value_t &&value)            __NE___;

        ND_ bool  Empty ()                          __NE___;
        ND_ uint  Count ()                          __NE___;
        ND_ bool  IsLockedForRead ()                __NE___;

        ND_ bool  First (OUT Value_t &value)        __NE___;
        ND_ bool  Pop (OUT Value_t &value)          __NE___;

        template <typename ArrayType>
        ND_ bool  ExtractAll (OUT ArrayType &dstArr) __NE___;
    };

} // AE::Threading

#include "threading/Containers/LfStaticQueue.inl.h"
