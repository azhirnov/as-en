// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TypeList.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedTupleArray.h"

namespace AE::Base
{

	//
	// Tuple Array View
	//

	template <typename ...Types>
	struct TupleArrayView
	{
	// types
	private:
		template <typename T>
		union ArrayPtr
		{
		// variables
			T const *	ptr;
			T const		(*dbgView)[400];		// debug viewer, don't use this field!

		// methods
			ArrayPtr ()				__NE___ : ptr{null} {}
			ArrayPtr (const T* p)	__NE___ : ptr{p} {}
		};

		using Array_t	= Tuple< ArrayPtr<Types>... >;
		using Self		= TupleArrayView< Types... >;
		using Types_t	= TypeList< Types... >;
		using CResult_t	= TupleRef< const Types ...>;

		StaticAssert( Types_t::Count > 0 );


		struct TIterator
		{
		// variables
		private:
			const Self*	_ptr	= null;
			usize		_index	= UMax;

		// methods
		public:
			TIterator ()										__NE___ {}
			TIterator (const TIterator &)						__NE___ = default;
			TIterator (TIterator &&)							__NE___ = default;
			TIterator (const Self* ptr, usize idx)				__NE___ : _ptr{ptr}, _index{idx} { NonNull( _ptr ); }

				TIterator&	operator = (const TIterator &)		__NE___ = default;
				TIterator&	operator = (TIterator &&)			__NE___ = default;

			ND_ bool		operator != (const TIterator &rhs)	C_NE___ { return not (*this == rhs); }
			ND_ bool		operator == (const TIterator &rhs)	C_NE___ { return _ptr == rhs._ptr and _index == rhs._index; }

				TIterator&  operator ++ ()						__NE___	{ NonNull( _ptr );		_index = Min( _index + 1, _ptr->size() );	return *this; }
				TIterator	operator ++ (int)					__NE___	{ TIterator res{*this};	this->operator++();							return res; }
				TIterator&	operator += (usize x)				__NE___	{ NonNull( _ptr );		_index = Min( _index + x, _ptr->size() );	return *this; }

			ND_ TIterator	operator + (usize x)				C_NE___	{ return (TIterator{*this} += x); }
			ND_ CResult_t	operator * ()						C_NE___	{ NonNull( _ptr );  return (*_ptr)[_index]; }
		};

	public:
		using const_iterator = TIterator;


	// variables
	private:
		usize		_count	= 0;
		Array_t		_arrays	= {};


	// methods
	public:
		__Cx__ TupleArrayView ()									__NE___ {}
		__Cx__ explicit TupleArrayView (usize count)				__NE___ : _count{ count } {}

		__Cx__ explicit TupleArrayView (usize count, const Types* ...args)	__NE___ :
			_count{ count },
			_arrays{ ArrayPtr<Types>{args} ... }
		{}

		__Cx__ explicit TupleArrayView (ArrayView<Types> ...args)	__NE___
		{
			_InitCount( args... );
			if_unlikely( not _InitPtr<0>( args... ))
				_count = 0;
		}

		template <usize S>
		__Cx__ explicit TupleArrayView (const FixedTupleArray<S,Types...> &arr) __NE___ : _count{arr.size()} { _InitArr<0>( arr ); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ ArrayView<T>		get ()							C_NE___	{ auto* ptr = data<I>();  return ArrayView<T>{ ptr, ptr ? _count : 0 }; }

		template <typename T>
		NdCx__ ArrayView<T>		get ()							C_NE___	{ return get< Types_t::template Index<T>, T >(); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ const T*			data ()							C_NE___	{ return _Data<I>(); }

		template <typename T>
		NdCx__ const T*			data ()							C_NE___	{ return data< Types_t::template Index<T>, T >(); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ T const&			at (usize i)					C_NE___	{ ASSERT( i < _count );  return _Data<I>()[i]; }

		template <typename T>
		NdCx__ T const&			at (usize i)					C_NE___	{ return at< Types_t::template Index<T>, T >( i ); }

		template <typename T>
		__Cx__ void				set (const T* ptr)				__NE___	{ set< Types_t::template Index<T>, T >( ptr ); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		__Cx__ void				set (const T* ptr)				__NE___	{ _arrays.template Get<I>().ptr = ptr; }

		template <typename T>
		__Cx__ void				set (ArrayView<T> arr)			__NE___	{ set< Types_t::template Index<T>, T >( arr ); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		__Cx__ void				set (ArrayView<T> arr)			__NE___	{ ASSERT( arr.empty() or arr.size() == size() );  _arrays.template Get<I>().ptr = arr.data(); }

		template <usize I>
		NdCx__ usize			size ()							C_NE___	{ return _Data<I>() != null ? _count : 0; }

		template <typename T>
		NdCx__ usize			size ()							C_NE___	{ return size< Types_t::template Index<T> >(); }

		template <usize I>
		NdCx__ bool				empty ()						C_NE___	{ return size<I>() == 0; }

		template <typename T>
		NdCx__ bool				empty ()						C_NE___	{ return size<T>() == 0; }

		NdCx__ bool				AllNonNull ()					C_NE___	{ return _AllNonNull<0>(); }

		NdCx__ usize			size ()							C_NE___	{ return _count; }
		NdCx__ bool				empty ()						C_NE___	{ return _count == 0; }

		NdCx__ const_iterator	begin ()						C_NE___	{ return const_iterator{ this, 0 }; }
		NdCx__ const_iterator	end ()							C_NE___	{ return begin() + _count; }

		NdCx__ CResult_t		operator [] (usize index)		C_NE___;

		NdCx__ bool				operator == (const Self &rhs)	C_NE___;
		NdCx__ bool				operator != (const Self &rhs)	C_NE___	{ return not (*this == rhs); }


	private:
		template <usize I>	NdCx__ auto*	 _Data ()			C_NE___	{ return _arrays.template Get<I>().ptr; }
		template <usize I>	NdCx__ auto*	 _Data ()			__NE___	{ return _arrays.template Get<I>().ptr; }

		template <typename Arg0, typename ...Args>
		__Cx__ void  _InitCount (Arg0 arg0, Args ...args)		__NE___
		{
			_count = Max( _count, arg0.size() );

			if constexpr( CountOf<Args...>() > 0 )
				_InitCount( args... );
		}

		template <usize I, typename Arg0, typename ...Args>
		NdCx__ bool  _InitPtr (Arg0 arg0, Args ...args)			__NE___
		{
			if_unlikely( not (arg0.empty() or arg0.size() == _count) )
			{
				DBG_WARNING( "array size mismatch" );
				return false;
			}
			_arrays.template Get<I>() = arg0.data();

			if constexpr( CountOf<Args...>() > 0 )
				return _InitPtr<I+1>( args... );
			else
				return true;
		}

		template <usize I, usize S>
		__Cx__ void  _InitArr (const FixedTupleArray<S,Types...> &arr) __NE___
		{
			_arrays.template Get<I>() = arr.template data<I>();

			if constexpr( I+1 < CountOf<Types...>() )
				_InitArr<I+1>( arr );
		}

		template <usize I>
		NdCx__ bool  _AllNonNull ()								C_NE___
		{
			bool	non_null = data<I>() != null;

			if constexpr( I+1 < CountOf<Types...>() )
				return non_null and _AllNonNull<I+1>();
			else
				return non_null;
		}

		template <usize I>
		NdCx__ bool  _Equal (const Self &rhs)					C_NE___;
	};


/*
=================================================
	operator ==
=================================================
*/
	template <typename ...Types>
	__Cx__ bool  TupleArrayView<Types...>::operator == (const Self &rhs) C_NE___
	{
		if ( _count != rhs._count )
			return false;

		return _Equal<0>( rhs );
	}

	template <typename ...Types>
	template <usize I>
	__Cx__ bool  TupleArrayView<Types...>::_Equal (const Self &rhs) C_NE___
	{
		if_unlikely( get<I>() != rhs.get<I>() )
			return false;

		if constexpr( I+1 < Types_t::Count )
			return _Equal<I+1>( rhs );
		else
			return true;
	}
//-----------------------------------------------------------------------------


	template <typename ...Types>	struct TMemCopyAvailable< TupleArrayView<Types...> >	: CT_True {};
	template <typename ...Types>	struct TZeroMemAvailable< TupleArrayView<Types...> >	: CT_True {};

} // AE::Base
