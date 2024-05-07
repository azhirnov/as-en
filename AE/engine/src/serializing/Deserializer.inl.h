// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Deserializer.h"

namespace AE::Serializing
{

	template <typename T>
	bool  Deserializer::_DeserializeObj (INOUT T &obj) __NE___
	{
		if constexpr( IsBaseOf< ISerializable, T >)
			return obj.Deserialize( *this );
		else
		{
			if ( factory )
			{
				// read 'SerializedID' and then deserialize
				return factory->Deserialize( *this, allocator, INOUT obj );
			}
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
	bool  Deserializer::_RecursiveDeserialize (INOUT Arg0 &arg0, INOUT Args& ...args) __NE___
	{
		#if AE_DEBUG_SERIALIZER
			uint	dbg_idx = 0;
			ASSERT( stream.Read( OUT dbg_idx ));
			if ( _dbgCounter == 0 ) _dbgCounter = dbg_idx;
			ASSERT_Eq( dbg_idx, _dbgCounter );
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
	bool  Deserializer::_Deserialize (INOUT T &value) __NE___
	{
		if constexpr( IsTriviallySerializable<T> )
			return stream.Read( OUT value );
		else
			return _DeserializeObj( INOUT value );
	}


	template <uint F, uint B, typename T>
	bool  Deserializer::_Deserialize (INOUT PackedBits<F,B,T> &value) __NE___
	{
		const uint	bit_cnt = value.BitCount();
		BitType		bits	= _packedBits;

		if_likely( _bitCount >= bit_cnt )
		{
			_bitCount	-= bit_cnt;
			_packedBits	>>= bit_cnt;
		}
		else
		{
			// read next bits
			if_unlikely( not stream.Read( OUT _packedBits ))
				return false;

			bits |= (_packedBits << _bitCount);

			_packedBits	>>= (bit_cnt - _bitCount);
			_bitCount	+= CT_SizeOfInBits<BitType> - bit_cnt;
		}

		value.FromBits( bits );
		return true;
	}


	template <typename F, typename S>
	bool  Deserializer::_Deserialize (INOUT Pair<F,S> &value) __NE___
	{
		return _Deserialize( INOUT value.first ) and _Deserialize( INOUT value.second );
	}


	template <usize N>
	bool  Deserializer::_Deserialize (INOUT BitSet<N> &value) __NE___
	{
		PackedBits<0,N,BitType>	temp;

		bool	res = _Deserialize( OUT temp );
		value = BitSet<N>{ BitType{temp} };

		return res;
	}


	template <typename T, typename A>
	bool  Deserializer::_Deserialize (INOUT Array<T,A> &arr) __NE___
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, MaxArrayLength );

		if_unlikely( (not res) or (count > MaxArrayLength) )
			return false;

		NOTHROW_ERR( arr.resize( count ));

		if constexpr( IsTriviallySerializable<T> )
			return (count == 0) or stream.Read( OUT arr.data(), ArraySizeOf(arr) );
		else{
			for (usize i = 0; res and (i < arr.size()); ++i) {
				res = _Deserialize( INOUT arr[i] );
			}
			return res;
		}
	}


	template <typename T, usize S>
	bool  Deserializer::_Deserialize (INOUT StaticArray<T,S> &arr) __NE___
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, S );

		if_unlikely( (not res) or (count > S) )
			return false;

		if constexpr( IsTriviallySerializable<T> )
			return (count == 0) or stream.Read( OUT arr.data(), SizeOf<T> * count );
		else{
			for (uint i = 0; res and (i < count); ++i) {
				res = _Deserialize( INOUT arr[i] );
			}
			return res;
		}
	}


	template <typename T, usize S>
	bool  Deserializer::_Deserialize (INOUT FixedArray<T,S> &arr) __NE___
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, S );

		if_unlikely( (not res) or (count > S) )
			return false;

		arr.resize( count );

		if constexpr( IsTriviallySerializable<T> )
			return (count == 0) or stream.Read( OUT arr.data(), ArraySizeOf(arr) );
		else{
			for (usize i = 0; res and (i < arr.size()); ++i) {
				res = _Deserialize( INOUT arr[i] );
			}
			return res;
		}
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT BasicString<T> &str) __NE___
	{
		uint	len = 0;
		bool	res = stream.Read( OUT len );
		ASSERT_LE( len, MaxStringLength );

		if_unlikely( (not res) or (len > MaxStringLength) )
			return false;

		NOTHROW_ERR( str.resize( len ));

		return (len == 0) or stream.Read( OUT str.data(), StringSizeOf(str) );
	}


	template <typename T, usize S>
	bool  Deserializer::_Deserialize (INOUT TFixedString<T,S> &str) __NE___
	{
		uint	len = 0;
		bool	res = stream.Read( OUT len );
		ASSERT_LE( len, S );

		if_unlikely( (not res) or (len > S) )
			return false;

		str.resize( len );
		return (len == 0) or stream.Read( OUT str.data(), SizeOf<T> * len );
	}


	template <typename T, int I, glm::qualifier Q>
	bool  Deserializer::_Deserialize (INOUT TVec<T,I,Q> &vec) __NE___
	{
		return stream.Read( OUT &vec.x, SizeOf<T>*I );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT Rectangle<T> &rect) __NE___
	{
		return stream.Read( OUT rect.data(), SizeOf<T>*4 );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT RGBAColor<T> &col) __NE___
	{
		return stream.Read( OUT col.data(), Sizeof(col) );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT HSVColor &col) __NE___
	{
		return stream.Read( OUT col.data(), Sizeof(col) );
	}


	template <usize Size, uint UID, uint Seed>
	bool  Deserializer::_Deserialize (INOUT NamedID<Size, UID, true, Seed> &id) __NE___
	{
		uint	hash = 0;
		bool	res  = stream.Read( OUT hash );

		id = NamedID<Size, UID, true, Seed>{ HashVal32{ hash }};
		return res;
	}


	template <usize Size, uint UID, uint Seed>
	bool  Deserializer::_Deserialize (INOUT NamedID<Size, UID, false, Seed> &id) __NE___
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
	bool  Deserializer::_DeserializeMap (INOUT MapType &map, const uint maxCount) __NE___
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, maxCount );

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
			ASSERT_Eq( map.size(), count );
			return res;
		}
		CATCH_ALL(
			Reconstruct( INOUT map );
			return false;
		)
	}


	template <typename T, typename SetType>
	bool  Deserializer::_DeserializeSet (INOUT SetType &set, const uint maxCount) __NE___
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, maxCount );

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
			ASSERT_Eq( set.size(), count );
			return res;
		}
		CATCH_ALL(
			Reconstruct( INOUT set );
			return false;
		)
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT Optional<T> &value) __NE___
	{
		bool	has_value;
		bool	res;

		if ( _bitCount > 0 or IsPackedBits<T> )
		{
			BoolBit	bit;
			res			= _Deserialize( OUT bit );
			has_value	= bool{bit};
		}
		else
		{
			res = stream.Read( OUT has_value );
		}

		if ( res and has_value )
			return _Deserialize( INOUT value.emplace() );

		return res;
	}


	template <usize I, typename ...Types>
	bool  Deserializer::_RecursiveDeserializeTuple (INOUT Tuple<Types...> &tuple) __NE___
	{
		bool	res = _Deserialize( INOUT tuple.template Get<I>() );

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveDeserializeTuple< I+1 >( INOUT tuple );
		else
			return res;
	}


	template <usize I, usize S, typename ...Types>
	bool  Deserializer::_RecursiveDeserializeTupleArray (INOUT FixedTupleArray<S, Types...> &arr) __NE___
	{
		using T = typename TypeList<Types...>::template Get<I>;

		T*		ptr		= arr.template data<I>();
		bool	res		= true;

		if constexpr( IsTriviallySerializable<T> )
			res = stream.Read( OUT ptr, SizeOf<T> * arr.size() );
		else{
			for (usize i = 0; res and (i < arr.size()); ++i) {
				res = _Deserialize( INOUT ptr[i] );
			}
		}
		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveDeserializeTupleArray< I+1 >( INOUT arr );
		else
			return res;
	}


	template <typename ...Types>
	bool  Deserializer::_Deserialize (INOUT Tuple<Types...> &tuple) __NE___
	{
		return _RecursiveDeserializeTuple<0>( INOUT tuple );
	}


	template <usize S, typename ...Types>
	bool  Deserializer::_Deserialize (INOUT FixedTupleArray<S,Types...> &arr) __NE___
	{
		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, S );

		if_unlikely( (not res) or (count > S) )
			return false;

		arr.resize( count );
		return _RecursiveDeserializeTupleArray<0>( INOUT arr );
	}


	template <typename ...Types>
	bool  Deserializer::_Deserialize (INOUT TupleArrayView<Types...> &arr) __NE___
	{
		CHECK_ERR( allocator );

		uint	count	= 0;
		bool	res		= stream.Read( OUT count );
		ASSERT_LE( count, MaxArrayLength );

		if_unlikely( (not res) or (count > MaxArrayLength) )
			return false;

		arr = TupleArrayView<Types...>{ count };
		if_unlikely( count == 0 )
			return true;

		return _RecursiveDeserializeTupleArrayView<0>( INOUT arr );
	}


	template <usize I, typename ...Types>
	bool  Deserializer::_RecursiveDeserializeTupleArrayView (INOUT TupleArrayView<Types...> &arr) __NE___
	{
		using T = typename TypeList<Types...>::template Get<I>;

		ArrayView<T>	view;
		bool			res = _Deserialize( OUT view );
		arr.template set<I>( view );

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveDeserializeTupleArrayView< I+1 >( INOUT arr );
		else
			return res;
	}


	template <typename V, typename D, typename S>
	bool  Deserializer::_Deserialize (INOUT PhysicalQuantity<V,D,S> &value) __NE___
	{
		return _Deserialize( INOUT value.GetNonScaledRef() );
	}


	template <typename Qt, int I, glm::qualifier Ql>
	bool  Deserializer::_Deserialize (INOUT PhysicalQuantityVec<Qt,I,Ql> &value) __NE___
	{
		return _Deserialize( INOUT value.GetNonScaledRef() );
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT ArrayView<T> &arr) __NE___
	{
		StaticAssert( IsTriviallyDestructible<T> );
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
			res = stream.Read( OUT dst, SizeOf<T> * count );
		else{
			for (uint i = 0; res and (i < count); ++i)
			{
				PlacementNew<T>( OUT dst + i );
				res = _Deserialize( INOUT dst[i] );
			}
		}
		return res;
	}


	template <typename T>
	bool  Deserializer::_Deserialize (INOUT BasicStringView<T> &str) __NE___
	{
		StaticAssert( IsTriviallyDestructible<T> );
		CHECK_ERR( allocator );

		uint	len		= 0;
		bool	res		= stream.Read( OUT len );
		ASSERT_LE( len, MaxStringLength );

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


	inline bool  Deserializer::_Deserialize (INOUT Path &path) __NE___
	{
		String	str;
		bool	res = _Deserialize( OUT str );

		path = Path{ RVRef(str) };
		return res;
	}


	template <typename ...Types>
	bool  Deserializer::_Deserialize (INOUT Union<Types...> &un) __NE___
	{
		uint	idx = 0;
		bool	res = stream.Read( OUT idx );

		if_unlikely( (not res) or (idx >= CountOf<Types...>()) )
			return false;

		return _RecursiveDeserializeUnion<0>( INOUT un, idx );
	}


	template <usize I, typename ...Types>
	bool  Deserializer::_RecursiveDeserializeUnion (INOUT Union<Types...> &un, const usize idx) __NE___
	{
		if_unlikely( idx == I )
		{
			using T = typename TypeList<Types...>::template Get<I>;

			T&	value = un.template emplace<T>();
			ASSERT_Eq( un.index(), idx );

			return _Deserialize( INOUT value );
		}

		if constexpr( I+1 < CountOf<Types...>() )
			return _RecursiveDeserializeUnion< I+1 >( un, idx );
		else
			return false;
	}


} // AE::Serializing
