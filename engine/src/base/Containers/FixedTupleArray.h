// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Memory/MemUtils.h"
#include "base/CompileTime/TypeList.h"
#include "base/Memory/CopyPolicy.h"

namespace AE::Base
{

	//
	// Fixed Size Tuple Array
	//

	template <usize ArraySize, typename ...Types>
	struct FixedTupleArray
	{
		STATIC_ASSERT( ArraySize < 256 );

	// types
	private:
		template <typename T>
		struct ElemArray
		{
			union {
				T		arr   [ArraySize];
				ubyte	_data [sizeof(T) * ArraySize];
			};

			ElemArray ()	{ DEBUG_ONLY( DbgInitMem( arr )); }
			~ElemArray ()	{ DEBUG_ONLY( DbgInitMem( arr )); }
		};

		using Array_t	= Tuple< ElemArray<Types>... >;
		using Self		= FixedTupleArray< ArraySize, Types... >;
		using Types_t	= TypeList< Types... >;
		using Result_t	= TupleRef< Types ...>;
		using CResult_t	= TupleRef< const Types ...>;
		

		template <bool IsConst>
		struct TIterator
		{
		// types
		private:
			using Iter		= TIterator< IsConst >;
			using ArrPtr	= Conditional< IsConst, const Self *, Self *>;
			using Res_t		= Conditional< IsConst, CResult_t, Result_t >;
			
		// variables
		private:
			ArrPtr	_ptr	= null;
			usize	_index	= UMax;

		// methods
		public:
			TIterator () {}
			TIterator (const Iter &) = default;
			TIterator (Iter &&) = default;
			TIterator (ArrPtr ptr, usize idx) : _ptr{ptr}, _index{idx} { ASSERT( _ptr != null ); }
			
			Iter& operator = (const Iter &) = default;
			Iter& operator = (Iter &&) = default;
			
			ND_ bool operator != (const Iter &rhs) const	{ return not (*this == rhs); }
			ND_ bool operator == (const Iter &rhs) const	{ return _ptr == rhs._ptr and _index == rhs._index; }
			
			Iter& operator ++ ()
			{
				ASSERT( _ptr != null );
				_index = Min( _index + 1, _ptr->size() );
				return *this;
			}

			Iter  operator ++ (int)
			{
				Iter	res{ *this };
				this->operator++();
				return res;
			}
			
			Iter&  operator += (usize x)
			{
				ASSERT( _ptr != null );
				_index = Min( _index + x, _ptr->size() );
				return *this;
			}

			ND_ Iter  operator + (usize x) const
			{
				return (Iter{*this} += x);
			}

			ND_ Res_t  operator * () const	{ ASSERT( _ptr != null );	return (*_ptr)[_index]; }
		};

	public:
		using iterator			= TIterator< false >;
		using const_iterator	= TIterator< true >;


	// variables
	private:
		usize		_count	= 0;
		Array_t		_arrays;


	// methods
	public:
		constexpr FixedTupleArray () {}

		~FixedTupleArray ()		{ clear(); }
		
		template <usize I>
		ND_ constexpr auto			get ()			const	{ return ArrayView<typename Types_t::template Get<I>>{ _Data<I>(), _count }; }

		template <typename T>
		ND_ constexpr ArrayView<T>	get ()			const	{ return get< Types_t::template Index<T> >(); }
		
		template <usize I>
		ND_ constexpr auto*			data ()					{ return  _Data<I>(); }

		template <typename T>
		ND_ constexpr T*			data ()					{ return data< Types_t::template Index<T> >(); }

		template <usize I>
		ND_ decltype(auto)			at (usize i)			{ ASSERT( i < _count ); return _Data<I>()[i]; }
		
		template <usize I>
		ND_ decltype(auto)			at (usize i)	const	{ ASSERT( i < _count ); return _Data<I>()[i]; }
		
		template <typename T>
		ND_ T&						at (usize i)			{ return at< Types_t::template Index<T> >( i ); }
		
		template <typename T>
		ND_ T const&				at (usize i)	const	{ return at< Types_t::template Index<T> >( i ); }

		ND_ constexpr usize			size ()			const	{ return _count; }
		ND_ constexpr bool			empty ()		const	{ return _count == 0; }
		
		ND_ iterator				begin ()				{ return iterator{ this, 0 }; }
		ND_ const_iterator			begin ()		const	{ return const_iterator{ this, 0 }; }
		ND_ iterator				end ()					{ return begin() + _count; }
		ND_ const_iterator			end ()			const	{ return begin() + _count; }

		ND_ static constexpr usize	capacity ()				{ return ArraySize; }
		
		ND_ constexpr Result_t		operator [] (usize index);
		ND_ constexpr CResult_t		operator [] (usize index) const;

			constexpr Result_t		emplace_back ();
		
		template <typename ...Args>
		constexpr bool  set (usize index, Args&&... values);
		
		template <typename ...Args>
		constexpr void  push_back (Args&&... values);
		
		template <typename ...Args>
		constexpr bool  try_push_back (Args&&... values);

		constexpr void  pop_back ();
		
		template <typename ...Args>
		constexpr void  insert (usize pos, Args&&... values);

		constexpr void  resize (usize newSize);

		constexpr void  erase (usize pos);
		constexpr void  fast_erase (usize pos);

		constexpr void  clear ();

		ND_ bool  operator == (const Self &rhs) const;
		ND_ bool  operator != (const Self &rhs) const	{ return not (*this == rhs); }

		ND_ HashVal  CalcHash () const;


	private:
		template <usize I>	ND_ constexpr auto*	 _Data () const	{ return _arrays.template Get<I>().arr; }
		template <usize I>	ND_ constexpr auto*	 _Data ()		{ return _arrays.template Get<I>().arr; }

		template <typename T>
		ND_ constexpr T*  _At (usize i)	{ return _Data< Types_t::template Index<T> >() + i; }

		template <usize I, typename Arg0, typename ...Args>
		constexpr void  _PushBack (Arg0 &&arg0, Args&&... args);

		template <usize I, typename Arg0, typename ...Args>
		constexpr void  _Insert (usize pos, Arg0 &&arg0, Args&&... args);

		template <usize I>
		constexpr void  _Destroy (usize index, usize count);

		template <usize I>
		constexpr void  _Replace (usize srcIdx, usize dstIdx, usize count);

		template <usize I>
		constexpr void  _Create (usize index, usize count);

		template <usize I>
		HashVal  _CalcHash () const;
		
		template <usize I>
		bool  _Equal (const Self &rhs) const;
	};

	
/*
=================================================
	set
=================================================
*/
	template <usize S, typename ...Types>
	template <typename ...Args>
	constexpr bool  FixedTupleArray<S, Types...>::set (usize index, Args&&... values)
	{
		STATIC_ASSERT( sizeof...(Args) == Types_t::Count );
		if_likely( index < _count )
		{
			_Destroy<0>( index, 1 );
			_Insert<0>( index, FwdArg<Args>(values)... );
			return true;
		}
		return false;
	}

/*
=================================================
	push_back
=================================================
*/
	template <usize S, typename ...Types>
	template <typename ...Args>
	constexpr void  FixedTupleArray<S, Types...>::push_back (Args&&... values)
	{
		STATIC_ASSERT( sizeof...(Args) == Types_t::Count );
		ASSERT( _count < capacity() );
		_PushBack<0>( FwdArg<Args>(values)... );
		++_count;
	}
		
/*
=================================================
	try_push_back
=================================================
*/
	template <usize S, typename ...Types>
	template <typename ...Args>
	constexpr bool  FixedTupleArray<S, Types...>::try_push_back (Args&&... values)
	{
		STATIC_ASSERT( sizeof...(Args) == Types_t::Count );
		if_likely( _count < capacity() )
		{
			_PushBack<0>( FwdArg<Args>(values)... );
			++_count;
			return true;
		}
		return false;
	}
	
/*
=================================================
	emplace_back
=================================================
*/
	template <usize S, typename ...Types>
	constexpr typename FixedTupleArray<S, Types...>::Result_t
		FixedTupleArray<S, Types...>::emplace_back ()
	{
		ASSERT( _count < capacity() );
		const usize	pos = _count;
		_Create<0>( pos, 1 );
		++_count;
		return operator[]( pos );
	}

/*
=================================================
	operator []
=================================================
*/
	template <usize S, typename ...Types>
	constexpr typename FixedTupleArray<S, Types...>::Result_t
		FixedTupleArray<S, Types...>::operator [] (usize index)
	{
		return Result_t{ _At<Types>( index )... };
	}
	
	template <usize S, typename ...Types>
	constexpr typename FixedTupleArray<S, Types...>::CResult_t
		FixedTupleArray<S, Types...>::operator [] (usize index) const
	{
		return CResult_t{ const_cast<Self*>(this)->_At<Types>( index )... };
	}

/*
=================================================
	pop_back
=================================================
*/
	template <usize S, typename ...Types>
	constexpr void  FixedTupleArray<S, Types...>::pop_back ()
	{
		ASSERT( _count > 0 );
		--_count;
		_Destroy<0>( _count, 1 );
	}
		
/*
=================================================
	insert
=================================================
*/
	template <usize S, typename ...Types>
	template <typename ...Args>
	constexpr void  FixedTupleArray<S, Types...>::insert (usize pos, Args&&... values)
	{
		STATIC_ASSERT( sizeof...(Args) == Types_t::Count );
		ASSERT( _count < capacity() );
		if ( pos >= _count ) {
			_PushBack<0>( FwdArg<Args>(values)... );
		}else{
			_Replace<0>( pos, pos+1, _count - pos );
			_Insert<0>( pos, FwdArg<Args>(values)... );
		}
		++_count;
	}
	
/*
=================================================
	resize
=================================================
*/
	template <usize S, typename ...Types>
	constexpr void  FixedTupleArray<S, Types...>::resize (usize newSize)
	{
		newSize = Min( newSize, capacity() );

		if ( newSize < _count )
		{
			_Destroy<0>( newSize, _count - newSize );
		}
		else
		if ( newSize > _count )
		{
			_Create<0>( _count, newSize - _count );
		}

		_count = newSize;
	}
	
/*
=================================================
	erase
=================================================
*/
	template <usize S, typename ...Types>
	constexpr void  FixedTupleArray<S, Types...>::erase (usize pos)
	{
		ASSERT( _count > 0 );
		--_count;
		_Destroy<0>( pos, 1 );

		if ( pos < _count )
			_Replace<0>( pos+1, pos, _count - pos );
	}
	
/*
=================================================
	fast_erase
=================================================
*/
	template <usize S, typename ...Types>
	constexpr void  FixedTupleArray<S, Types...>::fast_erase (usize pos)
	{
		ASSERT( _count > 0 );
		--_count;
		_Destroy<0>( pos, 1 );

		if ( pos < _count )
			_Replace<0>( _count, pos, 1 );
	}
	
/*
=================================================
	clear
=================================================
*/
	template <usize S, typename ...Types>
	constexpr void  FixedTupleArray<S, Types...>::clear ()
	{
		_Destroy<0>( 0, _count );
		_count = 0;
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <usize S, typename ...Types>
	bool  FixedTupleArray<S, Types...>::operator == (const Self &rhs) const
	{
		if ( this == &rhs )
			return true;

		if ( _count != rhs._count )
			return false;

		return _Equal<0>( rhs );
	}
	
	template <usize S, typename ...Types>
	template <usize I>
	bool  FixedTupleArray<S, Types...>::_Equal (const Self &rhs) const
	{
		if_unlikely( get<I>() != rhs.get<I>() )
			return false;
		
		if constexpr( I+1 < Types_t::Count )
			return _Equal<I+1>( rhs );
		else
			return true;
	}

/*
=================================================
	_PushBack
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I, typename Arg0, typename ...Args>
	constexpr void  FixedTupleArray<S, Types...>::_PushBack (Arg0 &&arg0, Args&&... args)
	{
		using T = std::remove_const_t< std::remove_reference_t< Arg0 >>;

		PlacementNew<T>( _Data<I>() + _count, FwdArg<Arg0>(arg0) );
			
		if constexpr( I+1 < Types_t::Count )
			_PushBack<I+1>( FwdArg<Args>(args)... );
	}
		
/*
=================================================
	_Insert
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I, typename Arg0, typename ...Args>
	constexpr void  FixedTupleArray<S, Types...>::_Insert (usize pos, Arg0 &&arg0, Args&&... args)
	{
		using T = std::remove_const_t< std::remove_reference_t< Arg0 >>;
		T* data = _Data<I>();
		
		PlacementNew<T>( data + pos, FwdArg<Arg0>(arg0) );
			
		if constexpr( I+1 < Types_t::Count )
			_Insert<I+1>( pos, FwdArg<Args>(args)... );
	}
	
/*
=================================================
	_Destroy
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I>
	constexpr void  FixedTupleArray<S, Types...>::_Destroy (usize index, usize count)
	{
		using T			 = typename TypeList< Types... >::template Get<I>;
		using CPolicy_t = CopyPolicy::template AutoDetect<T>;

		T* data = _Data<I>();
		CPolicy_t::Destroy( data + index, count );

		if constexpr( I+1 < Types_t::Count )
			_Destroy<I+1>( index, count );
	}
	
/*
=================================================
	_Replace
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I>
	constexpr void  FixedTupleArray<S, Types...>::_Replace (usize srcIdx, usize dstIdx, usize count)
	{
		using T			 = typename Types_t::template Get<I>;
		using CPolicy_t = CopyPolicy::template AutoDetect<T>;

		T* data = _Data<I>();
		CPolicy_t::Replace( OUT data + dstIdx, data + srcIdx, count );
			
		if constexpr( I+1 < Types_t::Count )
			_Replace<I+1>( srcIdx, dstIdx, count );
	}
	
/*
=================================================
	_Create
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I>
	constexpr void  FixedTupleArray<S, Types...>::_Create (usize index, usize count)
	{
		using T			 = typename Types_t::template Get<I>;
		using CPolicy_t = CopyPolicy::template AutoDetect<T>;

		T* data = _Data<I>();
		CPolicy_t::Create( data + index, count );

		if constexpr( I+1 < Types_t::Count )
			_Create<I+1>( index, count );
	}
	
/*
=================================================
	CalcHash
=================================================
*/
	template <usize S, typename ...Types>
	HashVal  FixedTupleArray<S, Types...>::CalcHash () const
	{
		return _CalcHash<0>();
	}
	
	template <usize S, typename ...Types>
	template <usize I>
	HashVal  FixedTupleArray<S, Types...>::_CalcHash () const
	{
		HashVal	h = HashOf( get<I>() );

		if constexpr( I+1 < Types_t::Count )
			return h + _CalcHash<I+1>();
		else
			return h;
	}

}	// AE::Base


namespace std
{
	template <size_t ArraySize, typename ...Types>
	struct hash< AE::Base::FixedTupleArray<ArraySize, Types...> >
	{
		ND_ size_t  operator () (const AE::Base::FixedTupleArray<ArraySize, Types...> &value) const
		{
			return size_t(value.CalcHash());
		}
	};

}	// std
