// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

	//
	// Mutable Array View
	//

	template <typename T>
	struct MutableArrayView
	{
		StaticAssert( not IsConst<T> );

	// types
	public:
		using value_type		= T;
		using iterator			= T *;
		using const_iterator	= T const *;


	// variables
	private:
		union {
			T *		_array;
			T		(*_dbgView)[400];		// debug viewer, don't use this field!
		};
		usize		_count	= 0;


	// methods
	public:
		constexpr MutableArrayView ()									__NE___ : _array{null} {}
		constexpr MutableArrayView (T* ptr, usize count)				__NE___ : _array{ptr}, _count{count}  {	ASSERT( (_count == 0) or (_array != null) ); }
		constexpr MutableArrayView (T* begin, T* end)					__NE___ : _array{begin}, _count{usize(std::distance( begin, end ))}  { ASSERT( begin <= end ); }

		template <typename AllocT>
		constexpr MutableArrayView (Array<T,AllocT> &vec)				__NE___ : _array{vec.data()}, _count{vec.size()}  { ASSERT( (_count == 0) or (_array != null) ); }

		template <usize S>
		constexpr MutableArrayView (StaticArray<T,S> &arr)				__NE___ : _array{arr.data()}, _count{arr.size()} {}

		template <usize S>
		constexpr MutableArrayView (T (&arr)[S])						__NE___ : _array{arr}, _count{S} {}

		template <usize S, typename CP>
		constexpr MutableArrayView (FixedArray<T,S,CP> &arr)			__NE___ : _array{arr.data()}, _count{arr.size()} {}

		constexpr MutableArrayView (T &singleElement)					__NE___ : _array{&singleElement}, _count{1} {}

		ND_ explicit constexpr operator Array<T> ()						C_NE___	{ return Array<T>{ begin(), end() }; }
		ND_ constexpr operator ArrayView<T> ()							C_NE___	{ return _AV(); }

		ND_ constexpr usize				size ()							C_NE___	{ return _count; }
		ND_ constexpr bool				empty ()						C_NE___	{ return _count == 0; }
		ND_ constexpr T const *			data ()							C_NE___	{ return _array; }
		ND_ constexpr T *				data ()							__NE___	{ return _array; }

			constexpr void				resize (usize newSize)			__NE___	{ _count = Min( _count, newSize ); }

		ND_ constexpr T const &			operator [] (usize i)			C_NE___	{ ASSERT( i < _count );  return _array[i]; }
		ND_ constexpr T &				operator [] (usize i)			__NE___	{ ASSERT( i < _count );  return _array[i]; }

		ND_ constexpr iterator			begin ()						__NE___	{ return _array; }
		ND_ constexpr iterator			end ()							__NE___	{ return _array + _count; }

		ND_ constexpr const_iterator	begin ()						C_NE___	{ return _array; }
		ND_ constexpr const_iterator	end ()							C_NE___	{ return _array + _count; }

		ND_ constexpr T const&			front ()						C_NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T &				front ()						__NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T const&			back ()							C_NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }
		ND_ constexpr T &				back ()							__NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }

		ND_ constexpr bool  operator == (ArrayView<T> rhs)				C_NE___	{ return _AV() == rhs; }
		ND_ constexpr bool  operator >  (ArrayView<T> rhs)				C_NE___	{ return _AV() >  rhs; }
		ND_ constexpr bool  operator != (ArrayView<T> rhs)				C_NE___	{ return _AV() != rhs; }
		ND_ constexpr bool  operator <  (ArrayView<T> rhs)				C_NE___	{ return _AV() <  rhs; }
		ND_ constexpr bool  operator >= (ArrayView<T> rhs)				C_NE___	{ return _AV() >= rhs; }
		ND_ constexpr bool  operator <= (ArrayView<T> rhs)				C_NE___	{ return _AV() <= rhs; }

		ND_ constexpr bool  AllEqual (const T &rhs)						C_NE___	{ return _AV().AllEqual( rhs ); }
		ND_ constexpr bool  AllGreater (const T &rhs)					C_NE___	{ return _AV().AllGreater( rhs ); }
		ND_ constexpr bool  AllGreaterEqual (const T &rhs)				C_NE___	{ return _AV().AllGreaterEqual( rhs ); }
		ND_ constexpr bool  AllLess (const T &rhs)						C_NE___	{ return _AV().AllLess( rhs ); }
		ND_ constexpr bool  AllLessEqual (const T &rhs)					C_NE___	{ return _AV().AllLessEqual( rhs ); }

		ND_ constexpr usize  IndexOf (const_iterator it)				C_NE___	{ return _AV().IndexOf( it ); }

		ND_ constexpr ArrayView<T>			section (usize first, usize count)	C_NE___	{ return _AV().section( first, count ); }
		ND_ constexpr MutableArrayView<T>	section (usize first, usize count)	__NE___;

		template <typename R>
		ND_ constexpr EnableIf<IsTrivial<T> and IsTrivial<R>, MutableArrayView<R>>  Cast () __NE___;

		template <typename R>
		ND_ constexpr EnableIf<IsTrivial<T> and IsTrivial<R>, ArrayView<R>>  Cast () C_NE___ { return _AV().template Cast<R>(); }

	private:
		ND_ constexpr ArrayView<T>	_AV ()								C_NE___	{ return ArrayView<T>{ data(), size() }; }
	};


	template <typename T, typename A>
	MutableArrayView (Array<T,A>) -> MutableArrayView<T>;

	template <typename T, usize S>
	MutableArrayView (T (&)[S]) -> MutableArrayView<T>;


/*
=================================================
	section
=================================================
*/
	template <typename T>
	constexpr MutableArrayView<T>  MutableArrayView<T>::section (usize first, usize count) __NE___
	{
		return first < size() ?
				MutableArrayView<T>{ data() + first, Math::Min( size() - first, count )} :
				MutableArrayView<T>{};
	}

/*
=================================================
	Cast
=================================================
*/
	template <typename T>
	template <typename R>
	constexpr EnableIf<IsTrivial<T> and IsTrivial<R>, MutableArrayView<R>>  MutableArrayView<T>::Cast () __NE___
	{
		StaticAssert( alignof(R) >= alignof(T) );
		StaticAssert( sizeof(R) > sizeof(T) ? IsMultipleOf( sizeof(R), sizeof(T) ) : IsMultipleOf( sizeof(T), sizeof(R) ));

		return MutableArrayView<R>{ static_cast<R*>(static_cast<void *>( _array )), (_count * sizeof(T)) / sizeof(R) };
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< MutableArrayView<T> >	{ static constexpr bool  value = true; };
	template <typename T>	struct TZeroMemAvailable< MutableArrayView<T> >	{ static constexpr bool  value = true; };

} // AE::Base


template <typename T>
struct std::hash< AE::Base::MutableArrayView<T> >
{
	ND_ size_t  operator () (const AE::Base::MutableArrayView<T> &value) C_NE___
	{
		return std::hash< AE::Base::ArrayView<T> >{}( AE::Base::ArrayView<T>{ value });
	}
};
