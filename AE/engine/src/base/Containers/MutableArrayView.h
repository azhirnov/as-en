// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

	//
	// Mutable Array View
	//
	
	template <typename T, typename IndexT = usize>
	struct MutableArrayView
	{
		StaticAssert( not IsConst<T> );

	// types
	public:
		using value_type		= T;
		using iterator			= T *;
		using const_iterator	= T const *;
		using Index_t			= IndexT;
		using Self				= MutableArrayView< T, IndexT >;
		using View_t			= ArrayView< T, IndexT >;


	// variables
	private:
		union {
			T *		_array;
			T		(*_dbgView)[400];		// debug viewer, don't use this field!
		};
		usize		_count	= 0;


	// methods
	public:
		__Cx__ MutableArrayView ()								__NE___ : _array{null} {}
		__Cx__ MutableArrayView (T* ptr, usize count)			__NE___ : _array{ptr}, _count{count}  {	ASSERT( (_count == 0) or (_array != null) ); }
		__Cx__ MutableArrayView (T* begin, T* end)				__NE___ : _array{begin}, _count{usize(std::distance( begin, end ))}  { ASSERT( begin <= end ); }

		template <typename AllocT>
		__Cx__ MutableArrayView (Array<T,AllocT> &vec)			__NE___ : _array{vec.data()}, _count{vec.size()}  { ASSERT( (_count == 0) or (_array != null) ); }

		template <usize S>
		__Cx__ MutableArrayView (StaticArray<T,S> &arr)			__NE___ : _array{arr.data()}, _count{arr.size()} {}

		template <usize S>
		__Cx__ MutableArrayView (T (&arr)[S])					__NE___ : _array{arr}, _count{S} {}

		template <usize S, typename CP>
		__Cx__ MutableArrayView (FixedArray<T,S,CP> &arr)		__NE___ : _array{arr.data()}, _count{arr.size()} {}

		__Cx__ MutableArrayView (T &singleElement)				__NE___ : _array{&singleElement}, _count{1} {}

		NdCx__ explicit operator Array<T> ()					C_NE___	{ return Array<T>{ begin(), end() }; }
		NdCx__ operator View_t ()								C_NE___	{ return _AV(); }

		NdCx__ usize			size ()							C_NE___	{ return _count; }
		NdCx__ bool				empty ()						C_NE___	{ return _count == 0; }
		NdCx__ T const *		data ()							C_NE___	{ return _array; }
		NdCx__ T *				data ()							__NE___	{ return _array; }

		NdCx__ Bytes			DataSize ()						C_NE___	{ return Bytes{ sizeof(T) * _count }; }

		__Cx__ void				resize (usize newSize)			__NE___	{ ASSERT( newSize <= _count );  _count = Min( _count, newSize ); }

		NdCx__ T const &		operator [] (Index_t i)			C_NE___	{ ASSERT( usize(i) < _count );  return _array[usize(i)]; }
		NdCx__ T &				operator [] (Index_t i)			__NE___	{ ASSERT( usize(i) < _count );  return _array[usize(i)]; }

		NdCx__ iterator			begin ()						__NE___	{ return _array; }
		NdCx__ iterator			end ()							__NE___	{ return _array + _count; }

		NdCx__ const_iterator	begin ()						C_NE___	{ return _array; }
		NdCx__ const_iterator	end ()							C_NE___	{ return _array + _count; }

		NdCx__ T const&			front ()						C_NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		NdCx__ T &				front ()						__NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		NdCx__ T const&			back ()							C_NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }
		NdCx__ T &				back ()							__NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }

		NdCx__ bool  operator == (View_t rhs)					C_NE___	{ return _AV() == rhs; }
		NdCx__ bool  operator >  (View_t rhs)					C_NE___	{ return _AV() >  rhs; }
		NdCx__ bool  operator != (View_t rhs)					C_NE___	{ return _AV() != rhs; }
		NdCx__ bool  operator <  (View_t rhs)					C_NE___	{ return _AV() <  rhs; }
		NdCx__ bool  operator >= (View_t rhs)					C_NE___	{ return _AV() >= rhs; }
		NdCx__ bool  operator <= (View_t rhs)					C_NE___	{ return _AV() <= rhs; }

		NdCx__ bool  AllEqual (const T &rhs)					C_NE___	{ return _AV().AllEqual( rhs ); }
		NdCx__ bool  AllGreater (const T &rhs)					C_NE___	{ return _AV().AllGreater( rhs ); }
		NdCx__ bool  AllGreaterEqual (const T &rhs)				C_NE___	{ return _AV().AllGreaterEqual( rhs ); }
		NdCx__ bool  AllLess (const T &rhs)						C_NE___	{ return _AV().AllLess( rhs ); }
		NdCx__ bool  AllLessEqual (const T &rhs)				C_NE___	{ return _AV().AllLessEqual( rhs ); }

		NdCx__ usize  IndexOf (const_iterator it)				C_NE___	{ return _AV().IndexOf( it ); }

		NdCx__ View_t	section (usize first, usize count)		C_NE___	{ return _AV().section( first, count ); }
		NdCx__ Self		section (usize first, usize count)		__NE___;

		template <typename R> requires(IsTrivial<T> and IsTrivial<R>)
		NdCx__ MutableArrayView<R>  Cast ()						__NE___;

		template <typename R> requires(IsTrivial<T> and IsTrivial<R>)
		NdCx__ ArrayView<R>			Cast ()						C_NE___ { return _AV().template Cast<R>(); }

	private:
		NdCx__ View_t				_AV ()						C_NE___	{ return View_t{ data(), size() }; }
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
	template <typename T, typename I>
	__Cx__ MutableArrayView<T,I>  MutableArrayView<T,I>::section (usize first, usize count) __NE___
	{
		return first < size() ?
				Self{ data() + first, Base::Min( size() - first, count )} :
				Self{};
	}

/*
=================================================
	Cast
=================================================
*/
	template <typename T, typename I>
	template <typename R> requires(IsTrivial<T> and IsTrivial<R>)
	__Cx__ MutableArrayView<R>  MutableArrayView<T,I>::Cast () __NE___
	{
		StaticAssert( alignof(R) >= alignof(T) );
		StaticAssert( sizeof(R) > sizeof(T) ? IsMultipleOf( sizeof(R), sizeof(T) ) : IsMultipleOf( sizeof(T), sizeof(R) ));

		return MutableArrayView<R>{ static_cast<R*>(static_cast<void *>( _array )), (_count * sizeof(T)) / sizeof(R) };
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< MutableArrayView<T> >	: CT_True {};
	template <typename T>	struct TZeroMemAvailable< MutableArrayView<T> >	: CT_True {};


/*
=================================================
	ZeroMem, MemCopy, MemMove
=================================================
*/
	template <typename T, typename I>
	inline void  ZeroMem (MutableArrayView<T,I> arr) __NE___
	{
		StaticAssert( IsZeroMemAvailable<T> );

		if_likely( not arr.empty() )
			std::memset( OUT arr.data(), 0, usize{arr.DataSize()} );
	}

	template <typename T, typename I>
	void  MemCopy (OUT MutableArrayView<T,I> dst, ArrayView<T> src) __NE___
	{
		MemCopy( OUT dst.data(), src.data(), Min( dst.size(), src.size() ));
	}

	template <typename T, typename I>
	void  MemMove (OUT MutableArrayView<T,I> dst, ArrayView<T> src) __NE___
	{
		MemMove( OUT dst.data(), src.data(), Min( dst.size(), src.size() ));
	}

} // AE::Base


template <typename T, typename I>
struct std::hash< AE::Base::MutableArrayView<T,I> >
{
	ND_ size_t  operator () (const AE::Base::MutableArrayView<T,I> &value) C_NE___
	{
		return std::hash< AE::Base::ArrayView<T,I> >{}( AE::Base::ArrayView<T,I>{ value });
	}
};
