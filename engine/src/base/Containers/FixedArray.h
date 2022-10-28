// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		constexpr FixedArray ();
		constexpr FixedArray (std::initializer_list<T> list);
		constexpr FixedArray (ArrayView<T> view);
		constexpr FixedArray (const Self &other);
		constexpr FixedArray (Self &&other);

		~FixedArray ()	{ clear(); }


		ND_ constexpr operator ArrayView<T> ()					const	{ return ArrayView<T>{ data(), size() }; }
		ND_ constexpr ArrayView<T>		ToArrayView()			const	{ return *this; }

		ND_ constexpr usize				size ()					const	{ return _count; }
		ND_ constexpr bool				empty ()				const	{ return _count == 0; }
		ND_ constexpr T *				data ()							{ return std::addressof( _array[0] ); }
		ND_ constexpr T const *			data ()					const	{ return std::addressof( _array[0] ); }

		ND_ constexpr T &				operator [] (usize i)			{ ASSERT( i < _count );  return _array[i]; }
		ND_ constexpr T const &			operator [] (usize i)	const	{ ASSERT( i < _count );  return _array[i]; }

		ND_ constexpr iterator			begin ()						{ return data(); }
		ND_ constexpr const_iterator	begin ()				const	{ return data(); }
		ND_ constexpr iterator			end ()							{ return data() + _count; }
		ND_ constexpr const_iterator	end ()					const	{ return data() + _count; }

		ND_ constexpr T &				front ()						{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T const&			front ()				const	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ constexpr T &				back ()							{ ASSERT( _count > 0 );  return _array[_count-1]; }
		ND_ constexpr T const&			back ()					const	{ ASSERT( _count > 0 );  return _array[_count-1]; }
		
		ND_ static constexpr usize		capacity ()						{ return ArraySize; }
		
		ND_ constexpr bool  operator == (ArrayView<T> rhs)		const	{ return ArrayView<T>{*this} == rhs; }
		ND_ constexpr bool  operator != (ArrayView<T> rhs)		const	{ return ArrayView<T>{*this} != rhs; }
		ND_ constexpr bool  operator >  (ArrayView<T> rhs)		const	{ return ArrayView<T>{*this} >  rhs; }
		ND_ constexpr bool  operator <  (ArrayView<T> rhs)		const	{ return ArrayView<T>{*this} <  rhs; }
		ND_ constexpr bool  operator >= (ArrayView<T> rhs)		const	{ return ArrayView<T>{*this} >= rhs; }
		ND_ constexpr bool  operator <= (ArrayView<T> rhs)		const	{ return ArrayView<T>{*this} <= rhs; }
		

		constexpr Self&  operator = (const Self &rhs);
		constexpr Self&  operator = (ArrayView<T> rhs);
		constexpr Self&  operator = (Self &&rhs);

		constexpr void  assign (const_iterator beginIter, const_iterator endIter);
		constexpr void  append (ArrayView<T> items);

		constexpr void  push_back (const T &value);
		constexpr void  push_back (T &&value);

		template <typename ...Args>
		constexpr T&    emplace_back (Args&& ...args);

		constexpr void  pop_back ();

		constexpr bool  try_push_back (const T &value);
		constexpr bool  try_push_back (T&& value);
		
		template <typename ...Args>
		constexpr bool  try_emplace_back (Args&& ...args);

		constexpr void  insert (usize pos, T &&value);

		constexpr void  resize (usize newSize);
		constexpr void  resize (usize newSize, const T &defaultValue);

		constexpr void  clear ();

		constexpr void  erase (usize index);
		constexpr void  fast_erase (usize index);

	private:
		ND_ forceinline bool  _IsMemoryAliased (const_iterator beginIter, const_iterator endIter) const
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
	constexpr FixedArray<T,S,CS>::FixedArray ()
	{
		DEBUG_ONLY( DbgInitMem( data(), SizeOf<T> * capacity() ));
			
		STATIC_ASSERT( alignof(Self) % alignof(T) == 0 );
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (std::initializer_list<T> list) : FixedArray()
	{
		ASSERT( list.size() <= capacity() );
		assign( list.begin(), list.end() );
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (ArrayView<T> view) : FixedArray()
	{
		ASSERT( view.size() <= capacity() );
		assign( view.begin(), view.end() );
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (const Self &other) : FixedArray()
	{
		assign( other.begin(), other.end() );
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>::FixedArray (Self &&other) : _count{other._count}
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
	constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (const Self &rhs)
	{
		assign( rhs.begin(), rhs.end() );
		return *this;
	}
		
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (ArrayView<T> rhs)
	{
		ASSERT( rhs.size() < capacity() );
		assign( rhs.begin(), rhs.end() );
		return *this;
	}
	
	template <typename T, usize S, typename CS>
	constexpr FixedArray<T,S,CS>&  FixedArray<T,S,CS>::operator = (Self &&rhs)
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
	constexpr void  FixedArray<T,S,CS>::assign (const_iterator beginIter, const_iterator endIter)
	{
		ASSERT( beginIter <= endIter );
		ASSERT( not _IsMemoryAliased( beginIter, endIter ));

		clear();

		for (auto iter = beginIter; _count < capacity() and iter != endIter; ++iter, ++_count)
		{
			PlacementNew<T>( data() + _count, *iter );
		}
	}
	
/*
=================================================
	append
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::append (ArrayView<T> items)
	{
		for (auto& item : items) {
			push_back( item );
		}
	}
	
/*
=================================================
	push_back
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::push_back (const T &value)
	{
		ASSERT( _count < capacity() );
		PlacementNew<T>( data() + (_count++), value );
	}
	
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::push_back (T &&value)
	{
		ASSERT( _count < capacity() );
		PlacementNew<T>( data() + (_count++), RVRef(value) );
	}
	
/*
=================================================
	emplace_back
=================================================
*/
	template <typename T, usize S, typename CS>
	template <typename ...Args>
	constexpr T&  FixedArray<T,S,CS>::emplace_back (Args&& ...args)
	{
		ASSERT( _count < capacity() );
		T* ptr = data() + (_count++);
		PlacementNew<T>( ptr, FwdArg<Args &&>( args )... );
		return *ptr;
	}
	
/*
=================================================
	pop_back
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::pop_back ()
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
	constexpr bool  FixedArray<T,S,CS>::try_push_back (const T &value)
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( data() + (_count++), value );
			return true;
		}
		return false;
	}
	
	template <typename T, usize S, typename CS>
	constexpr bool  FixedArray<T,S,CS>::try_push_back (T&& value)
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( data() + (_count++), RVRef(value) );
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
	constexpr bool  FixedArray<T,S,CS>::try_emplace_back (Args&& ...args)
	{
		if_likely( _count < capacity() )
		{
			PlacementNew<T>( data() + (_count++), FwdArg<Args &&>( args )... );
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
	constexpr void  FixedArray<T,S,CS>::insert (usize pos, T &&value)
	{
		ASSERT( _count < capacity() );

		pos = Min( pos, _count );
		CPolicy_t::Replace( &_array[pos+1], &_array[pos], _count - pos );

		++_count;
		PlacementNew<T>( &_array[pos], RVRef(value) );
	}
	
/*
=================================================
	resize
=================================================
*/
	template <typename T, usize S, typename CS>
	constexpr void  FixedArray<T,S,CS>::resize (usize newSize)
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
	constexpr void  FixedArray<T,S,CS>::resize (usize newSize, const T &defaultValue)
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
	constexpr void  FixedArray<T,S,CS>::clear ()
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
	constexpr void  FixedArray<T,S,CS>::fast_erase (usize index)
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
	constexpr void  FixedArray<T,S,CS>::erase (usize index)
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
		ND_ size_t  operator () (const AE::Base::FixedArray<T, ArraySize, CS> &value) const
		{
			return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
		}
	};

} // std
