// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Set emulation on static array with binary/linear search.
	Use only for small number of elements.

	Recomended maximum size is 8..16 elements, .
*/

#pragma once

#include "base/Containers/FixedMap.h"

namespace AE::Base
{

	//
	// Fixed Size Set
	//

	template <typename Value,
			  usize ArraySize,
			  typename Policy = CopyPolicy::AutoDetect< Value >
			 >
	struct FixedSet
	{
		STATIC_ASSERT( ArraySize < 256 );

	// types
	private:
		using Self			= FixedSet< Value, ArraySize, Policy >;
		using Index_t		= Conditional< (ArraySize < 0xFF), ubyte, Conditional< (ArraySize < 0xFFFF), ushort, uint >>;
		using CPolicy_t		= Policy;

	public:
		using value_type		= Value;
		using iterator			= const Value *;
		using const_iterator	= iterator;
		

	// variables
	private:
		Index_t				_count		= 0;
		mutable Index_t		_indices [ArraySize];
		union {
			value_type		_array  [ArraySize];
			char			_buffer [sizeof(value_type) * ArraySize];		// don't use this field!
		};


	// methods
	public:
		FixedSet ();
		FixedSet (Self &&);
		FixedSet (const Self &);

		~FixedSet ()	{ clear(); }

		ND_ usize		size ()		const	{ return _count; }
		ND_ bool		empty ()	const	{ return _count == 0; }

		ND_ iterator	begin ()	const	{ return &_array[0]; }
		ND_ iterator	end ()		const	{ return begin() + _count; }
		
		ND_ static constexpr usize	capacity ()		{ return ArraySize; }

			Self&	operator = (Self &&);
			Self&	operator = (const Self &);

		ND_ bool	operator == (const Self &rhs) const;
		ND_ bool	operator != (const Self &rhs) const		{ return not (*this == rhs); }

			template <typename ValueType>
			Pair<iterator,bool>  emplace (ValueType&& value);

			Pair<iterator,bool>  insert (const Value &value)	{ return emplace( value ); }
			Pair<iterator,bool>  insert (Value&& value)			{ return emplace( RVRef(value) ); }
			
			template <typename ValueType>
			Pair<iterator,bool>  insert_or_assign (ValueType&& value);
			
			template <typename ValueType>
			bool		try_insert (ValueType&& value);
			
			template <typename KeyType>
			bool		erase (const KeyType &key);

			template <typename KeyType>
		ND_ iterator	find (const KeyType &key)		const;
		
			template <typename KeyType>
		ND_ usize		count (const KeyType &key)		const	{ return contains( key ) ? 1 : 0; }
		
			template <typename KeyType>
		ND_ bool		contains (const KeyType &key)	const;

		ND_ HashVal		CalcHash ()	const;

			void		clear ();
			void		reserve (usize)		{}

		// cache friendly access to unsorted data
			
		ND_ explicit operator ArrayView<Value> () const	{ return { &_array[0], size() }; }

		ND_ Value const&	operator [] (usize i) const;

	private:
		ND_ forceinline bool _IsMemoryAliased (const Self* other) const
		{
			return IsIntersects( this, this+1, other, other+1 );
		}
	};
	


/*
=================================================
	constructor
=================================================
*/
	template <typename V, usize S, typename CS>
	FixedSet<V,S,CS>::FixedSet ()
	{
		DEBUG_ONLY( DbgInitMem( _indices ));
		DEBUG_ONLY( DbgInitMem( _array   ));
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename V, usize S, typename CS>
	FixedSet<V,S,CS>::FixedSet (const Self &other) : _count{ other._count }
	{
		ASSERT( not _IsMemoryAliased( &other ));

		CPolicy_t::Copy( _array, other._array, _count );
		MemCopy( _indices, other._indices, SizeOf<Index_t> * _count );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename V, usize S, typename CS>
	FixedSet<V,S,CS>::FixedSet (Self &&other) : _count{ other._count }
	{
		ASSERT( not _IsMemoryAliased( &other ));
		
		CPolicy_t::Replace( _array, other._array, _count );
		MemCopy( _indices, other._indices, SizeOf<Index_t> * _count );

		other._count = 0;
		DEBUG_ONLY( DbgInitMem( other._indices ));
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename V, usize S, typename CS>
	FixedSet<V,S,CS>&  FixedSet<V,S,CS>::operator = (Self &&rhs)
	{
		ASSERT( not _IsMemoryAliased( &rhs ));
		
		CPolicy_t::Destroy( _array, _count );

		_count = rhs._count;
		
		CPolicy_t::Replace( _array, rhs._array, _count );
		MemCopy( _indices, rhs._indices, SizeOf<Index_t> * _count );
		
		rhs._count = 0;
		DEBUG_ONLY( DbgInitMem( rhs._indices ));

		return *this;
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename V, usize S, typename CS>
	FixedSet<V,S,CS>&  FixedSet<V,S,CS>::operator = (const Self &rhs)
	{
		ASSERT( not _IsMemoryAliased( &rhs ));
		
		CPolicy_t::Destroy( _array, _count );

		_count = rhs._count;
		
		CPolicy_t::Copy( _array, rhs._array, _count );
		MemCopy( _indices, rhs._indices, SizeOf<Index_t> * _count );

		return *this;
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename V, usize S, typename CS>
	bool  FixedSet<V,S,CS>::operator == (const Self &rhs) const
	{
		if ( this == &rhs )
			return true;

		if ( _count != rhs._count )
			return false;

		for (usize i = 0; i < _count; ++i)
		{
			if_unlikely( not (_array[_indices[i]] == rhs._array[rhs._indices[i]]) )
				return false;
		}
		return true;
	}
	
/*
=================================================
	operator []
=================================================
*/
	template <typename V, usize S, typename CS>
	V const&  FixedSet<V,S,CS>::operator [] (usize i) const
	{
		ASSERT( i < _count );
		return _array[i]; // don't use '_indices'
	}
	
/*
=================================================
	try_insert
=================================================
*/
	template <typename V, usize S, typename CS>
	template <typename ValueType>
	bool  FixedSet<V,S,CS>::try_insert (ValueType&& value)
	{
		if_likely( _count < capacity() )
		{
			emplace( FwdArg<ValueType>(value) );
			return true;
		}
		return false;
	}

/*
=================================================
	emplace
=================================================
*/
	template <typename V, usize S, typename CS>
	template <typename ValueType>
	Pair< typename FixedSet<V,S,CS>::iterator, bool >
		FixedSet<V,S,CS>::emplace (ValueType&& value)
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< ValueType, value_type, Index_t >;

		usize	i = BinarySearch::LowerBound( _count, value, _indices, _array );

		if_likely( i < _count and value == _array[_indices[i]] )
			return { iterator{ &_array[_indices[i]] }, false };

		// insert
		ASSERT( _count < capacity() );

		const usize	j = _count++;
		PlacementNew<value_type>( &_array[j], FwdArg<ValueType>(value) );
			
		if ( i < _count )
			for (usize k = _count-1; k > i; --k) {
				_indices[k] = _indices[k-1];
			}
		else
			i = j;

		_indices[i]	= Index_t(j);
		return { iterator{ &_array[j] }, true };
	}
	
/*
=================================================
	insert_or_assign
=================================================
*/
	template <typename V, usize S, typename CS>
	template <typename ValueType>
	Pair< typename FixedSet<V,S,CS>::iterator, bool >
		FixedSet<V,S,CS>::insert_or_assign (ValueType&& value)
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< ValueType, value_type, Index_t >;

		usize	i = BinarySearch::LowerBound( _count, value, _indices, _array );

		if_likely( i < _count and value == _array[_indices[i]] )
		{
			_array[_indices[i]] = FwdArg<ValueType>(value);
			return { iterator{ &_array[_indices[i]] }, false };
		}

		// insert
		ASSERT( _count < capacity() );
	
		const usize	j = _count++;
		PlacementNew<value_type>( &_array[j], FwdArg<ValueType>(value) );
		
		if ( i < _count )
			for (usize k = _count-1; k > i; --k) {
				_indices[k] = _indices[k-1];
			}
		else
			i = j;

		_indices[i]	= Index_t(j);
		return { iterator{ &_array[j] }, true };
	}
	
/*
=================================================
	find
=================================================
*/
	template <typename V, usize S, typename CS>
	template <typename KeyType>
	typename FixedSet<V,S,CS>::iterator
		FixedSet<V,S,CS>::find (const KeyType &key) const
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, value_type, Index_t >;

		const usize	i = BinarySearch::Find( _count, key, _indices, _array );

		if_likely( i < _count and key == _array[_indices[i]] )
			return iterator{ &_array[_indices[i]] };
		
		return end();
	}
	
/*
=================================================
	contains
=================================================
*/
	template <typename V, usize S, typename CS>
	template <typename KeyType>
	bool  FixedSet<V,S,CS>::contains (const KeyType &key) const
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, value_type, Index_t >;
		
		const usize	i = BinarySearch::Find( _count, key, _indices, _array );
		
		return (i < _count) and (key == _array[_indices[i]]);
	}
	
/*
=================================================
	erase
=================================================
*/
	template <typename V, usize S, typename CS>
	template <typename KeyType>
	bool  FixedSet<V,S,CS>::erase (const KeyType &key)
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, value_type, Index_t >;

		usize	i = BinarySearch::LowerBound( _count, key, _indices, _array );
		
		if_likely( i < _count and key == _array[_indices[i]] )
		{
			_array[i].~V();
			
			for (usize k = 0; k < _count; ++k) {
				_indices[k] = _indices[k] >= i ? _indices[k]-1 : _indices[k];
			}
			
			--_count;
			CPolicy_t::Replace( &_array[i], &_array[i+1], _count - i );

			DEBUG_ONLY(
				DbgInitMem( _indices[_count] );
				DbgInitMem( _array[_count] );
			)
			return true;
		}
		return false;
	}

/*
=================================================
	clear
=================================================
*/
	template <typename V, usize S, typename CS>
	void  FixedSet<V,S,CS>::clear ()
	{
		CPolicy_t::Destroy( _array, _count );
		DEBUG_ONLY( DbgInitMem( _indices ));

		_count = 0;
	}
	
/*
=================================================
	CalcHash
=================================================
*/
	template <typename V, usize S, typename CS>
	HashVal  FixedSet<V,S,CS>::CalcHash () const
	{
		HashVal		result = HashOf( size() );

		for (usize i = 0; i < size(); ++i)
		{
			result << HashOf( _array[ _indices[i] ] );
		}
		return result;
	}

} // AE::Base


namespace std
{
	template <typename Value, size_t ArraySize, typename CS>
	struct hash< AE::Base::FixedSet<Value, ArraySize, CS> >
	{
		ND_ size_t  operator () (const AE::Base::FixedSet<Value, ArraySize, CS> &value) const
		{
			return size_t(value.CalcHash());
		}
	};

}	// std
