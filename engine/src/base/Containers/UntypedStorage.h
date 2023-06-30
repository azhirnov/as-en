// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/POTValue.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/RestrictPtr.h"

namespace AE::Base
{

    //
    // Static Untyped Storage
    //

    template <usize Size_v, usize Align_v>
    struct UntypedStorage final : Noncopyable
    {
    // types
    public:
        using Self = UntypedStorage< Size_v, Align_v >;


    // variables
    private:
        alignas(Align_v) char   _buffer [Size_v];


    // methods
    public:
        UntypedStorage ()                                               __NE___
        {
            DEBUG_ONLY( DbgInitMem( _buffer, Sizeof(_buffer) ));
        }

        //explicit UntypedStorage (Base::_hidden_::_Zero)               __NE___ : _buffer{}
        //{}

        template <typename T>
        explicit UntypedStorage (const T &value)                        __NE___
        {
            STATIC_ASSERT( sizeof(T) == Size_v );
            STATIC_ASSERT( alignof(T) <= Align_v );

            std::memcpy( OUT _buffer, &value, Size_v );
        }

        ~UntypedStorage ()                                              __NE___
        {
            DEBUG_ONLY( DbgFreeMem( _buffer, Sizeof(_buffer) ));
        }

        template <typename T>
        ND_ T*  Ptr (Bytes offset = 0_b)                                __NE___
        {
            ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
            return Base::Cast<T>( _buffer + offset );
        }

        template <typename T>
        ND_ T const*  Ptr (Bytes offset = 0_b)                          C_NE___
        {
            ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
            return Base::Cast<T>( _buffer + offset );
        }

        template <typename T>
        ND_ T&  Ref (Bytes offset = 0_b)                                __NE___ { return *Ptr<T>( offset ); }

        template <typename T>
        ND_ T const&  Ref (Bytes offset = 0_b)                          C_NE___ { return *Ptr<T>( offset ); }

        ND_ Bytes   Size ()                                             C_NE___ { return Bytes{Size_v}; }
        ND_ Bytes   Align ()                                            C_NE___ { return Bytes{Align_v}; }
    };



    //
    // Dynamic Untyped Storage
    //

    template <typename AllocType = UntypedAllocator>
    struct DynUntypedStorage final : Noncopyable
    {
    // types
    public:
        using Self          = DynUntypedStorage< AllocType >;
        using Allocator_t   = AllocType;


    // variables
    private:
        RstPtr<void>    _ptr;
        usize           _size   : 28;
        usize           _align  : 4;


    // methods
    public:
        DynUntypedStorage ()                            __NE___ {}
        DynUntypedStorage (Bytes size, Bytes align)     __NE___ { Alloc( SizeAndAlign{ size, align }); }
        explicit DynUntypedStorage (const SizeAndAlign sizeAndAlign) __NE___ { Alloc( sizeAndAlign ); }

        ~DynUntypedStorage ()                           __NE___ { Dealloc(); }

        ND_ Bytes               Size ()                 C_NE___ { return Bytes{_size}; }
        ND_ POTBytes            AlignPOT ()             C_NE___ { return POTBytes{PowerOfTwo( _align )}; }
        ND_ Bytes               Align ()                C_NE___ { return Bytes{ AlignPOT() }; }
        ND_ bool                Empty ()                C_NE___ { return _ptr == null; }

        ND_ RstPtr<void>        Data ()                 __NE___ { return _ptr; }
        ND_ RstPtr<const void>  Data ()                 C_NE___ { return RstPtr<const void>{ _ptr.get() }; }

        ND_ explicit operator bool ()                   C_NE___ { return not Empty(); }


        bool  Alloc (const SizeAndAlign sizeAndAlign)   __NE___
        {
            Dealloc();

            _size   = usize(sizeAndAlign.size);
            _align  = POTBytes( sizeAndAlign.align ).GetPOT();

            ASSERT( Size() == sizeAndAlign.size );
            ASSERT( Align() == sizeAndAlign.align );

            _ptr = RstPtr<void>{ Allocator_t::Allocate( sizeAndAlign )};
            return _ptr;
        }

        void  Dealloc ()                                __NE___
        {
            if ( _ptr )
                Allocator_t::Deallocate( _ptr.get(), SizeAndAlign{ Size(), Align() });

            _ptr    = null;
            _size   = 0;
            _align  = 0;
        }


        template <typename T>
        ND_ T*  Ptr (Bytes offset = 0_b)                __NE___
        {
            ASSERT( SizeOf<T> + offset <= Size() );
            return Base::Cast<T>( _ptr.get() + offset );
        }

        template <typename T>
        ND_ T const*  Ptr (Bytes offset = 0_b)          C_NE___
        {
            ASSERT( SizeOf<T> + offset <= Size() );
            return Base::Cast<T>( _ptr.get() + offset );
        }


        ND_ void*  Ptr (Bytes offset = 0_b)             __NE___
        {
            ASSERT( offset <= Size() );
            return _ptr.get() + offset;
        }

        ND_ void const*  Ptr (Bytes offset = 0_b)       C_NE___
        {
            ASSERT( offset <= Size() );
            return _ptr.get() + offset;
        }

        template <typename T>
        ND_ T&  Ref (Bytes offset = 0_b)                __NE___ { return *Ptr<T>( offset ); }

        template <typename T>
        ND_ T const&  Ref (Bytes offset = 0_b)          C_NE___ { return *Ptr<T>( offset ); }
    };


} // AE::Base
