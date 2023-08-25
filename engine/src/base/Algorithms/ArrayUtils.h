// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Containers/FixedArray.h"
#include "base/Containers/NtStringView.h"

namespace AE::Base
{

/*
=================================================
    CountOf
=================================================
*/
    template <typename T>
    ND_ forceinline constexpr usize  CountOf (T& value) __NE___
    {
        return std::size( value );
    }

    template <typename ...Types>
    ND_ forceinline constexpr usize  CountOf () __NE___
    {
        return sizeof... (Types);
    }

    template <typename T, usize I, typename Class>
    ND_ forceinline constexpr usize  CountOf (T (Class::*) [I]) __NE___
    {
        return I;
    }

    template <typename T, usize I, typename Class>
    ND_ forceinline constexpr usize  CountOf (StaticArray<T,I> Class::*) __NE___
    {
        return I;
    }

    template <usize I>
    ND_ forceinline constexpr usize  CountOf (const BitSet<I> &) __NE___
    {
        return I;
    }

/*
=================================================
    ArraySizeOf
=================================================
*/
    template <typename T, typename A>
    ND_ forceinline Bytes  ArraySizeOf (const Array<T,A> &arr) __NE___
    {
        return Bytes{ arr.size() * sizeof(T) };
    }

    template <typename T, usize S>
    ND_ forceinline Bytes  ArraySizeOf (const FixedArray<T,S> &arr) __NE___
    {
        return Bytes{ arr.size() * sizeof(T) };
    }

    template <typename T>
    ND_ forceinline Bytes  ArraySizeOf (const ArrayView<T> &arr) __NE___
    {
        return Bytes{ arr.size() * sizeof(T) };
    }

    template <typename T, usize S>
    ND_ forceinline constexpr Bytes  ArraySizeOf (const StaticArray<T,S> &) __NE___
    {
        return Bytes{ S * sizeof(T) };
    }

    template <typename T, usize S>
    ND_ forceinline constexpr Bytes  ArraySizeOf (const T (&)[S]) __NE___
    {
        return Bytes{ sizeof(T) * S };
    }

/*
=================================================
    StringSizeOf
=================================================
*/
    template <typename T, typename A>
    ND_ forceinline Bytes  StringSizeOf (const BasicString<T,A> &str) __NE___
    {
        return Bytes{ str.size() * sizeof(T) };
    }

    template <typename T>
    ND_ forceinline Bytes  StringSizeOf (BasicStringView<T> str) __NE___
    {
        return Bytes{ str.size() * sizeof(T) };
    }

    template <typename T>
    ND_ forceinline Bytes  StringSizeOf (const NtBasicStringView<T> &str) __NE___
    {
        return Bytes{ str.size() * sizeof(T) };
    }

/*
=================================================
    Distance
----
    rhs - lhs
=================================================
*/
    template <typename T>
    ND_ forceinline constexpr ssize  Distance (T *lhs, T *rhs) __NE___
    {
        return std::distance< T *>( lhs, rhs );
    }

    template <typename T>
    ND_ forceinline constexpr ssize  Distance (const T *lhs, T *rhs) __NE___
    {
        return std::distance< T const *>( lhs, rhs );
    }

    template <typename T>
    ND_ forceinline constexpr ssize  Distance (T *lhs, const T *rhs) __NE___
    {
        return std::distance< T const *>( lhs, rhs );
    }

/*
=================================================
    LowerBound (binary search)
=================================================
*/
    template <typename T, typename Key>
    ND_ forceinline usize  LowerBound (ArrayView<T> arr, const Key &key) __NE___
    {
        usize2  range { 0, arr.size() };

        for_likely(; range.x < range.y; )
        {
            usize   mid = range.x + ((range.y - range.x) >> 1);

            range = (key > arr[mid] ? usize2{mid + 1, range.y} : usize2{range.x, mid});
        }
        return range.x;
    }

    template <typename T, typename Key>
    ND_ forceinline usize  LowerBound2 (ArrayView<T> arr, const Key &key) __NE___
    {
        usize   i = LowerBound( arr, key );
        return (i < arr.size() and key == arr[i]) ? i : UMax;
    }

    template <typename T, typename Key>
    ND_ forceinline usize  LowerBound2 (const Array<T> &arr, const Key &key) __NE___
    {
        return LowerBound2( ArrayView<T>{arr}, key );
    }

/*
=================================================
    BinarySearch
=================================================
*/
    template <typename T, typename Key>
    ND_ forceinline usize  BinarySearch (ArrayView<T> arr, const Key &key) __NE___
    {
        ssize   left    = 0;
        ssize   right   = ssize(arr.size());

        for (; left < right; )
        {
            ssize   mid = left + ((right - left) >> 1);

            if ( key > arr[mid] )
                left = mid + 1;
            else
            if ( not (key == arr[mid]) )
                right = mid - 1;
            else
                return mid;
        }

        return (left < ssize(arr.size()) and key == arr[left]) ? usize(left) : UMax;
    }

    template <typename T, typename Key>
    ND_ forceinline usize  BinarySearch (const Array<T> &arr, const Key &key) __NE___
    {
        return BinarySearch( ArrayView<T>{arr}, key );
    }

/*
=================================================
    ExponentialSearch
=================================================
*/
    template <typename T, typename Key>
    ND_ forceinline usize  ExponentialSearch (ArrayView<T> arr, const Key &key) __NE___
    {
        if ( arr.empty() )
            return UMax;

        usize   left    = 0;
        usize   right   = arr.size();
        usize   bound   = 1;

        for (; bound < right and key > arr[bound]; bound *= 2)
        {}

        left  = bound >> 1;
        right = Min( bound+1, right );

        for (; left < right; )
        {
            usize   mid = left + ((right - left) >> 1);

            if ( key > arr[mid] )
                left = mid + 1;
            else
                right = mid;
        }

        return left < arr.size() and key == arr[left] ? left : UMax;
    }

    template <typename T, typename Key>
    ND_ forceinline usize  ExponentialSearch (const Array<T> &arr, const Key &key) __NE___
    {
        return ExponentialSearch( ArrayView<T>{arr}, key );
    }

/*
=================================================
    IsSorted
=================================================
*/
    template <typename Iter, typename Cmp>
    ND_ bool  IsSorted (Iter begin, Iter end, Cmp && fn) __NE___
    {
        //STATIC_ASSERT( IsNothrowInvocable<Cmp> );

        if ( begin == end )
            return true;

        for (auto curr = begin, next = begin+1; next != end; ++next)
        {
            if_unlikely( not fn( *curr, *next ))
                return false;

            curr = next;
        }
        return true;
    }

    template <typename Iter>
    ND_ bool  IsSorted (Iter begin, Iter end) __NE___
    {
        return IsSorted( begin, end, std::less{} );
    }

/*
=================================================
    HasDuplicates
=================================================
*/
    template <typename Iter>
    ND_ bool  HasDuplicates (Iter begin, Iter end) __NE___
    {
        if ( begin == end )
            return false;

        for (auto curr = begin, next = begin+1; next != end; ++next)
        {
            if_unlikely( *curr == *next )
                return true;

            curr = next;
        }
        return false;
    }

/*
=================================================
    RemoveDuplicates
=================================================
*/
    template <typename T>
    void  RemoveDuplicates (INOUT Array<T> &arr) __NE___
    {
        std::sort( arr.begin(), arr.end() );
        arr.erase( std::unique( arr.begin(), arr.end() ), arr.end() );
    }

    template <typename T, typename Compare>
    void  RemoveDuplicates (INOUT Array<T> &arr, Compare comp) __NE___
    {
        STATIC_ASSERT( IsNothrowInvocable<Compare> );

        std::sort( arr.begin(), arr.end(), comp );
        arr.erase( std::unique( arr.begin(), arr.end() ), arr.end() );
    }

/*
=================================================
    ArrayContains
----
    for non-sorted arrays
=================================================
*/
    template <typename Iter, typename T>
    ND_ bool  ArrayContains (Iter begin, Iter end, const T &value) __NE___
    {
        return  begin != end                        and // empty container, will return true otherwise
                std::find( begin, end, value ) != end;
    }

    template <typename A, typename T>
    ND_ bool  ArrayContains (ArrayView<A> arr, const T &value) __NE___
    {
        return ArrayContains( arr.begin(), arr.end(), value );
    }

/*
=================================================
    IndexOfArrayElement
----
    returns valid index or UMax
=================================================
*/
    template <typename Iter, typename T>
    ND_ usize  IndexOfArrayElement (Iter begin, Iter end, const T &value) __NE___
    {
        auto    it = std::find( begin, end, value );
        return  it != end ?
                    usize(std::distance( begin, it )) :
                    usize(UMax);
    }

    template <typename Container, typename T>
    ND_ usize  IndexOfArrayElement (const Container &container, const T &value) __NE___
    {
        return IndexOfArrayElement( container.begin(), container.end(), value );
    }

/*
=================================================
    IsFirstElement / IsLastElement
=================================================
*/
    template <typename T, typename Container>
    ND_ bool  IsFirstElement (const T &value, const Container &container) __NE___
    {
        ASSERT( container.empty() or
                ((&value >= container.data()) and (&value < container.data()+container.size())) );

        return  not container.empty()   and
                container.data() == &value;
    }

    template <typename T, typename Container>
    ND_ bool  IsLastElement (const T &value, const Container &container) __NE___
    {
        ASSERT( container.empty() or
                ((&value >= container.data()) and (&value < container.data()+container.size())) );

        return  not container.empty()   and
                container.data() + container.size()-1 == &value;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Reverse
----
    iterate array from back to front
=================================================
*/
    namespace _hidden_
    {
        template <typename Container>
        class ReverseContainerView
        {
        private:
            Container &     _container;

        public:
            explicit ReverseContainerView (Container& container)    __NE___ : _container{container} {}

            ND_ auto  begin ()                                      __NE___ { return std::rbegin( _container ); }
            ND_ auto  end ()                                        __NE___ { return std::rend( _container ); }
        };

    } // _hidden_

    template <typename Container>
    ND_ auto  Reverse (Container& container) __NE___
    {
        return Base::_hidden_::ReverseContainerView<Container>{ container };
    }

    template <typename Container>
    ND_ auto  Reverse (const Container& container) __NE___
    {
        return Base::_hidden_::ReverseContainerView<const Container>{ container };
    }

/*
=================================================
    IndicesOnly
=================================================
*/
    namespace _hidden_
    {
        struct IndicesOnly_End
        {
            usize   _size;

            explicit IndicesOnly_End (usize s)      __NE___ : _size{s} {}
        };

        struct IndicesOnly_Iter
        {
            using Self = IndicesOnly_Iter;

            usize   _index;

            explicit IndicesOnly_Iter (usize i)                 __NE___ : _index{i} {}

                Self&   operator = (const Self &)               __NE___ = default;
                Self&   operator = (Self &&)                    __NE___ = default;

            ND_ bool    operator != (const Self &rhs)           C_NE___ { return _index != rhs._index; }
            ND_ bool    operator == (const Self &rhs)           C_NE___ { return _index == rhs._index; }

            ND_ bool    operator != (const IndicesOnly_End &rhs)C_NE___ { return _index < rhs._size; }

                Self&   operator ++ ()                          __NE___ { ++_index;  return *this; }
                Self    operator ++ (int)                       __NE___ { return Self{_index++}; }
            ND_ usize   operator * ()                           __NE___ { return _index; }
        };


        class IndicesOnlyRange
        {
        private:
            const usize     _begin;
            const usize     _end;

        public:
            explicit IndicesOnlyRange (usize b, usize e)    __NE___ : _begin{b}, _end{e} {}

            ND_ IndicesOnly_Iter    begin ()                __NE___ { return IndicesOnly_Iter{ _begin }; }
            ND_ IndicesOnly_End     end ()                  __NE___ { return IndicesOnly_End{ _end }; }
        };

    } // _hidden_


    template <typename Container,
              typename = EnableIf< IsClass<Container> >
             >
    ND_ auto  IndicesOnly (const Container& container) __NE___
    {
        return Base::_hidden_::IndicesOnlyRange{ 0, container.size() };
    }

    ND_ inline auto  IndicesOnly (usize begin, usize end) __NE___
    {
        ASSERT( begin <= end );
        return Base::_hidden_::IndicesOnlyRange{ begin, end };
    }

    ND_ inline auto  IndicesOnly (usize count) __NE___
    {
        return Base::_hidden_::IndicesOnlyRange{ 0, count };
    }

/*
=================================================
    ReverseIndices
=================================================
*/
    namespace _hidden_
    {
        struct ReverseIndices_End
        {
            usize   _size;

            explicit ReverseIndices_End (usize s)   __NE___ : _size{s} {}
        };

        struct ReverseIndices_Iter
        {
            using Self = ReverseIndices_Iter;

            usize   _index;

            explicit ReverseIndices_Iter (usize i)                  __NE___ : _index{i} {}

                Self&   operator = (const Self &)                   __NE___ = default;
                Self&   operator = (Self &&)                        __NE___ = default;

            ND_ bool    operator != (const Self &rhs)               C_NE___ { return _index != rhs._index; }
            ND_ bool    operator == (const Self &rhs)               C_NE___ { return _index == rhs._index; }

            ND_ bool    operator != (const ReverseIndices_End &rhs) C_NE___ { return _index < rhs._size; }

                Self&   operator ++ ()                              __NE___ { --_index;  return *this; }
                Self    operator ++ (int)                           __NE___ { return Self{_index--}; }
            ND_ usize   operator * ()                               __NE___ { return _index; }
        };


        class ReverseIndicesRange
        {
        private:
            const usize     _begin;
            const usize     _end;

        public:
            explicit ReverseIndicesRange (usize b, usize e)     __NE___ : _begin{b}, _end{e} {}

            ND_ ReverseIndices_Iter     begin ()                __NE___ { return ReverseIndices_Iter{ _begin }; }
            ND_ ReverseIndices_End      end ()                  __NE___ { return ReverseIndices_End{ _end }; }
        };

    } // _hidden_


    template <typename Container,
              typename = EnableIf< IsClass<Container> >
             >
    ND_ auto  ReverseIndices (const Container& container) __NE___
    {
        return Base::_hidden_::ReverseIndicesRange{ container.size()-1, container.size() };
    }

    ND_ inline auto  ReverseIndices (usize count) __NE___
    {
        return Base::_hidden_::ReverseIndicesRange{ count-1, count };
    }

/*
=================================================
    WithIndex
=================================================
*/
    namespace _hidden_
    {
        template <typename Iter>
        struct WithIndex_Iter
        {
            using Self  = WithIndex_Iter<Iter>;

            Iter    _it;
            usize   _index;

            WithIndex_Iter (Iter it, usize idx)             __NE___ : _it{it}, _index{idx} {}

            ND_ bool    operator != (const Self &rhs)       C_NE___ { return _it != rhs._it; }
            ND_ bool    operator == (const Self &rhs)       C_NE___ { return _it == rhs._it; }

            ND_ auto    operator * ()                       __NE___ { return TupleRef{ &(*_it), &_index }; }

                Self&   operator ++ ()                      __NE___ { ++_it;  ++_index;  return *this; }
                Self    operator ++ (int)                   __NE___ { return Self{ ++_it, ++_index }; }
        };


        template <typename Container, typename Iter>
        class WithIndexContainerView
        {
        private:
            Container &     _container;

        public:
            explicit WithIndexContainerView (Container& container)  __NE___ : _container{container} {}

            ND_ auto    begin ()                                    __NE___ { return WithIndex_Iter<Iter>{ _container.begin(), 0 }; }
            ND_ auto    end ()                                      __NE___ { return WithIndex_Iter<Iter>{ _container.end(),   UMax }; }
        };

    } // _hidden_

    template <typename Container>
    ND_ auto  WithIndex (Container& container) __NE___
    {
        return Base::_hidden_::WithIndexContainerView< Container, typename Container::iterator >{ container };
    }

    template <typename Container>
    ND_ auto  WithIndex (const Container& container) __NE___
    {
        return Base::_hidden_::WithIndexContainerView< const Container, typename Container::const_iterator >{ container };
    }

/*
=================================================
    BitfieldIterate
----
    from low to high bit
=================================================
*/
    namespace _hidden_
    {
        struct BitfieldIterate_End
        {};

        template <typename T>
        struct BitfieldIterate_Iter
        {
        private:
            using Self  = BitfieldIterate_Iter<T>;
            using U     = ToUnsignedInteger<T>;

            T &     _bits;
            T       _current;   // may be invalid

        public:
            explicit BitfieldIterate_Iter (T& bits)         __NE___ : _bits{bits}, _current{ _ExtractBit( _bits )} {}

            ND_ bool    operator != (BitfieldIterate_End)   C_NE___ { return _bits != 0; }

            ND_ T       operator * ()                       C_NE___ { return _current; }

                Self&   operator ++ ()                      __NE___ { _bits = T(U(_bits) & ~U(_current));  _current = _ExtractBit( _bits );  return *this; }

        private:
            ND_ static T  _ExtractBit (T bits)              __NE___ { return T( U(bits) & ~(U(bits) - U{1}) ); }
        };

        template <typename T>
        struct BitfieldIterateView
        {
        private:
            T   _bits;

        public:
            explicit BitfieldIterateView (T bits)   __NE___ : _bits{bits} {}

            ND_ auto    begin ()                    __NE___ { return BitfieldIterate_Iter<T>{ _bits }; }
            ND_ auto    end ()                      __NE___ { return BitfieldIterate_End{}; }

        };

    } // _hidden_

    template <typename T>
    ND_ auto  BitfieldIterate (const T &bits) __NE___
    {
        STATIC_ASSERT( IsEnum<T> or IsUnsignedInteger<T> );
        return Base::_hidden_::BitfieldIterateView<T>{ bits };
    }


} // AE::Base
