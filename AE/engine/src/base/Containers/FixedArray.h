// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Memory/CopyPolicy.h"
#include "base/Math/Vec.h"

namespace AE::Base
{

    //
    // Fixed Size Array
    //

    template <typename T,
              usize ArraySize,
              typename TCopyPolicy = CopyPolicy::AutoDetect<T>
             >
    struct FixedArray : NothrowAllocatable
    {
    // types
    public:
        using iterator          = T *;
        using const_iterator    = const T *;
        using Self              = FixedArray< T, ArraySize, TCopyPolicy >;

    private:
        using CPolicy_t         = TCopyPolicy;
        using Count_t           = Conditional< (ArraySize <= MaxValue<ubyte>() and alignof(T) == alignof(ubyte)), ubyte,
                                    Conditional< (alignof(T) == alignof(ushort)), ushort, uint >>;

        StaticAssert( ArraySize <= MaxValue<Count_t>() );


    // variables
    private:
        Count_t         _count  = 0;
        union {
            T           _array[ ArraySize ];
            char        _data[ ArraySize * sizeof(T) ];     // don't use this field!
        };


    // methods
    public:
        constexpr FixedArray ()                                 __NE___;
        constexpr FixedArray (std::initializer_list<T> list)    __NE___;        // TODO: use Args... to allow move ctor
        constexpr FixedArray (ArrayView<T> view)                __NE___;
        constexpr FixedArray (const Self &other)                __NE___;
        constexpr FixedArray (Self &&other)                     __NE___;

        ~FixedArray ()                                          __NE___ { clear(); }


        ND_ constexpr operator ArrayView<T> ()                  C_NE___ { return ArrayView<T>{ data(), size() }; }
        ND_ constexpr ArrayView<T>      ToArrayView()           C_NE___ { return *this; }

        ND_ constexpr usize             size ()                 C_NE___ { return _count; }
        ND_ constexpr bool              empty ()                C_NE___ { return _count == 0; }
        ND_ constexpr T *               data ()                 __NE___ { return std::addressof( _array[0] ); }
        ND_ constexpr T const *         data ()                 C_NE___ { return std::addressof( _array[0] ); }

        ND_ constexpr T &               operator [] (usize i)   __NE___ { ASSERT( i < _count );  return _array[i]; }
        ND_ constexpr T const &         operator [] (usize i)   C_NE___ { ASSERT( i < _count );  return _array[i]; }

        ND_ constexpr iterator          begin ()                __NE___ { return data(); }
        ND_ constexpr const_iterator    begin ()                C_NE___ { return data(); }
        ND_ constexpr iterator          end ()                  __NE___ { return data() + _count; }
        ND_ constexpr const_iterator    end ()                  C_NE___ { return data() + _count; }

        ND_ constexpr T &               front ()                __NE___ { ASSERT( _count > 0 );  return _array[0]; }
        ND_ constexpr T const&          front ()                C_NE___ { ASSERT( _count > 0 );  return _array[0]; }
        ND_ constexpr T &               back ()                 __NE___ { ASSERT( _count > 0 );  return _array[_count-1]; }
        ND_ constexpr T const&          back ()                 C_NE___ { ASSERT( _count > 0 );  return _array[_count-1]; }

        ND_ static constexpr usize      capacity ()             __NE___ { return ArraySize; }

        ND_ constexpr bool  operator == (ArrayView<T> rhs)      C_NE___ { return ArrayView<T>{*this} == rhs; }
        ND_ constexpr bool  operator != (ArrayView<T> rhs)      C_NE___ { return ArrayView<T>{*this} != rhs; }
        ND_ constexpr bool  operator >  (ArrayView<T> rhs)      C_NE___ { return ArrayView<T>{*this} >  rhs; }
        ND_ constexpr bool  operator <  (ArrayView<T> rhs)      C_NE___ { return ArrayView<T>{*this} <  rhs; }
        ND_ constexpr bool  operator >= (ArrayView<T> rhs)      C_NE___ { return ArrayView<T>{*this} >= rhs; }
        ND_ constexpr bool  operator <= (ArrayView<T> rhs)      C_NE___ { return ArrayView<T>{*this} <= rhs; }


        constexpr Self&  operator = (const Self &rhs)           __NE___;
        constexpr Self&  operator = (ArrayView<T> rhs)          __NE___;
        constexpr Self&  operator = (Self &&rhs)                __NE___;

        constexpr void  assign (const_iterator beginIter, const_iterator endIter)   __NE___;
        constexpr void  append (ArrayView<T> items)                                 __NE___;

        constexpr void  push_back (const T &value)              __NE___;
        constexpr void  push_back (T &&value)                   __NE___;

        template <typename ...Args>
        constexpr T&    emplace_back (Args&& ...args)           __NE___;

        constexpr void  pop_back ()                             __NE___;

        constexpr bool  try_push_back (const T &value)          __NE___;
        constexpr bool  try_push_back (T&& value)               __NE___;

        template <typename ...Args>
        constexpr bool  try_emplace_back (Args&& ...args)       __NE___;

        constexpr void  insert (usize pos, T &&value)           __NE___;

        constexpr void  resize (usize newSize)                  __NE___;
        constexpr void  resize (usize newSize, const T &defaultValue) __NE___;

        constexpr void  clear ()                                __NE___;

        constexpr void  erase (usize index)                     __NE___;
        constexpr void  fast_erase (usize index)                __NE___;

    private:
        ND_ bool  _IsMemoryAliased (const_iterator beginIter, const_iterator endIter) C_NE___
        {
            return IsIntersects( begin(), end(), beginIter, endIter );
        }
    };


/*
=================================================
    constructor
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>::FixedArray () __NE___
    {
        DEBUG_ONLY( DbgInitMem( data(), SizeOf<T> * capacity() ));

        StaticAssert( IsMultipleOf( alignof(Self), alignof(T) ));
    }

    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>::FixedArray (std::initializer_list<T> list) __NE___ : FixedArray()
    {
        ASSERT( list.size() <= capacity() );
        assign( list.begin(), list.end() );
    }

    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>::FixedArray (ArrayView<T> view) __NE___ : FixedArray()
    {
        ASSERT( view.size() <= capacity() );
        assign( view.begin(), view.end() );
    }

    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>::FixedArray (const Self &other) __NE___ : FixedArray()
    {
        assign( other.begin(), other.end() );
    }

    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>::FixedArray (Self &&other) __NE___ : _count{other._count}
    {
        ASSERT( not _IsMemoryAliased( other.begin(), other.end() ));
        CheckNothrow( IsNothrowMoveCtor<T> );

        CPolicy_t::Replace( OUT _array, INOUT other._array, _count );
        other._count = 0;
    }

/*
=================================================
    operator =
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (const Self &rhs) __NE___
    {
        assign( rhs.begin(), rhs.end() );
        return *this;
    }

    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (ArrayView<T> rhs) __NE___
    {
        ASSERT( rhs.size() < capacity() );
        assign( rhs.begin(), rhs.end() );
        return *this;
    }

    template <typename T, usize S, typename CS>
    constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (Self &&rhs) __NE___
    {
        ASSERT( not _IsMemoryAliased( rhs.begin(), rhs.end() ));
        CheckNothrow( IsNothrowMoveCtor<T> );

        CPolicy_t::Destroy( INOUT _array, _count );
        CPolicy_t::Replace( OUT _array, INOUT rhs._array, rhs._count );

        _count      = rhs._count;
        rhs._count  = 0;

        return *this;
    }

/*
=================================================
    assign
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::assign (const_iterator beginIter, const_iterator endIter) __NE___
    {
        ASSERT( beginIter <= endIter );
        ASSERT( not _IsMemoryAliased( beginIter, endIter ));

        clear();

        for (auto iter = beginIter; (_count < capacity()) and (iter != endIter); ++iter, ++_count)
        {
            PlacementNew<T>( OUT data() + _count, *iter );
        }
    }

/*
=================================================
    append
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::append (ArrayView<T> items) __NE___
    {
        for (auto& item : items) {
            push_back( item );
        }
    }

/*
=================================================
    push_back
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::push_back (const T &value) __NE___
    {
        ASSERT( _count < capacity() );
        PlacementNew<T>( OUT data() + _count, value );
        ++_count;
    }

    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::push_back (T &&value) __NE___
    {
        ASSERT( _count < capacity() );
        PlacementNew<T>( OUT data() + _count, RVRef(value) );
        ++_count;
    }

/*
=================================================
    emplace_back
=================================================
*/
    template <typename T, usize S, typename CS>
    template <typename ...Args>
    constexpr T&  FixedArray<T,S,CS>::emplace_back (Args&& ...args) __NE___
    {
        ASSERT( _count < capacity() );
        T* ptr = data() + _count;
        PlacementNew<T>( OUT ptr, FwdArg<Args>( args )... );
        ++_count;
        return *ptr;
    }

/*
=================================================
    pop_back
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::pop_back () __NE___
    {
        ASSERT( _count > 0 );
        --_count;

        CPolicy_t::Destroy( INOUT &_array[_count], 1 );
    }

/*
=================================================
    try_push_back
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr bool  FixedArray<T,S,CS>::try_push_back (const T &value) __NE___
    {
        if_likely( _count < capacity() )
        {
            PlacementNew<T>( OUT data() + _count, value );
            ++_count;
            return true;
        }
        return false;
    }

    template <typename T, usize S, typename CS>
    constexpr bool  FixedArray<T,S,CS>::try_push_back (T&& value) __NE___
    {
        if_likely( _count < capacity() )
        {
            PlacementNew<T>( OUT data() + _count, RVRef(value) );
            ++_count;
            return true;
        }
        return false;
    }

/*
=================================================
    try_emplace_back
=================================================
*/
    template <typename T, usize S, typename CS>
    template <typename ...Args>
    constexpr bool  FixedArray<T,S,CS>::try_emplace_back (Args&& ...args) __NE___
    {
        if_likely( _count < capacity() )
        {
            PlacementNew<T>( OUT data() + _count, FwdArg<Args &&>( args )... );
            ++_count;
            return true;
        }
        return false;
    }

/*
=================================================
    insert
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::insert (usize pos, T &&value) __NE___
    {
        ASSERT( _count < capacity() );
        CheckNothrow( IsNothrowMoveCtor<T> );

        pos = Min( pos, _count );
        CPolicy_t::Replace( OUT &_array[pos+1], INOUT &_array[pos], _count - pos );

        ++_count;
        PlacementNew<T>( OUT &_array[pos], RVRef(value) );
    }

/*
=================================================
    resize
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::resize (usize newSize) __NE___
    {
        newSize = Min( newSize, capacity() );

        if ( newSize < _count )
        {
            CPolicy_t::Destroy( INOUT &_array[newSize], _count - newSize );
        }
        else
        if ( newSize > _count )
        {
            CheckNothrow( IsNothrowDefaultCtor<T> );
            CPolicy_t::Create( OUT &_array[_count], newSize - _count );
        }

        _count = Count_t(newSize);
    }

    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::resize (usize newSize, const T &defaultValue) __NE___
    {
        newSize = Min( newSize, capacity() );

        if ( newSize < _count )
        {
            CPolicy_t::Destroy( INOUT &_array[newSize], _count - newSize );
        }
        else
        if ( newSize > _count )
        {
            CheckNothrow( IsNothrowCopyCtor<T> );
            CPolicy_t::Create( OUT &_array[_count], newSize - _count, defaultValue );
        }

        _count = Count_t(newSize);
    }

/*
=================================================
    clear
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::clear () __NE___
    {
        CPolicy_t::Destroy( INOUT _array, _count );

        _count = 0;
    }

/*
=================================================
    fast_erase
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::fast_erase (usize index) __NE___
    {
        ASSERT( index < _count );

        --_count;
        CPolicy_t::Destroy( INOUT &_array[index], 1 );

        if ( index != _count )
        {
            // move element from back to 'index'
            CheckNothrow( IsNothrowMoveCtor<T> );
            CPolicy_t::Replace( OUT &_array[index], INOUT &_array[_count], 1 );
        }
        else
        {
            DEBUG_ONLY( DbgInitMem( data() + _count, SizeOf<T> ));
        }
    }

/*
=================================================
    erase
=================================================
*/
    template <typename T, usize S, typename CS>
    constexpr void  FixedArray<T,S,CS>::erase (usize index) __NE___
    {
        ASSERT( index < _count );

        CPolicy_t::Destroy( INOUT &_array[index], 1 );

        if ( index+1 < _count )
        {
            CheckNothrow( IsNothrowMoveCtor<T> );
            CPolicy_t::Replace( OUT &_array[index], INOUT &_array[index + 1], _count - index - 1 );
        }
        --_count;
    }
//-----------------------------------------------------------------------------


    template <typename T, usize S, typename CS> struct TMemCopyAvailable< FixedArray<T,S,CS> >      { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T, usize S, typename CS> struct TZeroMemAvailable< FixedArray<T,S,CS> >      { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T, usize S, typename CS> struct TTriviallyDestructible< FixedArray<T,S,CS> > { static constexpr bool  value = IsTriviallyDestructible<T>; };


} // AE::Base


template <typename T, size_t ArraySize, typename CS>
struct std::hash< AE::Base::FixedArray<T, ArraySize, CS> >
{
    ND_ size_t  operator () (const AE::Base::FixedArray<T, ArraySize, CS> &value) C_NE___
    {
        return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
    }
};
