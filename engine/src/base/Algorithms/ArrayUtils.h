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
	ND_ forceinline constexpr usize  CountOf (T& value)
	{
		return std::size( value );
	}
	
	template <typename ...Types>
	ND_ forceinline constexpr usize  CountOf ()
	{
		return sizeof... (Types);
	}
	
	template <typename T, usize I, typename Class>
	ND_ forceinline constexpr usize  CountOf (T (Class::*) [I])
	{
		return I;
	}
	
	template <usize I>
	ND_ forceinline constexpr usize  CountOf (const BitSet<I> &)
	{
		return I;
	}

/*
=================================================
	ArraySizeOf
=================================================
*/
	template <typename T, typename A>
	ND_ forceinline Bytes  ArraySizeOf (const Array<T,A> &arr)
	{
		return Bytes{ arr.size() * sizeof(T) };
	}

	template <typename T, usize S>
	ND_ forceinline Bytes  ArraySizeOf (const FixedArray<T,S> &arr)
	{
		return Bytes{ arr.size() * sizeof(T) };
	}

	template <typename T>
	ND_ forceinline Bytes  ArraySizeOf (const ArrayView<T> &arr)
	{
		return Bytes{ arr.size() * sizeof(T) };
	}
	
	template <typename T, usize S>
	ND_ forceinline constexpr Bytes  ArraySizeOf (const StaticArray<T,S> &)
	{
		return Bytes{ S * sizeof(T) };
	}

	template <typename T, usize S>
	ND_ forceinline constexpr Bytes  ArraySizeOf (const T (&)[S])
	{
		return Bytes{ sizeof(T) * S };
	}
	
/*
=================================================
	StringSizeOf
=================================================
*/
	template <typename T, typename A>
	ND_ forceinline Bytes  StringSizeOf (const BasicString<T,A> &str)
	{
		return Bytes{ str.size() * sizeof(T) };
	}

	template <typename T>
	ND_ forceinline Bytes  StringSizeOf (BasicStringView<T> str)
	{
		return Bytes{ str.size() * sizeof(T) };
	}

	template <typename T>
	ND_ forceinline Bytes  StringSizeOf (const NtBasicStringView<T> &str)
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
	ND_ forceinline constexpr ssize  Distance (T *lhs, T *rhs)
	{
		return std::distance< T *>( lhs, rhs );
	}
	
	template <typename T>
	ND_ forceinline constexpr ssize  Distance (const T *lhs, T *rhs)
	{
		return std::distance< T const *>( lhs, rhs );
	}
	
	template <typename T>
	ND_ forceinline constexpr ssize  Distance (T *lhs, const T *rhs)
	{
		return std::distance< T const *>( lhs, rhs );
	}
	
/*
=================================================
	LowerBound (binary search)
=================================================
*/
	template <typename T, typename Key>
	ND_ forceinline usize  LowerBound (ArrayView<T> arr, const Key &key)
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
	ND_ forceinline usize  LowerBound2 (ArrayView<T> arr, const Key &key)
	{
		usize	i = LowerBound( arr, key );
		return (i < arr.size() and key == arr[i]) ? i : UMax;
	}

	template <typename T, typename Key>
	ND_ forceinline usize  LowerBound2 (const Array<T> &arr, const Key &key)
	{
		return LowerBound2( ArrayView<T>{arr}, key );
	}
	
/*
=================================================
	BinarySearch
=================================================
*/
	template <typename T, typename Key>
	ND_ forceinline usize  BinarySearch (ArrayView<T> arr, const Key &key)
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
	ND_ forceinline usize  BinarySearch (const Array<T> &arr, const Key &key)
	{
		return BinarySearch( ArrayView<T>{arr}, key );
	}
	
/*
=================================================
	ExponentialSearch
=================================================
*/
	template <typename T, typename Key>
	ND_ forceinline usize  ExponentialSearch (ArrayView<T> arr, const Key &key)
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
	ND_ forceinline usize  ExponentialSearch (const Array<T> &arr, const Key &key)
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
			explicit ReverseContainerView (Container& container) : _container{container} {}

			ND_ auto  begin ()	{ return std::rbegin( _container ); }
			ND_ auto  end ()	{ return std::rend( _container ); }
		};

	}	// _hidden_

	template <typename Container>
	ND_ auto  Reverse (Container& container)
	{
		return Base::_hidden_::ReverseContainerView<Container>{ container };
	}

	template <typename Container>
	ND_ auto  Reverse (const Container& container)
	{
		return Base::_hidden_::ReverseContainerView<const Container>{ container };
	}
	
/*
=================================================
	IsSorted
=================================================
*/
	template <typename Iter, typename Cmp>
	ND_ bool  IsSorted (Iter begin, Iter end, Cmp && fn)
	{
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
	ND_ bool  IsSorted (Iter begin, Iter end)
	{
		return IsSorted( begin, end, std::less{} );
	}
	
/*
=================================================
	HasDuplicates
=================================================
*/
	template <typename Iter>
	ND_ bool  HasDuplicates (Iter begin, Iter end)
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
	void  RemoveDuplicates (INOUT Array<T> &arr)
	{
		std::sort( arr.begin(), arr.end() );
		arr.erase( std::unique( arr.begin(), arr.end() ), arr.end() );
	}

}	// AE::Base
