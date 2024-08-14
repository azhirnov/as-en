// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Packed Bits
	//

	template <uint FirstBit_v, uint BitCount_v, typename T>
	struct PackedBits
	{
	// types
	public:
		using BitType	= ulong;
		using Self		= PackedBits< FirstBit_v, BitCount_v, T >;

		StaticAssert( IsTrivial<T> );
		StaticAssert( BitCount_v > 0 );
		StaticAssert( BitCount_v <= CT_SizeOfInBits<T> );
		StaticAssert( BitCount_v <= CT_SizeOfInBits<BitType> );
		StaticAssert( FirstBit_v + BitCount_v <= CT_SizeOfInBits<T> );


	// variables
	private:
		T		_value;


	// methods
	public:
		constexpr PackedBits ()								__NE___	{}
		explicit constexpr PackedBits (T val)				__NE___	: _value{val} {}

		ND_ T*			operator -> ()						__NE___	{ return &_value; }
		ND_ T const*	operator -> ()						C_NE___	{ return &_value; }

		ND_ explicit constexpr operator T ()				C_NE___	{ return _value; }

		ND_ static constexpr uint	BitCount ()				__NE___	{ return BitCount_v; }
		ND_ constexpr BitType		ToBits ()				C_NE___;
			constexpr void			FromBits (BitType bits)	__NE___;
	};

	using BoolBit	= PackedBits< 0, 1, bool >;

	template <uint IgnoreMantissaBits>
	using PackedFloatBits = PackedBits< IgnoreMantissaBits, CT_SizeOfInBits<float> - IgnoreMantissaBits, float >;

	template <uint IgnoreMantissaBits>
	using PackedDoubleBits = PackedBits< IgnoreMantissaBits, CT_SizeOfInBits<double> - IgnoreMantissaBits, double >;


namespace _hidden_
{
	template <typename T>
	struct _IsPackedBits : CT_False {};

	template <uint F, uint B, typename T>
	struct _IsPackedBits< PackedBits<F,B,T> > : CT_True {};
}

	template <typename T>
	static constexpr bool	IsPackedBits = Base::_hidden_::_IsPackedBits<T>::value;

/*
=================================================
	ToBits
=================================================
*/
	template <uint F, uint B, typename T>
	constexpr typename PackedBits<F,B,T>::BitType  PackedBits<F,B,T>::ToBits () C_NE___
	{
		constexpr BitType	mask = ToBitMask<BitType>( B );

		if constexpr( IsUnsignedInteger<T>	or
					  IsBytes<T> )
		{
			return (BitType{_value} >> F) & mask;
		}
		else
		if constexpr( IsSameTypes< T, bool >)
		{
			StaticAssert( F == 0 and B == 1 );
			return BitType{_value};
		}
		else
		if constexpr( IsSameTypes< T, Bool32 >)
		{
			StaticAssert( F == 0 and B == 1 );
			return BitType{_value.ToBit()};
		}
		else
		if constexpr( IsPowerOf2Value<T> )
		{
			StaticAssert( F == 0 );
			return BitType{_value.GetPOT()} & mask;
		}
		else
		{
			return (BitType{BitCast< ToUnsignedInteger<T> >( _value )} >> F) & mask;
		}
	}

/*
=================================================
	FromBits
=================================================
*/
	template <uint F, uint B, typename T>
	constexpr void  PackedBits<F,B,T>::FromBits (BitType bits) __NE___
	{
		bits = (bits & ToBitMask<BitType>( B )) << F;

		if constexpr( IsBytes<T> )
		{
			_value = T{bits};
		}
		else
		if constexpr( IsSameTypes< T, bool > or IsSameTypes< T, Bool32 >)
		{
			StaticAssert( F == 0 and B == 1 );
			_value = T{ bits != 0 };
		}
		else
		if constexpr( IsUnsignedInteger<T> )
		{
			_value = T(bits);
		}
		else
		if constexpr( IsPowerOf2Value<T> )
		{
			StaticAssert( F == 0 );
			_value = T{ PowerOfTwo(bits) };
		}
		else
		{
			_value = BitCast<T>( ToUnsignedInteger<T>(bits) );
		}
	}


} // AE::Base
