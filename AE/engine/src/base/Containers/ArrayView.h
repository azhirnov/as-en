// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Byte.h"

namespace AE::Base
{

	//
	// Array View
	//

	template <typename T>
	struct ArrayView
	{
	// types
	public:
		using value_type		= RemoveConst<T>;
		using iterator			= T const *;
		using const_iterator	= T const *;

		struct reverse_iterator
		{
		private:
			T const*	_ptr;

		public:
			explicit reverse_iterator (T const* ptr)		__NE___ : _ptr{ptr} {}

			reverse_iterator&	operator ++ ()				__NE___	{ --_ptr;		return *this; }
			reverse_iterator	operator += (usize rhs)		__NE___	{ _ptr -= rhs;	return *this; }

			ND_ T const&		operator * ()				C_NE___	{ return *_ptr; }

			ND_ bool	operator == (reverse_iterator rhs)	C_NE___	{ return _ptr == rhs._ptr; }
			ND_ bool	operator != (reverse_iterator rhs)	C_NE___	{ return _ptr != rhs._ptr; }
			ND_ bool	operator <  (reverse_iterator rhs)	C_NE___	{ return _ptr >  rhs._ptr; }
			ND_ bool	operator >  (reverse_iterator rhs)	C_NE___	{ return _ptr <  rhs._ptr; }
			ND_ bool	operator <= (reverse_iterator rhs)	C_NE___	{ return _ptr >= rhs._ptr; }
			ND_ bool	operator >= (reverse_iterator rhs)	C_NE___	{ return _ptr <= rhs._ptr; }
		};
		using const_reverse_iterator = reverse_iterator;


	// variables
	private:
		union {
			T const *	_array;
			T const		(*_dbgView)[400];		// debug viewer, don't use this field!
		};
		usize		_count	= 0;


	// methods
	public:
		constexpr ArrayView ()									__NE___ : _array{null} {}
		constexpr ArrayView (T const* ptr, usize count)			__NE___ : _array{ptr}, _count{count}  {	ASSERT( (_count == 0) or (_array != null) ); }
		constexpr ArrayView (T const* begin, T const* end)		__NE___ : _array{begin}, _count{usize(std::distance( begin, end ))}  { ASSERT( begin <= end ); }
		constexpr ArrayView (value_type &elem)					__NE___ : _array{&elem}, _count{1} {}
		constexpr ArrayView (value_type &&elem)					__NE___ : _array{&elem}, _count{1} {}


		template <typename AllocT>
		constexpr ArrayView (const Array<T,AllocT> &vec)		__NE___ : _array{vec.data()}, _count{vec.size()}  { ASSERT( (_count == 0) or (_array != null) ); }

		template <usize S>
		constexpr ArrayView (const StaticArray<T,S> &arr)		__NE___ : _array{arr.data()}, _count{arr.size()} {}

		template <usize S>
		constexpr ArrayView (const T (&arr)[S])					__NE___ : _array{arr}, _count{S} {}

		ND_ explicit constexpr operator Array<T> ()				C_NE___	{ return Array<T>{ begin(), end() }; }

		ND_ constexpr usize				size ()					C_NE___	{ return _count; }
		ND_ constexpr bool				empty ()				C_NE___	{ return _count == 0; }
		ND_ constexpr T const *			data ()					C_NE___	{ return _array; }

		ND_ constexpr T const &			operator [] (usize i)	C_NE___	{ ASSERT( i < _count );  return _array[i]; }

		ND_ constexpr const_iterator	begin ()				C_NE___	{ return _array; }
		ND_ constexpr const_iterator	end ()					C_NE___	{ return _array + _count; }

		ND_ constexpr auto				rbegin ()				C_NE___	{ return reverse_iterator{_array + _count-1}; }
		ND_ constexpr auto				rend ()					C_NE___	{ return reverse_iterator{_array - 1}; }

		ND_ constexpr T const&			front ()				C_NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T const&			back ()					C_NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }

		ND_ constexpr bool  operator == (ArrayView<T> rhs)		C_NE___;
		ND_ constexpr bool  operator >  (ArrayView<T> rhs)		C_NE___;
		ND_ constexpr bool  operator != (ArrayView<T> rhs)		C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool  operator <  (ArrayView<T> rhs)		C_NE___	{ return (rhs > *this); }
		ND_ constexpr bool  operator >= (ArrayView<T> rhs)		C_NE___	{ return not (*this < rhs); }
		ND_ constexpr bool  operator <= (ArrayView<T> rhs)		C_NE___	{ return not (*this > rhs); }

		ND_ constexpr bool  AllEqual (const T &rhs)				C_NE___	{ return _All( rhs, std::equal_to<T>{} ); }
		ND_ constexpr bool  AllGreater (const T &rhs)			C_NE___	{ return _All( rhs, std::greater<T>{} ); }
		ND_ constexpr bool  AllGreaterEqual (const T &rhs)		C_NE___	{ return _All( rhs, std::greater_equal<T>{} ); }
		ND_ constexpr bool  AllLess (const T &rhs)				C_NE___	{ return _All( rhs, std::less<T>{} ); }
		ND_ constexpr bool  AllLessEqual (const T &rhs)			C_NE___	{ return _All( rhs, std::less_equal<T>{} ); }

		ND_ constexpr usize  IndexOf (const_iterator it)		C_NE___;

		ND_ constexpr ArrayView<T> section (usize first, usize count) C_NE___;

		template <typename R>
		ND_ constexpr EnableIf<IsTrivial<R>, ArrayView<R>>  Cast () C_NE___;


	private:
		template <typename Op>
		ND_ constexpr bool  _All (const T &rhs, const Op &op)	C_NE___;
	};



	//
	// Initializer List (! on stack !)
	//

	template <typename T>
	struct List
	{
	// variables
	private:
		ArrayView<T>	_view;

	// methods
	public:
		constexpr List ()										__NE___	{}
		constexpr List (std::initializer_list<T> list)			__NE___	: _view{ list.begin(), list.end() } {}

		ND_ constexpr operator ArrayView<T> ()					C_NE___	{ return _view; }

		ND_ constexpr const T*	begin ()						C_NE___	{ return _view.begin(); }
		ND_ constexpr const T*	end ()							C_NE___	{ return _view.end(); }
		ND_ constexpr usize		size ()							C_NE___	{ return _view.size(); }

		ND_ constexpr bool  operator == (ArrayView<T> rhs)		C_NE___	{ return _view == rhs; }
		ND_ constexpr bool  operator != (ArrayView<T> rhs)		C_NE___	{ return _view != rhs; }
		ND_ constexpr bool  operator >  (ArrayView<T> rhs)		C_NE___	{ return _view >  rhs; }
		ND_ constexpr bool  operator <  (ArrayView<T> rhs)		C_NE___	{ return _view <  rhs; }
		ND_ constexpr bool  operator >= (ArrayView<T> rhs)		C_NE___	{ return _view >= rhs; }
		ND_ constexpr bool  operator <= (ArrayView<T> rhs)		C_NE___	{ return _view <= rhs; }
	};


	template <typename T, typename A>
	ArrayView (Array<T,A>) -> ArrayView<T>;

	template <typename T, usize S>
	ArrayView (T (&)[S]) -> ArrayView<T>;

	template <typename T, usize S>
	ArrayView (const T (&)[S]) -> ArrayView<T>;

	template <typename T>
	ArrayView (const T &) -> ArrayView<T>;

	template <typename T>
	ArrayView (List<T>) -> ArrayView<T>;

	template <typename A0, typename ...Args>
	List (A0, A0, Args...) -> List<A0>;

/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	constexpr bool  ArrayView<T>::operator == (ArrayView<T> rhs) C_NE___
	{
		if ( (_array == rhs._array) and (_count == rhs._count) )
			return true;

		if ( size() != rhs.size() )
			return false;

		for (usize i = 0; i < size(); ++i)
		{
			if_unlikely( not Math::All( _array[i] == rhs[i] ))
				return false;
		}
		return true;
	}

/*
=================================================
	operator >
=================================================
*/
	template <typename T>
	constexpr bool  ArrayView<T>::operator >  (ArrayView<T> rhs) C_NE___
	{
		if ( size() != rhs.size() )
			return size() > rhs.size();

		for (usize i = 0; i < size(); ++i)
		{
			if_unlikely( not Math::All( _array[i] == rhs[i] ))
				return _array[i] > rhs[i];
		}
		return true;
	}

/*
=================================================
	_All
=================================================
*/
	template <typename T>
	template <typename Op>
	constexpr bool  ArrayView<T>::_All (const T &rhs, const Op &op) C_NE___
	{
		for (usize i = 0; i < size(); ++i) {
			if_unlikely( not op( _array[i], rhs ))
				return false;
		}
		return not empty();
	}

/*
=================================================
	section
=================================================
*/
	template <typename T>
	constexpr ArrayView<T>  ArrayView<T>::section (usize first, usize count) C_NE___
	{
		return first < size() ?
				ArrayView<T>{ data() + first, Math::Min( size() - first, count )} :
				ArrayView<T>{};
	}

/*
=================================================
	Cast
=================================================
*/
	template <typename T>
	template <typename R>
	constexpr EnableIf<IsTrivial<R>, ArrayView<R>>  ArrayView<T>::Cast () C_NE___
	{
		StaticAssert( IsTrivial<T> );
		StaticAssert( alignof(R) >= alignof(T) );
		StaticAssert( sizeof(R) > sizeof(T) ? IsMultipleOf( sizeof(R), sizeof(T) ) : IsMultipleOf( sizeof(T), sizeof(R) ));

		return ArrayView<R>{ static_cast<const R*>(static_cast<const void *>( _array )), (_count * sizeof(T)) / sizeof(R) };
	}

/*
=================================================
	IndexOf
=================================================
*/
	template <typename T>
	constexpr usize  ArrayView<T>::IndexOf (const_iterator it) C_NE___
	{
		ASSERT( it >= begin() and it < end() );
		return it - begin();
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< ArrayView<T> >	{ static constexpr bool  value = true; };
	template <typename T>	struct TZeroMemAvailable< ArrayView<T> >	{ static constexpr bool  value = true; };

} // AE::Base


template <typename T>
struct std::hash< AE::Base::ArrayView<T> >
{
	ND_ size_t  operator () (const AE::Base::ArrayView<T> &value) C_NE___
	{
		if constexpr( AE_FAST_HASH and AE::Base::IsTrivial<T> )
		{
			return size_t(AE::Base::HashOf( value.data(), value.size() * sizeof(T) ));
		}
		else
		{
			AE::Base::HashVal	result = AE::Base::HashOf( value.size() );

			for (auto& item : value) {
				result << AE::Base::HashOf( item );
			}
			return size_t(result);
		}
	}
};

template <typename T>
struct std::hash< std::vector<T> >
{
	ND_ size_t  operator () (const vector<T> &value) C_NE___
	{
		return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
	}
};

template <typename T, size_t S>
struct std::hash< std::array<T,S> >
{
	ND_ size_t  operator () (const array<T,S> &value) C_NE___
	{
		return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
	}
};
