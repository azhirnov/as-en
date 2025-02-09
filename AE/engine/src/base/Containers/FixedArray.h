// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/Math.h"
#include "base/Containers/ArrayView.h"
#include "base/Memory/CopyPolicy.h"
#include "base/Math/Vec.h"

namespace AE::Base
{

	//
	// Fixed Capacity Array
	//

	template <typename T,
			  usize ArraySize,
			  typename TCopyPolicy = CopyPolicy::AutoDetect<T>
			 >
	struct FixedArray : NothrowAllocatable
	{
	// types
	public:
		using iterator			= T *;
		using const_iterator	= const T *;
		using Self				= FixedArray< T, ArraySize, TCopyPolicy >;

	private:
		using CPolicy_t			= TCopyPolicy;
		using Count_t			= Conditional< (ArraySize <= MaxValue<ubyte>() and alignof(T) == alignof(ubyte)), ubyte,
									Conditional< (alignof(T) == alignof(ushort)), ushort, uint >>;

		StaticAssert( ArraySize <= MaxValue<Count_t>() );
		//StaticAssert( sizeof(T) * ArraySize <= (2u << 10) );


	// variables
	private:
		Count_t			_count	= 0;
		union {
			T			_array[ ArraySize ];
			char		_data[ ArraySize * sizeof(T) ];		// don't use this field!
		};


	// methods
	public:
		__Cz__ FixedArray ()								__NE___;
		__Cz__ FixedArray (std::initializer_list<T> list)	__NE___;		// TODO: use Args... to allow move ctor
		__Cz__ FixedArray (ArrayView<T> view)				__NE___;
		__Cz__ FixedArray (const Self &other)				__NE___;
		__Cz__ FixedArray (Self &&other)					__NE___;

		__Cz__ ~FixedArray ()								__NE___	{ clear(); }

		NdCx__ operator ArrayView<T> ()						C_NE___	{ return ArrayView<T>{ data(), size() }; }
		NdCx__ ArrayView<T>		ToArrayView()				C_NE___	{ return *this; }

		NdCx__ usize			size ()						C_NE___	{ return _count; }
		NdCx__ bool				empty ()					C_NE___	{ return _count == 0; }
		NdCx__ bool				IsFull ()					C_NE___	{ return size() >= capacity(); }
		NdCx__ T *				data ()						__NE___	{ return _array; }
		NdCx__ T const *		data ()						C_NE___	{ return _array; }

		NdCz__ T &				operator [] (usize i)		__NE___	{ ASSERT( i < _count );  return _array[i]; }
		NdCz__ T const &		operator [] (usize i)		C_NE___	{ ASSERT( i < _count );  return _array[i]; }

		NdCx__ iterator			begin ()					__NE___	{ return data(); }
		NdCx__ const_iterator	begin ()					C_NE___	{ return data(); }
		NdCx__ iterator			end ()						__NE___	{ return data() + _count; }
		NdCx__ const_iterator	end ()						C_NE___	{ return data() + _count; }

		NdCz__ T &				front ()					__NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		NdCz__ T const&			front ()					C_NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		NdCz__ T &				back ()						__NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }
		NdCz__ T const&			back ()						C_NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }

		NdCx__ static usize		capacity ()					__NE___	{ return ArraySize; }

		NdCx__ bool  operator == (ArrayView<T> rhs)			C_NE___	{ return ArrayView<T>{*this} == rhs; }
		NdCx__ bool  operator != (ArrayView<T> rhs)			C_NE___	{ return ArrayView<T>{*this} != rhs; }
		NdCx__ bool  operator >  (ArrayView<T> rhs)			C_NE___	{ return ArrayView<T>{*this} >  rhs; }
		NdCx__ bool  operator <  (ArrayView<T> rhs)			C_NE___	{ return ArrayView<T>{*this} <  rhs; }
		NdCx__ bool  operator >= (ArrayView<T> rhs)			C_NE___	{ return ArrayView<T>{*this} >= rhs; }
		NdCx__ bool  operator <= (ArrayView<T> rhs)			C_NE___	{ return ArrayView<T>{*this} <= rhs; }


		template <typename B, usize S, typename C>
		__Cz__ Self&  operator = (const FixedArray<B,S,C> &)__NE___;
		__Cz__ Self&  operator = (const Self &rhs)			__NE___	{ return operator=( ArrayView<T>{rhs} ); }
		__Cz__ Self&  operator = (Self &&rhs)				__NE___;
		template <typename B>
		__Cz__ Self&  operator = (ArrayView<B> rhs)			__NE___;

		template <typename B>
		__Cz__ void  assign (B* beginIter, B* endIter)		__NE___;
		template <typename B>
		__Cz__ void  append (B* beginIter, B* endIter)		__NE___;

		__Cz__ void  push_back (const T &value)				__NE___;
		__Cz__ void  push_back (T &&value)					__NE___;

		template <typename ...Args>
		__Cz__ T&	emplace_back (Args&& ...args)			__NE___;

		__Cz__ void  pop_back ()							__NE___;

		__Cz__ bool  try_push_back (const T &value)			__NE___;
		__Cz__ bool  try_push_back (T&& value)				__NE___;

		template <typename ...Args>
		__Cz__ bool  try_emplace_back (Args&& ...args)		__NE___;

		__Cz__ void  insert (usize pos, T &&value)			__NE___;

		__Cz__ void  resize (usize newSize)					__NE___;
		__Cz__ void  resize (usize newSize, const T &defaultValue) __NE___;

		__Cz__ void  reserve (usize newCapacity)			__NE___	{ ASSERT( newCapacity <= capacity() );  Unused( newCapacity ); }

		__Cz__ void  clear ()								__NE___;

		__Cz__ void  erase (usize index)					__NE___;
		__Cz__ void  fast_erase (usize index)				__NE___;

	private:
		NdCz__ bool  _IsMemoryAliased (const void* beginIter, const void* endIter) C_NE___
		{
			return IsIntersects<const void*>( begin(), end(), beginIter, endIter );
		}
	};


/*
=================================================
	constructor
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ FixedArray<T,S,CS>::FixedArray () __NE___
	{
		DEBUG_ONLY( DbgInitMem( data(), SizeOf<T> * capacity() ));

		StaticAssert( IsMultipleOf( alignof(Self), alignof(T) ));
	}

	template <typename T, usize S, typename CS>
	__Cz__ FixedArray<T,S,CS>::FixedArray (std::initializer_list<T> list) __NE___ : FixedArray()
	{
		ASSERT( list.size() <= capacity() );
		assign( list.begin(), list.end() );
	}

	template <typename T, usize S, typename CS>
	__Cz__ FixedArray<T,S,CS>::FixedArray (ArrayView<T> view) __NE___ : FixedArray()
	{
		ASSERT( view.size() <= capacity() );
		assign( view.begin(), view.end() );
	}

	template <typename T, usize S, typename CS>
	__Cz__ FixedArray<T,S,CS>::FixedArray (const Self &other) __NE___ : FixedArray()
	{
		assign( other.begin(), other.end() );
	}

	template <typename T, usize S, typename CS>
	__Cz__ FixedArray<T,S,CS>::FixedArray (Self &&other) __NE___ : _count{other._count}
	{
		ASSERT( not _IsMemoryAliased( other.begin(), other.end() ));
		CheckNothrow( IsNothrowMoveCtor<T> );

		CPolicy_t::Replace( OUT _array, INOUT other._array, _count );
		other._count = 0;
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename T2, usize S2, typename CS2>
	__Cz__ FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (const FixedArray<T2,S2,CS2> &rhs) __NE___
	{
		assign( rhs.begin(), rhs.end() );
		return *this;
	}

	template <typename T, usize S, typename CS>
	template <typename B>
	__Cz__ FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (ArrayView<B> rhs) __NE___
	{
		ASSERT( rhs.size() < capacity() );
		assign( rhs.begin(), rhs.end() );
		return *this;
	}

	template <typename T, usize S, typename CS>
	__Cz__ FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (Self &&rhs) __NE___
	{
		ASSERT( not _IsMemoryAliased( rhs.begin(), rhs.end() ));
		CheckNothrow( IsNothrowMoveCtor<T> );

		CPolicy_t::Destroy( INOUT _array, _count );
		CPolicy_t::Replace( OUT _array, INOUT rhs._array, rhs._count );

		_count		= rhs._count;
		rhs._count	= 0;

		return *this;
	}

/*
=================================================
	assign
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename B>
	__Cz__ void  FixedArray<T,S,CS>::assign (B* beginIter, B* endIter) __NE___
	{
		StaticAssert( IsConstructible< T, B >);

		ASSERT( beginIter <= endIter );
		ASSERT( not _IsMemoryAliased( beginIter, endIter ));

		clear();

		for (auto iter = beginIter; (_count < capacity()) and (iter != endIter); ++iter, ++_count)
		{
			PlacementNew<T>( OUT data() + _count, *iter );
		}
	}

/*
=================================================
	append
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename B>
	__Cz__ void  FixedArray<T,S,CS>::append (B* beginIter, B* endIter) __NE___
	{
		StaticAssert( IsConstructible< T, B >);

		ASSERT( beginIter <= endIter );
		ASSERT( not _IsMemoryAliased( beginIter, endIter ));

		for (auto iter = beginIter; (_count < capacity()) and (iter != endIter); ++iter, ++_count)
		{
			PlacementNew<T>( OUT data() + _count, *iter );
		}
	}

/*
=================================================
	push_back
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::push_back (const T &value) __NE___
	{
		ASSERT( _count < capacity() );
		PlacementNew<T>( OUT data() + _count, value );
		++_count;
	}

	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::push_back (T &&value) __NE___
	{
		ASSERT( _count < capacity() );
		PlacementNew<T>( OUT data() + _count, RVRef(value) );
		++_count;
	}

/*
=================================================
	emplace_back
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename ...Args>
	__Cz__ T&  FixedArray<T,S,CS>::emplace_back (Args&& ...args) __NE___
	{
		StaticAssert( IsConstructible< T, Args... >);
		ASSERT( _count < capacity() );

		T* ptr = data() + _count;
		PlacementNew<T>( OUT ptr, FwdArg<Args>( args )... );
		++_count;
		return *ptr;
	}

/*
=================================================
	pop_back
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::pop_back () __NE___
	{
		ASSERT( _count > 0 );
		--_count;

		CPolicy_t::Destroy( INOUT std::addressof(_array[_count]), 1 );
	}

/*
=================================================
	try_push_back
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ bool  FixedArray<T,S,CS>::try_push_back (const T &value) __NE___
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( OUT data() + _count, value );
			++_count;
			return true;
		}
		return false;
	}

	template <typename T, usize S, typename CS>
	__Cz__ bool  FixedArray<T,S,CS>::try_push_back (T&& value) __NE___
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( OUT data() + _count, RVRef(value) );
			++_count;
			return true;
		}
		return false;
	}

/*
=================================================
	try_emplace_back
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename ...Args>
	__Cz__ bool  FixedArray<T,S,CS>::try_emplace_back (Args&& ...args) __NE___
	{
		StaticAssert( IsConstructible< T, Args... >);

		if_likely( _count < capacity() )
		{
			PlacementNew<T>( OUT data() + _count, FwdArg<Args &&>( args )... );
			++_count;
			return true;
		}
		return false;
	}

/*
=================================================
	insert
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::insert (usize pos, T &&value) __NE___
	{
		ASSERT( _count < capacity() );
		CheckNothrow( IsNothrowMoveCtor<T> );

		pos = Min( pos, _count );
		CPolicy_t::Replace( OUT std::addressof(_array[pos+1]), INOUT std::addressof(_array[pos]), _count - pos );

		++_count;
		PlacementNew<T>( OUT std::addressof(_array[pos]), RVRef(value) );
	}

/*
=================================================
	resize
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::resize (usize newSize) __NE___
	{
		newSize = Min( newSize, capacity() );

		if ( newSize < _count )
		{
			CPolicy_t::Destroy( INOUT std::addressof(_array[newSize]), _count - newSize );
		}
		else
		if ( newSize > _count )
		{
			CheckNothrow( IsNothrowDefaultCtor<T> );
			CPolicy_t::Create( OUT std::addressof(_array[_count]), newSize - _count );
		}

		_count = Count_t(newSize);
	}

	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::resize (usize newSize, const T &defaultValue) __NE___
	{
		newSize = Min( newSize, capacity() );

		if ( newSize < _count )
		{
			CPolicy_t::Destroy( INOUT std::addressof(_array[newSize]), _count - newSize );
		}
		else
		if ( newSize > _count )
		{
			CheckNothrow( IsNothrowCopyCtor<T> );
			CPolicy_t::Create( OUT std::addressof(_array[_count]), newSize - _count, defaultValue );
		}

		_count = Count_t(newSize);
	}

/*
=================================================
	clear
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::clear () __NE___
	{
		CPolicy_t::Destroy( INOUT _array, _count );

		_count = 0;
	}

/*
=================================================
	fast_erase
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::fast_erase (usize index) __NE___
	{
		ASSERT( index < _count );

		--_count;
		CPolicy_t::Destroy( INOUT std::addressof(_array[index]), 1 );

		if ( index != _count )
		{
			// move element from back to 'index'
			CheckNothrow( IsNothrowMoveCtor<T> );
			CPolicy_t::Replace( OUT std::addressof(_array[index]), INOUT std::addressof(_array[_count]), 1 );
		}
		else
		{
			DEBUG_ONLY( DbgInitMem( data() + _count, SizeOf<T> ));
		}
	}

/*
=================================================
	erase
=================================================
*/
	template <typename T, usize S, typename CS>
	__Cz__ void  FixedArray<T,S,CS>::erase (usize index) __NE___
	{
		ASSERT( index < _count );

		CPolicy_t::Destroy( INOUT std::addressof(_array[index]), 1 );

		if ( index+1 < _count )
		{
			CheckNothrow( IsNothrowMoveCtor<T> );
			CPolicy_t::Replace( OUT std::addressof(_array[index]), INOUT std::addressof(_array[index + 1]), _count - index - 1 );
		}
		--_count;
	}
//-----------------------------------------------------------------------------


	template <typename T, usize S, typename CS>	struct TMemCopyAvailable< FixedArray<T,S,CS> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T, usize S, typename CS>	struct TZeroMemAvailable< FixedArray<T,S,CS> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T, usize S, typename CS>	struct TTriviallyDestructible< FixedArray<T,S,CS> >	: CT_Bool< IsTriviallyDestructible<T>	>{};


} // AE::Base


template <typename T, size_t ArraySize, typename CS>
struct std::hash< AE::Base::FixedArray<T, ArraySize, CS> >
{
	ND_ size_t  operator () (const AE::Base::FixedArray<T, ArraySize, CS> &value) C_NE___
	{
		return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
	}
};
