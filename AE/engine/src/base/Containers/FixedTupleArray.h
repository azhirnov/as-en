// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Cache-friendly array of structures.
*/

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Memory/MemUtils.h"
#include "base/CompileTime/TypeList.h"
#include "base/Memory/CopyPolicy.h"

namespace AE::Base
{

	//
	// Fixed Capacity Tuple Array
	//

	template <usize ArraySize,
			  typename ...Types
			 >
	struct FixedTupleArray : Noncopyable, NothrowAllocatable
	{
		StaticAssert( ArraySize < 256 );
		CheckNothrow( AllNothrowMoveCtor< Types... >);
		CheckNothrow( AllNothrowDefaultCtor< Types... >);

	// types
	private:
		template <typename T>
		struct ElemArray : Noncopyable
		{
			union {
				T		arr   [ArraySize];
				ubyte	_data [sizeof(T) * ArraySize];	// unused
			};

			__Cx__ ElemArray ()						__NE___	{ DEBUG_ONLY( DbgInitMem( arr ); )}
			__Cx__ ElemArray (ElemArray &&)			= delete;
			__Cx__ ElemArray (const ElemArray &)	= delete;
			__Cx__ ~ElemArray ()					__NE___	{ DEBUG_ONLY( DbgFreeMem( arr )); }
		};

		using Array_t	= Tuple< ElemArray<Types>... >;
		using Self		= FixedTupleArray< ArraySize, Types... >;
		using Types_t	= TypeList< Types... >;
		using Result_t	= TupleRef< Types ...>;
		using CResult_t	= TupleRef< const Types ...>;
		using IdxSeq_t	= MakeIndexSequence< Types_t::Count >;


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
			__Cx__ TIterator ()								__NE___ {}
			__Cx__ TIterator (const Iter &)					__NE___ = default;
			__Cx__ TIterator (Iter &&)						__NE___ = default;
			__Cx__ TIterator (ArrPtr ptr, usize idx)		__NE___ : _ptr{ptr}, _index{idx} { NonNull( _ptr ); }

			__Cx__ Iter&	operator = (const Iter &)		__NE___ = default;
			__Cx__ Iter&	operator = (Iter &&)			__NE___ = default;

			NdCx__ bool		operator != (const Iter &rhs)	C_NE___	{ return not (*this == rhs); }
			NdCx__ bool		operator == (const Iter &rhs)	C_NE___	{ return _ptr == rhs._ptr and _index == rhs._index; }

			__Cx__ Iter&	operator ++ ()					__NE___	{ NonNull( _ptr );		_index = Min( _index + 1, _ptr->size() );	return *this; }
			__Cx__ Iter		operator ++ (int)				__NE___	{ Iter res{ *this };	this->operator++();							return res; }
			__Cx__ Iter&	operator += (usize x)			__NE___	{ NonNull( _ptr );		_index = Min( _index + x, _ptr->size() );	return *this; }

			NdCx__ Iter		operator + (usize x)			C_NE___	{ return (Iter{*this} += x); }
			NdCx__ Res_t	operator * ()					C_NE___	{ NonNull( _ptr );  return (*_ptr)[_index]; }
		};

	public:
		using iterator			= TIterator< false >;
		using const_iterator	= TIterator< true >;


	// variables
	private:
		ubyte		_count	= 0;
		Array_t		_arrays;


	// methods
	public:
		__Cx__ FixedTupleArray ()							__NE___	{}

		__Cx__ FixedTupleArray (Self &&)					__NE___;
		__Cx__ FixedTupleArray (const Self &)				__NE___;

		__Cx__ ~FixedTupleArray ()							__NE___	{ clear(); }

		__Cx__ Self&  operator = (Self &&)					__NE___;
		__Cx__ Self&  operator = (const Self &)				__NE___;

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ ArrayView<T>	get ()							C_NE___	{ return ArrayView<T>{ data<I>(), _count }; }

		template <typename T>
		NdCx__ ArrayView<T>	get ()							C_NE___	{ return get< Types_t::template Index<T>, T >(); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ T*			data ()							__NE___	{ return _Data<I>(); }

		template <typename T>
		NdCx__ T*			data ()							__NE___	{ return data< Types_t::template Index<T>, T >(); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ const T*		data ()							C_NE___	{ return _Data<I>(); }

		template <typename T>
		NdCx__ const T*		data ()							C_NE___	{ return data< Types_t::template Index<T>, T >(); }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ T&			at (usize i)					__NE___	{ ASSERT( i < size() );  return _Data<I>()[i]; }

		template <usize I,
				  typename T = typename Types_t::template Get<I> >
		NdCx__ T const&		at (usize i)					C_NE___	{ ASSERT( i < size() );  return _Data<I>()[i]; }

		template <typename T>
		NdCx__ T&			at (usize i)					__NE___	{ return at< Types_t::template Index<T>, T >( i ); }

		template <typename T>
		NdCx__ T const&		at (usize i)					C_NE___	{ return at< Types_t::template Index<T>, T >( i ); }

		NdCx__ usize			size ()						C_NE___	{ return _count; }
		NdCx__ bool				empty ()					C_NE___	{ return _count == 0; }
		NdCx__ bool				IsFull ()					C_NE___	{ return size() >= capacity(); }

		NdCx__ iterator			begin ()					__NE___	{ return iterator{ this, 0 }; }
		NdCx__ const_iterator	begin ()					C_NE___	{ return const_iterator{ this, 0 }; }
		NdCx__ iterator			end ()						__NE___	{ return begin() + size(); }
		NdCx__ const_iterator	end ()						C_NE___	{ return begin() + size(); }

		NdCx__ static usize		capacity ()					__NE___	{ return ArraySize; }

		NdCx__ Result_t			operator [] (usize index)	__NE___	{ ASSERT( index < size() );  return _Elem( index, IdxSeq_t{} ); }
		NdCx__ CResult_t		operator [] (usize index)	C_NE___	{ ASSERT( index < size() );  return _Elem( index, IdxSeq_t{} ); }

		__Cx__ Result_t		emplace_back ()					__NE___;

		template <typename ...Args>
		__Cx__ bool  set (usize index, Args&&... values)	__NE___;

		template <typename ...Args>
		__Cx__ void  push_back (Args&&... values)			__NE___;

		template <typename ...Args>
		__Cx__ bool  try_push_back (Args&&... values)		__NE___;

		__Cx__ void  pop_back ()							__NE___;

		template <typename ...Args>
		__Cx__ void  insert (usize pos, Args&&... values)	__NE___;

		__Cx__ void  resize (usize newSize)					__NE___;

		__Cx__ void  erase (usize pos)						__NE___;
		__Cx__ void  fast_erase (usize pos)					__NE___;

		__Cx__ void  clear ()								__NE___;

		NdCx__ bool  operator == (const Self &rhs)			C_NE___;
		NdCx__ bool  operator != (const Self &rhs)			C_NE___	{ return not (*this == rhs); }

		ND_ HashVal  CalcHash ()							C_NE___;


	private:
		template <usize I>	NdCx__ auto*	 _Data ()		C_NE___	{ return _arrays.template Get<I>().arr; }
		template <usize I>	NdCx__ auto*	 _Data ()		__NE___	{ return _arrays.template Get<I>().arr; }

		template <usize... Idx>
		__Cx__ Result_t	_Elem (usize index, IndexSequence<Idx...>)		__NE___;

		template <usize... Idx>
		__Cx__ CResult_t	_Elem (usize index, IndexSequence<Idx...>)	C_NE___;

		template <usize I, typename Arg0, typename ...Args>
		__Cx__ void  _PushBack (Arg0 &&arg0, Args&&... args)			__NE___;

		template <usize I, typename Arg0, typename ...Args>
		__Cx__ void  _Insert (usize pos, Arg0 &&arg0, Args&&... args)	__NE___;

		template <usize I>
		__Cx__ void  _Destroy (usize index, usize count)				__NE___;

		template <usize I>
		__Cx__ void  _Replace (usize srcIdx, usize dstIdx, usize count) __NE___;

		template <usize I>
		__Cx__ void  _Create (usize index, usize count)					__NE___;

		template <usize I>
		static __Cx__ void  _Move (Self &dst, Self &src, usize count)	__NE___;

		template <usize I>
		static __Cx__ void  _Copy (Self &dst, const Self &src, usize count) __NE___;

		template <usize I>
		ND_ HashVal  _CalcHash ()										C_NE___;

		template <usize I>
		NdCx__ bool  _Equal (const Self &rhs)							C_NE___;
	};


/*
=================================================
	constructor
=================================================
*/
	template <usize S, typename ...Types>
	__Cx__ FixedTupleArray<S, Types...>::FixedTupleArray (Self &&other) __NE___ :
		_count{ other._count }
	{
		other._count = 0;
		if ( _count > 0 )
			_Move<0>( *this, other, _count );
	}

	template <usize S, typename ...Types>
	__Cx__ FixedTupleArray<S, Types...>::FixedTupleArray (const Self &other) __NE___ :
		_count{ other._count }
	{
		if ( _count > 0 )
			_Copy<0>( *this, other, _count );
	}

/*
=================================================
	operator =
=================================================
*/
	template <usize S, typename ...Types>
	__Cx__ FixedTupleArray<S, Types...>&  FixedTupleArray<S, Types...>::operator = (Self &&rhs) __NE___
	{
		clear();

		_count		= rhs._count;
		rhs._count	= 0;

		if ( _count > 0 )
			_Move<0>( *this, rhs, _count );

		return *this;
	}

	template <usize S, typename ...Types>
	__Cx__ FixedTupleArray<S, Types...>&  FixedTupleArray<S, Types...>::operator = (const Self &rhs) __NE___
	{
		clear();

		_count = rhs._count;

		if ( _count > 0 )
			_Copy<0>( *this, rhs, _count );

		return *this;
	}

/*
=================================================
	set
=================================================
*/
	template <usize S, typename ...Types>
	template <typename ...Args>
	__Cx__ bool  FixedTupleArray<S, Types...>::set (usize index, Args&&... values) __NE___
	{
		StaticAssert( sizeof...(Args) == Types_t::Count );
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
	__Cx__ void  FixedTupleArray<S, Types...>::push_back (Args&&... values) __NE___
	{
		StaticAssert( sizeof...(Args) == Types_t::Count );
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
	__Cx__ bool  FixedTupleArray<S, Types...>::try_push_back (Args&&... values) __NE___
	{
		StaticAssert( sizeof...(Args) == Types_t::Count );
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
	__Cx__ typename FixedTupleArray<S, Types...>::Result_t
		FixedTupleArray<S, Types...>::emplace_back () __NE___
	{
		ASSERT( _count < capacity() );
		const usize	pos = _count;
		_Create<0>( pos, 1 );
		++_count;
		return _Elem( pos, IdxSeq_t{} );
	}

/*
=================================================
	_Elem
=================================================
*/
	template <usize S, typename ...Types>
	template <usize... Idx>
	__Cx__ typename FixedTupleArray<S, Types...>::Result_t
		FixedTupleArray<S, Types...>::_Elem (usize index, IndexSequence<Idx...>) __NE___
	{
		return Result_t{ _Data<Idx>()+index ... };
	}

	template <usize S, typename ...Types>
	template <usize... Idx>
	__Cx__ typename FixedTupleArray<S, Types...>::CResult_t
		FixedTupleArray<S, Types...>::_Elem (usize index, IndexSequence<Idx...>) C_NE___
	{
		return CResult_t{ _Data<Idx>()+index ... };
	}

/*
=================================================
	pop_back
=================================================
*/
	template <usize S, typename ...Types>
	__Cx__ void  FixedTupleArray<S, Types...>::pop_back () __NE___
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
	__Cx__ void  FixedTupleArray<S, Types...>::insert (usize pos, Args&&... values) __NE___
	{
		StaticAssert( sizeof...(Args) == Types_t::Count );
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
	__Cx__ void  FixedTupleArray<S, Types...>::resize (usize newSize) __NE___
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

		_count = ubyte(newSize);
	}

/*
=================================================
	erase
=================================================
*/
	template <usize S, typename ...Types>
	__Cx__ void  FixedTupleArray<S, Types...>::erase (usize pos) __NE___
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
	__Cx__ void  FixedTupleArray<S, Types...>::fast_erase (usize pos) __NE___
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
	__Cx__ void  FixedTupleArray<S, Types...>::clear () __NE___
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
	__Cx__ bool  FixedTupleArray<S, Types...>::operator == (const Self &rhs) C_NE___
	{
		if ( this == std::addressof(rhs) )
			return true;

		if ( _count != rhs._count )
			return false;

		return _Equal<0>( rhs );
	}

	template <usize S, typename ...Types>
	template <usize I>
	__Cx__ bool  FixedTupleArray<S, Types...>::_Equal (const Self &rhs) C_NE___
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
	__Cx__ void  FixedTupleArray<S, Types...>::_PushBack (Arg0 &&arg0, Args&&... args) __NE___
	{
		using T = typename Types_t::template Get<I>;

		PlacementNew<T>( OUT _Data<I>() + _count, FwdArg<Arg0>(arg0) );

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
	__Cx__ void  FixedTupleArray<S, Types...>::_Insert (usize pos, Arg0 &&arg0, Args&&... args) __NE___
	{
		using T = typename Types_t::template Get<I>;

		PlacementNew<T>( OUT _Data<I>() + pos, FwdArg<Arg0>(arg0) );

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
	__Cx__ void  FixedTupleArray<S, Types...>::_Destroy (usize index, usize count) __NE___
	{
		using T			 = typename TypeList< Types... >::template Get<I>;
		using CPolicy_t = CopyPolicy::template AutoDetect<T>;

		CPolicy_t::Destroy( INOUT _Data<I>() + index, count );

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
	__Cx__ void  FixedTupleArray<S, Types...>::_Replace (usize srcIdx, usize dstIdx, usize count) __NE___
	{
		using T			 = typename Types_t::template Get<I>;
		using CPolicy_t = CopyPolicy::template AutoDetect<T>;

		T* data = _Data<I>();
		CPolicy_t::Replace( OUT data + dstIdx, INOUT data + srcIdx, count );

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
	__Cx__ void  FixedTupleArray<S, Types...>::_Create (usize index, usize count) __NE___
	{
		using T			= typename Types_t::template Get<I>;
		using CPolicy_t = CopyPolicy::template AutoDetect<T>;

		CPolicy_t::Create( OUT _Data<I>() + index, count );

		if constexpr( I+1 < Types_t::Count )
			_Create<I+1>( index, count );
	}

/*
=================================================
	CalcHash
=================================================
*/
	template <usize S, typename ...Types>
	HashVal  FixedTupleArray<S, Types...>::CalcHash () C_NE___
	{
		return _CalcHash<0>();
	}

	template <usize S, typename ...Types>
	template <usize I>
	HashVal  FixedTupleArray<S, Types...>::_CalcHash () C_NE___
	{
		HashVal	h = HashOf( get<I>() );

		if constexpr( I+1 < Types_t::Count )
			return h + _CalcHash<I+1>();
		else
			return h;
	}

/*
=================================================
	_Move
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I>
	__Cx__ void  FixedTupleArray<S, Types...>::_Move (Self &dst, Self &src, usize count) __NE___
	{
		using T			= typename Types_t::template Get<I>;
		using CPolicy_t	= CopyPolicy::template AutoDetect<T>;

		CPolicy_t::Replace( OUT dst._Data<I>(), INOUT src._Data<I>(), count );

		if constexpr( I+1 < Types_t::Count )
			return _Move<I+1>( dst, src, count );
	}

/*
=================================================
	_Copy
=================================================
*/
	template <usize S, typename ...Types>
	template <usize I>
	__Cx__ void  FixedTupleArray<S, Types...>::_Copy (Self &dst, const Self &src, usize count) __NE___
	{
		using T			= typename Types_t::template Get<I>;
		using CPolicy_t	= CopyPolicy::template AutoDetect<T>;

		CPolicy_t::Copy( OUT dst._Data<I>(), src._Data<I>(), count );

		if constexpr( I+1 < Types_t::Count )
			return _Copy<I+1>( dst, src, count );
	}
//-----------------------------------------------------------------------------


	template <usize S, typename ...Types>
	struct TMemCopyAvailable< FixedTupleArray<S, Types...> > :
		CT_Bool< TypeList< Types... >::template ForEach_And< TMemCopyAvailable >() >{};

	template <usize S, typename ...Types>
	struct TZeroMemAvailable< FixedTupleArray<S, Types...> > :
		CT_Bool< TypeList< Types... >::template ForEach_And< TZeroMemAvailable >() >{};

	template <usize S, typename ...Types>
	struct TTriviallyDestructible< FixedTupleArray<S, Types...> > :
		CT_Bool< TypeList< Types... >::template ForEach_And< TTriviallyDestructible >() >{};

} // AE::Base


template <size_t ArraySize, typename ...Types>
struct std::hash< AE::Base::FixedTupleArray<ArraySize, Types...> >
{
	ND_ size_t  operator () (const AE::Base::FixedTupleArray<ArraySize, Types...> &value) C_NE___
	{
		return size_t(value.CalcHash());
	}
};
