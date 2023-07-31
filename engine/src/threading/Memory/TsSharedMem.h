// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: yes
*/

#pragma once

#include "threading/Primitives/SpinLock.h"

namespace AE::Threading
{

    //
    // Shared Memory
    //

    class alignas(AE_CACHE_LINE) TsSharedMem final :
        public EnableRC< TsSharedMem >,
        public NonAllocatable
    {
    // types
    public:
        using Allocator_t   = RC<IAllocator>;
        using Self          = TsSharedMem;

    private:

        //
        // Write Access
        //
        struct WriteAccess : MovableOnly
        {
        private:
            Self *  _ref;

            friend class TsSharedMem;
            explicit WriteAccess (Self* ref)    __NE___ : _ref{ref} {}

        public:
            ~WriteAccess ()                     __NE___ { if ( _ref != null ) _ref->_guard.unlock(); }

            ND_ void*   Data ()                 C_NE___ { return _ref != null ? _ref->_Data() : null; }
            ND_ Bytes   Size ()                 C_NE___ { return _ref != null ? _ref->Size()  : 0_b; }
            ND_ Bytes   Align ()                C_NE___ { return _ref != null ? _ref->Align() : 0_b; }
        };


        //
        // Read Access
        //
        struct ReadAccess : MovableOnly
        {
        private:
            Self *  _ref;

            friend class TsSharedMem;
            explicit ReadAccess (Self* ref)     __NE___ : _ref{ref} {}

        public:
            ~ReadAccess ()                      __NE___ { if ( _ref != null ) _ref->_guard.unlock_shared(); }

            ND_ void*   Data ()                 C_NE___ { return _ref != null ? _ref->_Data() : null; }
            ND_ Bytes   Size ()                 C_NE___ { return _ref != null ? _ref->Size()  : 0_b; }
            ND_ Bytes   Align ()                C_NE___ { return _ref != null ? _ref->Align() : 0_b; }

            template <typename T>
            ND_ ArrayView<T>  ToView ()         C_NE___
            {
                ASSERT( Size() % SizeOf<T> == 0 );
                ASSERT( AlignOf<T> <= Align() );
                return ArrayView<T>{ Cast<T>(Data()), Size() / SizeOf<T> };
            }
        };


    // variables
    private:
        RWSpinLock      _guard;     // used to protect access to '_Data()'
        Bytes32u        _size;
        POTBytes        _align;
        Allocator_t     _allocator;


    // methods
    public:
        ND_ Bytes       Size ()         C_NE___ { return _size; }
        ND_ Bytes       Align ()        C_NE___ { return Bytes{ _align }; }
        ND_ auto        Allocator ()    C_NE___ { return _allocator; }

        ND_ ReadAccess  Read ()         __NE___ { return ReadAccess{  _guard.try_lock_shared()  ? this : null }; }
        ND_ WriteAccess Write ()        __NE___ { return WriteAccess{ _guard.try_lock()         ? this : null }; }

        ND_ static RC<Self>  Create (Allocator_t alloc, const SizeAndAlign sizeAndAlign)                    __NE___;
        ND_ static RC<Self>  Create (Allocator_t alloc, Bytes size, Bytes align = DefaultAllocatorAlign)    __NE___;
        ND_ static RC<Self>  Create (Bytes size, Bytes align = DefaultAllocatorAlign)                       __NE___;


    private:
        TsSharedMem (Bytes size, POTBytes align, Allocator_t alloc)     __NE___ : _size{size}, _align{align}, _allocator{RVRef(alloc)} {}
        ~TsSharedMem ()                                                 __NE_OV {}

        ND_ void const*     _Data ()                                    C_NE___ { return this + AlignUp( SizeOf<Self>, _align ); }
        ND_ void*           _Data ()                                    __NE___ { return this + AlignUp( SizeOf<Self>, _align ); }

        void  _ReleaseObject ()                                         __NE_OV;

        ND_ static SizeAndAlign  _CalcSize (Bytes size, POTBytes align) __NE___;
    };


/*
=================================================
    _CalcSize
=================================================
*/
    inline SizeAndAlign  TsSharedMem::_CalcSize (Bytes size, POTBytes align) __NE___
    {
        return SizeAndAlign{ AlignUp( SizeOf<Self>, align ) + size, Bytes{Max( POTAlignOf<Self>, align )} };
    }

/*
=================================================
    _ReleaseObject
=================================================
*/
    inline void  TsSharedMem::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );
        CHECK( _allocator );

        auto    alloc   = RVRef(_allocator);
        void*   ptr     = this;
        auto    sa      = _CalcSize( _size, _align );

        _allocator.~RC();

        alloc->Deallocate( ptr, sa );

        // don't call 'delete this'

        // TODO: flush cache ?
    }


} // AE::Threading
