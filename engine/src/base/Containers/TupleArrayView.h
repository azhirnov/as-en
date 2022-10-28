// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TypeList.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/Tuple.h"

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
			ArrayPtr () : ptr{null} {}
			ArrayPtr (const T* p) : ptr{p} {}
		};

		using Array_t	= Tuple< ArrayPtr<Types>... >;
		using Self		= TupleArrayView< Types... >;
		using Types_t	= TypeList< Types... >;
		using CResult_t	= TupleRef< const Types ...>;

		STATIC_ASSERT( Types_t::Count > 0 );
		

		struct TIterator
		{
		// variables
		private:
			const Self*	_ptr	= null;
			usize		_index	= UMax;

		// methods
		public:
			TIterator () {}
			TIterator (const TIterator &) = default;
			TIterator (TIterator &&) = default;
			TIterator (const Self *ptr, usize idx) : _ptr{ptr}, _index{idx} { ASSERT( _ptr != null ); }
			
			TIterator&  operator = (const TIterator &) = default;
			TIterator&  operator = (TIterator &&) = default;
			
			ND_ bool  operator != (const TIterator &rhs) const	{ return not (*this == rhs); }
			ND_ bool  operator == (const TIterator &rhs) const	{ return _ptr == rhs._ptr and _index == rhs._index; }
			
			TIterator&  operator ++ ()
			{
				ASSERT( _ptr != null );
				_index = Min( _index + 1, _ptr->size() );
				return *this;
			}

			TIterator  operator ++ (int)
			{
				TIterator	res{ *this };
				this->operator++();
				return res;
			}
			
			TIterator&  operator += (usize x)
			{
				ASSERT( _ptr != null );
				_index = Min( _index + x, _ptr->size() );
				return *this;
			}

			ND_ TIterator  operator + (usize x) const
			{
				return (TIterator{*this} += x);
			}

			ND_ CResult_t  operator * () const	{ ASSERT( _ptr != null );	return (*_ptr)[_index]; }
		};
		
	public:
		using const_iterator = TIterator;


	// variables
	private:
		usize		_count	= 0;
		Array_t		_arrays	= {};


	// methods
	public:
		constexpr TupleArrayView () {}
		
		explicit constexpr TupleArrayView (usize count, const Types* ...args) :
			_count{ count },
			_arrays{ ArrayPtr<Types>{args} ... }
		{}

		explicit constexpr TupleArrayView (ArrayView<Types> ...args)
		{
			_InitCount( args... );
			if_unlikely( not _InitPtr<0>( args... ))
				_count = 0;
		}
		
		template <usize I>
		ND_ constexpr auto			get ()			const	{ return ArrayView<typename Types_t::template Get<I>>{ _Data<I>(), _count }; }

		template <typename T>
		ND_ constexpr ArrayView<T>	get ()			const	{ return get< Types_t::template Index<T> >(); }
		
		template <usize I>
		ND_ constexpr auto*			data ()			const	{ return _Data<I>(); }

		template <typename T>
		ND_ constexpr const T*		data ()			const	{ return data< Types_t::template Index<T> >(); }

		template <usize I>
		ND_ decltype(auto)			at (usize i)	const	{ ASSERT( i < _count ); return _Data<I>()[i]; }
		
		template <typename T>
		ND_ T const&				at (usize i)	const	{ return at< Types_t::template Index<T> >( i ); }
		
		template <usize I>
		ND_ constexpr usize			size ()			const	{ return _Data<I>() != null ? _count : 0; }
		
		template <typename T>
		ND_ constexpr usize			size ()			const	{ return size< Types_t::template Index<T> >(); }
		
		template <usize I>
		ND_ constexpr bool			empty ()		const	{ return size<I>() == 0; }
		
		template <typename T>
		ND_ constexpr bool			empty ()		const	{ return size<T>() == 0; }

		ND_ constexpr bool			AllNonNull ()	const	{ return _AllNonNull<0>(); }

		ND_ constexpr usize			size ()			const	{ return _count; }
		ND_ constexpr bool			empty ()		const	{ return _count == 0; }
		
		ND_ const_iterator			begin ()		const	{ return const_iterator{ this, 0 }; }
		ND_ const_iterator			end ()			const	{ return begin() + _count; }

		ND_ constexpr CResult_t		operator [] (usize index) const;

		ND_ bool  operator == (const Self &rhs) const;
		ND_ bool  operator != (const Self &rhs) const	{ return not (*this == rhs); }

	private:
		template <usize I>	ND_ constexpr auto*	 _Data () const	{ return _arrays.template Get<I>().ptr; }
		template <usize I>	ND_ constexpr auto*	 _Data ()		{ return _arrays.template Get<I>().ptr; }

		template <typename Arg0, typename ...Args>
		constexpr void  _InitCount (Arg0 arg0, Args ...args)
		{
			_count = Max( _count, arg0.size() );

			if constexpr( sizeof...(Args) > 0 )
				_InitCount( args... );
		}

		template <usize I, typename Arg0, typename ...Args>
		ND_ constexpr bool  _InitPtr (Arg0 arg0, Args ...args)
		{
			if_unlikely( not (arg0.empty() or arg0.size() == _count) )
			{
				DBG_WARNING( "array size mismatch" );
				return false;
			}
			_arrays.template Get<I>() = arg0.data();

			if constexpr( sizeof...(Args) > 0 )
				return _InitPtr<I+1>( args... );
			else
				return true;
		}

		template <usize I>
		constexpr bool  _AllNonNull () const
		{
			bool	non_null = data<I>() != null;

			if constexpr( I+1 < sizeof...(Types) )
				return non_null & _AllNonNull<I+1>();
			else
				return non_null;
		}
	};

} // AE::Base
