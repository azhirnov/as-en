// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/RefCounter.h"
#include "base/Memory/IAllocator.h"
#include "base/Containers/ArrayView.h"
#include "base/Math/Math.h"
#include "base/Math/POTValue.h"
#include "base/Utils/Threading.h"

namespace AE::Base
{
namespace _hidden_
{
    template <typename T>
    struct TSharedMem_Extra {
        T       data {};
    };

    template <>
    struct TSharedMem_Extra<void> {};
}


    //
    // Shared Memory
    //

    template <typename ExtraDataType>
    class TSharedMem final :
        public EnableRC< TSharedMem<ExtraDataType> >,
        public NonAllocatable
    {
    // types
    public:
        using Allocator_t   = RC<IAllocator>;
        using Self          = TSharedMem< ExtraDataType >;

    private:
        using Extra_t       = Base::_hidden_::TSharedMem_Extra< ExtraDataType >;

        static constexpr bool   _HasExtra = not IsVoid< ExtraDataType >;


    // variables
    private:
        Bytes32u        _size;
        POTBytes        _align;
        Extra_t         _extra;
        Allocator_t     _allocator;


    // methods
    public:
        ND_ void const*     Data ()         C_NE___ { return this + AlignUp( SizeOf<Self>, _align ); }
        ND_ void*           Data ()         __NE___ { return this + AlignUp( SizeOf<Self>, _align ); }
        ND_ Bytes           Size ()         C_NE___ { return _size; }
        ND_ Bytes           Align ()        C_NE___ { return Bytes{ _align }; }
        ND_ auto            Allocator ()    C_NE___ { return _allocator; }

        ND_ auto&           Extra ()        __NE___ { if constexpr( _HasExtra ) return &_extra.data; }
        ND_ auto&           Extra ()        C_NE___ { if constexpr( _HasExtra ) return &_extra.data; }

        ND_ bool            Contains (const void* ptr, Bytes size = 0_b) C_NE___;

        template <typename T>
        ND_ ArrayView<T>    AsArray ()      C_NE___;

        template <typename T>
        ND_ T&              As ()           __NE___;

        template <typename T>
        ND_ T const&        As ()           C_NE___;


        ND_ static RC<Self>  Create (Allocator_t alloc, const SizeAndAlign sizeAndAlign)                    __NE___;
        ND_ static RC<Self>  Create (Allocator_t alloc, Bytes size, Bytes align = DefaultAllocatorAlign)    __NE___;


    private:
        TSharedMem (Bytes size, POTBytes align, Allocator_t alloc)      __NE___;
        ~TSharedMem ()                                                  __NE_OV {}

        void  _ReleaseObject ()                                         __NE_OV;

        ND_ static SizeAndAlign  _CalcSize (Bytes size, POTBytes align) __NE___;
    };

    using SharedMem = TSharedMem<void>;



/*
=================================================
    constructor
=================================================
*/
    template <typename E>
    TSharedMem<E>::TSharedMem (Bytes size, POTBytes align, Allocator_t alloc) __NE___ :
        _size{size}, _align{align}, _allocator{RVRef(alloc)}
    {
        DEBUG_ONLY( DbgInitMem( OUT Data(), Size() ));
    }

/*
=================================================
    Contains
=================================================
*/
    template <typename E>
    bool  TSharedMem<E>::Contains (const void* ptr, Bytes size) C_NE___
    {
        return IsIntersects<const void*>( Data(), Data() + _size, ptr, ptr + size );
    }

/*
=================================================
    AsArray
=================================================
*/
    template <typename E>
    template <typename T>
    ArrayView<T>  TSharedMem<E>::AsArray () C_NE___
    {
        ASSERT( IsMultipleOf( _size, SizeOf<T> ));
        ASSERT( POTAlignOf<T> <= _align );
        return ArrayView<T>{ Cast<T>(Data()), _size / SizeOf<T> };
    }

/*
=================================================
    As
=================================================
*/
    template <typename E>
    template <typename T>
    T&  TSharedMem<E>::As () __NE___
    {
        ASSERT( IsMultipleOf( _size, SizeOf<T> ));
        ASSERT( POTAlignOf<T> <= _align );
        return *Cast<T>(Data());
    }

    template <typename E>
    template <typename T>
    T const&  TSharedMem<E>::As () C_NE___
    {
        ASSERT( IsMultipleOf( _size, SizeOf<T> ));
        ASSERT( POTAlignOf<T> <= _align );
        return *Cast<T>(Data());
    }

/*
=================================================
    Create
=================================================
*/
    template <typename E>
    RC<TSharedMem<E>>  TSharedMem<E>::Create (Allocator_t alloc, const SizeAndAlign sizeAndAlign) __NE___
    {
        return Create( RVRef(alloc), sizeAndAlign.size, sizeAndAlign.align );
    }

    template <typename E>
    RC<TSharedMem<E>>  TSharedMem<E>::Create (Allocator_t alloc, Bytes size, Bytes align) __NE___
    {
        if_likely( alloc and size > 0 )
        {
            auto    align_pot   = POTBytes{ align };
            void*   self        = alloc->Allocate( _CalcSize( size, align_pot ));

            if_likely( self != null )
                return RC<Self>{ new(self) Self{ size, align_pot, RVRef(alloc) }};
        }
        return Default;
    }

/*
=================================================
    _CalcSize
=================================================
*/
    template <typename E>
    SizeAndAlign  TSharedMem<E>::_CalcSize (Bytes size, POTBytes align) __NE___
    {
        return SizeAndAlign{ AlignUp( SizeOf<Self>, align ) + size, Bytes{Max( POTAlignOf<Self>, align )} };
    }

/*
=================================================
    _ReleaseObject
=================================================
*/
    template <typename E>
    void  TSharedMem<E>::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );
        CHECK( _allocator );

        auto    alloc   = RVRef(_allocator);
        void*   ptr     = this;
        auto    sa      = _CalcSize( _size, _align );

        _extra.~Extra_t();
        _allocator.~RC();

        alloc->Deallocate( ptr, sa );

        // don't call 'delete this'

        // TODO: flush cache ?
    }


} // AE::Base
