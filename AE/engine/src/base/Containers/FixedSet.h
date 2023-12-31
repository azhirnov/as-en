// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Set emulation on static array with binary/linear search.
    Use only for small number of elements.

    Recommended maximum size is 8..16 elements.

    On insertion references are not invalidated.
    On erase references may be invalidated.
    Iterators are not invalidated.
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
              typename CopyPolicy = CopyPolicy::AutoDetect< Value >
             >
    struct FixedSet : NothrowAllocatable
    {
    // types
    private:
        using Self          = FixedSet< Value, ArraySize, CopyPolicy >;
        using Index_t       = Conditional< (ArraySize <= MaxValue<ubyte>()), ubyte, ushort >;
        using CPolicy_t     = CopyPolicy;

        StaticAssert( ArraySize <= MaxValue<Index_t>() );

    public:
        using value_type        = Value;
        using iterator          = const Value *;
        using const_iterator    = iterator;


    // variables
    private:
        Index_t         _count      = 0;
        Index_t         _indices [ArraySize];
        union {
            value_type  _array  [ArraySize];
            char        _buffer [sizeof(value_type) * ArraySize];       // don't use this field!
        };


    // methods
    public:
        FixedSet ()                                                 __NE___;
        FixedSet (Self &&)                                          __NE___;
        FixedSet (const Self &)                                     __NE___;

        ~FixedSet ()                                                __NE___ { clear(); }    // TODO: remove for trivial destructor

        ND_ usize       size ()                                     C_NE___ { return _count; }
        ND_ bool        empty ()                                    C_NE___ { return _count == 0; }

        ND_ iterator    begin ()                                    C_NE___ { return &_array[0]; }
        ND_ iterator    end ()                                      C_NE___ { return begin() + _count; }

        ND_ static constexpr usize  capacity ()                     __NE___ { return ArraySize; }

            Self&   operator = (Self &&)                            __NE___;
            Self&   operator = (const Self &)                       __NE___;

        ND_ bool    operator == (const Self &rhs)                   C_NE___;
        ND_ bool    operator != (const Self &rhs)                   C_NE___ { return not (*this == rhs); }

            template <typename ValueType>
            Pair<iterator,bool>  emplace (ValueType&& value)        __NE___;

            Pair<iterator,bool>  insert (const Value &value)        __NE___ { return emplace( value ); }
            Pair<iterator,bool>  insert (Value&& value)             __NE___ { return emplace( RVRef(value) ); }

            template <typename ValueType>
            Pair<iterator,bool>  insert_or_assign (ValueType&& value) __NE___;

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

        ND_ usize       IndexOf (iterator it)                       C_NE___;

        // cache friendly access to unsorted data

        ND_ ArrayView<Value>    GetValueArray ()                    C_NE___ { return { &_array[0], size() }; }
        ND_ explicit operator ArrayView<Value> ()                   C_NE___ { return GetValueArray(); }

        ND_ Value const&    operator [] (usize i)                   C_NE___;


    private:
        ND_ bool _IsMemoryAliased (const Self* other)               C_NE___
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
    FixedSet<V,S,CS>::FixedSet (const Self &other) __NE___ : _count{ other._count }
    {
        ASSERT( not _IsMemoryAliased( &other ));
        CheckNothrow( IsNothrowDefaultCtor< V >);

        CPolicy_t::Copy( OUT _array, other._array, _count );
        MemCopy( OUT _indices, other._indices, SizeOf<Index_t> * _count );
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
        CheckNothrow( IsNothrowMoveCtor< V >);

        CPolicy_t::Replace( OUT _array, INOUT other._array, _count );
        MemCopy( OUT _indices, other._indices, SizeOf<Index_t> * _count );

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
        CheckNothrow( IsNothrowMoveCtor< V >);

        CPolicy_t::Destroy( INOUT _array, _count );

        _count = rhs._count;

        CPolicy_t::Replace( OUT _array, INOUT rhs._array, _count );
        MemCopy( OUT _indices, rhs._indices, SizeOf<Index_t> * _count );

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
    FixedSet<V,S,CS>&  FixedSet<V,S,CS>::operator = (const Self &rhs) __NE___
    {
        ASSERT( not _IsMemoryAliased( &rhs ));
        CheckNothrow( IsNothrowCopyCtor< V >);

        CPolicy_t::Destroy( INOUT _array, _count );

        _count = rhs._count;

        CPolicy_t::Copy( OUT _array, rhs._array, _count );
        MemCopy( OUT _indices, rhs._indices, SizeOf<Index_t> * _count );

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
    emplace
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename ValueType>
    Pair< typename FixedSet<V,S,CS>::iterator, bool >
        FixedSet<V,S,CS>::emplace (ValueType&& value) __NE___
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< ValueType, value_type, Index_t >;

        usize   i = BinarySearch::LowerBound( _count, value, _indices, _array );

        if_likely( i < _count and value == _array[_indices[i]] )
            return { iterator{ &_array[_indices[i]] }, false };

        // insert
        if_likely( _count < capacity() )
        {
            const usize j = _count++;
            PlacementNew<value_type>( OUT &_array[j], FwdArg<ValueType>(value) );

            if ( i < _count )
                for (usize k = _count-1; k > i; --k) {
                    _indices[k] = _indices[k-1];
                }
            else
                i = j;

            _indices[i] = Index_t(j);
            return { iterator{ &_array[j] }, true };
        }
        else
        {
            DBG_WARNING( "overflow" );
            return { null, false };
        }
    }

/*
=================================================
    insert_or_assign
=================================================
*/
    template <typename V, usize S, typename CS>
    template <typename ValueType>
    Pair< typename FixedSet<V,S,CS>::iterator, bool >
        FixedSet<V,S,CS>::insert_or_assign (ValueType&& value) __NE___
    {
        using BinarySearch = Base::_hidden_::RecursiveBinarySearch< ValueType, value_type, Index_t >;

        usize   i = BinarySearch::LowerBound( _count, value, _indices, _array );

        if_likely( i < _count and value == _array[_indices[i]] )
        {
            _array[_indices[i]] = FwdArg<ValueType>(value);
            return { iterator{ &_array[_indices[i]] }, false };
        }

        // insert
        if_likely( _count < capacity() )
        {
            const usize j = _count++;
            PlacementNew<value_type>( OUT &_array[j], FwdArg<ValueType>(value) );

            if ( i < _count )
                for (usize k = _count-1; k > i; --k) {
                    _indices[k] = _indices[k-1];
                }
            else
                i = j;

            _indices[i] = Index_t(j);
            return { iterator{ &_array[j] }, true };
        }
        else
        {
            DBG_WARNING( "overflow" );
            return { null, false };
        }
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
            {
                CheckNothrow( IsNothrowMoveCtor< V >);
                CPolicy_t::Replace( OUT &_array[idx], INOUT &_array[_count], 1, True{"single mem block"} );
            }
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
        CPolicy_t::Destroy( INOUT _array, _count );
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

/*
=================================================
    IndexOf
=================================================
*/
    template <typename V, usize S, typename CS>
    usize  FixedSet<V,S,CS>::IndexOf (iterator it) C_NE___
    {
        ASSERT( it >= begin() and it < end() );
        return usize(it) - usize(begin());
    }
//-----------------------------------------------------------------------------


    template <typename V, usize S, typename CS>
    struct TMemCopyAvailable< FixedSet<V,S,CS> > {
        static constexpr bool  value = IsMemCopyAvailable<V>;
    };

    template <typename V, usize S, typename CS>
    struct TZeroMemAvailable< FixedSet<V,S,CS> > {
        static constexpr bool  value = IsZeroMemAvailable<V>;
    };

    template <typename V, usize S, typename CS>
    struct TTriviallyDestructible< FixedSet<V,S,CS> > {
        static constexpr bool  value = IsTriviallyDestructible<V>;
    };

} // AE::Base


template <typename Value, size_t ArraySize, typename CS>
struct std::hash< AE::Base::FixedSet<Value, ArraySize, CS> >
{
    ND_ size_t  operator () (const AE::Base::FixedSet<Value, ArraySize, CS> &value) C_NE___
    {
        return size_t(value.CalcHash());
    }
};
