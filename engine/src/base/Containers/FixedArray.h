// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exceptions:
		- array elements may throw exceptions (in copy-ctor)
*/

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Memory/CopyPolicy.h"
#include "base/Math/Vec.h"

namespace AE::Base
{

	//
	// Fixed Size Array
	//

	template <typename T,
			  usize ArraySize,
			  typename Policy = CopyPolicy::AutoDetect<T>
			 >
	struct FixedArray
	{
		//STATIC_ASSERT( IsNothrowMoveCtor<T> );
		//STATIC_ASSERT( IsNothrowDefaultCtor<T> );

	// types
	public:
		using iterator			= T *;
		using const_iterator	= const T *;
		using Self				= FixedArray< T, ArraySize, Policy >;

	private:
		using CPolicy_t		= Policy;
		using Count_t		= Conditional< (ArraySize <= MaxValue<ubyte>()), ubyte, ushort >;
		STATIC_ASSERT( ArraySize < MaxValue<Count_t>() );


	// variables
	private:
		Count_t			_count	= 0;
		union {
			T			_array[ ArraySize ];
			char		_data[ ArraySize * sizeof(T) ];		// don't use this field!
		};


	// methods
	public:
		constexpr FixedArray ()									__NE___;
		constexpr FixedArray (std::initializer_list<T> list)	noexcept(IsNothrowCopyCtor<T>);		// TODO: use Args... to allow move ctor
		constexpr FixedArray (ArrayView<T> view)				noexcept(IsNothrowCopyCtor<T>);
		constexpr FixedArray (const Self &other)				noexcept(IsNothrowCopyCtor<T>);
		constexpr FixedArray (Self &&other)						__NE___;

		~FixedArray ()											__NE___	{ clear(); }


		ND_ constexpr operator ArrayView<T> ()					C_NE___	{ return ArrayView<T>{ data(), size() }; }
		ND_ constexpr ArrayView<T>		ToArrayView()			C_NE___	{ return *this; }

		ND_ constexpr usize				size ()					C_NE___	{ return _count; }
		ND_ constexpr bool				empty ()				C_NE___	{ return _count == 0; }
		ND_ constexpr T *				data ()					__NE___	{ return std::addressof( _array[0] ); }
		ND_ constexpr T const *			data ()					C_NE___	{ return std::addressof( _array[0] ); }

		ND_ constexpr T &				operator [] (usize i)	__NE___	{ ASSERT( i < _count );  return _array[i]; }
		ND_ constexpr T const &			operator [] (usize i)	C_NE___	{ ASSERT( i < _count );  return _array[i]; }

		ND_ constexpr iterator			begin ()				__NE___	{ return data(); }
		ND_ constexpr const_iterator	begin ()				C_NE___	{ return data(); }
		ND_ constexpr iterator			end ()					__NE___	{ return data() + _count; }
		ND_ constexpr const_iterator	end ()					C_NE___	{ return data() + _count; }

		ND_ constexpr T &				front ()				__NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T const&			front ()				C_NE___	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T &				back ()					__NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }
		ND_ constexpr T const&			back ()					C_NE___	{ ASSERT( _count > 0 );  return _array[_count-1]; }
		
		ND_ static constexpr usize		capacity ()				__NE___	{ return ArraySize; }
		
		ND_ constexpr bool  operator == (ArrayView<T> rhs)		C_NE___	{ return ArrayView<T>{*this} == rhs; }
		ND_ constexpr bool  operator != (ArrayView<T> rhs)		C_NE___	{ return ArrayView<T>{*this} != rhs; }
		ND_ constexpr bool  operator >  (ArrayView<T> rhs)		C_NE___	{ return ArrayView<T>{*this} >  rhs; }
		ND_ constexpr bool  operator <  (ArrayView<T> rhs)		C_NE___	{ return ArrayView<T>{*this} <  rhs; }
		ND_ constexpr bool  operator >= (ArrayView<T> rhs)		C_NE___	{ return ArrayView<T>{*this} >= rhs; }
		ND_ constexpr bool  operator <= (ArrayView<T> rhs)		C_NE___	{ return ArrayView<T>{*this} <= rhs; }
		

		constexpr Self&  operator = (const Self &rhs)			noexcept(IsNothrowCopyCtor<T>);
		constexpr Self&  operator = (ArrayView<T> rhs)			noexcept(IsNothrowCopyCtor<T>);
		constexpr Self&  operator = (Self &&rhs)				__NE___;

		constexpr void  assign (const_iterator beginIter, const_iterator endIter)	noexcept(IsNothrowCopyCtor<T>);
		constexpr void  append (ArrayView<T> items)									noexcept(IsNothrowCopyCtor<T>);

		constexpr void  push_back (const T &value)				noexcept(IsNothrowCopyCtor<T>);
		constexpr void  push_back (T &&value)					__NE___;

		template <typename ...Args>
		constexpr T&    emplace_back (Args&& ...args)			noexcept(IsNothrowCopyCtor<T>);

		constexpr void  pop_back ()								__NE___;

		constexpr bool  try_push_back (const T &value)			noexcept(IsNothrowCopyCtor<T>);
		constexpr bool  try_push_back (T&& value)				__NE___;
		
		template <typename ...Args>
		constexpr bool  try_emplace_back (Args&& ...args)		noexcept(IsNothrowCopyCtor<T>);

		constexpr void  insert (usize pos, T &&value)			__NE___;

		constexpr void  resize (usize newSize)					__NE___;
		constexpr void  resize (usize newSize, const T &defaultValue) __NE___;

		constexpr void  clear ()								__NE___;

		constexpr void  erase (usize index)						__NE___;
		constexpr void  fast_erase (usize index)				__NE___;

	private:
		ND_ forceinline bool  _IsMemoryAliased (const_iterator beginIter, const_iterator endIter) C_NE___
		{
			return IsIntersects( begin(), end(), beginIter, endIter );
		}
	};

	
/*
=================================================
	constructor
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray () __NE___
	{
		DEBUG_ONLY( DbgInitMem( data(), SizeOf<T> * capacity() ));
			
		STATIC_ASSERT( alignof(Self) % alignof(T) == 0 );
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (std::initializer_list<T> list) noexcept(IsNothrowCopyCtor<T>) : FixedArray()
	{
		ASSERT( list.size() <= capacity() );
		assign( list.begin(), list.end() );		// throw
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (ArrayView<T> view) noexcept(IsNothrowCopyCtor<T>) : FixedArray()
	{
		ASSERT( view.size() <= capacity() );
		assign( view.begin(), view.end() );		// throw
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (const Self &other) noexcept(IsNothrowCopyCtor<T>) : FixedArray()
	{
		assign( other.begin(), other.end() );	// throw
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (Self &&other) __NE___ : _count{other._count}
	{
		ASSERT( not _IsMemoryAliased( other.begin(), other.end() ));

		CPolicy_t::Replace( _array, other._array, _count );
		other._count = 0;
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (const Self &rhs) noexcept(IsNothrowCopyCtor<T>)
	{
		assign( rhs.begin(), rhs.end() );	// throw
		return *this;
	}
		
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (ArrayView<T> rhs) noexcept(IsNothrowCopyCtor<T>)
	{
		ASSERT( rhs.size() < capacity() );
		assign( rhs.begin(), rhs.end() );	// throw
		return *this;
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (Self &&rhs) __NE___
	{
		ASSERT( not _IsMemoryAliased( rhs.begin(), rhs.end() ));
		
		CPolicy_t::Destroy( _array, _count );
		CPolicy_t::Replace( _array, rhs._array, rhs._count );
		
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
	constexpr void  FixedArray<T,S,CS>::assign (const_iterator beginIter, const_iterator endIter) noexcept(IsNothrowCopyCtor<T>)
	{
		ASSERT( beginIter <= endIter );
		ASSERT( not _IsMemoryAliased( beginIter, endIter ));

		clear();

		for (auto iter = beginIter; _count < capacity() and iter != endIter; ++iter, ++_count)
		{
			PlacementNew<T>( data() + _count, *iter );	// throw
		}
	}
	
/*
=================================================
	append
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::append (ArrayView<T> items) noexcept(IsNothrowCopyCtor<T>)
	{
		for (auto& item : items) {
			push_back( item );		// throw
		}
	}
	
/*
=================================================
	push_back
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::push_back (const T &value) noexcept(IsNothrowCopyCtor<T>)
	{
		ASSERT( _count < capacity() );
		PlacementNew<T>( data() + _count, value );	// throw
		++_count;
	}
	
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::push_back (T &&value) __NE___
	{
		ASSERT( _count < capacity() );
		PlacementNew<T>( data() + _count, RVRef(value) );	// nothrow
		++_count;
	}
	
/*
=================================================
	emplace_back
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename ...Args>
	constexpr T&  FixedArray<T,S,CS>::emplace_back (Args&& ...args) noexcept(IsNothrowCopyCtor<T>)
	{
		ASSERT( _count < capacity() );
		T* ptr = data() + _count;
		PlacementNew<T>( ptr, FwdArg<Args &&>( args )... );		// throw
		++_count;
		return *ptr;
	}
	
/*
=================================================
	pop_back
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::pop_back () __NE___
	{
		ASSERT( _count > 0 );
		--_count;

		CPolicy_t::Destroy( &_array[_count], 1 );
	}
	
/*
=================================================
	try_push_back
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr bool  FixedArray<T,S,CS>::try_push_back (const T &value) noexcept(IsNothrowCopyCtor<T>)
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( data() + _count, value );	// throw
			++_count;
			return true;
		}
		return false;
	}
	
	template <typename T, usize S, typename CS>
	constexpr bool  FixedArray<T,S,CS>::try_push_back (T&& value) __NE___
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( data() + _count, RVRef(value) );	// nothrow
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
	constexpr bool  FixedArray<T,S,CS>::try_emplace_back (Args&& ...args) noexcept(IsNothrowCopyCtor<T>)
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( data() + _count, FwdArg<Args &&>( args )... );		// throw
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
	constexpr void  FixedArray<T,S,CS>::insert (usize pos, T &&value) __NE___
	{
		ASSERT( _count < capacity() );

		pos = Min( pos, _count );
		CPolicy_t::Replace( &_array[pos+1], &_array[pos], _count - pos );

		++_count;
		PlacementNew<T>( &_array[pos], RVRef(value) );	// nothrow
	}
	
/*
=================================================
	resize
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::resize (usize newSize) __NE___
	{
		newSize = Min( newSize, capacity() );

		if ( newSize < _count )
		{
			CPolicy_t::Destroy( &_array[newSize], _count - newSize );
		}
		else
		if ( newSize > _count )
		{
			CPolicy_t::Create( &_array[_count], newSize - _count );
		}

		_count = Count_t(newSize);
	}
	
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::resize (usize newSize, const T &defaultValue) __NE___
	{
		newSize = Min( newSize, capacity() );

		if ( newSize < _count )
		{
			CPolicy_t::Destroy( &_array[newSize], _count - newSize );
		}
		else
		if ( newSize > _count )
		{
			CPolicy_t::Create( &_array[_count], newSize - _count, defaultValue );
		}

		_count = Count_t(newSize);
	}
	
/*
=================================================
	clear
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::clear () __NE___
	{
		CPolicy_t::Destroy( _array, _count );

		_count = 0;
	}
	
/*
=================================================
	fast_erase
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::fast_erase (usize index) __NE___
	{
		ASSERT( index < _count );

		--_count;
		CPolicy_t::Destroy( &_array[index], 1 );

		if ( index != _count )
		{
			// move element from back to 'index'
			CPolicy_t::Replace( &_array[index], &_array[_count], 1 );
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
	constexpr void  FixedArray<T,S,CS>::erase (usize index) __NE___
	{
		ASSERT( index < _count );

		CPolicy_t::Destroy( &_array[index], 1 );

		if ( index+1 < _count )
			CPolicy_t::Replace( &_array[index], &_array[index + 1], _count - index - 1 );

		--_count;
	}


} // AE::Base


namespace std
{
	template <typename T, size_t ArraySize, typename CS>
	struct hash< AE::Base::FixedArray<T, ArraySize, CS> >
	{
		ND_ size_t  operator () (const AE::Base::FixedArray<T, ArraySize, CS> &value) C_NE___
		{
			return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
		}
	};

} // std
