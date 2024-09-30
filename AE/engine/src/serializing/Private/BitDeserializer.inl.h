// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Public/BitDeserializer.h"

namespace AE::Serializing
{

	template <typename ...Args>
	bool  BitDeserializer::operator () (INOUT Args& ...args) __NE___
	{
		return _RecursiveDeserialize( INOUT args... );
	}


	template <typename Arg0, typename ...Args>
	bool  BitDeserializer::_RecursiveDeserialize (INOUT Arg0 &arg0, INOUT Args& ...args) __NE___
	{
		StaticAssert( not IsConst<Arg0> );

		bool res = _Deserialize( INOUT arg0 );

		if constexpr( CountOf<Args...>() > 0 )
			return res and _RecursiveDeserialize( INOUT args... );
		else
			return res;
	}


	template <uint F, uint B, typename T>
	bool  BitDeserializer::_Deserialize (INOUT PackedBits<F,B,T> &value) __NE___
	{
		const uint	bit_cnt = value.MaxBitCount();
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

			_packedBits	= SafeRightBitShift( _packedBits, bit_cnt - _bitCount );
			_bitCount	+= CT_SizeOfInBits<BitType> - bit_cnt;
		}

		value.FromBits( bits );
		return true;
	}


} // AE::Serializing
