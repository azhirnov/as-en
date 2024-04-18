// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Basic/Deserializer.h"

namespace AE::Serializing
{

	template <typename T>
	bool  Deserializer::_DeserializeObj (INOUT T &obj)
	{
		if ( factory )
		{
			// read 'SerializedID' and then deserialize
			return factory->Deserialize( *this, allocator, INOUT obj );
		}

		if constexpr( IsBaseOf< ISerializable, T >)
			return obj.Deserialize( *this );
		else
		{
			DBG_WARNING( "unknown type" );
			return false;
		}
	}


	template <typename ...Args>
	bool  Deserializer::operator () (INOUT Args& ...args) __NE___
	{
		return _RecursiveDeserialize( INOUT args... );
	}


	template <typename Arg0, typename ...Args>
	bool  Deserializer::_RecursiveDeserialize (INOUT Arg0 &arg0, INOUT Args& ...args)
	{
		#if AE_DEBUG_SERIALIZER
			uint	dbg_idx = 0;
			ASSERT( stream.Read( OUT dbg_idx ));
			if ( _dbgCounter == 0 ) _dbgCounter = dbg_idx;
			ASSERT( dbg_idx == _dbgCounter );
			++_dbgCounter;
		#endif

		StaticAssert( not IsConst<Arg0> );

		bool res = _Deserialize( INOUT arg0 );

		if constexpr( CountOf<Args...>() > 0 )
			return res and _RecursiveDeserialize( INOUT args... );
		else
			return res;
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT T &value)
	{
		if constexpr( IsTriviallySerializable<T> )
			return stream.Read( OUT value );
		else
			return _DeserializeObj( INOUT value );
	}


	template <typename F, typename S>
	bool  Deserializer::_Deserialize (INOUT Pair<F,S> &value)
	{
		return _Deserialize( INOUT value.first ) and _Deserialize( INOUT value.second );
	}


	template <usize N>
	bool  Deserializer::_Deserialize (INOUT BitSet<N> &value)
	{
		StaticAssert( N <= 64 );

		if constexpr( N <= 32 )
		{
			uint	bits = 0;
			bool	res  = _Deserialize( INOUT bits );
			value = BitSet<N>{ bits };
			return res;
		}
		else
		{
			ulong	bits = 0;
			bool		res  = _Deserialize( INOUT bits );
			value = BitSet<N>{ bits };
			return res;
		}
	}


	template <typename T, typename A>
	bool  Deserializer::_Deserialize (INOUT Array<T,A> &arr)
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT( count <= MaxArrayLength );

		if_unlikely( (not res) or (count > MaxArrayLength) )
			return false;

		NOTHROW_ERR( arr.resize( count ));

		if constexpr( IsTriviallySerializable<T> )
			return (count == 0) or stream.Read( OUT arr.data(), ArraySizeOf(arr) );
		else
		{
			for (usize i = 0; res and (i < arr.size()); ++i) {
				res = _Deserialize( INOUT arr[i] );
			}
			return res;
		}
	}


	template <typename T, usize S>
	bool  Deserializer::_Deserialize (INOUT StaticArray<T,S> &arr)
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT( count <= S );

		if_unlikely( (not res) or (count > S) )
			return false;

		if constexpr( IsTriviallySerializable<T> )
			return (count == 0) or stream.Read( OUT arr.data(), SizeOf<T> * count );
		else
		{
			for (uint i = 0; res and (i < count); ++i) {
				res = _Deserialize( INOUT arr[i] );
			}
			return res;
		}
	}


	template <typename T, usize S>
	bool  Deserializer::_Deserialize (INOUT FixedArray<T,S> &arr)
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT( count <= S );

		if_unlikely( (not res) or (count > S) )
			return false;

		arr.resize( count );

		if constexpr( IsTriviallySerializable<T> )
			return (count == 0) or stream.Read( OUT arr.data(), ArraySizeOf(arr) );
		else
		{
			for (usize i = 0; res and (i < arr.size()); ++i) {
				res = _Deserialize( INOUT arr[i] );
			}
			return res;
		}
	}


	inline bool  Deserializer::_Deserialize (INOUT String &str)
	{
		uint	len = 0;
		bool	res = stream.Read( OUT len );
		ASSERT( len <= MaxStringLength );

		if_unlikely( (not res) or (len > MaxStringLength) )
			return false;

		NOTHROW_ERR( str.resize( len ));

		return (len == 0) or stream.Read( OUT str.data(), StringSizeOf(str) );
	}


	template <typename T, usize S>
	bool  Deserializer::_Deserialize (INOUT TFixedString<T,S> &str)
	{
		uint	len = 0;
		bool	res = stream.Read( OUT len );
		ASSERT( len <= S );

		if_unlikely( (not res) or (len > S) )
			return false;

		str.resize( len );
		return (len == 0) or stream.Read( OUT str.data(), SizeOf<T> * len );
	}


	template <typename T, int I, glm::qualifier Q>
	bool  Deserializer::_Deserialize (INOUT TVec<T,I,Q> &vec)
	{
		return stream.Read( OUT &vec.x, SizeOf<T>*I );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT Rectangle<T> &rect)
	{
		return stream.Read( OUT rect.data(), SizeOf<T>*4 );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT RGBAColor<T> &col)
	{
		return stream.Read( OUT col.data(), Sizeof(col) );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT HSVColor &col)
	{
		return stream.Read( OUT col.data(), Sizeof(col) );
	}


	template <usize Size, uint UID, uint Seed>
	bool  Deserializer::_Deserialize (INOUT NamedID<Size, UID, true, Seed> &id)
	{
		uint	hash = 0;
		bool	res  = stream.Read( OUT hash );

		id = NamedID<Size, UID, true, Seed>{ HashVal32{ hash }};
		return res;
	}


	template <usize Size, uint UID, uint Seed>
	bool  Deserializer::_Deserialize (INOUT NamedID<Size, UID, false, Seed> &id)
	{
	#if AE_SERIALIZE_HASH_ONLY
		uint	hash = 0;
		bool	res  = stream.Read( OUT hash );

		id = NamedID<Size, UID, false, Seed>{ HashVal32{ hash }};
		return res;

	#else
		FixedString<Size>	str;
		if ( not _Deserialize( str ))
			return false;

		id = NamedID<Size, UID, false, Seed>{ str };
		return true;
	#endif
	}


	template <typename K, typename V, typename MapType>
	bool  Deserializer::_DeserializeMap (INOUT MapType &map, const uint maxCount)
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT( count <= maxCount );

		if_unlikely( (not res) or (count > maxCount) )
			return false;

		TRY{
			map.reserve( count );	// throw

			for (uint i = 0; res and (i < count); ++i)
			{
				K	key		= {};
				V	value	= {};
				res = (_Deserialize( OUT key ) and _Deserialize( OUT value ));
				map.insert_or_assign( RVRef(key), RVRef(value) );	// throw
			}
			ASSERT( map.size() == count );
			return res;
		}
		CATCH_ALL(
			Reconstruct( INOUT map );
			return false;
		)
	}


	template <typename T, typename SetType>
	bool  Deserializer::_DeserializeSet (INOUT SetType &set, const uint maxCount)
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT( count <= maxCount );

		if_unlikely( (not res) or (count > maxCount) )
			return false;

		TRY{
			set.reserve( count );	// throw

			for (uint i = 0; res and (i < count); ++i)
			{
				T	value = {};
				res = _Deserialize( OUT value );
				set.insert( RVRef(value) );		// throw
			}
			ASSERT( set.size() == count );
			return res;
		}
		CATCH_ALL(
			Reconstruct( INOUT set );
			return false;
		)
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT Optional<T> &value)
	{
		bool	has_value;
		bool	res		= stream.Read( OUT has_value );

		if ( res and has_value )
			return _Deserialize( INOUT value.emplace() );

		return res;
	}


	template <usize I, typename ...Types>
	bool  Deserializer::_RecursiveDeserializeTuple (INOUT Tuple<Types...> &tuple)
	{
		bool	res = _Deserialize( INOUT tuple.template Get<I>() );

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveDeserializeTuple< I+1 >( INOUT tuple );
		else
			return res;
	}


	template <usize I, usize S, typename ...Types>
	bool  Deserializer::_RecursiveDeserializeTupleArray (INOUT FixedTupleArray<S, Types...> &arr)
	{
		auto*	ptr		= arr.template data<I>();
		bool	res		= true;

		for (usize i = 0; res and (i < arr.size()); ++i) {
			res = _Deserialize( INOUT ptr[i] );
		}

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveDeserializeTupleArray< I+1 >( INOUT arr );
		else
			return res;
	}


	template <typename ...Types>
	bool  Deserializer::_Deserialize (INOUT Tuple<Types...> &tuple)
	{
		return _RecursiveDeserializeTuple<0>( INOUT tuple );
	}


	template <usize S, typename ...Types>
	bool  Deserializer::_Deserialize (INOUT FixedTupleArray<S,Types...> &arr)
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT( count <= S );

		if_unlikely( (not res) or (count > S) )
			return false;

		arr.resize( count );
		return _RecursiveDeserializeTupleArray<0>( INOUT arr );
	}


	template <typename V, typename D, typename S>
	bool  Deserializer::_Deserialize (INOUT PhysicalQuantity<V,D,S> &value)
	{
		return _Deserialize( INOUT value.GetNonScaledRef() );
	}


	template <typename Qt, int I, glm::qualifier Ql>
	bool  Deserializer::_Deserialize (INOUT PhysicalQuantityVec<Qt,I,Ql> &value)
	{
		return _Deserialize( INOUT value.GetNonScaledRef() );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT ArrayView<T> &arr)
	{
		CHECK_ERR( allocator );

		uint	count	= 0;
		bool	res		= stream.Read( OUT count );

		if_unlikely( (not res) or (count > MaxArrayLength) )
			return false;

		if_unlikely( count == 0 )
		{
			arr = {};
			return true;
		}

		T*	dst = Cast<T>( allocator->Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> }));
		if_unlikely( dst == null )
			return false;

		arr = ArrayView<T>{ dst, count };

		if constexpr( IsTriviallySerializable<T> )
		{
			res = stream.Read( OUT dst, SizeOf<T> * count );
		}
		else
		{
			for (uint i = 0; res and (i < count); ++i)
			{
				PlacementNew<T>( OUT dst + i );
				res = _Deserialize( INOUT dst[i] );
			}
		}
		return res;
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT BasicStringView<T> &str)
	{
		CHECK_ERR( allocator );

		uint	len		= 0;
		bool	res		= stream.Read( OUT len );
		ASSERT( len <= MaxStringLength );

		if_unlikely( (not res) or (len > MaxStringLength) )
			return false;

		if_unlikely( len == 0 )
		{
			str = {};
			return true;
		}

		T*	dst = Cast<T>( allocator->Allocate( SizeAndAlign{ SizeOf<T> * (len+1), AlignOf<T> }));
		if_unlikely( dst == null )
			return false;

		str = BasicStringView<T>{ dst, len };
		res = stream.Read( OUT dst, SizeOf<T> * len );

		dst[len] = T(0);
		return res;
	}


} // AE::Serializing
