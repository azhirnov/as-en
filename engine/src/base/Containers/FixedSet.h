// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Set emulation on static array with binary/linear search.
    Use only for small number of elements.

    Recomended maximum size is 8..16 elements.

    References are not invalidated.

    Exceptions:
        - key may throw exceptions (in copy-ctor)
*/

#pragma once

#include "base/Containers/FixedMap.h"

namespace AE::Base
{

    //
    // Fixed Size Set
    //

    template <typename Value,
              usize ArraySize,
              typename Policy = CopyPolicy::AutoDetect< Value >
             >
    struct FixedSet
    {
        STATIC_ASSERT( ArraySize < 256 );
        STATIC_ASSERT( IsNothrowDtor< Value >);
        //STATIC_ASSERT( IsNothrowMoveCtor< Value >);

    // types
    private:
        using Self          = FixedSet< Value, ArraySize, Policy >;
        using Index_t       = Conditional< (ArraySize < 0xFF), ubyte, Conditional< (ArraySize < 0xFFFF), ushort, uint >>;
        using CPolicy_t     = Policy;

        static constexpr bool   _IsNothrowCopy = IsNothrowCopyCtor<Value>;

    public:
        using value_type        = Value;
        using iterator          = const Value *;
        using const_iterator    = iterator;


    // variables
    private:
        Index_t             _count      = 0;
        mutable Index_t     _indices [ArraySize];
        union {
            value_type      _array  [ArraySize];
            char            _buffer [sizeof(value_type) * ArraySize];       // don't use this field!
        };


    // methods
    public:
        FixedSet ()                                                 __NE___;
        FixedSet (Self &&)                                          __NE___;
        FixedSet (const Self &)                                     noexcept(_IsNothrowCopy);

        ~FixedSet ()                                                __NE___ { clear(); }

        ND_ usize       size ()                                     C_NE___ { return _count; }
        ND_ bool        empty ()                                    C_NE___ { return _count == 0; }

        ND_ iterator    begin ()                                    C_NE___ { return &_array[0]; }
        ND_ iterator    end ()                                      C_NE___ { return begin() + _count; }

        ND_ static constexpr usize  capacity ()                     __NE___ { return ArraySize; }

            Self&   operator = (Self &&)                            __NE___;
            Self&   operator = (const Self &)                       noexcept(_IsNothrowCopy);

        ND_ bool    operator == (const Self &rhs)                   C_NE___;
        ND_ bool    operator != (const Self &rhs)                   C_NE___ { return not (*this == rhs); }

            template <typename ValueType>
            Pair<iterator,bool>  emplace (ValueType&& value)        noexcept(_IsNothrowCopy);

            Pair<iterator,bool>  insert (const Value &value)        noexcept(_IsNothrowCopy)    { return emplace( value ); }
            Pair<iterator,bool>  insert (Value&& value)             __NE___                     { return emplace( RVRef(value) ); }

            template <typename ValueType>
            Pair<iterator,bool>  insert_or_assign (ValueType&& value) noexcept(_IsNothrowCopy);

            template <typename ValueType>
            bool        try_insert (ValueType&& value)              noexcept(_IsNothrowCopy);

            template <typename KeyType>
            bool        erase (const KeyType &key)                  __NE___;

            template <typename KeyType>
        ND_ iterator    find (const KeyType &key)                   C_NE___;

            template <typename KeyType>
        ND_ usize       count (const KeyType &key)                  C_NE___ { return contains( key ) ? 1 : 0; }

            template <typename KeyType>
        ND_ bool        contains (const KeyType &key)               C_NE___;

        ND_ HashVal     CalcHash ()                                 C_NE___;

            void        clear ()                                    __NE___;
            void        reserve (usize)                             __NE___ {}

        // cache friendly access to unsorted data

        ND_ explicit operator ArrayView<Value> ()                   C_NE___ { return { &_array[0], size() }; }

        ND_ Value const&    operator [] (usize i)                   C_NE___;

    private:
        ND_ forceinline bool _IsMemoryAliased (const Self* other)   C_NE___
        {
            return IsIntersects( this, this+1, other, other+1 );
        }
    };



/*
=================================================
    constructor
=================================================
*/
    template <typename V, usize S, typename CS>
    FixedSet<V,S,CS>::FixedSet () __NE___
    {
        DEBUG_ONLY( DbgInitMem( _indices ));
        DEBUG_ONLY( DbgInitMem( _array   ));
    }

/*
=================================================
    constructor
=================================================
*/
    template <typename V, usize S, typename CS>
    FixedSet<V,S,CS>::FixedSet (const Self &other) noexcept(_IsNothrowCopy) : _count{ other._count }
    {
        ASSERT( not _IsMemoryAliased( &other ));

        CPolicy_t::Copy( _array, other._array, _count );    // throw
        MemCopy( _indices, other._indices, SizeOf<Index_t> * _count );
    }

/*
=================================================
    constructor
=================================================
*/
    template <typename V, usize S, typename CS>
    FixedSet<V,S,CS>::FixedSet (Self &&other) __NE___ : _count{ other._count }
    {
        ASSERT( not _IsMemoryAliased( &other ));

        CPolicy_t::Replace( _array, other._array, _count );
        MemCopy( _indices, other._indices, SizeOf<Index_t> * _count );

        other._count = 0;
        DEBUG_ONLY( DbgInitMem( other._indices ));
    }

/*
=================================================
    operator =
=================================================
*/
    template <typename V, usize S, typename CS>
    FixedSet<V,S,CS>&  FixedSet<V,S,CS>::operator = (Self &&rhs) __NE___
    {
        ASSERT( not _IsMemoryAliased( &rhs ));

        CPolicy_t::Destroy( _array, _count );

        _count = rhs._count;

        CPolicy_t::Replace( _array, rhs._array, _count );
        MemCopy( _indices, rhs._indices, SizeOf<Index_t> * _count );

        rhs._count = 0;
        DEBUG_ONLY( DbgInitMem( rhs._indices ));

        return *this;
    }

/*
=================================================
    operator =
=================================================
*/
    template <typename V, usize S, typename CS>
    FixedSet<V,S,CS>&  FixedSet<V,S,CS>::operator = (const Self &rhs) noexcept(_IsNothrowCopy)
    {
        ASSERT( not _IsMemoryAliased( &rhs ));

        CPolicy_t::Destroy( _array, _count );

        _count = rhs._count;

        CPolicy_t::Copy( _array, rhs._array, _count );  // throw
        MemCopy( _indices, rhs._indices, SizeOf<Index_t> * _count );

        return *this;
    }

/*
=================================================
    operator ==
=================================================
*/
    template <typename V, usize S, typename CS>
    bool  FixedSet<V,S,CS>::operator == (const Self &rhs) C_NE___
    {
        if ( this == &rhs )
            return true;

        if ( _count != rhs._count )
            return false;

        for (usize i = 0; i < _count; ++i)
        {
            if_unlikely( not (_array[_indices[i]] == rhs._array[rhs._indices[i]]) )
                return false;
        }
        return true;
    }

/*
=================================================
    operator []
=================================================
*/
    template <typename V, usize S, typename CS>
    V const&  FixedSet<V,S,CS>::operator [] (usize i) C_NE___
    {
        ASSERT( i < _count );
        return _array[i]; // don't use '_indices'
    }

/*
=================================================
    try_insert
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename ValueType>
    bool  FixedSet<V,S,CS>::try_insert (ValueType&& value) noexcept(_IsNothrowCopy)
    {
        if_likely( _count < capacity() )
        {
            emplace( FwdArg<ValueType>(value) );    // throw
            return true;
        }
        return false;
    }

/*
=================================================
    emplace
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename ValueType>
    Pair< typename FixedSet<V,S,CS>::iterator, bool >
        FixedSet<V,S,CS>::emplace (ValueType&& value) noexcept(_IsNothrowCopy)
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< ValueType, value_type, Index_t >;

        usize   i = BinarySearch::LowerBound( _count, value, _indices, _array );

        if_likely( i < _count and value == _array[_indices[i]] )
            return { iterator{ &_array[_indices[i]] }, false };

        // insert
        ASSERT( _count < capacity() );

        const usize j = _count++;
        PlacementNew<value_type>( &_array[j], FwdArg<ValueType>(value) );   // throw

        if ( i < _count )
            for (usize k = _count-1; k > i; --k) {
                _indices[k] = _indices[k-1];
            }
        else
            i = j;

        _indices[i] = Index_t(j);
        return { iterator{ &_array[j] }, true };
    }

/*
=================================================
    insert_or_assign
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename ValueType>
    Pair< typename FixedSet<V,S,CS>::iterator, bool >
        FixedSet<V,S,CS>::insert_or_assign (ValueType&& value) noexcept(_IsNothrowCopy)
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< ValueType, value_type, Index_t >;

        usize   i = BinarySearch::LowerBound( _count, value, _indices, _array );

        if_likely( i < _count and value == _array[_indices[i]] )
        {
            _array[_indices[i]] = FwdArg<ValueType>(value);
            return { iterator{ &_array[_indices[i]] }, false };
        }

        // insert
        ASSERT( _count < capacity() );

        const usize j = _count++;
        PlacementNew<value_type>( &_array[j], FwdArg<ValueType>(value) );   // throw

        if ( i < _count )
            for (usize k = _count-1; k > i; --k) {
                _indices[k] = _indices[k-1];
            }
        else
            i = j;

        _indices[i] = Index_t(j);
        return { iterator{ &_array[j] }, true };
    }

/*
=================================================
    find
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename KeyType>
    typename FixedSet<V,S,CS>::iterator
        FixedSet<V,S,CS>::find (const KeyType &key) C_NE___
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, value_type, Index_t >;

        const usize i = BinarySearch::Find( _count, key, _indices, _array );

        if_likely( i < _count and key == _array[_indices[i]] )
            return iterator{ &_array[_indices[i]] };

        return end();
    }

/*
=================================================
    contains
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename KeyType>
    bool  FixedSet<V,S,CS>::contains (const KeyType &key) C_NE___
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, value_type, Index_t >;

        const usize i = BinarySearch::Find( _count, key, _indices, _array );

        return (i < _count) and (key == _array[_indices[i]]);
    }

/*
=================================================
    erase
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename KeyType>
    bool  FixedSet<V,S,CS>::erase (const KeyType &key) __NE___
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, value_type, Index_t >;

        usize   i = BinarySearch::LowerBound( _count, key, _indices, _array );

        if_likely( i < _count and key == _array[_indices[i]] )
        {
            const auto  idx = _indices[i];

            _array[idx].~V();
            --_count;

            for (usize k = 0; k <= _count; ++k) {
                _indices[k] = (_indices[k] == _count ? idx : _indices[k]);
            }

            for (usize k = i; k < _count; ++k) {
                _indices[k] = (_indices[k+1] == _count ? idx : _indices[k+1]);
            }

            if ( idx != _count )
                CPolicy_t::Replace( &_array[idx], &_array[_count], 1, true );

            DEBUG_ONLY(
                DbgInitMem( _indices[_count] );
            )
            return true;
        }
        return false;
    }

/*
=================================================
    clear
=================================================
*/
    template <typename V, usize S, typename CS>
    void  FixedSet<V,S,CS>::clear () __NE___
    {
        CPolicy_t::Destroy( _array, _count );
        DEBUG_ONLY( DbgInitMem( _indices ));

        _count = 0;
    }

/*
=================================================
    CalcHash
=================================================
*/
    template <typename V, usize S, typename CS>
    HashVal  FixedSet<V,S,CS>::CalcHash () C_NE___
    {
        HashVal     result = HashOf( size() );

        for (usize i = 0; i < size(); ++i)
        {
            result << HashOf( _array[ _indices[i] ] );
        }
        return result;
    }

} // AE::Base


namespace std
{
    template <typename Value, size_t ArraySize, typename CS>
    struct hash< AE::Base::FixedSet<Value, ArraySize, CS> >
    {
        ND_ size_t  operator () (const AE::Base::FixedSet<Value, ArraySize, CS> &value) C_NE___
        {
            return size_t(value.CalcHash());
        }
    };

} // std
