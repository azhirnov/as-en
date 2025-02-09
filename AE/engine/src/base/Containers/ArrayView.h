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
		__Cx__ ArrayView ()									__NE___ : _array{null} {}
		__Cx__ ArrayView (T const* ptr, usize count)		__NE___ : _array{ptr}, _count{count}  {	ASSERT_Cx( (_count == 0) or (_array != null) ); }
		__Cx__ ArrayView (T const* begin, T const* end)		__NE___ : _array{begin}, _count{usize(std::distance( begin, end ))}  { ASSERT_Cx( begin <= end ); }
		__Cx__ ArrayView (value_type &elem)					__NE___ : _array{&elem}, _count{1} {}
		__Cx__ ArrayView (value_type &&elem)				__NE___ : _array{&elem}, _count{1} {}

		template <typename AllocT>
		__Cz__ ArrayView (const Array<T,AllocT> &vec)		__NE___ : _array{vec.data()}, _count{vec.size()}  { ASSERT( (_count == 0) or (_array != null) ); }

		template <usize S>
		__Cx__ ArrayView (const StaticArray<T,S> &arr)		__NE___ : _array{arr.data()}, _count{arr.size()} {}

		template <usize S>
		__Cx__ ArrayView (const T (&arr)[S])				__NE___ : _array{arr}, _count{S} {}

		NdCx__ explicit operator Array<T> ()				C_NE___	{ return Array<T>{ begin(), end() }; }

		NdCx__ usize			size ()						C_NE___	{ return _count; }
		NdCx__ bool				empty ()					C_NE___	{ return _count == 0; }
		NdCx__ T const *		data ()						C_NE___	{ return _array; }

		NdCx__ Bytes			DataSize ()					C_NE___	{ return Bytes{ sizeof(T) * _count }; }

		NdCz__ T const &		operator [] (usize i)		C_NE___	{ ASSERT( i < _count );  return _array[i]; }

		NdCx__ const_iterator	begin ()					C_NE___	{ return _array; }
		NdCx__ const_iterator	end ()						C_NE___	{ return _array + _count; }

		NdCx__ auto				rbegin ()					C_NE___	{ return reverse_iterator{_array + _count-1}; }
		NdCx__ auto				rend ()						C_NE___	{ return reverse_iterator{_array - 1}; }

		NdCz__ T const&			front ()					C_NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		NdCz__ T const&			back ()						C_NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }

		NdCx__ bool  operator == (ArrayView<T> rhs)			C_NE___;
		NdCx__ bool  operator >  (ArrayView<T> rhs)			C_NE___;
		NdCx__ bool  operator != (ArrayView<T> rhs)			C_NE___	{ return not (*this == rhs); }
		NdCx__ bool  operator <  (ArrayView<T> rhs)			C_NE___	{ return (rhs > *this); }
		NdCx__ bool  operator >= (ArrayView<T> rhs)			C_NE___	{ return not (*this < rhs); }
		NdCx__ bool  operator <= (ArrayView<T> rhs)			C_NE___	{ return not (*this > rhs); }

		NdCx__ bool  AllEqual (const T &rhs)				C_NE___	{ return _All( rhs, std::equal_to<T>{} ); }
		NdCx__ bool  AllGreater (const T &rhs)				C_NE___	{ return _All( rhs, std::greater<T>{} ); }
		NdCx__ bool  AllGreaterEqual (const T &rhs)			C_NE___	{ return _All( rhs, std::greater_equal<T>{} ); }
		NdCx__ bool  AllLess (const T &rhs)					C_NE___	{ return _All( rhs, std::less<T>{} ); }
		NdCx__ bool  AllLessEqual (const T &rhs)			C_NE___	{ return _All( rhs, std::less_equal<T>{} ); }

		NdCz__ usize  IndexOf (const_iterator it)			C_NE___;

		NdCx__ ArrayView<T> section (usize first, usize count) C_NE___;

		template <typename R>
		NdCx__ EnableIf<IsTrivial<R>, ArrayView<R>>  Cast () C_NE___;


	private:
		template <typename Op>
		NdCx__ bool  _All (const T &rhs, const Op &op)	C_NE___;
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
		__Cx__ List ()									__NE___	{}
		__Cx__ List (std::initializer_list<T> list)		__NE___	: _view{ list.begin(), list.end() } {}

		NdCx__ operator ArrayView<T> ()					C_NE___	{ return _view; }

		NdCx__ const T*		begin ()					C_NE___	{ return _view.begin(); }
		NdCx__ const T*		end ()						C_NE___	{ return _view.end(); }
		NdCx__ usize		size ()						C_NE___	{ return _view.size(); }

		NdCx__ T const&		operator [] (usize idx)		C_NE___	{ return _view[idx]; }

		NdCx__ bool  operator == (ArrayView<T> rhs)		C_NE___	{ return _view == rhs; }
		NdCx__ bool  operator != (ArrayView<T> rhs)		C_NE___	{ return _view != rhs; }
		NdCx__ bool  operator >  (ArrayView<T> rhs)		C_NE___	{ return _view >  rhs; }
		NdCx__ bool  operator <  (ArrayView<T> rhs)		C_NE___	{ return _view <  rhs; }
		NdCx__ bool  operator >= (ArrayView<T> rhs)		C_NE___	{ return _view >= rhs; }
		NdCx__ bool  operator <= (ArrayView<T> rhs)		C_NE___	{ return _view <= rhs; }
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
	__Cx__ bool  ArrayView<T>::operator == (ArrayView<T> rhs) C_NE___
	{
		if ( (_array == rhs._array) and (_count == rhs._count) )
			return true;

		if ( size() != rhs.size() )
			return false;

		for (usize i = 0; i < size(); ++i)
		{
			if_unlikely( not Base::All( _array[i] == rhs[i] ))
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
	__Cx__ bool  ArrayView<T>::operator >  (ArrayView<T> rhs) C_NE___
	{
		if ( size() != rhs.size() )
			return size() > rhs.size();

		for (usize i = 0; i < size(); ++i)
		{
			if_unlikely( not Base::All( _array[i] == rhs[i] ))
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
	__Cx__ bool  ArrayView<T>::_All (const T &rhs, const Op &op) C_NE___
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
	__Cx__ ArrayView<T>  ArrayView<T>::section (usize first, usize count) C_NE___
	{
		return first < size() ?
				ArrayView<T>{ data() + first, Base::Min( size() - first, count )} :
				ArrayView<T>{};
	}

/*
=================================================
	Cast
=================================================
*/
	template <typename T>
	template <typename R>
	__Cx__ EnableIf<IsTrivial<R>, ArrayView<R>>  ArrayView<T>::Cast () C_NE___
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
	__Cz__ usize  ArrayView<T>::IndexOf (const_iterator it) C_NE___
	{
		ASSERT( it >= begin() and it < end() );
		return it - begin();
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< ArrayView<T> >	: CT_True {};
	template <typename T>	struct TZeroMemAvailable< ArrayView<T> >	: CT_True {};

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
