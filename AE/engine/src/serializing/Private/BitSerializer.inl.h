// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Public/BitSerializer.h"

namespace AE::Serializing
{

	inline bool  BitSerializer::_FlushBits () __NE___
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


	template <typename ...Args>
	bool  BitSerializer::operator () (const Args& ...args) __NE___
	{
		return _RecursiveSerialize( args... );
	}


	template <typename Arg0, typename ...Args>
	bool  BitSerializer::_RecursiveSerialize (const Arg0 &arg0, const Args& ...args) __NE___
	{
		bool	res = _Serialize( arg0 );

		if constexpr( CountOf<Args...>() > 0 )
			return res and _RecursiveSerialize( args... );
		else
			return res;
	}


	template <uint F, uint B, typename T>
	bool  BitSerializer::_Serialize (const PackedBits<F,B,T> &value) __NE___
	{
		const uint		bit_cnt = value.MaxBitCount();
		const BitType	bits	= value.ToBits();
		bool			res		= true;

		_packedBits |= (bits << _bitCount);
		_bitCount	+= bit_cnt;

		if_unlikely( _bitCount >= CT_SizeOfInBits<BitType> )
		{
			res = stream.Write( _packedBits );

			_bitCount	&= (CT_SizeOfInBits<BitType> - 1);
			_packedBits	= SafeRightBitShift( bits, bit_cnt - _bitCount );
		}
		return res;
	}


} // AE::Serializing
