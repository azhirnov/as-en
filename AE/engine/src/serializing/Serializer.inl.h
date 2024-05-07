// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Serializer.h"

namespace AE::Serializing
{

	template <typename T>
	bool  Serializer::_SerializeObj (const T &obj) __NE___
	{
		if constexpr( IsBaseOf< ISerializable, T >)
		{
			return	_FlushBits() and
					obj.Serialize( *this );
		}
		else
		{
			if ( factory )
			{
				// write 'SerializedID' and then serialize
				return	_FlushBits() and
						factory->Serialize( *this, obj );
			}
			DBG_WARNING( "unknown type" );
			return false;
		}
	}


	template <typename ...Args>
	bool  Serializer::operator () (const Args& ...args) __NE___
	{
		return _RecursiveSerialize( args... );
	}


	template <typename Arg0, typename ...Args>
	bool  Serializer::_RecursiveSerialize (const Arg0 &arg0, const Args& ...args) __NE___
	{
		#if AE_DEBUG_SERIALIZER
			ASSERT( stream.Write( _dbgCounter ));
			++_dbgCounter;
		#endif

		bool	res = _Serialize( arg0 );

		if constexpr( CountOf<Args...>() > 0 )
			return res and _RecursiveSerialize( args... );
		else
			return res;
	}


	template <typename T>
	bool  Serializer::_Serialize (const T &value) __NE___
	{
		if constexpr( IsTriviallySerializable<T> )
			return _FlushBits() and stream.Write( value );
		else
			return _SerializeObj( value );
	}


	inline bool  Serializer::_FlushBits () __NE___
	{
		bool	res = true;

		if_unlikely( _bitCount > 0 )
		{
			DEBUG_ONLY(
				dbgUnusedBits += CT_SizeOfInBits<BitType> - _bitCount;
			)
			res = stream.Write( _packedBits );

			_bitCount	= 0;
			_packedBits	= 0;
		}
		return res;
	}


	template <uint F, uint B, typename T>
	bool  Serializer::_Serialize (const PackedBits<F,B,T> &value) __NE___
	{
		const uint		bit_cnt = value.BitCount();
		const BitType	bits	= value.ToBits();
		bool			res		= true;

		_packedBits |= (bits << _bitCount);
		_bitCount	+= bit_cnt;

		if_unlikely( _bitCount >= CT_SizeOfInBits<BitType> )
		{
			res = stream.Write( _packedBits );

			_bitCount	&= (CT_SizeOfInBits<BitType> - 1);
			_packedBits	= (bits >> (bit_cnt - _bitCount));
		}
		return res;
	}


	template <typename F, typename S>
	bool  Serializer::_Serialize (const Pair<F,S> &value) __NE___
	{
		return _Serialize( value.first ) and _Serialize( value.second );
	}


	template <usize N>
	bool  Serializer::_Serialize (const BitSet<N> &value) __NE___
	{
		StaticAssert( N <= 64 );
		return _Serialize( PackedBits<0,N,ulong>{ value.to_ullong() });
	}


	template <typename T>
	bool  Serializer::_Serialize (ArrayView<T> arr) __NE___
	{
		CHECK_ERR( arr.size() <= MaxArrayLength );

		bool	res =	_FlushBits() and
						stream.Write( CheckCast<uint>(arr.size()) );

		if constexpr( IsTriviallySerializable<T> )
			return res and (arr.empty() or stream.Write( arr.data(), SizeOf<T> * arr.size() ));
		else
		{
			for (usize i = 0; (i < arr.size()) and res; ++i) {
				res = _Serialize( arr[i] );
			}
			return res;
		}
	}


	template <typename T>
	bool  Serializer::_Serialize (BasicStringView<T> str) __NE___
	{
		CHECK_ERR( str.length() <= MaxStringLength );
		return	_FlushBits()									and
				stream.Write( CheckCast<uint>(str.length()) )	and
				(str.empty() or stream.Write( str.data(), StringSizeOf(str) ));
	}


	template <typename T, int I, glm::qualifier Q>
	bool  Serializer::_Serialize (const TVec<T,I,Q> &vec) __NE___
	{
		return	_FlushBits() and
				stream.Write( &vec.x, SizeOf<T>*I );
	}


	template <typename T>
	bool  Serializer::_Serialize (const Rectangle<T> &rect) __NE___
	{
		return	_FlushBits() and
				stream.Write( rect.data(), SizeOf<T>*4 );
	}


	template <typename T>
	bool  Serializer::_Serialize (const RGBAColor<T> &col) __NE___
	{
		return	_FlushBits() and
				stream.Write( col.data(), Sizeof(col) );
	}


	template <typename T>
	bool  Serializer::_Serialize (const HSVColor &col) __NE___
	{
		return	_FlushBits() and
				stream.Write( col.data(), Sizeof(col) );
	}


	template <usize Size, uint UID, uint Seed>
	bool  Serializer::_Serialize (const NamedID<Size, UID, true, Seed> &id) __NE___
	{
		return	_FlushBits() and
				stream.Write( uint{id.GetHash32()} );
	}


	template <usize Size, uint UID, uint Seed>
	bool  Serializer::_Serialize (const NamedID<Size, UID, false, Seed> &id) __NE___
	{
	#if AE_SERIALIZE_HASH_ONLY
		return _FlushBits() and stream.Write( uint{id.GetHash32()} );
	#else
		return _Serialize( id.GetName() );
	#endif
	}


	template <typename K, typename V, typename MapType>
	bool  Serializer::_SerializeMap (const MapType &map) __NE___
	{
		CHECK_ERR( map.size() <= MaxArrayLength );

		bool	res =	_FlushBits() and
						stream.Write( CheckCast<uint>(map.size()) );

		for (auto iter = map.begin(); (iter != map.end()) and res; ++iter)
		{
			res = (_Serialize( iter->first ) and _Serialize( iter->second ));
		}
		return res;
	}


	template <typename T, typename SetType>
	bool  Serializer::_SerializeSet (const SetType &set) __NE___
	{
		CHECK_ERR( set.size() <= MaxArrayLength );

		bool	res =	_FlushBits() and
						stream.Write( CheckCast<uint>(set.size()) );

		for (auto iter = set.begin(); (iter != set.end()) and res; ++iter)
		{
			res = _Serialize( *iter );
		}
		return res;
	}


	template <usize I, typename ...Types>
	bool  Serializer::_RecursiveSerializeTuple (const Tuple<Types...> &tuple) __NE___
	{
		bool	res = _Serialize( tuple.template Get<I>() );

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveSerializeTuple< I+1 >( tuple );
		else
			return res;
	}


	template <typename ...Types>
	bool  Serializer::_Serialize (const Tuple<Types...> &tuple) __NE___
	{
		return _RecursiveSerializeTuple<0>( tuple );
	}


	template <usize I, usize S, typename ...Types>
	bool  Serializer::_RecursiveSerializeTupleArray (const FixedTupleArray<S, Types...> &arr) __NE___
	{
		auto*	ptr	= arr.template data<I>();
		bool	res	= true;

		for (usize i = 0; (i < arr.size()) and res; ++i) {
			res = _Serialize( ptr[i] );
		}

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveSerializeTupleArray< I+1 >( arr );
		else
			return res;
	}


	template <usize S, typename ...Types>
	bool  Serializer::_Serialize (const FixedTupleArray<S,Types...> &arr) __NE___
	{
		CHECK_ERR( arr.size() <= MaxArrayLength );
		return	_FlushBits()								and
				stream.Write( CheckCast<uint>(arr.size()) )	and
				_RecursiveSerializeTupleArray<0>( arr );
	}


	template <usize I, typename ...Types>
	bool  Serializer::_RecursiveSerializeTupleArrayView (const TupleArrayView<Types...> &arr) __NE___
	{
		auto	view	= arr.template get<I>();
		bool	res		= _Serialize( view );

		if constexpr( I+1 < CountOf<Types...>() )
			return res and _RecursiveSerializeTupleArrayView< I+1 >( arr );
		else
			return res;
	}


	template <typename ...Types>
	bool  Serializer::_Serialize (const TupleArrayView<Types...> &arr) __NE___
	{
		CHECK_ERR( arr.size() <= MaxArrayLength );

		if_unlikely( not (_FlushBits() and stream.Write( uint(arr.size()) )) )
			return false;

		if ( arr.empty() )
			return true;

		return _RecursiveSerializeTupleArrayView<0>( arr );
	}


	template <typename T>
	bool  Serializer::_Serialize (const Optional<T> &value) __NE___
	{
		bool	res;
		if ( _bitCount > 0 or IsPackedBits<T> )
			res = _Serialize( BoolBit{ value.has_value() });
		else
			res = stream.Write( value.has_value() );

		if ( value.has_value() )
			return res and _Serialize( *value );

		return res;
	}


	template <typename ...Types>
	bool  Serializer::_Serialize (const Union<Types...> &un) __NE___
	{
		return	_FlushBits()								and
				stream.Write( CheckCast<uint>(un.index()) ) and	// TODO: use ubyte
				_RecursiveSerializeUnion< Types... >( un );
	}


	template <typename T, typename ...Args, typename ...Types>
	bool  Serializer::_RecursiveSerializeUnion (const Union<Types...> &un) __NE___
	{
		if ( auto* value = UnionGet<T>( un ))
			return _Serialize( *value );

		if constexpr( CountOf<Args...>() > 0 )
			return _RecursiveSerializeUnion< Args... >( un );
		else
			return false;
	}


	template <typename V, typename D, typename S>
	bool  Serializer::_Serialize (const PhysicalQuantity<V,D,S> &value) __NE___
	{
		return _Serialize( value.GetNonScaled() );
	}


	template <typename Qt, int I, glm::qualifier Ql>
	bool  Serializer::_Serialize (const PhysicalQuantityVec<Qt,I,Ql> &value) __NE___
	{
		return _Serialize( typename PhysicalQuantityVec<Qt,I,Ql>::ValVec_t{value} );
	}


	inline bool  Serializer::_Serialize (const Path &path) __NE___
	{
		return _Serialize( ToString( path ));
	}

} // AE::Serializing
