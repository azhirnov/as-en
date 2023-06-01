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
		usize2	range { 0, arr.size() };

		for_likely(; range.x < range.y; )
		{
			usize	mid = range.x + ((range.y - range.x) >> 1);

			range = (key > arr[mid] ? usize2{mid + 1, range.y} : usize2{range.x, mid});
		}
		return range.x;
	}

	template <typename T, typename Key>
	ND_ forceinline usize  LowerBound2 (ArrayView<T> arr, const Key &key) __NE___
	{
		usize	i = LowerBound( arr, key );
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
		ssize	left	= 0;
		ssize	right	= ssize(arr.size());

		for (; left < right; )
		{
			ssize	mid = left + ((right - left) >> 1);

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

		usize	left	= 0;
		usize	right	= arr.size();
		usize	bound	= 1;

		for (; bound < right and key > arr[bound]; bound *= 2)
		{}

		left  = bound >> 1;
		right = Min( bound+1, right );

		for (; left < right; )
		{
			usize	mid = left + ((right - left) >> 1);

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
	Reverse
=================================================
*/
	namespace _hidden_
	{
		template <typename Container>
		class ReverseContainerView
		{
		private:
			Container &		_container;

		public:
			explicit ReverseContainerView (Container& container)	__NE___	: _container{container} {}

			ND_ auto  begin ()										__NE___	{ return std::rbegin( _container ); }
			ND_ auto  end ()										__NE___	{ return std::rend( _container ); }
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
		return	begin != end						and	// empty container, will return true otherwise
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
=================================================
*/
	template <typename Iter, typename T>
	ND_ usize  IndexOfArrayElement (Iter begin, Iter end, const T &value)
	{
		auto	it = std::find( begin, end, value );
		return	it != end ?
					usize(std::distance( begin, it )) :
					usize(UMax);
	}
	
	template <typename A, typename T>
	ND_ usize  IndexOfArrayElement (ArrayView<A> arr, const T &value)
	{
		return IndexOfArrayElement( arr.begin(), arr.end(), value );
	}


} // AE::Base
