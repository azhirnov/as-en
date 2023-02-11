// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Map emulation on static array with binary/linear search.
	
	Use only for small number of elements.
	Recomended maximum size is 8..16 elements.

	References are not invalidated.

	Exceptions:
		- key & value may throw exceptions (in copy-ctor)
*/

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Containers/ArrayView.h"
#include "base/Memory/CopyPolicy.h"
#include "base/Math/Math.h"
#include "base/Math/Vec.h"
#include "base/CompileTime/TypeList.h"

namespace AE::Base
{

	//
	// Fixed Size Map
	//

	template <typename Key,
			  typename Value,
			  usize ArraySize,
			  typename KeyPolicy   = CopyPolicy::AutoDetect< Key >,
			  typename ValuePolicy = CopyPolicy::AutoDetect< Value >
			 >
	struct FixedMap
	{
		STATIC_ASSERT( ArraySize < 256 );
		STATIC_ASSERT( IsNothrowDtor< Key >);
		STATIC_ASSERT( IsNothrowDtor< Value >);
		//STATIC_ASSERT( IsNothrowMoveCtor< Key >);
		//STATIC_ASSERT( IsNothrowMoveCtor< Value >);

	// types
	private:
		using Self			= FixedMap< Key, Value, ArraySize, KeyPolicy, ValuePolicy >;
		using Index_t		= Conditional< (ArraySize < 0xFF), ubyte, Conditional< (ArraySize < 0xFFFF), ushort, uint >>;
		using PairRef_t		= Pair< const Key &, Value & >;
		using PairCRef_t	= Pair< const Key &, const Value & >;
		using KPolicy_t		= KeyPolicy;
		using VPolicy_t		= ValuePolicy;
		
		static constexpr bool	_IsNothrowCopy = AllNothrowCopyCtor< Key, Value >;


		template <typename PairType>
		struct TPairPtr
		{
		private:
			PairType	_pair;

		public:
			TPairPtr (PairType && p)			__NE___	: _pair{p} {}

			ND_ PairType*		operator -> ()	__NE___	{ return &_pair; }
			ND_ PairType const*	operator -> ()	C_NE___	{ return &_pair; }
		};


		template <bool IsConst>
		struct TIterator
		{
			friend struct FixedMap;

		// types
		private:
			using Iter		= TIterator< IsConst >;
			using MapPtr	= Conditional< IsConst, const Self *, Self *>;
			using PairRef	= Conditional< IsConst, PairCRef_t, PairRef_t >;

		// variables
		private:
			MapPtr		_mapPtr	= null;
			Index_t		_index	= UMax;		// value from '_indices[x]'

		// methods
		public:
			TIterator ()								__NE___ {}
			TIterator (const Iter &)					__NE___ = default;
			TIterator (Iter &&)							__NE___ = default;
			TIterator (MapPtr map, usize idx)			__NE___ : _mapPtr{map}, _index{Index_t(idx)} { ASSERT( _mapPtr != null ); }

			Iter&  operator = (const Iter &)			__NE___ = default;
			Iter&  operator = (Iter &&)					__NE___ = default;

			ND_ bool  operator != (const Iter &rhs)		C_NE___	{ return not (*this == rhs); }
			ND_ bool  operator == (const Iter &rhs)		C_NE___	{ return _mapPtr == rhs._mapPtr and _index == rhs._index; }

			Iter&  operator ++ ()						__NE___
			{
				ASSERT( _mapPtr != null );
				_index = Index_t( Min( _index + 1, _mapPtr->size() ));
				return *this;
			}

			Iter  operator ++ (int)						__NE___
			{
				Iter	res{ *this };
				this->operator++();
				return res;
			}

			Iter&  operator += (usize x)				__NE___
			{
				ASSERT( _mapPtr != null );
				_index = Index_t( Min( _index + x, _mapPtr->size() ));
				return *this;
			}

			ND_ Iter  operator + (usize x)				C_NE___	{ return (Iter{*this} += x); }

			ND_ PairRef		operator * ()				__NE___	{ ASSERT( _mapPtr != null );	return (*_mapPtr)[_index]; }
			ND_ PairCRef_t	operator * ()				C_NE___	{ ASSERT( _mapPtr != null );	return (*_mapPtr)[_index]; }

			ND_ TPairPtr<PairRef>		operator -> ()	__NE___	{ ASSERT( _mapPtr != null );	return (*_mapPtr)[_index]; }
			ND_ TPairPtr<PairCRef_t>	operator -> ()	C_NE___	{ ASSERT( _mapPtr != null );	return (*_mapPtr)[_index]; }
		};


	public:
		using pair_type			= Pair< Key, Value >;
		using key_type			= Key;
		using value_type		= Value;
		using iterator			= TIterator< false >;
		using const_iterator	= TIterator< true >;


	// variables
	private:
		Index_t				_count		= 0;
		mutable Index_t		_indices [ArraySize];
		union {
			key_type		_keyArray  [ArraySize];
			char			_keyBuffer [sizeof(key_type) * ArraySize];		// don't use this field!
		};
		union {
			value_type		_valArray  [ArraySize];
			char			_valBuffer [sizeof(value_type) * ArraySize];	// don't use this field!
		};


	// methods
	public:
		FixedMap ()													__NE___;
		FixedMap (Self &&)											__NE___;
		FixedMap (const Self &)										noexcept(_IsNothrowCopy);

		~FixedMap ()												__NE___	{ clear(); }

		ND_ usize			size ()									C_NE___	{ return _count; }
		ND_ bool			empty ()								C_NE___	{ return _count == 0; }

		ND_ iterator		begin ()								__NE___	{ return iterator{ this, 0 }; }
		ND_ const_iterator	begin ()								C_NE___	{ return const_iterator{ this, 0 }; }
		ND_ iterator		end ()									__NE___	{ return begin() + _count; }
		ND_ const_iterator	end ()									C_NE___	{ return begin() + _count; }
		
		ND_ static constexpr usize	capacity ()						__NE___	{ return ArraySize; }
		
			Self&	operator = (Self &&)							__NE___;
			Self&	operator = (const Self &)						noexcept(_IsNothrowCopy);

		ND_ bool	operator == (const Self &rhs)					C_NE___;
		ND_ bool	operator != (const Self &rhs)					C_NE___	{ return not (*this == rhs); }

			template <typename K, typename V>
			Pair<iterator,bool>  emplace (K&& key, V&& value)		noexcept(_IsNothrowCopy);

			Pair<iterator,bool>  insert (const pair_type &value)	noexcept(_IsNothrowCopy)	{ return emplace( value.first, value.second ); }
			Pair<iterator,bool>  insert (pair_type&& value)			__NE___						{ return emplace( RVRef(value.first), RVRef(value.second) ); }
			
			template <typename K, typename V>
			Pair<iterator,bool>  insert_or_assign (K&& key, V&& value) noexcept(_IsNothrowCopy);
			

		// on overflow 'iterator' will be 'null'
			template <typename K, typename V>
			Pair<iterator,bool>	 try_emplace (K&& key, V&& value)	noexcept(_IsNothrowCopy);

			template <typename K, typename V>
			Pair<iterator,bool>  try_insert_or_assign (K&& key, V&& value) noexcept(_IsNothrowCopy);

			
		// same as operator [] in std
			template <typename KeyType>
		ND_ Value &			operator () (KeyType&& key)				noexcept(_IsNothrowCopy)	{ auto[iter, inst] = emplace( FwdArg<KeyType>(key), Value{} );  return iter->second; }

			template <typename KeyType>
		ND_ const_iterator	find (const KeyType &key)				C_NE___	{ return _Find<const_iterator>( *this, key ); }
			template <typename KeyType>
		ND_ iterator		find (const KeyType &key)				__NE___	{ return _Find<iterator>( *this, key ); }
		
			template <typename KeyType>
		ND_ usize			count (const KeyType &key)				C_NE___	{ return contains( key ) ? 1 : 0; }
			template <typename KeyType>
		ND_ bool			contains (const KeyType &key)			C_NE___;
		
			template <typename KeyType>
			bool			EraseByKey (const KeyType &key)			__NE___;

			iterator		EraseByIter (const iterator &iter)		__NE___;
			const_iterator	EraseByIter (const const_iterator &iter)__NE___;

		ND_ HashVal			CalcHash ()								C_NE___;
		ND_ HashVal			CalcKeyHash ()							C_NE___;
		
			void			clear ()								__NE___;
			void			reserve (usize)							__NE___	{} // ignore

		// cache friendly access to unsorted data

		ND_ PairRef_t	operator [] (usize i)						__NE___;
		ND_ PairCRef_t	operator [] (usize i)						C_NE___;

		ND_ ArrayView<Key>		GetKeyArray ()						C_NE___	{ return { &_keyArray[0], _count }; }
		ND_ ArrayView<Value>	GetValueArray ()					C_NE___	{ return { &_valArray[0], _count }; }

		ND_ Pair<ArrayView<Key>, ArrayView<Value>>  ToArray ()		C_NE___	{ return {GetKeyArray(), GetValueArray()}; }

	private:
		void  _Erase (Index_t idx)									__NE___;

		template <typename IterType, typename MapType, typename KeyType>
		ND_ static IterType  _Find (MapType &map, const KeyType &key) __NE___;

		ND_ forceinline bool _IsMemoryAliased (const Self* other)	C_NE___
		{
			return IsIntersects( this, this+1, other, other+1 );
		}
	};
	

	
/*
=================================================
	constructor
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	FixedMap<K,V,S,KS,VS>::FixedMap () __NE___
	{
		DEBUG_ONLY( DbgInitMem( _indices  ));
		DEBUG_ONLY( DbgInitMem( _keyArray ));
		DEBUG_ONLY( DbgInitMem( _valArray ));
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	FixedMap<K,V,S,KS,VS>::FixedMap (const Self &other) noexcept(_IsNothrowCopy) : _count{ other._count }
	{
		ASSERT( not _IsMemoryAliased( &other ));

		KPolicy_t::Copy( _keyArray, other._keyArray, _count );	// throw
		VPolicy_t::Copy( _valArray, other._valArray, _count );	// throw
		MemCopy( _indices, other._indices, SizeOf<Index_t> * _count );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	FixedMap<K,V,S,KS,VS>::FixedMap (Self &&other) __NE___ : _count{ other._count }
	{
		ASSERT( not _IsMemoryAliased( &other ));
		
		KPolicy_t::Replace( _keyArray, other._keyArray, _count );
		VPolicy_t::Replace( _valArray, other._valArray, _count );
		MemCopy( _indices, other._indices, SizeOf<Index_t> * _count );
		
		other._count = 0;
		DEBUG_ONLY( DbgInitMem( other._indices ));
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	FixedMap<K,V,S,KS,VS>&  FixedMap<K,V,S,KS,VS>::operator = (Self &&rhs) __NE___
	{
		ASSERT( not _IsMemoryAliased( &rhs ));
		
		KPolicy_t::Destroy( _keyArray, _count );
		VPolicy_t::Destroy( _valArray, _count );

		_count = rhs._count;
		
		KPolicy_t::Replace( _keyArray, rhs._keyArray, _count );
		VPolicy_t::Replace( _valArray, rhs._valArray, _count );
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
	template <typename K, typename V, usize S, typename KS, typename VS>
	FixedMap<K,V,S,KS,VS>&  FixedMap<K,V,S,KS,VS>::operator = (const Self &rhs) noexcept(_IsNothrowCopy)
	{
		ASSERT( not _IsMemoryAliased( &rhs ));
		
		KPolicy_t::Destroy( _keyArray, _count );
		VPolicy_t::Destroy( _valArray, _count );

		_count = rhs._count;
		
		KPolicy_t::Copy( _keyArray, rhs._keyArray, _count );	// throw
		VPolicy_t::Copy( _valArray, rhs._valArray, _count );	// throw
		MemCopy( _indices, rhs._indices, SizeOf<Index_t> * _count );

		return *this;
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	bool  FixedMap<K,V,S,KS,VS>::operator == (const Self &rhs) C_NE___
	{
		if ( this == &rhs )
			return true;

		if ( _count != rhs._count )
			return false;

		for (usize i = 0; i < _count; ++i)
		{
			Index_t	lhs_idx = _indices[i];
			Index_t	rhs_idx = rhs._indices[i];

			if_unlikely( not ((_keyArray[lhs_idx] == rhs._keyArray[rhs_idx]) &
							  (_valArray[lhs_idx] == rhs._valArray[rhs_idx])) )
				return false;
		}
		return true;
	}
	
/*
=================================================
	operator []
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	typename FixedMap<K,V,S,KS,VS>::PairRef_t
		FixedMap<K,V,S,KS,VS>::operator [] (usize i) __NE___
	{
		ASSERT( i < _count );
		return PairRef_t{ _keyArray[i], _valArray[i] }; // don't use '_indices'
	}
	
	template <typename K, typename V, usize S, typename KS, typename VS>
	typename FixedMap<K,V,S,KS,VS>::PairCRef_t
		FixedMap<K,V,S,KS,VS>::operator [] (usize i) C_NE___
	{
		ASSERT( i < _count );
		return PairCRef_t{ _keyArray[i], _valArray[i] }; // don't use '_indices'
	}

/*
=================================================
	RecursiveBinarySearch
=================================================
*/
namespace _hidden_
{
	template <typename K1, typename K, typename I>
	struct RecursiveBinarySearch
	{
		forceinline static usize  Find (usize size, const K1 &key, const I* indices, const K* data) __NE___
		{
			usize2	range { 0, size };

			for_likely(; range.x < range.y; )
			{
				usize	mid  = (range.x + range.y) >> 1;	// should not overflow
				auto&	curr = data [indices [mid]];

				range = (curr < key ? usize2{mid + 1, range.y} : usize2{range.x, mid});
			}
			return range.x;
		}
		
		forceinline static usize  LowerBound (usize size, const K1 &key, const I* indices, const K* data) __NE___
		{
			usize2	range { 0, size };

			for_likely(; range.x < range.y; )
			{
				usize	mid  = (range.x + range.y) >> 1;	// should not overflow
				auto&	curr = data [indices [mid]];

				range = (curr < key ? usize2{mid + 1, range.y} : usize2{range.x, mid});
			}
			return range.x;
		}
	};

} // _hidden_
	
/*
=================================================
	try_emplace
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename KeyType, typename ValueType>
	Pair< typename FixedMap<K,V,S,KS,VS>::iterator, bool >
		FixedMap<K,V,S,KS,VS>::try_emplace (KeyType&& key, ValueType&& value) noexcept(_IsNothrowCopy)
	{
		if_unlikely( _count >= capacity() )
			return {};

		return emplace( FwdArg<KeyType>(key), FwdArg<ValueType>(value) );	// throw
	}

/*
=================================================
	emplace
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename KeyType, typename ValueType>
	Pair< typename FixedMap<K,V,S,KS,VS>::iterator, bool >
		FixedMap<K,V,S,KS,VS>::emplace (KeyType&& key, ValueType&& value) noexcept(_IsNothrowCopy)
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, K, Index_t >;

		usize	i = BinarySearch::LowerBound( _count, key, _indices, _keyArray );

		if_likely( i < _count and key == _keyArray[_indices[i]] )
			return { iterator{ this, _indices[i] }, false };

		// insert
		ASSERT( _count < capacity() );

		const usize	j = _count++;
		PlacementNew<key_type  >( &_keyArray[j], FwdArg<KeyType  >(key)   );	// throw
		PlacementNew<value_type>( &_valArray[j], FwdArg<ValueType>(value) );	// throw
			
		if ( i < _count )
			for (usize k = _count-1; k > i; --k) {
				_indices[k] = _indices[k-1];
			}
		else
			i = j;

		_indices[i]	= Index_t(j);
		return { iterator{ this, j }, true };
	}
	
/*
=================================================
	insert_or_assign
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename KeyType, typename ValueType>
	Pair< typename FixedMap<K,V,S,KS,VS>::iterator, bool >
		FixedMap<K,V,S,KS,VS>::insert_or_assign (KeyType&& key, ValueType&& value) noexcept(_IsNothrowCopy)
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, K, Index_t >;

		usize	i = BinarySearch::LowerBound( _count, key, _indices, _keyArray );

		if_likely( i < _count and key == _keyArray[_indices[i]] )
		{
			const Index_t	idx = _indices[i];
			KPolicy_t::Destroy( &_keyArray[idx], 1 );
			VPolicy_t::Destroy( &_valArray[idx], 1 );
			PlacementNew<key_type  >( &_keyArray[idx], FwdArg<KeyType  >(key)   );	// throw
			PlacementNew<value_type>( &_valArray[idx], FwdArg<ValueType>(value) );	// throw
			return { iterator{ this, idx }, false };
		}

		// insert
		ASSERT( _count < capacity() );
	
		const usize	j = _count++;
		PlacementNew<key_type  >( &_keyArray[j], FwdArg<KeyType  >(key)   );	// throw
		PlacementNew<value_type>( &_valArray[j], FwdArg<ValueType>(value) );	// throw
		
		if ( i < _count )
			for (usize k = _count-1; k > i; --k) {
				_indices[k] = _indices[k-1];
			}
		else
			i = j;

		_indices[i]	= Index_t(j);
		return { iterator{ this, j }, true };
	}
	
/*
=================================================
	try_insert_or_assign
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename KeyType, typename ValueType>
	Pair< typename FixedMap<K,V,S,KS,VS>::iterator, bool >
		FixedMap<K,V,S,KS,VS>::try_insert_or_assign (KeyType&& key, ValueType&& value) noexcept(_IsNothrowCopy)
	{
		if_unlikely( _count >= capacity() )
			return {};

		return insert_or_assign( FwdArg<KeyType>(key), FwdArg<ValueType>(value) );	// throw
	}

/*
=================================================
	_Find
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename IterType, typename MapType, typename KeyType>
	IterType  FixedMap<K,V,S,KS,VS>::_Find (MapType &map, const KeyType &key) __NE___
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, K, Index_t >;

		const usize	i = BinarySearch::Find( map._count, key, map._indices, map._keyArray );

		if_likely( i < map._count and key == map._keyArray[map._indices[i]] )
			return IterType{ &map, map._indices[i] };
		
		return map.end();
	}
	
/*
=================================================
	contains
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename KeyType>
	bool  FixedMap<K,V,S,KS,VS>::contains (const KeyType &key) C_NE___
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, K, Index_t >;
		
		const usize	i = BinarySearch::Find( _count, key, _indices, _keyArray );
		
		return (i < _count) and (key == _keyArray[_indices[i]]);
	}
	
/*
=================================================
	EraseByKey
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	template <typename KeyType>
	bool  FixedMap<K,V,S,KS,VS>::EraseByKey (const KeyType &key) __NE___
	{
		using BinarySearch = Base::_hidden_::RecursiveBinarySearch< KeyType, K, Index_t >;

		usize	i = BinarySearch::LowerBound( _count, key, _indices, _keyArray );
		
		if_likely( i < _count and key == _keyArray[_indices[i]] )
		{
			_Erase( _indices[i] );
			return true;
		}
		return false;
	}
	
/*
=================================================
	EraseByIter
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	typename FixedMap<K,V,S,KS,VS>::iterator
		FixedMap<K,V,S,KS,VS>::EraseByIter (const iterator &iter) __NE___
	{
		ASSERT( iter._mapPtr == this );
		
		if_likely( iter._index < _count )
			_Erase( iter._index );
		
		return iter;
	}

	template <typename K, typename V, usize S, typename KS, typename VS>
	typename FixedMap<K,V,S,KS,VS>::const_iterator
		FixedMap<K,V,S,KS,VS>::EraseByIter (const const_iterator &iter) __NE___
	{
		ASSERT( iter._mapPtr == this );
		
		if_likely( iter._index < _count )
			_Erase( iter._index );
		
		return iter;
	}
	
/*
=================================================
	_Erase
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	void  FixedMap<K,V,S,KS,VS>::_Erase (Index_t idx) __NE___
	{
		_keyArray[idx].~K();
		_valArray[idx].~V();
		--_count;
	
		usize	k = 0;
		for (; k <= _count; ++k)
		{
			if_unlikely( _indices[k] == idx )
				break;

			_indices[k] = (_indices[k] == _count ? idx : _indices[k]);
		}

		for (; k < _count; ++k) {
			_indices[k] = (_indices[k+1] == _count ? idx : _indices[k+1]);
		}

		if ( idx != _count )
		{
			KPolicy_t::Replace( &_keyArray[idx], &_keyArray[_count], 1, true );
			VPolicy_t::Replace( &_valArray[idx], &_valArray[_count], 1, true );
		}
		DEBUG_ONLY(
			DbgInitMem( _indices[_count] );
		)
	}

/*
=================================================
	clear
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	void  FixedMap<K,V,S,KS,VS>::clear () __NE___
	{
		KPolicy_t::Destroy( _keyArray, _count );
		VPolicy_t::Destroy( _valArray, _count );

		_count = 0;
		
		DEBUG_ONLY( DbgInitMem( _indices ));
	}
	
/*
=================================================
	CalcHash
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	HashVal  FixedMap<K,V,S,KS,VS>::CalcHash () C_NE___
	{
		HashVal		result = HashOf( size() );

		for (usize i = 0; i < size(); ++i)
		{
			result << HashOf( _keyArray[ _indices[i] ] );
			result << HashOf( _valArray[ _indices[i] ] );
		}
		return result;
	}
	
/*
=================================================
	CalcKeyHash
=================================================
*/
	template <typename K, typename V, usize S, typename KS, typename VS>
	HashVal  FixedMap<K,V,S,KS,VS>::CalcKeyHash () C_NE___
	{
		HashVal		result = HashOf( size() );

		for (usize i = 0; i < size(); ++i) {
			result << HashOf( _keyArray[ _indices[i] ] );
		}
		return result;
	}

} // AE::Base


namespace std
{
	template <typename Key, typename Value, size_t ArraySize, typename KS, typename VS>
	struct hash< AE::Base::FixedMap<Key, Value, ArraySize, KS, VS> >
	{
		ND_ size_t  operator () (const AE::Base::FixedMap<Key, Value, ArraySize, KS, VS> &value) C_NE___
		{
			return size_t(value.CalcHash());
		}
	};

} // std
